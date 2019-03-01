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
#undef OK

class Player {
public:
    GameObjects::BasicRobot robot;
    Player(GameObjects::BasicRobot robot) : robot(std::move(robot)) {};

    // hitWall:
    // 1 is left
    // 2 is top
    // 3 is right
    // 4 is down
    virtual GameObjects::BasicRobot tick(std::vector<GameObjects::BasicRobot> scanned, std::vector<int> hit_wall) = 0;
};

#endif //ROBOCODE_PLAYER_H
