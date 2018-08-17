#!/bin/bash
# Use this script to run a root macro in batch mode.
# Particular use case is for swif jobs on ifarm.

source /site/12gev_phys/production.sh 2.1

ct_setup=/home/jmatter/ct_scripts/setup.sh
source $ct_setup

hcana_dir=/home/jmatter/hcana
hcana_setup=$hcana_dir/setup.sh
source $hcana_setup
export PATH=$hcana_dir/bin:$PATH

cd /home/jmatter/ct_scripts/analysis/efficiency
hcana -q -b "$1++"
