"""
Implementace symbolu
@project: IPP18 proj2
@brief: Implementace symbolu
@author: Matej Mitas
@file: ippcode18_symbol.py
"""

from ippcode18_error import Error

class Symbol:

	"""
	Konstruktor
	@param symbol k pridani
	@param do jakeho ramce
	@return None
    """
	def __init__(self, symb, frames):

		self.type = symb['type']
		self.value = symb['value']
		# musime si jeste poslat ramce
		self.frames = frames


	"""
	Ziskani symbolu
	@return obsah symbolu
    """
	def symb_get(self):
		if (self.type == 'var'):
			var_prop = self.__symb_exec_tests()
			# navratova hodnota, v teto fazi jiz vime, ze promenna
			# na 100% existuje, neni treba resit chyby
			return self.__symb_var_exists(var_prop[0], var_prop[1])
		else:
			# proste vratime promennou
			return {
				'type'  : self.type,
				'value' : self.value
			}

	"""
	Je symbol definovan?
	@return obsah symbolu nebo chyba
    """
	def symb_get_defined(self):
		temp = self.symb_get()
		if (temp['type'] != None and temp['value'] != None):
			return {
				'type'  : temp['type'],
				'value' : temp['value']
			}
		else:
			Error(56, 'Nemame hodnotu nebo datovy typ promenne')


	"""
	Nastavime symbol s hodnotou a typem
	@param typ 
	@param hodnota
	@return None
    """
	def symb_set(self, type, value):
		if (self.type == 'var'):
			var_prop = self.__symb_exec_tests()
			# updatuj hodnotu
			self.__symb_update_var(var_prop[0], var_prop[1], type, value)
		else:
			Error(52, 'Volas nastaveni hodnoty promenne nad literalem')

	"""
	Vytvorime promennou ze symbolu
	@return None
    """
	def symb_create_var(self):
		if (self.type == 'var'):
			var_prop = self.__symb_exec_tests()
			# musime handlovat vrchol zasobniku
			if (var_prop[0] == 'LF'):
				if (not self.frames['stack'].top().frame_add(var_prop[1])):
					Error(52, 'Redefinice globalni promenne')
			# tady je nam to jedno, jelikoz tohle se kontroluje drive
			# ve funkci __fn_handle_frames
			else:
				if (not self.frames[var_prop[0].lower()].frame_add(var_prop[1])):
					Error(52, 'Redefinice globalni promenne')
		else:
			Error(52, 'Z ceho chces vytvaret promennou?')


	# PRIVATE

	"""
	Rozparsuj jmeno promenne
	@return None
    """
	def __symb_var_name(self):
		return [
			self.value[:2],
			self.value[3:]
		]

	"""
	Proved vsechny testy ohledne spravnosti ramce/jmena atd.
	@return jmeno promenne
	"""
	def __symb_exec_tests(self):
		# rozparsuj jmeno promenne (ramec/jmeno)
		var_prop = self.__symb_var_name()
		# zkontroluj, jestli je pristup k ramci validni
		self.__symb_check_frame(var_prop[0])
		# vratime si vytvoreny objekt
		return var_prop

	"""
	Vyzkousej pristup k ramci, pokud neexistuje -> chyba
	@param ramec k testu
	@return None
    """
	def __symb_check_frame(self, frame):
		# pokud je to GF, neni co resit, ramec je vzdy dostupny
		# pro nase opicky
		if (frame == 'TF'):
			if (self.frames['tf'] == None):
				Error(55, 'Nemuzes pridavat/cist z TF, kdyz neexistuje')
		elif (frame == 'LF'):
			if (not self.frames['stack'].not_empty()):
				Error(55, 'Zasobik ramcu je prazdny, proste нет')
		
	"""
	Precteni hodnoty promenne, pokud neexistuje, vyhod chybu
	@param ramec 
	@param promenna
	@return None
    """
	def __symb_var_exists(self, frame, name):
		# wrapper pro promennou
		ret_var = None

		# musime handlovat vrchol zasobniku
		if (frame == 'LF'):
			ret_var = self.frames['stack'].top().frame_read(name)
			if (ret_var == None):
				Error(54, 'Promenna, kterou chces cist/upravit neexistuje')
		# tady je nam to jedno, jelikoz tohle se kontroluje drive
		# ve funkci __fn_handle_frames
		else:
			#print(name)
			ret_var = self.frames[frame.lower()].frame_read(name)
			if (ret_var == None):
				Error(54, 'Promenna, kterou chces cist/upravit neexistuje')
		#
		
		return ret_var

	"""
	Update promenne (hodnota + typ)
	@param ramec
	@param promenna
	@param typ
	@param hodnota
	@return None
    """
	def __symb_update_var(self, frame, name, type, value):
		# promennou musim precist
		# promenna existuje, zde jiz nemusime nic testovat
		if (self.__symb_var_exists(frame, name) != None):
			if (frame == 'LF'):
				self.frames['stack'].top().frame_update(name, type, value)
			else:
				self.frames[frame.lower()].frame_update(name, type, value)