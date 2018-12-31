void LCDPrint(){

  switch(LCD_flag){
    case 0:
        lcd.clear();
        lcd.setCursor(0,0); lcd.print(DataType[0]); lcd.print(DataArray[0]);
        lcd.setCursor(0,1); lcd.print(DataType[1]); lcd.print(DataArray[1]);
    break;
    case 1:
        lcd.clear();
        lcd.setCursor(0,0); lcd.print("A:"); lcd.print(String(DataArray[3],7));
        lcd.setCursor(0,1); lcd.print("O:"); lcd.print(String(DataArray[4],7)); 
    break;

    case 2:
        lcd.clear();
        lcd.setCursor(0,0); lcd.print(DataType[2]); lcd.print(DataArray[2]);
        lcd.setCursor(0,1); lcd.print("S:"); lcd.print(DataArray[5]);
    break;
    
    default:
        Serial.println("No LCD flag found ...");
    break;
  
  }
}
