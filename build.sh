#!/bin/sh

ino=m5stack-os.ino
esp32=https://dl.espressif.com/dl/package_esp32_index.json

rm -rf firmware/m5stack-launcher.*

./tools/arduino-cli --additional-urls=$esp32 --verbose compile -b esp32:esp32:m5stack-core-esp32 $ino -o ./firmware/m5stack-launcher

