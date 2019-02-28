#!/bin/bash
if [ "$1" == "CX9000" ]; then
	if [ $(aplay -l | grep cxsmartspeaker2) == "" ]; then
		exit -1
	fi
	bash /home/pi/tuning/CODEC_CX9000Config_shell_script.sh
	bash /home/pi/tuning/CODEC_CX9000Tune_shell_script.sh
elif [ "$1" == "CX2X72X"]; then
	if [ $(aplay -l | grep cxsmartspeaker) == "" ]; then
		exit -1
	fi
	bash /home/pi/tuning/CODEC_CX2X72X_shell_script.sh
fi
