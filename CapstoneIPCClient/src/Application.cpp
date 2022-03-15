module;

module Capstone.Application;
import Util.Win32;

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
		// TODO: Implement the application logic here.
	}

	Application& Application::GetInstance()
	{
		static Application& instance{};
		return instance;
	}
}