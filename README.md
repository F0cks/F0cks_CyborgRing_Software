# F0cks_CyborgRing_Software
avrdude -c usbasp  -p t85 -U lfuse:w:0x43:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m   -B 400
avrdude -c usbasp  -p t85 -U flash:w:cyborg_ring_software.ino.hex -B 400 