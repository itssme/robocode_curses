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
    class DrawableObject {
    public:
        int pos_x;
        int pos_y;
        int width;
        int height;
        WINDOW *window;
        WINDOW *parent_window;
        void move(int pos_y, int pos_x);
    };

    class Robot : public DrawableObject {
    public:
        float gun_rotation{0};
        int gun_x[2];
        int gun_y[2];
        Robot(WINDOW *parent_window, int begin_y, int begin_x, int width = 4, int height = 7);
        void set_gun_rotation(float new_rotation);
        void draw_gun();
        void draw(bool remove_old= true);
        void refresh();
    };

    class Bullet : public DrawableObject {
    public:
        Bullet(WINDOW *parent_window, int begin_y, int begin_x, int width = 1, int height = 1);
        void draw(bool remove_old= true);
        void refresh();
    };
}

#endif //ROBOCODE_CURSES_DRAWABLE_OBJECTS_H
