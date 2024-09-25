#pragma once

#include "MsgClient.hpp"

namespace msg
{

// Definitions inside Codec
//   typename Codec::ExceptionType
//   typename Codec::IdType
//   IdType Codec::getId(const std::string& p_data)
//   std::string Codec::encode(const MsgType& msg)
//   MsgType Codec::decode(const std::string& msg)
template <typename Codec>
class DecodingClient
{
public:
    DecodingClient(ConnectionFactory p_factory)
        : client(p_factory)
    {}

    template <typename Req, typename Resp>
    Resp sendReq(const Req& p_req);
    template <typename Ind>
    void sendInd(const Ind& p_ind);

private:
    Client client;
};

template <typename Codec>
template <typename Req, typename Resp>
Resp DecodingClient<Codec>::sendReq(const Req& p_req)
{
    return Codec::template decode<Resp>(client.sendReq(Codec::encode(p_req)));
}

template <typename Codec>
template <typename Ind>
void DecodingClient<Codec>::sendInd(const Ind& p_ind)
{
    client.sendInd(Codec::encode(p_ind));
}

} // namespace msg
