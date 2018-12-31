boolean getRawDataGPS(){
  unsigned long start = millis();
  do 
  {
    while (Serial3.available())
      gps.encode(Serial3.read());
  } while (millis() - start <=1000);
  
  getDataGPS();
  return GPSLock;
}

boolean getDataGPS(){
  double ndda;
  double nddo;

  if(RawLat.isUpdated()){
    //Format => ddmm.mmmm
    uint8_t ida = String(RawLat.value()).indexOf('.');
    //mm
    uint16_t mma = String(RawLat.value()).substring(0,ida-2).toInt();
    //dd.dddd
    double dda = String(RawLat.value()).substring(ida-2,-1).toDouble();
    double diva = lastdda-dda;

    
    ORgpsArray[0] = mma;
    ORgpsArray[1] = (dda*10000000/60.0);

    if(gps.speed.kmph()<1 && diva<0.0006){
      ndda =  (dda*weightGPS[0][1]+ lastdda*weightGPS[0][0]);
    }else{
      ndda =  (dda*weightGPS[1][1]+ lastdda*weightGPS[1][0]);
    }

    double da = (ndda*10000000/60.0);
    gpsArray[0] = mma;
    gpsArray[1] = da;

    
    if(DataArray[3] == 0.0 && mma == 0){
      DataArray[3] = 0.0;
    }
    else{
      DataArray[3] = ((ndda/60.0)+mma);
    }
    
    lastdda = dda;
    
  } 
  else{
    DataArray[3] = DataArray[3];
    
    gpsArray[0] =  gpsArray[0];
    gpsArray[1] =  gpsArray[1];

    ORgpsArray[0] =  ORgpsArray[0];
    ORgpsArray[1] =  ORgpsArray[1];
  }

  if(RawLon.isUpdated()){
    //Format => ddmm.mmmm
    uint8_t ido = String(RawLon.value()).indexOf('.');
    //mm
    uint16_t mmo = String(RawLon.value()).substring(0,ido-2).toInt();
    //dd.dddd
    double ddo = String(RawLon.value()).substring(ido-2,-1).toDouble();
    double divo = lastddo-ddo;

    ORgpsArray[2] = mmo;
    ORgpsArray[3] = (ddo*10000000/60.0);

    if(gps.speed.kmph()<1.5 && divo<0.0006){
      nddo =  (ddo*weightGPS[0][1]+ lastddo*weightGPS[0][0]);
    }else{
      nddo =  (ddo*weightGPS[1][1]+ lastddo*weightGPS[1][0]);
    }

    double doo = (nddo*10000000/60.0);

    gpsArray[2] = mmo;
    gpsArray[3] = doo;

    if(DataArray[4] == 0.0 && mmo == 0){
      DataArray[4] = 0.0;
    }
    else{
      DataArray[4] = ((nddo/60.0)+mmo);
    } 
    
    lastddo = ddo;

  }
  else{
     DataArray[4] = DataArray[4];
     
     gpsArray[2] =  gpsArray[2];
     gpsArray[3] =  gpsArray[3];

     ORgpsArray[2] =  ORgpsArray[2];
     ORgpsArray[3] =  ORgpsArray[3];
    
  }
  
  GPSLock = true;
  return GPSLock;
}
