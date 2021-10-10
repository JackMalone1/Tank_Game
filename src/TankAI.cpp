#include "TankAi.h"

////////////////////////////////////////////////////////////
TankAi::TankAi(sf::Texture const& texture, std::vector<sf::Sprite>& wallSprites, std::map<int, std::list<sf::Sprite>>& t_walls)
	: m_aiBehaviour(AiBehaviour::SEEK_PLAYER)
	, m_texture(texture)
	, m_wallSprites(wallSprites)
	, m_steering(0, 0)
	, m_health(5)
	, m_aiState(AIState::PATROL_MAP)
	, m_pool(t_walls)
{
	// Initialises the tank base and turret sprites.
	initSprites();
}



////////////////////////////////////////////////////////////
void TankAi::update(Tank const & playerTank, double dt, std::map<int, std::list<sf::Sprite>>& t_walls)
{
	
	sf::Vector2f vectorToPlayer = seek(playerTank.getPosition());
	sf::Vector2f acceleration; // ADD THIS
	std::pair<bool, bool> result = isPlayerInVisionCone(playerTank.getPosition());
	float angleBetween = acos(thor::dotProduct(vectorToPlayer, m_visionConeLeft[1])
		/ thor::length(vectorToPlayer) * thor::length(m_visionConeLeft[1]));

	float distancecToPlayer = distanceFromPoint(playerTank.getPosition());
	switch (m_aiState)
	{
	case AIState::ATTACK_PLAYER:	
		attackPlayer(vectorToPlayer, acceleration, angleBetween, playerTank,distancecToPlayer);
		shootAtPlayer(playerTank.getPosition());
		break;
	case AIState::PATROL_MAP:
		patrolMap(vectorToPlayer, acceleration, playerTank,distancecToPlayer);
		break;
	default:
		break;
	}


	// Now we need to convert our velocity vector into a rotation angle between 0 and 359 degrees.
	// The m_velocity vector works like this: vector(1,0) is 0 degrees, while vector(0, 1) is 90 degrees.
	// So for example, 223 degrees would be a clockwise offset from 0 degrees (i.e. along x axis).
	// Note: we add 180 degrees below to convert the final angle into a range 0 to 359 instead of -PI to +PI
	auto dest = atan2(-1 * m_velocity.y, -1 * m_velocity.x) / thor::Pi * 180 + 180;

	auto currentRotation = m_rotation;

	// Find the shortest way to rotate towards the player (clockwise or anti-clockwise)
	if (std::round(currentRotation - dest) == 0.0)
	{
		m_steering.x = 0;
		m_steering.y = 0;
	}

	else if ((static_cast<int>(std::round(dest - currentRotation + 360))) % 360 < 180)
	{
		// rotate clockwise
		m_rotation = static_cast<int>((m_rotation)+1) % 360;
	}
	else
	{
		// rotate anti-clockwise
		m_rotation = static_cast<int>((m_rotation)-1) % 360;
	}

	moveOffScreen();
	updateMovement(dt);
	m_pool.update(dt, t_walls, std::pair<sf::Sprite&, sf::Sprite&>(playerTank.getTurret(), playerTank.getBase()));
}

////////////////////////////////////////////////////////////
void TankAi::render(sf::RenderWindow & window)
{
	// TODO: Don't draw if off-screen...
	window.draw(m_tankBase);
	window.draw(m_turret);
	sf::VertexArray visionCone(sf::Lines);
	visionCone.append(sf::Vertex(m_visionConeLeft[0], sf::Color::Red));
	visionCone.append(sf::Vertex(m_visionConeLeft[1], sf::Color::Red));
	visionCone.append(sf::Vertex(m_visionConeRight[0], sf::Color::Red));
	visionCone.append(sf::Vertex(m_visionConeRight[1], sf::Color::Red));
	visionCone.append(sf::Vertex(m_visionConeDir, sf::Color::Red));
	window.draw(visionCone);
	m_pool.draw(window);
}

