#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "projects.h"

const PROJECTS projects_table[] =
{
		{ DEBUG_PROJECT_UNKNOWN, "unknown" },
		{ DEBUG_PROJECT_BIGFLAG, "bigflagi" },
		{ DEBUG_PROJECT_OLCLAW, "olc_law" },
		{ DEBUG_PROJECT_LAW, "prawo w miastach" },
		{ DEBUG_PROJECT_SPELL_ITEMS, "komponenty do czarów" },
		{ 9999, NULL } //keep this one as the last entry
};

char *nazwa_projektu( int nr )
{
	int i;
	//do debugowania
	const char *call = "projects.c => nazwa_projektu";
	save_debug_info( call, NULL, NULL, DEBUG_PROJECT_BIGFLAG, DEBUG_LEVEL_ALL, TRUE );

	for ( i = 0; projects_table[i].name; i++ )
	{
		if ( projects_table[i].number == nr ) return projects_table[i].name;
	}
	return NULL;
}

