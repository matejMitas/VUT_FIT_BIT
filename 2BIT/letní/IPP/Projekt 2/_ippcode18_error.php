<?php

/*
Rozhrani pro chyby
@project: IPP18 proj2
@brief: Rozhrani pro chyby
@author: Matej Mitas
@file: ippcode18_error.php
*/

class ErrorCode {
	
	/**
     * Pole pro vysvetleni k chybovym kodum
     * @var pole
     */
	private $err_codes = [
		'10' => ">Spatne pouziti parametru spousteni\nNavratovy kod: 10",
		'11' => ">Nepodarilo se otevrit vstupni soubor\nNavratovy kod: 11",
		'12' => ">Nepodarilo se otevrit vystupni soubor\nNavratovy kod: 12",
		'21' => ">Lexikalni chyba analyzatoru\nNavratovy kod: 21",
		'22' => ">Hlavni souboru musi vzdy zacinat '.IPPcode18'"
	];

	/**
	 * Vypiseme chybovou zpravu a ukoncime program s kodem
	 * @return void
	 */
	public function yield_err() {
		fwrite(STDERR, $this->err_codes[$this->code]);
		exit($this->code);
	}

	/**
	 * Konstruktor
	 * @param chybovy kod
	 * @return void
	 */
	public function __construct($code) {
		$this->code = $code;
		$this->yield_err();
	}
}

?>