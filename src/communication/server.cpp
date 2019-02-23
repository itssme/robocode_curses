/* 
 * author: Joel Klimont
 * filename: server.cpp
 * date: 25/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/
#include "server.h"

Status ServerImpl::RegisterClient(grpc::ServerContext *context, const shared::Empty *msg, shared::PlayerId *response) {
    response->Clear();
    return Status::OK;
}
