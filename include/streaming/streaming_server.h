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
    void start_streaming_server(short unsigned int port);
    bool stop{false};
    tcp::acceptor *acceptor;
public:
    void send_to_all(shared::StreamingUpdate update);
    StreamingServer(int port);
    ~StreamingServer();
};



#endif //ROBOCODE_STREAMINGSERVER_H
