#pragma once
#ifndef PRESENTMON_API2_EXPORT
#ifdef PRESENTMONAPI2_EXPORTS
#define PRESENTMON_API2_EXPORT __declspec(dllexport)
#else
#define PRESENTMON_API2_EXPORT __declspec(dllimport)
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

PRESENTMON_API2_EXPORT void pmLoaderSetPathToMiddlewareDll_(const char*);

#ifdef __cplusplus
}
#endif