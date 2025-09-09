#pragma once

int CheckMetricSample(std::unique_ptr<pmapi::Session>&& pSession)
{
    try {
        auto& opt = clio::Options::Get();
        std::string metricSymbolName = *opt.metric;

        if (metricSymbolName.empty())
        {
            std::cout << "Must set PM_METRIC to check using --metric argument\n";
            std::cout << "ex. --check-metric-sample --metric PM_METRIC_PRESENTED_FPS\n";
            return -1;
        }

        // Example of how to use introspection to examine ALL metrics and determine
        // their availablity
        auto pIntrospectionRoot = pSession->GetIntrospectionRoot();
        auto metricEnums = pIntrospectionRoot->FindEnum(PM_ENUM_METRIC);
        // Loop through ALL PresentMon metrics
        for (auto metric : pIntrospectionRoot->GetMetrics())
        {
            // Look through PM_ENUM_METRIC enums to gather the metric symbol
            for (auto key : metricEnums.GetKeys())
            {
                if (key.GetId() == metric.GetId())
                {
                    if (metricSymbolName.compare(key.GetSymbol().c_str()) == 0) {
                        // Go through the device metric info to determine the metric's availability
                        auto metricInfo = metric.GetDeviceMetricInfo();
                        for (auto mi : metricInfo)
                        {
                            auto device = mi.GetDevice();
                            std::cout << std::format("Metric Id: {}, Metric Symbol: {}, Vendor Name: {}, Vendor Device Id: {}, Is Available: {}",
                                (int)metric.GetId(), metricSymbolName, device.GetName(), device.GetId(), mi.IsAvailable());
                            std::cout << std::endl;
                        }
                        return 0;
                    }
                }
            }
        }
        std::cout << "Unabled to find PM_METRIC: " << metricSymbolName << std::endl;
        return -1;
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cout << "Unknown Error" << std::endl;
        return -1;
    }
}