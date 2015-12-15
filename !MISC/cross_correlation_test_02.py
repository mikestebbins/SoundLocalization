import numpy as np
#import pylab as pl
from scipy.fftpack import fft, ifft
from scipy.signal import correlate

x = np.arange(0,8)
x = np.array(x)

## signal 1 trails behind signal 2
#y1 = np.array([  1,  1,  1,  1,  1,  1,500,250])
#y2 = np.array([  1,  1,  1,500,250,  1,  1,  1])

# signal 2 lags behind signal 1
y1 = np.array([1,500,250,1,1,1,1,1,])
y2 = np.array([1,1,1,500,250,1,1,1])

import matplotlib.pyplot as plt
plt.grid()
plt.plot(x, y1,label="y1")
plt.plot(x, y2,label="y2")
plt.title("signal2 lags behind signal1")
plt.legend()
plt.show()

#corr1 = correlate(y1,y2,'same')
#corr2 = correlate(y2,y1,'same')
#print("corr1 = ")
#print(corr1)
#print("corr2 = ")
#print(corr2)
#
#import matplotlib.pyplot as plt
#plt.plot(corr1,label="y1-y2")
#plt.plot(corr2,label="y2-y1")
#plt.grid()
#plt.legend()
#plt.show()

# Utilize the FFT method
A = fft(y1)
B = fft(y2)
Ar = -A.conjugate()
Br = -B.conjugate()
corr1 = np.abs(ifft(Ar*B))
corr1max = np.argmax(corr1)
print(corr1max)

corr2 = np.abs(ifft(A*Br))
corr2max = np.argmax(corr2)
print(corr2max)

import matplotlib.pyplot as plt
plt.plot(corr1,label="y1-y2")
plt.plot(corr2,label="y2-y1")
plt.grid()
plt.legend()
plt.show()

xZeros = np.array(np.arange(0,16))
y1Zeros = np.append(y1, np.zeros(8,dtype=np.int))
y2Zeros = np.append(y2, np.zeros(8,dtype=np.int))

import matplotlib.pyplot as plt
plt.grid()
plt.plot(xZeros, y1Zeros,label="y1")
plt.plot(xZeros, y2Zeros,label="y2")
plt.legend()
plt.show()

A = fft(y1Zeros)
B = fft(y2Zeros)
Ar = -A.conjugate()
Br = -B.conjugate()
corr1 = np.abs(ifft(Ar*B))
corr1max = np.argmax(corr1)
print(corr1max)

corr2 = np.abs(ifft(A*Br))
corr2max = np.argmax(corr2)
print(corr2max)

import matplotlib.pyplot as plt
plt.plot(corr1,label="y1-y2")
plt.plot(corr2,label="y2-y1")
plt.grid()
plt.legend()
plt.show()

def rfft_xcorr(x, y):
    M = len(x) + len(y) - 1
    N = 2 ** int(np.ceil(np.log2(M)))
    X = np.fft.rfft(x, N)
    Y = np.fft.rfft(y, N)
    cxy = np.fft.irfft(X * np.conj(Y))
    cxy = np.hstack((cxy[:len(x)], cxy[N-len(y)+1:]))
    return cxy

import matplotlib.pyplot as plt
plt.plot(rfft_xcorr(y1,y2),label="y1-y2")
plt.grid()
plt.legend()
plt.show()
    
