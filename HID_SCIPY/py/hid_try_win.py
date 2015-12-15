from ctypes import *
import ctypes
import time
from msvcrt import kbhit
from scipy.signal import butter, lfilter
from scipy.fftpack import fft, ifft
from scipy import signal, fftpack
import numpy as np

import matplotlib.pyplot as plt

# USER-DEFINED PARAMETERS------------------------------------------------------------------------------
sampleRate = 100000      # (Hz)
movingAverage = True
movingAverageWindow = 50
signalBandpass = False
lowcut = 80.0          # (Hz), low cutoff frequency for bandpass filter
highcut = 5000.0        # (Hz), high cutoff frequency for bandpass filter
filterOrder = 3
signalEnvelope = True
signalOnset = True
DCoffsetRemoved = True
signalNormalize = True
signalUnitized = True
micDistance = 168       # (mm), straight line distance between opposing sides microphones
speedOfSound = 344      # (meters/second), 70 degrees, http://www.engineeringtoolbox.com/air-speed-sound-d_603.html
global Beta     
Beta = 0.999           # determined experimentally, creates decay rate for enveloping of signals
FFTsize = 1024          # can't change here without changing a lot in Teensy and this code
displayGraphs = True
displayAllGraphs = False  # really only used in debugging signal processing/algorithms

# VARIABLE DEFINITION----------------------------------------------------------------------------------
recording = False
fft1 = []
fft2 = []
fft3 = []
fft4 = []
ffts = [fft1, fft2, fft3, fft4]

# FUNCTIONS-------------------------------------------------------------------------------------------
def writedat(filename, x):
    with open(filename,'w') as f:
        for a in x:
            print >> f, a
            
def butter_bandpass(lowcut, highcut, fs, order=5):
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype='band')
    return b, a

def butter_bandpass_filter(data, lowcut, highcut, fs, order):
    b, a = butter_bandpass(lowcut, highcut, fs, order=order)
    y = lfilter(b, a, data)
    return y

def moving_average(a, n) :
    ret = np.cumsum(a, dtype=float)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1:] / n

def signal_envelope(inputSignal): 
#    outputSignal = np.zeros((length,), dtype=np.int)  #.int casting causing issues
    outputSignal = np.zeros(len(inputSignal))
    for i in range (1,len(inputSignal)):
        temp = max(abs(Beta*outputSignal[i-1]),abs(inputSignal[i]))
        outputSignal[i] = temp
    return outputSignal
    
def signal_onset(inputSignal):
    #outputSignal = np.zeros((length,), dtype=np.int)    # int cast causing issues
    outputSignal = np.zeros(len(inputSignal))
    for i in range (1,len(inputSignal)):
        if (inputSignal[i] >= inputSignal[i-1]):
            outputSignal[i] = inputSignal[i];
        else:
            outputSignal[i] = 0.0
    return outputSignal
    
def DC_offset_removed(inputSignal):
    mean = np.mean(inputSignal)    
    inputSignal[:] = [x - mean for x in inputSignal]
    return inputSignal
    
def signal_normalized(inputSignal):
    outputSignal = inputSignal/(np.max(inputSignal)/1.0)
    return outputSignal
    
def signal_unitized(inputSignal):
    outputSignal = np.zeros(len(inputSignal))
    for i in range (0,len(inputSignal)):
        outputSignal[i] = (inputSignal[i]-min(inputSignal))/(max(inputSignal)-min(inputSignal))
    return outputSignal

def signal_processing(dataArray):
    if(signalBandpass == True):
        dataArray = butter_bandpass_filter(dataArray,lowcut,highcut,sampleRate,filterOrder)
        
        if displayAllGraphs == True:
            plt.plot(dataArray)
            plt.grid()
            plt.title("Processed Signal")
            plt.show()
    
    if (movingAverage == True):
        dataArray = moving_average(dataArray,movingAverageWindow)  
        
        if displayAllGraphs == True:
            plt.plot(dataArray)
            plt.grid()
            plt.title("Processed Signal")
            plt.show()
    
    if (signalNormalize == True):
        dataArray = signal_normalized(dataArray)
        
        if displayAllGraphs == True:
            plt.plot(dataArray)
            plt.grid()
            plt.title("Processed Signal")
            plt.show()
        
    if (DCoffsetRemoved == True):
        dataArray = DC_offset_removed(dataArray)
        
        if displayAllGraphs == True:
            plt.plot(dataArray)
            plt.grid()
            plt.title("Processed Signal")
            plt.show()

    if (signalEnvelope == True):
        dataArray = signal_envelope(dataArray)
        
        if displayAllGraphs == True:
            plt.plot(dataArray)
            plt.grid()
            plt.title("Processed Signal")
            plt.show()
    
    if (signalUnitized == True):
        dataArray = signal_unitized(dataArray)
        
        if displayAllGraphs == True:
            plt.plot(dataArray)
            plt.grid()
            plt.title("Processed Signal")
            plt.show()

    if (signalOnset == True):
        dataArray = signal_onset(dataArray)
                
        if displayAllGraphs == True:
            plt.plot(dataArray)
            plt.grid()
            plt.title("Processed Signal")
            plt.show()
        
    return dataArray

