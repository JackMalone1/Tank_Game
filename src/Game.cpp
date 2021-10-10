#include "Game.h"
#include <iostream>

/// <summary>
/// @author Jack Malone
/// seems to have an error where it will not add score to the player when they shoot a target but this has only happened
/// once for when the player hit the second target as the first target they hit in level 1
/// in the level creator they seem to need to press the left mouse click twice and sometimes 
/// need to input the file number to save to
/// when saving a level you created in the level creator and saving to the same file you loaded from, you need to input
/// the file nu,ber twice to get the file to save
/// the file wont save in the first attempt and ask you to input again but once you input again it will work fine
/// twice
/// </summary>

// Updates per milliseconds
static double const MS_PER_UPDATE = 10.0;
bool sort(int i, int j)
{
	return i > j;
}
////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_width, ScreenSize::s_height, 32), "SFML Playground", sf::Style::Default),
	m_tank(m_texture, m_spatialMap),
	m_aiTank(m_texture, m_wallSprites, m_spatialMap),
	m_hud(m_font) // Add this line
{
	m_window.setVerticalSyncEnabled(true);
	//sets what level is currently being played
	m_currentLevel = 1;


	//sets up the positions of all of the bullets for the main menu
	sf::Vector2f m_buttonPosition = sf::Vector2f{500.0f, 40.0f};
	for (int index = 0; index < m_NUM_BUTTONS; index++)
	{
		m_buttonPosition += sf::Vector2f(0.0f, 120.0f);
		m_buttons[index].setSize(sf::Vector2f(400.0f, 30.0f));
		m_buttons[index].setFillColor(UNPRESSEED_BUTTON);
		m_buttons[index].setPosition(m_buttonPosition);
	}

	//will generate an exception if level loading fails
	try
	{
		LevelLoader::load(m_currentLevel, m_level);
		//loadPreviousScores(currentLevel);
	}
	catch (std::exception& e)
	{
		std::cout << "Level loading failure." << std::endl;
		std::cout << e.what() << std::endl;
		throw e;
	}

	//loads in the sprite sheet	
	if (!m_texture.loadFromFile("./resources/images/SpriteSheet.png"))
	{
		std::string s("Error loading texture");
		throw std::exception(s.c_str());
	}
	
	//loads in the background texture
	if (!m_bgTexture.loadFromFile(m_level.m_background.m_fileName))
	{
		std::cout << "Error loading the texture for the background" << std::endl;
	}

	//sets up teh texture
	m_bgSprite.setTexture(m_bgTexture);
	m_sprite.setOrigin(m_sprite.getGlobalBounds().width / 2.0,
		m_sprite.getGlobalBounds().height / 2.0);


	//sets up the textures and positions of the walls
	generateWalls();
	setUpText();//sets up all of the text in the game

	//sets the sprite for the target
	sf::IntRect targetRect(44, 126, 27, 20);
	m_targetSprite.setTexture(m_texture);
	m_targetSprite.setTextureRect(targetRect);
	m_targetSprite.setOrigin(targetRect.width / 2.0, targetRect.height / 2.0);
	m_targetSprite.setScale(2.0f, 2.0f);

	m_numTargets = m_level.m_targets.size();

	//sets up the sprite for the warning
	sf::IntRect warningRect(98, 96, 9, 13);
	m_warningSprite.setTexture(m_texture);
	m_warningSprite.setTextureRect(warningRect);
	m_warningSprite.setOrigin(warningRect.width / 2.0, warningRect.height / 2.0);
	m_warningSprite.setScale(2.0f, 2.0f);

	initMap();
	// Populate the obstacle list and set the AI tank position.
	m_aiTank.init(m_level.m_aiTank.m_position);
}

//default game loop
////////////////////////////////////////////////////////////
void Game::run()
{
	sf::Clock clock;
	sf::Int32 lag = 0;

	while (m_window.isOpen())
	{
		sf::Time dt = clock.restart();

		lag += dt.asMilliseconds();

		processEvents();

		while (lag > MS_PER_UPDATE)
		{
			update(MS_PER_UPDATE);
			lag -= MS_PER_UPDATE;
		}
		update(MS_PER_UPDATE);

		render();
	}
}

