/* 
 * author: Joel Klimont
 * filename: bullet.cpp
 * date: 11/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include "bullet.h"

Bullet::Bullet(WINDOW *parent_window, int pos_y, int pos_x, int speed_y, int speed_x) : drawable_bullet(parent_window, pos_y, pos_x, 1, 1) {
    this->parent_window = parent_window;
    this->speed_y = speed_y;
    this->speed_x = speed_x;
}
