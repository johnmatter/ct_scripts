#!/bin/bash

# First get efficiency
./search.py SHMS_3_of_4 | awk 'NR>2 {print last} {last=$0}' | sed "s/_3_of_4 EFF : /,/" > SHMS_3_of_4_matches.txt
./search.py HMS_3_of_4  | awk 'NR>2 {print last} {last=$0}' | sed "s/_3_of_4 EFF : /,/" > HMS_3_of_4_matches.txt

cat SHMS_3_of_4_matches.txt > hodoscope.csv
cat HMS_3_of_4_matches.txt >> hodoscope.csv

cat hodoscope.csv | awk '{sub(/[^,]*/,"");sub(/,/,"")} 1' > hodoscopefix.csv
mv hodoscopefix.csv hodoscope.csv

# Next get number of clean tracks for weighting
./search.py pscleantrack | awk 'NR>2 {print last} {last=$0}' | sed "s/pscleantrack\s*:\s*/SHMS,/" > pscleantrack_matches.txt
./search.py hscleantrack | awk 'NR>2 {print last} {last=$0}' | sed "s/hscleantrack\s*:\s*/HMS,/" > hscleantrack_matches.txt

cat hscleantrack_matches.txt > cleantrack.csv
cat pscleantrack_matches.txt >> cleantrack.csv

cat cleantrack.csv | awk '{sub(/[^,]*/,"");sub(/,/,"")} 1' > cleantrackfix.csv
mv cleantrackfix.csv scleantrack.csv

# Add headers
sed -i "1s/^/run,spectrometer,efficiency\n/" hodoscope.csv
sed -i "1s/^/run,spectrometer,scleantracks\n/" scleantrack.csv
