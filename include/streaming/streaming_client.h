/* 
 * author: Joel Klimont
 * filename: streaming_client.h
 * date: 08/03/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#ifndef ROBOCODE_STREAMING_CLIENT_H
#define ROBOCODE_STREAMING_CLIENT_H

#include "streaming.h"

class StreamingClient {
private:
    std::thread asio_thread;
    void start_streaming_client(std::string server_address, short unsigned int port);
public:
    StreamingClient(std::string server_address, int port);
    ~StreamingClient();
};

#endif //ROBOCODE_STREAMING_CLIENT_H
