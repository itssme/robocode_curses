/* 
 * author: Joel Klimont
 * filename: robot.h
 * date: 28/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#ifndef ROBOCODE_ROBOT_H
#define ROBOCODE_ROBOT_H

#include "curses_drawable_objects.h"
#include "bullet.h"

class Robot {
public:
    double pos_x;
    double pos_y;
    double speed_x;
    double speed_y;
    int gun_degree;
    double energy;
    drawable::Robot drawable_robot;
    void draw();
    Bullet shoot();
    void tick();
    bool check_collision(Robot robot); // check if this robot has collided with the one provided
    bool check_collision(Bullet bullet);
    Robot(WINDOW* parent_window, drawable::Robot drawable_robot);
};

#endif //ROBOCODE_ROBOT_H
