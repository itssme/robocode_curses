/* 
 * author: Joel Klimont
 * filename: streaming_server.cpp
 * date: 07/03/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include "streaming/streaming_server.h"

StreamingServer::StreamingServer(int port) {
    this-> asio_thread = std::thread(&StreamingServer::start_streaming_server, this, port);
}

StreamingServer::~StreamingServer() {
    this->acceptor->cancel();
    this->acceptor->close();
    this->asio_thread.join();
}

void StreamingServer::close_connections(shared::GameScores scores) {
    this->stop = true;

    for (auto &socket: this->sockets) {
        asio_utils::send_proto(socket, scores);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void StreamingServer::start_streaming_server(short unsigned int port) {
    asio::io_context ioContext;
    tcp::endpoint endpoint{tcp::v4(), port};
    this->acceptor = new tcp::acceptor{ioContext, endpoint};
    this->acceptor->listen();

    while (! this->stop) {
        error_code l_error_code;

        unsigned long int socket_index = this->sockets.size();
        this->sockets.emplace_back(this->acceptor->accept(l_error_code));

        if (l_error_code) {
            spdlog::error("Socket Error -> {}", l_error_code.message());
            this->sockets.at(socket_index).close();
            continue;
        }

        asio_utils::MessageType messageType;
        asio_utils::get_proto_type(this->sockets.at(socket_index), messageType);

        if (messageType == asio_utils::MessageType::StartStreaming) {
            shared::StartStreaming start;
            asio_utils::get_proto_msg(this->sockets.at(socket_index), start);

            asio_utils::send_proto(this->sockets.at(socket_index), start);
        } else {
            spdlog::info("Received an unexpected message during accept");
        }
    }
}

void StreamingServer::send_to_all(shared::StreamingUpdate update) {
    for (auto &socket: this->sockets) {
        asio_utils::send_proto(socket, update);
    }
}
