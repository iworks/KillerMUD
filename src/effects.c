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
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: effects.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/effects.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "progs.h"


#define DAMAGE_MULTIPLIER	33
/* funckja testujaca zniszczenie obiektu od roznego rodzaju ataku
 * zwraca true jesli obiekt zniszczy sie, else false
 */
bool check_item_resist( OBJ_DATA *obj, int type, int dam )
{
    int resist;

    switch ( type )
    {
        case RESIST_FIRE: resist = material_table[ obj->material ].fire_res;break;
        case RESIST_ACID: resist = material_table[ obj->material ].acid_res;break;
        case RESIST_ELECTRICITY: resist = material_table[ obj->material ].shock_res;break;
        case RESIST_COLD: resist = material_table[ obj->material ].cold_res;break;
        default: resist = material_table[ obj->material ].other_res;break;
    }

    if ( IS_OBJ_STAT( obj, ITEM_UNDESTRUCTABLE ) )
        return FALSE;

    if ( type == RESIST_SOUND && material_table[ obj->material ].flag != MAT_EASYBREAK )
    	return FALSE;

    if ( type == RESIST_FIRE && IS_OBJ_STAT( obj, ITEM_BURN_PROOF ))
 		return FALSE;

	if ( type == RESIST_FIRE && obj->item_type == ITEM_WEAPON && IS_SET( obj->value[ 4 ], WEAPON_FLAMING ) )
		return FALSE;

	if ( type == RESIST_ACID && obj->item_type == ITEM_WEAPON && IS_SET( obj->value[ 4 ], WEAPON_TOXIC ) )
		return FALSE;

	if ( type == RESIST_ELECTRICITY && obj->item_type == ITEM_WEAPON && IS_SET( obj->value[ 4 ], WEAPON_SHOCKING ) )
		return FALSE;

	if ( type == RESIST_COLD && obj->item_type == ITEM_WEAPON && IS_SET( obj->value[ 4 ], WEAPON_FROST ) )
		return FALSE;

	if ( type == RESIST_SOUND && obj->item_type == ITEM_WEAPON && IS_SET( obj->value[ 4 ], WEAPON_RESONANT ) )
		return FALSE;

    if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
        resist = URANGE( 0, resist + 15, 100 );

    if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
        resist = URANGE( 0, resist + 35, 100 );

    if ( ( obj->item_type == ITEM_DRINK_CON ||
           obj->item_type == ITEM_POTION ) &&
         type == RESIST_COLD )
        resist = URANGE( 0, resist + 20, 100 );
//	resist = 20; Brohacz: zmiana na += i URANGE;

    if ( ( obj->item_type == ITEM_WAND ||
           obj->item_type == ITEM_STAFF ) &&
         type == RESIST_ELECTRICITY )
        resist = URANGE( 0, resist + 20, 100 );
//	resist = 20; Brohacz: zmiana na += i URANGE;

    /* Tener: zmieniam odporno¶æ tego typu przedmiotów z 20 na 80 procent. Powód? S± magiczne! 
     * Rellik: papier magiczny pali siê tak samo dobrze jak zwyk³y... ;) */
//2008-11-20, Brohacz: to zalezy, czy ksiazka ma flage magic. Przywracam resist 20, nie ulatwiajmy zdobywania ksiag w pojedynke, a przeciez o to tu chodzi.
    if ( ( obj->item_type == ITEM_SPELLBOOK ||
           obj->item_type == ITEM_SCROLL ) &&
         type == RESIST_FIRE )
        resist = URANGE( 0, resist + 20, 100 );
//	resist = 20; Brohacz: zmiana na += i URANGE;

    dam = DAMAGE_MULTIPLIER * dam / 100;
    dam = ( 100 - resist ) * dam / 100;
    dam = URANGE( 0, dam, 100 );
    obj->condition -= dam;

    if ( obj->condition < 0 )
        return TRUE;

    return FALSE;
}

