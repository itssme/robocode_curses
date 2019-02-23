/* 
 * author: Joel Klimont
 * filename: connection.h
 * date: 25/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/
#ifndef ROBOCODE_CONNECTION_H
#define ROBOCODE_CONNECTION_H

#include <grpcpp/grpcpp.h>
#include <messages.pb.h>
#include <messages.grpc.pb.h>

using grpc::Status;

class Client final: public shared::Client::Service {
public:
    Status GetUpdate(grpc::ServerContext *context, const shared::UpdateFromServer *msg, shared::UpdateFromClient *response) override;
};

#endif //ROBOCODE_CONNECTION_H
