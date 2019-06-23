"""
Typovani podle predpisu, vhodne pro tabulku symbolu atd.
@project: IPP18 proj2
@brief: Typovani podle predpisu, vhodne pro tabulku symbolu atd.
@author: Matej Mitas
@file: ippcode18_retype.py
"""

from ippcode18_error import Error
from collections import namedtuple

class Retype():

	"""
	Konstruktor
	@param typ pretypovani
	@param pole argumentu
	@param ocekavany typ
	@return None
    """
	def __init__(self, mode, *args, to_type = None):
		#
		self.rt_mode = mode
		self.rt_args = []
		#
		for arg in args:
			self.rt_args.append(arg)
		#
		self.rt_call = {
			'int': int,
			'bool': bool,
			'float': float
		}
		#
		self.rt_values = []
		# 
		if (to_type != None):
			self.type = to_type
		else:
			self.type = None

	"""
	Samotne pretypovani
	@return hodnoty
    """
	def rt_exec(self):
		# pouze pretypujeme jeden parametr
		if (self.rt_mode == 'r'):
			# musime dost jenom jeden parametr
			if (len(self.rt_mode) != 1):
				Error(99, 'Parametr -r bere pouze jeden typ pro pretypovani')
			else:
				if (self.type):
					self.__rt_retype(self.type)
				else:	
					self.__rt_retype(self.rt_args[0].get('type'))
				# prekonvertuj a vrat
				return self.rt_values[0]

		elif (self.rt_mode == 'r='):
			# musime dost jenom jeden parametr
			if (len(self.rt_mode) != 2):
				Error(99, 'Parametr -r= potrebuje dva parametry')
			# vybereme si parametry
			s1 = self.rt_args[0]
			s2 = self.rt_args[1]
			# a jejich datovy typy
			t1 = s1.get('type')
			t2 = s2.get('type')

			if (t1 != t2):
				Error(53, 'Nesouhlasne typy operandu {} a {}'.format(t1, t2))
			else:
				# zde je jedno, jaky typ pouzijeme, protoze v tuto
				# chvili jiz vime, ze jsou ekvivalenti
				self.__rt_retype(t1)
				# vytvor novy namedtuple
				Retyped_values = namedtuple('Retyped_values', ['a', 'b'])
				ret = Retyped_values(self.rt_values[0], self.rt_values[1])
				return ret
		else:
			Error(99, 'Neocekavany volba pro pretypovani')

	"""
	Pretypovani podle pole
	@param typ
	@return None
    """
	def __rt_retype(self, type):
		# list navratovych hodnot
		for item in self.rt_args:
			v = item.get('value')
			try:
				# namapujeme do typu a pretypujeme
				self.rt_values.append(self.rt_call[type](v))
			except AttributeError:
				Error(99, 'Na to co chces sakra pretypovat')