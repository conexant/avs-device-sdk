#!/bin/bash
# Purpose:
#       Shell script for configuration settings of Solana
# History:
# 2016/12/1       First release
# Note: 
# This script needs to be run with su or sudo 


#@ set audio_sampling_rate 48

# class-D gain
#@ set speaker_impedence 4.0
#@ set max_power 0
#@ set supply_voltage 5
#@ set calc_max_pow 2.53

# Power Averaging
#@ set pow_avg_enable 0
#@ set pow_avg 1.36
#@ set high_thresh 30
#@ set low_thresh 15
#@ set decision_time 488

#@ set vol_ctrl 0

# DRC
#@ set drc_boost 9
#@ set slow_release 128
#@ set slow_delay 2048
#@ set fast_release 128
#@ set fast_delay 32
#@ set attack_time 128
#@ set max_gain 12
#@ set drc_thresh -3
#@ set release_threshold 0
#@ set comp_slope "Infinite"
#@ set sol_drc_enable 0

# EQ
#@ set type_band0 "BIQUAD_PEAKING_FILTER"
#@ set type_band1 "BIQUAD_PEAKING_FILTER"
#@ set type_band2 "BIQUAD_PEAKING_FILTER"
#@ set type_band3 "BIQUAD_PEAKING_FILTER"
#@ set type_band4 "BIQUAD_PEAKING_FILTER"
#@ set type_band5 "BIQUAD_PEAKING_FILTER"
#@ set type_band6 "BIQUAD_PEAKING_FILTER"

#@ set freq_band0 40
#@ set freq_band1 60
#@ set freq_band2 80
#@ set freq_band3 100
#@ set freq_band4 120
#@ set freq_band5 140
#@ set freq_band6 160

#@ set gain_band0 0
#@ set gain_band1 0
#@ set gain_band2 0
#@ set gain_band3 0
#@ set gain_band4 0
#@ set gain_band5 0
#@ set gain_band6 0

#@ set qfactor_band0 1.0
#@ set qfactor_band1 1.0
#@ set qfactor_band2 1.0
#@ set qfactor_band3 1.0
#@ set qfactor_band4 1.0
#@ set qfactor_band5 1.0
#@ set qfactor_band6 1.0

#@ set sol_eq_enable 0

#@ set hpf_freq 180

# use to find which sound card is currently in use
CARD=$(aplay -l | grep cxsmtspkpiusb | sed 's/card  *\([0-9]*\).*/\1/')


EQ_BD0_B0_LSB=0
EQ_BD0_B0_MSB=64
EQ_BD0_B1_LSB=0
EQ_BD0_B1_MSB=0
EQ_BD0_B2_LSB=0
EQ_BD0_B2_MSB=0
EQ_BD0_A1_LSB=0
EQ_BD0_A1_MSB=0
EQ_BD0_A2_LSB=0
EQ_BD0_A2_MSB=0
EQ_BD0_G_MSB=3

## BAND 0 ###
amixer -D hw:$CARD cset name='DACL EQ 0' $EQ_BD0_B0_LSB,$EQ_BD0_B0_MSB,$EQ_BD0_B1_LSB,$EQ_BD0_B1_MSB,$EQ_BD0_B2_LSB,$EQ_BD0_B2_MSB,$EQ_BD0_A1_LSB,$EQ_BD0_A1_MSB,$EQ_BD0_A2_LSB,$EQ_BD0_A2_MSB,$EQ_BD0_G_MSB 
amixer -D hw:$CARD cset name='DACR EQ 0' $EQ_BD0_B0_LSB,$EQ_BD0_B0_MSB,$EQ_BD0_B1_LSB,$EQ_BD0_B1_MSB,$EQ_BD0_B2_LSB,$EQ_BD0_B2_MSB,$EQ_BD0_A1_LSB,$EQ_BD0_A1_MSB,$EQ_BD0_A2_LSB,$EQ_BD0_A2_MSB,$EQ_BD0_G_MSB

EQ_BD1_B0_LSB=0
EQ_BD1_B0_MSB=64
EQ_BD1_B1_LSB=0
EQ_BD1_B1_MSB=0
EQ_BD1_B2_LSB=0
EQ_BD1_B2_MSB=0
EQ_BD1_A1_LSB=0
EQ_BD1_A1_MSB=0
EQ_BD1_A2_LSB=0
EQ_BD1_A2_MSB=0
EQ_BD1_G_MSB=3

## BAND 1 ###
amixer -D hw:$CARD cset name='DACL EQ 1' $EQ_BD1_B0_LSB,$EQ_BD1_B0_MSB,$EQ_BD1_B1_LSB,$EQ_BD1_B1_MSB,$EQ_BD1_B2_LSB,$EQ_BD1_B2_MSB,$EQ_BD1_A1_LSB,$EQ_BD1_A1_MSB,$EQ_BD1_A2_LSB,$EQ_BD1_A2_MSB,$EQ_BD1_G_MSB  
amixer -D hw:$CARD cset name='DACR EQ 1' $EQ_BD1_B0_LSB,$EQ_BD1_B0_MSB,$EQ_BD1_B1_LSB,$EQ_BD1_B1_MSB,$EQ_BD1_B2_LSB,$EQ_BD1_B2_MSB,$EQ_BD1_A1_LSB,$EQ_BD1_A1_MSB,$EQ_BD1_A2_LSB,$EQ_BD1_A2_MSB,$EQ_BD1_G_MSB

