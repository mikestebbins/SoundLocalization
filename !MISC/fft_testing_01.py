import numpy as np
#import pylab as pl
from scipy.fftpack import fft, ifft
from scipy.signal import correlate

#----INPUTS-------------------------------------------------------------------
sampleRate = 1/20000
numberSamples = 1024

#Sine wave 1 parameters
amplitude1 = 5    # Volts
frequency1 = 300  # hz
phase1 = 0        # 0 -  2*pi

#Sine wave 2 parameters
amplitude2 = 4.5    # Volts
frequency2 = 300  # hz
phase2 = 1        # 0 -  2*pi

#----DO SOME WORK-------------------------------------------------------------
x = np.linspace(0.0, sampleRate*numberSamples, numberSamples)
#print(x)

y1 = amplitude1*(np.sin(2*np.pi*frequency1*x+phase1))
y2 = amplitude2*(np.sin(2*np.pi*frequency2*x+phase2))
#print(y1)

import matplotlib.pyplot as plt
plt.plot(x, y1, x, y2)
plt.show()

xFFT = np.linspace(00.0, 1.0/(2.0*sampleRate), numberSamples/2)
y1FFT = fft(y1)

import matplotlib.pyplot as plt
plt.plot(xFFT, 2.0/numberSamples * np.abs(y1FFT[0:numberSamples/2]))
plt.grid()
plt.xlim(0,1000)
plt.show()

window = np.hanning(numberSamples)
import matplotlib.pyplot as plt
plt.plot(x, window)
plt.show()

y1Window = y1 * window
y2Window = y2 * window
import matplotlib.pyplot as plt
plt.plot(x, y1Window)
plt.show()

yFFTWindow = fft(y1Window)

import matplotlib.pyplot as plt
plt.plot(xFFT, 2.0/numberSamples * np.abs(yFFTWindow[0:numberSamples/2]))
plt.grid()
plt.xlim(0,1000)
plt.show()


# Cross correlation, from: http://dsp.stackexchange.com/questions/736/how-do-i-implement-cross-correlation-to-prove-two-audio-files-are-similar
#a = np.array([1,2,3,4,5,6])
##from: http://docs.scipy.org/doc/numpy/reference/generated/numpy.zeros.html
#b = np.zeros(3,dtype=np.int)  
#c = np.append(a,b)
#print(a)
#print(c)
##from: http://stackoverflow.com/questions/6771428/most-efficient-way-to-reverse-a-numpy-array
#d = np.fliplr([a])[0]   
#print (d)

y1Zeros = np.append(y1Window, np.zeros(1024,dtype=np.int))
y2Zeros = np.append(y2Window, np.zeros(1024,dtype=np.int))
y2ZerosReversed = np.fliplr([y2Zeros])[0]
correlation = ifft(fft(y1Zeros)*fft(y2ZerosReversed))

import matplotlib.pyplot as plt
#plt.plot(xFFT, 2.0/numberSamples * np.abs(correlation[0:numberSamples/2]))
plt.plot(correlation)
plt.grid()
plt.show()

corr = correlate(y1Window,y2Window)
import matplotlib.pyplot as plt
#plt.plot(xFFT, 2.0/numberSamples * np.abs(correlation[0:numberSamples/2]))
plt.plot(corr)
plt.grid()
plt.show()

print (corr)
