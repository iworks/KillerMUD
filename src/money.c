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
 * money.c and associated patches copyright 2009 by Gurthg             *
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
 * KILLER MUD is copyright 2009-2011 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * ZMIENIA£E¦ CO¦? DOPISZ SIÊ!                                         *
 *                                                                     *
 * Pietrzak Marcin       (marcin@iworks.pl                ) [Gurthg  ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: money.c 7720 2009-07-11 14:25:37Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/09.06_money/src/money.c $
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
#include "recycle.h"
#include "interp.h"
#include "money.h"

/**
 * local functions
 */

void money_split_auto args ( ( CHAR_DATA *ch, OBJ_DATA *obj ) );

/**
 * wylicz liczbê noszonych przez postaæ monet
 */
int money_count_weight_carried_by_character ( CHAR_DATA *ch ) 
{
    return ( IS_NPC( ch ) )? 0 : money_weight( ch->copper, ch->silver, ch->gold, ch->mithril );
}

/**
 * wylicz wagê noszonych przez postaæ monet
 */
int money_weight ( long int copper, long int silver, long int gold, long int mithril )
{
    if ( copper > 0 || silver > 0 || gold > 0 || mithril > 0 )
    {
        return UMAX( 1, copper/40 + silver/20 + gold/10 + mithril/5 );
    }
    return 0;
}

/**
 * przelicz podane pieni±dze na mied¼
 */
long int money_to_copper ( long int copper, long int silver, long int gold, long int mithril )
{
    return UMAX( 0, copper + RATTING_SILVER * silver + RATTING_GOLD * gold + RATTING_MITHRIL * mithril );
}

/**
 * przelicz podane pieni±dze na mied¼ z okreslonego nominalu
 */
long int money_to_copper_by_nomination ( long int amount, int nomination )
{
    switch ( nomination )
    {
        case NOMINATION_COPPER:
            return amount * RATTING_COPPER;
        case NOMINATION_SILVER:
            return amount * RATTING_SILVER;
        case NOMINATION_GOLD:
            return amount * RATTING_GOLD;
        case NOMINATION_MITHRIL:
            return amount * RATTING_MITHRIL;
    }
    bug( "money_to_copper_by_nomination: error select nomination", 0 );
    return amount;
}

/**
 * policz pieni±dze noszone przez ch
 */
long int money_count_copper ( CHAR_DATA *ch )
{
    long int copper = 0;
    copper += ( ch->copper  > 0 )?                   ch->copper  : 0;
    copper += ( ch->silver  > 0 )? RATTING_SILVER  * ch->silver  : 0;
    copper += ( ch->gold    > 0 )? RATTING_GOLD    * ch->gold    : 0;
    copper += ( ch->mithril > 0 )? RATTING_MITHRIL * ch->mithril : 0;
    return copper;
}

/**
 * policz warto¶æ któr± mo¿e daæ obj
 */
long int money_count_copper_obj ( OBJ_DATA *obj )
{
    long int copper = 0;
    if ( obj->item_type == ITEM_MONEY )
    {
        copper += obj->value[0];
        copper += obj->value[1] * RATTING_SILVER;
        copper += obj->value[2] * RATTING_GOLD;
        copper += obj->value[3] * RATTING_MITHRIL;
    }
    return copper;
}

/**
 * policzca³kowit± kasê ch (wykorzystywane w rencie)
 */
long int money_count_copper_all ( CHAR_DATA *ch )
{
    return UMAX( 0, money_count_copper( ch ) + ch->bank );
}

/**
 * policz liczbê monet
 */
long int money_count_number_of_coinz ( OBJ_DATA *obj )
{
    long int coinz = 0;
    if ( obj->item_type == ITEM_MONEY )
    {
        coinz += obj->value[0];
        coinz += obj->value[1];
        coinz += obj->value[2];
        coinz += obj->value[3];
    }
    return coinz;
}

/**
 * zerowanie kasy ch
 */
void money_reset_character_money ( CHAR_DATA *ch )
{
    ch->copper  = 0;
    ch->silver  = 0;
    ch->gold    = 0;
    ch->mithril = 0;
}

/**
 * zmniejszanie kasy ch, wykorzystywaæ przy redukcjach
 * pieni±dza, je¿eli nie ma rozró¿niania monet
 */
void money_reduce ( CHAR_DATA *ch, long int copper )
{
    /**
     * sprawdzamy czy ma wystarczajaca ilosc kasy
     */
    if ( money_count_copper( ch ) > copper )
    {
        /**
         * doprowadzamy do sytuacji w której ma wystarczajaco ilosc
         * miedziakow, poprzez redukcje innych nominalow
         */
        while ( ch->copper < copper )
        {
            if ( ch->silver > 0 )
            {
                ch->copper += RATTING_SILVER_TO_COPPER;
                ch->silver -= 1;
            }
            else if ( ch->gold > 0 )
            {
                ch->silver += RATTING_GOLD_TO_SILVER;
                ch->gold -= 1;
            }
            else
            {
                ch->gold += RATTING_MITHRIL_TO_GOLD;
                ch->mithril -= 1;
            }
        }
        ch->copper = UMAX( 0, ch->copper - copper );
    }
    /**
     * w przeciwnym razie, kasujemu cala kasa juz
     * bez wnikania ile dokladnie jej ma
     */
    else
    {
        money_reset_character_money( ch );
    }
}

/**
 * dodawanie pieniedzy, wykorzystywac tylko w handlu z mobami
 * oraz w operacjach bankowych
 */
void money_gain ( CHAR_DATA *ch, long int copper )
{
    long int money = 0;
    if ( copper > 0 && ! copper < RATTING_MITHRIL )
    {
        money = copper;
        money /= RATTING_MITHRIL;
        ch->mithril += UMAX( 0, money );
        copper -= money * RATTING_MITHRIL;
    }
    if ( copper > 0 && ! copper < RATTING_GOLD )
    {
        money = copper;
        money /= RATTING_GOLD;
        ch->gold += UMAX( 0, money );
        copper -= money * RATTING_GOLD;
    }
    if ( copper > 0 && ! copper < RATTING_SILVER )
    {
        money = copper;
        money /= RATTING_SILVER;
        ch->silver += UMAX( 0, money );
        copper -= money * RATTING_SILVER;
    }
    if ( copper > 0 )
    {
        ch->copper += UMAX( 0, copper );
    }
}

/**
 * dzielenie siê pieniêdzmi
 */
void money_split( CHAR_DATA *ch, long int amount, int nomination, bool show_message, bool is_split_all, bool is_autosplit )
{
    int multiplier = RATTING_NONE;
    CHAR_DATA *gch;
    long int members = 0, share, extra;
    char buf[ MAX_INPUT_LENGTH ];
    char buf_amount[ MAX_INPUT_LENGTH ];

    switch ( nomination )
    {
        case NOMINATION_COPPER:
            if ( ch->copper < amount )
            {
                if ( show_message || is_autosplit )
                {
                    send_to_char( "Nie masz tylu miedzianych monet.\n\r", ch );
                }
                return;
            }
            break;
        case NOMINATION_SILVER:
            if ( ch->silver < amount )
            {
                if ( show_message || is_autosplit )
                {
                    send_to_char( "Nie masz tylu srebrnych monet.\n\r", ch );
                }
                return;
            }
            break;
        case NOMINATION_GOLD:
            if ( ch->gold < amount )
            {
                if ( show_message || is_autosplit )
                {
                    send_to_char( "Nie masz tylu zlotych monet.\n\r", ch );
                }
                return;
            }
            break;
        case NOMINATION_MITHRIL:
            if ( ch->mithril < amount )
            {
                if ( show_message || is_autosplit )
                {
                    send_to_char( "Nie masz tylu mithrilowych monet.\n\r", ch );
                }
                return;
            }
            break;
    } 

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
        if ( can_see( ch, gch ) && is_same_group( gch, ch ) && !IS_AFFECTED( gch, AFF_CHARM ) )
        {
            members++;
        }
    }

    if ( members < 2 )
    {
        if ( show_message && ! is_autosplit )
        {
            send_to_char( "Dzielisz siê piêniedzmi sam ze sob±.\n\r", ch );
        }
        return;
    }

    if ( amount < members )
    {
        if ( show_message && ! is_split_all && ! is_autosplit )
        {
            send_to_char( "Nawet siê nie trud¼ <&sk±pcu/sk±pa kobieto/sk±pirad³o>.\n\r", ch );
        }
        return;
    }

    share = amount;
    share /= members;
    if ( share < 1 )
    {
        share = 0;
    }

    extra = amount;
    extra %= members;
    if ( extra < 1 )
    {
        extra = 0;
    }

    if ( share < 1 )
    {
        if ( show_message && ! is_split_all && ! is_autosplit )
        {
            send_to_char( "Nawet siê nie trud¼ <&sk±pcu/sk±pa kobieto/sk±pirad³o>.\n\r", ch );
        }
        return;
    }

    append_file_format_daily
        (
         ch,
         MONEY_LOG_FILE,
         "-> S: before split /%d/%d/%d/%d/",
         ch->copper,
         ch->silver,
         ch->gold,
         ch->mithril
        );

    switch ( nomination )
    {
        case NOMINATION_COPPER:
            ch->copper += share + extra - amount;
            multiplier = RATTING_COPPER;
            break;
        case NOMINATION_SILVER:
            ch->silver += share + extra - amount;
            multiplier = RATTING_SILVER;
            break;
        case NOMINATION_GOLD:
            ch->gold += share + extra - amount;
            multiplier = RATTING_GOLD;
            break;
        case NOMINATION_MITHRIL:
            ch->mithril += share + extra - amount;
            multiplier = RATTING_MITHRIL;
            break;
    } 

    append_file_format_daily
        (
         ch,
         MONEY_LOG_FILE,
         "-> S: %ld %ld (%ld) - splitn±³ kasê w wysoko¶ci %s (%d/%d) nominal: %d /%d/%d/%d/%d/",
         money_count_copper( ch ) + ( extra + share - amount ) * multiplier,
         money_count_copper( ch ),
         ( share + extra - amount ) * multiplier,
         money_string_simple( amount, nomination, FALSE ),
         share,
         extra,
         nomination,
         ch->copper,
         ch->silver,
         ch->gold,
         ch->mithril
        );

    if ( show_message || is_autosplit )
    {
        sprintf ( buf_amount, "%s", money_string_simple ( amount, nomination, TRUE ) );
        sprintf( buf, "Rozdzielasz %s. Dostajesz %s.\n\r", buf_amount,  money_string_simple ( share + extra, nomination, TRUE ) );
        send_to_char( buf, ch );
        sprintf ( buf, "%s rozdziela %s. Dostajesz swoj± czê¶æ, %s.\n\r", ch->name, buf_amount, money_string_simple ( share, nomination, TRUE) );
    }

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
        if ( gch != ch && can_see( ch, gch ) && is_same_group( gch, ch ) && !IS_AFFECTED( gch, AFF_CHARM ) )
        {
            if ( show_message || is_autosplit )
            {
                /* act( buf, ch, NULL, gch, TO_VICT );*/
                send_to_char( buf, gch );
            }
            append_file_format_daily
                (
                 gch,
                 MONEY_LOG_FILE,
                 "-> S: %ld %ld (%ld) - dosta³ kasê ze splita od %s",
                 money_count_copper( gch ) - share * multiplier,
                 money_count_copper( gch ),
                 share * multiplier,
                 ch->name
                );
            switch ( nomination )
            {
                case NOMINATION_COPPER:
                    gch->copper += share;
                    break;
                case NOMINATION_SILVER:
                    gch->silver += share;
                    break;
                case NOMINATION_GOLD:
                    gch->gold += share;
                    break;
                case NOMINATION_MITHRIL:
                    gch->mithril += share;
                    break;
            }
        }
    }
}

