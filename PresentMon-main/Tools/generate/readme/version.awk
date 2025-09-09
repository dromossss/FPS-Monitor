BEGIN{ FS="[<>]" }
/PresentMonVersion/ {
    if ($2 == "PresentMonVersion")
        print $3
}
