#!/usr/bin/env bash

echo --------------------------------------------------------------------------
echo Q2=8
hadd lh2_hms_singles_q2_8.root \
    hms_coin_replay_production_2049_-1.root
echo
hadd lh2_shms_singles_q2_8.root \
    shms_coin_replay_production_2049_500000.root
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
hadd lh2_shms_singles_q2_9.5.root \
    shms_coin_replay_production_3189_500000.root \
    shms_coin_replay_production_3191_500000.root
echo
echo
echo
echo

echo --------------------------------------------------------------------------
echo Q2=11.5
hadd lh2_hms_singles_q2_11.5.root \
    hms_coin_replay_production_2454_-1.root
echo
hadd lh2_shms_singles_q2_11.5.root \
    shms_coin_replay_production_2454_500000.root
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
hadd lh2_shms_singles_q2_14.3.root \
    shms_coin_replay_production_2397_500000.root \
    shms_coin_replay_production_2404_500000.root \
    shms_coin_replay_production_2405_500000.root \
    shms_coin_replay_production_2436_500000.root \
    shms_coin_replay_production_2437_500000.root
echo
echo
echo
echo
