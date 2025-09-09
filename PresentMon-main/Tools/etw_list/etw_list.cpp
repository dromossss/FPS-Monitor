// Copyright (C) 2017-2024 Intel Corporation
// SPDX-License-Identifier: MIT

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <windows.h>
#include <tdh.h> // Must include after windows.h

#include <generated/version.h>

#include "../../presentdata/etw/microsoft_windows_eventmetadata.h"


// ----------------------------------------------------------------------------
// Helper functions

void usage()
{
    fprintf(stderr,
        "usage: etw_list.exe [options]\n"
        "options:\n"
        "    --etl=path           List information from an ETL file instead of the local system.\n"
        "    --provider=filter    List providers that match the filter, argument can be used more than once.\n"
        "                         filter can be a provider name or guid, and can include up to one '*'.\n"
        "    --sort=guid|name     Sort providers by specified element.\n"
        "    --event=filter       List events that match the filter, argument can be used more than once.\n"
        "                         filter is of the form Task::opcode, and can include up to one '*'.\n"
        "    --no_events          Don't print event information.\n"
        "    --no_event_structs   Don't print event structures.\n"
        "    --no_prop_enums      Don't print event property enums.\n"
        "    --no_keywords        Don't print keywords.\n"
        "    --no_levels          Don't print levels.\n"
        "    --no_channels        Don't print channels.\n"
        "build: %s\n", PRESENT_MON_VERSION);
}

struct Filter
{
    std::wstring part1_;
    std::wstring part2_;
    bool wildcard_;
    explicit Filter(wchar_t const* filter)
        : part1_(filter)
    {
        auto p = wcschr(filter, L'*');
        wildcard_ = p != nullptr;
        if (p != nullptr) {
            part1_.resize((uintptr_t) p - (uintptr_t) filter);
            part2_ = p + 1;
        }
    }
    bool Matches(wchar_t const* s) const
    {
        if (wildcard_) {
            auto s1 = s;
            auto s2 = s + wcslen(s) - part2_.size();
            return _wcsnicmp(s1, part1_.c_str(), part1_.size()) == 0 && _wcsicmp(s2, part2_.c_str()) == 0;
        }
        return _wcsicmp(s, part1_.c_str()) == 0;
    }
};

// Trace information (e.g., TRACE_PROVIDER_INFO) is provided in memory blocks
// where string members are specified as an offset from the base of the
// allocation.
//
// NOTE: in practice, some fields have tailing spaces (in particular
// Event::opcodeName_ and Event::layerName_ are typical) so we strip those here
// too.
wchar_t const* GetStringPtr(void* base, ULONG offset)
{
    wchar_t* s = nullptr;
    if (offset > 0) {
        s = (wchar_t*) ((uintptr_t) base + offset);
        for (auto n = wcslen(s); n-- && s[n] == L' '; ) {
            s[n] = '\0';
        }
    }
    return s;
}


// ----------------------------------------------------------------------------
// Providers

struct Provider {
    GUID guid_;
    std::wstring guidStr_;
    std::wstring name_;
    bool manifest_;

    Provider() {}
    Provider(PROVIDER_ENUMERATION_INFO* enumInfo, TRACE_PROVIDER_INFO const& info)
        : name_(GetStringPtr(enumInfo, info.ProviderNameOffset))
        , manifest_(info.SchemaSource == 0)
    {
        SetGUID(info.ProviderGuid);
    }

    void SetGUID(GUID const& guid)
    {
        guid_ = guid;
        guidStr_.clear();

        wchar_t* guidStr = nullptr;
        if (StringFromIID(guid, &guidStr) == S_OK) {
            guidStr_ = guidStr;
            CoTaskMemFree(guidStr);
        }
    }

    bool Matches(std::vector<Filter>* filters) const
    {
        for (auto ii = filters->begin(), ie = filters->end(); ii != ie; ++ii) {
            auto const& filter = *ii;
            if (filter.Matches(name_.c_str()) || filter.Matches(guidStr_.c_str())) {
                // Remove filter if we found an exact match
                if (!filter.wildcard_) {
                    filters->erase(ii);
                }
                return true;
            }
        }
        return false;
    }
};

void EnumerateSystemProviders(
    std::vector<Filter>* providerIds,
    std::vector<Provider>* providers)
{
    // Enumerate all providers on the system
    ULONG bufferSize = 0;
    auto status = TdhEnumerateProviders(nullptr, &bufferSize);
    if (status != ERROR_INSUFFICIENT_BUFFER) {
        fprintf(stderr, "error: could not enumerate providers (error=%u).\n", status);
        exit(1);
    }

    auto enumInfo = (PROVIDER_ENUMERATION_INFO*) malloc(bufferSize);
    if (enumInfo == nullptr) {
        fprintf(stderr, "error: could not allocate memory for providers (%u bytes).\n", bufferSize);
        exit(1);
    }

    status = TdhEnumerateProviders(enumInfo, &bufferSize);
    if (status != ERROR_SUCCESS) {
        fprintf(stderr, "error: could not enumerate providers (error=%u).\n", status);
        free(enumInfo);
        exit(1);
    }

    auto providerCount = enumInfo->NumberOfProviders;
    providers->reserve(providerCount);
    for (ULONG i = 0; i < providerCount; ++i) {
        Provider provider(enumInfo, enumInfo->TraceProviderInfoArray[i]);
        if (provider.Matches(providerIds)) {
            providers->emplace_back(provider);
        }
    }

    free(enumInfo);
}


