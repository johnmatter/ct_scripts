#!/bin/bash
# Each of these runs files contains the relevant run numbers.
# Workflow name will be the name of the run files, for consistency.
# Hopefully this will make re-replaying easy when changes are made
# to hcana or hallc_replay.


hcswif_dir=/home/jmatter/hcswif
runlist_dir=/home/jmatter/ct_scripts/runlists/
for f in `ls $runlist_dir/runs_*Q2_*`; do
    $hcswif_dir/hcswif.py --mode replay --spectrometer COIN --replay SCRIPTS/COIN/PRODUCTION/replay_production_coin_hElec_pProt.C --events -1 --project c-comm2017 --run file $f --name $f
done
