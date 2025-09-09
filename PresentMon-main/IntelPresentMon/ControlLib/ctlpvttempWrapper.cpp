//===========================================================================
//Copyright (C) 2022-23 Intel Corporation
//
// 
//
//SPDX-License-Identifier: MIT
//--------------------------------------------------------------------------

/**
 *
 * @file ctlpvttemp_api.cpp
 * @version v1-r1
 *
 */

// Note: UWP applications should have defined WINDOWS_UWP in their compiler settings
// Also at this point, it's easier by not enabling pre-compiled option to compile this file
// Not all functionalities are tested for a UWP application

#include <windows.h>
#include <strsafe.h>
#include <vector>

//#define CTL_APIEXPORT

#include "igcl_api.h"
#include "ctlpvttemp_api.h"

/////////////////////////////////////////////////////////////////////////////////
//
// Implementation of wrapper functions
//
HINSTANCE GetLoaderHandle(void);


/**
* @brief Get the Current Overclock GPU Frequency Offset
* 
* @details
*     - Determine the current frequency offset in effect (refer to
*       ::ctlOverclockGpuFrequencyOffsetSetV2() for details).
*     - The unit of the value returned is given in
*       ::ctl_oc_properties_t::gpuFrequencyOffset::units returned from
*       ::ctlOverclockGetProperties()
*     - The unit of the value returned can be different for different
*       generation of graphics product
* 
* @returns
*     - CTL_RESULT_SUCCESS
*     - CTL_RESULT_ERROR_UNINITIALIZED
*     - CTL_RESULT_ERROR_DEVICE_LOST
*     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
*         + `nullptr == hDeviceHandle`
*     - CTL_RESULT_ERROR_INVALID_NULL_POINTER
*         + `nullptr == pOcFrequencyOffset`
*/
ctl_result_t CTL_APICALL
ctlOverclockGpuFrequencyOffsetGetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double* pOcFrequencyOffset                      ///< [in,out] Current GPU Overclock Frequency Offset in units given in
                                                    ///< ::ctl_oc_properties_t::gpuFrequencyOffset::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    )
{
    ctl_result_t result = CTL_RESULT_ERROR_NOT_INITIALIZED;
    

    HINSTANCE hinstLibPtr = GetLoaderHandle();

    if (NULL != hinstLibPtr)
    {
        ctlpvttemp_$xOverclockGpuFrequencyOffsetGetV2_t pfnOverclockGpuFrequencyOffsetGetV2 = (ctlpvttemp_$xOverclockGpuFrequencyOffsetGetV2_t)GetProcAddress(hinstLibPtr, "ctlOverclockGpuFrequencyOffsetGetV2");
        if (pfnOverclockGpuFrequencyOffsetGetV2)
        {
            result = pfnOverclockGpuFrequencyOffsetGetV2(hDeviceHandle, pOcFrequencyOffset);
        }
    }

    return result;
}


/**
* @brief Set the Overclock Frequency Offset for the GPU
* 
* @details
*     - The purpose of this function is to increase/decrease the frequency
*       offset at which typical workloads will run within the same thermal
*       budget.
*     - The frequency offset is expressed in units given in
*       ::ctl_oc_properties_t::gpuFrequencyOffset::units returned from
*       ::ctlOverclockGetProperties()
*     - The actual operating frequency for each workload is not guaranteed to
*       change exactly by the specified offset.
*     - For positive frequency offsets, the factory maximum frequency may
*       increase by up to the specified amount.
*     - Specifying large values for the frequency offset can lead to
*       instability. It is recommended that changes are made in small
*       increments and stability/performance measured running intense GPU
*       workloads before increasing further.
*     - This setting is not persistent through system reboots or driver
*       resets/hangs. It is up to the overclock application to reapply the
*       settings in those cases.
*     - This setting can cause system/device instability. It is up to the
*       overclock application to detect if the system has rebooted
*       unexpectedly or the device was restarted. When this occurs, the
*       application should not reapply the overclock settings automatically
*       but instead return to previously known good settings or notify the
*       user that the settings are not being applied.
* 
* @returns
*     - CTL_RESULT_SUCCESS
*     - CTL_RESULT_ERROR_UNINITIALIZED
*     - CTL_RESULT_ERROR_DEVICE_LOST
*     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
*         + `nullptr == hDeviceHandle`
*/
ctl_result_t CTL_APICALL
ctlOverclockGpuFrequencyOffsetSetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double ocFrequencyOffset                        ///< [in] The GPU Overclocking Frequency Offset Desired in units given in
                                                    ///< ::ctl_oc_properties_t::gpuFrequencyOffset::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    )
{
    ctl_result_t result = CTL_RESULT_ERROR_NOT_INITIALIZED;
    

    HINSTANCE hinstLibPtr = GetLoaderHandle();

    if (NULL != hinstLibPtr)
    {
        ctlpvttemp_$xOverclockGpuFrequencyOffsetSetV2_t pfnOverclockGpuFrequencyOffsetSetV2 = (ctlpvttemp_$xOverclockGpuFrequencyOffsetSetV2_t)GetProcAddress(hinstLibPtr, "ctlOverclockGpuFrequencyOffsetSetV2");
        if (pfnOverclockGpuFrequencyOffsetSetV2)
        {
            result = pfnOverclockGpuFrequencyOffsetSetV2(hDeviceHandle, ocFrequencyOffset);
        }
    }

    return result;
}


