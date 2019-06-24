#!/apps/bin/python3
import os
import re
import pandas as pd
import numpy as np
import matplotlib.cm as cm
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.backends.backend_pdf import PdfPages
from itertools import product

# This script reads the efficiency info contained in hcana report output.
# These values are calculated during replay by the THcHodoEff class.
# WARNING: This script makes substantial assumptions about the format of your reports.

runlistDir = '/home/jmatter/ct_scripts/runlists/coin'
runlistRegex = 'runs_(.*)_Q2_(.*)'

reportDir = '/work/hallc/e1206107/CT12GeV/ct_replay/REPORT_OUTPUT/COIN/PRODUCTION/pass3'
reportTemplate = 'replay_coin_production_%d_-1.report'

regexTracksFound = 'Tracks found    = scleantrack = (.*)'
regexHodoEff = '3_of_4 EFF : (.*)'

#------------------------------------------------------------------------------
def main():
    # Get all runlists in runlistDir
    # lists = os.listdir(runlistDir)

    # Set explicit list of runlists to process
    lists = ["runs_LH2_Q2_8", "runs_LH2_Q2_10",
             "runs_LH2_Q2_12", "runs_LH2_Q2_14",
             "runs_C12_Q2_8", "runs_C12_Q2_10",
             "runs_C12_Q2_12", "runs_C12_Q2_14"]

    # Loop over lists and store info in a list of dictionaries.
    # Each run has a dictionary with efficiency, Q^2, etc.
    listOfDicts = []
    for listname in lists:
        runlistFilename = os.path.join(runlistDir,listname)

        print("Reading " + runlistFilename)

        # Get a tuple like ('LH2', 12)
        kinematics = parseFilename(listname)
        if (kinematics == ('','')):
            continue

        target  = kinematics[0]
        Q2 = int(kinematics[1])

        # Skip dummy target data
        if (target=='AL'):
            continue

        # Read list of runs from runlist
        runlistF = open(runlistFilename,'r')
        lines = runlistF.readlines()
        runs=[]
        for line in lines:
            run = line.strip('\n')
            if len(run)>0:
                runs.append(int(run))

        print("# runs = " + str(len(runs)))

        # Scan each report's output for hodoscope efficiency lines and store it
        for run in runs:
            results = parseReport(run)
            thisRun = {
                    'run'         : run,
                    'target'      : target,
                    'Q2'          : Q2,
                    'pTracks'     : results[0],
                    'pEfficiency' : results[1],
                    'hTracks'     : results[2],
                    'hEfficiency' : results[3]
                    }
            listOfDicts.append(thisRun)

    print("Checking for numbers too large for pd")
    # Safety check for large numbers
    listOfDicts = checkForLarge(listOfDicts)

    print("Creating dataframe")
    # Create dataframe
    colNames = ['run', 'target', 'Q2', 'pTracks', 'pEfficiency', 'hTracks', 'hEfficiency']
    df  = pd.DataFrame(listOfDicts, columns = colNames)

    # Clean df
    df = df[df['pTracks'].notna()]

    # Print, plot, etc.

    print("Writing to csv")
    df.to_csv("hodoEfficiency.csv")

    # Create label for each kinematics
    targets = df['target'].unique()
    Q2s = df['Q2'].unique()
    kinematics = product(targets, Q2s) # generate all (target, Q2) combos
    scatterGroup=0 # used for plotting
    for k in kinematics:
        # Kinematics string label e.g. LH2_Q2_8
        kString = '%s_Q2_%d' % k

        # Find this kinematics' rows
        kIdx = (df['target']==k[0]) & (df['Q2']==k[1])

        # Set label and integer group for scattter plot
        df.loc[kIdx, 'kinematics'] = kString
        df.loc[kIdx, 'scatterGroup'] = scatterGroup

        scatterGroup+=1

    print("Writing to pdf")
    # Scatter of efficiency versus run number
    fP = scatterplotByRun(df, 'pEfficiency', 'SHMS 3/4 Efficiency')
    fH = scatterplotByRun(df, 'hEfficiency', 'HMS 3/4 Efficiency')

    pp = PdfPages('hodoEfficiency.pdf')
    pp.savefig(fP)
    pp.savefig(fH)
    pp.close()

