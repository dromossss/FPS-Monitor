//===========================================================================
// Copyright (C) 2022-23 Intel Corporation
// This software and the related documents are Intel copyrighted materials, and
// your use of them is governed by the express license under which they were
// provided to you ("License"). Unless the License provides otherwise, you may
// not use, modify, copy, publish, distribute, disclose or transmit this software
// or the related documents without Intel's prior written permission. This software
// and the related documents are provided as is, with no express or implied
// warranties, other than those that are expressly stated in the License.
//--------------------------------------------------------------------------

/** 
 *
 * @file ctlpvttemp_api.h
 * @version v1-r1
 *
 */
#ifndef _CTLPVTTEMP_API_H
#define _CTLPVTTEMP_API_H
#if defined(__cplusplus)
#pragma once
#endif

// 'core' API headers
#include "igcl_api.h"

#if defined(__cplusplus)
extern "C" {
#endif

// Intel 'ctlApi'Pvt private common types
#if !defined(__GNUC__)
#pragma region common_private_temp
#endif
///////////////////////////////////////////////////////////////////////////////
/// @brief Reuse ctl_result_t as the only type of return error
typedef ctl_result_t ctl_result_t;

///////////////////////////////////////////////////////////////////////////////
#ifndef CTL_APICALL
#if defined(_WIN32)
/// @brief Calling convention for all API functions
#define CTL_APICALL  __cdecl
#else
#define CTL_APICALL  
#endif // defined(_WIN32)
#endif // CTL_APICALL

///////////////////////////////////////////////////////////////////////////////
#ifndef CTL_APIEXPORT
#if defined(_WIN32)
/// @brief Microsoft-specific dllexport storage-class attribute
#define CTL_APIEXPORT  __declspec(dllexport)
#else
#define CTL_APIEXPORT  
#endif // defined(_WIN32)
#endif // CTL_APIEXPORT

///////////////////////////////////////////////////////////////////////////////
#ifndef CTL_DLLEXPORT
#if defined(_WIN32)
/// @brief Microsoft-specific dllexport storage-class attribute
#define CTL_DLLEXPORT  __declspec(dllexport)
#endif // defined(_WIN32)
#endif // CTL_DLLEXPORT

///////////////////////////////////////////////////////////////////////////////
#ifndef CTL_DLLEXPORT
#if __GNUC__ >= 4
/// @brief GCC-specific dllexport storage-class attribute
#define CTL_DLLEXPORT  __attribute__ ((visibility ("default")))
#else
#define CTL_DLLEXPORT  
#endif // __GNUC__ >= 4
#endif // CTL_DLLEXPORT

///////////////////////////////////////////////////////////////////////////////
#ifndef CTL_MAKE_VERSION
/// @brief Test private macro
#define CTL_MAKE_VERSION( _major, _minor )  (( _major << 16 )|( _minor & 0x0000ffff))
#endif // CTL_MAKE_VERSION

///////////////////////////////////////////////////////////////////////////////
/// @brief VF Curve Detail
typedef enum _ctl_vf_curve_details_t
{
    CTL_VF_CURVE_DETAILS_SIMPLIFIED = 0,            ///< Read minimum num of VF points for simplified VF curve view
    CTL_VF_CURVE_DETAILS_MEDIUM = 1,                ///< Read medium num of VF points for more points than simplified view
    CTL_VF_CURVE_DETAILS_ELABORATE = 2,             ///< Read Maximum num of VF points for detailed VF curve View
    CTL_VF_CURVE_DETAILS_MAX

} ctl_vf_curve_details_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief VF Curve type
typedef enum _ctl_vf_curve_type_t
{
    CTL_VF_CURVE_TYPE_STOCK = 0,                    ///< Read default VF curve
    CTL_VF_CURVE_TYPE_LIVE = 1,                     ///< Read Live VF Curve
    CTL_VF_CURVE_TYPE_MAX

} ctl_vf_curve_type_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Overclock Voltage Frequency Point
typedef struct _ctl_voltage_frequency_point_t
{
    uint32_t Voltage;                               ///< [in][out] in milliVolts
    uint32_t Frequency;                             ///< [in][out] in MHz

} ctl_voltage_frequency_point_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Overclock properties
typedef struct _ctl_oc_properties2_t
{
    uint32_t Size;                                  ///< [in] size of this structure
    uint8_t Version;                                ///< [in] version of this structure
    bool bSupported;                                ///< [out] Indicates if the adapter supports overclocking.
    ctl_oc_control_info_t gpuFrequencyOffset;       ///< [out] related to function ::ctlOverclockGpuFrequencyOffsetSetV2
    ctl_oc_control_info_t gpuVoltageOffset;         ///< [out] related to function ::ctlOverclockGpuMaxVoltageOffsetSetV2
    ctl_oc_control_info_t vramFrequencyOffset;      ///< [out] Property Field Deprecated / No Longer Supported
    ctl_oc_control_info_t vramVoltageOffset;        ///< [out] Property Field Deprecated / No Longer Supported
    ctl_oc_control_info_t powerLimit;               ///< [out] related to function ::ctlOverclockPowerLimitSetV2
    ctl_oc_control_info_t temperatureLimit;         ///< [out] related to function ::ctlOverclockTemperatureLimitSetV2
    ctl_oc_control_info_t vramMemSpeedLimit;        ///< [out] related to function ::ctlOverclockVramMemSpeedLimitSetV2
                                                    ///< Supported only for Version > 0
    ctl_oc_control_info_t gpuVFCurveVoltageLimit;   ///< [out] related to function ::ctlOverclockWriteCustomVFCurve Supported
                                                    ///< only for Version > 0
    ctl_oc_control_info_t gpuVFCurveFrequencyLimit; ///< [out] related to function ::ctlOverclockWriteCustomVFCurve Supported
                                                    ///< only for Version > 0

} ctl_oc_properties2_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Get the Current Overclock GPU Frequency Offset
/// 
/// @details
///     - Determine the current frequency offset in effect (refer to
///       ::ctlOverclockGpuFrequencyOffsetSetV2() for details).
///     - The unit of the value returned is given in
///       ::ctl_oc_properties_t::gpuFrequencyOffset::units returned from
///       ::ctlOverclockGetProperties()
///     - The unit of the value returned can be different for different
///       generation of graphics product
/// 
/// @returns
///     - CTL_RESULT_SUCCESS
///     - CTL_RESULT_ERROR_UNINITIALIZED
///     - CTL_RESULT_ERROR_DEVICE_LOST
///     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
///         + `nullptr == hDeviceHandle`
///     - CTL_RESULT_ERROR_INVALID_NULL_POINTER
///         + `nullptr == pOcFrequencyOffset`
CTL_APIEXPORT ctl_result_t CTL_APICALL
ctlOverclockGpuFrequencyOffsetGetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double* pOcFrequencyOffset                      ///< [in,out] Current GPU Overclock Frequency Offset in units given in
                                                    ///< ::ctl_oc_properties_t::gpuFrequencyOffset::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Set the Overclock Frequency Offset for the GPU
/// 
/// @details
///     - The purpose of this function is to increase/decrease the frequency
///       offset at which typical workloads will run within the same thermal
///       budget.
///     - The frequency offset is expressed in units given in
///       ::ctl_oc_properties_t::gpuFrequencyOffset::units returned from
///       ::ctlOverclockGetProperties()
///     - The actual operating frequency for each workload is not guaranteed to
///       change exactly by the specified offset.
///     - For positive frequency offsets, the factory maximum frequency may
///       increase by up to the specified amount.
///     - Specifying large values for the frequency offset can lead to
///       instability. It is recommended that changes are made in small
///       increments and stability/performance measured running intense GPU
///       workloads before increasing further.
///     - This setting is not persistent through system reboots or driver
///       resets/hangs. It is up to the overclock application to reapply the
///       settings in those cases.
///     - This setting can cause system/device instability. It is up to the
///       overclock application to detect if the system has rebooted
///       unexpectedly or the device was restarted. When this occurs, the
///       application should not reapply the overclock settings automatically
///       but instead return to previously known good settings or notify the
///       user that the settings are not being applied.
/// 
/// @returns
///     - CTL_RESULT_SUCCESS
///     - CTL_RESULT_ERROR_UNINITIALIZED
///     - CTL_RESULT_ERROR_DEVICE_LOST
///     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
///         + `nullptr == hDeviceHandle`
CTL_APIEXPORT ctl_result_t CTL_APICALL
ctlOverclockGpuFrequencyOffsetSetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double ocFrequencyOffset                        ///< [in] The GPU Overclocking Frequency Offset Desired in units given in
                                                    ///< ::ctl_oc_properties_t::gpuFrequencyOffset::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Get the Current Overclock Voltage Offset for the GPU
/// 
/// @details
///     - Determine the current maximum voltage offset in effect on the hardware
///       (refer to ::ctlOverclockGpuMaxVoltageOffsetSetV2 for details).
///     - The unit of the value returned is given in
///       ::ctl_oc_properties_t::gpuVoltageOffset::units returned from
///       ::ctlOverclockGetProperties()
///     - The unit of the value returned can be different for different
///       generation of graphics product
/// 
/// @returns
///     - CTL_RESULT_SUCCESS
///     - CTL_RESULT_ERROR_UNINITIALIZED
///     - CTL_RESULT_ERROR_DEVICE_LOST
///     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
///         + `nullptr == hDeviceHandle`
///     - CTL_RESULT_ERROR_INVALID_NULL_POINTER
///         + `nullptr == pOcMaxVoltageOffset`
CTL_APIEXPORT ctl_result_t CTL_APICALL
ctlOverclockGpuMaxVoltageOffsetGetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double* pOcMaxVoltageOffset                     ///< [in,out] Current Overclock GPU Voltage Offset in Units given in
                                                    ///< ::ctl_oc_properties_t::gpuVoltageOffset::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Set the Overclock Voltage Offset for the GPU
/// 
/// @details
///     - The purpose of this function is to attempt to run the GPU up to higher
///       voltages beyond the part warrantee limits. This can permit running at
///       even higher frequencies than can be obtained using the frequency
///       offset setting, but at the risk of reducing the lifetime of the part.
///     - The voltage offset is expressed in units given in
///       ::ctl_oc_properties_t::gpuVoltageOffset::units returned from
///       ::ctlOverclockGetProperties()
///     - The overclock waiver must be set before calling this function
///       otherwise error will be returned.
///     - There is no guarantee that a workload can operate at the higher
///       frequencies permitted by this setting. Significantly more heat will be
///       generated at these high frequencies/voltages which will necessitate a
///       good cooling solution.
/// 
/// @returns
///     - CTL_RESULT_SUCCESS
///     - CTL_RESULT_ERROR_UNINITIALIZED
///     - CTL_RESULT_ERROR_DEVICE_LOST
///     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
///         + `nullptr == hDeviceHandle`
CTL_APIEXPORT ctl_result_t CTL_APICALL
ctlOverclockGpuMaxVoltageOffsetSetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double ocMaxVoltageOffset                       ///< [in] The Overclocking Maximum Voltage Desired in units given in
                                                    ///< ::ctl_oc_properties_t::gpuVoltageOffset::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Get the current Overclock Vram Memory Speed
/// 
/// @details
///     - The purpose of this function is to return the current VRAM Memory
///       Speed
///     - The unit of the value returned is given in
///       ctl_oc_properties_t::vramMemSpeedLimit::units returned from
///       ::ctlOverclockGetProperties()
///     - The unit of the value returned can be different for different
///       generation of graphics product
/// 
/// @returns
///     - CTL_RESULT_SUCCESS
///     - CTL_RESULT_ERROR_UNINITIALIZED
///     - CTL_RESULT_ERROR_DEVICE_LOST
///     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
///         + `nullptr == hDeviceHandle`
///     - CTL_RESULT_ERROR_INVALID_NULL_POINTER
///         + `nullptr == pOcVramMemSpeedLimit`
CTL_APIEXPORT ctl_result_t CTL_APICALL
ctlOverclockVramMemSpeedLimitGetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double* pOcVramMemSpeedLimit                    ///< [in,out] The current VRAM Memory Speed in units given in
                                                    ///< ctl_oc_properties_t::vramMemSpeedLimit::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Set the desired Overclock Vram Memory Speed
/// 
/// @details
///     - The purpose of this function is to increase/decrease the Speed of
///       VRAM.
///     - The Memory Speed is expressed in units given in
///       ctl_oc_properties_t::vramMemSpeedLimit::units returned from
///       ::ctlOverclockGetProperties() with a minimum step size given by
///       ::ctlOverclockGetProperties().
///     - The actual Memory Speed for each workload is not guaranteed to change
///       exactly by the specified offset.
///     - This setting is not persistent through system reboots or driver
///       resets/hangs. It is up to the overclock application to reapply the
///       settings in those cases.
///     - This setting can cause system/device instability. It is up to the
///       overclock application to detect if the system has rebooted
///       unexpectedly or the device was restarted. When this occurs, the
///       application should not reapply the overclock settings automatically
///       but instead return to previously known good settings or notify the
///       user that the settings are not being applied.
///     - If the memory controller doesn't support changes to memory speed on
///       the fly, one of the following return codes will be given:
///     - CTL_RESULT_ERROR_RESET_DEVICE_REQUIRED: The requested memory overclock
///       will be applied when the device is reset or the system is rebooted. In
///       this case, the overclock software should check if the overclock
///       request was applied after the reset/reboot. If it was and when the
///       overclock application shuts down gracefully and if the overclock
///       application wants the setting to be persistent, the application should
///       request the same overclock settings again so that they will be applied
///       on the next reset/reboot. If this is not done, then every time the
///       device is reset and overclock is requested, the device needs to be
///       reset a second time.
///     - CTL_RESULT_ERROR_FULL_REBOOT_REQUIRED: The requested memory overclock
///       will be applied when the system is rebooted. In this case, the
///       overclock software should check if the overclock request was applied
///       after the reboot. If it was and when the overclock application shuts
///       down gracefully and if the overclock application wants the setting to
///       be persistent, the application should request the same overclock
///       settings again so that they will be applied on the next reset/reboot.
///       If this is not done and the overclock setting is requested after the
///       reboot has occurred, a second reboot will be required.
///     - CTL_RESULT_ERROR_UNSUPPORTED_FEATURE: The Memory Speed Get / Set
///       Feature is currently not available or Unsupported in current platform
/// 
/// @returns
///     - CTL_RESULT_SUCCESS
///     - CTL_RESULT_ERROR_UNINITIALIZED
///     - CTL_RESULT_ERROR_DEVICE_LOST
///     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
///         + `nullptr == hDeviceHandle`
CTL_APIEXPORT ctl_result_t CTL_APICALL
ctlOverclockVramMemSpeedLimitSetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double ocVramMemSpeedLimit                      ///< [in] The desired Memory Speed in units given in
                                                    ///< ctl_oc_properties_t::vramMemSpeedLimit::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Get the Current Sustained power limit
/// 
/// @details
///     - The purpose of this function is to read the current sustained power
///       limit.
///     - The unit of the value returned is given in
///       ::ctl_oc_properties_t::powerLimit::units returned from
///       ::ctlOverclockGetProperties()
///     - The unit of the value returned can be different for different
///       generation of graphics product
/// 
/// @returns
///     - CTL_RESULT_SUCCESS
///     - CTL_RESULT_ERROR_UNINITIALIZED
///     - CTL_RESULT_ERROR_DEVICE_LOST
///     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
///         + `nullptr == hDeviceHandle`
///     - CTL_RESULT_ERROR_INVALID_NULL_POINTER
///         + `nullptr == pSustainedPowerLimit`
CTL_APIEXPORT ctl_result_t CTL_APICALL
ctlOverclockPowerLimitGetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double* pSustainedPowerLimit                    ///< [in,out] The current Sustained Power limit in Units given in
                                                    ///< ::ctl_oc_properties_t::powerLimit::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Set the Sustained power limit
/// 
/// @details
///     - The purpose of this function is to set the maximum sustained power
///       limit. If the average GPU power averaged over a few seconds exceeds
///       this value, the frequency of the GPU will be throttled.
///     - Set a value of 0 to disable this power limit. In this case, the GPU
///       frequency will not throttle due to average power but may hit other
///       limits.
///     - The unit of the PowerLimit to be set is given in
///       ::ctl_oc_properties_t::powerLimit::units returned from
///       ::ctlOverclockGetProperties()
///     - The unit of the value returned can be different for different
///       generation of graphics product
/// 
/// @returns
///     - CTL_RESULT_SUCCESS
///     - CTL_RESULT_ERROR_UNINITIALIZED
///     - CTL_RESULT_ERROR_DEVICE_LOST
///     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
///         + `nullptr == hDeviceHandle`
CTL_APIEXPORT ctl_result_t CTL_APICALL
ctlOverclockPowerLimitSetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double sustainedPowerLimit                      ///< [in] The desired sustained power limit in Units given in
                                                    ///< ::ctl_oc_properties_t::powerLimit::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Get the current temperature limit
/// 
/// @details
///     - The purpose of this function is to read the current thermal limit used
///       for Overclocking
///     - The unit of the value returned is given in
///       ::ctl_oc_properties_t::temperatureLimit::units returned from
///       ::ctlOverclockGetProperties()
///     - The unit of the value returned can be different for different
///       generation of graphics product
/// 
/// @returns
///     - CTL_RESULT_SUCCESS
///     - CTL_RESULT_ERROR_UNINITIALIZED
///     - CTL_RESULT_ERROR_DEVICE_LOST
///     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
///         + `nullptr == hDeviceHandle`
///     - CTL_RESULT_ERROR_INVALID_NULL_POINTER
///         + `nullptr == pTemperatureLimit`
CTL_APIEXPORT ctl_result_t CTL_APICALL
ctlOverclockTemperatureLimitGetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double* pTemperatureLimit                       ///< [in,out] The current temperature limit in Units given in
                                                    ///< ::ctl_oc_properties_t::temperatureLimit::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Set the temperature limit
/// 
/// @details
///     - The purpose of this function is to change the maximum thermal limit.
///       When the GPU temperature exceeds this value, the GPU frequency will be
///       throttled.
///     - The unit of the value to be set is given in
///       ::ctl_oc_properties_t::temperatureLimit::units returned from
///       ::ctlOverclockGetProperties()
/// 
/// @returns
///     - CTL_RESULT_SUCCESS
///     - CTL_RESULT_ERROR_UNINITIALIZED
///     - CTL_RESULT_ERROR_DEVICE_LOST
///     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
///         + `nullptr == hDeviceHandle`
CTL_APIEXPORT ctl_result_t CTL_APICALL
ctlOverclockTemperatureLimitSetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double temperatureLimit                         ///< [in] The desired temperature limit in Units given in
                                                    ///< ctl_oc_properties_t::temperatureLimit::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Read VF Curve
/// 
/// @details
///     - Read the Voltage-Frequency Curve
/// 
/// @returns
///     - CTL_RESULT_SUCCESS
///     - CTL_RESULT_ERROR_UNINITIALIZED
///     - CTL_RESULT_ERROR_DEVICE_LOST
///     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
///         + `nullptr == hDeviceAdapter`
///     - CTL_RESULT_ERROR_INVALID_ENUMERATION
///         + `::CTL_VF_CURVE_TYPE_LIVE < VFCurveType`
///         + `::CTL_VF_CURVE_DETAILS_ELABORATE < VFCurveDetail`
///     - CTL_RESULT_ERROR_UNKNOWN - "Unknown Error"
CTL_APIEXPORT ctl_result_t CTL_APICALL
ctlOverclockReadVFCurve(
    ctl_device_adapter_handle_t hDeviceAdapter,     ///< [in][release] Handle to control device adapter
    ctl_vf_curve_type_t VFCurveType,                ///< [in] Type of Curve to read
    ctl_vf_curve_details_t VFCurveDetail,           ///< [in] Detail of Curve to read
    uint32_t * pNumPoints,                          ///< [in][out] Number of points in the custom VF curve. If the NumPoints is
                                                    ///< zero, then the api will update the value with total number of Points
                                                    ///< based on requested VFCurveType and VFCurveDetail. If the NumPoints is
                                                    ///< non-zero, then the api will read and update the VF points in
                                                    ///< pVFCurveTable buffer provided. If the NumPoints doesn't match what the
                                                    ///< api returned in the first call, it will return an error.
    ctl_voltage_frequency_point_t * pVFCurveTable   ///< [in][out] Pointer to array of VF points, to copy the VF curve being
                                                    ///< read
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Write Custom VF curve
/// 
/// @details
///     - Modify the Voltage-Frequency Curve used by GPU
///     - Valid Voltage-Frequency Curve shall have Voltage and Frequency Points
///       in increasing order
///     - Recommended to create Custom V-F Curve from reading Current V-F Curve
///       using ::ctlOverclockReadVFCurve (Read-Modify-Write)
///     - If Custom V-F curve write request is Successful, the Applied VF Curve
///       might be slightly different than what is originally requested,
///       recommended to update the UI by reading the V-F curve again using
///       ctlOverclockReadVFCurve (with ctl_vf_curve_type_t::LIVE as input)
///     - The overclock waiver must be set before calling this function
///       otherwise error will be returned.
/// 
/// @returns
///     - CTL_RESULT_SUCCESS
///     - CTL_RESULT_ERROR_UNINITIALIZED
///     - CTL_RESULT_ERROR_DEVICE_LOST
///     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
///         + `nullptr == hDeviceAdapter`
///     - CTL_RESULT_ERROR_INVALID_NULL_POINTER
///         + `nullptr == pCustomVFCurveTable`
///     - CTL_RESULT_ERROR_UNKNOWN - "Unknown Error"
CTL_APIEXPORT ctl_result_t CTL_APICALL
ctlOverclockWriteCustomVFCurve(
    ctl_device_adapter_handle_t hDeviceAdapter,     ///< [in][release] Handle to control device adapter
    uint32_t NumPoints,                             ///< [in] Number of points in the custom VF curve
    ctl_voltage_frequency_point_t* pCustomVFCurveTable  ///< [in] Pointer to an array of VF Points containing 'NumPoints' Custom VF
                                                    ///< points
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Power Telemetry
typedef struct _ctl_power_telemetry2_t
{
    uint32_t Size;                                  ///< [in] size of this structure
    uint8_t Version;                                ///< [in] version of this structure
    ctl_oc_telemetry_item_t timeStamp;              ///< [out] Snapshot of the timestamp counter that measures the total time
                                                    ///< since Jan 1, 1970 UTC. It is a decimal value in seconds with a minimum
                                                    ///< accuracy of 1 millisecond.
    ctl_oc_telemetry_item_t gpuEnergyCounter;       ///< [out] Snapshot of the monotonic energy counter maintained by hardware.
                                                    ///< It measures the total energy consumed by the GPU chip. By taking the
                                                    ///< delta between two snapshots and dividing by the delta time in seconds,
                                                    ///< an application can compute the average power.
    ctl_oc_telemetry_item_t gpuVoltage;             ///< [out] Instantaneous snapshot of the voltage feeding the GPU chip. It
                                                    ///< is measured at the power supply output - chip input will be lower.
    ctl_oc_telemetry_item_t gpuCurrentClockFrequency;   ///< [out] Instantaneous snapshot of the GPU chip frequency.
    ctl_oc_telemetry_item_t gpuCurrentTemperature;  ///< [out] Instantaneous snapshot of the GPU chip temperature, read from
                                                    ///< the sensor reporting the highest value.
    ctl_oc_telemetry_item_t globalActivityCounter;  ///< [out] Snapshot of the monotonic global activity counter. It measures
                                                    ///< the time in seconds (accurate down to 1 millisecond) that any GPU
                                                    ///< engine is busy. By taking the delta between two snapshots and dividing
                                                    ///< by the delta time in seconds, an application can compute the average
                                                    ///< percentage utilization of the GPU..
    ctl_oc_telemetry_item_t renderComputeActivityCounter;   ///< [out] Snapshot of the monotonic 3D/compute activity counter. It
                                                    ///< measures the time in seconds (accurate down to 1 millisecond) that any
                                                    ///< 3D render/compute engine is busy. By taking the delta between two
                                                    ///< snapshots and dividing by the delta time in seconds, an application
                                                    ///< can compute the average percentage utilization of all 3D
                                                    ///< render/compute blocks in the GPU.
    ctl_oc_telemetry_item_t mediaActivityCounter;   ///< [out] Snapshot of the monotonic media activity counter. It measures
                                                    ///< the time in seconds (accurate down to 1 millisecond) that any media
                                                    ///< engine is busy. By taking the delta between two snapshots and dividing
                                                    ///< by the delta time in seconds, an application can compute the average
                                                    ///< percentage utilization of all media blocks in the GPU.
    bool gpuPowerLimited;                           ///< [out] Instantaneous indication that the desired GPU frequency is being
                                                    ///< throttled because the GPU chip is exceeding the maximum power limits.
                                                    ///< Increasing the power limits using ::ctlOverclockPowerLimitSet() is one
                                                    ///< way to remove this limitation.
    bool gpuTemperatureLimited;                     ///< [out] Instantaneous indication that the desired GPU frequency is being
                                                    ///< throttled because the GPU chip is exceeding the temperature limits.
                                                    ///< Increasing the temperature limits using
                                                    ///< ::ctlOverclockTemperatureLimitSet() is one way to reduce this
                                                    ///< limitation. Improving the cooling solution is another way.
    bool gpuCurrentLimited;                         ///< [out] Instantaneous indication that the desired GPU frequency is being
                                                    ///< throttled because the GPU chip has exceeded the power supply current
                                                    ///< limits. A better power supply is required to reduce this limitation.
    bool gpuVoltageLimited;                         ///< [out] Instantaneous indication that the GPU frequency cannot be
                                                    ///< increased because the voltage limits have been reached. Increase the
                                                    ///< voltage offset using ::ctlOverclockGpuVoltageOffsetSet() is one way to
                                                    ///< reduce this limitation.
    bool gpuUtilizationLimited;                     ///< [out] Instantaneous indication that due to lower GPU utilization, the
                                                    ///< hardware has lowered the GPU frequency.
    ctl_oc_telemetry_item_t vramEnergyCounter;      ///< [out] Snapshot of the monotonic energy counter maintained by hardware.
                                                    ///< It measures the total energy consumed by the local memory modules. By
                                                    ///< taking the delta between two snapshots and dividing by the delta time
                                                    ///< in seconds, an application can compute the average power.
    ctl_oc_telemetry_item_t vramVoltage;            ///< [out] Instantaneous snapshot of the voltage feeding the memory
                                                    ///< modules.
    ctl_oc_telemetry_item_t vramCurrentClockFrequency;  ///< [out] Instantaneous snapshot of the raw clock frequency driving the
                                                    ///< memory modules.
    ctl_oc_telemetry_item_t vramCurrentEffectiveFrequency;  ///< [out] Instantaneous snapshot of the effective data transfer rate that
                                                    ///< the memory modules can sustain based on the current clock frequency..
    ctl_oc_telemetry_item_t vramReadBandwidthCounter;   ///< [out] Instantaneous snapshot of the monotonic counter that measures
                                                    ///< the read traffic from the memory modules. By taking the delta between
                                                    ///< two snapshots and dividing by the delta time in seconds, an
                                                    ///< application can compute the average read bandwidth.
    ctl_oc_telemetry_item_t vramWriteBandwidthCounter;  ///< [out] Instantaneous snapshot of the monotonic counter that measures
                                                    ///< the write traffic to the memory modules. By taking the delta between
                                                    ///< two snapshots and dividing by the delta time in seconds, an
                                                    ///< application can compute the average write bandwidth.
    ctl_oc_telemetry_item_t vramCurrentTemperature; ///< [out] Instantaneous snapshot of the memory modules temperature, read
                                                    ///< from the sensor reporting the highest value.
    bool vramPowerLimited;                          ///< [out] Deprecated / Not-supported, will always returns false
    bool vramTemperatureLimited;                    ///< [out] Deprecated / Not-supported, will always returns false
    bool vramCurrentLimited;                        ///< [out] Deprecated / Not-supported, will always returns false
    bool vramVoltageLimited;                        ///< [out] Deprecated / Not-supported, will always returns false
    bool vramUtilizationLimited;                    ///< [out] Deprecated / Not-supported, will always returns false
    ctl_oc_telemetry_item_t totalCardEnergyCounter; ///< [out] Total Card Energy Counter.
    ctl_psu_info_t psu[CTL_PSU_COUNT];              ///< [out] PSU voltage and power.
    ctl_oc_telemetry_item_t fanSpeed[CTL_FAN_COUNT];///< [out] Fan speed.
    ctl_oc_telemetry_item_t gpuVrTemp;              ///< [out] GPU VR temperature. Supported for Version > 0.
    ctl_oc_telemetry_item_t vramVrTemp;             ///< [out] VRAM VR temperature. Supported for Version > 0.
    ctl_oc_telemetry_item_t saVrTemp;               ///< [out] SA VR temperature. Supported for Version > 0.
    ctl_oc_telemetry_item_t gpuEffectiveClock;      ///< [out] Effective frequency of the GPU. Supported for Version > 0.
    ctl_oc_telemetry_item_t gpuOverVoltagePercent;  ///< [out] OverVoltage as a percent between 0 and 100. Positive values
                                                    ///< represent fraction of the maximum over-voltage increment being
                                                    ///< currently applied. Zero indicates operation at or below default
                                                    ///< maximum frequency.  Supported for Version > 0.
    ctl_oc_telemetry_item_t gpuPowerPercent;        ///< [out] GPUPower expressed as a percent representing the fraction of the
                                                    ///< default maximum power being drawn currently. Values greater than 100
                                                    ///< indicate power draw beyond default limits. Values above OC Power limit
                                                    ///< imply throttling due to power. Supported for Version > 0.
    ctl_oc_telemetry_item_t gpuTemperaturePercent;  ///< [out] GPUTemperature expressed as a percent of the thermal margin.
                                                    ///< Values of 100 or greater indicate thermal throttling and 0 indicates
                                                    ///< device at 0 degree Celcius. Supported for Version > 0.
    ctl_oc_telemetry_item_t vramReadBandwidth;      ///< [out] Vram Read Bandwidth. Supported for Version > 0.
    ctl_oc_telemetry_item_t vramWriteBandwidth;     ///< [out] Vram Write Bandwidth. Supported for Version > 0.

} ctl_power_telemetry2_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Forward-declare ctl_voltage_frequency_point_t
typedef struct _ctl_voltage_frequency_point_t ctl_voltage_frequency_point_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Forward-declare ctl_oc_properties2_t
typedef struct _ctl_oc_properties2_t ctl_oc_properties2_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Forward-declare ctl_power_telemetry2_t
typedef struct _ctl_power_telemetry2_t ctl_power_telemetry2_t;



#if !defined(__GNUC__)
#pragma endregion // common_private_temp
#endif


///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for ctlOverclockGpuFrequencyOffsetGetV2 
typedef ctl_result_t (CTL_APICALL *ctlpvttemp_$xOverclockGpuFrequencyOffsetGetV2_t)(
    ctl_device_adapter_handle_t,
    double*
    );


///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for ctlOverclockGpuFrequencyOffsetSetV2 
typedef ctl_result_t (CTL_APICALL *ctlpvttemp_$xOverclockGpuFrequencyOffsetSetV2_t)(
    ctl_device_adapter_handle_t,
    double
    );


///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for ctlOverclockGpuMaxVoltageOffsetGetV2 
typedef ctl_result_t (CTL_APICALL *ctlpvttemp_$xOverclockGpuMaxVoltageOffsetGetV2_t)(
    ctl_device_adapter_handle_t,
    double*
    );


///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for ctlOverclockGpuMaxVoltageOffsetSetV2 
typedef ctl_result_t (CTL_APICALL *ctlpvttemp_$xOverclockGpuMaxVoltageOffsetSetV2_t)(
    ctl_device_adapter_handle_t,
    double
    );


///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for ctlOverclockVramMemSpeedLimitGetV2 
typedef ctl_result_t (CTL_APICALL *ctlpvttemp_$xOverclockVramMemSpeedLimitGetV2_t)(
    ctl_device_adapter_handle_t,
    double*
    );


///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for ctlOverclockVramMemSpeedLimitSetV2 
typedef ctl_result_t (CTL_APICALL *ctlpvttemp_$xOverclockVramMemSpeedLimitSetV2_t)(
    ctl_device_adapter_handle_t,
    double
    );


///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for ctlOverclockPowerLimitGetV2 
typedef ctl_result_t (CTL_APICALL *ctlpvttemp_$xOverclockPowerLimitGetV2_t)(
    ctl_device_adapter_handle_t,
    double*
    );


///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for ctlOverclockPowerLimitSetV2 
typedef ctl_result_t (CTL_APICALL *ctlpvttemp_$xOverclockPowerLimitSetV2_t)(
    ctl_device_adapter_handle_t,
    double
    );


///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for ctlOverclockTemperatureLimitGetV2 
typedef ctl_result_t (CTL_APICALL *ctlpvttemp_$xOverclockTemperatureLimitGetV2_t)(
    ctl_device_adapter_handle_t,
    double*
    );


///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for ctlOverclockTemperatureLimitSetV2 
typedef ctl_result_t (CTL_APICALL *ctlpvttemp_$xOverclockTemperatureLimitSetV2_t)(
    ctl_device_adapter_handle_t,
    double
    );


///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for ctlOverclockReadVFCurve 
typedef ctl_result_t (CTL_APICALL *ctlpvttemp_$xOverclockReadVFCurve_t)(
    ctl_device_adapter_handle_t,
    ctl_vf_curve_type_t,
    ctl_vf_curve_details_t,
    uint32_t *,
    ctl_voltage_frequency_point_t *
    );


///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for ctlOverclockWriteCustomVFCurve 
typedef ctl_result_t (CTL_APICALL *ctlpvttemp_$xOverclockWriteCustomVFCurve_t)(
    ctl_device_adapter_handle_t,
    uint32_t,
    ctl_voltage_frequency_point_t*
    );


#if defined(__cplusplus)
} // extern "C"
#endif

#endif // _CTLPVTTEMP_API_H