#include "DynamicPollingFetcher.h"
#include <PresentMonAPIWrapper/PresentMonAPIWrapper.h>
#include <CommonUtilities//str/String.h>
#include <ranges>

namespace p2c::pmon::met
{
    using ::pmon::util::str::ToWide;


    DynamicPollingFetcher::DynamicPollingFetcher(const PM_QUERY_ELEMENT& qel, const pmapi::intro::Root& introRoot,
        std::shared_ptr<DynamicQuery> pQuery)
        :
        pQuery_{ std::move(pQuery) },
        offset_{ (uint32_t)qel.dataOffset }
    {
        // overlay will always indicate preferred unit in the widget labels
        // so we must scale from output unit if necessary to match
        const auto metric = introRoot.FindMetric(qel.metric);
        const auto type = metric.GetDataTypeInfo().GetPolledType();
        if (metric.GetUnit() != metric.GetPreferredUnitHint()) {
            scale_ = (float)introRoot.FindUnit(metric.GetUnit())
                .MakeConversionFactor(metric.GetPreferredUnitHint());
        }
    }

    TypedDynamicPollingFetcher<PM_ENUM>::TypedDynamicPollingFetcher(const PM_QUERY_ELEMENT& qel, const pmapi::intro::Root& introRoot,
        std::shared_ptr<DynamicQuery> pQuery, std::shared_ptr<const pmapi::EnumMap::KeyMap> pKeyMap)
        :
        DynamicPollingFetcher{ qel, introRoot, std::move(pQuery) },
        pKeyMap_{ std::move(pKeyMap) }
    {}
    std::wstring TypedDynamicPollingFetcher<PM_ENUM>::ReadStringValue()
    {
        if (auto pBlobBytes = pQuery_->GetBlobData()) {
            return pKeyMap_->at(*reinterpret_cast<const int*>(&pBlobBytes[offset_])).wideName;
        }
        return {};
    }

    std::optional<float> TypedDynamicPollingFetcher<PM_ENUM>::ReadValue()
    {
        return {};
    }

    std::shared_ptr<DynamicPollingFetcher> MakeDynamicPollingFetcher(const PM_QUERY_ELEMENT& qel,
        const pmapi::intro::Root& introRoot, std::shared_ptr<DynamicQuery> pQuery)
    {
        const auto dataTypeInfo = introRoot.FindMetric(qel.metric).GetDataTypeInfo();
        switch (dataTypeInfo.GetPolledType()) {
        case PM_DATA_TYPE_BOOL:
            return std::make_shared<TypedDynamicPollingFetcher<bool>>(qel, introRoot, std::move(pQuery));
        case PM_DATA_TYPE_INT32:
            return std::make_unique<TypedDynamicPollingFetcher<int32_t>>(qel, introRoot, std::move(pQuery));
        case PM_DATA_TYPE_UINT32:
            return std::make_unique<TypedDynamicPollingFetcher<uint32_t>>(qel, introRoot, std::move(pQuery));
        case PM_DATA_TYPE_UINT64:
            return std::make_unique<TypedDynamicPollingFetcher<uint64_t>>(qel, introRoot, std::move(pQuery));
        case PM_DATA_TYPE_DOUBLE:
            return std::make_unique<TypedDynamicPollingFetcher<double>>(qel, introRoot, std::move(pQuery));
        case PM_DATA_TYPE_STRING:
            return std::make_unique<TypedDynamicPollingFetcher<const char*>>(qel, introRoot, std::move(pQuery));
        case PM_DATA_TYPE_ENUM:
            return std::make_unique<TypedDynamicPollingFetcher<PM_ENUM>>(qel, introRoot, std::move(pQuery),
                pmapi::EnumMap::GetKeyMap(dataTypeInfo.GetEnumId()));
        }
        // TODO: maybe throw exception here?
        return {};
    }
}