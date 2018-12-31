import xlsxwriter
import json
import paho.mqtt.client as mqtt
import time
import datetime
import requests


t = time.time()
dt = datetime.datetime.fromtimestamp(t).strftime('%Y%m%d_%H%M%S')
workbook = xlsxwriter.Workbook('%s.xlsx'%dt)
worksheet = workbook.add_worksheet("ACDATA")


##DataDic = {"Temperature":"","Humidity":"","RotatingSpeed":"","Latitude":"","Longitude":"","FlowRate":""}
DataDic = {"Temperature":"","Humidity":"","RotatingSpeed":"","Latitude":"","Longitude":"","Speed":""}
row = [1,1,1]
col  = [0,5,10]
Counter = [0,0,0]


deviceKey = 'Mediatek MCS Device Key '
headers = {'deviceKey':deviceKey,'Content-Type':'application/json'}
URLDATA = 'mediatek mcs endpoint URL'

##dataChnId = ["Temperature","Humidity","RotatingSpeed","GPS","FlowRate","RSSI"]
dataChnId = ["Temperature","Humidity","RotatingSpeed","GPS","Speed","RSSI"]

##dataChnIdT = "Temperature"
##dataChnIdH = "Humidity"
##dataChnIdG = "GPS"
##dataChnIdR = "RotatingSpeed"
##dataChnIdF = "FlowRate"
##dataChnIdRSSI = "RSSI"



Connected = False
client = mqtt.Client()

##-----------------------------------------------------------------------------------------------------------------------------------------##
def CreateSheet():
    global worksheet
    bold = workbook.add_format({'bold':True})
    DataListTopic =[['A1','UpTime'],['B1','RSSI'],['C1','CounterA'],['D1','TS'],['E1','HS'],
                    ['F1','UpTime'],['G1','RSSI'],['H1','CounterB'],['I1','LA'],['J1','LO'],
                    ['K1','UpTime'],['L1','RSSI'],['M1','CounterC'],['N1','RS'],['O1','FS']]

    for a,b in DataListTopic:
        worksheet.write(a,b,bold)

##-----------------------------------------------------------------------------------------------------------------------------------------##

def on_connect(client, userdata, flags, rc):
    if rc==0:
        print("Connection OK")
        global Connected                #Use global variable
        Connected = True                #Signal connection 
    else:
        print("Not Connect")     
    print("Connected with result code "+str(rc))

def on_message(client, userdata, msg):
    Msg= str(msg.payload.decode("utf-8"))
##    print(Msg)
    parserData(Msg)


##-----------------------------------------------------------------------------------------------------------------------------------------##

def parserData(MSG):
    global DataDic

    ParserString = ''
    Time = ''
    Rssi = ''
    Data = ''
    RawData = ''

    ParserString = json.loads(MSG)

    try:
        Time = ParserString[0]['time']
        Rssi = ParserString[0]['rssi']
        Data = ParserString[0]['data']
    except:
        print('Data Error')

    Time = Time.replace('T',' ')
    Time = datetime.datetime.strptime(Time,'%Y-%m-%d %H:%M:%S')+datetime.timedelta(hours = 8)
    Time = Time.strftime('%Y-%m-%d %H:%M:%S')
    print(Data)

    if(Data[0:1]!='1'):
        if(Data[0:2]=='54'):
            RowData = LoRaDataDecode(Data)
            DataA = RowData.split(',')
            DataListA = [Time,Rssi,DataA]
##            print(RowData)
##            print(DataListA)
            parserA(DataListA)
        elif(Data[0:2]=='52'):
            RowData = LoRaDataDecode(Data)
            DataC = RowData.split(',')
            DataListC = [Time,Rssi,DataC]
##            print(RowData)
##            print(DataListC)
            parserC(DataListC)
        
    else:
        
        DataB = Data.split('2c')
        DataListB = [Time,Rssi,DataB]
##        print(Data)
##        print(DataListB)
        parserB(DataListB)

##-----------------------------------------------------------------------------------------------------------------------------------------##
##DataDic = {"Temperature":"","Humidity":"","RotatingSpeed":"","Latitude":"","Longitude":"","FlowRate":""}
def parserA(ListA):
    global DataDic,Counter
    TS = ''
    HS = ''
    
    if ListA[2][0][0:1]=='T':
        TS = float(ListA[2][0][2:])/10.0
        DataDic["Temperature"] = TS
    else:
        DataDic["Temperature"] = DataDic["Temperature"]
        

    if ListA[2][1][0:1]=='H':
        HS = float(ListA[2][1][2:])/10.0
        DataDic["Humidity"] = HS
    else:
        DataDic["Humidity"] = DataDic["Humidity"]
        

    Counter[0]+=1
    DLA = [ListA[0],ListA[1],Counter[0],TS,HS]
    print(DLA)
    toExcel(0,DLA)
    UploadToMCS(0, DLA)
    
    
    
def parserB(ListB):
    global DataDic,Counter
    LA = ''
    LO = ''
    FR = ''
##    LS = ''
    if ListB[2][0][0:2]=='1a':
        LA = float(ListB[2][0][3:])/10000000.0
        DataDic["Latitude"] = LA
    else:
        DataDic["Latitude"] = DataDic["Latitude"]
    if ListB[2][1][0:2]=='fa':
        LO = float(ListB[2][1][2:])/10000000.0
        DataDic["Longitude"] = LO
    else:
        DataDic["Longitude"] = DataDic["Longitude"]


    Counter[1]+=1

    DLB = [ListB[0],ListB[1],Counter[1],LA,LO]
    print(DLB)
    toExcel(1,DLB)
    UploadToMCS(1, DLB)


