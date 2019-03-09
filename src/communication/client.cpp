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
    //std::cout << "GOT:\n" << msg->DebugString() << std::endl;

    this->player.robot.pos_height = msg->pos().y();
    this->player.robot.pos_width = msg->pos().x();
    this->player.robot.energy = msg->energy();

    if (this->player.robot.energy <= 0) {
        std::cout << "dead" << std::endl;
    }

    GameObjects::BasicRobot* robot = nullptr;
    if (msg->has_scanned_robot()) {
        robot = new GameObjects::BasicRobot(msg->scanned_robot().pos().y(), msg->scanned_robot().pos().x(), 0, 0,
                                      msg->energy(), 0);
    }

    std::vector<int> hit_wall;
    for (auto wall: msg->hitwall()) {
        hit_wall.push_back(wall);
    }

    response->set_shot(this->player.tick(robot, hit_wall));

    response->mutable_pos()->set_y(this->player.robot.pos_height);
    response->mutable_pos()->set_x(this->player.robot.pos_width);
    response->mutable_speed()->set_y(this->player.robot.speed_height);
    response->mutable_speed()->set_x(this->player.robot.speed_width);
    response->mutable_gun_pos()->set_degrees(this->player.robot.gun_degree);
    response->mutable_gun_pos()->set_speed(this->player.robot.gun_speed);

    //std::cout << "\nSENT:\n" << response->DebugString() << std::endl;

    return Status::OK;
}
