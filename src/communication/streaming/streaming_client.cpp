/* 
 * author: Joel Klimont
 * filename: streaming_client.cpp
 * date: 07/03/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#include "streaming/streaming_client.h"

StreamingClient::StreamingClient(std::string server_address, int port) {
    this-> asio_thread = std::thread(&StreamingClient::start_streaming_client, this, server_address, port);
}

void StreamingClient::start_streaming_client(std::string server_address, short unsigned int port) {
    asio::io_context io_context;

    tcp::resolver resolver{io_context};
    auto results = resolver.resolve(server_address, std::to_string(port));
    tcp::socket socket{io_context};

    std::cout << "before connect: " << server_address << " port " << port << std::endl;
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
            std::cout << start.DebugString() << std::endl;
        } else {
            std::cout << "Received wrong message" << std::endl;
            spdlog::info("Did not receive correct start streaming message");
        }
    }
    std::cout << "after msg process" << std::endl;
    bool got_end = false;

    while (! got_end) {
        asio_utils::MessageType messageType;
        asio_utils::get_proto_type(socket, messageType);
        std::cout << "after update process" << std::endl;

        if (messageType == asio_utils::MessageType::StreamingUpdate) {
            shared::StreamingUpdate start;
            asio_utils::get_proto_msg(socket, start);
            std::cout << start.DebugString() << std::endl;

        } else if (messageType == asio_utils::MessageType::End) {
            shared::Empty end;
            asio_utils::get_proto_msg(socket, end);
            std::cout << "end message is: " << end.DebugString() << std::endl;
            got_end = true;
        } else {
            std::cout << "Received wrong message" << std::endl;
            spdlog::info("Received an unexpected message");
        }
    }

    socket.close();
}

StreamingClient::~StreamingClient() {
    this->asio_thread.join();
}