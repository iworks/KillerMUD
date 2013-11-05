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
 * reward.c and associated patches copyright 2001 by Sandi Fallon      *
 *                                                                     *
 * In order to use any part of this ROM Merc Diku code you must comply *
 * the original Diku license in 'license.doc' as well the Merc license *
 * in 'license.txt' and also the ROM license in 'rom.license', each to *
 * be found in doc/. Using the reward.c code without conforming to the *
 * requirements of each of these documents is violation of any and all *
 * applicable copyright laws. In particular, you may not remove any of *
 * these copyright notices or claim other's work as your own.          *
 *                                                                     *
 * Much time and thought has gone into this software you are using.    *
 * We hope that you share your improvements, too.                      *
 * "What goes around, comes around."                                   *
 *                                                                     *
 ***********************************************************************
 *                                                                     *
 * KILLER MUD is copyright 2009-2011 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * ZMIENIA£E¦ CO¦? DOPISZ SIÊ!                                         *
 *                                                                     *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: reward.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/reward.c $
 *
 */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "money.h"

/* imported stuff */
DECLARE_DO_FUN( do_emote );
DECLARE_DO_FUN( do_say );
extern void do_function args( ( CHAR_DATA *ch, DO_FUN *do_fun, char *argument ) );

/* adapted from healer.c */
CHAR_DATA *find_captain( CHAR_DATA *ch )
{
    CHAR_DATA *captain;

    for( captain = ch->in_room->people; captain; captain = captain->next_in_room )
    {
        if ( IS_NPC( captain ) && EXT_IS_SET( captain->act, ACT_REWARD ) )
        {
            break;
        }
    }

    if ( captain == NULL )
    {
        return NULL;
    }

    if ( !can_see( captain, ch ) )
    {
        do_function( captain, &do_say, "Kto to powiedzia³? Co tu siê dzieje?" );
        return NULL;
    }

    if ( IS_NPC(ch) )
    {
        do_function( captain, &do_emote, "macha niecierpliwie rek±." );
        do_function( captain, &do_say, "No jasne! Tak jakbym nie mia³ na co czasu marnowaæ." );
        return NULL;
    }

    return captain;
}

/* adapted from healer.c */
CHAR_DATA *find_rewarder( CHAR_DATA *ch )
{
    CHAR_DATA *rewarder;

    for( rewarder = ch->in_room->people; rewarder; rewarder = rewarder->next_in_room )
    {
        if ( IS_NPC(rewarder) && rewarder->pIndexData->vnum == ch->pcdata->rewarder )
        {
            break;
        }
    }

    if ( rewarder == NULL )
    {
        return NULL;
    }

    if ( !can_see( rewarder, ch ) )
    {
        do_function( rewarder, &do_say, "Kto to powiedzia³? Co tu siê dzieje?" );
        return NULL;
    }

    if ( IS_NPC(ch) )// || IS_SET(ch->act,ACT_PET) )
    {
        do_function( rewarder, &do_emote, "macha niecierpliwie rek±." );
        do_function( rewarder, &do_say, "No jasne! Tak jakbym nie mia³ na co czasu marnowaæ." );
        return NULL;
    }

    return rewarder;
}

/* this is where we pretend to decrement a counter - Fallon */
/* the magic number 720 is the number of seconds in 12 minutes */
void reset_hunt( CHAR_DATA *ch )
{
    int diff = 0;

    if ( ( diff = (current_time - ch->pcdata->hunt_time) ) < 720 )
    {
        if ( diff < 0 )
        {
            ch->pcdata->recovery = current_time + 720;
        }
        else
        {
            ch->pcdata->recovery = current_time + 720 - diff;
        }
    }

    else
    {
        ch->pcdata->recovery  = 0;
        ch->pcdata->hunt_time = 0;
    }

    ch->pcdata->rewarder   = 0;
    ch->pcdata->reward_mob = 0;
    ch->pcdata->reward_obj = 0;
}

