/***********************************************************************
 *                                                                     *
 * Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,     *
 * Michael Seifert, Hans-Henrik Stæfeldt, Tom Madsen and Katja Nyboe   *
 *                                                                     *
 * Merc Diku Mud improvements copyright (C) 1992, 1993 by              *
 * Michael Chastain, Michael Quan, and Mitchell Tse                    *
 *                                                                     *
 *   ROM 2.4 is copyright 1993-1998 Russ Taylor                        *
 *        Russ Taylor (rtaylor@hypercube.org)                          *
 *        Gabrielle Taylor (gtaylor@hypercube.org)                     *
 *        Brian Moore (zump@rom.org)                                   *
 *   By using this code, you have agreed to follow the terms of the    *
 *   ROM license, in the file Rom24/doc/rom.license                    *
 *                                                                     *
 ***********************************************************************
 *                                                                     *
 * KILLER MUD is copyright 1999-2011 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Brosig Micha³         (brohacz@gmail.com             ) [Brohacz   ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: progs_vars.c 11232 2012-04-06 21:53:02Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/progs_vars.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include "merc.h"
#include "tables.h"
#include "progs.h"
#include "lang.h"
#include "interp.h"
#include "recycle.h"
#include "olc.h"

PROG_VAR *		temp_var_list = NULL;

const char * var_type_to_str [] =
{
	"<ró¿ne>",
	"<char>",
	"<obj>",
	"<room>",
	"<string>",
	"<int>",
	NULL
};

const	struct	var_cmd_type	var_cmd_table	[] =
{
/*	{	"nazwa",					funkcja obslugujaca,		parametry,	typ zwracany,		"opis"	} */
/*	funkcje operuj±ce na liczbach */
	{	"int_add",					vc_int_add,					2,			PROG_VAR_INT,		{ PROG_VAR_INT, PROG_VAR_INT, -1 },			"zwraca sumê dwóch liczb"	},
	{	"int_sub",					vc_int_sub,					2,			PROG_VAR_INT,		{ PROG_VAR_INT, PROG_VAR_INT, -1 },			"zwraca roznicê dwóch liczb"	},
	{	"int_mult",					vc_int_mult,				2,			PROG_VAR_INT,		{ PROG_VAR_INT, PROG_VAR_INT, -1 },			"zwraca wynik mno¿enia dwóch liczb"	},
	{	"int_div",					vc_int_div,					2,			PROG_VAR_INT,		{ PROG_VAR_INT, PROG_VAR_INT, -1 },			"zwraca wynika dzielenia ca³kowitego dwóch liczb"	},
	{	"int_mod",					vc_int_mod,					2,			PROG_VAR_INT,		{ PROG_VAR_INT, PROG_VAR_INT, -1 },			"zwraca resztê z dzielenia dwóch liczb"	},
	{	"int_to_str",				vc_int_to_str,				1,			PROG_VAR_STRING,	{ PROG_VAR_INT, -1 },						"konwertuje liczbê na string"	},
	{	"int_random",				vc_int_random,				1,			PROG_VAR_INT,		{ PROG_VAR_INT, PROG_VAR_INT, -1 },			"zwraca losow± liczbê z podanego zakresu"	},
/*	funkcje operuj±ce na stringach */
	{	"str_concat",				vc_str_concat,				2,			PROG_VAR_STRING,	{ PROG_VAR_STRING, PROG_VAR_STRING, -1 },	"³±czy ze sob± dwa stringi"	},
	{	"str_substr",				vc_str_substr,				3,			PROG_VAR_STRING,	{ PROG_VAR_STRING, PROG_VAR_INT, PROG_VAR_INT, -1 },	"wyci±ga ze stringa podci±g zaczynaj±cy siê od danej pozycji i maj±cy podan± d³ugo¶æ"	},
	{	"str_length",				vc_str_length,				1,			PROG_VAR_INT,		{ PROG_VAR_STRING, -1 },					"zwraca d³ugo¶æ stringa"	},
	{	"str_to_int",				vc_str_to_int,				1,			PROG_VAR_INT,		{ PROG_VAR_STRING, -1 },					"konwertuje string na liczbê"	},
/*	funkcje operuj±ce na lokacjach */
	{	"room_by_vnum",				vc_room_by_vnum,			1,			PROG_VAR_ROOM,		{ PROG_VAR_INT,  -1 },						"zwraca lokacjê o podanym vnumie"	},
	{	"room_by_char",				vc_room_by_char,			1,			PROG_VAR_ROOM,		{ PROG_VAR_CHAR, -1 },						"zwraca lokacjê w ktorej stoi postac"	},
	{	"room_by_obj",				vc_room_by_obj,				1,			PROG_VAR_ROOM,		{ PROG_VAR_OBJ, -1 },						"zwraca lokacjê w ktorej znajduje siê obiekt"	},
	{	"room_flag_value",			vc_common_flag_value,		2,			PROG_VAR_INT,		{ PROG_VAR_ROOM, PROG_VAR_STRING, -1 },		"zwraca warto¶æ flagi o podanej nazwie na³o¿onej na podany room"	},
	{	"room_first_flag",			vc_common_first_flag,		1,			PROG_VAR_STRING,	{ PROG_VAR_ROOM, -1 },						"zwraca nazwê pierwszej flagi na li¶cie flag na³o¿onych na podany room"	},
	{	"room_next_flag",			vc_common_next_flag,		2,			PROG_VAR_STRING,	{ PROG_VAR_ROOM, PROG_VAR_STRING, -1 },		"zwraca nazwê kolejnej flagi po tej podanej jako drugi parametr, dotyczy danego rooma"	},
/*	funkcje operuj±ce na postaciach */
	{	"char_first_in_room",		vc_char_first_in_room,		1,			PROG_VAR_CHAR,		{ PROG_VAR_ROOM, -1 },						"zwraca pierwsz± postaæ (pocz±tek listy postaci) stoj±c± w podanym roomie"	},
	{	"char_next_in_list",		vc_common_next_in_list,		1,			PROG_VAR_CHAR,		{ PROG_VAR_CHAR, -1 },						"zwraca nastepn± postaæ w li¶cie postaci stoj±cych w lokacji"	},
	{	"char_in_room_by_name",		vc_char_in_room_by_name,	2,			PROG_VAR_CHAR,		{ PROG_VAR_ROOM, PROG_VAR_STRING, -1 },		"zwraca postaæ o podanym imieniu stojaca w podanym roomie"	},
	{	"char_in_room_by_vnum",		vc_char_in_room_by_vnum,	2,			PROG_VAR_CHAR,		{ PROG_VAR_ROOM, PROG_VAR_INT, -1 },		"zwraca postaæ o podanym vnumie stojac± w podanym roomie"	},
	{	"char_flag_value",			vc_common_flag_value,		2,			PROG_VAR_INT,		{ PROG_VAR_CHAR, PROG_VAR_STRING, -1 },		"zwraca warto¶æ flagi o podanej nazwie na³o¿onej na podan± postaæ"	},
	{	"char_first_flag",			vc_common_first_flag,		1,			PROG_VAR_STRING,	{ PROG_VAR_CHAR, -1 },						"zwraca nazwê pierwszej flagi na li¶cie flag na³o¿onych na podan± postaæ"	},
	{	"char_next_flag",			vc_common_next_flag,		2,			PROG_VAR_STRING,	{ PROG_VAR_CHAR, PROG_VAR_STRING, -1 },		"zwraca nazwê kolejnej flagi po tej podanej jako drugi parametr, dotyczy danej postaci"	},