#from: http://stackoverflow.com/questions/4688715/find-time-shift-between-two-similar-waveforms
def rfft_xcorr(x, y):
    M = len(x) + len(y) - 1
    N = 2 ** int(np.ceil(np.log2(M)))
    X = np.fft.rfft(x, N)
    Y = np.fft.rfft(y, N)
    cxy = np.fft.irfft(X * np.conj(Y))
    cxy = np.hstack((cxy[:len(x)], cxy[N-len(y)+1:]))
#    print(cxy.astype(np.int64))
    
    #if array returned has maximum value at index X: means Y
    # 0: two signals have no offset
    # 1, 2, 3... up to the lenght of the first signal -1: signal 2 leads signal 1
    # -1, -2, -3... up to the length of the first signal: signal 2 lags signal 1 

    return cxy

def phase_offset(cxyArray):
    cxyFirstHalf = cxyArray[1:FFTsize]
    cxySecondHalf = cxyArray[FFTsize:]
    
    cxyFirstHalfmax = max(cxyFirstHalf)
    cxySecondHalfmax = max(cxySecondHalf)
    
    if displayAllGraphs == True: 
        # plot the correlation of the onset signal
        plt.plot(cxyArray,label="y1-y2",color="r")
        plt.grid()
        plt.title("Cross-correlation")
        plt.xlim(0,200)
        plt.show()
        plt.plot(cxyArray,label="y1-y2",color="r")
        plt.grid()
        plt.title("Cross-correlation")
        plt.xlim(len(cxyArray)-200,len(cxyArray))
        plt.show()
    
    if np.argmax(cxyArray) == 0:
        return 0
    else:
        if cxyFirstHalfmax > cxySecondHalfmax:  
            # second signal leading, result is negative
            return (-np.argmax(cxyFirstHalf)+1)
        elif cxyFirstHalfmax < cxySecondHalfmax:  
            # first signal leading, result is positive
            return (np.argmax(cxySecondHalf[::-1])+1)
        else:
            # if max value is the same in each half of the Cxy array, throw your hands up and try again
            return (999999)

