#include "Tank.h"
#include <iostream>

/// <summary>
/// @brief Constructor that stores drawable state (texture, sprite) for the tank.
/// Stores references to the texture and container of wall sprites. 
/// Creates sprites for the tank base and turret from the supplied texture.
/// </summary>
/// <param name="texture">A reference to the sprite sheet texture</param>
///< param name="texture">A reference to the container of wall sprites</param>  
////////////////////////////////////////////////////////////
Tank::Tank(sf::Texture const& texture, std::vector<sf::Sprite>& wallSprites)
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
	//healthbar initialise
	healthBar.setFillColor(sf::Color::Green);
	healthBar.setOutlineColor(sf::Color::White);
	healthBar.setOutlineThickness(2);

	//sound initialise
	fireBuffer.loadFromFile("fire.wav"); //load
	fireSound.setBuffer(fireBuffer); //setup
}

void Tank::update(double dt)
{	
	//healthbar update
	healthBar.setSize(sf::Vector2f(health, 10)); //set size
	healthBar.setPosition(m_tankBase.getPosition().x, m_tankBase.getPosition().y - 70); //update position

	handleKeyInput(dt); //handle input 

	//update movement
	float newX = m_tankBase.getPosition().x + cos(m_rotation * MathUtility::DEG_TO_RAD) * m_speed * (dt/1000);
	float newY = m_tankBase.getPosition().y + sin(m_rotation * MathUtility::DEG_TO_RAD) * m_speed * (dt / 1000);
	m_tankBase.setPosition(newX, newY);
	m_tankBase.setRotation(m_rotation);
	setPosition(sf::Vector2f(newX, newY));

	m_speed *= .99; //decrease speed
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

	//world boundaries checking
	//allows map scrolling
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
}

//draw everything
void Tank::render(sf::RenderWindow & window) 
{
	window.draw(m_tankBase); //draw tank
	window.draw(m_turret); //draw turret
	for (int i = 0; i < MAX_BULLETS; i++) //draw all bullets
	{
		window.draw(bulletArray[i].bulletSprite);
	}
}

//tank position setting
void Tank::setPosition(sf::Vector2f const& pos)
{
	m_previousPosition = m_tankBase.getPosition();
	m_tankBase.setPosition(pos);
	m_turret.setPosition(pos);
}

//fire a bullet
void Tank::fireBullet(double t_dt)
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

void Tank::setRandomPos() //sets tank to a random pos
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
			randomPosition = sf::Vector2f(100, 100); //top left
			break;
		}
	case 2:
		posPicker == 2;
		{
			randomPosition = sf::Vector2f(1270, 70); //top right corner
			break;
		}
	case 3:
		posPicker == 3;
		{
			randomPosition = sf::Vector2f(100, 800); //bttom left
			break;
		}
	case 4:
		posPicker == 4;
		{
			randomPosition = sf::Vector2f(1270, 700); //bottom right
			break;
		}
	}
	//move it to position
	m_tankBase.setPosition(randomPosition.x, randomPosition.y);
	m_turret.setPosition(m_tankBase.getPosition());
}

void Tank::reset() //reset all its stats
{
	health = 50;
	tankScore = 0.0;
	bulletsFired = 0;
	m_rotation={ 0.0 };
	m_turretRotation={ 0.0 };
	targetsDestroyed = 0;
}

void Tank::handleKeyInput(double dt)
{
	//key input
	//tank rotation
	if ( sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		decreaseRotation();
	}
	if ( sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		increaseRotation();
	}
	//forwards backwards tank move
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		increaseSpeed();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		decreaseSpeed();
	}
	//turret rotation
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		decreaseTurretRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
	{
		increaseTurretRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		m_needsRotation=true;
	}
	
	//bullet firing
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && canFire) //if you want to fire a bullet and the countdown is down
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
void Tank::increaseTurretRotation()
{
	m_turretRotation += 1;
	if (m_turretRotation == 360.0)
	{
		m_turretRotation = 0;
	}
	m_turret.setRotation(m_turretRotation);
}

//turrest rotation decrease
void Tank::decreaseTurretRotation()
{

	m_turretRotation -= 1;
	if (m_turretRotation == 0.0)
	{
		m_turretRotation = 359.0;
	}
	m_turret.setRotation(m_turretRotation);
}

