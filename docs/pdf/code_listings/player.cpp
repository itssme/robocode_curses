bool tick(GameObjects::BasicRobot* scanned,
          std::vector<int> hit_wall) {
    bool shot = false;

    // find first wall
    if (at_wall == -1) {
        this->robot.set_speed(1, 0);
        if (! hit_wall.empty()) {
            at_wall = hit_wall.at(0);
            this->robot.set_speed(0, -1);
        }
        return shot;
    }

    // cycle along walls
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

    // if a robot is scanned shoot
    if (scanned != nullptr) {
        shot = true;
    }
    this->robot.gun_degree += 10;
    this->robot.gun_degree = std::fmod(this->robot.gun_degree, 360);
    this->robot.gun_speed = 10;
    
    return shot;
};