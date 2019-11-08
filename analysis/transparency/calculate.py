#!/apps/bin/python3
import pandas as pd
from warnings import warn


# TODO:
# - Replace yield calculation with goodevents and charge. Goodevents exists. Need to write charge script.

#------------------------------------------------------------------------------
def main():
    # List of kinematics we're interested in
    kinematics = []
    keys = ['Q2', 'target', 'collimator']

    kinematics.append(dict(zip(keys, [8,    'LH2',       'pion'])))
    kinematics.append(dict(zip(keys, [9.5,  'LH2',       'pion'])))
    kinematics.append(dict(zip(keys, [9.5,  'LH2',       'large'])))
    kinematics.append(dict(zip(keys, [11.5, 'LH2',       'large'])))
    kinematics.append(dict(zip(keys, [14.3, 'LH2',       'large'])))
    kinematics.append(dict(zip(keys, [14.3, 'LH2',       'pion'])))
    kinematics.append(dict(zip(keys, [8,    'C12_thin',  'pion'])))
    kinematics.append(dict(zip(keys, [9.5,  'C12_thin',  'large'])))
    kinematics.append(dict(zip(keys, [8,    'C12_thick', 'pion'])))
    kinematics.append(dict(zip(keys, [9.5,  'C12_thick', 'large'])))
    kinematics.append(dict(zip(keys, [11.5, 'C12_thick', 'large'])))
    kinematics.append(dict(zip(keys, [14.3, 'C12_thick', 'pion'])))
    kinematics.append(dict(zip(keys, [14.3, 'C12_thick', 'large'])))

    # csvs necessary for processing
    csvs = {}
    csvs['pid']        = '/home/jmatter/ct_scripts/analysis/efficiency/coin/delta_weighted/ct_event_weighted_efficiency.csv'
    csvs['hTracking']  = '/home/jmatter/ct_scripts/analysis/efficiency/coin/tracking/HMS_tracking.csv'
    csvs['pTracking']  = '/home/jmatter/ct_scripts/analysis/efficiency/coin/tracking/SHMS_tracking.csv'
    csvs['yield']      = '/home/jmatter/ct_scripts/analysis/transparency/yield.csv'
    csvs['goodevents'] = '/home/jmatter/ct_scripts/analysis/transparency/goodevents.csv'
    csvs['absorption'] = '/home/jmatter/ct_scripts/analysis/transparency/absorption.csv'
    csvs['simc']       = '/home/jmatter/ct_scripts/analysis/transparency/simc.csv'
    csvs['livetime']   = '/home/jmatter/ct_scripts/analysis/transparency/livetime.csv'

    # Read csvs into a Pandas dataframe
    ctData = loadCsvs(csvs)

    # Filter on kinematics
    ctData = filterDataframe(ctData, kinematics)

    # Calculate transparency from read data
    ctData = calculateRatio(ctData)

    # Calculate transparency from read data
    # ctData = calculateTransparency(ctData)

    print(ctData)

    # Write transparency to csv for plotting in another application
    writeCsv(ctData, "transparency.csv")

#------------------------------------------------------------------------------
def loadCsvs(csvs):

    #------------------------
    # Start with simc data
    ctData = pd.read_csv(csvs['simc'])

    #------------------------
    # Add yield
    ctData = ctData.merge(pd.read_csv(csvs['yield']), how='inner', on=['Q2', 'target', 'collimator'])

    #------------------------
    # Add absorption
    ctData = ctData.merge(pd.read_csv(csvs['absorption']), how='inner', on=['Q2', 'target', 'collimator'])

    #------------------------
    # Add livetime
    ctData = ctData.merge(pd.read_csv(csvs['livetime']), how='inner', on=['Q2', 'target', 'collimator'])

    #------------------------
    # Load Cherenkov and calorimeter efficiency
    # We'll have to futz with these a bit because they're in "long" format rather than wide
    pidDf = pd.read_csv(csvs['pid'])

    # Loop over detector and add it
    for thisDetector in list(set(pidDf.detector)):
        # Get subset
        detectorDf = pidDf[pidDf.detector == thisDetector].copy()

        # Rename 'efficiency' column to detector name
        detectorDf.rename({'efficiency': thisDetector}, axis=1, inplace=True)
        keepColumns = ['target', 'Q2', 'hmsAngle', 'shmsAngle', 'hmsMomentum', 'shmsMomentum', 'collimator', thisDetector]
        detectorDf = detectorDf[keepColumns]

        # Add to ctData
        # This will add some duplicate columns; I give them a suffix and then drop them.
        # This isn't ideal, but because we're working with small amounts of data,
        # it isn't a problem.
        ctData = ctData.merge(detectorDf, how='inner', on=['Q2', 'target', 'collimator'], suffixes=('', '_DROPME'))
        ctData.drop(list(ctData.filter(regex='_DROPME$')), axis=1, inplace=True)

    #------------------------
    # Add tracking
    keepColumns = ['target', 'Q2', 'collimator', 'efficiency']

    for detector in ['hTracking', 'pTracking']:
        trackDf = pd.read_csv(csvs[detector])
        trackDf = trackDf[keepColumns]
        trackDf.rename({'efficiency': detector}, axis=1, inplace=True)
        ctData = ctData.merge(trackDf, how='inner', on=['Q2', 'target', 'collimator'])

    return ctData

