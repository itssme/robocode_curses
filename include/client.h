/* 
 * author: Joel Klimont
 * filename: connection.h
 * date: 25/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/
#ifndef ROBOCODE_CONNECTION_H
#define ROBOCODE_CONNECTION_H

#include "player.h"

#include <grpcpp/grpcpp.h>
#include <messages.pb.h>
#include <messages.grpc.pb.h>


using grpc::Status;

class Advertise {
public:
    std::unique_ptr<shared::GameServer::Stub> stub_;
    Advertise(std::shared_ptr<grpc::Channel> channel)
            : stub_(shared::GameServer::NewStub(channel)) {}
    int Register(const std::string& username, const int port) {
        shared::Register request;
        request.set_name(username);
        request.set_port(port);

        shared::PlayerId reply;
        grpc::ClientContext context;

        Status status = stub_->RegisterClient(&context, request, &reply);

        if (status.ok()) {
            return reply.id();
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -1;
        }
    }
};


class ClientImpl final: public shared::Client::Service {
public:
    //Player player;
    //ClientImpl(Player player) : player(player) {}
    Status GetUpdate(grpc::ServerContext *context, const shared::UpdateFromServer *msg, shared::UpdateFromClient *response) override;
};

#endif //ROBOCODE_CONNECTION_H