/* kwasowy efekt */
void acid_effect( void *vo, int level, int dam, int target )
{
    if ( target == TARGET_ROOM )
    {
        ROOM_INDEX_DATA * room = ( ROOM_INDEX_DATA * ) vo;
        OBJ_DATA *obj, *obj_next;

        for ( obj = room->contents; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );

            if ( IS_OBJ_STAT( obj, ITEM_NOPURGE ) )
                continue;

            acid_effect( obj, level, dam, TARGET_OBJ );
        }
        return ;
    }

    if ( target == TARGET_CHAR )   /* do the effect on a victim */
    {
        CHAR_DATA * victim = ( CHAR_DATA * ) vo;
        OBJ_DATA *obj, *obj_next;

        if ( !victim->in_room )
            return ;

		if ( is_affected(victim, gsn_mirrorfall) )
		{
			affect_strip(victim, gsn_mirrorfall);
			return;
		}

        /* brak sprawdzania czy zszedl */
        for ( obj = victim->carrying; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );
            acid_effect( obj, level, dam, TARGET_OBJ );
        }
        return ;
    }

    if ( target == TARGET_OBJ )  /* toast an object */
    {
        OBJ_DATA * obj = ( OBJ_DATA * ) vo;
        OBJ_DATA *t_obj, *n_obj;
        char *msg;

	/* Tener: dodanie szansy na unikniêcie w zale¿no¶ci od poziomu czaru [20080512] */
	//2008-11-20, Brohacz: wylaczam. Spelle powyzej 25 levela nie psuja przeciez przez to gratow!
	/*if ( URANGE( 1, number_range( 1, 30 - level ), 10 ) < 5 )
	   return;*/

        if ( !check_item_resist( obj, RESIST_ACID, dam ) )
            return ;

	if( obj->liczba_mnoga )
	{
	    switch ( obj->item_type )
            {
	        case ITEM_CONTAINER:
            	case ITEM_CORPSE_PC:
            	case ITEM_CORPSE_NPC:
	                msg = "$p sycz± i rozpuszczaj± siê.";
                	break;
            	case ITEM_ARMOR:
            	case ITEM_WEAPON:
	                msg = "$p skwiercz± i rozpadaj± siê.";
                	break;
            	case ITEM_CLOTHING:
	                msg = "$p czerniej± i krusz± siê.";
                	break;
            	case ITEM_STAFF:
            	case ITEM_WAND:
	                msg = "$p czerniej± i pêkaj±.";
                	break;
            	case ITEM_SCROLL:
	                msg = "$p rozpuszczaj± siê w k³ebach dymu.";
                	break;
            	default:
	                msg = "$p rozpuszczaj± siê w k³ebach dymu.";
                	break;
            }
        }
	else
	{
            switch ( obj->item_type )
            {
	        case ITEM_CONTAINER:
            	case ITEM_CORPSE_PC:
            	case ITEM_CORPSE_NPC:
	                msg = "$p syczy i rozpuszcza siê.";
                	break;
            	case ITEM_ARMOR:
            	case ITEM_WEAPON:
	                msg = "$p skwierczy i rozpada siê.";
                	break;
            	case ITEM_CLOTHING:
	                msg = "$p czernieje i kruszy siê.";
                	break;
            	case ITEM_STAFF:
            	case ITEM_WAND:
	                msg = "$p czernieje i pêka.";
                	break;
            	case ITEM_SCROLL:
	                msg = "$p rozpuszcza siê w k³ebach dymu.";
                	break;
            	default:
	                msg = "$p rozpuszcza siê w k³ebach dymu.";
                	break;
            }
        }



        if ( obj->carried_by != NULL )
            act( msg, obj->carried_by, obj, NULL, TO_ALL );
        else if ( obj->in_room != NULL && obj->in_room->people != NULL )
            act( msg, obj->in_room->people, obj, NULL, TO_ALL );

        /* get rid of the object */
        if ( obj->contains )   /* dump contents */
        {
            for ( t_obj = obj->contains; t_obj != NULL; t_obj = n_obj )
            {
                n_obj = t_obj->next_content;
                obj_from_obj( t_obj );

                if ( obj->in_room != NULL )
                    obj_to_room( t_obj, obj->in_room );

                else if ( obj->carried_by != NULL )
                    obj_to_room( t_obj, obj->carried_by->in_room );

                else
                {
                    /*artefact*/
                    if ( is_artefact( t_obj ) ) extract_artefact( t_obj );
                    if ( obj->contains ) extract_artefact_container( obj );
                    extract_obj( t_obj );
                    continue;
                }

                acid_effect( t_obj, level / 2, dam / 2, TARGET_OBJ );
            }
        }

        /*artefact*/
        if ( is_artefact( obj ) ) extract_artefact( obj );
        if ( obj->contains ) extract_artefact_container( obj );
        extract_obj( obj );
        return ;
    }
}


