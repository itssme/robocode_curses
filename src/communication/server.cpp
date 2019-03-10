/* 
 * author: Joel Klimont
 * filename: server.cpp
 * date: 25/01/19 
 * matrNr: i14083
 * class: 5CHIF
*/
#include "server.h"

/*!
 * Register a new clients
 * @param context server context
 * @param msg that the client has sent
 * @param response that has to be filled in by the server
 * @return a grpc status code
 */
Status ServerImpl::RegisterClient(grpc::ServerContext *context, const shared::Register *msg, shared::PlayerId *response) {
    std::string ip = context->peer();
    std::string token = ":";

    std::vector<std::string>result;
    while(!ip.empty()){
        long unsigned int index = static_cast<int>(ip.find(token));

        if(index != std::string::npos) {
            result.push_back(ip.substr(0, static_cast<unsigned long>(index)));
            ip = ip.substr(index+token.size());

            if(ip.empty()) {
                result.push_back(ip);
            }
        }else{
            result.push_back(ip);
            ip = "";
        }
    }

    spdlog::info("got connection from: {}", result.at(result.size()-2));

    ip = result.at(result.size()-2);
    std::string connect_str = ip + ":" + std::to_string(msg->port());
    this->connections.push_back(new Connection(grpc::CreateChannel(connect_str, grpc::InsecureChannelCredentials()),
                                               id, msg->name(), connect_str));

    response->Clear();
    response->set_id(id);
    id += 1;

    return Status::OK;
}