def process():
    tic1 = time.time()
    
    if displayGraphs == True:
        # plot raw signals
        plt.plot(fft1,label="micA")
        plt.plot(fft2,label="micE")   
        plt.grid()
        plt.legend()
        plt.title("Raw Signals A&E")
        plt.show()
   
    
    # perform any signal smoothing, enveloping, and onset-detection that is specified
    y1 = signal_processing(fft1)
    y2 = signal_processing(fft2)   

    if displayGraphs == True:    
        # plot filtered signals    
        plt.plot(y1,label="micA")
        plt.plot(y2,label="micE")   
        plt.grid()
        plt.legend()
        plt.title("Processed Signals A&E")
        plt.ylim(0,1.5)
        plt.show() 
    
    if displayAllGraphs == True:  
        #plot FFT of raw signals
        plt.plot(abs(fft(fft1)),label="micA")
        plt.plot(abs(fft(fft2)),label="micE")
        plt.grid()
        plt.title("FFT of signals, pre-filter")
        plt.legend()
        plt.xlim(0,200)
        plt.ylim(0,10000)
        plt.show()
    
    y1FFT = fft(fft1)
    y2FFT = fft(fft2) 
     
    if displayAllGraphs == True:   
        #plot FFT of filtered signals
        y1FFT = fft(y1)
        y2FFT = fft(y2) 
        plt.plot(abs(y1FFT),label="micA")    
        plt.plot(abs(y2FFT),label="micE")
        plt.grid()
        plt.title("FFT of signals, post-filter")
        plt.legend()
        plt.xlim(0,200)
        plt.ylim(0,10)
        plt.show() 

    # calc phgase offset, number of samples, and which is leading
    cxy1 = rfft_xcorr(y1,y2)
    firstPairOffset = phase_offset(cxy1)
    
   #########################################################################   
   # repeat whole shebang for the second pari of signals       
    if displayGraphs == True:
       # plot raw signals
        plt.plot(fft3,label="micC")
        plt.plot(fft4,label="micG")   
        plt.grid()
        plt.legend()
        plt.title("Raw Signals C&G")
        plt.show()    
    
    y3 = signal_processing(fft3)
    y4 = signal_processing(fft4)    
    
    if displayGraphs == True:     
        # plot filtered signals    
        plt.plot(y3,label="micC")
        plt.plot(y4,label="micG")   
        plt.grid()
        plt.legend()
        plt.title("Processed Signals C&G")
        plt.ylim(0,1.5)
        plt.show() 

    # calc phgase offset, number of samples, and which is leading
    cxy2 = rfft_xcorr(y3,y4)
    secondPairOffset = phase_offset(cxy2)
    
    print("1st pair offset = ",firstPairOffset)
    print("2nd pair offset = ",secondPairOffset)
    
    ### Mike's notes to himself
    ### must come back here and double-check the angle calculations against Teensy output, python processing, ppt
    ### diagram, and the actual microphone board
    firstPairAngle = np.degrees(np.arcsin((speedOfSound*firstPairOffset*(1/sampleRate))/micDistance))
    #print(firstPairAngle)
    secondPairAngle = np.degrees(np.arcsin((speedOfSound*secondPairOffset*(1/sampleRate))/micDistance))
    #print(secondPairAngle)  
    
    global recording
    recording = False
    global frameIndex
    frameIndex = 0
    del fft1[:]
    del fft2[:]
    del fft3[:]
    del fft4[:]
    
    toc1 = time.time()    
    print("Process signals, elapsed time = ",toc1-tic1,"sec")

# MAIN -----------------------------------------------------------------------------------------------
hid = CDLL('hidDLL')

r = hid.rawhid_open(1, 0x16C0, 0x0486, 0xFFAB, 0x0200)
frameIndex = 0

if(r is not 0):
    print ("got our hid")
else:
    print ("no hid for us")

buffer = (ctypes.c_char*64)()

while True:
    hidres = hid.rawhid_recv(0, ctypes.cast(buffer, ctypes.POINTER(ctypes.c_char)), 64, 10) #hope we just have one
    if(hidres < 1): #hid error, no process
        continue

    #print("length of x = ",len(x))
    if recording:
        
        # One set of 4 frames of data has occurred
        if (ord(buffer[56]) == 255 and ord(buffer[57]) == 255 and ord(buffer[58]) == 255 and ord(buffer[59]) == 255 and ord(buffer[60]) == 255 and ord(buffer[61]) == 255 and ord(buffer[62]) == 255 and ord(buffer[63]) == 255): #did we get an end of frame?
            #print("length of fft 1, 2, 3, 4 = ",len(fft1),",",len(fft2),",",len(fft3),",",len(fft4))
            toc = time.time()
            print("HID read and fill buffers, elapsed time = ",toc-tic,"sec")            
            process()
        
        # One frame of data has occurred (1 frame = 2048 bytes = 1024 samples @ 2 bytes each) 
        elif ord(buffer[60]) == 255 and ord(buffer[61]) == 255 and ord(buffer[62]) == 255 and ord(buffer[63]) == 255:
           # print(" change frame ", frameIndex, " ", len(ffts[frameIndex]))
            frameIndex+=1 #new frame
            if(frameIndex > 3):
                frameIndex = 0

        else:
            #print(" adding to fft ", frameIndex, " ", len(ffts[frameIndex]))
            bufferMax = 32 #int(math.floor(len(x)/2))-1
            for i in range(0, bufferMax):
                int16 = ord(buffer[i*2]) << 8 | ord(buffer[i*2+1])
                ffts[frameIndex].append(int16)
                
        #time.sleep(0.2)
    
    # Wait until the end of four full frames has gone by, then start recording
    else:
        if (ord(buffer[56]) == 255 and ord(buffer[57]) == 255 and ord(buffer[58]) == 255 and ord(buffer[59]) == 255 and ord(buffer[60]) == 255 and ord(buffer[61]) == 255 and ord(buffer[62]) == 255 and ord(buffer[63]) == 255): #did we get an end of frame?
            frameIndex = 0            
            recording = True
            tic = time.time()
            #print("recording")

hid.rawhid_close(0)
