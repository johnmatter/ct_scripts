#!/apps/bin/python3
import os
import re
import sys

# This script searches for all matches of an input regex in
# hcana report outputs.

runlistDir = '/home/jmatter/ct_scripts/runlists/coin'

reportDir = '/home/jmatter/ct_replay/REPORT_OUTPUT/COIN/PRODUCTION/'
reportTemplate = 'replay_coin_production_%d_-1.report'

# regexQuery = input("Enter regex to use as search: ")
regexQuery = sys.argv[1]

#------------------------------------------------------------------------------
def main():
    # Define explicit list of runlists to process
    lists = [
             "runs_LH2_Q2_8",
             "runs_LH2_Q2_10_pion_collimator",
             "runs_LH2_Q2_10_large_collimator",
             "runs_LH2_Q2_12",
             "runs_LH2_Q2_14_large_collimator",
             "runs_LH2_Q2_14_pion_collimator",
             "runs_C12_Q2_8_thin",
             "runs_C12_Q2_8_thick",
             "runs_C12_Q2_10_thin",
             "runs_C12_Q2_10_thick",
             "runs_C12_Q2_12",
             "runs_C12_Q2_14_pion_collimator",
             "runs_C12_Q2_14_large_collimator"
            ]

    # Loop over each runlist and store matches in a list
    print('list,run,match')
    for listname in lists:
        runlistFilename = os.path.join(runlistDir,listname)

        # Read list of runs from runlist
        runlistF = open(runlistFilename,'r')
        lines = runlistF.readlines()
        runs=[]
        for line in lines:
            run = line.strip('\n')
            if len(run)>0:
                runs.append(int(run))

        # Scan each run's report output for desired string and print it
        for run in runs:
            matches = parseReport(run, regexQuery)
            for match in matches:
                print('%s,%d,%s' % (listname,run,match))

#------------------------------------------------------------------------------
def parseReport(run, regexQuery):
    # Read file
    reportFilename = reportTemplate % run
    reportFilename = os.path.join(reportDir, reportFilename)
    try:
        reportF = open(reportFilename,'r')
    except:
        return ['no report found']

    lines = reportF.readlines()

    # Generate list of matches
    matches = []
    for line in lines:
        m = re.match(regexQuery, line)
        if m!=None:
            matches.append(re.sub('\n','',m.string))

    if len(matches)==0:
        matches = ['no match']

    return matches

#------------------------------------------------------------------------------
if __name__ == "__main__":
    main()
