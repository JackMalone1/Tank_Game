#pragma once
/// <summary>
/// records what screen that the player is currently on so it knows what it needs to update and render onto the screen
/// </summary>
enum class GameState
{
	Gameplay,
	GameOver,
	MainMenu,
	LevelCreator,
	Project2
};