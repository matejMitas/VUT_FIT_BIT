<?php

/*
Validace instrukce
@project: IPP18 proj2
@brief: Validace instrukce
@author: Matej Mitas
@file: ippcode18_instruction.php
*/

class Instruction {
	
	// ulozeni operacniho kodu
	public $opr;
	// ulozeni argumentu operaco
	public $args = [
		'elems' => []
	];
	// pomocne promenne pro poradi
	private $line;
	private $order;
	// regex pro nazvy instrukci
	private $_inst_regex = [
		// zadne dalsi argumenty
		'/^(BREAK|CREATEFRAME|POPFRAME|PUSHFRAME|RETURN|CLEARS|ADDS|SUBS|MULS|IDIVS|LTS|GTS|EQS|ANDS|ORS|NOTS|INT2CHARS|STR2INTS)$/',
		// jeden argument
		'/^(DEFVAR)$/',
		'/^(CALL|LABEL|JUMP|JUMPIFEQS|JUMPIFNEQS)$/',
		'/^(PUSHS|POPS|DPRINT|WRITE)$/',
		// dva parametry
		'/^(READ)$/',
		'/^(MOVE|INT2CHAR|STRLEN|TYPE|NOT)$/',
		// tri parametry
		'/^(JUMPIFEQ|JUMPIFNEQ)$/',
		'/^(ADD|SUB|MUL|IDIV|LT|GT|EQ|AND|OR|CONCAT|GETCHAR|SETCHAR|STRI2INT)$/'
	];
	// regex pro parametry
	private $_param_regex = [
		// var/symbol
		'/^(GF|LF|TF)[@][\w$&%*_-]+$/',
		// symbol
		'/^(int|string|float|bool)[@]/',
		// label
		'/^[\w$&%*_-]+$/',
		// type
		'/^(int|string|float|bool)$/'
	];
	// regex pro symboly
	private $_symb_regex = [
		// int
		'/^[-+]?[0-9]+$/',
		// bool
		'/^(true|false)$/',
		// float
		'/^[-+]?[0-9]*\.[0-9]+$/',
		// string
		'/^(\\\\[0-9]{3}|[^\p{C}\p{Z}\\\\#])+$/u'
	];
	// vzory pro jednotlive typy instrukci
	private $_inst_blueprint = [
		[],
		[VARB],
		[LABEL],
		[SYMB],
		[VARB, TYPE],
		[VARB, SYMB],
		[LABEL, SYMB, SYMB],
		[VARB, SYMB, SYMB]
	];

	/**
	 * Konstruktor
	 * @return void
	 */
	public function __construct($line_ctx, $order) {
		$this->line = $line_ctx;
		$this->order = $order;
	}

	/**
	 * Funkce pro escapovani znaku, ktere koliduji s XML standartem
	 * @param retezec
	 * @return void
	 */
	public function inst_parse($gen) {
		// odstranime bila mista na zacatku/konci
		$line = trim($this->line);
		// rozlozime na stringy
		$strs = preg_split("/[\s]+/", $line);
		//ulozime stringy
		foreach ($strs as $key => $value) {
			# oseknuti arugmentu
			$value = trim($value);	
			# pridavani
			if ($key == 0) {
				$this->opr = strtoupper($value);
			} else {
				// jenom pro oznaceni ramcu
				$needle = $value;
				// jejich uprava na uppercase 
				if (strpos($needle, '@') == 2) {
					$old = substr($needle, 0, 2);
					if (!preg_match('/^(GF|LT|TF)$/', $old)) {
						$needle = str_replace($old, "", $needle);
						$old = strtoupper($old);
						$old .= $needle;
						$needle = $old;
					}
				}
				# $key-1, korekce pro nasledne zpracovani
				$this->args['elems'][$key-1] = $value;
			}
		}
		// zjistime co je ta instrukci
		$opr_type = $this->_inst_type($this->opr);
		// v $opr_type je ulozen jeji predpis, musime zkontrolovat
		// jestli jsou vsechny parametry v poradku
		$this->_inst_argv_type($opr_type);
		// generovani XML argumentu u instrukci
		$gen->gen_start_inst($this->order, $this->opr);
		$gen->gen_args($this->args);
		$gen->gen_end_inst();
	}

