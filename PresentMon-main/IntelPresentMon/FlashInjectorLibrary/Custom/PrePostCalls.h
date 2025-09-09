#pragma once

#include "../ApiId.h"

namespace GfxLayer
{
	template<API_ID Id>
	struct PreCall
	{
		template <typename... Args>
		static bool Run(Args...)
		{
            return false;
        }
	};

	template<API_ID Id>
	struct PostCall
	{
		template <typename... Args>
		static void Run(Args...)
		{}
	};
}