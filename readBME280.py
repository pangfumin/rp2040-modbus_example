import time
import minimalmodbus
unit1 = minimalmodbus.Instrument('/dev/ttyUSB0',1)
unit1.serial.baudrate=115200

unit2 = minimalmodbus.Instrument('/dev/ttyUSB0',2)
unit2.serial.baudrate=115200


sensorID = { 0x58 : 'BMP280' , 0x60 : 'BME280'}

#address 2200   sensor1 type
#address 2201   sensor2 type

#address 2210..2215   sensor1 temp,humidity,pressure
#address 2210..2225   sensor1 temp,humidity,pressure



units = [unit1,unit2]
#units = [unit2]

while True:
    # check if sensors are valid
    for unit in units:
        for loop in range(2):
            print("Node {}  probe {}".format(unit.address,loop+1))
            ID=unit.read_register(2200+loop,0,4)
            try:
                print(" ID : ", sensorID[ID])
            except KeyError:
                pass
            if (ID == 0x58) or (ID==0x60):
                print(" Temperature: {:8.2f}˚C".format(unit.read_long(2210+loop*10,4,signed=True,number_of_registers=2)/100.0))
                if ID == 0x60:
                    print(" Humidity   : {:8.2f}%".format(unit.read_long(2212+loop*10,4,signed=False,number_of_registers=2)/1024.0))
                print(" Pressure   : {:8.2f}hPa".format(unit.read_long(2214+loop*10,4,signed=False,number_of_registers=2)/100.0))
            else:
                print("Sensor Error")
            print()
    print()
    time.sleep(10)

