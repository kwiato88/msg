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
	virtual void onError(std::exception&) {}
};

template <typename Ind>
class IndicationHandler
{
public:
	typedef Ind IndicationType;

	virtual ~IndicationHandler() {}
	virtual void handle(const Ind& p_message) = 0;
	virtual void onError(std::exception&) {}
};

}