void Game::initMap()
{
	if (!m_spatialMap.empty())
	{
		m_spatialMap.clear();
	}

	int cellHeight = ScreenSize::s_height / m_NUM_COLUMNS;
	int cellWidth = ScreenSize::s_height / m_NUM_COLUMNS;
	for (sf::Sprite& wall : m_wallSprites)
	{
		int cellId = 0;
		cellId = floor(wall.getPosition().x / cellWidth) + (floor(wall.getPosition().y / cellHeight) * m_NUM_COLUMNS);
		m_spatialMap[cellId].push_back(wall);
		cellId = floor(wall.getPosition().x + wall.getGlobalBounds().width / cellWidth)
			+ (floor(wall.getPosition().y / cellHeight) * m_NUM_COLUMNS);
		m_spatialMap[cellId].push_back(wall);
		cellId = floor(wall.getPosition().x / cellWidth) + (floor(wall.getPosition().y 
			+ wall.getGlobalBounds().height / cellHeight) * m_NUM_COLUMNS);
		m_spatialMap[cellId].push_back(wall);
		cellId = floor(wall.getPosition().x + wall.getGlobalBounds().width / cellWidth)
			+ (floor(wall.getPosition().y + wall.getGlobalBounds().height / cellHeight) * m_NUM_COLUMNS);
		m_spatialMap[cellId].push_back(wall);
	}
}

////////////////////////////////////////////////////////////
void Game::processEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)//close window
		{
			m_window.close();
		}
		processGameEvents(event);
		if (GameState::LevelCreator == m_gamestate)//checks for input for the level creator
		{
			m_creator.processEvents(event, m_window);
		}		
		if (GameState::GameOver == m_gamestate)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			{
				m_gamestate = GameState::MainMenu;
			}
		}

	}
}

////////////////////////////////////////////////////////////
void Game::processGameEvents(sf::Event& event)
{
	if (GameState::MainMenu == m_gamestate)//check if they want to close the window or not
	{
		processMenuEvents(event);	
	}
	if (GameState::Gameplay == m_gamestate || GameState::LevelCreator == m_gamestate
		|| GameState::GameOver == m_gamestate)//check if they want to go back to the main menu
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			m_gamestate = GameState::MainMenu;
			break;
		default:
			break;
		}
	}
}

/// <summary>
/// process main menu will return -1 if no button wass pressed
///	0 is to go to gameplay
///	1 will go to the level creator
/// 2 will close the game
/// </summary>
/// <param name="t_event"></param>
void Game::processMenuEvents(sf::Event& t_event)
{
	int output = processMainMenu(t_event);
	if (output != -1)
	{
		if (output == 0)
		{
			initGame();
			m_gamestate = GameState::Gameplay;
		}
		else if (output == 1)
		{
			initGame();
			m_gamestate = GameState::Project2;
		}
		else if (output == 2)
		{
			m_gamestate = GameState::LevelCreator;
			std::string message = "Enter the number of the level you want to open or number of level you want to create";
			m_currentLevel = m_creator.getInteger(m_currentLevel, &message);
			m_creator.openFile(m_currentLevel);
		}
		else if (output == 3)
		{
			m_window.close();
		}
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		m_window.close();
	}
}

/// <summary>
/// when the game is in the main menu it will check if the player is currently hovering over any of the buttons
/// that are on the screen
/// if the mouse button is currently on one of then buttons then the button will be highlighted to show the player
/// that they are selecting the button. the button will only be pressed when the player presses the left mouse
/// button
/// function uses the get position function for both the button and the mouse in relation to the window to figure out if they are
/// in the same place
/// </summary>
void Game::updateMainMenu()
{
	for (int index = 0; index < m_NUM_BUTTONS; index++)
	{
		if (sf::Mouse::getPosition(m_window).x > m_buttons[index].getPosition().x
			&& sf::Mouse::getPosition(m_window).x < m_buttons[index].getPosition().x + m_buttons[index].getSize().x)
		{
			m_buttons[index].setFillColor(PRESSED_BUTTON);

		}
		else
		{
			m_buttons[index].setFillColor(UNPRESSEED_BUTTON);

		}

		if (!(sf::Mouse::getPosition(m_window).y > m_buttons[index].getPosition().y
			&& sf::Mouse::getPosition(m_window).y < m_buttons[index].getPosition().y + m_buttons[index].getSize().y))
		{
			m_buttons[index].setFillColor(UNPRESSEED_BUTTON);
		}
	}

}