// ----------------------------------------------------------------------------
// Events

struct EventProperty : public EVENT_PROPERTY_INFO {
    std::wstring name_;
    std::wstring lengthName_;
    std::wstring countName_;
    std::wstring mapName_;
    std::vector<EventProperty> members_;

    EventProperty(TRACE_EVENT_INFO* eventInfo, EVENT_PROPERTY_INFO const& propInfo)
        : EVENT_PROPERTY_INFO(propInfo)
        , name_(GetStringPtr(eventInfo, propInfo.NameOffset))
    {
        if (propInfo.Flags & PropertyStruct) {
            auto propCount = propInfo.structType.NumOfStructMembers;
            members_.reserve(propCount);
            for (ULONG i = 0; i < propCount; ++i) {
                members_.emplace_back(eventInfo, eventInfo->EventPropertyInfoArray[propInfo.structType.StructStartIndex + i]);
            }
        } else {
            if (propInfo.nonStructType.MapNameOffset != 0) {
                mapName_ = GetStringPtr(eventInfo, propInfo.nonStructType.MapNameOffset);
            }
        }
        if (propInfo.Flags & PropertyParamLength) {
            lengthName_ = GetStringPtr(eventInfo, eventInfo->EventPropertyInfoArray[propInfo.lengthPropertyIndex].NameOffset);
        }
        if (propInfo.Flags & PropertyParamCount) {
            countName_ = GetStringPtr(eventInfo, eventInfo->EventPropertyInfoArray[propInfo.countPropertyIndex].NameOffset);
        }
    }
};

bool HasPointer(EventProperty const& prop);

bool HasPointer(std::vector<EventProperty> const& members)
{
    for (auto const& prop : members) {
        if (HasPointer(prop)) {
            return true;
        }
    }
    return false;
}

bool HasPointer(EventProperty const& prop)
{
    if (prop.Flags & PropertyStruct) {
        return HasPointer(prop.members_);
    }

    return prop.nonStructType.InType == TDH_INTYPE_POINTER;
}

struct Event : public EVENT_DESCRIPTOR {
    std::wstring name_;
    std::wstring taskName_;
    std::wstring levelName_;
    std::wstring opcodeName_;
    std::wstring message_;
    std::vector<EventProperty> properties_;

    Event(EVENT_DESCRIPTOR const& desc, TRACE_EVENT_INFO* eventInfo)
        : EVENT_DESCRIPTOR(desc)
    {
        // Note: MSDN doesn't say that task/opcode/level offsets can be zero
        // but there are cases of that
        if (eventInfo->TaskNameOffset == 0) {
            wchar_t b[126];
            _snwprintf_s(b, _TRUNCATE, L"Task_%u", desc.Task);
            taskName_ = b;
        } else {
            taskName_ = GetStringPtr(eventInfo, eventInfo->TaskNameOffset);
        }
        if (eventInfo->OpcodeNameOffset == 0) {
            wchar_t b[126];
            _snwprintf_s(b, _TRUNCATE, L"Opcode_%u", desc.Opcode);
            opcodeName_ = b;
        } else {
            opcodeName_ = GetStringPtr(eventInfo, eventInfo->OpcodeNameOffset);

            // ETL-loaded opcode names can have "win:" prepended.
            if (opcodeName_.rfind(L"win:", 0) == 0) {
                opcodeName_.erase(0, 4);
            }
        }
        if (eventInfo->LevelNameOffset == 0) {
            wchar_t b[126];
            _snwprintf_s(b, _TRUNCATE, L"Level_%u", desc.Level);
            levelName_ = b;
        } else {
            levelName_ = GetStringPtr(eventInfo, eventInfo->LevelNameOffset);
        }

        if (eventInfo->EventMessageOffset != 0) {
            message_ = GetStringPtr(eventInfo, eventInfo->EventMessageOffset);
        }

        auto propCount = eventInfo->TopLevelPropertyCount;
        properties_.reserve(propCount);
        for (ULONG i = 0; i < propCount; ++i) {
            properties_.emplace_back(eventInfo, eventInfo->EventPropertyInfoArray[i]);
        }

        name_ = taskName_ + L'_' + opcodeName_;
    }
};

