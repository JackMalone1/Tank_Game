#pragma once
#include "MathUtility.h"

#include <SFML/Graphics.hpp>
#include <Thor/Vectors.hpp>
#include <Thor/Time.hpp>
#include <iostream>
#include <queue>
#include <array>
#include "ProjectilePool.h"


class Tank; // forward reference, do not #include "Tank.h" yet...

enum class AIState
{
	PATROL_MAP,
	ATTACK_PLAYER
};

class TankAi
{
public:
	/// <summary>
	/// @brief Constructor that stores a reference to the obstacle container.
	/// Initialises steering behaviour to seek (player) mode, sets the AI tank position and
	///  initialises the steering vector to (0,0) meaning zero force magnitude.
	/// </summary>
	/// <param name="texture">A reference to the sprite sheet texture</param>
	///< param name="wallSprites">A reference to the container of wall sprites</param>
	TankAi(sf::Texture const& texture, std::vector<sf::Sprite>& wallSprites, std::map<int, std::list<sf::Sprite>>& t_walls);

	/// <summary>
	/// @brief Steers the AI tank towards the player tank avoiding obstacles along the way.
	/// Gets a vector to the player tank and sets steering and velocity vectors towards
	/// the player if current behaviour is seek. If behaviour is stop, the velocity vector
	/// is set to 0. Then compute the correct rotation angle to point towards the player tank. 
	/// If the distance to the player tank is < MAX_SEE_AHEAD, then the behaviour is changed from seek to stop.
	/// Finally, recalculate the new position of the tank base and turret sprites.
	/// </summary>
	/// <param name="playerTank">A reference to the player tank</param>
	/// <param name="dt">update delta time</param>
	void update(Tank const & playerTank, double dt, std::map<int, std::list<sf::Sprite>>& t_walls);

	/// <summary>
	/// @brief Draws the tank base and turret.
	///
	/// </summary>
	/// <param name="window">The SFML Render window</param>
	void render(sf::RenderWindow& window);

	/// <summary>
	/// @brief Initialises the obstacle container and sets the tank base/turret sprites to the specified position.
	/// <param name="position">An x,y position</param>
	/// </summary>
	void init(sf::Vector2f position);

	enum class AiType
	{
		AI_ID_NONE,
		AI_ID_SEEK_SHOOT_AT_PLAYER
	};

	/// <summary>
	/// @brief Checks for collision between the AI and player tanks.
	///
	/// </summary>
	/// <param name="player">The player tank instance</param>
	/// <returns>True if collision detected between AI and player tanks.</returns>
	bool collidesWithPlayer(Tank const& playerTank) const;
	sf::Sprite getBase() { return m_tankBase; }
	sf::Sprite getTurret() { return m_turret; }

	void takeDamage(int t_damage);
	int getHealth() { return m_health; }
	void resetHealth() { m_health = 5; }

	std::pair<bool, bool> isPlayerInVisionCone(sf::Vector2f t_playerPos);

	bool checkReadyToFire();//checks if enough time has passed since the last time that they fired a bullet

	void fireBullets();//fire bullet if enough time has passed since they last fired a bullet
	bool hitTarget(const std::pair<sf::Sprite, sf::Sprite> t_tank) const;//checks if any of the bullets fired hit a target
private:
	void initSprites();

	void updateMovement(double dt);

	sf::Vector2f seek(sf::Vector2f playerPosition) const;

	sf::Vector2f collisionAvoidance();

	const sf::CircleShape findMostThreateningObstacle();

	// A reference to the sprite sheet texture.
	sf::Texture const& m_texture;

	// A sprite for the tank base.
	sf::Sprite m_tankBase;

	// A sprite for the turret
	sf::Sprite m_turret;

	// A reference to the container of wall sprites.
	std::vector<sf::Sprite>& m_wallSprites;

	// The current rotation as applied to tank base and turret.
	double m_rotation{ 0.0 };

	// Current velocity.
	sf::Vector2f m_velocity;

	// Steering vector.
	sf::Vector2f m_steering;

	// The ahead vector.
	sf::Vector2f m_ahead;

	// The half-ahead vector.
	sf::Vector2f m_halfAhead;

	// The maximum see ahead range.
	static float constexpr MAX_SEE_AHEAD{ 250.0f };

	// The maximum avoidance turn rate.
	static float constexpr MAX_AVOID_FORCE{ 50.0f };

	// 
	static float constexpr MAX_FORCE{ 10.0f };

	// The maximum speed for this tank.
	float MAX_SPEED = 50.0f;

	// A container of circles that represent the obstacles to avoid.
	std::vector<sf::CircleShape> m_obstacles;

	enum class AiBehaviour
	{
		SEEK_PLAYER,
		STOP,
		RETREAT
	} m_aiBehaviour;
	bool circleInterectsLine(sf::Vector2f t_ahead, sf::Vector2f t_halfAhead, sf::CircleShape t_circle);
	float distance(sf::Vector2f t_position, sf::CircleShape t_obstacle);
	AIState m_aiState;

	const float MASS = 5.0f;

	int m_health;
	std::array<sf::Vector2f, 2> m_visionConeLeft;
	std::array<sf::Vector2f, 2> m_visionConeRight;

	bool isPointRight(std::array<sf::Vector2f, 2> t_visionCone, sf::Vector2f t_playerPos);
	int angle = 0;

	void rotate(float t_angle);

	sf::Vector2f m_visionConeDir{ 1,0 };  // Initially point along the x-axis

	void setVisionCone(float t_angle, float const MAX_SEE_AHEAD);

	int m_timeWaitedToFire{ 0 };
	const int m_FIRE_RATE{ 60 };//player is able to fire a bullet once every second

	const int m_ATTACK_PLAYER_ANGLE = 50.0f;

	const int m_PATROL_MAP_ANGLE = 60.0f;

	ProjectilePool m_pool;

	void attackPlayer(sf::Vector2f t_vectorToPlayer,
		sf::Vector2f& t_acceleration, float t_angle, Tank const & t_player,
		float t_distToPlayer);
	void patrolMap(sf::Vector2f t_vectorToPlayer, sf::Vector2f& t_acceleration, Tank const& t_player,
		float t_distToPlayer);

	void shootAtPlayer(sf::Vector2f t_playerPos);
	float distanceFromPoint(sf::Vector2f t_point);

	void moveOffScreen();
};

#include "Tank.h"