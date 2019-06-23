<?php 

/*
Hlavni soubor parseru
@project: IPP18 proj2
@brief: Hlavni soubor parseru
@author: Matej Mitas
@file: ippcode18_parser.php
*/

class Parser {
	
	// pro vypsani stavu po behu programu
	public $return_code;
	// typ spusteni parseru
	private $_parser_mode;
	public $parser_stats;
	// 
	public $parser_cnt_lines;
	//
	public $parser_output;
	//
	public $parser_flags;

	/**
	 * Konstruktor
	 * @return void
	 */
	public function __construct() {
		// zjisteni parametru se kterymi bude program spusten
		$opt = new CmdOptions($GLOBALS['argc'], $GLOBALS['argv']);
		// nastaveni modu
		$ret_opt = $opt->cmd_get_options();
		$this->parser_mode = $ret_opt['mode'];
		$this->parser_flags = $ret_opt['flags'];

		$this->parser_output = new Gen();
		$this->parser_output->gen_start_document();
	}

	/**
	 * Zjisti, jestli radek neco obsahuje, nebo je treba ho preskocit
	 * opet volame statistiku
	 * @return void
	 */
	public function parser_run() {
		// rozdeleni modu
		if ($this->parser_mode == HELP) {
			echo <<<EOT
--help vypíše na standardní výstup nápovědu skriptu (nenačítá žádný vstup), kterou lze převzít ze zadání (lze odstranit diakritiku, případně přeložit do angličtiny dle zvoleného jazyka dokumentace). Tento parametr nelze kombinovat s žádným dalším parametrem, jinak skript ukončete s chybou 10.
EOT;
		} else {
			// vytvorime objekt pro statistiku, pokud je ta moznost zvolena
			if ($this->parser_mode > 2) {
				$this->parser_stats = new Stats($this->parser_mode, $this->parser_flags);
			}
			$this->_parser_read_lines();
			// ukoncime generator a vypiseme data
			$this->parser_output->gen_end_document();
		}
	}

	/**
	 * Zjisti, jestli radek neco obsahuje, nebo je treba ho preskocit
	 * opet volame statistiku
	 * @return void
	 */
	private function _parser_read_lines() {
		// mame hlavicku?
		$shebang = False;
		// nacitej ze vstupu
		while (($this->ctx = fgets(STDIN)) !== False) {
			// novy radek
			$line = new Line($this->ctx);
			// mame vubec 
			if (!$shebang) {
				$line->line_parse_shebang();
				$shebang = !$shebang;
			}
			else {
				$value = $line->line_parse($this->parser_stats);
				if (!$value)
					$this->_parser_parse_inst($line);
			}
		}

		if (!$shebang)
			$er = new ErrorCode(21);

		// vypiseme statisticka data, pokud to chceme
		if ($this->parser_stats) {
			$this->parser_stats->stats_writeout();
		}
	}

	/**
	 * Kontrola radku, volame pro kazdy radek souboru
	 * @param radek
	 * @return void
	 */
	private function _parser_parse_inst($line) {
		// vytvoreni nove instrukce
		$inst = new Instruction($line->ctx, $line->line_order);
		$inst->inst_parse($this->parser_output);
	}
}

?>