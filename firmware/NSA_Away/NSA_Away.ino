/*
 NSA Away main application. Includes the UI,
 random number generator, USB HID keyboard, and
 SD card interaction.
 
 created   August 2014
 by Bob Baddeley
 
 This example code is in the public domain. 	 
 */
#include <SdFat.h>
#include <SdFatUtil.h>
#if !USE_MULTIPLE_CARDS
#error You must set USE_MULTIPLE_CARDS nonzero in SdFatConfig.h
#endif

#include <avr/pgmspace.h>

// include the library code:
#include <LiquidCrystal.h>

//pin for the backlight
#define LCD_BACKLIGHT 3

//#include <SoftwareSerial.h>

//#define SERIAL_IN 7
//#define SERIAL_OUT 17

//SoftwareSerial inputSerial(SERIAL_IN,SERIAL_OUT); //Rx, Tx

//left SD card access
SdFat sd_left;
const uint8_t SD_LEFT_CS = 4;  // chip select for sd_left
const uint8_t SD_LEFT_WP = 6;  //write protect test
//right SD card access
SdFat sd_right;
SdFile file_left;
SdFile file_right;

const uint8_t SD_RIGHT_CS = 5;   // chip select for sd_right
const uint8_t SD_RIGHT_WP = 7;  //write protect test
//temp variables for how many bytes to read/write
//how many we've read so far
long char_count = 0;
//how many we want to read
long char_count_destination = 0;
//1=bytes, 1024 = kilobytes, etc.
long char_count_multiplier = 1;

const uint8_t BUF_DIM = 10;
uint8_t buf[BUF_DIM];

//for the input buttons
int sensor_value = 0;
//for the LCD
char buffer[54];

//where the keys will be stored
//this is defined by the Android app rules
//const char folder_location[] = "/storage/extSdCard/Android/data/org.sector67.nsaaway/files/keys/";
//gonna take some work to get it to go that deep in the folder structure. let's start with this first level
const char folder_location[] = "/storage/";

//                                    First Row     Second Row
//                               11111111111111112222222222222222
prog_char const string_00[] PROGMEM = "                                ";//0
prog_char const string_01[] PROGMEM = "initializing...                 ";//1
prog_char const string_02[] PROGMEM = "complete!       ok             >";//2
prog_char const string_03[] PROGMEM = "error.          oops.          >";//3
prog_char const string_10[] PROGMEM = "Random        ^>to keyboard   v ";//4
prog_char const string_11[] PROGMEM = "length       <^>(unlimited)   v ";//5
prog_char const string_12[] PROGMEM = "start          >cancel         <";//6
prog_char const string_13[] PROGMEM = "printing...     stop           <";//7
prog_char const string_20[] PROGMEM = "Random        ^>to SD cards   v ";//8
prog_char const string_21[] PROGMEM = "length       <^>checking...   v ";//9
prog_char const string_22[] PROGMEM = "start          >cancel         <";//10
prog_char const string_23[] PROGMEM = "generating...   stop           <";//11
prog_char const string_30[] PROGMEM = "Copy Left     ^>to Right      v ";//12
prog_char const string_31[] PROGMEM = "file          ^>1.txt         v ";//13
prog_char const string_32[] PROGMEM = "start          >cancel         <";//14
prog_char const string_33[] PROGMEM = "copying file... stop           <";//15
prog_char const string_40[] PROGMEM = "Left Card     ^>to Keyboard   v ";//16
prog_char const string_41[] PROGMEM = "file          ^>1.txt         v ";//17
prog_char const string_42[] PROGMEM = "offset       <^>0             v ";//18
prog_char const string_43[] PROGMEM = "length       <^>(full)        v ";//19
prog_char const string_44[] PROGMEM = "start          >cancel         <";//20
prog_char const string_45[] PROGMEM = "printing...     stop           <";//21
prog_char const string_50[] PROGMEM = "Left Card     ^>to Screen     v ";//22
prog_char const string_51[] PROGMEM = "file          ^>1.txt         v ";//23
prog_char const string_52[] PROGMEM = "offset       <^>0             v ";//24
prog_char const string_53[] PROGMEM = "length       <^>(full)        v ";//25
prog_char const string_54[] PROGMEM = "start          >cancel         <";//26
prog_char const string_55[] PROGMEM = "printing...     stop           <";//27
prog_char const string_60[] PROGMEM = "Serial in     ^>to keyboard   v ";//28
prog_char const string_61[] PROGMEM = "printing...     stop           <";//29

