/* 
 * author: Joel Klimont
 * filename: game.cpp
 * date: 01/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include <ctime>
#include <chrono>
#include <thread>

#include "game.h"

Game::Game(WINDOW *window) {
    this->window = window;
}

void Game::game_loop(bool& running) {
    while (running) {
        auto start = std::chrono::steady_clock::now();

        this->tick_all();
        this->draw_all();

        auto end = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)-(end-start));
    }
}

void Game::tick_all() {
    // TODO: send requests to get new data from client and get the future object
    // TODO: get the future object and tick the object
}

void Game::draw_all() {
    for (auto robot:this->robots) {
        robot.draw();
    }
}
