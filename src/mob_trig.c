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
 * KILLER MUD is copyright 2009-2011 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: mob_trig.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/mob_trig.c $
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <ctype.h>
#include "merc.h"
#include "tables.h"
#include "progs.h"

int pos_to_flagpos[] =
    {
        FP_DEAD,
        FP_MORTAL,
        FP_INCAP,
        FP_STUNNED,
        FP_SLEEPING,
        FP_RESTING,
        FP_SITTING,
        FP_FIGHTING,
        FP_STANDING,
        0
    };

/*
 * A general purpose string trigger. Matches argument to a string trigger
 * phrase.
 */
void mp_act_trigger( char *argument, CHAR_DATA *mob, CHAR_DATA *ch,
                     void *arg1, void *arg2, BITVECT_DATA *type )
{
	char buf [ MAX_STRING_LENGTH ];
	PROG_LIST * prg;

	for ( prg = mob->pIndexData->progs; prg != NULL; prg = prg->next )
	{
		if ( !IS_SET( prg->valid_positions, pos_to_flagpos[ mob->position ] ) )
			continue;
		if ( prg->trig_type == type )
		{
			if ( prg->trig_phrase[ 0 ] == '@' )
			{
				sprintf( buf, "%s\n", argument );
				if ( !regexp_match( prg->trig_phrase + 1, buf ) )
					continue;
			}
			else if ( prg->trig_phrase[ 0 ] == '*' )
			{
				if ( str_prefix( argument, prg->trig_phrase + 1 ) )
					continue;
			}
			else
			{
				if ( str_infix( prg->trig_phrase, argument ) )
					continue;
			}

			create_mprog_env( prg->name, prg->code, mob, ch, arg1, arg2, prg->trig_type, argument );
			program_flow();
			break;
		}
	}
	return ;
}

/*
 * A general purpose percentage trigger. Checks if a random percentage
 * number is less than trigger phrase
 */
bool mp_percent_trigger( CHAR_DATA *mob, CHAR_DATA *ch,
                         void *arg1, void *arg2, BITVECT_DATA *type )
{
	PROG_LIST * prg;


	for ( prg = mob->pIndexData->progs; prg != NULL; prg = prg->next )
	{
		if ( !IS_SET( prg->valid_positions, pos_to_flagpos[ mob->position ] ) )
			continue;
		if ( prg->trig_type == type
		     && number_percent() <= atoi( prg->trig_phrase ) )
		{
			create_mprog_env( prg->name, prg->code, mob, ch, arg1, arg2, prg->trig_type, prg->trig_phrase );
			program_flow();
			return ( TRUE );
		}
	}
	return ( FALSE );
}

void mp_bribe_trigger( CHAR_DATA *mob, CHAR_DATA *ch, long int amount )
{
	PROG_LIST * prg;
	PROG_LIST *max = NULL;
	char buf[ MAX_STRING_LENGTH ];
	int maxmoney, phrase;

	maxmoney = 0;

	for ( prg = mob->pIndexData->progs; prg; prg = prg->next )
	{
		if ( !IS_SET( prg->valid_positions, pos_to_flagpos[ mob->position ] ) )
			continue;

		phrase = atoi( prg->trig_phrase );
		if ( prg->trig_type ==  &TRIG_BRIBE
		     && amount >= phrase && phrase > maxmoney )
		{
			max = prg;
			maxmoney = phrase;
		}
	}

	if ( max )
	{
		sprintf( buf, "%ld", amount );
		create_mprog_env( max->name, max->code, mob, ch, NULL, NULL, &TRIG_BRIBE, buf );
		program_flow();
	}
	return ;
}

