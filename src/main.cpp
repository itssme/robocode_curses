#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <ncurses.h>
#undef OK
#include <messages.pb.h>
#include <thread>
#include <chrono>
#include <random>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#pragma GCC diagnostic pop

#include "curses_utils.h"
#include "curses_drawable_objects.h"
#include "game_objects.h"
#include "server.h"
#include "client.h"
#include "game.h"
#define OK      (0)

using namespace std;

WINDOW* main_window;
bool running{false};
mutex draw_mutex;

void server() {
    spdlog::info("in server");
    std::string server_address("0.0.0.0:5000"); // TODO: read port from config json file

    Game game(main_window, server_address);

    std::cout << "after game init" << std::endl;

    while (game.service.connections.empty()) {
        this_thread::sleep_for(chrono::seconds(1));
    }
    std::cout << game.service.connections.at(0)->id << std::endl;
    game.service.connections.at(0)->sanity_check();
    while (1) {
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void client() {
    cout << "CLIENT" << endl;

    std::string server_address("0.0.0.0:0"); // ':0' will choose an random available port
    ClientImpl service;

    ServerBuilder builder;
    int selected_port = 0;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials(), &selected_port);
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());

    thread waiting([&]{
        std::cout << "Server listening on " << server_address << " -> " << selected_port << std::endl;
        server->Wait();
    });

    Advertise ad(grpc::CreateChannel("localhost:5000", grpc::InsecureChannelCredentials())); // TODO: read port from config json
    int id = ad.Register("Bla", selected_port);
    cout << "ID: " << id << endl;

    while (1) {
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void background_robot() {
    this_thread::sleep_for(std::chrono::seconds(1));
    drawable::Robot draw_rob(main_window, 6, 6);
    GameObjects::Robot robot(main_window, draw_rob);

    random_device rd;
    mt19937 gen{rd()};
    uniform_real_distribution<double> dis_x{2, (double) COLS-(draw_rob.height+3)};
    uniform_real_distribution<double> dis_y{2, (double) LINES-(draw_rob.width+3)};
    double rotation = 0;

    // calcuate size of menu window
    int height_obstacle = LINES / 3;
    int width_obstacle = COLS / 3;

    int pos_height_obstacle = (LINES - height_obstacle) / 2;
    int pos_width_obstacle = (COLS - width_obstacle) / 2;

    robot.set_speed(1, 1);
    robot.set_pos(dis_y(gen), dis_x(gen));

    while (running) {
        robot.tick();

        draw_mutex.lock();
        robot.draw();
        draw_mutex.unlock();

        robot.set_gun_rotation(rotation);
        rotation += 10;

        // check for collision with outside border
        if (robot.pos_height >= LINES-(robot.drawable_robot.height+1)) {
            robot.speed_height = -1;
        } else if (robot.pos_height <= 2) {
            robot.speed_height = 1;
        }

        if (robot.pos_width >= COLS-(robot.drawable_robot.width+1)) {
            robot.speed_width = -1;
        } else if (robot.pos_width <= 2) {
            robot.speed_width = 1;
        }

        // if the terminal has an odd number as height this wont work perfectly
        // check for collision with main menu and set new speed
        if (robot.pos_width - 1 < pos_width_obstacle + width_obstacle && robot.pos_width + robot.drawable_robot.width + 1 > pos_width_obstacle &&
                robot.pos_height - 1 < pos_height_obstacle + height_obstacle && robot.pos_height + robot.drawable_robot.height + 1 > pos_height_obstacle ) {
            if (pos_height_obstacle == static_cast<int>(robot.pos_height) + robot.drawable_robot.height + 1) {
                robot.speed_height *= -1;
            }
            if (static_cast<int>(robot.pos_height) == pos_height_obstacle + height_obstacle - 1) {
                robot.speed_height *= -1;
            }
            if (pos_width_obstacle == static_cast<int>(robot.pos_width) + robot.drawable_robot.width) {
                robot.speed_width *= -1;
            }
            if (static_cast<int>(robot.pos_width) == pos_width_obstacle + width_obstacle) {
                robot.speed_width = 1;
            }
        }

        this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // create logger
    try {
        auto logger = spdlog::basic_logger_mt("file_logger", "log.log");
        spdlog::set_default_logger(logger);
        spdlog::set_level(spdlog::level::info); // TODO: read loglevel from config file
    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }

    spdlog::info("started");

    shared::Position msg;
    msg.set_x(10);
    msg.set_y(10);

    cout << msg.x() << endl;
    cout << msg.y() << endl;

    /*
    int i;
    cin >> i;
    if (i == 0) {
        server();
    } else {
        client();
    }
     */

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    refresh();

    // window where the game will be played
    main_window = create_newwin(LINES, COLS, 0, 0);
    running = true;
    thread t = thread(background_robot);

    bool is_server = false;
    bool configured = false;

    while (! configured) {
        int main_choice;

        { // MAIN MENU
            vector<string> main_menu_config;
            main_menu_config.emplace_back("Connect to a game");
            main_menu_config.emplace_back("Host a new game");
            main_menu_config.emplace_back("Exit game");
            draw_mutex.lock();
            Menu main_menu = Menu(main_window, main_menu_config, 0, " Robocode Menu ");
            main_menu.refresh_all();
            draw_mutex.unlock();
            main_menu.loop(&draw_mutex);
            main_choice = main_menu.evaluate_choice();

            draw_mutex.lock();
            main_menu.erase();
            main_menu.refresh_all();
            draw_mutex.unlock();
        }

        {
            if (main_choice == 0) {
                vector<string> tmp;
                tmp.emplace_back("Back ..");
                tmp.emplace_back("Connect to");
                draw_mutex.lock();
                Menu connect_menu = Menu(main_window, tmp, 1, " Connect Menu ");
                connect_menu.refresh_all();
                draw_mutex.unlock();
                connect_menu.loop(&draw_mutex);
                if (connect_menu.evaluate_choice() != 0) {
                    configured = true;
                    is_server = false;
                }

                draw_mutex.lock();
                connect_menu.erase();
                connect_menu.refresh_all();
                draw_mutex.unlock();
            } else if (main_choice == 1) {
                vector<string> tmp;
                tmp.emplace_back("Back ..");
                tmp.emplace_back("Start getting connections");
                draw_mutex.lock();
                Menu connect_menu = Menu(main_window, tmp, 0, " Host Menu ");
                connect_menu.refresh_all();
                draw_mutex.unlock();
                connect_menu.loop(&draw_mutex);
                if (connect_menu.evaluate_choice() != 0) {
                    configured = true;
                    is_server = true;
                }

                draw_mutex.lock();
                connect_menu.erase();
                connect_menu.refresh_all();
                draw_mutex.unlock();

            } else if (main_choice == 2) {
                configured = true;
            }
        }
    }

    spdlog::info("stopped");

    running = false;
    t.join();
    endwin();

    if (is_server) {
        server();
    } else {
        client();
    }

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}