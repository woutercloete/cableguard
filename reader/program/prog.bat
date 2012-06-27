avr-objcopy -R .eeprom -O binary XReader.elf  "XReader.bin"
avr-objcopy -R .eeprom -O ihex XReader.elf  "XReader.hex"
atprogram -t jtagicemkii -i jtag -v -cl 1Mhz -d atmega128 program -c -fl -f XReader.hex
#atprogram -t jtagice3 -i jtag -v -cl 1Mhz -d atmega128 program -c -fl -f XReader.hex
#atprogram -t avrone -i jtag -v -cl 1Mhz -d atmega128 program -c -fl -f XReader.hex