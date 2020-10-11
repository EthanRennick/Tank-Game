#include "Menu.h"

Menu::Menu()
{
	initialise();
}

//this will initialise thee three main menu textyures
void Menu::initialise()
{
	if (!menuTexture.loadFromFile("./resources/images/menuTank.png")) //loads menu file
	{
		std::string s("Error loading menu texture"); //if error occurs , display this
		throw std::exception(s.c_str()); //crash the program
	}
	menuSprite.setTexture(menuTexture);
	menuSprite.setPosition(50,600);

	if (!singplayerTexture.loadFromFile("./resources/images/singleplayer.jpg")) //loads singleplayer file
	{
		std::string s("Error loading singleplayer texture"); //if error occurs , display this
		throw std::exception(s.c_str()); //crash the program
	}
	singlePlayerSprite.setTexture(singplayerTexture);
	singlePlayerSprite.setPosition(200, 200);

	if (!multiplayerTexture.loadFromFile("./resources/images/multiplayer.jpg")) //loads multiplayer file
	{
		std::string s("Error loading multiplayer texture"); //if error occurs , display this
		throw std::exception(s.c_str()); //crash the program
	}
	multiplayerSprite.setTexture(multiplayerTexture);
	multiplayerSprite.setPosition(200, 400);

}

//draw all sprites on the menu
void Menu::draw(sf::RenderWindow & window)
{
	window.draw(menuSprite);
	window.draw(singlePlayerSprite);
	window.draw(multiplayerSprite);
}
