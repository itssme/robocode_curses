/* 
 * author: Joel Klimont
 * filename: streaming.h
 * date: 07/03/19 
 * matrNr: i14083
 * class: 5CHIF
*/

#ifndef ROBOCODE_STREAMING_H
#define ROBOCODE_STREAMING_H

#include <asio.hpp>
#include <iostream>
#include <ostream>
#include <grpcpp/grpcpp.h>
#include <messages.pb.h>
#include <messages.grpc.pb.h>
#include <typeindex>
#include <bits/unordered_map.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#pragma GCC diagnostic pop

using namespace asio::ip;
using asio::buffer;
using asio::streambuf;
using asio::write;
using asio::ip::tcp;

using std::istream;
using std::ostream;

const int SEND_OK{0};

namespace asio_utils {

    template <typename E>
    constexpr auto toUnderlying(E e) noexcept {
        return static_cast<u_int8_t>(static_cast<std::underlying_type_t<E>>(e));
    }

    enum class MessageType {
        StreamingUpdate = 1,
        StartStreaming = 2,
        End = 3
    };

    const std::unordered_map<std::type_index, MessageType> typeMapping{
        {typeid(shared::StreamingUpdate), MessageType::StreamingUpdate},
        {typeid(shared::StartStreaming), MessageType::StartStreaming},
        {typeid(shared::Empty), MessageType::End}};

    inline int send_proto(tcp::socket &socket, google::protobuf::Message &message) {
        u_int8_t messageType{toUnderlying(typeMapping.at(typeid(message)))};
        u_int64_t messageSize{static_cast<u_int64_t>(message.ByteSize())};

        asio::write(socket, buffer(&messageType, sizeof(messageType)));
        asio::write(socket, buffer(&messageSize, sizeof(messageSize)));

        streambuf streamBuffer;
        ostream outputStream(&streamBuffer);
        message.SerializeToOstream(&outputStream);
        asio::write(socket, streamBuffer);
        return SEND_OK;
    }

    inline int get_proto_type(tcp::socket &socket, MessageType &messageType) {
        u_int8_t messageTypeRaw;

        socket.receive(buffer(&messageTypeRaw, sizeof(messageTypeRaw)), 0);
        messageType = static_cast<MessageType>(messageTypeRaw);

        return SEND_OK;
    }

    inline int get_proto_msg(tcp::socket &socket, google::protobuf::Message &message) {
        u_int64_t messageSize;
        socket.receive(buffer(&messageSize, sizeof(messageSize)), 0);

        streambuf streamBuffer;
        streambuf::mutable_buffers_type mutableBuffer{streamBuffer.prepare(messageSize)};

        streamBuffer.commit(read(socket, mutableBuffer));

        istream inputStream{&streamBuffer};
        message.ParseFromIstream(&inputStream);

        return SEND_OK;
    }

} // namespace asio_utils

#endif //ROBOCODE_STREAMING_H
