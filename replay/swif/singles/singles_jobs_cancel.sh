#!/bin/bash
json_dir=/home/jmatter/ct_scripts/replay/swif/singles/json
for f in `ls $json_dir/*.json`; do
    workflow_name=`cat $f | python -c 'import sys, json; print json.load(sys.stdin)["name"]'`
    swif cancel $workflow_name -delete
    echo
done
