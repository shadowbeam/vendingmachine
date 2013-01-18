#include <aJSON.h>
#include <Coil.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>
#include <String.h>

/* Coil SETUP */
Coil coils[] = {
  Coil(9,8,36),
};

/* KEYPAD SETUP */
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {43, 53, 51, 47}; //connect to the row pinouts of the keypad
// TODO CHANGE TO 21, 20, 19
byte colPins[COLS] = {45, 41, 49}; //connect to the column pinouts of the keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

/* LCD SETUP */
LiquidCrystal lcd(22, 24, 26, 28, 30, 32);

/* SERIAL JSON STREAM SETUP */
aJsonStream serial_stream(&Serial);

/* EEPROM LOCATION SETUP */
int eepromLoc[] = {0x0A, 0x0B, 0x0C, 0x0D};

void setup() {
  Serial.begin(9600);
  Serial.println();  
  lcd.begin(16, 2);
  attachInterrupt(2, keyInterrupt, CHANGE);
  attachInterrupt(3, keyInterrupt, CHANGE);
  attachInterrupt(4, keyInterrupt, CHANGE);
}

void loop() {
  // TODO implement scrolling text
}

/*** KEYPAD interrupt ***/

int  milliseconds;
char key;
int id;
char str[10];
int strIndex;

/** keyInterrupt
 *  If key is '*' resets the state
 *  If key is '#' it checks stock, if in
 *  If in stock another '#' dispenses it
 *  Any number is added to an array for
 *  future use
 */
void keyInterrupt() {
  milliseconds = 0;
  str[0] = 0;
  strIndex = 0;
  
  while (milliseconds < 4000) { // wait 4 secs for additional input
    key = kpd.getKey();
    id = atoi(str);
  
    if (!key) { // no key pressed, increment time
      milliseconds += 10;
      delay(10);
    }
    
    if (key == '*') { // exit interrupt
      break;
    }
  
    if (key == '#' && !validId(id)) {
      lcdPrintlnAndBlock("Invalid Id", 4);
      break;
    }
    
    if (key == '#' && validId(id) && coils[id-1].isEmpty()) {
      lcdPrintlnAndBlock("Out of stock", 4);
      break;
    }
    
    if (key == '#' && validId(id) && !coils[id-1].isEmpty()) {
      confirmVend();
      break;
    }
    
    if (isdigit(key)) { // add number to string
      str[strIndex++] = key;
      str[strIndex] = 0;
      lcdPrintln(str);
      milliseconds= 0;
    }
  }
}

/** confirmVend
 *  loops until # is pressed and vend completes or
 *  four seconds elapse
 */
void confirmVend() {
  lcdPrintln("Press # to vend");
  milliseconds = 0;
  
  while ( milliseconds < 4000) {
    milliseconds += 10;
    delay(10);
    key = kpd.getKey();
    if (key && key == '#') {
      coils[id - 1].vend();
      lcdPrintlnAndBlock("Vending product", 4);
      lcdPrintlnAndBlock("Thank you", 4);
      break;
    }
  }
}

/*** SERIAL ***/

/** serialEvent()
 *  this method is called after loop() whenever there is input
 *  to the serial port. It filters out any preceding white space
 *  calls handleJson() if there is data to be parsed.
 */
void serialEvent() {
  serial_stream.skip(); // Skip any whitespace
  if (serial_stream.available()) { // we have something to parse
    parseSerial();
  }
}

void parseSerial() {
  aJsonObject *msg = aJson.parse(&serial_stream);
  aJsonObject *cmd = aJson.getObjectItem(msg, "cmd");
  aJsonObject *id = aJson.getObjectItem(msg, "id");
  
  char *s = cmd->valuestring;
  int i = id->valueint;
  
  if (!msg || !cmd || !id) { // Incomplete or incorrect JSON
    while (Serial.available() > 0) {
      Serial.read(); // get rid of serial buffer
    }
    Serial.println('{"err":"Invalid JSON"}');
    return;
  }
  
  if (i && !validId(i)) { // Invalid id
    aJson.addStringToObject(msg, "err", "Id does not exist");
  }
  
  if (strcmp(s, "stock") == 0 && validId(i) && coils[i - 1].isEmpty()) {
    aJson.addFalseToObject(msg, "res");
  }
  
  if (strcmp(s, "stock") == 0 && validId(i) && !coils[i - 1].isEmpty()) {
    aJson.addTrueToObject(msg, "res");
  }
  
  if (strcmp(s, "vend") == 0 && validId(i) && coils[i - 1].isEmpty()) {
    aJson.addFalseToObject(msg, "res");
  }

  
  if (strcmp(s, "vend") == 0 && validId(i) && !coils[i - 1].isEmpty()) {
    aJson.addTrueToObject(msg, "res");
  }

  //not enough credit error
  if (strcmp(s, "vend") == 0 && validId(i) && !coils[i - 1].isEmpty() && getCredit() == 0) {
    aJson.addStringToObject(msg, "res", "Not enough credit");
  }
  
  if (strcmp(s, "addcredit") == 0) {
    setCredit(getCredit() + 1);
    aJson.addTrueToObject(msg, "res", getCredit());
  }

  
  if (strcmp(s, "getcredit") == 0) {
    aJson.addNumberToObject(msg, "res", getCredit());
  }
  
  if (strcmp(s, "stock") != 0 && strcmp(s, "vend") && strcmp(s, "addcredit") != 0 && strcmp(s, "getcredit")) {
    aJson.addStringToObject(msg, "err", "Command not recognized");
  }
  
  aJson.print(msg, &serial_stream);
  Serial.println(); // finish serial write with CRLF
}

/*** UTIL ***/

/** getCredit
 *  retrieves the stored credit from EEPROM
 */
int getCredit() {
  int credit = 0;
  credit = (credit << 8) + EEPROM.read(eepromLoc[0]);
  credit = (credit << 8) + EEPROM.read(eepromLoc[1]);
  credit = (credit << 8) + EEPROM.read(eepromLoc[2]);
  credit = (credit << 8) + EEPROM.read(eepromLoc[3]);
}

/** getCredit
 *  sets the stored credit in EEPROM
 */
void setCredit(int credit) {
  EEPROM.write(eepromLoc[3], credit & 0xFF);
  credit >>= 8;
  EEPROM.write(eepromLoc[2], credit & 0xFF);
  credit >>= 8;
  EEPROM.write(eepromLoc[1], credit & 0xFF);
  credit >>= 8;
  EEPROM.write(eepromLoc[0], credit & 0xFF);
}

/** checkId
 *  returns true if coil id is valid, false otherwise
 */
boolean validId(int id) {
  int arraySize = sizeof(coils) / sizeof(coils[0]);
  return id >= 1 && id <= arraySize;
}

/** lcdPrintln
 *  this method clears the screen, resets cursor
 *  to beginning and writes a new string to the
 *  LCD
 */
void lcdPrintln(char *s) {
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.write(s);
}

/** lcdPrintlnAndBlock
 *  as lcdPrintln with the addition of blockiing the program for
 *  seconds specified in parameters
 */
void lcdPrintlnAndBlock(char *str, int secs) {
  lcdPrintln(str);
  delay(1000 * secs);
}

