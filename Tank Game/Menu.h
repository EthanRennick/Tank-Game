#pragma once
#include <SFML/Graphics.hpp>

//menu class 
class Menu
{
public:
	Menu(); //constructor
	//main menu sprite
	sf::Sprite menuSprite;
	sf::Texture menuTexture;

	//multiplayer button texture
	sf::Texture multiplayerTexture;
	sf::Sprite multiplayerSprite;

	//single player button texture
	sf::Texture singplayerTexture;
	sf::Sprite singlePlayerSprite;

	//bools to control which game mode is active
	bool singlePlayer = false;
	bool multiplayer = false;
	bool active = true;

	//functions to initialise sprites n stuff
	void initialise();
	void draw(sf::RenderWindow & window);

};