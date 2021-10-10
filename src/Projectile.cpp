#include "Projectile.h"

/// <summary>
/// sets the bullet to the position of the player
/// also gives it the same rotation of the player
/// the sprite has to be given an extra rotation of 90 because of the direction that it is facing in the sprite sheet
/// </summary>
/// <param name="t_x"></param>
/// <param name="t_y"></param>
/// <param name="t_rotation"></param>
void Projectile::init(int t_x, int t_y, double t_rotation)
{
	m_x = t_x;
	m_y = t_y;

	m_rotation = t_rotation;

	m_sprite.setPosition(m_x, m_y);

	m_directionVector = thor::rotatedVector(sf::Vector2f(1.0f, 0.0f), static_cast<float>(m_rotation));
	m_sprite.setRotation(m_rotation + 90);

	initSprite();
}

/// <summary>
/// loads in the texxture for the bullet
/// </summary>
/// <param name="t_filename"></param>
void Projectile::loadTexture(std::string& t_filename)
{
	if(!m_texture.loadFromFile(t_filename))
	{
		std::string s("Error loading texture");
		throw std::exception(s.c_str());
	}
}

//sets up the sprite by selecting what part of the sprite sheet to use
void Projectile::initSprite()
{
	m_sprite.setTexture(m_texture);
	sf::IntRect rect(7, 177, 6, 9);

	m_sprite.setTextureRect(rect);
	m_sprite.setOrigin(rect.width / 2.0, rect.height / 2.0);
	m_sprite.setScale(3, 3);
}

/// <summary>
/// draws the sprite
/// </summary>
/// <param name="t_window"></param>
void Projectile::draw(sf::RenderWindow& t_window)
{
	t_window.draw(m_sprite);
}

/// <summary>
/// updates the position of the sprite by using the direction vector as this will move it in the diirecction that it is rotatesd
/// this will then be multiplied by the speed and (dt/1000) makes sure that it doesn't move too far each frame so that it is not
/// instantly off of the screen after a single frame
/// it will also checks to see if it hits any of the walls on the screen
/// if it hits a wall it will be moved off the screen so the code knows that it is not in use anymore
/// </summary>
/// <param name="dt"></param>
/// <param name="wallSprites"></param>
bool Projectile::update(double dt, std::map<int, std::list<sf::Sprite>>& t_walls, std::pair<sf::Sprite&, sf::Sprite&> t_aiTank)
{
	m_directionVector = thor::rotatedVector(sf::Vector2f(1.0f, 0.0f), static_cast<float>(m_rotation));

	double newX = m_directionVector.x  * m_speed * (dt / 1000);
	double newY = m_directionVector.y  * m_speed * (dt / 1000);

	m_sprite.move(newX, newY);

	int cellId = 0;
	int cellHeight = ScreenSize::s_height / m_NUM_COLUMNS;
	int cellWidth = ScreenSize::s_width / m_NUM_COLUMNS;

	cellId = floor(m_sprite.getTransform().transformPoint(0.0,0.0).x / cellWidth)
		+ (floor(m_sprite.getTransform().transformPoint(0.0, 0.0).y / cellHeight) * m_NUM_COLUMNS);
	if (checkWallCollision(t_walls[cellId]))
	{
		m_sprite.setPosition(-1000, -1000);
	}
	cellId = floor(m_sprite.getPosition().x + m_sprite.getGlobalBounds().width / cellWidth)
		+ (floor(m_sprite.getPosition().y / cellHeight) * m_NUM_COLUMNS);
	if (checkWallCollision(t_walls[cellId]))
	{
		m_sprite.setPosition(-1000, -1000);
	}
	cellId = floor(m_sprite.getPosition().x / cellWidth)
		+ (floor(m_sprite.getPosition().y + m_sprite.getGlobalBounds().height / cellHeight) * m_NUM_COLUMNS);
	if (checkWallCollision(t_walls[cellId]))
	{
		m_sprite.setPosition(-1000, -1000);
	}
	//m_sprite.getTransform.transformPoint(0, 0);
	//m_sprite.getTransform.transformPoint(width, 0);
	cellId = floor(m_sprite.getPosition().x + m_sprite.getGlobalBounds().width / cellWidth)
		+ (floor(m_sprite.getPosition().y + m_sprite.getGlobalBounds().height / cellHeight) * m_NUM_COLUMNS);
	if (checkWallCollision(t_walls[cellId]))
	{
		m_sprite.setPosition(-1000, -1000);
	}
	if (m_speed == 0)
	{		
		m_sprite.setPosition(-1000, -1000);
	}

	if (CollisionDetector::collision(m_sprite,t_aiTank.first) || CollisionDetector::collision(m_sprite,t_aiTank.second))
	{
		m_sprite.setPosition(-1000, -1000);
		m_x = m_sprite.getPosition().x;
		m_y = m_sprite.getPosition().y;
		return true;
	}
	m_x = m_sprite.getPosition().x;
	m_y = m_sprite.getPosition().y;
	return false;
}

/// <summary>
/// it is in use if it is still on the screen
/// </summary>
/// <returns></returns>
bool Projectile::inUse() const
{
	return (m_x >= 0 && m_x <= ScreenSize::s_width) && (m_y >= 0 && m_y <= ScreenSize::s_height);
}

bool Projectile::checkWallCollision(std::list<sf::Sprite>& walls)
{
	for (sf::Sprite const& sprite : walls)
	{
		//checks if either the tank base or turret has collided witht the current
		//wall sprite.
		if (CollisionDetector::collision(m_sprite, sprite))
		{
			return true;
		}
	}
	
	return false;
}
