#!/bin/python3
import time
import minimalmodbus
from datetime import datetime



#log file  folder

log_path = "sensor_log"


#modbus node definition

ADDR_TYPE = 2200
ADDR_TEMP= 2210
ADDR_HUMIDITY= 2212
ADDR_PRESSURE= 2214
ADDR_STEP = 10





unit1 = minimalmodbus.Instrument('/dev/ttyUSB0',1)
unit1.serial.baudrate=115200

unit2 = minimalmodbus.Instrument('/dev/ttyUSB0',2)
unit2.serial.baudrate=115200


#sensorID = { 0x58 : 'BMP280' , 0x60 : 'BME280'}


units = [ unit1, unit2]



# get time
now = datetime.now()


# create log file using  month/year
logFile = open(log_path + "/" + now.strftime("log_%Y_%m_%d.txt"),"at")


# capture all data
data=[]

for unit in units:

    #possible two sensors per node
    for idx in range(2):
        #get temp,Humidity and pressure
        # first what type of sensor
        T = ""
        H = ""
        P = ""
        try:
            ID = unit.read_register(ADDR_TYPE+idx,0,4)
            #set default value
            if ID == 0x60:
               H = str(unit.read_long(ADDR_HUMIDITY + idx*ADDR_STEP,4,signed=False,number_of_registers=2)/1024.0);
            if (ID == 0x60) or (ID== 0x58):
               T = str(unit.read_long(ADDR_TEMP + idx*ADDR_STEP,4,signed=True,number_of_registers=2)/100.0);
               P = str(unit.read_long(ADDR_PRESSURE + idx*ADDR_STEP,4,signed=False,number_of_registers=2)/100.0);
        except Exception as error:
            pass
        data.append(T)
        data.append(H)
        data.append(P)

# now let's append to the file
#first date like excel accept it
logFile.write(now.strftime('%x %X')+'\t')
for i in data:
    logFile.write(i+"\t")
logFile.write("\n")
logFile.close();


