/* 
 * author: Joel Klimont
 * filename: robot.cpp
 * date: 31/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include "game_objects.h"

GameObjects::Robot::Robot(WINDOW* parent_window, drawable::Robot drawable_robot) : GameObject(drawable_robot.pos_height,
                                                                                 drawable_robot.pos_width, 0, 0),
                                                                      drawable_robot(drawable_robot) {
    this->parent_window = parent_window;
}

void GameObjects::Robot::tick() {
    this->pos_height += this->speed_height;
    this->pos_width += this->speed_width;
}

void GameObjects::Robot::draw() {
    this->drawable_robot.pos_height = static_cast<int>(this->pos_height);
    this->drawable_robot.pos_width = static_cast<int>(pos_width);
    this->drawable_robot.draw();
}

bool GameObjects::Robot::check_collision(Robot robot) {
    return false;
}

bool GameObjects::Robot::check_collision(Bullet bullet) {
    return false;
}

GameObjects::Bullet GameObjects::Robot::shoot() {

}

void GameObjects::Robot::set_speed(double speed_height, double speed_width) {
    this->speed_height = speed_height;
    this->speed_width = speed_width;
}

void GameObjects::Robot::set_pos(double pos_height, double pos_width) {
    this->pos_height = pos_height;
    this->pos_width = pos_width;
}

void GameObjects::Robot::set_gun_rotation(double degrees) {
    this->gun_degree = fmod(degrees, 360);
    this->drawable_robot.set_gun_rotation(degrees);
}
