#include <iostream>
#include <fstream>
#include <string>
#include <message.pb.h>

using namespace std;


int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    shared::Position msg;
    msg.set_x(10);
    msg.set_y(10);

    cout << msg.x() << endl;
    cout << msg.y() << endl;

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}