#include "DamageList.h"
#include <iostream>

/// <summary>
/// sets up the text so that it will be on the right side of the screen just under the other part of the hud
/// </summary>
/// <param name="t_fontPath"></param>
/// <param name="t_textData"></param>
DamageList::DamageList(std::string t_fontPath, std::string t_textData) : m_baseString{t_textData}
{
	if (!m_font.loadFromFile(t_fontPath))
	{
		std::cout << "error loading font for damage list" << std::endl;
	}

	m_text.setFont(m_font);
	m_text.setCharacterSize(14);
	m_text.setFillColor(sf::Color(119, 136, 153));
	m_text.setString(m_baseString);
	m_text.setPosition(ScreenSize::s_width * 0.75, ScreenSize::s_height * 0.10);
	m_background.setFillColor(sf::Color(135, 206, 250, 125));
}

/// <summary>
/// draws all of the text to the screen
/// </summary>
/// <param name="t_window"></param>
void DamageList::render(sf::RenderWindow& t_window)
{
	t_window.draw(m_background);
	t_window.draw(m_text);

}

/// <summary>
/// adds whatever type of damage they took last to the list and then only appends to the string if it is a new instance of that type of damage for
/// the player playing the game this time
/// </summary>
/// <param name="t_type">type of damage that the player took</param>
/// <param name="t_damageInfo">message you want to be dsiplayed to the user</param>
void DamageList::appendList(DamageType t_type, std::string t_damageInfo)
{
	std::list<DamageType>::iterator it = m_damageTaken.begin();
	bool appendString = true;
	for (int i = 0; i < m_damageTaken.size(); i++)
	{
		if (i > 0)
		{
			std::advance(it, 1);
		}
		if (t_type == *it)
		{
			appendString = false;
			break;
		}
	}
	m_damageTaken.push_back(t_type);
	m_damageTaken.sort();
	m_damageTaken.unique();
	if (appendString)
	{
		m_text.setString(m_text.getString() + "\n" + t_damageInfo);
		m_background.setSize(sf::Vector2f(m_text.getGlobalBounds().width, m_text.getGlobalBounds().height) + sf::Vector2f(10.0f, 10.0f));
		m_background.setPosition(m_text.getPosition() + sf::Vector2f(-5.0f, -5.0f));
	}
}

/// <summary>
/// clears the list and gets rid of anything in the text object for when they play the game again
/// </summary>
void DamageList::clearList()
{
	m_damageTaken.clear();
	m_text.setString(m_baseString);
	m_background.setSize(sf::Vector2f(m_text.getGlobalBounds().width, m_text.getGlobalBounds().height) + sf::Vector2f(10.0f, 10.0f));
	m_background.setPosition(m_text.getPosition() + sf::Vector2f(-5.0f, -5.0f));
}
