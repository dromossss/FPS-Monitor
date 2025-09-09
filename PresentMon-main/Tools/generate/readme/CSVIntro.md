## Comma-separated value (CSV) file output

### CSV file names

By default, PresentMon creates a CSV file named "PresentMon-\<Time>.csv", where "\<Time>" is the
creation time in ISO 8601 format.  To specify your own output location, use the `--output_file PATH`
command line argument.

If `--multi_csv` is used, then one CSV is created for each process captured and
"-\<ProcessName>-\<ProcessId>" is appended to the file name.

If `--hotkey` is used, then one CSV is created for each time recording is started and "-\<Index>" is
appended to the file name.

### CSV columns

Each row of the CSV represents a frame that an application rendered and presented to the system for
display, and each column contains a particular metric value associated with that frame.  All time values
are in milliseconds.

Default metrics include:
