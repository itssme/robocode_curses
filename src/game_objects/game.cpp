/* 
 * author: Joel Klimont
 * filename: game.cpp
 * date: 01/02/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include "game.h"

Game::Game(WINDOW *window, std::string server_address) {
    this->window = window;

    server_thread = new std::thread([&]{
        ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());  // this will not throw an error if the port is already in use
        builder.RegisterService(&service);
        server = builder.BuildAndStart();
        spdlog::info("Server listening on " + server_address);
        //std::cout << "Server listening on " << server_address << std::endl;
        server->Wait();
    });

    this->ticks = 0;
}

void Game::game_loop(bool& running, StreamingServer& stream) {
    while (this->robots.size() > 1 && running) {
        auto start = std::chrono::steady_clock::now();
        this->tick_all();

        if (robots.size() <= 1) {
            continue;
        }

        this->draw_all();

        for (int i = 0; i < MODIFY_TICK; ++i) {
            auto start_mod_tick= std::chrono::steady_clock::now();

            this->tick_modify(MODIFY_TICK);

            /* // TODO: test this
            for (auto bullet: this->bullets) {
                bullet.draw();
            }
             */

            this->draw_all();

            auto end_mod_tick = std::chrono::steady_clock::now();
            std::this_thread::sleep_for(std::chrono::milliseconds(TICK/MODIFY_TICK)-(end_mod_tick-start_mod_tick));
        }

        this->send_stream(stream);
        this->ticks += 1;

        auto end = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(TICK)-(end-start));
    }

    // add last robot to scores
    if (this->robots.size() != 0) {
        this->game_results.push_back(std::tuple<int, std::string, double>(this->robots.at(0).id,
                                                                          this->service.connections.at(
                                                                                  this->get_connection_from_robot(
                                                                                          this->robots.at(
                                                                                                  0).id))->username,
                                                                          ticks + 10));
    }

    this->shutdown_server();
    this->cleanup();
}

void Game::send_stream(StreamingServer& stream) {
    shared::StreamingUpdate update;

    for (const auto &robot: this->robots) {
        shared::StreamingRobot *strm_robot = update.add_robots();

        strm_robot->mutable_pos()->set_y(static_cast<int>(robot.pos_height));
        strm_robot->mutable_pos()->set_x(static_cast<int>(robot.pos_width));
        strm_robot->mutable_gun_pos()->set_degrees(static_cast<int>(robot.gun_degree));
        strm_robot->set_energy_left(robot.energy);
    }

    for (const auto &bullet: this->bullets) {
        shared::StreamingBullet *strm_bullet = update.add_bullets();

        strm_bullet->mutable_pos()->set_y(static_cast<int>(bullet.pos_height));
        strm_bullet->mutable_pos()->set_x(static_cast<int>(bullet.pos_width));
    }

    stream.send_to_all(update);
}

void Game::tick_modify(int tick_modifier) {
    for (unsigned int i = 0; i < this->bullets.size(); i++) {
        this->bullets.at(i).pos_height += this->bullets.at(i).speed_height / tick_modifier;
        this->bullets.at(i).pos_width += this->bullets.at(i).speed_width / tick_modifier;
    }
}

