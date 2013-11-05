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
 * KILLER MUD is copyright 1999-2013 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Skrzetnicki Krzysztof (gtener@gmail.com              ) [Tener     ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: mob_cmds.c 12204 2013-03-29 21:17:17Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/mob_cmds.c $
 *
 */
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "progs.h"
#include "recycle.h"
#include "olc.h"
#include "lang.h"
#include "projects.h"
#include "money.h"

DECLARE_DO_FUN( do_look );
extern ROOM_INDEX_DATA *find_location( CHAR_DATA *, char * );

bool arte_can_load       args( ( unsigned int vnum) );
int  find_door           args( ( CHAR_DATA *ch, char *arg ) );
int  position_lookup     args( ( const char *name) );
void do_function         args( ( CHAR_DATA *ch, DO_FUN *do_fun, char *argument ) );
void forget              args( ( CHAR_DATA *mob, CHAR_DATA *who, int reaction, bool All ) );
void mem_update_count    args( ( CHAR_DATA *ch ) );
void mp_onload_trigger   args( ( CHAR_DATA *mob ) );
void op_onload_trigger   args( ( OBJ_DATA *obj ) );
void remember            args( ( CHAR_DATA *mob, CHAR_DATA *who, int reaction ) );
void wear_obj_silent     args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
void wield_weapon        args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool primary ) );
void wield_weapon_silent args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool primary ) );

OBJ_DATA* rand_item                                         args( ( int type, int points ) );
OBJ_DATA* randomize_item args ( ( int points ) );

void do_mob( CHAR_DATA *ch, char *argument )
{
    char log_buf[ MAX_STRING_LENGTH ];

    /**
     * limitacja uzycia mob komend dla okreslonych nickow
     */
    if ( ch->desc != NULL
#ifdef OFFLINE
            && str_cmp( ch->name, "Builder" )
#endif //OFFLINE
            && str_cmp( ch->name, "Gurthg" )
            && str_cmp( ch->name, "Karlam" )
       )
    {
        return ;
    }

    if ( !IS_NPC( ch ) )
    {
        sprintf( log_buf, "Log %s [%5d]: mob %s", ch->name, ch->in_room ? ch->in_room->vnum : 0, argument );
        wiznet( log_buf, ch, NULL, WIZ_SECURE, 0, get_trust( ch ) );
        log_string( log_buf );
    }

    sprintf( log_buf, "%s_mob_command", ch->name );
    create_mprog_env( log_buf, NULL, ch, NULL, NULL, NULL, NULL, NULL );
    mob_interpret( argument );
    free_prog_env( running_prog_env );
}

/*
 * Mob command interpreter. Implemented separately for security and speed
 * reasons. A trivial hack of interpret()
 */
PROGFUN( mob_interpret )
{
	char command[ MAX_INPUT_LENGTH ];
	int cmd;
	char buf[ MAX_STRING_LENGTH ];

	argument = one_argument( argument, command );

	/*
	 * Look for command in command table.
	 */
	//DEBUG_INFO( "mob_interpret: enter" );
	//sprintf( buf, "mob_interpret: Enter: %s", argument);
	//bug_prog_format( buf );
	for ( cmd = 0; mob_cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
	{
		if ( command[ 0 ] == mob_cmd_table[ cmd ].name[ 0 ]
		     && !str_prefix( command, mob_cmd_table[ cmd ].name ) )
		{
			//sprintf( buf, "mob_interpret: Found: %s", argument);
			//DEBUG_INFO( "mob_interpret: FOUND" );
			//bug_prog_format( buf );
			( *mob_cmd_table[ cmd ].prog_fun ) ( argument );
			tail_chain( );
			return ;
		}
	}
	sprintf( buf, "mob_interpret: invalid cmd: %s", argument);
	bug_prog_format( buf );
}

/*
 * Displays MOBprogram triggers of a mobile
 *
 * Syntax: mpstat [name]
 */
void do_mpstat( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_STRING_LENGTH ];
	PROG_LIST *mprg;
	CHAR_DATA *victim;
	int i;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Mpstat whom?\n\r", ch );
		return ;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "No such creature.\n\r", ch );
		return ;
	}

	if ( !IS_NPC( victim ) )
	{
		send_to_char( "That is not a mobile.\n\r", ch );
		return ;
	}

	sprintf( arg, "Mobile #%-6d [%s]\n\r",
	         victim->pIndexData->vnum, victim->short_descr );
	send_to_char( arg, ch );

	sprintf( arg, "Delay   %-6d [%s]\n\r",
	         victim->prog_delay,
	         victim->prog_target == NULL
	         ? "No target" : victim->prog_target->name );
	send_to_char( arg, ch );

	if ( !victim->pIndexData->prog_flags )
	{
		send_to_char( "[No programs set]\n\r", ch );
		return ;
	}

	for ( i = 0, mprg = victim->pIndexData->progs; mprg != NULL;mprg = mprg->next )
	{
		sprintf(
		    arg,
		    "[%2d] Trigger [%-8s] Program [%-15s] Phrase [%s]\n\r",
		    ++i,
		    ext_bit_name( prog_flags, mprg->trig_type ),
		    mprg->name,
		    mprg->trig_phrase
		);
		send_to_char( arg, ch );
	}

	return ;

}

/*
 * Displays the source code of a given MOBprogram
 *
 * Syntax: mpdump [name]
 */
void do_mpdump( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_INPUT_LENGTH ];
	PROG_CODE *mprg;
	AREA_DATA *pArea;
	one_argument( argument, buf );
	if ( ( mprg = get_mprog_index( buf ) ) == NULL )
	{
		send_to_char( "Nie ma takiego MOBproga.\n\r", ch );
		return ;
	}
	pArea = mprg->area;
	page_to_char( mprg->code, ch );
}

/*
 * Prints the argument to all active players in the game
 *
 * Syntax: mob gecho [string]
 */
PROGFUN( do_mpgecho )
{
	DESCRIPTOR_DATA * d;
	char arg[ MAX_INPUT_LENGTH ];

	if ( argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpgecho: missing argument" );
		return ;
	}

	expand_arg( arg, argument );

	for ( d = descriptor_list; d; d = d->next )
	{
		if ( !d->character ) continue;

		if ( d->connected == CON_PLAYING )
		{
			if ( IS_IMMORTAL( d->character ) )
				send_to_char( "Mob echo> ", d->character );
			send_to_char( arg, d->character );
			send_to_char( "\n\r", d->character );
		}
	}
}

/*
 * Prints the argument to all players in the same area as the mob
 *
 * Syntax: mob zecho [string]
 */
PROGFUN( do_mpzecho )
{
	DESCRIPTOR_DATA * d;
	char arg[ MAX_INPUT_LENGTH ];

	if ( argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpzecho: missing argument" );
		return ;
	}

	if ( running_prog_env->ch->in_room == NULL )
		return ;

	expand_arg( arg, argument );

	for ( d = descriptor_list; d; d = d->next )
	{
		if ( !d->character ) continue;

		if ( d->connected == CON_PLAYING &&
		     IS_AWAKE( d->character ) &&
		     d->character->in_room != NULL &&
		     SAME_AREA( d->character->in_room->area, running_prog_env->ch->in_room->area ) &&
		     SAME_AREA_PART( d->character, running_prog_env->ch ) &&
		     !EXT_IS_SET( d->character->in_room->room_flags, ROOM_NOZECHO ) )
		{
			if ( IS_IMMORTAL( d->character ) )
				send_to_char( "Mob echo> ", d->character );
			send_to_char( arg, d->character );
			send_to_char( "\n\r", d->character );
		}
	}
}

/*
 * Prints the argument to all the rooms aroud the mobile
 *
 * Syntax: mob asound [string]
 */
PROGFUN( do_mpasound )
{
	ROOM_INDEX_DATA * was_in_room;
	int door;

	if ( argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpasound: missing argument" );
		return ;
	}

	was_in_room = running_prog_env->ch->in_room;
	for ( door = 0; door < 6; door++ )
	{
		EXIT_DATA *pexit;

		if ( ( pexit = was_in_room->exit[ door ] ) != NULL
		     && pexit->u1.to_room != NULL
		     && pexit->u1.to_room != was_in_room )
		{
			running_prog_env->ch->in_room = pexit->u1.to_room;
			MOBtrigger = FALSE;
			act_prog( argument, running_prog_env->ch, NULL, TO_ROOM, POS_RESTING );
			MOBtrigger = TRUE;
		}
	}
	running_prog_env->ch->in_room = was_in_room;
	return ;

}

/*
 * Lets the mobile kill any player or mobile without murder
 *
 * Syntax: mob kill [victim]
 */
PROGFUN( do_mpkill )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	if ( running_prog_env->type != PROG_TYPE_MOB )
	{
		bug_prog_format( "mpkill: command only for mob progs" );
		return ;
	}

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		bug_prog_format( "mpkill: missing argument" );
		return ;
	}

	victim = resolve_char_arg( arg );
	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
		return ;

	if ( victim == running_prog_env->ch || IS_NPC( victim ) || running_prog_env->ch->position == POS_FIGHTING )
		return ;

	if ( IS_AFFECTED( running_prog_env->ch, AFF_CHARM ) && running_prog_env->ch->master == victim )
	{
		bug_prog_format( "mpkill - charmed mob attacking master" );
		return ;
	}

	multi_hit( running_prog_env->ch, victim, TYPE_UNDEFINED );
	return ;
}

/*
 * Lets the mobile assist another mob or player
 *
 * Syntax: mob assist [character]
 */
PROGFUN( do_mpassist )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	if ( running_prog_env->type != PROG_TYPE_MOB )
	{
		bug_prog_format( "mpassist: command only for mob progs" );
		return ;
	}

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		bug_prog_format( "mpassist: missing argument" );
		return ;
	}

	victim = resolve_char_arg( arg );
	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
		return ;

	if ( victim == running_prog_env->ch || running_prog_env->ch->fighting != NULL || victim->fighting == NULL )
		return ;

	multi_hit( running_prog_env->ch, victim->fighting, TYPE_UNDEFINED );
	return ;
}

/*
 * Lets the mobile destroy an object in its inventory
 * it can also destroy a worn object and it can destroy
 * items using all.xxxxx or just plain all of them
 *
 * Syntax: mob junk [item]
 */

PROGFUN ( do_mpjunk )
{
	char arg[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	if ( running_prog_env->type != PROG_TYPE_MOB )
	{
		bug_prog_format( "mpjunk: command only for mob progs" );
		return ;
	}

	one_argument( argument, expand );

	if ( expand[ 0 ] == '\0' )
	{
		bug_prog_format( "mpjunk: missing argument" );
		return ;
	}

	expand_arg( arg, expand );

	if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
	{
		if ( ( obj = get_obj_wear( running_prog_env->ch, arg, FALSE ) ) != NULL )
		{
			unequip_char( running_prog_env->ch, obj );

			/*artefact*/
			if ( is_artefact( obj ) ) extract_artefact( obj );
			if ( obj->contains ) extract_artefact_container( obj );
			extract_obj( obj );
			return ;
		}
		if ( ( obj = get_obj_carry( running_prog_env->ch, arg, running_prog_env->ch ) ) == NULL )
			return ;
		/*artefact*/
		if ( is_artefact( obj ) ) extract_artefact( obj );
		if ( obj->contains ) extract_artefact_container( obj );
		extract_obj( obj );
	}
	else
		for ( obj = running_prog_env->ch->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );
			if ( arg[ 3 ] == '\0' || is_name( &arg[ 4 ], obj->name ) )
			{
				if ( obj->wear_loc != WEAR_NONE )
					unequip_char( running_prog_env->ch, obj );
				/*artefact*/
				if ( is_artefact( obj ) ) extract_artefact( obj );
				if ( obj->contains ) extract_artefact_container( obj );
				extract_obj( obj );
			}
		}

	return ;

}

/*
 * Prints the message to everyone in the room other than the mob and victim
 *
 * Syntax: mob echoaround [victim] [string]
 */

PROGFUN( do_mpechoaround )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpechoaround: missing argument" );
		return ;
	}

	victim = resolve_char_arg( arg );
	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
		return ;

	act_prog( argument, running_prog_env->ch, victim, TO_NOTVICT, POS_RESTING );
}

/*
 * Prints the message to only the victim
 *
 * Syntax: mob echoat [victim] [string]
 */
PROGFUN( do_mpechoat )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpechoat: missing argument" );
		return ;
	}

	victim = resolve_char_arg( arg );
	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
		return ;

	act_prog( argument, running_prog_env->ch, victim, TO_VICT, POS_RESTING );
}

/*
 * Prints the message to the room at large
 *
 * Syntax: mpecho [string]
 */
PROGFUN( do_mpecho )
{
	if ( argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpecho: missing argument" );
		return ;
	}

	act_prog( argument, running_prog_env->ch, NULL, TO_ROOM, POS_RESTING );
}

/*
 * Lets the mobile load another mobile.
 *
 * Syntax: mob mload [vnum]
 */
PROGFUN( do_mpmload )
{
	char arg[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];
	MOB_INDEX_DATA *pMobIndex;
	CHAR_DATA *victim;
	unsigned int vnum;

	one_argument( argument, expand );
	expand_arg( arg, expand );

	if ( running_prog_env->ch->in_room == NULL || arg[ 0 ] == '\0' || !is_number( arg ) )
		return ;

	vnum = atoi( arg );
	if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
	{
		bug_prog_format( "mpmload: bad mob index (%d)", vnum );
		return ;
	}

	victim = create_mobile( pMobIndex );
	char_to_room( victim, running_prog_env->ch->in_room );
	victim->reset_vnum = running_prog_env->ch->in_room->vnum;

	/*artefact*/
	if ( is_artefact_load_mob( victim ) == 1 )
		load_artefact( NULL, NULL, victim );

	if ( HAS_TRIGGER( victim, TRIG_ONLOAD ) )
		mp_onload_trigger( victim );
	return ;
}

/*
 * Lets the mobile load an object
 *
 * Syntax: mob oload [vnum] {R}
 * wyciety level
 */
