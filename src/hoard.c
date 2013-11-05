/***********************************************************************
 *                                                                     *
 * KILLER MUD is copyright 2010-2011 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Void                  (                                ) [Void    ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: hoard.c 10694 2011-11-30 22:21:09Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/hoard.c $
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
#include "money.h"

void do_hoard( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found;

    if ( ch->position == POS_SLEEPING )
    {
        send_to_char( "¦ni ci si± horror w którym oddajesz wszystkie przedmioty.\n\r", ch );
        return;
    }
    if ( ch->position <= POS_INCAP )
    {
        send_to_char( "Widzisz swiat³o na koncu tunelu...\n\r", ch );
        return;
    }
    if ( ch->fighting )
    {
        send_to_char( "Nie poddawaj si±, walcz!\n\r", ch );
        return;
    }
    argument = one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
        do_hoard_list(ch);
        return;
    }
    if ( (ch->in_room) && !EXT_IS_SET( ch->in_room->room_flags, ROOM_HOARD ) )
    {
        send_to_char( "W tym miejscu nie mo¿esz oddawaæ przedmiotów na przechowanie.\n\r", ch );
        return;
    }
    /*
       mamy 4 opcje wrzucania rzeczy z przechowalni:
       - zwraca 1 przedmiot, obslugiwane przez get_obj_carry()
       1. unhoar <nazwa> - wrzuca przedmiot o nazwie <nazwa>
       2. unhoar x.<nazwa> - wrzuca który¶ z kolei przedmiot o nazwie <nazwa>
       - zwraca kilka przedmiotow, obslugiwane w tej funkcji
       3. unhoar all.<nazwa> - wrzuca wszystkie przedmioty o nazwie <nazwa>
       4. unhoar all - wrzuca wszystko
     */
    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
        if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
        {
            send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
            return;
        }

        if ( !can_drop_obj( ch, obj ) )
        {
            send_to_char( "Nie mo¿esz tego wypu¶ciæ z r±ki.\n\r", ch );
            return;
        }

        if ( !can_see_obj( ch, obj ))
        {
            send_to_char( "Nie widzisz tego.\n\r", ch );
            return;
        }

        if (
                IS_OBJ_STAT( obj, ITEM_NO_RENT )
                || obj->item_type == ITEM_KEY
                || obj->item_type == ITEM_CORPSE_NPC
                || obj->item_type == ITEM_FOOD
                || ( obj->item_type == ITEM_MAP && !obj->value[ 0 ] )
           )
        {
            send_to_char( "Nie mo¿esz tego oddaæ na przechowanie.\n\r", ch );
            return;
        }

        if ( obj->contains )
        {
            act( "Najpierw opró¿nij $h.", ch, obj, NULL, TO_CHAR );
            return;
        }

        if ( obj->in_obj )
        {
            send_to_char( "Najpierw to wyci±gnij.\n\r", ch );
            return;
        }

        if ( obj->wear_loc != WEAR_NONE )
        {
            send_to_char( "Musisz najpierw to zdj±æ.\n\r", ch );
            return;
        }

        if ( carring_to_hoard( obj, ch, TRUE ) )
        {
            act( "Oddajesz $h na przechowanie.", ch, obj, NULL, TO_CHAR );
            act( "$n oddaje $h na przechowanie.", ch, obj, NULL, TO_ROOM );
        }
    }
    else
    {
        /* 'hoar all' or 'hoar all.obj' */
        found = FALSE;
        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT_CONTENT( obj, obj_next );

            if ( ( arg[ 3 ] == '\0' || is_name( &arg[ 4 ], obj->name ) )
                    && can_see_obj( ch, obj )
                    && (obj->wear_loc == WEAR_NONE)
                    && can_drop_obj( ch, obj )
                    && !obj->contains
                    && !obj->in_obj
                    && !IS_OBJ_STAT( obj, ITEM_NO_RENT )
                    && !(obj->item_type == ITEM_KEY)
                    && !(obj->item_type == ITEM_CORPSE_NPC)
                    && !(obj->item_type == ITEM_FOOD)
                    && !( obj->item_type == ITEM_MAP && !obj->value[ 0 ] ) )
            {
                found = TRUE;

                if ( carring_to_hoard( obj, ch, TRUE ) )
                {
                    act( "Oddajesz $h na przechowanie.", ch, obj, NULL, TO_CHAR );
                    act( "$n oddaje $h na przechowanie.", ch, obj, NULL, TO_ROOM );
                }
                else
                {
                    break;
                }
            }
        }

        if ( !found )
        {
            if ( arg[ 3 ] == '\0' )
                act( "Nie masz przy sobie nic, co mo¿na oddaæ na przechowanie.", ch, NULL, arg, TO_CHAR );
            else
                act( "Nie masz przy sobie czegos takiego jak $T, co mo¿na oddaæ na przechowanie.", ch, NULL, &arg[ 4 ], TO_CHAR );
        }
    }

    /* listy artów nie ruszamy, bo arty s± ciagle w posiadaniu gracza */

    return;
}

