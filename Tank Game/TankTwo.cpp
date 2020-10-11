#include "TankTwo.h"
#include <iostream>

/// <summary>
/// @brief Constructor that stores drawable state (texture, sprite) for the tank.
/// Stores references to the texture and container of wall sprites. 
/// Creates sprites for the tank base and turret from the supplied texture.
/// </summary>
/// <param name="texture">A reference to the sprite sheet texture</param>
///< param name="texture">A reference to the container of wall sprites</param>  
////////////////////////////////////////////////////////////
TankTwo::TankTwo(sf::Texture const& texture, std::vector<sf::Sprite>& wallSprites)
	: m_texture(texture)
	, m_wallSprites(wallSprites)
{
	// Initialises the tank base and turret sprites.
	initSprites(sf::Vector2f{ 600,600 });

	for (int i = 0; i < MAX_BULLETS; i++)//initialise bullets
	{
		bulletArray[i].bulletSprite.setPosition(-2000, -2000);
		bulletArray[i].bulletCelocity.x = 0;
		bulletArray[i].bulletCelocity.y = 0;
	}
	setRandomPos();
	healthBar.setFillColor(sf::Color::Green);
	healthBar.setOutlineColor(sf::Color::White);
	healthBar.setOutlineThickness(2);

	fireBuffer.loadFromFile("fire.wav"); //load
	fireSound.setBuffer(fireBuffer); //setup
}

void TankTwo::update(double dt)
{

	healthBar.setSize(sf::Vector2f(health, 10));
	healthBar.setPosition(m_tankBase.getPosition().x, m_tankBase.getPosition().y - 70);
	handleKeyInput(dt); //handle input 
	//update movement
	float newX = m_tankBase.getPosition().x + cos(m_rotation * MathUtility::DEG_TO_RAD) * m_speed * (dt / 1000);
	float newY = m_tankBase.getPosition().y + sin(m_rotation * MathUtility::DEG_TO_RAD) * m_speed * (dt / 1000);
	m_tankBase.setPosition(newX, newY);
	m_tankBase.setRotation(m_rotation);

	setPosition(sf::Vector2f(newX, newY));

	m_speed *= .99;
	if (m_needsRotation == true) //if we need to rotate the turret, do it!
	{
		centreTurret();
	}
	if (checkWallCollision()) //if we hit the wall, bounce
	{
		deflect();
	}

	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (bulletArray[i].free != true) //if it's not free, it must be in use
		{
			bulletArray[i].bulletSprite.move(bulletArray[i].bulletCelocity); //move it

			if (bulletArray[i].bulletSprite.getPosition().x > 1440 || bulletArray[i].bulletSprite.getPosition().x < -5
				|| bulletArray[i].bulletSprite.getPosition().y < -5 || bulletArray[i].bulletSprite.getPosition().y > 1440)
				//if it goes out of bounds
			{
				bulletArray[i].free = true; //bullet goes to storage
			}
		}
	}

	if (m_tankBase.getPosition().x > 1450)
	{
		m_tankBase.setPosition(-10, m_tankBase.getPosition().y);
	}
	if (m_tankBase.getPosition().x < -10)
	{
		m_tankBase.setPosition(1450, m_tankBase.getPosition().y);
	}
	if (m_tankBase.getPosition().y < -10)
	{
		m_tankBase.setPosition(m_tankBase.getPosition().x, 910);
	}
	if (m_tankBase.getPosition().y > 910)
	{
		m_tankBase.setPosition(m_tankBase.getPosition().x, -10);
	}

	//controller
	leftTrigger = { sf::Joystick::getAxisPosition(0, sf::Joystick::Z) };

	rightTrigger = { sf::Joystick::getAxisPosition(0, sf::Joystick::Z) };

	leftThumbStick = { sf::Joystick::getAxisPosition(0, sf::Joystick::X), sf::Joystick::getAxisPosition(0, sf::Joystick::Y) };

	rightThumbStick = { sf::Joystick::getAxisPosition(0, sf::Joystick::U), sf::Joystick::getAxisPosition(0, sf::Joystick::V) };

}

//draw everything
void TankTwo::render(sf::RenderWindow& window)
{
	window.draw(m_tankBase); //draw tank
	window.draw(m_turret); //draw turret
	for (int i = 0; i < MAX_BULLETS; i++) //draw all bullets
	{
		window.draw(bulletArray[i].bulletSprite);
	}
	window.draw(healthBar);
}