////////////////////////////////////////////////////////////
void TankAi::init(sf::Vector2f position)
{
	m_tankBase.setPosition(position);
	m_turret.setPosition(position);

	setVisionCone(50.0f, MAX_SEE_AHEAD);

	for (sf::Sprite const wallSprite : m_wallSprites)
	{
		sf::CircleShape circle(wallSprite.getTextureRect().width * 1.5f);
		circle.setOrigin(circle.getRadius(), circle.getRadius());
		circle.setPosition(wallSprite.getPosition());
		m_obstacles.push_back(circle);
	}
}

////////////////////////////////////////////////////////////
sf::Vector2f TankAi::seek(sf::Vector2f playerPosition) const
{
	return playerPosition - m_tankBase.getPosition();
}

////////////////////////////////////////////////////////////
sf::Vector2f TankAi::collisionAvoidance()
{
	auto headingRadians = thor::toRadian(m_rotation);
	sf::Vector2f headingVector(std::cos(headingRadians) * MAX_SEE_AHEAD, std::sin(headingRadians) * MAX_SEE_AHEAD);
	m_ahead = m_tankBase.getPosition() + headingVector;
	m_halfAhead = m_tankBase.getPosition() + (headingVector * 0.5f);
	const sf::CircleShape mostThreatening = findMostThreateningObstacle();
	sf::Vector2f avoidance(0, 0);
	if (mostThreatening.getRadius() != 0.0)
	{		
		avoidance.x = m_ahead.x - mostThreatening.getPosition().x;
		avoidance.y = m_ahead.y - mostThreatening.getPosition().y;
		avoidance = thor::unitVector(avoidance);
		avoidance *= MAX_AVOID_FORCE;
	}
	else
	{
		avoidance *= 0.0f;
	}
	return avoidance;
}

////////////////////////////////////////////////////////////
const sf::CircleShape TankAi::findMostThreateningObstacle()
{
	sf::CircleShape mostThreatening;

	for (auto obstacle : m_obstacles)
	{
		bool collision = MathUtility::lineIntersectsCircle(m_ahead, m_halfAhead, obstacle);
		if (MathUtility::distance(m_tankBase.getPosition(), obstacle.getPosition()) > 0.95 * obstacle.getRadius()
			&& MathUtility::distance(m_tankBase.getPosition(), obstacle.getPosition()) > 1.05)
		{
			continue;
		}
		// "position" is the character's current position
		if (collision && (mostThreatening.getRadius() == 0 || MathUtility::distance(m_tankBase.getPosition(),
			obstacle.getPosition()) <
			MathUtility::distance(m_tankBase.getPosition(), mostThreatening.getPosition())))
		{
			mostThreatening = obstacle;
		}
	}

	return mostThreatening;
}

bool TankAi::circleInterectsLine(sf::Vector2f t_ahead, sf::Vector2f t_halfAhead, sf::CircleShape t_circle)
{
	return distance(t_ahead, t_circle) <= t_circle.getRadius() || distance(t_ahead, t_circle) <= t_circle.getRadius();
}

/// <summary>
/// distance from a point to an obstacle
/// </summary>
/// <param name="t_position"></param>
/// <param name="t_obstacle"></param>
/// <returns></returns>
float TankAi::distance(sf::Vector2f t_position, sf::CircleShape t_obstacle)
{
	return sqrt(pow(t_position.x - t_obstacle.getPosition().x, 2) + pow(t_position.y - t_obstacle.getPosition().y, 2));
}

/// <summary>
/// checks to see if the ai is currently colliding with the player
/// </summary>
/// <param name="playerTank">player tank</param>
/// <returns>returns true if the player is colliding with the player, otherwise false</returns>
bool TankAi::collidesWithPlayer(Tank const& playerTank) const
{
	// Checks if the AI tank has collided with the player tank.
	if (CollisionDetector::collision(m_turret, playerTank.getTurret()) ||
		CollisionDetector::collision(m_tankBase, playerTank.getBase()))
	{
		return true;
	}
	return false;
}

