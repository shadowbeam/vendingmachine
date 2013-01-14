#include <aJSON.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

/*** PIN SETUP ***/
#define trigPin 48
#define echoPin 50
#define ledPin 13

/*** KEYPAD SETUP ***/
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

boolean inStock = false;
String str = "";

/*** SERIAL JSON STREAM SETUP ***/
aJsonStream serial_stream(&Serial);

/*** LCD SETUP ***/
LiquidCrystal lcd(22, 24, 26, 28, 30, 32);

void setup() {
  digitalWrite(ledPin, HIGH);
  Serial.begin(9600);
  Serial.println();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);  
  lcd.begin(16, 2);
}

void loop() {
  char key = kpd.getKey();
  if(key == '*') { // Check for a valid key.
    str = "";
  } else if (key == '#' && inStock) {
    int id = str.toInt();
    int result = vendStock(id);
    Serial.println("Vending");
    
  } else if (key == '#') {
    int id = str.toInt();
    if (checkId(id)) {
      int stock = getStock(id);
      switch (stock) {
        case -1: inStock = false; break;
        case 0: inStock = false; break;
        default: inStock = true; Serial.println("In stock");
      }
    }
  }
  
  else if (key) {
    str += key;
    Serial.println(str);
  }
  
}

/*** SERIAL ***/
void serialEvent() {
    serial_stream.skip(); // Skip any whitespace
  
  if (serial_stream.available()) {
    handleJson(); // We have something, let's handle it
  }
}

/** handleJson
 *  handles the incoming json from serial port
 */
void handleJson() {
  aJsonObject *msg = aJson.parse(&serial_stream);
  if (!msg) {
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
    int result = getStock(id->valueint);
    switch (result) {
      case -1: aJson.addStringToObject(msg, "err", "Id does not exist");
               break;
      default: aJson.addNumberToObject(msg, "res", result);
               lcd.clear();
               lcd.print(result);
               lcd.print(" items left. Press # to vend");
    }
  }
  
  else if (strcmp(cmd->valuestring, "vend") == 0) { // vend command
    int result = getStock(id->valueint);
    switch (result) {
      case -1: aJson.addStringToObject(msg, "err", "Id does not exist");
                lcd.clear();
                lcd.print("Id does not exist");
                
                break;
      case 0: aJson.addStringToObject(msg, "err", "Out of stock");
                lcd.clear();
                lcd.print("Out of stock");
                Serial.println("Out of Stock");
               break;
      default: aJson.addStringToObject(msg, "res", "Success");
                lcd.clear();
                lcd.print("Success");
    }
  }
  
  else { // command not recognized
    aJson.addStringToObject(msg, "err", "Command not recognized");
  }
  
  aJson.print(msg, &serial_stream);
  Serial.println();
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


/*** VENDING***/

boolean checkId(int id) {
  if (id == 1 || id == 2) {
    return true;
  } else {
    return false;
  }
}

/** getStock
 *  get quantiy of stock for given id
 *  return quanity or -1 if invalid id
 */
int getStock(int id) {
  if (!checkId(id)) {
    return -1;
  }
  long distance = getDistance();
  if (distance < 10) {
    return 5 - (distance / 2);
  } else {
    return 0;
  }
  
  /*switch (id) { // TODO implement stock checking
    case 1: return 2;
    case 2: return 0;
    default: return -1; // id not recognized
  }*/
}

/** vendStock
 *  vends the item
 *  return 1 if sucessfull, 0 if out of stock, -1 if invalid id
 */
int vendStock(int id) {
  int stock = getStock(id);
  if (stock < 1) {
    return stock;
  } else {
    // TODO vend item
    return 1;
  }
}

/*** LCD ***/

void lcdRefreshString(char *s) {
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.write(s);
}

void lcdPrintChar() {
  
}
