// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "CsvWriter.h"
#include <CliCore/source/pmon/PresentMode.h>
#include <Core/source/infra/util/Util.h>
#include <iostream>
#include <set>
#include "Columns.h"
#include "ColumnGroups.h"


namespace p2c::cli::dat
{
    struct GroupFlags
    {
        bool core = true;
#define X_(name, description) bool name;
        COLUMN_GROUP_LIST
#undef X_
    };

    CsvWriter::~CsvWriter() = default;
    CsvWriter::CsvWriter(CsvWriter&&) = default;
    CsvWriter& CsvWriter::operator=(CsvWriter&&) = default;

    CsvWriter::CsvWriter(std::string path, const std::vector<std::string>& groups, bool writeStdout)
        :
        writeStdout_{ writeStdout }
    {
        if (!path.empty()) {
            file_.open(path, std::ios::trunc);
            if (!file_) {
                throw std::runtime_error{ "Failed to open file for writing: " + path };
            }
        }

        // setup group flags
        std::set<std::string> groupSet{ groups.begin(), groups.end() };
        pGroupFlags_ = std::make_unique<GroupFlags>();
#define X_(name, description) pGroupFlags_->name = groupSet.contains("all") || groupSet.contains(#name);
        COLUMN_GROUP_LIST
#undef X_

        int col = 0;
        // write header
#define X_(name, unit, symbol, transform, index, group) if (pGroupFlags_->group) { if (col++) buffer_ << ',';  buffer_ << name; }
        COLUMN_LIST
#undef X_
        buffer_ << "\n";

        Flush();
    }

    void CsvWriter::Process(const PM_FRAME_DATA& frame)
    {
        const auto TransformPresentMode = [](PM_PRESENT_MODE mode) {
            return infra::util::ToNarrow(pmon::PresentModeToString(pmon::ConvertPresentMode(mode)));
        };

        int col = 0;
#define X_(name, unit, symbol, transform, index, group) if (pGroupFlags_->group) { if (col++) buffer_ << ','; buffer_ << transform(frame.symbol index); }
        COLUMN_LIST
#undef X_
        buffer_ << "\n";

        Flush();
    }

    void CsvWriter::Flush()
    {
        const auto string = buffer_.GetString();
        if (writeStdout_) {
            std::cout << string;
        }
        if (file_) {
            file_ << string;
        }
        buffer_.Clear();
    }
}