//mapping the strings to an array to make it easier to refer to the strings
//we can also use the string table as the UI state where each string is a different
//state of our machine
const char* const string_table[] PROGMEM = 
{
	string_00,
	string_01,
	string_02,
	string_03,
	string_10,
	string_11,
	string_12,
	string_13,
	string_20,
	string_21,
	string_22,
	string_23,
	string_30,
	string_31,
	string_32,
	string_33,
	string_40,
	string_41,
	string_42,
	string_43,
	string_44,
	string_45,
	string_50,
	string_51,
	string_52,
	string_53,
	string_54,
	string_55,
        string_60,
        string_61
};

byte current_ui_state = 1;

/*
0=UP
1=DOWN
2=LEFT
3=RIGHT
4=LEFT SD PRESENT
5=RIGHT SD PRESENT
6=LEFT SD WRITE PROTECT
7=RIGHT SD WRITE PROTECT
*/
byte button_state = 0;
byte prev_button_state = 0;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 10,11,12,13);

void setup() {
  
  //Serial.begin(38400);
  //pinMode(SERIAL_IN,INPUT);
  //pinMode(SERIAL_OUT,OUTPUT);
  pinMode(SD_LEFT_WP,INPUT);
  pinMode(SD_RIGHT_WP,INPUT);
  //inputSerial.begin(38400);
  //inputSerial.listen();
  Serial1.begin(19200);
  //set up the LCD. The backlight is on a MOSFET
  //so we can control it easily.
  pinMode(LCD_BACKLIGHT,OUTPUT);
  //high turns on the backlight.
  digitalWrite(LCD_BACKLIGHT, HIGH);
  
  // turn on the USB keyboard:
  Keyboard.begin();
  
  digitalWrite(A4, HIGH);  // set pullup on analog pin 4 for detecting SD presence
  digitalWrite(A5, HIGH);  // set pullup on analog pin 5 for detecting SD presence
  digitalWrite(SD_LEFT_WP,HIGH);
  digitalWrite(SD_RIGHT_WP,HIGH);
  
  //temporary! Only doing this until we get the hardware RNG working
  randomSeed(0);
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  //setting it up to go!
  // Print a message to the LCD.
  current_ui_state = 4;//or maybe 2
  lcdPrintLongMessage(current_ui_state);
}

void loop() {
  //read the buttons to see if any were pressed.
  //changing SD card presence doesn't change the UI,
  //so don't bother checking those.
  readButtonStates(false);
  //see if the button states have changed and how it
  //changes the state machine
  processButtons();
  //do some things based on our current UI state if we're
  //supposed to be in the middle of a process
switch(current_ui_state){
    case 7: //random to keyboard
      if (char_count < (char_count_destination * char_count_multiplier)){
        char_count++;
        //we're calling getRandomChar() twice because we need two hex characters.
        //when we switch to the RNG, we'll be able to do this a lot cleaner since we'll
        //be getting the whole byte at once instead of this hacked method of getting
        //two nibbles.
        Keyboard.write(getRandomChar());
        Keyboard.write(getRandomChar());
      }
      //we're done!
      else {
        char_count = 0;
        char_count_destination = 0;
        current_ui_state = 4; 
        updateUIState(current_ui_state);
      }
      break;
    case 11: //Random to SD Cards
      if (char_count == 0){
        if (!sd_left.exists(folder_location)) {
          sd_left.mkdir(folder_location);
        }
        sd_left.chdir(folder_location);
        sd_left.chvol();
        file_left.open("TEST.BIN", O_RDWR | O_CREAT | O_TRUNC);
        if (!sd_right.exists(folder_location)) {
          sd_right.mkdir(folder_location);
        }
        sd_right.chdir(folder_location);
        sd_right.chvol();
        file_right.open("TEST.BIN", O_RDWR | O_CREAT | O_TRUNC);
      }
      if (char_count < (char_count_destination * char_count_multiplier)){
        char_count++;
        char b = getRandomByte();
        file_left.write(b);
        file_right.write(b);
      }
      else {
        char_count = 0;
        char_count_destination = 0;
        current_ui_state = 2; 
        updateUIState(current_ui_state);
        file_left.close();
        file_right.close();
      }
      break;
    case 21:
      if (char_count == 0){
        if (!sd_left.exists(folder_location)) {
          sd_left.mkdir(folder_location);
        }
        sd_left.chdir(folder_location);
        sd_left.chvol();
        file_left.open("TEST.BIN", O_RDWR);
      }
      if (char_count < (char_count_destination * char_count_multiplier)){
        char_count++;        
        file_left.read(buf,1);
        Keyboard.print(buf[0],HEX);
      }
      else {
        char_count = 0;
        char_count_destination = 0;
        current_ui_state = 2; 
        updateUIState(current_ui_state);
        file_left.close();
      }
      break;
    case 29:
      if (Serial1.available()){
        char c = Serial1.read();
        Keyboard.write(c);
      }
      break;
    default:
      delay(100); 
  }
}

