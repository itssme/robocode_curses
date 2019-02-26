/* 
 * author: Joel Klimont
 * filename: game.h
 * date: 01/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#ifndef ROBOCODE_GAME_H
#define ROBOCODE_GAME_H

#include <vector>
#include <ctime>
#include <chrono>
#include <thread>


#include "game_objects.h"
#include "spdlog/spdlog.h"
#undef OK
#include "server.h"
#define OK      (0)

class Game {
private:
    WINDOW* window;
    std::vector<GameObjects::Robot> robots;
public:
    ServerImpl service;
    std::thread* server_thread;
    Game(WINDOW* window, std::string server_address);
    void game_loop(bool& running);
    void tick_all();
    void draw_all();

};


#endif //ROBOCODE_GAME_H
