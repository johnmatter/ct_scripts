#!/usr/bin/env python3
import glob
import os
import re

# Where are the reports to process
reportDir = "/lustre/expphy/volatile/hallc/comm2017/e1206107/REPORT_OUTPUT/COIN/PRODUCTION/pass2"

# Get list of reports
reportGlob = os.path.join(reportDir,"replay_coin_production_*_-1.report")

# print header

# Loop over reports
for reportFilename in glob.glob(reportGlob):
    # Grab run from filename
    m = re.search(r'replay_coin_production_(\d+)_-1.report',reportFilename)
    run = int(m.group(1))

    # Read report's text
    reportFile = open(reportFilename,'r')
    reportText = reportFile.read()
    reportFile.close()

    # Get values from report

    # SHMS TRACKING ----------------------------------------------------------------
    # There are two lines of the form:
    #   HADRON SING FID TRACK EFFIC    :   0.6519 +-   0.0129
    # The first should be the SHMS tracking
    m = re.findall(r"HADRON SING FID TRACK EFFIC\W*:\W*(.*)",reportText)[0]

    # Now split on the "+-" to get efficiency and its uncertainty
    m = re.split(r"\W*\+-\W*", m)
    pTrackEff = float(m[0])
    pTrackEffErr = float(m[1])

    # Also grab pscinshouh and pdidh
    # There should be only one match for each.
    # "pscinshouh" here is not a typo; it's a typo in the report template
    pscinshouldh = int(re.findall(r"pscinshouh\W*:\W*(.*)",reportText)[0])
    pdidh = int(re.findall(r"pdidh\W*:\W*(.*)",reportText)[0])

    # HMS TRACKING -----------------------------------------------------------------
    # There are two lines of the form:
    #   HADRON SING FID TRACK EFFIC    :   0.6519 +-   0.0129
    # The second should be the HMS tracking
    m = re.findall(r"HADRON SING FID TRACK EFFIC\W*:\W*(.*)",reportText)[1]

    # Now split on the "+-" to get efficiency and its uncertainty
    m = re.split(r"\W*\+-\W*", m)
    hTrackEff = float(m[0])
    hTrackEffErr = float(m[1])

    # Also grab hscinshoulde and hdide
    # There should be only one match for each.
    # "hscinshoulde" is spelled correctly in the report template
    hscinshoulde = int(re.findall(r"hscinshoulde\W*:\W*(.*)",reportText)[0])
    hdide = int(re.findall(r"hdide\W*:\W*(.*)",reportText)[0])

    # HMS CHERENKOV ----------------------------------------------------------------
    # There are two lines of the form:
    #   Overall HGC Efficiency: 0.714286 = 965 of 1351 good tracks.
    # The first should be SHMS HGC. The second should be HMS Cherenkov.
    m = re.findall(r"Overall HGC Efficiency:\W*(0\.*\d*) = (\d+) of (\d+) good tracks\.",reportText)
    if (len(m)==2):
        m=m[1]
        hCerEff = float(m[0])
        hCerDid = int(m[1])
        hCerShould = int(m[2])
    else:
        print("WARNING: found %d HGC eff lines for run %d" % (len(m),run))

    # HMS CALORIMETER --------------------------------------------------------------
    # There are lines of the form:
    # hA       Counts: 415 eff : 0.689157
    # hB       Counts: 415 eff : 0.742169
    # hC       Counts: 415 eff : 0.527711
    # hD       Counts: 415 eff : 0.060241
    m = re.findall(r"h[ABCD]\W*Counts: (\d+) eff : (\d+\.*\d*)",reportText)

    # Get values
    hCalTrkSum = []
    hCalHitSum = []
    hCalEff = [] # Report template gives eff=stat_hitsum/stat_trksum
    for blockMatch in m:
        hCalTrkSum.append(int(blockMatch[0]))
        hCalEff.append(float(blockMatch[1]))
        hCalHitSum.append( round(hCalEff[-1]*hCalTrkSum[-1]) )

    print(run,hCalTrkSum[0],hCalTrkSum[1],hCalTrkSum[2],hCalTrkSum[3])

    # HMS HODOSCOPE ----------------------------------------------------------------