/**
* @brief Get the Current Overclock Voltage Offset for the GPU
* 
* @details
*     - Determine the current maximum voltage offset in effect on the hardware
*       (refer to ::ctlOverclockGpuMaxVoltageOffsetSetV2 for details).
*     - The unit of the value returned is given in
*       ::ctl_oc_properties_t::gpuVoltageOffset::units returned from
*       ::ctlOverclockGetProperties()
*     - The unit of the value returned can be different for different
*       generation of graphics product
* 
* @returns
*     - CTL_RESULT_SUCCESS
*     - CTL_RESULT_ERROR_UNINITIALIZED
*     - CTL_RESULT_ERROR_DEVICE_LOST
*     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
*         + `nullptr == hDeviceHandle`
*     - CTL_RESULT_ERROR_INVALID_NULL_POINTER
*         + `nullptr == pOcMaxVoltageOffset`
*/
ctl_result_t CTL_APICALL
ctlOverclockGpuMaxVoltageOffsetGetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double* pOcMaxVoltageOffset                     ///< [in,out] Current Overclock GPU Voltage Offset in Units given in
                                                    ///< ::ctl_oc_properties_t::gpuVoltageOffset::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    )
{
    ctl_result_t result = CTL_RESULT_ERROR_NOT_INITIALIZED;
    

    HINSTANCE hinstLibPtr = GetLoaderHandle();

    if (NULL != hinstLibPtr)
    {
        ctlpvttemp_$xOverclockGpuMaxVoltageOffsetGetV2_t pfnOverclockGpuMaxVoltageOffsetGetV2 = (ctlpvttemp_$xOverclockGpuMaxVoltageOffsetGetV2_t)GetProcAddress(hinstLibPtr, "ctlOverclockGpuMaxVoltageOffsetGetV2");
        if (pfnOverclockGpuMaxVoltageOffsetGetV2)
        {
            result = pfnOverclockGpuMaxVoltageOffsetGetV2(hDeviceHandle, pOcMaxVoltageOffset);
        }
    }

    return result;
}


/**
* @brief Set the Overclock Voltage Offset for the GPU
* 
* @details
*     - The purpose of this function is to attempt to run the GPU up to higher
*       voltages beyond the part warrantee limits. This can permit running at
*       even higher frequencies than can be obtained using the frequency
*       offset setting, but at the risk of reducing the lifetime of the part.
*     - The voltage offset is expressed in units given in
*       ::ctl_oc_properties_t::gpuVoltageOffset::units returned from
*       ::ctlOverclockGetProperties()
*     - The overclock waiver must be set before calling this function
*       otherwise error will be returned.
*     - There is no guarantee that a workload can operate at the higher
*       frequencies permitted by this setting. Significantly more heat will be
*       generated at these high frequencies/voltages which will necessitate a
*       good cooling solution.
* 
* @returns
*     - CTL_RESULT_SUCCESS
*     - CTL_RESULT_ERROR_UNINITIALIZED
*     - CTL_RESULT_ERROR_DEVICE_LOST
*     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
*         + `nullptr == hDeviceHandle`
*/
ctl_result_t CTL_APICALL
ctlOverclockGpuMaxVoltageOffsetSetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double ocMaxVoltageOffset                       ///< [in] The Overclocking Maximum Voltage Desired in units given in
                                                    ///< ::ctl_oc_properties_t::gpuVoltageOffset::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    )
{
    ctl_result_t result = CTL_RESULT_ERROR_NOT_INITIALIZED;
    

    HINSTANCE hinstLibPtr = GetLoaderHandle();

    if (NULL != hinstLibPtr)
    {
        ctlpvttemp_$xOverclockGpuMaxVoltageOffsetSetV2_t pfnOverclockGpuMaxVoltageOffsetSetV2 = (ctlpvttemp_$xOverclockGpuMaxVoltageOffsetSetV2_t)GetProcAddress(hinstLibPtr, "ctlOverclockGpuMaxVoltageOffsetSetV2");
        if (pfnOverclockGpuMaxVoltageOffsetSetV2)
        {
            result = pfnOverclockGpuMaxVoltageOffsetSetV2(hDeviceHandle, ocMaxVoltageOffset);
        }
    }

    return result;
}


