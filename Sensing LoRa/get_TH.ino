void dataTH() {
  if((millis()-lastTime) >= 1000 && (millis()-lastTime) <= 1800){
    float h = dht.readHumidity();
    float t = dht.readTemperature();
//    float f = dht.readTemperature(true);
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      DataArray[0] = DataArray[0];
      DataArray[1] = DataArray[1];
    }
    else{
      DataArray[0] = t ;
      DataArray[1] = h ;
    }
     if (h == 0.0 || t == 0.0) {
      DataArray[0] = DataArray[0];
      DataArray[1] = DataArray[1];
    }
    else{
      DataArray[0] = t ;
      DataArray[1] = h ;
    }   
  }
}

boolean get_THString(){
  String strT, strH;
  String hexString;
  uint8_t nTH[2];
  
  strT = DataType[0]+String(DataArray[0]*10).substring(0,String(DataArray[0]*10).length()-3);
  strH = DataType[1]+String(DataArray[1]*10).substring(0,String(DataArray[1]*10).length()-3);


  char hexBufT[strT.length()+1];
  char hexBufH[strH.length()+1];

  strT.toCharArray(hexBufT,strT.length()+1);
  strH.toCharArray(hexBufH,strT.length()+1);

  nTH[0] = sizeof(hexBufT);
  nTH[1] = sizeof(hexBufH);

  
  for(uint8_t i = 0 ; i<2 ; i++){
    for(uint8_t j = 0 ; j<nTH[i] ; j++){
      
      if(i==0){
        hexString += String(hexBufT[j], HEX);
      }
      else if(i==1){
        hexString += String(hexBufH[j], HEX);
      }
    }
    hexString = hexString.substring(0,hexString.length()-1);
//    Serial.print("hexString:");
//    Serial.println(hexString);
    hexString.toCharArray(DataString[i],hexString.length()+1);
    hexString = "";
  }

  THLock = true;

  return THLock;
}
  
