#!/bin/bash
#----------------------------------------------------------------
# Install script for C++ SDK
# To start the C++ SDK installation, go to the directory this file is located in and send the following command in a terminal:
# bash install.sh 
#----------------------------------------------------------------
# Once the above command is run, please answer the prompts and acknowledge the license agreement.
#----------------------------------------------------------------

ClientId=YOUR_CLIENT_ID
ProductId=YOUR_PRODUCT_ID
Locale=en-US
DeviceSerialNumber=123456

Origin=$(pwd)

get_answer() {
  local Answer
  clear
  echo ""
  echo "Account credentials have not been set."
  echo "Your account credentials can be found or created here:"
  echo "https://developer.amazon.com/edw/home.html"
  echo "Please write or copy and paste the $1 "
  echo "OR edit this script with the values and run it again"
  echo "Enter 'q' to quit"
  while [ "${#Answer}" -lt 1 ]; do
    read -p "Enter your $1>>" Answer
    if [ "$Answer" = "q" ]; then
      exit
    fi
  done
	eval "$1=$Answer"
}

#----------------------------------------------------------------
# Function to select a user's preference between several options
#----------------------------------------------------------------
# Arguments are: result_var option1 option2...
select_option()
{
  local _result=$1
  local ARGS=("$@")
  if [ "$#" -gt 0 ]; then
    while [ true ]; do
      local count=1
      for option in "${ARGS[@]:1}"; do
        echo "$count) $option"
        ((count+=1))
      done
      echo ""
      local USER_RESPONSE
      read -p "Please select an option [1-$(($#-1))] " USER_RESPONSE
      case $USER_RESPONSE in
        ''|*[!0-9]*) echo "Please provide a valid number"
          continue
          ;;
         *) if [[ "$USER_RESPONSE" -gt 0 && $((USER_RESPONSE+1)) -le "$#" ]]; then
            local SELECTION=${ARGS[($USER_RESPONSE)]}
            echo "Selection: $SELECTION"
            eval $_result=\$SELECTION
            return
          else
            clear
            echo "Please select a valid option"
          fi
          ;;
      esac
    done
  fi
}
write_run() {
  local var=""
  asound_file=$1
  wake_word_engine=$2
  echo "cp /home/pi/$asound_file /home/pi/.asoundrc" > $Origin/run.sh
  echo "sudo cp /home/pi/$asound_file /root/.asoundrc" >> $Origin/run.sh
  
  echo "sleep 2" >> $Origin/run.sh
  if [ "$device_type" == "24 LED ring" ] || [ "$device_type" == "32 LED ring" ];then
    echo "source /home/pi/pulsestart.sh" >> $Origin/run.sh
	  var="sudo"
  fi
  echo "cd /home/pi/sdk-folder/sdk-build/SampleApp/src/" >> $Origin/run.sh
  
  if [ "$wake_word_engine" == "Sensory" ]; then
	  echo "$var ./SampleApp /home/pi/sdk-folder/sdk-build/Integration/AlexaClientSDKConfig.json /home/pi/sdk-folder/third-party/alexa-rpi/models" >> $Origin/run.sh
  elif [ "$wake_word_engine" == "Snowboy" ]; then
	  echo "$var ./SampleApp /home/pi/sdk-folder/sdk-build/Integration/AlexaClientSDKConfig.json /home/pi/sdk-folder/third-party/snowboy/resources" >> $Origin/run.sh
  else
    echo "$var ./SampleApp /home/pi/sdk-folder/sdk-build/Integration/AlexaClientSDKConfig.json" >> $Origin/run.sh
  fi

}

#rpi_ws281x install
install_rpi_ws281x()
{
  cd /home/pi
  git clone https://github.com/jgarff/rpi_ws281x.git
  sudo apt-get -y install scons
  cd rpi_ws281x && sudo scons
}

