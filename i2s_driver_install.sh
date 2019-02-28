#!/bin/bash

Origin="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
kernelRelease=$(uname -r)
sudo cp $Origin/snd-soc-cx2092x.ko /lib/modules/$kernelRelease/kernel/sound/soc/codecs/ -v
sudo cp $Origin/snd-soc-cxsmtspk-pi-i2s.ko /lib/modules/$kernelRelease/kernel/sound/soc/bcm/ -v
sudo cp $Origin/i2s-config.txt /boot/config.txt -v