/// <summary>
/// if the playoer presses the left mouse button while in the main menu while they are currently over one of the buttons
/// then the function will return the index of the button they pressed so that the process events function is able
/// to change the game state correctly
/// returns -1 if they were not over any of the buttons
/// </summary>
/// <param name="t_event"></param>
/// <returns></returns>
int Game::processMainMenu(sf::Event t_event)
{
	if (sf::Event::MouseButtonPressed == t_event.type)
	{
		if (sf::Mouse::Left == t_event.mouseButton.button)
		{
			for (int i = 0; i < m_NUM_BUTTONS; i++)
			{
				if (m_buttons[i].getFillColor() == PRESSED_BUTTON)
				{
					return i;
				}
			}
		}
	}

	return -1;
}

/// <summary>
/// checks if a file is empty to check if data can be read from the file or not
/// </summary>
/// <param name="t_filename">level number if the file that you want to check</param>
/// <returns>returns true if the file is empty</returns>
bool Game::checkFileEmpty(int t_filename)
{
	std::ifstream stream(".\\resources\\levels\\level" + std::to_string(t_filename) + ".yaml");
	return stream.peek() == std::ifstream::traits_type::eof();
}

/// <summary>
/// resets all of the timers for the targets and resets the bools to display the warning and target
/// adds extra time to the target being displayed based on however much time was left for the target before it
/// </summary>
void Game::resetTargets()
{
	if (m_targetTimer.getElapsedTime() < m_totalDisplayTime)
	{
		m_totalDisplayTime = sf::seconds(6) + (m_totalDisplayTime - m_targetTimer.getElapsedTime());
		m_timeToSpawn = sf::seconds(0);//the next one is displayed right after the current one
	}
	else
	{
		m_timeToSpawn = sf::seconds(2);
		m_totalDisplayTime = sf::seconds(6);
	}

	m_targetTimer.restart();
	m_displayWarningTime.restart();
	m_displayTarget = false;
	m_displayWarning = false;
}

/// <summary>
/// if the target is close to disappearing it will set the bool to true to display a warning to the user
/// </summary>
void Game::displayWarning()
{
	if (m_displayWarningTime.getElapsedTime() >= m_timeToDisplayWarning)
	{
		m_displayWarning = true;
		m_displayWarningTime.restart();
	}
}

/// <summary>
/// reads in a single high score from the score node
/// </summary>
/// <param name="scoreNode">list of high scores</param>
/// <param name="score">where to store the high score</param>
void operator >> (const YAML::Node& scoreNode, int& score)
{
	score = scoreNode["value"].as<int>();
}

/// <summary>
/// needs to clear the vector of high scores if the player was playing already and loaded a different files
/// if it wasn't cleared it would keep the scores that they get on the other level as well in the vector so it would basically 
/// keep junk values
/// it then loads in the high scores for the current level anbd then sorts them highest to lowest
/// </summary>
/// <param name="scoresNode"></param>
/// <param name="level"></param>
void operator >> (const YAML::Node& scoresNode, HighScoreTable& level)
{
	if (!level.m_highScores.empty())
	{
		level.m_highScores.clear();
	}

	const YAML::Node& scoreNode = scoresNode["scores"].as<YAML::Node>();
	for (unsigned i = 0; i < scoreNode.size(); ++i)
	{
		int score;
		scoreNode[i] >> score;
		level.m_highScores.push_back(score);
	}
	std::sort(level.m_highScores.begin(), level.m_highScores.end());
}

