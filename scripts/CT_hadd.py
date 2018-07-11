#!/usr/bin/env python3
import os
import itertools
import subprocess

# CT kinematics
Q2s = [8,10,12,14]
targets = ['LH2', 'C12', 'AL']

# Where are the lists of run numbers by kinematics?
runlistDir = '/home/jmatter/ct_scripts/runlists'

# Run list filename template
runlistTemplate = 'runs_%s_Q2_%d'

# Where are your hcana-replayed root files?
rootfileDir = '/home/jmatter/hallc_replay/ROOTfiles'

# What's the input root filename template?
rootfileInputTemplate = 'coin_replay_production_%d_-1.root'

# Output root filename template
rootfileOutputTemplate = 'CT_coin_%s_%d.root'


# Loop over combinations of target and Q^2
kinematics = list(itertools.product(targets, Q2s))
for k in kinematics:
    # Form runlist filename and read it
    runlist = runlistTemplate % k
    runlist = os.path.join(runlistDir,runlist)
    f = open(runlist,'r') 
    lines = f.readlines()

    # Read each line and add to list
    runs = []
    for line in lines:
        run = line.strip('\n')
        if len(run)>0:
            runs.append(int(run))

    # Generate list of input files for hadd
    inputRootfiles = []
    for run in runs:
        filename = os.path.join(rootfileDir,rootfileInputTemplate % run) 
        inputRootfiles.append(filename)

    # Form output file for hadd
    outputRootfile = rootfileOutputTemplate % k
    outputRootfile = os.path.join(rootfileDir, outputRootfile)

    # Run hadd
    subprocess.call(['hadd'] + [outputRootfile] + inputRootfiles)