void cold_effect( void *vo, int level, int dam, int target )
{
    if ( target == TARGET_ROOM )  /* nail objects on the floor */
    {
        ROOM_INDEX_DATA * room = ( ROOM_INDEX_DATA * ) vo;
        OBJ_DATA *obj, *obj_next;

        for ( obj = room->contents; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );

            if ( IS_OBJ_STAT( obj, ITEM_NOPURGE ) )
                continue;

            cold_effect( obj, level, dam, TARGET_OBJ );
        }
        return ;
    }

    if ( target == TARGET_CHAR )  /* whack a character */
    {
        CHAR_DATA * victim = ( CHAR_DATA * ) vo;
        OBJ_DATA *obj, *obj_next;

        if ( !victim->in_room )
            return ;

		if ( is_affected(victim, gsn_mirrorfall) )
		{
			affect_strip(victim, gsn_mirrorfall);
			return;
		}

        /* let's toast some gear */
        for ( obj = victim->carrying; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );
            cold_effect( obj, level, dam, TARGET_OBJ );
        }
        return ;
    }

    if ( target == TARGET_OBJ )  /* toast an object */
    {
        OBJ_DATA * obj = ( OBJ_DATA * ) vo;
        OBJ_DATA *t_obj, *n_obj;
        char *msg;

        if( obj->liczba_mnoga )
        	msg = "$p zamarzaj± i rozpryskuj± siê na kawa³ki!";
        else
        	msg = "$p zamarza i rozpryskuje siê na kawa³ki!";

	/* Tener: dodanie szansy na unikniêcie w zale¿no¶ci od poziomu czaru [20080512] */
	//2008-11-20, Brohacz: wylaczam. Spelle powyzej 25 levela nie psuja przeciez przez to gratow!	
	/*if ( URANGE( 1, number_range( 1, 30 - level ), 10 ) < 5 )
	   return;*/

        if ( !check_item_resist( obj, RESIST_COLD, dam ) )
            return ;

        if ( obj->carried_by != NULL )
			act( msg, obj->carried_by, obj, NULL, TO_ALL );

        else if ( obj->in_room != NULL && obj->in_room->people != NULL )
            act( msg, obj->in_room->people, obj, NULL, TO_ALL );

        /* get rid of the object */
        if ( obj->contains )   /* dump contents */
        {
            for ( t_obj = obj->contains; t_obj != NULL; t_obj = n_obj )
            {
                n_obj = t_obj->next_content;
                obj_from_obj( t_obj );

                if ( obj->in_room != NULL )
                    obj_to_room( t_obj, obj->in_room );

                else if ( obj->carried_by != NULL )
                    obj_to_room( t_obj, obj->carried_by->in_room );

                else
                {
                    /*artefact*/
                    if ( is_artefact( t_obj ) ) extract_artefact( t_obj );
                    if ( obj->contains ) extract_artefact_container( obj );
                    extract_obj( t_obj );
                    continue;
                }

                cold_effect( t_obj, level / 2, dam / 2, TARGET_OBJ );
            }
        }

        /*artefact*/
        if ( is_artefact( obj ) ) extract_artefact( obj );
        if ( obj->contains ) extract_artefact_container( obj );
        extract_obj( obj );
        return ;
    }
}



