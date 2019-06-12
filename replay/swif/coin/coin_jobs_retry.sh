#!/bin/bash
json_dir=/home/jmatter/ct_scripts/replay/swif/coin/json
for f in `ls $json_dir/*.json`; do
    workflow_name=`cat $f | python -c 'import sys, json; print json.load(sys.stdin)["name"]'`
    swif retry-jobs -workflow $workflow_name -regexp ".*"
    echo
done
