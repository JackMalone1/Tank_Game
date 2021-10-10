#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Object.h"
#include "LevelLoader.h"
#include <filesystem>
#include <fstream>


class LevelCreator
{
public:
	LevelCreator();
	bool openFile(int t_filename);
	bool storeInFile(int t_filename);
	bool checkFileExists(int t_filename);
	void createNewFile(int t_filename);
	void placeObject();
	void draw(sf::RenderWindow& t_window);
	void pickObject(sf::RenderWindow& t_window);
	void update(sf::RenderWindow& t_window);
	void updateObjectPreviewRotation(sf::Event t_event);
	void processEvents(sf::Event t_event, sf::RenderWindow& t_window);
	void processMouseEvents(sf::Event t_event);
	void setUpLoadedObjects();
	int getInteger(int t_int, std::string* t_message);
	bool isFileEmpty(int t_filename);
	void setUpText();
private:
	std::vector<Object> m_placedObjects;
	Object m_previewObject;
	ObjectSelected m_objectSelected{ ObjectSelected::Wall };
	sf::Texture m_texture;
	float m_previewRotation;
	sf::Texture m_previewTexture;
	LevelData m_level;
	bool m_previewSelected{ false };
	sf::RectangleShape m_helpBox;
	sf::Text m_displayHelp;
	sf::Font m_font;
};

