#pragma once
#include <iostream>
#include "../../PresentData/PresentMonTraceConsumer.hpp"
#include "../PresentMonAPI2/PresentMonAPI.h"
#include "../ControlLib/PresentMonPowerTelemetry.h"

inline const char* PresentModeToString(PresentMode mode) {
  switch (mode) {
    case PresentMode::Hardware_Legacy_Flip:
      return "Hardware: Legacy Flip";
    case PresentMode::Hardware_Legacy_Copy_To_Front_Buffer:
      return "Hardware: Legacy Copy to front buffer";
    case PresentMode::Hardware_Independent_Flip:
      return "Hardware: Independent Flip";
    case PresentMode::Composed_Flip:
      return "Composed: Flip";
    case PresentMode::Composed_Copy_GPU_GDI:
      return "Composed: Copy with GPU GDI";
    case PresentMode::Composed_Copy_CPU_GDI:
      return "Composed: Copy with CPU GDI";
    case PresentMode::Hardware_Composed_Independent_Flip:
      return "Hardware Composed: Independent Flip";
    default:
      return "Other";
  }
}

inline const char* RuntimeToString(Runtime rt) {
  switch (rt) {
    case Runtime::DXGI:
      return "DXGI";
    case Runtime::D3D9:
      return "D3D9";
    default:
      return "Other";
  }
}

inline PM_PSU_TYPE TranslatePsuType(PresentMonPsuType psu_type_in) {
  switch (psu_type_in) {
    case PresentMonPsuType::Pcie:
      return PM_PSU_TYPE::PM_PSU_TYPE_PCIE;
    case PresentMonPsuType::Pin6:
      return PM_PSU_TYPE::PM_PSU_TYPE_6PIN;
    case PresentMonPsuType::Pin8:
      return PM_PSU_TYPE::PM_PSU_TYPE_8PIN;
    default:
      return PM_PSU_TYPE::PM_PSU_TYPE_NONE;
  }
}

inline PM_PRESENT_MODE TranslatePresentMode(
    PresentMode present_mode_in) {
  switch (present_mode_in) {
    case PresentMode::Hardware_Legacy_Flip:
      return PM_PRESENT_MODE::PM_PRESENT_MODE_HARDWARE_LEGACY_FLIP;
    case PresentMode::Hardware_Legacy_Copy_To_Front_Buffer:
      return PM_PRESENT_MODE::
          PM_PRESENT_MODE_HARDWARE_LEGACY_COPY_TO_FRONT_BUFFER;
    case PresentMode::Hardware_Independent_Flip:
      return PM_PRESENT_MODE::PM_PRESENT_MODE_HARDWARE_INDEPENDENT_FLIP;
    case PresentMode::Composed_Flip:
      return PM_PRESENT_MODE::PM_PRESENT_MODE_COMPOSED_FLIP;
    case PresentMode::Hardware_Composed_Independent_Flip:
      return PM_PRESENT_MODE::
          PM_PRESENT_MODE_HARDWARE_COMPOSED_INDEPENDENT_FLIP;
    case PresentMode::Composed_Copy_GPU_GDI:
      return PM_PRESENT_MODE::PM_PRESENT_MODE_COMPOSED_COPY_WITH_GPU_GDI;
    case PresentMode::Composed_Copy_CPU_GDI:
      return PM_PRESENT_MODE::PM_PRESENT_MODE_COMPOSED_COPY_WITH_CPU_GDI;
    default:
      return PM_PRESENT_MODE::PM_PRESENT_MODE_UNKNOWN;
  }
}