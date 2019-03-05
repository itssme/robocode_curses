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
#include <random>
#include <algorithm>

#include "game_objects.h"
#include "spdlog/spdlog.h"
#undef OK
#include "server.h"
#define OK      (0)

class Game {
private:
    WINDOW* window;
    std::vector<GameObjects::Bullet> bullets;
    std::vector<GameObjects::Robot> robots;
    std::unique_ptr<Server> server;
public:
    ServerImpl service;
    std::thread* server_thread;
    Game(WINDOW* window, std::string server_address);
    void start();
    void game_loop(bool& running);
    void tick_all();
    void draw_all();
    void shutdown_server();
};


#endif //ROBOCODE_GAME_H
