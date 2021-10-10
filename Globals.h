#pragma once
const int m_NUM_BUTTONS = 4;

enum class ButtonPressed
{
	LevelCreator = 0,
	Gameplay,
	Quit,
	None
};

const sf::Color UNPRESSEED_BUTTON{ 249,26,25,125 };//colour for when a button isn't currently being pressed

const sf::Color PRESSED_BUTTON{ 249,26,25,255 };//colour for when a button is being pressed
