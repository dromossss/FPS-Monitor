# Intel-PresentMon Provider

This directory contains the Intel-PresentMon ETW provider: a mechanism for applications, libraries,
and drivers to communicate directly with PresentMon while it is running.

## Using the provider

Copy and include the following source files in your project, then follow the usage instructions in
the PresentMonProvider.h comments:

- PresentMonProvider.h
- PresentMonProvider.cpp

## Installing the provider manifest

Installing the manifest is not required for the provider to create and send events to PresentMon.
However, installing the manifest will describe the events to the system and to other ETW tools such
as gpuview or Windows Performance Analyzer.

The provider can be installed by the Intel PresentMon installer.

To install the provider without installing Intel PresentMon, run `install_provider.cmd` as
Administrator.

To uninstall the manifest, run `uninstall_provider.cmd` as Administrator.

## Debugging events

In order to view the generated events independently from PresentMon, you can capture an ETW Trace
Log (ETL) file and view it in a tool like Windows Performance Analyzer.

To capture an ETL that includes Intel-PresentMon Provider events, either use the collection scripts
provided in PresentMon's Tools/ directory:

```bat
> Tools\start_etl_collection.cmd
> ...
> Tools\stop_etl_collection.cmd
```

or, you can use Windows Performance Toolkit's Xperf specifying the Intel-PresentMon Provider GUID:

```bat
> Xperf.exe -start NAME -on ECAA4712-4644-442F-B94C-A32F6CF8A499 -f FILE.etl
> ...
> Xperf.exe -stop NAME
```
