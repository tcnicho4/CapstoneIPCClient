module;
#include <string>

export module Util.Win32;

export namespace Util
{
	namespace Win32
	{
		enum class ConsoleFormat
		{
			NORMAL,
			WARNING,
			CRITICAL_ERROR
		};

		void InitializeWin32Components();

		void WriteFormattedConsoleMessage(const std::wstring_view msg, const ConsoleFormat format = ConsoleFormat::NORMAL);
		void WriteFormattedConsoleMessage(const std::string_view msg, const ConsoleFormat format = ConsoleFormat::NORMAL);
	}
}