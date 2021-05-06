/*
Author: <Wenlong Wang>
Class: ECE6122
Last Date Modified: <2020/11/25>
Description:
"the main file of the pacman game"
*/

#include <stdlib.h>
#include <GL/glut.h>
#include "ECE_Pacman.h"
#include "ECE_Ghost.h"
#include "gameControl.h"
#include <thread>

//the map of the maze
int map[22][19] = {
    //
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

ECE_Pacman myPacman;
ECE_Ghost ghost1(0.0f, 0.96f, 1.0f); //green-colored ghost
ECE_Ghost ghost2(1.0f, 0.75f, 0.79f); //pink-colored ghost
ECE_Ghost ghost3(0.85f, 0.65f, 0.13f); //orange-colored ghost
ECE_Ghost ghost4(1.0f, 0.0f, 0.0f); //red-colored ghost
//pack ghosts into a vector
std::vector<ECE_Ghost*> ghosts = { &ghost1,&ghost2,&ghost3,&ghost4 };
extern void ghostAction(ECE_Ghost& ghost, const ECE_Pacman& pac);
extern GameControl controller;
float angle = 0; //the maze's rotation angle
typedef enum { STILL, UP, DOWN, LEFT, RIGHT } moveFlag;
moveFlag oldPacMoveDir = STILL, pacMoveDir; //current moving direction

GLuint maze = glGenLists(1);


//initialiazation, lighting and depth test...
void init(void)
{
    //taking care of lights

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glShadeModel(GL_SMOOTH);
    
    
    GLfloat light_position[] = {3,5,5, 0.0 };
    GLfloat light_white[] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_white);
    glLightfv(GL_LIGHT2, GL_POSITION, light_position);
    glLightfv(GL_LIGHT2, GL_AMBIENT, light_white);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glEnable(GL_LIGHT1);
    //glEnable(GL_LIGHT2);

    //set material shiness
    GLfloat mat_shininess[] = { 50.0 };
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

}

//reshape func
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, (GLfloat)w / (GLfloat)h, 1.0, 40.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
 }

//draw coins
void drawCoins()
{
    //set material
    GLfloat mat_color[] = { 0.75, 0.75, 0.75, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_color);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_color);

    glColor3f(0.75, 0.75, 0.75); //silver solor
    /*bool coins[20][17] = //see if there is a coin to draw
    {
        {T, T, T, T, T, T, T, T, F, T, T, T, T, T, T, T, T},
        {F, F, F, T, F, F, F, T, F, T, F, F, F, T, F, F, F},
        {T, F, F, T, F, F, F, T, F, T, F, F, F, T, F, F, T},
        {T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T},
        {T, F, F, T, F, T, F, F, F, F, F, T, F, T, F, F, T},
        {T, T, T, T, F, T, T, T, F, T, T, T, F, T, T, T, T},
        {F, F, F, T, F, F, F, F, F, F, F, F, F, T, F, F, F},
        {F, F, F, T, F, F, F, F, F, F, F, F, F, T, F, F, F},
        {F, F, F, T, F, F, F, F, F, F, F, F, F, T, F, F, F},
        {F, F, F, T, F, F, F, F, F, F, F, F, F, T, F, F, F},
        {F, F, F, T, F, F, F, F, F, F, F, F, F, T, F, F, F},
        {F, F, F, T, F, F, F, F, F, F, F, F, F, T, F, F, F},
        {F, F, F, T, F, F, F, F, F, F, F, F, F, T, F, F, F},
        {T, T, T, T, T, T, T, T, F, T, T, T, T, T, T, T, T},
        {T, F, F, T, F, F, F, T, F, T, F, F, F, T, F, F, T},
        {F, T, F, T, T, T, T, T, F, T, T, T, T, T, F, T, F},
        {F, T, F, T, F, T, F, F, F, F, F, T, F, T, F, T, F},
        {T, T, T, T, F, T, T, T, F, T, T, T, F, T, T, T, T},
        {T, F, F, F, F, F, F, T, F, T, F, F, F, F, F, F, T},
        {T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T},
    };*/

    glPushMatrix();
    glTranslatef(-8 * len, 0, -9 * len);
    for (int i = 1; i < 21; ++i)
    {
        for (int j = 1; j < 18; ++j)
        {
            if (map[i][j] == COIN)
            {
                glutSolidSphere(0.15*len, 20, 20); //draw a coin where I should draw it
            }
            glTranslatef(1 * len, 0, 0);         //go to next spot
        }
        glTranslatef(-17 * len, 0, 1 * len);  //go to next row
    }
    glPopMatrix();

}

