#include "Tank.h"

Tank::Tank(sf::Texture const & texture, std::map<int, std::list<sf::Sprite>>& t_walls)
: m_texture(texture)
, m_walls(t_walls)
, m_pool(t_walls)
, m_health(10)
, m_damageList("ariblk.ttf", "Types of damage taken")
{
	initSprites();//intialises the sprites for the tank
	srand(time(NULL));//seeds the random function so it can be used
	int corner = (rand() % 4) + 1;//picks a random corner of the sccreen for the tank to start in
	//sets the position of the tank to a random corner of the screen depending on the output of the previouos line
	switch (corner)
	{
	case 1:
		m_tankBase.setPosition(0 + m_tankBase.getGlobalBounds().width,
			0 + m_tankBase.getGlobalBounds().height);		//top left
		break;
	case 2:
		m_tankBase.setPosition(SCREEN_WIDTH - m_tankBase.getGlobalBounds().width,
			                   SCREEN_HEIGHT - m_tankBase.getGlobalBounds().height); // bottom right
		break;
	case 3:
		m_tankBase.setPosition(0 + m_tankBase.getGlobalBounds().width,
			                   SCREEN_HEIGHT - m_tankBase.getGlobalBounds().height); //bottom left
		break;
	case 4:
		m_tankBase.setPosition(SCREEN_WIDTH - m_tankBase.getGlobalBounds().width,
			                   0 + m_tankBase.getGlobalBounds().height); // top right
		break;
	}
	m_turret.setPosition(m_tankBase.getPosition());//sets the position of the tank
}

void Tank::update(double dt, std::map<int, std::list<sf::Sprite>>& t_walls, TankAi& t_tank)
{	
	handleKeyInput();
	
	updatePosition(dt);

	//if the tank isnt colliding with a wall then the player can rotate the tank
	if(m_enableRotation)
	{
		m_tankBase.setRotation(m_rotation);
		m_turret.setRotation(m_rotation);
		m_turret.setRotation(m_turretRotation);
	}
	//makes sure that the player's speed is between the current min speed and max speed
	m_speed = std::clamp(m_speed, -m_currentMaxSpeed, m_currentMaxSpeed);

	/// <summary>
	/// applies either a positive or negative friction to the speed depending if the speed is greater than or less than zero
	/// once the speed gets close enough to zero it is just set to zero or it will never get to zero 
	/// </summary>
	applyFriction();

	/// <summary>
	/// checks to see if the tank is trying to move off of the screen and if it is then it will set the speed to 0
	/// </summary>
	moveOffScreen();

	//check for wall collisions and move off if you're colliding
	if (checkWallCollision())
	{
		deflect();
	}
	else
	{
		m_enableRotation = true;//lets player rotate if not colliding
	}
	std::pair<sf::Sprite&, sf::Sprite&> tank(t_tank.getBase(), t_tank.getTurret());
	int hitCount = m_pool.update(dt, t_walls, tank);//updates all bullets that are currently being used

	/// <summary>
	/// checks to see if the player has hit the ai tank or not and if it has then it will deal damage
	/// it checks if the hitcount is between 0 and 100 just it doesnt try to take away a negative value or more health than the ai actually has
	/// overall
	/// </summary>
	if (hitCount > 0 && hitCount < 100)
	{
		t_tank.takeDamage(hitCount);
		for (int i = 0; i < hitCount; i++)
		{
			increaseTargetsHit();
		}
	}
}

/// <summary>
/// draws the base, turret and all of the bullets
/// </summary>
/// <param name="window"></param>
void Tank::render(sf::RenderWindow & window) 
{
	//draws the tank
	window.draw(m_tankBase);
	window.draw(m_turret);

	//draws any bullets that are being used
	m_pool.draw(window);
}

void Tank::setPosition(sf::Vector2f t_position)
{
	//sets the previous position that the tank was in and then sets the sprites for both the turret and the base of the tank
	m_previousPosition = t_position;
	m_tankBase.setPosition(t_position);
	m_turret.setPosition(t_position);
}

////////////////////////////////////////////////////////////
sf::Vector2f Tank::getPosition() const
{
	return m_tankBase.getPosition();
}

/// <summary>
/// sets the previous speed and then increases the current speed if it is less than the maximum speed
/// </summary>
void Tank::increaseSpeed()
{
	m_previousSpeed = m_speed;
	if (m_speed < m_currentMaxSpeed)
	{
		m_speed += m_acceleration;
	}
}

