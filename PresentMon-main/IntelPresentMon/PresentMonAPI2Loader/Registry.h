#pragma once
#include "../CommonUtilities/reg/Registry.h"
#include "../PresentMonService/GlobalIdentifiers.h"

using namespace pmon::util;
struct Reg : public reg::RegistryBase<Reg, HKEY_LOCAL_MACHINE>
{
	Value<std::string> middlewarePath{ this, pmon::gid::middlewarePathKey };
	static constexpr const wchar_t* keyPath_ = pmon::gid::registryPath;
};