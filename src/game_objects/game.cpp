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
}

void Game::game_loop(bool& running) {
    while (running) {
        auto start = std::chrono::steady_clock::now();

        this->tick_all();
        this->draw_all();

        auto end = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)-(end-start));  // TODO: catch if loop took too long
    }
}

void Game::tick_all() {
    std::vector<std::tuple<int, std::future<shared::UpdateFromClient>>> updates;
    updates.resize(robots.size());
    for (unsigned int i = 0; i < this->robots.size(); i++) {
        if (robots.at(i).energy == 0) {
            continue;
        }

        robots.at(i).tick();

        // Wall collision
        std::vector<int> hit_wall;
        if (robots.at(i).pos_height >= LINES-(robots.at(i).drawable_robot.height+1)) {
            hit_wall.push_back(4);
        } else if (robots.at(i).pos_height <= 2) {
            hit_wall.push_back(2);
        }

        if (robots.at(i).pos_width >= COLS-(robots.at(i).drawable_robot.width+1)) {
            hit_wall.push_back(3);
        } else if (robots.at(i).pos_width <= 2) {
            hit_wall.push_back(1);
        }

        if (hit_wall.empty()) {
            robots.at(i).set_speed(0, 0);
        }

        // TODO: also check for robot collision

        // Bullet collision
        std::vector<GameObjects::Bullet> survived_bullets;
        for (const auto &bullet: this->bullets) {
            if (robots.at(i).check_collision(bullet)) {
                robots.at(i).energy -= 20;
                if (robots.at(i).energy > 0) {
                    robots.at(i).energy = 0;
                }
            } else if (! (bullet.pos_width >= COLS || bullet.pos_height >= LINES ||
                          bullet.pos_height <= 0 || bullet.pos_width <= 0)) {
                survived_bullets.push_back(bullet);
            }
        }

        this->bullets = survived_bullets;

        shared::UpdateFromServer update;

        update.set_energy(static_cast<google::protobuf::int32>(robots.at(i).energy));
        update.mutable_pos()->set_y(this->robots.at(0).pos_height);
        update.mutable_pos()->set_x(this->robots.at(0).pos_width);

        // TODO:
        //update.add_hitrobot(0);

        for (auto hit: hit_wall) {
            update.add_hitwall(hit);
        }

        update.mutable_scanned_robot()->mutable_pos()->set_y(0);
        update.mutable_scanned_robot()->mutable_pos()->set_x(0);
        update.mutable_scanned_robot()->set_id(0);
        update.mutable_scanned_robot()->set_energy_left(0);

        updates.emplace_back(i, std::async(std::launch::async, ([&]{
            return this->service.connections.at(i)->send_message(update);
        })));
    }

    for (auto &update : updates) {
        shared::UpdateFromClient update_client = std::get<1>(update).get();
    }

    // TODO: get the future object and update the robot object
}

void Game::draw_all() {
    for (auto robot: this->robots) {
        robot.draw();
    }

    for (auto bullet: this->bullets) {
        bullet.draw();
    }
}

void Game::start() {
    for (unsigned int i = 0; i < this->service.connections.size(); i++) {
        // TODO: create random position of robot
        drawable::Robot robot_draw(this->window, 5, 5); // TODO: create drawable robot in Robot() constructor
        GameObjects::Robot robot(this->window, robot_draw);
        this->robots.push_back(robot);
    }
}

void Game::shutdown_server() {
    // TODO: send stop message to peers
    this->server->Shutdown();
}
