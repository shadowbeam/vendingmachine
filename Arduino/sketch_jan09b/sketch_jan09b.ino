#include <aJSON.h>
#include <LiquidCrystal.h>


#define trigPin 48
#define echoPin 50

aJsonStream serial_stream(&Serial);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  Serial.begin(9600);
  Serial.println();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("hello, world!");
}




void loop() {
  long distance = getDistance();
  lcd.setCursor(0, 1);
  if (distance >= 200 || distance <= 0){
    lcd.clear();
    lcd.print("Out of range");
  }
  else {
    lcd.clear();
    lcd.print(distance);
    lcd.print(" cm");
  }
  
  if (serial_stream.available()) {
    serial_stream.skip(); // Skip any whitespace
  }
  
  if (serial_stream.available()) {
    handleMsg(); // We have something, let's handle it
  }
  
  delay(500);
}

void SerialEvent() {
  serial_stream.skip(); // Skip any whitespace
  if (serial_stream.available()) {
    handleMsg(); // We have something, let's handle it
  }
}

void handleMsg() {
  aJsonObject *msg = aJson.parse(&serial_stream);
  if (!msg) {
    Serial.println("{\"err\":\"Invalid JSON\"}");
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
  
  /*else if (cmd->type != aJson_String
      && id->type != aJson_Int) { // Invalid params
    aJson.addStringToObject(msg, "err", "Invalid parameters");
  }*/
  
  else if (strcmp(cmd->valuestring, "stock") == 0) {
    int result = getStock(id->valueint);
    switch (result) {
      case -1: aJson.addStringToObject(msg, "err", "Id does not exist");
               break;
      default: aJson.addNumberToObject(msg, "res", result);
    }
  }
  
  else if (strcmp(cmd->valuestring, "vend") == 0) {
    int result = getStock(id->valueint);
    switch (result) {
      case -1: aJson.addStringToObject(msg, "err", "Id does not exist");
               break;
      case 0: aJson.addStringToObject(msg, "err", "Out of stock");
               break;
      default: aJson.addStringToObject(msg, "res", "Success");
    }
  }
  
  else {
    aJson.addStringToObject(msg, "err", "Command not recognized");
  }
  
  aJson.print(msg, &serial_stream);
  Serial.println();
}

int getStock(int id) {
  switch (id) { // TODO implement stock checking
    case 1: return 2;
    case 2: return 0;
    default: return -1; // id not recognized
  }
}

int vendStock(int id) {
  int stock = getStock(id);
  if (stock < 1) {
    return stock;
  } else {
    // TODO vend item
    return 1;
  }
}

long geistance() {
  long duration;
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  digitalWrite(trigPin,HIGH);
  duration = pulseIn(echoPin, HIGH);
  return (duration/2) / 29.1;
}

void clearline(){
  lcd.setCursor(0,1);
  for (int i = 0; i < 16; i++) {
    lcd.print(" ");
  } 
}
