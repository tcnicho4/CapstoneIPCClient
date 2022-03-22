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
		
		std::uint32_t numBytesWritten = 0;
		IPCMessage messageToSend{
			.MsgID = MessageID,
			.Data{ std::move(dataToSend) }
		};

		std::uint32_t numBytesWritten = 0;

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

		MessageReceivedDataType<MessageID> receivedData;
		std::uint32_t numBytesRead = 0;

		

		while (numBytesRead != sizeof(receivedData))
		{
			hEvent.reset(CreateEvent(nullptr, TRUE, FALSE, nullptr));

			OVERLAPPED readFileOverlapped{};
			readFileOverlapped.hEvent = hEvent.get();

			result = ReadFile(
				mHPipe.get(),
				&receivedData,
				(sizeof(receivedData) - numBytesRead),
				nullptr,
				&readFileOverlapped
			);

			if (!result && GetLastError() != ERROR_IO_PENDING) [[unlikely]]
				throw std::runtime_error{ std::string{ "ERROR: ReadFile() failed during a call to IPCClient::TransactPipe() with error #" } + std::to_string(GetLastError()) + "!" };

			std::uint32_t numBytesReadThisCall = 0;

			// Wait for the read to finish.

			BOOL getReadResult = GetOverlappedResult(mHPipe.get(), &readFileOverlapped, reinterpret_cast<LPDWORD>(&numBytesReadThisCall), TRUE);
			assert(getReadResult);
			
			numBytesRead += numBytesReadThisCall;
		}

		return receivedData;
	}
}