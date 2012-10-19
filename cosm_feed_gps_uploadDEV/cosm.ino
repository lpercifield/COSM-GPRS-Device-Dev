static const int kCalculateDataLength =0;
static const int kSendData =1;
int cosmPUT(CosmFeed& aFeed, const char* aApiKey)
{
  int didConnect = sendCommand("AT+CIPSTART=\"tcp\",\"api.cosm.com\",\"80\"","CONNECT OK", 20000,5,10,1);
  if(didConnect == 0){
    blinkLED(uploadLED,5,100);
    attachGPRS();
    return 0;
  }
  digitalWrite(uploadLED,HIGH);
  //sendCommand("AT+CIPSTART=\"tcp\",\"google.com\",\"80\"","CONNECT OK", 20000,5,10,1);
  char path[30];
  buildPath(path, aFeed.id(), "HTTP/1.1");
  //http.beginRequest();
  //int ret = http.put("api.cosm.com", path);
  //Serial.println(path);
  GPRS_Serial.println("AT+CIPSEND");//begin send data to remote server
  delay(500);
  //GPRS_Serial.print("PUT /1791j1g1 HTTP/1.1\r\n");
  GPRS_Serial.print("PUT ");
  GPRS_Serial.print(path);
  GPRS_Serial.print(" HTTP/1.1\r\n");
  if(debug){
    Serial.print("PUT ");
    Serial.print(path);
    Serial.print(" HTTP/1.1\r\n");
  }
  delay(500);
  GPRS_Serial.print("Host: api.cosm.com\r\n"); 
  delay(500);
  //if (ret == 0)
  //{
  //http.sendHeader("X-ApiKey", aApiKey);
  GPRS_Serial.print("X-ApiKey: ");
  GPRS_Serial.println(aApiKey);
  delay(500);


  CountingStream countingStream; // Used to work out how long that data will be
  for (int i =kCalculateDataLength; i <= kSendData; i++)
  {
    Print* s;
    int len =0;
    if (i == kCalculateDataLength)
    {
      s = &countingStream;
    }
    else
    { 
      GPRS_Serial.print("Connection: close\r\n\r\n");
      s = &GPRS_Serial;
      //Serial.println("else");
    }
    len = s->print(aFeed);
    delay(500);
    if (i == kCalculateDataLength)
    {
      // We now know how long the data will be...
      //http.sendHeader("Content-Length", len);
      GPRS_Serial.print("Content-Length: ");
      GPRS_Serial.println(len);
      delay(500);
    }
  }
  // Now we're done sending the request
  //http.endRequest();
  GPRS_Serial.println();
  GPRS_Serial.println();
  GPRS_Serial.write(0x1A);
  if(debug)
    Serial.println("DONE");
  delay(1000);
  if(debug){
    ShowSerialData();
  }
  sendCommand("AT+CIPCLOSE","CLOSE OK",2000,3);
  digitalWrite(uploadLED,LOW);
  //    ret = http.responseStatusCode();
  //    if ((ret < 200) || (ret > 299))
  //    {
  //      // It wasn't a successful response, ensure it's -ve so the error is easy to spot
  //      if (ret > 0)
  //      {
  //        ret = ret * -1;
  //      }
  //    }
  //    http.stop();
  //}
  return 1;
}
void buildPath(char* aDest, unsigned long aFeedId, const char* aFormat)
{
  char idstr[12]; 
  strcpy(aDest, "/v2/feeds/");
  char* p = &idstr[10];
  idstr[11] = 0;
  for(*p--=aFeedId%10+0x30;aFeedId/=10;*p--=aFeedId%10+0x30);
  strcat(aDest, p+1);
  //strcat(aDest, " ");
  //strcat(aDest, aFormat);
}


