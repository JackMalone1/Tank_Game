#pragma once
#include <map>
#include <SFML/Graphics.hpp>
#include <random>
#include <list>
#include "MathUtility.h"
#include "CollisionDetector.h"
#include "ProjectilePool.h"
#include "ScreenSize.h"
#include "TankAI.h"
#include <time.h>
#include "DamageList.h"
/// <summary>
/// @brief A simple tank controller.
/// 
/// This class will manage all tank movement and rotations.
/// </summary>
class Tank
{
public:	
	/// <summary>
	/// @brief Constructor that stores drawable state (texture, sprite) for the tank.
	/// Stores references to the texture and container of wall sprites. 
	/// Creates sprites for the tank base and turret from the supplied texture.
	/// </summary>
	/// <param name="texture">A reference to the sprite sheet texture</param>
	///< param name="texture">A reference to the container of wall sprites</param> 
	Tank(sf::Texture const & texture, std::map<int, std::list<sf::Sprite>>& t_walls);
	void update(double dt, std::map<int, std::list<sf::Sprite>>& t_walls, TankAi& t_tank);
	void render(sf::RenderWindow & window);
	void setPosition(sf::Vector2f t_position);

	/// <summary>
	/// @brief Increases the speed by 1, max speed is capped at 100
	/// 
	/// </summary>
	void increaseSpeed();

	/// <summary>
	/// @brief Decreases the speed by 1, min speed is capped at -100
	/// </summary>
	void decreaseSpeed();

	/// <summary>
	/// @brief Increases the rotation by 1 degree, wraps to 0 degrees after 359
	/// </summary>
	void increaseRotation();

	/// <summary>
	/// @brief Decreases the rotation by 1 degree, wraps to 359 degrees after 0
	/// </summary>
	void decreaseRotation();

	/// <summary>
	/// @brief Checks for collisions between the tank and the walls.
	/// 
	/// </summary>
	/// <returns>True if collision detected between tank and wall.</returns>
	bool checkWallCollision();

	/// <summary>
	/// @brief Stops the tank if moving and applies a small increase in speed in the opposite direction of travel.
	/// If the tank speed is currently 0, the rotation is set to a value that is less than the previous rotation value
	///  (scenario: tank is stopped and rotates into a wall, so it gets rotated towards the opposite direction).
	/// If the tank is moving, further rotations are disabled and the previous tank position is restored.
	/// The tank speed is adjusted so that it will travel slowly in the opposite direction. The tank rotation 
	///  is also adjusted as above if necessary (scenario: tank is both moving and rotating, upon wall collision it's 
	///  speed is reversed but with a smaller magnitude, while it is rotated in the opposite direction of it's 
	///  pre-collision rotation).
	void deflect();

	/// <summary>
	/// @brief Reads the player tank position.
	/// <returns>The tank base position.</returns>
	/// </summary>
	sf::Vector2f getPosition() const;

	bool checkReadyToFire();//checks if enough time has passed since the last time that they fired a bullet

	void fireBullets();//fire bullet if enough time has passed since they last fired a bullet

	sf::Sprite tankBase() const { return m_tankBase; }//used for drawing the tank base

	bool hitTarget(const sf::Sprite& t_sprite) const;//checks if any of the bullets fired hit a target
	void increaseTargetsHit() { m_numBulletsHit++; }
	int getAccuracyPercentage();
	void resetAccuracy();

	void resetScore();
	int getScore();
	void setScore(int t_score);

	void setHealth(int t_health) { m_health = t_health; }
	int getHealth() { return m_health; };
	void takeDamage();

	sf::Sprite getTurret() const { return m_turret; }
	sf::Sprite getBase() const { return m_tankBase; }

	void renderDamageList(sf::RenderWindow& t_window) { m_damageList.render(t_window); }

	void clearList() { m_damageList.clearList(); }
	
	void resetDamage();
private:
	// A reference to the container of wall sprites.
	std::map<int, std::list<sf::Sprite>>& m_walls;
	sf::Sprite m_tankBase;
	sf::Sprite m_turret;
	sf::Texture const & m_texture;
	// The tank speed
	double m_speed{ 0.0 };

	const float m_DEFAULT_BRAKE_SPEED{ 2.0f };
	float m_brakeSpeed{ m_DEFAULT_BRAKE_SPEED };

	const float m_DEFAULT_ACCELERATION_SPEED{ 2.0f };
	float m_acceleration{ m_DEFAULT_ACCELERATION_SPEED };

	const float m_DEFAULT_ROTATION_ANGLE{ 2.0f };
	float m_rotationAngle{ m_DEFAULT_ROTATION_ANGLE };

	// The current rotation as applied to tank base
	double m_rotation{ 0.0 };
	double m_turretRotation{ 0.0 };

	double const m_FRICTION = 0.1;

	bool m_enableRotation{ true };
	double m_previousSpeed{ 0.0 };
	double m_previousRotation{ 0.0 };
	double m_previousTurretRotaion{ 0.0 };
	sf::Vector2f m_previousPosition{ 0.0f,0.0f };

	int m_timeWaitedToFire{ 0 };
	const int m_FIRE_RATE{ 60 };//player is able to fire a bullet once every second

	ProjectilePool m_pool;//pool of bullets that the player is able to use to fire a bullet

	static const int SCREEN_WIDTH{ 1440 };
	static const int SCREEN_HEIGHT{ 900 };

	int m_numBulletsFired{ 0 };
	int m_numBulletsHit{ 0 };

	int m_score{ 0 };
	int m_health{ 5 };

	const int m_NUM_COLUMNS = 10;

	const double m_MAX_SPEED = 140.0;
	double m_currentMaxSpeed = m_MAX_SPEED;

	DamageList m_damageList;
private:
	void initSprites();

	void handleKeyInput();

	void increaseTurretRotation();

	void decreaseTurretRotation();

	void centreTurret();

	void adjustRotation();

	bool checkCollisions(std::list<sf::Sprite>& t_walls);

	void moveOffScreen();

	void applyFriction();

	void updatePosition(double dt);
};
