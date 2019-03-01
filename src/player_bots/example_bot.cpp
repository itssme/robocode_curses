/* 
 * author: Joel Klimont
 * filename: example_bot.cpp
 * date: 28/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/
#include <utility>
#include "player.h"

class ExampleBot : public Player {
    ExampleBot(GameObjects::BasicRobot robot) : Player(std::move(robot)) {};
    GameObjects::BasicRobot tick(std::vector<GameObjects::BasicRobot> scanned, std::vector<int> hit_wall) override {
        if (! hit_wall.empty()) {
            if (hit_wall.at(0) == 1) {
                this->robot.set_speed(1, 0);
            } else if (hit_wall.at(0) == 2) {
                this->robot.set_speed(0, 1);
            } else if (hit_wall.at(0) == 3) {
                this->robot.set_speed(-1, 0);
            } else if (hit_wall.at(0) == 4) {
                this->robot.set_speed(0, -1);
            }
        }

        return this->robot;
    };
};