void EnumerateSystemEvents(GUID const& providerGuid, std::vector<Event>* events, std::wstring* outProviderName)
{
    ULONG bufferSize = 0;
    auto status = TdhEnumerateManifestProviderEvents((LPGUID) &providerGuid, nullptr, &bufferSize);
    switch (status) {
    case ERROR_EMPTY:                   return;
    case ERROR_INSUFFICIENT_BUFFER:     break;
    default:
        fprintf(stderr, "error: could not enumerate events (");
        switch (status) {
        case ERROR_INVALID_DATA:            fprintf(stderr, "ERROR_INVALID_DATA"); break;
        case ERROR_FILE_NOT_FOUND:          fprintf(stderr, "provider meta data not found"); break;
        case ERROR_RESOURCE_TYPE_NOT_FOUND: fprintf(stderr, "ERROR_RESOURCE_TYPE_NOT_FOUND"); break;
        case ERROR_NOT_FOUND:               fprintf(stderr, "provider schema information not found"); break;
        default:                            fprintf(stderr, "error=%u", status);
        }
        fprintf(stderr, ").\n");
        exit(1);
    }

    auto enumInfo = (PROVIDER_EVENT_INFO*) malloc(bufferSize);
    if (enumInfo == nullptr) {
        fprintf(stderr, "error: could not allocate memory for events (%u bytes).\n", bufferSize);
        exit(1);
    }

    status = TdhEnumerateManifestProviderEvents((LPGUID) &providerGuid, enumInfo, &bufferSize);
    if (status != ERROR_SUCCESS) {
        fprintf(stderr, "error: could not enumerate events (error=%u).\n", status);
        free(enumInfo);
        exit(1);
    }

    auto eventCount = enumInfo->NumberOfEvents;
    events->reserve(events->size() + eventCount);
    for (ULONG eventIndex = 0; eventIndex < eventCount; ++eventIndex) {
        auto desc = &enumInfo->EventDescriptorsArray[eventIndex];

        bufferSize = 0;
        status = TdhGetManifestEventInformation((LPGUID) &providerGuid, desc, nullptr, &bufferSize);
        if (status != ERROR_INSUFFICIENT_BUFFER) {
            fprintf(stderr, "error: could not get manifest event information (error=%u).\n", status);
            exit(1);
        }

        auto eventInfo = (TRACE_EVENT_INFO*) malloc(bufferSize);
        if (eventInfo == nullptr) {
            fprintf(stderr, "error: could not allocate memory for event information (%u bytes).\n", bufferSize);
            exit(1);
        }

        status = TdhGetManifestEventInformation((LPGUID) &providerGuid, desc, eventInfo, &bufferSize);
        if (status != ERROR_SUCCESS) {
            fprintf(stderr, "error: could not get manifest event information (error=%u).\n", status);
            free(eventInfo);
            exit(1);
        }

        events->emplace_back(*desc, eventInfo);

        // Patch provider name if we didn't find a name during provider
        // enumeration.
        //
        // Note: sometimes this can have different capitalization than the
        // name obtained during provider enumeration.
        auto providerName = GetStringPtr(eventInfo, eventInfo->ProviderNameOffset);
        if (outProviderName->empty()) {
            *outProviderName = providerName;
        }

        free(eventInfo);
    }

    free(enumInfo);
}

void FilterEvents(
    std::vector<Filter> const& eventIds,
    std::vector<Event>* events)
{
    for (auto ii = events->begin(), ie = events->end(); ii != ie; ) {
        auto id = ii->taskName_ + L"::" + ii->opcodeName_;
        auto keep = false;
        for (auto const& eventId : eventIds) {
            if (eventId.Matches(id.c_str())) {
                keep = true;
                break;
            }
        }
        if (keep) {
            ++ii;
        } else {
            ii = events->erase(ii);
            ie = events->end();
        }
    }
}

// ----------------------------------------------------------------------------
// ETL

struct EtlProvider {
    Provider provider_;
    std::vector<Event> events_;
};

struct GUIDHash {
    size_t operator()(GUID const& key) const
    {
        static_assert((sizeof(key) % sizeof(size_t)) == 0, "sizeof(GUID) must be multiple of sizeof(size_t)");
        auto p = (size_t const*)&key;
        auto h = (size_t)0;
        for (size_t i = 0; i < sizeof(key) / sizeof(size_t); ++i) {
            h ^= p[i];
        }
        return h;
    }
};

struct GUIDEqual {
    bool operator()(GUID const& lhs, GUID const& rhs) const
    {
        return memcmp(&lhs, &rhs, sizeof(GUID)) == 0;
    }
};

std::unordered_map<GUID, EtlProvider, GUIDHash, GUIDEqual> etlProviders_;

void CALLBACK EventRecordCallback(EVENT_RECORD* eventRecord)
{
    auto const& hdr = eventRecord->EventHeader;

    if (hdr.ProviderId == Microsoft_Windows_EventMetadata::GUID &&
        hdr.EventDescriptor.Opcode == Microsoft_Windows_EventMetadata::EventInfo::Opcode) {
        auto tei = (TRACE_EVENT_INFO*) eventRecord->UserData;

        auto pr = etlProviders_.emplace(tei->ProviderGuid, EtlProvider());
        auto p = &pr.first->second;
        if (pr.second) {
            p->provider_.SetGUID(tei->ProviderGuid);
        }

        if (p->provider_.name_.empty() && tei->ProviderNameOffset != 0) {
            p->provider_.name_ = GetStringPtr((void*) tei, tei->ProviderNameOffset);
        }

        p->events_.emplace_back(tei->EventDescriptor, tei);
    }
}

void EnumerateEtlProviders(
    wchar_t* etlFile,
    std::vector<Filter>* providerIds,
    std::vector<Provider>* providers)
{
    EVENT_TRACE_LOGFILEW traceProps = {};
    traceProps.LogFileName = etlFile;
    traceProps.ProcessTraceMode = PROCESS_TRACE_MODE_EVENT_RECORD | PROCESS_TRACE_MODE_RAW_TIMESTAMP;
    traceProps.EventRecordCallback = &EventRecordCallback;

    auto traceHandle = OpenTraceW(&traceProps);
    if (traceHandle == INVALID_PROCESSTRACE_HANDLE) {
        fprintf(stderr, "error: failed to open ETL file: %ls\n", etlFile);
        exit(1);
    }

    auto status = ProcessTrace(&traceHandle, 1, NULL, NULL);
    status = CloseTrace(traceHandle);

    for (auto ii = etlProviders_.begin(), ie = etlProviders_.end(); ii != ie; ) {
        auto const& provider = ii->second.provider_;
        if (provider.Matches(providerIds)) {
            providers->emplace_back(provider);
            ++ii;
        } else {
            ii = etlProviders_.erase(ii);
        }
    }
}

