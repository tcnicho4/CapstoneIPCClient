module;
#include <type_traits>
#include <cassert>
#include <memory>
#include "Win32Def.h"

export module Capstone.SafeHandle;

namespace IMPL
{
	// A HANDLE is really just a void*.
	using UnderlyingHandleType = std::remove_pointer_t<HANDLE>;

	struct HandleDeleter
	{
		void operator()(HANDLE hObject)
		{
			if (hObject != nullptr && hObject != INVALID_HANDLE_VALUE) [[likely]]
			{
				const BOOL result = CloseHandle(hObject);
				assert(result && "ERROR: A HANDLE could not be properly closed!");
			}
		}
	};
}

export namespace Capstone
{
	using SafeHandle = std::unique_ptr<IMPL::UnderlyingHandleType, IMPL::HandleDeleter>;
}