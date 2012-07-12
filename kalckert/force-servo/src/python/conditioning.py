


#To condition a sensor, place 110% of the test weight on the sensor, allow the sensor to stabilize, 
#and then remove the weight. Repeat this process four or five times. The interface between the 
#sensor and the test subject material should be the same during conditioning as during calibration
#and actual testing.

import serial
import time

# Change port name (/dev/tty.usbmodemfd3331) to the appropriate port
ser = serial.Serial('/dev/tty.usbmodemfd3331', 9600, timeout=1)

#time.sleep(10);
ser.flushInput()
sensor_type = 25;  # pounds
ibs2kg = 0.45359237;
max_weight = ibs2kg *sensor_type;
#print "Apply " 
#print max_weight*1.1 
#print "kg"

if (ser.isOpen()):
    ser.write('M;')
    line = ser.readline(10)
    print line

ser.close()             # close port

rounds = [1,2,3,4,5]

for i in rounds:
    print i



