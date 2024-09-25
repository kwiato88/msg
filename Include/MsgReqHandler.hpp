#pragma once

namespace msg
{

template <typename Req, typename Resp>
class ReqHandler
{
public:
	typedef Req RequestType;
	typedef Resp ResponseType;

	virtual ~ReqHandler() {}
	virtual Resp handle(const Req& p_message) = 0;
};

template <typename Ind>
class IndHandler
{
public:
	typedef Ind IndicationType;

	virtual ~IndHandler() {}
	virtual void handle(const Ind& p_message) = 0;
};

}