/// <summary>
/// loads in the high scores from the file for the current level
/// if no high scores currently exist for the level it will create base values for the file and then load in the files
/// throws an exception if it wasn't able to properly load from the files
/// </summary>
/// <param name="t_filename"></param>
void Game::loadPreviousScores(int t_filename)
{
	std::string filename = ".\\resources\\highscores\\highscores" + std::to_string(t_filename) + ".yaml";
	if (!m_highScoreTable.m_highScores.empty())
	{
		m_highScoreTable.m_highScores.clear();
	}

	try
	{
		std::ifstream stream(filename);
	
		if (stream.peek() == std::ifstream::traits_type::eof())
		{
			std::ofstream outfile(filename);
			outfile << "scores:\n";
			outfile << "  - type: regular\n";
			outfile << "    value: 0\n";
			outfile << "  - type: regular\n";
			outfile << "    value: 0\n";
			outfile.close();
		}
		YAML::Node baseNode = YAML::LoadFile(filename);
		if (baseNode.IsNull())
		{
			std::string message("File: " + filename + " not found");
			throw std::exception(message.c_str());
		}
		baseNode >> m_highScoreTable;
	}
	catch (YAML::ParserException& e)
	{
		std::string message(e.what());
		message = "YAML Parser Error: " + message;
		throw std::exception(message.c_str());
	}
	catch (std::exception& e)
	{
		std::string message(e.what());
		message = "Unexpected Error: " + message;
		throw std::exception(message.c_str());
	}
}

/// <summary>
/// re initialises the game every time they want to play a new level as well as asking them to input what level they want to play and then loads 
/// in the new level
/// </summary>
void Game::initGame()
{
	getLevel();

	loadLevel();

	generateWalls();

	initMap();

	m_numTargets = m_level.m_targets.size();

	m_tank.resetAccuracy();
	m_tank.resetScore();
	m_tank.setHealth(10);
	m_tank.clearList();
	m_tank.resetDamage();
	m_aiTank.resetHealth();
	//levels they create will have different numbers of targets so needs to change based on how many there are
	m_time = sf::seconds(10 * m_numTargets + 10);
	m_currentTarget = -1;
	restartTimers();
}

/// <summary>
/// restarts the timers so that when the game goes in to the gameplay it doesn't automatically go into game
/// over or is already halfway through displaying the targets
/// </summary>
void Game::restartTimers()
{
	m_timer.restart();
	m_targetTimer.restart();
	m_displayWarningTime.restart();
}

/// <summary>
/// gets the user to input the level number of the level that they want to play
/// </summary>
void Game::getLevel()
{
	std::cin.clear();
	std::cout << "Enter the number of the level you want to open or number of level you want to play" << std::endl;
	std::cin >> m_currentLevel;
}

/// <summary>
/// loads in the level that the player wants to play
/// if the level that they asked for was not there or empty then it will just load level one 
/// as level one should always exist and be able to be used
/// it knows if a file cannot be used if the first thing that it finds in the file is the end of the file
/// </summary>
void Game::loadLevel()
{
	try
	{
		if (!checkFileEmpty(m_currentLevel))
		{
			LevelLoader::load(m_currentLevel, m_level);
			loadPreviousScores(m_currentLevel);
		}
		else
		{
			std::cout << "File asked for was empty, loading base level instead" << std::endl;
			LevelLoader::load(1, m_level);
		}
	}
	catch (std::exception& e)
	{
		std::cout << "Level loading failure." << std::endl;
		std::cout << e.what() << std::endl;
		throw e;
	}
}


/// <summary>
/// attempt to save the high scores to the files
/// clear any data already in the file so that it doesn't duplicate values to the file
/// </summary>
void Game::saveHighScores()
{
	std::string filename = ".\\resources\\highscores\\highscores" + std::to_string(m_currentLevel) + ".yaml";
	std::ofstream outfile(filename);
	outfile.clear();
	outfile << "scores:\n";
	for (int i = 0; i < m_highScoreTable.m_highScores.size(); i++)
	{
		outfile << "  - type: regular\n";
		std::string string = "    value: " + std::to_string(m_highScoreTable.m_highScores.at(i)) + "\n";
		outfile << string;
	}
	outfile.close();
}

void Game::calculateScore()
{
	if (static_cast<int>((m_totalDisplayTime.asSeconds() - m_targetTimer.getElapsedTime().asSeconds() > 0))
		&& m_tank.hitTarget(m_targetSprite))
	{
		m_tank.setScore(m_tank.getScore() + (100 * static_cast<int>((m_totalDisplayTime.asSeconds()
			- m_targetTimer.getElapsedTime().asSeconds()))));
	}
	else if (m_tank.hitTarget(m_targetSprite))
	{
		m_tank.setScore(m_tank.getScore() + 100);
	}
}

