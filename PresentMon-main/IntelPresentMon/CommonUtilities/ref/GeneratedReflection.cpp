#include "GeneratedReflection.h"
#include <unordered_map>
#include <typeindex>
#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <any>
#include <sstream>

#include "gen/GeneratedReflection.h"

namespace pmon::util::ref
{
	namespace {
		bool initialized_ = false;
		std::unordered_map<std::type_index, std::function<std::string(const void*)>> dumpers_;

		void Init_()
		{
			gen::RegisterDumpers_(dumpers_);
		}
	}

	std::string DumpGenerated(const std::type_info& type, const void* pStruct)
	{
		if (!initialized_) {
			Init_();
		}
		if (auto i = dumpers_.find(type); i != dumpers_.end()) {
			return i->second(pStruct);
		}
		return "{ unknown }";
	}

	bool SupportsGeneratedDump(const std::type_info& type)
	{
		if (!initialized_) {
			Init_();
		}
		return dumpers_.contains(type);
	}
}