bool mp_exit_trigger( CHAR_DATA *ch, int dir )
{
	CHAR_DATA * mob;
	PROG_LIST *prg;

	for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
	{
		if ( IS_NPC( mob )
		     && ( HAS_TRIGGER( mob, TRIG_EXIT ) || HAS_TRIGGER( mob, TRIG_EXALL ) ) )
		{
			for ( prg = mob->pIndexData->progs; prg; prg = prg->next )
			{
				/*
				 * Exit trigger works only if the mobile is not busy
				 * (fighting etc.). If you want to be sure all players
				 * are caught, use ExAll trigger
				 */
				if ( !IS_SET( prg->valid_positions, pos_to_flagpos[ mob->position ] ) )
					continue;

				if ( prg->trig_type == &TRIG_EXIT
				     && dir == atoi( prg->trig_phrase )
				     && can_see( mob, ch ) )
				{
					create_mprog_env( prg->name, prg->code, mob, ch, NULL, NULL, prg->trig_type, NULL );
					program_flow();
					return TRUE;
				}
				else if ( prg->trig_type == &TRIG_EXALL
				          && dir == atoi( prg->trig_phrase ) )
				{
					if ( !IS_SET( prg->valid_positions, pos_to_flagpos[ mob->position ] ) )
						continue;

					create_mprog_env( prg->name, prg->code, mob, ch, NULL, NULL, prg->trig_type, NULL );
					program_flow();
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void mp_give_trigger( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj )
{

	char buf[ MAX_INPUT_LENGTH ], *p;
	PROG_LIST *prg;

	for ( prg = mob->pIndexData->progs; prg; prg = prg->next )
	{
		if ( !IS_SET( prg->valid_positions, pos_to_flagpos[ mob->position ] ) )
			continue;

		if ( prg->trig_type == &TRIG_GIVE )
		{
			p = prg->trig_phrase;
			/*
			 * Vnum argument
			 */
			if ( is_number( p ) )
			{
				if ( obj->pIndexData->vnum == atoi( p ) )
				{
					create_mprog_env( prg->name, prg->code, mob, ch,(  void * ) obj, NULL, prg->trig_type, NULL );
					program_flow();
					return ;
				}
			}
			/*
			 * Object name argument, e.g. 'sword'
			 */
			else
			{
				while ( *p )
				{
					p = one_argument( p, buf );

					if ( is_name( buf, obj->name )
					     || !str_cmp( "all", buf ) )
					{
						create_mprog_env( prg->name, prg->code, mob, ch,(  void * ) obj, NULL, prg->trig_type, NULL );
						program_flow();
						return ;
					}
				}
			}
		}
	}
}

void mp_greet_trigger( CHAR_DATA *ch )
{
	CHAR_DATA * mob, * mob_next;

	for ( mob = ch->in_room->people; mob != NULL; mob = mob_next )
	{
		mob_next = mob->next_in_room;

		if ( IS_NPC( mob )
		     && ( HAS_TRIGGER( mob, TRIG_GREET ) || HAS_TRIGGER( mob, TRIG_GRALL ) ) )
		{
			/*
			 * Greet trigger works only if the mobile is not busy
			 * (fighting etc.). If you want to catch all players, use
			 * GrAll trigger
			 */
			if ( HAS_TRIGGER( mob, TRIG_GREET )
			     && can_see( mob, ch ) )
				mp_percent_trigger( mob, ch, NULL, NULL, &TRIG_GREET );
			else
				if ( HAS_TRIGGER( mob, TRIG_GRALL ) )
					mp_percent_trigger( mob, ch, NULL, NULL, &TRIG_GRALL );
		}
	}
	return ;
}

void mp_hprct_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{
	PROG_LIST * prg;

	for ( prg = mob->pIndexData->progs; prg != NULL; prg = prg->next )
	{
		if ( !IS_SET( prg->valid_positions, pos_to_flagpos[ mob->position ] ) )
			continue;
		if ( prg->trig_type == &TRIG_HPCNT
		     && ( ( 100 * mob->hit / get_max_hp(mob) ) < atoi( prg->trig_phrase ) ) )
		{
			create_mprog_env( prg->name, prg->code, mob, ch, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			break;
		}
	}
}

void mp_time_trigger( CHAR_DATA *mob, int time )
{
	PROG_LIST * prg;


	for ( prg = mob->pIndexData->progs; prg; prg = prg->next )
	{
		if ( !IS_SET( prg->valid_positions, pos_to_flagpos[ mob->position ] ) )
			continue;
		if ( prg->trig_type == &TRIG_TIME
		     && time == atoi( prg->trig_phrase ) )
		{
			create_mprog_env( prg->name, prg->code, mob, NULL, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			break;
		}
	}
	return ;
}

void mp_onload_trigger( CHAR_DATA *mob )
{
	PROG_LIST * prg;

	for ( prg = mob->pIndexData->progs; prg; prg = prg->next )
	{
		if ( !IS_SET( prg->valid_positions, pos_to_flagpos[ mob->position ] ) )
			continue;
		if ( prg->trig_type == &TRIG_ONLOAD
		     && number_percent() <= atoi( prg->trig_phrase ) )
		{
			create_mprog_env( prg->name, prg->code, mob, NULL, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			break;
		}
	}
	return ;
}

bool mp_onehit_trigger( CHAR_DATA *mob, CHAR_DATA *victim )
{
	PROG_LIST * prg;

	for ( prg = mob->pIndexData->progs; prg; prg = prg->next )
	{
		if ( !IS_SET( prg->valid_positions, pos_to_flagpos[ mob->position ] ) )
			continue;
		if ( prg->trig_type == &TRIG_ONEHIT && number_percent() <= atoi( prg->trig_phrase ) )
		{
			create_mprog_env( prg->name, prg->code, mob, victim, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			return TRUE;
		}
	}
	return FALSE;
}

bool mp_hit_trigger( CHAR_DATA *mob, CHAR_DATA *victim )
{
	PROG_LIST * prg;

	for ( prg = mob->pIndexData->progs; prg; prg = prg->next )
	{
		if ( !IS_SET( prg->valid_positions, pos_to_flagpos[ mob->position ] ) )
			continue;
		if ( prg->trig_type == &TRIG_HIT && number_percent() <= atoi( prg->trig_phrase ) )
		{
			create_mprog_env( prg->name, prg->code, mob, victim, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			return TRUE;
		}
	}
	return FALSE;
}

bool mp_precommand_trigger( CHAR_DATA *mob, CHAR_DATA *victim, OBJ_DATA *obj, DO_FUN * fun, char *fun_name, char *argument )
{
	PROG_LIST * prg;

	for ( prg = mob->pIndexData->progs; prg; prg = prg->next )
	{
		if ( !IS_SET( prg->valid_positions, pos_to_flagpos[ mob->position ] ) )
			continue;
		if ( prg->trig_type == &TRIG_PRECOMMAND && !str_cmp( prg->trig_phrase, fun_name ) )
		{
			victim->precommand_fun = fun;
			free_string( victim->precommand_arg );
			victim->precommand_arg = str_dup( argument );
			victim->precommand_pending = FALSE;

			create_mprog_env( prg->name, prg->code, mob, victim, ( void * ) obj, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			return TRUE;
		}
	}
	return FALSE;
}
