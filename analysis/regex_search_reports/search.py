#!/apps/bin/python3
import os
import re
import sys

# This script searches for all matches of an input regex in
# hcana report outputs.

runlistDir = '/home/jmatter/ct_scripts/runlists/coin'
runlistRegex = 'runs_(.*)_Q2_(.*)'

reportDir = '/work/hallc/e1206107/CT12GeV/ct_replay/REPORT_OUTPUT/COIN/PRODUCTION/pass1'
reportTemplate = 'replay_coin_production_%d_-1.report'

# regexQuery = input("Enter regex to use as search: ")
regexQuery = sys.argv[1]

#------------------------------------------------------------------------------
def main():
    # Get all runlists in runlistDir
    # lists = os.listdir(runlistDir)

    # Define explicit list of runlists to process
    lists = ["runs_LH2_Q2_8", "runs_LH2_Q2_10",
             "runs_LH2_Q2_12", "runs_LH2_Q2_14",
             "runs_C12_Q2_8", "runs_C12_Q2_10",
             "runs_C12_Q2_12", "runs_C12_Q2_14"]

    # Loop over each runlist and store matches in a list
    for listname in lists:
        runlistFilename = os.path.join(runlistDir,listname)

        # Get a tuple like ('LH2', '12')
        kinematics = parseFilename(listname)
        if (kinematics == ('','')):
            print('Couldn''t parse runlist filename %s' % listname)
            continue

        target = kinematics[0]
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

        # Scan each run's report output for desired string and print it
        print('target,Q2,run,match')
        for run in runs:
            matches = parseReport(run, regexQuery)
            for match in matches:
                print('%s,%d,%d,%s' % (target,Q2,run,match))

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
