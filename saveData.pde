import processing.serial.*;
Serial mySerial;
PrintWriter sensorDataOutput;
PrintWriter manualChangeDataOutput;
String sensorDataFile = "SensorData_Day_0.txt";
String manualChangeDataFile = "ManualChangeData_Day_0.txt";

void setup() {
   mySerial = new Serial( this, Serial.list()[0], 9600 );
   sensorDataOutput = createWriter(sensorDataFile);
   manualChangeDataOutput = createWriter(manualChangeDataFile);
}

void draw() {
    if (mySerial.available() > 0 ) {
         String value = mySerial.readString();
         if ( value != null ) {
           if (value.charAt(0) == '0') {
             sensorDataOutput.println(value.substring(1));
             sensorDataOutput.flush();
           } else if (value.charAt(0) == '1') {
             manualChangeDataOutput.println(value.substring(1));
             manualChangeDataOutput.flush();
           } else if (value.charAt(0) == '2') {
             sensorDataOutput.close();
             sensorDataOutput = createWriter(value.substring(1));
           } else if (value.charAt(0) == '3') {
             manualChangeDataOutput.close();
             manualChangeDataOutput = createWriter(value.substring(1));
         }
    }
}

void keyPressed() {
  sensorDataOutput.close();
    manualChangeDataOutput.close();  // Finishes the file
    exit();  // Stops the program
}
