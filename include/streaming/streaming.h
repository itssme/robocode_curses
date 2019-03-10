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

    /*!
     * Match message type to and id
     */
    enum class MessageType {
        StreamingUpdate = 1,
        StartStreaming = 2,
        GameScores = 3,
        WindowSize = 4,
        End = 5
    };

    /*!
     * Gets message type by comparing to the MessageType enum
     */
    const std::unordered_map<std::type_index, MessageType> typeMapping{
        {typeid(shared::StreamingUpdate), MessageType::StreamingUpdate},
        {typeid(shared::StartStreaming), MessageType::StartStreaming},
        {typeid(shared::GameScores), MessageType::GameScores},
        {typeid(shared::WindowSize), MessageType::WindowSize},
        {typeid(shared::Empty), MessageType::End}};

    /*!
     * Send a proto message over a socket
     * @param socket the message will be sent over
     * @param message that will be sent
     * @return a status code
     */
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

    /*!
     * Get type of proto message that will be received next
     * @param socket the message will be received from
     * @param messageType reference the type will be stored in
     * @return a status code
     */
    inline int get_proto_type(tcp::socket &socket, MessageType &message_type) {
        u_int8_t messageTypeRaw;

        socket.receive(buffer(&messageTypeRaw, sizeof(messageTypeRaw)), 0);
        message_type = static_cast<MessageType>(messageTypeRaw);

        return SEND_OK;
    }

    /*!
     * Get a proto message from the socket
     * @param socket socket the message will be received from
     * @param message reference the message will be stored in
     * @return a status code
     */
    inline int get_proto_msg(tcp::socket &socket, google::protobuf::Message &message) {
        u_int64_t message_size;
        socket.receive(buffer(&message_size, sizeof(message_size)), 0);

        streambuf streamBuffer;
        streambuf::mutable_buffers_type mutableBuffer{streamBuffer.prepare(message_size)};

        streamBuffer.commit(read(socket, mutableBuffer));

        istream inputStream{&streamBuffer};
        message.ParseFromIstream(&inputStream);

        return SEND_OK;
    }

} // namespace asio_utils

#endif //ROBOCODE_STREAMING_H