/// <summary>
/// makes sure that the health doesn't go beneath 0
/// </summary>
/// <param name="t_damage"></param>
void TankAi::takeDamage(int t_damage)
{
	(t_damage < m_health) ? m_health -= t_damage : m_health = 0;
}

/// <summary>
/// checks to see if a point is right of the vision cone
/// </summary>
/// <param name="t_visionCone"></param>
/// <param name="t_playerPos"></param>
/// <returns></returns>
bool TankAi::isPointRight(std::array<sf::Vector2f, 2> t_visionCone, sf::Vector2f t_playerPos)
{
	return (t_visionCone[1].x - t_visionCone[0].x)
		* (t_playerPos.y - t_visionCone[0].y)
		- (t_visionCone[1].y - t_visionCone[0].y)
		* (t_playerPos.x - t_visionCone[0].x)
	> 0;
}

/// <summary>
/// rotates the tank
/// </summary>
/// <param name="t_angle"></param>
void TankAi::rotate(float t_angle)
{
	m_turret.rotate(t_angle);
	m_tankBase.rotate(t_angle);
	thor::rotate(m_visionConeLeft[1], t_angle);
	thor::rotate(m_visionConeRight[1], t_angle);
}

void TankAi::setVisionCone(float t_angle, float const MAX_SEE_AHEAD)
{
	m_visionConeLeft[0] = m_turret.getPosition();
	// Rotate the vision cone vector left of centre by the given angle.
	thor::rotate(m_visionConeDir, -t_angle);

	// Note carefully how we scale the vision cone vector to get the end point of the line
	m_visionConeLeft[1] = sf::Vector2f(m_visionConeLeft[0].x + (m_visionConeDir.x * MAX_SEE_AHEAD),
		m_visionConeLeft[0].y + (m_visionConeDir.y * MAX_SEE_AHEAD));

	m_visionConeRight[0] = m_turret.getPosition();
	// vision cone vector is current left of centre by -t_angle degrees, so to get it pointing to right of
	//  centre by t_angle degrees, we use (t_angle * 2)
	thor::rotate(m_visionConeDir, (t_angle * 2));

	m_visionConeRight[1] = sf::Vector2f(m_visionConeRight[0].x + (m_visionConeDir.x * MAX_SEE_AHEAD),
		m_visionConeRight[0].y + (m_visionConeDir.y * MAX_SEE_AHEAD));
	// Restore the vision cone vector to centre
	thor::rotate(m_visionConeDir, -t_angle);

}

/// <summary>
/// ai will try to keep the player inside of the vision cone by rotating either left or right depending on which direction that the player is moving
/// if the player moves outside of the vision cone then the ai will move back to the patrol map mode to try and find the player again
/// </summary>
/// <param name="t_vectorToPlayer"></param>
/// <param name="t_acceleration"></param>
/// <param name="t_angle"></param>
/// <param name="t_result"></param>
void TankAi::attackPlayer(sf::Vector2f t_vectorToPlayer,
	sf::Vector2f& t_acceleration,
	float t_angle,
	Tank const& t_player,
	float t_distToPlayer)
{
	m_steering += thor::unitVector(t_vectorToPlayer);
	m_steering += collisionAvoidance();
	m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
	t_acceleration = m_steering / MASS;  // ADD THIS
	m_velocity = MathUtility::truncate(m_velocity + t_acceleration, MAX_SPEED);
	if (t_angle < 180)
	{
		thor::rotate(m_visionConeDir, 0.5f);
		//m_rotation += 1.0f;
		setVisionCone(m_ATTACK_PLAYER_ANGLE, MAX_SEE_AHEAD * 0.75);
	}
	else
	{
		thor::rotate(m_visionConeDir, -0.5f);
		//m_rotation += -1.0f;
		setVisionCone(m_ATTACK_PLAYER_ANGLE, MAX_SEE_AHEAD * 0.75);
	}

	std::pair<bool, bool> result = isPlayerInVisionCone(t_player.getPosition());
	if ((result.first && result.second) || t_distToPlayer > (MAX_SEE_AHEAD))
	{
		std::cout << "set to patrol map\n";
		m_aiState = AIState::PATROL_MAP;
		setVisionCone(m_PATROL_MAP_ANGLE, MAX_SEE_AHEAD);
	}
	
}