/**
 * przenoszenie pieniêdzy z objektu na character,
 * wykorzystywane przy podnoszeniu ITEM_MONEY
 */
void money_gain_from_obj ( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    if ( obj->item_type == ITEM_MONEY )
    {
        long int copper = money_count_copper( ch );

        ch->copper  += UMAX ( 0, obj->value[ 0 ] );
        ch->silver  += UMAX ( 0, obj->value[ 1 ] );
        ch->gold    += UMAX ( 0, obj->value[ 2 ] );
        ch->mithril += UMAX ( 0, obj->value[ 3 ] );

        if ( container )
        {
            append_file_format_daily
                (
                 ch,
                 MONEY_LOG_FILE,
                 "-> S: %ld %ld (%ld) - /%d/%d/%d/%d/- wzi±³ kasê z kontenera [%5d] w lokacji [%5d]",
                 copper,
                 money_count_copper( ch ),
                 money_count_copper( ch ) - copper,
                 obj->value[ 0 ],
                 obj->value[ 1 ],
                 obj->value[ 2 ],
                 obj->value[ 3 ],
                 container->pIndexData ? container->pIndexData->vnum : 0,
                 container->in_room ? container->in_room->vnum : 0 
                );
            if 
                ( 
                 container->item_type == ITEM_CORPSE_NPC || 
                 (
                  container->item_type == ITEM_CORPSE_PC &&
                  str_cmp( container->hidden_description, ch->name2 )
                 )
                )
                {
                    money_split_auto( ch, obj );
                }
        }
        else
        {
            append_file_format_daily
                (
                 ch,
                 MONEY_LOG_FILE,
                 "-> S: %ld %ld (%ld) - /%d/%d/%d/%d/ - podniós³ kasê w lokacji [%5d]",
                 copper,
                 money_count_copper( ch ),
                 money_count_copper( ch ) - copper,
                 obj->value[ 0 ],
                 obj->value[ 1 ],
                 obj->value[ 2 ],
                 obj->value[ 3 ],
                 ch->in_room ? ch->in_room->vnum : 0
                );
        }
        extract_obj( obj );
    }
}

