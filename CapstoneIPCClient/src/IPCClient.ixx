module;
#include <string>
#include <stdexcept>
#include <cassert>
#include "Win32Def.h"

export module Capstone.IPCClient;
import Capstone.SafeHandle;
import Capstone.IPCMessageIDMap;
import Capstone.IPCMessageID;

export namespace Capstone
{
	class IPCClient
	{
	public:
		IPCClient() = default;
		explicit IPCClient(const std::wstring_view pipeName);

		IPCClient(const IPCClient& rhs) = delete;
		IPCClient& operator=(const IPCClient& rhs) = delete;

		IPCClient(IPCClient&& rhs) noexcept = default;
		IPCClient& operator=(IPCClient&& rhs) noexcept = default;

		void ConnectToNamedPipe(const std::wstring_view pipeName);

		template <IPCMessageID MessageID>
		MessageReceivedDataType<MessageID> TransactPipe(const MessageSentDataType<MessageID>& dataToSend);

	private:
		SafeHandle mHPipe;
	};
}

// ----------------------------------------------------------------------------------------------

namespace Capstone
{
	template <IPCMessageID MessageID>
	MessageReceivedDataType<MessageID> IPCClient::TransactPipe(const MessageSentDataType<MessageID>& dataToSend)
	{
		assert(mHPipe != nullptr && "ERROR: An attempt was made to call IPCClient::TransactPipe() before a connection to the IPC server could be established!");
		
		MessageReceivedDataType<MessageID> receivedData;
		std::uint32_t numBytesRead = 0;

		const BOOL result = TransactNamedPipe(
			mHPipe.get(),
			&dataToSend,
			sizeof(dataToSend),
			&receivedData,
			sizeof(receivedData),
			&numBytesRead,
			nullptr
		);

		if (!result) [[unlikely]]
		{
			const std::uint32_t lastError = GetLastError();

			// If lastError == ERROR_MORE_DATA, then we did not provide a large enough type to hold
			// the response data. In that case, we must have created our MessageType incorrectly.
			assert(lastError != ERROR_MORE_DATA && "ERROR: The IPC server responded with more data than what was expected for a given IPCMessageID!");

			throw std::runtime_error{ std::string{ "ERROR: TransactNamedPipe() failed during a call to IPCClient::TransactPipe() with error #" } + std::to_string(lastError) + "!" };
		}

		// This should always pass if result == TRUE, according to the documentation.
		assert(numBytesRead == sizeof(receivedData));

		return receivedData;
	}
}