
import serial
import time

# Change port name (/dev/ttyXXX) to the appropriate port,
# XXX is the port name on POSIX system (MacOSX, Linux)
ser = serial.Serial('/dev/tty.usbmodemfd3331', 9600)

rounds = [1,2,3,4,5]

#weights = [0.05, 0.10, 0.15, 0.20, 0.25, 0.30, 0.35, 0.40, 0.45, .50  ]

v0_array = [];
v1_array = [];

# Write the force data to the Arduino
#for i in range(len(weights)):
#    print "Put the "
#    print  weights[i]
#    print "and press the button"
#    # Recive force data
#time.sleep(10);

if (ser.isOpen()):
    ser.write('C;')
line = ser.readline()
print line
separator = line.index(' ')
while (separator > 0 ):   
    v0 = int(line[:separator])
    print v0
    v1 = int(line[separator+1:])
    print v1
    v0_array.append(v0)
    v1_array.append(v1)

average0 = float(sum(v0_array)) / len(v0_array)
average1 = float(sum(v0_array)) / len(v0_array)
    
print average0
print average1

# while ( line > -1 ):
#        line = ser.readline()
#        print line
#        fp.write(line)
#        line = -1
#
#	cstr = "E" + str(i)+ ";"
