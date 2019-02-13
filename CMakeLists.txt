cmake_minimum_required(VERSION 3.9)

project(robocode)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lncurses -lm -pthread")

find_package(Protobuf REQUIRED)

include_directories(robocode include/)
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS src/proto/messages.proto)

add_executable(robocode src/main.cpp src/proto/messages.proto ${PROTO_SRCS} ${PROTO_HDRS} src/communication/server.cpp include/server.h src/communication/connection.cpp include/connection.h include/robot.h include/curses_utils.h src/game_objects/robot.cpp src/game_objects/game.cpp include/game.h src/curses_drawable_objects.cpp include/curses_drawable_objects.h src/game_objects/bullet.cpp include/bullet.h)

target_link_libraries(robocode ${PROTOBUF_LIBRARY})