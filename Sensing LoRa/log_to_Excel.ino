void LogULTOEX(uint8_t index1,uint8_t index2,uint8_t index3){
  ULCount[index1]+=1;
  ULCount[index2]+=1;

  uint8_t fn = 2;
  if(index3==3){fn = 8;}
  else{fn = 2;}
  
  Serial.print(DataType[index1]);
  Serial.print(",");
  Serial.print(DataArray[index1],fn);
  Serial.print(",");
  Serial.print(ULCount[index1]);
  Serial.print(",");
  Serial.print(DataType[index2]);
  Serial.print(",");
  Serial.print(DataArray[index2],fn);
  Serial.print(",");
  Serial.print(ULCount[index2]);
  Serial.println();
}

void LogDLTOEX(String RDATA, uint16_t DCOUNT){
  Serial.print(RDATA);
  Serial.print(",");
  Serial.println(DCOUNT);
  
}
