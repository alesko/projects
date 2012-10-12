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

# Constants,
k = 1141; # Sensor parameter (from calibration)
g = 9.81; # Earth's gravity, fairly constant...

fpname="forces1.txt";       # Filename with forces
fp = open(fpname,"r"); 
A = [];                     # Create array with zero elements
c = fp.readline();          # Read one line

while c != '':              # Check for an empty line
    v = int(float(c)*k/g);  # Convert force to volt
    A.append(v);            # Put into the array
    c = fp.readline();      # Continue reading one line

fp.close();
# Random shuffle the array
random.shuffle(A)
#---------------------------------------------------------------


# Change port name (/dev/ttyXXX) to the appropriate port,
# XXX is the port name on POSIX system (MacOSX, Linux)
ser = serial.Serial('/dev/tty.usbmodemfd3331', 9600, timeout=1000)

# Run calibration
#ser.write("C;")

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

# Write the force data from the array to the Arduino
for i in range(len(A)):
    cstr = "E" + str(i)+ ";"
    ser.write(cstr) 
    print cstr
    cstr = "F"+str(A[i]) + ";"
    ser.write(cstr) 
    print cstr

# Tell Arduino to use force array
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
    print line
    while ( line > 0 ):
        #line = ser.readline()
        print line
        fp.write(line)
        line = ser.readline()
#else
#    print "Error when opening port"


fp.close()
ser.close()             # close port


#dt = datetime.now() - t0
#if (dt.seconds == 0):
#	dtstr = dt.microseconds
#else:
#	dtstr = dt.seconds*1000000+dt.microseconds
#fp.write(str(dtstr))
