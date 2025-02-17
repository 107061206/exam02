import matplotlib.pyplot as plt
import numpy as np
import serial
import time

Fs = 100.0;  # sampling rate
Ts = 10.0/Fs; # sampling interval
t = np.arange(0,10,Ts) # time vector; create Fs samples between 0 and 1.0 sec.
x = np.arange(0,10,Ts) # signal vector; create Fs samples
y = np.arange(0,10,Ts) # signal vector; create Fs samples
z = np.arange(0,10,Ts) # signal vector; create Fs samples
fivecm = np.arange(0,10,Ts) # signal vector; create Fs samples

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev,115200)
#line=s.readline() # Read an echo string from K66F terminated with '\n'
for i in range(0, int(Fs)):
    line=s.readline() # Read an echo string from K66F terminated with '\n'
    # print line
    x[i] = float(line)
    line=s.readline() # Read an echo string from K66F terminated with '\n'
    # print line
    y[i] = float(line)
    line=s.readline() # Read an echo string from K66F terminated with '\n'
    # print line
    z[i] = float(line)
    line=s.readline() # Read an echo string from K66F terminated with '\n'
    # print line
    fivecm[i] = float(line)

fig, ax = plt.subplots(2, 1)
ax[0].plot(t,x, color = "blue", label = "x")
ax[0].plot(t,y, color = "red", label = "y")
ax[0].plot(t,z, color = "green", label = "z")
ax[0].legend(loc='lower left')
ax[0].set_xlabel('Time')
ax[0].set_ylabel('Acc Vector')
for i in range(0, 100) :
    ax[1].plot([i/10, i/10], [0, fivecm[i]], color = 'blue',) 
    ax[1].scatter([i/10,], [fivecm[i],], 40, color = 'blue')
ax[1].plot([0, 10], [0, 0], color = "red",)
ax[1].set_xlabel('Time')
ax[1].set_ylabel('>5cm')
plt.show()
s.close()