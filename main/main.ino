#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Arduino.h>

// Pins.
int manualWindowButton = 13;  // Pin 13 is the pin for the manual window change button.
int humitureSensor = 12;      // Pin 12 is the pin for the humiture sensor.
int raindropSensor = 11;      // Pin 11 is the pin for the raindrop sensor.
int enablePin = 10;           // Pin 10 is the pin for the motor.
int In2 = 9;                  // Pin 9 is the pin for the motor.
int In1 = 8;                  // Pin 8 is the pin for the motor.
int closeInfraredSensor = 7;  // Pin 7 is the pin for the close infrared sensor.
int openInfraredSensor = 6;   // Pin 6 is the pin for the open infrared sensor.

// Setting up the save data functionality.
String sensorDataFile = "2SensorData_Day_0.txt";
String manualChangeDataFile = "3ManualChangeData_Day_0.txt";

// Setting up the humiture sensor.
DHT dht = DHT(humitureSensor, DHT11);

// Global Variables.
unsigned long startWaitRainTime = 0;
unsigned long startWaitHumidityTime = 0;
unsigned long startWaitTemperatureTime = 0;
unsigned long startWaitWindowChangeTime = 0;
unsigned long currentTime = millis();     // Current time in milliseconds since the start of the program.
bool manuallyChanged = false;             // Has the status of the window (Opened or Closed) been changed manually by the user.
bool currentWindowClosedStatus = false;   // Is the window currently closed.
bool supposedWindowClosedStatus = false;  // Is the window supposed to be closed.
bool wasRaining = false;                  // Was is raining.
bool hasSentHumidityMessage = false;      // Has a message about humidity levels already been sent.
bool hasSentTemperatureMessage = false;   // Has a message about temperature levels already been sent.
float temperatureValue;                   // Temperature of area in celcius.
float humidityValue;                      // Humidity of area in percentage.
int raindropValue;                        // Detects if it is raining. 1 is not raining, 0 is raining.
int manualWindowButtonValue;              // Detects if the manual window change button is being pressed.
int dayCount = 0;                         // Keeps track of the number of days passed since the start of the program.
int currentWindowPeriod = 0;              // The current 20-minute period of the day;
unsigned long adjustedCurrentTime = 0;    // Used to make sure overflow errors do not occur.

// Constants.
const unsigned long ADAY = 1800000;      //86400000;
const unsigned long HALFANHOUR = 37500;  //1800000;
const unsigned long FIVEMINUTES = 6250;  //300000;

// Day array.
char days[7][72];                    // Array of 20 minute periods for 7 days.
char permanentChange[72];            // The array of permanent window adaptations.
int alreadyChangedWindowState = -1;  // Keeps track of if the window's state has already been changed for the current 30 minute period.

