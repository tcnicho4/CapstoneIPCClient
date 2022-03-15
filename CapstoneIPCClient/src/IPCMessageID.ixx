module;
#include <cstdint>

export module Capstone.IPCMessageID;

export namespace Capstone
{
	enum class IPCMessageID : std::int32_t
	{
		/// <summary>
		/// IPCMessageID::REQUEST_THROTTLE
		/// 
		/// Description: Makes a request to the simulator to change the vehicle's
		/// throttle. Allowed values are in the range [0.0f, 1.0f].
		/// 
		/// Input Type: float
		/// Output Type: IPCServerResult
		/// </summary>
		REQUEST_THROTTLE,

		COUNT_OR_ERROR
	};
}