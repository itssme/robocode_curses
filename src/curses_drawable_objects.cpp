/* 
 * author: Joel Klimont
 * filename: curses_utils.cpp
 * date: 03/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include <curses_drawable_objects.h>

drawable::Robot::Robot(WINDOW *parent_window, int begin_y, int begin_x, int size_y, int size_x) {
    this->pos_x = begin_x;
    this->pos_y = begin_y;
    this->size_x = size_x;
    this->size_y = size_y;
    this->parent_window = parent_window;
    this->window = derwin(parent_window, size_y, size_x, begin_y, begin_x);
    this->gun_rotation = 0;
    this->gun_x = 1;
    this->gun_y = 0;
    box(this->window, 0, 0);
    mvwaddch(this->window, this->gun_y, this->gun_x, '#');
    this->refresh();
}

void drawable::Robot::set_gun_rotatoin(float new_rotation) {
    this->gun_rotation = new_rotation;

}

void drawable::Robot::move(int pos_y, int pos_x) {
    this->pos_y = pos_y;
    this->pos_x = pos_x;
    werase(this->window);
    this->refresh();
    this->window = derwin(parent_window, this->size_y, this->size_x, pos_y, pos_x);
    box(this->window, 0, 0);
    //mvwaddch(this->window, this->gun_y, this->gun_x, '#');
    this->refresh();
}

void drawable::Robot::refresh() {
    touchwin(this->window);
    wrefresh(this->window);
}
