long gpsMillis = 0;        // will store last time we got data from the GPS
long gpsInterval = 1000;           // interval at which to get data from the gps (milliseconds)
long gpsUpdateMillis = 0;
long gpsUpdateInterval = 90000;
TinyGPS gps;
const unsigned long gpsAgeTimeOut = 30000;
char isPowerOn = 0;


void checkGPS(){
  gpsUpdateMillis = millis();
  while(locationAvailable == 0){
    unsigned long currentMillis = millis();
    if(currentMillis - gpsUpdateMillis > gpsUpdateInterval){
      if(isPowerOn == 1){
        digitalWrite(gpsPowerPin,LOW);
        isPowerOn = 0;
      }
      break;
    }
    if(isPowerOn == 0){
      //Serial.println("turning on");
      digitalWrite(gpsPowerPin,HIGH);
      isPowerOn = 1;
    }
    feedgps();
    if(currentMillis - gpsMillis > gpsInterval) {
      // save the last time you got GPS data 
      gpsMillis = currentMillis;
      gpsdump(gps);
    }
  }
  //delay(500);
}
static void gpsdump(TinyGPS &gps)
{
  float flat, flon;
  unsigned long age = 0;
  gps.f_get_position(&flat, &flon, &age);
  locationAvailable = checkGPSConnection(age);
  lat = flat;
  lon = flon;
  //  Serial.print(lat);
  //  Serial.print("\t");
  //  Serial.println(lon);
}
int checkGPSConnection(unsigned long age){ 
  if(age != TinyGPS::GPS_INVALID_AGE && age > gpsAgeTimeOut){
    digitalWrite(gpsLED,LOW);
    //gpsConnected = false;
    return 0;
  }
  else if(age == TinyGPS::GPS_INVALID_AGE){
    digitalWrite(gpsLED,LOW);
    //gpsConnected = false;
    return 0;
  }
  else{
    Serial.println("turning off");
    digitalWrite(gpsLED,HIGH);
    //pinMode(gpsPowerPin,OUTPUT);
    digitalWrite(gpsPowerPin,LOW);
    isPowerOn = 0;
    //gpsUpdateMillis = millis();
    return 1;
  }
}
static bool feedgps()
{
  if(debug == false){
    while (Serial.available())
    {
      if (gps.encode(Serial.read()))
        return true;
    }
  }
  return false;
}




