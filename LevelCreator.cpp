#include "LevelCreator.h"

LevelCreator::LevelCreator()
{
	//loads the sprtiesheet so it is able to load the texture for a target and a wall
	if (!m_texture.loadFromFile("./resources/images/SpriteSheet.png"))
	{
		std::string s("Error loading texture");
		throw std::exception(s.c_str());
	}

	setUpText();
}

/// <summary>
/// checks to see if a fil exists before trying to open it
/// if it already exists it will load in the file and return true to say it was able to load it
/// if it doesn't exist it will create the file but wont load it in since there wouldnt be anything in the file at
/// the moment
/// </summary>
/// <param name="t_filename"></param>
/// <returns></returns>
bool LevelCreator::openFile(int t_filename)
{
	if (checkFileExists(t_filename))
	{
		//can't load in from the file if it is empty
		if (!isFileEmpty(t_filename))
		{
			LevelLoader::load(t_filename, m_level);
		}
		

		setUpLoadedObjects();
		std::cout << "File exists. File was opened" << std::endl;
		return true;
	}
	else
	{
		createNewFile(t_filename);
		std::cout << "File doesn't exist. The file was created instead." << std::endl;
	}
	return false;
}

/// <summary>
/// checks to see if the file if it is trying to save to already exists or not
/// if it does not exist it will create the file first before trying to save to it
/// after opening/creating and opening the file it will then save to the file
/// the function is mostly formatting the data that it has correctly so that it will be formatted correctly
/// </summary>
/// <param name="t_filename"></param>
/// <returns></returns>
bool LevelCreator::storeInFile(int t_filename)
{
	if (!checkFileExists(t_filename))
	{
		createNewFile(t_filename);
	}
	//splits the placed objects into targets and walls
	std::sort(m_placedObjects.begin(), m_placedObjects.end());
	//sets up the standard parts of the file that the user isn't able to input
	std::ofstream outfile(".\\resources\\levels\\level" + std::to_string(t_filename) + ".yaml");
	outfile << "background:\n   file: ./resources/images/Background.jpg # Sample comment\n";
	outfile << "tank:\n   position: {x: 800, y: 100}\n   max_projectiles: 10\n   reload_time: 1000\n";
	outfile << "ai_tank:\n   position: {x: 400, y: 700}\n   max_projectiles: 10\n   reload_time: 1000\n";
	outfile << "projectile:\n   speed: 1000\n   damage: 10\n";
	outfile << "targets:\n";
	int count = 0;
	//loads in the targets placed
	for (auto& target : m_placedObjects)
	{

		if (target.returnType() == ObjectSelected::Target)
		{
			outfile << "   - type: regular\n     position: {x: " +
				std::to_string(static_cast<int>(target.getSprite().getPosition().x))
				+ ", y: " + std::to_string(static_cast<int>(target.getSprite().getPosition().y)) 
				+ ", randomOffset: 20}\n";
			count++;
		}
	}
	//makes sure that it saves enough to the file so that the level loader is able to load from the file properly 
	while(count < 2)
	{
		outfile << "   - type: regular\n     position: {x: " +
			std::to_string(static_cast<int>(100))
			+ ", y: " + std::to_string(static_cast<int>(100))
			+ ", randomOffset: 20}\n";
	}
	count = 0;
	outfile << "obstacles:\n";
	//stores the walls placed
	for (auto& wall : m_placedObjects)
	{
		if (wall.returnType() == ObjectSelected::Wall)
		{
			outfile << "   - type: wall\n     position: {x: " 
				+ std::to_string(static_cast<int>(wall.getSprite().getPosition().x))
				+ ", y: " + std::to_string(static_cast<int>(wall.getSprite().getPosition().y)) 
				+ "}\n     rotation: " 
				+ std::to_string(static_cast<int>(wall.getSprite().getRotation())) + "\n";
			count++;
		}
	}
	//makes sure that it saves enough to the file so that the level loader is able to load from the file properly 
	while (count < 2)
	{
		outfile << "   - type: wall\n     position: {x: "
			+ std::to_string(static_cast<int>(100))
			+ ", y: " + std::to_string(200)
			+ "}\n     rotation:"
			+ std::to_string(static_cast<int>(0)) + "\n";
	}
	outfile.close();
	return false;
}

/// <summary>
/// checks to see if a file exists or not
/// </summary>
/// <param name="t_filename"></param>
/// <returns></returns>
bool LevelCreator::checkFileExists(int t_filename)
{
	return std::filesystem::exists(".\\resources\\levels\\level" + std::to_string(t_filename) + ".yaml");
}