def parserC(ListC):
    global DataDic,Counter
    FS = ''
    RS = ''

    if ListC[2][0][0:1] =='R':
        RS = float(ListC[2][0][2:])
        DataDic["RotatingSpeed"] = RS
    else:
        DataDic["RotatingSpeed"] = DataDic["RotatingSpeed"]
        

    if ListC[2][1][0:1] =='F':
        FS = float(ListC[2][1][2:])/100.0
        DataDic["Speed"] = FS
    else:
        DataDic["Speed"] = DataDic["Speed"]
        

    Counter[2]+=1
    DLC = [ListC[0],ListC[1],Counter[2],RS,FS]
    print(DLC)
    toExcel(2,DLC)
    UploadToMCS(2, DLC)


    

##-----------------------------------------------------------------------------------------------------------------------------------------##

def toExcel(ID,DTLS):
    global worksheet
    global row, col
    
    if ID == 0:
        for i in range(0,len(DTLS)):
            worksheet.write(row[0],col[0]+i,DTLS[i])
        row[0]+=1
        
    elif ID == 1:
        for i in range(0,len(DTLS)):
            worksheet.write(row[1],col[1]+i,DTLS[i])
        row[1]+=1
        
    elif ID == 2:
        for i in range(0,len(DTLS)):
            worksheet.write(row[2],col[2]+i,DTLS[i])
        row[2]+=1
    

##-----------------------------------------------------------------------------------------------------------------------------------------##
##  Decode Data 
def LoRaDataDecode(decodeData):
    tempList = []
    DecodeDataString = ''
    for j in range(0,len(decodeData),2):
        tp01 = ord(decodeData[j:j+1])
        tp02 = ord(decodeData[j+1:j+2])
        tempList = [tp01,tp02]
        #print(tp01,tp02)
        for k in range(0,len(tempList)):
            if(tempList[k]>96):
                tempList[k] = (tempList[k]-96)+9
                #print( tempList[k])
            elif(tempList[k]>47 and tempList[k]<58):
                tempList[k] = tempList[k]-48
                #print( tempList[k])
            else:
                tempList[k]=tempList[k]
                #print( tempList[k])

        tempChar = tempList[0]*16+tempList[1]
        #print(tempChar)
        #print(chr(tempChar))
        DecodeDataString+=chr(tempChar)

    return DecodeDataString




##-----------------------------------------------------------------------------------------------------------------------------------------##
    
def UploadToMCS(ID, UDLS):
    global headers, URLDATA
##    global dataChnIdT, dataChnIdH, dataChnIdG, dataChnIdR,dataChnIdF,dataChnIdRSSI
##    dataChnId = ["Temperature","Humidity","RotatingSpeed","GPS","FlowRate","RSSI"]
    global dataChnId
    Payload = {}

    if ID == 0:
        Payload = {
            "datapoints":[
                {
                    "dataChnId":dataChnId[0],
                    "values":{
                        "value":UDLS[3]
                        }
                },
                {
                    "dataChnId":dataChnId[1],
                    "values":{
                        "value":UDLS[4]
                        }
                },
                {
                    "dataChnId":dataChnId[5],
                    "values":{
                        "value":UDLS[1]
                        }
                }
            ]
        }
        
    elif ID == 1:
        Payload = {
            "datapoints":[
                {
                    "dataChnId":dataChnId[3],
                    "values":{
                        "latitude":UDLS[3],
                        "longitude":UDLS[4],
                        "altitude":"59"
                        }
                },
                {
                    "dataChnId":dataChnId[5],
                    "values":{
                        "value":UDLS[1]
                        }
                }
            ]
        }
    elif ID == 2:
        Payload = {
            "datapoints":[
                {
                    "dataChnId":dataChnId[2],
                    "values":{
                        "value":UDLS[3]
                        }
                },
                {
                    "dataChnId":dataChnId[4],
                    "values":{
                        "value":UDLS[4]
                        }
                },
                {
                    "dataChnId":dataChnId[5],
                    "values":{
                        "value":UDLS[1]
                        }
                }
            ]
        }
        
    json_payload = json.dumps(Payload)
    r = requests.post(URLDATA, data = json_payload, headers = headers)
##    print(json_payload)
    print(r.status_code)
    
    



##-----------------------------------------------------------------------------------------------------------------------------------------##
 


    

##-----------------------------------------------------------------------------------------------------------------------------------------##

def MainWork():
    global client
    ##Basic info
    ##Broker ="iot.eclipse.org"
    Broker = "Broker IP"
    port = "PORT Number"
    user = "Broker User Name"
    password = "Broker User PWD"
    ##From node to GW Data
    subscribeTopic = "LoRa Gateway Publish Topic"
	
    ##Authentication from LoRa Gateway
    client.username_pw_set(user, password=password)
    print("Connecting to  broker..."+Broker)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(Broker,port,60)
    client.subscribe(subscribeTopic)
    CreateSheet()
    client.loop_start()


if __name__ == '__main__':
    MainWork()
##    FirestoreDB()

try:
    while True:
        if(Connected):
            try:
                Now_Time = time.time()
            except:
                print("STOP Sampling")
                workbook.close()
                client.disconnect()
                client.loop_stop()
                exit()
            
except KeyboardInterrupt:
    print("KeyboardInterrupt")
    workbook.close()
    client.disconnect()
    client.loop_stop()
    exit()
    
