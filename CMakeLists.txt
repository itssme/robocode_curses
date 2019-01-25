cmake_minimum_required(VERSION 3.9)

project(robocode)

set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)

include_directories(robocode include/)
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS src/proto/message.proto)

add_executable(robocode src/main.cpp src/proto/message.proto ${PROTO_SRCS} ${PROTO_HDRS} src/server.cpp include/server.h src/connection.cpp include/connection.h)

target_link_libraries(robocode ${PROTOBUF_LIBRARY})