/*	funkcje operuj±ce na obiektach */
	{	"obj_first_carried",		vc_obj_first_carried,		1,			PROG_VAR_OBJ,		{ PROG_VAR_CHAR, -1 },						"zwraca pierwszy obiekt w li¶cie noszonych przez postaæ"	},
	{	"obj_first_in_room",		vc_obj_first_in_room,		1,			PROG_VAR_OBJ,		{ PROG_VAR_ROOM, -1 },						"zwraca pierwszy obiekt w li¶cie obiektów le¿±cych w podanym roomie"	},
	{	"obj_next_in_list",			vc_common_next_in_list,		1,			PROG_VAR_OBJ,		{ PROG_VAR_OBJ, -1 },						"zwraca nastepny obiekt w liscie"	},
	{	"obj_in_room_by_name",		vc_obj_in_room_by_name,		2,			PROG_VAR_OBJ,		{ PROG_VAR_ROOM, PROG_VAR_STRING, -1 },		"zwraca obiekt o podanej nazwie le¿±cy w podanym roomie"	},
	{	"obj_in_room_by_vnum",		vc_obj_in_room_by_vnum,		2,			PROG_VAR_OBJ,		{ PROG_VAR_ROOM, PROG_VAR_INT, -1 },		"zwraca obiekt o podanym vnumie le¿±cy w podanym roomie"	},
	{	"obj_in_obj_by_vnum",		vc_obj_in_obj_by_vnum,		2,			PROG_VAR_OBJ,		{ PROG_VAR_ROOM, PROG_VAR_INT, -1 },		"zwraca pierwszy obiekt w pojemniku o podanym vnumie le¿±cy w podanym roomie"	},
	{	"obj_char_eq_slot",			vc_obj_char_eq_slot,		2,			PROG_VAR_OBJ,		{ PROG_VAR_CHAR, PROG_VAR_STRING, -1 },		"zwraca obiekt noszony przez postaæ w podanym miejscu na ciele"	},
	{	"obj_flag_value",			vc_common_flag_value,		2,			PROG_VAR_INT,		{ PROG_VAR_OBJ, PROG_VAR_STRING, -1 },		"zwraca warto¶æ flagi o podanej nazwie na³o¿onej na podany obiekt"	},
	{	"obj_first_flag",			vc_common_first_flag,		1,			PROG_VAR_STRING,	{ PROG_VAR_OBJ, -1 },						"zwraca nazwê pierwszej flagi na li¶cie flag na³o¿onych na podany obiekt"	},
	{	"obj_next_flag",			vc_common_next_flag,		2,			PROG_VAR_STRING,	{ PROG_VAR_OBJ, PROG_VAR_STRING, -1 },		"zwraca nazwê kolejnej flagi po tej podanej jako drugi parametr, dotyczy danego obiektu"	},
/*	ró¿ne funkcje */
	{	"var_by_name",				vc_var_by_name,				1,			PROG_VAR_UNDEFINED,	{ PROG_VAR_STRING, -1 },					"zwraca zmienn± o nazwie podanej jako parametr"	},
	{	"global_flag_value",		vc_common_flag_value,		1,			PROG_VAR_INT,		{ PROG_VAR_STRING, -1 },					"zwraca warto¶æ globalnej flagi o nazwie podanej jako parametr"	},
	{	"global_first_flag",		vc_common_first_flag,		1,			PROG_VAR_STRING,	{ -1 },										"zwraca nazwê pierwszej flagi na li¶cie globalnych flag"	},
	{	"global_next_flag",			vc_common_next_flag,		2,			PROG_VAR_STRING,	{ PROG_VAR_STRING, -1 },					"zwraca nazwê kolejnej globalnej flagi po tej podanej jako drugi parametr"	},
	{	"datetime",					vc_datetime,				1,			PROG_VAR_INT,		{ PROG_VAR_STRING, -1 },					"zwraca warto¶æ wskazanego w parametrze pola bie¿±cej daty i czasu (mo¿liwe parametry to: year, month, week_day, month_day, hour)"	},
	{	"",							0,							0,			-1,					{ -1 },										""	}
};

/*
 * tworzy now± zmienna o okre¶lonej warto¶ci
 * NIE DOLACZA JEJ DO LISTY
 */
PROG_VAR * new_prog_var( char * name, void * value, int type )
{
	PROG_VAR* pvar;

	CREATE( pvar, PROG_VAR, 1 );

	pvar->name		= str_dup( name );
	pvar->type		= type;
	pvar->next		= NULL;

	switch( pvar->type )
	{
		case PROG_VAR_CHAR:
			pvar->value.character = ( CHAR_DATA * ) value;
			break;
		case PROG_VAR_OBJ:
			pvar->value.object = ( OBJ_DATA * ) value;
			break;
		case PROG_VAR_ROOM:
			pvar->value.room = ( ROOM_INDEX_DATA * ) value;
			break;
		case PROG_VAR_STRING:
			//rellik: dodaje warunek bo progs_engine.c=>program_flow wywoluje z value == NULL i powoduje wywalenie muda
			if ( value )
			{
				pvar->value.string = str_dup((char * ) value );
			} else {
				pvar->value.string = NULL;
			}
			break;
		case PROG_VAR_INT:
			pvar->value.integer = str_dup( ( char * ) value );
			break;
		default:
			DISPOSE( pvar );
			return NULL;
	}

	return pvar;
}

/*
 * tworzy now± zmienn± i kopiuje do niej typ i warto¶æ drugiej zmiennej
 * taki konstruktor kopiuj±cy:)
 */
PROG_VAR * copy_prog_var( char * name, PROG_VAR * cpy_pvar )
{
	PROG_VAR* pvar;

	if ( !cpy_pvar )
		return NULL;

	CREATE( pvar, PROG_VAR, 1 );

	pvar->name		= str_dup( name );
	pvar->type		= cpy_pvar->type;
	pvar->next		= NULL;

	switch( cpy_pvar->type )
	{
		case PROG_VAR_CHAR:
			pvar->value.character = ( CHAR_DATA * ) cpy_pvar->value.character;
			break;
		case PROG_VAR_OBJ:
			pvar->value.object = ( OBJ_DATA * ) cpy_pvar->value.object;
			break;
		case PROG_VAR_ROOM:
			pvar->value.room = ( ROOM_INDEX_DATA * ) cpy_pvar->value.room;
			break;
		case PROG_VAR_STRING:
			pvar->value.string = str_dup( ( char * ) cpy_pvar->value.string );
			break;
		case PROG_VAR_INT:
			pvar->value.integer = str_dup( ( char * ) cpy_pvar->value.integer );
			break;
		default:
			pvar->value.character = NULL;
			break;
	}

	return pvar;
}

/*
 * kopiuje typ i warto¶æ jednej zmiennej do drugiej
 */
void var_copy_value( PROG_VAR * pvar, PROG_VAR * cpy_pvar )
{
	if ( !cpy_pvar )
		return;

	switch( pvar->type )
	{
		case PROG_VAR_STRING:
			free_string( pvar->value.string );
			break;
		case PROG_VAR_INT:
			free_string( pvar->value.integer );
			break;
	}

	pvar->type		= cpy_pvar->type;

	switch( cpy_pvar->type )
	{
		case PROG_VAR_CHAR:
			pvar->value.character = ( CHAR_DATA * ) cpy_pvar->value.character;
			break;
		case PROG_VAR_OBJ:
			pvar->value.object = ( OBJ_DATA * ) cpy_pvar->value.object;
			break;
		case PROG_VAR_ROOM:
			pvar->value.room = ( ROOM_INDEX_DATA * ) cpy_pvar->value.room;
			break;
		case PROG_VAR_STRING:
			pvar->value.string = str_dup( ( char * ) cpy_pvar->value.string );
			break;
		case PROG_VAR_INT:
			pvar->value.integer = str_dup( ( char * ) cpy_pvar->value.integer );
			break;
		default:
			pvar->value.character = NULL;
			break;
	}
}

