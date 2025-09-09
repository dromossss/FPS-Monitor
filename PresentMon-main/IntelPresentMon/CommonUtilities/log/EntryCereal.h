#pragma once
#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/variant.hpp>
#include "Entry.h"
#include "StackTraceCereal.h"

namespace pmon::util::log
{
	template<class Archive>
	void serialize(Archive& ar, IErrorCodeResolver::Strings& s)
	{
		ar(s.type, s.description, s.name, s.symbol);
	}
	template<class Archive>
	void serialize(Archive& ar, ErrorCode& c)
	{
		ar(c.pStrings_, c.integral_);
	}
	template<class Archive>
	void serialize(Archive& ar, Entry::HeapedSourceStrings& s)
	{
		ar(s.file_, s.functionName_);
	}
	template<class Archive>
	void serialize(Archive& ar, Entry& e)
	{
		// TODO: consider rate control / hit count serialization
		ar(e.level_, e.subsystem_, e.note_, e.sourceLine_, e.timestamp_,
			e.pTrace_, e.errorCode_, e.pid_, e.tid_, e.diagnosticLayer_);

		// special handling for source strings as they might contain raw pointer alternatives
		// we always serialize as string container regardless of source
		if constexpr (Archive::is_saving::value) {
			// raw pointer sources should convert to string
			if (auto pStrings = std::get_if<Entry::StaticSourceStrings>(&e.sourceStrings_)) {
				// Convert to HeapedSourceStrings for serialization
				auto file = pStrings->file_ ? std::string(pStrings->file_) : std::string{};
				auto functionName = pStrings->functionName_ ? std::string(pStrings->functionName_) : std::string{};
				Entry::HeapedSourceStrings tempHeapedStrings{ std::move(file), std::move(functionName) };
				ar(tempHeapedStrings);
			}
			// string source needs no extra work
			else if (auto pStrings = std::get_if<Entry::HeapedSourceStrings>(&e.sourceStrings_)) {
				ar(*pStrings);
			}
			// we should never get here, but if we do just serialize empty string source
			else {
				Entry::HeapedSourceStrings tempHeapedStrings{};
				ar(tempHeapedStrings);
			}
		}
		// we always deserialize as string container
		else {
			Entry::HeapedSourceStrings strings;
			ar(strings);
			e.sourceStrings_ = std::move(strings);
		}
	}
}