EQ_BD2_B0_LSB=0
EQ_BD2_B0_MSB=64
EQ_BD2_B1_LSB=0
EQ_BD2_B1_MSB=0
EQ_BD2_B2_LSB=0
EQ_BD2_B2_MSB=0
EQ_BD2_A1_LSB=0
EQ_BD2_A1_MSB=0
EQ_BD2_A2_LSB=0
EQ_BD2_A2_MSB=0
EQ_BD2_G_MSB=3

## BAND 2 ###
amixer -D hw:$CARD cset name='DACL EQ 2' $EQ_BD2_B0_LSB,$EQ_BD2_B0_MSB,$EQ_BD2_B1_LSB,$EQ_BD2_B1_MSB,$EQ_BD2_B2_LSB,$EQ_BD2_B2_MSB,$EQ_BD2_A1_LSB,$EQ_BD2_A1_MSB,$EQ_BD2_A2_LSB,$EQ_BD2_A2_MSB,$EQ_BD2_G_MSB  
amixer -D hw:$CARD cset name='DACR EQ 2' $EQ_BD2_B0_LSB,$EQ_BD2_B0_MSB,$EQ_BD2_B1_LSB,$EQ_BD2_B1_MSB,$EQ_BD2_B2_LSB,$EQ_BD2_B2_MSB,$EQ_BD2_A1_LSB,$EQ_BD2_A1_MSB,$EQ_BD2_A2_LSB,$EQ_BD2_A2_MSB,$EQ_BD2_G_MSB

EQ_BD3_B0_LSB=0
EQ_BD3_B0_MSB=64
EQ_BD3_B1_LSB=0
EQ_BD3_B1_MSB=0
EQ_BD3_B2_LSB=0
EQ_BD3_B2_MSB=0
EQ_BD3_A1_LSB=0
EQ_BD3_A1_MSB=0
EQ_BD3_A2_LSB=0
EQ_BD3_A2_MSB=0
EQ_BD3_G_MSB=3

## BAND 3 ###
amixer -D hw:$CARD cset name='DACL EQ 3' $EQ_BD3_B0_LSB,$EQ_BD3_B0_MSB,$EQ_BD3_B1_LSB,$EQ_BD3_B1_MSB,$EQ_BD3_B2_LSB,$EQ_BD3_B2_MSB,$EQ_BD3_A1_LSB,$EQ_BD3_A1_MSB,$EQ_BD3_A2_LSB,$EQ_BD3_A2_MSB,$EQ_BD3_G_MSB
amixer -D hw:$CARD cset name='DACR EQ 3' $EQ_BD3_B0_LSB,$EQ_BD3_B0_MSB,$EQ_BD3_B1_LSB,$EQ_BD3_B1_MSB,$EQ_BD3_B2_LSB,$EQ_BD3_B2_MSB,$EQ_BD3_A1_LSB,$EQ_BD3_A1_MSB,$EQ_BD3_A2_LSB,$EQ_BD3_A2_MSB,$EQ_BD3_G_MSB

EQ_BD4_B0_LSB=0
EQ_BD4_B0_MSB=64
EQ_BD4_B1_LSB=0
EQ_BD4_B1_MSB=0
EQ_BD4_B2_LSB=0
EQ_BD4_B2_MSB=0
EQ_BD4_A1_LSB=0
EQ_BD4_A1_MSB=0
EQ_BD4_A2_LSB=0
EQ_BD4_A2_MSB=0
EQ_BD4_G_MSB=3

## BAND 4 ###
amixer -D hw:$CARD cset name='DACL EQ 4' $EQ_BD4_B0_LSB,$EQ_BD4_B0_MSB,$EQ_BD4_B1_LSB,$EQ_BD4_B1_MSB,$EQ_BD4_B2_LSB,$EQ_BD4_B2_MSB,$EQ_BD4_A1_LSB,$EQ_BD4_A1_MSB,$EQ_BD4_A2_LSB,$EQ_BD4_A2_MSB,$EQ_BD4_G_MSB
amixer -D hw:$CARD cset name='DACR EQ 4' $EQ_BD4_B0_LSB,$EQ_BD4_B0_MSB,$EQ_BD4_B1_LSB,$EQ_BD4_B1_MSB,$EQ_BD4_B2_LSB,$EQ_BD4_B2_MSB,$EQ_BD4_A1_LSB,$EQ_BD4_A1_MSB,$EQ_BD4_A2_LSB,$EQ_BD4_A2_MSB,$EQ_BD4_G_MSB

