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
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: bank.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/bank.c $
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

/* przy wyciaganiu kasy pobierana jest oplata- TAX_RATE % */
#define TAX_RATE	4

/**
 * local funkctions
 */

void do_bank( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) ) return;
    send_to_char( "Komenda usuniêta!\n\r'bank' zast±piono komendami 'deposit' i 'withdraw'.\n\r", ch );
    return;
}

CHAR_DATA *find_banker( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA * banker;
    BANK_DATA *pBank;

    if ( !EXT_IS_SET( ch->in_room->room_flags, ROOM_BANK ) )
    {
        send_to_char( "Id¼ do banku z t± spraw±.\n\r", ch );
        return NULL;
    }
    pBank = NULL;
    for ( banker = ch->in_room->people; banker; banker = banker->next_in_room )
    {
        if ( IS_NPC( banker ) && ( pBank = banker->pIndexData->pBank ) != NULL )
            break;
    }
    if ( pBank == NULL )
    {
        send_to_char( "Najwyra¼niej nikt nie zajmuje siê tu sprawami kont bankowych.\n\r", ch );
        return NULL;
    }
    /*
     * Invisible or hidden people.
     */
    if ( !can_see( banker, ch ) )
    {
        do_function( banker, &do_say, "Kto to powiedzia³? Co tu sie dzieje?" );
        return NULL;
    }
    if ( pBank->bank_room != 0 && pBank->bank_room != banker->in_room->vnum )
    {
	    do_function( banker, &do_say, "Przykro mi, ale tutaj nie przyjmujê." );
	    return NULL;
    }
    /*
     * Bank hours.
     */
    if ( time_info.hour < pBank->open_hour )
    {
        sprintf( buf, "Przykro mi, ale jeszcze nieczynne. Przyjd¼ o godzinie %d.", pBank->open_hour );
        do_function( banker, &do_say, buf );
        return NULL;
    }
    if ( time_info.hour > pBank->close_hour )
    {
        sprintf( buf, "Przykro mi, ale ju¿ nieczynne. Przyjd¼ o godzinie %d.", pBank->open_hour );
        do_function( banker, &do_say, buf );
        return NULL;
    }
    return banker;
}

