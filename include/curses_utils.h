/* 
 * author: Joel Klimont
 * filename: curses_utils.h
 * date: 28/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#ifndef ROBOCODE_CURSES_UTILS_H
#define ROBOCODE_CURSES_UTILS_H

#include <ncurses.h>
#include <string>
#include <vector>

WINDOW *create_newwin(int height, int width, int starty, int startx) {
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);
    wrefresh(local_win);

    return local_win;
};

struct Option {
public:
    int length;
    WINDOW* window;
    std::string title;
    Option(WINDOW* parent_window, const std::string &title, int length, int begin_x, int begin_y) {
        this->length = length-1;
        this->title = title;
        this->window = derwin(parent_window, 1, length-1, begin_x, begin_y);
        waddstr(this->window, title.c_str());
    }
    void select() {
        werase(this->window);
        wattroff(this->window, A_NORMAL);
        wattron(this->window, A_REVERSE);
        waddstr(this->window, this->title.c_str());
    }
    void de_select() {
        werase(this->window);
        wattroff(this->window, A_REVERSE);
        wattron(this->window, A_NORMAL);
        waddstr(this->window, this->title.c_str());
    }
    void refresh() {
        touchwin(this->window);
        wrefresh(this->window);
    }
};

class Menu {
private:
    WINDOW* parent_window;
    std::string title;
    int at_option;
    WINDOW* window;
    std::vector<Option> options{};
public:
    Menu(WINDOW* parent_window, const std::vector<std::string> &option_names, const std::string &title) {
        this->parent_window = parent_window;

        if (parent_window == nullptr) {
            int pos_x, pos_y, width, height;

            height = LINES / 3;
            width = COLS / 3;

            pos_y = (LINES - height) / 2;
            pos_x = (COLS - width) / 2;
            this->window = create_newwin(height, width, pos_y, pos_x);
            wrefresh(this->window);
            refresh();
        } else {
            // if the parent window is too small this will cause a segfault
            this->window = derwin(parent_window,
                                  parent_window->_maxy / 3,
                                  parent_window->_maxx / 3,
                                  (parent_window->_maxy - parent_window->_maxy / 3) / 2,
                                  (parent_window->_maxx - parent_window->_maxx / 3) / 2);
            box(this->window, 0 , 0);
            touchwin(this->window);
            wrefresh(this->window);
        }
        mvwaddstr(this->window, 0, 1, title.c_str());
        this->title = title;

        for (int i = 0; i < option_names.size(); ++i) {
            this->options.emplace_back(Option(this->window, option_names.at(i),
                                              static_cast<int>(option_names.at(i).size())+1,
                                              static_cast<int>(window->_maxy / 2 - option_names.size() / 2 + i),
                                              static_cast<int>(window->_maxx / 2 - option_names.at(i).length() / 2)));
        }

        this->options.at(0).select();
        this->at_option = 0;
    }

    void up() {
        this->options.at(at_option).de_select();
        if (at_option == 0) {
            at_option = static_cast<int>(options.size() - 1);
        } else {
            at_option--;
        }
        this->options.at(at_option).select();
    }

    void down() {
        this->options.at(at_option).de_select();
        if (at_option == options.size() - 1) {
            at_option = 0;
        } else {
            at_option++;
        }
        this->options.at(at_option).select();
    }

    int evaluate() {
        return at_option;
    }

    void refresh_all() {
        if (is_subwin(this->window)) {
            touchwin(this->window);
        }
        wrefresh(this->window);
        for (auto op: options) {
            op.refresh();
        }
    }
};

#endif //ROBOCODE_CURSES_UTILS_H
