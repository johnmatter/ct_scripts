#!/bin/bash
# Use this script to run a root macro in batch mode.
# Particular use case is for swif jobs on ifarm.

# JLab CUE
source /site/12gev_phys/softenv.sh 2.3
unset SCONSFLAGS

# Need access to my CTData and CTCuts classes
ct_setup=/home/jmatter/ct_scripts/setup.sh
source $ct_setup

# Need hcana
hcana_dir=/home/jmatter/hcana
hcana_setup=$hcana_dir/setup.sh
source $hcana_setup
export PATH=$hcana_dir/bin:$PATH

# Need some kind of replay
replay_dir=/home/jmatter/CT12GeV/ct_replay
replay_setup=$replay_dir/setup.sh
source $replay_setup

# Run analysis in the directory that our script lives in
analysis_dir=$(dirname $1)
echo cd to $analysis_dir
cd $analysis_dir
hcana -q -b "$1++"
