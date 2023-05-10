/*
Bharat Intern Task 2: Air pollution monitoring system

Componets Used in this system:
1) Arduino Uno
2) ESp8266
3) MQ135 Sensor
4) LED's

*/


//Library for serial communication between Arduino Uno to ESP8266
#include <SoftwareSerial.h>

// Thresholds for each gas
#define CO_THRESHOLD 50
#define NO2_THRESHOLD 30
#define SO2_THRESHOLD 20

//Defining Pins
#define RX 2
#define TX 3
#define RED_LED_PIN 4
#define YELLOW_LED_PIN 5
#define GREEN_LED_PIN 6
#define MQ135_PIN A0

SoftwareSerial esp8266(RX, TX);       //Connect Rx and Tx pins of ESP8266 to 2 & 3

String ssid = "Redmi 6 Pro";          // ssid NAME
String pass = "12349876";             // ssid PASSWORD
String API = "6A29YXM9Q8IWZ4TS";      // Thingspeak Api key(Write)
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand;
boolean found = false;
const int sensorPin= 0;
int air_quality;

void setup() {
  Serial.begin(9600);
  esp8266.begin(115200); 
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  sendCommand("AT", 5, "OK");                               //Check communication is estblished or not
  sendCommand("AT+CWMODE=1", 5, "OK");                      //Set the Wi-Fi mode
  sendCommand("AT+CWJAP=\"" + ssid + "\",\"" + pass + "\"", 20, "OK"); //Connect Wifi to that perticular SSID
}

void loop()
{
  int gasValue = analogRead(MQ135_PIN);      //Read value from sensor
  float gasPPM = gasValue / 10.24;            //Calibrate the sensor
  if (gasPPM >= CO_THRESHOLD) {
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
  } else if (gasPPM >= NO2_THRESHOLD) {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
  } else if (gasPPM >= SO2_THRESHOLD) {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
  } else {
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
  }

   //Send data to the Thingspeak
  String getData = "GET /update?api_key=" + API + "&field1=" + gasPPM;
  sendCommand("AT+CIPMUX=1", 5, "OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT, 15, "OK");
  sendCommand("AT+CIPSEND=0," + String(getData.length() + 4), 4, ">");
  esp8266.println(getData); delay(1500); countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0", 5, "OK");
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while (countTimeCommand < (maxTime * 1))
  {
    esp8266.println(command);
    if (esp8266.find(readReplay))
    {
      found = true;
      break;
    }
    countTimeCommand++;
  }
  if (found == true)          //If its true then we got correct reply
  {
    Serial.println("Done");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  if (found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  found = false;
}
