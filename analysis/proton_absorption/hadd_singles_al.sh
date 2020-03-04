#!/usr/bin/env bash

echo --------------------------------------------------------------------------
echo Q2=8
hadd dummy_q2_8.root \
    hms_coin_replay_production_2055_-1.root \
    hms_coin_replay_production_2067_-1.root \
    hms_coin_replay_production_2068_-1.root
echo
echo
echo
echo

echo --------------------------------------------------------------------------
echo Q2=9.5
hadd dummy_q2_9.5.root \
    hms_coin_replay_production_3192_-1.root \
    hms_coin_replay_production_3200_-1.root
echo
echo
echo
echo

echo --------------------------------------------------------------------------
echo Q2=11.5
hadd dummy_q2_11.5.root \
    hms_coin_replay_production_2455_-1.root
echo
echo
echo
echo

echo --------------------------------------------------------------------------
echo Q2=14.3
hadd dummy_q2_14.3.root \
    hms_coin_replay_production_2438_-1.root
echo
echo
echo
echo
