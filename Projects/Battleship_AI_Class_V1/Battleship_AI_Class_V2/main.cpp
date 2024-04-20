
/* 
 * File:   main.cpp
 * Author: Donald Burrell // Ryan Kingwill
 *
 * Created on April 19, 2024, 7:01 PM
 */

#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

//--BATTLESHIP AI--//
//This version same as previous, but add vertical ship generation to further test AI
//Tried to prevent generation off the edge, but not the best
//Nothing yet to prevent ships being generated adjacent to one another (happens more often now that I changed how they generate by borders)
//Still not dynamic--will probably wait for the rest of the project before that

class AI
{
private:
    //Member variables
    bool hit;            //Bool for if last shot was a hit
    bool shot[10][10];   //Test board for if spaces have been shot at

public:
    //Class Functions//

    //Constructors / Destructors
    AI()
    {
        //Initialize defaults
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                shot[i][j] = false; //Not shot at
            }
        }
        hit = false;
    }

    //Accessors
    bool getHit() const
    {
        return hit;
    }

    bool getShot(int r, int c) const
    {
        return shot[r][c];
    }

    //Mutators
    void setHit(bool hit)
    {
        //Set equal to argument value (T/F)
        this->hit = hit;
    }

    void setShot(int r, int c, bool shot)
    {
        //Sets a certain cell's shot value to this argument "shot"
        this->shot[r][c] = shot;
    }

    //-----------------------------------------------//

    //AI functions//
    //Attack the board once
    void attack(bool[][10]);
              //board

    //Check availability to attack adjacent spots
    void chkDirs(bool[], int, int);
               //dirAvl   row  col

    //Check if shot is a hit & fire
    bool htOrMss(bool[][10], int, int);
               //board       row   col  

};

//NOT AI FUNCTIONS
//Test functions
void setShip(bool [][10]); //Set the board


int main(int argc, char** argv) {
    //Driver
    srand(time(0));
    bool brd[10][10]; //Test board

    AI ai;

    //I didn't bother object-orienting the extraneous stuff (ie. ship/board)

    //Initialize board
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            brd[i][j] = false; //Not occupied
        }
    }

    //Assign Ships
    setShip(brd);

    //Test the attack "AI" a number of times
    for (int i = 0; i < 20; i++)
    {
        cout << "-----Shot " << i + 1 << "-----" << endl;
        //Call static AI function
        ai.attack(brd);
    }

    //Display which spots were shot at
    cout << "Shot: ";
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (ai.getShot(i, j))
            {
                cout << "[" << i << "][" << j << "], ";
            }
        }
    }
    cout << endl;

    //Display which spots were hit
    cout << "Hit: ";
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (ai.getShot(i, j) && brd[i][j])
            {
                cout << "[" << i << "][" << j << "], ";
            }
        }
    }

    return 0;
}