PROGFUN( do_mpoload )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char arg3[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	OBJ_DATA *cont;
	int cont_vnum;

	bool fToroom = FALSE, fWear = FALSE, fInside = FALSE;

	argument = one_argument( argument, expand );
	expand_arg( arg1, expand );
	argument = one_argument( argument, expand );
	expand_arg( arg2, expand );
	argument = one_argument( argument, arg3 );

	//je¶li to oprog to grat bedzie sie ladowal do inventory gracza posiadajacego ten przedmiot
	if ( running_prog_env->type == PROG_TYPE_OBJ && running_prog_env->obj->carried_by )
		victim = running_prog_env->obj->carried_by;
	else
		victim = running_prog_env->ch;

	if ( arg1[ 0 ] == '\0' )
	{
		bug_prog_format( "mpoload: missing 1st argument" );
		return ;
	}

	/* jesli to jest random */
	if ( !is_number( arg1 ) )
	{
		int type = -2, points = 0;

        save_debug_info("mob_cmds.c => PROGFUN( do_mpaload )", NULL, "before str_cmp", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

		if ( !str_cmp( arg1, "head" ) ) type = 0;
		else if ( !str_cmp( arg1, "body" ) ) type = 1;
		else if ( !str_cmp( arg1, "hands" ) ) type = 2;
		else if ( !str_cmp( arg1, "feet" ) ) type = 3;
		else if ( !str_cmp( arg1, "weapon" ) ) type = 4;
		else if ( !str_cmp( arg1, "shield" ) ) type = 5;
		else if ( !str_cmp( arg1, "about" ) ) type = 6;
		else if ( !str_cmp( arg1, "finger" ) ) type = 7;
		else if ( !str_cmp( arg1, "neck" ) ) type = 8;
		else if ( !str_cmp( arg1, "gem" ) ) type = 9;
		else if ( !str_cmp( arg1, "ear" ) ) type = 10;
		else if ( !str_cmp( arg1, "legs" ) ) type = 11;
		else if ( !str_cmp( arg1, "arms" ) ) type = 12;
		else if ( !str_cmp( arg1, "wrist" ) ) type = 13;
		else if ( !str_cmp( arg1, "random" ) ) type = -1;
		else
		{
			bug_prog_format( "mpoload: bad random body part" );
			return ;
		}

		if ( arg2[ 0 ] == '\0' || !is_number( arg2 ) )
		{
			bug_prog_format( "mpoload: missing 2nd argument or not number" );
			return ;
		}

		if ( type <= -2 )
        {
			return ;
        }

		points = atoi( arg2 );

		if ( points < 0 )
        {
			return ;
        }

		points = UMIN( points, 7 );


		if ( arg3[ 0 ] == 'R' || arg3[ 0 ] == 'r' )
        {
			fToroom = TRUE;
        }
		else if ( arg3[ 0 ] == 'W' || arg3[ 0 ] == 'w' )
        {
			fWear = TRUE;
        }
		else if ( arg3[ 0 ] == 'I' || arg3[ 0 ] == 'i' )
        {
			fInside = TRUE;
        }

        if ( type == -1 )
        {
            obj = randomize_item( points );
        }
        else
        {
            /**
             *
             * GURTHG: 2011-05-26
             *
             * dla przedmiotow randomowych ustawiamy maksymalne punkty
             * na poziomie level ladujacego moba / 5 co daja do 30 poziomu
             * maks 6 ACT_BOSS moze wiecej bo juz 7 osiagnac moze na 14
             * poziomie
             *
             */
            if ( IS_NPC( victim ) )
            {
                if ( EXT_IS_SET( victim->act, ACT_BOSS ) )
                {
                    points = URANGE( 1, points, UMAX( 1, victim->level / 2 ) );
                }
                else
                {
                    points = URANGE( 1, points, UMAX( 1, victim->level / 5 ) );
                }
            }
            obj = rand_item( type, points );
        }

		if ( fInside && !is_number( argument ) )
		{
			fInside = FALSE;
			fToroom = TRUE;
		}

		cont_vnum = atoi( argument );
		for ( cont = victim->in_room->contents; cont != NULL; cont = cont->next )
		{
			if ( cont->pIndexData->vnum == cont_vnum )
            {
				break;
            }
		}

		if ( fInside && ( !cont || cont->item_type != ITEM_CONTAINER ) )
		{
			fInside = FALSE;
			fToroom = TRUE;
		}

		if ( fInside )
		{
			obj_to_obj( obj, cont );
		}
		else
        {
			if ( ( fWear || !fToroom ) && CAN_WEAR( obj, ITEM_TAKE ) )
			{
				obj_to_char( obj, victim );

				if ( fWear )
				{
					if ( obj->item_type == ITEM_WEAPON )
                    {
						wield_weapon( victim, obj, TRUE );
                        if ( ! IS_NPC ( victim ) )
                        {
                            sprintf
                                (
                                 log_buf,
                                 "[%d] mpoload + wield %s (%d), type: %d, name: %s",
                                 victim->in_room ? victim->in_room->vnum : 0,
                                 IS_NPC( victim )? victim->short_descr : victim->name,
                                 points,
                                 obj->value[0],
                                 obj->short_descr
                                );
                            log_string( log_buf );
                        }
                    }
					else
                    {
						wear_obj( victim, obj, TRUE );
                        if ( ! IS_NPC ( victim ) )
                        {
                            sprintf
                                (
                                 log_buf,
                                 "[%d] mpoload + wear %s (%d), type: %d, name: %s",
                                 victim->in_room ? victim->in_room->vnum : 0,
                                 IS_NPC( victim )? victim->short_descr : victim->name,
                                 points,
                                 obj->item_type,
                                 obj->short_descr
                                );
                            log_string( log_buf );
                        }
                    }
				}
			}
			else
            {
                obj_to_room( obj, victim->in_room );
                if ( ! IS_NPC ( victim ) )
                {
                    sprintf
                        (
                         log_buf,
                         "[%d] mpoload + room %s (%d), type: %d, name: %s",
                         victim->in_room ? victim->in_room->vnum : 0,
                         IS_NPC( victim )? victim->short_descr : victim->name,
                         points,
                         obj->item_type,
                         obj->short_descr
                        );
                    log_string( log_buf );
                }
            }
        }

		return ;
	}

	/*
	 * Added 2rd argument
	 * omitted - load to mobile's inventory
	 * 'R'     - load to room
	 * 'W'     - load to mobile and force wear
	 */
	if ( arg2[ 0 ] == 'R' || arg2[ 0 ] == 'r' )
		fToroom = TRUE;
	else if ( arg2[ 0 ] == 'W' || arg2[ 0 ] == 'w' )
		fWear = TRUE;
	else if ( arg2[ 0 ] == 'I' || arg2[ 0 ] == 'i' )
		fInside = TRUE;

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
        sprintf( log_buf, "mpoload: bad vnum arg: %d.", atoi( arg1 ) );
        bug_prog_format( log_buf );
		return ;
	}

	obj = create_object( pObjIndex, FALSE );

	/*artefact*/ /*zabezpieczenie*/
	if ( is_artefact( obj ) )
	{
		extract_obj( obj );
		return ;
	}

	if ( fInside && !is_number( arg3 ) )
	{
		fInside = FALSE;
		fToroom = TRUE;
	}

	cont_vnum = atoi( arg3 );
	for ( cont = victim->in_room->contents; cont != NULL; cont = cont->next_content )
	{
		if ( cont->pIndexData->vnum == cont_vnum )
			break;
	}

	if ( fInside && ( !cont || cont->item_type != ITEM_CONTAINER ) )
	{
		fInside = FALSE;
		fToroom = TRUE;
	}

	if ( fInside )
	{
		obj_to_obj( obj, cont );
	}
	else
		if ( ( fWear || !fToroom ) && CAN_WEAR( obj, ITEM_TAKE ) )
		{
			obj_to_char( obj, victim );

			if ( fWear )
			{
				if ( obj->item_type == ITEM_WEAPON )
					wield_weapon( victim, obj, TRUE );
				else
					wear_obj( victim, obj, TRUE );
			}
		}
		else
		{
			obj_to_room( obj, victim->in_room );
		}
	/*artefact*/
	if ( is_artefact_load_obj( pObjIndex ) == 1 )
		load_artefact( NULL, obj, NULL );

	if ( HAS_OTRIGGER( obj, TRIG_ONLOAD ) )
		op_onload_trigger( obj );
	return ;
}

/*
 * mob aload vnum r/R/w/W/_
 */
PROGFUN( do_mpaload )
{
    char buf[ MAX_INPUT_LENGTH ];
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char arg3[ MAX_INPUT_LENGTH ];
    char expand[ MAX_INPUT_LENGTH ];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    OBJ_DATA *cont;
    CHAR_DATA * victim;
    int cont_vnum;
    bool fToroom = FALSE, fWear = FALSE, fInside = FALSE;

    argument = one_argument( argument, expand );
    expand_arg( arg1, expand );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[ 0 ] == '\0' || !is_number( arg1 ) )
    {
        bug_prog_format( "mpaload: missing argument or not number" );
        return ;
    }

    //je¶li to oprog to grat bedzie sie ladowal do inventory gracza posiadajacego ten przedmiot
    if ( running_prog_env->type == PROG_TYPE_OBJ && running_prog_env->obj->carried_by )
        victim = running_prog_env->obj->carried_by;
    else
        victim = running_prog_env->ch;

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
        bug_prog_format( "mpaload: bad vnum arg." );
        return ;
    }
    if ( !arte_can_load ( atoi(arg1) ) )
    {
        append_file_format_daily(NULL, ARTEFACT_LOG_FILE, "-> NIE STWORZONO %s vnum:%d   (load aload) count>limit",pObjIndex->short_descr,atoi(arg1));
        return;
    }

    obj = create_object ( pObjIndex, FALSE );

    if ( !is_artefact ( obj ) )
    {
        extract_obj( obj );
        return ;
    }

    append_file_format_daily(NULL, ARTEFACT_LOG_FILE, "-> STWORZONO %s vnum:%d   (load aload)",obj->short_descr,obj->pIndexData->vnum);
    create_artefact( obj->pIndexData->vnum );

    if ( arg2[ 0 ] == 'R' || arg2[ 0 ] == 'r' )
    {
        fToroom = TRUE;
    }
    else if ( arg2[ 0 ] == 'W' || arg2[ 0 ] == 'w' )
    {
        fWear = TRUE;
    }
    else if ( arg2[ 0 ] == 'I' || arg2[ 0 ] == 'i' )
    {
        fInside = TRUE;
    }

    if ( fInside && !is_number( arg3 ) )
    {
        fInside = FALSE;
        fToroom = TRUE;
    }

    cont_vnum = atoi( arg3 );
    for ( cont = victim->in_room->contents; cont != NULL; cont = cont->next )
    {
        if ( cont->pIndexData->vnum == cont_vnum )
        {
            break;
        }
    }

    if ( fInside && ( !cont || cont->item_type != ITEM_CONTAINER ) )
    {
        fInside = FALSE;
        fToroom = TRUE;
    }

    if ( fInside )
    {
        obj_to_obj( obj, cont );
        sprintf( buf, "Artefakt [%d] za³adowany do wnêtrza obiektu [%d] (mpaload).", obj->pIndexData->vnum, cont->pIndexData->vnum );
        wiznet( buf, NULL, NULL, WIZ_ARTEFACTLOAD, 0, 39 );
    }
    else
    {
        if ( ( fWear || !fToroom ) && CAN_WEAR( obj, ITEM_TAKE ) )
        {
            obj_to_char( obj, victim );
            sprintf( buf, "Artefakt [%d] za³adowany mobowi [%d] roomie [%d] (mpaload).", obj->pIndexData->vnum, victim->pIndexData->vnum, victim->in_room->vnum );
            wiznet( buf, NULL, NULL, WIZ_ARTEFACTLOAD, 0, 39 );

            if ( fWear )
            {
                if ( obj->item_type == ITEM_WEAPON )
                {
                    wield_weapon( victim, obj, TRUE );
                }
                else
                {
                    wear_obj( victim, obj, TRUE );
                }
            }
        }
        else
        {
            obj_to_room( obj, victim->in_room );
            sprintf( buf, "Artefakt [%d] za³adowany do lokacji [%d] (mpaload).", obj->pIndexData->vnum, victim->in_room->vnum );
            wiznet( buf, NULL, NULL, WIZ_ARTEFACTLOAD, 0, 39 );
        }
    }
    /**
     * trigger
     */
    if ( HAS_OTRIGGER( obj, TRIG_ONLOAD ) )
    {
        op_onload_trigger( obj );
    }
    return ;
}

/*
 Nowa komenda do zakladania komus czegos
 Mozna od razu zakladac badz ladowac do inv tylko
 mob equip [komu] [vnum rzeczy]
*/
PROGFUN( do_mpequip )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char expand[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    bool inv = FALSE;
    bool primary = TRUE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, expand );
    expand_arg( arg2, expand );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
    {
        bug_prog_format( "mpequip: missing argument" );
        return;
    }
    victim = resolve_char_arg( arg1 );
    if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
    {
        return;
    }
    if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg1 ) ) == NULL )
    {
        return;
    }
    /**
     * zaladuj przedmiot, ale go nie ubieraj
     */
    if ( argument[ 0 ] == 'I' || argument[ 0 ] == 'i' )
    {
        inv = TRUE;
    }
    /**
     * jezeli parametr jest "hold" to zaloz bron wlasnie
     * na hold. w kodzie jest odwtrotnie, bo patrzymy czy
     * bron jest primary
     */
    if ( argument[ 0 ] == 'H' || argument[ 0 ] == 'h' )
    {
        primary = FALSE;
    }
    /**
     * sprawdz czy obiekt istnieje
     */
    if ( ( pObjIndex = get_obj_index( atoi( arg2 ) ) ) == NULL )
    {
        char log_buf[ MAX_INPUT_LENGTH ];
        sprintf( log_buf, "mpequip vnum: %d", atoi( arg2 ) );
        log_string( log_buf );
        bug_prog_format( "mpequip: bad vnum arg" );
        return ;
    }
    /**
     * zaladuj przedmiot
     */
    obj = create_object( pObjIndex, FALSE );
    /*artefact zabezp.*/
    if ( is_artefact( obj ) )
    {
        extract_obj( obj );
        return ;
    }
    /*nie ma ladowania rzeczy bez flagi take*/
    if ( !CAN_WEAR( obj, ITEM_TAKE ) )
    {
        extract_obj( obj );
        return ;
    }
    obj_to_char( obj, victim );
    /*artefact*/
    if ( is_artefact_load_obj( pObjIndex ) )
    {
        load_artefact( NULL, obj, NULL );
    }
    if ( !inv )
    {
        if ( obj->item_type == ITEM_WEAPON )
        {
            wield_weapon_silent( victim, obj, primary );
        }
        else
        {
            wear_obj_silent( victim, obj, FALSE );
        }
    }
    if ( HAS_OTRIGGER( obj, TRIG_ONLOAD ) )
    {
        op_onload_trigger( obj );
    }
    return ;
}

/*
 * Lets the mobile purge all objects and other npcs in the room,
 * or purge a specified object or mob in the room. The mobile cannot
 * purge itself for safety reasons.
 *
 * syntax:
 * mob purge [all] - wszystko
 * mob purge o - wszystkie obiekty
 * mob purge m - wszystkie moby
 * mob purge <nazwa moba>|<nazwa obiektu> - mob lub obiekt o podanej nazwie
 */
PROGFUN( do_mppurge )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim = NULL;
	OBJ_DATA *obj = NULL;
	bool purge_objects = FALSE;
	bool purge_mobs = FALSE;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || !str_cmp( arg, "all") )
	{
		purge_objects = TRUE;
		purge_mobs = TRUE;
	}

	if ( !str_cmp( arg, "o") )
		purge_objects = TRUE;

	if ( !str_cmp( arg, "m") )
		purge_mobs = TRUE;

	if ( purge_mobs )
	{
		/* 'purge' */
		CHAR_DATA * vnext;

		for ( victim = running_prog_env->ch->in_room->people; victim != NULL; victim = vnext )
		{
			vnext = victim->next_in_room;
			if ( IS_NPC( victim ) && victim != running_prog_env->ch
			     && !EXT_IS_SET( victim->act, ACT_NOPURGE ) )
			{
				DEBUG_INFO( "do_mppurge:extract_all:pre" );
				extract_char( victim, TRUE );
				DEBUG_INFO( "do_mppurge:extract_all:post" );
			}
		}
	}

	if ( purge_objects )
	{
		OBJ_DATA *obj_next;
		for ( obj = running_prog_env->ch->in_room->contents; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;
			if ( !IS_OBJ_STAT( obj, ITEM_NOPURGE ) )
			{
				/*artefact*/
				if ( is_artefact( obj ) ) extract_artefact( obj );
				if ( obj->contains ) extract_artefact_container( obj );
				extract_obj( obj );
			}
		}
	}

	if ( purge_mobs || purge_objects )
		return ;

	victim = resolve_char_arg( arg );

	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return;

	if ( victim == NULL )
		victim = get_char_room( running_prog_env->ch, arg );

	if ( victim == NULL )
	{
		obj = resolve_obj_arg( arg );

		if ( obj && obj->in_room && running_prog_env->ch->in_room && obj->in_room != running_prog_env->ch->in_room )
			return;

		if ( obj == NULL )
			obj = get_obj_here( running_prog_env->ch, arg );
	}

	if ( victim )
	{
		if ( !IS_NPC( victim ) )
			return ;

		DEBUG_INFO( "do_mppurge:extract_end:pre" );
		extract_char( victim, TRUE );
		DEBUG_INFO( "do_mppurge:extract_end:post" );
	}
	else if ( obj )
	{
		/*artefact*/
		if ( is_artefact( obj ) ) extract_artefact( obj );
		if ( obj->contains ) extract_artefact_container( obj );
		extract_obj( obj );
	}
	return ;
}

/*
 * Lets the mobile goto any location it wishes that is not private.
 *
 * Syntax: mob goto [location]
 */
PROGFUN( do_mpgoto )
{
	char arg[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];
	ROOM_INDEX_DATA *location;

	one_argument( argument, expand );

	if ( expand[ 0 ] == '\0' )
	{
		bug_prog_format( "mpgoto: missing argument" );
		return ;
	}

	expand_arg( arg, expand );

	if ( ( location = find_location( running_prog_env->ch, arg ) ) == NULL )
	{
		bug_prog_format( "mpgoto: no such location" );
		return ;
	}

	if ( running_prog_env->ch->fighting != NULL )
		stop_fighting( running_prog_env->ch, TRUE );

	char_from_room( running_prog_env->ch );
	char_to_room( running_prog_env->ch, location );

	return ;
}

/*
 * Lets the mobile do a command at another location.
 *
 * Syntax: mob at [location] [commands]
 */
