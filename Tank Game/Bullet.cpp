#include "Bullet.h"

Bullet::Bullet() //dafult constructor
{
	if (!bulletTexture.loadFromFile("./resources/images/SpriteSheet.png")) //loads texture file
	{
		std::string s("Error loading bullet texture"); //if error occurs , display this
		throw std::exception(s.c_str()); //crash the program
	}

	sf::IntRect baseRect(5, 176, 10, 10); //sprite sheet coords
	bulletSprite.setTextureRect(baseRect); //set up
	bulletSprite.setTexture(bulletTexture); //set the background sprite to bg texture
	bulletSprite.setOrigin(5, 5);

	bulletPosition={ -2000,-2000 }; //set offscreen storage
	bulletCelocity = { 0, 0 }; //dont move

	free = true; //free to use

	explosionBuffer.loadFromFile("explosion.wav"); //load explosions
	explodeSound.setBuffer(explosionBuffer);//setup
}

bool Bullet::collisionDetect(sf::Sprite t_Sprite) //detect collisions with walls and targets
{
	if (bulletSprite.getGlobalBounds().intersects(t_Sprite.getGlobalBounds())) //if they hit
	{
		//put it back in storage
		free = false;
		bulletSprite.setPosition(sf::Vector2f(-2000, -2000));
		bulletPosition = sf::Vector2f(-2000, -2000);
		bulletCelocity = { 0,0 };
		explodeSound.play(); //sound stuff
		return true; //return true collision
	}
	else
		return false;
}