void TankTwo::setPosition(sf::Vector2f const& pos)
{
	m_previousPosition = m_tankBase.getPosition();
	m_tankBase.setPosition(pos);
	m_turret.setPosition(pos);
}

//fire a bullet
void TankTwo::fireBullet(double t_dt)
{
	bulletsFired++;
	waitToFireCounter.restart();
	canFire = false;

	for (int i = 0; i < MAX_BULLETS; i++) //look at all bullets
	{
		if (bulletArray[i].free) //if one of them are free
		{
			//bring them to the tank
			bulletArray[i].bulletSprite.setPosition(m_turret.getPosition());
			bulletArray[i].bulletSprite.setRotation(m_turretRotation - 90);

			//give them a direction and velocity (vector)
			lookDirection.x = cos(m_turretRotation * MathUtility::DEG_TO_RAD);
			lookDirection.y = sin(m_turretRotation * MathUtility::DEG_TO_RAD);
			bulletArray[i].bulletCelocity = lookDirection * 3.0f;

			//it is now in use
			bulletArray[i].free = false;
			fireSound.play();
			break;
		}
	}
}

void TankTwo::setRandomPos() //sets tank to a random pos
{
	//give the tank a random position
	sf::Vector2f randomPosition;
	int posPicker = 0;
	posPicker = rand() % 4 + 1; //generate random num
	switch (posPicker) //assign position accordingly 
	{
	case 1:
		posPicker == 1;
		{
			randomPosition = sf::Vector2f(100, 100);
			break;
		}
	case 2:
		posPicker == 2;
		{
			randomPosition = sf::Vector2f(1270, 70); //right corner
			break;
		}
	case 3:
		posPicker == 3;
		{
			randomPosition = sf::Vector2f(100, 800);
			break;
		}
	case 4:
		posPicker == 4;
		{
			randomPosition = sf::Vector2f(1270, 700);
			break;
		}
	}
	m_tankBase.setPosition(randomPosition.x, randomPosition.y);
	m_turret.setPosition(m_tankBase.getPosition());
}

void TankTwo::reset() //reset all features of tank
{
	health = 50;
	bulletsFired = 0;
	m_rotation = { 0.0 };
	m_turretRotation = { 0.0 };
	m_speed = 0;
	setRandomPos();
}

void TankTwo::handleKeyInput(double dt)
{

	//key input
	if (leftThumbStick.x < -20 || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		decreaseRotation();
	}
	if (leftThumbStick.x > 20 || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		increaseRotation();
	}
	if (leftTrigger > 20 || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		decreaseSpeed();
	}
	if (rightTrigger < -20 || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		increaseSpeed();
	}

	if (rightThumbStick.x < -20 || sf::Keyboard::isKeyPressed(sf::Keyboard::Comma))
	{
		decreaseTurretRotation();
	}

	if (rightThumbStick.x > 20 || sf::Keyboard::isKeyPressed(sf::Keyboard::Period))
	{
		increaseTurretRotation();
	}

	if (sf::Joystick::isButtonPressed(0, 9) || sf::Keyboard::isKeyPressed(sf::Keyboard::Slash))
	{
		m_needsRotation = true;
	}

	if ((sf::Joystick::isButtonPressed(0, 0) && canFire) || (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) && canFire)) //if you want to fire a bullet and the countdown is down
	{
		fireBullet(dt); //fire a bullet
	}
	else
	{
		if (waitToFireCounter.getElapsedTime().asSeconds() >= 1)
		{
			canFire = true;
			waitToFireCounter.stop();
		} //timer reset
	}
}

//increast turret rotation
void TankTwo::increaseTurretRotation()
{
	m_turretRotation += 1;
	if (m_turretRotation == 360.0)
	{
		m_turretRotation = 0;
	}
	m_turret.setRotation(m_turretRotation);
}

//turrest rotation decrease
void TankTwo::decreaseTurretRotation()
{

	m_turretRotation -= 1;
	if (m_turretRotation == 0.0)
	{
		m_turretRotation = 359.0;
	}
	m_turret.setRotation(m_turretRotation);
}

//initialise the spirtes
void TankTwo::initSprites(sf::Vector2f const& pos)
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(105, 42, 80, 46);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(122, 2, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);

}

////////////////////////////////////////////////////////////
void TankTwo::increaseSpeed() //go forward
{
	if (m_speed < 100.0)
	{
		m_speed += 1;
	}
}

