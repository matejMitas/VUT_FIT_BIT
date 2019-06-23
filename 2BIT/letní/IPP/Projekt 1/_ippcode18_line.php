<?php   

class Line {
	// nema zadny obsah
	private $line_is_empty;
	// sice nema zadny pouzitelny obsah
	// ale je na nem komentar
	private $line_has_comment;
	// samotny obsah radku
	public $ctx;
	public $line_order;

	/**
	 * @param samotny radek
	 * @return void
	 */
	public function __construct($line_ctx) {
		$this->ctx = $line_ctx;
	}
	
	/**
	 * Zjisti, jestli radek neco obsahuje, nebo je treba ho preskocit
	 * opet volame statistiku
	 * @return void
	 */
	public function line_parse($stats) {
		static $line_cnt = 1;

		$line_type = $this->_line_type();
		// je potreba neco evidovat?
		if ($stats) {
			if ($line_type > 1)
				$stats->stats_inc_comments(); // mame validni radek
			// musime rozdelit, podminky se kryji
			if (!$line_type || $line_type == INCLUDE_COM_LINE)
				$stats->stats_inc_lines(); // mame validni radek
		}

		// odstraneni komentaru (pokud si zvolime, jsou jiz prectene), k nicemu
		// jiz dal neslouzi
		if ($line_type == INCLUDE_COM_LINE) {
			$this->ctx = str_replace(substr($this->ctx, strpos($this->ctx, "#")), "", $this->ctx);
			// usekli jsme to bez konce radku, delalo by to 
			// obskurnosti pri dalsim zpracovani
			$this->ctx .= "\n";
			// nyni jim mame radek pouze s uzitecnymi informacemi
			$line_type = 0;
		}

		if (!$line_type) {
			$this->line_order = $line_cnt;
			$line_cnt++;
		}

		return $line_type;
	}

	/**
	 * Zjisti, jestli radek neco obsahuje, nebo je treba ho preskocit
	 * opet volame statistiku
	 * @return void
	 */
	public function line_parse_shebang() {
		$hashbang = strtolower(trim($this->ctx));
		// zkontrolujeme, jestli se nachazi spravna hlavicka
		if (strcmp($hashbang, ".ippcode18") !== 0) 
			$er = new ErrorCode(21);
	}

	/**
	 * Zjisti, jestli radek neco obsahuje, nebo je treba ho preskocit
	 * opet volame statistiku
	 * @return void
	 */
	private function _line_type() {
		$line = $this->ctx;

		if (strlen(trim($this->ctx)) === 0)
			return EMPTY_LINE; // tohle je prazdny radek
		else if ($line[0] === '#')
			return BEGIN_COM_LINE; // zacina komentarem
		else if (strpos($line, '#'))
			return INCLUDE_COM_LINE;
		else
			return VALID_LINE;
	}
}

?>