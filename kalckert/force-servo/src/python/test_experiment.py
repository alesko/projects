#!/usr/bin/python
# Import libs
#from Tkinter import *
import serial
import time
#from datetime import date
from datetime import datetime
from time import strftime
#import numpy as np
import random

# Read force from file and put in array

k = 1141;
g = 9.81;

fpname="forces1.txt"
fp = open(fpname,"r");
A = [];
c = fp.readline()

while c != '':
    v = int(float(c)*k/g);
    A.append(v);
    c = fp.readline()

fp.close()
# Random shuffle the array
#random.shuffle(A)

# Change port name (/dev/ttyXXX) to the appropriate port,
# XXX is the port name on POSIX system (MacOSX, Linux)
ser = serial.Serial('/dev/tty.usbmodemfd3331', 9600, timeout=1)

# Change settings in the Arduino
# Pause duration 1500ms:
ser.write("P1500;")
# Number of repetions
ser.write("N1;")
# Duration of force task, 3000ms
ser.write("D3000;")
# Delay between produced and actuated force: 0 ms
ser.write("L0;")
# Number of force elements
f_len =  len(A);
cstr = "I" + str(f_len) + ";"
ser.write(cstr)

# Write the force data to the Arduino
for i in range(len(A)):
    cstr = "E" + str(i)+ ";"
    ser.write(cstr) 
    print cstr
    cstr = "F"+str(A[i]) + ";"
    ser.write(cstr) 
    print cstr

# Use force array
cstr = "A;"
ser.write(cstr) 


# Set the filename to the curret date and time + .log
fpname = strftime("%Y%m%d-%H%M%S")+'.log'

# Open file for printing
fp = open(fpname,"w")

# Start execution of experiment
cstr = "S;"
ser.write(cstr) 

# Recive force data
if (ser.isOpen()):
    line = ser.readline()
    #print line
    while ( line > -1 ):
        line = ser.readline()
        print line
        fp.write(line)
        line = -1

else:
    print "Error when opening port"


fp.close()
ser.close()             # close port


#dt = datetime.now() - t0
#if (dt.seconds == 0):
#	dtstr = dt.microseconds
#else:
#	dtstr = dt.seconds*1000000+dt.microseconds
#fp.write(str(dtstr))
