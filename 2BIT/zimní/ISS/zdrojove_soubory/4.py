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
t = np.arange(0,Fs) # time vector


circle1 = plt.Circle((0, 0), 1, color='black', fill=False, alpha=.5)
#
a = [1, -0.2289, -0.4662]
# nechat
b = [0.2324, -0.4112, 0.2324]

y = lfilter(b, a, samples)

#
zeros, poles, gain = tf2zpk(b, a)
#
print(zeros)

X = [x.real for x in zeros]
Y = [x.imag for x in zeros]

X1 = [x.real for x in poles]
Y1 = [x.imag for x in poles]

print(X)
print(Y)

#
#plt.plot(t, y, 'm') # vzorky
plt.plot(X, Y,'ro', X1, Y1, 'bx')
# add to plot
plt.gcf().gca().add_artist(circle1)
#
plt.xlabel('Re')
plt.ylabel('Im')

plt.xlim(-1.5,1.5)
plt.ylim(-1.5,1.5)
plt.grid(alpha=.25)

#
plt.show()