#pragma once
#include <SFML/Graphics.hpp>
#include "MathUtility.h"
#include "CollisionDetector.h"
#include "Bullet.h"
#include "Thor/Time/StopWatch.hpp"

#include <SFML/Audio/SoundBuffer.hpp>
#include<SFML/Audio/Sound.hpp>


/// <summary>
/// @brief A simple tank controller.
/// 
/// This class will manage all tank movement and rotations.
/// </summary>
class Tank
{
public:	
	Tank(sf::Texture const & texture, std::vector<sf::Sprite>& wallSprites); //constructor
	void update(double dt); //tank updates
	void render(sf::RenderWindow & window); //draw tank and tank stuff
	void setPosition(sf::Vector2f const & pos); //set the tanks position
	void fireBullet(double dt); //fire a bullet from turret
	void setRandomPos();
	void reset();

	/// <summary>
/// @brief Processes control keys and applies speed/rotation as appropriate.
/// </summary>
	void handleKeyInput(double dt);

	void increaseTurretRotation(); //increases rotation

	void decreaseTurretRotation(); //decreases

	/// <summary>
/// @brief Increases the speed by 1, max speed is capped at 100.
/// 
/// </summary>
	void increaseSpeed();

	/// <summary>
	/// @brief Decreases the speed by 1, min speed is capped at -100.
	/// 
	/// </summary>
	void decreaseSpeed();

	/// <summary>
	/// @brief Increases the rotation by 1 degree, wraps to 0 degrees after 359.
	/// 
	/// </summary>
	void increaseRotation();

	/// <summary>
	/// @brief Decreases the rotation by 1 degree, wraps to 359 degrees after 0.
	/// 
	/// </summary>
	void decreaseRotation();

	void centreTurret(); //centre the turret

	/// <summary>
/// @brief Checks for collisions between the tank and the walls.
/// 
/// </summary>
/// <returns>True if collision detected between tank and wall.</returns>
	bool checkWallCollision();
	
	/// <summary>
/// @brief Stops the tank if moving and applies a small increase in speed in the opposite direction of travel.
/// If the tank speed is currently 0, the rotation is set to a value that is less than the previous rotation value
///  (scenario: tank is stopped and rotates into a wall, so it gets rotated towards the opposite direction).
/// If the tank is moving, further rotations are disabled and the previous tank position is restored.
/// The tank speed is adjusted so that it will travel slowly in the opposite direction. The tank rotation 
///  is also adjusted as above if necessary (scenario: tank is both moving and rotating, upon wall collision it's 
///  speed is reversed but with a smaller magnitude, while it is rotated in the opposite direction of it's 
///  pre-collision rotation).
/// </summary>

	//my functions
	void deflect(); //deflection off of obstacles

	void adjustRotation(); //rotate the turret

	sf::Sprite getBullets(int i); //return bullet sprites

	void checkBulletWallCollision(std::vector<sf::Sprite> t_wallSprite); //checks collisions between wall sprites and bullets

	bool checkBulletTargetCollisions(sf::Sprite t_targetSprite); //checks for collisions between 

	int getTankScore(); //returns the tank's score

	float getBulletsFired(); //returns the amount of bullets the player has shot

	float getTankTargetsDestroyed(); //how many targets the player killed

	bool getBulletTankCollision(sf::Sprite t_tank); //checks for collision between tanks and bullets

	sf::Sprite getTankSprite(); //return's the tank sprite

	void damageHealth(); //reduces tank's health

	bool death(); // the tank is deadddd

	void drawHealth(sf::RenderWindow & window); //draws the healthbar

private:
	// A reference to the container of wall sprites.
	std::vector<sf::Sprite>& m_wallSprites;

	void initSprites(sf::Vector2f const & pos);
	sf::Sprite m_tankBase;
	sf::Sprite m_turret;
	sf::Texture const & m_texture;

	// The tank speed.
	double m_speed{ 0.0 };

	// The current rotation as applied to tank base.
	double m_rotation{ 0.0 };
	double m_turretRotation{ 0.0 }; //to turret

	//has C been pressed? do we need to rotate the turret?
	bool m_needsRotation = false;
	bool m_enableRotation = true;

	//tank's previous data
	sf::Vector2f m_previousPosition; 
	int m_previousSpeed =0;
	double m_previousRotation =0.0f;
	double m_previousTurretRotation =0.0f;

	//bullets
	static const int MAX_BULLETS = 8;
	Bullet bulletArray[MAX_BULLETS];
	sf::Vector2f lookDirection = { 0,0 }; //direction you fire in
	
	//wait to fire counter for tank
	thor::StopWatch waitToFireCounter;
	bool canFire = true; // can fire once again

	float tankScore = 0.0; //targets destroyed
	float bulletsFired = 0; //how many bullets you shot

	int targetsDestroyed = 0; //how many targets you've shot down

	sf::RectangleShape healthBar; //healthbar setup
	float health = 50;

	sf::SoundBuffer fireBuffer; //fire buffer
	sf::Sound fireSound; //fire sound

	sf::SoundBuffer explosionBuffer; //explosion buffer
	sf::Sound explodeSound; //explosion sound
};
