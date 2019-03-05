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
    explicit Player(GameObjects::BasicRobot robot) : robot(std::move(robot)) {};

    int at_wall = -1;
    int shoot = 20;

    // hitWall:
    // 1 is left
    // 2 is top
    // 3 is right
    // 4 is down
    // return true if robot should shoot
    bool tick(GameObjects::BasicRobot* scanned, std::vector<int> hit_wall) {
        this->robot.gun_degree += 10;
        this->robot.gun_degree = std::fmod(this->robot.gun_degree, 360);
        this->robot.gun_speed = 10;

        bool shot = false;

        if (shoot == 0) {
            shoot = 2;
            shot = true;
        } else {
            shoot -= 1;
        }

        // find first wall
        if (at_wall == -1) {
            this->robot.set_speed(1, 0);

            if (! hit_wall.empty()) {
                at_wall = hit_wall.at(0);

                this->robot.set_speed(0, -1);
            }
            return shot;
        }

        if (! hit_wall.empty()) {
            int wall = 0;
            for (int possible_new_hit: hit_wall) {
                if (possible_new_hit != at_wall) {
                    wall = possible_new_hit;
                }
            }

            if (wall == 1 && wall != at_wall) {
                at_wall = wall;
                this->robot.set_speed(-1, 0);

            } else if (wall == 2 && wall != at_wall) {
                at_wall = wall;
                this->robot.set_speed(0, 1);

            } else if (wall == 3 && wall != at_wall) {
                at_wall = wall;
                this->robot.set_speed(1, 0);

            } else if (wall == 4 && wall != at_wall) {
                at_wall = wall;
                this->robot.set_speed(0, -1);
            }
        }

        if (scanned != nullptr) {
            std::cout << "scanned a robot" << std::endl;
        }

        return shot;
    };
};

#endif //ROBOCODE_PLAYER_H
