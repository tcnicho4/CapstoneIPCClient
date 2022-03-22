module;
#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>

module Capstone.UserCommandManager;
import Capstone.IPCClient;
import Util.Win32;

namespace
{
	template <Capstone::IPCMessageID MessageID>
	struct MessageIDDescription
	{
		static_assert(sizeof(MessageID) != sizeof(MessageID));
	};

	template <>
	struct MessageIDDescription<Capstone::IPCMessageID::REQUEST_THROTTLE>
	{
		static constexpr std::wstring_view OPTION_DESCRIPTION{ L"Request Throttle" };
	};

	template <Capstone::IPCMessageID MessageID>
	void PrintMessageIDDescriptions()
	{
		if constexpr (MessageID != Capstone::IPCMessageID::COUNT_OR_ERROR)
		{
			std::wcout << std::wstring{ std::to_wstring(std::to_underlying(MessageID) + 1) + L". " + std::wstring{ MessageIDDescription<MessageID>::OPTION_DESCRIPTION } + L"\n" };
			PrintMessageIDDescriptions<static_cast<Capstone::IPCMessageID>(std::to_underlying(MessageID) + 1)>();
		}
		else
			std::wcout << std::wstring{ std::to_wstring(std::to_underlying(MessageID) + 1) + L". Exit to Command Prompt\n\n" };
	}
}

namespace Capstone
{
	UserCommandManager::UserCommandManager() :
		mMessageCmdMap(),
		mIPCClient()
	{
		RegisterMessageCommands();
	}

	void UserCommandManager::ProcessUserInput()
	{
		// Connect to the simulation server.
		static constexpr std::wstring_view SIMULATION_PIPE_NAME{ L"__CAPSTONE_SIMULATOR_IPC__" };
		mIPCClient.ConnectToNamedPipe(SIMULATION_PIPE_NAME);
		
		while (true)
		{
			const IPCMessageID chosenMsgID = GetMessageIDFromUserInput();

			if (chosenMsgID == IPCMessageID::COUNT_OR_ERROR)
				break;

			const IPCServerResult serverResult = ExecuteMessageCommand(chosenMsgID);

			switch (serverResult)
			{
			case IPCServerResult::OK:
			{
				Util::Win32::WriteFormattedConsoleMessage(L"\nThe operation was successful. [Server Returned: IPCServerResult::OK]\n", Util::Win32::ConsoleFormat::SUCCESS);
				break;
			}

			case IPCServerResult::ERROR_INVALID_ARGUMENTS:
			{
				Util::Win32::WriteFormattedConsoleMessage(L"\nERROR: The provided arguments for the message were not considered valid. [Server Returned: IPCServerResult::ERROR_INVALID_ARGUMENTS]\n", Util::Win32::ConsoleFormat::CRITICAL_ERROR);
				break;
			}

			case IPCServerResult::ERROR_CORRUPT_STREAM:
			{
				Util::Win32::WriteFormattedConsoleMessage(L"\nERROR: The server has detected that its stream has been corrupted. Things will probably break spectacularly now. [Server Returned: IPCServerResult::ERROR_CORRUPT_STREAM]\n", Util::Win32::ConsoleFormat::CRITICAL_ERROR);
				break;
			}
			}
		}
	}

	void UserCommandManager::RegisterMessageCommands()
	{
		mMessageCmdMap[std::to_underlying(IPCMessageID::REQUEST_THROTTLE)] = [] (const IPCClient& client)
		{
			std::wcout << L"Enter the amount of throttle as a float in the range [0.0f, 1.0f] (NOTE: The value will be appropriately clamped as needed.): ";

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

			return client.TransactPipe<IPCMessageID::REQUEST_THROTTLE>(std::move(throttle));
		};
	}

	IPCMessageID UserCommandManager::GetMessageIDFromUserInput() const
	{
		while (true)
		{
			std::wcout << L"The following options are available:\n\n";

			PrintMessageIDDescriptions<static_cast<IPCMessageID>(0)>();

			std::wcout << L"Enter your choice here: ";

			std::wstring userStr{};
			std::getline(std::wcin, userStr);

			std::underlying_type_t<IPCMessageID> userInputNum = 0;

			try
			{
				userInputNum = std::stoi(userStr);
			}
			catch (const std::invalid_argument&)
			{
				std::wcout << L"\nERROR: This is an invalid input.\n\n";
				continue;
			}

			// Go back one on the userInputNum because enumerations start counting at 0.
			--userInputNum;

			if (userInputNum < 0 || userInputNum > std::to_underlying(IPCMessageID::COUNT_OR_ERROR))
			{
				std::wcout << L"\nERROR: The chosen number is outside of the range of valid inputs.\n\n";
				continue;
			}

			return static_cast<IPCMessageID>(userInputNum);
		}
	}

	IPCServerResult UserCommandManager::ExecuteMessageCommand(const IPCMessageID messageID) const
	{
		assert(messageID != IPCMessageID::COUNT_OR_ERROR && "ERROR: An attempt was made to execute a message command, but an invalid IPCMessageID was provided!");

		return mMessageCmdMap[std::to_underlying(messageID)](mIPCClient);
	}
}