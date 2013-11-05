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
 * KILLER MUD is copyright 1999-2012 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Jaron Krzysztof       (chris.jaron@gmail.com           ) [Razor   ] *
 * Pietrzak Marcin       (marcin@iworks.pl                ) [Gurthg  ] *
 * Sawicki Tomasz        (furgas@killer-mud.net           ) [Furgas  ] *
 * Trebicki Marek        (maro@killer.radom.net           ) [Maro    ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl        ) [Agron   ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: act_obj.c 12052 2013-02-19 20:41:24Z raszer $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/12.02/src/act_obj.c $
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
#include "merc.h"
#include "interp.h"
#include "lang.h"
#include "tables.h"
#include "progs.h"
#include "traps.h"
#include "act_obj.h"
#include "money.h"

void raw_damage     args ( (CHAR_DATA *ch, CHAR_DATA *victim, int dam ) );
void spell_identify args ( ( int sn, int level, CHAR_DATA *ch, void *vo, int target ) );
void turn_into_dust_objects_sensitive_to_light args( ( CHAR_DATA *ch, int dmg ) );

/*
 * Local functions.
 */
#define CD CHAR_DATA
#define OD OBJ_DATA
bool remove_obj            args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
void wear_obj              args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
CD * find_keeper           args( ( CHAR_DATA *ch, char *name ) );
int  get_cost              args( ( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy ) );
void obj_to_keeper         args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
OD * get_obj_keeper        args( ( CHAR_DATA *ch, CHAR_DATA *keeper, char *argument ) );
bool check_only_weight_cant_equip       args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void wield_weapon          args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool primary ) );
int  find_door             args( ( CHAR_DATA *ch, char *arg ) );
bool mp_precommand_trigger args( ( CHAR_DATA *mob, CHAR_DATA *victim, OBJ_DATA *obj, DO_FUN * fun, char *fun_name, char *argument ) );
bool op_precommand_trigger args( ( CHAR_DATA *ch, OBJ_DATA *obj1, OBJ_DATA *obj2, DO_FUN * fun, char *fun_name, char *argument ) );
bool arte_can_load         args( (ush_int vnum) );
void one_hit	           args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool second ) );
void                       poison_from_food( OBJ_DATA *obj, CHAR_DATA *victim );
REPAIR_DATA *              get_repair_data( REPAIR_DATA *list, OBJ_DATA *obj );
CHAR_DATA *                find_repair( CHAR_DATA *ch, char *name );

#undef OD
#undef CD

/* RT part of the corpse looting code */

