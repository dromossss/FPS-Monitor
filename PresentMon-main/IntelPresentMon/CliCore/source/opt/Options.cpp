#pragma once
#define PMCLI_OPTS_CPP
#include "Options.h"
#include <algorithm>

namespace p2c::cli::opt::impl
{
	std::vector<std::string> OptionsStruct::GetColumnGroups() const
	{
		std::vector<std::string> groups{
#define X_(name, desc) #name,
			COLUMN_GROUP_LIST
#undef X_
		};
		std::erase_if(groups, [](const std::string& g) {
			return !App<OptionsStruct>::CheckOptionPresence("--" + g);
		});
		return groups;
	}
}