/**
* @brief Get the current Overclock Vram Memory Speed
* 
* @details
*     - The purpose of this function is to return the current VRAM Memory
*       Speed
*     - The unit of the value returned is given in
*       ctl_oc_properties_t::vramMemSpeedLimit::units returned from
*       ::ctlOverclockGetProperties()
*     - The unit of the value returned can be different for different
*       generation of graphics product
* 
* @returns
*     - CTL_RESULT_SUCCESS
*     - CTL_RESULT_ERROR_UNINITIALIZED
*     - CTL_RESULT_ERROR_DEVICE_LOST
*     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
*         + `nullptr == hDeviceHandle`
*     - CTL_RESULT_ERROR_INVALID_NULL_POINTER
*         + `nullptr == pOcVramMemSpeedLimit`
*/
ctl_result_t CTL_APICALL
ctlOverclockVramMemSpeedLimitGetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double* pOcVramMemSpeedLimit                    ///< [in,out] The current VRAM Memory Speed in units given in
                                                    ///< ctl_oc_properties_t::vramMemSpeedLimit::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    )
{
    ctl_result_t result = CTL_RESULT_ERROR_NOT_INITIALIZED;
    

    HINSTANCE hinstLibPtr = GetLoaderHandle();

    if (NULL != hinstLibPtr)
    {
        ctlpvttemp_$xOverclockVramMemSpeedLimitGetV2_t pfnOverclockVramMemSpeedLimitGetV2 = (ctlpvttemp_$xOverclockVramMemSpeedLimitGetV2_t)GetProcAddress(hinstLibPtr, "ctlOverclockVramMemSpeedLimitGetV2");
        if (pfnOverclockVramMemSpeedLimitGetV2)
        {
            result = pfnOverclockVramMemSpeedLimitGetV2(hDeviceHandle, pOcVramMemSpeedLimit);
        }
    }

    return result;
}


