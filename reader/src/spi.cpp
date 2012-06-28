#include "spi.h"
#include <avr/io.h>

#define wait_Spi()  while(!(SPSR & _BV(SPIF)))



void SPI::init() 
{
  // Configure SPI pins
  sbi(PORTB,SPI_SS); // SS HIGH

  sbi(DDRB,SPI_SS);     // SS OUTPUT
  sbi(DDRB, SPI_MOSI);  // set MOSI as output
  cbi(DDRB, SPI_MISO);  // set MISO as input
  sbi(DDRB, SPI_SCK);   // set SCK as output
  

  sbi(PORTB,SPI_SCK); // set SCK HIGH
  cbi(PORTB,SPI_MOSI); // set MOSI LOW

  // SPI speed = clk/4
  SPCR = _BV(SPE) | _BV(MSTR);
}

u08 SPI::send(u08 value) 
{
  SPDR = value;                          // Transfer byte via SPI
  wait_Spi();                            // Wait until SPI operation is terminated
  return SPDR;
}

