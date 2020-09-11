#!/bin/bash
# Each of these runs files contains the relevant run numbers.
# Workflow name will be the name of the run files, for consistency.
# Hopefully this will make re-replaying easy when changes are made
# to hcana or hallc_replay.

hcswif_dir=/home/jmatter/swif/ctswif/
runlist_dir=/home/jmatter/ct_scripts/runlists/target_boiling

f=$runlist_dir/shms_lh2_boiling
f_base=`basename $f`
$hcswif_dir/hcswif.py --mode replay --track debug --spectrometer SHMS_PROD --replay SCRIPTS/SHMS/PRODUCTION/replay_production_all_shms.C --events 10000 --project c-comm2017 --run file $f --name ${f_base} --ram 7000000000

f=$runlist_dir/hms_lh2_boiling
f_base=`basename $f`
$hcswif_dir/hcswif.py --mode replay --track debug --spectrometer HMS_PROD --replay SCRIPTS/HMS/PRODUCTION/replay_production_all_hms.C --events 10000 --project c-comm2017 --run file $f --name ${f_base} --ram 7000000000

f=$runlist_dir/shms_ld2_boiling
f_base=`basename $f`
$hcswif_dir/hcswif.py --mode replay --track debug --spectrometer SHMS_PROD --replay SCRIPTS/SHMS/PRODUCTION/replay_production_all_shms.C --events 10000 --project c-comm2017 --run file $f --name ${f_base} --ram 7000000000

f=$runlist_dir/hms_ld2_boiling
f_base=`basename $f`
$hcswif_dir/hcswif.py --mode replay --track debug --spectrometer HMS_PROD --replay SCRIPTS/HMS/PRODUCTION/replay_production_all_hms.C --events 10000 --project c-comm2017 --run file $f --name ${f_base} --ram 7000000000

f=$runlist_dir/shms_carbon_boiling
f_base=`basename $f`
$hcswif_dir/hcswif.py --mode replay --track debug --spectrometer SHMS_PROD --replay SCRIPTS/SHMS/PRODUCTION/replay_production_all_shms.C --events 10000 --project c-comm2017 --run file $f --name ${f_base} --ram 7000000000

f=$runlist_dir/hms_carbon_boiling
f_base=`basename $f`
$hcswif_dir/hcswif.py --mode replay --track debug --spectrometer HMS_PROD --replay SCRIPTS/HMS/PRODUCTION/replay_production_all_hms.C --events 10000 --project c-comm2017 --run file $f --name ${f_base} --ram 7000000000

f=$runlist_dir/coin_carbon_boiling
f_base=`basename $f`
$hcswif_dir/hcswif.py --mode replay --track debug --spectrometer COIN --replay SCRIPTS/COIN/PRODUCTION/replay_production_ct.C --events 10000 --project c-comm2017 --run file $f --name ${f_base} --ram 7000000000

mv $hcswif_dir/output/*.json json
