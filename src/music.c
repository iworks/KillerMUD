/***************************************************************************
*  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
*  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
*                                                                         *
*  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
*  Chastain, Michael Quan, and Mitchell Tse.                              *
*                                                                         *
*  In order to use any part of this Merc Diku Mud, you must comply with   *
*  both the original Diku license in 'license.doc' as well the Merc       *
*  license in 'license.txt'.  In particular, you may not remove either of *
*  these copyright notices.                                               *
*                                                                         *
*  Much time and thought has gone into this software and you are          *
*  benefitting.  We hope that you share your changes too.  What goes      *
*  around, comes around.                                                  *
***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@hypercube.org)				   *
*	    Gabrielle Taylor (gtaylor@hypercube.org)			   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

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
#include "music.h"
#include "recycle.h"
#include "tables.h"
#include "progs.h"

void save_songs( void )
{
}

void load_songs( void )
{
    FILE * fp;

    if ( ( fp = fopen( MUSIC_FILE, "r" ) ) == NULL )
    {
        bug( "Couldn't open music file, no songs available.", 0 );
        fclose( fp );
        return ;
    }

}

void do_play( CHAR_DATA *ch )
{
    OBJ_DATA * musical_instrument;
    int hmm = 0, skill;

    if ( ( skill = get_skill( ch, gsn_music ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return ;
    }

    if ( ch->on != NULL )
    {
        if ( ch->on->item_type == ITEM_MUSICAL_INSTRUMENT )
        {
            hmm = 1;
        }
    }

    if ( ( musical_instrument = get_eq_char( ch, WEAR_INSTRUMENT ) ) == NULL &&
         hmm == 0 )
    {
        send_to_char( "Lepiej najpierw postaraj siê o jaki¶ instrument muzyczny.\n\r", ch );
        return ;
    }

    if ( musical_instrument == NULL )
    {
        if ( hmm == 1 )
        {
            musical_instrument = ch->on;
            send_to_char( "DZIALA!2\n\r", ch );
        }
    }

    if ( musical_instrument->item_type != ITEM_MUSICAL_INSTRUMENT )
    {
        send_to_char( "Lepiej najpierw postaraj siê o jaki¶ instrument muzyczny.\n\r", ch );
        return ;
    }
    print_char( ch, "HMM sprawdzam czy instrument dziala - %s.\n\r", musical_instrument->name );
    send_to_char( "DZIALA!\n\r", ch );
    return ;

    /*if ( !IS_NULLSTR( horn->value[0] ) )  - nie wiem co to jest, bylo to zostawiam - Kainti.
    {
        if ( !supermob )
            init_supermob();

        char_from_room ( supermob );
        char_to_room( supermob, ch->in_room );

        sprintf( buf, "horn-%d", horn->value[0] );
        program_flow( buf, horn->value[0], supermob, ch, NULL, NULL );
        release_supermob();
    }
    return;*/
}
