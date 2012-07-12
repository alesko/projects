


rounds = [1,2,3,4,5]

weights = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0  ]

# Write the force data to the Arduino
for i in range(len(weights)):
    print "Put the "
    print  weights[i]
    print "and press the button"

	cstr = "E" + str(i)+ ";"