//draw powerups
void drawPowerups()
{
    //set material
    GLfloat mat_color[] = { 0.93f, 0.86f, 0.51f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_color);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_color);

    glColor3f(0.93f, 0.86f, 0.51f); //golden color
    //spots on the map to draw powerups
    int powerups[4][2] =
    {
        {2,1},{2,17},{16,1},{16,17}
    };
    //draw 4 disks as powerups
    GLUquadric* objDisk = gluNewQuadric();
    for (int i = 0; i < 4; ++i)
    {
        int j = powerups[i][0], k = powerups[i][1];
        if (map[j][k] != POWERUP)
            continue;
        float xShift = (float)len * (k - 9);
        float zShift = (float)len * (j - 10);
        glPushMatrix();
        glTranslatef(xShift, 0, zShift);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(objDisk, 0.15, 0.15, 0.15*len,20, 20);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(xShift, 0.15 * len, zShift);
        glRotatef(-90, 1, 0, 0);
        gluDisk(objDisk, 0.00, 0.15, 20, 20);
        glPopMatrix();
    }
}

//draw maze
void drawMaze()
{
    //for still image,create a display list
 glNewList(maze, GL_COMPILE);
     GLfloat mat_color[] = { 0.0f, 0.60f, 0.80f, 1.0f };
     glMaterialfv(GL_FRONT, GL_SPECULAR, mat_color);
     glMaterialfv(GL_FRONT, GL_AMBIENT, mat_color);
     glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_color);
   
     /*draw blue cylinders*/
    glColor3f(0.0f, 0.60f, 0.80f);  
    GLUquadricObj* objCylinder = gluNewQuadric();
    //rectangle at the center
    glPushMatrix();
        glTranslatef(-2*len, 0 , -1*len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5); //down
        glTranslatef(4 * len, 0, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5); //down
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 4 * len, 32, 5); //left
        glTranslatef(2 * len, 0, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 4 * len, 32, 5); //left
        glTranslatef(0, 0, 4*len);
    glPopMatrix();
    
    //draw up-down T shapes (1/2)
    glPushMatrix();
        glTranslatef(-2 * len, 0, 3 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 4 * len, 32, 5);
        glTranslatef(8 * len, 0, 0 );
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 4 * len, 32, 5);
        glTranslatef(-12 * len, 0, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 4 * len, 32, 5);
    glPopMatrix();

    //draw up-down T shapes (2/2)
    glPushMatrix();
        glTranslatef(0, 0, 3 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);
        glTranslatef(0, 0, -8 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);
        glTranslatef(0, 0, 12 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);
    glPopMatrix();

    //draw the up-down direction cylinder 
    glPushMatrix();
        glTranslatef(0, 0, -7 * len);
        glRotatef(180, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 3 * len, 32, 5);
    glPopMatrix();

    /*the rest can be symmetric, draw the right half first*/
    //draw the furthest wall (upper half)
    glPushMatrix();
        glTranslatef(0, 0, -10 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 9 * len, 32, 5); //right
        glTranslatef(0, 0, 9 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 7 * len, 32, 5); //down
        glTranslatef(0, 0, 7 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 3 * len, 32, 5); //left
        glTranslatef(0, 0, 3 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5); //down
        glTranslatef(0, 0, 2 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 3 * len, 32, 5); //right
    glPopMatrix();
    //draw the furthest wall (lower half)
    glPushMatrix();
        glTranslatef(0, 0, 11 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 9 * len, 32, 5); //right
        glTranslatef(0, 0, 9 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 8 * len, 32, 5); //up
        glTranslatef(0, 0, 8 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 3 * len, 32, 5); //left
        glTranslatef(0, 0, 3 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5); //up
        glTranslatef(0, 0, 2 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 3 * len, 32, 5); //right
    glPopMatrix();

    //upper right rectangles
    glPushMatrix();
        glTranslatef(2*len, 0, -8 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //down
        glTranslatef(0, 0, 1 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //right
        glTranslatef(0, 0, 2 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //up
        glTranslatef(0, 0, 1 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //left
    glPopMatrix();
    glPushMatrix();
        glTranslatef(6 * len, 0, -8 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //down
        glTranslatef(0, 0, 1 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //right
        glTranslatef(0, 0, 1 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //up
        glTranslatef(0, 0, 1 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //left
    glPopMatrix();
    
    //T-shape
    glPushMatrix();
        glTranslatef(4 * len, 0, -5 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 4 * len, 32, 5);  //down
        glTranslatef(0, 0, 2 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //left
    glPopMatrix();
    //another T-shape
    glPushMatrix();
        glTranslatef(4 * len, 0, 7 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //down
        glTranslatef(-2*len, 0, 2 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 5 * len, 32, 5);  //right
    glPopMatrix();
    //L shape
    glPushMatrix();
        glTranslatef(6 * len, 0, 5 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //down
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //right
    glPopMatrix();
    //line
    glPushMatrix();
        glTranslatef(2* len, 0, 5 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //right
    glPopMatrix();
    //another line
    glPushMatrix();
        glTranslatef(4 * len, 0, 1 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //down
    glPopMatrix();
    //another line
    glPushMatrix();
        glTranslatef(8 * len, 0, 7 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //right
    glPopMatrix();
    //another line
    glPushMatrix();
        glTranslatef(6 * len, 0, -5 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //right
        glPopMatrix();


        /*the rest is symmetric to the above, draw the left half of the maze*/
        //draw the furthest wall (upper half)
        glPushMatrix();
        glTranslatef(0, 0, -10 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 9 * len, 32, 5); //left
        glTranslatef(0, 0, 9 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 7 * len, 32, 5); //down
        glTranslatef(0, 0, 7 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 3 * len, 32, 5); //right
        glTranslatef(0, 0, 3 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5); //down
        glTranslatef(0, 0, 2 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 3 * len, 32, 5); //left
        glPopMatrix();
        //draw the furthest wall (lower half)
        glPushMatrix();
        glTranslatef(0, 0, 11 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 9 * len, 32, 5); //left
        glTranslatef(0, 0, 9 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 8 * len, 32, 5); //up
        glTranslatef(0, 0, 8 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 3 * len, 32, 5); //right
        glTranslatef(0, 0, 3 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5); //up
        glTranslatef(0, 0, 2 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 3 * len, 32, 5); //left
        glPopMatrix();
       
        //upper right rectangles
        glPushMatrix();
        glTranslatef(-2 * len, 0, -8 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //down
        glTranslatef(0, 0, 1 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //left
        glTranslatef(0, 0, 2 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //up
        glTranslatef(0, 0, 1 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //right
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-6 * len, 0, -8 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //down
        glTranslatef(0, 0, 1 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //left
        glTranslatef(0, 0, 1 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //up
        glTranslatef(0, 0, 1 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //right
        glPopMatrix();

        //T-shape
        glPushMatrix();
        glTranslatef(-4 * len, 0, -5 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 4 * len, 32, 5);  //down
        glTranslatef(0, 0, 2 * len);
        glRotatef(90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //right
        glPopMatrix();
        //another T-shape
        glPushMatrix();
        glTranslatef(-4 * len, 0, 7 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //down
        glTranslatef(2 * len, 0, 2 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 5 * len, 32, 5);  //left
        glPopMatrix();

        //L shape
        glPushMatrix();
        glTranslatef(-6 * len, 0, 5 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //down
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //left
        glPopMatrix();
        //line
        glPushMatrix();
        glTranslatef(-2 * len, 0, 5 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //left
        glPopMatrix();
        //another line
        glPushMatrix();
        glTranslatef(-4 * len, 0, 1 * len);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 2 * len, 32, 5);  //down
        glPopMatrix();
        //another line
        glPushMatrix();
        glTranslatef(-8 * len, 0, 7 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //left
        glPopMatrix();
        //another line
        glPushMatrix();
        glTranslatef(-6 * len, 0, -5 * len);
        glRotatef(-90, 0, 1, 0);
        gluCylinder(objCylinder, 0.15*len, 0.15*len, 1 * len, 32, 5);  //left
        glPopMatrix();

        //add spheres at the corners, line by line
        glPushMatrix();
            glTranslatef(-9 * len, 0, -10 * len); //line 1
            glutSolidSphere(0.15 * len, 20, 20);
            glTranslatef(18 * len, 0, 0);
            glutSolidSphere(0.15 * len, 20, 20);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-7 * len, 0, -8 * len); //line 3
            glutSolidSphere(0.15 * len, 20, 20);
            glTranslatef(1 * len, 0, 0);
            glutSolidSphere(0.15 * len, 20, 20);
            glTranslatef(2 * len, 0, 0);
            glutSolidSphere(0.15 * len, 20, 20);
            glTranslatef(2 * len, 0, 0);
            glutSolidSphere(0.15 * len, 20, 20);
            glTranslatef(4 * len, 0, 0);
            glutSolidSphere(0.15 * len, 20, 20);
            glTranslatef(2 * len, 0, 0);
            glutSolidSphere(0.15 * len, 20, 20);
            glTranslatef(2 * len, 0, 0);
            glutSolidSphere(0.15 * len, 20, 20);
            glTranslatef(1 * len, 0, 0);
            glutSolidSphere(0.15 * len, 20, 20);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-7 * len, 0, -7 * len); //line 4
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(1 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(4 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(1 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glPopMatrix();
        //add spheres at the corners, line by line
        glPushMatrix();
        glTranslatef(-7 * len, 0, -5 * len); //line 6
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(1 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(4 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(1 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glPopMatrix();
        //add spheres at the corners, line by line
        glPushMatrix();
        glTranslatef(-9 * len, 0, -3 * len); //line 8
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(3 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(4 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(4 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(3 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glPopMatrix();
        //add spheres at the corners, line by line
        glPushMatrix();
        glTranslatef(-9 * len, 0, -1 * len); //line 10
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(3 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(4 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(3 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glPopMatrix();
        //add spheres at the corners, line by line
        glPushMatrix();
        glTranslatef(-9 * len, 0, 1 * len); //line 12
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(3 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(4 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(3 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glPopMatrix();
        //add spheres at the corners, line by line
        glPushMatrix();
        glTranslatef(-9 * len, 0, 3 * len); //line 14
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(3 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(4 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(3 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glPopMatrix();
        //add spheres at the corners, line by line
        glPushMatrix();
        glTranslatef(-7 * len, 0, 5 * len); //line 16
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(1 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(1 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glPopMatrix();
        //add spheres at the corners, line by line
        glPushMatrix();
        glTranslatef(-8 * len, 0, 7 * len); //line 18
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(4 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glPopMatrix();
        //add spheres at the corners, line by line
        glPushMatrix();
        glTranslatef(-7 * len, 0, 9 * len); //line 20
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(5 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(2 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(5 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glPopMatrix();
        //add spheres at the corners, line by line
        glPushMatrix();
        glTranslatef(-9 * len, 0, 11 * len); //line 22 (last)
        glutSolidSphere(0.15 * len, 20, 20);
        glTranslatef(18 * len, 0, 0);
        glutSolidSphere(0.15 * len, 20, 20);
        glPopMatrix();
  glEndList();
}

//draw string
void drawString(int x, int y, int z, const char* string)
{
    GLfloat mat_color[] = { 0.0f, 0.60f, 0.80f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_color);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_color);

    glColor3f(1.0f, 1.0f, 1.0f);
    //set the position of the text in the window using the x and y coordinates
    glRasterPos3f(x, y, z);
    //get the length of the string to display
    int length = (int)strlen(string);

    //loop to display character by character
    for (int i = 0; i < length; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
    }
};

//display func
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1.0); //background color is black
    glLoadIdentity();
    // Set the camera with the y-axis pointing up
    gluLookAt(15, 15, 15, 0, 0, 0, 0, 1, 0);
    glRotatef(-angle, 0, 1, 0); //press R to rotate
    //draw maze
    //glCallList(maze);
    drawMaze();
    drawCoins();
    drawPowerups();
    //draw pacman,cosidering motion
    switch (pacMoveDir)
    {
    case STILL:
        myPacman.drawPacman();
        break;
    case UP:
        myPacman.moveUp();
        myPacman.drawPacman();
        break;
    case DOWN:
        myPacman.moveDown();
        myPacman.drawPacman();
        break;
    case LEFT:
        myPacman.moveLeft();
        myPacman.drawPacman();
        break;
    case RIGHT:
        myPacman.moveRight();
        myPacman.drawPacman();
        break;
    }
    //draw gohsts,cosidering motion
        ghost1.drawGhost();
        ghost2.drawGhost();
        ghost3.drawGhost();
        ghost4.drawGhost();
    glutSwapBuffers();
    //allow keyboard control of direction
    myPacman.enableKey = true;
}

//press R to rotate
void keyboard(unsigned char key, int x, int y)
{
    //for game control
    switch (key)
    {
    case 'R': //rotate
    case 'r':
        angle += 5.0;
        glutPostRedisplay(); // redisplay everything
        break;
        //if not meant to pause, any key pressed means game started
    default:
        if(controller.gameStatus == PAUSED || controller.gameStatus == END)
            controller.gameStatus = STARTED;
        break;
    }
}

//special key func (arrows)
void pressSpecialKeys(int key, int x, int y)
{
    //while keyboard is disabled, do nothing
    if (!myPacman.enableKey)
    {
        return;
    }
    //while keyboard is enabled, press any key to start the game
    else if (controller.gameStatus == PAUSED)
    {
        controller.gameStatus = STARTED;
    }
    //control pac direction
    switch (key)
    {
    case GLUT_KEY_UP:
        pacMoveDir = UP;
        break;
    case GLUT_KEY_DOWN:
        pacMoveDir = DOWN;
        break;
    case GLUT_KEY_LEFT:
        pacMoveDir = LEFT;
        break;
    case GLUT_KEY_RIGHT:
        pacMoveDir = RIGHT;
        break;
    default: 
        break;
    }
    
}

//idle func
void idle()
{
    //when pacman dies/wins,stop the animation for a while
    while (controller.gameStatus == PACDIE || controller.gameStatus == END)
    {
        //disable control of direction
        myPacman.enableKey = false;
        //resume pacman's moveflag
        pacMoveDir = STILL;
        //stop redisplaying until everything is resumed
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    /*
    //when pacman wins,stop the animation for a while
    while (myPacman.countEats == myPacman.totalEatsToWin)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    */
}

//redisplay timer
void myTimer(int value)
{
    switch (value)
    {
    case 1:
        glutPostRedisplay(); // redisplay everything (put display list!!!!!!!
        glutTimerFunc(250, myTimer, 1); //loop timer for redisplaying
        break;
    case 2:
        break;
    default: break;
    }
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("PacmanGame");

    init(); //some preparations
    glutReshapeFunc(reshape);
    glutDisplayFunc(display); // (re)display callback
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(pressSpecialKeys);
    glutIdleFunc(idle);
    glutTimerFunc(200, myTimer, 1); //timer for pacman move 100ms/step
    glutTimerFunc(100, myTimer, 2); //timer2
    //distribute 4 threads for ghost activities and 1 for game control
    controller.controlThread = std::thread(gameControl, std::ref(controller));
    for (auto pGhost : ghosts)
    {
        pGhost->ghostThread = std::thread(ghostAction, std::ref(*pGhost), std::ref(myPacman));
    }
    // enter GLUT event processing cycle
    glutMainLoop();
}