PROGFUN( do_mpat )
{
	char arg[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];
	char *line;
	ROOM_INDEX_DATA *location;
	ROOM_INDEX_DATA *original;
	CHAR_DATA *wch;
	OBJ_DATA *on;

	argument = one_argument( argument, expand );

	if ( expand[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpat: missing argument" );
		return ;
	}


	expand_arg( arg, expand );

	if ( ( location = find_location( running_prog_env->ch, arg ) ) == NULL )
	{
		bug_prog_format( "mpat: no such location" );
		return ;
	}

	original = running_prog_env->ch->in_room;
	on = running_prog_env->ch->on;
	char_from_room( running_prog_env->ch );
	char_to_room( running_prog_env->ch, location );

	line = one_argument( argument, expand );

	if ( !str_cmp( expand, "mob" ) )
		mob_interpret( line );
	else
	{
		expand_arg( arg, argument );
		interpret( running_prog_env->ch, arg );
	}

	/*
	* See if 'ch' still exists before continuing!
	* Handles 'at XXXX quit' case.
	*/
	for ( wch = char_list; wch != NULL; wch = wch->next )
	{
		if ( wch == running_prog_env->ch )
		{
			char_from_room( running_prog_env->ch );
			char_to_room( running_prog_env->ch, original );
			running_prog_env->ch->on = on;
			break;
		}
	}

	return ;
}

/*
 * Lets the mobile transfer people.  The 'all' argument transfers
 *  everyone in the current room to the specified location
 *
 * Syntax: mob transfer [target|'all'] [location]
 */
PROGFUN( do_mptransfer )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];
	char	buf[ MAX_STRING_LENGTH ];
	ROOM_INDEX_DATA *location;
	CHAR_DATA *victim;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, expand );

	if ( arg1[ 0 ] == '\0' )
	{
		bug_prog_format( "mptransfer: missing argument" );
		return ;
	}

	if ( !str_cmp( arg1, "all" ) )
	{
		CHAR_DATA * victim_next;

		for ( victim = running_prog_env->ch->in_room->people; victim != NULL; victim = victim_next )
		{
			victim_next = victim->next_in_room;
			if ( !IS_NPC( victim ) )
			{
				sprintf( buf, "%s %s", victim->name, expand );
				do_mptransfer( buf );
			}
		}
		return ;
	}

	if ( expand[ 0 ] == '\0' )
		location = running_prog_env->ch->in_room;
	else
	{
		expand_arg( arg2, expand );

		if ( ( location = find_location( running_prog_env->ch, arg2 ) ) == NULL )
		{
			bug_prog_format( "mptransfer: no such location" );
			return ;
		}

		if ( room_is_private( location ) )
			return ;
	}

	victim = resolve_char_arg( arg1 );
	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg1 ) ) == NULL )
		return ;

	if ( victim->in_room == NULL ) return;

	if ( victim->fighting != NULL )
		stop_fighting( victim, TRUE );

	char_from_room( victim );
	char_to_room( victim, location );
	/*    do_look( victim, "auto" );*/

	if ( victim->position != POS_SLEEPING )
		do_look( victim, "auto" );

	return ;
}

/*
 * Lets the mobile transfer all chars in same group as the victim.
 *
 * Syntax: mob gtransfer [victim] [location]
 */
PROGFUN( do_mpgtransfer )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char	buf[ MAX_STRING_LENGTH ];
	CHAR_DATA *who, *victim, *victim_next;
	save_debug_info("mob_cmds.c => PROGFUN( do_mpgtransfer )", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[ 0 ] == '\0' )
	{
		bug_prog_format( "mpgtransfer: missing argument" );
		return ;
	}

	who = resolve_char_arg( arg1 );
	if ( who && who->in_room && running_prog_env->ch->in_room && who->in_room != running_prog_env->ch->in_room )
		return ;

	if ( who == NULL && ( who = get_char_room( running_prog_env->ch, arg1 ) ) == NULL )
		return ;

	for ( victim = running_prog_env->ch->in_room->people; victim; victim = victim_next )
	{
		victim_next = victim->next_in_room;
		if ( is_same_group( who, victim ) || ( IS_NPC( victim ) && victim->master == who ) )
		{
			sprintf( buf, "%s %s", victim->name, arg2 );
			do_mptransfer( buf );
		}
	}
	return ;
}

/*
 * Lets the mobile force someone to do something. Must be mortal level
 * and the all argument only affects those in the room with the mobile.
 *
 * Syntax: mob force [victim] [commands]
 */
PROGFUN( do_mpforce )
{
	CHAR_DATA *ch = running_prog_env->ch;
	char arg[ MAX_INPUT_LENGTH ];
	char control[ MAX_INPUT_LENGTH ];
	char *line;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpforce: missing argument" );
		return ;
	}

	if ( !str_cmp( arg, "all" ) )
	{
		CHAR_DATA * v_ch;
		CHAR_DATA *vch_next;

		for ( v_ch = char_list; v_ch != NULL; v_ch = vch_next )
		{
			vch_next = v_ch->next;

			if ( v_ch == running_prog_env->ch )
				continue;

			if ( v_ch->in_room == running_prog_env->ch->in_room
			     && can_see( running_prog_env->ch, v_ch ) )
			{
				line = one_argument( argument, control );

				if ( !str_cmp( control, "mob" ) )
				{
					running_prog_env->ch = v_ch;
					mob_interpret( line );
					running_prog_env->ch = ch;
				}
				else
				{
					expand_arg( arg, argument );
					if ( !IS_IMMORTAL( v_ch ) )
					  interpret( v_ch, arg );
				}
			}
		}
	}
	else
	{
		CHAR_DATA *victim;

		victim = resolve_char_arg( arg );
		if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
			return ;

		if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
			return ;

		if ( victim == running_prog_env->ch )
			return ;

		line = one_argument( argument, control );

		if ( !str_cmp( control, "mob" ) )
		{
			running_prog_env->ch = victim;
			mob_interpret( line );
			running_prog_env->ch = ch;
		}
		else
		{
			expand_arg( arg, argument );
			if ( !IS_IMMORTAL( victim ) )
			  interpret( victim, arg );
		}
	}

	return ;
}

/*
 * Lets the mobile force a group something. Must be mortal level.
 *
 * Syntax: mob gforce [victim] [commands]
 */
PROGFUN( do_mpgforce )
{
	CHAR_DATA *ch = running_prog_env->ch;
	char arg[ MAX_INPUT_LENGTH ];
	char control[ MAX_INPUT_LENGTH ];
	char *line;
	CHAR_DATA *victim, *v_ch, *vch_next;
	save_debug_info("mob_cmds.c => PROGFUN( do_mpgforce )", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpgforce: missing argument" );
		return ;
	}

	victim = resolve_char_arg( arg );
	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
		return ;

	if ( victim == running_prog_env->ch )
		return ;

	for ( v_ch = victim->in_room->people; v_ch != NULL; v_ch = vch_next )
	{
		vch_next = v_ch->next_in_room;

		if ( is_same_group( victim, v_ch ) || ( IS_NPC( v_ch ) && v_ch->master == victim ) )
		{
			line = one_argument( argument, control );

			if ( !str_cmp( control, "mob" ) )
			{
				running_prog_env->ch = v_ch;
				mob_interpret( line );
				running_prog_env->ch = ch;
			}
			else
			{
				expand_arg( arg, argument );
				interpret( v_ch, arg );
			}
		}
	}
	return ;
}

/*
 * Forces all mobiles of certain vnum to do something (except ch)
 *
 * Syntax: mob vforce [vnum] [commands]
 */
PROGFUN( do_mpvforce )
{
	CHAR_DATA *ch = running_prog_env->ch;
	CHAR_DATA * victim, *victim_next;
	char arg[ MAX_INPUT_LENGTH ];
	char control[ MAX_INPUT_LENGTH ];
	char *line;
	unsigned int vnum;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpvforce: missing argument" );
		return ;
	}

	if ( !is_number( arg ) )
	{
		bug_prog_format( "mpvforce: 1st argument not number" );
		return ;
	}

	vnum = atoi( arg );

	for ( victim = char_list; victim; victim = victim_next )
	{
		victim_next = victim->next;
		if ( IS_NPC( victim ) && victim->pIndexData->vnum == vnum
		     && running_prog_env->ch != victim && victim->fighting == NULL )
		{
			line = one_argument( argument, control );

			if ( !str_cmp( control, "mob" ) )
			{
				running_prog_env->ch = victim;
				mob_interpret( line );
				running_prog_env->ch = ch;
			}
			else
			{
				expand_arg( arg, argument );
				interpret( victim, arg );
			}
		}
	}
	return ;
}

/*
 * Lets the mobile cast spells --
 * Beware: this does only crude checking on the target validity
 * and does not account for mana etc., so you should do all the
 * necessary checking in your mob program before issuing this cmd!
 *
 * Syntax: mob cast <spell> [target] [level]
 */

PROGFUN( do_mpcast )
{
	CHAR_DATA * v_ch;
	OBJ_DATA *obj;
	void *victim = NULL;
	char spell[ MAX_INPUT_LENGTH ];
	char target[ MAX_INPUT_LENGTH ];
	int sn;
	int level;

	argument = one_argument( argument, spell );
	argument = one_argument( argument, target );

	if ( spell[ 0 ] == '\0' )
	{
		bug_prog_format( "mpcast: missing argument" );
		return ;
	}

	if ( ( sn = skill_lookup( spell ) ) < 0 )
	{
		bug_prog_format( "mpcast: no such spell or skill - %s", spell );
		return ;
	}

	v_ch = resolve_char_arg( target );
	obj = resolve_obj_arg( target );
	if ( v_ch && v_ch->in_room && running_prog_env->ch->in_room && v_ch->in_room != running_prog_env->ch->in_room )
		return ;

	if ( !v_ch )
		v_ch = get_char_room( running_prog_env->ch, target );

	if ( !obj )
		obj = get_obj_here( running_prog_env->ch, target );

	switch ( skill_table[ sn ].target )
	{
		default:
			return ;

		case TAR_IGNORE:
			break;

		case TAR_CHAR_OFFENSIVE:
			if ( v_ch == NULL || v_ch == running_prog_env->ch )
				return ;
			victim = ( void * ) v_ch;
			break;

		case TAR_CHAR_DEFENSIVE:
			victim = v_ch == NULL ? ( void * ) running_prog_env->ch : ( void * ) v_ch;
			break;

		case TAR_CHAR_SELF:
			victim = v_ch == NULL ? ( void * ) running_prog_env->ch : ( void * ) v_ch;
			break;

		case TAR_OBJ_CHAR_DEF:
		case TAR_OBJ_CHAR_OFF:
		case TAR_OBJ_INV:
			if ( obj == NULL && !v_ch )
				return ;
			if ( obj )
				victim = ( void * ) obj;
			else
				victim = ( void * ) v_ch;
			break;
	}

	if ( argument[ 0 ] != '\0' && is_number( argument ) )
		level = atoi( argument );
	else
		level = running_prog_env->ch->level;

	level = URANGE( 1, level, 35 );

	( *skill_table[ sn ].spell_fun ) ( sn, level, running_prog_env->ch, victim, skill_table[ sn ].target );
	return ;
}

/*
 * Lets mob cause unconditional damage to someone. Nasty, use with caution.
 * Also, this is silent, you must show your own damage message...
 *
 * Syntax: mob damage [victim] [min] [max] {kill}
 */
PROGFUN( do_mpdamage )
{
	CHAR_DATA * victim = NULL, *victim_next;
	MURDER_LIST *tmp_death;
	char tmpbuf[ MAX_INPUT_LENGTH ];
	char target[ MAX_INPUT_LENGTH ],
	min[ MAX_INPUT_LENGTH ],
	max[ MAX_INPUT_LENGTH ];
	int low, high, vnum = 0;
	bool fAll = FALSE, fKill = FALSE;

	argument = one_argument( argument, target );
	argument = one_argument( argument, min );
	argument = one_argument( argument, max );

	if ( target[ 0 ] == '\0' )
	{
		bug_prog_format( "mpdamage: missing argument" );
		return ;
	}

	if ( !str_cmp( target, "all" ) )
		fAll = TRUE;
	else
	{
		victim = resolve_char_arg( target );
		if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
			return ;

		if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, target ) ) == NULL )
			return ;
	}

	if ( is_number( min ) )
		low = atoi( min );
	else
	{
		bug_prog_format( "mpdamage: bad min damage" );
		return ;
	}

	if ( is_number( max ) )
		high = atoi( max );
	else
	{
		bug_prog_format( "mpdamage: bad max damage" );
		return ;
	}

	one_argument( argument, target );

	/*
	* If kill parameter is omitted, this command is "safe" and will not
	* kill the victim.
	*/

	if ( target[ 0 ] != '\0' )
		fKill = TRUE;
	if ( fAll )
	{
		for ( victim = running_prog_env->ch->in_room->people; victim; victim = victim_next )
		{
			victim_next = victim->next_in_room;
			if ( victim != running_prog_env->ch )
			{
				vnum = victim->in_room ? victim->in_room->vnum : 0;
				damage( victim, victim,
				        fKill ? number_range( low, high ) : UMIN( victim->hit, number_range( low, high ) ),
				        TYPE_UNDEFINED, DAM_NONE, FALSE );
				if ( !IS_NPC( victim ) && !victim->in_room )
				{
					tmp_death = malloc( sizeof( *tmp_death ) );
					if ( IS_NPC( running_prog_env->ch ) )
						sprintf( tmpbuf, "%s [%5d] z mob damage", running_prog_env->ch->name4, running_prog_env->ch->pIndexData->vnum );
					else
						sprintf( tmpbuf, "%s z mob damage", running_prog_env->ch->name4 );
					tmp_death->name = str_dup( tmpbuf );
					tmp_death->char_level = 0;
					tmp_death->victim_level = victim->level;
					tmp_death->room = vnum;
					tmp_death->time = current_time;
					tmp_death->next = victim->pcdata->death_statistics.pkdeath_list;
					victim->pcdata->death_statistics.pkdeath_list = tmp_death;
				}
			}
		}
	}
	else
	{
		vnum = victim->in_room ? victim->in_room->vnum : 0;
		damage( victim, victim,
		        fKill ? number_range( low, high ) : UMIN( victim->hit, number_range( low, high ) ),
		        TYPE_UNDEFINED, DAM_NONE, FALSE );
		if ( !IS_NPC( victim ) && !victim->in_room )
		{
			tmp_death = malloc( sizeof( *tmp_death ) );
			if ( IS_NPC( running_prog_env->ch ) )
				sprintf( tmpbuf, "%s [%5d] z mob damage", running_prog_env->ch->name4, running_prog_env->ch->pIndexData->vnum );
			else
				sprintf( tmpbuf, "%s z mob damage", running_prog_env->ch->name4 );
			tmp_death->name = str_dup( tmpbuf );
			tmp_death->char_level = 0;
			tmp_death->victim_level = victim->level;
			tmp_death->room = vnum;
			tmp_death->time = current_time;
			tmp_death->next = victim->pcdata->death_statistics.pkdeath_list;
			victim->pcdata->death_statistics.pkdeath_list = tmp_death;
		}
	}
	return ;
}

/*
 * Lets the mobile to remember a target. The target can be referred to
 * with $q and $Q codes in MOBprograms. See also "mob forget".
 *
 * Syntax: mob remember [victim]
 */
PROGFUN( do_mpremember )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	if ( argument[0] == '\0' )
	{
		bug_prog_format( "mpremeber: missing argument" );
		return;
	}

	one_argument( argument, arg );

	victim = resolve_char_arg( arg );

	if ( victim == NULL )
		victim = get_char_world( running_prog_env->ch, arg );

	switch( running_prog_env->type )
	{
		case PROG_TYPE_MOB:
			running_prog_env->ch->prog_target = victim;
			break;
		case PROG_TYPE_OBJ:
			running_prog_env->obj->prog_target = running_prog_env->ch->prog_target = victim;
			break;
		case PROG_TYPE_ROOM:
			running_prog_env->room->prog_target = running_prog_env->ch->prog_target = victim;
			break;
	}
	return;
}

/*
 * Reverse of "mob remember".
 *
 * Syntax: mob forget
 */