/**
* @brief Set the desired Overclock Vram Memory Speed
* 
* @details
*     - The purpose of this function is to increase/decrease the Speed of
*       VRAM.
*     - The Memory Speed is expressed in units given in
*       ctl_oc_properties_t::vramMemSpeedLimit::units returned from
*       ::ctlOverclockGetProperties() with a minimum step size given by
*       ::ctlOverclockGetProperties().
*     - The actual Memory Speed for each workload is not guaranteed to change
*       exactly by the specified offset.
*     - This setting is not persistent through system reboots or driver
*       resets/hangs. It is up to the overclock application to reapply the
*       settings in those cases.
*     - This setting can cause system/device instability. It is up to the
*       overclock application to detect if the system has rebooted
*       unexpectedly or the device was restarted. When this occurs, the
*       application should not reapply the overclock settings automatically
*       but instead return to previously known good settings or notify the
*       user that the settings are not being applied.
*     - If the memory controller doesn't support changes to memory speed on
*       the fly, one of the following return codes will be given:
*     - CTL_RESULT_ERROR_RESET_DEVICE_REQUIRED: The requested memory overclock
*       will be applied when the device is reset or the system is rebooted. In
*       this case, the overclock software should check if the overclock
*       request was applied after the reset/reboot. If it was and when the
*       overclock application shuts down gracefully and if the overclock
*       application wants the setting to be persistent, the application should
*       request the same overclock settings again so that they will be applied
*       on the next reset/reboot. If this is not done, then every time the
*       device is reset and overclock is requested, the device needs to be
*       reset a second time.
*     - CTL_RESULT_ERROR_FULL_REBOOT_REQUIRED: The requested memory overclock
*       will be applied when the system is rebooted. In this case, the
*       overclock software should check if the overclock request was applied
*       after the reboot. If it was and when the overclock application shuts
*       down gracefully and if the overclock application wants the setting to
*       be persistent, the application should request the same overclock
*       settings again so that they will be applied on the next reset/reboot.
*       If this is not done and the overclock setting is requested after the
*       reboot has occurred, a second reboot will be required.
*     - CTL_RESULT_ERROR_UNSUPPORTED_FEATURE: The Memory Speed Get / Set
*       Feature is currently not available or Unsupported in current platform
* 
* @returns
*     - CTL_RESULT_SUCCESS
*     - CTL_RESULT_ERROR_UNINITIALIZED
*     - CTL_RESULT_ERROR_DEVICE_LOST
*     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
*         + `nullptr == hDeviceHandle`
*/
ctl_result_t CTL_APICALL
ctlOverclockVramMemSpeedLimitSetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double ocVramMemSpeedLimit                      ///< [in] The desired Memory Speed in units given in
                                                    ///< ctl_oc_properties_t::vramMemSpeedLimit::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    )
{
    ctl_result_t result = CTL_RESULT_ERROR_NOT_INITIALIZED;
    

    HINSTANCE hinstLibPtr = GetLoaderHandle();

    if (NULL != hinstLibPtr)
    {
        ctlpvttemp_$xOverclockVramMemSpeedLimitSetV2_t pfnOverclockVramMemSpeedLimitSetV2 = (ctlpvttemp_$xOverclockVramMemSpeedLimitSetV2_t)GetProcAddress(hinstLibPtr, "ctlOverclockVramMemSpeedLimitSetV2");
        if (pfnOverclockVramMemSpeedLimitSetV2)
        {
            result = pfnOverclockVramMemSpeedLimitSetV2(hDeviceHandle, ocVramMemSpeedLimit);
        }
    }

    return result;
}


/**
* @brief Get the Current Sustained power limit
* 
* @details
*     - The purpose of this function is to read the current sustained power
*       limit.
*     - The unit of the value returned is given in
*       ::ctl_oc_properties_t::powerLimit::units returned from
*       ::ctlOverclockGetProperties()
*     - The unit of the value returned can be different for different
*       generation of graphics product
* 
* @returns
*     - CTL_RESULT_SUCCESS
*     - CTL_RESULT_ERROR_UNINITIALIZED
*     - CTL_RESULT_ERROR_DEVICE_LOST
*     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
*         + `nullptr == hDeviceHandle`
*     - CTL_RESULT_ERROR_INVALID_NULL_POINTER
*         + `nullptr == pSustainedPowerLimit`
*/
ctl_result_t CTL_APICALL
ctlOverclockPowerLimitGetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double* pSustainedPowerLimit                    ///< [in,out] The current Sustained Power limit in Units given in
                                                    ///< ::ctl_oc_properties_t::powerLimit::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    )
{
    ctl_result_t result = CTL_RESULT_ERROR_NOT_INITIALIZED;
    

    HINSTANCE hinstLibPtr = GetLoaderHandle();

    if (NULL != hinstLibPtr)
    {
        ctlpvttemp_$xOverclockPowerLimitGetV2_t pfnOverclockPowerLimitGetV2 = (ctlpvttemp_$xOverclockPowerLimitGetV2_t)GetProcAddress(hinstLibPtr, "ctlOverclockPowerLimitGetV2");
        if (pfnOverclockPowerLimitGetV2)
        {
            result = pfnOverclockPowerLimitGetV2(hDeviceHandle, pSustainedPowerLimit);
        }
    }

    return result;
}


