#include <aJSON.h>
#include <Coil.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <String.h>

/* Coil SETUP */
Coil coils[] = {
  Coil(1,2,3,100),
  Coil(4,5,6,80)
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
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

/* LCD SETUP */
LiquidCrystal lcd(22, 24, 26, 28, 30, 32);

/* SERIAL JSON STREAM SETUP */
aJsonStream serial_stream(&Serial);

void setup() {
  Serial.begin(9600);
  Serial.println();  
  lcd.begin(16, 2);
}

void loop() {

}

/*** KEYPAD INTERUPT ***/

/** keyInterupt
 *  If key is '*' resets the state
 *  If key is '#' it checks stock, if in
 *  If in stock another '#' dispense it
 *  Any number is added to an array for
 *  future use
 */
void keyInterupt() {
  int ms = 0;
  char key;
  char str[10];
  int strIndex = 0;
  
  while (ms < 4000) {
    key = kpd.getKey();
    
    if (!key) {
      ms += 10;
      delay(10);
    }
    
    else if (key == '*') {
      return;
    }
    
    else if (key == '#') {
      int id = atoi(str);
      if (!validId(id)) {
        lcdWriteln("Invalid Id");
        break;
      }
      
      else if (coils[id - 1].isEmpty()) {
        lcdWriteln("Out of stock");
        break;
      }
      
      else {
        ms = 0;
        lcdWriteln("Press # to vend");
        while (ms < 4000) {
          ms += 10;
          delay(1);
          key = kpd.getKey();
          if (key && key == '#') {
            coils[id - 1].vend();
            lcdWriteln("Vending product");
            delay(4000);
            lcdWriteln("Thank you");
          }
        }
      }
    }
    
    else {
      str[strIndex++] = key;
      str[strIndex] = 0;
      lcdWriteln(str);
      ms = 0;
    }
   
  }
  delay(4000);
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
    
    aJsonObject *msg = aJson.parse(&serial_stream);
    
    if (!msg) { // Message failed to parse
      while (Serial.available() > 0) {
        Serial.read(); // get rid of serial buffer
      }
      Serial.println('{"err":"Invalid JSON"}');
      return;
    }
    
    char *cmd = aJson.getObjectItem(msg, "cmd")->valuestring;
    int id = aJson.getObjectItem(msg, "id")->valueint;
    
    if (!validId(id)) { // Invalid id
      aJson.addStringToObject(msg, "err", "Id does not exist");
    }
    
    else if (strcmp(cmd, "stock") == 0) { // stock command
    if (coils[id - 1].isEmpty()) {
        aJson.addFalseToObject(msg, "res");
      } else {
      aJson.addTrueToObject(msg, "res");
      }
    }
    
    else if (strcmp(cmd, "vend") == 0) { // vend command
    if (coils[id - 1].vend()) {
        aJson.addTrueToObject(msg, "res");
      } else {
      aJson.addFalseToObject(msg, "res");
      }
    }
    
    else { // command not recognized
      aJson.addStringToObject(msg, "err", "Command not recognized");
    }
    
    aJson.print(msg, &serial_stream);
    Serial.println(); // finish serial write with CRLF
  }
}

/*** UTIL ***/

/** checkId
 *  returns true if id is valid, false otherwise
 */
boolean validId(int id) {
  int arraySize = sizeof(coils) / sizeof(coils[0]);
  return id >= 1 && id <= arraySize;
}

/** lcdRefreshString
 *  this method clears the screen, resets cursor
 *  to beginning and writes a new string to the
 *  LCD
 */
void lcdWriteln(char *s) {
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.write(s);
}
