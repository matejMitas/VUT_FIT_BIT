<?php

class Stats {
	private $_stats_mode;
	private $_stats_path;
	// kombinace zvoleni zobrazeni dat
	private $_stats_loc;
	private $_stats_comments;
	// poradi parametru
	private $_stats_order;

	public $stats_lines = 0;
	public $stats_comments = 0;

	/**
	 * Zjisti, jestli radek neco obsahuje, nebo je treba ho preskocit
	 * opet volame statistiku
	 * @return void
	 */
	public function __construct($stats_mode, $stats_flags) {
		$this->_stats_mode = $stats_mode;
		$this->_stats_path = $stats_flags['stats'][1];
		// flagy 
		$this->_stats_loc = $stats_flags['loc'];
		$this->_stats_comments = $stats_flags['comments'];
		// poradi flagu
		$this->_stats_order = $stats_flags['order'];
	}

	/**
	 * Zjisti, jestli radek neco obsahuje, nebo je treba ho preskocit
	 * opet volame statistiku
	 * @return void
	 */
	public function stats_inc_lines() {
		$this->stats_lines++;
	}

	/**
	 * Zjisti, jestli radek neco obsahuje, nebo je treba ho preskocit
	 * opet volame statistiku
	 * @return void
	 */
	public function stats_inc_comments() {
		$this->stats_comments++;
	}

	/**
	 * Zjisti, jestli radek neco obsahuje, nebo je treba ho preskocit
	 * opet volame statistiku
	 * @return void
	 */
	public function stats_writeout() {
		$out = "";
		// pokud nam nahodou prijde prazdna cesta, reseni uz 
		// v mechanismech pred, ale je mozne, ze se stane chyba
		if (!$this->_stats_path)
			$er = new ErrorCode(12);

		// pokud mame oba parametry
		if ($this->_stats_comments && $this->_stats_loc) {

			if ($this->_stats_order == 1) {
				$out .= "$this->stats_lines\n";
				$out .= "$this->stats_comments\n";
			} else if ($this->_stats_order == 2) {
				$out .= "$this->stats_comments\n";
				$out .= "$this->stats_lines\n";
			}
		} // jenom pocet radku
		else if ($this->_stats_loc) {
			$out .= "$this->stats_lines\n";
		} 
		// jenom komentare
		else if ($this->_stats_comments) {
			$out .= "$this->stats_comments\n";
		} 
		// otevreni souboru a vypsani statistiky
		$file_name = $this->_stats_path;
	    if (($file = fopen($file_name, "w")) !== false ) {
	    	fwrite($file, $out);  
	    }
	    else {
	      	$er = new ErrorCode(12);
	    }
	}
}

?>