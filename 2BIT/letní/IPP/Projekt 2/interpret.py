#!/usr/bin/env python

"""
Hlavni soubor intepretu, spojeni modulu
@project: IPP18 proj2
@brief: Hlavni soubor intepretu, spojeni modulu
@author: Matej Mitas
@file: interpret.py
"""

import sys
import re
# moje importy
from ippcode18_interpret import Interpret

# spust interpret
interpret = Interpret()
interpret.intr_run()