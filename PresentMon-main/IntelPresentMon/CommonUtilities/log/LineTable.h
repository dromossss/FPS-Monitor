#pragma once
#include <chrono>
#include <atomic>
#include <shared_mutex>
#include <unordered_map>

namespace pmon::util::log
{
	class LineTable
	{
	public:
		enum class ListMode
		{
			None,
			Black,
			White,
		};
		enum class TraceOverride
		{
			None,
			ForceOn,
			ForceOff,
		};
		struct Entry
		{
			uint32_t NextHit()
			{
				return ++hitCount_;
			}
			uint32_t PeekHit() const
			{
				return hitCount_;
			}
			std::atomic<uint32_t> hitCount_ = 0;
			bool isListed_ = false;
			TraceOverride traceOverride_ = TraceOverride::None;
		};
		static Entry* TryLookup(const std::string& file, int line) noexcept;
		static Entry& Lookup(const std::string& file, int line) noexcept;
		// blacklist, whitelist, or none
		static ListMode GetListMode() noexcept;
		static void SetListMode(ListMode mode) noexcept;
		// dictates whether line lookups necessary to check for per-line trace overrides
		static bool GetTraceOverride() noexcept;
		static void SetTraceOverride(bool) noexcept;
		static void RegisterListItem(const std::string& file, int line, TraceOverride traceOverride) noexcept;
		// returns true if there were any trace override lines, sets global trace and list settings based on contents
		static bool IngestList(const std::string& path, bool isBlacklist);

		// implementation functions
		static LineTable& Get_();
		LineTable::Entry* TryLookup_(const std::string& file, int line);
		Entry& Lookup_(const std::string& file, int line);
		void RegisterListItem_(const std::string& file, int line, TraceOverride traceOverride);
		bool GetTraceOverride_() const noexcept;
		void SetTraceOverride_(bool) noexcept;
		ListMode GetListMode_() const noexcept;
		void SetListMode_(ListMode mode) noexcept;
		bool IngestList_(const std::string& path, bool isBlacklist);
	private:
		// functions
		void RegisterListItem_(const std::string& key, TraceOverride traceOverride);
		static std::string MakeKey_(const std::string& file, int line);
		// data
		mutable std::shared_mutex mtx_;
		std::unordered_map<std::string, Entry> lines_;
		std::atomic<ListMode> listMode_ = ListMode::None;
		std::atomic<bool> traceOverride_ = false;
		static Entry dummyEntry_;
	};
}