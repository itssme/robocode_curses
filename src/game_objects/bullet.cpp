/* 
 * author: Joel Klimont
 * filename: bullet.cpp
 * date: 11/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include "game_objects.h"

GameObjects::Bullet::Bullet(WINDOW *parent_window, double pos_height, double pos_width, double speed_height, double speed_width) :
        GameObject(pos_height, pos_width, speed_height, speed_width), drawable_bullet(parent_window,
                                                                                      static_cast<int>(pos_height),
                                                                                      static_cast<int>(pos_width), 2, 2) {
    this->parent_window = parent_window;
    this->speed_height = speed_height;
    this->speed_width = speed_width;
}

void GameObjects::Bullet::tick() {
    this->pos_height += this->speed_height;
    this->pos_width += this->speed_width;
}

void GameObjects::Bullet::draw() {
    this->drawable_bullet.pos_height = static_cast<int>(pos_height);
    this->drawable_bullet.pos_width = static_cast<int>(pos_width);
    this->drawable_bullet.draw();
}
