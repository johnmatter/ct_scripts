#!/usr/bin/env python3
import json
import copy
import glob
import os

# Where are the swif jsons to concatenate?
jsonDir = "/home/jmatter/ct_scripts/analysis/efficiency/coin/tracking/stub_linking/json"

# Output json filename
jsonOutputName = "ptrackstublink.json"
jsonOutputName = os.path.join(jsonDir, jsonOutputName)

# Create empty workflow and jobs list
workflow = {}
workflow['name'] = 'ptrackstublink'
jobs = []

# Loop over json files and add jobs to the job list
jsonGlob = os.path.join(jsonDir,"*.json")
for jsonFile in glob.glob(jsonGlob):
    with open(jsonFile, "r") as readFile:
        data = json.load(readFile)
        jobs.append(data['jobs'][0])

# Put list of jobs in workflow
workflow['jobs'] = jobs

# Write workflow
with open(jsonOutputName, "w") as writeFile:
        json.dump(workflow, writeFile)
