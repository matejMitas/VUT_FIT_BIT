"""
Implementace stacku
@project: IPP18 proj2
@brief: Implementace stacku
@author: Matej Mitas
@file: ippcode18_stack.py
"""

from collections import deque
from ippcode18_error import Error

class Stack:

	"""
	Konstruktor
	@return None
    """
	def __init__(self):
		self.ctx = deque()
	

	"""
	odstran z vrcholu
	@return vrchol
    """
	def pop(self):
		# kdyz prejedeme index
		try:
			return self.ctx.popleft()
		except IndexError:
			sys.stderr.write("Stack is already empty\n")

	"""
	pridej na vrchol
	@param hodnota
	@return None
    """
	def push(self, value):
		self.ctx.appendleft(value)	

	"""
	vytiskni obsah
	@return None
    """
	def print(self):
		print("[", end='')
		for item in self.ctx:
			print(" {0} |".format(item), end='')
		print(" $ ]")

	"""
	je zasobnik prazdny
	@return delka zasobniku
    """
	def not_empty(self):
		return len(self.ctx)

	"""
	precti vrchol
	@return hodnota z vrcholu
    """
	def top(self):
		if len(self.ctx) > 0:
			return self.ctx[0]
		else:
			Error(56, 'Na zasobniku neni co vypopovat')

	"""
	vycisti zasobnik
	@return None
    """
	def clear(self):
		# vymaz stack
		self.ctx.clear()
