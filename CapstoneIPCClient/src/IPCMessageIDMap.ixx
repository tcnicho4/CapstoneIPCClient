module;

export module Capstone.IPCMessageIDMap;
import Capstone.IPCMessageID;
import Capstone.IPCServerResult;

namespace Capstone
{
	template <IPCMessageID MessageID>
	struct MessageInfo
	{
		static_assert(sizeof(MessageID) != sizeof(MessageID), "ERROR: An explicit template specialization of Capstone::MessageInfo was never provided for this IPCMessageID! (See IPCMessageIDMap.ixx.)");
	};

	template <typename SentDataType_, typename ReceivedDataType_>
	struct MessageInfoInstantiation
	{
		using SentDataType = SentDataType_;
		using ReceivedDataType = ReceivedDataType_;
	};

	// -------------------------------------------------------------------

	template <>
	struct MessageInfo<IPCMessageID::REQUEST_THROTTLE> : public MessageInfoInstantiation<float, IPCServerResult>
	{};
}

export namespace Capstone
{
	template <IPCMessageID MessageID>
	using MessageSentDataType = MessageInfo<MessageID>::SentDataType;

	template <IPCMessageID MessageID>
	using MessageReceivedDataType = MessageInfo<MessageID>::ReceivedDataType;
}