/* adapted from Furey's mfind() code */
void bounty( CHAR_DATA *ch, CHAR_DATA *captain )
{
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *room;
    CHAR_DATA *victim = NULL;
    CHAR_DATA *guard;
    OBJ_DATA *obj = NULL;
    OBJ_DATA *obj_next;
    int vnum = 0;
    bool obj_found = 0;
    bool aggie_found;


    /* start with a random number */
    /* note that the condition in the for() will never be met, so be careful
     * about changing things here or you'll have an infinite loop.
     */
    for ( vnum = 31; vnum < 65536; vnum = number_range( 32, 65536 ) )
    {
        /* make it a random room */
        if ( ( room = get_room_index( vnum ) ) != NULL )
        {
            /* go through the mobs in the room and check for aggies */
            aggie_found = FALSE;
            for ( victim = room->people; victim != NULL; victim = victim->next_in_room )
            {
                if ( IS_NPC(victim) && EXT_IS_SET(victim->act, ACT_AGGRESSIVE) && victim->level > ch->level )
                {
                    aggie_found = TRUE;
                    break;
                }
            }

            if ( !aggie_found )
            {
                /* go through the mobs in the room and check their level and flags */
                for ( victim = room->people; victim != NULL; victim = victim->next_in_room )
                {
                    if ( IS_NPC(victim)
                            && ( IS_NEUTRAL(victim) || (ch->alignment > 0 ? IS_EVIL(victim) : IS_GOOD(victim)) )
                            && ch->level + (ch->level / 4) >= victim->level
                            && ch->level - (ch->level / 3) <= victim->level
                            && !EXT_IS_SET( victim->act, ACT_PRACTICE )
                            && !EXT_IS_SET( victim->act, ACT_REWARD )
                            && !IS_SET( race_table[ GET_RACE( victim ) ].type, ANIMAL )
                            && !IS_SET( victim->form, FORM_ANIMAL )
                            && !IS_SET( victim->form, FORM_MIST )
                            && !IS_SET( victim->form, FORM_CONSTRUCT )
                            /* && !IS_SET(victim->act, ACT_TRAIN)
                               && !IS_SET(victim->act, ACT_PRACTICE)
                               && !IS_SET(victim->act, ACT_IS_HEALER)
                               && !IS_SET(victim->act, ACT_IS_CHANGER)
                               && !IS_SET(victim->act, ACT_BANKER) you might not have ACT_BANKER */
                            /* this idea is from Addams: add IMM_SUMMON to all the healers, shopkeeps,
                             * trainers, school mobs, etc. that you don't want to be hunted.
                             * they really should be, anyway. Otherwise, uncomment the code above. */
                            //&& !IS_SET(victim->imm_flags, IMM_SUMMON)
                            && !IS_AFFECTED( victim, AFF_CHARM )
                       )
                    {
                        /* see if the mob has an object */
                        /* you might want to add a check for wear_loc - the following
                         * includes inventory but without 'peek' they can only see what
                         * the mob is wearing. This does, however, give peek more value.
                         */
                        obj_found = FALSE;
                        for (obj = victim->carrying; obj != NULL; obj= obj_next)
                        {
                            obj_next = obj->next_content;

                            if ( 
                                    !IS_OBJ_STAT( obj, ITEM_NODROP ) &&
                                    !IS_OBJ_STAT( obj, ITEM_NOLOCATE ) &&
                                    !IS_OBJ_STAT( obj, ITEM_NOREMOVE ) &&
                                    !IS_OBJ_STAT( obj, ITEM_VIS_DEATH ) &&
                                    !IS_OBJ_STAT( obj, ITEM_ROT_DEATH ) &&
                                    IS_SET( obj->wear_flags, ITEM_TAKE )
                               )
                            {
                                obj_found = TRUE;
                                break;
                            }
                        }
                    }
                    if ( obj_found )
                    {
                        break;
                    }
                }
            }
        }
        if ( obj_found )
        {
            break;
        }
    }

    if ( !obj_found )
    {
        sprintf( buf, "Wybacz %s, ale na razie nie mam niczego na miarê twojego talentu.", ch->name );
        do_function ( captain, &do_say, buf );
        return;
    }

    /* make sure we didn't screw up - if any of these are missing, we go boom */
    if ( ( room = get_room_index( vnum ) ) == NULL || obj == NULL || victim == NULL )
    {
        bug( "Reward: found NULL in bounty.", 0 );
        return;
    }

    /* you need to either commit to fixing capitalised articles on mob->short in
     * all the areas, or make it LOWER(victim->short_descr) in the 4th argument */
    switch ( obj->item_type )
    {
        case ITEM_WEAPON :
            sprintf(
                    buf,
                    "S³uchaj %s, wszyscy wiedz±, ¿e %s ukrad³ %s. Znajd¼ z³odzieja i odbierz to. Potem przynie¶ ten przedmiot do mnie, a nagroda ciê nie ominie.",
                    ch->name,
                    victim->short_descr,
                    obj->short_descr
                   );
            break;
        case ITEM_ARMOR :
            sprintf
                (
                 buf,
                 "Doniesiono mi, ¿e zgubiono %s. Wiem kto to zabra³. Znajd¼ %s i przynis do to mnie. Zanim wrócisz, przygotujê nagrodê.",
                 capitalize(obj->short_descr),
                 victim->short_descr
                );
            break;
        case ITEM_CONTAINER :
            sprintf
                (
                 buf,
                 "%s przepad³! %s, jeden %s twierdzi, ¿e zwin±³ to %s. Zabij %s i zabierz %s. Jak przyniesiesz, to nagroda bêdzie spora",
                 capitalize(obj->short_descr),
                 ch->name,
                 race_table[victim->race].name,
                 victim->short_descr,
                 victim->short_descr,
                 obj->short_descr 
                );
            break;
        default :
            sprintf
                (
                 buf,
                 "%s zagin±³! %s, krótko przez ¶mierci± nasz informator z rasy %s doniós³ ¿e podwêdzi³ to %s. Zga¶ %s i przynie¶ do mnie %s, a bêdziesz wynagrodzony.",
                 capitalize(obj->short_descr),
                 ch->name,
                 race_table[victim->race].name,
                 victim->short_descr,
                 victim->short_descr,
                 obj->short_descr
                );
            break;
    }
    do_say( captain, buf );

    /* again, you'll probably need to edit a few areas - the Sewers has room names ending in periods */
    for( guard = ch->in_room->people; guard; guard = guard->next_in_room )
    {
        if ( IS_NPC(guard) && !IS_AFFECTED(guard, AFF_CHARM) && !EXT_IS_SET( guard->act, ACT_REWARD ) )
        {
            break;
        }
    }
    /* this is in case all the guards wander off while the door is open.
     * you might want to set one guard 'sentinel', so he stays put. */
    if ( guard == NULL )
    {
        guard = captain;
    }

    do_emote( guard, "patrzy na mapê." );

    sprintf
        (
         buf,
         "S³ysza³em, ¿e %s ostatnio widziano w %s.",
         victim->short_descr,
         room->area->name
        );
    do_say( guard, buf );

    ch->pcdata->rewarder = 0;
    ch->pcdata->reward_mob = victim->pIndexData->vnum;
    ch->pcdata->reward_obj = obj->pIndexData->vnum;
    ch->pcdata->hunt_time  = ( current_time + 1200 );
    return;
}


