#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <AWS_IOT.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <NTPClient.h>
#include <time.h>

const char* ssid = "Nakasha";
const char* password = "Baghdad68";

const char* awsEndpoint = "aysximiwi48yt.iot.ap-southeast-2.amazonaws.com";
const char* awsRegion = "ap-southeast-2";
const char* awsAccessKey = "AKIA2ITYZKRII5DOCYBF";
const char* awsSecretKey = "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";
const char* awsTopic = "IoT_Rule";

const char* awsCertCA = "-----BEGIN CERTIFICATE-----\n
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5\n-----END CERTIFICATE-----\n";

const char*  awsCertPrivateKey =  "-----BEGIN PRIVATE KEY-----\
nMIIEpQIBAAKCAQEA2Pm4WGBH5NAXiv9hY1EkhS2mVC0Gfd620Q9cELpp2CHZaqRZ
MyCSQPlD7pgehYKKpJ4m4WoIq/Kdd1kL7q8dTNi2i04zD1i0uZi2azvr/OxgnDo4
URgQqun8ecz5yYR5BzwsFfCYNK7tqQ9/4WDIWNuDdCy2bjbsg1zKLggY5qmMf3E7
6PSf4p7cNWTspa8S6UicZIyHX+pmS3f0LoXip2UoMqxrAkv+yZPupA05TXDxoaY+
eFEfbHUWbabMRUIOaIxkqsg49v8/zIJyAURPytte12fezEPWRuDcX01lVyZF30j7
64u+JzoM83UpJNk5BZz1qzsv4J7IIrJiA/IrdQIDAQABAoIBAQCg2hhQ5CfIOICL
miHPqBMD7EC3fJ2oaJhtSg69lATtpsJwraA3J/lk06YJ1poh7hHsUhaO27XMC1Oz
a79N1mJJWwuRIIr34JIyWUU7IDQlz/MbPooLjGSm4Eo83bl8LIow0+n25eNggyJL
R+NM6Rv9y9abJJ6l/G/APuTf1T7omACpUzqyDLAIIItypjsnrBdRegmOMJ25I1D8
BvYs1b6XyFoPZs2whjMLoHGjtjRpoYt0QiWZ5H2s/ev5sxzQkjWSTdAjzgcg/eZK
jUNA4IIY9cYLj/44FMzIXksgIUe3j9DXBdIqH9DY+Z6yvlKmBOs+H1s3ysL+AL9w
4+l3MTUhAoGBAP+X/k+qE142jEXgm2ojx6+VWbC0B4npcPXdAqOMDvjuiW+imsCD
R9yfnvFgH4kXs6rsU5YBXEO4dXSRJQ6sVguTxHJRoklV25/VU+64AsLIfGgRP2en
LZdRnZzGLQT3+Q4JS8Q7n0D0xJ4mRdxaNbU4IF3SP4k+vlUUjnbzqz+5AoGBANlS
AxiloYP+2O31+fMmrwItnUk8eLlbAZpgiZxM2jJZTEJyQHXl6tQCDShIFizw+OwR
604aubLoNlzdQSo2GCDs+DLrAcj1lPTwLm9QwrJtlRPnvOnSTxm0W3klLBMfQms+
A4jCE3RNzujOU/tCvsf5atxccpN0OKLj4qniUk+dAoGAEJAx+8miva09Q939VFCD
RT6hdFEGWAqXtmcfH6gY5ErY/zIDY0xeoM57R1saUBEIJKBx/juSVhUjD23EgAHt
BY0K4vmlUalNtTrm2IZ2L1c8sKXFfRsZWkAAvcMd8b98FUvG7C2M8XGrODB0wIXc
CyVa+5l/Yo59WRge71h1FYkCgYEAqlv3H2ggAGhUR96OjUTi5nM5nIKyJKOp0wog
dg1s8xE7jc8gpwuQlZfyFoTEFYDlsbGuYzDiJ9SPYSb1AZEcQxtgyhGd+ZpCpexu
ikamCdeLiOoeFrIN3hmbp5EzRt43Y/heLuPs+jxQmgG8pfhBtGPamFpEW0E9cLvy
ILjRS6UCgYEArruPYBotgBBVgBkysUygTX9UhdHfPLCh0hdHIsXxlrp5GivJhf4q
KoGG7EDi1U1aleNxM61A5yRTtxbwanTGvjnQG0tVGm87RJl0R/TB1FEO5H9ddAb8
J7SEsNJQmf9TTK9bxv0lFqbO7Ll6L4RT6wlEwiEU/5YkbtC8Ku1lNXY=
\n-----END PRIVATE KEY-----\n";