EQ_BD5_B0_LSB=0
EQ_BD5_B0_MSB=64
EQ_BD5_B1_LSB=0
EQ_BD5_B1_MSB=0
EQ_BD5_B2_LSB=0
EQ_BD5_B2_MSB=0
EQ_BD5_A1_LSB=0
EQ_BD5_A1_MSB=0
EQ_BD5_A2_LSB=0
EQ_BD5_A2_MSB=0
EQ_BD5_G_MSB=3

## BAND 5 ###
amixer -D hw:$CARD cset name='DACL EQ 5' $EQ_BD5_B0_LSB,$EQ_BD5_B0_MSB,$EQ_BD5_B1_LSB,$EQ_BD5_B1_MSB,$EQ_BD5_B2_LSB,$EQ_BD5_B2_MSB,$EQ_BD5_A1_LSB,$EQ_BD5_A1_MSB,$EQ_BD5_A2_LSB,$EQ_BD5_A2_MSB,$EQ_BD5_G_MSB  
amixer -D hw:$CARD cset name='DACR EQ 5' $EQ_BD5_B0_LSB,$EQ_BD5_B0_MSB,$EQ_BD5_B1_LSB,$EQ_BD5_B1_MSB,$EQ_BD5_B2_LSB,$EQ_BD5_B2_MSB,$EQ_BD5_A1_LSB,$EQ_BD5_A1_MSB,$EQ_BD5_A2_LSB,$EQ_BD5_A2_MSB,$EQ_BD5_G_MSB

EQ_BD6_B0_LSB=0
EQ_BD6_B0_MSB=64
EQ_BD6_B1_LSB=0
EQ_BD6_B1_MSB=0
EQ_BD6_B2_LSB=0
EQ_BD6_B2_MSB=0
EQ_BD6_A1_LSB=0
EQ_BD6_A1_MSB=0
EQ_BD6_A2_LSB=0
EQ_BD6_A2_MSB=0
EQ_BD6_G_MSB=3

## BAND 6 ###
amixer -D hw:$CARD cset name='DACL EQ 6' $EQ_BD6_B0_LSB,$EQ_BD6_B0_MSB,$EQ_BD6_B1_LSB,$EQ_BD6_B1_MSB,$EQ_BD6_B2_LSB,$EQ_BD6_B2_MSB,$EQ_BD6_A1_LSB,$EQ_BD6_A1_MSB,$EQ_BD6_A2_LSB,$EQ_BD6_A2_MSB,$EQ_BD6_G_MSB  
amixer -D hw:$CARD cset name='DACR EQ 6' $EQ_BD6_B0_LSB,$EQ_BD6_B0_MSB,$EQ_BD6_B1_LSB,$EQ_BD6_B1_MSB,$EQ_BD6_B2_LSB,$EQ_BD6_B2_MSB,$EQ_BD6_A1_LSB,$EQ_BD6_A1_MSB,$EQ_BD6_A2_LSB,$EQ_BD6_A2_MSB,$EQ_BD6_G_MSB

EQ_ENABLE=0

## EQ ENABLE ##
amixer -D hw:$CARD cset name='EQ Switch' $EQ_ENABLE

DRC_REL_ATTK_ENABLE=91
DRC_FAST_REL_DEL=85
DRC_BOOST=41
DRC_BALANCE_RAMP=4
DRC_VOLRAMP_GAINSHIFT=34
DRC_REL_RATE=0
DRC_COMPSLOPE_SLOW_DEL=117
DRC_MAX_LN_OUT=26
DRC_TEST=0

## DRC ##
amixer -D hw:$CARD cset name='DRC' $DRC_REL_ATTK_ENABLE,$DRC_FAST_REL_DEL,$DRC_BOOST,$DRC_BALANCE_RAMP,$DRC_VOLRAMP_GAINSHIFT,$DRC_REL_RATE,$DRC_COMPSLOPE_SLOW_DEL,$DRC_MAX_LN_OUT,$DRC_TEST

DRC_ENABLE=0

amixer -D hw:$CARD cset name='DRC Switch' $DRC_ENABLE

ANALOG_TEST10_LSB=40
ANALOG_TEST10_MSB=9
CODEC_TEST20_LSB=5
CODEC_TEST20_MSB=15
CODEC_TEST26_LSB=144
CODEC_TEST26_MSB=11

## CLASS-D/POWER AVERAGING ##
amixer -D hw:$CARD cset name='Class-D Output Level' $ANALOG_TEST10_LSB,$ANALOG_TEST10_MSB,$CODEC_TEST20_LSB,$CODEC_TEST20_MSB,$CODEC_TEST26_LSB,$CODEC_TEST26_MSB

HPF_PARAM=6

## HPF PARAMETER ##
amixer -D hw:$CARD cset name='HPF Freq' $HPF_PARAM  

VOL_CTRL_L=74
VOL_CTRL_R=74

amixer -D hw:$CARD cset name='DAC1 Volume' $VOL_CTRL_L,$VOL_CTRL_R

## Save setting
sudo alsactl store  