/**
 * 
 */
void money_from_container( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    if ( obj->item_type == ITEM_MONEY && ( obj->value[ 0 ] > 0 || obj->value[ 1 ] > 0 || obj->value[ 2 ] > 0 || obj->value[ 3 ] > 0 ) )
    {
        /**
         * pojedyncza moneta
         */
        if ( obj->value[ 0 ] == 1 && obj->value[ 1 ] == 0 && obj->value[ 2 ] == 0 && obj->value[ 3 ] == 0 )
        {
            send_to_char( "To tylko jedna miedziana moneta.\n\r", ch );
        }
        else if ( obj->value[ 0 ] == 0 && obj->value[ 1 ] == 1 && obj->value[ 2 ] == 0 && obj->value[ 3 ] == 0 )
        {
            send_to_char( "To tylko jedna srebrna moneta.\n\r", ch );
        }
        else if ( obj->value[ 0 ] == 0 && obj->value[ 1 ] == 0 && obj->value[ 2 ] == 1 && obj->value[ 3 ] == 0 )
        {
            send_to_char( "To tylko jedna z³ota moneta.\n\r", ch );
        }
        else if ( obj->value[ 0 ] == 0 && obj->value[ 1 ] == 0 && obj->value[ 2 ] == 0 && obj->value[ 3 ] == 1 )
        {
            send_to_char( "To tylko jedna mithrilowa moneta.\n\r", ch );
        }
        /**
         * rozne monety
         */
        else
        {
            if ( obj->value[ 0 ] > 0 )
            {
                print_char( ch, "Naliczy³<&e/a/o>¶ %s.\n\r", money_string_simple( obj->value[ 0 ], NOMINATION_COPPER,  TRUE ) );
            }
            if ( obj->value[ 1 ] > 0 )
            {
                print_char( ch, "Naliczy³<&e/a/o>¶ %s.\n\r", money_string_simple( obj->value[ 1 ], NOMINATION_SILVER,  TRUE ) );
            }
            if ( obj->value[ 2 ] > 0 )
            {
                print_char( ch, "Naliczy³<&e/a/o>¶ %s.\n\r", money_string_simple( obj->value[ 2 ], NOMINATION_GOLD,    TRUE ) );
            }
            if ( obj->value[ 3 ] > 0 )
            {
                print_char( ch, "Naliczy³<&e/a/o>¶ %s.\n\r", money_string_simple( obj->value[ 3 ], NOMINATION_MITHRIL, TRUE ) );
            }
        }
    }
    else
    {
        char buf [ MAX_INPUT_LENGTH ];
        send_to_char( "Dziwne, ale nie ma tu zadnych monet.\n\r", ch );
        sprintf ( buf, "money_from_container: ITEM_MONEY without any money value vnum %d", obj->pIndexData->vnum );
        bug( buf, 0 );
    }
}