/* reads the button states to see if they've changed from the previous read.
  could do some debouncing here, but meh.
  
  include_sd says whether we should bother checking the sd inputs (presence and write protect).
  if we don't check them it saves some time, so we don't always need to bother.
  
  by the end we have a byte that represents the 8 different inputs.
  */
void readButtonStates(boolean include_sd){
  prev_button_state = button_state;
  button_state = 0;
  if (include_sd == true){
    // There's no reason to check the SD card presence unless we need to.
    // read the value from the left SD card to detect presence:
    sensor_value = analogRead(A4);
    //sensor_value = 1000;//just for testing. It's not working right now.
    button_state = button_state | (sensor_value<512?1:0);
    // read the value from the right SD card to detect presence:
    sensor_value = analogRead(A5);
    //sensor_value = 1000;//just for testing. It's not working right now.
    button_state = button_state<<1;
    button_state = button_state | (sensor_value<512?1:0);
    //TODO check the write protect bits
    sensor_value = digitalRead(6);
    button_state = button_state<<1;
    button_state = button_state | 1;//sensor_value;
    sensor_value = digitalRead(7);
    button_state = button_state<<1;
    button_state = button_state | 1;//sensor_value;
  }
  // read the value from the left button:
  sensor_value = analogRead(A3);
  button_state = button_state<<1;
  button_state = button_state | (sensor_value>512?1:0);
  // read the value from the right button:
  sensor_value = analogRead(A2);
  button_state = button_state<<1;
  button_state = button_state | (sensor_value>512?1:0);
  // read the value from the up button:
  sensor_value = analogRead(A1);
  button_state = button_state<<1;
  button_state = button_state | (sensor_value>512?1:0);
  // read the value from the down button:
  sensor_value = analogRead(A0);
  button_state = button_state<<1;
  button_state = button_state | (sensor_value>512?1:0);
}

