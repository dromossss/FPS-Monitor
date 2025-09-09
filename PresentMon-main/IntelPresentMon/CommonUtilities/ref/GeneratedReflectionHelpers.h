#pragma once
#include <string>
#include <concepts>
#include "GeneratedReflection.h"

namespace pmon::util::ref::gen
{
	namespace {
		using namespace std::literals;

		template<typename T, size_t N, bool Primitive>
		std::string DumpArray_(const void* pArray)
		{
			auto& arr = *reinterpret_cast<const T(*)[N]>(pArray);
			std::ostringstream oss;
			oss << "[";
			// TODO: push this up to the static layer, simplify this helper and C# generation
			if constexpr (Primitive) {
				if constexpr (std::same_as<T, char> || std::same_as<T, unsigned char>) {
					for (size_t i = 0; i < N; i++) {
						oss << " " << (int)arr[i] << ",";
					}
				}
				else {
					for (size_t i = 0; i < N; i++) {
						oss << " " << arr[i] << ",";
					}
				}
			}
			else if constexpr (std::is_pointer_v<T>) {
				for (size_t i = 0; i < N; i++) {
					oss << " " << (arr[i] ? std::format("0x{{:016X}}", reinterpret_cast<std::uintptr_t>(arr[i])) : "null"s) << ",";
				}
			}
			else {
				if (SupportsGeneratedDump<T>()) {
					for (size_t i = 0; i < N; i++) {
						oss << " " << DumpGenerated(arr[i]) << ",";
					}
				}
				else {
					oss << " { unsupported } ";
				}
			}
			oss << "]";
			return oss.str();
		}
	}
}