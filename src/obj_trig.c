#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <ctype.h>
#include "merc.h"
#include "tables.h"
#include "progs.h"

bool op_percent_trigger( OBJ_DATA *obj, CHAR_DATA *ch, void *arg1, void *arg2, BITVECT_DATA *type )
{
	PROG_LIST * prg;

	for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == type
		     && number_percent() <= atoi( prg->trig_phrase ) )
		{
			create_oprog_env( prg->name, prg->code, obj, ch, arg1, arg2, prg->trig_type, prg->trig_phrase );
			program_flow();
			return ( TRUE );
		}
	}
	return ( FALSE );
}

bool op_common_trigger( CHAR_DATA *ch, OBJ_DATA *obj, BITVECT_DATA *type )
{
	bool result;
	if ( type == &TRIG_RANDOM )
	{
		if ( obj->carried_by && obj->carried_by->in_room )
			ch = obj->carried_by;
		else
			ch = NULL;
	}
	set_supermob( obj );
	result = op_percent_trigger( obj, ch, NULL, NULL, type );
	release_supermob();
	return result;
}

/* hmm wkladamy cos do czegos, czegos musi miec konkretny vnum*/
bool op_put_trigger( OBJ_DATA *obj, CHAR_DATA *ch, void *arg1, BITVECT_DATA *type )
{
	PROG_LIST * prg;
	OBJ_DATA *obj2 = NULL;
	ush_int vnum = -1;

	if ( arg1 == NULL )
		return ( FALSE );

	obj2 = ( OBJ_DATA * ) arg1;
	vnum = obj2->pIndexData->vnum;


	for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
	{
		if ( ( prg->trig_type == type ) && ( atoi( prg->trig_phrase ) == vnum ) )
		{
			set_supermob( obj );
			create_oprog_env( prg->name, prg->code, obj, ch, arg1, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			release_supermob();
			return ( TRUE );
		}
	}
	return ( FALSE );
}

bool op_connect_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
	PROG_LIST * prg;

	for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == &TRIG_CONNECT )
		{
			set_supermob( obj );
			create_oprog_env( prg->name, prg->code, obj, ch, NULL, NULL, prg->trig_type, NULL );
			program_flow();
			release_supermob();
			return ( TRUE );
		}
	}
	return ( FALSE );
}

bool op_consume_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
	PROG_LIST * prg;

	if ( obj->value[ 2 ] != obj->pIndexData->value[ 2 ] )
		return ( FALSE );

	for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == &TRIG_CONSUME )
		{
			set_supermob( obj );
			create_oprog_env( prg->name, prg->code, obj, ch, NULL, NULL, prg->trig_type, NULL );
			program_flow();
			release_supermob();
			return ( TRUE );
		}
	}
	return ( FALSE );
}

void op_hpcnt_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
	PROG_LIST * prg;

	for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == &TRIG_HPCNT
		     && ( ( 100 * ch->hit / get_max_hp(ch) ) < atoi( prg->trig_phrase ) ) )
		{
			set_supermob( obj );
			create_oprog_env( prg->name, prg->code, obj, ch, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			release_supermob();
			return ;
		}
	}
	return ;
}

void op_fight_trigger( CHAR_DATA *ch )
{
	OBJ_DATA * obj;

	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE )
			continue;

		if ( HAS_OTRIGGER( obj, TRIG_FIGHT ) )
		{
			set_supermob( obj );
			op_percent_trigger( obj, ch, obj, ch->fighting, &TRIG_FIGHT );
			release_supermob();
		}

		if ( HAS_OTRIGGER( obj, TRIG_HPCNT ) )
			op_hpcnt_trigger( obj, ch );
	}
	return ;
}

bool op_input_trigger( CHAR_DATA *ch, char *string )
{
	char buf [ MAX_STRING_LENGTH ];
	OBJ_DATA * obj;
	PROG_LIST *prg;


	/*najpierw graty w roomie*/
	for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
	{
		if ( can_see_obj( ch, obj ) && HAS_OTRIGGER( obj, TRIG_INPUT ) )
		{
			for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
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

					set_supermob( obj );
					create_oprog_env( prg->name, prg->code, obj, ch, NULL, NULL, prg->trig_type, string );
					program_flow();
					release_supermob();
					return ( TRUE );
				}
			}
		}
	}
	/*pozniej ZALOZONY eq*/
	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE ) continue;

		if ( HAS_OTRIGGER( obj, TRIG_INPUT ) )
		{
			for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
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

					set_supermob( obj );
					create_oprog_env( prg->name, prg->code, obj, ch, NULL, NULL, prg->trig_type, string );
					program_flow();
					release_supermob();
					return ( TRUE );
				}
			}
		}

	}
	return ( FALSE );
}

