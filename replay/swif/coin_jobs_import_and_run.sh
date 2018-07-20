#!/bin/bash
hcswif_dir=/home/jmatter/hcswif/
for f in `ls $hcswif_dir/output/runs_*Q2*.json`; do
    swif import -file $f
    echo Imported $f

    workflow_name=`cat $f | python -c 'import sys, json; print json.load(sys.stdin)["name"]'`
    swif run $workflow_name
    echo Submitted $workflow_name to run queue
    echo
done
