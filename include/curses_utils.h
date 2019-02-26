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

class DisplayText {
private:
    WINDOW* parent_window;
    std::string title;
    WINDOW* window;
    std::vector<std::string> text;
    int height;
    int width;
    int pos_y;
    int pos_x;
public:
    DisplayText(WINDOW* parent_window, const std::vector<std::string> &text, const int &input_options, const std::string &title) {
        if (parent_window == nullptr) {
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

        // TODO: add options to vector and display them


    }

};

struct Option {
public:
    std::string title;
    WINDOW* window;
    Option(WINDOW* parent_window, const std::string &title, int length, int begin_y, int begin_x) {
        this->window = derwin(parent_window, 1, length-1, begin_y, begin_x);
        this->title = title;
        waddstr(this->window, this->title.c_str());
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
    void erase() {
        werase(this->window);
    }
};

struct OptionTextInput {
    std::string title;
    std::string text;
    int length;
    int input_size;
    WINDOW* window;
    WINDOW* text_window;
    OptionTextInput(WINDOW* parent_window, const std::string &title, int begin_y, int begin_x, int input_size) {
        this->length = static_cast<int>(title.size() + input_size + 2);
        this->window = derwin(parent_window, 1, this->length, begin_y, parent_window->_maxx/2 - this->length/2);
        this->title = title;
        waddstr(this->window, (this->title + " ").c_str());
        this->text_window = derwin(this->window, 1, input_size, 0, title.size()+1);
        wattron(this->text_window, A_UNDERLINE);
        for (int i = 0; i < input_size; ++i) {
            waddch(this->text_window, ' ');
        }
        this->input_size = input_size;
    }
    void pass_input(int ch) {
        if (ch != 91 && ch != 27) {
            if (ch == 127) {
                if (this->text.length()) {
                    this->text.pop_back();
                }
            } else {
                this->text += static_cast<char>(ch);
            }
            this->redraw_subwin();
        }
    }
    std::string evaluate() {
        return this->text;
    }
    void redraw_subwin() {
        werase(this->text_window);

        for (auto ch: this->text) {
            waddch(this->text_window, ch);
        }
        for (int i = 0; i < this->length - this->text.length(); ++i) {
            waddch(this->text_window, ' ');
        }
    }
    void select() {
        werase(this->window);
        wattroff(this->window, A_NORMAL);
        wattron(this->window, A_REVERSE);
        waddstr(this->window, this->title.c_str());
        wattroff(this->window, A_REVERSE);
        waddstr(this->window, " ");
        redraw_subwin();
    }
    void de_select() {
        werase(this->window);
        wattron(this->window, A_NORMAL);
        waddstr(this->window, this->title.c_str());
        waddstr(this->window, " ");
        redraw_subwin();
    }
    void refresh() {
        touchwin(this->text_window);
        wrefresh(this->text_window);
        touchwin(this->window);
        wrefresh(this->window);
    }
    void erase() {
        werase(this->text_window);
        werase(this->window);
    }
};

class Menu {
private:
    WINDOW* parent_window;
    std::string title;
    int at_option;
    WINDOW* window;
    std::vector<Option> options{};
    std::vector<OptionTextInput> options_text_input{};
    int height;
    int width;
    int pos_y;
    int pos_x;
public:
    Menu(WINDOW* parent_window, const std::vector<std::string> &option_names, const int &input_options, const std::string &title) {
        this->parent_window = parent_window;

        if (parent_window == nullptr) {
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
            if (i+1 <= option_names.size() - input_options) {
                this->options.emplace_back(Option(this->window, option_names.at(i),
                                                  static_cast<int>(option_names.at(i).size()) + 1,
                                                  static_cast<int>(window->_maxy / 2 - option_names.size() / 2 + i),
                                                  static_cast<int>(window->_maxx / 2 -
                                                                   option_names.at(i).length() / 2)));
            } else {
                this->options_text_input.emplace_back(OptionTextInput(this->window, option_names.at(i),
                                                      static_cast<int>(window->_maxy / 2 - option_names.size() / 2 + i),
                                                      static_cast<int>(window->_maxx / 2 -
                                                                       option_names.at(i).length() / 2),
                                                       12));
            }
        }
        this->options.at(0).select();
        this->at_option = 0;
    }
    void loop(std::mutex* draw_mutex) {
        int ch;
        while ((ch = getch()) != 10) {
            if (ch == 66) {
                this->down();
            } else if (ch == 65) {
                this->up();
            } else {
                this->pass_input(ch);
            }

            draw_mutex->lock();
            this->refresh_all();
            draw_mutex->unlock();
        }
    }
    std::tuple<int, int, int, int> get_size() {
        return std::make_tuple(height, width, pos_y, pos_x);
    };
    void up() {
        if (at_option < options.size()) {
            this->options.at(at_option).de_select();
        } else {
            this->options_text_input.at(at_option-options.size()).de_select();
        }
        if (at_option == 0) {
            at_option = static_cast<int>((options.size()+options_text_input.size()) - 1);
        } else {
            at_option--;
        }
        if (at_option < options.size()) {
            this->options.at(at_option).select();
        } else {
            this->options_text_input.at(at_option-options.size()).select();
        }
    }
    void down() {
        if (at_option < options.size()) {
            this->options.at(at_option).de_select();
        } else {
            this->options_text_input.at(at_option-options.size()).de_select();
        }
        if (at_option == (options.size() + options_text_input.size()) - 1) {
            at_option = 0;
        } else {
            at_option++;
        }
        if (at_option < options.size()) {
            this->options.at(at_option).select();
        } else {
            this->options_text_input.at(at_option-options.size()).select();
        }
    }
    void pass_input(int ch) {
        if (at_option > this->options.size() - 1) {
            options_text_input.at(at_option-options.size()).pass_input(ch);
        }
    }
    std::string evaluate() {
        if (at_option < this->options.size()) {
            return this->options.at(at_option).title;
        } else {
            return this->options_text_input.at(at_option-options.size()).evaluate();
        }
    }
    int evaluate_choice() {
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
        for (auto op: options_text_input) {
            op.refresh();
        }
    }
    void erase() {
        wrefresh(this->window);
        for (auto op: options) {
            op.erase();
        }
        for (auto op: options_text_input) {
            op.erase();
        }
        werase(this->window);
    }
};

#endif //ROBOCODE_CURSES_UTILS_H