#pragma once
#include <typeinfo>
#include <string>
#include <ranges>
#include <sstream>

namespace pmon::util::ref
{
	std::string DumpGenerated(const std::type_info&, const void* pStruct);
	template<class T>
	std::string DumpGenerated(const T& s)
	{
		if constexpr (std::ranges::range<T>) {
			std::ostringstream oss;
			oss << "[ ";
			for (auto&& [i, el] : s | std::views::enumerate) {
				oss << i << ": " << DumpGenerated(el) << ", ";
			}
			oss << "]";
			return oss.str();
		}
		else {
			return DumpGenerated(typeid(T), &s);
		}
	}

	bool SupportsGeneratedDump(const std::type_info&);
	template<class T>
	bool SupportsGeneratedDump(const T&)
	{
		return SupportsGeneratedDump<T>();
	}
	template<class T>
	bool SupportsGeneratedDump()
	{
		return SupportsGeneratedDump(typeid(T));
	}
}