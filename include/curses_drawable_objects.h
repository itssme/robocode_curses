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

    /*!
     * Basic drawable object \n
     * Has a position, height, width
     * and a own window where the
     * object will be rendered on.
     */
    class DrawableObject {
    public:
        int pos_width;
        int pos_height;
        int width;
        int height;
        WINDOW *window;
        WINDOW *parent_window;
        void move(int pos_height, int pos_width);
    };

    /*!
     * Drawable robot which posses a
     * gun that can be draw.
     */
    class Robot : public DrawableObject {
    public:
        double gun_rotation{0};
        int gun_x[2];
        int gun_y[2];
        Robot(WINDOW *parent_window, int begin_height, int begin_width, int width = 4, int height = 7);
        void set_gun_rotation(float new_rotation);
        void draw_gun();
        void draw();
        void refresh();
    };

    /*!
     * Drawable bullet which can  be draw
     * using the draw function.
     */
    class Bullet : public DrawableObject {
    public:
        Bullet(WINDOW *parent_window, int begin_height, int begin_width, int width = 2, int height = 2);
        void draw();
        void refresh();
    };
}

#endif //ROBOCODE_CURSES_DRAWABLE_OBJECTS_H
