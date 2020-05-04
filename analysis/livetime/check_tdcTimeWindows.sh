#!/bin/bash

paramFile=/home/jmatter/ct_replay/PARAM/TRIG/tcoin.param

field="t_coin_TdcTimeWindowMin"
line=$(awk -v RS= "/$field/" $paramFile | tr -d "\n" | sed "s/$field = //")
IFS=', ' read -r -a tdcWindowMin <<<  "$line"

field="t_coin_TdcTimeWindowMax"
line=$(awk -v RS= "/$field/" $paramFile | tr -d "\n" | sed "s/$field = //")
IFS=', ' read -r -a tdcWindowMax <<<  "$line"

field="t_coin_tdcNames"
line=$(awk "/$field/" $paramFile | sed -e "s/$field = //" -e "s/\"//g")
IFS=', ' read -r -a tdcName <<<  "$line"

let numTdcs=${#tdcWindowMin[@]}-1
printf "%-15s %8s %8s\n" name min max
echo ---------------------------------
for n in $(seq 0 $numTdcs); do
     printf "%-15s %8d %8d\n" ${tdcName[n]} ${tdcWindowMin[n]} ${tdcWindowMax[n]}
done