/// <summary>
/// ai will move around the map and look for the player
/// if the ai finds the player inside of the vision cone then it will switch to the attack player mode and try to fire a bullet at the player
/// if it decides to switch to attack player mode then it will also change the size of the vision cone
/// </summary>
/// <param name="t_vectorToPlayer"></param>
/// <param name="t_acceleration"></param>
/// <param name="t_result"></param>
void TankAi::patrolMap(sf::Vector2f t_vectorToPlayer,
	sf::Vector2f& t_acceleration,
	Tank const& t_player,
	float t_distToPlayer)
{
	thor::rotate(m_visionConeDir, 0.5f);
	setVisionCone(m_PATROL_MAP_ANGLE, MAX_SEE_AHEAD);
	t_vectorToPlayer = seek(sf::Vector2f(ScreenSize::s_width / 2.0, ScreenSize::s_height / 2.0));
	m_steering += thor::unitVector(t_vectorToPlayer);
	m_steering += collisionAvoidance();
	m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
	t_acceleration = m_steering / MASS;  // ADD THIS
	m_velocity = MathUtility::truncate(m_velocity + t_acceleration, MAX_SPEED);

	std::pair<bool, bool> result = isPlayerInVisionCone(t_player.getPosition());
	if (!(result.first && result.second) && t_distToPlayer <= (MAX_SEE_AHEAD))
	{
		std::cout << "set to attack player\n";
		m_aiState = AIState::ATTACK_PLAYER;

		if (result.first)
		{
			thor::rotate(m_visionConeDir, -25.0f);
		}
		else
		{
			thor::rotate(m_visionConeDir, 25.0f);
		}
		setVisionCone(m_ATTACK_PLAYER_ANGLE, MAX_SEE_AHEAD * 0.75);
	}
}

/// <summary>
/// only fies at the player if they are close enough to them
/// </summary>
/// <param name="t_playerPos">position of the player</param>
void TankAi::shootAtPlayer(sf::Vector2f t_playerPos)
{
	float dist = distanceFromPoint(t_playerPos);
	if (dist < 50)
	{
		fireBullets();
	}
}

/// <summary>
/// checks the distance the tank is from a given point
/// </summary>
/// <param name="t_point"></param>
/// <returns></returns>
float TankAi::distanceFromPoint(sf::Vector2f t_point)
{
	return sqrt(pow(t_point.x - m_tankBase.getPosition().x, 2) + pow(t_point.y - m_tankBase.getPosition().y, 2));
}

void TankAi::moveOffScreen()
{
	if (m_tankBase.getPosition().x + m_tankBase.getGlobalBounds().width / 2.0 > ScreenSize::s_width
		|| m_tankBase.getPosition().x - m_tankBase.getGlobalBounds().width / 2.0 < 0 ||
		m_tankBase.getPosition().y + m_tankBase.getGlobalBounds().height / 2.0 > ScreenSize::s_height
		|| m_tankBase.getPosition().y - m_tankBase.getGlobalBounds().height / 2.0 < 0)
	{
		m_velocity = sf::Vector2f(0.0f,0.0f);
	}
	if (m_turret.getPosition().x + m_turret.getGlobalBounds().width / 2.0 > ScreenSize::s_width
		|| m_turret.getPosition().x - m_turret.getGlobalBounds().width / 2.0 < 0 ||
		m_turret.getPosition().y + m_turret.getGlobalBounds().height / 2.0 > ScreenSize::s_height
		|| m_turret.getPosition().y - m_turret.getGlobalBounds().height / 2.0 < 0)
	{
		m_velocity = sf::Vector2f(0.0f, 0.0f);
	}
}

