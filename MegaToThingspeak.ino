#define SSID "Unpad Wifi"    // put here the name of your wifi network
#define PASS ""             // put here the password of your wifi network
#define IP "184.106.153.149" // thingspeak.com's IP

String GET = "GET /update?key=AH7MJ789VHT51RAR&";    // put here your thingspeak key instead of [THINGSPEAK_KEY]
String GET1 = "field1=";

String valuetosend = "10";                           // for this first test we send a simple value

void updateFunction(String valuetosend){
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial2.println(cmd);
  delay(2000);
  if(Serial2.find("Error")){
    Serial.print("Error1");
    return;
  }
  cmd = GET + GET1;
  cmd += valuetosend;
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
 
boolean connectWiFi(){
  Serial2.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial2.println(cmd);
  delay(5000);
  if(Serial2.find("OK")){
    Serial.println("OK");
    return true;
  }else{
    Serial.println("KO");
    return false;
  }
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);
//  sensors.begin();
  Serial2.println("AT");
  delay(5000);
  if(Serial2.find("OK")){
    connectWiFi();
  }
}

void loop(){

  updateFunction(valuetosend);
  delay(5000);
}
