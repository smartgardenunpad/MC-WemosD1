#define SensorPin 0 //Header pH 
#include <Wire.h> //Header BH1750
#include <OneWire.h>
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

//define EC
#define StartConvert 0
 
const byte numReadings = 20;     //the number of sample times
byte ECsensorPin = A1;  //EC Meter analog output,pin on analog 1
byte DS18B20_Pin = 2; //DS18B20 signal, pin on digital 2
unsigned int AnalogSampleInterval=25,printInterval=700,tempSampleInterval=850;  //analog sample interval;serial print interval;temperature sample interval
unsigned int readings[numReadings];      // the readings from the analog input
byte index = 0;                  // the index of the current reading
unsigned long AnalogValueTotal = 0;                  // the running total
unsigned int AnalogAverage = 0,averageVoltage=0;                // the average
unsigned long AnalogSampleTime,printTime,tempSampleTime;
float temperature,ECcurrent; 
  
//Temperature chip i/o
OneWire ds(DS18B20_Pin);  // on digital pin 2

//Air Temperature
#define TAsensorPin A2

String BH = ""; //String untuk menampung nilai BH1750
String pH = "";//String untuk menampung nilai pH
String EC = ""; // String untuk menampung nilai EC
String TW = ""; // String untuk menampung nilai water temperature
String TA = ""; // String untuk menampung nilai Air temperature
String GET = "GET https://api.thingspeak.com/update?api_key=AH7MJ789VHT51RAR";    // String untuk fungsi GET ke HTTP Thingspeak
String GET1 = "&field1=";//field BH1750
String GET2 = "&field2=";//field pH
String GET3 = "&field3=";//field EC
String GET4 = "&field4=";//field water temp
String GET5 = "&field5=";//field air temp
String cmd = "";

//String Cahaya
  String light(){
  char lux_str[15];
  
//Kalkulasi cahaya
  uint16_t lux = lightMeter.readLightLevel();
  dtostrf(lux, 4, 2, lux_str);
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
  for(int i=0;i<9;i++){        //sort the analog from small to large
    for(int j=i+1;j<10;j++){
      if(buf[i]>buf[j]){
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;}
      }
    }
  
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue;                      //convert the millivolt into pH value
  dtostrf(phValue, 4, 2, phValue_str);
  return (String) phValue_str;
  Serial.print("    pH:");  
  Serial.print(phValue,2);
  Serial.println(" ");
  digitalWrite(13, HIGH);       
  delay(800);
  digitalWrite(13, LOW); 
  delay(1000);
  }

//String EC
  String ECVal(){
  char ECValue_str[15];

  if(millis()-AnalogSampleTime>=AnalogSampleInterval){ 
    AnalogSampleTime=millis();
     // subtract the last reading:
    AnalogValueTotal = AnalogValueTotal - readings[index];
    // read from the sensor:
    readings[index] = analogRead(ECsensorPin);
    // add the reading to the total:
    AnalogValueTotal = AnalogValueTotal + readings[index];
    // advance to the next position in the array:
    index = index + 1;
    // if we're at the end of the array...
    if (index >= numReadings)
    // ...wrap around to the beginning:
    index = 0;
    // calculate the average:
    AnalogAverage = AnalogValueTotal / numReadings;
  }
  if(millis()-printTime>=printInterval){
    printTime=millis();
    float ECValue=AnalogAverage*(float)5000/1024;
    dtostrf(ECValue, 4, 2, ECValue_str);
    return (String) ECValue_str;
    Serial.print("^C     EC:");
  }
  float TempCoefficient=1.0+0.0185*(temperature-25.0);
  float CoefficientVolatge=(float)averageVoltage/TempCoefficient;   
    if(CoefficientVolatge<150)Serial.println("No solution!");   //25^C 1413us/cm<-->about 216mv  if the voltage(compensate)<150,that is <1ms/cm,out of the range
    else if(CoefficientVolatge>3300)Serial.println("Out of the range!");  //>20ms/cm,out of the range
    else{
      if(CoefficientVolatge<=448)ECcurrent=6.84*CoefficientVolatge-64.32;   //1ms/cm<EC<=3ms/cm
      else if(CoefficientVolatge<=1457)ECcurrent=6.98*CoefficientVolatge-127;  //3ms/cm<EC<=10ms/cm
      else ECcurrent=5.3*CoefficientVolatge+2278;                           //10ms/cm<EC<20ms/cm
      ECcurrent/=1000;    //convert us/cm to ms/cm
      Serial.print(ECcurrent,2);  //two decimal
      Serial.println("ms/cm");
    }
  }
  
String TWVal(){
  char TWValue_str[15];

  float TWValue = getTemp();
  dtostrf(TWValue, 4, 2, TWValue_str);
  return (String) TWValue_str;
  Serial.print(TWValue,2);
  delay(100); //just here to slow down the output so it is easier to read
}


float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
  }

  String TAVal(){
  char TAValue_str[15];

  float TAValue = analogRead(TAsensorPin);
  TAValue = TAValue*5*100.00/1024;
  dtostrf(TAValue, 4, 2, TAValue_str);
  return (String) TAValue_str;
  Serial.print (TAValue,2);
  delay(1000);
  }
  
//Fungsi kirim dan update data
void updateFunction(String light, String pHVal, String ECVal, String TWVal, String TAVal){
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
  cmd += GET2;
  cmd += pHVal;
  cmd += GET3;
  cmd += ECVal;
  cmd += GET4;
  cmd += TWVal;
  cmd += GET5;
  cmd += TAVal;
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
  EC = ECVal();
  TW = TWVal();
  TA = TAVal();
  updateFunction(BH, pH, EC, TW, TA);
  delay(1000);
}
