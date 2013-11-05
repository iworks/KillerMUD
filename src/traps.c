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
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: traps.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/traps.c $
 *
 */
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "tables.h"
#include "progs.h"
#include "traps.h"
#include "recycle.h"

const struct trap_type trap_type_table[] =
{
    { "unlock",		TRAP_UNLOCK		},
    { "open",		TRAP_OPEN		},
    { "wear",		TRAP_WEAR		},
    { "drop",		TRAP_DROP		},
    { "get",		TRAP_GET		},
    { "pickok",		TRAP_PICKOK		},
    { "pickfailed",	TRAP_PICKFAILED	},
    { NULL, 		0				}
};


int trap_limit( int level, int dex, int skill )
{
	int ret_val = 0;

	ret_val = ( level & 0xff ) + ( ( dex & 0xff ) << 8 ) + ( ( skill & 0xff ) << 16 );
	return ret_val;
}

void set_trap_limit( TRAP_DATA *trap, int limit, int value )
{
	char * ptr;

	switch ( limit )
	{
		case LIMIT_DEX:
			ptr = ( char * ) & trap->limit;
			ptr++;
			*ptr = URANGE( 3, value, 27 ) & 0xff;
			break;

		case LIMIT_SKILL:
			ptr = ( char * ) & trap->limit;
			ptr += 2;
			*ptr = URANGE( 1, value, 99 ) & 0xff;
			break;

		case LIMIT_LEVEL:
			ptr = ( char * ) & trap->limit;
			*ptr = URANGE( 1, value, 29 ) & 0xff;
			break;
		default: return ;
	}
	return ;
}

int get_limit( TRAP_DATA * trap , int limit )
{
	if ( !trap || trap->limit == 0 )
		return 0;
	switch ( limit )
	{
		case LIMIT_DEX: return ( ( trap->limit >> 8 ) & 0xff );
		case LIMIT_SKILL: return ( ( trap->limit >> 16 ) & 0xff );
		case LIMIT_LEVEL: return ( trap->limit & 0xff );
		default: return 0;
	}

	return ( trap->limit & 0xff );
}





bool room_trap_handler( CHAR_DATA *ch )
{
	TRAP_DATA * trap;
	int chance = 50;
	char buf[ MAX_INPUT_LENGTH ];

	if ( IS_NPC( ch ) )
		return FALSE;

	if ( !ch || !ch->in_room )
		return FALSE;

	if ( ( trap = get_trap_index( ch->in_room->trap ) ) == NULL )
		return FALSE;

	if ( !trap->active || !EXT_IS_SET( ch->in_room->room_flags, ROOM_TRAP ) )
		return FALSE;

	chance += 5 * trap->level;

	switch ( ch->class )
	{
		case CLASS_THIEF:
			chance -= 2*ch->level;
			chance -= UMAX( 0, get_curr_stat_deprecated( ch, STAT_DEX ) - 18 ) *5;
			break;
		default: chance -= ch->level;break;
	}

	chance = URANGE( 10, chance, 95 );

	if ( number_percent() > chance )
		return FALSE;

	if ( !supermob )
		init_supermob();

	char_from_room ( supermob );
	char_to_room( supermob, ch->in_room );
	EXT_REMOVE_BIT( ch->in_room->room_flags, ROOM_TRAP );

	sprintf( buf, "trap%d", trap->vnum );
	create_rprog_env( buf, trap->triggered, ch->in_room, ch, NULL, NULL, NULL, NULL );
	program_flow();
	release_supermob();
	return TRUE;
}

bool room_exit_trap_handler( CHAR_DATA *ch, EXIT_DATA *pexit, int type )
{
	TRAP_DATA * trap;
	char buf[ MAX_INPUT_LENGTH ];

	if ( !ch || !ch->in_room || !pexit || pexit->trap <= 0 )
		return FALSE;

	if ( ( trap = get_trap_index( pexit->trap ) ) == NULL )
		return FALSE;

	if ( !trap->active ||
	     !IS_SET( trap->type, type ) ||
	     !IS_SET( pexit->exit_info, EX_TRAP ) )
		return FALSE;

	if ( !supermob )
		init_supermob();

	char_from_room ( supermob );
	char_to_room( supermob, ch->in_room );
	REMOVE_BIT( pexit->exit_info, EX_TRAP );

	sprintf( buf, "trap%d", trap->vnum );
	create_rprog_env( buf, trap->triggered, ch->in_room, ch, NULL, NULL, NULL, NULL );
	program_flow();
	release_supermob();
	return TRUE;
}

/* odpala trapy na obiektach, jesli powinien */
bool obj_trap_handler( CHAR_DATA *ch, OBJ_DATA *obj, int type )
{
	TRAP_DATA * trap;
	char buf[ MAX_INPUT_LENGTH ];

	if ( !ch || !ch->in_room || obj->trap <= 0 )
		return FALSE;

	if ( ( trap = get_trap_index( obj->trap ) ) == NULL )
		return FALSE;

	if ( !trap->active || !IS_SET( trap->type, type ) )
		return FALSE;

	if ( !supermob )
		init_supermob();

	char_from_room ( supermob );
	char_to_room( supermob, ch->in_room );

	obj->trap = 0;

	sprintf( buf, "trap%d", trap->vnum );
	create_rprog_env( buf, trap->triggered, ch->in_room, ch, obj, NULL, NULL, NULL );
	program_flow();
	release_supermob();
	/* jednorazowo!!! */
	return TRUE;
}

bool check_limits( TRAP_DATA *trap, CHAR_DATA *ch )
{
	int limit_dex = 0, limit_skill = 0, limit_level = 0;

	if ( trap->limit == 0 )
		return TRUE;

	limit_skill = get_limit( trap, LIMIT_SKILL );
	limit_dex = get_limit( trap, LIMIT_DEX );
	limit_level = get_limit( trap, LIMIT_LEVEL );

	if ( limit_level > ch->level ||
	     limit_skill > get_skill( ch, gsn_disarm_traps ) ||
	     limit_dex > get_curr_stat_deprecated( ch, STAT_DEX ) )
		return FALSE;

	return TRUE;
}

void save_traps( FILE *fp, AREA_DATA *pArea )
{
	TRAP_DATA * trap;
	int i;

	fprintf( fp, "#TRAPS\n" );

	for ( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
	{
		if ( ( trap = get_trap_index( i ) ) != NULL )
		{
			fprintf( fp, "#Vnum %d\n", trap->vnum );
			fprintf( fp, "Active %d\n", trap->active );
			fprintf( fp, "Level %d\n", trap->level );
			fprintf( fp, "Type %d\n", trap->type );
			fprintf( fp, "Triggered %s~\n", trap->triggered );
			fprintf( fp, "Disarmed %s~\n", trap->disarmed );
			fprintf( fp, "Failed %s~\n", trap->failed_disarm );
			fprintf( fp, "Limit %d\n", trap->limit );
			fprintf( fp, "Trapend\n" );
		}
	}

	fprintf( fp, "#Vnum 0\n\n" );
	return ;
}