//UI
//UI state
//if the button state has changed, find out which button was pressed
//for each of the different buttons depending on UI state
void processButtons(){
    boolean update_ui = false;//do we need to change the UI? Default to no.
	//if the button was UP
  if (button_state == 2 && prev_button_state == 0){
    switch(current_ui_state){
      case 4:
        current_ui_state = 28;
        update_ui = true;
        break;
      case 5:
      case 19:
      case 25:
        char_count_destination++;
        lcdPrintCharSize();
        break;
      case 1:
      case 2:
      case 3:
      case 8:
        current_ui_state = 4;
        update_ui = true;
       	break;
      case 9:
        char_count_destination++;
        lcdPrintCharSize();
        break;
      case 12:
        current_ui_state = 8;
        update_ui = true;
        break;
      case 16:
        current_ui_state = 12;
        update_ui = true;
        break;
      case 22:
        current_ui_state = 16;
        update_ui = true;
        break;
      case 28:
        current_ui_state = 22;
        update_ui = true;
        break;
      default:
        break;
    }
  }
  if (button_state == 1 && prev_button_state == 0){
    //if the button was DOWN
    switch(current_ui_state){
      case 4:
        current_ui_state = 8;
        update_ui = true;
        break;
      case 5:
      case 19:
      case 25:
        if (char_count_destination > 0){
          char_count_destination--;
        }
        lcdPrintCharSize();
        break;
      case 8:
        current_ui_state = 12;
        update_ui = true;
	break;
      case 9:
        if (char_count_destination > 0){
          char_count_destination--;
        }
        lcdPrintCharSize();
        break;
      case 12:
        current_ui_state = 16;
        update_ui = true;
        break;
      case 16:
        current_ui_state = 22;
        update_ui = true;
        break;
      case 22:
        current_ui_state = 28;
        update_ui = true;
        break;
      case 1:
      case 2:
      case 3:
      case 28:
        current_ui_state = 4;
        update_ui = true;
        break;
      default:
        break;
    }
  }
  if (button_state == 8 && prev_button_state == 0){
    //if the button was LEFT
    switch(current_ui_state){
      case 5:
      case 9:
      case 19:
      case 25:
        switch(char_count_multiplier){
          case 1:
            char_count_multiplier = 1024;
            break;
          case 1024:
            char_count_multiplier = 1048576;
            break;
          case 1048576:
            char_count_multiplier = 1;
            break;
        }
        lcdPrintCharSize();
        break;
      case 1:
      case 2:
      case 3:
      case 6:
      case 7:
        current_ui_state = 4;
        update_ui = true;
	break;
      case 10:
      case 11:
        current_ui_state = 8;
        update_ui = true;
	break;
      case 14:
      case 15:
        current_ui_state = 12;
        update_ui = true;
	break;
      case 20:
      case 21:
        current_ui_state = 16;
        update_ui = true;
	break;
      case 26:
      case 27:
        current_ui_state = 22;
        update_ui = true;
	break;
      case 29:
        current_ui_state = 28;
        update_ui = true;
        break;
      default:
        break;
    }
  }
  if (button_state == 4 && prev_button_state == 0){
    //if the button was RIGHT
    switch(current_ui_state){
      case 1:
      case 2:
      case 3:
        current_ui_state = 4;
        update_ui = true;
        break;
      case 4:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 5:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 6:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 8:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 9:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 10:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 12:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 13:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 14:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 16:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 17:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 18:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 19:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 20:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 22:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 23:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 24:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 25:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 26:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      case 28:
        current_ui_state = current_ui_state + 1;
        update_ui = true;
        break;
      default:
        break;
    }
  }
  if(update_ui==true){
    updateUIState(current_ui_state); 
    switch (current_ui_state){
      case 5:
        lcdPrintCharSize();
        break;
      case 9:
        if (!testCard(true,true)){
         current_ui_state = 3; 
        }
        if (!testCard(false,true)){
         current_ui_state = 3; 
        }
        lcdPrintCharSize();
        break;
      case 19:
      case 25:
         lcdPrintCharSize();
         break; 
      case 13:
        if (!testCard(false,true)){
         current_ui_state = 3; 
        }
      case 17:
      case 23:
        if (!testCard(true,false)){
         current_ui_state = 3; 
        }
        break;
    }
  }
}

void updateUIState(byte state){
	current_ui_state = state;
	//now update the display
	lcdPrintLongMessage(current_ui_state);
}

void lcdPrintCharSize(){
  lcd.setCursor(0,1);
  lcd.print("               ");
  lcd.setCursor(0,1);
  lcd.print(char_count_destination);
  lcd.print(" ");
  switch(char_count_multiplier){
    case 1:
      lcd.print("bytes");
      break;
    case 1024:
      lcd.print("kb");
      break;
    case 1048576:
      lcd.print("mb");
      break;
  } 
}

//takes a single string and prints it to the screen.
//used for updating the UI.
void lcdPrintLongMessage(byte message){
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

boolean testCard(boolean left, boolean wp){
  boolean success = false;
  readButtonStates(true);
  if (button_state & (left?128:64)){
    success = left?sd_left.begin(SD_LEFT_CS):sd_right.begin(SD_RIGHT_CS);
    if (!success){
      lcd.setCursor(0,1);
      lcd.print("              ");
      lcd.setCursor(0,1);
      lcd.print("Card Error");
      return false;
    }
    else if (wp && button_state & (left?32:16)){
      lcd.setCursor(0,1);
      lcd.print("              ");
      lcd.setCursor(0,1);
      lcd.print("Write Protect");
      return false;
    }
   }
   else {
    lcd.setCursor(0,1);
    lcd.print("              ");
    lcd.setCursor(0,1);
    lcd.print("No Card");
    return false;
  }
  return true;
}

//this is such a hack to get a random hex character.
byte getRandomChar(){
  byte b = random(16);
  return b<10?b+48:b+55;
}

byte getRandomByte(){
  byte b = random(255);
  return b; 
}