//initialise the spirtes
void Tank::initSprites(sf::Vector2f const & pos)
{
	// Initialise the tank base
	m_tankBase.setTexture(m_texture);
	sf::IntRect baseRect(2, 43, 79, 43);
	m_tankBase.setTextureRect(baseRect);
	m_tankBase.setOrigin(baseRect.width / 2.0, baseRect.height / 2.0);

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(19, 1, 83, 31);
	m_turret.setTextureRect(turretRect);
	m_turret.setOrigin(turretRect.width / 3.0, turretRect.height / 2.0);

}

void Tank::damageHealth() //lower health bar
{
	health -= 10;
}

bool Tank::death() //if his health is less than 0 he's dead
{
	if (health <= 0.0)
	{
		return true;
	}

	return false;
}

void Tank::drawHealth(sf::RenderWindow& window) //draw tank details
{
	window.draw(m_tankBase); //draw tank
	window.draw(m_turret); //draw turret
	for (int i = 0; i < MAX_BULLETS; i++) //draw all bullets
	{
		window.draw(bulletArray[i].bulletSprite);
	}
	window.draw(healthBar); //draw healthbar
}

////////////////////////////////////////////////////////////
void Tank::increaseSpeed() //go forward
{
	if (m_speed < 100.0)
	{
		m_speed += 1;
	}
}

////////////////////////////////////////////////////////////
void Tank::decreaseSpeed() // slow down/go forward
{
	if (m_speed > -100.0)
	{
		m_speed -= 1;
	}
}
///
//tank rotation
////////////////////////////////////////////////////////////
void Tank::increaseRotation()
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
void Tank::decreaseRotation()
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
void Tank::centreTurret()
{
	float val = 0.0f;

	//if it isnt centred move it until it is
	if (static_cast<int>(m_turretRotation) < static_cast<int>(m_rotation))
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
bool Tank::checkWallCollision()
{
	for (sf::Sprite const& sprite : m_wallSprites)
	{
		// Checks if either the tank base or turret has collided with the current wall sprite.
		if (CollisionDetector::collision(m_turret, sprite) ||
			CollisionDetector::collision(m_tankBase, sprite))
		{
			explodeSound.play(); //soundeffect
			return true;
		}
	}
	return false;
}

void Tank::deflect()
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
void Tank::adjustRotation()
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
sf::Sprite Tank::getBullets(int i)
{
	return bulletArray[i].bulletSprite;
}

//checks for bullet and wall collisions
//takes input from game cpp
//passes to bullet class
void Tank::checkBulletWallCollision(std::vector<sf::Sprite> t_wallSprite) //
{
	for (int i = 0; i < t_wallSprite.size(); i++)
	{
		for (int j = 0; j < 8; j++)
		{
			bulletArray[j].collisionDetect(t_wallSprite[i]);
		}
	}
}

//checks to see if bullets hit targets
//returns true or false
//looks at all bullets and sees if any hits the target
bool Tank::checkBulletTargetCollisions(sf::Sprite t_targetSprite)
{
	for (int j = 0; j < 8; j++)
	{
		if (bulletArray[j].collisionDetect(t_targetSprite) == true) //if target hit definitely occured
		{
			targetsDestroyed++; //tank score increases because he shot a target
			tankScore++; //score goes up
			explodeSound.play(); //explode sound
			return true;
		}
	}

	return false;
}

int Tank::getTankScore() //returns tank score - used for target destroying
{
	return targetsDestroyed;
}

float Tank::getBulletsFired() //see how many bullets he's fired
{
	return bulletsFired;
}

float Tank::getTankTargetsDestroyed() //how many targets he's destroyed
{
	return tankScore;
}

bool Tank::getBulletTankCollision(sf::Sprite t_tank) //check for collision between player one and tank
{
	for (int i = 0; i < 8; i++)
	{
		if (bulletArray[i].collisionDetect(t_tank))
		{
			explodeSound.play(); //sounddddd
			return true;
		}
	}
	return false;
}

sf::Sprite Tank::getTankSprite() //return tank body
{
	return m_tankBase;
}
