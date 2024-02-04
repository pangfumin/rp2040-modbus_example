import time
import minimalmodbus
t = minimalmodbus.Instrument('/dev/ttyUSB0',1)
t.serial.baudrate=115200

while True:
    print("probe1")
    print(" Temperature: {:.2f}˚C".format(t.read_long(2200,4,signed=True,number_of_registers=2)/100.0))
    print(" Humidity   : {:.2f}%".format(t.read_long(2202,4,signed=True,number_of_registers=2)/1024.0))
    print(" Pressure   : {:.2f}hPa".format(t.read_long(2204,4,signed=True,number_of_registers=2)/100.0))
    print("probe2")
    print(" Temperature: {:.2f}˚C".format(t.read_long(2206,4,signed=True,number_of_registers=2)/100.0))
    print(" Humidity   : {:.2f}%".format(t.read_long(2208,4,signed=True,number_of_registers=2)/1024.0))
    print(" Pressure   : {:.2f}hPa".format(t.read_long(2210,4,signed=True,number_of_registers=2)/100.0))
    print()
    time.sleep(10)

