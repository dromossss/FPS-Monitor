// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "PathSanitaryCheck.h"
#include <algorithm>
#include <Core/source/infra/util/FolderResolver.h>
#include "Logging.h"
#include <CommonUtilities/str/String.h>


namespace p2c::client::util
{
    using namespace ::p2c::infra;
    using namespace infra::util;

	std::filesystem::path ResolveSanitizedPath(FileLocation loc, std::wstring path)
	{
        // try to resolve location (Install or Appdata or Documents)
        std::filesystem::path base;
        {
            auto& fr = FolderResolver::Get();
            if (loc == FileLocation::Install) {
                base = fr.Resolve(FolderResolver::Folder::Install, L"");
            }
            else if (loc == FileLocation::Data) {
                base = fr.Resolve(FolderResolver::Folder::App, L"");
            }
            else if (loc == FileLocation::Documents) {
                base = fr.Resolve(FolderResolver::Folder::Documents, L"");
            }
            else {
                auto s = std::format("Bad file location: {}", uint32_t(loc));
                pmlog_error(s);
                throw std::runtime_error{ s };
            }
        }
        // compose path and make sure nobody is trying to escape sandbox
        auto filePath = base / path;
        if (!PathSanitaryCheck(filePath, base)) {
            auto s = std::format("Unsanitary path: {}", filePath.string());
            pmlog_error(s);
            throw std::runtime_error{ s };
        }
        return filePath;
	}
	bool PathSanitaryCheck(const std::filesystem::path& path, const std::filesystem::path& root)
	{
		const auto canonicalString = pmon::util::str::ToLower(std::filesystem::weakly_canonical(path).wstring());
		const auto rootString = pmon::util::str::ToLower(root.wstring());
        const auto isSanitary = canonicalString.starts_with(rootString);
        return isSanitary;
	}
}