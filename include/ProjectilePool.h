#pragma once
#include "Projectile.h"
#include <map>
#include <list>
class ProjectilePool
{
public:
	ProjectilePool(std::map<int, std::list<sf::Sprite>>& t_walls);//default constructor
	Projectile* create();//checks if there is a free bullet to be fired and returns its memory adress if there is

	static int inUse();//returns how many bullets are in use

	void init(std::string t_filename);//initialises the sprites for the bullets

	int update(double dt, std::map<int, std::list<sf::Sprite>>& t_walls, std::pair<sf::Sprite&, sf::Sprite&> t_aiTank);//updates the positions of active bullets and checks for collsions between walls

	void draw(sf::RenderWindow& t_window);//draws all of the bullets that are currently in use
	bool hitTarget(const sf::Sprite& t_sprite) const;//checks to see if any of the bullets in use have hit the current target
private:
	Projectile m_pool[100];//array of 100 bullets

	static int m_inUseCount;
	std::map<int, std::list<sf::Sprite>>& m_walls;//all of the walls that are in the current level
};

