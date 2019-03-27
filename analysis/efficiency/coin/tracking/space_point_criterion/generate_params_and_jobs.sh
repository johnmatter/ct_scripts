#!/bin/bash

#-------
# Filenames will use 3 decimal points parameters
# e.g. ptracking_spacepoint1.200
#-------

hcswif_dir=/home/jmatter/ctswif/
param_dir=/home/jmatter/ct_scripts/analysis/efficiency/coin/tracking/space_point_criterion/PARAM/

swifScript=/home/jmatter/ct_scripts/analysis/efficiency/coin/tracking/space_point_criterion/run.sh
replayScript=/home/jmatter/ct_scripts/analysis/efficiency/coin/tracking/space_point_criterion/replay.C

runNumber=2054

# Ranges
spacepoint_range=$(seq 0.000 0.125 4)

for x in $spacepoint_range; do
    x=$(printf "%.3f\n" $x)
    jobname="ptracking_spacepoint${x}"
    paramfile="$param_dir/${jobname}.param"
    cat ptracking_template.param | sed "s/SPCRIT/$x/g" > $paramfile
    $hcswif_dir/hcswif.py --mode command --command "${swifScript} ${replayScript} $runNumber -1 $x" --name $jobname --project c-comm2017
done
