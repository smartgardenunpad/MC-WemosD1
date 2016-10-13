// Baudrate Test for ESP8266 to Arduino Mega

void setup() {
  // put your setup code here, to run once:
  Serial2.begin(115200); //Rx Tx 2 (17,16)
  Serial.begin(115200); //Rx Tx 0 (0,1) should the same baudrate
}

void loop() {
  if (Serial.available() > 0) {
    byte b = Serial.read();
    Serial2.write(b);

  }

  if (Serial2.available() > 0) {
    byte b = Serial2.read();
    Serial.write(b);
  }
}
