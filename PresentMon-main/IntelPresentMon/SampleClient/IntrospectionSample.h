#pragma once

int IntrospectionSample(std::unique_ptr<pmapi::Session>&& pSession)
{
    // Example of how to use introspection to examine ALL metrics and determine
    // their availablity
    auto pIntrospectionRoot = pSession->GetIntrospectionRoot();
    auto metricEnums = pIntrospectionRoot->FindEnum(PM_ENUM_METRIC);

    // Loop through ALL PresentMon metrics
    for (auto metric : pIntrospectionRoot->GetMetrics())
    {
        std::string metricSymbol;
        // Look through PM_ENUM_METRIC enums to gather the metric symbol
        for (auto key : metricEnums.GetKeys())
        {
            if (key.GetId() == metric.GetId())
            {
                metricSymbol = key.GetSymbol();
                break;
            }
        }

        // Go through the device metric info to determine the metric's availability
        auto metricInfo = metric.GetDeviceMetricInfo();
        for (auto mi : metricInfo)
        {
            auto device = mi.GetDevice();
            std::cout << std::format("Metric Id: {}, Metric Symbol: {}, Vendor Name: {}, Vendor Device Id: {}, Is Available: {}",
                (int)metric.GetId(), metricSymbol, device.GetName(), device.GetId(), mi.IsAvailable());
            std::cout << std::endl;
        }
    }

    return 0;
}
