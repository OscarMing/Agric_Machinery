boolean Speed(){
  double Speed =  0.0;
  String strF;
  String hexString;
  if(gps.speed.isValid()){
//    Speed = gps.speed.kmph();
    Speed = gps.speed.mps();
    DataArray[5] = Speed;
    lastSpeed = Speed;
  }
  else{
    DataArray[5] = DataArray[5];
    Speed = lastSpeed;
  }

  strF = String(Speed*100).substring(0,(String(Speed*100).length()-3));
  char hexBufR[strF.length()+1];
  strF.toCharArray(hexBufR,strF.length()+1);
  for(uint8_t j = 0 ; j<sizeof(hexBufR) ; j++){
    hexString += String(hexBufR[j], HEX);
  }
  hexString = hexString.substring(0,hexString.length()-1);
  hexString.toCharArray(DataString[5],hexString.length()+1);

  SPLock = true;
}
