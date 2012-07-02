avr-objcopy -R .eeprom -O binary XReader.elf  "XReader.bin"
avr-objcopy -R .eeprom -O ihex XReader.elf  "XReader.hex"
