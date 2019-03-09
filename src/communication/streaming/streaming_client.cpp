/* 
 * author: Joel Klimont
 * filename: streaming_client.cpp
 * date: 07/03/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include "streaming/streaming_client.h"


StreamingClient::StreamingClient(WINDOW* game_window, std::string server_address, int port) {
    this-> asio_thread = std::thread(&StreamingClient::start_streaming_client, this, game_window, server_address, port);
}

void StreamingClient::start_streaming_client(WINDOW* game_window, std::string server_address, short unsigned int port) {
    asio::io_context io_context;

    tcp::resolver resolver{io_context};
    auto results = resolver.resolve(server_address, std::to_string(port));
    tcp::socket socket{io_context};

    spdlog::info("before stream connect: {} port: {}", server_address, port);
    asio::connect(socket, results);

    shared::StartStreaming start_strm;
    start_strm.set_start(true);
    asio_utils::send_proto(socket, start_strm);

    {
        asio_utils::MessageType messageType;
        asio_utils::get_proto_type(socket, messageType);

        if (messageType == asio_utils::MessageType::StartStreaming) {
            shared::StartStreaming start;
            asio_utils::get_proto_msg(socket, start);
            spdlog::info("got start message for streaming");
        } else {
            spdlog::info("Did not receive correct start streaming message");
        }
    }

    while (! this->stop) {
        asio_utils::MessageType messageType;
        asio_utils::get_proto_type(socket, messageType);
        spdlog::info("got a new message");

        if (messageType == asio_utils::MessageType::StreamingUpdate) {
            shared::StreamingUpdate update;
            asio_utils::get_proto_msg(socket, update);
            spdlog::info("Message is update: {}", update.DebugString());

            werase(game_window);

            for (const shared::StreamingRobot &robot: update.robots()) {
                drawable::Robot draw_robot(game_window,
                                           static_cast<int>(robot.pos().y()),
                                           static_cast<int>(robot.pos().x()));
                draw_robot.set_gun_rotation(static_cast<float>(robot.gun_pos().degrees()));
                draw_robot.draw();
            }

            for (const shared::StreamingBullet &bullet: update.bullets()) {
                drawable::Bullet draw_bullet(game_window,
                                             static_cast<int>(bullet.pos().y()),
                                             static_cast<int>(bullet.pos().x()), 2, 2);
                draw_bullet.draw();
            }

            box(game_window, 0, 0);
            wrefresh(game_window);
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

StreamingClient::~StreamingClient() {
    this->asio_thread.join();
}