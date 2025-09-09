#pragma once
#include "IdentificationTable.h"
#include <variant>
#include <vector>
#include <cereal/types/string.hpp>
#include <cereal/types/variant.hpp>

namespace pmon::util::log
{
	using MarshallPacket = std::variant<std::monostate, Entry, IdentificationTable::Bulk>;

	template<class Archive>
	void serialize(Archive& ar, IdentificationTable::Thread& e)
	{
		ar(e.tid, e.pid, e.name);
	}
	template<class Archive>
	void serialize(Archive& ar, IdentificationTable::Process& e)
	{
		ar(e.pid, e.name);
	}
	template<class Archive>
	void serialize(Archive& ar, IdentificationTable::Bulk& e)
	{
		ar(e.processes, e.threads);
	}
}