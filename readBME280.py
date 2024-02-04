import time
import minimalmodbus
unit1 = minimalmodbus.Instrument('/dev/ttyUSB0',1)
unit1.serial.baudrate=115200


units = [unit1]


while True:
    # check if sensors are valid
    for unit in units: 
        bmeValid = unit.read_register(2200,0,4)
        for loop in range(2):
            print("Node {}  probe {}".format(unit.address,loop+1))
            if  bmeValid & (1<<loop):
                print(" Temperature: {:.2f}˚C".format(unit.read_long(2201+loop*6,4,signed=True,number_of_registers=2)/100.0))
                print(" Humidity   : {:.2f}%".format(unit.read_long(2203+loop*6,4,signed=True,number_of_registers=2)/1024.0))
                print(" Pressure   : {:.2f}hPa".format(unit.read_long(2205+loop*6,4,signed=True,number_of_registers=2)/100.0))
            else:
                print("Sensor Error")
            print()
    print()
    time.sleep(10)

