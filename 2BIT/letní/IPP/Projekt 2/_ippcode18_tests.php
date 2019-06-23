<?php

/*
Hlavni testovy soubor
@project: IPP18 proj2
@brief: Hlavni testovy soubor
@author: Matej Mitas
@file: ippcode18_tests.php
*/

include '_ippcode18_tests_io.php';
include '_ippcode18_options_test.php';

class Tests {
	
	// pro prepinace prikazove radky
	public $opt = [];
	// navrat z prohledavani 
	private $_dirs = [];
	// cesty ke executables
	private $_parser;
	private $_inter;

	/**
	 * Konstruktor
	 * @return void
	 */
	public function __construct() {
		// zjisteni parametru se kterymi bude program spusten
		$opt = new CmdOptions($GLOBALS['argc'], $GLOBALS['argv']);
		// nastaveni modu
		$this->opt = $opt->cmd_get_options();

		// nastavime cesty k spustitelnym souborum
		$tmp_parse_path = $this->opt['flags']['parser'];
		$tmp_inter_path = $this->opt['flags']['inter'];

		//var_dump();

		// podivame se, jestli mame cestu pro parser
		if ($tmp_parse_path[0]) {
			$this->_parser = realpath($tmp_parse_path[1]);
		} else {
			$this->_parser = realpath('parse.php');
		}

		// podivame se, jestli mame cestu pro interpret
		if ($tmp_inter_path[0]) {
			$this->_inter = realpath($tmp_inter_path[1]);
		} else {
			$this->_inter = realpath('interpret.py');
		}

		// pokud se nepodari najit spustitelne soubory, vrat chybu
		if ($this->_inter == false || $this->_parser == false) {
			$er = new ErrorCode(10);
		}
	}

	/**
	 * Nachystani vsech potrebnych zalezitosti pro vykonani
	 * jednotlivych testu
	 * @return void
	 */
	public function test_init() {

		// chceme jenom zobrazit napovedu
		if ($this->opt['mode'] == HELP) {
			echo <<<EOT
--help vypíše na standardní výstup nápovědu skriptu (nenačítá žádný vstup), kterou lze převzít ze zadání (lze odstranit diakritiku, případně přeložit do angličtiny dle zvoleného jazyka dokumentace). Tento parametr nelze kombinovat s žádným dalším parametrem, jinak skript ukončete s chybou 10.
--directory=path testy bude hledat v zadaném adresáři (chybí-li tento parametr, tak skript
prochází aktuální adresář)
--recursive testy bude hledat nejen v zadaném adresáři, ale i rekurzivně ve všech jeho podadresářích
--parse-script=file soubor se skriptem v PHP 5.6 pro analýzu zdrojového kódu v IPP- code18 (chybí-li tento parametr, tak implicitní hodnotou je parse.php uložený v aktuálním adresáři)
--int-script=file soubor se skriptem v Python 3.6 pro interpret XML reprezentace kódu v IPPcode18 (chybí-li tento parametr, tak implicitní hodnotou je interpret.py uložený v ak- tuálním adresáři)
EOT;
		} else {
			// zjistime me, odkud chceme testy cist
			$path = $this->opt['flags']['directory'][1];
			// kdyz uzivatel nezada, prohledavame aktualni adresar
			if (strcmp($path, "") == 0) {
				$path = '.';
			}

			// chceme prohledavat rekurzivne?
			$recursive = $this->opt['flags']['recursive'];
			// prohledame adresar/adresare
			$io = new Tests_IO($path, $recursive);
			$this->_dirs = $io->io_lookup();

			// zavolame funkci na provedeni testu
			$this->_test_exec();
		}
	}

	/**
	 * Invokace renderovani
	 * @return void
	 */
	private function _test_render() {
		$render = new Render($this->_dirs);
		$render->render_write();
	}

