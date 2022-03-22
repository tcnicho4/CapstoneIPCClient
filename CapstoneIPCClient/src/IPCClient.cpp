module;
#include <string>
#include <stdexcept>
#include "Win32Def.h"

module Capstone.IPCClient;
import Util.Win32;
import Util.General;

namespace Capstone
{
	IPCClient::IPCClient(const std::wstring_view pipeName) :
		mHPipe(nullptr)
	{
		ConnectToNamedPipe(pipeName);
	}

	void IPCClient::ConnectToNamedPipe(const std::wstring_view pipeName)
	{
		{
			const std::wstring connectingMsg{ std::wstring{ L"Waiting for the pipe " } + std::wstring{ pipeName } + std::wstring{ L" to become available..." } };
			Util::Win32::WriteFormattedConsoleMessage(connectingMsg, Util::Win32::ConsoleFormat::NORMAL);
		}

		const std::wstring connectionPipeName{ L"\\\\.\\pipe\\" + std::wstring{pipeName} };
		
		// Wait for the pipe to become available. The function returns immediately if the
		// pipe does not exist, so we must wait here in a loop.
		while (!WaitNamedPipe(connectionPipeName.c_str(), NMPWAIT_WAIT_FOREVER));

		// Create the pipe connection.
		{
			HANDLE hPipe = CreateFile(
				connectionPipeName.c_str(),
				(GENERIC_READ | GENERIC_WRITE),
				0,
				nullptr,
				OPEN_EXISTING,
				(FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN),
				nullptr
			);

			if (hPipe == INVALID_HANDLE_VALUE) [[unlikely]]
				throw std::runtime_error{ std::string{ "ERROR: CreateFile() failed to open the pipe " } + Util::General::WStringToString(pipeName) + "!" };

			mHPipe = SafeHandle{ hPipe };
		}
	}
}