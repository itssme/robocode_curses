/* 
 * author: Joel Klimont
 * filename: robot.cpp
 * date: 31/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include "robot.h"

Robot::Robot(WINDOW* parent_window, drawable::Robot drawable_robot) : drawable_robot(drawable_robot) {

}

void Robot::draw() {

}

bool Robot::check_collision(Robot robot) {
    return false;
}

bool Robot::check_collision(Bullet bullet) {
    return false;
}

Bullet Robot::shoot() {

}
