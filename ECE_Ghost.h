/*Author: <Wenlong Wang>
Class : ECE6122
Last Date Modified : <2020 / 11 / 26>
Description :
header file of class ECE_Ghost
*/
#pragma once
#include <vector>
#include <cmath>
#include <thread>
#include <mutex>
#include <chrono> // std::chrono::milliseconds
#include "GameControl.h"
#include "ECE_Pacman.h"

typedef std::chrono::steady_clock::time_point time_point;
typedef float speed; //speed is represented by m steps / sec.
const speed HIGH = 4;
const speed LOW = 1.5;
extern std::vector<int> findPathToPac(int xStart, int zStart, int xEnd, int zEnd);
extern int map[22][19];


class ECE_Ghost
{
public:
	ECE_Ghost();
	//construct and set color,set a pacman to chase
	ECE_Ghost(float R, float G, float B); 
	void waitForRelease();
	void drawGhost();
	void setColor(float R, float G, float B);
	void die();
	void spawn(); //return the spot number
	void resume();
	friend void ghostAction(ECE_Ghost &ghost, const ECE_Pacman& pac);
	friend void gameControl(GameControl& controller);
	friend class GameControl;
	std::thread ghostThread;
	std::thread::id threadID;
	bool ready4AnotherRound;
	
private:
	//four respawn spot, each containing a (x,z) coordinates
	static float spawnSpot[4][2];
	//flag indicating ghost spots' status
	static bool spotOccupied[4]; 
	static time_point lastRelease;
	static std::chrono::seconds releaseInterval;
	static std::mutex mtx;
	float colorR, colorG, colorB;
	float originalR, originalG, originalB;
	float x, z; //coordinates on x-z plane (y pointing up)
	speed ghostSpeed; //speed is represented by m steps/sec.
	bool killed;
	void move(int dir);
	int spotNum;
	
};
	