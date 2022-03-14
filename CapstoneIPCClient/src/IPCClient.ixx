module;
#include <string>
#include <stdexcept>
#include <cassert>
#include "Win32Def.h"

export module Capstone.IPCClient;
import Capstone.SafeHandle;
import Capstone.IPCConcepts;

export namespace Capstone
{
	class IPCClient
	{
	public:
		explicit IPCClient(const std::wstring_view pipeName);

		IPCClient(const IPCClient& rhs) = delete;
		IPCClient& operator=(const IPCClient& rhs) = delete;

		IPCClient(IPCClient&& rhs) noexcept = default;
		IPCClient& operator=(IPCClient&& rhs) noexcept = default;

		/// <summary>
		/// Initiates a send-receive message call through the pipe. The data specified by dataToSend
		/// is the data which is to be sent through the pipe to the server. The return value of this
		/// function is the response data received from the server as a result of processing this
		/// message.
		/// 
		/// The types of the sent and received data are deduced from the MessageType parameter; see
		/// its documentation for more details.
		/// </summary>
		/// <typeparam name="MessageType">
		/// - This is a type which must satisfy the Capstone::PipeMessage concept, i.e., it must have
		///   two fields: SentData, which describes the data sent to the server, and ReceivedData,
		///   which describes the data received from the server. (See IPCConcepts.ixx for its
		///   implementation.)
		/// </typeparam>
		/// <param name="dataToSend">
		/// - The data which is to be sent to the server.
		/// </param>
		/// <returns>
		/// The function returns the data which was received from the server as a result of processing
		/// this message.
		/// </returns>
		template <typename MessageType>
			requires PipeMessage<MessageType>
		ReceivedDataType<MessageType> TransactPipe(const SentDataType<MessageType>& dataToSend);

	private:
		void ConnectToNamedPipe(const std::wstring_view pipeName);

	private:
		SafeHandle mHPipe;
	};
}

// ----------------------------------------------------------------------------------------------

namespace Capstone
{
	template <typename MessageType>
		requires PipeMessage<MessageType>
	ReceivedDataType<MessageType> IPCClient::TransactPipe(const SentDataType<MessageType>& dataToSend)
	{
		ReceivedDataType<MessageType> receivedData;
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
			assert(lastError != ERROR_MORE_DATA && "ERROR: An invalid response data type was specified for a MessageType used in IPCClient::TransactPipe()!");

			throw std::runtime_error{ std::string{ "ERROR: TransactNamedPipe() failed during a call to IPCClient::TransactPipe() with error #" } + std::to_string(lastError) + "!" };
		}

		// This should always pass if result == TRUE, according to the documentation.
		assert(numBytesRead == sizeof(receivedData));

		return receivedData;
	}
}