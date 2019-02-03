/* 
 * author: Joel Klimont
 * filename: curses_drawable_objects.h
 * date: 03/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#ifndef ROBOCODE_CURSES_DRAWABLE_OBJECTS_H
#define ROBOCODE_CURSES_DRAWABLE_OBJECTS_H

#include <ncurses.h>

namespace drawable {
    class Robot {
    public:
        int pos_x;
        int pos_y;
        int size_x;
        int size_y;
        float gun_rotation{0};
        int gun_x{1};
        int gun_y{0};
        WINDOW *window;
        WINDOW *parent_window;
        Robot(WINDOW *parent_window, int begin_y, int begin_x, int size_y = 4, int size_x = 7);
        void set_gun_rotatoin(float new_rotation);
        void move(int pos_y, int pos_x);
        void refresh();
    };
}

#endif //ROBOCODE_CURSES_DRAWABLE_OBJECTS_H