void Game::tick_all() {
    std::vector<std::tuple<unsigned long int, shared::UpdateFromClient>> messages;

    for (unsigned int i = 0; i < this->robots.size(); i++) {
        if (robots.at(i).energy <= 0) {
            continue;
        }

        for (auto &robot: this->robots) {
            if (this->robots.at(i).id != robot.id && this->robots.at(i).check_collision(robot)) {
                this->robots.at(i).pos_height -= this->robots.at(i).speed_height;
                this->robots.at(i).pos_width -= this->robots.at(i).speed_width;
                this->robots.at(i).set_speed(0.0, 0.0);
            }
        }

        robots.at(i).tick();

        for (auto &robot: this->robots) {
            if (this->robots.at(i).id != robot.id && this->robots.at(i).check_collision(robot)) {
                // revert last tick
                this->robots.at(i).pos_height -= this->robots.at(i).speed_height;
                this->robots.at(i).pos_width -= this->robots.at(i).speed_width;

                this->robots.at(i).set_speed(0.0, 0.0);
            }
        }

        // Wall collision
        std::vector<int> hit_wall;
        if (robots.at(i).pos_height >= LINES -(robots.at(i).drawable_robot.height + 1)) {
            this->robots.at(i).pos_height = LINES - (robots.at(i).drawable_robot.height + 1);
            hit_wall.push_back(4);
        } else if (robots.at(i).pos_height <= 1) {
            this->robots.at(i).pos_height = 1;
            hit_wall.push_back(2);
        }

        if (robots.at(i).pos_width >= COLS -(robots.at(i).drawable_robot.width + 1)) {
            this->robots.at(i).pos_width = COLS - (robots.at(i).drawable_robot.width + 1);
            hit_wall.push_back(3);
        } else if (robots.at(i).pos_width <= 1) {
            this->robots.at(i).pos_width= 1;
            hit_wall.push_back(1);
        }

        // Bullet collision
        std::vector<GameObjects::Bullet> survived_bullets;
        for (auto &bullet: this->bullets) {
            if (robots.at(i).check_collision(bullet) && robots.at(i).id != bullet.created_by) {
                spdlog::debug("HIT BECAUSE CHECK COLLISION IS: {} AND ID ROBOT: {} AND ID BULLET: {}",
                              robots.at(i).check_collision(bullet), robots.at(i).id, bullet.created_by);
                robots.at(i).energy -= 20;
                if (robots.at(i).energy <= 0) {
                    robots.at(i).energy = -1;
                }
            } else if (! (bullet.pos_width >= COLS || bullet.pos_height >= LINES ||
                          bullet.pos_height <= 0 || bullet.pos_width <= 0)) {
                survived_bullets.push_back(bullet);
            }
        }
        this->bullets = survived_bullets;

        shared::UpdateFromServer update;

        update.set_energy(static_cast<google::protobuf::int32>(robots.at(i).energy));
        update.mutable_pos()->set_y(this->robots.at(i).pos_height);
        update.mutable_pos()->set_x(this->robots.at(i).pos_width);

        for (auto hit: hit_wall) {
            update.add_hitwall(hit);
        }

        for (const auto &robot_scan: this->robots) {
            if (robot_scan.id == this->robots.at(i).id) {
                continue;
            }

            auto nx = cos((std::fmod(this->robots.at(i).gun_degree, 360)*M_PI)/180);
            auto ny = sin((std::fmod(this->robots.at(i).gun_degree, 360)*M_PI)/180);

            auto px = this->robots.at(i).pos_width;
            auto py = this->robots.at(i).pos_height;

            auto rx = robot_scan.pos_width;
            auto ry = robot_scan.pos_height;
            auto rw = robot_scan.width;
            auto rh = robot_scan.height;

            auto upper_limit_x = (rx - px) / nx;
            auto upper_limit_y =  (ry - py) / ny;
            auto lower_limit_x = (rx + rw - px) / nx;
            auto lower_limit_y  = (ry + rh - py) / ny;

            if (nx >= 0) {
                auto temp = upper_limit_x;
                upper_limit_x = lower_limit_x;
                lower_limit_x = temp;
            }

            if (ny >= 0) {
                auto temp = upper_limit_y;
                upper_limit_y = lower_limit_y;
                lower_limit_y = temp;
            }

            auto max_alpha = std::min(upper_limit_x, upper_limit_y);
            auto min_alpha = std::max(lower_limit_x, lower_limit_y);

            if (min_alpha <= max_alpha && min_alpha > 0 && max_alpha > 0) {
                update.mutable_scanned_robot()->mutable_pos()->set_y(robot_scan.height);
                update.mutable_scanned_robot()->mutable_pos()->set_x(robot_scan.width);
                update.mutable_scanned_robot()->set_id(robot_scan.id);
                update.mutable_scanned_robot()->set_energy_left(robot_scan.energy);
            }
        }

        messages.emplace_back(i, this->service.connections.at(this->get_connection_from_robot(robots.at(i).id))->send_message(update));
    }

    for (unsigned long int i = 0; i < messages.size(); ++i) {
        shared::UpdateFromClient update_client = std::get<1>(messages.at(i));

        // player has not sent an update -> using speed values of last update
        // TODO: if player does not respond after 5 messages, declare connection lost
        if (update_client.pos().y() == -1) {
            continue;
        }

        unsigned long int robot_index = std::get<0>(messages.at(i));

        if (this->robots.at(robot_index).energy <= 0) {
            this->game_results.emplace_back(this->robots.at(robot_index).id,
                                            this->service.connections.at(this->get_connection_from_robot(this->robots.at(robot_index).id))->username,
                                            ticks);
            continue;
        }

        // create bullet object if player shot
        if (update_client.shot()) {
            this->bullets.push_back(this->robots.at(robot_index).shoot(this->window));
        }

        // update position of player
        auto y_diff = fabs(((this->robots.at(robot_index).pos_height + update_client.speed().y()) - this->robots.at(robot_index).speed_height) - update_client.pos().y());
        auto x_diff = fabs(((this->robots.at(robot_index).pos_width  + update_client.speed().x()) - this->robots.at(robot_index).speed_width) - update_client.pos().x());

        if (y_diff >= 1 || x_diff >= 1) {
            this->robots.at(robot_index).energy -= 20;
            spdlog::info("{} tried to cheat!", this->robots.at(robot_index).id);
            this->robots.at(robot_index).set_pos(update_client.pos().y(), update_client.pos().x());
        }

        // update speed of player
        if (fabs(update_client.speed().y()) <= 1 && fabs(update_client.speed().x() <= 1)) {
            this->robots.at(robot_index).set_speed(update_client.speed().y(), update_client.speed().x());
        }

        this->robots.at(robot_index).set_gun_rotation(update_client.gun_pos().degrees());
        this->robots.at(robot_index).gun_speed = update_client.gun_pos().speed();

        if (this->robots.at(robot_index).energy <= 0) {
            this->game_results.emplace_back(this->robots.at(robot_index).id,
                                            this->service.connections.at(this->get_connection_from_robot(this->robots.at(robot_index).id))->username,
                                            ticks);
            continue;
        }
    }

    std::vector<GameObjects::Robot> new_robots;

    for (unsigned int i = 0; i < this->robots.size(); i++) {
        if (this->robots.at(i).energy > 0) {
            new_robots.push_back(this->robots.at(i));
        }
    }

    this->robots = new_robots;
}

