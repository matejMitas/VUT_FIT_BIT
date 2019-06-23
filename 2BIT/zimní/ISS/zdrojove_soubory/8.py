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
from scipy.signal import lfilter, butter
# graphs
import matplotlib.pyplot as plt
# 
from pylab import arange

#
[Fs, samples] = wavread("xmitas02.wav")

#Fs = 150.0;  # sampling rate
Ts = 1.0/Fs; # sampling interval
t = np.arange(0,Fs) # time vector
nyq = 0.5 * Fs # pro filter

# vygeneruj pasmovou propust na 4kHz +- 100Hz
b, a = butter(1, [3900 / nyq, 4100 / nyq], 'bandpass', analog=False)

filtered = lfilter(b, a, samples)

#
plt.plot(t, filtered, 'green', linewidth=.1) # plotting the spectrum
#
plt.xlabel('Frekvence (Hz)')
plt.ylabel('Amplituda')
#
#plt.show()
plt.grid(alpha=.25)
plt.show()