const char* awsCertPEM = "-----BEGIN CERTIFICATE-----\n
MIIDWjCCAkKgAwIBAgIVAKWDBub05Y+FBub1X4dOyXpK2vaZMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMzA1MDYwNTUz
MjZaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDY+bhYYEfk0BeK/2Fj
USSFLaZULQZ93rbRD1wQumnYIdlqpFkzIJJA+UPumB6Fgoqknibhagir8p13WQvu
rx1M2LaLTjMPWLS5mLZrO+v87GCcOjhRGBCq6fx5zPnJhHkHPCwV8Jg0ru2pD3/h
YMhY24N0LLZuNuyDXMouCBjmqYx/cTvo9J/intw1ZOylrxLpSJxkjIdf6mZLd/Qu
heKnZSgyrGsCS/7Jk+6kDTlNcPGhpj54UR9sdRZtpsxFQg5ojGSqyDj2/z/MgnIB
RE/K217XZ97MQ9ZG4NxfTWVXJkXfSPvri74nOgzzdSkk2TkFnPWrOy/gnsgismID
8it1AgMBAAGjYDBeMB8GA1UdIwQYMBaAFAsg20JG8LjB+0tgucem9YRZTt0JMB0G
A1UdDgQWBBRnUFxWJ6tSexoClPhSrfUxLNt8pzAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAmoQsXMhWmxh/FxOaEwviyCGT
fbkix1N6OFcWRTSKrMaVMhRITSDrLDKuhNcd0Sj4tS3vkOj8qyiORq3AF5SUyKem
yd+F1UZkL5+F3ZkOIMQWmfVp6x5MaPsdbb3UyaVRzTLF8kv7jgHvD1yemn7bZWjn
18tfAT28vLL25+qeZEZw/aFoZr/dvqZtuOUId9yyiIqbgtrOkD1erbGfe4K13Bl8
kp5X5EhPhxMFMdTCHxiGYqQnTv1uyH8IMFX/OFf9tuAinzf7oMoTAPctf4vL5tjk
qcIGwzwbdVXd+wcYgqPPeHQhMKQxgobh9HA7QaN/H9hTJ7U2XWrHRGOK/hFK1Q==
\n-----END CERTIFICATE-----\n";


int raindropSensor = 11; // Pin 11 is the pin for the raindrop sensor
int humitureSensor = 12; // Pin 12 is the pin for the humiture sensor
int infraredSensor = 13; // Pin 13 is the pin for the infrared sensor
int In1 = 8; // Pin 8 is the pin for the Motor Sensor
int In2 = 9; // Pin 9 is the pin for the Motor Sensor
int enablePin = 10; // Pin 10 is the pin for the Motor Sensor

//int infraredValue; // 0 is not closed, 1 is closed.
DHT dht = DHT(humitureSensor, DHT11);

unsigned long startWaitRainTime = 0;
unsigned long startWaitHumidityTime = 0;
unsigned long startWaitTemperatureTime = 0;
unsigned long startWaitWindowChangeTime = 0;
unsigned long currentTime = millis();

bool manuallyChanged = false;
bool currentWindowClosedStatus = false; // Is the window currently closed.
bool supposedWindowClosedStatus = false; // Is the window supposed to be closed.
bool wasRaining = false;
bool hasSentHumidityMessage = false;
bool hasSentTemperatureMessage = false;

// Closes the window. Returns true if successful. Else returns false.
bool closeWindow() {
  // ADD CODE HERE

  digitalWrite(enablePin, HIGH);
  analogWrite(In1, HIGH);
  analogWrite(In2, LOW);


  for (int i=128;i<150;i++)
  {   digitalWrite(In1, HIGH);
      digitalWrite(In2, LOW);
      analogWrite(enablePin, i);
      delay(200); //cheat way to increase the distance     
      }

  analogWrite(In1, 0);
  analogWrite(In2, 0);
  digitalWrite(enablePin, LOW);

  Serial.print(" Window is closed!!!");
  

  currentWindowClosedStatus = true;
  supposedWindowClosedStatus = true;
}