/*
 * zeruje wartosc zmiennej
 */
void var_clear( PROG_VAR * pvar )
{
	if ( !pvar )
		return;

	pvar->type		= 0;
	pvar->value.character = NULL;
}

/*
 * ustawia wartosc zmiennej
 */
void var_set_value( PROG_VAR * pvar, void * value, int type )
{
	if ( !pvar )
		return;

	switch( pvar->type )
	{
		case PROG_VAR_STRING:
			free_string( pvar->value.string );
			break;
		case PROG_VAR_INT:
			free_string( pvar->value.integer );
			break;
	}

	pvar->type		= type;

	switch( type )
	{
		case PROG_VAR_CHAR:
			pvar->value.character = ( CHAR_DATA * ) value;
			break;
		case PROG_VAR_OBJ:
			pvar->value.object = ( OBJ_DATA * ) value;
			break;
		case PROG_VAR_ROOM:
			pvar->value.room = ( ROOM_INDEX_DATA * ) value;
			break;
		case PROG_VAR_STRING:
			pvar->value.string = str_dup( ( char * ) value );
			break;
		case PROG_VAR_INT:
			pvar->value.integer = str_dup( ( char * ) value );
			break;
	}
}

/*
 * ustawia wartosc zmiennej, wersja dla parametru typu int
 */
void var_set_int_value( PROG_VAR * pvar, int value )
{
	char num[ MAX_INPUT_LENGTH ];

	if ( !pvar )
		return;

	pvar->type		= PROG_VAR_INT;
	sprintf( num, "%d", value );
	pvar->value.integer = str_dup( num );
}

/*
 * zwalnia pamiêæ zajmowan± przez zmienn±
 * MUSI BYC USUNIETA Z LISTY
 */
void free_prog_var( PROG_VAR * pvar )
{
	if ( !pvar )
		return;

	free_string( pvar->name );

	if ( pvar->type == PROG_VAR_STRING )
		free_string( pvar->value.string );

	if ( pvar->type == PROG_VAR_INT )
		free_string( pvar->value.integer );

	DISPOSE( pvar );
	return;
}

/*
 * wrzuca zmienn± na dno stosu (na koniec)
 * wiem ze stos inaczej dzia³a ale niech juz zostanie nazwa
 */
void push_var( PROG_VAR *pvar, PROG_VAR ** stack )
{
	PROG_VAR * last_pvar;

	if ( !pvar )
		return;

	if ( *stack == NULL )
	{
		pvar->next	= NULL;
		*stack		= pvar;
		return;
	}

	for ( last_pvar = *stack; last_pvar->next; last_pvar = last_pvar->next );

	last_pvar->next = pvar;
	pvar->next		= NULL;

	return;
}

/*
 * zwraca wskaznik na któr±¶ z kolei zmienn± w stosie
 * pierwsza zmienna ma numerek 1
 */
PROG_VAR * pop_var( PROG_VAR * stack, int number )
{
	PROG_VAR * ret;

	if ( stack == NULL || number <= 0 )
		return NULL;

	for ( ret = stack; number > 1; ret = ret->next, number-- );

	return ret;
}

/*
 * dodaje zmienn± na pocz±tek listy
 */
void var_to_list( PROG_VAR * pvar, PROG_VAR ** list )
{
	if ( !pvar )
		return;

	pvar->next = *list;
	*list = pvar;
	return;
}

/*
 * wywala zmienn± z listy i zwalnia j±
 */
void var_from_list( PROG_VAR * pvar, PROG_VAR ** list )
{
	if ( pvar == NULL || *list == NULL )
		return;

	if ( pvar == *list )
	{
		*list = pvar->next;
	}
	else
	{
		PROG_VAR * prev;

		for ( prev = *list; prev; prev = prev->next )
		{
			if ( prev->next == pvar )
			{
				prev->next = pvar->next;
				break;
			}
		}
	}

	free_prog_var( pvar );

	return;
}

/*
 * zwalnia wszystkie zmienne na li¶cie
 */
void free_var_list( PROG_VAR ** list )
{
	PROG_VAR *pvar, *pvar_next;

	for ( pvar = *list; pvar; pvar = pvar_next )
	{
		pvar_next = pvar->next;
		free_prog_var( pvar );
	}

	*list = NULL;
	return;
}

/*
 * zwalnia zmienne na li¶cie licz±c od poczatku listy a koñcz±c na tej podanej w parametrze stop (bez niej)
 */
void partial_free_var_list( PROG_VAR ** list, PROG_VAR * stop )
{
	PROG_VAR *pvar, *pvar_next;

	if ( !stop )
		return;

	for ( pvar = *list; pvar && pvar != stop; pvar = pvar_next )
	{
		pvar_next = pvar->next;
		free_prog_var( pvar );
	}

	if ( pvar )
		*list = stop;
	else
		*list = NULL;
	return;
}

/*
 * kopiuje zmienne z listy from do listy wskazywanej przez adres w to
 */
void copy_var_list( PROG_VAR *from, PROG_VAR ** to )
{
	PROG_VAR *pvar, *pvar_next, *pvar_new;

	for ( pvar = from; pvar; pvar = pvar_next )
	{
		pvar_next = pvar->next;
		pvar_new = copy_prog_var( pvar->name, pvar );
		var_to_list( pvar_new, to );
	}

	return;
}

/*
 * zlicza liczbê zmiennych w li¶cie
 */
int count_vars( PROG_VAR * list )
{
	int result = 0;

	PROG_VAR *pvar;

	for ( pvar = list; pvar; pvar = pvar->next )
		result++;

	return result;
}

/*
 * sprawdza czy dany string to poprawna deklaracja zmiennej
 * czyli ci±g znaków $_znaki_lub_cyfry
 */
bool validate_var_name( char * name )
{
	if ( strlen( name ) < 2 )
		return FALSE;

	if ( name[0] != '$' || name[1] != '_' )
		return FALSE;

	name += 2;

	while( *name )
	{
		if ( !isalpha( *name ) && !isdigit( *name ) )
			return FALSE;
		name++;
	}

	return TRUE;
}

/*
 * zwraca zmienn± o podanej nazwie znajduj±c± siê w podanej li¶cie
 */
PROG_VAR * get_var( char * name, PROG_VAR * list )
{
	PROG_VAR * pvar = NULL;
	PROG_VAR * temp_pvar = NULL;
	char attr[ MAX_INPUT_LENGTH ];

	if ( !list || IS_NULLSTR( name ) )
		return NULL;

	name = split_var_name( name, attr );

	for ( pvar = list; pvar; pvar = pvar->next )
	{
		if ( FAST_STR_CMP( pvar->name, name ) )
			break;
	}

	if ( pvar && !IS_NULLSTR( attr ) )
	{
		temp_pvar = var_attribute( pvar, attr );
		if ( temp_pvar )
		{
			var_to_list( temp_pvar, &temp_var_list );
			return temp_pvar;
		}
	}

	return pvar;
}

