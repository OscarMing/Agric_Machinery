/*=====================================================//
#2018/11/12
#NCYU BME BCELAB LoRaWAN Project
#Gateway uesd GIOT Indoor or Outdoor Gateway
#LoRa Node used Acsip S76 module
#LoRa Node used Acsip S76G module
#Support Class A & C Mode
#Design by Hugo Lin and ref from Acsip sourse
#Support Partner : Danny Deng, Kevin Tang(NCYU 107)
#Add Log Data to Excel 
#Version 2.5.0
//=====================================================*/
//Library Include
#include <Timer.h>
#include <Streaming.h>
#include <TinyGPS++.h>
#include <math.h>
#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

//RPM Measurement Variable
volatile unsigned long overflowCount;
volatile unsigned long startTime;
volatile unsigned long finishTime;
volatile float Counter = 0;
volatile float lastCounter = 0;

unsigned long elapsedTime;
unsigned int counter = TCNT1;

float rpm, rpms, temprpm;
float rpmArray[3];
float weightRPM[] = {0.1, 0.3, 0.6};

boolean LockCLC = false;


uint8_t sensorPin = A0;
uint8_t val = 0;
uint8_t prev_val = 0;

uint16_t sensorValue = 0;
uint16_t prev_sensorValue = 0;


//Flow
volatile unsigned long startTimeFW;
volatile unsigned long finishTimeFW;
volatile float CounterFW = 0;
volatile float lastCounterFW = 0;

unsigned long elapsedTimeFW;


float FW, FWS, tempFW;
float FWArray[3];
float weightFW[] = {0.1, 0.3, 0.6};

boolean LockCLCFW = false;


uint8_t sensorPinFW = A1;
uint8_t valFW = 0;
uint8_t prev_valFW = 0;

uint16_t sensorValueFW = 0;
uint16_t prev_sensorValueFW = 0;


//GPS Measurement Variable
TinyGPSPlus gps;
TinyGPSCustom RawLat(gps, "GPRMC", 3);
TinyGPSCustom RawLon(gps, "GPRMC", 5);

float lastdda = 0.0;
float lastddo = 0.0;
float weightGPS[2][2] = {{0.4,0.6},{0.2,0.8}};
double gpsArray[4];
double ORgpsArray[4];

//TH Measurement Variable
#define DHTPIN 4   
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

//Speed
double lastSpeed = 0.0;

//Global Variable

boolean classModeLock = false;
boolean joinNetLock = false;
boolean startLock = false;
boolean THLock = false;
boolean RPMLock = false;
boolean GPSLock = false;
boolean FWLock = false;
boolean SPLock = false;
boolean msgLock = false;

boolean SLock01 = false;
boolean SLock02 = false;
boolean SLock03 = false;
boolean SendLock = false;

//Counter
uint16_t DLCount = 0;
uint16_t ULCount[] = {0,0,0,0,0,0} ;

//DataStringArray
double DataArray[6];
char DataString[6][20];
char* DataType[] = {"T:","H:","R:","1a","fa","463a"};

unsigned long lastTime;
uint8_t LCD_flag = 0;
unsigned long workingStart;
unsigned long workingFinish;
String S = "";

void setup() {
  Serial.begin(9600);
  Serial2.begin(115200);
  Serial3.begin(9600);
  lcd.begin();
  //TH
  dht.begin();
  //RPM
  rpmTimerSetting();
  prepareForInterrupts();
  prepareForFW();
  StartCommand();
  if(startLock){
    msgLock = true;
    SLock01 = true;
    SLock02 = false;
    SLock03 = false;
  } 
}

//===============================================================================================================//

//Initialize the LoRa Module, get class first, then join abp network
void StartCommand(){
  uint8_t echoChar;
  String echoString = "";
  Serial2 << "mac" << " " << "get_class";
  delay(10);
  getClass();
  //Join LoRa Network ABP
  if(joinNetLock){
    Serial2 << "mac" << " " << "join" << " " << "abp";
    delay(10);
    for(uint8_t i=0 ; i<10 ; i++){
      if(Serial2.available()>0){
        while(Serial2.available()){
          echoChar = Serial2.read();
          echoString+=char(echoChar);
//          delay(2);
          delayMicroseconds(50);
        }
      }
      else{
        break;
      }
      delay(20);
    }
//    Serial.println(echoString);
    echoString = echoString.substring(5,7);
    if(echoString == "Ok"){
      startLock = true;
      Serial.println("Join ABP Network");
      Serial.flush();
      Serial2.flush();
    }
    else{
      Serial.println("Error");
    } 
  }
}

//===============================================================================================================//

