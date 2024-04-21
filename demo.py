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

unit2 = minimalmodbus.Instrument('/dev/ttyUSB0',1)
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


cnt = 0

while True:
    # check if sensors are valid
    for unit in units:
        print("\nNode ",unit.address)
        try:
            # print("\n Pico Unique ID:",hex(unit.read_register(200,0,3)),end="")
            # print(hex(unit.read_register(201,0,3))[2:],end="")
            # print(hex(unit.read_register(202,0,3))[2:],end="")
            # print(hex(unit.read_register(203,0,3))[2:])
            # print(" Unit Type Id :", unit.read_register(204,0,3))
            # print(" Unit Type String:", reg2string(unit.read_registers(300,16,3)))
            # V = unit.read_register(205,0,3)
            # print(" Version: {}.{:02d}".format(V>>8,V&255))

            # sensor_0_data = unit.read_register(1000,0,3)
            # print ("  sensor_0 : ", sensor_0_data)
            # sensor_1_data = unit.read_register(1001,0,3)
            # print ("  sensor_1 : ", sensor_1_data)
            # sensor_2_data = unit.read_register(1002,0,3)
            # print ("  sensor_2 : ", sensor_2_data)


            # write to register
            led_state = not led_state
            unit.write_bit(0,led_state,5)

            unit.write_register(110 + 0, 0, 0, 6)  # led_pattern
            unit.write_register(110 + 1, 50, 0, 6) # delay_us

            led_pattern = unit.read_register(110,0,3)
            delay_us = unit.read_register(111,0,3)
            print("\n led_pattern {} -> {}".format(110, led_pattern))
            print("\n delay_us {} -> {}".format(111, delay_us))

            # # write led register 07
            for shift in range(0,8):
                led_data = 1 << shift
                for i in range(0,8):
                    unit.write_register(1108 + i,led_data,0,6)
            # for i in range(0,5):
            #     panel_input_05 = unit.read_register(i + 1100,0,3)
            #     print("\n panel_input_05 {} -> {}".format(i, panel_input_05))
            #     unit.write_register(1108 + i,panel_input_05,0,6)

        except Exception as error:
            print("Unable to read Modbus Node ",unit.address)
            time.sleep(0.1)
            unit.serial.flush()
    print()
    # time.sleep(0.1)