/**
 * funkcja pomocnicza, tylko do autosplita
 */
void money_split_auto ( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( !IS_NPC( ch ) && EXT_IS_SET( ch->act, PLR_AUTOSPLIT ) )
    {
        money_split( ch, UMAX( 0, obj->value[ 0 ] ), NOMINATION_COPPER,  FALSE, FALSE, TRUE );
        money_split( ch, UMAX( 0, obj->value[ 1 ] ), NOMINATION_SILVER,  FALSE, FALSE, TRUE );
        money_split( ch, UMAX( 0, obj->value[ 2 ] ), NOMINATION_GOLD,    FALSE, FALSE, TRUE );
        money_split( ch, UMAX( 0, obj->value[ 3 ] ), NOMINATION_MITHRIL, FALSE, FALSE, TRUE );
    }
}

/**
 * przekazywanie pieniêdzy
 */
bool money_from_char_to_char( CHAR_DATA *ch, CHAR_DATA *victim, long int amount, int nomination )
{
    char buf [ MAX_INPUT_LENGTH ];
    int multiplier = RATTING_NONE;

    switch ( nomination )
    {
        case NOMINATION_COPPER:
            if ( ch->copper < amount )
            {
                send_to_char( "Przecie¿ nie masz a¿ tylu miedzianych monet.\n\r", ch );
                return FALSE;
            }
            ch->copper -= amount;
            victim->copper += amount;
            multiplier = RATTING_COPPER;
            break;
        case NOMINATION_SILVER:
            if ( ch->silver < amount )
            {
                send_to_char( "Przecie¿ nie masz a¿ tylu srebrnych monet.\n\r", ch );
                return FALSE;
            }
            ch->silver -= amount;
            victim->silver += amount;
            multiplier = RATTING_SILVER;
            break;
        case NOMINATION_GOLD:
            if ( ch->gold < amount )
            {
                send_to_char( "Przecie¿ nie masz a¿ tylu z³otych monet.\n\r", ch );
                return FALSE;
            }
            ch->gold -= amount;
            victim->gold += amount;
            multiplier = RATTING_GOLD;
            break;
        case NOMINATION_MITHRIL:
            if ( ch-> mithril < amount )
            {
                send_to_char( "Przecie¿ nie masz a¿ tylu mithrilowych monet.\n\r", ch );
                return FALSE;
            }
            ch->mithril -= amount;
            victim->mithril += amount;
            multiplier = RATTING_MITHRIL;
            break;
        default:
            send_to_char( "Ale o co chodzi z tymi wszystkimi monetami?\n\r", ch );
            return FALSE;
    }

    sprintf ( buf, "$n daje ci %s.", money_string_simple( amount, nomination, TRUE ) );
    act( buf, ch, NULL, victim, TO_VICT );
    
    act( "$n daje $X trochê monet.", ch, NULL, victim, TO_NOTVICT );
    
    sprintf ( buf, "Dajesz $X %s.", money_string_simple( amount, nomination, TRUE ) );
    act( buf, ch, NULL, victim, TO_CHAR );

    amount *= multiplier;

    append_file_format_daily
        (
         ch,
         MONEY_LOG_FILE,
         "-> S: %d %d (%d) - da³ kasê postaci %s",
         money_count_copper( ch ) + amount,
         money_count_copper( ch ),
         -amount,
         IS_NPC( victim ) ? victim->short_descr : victim->name
        );

    append_file_format_daily
        (
         victim,
         MONEY_LOG_FILE,
         "-> S: %d %d (%d) - dosta³ kasê od postaci %s",
         money_count_copper( victim ) - amount,
         money_count_copper( victim ),
         amount,
         IS_NPC( ch ) ? ch->short_descr : ch->name
        );
    return TRUE;
}

