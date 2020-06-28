//read data from different sensors(stored in SD Card) and send to nodeMCU which is further sent to the web server with GET and POST request

#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

#define sensor1 A0
#define sensor2 A0
#define sensor3 A1

String dataString;
const int chipSelect = 4; //choose according to the board

SoftwareSerial nodeSerial(5, 6); //rx, tx

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  nodeSerial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}


String datastringFromSensor(void) {
  //dataString = "pm=1&temp=1&hum=1";
  int pm_Value = analogRead(sensor1);
  int temp_Value = analogRead(sensor2);
  int hum_Value = analogRead(sensor3);

  String stringFromSensor = "pm=" + String(pm_Value) + "&temp=" + String(temp_Value) + "&hum=" + String(hum_Value);
  return stringFromSensor;

}


void loop() {

//  dataString = "pm=1&temp=1&hum=1";
dataString=datastringFromSensor();
Serial.print(dataString);

  //send data to node mcu
  if (nodeSerial.available() > 0) {
    if (nodeSerial.read() == 's') {
      nodeSerial.print(dataString);
      Serial.println(dataString);
    }
  }
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("datalog.csv", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    for (int i = 0; i < 100 ; i++)
      dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    //Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}