/// <summary>
/// creates a new file
/// </summary>
/// <param name="t_filename"></param>
void LevelCreator::createNewFile(int t_filename)
{
	std::ofstream outfile(".\\resources\\levels\\level" + std::to_string(t_filename) + ".yaml");
	outfile.close();
}

/// <summary>
/// place object by adding it to the end of the vector of currently placed objects
/// </summary>
void LevelCreator::placeObject()
{
	m_placedObjects.push_back(m_previewObject);
	m_previewObject = Object();
}

/// <summary>
/// draws everything to the screen
/// </summary>
/// <param name="t_window"></param>
void LevelCreator::draw(sf::RenderWindow& t_window)
{
	for (Object& object : m_placedObjects)
	{
		t_window.draw(object.getSprite());
	}

	if (m_previewSelected)
	{
		t_window.draw(m_previewObject.getSprite());
	}

	t_window.draw(m_helpBox);
	t_window.draw(m_displayHelp);
}

/// <summary>
/// checks what object the user selects then sets the type correctly as well as the texture correctly
/// it will also set the position of this object to the current position of the mosue in relation the window
/// </summary>
/// <param name="t_window"></param>
void LevelCreator::pickObject(sf::RenderWindow& t_window)
{
	sf::IntRect wallRect(2, 129, 33, 23);
	sf::IntRect targetRect(44, 126, 27, 20);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
	{
		m_previewRotation = 0.0f;
		m_previewSelected = true;
		m_previewObject = Object();
		m_previewObject.setType(ObjectSelected::Wall);
		m_previewObject.setTexture(m_texture);
		m_previewObject.setTextureRect(wallRect);
		m_previewObject.setOrigin(sf::Vector2f(wallRect.width / 2.0, wallRect.height / 2.0));
		m_previewObject.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(t_window)));
		m_previewObject.setRotation(m_previewRotation);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
	{
		m_previewSelected = true;
		m_previewObject = Object();
		m_previewObject.setType(ObjectSelected::Target);
		m_previewObject.setTexture(m_texture);
		m_previewObject.setTextureRect(targetRect);
		m_previewObject.setOrigin(sf::Vector2f(targetRect.width / 2.0, targetRect.height / 2.0));
		m_previewObject.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(t_window)));
	}
}

/// <summary>
/// updates the posiution of the currently selected object if there is one selected
/// </summary>
/// <param name="t_window"></param>
void LevelCreator::update(sf::RenderWindow& t_window)
{
	if (m_previewSelected)
	{
		m_previewObject.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(t_window)));
	}
}

/// <summary>
/// changes the rotation of the currently selected object
/// </summary>
/// <param name="t_event"></param>
void LevelCreator::updateObjectPreviewRotation(sf::Event t_event)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		m_previewRotation -= 1;
		if (m_previewRotation == 0.0)
		{
			m_previewRotation = 359.0;
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		m_previewRotation += 1;
		if (m_previewRotation == 360.0)
		{
			m_previewRotation = 0.0;
		}
	}

	m_previewObject.setRotation(m_previewRotation);
}

/// <summary>
/// checks for keyboard input from the user
/// if the player presses enter then it will prompt the player to input the level number that they want to save in
/// at the moment it needs to ask the player twice when they want to save it in the same file that they opened the file
/// from
/// once they input where they want to save the file it will then save the file and they can either continue working
/// on the level or go back to the main menu by pressing escape
/// if they havent picked the type of object that they want to use then it will check if they press the number 1 for
/// picking a wall and the number 2 for a target
/// if they have a picked the type of object that they want to place it will let them rotate the object before placing it
/// however this rotation will only be kept for the wall since this is the only one that the rotation will really matter
/// since the target is a circle
/// process event swill also check to see if the player presses the left mouse  button to know when they are trying to place
/// an object
/// only have one check to see if they are trying to place an object when they are currently placing an object but takes two
/// clicks to be able to place the object
/// </summary>
/// <param name="t_event">event that was passed in from the game</param>
/// <param name="t_window">window that the game is running on, useed for figuring out where they are picking the object</param>
void LevelCreator::processEvents(sf::Event t_event, sf::RenderWindow& t_window)
{
	if (t_event.KeyPressed == t_event.type)
	{
		//when the player presses the enter the level creator will set the player save the level to a file
		if (sf::Keyboard::isKeyPressed
		(sf::Keyboard::Enter))
		{
			int fileNum = 0;
			std::string message = "Please enter a file number to store the new level in: ";
			fileNum = getInteger(fileNum, &message);
			storeInFile(fileNum);
		}
		if (!m_previewSelected)
		{
			pickObject(t_window);
		}
		else
		{
			updateObjectPreviewRotation(t_event);
		}
	}
	processMouseEvents(t_event);
}