void getClass(){
  uint8_t classChar;
  String classString = "";
  for(uint8_t i=0 ; i<10 ; i++){
    if(Serial2.available()>0){
      while(Serial2.available()){
        classChar = Serial2.read();
        classString+=char(classChar);
//        delay(5);
        delayMicroseconds(50);
      }
    }
    else{
      break;  
    }
    delay(10);
  }
//  Serial.println(classString);
  classString = classString.substring(5,6);
  if(classString=="A"){
    classModeLock = false;
    joinNetLock  = true;
  }else if(classString=="C"){
    classModeLock = true;
    joinNetLock  = true;
  }else{
    Serial2 << "mac" << " " << "get_class";
    delay(10);
    getClass();
    }
  return;
}
//===============================================================================================================//

void sendDT01(){
  workingStart = millis();
  uint16_t DY = 0;
  uint16_t WT = 0;
//  String S = "";
  S = "";
  msgLock = false;
  SLock01 = false;
  SLock02 = false;
  SLock03 = false;
  boolean CK;
  get_THString();
  if(THLock){
    S = String(DataString[0])+"2c"+String(DataString[1]);
    
    workingFinish = millis();
    WT = workingFinish-workingStart;
    if(WT>800){
      DY = 100;
    }else{
      DY = 800-WT;
    }
    
    delay(DY);
    Serial2 << "mac tx cnf 15 " << S;
    
    THLock = false;
    RPMLock = false;
    
  }
  LogULTOEX(0,1,0);
  delay(5);
  CK = getMsg();
  
  LCD_flag = 0;
  LCDPrint();
  

//  Serial.print("WT01:");
//  Serial.print(WT);
//  Serial.print("\t DY01:");
//  Serial.println(DY);
  if(CK){
    delay(100);
    SLock02 = true;
    SLock01 = false;
    SLock03 = false;
  }
  else if(!CK){
    sendDT01();
  }
  
}

void sendDT02(){
  workingStart = millis();
  uint16_t DY = 0;
  uint16_t WT = 0;
  S = "";
//  String S = "";
  String Saa = "";
  String Soo = "";
  msgLock = false;
  SLock01 = false;
  SLock02 = false;
  SLock03 = false;
  boolean CK;

  getRawDataGPS();

  if(GPSLock){
    Saa = String(gpsArray[0]).substring(0,String(gpsArray[0]).length()-3)+String(gpsArray[1]).substring(0,String(gpsArray[1]).length()-3);
    
    Soo = String(gpsArray[2]).substring(0,String(gpsArray[2]).length()-3)+String(gpsArray[3]).substring(0,String(gpsArray[3]).length()-3);
    
    if(gpsArray[0]==0.0){
      S = DataType[3]+Saa+"2c"+DataType[4]+Soo;
    }
    else{
      S = DataType[3]+String(0)+Saa+"2c"+DataType[4]+Soo;
    }
//     Serial.print("S:"); 
//     Serial.println(S);
//     Serial.print("SL:"); 
//     Serial.println(S.length());
     workingFinish = millis();
     WT = workingFinish-workingStart;
  
     if(WT>1500){
       DY = 100;
     }else{
       DY = 1500-WT;
     }
     delay(DY);
     Serial2 << "mac tx cnf 15 " << S; 
     GPSLock = false; 
  }
  
  LogULTOEX(3,4,3);
  delay(5);
  CK = getMsg();
  
  
  LCD_flag = 1;
  LCDPrint();
  
//  Serial.print("WT02:");
//  Serial.print(WT);
//  Serial.print("\t DY02:");
//  Serial.println(DY);
  if(CK){
//    delay(DY);
    delay(100);
    SLock03 = true;
    SLock01 = false;
    SLock02 = false;
  }
  else if(!CK){
    sendDT02();
  }
//  SLock03 = true;
}

void sendDT03(){
  workingStart = millis();
  uint16_t DY = 0;
  uint16_t WT = 0;
  S = "";
//  String S = "";
  msgLock = false;
  SLock01 = false;
  SLock02 = false;
  SLock03 = false;
  boolean CK;
  RPM();
//  FWC();
  Speed();

  if(RPMLock && SPLock){
    S = String(DataString[2])+"2c";
    S+=DataType[5]+String(DataString[5]);
    
//    Serial.print("S:"); 
//    Serial.println(S);
//    Serial.print("SL:"); 
//    Serial.println(S.length());
    workingFinish = millis();
    WT = workingFinish-workingStart;
  
    if(WT>1800){
      DY = 100;
    }else{
      DY = 1800-WT;
    }
    
    delay(DY);
    
    Serial2 << "mac tx cnf 15 " << S; 
    
    RPMLock = false;
    FWLock = false;
    
  }

  LogULTOEX(2,5,0);
  delay(5);
  CK = getMsg();
  
  LCD_flag = 2;
  LCDPrint();

  prepareForInterrupts ();
//  prepareForFW ();
  lastTime = millis();
  SendLock= false;
//  Serial.print("WT03:");
//  Serial.println(WT);

  if(CK){
    delay(100);
    SLock01 = true;
    SLock02 = false;
    SLock03 = false;
  }else if(!CK){
    sendDT03();
  }
//  SLock01 = true;
}

//===============================================================================================================//

