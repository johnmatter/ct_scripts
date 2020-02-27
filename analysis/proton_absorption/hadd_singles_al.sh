#!/usr/bin/env bash

echo --------------------------------------------------------------------------
echo Q2=8
hadd dummy_q2_8.root \
    hms_coin_replay_production_2055_500000.root \
    hms_coin_replay_production_2067_500000.root \
    hms_coin_replay_production_2068_500000.root
echo
echo
echo
echo

echo --------------------------------------------------------------------------
echo Q2=9.5
hadd dummy_q2_9.5.root \
    hms_coin_replay_production_3192_500000.root \
    hms_coin_replay_production_3200_500000.root
echo
echo
echo
echo

echo --------------------------------------------------------------------------
echo Q2=11.5
hadd dummy_q2_11.5.root \
    hms_coin_replay_production_2455_500000.root
echo
echo
echo
echo

echo --------------------------------------------------------------------------
echo Q2=14.3
hadd dummy_q2_14.3.root \
    hms_coin_replay_production_2438_500000.root
echo
echo
echo
echo