// Opens the window. Returns true if successful. Else returns false.
bool openWindow() {

  analogWrite(In1, LOW);
  analogWrite(In2, HIGH);
  digitalWrite(enablePin, HIGH); 

  for (int i=128;i<150;i++)
  {   digitalWrite(In1, LOW);
      digitalWrite(In2, HIGH);
      analogWrite(enablePin, i);
      delay(200); //cheat way to increase the distance     
      }

  analogWrite(In1, 0);
  analogWrite(In2, 0);
  digitalWrite(enablePin, LOW);

  Serial.print(" Window is opened!!!");

  currentWindowClosedStatus = false;
  supposedWindowClosedStatus = false;
}


// Sends the argument given as a message to the user's phone. Returns true if successful. Else returns false.
bool sendNotification(String message) {
  WiFiClientSecure snsClient;
  snsClient.setCACert(awsCertCA);
  snsClient.setCertificate(awsCertCRT);
  snsClient.setPrivateKey(awsCertPrivateKey);
  
  HTTPClient http;
  String url = "https://sns." + String(awsRegion) + ".amazonaws.com";
  http.begin(snsClient, url);
  http.addHeader("Content-Type", "application/json");
  String body = "{\"TopicArn\":\"arn:aws:sns:ap-southeast-2:705701696592:Push-Notifications-IOT\",\"Message\":\"" + message + "\"}";
  int httpCode = http.POST(body);
  
  Serial.println("HTTP response code: " + String(httpCode));
  
  http.end();
  
  // return true if HTTP response code is between 200 and 299
  if (httpCode >= 200 && httpCode < 300) {
    return true;
  } else {
    return false;
  }
}

// Saves the sensor data to an external storage location
void saveSensorData(int rainData, float temperatureData, float humidityData) {
  // ADD CODE HERE
}

// Saves the time and date to an external storage location. Is called when the state of the window is manually changed.
void saveManualWindowChangeData(bool currentWindowIsClosedStatus) {
  // ADD CODE HERE
}

void setup() {
  // put your setup code here, to run once:
  pinMode(raindropSensor, INPUT);
  Serial.begin(9600);
  dht.begin();

   //motor setup
  pinMode(In1, OUTPUT); //IN1
  pinMode(In2, OUTPUT); //IN2
  pinMode(enablePin, OUTPUT); //Enable Pin
}

