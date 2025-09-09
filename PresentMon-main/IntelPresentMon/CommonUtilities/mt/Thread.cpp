#include "Thread.h"
#include <format>


namespace pmon::util::mt
{
	std::string Thread::MakeThreadName_(const std::string& name, int number)
	{
		return std::format("{}-{}", name, number);
	}
}