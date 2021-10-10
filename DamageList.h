#pragma once

#include <list>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <iterator>

#include "ScreenSize.h"

enum class DamageType
{
	TopSpeed,
	TurnRadius,
	BrakeSpeed,
	Acceleration
};


class DamageList
{
private:
	std::list<DamageType> m_damageTaken;
	sf::Font m_font;
	sf::Text m_text;
	sf::RectangleShape m_background;
	const std::string m_baseString;
public:
	DamageList() = delete;
	DamageList(std::string t_fontPath, std::string t_textData);
	void render(sf::RenderWindow& t_window);
	void appendList(DamageType t_type, std::string t_damageInfo);
	void clearList();
	void clearText(std::string t_defaultText) { m_text.setString(t_defaultText); }
	std::list<DamageType> getList() { return m_damageTaken; };
};