void Game::updateGameplay(double dt)
{
	//creates a new target if the respawn timer reaches 2 seconds and there isnt already a target being displayed
	if (m_targetTimer.getElapsedTime() >= m_timeToSpawn && !m_displayTarget)
	{
		createTarget();
	}
	if (m_displayTarget)//if target is being displayed
	{
		//if 4 seconds has passed since the target was first displayed it will display the warning
		displayWarning();
		//if the player hits the target with a bullet it will stop displaying the target and reset
		//all of the timers so that it will display everything normally for the next target
		if (m_tank.hitTarget(m_targetSprite) || m_targetTimer.getElapsedTime() >= m_totalDisplayTime)
		{
			//gives the player more score the quicker that they were able to shoot the targets
			calculateScore();
			resetTargets();
			if (m_tank.hitTarget(m_targetSprite))
			{
				m_tank.increaseTargetsHit();
			}
		}
	}
	m_tank.update(dt, m_spatialMap,m_aiTank);//updates the player
	m_tank.fireBullets();//checks to see if the player can fire a bullet and if they pressed space to fire a bullet
	m_aiTank.update(m_tank, dt, m_spatialMap);

	if (m_aiTank.collidesWithPlayer(m_tank))
	{
		setUpGameOver();
		m_gamestate = GameState::GameOver;
	}
	if (m_aiTank.getHealth() == 0)
	{
		setUpGameOver();
		m_gamestate = GameState::GameOver;
	}
}

/// <summary>
/// sets up text to display what our accuracy was and what your score was
/// </summary>
void Game::setUpGameOver()
{
	m_displayGameOver.setString("Game Over\nYour accuracy was: " + std::to_string(m_tank.getAccuracyPercentage())
		+ "%\n");
	m_highScoreTable.m_highScores.push_back(m_tank.getScore());
	std::sort(m_highScoreTable.m_highScores.begin(), m_highScoreTable.m_highScores.end(), sort);
	m_displayGameOver.setString(m_displayGameOver.getString() + "Your score was: " + std::to_string(m_tank.getScore())
		+ "\n");
}

/// <summary>
/// saves all of the high scores to the yaml file for the current level and then sets up the text object to display 
/// all of the current high scores to be displayed when the game is over
/// </summary>
void Game::saveHighScoresToFile()
{
	saveHighScores();
	for (int i = 0; i < m_highScoreTable.m_highScores.size(); i++)
	{
		m_displayGameOver.setString(m_displayGameOver.getString() + "Score" + std::to_string(i) + ": " +
			std::to_string(m_highScoreTable.m_highScores.at(i)) + "\n");
	}
}

/// <summary>
/// draws everything for the gameplay
/// </summary>
void Game::drawGameplay()
{
	for (auto& obstacle : m_wallSprites)//draws all walls
	{
		m_window.draw(obstacle);
	}
	if (m_displayTarget)//draws the target if it is on the screen
	{
		m_window.draw(m_targetSprite);
	}
	if (m_displayWarning)//displays the warning if it is on the screen
	{
		m_window.draw(m_warningSprite);
	}
	m_tank.render(m_window);//draws the tank

	m_aiTank.render(m_window);
	//displays the time left in the game
	m_displayTimeLeft.setString("Time left: " + std::to_string(60 -
		static_cast<int>(m_timer.getElapsedTime().asSeconds())));
	m_window.draw(m_displayTimeLeft);
	m_displayScore.setString("Score: " + std::to_string(m_tank.getScore()));
	m_window.draw(m_displayScore);
}

