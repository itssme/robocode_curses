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

#include "curses_utils.h"
#include "curses_drawable_objects.h"
#include "server.h"
#include "client.h"
#define OK      (0)

using namespace std;

WINDOW* main_window;
bool running{false};
mutex draw_mutex;

void server() {
    cout << "SERVER" << endl;
    std::string server_address("0.0.0.0:5000"); // TODO: read port from config json file
    ServerImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());  // this will not throw an error if the port is already in use
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    thread waiting([&]{
        server->Wait();
    });
    std::cout << "after wait" << std::endl;
    while (service.connections.empty()) {
        this_thread::sleep_for(chrono::seconds(1));
    }
    std::cout << "sanity check" << std::endl;
    std::cout << service.connections.at(0)->id << std::endl;
    service.connections.at(0)->sanity_check();
    std::cout << "DONE" << std::endl;
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
    float speed_width = 1;
    float speed_height = 1;
    drawable::Robot robot = drawable::Robot(main_window, 6, 6);
    random_device rd;
    mt19937 gen{rd()};
    uniform_real_distribution<float> dis_x{2, (float) COLS-(robot.height+3)};
    uniform_real_distribution<float> dis_y{2, (float) LINES-(robot.width+3)};
    float pos_width = dis_x(gen);
    float pos_height = dis_y(gen);
    float rotation = 0;

    // calcuate size of menu window
    int height_obstacle = LINES / 3;
    int width_obstacle = COLS / 3;

    int pos_height_obstacle = (LINES - height_obstacle) / 2;
    int pos_width_obstacle = (COLS - width_obstacle) / 2;

    while (running) {
        pos_height += speed_height;
        pos_width += speed_width;
        robot.move(pos_height, pos_width);

        draw_mutex.lock();
        robot.draw();
        draw_mutex.unlock();

        robot.set_gun_rotation(rotation);
        rotation+=10;

        if (pos_height >= LINES-(robot.height+1)) {
            speed_height = -1;
        } else if (pos_height <= 2) {
            speed_height = 1;
        }

        if (pos_width >= COLS-(robot.width+1)) {
            speed_width = -1;
        } else if (pos_width <= 2) {
            speed_width = 1;
        }

        if (pos_width - 1 < pos_width_obstacle + width_obstacle && pos_width + robot.width + 1 > pos_width_obstacle &&
                pos_height - 1 < pos_height_obstacle + height_obstacle && pos_height + robot.height + 1 > pos_height_obstacle ) {
            /*
            if (pos_width + robot.width + 1 > pos_width_obstacle && pos_width - 1 < pos_width_obstacle + width_obstacle) {
                speed_width *= -1;
            } else if (pos_height + robot.height + 1 > pos_height_obstacle && pos_height - 1 < pos_height_obstacle + height_obstacle) {
                //speed_height *= -1;
            }
            */

            if (pos_height_obstacle == static_cast<int>(pos_height) + robot.height + 1) {
                speed_height *= -1;
            }
            if (static_cast<int>(pos_height) == pos_height_obstacle + height_obstacle - 1) {
                speed_height *= -1;
            }
            if (pos_width_obstacle == static_cast<int>(pos_width) + robot.width) {
                speed_width *= -1;
            }
            if (static_cast<int>(pos_width) == pos_width_obstacle + width_obstacle) {
                speed_width *= -1;
            }
        }

        this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

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
            Menu main_menu = Menu(main_window, main_menu_config, 0, " Robocode Menu ");

            main_menu.refresh_all();
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
                Menu connect_menu = Menu(main_window, tmp, 1, " Connect Menu ");
                connect_menu.refresh_all();
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
                Menu connect_menu = Menu(main_window, tmp, 0, " Host Menu ");
                connect_menu.refresh_all();
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