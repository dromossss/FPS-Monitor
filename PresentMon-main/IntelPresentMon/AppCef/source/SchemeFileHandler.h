// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include <CommonUtilities/win/WinAPI.h>
#include <include/cef_scheme.h>
#include <include/wrapper/cef_helpers.h>
#include <filesystem>
#include <fstream>


namespace p2c::client::cef
{
    class SchemeFileHandler : public CefResourceHandler
    {
    public:
        SchemeFileHandler(std::filesystem::path& directory);

        bool Open(
            CefRefPtr<CefRequest> request,
            bool& handle_request,
            CefRefPtr<CefCallback> callback) override;

        void GetResponseHeaders(
            CefRefPtr<CefResponse> response,
            int64_t& response_length,
            CefString& redirectUrl) override;

        void Cancel() override;

        bool Read(
            void* data_out,
            int bytes_to_read,
            int& bytes_read,
            CefRefPtr<CefResourceReadCallback> callback) override;

    private:
        std::string mime_type_;
        size_t offset_;
        std::filesystem::path directory_;
        std::filesystem::path file_path_;
        uintmax_t file_size_;
        std::ifstream file_stream_;

        IMPLEMENT_REFCOUNTING(SchemeFileHandler);
        DISALLOW_COPY_AND_ASSIGN(SchemeFileHandler);
    };
}