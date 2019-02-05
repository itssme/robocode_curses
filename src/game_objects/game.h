/* 
 * author: Joel Klimont
 * filename: game.h
 * date: 01/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#ifndef ROBOCODE_GAME_H
#define ROBOCODE_GAME_H

#include <curses.h>
#include <vector>

#include "robot.h"

class Game {
private:
    WINDOW* window;
    std::vector<Robot> robots;

public:
    Game(WINDOW* window);
    void game_loop(bool& running);
    void tick_all();
    void draw_all();

};


#endif //ROBOCODE_GAME_H
