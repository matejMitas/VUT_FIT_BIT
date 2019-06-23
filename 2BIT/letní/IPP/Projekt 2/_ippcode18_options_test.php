<?php

/*
Parsovani vstupnich parametru pro test
@project: IPP18 proj2
@brief: Parsovani vstupnich parametru pro test
@author: Matej Mitas
@file: ippcode18_options_test.php
*/

include '_ippcode18_defines.php';
include '_ippcode18_error.php';

class CmdOptions {

	private $_cmd_argc;
	private $_cmd_argv;
	private $_cmd_options;
	//
	public $cmd_flags = [
		"help" 			=> False,
		"directory" 	=> [False, ""],
		"recursive" 	=> False,
		"parser" 		=> [False, ""],
		"inter"     	=> [False, ""],
	];
	/* BASIC - standartni beh programu
	 * HELP - program zobrazi pouze napovedu
	 * STATS - zobrazi kompletni statistiku
	 * STATS_LINES - zobrazi pouze pocet radku s uzitnou informaci
	 * STATS_COMMENTS - zobrazi pocet komentaru
	 */
	public $cmd_mode;


	/**
	 * Konstruktor
	 * @param pocet parametru
	 * @param pole parametru
	 * @return void
	 */
	public function __construct($argc, $argv) {
		$this->_cmd_argc = $argc - 1; // nezajima nas filename
		$this->_cmd_argv = $argv; 
	}

	/**
	 * Pomoci getopt zjistime, co mame za parametry
     * @return zjistene prepinace a mod programu
     */
	public function cmd_get_options() {
		// nastaveni prijimanych parametru
		$longopts = [
			"help::",
			"directory:",
			"recursive::",
			"parse-script:",
			"int-script:",
		];
		
		// nacteni parametru
		$this->_cmd_options = getopt("", $longopts);
		//
		$this->_cmd_parse_options();
		$this->_cmd_check_options();
		// vrat mod programu
		return [
			'flags' => $this->cmd_flags,
			'mode' => $this->cmd_mode
		];
	}

	/**
     * Zjistime, jak se nam co kombinuje
     * @return void
     */
	private function _cmd_parse_options() {
		$argc_valid = count($this->_cmd_options);
		$argc_real = $this->_cmd_argc;
		// pomocne promenne pro citelnejsi zapis
		$opt = $this->_cmd_options;
		$flg = &$this->cmd_flags; // chceme menit promennou na "tridni" urovni
		// promenna na odchyt chyb 
		$err; 

		// zakladni kontrola na pocet paramateru/validnich parametru
		if ($argc_valid != $argc_real) {
			$err = new ErrorCode(10);
		}
		
		// postupne nastavime priznaky
		foreach ($opt as $flag => $value) {
			// obecne zaruceni prazdne hodnoty
			if (preg_match('/(help|recursive)/', $flag) && $value)
				$er = new ErrorCode(10);

			switch ($flag) {
				case "help":
					$flg['help'] = True;
					break;
				case "directory":
					$flg['directory'][0] = True;
					if ($value)
						$flg['directory'][1] = $value;
					else
						$er = new Error_Code(10);
					break;
				case "recursive":
					$flg['recursive'] = True;
					break;
				case "parse-script":
					$flg['parser'][0] = True;
					if ($value)
						$flg['parser'][1] = $value;
					else
						$er = new Error_Code(10);
					break;
				case "int-script":
					$flg['inter'][0] = True;
					if ($value)
						$flg['inter'][1] = $value;
					else
						$er = new Error_Code(10);
					break;
				default:
					break;
			}	
		}
	}

	/**
     * Nastavime mod programu
     * @return void
     */
	private function _cmd_check_options() {
		$flg = $this->cmd_flags;
		$mode = &$this->cmd_mode;

		// nic se nevylucuje, pouze prenastavime
		if ($flg['help']) {
			$mode = HELP;
		} else {
			$mode = BASIC;
		}
	}
}

?>