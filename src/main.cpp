#include <iostream>
#include <fstream>
#include <string>
#include <ncurses.h>
#include <messages.pb.h>
#include <thread>
#include <chrono>
#include <random>

#include "curses_utils.h"
#include "curses_drawable_objects.h"

using namespace std;

WINDOW* main_window;
bool running{true};

void server() {
    cout << "SERVER" << endl;
}

void client() {
    cout << "CLIENT" << endl;
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
        robot.draw();
        robot.set_gun_rotation(rotation);
        rotation+=5;

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

        this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    mutex input;

    shared::Position msg;
    msg.set_x(10);
    msg.set_y(10);

    cout << msg.x() << endl;
    cout << msg.y() << endl;

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
        vector<string> main_menu_config;
        main_menu_config.emplace_back("Connect to a game");
        main_menu_config.emplace_back("Host a new game");
        main_menu_config.emplace_back("Exit game");

        { // MAIN MENU
            Menu main_menu = Menu(main_window, main_menu_config, 0, " Robocode Menu ");
            main_menu.refresh_all();
            int ch;
            while ((ch = getch()) != 10) {
                if (ch == 66) {
                    main_menu.down();
                } else if (ch == 65) {
                    main_menu.up();
                }

                main_menu.refresh_all();
            }
            main_choice = main_menu.evaluate_choice();
            main_menu.erase();
            main_menu.refresh_all();
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

                    connect_menu.refresh_all();
                }
                if (connect_menu.evaluate_choice() != 0) {
                    configured = true;
                }
                connect_menu.erase();
                connect_menu.refresh_all();
            } else if (main_choice == 1) {
                vector<string> tmp;
                tmp.emplace_back("Back ..");
                tmp.emplace_back("TEMP");
                Menu connect_menu = Menu(main_window, tmp, 1, " Host Menu ");
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

                    connect_menu.refresh_all();
                }
                if (connect_menu.evaluate_choice() != 0) {
                    configured = true;
                }
                connect_menu.erase();

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