// Closes the window. Returns true if successful. Else returns false.
bool closeWindow() {
  // The value to be returned.
  bool returnValue = false;

  // Sets up the motor.
  digitalWrite(enablePin, HIGH);
  analogWrite(In1, HIGH);
  analogWrite(In2, LOW);

  if (!currentWindowClosedStatus) {
    // Turns on the motor for a period of time.
    digitalWrite(In1, HIGH);
    digitalWrite(In2, LOW);
    analogWrite(enablePin, 150);

    for (;;) {
      // Ensures the motor doesn't run excessively.
      if (digitalRead(closeInfraredSensor) == 0) {  // Detects if the window is closed. 1 is not closed, 0 is closed.
        break;
      }
    }

    // Turns off the motor.
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
  // The value to be returned.
  bool returnValue = false;

  // Sets up the motor.
  digitalWrite(enablePin, HIGH);
  analogWrite(In2, HIGH);
  analogWrite(In1, LOW);

  if (currentWindowClosedStatus) {
    // Turns on the motor for a period of time.
    digitalWrite(In1, LOW);
    digitalWrite(In2, HIGH);
    analogWrite(enablePin, i);

    for (;;) {
      // Ensures the motor doesn't run excessively.
      if (digitalRead(openInfraredSensor) == 0) {  // Detects if the window is open. 1 is not open, 0 is open.
        break;
      }
    }

    // Turns off the motor.
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
// Message given must not contain 0, 1, 2, or 3 at the beginning of the message.
bool sendMessage(String message) {
  if (message != "" || message != NULL || message.charAt(0) == '0' || message.charAt(0) == '1' || message.charAt(0) == '2' || message.charAt(0) == '3') {
    Serial.println(message);
    delay(1000);

    return true;
  }

  return false;
}

// Saves the sensor data to an external text file.
void saveSensorData(int rainData, float temperatureData, float humidityData) {
  String outputString = "0";
  if (currentWindowIsClosedStatus) {
    outputString.concat("Window Closed: Yes");
  } else {
    outputString.concat("Window Closed: No");
  }
  outputString.concat(", Temperature: ");
  outputString.concat(temperatureValue);
  outputString.concat(", Humidity: ");
  outputString.concat(humidityValue);
  if (rainData == 0) {
    outputString.concat(", Raining: Yes");
  } else {
    outputString.concat(", Raining: No");
  }
  outputString.concat(", Time since start: ");
  outputString.concat(currentTime);

  Serial.println(outputString);
}

// Saves the time and date to an external text file. Is called when the state of the window is manually changed.
void saveManualWindowChangeData(bool currentWindowIsClosedStatus) {
  String outputString = "1";
  if (currentWindowIsClosedStatus) {
    outputString.concat("Window Closed: Yes");
  } else {
    outputString.concat("Window Closed: No");
  }
  outputString.concat(", Time since start: ");
  outputString.concat(currentTime);

  Serial.println(outputString);
}

void manualWindowChange() {
  bool holdSupposedWindowClosedStatus = supposedWindowClosedStatus;
  if (currentWindowClosedStatus) {
    openWindow();
  } else {
    closeWindow();
  }
  startWaitWindowChangeTime = currentTime;
  supposedWindowClosedStatus = holdSupposedWindowClosedStatus;
  saveManualWindowChangeData(currentWindowClosedStatus);

  // Log the change in the day array for the current day.
  if (currentWindowClosedStatus) {
    days[0][currentWindowPeriod] = 1;
  } else {
    days[0][currentWindowPeriod] = 0;
  }

  // Checks the last 7 days for changes
  int closeAtThisTime = 0;
  int openAtThisTime = 0;
  int periodBefore = currentWindowPeriod - 1;
  int periodAfter = currentWindowPeriod + 1;
  if (periodBefore < 0) {
    periodBefore = 71;
  }
  if (periodAfter > 71) {
    periodAfter = 0;
  }

  // Checks the time periods before and after the current time period for the last 7 days to see if there's an adaptation pattern.
  if (days[6][currentWindowPeriod] == '2') {  // If, in the last 7, the window was manually closed.
    closeAtThisTime++;
    for (int checkDay = 0; checkDay <= 5; checkDay++) {
      if (days[checkDay][currentWindowPeriod] == '2' || days[checkDay][periodBefore] == '2' || days[checkDay][periodAfter] == '2') {
        closeAtThisTime++;
      }
    }
    // If all the time periods checked are 1, add it to the permanentChange array.
    if (closeAtThisTime >= 6) {
      alreadyChangedWindowState = currentWindowPeriod;
      permanentChange[currentWindowPeriod] = '2';
    }
  } else if (days[6][currentWindowPeriod] == '1') {  // If, in the last 7, the window was manually opened.
    openAtThisTime++;
    for (int checkDay = 0; checkDay <= 5; checkDay++) {
      if (days[checkDay][currentWindowPeriod] == '1' || days[checkDay][periodBefore] == '1' || days[checkDay][periodAfter] == '1') {
        openAtThisTime++;
      }
    }
    // If all the time periods checked are 2, add it to the permanentChange array.
    if (openAtThisTime >= 6) {
      alreadyChangedWindowState = currentWindowPeriod;
      permanentChange[currentWindowPeriod] = '1';
    }
  }

  manuallyChanged = true;
}

unsigned long adjustCurrentTime(unsigned long period) {
  unsigned long returnValue = currentTime - period;

  if (currentTime < period) {
    returnValue = 0;
  }

  return returnValue;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(raindropSensor, INPUT);
  Serial.begin(9600);
  dht.begin();

  // Motor setup.
  pinMode(In1, OUTPUT);        // IN1.
  pinMode(In2, OUTPUT);        // IN2.
  pinMode(enablePin, OUTPUT);  // Enable Pin.

  // Sets all permanent change values and day 0 values to -1.
  for (int i = 0; i < 72; i++) {
    permanentChange[i] = '0';
    days[0][i] = '0';
  }

  // Start the new data on a new line
  Serial.println("");

  // Resets the save file names
  Serial.println(sensorDataFile);
  delay(1000);
  Serial.println(manualChangeDataFile);
  delay(1000);

  // Makes the window initially open
  openWindow();
}

void loop() {
  // put your main code here, to run repeatedly:
  currentTime = millis();                       // Current time in milliseconds since the start of the program.
  temperatureValue = dht.readTemperature();     // Temperature of area in celcius.
  humidityValue = dht.readHumidity();           // Humidity of area in percentage.
  raindropValue = digitalRead(raindropSensor);  // Detects if it is raining. 1 is not raining, 0 is raining.
  currentWindowPeriod = int(((currentTime % ADAY) / HALFANHOUR));

  // Updates the current date if a day has passed, and updates the save data file.
  if (int(currentTime / ADAY) > dayCount) {
    // Increases the day count.
    dayCount++;

    // Updates which file to be saved to.
    sensorDataFile = "2SensorData_Day_";
    manualChangeDataFile = "3ManualChangeData_Day_";
    sensorDataFile.concat(dayCount);
    manualChangeDataFile.concat(dayCount);
    sensorDataFile.concat(".txt");
    manualChangeDataFile.concat(".txt");
    Serial.println(sensorDataFile);
    delay(1000);
    Serial.println(manualChangeDataFile);
    delay(1000);

    // Shifts all the data in each day by 1, forgetting any data later than 7 days ago.
    for (int a = 6; a > 0; a--) {
      for (int b = 0; b < 72; b++) {
        days[a][b] = days[a - 1][b];
      }
    }
    for (int i = 0; i < 72; i++) {
      days[0][i] = '0';
    }

    // Resets the "alreadyChangedWindowState" variable to -1, so that each window period can be checked again for permanent adaptation changes.
    alreadyChangedWindowState = -1;
  }

  // After 30 minutes of having been manually changed, return to normal functionality.
  adjustedCurrentTime = adjustCurrentTime(HALFANHOUR);
  if (manuallyChanged && startWaitWindowChangeTime < adjustedCurrentTime && startWaitWindowChangeTime != 0) {
    manuallyChanged = false;
    if (supposedWindowClosedStatus) {
      closeWindow();
    } else {
      openWindow();
    }
    startWaitWindowChangeTime = 0;
  }

  // If the window was manually changed within the last 30 minutes, don't do normal actions.
  if (!manuallyChanged) {
    // Regular Rain Logic.
    adjustedCurrentTime = adjustCurrentTime(FIVEMINUTES);
    if (raindropValue == 0 && !wasRaining) {  // Is raining.
      wasRaining = true;
      startWaitRainTime = 0;
      sendMessage("Raining. Closing Window.");
      closeWindow();
    } else if (raindropValue == 1 && wasRaining && startWaitRainTime == 0) {  // Rain has stopped.
      startWaitRainTime = currentTime;
    } else if (raindropValue == 1 && wasRaining && startWaitRainTime < adjustedCurrentTime && startWaitRainTime != 0) {  // Has not rained for 5 minutes.
      wasRaining = false;
      sendMessage("Not raining. Opening Window.");
      openWindow();
    }

    // Regular Humiture Logic.
    adjustedCurrentTime = adjustCurrentTime(FIVEMINUTES);
    if (!hasSentHumidityMessage) {
      if (humidityValue > 90) {  // If the humidity is greater than 90% send a message.
        sendMessage("Current Humidity greater than 90%! Please turn on your air conditioning.");
        startWaitHumidityTime = currentTime;
        hasSentHumidityMessage = true;
      } else if (humidityValue < 10) {  // If the humidity is less than 10% send a message.
        sendMessage("Current Humidity less than 10%! Please turn on your air conditioning.");
        startWaitHumidityTime = currentTime;
        hasSentHumidityMessage = true;
      }
    } else if (startWaitHumidityTime < adjustedCurrentTime) {  // If a message has been sent, and it's been 5 minutes since the last check.
      if (humidityValue < 85 && humidityValue > 15) {          // If the humidity is normal, reset values.
        hasSentHumidityMessage = false;
        startWaitHumidityTime = 0;
      } else {  // If humidity is not normal, check again in 5 minutes.
        startWaitHumidityTime = currentTime;
      }
    }

    // Regular Temperature Logic.
    adjustedCurrentTime = adjustCurrentTime(FIVEMINUTES);
    if (!hasSentTemperatureMessage) {
      if (temperatureValue > 35) {  // If the temperature is greater than 90% send a message.
        sendMessage("Current Temperature greater than 30 Degrees Celcius! Please turn on your air conditioning.");
        startWaitTemperatureTime = currentTime;
        hasSentTemperatureMessage = true;
      } else if (temperatureValue < 5) {  // If the temperature is less than 10% send a message.
        sendMessage("Current Temperature less than 5 Degrees Celcius! Please turn on your air conditioning.");
        startWaitTemperatureTime = currentTime;
        hasSentTemperatureMessage = true;
      }
    } else if (startWaitTemperatureTime < adjustedCurrentTime) {  // If a message has been sent, and it's been 5 minutes since the last check.
      if (temperatureValue < 30 && temperatureValue > 10) {       // If the temperature is normal, reset values.
        hasSentTemperatureMessage = false;
        startWaitTemperatureTime = 0;
      } else {  // If temperature is not normal, check again in 5 minutes.
        startWaitTemperatureTime = currentTime;
      }
    }
  }

  // Automatically Open/Close the window if the state is stored in the list.
  if (currentWindowPeriod > alreadyChangedWindowState) {
    if (permanentChange[currentWindowPeriod] == '2') {
      closeWindow();
      alreadyChangedWindowState = currentWindowPeriod;
    } else if (permanentChange[currentWindowPeriod] == '1') {
      openWindow();
      alreadyChangedWindowState = currentWindowPeriod;
    }
  }

  // Save the sensor data.
  saveSensorData(raindropValue, temperatureValue, humidityValue);

  // Checks if the button has been pressed while it delays for a second.
  for (int delayTime = 100; delayTime > 0; delayTime--) {
    manualWindowButtonValue = digitalRead(manualWindowButton);  // Detects if the manual window change button is being pressed.
    if (manualWindowButtonValue == 0) {
      manualWindowChange();
      break;
    }
    delay(10);
  }
}