/*
 * zwraca warto¶æ typu CHAR_DATA podanej zmiennej
 * sprawdza czy zgadza siê typ oraz czy zmienna czasem nie jest NULL
 */
CHAR_DATA * get_char_var( PROG_VAR * pvar )
{
	return pvar && pvar->type == PROG_VAR_CHAR ? pvar->value.character : NULL;
}

/*
 * zwraca warto¶æ typu OBJ_DATA podanej zmiennej
 */
OBJ_DATA * get_obj_var( PROG_VAR * pvar )
{
	return pvar && pvar->type == PROG_VAR_OBJ ? pvar->value.object : NULL;
}

/*
 * zwraca warto¶æ typu ROOM_INDEX_DATA podanej zmiennej
 */
ROOM_INDEX_DATA * get_room_var( PROG_VAR * pvar )
{
	return pvar && pvar->type == PROG_VAR_ROOM ? pvar->value.room : NULL;
}

/*
 * zwraca warto¶æ typu string (char*) podanej zmiennej
 */
char * get_string_var( PROG_VAR * pvar )
{
	return pvar && pvar->type == PROG_VAR_STRING ? pvar->value.string : NULL;
}

/*
 * zwraca warto¶æ typu int podanej zmiennej
 */
int get_int_var( PROG_VAR * pvar )
{
	return pvar && pvar->type == PROG_VAR_INT ? atoi( (char*) pvar->value.integer ) : UNDEFINED_INT;
}

/*
 * sprawdza czy zmienna jest NULL lub ma warto¶æ NULL
 */
bool var_is_null( PROG_VAR * pvar )
{
	return pvar && pvar->value.string != NULL ? FALSE : TRUE;
}

/*
 * do wypisywania warto¶ci zmiennej, czyli zamiana zmiennej na string
 */
char * var_to_string( PROG_VAR * pvar )
{
	if ( !pvar || !pvar->value.string )
		return NULL;

	switch ( pvar->type )
	{
		case PROG_VAR_STRING:
			return pvar->value.string;
			break;
		case PROG_VAR_INT:
			return pvar->value.integer;
		case PROG_VAR_CHAR:
			return pvar->value.character->name;
		case PROG_VAR_OBJ:
			return pvar->value.object->name;
		case PROG_VAR_ROOM:
			return pvar->value.room->name;
			break;
	}

	return NULL;
}

char * split_var_name( char * argument, char * attr )
{
    char *pdot;

    attr[0] = '\0';

    if ( IS_NULLSTR( argument ) )
        return argument;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
	{
		if ( *pdot == '.' )
		{
		    *pdot = '\0';
		    strcpy( attr, pdot+1 );
		    return argument;
		}
	}

    return argument;
}

char *param_one_argument( char *argument, char *arg_first )
{
	char cEnd;
	bool bracket = FALSE;

	if ( argument == NULL )
	{
		arg_first[0] = '\0';
		return str_dup( "" );
	}

	while ( isspace(*argument) )
		argument++;

	cEnd = ' ';
	if ( *argument == '\'' || *argument == '"' )
	{
		cEnd = *argument;
		*arg_first = *argument;
		arg_first++;
		argument++;
	}

	while ( *argument != '\0' )
	{
		if ( cEnd == ' ' )
		{
			if ( *argument == ')' && !bracket )
			{
				*arg_first = *argument;
				arg_first++;
				argument++;
				bracket = TRUE;
				continue;
			}

			if ( *argument == ')' && bracket )
				break;
		}

		if ( *argument == cEnd )
		{
			*arg_first = *argument;
			if ( cEnd != ' ' )
			{
				arg_first++;
				argument++;
			}
			break;
		}
		*arg_first = *argument;
		arg_first++;
		argument++;
	}
	*arg_first = '\0';

	while ( isspace(*argument) )
		argument++;

	return argument;
}

/*
 * modyfikacja one_argument, tak ¿eby liczy³ nawiasy jako ograniczniki
 * oraz nie obcina³ spacji znajduj±cych siê po zmiennej
 */
char * grab_var_name( char *argument, char *arg_first )
{
	char cEnd;

	if ( argument == NULL )
	{
		arg_first[ 0 ] = '\0';
		return str_dup( "" );
	}

	while ( isspace( *argument ) )
		argument++;

	cEnd = ' ';
	if ( *argument == '\'' || *argument == '"' )
		cEnd = *argument++;

	if ( *argument == '(' )
	{
		cEnd = ')';
		argument++;
	}


	while ( *argument != '\0' )
	{
		if ( *argument == cEnd )
		{
			if ( *argument != ' ' )
				argument++;
			break;
		}
		*arg_first = LOWER( *argument );
		arg_first++;
		argument++;
	}
	*arg_first = '\0';

	return argument;
}

/*
 * sprawdza czy podany ci±g jest nazw± jakiej¶ komendy operuj±cej na zmiennych
 */
bool is_var_cmd( char * command )
{
	int cmd;

	if ( *command == '(' )
		command++;

	for ( cmd = 0; var_cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
	{
		if ( FAST_STR_CMP( command, var_cmd_table[ cmd ].name ) )
			return TRUE;
	}
	return FALSE;
}

/*
 * zwraca numer w tablicy komendy o podanej nazwie i podanych typach parametrów
 */
int get_var_cmd( char * command, PROG_VAR * vars )
{
	PROG_VAR * list = vars;
	int cmd;
	int var;
	bool ok = TRUE;

	if ( *command == '(' )
		command++;

	for ( cmd = 0; var_cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
	{
		if ( FAST_STR_CMP( command, var_cmd_table[ cmd ].name ) )
		{
			ok = TRUE;
			for ( var = 0, list = vars; var_cmd_table[ cmd ].args_type[var] != -1; var++, list = list->next )
			{
				if ( !list || list->type != var_cmd_table[ cmd ].args_type[var] )
				{
					ok = FALSE;
					break;
				}
			}

			if ( ok )
				return cmd;
		}
	}
	return -1;
}

bool smash_bracket( char * txt )
{
	int i;

	if ( IS_NULLSTR( txt ) )
		return FALSE;

	for( i = strlen( txt ) - 1; i >= 0 && txt[i] == ')'; i--)
	{
		txt[i] = '\0';
		return TRUE;
	}

	return FALSE;
}

/*
 * handler komend operuj±cych na zmiennych, konstruuje listê parametrów
 * sprawdza warunki oraz przypisuje wynik zmiennej podanej jako pierwszy parametr
 */
char * var_dispatch_cmd( PROG_VAR * lvar, char * command, char * args )
{
	PROG_VAR *var_args = NULL, *pvar = NULL;
	char arg[ MAX_INPUT_LENGTH ];
	int cmd;
	bool bracket;

	args = param_one_argument( args, arg );
	bracket = smash_bracket( arg );

	while( arg[0] )
	{
		if ( IS_NUMBER( arg ) )
		{
			pvar = new_prog_var( "", arg, PROG_VAR_INT );
			push_var( pvar, &var_args );
		}
		else if ( IS_VAR( arg ) )
		{
			pvar = copy_prog_var( "", get_var( arg + 2, running_prog_env->vars ) );
			push_var( pvar, &var_args );
		}
		else if ( IS_BUILT_IN_VAR( arg ) )
		{
			CHAR_DATA * var_char = resolve_char_arg( arg );
			OBJ_DATA * var_obj = resolve_obj_arg( arg );

			if ( var_char )
			{
				pvar = new_prog_var( "", var_char, PROG_VAR_CHAR );
				push_var( pvar, &var_args );
			}
			else if ( var_obj )
			{
				pvar = new_prog_var( "", var_obj, PROG_VAR_OBJ );
				push_var( pvar, &var_args );
			}
		}
		else if ( arg[0] == '(' && is_var_cmd( arg+1 ) )
		{
			pvar = new_prog_var( "", NULL, PROG_VAR_STRING );
			args = var_dispatch_cmd( pvar, arg+1, args );
			push_var( pvar, &var_args );
		}
		else if ( arg[0] == '"' || arg[0] == '\'' )
		{
			arg[strlen( arg ) - 1] = '\0';
			pvar = new_prog_var( "", arg+1, PROG_VAR_STRING );
			push_var( pvar, &var_args );
		}

		if ( bracket )
			break;

		args = param_one_argument( args, arg );
		bracket = smash_bracket( arg );
	}

	for ( pvar = var_args; pvar; pvar = pvar->next )
	{
		if ( var_is_null( pvar ) )
		{
			bug_prog_format( "var_dispatch_cmd: null parameter in command %s", command );
			free_var_list( &var_args );
			return args;
		}
	}

	if ( ( cmd = get_var_cmd( command, var_args ) ) < 0 )
	{
		bug_prog_format( "var_dispatch_cmd: wrong var command name %s or wrong parameter list", command );
		free_var_list( &var_args );
		return args;
	}

	pvar = ( *var_cmd_table[ cmd ].cmd_fun ) ( var_cmd_table[ cmd ].name, var_args );
	if ( pvar && ( var_cmd_table[ cmd ].ret_type == PROG_VAR_UNDEFINED || pvar->type == var_cmd_table[ cmd ].ret_type ) )
		var_copy_value( lvar, pvar );

	free_var_list( &var_args );
	free_prog_var( pvar );
	return args;
}

/* FUNKCJE OPERUJ¡CE NA ZMIENNYCH */

VAR_CMD( vc_int_add )
{
	char buf [ MAX_STRING_LENGTH ];
	PROG_VAR * arg1;
	PROG_VAR * arg2;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );

	sprintf( buf, "%d", get_int_var( arg1 ) + get_int_var( arg2 ) );

	return new_prog_var( "result", buf, PROG_VAR_INT );
}

VAR_CMD( vc_int_sub )
{
	char buf [ MAX_STRING_LENGTH ];
	PROG_VAR * arg1;
	PROG_VAR * arg2;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );

	sprintf( buf, "%d", get_int_var( arg1 ) - get_int_var( arg2 ) );

	return new_prog_var( "result", buf, PROG_VAR_INT );
}

