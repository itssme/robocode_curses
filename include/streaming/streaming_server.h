/* 
 * author: Joel Klimont
 * filename: streamingServer.h
 * date: 08/03/19 
 * matrNr: i14083
 * class: 5CHIF
*/
#ifndef ROBOCODE_STREAMINGSERVER_H
#define ROBOCODE_STREAMINGSERVER_H

#include "streaming.h"

using namespace asio::ip;
using asio::error_code;

class StreamingServer {
private:
    std::thread asio_thread;
    std::vector<tcp::socket> sockets;
    void start_streaming_server(short unsigned int port, int height, int width);
    bool stop{false};
    tcp::acceptor *acceptor;
public:
    void send_to_all(shared::StreamingUpdate update);
    void close_connections(shared::GameScores scores);
    StreamingServer(int port, int height, int width);
    ~StreamingServer();
};

#endif //ROBOCODE_STREAMINGSERVER_H
