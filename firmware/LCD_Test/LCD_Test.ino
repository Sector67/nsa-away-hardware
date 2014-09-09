/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the 
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 
 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 
 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 */

// include the library code:
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>

prog_char string_0[] PROGMEM = "12345678901234567890123456789012";
prog_char string_1[] PROGMEM = "23456789012345678901234567890123";
prog_char string_2[] PROGMEM = "34567890123456789012345678901234";

PROGMEM const char *string_table[] = 
{
 string_0,
 string_1,
 string_2 
};

char buffer[33];
// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
LiquidCrystal lcd(8, 9, 10,11,12,13);

void setup() {
  pinMode(3,OUTPUT);
  digitalWrite(3,HIGH);
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("hello, world!");
  lcd_print_long_message(0);
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  //lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  //lcd.print(millis()/1000);
  delay(100);
}

void lcd_print_long_message(int message){
 int numChar = 0;
 strcpy_P(buffer, (char*)pgm_read_word(&(string_table[message])));
 lcd.setCursor(0,0);
 for (numChar = 0; numChar < 16;numChar++){
   lcd.print(buffer[numChar]);
 }
 lcd.setCursor(0,1);
 for (numChar = 0; numChar < 16;numChar++){
   lcd.print(buffer[numChar+16]);
 }
}
