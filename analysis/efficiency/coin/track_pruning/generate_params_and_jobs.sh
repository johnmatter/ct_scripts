#!/bin/bash

#-------
# Max number of decimal points is 3, so for standardization, filenames
# will use 3 decimal points for all 4 parameters
# e.g. ptracking_10.000_0.325_7.00_0.025
#-------

hcswif_dir=/home/jmatter/ctswif/
param_dir=/home/jmatter/ct_scripts/analysis/efficiency/coin/track_pruning/PARAM/

swifScript=/home/jmatter/ct_scripts/analysis/efficiency/coin/track_pruning/run.sh
replayScript=/home/jmatter/ct_scripts/analysis/efficiency/coin/track_pruning/replay.C

runNumber=2054

# Ranges
xt_range=$(seq 0.5 0.5 20)
yt_range=$(seq 0.25 0.25 5)
xpt_range=$(seq 0.025 0.025 0.625)
ypt_range=$(seq 0.025 0.025 0.5)


# Fixed vals
x_fix=10
xp_fix=0.4
y_fix=7
yp_fix=0.2

# Scan x -------------
x=$x_fix; xp=$xp_fix; y=$y_fix; yp=$yp_fix
for x in $xt_range; do
    x=$(printf "%.3f\n" $x); xp=$(printf "%.3f\n" $xp); y=$(printf "%.3f\n" $y); yp=$(printf "%.3f\n" $yp);
    jobname="ptracking_xt${x}_xpt${xp}_yt${y}_ypt${yp}"
    paramfile="$param_dir/${jobname}.param"
    cat ptracking_template.param | sed -e "s/XT/$x/" -e "s/YT/$y/" -e "s/XPT/$xp/" -e "s/YPT/$yp/" > $paramfile
    $hcswif_dir/hcswif.py --mode command --command "${swifScript} ${replayScript} $runNumber -1 $x $xp $y $yp" --name $jobname --project c-comm2017
done

# Scan xp ------------
x=$x_fix; xp=$xp_fix; y=$y_fix; yp=$yp_fix
for xp in $xpt_range; do
    x=$(printf "%.3f\n" $x); xp=$(printf "%.3f\n" $xp); y=$(printf "%.3f\n" $y); yp=$(printf "%.3f\n" $yp);
    jobname="ptracking_xt${x}_xpt${xp}_yt${y}_ypt${yp}"
    paramfile="$param_dir/${jobname}.param"
    cat ptracking_template.param | sed -e "s/XT/$x/" -e "s/YT/$y/" -e "s/XPT/$xp/" -e "s/YPT/$yp/" > $paramfile
    $hcswif_dir/hcswif.py --mode command --command "${swifScript} ${replayScript} $runNumber -1 $x $xp $y $yp" --name $jobname --project c-comm2017
done

# Scan y -------------
x=$x_fix; xp=$xp_fix; y=$y_fix; yp=$yp_fix
for y in $yt_range; do
    x=$(printf "%.3f\n" $x); xp=$(printf "%.3f\n" $xp); y=$(printf "%.3f\n" $y); yp=$(printf "%.3f\n" $yp);
    jobname="ptracking_xt${x}_xpt${xp}_yt${y}_ypt${yp}"
    paramfile="$param_dir/${jobname}.param"
    cat ptracking_template.param | sed -e "s/XT/$x/" -e "s/YT/$y/" -e "s/XPT/$xp/" -e "s/YPT/$yp/" > $paramfile
    $hcswif_dir/hcswif.py --mode command --command "${swifScript} ${replayScript} $runNumber -1 $x $xp $y $yp" --name $jobname --project c-comm2017
done

# Scan yp ------------
x=$x_fix; xp=$xp_fix; y=$y_fix; yp=$yp_fix
for yp in $ypt_range; do
    x=$(printf "%.3f\n" $x); xp=$(printf "%.3f\n" $xp); y=$(printf "%.3f\n" $y); yp=$(printf "%.3f\n" $yp);
    jobname="ptracking_xt${x}_xpt${xp}_yt${y}_ypt${yp}"
    paramfile="$param_dir/${jobname}.param"
    cat ptracking_template.param | sed -e "s/XT/$x/" -e "s/YT/$y/" -e "s/XPT/$xp/" -e "s/YPT/$yp/" > $paramfile
    $hcswif_dir/hcswif.py --mode command --command "${swifScript} ${replayScript} $runNumber -1 $x $xp $y $yp" --name $jobname --project c-comm2017
done
