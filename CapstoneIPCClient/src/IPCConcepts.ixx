module;
#include <tuple>

export module Capstone.IPCConcepts;

export namespace Capstone
{
	template <typename T>
	concept PipeMessage = requires (T msg)
	{
		msg.SentData;
		msg.ReceivedData;
	};
}

namespace Capstone
{
	template <typename T>
		requires PipeMessage<T>
	consteval auto GetMessageTuple()
	{
		auto& [sentData, receivedData] = T{};
		return std::tie(sentData, receivedData);
	}

	template <typename T>
	struct MessageTupleSolver
	{};

	template <typename SentDataType_, typename ReceivedDataType_>
	struct MessageTupleSolver<std::tuple<SentDataType_, ReceivedDataType_>>
	{
		using SentDataType = std::remove_reference_t<SentDataType_>;
		using ReceivedDataType = std::remove_reference_t<ReceivedDataType_>;
	};
}

export namespace Capstone
{
	template <typename T>
		requires PipeMessage<T>
	using SentDataType = MessageTupleSolver<decltype(GetMessageTuple<T>())>::SentDataType;

	template <typename T>
		requires PipeMessage<T>
	using ReceivedDataType = MessageTupleSolver<decltype(GetMessageTuple<T>())>::ReceivedDataType;
}