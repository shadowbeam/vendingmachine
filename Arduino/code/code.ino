#include <aJSON.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <String.h>

/* PIN SETUP */
#define trigPin 48
#define echoPin 50

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
char *defaultMsg = "Enter code";


/* SERIAL JSON STREAM SETUP */
aJsonStream serial_stream(&Serial);

void setup() {
  Serial.begin(9600);
  Serial.println();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);  
  lcd.begin(16, 2);
  resetKeyInput();
}

void loop() {
  char key = kpd.getKey();
  if (key) {
    handleKey(key);
  }
}

/*** RANGE FINDER ***/

/** getDistance
 *  returns the distance in cm from the range finder
 */
long getDistance() {
  long duration;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  digitalWrite(trigPin,HIGH);
  duration = pulseIn(echoPin, HIGH);
  return (duration/2) / 29.1;
}

/** getQuantity
 *  get quantiy of stock for given id
 *  return quanity or -1 if invalid id
 */
int getQuantity(int id) {
  if (!checkId(id)) {
    return -1;
  }
  long distance = getDistance();
  if (distance < 10) {
    return 5 - (distance / 2);
  } else {
    return 0;
  }
}

/*** UTIL ***/

/** checkId
 *  returns true if id is valid, false otherwise
 */
boolean checkId(int id) {
  if (id == 1 || id == 2) {
    return true;
  } else {
    return false;
  }
}

/*** MOTOR ***/

/** vendStock
 *  vends the item
 *  return 1 if sucessfull, 0 if out of stock, -1 if invalid id
 */
int vendStock(int id) {
  int stock = getQuantity(id);
  if (stock < 1) {
    return stock;
  } else {
    // TODO vend item
    return 1;
  }
}

/*** LCD ***/

/** lcdRefreshString
 *  this method clears the screen, resets cursor
 *  to beginning and writes a new string to the
 *  LCD
 */
void lcdRefreshString(char *s) {
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.write(s);
}

/** lcdDisplayMessage
 *  Displays message on LCD for 4 seconds
 */
void lcdDisplayMessage(char *s) {
  lcdRefreshString(s);
  delay(4000);
}

/*** KEYPAD ***/

boolean readyToVend;
char str[10];
int strIndex;

void resetKeyInput() {
  str[0] = 0;
  strIndex = 0;
  readyToVend = false;
  lcdRefreshString(defaultMsg);
}

/** handleKey
 *  If key is '*' resets the state
 *  If key is '#' it checks stock, if in
 *  If in stock another '#' dispense it
 *  Any number is added to an array for
 *  future use
 */
void handleKey(char key) {
  switch (key) {
    case '*': resetKeyInput();
              break;
    
    case '#': {int id = atoi(str);
              if (readyToVend) {
                int result = vendStock(id);
                switch (result) {
                  case -1: lcdDisplayMessage("Invalid ID"); break;
                  case  0: lcdDisplayMessage("Out of stock"); break;
                  default: lcdDisplayMessage("Thank You");
                }
                resetKeyInput();
              }
              
              else {
                int result = getQuantity(id);
                switch (result) {
                  case -1: lcdDisplayMessage("Invalid ID");
                           resetKeyInput();
                           break;
                  case  0: lcdDisplayMessage("Out of Stock");
                           resetKeyInput();
                           break;
                  default: lcdRefreshString("Press # to vend");
                           readyToVend = true;
                }
              }
              break;}
              
    default : str[strIndex++] = key;
              str[strIndex] = 0;
              lcdRefreshString(str);
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
  if (serial_stream.available()) {
    handleJson(); // We have something, let's handle it
  }
}

/** handleJson
 *  parses the incoming JSON from serial port and acts accordingly
 */
void handleJson() {
  aJsonObject *msg = aJson.parse(&serial_stream);
  if (!msg) { // Message failed to parse
    Serial.println('{"err":"Invalid JSON"}');
    return;
  }
  
  aJsonObject *cmd = aJson.getObjectItem(msg, "cmd");
  aJsonObject *id = aJson.getObjectItem(msg, "id");
  
  if (!cmd) { // no command
    aJson.addStringToObject(msg, "err", "No command");
  }
  
  else if (!id) { // no id 
    aJson.addStringToObject(msg, "err", "No id");
  }
  
  else if (strcmp(cmd->valuestring, "stock") == 0) { // stock command
    int result = getQuantity(id->valueint);
    switch (result) {
      case -1: aJson.addStringToObject(msg, "err", "Id does not exist");
               break;
      default: aJson.addNumberToObject(msg, "res", result);
    }
  }
  
  else if (strcmp(cmd->valuestring, "vend") == 0) { // vend command
    int result = getQuantity(id->valueint);
    switch (result) {
      case -1: aJson.addStringToObject(msg, "err", "Id does not exist");
                break;
      case  0: aJson.addStringToObject(msg, "err", "Out of stock");
               break;
      default: aJson.addStringToObject(msg, "res", "Success");
    }
  }
  
  else { // command not recognized
    aJson.addStringToObject(msg, "err", "Command not recognized");
  }
  
  aJson.print(msg, &serial_stream);
  Serial.println(); // finish serial write with CRLF
}