/**
* @brief Set the Sustained power limit
* 
* @details
*     - The purpose of this function is to set the maximum sustained power
*       limit. If the average GPU power averaged over a few seconds exceeds
*       this value, the frequency of the GPU will be throttled.
*     - Set a value of 0 to disable this power limit. In this case, the GPU
*       frequency will not throttle due to average power but may hit other
*       limits.
*     - The unit of the PowerLimit to be set is given in
*       ::ctl_oc_properties_t::powerLimit::units returned from
*       ::ctlOverclockGetProperties()
*     - The unit of the value returned can be different for different
*       generation of graphics product
* 
* @returns
*     - CTL_RESULT_SUCCESS
*     - CTL_RESULT_ERROR_UNINITIALIZED
*     - CTL_RESULT_ERROR_DEVICE_LOST
*     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
*         + `nullptr == hDeviceHandle`
*/
ctl_result_t CTL_APICALL
ctlOverclockPowerLimitSetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double sustainedPowerLimit                      ///< [in] The desired sustained power limit in Units given in
                                                    ///< ::ctl_oc_properties_t::powerLimit::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    )
{
    ctl_result_t result = CTL_RESULT_ERROR_NOT_INITIALIZED;
    

    HINSTANCE hinstLibPtr = GetLoaderHandle();

    if (NULL != hinstLibPtr)
    {
        ctlpvttemp_$xOverclockPowerLimitSetV2_t pfnOverclockPowerLimitSetV2 = (ctlpvttemp_$xOverclockPowerLimitSetV2_t)GetProcAddress(hinstLibPtr, "ctlOverclockPowerLimitSetV2");
        if (pfnOverclockPowerLimitSetV2)
        {
            result = pfnOverclockPowerLimitSetV2(hDeviceHandle, sustainedPowerLimit);
        }
    }

    return result;
}


/**
* @brief Get the current temperature limit
* 
* @details
*     - The purpose of this function is to read the current thermal limit used
*       for Overclocking
*     - The unit of the value returned is given in
*       ::ctl_oc_properties_t::temperatureLimit::units returned from
*       ::ctlOverclockGetProperties()
*     - The unit of the value returned can be different for different
*       generation of graphics product
* 
* @returns
*     - CTL_RESULT_SUCCESS
*     - CTL_RESULT_ERROR_UNINITIALIZED
*     - CTL_RESULT_ERROR_DEVICE_LOST
*     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
*         + `nullptr == hDeviceHandle`
*     - CTL_RESULT_ERROR_INVALID_NULL_POINTER
*         + `nullptr == pTemperatureLimit`
*/
ctl_result_t CTL_APICALL
ctlOverclockTemperatureLimitGetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double* pTemperatureLimit                       ///< [in,out] The current temperature limit in Units given in
                                                    ///< ::ctl_oc_properties_t::temperatureLimit::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    )
{
    ctl_result_t result = CTL_RESULT_ERROR_NOT_INITIALIZED;
    

    HINSTANCE hinstLibPtr = GetLoaderHandle();

    if (NULL != hinstLibPtr)
    {
        ctlpvttemp_$xOverclockTemperatureLimitGetV2_t pfnOverclockTemperatureLimitGetV2 = (ctlpvttemp_$xOverclockTemperatureLimitGetV2_t)GetProcAddress(hinstLibPtr, "ctlOverclockTemperatureLimitGetV2");
        if (pfnOverclockTemperatureLimitGetV2)
        {
            result = pfnOverclockTemperatureLimitGetV2(hDeviceHandle, pTemperatureLimit);
        }
    }

    return result;
}


/**
* @brief Set the temperature limit
* 
* @details
*     - The purpose of this function is to change the maximum thermal limit.
*       When the GPU temperature exceeds this value, the GPU frequency will be
*       throttled.
*     - The unit of the value to be set is given in
*       ::ctl_oc_properties_t::temperatureLimit::units returned from
*       ::ctlOverclockGetProperties()
* 
* @returns
*     - CTL_RESULT_SUCCESS
*     - CTL_RESULT_ERROR_UNINITIALIZED
*     - CTL_RESULT_ERROR_DEVICE_LOST
*     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
*         + `nullptr == hDeviceHandle`
*/
ctl_result_t CTL_APICALL
ctlOverclockTemperatureLimitSetV2(
    ctl_device_adapter_handle_t hDeviceHandle,      ///< [in][release] Handle to display adapter
    double temperatureLimit                         ///< [in] The desired temperature limit in Units given in
                                                    ///< ctl_oc_properties_t::temperatureLimit::units returned from
                                                    ///< ::ctlOverclockGetProperties()
    )
{
    ctl_result_t result = CTL_RESULT_ERROR_NOT_INITIALIZED;
    

    HINSTANCE hinstLibPtr = GetLoaderHandle();

    if (NULL != hinstLibPtr)
    {
        ctlpvttemp_$xOverclockTemperatureLimitSetV2_t pfnOverclockTemperatureLimitSetV2 = (ctlpvttemp_$xOverclockTemperatureLimitSetV2_t)GetProcAddress(hinstLibPtr, "ctlOverclockTemperatureLimitSetV2");
        if (pfnOverclockTemperatureLimitSetV2)
        {
            result = pfnOverclockTemperatureLimitSetV2(hDeviceHandle, temperatureLimit);
        }
    }

    return result;
}


