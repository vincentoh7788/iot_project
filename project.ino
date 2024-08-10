#define RELAY1_PIN 27
#define TEMP_PIN 4
#define TURBIDITY_PIN 34
#define SERVO_PIN 25
#define RELAY2_PIN 26
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include <ESP32Servo.h>
#include "secrets.h"
int turbidity;
  OneWire oneWire(TEMP_PIN);
  DallasTemperature sensors(&oneWire);
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
  float tempC;
Servo myServo;
const long  gmtOffset_sec = 28800;
const int   daylightOffset_sec = 0;

const char* ntpServer = "pool.ntp.org";
void setup()
{
  
 Serial.begin(115200);
 myServo.attach(SERVO_PIN);
 
//connect to WiFi

Serial.println("Connecting to Wi-Fi");
WiFi.mode(WIFI_STA);
WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
   
}
Serial.println(" CONNECTED");

net.setCACert(AWS_CERT_CA);
net.setCertificate(AWS_CERT_CRT);
net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
 
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
  Serial.println("AWS IoT Connected!");
configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(TURBIDITY_PIN,INPUT);
  digitalWrite(RELAY1_PIN,LOW);
  digitalWrite(RELAY2_PIN,LOW);

  sensors.begin();
}

void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["turbidity"] = turbidity;
  doc["temperature"] = tempC;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}
void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];

    Serial.println(message);
}
void loop()
{
  turbidity = analogRead(TURBIDITY_PIN);
  
  
  Serial.println(turbidity);
  
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
  if(tempC != DEVICE_DISCONNECTED_C) 
  {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(tempC);

  } 
  else
  {
    Serial.println("Error: Could not read temperature data");
  }
  
    int val;
    //test servo
  while (Serial.available()>0){
    val = Serial.parseInt();
    if(val!=0){
      Serial.println(val);
      myServo.write(val);
    }
  }

//Threshold around 3100(At night and without temp sensor and servo)
   if(turbidity<=3200){
    digitalWrite(RELAY1_PIN,HIGH);
    
  }
  else{
     digitalWrite(RELAY1_PIN,LOW);
     
  }
  
  //
  if(tempC<30){
    digitalWrite(RELAY2_PIN,HIGH);
    
  }
  else{
     digitalWrite(RELAY2_PIN,LOW);
     
  }
   feedFish();
   publishMessage();
  client.loop();
  delay(1000);
  
}
void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
void feedFish() {
  struct tm timeinfo;
  char hr[3];
  char min[3];
  char sec[3];
  int hrInt;
  int minInt;
  int secInt;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  else{
  strftime(hr,3, "%H", &timeinfo);
  Serial.print(hr);
   strftime(min,3, "%M", &timeinfo);
  Serial.print(min);
  strftime(sec,3, "%S", &timeinfo);
  Serial.println();
  hrInt = atoi(hr);
  minInt = atoi(min);
  secInt = atoi(sec);
  
    // Feeding time: 7:00 AM and 7:00 PM
    if ((hrInt == 7 && minInt == 0 && secInt == 0) || (hrInt == 19 && minInt ==00 && secInt==00)) {
        myServo.write(90); // Move the servo to 90 degrees (feeding position)
         Serial.println("servo open");
        delay(1000); 
        myServo.write(-90);  

    }
    
}
}

