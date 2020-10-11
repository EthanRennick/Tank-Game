#include "Game.h"
#include <iostream>
static double const MS_PER_UPDATE = 10.0;

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_height, ScreenSize::s_width, 32), "A Bridge Too Far", sf::Style::Default)
	, m_tank(m_texture, m_wallSprites), m_tankTwo(m_texture, m_wallSprites)
{
	m_window.setVerticalSyncEnabled(true);

	int currentLevel = 1;

	// Will generate an exception if level loading fails.
	try
	{
		LevelLoader::load(currentLevel, m_level);
	}
	catch (std::exception& e)
	{
		std::cout << "Level Loading failure." << std::endl;
		std::cout << e.what() << std::endl;
		throw e;
	}

	//load background 
	if (!m_bgTexture.loadFromFile("./resources/images/background.jpg")) //loads texture file
	{
		std::string s("Error loading background texture"); //if error occurs , display this
		throw std::exception(s.c_str()); //crash the program
	}
	m_bgSprite.setTexture(m_bgTexture); //set the background sprite to bg texture

	if (!m_spriteSheetTexture.loadFromFile("./resources/images/SpriteSheet.png"))
	{
		std::string errorMsg("Error loading sprite sheet texture");
		throw std::exception(errorMsg.c_str());
	}
	// Now the level data is loaded, set the tank position.

	// Extract the wall image from the spritesheet.
	sf::Sprite sprite;
	sf::IntRect wallRect(2, 129, 33, 23);
	sprite.setTexture(m_spriteSheetTexture);
	sprite.setTextureRect(wallRect);

	// Loop through each Obstacle instance - recall that Obstacles are structs
	for (auto& obstacle : m_level.m_obstacles)
	{
		// Position the wall sprite using the obstacle data
		sprite.setPosition(obstacle.m_position);
		sprite.setRotation(obstacle.m_rotation);
		m_sprites.push_back(sprite);
	}


	if (!m_texture.loadFromFile("./resources/images/SpriteSheet.png")) //loads texture file
	{
		std::string s("Error loading texture"); //if error occurs , display this
		throw std::exception(s.c_str()); //crash the program
	}

	//targets
	if (!m_targetTexture.loadFromFile("./resources/images/target.png"))
	{
		std::string errorMsg("Error loading target texture");
		throw std::exception(errorMsg.c_str());
	}
	sf::Sprite targetSprite;
	targetSprite.setTexture(m_targetTexture);
	for (auto& target : m_level.m_targets)
	{
		targetSprite.setPosition(target.m_targetPosition);
		targetSprite.setRotation(target.m_rotation);
		m_targetSprites.push_back(targetSprite);
	}
	
	//generate walls and targets
	generateWalls();
	generateTargets();

	///setup string timer data
	if (!font.loadFromFile("./resources/fonts/arial.ttf")) //loads font file
	{
		std::string s("Error loading fonts"); //if error occurs , display this
		throw std::exception(s.c_str()); //crash the program
	}
	//all sorts of text the game uses is initialised here
	timeText.setString("Time Remaining: ");
	timeText.setCharacterSize(24);
	timeText.setPosition(2, 0);
	timeText.setFont(font);

	scoreText.setString("Targets Destroyed: ");
	scoreText.setCharacterSize(24);
	scoreText.setPosition(2, 0);
	scoreText.setFont(font);
	scoreText.setPosition(800, 0);

	currentGrade.setCharacterSize(30);
	currentGrade.setPosition(500, 200);
	currentGrade.setFont(font);

	previousGrade.setCharacterSize(26);
	previousGrade.setPosition(500, 600);
	previousGrade.setFont(font);

	//warning system
	if (!m_warningTexture.loadFromFile("./resources/images/attention.png"))
	{
		std::string errorMsg("Error loading warning system texture");
		throw std::exception(errorMsg.c_str());
	}
	m_warningSprite.setTexture(m_warningTexture);

	//arrow sprite setups!
	if (!arrowTexture.loadFromFile("./resources/images/arrow.png"))
	{
		std::string errorMsg("Error loading arrow texture");
		throw std::exception(errorMsg.c_str());
	}
	arrowSprite.setTexture(arrowTexture);
}

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

		if (menu.singlePlayer && !gameOver) //if game's still going, display all
		{
			m_countdownTimer -= dt; //the count down timer decreases
			thorStopWatch.restart(); //resets the stop watch for targets

			timeText.setString("Time Remaining : " + std::to_string(static_cast<int>(m_countdownTimer.asSeconds())) + "\nPress Enter for menu."); //game over
			
			if (m_countdownTimer < sf::seconds(0.0)) //if time is out for the game
			{
				gameOver = true; //game is over!
				timeText.setString("Game Over. Press Esc to close.\nPress Enter to restart.\nPress Backspace to go back to menu."); //game over
				calculateStats(); //calculate end game stuff
			}

			if (timeLeft) //if you shot a target and there was time left, add that time to next target's duration (it stacks)
			{
				leftOverTime = 8 - static_cast<int>(thorStopWatch.getElapsedTime().asSeconds()); //reset target duration
				timeLeft = false;
				whatTargetToDraw = (whatTargetToDraw + 1) % m_targetSprites.size(); //up the index
				thorStopWatch.restart(); //reset thing
				alarmActive = false; //disable alarm
				m_warningSprite.setPosition(-2000, -2000); //set its position to storage
			}

			if (static_cast<int>(thorStopWatch.getElapsedTime().asSeconds()) == 6 + leftOverTime) //if time is running out, activate alarm
			{
				alarmActive = true; //alarm is alarming
				m_warningSprite.setPosition(m_targetSprites[whatTargetToDraw].getPosition().x + 20, m_targetSprites[whatTargetToDraw].getPosition().y + 20); //set its position
			}

			if (static_cast<int>(thorStopWatch.getElapsedTime().asSeconds()) == 8 + leftOverTime) //if time runs out, reset all things
			{
				whatTargetToDraw = (whatTargetToDraw + 1) % m_targetSprites.size(); //up the index
				std::cout << "8 seconds elapsed" << std::endl;
				thorStopWatch.restart();
				leftOverTime = 0;
				alarmActive = false; //disable alarm
				m_warningSprite.setPosition(-2000, -2000); //set its position to storage
			}
			scoreText.setString("Targets Destroyed: " + std::to_string(m_tank.getTankScore())); //updates ui to show targets destroyed

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) //if i press enter from singleplayer, go back to menu
			{
				//resets all game shit
				gameOver = false;
				m_countdownTimer = sf::seconds(61.0f);
				m_tank.setRandomPos();
				m_tank.reset();
				thorStopWatch.reset();
				leftOverTime = 0;
				whatTargetToDraw = 0;
				newGame = true;

				//go to menu
				menu.active = true;
				menu.singlePlayer = false;
				menu.multiplayer = false;
			}
		}
		else if (gameOver) //if the game is over
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) //if the game is over also go back to main menu
			{
				gameOver = false;
				m_countdownTimer = sf::seconds(61.0f);
				m_tank.setRandomPos();
				m_tank.reset();
				thorStopWatch.restart();
				leftOverTime = 0;
				whatTargetToDraw = 0;
				newGame = true;
			}
		}
		if (menu.multiplayer && !multiplayerVictory) //if the mutliplayer game has won go back to main menu if enter pressed
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			{
				gameOver = false;
				m_countdownTimer = sf::seconds(61.0f);
				m_tank.setRandomPos();
				m_tank.reset();
				thorStopWatch.reset();
				leftOverTime = 0;
				whatTargetToDraw = 0;

				menu.active = true;
				menu.singlePlayer = false;
				menu.multiplayer = false;

				m_tankTwo.reset();
				m_tankTwo.setRandomPos();
			}
		}
		else if (menu.multiplayer && multiplayerVictory)
		{
			timeText.setString("Game Over. Press Esc to close.\nPress Enter to restart."); //game over
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			{
				//reset all game data for tanks
				gameOver = false;
				m_countdownTimer = sf::seconds(61.0f);
				m_tank.setRandomPos();
				m_tank.reset();
				thorStopWatch.reset();
				leftOverTime = 0;
				whatTargetToDraw = 0;

				menu.active = true;
				menu.singlePlayer = false;
				menu.multiplayer = false;

				m_tankTwo.reset();
				m_tankTwo.setRandomPos();

				multiplayerVictory = false;
			}
		}
		render(); //render everything
	}
}

