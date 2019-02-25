/* 
 * author: Joel Klimont
 * filename: server.cpp
 * date: 25/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/
#include "server.h"

Status ServerImpl::RegisterClient(grpc::ServerContext *context, const shared::Register *msg, shared::PlayerId *response) {
    std::cout << "got message: " << msg->name() << std::endl;
    std::cout << "peer: " << context->peer() << std::endl;
    std::cout << "peer has sent port: " << msg->port() << std::endl;

    std::string ip = "localhost"; // TODO: extract from context->peer();

     std::cout << ip + ":" + std::to_string(msg->port()) << std::endl;
     std::string connect_str = ip + ":" + std::to_string(msg->port());
    this->connections.push_back(new Connection(grpc::CreateChannel(connect_str, grpc::InsecureChannelCredentials()), id));
    //this->connections.push_back(new Connection(grpc::CreateChannel("localhost:41553", grpc::InsecureChannelCredentials()), id));

    response->Clear();
    response->set_id(id);
    id += 1;

    return Status::OK;
}
