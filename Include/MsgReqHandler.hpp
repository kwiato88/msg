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

}
