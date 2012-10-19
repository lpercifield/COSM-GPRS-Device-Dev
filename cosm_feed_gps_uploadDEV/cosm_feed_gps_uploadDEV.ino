
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Cosm.h>
#include <CountingStream.h>
#include <HttpClient.h>
#include <TinyGPS.h>
#include <Wire.h>
#include <DS3231.h>
//GPS STUFF
int locationAvailable = 0;
float lat;
float lon;
int gpsPowerPin = 5;

/////COSM
char cosmKey[] = "rewjrtZS0MqOWG4R-j4EJ5u9yPYOvCwKUMuK8O_cwbk";
char sensorId[] = "sensor_reading"; //NAME OF FIRST DATASTREAM
char sensorTemp[] = "temp"; //NAME OF SECOND DATASTREAM
CosmDatastream datastreams[] = {
  CosmDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
  CosmDatastream(sensorTemp, strlen(sensorTemp), DATASTREAM_FLOAT)
  };
  // Finally, wrap the datastreams into a feed
  //70062 TEST DEVICE ID
  //71441 device #1
  //71453 device #2
  //71456 device #3
  //71459 device #4
  //71462 device #5
  //71463 device #6
  CosmFeed feed(70062, datastreams, 2 /* number of datastreams */,1 /* mobile = 1 fixed = 0 */);


//////GPRS
int sendCommand(const char *command, const char *resp, int delayTime=500, int reps=5, int numData=2, int multi=1);
SoftwareSerial GPRS_Serial(7, 8);
boolean config = false;
boolean power = false;
int gprsPowerPin = A0;


////RTC/Temp Sensor
DS3231 RTC; //Create RTC object for DS3231 RTC come Temp Sensor

///APP STUFF
long previousMillis = 0;        // will store last time the data was uploaded
long interval = 300000;           // interval at which to upload the data
int sensorValue = 0;
int gpsLED = 13;
int uploadLED = 12;
boolean startup = true;

boolean debug = true; //DO YOU WANT SERIAL OUTPUT TO DEBUG??

/////////
void setup() {
  Serial.begin(9600);
  //  while(!Serial){ //need this if using a leonardo
  //    ;
  //  }
  if(debug)
    Serial.println("start setup");
  pinMode(gpsLED, OUTPUT);
  pinMode(uploadLED, OUTPUT);
  //control GPRS power
  pinMode(gprsPowerPin,OUTPUT);
  digitalWrite(gprsPowerPin,LOW); //MAKE SURE IT STARTS UP OFF
  //control GPS power
  pinMode(gpsPowerPin,OUTPUT);
  digitalWrite(gpsPowerPin,LOW);// MAKE SURE IT STARTS UP OFF

  Wire.begin();
  GPRS_Serial.begin(19200);


  RTC.begin();

  startUpBlink();
  if(debug)
    Serial.println("end setup");
  delay(1000);


}

void loop() {


  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval || startup == true) {
    
    if(debug == false){
      checkGPS();
    }
    powerOn(); //Turn on the GPRS modem
    attachGPRS(); //This creates a GPRS connection
    RTC.convertTemperature();          //convert current temperature into registers
    float temp = RTC.getTemperature(); //Read temperature sensor value
    if(debug){
      Serial.print("temp: ");
      Serial.println(temp);
    }
    datastreams[0].setFloat(sensorValue);
    datastreams[1].setFloat(temp);
    if(locationAvailable == 1){
      feed.setLoc(lat,lon);
    }
    cosmPUT(feed,cosmKey);
    sensorValue++;
    powerOff();
    previousMillis = millis();
    startup = false;
  }
  //delay(1000);

}
void startUpBlink(){
  blinkLED(gpsLED,2,200);
  blinkLED(uploadLED,2,200);
}
// this function blinks the an LED light as many times as requested, at the requested blinking rate
void blinkLED(byte targetPin, int numBlinks, int blinkRate) {
  for (int i=0; i<numBlinks; i++) {
    digitalWrite(targetPin, HIGH);   // sets the LED on
    delay(blinkRate);                     // waits for blinkRate milliseconds
    digitalWrite(targetPin, LOW);    // sets the LED off
    delay(blinkRate);
  }
}



