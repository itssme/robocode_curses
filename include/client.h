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

class Advertise {
public:
    std::unique_ptr<shared::GameServer::Stub> stub_;
    Advertise(std::shared_ptr<grpc::Channel> channel) : stub_(shared::GameServer::NewStub(channel)) {}
    int Register(const std::string& username, const int port) {
        // Data we are sending to the server.
        shared::Register request;
        request.set_name(username);
        request.set_port(port);

        // Container for the data we expect from the server.
        shared::PlayerId reply;

        // Context for the client. It could be used to convey extra information to
        // the server and/or tweak certain RPC behaviors.
        grpc::ClientContext context;

        // The actual RPC.
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
    Status GetUpdate(grpc::ServerContext *context, const shared::UpdateFromServer *msg, shared::UpdateFromClient *response) override;
};

#endif //ROBOCODE_CONNECTION_H
