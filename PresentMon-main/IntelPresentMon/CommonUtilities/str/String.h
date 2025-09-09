// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <string_view>
#include <string>
#include <vector>

namespace pmon::util::str
{
	std::vector<std::string> TokenizeQuoted(const std::string& input);
	std::wstring ToWide(const std::string& narrow) noexcept;
	std::string ToNarrow(const std::wstring& wide) noexcept;
	template<class S> S TrimWhitespace(const S& input);
	std::string ToLower(const std::string& input);
	std::wstring ToLower(const std::wstring& input);
	std::string ToUpper(const std::string& input);
	std::wstring ToUpper(const std::wstring& input);
}