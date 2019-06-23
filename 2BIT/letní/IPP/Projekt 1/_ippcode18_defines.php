<?php

// pro beh programu
/* BASIC - standartni beh programu
 * HELP - program zobrazi pouze napovedu
 * STATS - zobrazi kompletni statistiku
 * STATS_LINES - zobrazi pouze pocet radku s uzitnou informaci
 * STATS_COMMENTS - zobrazi pocet komentaru
 */

define("BASIC", 1);
define("HELP", 2);
define("STATS", 3);
define("STATS_LINES", 4);
define("STATS_COMMENTS", 5);

// pro rozdeleni radku
define("VALID_LINE", 0);
define("EMPTY_LINE", 1);
define("BEGIN_COM_LINE", 2);
define("INCLUDE_COM_LINE", 3);

// instrukce
define("INST", -1);
define("VARB", 0);
define("SYMB", 1);
define("LABEL", 2);
define("TYPE", 3);

?>