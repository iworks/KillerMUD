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
 * $Id: act_enter.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/act_enter.c $
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
#include "traps.h"


/* random room generation procedure */
ROOM_INDEX_DATA  *get_random_room(CHAR_DATA *ch)
{
	ROOM_INDEX_DATA *room;

	for ( ; ; )
	{
		room = get_room_index( number_range( 0, 65535 ) );
		if ( room != NULL )
		if ( can_see_room( ch, room )
		&& !room_is_private( room )
		&& !EXT_IS_SET(room->room_flags, ROOM_PRIVATE )
		&& !EXT_IS_SET(room->room_flags, ROOM_SOLITARY )
		&& !EXT_IS_SET(room->room_flags, ROOM_SAFE )
		&& ( EXT_IS_SET( ch->act, ACT_AGGRESSIVE ) || !EXT_IS_SET( room->room_flags, ROOM_LAW ) ) )
			break;
	}

	return room;
}

/* RT Enter portals */
void do_enter( CHAR_DATA *ch, char *argument)
{
	ROOM_INDEX_DATA *location;

	if ( ch->fighting != NULL )
	return;

	/* nifty portal stuff */
	if (argument[0] != '\0')
	{
		ROOM_INDEX_DATA *old_room;
	OBJ_DATA *portal;
	CHAR_DATA *fch, *fch_next;

		old_room = ch->in_room;

	portal = get_obj_list( ch, argument,  ch->in_room->contents );

	if (portal == NULL)
	{
		send_to_char("Nie widzisz tego tutaj.\n\r",ch);
		return;
	}

	if (portal->item_type != ITEM_PORTAL
		||  (IS_SET(portal->value[1],EX_CLOSED) && !IS_TRUSTED(ch,ANGEL)))
	{
		send_to_char("Nie mo¿esz znale¼æ wej¶cia.\n\r",ch);
		return;
	}

	if (!IS_TRUSTED(ch,ANGEL) && !IS_SET(portal->value[2],GATE_NOCURSE)
	&&  (IS_AFFECTED(ch,AFF_CURSE)
	||   EXT_IS_SET(old_room->room_flags,ROOM_NO_RECALL)))
	{
		send_to_char("Co¶ nie pozwala ci odej¶æ...\n\r",ch);
		return;
	}

	if (IS_SET(portal->value[2],GATE_RANDOM) || portal->value[3] == -1)
	{
		location = get_random_room(ch);
		portal->value[3] = location->vnum; /* for record keeping :) */
	}
	else if (IS_SET(portal->value[2],GATE_BUGGY) && (number_percent() < 5))
		location = get_random_room(ch);
	else
		location = get_room_index(portal->value[3]);

	if (location == NULL
	||  location == old_room
	||  !can_see_room(ch,location)
	||  (room_is_private(location) && !IS_TRUSTED(ch,IMPLEMENTOR)))
	{
	   act("$p nigdzie nie prowadzi.",ch,portal,NULL,TO_CHAR);
	   return;
	}

		if ( EXT_IS_SET( ch->act, ACT_AGGRESSIVE ) && EXT_IS_SET( location->room_flags, ROOM_LAW ) )
		{
			send_to_char("Co¶ nie pozwala ci odej¶æ...\n\r",ch);
			return;
		}

	act("$n wchodzi w $h.",ch,portal,NULL,TO_ROOM);

	if (IS_SET(portal->value[2],GATE_NORMAL_EXIT))
		act("Wchodzisz w $h.", ch, portal, NULL, TO_CHAR);
	else
		act("Wchodzisz w  $h i nagle jeste¶ gdzie¶ indziej...", ch, portal, NULL, TO_CHAR);

	char_from_room(ch);
	char_to_room(ch, location);

	if (IS_SET(portal->value[2],GATE_GOWITH)) /* take the gate along */
	{
		obj_from_room(portal);
		obj_to_room(portal,location);
	}

	if (IS_SET(portal->value[2],GATE_NORMAL_EXIT))
		if (ch->sex == 0) {
			act("$n pojawi³o sie.",ch,portal,NULL,TO_ROOM);
		}
		else if (ch->sex == 1) {
			act("$n pojawi³ sie.",ch,portal,NULL,TO_ROOM);
		}
		else {
			act("$n pojawi³a sie.",ch,portal,NULL,TO_ROOM);
		}
	else
		act("$n wychodzi z $f.",ch,portal,NULL,TO_ROOM);


	room_trap_handler( ch );

	if( !ch->in_room )
		return;

		if(!IS_NPC(ch))
		rp_enter_trigger(ch);

	if(!ch->in_room)
		return;

/*		if ( IS_SET( sector_table[ location->sector_type ].flag, SECT_UNDERWATER ))
		{
			send_to_char( "Przestajesz unosiæ siê w powietrzu.\n\r", ch );
			affect_strip(ch, 52);
		}
*/
	do_function(ch, &do_look, "auto");

	/* charges */
	if (portal->value[0] > 0)
	{
		portal->value[0]--;
		if (portal->value[0] == 0)
		portal->value[0] = -1;
	}

/* protect against circular follows */
	if (old_room == location)
		return;

		for ( fch = old_room->people; fch != NULL; fch = fch_next )
		{
			fch_next = fch->next_in_room;

			if (portal == NULL || portal->value[0] == -1)
		/* no following through dead portals */
				continue;

			if ( fch->master == ch && IS_AFFECTED(fch,AFF_CHARM)
			&&   fch->position < POS_STANDING)
				do_function(fch, &do_stand, "");

			if ( fch->master == ch && fch->position == POS_STANDING)
			{

				if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_LAW ) && EXT_IS_SET( fch->act, ACT_AGGRESSIVE ) ) {
					act("Nie mo¿esz sprowadziæ $C do miasta.", ch,NULL,fch,TO_CHAR);
					act("Nie masz pozwolenia na przeniesienie siê do miasta.", fch,NULL,NULL,TO_CHAR);
					continue;
				}
				if(IS_AFFECTED(fch, AFF_FLYING))
				{
					act( "Lecisz za $V.", fch, NULL, ch, TO_CHAR );
				}
				else if(IS_AFFECTED(fch, AFF_FLOAT))
				{
					act( "Lewitujesz za $V.", fch, NULL, ch, TO_CHAR );
				}
				else
				{
					act( "Idziesz za $V.", fch, NULL, ch, TO_CHAR );
				}
				do_function(fch, &do_enter, argument);
			}
		}

	 if (portal != NULL && portal->value[0] == -1)
	{
		act("$p znika.",ch,portal,NULL,TO_CHAR);
		if (ch->in_room == old_room)
		act("$p znika.",ch,portal,NULL,TO_ROOM);
		else if (old_room->people != NULL) {
			act("$p znika.", old_room->people,portal,NULL,TO_CHAR);
			act("$p znika.", old_room->people,portal,NULL,TO_ROOM);
		}
		/*artefact*/
		if( is_artefact(portal) ) extract_artefact(portal);
		extract_obj(portal);
	}

	/*
	 * If someone is following the char, these triggers get activated
	 * for the followers before the char, but it's safer this way...
	 */
	if ( IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_ENTRY ) )
		mp_percent_trigger( ch, NULL, NULL, NULL, &TRIG_ENTRY );

	/* jesli ktos go zalatwil w progu*/
	if(!ch->in_room)
		return;

	if ( !IS_NPC( ch ) && can_move( ch ) && !ch->fighting )
		mp_greet_trigger( ch );
	/*roomprog tylko na graczy*/


/*	if(!IS_NPC(ch))
	rp_enter_trigger(ch);*/

		return;
	}

	send_to_char("Nie, nie da rady.\n\r",ch);
	return;
}
