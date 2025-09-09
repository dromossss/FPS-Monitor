#pragma once
#include <CLI/CLI.hpp>
#include <optional>
#include <stdexcept>
#include <type_traits>

namespace p2c::cli::opt::impl
{
	struct OptionStructBase_
	{
	protected:
		friend class AppBase_;
		virtual std::string GetDescription() const = 0;
		virtual std::string GetName() const = 0;
	};

	class AppBase_
	{
		template<typename T>
		friend class Option;
		friend class Flag;
		friend class Group;
	protected:
		AppBase_() = default;
		static void InitInternal_(int argc, char** argv);
		static CLI::App& GetApp_();
		static void Finalize_(const OptionStructBase_&);
	private:
		// types
		struct CommandLine
		{
			int argc;
			char** argv;
		};
		// data
		static std::optional<CLI::App> app_;
		static std::optional<CommandLine> line_;
		static const char* group_;
	};

	template<class T> concept OptionStructable = std::derived_from<T, OptionStructBase_>;

	template<OptionStructable OptionStruct>
	class App : public AppBase_
	{
	public:
		static void Init(int argc, char** argv)
		{
			InitInternal_(argc, argv);
			GetOptions();
		}
		static OptionStruct& GetOptions()
		{
			static App singleton;
			return singleton.options_;
		}
		static bool CheckOptionPresence(const std::string& key)
		{
			const auto pOption = GetApp_()[key];
			return pOption && pOption->count() > 0;
		}
	private:
		App() { Finalize_(options_); }
		OptionStruct options_;
	};

	class Group
	{
	public:
		Group(const char* name);
	private:
		OptionStructBase_* optionStruct_ = nullptr;
	};

	template<typename T>
	class Option
	{
	public:
		template<class U>
		Option(std::string names, std::string description, U&& customizer)
		{
			pOption_ = AppBase_::GetApp_().add_option(std::move(names), value_, std::move(description));
			if (AppBase_::group_) {
				pOption_->group(AppBase_::group_);
			}
			// if the customizer returns a T, it is the default value
			using R = std::invoke_result_t<U, CLI::Option*>;
			if constexpr (std::is_same_v<R, T>) {
				value_ = customizer(pOption_);
			}
			else {
				customizer(pOption_);
			}
		}
		Option(std::string names, std::string description)
		{
			pOption_ = AppBase_::GetApp_().add_option(std::move(names), value_, std::move(description));
			if (AppBase_::group_) {
				pOption_->group(AppBase_::group_);
			}
		}
		operator bool() const
		{
			return (bool)*pOption_;
		}
		bool operator!() const
		{
			return !bool(*this);
		}
		const T& operator*() const
		{
			return value_;
		}
		CLI::Option* opt() const { return pOption_; }
	private:
		T value_{};
		CLI::Option* pOption_ = nullptr;
	};

	class Flag
	{
	public:
		template<class U>
		Flag(std::string names, std::string description, U&& customizer)
		{
			pOption_ = AppBase_::GetApp_().add_flag(std::move(names), value_, std::move(description));
			if (AppBase_::group_) {
				pOption_->group(AppBase_::group_);
			}
			// if the customizer returns a bool, it is the default value
			using R = std::invoke_result_t<U, CLI::Option*>;
			if constexpr (std::is_same_v<R, bool>) {
				value_ = customizer(pOption_);
			}
			else {
				customizer(pOption_);
			}
		}
		Flag(std::string names, std::string description);
		operator bool() const;
		CLI::Option* opt() const;
	private:
		// data
		bool value_ = false;
		CLI::Option* pOption_ = nullptr;
	};
}