void fire_effect( void *vo, int level, int dam, int target )
{
    if ( target == TARGET_ROOM )   /* nail objects on the floor */
    {
        ROOM_INDEX_DATA * room = ( ROOM_INDEX_DATA * ) vo;
        OBJ_DATA *obj, *obj_next;

        for ( obj = room->contents; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );

            if ( IS_OBJ_STAT( obj, ITEM_NOPURGE ) )
                continue;
            fire_effect( obj, level, dam, TARGET_OBJ );
        }
        return ;
    }

    if ( target == TARGET_CHAR )    /* do the effect on a victim */
    {
        CHAR_DATA * victim = ( CHAR_DATA * ) vo;
        OBJ_DATA *obj, *obj_next;

        if ( !victim->in_room )
            return ;

		if ( is_affected(victim, gsn_mirrorfall) )
		{
			affect_strip(victim, gsn_mirrorfall);
			return;
		}

        /* let's toast some gear! */
        for ( obj = victim->carrying; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );
            fire_effect( obj, level, dam, TARGET_OBJ );
        }
        return ;
    }

    if ( target == TARGET_OBJ )   /* toast an object */
    {
        OBJ_DATA * obj = ( OBJ_DATA * ) vo;
        OBJ_DATA *t_obj, *n_obj;
        char *msg;

	/* Tener: przeniesienie if'a w dobre miejsce [20080523] */
	//2008-11-20, Brohacz: wylaczam. Spelle powyzej 25 levela nie psuja przeciez przez to gratow!
	/*if ( URANGE( 1, number_range( 1, 30 - level ), 10 ) < 5 )
	   return;*/

        if ( !check_item_resist( obj, RESIST_FIRE, dam ) )
            return ;

        /*
          Gurthg
          2003-04-01
          powi±zanie levelu z szans± na zniszczenie grata
         */
 /*ale ten if przecie nie ma sensu - przepuszcza itemy o ujemnej kondycji - kainti 2006 07 10
        if ( URANGE( 1, number_range( 1, 30 - level ), 10 ) < 5 )
            return ;*/

		if(obj->liczba_mnoga)
		{
        	switch ( obj->item_type )
        	{
	            case ITEM_CONTAINER:
                	msg = "$p zapalaj± siê i p³on±!";
                	break;
            	case ITEM_POTION:
	                msg = "$p bulgocz± i eksploduj±!";
                	break;
            	case ITEM_SCROLL:
	                msg = "$p skwiercz± i spalaj± siê!";
                	break;
            	case ITEM_STAFF:
	                msg = "$p dymi± siê i czerniej±!";
                	break;
            	case ITEM_WAND:
	                msg = "$p iskrz± siê i top±!";
                	break;
            	case ITEM_FOOD:
	                msg = "$p czerniej± i krusz± sie!";
                	break;
            	case ITEM_PILL:
	                msg = "$p czerniej± i krusz± sie!";
                	break;
            	case ITEM_GEM:
	                msg = "$p rozgrzewaj± siê i eksploduj±!";
                	break;
            	default:
	                msg = "$p zapalaj± siê i p³on±!";
                	break;
        	}
        }
		else
		{
        	switch ( obj->item_type )
        	{
	            case ITEM_CONTAINER:
                	msg = "$p zapala siê i p³onie!";
                	break;
            	case ITEM_POTION:
	                msg = "$p bulgocze i eksploduje!";
                	break;
            	case ITEM_SCROLL:
	                msg = "$p skwierczy i spala siê!";
                	break;
            	case ITEM_STAFF:
	                msg = "$p dymi siê i czernieje!";
                	break;
            	case ITEM_WAND:
	                msg = "$p iskrzy siê i topi!";
                	break;
            	case ITEM_FOOD:
	                msg = "$p czernieje i kruszy sie!";
                	break;
            	case ITEM_PILL:
	                msg = "$p czernieje i kruszy sie!";
                	break;
            	case ITEM_GEM:
	                msg = "$p rozgrzewa siê i eksploduje!";
                	break;
            	default:
	                msg = "$p zapala siê i p³onie!";
                	break;
        	}
        }

        if ( obj->carried_by != NULL )
            act( msg, obj->carried_by, obj, NULL, TO_ALL );
        else if ( obj->in_room != NULL && obj->in_room->people != NULL )
            act( msg, obj->in_room->people, obj, NULL, TO_ALL );

        if ( obj->contains )
        {
            /* dump the contents */

            for ( t_obj = obj->contains; t_obj != NULL; t_obj = n_obj )
            {
                n_obj = t_obj->next_content;
                obj_from_obj( t_obj );
                if ( obj->in_room != NULL )
                    obj_to_room( t_obj, obj->in_room );
                else if ( obj->carried_by != NULL )
                    obj_to_room( t_obj, obj->carried_by->in_room );
                else
                {
                    /*artefact*/
                    if ( is_artefact( t_obj ) ) extract_artefact( t_obj );
                    if ( obj->contains ) extract_artefact_container( obj );
                    extract_obj( t_obj );
                    continue;
                }
                fire_effect( t_obj, level / 2, dam / 2, TARGET_OBJ );
            }
        }

        /*artefact*/
        if ( is_artefact( obj ) ) extract_artefact( obj );
        if ( obj->contains ) extract_artefact_container( obj );
        extract_obj( obj );
        return ;
    }
}

