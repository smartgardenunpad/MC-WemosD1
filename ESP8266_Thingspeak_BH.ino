//Header Sensor
#include <Wire.h> //BH1750 IIC Mode 
#include <BH1750.h>
//Header AT Command & Thingspeak
#define SSID "UnpadWiFi"    // put here the name of your wifi network
#define PASS ""             // put here the password of your wifi network
#define IP "184.106.153.149" // thingspeak.com's IP

//Define Light Sensor
int BH1750address = 0x23; //setting i2c address light sensor
byte buff[2];
BH1750 lightMeter;

String BH = "";//String untuk menampung nilai BH1750

String GET = "GET https://api.thingspeak.com/update?api_key=AH7MJ789VHT51RAR";    // String untuk fungsi GET ke HTTP Thingspeak
String GET1 = "&field1="; //field data sensor
String cmd = "";

//String Cahaya
String light(){
  char lux_str[15];
  //Kalkulasi cahaya
  uint16_t lux = lightMeter.readLightLevel();
  dtostrf(lux, 5, 2, lux_str);
  return (String) lux_str;
  Serial.print("     Light: ");
  Serial.print(lux);
  Serial.println(" lx");
  delay(1000);
  }

  //Fungsi kirim dan update data
void updateFunction(String light){
  cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial2.println(cmd);
  delay(1000);
  if(Serial2.find("Error")){
    Serial.print("Error1");
    return;
  }
  cmd = GET+GET1;
  cmd += light;
  cmd += "\r\n";
  Serial.print(cmd);
  Serial2.print("AT+CIPSEND=");
  Serial2.println(cmd.length());
  if(Serial2.find(">")){
    Serial2.print(cmd);
  }else{
    Serial2.println("AT+CIPCLOSE");
  }
}

// fungsi connect Wifi ke AP
boolean connectWiFi(){
  Serial2.println("AT+CWMODE=1");
  delay(1000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial2.println(cmd);
  delay(1000);
  if(Serial2.find("OK")){
    Serial.println("OK");
    return true;
  }else{
    Serial.println("KO");
    return false;
  }
}

//fungsi sambung wifi dgn AT
void setup()
{
  Wire.begin();
  lightMeter.begin();
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial2.println("AT");
  delay(1000);
  if(Serial2.find("OK")){
    connectWiFi();
  }
}

void loop(){
  BH = light();
  updateFunction(BH);
  delay(1000);
}
