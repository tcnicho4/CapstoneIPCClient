module;
#include <string>
#include <stdexcept>
#include "Win32Def.h"

module Util.General;

namespace Util
{
	namespace General
	{
		std::wstring StringToWString(const std::string_view str)
		{
			const std::int32_t requiredBufferSize = MultiByteToWideChar(
				CP_UTF8,
				0,
				str.data(),
				-1,
				nullptr,
				0
			);

			if (requiredBufferSize == 0) [[unlikely]]
				throw std::runtime_error{ "ERROR: MultiByteToWideChar() failed to get the required buffer size for conversion!" };

			wchar_t* wCString = new wchar_t[requiredBufferSize];

			const std::int32_t numCharsWritten = MultiByteToWideChar(
				CP_UTF8,
				0,
				str.data(),
				-1,
				wCString,
				requiredBufferSize
			);

			if (numCharsWritten != requiredBufferSize) [[unlikely]]
				throw std::runtime_error{ "ERROR: MultiByteToWideChar() failed to convert a string properly!" };

			const std::wstring wStr{ wCString };
			delete[] wCString;

			return wStr;
		}

		std::string WStringToString(const std::wstring_view wStr)
		{
			const std::int32_t requiredBufferSize = WideCharToMultiByte(
				CP_UTF8,
				0,
				wStr.data(),
				-1,
				nullptr,
				0,
				nullptr,
				nullptr
			);

			if(requiredBufferSize == 0) [[unlikely]]
				throw std::runtime_error{ "ERROR: WideCharToMultiByte() failed to get the required buffer size for conversion!" };

			char* cString = new char[requiredBufferSize];

			const std::int32_t numCharsWritten = WideCharToMultiByte(
				CP_UTF8,
				0,
				wStr.data(),
				-1,
				cString,
				requiredBufferSize,
				nullptr,
				nullptr
			);

			if (numCharsWritten != requiredBufferSize) [[unlikely]]
				throw std::runtime_error{ "ERROR: WideCharToMultiByte() failed to convert a string properly!" };

			const std::string str{ cString };
			delete[] cString;

			return str;
		}
	}
}