void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
	CHAR_DATA * gch;
	char buf[ MAX_STRING_LENGTH ];

	if ( !CAN_WEAR( obj, ITEM_TAKE ) )
	{
		send_to_char( "Nie mo¿esz tego wzi±æ.\n\r", ch );
		return;
	}

	if ( !IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, ITEM_TAKE ) )
    {
		return;
    }

	if ( obj->item_type != ITEM_MONEY && ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
	{
		act( "$p: nie mo¿esz nosiæ tylu rzeczy.", ch, obj, NULL, TO_CHAR );
		return;
	}

	/* check na podnoszenie rzeczy */
	if ( obj->item_type != ITEM_MONEY
		&& ( !obj->in_obj || obj->in_obj->carried_by != ch )
		&& ( get_carry_weight( ch ) + get_obj_weight( obj ) > can_carry_w( ch ) ) )
	{
		act( "$p: to jest dla ciebie za ciê¿kie.", ch, obj, NULL, TO_CHAR );
		return;
	}

	if ( obj->in_room != NULL )
	{
		for ( gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room )
		{
			if ( gch->on == obj )
			{
				if (gch == ch)
				{
					act( "Wygl±da na to, ¿e sam<&/a/o> u¿ywasz $f.", ch, obj, gch, TO_CHAR );
				}
				else
				{
					act( "Wygl±da na to, ¿e $N u¿ywa $h.", ch, obj, gch, TO_CHAR );
				}
				act( "Wygl±da na to, ¿e $n próbuje zabraæ $p.", ch, obj, gch, TO_ROOM );
				return;
			}
		}
	}

    if ( container != NULL )
    {
        if ( IS_SET( container->value[ 1 ], CONT_PUT_ON ) )
        {
            sprintf( buf, "Podnosisz $h %s $F.",
                    ( NOPOL( container->name2[ 0 ] ) == 'Z' || NOPOL( container->name2[ 0 ] ) == 'S' )
                    ? "ze" : "z" );
            act( buf, ch, obj, container, TO_CHAR );
            sprintf( buf, "$n podnosi $h %s $F.",
                    ( NOPOL( container->name2[ 0 ] ) == 'Z' || NOPOL( container->name2[ 0 ] ) == 'S' )
                    ? "ze" : "z" );
            act( buf, ch, obj, container, TO_ROOM );
        }
        else
        {
            sprintf( buf, "Wyjmujesz $h %s $F.",
                    ( NOPOL( container->name2[ 0 ] ) == 'Z' || NOPOL( container->name2[ 0 ] ) == 'S' )
                    ? "ze" : "z" );
            act( buf, ch, obj, container, TO_CHAR );
            sprintf( buf, "$n wyjmuje $h %s $F.",
                    ( NOPOL( container->name2[ 0 ] ) == 'Z' || NOPOL( container->name2[ 0 ] ) == 'S' )
                    ? "ze" : "z" );
            act( buf, ch, obj, container, TO_ROOM );
        }

        if ( obj->item_type == ITEM_MONEY )
        {
            money_from_container( ch, obj, container );
        }
        EXT_REMOVE_BIT( obj->extra_flags, ITEM_HAD_TIMER );
        obj_from_obj( obj );
	}
	else
	{
        act( "Podnosisz $h.", ch, obj, container, TO_CHAR );
        /*
         * zapalone swiatlo?
         */
        if ( obj->item_type == ITEM_LIGHT && obj->value[4] != 0 )
        {
            --(obj->in_room->light);
            obj->value[2] = obj->value[4]; //to be on the safe side but think unnecessary
            obj->value[4] = 0;
        }
        act( "$n podnosi $h.", ch, obj, container, TO_ROOM );
        obj_from_room( obj );
    }

	if ( obj->item_type == ITEM_MONEY )
	{
        money_gain_from_obj ( ch, obj, container );
	}
	else
	{
		/*artefact*/
		if ( container == NULL )  //znaczy ze z ziemi bo nie ma contenera
		{
			if ( is_artefact( obj ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
				artefact_to_char( obj, ch );

			if ( obj->contains != NULL && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )   //cza sprawdzic czy artefakt w srodku nie siedzi
				container_artefact_check( obj, ch, 0 ); //0 - podnosimy
		}
		else //z kontenera...
		{
			if ( container->carried_by != ch )  //... w invetory (olewamy)
			{
				if ( is_artefact( obj ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
				    artefact_to_char( obj, ch );
				if ( obj->contains != NULL && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )   //cza sprawdzic czy artefakt w srodku nie siedzi
				    container_artefact_check( obj, ch, 0 ); //0 - podnosimy
			}
		}

		obj_to_char( obj, ch );

		obj_trap_handler( ch, obj, TRAP_GET );

		if ( !ch->in_room )
        {
			return;
        }

		if ( HAS_OTRIGGER( obj, TRIG_GET ) )
        {
			op_common_trigger( ch, obj, &TRIG_GET );
        }
	}

	if ( obj->material == 7 ) 
	{
	    turn_into_dust_objects_sensitive_to_light( ch, 1 );
	}

	return;
}

/* do_get */

void do_get( CHAR_DATA *ch, char *argument )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	OBJ_DATA *container;
	bool found;
	char *orig_argument;

	orig_argument = argument;
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( !str_cmp( arg2, "from" ) )
    {
		argument = one_argument( argument, arg2 );
    }

	/* Get type. */
	if ( arg1[ 0 ] == '\0' )
	{
		send_to_char( "Co chcesz wzi±æ?\n\r", ch );
		return;
	}

	if ( arg2[ 0 ] == '\0' )
	{
		if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
		{
			/* 'get obj' */
			obj = get_obj_list( ch, arg1, ch->in_room->contents );

			if ( obj == NULL )
			{
				act( "Nie widze tu czego¶ takiego jak $T.", ch, NULL, arg1, TO_CHAR );
				return;
			}
			get_obj( ch, obj, NULL );
		}
		else
		{
			/* 'get all' or 'get all.obj' */
			found = FALSE;
			for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
			{
				OBJ_NEXT_CONTENT( obj, obj_next );

				if ( ( arg1[ 3 ] == '\0' || is_name( &arg1[ 4 ], obj->name ) )
				     && can_see_obj( ch, obj ) )
				{
				    found = TRUE;
				    get_obj( ch, obj, NULL );
				}
			}
			if ( !found )
			{
				if ( arg1[ 3 ] == '\0' )
                {
				    send_to_char( "Niczego tutaj nie widzisz.\n\r", ch );
                }
				else
                {
				    act( "Nie widze tu niczego takiego jak $T.", ch, NULL, &arg1[ 4 ], TO_CHAR );
                }
			}
		}
	}
	else
	{
		/* 'get... container' */
		if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
		{
			send_to_char( "Nie mo¿esz tego zrobiæ.\n\r", ch );
			return;
		}

		if ( ( container = get_obj_here_alt( ch, arg2 ) ) == NULL )
		{
			act( "Nie widaæ tu czego¶ takiego jak $T.", ch, NULL, arg2, TO_CHAR );
			return;
		}

		switch ( container->item_type )
		{
			default:
				send_to_char( "To nie jest pojemnik.\n\r", ch );
				return;

			case ITEM_CONTAINER:
			case ITEM_CORPSE_NPC:
			case ITEM_CORPSE_PC:
				break;

		}

		if ( container->item_type == ITEM_CONTAINER && IS_SET( container->value[ 1 ], CONT_CLOSED ) )
		{
			act( "Ten pojemnik jest zamkniêty ($p).", ch, container, NULL, TO_CHAR );
			return;
		}

		if ( get_obj_memdat( container, NULL, NULL, MEMDAT_ANY ) && HAS_OTRIGGER( container, TRIG_PRECOMMAND ) )
		{
			send_to_char( "Teraz kto¶ inny wyci±ga z tego przedmioty. Poczekaj chwilê.\n\r", ch );
			return;
		}

		if ( !ch->precommand_pending && HAS_OTRIGGER( container, TRIG_PRECOMMAND ) )
		{
			if ( op_precommand_trigger( ch, container, NULL, &do_get, "get", orig_argument ) )
            {
				return;
            }
		}

 		if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
		{
			/* 'get obj container' */
			obj = get_obj_list( ch, arg1, container->contains );

			if ( obj == NULL )
			{
				act( "Nie ma w $k czego¶ takiego jak $T.", ch, container, arg1, TO_CHAR );
				return;
			}
			get_obj( ch, obj, container );
		}
		else
		{
			/* 'get all container' or 'get all.obj container' */
			found = FALSE;

			for ( obj = container->contains; obj != NULL; obj = obj_next )
			{
				OBJ_NEXT_CONTENT( obj, obj_next );
				if ( ( arg1[ 3 ] == '\0' || is_name( &arg1[ 4 ], obj->name ) ) && can_see_obj( ch, obj ) )
				{
				    found = TRUE;
				    get_obj( ch, obj, container );
				}
			}

			if ( !found )
			{
				if ( arg1[ 3 ] == '\0' )
                {
				    act( "Niestety, $T nic takiego nie zawiera.", ch, NULL, arg2, TO_CHAR );
                }
				else
                {
				    act( "Niestety, $T nic takiego nie zawiera.", ch, NULL, arg2, TO_CHAR );
                }
			}
            else
            {
				WAIT_STATE( ch, 6 );
            }
		}
	}

	if ( !ch->mount && get_carry_weight( ch ) > can_carry_w( ch ) )
	{
		if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED( ch, AFF_FLOAT))
		{
			send_to_char( "Pod wp³ywem obci±¿enia opadasz na ziemiê.\n\r", ch );
			affect_strip( ch, gsn_fly );
			affect_strip( ch, gsn_float );
		}
		return;
	}
	return;
}
/**
 * poison to drinkcontainer
 */
void do_put_envenomer_to_drink_container ( CHAR_DATA *ch, char arg1[ MAX_INPUT_LENGTH ], char arg2[ MAX_INPUT_LENGTH ] )
{
	OBJ_DATA *obj, *container;

	if ( ( container = get_obj_carry( ch, arg2, ch ) ) == NULL )
	{
		send_to_char( "Nie masz takiego pojemnika na napoj.\n\r", ch );
		return;
    }

    if
        (
         ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL ||
         obj->item_type != ITEM_ENVENOMER
        )
    {
        send_to_char( "Nie masz takiej trucizny.\n\r", ch );
        return;
    }

    if ( number_percent() < 95 )
    {
        container->value[3] = 1;
        act( "$n zatruwa $h ¶mierteln± trucizn±.", ch, container, NULL, TO_ROOM );
        act( "Zatruwasz $h trucizn±.", ch, container, NULL, TO_CHAR );
        if ( ( get_skill( ch, gsn_envenom ) ) > 0 )
        {
            check_improve( ch, NULL, gsn_envenom, TRUE, 12 );
        }
    }
    else
    {
        act( "Nie uda³o ci siê zatruæ $f.", ch, container, NULL, TO_CHAR );
        if ( ( get_skill( ch, gsn_envenom ) ) > 0 )
        {
            check_improve( ch, NULL, gsn_envenom, FALSE, 12 );
        }
    }
    WAIT_STATE( ch, dice( 2, 2) );
    if ( obj->value[ 1 ] > 0 && --obj->value[ 1 ] == 0 )
    {
        /*artefact*/
        if ( is_artefact( obj ) ) extract_artefact( obj );
        {
            extract_obj( obj );
        }
    }
    return;

}
/* do_put */
void do_put( CHAR_DATA *ch, char *argument )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	OBJ_DATA *container;
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

    if
        (
         !str_cmp( arg2, "in" ) ||
         !str_cmp( arg2, "to" ) ||
         !str_cmp( arg2, "on" )
        )
    {
		argument = one_argument( argument, arg2 );
    }

	if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
	{
		send_to_char( "Co chcia³<&/a/o>by¶ i do czego w³o¿yæ?\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
	{
		send_to_char( "Nie da rady tego zrobiæ.\n\r", ch );
		return;
	}

	if ( ( container = get_obj_here_alt( ch, arg2 ) ) == NULL )
	{
		send_to_char( "Nie masz takiego pojemnika.\n\r", ch );
		return;
	}

    if ( container->item_type == ITEM_DRINK_CON )
    {
        do_put_envenomer_to_drink_container( ch, arg1, arg2 );
        return;
    }

	if ( container->item_type != ITEM_CONTAINER )
	{
		send_to_char( "To nie jest pojemnik.\n\r", ch );
		return;
	}

	if ( IS_SET( container->value[ 1 ], CONT_CLOSED ) )
	{
		act( "Najpierw musisz otworzyæ $h.", ch, container, NULL, TO_CHAR );
		return;
	}

	if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
	{
		if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
		{
			send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
			return;
		}

		if ( obj == container )
		{
			send_to_char( "Nie mo¿esz tego wepchn±æ do tego samego.\n\r", ch );
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
		{
			print_char( ch, "Nie mo¿esz odrzuciæ tego przedmiotu ani w³o¿yæ go do %s.\n\r", container->name2 );
			return;
		}


		if ( get_obj_weight( obj ) + get_true_weight( container ) > ( container->value[ 0 ] * 22 ) ||
			 get_obj_weight( obj ) > ( container->value[ 3 ] * 22 ) ||
			 ( get_obj_number( container ) >= 1 && IS_SET( container->value[ 1 ], CONT_SINGLE_OBJ ) ) )
		{
			send_to_char( "Nie zmie¶cisz ju¿ tego.\n\r", ch );
			return;
		}

        if ( container->value[ 5 ] > 0 )
        {
            bool dont_match = TRUE;
            if
                (
                 ( IS_SET( container->value[ 1 ], CONT_ONLY_VNUM ) && obj->pIndexData->vnum == container->value[ 5 ] ) // czy tylko okreslony vnum
                 || obj->item_type == container->value[ 5 ] // czy tylko okreslony typ
                )
                {
                    dont_match = FALSE;
                }

            if ( dont_match )
            {
                send_to_char( "To nie bardzo pasuje.\n\r", ch );
                return;
            }
		}

		/*artefact*/
		if ( container->in_room )  //jesli w roomie to sie gosc pozbywa
		{
			if ( is_artefact( obj ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
				artefact_from_char( obj, ch );
		}

		obj_from_char( obj );
		obj_to_obj( obj, container );

		if ( IS_SET( container->value[ 1 ], CONT_PUT_ON ) )
		{
			act( "$n k³adzie $h na $K.", ch, obj, container, TO_ROOM );
			act( "K³adziesz $h na $K.", ch, obj, container, TO_CHAR );
		}
		else
		{
			act( "$n wk³ada $h do $F.", ch, obj, container, TO_ROOM );
			act( "Wk³adasz $h do $F.", ch, obj, container, TO_CHAR );
		}

		if ( HAS_OTRIGGER( obj, TRIG_PUT ) )
			op_put_trigger( obj, ch, container, &TRIG_PUT );
	}
	else
	{
		/* 'put all container' or 'put all.obj container' */
		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( ( arg1[ 3 ] == '\0' || is_name( &arg1[ 4 ], obj->name ) )
				 && can_see_obj( ch, obj )
				 && WEIGHT_MULT( obj ) == 100
				 && obj->wear_loc == WEAR_NONE
				 && obj != container
				 && !IS_OBJ_STAT( obj, ITEM_NODROP )
				 && get_obj_weight( obj ) + get_true_weight( container ) <= ( container->value[ 0 ] * 22 )
				 && get_obj_weight( obj ) < ( container->value[ 3 ] * 22 )
				 && ( get_obj_number( container ) <= 0 || !IS_SET( container->value[ 1 ], CONT_SINGLE_OBJ ) ) )
			{
				if ( container->value[ 5 ] > 0 )
				{
				    if ( ( container->value[ 5 ] < 50 && obj->item_type != container->value[ 5 ] ) ||
				         ( container->value[ 5 ] >= 50 && obj->pIndexData->vnum != container->value[ 5 ] ) ||
				         ( container->value[ 5 ] < 50 && obj->item_type == container->value[ 5 ] &&
				           obj->item_type == ITEM_WEAPON && ( obj->value[ 0 ] != container->value[ 6 ] || container->value[ 6 ] == -1 ) ) )
				        continue;
				}

				/*artefact*/
				if ( container->in_room )  //jesli w roomie to sie gosc pozbywa
				{
				    if ( is_artefact( obj ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
				        artefact_from_char( obj, ch );
				}

				obj_from_char( obj );
				obj_to_obj( obj, container );

				if ( IS_SET( container->value[ 1 ], CONT_PUT_ON ) )
				{
					act( "$n k³adzie $h na $K.", ch, obj, container, TO_ROOM );
					act( "K³adziesz $h na $K.", ch, obj, container, TO_CHAR );
				}
				else
				{
					act( "$n wk³ada $h do $F.", ch, obj, container, TO_ROOM );
					act( "Wk³adasz $h do $F.", ch, obj, container, TO_CHAR );
				}

				if ( HAS_OTRIGGER( obj, TRIG_PUT ) )
					op_put_trigger( obj, ch, container, &TRIG_PUT );
			}
		}
	}

	return;
}

void do_drop( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	char buf[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	bool found;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Co chcia³<&/a/o>by¶ upu¶ciæ?\n\r", ch );
		return;
	}

	if ( is_number( arg ) )
	{
		/* 'drop NNNN coins' */
		int amount, copper = 0, silver = 0, gold = 0, mithril = 0;
        int nomination = NOMINATION_NONE;

		amount = atoi( arg );
		argument = one_argument( argument, arg );

		if ( amount < 1 )
		{
			send_to_char( "Przykro mi, nie mo¿esz tego zrobiæ.\n\r", ch );
			return;
		}

        nomination = money_nomination_find( arg );
        switch ( nomination )
        {
            case NOMINATION_COPPER:
                if ( ch->copper < amount )
                {
                    send_to_char( "Nie masz tyle miedziaków.\n\r", ch );
                    return;
                }
                append_file_format_daily
                    (
                     ch,
                     MONEY_LOG_FILE,
                     "-> S: %d %d (%d) copper - wyrzuci³ kasê w lokacji [%5d]",
                     ch->copper,
                     ch->copper - amount,
                     -amount,
                     ch->in_room->vnum
                    );
                ch->copper -= amount;
                copper = amount;
                break;

            case NOMINATION_SILVER:
                if ( ch->silver < amount )
                {
                    send_to_char( "Nie masz tyle srebra.\n\r", ch );
                    return;
                }
                append_file_format_daily
                    (
                     ch,
                     MONEY_LOG_FILE,
                     "-> S: %d %d (%d) silver - wyrzuci³ kasê w lokacji [%5d]",
                     ch->silver,
                     ch->silver - amount,
                     -amount,
                     ch->in_room->vnum
                    );
                ch->silver -= amount;
                silver = amount;
                break;

            case NOMINATION_GOLD:
                if ( ch->gold < amount )
                {
                    send_to_char( "Nie masz tyle zlota.\n\r", ch );
                    return;
                }
                append_file_format_daily
                    (
                     ch,
                     MONEY_LOG_FILE,
                     "-> S: %d %d (%d) gold - wyrzuci³ kasê w lokacji [%5d]",
                     ch->gold,
                     ch->gold - amount,
                     -amount,
                     ch->in_room->vnum
                    );
                ch->gold -= amount;
                gold = amount;
                break;

            case NOMINATION_MITHRIL:
                if ( ch->mithril < amount )
                {
                    send_to_char( "Nie masz tyle mithrilu.\n\r", ch );
                    return;
                }
                append_file_format_daily
                    (
                     ch,
                     MONEY_LOG_FILE,
                     "-> S: %d %d (%d) mithril - wyrzuci³ kasê w lokacji [%5d]",
                     ch->mithril,
                     ch->mithril - amount,
                     -amount,
                     ch->in_room->vnum
                    );
                ch->mithril -= amount;
                mithril = amount;
                break;

            default:
                send_to_char( "Wybierz jaki rodzaj monet chcesz upuscic.\n\r", ch );
                return;
        }

        obj_to_room( create_money( copper, silver, gold, mithril ), ch->in_room );

        if ( amount == 1 )
        {
            sprintf ( buf, "$n upuszcza jedn± %s monetê.", money_nomination( nomination, amount ) );
			act( buf, ch, NULL, NULL, TO_ROOM );
            print_char( ch, "Upuszczasz jedna %s monetê.\n\r", money_nomination( nomination, amount) );
        }
        else
        {
            act( "$n upuszcza troche monet.", ch, NULL, NULL, TO_ROOM );
            print_char( ch, "Upuszczasz %ld %s monet.\n\r", amount, money_nomination( nomination, amount) );
        }
		return;
	}

	if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
	{
		/* 'drop obj' */
		if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
		{
			send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
			return;
		}

		if ( !can_drop_obj( ch, obj ) )
		{
			send_to_char( "Nie mo¿esz tego wypu¶ciæ z rêki.\n\r", ch );
			return;
		}

		/*artefact*/
		if ( !IS_IMMORTAL( ch ) && is_artefact( obj ) && !IS_NPC( ch ) )
			artefact_from_char( obj, ch );
		if ( obj->contains != NULL && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) )
			container_artefact_check( obj, ch, 1 ); //1 - zdejmujemy jak co¶ jest

		obj_from_char( obj );
		//ustawienie czasu na glebie
		obj->on_ground = 15;
		act( "$n upuszcza $h.", ch, obj, NULL, TO_ROOM );
		act( "Upuszczasz $h.", ch, obj, NULL, TO_CHAR );
		obj_to_room( obj, ch->in_room );

		obj_trap_handler( ch, obj, TRAP_DROP );

		if ( !ch->in_room )
			return;

		if ( HAS_OTRIGGER( obj, TRIG_DROP ) )
			op_common_trigger( ch, obj, &TRIG_DROP );

		/* wyrzucanie na wodê? */
		if ( obj_fall_into_water( ch, obj ) )
            return;

        if (
                IS_OBJ_STAT( obj, ITEM_MELT_DROP )
                || obj->material == 45 // eteryczny
           )
        {
			if ( obj->liczba_mnoga )
				act( "$p rozp³ywaj± siê w dym.", ch, obj, NULL, TO_ALL );
			else
				act( "$p rozp³ywa siê w dym.", ch, obj, NULL, TO_ALL );
			/*artefact*/
			if ( is_artefact( obj ) ) extract_artefact( obj );
			if ( obj->contains ) extract_artefact_container( obj );
			extract_obj( obj );
		}
		else if ( IS_SET( material_table[ obj->material ].flag, MAT_EASYBREAK )
				  && number_percent() > material_table[ obj->material ].hardness )
			/*	(obj->item_type==ITEM_POTION &&
					(obj->material==11 || obj->material==15) )*/
		{
			if ( obj->liczba_mnoga )
				act( "$p rozpryskuj± siê na kawa³eczki.", ch, obj, NULL, TO_ALL );
			else
				act( "$p rozpryskuje siê na kawa³eczki.", ch, obj, NULL, TO_ALL );
			/*artefact*/
			if ( is_artefact( obj ) ) extract_artefact( obj );
			if ( obj->contains ) extract_artefact_container( obj );
			extract_obj( obj );
		}
	}
	else
	{
        int i = 0;
		/* 'drop all' or 'drop all.obj' */
		found = FALSE;
		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
            i++;
            /**
             * drop all - limit to 30 items
             */
            if ( i > 36 )
            {
                act( "{RWywalasz trzy tuziny przedmiotów i wiêcej ci siê ju¿ nie chce.{x", ch, NULL, arg, TO_CHAR );
                return;
            }


			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( ( arg[ 3 ] == '\0' || is_name( &arg[ 4 ], obj->name ) )
				 && can_see_obj( ch, obj )
				 && obj->wear_loc == WEAR_NONE
				 && can_drop_obj( ch, obj ) )
			{
				found = TRUE;

				/*artefact*/
				if ( !IS_IMMORTAL( ch ) && is_artefact( obj ) && !IS_NPC( ch ) )
				    artefact_from_char( obj, ch );
				if ( obj->contains != NULL && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) )
				    container_artefact_check( obj, ch, 1 ); //1 - zdejmujemy jak co¶ jest


				obj_from_char( obj );
				//ustawienie czasu na glebie
				obj->on_ground = 15;
				act( "$n upuszcza $h.", ch, obj, NULL, TO_ROOM );
				act( "Upuszczasz $h.", ch, obj, NULL, TO_CHAR );
				obj_to_room( obj, ch->in_room );

				obj_trap_handler( ch, obj, TRAP_DROP );

				if ( !ch->in_room )
				    return;

				if ( HAS_OTRIGGER( obj, TRIG_DROP ) )
				    op_common_trigger( ch, obj, &TRIG_DROP );

				/* wyrzucanie na wodê? */
				if ( obj_fall_into_water( ch, obj ) )
                    return;

                if (
                        IS_OBJ_STAT( obj, ITEM_MELT_DROP )
                        || obj->material == 45 // eteryczny
                   )
				{
					if ( obj->liczba_mnoga )
						act( "$p rozp³ywaj± siê w dym.", ch, obj, NULL, TO_ALL );
					else
						act( "$p rozp³ywa siê w dym.", ch, obj, NULL, TO_ALL );
					/*artefact*/
					if ( is_artefact( obj ) ) extract_artefact( obj );
					if ( obj->contains ) extract_artefact_container( obj );
					extract_obj( obj );
				}
				else if ( IS_SET( material_table[ obj->material ].flag, MAT_EASYBREAK )
				  		&& number_percent() > material_table[ obj->material ].hardness )
						/*	(obj->item_type==ITEM_POTION &&
							(obj->material==11 || obj->material==15) )*/
				{
					if ( obj->liczba_mnoga )
						act( "$p rozpryskuj± siê na kawa³eczki.", ch, obj, NULL, TO_ALL );
					else
						act( "$p rozpryskuje siê na kawa³eczki.", ch, obj, NULL, TO_ALL );
					/*artefact*/
					if ( is_artefact( obj ) ) extract_artefact( obj );
					if ( obj->contains ) extract_artefact_container( obj );
					extract_obj( obj );
				}
			}
		}

		if ( !found )
		{
			if ( arg[ 3 ] == '\0' )
				act( "Nie nosisz nic przy sobie.", ch, NULL, arg, TO_CHAR );
			else
				act( "Nie masz przy sobie czegos takiego jak $T.", ch, NULL, &arg[ 4 ], TO_CHAR );
		}
	}

	return;
}


void do_empty( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA * container, *into, *temp_obj, *temp_next;
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];

	/* First, check to ensure they can dump.  */
	argument = one_argument( argument, arg1 );

	if ( arg1[ 0 ] == '\0' )
	{
		send_to_char( "Co takiego chcesz opró¿niæ?\n\r", ch );
		return;
	}

	container = get_obj_list( ch, arg1, ch->carrying );
	if ( container == NULL )
	{
		container = get_obj_list( ch, arg1, ch->in_room->contents );
		if ( container == NULL )
		{
			act( "Nie masz czego¶ takiego jak $T.", ch, NULL, arg1, TO_CHAR );
			return;
		}
	}

	if ( container->item_type != ITEM_CONTAINER )
	{
		print_char( ch, "%s niespecjalnie daje siê to opró¿niaæ.\n\r", capitalize(container->short_descr) );
		return;
	}

	if ( IS_SET( container->value[ 1 ], CONT_CLOSED ) )
	{
		if (container->liczba_mnoga)
		{
			print_char( ch, "Hmmm, ale %s s± zamkniête.", container->short_descr);
		}
		else
		{
			switch(container->gender)
			{

				case 2:  // mêski
					print_char( ch, "Hmmm, ale %s jest zamkniêty.", container->short_descr);
					break;
				case 3:  // ¿eñski
					print_char( ch, "Hmmm, ale %s jest zamkniêta.", container->short_descr);
					break;
				case 0:  // nieokre¶lony
				case 1:  // nijaki
				default: // reszta
					print_char( ch, "Hmmm, ale %s jest zamkniête.", container->short_descr);
					break;

			}
		}
		return;
	}

	/* Next, check to see if they want to dump into another container.  */
	argument = one_argument( argument, arg2 );

	if ( !str_cmp( arg2, "in" ) || !str_cmp( arg2, "into" ) || !str_cmp( arg2, "on" ) )
		argument = one_argument( argument, arg2 );

	if ( arg2[ 0 ] != '\0' )
	{
		/* Prefer obj in inventory other than object in room.  */

		into = get_obj_list( ch, arg2, ch->carrying );
		if ( into == NULL )
		{
			if ( ( into = get_obj_here( ch, arg2 ) ) == NULL )
			{
				send_to_char( "Nie widzisz tego tutaj.\n\r", ch );
				return;
			}
		}

		if ( into->item_type != ITEM_CONTAINER )
		{
			print_char( ch, "Nie mo¿esz do %s niczego w³o¿yæ\n\r", into->name2 );
			return;
		}

		if ( IS_SET( into->value[ 1 ], CONT_CLOSED ) )
		{
			if (into->liczba_mnoga)
			{
				print_char( ch, "Hmmm, ale %s s± zamkniête.", into->short_descr);
			}
			else
			{
				switch(into->gender)
				{

					case 2:  // mêski
						print_char( ch, "Hmmm, ale %s jest zamkniêty.", into->short_descr);
						break;
					case 3:  // ¿eñski
						print_char( ch, "Hmmm, ale %s jest zamkniêta.", into->short_descr);
						break;
					case 0:  // nieokre¶lony
					case 1:  // nijaki
					default: // reszta
						print_char( ch, "Hmmm, ale %s jest zamkniête.", into->short_descr);
						break;

				}
			}
			return;
		}

		act( "Przerzucasz wszystkie rzeczy z $f do $F.", ch, container, into, TO_CHAR );
		act( "$n przerzuca wszystkie rzeczy z $f do $F.", ch, container, into, TO_ROOM );

		for ( temp_obj = container->contains; temp_obj != NULL; temp_obj = temp_next )
		{
			temp_next = temp_obj->next_content;

			if ( ( get_obj_weight( temp_obj ) + get_true_weight( into )
				   > ( into->value[ 0 ] * 22 ) )
				 || ( get_obj_weight( temp_obj ) > ( into->value[ 3 ] * 22 ) ) )
				act( "$P nie zmie¶ci siê ju¿ do $f.", ch, into, temp_obj, TO_CHAR );

			else
			{
				obj_from_obj( temp_obj );
				obj_to_obj( temp_obj, into );
			}
		}
	}

	/* Dumping to the floor.  */
	else
	{
		if ( container->contains == NULL )
		{
			act( "Odwracasz $j do góry nogami, potrz±sasz chwilê, ale nic nie wylatuje.", ch, container, NULL, TO_CHAR );
			act( "$n odwraca do góry nogami $j, potrz±sa chwilê, ale nic nie wylatuje.", ch, container, NULL, TO_ROOM );
			return;
		}

		act( "Wyrzucasz przed sob± ca³± zawarto¶æ $f.", ch, container, NULL, TO_CHAR );
		act( "$n wyrzuca przed sob± ca³± zawarto¶æ $f.", ch, container, NULL, TO_ROOM );

		for ( temp_obj = container->contains; temp_obj != NULL; temp_obj = temp_next )
		{
			temp_next = temp_obj->next_content;

			/*artefact*/
			if ( !IS_IMMORTAL( ch ) && is_artefact( temp_obj ) && !IS_NPC( ch ) )
			{
				artefact_from_char( temp_obj, ch );
			}
			if ( temp_obj->contains != NULL && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) )
			{
				container_artefact_check( temp_obj, ch, 1 ); //1 - zdejmujemy jak co¶ jest
			}

			if( temp_obj->liczba_mnoga )
			{
				act( "... $p upadaj± ci pod nogi.", ch, temp_obj, NULL, TO_CHAR );
				act( "... $p upadaj± $c pod nogi.", ch, temp_obj, NULL, TO_ROOM );
			}
			else
			{
				act( "... $p upada ci pod nogi.", ch, temp_obj, NULL, TO_CHAR );
				act( "... $p upada $c pod nogi.", ch, temp_obj, NULL, TO_ROOM );
			}

			obj_from_obj( temp_obj );
			obj_to_room( temp_obj, ch->in_room );
			temp_obj->on_ground = 15;

			obj_trap_handler( ch, temp_obj, TRAP_DROP );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( temp_obj, TRIG_DROP ) )
				op_common_trigger( ch, temp_obj, &TRIG_DROP );

			/* wyrzucanie na wodê? */
            if ( obj_fall_into_water( ch, temp_obj ) )
                    return;

            if (
                    IS_OBJ_STAT( temp_obj, ITEM_MELT_DROP )
                    || temp_obj->material == 45 // eteryczny
               )
			{
				if ( temp_obj->liczba_mnoga )
					act( "$p rozp³ywaj± siê w dym.", ch, temp_obj, NULL, TO_ALL );
				else
					act( "$p rozp³ywa siê w dym.", ch, temp_obj, NULL, TO_ALL );
				extract_obj( temp_obj );
			}

			else if ( IS_SET( material_table[ temp_obj->material ].flag, MAT_EASYBREAK )
				      && number_percent() > material_table[ temp_obj->material ].hardness )
			{
				if ( temp_obj->liczba_mnoga )
					act( "$p rozpryskuj± siê na kawa³eczki.", ch, temp_obj, NULL, TO_ALL );
				else
					act( "$p rozpryskuje siê na kawa³eczki.", ch, temp_obj, NULL, TO_ALL );
				/*artefact*/
				if ( is_artefact( temp_obj ) ) extract_artefact( temp_obj );
				if ( temp_obj->contains ) extract_artefact_container( temp_obj );
				extract_obj( temp_obj );
			}
		}
	}
}

void do_give( CHAR_DATA *ch, char *argument )
{
	char arg1 [ MAX_INPUT_LENGTH ];
	char arg2 [ MAX_INPUT_LENGTH ];
	char buf[ MAX_STRING_LENGTH ];
	CHAR_DATA *victim;
	OBJ_DATA *obj;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
	{
		send_to_char( "Dac co i komu?\n\r", ch );
		return;
	}

	if ( is_number( arg1 ) )
	{
		/* 'give NNNN coins victim' */
		int amount = atoi( arg1 );
        int nomination = money_nomination_find( arg2 );

        DEBUG_INFO( "do_give: i know nomination\n\r" );
        if ( amount < 1 || nomination == NOMINATION_NONE )
        {
            send_to_char( "Nie mo¿esz tego zrobiæ.\n\r", ch );
            return;
        }

        argument = one_argument( argument, arg2 );

        if ( arg2[ 0 ] == '\0' )
        {
            send_to_char( "Daæ co¶, ale komu?\n\r", ch );
            return;
        }

        if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
        {
            send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
            return;
        }

        if ( victim == ch )
        {
            send_to_char( "A to ci dopiero ciekawostka.\n\r", ch );
            return;
        }

        /* mistrzowie przemian z razorblade hands nie wezma od nikogo przedmiotu */
        if ( IS_AFFECTED( victim, AFF_RAZORBLADED ) )
        {
            act( "D³onie $Z s± zamienione w ostrza, nie ma jak odebraæ od ciebie monet.", ch, NULL, victim, TO_CHAR );
            act( "$n chce ci daæ monety, ale twe d³onie zamienione w ostrza uniemo¿liwiaj± ci ich odebranie.", ch, NULL, victim, TO_VICT );
            act( "$n chce daæ $X monety, niestety nie ma jak.", ch, NULL, victim, TO_NOTVICT );
            return;
        }

        if ( IS_AFFECTED( victim, AFF_BEAST_CLAWS ) )
        {
            act( "D³onie $Z s± zamienione w szpony, nie ma jak odebraæ od ciebie monet.", ch, NULL, victim, TO_CHAR );
            act( "$n chce ci daæ monety, ale twe d³onie zamienione w szpony uniemo¿liwiaj± ci ich odebranie.", ch, NULL, victim, TO_VICT );
            act( "$n chce daæ $X monety, niestety nie ma jak.", ch, NULL, victim, TO_NOTVICT );
            return;
        }

        if ( IS_AFFECTED( victim, AFF_FORCE_FIELD ) )
        {
            act( "$N znajduje siê w kopule mocy, nie jeste¶ w stanie tego uczyniæ.", ch, NULL, victim, TO_CHAR );
            return;
        }

        if ( victim->desc == NULL && !IS_NPC( victim ) )
        {
            act( "$N ma zerwane po³±czenie... sprobuj po¼niej.", ch, NULL, victim, TO_CHAR );
            return;
        }

        if ( !IS_SET( race_table[ GET_RACE( victim ) ].wear_flag, ITEM_TAKE ) )
        {
            return;
        }

        if ( ! money_from_char_to_char( ch, victim, amount, nomination ) )
        {
            return;
        }

		/*
		 * Bribe trigger
		 */
		if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_BRIBE ) )
		{
			sprintf(buf, "[%d] DEBUG: give bribe dla moba [%d]", ch->in_room ? ch->in_room->vnum : 0, victim->pIndexData->vnum);
			log_string(buf);
			mp_bribe_trigger( victim, ch, money_to_copper_by_nomination( amount, nomination ) );
		}

		return;
	}

	if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
	{
		send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
		return;
	}

	if ( obj->wear_loc != WEAR_NONE )
	{
		send_to_char( "Musisz najpierw to zdj±æ.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
		send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "A to ci dopiero ciekawostka.\n\r", ch );
		return;
	}

		/* mistrzowie przemian z razorblade hands nie wezma od nikogo przedmiotu */
		if ( IS_AFFECTED( victim, AFF_RAZORBLADED ) )
		{
			act( "D³onie $Z s± zamienione w ostrza, nie ma jak odebraæ od ciebie $f.", ch, obj, victim, TO_CHAR );
			act( "$n chce ci daæ $p, ale twe d³onie zamienione w ostrza uniemo¿liwiaj± ci odebranie tego.", ch, obj, victim, TO_VICT );
			act( "$n chce daæ $X $h, niestety nie ma jak.", ch, obj, victim, TO_NOTVICT );
			return;
		}

		if ( IS_AFFECTED( victim, AFF_BEAST_CLAWS ) )
		{
			act( "D³onie $Z s± zamienione w szpony, nie ma jak odebraæ od ciebie $f.", ch, obj, victim, TO_CHAR );
			act( "$n chce ci daæ $p, ale twe d³onie zamienione w szpony uniemo¿liwiaj± ci odebranie tego.", ch, obj, victim, TO_VICT );
			act( "$n chce daæ $X $h, niestety nie ma jak.", ch, obj, victim, TO_NOTVICT );
			return;
		}

		if ( IS_AFFECTED( victim, AFF_FORCE_FIELD ) )
		{
			act( "$N znajduje siê w kopule mocy, nie jeste¶ w stanie tego uczyniæ.", ch, obj, victim, TO_CHAR );
			return;
		}

	if ( !can_drop_obj( ch, obj ) )
	{
		send_to_char( "Nie mo¿esz tego wypu¶ciæ.\n\r", ch );
		return;
	}

	if ( !IS_SET( race_table[ GET_RACE( victim ) ].wear_flag, ITEM_TAKE ) )
		return;

	if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
	{
		act( "$N nie moze nosiæ niczego wiecej.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( get_carry_weight( victim ) + get_obj_weight( obj ) > can_carry_w( victim ) )
	{
		act( "$N nie ud¼wignie tego.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( !can_see_obj( victim, obj ) )
	{
		act( "$N nie widzi tego.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->desc == NULL && !IS_NPC( victim ) )
	{
		act( "$N ma zerwane po³±czenie... spróbuj pó¼niej.", ch, NULL, victim, TO_CHAR );
		return;
	}

	/*artefact*/
	if ( is_artefact( obj ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
		artefact_from_char( obj, ch );
	if ( obj->contains != NULL && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) )
		container_artefact_check( obj, ch, 1 ); //1 - zdejmujemy jak co¶ jest
	obj_from_char( obj );

	/*artefact*/
	if ( is_artefact( obj ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
		artefact_to_char( obj, victim );
	if ( obj->contains != NULL && !IS_IMMORTAL( victim ) && !IS_NPC( victim ) )
		container_artefact_check( obj, victim, 0 ); //0 - dodajemy
	obj_to_char( obj, victim );

	MOBtrigger = FALSE;
	OBJtrigger = FALSE;
	act( "$n daje $h $X.", ch, obj, victim, TO_NOTVICT );
	act( "$n daje ci $h.", ch, obj, victim, TO_VICT );
	act( "Dajesz $h $X.", ch, obj, victim, TO_CHAR );
	MOBtrigger = TRUE;
	OBJtrigger = TRUE;

	/*
	 * Give trigger
	 */
	if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_GIVE ) && !victim->fighting )
		mp_give_trigger( victim, ch, obj );


	return;
}

/* for poisoning weapons and food/drink */
void do_envenom( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA * obj, *weapon;
    char arg1[ MAX_INPUT_LENGTH ];
    AFFECT_DATA af;
    int skill;

    if ( ( skill = get_skill( ch, gsn_envenom ) ) < 1 )
    {
        switch ( number_range( 1, 6 ) )
        {
            case 1:
                send_to_char( "Zatruæ? Co w³a¶ciwie chcesz tak naprawdê zrobiæ?\n\r", ch );
                break;
            case 2:
                send_to_char( "Co ty wiesz o zatruwaniu?\n\r", ch );
                break;
            case 3:
                send_to_char( "Co ty wiesz o zatruwaniu, ty stara dupa jeste¶.\n\r", ch );
                break;
            case 4:
                send_to_char( "Chcesz mieæ k³opty z ¿o³±dkiem?\n\r", ch );
                break;
            case 5:
                send_to_char( "W³a¶ciwie to nic nie wiesz o zatruwaniu.\n\r", ch );
                break;
            case 6:
            default:
                send_to_char( "Oszala³<&e¶/a¶/e¶>? Zatrujesz sam<&/a/o> siebie!\n\r", ch );
                break;
        }
        WAIT_STATE( ch, number_range( 1, 3 ) );
        return;
    }

    argument = one_argument( argument, arg1 );

    /* find out what */
    if ( arg1[ 0 ] == '\0' )
    {
        send_to_char( "Co i czym chcesz zatruæ?\n\r", ch );
        return;
    }

    weapon = get_obj_list( ch, arg1, ch->carrying );

    if ( weapon == NULL )
    {
        send_to_char( "Któr± broñ chcesz zatruæ?\n\r", ch );
        return;
    }

    if ( weapon->item_type != ITEM_WEAPON )
    {
        send_to_char( "Przecie¿ to nie jest broñ.\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        act( "Czym chcesz zatruæ $h?", ch, weapon, NULL, TO_CHAR );
        return;
    }

    obj = get_obj_list( ch, argument, ch->carrying );

    if ( ! obj )
    {
        act( "Czym chcesz zatruæ $h?", ch, weapon, NULL, TO_CHAR );
        return;
    }

    if ( obj->item_type != ITEM_ENVENOMER )
    {
        send_to_char( "Tym raczej nic nie zatrujesz.\n\r", ch );
        return;
    }

    /**
     * case nieskoñczonej trutki, przy braku ustawionej liczby u¿yæ
     */
    if ( obj->value[ 1 ] < 1 )
    {
        send_to_char( "Tym, to ju¿ raczej niczego nie zatrujesz.\n\r", ch );
        return;
    }

    if ( weapon->item_type == ITEM_WEAPON )
    {
        if ( IS_WEAPON_STAT( weapon, WEAPON_POISON ) )
        {
            send_to_char( "To jest ju¿ zatrute.\n\r", ch );
            return;
        }

        if ( IS_WEAPON_STAT( weapon, WEAPON_FLAMING )
                || IS_WEAPON_STAT( weapon, WEAPON_FROST )
                || IS_WEAPON_STAT( weapon, WEAPON_VAMPIRIC )
                || IS_WEAPON_STAT( weapon, WEAPON_TOXIC )
                || IS_WEAPON_STAT( weapon, WEAPON_SACRED )
                || IS_WEAPON_STAT( weapon, WEAPON_RESONANT )
                || IS_WEAPON_STAT( weapon, WEAPON_SHOCKING ) )
        {
            act( "Chyba nie dasz rady zatruæ $f.", ch, weapon, NULL, TO_CHAR );
            return;
        }

        // explicite podane typy broni s± gwarancj±, ¿e dodaj±c nowe, nie wpadn± z autoamtu tutaj
        if (
                weapon->value[0] != WEAPON_SWORD
                && weapon->value[0] != WEAPON_DAGGER
                && weapon->value[0] != WEAPON_SPEAR
                && weapon->value[0] != WEAPON_AXE
                && weapon->value[0] != WEAPON_POLEARM
                && weapon->value[0] != WEAPON_SHORTSWORD
                && weapon->value[0] != WEAPON_CLAWS
                //weapon->value[ 3 ] < 0 || attack_table[ weapon->value[ 3 ] ].damage == DAM_BASH
           )
        {
            //send_to_char( "Mo¿esz zatruæ tylko bronie z jak±¶ krawêdzi± tn±c±, b±d¼ posiadaj±ce czubek.\n\r", ch );
            act( "Nie dasz rady zatruæ trucizn± $f.", ch, weapon, NULL, TO_CHAR );
            return;
        }

        if ( ch->class == CLASS_BARD || ch->class == CLASS_BLACK_KNIGHT )
        {
            skill -= skill/4;
        }

        //czarni rycerze maja ograniczony wybor trucizn
        //nie maja sleepujacej, holdujacej, zabijajacej i bardzo silnej
        if ( ch->class == CLASS_BLACK_KNIGHT && obj->value[ 0 ] > 3 && obj->value[ 0 ] != 6 && obj->value[ 0 ] != 9 )
        {
            send_to_char( "Korzystanie z tak silnej trucizny le¿y poza twoimi mo¿liwo¶ciami.\n\r", ch );
            return;
        }

        if ( number_percent() < skill )
        {
            af.where = TO_WEAPON;
            af.type = gsn_poison;
            af.level = obj->value[ 0 ];
            af.duration = 5 + skill / 10;
            af.rt_duration = 0;
            af.location = WEAPON_POISON;
            af.modifier = 0;
            af.bitvector = &AFF_NONE;
            affect_to_obj( weapon, &af );

            act( "$n pokrywa $h ¶mierteln± trucizn±.", ch, weapon, NULL, TO_ROOM );
            act( "Pokrywasz $h trucizn±.", ch, weapon, NULL, TO_CHAR );
            check_improve( ch, NULL, gsn_envenom, TRUE, 12 );
        }
        else
        {
            act( "Nie uda³o ci siê zatruæ $f.", ch, weapon, NULL, TO_CHAR );
            check_improve( ch, NULL, gsn_envenom, FALSE, 15 );
        }
        WAIT_STATE( ch, skill_table[ gsn_envenom ].beats );
        if ( obj->value[ 1 ] > 0 && --obj->value[ 1 ] == 0 )
        {
            /*artefact*/
            if ( is_artefact( obj ) ) extract_artefact( obj );
            {
                extract_obj( obj );
            }
        }
        return;
    }

    act( "Nie mo¿esz zatruæ $f.", ch, weapon, NULL, TO_CHAR );
    return;
}

void do_fill( CHAR_DATA *ch, char *argument )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char buf[ MAX_STRING_LENGTH ];
	OBJ_DATA *obj;
	OBJ_DATA *fountain;
	bool found;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );


	if ( ch->fighting )
	{
		send_to_char( "Lepiej skup siê na walce.\n\r", ch );
		return;
	}

	if ( arg1[ 0 ] == '\0' )
	{
		print_char( ch, "A co by¶ chcia³<&/a/o> nape³niæ?\n\r" );
		return;
	}

	obj = get_obj_carry( ch, arg1, ch );

	if ( obj == NULL ) obj = get_eq_char( ch, WEAR_HOLD );

	if ( obj == NULL )
	{
		send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_DRINK_CON )
	{
		send_to_char( "Nie da rady tego nape³niæ.\n\r", ch );
		return;
	}

	if ( obj->value[ 1 ] >= obj->value[ 0 ] )
	{
		send_to_char( "Ten pojemnik jest pe³ny.\n\r", ch );
		return;
	}

	if ( arg2[0] == '\0' )
	{
		if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_CANDRINK ) )
		{
			sprintf( buf, "Pochylasz siê lekko i nape³niasz $h wod± zaczerpuj±c j± tu¿ sprzed swoich stóp." );
			act( buf, ch, obj, NULL, TO_CHAR );

			sprintf( buf, "$n pochyla siê lekko i nape³nia $h wod± zaczerpuj±c j± tu¿ sprzed swoich stóp." );
			act( buf, ch, obj, NULL, TO_ROOM );

			if ( obj->value[ 1 ] != 0 && obj->value[ 2 ] != 0 /*woda*/ )
			{
				send_to_char( "Jest w tym ju¿ inna ciecz.\n\r", ch );
				return;
			}

			obj->value[ 2 ] = 0;
			obj->value[ 1 ] = obj->value[ 0 ];
			return;
		}

		found = FALSE;
		for ( fountain = ch->in_room->contents; fountain != NULL;
			  fountain = fountain->next_content )
		{
			if ( fountain->item_type == ITEM_FOUNTAIN && can_see_obj( ch, fountain ) )
			{
				found = TRUE;
				break;
			}
		}

		if ( !found )
		{
			send_to_char( "Nie ma tu ¿adnej fontanny.\n\r", ch );
			return;
		}


		if ( obj->value[ 1 ] != 0 && obj->value[ 2 ] != fountain->value[ 2 ] )
		{
			send_to_char( "Jest w tym ju¿ inna ciecz.\n\r", ch );
			return;
		}
	}
	else
	{
		found = FALSE;
		for ( fountain = ch->in_room->contents; fountain != NULL;
			  fountain = fountain->next_content )
		{
			if ( fountain->item_type == ITEM_FOUNTAIN && can_see_obj( ch, fountain ) && is_name( arg2, fountain->name ) )
			{
				found = TRUE;
				break;
			}
		}

		if ( !found )
		{
			send_to_char( "Nie ma tu takiej fontanny.\n\r", ch );
			return;
		}


		if ( obj->value[ 1 ] != 0 && obj->value[ 2 ] != fountain->value[ 2 ] )
		{
			send_to_char( "Jest w tym ju¿ inna ciecz.\n\r", ch );
			return;
		}
	}

	if ( fountain->name2[ 0 ]
		 && ( NOPOL( fountain->name2[ 0 ] ) == 'S' || NOPOL( fountain->name2[ 0 ] ) == 'Z' ) )
		sprintf( buf, "Nape³niasz $h %s wprost ze $F.", liq_table[ fountain->value[ 2 ] ].liq_name5 );
	else
		sprintf( buf, "Nape³niasz $h %s wprost z $F.", liq_table[ fountain->value[ 2 ] ].liq_name5 );
	act( buf, ch, obj, fountain, TO_CHAR );

	if ( fountain->name2[ 0 ]
		 && ( NOPOL( fountain->name2[ 0 ] ) == 'S' || NOPOL( fountain->name2[ 0 ] ) == 'Z' ) )
		sprintf( buf, "$n nape³nia $h %s ze $F.", liq_table[ fountain->value[ 2 ] ].liq_name5 );
	else
		sprintf( buf, "$n nape³nia $h %s z $F.", liq_table[ fountain->value[ 2 ] ].liq_name5 );
	act( buf, ch, obj, fountain, TO_ROOM );
	obj->value[ 2 ] = fountain->value[ 2 ];
	obj->value[ 1 ] = obj->value[ 0 ];
	return;
}

void do_pour ( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_STRING_LENGTH ], buf[ MAX_STRING_LENGTH ];
	OBJ_DATA *out, *in;
	CHAR_DATA *vch = NULL;
	int amount;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		send_to_char( "Co chcesz przelaæ lub wylaæ?\n\r", ch );
		return;
	}

	out = get_obj_carry( ch, arg, ch );

// je¿eli obiekt nieistnieje w inventory, to mo¿e kto¶ co¶ takiego trzyma
// w ³apce? z czego próbuje siê napiæ, warto sprawdziæ.

	if ( out == NULL ) out = get_eq_char( ch, WEAR_HOLD );

	if ( out == NULL )
	{
		send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
		return;
	}

	if ( out->item_type != ITEM_DRINK_CON )
	{
		send_to_char( "To nie jest pojemnik na ciecze.\n\r", ch );
		return;
	}

	if ( !str_cmp( argument, "out" ) )
	{
		if ( out->value[ 1 ] == 0 )
		{
			send_to_char( "To jest ju¿ puste.\n\r", ch );
			return;
		}

		if ( out->value[ 1 ] == -1 )
		{
			print_char( ch, "Wylewasz, wylewasz, ale %s siê nie koñczy.\n\r",liq_table[ out->value[ 2 ] ].liq_name );
			return;
		}

		out->value[ 1 ] = 0;
		out->value[ 3 ] = 0;

		sprintf( buf, "Wylewasz %s na ziemiê.", liq_table[ out->value[ 2 ] ].liq_name4 );
		act( buf, ch, out, NULL, TO_CHAR );

		sprintf( buf, "$n wylewa %s na ziemiê.", liq_table[ out->value[ 2 ] ].liq_name4 );
		act( buf, ch, out, NULL, TO_ROOM );

		//puste pojemniki zamieniaja sie na obiekt o vnumie podanym w value[4]
		//coby na przyklad butelka piwa po wyproznieniu i nalaniu do niej wody
		//dalej nie byla butelka piwa
		if ( out->value[ 4 ] > 0 && out->value[ 1 ] <= 0 )
		{
			OBJ_DATA *empty;
			int wear_loc;

			if ( ( empty = create_object( get_obj_index( out->value[ 4 ] ), FALSE ) ) == NULL )
				return;

			//taka sama pojemno¶æ jak orygina³, puste, nie zatrute
			empty->value[ 0 ] = out->value[ 0 ];
			empty->value[ 1 ] = 0;
			empty->value[ 2 ] = 0;
			empty->value[ 3 ] = 0;
			empty->value[ 4 ] = 0;

			SET_BIT( empty->wear_flags, out->wear_flags );
			empty->material = out->material;
			empty->weight = out->weight;
			empty->condition = out->condition;
            empty->rent_cost = RENT_COST( empty->cost );

			wear_loc = out->wear_loc;

			if ( out->carried_by )
			{
				obj_from_char( out );
				obj_to_char( empty, ch);
				if ( wear_loc != WEAR_NONE )
					equip_char( ch, empty, wear_loc, FALSE );
			}
			else
            {
				obj_to_room( empty, ch->in_room );
            }

			extract_obj( out );
		}

		return;
	}

	if ( ( in = get_obj_here_alt( ch, argument ) ) == NULL )
	{
		vch = get_char_room( ch, argument );

		if ( vch == NULL )
		{
			send_to_char( "Przelaæ do czego?\n\r", ch );
			return;
		}

		in = get_eq_char( vch, WEAR_HOLD );

		if ( in == NULL )
		{
			send_to_char( "Ta osoba nie trzyma nic w rêce.\n\r", ch );
			return;
		}
	}

	if ( in->item_type != ITEM_DRINK_CON )
	{
		send_to_char( "Mo¿esz przelewaæ tylko do innych pojemników na ciecze.\n\r", ch );
		return;
	}

	if ( in == out )
	{
		send_to_char( "To raczej nie ma sensu.\n\r", ch );
		return;
	}

	if ( in->value[ 1 ] != 0 && in->value[ 2 ] != out->value[ 2 ] )
	{
		send_to_char( "Te pojemniki nie zawieraj± tej samej cieczy.\n\r", ch );
		return;
	}

	if ( out->value[ 1 ] == 0 )
	{
		act( "W $k przecie¿ nic nie ma.", ch, out, NULL, TO_CHAR );
		return;
	}

	if ( in->value[ 1 ] >= in->value[ 0 ] || in->value[ 1 ] == -1 )
	{
		act( "$p wiêcej nie pomie¶ci.", ch, in, NULL, TO_CHAR );
		return;
	}

	amount = UMIN( out->value[ 1 ], in->value[ 0 ] - in->value[ 1 ] );

	if ( in->value[ 1 ] != -1 )
		in->value[ 1 ] += amount;

	if ( out->value[ 1 ] != -1 )
		out->value[ 1 ] -= amount;

	in->value[ 2 ] = out->value[ 2 ];


	//puste pojemniki zamieniaja sie na obiekt o vnumie podanym w value[4]
	//coby na przyklad butelka piwa po wyproznieniu i nalaniu do niej wody
	//dalej nie byla butelka piwa
	if ( out->value[ 4 ] > 0 && out->value[ 1 ] <= 0 )
	{
		OBJ_DATA *empty;
		int wear_loc;

		if ( ( empty = create_object( get_obj_index( out->value[ 4 ] ), FALSE ) ) == NULL )
			return;

		//taka sama pojemno¶æ jak orygina³, puste, nie zatrute
		empty->value[ 0 ] = out->value[ 0 ];
		empty->value[ 1 ] = 0;
		empty->value[ 2 ] = out->value[ 2 ];
		empty->value[ 3 ] = 0;
		empty->value[ 4 ] = 0;

		SET_BIT( empty->wear_flags, out->wear_flags );
		empty->material = out->material;
		empty->weight = out->weight;
        empty->condition = out->condition;
        empty->rent_cost = RENT_COST( empty->cost );

		wear_loc = out->wear_loc;

		if ( out->carried_by )
		{
			obj_from_char( out );
			obj_to_char( empty, ch);
			if ( wear_loc != WEAR_NONE )
				equip_char( ch, empty, wear_loc, FALSE );
		}
		else
        {
			obj_to_room( empty, ch->in_room );
        }

		extract_obj( out );
		out = empty;
	}

	if ( vch == NULL )
	{
		if ( out->name2[ 0 ]
			 && ( NOPOL( out->name2[ 0 ] ) == 'S' || NOPOL( out->name2[ 0 ] ) == 'Z' ) )
			sprintf( buf, "Przelewasz %s ze $F do $f.", liq_table[ out->value[ 2 ] ].liq_name4 );
		else
			sprintf( buf, "Przelewasz %s z $F do $f.", liq_table[ out->value[ 2 ] ].liq_name4 );
		act( buf, ch, in, out, TO_CHAR );

		if ( out->name2[ 0 ]
			 && ( NOPOL( out->name2[ 0 ] ) == 'S' || NOPOL( out->name2[ 0 ] ) == 'Z' ) )
			sprintf( buf, "$n przelewa %s ze $F do $f.", liq_table[ out->value[ 2 ] ].liq_name4 );
		else
			sprintf( buf, "$n przelewa %s z $F do $f.", liq_table[ out->value[ 2 ] ].liq_name4 );
		act( buf, ch, in, out, TO_ROOM );
	}
	else
	{
		sprintf( buf, "Odlewasz $X trochê %s do $f.", liq_table[ out->value[ 2 ] ].liq_name2 );
		act( buf, ch, in, vch, TO_CHAR );

		sprintf( buf, "$n odlewa ci trochê %s do $f.", liq_table[ out->value[ 2 ] ].liq_name2 );
		act( buf, ch, in, vch, TO_VICT );

		sprintf( buf, "$n odlewa $X trochê %s do $f.", liq_table[ out->value[ 2 ] ].liq_name2 );
		act( buf, ch, in, vch, TO_NOTVICT );
	}
}

void do_drink( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	int amount;
	int liquid;

	if ( IS_NPC( ch ) )
		return;

	one_argument( argument, arg );

	if ( ch->fighting )
	{
		send_to_char( "Lepiej skup sie na walce!\n\r", ch );
		return;
	}

	if ( ch->precommand_fun && !ch->precommand_pending )
	{
		send_to_char( "Robisz teraz co¶ innego.\n\r", ch );
		return;
	}

	if( is_undead(ch) && !IS_IMMORTAL(ch) )
	{
		send_to_char( "Po co chcesz to robiæ?\n\r", ch );
		return;
	}

	if ( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && ch->condition[ COND_THIRST ] > DRINK_FULL )
	{
		send_to_char( "Przecie¿ nie czujesz pragnienia.\n\r", ch );
		return;
	}

	if ( arg[ 0 ] == '\0' )
	{
		if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_CANDRINK ) )
		{
			act( "$n nabiera wody w rêce, zbli¿a do ust i pije.", ch, NULL, NULL, TO_ROOM );
			print_char( ch, "Nabierasz wody w rêce, zbli¿asz do ust i pijesz.\n\r" );
			gain_condition( ch, COND_THIRST, DRINK_AMOUNT );

			if ( !IS_NPC( ch ) && ch->condition[ COND_THIRST ] > DRINK_FULL_FOUNTAIN )
				send_to_char( "Twoje pragnienie jest zaspokojone.\n\r", ch );

			return;
		}

		print_char( ch, "Czego chcia³<&/a/o>by¶ siê napiæ?\n\r" );
		return;
	}
	else
	{
		obj = get_obj_carry( ch, arg, ch );

// je¿eli obiekt nieistnieje w inventory, to mo¿e kto¶ co¶ takiego trzyma
// w ³apce? z czego próbuje siê napiæ, warto sprawdziæ.

		if ( obj == NULL ) obj = get_obj_wear( ch, arg, FALSE );

		if ( obj == NULL )
		{
			if ( ( obj = get_obj_list( ch, arg, ch->in_room->contents ) ) == NULL )
			{
				send_to_char( "Nie mo¿esz tego znale¼æ.\n\r", ch );
				return;
			}
		}
	}

	if ( get_obj_memdat( obj, NULL, NULL, MEMDAT_ANY ) && HAS_OTRIGGER( obj, TRIG_PRECOMMAND ) )
	{
		send_to_char( "Kto¶ inny teraz z tego pije. Poczekaj chwilê.\n\r", ch );
		return;
	}

	switch ( obj->item_type )
	{
		default:
			send_to_char( "Raczej siê z tego nie napijesz.\n\r", ch );
			return;

		case ITEM_FOUNTAIN:
			if ( ( liquid = obj->value[ 2 ] ) < 0 )
			{
				bug( "Do_drink: bad liquid number %d.", liquid );
				liquid = obj->value[ 2 ] = 0;
			}
			if ( obj->value[ 1 ] == 0 )
			{
				print_char( ch, "Wygl±da na to, ¿e z %s siê ju¿ nie napijesz.\n\r", obj->name2 );
				return;
			}
			amount = liq_table[ liquid ].liq_affect[ 4 ] * 3;
			break;

		case ITEM_DRINK_CON:

			/* mistrzowie przemian z razorblade hands se niepopija z buklakow */
			if ( IS_AFFECTED( ch, AFF_RAZORBLADED ) )
			{
				send_to_char( "Przemienione w ostrza d³onie uniemo¿liwiaj± ci wykonanie tej czynno¶ci.\n\r", ch );
				return;
			}

			if ( IS_AFFECTED( ch, AFF_BEAST_CLAWS ) )
			{
				send_to_char( "Przemienione w szpony d³onie uniemo¿liwiaj± ci wykonanie tej czynno¶ci.\n\r", ch );
				return;
			}

			if ( obj->value[ 1 ] == 0 )
			{
				send_to_char( "To jest puste.\n\r", ch );
				return;
			}

			if ( ( liquid = obj->value[ 2 ] ) < 0 )
			{
				bug( "Do_drink: bad liquid number %d.", liquid );
				liquid = obj->value[ 2 ] = 0;
			}

			amount = liq_table[ liquid ].liq_affect[ 4 ];

			if ( obj->value[1] > 0 )
				amount = UMIN( amount, obj->value[ 1 ] );

			break;
	}

	if ( !ch->precommand_pending && HAS_OTRIGGER( obj, TRIG_PRECOMMAND ) )
	{
		if ( op_precommand_trigger( ch, obj, NULL, &do_drink, "drink", argument ) )
			return;
	}

	if ( !op_consume_trigger( obj, ch ) && ( !ch->precommand_pending || !HAS_OTRIGGER( obj, TRIG_PRECOMMAND ) ) )
	{
		if ( obj->name2[ 0 ] && ( NOPOL( obj->name2[ 0 ] ) == NOPOL( 's' ) || NOPOL( obj->name2[ 0 ] ) == NOPOL( 'z' ) ) )
		{
			act( "$n pije $T ze $f.", ch, obj, liq_table[ liquid ].liq_name4, TO_ROOM );
			act( "Poci±gasz ³yk $T ze $f.", ch, obj, liq_table[ liquid ].liq_name2, TO_CHAR );
		}
		else
		{
			act( "$n pije $T z $f.", ch, obj, liq_table[ liquid ].liq_name4, TO_ROOM );
			act( "Poci±gasz ³yk $T z $f.", ch, obj, liq_table[ liquid ].liq_name2, TO_CHAR );
		}
	}

	gain_condition( ch, COND_DRUNK, amount * liq_table[ liquid ].liq_affect[ COND_DRUNK ] );

	gain_condition( ch, COND_THIRST,
				    amount * liq_table[ liquid ].liq_affect[ COND_THIRST ] );

	gain_condition( ch, COND_HUNGER,
				    amount * liq_table[ liquid ].liq_affect[ COND_HUNGER ] );

	// Tener: LIQ_HEALING_WATER -- uzdrawiaj±ca woda
    if ( liquid == LIQ_HEALING_WATER && !IS_SET( ch->form, FORM_CONSTRUCT ) )
    {
        int value = 0;
        if ( is_undead( ch ) )
        {
            value = -number_range( 1, 5 );
            act("Ten napój pali twoje gard³o i wnêtrzno¶ci!", ch, NULL, NULL, TO_CHAR );
        }
        else
        {
            if ( get_max_hp( ch ) > ch->hit )
            {
                act("Czujesz, jak uzdrawiaj±ca woda leczy twoje rany.", ch, NULL, NULL, TO_CHAR );
            }
            else
            {
                act("Czujesz, jak po twoim ciele rozchodzi siê przyjemne ciep³o.", ch, NULL, NULL, TO_CHAR );
            }
            value = number_range( 1, 5 );
        }
        ch->hit = UMIN( ch->hit + value, get_max_hp( ch ) );
    }

	if ( !IS_NPC( ch ) && ch->condition[ COND_DRUNK ] &&
		 liq_table[ liquid ].liq_affect[ COND_DRUNK ] != 0 )
		send_to_char( "Krêci ci siê w g³owie.\n\r", ch );

	if ( !IS_NPC( ch ) && ch->condition[ COND_THIRST ] > DRINK_PART )
		send_to_char( "Twoje pragnienie jest zaspokojone.\n\r", ch );

	if ( obj->value[ 3 ] != 0 )
	{
		/* The drink was poisoned ! */
		AFFECT_DATA af;
		int insensibility = 0;

		// dodajemy odporno¶æ na poison dla undeadów i golemów
		if ( is_undead( ch ) || IS_SET( ch->form, FORM_CONSTRUCT ) )
			insensibility = 1;

		if (insensibility == 0)
		{
			act( "$n krztusi siê i kaszle.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Krztusisz siê i kaszlesz.\n\r", ch );
			af.where = TO_AFFECTS;
			af.type = gsn_poison;
			af.level = 0;
			af.duration = 2; af.rt_duration = 0;
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = &AFF_POISON;
			affect_join( ch, &af );
		}
	}

	if ( obj->value[ 0 ] > 0 )
	{
		obj->value[ 1 ] = UMAX( obj->value[ 1 ] - amount, 0 );
	}

	if ( !IS_NPC( ch ) && ch->condition[ COND_DRUNK ] > DRUNK_FULL )
	{
		if (IS_SET( obj->wear_flags, ITEM_TAKE ))
		{
			send_to_char( "Ledwo trafiasz do ust, wiêkszo¶æ leje ci siê po brodzie.\n\r", ch );
			act( "$n ledwo trafia do ust $j, wiêkszo¶æ wylewaj±c sobie na brodê.", ch, obj, NULL, TO_ROOM );
		}
		else
		{
			act( "Próbujesz piæ z $f, ale wiêkszo¶æ rozchlapujesz dooko³a.", ch, obj, NULL, TO_CHAR );
			act( "$n próbuje piæ z $f, ale wiêkszo¶æ rochlapuje dooko³a.", ch, obj, NULL, TO_ROOM );
		}
		if ( obj->value[ 0 ] > 0 )
		{
			obj->value[ 1 ] = UMAX( 0, obj->value[ 1 ] - 2 * amount );
		}
	}

	//puste pojemniki zamieniaja sie na obiekt o vnumie podanym w value[4]
	//coby na przyklad butelka piwa po wyproznieniu i nalaniu do niej wody
	//dalej nie byla butelka piwa
	if ( obj->value[ 4 ] > 0 && obj->value[ 1 ] <= 0 )
	{
		OBJ_DATA *empty;
		int wear_loc;

		if ( ( empty = create_object( get_obj_index( obj->value[ 4 ] ), FALSE ) ) == NULL )
			return;

		//taka sama pojemno¶æ jak orygina³, puste, nie zatrute
		empty->value[ 0 ] = obj->value[ 0 ];
		empty->value[ 1 ] = 0;
		empty->value[ 2 ] = 0;
		empty->value[ 3 ] = 0;
		empty->value[ 4 ] = 0;

		SET_BIT( empty->wear_flags, obj->wear_flags );
		empty->material = obj->material;
		empty->weight = obj->weight;
        empty->condition = obj->condition;
        empty->rent_cost = RENT_COST( empty->cost );

		wear_loc = obj->wear_loc;

		if ( obj->carried_by )
		{
			obj_from_char( obj );
			obj_to_char( empty, ch);
			if ( wear_loc != WEAR_NONE )
				equip_char( ch, empty, wear_loc, FALSE );
		}
		else
        {
			obj_to_room( empty, ch->in_room );
        }

		extract_obj( obj );

		act( "Ca³kowicie opró¿niasz $h.", ch, empty, NULL, TO_CHAR );
	}

	return;
}

/*void add_food(CHAR_DATA *ch, sh_int down, sh_int up)
{
	sh_int val = 0;

	val = number_range(down,up);
	gain_condition( ch, COND_HUNGER, number_range(down,up) );
}*/