void Game::drawMainMenu()
{
	for (int index = 0; index < m_NUM_BUTTONS; index++)//drawing the shapes for the buttons
	{
		m_window.draw(m_buttons[index]);
	}
	//drawing the text on the buttons
	m_mainMenuMessage.setString("Play the game");
	m_mainMenuMessage.setPosition(m_buttons[0].getPosition() + sf::Vector2f(150.0f, 5.0f));
	m_window.draw(m_mainMenuMessage);

	m_mainMenuMessage.setString("Project 2");
	m_mainMenuMessage.setPosition(m_buttons[1].getPosition() + sf::Vector2f(150.0f, 5.0f));
	m_window.draw(m_mainMenuMessage);

	m_mainMenuMessage.setString("Level Creator");
	m_mainMenuMessage.setPosition(m_buttons[2].getPosition() + sf::Vector2f(150.0f, 5.0f));
	m_window.draw(m_mainMenuMessage);

	m_mainMenuMessage.setString("Exit Game");
	m_mainMenuMessage.setPosition(m_buttons[3].getPosition() + sf::Vector2f(150.0f, 5.0f));
	m_window.draw(m_mainMenuMessage);
}

void Game::loadFont()
{
	if (!m_font.loadFromFile("ariblk.ttf"))
	{
		std::string errorMsg("Error loading font");
		throw std::exception(errorMsg.c_str());
	}
}

/// <summary>
/// places all of the walls in their correcct position from the positions that were loaded in from the yaml file
/// they will be added into a vector so if different levels have different amounts of walls it will still be able to 
/// handle all of the extra walls
/// </summary>
void Game::generateWalls()
{
	sf::IntRect wallRect(2, 129, 33, 23);
	if (!m_wallSprites.empty())
	{
		m_wallSprites.clear();
	}

	//create the walls
	for (ObstacleData const& obstacle : m_level.m_obstacles)
	{
		sf::Sprite sprite;
		sprite.setTexture(m_texture);
		sprite.setTextureRect(wallRect);
		sprite.setOrigin(wallRect.width / 2.0, wallRect.height / 2.0);
		sprite.setPosition(obstacle.m_position);
		sprite.setRotation(obstacle.m_rotation);
		m_wallSprites.push_back(sprite);
	}
}


/// <summary>
/// sets up the font and text for all of the texxt in the game
/// </summary>
void Game::setUpText()
{
	loadFont();

	m_displayTimeLeft.setFont(m_font);
	m_displayGameOver.setFont(m_font);
	m_displayScore.setFont(m_font);

	m_displayTimeLeft.setCharacterSize(24);
	m_displayGameOver.setCharacterSize(24);
	m_displayScore.setCharacterSize(24);

	m_displayTimeLeft.setFillColor(sf::Color::Black);
	m_displayGameOver.setFillColor(sf::Color::Red);
	m_displayScore.setFillColor(sf::Color::Black);

	m_displayTimeLeft.setPosition(sf::Vector2f(50, 50));
	m_displayGameOver.setPosition(sf::Vector2f(50, 50));
	m_displayScore.setPosition(sf::Vector2f(50, 100));

	m_displayTimeLeft.setString("Time left: " + std::to_string(60 - 
static_cast<int>(m_timer.getElapsedTime().asSeconds())));
m_displayGameOver.setString("Game Over!");
m_displayScore.setString("Score: 0");


m_mainMenuMessage.setFont(m_font);
m_mainMenuMessage.setCharacterSize(14);
m_mainMenuMessage.setFillColor(sf::Color::White);
m_mainMenuMessage.setPosition(m_buttons[0].getPosition() + sf::Vector2f(5, 5));
m_mainMenuMessage.setString("hello world");
}

/// <summary>
/// updates the index of the target that is currently being used
/// gives an offset to the targets position based on the offset that was loaded in from the yaml file
/// it will randomly find an offset from the minus and positive of this offset
/// it will the set the position of the target as well as the warning and set the bools to true for the target 
/// and false for the warning
/// </summary>
void Game::createTarget()
{
	m_currentTarget++;
	if (m_currentTarget < m_numTargets && m_currentTarget > -1)
	{
		int offset = -m_level.m_targets[m_currentTarget].offset +
			(rand() % (m_level.m_targets[m_currentTarget].offset -
				-m_level.m_targets[m_currentTarget].offset + 1));
		m_targetSprite.setPosition(m_level.m_targets[m_currentTarget].m_position.x + offset,
			m_level.m_targets[m_currentTarget].m_position.y + offset);
		m_displayTarget = true;
		m_displayWarning = false;
		m_warningSprite.setPosition(m_targetSprite.getPosition().x, m_targetSprite.getPosition().y - 10);
	}
}

