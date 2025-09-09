#pragma once
#include <filesystem>
#include <ostream>
#include <memory>

namespace pmon::util::log
{
	class IFileStrategy
	{
	public:
		// get stream for writting (forces deferred file creation to resolve)
		virtual std::shared_ptr<std::ostream> AddLine() = 0;
		// get stream for other purposes (returns empty pointer if file not created yet)
		virtual std::shared_ptr<std::ostream> GetFileStream() const = 0;
		virtual void Cleanup() = 0;
	};
}

