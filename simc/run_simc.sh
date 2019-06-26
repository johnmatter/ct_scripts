#!/bin/bash

# We expect user to specify infile as argument
infile=$1

# This uses /bin/expect to "simulate" user input to simc
simc_expect_script=/home/jmatter/ct_scripts/simc/simc.exp

# We need to cd here to run simc
simc_dir=/work/hallc/e1206107/jmatter/simc_gfortran

cd $simc_dir
$simc_expect_script $infile
