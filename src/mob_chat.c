/*********************************************************************
*                                                                   *
* KILLER MUD is copyright 2010-2010 Killer MUD Staff (alphabetical) *
*                                                                   *
*   Pietrzak Marcin     (marcin.pietrzak@mud.pl        ) [Gurthg  ] *
*                                                                   *
*********************************************************************/
/* $Id: mob_chat.c 10003 2011-01-24 23:37:46Z rysand $*/
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
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
/*
 *
 * GADANIE MOBOW
 *
 */

bool mob_can_speak  args ( ( CHAR_DATA *ch ) );
void mob_chat_fight args ( ( CHAR_DATA *ch ) );

void mob_chat_fight ( CHAR_DATA *ch )
{
    if ( IS_NPC ( ch ) && mob_can_speak( ch ) && number_percent() < 5 )
    {
        switch ( GET_RACE ( ch ) )
        {
            case RACE_PC_DWARF:// krasnolud
                switch ( number_range( 1, 6 ) )
                {
                    case 1:
                        do_function( ch, &do_say, "Na pochybel!" );
                        break;
                    case 2:
                        do_function( ch, &do_say, "Giñ oprawco!" );
                        break;
                    case 3:
                        do_function( ch, &do_say, "Rozdepczê ciê!" );
                        break;
                    case 4:
                        do_function( ch, &do_say, "Zniszczê ciê!" );
                        break;
                    case 5:
                        do_function( ch, &do_say, "Bez lito¶ci!" );
                    case 6:
                        do_function( ch, &do_emote, "toczy pianê z ust." );
                        break;
                }
            break;
        }
    }
    return;
}

bool mob_can_speak( CHAR_DATA *ch )
{
    if ( IS_AFFECTED( ch, AFF_SILENCE ) )
    {
        return FALSE;
    }
    if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ) )
    {
        switch (ch->sex)
        {
            case SEX_NEUTRAL:
                act( "$n probuje co¶ powiedzieæ, chmura b±belków powietrza wydobywa sie temu z ust.", ch, NULL, NULL, TO_ROOM );
                break;
            case SEX_MALE:
                act( "$n probuje co¶ powiedzieæ, chmura b±belków powietrza wydobywa sie mu z ust.", ch, NULL, NULL, TO_ROOM );
                break;
            case SEX_FEMALE:
            default:
                act( "$n probuje co¶ powiedzieæ, chmura b±belków powietrza wydobywa sie jej z ust.", ch, NULL, NULL, TO_ROOM );
                break;
        }
        return FALSE;
    }
    return TRUE;
}
