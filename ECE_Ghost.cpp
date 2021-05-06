/*
Author: <Wenlong Wang>
Class : ECE6122
Last Date Modified : <2020 / 11 / 26>
Description :
"implementation of class ECE_Ghost"
*/
#include "ECE_Ghost.h"
#include <GL/glut.h>
extern GameControl controller;

//ghost's thread function
void ghostAction(ECE_Ghost &ghost, const ECE_Pacman &pac)
{
    ghost.waitForRelease();
    std::vector<int> shortestPath;
    while (controller.gameStatus == STARTED)
    {
        //starting position and goal position
        int zStart = round(ghost.z / len) + 10;
        int xStart = round(ghost.x / len) + 9;
        int zEnd = round(pac.z / len) + 10;
        int xEnd = round(pac.x / len) + 9;
        //alive, do as following
        if (!ghost.killed )
        {
            //pursue pacman
            if (!controller.powerModeStart)
            {
                //find the shortest path
                shortestPath = findPathToPac(xStart, zStart, xEnd, zEnd);
                //go along that path to catch pacman
                for (auto direction : shortestPath)
                {
                    //incase pacman is suddenly dead, don't waste time here,finish this thread
                    if (pac.killed || controller.powerModeStart || controller.gameStatus == END)
                        break;
                    ghost.move(direction);
                    Sleep(1000 / ghost.ghostSpeed);
                }
            }
            //run away from pacman
            else if (controller.powerModeStart)
            {
                //int des[2];
                zEnd = pac.z > ghost.z ? 1 : 20;
                xEnd = pac.x > ghost.x ? 1 : 17;
                //int randDir = rand() % 4 + 1; //rand number in range 1~4
                //find the shortest path
                shortestPath = findPathToPac(xStart, zStart, xEnd, zEnd);
                //go along that path to avoid pacman
                for (auto direction : shortestPath)
                {
                    //incase pacman is suddenly dead, don't waste time here,finish this thread
                    if (pac.killed || !controller.powerModeStart || controller.gameStatus == END)
                        break;
                    ghost.move(direction);
                    Sleep(1000 / ghost.ghostSpeed);
                }
            }
            
        }
        //killed, go back to the maze center
        else if (ghost.killed)
        {
            //ghost.setColor(0, 0, 0); //dead, body is invisible
            //wait for 5s to respawn:
            time_point start = std::chrono::steady_clock::now();
            time_point now = std::chrono::steady_clock::now();
            while (now - start < std::chrono::seconds(5))
            {
                if (controller.gameStatus == PACDIE || controller.gameStatus == END)
                {
                    ghost.ready4AnotherRound = true;
                    return;
                }
                now = std::chrono::steady_clock::now();
            }
            ghost.resume();
            ghost.waitForRelease();
        }
    }
    //when the code goes here,the pacman should be dead, 
    //the ghost ready to play another round, inform the control thread
    ghost.ready4AnotherRound = true;
}

//default constructor
ECE_Ghost::ECE_Ghost(): colorR(0.5), colorG(0.5), colorB(0.5)
{
}

//constructor with arguments
ECE_Ghost::ECE_Ghost(float R, float G, float B)
    : colorR(R), colorG(G), colorB(B), 
      originalR(R), originalG(G), originalB(B)
{
    this->threadID = this->ghostThread.get_id();
    ghostSpeed = HIGH;
    this->spawn();
    //spotNum = 5;
}

//draw a ghost on current location (this->x, this->z)
void ECE_Ghost::drawGhost()
{
    //material color
    GLfloat mat_color[] = { this->colorR, this->colorG, this->colorB, 1.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_color);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_color);
    glEnable(GL_COLOR_MATERIAL);

    glColor3f(this->colorR, this->colorG, this->colorB);
    //glLoadIdentity();
    GLUquadric* obj = gluNewQuadric();
    glPushMatrix();
        glTranslatef(this->x, 0, this->z);
        glRotatef(-90, 1, 0, 0);
        gluCylinder(obj, 0.4 * len, 0.4 * len, 0.3 * len, 32, 5);
        glTranslatef(0, 0, 0.3*len);
        glutSolidSphere(0.4 * len, 20, 20);
    glPopMatrix(); 
}

