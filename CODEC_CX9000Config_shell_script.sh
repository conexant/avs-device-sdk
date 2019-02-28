#@ set audio_sampling_rate 48

#@ set hpf_freq_left 90
#@ set hpf_freq_right 90

#@ set hpf_order_left "2nd"
#@ set hpf_order_right "2nd"

#@ set hpf_enable_left 1
#@ set hpf_enable_right 1

#@ set hwpid_voltage 12.80
#@ set hwpid_max_power 3.1697863849222268
#@ set hwpid_pthresh 0.04763719512195122
#@ set hwpid_mode "V * I"

#@ set spkr_impedence 4
#@ set spkr_long_term 2
#@ set spkr_short_term 3.5

#@ set dac_config "Bi-mono"
#@ set dac_gain -8
#@ set class_d_gain 18
#@ set class_d_fine 0.0

# use to find which sound card is currently in use
CARD=$(aplay -l | grep cxsmartspeaker2 | sed 's/card  *\([0-9]*\).*/\1/')

SPKR_IMPED=1
DAC_CONFIG=3
CLASSD_GAIN=0
CLASSD_GAIN_FINE=0
CSDAC_GAIN=2
HPF_ORDER_L=1
HPF_ENABLE_L=1
HPF_FREQ_L=3
HPF_ORDER_R=1
HPF_ENABLE_R=1
HPF_FREQ_R=3

amixer -D hw:$CARD cset name='Speaker Impedance' $SPKR_IMPED
amixer -D hw:$CARD cset name='DAC Config' $DAC_CONFIG
amixer -D hw:$CARD cset name='ClassD Gain' $CLASSD_GAIN
amixer -D hw:$CARD cset name='ClassD Gain Fine Control' $CLASSD_GAIN_FINE
amixer -D hw:$CARD cset name='CSDAC Gain' $CSDAC_GAIN
amixer -D hw:$CARD cset name='HPF 2nder' $HPF_ORDER_L $HPF_ORDER_R
amixer -D hw:$CARD cset name='HPF Enable' $HPF_ENABLE_L $HPF_ENABLE_R
amixer -D hw:$CARD cset name="HPF Freq" $HPF_FREQ_L $HPF_FREQ_R

PID_BYTE=12787512

amixer -D hw:$CARD cset name='SA2 L PID Band 0' $PID_BYTE
amixer -D hw:$CARD cset name='SA2 R PID Band 0' $PID_BYTE

sudo alsactl store




