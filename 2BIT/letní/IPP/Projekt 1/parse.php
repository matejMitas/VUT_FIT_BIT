<?php

include '_ippcode18_error.php';
include '_ippcode18_options.php';
include '_ippcode18_instruction.php';
include '_ippcode18_parser.php';
include '_ippcode18_stats.php';
include '_ippcode18_line.php';
include '_ippcode18_generator.php';


// spust parser
$my_parser = new Parser();
$my_parser->parser_run();


?>