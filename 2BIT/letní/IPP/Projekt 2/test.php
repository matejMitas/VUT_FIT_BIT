<?php

/*
Invokace testu
@project: IPP18 proj2
@brief: Invokace testu
@author: Matej Mitas
@file: test.php
*/


// nacteme renderovaci komponentu
include '_ippcode18_render.php';
include '_ippcode18_tests.php';


$my_tests = new Tests();
$my_tests->test_init();

?>