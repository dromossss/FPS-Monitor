#include "QPCUtils.h"

double QpcDeltaToSeconds(uint64_t qpcDelta, LARGE_INTEGER qpcFrequency)
{
    return static_cast<double>(qpcDelta) / qpcFrequency.QuadPart;
}

uint64_t SecondsDeltaToQpc(double secondsDelta, LARGE_INTEGER qpcFrequency)
{
    return static_cast<uint64_t>((secondsDelta * qpcFrequency.QuadPart));
}

double QpcToSeconds(uint64_t qpc, LARGE_INTEGER qpcFrequency, LARGE_INTEGER startQpc)
{
    return QpcDeltaToSeconds(qpc - startQpc.QuadPart, qpcFrequency);
}

void QpcToLocalSystemTime(uint64_t qpc, LARGE_INTEGER startQpc, LARGE_INTEGER qpcFrequency, FILETIME startTime, SYSTEMTIME* st, uint64_t* ns)
{
    auto tns = (qpc - startQpc.QuadPart) * 1000000000ull / qpcFrequency.QuadPart;
    auto t100ns = tns / 100;
    auto ft = (*reinterpret_cast<uint64_t*>(&startTime)) + t100ns;

    FileTimeToSystemTime(reinterpret_cast<FILETIME const*>(&ft), st);
    *ns = (ft - (ft / 10000000ull) * 10000000ull) * 100ull + (tns - t100ns * 100ull);
}

double QpcDeltaToMs(uint64_t qpc_data, LARGE_INTEGER qpc_frequency) {
    return 1000.0f * (static_cast<double>(qpc_data)/ qpc_frequency.QuadPart);
}