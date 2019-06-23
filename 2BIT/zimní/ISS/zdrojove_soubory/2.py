#!/usr/bin/python3
# coding=utf-8

'''
Project 'Signal and Systems'
Copyright (C) 2017 Brno University of Technology,
                   Faculty of Information Technology
Author(s): Matej Mitas, xmitas02
'''

# imports
# numpy
import numpy as np
# imports of scipy 
from scipy.io.wavfile import read as wavread
from scipy.fftpack import fft
# graphs
import matplotlib.pyplot as plt
# 
from pylab import arange

#
[Fs, samples] = wavread("xmitas02.wav")

#Fs = 150.0;  # sampling rate
Ts = 1.0/Fs; # sampling interval
t = np.arange(0,Fs/2) # time vector

y = samples


n = len(samples) # length of the signal

Y = np.fft.fft(y) # fft computing and normalization
Y = Y[range(n/2)]
Y = abs(Y)

#maximalni hodnota
print(np.argmax(Y))

#
plt.plot(t, Y, 'r', linewidth=.25) # plotting the spectrum
#
plt.xlabel('Frekvence [Hz]')
plt.ylabel('Amplituda')
#
plt.grid(alpha=.25)
plt.show()
#plt.show()