#------------------------------------------------------------------------------
def filterDataframe(ctData, allKinematics):
    # Initialize list of False-es with same length as ctData
    keepIndex = (ctData.Q2 == -1)

    # Loop over allKinematics
    # Find matching row(s)
    # then OR this list with keepIndex
    for kinematics in allKinematics:
        collim = kinematics['collimator']
        target = kinematics['target']
        Q2     = kinematics['Q2']
        thisIndex = (ctData.collimator==collim) & (ctData.target==target) & (ctData.Q2==Q2)
        keepIndex = (keepIndex | thisIndex)

    return ctData[keepIndex]

#------------------------------------------------------------------------------
def calculateRatio(df):
    df = df.assign(undercounting=df.livetime*df.absorption*df.pCer*df.hCal*df.hCer*df.hTracking*df.pTracking)
    df = df.assign(yieldratio=df.datayield/df.simcyield/df.undercounting)

    return df

#------------------------------------------------------------------------------
# This calculates transparency for every combo of Q2 and target that has the same collimator
def calculateTransparency(df):
    transparencies = []

    #------------------
    # Q^2=8
    q=8
    collim="pion"
    thickness="C12_thick"
    lh2 = df[(df.collimator==collim) & (df.Q2==q) & (df.target=="LH2")]
    c12 = df[(df.collimator==collim) & (df.Q2==q) & (df.target==thickness)]

    if (len(lh2.index)!=1):
        warn('Too many (len=%d) lh2 matches for: collimator=%s, Q^2=%f, C12=%s' % (len(lh2.index), collim, q, thickness))
    if (len(c12.index)!=1):
        warn('Too many (len=%d) c12 matches for: collimator=%s, Q^2=%f, C12=%s' % (len(lh2.index), collim, q, thickness))

    T = c12.yieldratio.values[0] / lh2.yieldratio.values[0]
    transparencies.append({"Q2": q, "C12": thickness, "collimator": collim, "transparency": T})

    #------------------
    # Q^2=9.5
    q=9.5
    collim="large"
    thickness="C12_thick"
    lh2 = df[(df.collimator==collim) & (df.Q2==q) & (df.target=="LH2")]
    c12 = df[(df.collimator==collim) & (df.Q2==q) & (df.target==thickness)]

    if (len(lh2.index)!=1):
        warn('Too many (len=%d) lh2 matches for: collimator=%s, Q^2=%f, C12=%s' % (len(lh2.index), collim, q, thickness))
    if (len(c12.index)!=1):
        warn('Too many (len=%d) c12 matches for: collimator=%s, Q^2=%f, C12=%s' % (len(lh2.index), collim, q, thickness))

    T = c12.yieldratio.values[0] / lh2.yieldratio.values[0]
    transparencies.append({"Q2": q, "C12": thickness, "collimator": collim, "transparency": T})

    #------------------
    # Q^2=11.5
    q=11.5
    collim="large"
    thickness="C12_thick"
    lh2 = df[(df.collimator==collim) & (df.Q2==q) & (df.target=="LH2")]
    c12 = df[(df.collimator==collim) & (df.Q2==q) & (df.target==thickness)]

    if (len(lh2.index)!=1):
        warn('Too many (len=%d) lh2 matches for: collimator=%s, Q^2=%f, C12=%s' % (len(lh2.index), collim, q, thickness))
    if (len(c12.index)!=1):
        warn('Too many (len=%d) c12 matches for: collimator=%s, Q^2=%f, C12=%s' % (len(lh2.index), collim, q, thickness))

    T = c12.yieldratio.values[0] / lh2.yieldratio.values[0]
    transparencies.append({"Q2": q, "C12": thickness, "collimator": collim, "transparency": T})

    #------------------
    # Q^2=14.3
    q=14.3
    collim="large"
    thickness="C12_thick"
    lh2 = df[(df.collimator==collim) & (df.Q2==q) & (df.target=="LH2")]
    c12 = df[(df.collimator==collim) & (df.Q2==q) & (df.target==thickness)]

    if (len(lh2.index)!=1):
        warn('Too many (len=%d) lh2 matches for: collimator=%s, Q^2=%f, C12=%s' % (len(lh2.index), collim, q, thickness))
    if (len(c12.index)!=1):
        warn('Too many (len=%d) c12 matches for: collimator=%s, Q^2=%f, C12=%s' % (len(lh2.index), collim, q, thickness))

    T = c12.yieldratio.values[0] / lh2.yieldratio.values[0]
    transparencies.append({"Q2": q, "C12": thickness, "collimator": collim, "transparency": T})

    transparencyDf = pd.DataFrame(transparencies)

    return transparencyDf

#------------------------------------------------------------------------------
def writeCsv(ctData, filename):
    ctData.to_csv(filename, index=False)

#------------------------------------------------------------------------------
if __name__ == '__main__':
    main()
