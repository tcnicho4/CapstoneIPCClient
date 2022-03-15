#include <stdexcept>

import Capstone.Application;
import Util.Win32;

int main(int argc, char* argv[])
{
	try
	{
		Capstone::Application::GetInstance().Run();
	}
	catch (const std::exception& e)
	{
		Util::Win32::WriteFormattedConsoleMessage(e.what(), Util::Win32::ConsoleFormat::CRITICAL_ERROR);
		return 1;
	}

	return 0;
}