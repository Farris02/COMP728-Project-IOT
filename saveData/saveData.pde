import processing.serial.*;

Serial mySerial;
PrintWriter sensorDataOutput;
PrintWriter manualChangeDataOutput;
PrintWriter sendNotifOutput;
String sensorDataFile = "../SensorData_Day_0.txt";
String manualChangeDataFile = "../ManualChangeData_Day_0.txt";
String sendNotifFile = "../sendNotifications.txt";

void setup() {
   mySerial = new Serial(this, Serial.list()[0], 9600);
   sensorDataOutput = createWriter(sensorDataFile);
   manualChangeDataOutput = createWriter(manualChangeDataFile);
   sendNotifOutput = createWriter(sendNotifFile);
}

void draw() {
    if (mySerial.available() > 0 ) {
         String value = mySerial.readString();
         if ( value != null ) {
           String[] fileDirectory = new String[2];
           fileDirectory[0] = "../";
           println(value);
           if (value.charAt(0) == '0') {
             sensorDataOutput.println(value.substring(1));
             sensorDataOutput.flush();
           } else if (value.charAt(0) == '1') {
             manualChangeDataOutput.println(value.substring(1));
             manualChangeDataOutput.flush();
           } else if (value.charAt(0) == '2') {
             fileDirectory[1] = value.substring(1);
             sensorDataOutput.flush();
             sensorDataOutput.close();
             sensorDataOutput = createWriter(join(fileDirectory, '\0'));
           } else if (value.charAt(0) == '3') {
             fileDirectory[1] = value.substring(1);
             manualChangeDataOutput.flush();
             manualChangeDataOutput.close();
             manualChangeDataOutput = createWriter(join(fileDirectory, '\0'));
           } else {
             sendNotifOutput.println(value);
             sendNotifOutput.flush();
           }
        }
    }
}

void keyPressed() {
  // Finishes the files
  sensorDataOutput.close();
  manualChangeDataOutput.close();
  sendNotifOutput.close();
  exit();  // Stops the program
}
