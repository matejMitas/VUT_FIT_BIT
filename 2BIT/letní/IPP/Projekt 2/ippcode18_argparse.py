"""
Prace s parametry prikazove radky
@project: IPP18 proj2
@brief: Prace s parametry prikazove radky
@author: Matej Mitas
@file: ippcode18_argparse.py
"""

from ippcode18_error import Error

class Argparse():

	"""
	Konstruktor
	@param parametry prikazove radky
	@return None
    """
	def __init__(self, argv):
		# mod pro beh programu
		# 1 - napoveda
		# 2 - standartni spusteni
		# 3 - pocitej instrukce
		# 4 - pocitej pocet promennych
		# 5 - oba dva (vars + instr)
		# 6 - oba dva (instr + vars)
		self.mode = 0
		# parametry prikazove radky
		self.argv = argv
		# prepinac pro debug
		self.accepted = {
			"help": False,
			# vstupni soubor
			"source": False,
			"source_file": None,
			# statistiky
			"stats": False,
			"stats_file": None,
			# pomocne pro statistiky
			"insts": False,
			"vars": False
		}

	"""
	Rozparsujeme jednotlive argumenty
	@param parametry prikazove radky
	@return atrib typu dict s parametry
    """
	def argp_parse_argv(self):
		# ulozime argumenty a odsekneme 1. param.
		argv = self.argv
		argc = len(argv)

		# kdyby tam nekdo nahodou dal velky pocet parametru
		if (argc < 1 or argc > 5):
			Error(10, 'Nespravny pocet argumentu')
		else:
			for arg in argv:
				# osekneme -- ze zacatku
				arg = arg[2:]
				optarg = ""
				
				# podivame se, jestli nahodou nemame optarg
				try:
					index = arg.index('=')
					optarg = arg[index+1:]
					arg = arg[:index]
				except ValueError:
					pass

				# zkusime porovnat
				if (arg in self.accepted):

					# duplicitni argumenty
					if (self.accepted[arg]):
						Error(10, 'Duplicitni argumenty')

					# pokud chceme optarg
					if (arg == 'source' or arg == 'stats'):
						pass
						# zde je povinny, takze kdyz error
						if (optarg == ""):
							Error(10, 'Source a Stats musi mit soubor')
						else:
							self.accepted[arg] = True
							self.accepted[arg+"_file"] = optarg
					# pokud mame pocitat pocet proveden instrukci
					elif (arg == 'insts'):
						if (self.accepted['vars'] == 1):
							self.accepted[arg] = 2
						else:
							self.accepted[arg] = 1
					# pokud mame pocitat pocet inicializovanych promennych
					elif (arg == 'vars'):
						if (self.accepted['insts'] == 1):
							self.accepted[arg] = 2
						else:
							self.accepted[arg] = 1
					else:
						self.accepted[arg] = True
				else:
					Error(10, 'Nevalidni parametr')

		# podivame se jestli je to validni a nastavime mod programu
		self.__argp_check_argv()
		# vratime zjisteny mod
		return {
			'mode' : self.mode,
			'src_url': self.accepted.get('source_file'),
			'stats_url': self.accepted.get('stats_file')
		}

	"""
	Zkontrolujeme platnost a vzajmenou nekonfliknost rozparsovanych argumentu
	@return None
    """
	def __argp_check_argv(self):
		argv = self.accepted
		# zkontrolujeme jednotlive mody
		if (argv.get('help')):
			self.mode = 1
		# cokoliv jineho nez help
		elif (argv.get('source')):
			# pokud mame parametry a spravny beh programu
			if ((argv.get('insts') or argv.get('vars')) and not argv.get('stats')):
				Error(10, 'Statisticke parametry vyzaduji vystupni soubor')
			# mame oba parametry?
			if (argv.get('insts') and argv.get('vars')):
				if (argv.get('insts') == 1):
					self.mode = 5
				elif (argv.get('insts') == 2):
					print('prvni promenne')
					self.mode = 6
			# pokud mame jenom jeden z parametru
			elif (argv.get('insts')):
				self.mode = 3
			elif (argv.get('vars')):
				self.mode = 4
			# evidetne mame jenom jeden soubor
			else:
				self.mode = 2
		else:
			Error(10, 'Potrebuju nejakej vstupni soubor .xml')