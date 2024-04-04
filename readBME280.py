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



# unit1 = minimalmodbus.Instrument('/dev/ttyUSB0',1)
# unit1.serial.baudrate=115200
# unit1.serial.flush()
# time.sleep(0.1)

unit2 = minimalmodbus.Instrument('/dev/ttyUSB0',2)
unit2.serial.baudrate=115200


sensorID = { 0x58 : 'BMP280' , 0x60 : 'BME280'}

#address 2200   sensor1 type
#address 2201   sensor2 type
#address 2210..2215   sensor1 temp,humidity,pressure
#address 2210..2225   sensor1 temp,humidity,pressure


# units = [unit1,unit2]
units = [unit2]

led_state = 0

led_data = 0

while True:
    # check if sensors are valid
    for unit in units:
        print("\nNode ",unit.address)
        try:
            print("\n Pico Unique ID:",hex(unit.read_register(200,0,3)),end="")
            print(hex(unit.read_register(201,0,3))[2:],end="")
            print(hex(unit.read_register(202,0,3))[2:],end="")
            print(hex(unit.read_register(203,0,3))[2:])
            print(" Unit Type Id :", unit.read_register(204,0,3))
            print(" Unit Type String:", reg2string(unit.read_registers(300,16,3)))
            V = unit.read_register(205,0,3)
            print(" Version: {}.{:02d}".format(V>>8,V&255))

            sensor_0_data = unit.read_register(1000,0,3)
            print ("  sensor_0 : ", sensor_0_data)
            sensor_1_data = unit.read_register(1001,0,3)
            print ("  sensor_1 : ", sensor_1_data)
            sensor_2_data = unit.read_register(1002,0,3)
            print ("  sensor_2 : ", sensor_2_data)


            # write to register
            led_state = not led_state
            unit.write_bit(0,led_state,5)

            # write led register 07
            if led_data == 0x100:
                led_data = 0
            unit.write_register(1115,led_data,0,6)
            led_data = led_data + 1

            panel_input_05 = unit.read_register(1105,0,3)
            print("\n panel_input_05 {}".format(panel_input_05))

            

            # for loop in range(2):
            #     print("\n  sensor{}".format(loop+1))

            #     ID=unit.read_register(2200+loop,0,4)
            #     try:
            #         print("  Sensor ID : ", sensorID[ID])
            #     except KeyError:
            #         pass
            #     if (ID == 0x58) or (ID==0x60):
            #         print("  Temperature: {:8.2f}˚C".format(unit.read_long(2210+loop*10,4,signed=True,number_of_registers=2)/100.0))
            #         if ID == 0x60:
            #             print("  Humidity   : {:8.2f}%".format(unit.read_long(2212+loop*10,4,signed=False,number_of_registers=2)/1024.0))
            #         print("  Pressure   : {:8.2f}hPa".format(unit.read_long(2214+loop*10,4,signed=False,number_of_registers=2)/100.0))
            #     else:
            #         print("  Sensor Error")
            #     print()
        except Exception as error:
            print("Unable to read Modbus Node ",unit.address)
            time.sleep(0.1)
            unit.serial.flush()
    print()
    # time.sleep(0.1)

