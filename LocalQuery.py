import xlsxwriter
import time
import datetime
import serial

t = time.time()
##dt = datetime.datetime.fromtimestamp(t).strftime('%Y-%m-%d %H:%M:%S')
dt = datetime.datetime.fromtimestamp(t).strftime('%Y%m%d_%H%M%S')
##workbook = xlsxwriter.Workbook('/home/pi/Desktop/testData/%s.xlsx'%dt)
workbook = xlsxwriter.Workbook('%s.xlsx'%dt)

worksheet = workbook.add_worksheet()
## ser = serial.Serial('/dev/ttyACM0', 9600,timeout=0.5)
ser = serial.Serial('/dev/ttyUSB0', 9600,timeout=0.5)
## ser = serial.Serial('COM9', 9600,timeout=0.5)

row = [0,0,0]
col  = [0,7,14]

##period = 8*60*60
period = 4*60*60

def GetSerialData():
    global ser, row, col
    global worksheet
    global t

    tempList = []
    
    try:
        SerialData = ser.readline()
        SDS = str(SerialData)
        SD = SerialData.decode('UTF-8','replace').strip('\r\n')

        if len(SerialData)>1:
##            print("RowString=>"+SDS)
##            print("RealString=>"+SD)

            tempList = SD.split(',')
            ListLens = len(tempList)
            print(tempList)
            print("LEN:", ListLens)


            if ListLens == 6:
                if(tempList[0][0:1]=='T'):
                    nowtime = time.time()
                    NT = datetime.datetime.fromtimestamp(nowtime).strftime('%Y-%m-%d %H:%M:%S')
                    worksheet.write(row[0],col[0],NT)
                    for i in range(0,ListLens):
                        worksheet.write(row[0],col[0]+1+i,tempList[i])
       
                    row[0]+=1
                        
                    
                elif(tempList[0][0:2]=='1a'):
                    nowtime = time.time()
                    NT = datetime.datetime.fromtimestamp(nowtime).strftime('%Y-%m-%d %H:%M:%S')
                    worksheet.write(row[1],col[1],NT)
                    for i in range(0,ListLens): 
                        worksheet.write(row[1],col[1]+1+i,tempList[i])

                    row[1]+=1

                elif(tempList[0][0:1]=='R'):
                    nowtime = time.time()
                    NT = datetime.datetime.fromtimestamp(nowtime).strftime('%Y-%m-%d %H:%M:%S')
                    worksheet.write(row[2],col[2],NT)
                    for i in range(0,ListLens): 
                        worksheet.write(row[2],col[2]+1+i,tempList[i])

                    row[2]+=1

    except:
        print("STOP Sampling")
        workbook.close()
        ser.close()
        exit()


            
try:
    while True:
        GetSerialData()
        tt = time.time()
##        print(t)
##        print(tt)

##        if tt>=t+period:
##            workbook.close()
##            ser.close()
##            print("STOP")
##            exit()

except KeyboardInterrupt:
    workbook.close()
    ser.close()
    exit()
    print("Exit")
