"""
Modul pro statistiky
@project: IPP18 proj2
@brief: Modul pro statistiky
@author: Matej Mitas
@file: ippcode18_stats.py
"""

import os
from ippcode18_error import Error

class Stats():

	"""
	Konstruktor
	@param mod
	@param cesta
	@return None
    """
	def __init__(self, mode, path):
		self.mode = mode
		self.path = path
		self.ctx = {
			'insts': 0,
			'vars': 0
		}

	"""
	Instrukce
	@param cislo
	@return None
    """
	def stats_inc_insts(self, num):
		self.ctx['insts'] = num

	"""
	Promenne
	@param cislo
	@return None
    """
	def stats_inc_vars(self, num):
		self.ctx['vars'] = num

	"""
	Vytiskneme do souboru v poradi
	@return None
    """
	def stats_inc_write_out(self):
		f = open(self.path, 'w+')

		if (self.mode == 3):
			f.write(str(self.ctx.get('vars')) + '\n')
		elif (self.mode == 4):
			f.write(str(self.ctx.get('insts')) + '\n')
		elif (self.mode == 5):
			f.write(str(self.ctx.get('vars')) + '\n')
			f.write(str(self.ctx.get('insts')) + '\n')
		elif (self.mode == 6):			
			f.write(str(self.ctx.get('insts')) + '\n')
			f.write(str(self.ctx.get('vars')) + '\n')


