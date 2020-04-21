#!/bin/bash
# Each of these runs files contains the relevant run numbers.
# Workflow name will be the name of the run files, for consistency.
# Hopefully this will make re-replaying easy when changes are made
# to hcana or hallc_replay.

hcswif_dir=/home/jmatter/swif/ctswif/
runlist_dir=/home/jmatter/ct_scripts/runlists/carbon_boiling

f=$runlist_dir/coin_carbon_boiling
f_base=`basename $f`
$hcswif_dir/hcswif.py --mode replay --spectrometer COIN --replay SCRIPTS/COIN/PRODUCTION/replay_production_ct.C --events -1 --project c-comm2017 --run file $f --name $f_base

f=$runlist_dir/shms_carbon_boiling
f_base=`basename $f`
$hcswif_dir/hcswif.py --mode replay --spectrometer SHMS_PROD --replay SCRIPTS/SHMS/PRODUCTION/replay_production_shms.C --events -1 --project c-comm2017 --run file $f --name $f_base

f=$runlist_dir/hms_carbon_boiling
f_base=`basename $f`
$hcswif_dir/hcswif.py --mode replay --spectrometer HMS_PROD --replay SCRIPTS/HMS/PRODUCTION/replay_production_hms.C --events -1 --project c-comm2017 --run file $f --name $f_base

mv $hcswif_dir/output/*.json json
