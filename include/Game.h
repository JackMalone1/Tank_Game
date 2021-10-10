#pragma once

#include <map>
#include <list>

#include <SFML/Graphics.hpp>

#include "ScreenSize.h"
#include "LevelLoader.h"
#include "include/Tank.h"
#include "ProjectilePool.h"
#include "Projectile.h"
#include "LevelCreator.h"
#include "Globals.h"
#include "TankAI.h"
#include "Gamestate.h"
#include "HUD.h"


/// <summary>
/// @author RP
/// @date June 2017
/// @version 1.0
/// 
/// </summary>

/// <summary>
/// @brief Main class for the SFML Playground project.
/// 
/// This will be a single class framework for learning about SFML. 
/// Example usage:
///		Game game;
///		game.run();
/// </summary>



/// <summary>
/// used to store the high scores for the level that the player is currently playing
/// </summary>
struct HighScoreTable
{
	std::vector<int> m_highScores;
};

class Game
{
public:
	/// <summary>
	/// @brief Default constructor that initialises the SFML window, 
	///   and sets vertical sync enabled. 
	/// </summary>
	Game();

	/// <summary>
	/// @brief the main game loop.
	/// 
	/// A complete loop involves processing SFML events, updating and drawing all game objects.
	/// The actual elapsed time for a single game loop results (lag) is stored. If this value is 
	///  greater than the notional time for one loop (MS_PER_UPDATE), then additional updates will be 
	///  performed until the lag is less than the notional time for one loop.
	/// The target is one update and one render cycle per game loop, but slower PCs may 
	///  perform more update than render operations in one loop.
	/// </summary>
	void run();

protected:

	Tank m_tank;//player object
	TankAi m_aiTank;

	sf::Sprite m_sprite;//used for the walls
	sf::Texture m_texture;//sprite sheet

	sf::Texture m_bgTexture;//used for the backkground
	sf::Sprite m_bgSprite;//used to display the background
	//wall sprites
	std::vector<sf::Sprite> m_wallSprites;
	//target sprites
	sf::Sprite m_targetSprite;

	const int m_NUM_COLUMNS = 10;

	std::map<int, std::list<sf::Sprite>> m_spatialMap;

	void initMap();

	/// <summary>
	/// used to display a warning when the target is about to dissapear
	/// </summary>
	sf::Sprite m_warningSprite;

	sf::Clock m_timer;//used as a timer to check when the game is over
	sf::Time m_time = sf::seconds(60);//how much time the player has to complete the game

	sf::Clock m_targetTimer;//used to check when to spawn a new target as well as how long the target should stay on the screen for
	sf::Time m_timeToSpawn = sf::seconds(2);//how long a target should wait from the last target disappearing to spawning on the screen

	sf::Clock m_displayWarningTime;//used to check when to display a warning for the target about to dissapear
	sf::Time m_timeToDisplayWarning = sf::seconds(4);//warning displayed four seconds after target appears on scree
	sf::Time m_totalDisplayTime = sf::seconds(6);//target stays on screen for four seconds

	bool m_displayTarget{ false };//used to check if the target should be displayed
	bool m_displayWarning{ false };//used to check if the warning should be displayed

	int m_currentTarget{ -1 };//current target that is being used. first target is at index zero but this is incremented before displaying target so this will have correct value

	GameState m_gamestate = GameState::MainMenu;//game starts out at the main menu of the game

	sf::Text m_displayTimeLeft;//displays the total time of gameplay left in the game
	sf::Text m_displayGameOver;//displays text for the game being over

	sf::Text m_displayScore;//displays the current score that the player has

	sf::Font m_font;//font used in the game

	sf::Text m_mainMenuMessage;//text used for the message to display the main menu

	LevelCreator m_creator;//used to create new levels in the game

	sf::RectangleShape m_buttons[m_NUM_BUTTONS];//used to make the buttons
	ButtonPressed m_buttonPressed = ButtonPressed::None;//keeps track of what button the player clicked on

	int m_numTargets{ 0 };//number of targets on the current level of the game

	int m_currentLevel{ 0 };//keeps track of the number of the level that they are playing


	/// <summary>
	/// @brief Creates the wall sprites and loads them in a vector.
	/// note that sf::sprite is conosidered a light weight class, so
	/// storing copies(instead of pointers to sf::sprite) in std::vector is acceptable.
	/// </summary>
	void generateWalls();

	void setUpText();//sets up the font and all of the text objects

	void createTarget();//sets a target in a new position depending on what target should be being used at the moment

	LevelData m_level;//stores data about the level that the player is currently playing
	/// <summary>
	/// @brief Placeholder to perform updates to all game objects.
	/// </summary>
	/// <param name="time">update delta time</param>
	void update(double dt);

	void updateProject2(double dt);

	/// <summary>
	/// @brief Draws the background and foreground game objects in the SFML window.
	/// The render window is always cleared to black before anything is drawn.
	/// </summary>
	void render();

	void renderProject2();

	/// <summary>
	/// @brief Checks for events.
	/// Allows window to function and exit. 
	/// Events are passed on to the Game::processGameEvents() method.
	/// </summary>	
	void processEvents();

	/// <summary>
	/// @brief Handles all user input.
	/// </summary>
	/// <param name="event">system event</param>
	void processGameEvents(sf::Event&);

	void processMenuEvents(sf::Event& t_event);

	void updateMainMenu();//highlights buttons that the player is hovering over
	int processMainMenu(sf::Event t_event);//used to check if the player presses any buttons on the main menu screen
	bool checkFileEmpty(int t_filename);//used to check if a file is empty before trying to load from it
	void resetTargets();//resets the timers for the targets and adds on any extrea time to the timer for the next target
	void displayWarning();//display warning if a target is about to disappear
	void loadPreviousScores(int t_filename);//loads in there previous scores for the level that they are on
	void initGame();//re inits the game every time that they play the game again
	void saveHighScores();//saves their high scores into a yaml file
	void calculateScore();

	void updateGameplay(double dt);//used to update gameplay when the game is running

	void setUpGameOver();//sets up text for accuracy and score to display when the game is tomorrow
	void saveHighScoresToFile();//saves the high scores for the current screen to a yaml file for that specific screen

	void drawGameplay();//draws the gameplay screen when i tis on that screen

	void drawMainMenu();//draws the main menu when the game is on the main menu

	void loadFont();//loads in the font that the text objects use in the game

	void restartTimers();//restarts the timers used to keep track of things in the game when the player starts a new game

	void getLevel();//asks the user to input a level number

	void loadLevel();//loads in the next level
	// main window
	sf::RenderWindow m_window;

	HighScoreTable m_highScoreTable;//used to store the high scores for the current level

	HUD m_hud;
};