void poison_effect( void *vo, int level, int dam, int target )
{
    if ( target == TARGET_ROOM )   /* nail objects on the floor */
    {
        ROOM_INDEX_DATA * room = ( ROOM_INDEX_DATA * ) vo;
        OBJ_DATA *obj, *obj_next;

        for ( obj = room->contents; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );

            if ( IS_OBJ_STAT( obj, ITEM_NOPURGE ) )
                continue;

            poison_effect( obj, level, dam, TARGET_OBJ );
        }
        return ;
    }

    if ( target == TARGET_CHAR )    /* do the effect on a victim */
    {
        CHAR_DATA * victim = ( CHAR_DATA * ) vo;
        OBJ_DATA *obj, *obj_next;

        if ( !victim->in_room )
            return ;

		if ( is_affected(victim, gsn_mirrorfall) )
		{
			affect_strip(victim, gsn_mirrorfall);
			return;
		}

        /* chance of poisoning */
        /*        if (!saves_spell_new(victim, SAVE_DEATH, 0))
                {
        	    AFFECT_DATA af;

                    send_to_char("Czujesz jak trucizna rozprzestrzenia siê w twoich zylach.\n\r",victim);
                    act("$n wyglada bardzo niezdrowo.",victim,NULL,NULL,TO_ROOM);

                    af.where     = TO_AFFECTS;
                    af.type      = gsn_poison;
                    af.level     = 4;
                    af.duration  = dice(poison_table[4].duration[0],poison_table[4].duration[1]) + poison_table[4].duration[2];
                    af.rt_duration = 0;
                    af.location  = APPLY_NONE;
                    af.modifier  = 0;
                    af.bitvector = &AFF_POISON;
                    affect_join( victim, &af );
                }*/

        /* equipment */

		if ( is_affected(victim, gsn_mirrorfall) )
		{
			affect_strip(victim, gsn_mirrorfall);
			return;
		}

		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );
            poison_effect( obj, level, dam, TARGET_OBJ );
        }
        return ;
    }

    if ( target == TARGET_OBJ )   /* do some poisoning */
    {
        OBJ_DATA * obj = ( OBJ_DATA * ) vo;
        int chance;

        if ( IS_OBJ_STAT( obj, ITEM_BLESS ))
            return ;

	/* Tener: dodanie szansy na unikniêcie w zale¿no¶ci od poziomu czaru [20080512] */
	//2008-11-20, Brohacz: wylaczam. Spelle powyzej 25 levela nie psuja przeciez przez to gratow!
	/*if ( URANGE( 1, number_range( 1, 30 - level ), 10 ) < 5 )
	   return;*/

        chance = level + dam / 7;

 		switch ( obj->item_type )
        {
            default:
                return ;
            case ITEM_FOOD:
                break;
            case ITEM_DRINK_CON:
                if ( obj->value[ 0 ] == obj->value[ 1 ] )
                    return ;
                break;
        }

        if ( number_percent() > chance )
            return ;

        obj->value[ 3 ] = 1;
        return ;
    }
}


