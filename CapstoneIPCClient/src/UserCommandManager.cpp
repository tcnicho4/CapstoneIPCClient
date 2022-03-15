module;
#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <algorithm>

module Capstone.UserCommandManager;
import Capstone.IPCClient;
import Util.Win32;

namespace Capstone
{
	UserCommandManager::UserCommandManager() :
		mMessageCmdMap()
	{
		RegisterMessageCommands();
	}

	void UserCommandManager::RegisterMessageCommands()
	{
		mMessageCmdMap[std::to_underlying(IPCMessageID::REQUEST_THROTTLE)] = [] (IPCClient& client)
		{
			std::wcout << "Enter the amount of throttle as a float in the range [0.0f, 1.0f] (NOTE: The value will be appropriately clamped as needed.): ";

			std::wstring userStr{};
			std::getline(std::wcin, userStr);

			float throttle = 0.0f;

			try
			{
				throttle = std::stof(userStr);
			}
			catch (...)
			{
				return IPCServerResult::ERROR_INVALID_ARGUMENTS;
			}

			// Clamp the throttle.
			throttle = std::clamp(throttle, 0.0f, 1.0f);

			return client.TransactPipe<IPCMessageID::REQUEST_THROTTLE>(throttle);
		};
	}
}