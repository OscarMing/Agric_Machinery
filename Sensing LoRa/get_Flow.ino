// Start prepareForInterrupts
void prepareForFW (){
  CounterFW = 0;
  tempFW = 0;
  startTimeFW = millis();   
  LockCLCFW = false;
}  
// End of prepareForInterrupts

//Start isr for hall sensor interrupt
void clcFW(){
  if(!LockCLCFW){
    sensorValueFW = analogRead(sensorPinFW);
//    Serial.print("sensorValue:");
//    Serial.println(sensorValue);
    if(sensorValueFW<=35 && prev_sensorValueFW>800){
      valFW =1;
      prev_sensorValueFW = sensorValueFW;
      }
    else{
      valFW = 0;
      prev_sensorValueFW = sensorValueFW;
      }
  
    if(prev_valFW==0 && valFW==1){
      CounterFW++;
      valFW = 0;
      }
    prev_valFW=valFW;
  }
  else{return;}
}
// end of isr


boolean FWC(){
  LockCLCFW = true;
  String strF;
  String hexString;
//  finishTime = (overflowCount << 16) + counter;//
  elapsedTimeFW = finishTimeFW - startTimeFW;

//  Serial.print("CTO:");
//  Serial.print(Counter);
//  Serial.print ("\t");
//  Serial.print("CTL:");
//  Serial.print(lastCounter);
//  Serial.print ("\t");
  
  if(CounterFW == 0){
    lastCounterFW = CounterFW;
  }
  if(abs(lastCounterFW-CounterFW)<3){
    CounterFW = (CounterFW*0.15)+(round(lastCounterFW)*0.85);
    lastCounterFW = CounterFW;
  }
  else if(abs(lastCounterFW-CounterFW)<4){
    CounterFW = (CounterFW*0.2)+(round(lastCounterFW)*0.8);
    lastCounterFW = CounterFW;
  }
  else if(abs(lastCounterFW-CounterFW)<5){
    CounterFW = (CounterFW*0.25)+(round(lastCounterFW)*0.75);
    lastCounterFW = CounterFW;
  }
  else{
    lastCounterFW = CounterFW;
  }
  
//  FW = (60000*(CounterFW)/float (elapsedTimeFW))/75.0;
  FW = 60000*(CounterFW)/float (elapsedTimeFW);

//  Serial.print("CTFW:");
//  Serial.print(CounterFW);
////  Serial.println();
//  Serial.print ("\t");

  for(uint8_t i = 0 ; i<3 ; i++){
    if(i==2){
      FWArray[i] = FW;
    }
    else{
      FWArray[i] = FWArray[i+1];
      if(i==0){
        tempFW = 0;
      }
    }
    tempFW += FWArray[i]*weightFW[i];
  }
  FWS = tempFW;

//  Serial.print ("elapsedTimeFW: ");
//  Serial.println (elapsedTimeFW);

  
  DataArray[5] = FWS ;

  strF = String(FWS*10000).substring(0,(String(FWS*10000).length()-3));

  char hexBufR[strF.length()+1];
  strF.toCharArray(hexBufR,strF.length()+1);
  for(uint8_t j = 0 ; j<sizeof(hexBufR) ; j++){
    hexString += String(hexBufR[j], HEX);
  }
  hexString = hexString.substring(0,hexString.length()-1);
  hexString.toCharArray(DataString[5],hexString.length()+1);

  FWLock = true;
//  prepareForInterrupts ();
  return FWLock;
 
}