////////////////////////////////////////////////////////////
void Game::processEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
		processGameEvents(event);
	}
}

//////////////////////////////////////////////////////////////
void Game::processGameEvents(sf::Event& event)
{
	// check if the event is a a mouse button release
	if (sf::Event::KeyPressed == event.type)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) //close window if i press Esc 
		{
			m_window.close();
		}
	}
	if (sf::Event::MouseButtonReleased == event.type && menu.active) //if i click and release
	{	
		if (sf::Mouse::Left == event.mouseButton.button) //if it touches a menu thing, then go into it
		{
			if (arrowSprite.getGlobalBounds().intersects(menu.singlePlayerSprite.getGlobalBounds())) //singleplayer menu
			{
				menu.active = false;
				menu.singlePlayer = true;
			}

			if (arrowSprite.getGlobalBounds().intersects(menu.multiplayerSprite.getGlobalBounds())) //mulitplayer menu
			{
				menu.active = false;
				menu.multiplayer = true;
				m_tank.setPosition(sf::Vector2f(100, 100)); //setup position
			}
		}
	}
}

////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	if (!gameOver) //if the game isnt over
	{
		if (menu.active) //if you're in the menu, put the sprite on the cursor
		{
			arrowSprite.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(m_window)));
		}
		else if (menu.singlePlayer) //if youre in a singleplayer game
		{
			//update singleplayer info
			m_tank.update(dt); //update tank
			m_tank.checkBulletWallCollision(m_wallSprites); //pass wallsprites to the tank function to check for collisions
			if (m_tank.checkBulletTargetCollisions(m_targetSprites[whatTargetToDraw])) //checks to see if targets are hit by bullets
			{
				timeLeft = true; //there has been time left between targets being destroyed 
			}
			
		}
		else if(menu.multiplayer && !multiplayerVictory) //if youre in a multiplayer game and no one has won
		{
			//update multiplayer stuff
			m_tank.update(dt); //update tank
			m_tank.checkBulletWallCollision(m_wallSprites); //pass wallsprites to the tank function to check for collisions
			if (m_tank.getBulletTankCollision(m_tankTwo.getTankSprite())) //if you shoot the tank, damage enemy
			{
				m_tankTwo.damageHealth();
			}
			if (m_tank.death()) //if his health is depleted, he dies
			{
				multiplayerVictory = true;
			}

			m_tankTwo.update(dt); //update tank
			m_tankTwo.checkBulletWallCollision(m_wallSprites); //pass wallsprites to the tank function to check for collisions
			if (m_tankTwo.getBulletTankCollision(m_tank.getTankSprite())) //if you shoot the tank, damage enemy
			{
				m_tank.damageHealth();
			}
			if (m_tankTwo.death())  //if his health is depleted, he dies
			{
				multiplayerVictory = true;
			}
		}
	}
}

