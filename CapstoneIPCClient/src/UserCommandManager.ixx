module;
#include <array>
#include <functional>

export module Capstone.UserCommandManager;
import Capstone.IPCMessageID;
import Capstone.IPCServerResult;

export namespace Capstone
{
	class IPCClient;
}

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

	private:
		void RegisterMessageCommands();

	private:
		std::array<std::function<IPCServerResult(IPCClient&)>, std::to_underlying(IPCMessageID::COUNT_OR_ERROR)> mMessageCmdMap;
	};
}