/// <summary>
/// sets the previous speed and then decreases the current speed if it is greater than the minimum speed
/// </summary>
void Tank::decreaseSpeed()
{
	m_previousSpeed = m_speed;
	if (m_speed > -m_currentMaxSpeed)
	{
		m_speed -= m_brakeSpeed;
	}
}

/// <summary>
/// increases rotation
/// loops back to zero after 360 because the rotation will be the same as if it wasnt reset but helps keep the number low
/// </summary>
void Tank::increaseRotation()
{
	m_previousRotation = m_rotation;
	m_rotation += m_rotationAngle;
	if (m_rotation == 360.0)
	{
		m_rotation = 0.0;
	}
}

/// <summary>
/// decreases the rotation but the rotation is always kept in the range zero to 360 because once it goes below zero it will be the same
/// as 360
/// </summary>
void Tank::decreaseRotation()
{
	m_previousRotation = m_rotation;
	m_rotation -= m_rotationAngle;
	if (m_rotation == 0.0)
	{
		m_rotation = 359.0;
	}
}

/// <summary>
/// returns if the tank is collidding with any of the walls
/// it does this by figuring out the 4 cells that the tank is currently in and then it only checks for collisions with walls in any of these 4
/// cells
/// it does this so that it doesnt have to check for collisions with walls that it isnt close to
/// </summary>
/// <returns></returns>
bool Tank::checkWallCollision()
{
	int cellId = 0;
	int cellHeight = ScreenSize::s_height / m_NUM_COLUMNS;
	int cellWidth = ScreenSize::s_width / m_NUM_COLUMNS;

	cellId = floor(m_tankBase.getPosition().x / cellWidth) + (floor(m_tankBase.getPosition().y / cellHeight) * m_NUM_COLUMNS);
	if (checkCollisions(m_walls[cellId]))
	{
		return true;
	}
	cellId = floor(m_tankBase.getPosition().x + m_tankBase.getGlobalBounds().width / cellWidth)
		           + (floor(m_tankBase.getPosition().y / cellHeight) * m_NUM_COLUMNS);
	if (checkCollisions(m_walls[cellId]))
	{
		return true;
	}
	cellId = floor(m_tankBase.getPosition().x / cellWidth)
		     + (floor(m_tankBase.getPosition().y + m_tankBase.getGlobalBounds().height / cellHeight) * m_NUM_COLUMNS);
	if (checkCollisions(m_walls[cellId]))
	{
		return true;
	}
	cellId = floor(m_tankBase.getPosition().x + m_tankBase.getGlobalBounds().width / cellWidth)
		+ (floor(m_tankBase.getPosition().y + m_tankBase.getGlobalBounds().height / cellHeight) * m_NUM_COLUMNS);
	if (checkCollisions(m_walls[cellId]))
	{
		return true;
	}
	return false;
}

/// <summary>
/// checks for the collisions between the tank and the walls in the game
/// </summary>
/// <param name="t_walls">list of the walls in the current level</param>
/// <returns>returns true if there is a collision, otherwise false</returns>
bool Tank::checkCollisions(std::list<sf::Sprite>& t_walls)
{
	for (sf::Sprite const& sprite : t_walls)
	{
		//checks if either the tank base or turret has collided witht the current
		//wall sprite.
		if (CollisionDetector::collision(m_turret, sprite) ||
			CollisionDetector::collision(m_tankBase, sprite))
		{
			return true;
		}
	}
	return false;
}

/// <summary>
/// checks to see if the tank is trying to move off of the screen and if it is then it will set the speed to 0
/// </summary>
void Tank::moveOffScreen()
{
	if (m_tankBase.getPosition().x + m_tankBase.getGlobalBounds().width / 2.0 > ScreenSize::s_width
		|| m_tankBase.getPosition().x - m_tankBase.getGlobalBounds().width / 2.0 < 0 ||
		m_tankBase.getPosition().y + m_tankBase.getGlobalBounds().height / 2.0 > ScreenSize::s_height
		|| m_tankBase.getPosition().y - m_tankBase.getGlobalBounds().height / 2.0 < 0)
	{
		m_speed = 0;
	}
	if (m_turret.getPosition().x + m_turret.getGlobalBounds().width / 2.0 > ScreenSize::s_width
		|| m_turret.getPosition().x - m_turret.getGlobalBounds().width / 2.0 < 0 ||
		m_turret.getPosition().y + m_turret.getGlobalBounds().height / 2.0 > ScreenSize::s_height
		|| m_turret.getPosition().y - m_turret.getGlobalBounds().height / 2.0 < 0)
	{
		m_speed = 0;
	}
}

