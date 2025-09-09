# PresentMon Service

**PresentMon Service** aggregates the ETW frame data of the **PresentMon Analysis library** with hardware telemetry measurements such as GPU temperature and power.  It exposes this information to client applications via the **PresentMon API**.  e.g., the [PresentMon Capture Application](README-CaptureApplication.md) is a *PresentMon Service* client. Applications should use the PresentMon SDK to communicate with the service via the PresentMon API.

![Architecture](IntelPresentMon/docs/images/PresentMonServiceArchitecture.PNG)

![PresentMon2_Sequence_Diagram](IntelPresentMon/docs/images/PresentMonService_Sequence_Diagram.png)

# PresentMon SDK

## Getting Started

All installations of PresentMon Service from version 2.3.1 onwards will deploy PresentMonAPI2.dll together with the service during installation. Client applications should include the C header PresentMonAPI.h and dynamically load PresentMonAPI2.dll at runtime. If a client ships their own copy of PresentMonAPI2.dll, binary compatibility with the service will not be guaranteed.

We also provide a loader library to reduce the developer burden of manually loading the .dll and resolving endpoints. Applications using the loader can link to the PresentMonAPI2Loader.lib import library during build and deploy PresentMonAPI2Loader.dll with their application.

PresentMonAPI.h, PresentMonAPI2Loader.lib, and PresentMonAPI2Loader.dll are optionally installed together with the service, and can be found by default in Program Files\Intel\PresentMon\SDK.

## Diagnostics

All of the PresentMonAPI functions return an enum type PM_STATUS that indicates success/failure and can give a hint as to the cause of any failure. For more detailed diagnostic messages, refer to PresentMonDiagnostics.h found in the PresentMonAPI2 project directory.