PROGFUN( do_mpforget )
{
	switch( running_prog_env->type )
	{
		case PROG_TYPE_MOB:
			running_prog_env->ch->prog_target = NULL;
			break;
		case PROG_TYPE_OBJ:
			running_prog_env->obj->prog_target = running_prog_env->ch->prog_target = NULL;
			break;
		case PROG_TYPE_ROOM:
			running_prog_env->room->prog_target = running_prog_env->ch->prog_target = NULL;
			break;
	}
	return;
}

/*
 * Sets a delay for MOBprogram execution. When the delay time expires,
 * the mobile is checked for a MObprogram with DELAY trigger, and if
 * one is found, it is executed. Delay is counted in PULSE_MOBILE
 *
 * Syntax: mob delay [pulses]
 */
PROGFUN( do_mpdelay )
{
	char arg[ MAX_INPUT_LENGTH ];

	one_argument( argument, arg );

	if ( arg[0] == '\0' || !is_number( arg ) )
	{
		bug_prog_format( "mpforget: missing argument or not number" );
		return;
	}

	switch( running_prog_env->type )
	{
		case PROG_TYPE_MOB:
			running_prog_env->ch->prog_delay = atoi( arg );
			break;
		case PROG_TYPE_ROOM:
			running_prog_env->room->prog_delay = atoi( arg );
			break;
	}

	return ;
}

/*
 * Reverse of "mob delay", deactivates the timer.
 *
 * Syntax: mob cancel
 */
PROGFUN( do_mpcancel )
{
	switch( running_prog_env->type )
	{
		case PROG_TYPE_MOB:
			running_prog_env->ch->prog_delay = 0;
			break;
		case PROG_TYPE_ROOM:
			running_prog_env->room->prog_delay = 0;
			break;
	}

	return;
}

/*
 * NIEDZIALA - uzywac call mobprog|objprog|roomprog obsluzone w program_flow
 */
PROGFUN( do_mpcall )
{
	return ;
}

/*
 * Forces the mobile to flee.
 *
 * Syntax: mob flee
 *
 */
PROGFUN( do_mpflee )
{
	ROOM_INDEX_DATA * was_in;
	EXIT_DATA *pexit;
	int door, attempt;

	if ( running_prog_env->type != PROG_TYPE_MOB )
	{
		bug_prog_format( "mpflee: command only for mob progs" );
		return ;
	}

	if ( running_prog_env->ch->fighting != NULL )
		return ;

	if ( ( was_in = running_prog_env->ch->in_room ) == NULL )
		return ;

	for ( attempt = 0; attempt < 6; attempt++ )
	{
		door = number_door( );

		if ( ( pexit = was_in->exit[ door ] ) == 0
		     || pexit->u1.to_room == NULL
		     || IS_SET( pexit->exit_info, EX_CLOSED )
		     || IS_SET( pexit->exit_info, EX_HIDDEN )
		     || IS_SET( sector_table[ running_prog_env->ch->in_room->sector_type ].flag, SECT_UNDERWATER )
		     || IS_SET( pexit->exit_info, EX_SECRET )
		     || IS_SET( pexit->exit_info, EX_NO_FLEE )
		     || ( IS_NPC( running_prog_env->ch )
		          && ( EXT_IS_SET( pexit->u1.to_room->room_flags, ROOM_NO_MOB )
		               || IS_SET( pexit->exit_info, EX_NO_MOB ) ) ) )
			continue;

		move_char( running_prog_env->ch, door, FALSE, NULL );

		if ( running_prog_env->ch->in_room != was_in )
			return ;
	}
}

/*
 * Lets the mobile to transfer an object. The object must be in the same
 * room with the mobile.
 *
 * Syntax: mob otransfer [item name] [location]
 */
PROGFUN( do_mpotransfer )
{
	OBJ_DATA * obj;
	ROOM_INDEX_DATA *location;
	char arg[ MAX_INPUT_LENGTH ];
	char buf[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		bug_prog_format( "mpotransfer: missing argument" );
		return;
	}

	one_argument( argument, expand );
	expand_arg( buf, expand );

	if ( ( location = find_location( running_prog_env->ch, buf ) ) == NULL )
	{
		bug_prog_format( "mpotransfer: no such location" );
		return ;
	}

	obj = resolve_obj_arg( arg );

	if ( obj == NULL && ( obj = get_obj_here( running_prog_env->ch, arg ) ) == NULL )
		return ;

	if ( obj->carried_by == NULL )
		obj_from_room( obj );
	else
	{
		if ( obj->wear_loc != WEAR_NONE )
			unequip_char( running_prog_env->ch, obj );

		/*artefact*/
		if ( is_artefact( obj ) && !IS_NPC( running_prog_env->ch ) && !IS_IMMORTAL( running_prog_env->ch ) )
			artefact_from_char( obj, running_prog_env->ch );

		obj_from_char( obj );
	}
	obj_to_room( obj, location );
}

/*
 * Lets the mobile to strip an object or all objects from the victim.
 * Useful for removing e.g. quest objects from a character.
 *
 * Syntax: mob remove [victim] [object vnum|'all']
 */
PROGFUN( do_mpremove )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj, *obj_next;
	unsigned int vnum = 0;
	bool fAll = FALSE;
	char arg[ MAX_INPUT_LENGTH ];
	int count = 0;

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		bug_prog_format( "mpremove: missing 1st argument" );
		return;
	}

	victim = resolve_char_arg( arg );

	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
		return ;

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		bug_prog_format( "mpremove: missing 2nd argument" );
		return;
	}

	if ( !str_cmp( arg, "all" ) )
		fAll = TRUE;
	else if ( !is_number( arg ) )
	{
		bug_prog_format ( "mpremove: invalid object" );
		return ;
	}
	else
		vnum = atoi( arg );

	if ( argument[ 0 ] != '\0' && is_number( argument ) )
		count = atoi( argument );

	for ( obj = victim->carrying; obj; obj = obj_next )
	{
		OBJ_NEXT_CONTENT( obj, obj_next );

		if ( fAll || obj->pIndexData->vnum == vnum )
		{
			unequip_char( victim, obj );

			/*artefact*/
			if ( is_artefact( obj ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
				artefact_from_char( obj, victim );

			obj_from_char( obj );

			/*artefact*/
			if ( is_artefact( obj ) ) extract_artefact( obj );
			if ( obj->contains ) extract_artefact_container( obj );

			extract_obj( obj );
			count--;

			if ( !fAll && count == 0 )
				break;
		}
	}
}

/*
 * flagi
 */
PROGFUN( do_mpsetflag )
{
	char arg[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, arg );
	expand_arg( expand, argument );

	if ( arg[ 0 ] == '\0' || expand[ 0 ] == '\0' )
	{
		bug_prog_format( "mpsetflag: missing argument" );
		return;
	}

	if ( !str_cmp( arg, "all" ) )
	{
		CHAR_DATA * v_ch;
		CHAR_DATA *vch_next;

		for ( v_ch = char_list; v_ch != NULL; v_ch = vch_next )
		{
			vch_next = v_ch->next;

			if ( v_ch->in_room == running_prog_env->ch->in_room )
				setcharflag( v_ch, expand );
		}
	}
	else if ( !str_cmp( arg, "room" ) )
	{
		setroomflag( running_prog_env->ch->in_room, expand );
		return ;
	}
	else if ( !str_cmp( arg, "global" ) )
	{
		setglobalflag( expand );
		return ;
	}
	else
	{
		CHAR_DATA *victim;
		OBJ_DATA *obj;

		victim = resolve_char_arg( arg );
		obj = resolve_obj_arg( arg );

		if ( victim )
		{
			if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
				return ;

			if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
				return ;
			setcharflag( victim, expand );
		}
		else if ( obj )
		{
			setobjflag( obj, expand );
		}
	}
	return ;
}

PROGFUN( do_mpremoveflag )
{
	char arg[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];
	PFLAG_DATA **list = NULL;

	argument = one_argument( argument, arg );
	           one_argument( argument, arg2 );
	expand_arg( expand, arg2 );

	if ( arg[ 0 ] == '\0' || expand[ 0 ] == '\0' )
	{
		bug_prog_format( "mpremoveflag: missing argument" );
		return;
	}

	if ( !str_cmp( arg, "all" ) )
	{
		CHAR_DATA * v_ch;
		CHAR_DATA *vch_next;

		for ( v_ch = char_list; v_ch != NULL; v_ch = vch_next )
		{
			vch_next = v_ch->next;

			if ( v_ch->in_room == running_prog_env->ch->in_room )
				rem_flag( &v_ch->pflag_list, expand );
		}
	}
	else if ( !str_cmp( arg, "room" ) )
		list = &running_prog_env->ch->in_room->pflag_list;
	else if ( !str_cmp( arg, "global" ) )
		list = &misc.global_flag_list;
	else
	{
		CHAR_DATA * victim;
		OBJ_DATA * obj;

		victim = resolve_char_arg( arg );
		obj = resolve_obj_arg( arg );

		if ( victim )
		{
			if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
				return ;

			if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
				return ;

			list = &victim->pflag_list;
		}
		else if ( obj )
			list = &obj->pflag_list;
		else
			return;
	}

	rem_flag( list, expand );
	return ;
}

/*
 mob reward [kto] [exp/gold/silver/...] [ile]
 */
PROGFUN( do_mpreward )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;
	long exp = 0;
	int sn, val;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	expand_arg( expand, argument );

	if ( arg1[ 0 ] == '\0' || expand[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
	{
		bug_prog_format( "mpreward: missing argument" );
		return;
	}

	victim = resolve_char_arg( arg1 );

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg1 ) ) == NULL )
		return ;

	if ( !str_cmp( arg2, "wait" ) )
	{
		if ( !is_number( expand ) )
			return ;

		victim->wait += atoi( expand );
		victim->wait = UMAX( 0, victim->wait );
	}
	else if ( !str_cmp( arg2, "daze" ) )
	{
		if ( !is_number( expand ) )
			return ;

		victim->daze += UMAX( 0, atoi( expand ) );
	}
	else if ( !str_cmp( arg2, "silver" ) )
	{
		long copper = money_count_copper( victim );
        long amount;

		if ( !is_number( expand ) )
        {
			return ;
        }

		//Rysand: Je¶li ok - to odkomentowaæ - czy nie lepiej dodaæ do progów opcji copper?
        //amount = atoi( expand ) * RATTING_SILVER_TO_COPPER;
        amount = atoi( expand );

        if ( amount > 0 )
        {
            money_gain ( victim, amount );
        }
        else if ( amount < 0 )
        {
            money_reduce ( victim, amount );
        }

		if ( IS_NPC( running_prog_env->ch ) )
        {
            append_file_format_daily
                (
                 victim,
                 MONEY_LOG_FILE,
                 "-> S: %d %d (%d) - mpreward z moba [%d]",
                 copper,
                 copper + amount,
                 amount,
                 running_prog_env->ch->pIndexData->vnum
                );
        }
		else
        {
            append_file_format_daily
                (
                 victim,
                 MONEY_LOG_FILE,
                 "-> S: %d %d (%d) - mpreward z postaci %s",
                 copper,
                 copper + amount,
                 amount,
                 running_prog_env->ch->name
                );
        }
	}

	if ( IS_NPC( victim ) )
    {
		return ;
    }

	/* taaa, moze sie wydac dziwne
	ale mp_reward moze tez robic rzeczy wrecz przeciwne
	np zabrac komus kase, expa
	*/

	if ( !str_cmp( arg2, "exp" ) )
	{
		if ( !is_number( expand ) )
			return ;

		gain_exp( victim, UMAX( 0, atoi( expand ) ), FALSE );

		append_file_format_daily( victim, EXP_LOG_FILE,
				"-> %d (%d) - mob reward exp z proga [%s]",
                victim->exp,
                atoi(expand),
				running_prog_env->name );
	}
	else if ( !str_cmp( arg2, "move" ) )
	{
		if ( !is_number( expand ) )
			return ;

		victim->move += UMAX( 0, atoi( expand ) );
	}
	else if ( !str_cmp( arg2, "hunger" ) )
	{
		if ( !is_number( expand ) )
			return ;

		if ( IS_NPC( victim ) )
			return ;

		victim->condition[ COND_HUNGER ] = URANGE( 0, victim->condition[ COND_HUNGER ] + atoi( expand ), 48 );
	}
	else if ( !str_cmp( arg2, "thirst" ) )
	{
		if ( !is_number( expand ) )
			return ;

		if ( IS_NPC( victim ) )
			return ;

		victim->condition[ COND_THIRST ] = URANGE( 0, victim->condition[ COND_THIRST ] + atoi( expand ), 48 );
	}
	else if ( !str_cmp( arg2, "sleepy" ) )
	{
		if ( !is_number( expand ) )
			return ;

		victim->condition[ COND_SLEEPY ] += atoi( expand );
		victim->condition[ COND_SLEEPY ] = URANGE( 0, victim->condition[ COND_SLEEPY ] + atoi( expand ), 48 );
	}
	else if ( !str_cmp( arg2, "glory" ) )
	{
		if ( !is_number( expand ) )
			return ;

		victim->glory += atoi( expand );
	}
	else if ( !str_cmp( arg2, "level" ) )
	{
		if ( !is_number( expand ) )
			return ;

		//jesli 1 to robi level
		if ( atoi( expand ) == 1 )
		{
			exp = exp_per_level( victim, victim->level ) - victim->exp;
			gain_exp( victim, exp, FALSE );

			append_file_format_daily( victim, EXP_LOG_FILE,
					"-> %d (%d) - mob reward level z proga [%s]",
					victim->exp,
                    exp,
					running_prog_env->name );
		}
		//inaczej - w procentach bylo dane
		else
		{
			//caly level
			exp = exp_per_level( victim, victim->level ) - exp_per_level( victim, victim->level - 1 );
			exp = ( exp * atoi( expand ) ) / 100;

			gain_exp( victim, UMAX( 0, exp ), FALSE );

			append_file_format_daily( victim, EXP_LOG_FILE,
					"-> %d (%d) - mob reward level z proga [%s]",
					victim->exp,
                    UMAX( 0, exp ),
					running_prog_env->name );
		}
	}
	else if ( ( sn = skill_lookup( arg2 ) ) != -1 )
	{
		if ( !is_number( expand ) )
			return ;
		else
			val = atoi( expand );

		if ( skill_table[ sn ].spell_fun == spell_null )
		{
			victim->pcdata->learned[ sn ] += atoi( expand );

			if ( val >= 0 )
				victim->pcdata->learned[ sn ] = UMIN( victim->pcdata->learned[ sn ], 100 );
			else
				victim->pcdata->learned[ sn ] = UMAX( victim->pcdata->learned[ sn ], 0 );

			return ;
		}
		else
		{
			if ( val > 0 )
				victim->pcdata->learned[ sn ] = 1;
			else
				victim->pcdata->learned[ sn ] = 0;
		}
	}
	return ;
}

/*
 * mob greward <kto> <level/exp> <ile> <g/e>
 * g - kazdy dostaje ilosc/czlonkowie
 * e - kazdy dostaje dana wartosc
 * defoltowo dzielone
 */
PROGFUN( do_mpgreward )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char arg3[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim, *gch;
	long value = 0;
	int people_count = 0;
	long exp;
	save_debug_info("mob_cmds.c => PROGFUN( do_mpgreward )", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, expand );
	expand_arg( arg3, expand );

	if ( arg1[ 0 ] == '\0' || arg3[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
	{
		bug_prog_format( "mpgreward: missing argument" );
		return;
	}

	victim = resolve_char_arg( arg1 );

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg1 ) ) == NULL )
		return ;

	//zliczanie ludzikow w grupie
	if ( argument[ 0 ] == 'g' || argument[ 0 ] == '\0' )
	{
		people_count = 0;
		for ( gch = char_list; gch != NULL; gch = gch->next )
		{
			if ( is_same_group( gch, victim ) )
				people_count++;
		}
	}
	else
		people_count = 1;


	if ( !str_cmp( arg2, "exp" ) )
	{
		if ( !is_number( arg3 ) )
			return ;

		value = atoi( arg3 );

		for ( gch = char_list; gch != NULL; gch = gch->next )
		{
			if ( is_same_group( gch, victim ) )
			{
				gain_exp( gch, UMAX( 0, value / people_count ), FALSE );

				if ( !IS_NPC( gch ) )
					append_file_format_daily( gch, EXP_LOG_FILE,
							"-> %d (%d) - mob greward exp z proga [%s]",
							gch->exp,
                            UMAX( 0, value / people_count ),
							running_prog_env->name );
			}
		}
		return ;
	}

	if ( !str_cmp( arg2, "level" ) )
	{
		if ( !is_number( arg3 ) )
			return ;
		value = atoi( arg3 );

		for ( gch = char_list; gch != NULL; gch = gch->next )
		{
			if ( is_same_group( gch, victim ) )
			{
				//jesli 1 to robi level
				if ( value == 1 )
				{
					exp = exp_per_level( gch, gch->level ) - gch->exp;
					gain_exp( gch, UMAX( 0, exp / people_count ), FALSE );

					if ( !IS_NPC( gch ) )
						append_file_format_daily( gch, EXP_LOG_FILE,
								"-> %d (%d) - mob greward level z proga [%s]",
								gch->exp,
                                UMAX( 0, exp / people_count ),
								running_prog_env->name );
				}
				//inaczej - w procentach bylo dane
				else
				{
					//caly level
					exp = exp_per_level( gch, gch->level ) - exp_per_level( gch, gch->level - 1 );
					exp = exp * value / 100;
					gain_exp( gch, UMAX( 0, exp / people_count ), FALSE );

					if ( !IS_NPC( gch ) )
						append_file_format_daily( gch, EXP_LOG_FILE,
								"-> %d (%d) - mob greward level z proga [%s]",
								gch->exp,
                                UMAX( 0, exp / people_count ),
								running_prog_env->name );
				}
			} //same group
		} //for
	}

	return ;
}

