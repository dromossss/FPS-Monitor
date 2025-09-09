// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <fstream>
#include <memory>
#include <vector>
#include <sstream>
#include "FrameSink.h"
#include <PresentMonAPI/PresentMonAPI.h>

namespace p2c::cli::dat
{
	struct GroupFlags;

	template<typename T>
	concept PresentMonOptionalStrict_ =
		std::same_as<PM_FRAME_DATA_OPT_UINT64, T> ||
		std::same_as<PM_FRAME_DATA_OPT_DOUBLE, T> ||
		std::same_as<PM_FRAME_DATA_OPT_INT, T> ||
		std::same_as<PM_FRAME_DATA_OPT_PSU_TYPE, T>;

	template<typename T>
	concept PresentMonOptional = PresentMonOptionalStrict_<std::remove_cvref_t<T>>;

	class CsvWriter : public FrameSink
	{
	public:
        CsvWriter(std::string path, const std::vector<std::string>& groups, bool writeStdout = false);
		CsvWriter(const CsvWriter&) = delete;
		CsvWriter(CsvWriter&&);
		CsvWriter& operator=(const CsvWriter&) = delete;
		CsvWriter& operator=(CsvWriter&&);
		~CsvWriter();
		void Process(const struct PM_FRAME_DATA& frame) override;
	private:
		// types
		class Buffer_
		{
		public:
			template<PresentMonOptional T>
			Buffer_& operator<<(const T& input)
			{
				if (!input.valid) {
					stream_ << "NA";
				}
				else {
					stream_ << input.data;
				}
				return *this;
			}
			template<typename T>
			Buffer_& operator<<(T&& input)
			{
				stream_ << std::forward<T>(input);
				return *this;
			}
			std::string GetString() const
			{
				return stream_.str();
			}
			void Clear()
			{
				stream_.str({});
				stream_.clear();
			}
		private:
			std::stringstream stream_;
		};
		// functions
		void Flush();
		// data
		std::unique_ptr<GroupFlags> pGroupFlags_;
		Buffer_ buffer_;
		std::ofstream file_;
		bool writeStdout_;
	};
}
