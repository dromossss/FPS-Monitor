#include "LineTable.h"
#include <fstream>
#include <regex>
#include "../str/String.h"
#include "Log.h"

namespace pmon::util::log
{
	LineTable::Entry LineTable::dummyEntry_ = {};

	LineTable::Entry* LineTable::TryLookup(const std::string& file, int line) noexcept
	{
		try {
			return Get_().TryLookup_(file, line);
		}
		catch (...) { return nullptr; }
	}
	LineTable::Entry& LineTable::Lookup(const std::string& file, int line) noexcept
	{
		try {
			return Get_().Lookup_(file, line);
		}
		catch (...) { return dummyEntry_; }
	}
	LineTable::ListMode LineTable::GetListMode() noexcept
	{
		try {
			return Get_().listMode_;
		}
		catch (...) { return ListMode::None; }
	}
	void LineTable::SetListMode(ListMode mode) noexcept
	{
		try {
			Get_().listMode_ = mode;
		}
		catch (...) {}
	}
	bool LineTable::GetTraceOverride() noexcept
	{
		try {
			return Get_().traceOverride_;
		}
		catch (...) { return false; }
	}
	void LineTable::SetTraceOverride(bool ov) noexcept
	{
		try {
			Get_().traceOverride_ = ov;
		}
		catch (...) {}
	}
	void LineTable::RegisterListItem(const std::string& file, int line, TraceOverride traceOverride) noexcept
	{
		try {
			Get_().RegisterListItem_(file, line, traceOverride);
		}
		catch (...) {}
	}

	bool LineTable::IngestList_(const std::string& path, bool isBlacklist)
	{
		TraceOverride tover = TraceOverride::None;
		if (std::ifstream listFile{ path }) {
			std::string line;
			bool hasNontraceLine = false;
			while (std::getline(listFile, line)) {
				line = str::TrimWhitespace(line);
				if (line.empty()) {
					continue;
				}
				// detect marker for trace control line and remove
				if (line.back() == L'%') {
					tover = TraceOverride::ForceOff;
				}
				else if (line.back() == L'$') {
					tover = TraceOverride::ForceOn;
				}
				if (tover != TraceOverride::None) {
					line.pop_back();
				}
				// check if pattern matches line identifier
				std::regex patternLogged(R"((.+)\((\d+)\)$)");
				std::smatch matches;
				// try to match a line in the log line format (42)
				if (std::regex_search(line, matches, patternLogged)) {
					if (matches.size() == 3) {  // matches[0] is the whole string, [1] is the path, [2] is the number
						RegisterListItem_(matches[1], std::stoi(matches[2]), tover);
						if (tover == TraceOverride::None) {
							hasNontraceLine = true;
						}
					}
				}
				// set global flag to check for allow/deny rules for every log line
				if (hasNontraceLine) {
					listMode_ = isBlacklist ? ListMode::Black : ListMode::White;
				}
			}
			return tover != TraceOverride::None;
		}
		else {
			pmlog_error();
			throw std::runtime_error{ "Failed to open listfile {}" };
		}
	}
		
	bool LineTable::IngestList(const std::string& path, bool isBlacklist)
	{
		return Get_().IngestList_(path, isBlacklist);
	}

	LineTable& LineTable::Get_()
	{
		// @SINGLETON
		static LineTable table;
		return table;
	}
	LineTable::Entry* LineTable::TryLookup_(const std::string& file, int line)
	{
		std::shared_lock lk{ mtx_ };
		if (auto i = lines_.find(MakeKey_(file, line)); i != lines_.end()) {
			return &i->second;
		}
		return nullptr;
	}
	LineTable::Entry& LineTable::Lookup_(const std::string& file, int line)
	{
		std::shared_lock lk{ mtx_ };
		return lines_[MakeKey_(file, line)];
	}
	void LineTable::RegisterListItem_(const std::string& file, int line, TraceOverride traceOverride)
	{
		RegisterListItem_(MakeKey_(file, line), traceOverride);
	}
	bool LineTable::GetTraceOverride_() const noexcept
	{
		return traceOverride_;
	}
	void LineTable::SetTraceOverride_(bool on) noexcept
	{
		traceOverride_ = on;
	}
	LineTable::ListMode LineTable::GetListMode_() const noexcept
	{
		return listMode_;
	}
	void LineTable::SetListMode_(ListMode mode) noexcept
	{
		listMode_ = mode;
	}
	void LineTable::RegisterListItem_(const std::string& key, TraceOverride traceOverride)
	{
		std::lock_guard lk{ mtx_ };
		auto& e = lines_[key];
		if (traceOverride != TraceOverride::None) {
			e.traceOverride_ = traceOverride;
			traceOverride_ = true;
		}
		else {
			e.isListed_ = true;
		}
	}
	std::string LineTable::MakeKey_(const std::string& file, int line)
	{
		return file + "?" + std::to_string(line);
	}
}