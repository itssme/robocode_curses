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

class Server {
private:
    std::vector<Connection> connections;
public:
    void get_connnections();
    void stop_getting_connections();
};

#endif //ROBOCODE_SERVER_H
