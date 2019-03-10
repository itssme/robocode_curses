/* 
 * author: Joel Klimont
 * filename: streaming_client.cpp
 * date: 07/03/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include "streaming/streaming_client.h"

/*!
 * Start the streaming client and connect to server
 * @param game_window window where the game will be rendered
 * @param server_address of the streaming server
 * @param port used by the streaming server
 */
StreamingClient::StreamingClient(WINDOW* game_window, std::string server_address, int port) {
    this-> asio_thread = std::thread(&StreamingClient::start_streaming_client, this, game_window, server_address, port);
}

/*!
 * Loop that continuously gets the position
 * of all game objects.
 * @param game_window window where the game will be rendered
 * @param server_address of the streaming server
 * @param port used by the streaming server
 */
void StreamingClient::start_streaming_client(WINDOW* game_window, std::string server_address, short unsigned int port) {
    asio::io_context io_context;

    tcp::resolver resolver{io_context};
    auto results = resolver.resolve(server_address, std::to_string(port));
    tcp::socket socket{io_context};

    shared::WindowSize w_size;

    try {
        spdlog::info("before stream connect: {} port: {}", server_address, port);
        asio::connect(socket, results);

        shared::StartStreaming start_strm;
        start_strm.set_start(true);
        asio_utils::send_proto(socket, start_strm);


        asio_utils::MessageType messageType;
        asio_utils::get_proto_type(socket, messageType);

        if (messageType == asio_utils::MessageType::WindowSize) {
            asio_utils::get_proto_msg(socket, w_size);
            spdlog::info("got start message for streaming");
        } else {
            spdlog::info("Did not receive correct start streaming message");
        }
    } catch (const std::exception &exc) {
        spdlog::critical("critical error while receiving start message {}", exc.what());
        this->stop = true;
    }

    werase(game_window);
    wrefresh(game_window);

    while (LINES < w_size.height() || COLS < w_size.width()) {
        werase(game_window);
        std::string height = "This height is '" + std::to_string(LINES) + "' but should be '" + std::to_string(w_size.height()) + "'";
        std::string width = "This width is '" + std::to_string(COLS) + "' but should be '" + std::to_string(w_size.width()) + "'";

        mvwaddstr(game_window, 1, 2, "Resize your window to display the game");
        mvwaddstr(game_window, 2, 2, height.c_str());
        mvwaddstr(game_window, 3, 2, width.c_str());

        wrefresh(game_window);
    }

    WINDOW* game_win_sized = derwin(game_window, w_size.height(), w_size.width(), 0, 0);
    box(game_win_sized, 0, 0);
    wrefresh(game_win_sized);

    while (! this->stop) {
        asio_utils::MessageType messageType;

        try {
            asio_utils::get_proto_type(socket, messageType);
        } catch (const std::exception &exc) {
            spdlog::critical("critical error when receiving stream message {}", exc.what());
            this->stop = true;
            continue;
        }

        spdlog::info("got a new message");

        if (messageType == asio_utils::MessageType::StreamingUpdate) {
            shared::StreamingUpdate update;
            asio_utils::get_proto_msg(socket, update);
            spdlog::info("Message is update: {}", update.DebugString());

            werase(game_win_sized);

            for (const shared::StreamingRobot &robot: update.robots()) {
                drawable::Robot draw_robot(game_win_sized,
                                           static_cast<int>(robot.pos().y()),
                                           static_cast<int>(robot.pos().x()));
                draw_robot.set_gun_rotation(static_cast<float>(robot.gun_pos().degrees()));
                draw_robot.draw();
            }

            for (const shared::StreamingBullet &bullet: update.bullets()) {
                drawable::Bullet draw_bullet(game_win_sized,
                                             static_cast<int>(bullet.pos().y()),
                                             static_cast<int>(bullet.pos().x()), 2, 2);
                draw_bullet.draw();
            }

            box(game_win_sized, 0, 0);
            wrefresh(game_win_sized);
        } else if (messageType == asio_utils::MessageType::GameScores) {
            shared::GameScores l_scores;
            asio_utils::get_proto_msg(socket, l_scores);
            spdlog::info("Message is score: {}", l_scores.DebugString());
            this->scores = l_scores;
            this->stop = true;
        } else if (messageType == asio_utils::MessageType::End) {
            shared::Empty end;
            asio_utils::get_proto_msg(socket, end);
            spdlog::info("Message is end: {}", end.DebugString());
            this->stop = true;
        } else {
            spdlog::info("Received an unexpected message");
        }
    }

    socket.close();
}

/*!
 * Stop the streaming client
 */
StreamingClient::~StreamingClient() {
    this->asio_thread.join();
}