////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	/// <summary>
	/// if the time for gameplay reaches the max time then the game will go to game over
	/// if the game finishes displaying all of the targets then the game will also go to game over
	/// once it reaches game over it will set up the text with score taht the player got as well as saving the scores to the file
	/// in case that these values were corrupted later on in the code
	/// </summary>
	/// <param name="dt"></param>
	if (GameState::Gameplay == m_gamestate)//if it is displaying the gameplay
	{
		updateGameplay(dt);

		if ((m_timer.getElapsedTime() >= m_time || m_currentTarget > m_numTargets - 1))
		{
			setUpGameOver();
			saveHighScoresToFile();
			m_gamestate = GameState::GameOver;
		}
	}
	else if (GameState::LevelCreator == m_gamestate)
	{
		m_creator.update(m_window);//updates the level creator if they are currently using it
	}
	else if (GameState::MainMenu == m_gamestate)
	{
		updateMainMenu();//updates the main menu if they are on the game menu
	}
	else if (GameState::Project2 == m_gamestate)
	{
		updateProject2(dt);
	}
	m_hud.update(m_gamestate);
}

void Game::updateProject2(double dt)
{
	//creates a new target if the respawn timer reaches 2 seconds and there isnt already a target being displayed
	if (m_targetTimer.getElapsedTime() >= m_timeToSpawn && !m_displayTarget)
	{
		createTarget();
	}
	if (m_displayTarget)//if target is being displayed
	{
		//if 4 seconds has passed since the target was first displayed it will display the warning
		displayWarning();
		//if the player hits the target with a bullet it will stop displaying the target and reset
		//all of the timers so that it will display everything normally for the next target
		if (CollisionDetector::collision(m_tank.getBase(), m_targetSprite)
			|| CollisionDetector::collision(m_tank.getTurret(), m_targetSprite)
			|| m_targetTimer.getElapsedTime() >= m_totalDisplayTime)
		{
			//gives the player more score the quicker that they were able to shoot the targets
			calculateScore();
			resetTargets();
			if (m_tank.hitTarget(m_targetSprite))
			{
				m_tank.increaseTargetsHit();
			}
		}
	}
	m_tank.update(dt, m_spatialMap, m_aiTank);//updates the player
	m_aiTank.update(m_tank, dt, m_spatialMap);

	//checks to see if the ai has hit the player or not
	if(m_aiTank.hitTarget(std::pair<sf::Sprite, sf::Sprite>(m_tank.getBase(), m_tank.getTurret())))
	{
		m_tank.takeDamage();
	}

	/// <summary>
	/// set up game over for either when the ai hits the player or the player loses all of their health
	/// </summary>
	if (m_aiTank.collidesWithPlayer(m_tank))
	{
		setUpGameOver();
		m_gamestate = GameState::GameOver;
	}
	if (m_tank.getHealth() <= 0)
	{
		setUpGameOver();
		m_gamestate = GameState::GameOver;
	}
}

/// <summary>
/// figures out what screen is supposed to be displayed at the moment and then draws the correct screen
/// </summary>
////////////////////////////////////////////////////////////
void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));
	m_window.draw(m_bgSprite);//always draws the background
	if (GameState::LevelCreator == m_gamestate)
	{
		m_creator.draw(m_window);
	}
	else if (GameState::Gameplay == m_gamestate)//draws everything in the gameplay screen
	{
		drawGameplay();
	}
	else if(GameState::MainMenu == m_gamestate)
	{
		drawMainMenu();
	}
	else if (GameState::GameOver == m_gamestate)
	{
		m_window.draw(m_displayGameOver);//draws the game over text
	}
	else if (GameState::Project2 == m_gamestate)
	{
		renderProject2();
	}
	if (GameState::Project2 == m_gamestate)
	{
		m_hud.render(m_window, m_tank.getHealth());
	}
	else
	{
		m_hud.render(m_window, m_aiTank.getHealth());
	}
	
	m_window.display();
}

void Game::renderProject2()
{
	drawGameplay();
	m_tank.renderDamageList(m_window);//tell the user that type of damage they have taken so far in the game
}
