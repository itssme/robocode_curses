/* 
 * author: Joel Klimont
 * filename: connection.cpp
 * date: 25/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/
#include "client.h"

Status ClientImpl::GetUpdate(grpc::ServerContext *context, const shared::UpdateFromServer *msg,
                               shared::UpdateFromClient *response) {
    return Status::OK;
}
