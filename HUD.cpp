#include "HUD.h"


////////////////////////////////////////////////////////////
HUD::HUD(sf::Font& hudFont)
	: m_textFont(hudFont)
{
	m_gameStateText.setFont(hudFont);
	m_gameStateText.setCharacterSize(30);
	m_gameStateText.setFillColor(sf::Color::Blue);
	m_gameStateText.setPosition(sf::Vector2f(480, 5));
	m_gameStateText.setString("Game Running");

	m_AITankHealthtext.setFont(hudFont);
	m_AITankHealthtext.setCharacterSize(30);
	m_AITankHealthtext.setFillColor(sf::Color::Blue);
	m_AITankHealthtext.setString("AI Tank Health: ");
	//Setting up our hud properties 
	m_hudOutline.setSize(sf::Vector2f(ScreenSize::s_width, m_gameStateText.getGlobalBounds().height + 10.0f));
	m_hudOutline.setFillColor(sf::Color(0, 0, 0, 38));
	m_hudOutline.setOutlineThickness(-.5f);
	m_hudOutline.setOutlineColor(sf::Color(0, 0, 0, 100));
	m_hudOutline.setPosition(0, 0);
}

////////////////////////////////////////////////////////////
void HUD::update(GameState const& gameState)
{
	switch (gameState)
	{
	case GameState::Gameplay:
		m_gameStateText.setString("Gameplay");
		break;
	case GameState::GameOver:
		m_gameStateText.setString("Game Over");
		break;
	case GameState::LevelCreator:
		m_gameStateText.setString("Level Creator");
		break;
	case GameState::MainMenu:
		m_gameStateText.setString("Main Menu");
		break;
	case GameState::Project2:
		m_gameStateText.setString("Project 2");
		break;
	default:
		break;
	}
}

void HUD::render(sf::RenderWindow& window, int t_health)
{
	window.draw(m_hudOutline);
	window.draw(m_gameStateText);
	if (m_gameStateText.getString() == "Gameplay")
	{
		sf::RectangleShape healthBar;
		healthBar.setSize(sf::Vector2f(t_health * 50.0f, 20.0f));
		healthBar.setPosition(m_AITankHealthtext.getPosition()
		+ sf::Vector2f(m_AITankHealthtext.getGlobalBounds().width + 20.0f, m_AITankHealthtext.getGlobalBounds().height / 2.0));
		healthBar.setFillColor(sf::Color::Red);
		window.draw(healthBar);
		m_AITankHealthtext.setPosition(m_gameStateText.getPosition()
		+ sf::Vector2f(m_gameStateText.getGlobalBounds().width + 20.0f, 0.0f));
		window.draw(m_AITankHealthtext);
	}
	if (m_gameStateText.getString() == "Project 2")
	{
		sf::RectangleShape healthBar;
		healthBar.setSize(sf::Vector2f(t_health * 50.0f, 20.0f));
		healthBar.setPosition(m_AITankHealthtext.getPosition()
			+ sf::Vector2f(m_AITankHealthtext.getGlobalBounds().width + 20.0f, m_AITankHealthtext.getGlobalBounds().height / 2.0));
		healthBar.setFillColor(sf::Color::Red);
		window.draw(healthBar);
		m_AITankHealthtext.setString("Player Health");
		m_AITankHealthtext.setPosition(m_gameStateText.getPosition()
			+ sf::Vector2f(m_gameStateText.getGlobalBounds().width + 20.0f, 0.0f));
		window.draw(m_AITankHealthtext);
	}
}