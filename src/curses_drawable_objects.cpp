/* 
 * author: Joel Klimont
 * filename: curses_utils.cpp
 * date: 03/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include <curses_drawable_objects.h>
#include <cmath>
#include <string>

/*!
 * Move a drawable object
 * @param pos_height new position in y
 * @param pos_width new position in x
 */
void drawable::DrawableObject::move(int pos_height, int pos_width) {
    this->pos_height = pos_height;
    this->pos_width = pos_width;
}

/*!
 * Create a new robot
 * @param parent_window the robot will be drawn on
 * @param begin_height y position of the robot
 * @param begin_width x position of the robot
 * @param height of the robot
 * @param width of the robot
 */
drawable::Robot::Robot(WINDOW *parent_window, int begin_height, int begin_width, int height, int width) : DrawableObject() {
    this->pos_width = begin_width;
    this->pos_height = begin_height;
    this->width = width;
    this->height = height;
    this->parent_window = parent_window;
    this->window = derwin(parent_window, height, width, begin_height, begin_width);
    this->gun_rotation = 0;
    this->gun_x[0] = 1;
    this->gun_y[0] = 0;
    box(this->window, 0, 0);
}

/*!
 * Set the nwe gun rotation
 * @param new_rotation in degrees
 */
void drawable::Robot::set_gun_rotation(float new_rotation) {
    this->gun_rotation = new_rotation;
}

/*!
 * Draw the gun
 */
void drawable::Robot::draw_gun() {
    auto X = (this->height/1.25) * std::cos(((this->gun_rotation)*M_PI)/180);
    auto Y = (this->height/1.25) * std::sin(((this->gun_rotation)*M_PI)/180);
    this->gun_y[0] = static_cast<int>(std::round(Y) + (this->height / 2.0));
    this->gun_x[0] = static_cast<int>(std::round(X) + (this->width / 2.0));
    this->gun_y[1] = this->gun_y[0];
    this->gun_x[1] = this->gun_x[0];

    for (int i = 0; i < 2; ++i) {
        if (this->gun_y[i] > this->height-(i+1)) {
            this->gun_y[i] = this->height-(i+1);
        } else if (this->gun_y[i] < i) {
            this->gun_y[i] = i;
        }

        if (this->gun_x[i] > this->width-(i+1)) {
            this->gun_x[i]  = this->width-(i+1);
        } else if (this->gun_x[i] < i) {
            this->gun_x[i] = i;
        }
        mvwaddch(this->window, this->gun_y[i], this->gun_x[i], '#');
    }
    wrefresh(this->parent_window);
}

/*!
 * Draw all components of the robot
 */
void drawable::Robot::draw() {
    werase(this->window);
    this->refresh();
    this->window = derwin(parent_window, this->height, this->width, pos_height, pos_width);
    box(this->window, 0, 0);
    draw_gun();
    this->refresh();
}

/*!
 * Refresh the subwindow of the robot
 */
void drawable::Robot::refresh() {
    touchwin(this->window);
    wrefresh(this->window);
}

/*!
 * Create a new bullet robot
 * @param parent_window parent_window the bullet will be drawn on
 * @param begin_height y position of the bullet
 * @param begin_width x position of the bullet
 * @param width of the bullet
 * @param height of the bullet
 */
drawable::Bullet::Bullet(WINDOW *parent_window, int begin_height, int begin_width, int width, int height) {
    this->pos_width = begin_width;
    this->pos_height = begin_height;
    this->width = width;
    this->height = height;
    this->parent_window = parent_window;
    this->window = derwin(parent_window, height, width, begin_height, begin_width);
    box(this->window, 0, 0);
    this->refresh();
}

/*!
 * Draw the bullet
 */
void drawable::Bullet::draw() {
    werase(this->window);
    this->refresh();
    this->window = derwin(parent_window, this->height, this->width, pos_height, pos_width);
    box(this->window, 0, 0);
    this->refresh();
}

/*!
 * Refresh the subwindow of the bullet
 */
void drawable::Bullet::refresh() {
    touchwin(this->window);
    wrefresh(this->window);
}