/// <summary>
/// applies either a positive or negative friction to the speed depending if the speed is greater than or less than zero
/// once the speed gets close enough to zero it is just set to zero or it will never get to zero 
/// </summary>
void Tank::applyFriction()
{
	if (m_speed > m_FRICTION)
	{
		m_speed -= m_FRICTION;
	}
	else if (m_speed < -m_FRICTION)
	{
		m_speed += m_FRICTION;
	}
	if (m_speed <= m_FRICTION && m_speed >= -m_FRICTION)
	{
		m_speed = 0;
	}
}

void Tank::updatePosition(double dt)
{
	//updates the position of the tank by using its current position and the direction that it is currently facing to move
	//moves in a straight line in the direction that it is currently facing
	//the current speed of the tank is used to move the tank by however much the player wants
	// and the (dt / 1000) is used to account for the time elapsed to make sure that it doesn't move too much per frame
	double oldX = m_tankBase.getPosition().x;
	double oldY = m_tankBase.getPosition().y;
	double newX = oldX + cos(m_rotation * MathUtility::DEG_TO_RAD) * m_speed * (dt / 1000);
	double newY = oldY + sin(m_rotation * MathUtility::DEG_TO_RAD) * m_speed * (dt / 1000);

	//moves the sprite to its new position
	setPosition(sf::Vector2f(newX, newY));
}

/// <summary>
/// tries to deflect the tank if it is colliding with a tank
/// </summary>
void Tank::deflect()
{
	//in case tank was rotating.
	adjustRotation();

	//if tank was moving.
	if (m_speed != 0)
	{
		//temporarily disable turret rotations on collision.
		m_enableRotation = false;
		//back up to oposition in previous frame
		m_tankBase.setPosition(m_previousPosition);
		//apply small force in opposite direction of travel.
		if (m_previousSpeed < 0)
		{
			m_speed = 15;
		}
		else
		{
			m_speed = -15;
		}
	}
}

/// <summary>
/// returns if the timer has reached the time for the player to be able to fire again
/// if they are not able to fire it will increase the counter
/// otherwise it will just decrease the counter again and start counting again
/// </summary>
/// <returns></returns>
bool Tank::checkReadyToFire()
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
/// once the player is able to fire a bullet after the countdown has finished it will create a neww bullet in the bullet pool
/// it will do this by looking through the bullet pool and returns a pointer to the firrst bullet available if there is one
/// if there are no free bullets then it will return a null pointer
/// if it did not return a null pointer it will go initialise the bullet
/// </summary>
void Tank::fireBullets()
{
	if (checkReadyToFire())
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			Projectile* p = nullptr;
			p = m_pool.create();

			if (nullptr != p)
			{
				p->init(tankBase().getPosition().x,tankBase().getPosition().y, m_turretRotation);
			}
			m_numBulletsFired++;
		}
	}
}

/// <summary>
/// checks if any of the bullets in use are colliding with any of the walls
/// </summary>
/// <param name="t_sprite">returns true if the bullet hit the wall</param>
/// <returns></returns>
bool Tank::hitTarget(const sf::Sprite& t_sprite) const
{
	return m_pool.hitTarget(t_sprite);
}

/// <summary>
/// returns their accuracy to the nearest percent
/// </summary>
/// <returns></returns>
int Tank::getAccuracyPercentage()
{
	if (m_numBulletsFired > 0)
	{
 		return static_cast<int>((static_cast<double>(m_numBulletsHit) / m_numBulletsFired) * 100); 
	}
	else
	{
		return 0;
	}
}

/// <summary>
/// resets the accuracy for the player whenever they start a new game so that it doesn't give them an incorrect accuracy
/// </summary>
void Tank::resetAccuracy()
{
	m_numBulletsFired = 0;
	m_numBulletsHit = 0;
}

/// <summary>
/// sets back to zero for whenever the player is starting a new level
/// </summary>
void Tank::resetScore()
{
	m_score = 0;
}

/// <summary>
/// gets the current score of the player
/// </summary>
/// <returns></returns>
int Tank::getScore()
{
	return m_score;
}

/// <summary>
/// sets the score of the player
/// the player will get a bonus to their score based on however much time the current target had left on the screen
/// </summary>
/// <param name="t_score"></param>
void Tank::setScore(int t_score)
{
	m_score = t_score;
}

