//Header Sensor
#define SensorPin 0          //pH meter Analog output to Arduino Analog Input 0

//Header AT Command & Thingspeak
#define SSID "UnpadWiFi"    // put here the name of your wifi network
#define PASS ""             // put here the password of your wifi network
#define IP "184.106.153.149" // thingspeak.com's IP

//Define pHVal Sensor
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;

String pH = "";//String untuk menampung nilai pH1750

String GET = "GET https://api.thingspeak.com/update?api_key=AH7MJ789VHT51RAR";    // String untuk fungsi GET ke HTTP Thingspeak
String GET1 = "&field1="; //field data sensor
String cmd = "";

//String pH
String pHVal(){
  char phValue_str[15];
  //Kalkulasi pH
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
  //Serial.print("     pHVal: ");
  //Serial.print(phValue);
  //Serial.println(" ph");
  delay(1000);
  }

  //Fungsi kirim dan update data
void updateFunction(String pHVal){
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
  pinMode(13,OUTPUT);  
  Serial.println("Ready");
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
  updateFunction(pH);
  delay(1000);
}