VAR_CMD( vc_int_mult )
{
	char buf [ MAX_STRING_LENGTH ];
	PROG_VAR * arg1;
	PROG_VAR * arg2;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );

	sprintf( buf, "%d", get_int_var( arg1 ) * get_int_var( arg2 ) );

	return new_prog_var( "result", buf, PROG_VAR_INT );
}

VAR_CMD( vc_int_div )
{
	char buf [ MAX_STRING_LENGTH ];
	PROG_VAR * arg1;
	PROG_VAR * arg2;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );

	sprintf( buf, "%d", get_int_var( arg1 ) / get_int_var( arg2 ) );

	return new_prog_var( "result", buf, PROG_VAR_INT );
}

VAR_CMD( vc_int_mod )
{
	char buf [ MAX_STRING_LENGTH ];
	PROG_VAR * arg1;
	PROG_VAR * arg2;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );

	sprintf( buf, "%d", get_int_var( arg1 ) % get_int_var( arg2 ) );

	return new_prog_var( "result", buf, PROG_VAR_INT );
}

VAR_CMD( vc_int_random )
{
	char buf [ MAX_STRING_LENGTH ];
	PROG_VAR * arg1;
	PROG_VAR * arg2;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );

	sprintf( buf, "%d", number_range(get_int_var( arg1 ), get_int_var( arg2 ) ) );

	return new_prog_var( "result", buf, PROG_VAR_INT );
}

VAR_CMD( vc_room_by_vnum )
{
	PROG_VAR * arg1;

	arg1 = pop_var( args, 1 );

	return new_prog_var( "result", get_room_index( get_int_var( arg1 ) ), PROG_VAR_ROOM );
}

VAR_CMD( vc_room_by_char )
{
	CHAR_DATA * ch;
	PROG_VAR * arg1;

	arg1 = pop_var( args, 1 );

	ch = get_char_var( arg1 );

	if ( !ch )
		return NULL;

	return new_prog_var( "result", ch->in_room, PROG_VAR_ROOM );
}

VAR_CMD( vc_room_by_obj )
{
	OBJ_DATA * obj, * in_obj;
	ROOM_INDEX_DATA * pRoom;
	PROG_VAR * arg1;

	arg1 = pop_var( args, 1 );

	obj = get_obj_var( arg1 );

	if ( !obj )
		return NULL;

	pRoom = obj->in_room;

	if ( !pRoom )
	{
		for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj );

		if ( in_obj->carried_by )
			pRoom = in_obj->carried_by->in_room;
		else
			pRoom = in_obj->in_room;
	}

	if ( !pRoom )
		return NULL;

	return new_prog_var( "result", pRoom, PROG_VAR_ROOM );
}


VAR_CMD( vc_char_first_in_room )
{
	ROOM_INDEX_DATA * pRoom;
	PROG_VAR * arg1;

	arg1 = pop_var( args, 1 );

	pRoom = get_room_var( arg1 );

	if ( !pRoom )
		return NULL;

	if ( pRoom->people && pRoom->people->pIndexData && pRoom->people->pIndexData->vnum == 3 )
		return new_prog_var( "result", pRoom->people->next_in_room, PROG_VAR_CHAR );
	else
		return new_prog_var( "result", pRoom->people, PROG_VAR_CHAR );
}

VAR_CMD( vc_char_in_room_by_name )
{
	ROOM_INDEX_DATA * pRoom;
	CHAR_DATA * ch = NULL, * vch;
	char buf [ MAX_INPUT_LENGTH ];
	char * name;
	int number = 0;
	int count = 0;
	PROG_VAR * arg1;
	PROG_VAR * arg2;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );

	pRoom = get_room_var( arg1 );

	if ( !pRoom || !pRoom->people )
		return NULL;

	name = get_string_var( arg2 );

	if ( IS_NULLSTR( name ) )
		return NULL;

	number = number_argument( name, buf );
	count  = 0;
	for ( vch = pRoom->people; vch; vch = vch->next_in_room )
	{
		if ( !is_name( buf, vch->ss_data ? vch->short_descr : vch->name ) )
			continue;

		if ( ++count == number )
		{
			ch = vch;
			break;
		}
	}

	if ( !ch )
		return NULL;

	return new_prog_var( "result", ch, PROG_VAR_CHAR );
}

VAR_CMD( vc_char_in_room_by_vnum )
{
	ROOM_INDEX_DATA * pRoom;
	CHAR_DATA * ch = NULL, * vch;
	int vnum = 0;
	PROG_VAR * arg1;
	PROG_VAR * arg2;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );

	pRoom = get_room_var( arg1 );

	if ( !pRoom || !pRoom->people )
		return NULL;

	vnum = get_int_var( arg2 );

	if ( vnum == UNDEFINED_INT )
		return NULL;

	for ( vch = pRoom->people; vch; vch = vch->next_in_room )
	{
		if ( IS_NPC( vch ) && vch->pIndexData && vch->pIndexData->vnum == vnum )
		{
			ch = vch;
			break;
		}
	}

	if ( !ch )
		return NULL;

	return new_prog_var( "result", ch, PROG_VAR_CHAR );
}


