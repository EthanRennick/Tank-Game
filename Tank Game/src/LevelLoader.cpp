#include "LevelLoader.h"

/// The various operator >> overloads below are non-member functions used to extract
///  the game data from the YAML data structure.

/// <summary>
/// @brief Extracts the obstacle type, position and rotation values.
/// 
/// </summary>
/// <param name="obstacleNode">A YAML node</param>
/// <param name="obstacle">A simple struct to store the obstacle data</param>
////////////////////////////////////////////////////////////
void operator >> (const YAML::Node& obstacleNode, ObstacleData& obstacle)
{
	obstacle.m_type = obstacleNode["type"].as<std::string>();
	obstacle.m_position.x = obstacleNode["position"]["x"].as<float>();
	obstacle.m_position.y = obstacleNode["position"]["y"].as<float>();
	obstacle.m_rotation = obstacleNode["rotation"].as<double>();
}

//extracts target duration, position and rotation
void operator >> (const YAML::Node& targetNode, TargetData& target)
{
	target.m_type = targetNode["type"].as<std::string>();
	target.m_time = targetNode["time"].as<int>();
	int offset = rand() % 4 + 1; //generate a random number
	if (offset == 1) //change the target's position offset accordingly
	{
		target.m_targetPosition.x = targetNode["position"]["x"].as<float>() + 30;
		target.m_targetPosition.y = targetNode["position"]["y"].as<float>() + 30;

	}
	if (offset == 2)
	{
		target.m_targetPosition.x = targetNode["position"]["x"].as<float>() - 30;
		target.m_targetPosition.y = targetNode["position"]["y"].as<float>() - 30;
	}
	if (offset == 3)
	{
		target.m_targetPosition.x = targetNode["position"]["x"].as<float>() + 30;
		target.m_targetPosition.y = targetNode["position"]["y"].as<float>() - 30;
	}
	else
	{
		target.m_targetPosition.x = targetNode["position"]["x"].as<float>() - 30;
		target.m_targetPosition.y = targetNode["position"]["y"].as<float>() + 30;
	}
	target.m_rotation = targetNode["rotation"].as<double>();
}


/// <summary>
/// @brief Extracts the filename for the game background texture.
/// 
/// </summary>
/// <param name="backgroundNode">A YAML node</param>
/// <param name="background">A simple struct to store background related data</param>
////////////////////////////////////////////////////////////
void operator >> (const YAML::Node& backgroundNode, BackgroundData& background)
{
	background.m_fileName = backgroundNode["file"].as<std::string>();
}

/// <summary>
/// @brief Extracts the initial screen position for the player tank.
/// this function is called by:
///	
///			levelNode["tank"] >> level.m_tank;
/// 
/// that line of code ^
/// 
/// the function is used to set the position of the tank.
/// it kinda reads right to left.
/// the 'x' coord, a part of 'position' which is found in the tankNode
/// is assigned to the tank's position variable.
/// 
/// 
/// </summary>
/// <param name="tankNode">A YAML node</param>
/// <param name="tank">A simple struct to store data related to the player tank</param>
////////////////////////////////////////////////////////////
void operator >> (const YAML::Node& tankNode, TankData& tank)
{
	tank.m_position.x = tankNode["position"]["x"].as<float>();
	tank.m_position.y = tankNode["position"]["y"].as<float>();
}

/// <summary>
/// @brief Top level function that extracts various game data from the YAML data stucture.
/// 
/// Invokes other functions to extract the background, tank and obstacle data.
//   Because there are multiple obstacles, obstacle data are stored in a vector.
/// </summary>
/// <param name="tankNode">A YAML node</param>
/// <param name="tank">A simple struct to store data related to the player tank</param>
////////////////////////////////////////////////////////////
void operator >> (const YAML::Node& levelNode, LevelData& level)
{
	levelNode["background"] >> level.m_background;
	levelNode["tank"] >> level.m_tank; //calls the position function for tank

	//obstacle loading
	const YAML::Node& obstaclesNode = levelNode["obstacles"].as<YAML::Node>();
	for (unsigned i = 0; i < obstaclesNode.size(); ++i)
	{
		ObstacleData obstacle;
		obstaclesNode[i] >> obstacle;
		level.m_obstacles.push_back(obstacle);
	}

	//target loading
	const YAML::Node& targetsNode = levelNode["targets"].as<YAML::Node>();
	for (unsigned i = 0; i < targetsNode.size(); ++i)
	{
		TargetData target;
		targetsNode[i] >> target;
		level.m_targets.push_back(target);
	}
}

//error checking
////////////////////////////////////////////////////////////
void LevelLoader::load(int nr, LevelData& level)
{
	std::stringstream ss;
	ss << ".\\resources\\levels\\level1.yaml";  // NB path

	try
	{
		YAML::Node baseNode = YAML::LoadFile(ss.str());
		if (baseNode.IsNull())
		{
			std::string message("File: " + ss.str() + " not found");
			throw std::exception(message.c_str());
		}
		baseNode >> level;
	}
	catch (YAML::ParserException& e)
	{
		std::string message(e.what());
		message = "YAML Parser Error: " + message;
		throw std::exception(message.c_str());
	}
	catch (std::exception& e)
	{
		std::string message(e.what());
		message = "Unexpected Error: " + message;
		throw std::exception(message.c_str());
	}
}


