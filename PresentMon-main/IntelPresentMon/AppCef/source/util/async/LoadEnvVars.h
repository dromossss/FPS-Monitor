// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "../AsyncEndpoint.h"
#include "../CliOptions.h"
#include "../CefValues.h"

namespace p2c::client::util::async
{
    class LoadEnvVars : public AsyncEndpoint
    {
    public:
        static constexpr std::string GetKey() { return "loadEnvVars"; }
        LoadEnvVars() : AsyncEndpoint{ AsyncEndpoint::Environment::RenderProcess } {}
        // {} => EnvVars
        Result ExecuteOnRenderer(uint64_t uid, CefRefPtr<CefValue> pArgObj, cef::DataBindAccessor&) const override
        {
            const auto& opt = cli::Options::Get();
            auto vars = MakeCefObject(
                CefProp{ "useDebugBlocklist", (bool)opt.filesWorking },
                CefProp{ "enableDevMode", (bool)opt.enableUiDevOptions }
            );
            return Result{ true, std::move(vars) };
        }
    };
}