VAR_CMD( vc_obj_first_carried )
{
	CHAR_DATA * ch;
	PROG_VAR * arg1;

	arg1 = pop_var( args, 1 );

	ch = get_char_var( arg1 );

	if ( !ch )
		return NULL;

	return new_prog_var( "result", ch->carrying, PROG_VAR_OBJ );
}

VAR_CMD( vc_obj_first_in_room )
{
	ROOM_INDEX_DATA * pRoom;
	PROG_VAR * arg1;

	arg1 = pop_var( args, 1 );

	pRoom = get_room_var( arg1 );

	if ( !pRoom )
		return NULL;

	return new_prog_var( "result", pRoom->contents, PROG_VAR_OBJ );
}

VAR_CMD( vc_obj_in_room_by_name )
{
	ROOM_INDEX_DATA * pRoom;
	OBJ_DATA * obj = NULL, * vobj;
	char buf [ MAX_INPUT_LENGTH ];
	char * name;
	int number = 0;
	int count = 0;
	PROG_VAR * arg1;
	PROG_VAR * arg2;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );

	pRoom = get_room_var( arg1 );

	if ( !pRoom || !pRoom->contents )
		return NULL;

	name = get_string_var( arg2 );

	if ( IS_NULLSTR( name ) )
		return NULL;

	number = number_argument( name, buf );
	count  = 0;
	for ( vobj = pRoom->contents; vobj; vobj = vobj->next_content )
	{
		if ( !is_name( buf, vobj->name ) )
			continue;

		if ( ++count == number )
		{
			obj = vobj;
			break;
		}
	}

	if ( !obj )
		return NULL;

	return new_prog_var( "result", obj, PROG_VAR_OBJ );
}

VAR_CMD( vc_obj_in_room_by_vnum )
{
	ROOM_INDEX_DATA * pRoom;
	OBJ_DATA * obj = NULL, * vobj;
	int vnum = 0;
	PROG_VAR * arg1;
	PROG_VAR * arg2;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );

	pRoom = get_room_var( arg1 );

	if ( !pRoom || !pRoom->contents )
		return NULL;

	vnum = get_int_var( arg2 );

	if ( vnum == UNDEFINED_INT )
		return NULL;

	for ( vobj = pRoom->contents; vobj; vobj = vobj->next_content )
	{
		if ( vobj->pIndexData && vobj->pIndexData->vnum == vnum )
		{
			obj = vobj;
			break;
		}
	}

	if ( !obj )
		return NULL;

	return new_prog_var( "result", obj, PROG_VAR_OBJ );
}

VAR_CMD( vc_obj_in_obj_by_vnum )
{
	ROOM_INDEX_DATA * pRoom;
	OBJ_DATA * obj = NULL, * vobj;
	int vnum = 0;
	PROG_VAR * arg1;
	PROG_VAR * arg2;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );

	pRoom = get_room_var( arg1 );

	if ( !pRoom || !pRoom->contents )
		return NULL;

	vnum = get_int_var( arg2 );

	if ( vnum == UNDEFINED_INT )
		return NULL;

	for ( vobj = pRoom->contents; vobj; vobj = vobj->next_content )
	{
		if ( vobj->pIndexData && vobj->pIndexData->vnum == vnum )
		{
			obj = vobj;
			break;
		}
	}

	if ( !obj || ( !obj && !obj->contains ) )
		return NULL;

	return new_prog_var( "result", obj->contains, PROG_VAR_OBJ );
}

VAR_CMD( vc_obj_char_eq_slot )
{
	CHAR_DATA * ch;
	PROG_VAR * arg1;
	PROG_VAR * arg2;
	int slot;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );

	ch = get_char_var( arg1 );

	if ( !ch )
		return NULL;

	if ( ( slot = flag_value( wear_loc_flags, get_string_var( arg2 ) ) ) == NO_FLAG )
		return NULL;

	return new_prog_var( "result", get_eq_char( ch, slot ), PROG_VAR_OBJ );
}

VAR_CMD( vc_str_concat )
{
	char buf [ MAX_STRING_LENGTH ];
	PROG_VAR * arg1;
	PROG_VAR * arg2;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );

	sprintf( buf, "%s%s", get_string_var( arg1 ), get_string_var( arg2 ) );

	return new_prog_var( "result", buf, PROG_VAR_STRING );
}

VAR_CMD( vc_str_substr )
{
	char buf [ MAX_STRING_LENGTH ];
	PROG_VAR * arg1;
	PROG_VAR * arg2;
	PROG_VAR * arg3;
	int start, len;
	char * str;
	char * p;

	arg1 = pop_var( args, 1 );
	arg2 = pop_var( args, 2 );
	arg3 = pop_var( args, 3 );

	str = get_string_var( arg1 );
	start = get_int_var( arg2 );
	len = get_int_var( arg3 );

	p = buf;

	while( start > 0 && *str )
	{
		str++;
		start--;
	}

	while( len > 0 && *str )
	{
		*p++ = *str++;
		len--;
	}

	*p = '\0';

	return new_prog_var( "result", buf, PROG_VAR_STRING );
}

VAR_CMD( vc_str_length )
{
	char buf [ MAX_STRING_LENGTH ];
	PROG_VAR * arg1;

	arg1 = pop_var( args, 1 );

	sprintf( buf, "%d", strlen( get_string_var( arg1 ) ) );

	return new_prog_var( "result", buf, PROG_VAR_INT );
}

VAR_CMD( vc_str_to_int )
{
	PROG_VAR * arg1;
	char * str;

	arg1 = pop_var( args, 1 );

	str = get_string_var( arg1 );

	if ( !is_number( str ) )
		return NULL;

	return new_prog_var( "result", str, PROG_VAR_INT );
}

VAR_CMD( vc_int_to_str )
{
	PROG_VAR * arg1;
	char str[ MAX_INPUT_LENGTH ];
	int i;

	arg1 = pop_var( args, 1 );

	i = get_int_var( arg1 );

	sprintf( str, "%d", i );
	return new_prog_var( "result", str, PROG_VAR_STRING );
}

VAR_CMD( vc_var_by_name )
{
	PROG_VAR * arg1;
	char * str;

	arg1 = pop_var( args, 1 );

	str = get_string_var( arg1 );

	return copy_prog_var( "result", get_var( str, running_prog_env->vars ) );
}

VAR_CMD( vc_common_next_in_list )
{
	CHAR_DATA * ch;
	OBJ_DATA * obj;
	PROG_VAR * arg1;

	arg1 = pop_var( args, 1 );

	switch ( arg1->type )
	{
		case PROG_VAR_CHAR:
			ch = get_char_var( arg1 );

			if ( !ch )
				return NULL;

			if ( ch->next_in_room && ch->next_in_room->pIndexData && ch->next_in_room->pIndexData->vnum == 3 )
				return new_prog_var( "result", ch->next_in_room->next_in_room, PROG_VAR_CHAR );
			else
				return new_prog_var( "result", ch->next_in_room, PROG_VAR_CHAR );
			break;
		case PROG_VAR_OBJ:
			obj = get_obj_var( arg1 );

			if ( !obj )
				return NULL;

			return new_prog_var( "result", obj->next_content, PROG_VAR_OBJ );
			break;
	}
	return NULL;
}