boolean getMsg(){
  boolean FG = false;
  uint8_t dataBuffer[64], LoopCount, Len, i, j;
  uint8_t count = 25 ;
  uint8_t txChar;
  uint8_t x = 0;
  String TxAckString = "";
  TxAckString.reserve(64);
  String TxAckSubString = "";
  TxAckSubString.reserve(32);
  Len = Serial2.available();
  

  for(LoopCount = 0 ; LoopCount < count ; LoopCount++){
    if(Len>0){
//      while(Serial2.available()){
        txChar = Serial2.read();
        dataBuffer[x] = txChar;
//        Serial.print(char(dataBuffer[x]));//
        delayMicroseconds(30);
        x++;
//      }
    }
    
    for(i=0; i<Len; i++){
      if((dataBuffer[i]=='m'||dataBuffer[i+1]=='a'||dataBuffer[i+2]=='c')&&(dataBuffer[i+4]=='r'||dataBuffer[i+5]=='x')){
        DLCount++;//Add the Downlink Counter
        for(j=i;j<Len;j++){
          TxAckString+=(char)dataBuffer[j];
          delayMicroseconds(50);
        }
        TxAckSubString = TxAckString.substring(9,TxAckString.length());      
        FG = false;
        Len = 0;
        
//        Serial.println(TxAckSubString);//
//        Serial.print("TxAckSubString LEN:");
//        Serial.println(TxAckSubString.length());
        
      }
//      else if(dataBuffer[i]=='t'|| dataBuffer[i+2]=='_'|| dataBuffer[i+3]=='o'){
      else if(dataBuffer[i]=='t'&& dataBuffer[i+2]=='_'&& dataBuffer[i+3]=='o'){
//        Serial.println("GETMSG");//
//        Serial.print("LoopCount:");
//        Serial.println(LoopCount);
        Len = 0;
        FG = false;
      }
      else if(dataBuffer[i]=='M'&& dataBuffer[i+1]=='A'&& dataBuffer[i+2]=='C'){  
//        Serial.println("ERR");
        delay(800);
//        FG = true;
        FG = false;
        Len = 0;
        
        break;
      }
    }
    if(Len==0){LoopCount = 26;}  
    delay(100);
    
  }

  if(TxAckSubString!=""){
    for(uint8_t k = 0 ; k<TxAckSubString.length() ; k++){
      if(TxAckSubString[k] == '\x0a'|| TxAckSubString[k+1]=='\x0d'){
      TxAckSubString = TxAckSubString.substring(0,k+1);
      TxAckSubString.trim();
      break;
      } 
    }
    String RealData = decodeAsciiData(TxAckSubString);
//      DLTOEX(RealData,DLCount);
//    LogDLTOEX(RealData,DLCount); 
  }
  if(!FG){msgLock = true;}
  else{
    msgLock = false;
    
  }
//  msgLock = true;
  return msgLock;

}


//===============================================================================================================//

String decodeAsciiData(String Data){
  uint8_t dataLen ,dataFormate;
  uint8_t count = 0;
  String rdata = "";
  dataLen = Data.length();
  char temp[dataLen];
//  Serial.print("dataLen:");
//  Serial.println(dataLen);
  for(uint8_t i=0;i<dataLen;i++){
    if(Data[i]>96){                         //check about char from 'a'~'f'
      temp[i] = (Data[i]-96)+9;             //change to real value from 10~15
      }
    else if(Data[i]>47 && Data[i]<58){      //check about char from '0'~'9'
      temp[i] = Data[i]-48;                 //change to real value from 0~9
      }
    else{
      temp[i] = Data[i];
      }
  }
  int a = sizeof(temp);
//  Serial.print("tempLen:");
//  Serial.println(a); 
  for(uint8_t j=0;j<dataLen;j+=2){ 
    dataFormate=(temp[j])*16+temp[j+1];     //combine two value together to HEX Value
    rdata += char(dataFormate);
//    Serial.print(char(dataFormate));
   }
//   Serial.println();
//   Serial.println(rdata);
  return rdata ;
  }

//===============================================================================================================//
void loop() {
  do{
    clcRPM();
    clcFW();
    dataTH();
  }
  while(millis()-lastTime<=2500);
  
  if(millis()-lastTime>2500 && millis()-lastTime <2600){
    LockCLC = true;
    LockCLCFW = true;
    finishTime = (overflowCount << 16) + counter;
    finishTimeFW = millis();
    SendLock= true;
    
  }
  if(SendLock){
    mainWork();
  }
  
}

void mainWork(){
  if(classModeLock){
    if (Serial2.available()) {
        delayMicroseconds(50);
        getMsg();
    }
  }
  else{
    
    if(msgLock){
      if(SLock01){
        delay(100);
        sendDT01();
      }
      else if(SLock02){
        delay(100);
        sendDT02();
      }
      else if(SLock03){
        delay(100);
        sendDT03();
      }
    }
  } 
}