install_I2S_driver()
{
  ###Kernel Build / Driver installation
  sudo apt-get -y install bc libncurses5-dev libncursesw5-dev pulseaudio pavucontrol gstreamer1.0-pulseaudio
  cd ~
  git clone --depth 10 -n --branch cnxt-rpi-4.4.y --single-branch https://github.com/conexant/rpi-linux.git
  KERNEL=kernel7
  cd rpi-linux
  git checkout

  make bcm2709_defconfig
  make zImage modules dtbs -j2

  sudo make modules_install
  sudo cp arch/arm/boot/dts/*.dtb /boot/
  sudo cp arch/arm/boot/dts/overlays/*.dtb* /boot/overlays/
  sudo cp arch/arm/boot/dts/overlays/README /boot/overlays/
  sudo cp arch/arm/boot/zImage /boot/$KERNEL.img

  sync
}

install_drv()
{
    BUILDFOLDER=""

    sudo apt-get -y install pulseaudio pavucontrol gstreamer1.0-pulseaudio raspberrypi-kernel raspberrypi-kernel-headers

    sync

    folders=($(ls /lib/modules/ | grep "v7"))
    for folder in "${folders[@]}";
    do
        stat /lib/modules/${folder}/build
	if [ $? -eq 0 ]; then	
	    echo "Find the latest version: ${folder}"
	    BUILDFOLDER=${folder}
	    break
	fi
    done

    make clean BUILD_ARG=${BUILDFOLDER}
    make BUILD_ARG=${BUILDFOLDER}
    [ "$?" -eq 0 ] || exit $?
    sudo cp snd-soc-cx*.ko /lib/modules/${BUILDFOLDER}/kernel/sound/soc/codecs/
    sudo cp snd-soc-simple*.ko /lib/modules/${BUILDFOLDER}/kernel/sound/soc/generic/
    sudo cp *.dtbo /boot/overlays/
    sync
    sudo depmod ${BUILDFOLDER}

    sync
}

install_CX9000_I2S_driver()
{
  mkdir /home/pi/cx9000_drv_install -v
	cd /home/pi/cx9000_drv_install
	
	wget https://raw.githubusercontent.com/conexant/codec_drivers/rpi-4.4.y/sound/soc/codecs/cx9000.h
	wget https://raw.githubusercontent.com/conexant/codec_drivers/rpi-4.4.y/sound/soc/codecs/cx9000.c
	wget https://raw.githubusercontent.com/conexant/codec_drivers/rpi-4.4.y/sound/soc/generic/simple-card-plus.c
	wget https://raw.githubusercontent.com/conexant/codec_drivers/rpi-4.4.y/sound/soc/generic/simple_card_plus.h
	wget https://raw.githubusercontent.com/conexant/codec_drivers/rpi-4.4.y/arch/arm/boot/dts/overlays/rpi-cxsmartspk2-i2s-plus-overlay.dts
    
    cp $Origin/Makefile .
    # Build and Install CX9000 Driver
    install_drv

    cd ~

    echo
    echo "==============> The building & installing of Synaptics' CX9000 codec driver is complete == =============="
    echo
}

#ATS certification is enabled by Amazon after June 15 2018, 
#Please get more info:https://developer.amazon.com/zh/docs/alexa-voice-service/update-certificate-authorities.html
ats_certification()
{
	if [ ! -d "/usr/share/ca-certificates" ]; then
	echo "Please make sure your RPi installed OpenSSL correctly, couldn't find folder $SYS_CERT_FOLDER"
	exit -1
	fi

	if [ ! -f "/etc/ca-certificates.conf" ]; then
		echo "Please make sure your RPi installed OpenSSL correctly, couldn't find folder $SYS_CERT_FILE"
		exit -1
	fi

	cd /usr/share/ca-certificates

	sudo wget https://www.amazontrust.com/repository/AmazonRootCA1.pem -O /usr/share/ca-certificates/AmazonRootCA1.pem
	
	if [ $? -eq 0 ];then
		echo "Get CA File successful."
	else
		echo "Get CA File Error."
		exit -1
	fi
	sudo chmod 606 /etc/ca-certificates.conf

	sudo echo "AmazonRootCA1.pem" >> /etc/ca-certificates.conf
	sudo update-ca-certificates
	# openssl s_client -tls1_2 -connect avs-ats-cert-test.amazon.com:443 -verify 10

	# if [ $? -eq 0 ];then
	# 	echo "ATS certification OK."
	# else
	# 	echo "ATS certification Error, Here is the guide about the ATS certification from Amazon:https://aws.amazon.com/cn/blogs/security/how-to-prepare-for-aws-move-to-its-own-certificate-authority/"
	# 	exit -1
	# fi
}

if [ "$ClientId" = "YOUR_CLIENT_ID" ]; then
  ClientId=""
fi
if [ "$ProductId" = "YOUR_PRODUCT_ID" ]; then
  ProductId=""
fi

if [ "${#ClientId}" -lt 1 ]; then
  get_answer "ClientId"
fi

if [ "${#ProductId}" -lt 1 ]; then
  get_answer "ProductId"
fi

# Select Device type
clear
echo ""
echo ""
echo ""
echo "Do you have LEDs? If so, what type?"
echo ""
echo ""
select_option device_type "No" "24 LED ring" "32 LED ring" 

# Select Playback path
clear
echo ""
echo ""
echo ""
echo "Please select the playback path that you want to use:"
echo "The RPi's integrated DAC (note that the LEDs will not function correctly),  the CX2072X Amplifier, or the CX9000 Amplifier?"
echo ""
echo ""
select_option playback_type "RPi" "CX2X72X" "CX9000" 

# Select Recording Path
clear
echo ""
echo ""
echo ""
echo "Please select the recording path that you want to use:"
echo ""
echo ""
select_option record_type "USB from DSP" "I2S from CX2092X"


# Select Trigger Word Engine
clear
echo ""
echo ""
echo ""
echo "Please select the keyword detector running on the RPi you wish to use"
echo "If you are planning to use the Embedded Synaptics Smart Trigger (recommended), select None"
echo ""
echo ""
select_option wake_word_engine_type "Sensory" "Snowboy" "None" 


# Select DSP Trigger Word Engine
clear
echo ""
echo ""
echo ""
echo "Please select the keyword detector running on the DSP you wish to use"
echo "To use the Embedded Synaptics Smart Trigger, select GPIO"
echo ""
echo ""
select_option dsp_wake_word_engine_type "GPIO" "None" 


if [ "$wake_word_engine_type" != "None" ] || [ "$dsp_wake_word_engine_type" == "GPIO" ]; then
clear
echo ""
echo ""
echo ""
echo "Do you want the keyword cloud revalidation to be on or off?"
echo ""
echo ""
select_option cloud_revalidation "On" "Off"
fi

clear

cd /home/pi/ && mkdir sdk-folder
cd sdk-folder && mkdir sdk-build sdk-source third-party application-necessities 
cd application-necessities && mkdir sound-files

avs_cmake="cmake $Origin -DGSTREAMER_MEDIA_PLAYER=ON -DPORTAUDIO=ON -DPORTAUDIO_LIB_PATH=/home/pi/sdk-folder/third-party/portaudio/lib/.libs/libportaudio.a -DPORTAUDIO_INCLUDE_DIR=/home/pi/sdk-folder/third-party/portaudio/include"


sudo apt-get update
sudo apt-get -y install git 
echo ''
echo '--------------------------------------------------------------------------'
if [ "$wake_word_engine_type" == "Sensory" ]; then
	echo ''
	echo "Please wait a little for the Sensory license agreement"
	echo '--------------------------------------------------------------------------'
	cd /home/pi/sdk-folder/third-party && git clone git://github.com/Sensory/alexa-rpi.git
	cd /home/pi/sdk-folder/third-party/alexa-rpi/bin/ && ./license.sh
	avs_cmake="$avs_cmake -DSENSORY_KEY_WORD_DETECTOR=ON -DSENSORY_KEY_WORD_DETECTOR_LIB_PATH=/home/pi/sdk-folder/third-party/alexa-rpi/lib/libsnsr.a -DSENSORY_KEY_WORD_DETECTOR_INCLUDE_DIR=/home/pi/sdk-folder/third-party/alexa-rpi/include"

elif [ "$wake_word_engine_type" == "Snowboy" ]; then
	cd /home/pi/sdk-folder/third-party && git clone https://github.com/Kitt-AI/snowboy
	avs_cmake="$avs_cmake -DKITTAI_KEY_WORD_DETECTOR=ON -DKITTAI_KEY_WORD_DETECTOR_LIB_PATH=/home/pi/sdk-folder/third-party/snowboy/lib/rpi/libsnowboy-detect.a -DKITTAI_KEY_WORD_DETECTOR_INCLUDE_DIR=/home/pi/sdk-folder/third-party/snowboy/include"
fi

if [ "$dsp_wake_word_engine_type" == "GPIO" ] && [ "$wake_word_engine_type" == "None" ]; then
  avs_cmake="$avs_cmake -DGPIO_KEY_WORD_DETECTOR=ON"
fi

if [ "$dsp_wake_word_engine_type" == "USB Keypress (t)" ]; then
  sudo apt-get -y install libncurses5-dev libncursesw5-dev
  avs_cmake="$avs_cmake -DCURSES=ON"
fi
echo '--------------------------------------------------------------------------'

if [ "$fast_transfer" == "Yes" ]; then
	   avs_cmake="$avs_cmake -DTWO_STAGE_TRIGGER_CONF=ON"
	   if [ "$record_type" == "I2S from CX2092X" ]; then
	   avs_cmake="$avs_cmake -DTWO_STAGE_TRIGGER_I2S_MODE=ON"
	   fi
fi

echo ''
echo '--------------------------------------------------------------------------'
if [ "$device_type" == "4-Mic EVK V1.0" ]; then
  echo "Installation starting, this will take a few hours."
elif [ "$device_type" == "2-Mic EVK" ] || [ "$device_type" == "4-Mic EVK V2.0" ]; then
  echo "Installation starting, this should take less than an hour."
fi
echo '--------------------------------------------------------------------------'

sudo apt-get -y install git gcc cmake build-essential libsqlite3-dev libcurl4-openssl-dev libfaad-dev libsoup2.4-dev libgcrypt20-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-good libasound2-dev sox gedit vim doxygen libblas-dev python3-pip
pip install flask commentjson
  
sudo apt-get -y install libgtest-dev
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo make install

sudo apt-get purge bluealsa -y

cd /home/pi/sdk-folder/third-party && wget -c http://www.portaudio.com/archives/pa_stable_v190600_20161030.tgz && tar zxf pa_stable_v190600_20161030.tgz && cd portaudio && ./configure --without-jack && make

chmod +x $Origin/SampleApp/src/host_demo.exe
chmod +x $Origin/SampleApp/src/host_demo_aur.exe

sudo cp $Origin/alsa.conf /usr/share/alsa/alsa.conf 

if [ "$device_type" == "24 LED ring" ] || [ "$device_type" == "32 LED ring" ]; then
  avs_cmake="$avs_cmake -DLED=ON -DLED_INCLUDE_DIR=/home/pi/rpi_ws281x"
  sudo apt-get install -y pulseaudio pavucontrol gstreamer1.0-pulseaudio
  
  cp $Origin/pulsestart.sh /home/pi/pulsestart.sh

  install_rpi_ws281x

if [ "$device_type" == "24 LED ring" ]; then
  avs_cmake="$avs_cmake -DLED24=ON"
fi
fi



if [ "$playback_type" == "CX2X72X" ]; then
  install_I2S_driver
  sudo cp  $Origin/config.txt /boot/config.txt
  sudo sh -c "echo 'dtoverlay=rpi-cxsmartspk-usb' >> /boot/config.txt"
  # Re-sampling method fix
  sudo sed -i "s/; resample-method = speex-float-1/resample-method = soxr-vhq/" /etc/pulse/daemon.conf
  sudo sed -i "s/; default-sample-rate = 44100/default-sample-rate = 48000/" /etc/pulse/daemon.conf
  mkdir /home/pi/tuning/
  cp $Origin/CODEC_CX2X72X_shell_script.sh /home/pi/tuning/
  cp $Origin/run_tuning.sh /home/pi/tuning/
  if [[ $(grep "run_tuning.sh" /home/pi/.profile) = "" ]]; then
    echo "bash /home/pi/tuning/run_tuning.sh CX2X72X" >> /home/pi/.profile
  fi
elif [ "$playback_type" == "CX9000" ]; then
  install_CX9000_I2S_driver
  sudo cp  $Origin/config.txt /boot/config.txt
  sudo sh -c "echo 'dtoverlay=rpi-cxsmartspk2-i2s-plus' >> /boot/config.txt"
  # Re-sampling method fix
  sudo sed -i "s/; resample-method = speex-float-1/resample-method = soxr-vhq/" /etc/pulse/daemon.conf
	sudo sed -i "s/; default-sample-rate = 44100/default-sample-rate = 48000/" /etc/pulse/daemon.conf
  mkdir /home/pi/tuning
  cp $Origin/CODEC_CX9000Config_shell_script.sh /home/pi/tuning/
  cp $Origin/CODEC_CX9000Tune_shell_script.sh /home/pi/tuning/
  cp $Origin/run_tuning.sh /home/pi/tuning/
  if [[ $(grep "run_tuning.sh" /home/pi/.profile) = "" ]]; then
    echo "bash /home/pi/tuning/run_tuning.sh CX9000" >> /home/pi/.profile
  fi
else #RPi
  #Force 3.5mm jack instead of HDMI audio
  sudo amixer cset numid=3 1
fi

if [ "$record_type" == "I2S from CX2092X" ]; then
  install_I2S_driver

  kernelRelease=$(uname -r)
  sudo cp $Origin/snd-soc-cx2092x.ko /lib/modules/$kernelRelease/kernel/sound/soc/codecs/ -v
  sudo cp $Origin/snd-soc-cxsmtspk-pi-i2s.ko /lib/modules/$kernelRelease/kernel/sound/soc/bcm/ -v
  sudo cp $Origin/i2s-config.txt /boot/config.txt -v
# else
  #Nothing?
fi


if [ "$cloud_revalidation" == "Off" ]; then
  avs_cmake="$avs_cmake -DNO_CLOUD_REVALIDATION=ON"
fi

# Build AVS
cd /home/pi/sdk-folder/sdk-build
eval "$avs_cmake"
make SampleApp -j2

if [ "$fast_transfer" == "Yes" ]; then
  if [ "$record_type" == "I2S from CX2092X" ]; then
    cp $Origin/i2s-twostagearc /home/pi/i2s-twostagearc
    write_run i2s-twostagearc $wake_word_engine_type
  else
    cp $Origin/twostagearc /home/pi/twostagearc
    write_run twostagearc $wake_word_engine_type
  fi
else
  if [ "$playback_type" == "RPi" ]; then
    cp $Origin/leftarc /home/pi/leftarc
  else
    cp $Origin/leftarc4mic /home/pi/leftarc
  fi
  write_run leftarc $wake_word_engine_type
fi

ats_certification

chmod +x $Origin/run.sh
cp $Origin/run.sh /home/pi/run.sh
rm $Origin/run.sh

python $Origin/config.py /home/pi/sdk-folder/sdk-build/Integration/AlexaClientSDKConfig.json $ClientId $ProductId $DeviceSerialNumber $Locale

cd $Origin

echo ''
echo '--------------------------------------------------------------------------'
echo "You're all done!"
echo "Reboot and try running run.sh in your home directory."