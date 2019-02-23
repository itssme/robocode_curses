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
    Connection(std::shared_ptr<grpc::Channel> channel, const std::string& db) : stub_(shared::Client::NewStub(channel)) {}
};


class ServerImpl final: public GameServer::Service {
public:
    Status RegisterClient(grpc::ServerContext *context, const shared::Empty *msg, shared::PlayerId *response) override;
};


#endif //ROBOCODE_SERVER_H
