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

drawable::Robot::Robot(WINDOW *parent_window, int begin_y, int begin_x, int height, int width) {
    this->pos_x = begin_x;
    this->pos_y = begin_y;
    this->width = width;
    this->height = height;
    this->parent_window = parent_window;
    this->window = derwin(parent_window, height, width, begin_y, begin_x);
    this->gun_rotation = 0;
    this->gun_x = 1;
    this->gun_y = 0;
    box(this->window, 0, 0);
    this->refresh();
}

void drawable::Robot::set_gun_rotation(float new_rotation) {
    this->gun_rotation = new_rotation;
}

void drawable::Robot::draw_gun() {
    auto X = (this->height) * std::cos(((this->gun_rotation)*M_PI)/180);
    auto Y = (this->height) * std::sin(((this->gun_rotation)*M_PI)/180);
    this->gun_y = std::round(Y)+(this->height/2)-1;
    this->gun_x = std::round(X)+(this->width/2);

    if (this->gun_y > this->height-1) {
        this->gun_y = this->height-1;
    } else if (this->gun_y < 0) {
        this->gun_y = 0;
    }

    if (this->gun_x > this->width-1) {
        this->gun_x  = this->width-1;
    } else if (this->gun_x < 0) {
        this->gun_x = 0;
    }

    mvwaddch(this->window, this->gun_y, this->gun_x, '#');
    wrefresh(this->parent_window);
}

void drawable::Robot::draw(bool remove_old) {
    werase(this->window);
    if (remove_old) {
        this->refresh();
    }
    this->window = derwin(parent_window, this->height, this->width, pos_y, pos_x);
    box(this->window, 0, 0);
    draw_gun();
    this->refresh();
}

void drawable::Robot::move(int pos_y, int pos_x) {
    this->pos_y = pos_y;
    this->pos_x = pos_x;
}

void drawable::Robot::refresh() {
    touchwin(this->window);
    wrefresh(this->window);
}