//set the Ghost's color
void ECE_Ghost::setColor(float R, float G, float B)
{
    colorR = R;
    colorG = G;
    colorB = B;
}

//move toward given direction
void ECE_Ghost::move(int dir)
{
    int xTemp, zTemp;
    //update coordinates 0-still, 1-up, 2-down, 3-left, 4-right
    switch (dir)
    {
    case 0:
        break;
    case 1:
        //check walls. see if moving one step would crash onto a wall
         zTemp = round((z - 1 * len) / len) + 10;
         xTemp = round(x / len) + 9;
        //if the move is illegal, don't move
        if (map[zTemp][xTemp] == WALL)
        {
            return;
        }
        else
            z -= 1 * len;
        break;
    case 2:
        //check walls. see if moving one step would crash onto a wall
         zTemp = round((z + 1 * len) / len) + 10;
         xTemp = round(x / len) + 9;
        //if the move is illegal, don't move
        if (map[zTemp][xTemp] == WALL)
        {
            return;
        }
        z += 1 * len;
        break;
    case 3:
        //check walls. see if moving one step would crash onto a wall
         zTemp = round(z / len) + 10;
         xTemp = round((x - 1 * len )/ len) + 9;
         //it's ok to pass special spots
         if (zTemp == 10 && xTemp == -1)
         {
             x = 9 * len;
             return;
         }
        //if the move is illegal, don't move
        if (map[zTemp][xTemp] == WALL)
        {
            return;
        }
        x -= 1 * len;
        break;
    case 4:
        //check walls. see if moving one step would crash onto a wall
         zTemp = round(z / len) + 10;
         xTemp = round((x + 1 * len) / len) + 9;
         //it's ok to pass special spots
         if (zTemp == 10 && xTemp == 19)
         {
             x = -9 * len;
             return;
         }
        //if the move is illegal, don't move
        if (map[zTemp][xTemp] == WALL)
        {
            return;
        }
        x += 1 * len;
        break;
    }
    //in the displayfunc(),call drawPacman() to move the pacman image up
}

//when the game start, init ghost, name the pacman to chase
void ECE_Ghost::waitForRelease()
{
    while (controller.gameStatus != STARTED)
    {
    }
    //if the last ghost is released >2s ago, allow releasing this ghost
    time_point now = std::chrono::steady_clock::now();
    mtx.lock();
    while (now - lastRelease < releaseInterval)
    {
        //not allowed out ,wait for 0.5s and check again
        Sleep(500);
        now = std::chrono::steady_clock::now();
    }
    //if the code goes down here, this ghost is allowed out, reset lastRelease
    lastRelease = std::chrono::steady_clock::now();
    //the respawn seat is open now
    spotOccupied[this->spotNum] = false;
    mtx.unlock();
    this->z = -2 * len;
    this->x = 0;
    //now! go for pacman!!!
}

//spawn
void ECE_Ghost::spawn()
{
    //find a spot to put a newly-spawn ghost, up to 4 spots
    mtx.lock();
    for (int i = 0; i < 4; ++i)
    {
        if (spotOccupied[i])
            continue;
        else
        {
            this->x = spawnSpot[i][0];
            this->z = spawnSpot[i][1];
            spotOccupied[i] = true;  
            this->spotNum = i;
            break;
        }
    }
    mtx.unlock();
}

//resume after pacman dies or game ends
void ECE_Ghost::resume()
{
    if (controller.powerModeStart)
    {
        this->setColor(1.0f, 1.0f, 1.0f);
        ghostSpeed = LOW;
    }
    else
    {
        ghostSpeed = HIGH;
        this->setColor(originalR, originalG, originalB);
    }
    this->spawn();
    this->killed = false;
}


/*static members*/

//four respawn spots
float ECE_Ghost::spawnSpot[4][2] = { {0, -0.5 * len}, {0, 0.5 * len},{-1 * len, 0.5 * len},{1 * len, 0.5 * len} };
//flag indicating ghost spots' status
bool ECE_Ghost::spotOccupied[4] = { false, false,false,false };
//time at last release
time_point ECE_Ghost::lastRelease = std::chrono::steady_clock::now();
//release interval = 2s
std::chrono::seconds ECE_Ghost::releaseInterval(2);
//init the mutex object
std::mutex ECE_Ghost::mtx;