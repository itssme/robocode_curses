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
#include <istream>
#include <sstream>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <messages.pb.h>
#include <messages.grpc.pb.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#pragma GCC diagnostic pop

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
    std::string username;
    std::string peer;
    Connection(std::shared_ptr<grpc::Channel> channel, int id, std::string name, std::string peer) : stub_(shared::Client::NewStub(channel)) {
        this->id = id;
        this->username = name;
        this->peer = peer;
    }
    shared::UpdateFromClient send_message(shared::UpdateFromServer message) {
        shared::UpdateFromClient reply;
        grpc::ClientContext context;

        Status status = stub_->GetUpdate(&context, message, &reply);

        if (status.ok()) {
            return reply;
        } else {
            //std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            spdlog::error(status.error_code() + ": " + status.error_message());
            shared::UpdateFromClient err;
            err.mutable_pos()->set_y(-1);
            return err;
        }
    }
    shared::UpdateFromClient sanity_check() {
        std::cout << "in check" << std::endl;
        shared::UpdateFromServer update;
        update.set_energy(100);
        update.add_hitrobot(0);
        update.add_hitwall(0);

        update.mutable_scanned_robot()->mutable_pos()->set_y(22.0);
        update.mutable_scanned_robot()->mutable_pos()->set_x(25.0);
        update.mutable_scanned_robot()->set_id(1);
        update.mutable_scanned_robot()->set_energy_left(80);

        update.mutable_pos()->set_y(10.0);
        update.mutable_pos()->set_x(12.0);

        shared::UpdateFromClient reply;
        grpc::ClientContext context;
        std::cout << context.peer() << std::endl;

        Status status = stub_->GetUpdate(&context, update, &reply);

        if (status.ok()) {
            return reply;
        } else {
            std::cout << status.error_code() << ": " << status.error_message() << std::endl;
            return shared::UpdateFromClient();
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
