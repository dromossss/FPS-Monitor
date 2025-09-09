// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "../AsyncEndpoint.h"
#include "../CefValues.h"
#include <fstream>
#include <Core/source/infra/util/FolderResolver.h>
#include <CommonUtilities/win/WinAPI.h>
#include <CommonUtilities/Exception.h>
#include <shellapi.h>

namespace p2c::client::util::async
{
    class ExploreCaptures : public AsyncEndpoint
    {
    public:
        static constexpr std::string GetKey() { return "exploreCaptures"; }
        ExploreCaptures() : AsyncEndpoint{ AsyncEndpoint::Environment::RenderProcess } {}
        // {} => null
        Result ExecuteOnRenderer(uint64_t uid, CefRefPtr<CefValue> pArgObj, cef::DataBindAccessor&) const override
        {
            // try to resolve app folder, fallback to cwd
            std::wstring path = infra::util::FolderResolver::Get()
                .Resolve(infra::util::FolderResolver::Folder::Documents, L"Captures");

            if ((INT_PTR)ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWDEFAULT) <= 32) {
                pmlog_error("Failed to explore Captures folder");
                throw ::pmon::util::Except<::pmon::util::Exception>();
            }

            return Result{ true, CefValueNull() };
        }
    };
}