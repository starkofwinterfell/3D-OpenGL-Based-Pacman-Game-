/*
Author: <Wenlong Wang>
Class: ECE6122
Last Date Modified: <2020/11/24>
Description:
"class GameControl, monitor and control the game flow"
*/

#include "GameControl.h"
#include <iostream>

extern ECE_Pacman myPacman;
extern std::vector<ECE_Ghost*> ghosts;
extern int map[22][19];
extern void ghostAction(ECE_Ghost& ghost, const ECE_Pacman& pac);
GameControl controller;


//game controller thread func
void gameControl(GameControl& controller)
{
    while (1)
    {
        //wait for the game to start
        while (controller.gameStatus != STARTED)
        {
            Sleep(200);
        }
        //check if we should enter power-mode
        if (!controller.powerModeStart && controller.powerUpEaten )
        {
            controller.triggerPowerMode();
        }
        //if already in power-up mode (somewhere between the 5s)
        if (controller.powerModeStart)
        {
            controller.inPowerMode();
        }
        //if not in power-up mode, check if pacman should be dead
        else
        {
            controller.inNormalMode();
        }
        //check if pacman won
        if (myPacman.countEats == myPacman.totalEatsToWin)
        {
            std::cout << "CONGRATS, YOU WON !!!" << std::endl<< std::endl;
            controller.endGame();
        }
        
    }
    
}

//trigger power-up mode
void GameControl::triggerPowerMode()
{
    //if a power-up mode is triggered, set all ghosts to white
    //and slow the ghosts down, start counting 5s
    for (auto gh : ghosts)
    {
        gh->setColor(1.0f, 1.0f, 1.0f);
        gh->ghostSpeed = LOW;
    }
    controller.powerModeStart = true;
    controller.t_powerModeStart = std::chrono::steady_clock::now();
}

//handling power-up mode game control
void GameControl::inPowerMode()
{
    //eat another pill? then start over the 5s counting 
    if (controller.powerUpEaten)
    {
        controller.t_powerModeStart = std::chrono::steady_clock::now();
        controller.powerUpEaten = false;
    }
    //check if any ghost should be dead
    for (int i = 0; i < 4; i++)
    {
        if (ArrayX(ghosts[i]->x) == ArrayX(myPacman.x) && ArrayZ(ghosts[i]->z) == ArrayZ(myPacman.z))
        {
            ghosts[i]->killed = true;
            ghosts[i]->setColor(0, 0, 0); //dead, body is invisible
            ghosts[i]->x = 30 * len; //dead, move it out of scene
            ghosts[i]->z = 50 * len;
        }
    }
    //if 5s ellapsed, quit power-up mode
    time_point rightNow = std::chrono::steady_clock::now();
    if (rightNow - controller.t_powerModeStart >= controller.powerModeDuration)
    {
        quitPowerMode();
    }
}

//quit power-up mode
void GameControl::quitPowerMode()
{
    //return to normal mode,use original color for living ghosts
    for (auto gh : ghosts)
    {
        if (gh->killed)
            continue;
        gh->setColor(gh->originalR, gh->originalG, gh->originalB);
        gh->ghostSpeed = HIGH;
    }
    //flag reset
    controller.powerModeStart = false;
    controller.powerUpEaten = false;
}

//handling normal mode
void GameControl::inNormalMode()
{
    //always check whether pacman meets a ghost (killed)
    for (int i = 0; i < 4; i++)
    {
        //if pacman is killed, start another round or end game
        if (ArrayX(ghosts[i]->x) == ArrayX(myPacman.x) &&
            ArrayZ(ghosts[i]->z) == ArrayZ(myPacman.z))
        {
            myPacman.killed = true;
            controller.pacDeaths++;
            controller.gameStatus = PACDIE;

            if (controller.pacDeaths == 3)
            {
                std::cout << "Pacman deaths : 3" << std::endl;
                std::cout << "GAME OVER, YOU LOST ..." << std::endl <<std::endl;
                controller.endGame();
            }   
            else
            {
                std::cout << "Pacman deaths : " <<controller.pacDeaths << std::endl;
                controller.startAnotherRound();
            }
                
            break;
        }
    }
}

//end game
void GameControl::endGame()
{
    controller.gameStatus = END;
    controller.pacDeaths = 0;
    //reset map
    for (int i = 0; i < 21; i++)
        for (int j = 0; j < 18; j++)
        {
            map[i][j] = controller.backUpMap[i][j];
        }
    //wait if we can't restart game
    while (!ghosts[0]->ready4AnotherRound || !ghosts[1]->ready4AnotherRound ||
        !ghosts[2]->ready4AnotherRound || !ghosts[3]->ready4AnotherRound)
    {
        Sleep(200);
    }
    //here, ready to restart
    //let ghosts resume their status, and go catch pacman
    for (auto gh : ghosts)
    {
        gh->resume();
        gh->ghostThread.detach();
        gh->ghostThread = std::thread(ghostAction, std::ref(*gh), std::ref(myPacman));
    }
    myPacman.respawn();
    myPacman.countEats = 0;
    myPacman.killed = false;
    controller.gameStatus = PAUSED;
}

//when pacman is killed,start again
void GameControl::startAnotherRound()
{
    //wait if we can't start another round
    while (!ghosts[0]->ready4AnotherRound || !ghosts[1]->ready4AnotherRound ||
        !ghosts[2]->ready4AnotherRound || !ghosts[3]->ready4AnotherRound)
    {
        Sleep(200);
    }

    //here, ready to start another round
    //let ghosts resume their status, and go catch pacman
    for (auto gh : ghosts)
    {
        gh->resume();
        gh->ghostThread.detach();
        gh->ghostThread = std::thread(ghostAction, std::ref(*gh), std::ref(myPacman));
    }
    myPacman.respawn();
    myPacman.killed = false;
    controller.gameStatus = PAUSED;
}

//constructor
GameControl::GameControl():powerModeDuration(5)
{
    gameStatus = PAUSED;
}
