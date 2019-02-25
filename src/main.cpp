#include <iostream>
#include <fstream>
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
bool running{true};
mutex draw_mutex;

void server() {
    cout << "SERVER" << endl;
    std::string server_address("0.0.0.0:5000");
    ServerImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());  // this will not throw an error if the port is already in use
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
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


    Advertise ad(grpc::CreateChannel("localhost:5000", grpc::InsecureChannelCredentials()));
    int id = ad.Register("Bla", selected_port);
    cout << "ID: " << id << endl;

    std::cout << "Server listening on " << server_address << ":" << selected_port << std::endl;
    server->Wait();
}

void background_robot() {
    this_thread::sleep_for(std::chrono::seconds(1));
    float speed_x = 1;
    float speed_y = 1;
    drawable::Robot robot = drawable::Robot(main_window, 6, 6);
    random_device rd;
    mt19937 gen{rd()};
    uniform_real_distribution<float> dis_x{2, (float) COLS-(robot.height+2)};
    uniform_real_distribution<float> dis_y{2, (float) LINES-(robot.width+2)};
    float pos_x = dis_x(gen);
    float pos_y = dis_y(gen);
    float rotation = 0;

    while (running) {
        pos_y += speed_y;
        pos_x += speed_x;
        robot.move(pos_y, pos_x);

        draw_mutex.lock();
        robot.draw();
        draw_mutex.unlock();

        robot.set_gun_rotation(rotation);
        rotation+=10;

        if (pos_y >= LINES-(robot.height+1)) {
            speed_y = -1;
        } else if (pos_y <= 2) {
            speed_y = 1;
        }

        if (pos_x >= COLS-(robot.width+1)) {
            speed_x = -1;
        } else if (pos_x <= 2) {
            speed_x = 1;
        }

        this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    shared::Position msg;
    msg.set_x(10);
    msg.set_y(10);

    cout << msg.x() << endl;
    cout << msg.y() << endl;

    int i;
    cin >> i;
    if (i == 0) {
        server();
    } else {
        client();
    }

    google::protobuf::ShutdownProtobufLibrary();

    initscr();
    cbreak();
    noecho();
    curs_set(0);
    refresh();

    // window where the game will be played
    main_window = create_newwin(LINES, COLS, 0, 0);
    thread t = thread(background_robot);

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
            int ch;
            while ((ch = getch()) != 10) {
                if (ch == 66) {
                    main_menu.down();
                } else if (ch == 65) {
                    main_menu.up();
                }

                draw_mutex.lock();
                main_menu.refresh_all();
                draw_mutex.unlock();
            }
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
                int ch;
                while ((ch = getch()) != 10) {
                    if (ch == 66) {
                        connect_menu.down();
                    } else if (ch == 65) {
                        connect_menu.up();
                    } else {
                        connect_menu.pass_input(ch);
                    }

                    draw_mutex.lock();
                    connect_menu.refresh_all();
                    draw_mutex.unlock();
                }
                if (connect_menu.evaluate_choice() != 0) {
                    configured = true;
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
                int ch;
                while ((ch = getch()) != 10) {
                    if (ch == 66) {
                        connect_menu.down();
                    } else if (ch == 65) {
                        connect_menu.up();
                    } else {
                        connect_menu.pass_input(ch);
                    }

                    draw_mutex.lock();
                    connect_menu.refresh_all();
                    draw_mutex.unlock();
                }
                if (connect_menu.evaluate_choice() != 0) {
                    configured = true;
                }

                draw_mutex.lock();
                connect_menu.erase();
                connect_menu.refresh_all();
                draw_mutex.unlock();

                if (connect_menu.evaluate_choice() == 1) {
                    // TODO: start getting connections and output information about connected users
                }

            } else if (main_choice == 2) {
                configured = true;
            }
        }
    }

    running = false;
    t.join();
    endwin();

    return 0;
}