	/**
	 * Prace s polem souboru a volani testu
	 * @return void
	 */
	private function _test_exec() {
		// pro kazdy jeden testovaci soubor
		foreach ($this->_dirs as $name => $dir) {
			foreach ($dir as $index => $test) {
				// pro kazdou cestu proved test
				$test_ret = $this->_test_single_test($test[0], $test[1]);
				// vysledek testu uloz
				$this->_dirs[$name][$index][1] = $test_ret;
			}
		}

		$this->_test_render();
	}

	/**
	 * Metoda pro vykonani jednotliveho testu
	 * @param jmeno testu
	 * @param cesta k .src souboru
	 * @return void
	 */
	private function _test_single_test($name, $src_path) {
	 	// nachystame si cesty pro vsechny soubory
		$in_path = substr($src_path, 0, -3) . "in";
		$out_path = substr($src_path, 0, -3) . "out";
		$rc_out = substr($src_path, 0, -3) . "rc";

		// pole pro obsah souboru
		$ret = [];
		// vlastni kod
		$ret_code = 0;
		// nacteny ocekavany kod
		$exp_ret_code = 0;
		// vlastni navratova hodnota
		$value = 0;

		// nacteme vsechny potrebne soubory
		// vstupni soubor
		$in_file = fopen($in_path, 'c');
		// vystupni soubor
		$out_file = fopen($out_path, 'c');
		// zde jiz musime kontrolovat existenci, jelikoz apendujeme
		// pak staci jenom precist kod a zavrit, vice nas nezajima
		if (file_exists($rc_out)) {
			// otevreme pro cteni
			$rc_file = fopen($rc_out, 'r');
			// precteme hodnotu
			$tmp_code = fscanf($rc_file, "%d\n")[0];
			if ($tmp_code)
				$exp_ret_code = $tmp_code;

		} else {
			// otevreme pro cteni
			$rc_file = fopen($rc_out, 'w');
			fwrite($rc_file, "0\n");
		}

		// zavreme, dal jiz nebudeme potrebovat
		fclose($rc_file);

		// vytvorime si docasne soubory pro vystupy z jednotlivych
		// casti celeho programu
		$tmp_parser = tempnam(realpath('./temp'), 'ipp18_parser');
		$tmp_inter = tempnam(realpath('./temp'), 'ipp18_inter');

		// vytvorime si strukturu volani parseru
		$parser_call = <<<EOT
php5.6 $this->_parser < $src_path > $tmp_parser\n
EOT;

		// vytvorime si volani pro interpret
		$inter_call = <<<EOT
python3.6 $this->_inter --source=$tmp_parser < $in_path > $tmp_inter\n
EOT;
	
		// vytvorime volani pro porovnani vystupu
		$diff_call = <<<EOT
diff $tmp_inter $out_path > /dev/null\n
EOT;

		// zavolame parser
		exec($parser_call, $ret, $ret_code);

		//echo "Parser: " . $ret_code . "\n"; 
		// jestli je vystup z parseru 0, pokracujeme dal, jinak vratime
		if ($ret_code == 0) {
			// zavolame interpret
			exec($inter_call, $ret, $ret_code);
			// porovname navratove kody
			// nema cenu diffovat, uz navratovy kod je spatny
			if ($ret_code != $exp_ret_code) {
				$value = 2;
			} else {
				// provedeme differenci
				exec($diff_call, $ret, $ret_code);

				//echo "Diff: " . $ret_code . "\n"; 
				// sedi nam navratove kody, ale nesedi obsah souboru
				// proto vratime 1
				if ($ret_code) 
					$value = 3;
			}
		} else {
			// proste vratime kod pri chybe parseru
			// neni treba diffovat vystup, nicmene je 
			// treba zkontrolovat navratove kody
			// napriklad, pokud uz to ma padnout na parseru
			// musime se kody rovnat
			if ($ret_code != $exp_ret_code)
				$value = 1;
			// jinak neni treba nic delat, nula je jiz 
			// nastavena vyse
		}


		// vymazeme docasne soubory
		unlink($tmp_inter);
		unlink($tmp_parser);

		// zavreme soubory
		fclose($out_file);
		fclose($in_file);

		// vratime vysledek provedene operace
		return $value;
	} 

}