void shock_effect( void *vo, int level, int dam, int target )
{
    if ( target == TARGET_ROOM )
    {
        ROOM_INDEX_DATA * room = ( ROOM_INDEX_DATA * ) vo;
        OBJ_DATA *obj, *obj_next;

        for ( obj = room->contents; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );

            if ( IS_OBJ_STAT( obj, ITEM_NOPURGE ) )
                continue;

            shock_effect( obj, level, dam, TARGET_OBJ );
        }
        return ;
    }

    else if ( target == TARGET_CHAR )
    {
        CHAR_DATA * victim = ( CHAR_DATA * ) vo;
        OBJ_DATA *obj, *obj_next;

        if ( !victim->in_room )
            return ;

        /* jesli ma mozg znaczy siê mozna zszkokowac */
        if ( IS_SET( race_table[ GET_RACE( victim ) ].parts, PART_BRAINS ) &&
             !IS_SET( race_table[ GET_RACE( victim ) ].form, FORM_MAGICAL ) &&
             !is_undead( victim ) &&
             !IS_SET( race_table[ GET_RACE( victim ) ].form, FORM_CONSTRUCT ) &&
             !IS_SET( race_table[ GET_RACE( victim ) ].form, FORM_MIST ) &&
             !IS_SET( race_table[ GET_RACE( victim ) ].form, FORM_BLOB ) &&
             !IS_SET( race_table[ GET_RACE( victim ) ].form, FORM_DRAGON ) )
        {
            int chance_of_knockout = 0;

            if ( IS_SET( race_table[ GET_RACE( victim ) ].type, PERSON ) )
                chance_of_knockout = 10 + dam/10 + level/10;
            else if ( IS_SET( race_table[ GET_RACE( victim ) ].type, ANIMAL ) )
                chance_of_knockout = 5 + dam/11.5 + level/11.5;
            else if ( IS_SET( race_table[ GET_RACE( victim ) ].type, MONSTER ) )
                chance_of_knockout = 3 + dam/13 + level/13;

            if ( chance_of_knockout > 0 && number_percent() < chance_of_knockout && !IS_AFFECTED(victim,AFF_STABILITY) )
            {
                /* wait */
                WAIT_STATE( victim, URANGE( 12, dam/3, 36 ) );
                /* stoi badz walczy? no to hop na glebe */
                if ( victim->position == POS_STANDING || victim->position == POS_FIGHTING )
                {
                    if ( victim->sex == 0 )
                    {
                        send_to_char( "Tracisz kontrolê nad swoim cia³em i osuwasz siê po³przytomne na ziemie.\n\r", victim );
                        act( "$n osuwa siê polprzytomne na ziemie.", victim, NULL, NULL, TO_ROOM );
                    }
                    else if ( victim->sex == 1 )
                    {
                        send_to_char( "Tracisz kontrolê nad swoim cia³em i osuwasz siê po³przytomny na ziemie.\n\r", victim );
                        act( "$n osuwa siê polprzytomny na ziemie.", victim, NULL, NULL, TO_ROOM );
                    }
                    else
                    {
                        send_to_char( "Tracisz kontrolê nad swoim cia³em i osuwasz siê po³przytomna na ziemie.\n\r", victim );
                        act( "$n osuwa siê polprzytomna na ziemie.", victim, NULL, NULL, TO_ROOM );
                    }
                    victim->position = POS_SITTING;
                }
                /* lezy, siedzi, restuje, umiera etc? no to niech dalej umiera */
                else
                {
                    send_to_char( "Tracisz kontrolê nad swoim cia³em.\n\r", victim );
                    act( "Cia³o $z wyprê¿a siê i wiotczeje pod wp³ywem potê¿nego wy³adowania.", victim, NULL, NULL, TO_ROOM );
                }
            }
        }

		if ( is_affected(victim, gsn_mirrorfall) )
		{
			affect_strip(victim, gsn_mirrorfall);
			return;
		}

		/* toast some gear */
        for ( obj = victim->carrying; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );
            shock_effect( obj, level, dam, TARGET_OBJ );
        }
        return ;
    }

    if ( target == TARGET_OBJ )
    {
        OBJ_DATA * obj = ( OBJ_DATA * ) vo;
        OBJ_DATA *t_obj, *n_obj;
        char *msg;

	/* Tener: dodanie szansy na unikniêcie w zale¿no¶ci od poziomu czaru [20080512] */
	//2008-11-20, Brohacz: wylaczam. Spelle powyzej 25 levela nie psuja przeciez przez to gratow!
	/*if ( URANGE( 1, number_range( 1, 30 - level ), 10 ) < 5 )
	   return;*/

	if ( obj->item_type != ITEM_WAND && obj->item_type != ITEM_STAFF && obj->item_type != ITEM_JEWELRY )
	    return;

        if ( !check_item_resist( obj, RESIST_ELECTRICITY, dam ) )
            return ;

	if(obj->liczba_mnoga)
	{
            switch ( obj->item_type )
            {
	        default:
                	return ;
            	case ITEM_WAND:
            	case ITEM_STAFF:
	                msg = "$p rozb³yskuj± i po chwili eksploduj±!";
                	break;
            	case ITEM_JEWELRY:
	                msg = "$p stapiaj± siê w bezwarto¶ciow± bry³kê.";
            }
	}
	else
        {
            switch ( obj->item_type )
            {
	        default:
                	return ;
            	case ITEM_WAND:
            	case ITEM_STAFF:
	                msg = "$p rozb³yskuje i po chwili eksploduje!";
                	break;
            	case ITEM_JEWELRY:
	                msg = "$p stapia siê w bezwarto¶ciow± bry³kê.";
            }
        }

        if ( obj->carried_by != NULL )
            act( msg, obj->carried_by, obj, NULL, TO_ALL );
        else if ( obj->in_room != NULL && obj->in_room->people != NULL )
            act( msg, obj->in_room->people, obj, NULL, TO_ALL );

        /* get rid of the object */
        if ( obj->contains )   /* dump contents */
        {
            for ( t_obj = obj->contains; t_obj != NULL; t_obj = n_obj )
            {
                n_obj = t_obj->next_content;
                obj_from_obj( t_obj );

                if ( obj->in_room != NULL )
                    obj_to_room( t_obj, obj->in_room );

                else if ( obj->carried_by != NULL )
                    obj_to_room( t_obj, obj->carried_by->in_room );

                else
                {
                    /*artefact*/
                    if ( is_artefact( t_obj ) ) extract_artefact( t_obj );
                    if ( obj->contains ) extract_artefact_container( obj );
                    extract_obj( t_obj );
                    continue;
                }

                shock_effect( t_obj, level / 2, dam / 2, TARGET_OBJ );
            }
        }

        /*artefact*/
        if ( is_artefact( obj ) ) extract_artefact( obj );
        if ( obj->contains ) extract_artefact_container( obj );
        extract_obj( obj );
        return ;
    }
}

