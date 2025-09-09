#pragma once
#include <string>
#include <memory>
#include "WinAPI.h"
#include "Handle.h"

namespace pmon::util::win
{
	using SidPtr = std::unique_ptr<void,decltype(&FreeSid)>;
	// we need to keep a related SID alive until we're done with the token so we bundle them together here
	struct TokenPack
	{
		SidPtr pSid;
		Handle hMediumToken;
	};
	Handle OpenCurrentProcessToken(DWORD rightsFlags);
	void EnablePrivilege(HANDLE hToken, const std::string& privName);
	TokenPack PrepareMediumIntegrityToken();
	bool WeAreElevated() noexcept;
}