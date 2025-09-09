#pragma once
#include "CppUnitTest.h"
#include <string>
#include "../PresentMonAPI2/PresentMonAPI.h"
#include "../Interprocess/source/metadata/EnumStatus.h"
#include "../Interprocess/source/IntrospectionMacroHelpers.h"
#include "../CommonUtilities//str/String.h"

namespace Microsoft::VisualStudio::CppUnitTestFramework
{
    template<> static inline std::wstring ToString<PM_STATUS>(const PM_STATUS& value)
    {
        switch (value) {
#define GEN_CASE_(enum_name_fragment, key_name_fragment, name, short_name, description) \
        case MAKE_KEY_SYMBOL(enum_name_fragment, key_name_fragment): \
            return pmon::util::str::ToWide(STRINGIFY_MACRO_CALL( \
            MAKE_KEY_SYMBOL(enum_name_fragment, key_name_fragment)));

            ENUM_KEY_LIST_STATUS(GEN_CASE_)

#undef GEN_CASE_
        }
        return L"Unknown-Status-Code";
    }
}