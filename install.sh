#!/bin/bash
#----------------------------------------------------------------
# Install script for C++ SDK
# To start the C++ SDK installion, go to the directory this file is located in and send the following command in a terminal:
# bash install.sh 
#----------------------------------------------------------------
# Once the above command is run, please answer the prompts and acknowledge the license agreement.
#----------------------------------------------------------------

ClientSecret=YOUR_CLIENT_SECRET
ClientId=YOUR_CLIENT_ID
ProductId=YOUR_PRODUCT_ID
Locale=en-US
DeviceSerialNumber=123456
RefreshToken=RefreshTokenPlaceholder

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
  echo "cp /home/pi/leftarc /home/pi/.asoundrc" > $Origin/run.sh
  echo "sleep 1" >> $Origin/run.sh
  if [ "$device_type" == "4-Mic EVK" ];then
    echo "source /home/pi/pulsestart.sh" >> $Origin/run.sh
    var="sudo"
  fi
  echo "cd /home/pi/sdk-folder/sdk-build/SampleApp/src/" >> $Origin/run.sh
  echo "TZ=UTC $var ./SampleApp /home/pi/sdk-folder/sdk-build/Integration/AlexaClientSDKConfig.json /home/pi/sdk-folder/third-party/alexa-rpi/models" >> $Origin/run.sh
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
  sudo apt-get -y install bc libncurses5-dev libncursesw5-dev pulseaudio pavucontrol
  cd ~
  git clone --depth 1 -n --branch cnxt-rpi-4.4.y --single-branch https://github.com/conexant/rpi-linux.git
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
}

#ATS certification is enabled by Amazon after June 15 2018, 
#Please get more info:https://developer.amazon.com/zh/docs/alexa-voice-service/update-certificate-authorities.html
ats_certification()
{
  if [ ! -d "/usr/share/ca-certificates" ]; then
  echo "Please make sure your Rpi intall OpenSSL correctly, could't find folder $SYS_CERT_FOLDER"
  exit -1
  fi

  if [ ! -f "/etc/ca-certificates.conf" ]; then
    echo "Please make sure your Rpi intall OpenSSL correctly, could't find folder $SYS_CERT_FILE"
    exit -1
  fi

  cd /usr/share/ca-certificates

  sudo wget https://www.amazontrust.com/repository/AmazonRootCA1.pem -O /usr/share/ca-certificates/AmazonRootCA1.pem
  
  if [ $? -eq 0 ];then
    echo "Get CA File successfull."
  else
    echo "Get CA File Error."
    exit -1
  fi
  sudo chmod 606 /etc/ca-certificates.conf

  sudo echo "AmazonRootCA1.pem" >> /etc/ca-certificates.conf
  sudo update-ca-certificates
  openssl s_client -tls1_2 -connect avs-ats-cert-test.amazon.com:443 -verify 10

  if [ $? -eq 0 ];then
    echo "ATS certification OK."
  else
    echo "ATS certification Error, Here is the guide about the ATS certification from Amazon:https://aws.amazon.com/cn/blogs/security/how-to-prepare-for-aws-move-to-its-own-certificate-authority/"
    exit -1
  fi
}

if [ "$ClientSecret" = "YOUR_CLIENT_SECRET" ]; then
  ClientSecret=""
fi
if [ "$ClientId" = "YOUR_CLIENT_ID" ]; then
  ClientId=""
fi
if [ "$ProductId" = "YOUR_PRODUCT_ID" ]; then
  ProductId=""
fi

if [ "${#ClientId}" -lt 1 ]; then
  get_answer "ClientId"
fi
if [ "${#ClientSecret}" -lt 1 ]; then
  get_answer "ClientSecret"
fi
if [ "${#ProductId}" -lt 1 ]; then
  get_answer "ProductId"
fi

# Select Device type
clear
echo ""
echo ""
echo ""
echo "Are you using the 2-Mic or the 4-Mic EVK?"
echo ""
echo ""
select_option device_type "2-Mic EVK" "4-Mic EVK" 

