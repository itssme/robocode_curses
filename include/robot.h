/* 
 * author: Joel Klimont
 * filename: robot.h
 * date: 28/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/
#ifndef ROBOCODE_ROBOT_H
#define ROBOCODE_ROBOT_H

struct Robot {
public:
    int pos_x;
    int pos_y;
    int speed_x;
    int speed_y;
    int gun_degree;
    int energy;
};

#endif //ROBOCODE_ROBOT_H
