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
    ExampleBot(GameObjects::Robot robot) : Player(std::move(robot)) {};
    GameObjects::Robot tick(std::vector<GameObjects::BasicRobot> scanned, std::vector<int> hit_wall) override {


        return this->robot;
    };
};