module;

module Capstone.Application;
import Util.Win32;
import Capstone.UserCommandManager;

namespace Capstone
{
	void Application::Run()
	{
		Initialize();
		RunIMPL();
	}

	void Application::Initialize()
	{
		Util::Win32::InitializeWin32Components();
	}

	void Application::RunIMPL()
	{
		UserCommandManager cmdManager{};
		cmdManager.ProcessUserInput();
	}

	Application& Application::GetInstance()
	{
		static Application instance{};
		return instance;
	}
}