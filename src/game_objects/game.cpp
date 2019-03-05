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
        std::this_thread::sleep_for(std::chrono::milliseconds(100)-(end-start));
    }
}

void Game::tick_all() {
    std::vector<std::future<shared::UpdateFromClient>> updates;
    std::vector<shared::UpdateFromClient> messages;

    //messages.resize(robots.size());
    updates.resize(robots.size());
    for (unsigned int i = 0; i < this->robots.size(); i++) {
        if (robots.at(i).energy <= 0) {
            continue;
        }

        robots.at(i).tick();

        // Wall collision
        std::vector<int> hit_wall;
        if (robots.at(i).pos_height >= LINES -(robots.at(i).drawable_robot.height + 1)) {
            hit_wall.push_back(4);
        } else if (robots.at(i).pos_height <= 1) {
            hit_wall.push_back(2);
        }

        if (robots.at(i).pos_width >= COLS -(robots.at(i).drawable_robot.width + 1)) {
            hit_wall.push_back(3);
        } else if (robots.at(i).pos_width <= 1) {
            hit_wall.push_back(1);
        }

        // TODO: also check for robot collision

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
                bullet.tick();
                survived_bullets.push_back(bullet);
            }
        }

        this->bullets = survived_bullets;

        shared::UpdateFromServer update;

        update.set_energy(static_cast<google::protobuf::int32>(robots.at(i).energy));
        update.mutable_pos()->set_y(this->robots.at(i).pos_height);
        update.mutable_pos()->set_x(this->robots.at(i).pos_width);

        // TODO:
        //update.add_hitrobot(0);

        for (auto hit: hit_wall) {
            update.add_hitwall(hit);
        }

        //update.mutable_scanned_robot()->mutable_pos()->set_y(0);
        //update.mutable_scanned_robot()->mutable_pos()->set_x(0);
        //update.mutable_scanned_robot()->set_id(0);
        //update.mutable_scanned_robot()->set_energy_left(0);

        //updates.push_back(std::async(std::launch::async, ([&]{
        //    return this->service.connections.at(i)->send_message(update);
        //})));
        messages.push_back(this->service.connections.at(i)->send_message(update));
    }

    for (unsigned long int i = 0; i < messages.size(); ++i) {
        //std::cout << messages.at(i).DebugString() << std::endl;

        shared::UpdateFromClient update_client = messages.at(i);  // TODO: check if robot is still alive

        // player has not sent an update -> using speed values of last update
        // TODO: if player does not respond after 5 messages, declare connection lost
        if (update_client.pos().y() == -1) {
            this->robots.at(i).tick();
            continue;
        }

        // create bullet object if player shot
        if (update_client.shot()) {
            this->bullets.push_back(this->robots.at(i).shoot(this->window));
        }

        // update position of player
        // TODO: check if last speed matches new position to avoid cheating
        this->robots.at(i).set_pos(update_client.pos().y(), update_client.pos().x());

        // update speed of player
        // TODO: check if player is not above max speed (read max speed from config file)
        this->robots.at(i).set_speed(update_client.speed().y(), update_client.speed().x());

        this->robots.at(i).set_gun_rotation(update_client.gun_pos().degrees());
        this->robots.at(i).gun_speed = update_client.gun_pos().speed();
    }

    /*
    for (unsigned long int i = 0; i < updates.size(); ++i) {
        //std::cout << "Update is: " << updates.at(i).valid() << std::endl;
        while (! updates.at(i).valid()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        shared::UpdateFromClient update_client = updates.at(i).get();  // TODO: check if robot is still alive

        GameObjects::Robot* robot = &this->robots.at(i);

        // create bullet object if player shot
        if (update_client.shot()) {
            this->bullets.push_back(this->robots.at(i).shoot());
        }

        // update position of player
        // TODO: check if last speed matches new position to avoid cheating
        this->robots.at(i).set_pos(update_client.pos().y(), update_client.pos().x());

        // update speed of player
        // TODO: check if player is not above max speed (read max speed from config file)
        this->robots.at(i).set_speed(update_client.speed().y(), update_client.speed().x());

        this->robots.at(i).set_gun_rotation(update_client.gun_pos().degrees());
    }
    */
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
        drawable::Robot robot_draw(this->window,
                                   static_cast<int>(dis_y(gen)),
                                   static_cast<int>(dis_x(gen))); // TODO: create drawable robot in Robot() constructor
        GameObjects::Robot robot(this->window, robot_draw, this->service.connections.at(i)->id);
        robot.energy = 100;
        this->robots.push_back(robot);
    }
}

void Game::shutdown_server() {
    // TODO: send stop message to peers
    this->server->Shutdown();
}
