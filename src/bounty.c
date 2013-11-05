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
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Brosig Michal         (brohacz@gmail.com             ) [Brohacz   ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (tzdziech@gmail.com            ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: act_comm.c 11154 2012-03-24 10:00:25Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/act_comm.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "name_check.h"
#include "money.h"
//Brohacz: bounty: usuwa gracza z bounty list przy odbieraniu nagrody
bool rem_bounty( BOUNTY_DATA **list, char *argument )
{
    BOUNTY_DATA * tmp, *org_list, *tmp_prev;
    bool result = FALSE;
    org_list = *list;
    tmp = org_list;
    if ( !tmp )
    {
        return FALSE;
    }
    if ( !str_cmp( tmp->name, argument ) )
    {
        org_list = org_list->next;
        free_bounty( tmp );
        tmp = NULL;
        *list = org_list;
        result = TRUE;
    }
    else
    {
        tmp_prev = tmp;
        for ( ;tmp;tmp = tmp->next )
        {
            if ( !str_cmp( tmp->name, argument ) )
            {
                tmp_prev->next = tmp->next;
                tmp->next = NULL;
                free_bounty( tmp );
                tmp = NULL;
                result = TRUE;
                break;
            }
            tmp_prev = tmp;
        }
    }
    if ( *list == misc.bounty_list )
    {
        save_misc_data();
    }
    return result;
}

//Brohacz: bounty: dodaje gracza do bounty list
BOUNTY_DATA * add_bounty( BOUNTY_DATA **list, char *name, int value )
{
    BOUNTY_DATA *tmp, *it, *old_it;
    int i;
    if ( !name || name[ 0 ] == '\0' )
    {
        return NULL;
    }
    if ( strlen( name ) > 32 )
    {
        return NULL;
    }
    name[ 0 ] = UPPER( name[ 0 ] );
    for ( i = 1; name[ i ] != '\0'; i++ )
    {
        name[ i ] = LOWER( name[ i ] );
    }
    tmp = search_bounty( name );
    if ( tmp ) //jezeli gosc jest juz na liscie, to sumujemy nagrody, wywalamy go i dodajemy ponownie, zeby zachowac sortowanie
    {
        value += tmp->value;
        rem_bounty( &misc.bounty_list, name );
        tmp = add_bounty( &misc.bounty_list, name, value );
        return tmp;
    }
    else
    {
        it = *list;
        old_it = NULL;
        while ( it && it->value > value )
        {
            old_it = it;
            it = it->next;
        }
        tmp = new_bounty();
        tmp->name = str_dup( name );
        tmp->value = value;
        tmp->next = it;
        if ( !old_it )
        {
            (*list) = tmp;
        }
        else
        {
            old_it->next = tmp;
        }
    }
    return tmp;
}

//Brohacz: bounty: sprawdza, czy gracz jest na bounty list
BOUNTY_DATA * search_bounty( char *name )
{
    BOUNTY_DATA * tmp;
    BOUNTY_DATA * list = misc.bounty_list;
    if ( !name || name[ 0 ] == '\0' )
    {
        return NULL;
    }
    if ( strlen( name ) > 32 )
    {
        return NULL;
    }
    if ( list )
    {
        for ( tmp = list; tmp; tmp = tmp->next )
        {
            if ( !str_cmp( tmp->name, name ) )
            {
                return tmp;
            }
        }
    }
    return NULL;
}

//orginalny pomysl by FRiTZ <fritz1@swbell.net>
//bardzo zmodyfikowany snippet z ftp.game.org
void do_bounty( CHAR_DATA *ch, char *argument )
{
    char arg1 [ MAX_INPUT_LENGTH ];
    char arg2 [ MAX_INPUT_LENGTH ];
    BOUNTY_DATA *wch;
    int bounty_price;
    OBJ_DATA *head;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( !EXT_IS_SET( ch->in_room->room_flags, ROOM_BOUNTY_OFFICE ) )
    {
        send_to_char( "Nie mo¿esz tego tutaj zrobiæ, id¼ do biura ³owców nagród.\n\r", ch );
        return;
    }
    if ( !str_prefix( arg1, "list" ) )
    {
        if ( !misc.bounty_list )
        {
            send_to_char( "W tym momencie nie ma na ¶wiecie nikogo, za którego g³owê by³aby wyznaczona nagroda.\n\r", ch );
        }
        else
        {
            print_char( ch, "Aktualnie nagrody wyznaczone s± za:\n\r" );
            for ( wch = misc.bounty_list; wch != NULL; wch = wch->next )
            {
                print_char( ch, "%-9s - %d srebrnych monet\n\r", wch->name, wch->value );
            }
        }
        return;
    }
    if ( arg1[ 0 ] == '\0' )
    {
        for ( head = ch->carrying; head; head = head->next_content )
        {
            if ( head->item_type == ITEM_TROPHY && head->value[ 0 ] > 0 )
            {
                print_char( ch, "Otrzymujesz %s za przyniesienie %s.\n\r", money_string( head->value[ 0 ], FALSE ), head->name2 );
                act( "$n odbiera nagrodê.", ch, NULL, NULL, TO_ROOM );
                money_gain( ch, head->value[ 0 ] );
                extract_obj( head );
                return;
            }
        }
        send_to_char( "Za zabicie kogo chcesz wyznaczyæ nagrodê?\n\r", ch );
        return;
    }
    if ( is_allowed( arg1 ) != NAME_ALLOWED && is_allowed( arg1 ) != NAME_NEW )
    {
        send_to_char( "Nikt o takim imieniu nie przemierza ¶wiata.\n\r", ch );
        return;
    }
    if ( arg2[ 0 ] == '\0' || !is_number( arg2 ) )
    {
        send_to_char( "Jak wysok± nagrodê chcesz na³o¿yæ za tê g³owê?\n\r", ch );
        return;
    }

    bounty_price = atoi( arg2 );
    if ( bounty_price < 500 * RATTING_SILVER )
    {
        send_to_char( "Minimalna warto¶æ stawki to 500 srebrnych monet.\n\r", ch );
        return;
    }
    if ( money_count_copper ( ch ) < bounty_price )
    {
        send_to_char( "Masz przy sobie za malo pieniêdzy.\n\r", ch );
        return;
    }
    money_reduce( ch, bounty_price );
    wch = add_bounty( &misc.bounty_list, arg1, bounty_price );
    save_misc_data();
    if ( bounty_price != wch->value )
    {
        print_char( ch, "Do³o¿y³<&e¶/a¶/o¶> %s do nagrody za zabicie swojego wroga o imieniu %s.\n\r", money_string( bounty_price, FALSE ), arg1 );
    }
    else
    {
        print_char( ch, "Na³o¿y³<&e¶/a¶/o¶> %s nagrody za zabicie swojego wroga o imieniu %s.\n\r", money_string( bounty_price, FALSE ), arg1 );
    }
    return;
}