void Game::draw_all() {
    werase(this->window);
    box(this->window, 0 , 0);

    for (auto bullet: this->bullets) {
        bullet.draw();
    }

    for (auto robot: this->robots) {
        robot.draw();
    }

    wrefresh(this->window);
}

void Game::start() {
    this->robots.reserve(this->service.connections.size());

    std::random_device rd;
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<double> dis_y{5, (double) LINES - 5};
    std::uniform_real_distribution<double> dis_x{5, (double) COLS - 5};

    for (unsigned int i = 0; i < this->service.connections.size(); i++) {
        bool intersects{false};

        // loop until the created robot does not intersect with another robot
        do {
            intersects = false;

            drawable::Robot robot_draw(this->window,
                                       static_cast<int>(dis_y(gen)),
                                       static_cast<int>(dis_x(gen)));

            GameObjects::Robot robot(this->window, robot_draw, this->service.connections.at(i)->id);

            for (auto &collision_robot: this->robots) {
                if (robot.id != collision_robot.id && robot.check_collision(collision_robot)) {
                    intersects = true;
                }
            }

            if (! intersects) {
                robot.energy = 100;
                this->robots.push_back(robot);
            }

        } while (intersects);
    }
}

unsigned long int Game::get_connection_from_robot(int robot_id) {
    for (unsigned long int i = 0; i <= this->service.connections.size(); i++) {
        if (this->service.connections.at(i)->id == robot_id) {
            return i;
        }
    }
    return static_cast<unsigned long>(-1);
}

unsigned long int Game::get_robot_from_connection(int connection_index) {
    for (unsigned long int i = 0; i <= this->robots.size(); i++) {
        if (this->robots.at(i).id == connection_index) {
            return i;
        }
    }
    return static_cast<unsigned long>(-1);
}

std::vector<std::tuple<int, std::string, double>> Game::get_results() {
    auto results = this->game_results;

    // sort by time the robot has been alive (calculated in ticks)
    std::sort(results.begin(), results.end(), [](std::tuple<int, std::string, double> &sc1, std::tuple<int, std::string, double> &sc2) {
        return (std::get<2>(sc1) > std::get<2>(sc2));
    });
    return results;
}

void Game::shutdown_server() {
    // TODO: send stop message to peers
    this->server->Shutdown();
    this->server_thread->join();
}

void Game::cleanup() {
    werase(this->window);
    wrefresh(this->window);
}
