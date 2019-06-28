#!/bin/bash
textfileDir=/work/hallc/e1206107/jmatter/simc_gfortran/outfiles/

echo radCorrDir,collimator,target,q2,normfac
for radCorrDir in $(ls $textfileDir); do
    for collimDir in $(ls $textfileDir/$radCorrDir); do
        for f in $(ls $textfileDir/$radCorrDir/$collimDir/*.hist); do
            normfac=$(grep normfac $f | cut -d= -f2 | sed "s/\s*//g")

            collimator=$(echo $collimDir| sed "s/\(.*\)_collimator/\1/")

            bname=$(basename $f .hist)
            target=$(echo $bname | sed "s/\(.*\)_q_.*/\1/")
            q2=$(echo $bname | sed "s/.*_q_\(.*\)/\1/")

            echo $radCorrDir,$collimator,$target,$q2,$normfac
        done
    done
done