void EnumerateEtlEvents(GUID const& providerGuid, std::vector<Event>* events, std::wstring* outProviderName)
{
    (void) outProviderName;

    auto ii = etlProviders_.find(providerGuid);
    if (ii == etlProviders_.end()) {
        events->clear();
    } else {
        *events = ii->second.events_;
    }
}



// ----------------------------------------------------------------------------
// Printing functions

wchar_t const* InTypeToString(USHORT intype) {
#define RETURN_INTYPE(_Type) if (intype == TDH_INTYPE_##_Type) return L#_Type
    RETURN_INTYPE(NULL);
    RETURN_INTYPE(UNICODESTRING);
    RETURN_INTYPE(ANSISTRING);
    RETURN_INTYPE(INT8);
    RETURN_INTYPE(UINT8);
    RETURN_INTYPE(INT16);
    RETURN_INTYPE(UINT16);
    RETURN_INTYPE(INT32);
    RETURN_INTYPE(UINT32);
    RETURN_INTYPE(INT64);
    RETURN_INTYPE(UINT64);
    RETURN_INTYPE(FLOAT);
    RETURN_INTYPE(DOUBLE);
    RETURN_INTYPE(BOOLEAN);
    RETURN_INTYPE(BINARY);
    RETURN_INTYPE(GUID);
    RETURN_INTYPE(POINTER);
    RETURN_INTYPE(FILETIME);
    RETURN_INTYPE(SYSTEMTIME);
    RETURN_INTYPE(SID);
    RETURN_INTYPE(HEXINT32);
    RETURN_INTYPE(HEXINT64);
#undef RETURN_INTYPE
    return L"Unknown intype";
}

wchar_t const* OutTypeToString(USHORT outtype) {
#define RETURN_OUTTYPE(_Type) if (outtype == TDH_OUTTYPE_##_Type) return L#_Type
    RETURN_OUTTYPE(NULL);
    RETURN_OUTTYPE(STRING);
    RETURN_OUTTYPE(DATETIME);
    RETURN_OUTTYPE(BYTE);
    RETURN_OUTTYPE(UNSIGNEDBYTE);
    RETURN_OUTTYPE(SHORT);
    RETURN_OUTTYPE(UNSIGNEDSHORT);
    RETURN_OUTTYPE(INT);
    RETURN_OUTTYPE(UNSIGNEDINT);
    RETURN_OUTTYPE(LONG);
    RETURN_OUTTYPE(UNSIGNEDLONG);
    RETURN_OUTTYPE(FLOAT);
    RETURN_OUTTYPE(DOUBLE);
    RETURN_OUTTYPE(BOOLEAN);
    RETURN_OUTTYPE(GUID);
    RETURN_OUTTYPE(HEXBINARY);
    RETURN_OUTTYPE(HEXINT8);
    RETURN_OUTTYPE(HEXINT16);
    RETURN_OUTTYPE(HEXINT32);
    RETURN_OUTTYPE(HEXINT64);
    RETURN_OUTTYPE(PID);
    RETURN_OUTTYPE(TID);
    RETURN_OUTTYPE(PORT);
    RETURN_OUTTYPE(IPV4);
    RETURN_OUTTYPE(IPV6);
    RETURN_OUTTYPE(SOCKETADDRESS);
    RETURN_OUTTYPE(CIMDATETIME);
    RETURN_OUTTYPE(ETWTIME);
    RETURN_OUTTYPE(XML);
    RETURN_OUTTYPE(ERRORCODE);
    RETURN_OUTTYPE(WIN32ERROR);
    RETURN_OUTTYPE(NTSTATUS);
    RETURN_OUTTYPE(HRESULT);
    RETURN_OUTTYPE(CULTURE_INSENSITIVE_DATETIME);
    RETURN_OUTTYPE(JSON);
    //RETURN_OUTTYPE(UTF8);
   // RETURN_OUTTYPE(PKCS7_WITH_TYPE_INFO);
#undef RETURN_OUTTYPE
    return L"Unknown outtype";
}

std::wstring CppCondition(std::wstring s)
{
    auto n = s.length();
    if (n > 0) {
        // Add _ if first character is a number
        size_t i = 0;
        if (isdigit(s[i])) {
            s.insert(s.begin(), '_');
            i += 1;
        }

        // Convert bad characters into _
        for ( ; i < n; ++i) {
            if (s[i] == L' ' ||
                s[i] == L'-' ||
                s[i] == L'/' ||
                s[i] == L':' ||
                s[i] == L'.' ||
                s[i] == L',' ||
                s[i] == L'(' ||
                s[i] == L')') {
                s[i] = L'_';
            }
        }
    }
    return s;
}

std::wstring GetMemberStructName(std::wstring const& name, size_t memberIndex)
{
    wchar_t append[128] = {};
    _snwprintf_s(append, _TRUNCATE, L"_MemberStruct_%zu", memberIndex);
    return name + append;
}

