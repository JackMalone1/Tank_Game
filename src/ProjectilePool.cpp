#include "ProjectilePool.h"

int ProjectilePool::m_inUseCount = 0;

/// <summary>
/// default constructor
/// </summary>
ProjectilePool::ProjectilePool(std::map<int, std::list<sf::Sprite>>& t_walls) : m_walls{t_walls}
{
}

/// <summary>
/// checks through all of the bullets to see if there is one available to be used
/// if they are able to be used then it will return a pointer to the bullet
/// otherwise it will return a nullptr so the tank knows not to use that bullet
/// </summary>
/// <returns></returns>
Projectile* ProjectilePool::create()
{
	for (Projectile& p : m_pool)
	{
		if (!p.inUse())
		{
			m_inUseCount++;
			return &p;
		}
	}
	return nullptr;
}

/// <summary>
/// returns the number of bullets
/// </summary>
/// <returns></returns>
int ProjectilePool::inUse()
{
	return m_inUseCount;
}

/// <summary>
/// passes the name of the file that has the texture to init the textures as well as initialisng all of the sprites for the bullets
/// </summary>
/// <param name="t_filename"></param>
void ProjectilePool::init(std::string t_filename)
{
	for (auto& a : m_pool)
	{
		a.loadTexture(t_filename);
		a.initSprite();
	}
}

/// <summary>
/// moves and checks for collisions between the bullet and the walls for any bullet that is currently in use
/// </summary>
/// <param name="dt"></param>
/// <param name="wallSprites"></param>
int ProjectilePool::update(double dt, std::map<int, std::list<sf::Sprite>>& t_walls, std::pair<sf::Sprite&, sf::Sprite&> t_aiTank)
{
	int hitCount = 0;
	for (auto& a : m_pool)
	{
		if (a.inUse())
		{
			if (a.update(dt, t_walls, t_aiTank))
			{
				hitCount++;
			}
		}
	}
	return hitCount;
}

/// <summary>
/// draws any bullets that are in use
/// </summary>
/// <param name="t_window"></param>
void ProjectilePool::draw(sf::RenderWindow& t_window)
{
	for (auto& a : m_pool)
	{
		if (a.inUse())
		{
			a.draw(t_window);
		}
	}
}

/// <summary>
/// checks to see if any of the bullets in use have hit the target
/// </summary>
/// <param name="t_sprite"></param>
/// <returns></returns>
bool ProjectilePool::hitTarget(const sf::Sprite& t_sprite) const
{
	for (auto& a : m_pool)
	{
		if (a.inUse())
		{
			if (a.m_sprite.getGlobalBounds().intersects(t_sprite.getGlobalBounds()))
			{
				return true;
			}
		}
	}
	return false;
}
