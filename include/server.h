/*
 * author: Joel Klimont
 * filename: server.h
 * date: 25/01/19
 * matrNr: i14083
 * class: 5CHIF
*/
#ifndef ROBOCODE_SERVER_H
#define ROBOCODE_SERVER_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <future>
#include <grpcpp/grpcpp.h>
#include <messages.pb.h>
#include <messages.grpc.pb.h>

#include <grpcpp/security/server_credentials.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;
using std::chrono::system_clock;
using shared::GameServer;

class Connection {
public:
    std::unique_ptr<shared::Client::Stub> stub_;
    int id;
    Connection(std::shared_ptr<grpc::Channel> channel, int id) : stub_(shared::Client::NewStub(channel)) {
        this->id = 10;
    }
    int sanity_check() {
        std::cout << "in check" << std::endl;
        shared::UpdateFromServer update;
        update.set_energy(100);
        update.add_hitrobot(0);
        update.add_hitwall(0);

        update.mutable_scanned_robot()->mutable_pos()->set_y(22.0);
        update.mutable_scanned_robot()->mutable_pos()->set_y(25.0);
        update.mutable_scanned_robot()->set_id(1);
        update.mutable_scanned_robot()->set_energy_left(80);

        update.mutable_pos()->set_y(10.0);
        update.mutable_pos()->set_x(12.0);

        shared::UpdateFromClient reply;
        grpc::ClientContext context;
        std::cout << context.peer() << std::endl;

        Status status = stub_->GetUpdate(&context, update, &reply);

        if (status.ok()) {
            std::cout << "got reply:" << std::endl;
            std::cout << reply.DebugString() << std::endl;
            return 0;
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return -1;
        }
    }
};


class ServerImpl final: public GameServer::Service {
public:
    int id = 0;
    std::vector<Connection*> connections;
    Status RegisterClient(grpc::ServerContext *context, const shared::Register *msg, shared::PlayerId *response) override;
};


#endif //ROBOCODE_SERVER_H