void AI::attack(bool brd[][10])
{
    static int row = 0;     //Target Row value 0-9
    static int col = 0;     //Target Row value 0-9
    static bool shoot = 0;  //Bool for whether to fire or not
                             //if false, tries a different spot/direction
    static int hitDir = -1; //Direction to be continued in if 2 consecutive hits
    static bool swpd = 0;   //True if has already switched to opposite direction before

    //Loop until the generated position/adjacent position can be fired at
    do
    {
        cout << " Check pre-rand_pos" << endl;
        //Generate new random space if previous shot was a miss
        if (!this->hit)
        {
            cout << "  New Random Position: " << endl;
            row = rand() % 10;
            col = rand() % 10;
        }
        cout << "  [" << row << "][" << col << "]" << endl;

        if (this->hit)
        {
            //--HEURISTIC--//
            //Given that it hit last turn, the AI will choose randomly of the adjacent cells
            int randDec = rand() % 4; //Random for up down, left right of hit. 

            //Keep track of which adjacent spaces can be fired at
            bool dirAvl[4] = {1, 1, 1, 1};  //(right, left, down, up)

            //Check availability to attack adjacent spots
            //Checks if on edge, or if adjacent spot has been fired at before
            chkDirs(dirAvl, row, col);

            //If previous shot was at least the second hit of the ship, change randDec
            if (hitDir > -1)
            {
                //If direction is available, continue in same direction as before
                if (dirAvl[hitDir])
                {
                    cout << "    Continue same direction" << endl;
                    randDec = hitDir;
                }
                //Otherwise, go in the opposite direction from other side of the ship
                //as long as it has not already done so before (if swpd == false)
                else if ((!this->getHit() || !shoot || !dirAvl[hitDir]) && !swpd)
                {
                    cout << "    Go opposite direction" << endl;
                    swpd = true;
                    shoot = true;

                    //Check previous direction
                    switch (hitDir)
                    {
                    case 0: //If original direction was right, switch to left
                        //Moves "crosshair aim" to left until left shot is available
                        while (!dirAvl[1] && col != 0)
                        {
                            col--;
                            cout << "  at [" << row << "][" << col << "]: " << endl;
                            chkDirs(dirAvl, row, col);
                        }
                        randDec = 1; //Swap direction to left
                        break;
                    case 1: //If original direction was left, switch to right
                        //Moves "crosshair aim" to right until right shot is available
                        while (!dirAvl[0] && col != 9)
                        {
                            col++;
                            cout << "  at [" << row << "][" << col << "]: " << endl;
                            chkDirs(dirAvl, row, col);
                        }
                        randDec = 0; //Swap direction to right
                        break;
                    case 2: //If original direction was down, switch to up
                        //Moves "crosshair aim" up until upward shot is available
                        while (!dirAvl[3] && row != 0)
                        {
                            row--;
                            cout << "  at [" << row << "][" << col << "]: " << endl;
                            chkDirs(dirAvl, row, col);
                        }
                        randDec = 3; //Swap direction to up
                        break;
                    case 3: //If original direction was up, switch to down
                        //Moves "crosshair aim" down until downward shot is available
                        while (!dirAvl[2] && row != 9)
                        {
                            row++;
                            cout << "  at [" << row << "][" << col << "]: " << endl;
                            chkDirs(dirAvl, row, col);
                        }
                        randDec = 2; //Swap direction to down
                        break;
                    }
                }
                //If can no longer continue in same or opposite direction,
                //then set it up so that a new location is generated instead
                else
                {
                    swpd = 0;
                    randDec = -1;
                    cout << "    Continuing same/opposite direction is unavailable";
                    cout << " from: [" << row << "][" << col << "] " << endl;
                }
            }

            //Change position to be fired at based on direction and availability
            if (randDec == 0 && dirAvl[0])
            {
                //Move position right one
                col++;
                shoot = true;
                cout << "    Shoot right" << endl;
                if (!htOrMss(brd, row, col))
                {
                    //If miss, re-target original spot
                    col--;
                }
                else
                {
                    hitDir = 0;
                }
            }
            else if (randDec == 1 && dirAvl[1])
            {
                //Move position left one
                col--;
                shoot = true;
                cout << "    Shoot left" << endl;
                if (!htOrMss(brd, row, col))
                {
                    //If miss, re-target original spot
                    col++;
                }
                else
                {
                    hitDir = 1;
                }
            }
            else if (randDec == 2 && dirAvl[2])
            {
                //Move position down one
                row++;
                shoot = true;
                cout << "    Shoot down" << endl;
                if (!htOrMss(brd, row, col))
                {
                    //If miss, re-target original spot
                    row--;
                }
                else
                {
                    hitDir = 2;
                }
            }
            else if (randDec == 3 && dirAvl[3])
            {
                //Move position up one
                row--;
                shoot = true;
                cout << "    Shoot up" << endl;
                if (!htOrMss(brd, row, col))
                {
                    //If miss, re-target original spot
                    row++;
                }
                else
                {
                    hitDir = 3;
                }
            }
            else if (randDec == -1)
            { //Cannot go in same or opposite direction
                shoot = false;
                this->hit = false;
                hitDir = -1;
                cout << "Generate new random direction" << endl;
            }
            //Either random direction is unavailable, or there are not available adjacent spots
            else
            {
                //Set hit to false to generate new random position try and hit
                shoot = false;
                if (!dirAvl[0] && !dirAvl[1] && !dirAvl[2] && !dirAvl[3])
                {
                    this->hit = false;
                    cout << "    No adjacent spot available" << endl;
                }
                else
                {
                    cout << "    This adjacent spot is unavailable" << endl;
                }
            }
        }
        else
        { //If previous shot was a miss, just shoot the random spot
            shoot = true;
            htOrMss(brd, row, col);
        }
    } while (!shoot);
}

