#pragma once
#include <iostream>


namespace p2c::cli::cons
{
	class OptionalConsoleOut
	{
	public:
		OptionalConsoleOut(bool enabled)
			:
			enabled_(enabled)
		{}
		template<typename T>
		OptionalConsoleOut& operator<<(T&& val)
		{
			if (enabled_) {
				std::cout << std::forward<T>(val);
			}
			return *this;
		}
		OptionalConsoleOut& operator<<(std::ostream& (*manip)(std::ostream&))
		{
			if (enabled_) {
				std::cout << manip;
			}
			return *this;
		}
	private:
		bool enabled_;
	};
}