void do_deposit( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * banker;
    char arg1 [ MAX_INPUT_LENGTH ];
    char arg2 [ MAX_INPUT_LENGTH ];
    long int amount = 0, copper;
    long int prev_bank = ch->bank;
    int nomination = NOMINATION_NONE, multiplier = RATTING_NONE;

    if ( IS_NPC( ch ) )
    {
        return;
    }

    if ( ( banker = find_banker( ch ) ) == NULL )
    {
        return;
    }

    if ( !IS_ACTIVE( banker ) )
    {
        return;
    }

    if ( IS_AFFECTED( ch, AFF_SILENCE ) )
    {
        send_to_char( "Pokazujesz bankierowi swoj± sakiewkê.\n\r", ch );
        act( "$n pokazuje na swoj± sakiewkê.", ch, NULL, NULL, TO_ROOM );
        switch ( number_range( 1, 5 ) )
        {
            case 1:
                do_function( banker, &do_say, "No tak, to jest sakiewka." );
                break;
            case 2:
                do_function( banker, &do_say, "Chcesz mi oddaæ swoj± sakiewkê?" );
                break;
            case 3:
                do_function( banker, &do_say, "Mimowie, przeklêci mimowie." );
                break;
            case 4:
                do_function( banker, &do_say, "A co w³a¶ciwie chcesz, wykrztusisz to wreszcie?" );
                break;
            case 5:
                do_function( banker, &do_say, "Chcesz moj± swoj± sakiewkê?" );
                break;
        }
        return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( !is_number( arg1 ) && str_cmp( arg1, "all" ) )
    {
        act( "{k$N mówi ci '{KIle i jakich monet chcesz zdeponowaæ na swoim koncie?{k'{x", ch, NULL, banker, TO_CHAR );
        return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
        if ( !money_count_copper( ch ) )
        {
            send_to_char( "Przecie¿ nie masz przy sobie ¿adnych pieniêdzy.\n\r", ch );
            return;
        }

        copper = money_count_copper( ch );

        if ( ch->copper > 0 )
        {
            if ( ch->copper == 1 )
            {
                send_to_char( "Wp³acasz na swoje konto 1 miedzian± monetê.\n\r", ch );
            }
            else
            {
                print_char( ch, "Wp³acasz na swoje konto %ld miedzianych monet.\n\r", ch->copper );
            }
            ch->bank += ch->copper;
        }

        if ( ch->silver > 0 )
        {
            if ( ch->silver == 1 )
            {
                send_to_char( "Wp³acasz na swoje konto 1 srebrn± monetê.\n\r", ch );
            }
            else
            {
                print_char( ch, "Wp³acasz na swoje konto %ld srebrnych monet.\n\r", ch->silver );
            }
            ch->bank += ch->silver * RATTING_SILVER;
        }

        if ( ch->gold > 0 )
        {
            if ( ch->gold == 1 )
            {
                send_to_char( "Wp³acasz na swoje konto 1 zlot± monetê.\n\r", ch );
            }
            else
            {
                print_char( ch, "Wp³acasz na swoje konto %ld zlotych monet.\n\r", ch->gold );
            }
            ch->bank += ch->gold * RATTING_GOLD;
        }

        if ( ch->mithril > 0 )
        {
            if ( ch->mithril == 1 )
            {
                send_to_char( "Wp³acasz na swoje konto 1 mithrilow± monetê.\n\r", ch );
            }
            else
            {
                print_char( ch, "Wp³acasz na swoje konto %ld mithrilowych monet.\n\r", ch->mithril );
            }
            ch->bank += ch->mithril * RATTING_MITHRIL;
        }

        money_reset_character_money( ch );

        act( "$n wp³aca pieni±dze na swoje konto.", ch, NULL, banker, TO_ROOM );
        act( "{k$N mówi ci '{KDziêkujê za korzystanie z naszych us³ug. Zapraszam w przysz³o¶ci.{k'{x", ch, NULL, banker, TO_CHAR );
        append_file_format_daily
            (
             ch,
             MONEY_LOG_FILE,
             "-> S: %d %d (%d), B: %d %d (%d) - wp³ata na konto",
             copper,
             0,
             -copper,
             prev_bank,
             ch->bank,
             ch->bank - prev_bank
            );
        return;
    }

    amount = atoi( arg1 );
    if ( amount <= 0 )
    {
        send_to_char( "Nie zawracaj g³owy bankierowi.\n\r", ch );
        return;
    }

    nomination = money_nomination_find( arg2 );

    if ( nomination )
    {
        copper = money_count_copper( ch );
    }

    switch ( nomination )
    {
        case NOMINATION_COPPER:
            if ( ch->copper < amount )
            {
                send_to_char( "Przecie¿ nie masz przy sobie tylu miedzianych monet.\n\r", ch );
                return;
            }
            multiplier = RATTING_COPPER;
            ch->copper -= amount;
            break;
        case NOMINATION_SILVER:
            if ( ch->silver < amount )
            {
                send_to_char( "Przecie¿ nie masz przy sobie tylu srebrnych monet.\n\r", ch );
                return;
            }
            multiplier = RATTING_SILVER;
            ch->silver -= amount;
            break;
        case NOMINATION_GOLD:
            if ( ch->gold < amount )
            {
                send_to_char( "Przecie¿ nie masz przy sobie tylu zlotych monet.\n\r", ch );
                return;
            }
            multiplier = RATTING_GOLD;
            ch->gold -= amount;
            break;
        case NOMINATION_MITHRIL:
            if ( ch->mithril < amount )
            {
                send_to_char( "Przecie¿ nie masz przy sobie tylu mithrilowych monet.\n\r", ch );
                return;
            }
            multiplier = RATTING_MITHRIL;
            ch->mithril -= amount;
            break;
        case NOMINATION_NONE:
        default:
            act( "{k$N mówi ci '{KJakie monety chcesz zdeponowaæ na swoim koncie?{k'{x", ch, NULL, banker, TO_CHAR );
            return;
    }

    ch->bank += amount * multiplier;

    append_file_format_daily
        (
         ch,
         MONEY_LOG_FILE,
         "-> S: %d %d (%d), B: %d %d (%d) - wp³ata na konto",
         copper,
         money_count_copper( ch ),
         copper - ( amount * multiplier ),
         prev_bank,
         ch->bank,
         ch->bank-prev_bank
        );

    if ( amount == 1 )
    {
        print_char( ch, "Wp³acasz na swoje konto jedn± %s monetê.\n\r", money_nomination( nomination, amount ) );
    }
    else
    {
        print_char( ch, "Wp³acasz na swoje konto %ld %s monet.\n\r", amount, money_nomination( nomination, amount ) );
    }
    act( "$n wp³aca pieni±dze na swoje konto.", ch, NULL, banker, TO_ROOM );
    act( "{k$N mówi ci '{KDziêkujê za korzystanie z naszych us³ug. Zapraszam w przysz³o¶ci.{k'{x", ch, NULL, banker, TO_CHAR );
    return;
}

void do_withdraw( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * banker;
    char buf [ MAX_STRING_LENGTH ];
    char arg1 [ MAX_INPUT_LENGTH ];
    char arg2 [ MAX_INPUT_LENGTH ];
    long int amount = 0, tax;
    int prev_silver = money_count_copper( ch );
    int prev_bank = ch->bank;
    int nomination = NOMINATION_NONE;

    if ( IS_NPC( ch ) || IS_AFFECTED( ch, AFF_CHARM ) )
        return;

    if ( ( banker = find_banker( ch ) ) == NULL )
        return;

    if ( !IS_ACTIVE( banker ) )
        return;

    if ( IS_AFFECTED( ch, AFF_SILENCE ) )
    {
        send_to_char( "Pokazujesz bankierowi swoj± sakiewkê.\n\r", ch );
        act( "$n pokazuje na swoj± sakiewkê.", ch, NULL, NULL, TO_ROOM );
        switch ( number_range( 1, 5 ) )
        {
            case 1:
                do_function( banker, &do_say, "No tak, to jest sakiewka." );
                break;
            case 2:
                do_function( banker, &do_say, "Chcesz mi oddaæ swoj± sakiewkê?" );
                break;
            case 3:
                do_function( banker, &do_say, "Mimowie, przeklêci mimowie." );
                break;
            case 4:
                do_function( banker, &do_say, "A co w³a¶ciwie chcesz, wykrztusisz to wreszcie?" );
                break;
            case 5:
                do_function( banker, &do_say, "Chcesz moj± swoj± sakiewkê?" );
                break;
        }
        return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( !is_number( arg1 ) && str_cmp( arg1, "all" ) )
    {
        act( "{k$N mówi ci '{KIle i jakich monet chcesz wyp³aciæ?{k'{x", ch, NULL, banker, TO_CHAR );
        return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
        tax = UMAX( 1, banker->pIndexData->pBank->provision * ch->bank / 100 );
        if ( tax > 0 )
        {
            ch->bank -= tax;
        }
        if ( ch->bank < 1 )
        {
            act( "{k$N mówi ci '{KPrzecie¿ masz puste konto.{k'{x", ch, NULL, banker, TO_CHAR );
            return;
        }
        else if ( ch->bank == 1 )
        {
            act( "{k$N mówi ci '{KTo co masz, starczy tylko na moja prowizje.{k'{x", ch, NULL, banker, TO_CHAR );
            return;
        }
            
        money_gain( ch, ch->bank ); 
        sprintf( buf, "$N wyplaca ci z konta %s.", money_string( ch->bank, TRUE )  );
        act( buf, ch, NULL, banker, TO_CHAR );
        act( "$N wyplaca $x troche monet z konta.", ch, NULL, banker, TO_ROOM );
        ch->bank = 0;

        if ( tax > 0 )
        {
            sprintf( buf, "{k$N mówi ci '{KPobra³em prowizjê w wysoko¶ci %s za prowadzenie konta.{k'{x", money_string ( tax, FALSE ) );
            act( buf, ch, NULL, banker, TO_CHAR );
        }

        append_file_format_daily
            (
             ch,
             MONEY_LOG_FILE,
             "-> S: %d %d (%d), B: %d %d (%d) - withdraw u moba [%5d], podatek %d",
             prev_silver,
             money_count_copper( ch ),
             money_count_copper( ch ) - prev_silver,
             prev_bank,
             ch->bank,
             ch->bank - prev_bank,
             banker->pIndexData->vnum,
             tax
            );
        return;
    }

    if ( ch->bank < 1 )
    {
        act( "{k$N mówi ci '{KNiestety, masz puste konto.{k'{x", ch, NULL, banker, TO_CHAR );
        return;
    }

    amount = atoi( arg1 );

    if ( amount < 1 )
    {
        send_to_char( "Nie zawracaj g³owy bankierowi.\n\r", ch );
        return;
    }
    
    nomination = money_nomination_find( arg2 );

    switch ( nomination )
    {
        case NOMINATION_COPPER:
            if ( amount == 1 && ch->bank == 1 )
            {
                act( "{k$N mówi ci '{KTo co masz, starczy tylko na moja prowizje.{k'{x", ch, NULL, banker, TO_CHAR );
                return;
            }
            amount *= RATTING_COPPER;
            break;
        case NOMINATION_SILVER:
            amount *= RATTING_SILVER;
            break;
        case NOMINATION_GOLD:
            amount *= RATTING_GOLD;
            break;
        case NOMINATION_MITHRIL:
            amount *= RATTING_MITHRIL;
            break;
        case NOMINATION_NONE:
        default:
            act( "{k$N mówi ci '{KJakie monety chcesz wybrac ze swojego konta?{k'{x", ch, NULL, banker, TO_CHAR );
            return;
    }

    if ( ch->bank < amount )
    {
        act( "{k$N mówi ci '{KPrzykro mi to mówiæ, ale nie masz tyle pieniêdzy na koncie.{k'{x", ch, NULL, banker, TO_CHAR );
        return;
    }

    tax = UMAX( 1, banker->pIndexData->pBank->provision * amount / 100 );

    ch->bank -= tax;

    if ( ch->bank < amount )
    {
        amount = ch->bank;
        ch->bank = 0;
    }
    else
    {
        ch->bank -= amount ;
    }

    money_gain ( ch, amount );

    append_file_format_daily
        (
         ch,
         MONEY_LOG_FILE,
         "-> S: %d %d (%d), B: %d %d (%d) - withdraw u moba [%5d], podatek %d",
         prev_silver,
         money_count_copper( ch ),
         money_count_copper( ch ) - prev_silver,
         prev_bank,
         ch->bank,
         ch->bank-prev_bank,
         banker->pIndexData->vnum,
         tax
        );

    sprintf( buf, "$N wyp³aca ci z konta %s.", money_string( amount, TRUE ) );
    act_new( buf, ch, NULL, banker, TO_CHAR, POS_DEAD );
    act( "$N wyplaca $x troche monet z konta.", ch, NULL, banker, TO_ROOM );

    if ( tax > 0 )
    {
        switch ( banker->sex )
        {
            case 0:
                sprintf( buf, "{k$N mówi ci '{KPobra³om prowizjê w wysoko¶ci %s za prowadzenie konta.{k'{x", money_string ( tax, FALSE ) );
                break;
            case 1:
                sprintf( buf, "{k$N mówi ci '{KPobra³em prowizjê w wysoko¶ci %s za prowadzenie konta.{k'{x", money_string ( tax, FALSE ) );
                break;
            default :
                sprintf( buf, "{k$N mówi ci '{KPobra³am prowizjê w wysoko¶ci %s za prowadzenie konta.{k'{x", money_string ( tax, FALSE ) );
                break;
        }
        act( buf, ch, NULL, banker, TO_CHAR );
    }
    return;
}

