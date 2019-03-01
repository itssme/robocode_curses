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
    std::cout << msg->DebugString() << std::endl;
    response->set_shot(true);
    response->mutable_pos()->set_y(12.0);
    response->mutable_pos()->set_x(10.0);
    response->mutable_speed()->set_y(1);
    response->mutable_speed()->set_x(1);
    response->mutable_gun_pos()->set_degrees(45);

    return Status::OK;
};