/// <summary>
/// checks to see if the player is inside the vision cone of the ai
/// </summary>
/// <param name="t_playerPos">position of the player</param>
/// <returns>returns whether the player is left or right of both sides of the vision cone</returns>
std::pair<bool, bool> TankAi::isPlayerInVisionCone(sf::Vector2f t_playerPos)
{
	bool isPlayerLeft = false;
	bool isPlayerRight = false;
	if (!isPointRight(m_visionConeRight, t_playerPos))
	{
		isPlayerRight = true;
	}
	if (isPointRight(m_visionConeLeft, t_playerPos))
	{
		isPlayerLeft = true;
	}
	return std::pair<bool, bool>(isPlayerLeft, isPlayerRight);
}

/// <summary>
/// checks the time since the last time that the ai fired to see if it is able to fire again
/// </summary>
/// <returns>returns true if it is able to fire, otherwise it will return false</returns>
bool TankAi::checkReadyToFire()
{
	if (m_timeWaitedToFire >= m_FIRE_RATE)
	{
		m_timeWaitedToFire = 0;
		return true;
	}
	else
	{
		m_timeWaitedToFire++;
		return false;
	}
}

/// <summary>
/// fires a bullet if there has been enough time that has passed since the last time that it fired a bullet
/// it will only shoot at the player if they are in the vision cone of the ai
/// </summary>
void TankAi::fireBullets()
{
	if (checkReadyToFire())
	{
		if (AIState::ATTACK_PLAYER == m_aiState)
		{
			Projectile* p = nullptr;
			p = m_pool.create();

			if (nullptr != p)
			{
				p->init(m_tankBase.getPosition().x, m_tankBase.getPosition().y, m_rotation);
			}
		}
	}
}

/// <summary>
/// checks to see if any of the bullets that the ai has fired has hit the player tank or not
/// if they do manage to hit the player tank then the player will lose some health and then they will receive some random status effect too
/// </summary>
/// <param name="t_tank">player tank</param>
/// <returns>returns true if they hit the player. Doesnt do multiple hits to make it a bit more fair for the player</returns>
bool TankAi::hitTarget(const std::pair<sf::Sprite, sf::Sprite> t_tank) const
{
	return m_pool.hitTarget(t_tank.first) || m_pool.hitTarget(t_tank.second);
}

////////////////////////////////////////////////////////////
/// <summary>
/// initialises the sprites for the base and the turret
/// </summary>
void TankAi::initSprites()
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(103, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(122, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);

	m_pool.init("./resources/images/SpriteSheet.png");//initialise the bullets
}


////////////////////////////////////////////////////////////
/// <summary>
/// gets a new position for the tank based on its speed and rotation
/// also takes into account the time since the last update
/// it will then make sure that the tank base, turret and vision cone have all been rotated corectly and given the correct position
/// </summary>
/// <param name="dt">time since the last update</param>
void TankAi::updateMovement(double dt)
{
	double speed = thor::length(m_velocity);
	sf::Vector2f newPos(m_tankBase.getPosition().x + std::cos(MathUtility::DEG_TO_RAD  * m_rotation)
		* speed * (dt / 1000),
		m_tankBase.getPosition().y + std::sin(MathUtility::DEG_TO_RAD  * m_rotation)
		* speed * (dt / 1000));
	m_tankBase.setPosition(newPos.x, newPos.y);
	m_tankBase.setRotation(static_cast<float>(m_rotation));
	m_turret.setPosition(m_tankBase.getPosition());
	m_turret.setRotation(static_cast<float>(m_rotation));

	m_visionConeLeft[0] = m_turret.getPosition();
	m_visionConeRight[0] = m_turret.getPosition();
}