////////////////////////////////////////////////////////////
void Game::render() //drawing everything
{
	m_window.clear(sf::Color(0, 0, 0, 0));
	m_window.draw(m_bgSprite); //draw background

	if (menu.active) //if in the menu, draw menu sprites
	{
		menu.draw(m_window);
		m_window.draw(arrowSprite);
	}
	else if (menu.singlePlayer) //if he's in singleplayer draw singleplayer stuff
	{

		for (int i = 0; i < m_sprites.size(); i++)
		{
			m_window.draw(m_sprites[i]); //draw all walls
		}
		m_window.draw(m_targetSprites[whatTargetToDraw]); //draw the specified target
		if (alarmActive) //if the alarm is in use
		{
			m_window.draw(m_warningSprite); //draw the alarm if necessary
		}
		if (gameOver) //if the game is over
		{
			m_window.draw(currentGrade);  //draw his current grade earned
			if (newGame) //draw his last grade if 
			{
				m_window.draw(previousGrade); //draw his last grade
			}
		}

		m_tank.render(m_window); //draw tank
		m_window.draw(timeText); //draw the count down timer
		m_window.draw(scoreText); //draw score text
	}
	else
	{
		for (int i = 0; i < m_sprites.size(); i++)
		{
			m_window.draw(m_sprites[i]); //draw all walls
		}

		//draw tank healthbars
		m_tank.drawHealth(m_window); 
		m_tankTwo.render(m_window);

		if (multiplayerVictory) //if multiplayer victory draw the game result
		{
			m_window.draw(timeText);
		}
	}

	m_window.display(); //display it all
}

