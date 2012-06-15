#include <string.h>
#include <stdio.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "avrlibdefs.h"
#include "types.h"
#include "Clcd.h"

#define BUSY		0x80

Clcd::Clcd(void) {
}

void Clcd::init() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    _delay_ms(500);
    delay(LCD_DELAY + 500);
    LCD_RW_INIT();
    LCD_RS_INIT();
    LCD_EN_INIT();
    LCD_DATA_DDR |= 0xF0;
    LCD_DISABLE();
    LCD_RSOFF();
    LCD_RWOFF();
    delay(LCD_DELAY + 500);
    command(LCD_SET_SYSTEM | LCD_SYS_4BIT | LCD_SYS_2LINE);
    command(LCD_SET_CUR_SHIFT | LCD_DISPLAY_SHIFT | LCD_SURSOR_SHIFT_RIGHT);
    command(LCD_SET_DISPLAY | LCD_DISP_DISPLAY_ON); //| LCD_DISP_CURSOR_ON | LCD_DISP_CURSOR_FLASH);
    command(0x01);
    command(LCD_CURSOR_HOME);
    delay(LCD_DELAY + 4000);
    clear();
    gotoXY(0, 0);
  }
}

void Clcd::delay(u32 time) {
  volatile unsigned int i, j;
  while (time) {
    for (i = 0; i < 3; i++)
      for (j = 0; j < 1; j++)
        ;
    time--;
  }
}

/**
 @brief	Check to be ready LCD.
 @return 	If it's ready ,then '1',else then '-1'. If LCD Device is not insert actually,then return '-1'.
 */
void Clcd::ready(void) {
  volatile unsigned int i, j, time;
  time = LCD_DELAY;

  while (time) {
    for (i = 0; i < 3; i++)
      for (j = 0; j < 1; j++)
        ;
    time--;
  }
}

void Clcd::command(u08 Value) {
  delay(LCD_DELAY); //busy check
  LCD_RSOFF();
  LCD_RWOFF();
  LCD_DATA(Value);
  LCD_ENABLE();
  LCD_DISABLE();
}

/**
 @brief		Clear LCD.
 */
void Clcd::clear(void) {
  ready();
  command(0x01);
}

void Clcd::data(u08 ch) {
  delay(LCD_DELAY); //busy check
  LCD_RSON(); //RS On
  LCD_RWOFF();
  LCD_DATA(ch);
  LCD_ENABLE();
  LCD_DISABLE();
}

void Clcd::printf(u08 x, u08 y, u08 mode, const char *__fmt, ...) {
  u08 len;
  c08 str[255];
  va_list arg;
  va_start(arg, __fmt);
  vsprintf(str, __fmt, arg);
  va_end(arg);

  if (bit_is_set(mode,1)) {
    clear();
  }
  // When selected XY is speced
  if (bit_is_set(mode,0)) {
    gotoXY(x, y);
  }
  len = MIN(strlen(str),LCD_MAX_COL);
  puts((u08*) str, len);
}

/**
 @brief		Move Cursor to X Column, Y Row.

 LCD to be offered by WIZnet is '2*16' Dimension, so Row(Argument y) is not above 1.
 */
void Clcd::gotoXY(u08 row, u08 col) {
  ready();
  switch (col) {
    case 0:
      command(0x80 + row);
      break;
    case 1:
      command(0xC0 + row);
      break;
    case 2:
      command(0x94 + row);
      break;
    case 3:
      command(0xD4 + row);
      break;
  }
}

/**
 @brief		Output character string in current Cursor.
 @return 	Character string's Pointer to be output
 */
u08* Clcd::puts(u08* str, u08 len) {
  unsigned char i;

  for (i = 0; i < len; i++) {
    ready();
    data(str[i]);
  }
  return str;
}

/**
 @brief		Output 1 character in current Cursor.
 @return 	Character string's Pointer to be output
 */
void Clcd::putch(u08 ch) {
  ready();
  data(ch);
}

#ifndef __LCD_UNUSED
/**
 @brief		Decide Cursor type.
 */
void lcd_set_cursor_type(
    u_char type /**< type - Cursor type(INPUT) */
)
{
  lcd_ready();
  switch(type)
  {
    //No Cursor
    case 0 : command(0x0C); break;
    // Normal Cursor
    case 1 : command(0x0E); break;
    // No Cursor | Blink
    case 2 : command(0x0D); break;
    // Normal Cursor | Blink
    case 3 : command(0x0F); break;
  }

  /**
   @brief		Shift to Left and Right current Cursor.
   */
  void ShiftCursor(
      u_char dir /**< dir - Decide direction to be Shift.(INPUT)  dir !=0  -> Right Shift, dir == 0 -> Left Shift */
  )
  {
    lcd_ready();
    command((dir ? 0x14: 0x10));
  }

  /**
   @brief		Move Cursor first Column.
   */
  void lcd_home_cursor(void)
  {
    lcd_ready();
    command(2);
  }

#endif
