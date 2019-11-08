#!/bin/bash

list=$1

while read line
do

    fname=/home/jmatter/ROOTfiles/pass3_with_edtm_decode/coin_replay_production_${line}_-1.root
    calib_directory=/home/jmatter/CT12GeV/ct_replay/CALIBRATION/bcm_current_map
    calib_script=$calib_directory/run.C


    echo Processing ${fname}

    root -l -b -q "$calib_script(\"${fname}\", \"P\")"

    [ ! -d "coin" ] && `mkdir coin`

    mv bcmcurrent_${line}.param ./coin/

    echo Created bcmcurrent_${line}.param

done < ${list}
