from ctypes import *
import ctypes
from msvcrt import kbhit

from scipy import signal, fftpack
import numpy

import itertools
import time
import math

recording = False

fft1 = []
fft2 = []
fft3 = []
fft4 = []

ffts = [fft1, fft2, fft3, fft4]


def writedat(filename, x):
    with open(filename,'w') as f:
        for a in x:
            print >> f, a

def process():
    print ("processing")

# code from: http://stackoverflow.com/questions/4688715/find-time-shift-between-two-similar-waveforms
# Results explanation from the stackoverflow answer:
#   The non-negative lags are cxy[0], cxy[1], ..., cxy[len(x)-1], 
#   while the negative lags are cxy[-1], cxy[-2], ..., cxy[-len(y)+1]
# Mike's interpretation:
#   for n = 8 samples per signal:
#       max in position 0-7 = signal 2 leading signal 1
#       max in position 8-15 
#
    M = len(fft1) + len(fft2) - 1
    N = 2 ** int(np.ceil(np.log2(M)))
    X = np.fft.rfft(x, N)
    Y = np.fft.rfft(y, N)
    cxy = np.fft.irfft(X * np.conj(Y))
    cxy = np.hstack((cxy[:len(x)], cxy[N-len(y)+1:])) 
    maxIndex = argmax(correlationArray)
    if (maxIndex < (len(fft1))):
        # Signal 2 is leading Signal 1 by XYZ samples
        print(maxIndex)
    else:
        # Signal 2 is lagging behind Signal 1 by XYZ samples
        print(maxIndex - len(fft1))

    # we're done recording so we need to wait for the end of a frame now
    recording = False
    frameIndex = 0

def sample_handler(data):
    size = len(data)
    print(size)
    print(data)
    '''if data[0] & 0x80 == 0:
        if data[63] == 255 and data[62] == 255 and data[61] == 255:
            int16 = data[2] << 8 | data[3]
            fft1.append(int16)
        else:
            for i in range(2, size/2):
                int16 = data[i*2] << 8 | data[i*2+1]
                fft1.append(int16)
    else:
        if data[63] == 255 and data[62] == 255 and data[61] == 255:
             int16 = data[2] << 8 | data[3]
             fft2.append(int16)
             writedat("log1", fft1)
             writedat("log2", fft2)
             process()
             #now it's the end of a frame
        else:
            for i in range(2, size/2):
                int16 = data[i*2] << 8 | data[i*2+1]
                fft2.append(int16)'''


hid = CDLL('hidDLL')

r = hid.rawhid_open(1, 0x16C0, 0x0486, 0xFFAB, 0x0200)
frameIndex = 0

if(r is not 0):
    print ("got our hid")
else:
    print ("no hid for us")

x = (ctypes.c_char*64)()

while True:
    hid.rawhid_recv(0, ctypes.cast(x, ctypes.POINTER(ctypes.c_char)), 64, 10) #hope we just have one       
    if recording:
        if ord(x[63]) == 255 and ord(x[62]) == 255 and ord(x[61]) == 255 and ord(x[60]) == 255:
            frameIndex+=1 #new frame
        elif ord(x[57]) == 255 and ord(x[58]) == 255 and ord(x[59]) == 255 and ord(x[60]) == 255: #did we get an end of frame?                
            process()

        else:
###############################################################################
# Oh so close... the loop that builds up the FFT needs some work, errors out 
# on #ffts[frameindex]...line being index out of range
        list index out of range on the ffts...append9(int16) line         
            for i in range(2, math.floor(len(x)/2)-1):
                int16 = ord(x[i*2]) << 8 | ord(x[i*2+1])
                ffts[frameIndex].append(int16)
    else:
        if ord(x[63]) == 255 and ord(x[62]) == 255 and ord(x[61]) == 255 and ord(x[60]) == 255: #did we get an end of frame?
            recording = True
            print("recording")

hid.rawhid_close(0)
