#pragma once
#include <concepts>
#include <string>
#include <sstream>
#include <ranges>
#include <optional>
#include "WrapReflect.h"
#include "../Meta.h"
#include <bitset>
#include <map>


namespace pmon::util::ref
{
	template<class S>
	void DumpStaticImpl_(const S& s, std::ostringstream& oss)
	{
		if constexpr (IsContainer<std::optional, S>) {
			if (s) {
				oss << *s;
			}
			else {
				oss << "{empty}";
			}
		}
		else if constexpr (std::same_as<std::string, S> || std::same_as<bool, S> || std::is_arithmetic_v<S>) {
			oss << s;
		}
		else if constexpr (std::ranges::range<S>) {
			oss << "[ ";
			for (auto& el : s) {
				DumpStaticImpl_(el, oss);
				oss << ", ";
			}
			oss << "], ";
		}
		else if constexpr (IsContainerLike<S>) {
			oss << "{unknown}";
		}
		else if constexpr (std::is_enum_v<S>) {
			oss << reflect::type_name<S>() << "::" << reflect::enum_name(s);
		}
		else if constexpr (std::is_class_v<S>) {
			oss << "struct " << reflect::type_name(s) << " { ";
			if constexpr (reflect::size<S>() == 0) {
				oss << "{empty} }";
				return;
			}
			reflect::for_each([&](const auto I) {
				oss << "." << reflect::member_name<I>(s) << " = ";
				DumpStaticImpl_(reflect::get<I>(s), oss);
				oss << ", ";
				}, s);
			oss << " }";
		}
		else {
			oss << "{unknown}";
		}
	}

	template<class S>
	std::string DumpStatic(const S& s)
	{
		std::ostringstream oss;
		oss << std::boolalpha;
		DumpStaticImpl_(s, oss);
		return oss.str();
	}

	template<typename E, size_t N>
	std::string DumpEnumBitset(const std::bitset<N>& set)
	{
		std::ostringstream oss;
		oss << "| ";
		for (size_t n = 0; n < N; n++) {
			if (!set.test(n)) {
				continue;
			}
			const auto name = reflect::enum_name(E(n));
			if (!name.length()) {
				continue;
			}
			oss << name << ", ";
		}
		oss << "|";
		return oss.str();
	}

	template<typename E>
	std::string DumpEnumFlagSet(uint64_t bits)
	{
		// simple hacky impl assumes 32-bit ints, logically unsigned for flags
		// unless underlying type is 8-bytes, then assume 64-bit
		size_t nBits;
		if constexpr (sizeof(std::underlying_type_t<E>) == 8) {
			nBits = 64;
		}
		else {
			nBits = 32;
		}
		std::ostringstream oss;
		oss << "| ";
		// loop over n-bits, shifting a 1-hot bit value to test all positions
		for (size_t n = 0, b = 1; n < nBits; n++, b<<1) {
			if (!(bits & b)) {
				continue;
			}
			const auto name = reflect::enum_name(E(b));
			if (!name.length()) {
				continue;
			}
			oss << name << ", ";
		}
		oss << "|";
	}

	template<typename E>
	auto GenerateEnumKeyMap()
	{
		std::map<std::string, E> map;
		for (int i = 0; i < int(E::Count); i++) {			
			if (const auto name = reflect::enum_name(E(i)); name.length()) {
				map[std::string(name)] = E(i);
			}
		}
		return map;
	}
}