#!/usr/bin/env python3
import os
import subprocess

#-------------------------------------------------------------------------------
# hadd all files for CT kinematics
# Run hadd_filecheck.py before this script
#-------------------------------------------------------------------------------

# Where are the lists of run numbers by kinematics?
runlistDir = '/home/jmatter/ct_scripts/runlists/coin'

# Run lists to process, and the filename to use for the hadd-ed files
haddFilename = {}
haddFilename['runs_C12_Q2_8_thick']             = 'coin_replay_production_C12_thick_8_smallcoll.root'
haddFilename['runs_C12_Q2_8_thin']              = 'coin_replay_production_C12_thin_8_smallcoll.root'
haddFilename['runs_C12_Q2_10_thick']            = 'coin_replay_production_C12_thick_9.5_largecoll.root'
haddFilename['runs_C12_Q2_10_thin']             = 'coin_replay_production_C12_thin_9.5_largecoll.root'
haddFilename['runs_C12_Q2_12']                  = 'coin_replay_production_C12_thick_11.5_largecoll.root'
haddFilename['runs_C12_Q2_14_large_collimator'] = 'coin_replay_production_C12_thick_14.3_largecoll.root'
haddFilename['runs_C12_Q2_14_pion_collimator']  = 'coin_replay_production_C12_thick_14.3_smallcoll.root'

haddFilename['runs_LH2_Q2_8']                   = 'coin_replay_production_LH2_8_smallcoll.root'
haddFilename['runs_LH2_Q2_10_large_collimator'] = 'coin_replay_production_LH2_9.5_largecoll.root'
haddFilename['runs_LH2_Q2_10_pion_collimator']  = 'coin_replay_production_LH2_9.5_smallcoll.root'
haddFilename['runs_LH2_Q2_12']                  = 'coin_replay_production_LH2_11.5_largecoll.root'
haddFilename['runs_LH2_Q2_14_large_collimator'] = 'coin_replay_production_LH2_14.3_largecoll.root'
haddFilename['runs_LH2_Q2_14_pion_collimator']  = 'coin_replay_production_LH2_14.3_smallcoll.root'

# Where are your hcana-replayed root files?
rootfileDir = '/home/jmatter/ct_replay/ROOTfiles/'

# What's the input root filename template?
rootfileInputTemplate = 'coin_replay_production_%d_-1.root'

#-------------------------------------------------------------------------------
# Loop over kinematics and hadd rootfiles

# Each key in haddFilename is a runlist.
for key in haddFilename:
    # Form runlist filename and read it
    runlist = os.path.join(runlistDir, key)
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
    outputRootfile = os.path.join(rootfileDir, haddFilename[key])

    # Run hadd
    subprocess.call(['hadd'] + [outputRootfile] + inputRootfiles)
