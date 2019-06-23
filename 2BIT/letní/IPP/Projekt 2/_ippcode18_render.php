<?php

/*
Vykreslovaci komponenta pro test
@project: IPP18 proj2
@brief: Vykreslovaci komponenta pro test
@author: Matej Mitas
@file: ippcode18_render.php
*/

class Render {
	
	// obecne pro HTML hlavicku
	private $_render_charset = 'UTF-8';
	private $_render_lang = 'cs';
	private $_render_title = 'IPP18code Testy';
	// pro vypsani testu
	private $_render_ctx;
	
	/**
	 * Konstruktor
	 * @param obsah k vyrenderovani
	 * @return void
	 */
	public function __construct($ctx) {
		$this->_render_ctx = $ctx;
	}

	/**
	 * Obecna cinnost renderu
	 * @return void
	 */
	public function render_write() {
		$this->_render_template();
	}

	/**
	 * Sablona, header & footer
	 * @return void
	 */
	private function _render_template() {
		echo <<<EOT
<!DOCTYPE html>
<html lang={$this->_render_lang}>
<head>
	<meta charset={$this->_render_charset}>
	<title>{$this->_render_title}</title>
	<style>
		body {
			background: #ffffff;

			font-family: -apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Oxygen-Sans,Ubuntu,Cantarell,"Helvetica Neue",sans-serif;
		}

		main {
			width: 100%;
			max-width: 960px;
			margin: 50px auto;
			background: #fff;
			box-shadow: 0 0 20px 1px rgba(0, 0, 0, .1);
			border: 1px solid rgba(0,0,0,.15);
			border-radius: 5px;
		}

		body, h1, h2, p, ul, li {
			padding: 0;
			margin: 0;
		}

		ul {
			list-style: none;
		}

		section > ul ul {
			background: rgba(0,0,255,0.04);
			display: flex;
		}

		section > ul ul li {
			flex: 0 0 33.3333%;
		}

		h1 {
			border-bottom:  1px solid rgba(0,0,0,.15);
			padding: 40px;
			font-size: 3em;
			font-weight: 300;
			background: rgba(0,0,255,0.5);
			color: rgba(0,0,0,.5);
			text-align: center;
		}

		.header {
			font-size: 0.8em;
			color: rgba(0,0,0,.25);
			border-bottom:  1px solid rgba(0,0,0,.05);
			background: rgba(0,0,255,0.075);
		}


		.l-padding {
			padding: 15px 35px;
		}

		h2.h2 {
			padding: 35px;
			color: rgba(0,0,0,0.75);
		}

		.ctx {
			//background: red;
			flex-wrap: wrap;
		}

		.ctx ul {
			display: flex;
			flex-wrap: wrap;
			flex: 0 0 calc(100% - 70px);
			padding: 15px 35px;
		}

		.green {
			background: rgba(0,255,0,.2);
		}

		.red {
			background: rgba(255,0,0,.2);
		}
</style>
</head>
<body>
	{$this->_render_body()}
</body>
</html>
EOT;
	}

	/**
	 * Specifikace data (radky pro testy)
	 * @return void
	 */
	private function _render_body() {

		$body = '
			<main>
				<h1>Testy pro IPP18</h1>
					<section>
						<ul>
							';

		// projedeme kazdou slozku
		foreach ($this->_render_ctx as $key => $dir) { 
			$body .= '
				<li>
					<h2 class="l-padding h2">' . $key . '</h2>
					<ul class="l-padding header">
						<li>Název testu</li>
						<li>Jméno souboru</li>
						<li>Status</li>
					</ul>
					<ul class="ctx">
					';
			
			foreach ($dir as $test) {

				if ($test[1] == 0) {
					$class = 'green';
				} else {
					$class = 'red';
				} 

				$body .= '
					<ul class=' . $class . '>
						<li>' . substr($test[0], 0, strlen($test[0])-4) . '</li>
						<li>' . $test[0] . '</li>';

				// co vlastne vypiseme
				if ($test[1] == 0) {
					$body .= '<li>V pořádku</li>';
				} else if ($test[1] == 1) {
					$body .= '<li>Chyba v parseru</li>';
				} else if ($test[1] == 2) {
					$body .= '<li >Chyba v interpretu</li>';
				} else if ($test[1] == 3) {
					$body .= '<li>Chyba ve výstup. souboru</li>';
				}

		
				$body .= '</ul>';
			}

			$body .= '
					</ul>
				</li>';
		}

		$body .= '
				</ul>
			</section>
		</main>';

		return $body;
	}
}