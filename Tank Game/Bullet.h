#pragma once
#include <SFML/Graphics.hpp>

#include <SFML/Audio/SoundBuffer.hpp>
#include<SFML/Audio/Sound.hpp>


class Bullet //bullet class
{
public:
	Bullet(); //default constructor
	bool collisionDetect(sf::Sprite t_wallSprite); //detects collisions with walls

	sf::Texture bulletTexture; //bullet's texture (shell)
	sf::Sprite bulletSprite; //bullet sprite
	sf::Vector2f bulletPosition; //it's position
	sf::Vector2f bulletCelocity; //its velocity 
	bool free; //is the bullet in use or not

	sf::SoundBuffer explosionBuffer; //explosion buffer
	sf::Sound explodeSound; //explosion sound
};
