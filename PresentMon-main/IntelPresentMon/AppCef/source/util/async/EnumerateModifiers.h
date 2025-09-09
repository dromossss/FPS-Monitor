// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#pragma once
#include "../AsyncEndpoint.h"
#include "../CefValues.h"
#include <Core/source/win/ModSet.h>

namespace p2c::client::util::async
{
    class EnumerateModifiers : public AsyncEndpoint
    {
    public:
        static constexpr std::string GetKey() { return "enumerateModifiers"; }
        EnumerateModifiers() : AsyncEndpoint{ AsyncEndpoint::Environment::RenderProcess } {}
        // {} => {mods: [{code: uint, text: string}]}
        Result ExecuteOnRenderer(uint64_t uid, CefRefPtr<CefValue> pArgObj, cef::DataBindAccessor&) const override
        {
            auto modList = win::ModSet::EnumerateMods();
            // remove the "Null" option from this list, it's stinky
            std::erase_if(modList, [](const auto& mod) {return mod.text == "Null"; });
            auto modListCef = MakeCefList(modList.size());
            for (int i = 0; i < modList.size(); i++)
            {
                const auto& mod = modList[i];
                modListCef->SetValue(i, MakeCefObject(
                    CefProp{ "code", mod.code },
                    CefProp{ "text", std::move(mod.text) }
                ));
            }
            return Result{ true, MakeCefObject(CefProp{ "mods", CefValueDecay(std::move(modListCef)) }) };
        }
    };
}