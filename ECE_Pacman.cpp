/*
Author: <Wenlong Wang>
Class : ECE6122
Last Date Modified : <2020 / 11 / 25>
Description :
"implementation of class ECE_Pacman"
*/
#include "ECE_Pacman.h"
#include "GameControl.h"
#include  <GL/glut.h>
#include  <cmath>
extern enum moveFlag pacMoveDir, oldPacMoveDir;
extern GameControl controller;

//constructor, default color is yelow
ECE_Pacman::ECE_Pacman() : colorR(1.0f), colorG(1.0f), colorB(1.0f) 
{
    //setting initial speed and location
    x = 0.0f;
    z = 6 * len;
    speed = 5 * len; //per second
    killed = false;
    countEats = 0;
    totalEatsToWin = 156;
}

//draw a pacman
void ECE_Pacman::drawPacman()
{
    GLfloat mat_color[] = { 1.0, 1.0, 0, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_color);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_color);
    glEnable(GL_COLOR_MATERIAL);

    glColor3f(1.0, 1.0, 0.0); //yellow
    glPushMatrix();
    glTranslatef(this->x, 0, this->z);
    glutSolidSphere(0.5 * len, 20, 20);
    glPopMatrix();
}

//pacman respawn after killed or game ends
void ECE_Pacman::respawn()
{
    //to show pacman is dead, pause the animation for 1s
    Sleep(1000);
    //back to starting point
    this->x = 0.0f;
    this->z = 6 * len;
}
//set color
void ECE_Pacman::setColor(float R, float G, float B)
{
    colorR = R;
    colorG = G;
    colorB = B;
}

//pacman moves one step up
void ECE_Pacman::moveUp()
{
    //check walls. see if moving up one step would crash onto a wall
    int zTemp = round((z - 1*len) / len) + 10;
    int xTemp = round(x / len) + 9;
    //if the intended move is illegal, continue move at the old direction
    if (map[zTemp][xTemp] == WALL)
    {
        pacMoveDir = oldPacMoveDir;
        return;
    }
    else if (map[zTemp][xTemp] == POWERUP)
    {
        controller.powerUpEaten = true;
        countEats++;
    }
    else if (map[zTemp][xTemp] == COIN)
    {
        countEats++;
    }
    //if the move is legal, update coordinates
    z -= 1 * len;
    //set wherever the pacman went to "AIR"
    map[zTemp][xTemp] = AIR;
    //direction changed
    oldPacMoveDir = pacMoveDir;
    //in the displayfunc(),call drawPacman() to move the pacman image up
}
void ECE_Pacman::moveDown()
{
    //check walls
    int zTemp = round((z + len) / len) + 10;
    int xTemp = round(x / len) + 9;
    //if the intended move is illegal, continue move at the old direction
    if (map[zTemp][xTemp] == WALL)
    {
        pacMoveDir = oldPacMoveDir;
        return;
    }
    else if (map[zTemp][xTemp] == POWERUP)
    {
        controller.powerUpEaten = true;
        countEats++;
    }
    else if (map[zTemp][xTemp] == COIN)
    {
        countEats++;
    }
    //update coordinates
    z += 1 * len;
    //set wherever the pacman went to "AIR"
    map[zTemp][xTemp] = AIR;
    //direction changed
    oldPacMoveDir = pacMoveDir;
    //in the displayfunc(),call drawPacman() to move the pacman image up
}
void ECE_Pacman::moveLeft()
{
    //check walls
    int zTemp = round(z/ len) + 10;
    int xTemp = round((x - len) / len) + 9;
    //it's ok to pass special spots
    if (zTemp == 10 && xTemp == -1)
    {
        x = 9 * len;
        return;
    }
    //if the intended move is illegal, continue move at the old direction
    if (map[zTemp][xTemp] == WALL)
    {
        pacMoveDir = oldPacMoveDir;
        return;
    }
    else if (map[zTemp][xTemp] == POWERUP)
    {
        controller.powerUpEaten = true;
        countEats++;
    }
    else if (map[zTemp][xTemp] == COIN)
    {
        countEats++;
    }
    //update coordinates
    x -= 1 * len;
    //set wherever the pacman went to "AIR"
    map[zTemp][xTemp] = AIR;
    //direction changed
    oldPacMoveDir = pacMoveDir;
    //in the displayfunc(),call drawPacman() to move the pacman image up
}
void ECE_Pacman::moveRight()
{
    //check walls
        int zTemp = round(z / len) + 10;
        int xTemp = round((x + len) / len) + 9;
        //it's ok to pass special spots
        if (zTemp == 10 && xTemp == 19)
        {
            x = -9 * len;
            return;
        }
        //if the intended move is illegal, continue move at the old direction
        if (map[zTemp][xTemp] == WALL)
        {
            pacMoveDir = oldPacMoveDir;
            return;
        }
        else if (map[zTemp][xTemp] == POWERUP)
        {
            controller.powerUpEaten = true;
            countEats++;
        }
        else if (map[zTemp][xTemp] == COIN)
        {
            countEats++;
        }
    //update coordinates
    x += 1 * len;
    //set wherever the pacman went to "AIR"
    map[zTemp][xTemp] = AIR;
    //direction changed
    oldPacMoveDir = pacMoveDir;
    //in the displayfunc(),call drawPacman() to move the pacman image up
}
