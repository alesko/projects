

import serial
import time
#from datetime import date
from datetime import datetime
from time import strftime

fpname = strftime("%Y%m%d-%H%M%S")+'.log'

# Change port name (/dev/tty.usbmodemfd3331) to the appropriate port
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)

#print ser.portstr       # check which port was really used
#ser.write("hello")      # write a string

# Open file for printing
fp = open(fpname,"w")

t0 = datetime.now()
if (ser.isOpen()):
	dt = datetime.now() - t0
	if (dt.seconds == 0):
		dtstr = dt.microseconds
	else:
		dtstr = dt.seconds*1000000+dt.microseconds
	fp.write(str(dtstr))
	#ser.write('s')
	#line = ser.readline(6)
	#print line
	#fp.write(str(line))
	#ser.write("I")
	#line = ser.readline(6)
	#fp.write(str(line))
	#ser.write("A")
	#line = ser.readline(6)
	#fp.write(str(line))

	#while ( line != 5 ):
	#	line = ser.readline(3)
	#	fp.write(str(line))
	#	ser.write("S")
else:
	print "Error when opening port"

fp.close()

ser.close()             # close port
