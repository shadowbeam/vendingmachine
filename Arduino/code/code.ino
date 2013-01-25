#include <aJSON.h>
#include <Coil.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>

/* Credit setup */

#define creditLoc 0

/* Coil SETUP */
Coil coils[] = {
  Coil(9,8,36,1),
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

int credit;

void setup() {
  Serial.begin(9600);
  Serial.println("START");  
  lcd.begin(16, 2);
  credit = getCredit();
/*  attachInterrupt(2, keyInterrupt, CHANGE);
  attachInterrupt(3, keyInterrupt, CHANGE);
  attachInterrupt(4, keyInterrupt, CHANGE);*/
}

void loop() {
  lcdPrintln("Enter code");
  delay(50);
  keyInterrupt();
  
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
      str[0] = 0;
      break;
    }
    
    if (key == '#' && id == 33) {
      setStockGetId();
      break;
    }
    
    if (key == '#' && id == 99) {
      changeCredit(1);
      lcdPrintlnAndBlock("Added credit", 4);
      break;
    }
  
    if (key == '#' && !validId(id) && id != 33 && id != 99) {
      lcdPrintlnAndBlock("Invalid Id", 4);
      break;
    }
    
    if (key == '#' && validId(id) && coils[id-1].isEmpty()) {
      lcdPrintlnAndBlock("Out of stock", 4);
      break;
    }
    
    if (key == '#' && !isCredit()) {
      lcdPrintlnAndBlock("Out of credit", 4);
      break;
    }
    
    if (key == '#' && validId(id) && !coils[id-1].isEmpty() && !isCredit()) {
      lcdPrintlnAndBlock("Out of credit", 4);
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
    id = atoi(str);
    milliseconds += 10;
    delay(10);
    key = kpd.getKey();
    if (key == '#') {
      lcdPrintln("Vending product");
      coils[id - 1].vend();
      lcdPrintlnAndBlock("Thank you", 4);
      changeCredit(-1);
      break;
    }
  }
}

/** setStockGetId
 *  loops until # is pressed and vend completes or
 *  four seconds elapse
 */
void setStockGetId() {
  lcdPrintln("Enter id");
  milliseconds = 0;
  str[0] = 0;
  strIndex = 0;
  
  while ( milliseconds < 4000) {
    id = atoi(str);
    milliseconds += 10;
    delay(10);
    key = kpd.getKey();
    if (key == '#' && validId(id)) {
      setStockGetQuant(id);
      break;
    }
    
    if (key == '#' && !validId(id)) {
      lcdPrintlnAndBlock("Invalid ID", 4);
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

/** setStockGetQuant
 *  loops until # is pressed and vend completes or
 *  four seconds elapse
 */
void setStockGetQuant(int coilId) {
  lcdPrintln("Enter quant");
  milliseconds = 0;
  str[0] = 0;
  strIndex = 0;
  
  while ( milliseconds < 4000) {
    id = atoi(str);
    milliseconds += 10;
    delay(10);
    key = kpd.getKey();
    if (key == '#') {
      coils[coilId - 1].setStock(id);
      sprintf(str, "Stock: %2d", id);
      lcdPrintlnAndBlock(str, 4);
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
  
  int i = id->valueint;
  
  if (strcmp(cmd->valuestring, "stock") != 0 && strcmp(cmd->valuestring, "vend") && strcmp(cmd->valuestring, "addcredit") != 0 && strcmp(cmd->valuestring, "getcredit")) {
    aJson.addStringToObject(msg, "err", "Command not recognized");
  }
  
  else if (validId(i) && (strcmp(cmd->valuestring, "stock") == 0 || strcmp(cmd->valuestring, "vend") == 0)) {
    if (strcmp(cmd->valuestring, "stock") == 0 && coils[id->valueint - 1].isEmpty()) {
      aJson.addFalseToObject(msg, "res");
    }
    
    if (strcmp(cmd->valuestring, "stock") == 0 && !coils[id->valueint - 1].isEmpty()) {
      aJson.addTrueToObject(msg, "res");
    }
    
    if (strcmp(cmd->valuestring, "vend") == 0 && coils[i - 1].isEmpty()) {
      aJson.addFalseToObject(msg, "res");
    }
    
    if (strcmp(cmd->valuestring, "vend") == 0 && !coils[i - 1].isEmpty()) {
      aJson.addTrueToObject(msg, "res");
      coils[i - 1].vend();
    }
  
  }
  
  else if (strcmp(cmd->valuestring, "addcredit") == 0) {
    changeCredit(i);
    aJson.addTrueToObject(msg, "res");
  }
  
  else if (strcmp(cmd->valuestring, "getcredit") == 0) {
    aJson.addNumberToObject(msg, "res", getCredit());
  }
  
  else { // Invalid id
    aJson.addStringToObject(msg, "err", "Id does not exist");
  }
  
  aJson.print(msg, &serial_stream);
  Serial.println(); // finish serial write with CRLF
}

/*** CREDIT EEPROM FUNCTIONS ***/

/** getCredit
 *  retrieves the stored credit from EEPROM
 */
 
int getCredit() {
  return EEPROM.read(creditLoc);
}

/** setCredit
 *  sets the stored credit in EEPROM
 */
void setCredit(int credit) {
  EEPROM.write(creditLoc, credit);
}
/*
/** changeCredit
 *  adds the specified credit to the stored
 *  credit in EEPROM
 */
void changeCredit(int change) {
  setCredit(getCredit() + change);
  credit += change;
}

/** isCredit
 *  checks if there is credit in EEPROM
 */
boolean isCredit() {
  return credit > 0;
}

/*** UTIL ***/

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
  lcd.setCursor(0,0);
  char creditString[10];
  sprintf(creditString, "Credit: %2d", credit);
  lcd.write(creditString);
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
