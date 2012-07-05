avr-objcopy -R .eeprom -O binary %1.elf  %1.bin
avr-objcopy -R .eeprom -O ihex %1.elf  %1.hex
atprogram -t jtagicemkii -i jtag -v -cl 1Mhz -d atmega128 program -c -fl -f %1.hex
rem atprogram -t jtagice3 -i jtag -v -cl 1Mhz -d atmega128 program -c -fl -f XReader.hex
rem atprogram -t avrone -i jtag -v -cl 1Mhz -d atmega128 program -c -fl -f XReader.hex