void PrintCppStruct(std::vector<EventProperty> const& members, std::wstring const& name)
{
    auto memberCount = members.size();
    if (memberCount == 0) {
        return;
    }

    // First print any member struct dependencies
    {
        size_t memberIndex = 1;
        for (auto const& member : members) {
            if (member.Flags & PropertyStruct) {
                PrintCppStruct(member.members_, GetMemberStructName(name, memberIndex));
                memberIndex += 1;
            }
        }
    }

    // Break the struct up into parts at any variable-sized member
    std::vector<std::pair<size_t, bool> > parts;
    {
        bool hasPointerMember = false;
        for (size_t i = 0; ; ++i) {
            auto const& member = members[i];
            if (HasPointer(member)) {
                hasPointerMember = true;
            }
            if (i == memberCount - 1) break;
            // If is variable length ...
            if (((member.Flags & (PropertyParamLength | PropertyParamCount)) != 0) ||
                ((member.Flags & (PropertyWBEMXmlFragment | PropertyHasCustomSchema | PropertyParamFixedLength | PropertyParamFixedCount)) == 0 &&
                 (member.nonStructType.InType == TDH_INTYPE_UNICODESTRING ||
                  member.nonStructType.InType == TDH_INTYPE_ANSISTRING ||
                  member.nonStructType.InType == TDH_INTYPE_SID))) {
                parts.emplace_back(i + 1, hasPointerMember);
                hasPointerMember = false;
            }
        }
        parts.emplace_back(memberCount, hasPointerMember);
    }

    for (size_t memberIndex = 0, structMemberIndex = 1, partIndex = 0, partCount = parts.size(); partIndex < partCount; ++partIndex) {
        auto const& part = parts[partIndex];
        auto partEnd = part.first;
        auto partHasPointerMembers = part.second;

        // Start the struct for this part
        if (partHasPointerMembers) {
            printf("template<typename PointerT>\n");
        }
        printf("struct %ls_Struct", name.c_str());
        if (partCount > 1) {
            printf("_Part%zu", partIndex + 1);
        }
        printf(" {\n");

        // Add the members for this part
        for ( ; memberIndex < partEnd; ++memberIndex) {
            auto const& member = members[memberIndex];

            auto isStruct           = (member.Flags & PropertyStruct) != 0;
            auto isNonStruct        = (member.Flags & (PropertyWBEMXmlFragment | PropertyHasCustomSchema)) == 0;
            auto isParamLength      = (member.Flags & PropertyParamLength) != 0;
            auto isParamCount       = (member.Flags & PropertyParamCount) != 0;
            auto isParamFixedLength = (member.Flags & PropertyParamFixedLength) != 0;
            auto isParamFixedCount  = (member.Flags & PropertyParamFixedCount) != 0;

            auto fixedCount = isParamFixedLength ? member.length : member.count;
            auto ending = L";";

            assert(isStruct || isNonStruct);
            (void) isNonStruct;

            std::wstring stype;
            wchar_t const* type = L"unsupported_type";
            if (isStruct) {
                stype = std::wstring(L"struct ") + GetMemberStructName(name, structMemberIndex) + L"_Struct";
                if (HasPointer(member.members_)) {
                    stype += L"<PointerT>";
                }
                type = stype.c_str();
                structMemberIndex += 1;
            } else if (member.nonStructType.MapNameOffset != 0) {
                type = member.mapName_.c_str();
            } else {
                switch (member.nonStructType.InType) {
                case TDH_INTYPE_INT8:     type = L"int8_t"; break;
                case TDH_INTYPE_UINT8:    type = L"uint8_t"; break;
                case TDH_INTYPE_INT16:    type = L"int16_t"; break;
                case TDH_INTYPE_UINT16:   type = L"uint16_t"; break;
                case TDH_INTYPE_INT32:    type = L"int32_t"; break;
                case TDH_INTYPE_BOOLEAN:
                case TDH_INTYPE_HEXINT32:
                case TDH_INTYPE_UINT32:   type = L"uint32_t"; break;
                case TDH_INTYPE_INT64:    type = L"int64_t"; break;
                case TDH_INTYPE_HEXINT64:
                case TDH_INTYPE_UINT64:   type = L"uint64_t"; break;
                case TDH_INTYPE_FLOAT:    type = L"float"; break;
                case TDH_INTYPE_DOUBLE:   type = L"double"; break;

                case TDH_INTYPE_POINTER:
                    // If eventRecord.EventHeader.Flags has
                    // EVENT_HEADER_FLAG_32_BIT_HEADER set, the field size is 4
                    // bytes.  If the EVENT_HEADER_FLAG_64_BIT_HEADER is set the
                    // field size is 8 bytes.
                    type = L"PointerT";
                    break;

                case TDH_INTYPE_UNICODESTRING:
                    type = L"wchar_t";
                    if (!isParamLength && !isParamFixedLength) {
                        ending = L"[]; // null-terminated";
                    }
                    break;

                case TDH_INTYPE_ANSISTRING:
                    type = L"uint8_t";
                    if (!isParamLength && !isParamFixedLength) {
                        ending = L"[]; // null-terminated";
                    }
                    break;

                case TDH_INTYPE_BINARY:
                    type = L"uint8_t";
                    if (member.nonStructType.OutType == TDH_OUTTYPE_IPV6) {
                        isParamFixedCount = true;
                        fixedCount = 16;
                    }
                    break;

                case TDH_INTYPE_GUID:       type = L"uint8_t"; isParamFixedCount = true; fixedCount = 16; break;
                case TDH_INTYPE_FILETIME:   type = L"uint8_t"; isParamFixedCount = true; fixedCount = 8; break;
                case TDH_INTYPE_SYSTEMTIME: type = L"uint8_t"; isParamFixedCount = true; fixedCount = 16; break;

                case TDH_INTYPE_SID:
                    type = L"uint8_t";
                    ending = L"[]; // Field size is determined by reading the first few bytes of the field value.";
                    break;

                default: assert(0);
                }
            }

            printf("    %-*ls %ls", 11, type, CppCondition(member.name_).c_str());

            // Array count
            if (isParamLength || isParamCount) {
                printf("[]; // Count provided by %ls.\n", (isParamLength ? member.lengthName_ : member.countName_).c_str());
            } else if (isParamFixedLength || isParamFixedCount) {
                printf("[%u];\n", fixedCount);
            } else {
                printf("%ls\n", ending);
            }
        }

        // End the struct for this part
        printf("};\n");
    }

    printf("\n");
}