/// <summary>
/// reduces the health of the tank and then it will apply a random side effect from getting hit, such as reducing the top speed of the tank
/// if it reduces the top speed it will clamp the speed in case the player was travelling at the previous max speed as this might cause errors
/// later if it was left at the old max speed
/// </summary>
void Tank::takeDamage()
{
	m_health--;

	int random = (rand() % 4) + 1;

	switch (random)
	{
	case 1:
		if (m_currentMaxSpeed > m_MAX_SPEED / 2)
		{
			m_currentMaxSpeed -= 5;
		}
		m_speed = std::clamp(m_speed, -m_currentMaxSpeed, m_currentMaxSpeed);
		m_damageList.appendList(DamageType::TopSpeed, "Top Speed Reduced");
		break;
	case 2:
		if (m_brakeSpeed > 0.5)
		{
			m_brakeSpeed -= 0.1;
		}
		m_damageList.appendList(DamageType::BrakeSpeed, "Brake speed reduced");
		break;
	case 3:
		if (m_acceleration > 0.5)
		{
			m_acceleration -= 0.1;
		}
		m_damageList.appendList(DamageType::Acceleration, "acceleration speed reduced");
		break;
	case 4:
		if (m_rotationAngle > 0.5)
		{
			m_rotationAngle -= 0.1;
		}
		m_damageList.appendList(DamageType::TurnRadius, "turn radius increased");
		break;
	}
}

void Tank::resetDamage()
{
	m_brakeSpeed = m_DEFAULT_BRAKE_SPEED;
	m_acceleration = m_DEFAULT_ACCELERATION_SPEED;
	m_rotationAngle = m_DEFAULT_ROTATION_ANGLE;
}


/// <summary>
/// loads in the sprite for the tank base and the turret and also sets up the sprites for the bullets
/// </summary>
void Tank::initSprites()
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(2, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(19, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);

	m_pool.init("./resources/images/SpriteSheet.png");//initialise the bullets
}

/// <summary>
/// checks for all of the input from the player
/// </summary>
void Tank::handleKeyInput()
{
	//increase speed
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		increaseSpeed();
	}
	//decrerase speed
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		decreaseSpeed();
	}
	//rotate to the left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		decreaseRotation();
		decreaseTurretRotation();
	}
	//rotate to the right 
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		increaseRotation();
		increaseTurretRotation();
	}
	//rotate only the turret to the left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		decreaseTurretRotation();
	}
	//rotate only the turret to the left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
	{
		increaseTurretRotation();
	}
	//alligns the turret back to the rotation of the base of the tank
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		centreTurret();
	}


}

/// <summary>
/// increases the rotation of the tirret
/// loops back once it reaches 360 because values above 360 will be the same as if you just keep subtracting 360 from them until they're
/// between 0 and 360
/// </summary>
void Tank::increaseTurretRotation()
{
	m_previousTurretRotaion = m_turretRotation;
	m_turretRotation += m_rotationAngle;
	if (m_turretRotation == 360.0)
	{
		m_turretRotation = 0.0;
	}
}

/// <summary>
/// decreases the rotation of the tirret
/// loops back once it reaches 0 because values above 360 will be the same as if you just keep subtracting 360 from them until they're
/// between 0 and 360
/// </summary>
void Tank::decreaseTurretRotation()
{
	m_previousTurretRotaion = m_turretRotation;
	m_turretRotation -= m_rotationAngle;
	if (m_turretRotation == 0.0)
	{
		m_turretRotation = 359.0;
	}
}

/// <summary>
/// sets the rotation of the turret to the rotation of the base of the tank
/// </summary>
void Tank::centreTurret()
{
	m_previousTurretRotaion = m_turretRotation;
	m_turretRotation = m_rotation;
}

/// <summary>
/// adjusts the rotation of the tank when it is colliding with a wall to try and get it to stop deflecting
/// </summary>
void Tank::adjustRotation()
{
	//if tank was rotating...
	if (m_rotation != m_previousRotation)
	{
		//work out which direction to rotate the tank base post collision
		if (m_rotation > m_previousRotation)
		{
			m_rotation = m_previousRotation - 1;
		}
		else
		{
			m_rotation = m_previousRotation + 1;
		}
	}

	//if turret was rotating while tank was moving
	if (m_turretRotation != m_previousTurretRotaion)
	{
		//set the turret rotation back to it's pre-collision value
		m_turretRotation = m_previousTurretRotaion;
	}
}