////////////////////////////////////////////////////////////
void Game::generateWalls()
{
	sf::IntRect wallRect(2, 129, 33, 23);
	// Create the Walls 
	for (ObstacleData const& obstacle : m_level.m_obstacles)
	{
		//sets up every thing
		sf::Sprite sprite;
		sprite.setTexture(m_texture);
		sprite.setTextureRect(wallRect);
		sprite.setOrigin(wallRect.width / 2.0, wallRect.height / 2.0);
		sprite.setPosition(obstacle.m_position);
		sprite.setRotation(obstacle.m_rotation);
		m_wallSprites.push_back(sprite);
	}
}

//creates the targets and pushed them to vector
void Game::generateTargets()
{
	// Create the targets 
	for (TargetData const& target : m_level.m_targets)
	{
		//set up all targets
		sf::Sprite targetSprite;
		targetSprite.setTexture(m_targetTexture);
		targetSprite.setPosition(target.m_targetPosition);
		targetSprite.setRotation(target.m_rotation);
		m_targetSprites.push_back(targetSprite);
	}
}

void Game::calculateStats() //calculates the result of games
{
	//setup previous string
	previousGrade.setString("Previous mark was: " + std::to_string(displayGrade)+ "\nPrevious accuracy was: " + std::to_string(displayAccuracy));

	//maths stuff to get grade and accuracy
	accuracy = m_tank.getTankTargetsDestroyed() / m_tank.getBulletsFired();
	calcGrade = (m_tank.getTankTargetsDestroyed() / 12 * 100)  * accuracy;

	//the int values we output
	displayAccuracy = m_tank.getTankTargetsDestroyed() / m_tank.getBulletsFired() * 100;
	displayGrade = calcGrade;

	//determines the correct mark for the user and assigns correct string
	if (calcGrade < 40)
	{
		currentGrade.setString("You scored: " + std::to_string(calcGrade) + "%.\nYour grade was: F\nYou have failed the simulation.\nRegards, High Command.\nYour accuracy was: " + std::to_string(displayAccuracy) + "%.\n");
	}
	else if (calcGrade > 40 && calcGrade < 55)
	{
		currentGrade.setString("You scored: " + std::to_string(calcGrade) + "%.\nYour grade was: D\nYou have passed the simulation.\nRegards, High Command.\nYour accuracy was: " + std::to_string(displayAccuracy) + "%.\n");
	}
	else if (calcGrade > 55  && calcGrade < 70)
	{
		currentGrade.setString("You scored: " + std::to_string(calcGrade) + "%.\nYour grade was: C\nYou have passed the simulation with a passing mark.\nSincerely, High Command.\nYour accuracy was: " + std::to_string(displayAccuracy) + "%.\n");
	}
	else if (calcGrade > 70 && calcGrade < 85)
	{
		currentGrade.setString("You scored: " + std::to_string(calcGrade) + "%.\nYour grade was: B\nYou have passed the simulation with a merit.\n Well Done. Regards, High Command.\nYour accuracy was: " + std::to_string(displayAccuracy) + "%.\n");
	}
	else if (calcGrade > 100) //error checking to ensure we dont have more than 100% which is impossible
	{
		calcGrade = 100.0;
		displayGrade = calcGrade;
		//grade string 
		currentGrade.setString("You scored: " + std::to_string(calcGrade) + "%.\nYour grade was: A\nYou have passed the simulation with a distinction.\n Congratulations. Regards, High Command.\nYour accuracy was: " + std::to_string(displayAccuracy) + "%.\n");
	}
}



