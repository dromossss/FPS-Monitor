#include "Framework.h"
#include <cassert>


namespace p2c::cli::opt::impl
{
	std::optional<CLI::App> AppBase_::app_;
	std::optional<AppBase_::CommandLine> AppBase_::line_;
	const char* AppBase_::group_ = nullptr;

	void AppBase_::InitInternal_(int argc, char** argv)
	{
		assert(!line_);
		line_.emplace(argc, argv);
		assert(!app_);
		app_.emplace();
	}

	CLI::App& AppBase_::GetApp_()
	{
		assert(app_);
		return *app_;
	}

	void AppBase_::Finalize_(const OptionStructBase_& options)
	{
		auto& app = GetApp_();
		assert(!app.parsed());

		app.description(options.GetDescription());
		app.name(options.GetName());
		try {
			app.parse(line_->argc, line_->argv);
		}
		catch (const CLI::ParseError& e) {
			app.exit(e);
			throw;
		};
	}


	Flag::Flag(std::string names, std::string description)
	{
		pOption_ = AppBase_::GetApp_().add_flag(std::move(names), value_, std::move(description));
		if (AppBase_::group_) {
			pOption_->group(AppBase_::group_);
		}
	}

	Flag::operator bool() const
	{
		return value_;
	}

	CLI::Option* Flag::opt() const { return pOption_; }

	Group::Group(const char* name)
	{
		AppBase_::group_ = name;
	}
}