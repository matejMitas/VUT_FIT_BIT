"""
Procedura pro vykonani pomocne funkce dle operacniho kodu
@project: IPP18 proj2
@brief: Procedura pro vykonani pomocne funkce dle operacniho kodu
@author: Matej Mitas
@file: ippcode18_function.py
"""

import sys
from ippcode18_frame import Frame
from ippcode18_error import Error
from ippcode18_symbol import Symbol
from ippcode18_retype import Retype
# pro dekodovani prepisu
import json

class Function():
	
	"""
	Konstruktor
	@param instrukce
	@param konfigurace instrukce
	@param dict ramcu
	@param datovy zasobnik
	@param zasobnik volani
	@param struktura pro navesti
	@param program counter
	@return None
    """
	def __init__(self, inst, config, frames, data_stack, call_stack, labels, pc):
		self.opcode = inst['opcode']
		self.frames = frames
		self.argv = inst['argv']
		# pomocna promenna pro program counter
		self.pc = pc
		# predpis pro volani instrukce
		self.config = config
		# zasobnik na volani
		self.call_stack = call_stack
		# datovy zasobnik
		self.data_stack = data_stack
		#
		self.labels = labels

	"""
	Metoda pro vykonani pomocne funkce dle operacniho kodu
	@return funkce k zavolani i s parametry
    """
	def fn_execute(self):
		fn_name = self.opcode
		# u labelu neni treba nic delat, je kontrolovan drive
		if (fn_name != 'label'):
			# funkce zcela bez parametru a neni to stackova
			# napr. break, popframe atd.
			if (self.config == []):
				# neni treba nic zpracovat
				ret_val = None
			else:
				# jedna se o instrukce zasobnikove nebo triadresne?
				mode = self.config[0][0]

				# pokud se jedna o zasobnikovou instrukci
				if (mode == 'stack'):
					ret_val = self._fn_prepare_stack_args(fn_name, self.config[0][1])
					# dale je treba normalizovat jeji predpis
					# pouze urizneme konecne 's'
					fn_name = fn_name[0:-1]
				else:
					ret_val = self._fn_prepare_args(fn_name)
			

			# nalezneme jmeno instrukce v souhrnu metod
			try:
				fn_exec = getattr(self, "_{0}".format(fn_name))
			except AttributeError:
				raise NotImplementedError("Nejak se zapomnelo implementovat `{}`".format(fn_name))

			# a podle poctu instrukci volam s/bez parametru
			if (ret_val):
				return fn_exec(*(ret_val))
			else:
				return (fn_exec())

			

	"""
	Metoda argumentu instrukce zasobnikoveho typu
	@params jmeno instrukce
	@params pocet parametru
	@return tuple s pripravenou funkci
    """		
	def _fn_prepare_stack_args(self, fn_name, count):
		# parametry ze stacku
		params = []
		# 1. parametr, cil, nemame symbol, pouze zpatky stack
		# nezapomen, ze je treba obratit!
		params.append(True);
		# x. parametry
		for _ in range(count):
			# postupne naplnime argumenty
			# TODO: zde portovat checkovani parametru podle pravidel
			# idealne do vlastni funkce
			params.append(self.data_stack.top())
			self.data_stack.pop()

		# posledni parametr, jedna o zasobnikovou instukci
		# nezapomen, ze je treba obratit!
		params.append(None);
		# obratime list, z duvodu opacneho volani na stacku
		params = params[::-1]
		# prevedeme na tuple, z duvodu volani funkci
		return tuple(params)
		
	"""
	Metoda argumentu instrukce triadresneho typu
	@params odkaz do objektu blueprintu
	@return tuple s pripravenou funkci
    """		
	def _fn_prepare_args(self, index):
		# indexace v predpisech
		rules = self.config
		# pole s pripravenymi argumenty
		prepared_args = []
		# pro loopovani
		argv_index = 0
		# insturukce bez parametru
		if (rules != []):
			#zkontroluj, jestli vsechny pravidla sedi
			for rule in rules:
				# reset nalezeneho typu
				match_type = None
				# nastaveni typu a modu (cteni, zapis, oboje)
				arg_type = rule[0]
				# pokud je to promenna
				if (arg_type == 'var'):
					# vytvorim z ni symbol
					var_temp = Symbol(self.argv[argv_index], self.frames)
					# nactu si mod
					var_mode = rule[1]
					# jinak proste vytvorime objekt pro symbol
					# a posilame dale (v samotnych funkcich uz 
					# resime tvoreni ci zapis)
					prepared_args.append(var_temp)

				# prace se symbolem (tedy literal nebo promenna)
				elif (arg_type == 'symb'):
					#
					allowed_types = rule[1]
					symbol_defined = rule[2]
					# vytvor docasny symbol pro polozky symb
					temp = Symbol(self.argv[argv_index], self.frames)
					# zde nas zajima jakou metodu mame pouzit, jelikoz pri 'move' je dovoleno int@
					# treba u scitani nemuzeme scitat prazdny datovy typ, jehoz hodnota je nedefinovana
					# TODO: problem u setchar a prazdneho stringu, zkus to
					temp = temp.symb_get_defined() if symbol_defined else temp.symb_get()
					# existuje mnozina povolenych datovych typu? Jestli ano, musime 
					# kontrolovat, jinak proste pridame nactenou polozku
					if (allowed_types == []):
						prepared_args.append(temp)
					elif (allowed_types == ["="]):
						prepared_args.append(temp)
					else:
						# projed vsechny akceptovane typy
						has_type = None 
						# pokud nemame zadny datovy typ
						if (match_type == None):
							for t in allowed_types:
								if (t == temp['type']):
									has_type = t
						# pokud ho jiz mame, nemusime nic resit
						else:
							has_type = match_type if match_type == temp['type'] else None

						# osetreni chyby, pripadna dalsi obrata v cyklu
						if (has_type == None):
						 	Error(53, 'Zadany datovy typ nepodporuje operaci zadanou instrukci')
						else:
							# nastavime realny datovy typ
							match_type = has_type
							prepared_args.append(temp)

				elif (arg_type == 'label'):
					prepared_args.append(self.argv[argv_index].get('value'))
				else:
					Error(99, 'Neznamy typ parametru')
				# posunuti se v poli parametru
				argv_index += 1

			# prevod na tuple z duvodu vyhodnosti volani funkcnich parametru
			return tuple(prepared_args)


	"""
	=================================================
	SEKCE: Ramce/funkce
	=================================================
	"""

	# DEFVAR
	def _defvar(self, name):
		name.symb_create_var()

	# MOVE
	def _move(self, dest, source):
		# nastaveni hodnoty promenne
		dest.symb_set(source.get('type'), source.get('value'))

	# CREATEFRAME
	def _createframe(self):
		# zavolej destruktor pro ramec
		if (self.frames.get('tf') != None):
			pass 
		# vytvor novy docasny ramec
		self.frames['tf'] = Frame(tf = True)

	# PUSHFRAME
	def _pushframe(self):
		tf = self.frames.get('tf')
		# mame vubec co pushovat?
		if (tf != None):
			# zmena typu ramce
			self.frames.get('tf').frame_tf2lf()
			# ulozeni na zasobnik
			self.frames.get('stack').push(tf)
			self.frames['tf'] = None
		else:
			# ramec TF neexistuje
			Error(55, 'Soudruhu, TF neexistuje a ty ho zadas!')

	# POPFRAME
	def _popframe(self):
		if (self.frames.get('stack').not_empty()):
			self.frames['tf'] = self.frames['stack'].pop()
			self.frames.get('tf').frame_lf2tf()
		else:
			Error(55, 'Soudruhu, chces si do TF ulozit LF, ale zadne tam neni')

	"""
	=================================================
	SEKCE: Tok programu
	=================================================
	"""

	# JUMP
	def _jump(self, label_name):
		# dummy hodnota pro tok programu
		return self.labels.get(label_name)

	# JUMPIFEQ
	def _jumpifeq(self, label_name, eq1, eq2):
		ret = Retype('r=', eq1, eq2)
		ret = ret.rt_exec()

		# pokud se nam rovnaji hodnoty skoc
		if (ret.a == ret.b):
			return self.labels.get(label_name)
		else:
			return None;

	# JUMPIFNEQ
	def _jumpifneq(self, label_name, eq1, eq2):
		ret = Retype('r=', eq1, eq2)
		ret = ret.rt_exec()

		# pokud se nam rovnaji hodnoty skoc
		if (ret.a != ret.b):
			return self.labels.get(label_name)
		else:
			return None;

	# CALL
	def _call(self, label_name):
		# ulozime na zasobnik volani funkci
		self.call_stack.push(self.pc + 1)
		# skocime na dane navesti
		return self.labels.get(label_name)

	# RETURN
	def _return(self):
		# vybereme ze zasobniku volani a skocime
		ret_place = self.call_stack.top();
		if (ret_place == None):
			Error(56, 'Davas return, ale nemas kam skocit, zavolal jsi call?')

		return ret_place

	"""
	=================================================
	SEKCE: Datovy zasobnik
	=================================================
	"""

	# PUSHS
	def _pushs(self, val):
		self.data_stack.push(val)

	# POPS
	def _pops(self, var):
		top = self.data_stack.top()
		self.data_stack.pop()
		# kontrolujeme obsah jiz v top fci
		var.symb_set(top.get('type'), top.get('value'))

	"""
	=================================================
	SEKCE: Aritmeticke
	=================================================
	"""

	"""
	Metoda pro reseni prirazeni (stack/promenna)
	@param symbol
	@param typ
	@param hodnota
	@param boolovska hodnota jestli na stack nebo do promenne
	@return None
    """
	def __fn_assign(self, symbol, type, val, stack):
		if (stack):
			self.data_stack.push({'type': type, 'value': val})
		else:	
			symbol.symb_set(type, val)

	# ADD
	def _add(self, dest, s1, s2, stack = False):
		ret = Retype('r=', s1, s2)
		ret = ret.rt_exec()
		self.__fn_assign(dest, s1.get('type'), ret.a + ret.b, stack)

	# SUB
	def _sub(self, dest, s1, s2, stack = False):
		ret = Retype('r=', s1, s2)
		ret = ret.rt_exec()
		self.__fn_assign(dest, s1.get('type'), ret.a - ret.b, stack)

	# MUL
	def _mul(self, dest, s1, s2, stack = False):
		ret = Retype('r=', s1, s2)
		ret = ret.rt_exec()
		self.__fn_assign(dest, s1.get('type'), ret.a * ret.b, stack)

	# IDIV
	def _idiv(self, dest, s1, s2, stack = False):
		ret = Retype('r=', s1, s2)
		ret = ret.rt_exec()
		# kontrola deleni nulou
		if (ret.b == 0 or ret.a == 0):
			Error(57, 'Delime nulou, coz tak nejak nejde')
		else:
			self.__fn_assign(dest, s1.get('type'), ret.a // ret.b, stack)


	"""
	=================================================
	SEKCE: Relacni
	=================================================
	"""

	# LT
	def _lt(self, dest, s1, s2, stack = False):
		ret = Retype('r=', s1, s2)
		ret = ret.rt_exec()
		if (ret.a < ret.b):
			self.__fn_assign(dest, 'bool', 1, stack)
		else:
			self.__fn_assign(dest, 'bool', 0, stack)
	
	# GT
	def _gt(self, dest, s1, s2, stack = False):
		ret = Retype('r=', s1, s2)
		ret = ret.rt_exec()
		if (ret.a > ret.b):
			self.__fn_assign(dest, 'bool', 1, stack)
		else:
			self.__fn_assign(dest, 'bool', 0, stack)

	# EQ
	def _eq(self, dest, s1, s2, stack = False):
		ret = Retype('r=', s1, s2)
		ret = ret.rt_exec()
		if (ret.a == ret.b):
			self.__fn_assign(dest, 'bool', 1, stack)
		else:
			self.__fn_assign(dest, 'bool', 0, stack)

	"""
	=================================================
	SEKCE: Boolovske
	=================================================
	"""

	# AND
	def _and(self, dest, s1, s2, stack = False):
		self.__fn_assign(dest, 'bool', s1.get('value') & s2.get('value'), stack)

	# OR
	def _or(self, dest, s1, s2, stack = False):
		self.__fn_assign(dest, 'bool', s1.get('value') | s2.get('value'), stack)

	# NOT
	def _not(self, dest, s1, stack = False):
		if (s1.get('value')):
			self.__fn_assign(dest, 'bool', 0, stack)
		else:
			self.__fn_assign(dest, 'bool', 1, stack)

	"""
	=================================================
	SEKCE: Konverzni
	=================================================
	"""

	# INT2CHAR	
	def _int2char(self, dest, s1, stack = False):
		try: 
			convert = chr(s1.get('value'))
			self.__fn_assign(dest, 'string', convert, stack)
		except ValueError:
			Error(58, "Konverze int2char vyzaduje cislo znaku v Unicode")

	# STRI2INT
	def _stri2int(self, dest, s1, s2):
		# TODO: pro stack
		# nacteme string
		string = s1.get('value')
		# normalizace pro index
		string_len = len(string) - 1
		# neindexujeme nekam do pekel?
		index = s2.get('value')
		if (index < 0 or index > string_len):
			Error(58, "Indexace mimo retezec (str2int)")
		# uloz symbol
		dest.symb_set('int', ord(string[index]))
		
	def _int2float(self, dest, s1, stack = False):
		"""
		ret = Retype('r', s1, 'float')
		ret = ret.rt_exec()
		print(ret)

		self.__fn_assign(dest, 'float', ret, stack)"""
		pass

	def _float2int(self, dest, s1):
		"""
		dest.symb_set('int', int(s1.get('value')))
		"""
		pass

	"""
	=================================================
	SEKCE: I/O
	=================================================
	"""

	# READ
	def _read(self, dest, type):
		# zkusime, jestli je vubec co cist
		try:
			# nacteme hodnotu
			value = input()
			type = type.get('value')

			# typovani symbolu
			if (type == 'int'):
				try:
					value = int(value)
				except ValueError:
					value = 0;

			elif (type == 'float'):
				try:
					# TODO: poresit ty silene zapisy floatu
					value = float(value)
				except ValueError:
					value = 0;

			elif (type == 'bool'):
				# zde se pouze podivani, co mame za hodnotu
				if (value == 'true'):
					value = bool(1)
				else:
					value = bool(0)
			# prirad do symbolu
			dest.symb_set(type, value)
		except EOFError:
			Error(56, "V .in souboru neni hodnota")
		
	# WRITE
	def _write(self, to_write):
		to_write_type = to_write.get('type')
		# ma vubec neco cenu vypisovat?
		if (to_write_type != None):
			if (to_write_type == 'bool'):
				v = to_write.get('value')
				if (v == 0):
					print('false')
				else:
					print('true')
			else:
				print(to_write.get('value'))
		else:
			Error(56, 'Neni co vytisknout')


	"""
	=================================================
	SEKCE: Strings
	=================================================
	"""

	# CONCAT
	def _concat(self, dest, s1, s2):
		s1 = s1.get('value')
		s2 = s2.get('value')
		# pokud mame prazdne string
		if (s1 == None):
			s1 = ""
		elif (s2 == None):
			s2 = ""
		# nastavime symbol
		dest.symb_set('string', s1 + s2)

	# STRLEN
	def _strlen(self, dest, s1):
		s1 = s1.get('value')
		if (s1 == None):
			s1 = ""
		dest.symb_set('int', len(s1))

	# GETCHAR
	def _getchar(self, dest, s1, s2):
		# nacteme string
		string = s1.get('value')
		# normalizace pro index
		string_len = len(string) - 1
		# neindexujeme nekam do pekel?
		index = s2.get('value')
		if (index < 0 or index > string_len):
			Error(58, "Indexace mimo retezec (str2int)")
		# uloz symbol
		dest.symb_set('string', string[index])

	# SETCHAR
	def _setchar(self, dest, s1, s2):
		# nacteme string
		index = s1.get('value')
		chars = s2.get('value')
		# nacteme retezec na upravu
		string = dest.symb_get().get('value')
		string_len = len(string)

		if (index < 0 or index > string_len):
			Error(58, "Indexace mimo retezec (str2int)")

		new_val = chars[0]
		# prirad hodnotu
		string = string[:index] + new_val + string[index+1:]
		# uloz symbol
		dest.symb_set('string', string)


	"""
	=================================================
	SEKCE: Typova kontrola
	=================================================
	"""

	# TYPE
	def _type(self, dest, s1):
		dest.symb_set('string', s1.get('type'))

	"""
	=================================================
	SEKCE: Debugging funkce
	=================================================
	"""

	# DPRINT
	def _dprint(self, s1):
		# vypis na standartni chybovy vystup
		sys.stderr.write("- Typ     : {}\n- Hodnota : {}\n".format(
				s1.get('type'),
				s1.get('value')
			)
		)

	# BREAK
	def _break(self):
		print('vytiskni ladici informace')
