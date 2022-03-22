module;
#include <cstdint>

export module Capstone.IPCServerResult;

export namespace Capstone
{
	enum class IPCServerResult : std::int32_t
	{
		/// <summary>
		/// This result indicates that the IPC server successfully received and
		/// processed the message.
		/// </summary>
		OK,

		/// <summary>
		/// This result indicates that the message was sent to the IPC server in
		/// the proper format, but that the input data sent as arguments for the
		/// message was not within the range of valid inputs. For instance, if
		/// the server expects a value in the range [0, 1], it would return this
		/// if it was instead sent a value of, say, 2.
		/// </summary>
		ERROR_INVALID_ARGUMENTS,

		/// <summary>
		/// This result indicates that the IPC server has detected that the stream
		/// of messages was corrupted. This is likely caused by sending messages in
		/// an unexpected format, e.g., forgetting to specify a required argument.
		/// 
		/// If this value is returned, then it is unlikely that the IPC server will
		/// be able to process any further requests.
		/// </summary>
		ERROR_CORRUPT_STREAM
	};

	__forceinline constexpr bool IPCServerOperationSucceeded(const IPCServerResult result)
	{
		return (result == IPCServerResult::OK);
	}
}