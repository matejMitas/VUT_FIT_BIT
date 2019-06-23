"""
Rozhrani pro chyby a navratove kody
@project: IPP18 proj2
@brief: Rozhrani pro chyby a navratove kody
@author: Matej Mitas
@file: ippcode18_error.py
"""

import sys

class Error:

	err_codes = {
		# parametry a soubory
		'10': 'Spatne pouziti parametru spousteni',
		'11': 'Nevalidni vstupni soubor',
		'12': 'Nevalidni vystupni soubor',
		# lea a sya
		'31': 'Co jsi mi to poslal za XML?',
		'32': 'Lexikalni nebo Syntakticka chyba',
		# interpretace
		'52': 'Semanticka chyba',
		'53': 'Spatne typy operandu',
		'54': 'Promenna neexistuje (ramec OK)',
		'55': 'Ramec neexistuje',
		'56': 'Chybejici hodnota',
		'57': 'Deleni nulou',
		'58': 'Chybna prace s retezcem',
		# vnitrni chyba, jake je vyvolani?
		'99': 'Vnitrni chyba interpretu',
	}

	"""
	Konstruktor
	@param kod
	@param vlastni zprava pro uzivatele pri chybe
	@return None
    """
	def __init__(self, code, custom_message = ''):
		ec = str(code)
		if ec in Error.err_codes:
			sys.stderr.write("▷ {}\n⎆ {}\n".format(Error.err_codes[ec], custom_message))
			sys.exit(code)