/**
 * wynajdowanie nominalu na podstawie wprowadzonnogo ciagu znakow
 */
int money_nomination_find ( char arg [ MAX_INPUT_LENGTH ] )
{
    if
        (
            !str_prefix( arg, "copper"      ) ||
            !str_prefix( arg, "mied¼"       ) ||
            !str_prefix( arg, "miedzi"      ) ||
            !str_prefix( arg, "miedzianych" )
            )
    {
        return NOMINATION_COPPER;
    }
    if
        (
         !str_prefix( arg, "silver"    ) ||
         !str_prefix( arg, "srebra"    ) ||
         !str_prefix( arg, "srebrnych" ) ||
         !str_prefix( arg, "srebro"    )
         )
    {
        return NOMINATION_SILVER;
    }
    if
        (
         !str_prefix( arg, "gold"    ) ||
         !str_prefix( arg, "z³ota"   ) ||
         !str_prefix( arg, "z³oto"   ) ||
         !str_prefix( arg, "z³otych" )
         )
    {
        return NOMINATION_GOLD;
    }
    if
        (
         !str_prefix( arg, "mithril"      ) ||
         !str_prefix( arg, "mithrilu"     ) ||
         !str_prefix( arg, "mitril"       ) ||
         !str_prefix( arg, "mitryl"       ) ||
         !str_prefix( arg, "mithrilowych" )
         )
    {
        return NOMINATION_MITHRIL;
    }
    return NOMINATION_NONE;
}

