#include "FrameFilterPump.h"
#include <CliCore/source/pmon/FrameDataStream.h>
#include <filesystem>
#include <ranges>

namespace p2c::cli::dat
{
	namespace rn = std::ranges;
	namespace vi = rn::views;

	FrameFilterPump::FrameFilterPump(
		std::shared_ptr<pmon::FrameDataStream> pSource,
		std::shared_ptr<FrameSink> pSink,
		const std::vector<std::string>& excludes,
		const std::vector<uint32_t>& includes,
		bool excludeDropped,
		bool ignoreCase)
		:
		pSource_{ std::move(pSource) },
		pSink_{ std::move(pSink) },
		excludes_{ excludes.begin(), excludes.end() },
		includes_{ includes.begin(), includes.end() },
		excludeDropped_{ excludeDropped },
		ignoreCase_{ ignoreCase } 
	{}
	void FrameFilterPump::AddInclude(uint32_t pid)
	{
		includes_.insert(pid);
	}
	void FrameFilterPump::Process(double timestamp)
	{
		for (auto& f : pSource_->Pull(timestamp)) {
			// skip rows for applications in the exclude list
			if (!excludes_.empty()) {
				auto appName = std::filesystem::path{ f.application }.filename().string();
				if (ignoreCase_) {
					appName = appName
						| vi::transform([](char c) {return(char)std::tolower(c); })
						| rn::to<std::basic_string>();
				}
				if (excludes_.contains(appName)) {
					continue;
				}
			}
			// skip rows for pid NOT in the include list
			if (!includes_.empty()) {
				if (!includes_.contains(f.process_id)) {
					continue;
				}
			}
			// skip dropped frames
			if (excludeDropped_) {
				if (f.dropped) {
					continue;
				}
			}
			pSink_->Process(f);
		}
	}
	uint32_t FrameFilterPump::GetPid() const
	{
		return pSource_->GetPid();
	}
}