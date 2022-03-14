module;
#include <string>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include "Win32Def.h"

module Util.Win32;
import Util.General;

namespace
{
	void EnableVirtualTerminalSequences()
	{
		const HANDLE hSTDOut = GetStdHandle(STD_OUTPUT_HANDLE);

		if (hSTDOut == INVALID_HANDLE_VALUE || hSTDOut == nullptr) [[unlikely]]
			throw std::runtime_error{ "ERROR: GetStdHandle() failed to get a handle to STDOUT!" };

		DWORD dwConsoleMode = 0;

		if (!GetConsoleMode(hSTDOut, &dwConsoleMode)) [[unlikely]]
			throw std::runtime_error{ "ERROR: GetConsoleMode() failed to get the mode of the STDOUT buffer!" };

		// Enable virtual terminal sequences.
		dwConsoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

		if (!SetConsoleMode(hSTDOut, dwConsoleMode)) [[unlikely]]
			throw std::runtime_error{ "ERROR: SetConsoleMode() could not be used to enable virtual terminal sequences for STDOUT!" };
	}

	std::wstring CreateFormattedConsoleMessage(const std::wstring_view msg, const Util::Win32::ConsoleFormat format)
	{
		// You can find more information regarding the virtual terminal sequences supported by Windows
		// at https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences.

		switch (format)
		{
		case Util::Win32::ConsoleFormat::NORMAL:
			return std::wstring{ L"\x1b[22;39m" } + std::wstring{ msg } + L"\x1b[0m\n";

		case Util::Win32::ConsoleFormat::WARNING:
			return std::wstring{ L"\x1b[22;33m" } + std::wstring{ msg } + L"\x1b[0m\n";

		case Util::Win32::ConsoleFormat::CRITICAL_ERROR:
			return std::wstring{ L"\x1b[1;31m" } + std::wstring{ msg } + L"\x1b[0m\n";

		default:
		{
			// This should never happen.
			assert(false);
			return std::wstring{ msg };
		}
		}
	}
}

namespace Util
{
	namespace Win32
	{
		void InitializeWin32Components()
		{
			EnableVirtualTerminalSequences();
		}

		void WriteFormattedConsoleMessage(const std::wstring_view msg, const ConsoleFormat format)
		{
			const std::wstring consoleMsg{ CreateFormattedConsoleMessage(msg, format) };
			std::wcout << consoleMsg;
		}

		void WriteFormattedConsoleMessage(const std::string_view msg, const ConsoleFormat format)
		{
			return WriteFormattedConsoleMessage(Util::General::StringToWString(msg), format);
		}
	}
}