#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "tables.h"
#include "progs.h"
#include "recycle.h"
#include "olc.h"
#include "lang.h"

DECLARE_DO_FUN( do_look );

ROOM_INDEX_DATA *
find_location	( CHAR_DATA *, char * );
int
find_door	( CHAR_DATA *ch, char *arg );
void
remember	( CHAR_DATA *mob, CHAR_DATA *who, int reaction );
OBJ_DATA *
rand_item	( int type, int points );
void
objstring	( OBJ_DATA *obj, char *argument );

PROGFUN( obj_interpret )
{
	char command[ MAX_INPUT_LENGTH ];
	int cmd;

	if ( !running_prog_env->obj )
		return ;

	argument = one_argument( argument, command );

	/*
	 * Look for command in command table.
	 */
	for ( cmd = 0; obj_cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
	{
		if ( command[ 0 ] == obj_cmd_table[ cmd ].name[ 0 ]
		     && !str_prefix( command, obj_cmd_table[ cmd ].name ) )
		{
			( *obj_cmd_table[ cmd ].prog_fun ) ( argument );
			tail_chain( );
			return ;
		}
	}
	return ;
}


PROGFUN( do_osetflag )
{
	char arg[ MAX_INPUT_LENGTH ];

	if ( argument[ 0 ] == '\0' )
	{
		bug_prog_format( "osetflag: missing argument" );
		return ;
	}

	expand_arg( arg, argument );

	setobjflag( running_prog_env->obj, arg );
	return ;
}

/* obj setobj [field] [value] */
PROGFUN( do_osetobj )
{
	char arg[ MAX_INPUT_LENGTH ];
	char field[ MAX_INPUT_LENGTH ];
	int val;

	argument = one_argument( argument, field );

	expand_arg( arg, argument );

	if ( !str_cmp( field, "timer" ) )
	{
		if ( !is_number( arg ) )
			return ;

		val = atoi( arg );

		if ( val < 0 )
			return ;

		running_prog_env->obj->timer = val;
		return ;
	}
	else if ( !str_cmp( field, "cost" ) )
	{
		if ( !is_number( arg ) )
			return ;

		val = atoi( arg );

		if ( val < 0 )
			return ;

		running_prog_env->obj->cost = val;
		return ;
	}
	else if ( !str_cmp( field, "wear" ) )
	{
		int value;

		if ( arg[ 0 ] == '\0' )
			return ;

		if ( ( value = flag_value( wear_flags, arg ) ) == NO_FLAG )
			return ;

		TOGGLE_BIT( running_prog_env->obj->wear_flags, value );
		return ;
	}
	else if ( !str_cmp( field, "extra" ) )
	{
		BITVECT_DATA * value;
		bool settable;

		if ( arg[ 0 ] == '\0' )
			return ;

		value = ext_flag_lookup( arg, extra_flags );
		settable = ext_is_settable( arg, extra_flags );

		if ( value == &EXT_NONE || !settable )
			return ;

		EXT_TOGGLE_BIT( running_prog_env->obj->extra_flags, *value );
		return ;
	}
	else if ( !str_cmp( field, "exwear" ) )
	{
		BITVECT_DATA * value;
		bool settable;

		if ( arg[ 0 ] == '\0' )
			return ;

		value = ext_flag_lookup( arg, wear_flags2 );
		settable = ext_is_settable( arg, wear_flags2 );

		if ( value == &EXT_NONE  || !settable )
			return ;

		EXT_TOGGLE_BIT( running_prog_env->obj->wear_flags2, *value );
		return ;
	}
	else if ( !str_cmp( field, "rent_cost" ) )
	{
		if ( !is_number( arg ) )
			return ;

		val = atoi( arg );

		if ( val < 0 )
			return ;

		running_prog_env->obj->rent_cost = val;
		return ;
	}

	return ;
}

/* obj modflag set/mod id val */
PROGFUN( do_omodflag )
{
	PFLAG_DATA * list, *tmp = NULL;
	int val;
	char arg[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char arg3[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, expand );
	expand_arg( arg, expand );
	argument = one_argument( argument, expand );
	expand_arg( arg2, expand );
	expand_arg( arg3, argument );

	if ( arg[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
	{
		bug_prog_format( "omodflag: missing argument" );
		return ;
	}

	for ( list = running_prog_env->obj->pflag_list; list; list = list->next )
		if ( NOPOL( arg2[ 0 ] ) == NOPOL( list->id[ 0 ] ) && !str_cmp( arg2, list->id ) )
		{
			tmp = list;
			break;
		}

	if ( !tmp )
		return ;


	if ( !str_cmp( arg, "set" ) )
	{
		if ( arg3[ 0 ] == '\0' || !is_number( arg3 ) )
			return ;

		val = atoi( arg3 );

		tmp->duration = val;
		return ;
	}
	else if ( !str_cmp( arg, "mod" ) )
	{
		if ( arg3[ 0 ] == '\0' || !is_number( arg3 ) )
			return ;

		val = atoi( arg3 );

		tmp->duration += val;
	}
	else if ( !str_cmp( arg, "rem" ) )
	{
		tmp->duration = 0;
		removeobjflag( running_prog_env->obj, tmp->id );
		return ;
	}

	return ;
}

PROGFUN( do_oremove )
{
	OBJ_DATA * in_obj;
	CHAR_DATA *victim;

	for ( in_obj = running_prog_env->obj; in_obj->in_obj; in_obj = in_obj->in_obj );

	victim = in_obj->carried_by;

	if ( victim )
	{
		unequip_char( victim, running_prog_env->obj );

		/*artefact*/
		if ( is_artefact( running_prog_env->obj ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
			artefact_from_char( running_prog_env->obj, victim );

		obj_from_char( running_prog_env->obj );
	}

	/*artefact*/
	if ( is_artefact( running_prog_env->obj ) ) extract_artefact( running_prog_env->obj );
	if ( running_prog_env->obj->contains ) extract_artefact_container( running_prog_env->obj );

	extract_obj( running_prog_env->obj );

	return ;
}

PROGFUN( do_ostring )
{
	char arg[ MAX_INPUT_LENGTH ];

	if ( argument[ 0 ] == '\0' )
	{
		bug_prog_format( "ostring: missing argument" );
		return ;
	}

	expand_arg( arg, argument );

	objstring( running_prog_env->obj, arg );
}

PROGFUN( do_objwear )
{
	if ( !running_prog_env->obj->carried_by )
		return ;

	equip_char( running_prog_env->obj->carried_by, running_prog_env->obj, running_prog_env->obj->prewear_loc, FALSE );
	running_prog_env->obj->prewear_loc = -1;
}

PROGFUN( do_objunwear )
{

	if ( !running_prog_env->obj->carried_by )
		return ;

	unequip_char( running_prog_env->obj->carried_by, running_prog_env->obj );
}

PROGFUN( do_objsubstitute )
{
	OBJ_DATA * new_obj;
	OBJ_INDEX_DATA * pObj;
	CHAR_DATA *victim;
	char arg[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];
	int wear_loc;

	if ( is_artefact( running_prog_env->obj ) )
	{
		bug_prog_format( "objsubstitute: not on artefacts" );
		return ;
	}

	argument = one_argument( argument, expand );
	expand_arg( arg, expand );

	if ( !is_number( arg ) )
	{
		bug_prog_format( "objsubstitute: missing argument" );
		return ;
	}

	if ( ( pObj = get_obj_index( atoi( arg ) ) ) == NULL )
		return ;

	if ( ( new_obj = create_object( pObj, FALSE ) ) == NULL )
		return ;

	if ( is_artefact( new_obj ) )
		return ;

	victim = running_prog_env->obj->carried_by;

	if ( victim )
	{
		wear_loc = running_prog_env->obj->wear_loc;

		unequip_char( victim, running_prog_env->obj );

		obj_to_char( new_obj, victim );
		equip_char( victim, new_obj, wear_loc, FALSE );
	}
	else
	{
		obj_to_room( new_obj, running_prog_env->obj->in_room );
	}

	extract_obj( running_prog_env->obj );
	running_prog_env->obj = new_obj;
	return ;
}