void AI::chkDirs(bool dirAvl[], int row, int col)
{
    //Check availability to attack adjacent spots
    //Checks if on edge, or if adjacent spot has been fired at before
    if (col == 9 || this->shot[row][col + 1])
    { //Right edge/spot
        dirAvl[0] = false;
        cout << "   Unavailable right" << endl;
    }
    else
    {
        dirAvl[0] = true;
    }
    if (col == 0 || this->shot[row][col - 1])
    { //Left edge/spot
        dirAvl[1] = false;
        cout << "   Unavailable left" << endl;
    }
    else
    {
        dirAvl[1] = true;
    }
    if (row == 9 || this->shot[row + 1][col])
    { //Lower edge/spot
        dirAvl[2] = false;
        cout << "   Unavailable down" << endl;
    }
    else
    {
        dirAvl[2] = true;
    }
    if (row == 0 || this->shot[row - 1][col])
    { //Upper edge/spot
        dirAvl[3] = false;
        cout << "   Unavailable up" << endl;
    }
    else
    {
        dirAvl[3] = true;
    }
}

bool AI::htOrMss(bool brd[][10], int r, int c)
{
    this->shot[r][c] = true; //Record that spot has been fired upon

    if (brd[r][c])
    { //HIT
        this->hit = true;
        cout << "      Hit spot: " << "[" << r << "][" << c << "]" << endl;
        return true;
    }
    else
    { //MISS
        cout << "      Miss spot: " << "[" << r << "][" << c << "]" << endl;
        return false;
    }
}


void setShip(bool brd[][10])
{
    //Assign ship locations -- MUCH BETTER BUT NOT DYNAMIC!
    int shpSzs[] = {2, 3, 3, 4, 5};
    for (int i = 0; i < 5; i++)
    {
        //Find random location to place each ship [i]
        int strtRow = rand() % 10;
        int strtCol = rand() % (10 - shpSzs[i] + 1);
        bool valPos = true;
        //Check if position is already assigned to another ship
        for (int j = 0; j < shpSzs[i]; j++)
        {
            if (brd[strtRow][strtCol + j])
            {
                valPos = false;
                break;
            }
        }
        //Also add vertical ships
        //If it is Valid position, mark as now occupied and set board space to TRUE
        if (valPos)
        {
            int rndVert = rand() % 2 + 1;    //Random number 1 or 2
            //Let 1 be horizontal and 2 be vertical ship generation
            
            //Generate horizontally
            if(rndVert == 1)
            {
                for (int k = 0; k < shpSzs[i]; k++)
                {
                    //My attempt at managing ship being generated off the edge
                    brd[strtRow][strtCol + k] = true;
                    if(strtCol >= 5)
                    {
                        brd[strtRow][strtCol - k] = true;
                    }
                    else
                    {
                        brd[strtRow][strtCol + k] = true;
                    }
                }
            }
            else if(rndVert == 2)
            {
                //Generate vertically
                for (int k = 0; k < shpSzs[i]; k++)
                {
                    //My attempt at managing ship being generated off the edge
                    if(strtRow >= 5)
                    {
                        brd[strtRow - k][strtCol] = true;
                    }
                    else
                    {
                        brd[strtRow + k][strtCol] = true;
                    }
                }
            }
            else
            {
                cout << "\n!!SHIP GENERATION ERROR!!\n";
            }
            
        }
        else
        {
            //If not valid, extend loop to get new rand() number
            i--;
        }
    }

    for (int j = 0; j < 10; j++)
    {
        for (int k = 0; k < 10; k++)
        {
            if (brd[j][k])
            {
                cout << "X ";
            }
            else
            {
                cout << "O ";
            }
        }
        cout << endl;
    }
}
