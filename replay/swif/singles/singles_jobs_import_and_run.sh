#!/bin/bash
json_dir=/home/jmatter/ct_scripts/replay/swif/singles/json
for f in `ls $json_dir/*.json`; do
    swif import -file $f
    echo Imported $f

    workflow_name=`cat $f | python -c 'import sys, json; print json.load(sys.stdin)["name"]'`
    swif run $workflow_name
    echo Submitted $workflow_name to run queue
    echo
done
