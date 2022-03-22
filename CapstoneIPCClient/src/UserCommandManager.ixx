module;
#include <array>
#include <functional>

export module Capstone.UserCommandManager;
import Capstone.IPCMessageID;
import Capstone.IPCServerResult;
import Capstone.IPCClient;

export namespace Capstone
{
	class UserCommandManager
	{
	public:
		UserCommandManager();

		UserCommandManager(const UserCommandManager& rhs) = delete;
		UserCommandManager& operator=(const UserCommandManager& rhs) = delete;

		UserCommandManager(UserCommandManager&& rhs) noexcept = default;
		UserCommandManager& operator=(UserCommandManager&& rhs) noexcept = default;

		void ProcessUserInput();

	private:
		void RegisterMessageCommands();
		IPCMessageID GetMessageIDFromUserInput() const;
		IPCServerResult ExecuteMessageCommand(const IPCMessageID messageID) const;

	private:
		std::array<std::function<IPCServerResult(const IPCClient&)>, std::to_underlying(IPCMessageID::COUNT_OR_ERROR)> mMessageCmdMap;
		IPCClient mIPCClient;
	};
}