import processing.serial.*;

Serial mySerial;
PrintWriter sensorDataOutput;
PrintWriter manualChangeDataOutput;
PrintWriter sendNotifOutput;
String sensorDataFile = "../SensorData_Day_0.txt";
String manualChangeDataFile = "../ManualChangeData_Day_0.txt";
String sendNotifFile = "../sendNotifications.txt";
boolean started = false;


void setup() {
   mySerial = new Serial( this, Serial.list()[0], 9600 );
   sensorDataOutput = createWriter(sensorDataFile);
   manualChangeDataOutput = createWriter(manualChangeDataFile);
   sendNotifOutput = createWriter(sendNotifFile);
}

void draw() {
    // Checks if anything is in the serial port.
    if (mySerial.available() > 0 ) {
         // Reads each line of the serial port.
         String value = mySerial.readStringUntil('\n');
         
         // If there is something in the serial port.
         if ( value != null ) {
           // Used to create files outside of the immediate folder.
           String[] fileDirectory = new String[2];
           fileDirectory[0] = "..";
           
           // Makes sure that the files are initiated first, before the data starts being recorded.
           if ((value.charAt(0) == '2' || value.charAt(0) == '3') && !started) {
             started = true;
           }
           
           // Prints data into console.
           if (started) {
             print(value);
           }
           
           // Uses the data appropriately.
           if (value.charAt(0) == '0' && started) { // Prints the data in the Sensor Data file.
             sensorDataOutput.print(value.substring(1));
             sensorDataOutput.flush();
           } else if (value.charAt(0) == '1' && started) { // Prints the data in the Manual Change Data file.
             manualChangeDataOutput.print(value.substring(1));
             manualChangeDataOutput.flush();
           } else if (value.charAt(0) == '2') { // Changes the Sensor Data file to be saved to.
             fileDirectory[1] = value.substring(1, value.length()-2);
             sensorDataOutput.flush();
             sensorDataOutput.close();
             sensorDataOutput = createWriter(join(fileDirectory, '/'));
           } else if (value.charAt(0) == '3') { // Changes the Manual Change Data file to be saved to.
             fileDirectory[1] = value.substring(1, value.length()-2);
             manualChangeDataOutput.flush();
             manualChangeDataOutput.close();
             manualChangeDataOutput = createWriter(join(fileDirectory, '/'));
           } else if (started) { // Outputs the data into the Send Notification file.
             sendNotifOutput.print(value);
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
