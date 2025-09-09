#pragma once

#include <Windows.h>
#include <stdint.h>

double QpcDeltaToSeconds(uint64_t qpcDelta, LARGE_INTEGER qpcFrequency);
uint64_t SecondsDeltaToQpc(double secondsDelta, LARGE_INTEGER qpcFrequency);
double QpcToSeconds(uint64_t qpc, LARGE_INTEGER qpcFrequency, LARGE_INTEGER startQpc);
void QpcToLocalSystemTime(
    uint64_t qpc,
    LARGE_INTEGER startQpc,
    LARGE_INTEGER qpcFrequency,
    FILETIME startTime, 
    SYSTEMTIME* st,
    uint64_t* ns);
double QpcDeltaToMs(uint64_t qpc_data, LARGE_INTEGER qpc_frequency);