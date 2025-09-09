#pragma once
#include "IFileStrategy.h"
#include <fstream>

namespace pmon::util::log
{
	class SimpleFileStrategy : public IFileStrategy
	{
	public:
		SimpleFileStrategy(std::filesystem::path path);
		std::shared_ptr<std::ostream> AddLine() override;
		std::shared_ptr<std::ostream> GetFileStream() const override;
		void Cleanup() override;
	private:
		// functions
		void CreateFile_();
		// data
		std::filesystem::path path_;
		std::shared_ptr<std::ofstream> pFileStream_;
	};
}

