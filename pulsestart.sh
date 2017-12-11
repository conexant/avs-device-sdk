if [ $( pactl list sources | grep alsa_input.left -c ) -lt 1 ]; then
  pactl load-module module-alsa-source device=left
  pactl load-module module-alsa-source device=right
fi
pactl set-default-source alsa_input.left
