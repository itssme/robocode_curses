/* 
 * author: Joel Klimont
 * filename: game_objects.h
 * date: 26/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/
#ifndef ROBOCODE_GAME_OBJECTS_H
#define ROBOCODE_GAME_OBJECTS_H

#include <cmath>
#include "curses_drawable_objects.h"

namespace GameObjects {

    class GameObject {
    public:
        double pos_height;
        double pos_width;
        double speed_height;
        double speed_width;
        GameObject(double pos_height, double pos_width, double speed_height, double speed_width) {
            this->pos_height = pos_height;
            this->pos_width = pos_width;
            this->speed_height = speed_height;
            this->speed_width = speed_width;
        }
        virtual void draw()= 0;
        virtual void tick()= 0;
    };

    class Bullet : public GameObject {
    public:
        drawable::Bullet drawable_bullet;
        void draw();
        void tick();
        WINDOW *parent_window;
        Bullet(WINDOW *parent_window, double pos_height, double pos_width, double speed_height, double speed_width);
    };

    class Robot : public GameObject {
    public:
        double gun_degree;
        double energy;
        drawable::Robot drawable_robot;
        void draw() override;
        Bullet shoot();
        void tick() override;
        bool check_collision(Robot robot); // check if this robot has collided with the one provided
        bool check_collision(Bullet bullet);
        void set_speed(double speed_height, double speed_width);
        void set_pos(double pos_height, double pos_width);
        void set_gun_rotation(double degrees);
        WINDOW *parent_window;
        Robot(WINDOW *parent_window, drawable::Robot drawable_robot);
    };

}


#endif //ROBOCODE_GAME_OBJECTS_H
