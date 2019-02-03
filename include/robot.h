/* 
 * author: Joel Klimont
 * filename: robot.h
 * date: 28/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#ifndef ROBOCODE_ROBOT_H
#define ROBOCODE_ROBOT_H

#include <ncurses.h>
#include "curses_drawable_objects.h"

struct Robot {
public:
    float pos_x;
    float pos_y;
    float speed_x;
    float speed_y;
    int gun_degree;
    float energy;
    drawable::Robot drawable_robot;
    void draw(WINDOW* window);
    bool check_collision(Robot robot); // check if this robot has collided with the one provided
};

#endif //ROBOCODE_ROBOT_H