PROGFUN( do_mpunlock )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	int door;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		bug_prog_format( "mpunlock: missing argument" );
		return;
	}

	obj = resolve_obj_arg( arg );

	if ( !obj )
		obj = get_obj_here( running_prog_env->ch, arg );

	if ( obj )
	{
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL )
		{
			if ( !IS_SET( obj->value[ 1 ], EX_ISDOOR ) )
				return ;


			if ( !IS_SET( obj->value[ 1 ], EX_CLOSED ) )
				return ;

			if ( obj->value[ 4 ] < 0 )
				return ;

			if ( !IS_SET( obj->value[ 1 ], EX_LOCKED ) )
				return ;


			REMOVE_BIT( obj->value[ 1 ], EX_LOCKED );
			return ;
		}

		/* 'unlock object' */
		if ( obj->item_type != ITEM_CONTAINER ) return ;
		if ( !IS_SET( obj->value[ 1 ], CONT_CLOSED ) ) return ;
		if ( obj->value[ 2 ] < 0 ) return ;
		if ( !IS_SET( obj->value[ 1 ], CONT_LOCKED ) ) return ;

		REMOVE_BIT( obj->value[ 1 ], CONT_LOCKED );
		return ;
	}

	if ( ( door = find_door( running_prog_env->ch, arg ) ) >= 0 )
	{
		/* 'unlock door' */
		ROOM_INDEX_DATA * to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = running_prog_env->ch->in_room->exit[ door ];
		if ( !IS_SET( pexit->exit_info, EX_CLOSED ) ) return ;
		if ( pexit->key == 0 )
			if ( !IS_SET( pexit->exit_info, EX_LOCKED ) ) return ;

		REMOVE_BIT( pexit->exit_info, EX_LOCKED );

		/* unlock the other side */
		if ( ( to_room = pexit->u1.to_room ) != NULL
		     && ( pexit_rev = to_room->exit[ rev_dir[ door ] ] ) != NULL
		     && pexit_rev->u1.to_room == running_prog_env->ch->in_room )
			REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
	}

	return ;
}

PROGFUN( do_mplock )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	int door;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		bug_prog_format( "mplock: missing argument" );
		return;
	}

	obj = resolve_obj_arg( arg );

	if ( !obj )
		obj = get_obj_here( running_prog_env->ch, arg );

	if ( obj )
	{
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL )
		{
			if ( !IS_SET( obj->value[ 1 ], EX_ISDOOR )
			     || IS_SET( obj->value[ 1 ], EX_NOCLOSE ) )
				return ;
			if ( obj->value[ 4 ] < 0 || IS_SET( obj->value[ 1 ], EX_NOLOCK ) )
				return ;

			if ( !IS_SET( obj->value[ 1 ], EX_CLOSED ) )
				return ;



			if ( IS_SET( obj->value[ 1 ], EX_LOCKED ) )
				return ;


			SET_BIT( obj->value[ 1 ], EX_LOCKED );
			return ;
		}

		/* 'lock object' */
		if ( obj->item_type != ITEM_CONTAINER ) return ;
		if ( !IS_SET( obj->value[ 1 ], CONT_CLOSED ) ) return ;
		if ( obj->value[ 2 ] < 0 ) return ;
		if ( IS_SET( obj->value[ 1 ], CONT_LOCKED ) ) return ;

		SET_BIT( obj->value[ 1 ], CONT_LOCKED );
		return ;
	}

	if ( ( door = find_door( running_prog_env->ch, arg ) ) >= 0 )
	{
		/* 'lock door' */
		ROOM_INDEX_DATA * to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit	= running_prog_env->ch->in_room->exit[ door ];

		if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
			return ;

		if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
			return ;

		if ( pexit->key == 0 )
			return ;

		SET_BIT( pexit->exit_info, EX_LOCKED );

		/* lock the other side */
		if ( ( to_room = pexit->u1.to_room ) != NULL
		     && ( pexit_rev = to_room->exit[ rev_dir[ door ] ] ) != 0
		     && pexit_rev->u1.to_room == running_prog_env->ch->in_room )
			SET_BIT( pexit_rev->exit_info, EX_LOCKED );
	}

	return ;
}

PROGFUN( do_mpopen )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	int door;


	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		bug_prog_format( "mpopen: missing argument" );
		return;
	}

	obj = resolve_obj_arg( arg );

	if ( !obj )
		obj = get_obj_here( running_prog_env->ch, arg );

	if ( obj )
	{
		if ( obj->item_type == ITEM_PORTAL )
		{
			if ( !IS_SET( obj->value[ 1 ], EX_ISDOOR ) )
				return ;

			if ( !IS_SET( obj->value[ 1 ], EX_CLOSED ) )
				return ;

			if ( IS_SET( obj->value[ 1 ], EX_LOCKED ) )
				return ;

			REMOVE_BIT( obj->value[ 1 ], EX_CLOSED );
			return ;
		}

		if ( obj->item_type != ITEM_CONTAINER )
			return ;

		if ( !IS_SET( obj->value[ 1 ], CONT_CLOSED ) )
			return ;

		if ( !IS_SET( obj->value[ 1 ], CONT_CLOSEABLE ) )
			return ;

		if ( IS_SET( obj->value[ 1 ], CONT_LOCKED ) )
			return ;

		REMOVE_BIT( obj->value[ 1 ], CONT_CLOSED );
		return ;
	}

	if ( ( door = find_door( running_prog_env->ch, arg ) ) >= 0 )
	{
		ROOM_INDEX_DATA * to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit = running_prog_env->ch->in_room->exit[ door ];
		if ( !IS_SET( pexit->exit_info, EX_CLOSED ) ) return ;
		if ( IS_SET( pexit->exit_info, EX_LOCKED ) ) return ;

		REMOVE_BIT( pexit->exit_info, EX_CLOSED );

		/* open the other side */
		if ( ( to_room = pexit->u1.to_room ) != NULL
		     && ( pexit_rev = to_room->exit[ rev_dir[ door ] ] ) != NULL
		     && pexit_rev->u1.to_room == running_prog_env->ch->in_room )
			REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
	}

	return ;
}

PROGFUN( do_mpclose )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	int door;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		bug_prog_format( "mpclose: missing argument" );
		return;
	}

	obj = resolve_obj_arg( arg );

	if ( !obj )
		obj = get_obj_here( running_prog_env->ch, arg );

	if ( obj )
	{
		/* portal stuff */
		if ( obj->item_type == ITEM_PORTAL )
		{
			if ( !IS_SET( obj->value[ 1 ], EX_ISDOOR )
			     || IS_SET( obj->value[ 1 ], EX_NOCLOSE ) ) return ;

			if ( IS_SET( obj->value[ 1 ], EX_CLOSED ) ) return ;

			SET_BIT( obj->value[ 1 ], EX_CLOSED );
			return ;
		}

		if ( obj->item_type != ITEM_CONTAINER ) return ;
		if ( IS_SET( obj->value[ 1 ], CONT_CLOSED ) ) return ;
		if ( !IS_SET( obj->value[ 1 ], CONT_CLOSEABLE ) ) return ;


		SET_BIT( obj->value[ 1 ], CONT_CLOSED );
		return ;
	}

	if ( ( door = find_door( running_prog_env->ch, arg ) ) >= 0 )
	{
		ROOM_INDEX_DATA * to_room;
		EXIT_DATA *pexit;
		EXIT_DATA *pexit_rev;

		pexit	= running_prog_env->ch->in_room->exit[ door ];
		if ( IS_SET( pexit->exit_info, EX_CLOSED ) ) return ;

		SET_BIT( pexit->exit_info, EX_CLOSED );

		if ( ( to_room = pexit->u1.to_room ) != NULL
		     && ( pexit_rev = to_room->exit[ rev_dir[ door ] ] ) != 0
		     && pexit_rev->u1.to_room == running_prog_env->ch->in_room )
			SET_BIT( pexit_rev->exit_info, EX_CLOSED );
	}

	return ;
}

PROGFUN( do_mpsetrent )
{
	ROOM_INDEX_DATA * location;
	CHAR_DATA	*victim;
	char arg[ MAX_INPUT_LENGTH ];
	char buf[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		bug_prog_format( "mpsetrent: missing argument" );
		return;
	}

	one_argument( argument, expand );
	expand_arg( buf, expand );

	if ( ( location = find_location( running_prog_env->ch, buf ) ) == NULL )
	{
		bug_prog_format( "mpsetrent: no such location" );
		return ;
	}

	if ( !EXT_IS_SET( location->room_flags, ROOM_INN ) )
		return ;

	victim = resolve_char_arg( arg );
	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
		return ;

	if ( IS_NPC( victim ) ) return ;

	victim->pcdata->last_rent = location->vnum;
	return ;
}

/*
 * mob clearmem <kto> <all lub procent >
 * ustawia wszystkie czary na liscie zapamietywanych jako niezapamietane
 * przy procentowym 2 argumencie dla kazdego czaru jest szansa ze tak sie nie stanie
 */
PROGFUN( do_mpclrmem )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;
	MSPELL_DATA *tmp;
	sh_int	chance = 0;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpsetrent: missing argument" );
		return;
	}

	victim = resolve_char_arg( arg );

	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
		return ;

	if ( victim == running_prog_env->ch )
		return ;

	if ( victim->memspell == NULL )
		return;

	if ( !str_cmp( argument, "all" ) )
	{
		for ( tmp = victim->memspell; tmp; tmp = tmp->next )
		{
			tmp->done = FALSE;
		}
		mem_update_count( victim );
	}
	else if ( is_number( argument ) )
	{
		if ( victim->memspell == NULL )
			return ;

		chance = atoi( argument );

		/*usuwamy tylko te ktore maja count wiekszy od jeden i sa done*/
		for ( tmp = victim->memspell; tmp; tmp = tmp->next )
		{
			if ( number_range( 1, 99 ) < chance && tmp->done )
			{
				tmp->done = FALSE;
			}
			mem_update_count( victim );
		}
	}
	return ;
}

PROGFUN( do_mpsethhf )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpsethhf: missing argument" );
		return;
	}

	victim = resolve_char_arg( argument );

	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, argument ) ) == NULL )
		return ;

	if ( !str_cmp( arg, "hate" ) )
	{
		forget( running_prog_env->ch, victim, MEM_FEAR, FALSE );
		EXT_SET_BIT( running_prog_env->ch->act, ACT_MEMORY );
		start_hating( running_prog_env->ch, victim );
		return ;
	}
	else if ( !str_cmp( arg, "hunt" ) )
	{
		if ( victim->fighting )
			return ;

		forget( running_prog_env->ch, victim, MEM_FEAR, FALSE );
		EXT_SET_BIT( running_prog_env->ch->act, ACT_MEMORY );
		start_hunting( running_prog_env->ch, victim );
		return ;
	}
	else if ( !str_cmp( arg, "fear" ) )
	{
		forget( running_prog_env->ch, victim, MEM_HATE, FALSE );
		victim->hunting = NULL;
		EXT_SET_BIT( running_prog_env->ch->act, ACT_MEMORY );
		start_fearing( running_prog_env->ch, victim );
		return ;
	}

	return ;
}

/*
 * skladnia: mob exit dir command <vnum>
 */
PROGFUN( do_mpexit )
{
	ROOM_INDEX_DATA * pRoom;
	char dir[ MAX_INPUT_LENGTH ];
	char command[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];
	sh_int door = -1, exit;
	int value;

	argument = one_argument( argument, dir );
	argument = one_argument( argument, command );
	expand_arg( expand, argument );

	if ( dir[ 0 ] == '\0' || command[ 0 ] == '\0' )
	{
		bug_prog_format( "mpexit: missing argument" );
		return;
	}

	for ( exit = 0; exit < 6; exit++ )
		if ( !str_cmp( dir, dir_name[ exit ] ) )
			door = exit;

	if ( door == -1 )
		return ;

	if ( ! ( pRoom = running_prog_env->ch->in_room ) )
		return ;

	if ( ( value = flag_value( exit_flags, command ) ) != NO_FLAG )
	{
		ROOM_INDEX_DATA * pToRoom;
		sh_int rev;

		if ( !pRoom->exit[ door ] )
			return ;

		TOGGLE_BIT( pRoom->exit[ door ] ->rs_flags, value );
		pRoom->exit[ door ] ->exit_info = pRoom->exit[ door ] ->rs_flags;

		pToRoom = pRoom->exit[ door ] ->u1.to_room;
		rev = rev_dir[ door ];

		if ( pToRoom->exit[ rev ] != NULL )
		{
			pToRoom->exit[ rev ] ->rs_flags = pRoom->exit[ door ] ->rs_flags;
			pToRoom->exit[ rev ] ->exit_info = pRoom->exit[ door ] ->exit_info;
		}

		return ;
	}

	if ( !str_cmp( command, "delete" ) )
	{
		ROOM_INDEX_DATA * pToRoom;
		sh_int rev;

		if ( !pRoom->exit[ door ] )
			return ;

		rev = rev_dir[ door ];
		pToRoom = pRoom->exit[ door ] ->u1.to_room;

		if ( pToRoom->exit[ rev ] )
		{
			free_exit( pToRoom->exit[ rev ] );
			pToRoom->exit[ rev ] = NULL;
		}

		free_exit( pRoom->exit[ door ] );
		pRoom->exit[ door ] = NULL;
		return ;
	}
	else if ( !str_cmp( command, "link" ) )
	{
		EXIT_DATA * pExit;
		ROOM_INDEX_DATA *toRoom;

		if ( expand[ 0 ] == '\0' || !is_number( expand ) )
			return ;

		value = atoi( expand );

		if ( ! ( toRoom = get_room_index( value ) ) )
			return ;

		if ( toRoom->exit[ rev_dir[ door ] ] )
			return ;

		if ( !pRoom->exit[ door ] )
			pRoom->exit[ door ] = new_exit();

		pRoom->exit[ door ] ->u1.to_room = toRoom;
		pRoom->exit[ door ] ->orig_door = door;

		door = rev_dir[ door ];
		pExit = new_exit();
		pExit->u1.to_room = pRoom;
		pExit->orig_door	= door;
		toRoom->exit[ door ] = pExit;
		return ;
	}
	else if ( !str_cmp( command, "room" ) )
	{
		ROOM_INDEX_DATA * toRoom;

		if ( expand[ 0 ] == '\0' || !is_number( expand ) )
			return ;

		value = atoi( expand );

		if ( !( toRoom = get_room_index( value ) ) )
			return ;

		if ( !pRoom->exit[ door ] )
			pRoom->exit[ door ] = new_exit();

		pRoom->exit[ door ] ->u1.to_room = toRoom;
		pRoom->exit[ door ] ->orig_door = door;

		return ;
	}
	else if ( !str_cmp( command, "key" ) )
	{
		OBJ_INDEX_DATA * key;

		if ( expand[ 0 ] == '\0' || !is_number( expand ) )
			return ;

		if ( !pRoom->exit[ door ] )
			return ;

		value = atoi( expand );

		if ( !( key = get_obj_index( value ) ) )
			return ;

		if ( key->item_type != ITEM_KEY )
			return ;

		pRoom->exit[ door ] ->key = value;

		return ;
	}
	else if ( !str_cmp( command, "name" ) )
	{
		if ( expand[ 0 ] == '\0' )
			return ;

		if ( !pRoom->exit[ door ] )
			return ;

		free_string( pRoom->exit[ door ] ->keyword );

		if ( str_cmp( expand, "none" ) )
			pRoom->exit[ door ] ->keyword = str_dup( expand );
		else
			pRoom->exit[ door ] ->keyword = str_dup( "" );
	}
	else if ( !str_cmp( command, "vname" ) )
	{
		if ( expand[ 0 ] == '\0' )
			return ;

		if ( !pRoom->exit[ door ] )
			return ;

		free_string( pRoom->exit[ door ] ->vName );

		if ( str_cmp( expand, "none" ) )
			pRoom->exit[ door ] ->vName = str_dup( expand );
		else
			pRoom->exit[ door ] ->vName = str_dup( "" );
	}
	else if ( !str_cmp( command, "textout" ) )
	{
		if ( expand[ 0 ] == '\0' )
			return ;

		if ( !pRoom->exit[ door ] )
			return ;

		free_string( pRoom->exit[ door ] ->TextOut );

		if ( str_cmp( expand, "none" ) )
			pRoom->exit[ door ] ->TextOut = str_dup( expand );
		else
			pRoom->exit[ door ] ->TextOut = str_dup( "" );
	}
	else if ( !str_cmp( command, "textin" ) )
	{
		if ( expand[ 0 ] == '\0' )
			return ;

		if ( !pRoom->exit[ door ] )
			return ;

		free_string( pRoom->exit[ door ] ->TextIn );

		if ( str_cmp( expand, "none" ) )
			pRoom->exit[ door ] ->TextIn = str_dup( expand );
		else
			pRoom->exit[ door ] ->TextIn = str_dup( "" );
	}
	else if ( !str_cmp( command, "biernik" ) )
	{
		if ( expand[ 0 ] == '\0' )
			return ;

		if ( !pRoom->exit[ door ] )
			return ;

		free_string( pRoom->exit[ door ]->biernik );

		if ( str_cmp( expand, "none" ) )
			pRoom->exit[ door ]->biernik = str_dup( expand );
		else
			pRoom->exit[ door ]->biernik = str_dup( "" );
	}
	else if ( !str_cmp( command, "liczba" ) )
	{
		if ( expand[ 0 ] == '\0' )
			return ;

		if ( !pRoom->exit[ door ] )
			return ;

		if (!str_prefix(expand,"mnoga"))
		{
			pRoom->exit[door]->liczba_mnoga = TRUE;
		}
		else
		{
			pRoom->exit[door]->liczba_mnoga = FALSE;
		}
	}

	return ;
}

