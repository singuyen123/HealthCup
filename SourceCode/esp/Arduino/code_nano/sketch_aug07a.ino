
#include <Wire.h>
//#include <i2cmaster.h>
#include <RTClib.h> 
#include "HX711.h"
#include <Adafruit_MLX90614.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

struct wifiObject{
  String userName;
  String pass;
  };

wifiObject wifi[10];
SoftwareSerial BTSerial(2, 3);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

HX711 scale(5, 6);
float calibration_factor = 2230; // this calibration factor is adjusted according to my load cell
float units;
float ounces;

RTC_DS1307 RTC;
const byte NumberOfFields = 7;
const byte DS1307 = 0x68;
int second, minute, hour, day, wday, month, year;
void setup(){
    Serial.begin(9600);
    Serial.println("Setup...");
    mlx.begin();
    //i2c_init(); //Initialise the i2c bus
    //PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pullups
      Wire.begin();
      BTSerial.begin(9600);
  /* cài đặt thời gian cho module */
  //setTime(3, 48, 58, 4, 8, 8, 18); // 12:30:45 CN 08-02-2015
  setup_loadcell();
}

void setup_loadcell(){
  scale.set_scale();
  scale.tare();  //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
}

void loadcell(){
   scale.set_scale(calibration_factor); //Adjust to this calibration factor
  units = scale.get_units(), 10;
  if (units < 0)
  {
    units = 0.00;
  }
  ounces = units * 0.035274;
  Serial.println(units);
  delay(500);
}

void loop(){
  Serial.print(mlx.readAmbientTempC()); 
  Serial.print("*C\t");
  Serial.println(mlx.readObjectTempC()); 
  Serial.print(mlx.readAmbientTempF()); 
  Serial.print("*F\t"); 
  Serial.println(mlx.readObjectTempF()); 
  
  readDS1307();
  /* Hiển thị thời gian ra Serial monitor */
  digitalClockDisplay();

  loadcell();
  while (BTSerial.available()){
    for(byte i=0; i<10; i++){
      wifi[i].userName="";
      wifi[i].pass="";  
    }
    char temp1 = BTSerial.read();
    for(byte i=0;i<10;i++){
       if(temp1 == '/'){
        for(byte j=0;j<20;j++){
          if(BTSerial.available()){
            char temp = BTSerial.read();
            if(temp == ',') {temp1=',';break;}
            wifi[i].userName += temp;
          }
        }
       }
       if(temp1==','){
        for(byte j=0;j<20;j++){
          if(BTSerial.available()){
            char temp = BTSerial.read();
            if(temp == '/') {temp1='/';break;}
            if(temp == '.') {temp1='.';break;}
            wifi[i].pass += temp;
          }
        }
       }
       if(temp1 == '.'){
        for(byte i=0; i<2;i++){
          Serial.print("username:");
          Serial.println(wifi[i].userName);
          Serial.print("pass:");
          Serial.println(wifi[i].pass);
         }
         break;
       }
        
    }
    //while(!BTSerial.available()){}
    
  }
  
  while (Serial.available())
       BTSerial.write(Serial.read());
  delay(1000); // wait a second before printing again
}

void readDS1307()
{
        Wire.beginTransmission(0x68);
        Wire.write((byte)0x00);
        Wire.endTransmission();
        Wire.requestFrom(DS1307, NumberOfFields);
        
        second = bcd2dec(Wire.read() & 0x7f);
        minute = bcd2dec(Wire.read() );
        hour   = bcd2dec(Wire.read() & 0x3f); // chế độ 24h.
        wday   = bcd2dec(Wire.read() );
        day    = bcd2dec(Wire.read() );
        month  = bcd2dec(Wire.read() );
        year   = bcd2dec(Wire.read() );
        year += 2000;    
}
/* Chuyển từ format BCD (Binary-Coded Decimal) sang Decimal */
int bcd2dec(byte num)
{
        return ((num/16 * 10) + (num % 16));
}
/* Chuyển từ Decimal sang BCD */
int dec2bcd(byte num)
{
        return ((num/10 * 16) + (num % 10));
}
 
void digitalClockDisplay(){
    // digital clock display of the time
    Serial.print(hour);
    printDigits(minute);
    printDigits(second);
    Serial.print(" ");
    Serial.print(day);
    Serial.print(" ");
    Serial.print(month);
    Serial.print(" ");
    Serial.print(year); 
    Serial.println(); 
}
 
void printDigits(int digits){
    // các thành phần thời gian được ngăn chách bằng dấu :
    Serial.print(":");
        
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}
 
/* cài đặt thời gian cho DS1307 */
void setTime(byte hr, byte min, byte sec, byte wd, byte d, byte mth, byte yr)
{
        Wire.beginTransmission(0x68);
        Wire.write(byte(0x00)); // đặt lại pointer
        Wire.write(dec2bcd(sec));
        Wire.write(dec2bcd(min));
        Wire.write(dec2bcd(hr));
        Wire.write(dec2bcd(wd)); // day of week: Sunday = 1, Saturday = 7
        Wire.write(dec2bcd(d)); 
        Wire.write(dec2bcd(mth));
        Wire.write(dec2bcd(yr));
        Wire.endTransmission();
}


