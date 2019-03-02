/* 
 * author: Joel Klimont
 * filename: robot.cpp
 * date: 31/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include "game_objects.h"

GameObjects::BasicRobot::BasicRobot(double pos_height, double pos_width, double speed_height, double speed_width,
                                    int energy, double gun_rotation, int height, int width) : GameObject(pos_height, pos_width, speed_height,
                                                                        speed_width, height, width) {
    this->energy = energy;
    this->gun_degree = gun_rotation;
    this->gun_speed = 0;
}

GameObjects::Robot::Robot(WINDOW* parent_window, drawable::Robot drawable_robot) :
        BasicRobot(drawable_robot.pos_height, drawable_robot.pos_width, 0, 0, 100, 0, drawable_robot.height,
                   drawable_robot.width),
        parent_window(parent_window),
        drawable_robot(drawable_robot) {}

void GameObjects::BasicRobot::tick() {
    this->pos_height += this->speed_height;
    this->pos_width += this->speed_width;
    this->gun_degree += this->gun_speed;
}

bool GameObjects::BasicRobot::check_collision(BasicRobot robot) {
    return false;
}

GameObjects::Bullet GameObjects::BasicRobot::shoot(WINDOW* parent_window) {
}

bool GameObjects::BasicRobot::check_collision(Bullet bullet) {
    return bullet.pos_height >= this->pos_height && bullet.pos_height <= this->pos_height + this->height
           && bullet.pos_width >= this->pos_width && bullet.pos_width <= this->pos_width + this->width;
}

void GameObjects::BasicRobot::set_speed(double speed_height, double speed_width) {
    this->speed_height = speed_height;
    this->speed_width = speed_width;
}

void GameObjects::BasicRobot::set_pos(double pos_height, double pos_width) {
    this->pos_height = pos_height;
    this->pos_width = pos_width;
}

void GameObjects::BasicRobot::set_gun_rotation(double degrees) {
    this->gun_degree = fmod(degrees, 360);
}

void GameObjects::Robot::draw() {
    this->drawable_robot.pos_height = static_cast<int>(this->pos_height);
    this->drawable_robot.pos_width = static_cast<int>(pos_width);
    this->drawable_robot.draw();
}

void GameObjects::Robot::set_gun_rotation(double degrees) {
    this->gun_degree = fmod(degrees, 360);
    this->drawable_robot.set_gun_rotation(degrees);
}
