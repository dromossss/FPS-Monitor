// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <filesystem>
#include "FileLocation.h"

namespace p2c::client::util
{
	std::filesystem::path ResolveSanitizedPath(FileLocation loc, std::wstring path);
	bool PathSanitaryCheck(const std::filesystem::path& path, const std::filesystem::path& root);
}