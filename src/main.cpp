#include <iostream>
#include <fstream>
#include <string>
#include <ncurses.h>
#include <curses_utils.h>
#include <messages.pb.h>

using namespace std;

WINDOW* main_window;

void server() {
    cout << "SERVER" << endl;
}

void client() {
    cout << "CLIENT" << endl;
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

    endwin();

    return 0;
}