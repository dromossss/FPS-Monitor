#pragma once
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include "StackTrace.h"

namespace pmon::util::log
{
	template<class Archive>
	void serialize(Archive& ar, StackTrace::FrameInfo& i)
	{
		ar(i.description, i.file, i.line, i.index);
	}

	struct StackTraceCereal
	{
		template<class Archive>
		static void Serialize(Archive& ar, StackTrace& trace)
		{
			ar(trace.frames_);
		}
	};

	template<class Archive>
	void serialize(Archive& ar, StackTrace& trace)
	{
		StackTraceCereal::Serialize(ar, trace);
	}
}