#!/bin/bash
# Use this script to run a root macro in batch mode.
# Particular use case is for swif jobs on ifarm.

source /site/12gev_phys/softenv.sh 2.1

ct_setup=/home/jmatter/ct_scripts/setup.sh
source $ct_setup

hcana_dir=/home/jmatter/hcana
hcana_setup=$hcana_dir/setup.sh
source $hcana_setup
export PATH=$hcana_dir/bin:$PATH

analysis_dir=/home/jmatter/CT12GeV/ct_replay/
param_dir=/home/jmatter/CT12GeV/ct_replay/PARAM/SHMS/DC
rootfile_dir=/home/jmatter/CT12GeV/ct_replay/ROOTfiles/pass1

# Windows to analyze
windows=(1600 3200 4800 6400 8000)

# Runs to analyze
# run_numbers=(2054 2279 3188 2453 2283 2427 2191 2065 3181 3198 2441 2317 2358)
run_numbers=(2441)

# Replay script
replay_script=/home/jmatter/ct_scripts/analysis/efficiency/coin/shms_dc_tdc_window/replay_production_ct.C

# Loop over runs and windows, replaying each and mv-ing file for later analysis
for run_number in ${run_numbers[@]}; do
    for window in ${windows[@]}; do
        echo !JM
        echo Running for window $window

        # Link DC cut params
        cd $param_dir
        rm pdc_cuts.param
        ln -s pdc_cuts_window${window}.param pdc_cuts.param

        # Analyze
        cd $analysis_dir
        hcana -q -b "$replay_script($run_number,-1)"

        # Move root file
        cd $rootfile_dir
        mv coin_replay_production_${run_number}_-1.root pdc_time_window_study/coin_replay_production_${run_number}_-1_window_${window}.root
    done
done
