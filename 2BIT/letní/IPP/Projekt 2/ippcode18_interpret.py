"""
Hlavni modul pro spusteni celeho programu
@project: IPP18 proj2
@brief: Hlavni modul pro spusteni celeho programu
@author: Matej Mitas
@file: ippcode18_interpret.py
"""

from ippcode18_stack import Stack
from ippcode18_frame import Frame
from ippcode18_parser import Parser
from ippcode18_function import Function
from ippcode18_error import Error
from ippcode18_argparse import Argparse
from ippcode18_stats import Stats
# pro nacteni parseru JSONu
import json
import sys

class Interpret:

	"""
	Konstruktor
	@param chceme zapnout debuggovaci mod?
	@return None
    """
	def __init__(self, debug = False):
		# nachystej ramce
		self.frames = {
			'gf' 	: Frame(gf = True),
			'tf' 	: None, # vytvor ukazatel na docasny ramec
			'stack' : Stack()
		}
		# nachystej datovy zasobnik
		self.data_stack = Stack()
		# nachystej zasobnik volani
		self.call_stack = Stack()
		# zasobnik pro ulozeni labelu
		self.labels = {}
		# program counter
		self.pc = 0
		# prepinac pro debug
		self.accepted = {
			"help": False,
			# vstupni soubor
			"source": False,
			"source_file": "",
			# statistiky
			"stats": False,
			"stats_file": "",
			# pomocne pro statistiky
			"insts": False,
			"vars": False
		}
		# mod pro beh programu
		# 1 - napoveda
		# 2 - standartni spusteni
		# 3 - pocitej instrukce
		# 4 - pocitej pocet promennych
		# 5 - oba dva (vars + instr)
		# 6 - oba dva (instr + vars)
		self.mode = 0

	"""
	Incializace a samotny beh interpretu
	@return None
    """
	def intr_run(self):
		# 
		args = Argparse(sys.argv[1:])
		# zjistime mod programu
		args_ret = args.argp_parse_argv()
		# nastavime mod
		self.mode = args_ret.get('mode')

		# pokud mame napovedu, neni jiz treba nic delat
		if (self.mode == 1):
			print("--help - napoveda\n--source=file - vstupni soubor")
		else:
			# pokud mame zaple statistiky
			stats = None
			if (self.mode > 2):
				# Vytvor statistiky
				stats = Stats(self.mode, args_ret.get('stats_url'))

			# vytvor parser
			self.parser = Parser(args_ret.get('src_url'))
			# provedeme LEA a SYA
			self.prg = self.parser.parser_run()
			# pocet instrukci
			inst_count = len(self.prg)
			# zkontrolujeme tok programu (navesti a skoky)
			# prvni dva pruchody souborem
			self.__intr_check_flow(inst_count)
			# nacteme konfiguraci
			config = self.__intr_config_load()
			# zavolame programovou smycku
			self.__intr_loop(config, inst_count, stats);
		

	"""
	Programova smycka
	@param polozka blueprintu instrukce
	@param pocet instrukci
	@param chceme loggovat statistiky?
	@return None
    """
	def __intr_loop(self, config, count, stats):
		inst_count = 0;
		vars_defined = 0;
		# postupne projizdime programem
		# posledni pruchod souborem, treti
		while (self.pc < count):
			# vybereme jednotlivou polozku podle program counteru
			item = self.prg[self.pc]
			# najdeme aplikovatelny predpis k instrukci
			rules = self.__intr_instruction(config, item.get('opcode'))
			# pocitame polozky pro defvar (pro statistiky)
			if (item.get('opcode') == 'defvar'):
				vars_defined += 1
			# provedeme danou instrukci
			fn = Function(item, rules, self.frames, self.data_stack, self.call_stack, self.labels, self.pc)
			# mam linearne pokracovat v programu, nebo mam nekam skoci?
			flow = fn.fn_execute()
			# zde zajistujeme moznost skoku (podmineneho i nepodmineneho)
			# skoc na instrukci s poradim, nebo pokracuj linearne dal
			if (flow != None):
				self.pc = flow
			else:
				self.pc += 1
			# pocitame celkovy pocet instrukci
			inst_count += 1

		if (stats):
			stats.stats_inc_insts(inst_count)
			stats.stats_inc_vars(vars_defined)
			# zapiseme do souboru statistik
			stats.stats_inc_write_out()

		#self.__print_frames()

	"""
	Typ a predpis instrukce (triadresna nebo zasobnikova)
	@param polozka blueprintu instrukce
	@param jmeno instrukce
	@return navraceni pravidel
    """
	def __intr_instruction(self, config, opcode):
		# umisteni predpisu instrukci v JSON configu
		with_params = config.get('operd')
		on_stack = config.get('stack')
		# vysekneme posledni znak
		suffix = opcode[-1]

		try:
			# mame zasobnikovou instrukci
			if (suffix == 's' and opcode != 'pushs' and opcode != 'pops'):
			# instrukce s argumenty
				#print("Na stacku: {}".format(opcode))
				# normalizace pro rozpoznavani argumentu
				# sice nejsou treba, ale metodou musime projit, proto ji obalime
				rules = [['stack', on_stack.get(opcode)]]
			else:
				#print("S argumenty: {}".format(opcode))
				rules = with_params.get(opcode)

		except KeyError:
			Error(99, 'Pro dany instrukci neexistuje predpis')

		return rules


	"""
	Vyres navesti a jejich volani
	@param pocet instrukci
	@return None
    """
	def __intr_check_flow(self, inst_count):
		# pruchod pro labely	
		for i in range(0, inst_count):
			item = self.prg[i]
			opcode = item.get('opcode')

			if (opcode == 'label'):
				key = item.get('argv')[0].get('value')
				value = item.get('order') 
				# najdi label
				if (key in self.labels):
					Error(52, 'Duplictni label')
				# jinak proste prirad label
				self.labels[key] = value

		# pruchod pro funkci volani a jump
		for i in range(0, inst_count):
			item = self.prg[i]
			opcode = item.get('opcode')
			# pokud se jedna o jakoukoliv skokovou instrukci
			if (opcode == 'call' or 
				opcode == 'jump' or
				opcode == 'jumpifeq' or
				opcode == 'jumpifneq'):
				# najdeme jmeno labelu
				key = item.get('argv')[0].get('value')
				
				if (not key in self.labels):
					Error(52, 'Referencujeme neexistujici label')
					

	"""
	Nacti soubor s pozadavky kazde instrukce
	@return blueprint instrukce
    """
	def __intr_config_load(self):	
		# otevreni souboru
		try:
			config = open("ippcode18_blueprint.json", "r")
		except FileNotFoundError:
			Error(99, 'Nepodarilo se otevrit soubor pro popis instrukci (FILE CORRUPTED)')
		# precteme config
		config = config.read()
		# prevedeme do pouzitelne podoby
		config = json.loads(config)
		# vratime
		return config

	"""
	Debuggovaci vypis vsech ramcu
	@return None
    """
	def __print_frames(self):
		# docasna promenna
		f = self.frames
		# globalni frame
		f['gf'].frame_print()
		# docasny ramec
		if (f['tf'] != None):
			f['tf'].frame_print()
		# zasobnik lokalnich ramcu
		if (f['stack'].not_empty()):
			print("\n------------------------- @LF stack ------------------------")

			cnt = 0
			for item in f['stack'].ctx:
				print("\n                     Ramec_{} na stacku                      ".format(cnt))
				item.frame_print()
				cnt += 1

			print("\n************************************************************")
