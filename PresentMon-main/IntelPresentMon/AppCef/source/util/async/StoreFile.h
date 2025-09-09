// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "../AsyncEndpoint.h"
#include "../CefValues.h"
#include "../PathSanitaryCheck.h"
#include <fstream>

namespace p2c::client::util::async
{
    class StoreFile : public AsyncEndpoint
    {
    public:
        static constexpr std::string GetKey() { return "storeFile"; }
        StoreFile() : AsyncEndpoint{ AsyncEndpoint::Environment::RenderProcess } {}
        // {payload: string, location: int, path: string} => null
        Result ExecuteOnRenderer(uint64_t uid, CefRefPtr<CefValue> pArgObj, cef::DataBindAccessor&) const override
        {
            const auto filePath = ResolveSanitizedPath(
                Traverse(pArgObj)["location"],
                Traverse(pArgObj)["path"].AsWString());
            // open file (over)write with payload
            if (std::wofstream file{ filePath, std::ios::trunc }) {
                file << Traverse(pArgObj)["payload"].AsWString();
                return Result{ true, CefValueNull() };
            }
            // if opening failed, that's an error
            auto s = std::format("Unable to open (for writing) file path: {}", filePath.string());
            pmlog_error(s);
            throw std::runtime_error{ s };
        }
    };
}