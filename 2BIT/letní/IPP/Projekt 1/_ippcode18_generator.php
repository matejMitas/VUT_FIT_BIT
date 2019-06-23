<?php

class Gen {
	// hlavni objekt generatoru
	private $gen_main;
	// pole pro nahrazeni znaku
	private $gen_escape = [
		'before' => ['/"/', "/'/", '/</', '/>/', '/&/'],
		'after' => ['&quot;', '&apos;', '&lt;', '&gt;', '&amp;']
	];

	/**
	 * Konstruktor
	 * @return void
	 */
	public function __construct() {
		// hlavni objekt
		$this->gen_main = new XMLWriter();
		// nastaveni voleb generatoru
		$this->gen_main->openMemory();
		$this->gen_main->setIndent(1);
		$this->gen_main->setIndentString("    ");
	}

	/**
	 * Desktruktor
	 * @return void
	 */
	public function __destruct() {
		unset($this->gen_main);
	}

	/**
	 * Funkce pro escapovani znaku, ktere koliduji s XML standartem
	 * @param retezec
	 * @return void
	 */
	private function _gen_escape($str) {
		$before = $this->gen_escape['before'];
		$after = $this->gen_escape['after'];

		foreach ($before as $key => $value) {
			preg_replace($value, $after[$key], $str);
		}
		return $str;
	}	

	/**
	 * Hlavicka dokumentu
	 * @return void
	 */
	public function gen_start_document() {
		// hlavicka dokumentu
		$this->gen_main->startDocument('1.0', 'UTF-8');
		$this->gen_main->startElement("program");
		$this->gen_main->writeAttribute('language', 'IPPcode18'); 
	}

	/**
	 * Vygenerujeme jednu instrukci
	 * @param poradi
	 * @param operacni kod
	 * @return void
	 */
	public function gen_start_inst($order, $opr) {
		$this->gen_main->startElement("instruction");
		$this->gen_main->writeAttribute('order', $order); 
		$this->gen_main->writeAttribute('opcode', $opr); 
	} 

	/**
	 * Generovani argumentu
	 * @param pole argumentu
	 * @return void
	 */
	public function gen_args($args) {
		foreach ($args['elems'] as $key => $value) {
			// nutna korekce pro vypis
			$display = $key + 1;
			// samotny vypis
			$this->gen_main->startElement("arg$display");
			$this->gen_main->writeAttribute('type', $args['data_types'][$key]); 
			// zde je potreba zjistit, zdali neni treba neco escapovat
			// primarne znaky, ktere XML neumi (",',<,>,&)
			$this->gen_main->text($this->_gen_escape($args['elems'][$key]));
			$this->gen_main->endElement();
		}
	} 

	/**
	 * Generovani konce instrukce
	 * @return void
	 */
	public function gen_end_inst() {
		$this->gen_main->endElement();
	} 

	/**
	 * Generovani konce dokumentu
	 * @return void
	 */
	public function gen_end_document() {
		$this->gen_main->endElement();
		$this->gen_main->endDocument();
		echo $this->gen_main->outputMemory();
	}
}

?>