void CollectUsedEnums(
    Event const& event,
    std::vector<EventProperty> const& members,
    std::map<std::wstring, Event const*>* usedEnums)
{
    for (auto const& member : members) {
        if (member.Flags & PropertyStruct) {
            CollectUsedEnums(event, member.members_, usedEnums);
        } else if (member.nonStructType.MapNameOffset != 0) {
            usedEnums->emplace(member.mapName_, &event);
        }
    }
}

void PrintEnum(
    GUID const& providerGuid,
    Event const& event,
    std::wstring const& name)
{
    EVENT_RECORD eventRecord = {};
    eventRecord.EventHeader.ProviderId = providerGuid;
    eventRecord.EventHeader.EventDescriptor = event;

    ULONG bufferSize = 0;
    TDHSTATUS status = TdhGetEventMapInformation(&eventRecord, (PWSTR) name.c_str(), nullptr, &bufferSize);
    if (status != ERROR_INSUFFICIENT_BUFFER) {
        fprintf(stderr, "error: could not get event map information (error=%u).\n", status);
        exit(1);
    }

    auto mapInfo = (EVENT_MAP_INFO*) malloc(bufferSize);
    if (mapInfo == nullptr) {
        fprintf(stderr, "error: could not allocate memory for event information (%u bytes).\n", bufferSize);
        exit(1);
    }

    status = TdhGetEventMapInformation(&eventRecord, (PWSTR) name.c_str(), mapInfo, &bufferSize);
    if (status != ERROR_SUCCESS) {
        fprintf(stderr, "error: could not get manifest event information (error=%u).\n", status);
        free(mapInfo);
        exit(1);
    }

    if (!(mapInfo->Flag == EVENTMAP_INFO_FLAG_MANIFEST_BITMAP ||
          mapInfo->Flag == EVENTMAP_INFO_FLAG_MANIFEST_VALUEMAP) ||
        mapInfo->MapEntryValueType != EVENTMAP_ENTRY_VALUETYPE_ULONG) {
        fprintf(stderr, "error: unsupported map type: 0x%x, %u.\n", mapInfo->Flag, mapInfo->MapEntryValueType);
        free(mapInfo);
        exit(1);
    }

    auto nameLength = name.length();
    auto nameWithoutTYPE = name;
    if (nameLength > 5 && nameWithoutTYPE.compare(nameLength - 5, 5, L"_TYPE") == 0) {
        nameWithoutTYPE.resize(nameLength - 5);
    }

    std::map<std::wstring, uint32_t> entryNameCount;

    printf("\nenum class %ls : uint32_t {\n", nameWithoutTYPE.c_str());
    for (ULONG i = 0; i < mapInfo->EntryCount; ++i) {
        auto const& entry = mapInfo->MapEntryArray[i];

        auto str = GetStringPtr(mapInfo, entry.OutputOffset);
             if (wcsncmp(name.c_str(),            str, nameLength    ) == 0) str += nameLength;
        else if (wcsncmp(nameWithoutTYPE.c_str(), str, nameLength - 5) == 0) str += nameLength - 5;
        if (*str == L'_') str += 1;

        auto entryName = CppCondition(str); // str can have spaces, parenthesis, etc..
        auto count = &entryNameCount.emplace(entryName, 0).first->second;
        *count += 1;
        if (*count == 1) {
            printf("    %ls = %lu,\n", entryName.c_str(), entry.Value);
        } else {
            printf("    %ls_%u = %lu,\n", entryName.c_str(), *count, entry.Value);
        }
    }
    printf("};\n");

    free(mapInfo);
}