VAR_CMD( vc_common_flag_value )
{
	ROOM_INDEX_DATA * pRoom;
	CHAR_DATA * ch;
	OBJ_DATA * obj;
	PROG_VAR * arg1;
	PROG_VAR * arg_flag;
	PFLAG_DATA * flag = NULL, * pflag_list = NULL;
	char buf [ MAX_INPUT_LENGTH ];
	char * flag_name;

	arg1 = pop_var( args, 1 );

	if ( !str_cmp( vc_name, "global_flag_value" ) )
	{
		pflag_list = misc.global_flag_list;
		arg_flag = arg1;
	}
	else
	{
		switch ( arg1->type )
		{
			case PROG_VAR_ROOM:
				if ( ( pRoom = get_room_var( arg1 ) ) == NULL )
					return NULL;

				pflag_list = pRoom->pflag_list;

				arg_flag = pop_var( args, 2 );

				break;

			case PROG_VAR_CHAR:
				if ( ( ch = get_char_var( arg1 ) ) == NULL )
					return NULL;

				pflag_list = ch->pflag_list;

				arg_flag = pop_var( args, 2 );

				break;

			case PROG_VAR_OBJ:
				if ( ( obj = get_obj_var( arg1 ) ) == NULL )
					return NULL;

				pflag_list = obj->pflag_list;

				arg_flag = pop_var( args, 2 );

				break;
		}
	}

	flag_name = get_string_var( arg_flag );

	if ( IS_NULLSTR( flag_name ) )
		return NULL;

	for ( flag = pflag_list; flag; flag = flag->next )
	{
		if ( FAST_STR_CMP( flag->id, flag_name ) )
			break;
	}

	if ( !flag )
		return NULL;

	sprintf( buf, "%d", flag->duration );

	return new_prog_var( "result", buf, PROG_VAR_INT );
}

VAR_CMD ( vc_common_first_flag )
{
	ROOM_INDEX_DATA * pRoom;
	CHAR_DATA * ch;
	OBJ_DATA * obj;
	PROG_VAR * arg1;
	char * flag_name;

	if ( !str_cmp( vc_name, "global_first_flag" ) )
	{
		if ( !misc.global_flag_list )
			return NULL;
		flag_name = misc.global_flag_list->id;
	}
	else
	{
		arg1 = pop_var( args, 1 );

		switch ( arg1->type )
		{
			case PROG_VAR_ROOM:
				pRoom = get_room_var( arg1 );

				if ( !pRoom || !pRoom->pflag_list )
					return NULL;

				flag_name = pRoom->pflag_list->id;

				break;
			case PROG_VAR_CHAR:
				ch = get_char_var( arg1 );

				if ( !ch || !ch->pflag_list )
					return NULL;

				flag_name = ch->pflag_list->id;

				break;
			case PROG_VAR_OBJ:
				obj = get_obj_var( arg1 );

				if ( !obj || !obj->pflag_list )
					return NULL;

				flag_name = obj->pflag_list->id;

				break;
		}
	}

	if ( IS_NULLSTR( flag_name ) )
		return NULL;

	return new_prog_var( "result", flag_name, PROG_VAR_STRING );
}

VAR_CMD ( vc_common_next_flag )
{
	ROOM_INDEX_DATA * pRoom;
	CHAR_DATA * ch;
	OBJ_DATA * obj;
	PROG_VAR * arg1;
	PROG_VAR * arg_flag;
	PFLAG_DATA * flag = NULL, * pflag_list = NULL;
	char * flag_name;

	arg1 = pop_var( args, 1 );

	if ( !str_cmp( vc_name, "global_next_flag" ) )
	{
		pflag_list = misc.global_flag_list;
		arg_flag = arg1;
	}
	else
	{
		switch ( arg1->type )
		{
			case PROG_VAR_ROOM:
				if ( ( pRoom = get_room_var( arg1 ) ) == NULL )
					return NULL;

				pflag_list = pRoom->pflag_list;

				arg_flag = pop_var( args, 2 );

				break;

			case PROG_VAR_CHAR:
				if ( ( ch = get_char_var( arg1 ) ) == NULL )
					return NULL;

				pflag_list = ch->pflag_list;

				arg_flag = pop_var( args, 2 );

				break;

			case PROG_VAR_OBJ:
				if ( ( obj = get_obj_var( arg1 ) ) == NULL )
					return NULL;

				pflag_list = obj->pflag_list;

				arg_flag = pop_var( args, 2 );

				break;
		}
	}

	flag_name = get_string_var( arg_flag );

	if ( IS_NULLSTR( flag_name ) )
		return NULL;

	for ( flag = pflag_list; flag; flag = flag->next )
	{
		if ( FAST_STR_CMP( flag->id, flag_name ) )
			break;
	}

	if ( !flag->next )
		return NULL;

	return new_prog_var( "result", flag->next->id, PROG_VAR_STRING );
}

VAR_CMD( vc_datetime )
{
	PROG_VAR * arg1;
	char str[ MAX_INPUT_LENGTH ];
	char * type;
	int result = -1;

	arg1 = pop_var( args, 1 );
	type = get_string_var( arg1 );

	if ( !str_cmp( type, "year") )
		result = time_info.year;
	else if ( !str_cmp( type, "month") )
		result = time_info.month+1;
	else if ( !str_cmp( type, "week_day") )
		result = ( time_info.day + 1 ) % 7;
	else if ( !str_cmp( type, "month_day") )
		result = time_info.day + 1;
	else if ( !str_cmp( type, "hour") )
		result = time_info.hour;

	sprintf( str, "%d", result );
	return new_prog_var( "result", str, PROG_VAR_INT );
}