void herb_hallucinations( CHAR_DATA *ch )
{
	AFFECT_DATA af;
	int dur, mod, intt;
	int level = 31;

	intt = get_curr_stat_deprecated( ch, STAT_INT );

	if ( is_affected( ch, gsn_hallucinations ) )return;

	if ( number_range( 1, 2 ) == 2 )  //Kulanie czy positive czy negative
	{
		/*positive - koles ma dola, lepiej trafia, ma rozne ciekawe komunikaty
		oraz o 5% zwiekszona szanse na trafienie krytyczne, o polowe
		trudniej takiemu uciekac, szansa na mini-frenzy*/

		dur = level / 5;
		if ( dice( 2, intt - 20 ) > 5 ) ++dur;
		if ( dice( 2, intt - 18 ) == 2 ) --dur;
		mod = level / 6;

		af.where = TO_AFFECTS;
		af.type = gsn_hallucinations;
		af.level = level;
		af.duration = dur;
	af.rt_duration = 0;
		af.location = APPLY_HITROLL;
		af.modifier = mod;
		af.bitvector = &AFF_HALLUCINATIONS_POSITIVE;
		affect_to_char( ch, &af, NULL, TRUE );

		af.where = TO_AFFECTS;
		af.level = 100; //koles na 'dole' niczego sie nie boi, zgine? etam, no to co:P
		af.location = APPLY_RESIST;
		af.modifier = RESIST_FEAR;
		af.bitvector = &AFF_BRAVE_CLOAK; //do progow if affected
		affect_to_char( ch, &af, NULL, TRUE );

		if ( dice( 2, intt - 19 ) > 5 ) //szansa na jakby mini-frenzy
		{
			af.where = TO_AFFECTS;
			af.type = gsn_hallucinations;
			af.level = level;
			af.location = APPLY_DAMROLL;
			af.modifier = 1;
			affect_to_char( ch, &af, NULL, TRUE );

			af.where = TO_AFFECTS;
			af.type = gsn_hallucinations;
			af.level = level;
			af.location = APPLY_AC;
			af.modifier = 20;
			affect_to_char( ch, &af, NULL, TRUE );

		}
		if ( ch != ch )
		{
			act( "$N spuszcza mêtny ju¿ wzrok i wzdycha ciê¿ko.", ch, NULL, ch, TO_CHAR );
			act( "¦wiat dooko³a traci barwy, ogarnia ciê smutek i przygnêbienie.", ch, NULL, ch, TO_VICT );
		}
		else
		{
			act( "¦wiat dooko³a traci barwy, ogarnia ciê smutek i przygnêbienie.", ch, NULL, ch, TO_CHAR );
		}
		act( "$N spuszcza mêtny ju¿ wzrok i wzdycha ciê¿ko.", ch, NULL, ch, TO_NOTVICT );
		if ( IS_NPC ( ch ) && can_see( ch, ch ) && can_move( ch ) && ch->fighting == NULL )
			multi_hit( ch, ch, TYPE_UNDEFINED );
	}
	else
	{
		/*i negative, koles ma faze, minus do hitrolla, szansa na odwrotnosc
		mini frenzy - skacze sobie wkolko jak najebany*/

		dur = level / 5;
		if ( dice( 2, intt - 20 ) > 5 ) ++dur;
		if ( dice( 2, intt - 18 ) == 2 ) --dur;
		mod = level / 10;

		af.where = TO_AFFECTS;
		af.type = gsn_hallucinations;
		af.level = level;
		af.duration = dur;
	af.rt_duration = 0;
		af.location = APPLY_HITROLL;
		af.modifier = -mod;
		af.bitvector = &AFF_HALLUCINATIONS_NEGATIVE;
		affect_to_char( ch, &af, NULL, TRUE );

		af.where = TO_AFFECTS;
		af.level = 100; //koles na 'fazie' niczego sie nie boi - nie wie co sie dzieje:P
		af.location = APPLY_RESIST;
		af.modifier = RESIST_FEAR;
		af.bitvector = &AFF_BRAVE_CLOAK; //do progow if affected
		affect_to_char( ch, &af, NULL, TRUE );

		if ( dice( 2, intt - 19 ) > 5 )
		{
			af.where = TO_AFFECTS;
			af.type = gsn_hallucinations;
			af.level = level;
			af.location = APPLY_DAMROLL;
			af.modifier = -2;
			affect_to_char( ch, &af, NULL, TRUE );

			af.where = TO_AFFECTS;
			af.type = gsn_hallucinations;
			af.level = level;
			af.location = APPLY_AC;
			af.modifier = -10;
			affect_to_char( ch, &af, NULL, TRUE );

		}
		if ( ch != ch )
		{
			act( "$N rozgl±da siê dooko³a, g³upawo siê u¶miechaj±c.", ch, NULL, ch, TO_CHAR );
			act( "Nagle ¶wiat doko³a ciebie rozb³yskuje têczowymi kolorami! Jak piêknie! Jak wspaniale!", ch, NULL, ch, TO_VICT );
		}
		else
		{
			act( "Nagle ¶wiat doko³a ciebie rozb³yskuje têczowymi kolorami! Jak piêknie! Jak wspaniale!", ch, NULL, ch, TO_CHAR );
		}
		act( "$N rozgl±da siê dooko³a, g³upawo siê u¶miechaj±c.", ch, NULL, ch, TO_NOTVICT );
		if ( IS_NPC ( ch ) && can_see( ch, ch ) && can_move( ch ) && ch->fighting == NULL )
			multi_hit( ch, ch, TYPE_UNDEFINED );
	}
	return;
}

void consume_herbs_eat(CHAR_DATA *ch, sh_int effect)
{
	bool food_added = FALSE;
	//act( "Zjadasz $h.", ch, obj, NULL, TO_CHAR );
	//act( "$n zjada $h.", ch, obj, NULL, TO_ROOM );
	//switch na wszystkie
	switch(effect)
	{
		/*1*/
		case PLANT_EFFECT_POISON_LEVEL1:
		{
			poison_to_char( ch, 1 );
			food_added = TRUE;
			break;
		}
		case PLANT_EFFECT_POISON_LEVEL10:
		{
			poison_to_char( ch, 2);
			food_added = TRUE;
			break;
		}
		case PLANT_EFFECT_POISON_LEVEL20:
		{
			poison_to_char( ch, 3 );
			food_added = TRUE;
			break;
		}
		case PLANT_EFFECT_POISON_LEVEL30:
		{
			poison_to_char( ch, 4 );
			food_added = TRUE;
			break;
		}/*5*/
		case PLANT_EFFECT_POISON_PARALIZING:
		{
			poison_to_char( ch, 7 );
			food_added = TRUE;
			break;
		}
		case PLANT_EFFECT_POISON_DEATH:
		{
			poison_to_char( ch, 8 );
			food_added = TRUE;
			break;
		}
		case PLANT_EFFECT_POISON_WEAKEN:
		{
			poison_to_char( ch, 9 );
			food_added = TRUE;
			break;
		}
		case PLANT_EFFECT_POISON_BLIND:
		{
			poison_to_char( ch, 6 );
			food_added = TRUE;
			break;
		}
		case PLANT_EFFECT_POISON_SLEEP:
		{
			poison_to_char( ch, 5 );
			food_added = TRUE;
			break;
		}/*10*/
		case PLANT_EFFECT_SMALL_HP:	  //1-5HP
		{
			ch->hit = UMIN( ch->hit + number_range(1,5), get_max_hp( ch ) );
			//food_added = FALSE;
			//gain_condition( ch, COND_HUNGER, number_range(3,3) );
			break;
		}
		case PLANT_EFFECT_MEDIUM_HP:	//5-15
		{
			ch->hit = UMIN( ch->hit + number_range(5,15), get_max_hp( ch ) );
			//food_added = FALSE;
			//gain_condition( ch, COND_HUNGER, number_range(3,3) );
			break;
		}
		case PLANT_EFFECT_HIGH_HP:	//15-30
		{
			ch->hit = UMIN( ch->hit + number_range(15,30), get_max_hp( ch ) );
			//gain_condition( ch, COND_HUNGER, number_range(3,3) );
			break;
		}
		case PLANT_EFFECT_SMALL_MOVE:  //1-5MOVE
		{
			ch->move = UMIN( ch->move + number_range(1,5), ch->max_move );
			//gain_condition( ch, COND_HUNGER, number_range(3,3) );
			break;
		}
		case PLANT_EFFECT_MEDIUM_MOVE:	//5-15
		{
			ch->move = UMIN( ch->move + number_range(5,15), ch->max_move );
			//gain_condition( ch, COND_HUNGER, number_range(3,3) );
			break;
		}/*15*/
		case PLANT_EFFECT_HIGH_MOVE://15-30
		{
			ch->move = UMIN( ch->move + number_range(15,25), ch->max_move );
			//gain_condition( ch, COND_HUNGER, number_range(3,3) );
			break;
		}
		case PLANT_EFFECT_SMALL_FOOD://1-5FOOD
		{
			gain_condition( ch, COND_HUNGER, number_range(1,6) );
			food_added = TRUE;
			break;
		}
		case PLANT_EFFECT_MEDIUM_FOOD:	//5-15
		{
			gain_condition( ch, COND_HUNGER, number_range(5,15) );
			food_added = TRUE;
			break;
		}
		case PLANT_EFFECT_HIGH_FOOD:	//15-30
		{
			gain_condition( ch, COND_HUNGER, number_range(15,25) );
			food_added = TRUE;
			break;
		}
		case PLANT_EFFECT_SM_SF:	//AFFECTY 13 + 16
		{
			consume_herbs_eat(ch, 13);
			consume_herbs_eat(ch, 16);
			food_added = TRUE;
			break;
		}
		/*20*/
		case PLANT_EFFECT_CURE_POISON:
		{
			if ( !is_affected( ch, gsn_poison ) )
			{
				send_to_char( "Czujesz dziwne mrowienie ca³ego cia³a, ale nic szczególnego siê nie dzieje.\n\r", ch );
			}
			else
			{
				affect_strip( ch, gsn_poison );
				send_to_char( "Lecznicze ciep³o wype³nia twoje cia³o.\n\r", ch );
				act( "$n wygl±da o wiele lepiej.", ch, NULL, NULL, TO_ROOM );
			}
			break;
		}
		case PLANT_EFFECT_HALLUCINATIONS:	//31 POZIOM
		{
			herb_hallucinations(ch);
			break;
		}
		case PLANT_EFFECT_MM_SF: //14+16
		{
			consume_herbs_eat(ch, 14);
			consume_herbs_eat(ch, 16);
			food_added = TRUE;
			break;
		}
		case PLANT_EFFECT_AID:
		{
			consume_herbs_eat(ch, PLANT_EFFECT_STOP_BLEEDING);
			break;
		}
		case PLANT_EFFECT_SMALL_THIRST: //GASI PRAGNIENIE 1-5
		{
			gain_condition( ch, COND_THIRST, number_range(1,6) );
			break;
		}
		/*25*/
		case PLANT_EFFECT_SF_ST: //16 + 24
		{
			consume_herbs_eat(ch, 44);
			consume_herbs_eat(ch, 16);
			food_added = TRUE;
			break;
		}
		case PLANT_EFFECT_HOLD_PERSON:
		case PLANT_EFFECT_BLIND:
		case PLANT_EFFECT_P30_H:	//4+26
		case PLANT_EFFECT_PB_H: //8+26
			break;
		/*30*/
		case PLANT_EFFECT_MINUS_BIG_HP: //-100 DO 200 HP
		{
			raw_damage( ch, ch, number_range(100,200) );
			break;
		}
		case PLANT_EFFECT_MINUS_SMALL_HP: //-50 DO 100 HP
		{
			raw_damage( ch, ch, number_range(50,100) );
			break;
		}
		case PLANT_EFFECT_ENDURE_POISON: //NA 30 POZIOMIE
		case PLANT_EFFECT_SHOCKING_GRASP:
		case PLANT_EFFECT_SLOW_POISON:
			break;
		/*35*/
		case PLANT_EFFECT_MINUS_SMALL_THIRST: //- 1-10 THIRST
		{
			gain_condition( ch, COND_THIRST, number_range(-10,-1) );
			break;
		}
		case PLANT_EFFECT_MINUS_DEX_P10: //AFFECT NA MINUS DEX O 2
		case PLANT_EFFECT_STOP_BLEEDING:  //zatrzymuje krwawienia
		{
			if ( is_affected ( ch, gsn_bleeding_wound ) || is_affected ( ch, gsn_bleed ) )
			{
				affect_strip( ch, gsn_bleed );
				affect_strip( ch, gsn_bleeding_wound );
				send_to_char( "Twoje rany siê zasklepiaj±.\n\r", ch );
				act( "$n oddycha z ulg±, jego rany przestaj± krwawiæ.", ch, NULL, NULL, TO_ROOM );
			}
		}
		case PLANT_EFFECT_MINUS_MINOR_HP: //-50 DO 100 HP
		{
			raw_damage( ch, ch, number_range(5,15) );
			break;
		}
		case PLANT_EFFECT_SF_SH:
		{
			consume_herbs_eat(ch, 10);
			consume_herbs_eat(ch, 16);
			food_added = TRUE;
			break;
		}
		/*40*/
		case  PLANT_EFFECT_SF_ST_MLH:  //16+24+41
		case PLANT_EFFECT_MINUS_LITTLE_HP:  // - 10-20HP
		case PLANT_EFFECT_MF_MT_MLH:  //17+43+41
		case PLANT_EFFECT_MEDIUM_THIRST:  // THIRST 5-10
		case PLANT_EFFECT_PP_BLIND:  //5 +45
		//case PLANT_EFFECT_BLIND:
		case PLANT_EFFECT_NONE:
		default:
			break;
	}

	if(food_added == FALSE ) gain_condition( ch, COND_HUNGER, number_range(3,3) );

	return;
}

void consume_herbs_smoke(CHAR_DATA *ch, sh_int effect)
{
	return;
}

void consume_herbs( CHAR_DATA *ch, OBJ_DATA *obj, sh_int param ) //param 0 jedzenie, 1 palenie
{
	//return;

	switch(param)
	{
		case 0:{
		//	print_char( ch, "DEBUG: wchodzimy w consume eat parametr:%d\n\r", herb_table[obj->value[ 0 ]].eating_effect );
			consume_herbs_eat(ch,herb_table[obj->value[ 0 ]].eating_effect);
			break;}
		case 1:{
			consume_herbs_smoke(ch,herb_table[obj->value[ 0 ]].smoke_effect);
			break;}

	}

	return;

}
void do_eat( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	int val;

	if ( IS_NPC( ch ) )
		return;

	one_argument( argument, arg );

	if ( ch->fighting )
	{
		send_to_char( "Lepiej skup siê na walce!\n\r", ch );
		return;
	}

	if ( ch->precommand_fun && !ch->precommand_pending )
	{
		send_to_char( "Robisz teraz co¶ innego.\n\r", ch );
		return;
	}

	if ( arg[ 0 ] == '\0' )
	{
		print_char( ch, "Co by¶ zjad³<&/a/o>?\n\r" );
		return;
	}

	if( is_undead(ch) && !IS_IMMORTAL(ch) )
	{
		send_to_char( "Po co chcesz to robiæ?\n\r", ch );
		return;
	}

	obj = get_obj_carry( ch, arg, ch );
// je¿eli obiekt nieistnieje w inventory, to mo¿e kto¶ co¶ takiego trzyma
// w ³apce? z czego próbuje siê napiæ, warto sprawdziæ.
	if ( obj == NULL ) obj = get_eq_char( ch, WEAR_HOLD );

	if ( obj == NULL )
	{
		send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
		return;
	}

	if ( get_obj_memdat( obj, NULL, NULL, MEMDAT_ANY ) && HAS_OTRIGGER( obj, TRIG_PRECOMMAND ) )
	{
		send_to_char( "Kto¶ inny teraz to je. Poczekaj chwilê.\n\r", ch );
		return;
	}

	if ( !IS_IMMORTAL( ch ) )
	{
		if ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL  && obj->item_type != ITEM_HERB)
		{
			send_to_char( "Fuj, tego sie nie da zje¶æ.\n\r", ch );
			return;
		}

		if ( obj->item_type == ITEM_HERB && obj->pIndexData->vnum != 6)//jesli herb ale nie z kodu
		{
			send_to_char( "Fuj, tego sie nie da zje¶æ.\n\r", ch );
			return;
		}

		if ( !IS_NPC( ch ) && obj->item_type != ITEM_PILL && ch->condition[ COND_HUNGER ] > EAT_FULL )
		{
			send_to_char( "Ju¿ chyba wiêcej nie zmie¶cisz.\n\r", ch );
			return;
		}
	}

	/* start food state - by Fuyara */
	if ( !IS_IMMORTAL( ch ) && ( obj->item_type == ITEM_FOOD ) && ( obj->value[2] == FOOD_STATE_RAW ) )
	{
        if ( !( ( ch->class == CLASS_BARBARIAN ) && !str_cmp( race_table[GET_RACE(ch)].name, "pó³ork" ) ) )
        {
            const char * disgusting_food_table[] =
            {
                "Na sam zapach dostajesz md³o¶ci.\n\r",
                "W ¿yciu surowego do ust nie we¼miesz.\n\r",
                "To siê chyba jeszcze rusza...\n\r",
                "£ob¿ydlistwo! Bez obróbki termicznej nie do zjedzenia.\n\r",
                "Ugh! W³a¶nie Ci odesz³a ochota na jedzenie.\n\r"
            };
            //		int length = sizeof( disgusting_food_table ) / sizeof( disgusting_food_table[0] );
            send_to_char( disgusting_food_table[number_range( 0, 4 )], ch );
        }
	    return;
	}
	/* end food state */

	if ( !ch->precommand_pending && HAS_OTRIGGER( obj, TRIG_PRECOMMAND ) )
	{
		if ( op_precommand_trigger( ch, obj, NULL, &do_eat, "eat", argument ) )
        {
			return;
        }
	}

	if ( !op_consume_trigger( obj, ch ) && ( !ch->precommand_pending || !HAS_OTRIGGER( obj, TRIG_PRECOMMAND ) ) )
	{
		val = UMIN( EAT_AMOUNT, obj->value[ 0 ] );
		if ( obj->item_type != ITEM_FOOD || obj->value[ 0 ] - val <= 0 )
		{
			act( "Zjadasz $h.", ch, obj, NULL, TO_CHAR );
			act( "$n zjada $h.", ch, obj, NULL, TO_ROOM );
		}
		else
		{
			act( "Zjadasz kawa³ek $f.", ch, obj, NULL, TO_CHAR );
			act( "$n zjada kawa³ek $f.", ch, obj, NULL, TO_ROOM );
		}
	}

	switch ( obj->item_type )
	{

		case ITEM_FOOD:
			if ( !IS_NPC( ch ) )
			{
				int condition;

				condition = ch->condition[ COND_HUNGER ];
				val = UMIN( EAT_AMOUNT, obj->value[ 0 ] );
				gain_condition( ch, COND_HUNGER, val );
				obj->value[ 0 ] -= val;

				if ( condition == 0 && ch->condition[ COND_HUNGER ] > 0 )
				    print_char( ch, "Ju¿ nie jeste¶ g³odn<&y/a/e>.\n\r" );

				else if ( ch->condition[ COND_HUNGER ] > EAT_FULL )
				    send_to_char( "Twój ¿o³±dek jest juz pe³ny.\n\r", ch );
			}

			poison_from_food(obj, ch);

			break;

		case ITEM_PILL:
			obj_cast_spell( obj->value[ 1 ], obj->value[ 0 ], ch, ch, NULL );
			obj_cast_spell( obj->value[ 2 ], obj->value[ 0 ], ch, ch, NULL );
			obj_cast_spell( obj->value[ 3 ], obj->value[ 0 ], ch, ch, NULL );
			obj_cast_spell( obj->value[ 4 ], obj->value[ 0 ], ch, ch, NULL );
			break;
		case ITEM_HERB:
	//		act( "Zjadasz $h.", ch, obj, NULL, TO_CHAR );
	//		act( "$n zjada $h.", ch, obj, NULL, TO_ROOM );
		//	send_to_char( "DEBUG: w do eat case ITEM_HERB.\n\r", ch );


			consume_herbs(ch, obj, 0);
			if ( ch->condition[ COND_HUNGER ] > EAT_FULL )
				    send_to_char( "Twój ¿o³±dek jest juz pe³ny.\n\r", ch );
			break;
	}


	if ( obj->item_type != ITEM_FOOD )
	{
		/*artefact*/
		if ( is_artefact( obj ) ) extract_artefact( obj );
		if ( obj->contains ) extract_artefact_container( obj );
		extract_obj( obj );
	}
	else
	{
		if ( obj->value[ 0 ] <= 0 )
		{
			//print_char(ch,"Nic juz nie zostalo z %s.\n\r", obj->name2);
			/*artefact*/
			if ( is_artefact( obj ) ) extract_artefact( obj );
			extract_obj( obj );
			return;
		}

		//print_char(ch,"Zostalo jeszcze troche z %s.\n\r", obj->name2);
		return;
	}

	return;
}

/**
 * Poison z jedzenia - wspólny dla eat, feed i mount feed
 */
void poison_from_food(OBJ_DATA *obj, CHAR_DATA *ch)
{
	if ( obj->value[ 3 ] != 0 )
	{
		/* The food was poisoned! */
		AFFECT_DATA af;

		/* barbarzyncy maja tylko 10% na zatrucie sie zepsutym zarciem */
		if ( ch->class == CLASS_BARBARIAN && number_percent() < 90 )
		{
			act( "$n delektuje siê wyszukanym smakiem $f g³o¶no przy tym mlaszcz±c.", ch, obj, 0, TO_ROOM );
			print_char( ch, "%s niezbyt przyjemnie pachnie, jednak ca³kiem ci to smakuje.\n\r", capitalize( obj->short_descr ) );
		}
		else
		{
			// dodajemy odporno¶æ na poison dla undeadów i golemów
			if (!(is_undead(ch) || IS_SET( ch->form, FORM_CONSTRUCT )))
			{
				act( "$n krztusi siê i d³awi.", ch, 0, 0, TO_ROOM );
				send_to_char( "D³awisz siê i krztusisz.\n\r", ch );
				af.where = TO_AFFECTS;
				af.type = gsn_poison;
				af.level = 0;
				af.duration = number_range( 0, 2 );
				af.rt_duration = 0;
				af.location = APPLY_NONE;
				af.modifier = 0;
				af.bitvector = &AFF_POISON;
				affect_join( ch, &af );
			}
		}
	}
}

/* karmienie */
//TODO Rysand przerobiæ dla mountów

void do_feed( CHAR_DATA * ch, char * argument )
{
	char karmiony [ MAX_INPUT_LENGTH ];
	char pozywienie [ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, karmiony );
	argument = one_argument( argument, pozywienie );

	do_feed_body(ch, karmiony, pozywienie);
}

