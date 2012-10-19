#define BUFFSIZ 20 // plenty big
char buffer[BUFFSIZ];
char buffidx;
uint8_t retry = 5;
uint8_t counter = 0;
uint8_t commandStatus=0;
uint8_t setupCount=0;
uint8_t attachStatus=0;
uint8_t attachCount=0;
long signalTimeout = 30000;
unsigned long signalTimeCounter = 0;


int setupCommands(){
  setupCount++;

  switch (commandStatus) {
  case 0:
    if(sendCommand("AT&F","OK"))
      commandStatus++;
    setupCommands();
    break;
  case 1:
    if(sendCommand("ATE0","OK"))
      commandStatus++;
    setupCommands();
    break;
  case 2:
    if(sendCommand("AT+CLIP=1","OK"))
      commandStatus++;
    setupCommands();
    break;
  case 3:
    if(sendCommand("AT+CMEE=0","OK"))
      commandStatus++;
    setupCommands();
    break;
  case 4:
    if(sendCommand("AT+CIPSHUT","SHUT"))
      commandStatus++;
    setupCommands();
    break;
  case 5:
    return 1;
    break;
  }
}
int attachGPRS(){
  sendCommand("AT+CGATT?","+CGATT:");
  sendCommand("AT+CIPSHUT","SHUT");
  sendCommand("AT+CSTT=\"wap.cingular\"","OK",1000);
  sendCommand("AT+CIICR","OK",8000);
  if(sendCommand("AT+CIFSR","10",8000)!=0){
    return 1;
  }
  else{
    attachGPRS();
  }
  GPRS_Serial.flush();
  if(debug)
    Serial.flush();
  delay(1000);
}

int getAttachedGPRS(){
  return sendCommand("AT+CGATT?","+CGATT: 1");
}

void ShowSerialData()
{
  Serial.print("SHOW: ");
  while(GPRS_Serial.available()!=0){
    Serial.write(GPRS_Serial.read());
    delay(1);
  }
  GPRS_Serial.flush();
  delay(100);
}
int sendCommand(const char *command, const char *resp, int delayTime, int reps, int numData, int multi){
  memset(buffer,0,20);
  int returnVal;
  int timeout;
  if(delayTime == 500){
    timeout = 500;
  }
  else{
    timeout = delayTime;
  }
  for(int i=0;i<reps;i++){
    if (i > 0) delay(500);
    GPRS_Serial.flush();
    delay(100);
    GPRS_Serial.println(command);
    if(debug){
      Serial.print(command);
      Serial.print(": ");
    }
    delay(200);
    while(GPRS_Serial.available() || multi==1)  {
      if(!readline(timeout))
        break;
      if (strncmp(buffer, resp,numData) == 0) {
        if(debug){
          Serial.println(buffer);
        }
        return 1;
      }
    }
    if(debug){
      Serial.print("FAILED: ");
      Serial.print(strncmp(buffer, resp,numData));
      Serial.println(buffer);
    }
  }
  return 0;
}

int readline(int timeOut) {

  char c;
  int stopBit=0;
  buffidx = 0; // start at begninning
  long previousMillis = millis();
  while (1) {
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > timeOut) {
      if(debug){
        Serial.println("TIMEOUT");
      }
      return 0;
    }
    delay(2);
    c=GPRS_Serial.read();
    if(c == '\r'){
      stopBit = 1;
      continue;
    }
    else if(stopBit == 1){
      if(c=='\n' && buffidx!=0){
        return 1;
      }
    }
    if (c == -1)
      continue;
    if (c == '\n')
      continue;
    if ((buffidx == BUFFSIZ-1)) {
      buffer[buffidx] = 0;
      return 1;
    }
    buffer[buffidx++]= c;
    delay(2);
  }
}
/////////////////////////////////////////
///////* Power Functions for GPRS*///////
/////////////////////////////////////////
int powerOn(){

  digitalWrite(gprsPowerPin,HIGH);
  delay(500);
  if(!powerStatus()){
    if(debug){
      Serial.println("Powering On...");
    }
    pinMode(9, OUTPUT); 
    digitalWrite(9,LOW);
    delay(1000);
    digitalWrite(9,HIGH);
    delay(2000);
    digitalWrite(9,LOW);
    delay(4000);
    //delay(15500);
    signalTimeCounter = millis();
    while(!checkRegistration()){
      unsigned long signalTimer = millis();
      if(signalTimer - signalTimeCounter > signalTimeout){
        if(debug){
      Serial.println("NO SIGNAL...");
    }
        powerOff();
        powerOn();
      }
      //Serial.println("REGISTERED");
      //Serial.print(".");
    }
    if(debug){
      Serial.println("REGISTERED");
    }
    power = true;
    return setupCommands();
  }
  else{
    if(debug){
      Serial.println("ALREADY ON");
    }
    power = true;
    return setupCommands();
  }

}
int powerStatus(){
  return sendCommand("AT","OK",499,3);
}
int checkRegistration(){
  return sendCommand("AT+CREG?","+CREG: 0,1",499,1,10);
}
/////////////////////////////////////////
int powerOff(){
  pinMode(9, OUTPUT); 
  digitalWrite(9,LOW);
  delay(1000);
  digitalWrite(9,HIGH);
  delay(2000);
  digitalWrite(9,LOW);
  delay(1000);
  //power = false;
  digitalWrite(gprsPowerPin,LOW);
  //pinMode(gprsPowerPin,INPUT);
  if(!sendCommand("AT","OK",500,1))
    return 1;
  else
    return 0;
  //delay(10500);
}