////////////////////////////////////////////////////////////
void TankTwo::decreaseSpeed() // slow down/go forward
{
	if (m_speed > -100.0)
	{
		m_speed -= 1;
	}
}
///
//tank rotation
////////////////////////////////////////////////////////////
void TankTwo::increaseRotation()
{
	m_previousRotation = m_rotation;
	m_previousTurretRotation = m_turretRotation;
	m_rotation += 1;
	m_turretRotation += 1;
	if (m_rotation == 360.0)
	{
		m_rotation = 0;
	}

	m_turret.setRotation(m_turretRotation);

}

////////////////////////////////////////////////////////////
void TankTwo::decreaseRotation()
{
	m_previousRotation = m_rotation;
	m_previousTurretRotation = m_turretRotation;

	m_rotation -= 1;
	m_turretRotation -= 1;

	if (m_rotation == 0.0)
	{
		m_rotation = 359.0;
	}
	m_turret.setRotation(m_turretRotation);

}

//centres the turret
void TankTwo::centreTurret()
{
	float val = 0.0f;

	if (static_cast<int>(m_turretRotation) < static_cast<int>(m_rotation)) //if it isnt centred move it until it is
	{
		if (m_turretRotation == 360)
		{
			m_turretRotation = 0;
		}
		else
		{
			m_turretRotation += 1;
			m_turret.setRotation(m_turretRotation);
		}
	}
	else if (static_cast<int>(m_turretRotation) > static_cast<int>(m_rotation))
	{
		if (m_turretRotation == 360)
		{
			m_turretRotation = 0;
		}
		else
		{
			m_turretRotation -= 1;
			m_turret.setRotation(m_turretRotation);
		}
	}
	else
	{
		m_needsRotation = false; //it  is now centred, it doesnt need rotation
	}
}



////////////////////////////////////////////////////////////
bool TankTwo::checkWallCollision()
{
	for (sf::Sprite const& sprite : m_wallSprites)
	{
		// Checks if either the tank base or turret has collided with the current wall sprite.
		if (CollisionDetector::collision(m_turret, sprite) ||
			CollisionDetector::collision(m_tankBase, sprite))
		{
			explodeSound.play();
			return true;

		}
	}
	return false;
}

void TankTwo::deflect()
{
	// In case tank was rotating.
	adjustRotation();

	// If tank was moving.
	if (m_speed != 0)
	{
		// Temporarily disable turret rotations on collision.
		m_enableRotation = false;
		// Back up to position in previous frame.
		m_tankBase.setPosition(m_previousPosition);
		// Apply small force in opposite direction of travel.
		if (m_previousSpeed < 0)
		{
			m_speed = 8;
		}
		else
		{
			m_speed = -8;
		}
	}
}

////////////////////////////////////////////////////////////
void TankTwo::adjustRotation()
{
	// If tank was rotating...
	if (m_rotation != m_previousRotation)
	{
		// Work out which direction to rotate the tank base post-collision.
		if (m_rotation > m_previousRotation)
		{
			m_rotation = m_previousRotation - 1;
		}
		else
		{
			m_rotation = m_previousRotation + 1;
		}
	}
	// If turret was rotating while tank was moving
	if (m_turretRotation != m_previousTurretRotation)
	{
		// Set the turret rotation back to it's pre-collision value.
		m_turretRotation = m_previousTurretRotation;
	}
}

//returns bullet sprite
sf::Sprite TankTwo::getBullets(int i)
{
	return bulletArray[i].bulletSprite;
}

//checks for bullet and wall collisions
//takes input from game cpp
//passes to bullet class
void TankTwo::checkBulletWallCollision(std::vector<sf::Sprite> t_wallSprite)
{
	for (int i = 0; i < t_wallSprite.size(); i++)
	{
		for (int j = 0; j < 8; j++)
		{
			bulletArray[j].collisionDetect(t_wallSprite[i]);
		}
	}
}




sf::Sprite TankTwo::getTankSprite()
{
	return m_tankBase;
}

bool TankTwo::getBulletTankCollision(sf::Sprite t_tank) //collision between player two bullet and p1 tank
{
	for (int i = 0; i < 8; i++)
	{
		if (bulletArray[i].collisionDetect(t_tank)) //if so
		{
			return true;
		}
	}
	//else
	return false;

}

void TankTwo::damageHealth()
{
	health -= 10;
}

bool TankTwo::death()
{
	if (health <= 0.0)
	{
		return true;
	}

	return false;
}