/************************************************************************
*                                                                      *
* KILLER MUD is copyright 1999-2011 Killer MUD Staff (alphabetical)    *
*                                                                      *
*    Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti  ] *
*    Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor   ] *
*    Pietrzak Marcin       (marcin.pietrzak@mud.pl        ) [Gurthg  ] *
*    Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas  ] *
*    Skrzetnicki Krzysztof (tener@tenet.pl                ) [Tener   ] *
*    Trebicki Marek        (maro@killer.radom.net         ) [Maro    ] *
*    Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron   ] *
*                                                                      *
************************************************************************/
/*
$Id: flags.c 10701 2011-12-02 16:03:39Z illi $
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
#include "merc.h"
#include "recycle.h"
#include "progs.h"

bool rem_flag( PFLAG_DATA **list, char *argument )
{
	PFLAG_DATA * tmp, *org_list, *tmp_prev;
	bool result = FALSE;

	org_list = *list;
	tmp = org_list;

	if ( !tmp )
		return FALSE;

	if ( !str_cmp( tmp->id, argument ) )
	{
		org_list = org_list->next;
		free_pflag( tmp );
		tmp = NULL;
		*list = org_list;
		result = TRUE;
	}
	else
	{

		tmp_prev = tmp;

		for ( ;tmp;tmp = tmp->next )
		{
			if ( !str_cmp( tmp->id, argument ) )
			{
				tmp_prev->next = tmp->next;
				tmp->next = NULL;
				free_pflag( tmp );
				tmp = NULL;
				result = TRUE;
				break;
			}

			tmp_prev = tmp;
		}
	}

	if ( *list == misc.global_flag_list )
		save_misc_data();

	return result;
}

PFLAG_DATA * add_flag( PFLAG_DATA **list, char *id, int duration )
{
	PFLAG_DATA * tmp;

	if ( !id || id[ 0 ] == '\0' )
		return NULL;

	if ( strlen( id ) > 32 )
		return NULL;

	if ( *list )
	{
		for ( tmp = *list;tmp;tmp = tmp->next )
		{
			if ( !str_cmp( tmp->id, id ) )
			{
				if ( tmp->duration != -1 )
					tmp->duration = duration;

				return tmp;
			}
		}
	}

	tmp = new_pflag();
	tmp->id = str_dup( id );
	tmp->duration = duration;
	tmp->next = NULL;

	tmp->next = ( *list );
	*list = tmp;

	return tmp;
}


bool setcharflag( CHAR_DATA *ch, char *argument )
{
	int counter;
	char arg[ MAX_INPUT_LENGTH ];

	if ( argument[ 0 ] == '\0' || !ch )
		return FALSE;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
		return FALSE;

	if ( argument[ 0 ] != '\0' && is_number( argument ) )
	{
		counter = atoi( argument );
		counter = UMAX( -1, counter );
	}
	else
		counter = 0;

	if ( !add_flag( &ch->pflag_list, arg, counter ) )
		return FALSE;

	return TRUE;
}

bool removecharflag( CHAR_DATA *ch, char *argument )
{
	if ( argument[ 0 ] == '\0' || !ch )
		return FALSE;

	return rem_flag( &ch->pflag_list, argument );
}

bool setobjflag( OBJ_DATA *obj, char *argument )
{
	int counter;
	char arg[ MAX_INPUT_LENGTH ];

	if ( argument[ 0 ] == '\0' || !obj )
		return FALSE;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
		return FALSE;

	if ( argument[ 0 ] != '\0' && is_number( argument ) )
	{
		counter = atoi( argument );
		counter = UMAX( -1, counter );
	}
	else
		counter = 0;

	if ( !add_flag( &obj->pflag_list, arg, counter ) )
		return FALSE;

	return TRUE;
}

bool removeobjflag( OBJ_DATA *obj, char *argument )
{
	if ( argument[ 0 ] == '\0' || !obj )
		return FALSE;

	return rem_flag( &obj->pflag_list, argument );
}

bool setroomflag( ROOM_INDEX_DATA *room, char *argument )
{
	int counter;
	char arg[ MAX_INPUT_LENGTH ];

	if ( argument[ 0 ] == '\0' || !room )
		return FALSE;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
		return FALSE;

	if ( argument[ 0 ] != '\0' && is_number( argument ) )
	{
		counter = atoi( argument );
		counter = UMAX( -1, counter );
	}
	else
		counter = 0;

	if ( !add_flag( &room->pflag_list, arg, counter ) )
		return FALSE;

	if ( arg[ 0 ] == 'r' && arg[ 1 ] == 'e' && arg[ 2 ] == 'g' )
		return TRUE;

	new_room_update( room, TYPE_PFLAG, TRUE );
	return TRUE;
}

bool removeroomflag( ROOM_INDEX_DATA *room, char *argument )
{
	if ( argument[ 0 ] == '\0' || !room )
		return FALSE;

	return rem_flag( &room->pflag_list, argument );
}

bool setglobalflag( char *argument )
{
	int counter;
	char arg[ MAX_INPUT_LENGTH ];

	if ( argument[ 0 ] == '\0' )
		return FALSE;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
		return FALSE;

	if ( argument[ 0 ] != '\0' && is_number( argument ) )
	{
		counter = atoi( argument );
		counter = UMAX( -1, counter );
	}
	else
		counter = 0;

	if ( !add_flag( &misc.global_flag_list, arg, counter ) )
		return FALSE;

	save_misc_data();

	if ( arg[ 0 ] == 'r' && arg[ 1 ] == 'e' && arg[ 2 ] == 'g' )
		return TRUE;

	return TRUE;
}

bool removeglobalflag( char *argument )
{
	if ( argument[ 0 ] == '\0' )
		return FALSE;

	return rem_flag( &misc.global_flag_list, argument );
}

bool flag_exists( char *buf, PFLAG_DATA *plist )
{
	PFLAG_DATA * flag;

	for ( flag = plist; flag; flag = flag->next )
		if ( NOPOL( buf[ 0 ] ) == NOPOL( flag->id[ 0 ] ) && !str_cmp( buf, flag->id ) )
			return TRUE;

	return FALSE;
}

void update_global_flags()
{
	PFLAG_DATA * global_flag, * global_flag_next;

	for ( global_flag = misc.global_flag_list ; global_flag ; global_flag = global_flag_next )
	{
		global_flag_next = global_flag->next;

		if ( NOPOL ( global_flag->id[ 0 ] ) == 'R' &&
		     NOPOL ( global_flag->id[ 1 ] ) == 'E' &&
		     NOPOL ( global_flag->id[ 2 ] ) == 'G' )
			continue;

		if ( global_flag->duration > 0 )
		{
			global_flag->duration--;

			if ( global_flag->duration == 0 )
				removeglobalflag( global_flag->id );
			else
				save_misc_data();
		}

	}
}