/**
 * kradziez w wykonaniu moba
 */
void money_transfer_spec_nasty ( CHAR_DATA *ch, CHAR_DATA *victim )
{
    long int copper = money_count_copper( victim );

    ch->copper += victim->copper  * RATTING_COPPER;
    ch->copper += victim->silver  * RATTING_SILVER;
    ch->copper += victim->gold    * RATTING_GOLD;
    ch->copper += victim->mithril * RATTING_MITHRIL;

    money_reset_character_money( victim );

    append_file_format_daily
        (
         victim,
         MONEY_LOG_FILE,
         "-> S: %ld 0 (%ld) - ukradziono przez spec_nasty [%5d]",
         copper,
         -copper,
         ch->pIndexData->vnum
        );
}

void money_steal( CHAR_DATA *ch, CHAR_DATA *victim )
{
    long int amount;
    int nomination = number_range( NOMINATION_COPPER, NOMINATION_MITHRIL ), multiplier = RATTING_NONE;

    /**
     * 0.001  szansy, ze ukradnie ca³o¶æ miedzianych monet 
     * 0.0007 szansy, ze ukradnie ca³o¶æ srebrnych monet
     * 0.0004 szansy, ze ukradnie ca³o¶æ z³otych monet
     * 0.0001 szansy, ze ukradnie ca³o¶æ mithrilowych monet
     */
    if ( number_percent() == 1 && number_percent() < ( 11 - 3 * ( nomination - NOMINATION_COPPER ) ) )
    {
        switch ( nomination )
        {
            case NOMINATION_COPPER:
                amount = victim->copper;
                break;
            case NOMINATION_SILVER:
                amount = victim->silver;
                break;
            case NOMINATION_GOLD:
                amount = victim->gold;
                break;
            case NOMINATION_MITHRIL:
                amount = victim->mithril;
                break;
        }
    }
    else
    {
        /**
         * budowanie ilosci kradzionej kasy
         * jak widac w zaleznosci od nominalu przyjmujemy
         * rozne wartosci mozliwe do ukradzenia
         */
        switch ( nomination )
        {
            case NOMINATION_COPPER:
                amount = UMIN( victim->copper,  UMAX( number_range( 1, 8 ), number_range( 1, 100 ) * victim->copper  / 100 ) );
                break;
            case NOMINATION_SILVER:
                amount = UMIN( victim->silver,  UMAX( number_range( 1, 4 ), number_range( 1,  50 ) * victim->silver  / 100 ) );
                break;
            case NOMINATION_GOLD:
                amount = UMIN( victim->gold,    UMAX( number_range( 1, 1 ), number_range( 1,  10 ) * victim->gold    / 100 ) );
                break;
            case NOMINATION_MITHRIL:
                amount = UMIN( victim->mithril, UMAX(                    1, number_range( 1,   5 ) * victim->mithril / 100 ) );
                break;
        }
    }
    switch ( nomination )
    {
        case NOMINATION_COPPER:
            ch->copper	+= amount;
            victim->copper -= amount;
            multiplier = RATTING_COPPER;
            break;
        case NOMINATION_SILVER:
            ch->silver	+= amount;
            victim->silver -= amount;
            multiplier = RATTING_SILVER;
            break;
        case NOMINATION_GOLD:
            ch->gold	+= amount;
            victim->gold -= amount;
            multiplier = RATTING_GOLD;
            break;
        case NOMINATION_MITHRIL:
            ch->mithril	+= amount;
            victim->mithril -= amount;
            multiplier = RATTING_MITHRIL;
            break;
    }
    append_file_format_daily
        (
         victim,
         MONEY_LOG_FILE,
         "-> S: %ld %ld (%ld) - ukradziono przez spec_thief [%5d]",
         money_count_copper( victim ) + multiplier * amount,
         money_count_copper( victim ),
         multiplier * amount,
         ch->pIndexData->vnum
        );
}
char * money_nomination ( int nomination, long int amount )
{
    bool first = ( ( amount < 11 || amount > 20 ) && ( amount % 10 == 2 || amount % 10 == 3 || amount % 10 == 4 ) );
    switch ( nomination )
    {
        case NOMINATION_COPPER:
            if ( amount > 1 )
            {
                if ( first )
                {
                    return "miedziane";
                }
                return "miedzianych";
            }
            return "miedzian±";
        case NOMINATION_SILVER:
            if ( amount > 1 )
            {
                if ( first )
                {
                    return "srebrne";
                }
                return "srebrnych";
            }
            return "srebrn±";
        case NOMINATION_GOLD:
            if ( amount > 1 )
            {
                if ( first )
                {
                    return "z³ote";
                }
                return "z³otych";
            }
            return "z³ot±";
        case NOMINATION_MITHRIL:
            if ( amount > 1 )
            {
                if ( first )
                {
                    return "mithrilowe";
                }
                return "mithrilowych";
            }
            return "mithrilow±";
    }
    return "";
}