/*
 * mob setaff victim affvec duration text TRUE/FALSE
 */
PROGFUN( do_mpsetaff )
{
	CHAR_DATA * victim;
	BITVECT_DATA * vector;
	AFFECT_DATA af;
	char arg[ MAX_INPUT_LENGTH ];
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char arg3[ MAX_INPUT_LENGTH ];
	int duration;
	char *info = NULL;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		bug_prog_format( "mpsetaff: missing 1st argument" );
		return;
	}

	victim = resolve_char_arg( arg );
	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
		return ;

	argument = one_argument( argument, arg1 );

	if ( arg1[ 0 ] == '\0' )
	{
		bug_prog_format( "mpsetaff: missing 2nd argument" );
		return;
	}

	vector = ext_flag_lookup( arg1, affect_flags );

	if ( vector == &EXT_NONE )
		return ;

	argument = one_argument( argument, arg2 );

	if ( arg2[ 0 ] == '\0' || !is_number( arg2 ) )
	{
		bug_prog_format( "mpsetaff: missing 3rd argument" );
		return;
	}

	argument = case_one_argument( argument, arg3 );

	duration = atoi( arg2 );

	if ( duration < 0 )
		return ;

	af.where = TO_AFFECTS;
	af.type = 0;
	af.level = 0;
	af.duration = duration;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = vector;

	if ( arg3[ 0 ] != '\0' )
		info = str_dup( arg3 );

	affect_to_char( victim, &af, info, argument[ 0 ] != '\0' ? TRUE : FALSE );

	return ;
}

/*
 * mob delaff victim 'nazwany affect'
 */
PROGFUN( do_mpdelaff )
{
	CHAR_DATA * victim;
	AFFECT_DATA *pAf, *pAf_next;
	char vict[ MAX_STRING_LENGTH ];
	char aff_name[ MAX_STRING_LENGTH ];

	argument = one_argument( argument, vict );

	if ( vict[ 0 ] == '\0' )
	{
		bug_prog_format( "mpdelaff: missing 1st argument" );
		return;
	}

	victim = resolve_char_arg( vict );
	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, vict ) ) == NULL )
		return ;

	argument = one_argument( argument, aff_name );

	if ( aff_name[ 0 ] == '\0' )
	{
		bug_prog_format( "mpdelaff: missing 2nd argument" );
		return;
	}

	for ( pAf = victim->affected; pAf != NULL; pAf = pAf_next )
	{
		pAf_next = pAf->next;

		if ( pAf->info && !str_cmp( pAf->info, aff_name ) )
			affect_remove( victim, pAf );
	}

	return ;
}

/*
 * mob addaf victim location mod [level] duration
 */
PROGFUN( do_mpaddaff )
{
	CHAR_DATA * victim;
	int location, sn = -1, lev = 0, duration;
	AFFECT_DATA *pAf;
	char loc[ MAX_STRING_LENGTH ];
	char mod[ MAX_STRING_LENGTH ];
	char vict[ MAX_STRING_LENGTH ];
	char level[ MAX_STRING_LENGTH ];
	char dur_str[ MAX_STRING_LENGTH ];
	char str_info[ MAX_STRING_LENGTH ];
	char *info = NULL;

	argument = one_argument( argument, vict );

	if ( vict[ 0 ] == '\0' )
	{
		bug_prog_format( "mpaddaff: missing 1st argument" );
		return;
	}

	victim = resolve_char_arg( vict );
	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, vict ) ) == NULL )
		return ;

	argument = one_argument( argument, loc );

	if ( loc[ 0 ] == '\0' )
	{
		bug_prog_format( "mpaddaff: missing 2nd argument" );
		return;
	}

	if ( ( location = flag_value( apply_flags, loc ) ) == NO_FLAG )
		return ;

	argument = one_argument( argument, mod );

	if ( mod[ 0 ] == '\0' )
	{
		bug_prog_format( "mpaddaff: missing 3rd argument" );
		return;
	}

	if ( location == APPLY_SKILL )
	{
		sn = skill_lookup( mod );
		argument = one_argument( argument, level );

		if ( sn < 0 || level[ 0 ] == '\0' || !is_number( level ) || skill_table[ sn ].spell_fun != spell_null )
			return ;

		lev = atoi( level );
	}
	else if ( location == APPLY_LANG )
	{
		sn = get_langnum( mod );
		argument = one_argument( argument, level );

		if ( sn < 0 || sn > MAX_LANG || level[ 0 ] == '\0' || !is_number( level ) )
			return ;

		lev = atoi( level );
	}
	else if ( location == APPLY_MEMMING )
	{
		argument = one_argument( argument, level );

		if ( !is_number( mod ) || level[ 0 ] == '\0' || !is_number( level ) )
			return ;

		sn = atoi( mod );

		if ( sn < 1 || sn > 9 )
			return ;

		lev = atoi( level );
	}
	else if ( !is_number( mod ) )
		return ;

	argument = one_argument( argument, dur_str );

	if ( !is_number( dur_str ) )
		return ;

	duration = atoi( dur_str );

	argument = one_argument( argument, str_info );

	if ( str_info[ 0 ] != '\0' )
		info = str_dup( str_info );

	pAf = new_affect();
	pAf->location = location;

	if ( sn < 0 )
	{
		pAf->modifier = atoi( mod );
		pAf->level = 0;
	}
	else
	{
		pAf->modifier = sn;
		pAf->level = lev;
	}

	pAf->where	= TO_AFFECTS;
	pAf->type = 0;
	pAf->duration = duration;
	pAf->rt_duration = 0;
	pAf->bitvector = &AFF_NONE;
	affect_to_char( victim, pAf, info, argument[ 0 ] != '\0' ? TRUE : FALSE );

	return ;
}

/*
 * mob modflag target set/mod id val
 */
PROGFUN( do_mpmodflag )
{
	PFLAG_DATA * list = NULL, *tmp = NULL;
	CHAR_DATA *victim;
	OBJ_DATA * obj;
	int val;
	char arg[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char arg3[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );
	expand_arg( expand, argument );
	argument = expand;
	argument = one_argument( argument, arg3 );

	if ( arg[ 0 ] == '\0' || arg2[ 0 ] == '\0' || arg[ 0 ] == '\0' ||
	     argument[ 0 ] == '\0' || !is_number( argument ) )
	{
		bug_prog_format( "mpmodaff: missing argument" );
		return;
	}

	if ( !str_cmp( arg, "room" ) )
		list = running_prog_env->ch->in_room->pflag_list;
	else if ( !str_cmp( arg, "global" ) )
		list = misc.global_flag_list;
	else
	{
		victim = resolve_char_arg( arg );
		obj = resolve_obj_arg( arg );

		if ( victim )
		{
			if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
				return ;

			if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
				return ;
			list = victim->pflag_list;
		}
		else if ( obj )
		{
			list = obj->pflag_list;
		}

	}

	for ( ;list; list = list->next )
	{
		if ( NOPOL( arg3[ 0 ] ) == NOPOL( list->id[ 0 ] ) && !str_cmp( arg3, list->id ) )
		{
			tmp = list;
			break;
		}
	}

	if ( !tmp )
	{
		tmp = add_flag( &list, arg3, 0 );
		if ( !tmp )
		return;
	}

	val = atoi( argument );

	if ( !str_cmp( arg2, "set" ) )
		tmp->duration = val;
	else if ( !str_cmp( arg2, "mod" ) )
		tmp->duration += val;

	save_misc_data();
	return ;
}

/*
 * mob roomdesc clear/add/format string
 */
PROGFUN( do_mproomdesc )
{
	ROOM_INDEX_DATA * room;
	char arg[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];

	if ( ( room = running_prog_env->ch->in_room ) == NULL )
		return ;

	expand_arg( expand, argument );
	argument = expand;
	argument = one_argument( argument, arg );

	if ( !str_cmp( arg, "add" ) )
	{
		char buf[ 2 * MAX_INPUT_LENGTH ];

		buf[ 0 ] = '\0';

		if ( room->description )
			strcpy( buf, room->description );

		strcat( buf, argument );
		strcat( buf, "\n\r" );
		free_string( room->description );
		room->description = str_dup( buf );

		return ;
	}
	else if ( !str_cmp( arg, "clear" ) )
	{
		free_string( room->description );
		room->description = str_dup( "" );
		return ;
	}
	else if ( !str_cmp( arg, "format" ) )
	{
		room->description = format_string( room->description );
		return ;
	}

	return ;
}

PROGFUN( do_mpmesg )
{
	CHAR_DATA * vict1, *vict2;
	char arg[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char arg3[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( arg[ 0 ] == '\0' || arg2[ 0 ] == '\0' || arg3[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpmesg: missing argument" );
		return;
	}

	vict1 = resolve_char_arg( arg );
	if ( vict1 && vict1->in_room && running_prog_env->ch->in_room && vict1->in_room != running_prog_env->ch->in_room )
		return ;
	vict2 = resolve_char_arg( arg2 );
	if ( vict2 && vict2->in_room && running_prog_env->ch->in_room && vict2->in_room != running_prog_env->ch->in_room )
		return ;

	if ( !vict1 )
		vict1 = get_char_room( running_prog_env->ch, arg );

	if ( !vict2 )
		vict2 = get_char_room( running_prog_env->ch, arg2 );

	if ( !vict1 )
		return ;

	if ( !str_cmp( arg3, "char" ) )
	{
		act_prog( argument, vict1, vict2, TO_CHAR, POS_RESTING );
		return ;
	}
	else if ( !str_cmp( arg3, "vict" ) )
	{
		if ( !vict2 || vict1 == vict2 )
			return ;

		act_prog( argument, vict1, vict2, TO_VICT, POS_RESTING );
		return ;
	}
	else if ( !str_cmp( arg3, "room" ) )
	{
		act_prog( argument, vict1, vict2, TO_NOTVICT, POS_RESTING );
		return ;
	}

	return ;
}

extern int to_hit_bonus;
extern int to_dam_bonus;

PROGFUN( do_mponehit )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, expand );
	expand_arg(arg1, expand);
	argument = one_argument( argument, expand );
	expand_arg(arg2, expand);

	if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || !is_number( arg1 ) || !is_number( arg2 ) )
	{
		bug_prog_format( "mponehit: missing argument" );
		return;
	}

	to_hit_bonus = atoi( arg1 );
	to_dam_bonus = atoi( arg2 );
	return ;
}

/*
 * mob addgroup [vnum]
 */
PROGFUN( do_mpaddgroup )
{
	char arg[ MAX_INPUT_LENGTH ];
	MOB_INDEX_DATA *pMobIndex;
	CHAR_DATA *victim;
	CHAR_DATA	*leader;
	unsigned int vnum;
	char expand[ MAX_INPUT_LENGTH ];

	expand_arg( expand, argument );
	argument = expand;
	one_argument( argument, arg );

	if ( running_prog_env->ch->in_room == NULL || arg[ 0 ] == '\0' || !is_number( arg ) )
	{
		bug_prog_format( "mpaddgroup: missing argument" );
		return;
	}

	if ( running_prog_env->type == PROG_TYPE_ROOM )
		return;
	else if ( running_prog_env->type == PROG_TYPE_OBJ )
		leader = running_prog_env->obj->carried_by;
	else
		leader = running_prog_env->ch;

	vnum = atoi( arg );

	if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
	{
		bug_prog_format( "mpaddgroup: bad mob index (%d)", vnum );
		return ;
	}

	victim = create_mobile( pMobIndex );

	if ( !victim )
		return ;

	char_to_room( victim, leader->in_room );

	/*artefact*/
	if ( is_artefact_load_mob( victim ) == 1 )
		load_artefact( NULL, NULL, victim );

	victim->master = leader;
	victim->leader = leader;

	if ( HAS_TRIGGER( victim, TRIG_ONLOAD ) && ( running_prog_env->trigger != &TRIG_ONLOAD || leader->pIndexData->vnum != vnum ) )
		mp_onload_trigger( victim );

	return ;
}

PROGFUN( do_mpdamageex )
{
    CHAR_DATA * victim = NULL, *victim_next;
    MURDER_LIST *tmp_death;
    char tmpbuf[ MAX_INPUT_LENGTH ];
    char target[ MAX_INPUT_LENGTH ];
    char min[ MAX_INPUT_LENGTH ];
    char max[ MAX_INPUT_LENGTH ];
    char type[ MAX_INPUT_LENGTH ];
    int low, high, dam_type = DAM_NONE, dam, vnum = 0;
    bool fAll = FALSE, fKill = FALSE;

    argument = one_argument( argument, target );
    argument = one_argument( argument, type );
    argument = one_argument( argument, min );
    argument = one_argument( argument, max );

    if ( target[ 0 ] == '\0' )
    {
        bug_prog_format( "mpdamageex: missing argument" );
        return;
    }

    if ( !str_cmp( target, "all" ) )
    {
        fAll = TRUE;
    }
    else
    {
        victim = resolve_char_arg( target );
        if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
        {
            return ;
        }
        if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, target ) ) == NULL )
        {
            return ;
        }
    }

    if ( is_number( type ) )
    {
        dam_type = URANGE( DAM_NONE, atoi( type ), DAM_SOUND );
    }
    else
    {
        bug_prog_format( "mpdamageex: bad damage type" );
        return ;
    }

    if ( is_number( min ) )
    {
        low = atoi( min );
    }
    else
    {
        bug_prog_format( "mpdamageex: bad damage min" );
        return ;
    }

    if ( is_number( max ) )
    {
        high = atoi( max );
    }
    else
    {
        bug_prog_format( "mpdamageex: bad damage max" );
        return ;
    }

    one_argument( argument, target );

    /*
     * If kill parameter is omitted, this command is "safe" and will not
     * kill the victim.
     */

    low /= DAMAGE_MODIFIER_PLAYER_VS_MOB;
    high /= DAMAGE_MODIFIER_PLAYER_VS_MOB;

    if ( target[ 0 ] != '\0' )
    {
        fKill = TRUE;
    }
    if ( fAll )
    {
        for ( victim = running_prog_env->ch->in_room->people; victim; victim = victim_next )
        {
            victim_next = victim->next_in_room;

            dam = fKill ? number_range( low, high ) : UMIN( victim->hit, number_range( low, high ) );

            switch ( dam_type )
            {
                case DAM_ACID:
                    acid_effect( victim, dam / 10, dam / 8, TARGET_CHAR );
                    break;
                case DAM_FIRE:
                    fire_effect( victim, dam / 10, dam / 8, TARGET_CHAR );
                    break;
                case DAM_COLD:
                    cold_effect( victim, dam / 10, dam / 8, TARGET_CHAR );
                    break;
                case DAM_POISON:
                    poison_effect( victim, dam / 10, dam / 8, TARGET_CHAR );
                    break;
                case DAM_LIGHTNING:
                    shock_effect( victim, dam / 10, dam / 8, TARGET_CHAR );
                    break;
                case DAM_SOUND:
                    sound_effect( victim, dam / 10, dam / 8, TARGET_CHAR );
                    break;
                default:
                    break;
            }


            if ( victim != running_prog_env->ch )
            {
                vnum = victim->in_room ? victim->in_room->vnum : 0;
                damage( victim, victim, dam, TYPE_UNDEFINED, dam_type, FALSE );
                if ( !IS_NPC( victim ) && !victim->in_room )
                {
                    tmp_death = malloc( sizeof( *tmp_death ) );
                    if ( IS_NPC( running_prog_env->ch ) )
                        sprintf( tmpbuf, "%s [%5d] z mob damage", running_prog_env->ch->name4, running_prog_env->ch->pIndexData->vnum );
                    else
                        sprintf( tmpbuf, "%s z mob damageex", running_prog_env->ch->name4 );
                    tmp_death->name = str_dup( tmpbuf );
                    tmp_death->char_level = 0;
                    tmp_death->victim_level = victim->level;
                    tmp_death->room = vnum;
                    tmp_death->time = current_time;
                    tmp_death->next = victim->pcdata->death_statistics.pkdeath_list;
                    victim->pcdata->death_statistics.pkdeath_list = tmp_death;
                }
            }
        }
    }
    else
    {
        dam = fKill ? number_range( low, high ) : UMIN( victim->hit, number_range( low, high ) );

        switch ( dam_type )
        {
            case DAM_ACID:
                acid_effect( victim, dam / 10, dam / 8, TARGET_CHAR );
                break;
            case DAM_FIRE:
                fire_effect( victim, dam / 10, dam / 8, TARGET_CHAR );
                break;
            case DAM_COLD:
                cold_effect( victim, dam / 10, dam / 8, TARGET_CHAR );
                break;
            case DAM_POISON:
                poison_effect( victim, dam / 10, dam / 8, TARGET_CHAR );
                break;
            case DAM_LIGHTNING:
                shock_effect( victim, dam / 10, dam / 8, TARGET_CHAR );
                break;
            case DAM_SOUND:
                sound_effect( victim, dam / 10, dam / 8, TARGET_CHAR );
                break;
            default:
                break;
        }

        vnum = victim->in_room ? victim->in_room->vnum : 0;
        damage( victim, victim, dam, TYPE_UNDEFINED, dam_type, FALSE );
        if ( !IS_NPC( victim ) && !victim->in_room )
        {
            tmp_death = malloc( sizeof( *tmp_death ) );
            if ( IS_NPC( running_prog_env->ch ) )
            {
                sprintf( tmpbuf, "%s [%5d] z mob damage", running_prog_env->ch->name4, running_prog_env->ch->pIndexData->vnum );
            }
            else
            {
                sprintf( tmpbuf, "%s z mob damageex", running_prog_env->ch->name4 );
            }
            tmp_death->name = str_dup( tmpbuf );
            tmp_death->char_level = 0;
            tmp_death->victim_level = victim->level;
            tmp_death->room = vnum;
            tmp_death->time = current_time;
            tmp_death->next = victim->pcdata->death_statistics.pkdeath_list;
            victim->pcdata->death_statistics.pkdeath_list = tmp_death;
        }
    }
    return ;
}

