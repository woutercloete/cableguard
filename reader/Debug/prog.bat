avr-objcopy -R .eeprom -O binary %1.elf  %1.bin
avr-objcopy -R .eeprom -O ihex %1.elf  %1.hex
rem atprogram -t avrispmk2 -i isp -v -cl 125khz -d  atmega128 program -c -fl -f %1.elf
atprogram -t jtagicemkii -i jtag -v -cl 1000khz -d atmega128 program -c -fl -f %1.elf
rem atprogram -t jtagice3 -i jtag -v -cl 125khz -d atmega168pa program -c -fl -f %1.elf
