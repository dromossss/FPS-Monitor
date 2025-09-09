// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include "SchemeFileHandler.h"
#include <include/cef_parser.h>
#include "util/Logging.h"


namespace p2c::client::cef
{
    SchemeFileHandler::SchemeFileHandler(std::filesystem::path& directory)
        :
        offset_(0),
        directory_(directory),
        file_size_(0)
    {}

    // Open the response stream
    //
    // | handle_request | result | description                                                       |
    // -----------------------------------------------------------------------------------------------
    // | true           | true   | Handle the request immediately                                    |
    // | false          | true   | Decide at a later time. Execute callback to continue or cancel    |
    // | true           | false  | Cancel the request immediately                                    |
    // | false          | false  | backwards compatibility. the ProcessRequest method will be called |
    bool SchemeFileHandler::Open(CefRefPtr<CefRequest> request,
        bool& handle_request,
        CefRefPtr<CefCallback> callback)
    {
        handle_request = true;
        CefURLParts url_parts;
        if (!CefParseURL(request->GetURL(), url_parts))
        {
            pmlog_error(std::format("Failed parsing URL: {}", request->GetURL().ToString()));
            return false;
        }

        std::filesystem::path path_part(CefString(&url_parts.path).ToWString());
        path_part = path_part.relative_path();

        file_path_ = directory_ / path_part;
        if (!std::filesystem::is_regular_file(file_path_))
        {
            pmlog_error(std::format("Failed locating file: {}", file_path_.string()));
            return false;
        }
        file_size_ = std::filesystem::file_size(file_path_);
        std::string extension = file_path_.extension().string();
        extension.erase(std::remove(extension.begin(), extension.end(), '.'), extension.end());
        if (!extension.empty())
        {
            mime_type_ = CefGetMimeType(extension);
        }

        file_stream_ = std::ifstream(file_path_, std::ios::binary);
        if (!file_stream_)
        {
            pmlog_error(std::format("Failed opening file: {}", file_path_.string()));
            return false;
        }

        pmlog_dbg(std::format("Opening file: {}", file_path_.string()));
        return true;
    }

    void SchemeFileHandler::GetResponseHeaders(CefRefPtr<CefResponse> response,
        int64_t& response_length,
        CefString& redirectUrl)
    {
        if (!mime_type_.empty())
        {
            response->SetMimeType(mime_type_);
        }
        response->SetStatus(200);

        // Set the resulting response length
        response_length = static_cast<int64_t>(file_size_);

        if (response_length < 0)
        {
            pmlog_error(std::format("File too large: {}", file_size_));
        }
    }

    void SchemeFileHandler::Cancel()
    {
    }

    // Read response data.
    //
    // If data is available immediately:
    //    * copy up to |bytes_to_read| bytes into |data_out|,
    //    * set |bytes_read| to the number of bytes copied,
    //    * return true.
    //
    // To read the data at a later time:
    //    * keep a pointer to |data_out|,
    //    * set |bytes_read| to 0,
    //    * return true,
    //    * execute |callback| when the data is available
    //    * (|data_out| will remain valid until the callback is executed).
    //
    // To indicate response completion:
    //    * set |bytes_read| to 0,
    //    * return false.
    //
    // To indicate failure:
    //    * set |bytes_read| to < 0 (e.g. -2 for ERR_FAILED),
    //    * return false.
    //
    // To call legacy ReadResponse method:
    //    * |bytes_read| to -1,
    //    * return false.
    bool SchemeFileHandler::Read(void* data_out,
        int bytes_to_read,
        int& bytes_read,
        CefRefPtr<CefResourceReadCallback> callback)
    {
        if (!data_out)
        {
            bytes_read = -2;
            return false;
        }

        bytes_read = 0;

        if (offset_ < file_size_)
        {
            int transfer_size = std::min(bytes_to_read, static_cast<int>(file_size_ - offset_));
            file_stream_.read(static_cast<char*>(data_out), transfer_size);
            offset_ += transfer_size;

            bytes_read = transfer_size;
            return true;
        }

        return false;
    }
}