/// <summary>
/// checks to see if the player presses the left mouse button to know if it should place the object that they currently
/// have selected
/// </summary>
/// <param name="t_event"></param>
void LevelCreator::processMouseEvents(sf::Event t_event)
{
		if (sf::Mouse::Left == t_event.mouseButton.button)
		{
			if (m_previewSelected)
			{
				m_previewSelected = false;
				placeObject();
			}
		}
}

/// <summary>
/// loads in objects from a file if the player is using a file that already existed
/// </summary>
void LevelCreator::setUpLoadedObjects()
{
	sf::IntRect wallRect(2, 129, 33, 23);
	sf::IntRect targetRect(44, 126, 27, 20);
	int i = 0;
	//create the walls
	for (ObstacleData const& obstacle : m_level.m_obstacles)
	{
		m_placedObjects.push_back(Object());
		m_placedObjects.at(i).setTexture(m_texture);
		m_placedObjects.at(i).setTextureRect(wallRect);
		m_placedObjects.at(i).setOrigin(sf::Vector2f(wallRect.width / 2.0, wallRect.height / 2.0));
		m_placedObjects.at(i).setPosition(obstacle.m_position);
		m_placedObjects.at(i).setRotation(obstacle.m_rotation);
		m_placedObjects.at(i).setType(ObjectSelected::Wall);
		i++;
	}
	for (TargetData const& target : m_level.m_targets)
	{
		m_placedObjects.push_back(Object());
		m_placedObjects.at(i).setTexture(m_texture);
		m_placedObjects.at(i).setTextureRect(targetRect);
		m_placedObjects.at(i).setOrigin(sf::Vector2f(targetRect.width / 2.0, targetRect.height / 2.0));
		m_placedObjects.at(i).setPosition(target.m_position);
		m_placedObjects.at(i).setType(ObjectSelected::Target);
		i++;
	}
}

/// <summary>
/// displays a message to the player on what they should be inputting
/// it then makes sure taht what they inputted is an int
/// while they have not correctly inputted an int it will continually ask for more input
/// and will telll them that they inputted the data incoreectly
/// </summary>
/// <param name="t_int">used to store the int that they inputted</param>
/// <param name="t_message">message to be displayed to the user so they know what data is being inputted</param>
/// <returns>returns the int that they inputted once a proper input has been received</returns>
int LevelCreator::getInteger(int t_int, std::string* t_message)
{
	std::cin.clear();
	std::cout << *t_message << std::endl;
	std::cin >> t_int;

	return t_int;
}

/// <summary>
/// checks to see if a file is empty before the code tries to load anything from a file
/// </summary>
/// <param name="t_filename">level number that you want to check</param>
/// <returns>returns true if the file is empty</returns>
bool LevelCreator::isFileEmpty(int t_filename)
{
	std::ifstream stream(".\\resources\\levels\\level" + std::to_string(t_filename) + ".yaml");
	return stream.peek() == std::ifstream::traits_type::eof();
}

/// <summary>
/// sets up all of the text on the screen to tell the player how to use the level creator
/// </summary>
void LevelCreator::setUpText()
{
	if (!m_font.loadFromFile(".\\resources\\fonts\\ariblk.ttf"))
	{
		std::cout << "error loading the font to display the help text for the level creator" << std::endl;
	}
	m_displayHelp.setFont(m_font);
	m_displayHelp.setString("Press 1 to select a wall type to be able to move around and place.\n");
	m_displayHelp.setString(m_displayHelp.getString() + "Press 2 to select a target type to move around and place.\n");
	m_displayHelp.setString(m_displayHelp.getString() +
		"Press the left mouse button to place your currently selected object.\n");
	m_displayHelp.setString(m_displayHelp.getString() + "Press enter to save your file.\n");
	m_displayHelp.setString(m_displayHelp.getString() + "Press escape to go to the main menu without saving.\n");
	m_displayHelp.setCharacterSize(14);
	m_displayHelp.setFillColor(sf::Color::Red);
	m_displayHelp.setPosition(30, 30);
	m_helpBox.setSize(sf::Vector2f(m_displayHelp.getGlobalBounds().width, m_displayHelp.getGlobalBounds().height));
	m_helpBox.setSize(sf::Vector2f(m_helpBox.getSize().x + 20, m_helpBox.getSize().y + 20));
	m_helpBox.setPosition(m_displayHelp.getPosition().x - 10, m_displayHelp.getPosition().y - 10);
	m_helpBox.setFillColor(sf::Color(211, 211, 211, 155));
}