void do_feed_body(CHAR_DATA * ch, char karmiony [ MAX_INPUT_LENGTH ], char pozywienie [ MAX_INPUT_LENGTH ])
{
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	int val, condition;

	if ( karmiony[ 0 ] == '\0' || pozywienie[ 0 ] == '\0' )
	{
		send_to_char( "Kogo nakarmiæ? Czym?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_carry( ch, pozywienie, ch ) ) == NULL )
	{
		send_to_char( "Nie nosisz przy sobie niczego takiego.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL )
	{
		send_to_char( "To nie nadaje siê do jedzenia.\n\r", ch );
		return;
	}

	if ( ch->fighting != NULL )
	{
		send_to_char( "Nie, teraz walcz!\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_CHARM ) )
	{
		send_to_char( "Marzysz sobie jak fajnie by³oby kogo¶ karmiæ...\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room( ch, karmiony ) ) == NULL )
	{
		send_to_char( "Nikogo takiego tu nie ma.\n\r", ch );
		return;
	}

	if ( victim->position < POS_RESTING )
	{
		act( "$N nie jest teraz w stanie niczego zje¶æ.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->position == POS_FIGHTING )
	{
		act( "Jak? przecie¿ $N walczy!", ch, NULL, victim, TO_CHAR );
		return;
	}


	if(!EXT_IS_SET(victim->act, ACT_MOUNTABLE )){
		if ( IS_NPC ( victim ) )
		{



			switch (victim->sex )
			{
			case 0:
				act( "$N nie bardzo chce byæ karmione.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N nie bardzo chce byæ karmiony.", ch, NULL, victim, TO_CHAR );
				break;
			default:
				act( "$N nie bardzo chce byæ karmiona.", ch, NULL, victim, TO_CHAR );
				break;
			}
			return;
		}
	}

	if ( IS_AFFECTED( victim, AFF_FORCE_FIELD ) )
	{
		act( "$N znajduje siê w kopule mocy, nie jeste¶ w stanie tego uczyniæ.", ch, obj, victim, TO_CHAR );
		return;
	}

	if ( victim == ch )
	{
		do_function( ch, do_eat, pozywienie );
		return;
	}


	if ( !is_same_group( ch, victim ) && (!EXT_IS_SET(victim->act, ACT_MOUNTABLE )))
	{
		act( "Obcych ludzi chcesz karmiæ? $N nie nale¿y do twojej grupy.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if( is_undead(victim) && !IS_IMMORTAL(ch) )
	{
		send_to_char( "Po co chcesz to robiæ?\n\r", ch );
		return;
	}

	if ( victim->wait > 0 )
	{
		print_char(ch,"%s wydaje siê byæ teraz czym¶ ",capitalize( PERS( victim, ch ) ) );
		switch ( victim->sex )
		{
			case SEX_NEUTRAL:
				send_to_char("zajête",ch);
				break;
			case SEX_FEMALE:
				send_to_char("zajêta",ch);
				break;
			case SEX_MALE:
			default:
				send_to_char("zajêty",ch);
				break;
		}
		send_to_char(".\n\r",ch);
		WAIT_STATE( ch, PULSE_VIOLENCE );
		return;
	}

	if ( !IS_IMMORTAL( victim ) )
	{
		if ( !IS_NPC( victim ) && obj->item_type != ITEM_PILL && victim->condition[ COND_HUNGER ] > 21 )
		{
			switch ( victim->sex )
			{
				case SEX_NEUTRAL:
				    act( "Próbujesz wepchn±æ $X $h, jednak to wzbrania siê pokazuj±c, ¿e jest ju¿ najedzone.", ch, obj, victim, TO_CHAR );
				    act( "$n próbuje wepchn±æ $h $X, jednak to wzbrania siê pokazuj±c, ¿e jest ju¿ najedzone.", ch, obj, victim, TO_NOTVICT );
				    break;
                case SEX_FEMALE:
				    act( "Próbujesz wepchn±æ $X $h, jednak ta wzbrania siê pokazuj±c, ¿e jest ju¿ najedzona.", ch, obj, victim, TO_CHAR );
				    act( "$n próbuje wepchn±æ $h $X, jednak ta wzbrania siê pokazuj±c, ¿e jest ju¿ najedzona.", ch, obj, victim, TO_NOTVICT );
				    break;
				case SEX_MALE:
                default:
				    act( "Próbujesz wepchn±æ $X $h, jednak ten wzbrania siê pokazuj±c, ¿e jest ju¿ najedzony.", ch, obj, victim, TO_CHAR );
				    act( "$n próbuje wepchn±æ $h $X, jednak ten wzbrania siê pokazuj±c, ¿e jest ju¿ najedzony.", ch, obj, victim, TO_NOTVICT );
				    break;
			}
			act( "$n próbuje ci wepchn±æ $h, jednak ty czujesz siê ju¿ najedzon<&y/a/e> i zas³aniasz swe usta.", ch, obj, victim, TO_VICT );
			return;
		}

		if (EXT_IS_SET(victim->act, ACT_MOUNTABLE ) && obj->item_type != ITEM_PILL && victim->condition[ COND_HUNGER ] > 21 )
		{
			//TODO Rysand przerobiæ komunikaty
			switch ( victim->sex )
			{
				case SEX_NEUTRAL:
				    act( "Próbujesz wepchn±æ $X $h, jednak to wzbrania siê pokazuj±c, ¿e jest ju¿ najedzone.", ch, obj, victim, TO_CHAR );
				    act( "$n próbuje wepchn±æ $h $X, jednak to wzbrania siê pokazuj±c, ¿e jest ju¿ najedzone.", ch, obj, victim, TO_NOTVICT );
				    break;
                case SEX_FEMALE:
				    act( "Próbujesz wepchn±æ $X $h, jednak ta wzbrania siê pokazuj±c, ¿e jest ju¿ najedzona.", ch, obj, victim, TO_CHAR );
				    act( "$n próbuje wepchn±æ $h $X, jednak ta wzbrania siê pokazuj±c, ¿e jest ju¿ najedzona.", ch, obj, victim, TO_NOTVICT );
				    break;
				case SEX_MALE:
                default:
				    act( "Próbujesz wepchn±æ $X $h, jednak ten wzbrania siê pokazuj±c, ¿e jest ju¿ najedzony.", ch, obj, victim, TO_CHAR );
				    act( "$n próbuje wepchn±æ $h $X, jednak ten wzbrania siê pokazuj±c, ¿e jest ju¿ najedzony.", ch, obj, victim, TO_NOTVICT );
				    break;
			}
			act( "$n próbuje ci wepchn±æ $h, jednak ty czujesz siê ju¿ najedzon<&y/a/e> i zas³aniasz swe usta.", ch, obj, victim, TO_VICT );
			return;
		}
	}

	if ( !op_consume_trigger( obj, victim ) )
	{
		val = UMIN( 6, obj->value[ 0 ] );
		if ( obj->item_type != ITEM_FOOD || obj->value[ 0 ] - val <= 0 )
		{
			act( "Karmisz $C $j, $E zjada i nie zostawia resztek.", ch, obj, victim, TO_CHAR );
			act( "$n karmi ciê $j, zjadasz i nie zostawiasz resztek.", ch, obj, victim, TO_VICT );
			act( "$n karmi $C $j, $E zjada i nie zostawia resztek.", ch, obj, victim, TO_NOTVICT );
		}
		else
		{
			act( "Karmisz $C $j, $E zjada pokornie kawa³ek.", ch, obj, victim, TO_CHAR );
			act( "$n karmi ciê $j, zjadasz pokornie kawa³ek.", ch, obj, victim, TO_VICT );
			act( "$n karmi $C $j, $E zjada pokornie kawa³ek.", ch, obj, victim, TO_NOTVICT );
		}
	}

	switch ( obj->item_type )
	{

		case ITEM_FOOD:
			condition = victim->condition[ COND_HUNGER ];
			val = UMIN( 6, obj->value[ 0 ] );
			gain_condition( victim, COND_HUNGER, val );
			obj->value[ 0 ] -= val;

			if ( condition == 0 && ch->condition[ COND_HUNGER ] > 0 )
				print_char( victim, "Ju¿ nie jeste¶ g³odn<&y/a/e>.\n\r" );

			else if ( victim->condition[ COND_HUNGER ] > 21 )
				send_to_char( "Twój ¿o³±dek jest juz pe³ny.\n\r", victim );

			poison_from_food(obj, victim);
	    break;

		case ITEM_PILL:
			obj_cast_spell( obj->value[ 1 ], obj->value[ 0 ], victim, victim, NULL );
			obj_cast_spell( obj->value[ 2 ], obj->value[ 0 ], victim, victim, NULL );
			obj_cast_spell( obj->value[ 3 ], obj->value[ 0 ], victim, victim, NULL );
			obj_cast_spell( obj->value[ 4 ], obj->value[ 0 ], victim, victim, NULL );
			break;
	}

	if ( obj->item_type != ITEM_FOOD )
	{
		/*artefact*/
		if ( is_artefact( obj ) ) extract_artefact( obj );
		if ( obj->contains ) extract_artefact_container( obj );
		extract_obj( obj );
	}
	else
	{
		if ( obj->value[ 0 ] <= 0 )
		{
			//print_char(ch,"Nic juz nie zostalo z %s.\n\r", obj->name2);
			/*artefact*/
			if ( is_artefact( obj ) ) extract_artefact( obj );
			extract_obj( obj );
			return;
		}

		//print_char(ch,"Zostalo jeszcze troche z %s.\n\r", obj->name2);
		return;
	}

	return;
}

/* napoiæ */
//TODO Rysand przerobiæ dla mounta
void do_water( CHAR_DATA * ch, char * argument )
{
	char pojony [ MAX_INPUT_LENGTH ];
	char napitek [ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, pojony );
	argument = one_argument( argument, napitek );

	do_water_body(ch, pojony, napitek);
}

void do_water_body(CHAR_DATA * ch, char pojony [ MAX_INPUT_LENGTH ], char napitek [ MAX_INPUT_LENGTH ])
{

	char buf[ MAX_STRING_LENGTH ];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	int liquid, amount=0;

	if ( pojony[ 0 ] == '\0')
	{
		send_to_char( "Kogo napoiæ?\n\r", ch );
		return;
	}


	if ( ( victim = get_char_room( ch, pojony ) ) == NULL )
	{
		send_to_char( "Nikogo takiego tu nie ma.\n\r", ch );
		return;
	}

	if ( napitek[ 0 ] == '\0' )
	{
		if ( IS_SET( sector_table[ victim->in_room->sector_type ].flag, SECT_CANDRINK ) )
		{
			act( "$n pije ile wlezie.", victim, NULL, NULL, TO_ROOM );
			gain_condition( victim, COND_THIRST, DRINK_AMOUNT );
			return;
		}
	}

	if ( pojony[ 0 ] == '\0' || napitek[ 0 ] == '\0' )
	{
		send_to_char( "Kogo napoiæ? Czym?\n\r", ch );
		return;
	}

	if((obj = get_obj_here(ch, napitek)) == NULL)
	{
		send_to_char( "Nie ma tu niczego takiego.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_DRINK_CON && obj->item_type != ITEM_POTION && obj->item_type != ITEM_FOUNTAIN)
	{
		send_to_char( "To nie jest pojemnik na p³yny.\n\r", ch );
		return;
	}

	if ( ch->fighting != NULL )
	{
		send_to_char( "Nie, teraz walcz!\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_CHARM ) )
	{
		send_to_char( "Marzysz sobie jak fajnie by³oby kogo¶ poiæ...\n\r", ch );
		return;
	}

	if ( victim->position < POS_RESTING )
	{
		act( "$N nie jest teraz w stanie niczego wypiæ.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->position == POS_FIGHTING )
	{
		act( "Jak? przecie¿ $N walczy!", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_undead(victim) || IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		act( "$N chyba nie potrzebuje piæ.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if(!EXT_IS_SET(victim->act, ACT_MOUNTABLE )){
		if ( IS_NPC( victim ))
		{
			switch (victim->sex )
			{
			case SEX_NEUTRAL:
				act( "$N nie wygl±da na spragnione.", ch, NULL, victim, TO_CHAR );
				break;
			case SEX_MALE:
				act( "$N nie wygl±da na spragnionego.", ch, NULL, victim, TO_CHAR );
				break;
			case SEX_FEMALE:
			default:
				act( "$N nie wygl±da na spragnion±.", ch, NULL, victim, TO_CHAR );
				break;
			}
			return;
		}
	}

	if ( victim == ch )
	{
		if ( obj->item_type != ITEM_POTION )
			do_function( ch, do_drink, napitek );
		else
			do_function( ch, do_quaff, napitek );
		return;
	}

	if ( !is_same_group( ch, victim ) && !EXT_IS_SET(victim->act, ACT_MOUNTABLE ))
	{
		act( "Obcych ludzi chcesz poiæ? $N nie nale¿y do twojej grupy.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( (obj->value[ 1 ] <= 0) && (obj->item_type != ITEM_FOUNTAIN) )
	{
		send_to_char( "To jest puste.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_FORCE_FIELD ) )
	{
		act( "$N znajduje siê w kopule mocy, nie jeste¶ w stanie tego uczyniæ.", ch, obj, victim, TO_CHAR );
		return;
	}

	if ( victim->wait > 0 )
	{
        print_char(ch,"%s wydaje siê byæ teraz czym¶ ",capitalize( PERS( victim, ch ) ) );
        switch ( victim->sex )
        {
            case SEX_NEUTRAL:
                send_to_char("zajête",ch);
                break;
            case SEX_FEMALE:
                send_to_char("zajêta",ch);
                break;
            case SEX_MALE:
            default:
                send_to_char("zajêty",ch);
                break;
        }
        send_to_char(".\n\r",ch);
		WAIT_STATE( ch, PULSE_VIOLENCE );
		return;
	}
    if (!can_see_obj( victim, obj ))
    {
		act( "Przecie¿ $N tego nie widzi!.", ch, NULL, victim, TO_CHAR );
		return;
    }

	if( obj->item_type != ITEM_POTION )
	{
		if ( ( liquid = obj->value[ 2 ] ) < 0 )
		{
			bug( "Do_drink: bad liquid number %d.", liquid );
			liquid = obj->value[ 2 ] = 0;
		}

		amount = liq_table[ liquid ].liq_affect[ 4 ];
		amount = UMIN( amount, obj->value[ 1 ] );
	}

	if ( !IS_IMMORTAL( victim ) )
	{
		if ( victim->condition[ COND_THIRST ] > DRINK_FULL && obj->item_type != ITEM_POTION )
		{
			switch ( victim->sex )
			{
				case 0:
				    sprintf( buf, "Próbujesz wlaæ $X do ust %s z $f, jednak to wzbrania siê pokazuj±c, ¿e wiêcej nie jest w stanie wypiæ.", liq_table[ liquid ].liq_name4 );
				    act( buf, ch, obj, victim, TO_CHAR );
				    sprintf( buf, "$n próbuje wlaæ do ust $Z %s z $f, jednak to wzbrania siê pokazuj±c, ¿e wiêcej nie jest w stanie wypiæ", liq_table[ liquid ].liq_name4 );
				    act( buf, ch, obj, victim, TO_NOTVICT );
				    break;
				case 1:
				    sprintf( buf, "Próbujesz wlaæ $X do ust %s z $f, jednak ten wzbrania siê pokazuj±c, ¿e wiêcej nie jest w stanie wypiæ.", liq_table[ liquid ].liq_name4 );
				    act( buf, ch, obj, victim, TO_CHAR );
				    sprintf( buf, "$n próbuje wlaæ do ust $Z %s z $f, jednak ten wzbrania siê pokazuj±c, ¿e wiêcej nie jest w stanie wypiæ", liq_table[ liquid ].liq_name4 );
				    act( buf, ch, obj, victim, TO_NOTVICT );
				    break;
				default:
				    sprintf( buf, "Próbujesz wlaæ $X do ust %s z $f, jednak ta wzbrania siê pokazuj±c, ¿e wiêcej nie jest w stanie wypiæ.", liq_table[ liquid ].liq_name4 );
				    act( buf, ch, obj, victim, TO_CHAR );
				    sprintf( buf, "$n próbuje wlaæ do ust $Z %s z $f, jednak ta wzbrania siê pokazuj±c, ¿e wiêcej nie jest w stanie wypiæ", liq_table[ liquid ].liq_name4 );
				    act( buf, ch, obj, victim, TO_NOTVICT );
				    break;

			}
			sprintf( buf, "$n próbuje ci wlaæ do ust %s z $f, jednak ty czujesz, ¿e wiêcej nie jeste¶ w stanie wypiæ, i zas³aniasz swoje usta.", liq_table[ liquid ].liq_name4 );
			act( buf, ch, obj, victim, TO_VICT );
			return;
		}
	}

	if ( !op_consume_trigger( obj, victim ) && obj->item_type != ITEM_POTION && obj->item_type != ITEM_FOUNTAIN)
	{
		sprintf( buf, "Wlewasz do ust $Z %s ze $f.", liq_table[ liquid ].liq_name4 );
		act( buf, ch, obj, victim, TO_CHAR );
		sprintf( buf, "$n wlewa ci do ust %s ze $f.", liq_table[ liquid ].liq_name4 );
		act( buf, ch, obj, victim, TO_VICT );
		sprintf( buf, "$n wlewa do ust $Z %s ze $f.", liq_table[ liquid ].liq_name4 );
		act( buf, ch, obj, victim, TO_NOTVICT );
	}
	else if( obj->item_type == ITEM_POTION )
	{
		act( "Wlewasz do ust $Z miksturê ze $f.", ch, obj, victim, TO_CHAR );
		act( "$n wlewa ci do ust miksturê ze $f.", ch, obj, victim, TO_VICT );
		act( "$n wlewa do ust $Z miksturê ze $f.", ch, obj, victim, TO_NOTVICT );
		obj_cast_spell( obj->value[ 1 ], obj->value[ 0 ], victim, victim, NULL );
		obj_cast_spell( obj->value[ 2 ], obj->value[ 0 ], victim, victim, NULL );
		obj_cast_spell( obj->value[ 3 ], obj->value[ 0 ], victim, victim, NULL );
		obj_cast_spell( obj->value[ 4 ], obj->value[ 0 ], victim, victim, NULL );
		return;
	}else if(obj->item_type == ITEM_FOUNTAIN){
		sprintf( buf, "Poisz $Z %s z $f.", liq_table[ liquid ].liq_name5 );
		act( buf, ch, obj, victim, TO_CHAR );
		sprintf( buf, "$n poi $Z %s z $f.", liq_table[ liquid ].liq_name5 );
		act( buf, ch, obj, victim, TO_NOTVICT );
	}

	if(amount > -1){
		gain_condition( victim, COND_DRUNK, amount * liq_table[ liquid ].liq_affect[ COND_DRUNK ] );
		gain_condition( victim, COND_THIRST, amount * liq_table[ liquid ].liq_affect[ COND_THIRST ] );
		gain_condition( victim, COND_HUNGER, amount * liq_table[ liquid ].liq_affect[ COND_HUNGER ] );
	}
	else{
		gain_condition( victim, COND_DRUNK, DRUNK_FULL * liq_table[ liquid ].liq_affect[ COND_DRUNK ] );
		gain_condition( victim, COND_THIRST, DRINK_FULL * liq_table[ liquid ].liq_affect[ COND_THIRST ] );
		gain_condition( victim, COND_HUNGER, EAT_FULL * liq_table[ liquid ].liq_affect[ COND_HUNGER ] );
	}

	if ( victim->condition[ COND_DRUNK ] && liq_table[ liquid ].liq_affect[ COND_DRUNK ] != 0 )
		send_to_char( "Krêci ci siê w g³owie.\n\r", victim );

	if ( victim->condition[ COND_THIRST ] > DRINK_PART )
		send_to_char( "Twoje pragnienie jest zaspokojone.\n\r", victim );

	if ( obj->value[ 3 ] != 0 )
	{
		/* The drink was poisoned ! */
		AFFECT_DATA af;
		act( "$n krztusi siê i kaszle.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Krztusisz siê i kaszlesz.\n\r", victim );
		af.where = TO_AFFECTS;
		af.type = gsn_poison;
		af.level = 0;
		af.duration = 2; af.rt_duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &AFF_POISON;
		affect_join( victim, &af );
	}

	if ( obj->value[ 0 ] > 0 )
		obj->value[ 1 ] -= amount;

	if ( victim->condition[ COND_DRUNK ] > DRUNK_FULL )
	{
		sprintf( buf, "Pij±c %s z $f zauwa¿asz, ¿e ¶wiat dooko³a zaczyna zabawnie falowaæ i rozmazywaæ siê...", liq_table[ liquid ].liq_name4 );
		act( buf, ch, obj, victim, TO_VICT );
		sprintf( buf, "$N z po¿±daniem ch³epta %s czkaj±c co chwilkê, po czym rozgl±da siê pijanym wzrokiem.", liq_table[ liquid ].liq_name4 );
		act( buf, ch, obj, victim, TO_NOTVICT );
		sprintf( buf, "$N z po¿±daniem ch³epta %s czkaj±c co chwilkê, po czym rozgl±da siê pijanym wzrokiem.", liq_table[ liquid ].liq_name4 );
		act( buf, ch, obj, victim, TO_CHAR );
		if ( obj->value[ 0 ] > 0 ) obj->value[ 1 ] = UMAX( 0, obj->value[ 1 ] - 2 * amount );
	}

	return;
}

/*
 * Remove an object.
 */
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
	OBJ_DATA * obj, *cover;
	extern int wear_to_itemwear[ MAX_WEAR ];

	cover = get_eq_char( ch, WEAR_ABOUT );
	if ( cover && !IS_OBJ_STAT( cover, ITEM_COVER ) ) cover = NULL;

	if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
		return TRUE;

	if ( cover
		 && IS_SET( cover->wear_flags, wear_to_itemwear[ iWear ] ) )
	{
		send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
		return FALSE;
	}

	if ( !fReplace )
		return FALSE;

	if ( IS_OBJ_STAT( obj, ITEM_NOREMOVE ) && !IS_IMMORTAL(ch) )
	{
		act( "Nie mo¿esz zdj±æ $f.", ch, obj, NULL, TO_CHAR );
		return FALSE;
	}

	if( IS_OBJ_STAT( obj, ITEM_DOUBLE_GRIP ) && obj->item_type == ITEM_WEAPON )
	{
		REMOVE_BIT( obj->value[4], WEAPON_TWO_HANDS );
    	EXT_REMOVE_BIT( obj->extra_flags, ITEM_DOUBLE_GRIP );
    }

    if( obj->item_type == ITEM_WEAPON && is_affected(ch,gsn_glorious_impale) )
    {
    	affect_strip( ch, gsn_glorious_impale );
    	if( ch->fighting && IS_AFFECTED( ch->fighting, AFF_PARALYZE) && is_affected(ch->fighting, gsn_glorious_impale ) )
    	{
    		act( "Gwa³townie wyrywasz $h z cia³a $Z powiêkszaj±c jeszcze ranê.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_CHAR );
    		act( "$n gwa³townie wyrywa $h z twojego cia³a powiêkszaj±c jeszcze ranê.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_VICT );
			act( "$n gwa³townie wyrywa $h z cia³a $Z powiêkszaj±c jeszcze ranê.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_NOTVICT );
			affect_strip( ch->fighting, gsn_glorious_impale );
			damage( ch, ch->fighting, 2*(dice(get_eq_char(ch,WEAR_WIELD)->value[1],get_eq_char(ch,WEAR_WIELD)->value[2])+get_eq_char(ch,WEAR_WIELD)->value[6]+GET_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))+GET_SKILL_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))),gsn_glorious_impale, DAM_PIERCE, FALSE );
		}
	}

	if ( HAS_OTRIGGER( obj, TRIG_PREREMOVE ) )
		op_common_trigger( ch, obj, &TRIG_PREREMOVE );
	else
	{
		unequip_char( ch, obj );
		act( "$n przestaje u¿ywaæ $f.", ch, obj, NULL, TO_ROOM );
		act( "Przestajesz u¿ywaæ $f.", ch, obj, NULL, TO_CHAR );
		if ( HAS_OTRIGGER( obj, TRIG_REMOVE ) )
			op_common_trigger( ch, obj, &TRIG_REMOVE );
	}
	return TRUE;
}

void do_hold( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Co chcesz za³o¿yæ?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	{
		send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
		return;
	}

	if ( obj->item_type == ITEM_WEAPON )
		wield_weapon( ch, obj, FALSE );
	else
		wear_obj( ch, obj, TRUE );
	return;
}

void do_wield( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Której broni chcesz u¿yæ?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	{
		send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_WEAPON )
	{
		print_char( ch, "%s nie wygl±da na broñ, poszukaj czego¶ lepszego.\n\r", obj->short_descr );
		return;
	}

	wield_weapon( ch, obj, TRUE );
	return;
}

int get_hand_slots( CHAR_DATA *ch, int wear )
{
	OBJ_DATA * weapon;
	int slots = 0;

	if ( wear != WEAR_LIGHT && get_eq_char( ch, WEAR_LIGHT ) != NULL ) slots++;
	if ( wear != WEAR_HOLD && get_eq_char( ch, WEAR_HOLD ) != NULL ) slots++;
	if ( wear != WEAR_SHIELD && get_eq_char( ch, WEAR_SHIELD ) != NULL ) slots++;
	if ( wear != WEAR_INSTRUMENT && get_eq_char( ch, WEAR_INSTRUMENT ) != NULL ) slots += 2;
	if ( wear != WEAR_WIELD )
	{
		weapon = get_eq_char( ch, WEAR_WIELD );
		if ( weapon != NULL )
		{
			if ( IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS ) )
				slots += 2;
			else
				slots++;
		}
	}
	if ( wear != WEAR_SECOND && get_eq_char( ch, WEAR_SECOND ) != NULL ) slots++;

	return slots;
}

void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
    OBJ_DATA * cover;
    CHAR_DATA *vch, *vch_next;
    char buf[ MAX_STRING_LENGTH ];

    if ( IS_OBJ_STAT( obj, ITEM_COVER ) )
    {
        if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
        {
            return;
        }
        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_ABOUT ) )
            {
                return;
            }
            obj->prewear_loc = WEAR_ABOUT;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
        {
            if ( equip_char( ch, obj, WEAR_ABOUT, TRUE ) )
            {
                act( "$n narzuca $h na swoje cia³o.", ch, obj, NULL, TO_ROOM );
                act( "Narzucasz $h na swoje cia³o.", ch, obj, NULL, TO_CHAR );
                obj_trap_handler( ch, obj, TRAP_WEAR );
                if ( !ch->in_room )
                {
                    return;
                }
                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                {
                    op_common_trigger( ch, obj, &TRIG_WEAR );
                }
            }
        }
        return;
    }

    if ( ch->fighting && !CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
    {
        print_char( ch, "Nie masz czasu ¿eby za³o¿yæ %s, walcz!\n\r", obj->name4, ch );
        return;
    }

    cover = get_eq_char( ch, WEAR_ABOUT );
    if ( cover && !IS_OBJ_STAT( cover, ITEM_COVER ) )
    {
        cover = NULL;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_LIGHT ) )
    {

        if ( cover
                && IS_SET( cover->wear_flags, ITEM_WEAR_LIGHT ) )
        {
            send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
            return;
        }
        if ( get_hand_slots( ch, WEAR_LIGHT ) >= 2 )
        {
            send_to_char( "Masz ju¿ dwie zajête rêce. Zdejmij co¶.\n\r", ch );
            return;
        }
        if ( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
        {
            return;
        }
        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_LIGHT ) )
                return;
            obj->prewear_loc = WEAR_LIGHT;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
        {
            if ( equip_char( ch, obj, WEAR_LIGHT, TRUE ) )
            {
                if (obj->value[2] == 0)
                {
                    act( "$n zapala i unosi $h nad g³ow±, ale nie daje to ani odrobiny ¶wiat³a.", ch, obj, NULL, TO_ROOM );
                    act( "Podnosisz $h nad g³ow±, ale nie daje to ani odrobiny ¶wiat³a.", ch, obj, NULL, TO_CHAR );
                }
                else
                {
                    sprintf( buf, "%s%s%s", "Zapalasz i unosisz ", obj->name4, " nad g³ow± rozja¶niaj±c nieco panuj±cy wcze¶niej mrok.\n\r");
                    act( buf, ch, NULL, NULL, TO_CHAR );
                    for(vch=ch->in_room->people;vch;vch=vch_next)
                    {
                        vch_next=vch->next_in_room;
                        if ( !IS_NPC( vch )
                                && !EXT_IS_SET( vch->act, PLR_HOLYLIGHT )
                                && !IS_AFFECTED( vch, AFF_DARK_VISION )
                                && !IS_AFFECTED( vch, AFF_BLIND )
                                && vch->position != POS_SLEEPING )
                        {
                            if (vch != ch)
                            {
                                sprintf( buf, "%s%s%s%s", ( can_see(vch,ch) ? ch->name : "Kto¶" ), " zapala i unosi ", obj->name4, " nad g³ow± rozja¶niaj±c nieco panuj±cy wcze¶niej mrok.\n\r");
                                act( buf, vch, NULL, vch, TO_CHAR );
                            }
                            do_function(vch, &do_look, "auto");
                        }
                    }
                }
                obj_trap_handler( ch, obj, TRAP_WEAR );
                if ( !ch->in_room )
                {
                    return;
                }
                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                {
                    op_common_trigger( ch, obj, &TRIG_WEAR );
                }
            }
        }
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) && GOT_PART( ch, PART_FINGERS ) )
    {

        if ( cover && IS_SET( cover->wear_flags, ITEM_WEAR_FINGER ) )
        {
            send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
            return;
        }

        if (
                get_eq_char( ch, WEAR_FINGER_L ) != NULL
                && get_eq_char( ch, WEAR_FINGER_R ) != NULL
                && !remove_obj( ch, WEAR_FINGER_L, fReplace )
                && !remove_obj( ch, WEAR_FINGER_R, fReplace )
           )
        {
            return;
        }

        if ( get_eq_char( ch, WEAR_FINGER_L ) == NULL )
        {
            if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
            {
                if ( !can_equip_obj( ch, obj, WEAR_FINGER_L ) ) return;
                obj->prewear_loc = WEAR_FINGER_L;
                op_common_trigger( ch, obj, &TRIG_PREWEAR );
            }
            else
            {
                if ( equip_char( ch, obj, WEAR_FINGER_L, TRUE ) )
                {
                    act( "$n wsuwa $h na serdeczny palec lewej rêki.", ch, obj, NULL, TO_ROOM );
                    act( "Wsuwasz $h na serdeczny palec lewej rêki.", ch, obj, NULL, TO_CHAR );

                    obj_trap_handler( ch, obj, TRAP_WEAR );

                    if ( !ch->in_room )
                    {
                        return;
                    }

                    if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    {
                        op_common_trigger( ch, obj, &TRIG_WEAR );
                    }
                }
            }
            return;
        }

        if ( get_eq_char( ch, WEAR_FINGER_R ) == NULL )
        {
            if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
            {
                if ( !can_equip_obj( ch, obj, WEAR_FINGER_R ) ) return;
                obj->prewear_loc = WEAR_FINGER_R;
                op_common_trigger( ch, obj, &TRIG_PREWEAR );
            }
            else
                if ( equip_char( ch, obj, WEAR_FINGER_R, TRUE ) )
                {
                    act( "$n wsuwa $h na serdeczny palec prawej rêki.", ch, obj, NULL, TO_ROOM );
                    act( "Wsuwasz $h na serdeczny palec prawej rêki.", ch, obj, NULL, TO_CHAR );

                    obj_trap_handler( ch, obj, TRAP_WEAR );

                    if ( !ch->in_room )
                        return;

                    if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                        op_common_trigger( ch, obj, &TRIG_WEAR );

                }
            return;
        }

        bug( "Wear_obj: no free finger.", 0 );
        send_to_char( "Masz juz dwa pier¶cienie.\n\r", ch );
        return;
    }

    //musi miec glowe...
    if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) && GOT_PART( ch, PART_HEAD ) )
    {
        if ( cover && IS_SET( cover->wear_flags, ITEM_WEAR_NECK ) )
        {
            send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
            return;
        }

        if (
                get_eq_char( ch, WEAR_NECK_1 ) != NULL
                && get_eq_char( ch, WEAR_NECK_2 ) != NULL
                && !remove_obj( ch, WEAR_NECK_1, fReplace )
                && !remove_obj( ch, WEAR_NECK_2, fReplace )
           )
        {
            return;
        }
        if ( get_eq_char( ch, WEAR_NECK_1 ) == NULL )
        {
            if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
            {
                if ( !can_equip_obj( ch, obj, WEAR_NECK_1 ) )
                {
                    return;
                }
                obj->prewear_loc = WEAR_NECK_1;
                op_common_trigger( ch, obj, &TRIG_PREWEAR );
            }
            else
                if ( equip_char( ch, obj, WEAR_NECK_1, TRUE ) )
                {
                    act( "$n zak³ada $h wokó³ szyi.", ch, obj, NULL, TO_ROOM );
                    act( "Zak³adasz $h wokó³ szyi.", ch, obj, NULL, TO_CHAR );
                    obj_trap_handler( ch, obj, TRAP_WEAR );
                    if ( !ch->in_room )
                    {
                        return;
                    }
                    if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    {
                        op_common_trigger( ch, obj, &TRIG_WEAR );
                    }
                }
            return;
        }

        if ( get_eq_char( ch, WEAR_NECK_2 ) == NULL )
        {
            if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
            {
                if ( !can_equip_obj( ch, obj, WEAR_NECK_2 ) ) return;
                obj->prewear_loc = WEAR_NECK_2;
                op_common_trigger( ch, obj, &TRIG_PREWEAR );
            }
            else
            {
                if ( equip_char( ch, obj, WEAR_NECK_2, TRUE ) )
                {
                    act( "$n zak³ada $h wokó³ szyi.", ch, obj, NULL, TO_ROOM );
                    act( "Zak³adasz $h wokó³ szyi.", ch, obj, NULL, TO_CHAR );
                    obj_trap_handler( ch, obj, TRAP_WEAR );
                    if ( !ch->in_room )
                    {
                        return;
                    }
                    if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    {
                        op_common_trigger( ch, obj, &TRIG_WEAR );
                    }
                }
            }
            return;
        }
        bug( "Wear_obj: no free neck.", 0 );
        send_to_char( "Masz ju¿ dwie rzeczy na szyi.\n\r", ch );
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
    {
        if ( cover
                && IS_SET( cover->wear_flags, ITEM_WEAR_BODY ) )
        {
            send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
            return;
        }

        if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
            return;

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_BODY ) ) return;
            obj->prewear_loc = WEAR_BODY;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_BODY, TRUE ) )
            {
                act( "$n zak³ada $h na cia³o.", ch, obj, NULL, TO_ROOM );
                act( "Zak³adasz $h na cia³o.", ch, obj, NULL, TO_CHAR );

                obj_trap_handler( ch, obj, TRAP_WEAR );

                if ( !ch->in_room )
                    return;

                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    op_common_trigger( ch, obj, &TRIG_WEAR );

            }
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) && GOT_PART( ch, PART_HEAD ) )
    {
        if ( cover
                && IS_SET( cover->wear_flags, ITEM_WEAR_HEAD ) )
        {
            send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
            return;
        }

        if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
            return;

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_HEAD ) )
                return;
            obj->prewear_loc = WEAR_HEAD;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_HEAD, TRUE ) )
            {
                act( "$n zak³ada $h na g³owê.", ch, obj, NULL, TO_ROOM );
                act( "Zak³adasz $h na g³owê.", ch, obj, NULL, TO_CHAR );

                obj_trap_handler( ch, obj, TRAP_WEAR );

                if ( !ch->in_room )
                    return;

                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    op_common_trigger( ch, obj, &TRIG_WEAR );
            }
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) && GOT_PART( ch, PART_LEGS ) )
    {
        if ( cover
                && IS_SET( cover->wear_flags, ITEM_WEAR_LEGS ) )
        {
            send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
            return;
        }

        if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
            return;

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_LEGS ) ) return;
            obj->prewear_loc = WEAR_LEGS;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_LEGS, TRUE ) )
            {
                act( "$n wsuwa $h na nogi.", ch, obj, NULL, TO_ROOM );
                act( "Wsuwasz $h na nogi.", ch, obj, NULL, TO_CHAR );

                obj_trap_handler( ch, obj, TRAP_WEAR );

                if ( !ch->in_room )
                    return;

                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    op_common_trigger( ch, obj, &TRIG_WEAR );

            }
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) && GOT_PART( ch, PART_FEET ) )
    {
        if ( cover
                && IS_SET( cover->wear_flags, ITEM_WEAR_FEET ) )
        {
            send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
            return;
        }

        if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
            return;

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_FEET ) ) return;
            obj->prewear_loc = WEAR_FEET;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_FEET, TRUE ) )
            {
                act( "$n zak³ada $h na stopy.", ch, obj, NULL, TO_ROOM );
                act( "Zak³adasz $h na stopy.", ch, obj, NULL, TO_CHAR );

                obj_trap_handler( ch, obj, TRAP_WEAR );

                if ( !ch->in_room )
                    return;

                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    op_common_trigger( ch, obj, &TRIG_WEAR );

            }
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) && GOT_PART( ch, PART_HANDS ) )
    {
        if ( cover
                && IS_SET( cover->wear_flags, ITEM_WEAR_HANDS ) )
        {
            send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
            return;
        }

        if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
            return;

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_HANDS ) ) return;
            obj->prewear_loc = WEAR_HANDS;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_HANDS, TRUE ) )
            {
                act( "$n zak³ada $h na rêce.", ch, obj, NULL, TO_ROOM );
                act( "Zak³adasz $h na rêce.", ch, obj, NULL, TO_CHAR );

                obj_trap_handler( ch, obj, TRAP_WEAR );

                if ( !ch->in_room )
                    return;

                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    op_common_trigger( ch, obj, &TRIG_WEAR );

            }
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) && GOT_PART( ch, PART_ARMS ) )
    {
        if ( cover
                && IS_SET( cover->wear_flags, ITEM_WEAR_ARMS ) )
        {
            send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
            return;
        }

        if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
            return;

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_ARMS ) ) return;
            obj->prewear_loc = WEAR_ARMS;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_ARMS, TRUE ) )
            {
                act( "$n zak³ada $h na ramiona.", ch, obj, NULL, TO_ROOM );
                act( "Zak³adasz $h na ramiona.", ch, obj, NULL, TO_CHAR );

                obj_trap_handler( ch, obj, TRAP_WEAR );

                if ( !ch->in_room )
                    return;

                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    op_common_trigger( ch, obj, &TRIG_WEAR );

            }
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
    {
        if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
            return;

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_ABOUT ) ) return;
            obj->prewear_loc = WEAR_ABOUT;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_ABOUT, TRUE ) )
            {
                act( "$n narzuca $h na swoje cia³o.", ch, obj, NULL, TO_ROOM );
                act( "Narzucasz $h na swoje cia³o.", ch, obj, NULL, TO_CHAR );

                obj_trap_handler( ch, obj, TRAP_WEAR );

                if ( !ch->in_room )
                    return;

                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    op_common_trigger( ch, obj, &TRIG_WEAR );

            }
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
    {
        if ( cover
                && IS_SET( cover->wear_flags, ITEM_WEAR_WAIST ) )
        {
            send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
            return;
        }

        if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
            return;

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_WAIST ) ) return;
            obj->prewear_loc = WEAR_WAIST;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_WAIST, TRUE ) )
            {
                act( "$n zapina $h wokó³ pasa.", ch, obj, NULL, TO_ROOM );
                act( "Zak³adasz $h wokó³ pasa.", ch, obj, NULL, TO_CHAR );

                obj_trap_handler( ch, obj, TRAP_WEAR );

                if ( !ch->in_room )
                    return;

                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    op_common_trigger( ch, obj, &TRIG_WEAR );

            }
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) && GOT_PART( ch, PART_ARMS ) )
    {
        if ( cover
                && IS_SET( cover->wear_flags, ITEM_WEAR_WRIST ) )
        {
            send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
            return;
        }

        if ( get_eq_char( ch, WEAR_WRIST_L ) != NULL
                && get_eq_char( ch, WEAR_WRIST_R ) != NULL
                && !remove_obj( ch, WEAR_WRIST_L, fReplace )
                && !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
            return;

        if ( get_eq_char( ch, WEAR_WRIST_L ) == NULL )
        {
            if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
            {
                if ( !can_equip_obj( ch, obj, WEAR_WRIST_L ) ) return;
                obj->prewear_loc = WEAR_WRIST_L;
                op_common_trigger( ch, obj, &TRIG_PREWEAR );
            }
            else
                if ( equip_char( ch, obj, WEAR_WRIST_L, TRUE ) )
                {
                    act( "$n zak³ada $h na lewy nadgarstek.", ch, obj, NULL, TO_ROOM );
                    act( "Zak³adasz $h na lewy nadgarstek.", ch, obj, NULL, TO_CHAR );

                    obj_trap_handler( ch, obj, TRAP_WEAR );

                    if ( !ch->in_room )
                        return;

                    if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                        op_common_trigger( ch, obj, &TRIG_WEAR );

                }
            return;
        }

        if ( get_eq_char( ch, WEAR_WRIST_R ) == NULL )
        {
            if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
            {
                if ( !can_equip_obj( ch, obj, WEAR_WRIST_R ) ) return;
                obj->prewear_loc = WEAR_WRIST_R;
                op_common_trigger( ch, obj, &TRIG_PREWEAR );
            }
            else
                if ( equip_char( ch, obj, WEAR_WRIST_R, TRUE ) )
                {
                    act( "$n zak³ada $h na prawy nadgarstek.", ch, obj, NULL, TO_ROOM );
                    act( "Zak³adasz $h na prawy nadgarstek.", ch, obj, NULL, TO_CHAR );

                    obj_trap_handler( ch, obj, TRAP_WEAR );

                    if ( !ch->in_room )
                        return;

                    if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                        op_common_trigger( ch, obj, &TRIG_WEAR );

                }
            return;
        }

        bug( "Wear_obj: no free wrist.", 0 );
        send_to_char( "Nie masz ju¿ tego gdzie za³o¿yæ.\n\r", ch );
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) && GOT_PART( ch, PART_HANDS ) )
    {
        if ( cover
                && IS_SET( cover->wear_flags, ITEM_WEAR_SHIELD ) )
        {
            send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
            return;
        }

        if ( get_eq_char ( ch, WEAR_SECOND ) != NULL )
        {
            send_to_char ( "Nie mo¿esz chwyciæ tarczy trzymaj±c broñ w drugiej rêce.\n\r", ch );
            return;
        }

        if ( get_hand_slots( ch, WEAR_SHIELD ) >= 2 )
        {
            send_to_char( "Masz ju¿ dwie zajête rêce. Zdejmij co¶.\n\r", ch );
            return;
        }

        if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
            return;

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_SHIELD ) ) return;
            obj->prewear_loc = WEAR_SHIELD;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_SHIELD, TRUE ) )
            {
                act( "$n chwyta do rêki $h.", ch, obj, NULL, TO_ROOM );
                act( "Chwytasz do rêki $h.", ch, obj, NULL, TO_CHAR );

                obj_trap_handler( ch, obj, TRAP_WEAR );

                if ( !ch->in_room )
                    return;

                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    op_common_trigger( ch, obj, &TRIG_WEAR );
            }
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WIELD ) || obj->item_type == ITEM_WEAPON )
    {
        send_to_char( "Chcia³<&/a/o>by¶ broñ za³o¿yæ? A mo¿e lepiej we¼ j± w gar¶æ.\n\r", ch );
        return;
    }

    if ( CAN_WEAR( obj, ITEM_HOLD ) && GOT_PART( ch, PART_HANDS ) )
    {
        if ( get_hand_slots( ch, WEAR_HOLD ) >= 2 )
        {
            send_to_char( "Masz ju¿ dwie zajête rêce. Zdejmij co¶.\n\r", ch );
            return;
        }

        if ( !remove_obj( ch, WEAR_HOLD, fReplace ) )
            return;

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_HOLD ) ) return;
            obj->prewear_loc = WEAR_HOLD;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_HOLD, TRUE ) )
            {
                act( "$n bierze $h do rêki.", ch, obj, NULL, TO_ROOM );
                act( "Bierzesz $h do rêki.", ch, obj, NULL, TO_CHAR );

                obj_trap_handler( ch, obj, TRAP_WEAR );

                if ( !ch->in_room )
                    return;

                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    op_common_trigger( ch, obj, &TRIG_WEAR );
            }
        return;
    }

    if ( CAN_WEAR( obj, ITEM_INSTRUMENT ) && GOT_PART( ch, PART_HANDS ) )
    {
        if ( get_hand_slots( ch, WEAR_INSTRUMENT ) != 0 )
        {
            send_to_char( "Masz ju¿ zajête rêce. Zdejmij co¶.\n\r", ch );
            return;
        }

        if ( !remove_obj( ch, WEAR_INSTRUMENT, fReplace ) )
            return;

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_INSTRUMENT ) ) return;
            obj->prewear_loc = WEAR_INSTRUMENT;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_INSTRUMENT, TRUE ) )
            {
                act( "$n bierze $h do rêki.", ch, obj, NULL, TO_ROOM );
                act( "Bierzesz $h do rêki.", ch, obj, NULL, TO_CHAR );

                obj_trap_handler( ch, obj, TRAP_WEAR );

                if ( !ch->in_room )
                    return;

                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    op_common_trigger( ch, obj, &TRIG_WEAR );
            }
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_EAR ) && GOT_PART( ch, PART_EAR ) )
    {

        if ( cover
                && IS_SET( cover->wear_flags, ITEM_WEAR_EAR ) )
        {
            send_to_char( "Najpierw zdejmij swoje okrycie.\n\r", ch );
            return;
        }

        if ( get_eq_char( ch, WEAR_EAR_L ) != NULL
                && get_eq_char( ch, WEAR_EAR_R ) != NULL
                && !remove_obj( ch, WEAR_EAR_L, fReplace )
                && !remove_obj( ch, WEAR_EAR_R, fReplace ) )
            return;

        if ( get_eq_char( ch, WEAR_EAR_L ) == NULL )
        {
            if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
            {
                if ( !can_equip_obj( ch, obj, WEAR_EAR_L ) ) return;
                obj->prewear_loc = WEAR_EAR_L;
                op_common_trigger( ch, obj, &TRIG_PREWEAR );
            }
            else
                if ( equip_char( ch, obj, WEAR_EAR_L, TRUE ) )
                {
                    act( "$n przypina $h do lewego ucha.", ch, obj, NULL, TO_ROOM );
                    act( "Przypinasz $h do lewego ucha.", ch, obj, NULL, TO_CHAR );

                    obj_trap_handler( ch, obj, TRAP_WEAR );

                    if ( !ch->in_room )
                        return;

                    if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                        op_common_trigger( ch, obj, &TRIG_WEAR );


                }
            return;
        }

        if ( get_eq_char( ch, WEAR_EAR_R ) == NULL )
        {
            if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
            {
                if ( !can_equip_obj( ch, obj, WEAR_EAR_R ) ) return;
                obj->prewear_loc = WEAR_EAR_R;
                op_common_trigger( ch, obj, &TRIG_PREWEAR );
            }
            else
                if ( equip_char( ch, obj, WEAR_EAR_R, TRUE ) )
                {
                    act( "$n przypina $h do prawego ucha.", ch, obj, NULL, TO_ROOM );
                    act( "Przypinasz $h do prawego ucha.", ch, obj, NULL, TO_CHAR );

                    obj_trap_handler( ch, obj, TRAP_WEAR );

                    if ( !ch->in_room )
                        return;

                    if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                        op_common_trigger( ch, obj, &TRIG_WEAR );

                }
            return;
        }

        bug( "Wear_obj: no free ear.", 0 );
        send_to_char( "Masz juz dwa kolczyki.\n\r", ch );
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FLOAT ) )
    {
        if ( !remove_obj( ch, WEAR_FLOAT, fReplace ) )
            return;

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_FLOAT ) ) return;
            obj->prewear_loc = WEAR_FLOAT;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_FLOAT, TRUE ) )
            {
                act( "Po chwili $p zaczyna lewitowaæ nad twoj± g³ow±.", ch, obj, NULL, TO_CHAR );
                for(vch=ch->in_room->people;vch;vch=vch_next)
                {
                    vch_next=vch->next_in_room;
                    if ( !IS_NPC( vch )
                            && !EXT_IS_SET( vch->act, PLR_HOLYLIGHT )
                            && !IS_AFFECTED( vch, AFF_DARK_VISION )
                            && !IS_AFFECTED( vch, AFF_BLIND )
                            && vch->position != POS_SLEEPING)
                    {
                        if (vch != ch)
                            act( "Widzisz jak $p zaczyna lewitowaæ nad g³ow± $Z.", vch, obj, ch, TO_CHAR );
                        do_function(vch, &do_look, "auto");
                    }
                }

                obj_trap_handler( ch, obj, TRAP_WEAR );

                if ( !ch->in_room )
                    return;

                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                    op_common_trigger( ch, obj, &TRIG_WEAR );
            }
        return;
    }

    if ( fReplace )
        send_to_char( "Nie mo¿esz tego za³o¿yæ.\n\r", ch );

    return;
}

