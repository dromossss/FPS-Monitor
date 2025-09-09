#include "EnumMap.h"
#include <format>
#include "Exception.h"

namespace pmapi
{
    std::shared_ptr<const EnumMap::KeyMap> EnumMap::GetKeyMap(PM_ENUM enumId)
    {
        auto& singleton = Get_();
        std::shared_lock lk{ singleton.mtx_ };
        if (!singleton.initialized_) {
            throw Exception{ "Enum lookup accessed without being initialized" };
        }
        try { return singleton.enumMap_.at(enumId); }
        catch (const std::out_of_range&) {
            throw LookupException{ std::format("Enum lookup failed to find Enum id={}", int(enumId)) };
        }
    }

    void EnumMap::Refresh(const pmapi::intro::Root& introRoot)
    {
        using ::pmon::util::str::ToWide;
        auto& singleton = Get_();
        std::lock_guard lk{ singleton.mtx_ };
        for (auto e : introRoot.GetEnums()) {
            auto pKeys = std::make_shared<KeyMap>(); auto& keys = *pKeys;
            for (auto k : e.GetKeys()) {
                keys[k.GetId()] = {
                    .narrowSymbol = k.GetSymbol(),
                    .wideSymbol = ToWide(k.GetSymbol()),
                    .narrowName = k.GetName(),
                    .wideName = ToWide(k.GetName()),
                    .narrowShortName = k.GetShortName(),
                    .wideShortName = ToWide(k.GetShortName()),
                    .narrowDescription = k.GetDescription(),
                    .wideDescription = ToWide(k.GetDescription()),
                };
            }
            singleton.enumMap_[e.GetId()] = std::move(pKeys);
        }
        singleton.initialized_ = true;
    }

    bool EnumMap::Initialized()
    {
        return Get_().initialized_.load();
    }

    EnumMap& EnumMap::Get_()
    {
        // exception if not initialized
        static EnumMap singleton;
        return singleton;
    }
}