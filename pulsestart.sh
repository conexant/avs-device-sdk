if [ -z "$PACTLSET" ]; then
  pactl load-module module-alsa-source device=left
  pactl load-module module-alsa-source device=right
  export PACTLSET="1"
fi
pactl set-default-source alsa_input.left
