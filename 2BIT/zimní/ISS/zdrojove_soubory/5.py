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
from scipy.signal import lfilter, freqz
# graphs
import matplotlib.pyplot as plt
# 
from pylab import arange

#
a = [1, -0.2289, -0.4662]
# nechat
b = [0.2324, -0.4112, 0.2324]

w, h = freqz(b, a, 128)
f = 16000 * w / (2*np.pi)

# rozdeleni na x hodnot z freqz, vynasob x (sample rate / 2)
# angle
# abs
modul = np.abs(h) 
arg = np.angle(h)


#
plt.plot(f, modul, 'y') # plotting the spectrum
#
plt.xlabel('Frekvence [Hz]')
plt.ylabel('$|H(e^{j\omega}$)|')
#
plt.grid(alpha=.25)
plt.show()