long int money_copper_from_copper ( long int copper )
{
    copper %= RATTING_MITHRIL;
    copper %= RATTING_GOLD;
    copper %= RATTING_SILVER;
    if ( copper < RATTING_COPPER )
    {
        return 0;
    }
    copper /= RATTING_COPPER;
    return copper;
}

long int money_silver_from_copper ( long int copper )
{
    copper %= RATTING_MITHRIL;
    copper %= RATTING_GOLD;
    if ( copper < RATTING_SILVER )
    {
        return 0;
    }
    copper /= RATTING_SILVER;
    return copper;
}

long int money_gold_from_copper ( long int copper )
{
    copper %= RATTING_MITHRIL;
    if ( copper < RATTING_GOLD )
    {
        return 0;
    }
    copper /= RATTING_GOLD;
    return copper;
}

long int money_mithril_from_copper ( long int copper )
{
    if ( copper < RATTING_MITHRIL )
    {
        return 0;
    }
    copper /= RATTING_MITHRIL;
    return copper;
}

/**
 * przelicza podana sume na string.
 * uzywac tylko wtedy kiedy mud moze przeliczyc
 * pieniadze ustalajc samodzielnie liczbe
 * poszczegolnych rodzajow monet, przy konkretnej
 * liczbie uzywaj money_string_simple
 */
