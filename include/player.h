/* 
 * author: Joel Klimont
 * filename: player.h
 * date: 23/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#ifndef ROBOCODE_PLAYER_H
#define ROBOCODE_PLAYER_H

#include <utility>
#include "messages.pb.h"
#include "game_objects.h"

class Player {
public:
    GameObjects::Robot robot;
    Player(GameObjects::Robot robot) : robot(std::move(robot)) {};
    virtual GameObjects::Robot tick(std::vector<GameObjects::BasicRobot> scanned, std::vector<int> hit_wall) = 0;
};

#endif //ROBOCODE_PLAYER_H