void wear_obj_silent( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
	OBJ_DATA * cover;

	cover = get_eq_char( ch, WEAR_ABOUT );
	if ( cover && !IS_OBJ_STAT( cover, ITEM_COVER ) ) cover = NULL;

	if ( IS_OBJ_STAT( obj, ITEM_COVER ) )
	{
		if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
			return;

		if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
		{
			if ( !can_equip_obj( ch, obj, WEAR_ABOUT ) )
				return;
			obj->prewear_loc = WEAR_ABOUT;
			op_common_trigger( ch, obj, &TRIG_PREWEAR );
		}
		else
			if ( equip_char( ch, obj, WEAR_ABOUT, TRUE ) )
			{
				obj_trap_handler( ch, obj, TRAP_WEAR );

				if ( !ch->in_room )
				    return;

				if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				    op_common_trigger( ch, obj, &TRIG_WEAR );
			}
		return;
	}

	/*	if( ch->fighting && !CAN_WEAR( obj, ITEM_WEAR_SHIELD))
		{
		send_to_char("Nie masz czasu na to, walcz!\n\r", ch);
		return;
		}*/

	if ( CAN_WEAR( obj, ITEM_WEAR_LIGHT ) )
	{
		if ( get_hand_slots( ch, WEAR_LIGHT ) >= 2 )
			return;

		if ( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
			return;

		if ( equip_char( ch, obj, WEAR_LIGHT, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );
		}
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) && GOT_PART( ch, PART_FINGERS ) )
	{
		if ( get_eq_char( ch, WEAR_FINGER_L ) != NULL
			 && get_eq_char( ch, WEAR_FINGER_R ) != NULL
			 && !remove_obj( ch, WEAR_FINGER_L, fReplace )
			 && !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
			return;

		if ( get_eq_char( ch, WEAR_FINGER_L ) == NULL )
		{
			if ( equip_char( ch, obj, WEAR_FINGER_L, TRUE ) )
			{
				obj_trap_handler( ch, obj, TRAP_WEAR );

				if ( !ch->in_room )
				    return;

				if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				    op_common_trigger( ch, obj, &TRIG_WEAR );
			}
			return;
		}

		if ( get_eq_char( ch, WEAR_FINGER_R ) == NULL )
		{
			if ( equip_char( ch, obj, WEAR_FINGER_R, TRUE ) )
			{
				obj_trap_handler( ch, obj, TRAP_WEAR );

				if ( !ch->in_room )
				    return;

				if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				    op_common_trigger( ch, obj, &TRIG_WEAR );

			}
			return;
		}

		bug( "Wear_obj: no free finger.", 0 );
		send_to_char( "Masz juz dwa pierscienie.\n\r", ch );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) && GOT_PART( ch, PART_HEAD ) )
	{
		if ( get_eq_char( ch, WEAR_NECK_1 ) != NULL
			 && get_eq_char( ch, WEAR_NECK_2 ) != NULL
			 && !remove_obj( ch, WEAR_NECK_1, fReplace )
			 && !remove_obj( ch, WEAR_NECK_2, fReplace ) )
			return;

		if ( get_eq_char( ch, WEAR_NECK_1 ) == NULL )
		{
			if ( equip_char( ch, obj, WEAR_NECK_1, TRUE ) )
			{
				obj_trap_handler( ch, obj, TRAP_WEAR );

				if ( !ch->in_room )
				    return;

				if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				    op_common_trigger( ch, obj, &TRIG_WEAR );

			}
			return;
		}

		if ( get_eq_char( ch, WEAR_NECK_2 ) == NULL )
		{
			if ( equip_char( ch, obj, WEAR_NECK_2, TRUE ) )
			{
				obj_trap_handler( ch, obj, TRAP_WEAR );

				if ( !ch->in_room )
				    return;

				if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				    op_common_trigger( ch, obj, &TRIG_WEAR );

			}
			return;
		}

		bug( "Wear_obj: no free neck.", 0 );
		send_to_char( "Masz juz dwie rzeczy na szyi.\n\r", ch );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
	{
		if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
			return;
		if ( equip_char( ch, obj, WEAR_BODY, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );

		}
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) && GOT_PART( ch, PART_HEAD ) )
	{
		if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
			return;

		if ( equip_char( ch, obj, WEAR_HEAD, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );

		}
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) && GOT_PART( ch, PART_LEGS ) )
	{
		if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
			return;

		if ( equip_char( ch, obj, WEAR_LEGS, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );

		}
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) && GOT_PART( ch, PART_FEET ) )
	{
		if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
			return;

		if ( equip_char( ch, obj, WEAR_FEET, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );

		}
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) && GOT_PART( ch, PART_HANDS ) )
	{
		if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
			return;

		if ( equip_char( ch, obj, WEAR_HANDS, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );

		}
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) && GOT_PART( ch, PART_ARMS ) )
	{
		if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
			return;

		if ( equip_char( ch, obj, WEAR_ARMS, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );

		}
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
	{
		if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
			return;

		if ( equip_char( ch, obj, WEAR_ABOUT, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );

		}
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) && GOT_PART( ch, PART_ARMS ) )
	{
		if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
			return;

		if ( equip_char( ch, obj, WEAR_WAIST, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );

		}
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
	{
		if ( get_eq_char( ch, WEAR_WRIST_L ) != NULL
			 && get_eq_char( ch, WEAR_WRIST_R ) != NULL
			 && !remove_obj( ch, WEAR_WRIST_L, fReplace )
			 && !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
			return;

		if ( get_eq_char( ch, WEAR_WRIST_L ) == NULL )
		{
			if ( equip_char( ch, obj, WEAR_WRIST_L, TRUE ) )
			{
				obj_trap_handler( ch, obj, TRAP_WEAR );

				if ( !ch->in_room )
				    return;

				if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				    op_common_trigger( ch, obj, &TRIG_WEAR );

			}
			return;
		}

		if ( get_eq_char( ch, WEAR_WRIST_R ) == NULL )
		{
			if ( equip_char( ch, obj, WEAR_WRIST_R, TRUE ) )
			{
				obj_trap_handler( ch, obj, TRAP_WEAR );

				if ( !ch->in_room )
				    return;

				if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				    op_common_trigger( ch, obj, &TRIG_WEAR );

			}
			return;
		}

		bug( "Wear_obj: no free wrist.", 0 );
		send_to_char( "Nie masz juz tego gdzie zalozyc.\n\r", ch );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) && GOT_PART( ch, PART_HANDS ) )
	{

		if ( get_hand_slots( ch, WEAR_SHIELD ) >= 2 )
			return;

		if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
			return;

		if ( equip_char( ch, obj, WEAR_SHIELD, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );
		}
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WIELD ) || obj->item_type == ITEM_WEAPON )
		return;

	if ( CAN_WEAR( obj, ITEM_HOLD ) && GOT_PART( ch, PART_HANDS ) )
	{

		if ( get_hand_slots( ch, WEAR_HOLD ) >= 2 )
			return;

		if ( !remove_obj( ch, WEAR_HOLD, fReplace ) )
			return;

		if ( equip_char( ch, obj, WEAR_HOLD, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );

		}
		return;
	}

	if ( CAN_WEAR( obj, ITEM_INSTRUMENT ) && GOT_PART( ch, PART_HANDS ) )
	{

		if ( get_hand_slots( ch, WEAR_INSTRUMENT ) != 0 )
			return;

		if ( !remove_obj( ch, WEAR_INSTRUMENT, fReplace ) )
			return;

		if ( equip_char( ch, obj, WEAR_INSTRUMENT, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );

		}
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_EAR ) && GOT_PART( ch, PART_EAR ) )
	{
		if ( get_eq_char( ch, WEAR_EAR_L ) != NULL
			 && get_eq_char( ch, WEAR_EAR_R ) != NULL
			 && !remove_obj( ch, WEAR_EAR_L, fReplace )
			 && !remove_obj( ch, WEAR_EAR_R, fReplace ) )
			return;

		if ( get_eq_char( ch, WEAR_EAR_L ) == NULL )
		{
			if ( equip_char( ch, obj, WEAR_EAR_L, TRUE ) )
			{
				obj_trap_handler( ch, obj, TRAP_WEAR );

				if ( !ch->in_room )
				    return;

				if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				    op_common_trigger( ch, obj, &TRIG_WEAR );
			}
			return;
		}

		if ( get_eq_char( ch, WEAR_EAR_R ) == NULL )
		{
			if ( equip_char( ch, obj, WEAR_EAR_R, TRUE ) )
			{
				obj_trap_handler( ch, obj, TRAP_WEAR );

				if ( !ch->in_room )
				    return;

				if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				    op_common_trigger( ch, obj, &TRIG_WEAR );

			}
			return;
		}

		bug( "Wear_obj: no free ear.", 0 );
		send_to_char( "Masz juz dwa kolczyki.\n\r", ch );
		return;
	}

	if ( CAN_WEAR( obj, ITEM_WEAR_FLOAT ) )
	{
		if ( !remove_obj( ch, WEAR_FLOAT, fReplace ) )
			return;

		if ( equip_char( ch, obj, WEAR_FLOAT, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );
		}
		return;
	}

	if ( fReplace )
		send_to_char( "Nie mo¿esz tego zalozyc.\n\r", ch );

	return;
}

void do_wear( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Co chcesz za³o¿yæ?\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_RAZORBLADED ) )
    {
        send_to_char( "Przemienione w ostrza d³onie uniemo¿liwiaj± ci wykonanie tej czynno¶ci.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_BEAST_CLAWS ) )
    {
        send_to_char( "Przemienione w szpony d³onie uniemo¿liwiaj± ci wykonanie tej czynno¶ci.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        OBJ_DATA * last_cloak;
        OBJ_DATA * obj_next;

        last_cloak = NULL;

        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );
            if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
            {
                if ( obj->item_type == ITEM_WEAPON )
                {
                    if ( !get_eq_char( ch, WEAR_WIELD ) )
                    {
                        wield_weapon( ch, obj, TRUE );
                    }
                }
                else
                {
                    if ( IS_OBJ_STAT( obj, ITEM_COVER ))
                    {
                        last_cloak = obj;
                    }
                    else
                    {
                        wear_obj( ch, obj, FALSE );
                    }
                }
            }
            if ( !ch->in_room )
            {
                return;
            }
        }
        if ( last_cloak )
        {
            wear_obj( ch, last_cloak, FALSE );
        }
        return;
    }
    else
    {
        if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
        {
            send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
            return;
        }
        wear_obj( ch, obj, TRUE );
    }
    return;
}

void do_remove( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Co chcesz zdj±æ?\n\r", ch );
		return;
	}

	if ( !str_cmp( arg, "all" ) )
	{
		for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
			if ( obj->wear_loc != WEAR_NONE	)
				remove_obj( ch, obj->wear_loc, TRUE );

		return;
	}

	if ( ( obj = get_obj_wear( ch, arg, TRUE ) ) == NULL )
	{
		send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
		return;
	}

	if ( obj->wear_loc == WEAR_WIELD && get_eq_char( ch, WEAR_SECOND ) != NULL )
	{
		send_to_char( "Najpierw zdejmij drug± broñ.\n\r", ch );
		return;
	}

	remove_obj( ch, obj->wear_loc, TRUE );
	return;
}

void do_quaff( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Co chcesz za¿yæ?\n\r", ch );
		return;
	}

	obj = get_obj_carry( ch, arg, ch );

// je¿eli obiekt nieistnieje w inventory, to mo¿e kto¶ co¶ takiego trzyma
// w ³apce? z czego próbuje siê napiæ, warto sprawdziæ.

	if ( obj == NULL ) obj = get_eq_char( ch, WEAR_HOLD );

	if ( obj == NULL )
	{
		send_to_char( "Nie masz takiego eliksiru.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_POTION )
	{
		send_to_char( "Mozesz za¿ywaæ tylko eliksiry.\n\r", ch );
		return;
	}


	if ( !op_consume_trigger( obj, ch ) )
	{
		act( "$n wypija $h.", ch, obj, NULL, TO_ROOM );
		act( "Wypijasz $h.", ch, obj, NULL , TO_CHAR );
	}

	obj_cast_spell( obj->value[ 1 ], obj->value[ 0 ], ch, ch, NULL );
	obj_cast_spell( obj->value[ 2 ], obj->value[ 0 ], ch, ch, NULL );
	obj_cast_spell( obj->value[ 3 ], obj->value[ 0 ], ch, ch, NULL );
	obj_cast_spell( obj->value[ 4 ], obj->value[ 0 ], ch, ch, NULL );

	/*artefact*/
	if ( is_artefact( obj ) ) extract_artefact( obj );
	extract_obj( obj );
	return;
}

void do_recite( CHAR_DATA *ch, char *argument )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;
	OBJ_DATA *scroll;
	OBJ_DATA *obj;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	scroll = get_obj_carry( ch, arg1, ch );

// je¿eli obiekt nieistnieje w inventory, to mo¿e kto¶ co¶ takiego trzyma
// w ³apce? z czego próbuje siê napiæ, warto sprawdziæ.

	if ( scroll == NULL ) scroll = get_eq_char( ch, WEAR_HOLD );

	if ( scroll == NULL )
	{
		send_to_char( "Nie masz takiego zwoju.\n\r", ch );
		return;
	}

	if ( scroll->item_type != ITEM_SCROLL )
	{
		send_to_char( "Mo¿esz recytowaæ tylko zwoje.\n\r", ch );
		return;
	}

	/* dajemy standardowy tekst jak go¶æ jest pod wp³ywem 'silence' */
	if ( IS_AFFECTED( ch, AFF_SILENCE ) )
	{
		send_to_char( AFF_SILENCE_TEXT, ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_BERSERK ) )
	{
		send_to_char( "Lepiej skup siê na walce.\n\r", ch );
		return;
	}


	obj = NULL;
	if ( arg2[ 0 ] == '\0' )
	{
		victim = ch;
	}
	else
	{
		if ( ( victim = get_char_room ( ch, arg2 ) ) == NULL
			 && ( obj = get_obj_here ( ch, arg2 ) ) == NULL )
		{
			send_to_char( "Nie mo¿esz tego znale¼æ.\n\r", ch );
			return;
		}
	}

	act( "$n recytuje $p.", ch, scroll, NULL, TO_ROOM );
	act( "Recytujesz $p.", ch, scroll, NULL, TO_CHAR );

	obj_cast_spell( scroll->value[ 1 ], scroll->value[ 0 ], ch, victim, obj );
	obj_cast_spell( scroll->value[ 2 ], scroll->value[ 0 ], ch, victim, obj );
	obj_cast_spell( scroll->value[ 3 ], scroll->value[ 0 ], ch, victim, obj );
	obj_cast_spell( scroll->value[ 4 ], scroll->value[ 0 ], ch, victim, obj );

	/*artefact*/
	if ( is_artefact( scroll ) )
		extract_artefact( scroll );
	extract_obj( scroll );
	return;
}

