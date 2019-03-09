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
    this->stop = true;
    this->acceptor->cancel();
    this->acceptor->close();

    for (auto &socket: this->sockets) {
        shared::Empty end;
        asio_utils::send_proto(socket, end);
        socket.close();
    }

    std::cout << "before join" << std::endl;
    this->asio_thread.join();
}

void StreamingServer::start_streaming_server(short unsigned int port) {
    asio::io_context ioContext;
    tcp::endpoint endpoint{tcp::v4(), port};
    this->acceptor = new tcp::acceptor{ioContext, endpoint};
    this->acceptor->listen();

    while (! this->stop) {
        error_code l_error_code;

        std::cout << "before accept at port: " << port << std::endl;
        unsigned long int socket_index = this->sockets.size();
        this->sockets.emplace_back(this->acceptor->accept(l_error_code));
        std::cout << "after accept" << std::endl;

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
            std::cout << start.DebugString() << std::endl;

            asio_utils::send_proto(this->sockets.at(socket_index), start);
        } else {
            std::cout << "Received an unexpected message during accept" << std::endl;
            spdlog::info("Received an unexpected message during accept");
        }
    }
}

void StreamingServer::send_to_all(shared::StreamingUpdate update) {
    for (auto &socket: this->sockets) {
        asio_utils::send_proto(socket, update);
    }
}
