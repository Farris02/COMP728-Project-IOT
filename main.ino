#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Arduino.h>

// Pins
int raindropSensor = 11; // Pin 11 is the pin for the raindrop sensor
int humitureSensor = 12; // Pin 12 is the pin for the humiture sensor
int infraredSensor = 13; // Pin 13 is the pin for the infrared sensor
int In1 = 8; // Pin 8 is the pin for the motor
int In2 = 9; // Pin 9 is the pin for the motor
int enablePin = 10; // Pin 10 is the pin for the motor

// Setting up the save data functionality
import processing.serial.*;
Serial mySerial;
PrintWriter sensorDataOutput;
PrintWriter manualChangeDataOutput;
String sensorDataFile = "SensorData_Day_";
String manualChangeDataFile = "ManualChangeData_Day_";

// Setting up the humiture sensor
DHT dht = DHT(humitureSensor, DHT11);

// Global Variables
unsigned long startWaitRainTime = 0;
unsigned long startWaitHumidityTime = 0;
unsigned long startWaitTemperatureTime = 0;
unsigned long startWaitWindowChangeTime = 0;
unsigned long currentTime = millis(); // Current time in milliseconds since the start of the program.
bool manuallyChanged = false; // Has the status of the window (Opened or Closed) been changed manually by the user.
bool currentWindowClosedStatus = false; // Is the window currently closed.
bool supposedWindowClosedStatus = false; // Is the window supposed to be closed.
bool wasRaining = false; // Was is raining.
bool hasSentHumidityMessage = false; // Has a message about humidity levels already been sent.
bool hasSentTemperatureMessage = false; // Has a message about temperature levels already been sent.
float temperatureValue; // Temperature of area in celcius.
float humidityValue; // Humidity of area in percentage.
int infraredValue; // 1 is not closed, 0 is closed.
int raindropValue; // Detects if it is raining. 1 is not raining, 0 is raining.
int dayCount = 0; // Keeps track of the number of days passed since the start of the program.
unsigned long whenDayCountLastUpdated = 0; // When the dayCount was last updated.

// Closes the window. Returns true if successful. Else returns false.
bool closeWindow() {
  // The value to be returned
  bool returnValue = false;

  // Sets up the motor
  digitalWrite(enablePin, HIGH);
  analogWrite(In1, HIGH);
  analogWrite(In2, LOW);

  if(!currentWindowClosedStatus || currentWindowClosedStatus && infraredValue == 1){
    // Turns on the motor for a period of time
    for (int i=128;i<150;i++)
    {   digitalWrite(In1, HIGH);
        digitalWrite(In2, LOW);
        analogWrite(enablePin, i);
        delay(200);     
    }

    // Turns off the motor
    analogWrite(In1, 0);
    analogWrite(In2, 0);
    digitalWrite(enablePin, LOW);

    returnValue = true;
  }
  
  currentWindowClosedStatus = true;
  supposedWindowClosedStatus = true;

  return returnValue;
}

// Opens the window. Returns true if successful. Else returns false.
bool openWindow() {
  // The value to be returned
  bool returnValue = false;

  // Sets up the motor
  digitalWrite(enablePin, HIGH);
  analogWrite(In2, HIGH); 
  analogWrite(In1, LOW);

  if(!currentWindowClosedStatus || currentWindowClosedStatus && infraredValue == 0){
    // Turns on the motor for a period of time
    for (int i=128;i<150;i++)
      {   digitalWrite(In1, LOW);
          digitalWrite(In2, HIGH);
          analogWrite(enablePin, i);
          delay(200);  
      }

    // Turns off the motor
    analogWrite(In1, 0);
    analogWrite(In2, 0);
    digitalWrite(enablePin, LOW);

    returnValue = true;
  }

  currentWindowClosedStatus = false;
  supposedWindowClosedStatus = false;

  return returnValue;
}

// Sends the argument given as a message to the user's phone. Returns true if successful. Else returns false.
bool sendMessage(String message) {
  // ADD CODE HERE
}

// Saves the sensor data to an external storage location
void saveSensorData(int rainData, float temperatureData, float humidityData) {
  sensorDataOutput.print("Temperature: ");
  sensorDataOutput.print(temperatureValue);
  sensorDataOutput.print(", Humidity: ");
  sensorDataOutput.print(humidityValue);
  if (rainData == 0) {
    sensorDataOutput.print(", Raining: Yes");
  } else {
    sensorDataOutput.print(", Raining: No");
  }
  sensorDataOutput.print(", Time since start: ");
  sensorDataOutput.println(currentTime);

  sensorDataOutput.flush();
}

