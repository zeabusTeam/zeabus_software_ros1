/*
	File name			:	text_colot.h		
	Author				:	Supasan Komonlit
	Date created		:	2018 , DEC 20
	Date last modified	:	2018 , ??? ??
	Purpose				:	This header library for define text_color
							
	Maintainer			:	Supasan Komonlit
	e-mail				:	supasan.k@ku.th
	version				:	0.5.0
	status				:	Production

	Namespace			:	zeabus_library
*/

#include <stdio.h>

#ifndef _ZEABUS_LIBRARY_TEXT_COLOR_H__
#define _ZEABUS_LIBRARY_TEXT_COLOR_H__

	// TCC when T is type N = normal and B = bold and U = underscore
	//			CC is colort example RE = RED
	//	<special> RES is reset to normal color
	// Reference http://ascii-table.com/ansi-escape-sequences.php

	#define NRE "\x1B[0;31m";
	#define BRE "\x1B[1;31m";

	#define NGR "\x1B[0;32m";
	#define BGR "\x1B[1;32m";

	#define NYE "\x1B[0;33m";
	#define BYE "\x1B[1:33m";

	#define NBL "\x1B[0;34m";
	#define BBL "\x1B[1;34m";

	#define NMA "\x1B[0;35m";
	#define BMA "\x1B[1;35m";

	#define RES "\x1B[0m";

#endif
