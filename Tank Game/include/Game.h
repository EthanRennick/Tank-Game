#pragma once

#include <SFML/Graphics.hpp>
#include "ScreenSize.h"
#include "LevelLoader.h"
#include "Tank.h"
#include "Bullet.h"
#include "Thor/Time/StopWatch.hpp"
#include "TankTwo.h"
#include "Menu.h"


/// <summary>
/// @author RP
/// @date June 2017
/// @version 1.0
/// 
/// @author two, Ethan Rennick - C00240102
/// 20/9/19
/// project tank
/// </summary>

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
	/// <summary>
	/// @brief Placeholder to perform updates to all game objects.
	/// </summary>
	/// <param name="time">update delta time</param>
	void update(double dt);

	/// <summary>
	/// @brief Draws the background and foreground game objects in the SFML window.
	/// The render window is always cleared to black before anything is drawn.
	/// </summary>
	void render();

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

	// main window
	sf::RenderWindow m_window;

	//this must belong to game class, not limited to the constructor
	sf::Texture m_texture; //creates a texture variable

	// Load the game level data.
	LevelData m_level;
	sf::Texture m_bgTexture;
	sf::Sprite m_bgSprite;

	std::vector<sf::Sprite>  m_sprites;
	// A texture for the sprite sheet
	sf::Texture m_spriteSheetTexture;

	//target data
	sf::Texture m_targetTexture;

	//lab 3
	Tank m_tank;

	//lab 5
	/// <summary>
/// @brief Creates the wall sprites and loads them into a vector.
/// Note that sf::Sprite is considered a light weight class, so 
///  storing copies (instead of pointers to sf::Sprite) in std::vector is acceptable.
/// </summary>
	void generateWalls();

	/// <summary>
	/// creates the targets and loads them into a vector
	/// //sf::Sprite is lightweight?
	/// //copies are okay i guess
	/// </summary>
	void generateTargets();

	// Wall sprites
	std::vector<sf::Sprite> m_wallSprites;

	//target sprites
	std::vector<sf::Sprite> m_targetSprites;

	//game timer
	sf::Text timeText;
	sf::Font font;
	bool gameOver = false;

	//deciding what thing to draw
	//a  lot of timers
	int whatTargetToDraw = 0; //used as an index in the target vector
	int targetTimer = 0;                                
	int targetIncrement = 0; //this is the length of which targets appear on screen
	int targetClock = 0;
	int targetClockTimer = 0; //used to check if the alarm is about to disappear

	sf::Sprite m_warningSprite; //alarm sprite
	sf::Texture m_warningTexture; //alarm's texture
	bool alarmActive = false; //is the alarm in use?

	sf::Text scoreText;

	int leftOverTime = 0;
	bool timeLeft = false;

	sf::Time m_countdownTimer = sf::seconds(60.0f);
	thor::StopWatch thorStopWatch;

	//player stats at end of game
	sf::String playerStats;
	sf::Text previousGrade;
	sf::Text currentGrade;
	float calcGrade = 0.0;
	float accuracy = 0.0;
	int displayAccuracy = 0;
	int displayGrade = 0;
	bool newGame = false;

	void calculateStats();


	//playerTwo
	TankTwo m_tankTwo;

	//creates a menu class
	Menu menu;
	
	//menu cursor sprite
	sf::Texture arrowTexture;
	sf::Sprite arrowSprite;

	bool multiplayerVictory = false; //controls the multiplayer win condition

};

