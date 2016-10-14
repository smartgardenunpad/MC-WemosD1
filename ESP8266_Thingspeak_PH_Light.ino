#define SensorPin 0 //Header pH 
#include <Wire.h> //Header BH1750
#include <BH1750.h>

//Header AT Command & Thingspeak
#define SSID "UnpadWiFi"    // put here the name of your wifi network
#define PASS ""             // put here the password of your wifi network
#define IP "184.106.153.149" // thingspeak.com's IP

//define pH
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;

//Define Light Sensor
int BH1750address = 0x23; //setting i2c address light sensor
byte buff[2];
BH1750 lightMeter;

String BH = ""; //String untuk menampung nilai BH1750
String pH = "";//String untuk menampung nilai pH
String GET = "GET https://api.thingspeak.com/update?api_key=AH7MJ789VHT51RAR";    // String untuk fungsi GET ke HTTP Thingspeak
String GET1 = "&field1=";//field BH1750
String GET2 = "&field2=";//field pH
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

  String pHVal(){
  char phValue_str[15];
  
  for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue;                      //convert the millivolt into pH value
  dtostrf(phValue, 5, 2, phValue_str);
  return (String) phValue_str;
  Serial.print("    pH:");  
  Serial.print(phValue,2);
  Serial.println(" ");
  digitalWrite(13, HIGH);       
  delay(800);
  digitalWrite(13, LOW); 
  delay(1000);
  }

//Fungsi kirim dan update data
void updateFunction(String light, String pHVal){
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
  cmd = GET2;
  cmd += pHVal;
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
  //Setup BH1750
  Wire.begin(); 
  lightMeter.begin();
  
  //Setup pin pH on Analog 13
  pinMode(13,OUTPUT);
 
  //Connect ESP&ArdMega
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial2.println("AT");
  delay(1000);
  if(Serial2.find("OK")){
    connectWiFi();
  }
}

void loop(){
  pH = pHVal();
  BH = light();
  updateFunction(BH, pH);
  delay(1000);
}
