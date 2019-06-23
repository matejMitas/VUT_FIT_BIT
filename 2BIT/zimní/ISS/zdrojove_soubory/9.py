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
from scipy.signal import lfilter, tf2zpk
from scipy.fftpack import fft
# graphs
import matplotlib.pyplot as plt
# 
from pylab import arange

#
[Fs, samples] = wavread("xmitas02.wav")
#
t = np.arange(-50,51,1) # time vector
z = []

def calc_coef(k):
	temp = 0
	for n in range(0, Fs):
		#
		n_k = n+k
		#
		if (n_k >= 0 and n_k < Fs):
			temp += np.int32(samples[n]) * np.int32(samples[n_k])
		else:
			temp += 0
	return temp * (1.0/Fs)

for i in range(-50, 51):
	z.append(calc_coef(i))

print(z[59])



plt.plot(t, z, 'r')
plt.xlabel('Autokorelacni koeficient')
#print(z)

#
plt.grid(alpha=.25)
plt.show()