#!/usr/bin/env bash

# convert filename to "spectrometer,run#"
sed -i "s/\(.*\)_all_0\(.*\).xml/\1,\2/" epics.csv

# fix header
nlines=$(wc -l epics.csv | awk '{ print $1 }')
let nlines--

echo spectrometer,run,field,value > temp
cat epics.csv | tail -n $nlines >> temp

mv temp epics.csv
