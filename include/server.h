/* 
 * author: Joel Klimont
 * filename: server.h
 * date: 25/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/
#ifndef ROBOCODE_SERVER_H
#define ROBOCODE_SERVER_H

#include <connection.h>
#include <vector>
#include <future>
#include "message.pb.h"

class Server {
private:
    std::vector<Connection> connections;
public:
    void get_connnections();
    void stop_getting_connections();
    std::future<shared::Position> get_update(int id);
};

#endif //ROBOCODE_SERVER_H