PROG_VAR * var_attribute( PROG_VAR * pvar, char * attr )
{
	CHAR_DATA * ch;
	OBJ_DATA * obj;
	ROOM_INDEX_DATA * room;
	char * string;
	PROG_VAR * temp_pvar = NULL;
	char full_name[ MAX_INPUT_LENGTH ];

	if ( !pvar || IS_NULLSTR( attr ) )
		return NULL;

	sprintf( full_name, "%s->%s", pvar->name, attr );
	temp_pvar = new_prog_var( full_name, "", PROG_VAR_STRING );
	temp_pvar->type = PROG_VAR_UNDEFINED;

	switch ( pvar->type )
	{
		case PROG_VAR_ROOM:
			if ( ( room = get_room_var( pvar ) ) == NULL )
				break;

			switch ( NOPOL( attr[0] ) )
			{
				case 'N':
					if ( !str_cmp( attr, "name" ) )
					{
						var_set_value( temp_pvar, room->name, PROG_VAR_STRING );
					}
					break;
				case 'S':
					if ( !str_cmp( attr, "sector" ) )
					{
						var_set_value( temp_pvar, sector_table[room->sector_type].name, PROG_VAR_STRING);
					}
					break;
				case 'V':
					if ( !str_cmp( attr, "vnum" ) )
					{
						var_set_int_value( temp_pvar, room->vnum );
					}
					break;
			}
			break;

		case PROG_VAR_CHAR:
			if ( ( ch = get_char_var( pvar ) ) == NULL )
				break;

			switch ( NOPOL( attr[0] ) )
			{
				case 'B':
					if ( !str_cmp( attr, "biernik" ) )
					{
						var_set_value( temp_pvar, ch->name4, PROG_VAR_STRING );
					}
					break;
				case 'C':
					if ( !str_cmp( attr, "cha" ) )
					{
						var_set_int_value( temp_pvar, get_curr_stat_deprecated( ch, STAT_CHA ) );
					}
					else if ( !str_cmp( attr, "con" ) )
					{
						var_set_int_value( temp_pvar, get_curr_stat_deprecated( ch, STAT_CON ) );
					}
					else if ( !str_cmp( attr, "celownik" ) )
					{
						var_set_value( temp_pvar, ch->name3, PROG_VAR_STRING );
					}
					break;
				case 'D':
					if ( !str_cmp( attr, "dex" ) )
					{
						var_set_int_value( temp_pvar, get_curr_stat_deprecated( ch, STAT_DEX ) );
					}
					else if ( !str_cmp( attr, "dopelniacz" ) )
					{
						var_set_value( temp_pvar, ch->name2, PROG_VAR_STRING );
					}
					break;
				case 'H':
					if ( !str_cmp( attr, "hp" ) )
					{
						var_set_int_value( temp_pvar, (int)ch->hit );
					}
					else if ( !str_cmp( attr, "hpmax" ) )
					{
						var_set_int_value( temp_pvar, get_max_hp( ch ) );
					}
					break;										
				case 'I':
					if ( !str_cmp( attr, "int" ) )
					{
						var_set_int_value( temp_pvar, get_curr_stat_deprecated( ch, STAT_INT ) );
					}
					break;
				case 'L':
					if ( !str_cmp( attr, "luc" ) )
					{
						var_set_int_value( temp_pvar, get_curr_stat_deprecated( ch, STAT_LUC ) );
					}
					else if ( !str_cmp( attr, "long" ) )
					{
						var_set_value( temp_pvar, ch->long_descr, PROG_VAR_STRING );
					}
					break;
				case 'M':
					if ( !str_cmp( attr, "miejscownik" ) )
					{
						var_set_value( temp_pvar, ch->name6, PROG_VAR_STRING );
					}
					break;
				case 'N':
					if ( !str_cmp( attr, "name" ) )
					{
						var_set_value( temp_pvar, ch->name, PROG_VAR_STRING );
					}
					else if ( !str_cmp( attr, "narzednik" ) )
					{
						var_set_value( temp_pvar, ch->name5, PROG_VAR_STRING );
					}
					break;
				case 'S':
					if ( !str_cmp( attr, "str" ) )
					{
						var_set_int_value( temp_pvar, get_curr_stat_deprecated( ch, STAT_STR ) );
					}
					else if ( !str_cmp( attr, "short" ) )
					{
						var_set_value( temp_pvar, ch->short_descr, PROG_VAR_STRING );
					}
					else if ( !str_cmp( attr, "silver" ) )
					{
						var_set_int_value( temp_pvar, ch->silver );
					}
					break;
				case 'W':
					if ( !str_cmp( attr, "wis" ) )
					{
						var_set_int_value( temp_pvar, get_curr_stat_deprecated( ch, STAT_WIS ) );
					}
					break;
				case 'V':
					if ( !str_cmp( attr, "vnum" ) )
					{
						if ( IS_NPC( ch ) && ch->pIndexData )
							var_set_int_value( temp_pvar, ch->pIndexData->vnum );
					}
					break;
			}
			break;

		case PROG_VAR_OBJ:
			if ( ( obj = get_obj_var( pvar ) ) == NULL )
				break;

			switch ( NOPOL( attr[0] ) )
			{
				case 'B':
					if ( !str_cmp( attr, "biernik" ) )
					{
						var_set_value( temp_pvar, obj->name4, PROG_VAR_STRING );
					}
					break;
				case 'C':
					if ( !str_cmp( attr, "cost" ) )
					{
						var_set_int_value( temp_pvar, obj->cost );
					}
					else if ( !str_cmp( attr, "condition" ) )
					{
						var_set_int_value( temp_pvar, obj->condition );
					}
					else if ( !str_cmp( attr, "celownik" ) )
					{
						var_set_value( temp_pvar, obj->name3, PROG_VAR_STRING );
					}
					break;
				case 'D':
					if ( !str_cmp( attr, "dopelniacz" ) )
					{
						var_set_value( temp_pvar, obj->name2, PROG_VAR_STRING );
					}
					break;
				case 'L':
					if ( !str_cmp( attr, "long" ) )
					{
						var_set_value( temp_pvar, obj->description, PROG_VAR_STRING );
					}
					break;
				case 'M':
					if ( !str_cmp( attr, "material" ) )
					{
						var_set_value( temp_pvar, material_table[ obj->material ].name, PROG_VAR_STRING );
					}
					else if ( !str_cmp( attr, "miejscownik" ) )
					{
						var_set_value( temp_pvar, obj->name6, PROG_VAR_STRING );
					}
					break;
				case 'N':
					if ( !str_cmp( attr, "name" ) )
					{
						var_set_value( temp_pvar, obj->name, PROG_VAR_STRING );
					}
					else if ( !str_cmp( attr, "narzednik" ) )
					{
						var_set_value( temp_pvar, obj->name5, PROG_VAR_STRING );
					}
					break;
				case 'R':
					if ( !str_cmp( attr, "rent" ) )
					{
						if ( obj->pIndexData )
							var_set_int_value( temp_pvar, obj->rent_cost );
					}
					break;
				case 'S':
					if ( !str_cmp( attr, "short" ) )
					{
						var_set_value( temp_pvar, obj->short_descr, PROG_VAR_STRING );
					}
					break;
				case 'T':
					if ( !str_cmp( attr, "type" ) )
					{
						var_set_value( temp_pvar, item_name( obj->item_type ), PROG_VAR_STRING );
					}
					break;
				case 'V':
					if ( !str_cmp( attr, "vnum" ) )
					{
						if ( obj->pIndexData )
							var_set_int_value( temp_pvar, obj->pIndexData->vnum );
					}
					else if ( !str_cmp( attr, "value0" ) )
					{
							var_set_int_value( temp_pvar, obj->value[0] );
					}
					else if ( !str_cmp( attr, "value1" ) )
					{
							var_set_int_value( temp_pvar, obj->value[1] );
					}
					else if ( !str_cmp( attr, "value2" ) )
					{
							var_set_int_value( temp_pvar, obj->value[2] );
					}
					else if ( !str_cmp( attr, "value3" ) )
					{
							var_set_int_value( temp_pvar, obj->value[3] );
					}
					else if ( !str_cmp( attr, "value4" ) )
					{
							var_set_int_value( temp_pvar, obj->value[4] );
					}
					else if ( !str_cmp( attr, "value5" ) )
					{
							var_set_int_value( temp_pvar, obj->value[5] );
					}
					else if ( !str_cmp( attr, "value6" ) )
					{
							var_set_int_value( temp_pvar, obj->value[6] );
					}
					break;
				case 'W':
					if ( !str_cmp( attr, "weight" ) )
					{
						var_set_int_value( temp_pvar, obj->weight );
					}
					else if ( !str_cmp( attr, "wear_loc" ) )
					{
						var_set_value( temp_pvar, flag_string( wear_loc_flags, obj->wear_loc ), PROG_VAR_STRING );
					}
					break;
			}
			break;

		case PROG_VAR_STRING:
			if ( ( string = get_string_var( pvar ) ) == NULL )
				break;

			switch ( NOPOL( attr[0] ) )
			{
				case 'L':
					if ( !str_cmp( attr, "len" ) || !str_cmp( attr, "length" ) )
					{
						var_set_int_value( temp_pvar, strlen( string ) );
					}
					break;
			}
			break;
	}

	if ( temp_pvar->type == PROG_VAR_UNDEFINED )
	{
		free_prog_var( temp_pvar );
		return NULL;
	}

	return temp_pvar;
}
