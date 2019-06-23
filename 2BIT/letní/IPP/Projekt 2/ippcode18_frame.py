"""
Inicializace a prace s ramci
@project: IPP18 proj2
@brief: Inicializace a prace s ramci
@author: Matej Mitas
@file: ippcode18_frame.py
"""

class Frame:
	
	"""
	Konstruktor
	@param jedna se o globalni ramec?
	@param jedna se o docasny ramec?
	@return None
    """
	def __init__(self, gf = False, tf = False):
		# samotny obsah ramce
		self.ctx = {}
		# pokud se jedna o docasny ramec
		self.gf = gf
		self.tf = tf

	"""
	Zmenime typ ramce z TF na LF
	@return None
    """
	def frame_tf2lf(self):
		self.tf = False

	"""
	Zmenime typ ramce z LF na TF
	@return None
    """
	def frame_lf2tf(self):
		self.tf = True
	
	"""
	Pridany promenne do tabulky symbolu, kontroluje jestli neni promenna
	pridavana vicekat
	@param jmeno promenne
	@return chyba nebo bez chyby
    """
	def frame_add(self, name):
		if (not self.__frame_var_in_frame(name)):
			# vytvor novy symbol
			new_symb = []
			# vypln list prazdny datovym typem a hodnotu
			for i in range(2):
				new_symb.append(None)
			# pridej symbol na jmenne misto
			self.ctx[name] = new_symb
			# v poradku pridano
			return True
		else:
			# redefinice promenne
			return False
	
	"""
	Upravi datovy typ/hodnotu promenne (pokud existuje)
	@param jmeno promenne
	@param novy typ promenne
	@param novy hodnota promenne
	@return None
    """
	def frame_update(self, name, new_type, new_value):
		# ma vubec cenu pridavat?
		if (self.__frame_var_in_frame(name)):
			self.ctx.get(name)[0] = new_type
			"""
			# TODO: tady je to dost zbytecne
			# urci novy datovy typ polozky
			if (new_type == 'int'):
				new_value = int(new_value)
			elif (new_type == 'float'):
				new_value = float(new_value)
			elif (new_type == 'bool'):
				new_value = bool(new_value)"""
			# pridej do polozky
			self.ctx.get(name)[1] = new_value

	"""
	Precte hodnotu promenne (pokud existuje)
	@param jmeno promenne
	@return ziskana promenna
    """
	def frame_read(self, name):
		if (self.__frame_var_in_frame(name)):
			return {
				'type'  : self.ctx.get(name)[0],
				'value' : self.ctx.get(name)[1]
			}
	

	"""
	Vytiskne obsah symbolu v vizualni forme (pokud existuje)
	@param jmeno promenne
	@return None
    """
	def frame_print_symb(self, name):
		symb = self.frame_read(name)
		if (symb != None):
			print("{0}\n  - Typ     : {1}\n  - Hodnota : {2}".format(name, symb['type'], symb['value']))
		else:
			print(">Symbol nenalezen")

	"""
	Vytiskne obsah ramce v vizualni forme
	@return None
    """
	def frame_print(self):
		if (self.gf):
			print("\n---------------------------- @GF ---------------------------")
		elif (self.tf):
			print("\n---------------------------- @TF ---------------------------")


		if (len(self.ctx)):

			for key, value in self.ctx.items():
				print("{0}\n  - {1}".format(key, value))
			# konec tisku
			print("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^")

	"""
	Pomocna funkce pro nalezeni promenne v ramci
	@param jmeno promenne
	@return jmeno promenne
    """
	def __frame_var_in_frame(self, name):
		return name in self.ctx