void op_flagoff_trigger( char *argument, OBJ_DATA *obj )
{
	PROG_LIST * prg;

	for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == &TRIG_FLAGOFF
		     && !str_cmp( argument, prg->trig_phrase ) )
		{
			CHAR_DATA * owner = NULL;
			OBJ_DATA *in_obj;
			/* find owner of this item */
			for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj );

			if ( in_obj->carried_by )
				owner = in_obj->carried_by;
			else
				owner = NULL;

			/**/
			set_supermob( obj );
			create_oprog_env( prg->name, prg->code, obj, owner, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			release_supermob();
			return ;
		}
	}
	return ;
}

bool op_onehit_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
	PROG_LIST * prg;

	for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == &TRIG_ONEHIT && number_percent() <= atoi( prg->trig_phrase ) )
		{
			set_supermob( obj );
			create_oprog_env( prg->name, prg->code, obj, ch, obj, ch->fighting, prg->trig_type, prg->trig_phrase );
			program_flow();
			release_supermob();
			return ( TRUE );
		}
	}
	return ( FALSE );
}

bool op_hit_trigger( OBJ_DATA *obj, CHAR_DATA *ch )
{
	PROG_LIST * prg;

	for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == &TRIG_HIT && number_percent() <= atoi( prg->trig_phrase ) )
		{
			set_supermob( obj );
			create_oprog_env( prg->name, prg->code, obj, ch, obj, ch->fighting, prg->trig_type, prg->trig_phrase );
			program_flow();
			release_supermob();
			return ( TRUE );
		}
	}
	return ( FALSE );
}


void op_act_trigger( char *argument, CHAR_DATA *ch, bool room_enabled )
{
	PROG_LIST * prg;
	OBJ_DATA *obj;

	/*najpierw graty w roomie*/
	if ( room_enabled )
	{
		for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
		{
			if ( can_see_obj( ch, obj ) && HAS_OTRIGGER( obj, TRIG_ACT ) )
				for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
				{
					if ( prg->trig_type == &TRIG_ACT
					     && !str_infix( prg->trig_phrase, argument ) )
					{
						set_supermob( obj );
						create_oprog_env( prg->name, prg->code, obj, ch, NULL, NULL, prg->trig_type, argument );
						program_flow();
						release_supermob();
					}
				}
		}
	}

	/*potem zalozony eq*/
	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE )
			continue;

		if ( HAS_OTRIGGER( obj, TRIG_ACT ) )
			for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
			{
				if ( prg->trig_type == &TRIG_ACT
				     && !str_infix( prg->trig_phrase, argument ) )
				{
					set_supermob( obj );
					create_oprog_env( prg->name, prg->code, obj, ch, NULL, NULL, prg->trig_type, argument );
					program_flow();
					release_supermob();
				}
			}
	}

	return ;
}

bool op_precommand_trigger( CHAR_DATA *ch, OBJ_DATA *obj1, OBJ_DATA *obj2, DO_FUN * fun, char *fun_name, char *argument )
{
	PROG_LIST * prg;

	for ( prg = obj1->pIndexData->progs; prg != NULL; prg = prg->next )
	{
		if ( prg->trig_type == &TRIG_PRECOMMAND && !str_cmp( prg->trig_phrase, fun_name ) )
		{
			ch->precommand_fun = fun;
			free_string( ch->precommand_arg );
			ch->precommand_arg = str_dup( argument );
			ch->precommand_pending = FALSE;

			set_supermob( obj1 );
			create_oprog_env( prg->name, prg->code, obj1, ch, obj2, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			release_supermob();
			return TRUE;
		}
	}
	return FALSE;
}

void op_time_trigger( OBJ_DATA *obj, int time )
{
	PROG_LIST * prg;
	CHAR_DATA *ch;

	for ( prg = obj->pIndexData->progs; prg; prg = prg->next )
	{
		if ( prg->trig_type == &TRIG_TIME
		     && time == atoi( prg->trig_phrase ) )
		{
			if ( obj->carried_by )
				ch = obj->carried_by;
			else
				ch = NULL;

			set_supermob( obj );
			create_oprog_env( prg->name, prg->code, obj, ch, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			release_supermob();
			break;
		}
	}
	return ;
}

void op_onload_trigger( OBJ_DATA *obj )
{
	PROG_LIST * prg;

	for ( prg = obj->pIndexData->progs; prg; prg = prg->next )
	{
		if ( prg->trig_type == &TRIG_ONLOAD && number_percent() <= atoi( prg->trig_phrase ) )
		{
			set_supermob( obj );
			create_oprog_env( prg->name, prg->code, obj, obj->carried_by, NULL, NULL, prg->trig_type, prg->trig_phrase );
			program_flow();
			release_supermob();
			break;
		}
	}
	return ;
}
