/* 
 * author: Joel Klimont
 * filename: streaming_server.cpp
 * date: 07/03/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include "streaming/streaming_server.h"

/*!
 * Start listening for stream connections
 * @param port the server will use
 * @param height of the game window
 * @param width of the gme window
 */
StreamingServer::StreamingServer(int port, int height, int width) {
    this-> asio_thread = std::thread(&StreamingServer::start_streaming_server, this, port, height, width);
}

/*!
 * Stop listening and join server thread
 */
StreamingServer::~StreamingServer() {
    this->acceptor->cancel();
    this->acceptor->close();
    this->asio_thread.join();
}

/*!
 * Send scores to all connections
 * and close them afterwards
 * @param scores of the game
 */
void StreamingServer::close_connections(shared::GameScores scores) {
    this->stop = true;

    for (auto &socket: this->sockets) {
        try {
            asio_utils::send_proto(socket, scores);
        } catch (const std::exception &exc) {
            spdlog::critical("critical error while sending scores message {}", exc.what());
        }
    }
}

/*!
 * Continuously send new positions
 * to the clients.
 * @param port the server will use
 * @param height of the game window
 * @param width of the gme window
 */
void StreamingServer::start_streaming_server(short unsigned int port, int height, int width) {
    asio::io_context ioContext;
    tcp::endpoint endpoint{tcp::v4(), port};
    this->acceptor = new tcp::acceptor{ioContext, endpoint};
    this->acceptor->listen();

    while (! this->stop) {
        error_code l_error_code;

        unsigned long int socket_index = this->sockets.size();
        this->sockets.emplace_back(this->acceptor->accept(l_error_code));

        if (l_error_code) {
            spdlog::error("Socket error -> {}", l_error_code.message());
            this->sockets.at(socket_index).close();
            continue;
        }

        asio_utils::MessageType messageType;
        asio_utils::get_proto_type(this->sockets.at(socket_index), messageType);

        if (messageType == asio_utils::MessageType::StartStreaming) {
            shared::StartStreaming start;
            asio_utils::get_proto_msg(this->sockets.at(socket_index), start);

            shared::WindowSize w_size;
            w_size.set_height(height);
            w_size.set_width(width);
            asio_utils::send_proto(this->sockets.at(socket_index), w_size);
        } else {
            spdlog::info("Received an unexpected message during accept");
        }
    }
}

/*!
 * Send a update message to all clients.
 * @param update message that will be sent
 */
void StreamingServer::send_to_all(shared::StreamingUpdate update) {
    for (auto &socket: this->sockets) {
        try {
            asio_utils::send_proto(socket, update);
        } catch (const std::exception &exc) {
            spdlog::error("error while trying to send update message");
        }
    }
}
