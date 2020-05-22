#!/bin/sh

port=/dev/tty.SLAB_USBtoUART
 
esptool.py --chip esp32 --port $port write_flash \
    0x8000  firmware/m5stack-launcher.partitions.bin \
    0x10000 firmware/m5stack-launcher.bin