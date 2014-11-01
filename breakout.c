//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// velocity speed
#define SPEED 4

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);
void endGameMessage (GWindow window, char* message);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel scoreboard = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // velocity of the ball translating the speed of the bal
    double velocity_x = -2; //*(drand48() * SPEED) - SPEED / 2 - SPEED*/
    double velocity_y =  2;
    
    /* Bonus feature how many shot you ended the game
    int it = 0; */
    
    // keep playing until game over until no lives or bricks left
    while (lives > 0 && bricks > 0)
    {
        // check for mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);
        
        // if we heard one
        if(event != NULL)
        {
            // if the event is mouvement
            if(getEventType(event) == MOUSE_MOVED)
            {
                // position the paddle x and y coordinates
                double x = getX(event) - getWidth(paddle) / 2;
                double y = getHeight(window) - getHeight(paddle) - 40;
                
                // keep the paddle from leaving the frame of the window
                if (x + getWidth(paddle) >= getWidth(window) )
                {
                    x = getWidth(window) - getWidth(paddle);
                }else if (x < 0)
                {
                    x = 0;
                }
            
                setLocation(paddle, x, y);
            }
        }
        
        // make the ball move
        move(ball, velocity_x, velocity_y);
        
        // bounce  from top of the frame
        if (getY(ball) <= 0)
        {
             velocity_y = -velocity_y;
        }
        
        // make the ball bounce from the right side
        else if (getX(ball) + getWidth(ball) >= getWidth(window))
        {
            velocity_x = -velocity_x;
        }
        
        // if the ball bounce from the left side
        else if (getX(ball) <= 0)
        {
            velocity_x = -velocity_x;
        }
        
        // if it touches the bottom 
        else if (getY(ball) + getHeight(ball) >= getHeight(window))
        {
            double x = getWidth(window) / 2 - RADIUS;
            double y = getHeight(window) / 2 - RADIUS;
            lives--;
            setLocation(ball, getWidth(window) + RADIUS, getHeight(window) + RADIUS);
            waitForClick();
            setLocation(ball, x, y);
        }
        
        // detect collision of the ball
        GObject object = detectCollision(window, ball);
        
        if (object !=NULL)
        {
            // if the ball colides with the paddle or top of rect
            if (object == paddle)
            {
                //printf("it touches it %i times\n", it);
                if (getY(ball) + getHeight(ball) >= getY(paddle)) 
                {
                    velocity_y = -velocity_y;
                    //it++;
                    //object = detectCollision(window, ball);
                }
            }
        
            // remove the brick
            if (strcmp(getType(object), "GRect") == 0 && object != paddle)
            {
                removeGWindow(window, object);
                bricks--;
                points++;
                if (getY(ball) + getHeight(ball) >= getY(object)) 
                {
                    velocity_y = -velocity_y;
                }
                else if (getY(ball) >= getY(object) + getHeight(object)) 
                {
                    velocity_y = -velocity_y;
                }
            }
        }
        
        pause(10);
        
        updateScoreboard(window, scoreboard, points);
    }
    char* s;
    if (lives > 0) 
    {
        s = "Bravo You Won!";
        if (s != NULL)
        {
            endGameMessage(window, s);
        } else 
        {
            printf("NULL char");
        }
    } else 
    {
        s = "Game Over!";
        if (s != NULL)
        {
            endGameMessage(window, s);
        } else 
        {
            printf("NULL char");
        }
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // initialize the gap between bricks in pixel
    double gap = 4;
    
    
    // initialize x and y
    double x = gap / 2;
    double y = 50;
    double brickWidth = (getWidth(window) / COLS)- gap;
    
    // draw bricks in window
    for(int i = 0; i < ROWS; i++)
    {
        for(int j = 0; j < COLS; j++)
        {
            //instantiate brick
            GRect brick = newGRect(x, y, brickWidth, 10);
            
            setFilled(brick, true);
      
            setColor(brick, "GRAY");
            
            // change color
            if (i == 1)
            {
                setColor(brick, "BLUE");
            }
            else if (i == 2)
            {
                setColor(brick, "GREEN");
            }
            else if (i == 3)
            {
                setColor(brick, "MAGENTA");
            }
            else if (i == 4)
            {
                setColor(brick, "ORANGE");
            }
            add(window, brick);
            setLocation(brick, x, y);
            
            x += getWidth(brick) + 4;
            
        }
           // return brick to the left of the screen 
           x = gap / 2; 
           
           // bottom gap   
           y += 10 + 5;
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // instaltiate a ball
    GOval ball = newGOval(0, 0, 2 * RADIUS, 2 * RADIUS);
    setFilled(ball, true);
    setColor(ball, "BLACK");
    
    // place in center of window
    double x = getWidth(window) / 2 - RADIUS;
    double y = getHeight(window) / 2 - RADIUS;
    setLocation(ball, x, y);
    add(window, ball);
    
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // instantiate a rectangle
    GRect paddle = newGRect(0, 0, 70, 10); //getWidth(window)
    
    // give him swagg
    setColor(paddle, "RED");
    setFilled(paddle, true);
    
    //center paddle in bottom-middle
    double x = (getWidth(window) - getWidth(paddle)) / 2;
    double y = getHeight(window) - getHeight(paddle) - 30;
    setLocation(paddle, x, y);
    
    // add it to the window
    add(window, paddle);
    
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel scoreboard = newGLabel("");
    
    double x = getWidth(window) - getWidth(scoreboard) / 2;
    double y = getHeight(window) - getHeight(scoreboard) / 2;
    
    setFont(scoreboard, "SansSerif-36");
    add(window, scoreboard);
    setLocation(scoreboard, x, y);
    
    return scoreboard;
}

/**
* Show a message at the end of the game
*/
void endGameMessage(GWindow window, char* message) 
{
    GLabel messageboard = newGLabel(message);
    
    double x = getWidth(window) - getWidth(messageboard) / 2;
    double y = getHeight(window) - getHeight(messageboard) / 2;
    
    setColor(messageboard, "RED");
    setFont(messageboard, "SansSerif-36");
    add(window, messageboard);
    setLocation(messageboard, x, y);
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