void loop() {
  // put your main code here, to run repeatedly:
  currentTime = millis();
  bool wasRaining = false;
  bool hasSentHumidityMessage = false;
  bool hasSentTemperatureMessage = false;
  float temperatureValue = dht.readTemperature(); // Temperature of area in celcius.
  float humidityValue = dht.readHumidity(); // Humidity of area in percentage.
  int infraredValue = digitalRead(infraredSensor); // Detects if the window is closed. 0 is not closed, 1 is closed.
  int raindropValue = digitalRead(raindropSensor); // Detects if it is raining. 0 is not raining, 1 is raining.

  // Humiture Sensor Text Output
  Serial.print("Temperature: ");
  Serial.print(temperatureValue);
  Serial.print(", Humidity: ");
  Serial.print(humidityValue);

  // IR Sensor Text Output
  if (infraredValue == 0) {
    Serial.print(", Window Closed: No, ");
  } else {
    Serial.print(", Window Closed: Yes, ");
  }

  // Raindrop sensor Text Output
  if (raindropValue == 0) {
    Serial.print("Raining: No.");
  } else {
    Serial.print("Raining: Yes.");
  }

  // If the window was manually changed within the last 30 minutes, don't do normal actions.
  if (!manuallyChanged && (startWaitWindowChangeTime != 0 || startWaitWindowChangeTime < currentTime-(30*60*1000))) {
    // Regular Rain Logic
    if (raindropValue != 0) { // Is raining
      wasRaining = true;
      startWaitRainTime = 0;
      closeWindow();
      sendMessage("Raining. Closing Window.");
    } else if (raindropValue == 0 && wasRaining && startWaitRainTime == 0) { // Rain has stopped
      startWaitRainTime = currentTime;
    } else if (raindropValue == 0 && wasRaining && startWaitRainTime < currentTime-(5*60*1000)) { // Has not rained for 5 minutes
      wasRaining = false;
      startWaitRainTime = 0;
      openWindow();
      sendMessage("Not raining. Opening Window.");
    }
    
    // Regular Humiture Logic
    if (!hasSentHumidityMessage) {
      if (humidityValue > 90) { // If the humidity is greater than 90% send a message.
        sendMessage("Current Humidity greater than 90%! Please turn on your air conditioning.");
        startWaitHumidityTime = currentTime;
        hasSentHumidityMessage = true;
      } else if (humidityValue < 10) { // If the humidity is less than 10% send a message.
        sendMessage("Current Humidity less than 10%! Please turn on your air conditioning.");
        startWaitHumidityTime = currentTime;
        hasSentHumidityMessage = true;
      }
    } else if (startWaitHumidityTime < currentTime-(5*60*1000)) { // If a message has been sent, and it's been 5 minutes since the last check.
      if (humidityValue < 85 && humidityValue > 15) { // If the humidity is normal, reset values.
        hasSentHumidityMessage = false;
        startWaitHumidityTime = 0;
      } else { // If humidity is not normal, check again in 5 minutes.
        startWaitHumidityTime = currentTime;
      }
    }

    // Regular Temperature Logic
    if (!hasSentTemperatureMessage) {
      if (temperatureValue > 35) { // If the temperature is greater than 90% send a message.
        sendMessage("Current Temperature greater than 30 Degrees Celcius! Please turn on your air conditioning.");
        startWaitTemperatureTime = currentTime;
        hasSentTemperatureMessage = true;
      } else if (temperatureValue < 5) { // If the temperature is less than 10% send a message.
        sendMessage("Current Temperature less than 5 Degrees Celcius! Please turn on your air conditioning.");
        startWaitTemperatureTime = currentTime;
        hasSentTemperatureMessage = true;
      }
    } else if (startWaitTemperatureTime < currentTime-(5*60*1000)) { // If a message has been sent, and it's been 5 minutes since the last check.
      if (temperatureValue < 30 && temperatureValue > 10) { // If the temperature is normal, reset values.
        hasSentTemperatureMessage = false;
        startWaitTemperatureTime = 0;
      } else { // If temperature is not normal, check again in 5 minutes.
        startWaitTemperatureTime = currentTime;
      }
    }

    // After 30 minutes of having been manually changed, return to normal functionality
    if (manuallyChanged && startWaitWindowChangeTime < currentTime-(30*60*1000)) {
      manuallyChanged = false;
      if (supposedWindowClosedStatus) {
        closeWindow();
      } else {
        openWindow();
      }
    }

    // Manual Open/Close Detection
    if (currentWindowClosedStatus != supposedWindowClosedStatus) {
      manuallyChanged = true;
      startWaitWindowChangeTime = currentTime;
      saveManualWindowChangeData(currentWindowClosedStatus);
    }
  }


//Push notifs start here 
WiFiClientSecure wifiClient;
AWS_IOT awsIot(wifiClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  wifiClient.setCACert(awsCertCA);
  wifiClient.setCertificate(awsCertCRT);
  wifiClient.setPrivateKey(awsCertPrivateKey);

  awsIot.begin(awsEndpoint, awsRegion, awsAccessKey, awsSecretKey);
  
  while(!awsIot.connect()) {
    Serial.println("Connecting to AWS");
    delay(1000);
  }
  
  Serial.println("Connected to AWS");

  //subscribe to the MQTT topic
  awsIot.subscribe(awsTopic);
}
//loops message
void loop() {
  //check if there's a message received
  if (awsIot.available()) {
    char messageBuf[256];
    int messageSize = awsIot.read(messageBuf, 256);
    messageBuf[messageSize] = '\0';
    Serial.println("Received message: " + String(messageBuf));
    
    //send push notification to SNS topic
    sendNotification(String(messageBuf));
  }
}


  // Adaptation Logic
  /*
  Data from the last 7 days that the window has been manually changed is checked.
  If the window has been opened/closed at a specific time consistently for the last 7 days, then open / close the window at the appropriate time.
  The specific time that the window has been opened/closed has a grace period of 30 minutes.
  E.g Always opening the window between 9:30 and 10:00 for 7 days will still activate this part of the code.
  */

  // Save the sensor data
  saveSensorData(raindropValue, temperatureValue, humidityValue);

  delay(1000);
}