/*
  Tener

  questlogi - czyli informacje na temat zadañ, co¶ w rodzaju dzienniczka z zadañ w grach fabularnych

  Komendy dla gracza:

  questlog [current]
  questlog all
  questlog finished

  Wypisuje kolejno wpisy na temat zadañ aktualnie wykonywanych, wszystkich albo ukoñczonych

  Mob komendy:

  mob questlog add komu <nazwa_questloga> <tre¶æ questloga>
  mob questlog delete komu <nazwa_questloga>
  mob questlog state komu <nazwa_questloga> (finished|current)
  mob questlog inform kogo
  mob questlog currentdesc komu <nazwa_questloga> <szczegó³owe informacje na temat zadania>
  mob questlog title komu <nazwa_questloga> <tytu³>

  Questlogi s± identyfikowane przez nazwê. Wypisywane s± kolejno, jak leci w pamiêci.
  Dodanie questloga o nazwie pokrywaj±cej siê z nazw± ju¿ istniej±cego questloga jest b³êdem i nie zmienia nic.
  Usuniêcie nieistniej±cego questloga tak¿ej jest b³êdem.


  If checki:

  hasqlog <osoba> <nazwa_questloga>

 */

bool has_qlog ( CHAR_DATA * ch, char * qlog) /* Sprawdzamy czy gracz wogole ma tego questloga */
{
    QL_DATA *tmp;
    char error_buf[ MAX_STRING_LENGTH ];

    if ( !ch || !qlog )
    {
        sprintf( error_buf, "has_qlog: brak celu lub nazwy questloga" );
        bug_prog_format( error_buf );
        return FALSE;
    }


    if ( IS_NPC(ch) )
    {
        return FALSE;
    }

    for(tmp = ch->pcdata->ql_data; tmp != NULL; tmp = tmp->next)
    {
        if ( !str_cmp( qlog, tmp->qname ) )
        {
            return TRUE;
        }
    }

    return FALSE;
}

bool add_questlog( CHAR_DATA* ch, char* qlname, char* qltext, int state, int date )
{
    QL_DATA *tmp;
    char error_buf[ MAX_STRING_LENGTH ];

    /* sanity checks */

    if (!qlname )
    {
        sprintf( error_buf, "mpquestlog add: brak nazwy" );
        bug_prog_format( error_buf );
        return FALSE;
    }

    if ( !ch )
    {
        sprintf( error_buf, "mpquestlog add: brak celu [%s]", qlname );
        bug_prog_format( error_buf );
        return FALSE;
    }

    if ( !qltext )
    {
        sprintf( error_buf, "mpquestlog add: brak tekstu [%s]", qlname );
        bug_prog_format( error_buf );
        return FALSE;
    }


    if ( IS_NPC( ch ) )
    {
        sprintf( error_buf, "mpquestlog add: wykonanie na npc'u [%s]", qlname );
        bug_prog_format( error_buf );
        return FALSE;
    }

    if ( has_qlog( ch, qlname ) )
    {
        sprintf( error_buf, "mpquestlog add: cel [%s] ju¿ posiada questlog o tej nazwie [%s]", ch->name, qlname );
        bug_prog_format( error_buf );
        return FALSE;
    }

    /* main stuff */

    CREATE( tmp, QL_DATA, 1 );

    tmp->qname = str_dup( qlname );
    tmp->text = str_dup( qltext );
    tmp->state = state;
    tmp->date = date;

    tmp->next = ch->pcdata->ql_data;
    ch->pcdata->ql_data = tmp;

    return TRUE;

}


/* Usuwa questloga, zwraca TRUE w przypadku sukcesu, else FALSE */

bool del_questlog( CHAR_DATA *ch, char * qname)
{
    QL_DATA *tmp;
    QL_DATA **previous_next; // wska¼nik na zmienn± która wskazywa³a na usuniêtego questloga
    char error_buf[ MAX_STRING_LENGTH ];

    previous_next = &(ch->pcdata->ql_data); // gdyby siê okaza³o, ¿e usuwany questlog jest pierwszy na li¶cie

    for(tmp=ch->pcdata->ql_data;tmp != NULL; tmp = tmp->next)
    {
        if ( !str_cmp( tmp->qname, qname ) )
        {
            *previous_next = tmp->next;
            DISPOSE(tmp);
            return TRUE;
        }
        previous_next = &(tmp->next);
    }

    sprintf( error_buf, "mpquestlog delete: nie znaleziono questloga o nazwie [%s]", qname );
    bug_prog_format( error_buf );

    return FALSE;
}

/* zmienia stan podanego questloga */

bool state_questlog( CHAR_DATA* ch, char* qname, int state )
{
    QL_DATA *tmp;
    char error_buf[ MAX_STRING_LENGTH ];

    for(tmp=ch->pcdata->ql_data;tmp != NULL; tmp = tmp->next)
    {
        if ( !str_cmp( tmp->qname, qname ) )
        {
            tmp->state = state;
            return TRUE;
        }
    }

    sprintf( error_buf, "mpquestlog state: nie znaleziono questloga o nazwie [%s]", qname );
    bug_prog_format( error_buf );

    return FALSE;

}

int ql_compare( const void* a, const void* b )
{
    QL_DATA** qa = (QL_DATA**) a;
    QL_DATA** qb = (QL_DATA**) b;
    return strcmp( (*qa)->qname, (*qb)->qname );
}

// sortowanie questlogów wed³ug qname
void sort_questlogs( CHAR_DATA * ch )
{
    // FIXME
    return;

    QL_DATA *tmp;
    QL_DATA **array; // tablica wska¼ników na elementy
    int count = 0;

    for(tmp=ch->pcdata->ql_data;tmp != NULL; tmp = tmp->next)
    {
        count++;
    }

    // FIXME
    //CREATE( array, (QL_DATA*), count+1 );

    array[count] = NULL; // ostatnie pole to wska¼nik na NULL

    int pos = 0;

    for(tmp=ch->pcdata->ql_data;tmp != NULL; tmp = tmp->next)
    {
        array[pos] = tmp;
        pos++;
    }

    qsort( array, count, sizeof(QL_DATA), &ql_compare );

    for(pos=0; pos < count; pos++)
    {
        // FIXME
        array[pos]->next = array[pos+1];
    }

    ch->pcdata->ql_data = array[0];

    DISPOSE( array );
}

bool title_questlog( CHAR_DATA* ch, char* qname, char* title )
{
    QL_DATA *tmp;
    char error_buf[ MAX_STRING_LENGTH ];

    for(tmp=ch->pcdata->ql_data;tmp != NULL; tmp = tmp->next)
    {
        if ( !str_cmp( tmp->qname, qname ) )
        {
            tmp->title = str_dup( title );
            return TRUE;
        }
    }

    sprintf( error_buf, "mpquestlog title: nie znaleziono questloga o nazwie [%s]", qname );
    bug_prog_format( error_buf );

    return FALSE;
}

bool currentdesc_questlog( CHAR_DATA* ch, char* qname, char* currentdesc )
{
    QL_DATA *tmp;
    char error_buf[ MAX_STRING_LENGTH ];

    for(tmp=ch->pcdata->ql_data;tmp != NULL; tmp = tmp->next)
    {
        if ( !str_cmp( tmp->qname, qname ) )
        {
            tmp->currentdesc = str_dup( currentdesc );
            return TRUE;
        }
    }

    sprintf( error_buf, "mpquestlog title: nie znaleziono questloga o nazwie [%s]", qname );
    bug_prog_format( error_buf );

    return FALSE;
}

/*
 * No i komenda progowa
 */
PROGFUN( do_mpquestlog )
{
    CHAR_DATA *cel;

    char komenda[ MAX_STRING_LENGTH ];
    char kto[ MAX_STRING_LENGTH ];
    char qlog[ MAX_STRING_LENGTH ];

    char error_buf[ MAX_STRING_LENGTH ];

    smash_tilde( argument ); // aby nie przypa³êta³a nam siê gdzie¶ tylda

    argument = one_argument( argument, komenda );
    argument = one_argument( argument, kto );
    argument = one_argument( argument, qlog );

    cel = resolve_char_arg( kto );

    if ( !cel )
    {
        sprintf( error_buf, "mpquestlog: z³y cel lub brak celu [%s]", kto );
        bug_prog_format( error_buf );
        return;
    }

    // dopasowanie do poszczególnych komend
    if ( !str_cmp( komenda, "add" ) ) // dodajemy, domy¶lnie jako aktualny
    {
        add_questlog( cel, qlog, argument, QL_STATE_CURRENT, time( 0 ) );
    }
    else if ( !str_cmp( komenda, "delete" ) ) // kasujemy
    {
        del_questlog( cel, qlog );
    }
    else if ( !str_cmp( komenda, "state" ) ) // zmiana stanu
    {
        int state;
        if ( !str_cmp( argument, "current" ) )
        {
            state = QL_STATE_CURRENT;
        }
        else if ( !str_cmp( argument, "finished" ) )
        {
            state = QL_STATE_FINISHED;
        }
        else
        {
            sprintf( error_buf, "mpquestlog: podano b³êdny stan quesloga: [%s]", argument );
            bug_prog_format( error_buf );
            return;
        }
        state_questlog( cel, qlog, state );
    }
    else if ( !str_cmp( komenda, "inform" ) ) // poinformujemy gracza o zmianie dziennika
    {
        send_to_char("{GDziennik zadañ zosta³ aktualizowany.{x\r\n", cel );
    }
    else if ( !str_cmp( komenda, "title" ) )
    {
        title_questlog( cel, qlog, argument );
    }
    else if ( !str_cmp( komenda, "currentdesc" ) )
    {
        currentdesc_questlog( cel, qlog, argument );
    }
    else // hmm, nic nie pasuje
    {
        sprintf( error_buf, "mpquestlog: nieznana komenda [%s]", komenda );
        bug_prog_format( error_buf );
        return;
    }

    // sortowanie questlogów wed³ug qname
    sort_questlogs( cel );

    return;
}

/*
 * mob applypoison victim/all poison_level
 */
PROGFUN( do_mpapplypoison )
{
	CHAR_DATA * victim;
	CHAR_DATA *victim_next;
	char vict[ MAX_STRING_LENGTH ];

	argument = one_argument( argument, vict );

	if ( vict[ 0 ] == '\0' )
	{
		bug_prog_format( "mpapplypoison: missing 1st argument" );
		return;
	}

	if ( argument[ 0 ] == '\0' || !is_number( argument ) )
	{
		bug_prog_format( "mpapplypoison: missing 2nd argument or not number" );
		return;
	}

	if ( !str_cmp( vict, "all" ) )
	{
		for ( victim = running_prog_env->ch->in_room->people; victim; victim = victim_next )
		{
			victim_next = victim->next_in_room;

			if ( victim != running_prog_env->ch )
				poison_to_char( victim, atoi( argument ) );
		}

		return ;
	}

	victim = resolve_char_arg( vict );

	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, vict ) ) == NULL )
		return ;

	poison_to_char( victim, atoi( argument ) );
	return ;
}

/*
 * mob mobstring target name/short/long/title string
 * mob mobstring target desc clear/add/format string
 */