void do_brandish( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * vch;
	CHAR_DATA *vch_next;
	OBJ_DATA *staff;
	int sn;

	if ( ( staff = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
	{
		send_to_char( "Nie trzymasz potrzebnej rzeczy w rêce.\n\r", ch );
		return;
	}

	if ( staff->item_type != ITEM_STAFF )
	{
		send_to_char( "To siê nie uda.\n\r", ch );
		return;
	}

	if ( ( sn = staff->value[ 3 ] ) < 0
		 || sn >= MAX_SKILL
		 || skill_table[ sn ].spell_fun == 0 )
	{
		bug( "Do_brandish: bad sn %d.", sn );
		return;
	}

	WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

	if ( staff->value[ 2 ] > 0 )
	{
		act( "$n kreci w powietrzu $j.", ch, staff, NULL, TO_ROOM );
		act( "Krecisz w powietrzu $j.", ch, staff, NULL, TO_CHAR );

		for ( vch = ch->in_room->people; vch; vch = vch_next )
		{
			vch_next	= vch->next_in_room;

			switch ( skill_table[ sn ].target )
			{
				default:
				    bug( "Do_brandish: bad target for sn %d.", sn );
				    return;

				case TAR_IGNORE:
				    if ( vch != ch )
				        continue;
				    break;
				case TAR_CHAR_OFFENSIVE:
//				    if ( IS_NPC( ch ) ? IS_NPC( vch ) : !IS_NPC( vch ) )
				    if (is_same_group( vch, ch ))
				        continue;
				    break;

				case TAR_CHAR_DEFENSIVE:
//				    if ( IS_NPC( ch ) ? !IS_NPC( vch ) : IS_NPC( vch ) )
				    if (!is_same_group( vch, ch ))
				        continue;
				    break;
				case TAR_CHAR_SELF:
				    if ( vch != ch )
				        continue;
				    break;
			}

			obj_cast_spell( staff->value[ 3 ], staff->value[ 0 ], ch, vch, NULL );
		}
	}

	if ( --staff->value[ 2 ] <= 0 )
	{
		act( "$p $c rozb³yska jasnym ¶wiat³em i znika.", ch, staff, NULL, TO_ROOM );
		act( "$p rozb³yska jasnym ¶wiat³em i znika.", ch, staff, NULL, TO_CHAR );
		/*artefact*/
		if ( is_artefact( staff ) ) extract_artefact( staff );
		extract_obj( staff );
	}
	return;
}

void do_zap( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;
	OBJ_DATA *wand;
	OBJ_DATA *obj;

	one_argument( argument, arg );
	if ( arg[ 0 ] == '\0' && ch->fighting == NULL )
	{
		send_to_char( "Z czego czy na kogo chcesz wyzwoliæ energiê?\n\r", ch );
		return;
	}

	if ( ( wand = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
	{
		send_to_char( "Nie trzymasz potrzebnej rzeczy w recê.\n\r", ch );
		return;
	}

	if ( wand->item_type != ITEM_STAFF )
	{
		send_to_char( "Energie mozna wyzwoliæ tylko z ro¿d¿ek.\n\r", ch );
		return;
	}

	obj = NULL;
	if ( arg[ 0 ] == '\0' )
	{
		if ( ch->fighting != NULL )
		{
			victim = ch->fighting;
		}
		else
		{
			send_to_char( "Z czego czy na kogo chcesz wyzwoliæ energiê?\n\r", ch );
			return;
		}
	}
	else
	{
		if ( ( victim = get_char_room ( ch, arg ) ) == NULL
			 && ( obj = get_obj_here ( ch, arg ) ) == NULL )
		{
			send_to_char( "Nie mo¿esz tego znale¼æ.\n\r", ch );
			return;
		}

		if ( obj && obj == wand )
		{
			send_to_char( "Nie mo¿esz tego znale¼æ.\n\r", ch );
			return;
		}

	}

	WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

	if ( wand->value[ 2 ] > 0 )
	{
		if ( victim != NULL )
		{
			act( "$n wyzwala energie z $f w kierunku $Z.", ch, wand, victim, TO_NOTVICT );
			act( "Wyzwalasz energie z $f w kierunku $Z.", ch, wand, victim, TO_CHAR );
			act( "$n wyzwala energie z $f w twoim kierunku.", ch, wand, victim, TO_VICT );
		}
		else
		{
			act( "$n wyzwala energie z $f w kierunku $F.", ch, wand, obj, TO_ROOM );
			act( "Wyzwalasz energie z $f w kierunku $F.", ch, wand, obj, TO_CHAR );
		}

		obj_cast_spell( wand->value[ 3 ], wand->value[ 0 ], ch, victim, obj );
	}

	if ( --wand->value[ 2 ] <= 0 )
	{
		if( wand->liczba_mnoga )
		{
			act( "$p $z rozpadaj± siê na kawa³ki.", ch, wand, NULL, TO_ROOM );
			act( "$p rozpadaj± siê na kawa³ki.", ch, wand, NULL, TO_CHAR );
		}
		else
		{
			act( "$p $z rozpada siê na kawa³ki.", ch, wand, NULL, TO_ROOM );
			act( "$p rozpada siê na kawa³ki.", ch, wand, NULL, TO_CHAR );
		}

		/*artefact*/
		if ( is_artefact( wand ) ) extract_artefact( wand );
		extract_obj( wand );
	}

	return;
}

void do_steal( CHAR_DATA *ch, char *argument )
{
	char buf [ MAX_STRING_LENGTH ];
	char arg1 [ MAX_INPUT_LENGTH ];
	char arg2 [ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	int chance, victim_skill, iWear, nomination;

	if ( ( chance = get_skill( ch, gsn_steal ) ) <= 0 )
	{
		print_char( ch, "Nie masz pojêcia jak siê do tego zabraæ.\n\r" );
		return;
	}

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
	{
		send_to_char( "Ukra¶æ co¶, ale komu?\n\r", ch );
		act( "$n krêci siê niespokojnie.", ch, NULL, NULL, TO_ROOM );
		return;
	}

	if ( ( victim = get_char_room( ch, arg2 ) ) == NULL )
	{
		send_to_char( "Nie ma tej osoby tutaj.\n\r", ch );
		act( "$n rozgl±da siê, szukaj±c kogo¶.", ch, NULL, NULL, TO_ROOM );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Wk³adasz sobie rekê do kieszeni... hmm, pusto?\n\r", ch );
		act( "$n robi co¶ dziwnego ze swoimi rêkami.", ch, NULL, NULL, TO_ROOM );
		return;
	}

	if (ch->class == CLASS_BARD )
	{
		for ( iWear = 0; iWear < MAX_WEAR - 1; iWear++ )
		{
			if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
				continue;

			if ( obj->item_type != ITEM_ARMOR )
				continue;

			if ( iWear != WEAR_HEAD && iWear != WEAR_BODY &&
				 iWear != WEAR_LEGS && iWear != WEAR_FEET &&
				 iWear != WEAR_HANDS && iWear != WEAR_ARMS )
				 continue;

			if ( obj->value[4] != 0 && obj->value[4] != 1 && obj->value[4] != 6 )
			{
				send_to_char( "Twoja zbroja zbyt ciê uwiera, by¶ by<&³/³a/³o> w stanie kogokolwiek okra¶æ.\n\r", ch );
				return;
			}
		}
	}

	if ( is_safe( ch, victim ) || ( !IS_NPC( victim ) && IS_IMMORTAL( victim ) ) )
	{
		print_char( ch, "Nie mo¿esz okra¶æ %s.\n\r", victim->name2 );
		return;
	}

	if ( victim->fighting )
	{
		send_to_char( "Uwa¿aj, bo mo¿e ci siê oberwaæ.\n\r", ch );
		return;
	}

	if (ch->move < 0)
    {
		ch->move = 0;
    }

	if (ch->move == 0)
	{
		send_to_char( "Nie masz na to si³.\n\r", ch );
		return;
	}

	WAIT_STATE( ch, skill_table[ gsn_steal ].beats );

	//skill ofiary
	victim_skill = get_skill( victim, gsn_steal );
	chance = URANGE( 0, chance - victim_skill, 95 );
	/* polowa skilla */
	chance = chance / 2;

	/* plus dex */
	//chance += (get_curr_stat_deprecated( ch, STAT_DEX ) - (MAX_STAT_VALUE * 5)/8)*5;
	chance += (get_curr_stat( ch, STAT_DEX ) - (MAX_STAT_VALUE * 2)/5)*4/5;	//nowy

	if( victim->level - ch->level > 12 )
    {
		chance -= ( victim->level - ch->level ) * 2;
    }

	chance = UMAX(1, chance);
	ch->move = UMAX( 0, ch->move - 2);

    if ( IS_AFFECTED( ch, AFF_HIDE ))
    {
        chance += 15;
    }
    else if ( IS_AFFECTED( ch, AFF_SNEAK ))
    {
        chance += 5;
    }

    if ( !IS_AWAKE( victim ) )
    {
        chance += 20;
    }
    else if ( !can_see( victim, ch ) )
    {
        chance += 5;
    }

    if (ch->class == CLASS_BARD )
    {
        chance -= chance/4;
    }

// obni¿amy szansê na okradzenie, je¿eli cel jest pod wp³ywem zaklêcia
// mirror image
	if ( IS_AFFECTED( victim, AFF_MIRROR_IMAGE ) )
	{
		if (number_percent() > 20)
		{
			chance -= 50;
		}
	}

	if ( !IS_NPC( victim ) )
	{
		sprintf(buf,"%s tried to steal from %s.",ch-> name, victim->name);
		wiznet( buf, ch, NULL, WIZ_FLAGS, 0, 0 );
	}

    if ( number_percent() > chance )
    {
        if ( IS_NPC( victim ) && victim->position == POS_SLEEPING )
        {
            do_function(victim, &do_stand, "");
            return;
        }
            /*
             * Failure.
             */
        if ( can_see( victim, ch ) )
        {
            send_to_char( "Oops, zauwa¿ono ciê.\n\r", ch );
        }
        affect_strip( ch, gsn_sneak );
        // zdejmujemy invisa, z komunikatami
        strip_invis( ch, TRUE, TRUE );
        EXT_REMOVE_BIT( ch->affected_by, AFF_HIDE );

        check_improve( ch, victim, gsn_steal, FALSE, 15 );

        switch ( ch->sex)
        {
            case  0:
                act( "$n próbowa³o ciê okra¶æ.", ch, NULL, victim, TO_VICT );
                act( "$n próbowa³o co¶ ukra¶æ $X.", ch, NULL, victim, TO_NOTVICT );
                break;
            case  1:
                act( "$n próbowa³ ciê okra¶æ.", ch, NULL, victim, TO_VICT );
                act( "$n próbowa³ co¶ ukra¶æ $X.", ch, NULL, victim, TO_NOTVICT );
                break;
            default:
                act( "$n próbowa³a ciê okra¶æ.", ch, NULL, victim, TO_VICT );
                act( "$n próbowa³a co¶ ukra¶æ $X.", ch, NULL, victim, TO_NOTVICT );
                break;
        }

        if ( IS_NPC( victim ) )
        {
            if(victim->position != POS_SLEEPING)
            {
                do_function(victim, &do_stand, "");
                multi_hit( victim, ch, TYPE_UNDEFINED );
            }
        }
        return;
    }

    nomination = money_nomination_find ( arg1 );
    if ( nomination > NOMINATION_NONE )
	{
		long int amount = 0;
        int multiplier = RATTING_NONE;

        switch ( nomination )
        {
            case NOMINATION_COPPER:
                amount = victim->copper * number_range( 1, 2 * ch->level ) / MAX_LEVEL;
                ch->copper += amount;
                victim->copper -= amount;
                multiplier = RATTING_COPPER;
                break;
            case NOMINATION_SILVER:
                amount = victim->silver * number_range( 1, ch->level / 2 ) / MAX_LEVEL;
                ch->silver += amount;
                victim->silver -= amount;
                multiplier = RATTING_SILVER;
                break;
            case NOMINATION_GOLD:
                amount = victim->gold    * number_range( 1, ch->level / 4 ) / MAX_LEVEL;
                ch->gold += amount;
                victim->gold -= amount;
                multiplier = RATTING_GOLD;
                break;
            case NOMINATION_MITHRIL:
                amount = victim->mithril * number_range( 1, ch->level / 8 ) / ( 4 * MAX_LEVEL );
                ch->mithril += amount;
                victim->mithril -= amount;
                multiplier = RATTING_MITHRIL;
                break;
        }

        if ( amount < 1 )
        {
            send_to_char( "Nie uda³o ci sie ukra¶æ ¿adnych monet.\n\r", ch );
            return;
        }

        append_file_format_daily
            (
             ch,
             MONEY_LOG_FILE,
             "-> S: %ld %ld (%ld), %s - ukrad³ kasê postaci %s",
             money_count_copper( ch ) - multiplier * amount,
             money_count_copper( ch ),
             amount,
             buf,
             IS_NPC( victim ) ? victim->short_descr : victim->name
            );


        append_file_format_daily
            (
             victim,
             MONEY_LOG_FILE,
             "-> S: %ld %ld (%ld), %s - okradziony przez postaæ %s",
             money_count_copper( victim ) + multiplier * amount,
             money_count_copper( victim ),
             -amount,
             buf,
             IS_NPC( ch ) ? ch->short_descr : ch->name
            );


        sprintf( buf, "Ukrad³<&e/a/o>¶ %s.\n\r", money_string_simple( amount, nomination, TRUE ) );

		send_to_char( buf, ch );
		check_improve( ch, victim, gsn_steal, TRUE, 3 );
		return;
	}

	if ( ( obj = get_obj_carry( victim, arg1, ch ) ) == NULL )
	{
		send_to_char( "Nie mo¿esz tego znale¼æ.\n\r", ch );
		return;
	}

	if ( !can_drop_obj( ch, obj )
		 || IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
	{
		send_to_char( "Nie mo¿esz tego wyciagn±æ.\n\r", ch );
		return;
	}

	if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
	{
		send_to_char( "Masz ju¿ pe³ne rêce.\n\r", ch );
		return;
	}

	if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
	{
		send_to_char( "Nie uniesiesz tego.\n\r", ch );
		return;
	}

	/*artefact*/
	if ( is_artefact( obj ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
		artefact_from_char( obj, victim );
	obj_from_char( obj );
	/*artefact*/
	if ( is_artefact( obj ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
		artefact_to_char( obj, ch );
	obj_to_char( obj, ch );
	act( "Ma siê te lepkie paluszki! Kradniesz $h.", ch, obj, NULL, TO_CHAR );
	check_improve( ch, victim, gsn_steal, TRUE, 15 );
//	send_to_char( "Super!\n\r", ch );
	return;
}


/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper( CHAR_DATA *ch, char *name )
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA * keeper;
	SHOP_DATA *pShop;

	pShop = NULL;
	if ( IS_NULLSTR( name ) )
	{
		for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
		{
			if ( IS_NPC( keeper ) && ( pShop = keeper->pIndexData->pShop ) != NULL )
				break;
		}
	}
	else
	{
		for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
		{
			if ( IS_NPC( keeper ) && is_name( name, keeper->name ) && ( pShop = keeper->pIndexData->pShop ) != NULL )
				break;
		}
	}

	if ( pShop == NULL )
	{
		if ( IS_NULLSTR( name ) )
			send_to_char( "Tu raczej nie pohandlujesz.\n\r", ch );
		else
			send_to_char( "Nie ma tutaj takiego sklepikarza.\n\r", ch );
		return NULL;
	}
	/*
	 * Invisible or hidden people.
	 */
	if ( !can_see( keeper, ch ) )
	{
		do_function( keeper, &do_say, "Kto to powiedzia³? Co tu siê dzieje?" );
		return NULL;
	}
	/*
	 * Czy stoi w odpowiedniej lokacji.
	 */
	if ( pShop->shop_room != 0 && pShop->shop_room != keeper->in_room->vnum )
	{
		do_function( keeper, &do_say, "Przykro mi, ale tutaj nie handlujê." );
		return NULL;
	}
	/*
	 * Shop hours.
	 */
	if ( time_info.hour < pShop->open_hour )
	{
		sprintf( buf, "Przykro mi, ale jeszcze nieczynne. Przyjd¼ o godzinie %d.", pShop->open_hour );
		do_function( keeper, &do_say, buf );
		return NULL;
	}
	if ( time_info.hour > pShop->close_hour )
	{
		sprintf( buf, "Przykro mi, ale ju¿ nieczynne. Przyjd¼ o godzinie %d.", pShop->open_hour );
		do_function( keeper, &do_say, buf );
		return NULL;
	}
	return keeper;
}

/* insert an object at the right spot for the keeper */
void obj_to_keeper( OBJ_DATA *obj, CHAR_DATA *ch )
{
	OBJ_DATA *t_obj = NULL;
	OBJ_DATA *t_obj_next = NULL;
	OBJ_DATA *prev_obj = NULL;

	/* see if any duplicates are found */
	for ( t_obj = ch->carrying; t_obj != NULL; t_obj = t_obj_next )
	{
		prev_obj = t_obj;
		OBJ_NEXT_CONTENT( t_obj, t_obj_next );

		if ( obj->pIndexData == t_obj->pIndexData
			 && !str_cmp( obj->short_descr, t_obj->short_descr ) )
		{
			/* if this is an unlimited item, destroy the new one */
			if ( IS_OBJ_STAT( t_obj, ITEM_INVENTORY ) )
			{
				/*artefact*/
				if ( is_artefact( obj ) ) extract_artefact( obj );
				if ( obj->contains ) extract_artefact_container( obj );
				extract_obj( obj );
				return;
			}
			obj->cost = t_obj->cost; /* keep it standard */
			break;
		}
	}

	if ( t_obj == NULL )
	{
		//dodajemy na koniec listy
		if ( prev_obj )
		{
			obj->next_content = NULL;
			prev_obj->next_content = obj;
		}
		else
		{
			obj->next_content = ch->carrying;
			ch->carrying = obj;
		}
	}
	else
	{
		obj->next_content = t_obj->next_content;
		t_obj->next_content = obj;
	}

	obj->carried_by = ch;
	obj->in_room = NULL;
	obj->in_obj = NULL;
	ch->carry_number += get_obj_number( obj );
	ch->carry_weight += get_obj_weight( obj );
}

/* get an object from a shopkeeper's list */
OBJ_DATA *get_obj_keeper( CHAR_DATA *ch, CHAR_DATA *keeper, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	int number;
	int count;
	bool TEST = FALSE;


	number = number_argument( argument, arg );

	count = 1;

	if ( is_number( arg ) )
	{
		TEST = TRUE;
		number = atoi( arg );
	}

	for ( obj = keeper->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE
			 && can_see_obj( keeper, obj )
			 && can_see_obj( ch, obj )
			 && get_cost( keeper, obj, TRUE ) > 0
			 && ( is_name( arg, obj->name ) || TEST ) )
		{
			if ( count++ == number )
				return obj;

			/* skip other objects of the same name */
			while ( obj->next_content != NULL
				    && obj->pIndexData == obj->next_content->pIndexData
				    && !str_cmp( obj->short_descr, obj->next_content->short_descr ) )
				obj = obj->next_content;
		}
	}

	return NULL;
}

int get_cost( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
	SHOP_DATA * pShop;
	int cost;

	if ( obj == NULL || ( pShop = keeper->pIndexData->pShop ) == NULL )
		return -1;

	if ( fBuy )
	{
		cost = obj->cost * pShop->profit_buy / 100;
	}
	else
	{
		OBJ_DATA *obj2;
		int itype;

		cost = 0;
		for ( itype = 0; itype < MAX_TRADE; itype++ )
		{
			if ( obj->item_type == pShop->buy_type[ itype ] )
			{
				cost = obj->cost * pShop->profit_sell / 100;
				cost = ( cost * obj->condition ) / 100;

				break;
			}
		}

		if ( !IS_OBJ_STAT( obj, ITEM_SELL_EXTRACT ) )
			for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
			{
				if ( obj->pIndexData == obj2->pIndexData && !str_cmp( obj->short_descr, obj2->short_descr ) )
				{
				    if ( IS_OBJ_STAT( obj2, ITEM_INVENTORY ) )
				        cost /= 2;
				    else
				        cost = cost * 3 / 4;
				}
			}
	}

	return UMAX( 0, cost );
}

void do_buy( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * keeper;
	OBJ_DATA *obj, *t_obj;
	char arg[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char arg_keeper[ MAX_INPUT_LENGTH ];
	int number, count = 1;
	char buf[ MAX_STRING_LENGTH ];
	int cost, trade, charisma, bonus = 0, vwis, luck;
	char *arg_rest;

    if ( ch->fighting )
    {
      send_to_char( "Lepiej skup sie na walce!\n\r", ch );
      return;
    }

	arg_keeper[0] = '\0';
	arg[0] = '\0';

	arg_rest = one_argument( argument, arg2 );

	if ( arg_rest[0] != '\0' )
	{
	   number = mult_argument( arg2, arg );
				one_argument( arg_rest, arg_keeper );
	}
	else
	   number = mult_argument( arg2, arg );

	if ( IS_AFFECTED( ch, AFF_SILENCE ) )
	{
		send_to_char( AFF_SILENCE_TEXT, ch );
		return;
	}

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Co chcesz kupic?\n\r", ch );
		return;
	}

	if ( ch->precommand_fun && !ch->precommand_pending )
	{
		send_to_char( "Robisz teraz co¶ innego.\n\r", ch );
		return;
	}

	if ( ( keeper = find_keeper( ch, arg_keeper ) ) == NULL )
		return;

	if ( keeper->position != keeper->default_pos )
		return;

	if ( get_mob_memdat( keeper, NULL, NULL, MEMDAT_ANY ) && HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
	{
		if ( keeper->sex == 2 )
			act( "{k$n mówi ci '{KJestem teraz zajêta. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
		else
			act( "{k$n mówi ci '{KJestem teraz zajêty. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
		return;
	}

	if ( ( ch->speaking != 19 && ch->speaking != 0 && UMIN( knows_language( keeper, ch->speaking, ch ),
			knows_language( ch, ch->speaking, keeper ) ) < 50 ) ||
		 ( ch->speaking == 0 && UMIN( knows_language( keeper, ch->speaking, ch ),
				                      knows_language( ch, ch->speaking, keeper ) ) < 35 ) )
	{
		print_char( ch, "%s chyba nie rozumie tego co mówisz.\n\r", capitalize( keeper->short_descr ) );
		return;
	}

	obj = get_obj_keeper( ch, keeper, arg );
	cost = get_cost( keeper, obj, TRUE );

	if ( !cost || !can_see_obj( ch, obj ) )
	{
		act( "{k$n mówi ci '{KNiczego takiego nie mam na sprzeda¿.{k'{x", keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}

	if ( !ch->precommand_pending && HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
	{
		if ( mp_precommand_trigger( keeper, ch, obj, &do_buy, "buy", argument ) )
			return;
	}

	if ( ( trade = get_skill( ch, gsn_trade ) ) > 0 )
    {
        charisma = get_curr_stat_deprecated( ch, STAT_CHA );
        vwis = get_curr_stat_deprecated( keeper, STAT_WIS );
        luck = get_curr_stat_deprecated( ch, STAT_LUC );
		if ((ch->sex == 1 && keeper->sex == 2) || (ch->sex == 2	&& keeper->sex != 2))
			bonus = 20;

		if ( number_range(0,luck+charisma+trade/2) < 10)
		{
			cost += ((100 - trade)*cost)/800;
			send_to_char( "Twoje starania o wytargowanie jak najni¿szej ceny bardzo denerwuj± sklepikarza.\n\r", ch );
			check_improve(ch, NULL, gsn_trade, TRUE, 80 );
		}
		else if ( (charisma-10)*2 + trade + luck/3 + bonus < number_range(1,100) + vwis*2 )
		{
			send_to_char( "Starasz siê wytargowaæ jak najni¿sz± cenê, jednak sprzedawca pozostaje nieugiêty.\n\r", ch );
			check_improve(ch, NULL, gsn_trade, TRUE, 85 );
		}
		else
		{
			cost -= ((trade + (charisma/2))*cost)/400;
			send_to_char( "D³ugo k³ócisz siê ze sklepikarzem, w koñcu udaje ci siê wytargowaæ najkorzystniejsz± cenê.\n\r", ch );
			check_improve(ch, NULL, gsn_trade, TRUE, 70);
		}
	}

	if ( number < 1 || number > 99 )
	{
		switch ( ch->sex )
		{
			case 0:
				act( "{k$n mówi ci '{KEeee... Pi³o¶ co¶?!{k'{x", keeper, NULL, ch, TO_VICT );
				break;
			case 1:
				act( "{k$n mówi ci '{KStary, pi³e¶ co¶?!{k'{x", keeper, NULL, ch, TO_VICT );
				break;
			default:
				act( "{k$n mówi ci '{KPrzepraszam, ale nie wiem czego chcesz.{k'{x", keeper, NULL, ch, TO_VICT );
				break;
		}
		return;
	}

	if ( !IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
	{
		for ( t_obj = obj->next_content;
			  count < number && t_obj != NULL;
			  t_obj = t_obj->next_content )
		{
			if ( t_obj->pIndexData == obj->pIndexData
				 && !str_cmp( t_obj->short_descr, obj->short_descr ) )
				count++;
			else
				break;
		}

		if ( count < number )
		{
			act( "{k$n mówi ci '{KNie mam tyle sztuk.{k'{x",
				 keeper, NULL, ch, TO_VICT );
			ch->reply = keeper;
			return;
		}
	}

	if ( money_count_copper( ch ) < cost * number )
	{
		if ( number > 1 )
        {
			act( "{k$n mówi ci '{KNie staæ ciê na tyle sztuk.{k'{x", keeper, obj, ch, TO_VICT );
        }
		else
        {
			act( "{k$n mówi ci '{KNie staæ ciê na kupienie $f.{k'{x", keeper, obj, ch, TO_VICT );
        }
		ch->reply = keeper;
		return;
	}

	if ( ch->carry_number + number * get_obj_number( obj ) > can_carry_n( ch ) )
	{
		send_to_char( "Nie mo¿esz nosiæ wiecej rzeczy.\n\r", ch );
		return;
	}

	if ( ch->carry_weight + number * get_obj_weight( obj ) > can_carry_w( ch ) )
	{
		send_to_char( "Nie uniesiesz juz wiecej.\n\r", ch );
		return;
	}

	if ( number > 1 )
	{
		sprintf( buf, "$n kupuje $h w ilo¶ci %d sztuk.", number );
		act( buf, ch, obj, NULL, TO_ROOM );
		sprintf( buf, "Kupujesz $h w ilo¶ci %d sztuk za %s.", number, money_string ( cost * number, TRUE ) );
		act( buf, ch, obj, NULL, TO_CHAR );
	}
	else
	{
		act( "$n kupuje $h.", ch, obj, NULL, TO_ROOM );
		sprintf( buf, "Kupujesz $h za %s.", money_string( cost, TRUE ) );
		act( buf, ch, obj, NULL, TO_CHAR );
	}

    append_file_format_daily
        (
         ch,
         MONEY_LOG_FILE,
         "-> S: %ld %ld (%ld) - kupi³ obiekt [%5d] od moba [%5d]",
         money_count_copper( ch ),
         money_count_copper( ch ) - ( cost * number ),
         -( cost * number ),
         obj->pIndexData->vnum,
         keeper->pIndexData->vnum
        );

    money_reduce( ch, cost * number );
    money_gain( keeper, cost * number );

	for ( count = 0; count < number; count++ )
	{
		if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
        {
			t_obj = create_object( obj->pIndexData, FALSE );
        }
		else
		{
			t_obj = obj;
			obj = obj->next_content;
			obj_from_char( t_obj );
		}

		if ( t_obj->timer > 0 && !IS_OBJ_STAT( t_obj, ITEM_HAD_TIMER ) )
        {
			t_obj->timer = 0;
        }
		EXT_REMOVE_BIT( t_obj->extra_flags, ITEM_HAD_TIMER );

		EXT_REMOVE_BIT( t_obj->extra_flags, ITEM_INVENTORY );
		/*artefact*/
		if ( is_artefact( t_obj ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
        {
			artefact_to_char( t_obj, ch );
        }
		if ( cost < t_obj->cost )
        {
			t_obj->cost = cost;
        }
        t_obj->rent_cost = RENT_COST( t_obj->cost );
		obj_to_char( t_obj, ch );
	}
}

/* repair stuff */
REPAIR_DATA * get_repair_data( REPAIR_DATA *list, OBJ_DATA *obj )
{
    ush_int	item_type = -1, item_class = -1, item_group = -1;
    REPAIR_DATA *tmp = NULL;

    if( !list || !obj )
        return NULL;

    //wyliczamy typy itema
    switch( obj->item_type )
    {
	    case ITEM_WEAPON:
			item_class = REP_ITEM_WEAPON;
			switch( obj->value[0] )
			{
			    case WEAPON_SWORD:
			        item_group  = REP_ITEM_SWORDS;
                    item_type   = REP_ITEM_LSWORD;
                    break;
			    case WEAPON_SHORTSWORD:
			        item_group  = REP_ITEM_SWORDS;
                    item_type   = REP_ITEM_SHSWORD;
                    break;
			    case WEAPON_MACE:
                    item_group  = REP_ITEM_BLUDGEONS;
                    item_type   = REP_ITEM_MACE;
                    break;
			    case WEAPON_FLAIL:
			        item_group  = REP_ITEM_BLUDGEONS;
                    item_type   = REP_ITEM_FLAIL;
                    break;
			    case WEAPON_POLEARM:
			        item_group  = REP_ITEM_BLUDGEONS;
                    item_type   = REP_ITEM_POLEARM;
                    break;
			    case WEAPON_STAFF:
                    item_group  = REP_ITEM_BLUDGEONS;
			        item_type   = REP_ITEM_STAFF;
                    break;
			    case WEAPON_DAGGER:
                    item_type   = REP_ITEM_DAGGER;
                    break;
			    case WEAPON_SPEAR:
			        item_type   = REP_ITEM_SPEAR;
			        break;
			    case WEAPON_AXE:
			        item_type   = REP_ITEM_AXE;
			        break;
			    case WEAPON_WHIP:
			        item_type   = REP_ITEM_WHIP;
			        break;
			    case WEAPON_CLAWS:
			    		item_type   = REP_ITEM_CLAWS;
			}
			break;
		case ITEM_ARMOR:
			item_class = REP_ITEM_ARMOR;
			switch( obj->value[4])
			{
			    case 0:
			    case 1:
			    case 2:
			    case 3:
				    item_type = REP_ITEM_ARMOR_LIGHT;
				    break;
			    case 4:
			    case 5:
			    case 6:
			    case 7:
				    item_type = REP_ITEM_ARMOR_MEDIUM;
				    break;
			    case 8:
			    case 9:
			    case 10:
			    case 11:
				    item_type = REP_ITEM_ARMOR_HEAVY;
				    break;
			    case 12:
			    case 13:
			    case 14:
			    case 15:
				    item_type = REP_ITEM_ARMOR_VHEAVY;
				    break;
			}
			break;
		case ITEM_CLOTHING:
			item_class = REP_ITEM_CLOTHING;
			break;
		case ITEM_JEWELRY:
			item_class = REP_ITEM_JEWELRY;
			break;
		case ITEM_GEM:
			item_class = REP_ITEM_GEM;
			break;
		case ITEM_MUSICAL_INSTRUMENT:
			item_class = REP_ITEM_MUSICAL_INSTRUMENT;
			break;
		case ITEM_SHIELD:
			item_class = REP_ITEM_SHIELD;
			break;
		case ITEM_TOOL:
		    item_class = REP_ITEM_TOOL;
		    break;
		default: return NULL;
    }

    if( !item_class && !item_type && !item_group )
        return NULL;

    for( tmp = list; tmp; tmp = tmp->next )
    {
        /* repair certain type - ie. weapons */
        if( tmp->item_type == item_type )
            return tmp;

        /* repair certain group - ie. swords */
        if( tmp->item_type == item_group )
            return tmp;

        /* repair certain class - ie. short shords */
        if( tmp->item_type == item_class )
            return tmp;
    }

    return NULL;
}

CHAR_DATA *find_repair( CHAR_DATA *ch, char *name )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA * tch, *keeper = NULL;
    REPAIR_DATA *pRepair = NULL;

    if ( IS_NULLSTR( name ) )
    {
        for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
        {
            if ( IS_NPC( tch ) && ( pRepair = tch->pIndexData->pRepair ) != NULL )
            {
                keeper = tch;
                break;
            }
        }
    }
    else
    {
        for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
        {
            if ( IS_NPC( tch ) && is_name( name, tch->name ) && ( pRepair = tch->pIndexData->pRepair ) != NULL )
            {
                keeper = tch;
                break;
            }
        }
    }

    if ( !keeper )
    {
        if ( IS_NULLSTR( name ) )
            send_to_char( "Tu raczej nic nie naprawisz.\n\r", ch );
        else
            send_to_char( "Nie ma tutaj nikogo takiego.\n\r", ch );

        return NULL;
    }

    if ( !can_see( keeper, ch ) )
    {
        do_function( keeper, &do_say, "Kto to powiedzial? Co tu sie dzieje?" );
        return NULL;
    }

    /*
     * Shop hours.
     */

    if ( pRepair->repair_room != 0 && keeper->in_room->vnum != pRepair->repair_room )
    {
        do_function( keeper, &do_say, "Przykro mi, ale tutaj nic nie naprawiam." );
        return NULL;
    }

    if ( time_info.hour < pRepair->repair_open_hour )
    {
        sprintf( buf, "Przykro mi, ale jeszcze nieczynne. Przyjd¼ o godzinie %d.", pRepair->repair_open_hour );
        do_function(keeper, &do_say, buf );
        return NULL;
    }

    if ( time_info.hour > pRepair->repair_close_hour )
    {
        sprintf( buf, "Przykro mi, ale ju¿ nieczynne. Przyjd¼ o godzinie %d.", pRepair->repair_open_hour );
        do_function(keeper, &do_say, buf );
        return NULL;
    }

    return keeper;
}

void do_estimate( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg_keeper[MAX_INPUT_LENGTH];
    char *arg_rest;
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    char buf[MAX_INPUT_LENGTH];
    int cost;
    REPAIR_DATA *pRep = NULL;


    arg_keeper[0] = '\0';
    arg[0] = '\0';

    arg_rest = one_argument( argument, arg );

    if ( arg_rest[0] != '\0' )
    {
        arg_rest = one_argument( argument, arg );
        one_argument( arg_rest, arg_keeper );
    }

    if (IS_AFFECTED(ch, AFF_SILENCE))
    {
        send_to_char(AFF_SILENCE_TEXT, ch);
        return;
    }

    if ( arg[0] == '\0' )
    {
        send_to_char( "Koszt naprawy czego chcesz oceniæ?\n\r", ch );
        return;
    }

    if ( ch->precommand_fun && !ch->precommand_pending )
    {
        send_to_char( "Robisz teraz co¶ innego.\n\r", ch );
        return;
    }

    if ( ( keeper = find_repair( ch, arg_keeper ) ) == NULL )
        return;

    if ( keeper->position != keeper->default_pos )
        return;

    if ( get_mob_memdat( keeper, NULL, NULL, MEMDAT_ANY ) && HAS_TRIGGER( keeper, TRIG_PRECOMMAND) )
    {
        if ( keeper->sex == 2 )
            act("{k$n mówi ci '{KJestem teraz zajêta. Proszê poczekaæ chwilkê.{k'{x",keeper,NULL,ch,TO_VICT);
        else
            act("{k$n mówi ci '{KJestem teraz zajêty. Proszê poczekaæ chwilkê.{k'{x",keeper,NULL,ch,TO_VICT);

        return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        act( "{k$n mówi ci '{KNie masz takiej rzeczy{k'{x", keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
        send_to_char( "Nie mo¿esz tego wypu¶ciæ.\n\r", ch );
        return;
    }

    if ( !can_see_obj(keeper,obj) )
    {
        act( "{k$n mówi ci '{KNic takiego nie widzê{k'{x", keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return;
    }

    if ( obj->item_type == ITEM_GEM || obj->item_type == ITEM_SPELLBOOK || EXT_IS_SET( obj->extra_flags, ITEM_NOREPAIR ) )
    {
        act( "{k$n mówi ci '{KNie mogê wyceniæ, poniewa¿ tego siê nie da naprawiæ.{k'{x", keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return;
    }

    /**
     * ITEM_NOREPAIR
     */
    if ( EXT_IS_SET( obj->extra_flags, ITEM_NOREPAIR ) )
    {
        act( "{k$n mówi ci '{KA to dobre... Wiesz, tego siê nie da naprawiæ.{k'{x", keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return ;
    }

    /**
     * repair limits
     */
    if ( obj->repair_counter > obj->repair_limit )
    {
        act( "{k$n mówi ci '{KTego nie da siê ju¿ wiêcej naprawiæ.{k'{x", keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return ;
    }
    if ( obj->condition > obj->repair_condition )
    {
        act( "{k$n mówi ci '{KTego nie da siê ju¿ bardziej naprawiaæ.{k'{x", keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return ;
    }

    if ( !ch->precommand_pending && HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
    {
        if ( mp_precommand_trigger( keeper, ch, obj, &do_estimate, "estimate", argument ) )
            return ;
    }

    if ( ( pRep = get_repair_data( keeper->pIndexData->pRepair, obj ) ) == NULL )
    {
        switch ( obj->item_type )
        {
            case ITEM_ARMOR:
            case ITEM_WEAPON:
                act( "{k$n mówi ci '{KZwróæ siê z tym do kogo¶ innego, ja ci nie pomogê.{k'{x", keeper, NULL, ch, TO_VICT );
                break;
            case ITEM_CLOTHING:
                act( "{k$n mówi ci '{KZwróæ siê z tym do krawca, ja ci nie pomogê.{k'{x", keeper, NULL, ch, TO_VICT );
                break;
            case ITEM_GEM:
                act( "{k$n mówi ci '{KJa siê na tym nie znam, id¼ do jubilera.{k'{x", keeper, NULL, ch, TO_VICT );
                break;
            case ITEM_MUSICAL_INSTRUMENT:
                act( "{k$n mówi ci '{KZwróæ siê z tym lepiej do jakiego¶ muzyka, ja ci nie pomogê.{k'{x", keeper, NULL, ch, TO_VICT );
                break;
            default:
                act( "{k$n mówi ci '{KNie wiem co ty chcesz z tym zrobiæ, ale ja tego nie naprawiê.{k'{x", keeper, NULL, ch, TO_VICT );
                break;
        }
        ch->reply = keeper;
        return ;
    }

    if ( obj->condition == 100 && obj->item_type != ITEM_MUSICAL_INSTRUMENT )
    {
        act( "{k$n mówi ci '{KNie ma tu co naprawiaæ.{k'{x", keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return ;
    }

    if ( obj->condition == 100 && obj->value[ 2 ] == 100 && obj->item_type == ITEM_MUSICAL_INSTRUMENT )
    {
        act( "{k$n mówi ci '{KTen instrument nie jest ani rozstrojony, ani uszkodzony.{k'{x", keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return ;
    }

    if ( obj->condition < pRep->min_condition )
    {
        act( "$n wskazuje na $h, poczym bezradnie rozk³ada rêce.", keeper, obj, ch, TO_ROOM );
        act( "{k$n mówi ci '{KNie potrafiê tego naprawiæ, poniewa¿ uszkodzenia s± zbyt du¿e. Poszukaj kogo¶ bieglejszego w moim zawodzie.{k'{x", keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return ;
    }

    //0 - no limits
    if ( obj->cost > pRep->max_cost && pRep->max_cost != 0 )
    {
        act( "{k$n mówi ci '{KNie mam pojêcia jak to naprawiæ, to dla mnie zbyt skomplikowane.{k'{x", keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return ;
    }

    if ( obj->item_type != ITEM_MUSICAL_INSTRUMENT )
    {
        cost = UMAX( ( obj->repair_condition - obj->condition ) * obj->cost / 200, 1 );
        cost = pRep->repair_cost_mod * cost / 100;
    }
    else
    {
        cost = UMAX( ( obj->repair_condition - obj->condition ) * obj->cost / 200, 1 ) + UMAX( ( obj->repair_condition - obj->value[ 2] ) * obj->cost / 200, 1 );
        cost = pRep->repair_cost_mod * cost / 100;
    }

    if ( IS_OBJ_STAT( obj, ITEM_HIGH_REPAIR_COST ) )
    {
        cost *= 3;
    }

    sprintf(buf,"{k$n mówi ci '{KNaprawa $f bêdzie kosztowaæ %s.{k'{x", money_string ( cost, FALSE ) );
    act( buf,  keeper, obj, ch, TO_VICT );
    ch->reply = keeper;
    return ;
}

void do_repair( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    char arg_keeper[ MAX_INPUT_LENGTH ];
    char *arg_rest;
    CHAR_DATA * keeper;
    OBJ_DATA * obj;
    OBJ_DATA * obj_next;
    char buf[ MAX_INPUT_LENGTH ];
    long cost;
    REPAIR_DATA *pRep = NULL;
    bool found;
    int repair_counter_increase_chance =  5;// 5% szansy na podbicie countera reperacji
    int repair_penalty_increase_chance = 20;// 1% (bo 20% z 5%) szansy na zmniejszenie maksymalnego limitu

    arg_keeper[0] = '\0';
    arg[0] = '\0';

    arg_rest = one_argument( argument, arg );

    if ( arg_rest[0] != '\0' )
    {
        arg_rest = one_argument( argument, arg );
        one_argument( arg_rest, arg_keeper );
    }

    if ( IS_AFFECTED( ch, AFF_SILENCE ) )
    {
        send_to_char( AFF_SILENCE_TEXT, ch );
        return ;
    }

    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Co chcesz naprawiæ?\n\r", ch );
        return ;
    }

    if ( ch->precommand_fun && !ch->precommand_pending )
    {
        send_to_char( "Robisz teraz co¶ innego.\n\r", ch );
        return ;
    }

    if ( ( keeper = find_repair( ch, arg_keeper ) ) == NULL )
        return ;

    if ( keeper->position != keeper->default_pos )
        return ;

    if ( get_mob_memdat( keeper, NULL, NULL, MEMDAT_ANY ) && HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
    {
        if ( keeper->sex == 2 )
            act( "{k$n mówi ci '{KJestem teraz zajêta. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
        else
            act( "{k$n mówi ci '{KJestem teraz zajêty. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );

        return ;
    }

    if ( ( ch->speaking != 19 &&
           ch->speaking != 0 &&
           UMIN( knows_language( keeper, ch->speaking, ch ), knows_language( ch, ch->speaking, keeper ) ) < 50 ) ||
         ( ch->speaking == 0 && UMIN( knows_language( keeper, ch->speaking, ch ), knows_language( ch, ch->speaking, keeper ) ) < 35 ) )
    {
        act( "$n chyba nie rozumie tego co mówisz.", keeper, NULL, ch, TO_VICT );
        return ;
    }

    if ( (keeper->prog_target != NULL) && (ch != keeper->prog_target))
    {
        sprintf( buf, "{k$n mówi ci '{KObs³ugujê teraz %s, proszê stan±æ w kolejce.{k'{x", keeper->prog_target->name2 );
        act( buf, keeper, NULL, ch, TO_VICT );
        return;
    }

    /* obj check start here */

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
        /* obj check - can't see obj */
        if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
        {
            act( "{k$n mówi ci '{KNie masz takiej rzeczy{k'{x", keeper, NULL, ch, TO_VICT );
            ch->reply = keeper;
            return ;
        }

        /* precommand require obj! */
        if ( !ch->precommand_pending && HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
        {
            if ( mp_precommand_trigger( keeper, ch, obj, &do_repair, "repair", argument ) )
                return ;
        }

        if ( !can_drop_obj( ch, obj ) )
        {
            send_to_char( "Nie mo¿esz tego wypu¶ciæ.\n\r", ch );
            return ;
        }

        if ( !can_see_obj( keeper, obj ) )
        {
            act( "{k$n mówi ci '{KNic takiego nie widzê{k'{x", keeper, NULL, ch, TO_VICT );
            ch->reply = keeper;
            return ;
        }

        /* obj check - can see, don't know obj */
        if ( ( pRep = get_repair_data( keeper->pIndexData->pRepair, obj ) ) == NULL )
        {
            switch ( obj->item_type )
            {
                case ITEM_ARMOR:
                case ITEM_WEAPON:
                    act( "{k$n mówi ci '{KZwróæ siê z tym do kogo¶ innego, ja ci nie pomogê.{k'{x", keeper, NULL, ch, TO_VICT );
                    break;
                case ITEM_CLOTHING:
                    act( "{k$n mówi ci '{KZwróæ siê z tym do krawca, ja ci nie pomogê.{k'{x", keeper, NULL, ch, TO_VICT );
                    break;
                case ITEM_GEM:
                    act( "{k$n mówi ci '{KJa siê na tym nie znam, id¼ do jubilera.{k'{x", keeper, NULL, ch, TO_VICT );
                    break;
                case ITEM_MUSICAL_INSTRUMENT:
                    act( "{k$n mówi ci '{KZwróæ siê z tym lepiej do jakiego¶ muzyka, ja ci nie pomogê.{k'{x", keeper, NULL, ch, TO_VICT );
                    break;
                default:
                    act( "{k$n mówi ci '{KNie wiem co ty chcesz z tym zrobiæ, ale ja tego nie naprawiê.{k'{x", keeper, NULL, ch, TO_VICT );
                    break;
            }
            ch->reply = keeper;
            return ;
        }

        /**
         * ITEM_NOREPAIR
         */
        if ( EXT_IS_SET( obj->extra_flags, ITEM_NOREPAIR ) )
        {
            act( "{k$n mówi ci '{KA to dobre... Wiesz, tego siê nie da naprawiæ.{k'{x", keeper, NULL, ch, TO_VICT );
            ch->reply = keeper;
            return ;
        }

        /* obj check - can see, do know, can't repair obj */
        if ( obj->item_type == ITEM_GEM )
        {
            act( "{k$n mówi ci '{KA to dobre... Wiesz, tego siê nie da naprawiæ.{k'{x", keeper, NULL, ch, TO_VICT );
            ch->reply = keeper;
            return ;
        }

        if ( obj->item_type == ITEM_SKIN )
        {
            act( "{k$n mówi ci 'A niby jak chcesz to naprawiæ. Mam tam co¶ pocerowaæ?{k'{x", keeper, NULL, ch, TO_VICT );
            ch->reply = keeper;
            return ;
        }

        if ( obj->condition == 100 && obj->item_type != ITEM_MUSICAL_INSTRUMENT )
        {
            act( "{k$n mówi ci '{KNie ma tu co naprawiaæ.{k'{x", keeper, NULL, ch, TO_VICT );
            ch->reply = keeper;
            return ;
        }

        if ( obj->condition == 100 && obj->value[ 2 ] == 100 && obj->item_type == ITEM_MUSICAL_INSTRUMENT )
        {
            act( "{k$n mówi ci '{KTen instrument nie jest ani rozstrojony, ani uszkodzony.{k'{x", keeper, NULL, ch, TO_VICT );
            ch->reply = keeper;
            return ;
        }

        if ( obj->condition < pRep->min_condition )
        {
            act( "$n wskazuj±c na $h, poczym bezradnie rozk³ada rêce.", keeper, obj, ch, TO_ROOM );
            act( "{k$n mówi ci '{KNie potrafiê tego naprawiæ, poniewa¿ uszkodzenia s± zbyt du¿e. Poszukaj kogo¶ bieglejszego w moim zawodzie.{k'{x", keeper, NULL, ch, TO_VICT );
            return ;
        }

        /**
         * repair limits
         */
        if ( obj->condition < 50 )
        {
            if ( obj->repair_counter > obj->repair_limit )
            {
                act( "{k$n mówi ci '{KTego nie da siê ju¿ wiêcej naprawiæ.{k'{x", keeper, NULL, ch, TO_VICT );
                ch->reply = keeper;
                return ;
            }
            if ( obj->condition > obj->repair_condition )
            {
                act( "{k$n mówi ci '{KTego nie da siê ju¿ bardziej naprawiaæ.{k'{x", keeper, NULL, ch, TO_VICT );
                ch->reply = keeper;
                return ;
            }
        }

        //0 - no limits
        if ( obj->cost > pRep->max_cost && pRep->max_cost != 0 )
        {
            act( "{k$n mówi ci '{KNie mam pojêcia jak to naprawiæ, to dla mnie zbyt skomplikowane.{k'{x", keeper, NULL, ch, TO_VICT );
            return ;
        }

        /* obj check - can see, do know, can repair obj */
        if ( obj->item_type != ITEM_MUSICAL_INSTRUMENT )
        {
            cost = UMAX( ( obj->repair_condition - obj->condition ) * obj->cost / 200, 1 );
            cost = pRep->repair_cost_mod * cost / 100;
        }
        else
        {
            cost = UMAX( ( obj->repair_condition - obj->condition ) * obj->cost / 200, 1 ) + UMAX( ( obj->repair_condition - obj->value[ 2] ) * obj->cost / 200, 1 );
            cost = pRep->repair_cost_mod * cost / 100;
        }

        if ( IS_OBJ_STAT( obj, ITEM_HIGH_REPAIR_COST ) )
        {
            cost *= 3;
        }

        if ( money_count_copper( ch ) < cost )
        {
            sprintf( buf, "{k$n mówi ci '{KNaprawa kosztowa³aby %s, ale nie staæ ciê na to{k'{x", money_string ( cost, FALSE ) );
            act( buf, keeper, NULL, ch, TO_VICT );
            ch->reply = keeper;
            return ;
        }

        keeper->prog_target = ch;

        act( "{k$n mówi ci '{KNo dobrze, zobaczê co da siê zrobiæ{k'{x", keeper, NULL, ch, TO_VICT );
        act( "$n oddaje $h do naprawy.", ch, obj, NULL, TO_NOTVICT );
        act( "$n w kilku wprawnych ruchach naprawia $h.", keeper, obj, NULL, TO_ROOM );

        if ( ch->wait <= 0 )
        {
            WAIT_STATE( ch, PULSE_VIOLENCE );
        }

        sprintf( buf, "Naprawa kosztowa³a %s.\n\r", money_string ( cost, TRUE ) );
        send_to_char( buf, ch );
        ch->reply = keeper;

        keeper->prog_target = NULL;

        append_file_format_daily( ch, MONEY_LOG_FILE,
                            "-> S: %d %d (%d), B: %d %d (%d) - naprawiony przedmiot [%5d] u moba [%5d]",
                            money_count_copper( ch ), money_count_copper( ch ) - cost, -cost,
                            ch->bank, ch->bank, 0,
                            obj->pIndexData->vnum, keeper->pIndexData->vnum );
        money_reduce( ch, cost );

        /**
         * repair limits
         */
        obj->condition = obj->repair_condition;
        if ( obj->condition < 50 )
        {
            if ( number_percent() < repair_counter_increase_chance )
            {
                obj->repair_counter++;
                if ( obj->repair_penalty > 0 && number_percent() < repair_penalty_increase_chance )
                {
                    obj->repair_condition = UMAX( 0, obj->repair_condition - obj->repair_penalty );
                }
            }
        }

        if ( obj->item_type == ITEM_MUSICAL_INSTRUMENT ) obj->value[ 2 ] = 100;
    }
    else
    {
        found       = FALSE;
        if ( HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
        {
            act( "{k$n mówi ci '{KPoka¿ konkretnie, co chcesz naprawiæ, bo ja nie zajmujê siê masówk±{k'{x", keeper, NULL, ch, TO_VICT );
            return;
        }
        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );

            if ( ( arg[ 3 ] == '\0' || is_name( &arg[ 4 ], obj->name ) )
                && can_drop_obj( ch, obj )
                && can_see_obj( keeper, obj )
                && ( pRep = get_repair_data( keeper->pIndexData->pRepair, obj ))
                && ( obj->item_type != ITEM_GEM )
                && ( obj->item_type != ITEM_SKIN )
                && ( obj->condition < 100 )
                && ( obj->condition > pRep->min_condition )
                && ( obj->cost < pRep->max_cost || pRep->max_cost == 0 )
                && ( obj->item_type != ITEM_MUSICAL_INSTRUMENT )
                && ( obj->repair_limit == 0 || obj->repair_counter < obj->repair_limit )
                && ( obj->condition < obj->repair_condition )
                && !EXT_IS_SET( obj->extra_flags, ITEM_NOREPAIR )
               )
            {
                found = TRUE;

                /**
                 * jezeli celu nie ma w lokacji, lub walczy, to darujmy sobie
                 * to cale naprawianie
                 */
                if ( !ch->in_room || ch->in_room->vnum != keeper->in_room->vnum || ch->fighting != NULL )
                {
                    do_function( keeper, &do_say, "Nieee, no nie bêde pracowaæ w takich warunkach!" );
                    keeper->prog_target = NULL;
                    return;
                }

                cost = UMAX( ( obj->repair_condition - obj->condition ) * obj->cost / 200, 1 );
                cost = pRep->repair_cost_mod * cost / 100;

                if ( IS_OBJ_STAT( obj, ITEM_HIGH_REPAIR_COST ) )
                    cost *= 3;

                if ( money_count_copper( ch ) < cost )
                {
                    if ( keeper->prog_target )
                        act( "{k$n mówi ci '{KNie staæ ciê na dalsze naprawianie.{k'{x", keeper, NULL, ch, TO_VICT );
                    else
                        act( "{k$n mówi ci '{KNie staæ ciê, aby cokolwiek naprawiæ.{k'{x", keeper, NULL, ch, TO_VICT );

                    ch->reply = keeper;
                    keeper->prog_target = NULL;
                    return;
                }

                if ( keeper->prog_target == NULL )
                {
                    keeper->prog_target = ch;
                }

                act( "Podajesz $x $h do naprawy.", keeper, obj, ch, TO_VICT );
                act( "$n oddaje $h do naprawy.", ch, obj, NULL, TO_NOTVICT );
                act( "$n wprawnymi ruchami zaczyna naprawiaæ $h.", keeper, obj, NULL, TO_ROOM );

                sprintf( buf, "Naprawa kosztowa³a %s.\n\r", money_string ( cost, TRUE ) );
                send_to_char( buf, ch );
                ch->reply = keeper;

                append_file_format_daily( ch, MONEY_LOG_FILE,
                                    "-> S: %d %d (%d), B: %d %d (%d) - naprawiony przedmiot [%5d] u moba [%5d]",
                                    money_count_copper( ch ), money_count_copper( ch ) - cost, -cost,
                                    ch->bank, ch->bank, 0,
                                    obj->pIndexData->vnum, keeper->pIndexData->vnum );
                money_reduce( ch, cost );

                /**
                 * repair limits
                 */
                obj->condition = obj->repair_condition;
                if ( obj->condition < 50 )
                {
                    if ( number_percent() < repair_counter_increase_chance )
                    {
                        obj->repair_counter++;
                        if ( obj->repair_penalty > 0 && number_percent() < repair_penalty_increase_chance )
                        {
                            obj->repair_condition = UMAX( 0, obj->repair_condition - obj->repair_penalty );
                        }
                    }
                }
                WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
                wait_function(ch, 1, &do_repair, argument );
                return;
            }
            else
            {
                keeper->prog_target = NULL;
            }
        }

        if ( !found )
        {
            if ( keeper->prog_target )
            {
                act( "$n otrzepuje rêce.", keeper, NULL, NULL, TO_ROOM );
                do_function( keeper, &do_say, "No, to chyba ju¿ wszystko" );
            }
            else
            {
                act( "{k$n mówi ci '{KNie masz przy sobie nic takiego, co mogê naprawiæ{k'{x", keeper, NULL, ch, TO_VICT );
            }
            ch->reply = keeper;
        }
        keeper->prog_target = NULL;
    }
    return ;
}

/* do_list */

void do_list( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	char mxp[ MAX_STRING_LENGTH ];
	char arg_keeper[MAX_INPUT_LENGTH];
	char arg[ MAX_INPUT_LENGTH ];
	char *arg_rest;
	int counter = 0;
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int cost, count;
	bool found;

    if ( ch->fighting )
    {
      send_to_char( "Lepiej skup sie na walce!\n\r", ch );
      return;
    }

	arg_keeper[0] = '\0';
	arg[0] = '\0';

	arg_rest = one_argument( argument, arg );

    if ( arg_rest[0] != '\0' )
    {
        arg_rest = one_argument( argument, arg );
        one_argument( arg_rest, arg_keeper );
    }
	else
	{
		arg[0] = '\0';
		one_argument( argument, arg_keeper );
	}


	if ( IS_AFFECTED( ch, AFF_SILENCE ) )
	{
		send_to_char( AFF_SILENCE_TEXT, ch );
		return;
	}

	if ( ch->precommand_fun && !ch->precommand_pending )
	{
		send_to_char( "Robisz teraz co¶ innego.\n\r", ch );
		return;
	}

	if ( ( keeper = find_keeper( ch, arg_keeper ) ) == NULL )
    {
		return;
    }

	if ( keeper->position != keeper->default_pos )
    {
		return;
    }

	if ( get_mob_memdat( keeper, NULL, NULL, MEMDAT_ANY ) && HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
	{
        switch ( keeper->sex )
        {
            case SEX_FEMALE:
                act( "{k$n mówi ci '{KJestem teraz zajêta. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
                break;
            case SEX_NEUTRAL:
                act( "{k$n mówi ci '{KJestem teraz zajête. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
                break;
            case SEX_MALE:
            default:
                act( "{k$n mówi ci '{KJestem teraz zajêty. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
                break;
        }
        return;
	}

	if ( !ch->precommand_pending && HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
	{
		if ( mp_precommand_trigger( keeper, ch, NULL, &do_list, "list", argument ) )
        {
			return;
        }
	}

	if ( ( ch->speaking != 19 && ch->speaking != 0 && UMIN( knows_language( keeper, ch->speaking, ch ),
			knows_language( ch, ch->speaking, keeper ) ) < 40 ) ||
		 ( ch->speaking == 0 && UMIN( knows_language( keeper, ch->speaking, ch ),
				                      knows_language( ch, ch->speaking, keeper ) ) < 20 ) )
	{
		print_char( ch, "%s chyba nie rozumie tego co mówisz, jednak na wszelki wypadek pokazuje ci swoje towary.\n\r", capitalize( keeper->short_descr ) );
	}

	found = FALSE;

	for ( counter = 1, obj = keeper->carrying; obj; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE
			 && can_see_obj( ch, obj )
			 && can_see_obj( keeper, obj )
			 && ( cost = get_cost( keeper, obj, TRUE ) ) > 0
			 && ( arg[ 0 ] == '\0' || is_name( arg, obj->name ) ) )
		{
			if ( !found )
			{
				found = TRUE;
				print_char( ch, "%s pokazuje ci swoje towary.\n\r", capitalize( keeper->short_descr ) );
				send_to_char( "[Nr         Cena Ilo¶æ] Nazwa\n\r", ch );
			}

			if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && IS_SET( ch->wiznet, WIZ_MXP ) )
			{
				sprintf( mxp, "\e[1z<send 'buy %d|try %d' 'kup %s|wypróbuj %s'>", counter, counter, obj->name4, obj->name4 );
				send_to_char( mxp, ch );
			}

			if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
			{
				if (obj->item_type == ITEM_WEAPON) {
					sprintf( buf, "[%2d  %s -- ] %s {D(", counter, money_string_short( cost ), obj->short_descr );
					send_to_char( buf, ch );
					switch ( obj->value[ 0 ] )
					{
						case( WEAPON_EXOTIC )     : send_to_char( "nieznany", ch       ); break;
						case( WEAPON_SWORD )      : send_to_char( "miecz", ch          ); break;
						case( WEAPON_DAGGER )     : send_to_char( "sztylet", ch        ); break;
						case( WEAPON_SPEAR )      : send_to_char( "w³ócznia", ch       ); break;
						case( WEAPON_MACE )       : send_to_char( "maczuga", ch        ); break;
						case( WEAPON_AXE )        : send_to_char( "topór", ch          ); break;
						case( WEAPON_FLAIL )      : send_to_char( "korbacz", ch        ); break;
						case( WEAPON_WHIP )       : send_to_char( "bat", ch            ); break;
						case( WEAPON_POLEARM )    : send_to_char( "broñ drzewcowa", ch ); break;
						case( WEAPON_STAFF )      : send_to_char( "laska", ch          ); break;
						case( WEAPON_SHORTSWORD ) : send_to_char( "krótki miecz", ch   ); break;
						case( WEAPON_CLAWS )      : send_to_char( "pazury", ch         ); break;
						default                   : send_to_char( "nieznany", ch       ); break;
					}
					send_to_char("){x", ch);
				}
				else {
					sprintf( buf, "[%2d  %s -- ] %s", counter, money_string_short( cost ), obj->short_descr );
					send_to_char( buf, ch );
				}
			}
			else
			{
				count = 1;

				while ( obj->next_content != NULL &&
				        obj->pIndexData == obj->next_content->pIndexData &&
				        !str_cmp( obj->short_descr, obj->next_content->short_descr ) )
				{
				    obj = obj->next_content;
				    count++;
				}

				if (obj->item_type == ITEM_WEAPON) {
					sprintf( buf, "[%2d  %s %2d ] %s {D(", counter, money_string_short( cost ), count, obj->short_descr );
					send_to_char( buf, ch );
					switch ( obj->value[ 0 ] )
					{
						case( WEAPON_EXOTIC )     : send_to_char( "nieznany", ch       ); break;
						case( WEAPON_SWORD )      : send_to_char( "miecz", ch          ); break;
						case( WEAPON_DAGGER )     : send_to_char( "sztylet", ch        ); break;
						case( WEAPON_SPEAR )      : send_to_char( "w³ócznia", ch       ); break;
						case( WEAPON_MACE )       : send_to_char( "maczuga", ch        ); break;
						case( WEAPON_AXE )        : send_to_char( "topór", ch          ); break;
						case( WEAPON_FLAIL )      : send_to_char( "korbacz", ch        ); break;
						case( WEAPON_WHIP )       : send_to_char( "bat", ch            ); break;
						case( WEAPON_POLEARM )    : send_to_char( "broñ drzewcowa", ch ); break;
						case( WEAPON_STAFF )      : send_to_char( "laska", ch          ); break;
						case( WEAPON_SHORTSWORD ) : send_to_char( "krótki miecz", ch   ); break;
						case( WEAPON_CLAWS )      : send_to_char( "pazury", ch         ); break;
						default                   : send_to_char( "nieznany", ch       ); break;
					}
					send_to_char("){x", ch);
				}
				else {
					sprintf( buf, "[%2d  %s %2d ] %s", counter, money_string_short( cost ), count, obj->short_descr );
					send_to_char( buf, ch );
				}
			}
			if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && IS_SET( ch->wiznet, WIZ_MXP ) )
			{
				send_to_char( "</send>", ch );
			}
            if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
            {
                sprintf( buf, " {D[%d]{x", obj->pIndexData->vnum );
                send_to_char( buf, ch );
            }
			send_to_char( "\n\r", ch );
			counter++;
		}
	}

	if ( !found )
    {
		send_to_char( "Nie mo¿esz tu niczego kupiæ.\n\r", ch );
    }
	return;
}

void do_sell( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	char arg_keeper[MAX_INPUT_LENGTH];
	char *arg_rest;
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int vwis, luck, charisma, trade, bonus = 0, cost;

    if ( ch->fighting )
    {
      send_to_char( "Lepiej skup sie na walce!\n\r", ch );
      return;
    }

	arg_keeper[0] = '\0';
	arg[0] = '\0';

	arg_rest = one_argument( argument, arg );

    if ( arg_rest[0] != '\0' )
    {
        arg_rest = one_argument( argument, arg );
        one_argument( arg_rest, arg_keeper );
    }

	if ( IS_AFFECTED( ch, AFF_SILENCE ) )
	{
		send_to_char( AFF_SILENCE_TEXT, ch );
		return;
	}

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Co chcesz sprzedaæ?\n\r", ch );
		return;
	}

	if ( ch->precommand_fun && !ch->precommand_pending )
	{
		send_to_char( "Robisz teraz co¶ innego.\n\r", ch );
		return;
	}

	if ( ( keeper = find_keeper( ch, arg_keeper ) ) == NULL )
    {
		return;
    }

	if ( keeper->position != keeper->default_pos )
    {
		return;
    }

    if ( get_mob_memdat( keeper, NULL, NULL, MEMDAT_ANY ) && HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
    {
        switch ( keeper->sex )
        {
            case SEX_FEMALE:
                act( "{k$n mówi ci '{KJestem teraz zajêta. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
                break;
            case SEX_NEUTRAL:
                act( "{k$n mówi ci '{KJestem teraz zajête. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
                break;
            case SEX_MALE:
            default:
                act( "{k$n mówi ci '{KJestem teraz zajêty. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
        }
        return;
    }

	if ( ( ch->speaking != 19 && ch->speaking != 0 && UMIN( knows_language( keeper, ch->speaking, ch ),
			knows_language( ch, ch->speaking, keeper ) ) < 50 ) ||
		 ( ch->speaking == 0 && UMIN( knows_language( keeper, ch->speaking, ch ),
				                      knows_language( ch, ch->speaking, keeper ) ) < 35 ) )
	{
		act( "$n chyba nie rozumie tego co mówisz.", keeper, NULL, ch, TO_VICT );
		return;
	}

	if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	{
		act( "{k$n mówi ci '{KNie masz takiej rzeczy.{k'{x", keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
		send_to_char( "Nie mo¿esz tego wypu¶ciæ.\n\r", ch );
		return;
	}

	if ( !can_see_obj( keeper, obj ) )
	{
		act( "$n nie widzi tego co oferujesz.", keeper, NULL, ch, TO_VICT );
		return;
	}

	if ( !ch->precommand_pending &&
		 HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
	{
		if ( mp_precommand_trigger( keeper, ch, obj, &do_sell, "sell", argument ) )
        {
			return;
        }
	}

	if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
	{
		switch ( keeper->sex )
		{
			case SEX_NEUTRAL:
				act( "$n nie jest zainteresowane kupnem $f.", keeper, obj, ch, TO_VICT );
				break;
            case SEX_FEMALE:
				act( "$n nie jest zainteresowana kupnem $f.", keeper, obj, ch, TO_VICT );
				break;
			case SEX_MALE:
			default:
				act( "$n nie jest zainteresowany kupnem $f.", keeper, obj, ch, TO_VICT );
				break;
		}
		return;
	}

	charisma = get_curr_stat_deprecated( ch, STAT_CHA );
	vwis = get_curr_stat_deprecated( keeper, STAT_WIS );
	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( ( trade = get_skill( ch, gsn_trade ) ) > 0 )
	{
		if ((ch->sex == 1 && keeper->sex == 2) || (ch->sex == 2	&& keeper->sex != 2))
			bonus = 20;

		if ( number_range(0,luck+charisma+trade/2) < 10)
		{
			cost -= ((100 - trade)*cost)/800;
			send_to_char( "Twoje starania o wytargowanie jak najwy¿szej ceny bardzo denerwuj± sklepikarza.\n\r", ch );
			check_improve(ch, NULL, gsn_trade, TRUE, 60 );
		}
		else if ( (charisma-10)*2 + trade + luck/3 + bonus < number_range(1,100) + vwis*2 )
		{
			send_to_char( "Starasz siê wytargowaæ jak najwy¿sz± cenê, jednak sprzedawca pozostaje nieugiêty.\n\r", ch );
			check_improve(ch, NULL, gsn_trade, TRUE, 80 );
		}
		else
		{
			cost += ((trade + (charisma/2))*cost)/400;
			send_to_char( "D³ugo k³ócisz siê ze sklepikarzem, w koñcu udaje ci siê wytargowaæ najkorzystniejsz± cenê.\n\r", ch );
			check_improve(ch, NULL, gsn_trade, TRUE, 70 );
		}
	}

	if ( cost > money_count_copper ( keeper ) )
	{
		act( "{k$n mówi ci '{KNie mam wystarczaj±cej sumy pieniêdzy ¿eby kupiæ $h.{k'{x", keeper, obj, ch, TO_VICT );
		return;
	}

	if ( cost > 5000 * RATTING_SILVER )
    {
		act( "$n sprzedaje $h za niewyobra¿aln± sumê pieniêdzy.", ch, obj, NULL, TO_ROOM );
    }
	else if ( cost > 1000 * RATTING_SILVER )
    {
		act( "$n sprzedaje $h za ogromn± sumê pieniêdzy.", ch, obj, NULL, TO_ROOM );
    }
	else if ( cost > 500 * RATTING_SILVER )
    {
		act( "$n sprzedaje $h za du¿± sumê pieniêdzy.", ch, obj, NULL, TO_ROOM );
    }
	else if ( cost > 250 * RATTING_SILVER )
    {
		act( "$n sprzedaje $h za do¶æ du¿± sumê pieniêdzy.", ch, obj, NULL, TO_ROOM );
    }
	else if ( cost == 1 )
    {
		act( "$n sprzedaje $h za jedn± miedzian± monetê.", ch, obj, NULL, TO_ROOM );
    }
	else
    {
		act( "$n sprzedaje $h.", ch, obj, NULL, TO_ROOM );
    }

	if ( cost == 1 )
    {
		sprintf( buf, "Sprzedajesz $h za jedn± miedzian± monetê." );
    }
	else
    {
		sprintf( buf, "Sprzedajesz $h za %s.", money_string( cost, TRUE ) );
    }

	act( buf, ch, obj, NULL, TO_CHAR );

	/* operacja finansowa, sprawdzenie czy keeper nie ma "mniej ni¿ zero */

    append_file_format_daily
        (
         ch,
         MONEY_LOG_FILE,
         "-> S: %ld %ld (%ld) - sprzeda³ obiekt [%5d] u moba [%5d]",
         money_count_copper( ch ),
         money_count_copper( ch ) + cost,
         cost,
         obj->pIndexData->vnum,
         keeper->pIndexData->vnum
        );

    money_gain( ch, cost );
    money_reduce( keeper, cost );

	if ( obj->item_type == ITEM_TRASH || IS_OBJ_STAT( obj, ITEM_SELL_EXTRACT ) )
	{
		/*artefact*/
		if ( is_artefact( obj ) )
			extract_artefact( obj );
		extract_obj( obj );
	}
	else
	{
		/*artefact*/
		if ( is_artefact( obj ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
			artefact_from_char( obj, ch );
		obj_from_char( obj );

		if ( obj->timer )
			EXT_SET_BIT( obj->extra_flags, ITEM_HAD_TIMER );
		else
			obj->timer = number_range( 50, 100 );
		obj_to_keeper( obj, keeper );
		/* taki fix co by sprzedawca nie naprawia³ kamieni, bo skoro wcze¶niej
		zawsze mówi, ¿e siê nie da naprawiæ, to by³by burak, jakby mo¿na
		by³o sprzedaæ/kupiæ i by³pby w porzo, prawda? */
		if ( obj->item_type != ITEM_GEM )
			obj->condition = 100;
	}

	return;
}

void do_value( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	char arg_keeper[MAX_INPUT_LENGTH];
	char *arg_rest;
	CHAR_DATA *keeper;
	OBJ_DATA *obj;
	int amount;

    if ( ch->fighting )
    {
      send_to_char( "Lepiej skup sie na walce!\n\r", ch );
      return;
    }

	arg_keeper[0] = '\0';
	arg[0] = '\0';

	arg_rest = one_argument( argument, arg );

    if ( arg_rest[0] != '\0' )
    {
        arg_rest = one_argument( argument, arg );
        one_argument( arg_rest, arg_keeper );
    }

	if ( IS_AFFECTED( ch, AFF_SILENCE ) )
	{
		send_to_char( AFF_SILENCE_TEXT, ch );
		return;
	}

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Co wyceniæ?\n\r", ch );
		return;
	}

	if ( ch->precommand_fun && !ch->precommand_pending )
	{
		send_to_char( "Robisz teraz co¶ innego.\n\r", ch );
		return;
	}

	if ( ( keeper = find_keeper( ch, arg_keeper ) ) == NULL )
    {
        return;
    }

	if ( keeper->position != keeper->default_pos )
    {
		return;
    }

	if ( get_mob_memdat( keeper, NULL, NULL, MEMDAT_ANY ) && HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
	{
        switch ( keeper->sex )
        {
            case SEX_FEMALE:
                act( "{k$n mówi ci '{KJestem teraz zajêta. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
                break;
            case SEX_NEUTRAL:
                act( "{k$n mówi ci '{KJestem teraz zajête. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
                break;
            case SEX_MALE:
            default:
                act( "{k$n mówi ci '{KJestem teraz zajêty. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
                break;
        }
		return;
	}

	if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	{
		act( "{k$n mówi ci '{KNie masz tej rzeczy.{k'{x", keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}


	if ( !can_see_obj( keeper, obj ) )
	{
		act( "$n nie widzi tego co oferujesz.", keeper, NULL, ch, TO_VICT );
		return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
		send_to_char( "Nie mo¿esz tego wypu¶ciæ.\n\r", ch );
		return;
	}

	if ( !ch->precommand_pending && HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
	{
		if ( mp_precommand_trigger( keeper, ch, obj, &do_value, "value", argument ) )
        {
			return;
        }
	}

	if ( ( amount = get_cost( keeper, obj, FALSE ) ) < 1 )
	{
		switch ( keeper->sex )
		{
			case SEX_NEUTRAL:
				act( "$n nie jest zainteresowane kupnem $f.", keeper, obj, ch, TO_VICT );
				break;
            case SEX_FEMALE:
				act( "$n nie jest zainteresowana kupnem $f.", keeper, obj, ch, TO_VICT );
				break;
			case SEX_MALE:
			default:
				act( "$n nie jest zainteresowany kupnem $f.", keeper, obj, ch, TO_VICT );
				break;
		}
		return;
	}

	if ( !ch->precommand_pending && HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
	{
		if ( mp_precommand_trigger( keeper, ch, obj, &do_value, "value", argument ) )
        {
			return;
        }
	}

    if ( amount > money_count_copper( keeper ) )
    {
        switch ( keeper->sex )
        {
            case SEX_NEUTRAL:
                sprintf( buf, "{k$n mówi ci '{KMoglobym ci daæ %s za $h, ale aktualnie moja sakiewka jest chudsza.{k'{x", money_string( amount, FALSE ) );
                break;
            case SEX_FEMALE:
                sprintf( buf, "{k$n mówi ci '{KMoglabym ci daæ %s za $h, ale aktualnie moja sakiewka jest chudsza.{k'{x", money_string( amount, FALSE ) );
                break;
            case SEX_MALE:
            default:
                sprintf( buf, "{k$n mówi ci '{KMoglbym ci daæ %s za $h, ale aktualnie moja sakiewka jest chudsza.{k'{x", money_string( amount, FALSE ) );
                break;
        }
    }
    else
    {
        sprintf( buf, "{k$n mówi ci '{KMogê ci daæ %s za $h.{k'{x", money_string( amount, FALSE ) );
    }

	act( buf, keeper, obj, ch, TO_VICT );
	ch->reply = keeper;

	return;
}

void do_try( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
	char arg_keeper[MAX_INPUT_LENGTH];
	char *arg_rest;
	CHAR_DATA * keeper;
	OBJ_DATA *obj;
	bool can_wear;
	bool shown;

    if ( ch->fighting )
    {
      send_to_char( "Lepiej skup sie na walce!\n\r", ch );
      return;
    }

	arg_keeper[0] = '\0';
	arg[0] = '\0';

	arg_rest = one_argument( argument, arg );

	if ( arg_rest[0] != '\0' )
	{
	   argument = one_argument( argument, arg );
				  one_argument( argument, arg_keeper );
	}

	if ( IS_AFFECTED( ch, AFF_SILENCE ) )
	{
		send_to_char( AFF_SILENCE_TEXT, ch );
		return;
	}

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Co chcesz przymierzyæ?\n\r", ch );
		return;
	}

	if ( ch->precommand_fun && !ch->precommand_pending )
	{
		send_to_char( "Robisz teraz co¶ innego.\n\r", ch );
		return;
	}

	if ( ( keeper = find_keeper( ch, arg_keeper ) ) == NULL )
		return;

	if ( keeper->position != keeper->default_pos )
		return;

	if ( get_mob_memdat( keeper, NULL, NULL, MEMDAT_ANY ) && HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
	{
		if ( keeper->sex == 2 )
			act( "{k$n mówi ci '{KJestem teraz zajêta. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
		else
			act( "{k$n mówi ci '{KJestem teraz zajêty. Proszê poczekaæ chwilkê.{k'{x", keeper, NULL, ch, TO_VICT );
		return;
	}

	if ( ( ch->speaking != 19 && ch->speaking != 0 && UMIN( knows_language( keeper, ch->speaking, ch ),
			knows_language( ch, ch->speaking, keeper ) ) < 50 ) ||
		 ( ch->speaking == 0 && UMIN( knows_language( keeper, ch->speaking, ch ),
				                      knows_language( ch, ch->speaking, keeper ) ) < 35 ) )
	{
		print_char( ch, "%s chyba nie rozumie tego co mówisz.\n\r", capitalize( keeper->short_descr ) );
		return;
	}

	obj = get_obj_keeper( ch, keeper, arg );

	if ( !obj || !can_see_obj( ch, obj ) )
	{
		act( "{k$n mówi ci '{KNic takiego nie mam na sprzeda¿.{k'{x",
			 keeper, NULL, ch, TO_VICT );
		ch->reply = keeper;
		return;
	}

	if ( !ch->precommand_pending && HAS_TRIGGER( keeper, TRIG_PRECOMMAND ) )
	{
		if ( mp_precommand_trigger( keeper, ch, obj, &do_try, "try", argument ) )
			return;
	}

	can_wear = can_equip_obj( ch, obj, WEAR_TRY );

	if ( obj->wear_flags == 0 )
	{
		send_to_char( "Tego przedmiotu nie da siê podnie¶æ.\n\r", ch );
		return;
	}

	shown = FALSE;
	switch ( obj->item_type )
	{
		case ITEM_LIGHT:
			break;
		case ITEM_WAND:
			break;
		case ITEM_STAFF:
			break;
		case ITEM_WEAPON:
			if ( IS_WEAPON_STAT( obj, WEAPON_TWO_HANDS ) )
			{
				if ( can_wear )
					act( "Chwytasz $h w obie rêce. Broñ w sam raz dla ciebie.", ch, obj, keeper, TO_CHAR );
				else
					act( "Chwytasz $h w obie rêce, ale co¶ ci nie pasuje.", ch, obj, keeper, TO_CHAR );
				act( "$n przymierza $h.", ch, obj, keeper, TO_ROOM );
			}
			else
			{
				if ( can_wear )
					act( "Bierzesz $h do rêki. Broñ w sam raz dla ciebie.", ch, obj, keeper, TO_CHAR );
				else
					act( "Bierzesz $h do rêki, ale co¶ ci nie pasuje.", ch, obj, keeper, TO_CHAR );
				act( "$n przymierza $h.", ch, obj, keeper, TO_ROOM );
			}
			shown = TRUE;
			break;
		case ITEM_TREASURE:
			break;
		case ITEM_ARMOR:
			if ( can_wear )
			{
				act( "Przymierzasz $h. Wszystko wydaje siê doskonale pasowaæ.", ch, obj, keeper, TO_CHAR );
				act( "$n przymierza $h. Wszystko wydaje siê doskonale pasowaæ.", ch, obj, keeper, TO_ROOM );
			}
			else
			{
				act( "Przymierzasz $p. To raczej nie bêdzie na ciebie pasowaæ.", ch, obj, keeper, TO_CHAR );
				act( "$n przymierza $h. Chyba jednak co¶ nie bardzo pasuje.", ch, obj, keeper, TO_ROOM );
			}
			shown = TRUE;
			break;
		case ITEM_CLOTHING:
			if ( can_wear )
			{
				act( "Przymierzasz $h, ca³kiem wygodne ubranie.", ch, obj, keeper, TO_CHAR );
				act( "$n przymierza $h, pasuje jak ula³.", ch, obj, keeper, TO_ROOM );
			}
			else
			{
				act( "$p raczej nie bêdzie na ciebie pasowaæ. Wypróbuj inne ubranie.", ch, obj, keeper, TO_CHAR );
				act( "$n przymierza $h, jednak to ubranie nie pasuje.", ch, obj, keeper, TO_ROOM );
			}
			shown = TRUE;
			break;
		case ITEM_TRASH:
		case ITEM_TROPHY:
			break;
		case ITEM_CONTAINER:
			if ( can_wear )
			{
				act( "$p pasuje na ciebie, ciekawe co bêdziesz w tym nosiæ?", ch, obj, keeper, TO_CHAR );
				act( "$n przymierza $h. Ciekawe co bêdzie w tym nosiæ.", ch, obj, keeper, TO_ROOM );
			}
			else
			{
				act( "$p nie pasuje na ciebie, rozejrzyj sie za innym pojemnikiem.", ch, obj, keeper, TO_CHAR );
				act( "$n na pró¿no przymierza $h. Po chwili odk³ada niepasuj±cy pojemnik.", ch, obj, keeper, TO_ROOM );
			}
			shown = TRUE;
			break;
		case ITEM_DRINK_CON:
			act( "Usi³ujesz niepostrze¿enie upiæ trochê z $f, lecz $N stanowczo wyciaga ³apê po pieni±dze.", ch, obj, keeper, TO_CHAR );
			act( "$n usi³uje niepostrze¿enie upiæ trochê z $f, lecz $N stanowczo wyciaga ³apê po pieni±dze.", ch, obj, keeper, TO_ROOM );
			shown = TRUE;
			break;
		case ITEM_KEY:
			break;
		case ITEM_FOOD:
			act( "Usi³ujesz nadgry¼æ $h, lecz $N stanowczo wyciaga ³apê po pieni±dze.", ch, obj, keeper, TO_CHAR );
			act( "$n usi³uje niepostrze¿enie posmakowaæ $h, lecz $N stanowczo wyci±ga ³apê po pieni±dze.", ch, obj, keeper, TO_ROOM );
			shown = TRUE;
			break;
		case ITEM_ROOM_KEY:
			if ( can_wear )
			{
				act( "Bierzesz $p, wygl±da przyzwoicie, ale wypróbuj go lepiej na zamku.", ch, obj, keeper, TO_CHAR );
				act( "$n z zadowoleniem przymierza $h wszedzie, tylko nie do zamka.", ch, obj, keeper, TO_ROOM );
			}
			else
			{
				act( "Bierzesz $p, nie wygl±da najlepiej, ale wypróbuj go lepiej na zamku.", ch, obj, keeper, TO_CHAR );
				act( "$n z niewyra¼n± min± usi³uje przymierzyæ $h wszedzie, tylko nie do zamka.", ch, obj, keeper, TO_ROOM );
			}
			shown = TRUE;
			break;
		case ITEM_JEWELRY:
			break;
		case ITEM_JUKEBOX:
			break;
		case ITEM_PIECE:
			break;
		case ITEM_SPELLBOOK:
			break;
		case ITEM_SPELLITEM:
			break;
		case ITEM_ENVENOMER:
			act( "Usi³ujesz siê przytruæ $j, lecz $N stanowczo wyciaga ³apê po pieni±dze.", ch, obj, keeper, TO_CHAR );
			act( "$n usi³uje siê przytruæ $j, lecz $N stanowczo wyci±ga ³apê po pieni±dze.", ch, obj, keeper, TO_ROOM );
			shown = TRUE;
			break;
		case ITEM_BANDAGE:
			break;
		case ITEM_TURN:
			break;
      case ITEM_TOTEM:
         break;
		case ITEM_SHIELD:
			break;
		case ITEM_TOOL:
			if ( can_wear )
				act( "Bierzesz $h do rêki. Narzêdzie w sam raz dla ciebie.", ch, obj, keeper, TO_CHAR );
			else
				act( "Bierzesz $h do rêki, ale co¶ ci nie pasuje.", ch, obj, keeper, TO_CHAR );
			act( "$n przymierza $h.", ch, obj, keeper, TO_ROOM );
			break;
		default:
			act( "Nie za bardzo wiesz jak to przymierzyæ.", ch, obj, keeper, TO_CHAR );
			shown = TRUE;
			break;
	}

	if ( !shown )
	{
		if ( can_wear )
			act( "Przymierzasz $h. Wszystko wydaje siê w porz±dku.", ch, obj, keeper, TO_CHAR );
		else
			act( "Przymierzasz $h, ale co¶ ci nie pasuje.", ch, obj, keeper, TO_CHAR );
		act( "$n przymierza $h.", ch, obj, keeper, TO_ROOM );
	}
}

bool check_only_weight_cant_equip( CHAR_DATA *ch, OBJ_DATA *obj )
{
    int val;
    int weight = get_obj_weight( obj );
    OBJ_DATA *obj_in_loop;
    /**
     * jak cos nie wazy, to nie ma co dalej sprawdzac
     */
    if ( weight < 1 )
    {
        return FALSE;
    }
    /**
     * sprawdzanie broni oddzielnie, bo inaczej liczone
     * oraz inne komunikaty
     */
    if ( obj->item_type == ITEM_WEAPON )
    {
        bool check = FALSE;
        val = 10 * str_app[ get_curr_stat( ch, STAT_STR ) ].wield;
        if ( weight > ( ( check && IS_WEAPON_STAT( obj, WEAPON_TWO_HANDS ) )? ( 3 * val ) / 2 : val ) )
        {
            send_to_char( "Ta broñ jest zbyt ciê¿ka.\n\r", ch );
            act( "$n próbuje dobyæ $h, ale nie daje rady.", ch, obj, NULL, TO_ROOM );
            return TRUE;
        }
        return FALSE;
    }
    /**
     * jezeli nie bron, to liczymy dalej
     *
     * dwie trzecie tego co mozny podniesc, mozemy na siebie zalozyc
     */
    val  = can_carry_w( ch );
    val *= 2;
    val /= 3;
    /**
     * najpierw spradzamy czy przedmiot jest po prostu za ciezki
     */
    if ( weight > val )
    {
        act( "Próbujesz za³o¿yæ $h, ale nie dajesz rady.", ch, obj, NULL, TO_CHAR );
        act( "$n próbuje za³o¿yæ $h, ale nie daje rady.", ch, obj, NULL, TO_ROOM );
        return TRUE;
    }
    /**
     * potem zliczamy wage tego co ma postac ubrane
     */
    val  = can_carry_w( ch );
    val *= 3;
    val /= 4;
    for ( obj_in_loop = ch->carrying; obj_in_loop != NULL; obj_in_loop = obj_in_loop->next_content )
    {
        if ( obj_in_loop->wear_loc != WEAR_NONE )
        {
            val -= get_obj_weight( obj_in_loop );
        }
    }
    /**
     * i sprawdzamy czy mozna to zalozyc
     */
    if ( weight > val )
    {
        if ( obj->liczba_mnoga )
        {
            act( "Nie dasz rady teraz za³o¿yæ $f, bo ciê przewa¿±.", ch, obj, NULL, TO_CHAR );
        }
        else
        {
            act( "Nie dasz rady teraz za³o¿yæ $f, bo ciê przewa¿y.", ch, obj, NULL, TO_CHAR );
        }
        act( "$n próbuje za³o¿yæ $h, ale nie daje rady.", ch, obj, NULL, TO_ROOM );
        return TRUE;
    }
    return FALSE;
}

void wield_weapon_helper_draining_hands(CHAR_DATA *ch, OBJ_DATA *obj)
{
    if ( is_affected( ch, gsn_draining_hands ) )
    {
        affect_strip(ch, gsn_draining_hands);
        send_to_char( "Otaczaj±ca twe d³onie aura znika.\n\r", ch );
    }
    return;
}

void wield_weapon( CHAR_DATA *ch, OBJ_DATA *obj, bool primary )
{
    OBJ_DATA * weapon;
    bool check;

    if ( !GOT_PART( ch, PART_HANDS ) )
    {
        send_to_char( "Nie masz czym chwycic broni.\n\r", ch );
        return;
    }

    if ( !IS_SET( race_table[ ch->real_race == 0 ? GET_RACE( ch ) : ch->real_race ].wear_flag, ITEM_WIELD ) && primary )
    {
        return;
    }

    if ( !IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, ITEM_WIELDSECOND ) && !primary )
    {
        return;
    }

    if ( primary && CAN_WEAR( obj, ITEM_WIELD ) )
    {
        if ( IS_WEAPON_STAT( obj, WEAPON_TWO_HANDS ) && get_hand_slots( ch, WEAR_WIELD ) >= 1 )
        {
            send_to_char( "Masz ju¿ dwie zajête rêce. Zdejmij co¶.\n\r", ch );
            return;
        }
        else if ( get_hand_slots( ch, WEAR_WIELD ) >= 2 )
        {
            send_to_char( "Masz ju¿ dwie zajête rêce. Zdejmij co¶.\n\r", ch );
            return;
        }

        if ( !remove_obj( ch, WEAR_WIELD, TRUE ) )
        {
            return;
        }


        if ( check_only_weight_cant_equip( ch, obj ) )
        {
            return;
        }

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_WIELD ) ) return;
            obj->prewear_loc = WEAR_WIELD;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_WIELD, TRUE ) )
            {
                wield_weapon_helper_draining_hands(ch, obj);
                if ( IS_WEAPON_STAT( obj, WEAPON_TWO_HANDS ) )
                {
                    act( "$n chwyta $h w obie rece.", ch, obj, NULL, TO_ROOM );
                    act( "Chwytasz $h w obie rece.", ch, obj, NULL, TO_CHAR );
                }
                else
                {
                    act( "$n bierze do reki $h.", ch, obj, NULL, TO_ROOM );
                    act( "Bierzesz do reki $h.", ch, obj, NULL, TO_CHAR );
                }

                obj_trap_handler( ch, obj, TRAP_WEAR );

                if ( !ch->in_room )
                {
                    return;
                }

                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                {
                    op_common_trigger( ch, obj, &TRIG_WEAR );
                }
            }
        return;
    }
    else if ( !primary && CAN_WEAR( obj, ITEM_WIELD ) )
    {
        if ( get_hand_slots( ch, WEAR_SECOND ) >= 2 )
        {
            send_to_char( "Masz ju¿ dwie zajête rêce. Zdejmij co¶.\n\r", ch );
            return;
        }

        if ( !remove_obj( ch, WEAR_SECOND, TRUE ) )
        {
            return;
        }

        if ( ( weapon = get_eq_char ( ch, WEAR_WIELD ) ) == NULL )
        {
            send_to_char ( "Nie mo¿esz u¿ywaæ tylko drugiej broni.\n\r", ch );
            return;
        }

        check = TRUE;
        if ( ( IS_WEAPON_STAT( obj, WEAPON_PRIMARY ) && check ) || IS_WEAPON_STAT( obj, WEAPON_TWO_HANDS ) )
        {
            send_to_char ( "Ta broñ siê do tego nie nadaje.\n\r", ch );
            return;
        }

        if ( check_only_weight_cant_equip( ch, obj ) )
        {
            return;
        }

        if ( !remove_obj( ch, WEAR_SECOND, TRUE ) )
        {
            print_char( ch, "Nie mo¿esz od³o¿yæ %s.\n\r", get_eq_char ( ch, WEAR_SECOND ) ->name2 );
            return;
        }

        if ( HAS_OTRIGGER( obj, TRIG_PREWEAR ) )
        {
            if ( !can_equip_obj( ch, obj, WEAR_SECOND ) )
            {
                return;
            }
            obj->prewear_loc = WEAR_SECOND;
            op_common_trigger( ch, obj, &TRIG_PREWEAR );
        }
        else
            if ( equip_char( ch, obj, WEAR_SECOND, TRUE ) )
            {
                wield_weapon_helper_draining_hands(ch, obj);
                act ( "$n bierze $h w drug± rêkê.", ch, obj, NULL, TO_ROOM );
                act ( "Bierzesz $h w drug± rêkê.", ch, obj, NULL, TO_CHAR );
                obj_trap_handler( ch, obj, TRAP_WEAR );
                if ( !ch->in_room )
                {
                    return;
                }
                if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
                {
                    op_common_trigger( ch, obj, &TRIG_WEAR );
                }
            }
    }
    return;
}

void wield_weapon_silent( CHAR_DATA *ch, OBJ_DATA *obj, bool primary )
{
	OBJ_DATA * weapon;
    bool check;

	if ( !GOT_PART( ch, PART_HANDS ) )
		return;

	if ( primary && CAN_WEAR( obj, ITEM_WIELD ) )
	{
		if ( !remove_obj( ch, WEAR_WIELD, TRUE ) )
        {
			return;
        }

        if ( check_only_weight_cant_equip( ch, obj ) )
        {
            return;
        }

		if ( !IS_NPC( ch )
			 && ( IS_WEAPON_STAT( obj, WEAPON_TWO_HANDS ) )
			 && ( ( get_eq_char( ch, WEAR_SHIELD ) != NULL )
				  || ( get_eq_char( ch, WEAR_SECOND ) != NULL )
				  || ( get_eq_char( ch, WEAR_HOLD ) != NULL ) ) )
		{
			send_to_char( "Potrzebujesz dwóch r±k ¿eby to trzymac.\n\r", ch );
			return;
		}

		if ( equip_char( ch, obj, WEAR_WIELD, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );
		}
		return;
	}
	else if ( !primary && CAN_WEAR( obj, ITEM_WIELD ) )
	{
		if ( !remove_obj( ch, WEAR_SECOND, TRUE ) )
			return;

		if ( ( get_eq_char ( ch, WEAR_SHIELD ) != NULL ) ||
			 ( get_eq_char ( ch, WEAR_HOLD ) != NULL ) )
		{
			send_to_char ( "Zdejmij tarcze i to co masz w rêce je¶li chcesz u¿ywaæ drugiej broni.\n\r", ch );
			return;
		}

		if ( ( weapon = get_eq_char ( ch, WEAR_WIELD ) ) == NULL )
		{
			send_to_char ( "Nie mo¿esz u¿ywaæ tylko drugiej broni.\n\r", ch );
			return;
		}

		if ( IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS ) )
		{
			send_to_char ( "Nie masz wolnej rêki.\n\r", ch );
			return;
		}

        check = FALSE;
		if ( ( IS_WEAPON_STAT( obj, WEAPON_PRIMARY ) && !check ) || IS_WEAPON_STAT( obj, WEAPON_TWO_HANDS ) )
		{
			send_to_char ( "Ta broñ sie do tego nie nadaje.\n\r", ch );
			return;
		}

        if ( check_only_weight_cant_equip( ch, obj ) )
        {
            return;
        }

		if ( !remove_obj( ch, WEAR_SECOND, TRUE ) )
		{
			print_char( ch, "Nie mo¿esz od³o¿yæ %s.\n\r", get_eq_char ( ch, WEAR_SECOND ) ->name2 );
			return;
		}

		if ( equip_char( ch, obj, WEAR_SECOND, TRUE ) )
		{
			obj_trap_handler( ch, obj, TRAP_WEAR );

			if ( !ch->in_room )
				return;

			if ( HAS_OTRIGGER( obj, TRIG_WEAR ) )
				op_common_trigger( ch, obj, &TRIG_WEAR );
		}
	}
	return;
}


void do_connect( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA * first_ob;
	OBJ_DATA * second_ob;
	OBJ_DATA * new_ob = NULL;
	char arg1[ MAX_STRING_LENGTH ];
	char arg2[ MAX_STRING_LENGTH ];

	if ( IS_NPC( ch ) )
		return;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
	{
		send_to_char( "Musisz po³±czyc jeden kawa³ek z drugim.\n\r", ch );
		return;
	}

	if ( ( first_ob = get_obj_carry( ch, arg1, ch ) ) == NULL )
	{
		send_to_char( "Przecie¿ tego nie nosisz!\n\r" , ch );
		return;
	}



	if ( ( second_ob = get_obj_carry( ch, arg2, ch ) ) == NULL )
	{
		send_to_char( "Przecie¿ nie nosisz tego!!!\n\r", ch );
		return;
	}

	if ( first_ob == second_ob )
	{
		send_to_char( "Potrzebujesz dwóch czê¶ci do po³±czenia.\n\r", ch );
		return;
	}

	if ( first_ob->item_type != ITEM_PIECE || second_ob->item_type != ITEM_PIECE )
	{
		send_to_char( "Oba przedmioty musz± byæ czê¶ciami innych przedmiotów!\n\r", ch );
		return;
	}

	/* check to see if the pieces connect */

	if ( ( first_ob->value[ 0 ] == second_ob->pIndexData->vnum )
		 || ( first_ob->value[ 1 ] == second_ob->pIndexData->vnum ) )
	{
		if ( get_obj_index( first_ob->value[ 2 ] ) == NULL )
		{
			send_to_char( "Jako¶ jednak nie uda³o ci siê tych przedmiotów po³±czyæ.\n\r", ch );
			return;
		}

		new_ob = create_object( get_obj_index( first_ob->value[ 2 ] ), FALSE );
		/*artefact*/
		if ( is_artefact( first_ob ) ) extract_artefact( first_ob );
		extract_obj( first_ob );
		/*artefact*/
		if ( is_artefact( second_ob ) ) extract_artefact( second_ob );
		extract_obj( second_ob );
		/*artefact*/
		if ( is_artefact( new_ob ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
		{
                        if(!arte_can_load(first_ob->value[2])) //gdy istenieje juz maks
                        {
                            act( "$n usi³uje dopasowaæ czê¶ci do siebie, po chwili s³ychaæ cichy trzask kiedy czê¶ci ³±cz± siê ze sob± tworz±c $p. Nagle przedmiot twojej uwagi zaczyna rozplywac siê, a¿ znika zupe³nie, zdajesz sobie sprawe, ¿e gdzie¶ istnieje ju¿ ten artefakt i tylko chwile pojawi³ siê w twoich d³oniach mami±c cie swoj± moc±.", ch, new_ob, NULL, TO_ROOM );
			    act( "Usi³ujesz dopasowaæ czê¶ci do siebie, po chwili s³ychaæ cichy trzask kiedy czê¶ci ³±cz± siê ze sob± tworz±c $p. Nagle przedmiot ten zaczyna rozplywaæ siê, a¿ znika zupe³nie.", ch, new_ob, NULL, TO_CHAR );
                            extract_obj(new_ob);
                            return;
                        }
                        create_artefact(first_ob->value[2]);
                        artefact_to_char( new_ob, ch );
                }
        new_ob->rent_cost = RENT_COST( new_ob->cost );
		obj_to_char( new_ob , ch );

		if ( !op_connect_trigger( new_ob, ch ) )
		{
			act( "$n usi³uje dopasowaæ czê¶ci do siebie, po chwili s³ychaæ cichy trzask kiedy czê¶ci ³±cz± siê ze sob± tworz±c $p.", ch, new_ob, NULL, TO_ROOM );
			act( "Usi³ujesz dopasowaæ czê¶ci do siebie, po chwili s³ychaæ cichy trzask kiedy czê¶ci ³±cz± siê ze sob± tworz±c $p.", ch, new_ob, NULL, TO_CHAR );
		}
	}
	else
	{
		act( "$n usi³uje dopasowaæ czê¶ci do siebie, jednak po chwili rezygnuje.", ch, new_ob, NULL, TO_ROOM );
		act( "Usi³ujesz dopasowaæ czê¶ci do siebie, jednak po chwili rezygnujesz.", ch, new_ob, NULL, TO_CHAR );
		return;
	}

	return;
}


void do_drag( CHAR_DATA * ch, char * argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	char dir[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	CHAR_DATA *vch;
	EXIT_DATA* pexit;
	int door = -1;
	int iter;

	argument = one_argument ( argument, arg );
	one_argument ( argument, dir );

	obj = get_obj_list( ch, arg, ch->in_room->contents );

	if ( !obj )
	{
		send_to_char ( "Nic takiego tu nie ma... chyba.\n\r", ch );
		return;
	}

	if ( !IS_SET( obj->wear_flags, ITEM_TAKE ) && !IS_OBJ_STAT( obj, ITEM_DRAGGABLE ) )
	{
		act( "Próbujesz ci±gn±æ $p, ale niestety nie udaje ci siê.", ch, obj, NULL, TO_CHAR );
		act( "$n próbuje ruszyæ $p, ale to ani drgnie.", ch, obj, NULL, TO_ROOM );
		return;
	}

	if ( obj->in_room != NULL )
	{
		for ( vch = obj->in_room->people; vch != NULL; vch = vch->next_in_room )
			if ( vch->on == obj )
			{
				act( "Wyglada na to, ¿e $N u¿ywa $h.", ch, obj, vch, TO_CHAR );
				return;
			}
	}


	if ( get_obj_weight( obj ) > ( 5 * can_carry_w ( ch ) / 2 ) )
	{
		act( "$p jest chyba dla ciebie za ciê¿kie, nie dasz rady.", ch, obj, NULL, TO_CHAR );
		act( "$n probuje ruszyæ $p, ale kapituluje.", ch, obj, NULL, TO_ROOM );
		return;
	}

	if ( dir[ 0 ] == '\0' )
	{
		send_to_char ( "Gdzie dok³adnie chcesz to zaci±gn±æ?\n\r", ch );
		return;
	}

	door = get_door( ch, argument );

	if ( door >= 0 && check_vname( ch, door, TRUE ) )
		return;

	if ( door >= 0 )
	{
		move_char( ch, door, FALSE, obj );
		return;
	}
	else
	{
		door = -1;
		for ( iter = 0; iter <= 5; iter++ )
		{
			if ( ( pexit = ch->in_room->exit[ iter ] ) != NULL
				 && !str_prefix( argument, pexit->vName )
				 && !( ( IS_SET( pexit->exit_info, EX_SECRET ) ||
				         IS_SET( pexit->exit_info, EX_HIDDEN ) ) &&
				       !IS_AFFECTED( ch, AFF_DETECT_HIDDEN ) ) )
			{
				door = iter;
				break;
			}
		}
	}

	if ( door < 0 || ch->in_room->exit[ door ] == NULL )
	{
		send_to_char( "Nie mo¿esz isc w tym kierunku.\n\r", ch );
		return;
	}
	move_char( ch, door, FALSE, obj );
	return;
}

void do_display( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj;

    if ( IS_AFFECTED( ch, AFF_HIDE ) )
        affect_strip( ch, gsn_hide );

    strip_invis( ch, TRUE, TRUE );

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
        act( "$n pokazuje ci co¶, ale nie bardzo wiadomo co.", ch, NULL, NULL, TO_ROOM );
        print_char( ch, "Co chcia³<&/a/o>by¶ pokazaæ?\n\r" );
        return;
    }

    if ( is_number( arg ) )
    {
        int nomination, amount = atoi( arg );
        argument = one_argument( argument, arg );

        if ( amount <= 0 )
        {
            act( "$n próbuje co¶ zrobiæ, ale nie bardzo wiesz co.", ch, NULL, NULL, TO_ROOM );
            send_to_char( "Przykro mi, nie mo¿esz tego zrobic.\n\r", ch );
            return;
        }

        nomination = money_nomination_find ( arg );

        switch ( nomination )
        {
            case NOMINATION_COPPER:
                if ( ch->copper < amount )
                {
                    act( "$n próbuje co¶ zrobiæ, ale nie bardzo wiesz co.", ch, NULL, NULL, TO_ROOM );
                    send_to_char( "Nie masz tylu miedzianych monet.\n\r", ch );
                    return;
                }
                sprintf ( buf, "%s", ( amount > 1 )? "miedziane":"miedziana" );
                break;
            case NOMINATION_SILVER:
                if ( ch->silver < amount )
                {
                    act( "$n próbuje co¶ zrobiæ, ale nie bardzo wiesz co.", ch, NULL, NULL, TO_ROOM );
                    send_to_char( "Nie masz tylu srebrnych monet.\n\r", ch );
                    return;
                }
                sprintf ( buf, "%s", ( amount > 1 )? "srebrne":"srebrnych" );
                break;
            case NOMINATION_GOLD:
                if ( ch->gold < amount )
                {
                    act( "$n próbuje co¶ zrobiæ, ale nie bardzo wiesz co.", ch, NULL, NULL, TO_ROOM );
                    send_to_char( "Nie masz tylu zlotych monet.\n\r", ch );
                    return;
                }
                sprintf ( buf, "%s", ( amount > 1 )? "zlote":"zlota" );
                break;
            case NOMINATION_MITHRIL:
                if ( ch->mithril < amount )
                {
                    act( "$n próbuje co¶ zrobiæ, ale nie bardzo wiesz co.", ch, NULL, NULL, TO_ROOM );
                    send_to_char( "Nie masz tylu mithrilowych monet.\n\r", ch );
                    return;
                }
                sprintf ( buf, "%s", ( amount > 1 )? "mithrilowe":"mithrilowa" );
                break;
        }

        switch ( amount )
        {
            case 1:
                print_char( ch, "Pokazujesz wszystkim jedn±, %s monete.\n\r", buf );
                sprintf ( buf, "$n pokazuje ci jedn±, %s monete.", buf );
                act( buf, ch, NULL, NULL, TO_ROOM );
                break;
            case 2:
            case 3:
            case 4:
                print_char( ch, "Pokazujesz wszystkim %d %s monety.\n\r", amount, buf );
                sprintf( buf, "$n pokazuje ci %d %s monety.", amount, buf );
                act( buf, ch, NULL, NULL, TO_ROOM );
                break;
            default:
                switch ( nomination )
                {
                    case NOMINATION_COPPER:
                        print_char( ch, "Pokazujesz wszystkim %d miedzianych monet.\n\r", amount );
                        sprintf( buf, "$n pokazuje ci %d miedzianych monet.", amount );
                        break;
                    case NOMINATION_SILVER:
                        print_char( ch, "Pokazujesz wszystkim %d srebrnych monet.\n\r", amount );
                        sprintf( buf, "$n pokazuje ci %d srebrnych monet.", amount );
                        break;
                    case NOMINATION_GOLD:
                        print_char( ch, "Pokazujesz wszystkim %d zlotych monet.\n\r", amount );
                        sprintf( buf, "$n pokazuje ci %d zlotych monet.", amount );
                        break;
                    case NOMINATION_MITHRIL:
                        print_char( ch, "Pokazujesz wszystkim %d mithrilowych monet.\n\r", amount );
                        sprintf( buf, "$n pokazuje ci %d mithrilowych monet.", amount );
                        break;
                    default:
                        send_to_char( "Masz chyba dziurawa sakiewke.\n\r", ch );
                        return;
                }
                act( buf, ch, NULL, NULL, TO_ROOM );
                break;
        }
        return;
    }
    else
    {
		if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
		{
			act( "$n próbuje co¶ zrobiæ, ale nie bardzo wiadomo co.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
			return;
		}
		act( "$n pokazuje wszystkim $h.", ch, obj, NULL, TO_ROOM );
		act( "Pokazujesz wszystkim $h.", ch, obj, NULL, TO_CHAR );
		return;
        /*
        act( "$n pokazuje ci co¶, ale nie bardzo wiadomo co.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "Przykro mi, nie mo¿esz tego zrobiæ.\n\r", ch );
        return;
        */
    }

    /* aby przywrocic display all, odkomentuj ponizsze, oraz wywal ELSE z ifa powyzej */
    /*
	if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
	{
		if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
		{
			act( "$n próbuje co¶ zrobiæ, ale nie bardzo wiadomo co.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
			return;
		}
		act( "$n pokazuje wszystkim $h.", ch, obj, NULL, TO_ROOM );
		act( "Pokazujesz wszystkim $h.", ch, obj, NULL, TO_CHAR );
		return;
	}
	else
	{
		bool found = FALSE;
		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( ( arg[ 3 ] == '\0' || is_name( &arg[ 4 ], obj->name ) )
				 && can_see_obj( ch, obj )
				 && obj->wear_loc == WEAR_NONE )
			{
				found = TRUE;

				act( "$n pokazuje ci $h.", ch, obj, NULL, TO_ROOM );
				act( "Pokazujesz wszystkim $h.", ch, obj, NULL, TO_CHAR );
			}
		}

		if ( !found )
		{
			if ( arg[ 3 ] == '\0' )
				act( "Nie nosisz nic przy sobie.", ch, NULL, arg, TO_CHAR );
			else
				act( "Nie masz przy sobie czegos takiego jak $T.", ch, NULL, &arg[ 4 ], TO_CHAR );
			act( "$n próbuje co¶ zrobiæ, ale nie bardzo wiadomo co.", ch, NULL, NULL, TO_ROOM );
		}
	}
	return;
	*/
}

void do_use( CHAR_DATA *ch, char *argument )
{
	PROG_LIST * prg;
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim = NULL;
	OBJ_DATA *obj, *obj_target = NULL;
	char *string = NULL;
	bool tar_obj = FALSE, off = FALSE;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Co chcia³<&/a/o>by¶ u¿yæ?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_wear( ch, arg, FALSE ) ) == NULL )
	{
		send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
		return;
	}

	if ( !HAS_OTRIGGER( obj, TRIG_USE ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}


	for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
		if ( prg->trig_type == &TRIG_USE )
		{
			string = prg->trig_phrase;
			break;
		}

	if ( !string )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}


	if ( !str_cmp( string, "self" ) )
	{
		if ( argument[ 0 ] != '\0' )
		{
			if ( ( victim = get_char_room ( ch, argument ) ) == NULL )
			{
				send_to_char( "Ten przedmiot mo¿esz u¿yæ tylko na sobie.\n\r", ch );
				return;
			}

			if ( victim != ch )
			{
				send_to_char( "Ten przedmiot mo¿esz u¿yæ tylko na sobie.\n\r", ch );
				return;
			}
		}
		victim = ch;
	}

	else if ( !str_cmp( string, "off" ) )
	{
		if ( argument[ 0 ] == '\0' )
		{
			if ( ch->fighting != NULL )
			{
				victim = ch->fighting;
			}
			else
			{
				send_to_char( "Kto ma byæ celem?\n\r", ch );
				return;
			}
		}
		else
		{
			if ( ( victim = get_char_room ( ch, argument ) ) == NULL )
			{
				send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
				return;
			}
		}

		if ( is_safe( ch, victim ) && victim != ch )
		{
			send_to_char( "Na ten cel nie da rady.\n\r", ch );
			return;
		}

		if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
		{
			send_to_char( "Przecieæ to twój przyjaciel!\n\r", ch );
			return;
		}

		off = TRUE;
	}

	else if ( !str_cmp( string, "def" ) )
	{
		if ( ( victim = get_char_room ( ch, argument ) ) == NULL )
		{
			send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
			return;
		}
	}

	else if ( !str_cmp( string, "obj" ) )
	{
		if ( ( obj_target = get_obj_carry( ch, argument, ch ) ) == NULL )
		{
			send_to_char( "Nie masz tego przy sobie.\n\r", ch );
			return;
		}
		tar_obj = TRUE;
	}
	else
	{
		victim = ch;
	}

	if ( ( tar_obj && !obj_target ) || ( !tar_obj && !victim ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	set_supermob( obj );

	if ( tar_obj )
	{
		create_oprog_env( prg->name, prg->code, obj, ch, obj_target, NULL, &TRIG_USE, NULL );
		program_flow();
	}
	else
	{
		create_oprog_env( prg->name, prg->code, obj, ch, NULL, victim, &TRIG_USE, NULL );
		program_flow();
	}

	release_supermob();

	if ( off && !victim->fighting && can_see( victim, ch ) )
		multi_hit( victim, ch, TYPE_UNDEFINED );

	if ( ch->wait <= 0 )
		WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

	return;
}

void do_horn( CHAR_DATA *ch, char *argument )
{
	PROG_LIST * prg;
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	char *string = NULL;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "W co chcia³<&/a/o>by¶ zadmiæ?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_wear( ch, arg, FALSE ) ) == NULL )
	{
		send_to_char( "Nie masz takiej rzeczy przy sobie.\n\r", ch );
		return;
	}

	if ( !HAS_OTRIGGER( obj, TRIG_HORN ) )
	{
		send_to_char( "Nic siê nie sta³o...\n\r", ch );
		return;
	}


	for ( prg = obj->pIndexData->progs; prg != NULL; prg = prg->next )
		if ( prg->trig_type == &TRIG_HORN )
		{
			string = prg->trig_phrase;
			break;
		}

	if ( !string )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}


	set_supermob( obj );
	create_oprog_env( prg->name, prg->code, obj, ch, NULL, NULL, &TRIG_HORN, string );
	program_flow();
	release_supermob();


	if ( ch->wait <= 0 )
		WAIT_STATE( ch, 1 * PULSE_VIOLENCE );

	return;
}

/*trofeum - sluzy wydobywaniu z ciala PC jakiegos kawalka ciala jako dowodu zabicia*/
void do_ucho(CHAR_DATA *ch, char *argument)
{
/*
    char arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *corpse=NULL, *skin=NULL;

    if ( ch->fighting )
    {
      send_to_char( "Lepiej skup sie na walce!\n\r", ch );
      return;
    }

    if ( ch->mount )
    {
      send_to_char( "Najpierw zsi±d¿ ze swojego wierzchowca.\n\r", ch );
      return;
    }

    if ( arg[ 0 ] == '\0' )
      {
         send_to_char( "Z czego chcesz wzi±ætrofeum?\n\r", ch );
         return;
      }

    if ( ( corpse = get_obj_list( ch, arg, ch->in_room->contents ) ) == NULL )
    {
      send_to_char( "Nie widzisz tu nic takiego.\n\r", ch );
      return;
    }

    if ( corpse->item_type == ITEM_CORPSE_NPC )
    {
       print_char( ch, "Z tego to ty trofeum chyba nie zdobedziesz.\n\r" );
       return;
    }

    if ( corpse->item_type != ITEM_CORPSE_PC )
    {
       print_char( ch, "Trofeum z tego? Masz dziwne pomys³y...\n\r" );
       return;
    }

  if ( corpse->value[ 4 ] == 2 )
  {
    print_char( ch, "Trofeum z tego cia³a zosta³o ju¿ zabrane.\n\r" );
    return;
  }

  if ( !get_obj_index( OBJ_VNUM_TROPHY ) )
    {
       print_char( ch, "Nic specjalnego siê nie dzieje.\n\r" );
       DEBUG_INFO( "do_trophy: Nie ma TROPHY  do za³adowania.\n\r" );
       return;
    }

    if ( ( skin = create_object( get_obj_index( OBJ_VNUM_TROPHY ), FALSE ) ) == NULL )
    {
       print_char( ch, "Nic siê nie sta³o.\n\r" );
       DEBUG_INFO( "do_skin: Nie ma TROPHY do za³adowania.\n\r" );
       return;
    }


  */

  /*
  TWRZENIE TROFEUM
  */


   // corpse->value[4] = 2;


    return;
}


void do_double_grip( CHAR_DATA *ch, char *argument )
{
	char arg [MAX_INPUT_LENGTH];
	OBJ_DATA *obj;

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Któr± broñ chcesz chwyciæ w obie rêce?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	{
		send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_WEAPON )
	{
		print_char( ch, "%s nie wygl±da na broñ, poszukaj czego¶ lepszego.\n\r", obj->short_descr );
		return;
	}

	if ( obj->value[0] == WEAPON_DAGGER || obj->value[0] == WEAPON_WHIP || obj->value[0] == WEAPON_CLAWS || obj->value[0] == WEAPON_SHORTSWORD )
	{
		send_to_char( "Tej broni nie dasz rady z³apaæ obiema rêkami.\n\r", ch );
		return;
	}

	if( IS_WEAPON_STAT( obj, WEAPON_TWO_HANDS ) )
	{
		wield_weapon( ch, obj, TRUE );
		return;
	}

	SET_BIT( obj->value[4], WEAPON_TWO_HANDS );
	EXT_SET_BIT( obj->extra_flags, ITEM_DOUBLE_GRIP );

	wield_weapon( ch, obj, TRUE );

	//jak nie moze wieldnac to od razu zabiera
	if( get_eq_char( ch, WEAR_WIELD ) != obj )
	{
		REMOVE_BIT( obj->value[4], WEAPON_TWO_HANDS );
    	EXT_REMOVE_BIT( obj->extra_flags, ITEM_DOUBLE_GRIP );
    }

    return;
}

void do_uncloak( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *cloak;

	cloak = get_eq_char( ch,WEAR_ABOUT );

	if( cloak == NULL || !IS_OBJ_STAT( cloak, ITEM_COVER ) )
	{
		send_to_char( "Nie masz na sobie maskuj±cej odzie¿y.\n\r", ch);
		return;
	}

	if ( IS_OBJ_STAT( cloak, ITEM_NOREMOVE ) || IS_OBJ_STAT( cloak, ITEM_NODROP ) )
	{
		act( "Nie mo¿esz zdj±æ $f.", ch, cloak, NULL, TO_CHAR );
		return;
	}

	if ( HAS_OTRIGGER( cloak, TRIG_PREREMOVE ) )
		op_common_trigger( ch, cloak, &TRIG_PREREMOVE );
	else
	{
		unequip_char( ch, cloak );
		act( "Szybkim ruchem rêki zrzucasz z siebie $h.", ch, cloak, NULL, TO_CHAR );
		act( "Szybkim ruchem rêki $n zrzuca z siebie $h.", ch, cloak, NULL, TO_ROOM );
		if ( HAS_OTRIGGER( cloak, TRIG_REMOVE ) )
			op_common_trigger( ch, cloak, &TRIG_REMOVE );

		if( !ch->in_room )
			return;

		/*artefact*/
		if ( !IS_IMMORTAL( ch ) && is_artefact( cloak ) && !IS_NPC( ch ) )
			artefact_from_char( cloak, ch );
		if ( cloak->contains != NULL && !IS_IMMORTAL( ch ) && !IS_NPC( ch ) )
			container_artefact_check( cloak, ch, 1 );

		obj_from_char( cloak );
		obj_to_room( cloak, ch->in_room );
		cloak->on_ground = 15;

		obj_trap_handler( ch, cloak, TRAP_DROP );

		if ( HAS_OTRIGGER( cloak, TRIG_DROP ) )
			op_common_trigger( ch, cloak, &TRIG_DROP );


		if ( obj_fall_into_water( ch, cloak ) )
            return;

        if (
                IS_OBJ_STAT( cloak, ITEM_MELT_DROP )
                || cloak->material == 45 //eteryczny
           )
		{
			if ( cloak->liczba_mnoga )
				act( "$p rozp³ywaj± siê w dym.", ch, cloak, NULL, TO_ALL );
			else
				act( "$p rozp³ywa siê w dym.", ch, cloak, NULL, TO_ALL );

			if ( is_artefact( cloak ) ) extract_artefact( cloak );
			if ( cloak->contains ) extract_artefact_container( cloak );
			extract_obj( cloak );
		}
		else if ( IS_SET( material_table[ cloak->material ].flag, MAT_EASYBREAK )
				  && number_percent() > material_table[ cloak->material ].hardness )
		{
			if ( cloak->liczba_mnoga )
				act( "$p rozpryskuj± siê na kawa³eczki.", ch, cloak, NULL, TO_ALL );
			else
				act( "$p rozpryskuje siê na kawa³eczki.", ch, cloak, NULL, TO_ALL );

			if ( is_artefact( cloak ) ) extract_artefact( cloak );
			if ( cloak->contains ) extract_artefact_container( cloak );
			extract_obj( cloak );
		}
	}
	return;
}
// Wtykanie pochodni w ziemie zeby oswietlaly
void do_illuminate( CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char buf1[MAX_STRING_LENGTH];
	bool pomin;
	OBJ_DATA *obj;

	if ( ch->fighting )
	{
		send_to_char( "Lepiej skup siê na walce.\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Czym chcesz o¶wietlaæ?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	{
		send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_LIGHT )
	{
		send_to_char( "Spróbuj tej sztuczki z czym¶ co o¶wietla.\n\r", ch );
		return;
	}

	if ( !can_drop_obj( ch, obj ) )
	{
		send_to_char( "Niestety nie mo¿esz siê tego pozbyæ.\n\r", ch );
		return;
	}
	//  Jak mozna zamocowac swiatlo zeby oswietlalo lokacje
	if ( obj->value[1] == LIGHT_NONE )
	{
		act( "Nijak nie potrafisz umocowaæ $f", ch, obj, NULL, TO_CHAR );
		return;
	}
	if ( IS_SET( obj->value[1], LIGHT_LIE ) )
	{
		pomin = FALSE;
		switch ( ch->in_room->sector_type )
		{
		case 4: //wzgorza
		case 5: //gory
		case 13: //wys. gory
		case 43: //stroma sciezka
			{
				sprintf( buf, "Próbujesz po³o¿yæ $h na nierównym terenie ale za ka¿dym razem stacza siê w dó³." );
				sprintf( buf1, "$n próbuje po³o¿yæ $h na nierównej ziemi ale nie bardzo mu siê to udaje.");
				pomin = TRUE;
			} break;
		case 3: //las
		case 11: //puszcza
		case 38: //park
			{
				sprintf( buf, "Próbujesz po³o¿yæ $h ale zahaczasz o korzeñ, jeszcze wywo³asz po¿ar!" );
				sprintf( buf1, "$n próbuje po³o¿yæ $h ale zahacza o korzeñ o ma³o co nie wywo³uj±c po¿aru." );
				pomin = TRUE;
			} break;
		case 6: //woda
		case 7: //woda
		case 19: //rzeka
		case 20: //jezioro
		case 21: //morze
		case 22: //ocean
		case 34: //pod woda
		case 37: //ciemna woda
		case 39: //podziemne jezioro

			{
				sprintf( buf, "W wodzie? Kiepski pomys³." );
				sprintf( buf1, "$n próbuje po³o¿yæ $h ale nie jest to dobry pomys³." );
				pomin = TRUE;
			} break;
		case 9: //powietrze
			{
				sprintf( buf, "Niby gdzie chcesz po³o¿yæ $h?" );
				sprintf( buf1, "$n przez chwilê zastanawia siê gdzie by tu mo¿na po³o¿yæ $h.");
				pomin = TRUE;
			} break;
		}
		if ( !pomin )
		{
			act( "Ostro¿nie k³adziesz $h na ziemi.", ch, obj, NULL, TO_CHAR );
			act( "$n ostro¿nie k³adzie $h na ziemi.", ch, obj, NULL, TO_ROOM );
			obj_from_char( obj );
			obj_to_room( obj, ch->in_room );
			obj->value[4] = obj->value[2]; //znaczy jest zapalona
			if ( obj->value[4] != 0 ) ++ch->in_room->light;
			return;
		}
	}
	if ( IS_SET( obj->value[1], LIGHT_STICK ) )
	{
		pomin = FALSE;
		switch ( ch->in_room->sector_type )
		{
		case 0: //wewnatrz
		case 1: //miasto
		case 14: //podziemia
			{
				sprintf( buf, "Nie bardzo udaje ci siê wbiæ tutaj $h." );
				sprintf( buf1, "$n próbuje wbiæ $h ale nie bardzo mu siê to udaje." );
				pomin = TRUE;
			} break;
		case 5: //gory
		case 13: //wysokie gory
		case 15: //jaskinia
			{
				sprintf( buf, "Próbujesz wbiæ $h w skaliste pod³o¿e ale okazuje siê to niemo¿liwe." );
				sprintf( buf1, "$n próbuje wbiæ $h w skaliste pod³o¿e ale okazuje siê to niemo¿liwe." );
				pomin = TRUE;
			} break;
		case 6: //woda
		case 7: //woda
		case 19: //rzeka
		case 20: //jezioro
		case 21: //morze
		case 22: //ocean
		case 34: //pod woda
		case 37: //ciemna woda
		case 39: //podziemne jezioro
			{
				sprintf( buf, "W wodzie? Kiepski pomys³." );
				sprintf( buf1, "$n próbuje wbiæ $p ale nie siêga dna." );
				pomin = TRUE;
			} break;
		case 9: //powietrze
			{
				sprintf( buf, "Niby gdzie chcesz wbiæ $h?" );
				sprintf( buf1, "$n przez chwilê zastanawia siê gdzie by tu mo¿na wbiæ $h." );
				pomin = TRUE;
			} break;
		}
		if ( !pomin )
		{
			act( "Zdecydowanym ruchem wbijasz $h w pod³o¿e.", ch, obj, NULL, TO_CHAR );
			act( "$n zdecydowanym ruchem wbija $h w pod³o¿e.", ch, obj, NULL, TO_ROOM );
			obj_from_char( obj );
			obj_to_room( obj, ch->in_room );
			obj->value[4] = obj->value[2]; //znaczy jest zapalona
			if ( obj->value[4] != 0 ) ++ch->in_room->light;
			return;
		}
	}
	if ( IS_SET( obj->value[1], LIGHT_HANG ) )
	{
		pomin = FALSE;
		switch ( ch->in_room->sector_type )
		{
		case 2: //pole
		case 4: //wzgorza
		case 10: //pustynia
		case 18: //droga
		case 23: //lawa
		case 24: //ruchome piaski
		case 29: //plaza
		case 31: //sawanna
		case 32: //trawa
		case 33: //step
		case 36: //plac
		case 40: //wydmy
		case 42: //laka
		case 46: //pustynna droga
			{
				sprintf( buf, "Nie bardzo gdzie jest powiesiæ $h." );
				sprintf( buf1, "$n rozglada siê gdzie by tu powiesiæ $h." );
				pomin = TRUE;
			} break;
		case 6: //woda
		case 7: //woda
		case 19: //rzeka
		case 20: //jezioro
		case 21: //morze
		case 22: //ocean
		case 34: //pod woda
		case 37: //ciemna woda
		case 39: //podziemne jezioro
			{
				sprintf( buf, "W wodzie? Kiepski pomys³." );
				sprintf( buf1, "Z rozbawieniem zauwa¿asz, ¿e $n próbuje powiesiæ gdzie¶ $h." );
				pomin = TRUE;
			} break;
		case 9: //powietrze
			{
				sprintf( buf, "Niby gdzie chcesz powiesiæ $h?" );
				sprintf( buf1, "$n przez chwilê zastanawia siê gdzie by tu mo¿na powiesiæ $h." );
				pomin = TRUE;
			} break;
		}
		if ( !pomin )
		{
			act( "Ostro¿nie wieszasz $h.", ch, obj, NULL, TO_CHAR );
			act( "$n ostro¿nie wiesza $h.", ch, obj, NULL, TO_ROOM );
			obj_from_char( obj );
			obj_to_room( obj, ch->in_room );
			obj->value[4] = obj->value[2]; //znaczy jest zapalona
			if ( obj->value[4] != 0 ) ++ch->in_room->light;
			return;
		}
	}
	act( buf, ch, obj, NULL, TO_CHAR );
	act( buf1, ch, obj, NULL, TO_ROOM );
	return;
}

//Odwracanie porz±dku kilku identycznych rzeczy w inventory
void do_reverse( CHAR_DATA *ch, char *argument)
{
	OBJ_DATA *obj;
	char arg[MAX_INPUT_LENGTH];
	const int max_items = 99;
	OBJ_DATA *tmp[99]; //wiem... kiepskie rozwi±zanie, nale¿a³oby zrobiæ listê ale w 99,9999% przypadków bêdzie ok wiêc olewam :)
	OBJ_DATA *prev;
	OBJ_DATA *last;
	int licznik;
	int i;

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Porz±dek czego chcesz odwróciæ?\n\r", ch );
		return;
	}

	if ( get_obj_carry( ch, arg, ch ) == NULL )
	{
		send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
		return;
	}
	prev = ch->carrying;
	last = prev;
	licznik = 0;
	for ( obj = ch->carrying; obj; obj = obj->next_content )
	{
		if ( !obj )
		{
			break;
		}
		if ( obj->wear_loc == WEAR_NONE
				&&   can_see_obj( ch, obj )
				&&   is_name( arg, obj->name ) )
		{
			tmp[licznik] = obj;
			if ( obj == ch->carrying )
			{
				ch->carrying = obj->next_content;
			} else
			{
				prev->next_content = obj->next_content;
			}
			licznik++;
		} else
		{
			last = obj;
			prev = obj;
		}
		if ( licznik == max_items )
		{
			send_to_char( "Masz za du¿o rzeczy tego typu.\n\r", ch );
			return;
		}
	}
	for ( i = licznik-1; i>=0; i-- )
	{
		last->next_content = tmp[i];
		last = last->next_content;
	}
	last->next_content = NULL;
	send_to_char( "Kolejno¶æ zosta³a odwrócona.\n\r", ch );
	return;
}

/**
 *
 * Written by esnible@goodnet.com
 * Wandering Sage Code
 *
 */
void do_identify( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    CHAR_DATA *rch;
    char arg1[ MAX_INPUT_LENGTH ];
    argument = one_argument( argument, arg1 );

    if ( arg1[ 0 ] == '\0' )
    {
        send_to_char( "Co chcesz zidentyfikowaæ?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry( ch, arg1 , ch ) ) == NULL )
    {
        send_to_char( "Nie masz czego¶ takiego.\n\r", ch );
        return;
    }

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
        if (IS_NPC(rch) && EXT_IS_SET( rch->act, ACT_SAGE ))
        {
            break;
        }
    }

    if (!rch)
    {
        send_to_char("Nikt tutaj ci nie pomo¿e.\n\r", ch);
        return;
    }

    if ( ch->gold < SAGE_PRICE )
    {
        act( "$n nic sobie nie robi z zaczepek $Z i nawet nie patrzy na $p.", rch, obj, ch, TO_NOTVICT );
        act( "$n nie zwraca na ciebie uwagi.", rch, obj, ch, TO_VICT );
        return;
    }
    else
    {
        ch->gold -= SAGE_PRICE;
        act( "$N zrêcznym ruchem uszczupla twoj± sakiewkê.", ch, NULL, rch, TO_CHAR );
    }

    act( "$n czule dotyka $h po czym rozrzuca drobne, ptasie ko¶ci.", rch, obj, 0, TO_ROOM );
    spell_identify( 0, 0, ch, obj, TARGET_SAGE );
}