/* the reward command structure eliminates the need for arguments
 * but do_command needs them */
void do_reward( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *captain;
    CHAR_DATA *rewarder = NULL;
    OBJ_DATA *obj = NULL;
    OBJ_DATA *obj_next;
    ROOM_INDEX_DATA *room;
    int reward = 0;
    int vnum = 0;
    bool obj_found = FALSE;
    bool rewarder_found = FALSE;

    /* Newbie's can't hunt bounties. */
    if (ch->level < 2 )
    {
        sprintf( buf, "Jeszcze nie mo¿esz wykonywaæ zadañ.\n\r" );
        send_to_char( buf, ch );
        return;
    }

    /* reset them if they've run out of time */
    if ( ch->pcdata->hunt_time < current_time )
    {
        reset_hunt(ch);
    }

    /* see if they're available for work */
    if ( ch->pcdata->recovery > current_time )
    {
        sprintf( buf, "Do wykonania zadania zosta³o ci jeszcze %ld %s.\n\r",
                ((ch->pcdata->recovery - current_time) / 60) + 1,
                (((ch->pcdata->recovery - current_time) / 60)) + 1 == 1 ? "godzina" : "godziny" );
        send_to_char( buf, ch );
        return;
    }

    /* if they have been assigned a hunt, look for the object in
     * the char's inventory */
    if ( ch->pcdata->reward_obj > 0 )
    {
        obj_found = FALSE;
        for (obj = ch->carrying; obj != NULL; obj= obj_next)
        {
            obj_next = obj->next_content;

            if ( obj->pIndexData->vnum == ch->pcdata->reward_obj )
            {
                obj_found = TRUE;
                break;
            }
        }
    }

    /* if they have the object, check for the rewarder in the room */
    if ( obj_found && (rewarder = find_rewarder(ch)) != NULL )
    {
        reward = UMAX ( ch->level * number_range( 10, 20 ), obj->cost * UMAX ( ch->level / 10, 1 ) * UMAX( get_mob_index(ch->pcdata->reward_mob)->level / 10, 1 ) );

        switch( number_range(1, 3) )
        {
            case 1:
                sprintf
                    (
                     buf,
                     "Fantastycznie %s! Znalaz³e¶ %s który by³ ukradziony %s! W nagrodê otrzymujesz %d srebra.",
                     ch->name, obj->pIndexData->short_descr,
                     get_mob_index(ch->pcdata->reward_mob)->short_descr,
                     reward
                    );
                break;
            case 2:
                sprintf
                    (
                     buf,
                     "Hej %s znalaz³e¶ %s! Zdumiewajace! Dam ci %d srebra w nagrodê.",
                     ch->name,
                     obj->pIndexData->short_descr,
                     reward
                    );
                break;
            case 3:
                sprintf
                    (
                     buf,
                     "%s! %s, masz to! Aby okazaæ moj± wdziêczno¶æ otrzymujesz w nagrodê %d srebra.",
                     capitalize(obj->pIndexData->short_descr),
                     ch->name,
                     reward 
                    );
                break;
        }
        do_function ( rewarder, &do_say, buf );

        act( "$c zabiera $p od $N i daje $M nagrodê.", rewarder, obj, ch, TO_NOTVICT );
        act( "$c zabiera od ciebie $p i daje nagrodê.", rewarder, obj, ch, TO_VICT );

        extract_obj(obj);
        reset_hunt(ch);
        money_gain( ch, reward );
        ch->pcdata->rewards += 1;
        return;
    }

    /* see if the character is in the room with the ACT_REWARD mob */
    if ( ( captain = find_captain(ch) ) != NULL )
    {
        /* if the char has no reward_obj set, give her one */
        if ( ch->pcdata->reward_obj < 1 )
        {
            /* bounty() will almost always find a mob, so if you want a chance of
             * failure, THIS is the place to put it - before we load the CPU. */
            bounty( ch, captain );
            return;
        }

        /* they have found the object, so we find someone to send them to */
        if ( obj_found )
        {
            /* first make sure it's their first time asking */
            if ( ch->pcdata->rewarder > 0 )
            {
                sprintf
                    (
                     buf,
                     "%s, masz %s, który to przedmiot nale¿y di %s. Pospiesz siê, bo zosta³o tylko %ld %s.",
                     ch->name,
                     get_obj_index(ch->pcdata->reward_obj)->short_descr,
                     get_mob_index(ch->pcdata->rewarder)->short_descr,
                     (ch->pcdata->hunt_time - current_time) / 60 + 1,
                     (((ch->pcdata->hunt_time - current_time ) / 60)) + 1 == 1 ? "godzina" : "godzin"
                    );
                do_say( captain, buf );
                return;
            }

            else
            {
                rewarder_found = FALSE;
                /* start with a random number */
                for ( vnum = 30; vnum < 65536; vnum = number_range( 31, 65535 ) )
                {
                    /*make it a random room */
                    if ( ( room = get_room_index(vnum) ) != NULL )
                    {
                        for ( rewarder = room->people; rewarder != NULL; rewarder = rewarder->next_in_room )
                        {
                            if ( IS_NPC(rewarder)
                                    && ch->level + (ch->level / 3) + 3 >= rewarder->level
                                    && !EXT_IS_SET(rewarder->act, ACT_AGGRESSIVE) /* by popular demand! */
                                    && !EXT_IS_SET(rewarder->act, ACT_PRACTICE) /* this takes care of school */
                                    // && !EXT_IS_SET(rewarder->act, ACT_GAIN) /* watch out for mobs in clan halls */
                                    && !EXT_IS_SET(rewarder->affected_by, AFF_CHARM) ) /* No children, no pets allowed */
                            {
                                ch->pcdata->rewarder = rewarder->pIndexData->vnum;
                                ch->pcdata->hunt_time = ( current_time + 3600 );
                                rewarder_found = TRUE;
                                break;
                            }
                        }
                    }
                    if ( rewarder_found )
                    {
                        break;
                    }
                }
            }

            if ( ( room = get_room_index(vnum) ) == NULL || rewarder == NULL )
            {
                bug( "Reward: found NULL in reward.", 0 );
                return;
            }

            sprintf
                ( 
                 buf,
                 "Bardzo dobrze siê spisa³e¶ %s. Znalaz³e¶ %s! Teraz zanie¶ to do %s, którego znajdziesz w %s i zapytaj o nagrodê.",
                 ch->name,
                 get_obj_index(ch->pcdata->reward_obj)->short_descr,
                 get_mob_index(ch->pcdata->rewarder)->short_descr,
                 lowercase(room->name)
                );
            do_say( captain, buf );

            return;
        }

        /* OK, no obj, let's give them a progress report */
        else
        {
            sprintf
                (
                 buf, "%s, szukasz %s ukradzionego przez %s. Zosta³o ci jeszcze %ld %s.",
                 ch->name,
                 get_obj_index(ch->pcdata->reward_obj)->short_descr,
                 get_mob_index(ch->pcdata->reward_mob)->short_descr,
                 (ch->pcdata->hunt_time - current_time) / 60 + 1,
                 (((ch->pcdata->hunt_time - current_time ) / 60)) + 1 == 1 ? "godzina" : "godzin"
                );
            do_say( captain, buf );
            return;
        }
    }

    /* not with the ACT_REWARD or REWARDER mob, so... */

    if ( ch->pcdata->reward_obj < 1 )
    {
        sprintf( buf, "Aktualnie niczego nie szukasz. Znajd¼ kogo¶ u kogo mo¿esz zapytaæ o zadanie.\n\r" );
        send_to_char( buf, ch );
        return;
    }

    if ( obj_found )
    {
        /* if they have a rewarder, send a reminder */
        if ( ch->pcdata->rewarder != 0 )
        {
            sprintf
                (
                 buf, "Zosta³o ci %ld %s ¿eby daæ %s %s!\n\r",
                 (ch->pcdata->hunt_time - current_time) / 60 + 1,
                 (((ch->pcdata->hunt_time - current_time ) / 60)) + 1 == 1 ? "godzina" : "godzin",
                 get_obj_index(ch->pcdata->reward_obj)->short_descr,
                 get_mob_index(ch->pcdata->rewarder)->short_descr
                );
            send_to_char( buf, ch );
            return;
        }

        /* if they've just found the object, send them back to the captain */
        else
        {
            sprintf
                (
                 buf, "Znalaz³e¶ %s. Zosta³o %ld %s!\n\rWróæ do tego, kto zleca³ zadanie po dalsze instrukcje.\n\r",
                 get_obj_index(ch->pcdata->reward_obj)->short_descr,
                 (ch->pcdata->hunt_time - current_time) / 60 + 1,
                 (((ch->pcdata->hunt_time - current_time ) / 60)) + 1 == 1 ? "godzina" : "godzin"
                );
            send_to_char( buf, ch );
            return;
        }
    }

    /* if they're still hunting, give them a progress report */
    if ( ch->pcdata->reward_obj > 0 )
    {
        sprintf
            (
             buf, "Szukasz %s ukradzionego przez %s.\n\rZosta³a ci %ld %s.\n\r",
             get_obj_index(ch->pcdata->reward_obj)->short_descr,
             get_mob_index(ch->pcdata->reward_mob)->short_descr,
             (ch->pcdata->hunt_time - current_time) / 60 + 1,
             (((ch->pcdata->hunt_time - current_time ) / 60)) + 1 == 1 ? "godzina" : "godzin"
            );
        send_to_char( buf, ch );
        return;
    }

    /* since we've tested all the possibilities, you should never see this message */
    else
    {
        bug( "Reward: A player passed ALL the tests. Error!", 0 );
        return;
    }

}
