module;
#include <string>

export module Util.General;

export namespace Util
{
	namespace General
	{
		std::wstring StringToWString(const std::string_view str);
		std::string WStringToString(const std::wstring_view wStr);
	}
}