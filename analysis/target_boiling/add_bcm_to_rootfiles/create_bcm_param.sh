#!/bin/bash

list=$1

while read line
do

    fname=/home/jmatter/ROOTfiles/pass5/shms_replay_production_all_${line}_1000000.root
    calib_directory=/home/jmatter/CT12GeV/ct_replay/CALIBRATION/bcm_current_map
    calib_script=$calib_directory/run.C


    echo Processing ${fname}

    root -l -b -q "$calib_script(\"${fname}\", \"P\")"

    [ ! -d "shms" ] && `mkdir shms`

    # Use this one if you're using the 'all' replay script
    # The BCM current map script indexes the filenames uncarefully
    mv bcmcurrent_all_.param ./shms/bcmcurrent_${line}.param

    # Use this is you're not using the 'all' replay script
    # mv bcmcurrent_${line}.param ./shms/


    echo Created bcmcurrent_${line}.param

done < ${list}