/**
* @brief Read VF Curve
* 
* @details
*     - Read the Voltage-Frequency Curve
* 
* @returns
*     - CTL_RESULT_SUCCESS
*     - CTL_RESULT_ERROR_UNINITIALIZED
*     - CTL_RESULT_ERROR_DEVICE_LOST
*     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
*         + `nullptr == hDeviceAdapter`
*     - CTL_RESULT_ERROR_INVALID_ENUMERATION
*         + `::CTL_VF_CURVE_TYPE_LIVE < VFCurveType`
*         + `::CTL_VF_CURVE_DETAILS_ELABORATE < VFCurveDetail`
*     - CTL_RESULT_ERROR_UNKNOWN - "Unknown Error"
*/
ctl_result_t CTL_APICALL
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
    )
{
    ctl_result_t result = CTL_RESULT_ERROR_NOT_INITIALIZED;
    

    HINSTANCE hinstLibPtr = GetLoaderHandle();

    if (NULL != hinstLibPtr)
    {
        ctlpvttemp_$xOverclockReadVFCurve_t pfnOverclockReadVFCurve = (ctlpvttemp_$xOverclockReadVFCurve_t)GetProcAddress(hinstLibPtr, "ctlOverclockReadVFCurve");
        if (pfnOverclockReadVFCurve)
        {
            result = pfnOverclockReadVFCurve(hDeviceAdapter, VFCurveType, VFCurveDetail, pNumPoints, pVFCurveTable);
        }
    }

    return result;
}


/**
* @brief Write Custom VF curve
* 
* @details
*     - Modify the Voltage-Frequency Curve used by GPU
*     - Valid Voltage-Frequency Curve shall have Voltage and Frequency Points
*       in increasing order
*     - Recommended to create Custom V-F Curve from reading Current V-F Curve
*       using ::ctlOverclockReadVFCurve (Read-Modify-Write)
*     - If Custom V-F curve write request is Successful, the Applied VF Curve
*       might be slightly different than what is originally requested,
*       recommended to update the UI by reading the V-F curve again using
*       ctlOverclockReadVFCurve (with ctl_vf_curve_type_t::LIVE as input)
*     - The overclock waiver must be set before calling this function
*       otherwise error will be returned.
* 
* @returns
*     - CTL_RESULT_SUCCESS
*     - CTL_RESULT_ERROR_UNINITIALIZED
*     - CTL_RESULT_ERROR_DEVICE_LOST
*     - CTL_RESULT_ERROR_INVALID_NULL_HANDLE
*         + `nullptr == hDeviceAdapter`
*     - CTL_RESULT_ERROR_INVALID_NULL_POINTER
*         + `nullptr == pCustomVFCurveTable`
*     - CTL_RESULT_ERROR_UNKNOWN - "Unknown Error"
*/
ctl_result_t CTL_APICALL
ctlOverclockWriteCustomVFCurve(
    ctl_device_adapter_handle_t hDeviceAdapter,     ///< [in][release] Handle to control device adapter
    uint32_t NumPoints,                             ///< [in] Number of points in the custom VF curve
    ctl_voltage_frequency_point_t* pCustomVFCurveTable  ///< [in] Pointer to an array of VF Points containing 'NumPoints' Custom VF
                                                    ///< points
    )
{
    ctl_result_t result = CTL_RESULT_ERROR_NOT_INITIALIZED;
    

    HINSTANCE hinstLibPtr = GetLoaderHandle();

    if (NULL != hinstLibPtr)
    {
        ctlpvttemp_$xOverclockWriteCustomVFCurve_t pfnOverclockWriteCustomVFCurve = (ctlpvttemp_$xOverclockWriteCustomVFCurve_t)GetProcAddress(hinstLibPtr, "ctlOverclockWriteCustomVFCurve");
        if (pfnOverclockWriteCustomVFCurve)
        {
            result = pfnOverclockWriteCustomVFCurve(hDeviceAdapter, NumPoints, pCustomVFCurveTable);
        }
    }

    return result;
}


//
// End of wrapper function implementation
//
/////////////////////////////////////////////////////////////////////////////////