int wmain(
    int argc,
    wchar_t** argv)
{
    // Parse command line arguments
    std::vector<Filter> providerIds;
    std::vector<Filter> eventIds;
    wchar_t* etlFile = nullptr;
    auto sortByName = false;
    auto sortByGuid = false;
    auto showKeywords = true;
    auto showLevels = true;
    auto showChannels = true;
    auto showEvents = true;
    auto showEventStructs = true;
    auto showPropertyEnums = true;
    for (int i = 1; i < argc; ++i) {
        if (wcsncmp(argv[i], L"--etl=", 6) == 0) {
            etlFile = argv[i] + 6;
            continue;
        }

        if (wcsncmp(argv[i], L"--provider=", 11) == 0) {
            providerIds.emplace_back(argv[i] + 11);
            continue;
        }

        if (wcsncmp(argv[i], L"--event=", 8) == 0) {
            eventIds.emplace_back(argv[i] + 8);
            continue;
        }

        if (wcscmp(argv[i], L"--sort=guid") == 0) {
            sortByName = false;
            sortByGuid = true;
            continue;
        }

        if (wcscmp(argv[i], L"--sort=name") == 0) {
            sortByName = true;
            sortByGuid = false;
            continue;
        }

        if (wcscmp(argv[i], L"--no_keywords") == 0) {
            showKeywords = false;
            continue;
        }

        if (wcscmp(argv[i], L"--no_levels") == 0) {
            showLevels = false;
            continue;
        }

        if (wcscmp(argv[i], L"--no_channels") == 0) {
            showChannels = false;
            continue;
        }

        if (wcscmp(argv[i], L"--no_events") == 0) {
            showEvents = false;
            continue;
        }

        if (wcscmp(argv[i], L"--no_event_structs") == 0) {
            showEventStructs = false;
            continue;
        }

        if (wcscmp(argv[i], L"--no_prop_enums") == 0) {
            showPropertyEnums = false;
            continue;
        }

        fprintf(stderr, "error: unrecognized argument '%ls'.\n", argv[i]);
        usage();
        return 1;
    }

    if (providerIds.empty()) {
        fprintf(stderr, "error: nothing to list, --provider argument is required.\n");
        usage();
        return 1;
    }

    if (eventIds.empty()) {
        eventIds.emplace_back(L"*");
    }

    if (etlFile) {
        showPropertyEnums = false;
    }

    // Enumerate all providers that match providerIds
    std::vector<Provider> providers;
    if (etlFile) {
        EnumerateEtlProviders(etlFile, &providerIds, &providers);
    } else {
        EnumerateSystemProviders(&providerIds, &providers);
    }

    if (providers.empty()) {
        fprintf(stderr, "error: no matching providers installed.\n");
        return 1;
    }

    // Add any full GUIDs provided by user, even if not enumerated by the
    // system/etl.  If we see events from this provider we'll try to patch the
    // name from the EVENT_INFO.
    for (auto providerId : providerIds) {
        if (!providerId.wildcard_) {
            GUID guid = {};
            if (IIDFromString(providerId.part1_.c_str(), &guid) == S_OK) {
                providers.emplace_back();
                providers.back().guid_ = guid;
                providers.back().guidStr_ = providerId.part1_;
                providers.back().name_ = L"Unknown";
                providers.back().manifest_ = true;
            }
        }
    }

    // Sort providers
    if (sortByName) {
        std::sort(providers.begin(), providers.end(), [](Provider const& a, Provider const& b) {
            return _wcsicmp(a.name_.c_str(), b.name_.c_str()) < 0;
        });
    } else if (sortByGuid) {
        std::sort(providers.begin(), providers.end(), [](Provider const& a, Provider const& b) {
            return _wcsicmp(a.guidStr_.c_str(), b.guidStr_.c_str()) < 0;
        });
    }

    // List providers
    SYSTEMTIME t = {};
    GetSystemTime(&t);

    printf(
        "// Copyright (C) 2020-%d Intel Corporation\n"
        "// SPDX-License-Identifier: MIT\n"
        "//\n"
        "// This file originally generated by etw_list\n"
        "//     version:    %s\n"
        "//     parameters:",
        t.wYear,
        PRESENT_MON_VERSION);
    for (int i = 1; i < argc; ++i) {
        printf(" %ls", argv[i]);
    }
    printf("\n#pragma once\n");
    for (auto& provider : providers) {
        // Enumerate events first, since that can help resolve the provider
        // name.
        std::vector<Event> events;
        if (etlFile) {
            EnumerateEtlEvents(provider.guid_, &events, &provider.name_);
        } else {
            EnumerateSystemEvents(provider.guid_, &events, &provider.name_);
        }

        // Print provider name/guid
        printf(
            "\n"
            "namespace %ls {\n"
            "\n"
            "struct __declspec(uuid(\"%ls\")) GUID_STRUCT;\n"
            "static const auto GUID = __uuidof(GUID_STRUCT);\n",
            CppCondition(provider.name_).c_str(),
            provider.guidStr_.c_str());

        // Print field information
        if (etlFile == nullptr) {
            std::vector<EVENT_FIELD_TYPE> fieldTypes;
            if (showKeywords) fieldTypes.emplace_back(EventKeywordInformation);
            if (showLevels)   fieldTypes.emplace_back(EventLevelInformation);
            if (showChannels) fieldTypes.emplace_back(EventChannelInformation);
            for (auto fieldType : fieldTypes) {
                ULONG size = 0;
                auto hr = TdhEnumerateProviderFieldInformation(&provider.guid_, fieldType, nullptr, &size);
                if (hr == ERROR_NOT_SUPPORTED ||
                    hr == ERROR_NOT_FOUND) {
                    continue;
                }

                char const* eventFieldTypeStr = nullptr;
                char const* eventFieldTypeTypeStr = nullptr;
                switch (fieldType) {
                case EventKeywordInformation: eventFieldTypeStr = "Keyword"; eventFieldTypeTypeStr = "uint64_t"; break;
                case EventLevelInformation:   eventFieldTypeStr = "Level";   eventFieldTypeTypeStr = "uint8_t"; break;
                case EventChannelInformation: eventFieldTypeStr = "Channel"; eventFieldTypeTypeStr = "uint8_t"; break;
                }

                if (hr != ERROR_INSUFFICIENT_BUFFER) {
                    fprintf(stderr, "error: failed to enumerate provider %s information (%u)\n", eventFieldTypeStr, hr);
                    return 1;
                }

                printf("\nenum class %s : %s {\n", eventFieldTypeStr, eventFieldTypeTypeStr);

                auto providerFieldInfo = (PROVIDER_FIELD_INFOARRAY*) malloc(size);
                hr = TdhEnumerateProviderFieldInformation(&provider.guid_, fieldType, providerFieldInfo, &size);
                if (hr != ERROR_SUCCESS) {
                    fprintf(stderr, "error: failed to enumerate provider %s information (%u)\n", eventFieldTypeStr, hr);
                    free(providerFieldInfo);
                    return 1;
                }

                std::vector<std::wstring> names;
                names.reserve(providerFieldInfo->NumberOfElements);
                int maxNameWidth = 0;
                for (ULONG i = 0; i < providerFieldInfo->NumberOfElements; ++i) {
                    auto name = GetStringPtr(providerFieldInfo, providerFieldInfo->FieldInfoArray[i].NameOffset);
                    names.emplace_back(CppCondition(name));
                    maxNameWidth = std::max(maxNameWidth, (int) names.back().length());
                }

                for (ULONG i = 0; i < providerFieldInfo->NumberOfElements; ++i) {
                    printf("    %-*ls = 0x%llx,\n", maxNameWidth, names[i].c_str(), providerFieldInfo->FieldInfoArray[i].Value);
                }

                free(providerFieldInfo);

                printf("};\n");
            }
        }

        // Print events and/or event structs ordered by task
        if (showEvents || showEventStructs || showPropertyEnums) {
            FilterEvents(eventIds, &events);
            auto eventCount = events.size();
            if (eventCount > 0) {
                // Sort events based on name, then id.
                std::sort(events.begin(), events.end(), [](Event const& a, Event const& b) {
                    auto r = a.name_.compare(b.name_);
                    return r < 0 || (r == 0 && a.Id < b.Id);
                });

                // There can be events with the same name but different id, so
                // change duplicate names by appending _2, _3 etc.
                auto maxEventNameWidth = events[0].name_.length();
                for (size_t i = 1, j = 1; i < eventCount; ++i) {
                    if (events[i - j].name_ == events[i].name_) {
                        wchar_t num[128];
                        _snwprintf_s(num, _TRUNCATE, L"_%zu", j + 1);
                        events[i].name_ = events[i].name_ + num;
                        j += 1;
                    } else {
                        j = 1;
                    }

                    maxEventNameWidth = std::max(maxEventNameWidth, events[i].name_.length());
                }

                // Print event descriptors
                if (showEvents) {
                    printf(
                        "\n"
                        "// Event descriptors:\n"
                        "#define EVENT_DESCRIPTOR_DECL(name_, id_, version_, channel_, level_, opcode_, task_, keyword_) "
                        "struct name_ { \\\n"
                        "    static uint16_t const Id      = id_; \\\n"
                        "    static uint8_t  const Version = version_; \\\n"
                        "    static uint8_t  const Channel = channel_; \\\n"
                        "    static uint8_t  const Level   = level_; \\\n"
                        "    static uint8_t  const Opcode  = opcode_; \\\n"
                        "    static uint16_t const Task    = task_; \\\n"
                        "    static %s const Keyword = %skeyword_; \\\n"
                        "}\n"
                        "\n",
                        showKeywords ? "Keyword " : "uint64_t",
                        showKeywords ? "(Keyword) " : "");

                    for (auto const& event : events) {
                        printf("EVENT_DESCRIPTOR_DECL(%-*ls, 0x%04x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%04x, 0x%016llx);\n",
                            (int) maxEventNameWidth,
                            event.name_.c_str(),
                            event.Id,
                            event.Version,
                            event.Channel,
                            event.Level,
                            event.Opcode,
                            event.Task,
                            event.Keyword);
                    }

                    printf("\n#undef EVENT_DESCRIPTOR_DECL\n");
                }

                // Print event property enums
                if (showPropertyEnums) {
                    std::map<std::wstring, Event const*> usedEnums;
                    for (auto const& event : events) {
                        CollectUsedEnums(event, event.properties_, &usedEnums);
                    }

                    for (auto const& pr : usedEnums) {
                        PrintEnum(provider.guid_, *pr.second, pr.first);
                    }
                }

                // Print event structs
                if (showEventStructs) {
                    printf(
                        "\n"
                        "#pragma warning(push)\n"
                        "#pragma warning(disable: 4200) // nonstandard extension used: zero-sized array in struct\n"
                        "#pragma pack(push)\n"
                        "#pragma pack(1)\n"
                        "\n");

                    for (auto const& event : events) {
                        PrintCppStruct(event.properties_, event.name_);
                    }

                    printf(
                        "#pragma pack(pop)\n"
                        "#pragma warning(pop)\n");
                }
            }
        }

        printf("\n}\n");
    }

    // Done
    return 0;
}
