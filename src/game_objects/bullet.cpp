/* 
 * author: Joel Klimont
 * filename: bullet.cpp
 * date: 11/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include "game_objects.h"

/*!
 * Create a new bullet
 * @param parent_window window of the game
 * @param pos_height position y o the bullet
 * @param pos_width position x f the bullet
 * @param speed_height speed y of the bullet
 * @param speed_width speed x of the bullet
 * @param created_by robot that has shot the bullet
 */
GameObjects::Bullet::Bullet(WINDOW *parent_window, double pos_height, double pos_width, double speed_height, double speed_width, int created_by) :
        GameObject(pos_height, pos_width, speed_height, speed_width, 1, 1), drawable_bullet(parent_window,
                                                                                      static_cast<int>(pos_height),
                                                                                      static_cast<int>(pos_width)) {
    this->parent_window = parent_window;
    this->speed_height = speed_height;
    this->speed_width = speed_width;
    this->created_by = created_by;
}

/*!
 * Tick the bullet and apply the
 * speed to the position.
 */
void GameObjects::Bullet::tick() {
    this->pos_height += this->speed_height;
    this->pos_width += this->speed_width;
}

/*!
 * Draw the bullet.
 */
void GameObjects::Bullet::draw() {
    this->drawable_bullet.pos_height = static_cast<int>(pos_height);
    this->drawable_bullet.pos_width = static_cast<int>(pos_width);
    this->drawable_bullet.draw();
}