// Saves the time and date to an external storage location. Is called when the state of the window is manually changed.
void saveManualWindowChangeData(bool currentWindowIsClosedStatus) {
  if (!currentWindowIsClosedStatus) {
    manualChangeDataOutput.print("Window Closed: No");
  } else {
    manualChangeDataOutput.print("Window Closed: Yes");
  }
  manualChangeDataOutput.print(", Time since start: ");
  manualChangeDataOutput.println(currentTime);

  manualChangeDataOutput.flush();
}

void setup() {
  // put your setup code here, to run once:
  pinMode(raindropSensor, INPUT);
  Serial.begin(9600);
  dht.begin();

  // Motor setup
  pinMode(In1, OUTPUT); // IN1
  pinMode(In2, OUTPUT); // IN2
  pinMode(enablePin, OUTPUT); // Enable Pin

  // Save data functionality setup
  sensorDataFile.concat(dayCount);
  manualChangeDataFile.concat(dayCount);
  sensorDataFile.concat(".txt");
  manualChangeDataFile.concat(".txt");
  sensorDataOutput = createWriter(sensorDataFile);
  manualChangeDataOutput = createWriter(manualChangeDataFile);
}

void loop() {
  // put your main code here, to run repeatedly:
  currentTime = millis(); // Current time in milliseconds since the start of the program.
  temperatureValue = dht.readTemperature(); // Temperature of area in celcius.
  humidityValue = dht.readHumidity(); // Humidity of area in percentage.
  infraredValue = digitalRead(infraredSensor); // Detects if the window is closed. 1 is not closed, 0 is closed.
  raindropValue = digitalRead(raindropSensor); // Detects if it is raining. 1 is not raining, 0 is raining.

  // Humiture Sensor Text Output
  Serial.print("Temperature: ");
  Serial.print(temperatureValue);
  Serial.print(", Humidity: ");
  Serial.print(humidityValue);

  // IR Sensor Text Output. Also sets currentWindowClosedStatus.
  if (infraredValue == 1) {
    Serial.print(", Window Closed: No");
    currentWindowClosedStatus = false;
  } else {
    Serial.print(", Window Closed: Yes");
    currentWindowClosedStatus = true;
  }

  // Raindrop sensor Text Output
  if (raindropValue == 0) {
    Serial.println(", Raining: Yes.");
  } else {
    Serial.println(", Raining: No.");
  }

  // Updates the current date if a day has passed, and updates the save data file
  if ((currentTime % (24*60*60*1000)) > 0 && (currentTime % (24*60*60*1000)) < 10000 && currentTime > (whenDayCountLastUpdated+(12*60*60*1000))) {
    // Increases the day count
    dayCount++;
    whenDayCountLastUpdated = currentTime;

    // Updates the file to be saved to
    sensorDataOutput.close();
    manualChangeDataOutput.close();
    sensorDataFile = "SensorData_Day_";
    manualChangeDataFile ="ManualChangeData_Day_";
    sensorDataFile.concat(dayCount);
    manualChangeDataFile.concat(dayCount);
    sensorDataFile.concat(".txt");
    manualChangeDataFile.concat(".txt");
    sensorDataOutput = createWriter(sensorDataFile);
    manualChangeDataOutput = createWriter(manualChangeDataFile);
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
  
  // If the window was manually changed within the last 30 minutes, don't do normal actions.
  if (!manuallyChanged && (startWaitWindowChangeTime != 0 || startWaitWindowChangeTime < currentTime-(30*60*1000))) {
    // Regular Rain Logic
    if (raindropValue == 0) { // Is raining
      wasRaining = true;
      startWaitRainTime = 0;
      closeWindow();
      sendMessage("Raining. Closing Window.");
    } else if (raindropValue == 1 && wasRaining && startWaitRainTime == 0) { // Rain has stopped
      startWaitRainTime = currentTime;
    } else if (raindropValue == 1 && wasRaining && startWaitRainTime < currentTime-(10000) && startWaitRainTime != 0) { // Has not rained for 5 minutes
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

    // Manual Open/Close Detection
    if (currentWindowClosedStatus != supposedWindowClosedStatus && !manuallyChanged) {
      manuallyChanged = true;
      startWaitWindowChangeTime = currentTime;
      saveManualWindowChangeData(currentWindowClosedStatus);
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

  delay(5000);
}
