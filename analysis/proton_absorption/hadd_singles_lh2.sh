#!/usr/bin/env bash

echo --------------------------------------------------------------------------
echo Q2=8
hadd lh2_hms_singles_q2_8.root \
    hms_coin_replay_production_2049_-1.root
echo
echo
echo
echo

echo --------------------------------------------------------------------------
echo Q2=9.5
hadd lh2_hms_singles_q2_9.5.root \
    hms_coin_replay_production_3189_-1.root \
    hms_coin_replay_production_3191_-1.root
echo
echo
echo
echo

echo --------------------------------------------------------------------------
echo Q2=11.5
hadd lh2_hms_singles_q2_11.5.root \
    hms_coin_replay_production_2454_-1.root
echo
echo
echo
echo

echo --------------------------------------------------------------------------
echo Q2=14.3
hadd lh2_hms_singles_q2_14.3.root \
    hms_coin_replay_production_2397_-1.root \
    hms_coin_replay_production_2404_-1.root \
    hms_coin_replay_production_2405_-1.root \
    hms_coin_replay_production_2436_-1.root \
    hms_coin_replay_production_2437_-1.root
echo
echo
echo
echo