void sound_effect( void *vo, int level, int dam, int target )
{
	if ( target == TARGET_CHAR )
    {
        CHAR_DATA * victim = ( CHAR_DATA * ) vo;
        OBJ_DATA *obj, *obj_next;
        AFFECT_DATA af;

        if ( !victim->in_room )
            return ;

        /* jesli ma uszy moze ogluchnac */
        if ( IS_SET( race_table[ GET_RACE( victim ) ].parts, PART_EAR ) &&
             !IS_SET( race_table[ GET_RACE( victim ) ].form, FORM_MAGICAL ) &&
             !is_undead( victim ) &&
             !IS_SET( race_table[ GET_RACE( victim ) ].form, FORM_CONSTRUCT ) &&
             !IS_SET( race_table[ GET_RACE( victim ) ].form, FORM_MIST ) &&
             !IS_SET( race_table[ GET_RACE( victim ) ].form, FORM_BLOB ) &&
             !IS_SET( race_table[ GET_RACE( victim ) ].form, FORM_DRAGON ) &&
             !IS_AFFECTED( victim, AFF_DEAFNESS ) &&
             !IS_AFFECTED( victim, AFF_PERFECT_SENSES ) )
        {
            if ( number_percent() < dam - victim->resists[RESIST_SOUND]  )
            {
				af.where = TO_AFFECTS;
				af.type = 169;
				af.level = level;
				af.duration = 3;
				af.rt_duration = 0;
				af.location = APPLY_NONE;
				af.modifier = 0;
				af.bitvector = &AFF_DEAFNESS;
				affect_to_char( victim, &af, NULL, TRUE );
				send_to_char( "Ból w uszach potêguje siê coraz bardziej, a g³osy wokó³ ciebie nagle milkn±.\n\r", victim );
				act( "Z uszu $z wyp³ywa stru¿ka krwi.", victim, NULL, NULL, TO_ROOM );
            }
        }

        if ( is_affected(victim, gsn_mirrorfall) )
		{
			affect_strip(victim, gsn_mirrorfall);
			return;
		}

        for ( obj = victim->carrying; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );
            sound_effect( obj, level, dam, TARGET_OBJ );
        }
        return ;
    }
	else if ( target == TARGET_OBJ )
    {
        OBJ_DATA * obj = ( OBJ_DATA * ) vo;
        OBJ_DATA *t_obj, *n_obj;

	/* Tener: dodanie szansy na unikniêcie w zale¿no¶ci od poziomu czaru [20080512] */
	//2008-11-20, Brohacz: wylaczam. Spelle powyzej 25 levela nie psuja przeciez przez to gratow!
	/*if ( URANGE( 1, number_range( 1, 30 - level ), 10 ) < 5 )
	   return;*/


		if ( !check_item_resist( obj, RESIST_SOUND, dam+level ) )
			return;

        if ( obj->carried_by != NULL )
        {
        	if( obj->liczba_mnoga)
        		act( "$p rozpryskuj± siê na kawa³eczki.", obj->carried_by, obj, NULL, TO_ALL );
        	else
            	act( "$p rozpryskuje siê na kawa³eczki.", obj->carried_by, obj, NULL, TO_ALL );
        }
        else if ( obj->in_room != NULL && obj->in_room->people != NULL )
        {
          	if( obj->liczba_mnoga)
        		act( "$p rozpryskuj± siê na kawa³eczki.", obj->carried_by, obj, NULL, TO_ALL );
        	else
            	act( "$p rozpryskuje siê na kawa³eczki.", obj->carried_by, obj, NULL, TO_ALL );
        }

        if ( obj->contains )
        {
            for ( t_obj = obj->contains; t_obj != NULL; t_obj = n_obj )
            {
                n_obj = t_obj->next_content;
                obj_from_obj( t_obj );

                if ( obj->in_room != NULL )
                    obj_to_room( t_obj, obj->in_room );
                else if ( obj->carried_by != NULL )
                    obj_to_room( t_obj, obj->carried_by->in_room );
                else
                {
                    /*artefact*/
                    if ( is_artefact( t_obj ) ) extract_artefact( t_obj );
                    if ( obj->contains ) extract_artefact_container( obj );
                    extract_obj( t_obj );
                    continue;
                }
                shock_effect( t_obj, level / 2, dam / 2, TARGET_OBJ );
            }
        }

        /*artefact*/
        if ( is_artefact( obj ) ) extract_artefact( obj );
        if ( obj->contains ) extract_artefact_container( obj );
        extract_obj( obj );
        return ;
    }
    else if ( target == TARGET_ROOM )
    {
        ROOM_INDEX_DATA * room = ( ROOM_INDEX_DATA * ) vo;
        OBJ_DATA *obj, *obj_next;

        for ( obj = room->contents; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );

            sound_effect( obj, level, dam, TARGET_OBJ );
        }
        return ;
    }
	return;
}

