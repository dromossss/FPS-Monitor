# Contributing

## Requesting Features and Reporting Issues

We track feature requests and issues using GitHub Issues [here](https://github.com/GameTechDev/PresentMon/issues).  Clearly describe the issue/request, as well as the impact and priority from your perspective.

### If you are having an issue with the installer...

Provide an installer log:

1. Collect a log while running the installer: `msiexec /i PresentMon.msi /l*v PresentMonInstallerLog.txt`
2. Attach the resulting "PresentMonInstallerLog.txt" to the issue

### If PresentMon is crashing...

Provide a user-mode minidump of the crash:

1. Set up dump collection as described [here](https://docs.microsoft.com/en-us/windows/win32/wer/collecting-user-mode-dumps).
2. Re-run PresentMon in the scenario that crashes.
3. Provide the captured PresentMon-....exe.####.dmp file.

### If there is something wrong with the data PresentMon is reporting...

Provide an ETL trace:

1. Install the [Windows Performance Toolkit](https://www.google.com/search?q=windows+performance+toolkit+download&btnI) and ensure xperf.exe is in your path.
2. Start a capture by running `Tools\start_etl_collection.cmd` in the PresentMon repository as administrator.
3. Run the test scenario.
4. Stop the capture by running `Tools\stop_etl_collection.cmd` as administrator. The capture will be output to trace.etl in the working directory.  Try to capture for as short as possible to limit the size of the file, while still capturing the problem.
5. Test that the .etl capture exhibits the issue you are reporting, by running the PresentMon console application with "-etl_file trace.etl" (to read from the capture instead of the system).
6. Provide the resulting trace.etl file along with your report.

## Contributing Source Code

We accept contributions as pull requests on GitHub [here](https://github.com/GameTechDev/PresentMon/pulls). Clearly describe the pull request, as well as the impact and priority from your perspective.  Also, clearly describe each commit and limit the length of the commit message's first line to less than ~80 characters.

PresentMon is licensed under the terms in [LICENSE](https://github.com/GameTechDev/PresentMon/blob/main/LICENSE.txt). By contributing to the project, you agree to the license and copyright terms therein and release your contribution under these terms.

You must also certify that the contributions adhere to the requirements outlined in the following Developer Certificate of Origin:

```text
Developer Certificate of Origin
Version 1.1

Copyright (C) 2004, 2006 The Linux Foundation and its contributors.
660 York Street, Suite 102,
San Francisco, CA 94110 USA

Everyone is permitted to copy and distribute verbatim copies of this
license document, but changing it is not allowed.

Developer's Certificate of Origin 1.1

By making a contribution to this project, I certify that:

(a) The contribution was created in whole or in part by me and I
    have the right to submit it under the open source license
    indicated in the file; or

(b) The contribution is based upon previous work that, to the best
    of my knowledge, is covered under an appropriate open source
    license and I have the right under that license to submit that
    work with modifications, whether created in whole or in part
    by me, under the same open source license (unless I am
    permitted to submit under a different license), as indicated
    in the file; or

(c) The contribution was provided directly to me by some other
    person who certified (a), (b) or (c) and I have not modified
    it.

(d) I understand and agree that this project and the contribution
    are public and that a record of the contribution (including all
    personal information I submit with it, including my sign-off) is
    maintained indefinitely and may be redistributed consistent with
    this project or the open source license(s) involved.
```

To do so, each commit must be signed off by including a line like the following in your commit message (using your full legal name, and email address):

```text
Signed-off-by: Joe Smith <joe.smith@email.com>
```

If you set your `user.name` and `user.email` git config accordingly, this line will be added if you use `git commit -s`.

## How to add tracking for a new event

1. If this is a new provider, create a new header file in
   PresentData/ETW/_PROVIDER\_NAME_.h (otherwise modify the existing one) and
   add an `EVENT_DESCRIPTOR_DECL()` line to it for the new event.

   If you have the provider's manifest installed, or if you have an example
   .ETL file containing the event, then you can use the Tools/etw\_list tool to
   extract the information and you should also add the relevant command line to
   Tools/collect\_etw\_info.cmd to ensure the event persists through updates.
   e.g.:

    ```bat
    > msbuild Tools\etw_list
    > build\Debug\etw_list-dev-x64.exe --etl=TRACE.etl --provider=PROVIDER_NAME --event=* --no_event_structs
    ```

2. In PresentData/PresentMonTraceSession.cpp, modify `EnableProviders()` to add the new
   event to the provider before it is enabled.  If this is a new provider, you
   will also need to:

    1. Add the code to define the `FilteredProvider` and call `Enable()` on it.

    2. Modify `DisableProviders()` to disable it.

    3. Modify `EventRecordCallback()` to call a new handler for your provider.

3. In PresentData/PresentMonTraceConsumer.cpp, modify the provider's
   `Handle___()` function to handle the new event.  Typical code to do that
    looks like:

    ```cpp
    switch (hdr.EventDescriptor.Id) {
    case PROVIDER_NAMESPACE::EVENT_NAME::Id:
    {
        EventDataDesc desc[] = {
            { L"SomeEventPropertyName" },
            { L"AnotherPropertyName" },
            // ...
        };
        mMetadata.GetEventData(pEventRecord, desc, _countof(desc));
        auto SomeEventPropertyName = desc[0].GetData<PROPERTY1_TYPE>();
        auto AnotherPropertyName   = desc[1].GetData<PROPERTY2_TYPE>();
        // ...

        // ... Code to handle event ...
        break;
    }
    ```

    The properties associated with the event can be obtained from the manifest,
    or from first-hand knowledge about the event, or by stepping into the
    `mMetadata.GetEventData()` call and seeing what names/types of properties are
    iterated through.

    *"Code to handle event"* will typically involve looking up the right
    `PresentEvent` using one of `PMTraceConsumer`'s tracking data structures.
    Which structure to look the present up in depends on when your event is created
    among other factors.  `PMTraceConsumer::FindOrCreatePresent()` may be a good
    default, or at least an example of how you might need to look up the present.

If you need to add a column of data  to the output CSV:

1. In PresentMon/CsvOutput.cpp, modify `WriteCsvHeader()` and `UpdateCsv()`.
   It is important that the new column is added to the header and update
   functions in the same order (with respect to other existing columns).

2. In Tests/PresentMonTests.h, add a `Header` enum value and a case handling it
   to `GetHeaderString()`.

3. In Tests/PresentMon.cpp, add the new enum value to the appropriate
   `headerGroups` in `PresentMonCsv::Open()`.
