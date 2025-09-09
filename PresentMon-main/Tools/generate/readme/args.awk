BEGIN{ FS="[()]" }
/LR"\(--/ {
    if ($3 == "\", nullptr,") {
        print ""
        printf "| %-30s |     |\n", substr($2, 3)
        print "| ------------------------------ | --- |"
    } else {
        gsub(/\. /, ".  ", $4)
        printf "| %-30s | %s |\n", "`" $2 "`", $4
    }
}
