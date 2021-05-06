/*
Author: <Wenlong Wang>
Class: ECE6122
Last Date Modified: <2020/11/24>
Description:
"header file of class GameControl"
*/
#pragma once
#include <vector>
#include <cmath>
#include <thread>
#include <mutex>
#include <chrono>
#include <GL/glut.h>
#include "ECE_Ghost.h"
#include "ECE_Pacman.h"
#define ArrayX(x) round(x / len) + 9
#define ArrayZ(z) round(z / len) + 10
const int STARTED = 1;
const int PAUSED = 2;
const int END = 3;
const int PACDIE = 4;
typedef std::chrono::steady_clock::time_point time_point;
class GameControl
{
public:
	GameControl();
	bool powerUpEaten, powerModeStart;
	int gameStatus;
	int pacDeaths;
	std::thread controlThread;
	time_point t_powerModeStart;
	const std::chrono::seconds powerModeDuration;
    int backUpMap[22][19] =
    {
                        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                        {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
                        {1,3,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,3,1},
                        {1,2,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,2,1},
                        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
                        {1,2,1,1,2,1,2,1,1,1,1,1,2,1,2,1,1,2,1},
                        {1,2,2,2,2,1,2,2,2,1,2,2,2,1,2,2,2,2,1},
                        {1,1,1,1,2,1,1,1,0,1,0,1,1,1,2,1,1,1,1},
                        {0,0,0,1,2,1,0,0,0,0,0,0,0,1,2,1,0,0,0},
                        {1,1,1,1,2,1,0,1,1,1,1,1,0,1,2,1,1,1,1}, //line 10
                        {0,0,0,0,2,0,0,1,0,0,0,1,0,0,2,0,0,0,0}, //line 11
                        {1,1,1,1,2,1,0,1,1,1,1,1,0,1,2,1,1,1,1},
                        {0,0,0,1,2,1,0,0,0,0,0,0,0,1,2,1,0,0,0},
                        {1,1,1,1,2,1,0,1,1,1,1,1,0,1,2,1,1,1,1},
                        {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
                        {1,2,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,2,1},
                        {1,3,2,1,2,2,2,2,2,0,2,2,2,2,2,1,2,3,1},
                        {1,1,2,1,2,1,2,1,1,1,1,1,2,1,2,1,2,1,1},
                        {1,2,2,2,2,1,2,2,2,1,2,2,2,1,2,2,2,2,1},
                        {1,2,1,1,1,1,1,1,2,1,2,1,1,1,1,1,1,2,1},
                        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
                        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
                        //
    };
private:
	//float ghostsOriginalColor[4][3];
    friend void gameControl(GameControl& controller);
    void triggerPowerMode();
    void inPowerMode();
    void quitPowerMode();
    void inNormalMode();
    void endGame();
    void startAnotherRound();
};


void gameControl(GameControl& controller);