PROGFUN( do_mpmobstring )
{
	char arg[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char expand[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	expand_arg( expand, argument );
	argument = expand;
	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpmobstring: missing argument" );
		return;
	}

	if ( !str_cmp( arg, "self" ) )
		victim = running_prog_env->ch;
	else
		if ( ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
			return ;

	argument = one_argument( argument, arg );
	sprintf( arg2, "%s", argument );

	if ( !str_cmp( arg, "name" ) )
	{
		if ( !IS_NPC( victim ) )
			return ;
		free_string( victim->name );
		victim->name = str_dup( arg2 );
		return ;
	}
	else if ( !str_cmp( arg, "short" ) )
	{
		free_string( victim->short_descr );
		victim->short_descr = str_dup( arg2 );
		return ;
	}
	else if ( !str_cmp( arg, "long" ) )
	{
		free_string( victim->long_descr );
		victim->long_descr = str_dup( arg2 );
		return ;
	}
	else if ( !str_cmp( arg, "title" ) )
	{
		if ( IS_NPC( victim ) )
			return ;
		set_title( victim, arg2 );
		return ;
	}
	else if ( !str_cmp( arg, "desc" ) )
	{
		argument = one_argument( argument, arg );
		if ( !str_cmp( arg, "add" ) )
		{
			char buf[ 2 * MAX_INPUT_LENGTH ];

			buf[ 0 ] = '\0';
			strcpy( buf, victim->description );
			strcat( buf, argument );
			strcat( buf, "\n\r" );
			free_string( victim->description );
			victim->description = str_dup( buf );
			return ;
		}
		else if ( !str_cmp( arg, "clear" ) )
		{
			free_string( victim->description );
			victim->description = str_dup( "" );
			return ;
		}
		else if ( !str_cmp( arg, "format" ) )
		{
			victim->description = format_string( victim->description );
			return ;
		}
	}
	return ;
}

/*
 * mob objstring target name/short/long string
 * mob objstring target desc clear/add/format string
 * mob objstring target ident clear/add/format string
 * mob objstring target extra add/del/format/clear name
 * mob objstring target extra addline name string
 */
void objstring( OBJ_DATA *obj, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_STRING_LENGTH ];
	char expand[ MAX_STRING_LENGTH ];
	char keyword[ MAX_INPUT_LENGTH ];
	EXTRA_DESCR_DATA *ed;

	expand_arg( expand, argument );
	argument = expand;
	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
		return;

	sprintf( arg2, "%s", argument );

	if ( !str_cmp( arg, "name" ) )
	{
		free_string( obj->name );
		obj->name = str_dup( arg2 );
		return ;
	}
	else if ( !str_cmp( arg, "short" ) )
	{
		free_string( obj->short_descr );
		obj->short_descr = str_dup( arg2 );
		return ;
	}
	else if ( !str_cmp( arg, "long" ) )
	{
		free_string( obj->description );
		obj->description = str_dup( arg2 );
		return ;
	}
	else if ( !str_cmp( arg, "desc" ) )
	{
		argument = one_argument( argument, arg );
		if ( !str_cmp( arg, "add" ) )
		{
			char buf[ MAX_STRING_LENGTH ];

			buf[ 0 ] = '\0';
			if ( obj->item_description ) strcpy( buf, obj->item_description );
			strcat( buf, argument );
			strcat( buf, "\n\r" );
			free_string( obj->item_description );
			obj->item_description = str_dup( buf );
			return ;
		}
		else if ( !str_cmp( arg, "clear" ) )
		{
			free_string( obj->item_description );
			obj->item_description = str_dup( "" );
			return ;
		}
		else if ( !str_cmp( arg, "format" ) )
		{
			obj->item_description = format_string( obj->item_description );
			return ;
		}
	}
	else if ( !str_cmp( arg, "ident" ) )
	{
		argument = one_argument( argument, arg );
		if ( !str_cmp( arg, "add" ) )
		{
			char buf[ MAX_STRING_LENGTH ];

			buf[ 0 ] = '\0';
			if ( obj->ident_description ) strcpy( buf, obj->ident_description );
			strcat( buf, argument );
			strcat( buf, "\n\r" );
			free_string( obj->ident_description );
			obj->ident_description = str_dup( buf );
			return ;
		}
		else if ( !str_cmp( arg, "clear" ) )
		{
			free_string( obj->ident_description );
			obj->ident_description = str_dup( "" );
			return ;
		}
		else if ( !str_cmp( arg, "format" ) )
		{
			obj->ident_description = format_string( obj->ident_description );
			return ;
		}
	}
	else if ( !str_cmp( arg, "hidden" ) )
	{
		argument = one_argument( argument, arg );
		if ( !str_cmp( arg, "add" ) )
		{
			char buf[ MAX_STRING_LENGTH ];

			buf[ 0 ] = '\0';
			if ( obj->hidden_description ) strcpy( buf, obj->hidden_description );
			strcat( buf, argument );
			strcat( buf, "\n\r" );
			free_string( obj->hidden_description );
			obj->hidden_description = str_dup( buf );
			return ;
		}
		else if ( !str_cmp( arg, "clear" ) )
		{
			free_string( obj->hidden_description );
			obj->hidden_description = str_dup( "" );
			return ;
		}
		else if ( !str_cmp( arg, "format" ) )
		{
			obj->hidden_description = format_string( obj->hidden_description );
			return ;
		}
	}
	else if ( !str_cmp( arg, "extra" ) )
	{
		argument = one_argument( argument, arg );
		argument = one_argument( argument, keyword );
		if ( !str_cmp( arg, "add" ) )
		{
			if ( keyword[ 0 ] == '\0' )
			{
				return ;
			}

			ed = new_extra_descr();
			ed->keyword = str_dup( keyword );
			ed->description = str_dup( "" );
			ed->next = obj->extra_descr;
			obj->extra_descr = ed;

			return ;
		}

		if ( !str_cmp( arg, "del" ) )
		{
			EXTRA_DESCR_DATA * ped = NULL;

			if ( keyword[ 0 ] == '\0' ) return ;

			for ( ed = obj->extra_descr; ed; ed = ed->next )
			{
				if ( is_name( keyword, ed->keyword ) )
					break;
				ped = ed;
			}

			if ( !ed ) return ;

			if ( !ped )
				obj->extra_descr = ed->next;
			else
				ped->next = ed->next;

			free_extra_descr( ed );
			return ;
		}

		if ( !str_cmp( arg, "format" ) )
		{
			if ( keyword[ 0 ] == '\0' ) return ;

			for ( ed = obj->extra_descr; ed; ed = ed->next )
			{
				if ( is_name( keyword, ed->keyword ) )
					break;
			}

			if ( !ed ) return ;

			ed->description = format_string( ed->description );
			return ;
		}

		if ( !str_cmp( arg, "clear" ) )
		{
			if ( keyword[ 0 ] == '\0' ) return ;

			for ( ed = obj->extra_descr; ed; ed = ed->next )
			{
				if ( is_name( keyword, ed->keyword ) )
					break;
			}

			if ( !ed ) return ;

			free_string( ed->description );
			ed->description = str_dup( "" );
			return ;
		}

		if ( !str_cmp( arg, "addline" ) )
		{
			char buf[ MAX_STRING_LENGTH ];
			if ( keyword[ 0 ] == '\0' ) return ;

			for ( ed = obj->extra_descr; ed; ed = ed->next )
			{
				if ( is_name( keyword, ed->keyword ) )
					break;
			}

			if ( !ed ) return ;

			buf[ 0 ] = '\0';
			if ( ed->description ) strcpy( buf, ed->description );
			strcat( buf, argument );
			strcat( buf, "\n\r" );
			free_string( ed->description );
			ed->description = str_dup( buf );
			return ;
		}

	}
	return ;
}

PROGFUN( do_mpobjstring )
{
	char arg[ MAX_STRING_LENGTH ];
	OBJ_DATA *obj;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpobjstring: missing argument" );
		return;
	}

	obj = resolve_obj_arg( arg );

	if ( obj == NULL && ( obj = get_obj_here( running_prog_env->ch, arg ) ) == NULL )
		return ;

	expand_arg( arg, argument );

	objstring( obj, arg );
}

PROGFUN( do_mplog )
{
	char arg[ MAX_STRING_LENGTH ];
	char arg2[ MAX_STRING_LENGTH ];

	if ( argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mplog: missing argument" );
		return;
	}
	expand_arg( arg, argument );
	sprintf( arg2, "mob log: %s", arg );

	log_string( arg2 );
	return ;
}

PROGFUN( do_mpsay )
{
	CHAR_DATA * v_ch;
	char buf[ MAX_STRING_LENGTH ];
	int speaking;
	int speakswell;
	char *sbuf;

	speaking = running_prog_env->ch->speaking;

	if ( IS_AFFECTED( running_prog_env->ch, AFF_SILENCE ) )
		return ;

	if ( IS_SET( sector_table[ running_prog_env->ch->in_room->sector_type ].flag, SECT_UNDERWATER ) )
	{
		act( "$n probuje co¶ powiedzieæ, chmura b±belków powietrza wydobywa siê $o z ust.", running_prog_env->ch, NULL, NULL, TO_ROOM ) ;
		return ;
	}

	if ( speaking < 0 || speaking > MAX_LANG )
	{
		running_prog_env->ch->speaking = 0;
		return ;
	}

	if ( argument[ 0 ] == '\0' )
		return ;

	for ( v_ch = running_prog_env->ch->in_room->people; v_ch; v_ch = v_ch->next_in_room )
	{
		sbuf = argument;

		if ( v_ch == running_prog_env->ch || IS_AFFECTED( v_ch, AFF_DEAFNESS ) )
			continue;

		speakswell = UMIN( knows_language( v_ch, running_prog_env->ch->speaking, running_prog_env->ch ),
		                   knows_language( running_prog_env->ch, running_prog_env->ch->speaking, v_ch ) );

		if ( speakswell < 95 )
			sbuf = translate( speakswell, argument, lang_table[ speaking ].name );

		sprintf( buf, "{6%s mówi '{7%s{6'{x", PERS( running_prog_env->ch, v_ch ), sbuf );
		act_prog( buf, running_prog_env->ch, v_ch, TO_VICT, POS_RESTING );
	}

	return ;
}

PROGFUN( do_mpsayto )
{
	CHAR_DATA * v_ch;
	CHAR_DATA * victim;
	char name3[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	int speaking;
	int speakswell;
	char *sbuf;

	speaking = running_prog_env->ch->speaking;

	if ( IS_AFFECTED( running_prog_env->ch, AFF_SILENCE ) )
		return ;

	if ( IS_SET( sector_table[ running_prog_env->ch->in_room->sector_type ].flag, SECT_UNDERWATER ) )
	{
		act( "$n probuje co¶ powiedzieæ, chmura b±belków powietrza wydobywa siê $o z ust.", running_prog_env->ch, NULL, NULL, TO_ROOM ) ;
		return ;
	}

	if ( speaking < 0 || speaking > MAX_LANG )
	{
		running_prog_env->ch->speaking = 0;
		return ;
	}

	sbuf = argument;
	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
		return ;

	victim = resolve_char_arg( arg );
	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
		return ;

	if ( running_prog_env->ch == victim )
		return ;

	for ( v_ch = running_prog_env->ch->in_room->people; v_ch; v_ch = v_ch->next_in_room )
	{
		sbuf = argument;

		if ( v_ch == running_prog_env->ch || IS_AFFECTED( v_ch, AFF_DEAFNESS ) )
			continue;

		speakswell = UMIN( knows_language( v_ch, running_prog_env->ch->speaking, running_prog_env->ch ),
		                   knows_language( running_prog_env->ch, running_prog_env->ch->speaking, v_ch ) );

		if ( speakswell < 95 )
			sbuf = translate( speakswell, argument, lang_table[ speaking ].name );

		if ( can_see( v_ch, victim ) )
			sprintf( name3, "%s '{7%s{6'{x", victim->name3, sbuf );
		else
			sprintf( name3, "komu¶ '{7%s{6'{x", sbuf );

		if ( v_ch == victim )
			act( "{6$n mówi tobie '{7$t{6'{x", running_prog_env->ch, sbuf, v_ch, TO_VICT );
		else
			act( "{6$n mówi $t", running_prog_env->ch, name3, v_ch, TO_VICT );
	}

	return ;
}

PROGFUN( do_mpcommand )
{
	if ( !running_prog_env->vch || !running_prog_env->vch->precommand_fun )
		return ;

	running_prog_env->vch->precommand_pending = TRUE;
	do_function( running_prog_env->vch, running_prog_env->vch->precommand_fun, running_prog_env->vch->precommand_arg );
	running_prog_env->vch->precommand_pending = FALSE;
	running_prog_env->vch->precommand_fun = NULL;
	free_string( running_prog_env->vch->precommand_arg );
	running_prog_env->vch->precommand_arg = NULL;
	return ;
}

PROGFUN( do_mpwalk )
{
	char arg[ MAX_INPUT_LENGTH ];
	char expand[ MAX_STRING_LENGTH ];

	expand_arg( expand, argument );
	argument = expand;
	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || !is_number( arg ) )
	{
		bug_prog_format( "mpsetpos: missing argument or not number" );
		return;
	}

	if ( argument[ 0 ] == '\0' )
		running_prog_env->ch->walking = atoi( arg );
	else
		running_prog_env->ch->walking = -1 * atoi( arg );
	return ;
}

PROGFUN( do_mpsetpos )
{
	CHAR_DATA * victim;
	char arg[ MAX_INPUT_LENGTH ];
	int pos;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mpsetpos: missing argument" );
		return;
	}

	victim = resolve_char_arg( arg );
	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
		return ;

	if ( ( pos = position_lookup( argument ) ) == -1 )
		return ;

	if ( victim->on )
		victim->on = NULL;

	victim->position = pos;
	return ;
}

PROGFUN( do_mpsetposdefaul )
{
	CHAR_DATA * victim;
	char arg[ MAX_INPUT_LENGTH ];
	int pos;

	//DEBUG_INFO( "mpsetposdefault: enter" );
	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		bug_prog_format( "mp_setposdefault: missing argument" );
		return;
	}

	victim = resolve_char_arg( arg );
	if (!victim)
	   {
	      bug_prog_format( "mp_setposdefault: null victim" );
	   }
	if (!IS_NPC(victim))
	   {
	      bug_prog_format( "mp_setposdefault: used on player character" );
	      return;
	   }
	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, arg ) ) == NULL )
		return ;

	if ( ( pos = position_lookup( argument ) ) == -1 )
		return ;

	if ( victim->on )
		victim->on = NULL;
	//victim->pIndexData->start_pos = pos;
	//victim->pIndexData->default_pos = pos;
	victim->default_pos = pos;
	return ;
}

//dziala jak immo peace
PROGFUN( do_mppeace )
{
	CHAR_DATA * rch;
	for ( rch = running_prog_env->ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( rch->fighting != NULL )
			stop_fighting( rch, TRUE );
		if ( IS_NPC( rch ) && EXT_IS_SET( rch->act, ACT_AGGRESSIVE ) )
			EXT_REMOVE_BIT( rch->act, ACT_AGGRESSIVE );

		stop_hating( rch, NULL, TRUE );
		stop_hunting( rch );
		stop_fearing( rch, NULL, TRUE );
	}
	return ;
}

//Komenda do nakladania gsn_on_smoke do palenia mudowych fajek
//z zielem. Najaranie objawia sie extra desciem postaci, wykonywaniem randomowo
//roznych sociali itd. Spalenie trwa ticka, jezeli ktos juz jest najarany to
//duration zwieksza sie, do max 7 tickow

//teraz to jest w pipes.c
/*PROGFUN( do_mpsmoke )
{
	CHAR_DATA *victim;
	char vict[ MAX_STRING_LENGTH ];
	AFFECT_DATA *old_smoke;
	AFFECT_DATA new_smoke;
	int dur = 1, old_dur = 0, old_level = 0, level = 1;

	argument = one_argument( argument, vict );

	if ( vict[ 0 ] == '\0' )
	{
		bug_prog_format( "mpsmoke: nie podano victima" );
		return;
	}

	victim = resolve_char_arg( vict );

	if ( victim && victim->in_room && running_prog_env->ch->in_room && victim->in_room != running_prog_env->ch->in_room )
		return ;

	if ( victim == NULL && ( victim = get_char_room( running_prog_env->ch, vict ) ) == NULL )
		return ;

	if ( is_affected(victim, gsn_on_smoke ))
	{
		old_smoke = affect_find( victim->affected, gsn_on_smoke );
		old_dur = old_smoke->duration;
		old_level = old_smoke->level;
		affect_strip( victim, gsn_on_smoke );
	}

	dur += old_dur;
	level += old_level;

	new_smoke.where = TO_AFFECTS;
	new_smoke.type = gsn_on_smoke;
	new_smoke.level = level;
	new_smoke.duration = UMIN(dur,number_range(5,7));
	new_smoke.rt_duration = 0;
	new_smoke.location = APPLY_NONE;
	new_smoke.modifier = 0;
	new_smoke.bitvector = AFF_NONE;

	if( level > number_range(5,7) )
	{
		if( dice(1,2) == 1 )
			new_smoke.bitvector = AFF_HALLUCINATIONS_NEGATIVE;
		else
			new_smoke.bitvector = AFF_HALLUCINATIONS_POSITIVE;
	}
	new_smoke.visible = FALSE;
	affect_to_char( victim, &new_smoke, NULL, FALSE );

	return ;
}*/