	/**
	 * Zkontrolujeme typ instrukce
	 * @param instrukce
	 * @return nalezena instrukce
	 */
	private function _inst_type($inst) {
		$found = 0;
		foreach ($this->_inst_regex as $index => $regex) {
			if (preg_match($regex, $inst))
				$found = $index;
		}

		if ($found == -1)
			$er = new ErrorCode(21);

		return $found;
	}

	/**
	 * Zkontrolujeme symbol
	 * @param symbol
	 * @param key
	 * @return void
	 */
	private function _inst_argv_symb($symb, $key) {
		// odstraneni identifikatoru
		// zde je to jenom pro versicherung 
		if (($at_pos = strpos($symb, '@'))) {
			// vyseparuju
			$prefix = substr($symb, 0, $at_pos + 1);
			$value = str_replace($prefix, "", $symb);
			// odstranim 
			$prefix = str_replace("@", "", $prefix);
			// je to validni typ konkretniho literalu
			$is_matched = false;
			// ma vubec cenu resit prochazeni?
			if ($value) {
				// 
				switch ($prefix) {
					case 'int': 
						if (preg_match_all($this->_symb_regex[0], $value))
							$is_matched = true;
						break;
					case 'bool':
						// explicitni uprava velikosti pismen pro binarni hodnoty
						$value = strtolower($value);
						if (preg_match_all($this->_symb_regex[1], $value))
							$is_matched = true;
						break;
					case 'float':
						if (preg_match_all($this->_symb_regex[2], $value))
							$is_matched = true;
						break;
					case 'string':
						if (preg_match_all($this->_symb_regex[3], $value))
							$is_matched = true;
						break;
				}
				// pokud jsem v regexu nenasel spravnou hodnotu
				if (!$is_matched)
					$er = new ErrorCode(21);
			}
			// ulozim do klice
			$this->args['elems'][$key] = $value;
		}
	}

	/**
	 * Zkontrolujeme typy parametru
	 * @param typ instrukce
	 * @return void
	 */
	private function _inst_argv_type($inst_type) {
		// echo "\nTyp instrukce: " . $this->opr . "\n";
		// pomocne promenne
		$args = $this->args['elems'];
		$blueprint = $this->_inst_blueprint[$inst_type]; 

		// nesedi pocet parametru k instrukci
		if (count($args) !== count($blueprint))
			$er = new ErrorCode(21);

		//echo "Parametry: \n";
		foreach ($blueprint as $key => $value) {
			$primitive_type;
			$arg = $args[$key];
			//symbol muze byt promenna i konstanta
			if ($value == SYMB) {

				$var_regex = $this->_param_regex[VARB];
				$sym_regex = $this->_param_regex[SYMB];

				// bud promenna nebo literal
				if (!(preg_match($var_regex, $arg) || preg_match($sym_regex, $arg))) {
					$er = new ErrorCode(21);
				}
				// pokud resime symbol, nikoliv promennou, kde je oddeleni
				// obsahu pred @ (zde oznaceni ramce) nezadouci
				if (preg_match($sym_regex, $arg)) {
					// potrebuji overit typ literalu
					$this->_inst_argv_symb($arg, $key);
				}
				// nastaveni datovych typu
				switch ($args[$key][0]) {
					case 'i':
						$primitive_type = 'int';
						break;
					case 'b':
						$primitive_type = 'bool';
						break;
					case 's':
						$primitive_type = 'string';
						break;
					case 'f':
						$primitive_type = 'float';
						break;
					default:
						$primitive_type = 'var';
						break;
				}
			} else {
				if (!(preg_match($this->_param_regex[$value], $arg))) {
					$er = new ErrorCode(21);
				}
				// nastaveni datovych typu
				switch ($value) {
					case 0:
						$primitive_type = 'var';
						break;
					case 2:
						$primitive_type = 'label';
						break;
					case 3:
						$primitive_type = 'type';
						break;
				}
			}
			// pridani datove typu
			$this->args['data_types'][$key] = $primitive_type;
		}
	}
}