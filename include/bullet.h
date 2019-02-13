/* 
 * author: Joel Klimont
 * filename: bullet.h
 * date: 11/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/
#ifndef ROBOCODE_BULLET_H
#define ROBOCODE_BULLET_H

#include "curses_drawable_objects.h"

class Bullet {
public:
    float pos_x;
    float pos_y;
    float speed_x;
    float speed_y;
    drawable::Bullet drawable_bullet;
    void draw();
    void tick();
    WINDOW* parent_window;
    Bullet(WINDOW* parent_window, int pos_y, int pos_x, int speed_y, int speed_x);
};


#endif //ROBOCODE_BULLET_H
