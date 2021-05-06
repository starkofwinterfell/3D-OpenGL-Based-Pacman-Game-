/*
Author: <Wenlong Wang>
Class : ECE6122
Last Date Modified : <2020 / 11 / 24>
Description :
"the header file of ECE_Pacman class"
*/
#pragma once

#define len 0.5f //the length from one coin to another
#define AIR 0
#define WALL 1
#define COIN 2
#define POWERUP 3
extern int map[22][19];
class ECE_Ghost;
class GameControl;
class ECE_Pacman
{
public:
	ECE_Pacman();
	void drawPacman();
	void setColor(float R, float G, float B);
	void moveUp();
	void moveDown();
	void moveLeft();
	void moveRight();
	void respawn();
	friend void ghostAction(ECE_Ghost &ghost, const ECE_Pacman &pac);
	friend void gameControl(GameControl& controller);
	friend class GameControl;
	int countEats;
	int totalEatsToWin;
	bool enableKey;
private:
	float colorR, colorG, colorB; //color
	float x, z; //coordinates on x-z plane (y pointing up)
	float speed; //default is 5 steps / sec
	bool killed;
};

