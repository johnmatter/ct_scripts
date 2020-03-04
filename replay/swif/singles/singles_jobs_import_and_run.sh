#!/bin/bash
json_dir=/home/jmatter/ct_scripts/replay/swif/singles/json

for f in `ls $json_dir/*_runs_Al*.json`; do
    swif import -file $f
    echo Imported $f

    workflow_name=`cat $f | python -c 'import sys, json; print json.load(sys.stdin)["name"]'`
    swif run $workflow_name
    echo Submitted $workflow_name to run queue
    echo
done

for f in `ls $json_dir/*_runs_LH2*.json`; do
    swif import -file $f
    echo Imported $f

    workflow_name=`cat $f | python -c 'import sys, json; print json.load(sys.stdin)["name"]'`
    swif run $workflow_name
    echo Submitted $workflow_name to run queue
    echo
done

for f in `ls $json_dir/*_runs_C12*.json`; do
    # swif import -file $f
    echo Imported $f

    workflow_name=`cat $f | python -c 'import sys, json; print json.load(sys.stdin)["name"]'`
    # swif run $workflow_name
    echo Submitted $workflow_name to run queue
    echo
done