void do_unhoard( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found;

    if ( ch->position == POS_SLEEPING )
    {
        send_to_char( "¦ni ci si± pi±kny sen, w którym odbierasz wszystkie przedmioty ¶wiata.\n\r", ch );
        return;
    }
    if ( ch->position <= POS_INCAP )
    {
        send_to_char( "Widzisz swiat³o na koncu tunelu...\n\r", ch );
        return;
    }
    if ( ch->fighting )
    {
        send_to_char( "Nie poddawaj si±, walcz!\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
        do_hoard_list(ch);
        return;
    }

    if ( (ch->in_room) && !EXT_IS_SET( ch->in_room->room_flags, ROOM_HOARD ) )
    {
        send_to_char( "W tym miejscu nie mo¿esz odbieraæ przechowywanych przedmiotów.\n\r", ch );
        return;
    }

    if ( !ch->hoard )
    {
        send_to_char( "Aktualnie nie przechowujesz ¿adnych rzeczy.\n\r", ch );
        return;
    }

    /*
       mamy 5 opcji wyci±gania rzeczy z przechowalni:
       - zwraca 1 przedmiot, obslugiwane przez get_obj_hoard()
       1. unhoar <nazwa> - wyciaga przedmiot o nazwie <nazwa>
       2. unhoar x.<nazwa> - wyciaga który¶ z kolei przedmiot o nazwie <nazwa>
       3. unhoar <numer> - wyciaga przedmiot o numerze x
       - zwraca kilka przedmiotow, obslugiwane w tej funkcji
       4. unhoar all.<nazwa> - wyciaganie wszystkie przedmioty o nazwie <nazwa>
       5. unhoar all - wyciaga wszystko
     */

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
        if ( ( obj = get_obj_hoard( ch, arg ) ) == NULL )
        {
            send_to_char( "Nie przechowujesz takiej rzeczy.\n\r", ch );
            return;
        }

        if ( obj->vnum_hoard != ch->in_room->vnum )
        {
            send_to_char( "Ten przedmiot przechowujesz w innym miejscu.\n\r", ch );
            return;
        }

        if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
        {
            send_to_char( "Nie mo¿esz nosiæ niczego wiecej.\n\r", ch );
            return;
        }

        if ( get_carry_weight( ch ) + get_obj_weight( obj ) > can_carry_w( ch ) )
        {
            send_to_char( "Nie ud¼wigniesz tego.\n\r", ch );
            return;
        }

        obj_from_hoard( obj );
        obj_to_char( obj, ch );

        act( "Odbierasz $h z przechowania.", ch, obj, NULL, TO_CHAR );
        act( "$n odbiera $h z przechowania.", ch, obj, NULL, TO_ROOM );
    }
    else
    {
        /* 'unhoar all' or 'unhoar all.obj' */
        found = FALSE;
        for ( obj = ch->hoard; obj != NULL; obj = obj_next )
        {
            /* musze tutaj zapamietac obj_next, bo obj->next_content zmienia sie po obj_from_hoard( obj ) */

            obj_next = obj->next_content;
            if ( arg[ 3 ] == '\0' || is_name( &arg[ 4 ], obj->name ) )
            {
                if ( obj->vnum_hoard != ch->in_room->vnum )
                {
                    continue;
                }
                found = TRUE;
                if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
                {
                    send_to_char( "Nie mo¿esz nosiæ niczego wiecej.\n\r", ch );
                    break;
                }
                if ( get_carry_weight( ch ) + get_obj_weight( obj ) > can_carry_w( ch ) )
                {
                    send_to_char( "Nie ud¼wigniesz tego.\n\r", ch );
                    break;
                }
                obj_from_hoard( obj );
                obj_to_char( obj, ch );
                act( "Odbierasz $h z przechowalni.", ch, obj, NULL, TO_CHAR );
                act( "$n odbiera $h z przechowalni.", ch, obj, NULL, TO_ROOM );
            }
        }

        if ( !found )
        {
            if ( arg[ 3 ] == '\0' )
            {
                act( "Chyba niczego tutaj nie przechowujesz.", ch, NULL, arg, TO_CHAR );
            }
            else
            {
                act( "Nie przechowujesz tutaj czegos takiego jak $T.", ch, NULL, &arg[ 4 ], TO_CHAR );
            }
        }
    }
    return;
}
OBJ_DATA *get_obj_hoard( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    if ( is_number( argument ) )
    {
        number = atoi( argument );
        count  = 0;
        for ( obj = ch->hoard; obj != NULL; obj = obj->next_content )
        {
            if ( ++count == number )
            {
                return obj;
            }
        }
    }
    else
    {
        number = number_argument( argument, arg );
        count  = 0;
        for ( obj = ch->hoard; obj != NULL; obj = obj->next_content )
        {
            if ( is_name( arg, obj->name ) && ++count == number )
            {
                return obj;
            }
        }
    }
    return NULL;
}