char * money_string ( long int amount, bool colorize )
{
    static char money_str[MAX_INPUT_LENGTH];

    long int copper  = money_copper_from_copper  ( amount );
    long int silver  = money_silver_from_copper  ( amount );
    long int gold    = money_gold_from_copper    ( amount );
    long int mithril = money_mithril_from_copper ( amount );

    int counter = 0;
    bool lots = FALSE;

    if ( copper  ) { counter++; }
    if ( silver  ) { counter++; }
    if ( gold    ) { counter++; }
    if ( mithril ) { counter++; }

    if ( copper ) 
    {
        sprintf
            (
             money_str,
             "%s%ld %s%s",
             colorize ? MONEY_COLOR( NOMINATION_COPPER ) : "",
             copper,
             money_nomination( NOMINATION_COPPER, copper ),
             colorize ? "{x" : ""
            );
        counter--;
        amount = copper;
    }

    if ( silver ) 
    {
        if ( copper )
        {
            sprintf
                (
                 money_str,
                 "%s%s %s%ld %s%s",
                 money_str,
                 counter == 1 ? " i": ",",
                 colorize ? MONEY_COLOR ( NOMINATION_SILVER ) : "",
                 silver,
                 money_nomination( NOMINATION_SILVER, silver ),
                 colorize ? "{x" : ""
                );
        }
        else
        {
            sprintf
                (
                 money_str,
                 "%s%ld %s%s",
                 colorize ? MONEY_COLOR ( NOMINATION_SILVER ) : "",
                 silver,
                 money_nomination( NOMINATION_SILVER, silver ),
                 colorize ? "{x" : ""
                );
        }
        counter--;
        amount = silver;
    }

    if ( gold ) 
    {
        if ( copper || silver )
        {
            sprintf
                (
                 money_str,
                 "%s%s %s%ld %s%s",
                 money_str,
                 counter == 1 ? " i": ",",
                 colorize ? MONEY_COLOR ( NOMINATION_GOLD ) : "",
                 gold,
                 money_nomination( NOMINATION_GOLD, gold ),
                 colorize ? "{x" : ""
                );
        }
        else
        {
            sprintf
                (
                 money_str,
                 "%s%ld %s%s",
                 colorize ? MONEY_COLOR ( NOMINATION_GOLD ) : "",
                 gold,
                 money_nomination( NOMINATION_GOLD, gold ),
                 colorize ? "{x" : ""
                );
        }
        counter--;
        amount = gold;
    }
    if ( mithril ) 
    {
        if ( copper || silver || gold )
        {
            sprintf
                (
                 money_str,
                 "%s%s%s %ld %s%s",
                 money_str,
                 counter == 1 ? " i": ",",
                 colorize ? MONEY_COLOR ( NOMINATION_MITHRIL ) : "",
                 mithril,
                 money_nomination( NOMINATION_MITHRIL, mithril ),
                 colorize ? "{x" : ""
                );
        }
        else
        {
            sprintf
                (
                 money_str,
                 "%s%ld %s%s",
                 colorize ? MONEY_COLOR ( NOMINATION_MITHRIL ) : "",
                 mithril,
                 money_nomination( NOMINATION_MITHRIL, mithril ),
                 colorize ? "{x" : ""
                );
        }
        counter--;
        amount = mithril;
    }
    lots = ( ( amount < 11 || amount > 20 ) && ( amount % 10 == 2 || amount % 10 == 3 || amount % 10 == 4 ) );

    sprintf ( money_str, "%s monet%s", money_str, lots ? "y" : ( amount == 1 ) ? "ê" : "" );

    return money_str;
}

/**
 * w przeciwienstwie do money_string nie przelicza podanej
 * sumy, uzywac w konkretnych przypadkach podoszenia 
 * konkretnej kasy
 */
char * money_string_simple ( long int amount, int nomination, bool colorize )
{
    static char money_str[MAX_INPUT_LENGTH];
    bool lots = FALSE;

    if ( amount ) 
    {
        sprintf
            (
             money_str,
             "%s%ld %s%s",
             colorize ? MONEY_COLOR( nomination ) : "",
             amount,
             money_nomination( nomination, amount ),
             colorize ? "{x" : ""
            );
    }

    lots = ( ( amount < 11 || amount > 20 ) && ( amount % 10 == 2 || amount % 10 == 3 || amount % 10 == 4 ) );

    sprintf ( money_str, "%s monet%s", money_str, lots ? "y" : ( amount == 1 ) ? "ê" : "" );

    return money_str;
}

/**
 * przelicza podana sume na string.
 * uzywane tylko w komendzie do_list
 */
char * money_string_short ( long int amount )
{
    static char money_str[MAX_INPUT_LENGTH];
    sprintf
        (
         money_str,
         "%s%2ld{x/%s%2ld{x/%s%2ld{x/%s%2ld{x",
         MONEY_COLOR( NOMINATION_COPPER  ),
         money_copper_from_copper  ( amount ),
         MONEY_COLOR( NOMINATION_SILVER  ),
         money_silver_from_copper  ( amount ),
         MONEY_COLOR( NOMINATION_GOLD    ),
         money_gold_from_copper    ( amount ),
         MONEY_COLOR( NOMINATION_MITHRIL ),
         money_mithril_from_copper ( amount )
        );
    return money_str;
}

