import time
import minimalmodbus


def reg2string(my_list):
    LSB = my_list[0] & 0xff
    MSB = my_list[0] >> 8
    if len(my_list)==0:
       return ''
    if LSB==0:
        return ''
    if MSB==0:
        return  chr(LSB)
    if len(my_list)==1:
        return  chr(LSB) + chr(MSB)
    return  chr(LSB) + chr(MSB) +  reg2string(my_list[1:])



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
#units = [unit1]


while True:
    # check if sensors are valid
    for unit in units:
        print("\nNode ",unit.address)
        print("\n Pico Unique ID:",hex(unit.read_register(200,0,3)),end="")
        print(hex(unit.read_register(201,0,3))[2:],end="")
        print(hex(unit.read_register(202,0,3))[2:],end="")
        print(hex(unit.read_register(203,0,3))[2:])
        print(" Unit Type Id :", unit.read_register(204,0,3))
        print(" Unit Type String:", reg2string(unit.read_registers(300,16,3)))
        V = unit.read_register(205,0,3)
        print(" Version: {}.{:02d}".format(V>>8,V&255))

        for loop in range(2):
            print("\n  sensor{}".format(loop+1))

            ID=unit.read_register(2200+loop,0,4)
            try:
                print("  Sensor ID : ", sensorID[ID])
            except KeyError:
                pass
            if (ID == 0x58) or (ID==0x60):
                print("  Temperature: {:8.2f}˚C".format(unit.read_long(2210+loop*10,4,signed=True,number_of_registers=2)/100.0))
                if ID == 0x60:
                    print("  Humidity   : {:8.2f}%".format(unit.read_long(2212+loop*10,4,signed=False,number_of_registers=2)/1024.0))
                print("  Pressure   : {:8.2f}hPa".format(unit.read_long(2214+loop*10,4,signed=False,number_of_registers=2)/100.0))
            else:
                print("  Sensor Error")
            print()
#            time.sleep(0.1)
    print()
    time.sleep(3)