void obj_from_hoard( OBJ_DATA * obj )
{
    DEBUG_INFO("handler.c:obj_from_hoard");

    if ( !obj || !(obj->hoarded_by) )
    {
        return;
    }
    if ( obj->hoarded_by->hoard == obj )
    {
        obj->hoarded_by->hoard = obj->next_content;
    }
    else
    {
        OBJ_DATA *prev;

        for ( prev = obj->hoarded_by->hoard; prev != NULL; prev = prev->next_content )
        {
            if ( prev->next_content == obj )
            {
                prev->next_content = obj->next_content;
                break;
            }
        }
    }

    obj->vnum_hoard = 0;
    obj->hoarded_by = NULL;
}

bool carring_to_hoard( OBJ_DATA *obj, CHAR_DATA *ch, bool check_capacity )
{
    DEBUG_INFO("handler.c:carring_to_stash");

    if ( !ch || !obj )
    {
        return FALSE;
    }

    if ( check_capacity )
    {
        int count = 0;
        OBJ_DATA *obj_tmp;
        for ( obj_tmp = ch->hoard; obj_tmp != NULL; obj_tmp = obj_tmp->next_content )
        {
            if  ( obj_tmp->vnum_hoard == ch->in_room->vnum )
            {
                count++;
            }
        }
        if ( count > 23 ) {
            send_to_char( "Nie mo¿esz ju¿ tu niczego wi±cej przechowaæ.\n\r", ch );
            return FALSE;
        }
    }

    obj_from_char(obj);

    obj->next_content = ch->hoard;
    ch->hoard         = obj;
    obj->hoarded_by   = ch;

    if (ch->in_room)
    {
        obj->vnum_hoard = ch->in_room->vnum;
    }

    return TRUE;
}

void do_hoard_list( CHAR_DATA *ch )
{
    OBJ_DATA * obj;
    char buff[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *room;
    int count;

    if( ch && IS_NPC(ch) )
    {
        return;//mobom dzi±kujemy
    }

    if ( ch->position == POS_SLEEPING )
    {
        send_to_char( "¦nisz o przedmiotach które pewnie gdzie¶ przechowujesz.\n\r", ch );
        return ;
    }
    send_to_char( "Aktualnie przechowujesz:\n\r", ch );

    if ( !ch->hoard )
    {
        send_to_char( "     Ogólnie nic.\n\r", ch );
        return;
    }

    count = 0;
    for ( obj = ch->hoard; obj != NULL; obj = obj->next_content )
    {
        room = get_room_index( obj->vnum_hoard );
        if  ( obj->vnum_hoard != ch->in_room->vnum )
        {
            sprintf( buff, "%2d. [%-*s] : przechowywane w '%s'.\n\r", ++count, 30+count_colors(obj->short_descr,MAX_INPUT_LENGTH), obj->short_descr, room ? room->name : "gdzie¶");
        }
        else
        {
            sprintf( buff, "%2d. [%-*s] : przechowywane {Wtutaj{x.\n\r", ++count, 30+count_colors(obj->short_descr,MAX_INPUT_LENGTH), obj->short_descr );
        }
        send_to_char( buff, ch );
    }

    return ;
}

