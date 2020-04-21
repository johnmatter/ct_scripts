#!/usr/bin/env bash

xmldir=evioxml
output=epics.csv

declare -a fields=("ecSHMS_Angle"
                   "ecSP_SHMS"
                   "SHMS_Momentum"
                   "ecSDI_Set_Current"
                   "ecHMS_Angle"
                   "ecP_HMS"
                   "HMS_Momentum"
                   "ecDI_Set_Current"
                   "hcBDSPOS"
                  )

# Header
echo file,field,value > $output

nfiles=$(ls -l $xmldir/*xml | wc -l)
let n=1

for file in $(ls $xmldir); do
    printf "%-25s    %3d / %3d\n" $(basename $file) $n $nfiles
    for field in ${fields[@]}; do
        value=$(grep $field $xmldir/$file | awk '{ print $2 }')
        # Check if value is numeric. If not, make it NA for r analysis
        check=`echo "$value" | grep -E ^\-?[0-9]*\.?[0-9]+\(....\)*$` 
        if [ "$check" =  '' ]; then
            echo $field=$value not numerical
            value="NA"
        fi
        echo $file,$field,$value >> $output
    done
    let n++
done
