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

    // windows where the game will be played
    main_window = create_newwin(LINES, COLS, 0, 0);

    { // MAIN MENU
        vector<string> tmp;
        tmp.emplace_back("Connect to a game");
        tmp.emplace_back("Host a new game");
        tmp.emplace_back("Exit game");
        Menu main_menu = Menu(main_window, tmp, " Robocode Menu ");
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
        endwin();

        cout << main_menu.evaluate() << endl;
        if (main_menu.evaluate() == 0) {
            client();
        } else if (main_menu.evaluate() == 1) {
            server();
        } else {
            cout << "EXIT" << endl;
        }
    }

    return 0;
}