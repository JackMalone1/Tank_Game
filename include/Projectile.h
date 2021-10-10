#pragma once
#include <SFML/Graphics.hpp>
#include "MathUtility.h"
#include "CollisionDetector.h"
#include <list>
#include "ScreenSize.h"

class Projectile
{
public:
	friend class ProjectilePool;//projectile pool is the only class allowed to make bullets soo it is a friend clas and the constructor was made private
	void init(int t_x, int t_y, double t_rotation);//inits the bullet to the position and rotation of the player when it is fired
private:
	void loadTexture(std::string& t_filename);//loads in a texture from a file
	void initSprite();//sets the texture of the sprite as well as giving it a default position which is off of the screen
	void draw(sf::RenderWindow& t_window);//draws the bullet if it is in use

	bool update(double dt,
		        std::map<int, std::list<sf::Sprite>>& t_walls, std::pair<sf::Sprite&,sf::Sprite&> t_aiTank);//moves the bullet and checks if it collides with walls if it is use

	bool inUse() const;//checks if the bullet is on the screen or not to determine if it is still in use
	bool checkWallCollision(std::list<sf::Sprite>& walls);//checks to see if the bullet hits a wall

	sf::Sprite sprite() { return m_sprite; }//returns the sprite
private:
	// NB Constructor function is hidden - only friend classes can make an 
	//  instance of Particle.
	Projectile() = default;

	int m_x{ -1 };
	int m_y{ -1 };

	int m_xVel{ 0 };
	int m_yVel{ 0 };

	double m_rotation{ 0 };
	const double m_speed{ 1000 };

	sf::Sprite m_sprite;
	sf::Texture m_texture;

	sf::Vector2f m_directionVector{0.0f, 0.0f};//used to determine in what direction the bullet should be moving
	const int m_NUM_COLUMNS = 10;

	std::pair<bool, bool> checkCollisions();
};

