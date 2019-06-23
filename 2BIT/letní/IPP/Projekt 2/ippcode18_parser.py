"""
Parser vstupniho XML
@project: IPP18 proj2
@brief: Parser vstupniho XML
@author: Matej Mitas
@file: ippcode18_parser.py
"""

import xml.etree.ElementTree as ET
import sys
import re
import os

from collections import deque
from ippcode18_error import Error
from enum import Enum

class Parser:
	# hlavni objekt pro program
	output = deque()

	#
	_inst_regex = [
		# zadne dalsi argumenty
		'^(BREAK|CREATEFRAME|POPFRAME|PUSHFRAME|RETURN|CLEARS|ADDS|SUBS|MULS|IDIVS|LTS|GTS|EQS|ANDS|ORS|NOTS|INT2CHARS|STR2INTS|INT2FLOATS|FLOAT2INTS)$',
		# jeden argument
		'^(DEFVAR)$',
		'^(CALL|LABEL|JUMP|JUMPIFEQS|JUMPIFNEQS)$',
		'^(PUSHS|POPS|DPRINT|WRITE)$',
		# dva parametry
		'^(READ)$',
		'^(MOVE|NOT|INT2CHAR|INT2FLOAT|FLOAT2INT|STRLEN|TYPE)$',
		# tri parametry
		'^(JUMPIFEQ|JUMPIFNEQ)$',
		'^(ADD|SUB|MUL|IDIV|LT|GT|EQ|AND|OR|CONCAT|GETCHAR|SETCHAR|STRI2INT)$'
	]
	# regularni vyraz pro parametry
	_param_regex = [
		# TODO: UTF-8 regex
		# var/symbol
		'^(GF|LF|TF)[@][\w$&%*_-]+$',
		# symbol
		'^(int|string|float|bool)[@][\x00-\x7F]*$',
		# label
		'^[\w$&%*_-]+$',
		# type
		'^(int|string|float|bool)$'
	];

	_data_types = '^(label|type|var|int|float|string|bool)$'


	_blueprint = [
		[],
		['var'],
		['label'],
		['symb'],
		['var', 'type'],
		['var', 'symb'],
		['label', 'symb', 'symb'],
		['var', 'symb', 'symb']
	];

	"""
	Konstruktor
	@param Jmeno vstupniho souboru
	@return None
    """
	def __init__(self, filename):
		if (not os.path.isfile(filename)):
			Error(11, 'Vstupni soubor neexistuje')
		elif (os.path.getsize(filename) == 0):
			Error(11, 'Vstupni soubor existuje, ale prazdny')
		else:
			# muzeme bezpecne priradit
			self.filename = filename

	"""
	Hlavni funkce pro beh celeho parseru
	@return objekt s obsahem souboru v citelne forme
    """
	def parser_run(self):
		# je to vubec validni xml
		try:
			# nacteme do promennes
			self.tree = ET.parse(self.filename)
		except ET.ParseError:
			Error(31, 'Spatne formatovane vstupni XML')


		self.root = self.tree.getroot()
		# mame tam spravnou hlavicku
		self.__parser_check_shebang()
		# projed XML a nasyp to do 'output'
		self.__parser_scrape_xml()
		# vrat objekt s daty
		return Parser.output

	"""
	Kontrola spavnosti hlavicky souboru
	@return None
    """
	def __parser_check_shebang(self):
		# kontrola spravne hlavicky vstupu
		if (not self.root.attrib['language'] == 'IPPcode18'):
			Error(32, 'Spatna/neexistujici hlavicka')


	"""
	Funkce pro pruchod celym XML stromem a jeho rozparsovanim
	@return None
    """
	def __parser_scrape_xml(self):
		# citac pro cislo instrukci, zajistime duplicitni a nesourode
		# poradi instrukci, ktere muze pozdeji rozbit program
		# viceme integritni omezeni
		order_counter = 0;
		# projed skrz cele XML
		for instruction in self.root:
			# konverze z XML.tree objektu na standarni list
			# argumenty instrukce
			argv = list(instruction)
			# pomocne promenne
			opcode = instruction.attrib.get('opcode')
			# poradi instrukce i s normalizaci pro Python
			order = int(instruction.attrib.get('order')) - 1
			# pokud nam nesedi poradi instrukci
			if (order != order_counter):
				Error(31, 'Nesedi order parametru u instrukce, neni v tom border?')

			# prace s ukazatelem poradi instrukce
			try:
				order = int(instruction.attrib.get('order'))
			except ValueError:
				Error(31, 'Atribut poradi evidetne neni validni int')
			# kontrola spravnosti argumentu poradi
			if (order < 1):
				Error(31, 'Atribut poradi musi byt > 0')

			# zkontroluj, jestli je operacni kod spravny
			blueprint_index = self.__parser_inst_check(instruction.attrib['opcode'])
			blueprint = Parser._blueprint[blueprint_index]
			#inst_blueprint = Parser._blueprint[inst_blueprint]
			# zkontroluj, zdali sedi pocet argumentu, vzhledem k operacnimu kodu
			self.__parser_arg_count(argv, blueprint_index)


			# pole pro jiz osetrene (escapove) a jinak upravene/zkontrolovane argumenty
			checked_argv = []

			for i in range(len(argv)):
				# slovnik pro obsah datovych atributu argumentu
				checked_arg = {}
				# kontrola validity parametru
				checked_arg = self.__parser_arg_check(
					blueprint[i],
					argv[i].attrib.get('type'), 
					argv[i].text
				)
				# pridej ho do pole argumentu instrukce
				checked_argv.append(checked_arg)

			# pridej do pole instrukci
			out = {}
			out['opcode'] = opcode.lower()
			# normalizace pro indexovani pole
			out['order'] = order - 1;
			out['argv'] = checked_argv

			# inkrementuj citac instrukci
			order_counter += 1
			# pridej novou polozku
			Parser.output.append(out)

	"""
	Overeni spravnosti operacniho kodu
	@param Jmeno instrukce
	@return vlastnosti instrukce
    """
	def __parser_inst_check(self, inst):
		regexes = Parser._inst_regex
		ret = None
		# projed vsechny mozne regexy
		for i in range(len(regexes)):
			if (re.match(regexes[i], inst)):
				ret = i
				break
		# vyres navratove typy
		return ret if (ret != None) else Error(32, 'Neznamy typ instrukce')

	"""
	Sedi pocet argumentu instrukce k spravnemu poctu?
	@param argument instrukce
	@param pozice argumentu
    """
	def __parser_arg_count(self, temp_argv, index):
		if (len(temp_argv) != len(Parser._blueprint[index])):
			Error(32, 'Nesedi pocet parametru k instrukci')

	"""
	Je operand validni co se tyce spravnosti zapisu?
	@param ocekavany typ
	@param realny typ
	@param hodnota
	@return argument
    """
	def __parser_arg_check(self, expected_type, type, value):
		# sedi nam typy argumentu?
		self.__parser_map_type(expected_type, type)
		# samotne kolecko kontrolovani spravnosti hodnot
		if (type == 'var'):
			if (not re.match(Parser._param_regex[0], value)):
				Error(32, 'Spatne jmeno promenne')

		elif (type == 'label'):
			if (not re.match(Parser._param_regex[2], value)):
				Error(32, 'Spatny nazev labelu')

		elif (type == 'type'):
			if (not re.match(Parser._param_regex[3], value)):
				Error(32, 'Spatny nazev datoveho typu u typu')

		elif (expected_type == 'symb'):
			# typovani symbolu
			if (type == 'int'):
				try:
					value = int(value)
				except ValueError:
					Error(52, 'Toto jaksi neni INT')
				except TypeError:
					value = None

			elif (type == 'float'):
				try:
					value = float(value)
				except ValueError:
					Error(52, 'Toto jaksi neni FLOAT')
				except TypeError:
					value = None

			elif (type == 'bool'):
				# zde se pouze podivani, co mame za hodnotu
				if (value == 'true'):
					value = bool(1)
				elif (value == 'false'):
					value = bool(0)
				else:
					Error(52, 'Toto jaksi neni BOOL')

			elif (type == 'string'):
				# escapovani stringu
				if (value):
					value = self.__parser_escape_arg(value)

		# slovnik pro kopii do listu rozparsovanych instrukci
		return {
			'type': type,
			'value': value
		}

	def __parser_arg_cast(self, value, type_name):
		pass

	"""
	Kontrola spravnosti ocekavanych a realnych typu
	@param ocekavany typ
	@param realny typ
	@return None
    """
	def __parser_map_type(self, expected_type, type):

		# korekce pro symbol
		if (expected_type == 'symb'):
			valid_subtypes = ['int', 'float', 'bool', 'string', 'var']
			valid = False
			for subtype in valid_subtypes:
				if (type == subtype): 
					valid = True
			if (not valid):
				Error(32, 'Typ operandu nesedi k ocekavanemu typu')

		else:
			if (expected_type != type):
				Error(32, 'Typ operandu nesedi k ocekavanemu typu')

	"""
	Prevadeni escape sekvenci
	@param retezec na upravu
	@return modifikovany retezec
    """
	def __parser_escape_arg(self, str_to_escape):
		# spravny datovy typ argumentu
		value = str_to_escape
		# priprav se pro praci s Python string
		# kompiluj regex
		regex = re.compile('\\\\[0-9][0-9]?[0-9]?')
		# najdi vsechny escape sekvence pro nahrazeni
		escape_seq = re.findall(regex, value)

		for seq in escape_seq:
			value = value.replace(seq, chr(int(seq[1:])))

		return value