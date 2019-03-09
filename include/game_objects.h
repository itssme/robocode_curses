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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#pragma GCC diagnostic pop


namespace GameObjects {

    class GameObject {
    public:
        double pos_height;
        double pos_width;
        double speed_height;
        double speed_width;
        int height;
        int width;
        GameObject(double pos_height, double pos_width, double speed_height, double speed_width, int height, int width) {
            this->pos_height = pos_height;
            this->pos_width = pos_width;
            this->speed_height = speed_height;
            this->speed_width = speed_width;
            this->height = height;
            this->width = width;
        }
        virtual ~GameObject() {}
        virtual void tick()= 0;
    };

    class Bullet : public GameObject {
    public:
        drawable::Bullet drawable_bullet;
        void draw();
        void tick() override;
        WINDOW *parent_window;
        int created_by;
        Bullet(WINDOW *parent_window, double pos_height, double pos_width, double speed_height, double speed_width, int created_by = -1);
    };

    class BasicRobot : public GameObject {
    public:
        double gun_degree;
        double gun_speed;
        double energy;
        Bullet shoot(WINDOW* parent_window);
        void tick() override;
        bool check_collision(BasicRobot robot); // check if this robot has collided with the one provided
        bool check_collision(Bullet bullet);
        void set_speed(double speed_height, double speed_width);
        void set_pos(double pos_height, double pos_width);
        virtual void set_gun_rotation(double degrees);
        int id;
        BasicRobot(double pos_height, double pos_width, double speed_height,double speed_width,
                   int energy, double gun_rotation, int width = 4, int height = 7, int id = -1);
    };

    // extend BasicRobot and make it drawable
    class Robot : public BasicRobot {
    public:
        void draw();
        void set_gun_rotation(double degrees) override;
        WINDOW *parent_window;
        drawable::Robot drawable_robot;
        Robot(WINDOW *parent_window, drawable::Robot drawable_robot, int id = -1);
    };

} // namespace GameObjects


#endif //ROBOCODE_GAME_OBJECTS_H
