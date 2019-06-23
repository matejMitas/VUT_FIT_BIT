<?php

/*
Reseni pruchodu adresari
@project: IPP18 proj2
@brief: Reseni pruchodu adresari
@author: Matej Mitas
@file: ippcode18_tests_io.php
*/

class Tests_IO {

	private $_path;
	private $_recursive;
	// hlavni objekt pro vraceni
	public $dirs = [];

	/**
	 * Konstruktor
	 * @param cesta pro prohledani
	 * @param je nutno prochazet rekurzivne? (parametr programu)
	 * @return void
	 */
	public function __construct($path, $rec) {
		$this->_pathname = $path;
		$this->_path = realpath($path);
		$this->_recursive = $rec;
		// TODO: je to opravdu 10?
		// nevalidni ci neexistujici cesta
		if (strcmp($this->_path, "") == 0) 
			$err = new ErrorCode(10);
	}

	/**
	 * Samotne prohledavani cesty ([ne]rekurzivne)
	 * @return void
	 */
	public function io_lookup() {
		// hledame rekuzivne
		if ($this->_recursive) {
			//echo "rekurze";
			// objekt iteratoru
			$objects = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($this->_path));
			// je mozno ke vsem souborum pristoupit?
			try {
				// projedeme vsechny soubory
				foreach ($objects as $full_path => $object){
					// nacteme prvni lomitko od konce (pro soubor)
					$file_slash = strrpos($full_path, '/');
					// nacteme druhe lomitko (pro nazev slozky)
					$dir_slash = strrpos($full_path, '/', -(strlen($full_path) - $file_slash + 1));

					// nacteme jmeno souboru
					$file = substr($full_path, $file_slash + 1);
					$extenstion = substr($file, strrpos($file, '.'));

					// nacteme jmeno slozky
					$dir = substr($full_path, $dir_slash + 1, ($file_slash - $dir_slash - 1));
					
					// v tuto chvili nas zajimaji pouze .src, zbytek se
					// da volitelne dogenerovat
					if (strcmp($extenstion, ".src") == 0) {
				  		// pokud uz mame index, tak pridame, jinak tvorime
				  		if (!array_key_exists($dir, $this->dirs))
				  			$this->dirs[$dir] = [];

				  		$temp_arr = [];

				  		array_push($temp_arr, $file);
				  		array_push($temp_arr, $full_path);

				  		array_push($this->dirs[$dir], $temp_arr);
					}
				}

			} catch(UnexpectedValueException $e) { 
				// TODO: co vracet tady za chybu?
				$err = new ErrorCode(10);
        	} 

		} else {
			//echo "iterativni";
			$files = scandir($this->_path);
			// jeli cesta bez podslozek
			$slash = strrpos($this->_pathname, '/');
			// mame co osekavat
			if ($slash) {
				$test_name = substr($this->_pathname, strrpos($this->_pathname, '/') + 1);
			} else {
				$test_name = $this->_pathname;
			}
			// vytvorime array, do ktere pridavame
			$this->dirs[$test_name] = [];

			// projedeme vsechny soubory v ve slozke
			foreach ($files as $file) {
				// precteme koncovku
				$extenstion = substr($file, strrpos($file, '.'));
				// v tuto chvili nas zajimaji pouze .src, zbytek se
				// da volitelne dogenerovat
				if (strcmp($extenstion, ".src") == 0) {
					// vytvorime docasne pole pro nazev souboru a jeho
					// absolutni cestu k nemu
					$temp_arr = [];
					array_push($temp_arr, $file);
					// nemuzeme pouzit realpath, protoze soubory jsou v 
					// jine urovni zanoreni, nez 'test.php', nicmene
					// zname slozku, takze pouze slepime cesty
					array_push($temp_arr, $this->_path . "/" . $file);
					// pridej polozku
					array_push($this->dirs[$test_name], $temp_arr);
				}
			}
		}
		// vratime nalezene polozky
		return $this->dirs;
	}
}
