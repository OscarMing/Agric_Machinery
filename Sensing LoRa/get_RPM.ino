void rpmTimerSetting(){
  TCCR1A = 0;
  TCCR1B = 0;
  // Timer 1 - interrupt on overflow
  TIMSK1 = bit (TOIE1);   // enable Timer1 Interrupt
  // zero it
  TCNT1 = 0;  
  overflowCount = 0;  
  // start Timer 1
  TCCR1B =  bit (CS20);  //  no prescaling
  return;
}
// Start TIMER1_OVF_vect
ISR (TIMER1_OVF_vect) 
{
  overflowCount++;
}  
// End of TIMER1_OVF_vect

// Start prepareForInterrupts
void prepareForInterrupts (){
  Counter = 0;
  overflowCount = 0;
  TCNT1 = 0;
  temprpm = 0;
  startTime = (overflowCount << 16) + counter;   
  LockCLC = false;
}  
// End of prepareForInterrupts

//Start isr for hall sensor interrupt
void clcRPM(){
  if(!LockCLC){
    sensorValue = analogRead(sensorPin);
//    Serial.print("sensorValue:");
//    Serial.println(sensorValue);
    if(sensorValue<=35 && prev_sensorValue>800){
      val =1;
      prev_sensorValue = sensorValue;
      }
    else{
      val = 0;
      prev_sensorValue = sensorValue;
      }
  
    if(prev_val==0 && val==1){
      Counter++;
      val = 0;
      }
    prev_val=val;
  }
  else{return;}
}
// end of isr


boolean RPM(){
  LockCLC = true;
  String strR;
  String hexString;
//  finishTime = (overflowCount << 16) + counter;//
  elapsedTime = finishTime - startTime;

//  Serial.print("CTO:");
//  Serial.print(Counter);
//  Serial.print ("\t");
//  Serial.print("CTL:");
//  Serial.print(lastCounter);
//  Serial.print ("\t");
  
  if(Counter == 0){
    lastCounter = Counter;
  }
  if(abs(lastCounter-Counter)<3){
    Counter = (Counter*0.15)+(round(lastCounter)*0.85);
    lastCounter = Counter;
  }
  else if(abs(lastCounter-Counter)<4){
    Counter = (Counter*0.2)+(round(lastCounter)*0.8);
    lastCounter = Counter;
  }
  else if(abs(lastCounter-Counter)<5){
    Counter = (Counter*0.25)+(round(lastCounter)*0.75);
    lastCounter = Counter;
  }
  else{
    lastCounter = Counter;
  }
  
  rpm = (60 * F_CPU / float (elapsedTime))*(Counter);

//  Serial.print("CT:");
//  Serial.print(Counter);
//  Serial.println();
////  Serial.print ("\t");

  for(uint8_t i = 0 ; i<3 ; i++){
    if(i==2){
      rpmArray[i] = rpm;
    }
    else{
      rpmArray[i] = rpmArray[i+1];
      if(i==0){
        temprpm = 0;
      }
    }
    temprpm += rpmArray[i]*weightRPM[i];
  }
  rpms = temprpm;

//  Serial.print ("elapsedTime: ");
//  Serial.println (elapsedTime);
  
  DataArray[2] = rpms ;
  strR = DataType[2]+String(round(rpms));
  char hexBufR[strR.length()+1];
  strR.toCharArray(hexBufR,strR.length()+1);
  for(uint8_t j = 0 ; j<sizeof(hexBufR) ; j++){
    hexString += String(hexBufR[j], HEX);
  }
  hexString = hexString.substring(0,hexString.length()-1);
  hexString.toCharArray(DataString[2],hexString.length()+1);

  RPMLock = true;
//  prepareForInterrupts ();
  return RPMLock;
 
}
