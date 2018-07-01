#!/bin/bash
hcswif_dir=/home/jmatter/hcswif/
for f in `ls $hcswif_dir/output/runs_*Q2*.json`; do
    workflow_name=`cat $f | python -c 'import sys, json; print json.load(sys.stdin)["name"]'`
    swif cancel $workflow_name -delete
    echo
done