#------------------------------------------------------------------------------
def scatterplotByRun(df, field, title):
    f = plt.figure()
    legendHandles=[]
    for n in range(8):
        # Select this kinematics
        subset = df[df['scatterGroup']==n]
        if (subset.index.empty):
            print('Empty subset for scatterGroup=%d' % n)
            continue

        # Formatting
        color = cm.Set1(n)

        # Plot
        plt.scatter(subset['run'], subset[field], s=3, c=color)

        # Manual legend so we avoid listing every run
        kString = subset.loc[subset.index[0],'kinematics'] # first item; others should be same
        mpatch = mpatches.Patch(color=color, label=kString)
        legendHandles.append(mpatch)

    plt.legend(handles=legendHandles)
    plt.xlabel('Run Number')
    plt.ylabel('Efficiency')
    plt.title(title)
    plt.ylim(0.9,1.01)

    return f

#------------------------------------------------------------------------------
def parseFilename(filename):
    # Return a tuple of capture groups
    try:
        matches = re.match(runlistRegex, filename).groups()
        return matches
    except:
        print('Filename %s doesn\'t match regex: %s' % (filename, runlistRegex))
        return ('','')

#------------------------------------------------------------------------------
def parseReport(run):
    # Read file
    reportFilename = reportTemplate % run
    reportFilename = os.path.join(reportDir, reportFilename)
    try:
        reportF = open(reportFilename,'r')
    except:
        print('Couldn\'t find report for run %d' % run)
        return (np.nan, np.nan, np.nan, np.nan)

    lines = reportF.readlines()

    # We assume SHMS is listed first and HMS second
    numMatchesTracksFound=0
    numMatchesHodoEff=0
    for line in lines:
        m = re.match(regexTracksFound, line)
        if m!=None:
            numMatchesTracksFound+=1
            if numMatchesTracksFound==1:
                SHMSTracksFound = m.groups(0)[0]
            elif numMatchesTracksFound==2:
                HMSTracksFound = m.groups(0)[0]
            elif numMatchesTracksFound>4:
                print('Too many "tracks found" matches! Run %d' % run)

        m = re.match(regexHodoEff, line)
        if m!=None:
            numMatchesHodoEff+=1
            if numMatchesHodoEff==1:
                SHMSHodoEff = m.groups(0)[0]
            elif numMatchesHodoEff==2:
                HMSHodoEff = m.groups(0)[0]
            elif numMatchesHodoEff>2:
                print('Too many 3of4 matches! Run %d' % run)

    try:
        SHMSTracksFound = int(float(SHMSTracksFound))
    except NameError:
        print('Error processing run %d' % run)
        return (np.nan, np.nan, np.nan, np.nan)

    try:
        HMSTracksFound = int(float(HMSTracksFound))
    except NameError:
        print('Error processing run %d' % run)
        return (np.nan, np.nan, np.nan, np.nan)

    try:
        SHMSHodoEff = float(SHMSHodoEff)
    except NameError:
        print('Error processing run %d' % run)
        return (np.nan, np.nan, np.nan, np.nan)

    try:
        HMSHodoEff = float(HMSHodoEff)
    except NameError:
        print('Error processing run %d' % run)
        return (np.nan, np.nan, np.nan, np.nan)


    return (SHMSTracksFound, SHMSHodoEff, HMSTracksFound, HMSHodoEff)

#------------------------------------------------------------------------------
def checkForLarge(listOfDicts):
    # Get the keys listed in the first dictionary; assume all other have same keys
    keys = list(listOfDicts[0].keys())

    # This is pandas' upper limit for ints
    largeInt = pow(2,64)

    # Loop over dictionaries and set "large" values to be NaN
    for n in range(len(listOfDicts)):
        for k in keys:
            val = listOfDicts[n][k]
            if (isinstance(val, (int,float)) and val>=largeInt):
                run = listOfDicts[n]['run']
                print('Large number for run %d, %s = %f' % (run,k,val))
                listOfDicts[n][k] = np.nan

    return listOfDicts

#------------------------------------------------------------------------------
if __name__ == "__main__":
    main()
