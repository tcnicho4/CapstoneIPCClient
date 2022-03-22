module;
#include <string>
#include <stdexcept>
#include <cassert>
#include <memory>
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
		MessageReceivedDataType<MessageID> TransactPipe(MessageSentDataType<MessageID>&& dataToSend) const;

	private:
		SafeHandle mHPipe;
	};
}

// ----------------------------------------------------------------------------------------------

namespace Capstone
{
	static constexpr std::wstring_view SERVER_RESPONSE_EVENT_NAME = L"__CAPSTONE_IPC_SERVER_RESPONSE_EVENT__";
}

namespace Capstone
{
	template <IPCMessageID MessageID>
	MessageReceivedDataType<MessageID> IPCClient::TransactPipe(MessageSentDataType<MessageID>&& dataToSend) const
	{
		// Pack the generated structure tightly. That way, we don't have to worry
		// about alignment issues when receiving the data on the server.
#pragma pack(push, 1)
		struct IPCMessage
		{
			IPCMessageID MsgID;
			MessageSentDataType<MessageID> Data;
		};
#pragma pack(pop)
		
		assert(mHPipe != nullptr && "ERROR: An attempt was made to call IPCClient::TransactPipe() before a connection to the IPC server could be established!");
		
		// We'll need an event to signal across processes that the IPC server has
		// received the message, processed it, and written the message result to the
		// pipe. This is because we cannot have two threads simultaneously writing to
		// and reading from the same pipe. (Another option would be to create a separate
		// pipe which the server sends its response through.)
		//
		// However, even though the event is used to signal that the server has
		// responded to a message, we need to create the event *BEFORE* we send the
		// message. Otherwise, we will have a race condition where the server attempts
		// to open a handle to the event before the client could create it.
		SafeHandle hServerResponseEvent{ CreateEvent(nullptr, TRUE, FALSE, SERVER_RESPONSE_EVENT_NAME.data()) };
		
		std::uint32_t numBytesWritten = 0;
		IPCMessage messageToSend{
			.MsgID = MessageID,
			.Data{ std::move(dataToSend) }
		};

		BOOL result = WriteFile(
			mHPipe.get(),
			&messageToSend,
			sizeof(messageToSend),
			reinterpret_cast<LPDWORD>(&numBytesWritten),
			nullptr
		);

		if (!result) [[unlikely]]
			throw std::runtime_error{ std::string{ "ERROR: WriteFile() failed during a call to IPCClient::TransactPipe() with error #" } + std::to_string(GetLastError()) + "!" };

		assert(numBytesWritten == sizeof(messageToSend));

		// Wait for the stream to be written to by the IPC server.
		{
			const DWORD waitResult = WaitForSingleObject(hServerResponseEvent.get(), INFINITE);

			if (waitResult == WAIT_FAILED) [[unlikely]]
				throw std::runtime_error{ std::string{ "ERROR: WaitForSingleObject() failed during a call to IPCClient::TransactPipe() with error #" } + std::to_string(GetLastError()) + "!" };

			assert(waitResult == WAIT_OBJECT_0);
		}

		// We can now proceed to read from the pipe.
		MessageReceivedDataType<MessageID> receivedData;
		std::uint32_t numBytesRead = 0;

		result = ReadFile(
			mHPipe.get(),
			&receivedData,
			sizeof(receivedData),
			reinterpret_cast<LPDWORD>(&numBytesRead),
			nullptr
		);

		if (!result) [[unlikely]]
			throw std::runtime_error{ std::string{ "ERROR: ReadFile() failed during a call to IPCClient::TransactPipe() with error #" } + std::to_string(GetLastError()) + "!" };

		assert(numBytesRead == sizeof(receivedData));

		return receivedData;
	}
}