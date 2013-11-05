#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <ctype.h>
#include "merc.h"
#include "tables.h"
#include "progs.h"

bool rp_percent_trigger( CHAR_DATA *mob, CHAR_DATA *ch,
                         void *arg1, void *arg2, BITVECT_DATA *type )
{
	PROG_LIST * prg;
	if ( !mob ) return ( FALSE );

	for ( prg = mob->in_room->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == type
		     && number_percent() <= atoi( prg->trig_phrase ) )
		{
			create_rprog_env( prg->name, prg->code, mob->in_room, ch, arg1, arg2, prg->trig_type, prg->trig_phrase );
			program_flow();
			return ( TRUE );
		}
	}
	return ( FALSE );
}

void rp_enter_trigger( CHAR_DATA *ch )
{
	rset_supermob( ch->in_room );
	rp_percent_trigger( supermob, ch, NULL, NULL, &TRIG_ENTRY );
	release_supermob();
}

void rp_random_trigger( ROOM_INDEX_DATA *pRoom, BITVECT_DATA *type )
{
	rset_supermob( pRoom );
	rp_percent_trigger( supermob, NULL, NULL, NULL, type );
	release_supermob();
}

void rp_reset_trigger( ROOM_INDEX_DATA *pRoom )
{
	if ( !HAS_RTRIGGER( pRoom, TRIG_RESET ) ) return ;
	rset_supermob( pRoom );
	rp_percent_trigger( supermob, NULL, NULL, NULL, &TRIG_RESET );
	release_supermob();
}

void rp_delay_trigger( CHAR_DATA *ch )
{
	rset_supermob( ch->in_room );
	rp_percent_trigger( supermob, ch, NULL, NULL, &TRIG_DELAY );
	release_supermob();
}



bool rp_exit_trigger( CHAR_DATA *ch, int door )
{
	PROG_LIST * prg;

	for ( prg = ch->in_room->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == &TRIG_EXIT
		     && door == atoi( prg->trig_phrase ) )
		{
			rset_supermob( ch->in_room );
			create_rprog_env( prg->name, prg->code, ch->in_room, ch, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			release_supermob();
			return ( TRUE );
		}
	}
	return ( FALSE );
}

bool rp_input_trigger( CHAR_DATA *ch, char * string )
{
	char buf[ MAX_STRING_LENGTH ];
	PROG_LIST *prg;

	for ( prg = ch->in_room->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == &TRIG_INPUT )
		{
			if ( prg->trig_phrase[ 0 ] == '@' )
			{
				sprintf( buf, "%s\n", string );
				if ( !regexp_match( prg->trig_phrase + 1, buf ) )
					continue;
			}
			else if ( prg->trig_phrase[ 0 ] == '*' )
			{
				if ( str_prefix( string, prg->trig_phrase + 1 ) )
					continue;
			}
			else
			{
				if ( str_cmp( string, prg->trig_phrase ) )
					continue;
			}

			rset_supermob( ch->in_room );
			create_rprog_env( prg->name, prg->code, ch->in_room, ch, NULL, NULL, prg->trig_type, string );
			program_flow();
			release_supermob();
			return ( TRUE );
		}
	}
	return ( FALSE );
}

/* sit, rest, sleep + object ewentualny */
bool rp_pos_trigger( CHAR_DATA *ch, OBJ_DATA *obj, BITVECT_DATA *type )
{
	PROG_LIST * prg;

	for ( prg = ch->in_room->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == type )
			if ( ( is_number( prg->trig_phrase ) && obj != NULL
			       && ( obj->pIndexData->vnum == atoi( prg->trig_phrase ) ) )
			     || !str_cmp( prg->trig_phrase, "none" ) )
			{
				rset_supermob( ch->in_room );
				create_rprog_env( prg->name, prg->code, ch->in_room, ch, obj, NULL, prg->trig_type, prg->trig_phrase );
				program_flow();
				release_supermob();
				return ( TRUE );
			}
	}
	return ( FALSE );
}

void rp_act_trigger( char *argument, CHAR_DATA *ch, BITVECT_DATA *type )
{
	PROG_LIST * prg;
	for ( prg = ch->in_room->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == type
		     && !str_infix( prg->trig_phrase, argument ) )
		{
			rset_supermob( ch->in_room );
			create_rprog_env( prg->name, prg->code, ch->in_room, ch, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			release_supermob();
			break;
		}
	}
	return ;
}


bool rp_locks_trigger( CHAR_DATA *ch, int exit, BITVECT_DATA *type )
{
	PROG_LIST * prg;

	for ( prg = ch->in_room->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == type && exit == atoi( prg->trig_phrase ) )
		{
			rset_supermob( ch->in_room );
			create_rprog_env( prg->name, prg->code, ch->in_room, ch, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			release_supermob();
			return ( TRUE );
		}
	}
	return ( FALSE );
}

void rp_time_trigger( ROOM_INDEX_DATA *room, int time )
{
	PROG_LIST * prg;


	for ( prg = room->progs; prg; prg = prg->next )
	{
		if ( prg->trig_type == &TRIG_TIME
		     && time == atoi( prg->trig_phrase ) )
		{
			rset_supermob( room );
			create_rprog_env( prg->name, prg->code, room, NULL, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			release_supermob();
			break;
		}
	}
	return ;
}

bool rp_knock_trigger( CHAR_DATA *ch, int door )
{
	PROG_LIST * prg;

	for ( prg = ch->in_room->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == &TRIG_KNOCK
		     && door == atoi( prg->trig_phrase ) )
		{
			rset_supermob( ch->in_room );
			create_rprog_env( prg->name, prg->code, ch->in_room, ch, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			release_supermob();
			return ( TRUE );
		}
	}
	return ( FALSE );
}
