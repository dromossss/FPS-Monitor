BEGIN{
    FS = "\"";
    print "| Column Header          | Description |";
    print "| ---------------------- | ----------- |";
}
{
    n = split($1, a, ",")

    #printf "\n%u:%u: ", NF, n
    #for (i = 1; i <= n; ++i) printf "|%s", a[i]
    #printf("|-")
    #for (i = 2; i <= NF; ++i) printf "|%s", $i
    #printf("|\n")

    if (a[1] == "") next
    if (substr(a[1], 1, 2) == "//") next
    if (a[2] == "1") next

    if (NF == 1) {
        name = a[3]
        desc = a[4]
    } else if (NF == 3) {
        name = substr(a[3], 1, length($1)-1)
        desc = $2
    }

    shortname = name
    gsub(/ /, "", shortname)
    gsub(/-/, "", shortname)

    if (shortname == "CPUStartTime") {
        shortname = "CPUStartTime<br>(CPUStartQPC)<br>(CPUStartQPCTime)<br>(CPUStartDateTime)"
        desc = desc "  By default, this is the time since recording started unless:<br>&bull; When `--qpc_time` is used, the value is a [performance counter value](https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter) and the column is named *CPUStartQPC*.<br>&bull; When `--qpc_time_ms` is used, the value is the query performance counter value converted to milliseconds and the column is named *CPUStartQPCTime*.<br>&bull; When `--date_time` is used, the value is a date and the column is named *CPUStartDateTime*."
    }

    if (shortname == "ClickToPhotonLatency") {
        desc = desc "  When supported HW measuring devices are not available, this is the software-visible subset of the full click-to-photon latency and doesn't include:<br>&bull; time spent processing input in the keyboard/controller hardware or drivers (typically a fixed additional overhead),<br>&bull; time spent processing the output in the display hardware or drivers (typically a fixed additional overhead), and<br>&bull; a combination of display blanking interval and scan time (which varies, depending on timing and tearing)."
    }

    printf "| %-22s | %s |\n", "*" shortname "*", desc
}
