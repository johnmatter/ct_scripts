#!/usr/bin/bash
ARGC=$#
script=$1
run=$2
evt=$3
xt=$4
xpt=$5
yt=$6
ypt=$7

# Setup environment
hcswif_dir=/home/jmatter/ctswif/
source $hcswif_dir/setup.sh

# Check environment
if ! [ $(command -v hcana) ]; then
    echo Could not find hcana! Please edit $hcswif_dir/setup.sh appropriately
    exit 1
fi

# Replay the run
runHcana="./hcana -q \"$script($run,$evt,$xt,$xpt,$yt,$ypt)\""
cd $hallc_replay_dir
echo pwd: $(pwd)
echo $runHcana
eval $runHcana
