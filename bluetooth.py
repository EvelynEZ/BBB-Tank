# This python program create directory /dev/ttyO1 as UART port for serial writing and reading
# The program also set the baud rate to 115200
# After setting up, the program will print out "Serial is open!" on local terminal 
# and "Hello World!" on the device who tried to communicate with BeagleBone

import Adafruit_BBIO.UART as UART
import serial
 
UART.setup("UART1")
 
ser = serial.Serial(port = "/dev/ttyO1", baudrate=115200)
ser.close()
ser.open()
if ser.isOpen():
	print "Serial is open!"
   	ser.write("Hello World!")
ser.close()

