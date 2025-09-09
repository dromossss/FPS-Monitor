#include "gtest/gtest.h"
#include "../ControlLib/TelemetryHistory.h"
#include <algorithm>
#include <iterator>
#include <ranges>
#include <functional>

TEST(TelemetryHistory, iterationEmpty)
{
    pwr::TelemetryHistory<PresentMonPowerTelemetryInfo> hist(5);
    EXPECT_EQ(0, hist.end() - hist.begin());
}

TEST(TelemetryHistory, iterationPartial)
{
    pwr::TelemetryHistory<PresentMonPowerTelemetryInfo> hist(5);
    hist.Push({ .qpc = 10 });
    hist.Push({ .qpc = 20 });
    hist.Push({ .qpc = 30 });
    EXPECT_EQ(3, hist.end() - hist.begin());
    EXPECT_EQ(10, (*(hist.begin() + 0)).qpc);
    EXPECT_EQ(20, (hist.begin() += 1)->qpc);
    EXPECT_EQ(30, hist.begin()[2].qpc);
}

TEST(TelemetryHistory, iterationFull)
{
    pwr::TelemetryHistory<PresentMonPowerTelemetryInfo> hist(5);
    hist.Push({ .qpc = 10 });
    hist.Push({ .qpc = 20 });
    hist.Push({ .qpc = 30 });
    hist.Push({ .qpc = 40 });
    hist.Push({ .qpc = 50 });
    EXPECT_EQ(5, hist.end() - hist.begin());

    std::vector<uint64_t> plucked;
    std::transform(hist.begin(), hist.end(), std::back_inserter(plucked),
        std::mem_fn(&PresentMonPowerTelemetryInfo::qpc));

    std::vector<uint64_t> expected{ 10, 20, 30, 40, 50 };
    EXPECT_TRUE(plucked == expected);
}

TEST(TelemetryHistory, iterationExcursioned)
{
    pwr::TelemetryHistory<PresentMonPowerTelemetryInfo> hist(5);
    hist.Push({ .qpc = 10 });
    hist.Push({ .qpc = 20 });
    hist.Push({ .qpc = 30 });
    hist.Push({ .qpc = 40 });
    hist.Push({ .qpc = 50 });
    hist.Push({ .qpc = 60 });
    hist.Push({ .qpc = 70 });
    EXPECT_EQ(5, hist.end() - hist.begin());

    std::vector<uint64_t> plucked;
    std::transform(hist.begin(), hist.end(), std::back_inserter(plucked),
        std::mem_fn(&PresentMonPowerTelemetryInfo::qpc));

    std::vector<uint64_t> expected{ 30, 40, 50, 60, 70 };
    EXPECT_TRUE(plucked == expected);
}

TEST(TelemetryHistory, nearestInside)
{
    pwr::TelemetryHistory<PresentMonPowerTelemetryInfo> hist(5);
    hist.Push({ .qpc = 10 });
    hist.Push({ .qpc = 20 });
    hist.Push({ .qpc = 30 });
    hist.Push({ .qpc = 40 });
    hist.Push({ .qpc = 50 });
    hist.Push({ .qpc = 60 });
    hist.Push({ .qpc = 70 });

    const auto nearest = hist.GetNearest(50);
    EXPECT_TRUE(bool(nearest));
    EXPECT_EQ(50, nearest->qpc);
}

TEST(TelemetryHistory, nearestCloseLower)
{
    pwr::TelemetryHistory<PresentMonPowerTelemetryInfo> hist(5);
    hist.Push({ .qpc = 10 });
    hist.Push({ .qpc = 20 });
    hist.Push({ .qpc = 30 });
    hist.Push({ .qpc = 40 });
    hist.Push({ .qpc = 50 });
    hist.Push({ .qpc = 60 });
    hist.Push({ .qpc = 70 });

    const auto nearest = hist.GetNearest(33);
    EXPECT_TRUE(bool(nearest));
    EXPECT_EQ(30, nearest->qpc);
}

TEST(TelemetryHistory, nearestCloseHigher)
{
    pwr::TelemetryHistory<PresentMonPowerTelemetryInfo> hist(5);
    hist.Push({ .qpc = 10 });
    hist.Push({ .qpc = 20 });
    hist.Push({ .qpc = 30 });
    hist.Push({ .qpc = 40 });
    hist.Push({ .qpc = 50 });
    hist.Push({ .qpc = 60 });
    hist.Push({ .qpc = 70 });

    const auto nearest = hist.GetNearest(58);
    EXPECT_TRUE(bool(nearest));
    EXPECT_EQ(60, nearest->qpc);
}