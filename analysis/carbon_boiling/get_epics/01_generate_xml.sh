#!/usr/bin/env bash

# Create directory for xml output if it doesn't exist
[ ! -d "evioxml" ] && mkdir evioxml

filelist=/home/jmatter/ct_scripts/analysis/carbon_boiling/get_epics/filelist

nfiles=$(wc -l $filelist | cut -f 1 -d\ )
echo $nfiles

let n=1
for file in $(cat $filelist); do
    printf "%-25s    %3d / %3d\n" $(basename $file) $n $nfiles
    evio2xml -max 1 -ev 180 $file > evioxml/$(basename $file .dat).xml
    let n++
done
