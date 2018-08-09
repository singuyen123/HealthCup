
#include <EEPROM.h>

struct MyObject {
  int field1;
  byte field2;
  String name;
};

void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  float f = 123.456f;  //Variable to store in EEPROM.
  int eeAddress = 0;   //Location we want the data to be put.


  //One simple call, with the address first and the object second.
  EEPROM.put(eeAddress, f);

  Serial.println("Written float data type!");

  /** Put is designed for use with custom structures also. **/

  //Data to store.
  MyObject customVar = {
    3,
    65,
    "Working!"
  };

  //eeAddress += sizeof(float); //Move address to the next byte after float 'f'.

  EEPROM.put(eeAddress, customVar);
  MyObject data;
  EEPROM.get(eeAddress, data);
  Serial.println(sizeof(MyObject));
  Serial.println(sizeof(String));
  Serial.println(data.field1);
  Serial.println(data.field2);
  Serial.println(data.name);
  Serial.print("Written custom data type! \n\nView the example sketch eeprom_get to see how you can retrieve the values!");
  eeAddress += sizeof(MyObject);
  EEPROM.put(eeAddress, f);
  EEPROM.get(eeAddress, f);
  Serial.print(f);
  for(int i=0; i<20;i++){
    Serial.println(EEPROM.read(i));  
  }

  
}

void loop() {   /* Empty loop */ }