clear

echo ''
echo "Please wait a little for the Sensory license agreement"
echo '--------------------------------------------------------------------------'


cd /home/pi/ && mkdir sdk-folder && cd sdk-folder && mkdir sdk-build sdk-source third-party application-necessities && cd application-necessities && mkdir sound-files
sudo apt-get update
sudo apt-get -y install git 

cd /home/pi/sdk-folder/third-party && git clone git://github.com/Sensory/alexa-rpi.git
cd /home/pi/sdk-folder/third-party/alexa-rpi/bin/ && ./license.sh

avs_cmake="cmake $Origin -DSENSORY_KEY_WORD_DETECTOR=ON -DSENSORY_KEY_WORD_DETECTOR_LIB_PATH=/home/pi/sdk-folder/third-party/alexa-rpi/lib/libsnsr.a -DSENSORY_KEY_WORD_DETECTOR_INCLUDE_DIR=/home/pi/sdk-folder/third-party/alexa-rpi/include -DGSTREAMER_MEDIA_PLAYER=ON -DPORTAUDIO=ON -DPORTAUDIO_LIB_PATH=/home/pi/sdk-folder/third-party/portaudio/lib/.libs/libportaudio.a -DPORTAUDIO_INCLUDE_DIR=/home/pi/sdk-folder/third-party/portaudio/include"

echo ''
echo '--------------------------------------------------------------------------'
if [ "$device_type" == "4-Mic EVK" ]; then
  echo "Installation starting, this will take a few hours."
elif [ "$device_type" == "2-Mic EVK" ]; then
  echo "Installation starting, this should take less than an hour."
fi
echo '--------------------------------------------------------------------------'

sudo apt-get -y install gcc cmake build-essential libsqlite3-dev libcurl4-openssl-dev libfaad-dev libsoup2.4-dev libgcrypt20-dev libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-good libasound2-dev doxygen
pip install commentjson

cd /home/pi/sdk-folder/third-party && wget -c http://www.portaudio.com/archives/pa_stable_v190600_20161030.tgz && tar zxf pa_stable_v190600_20161030.tgz && cd portaudio && ./configure --without-jack && make

chmod +x $Origin/SampleApp/src/host_demo.exe

if [ "$device_type" == "4-Mic EVK" ]; then
  avs_cmake="$avs_cmake -DLED=ON -DLED_INCLUDE_DIR=/home/pi/rpi_ws281x"

  install_I2S_driver

  sudo cp  $Origin/config.txt /boot/config.txt
  cp $Origin/pulsestart.sh /home/pi/pulsestart.sh

  install_rpi_ws281x
else
  #Change default playback device plugin
  sed -i '37s/.*/    slave.pcm "hw:0,0"/' $Origin/leftarc
  #Force 3.5mm jack instead of HDMI audio
  sudo amixer cset numid=3 1
fi


# Build AVS
cd /home/pi/sdk-folder/sdk-build
eval "$avs_cmake"
make SampleApp -j2

ats_certification

cp $Origin/leftarc /home/pi/leftarc
write_run
chmod +x $Origin/run.sh
cp $Origin/run.sh /home/pi/run.sh
rm $Origin/run.sh

python $Origin/config.py /home/pi/sdk-folder/sdk-build/Integration/AlexaClientSDKConfig.json $ClientId $ClientSecret $ProductId $DeviceSerialNumber $RefreshToken $Locale

echo ''
echo '--------------------------------------------------------------------------'
echo "If you haven't already done so, in your product's security profile found in the AVS developer console, add"
echo "http://localhost:3000 to allowed origins and"
echo "http://localhost:3000/authresponse to allowed return URLs"
echo 'Open up your browser, go to http://localhost:3000, and login or create an account.'
echo '--------------------------------------------------------------------------'
cd /home/pi/sdk-folder/sdk-build && python AuthServer/AuthServer.py
echo ''
echo '--------------------------------------------------------------------------'
echo "You're all done!"
echo "Reboot and try running run.sh in your home directory."