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
#include "curses_drawable_objects.h"

class StreamingClient {
private:
    std::thread asio_thread;
    void start_streaming_client(WINDOW* game_window, std::string server_address, short unsigned int port);
public:
    shared::GameScores scores;
    bool stop{false};
    StreamingClient(WINDOW* game_window, std::string server_address, int port);
    ~StreamingClient();
};

#endif //ROBOCODE_STREAMING_CLIENT_H
