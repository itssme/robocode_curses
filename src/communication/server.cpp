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
    std::cout << "peer has sent id: " << msg->port() << std::endl;
    response->Clear();
    response->set_id(10);
    return Status::OK;
}
