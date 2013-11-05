#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "progs.h"
#include "olc.h"
#include "recycle.h"
#include "lang.h"
#include "music.h"

#define SEDIT( fun )           bool fun( CHAR_DATA *ch, char *argument )

char *  string_linedel      args( ( char *, int ) );
char *  string_lineadd      args( ( char *, char *, int ) );
char *  numlineas           args( ( char * ) );

const struct olc_cmd_type sedit_table[] =
{
    {    "commands",     show_commands     },
    {    "create",       sedit_create      },
    {    "show",         sedit_show        },
    {    "list",         sedit_list        },
    {    "?",            show_help         },
    {    "description",  sedit_description },
    {    "addcomment",   sedit_addcomment  },
    {    "delcomment",   sedit_delcomment  },
    {    "name",         sedit_name        },
    {    "type",         sedit_type        },
    {    "slots",        sedit_slots       },
    {    "target",       sedit_target      },
    {    "perform",      sedit_perform     },
    {    NULL,           0                 }
};

void sedit( CHAR_DATA *ch, char *argument )
{
    SONG_DATA * pSong;
    char arg[ MAX_INPUT_LENGTH ];
    char command[ MAX_INPUT_LENGTH ];
    int cmd;

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    EDIT_SDATA( ch, pSong );

    if ( command[ 0 ] == '\0' )
    {
        sedit_show( ch, argument );
        return ;
    }

    if ( !str_cmp( command, "done" ) )
    {
        edit_done( ch );
        return ;
    }

    for ( cmd = 0; sedit_table[ cmd ].name != NULL; cmd++ )
    {
        if ( !str_prefix( command, sedit_table[ cmd ].name ) )
        {
            ( *sedit_table[ cmd ].olc_fun ) ( ch, argument );
            return ;
        }
    }

    interpret( ch, arg );
    return ;
}


void do_sedit( CHAR_DATA *ch, char *argument )
{
    SONG_DATA * pSong;
    char command[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, command );

    if ( is_number( command ) )
    {
        ush_int number = atoi( command );

        if ( ( pSong = get_song_index( number ) ) == NULL )
        {
            send_to_char( "Sedit : Nie istnieje taka piosenka.\n\r", ch );
            return ;
        }

        ch->desc->pEdit	= ( void * ) pSong;
        ch->desc->editor	= ED_SDATA;

        return ;
    }

    if ( !str_cmp( command, "create" ) )
    {
        sedit_create( ch, argument );
        return ;
    }

    send_to_char( "Syntax :   sedit [number]\n\r", ch );
    send_to_char( "           sedit create\n\r", ch );

    return ;
}

SEDIT ( sedit_create )
{
    SONG_DATA * pSong;

    pSong	         = new_song_index();
    pSong->next	     = song_list;
    song_list	     = pSong;
    ch->desc->pEdit  = ( void * ) pSong;
    ch->desc->editor = ED_SDATA;

    send_to_char( "Nowa piosenka zosta³a utworzona.\n\r", ch );

    return TRUE;
}

SEDIT( sedit_show )
{
    SONG_DATA * pSong;
    PERFORM_DATA * pPerf;
    char buf[ MAX_STRING_LENGTH ];
    int cnt;

    EDIT_SDATA( ch, pSong );

    sprintf( buf,
             "{GName{x:           [%s]\n\r"
             "{GDescription{x:\n\r%s\n\r",
             IS_NULLSTR( pSong->name ) ? "none" : pSong->name, pSong->description );
    send_to_char( buf, ch );

    if ( pSong->perform )
    {
        for ( cnt = 0, pPerf = pSong->perform; pPerf; pPerf = pPerf->next )
        {
            if ( cnt == 0 )
            {
                send_to_char( "{CPerforms{x:\n\r", ch );
                send_to_char( "Number Instruments\n\r", ch );
                send_to_char( "------ -----------\n\r", ch );
            }

            //TODO: wyswietlanie flag

            send_to_char( buf, ch );
            cnt++;
        }
        send_to_char( "\n\r", ch );
    }

    //TODO: wyswietlanie type
    //TODO: wyswietlanie slots
    //TODO: wyswietlanie effects

    if ( !IS_NULLSTR( pSong->comments ) )
    {
        send_to_char( "\n\r{CKomentarze{x:\n\r", ch );
        send_to_char( numlineas( pSong->comments ), ch );
    }

    return FALSE;
}

SEDIT( sedit_list )
{
    int count = 1;
    SONG_DATA *pSong, *song;
    char buf[ MAX_STRING_LENGTH ];
    BUFFER *buffer;

    EDIT_SDATA( ch, pSong );
    buffer = new_buf();

    for ( song = song_list; song; song = song->next )
    {
        sprintf( buf, "[%2d] %s\n\r", count, IS_NULLSTR( song->name ) ? "no name" : song->name );
        add_buf( buffer, buf );

        count++;
    }

    if ( count == 1 )
    {
        add_buf( buffer, "Nie znaleziono ¿adnych piosenek.\n\r" );
    }

    page_to_char( buf_string( buffer ), ch );
    free_buf( buffer );

    return FALSE;
}
/*
SEDIT( sedit_addaffect )
{
    int value, sn = -1;
    SONG_DATA *pSong;
    AFFECT_DATA *pAf;
    char loc[ MAX_STRING_LENGTH ];
    char mod[ MAX_STRING_LENGTH ];
    char arg_last[ MAX_STRING_LENGTH ];

    EDIT_SDATA( ch, pSong );

    argument = one_argument( argument, loc );
    argument = one_argument( argument, mod );

    if ( loc[ 0 ] == '\0' || mod[ 0 ] == '\0' )
    {
        send_to_char( "Syntax:  addaffect [location] [#xmod]\n\r", ch );
        return FALSE;
    }

    if ( ( value = flag_value( apply_flags, loc ) ) == NO_FLAG )
    {
        send_to_char( "Valid affects are:\n\r", ch );
        show_help( ch, "apply" );
        return FALSE;
    }

    if ( value == APPLY_SKILL )
    {
        sn = skill_lookup( mod );
        one_argument( argument, arg_last );

        if ( sn < 0 || arg_last[ 0 ] == '\0' || !is_number( arg_last ) || skill_table[ sn ].spell_fun != spell_null )
        {
            send_to_char( "Syntax:  addaffect skill [nazwa skilla] [wartosc]\n\r", ch );
            return FALSE;
        }
    }

    else if ( value == APPLY_LANG )
    {
        sn = get_langnum( mod );
        one_argument( argument, arg_last );

        if ( sn < 0 || sn > MAX_LANG || arg_last[ 0 ] == '\0' || !is_number( arg_last ) )
        {
            send_to_char( "Syntax:  addaffect lang [nazwa jezyka] [mod]\n\r", ch );
            return FALSE;
        }
    }
    else if ( value == APPLY_RESIST )
    {
        sn = resist_number( mod );
        one_argument( argument, arg_last );

        if ( sn < 0 || sn > MAX_RESIST || arg_last[ 0 ] == '\0' || !is_number( arg_last ) )
        {
            send_to_char( "Syntax:  addaffect resist [odpornosc] [wartosc%]\n\r", ch );
            return FALSE;
        }
    }

    else if ( value == APPLY_MEMMING )
    {
        one_argument( argument, arg_last );

        if ( !is_number( mod ) || arg_last[ 0 ] == '\0' || !is_number( arg_last ) )
        {
            send_to_char( "Syntax:  addaffect spellmem [krag] [wartosc]\n\r", ch );
            return FALSE;
        }

        sn = atoi( mod );

        if ( sn < 1 || sn > 11 )
        {
            send_to_char( "Syntax:  addaffect spellmem [krag] [wartosc]\n\r", ch );
            return FALSE;
        }

    }

    else if ( !is_number( mod ) )
    {
        send_to_char( "Syntax:  addaffect [location] [#xmod]\n\r", ch );
        return FALSE;
    }

    pAf = new_affect();
    pAf->location = value;

    if ( sn < 0 )
    {
        pAf->modifier = atoi( mod );
        pAf->level = 0;
    }
    else
    {
        pAf->modifier = sn;
        pAf->level = atoi( arg_last );
    }
    pAf->where	    = TO_OBJECT;
    pAf->type       = -1;
    pAf->duration   = -1;
    pAf->bitvector  = AFF_NONE;
    pAf->next       = pSong->affects;
    pSong->affects = pAf;

    send_to_char( "Affect dodany.\n\r", ch );
    return TRUE;
}

SEDIT( sedit_delaffect )
{
    SONG_DATA *pSong;
    AFFECT_DATA *pAf;
    AFFECT_DATA *pAf_next;
    char affect[ MAX_STRING_LENGTH ];
    int value;
    int cnt = 0;

    EDIT_SDATA( ch, pSong );

    one_argument( argument, affect );

    if ( !is_number( affect ) || affect[ 0 ] == '\0' )
    {
        send_to_char( "Syntax:  delaffect [#xaffect]\n\r", ch );
        return FALSE;
    }

    value = atoi( affect );

    if ( value < 0 )
    {
        send_to_char( "Only non-negative affect-numbers allowed.\n\r", ch );
        return FALSE;
    }

    if ( !( pAf = pSong->affects ) )
    {
        send_to_char( "OEdit:  Non-existant affect.\n\r", ch );
        return FALSE;
    }

    if ( value == 0 )
    {
        pAf = pSong->affects;
        pSong->affects = pAf->next;
        free_affect( pAf );
    }
    else
    {
        while ( ( pAf_next = pAf->next ) && ( ++cnt < value ) )
            pAf = pAf_next;

        if ( pAf_next )
        {
            pAf->next = pAf_next->next;
            free_affect( pAf_next );
        }
        else
        {
            send_to_char( "No such affect.\n\r", ch );
            return FALSE;
        }
    }

    send_to_char( "Affect removed.\n\r", ch );
    return TRUE;
}

SEDIT( sedit_addapply )
{
    int value, typ;
    SONG_DATA *pSong;
    AFFECT_DATA *pAf;
    char loc[ MAX_STRING_LENGTH ];
    char mod[ MAX_STRING_LENGTH ];
    char type[ MAX_STRING_LENGTH ];
    char bvector[ MAX_STRING_LENGTH ];
    BITVECT_DATA vector;

    EDIT_SDATA( ch, pSong );

    argument = one_argument( argument, type );
    argument = one_argument( argument, loc );
    argument = one_argument( argument, mod );
    one_argument( argument, bvector );

    if ( type[ 0 ] == '\0' || ( typ = flag_value( apply_types, type ) ) == NO_FLAG )
    {
        send_to_char( "Invalid apply type. Valid apply types are:\n\r", ch );
        show_help( ch, "apptype" );
        return FALSE;
    }

    if ( loc[ 0 ] == '\0' || ( value = flag_value( apply_flags, loc ) ) == NO_FLAG )
    {
        send_to_char( "Valid applys are:\n\r", ch );
        show_help( ch, "apply" );
        return FALSE;
    }

    if ( bvector[ 0 ] == '\0' )
    {
        send_to_char( "Invalid bitvector type.\n\r", ch );
        send_to_char( "Valid bitvector types are:\n\r", ch );
        show_help( ch, bitvector_type[ typ ].help );
        return FALSE;
    }

    vector = affect_flag_lookup( bvector, affect_flags );

    if ( vector.vector == 0 )
    {
        send_to_char( "Invalid bitvector type.\n\r", ch );
        send_to_char( "Valid bitvector types are:\n\r", ch );
        show_help( ch, bitvector_type[ typ ].help );
        return FALSE;
    }


    if ( mod[ 0 ] == '\0' || !is_number( mod ) )
    {
        send_to_char( "Syntax:  addapply [type] [location] [#xmod] [bitvector]\n\r", ch );
        return FALSE;
    }

    pAf = new_affect();
    pAf->location = value;
    pAf->modifier = atoi( mod );
    pAf->where	= apply_types[ typ ].bit;
    pAf->type	= -1;
    pAf->duration = -1;
    pAf->rt_duration = 0;
    pAf->bitvector = vector;
    pAf->level = 0;
    pAf->next = pSong->affects;
    pSong->affects = pAf;

    send_to_char( "Apply dodany.\n\r", ch );
    return TRUE;
}
*/

SEDIT( sedit_description )
{
    SONG_DATA *pSong;

    EDIT_SDATA( ch, pSong );

    if ( argument[ 0 ] == '\0' )
    {
        string_append( ch, &pSong->description );
        return TRUE;
    }

    send_to_char( "Syntax:  desc    - krótki opis utworu\n\r", ch );
    return TRUE;
}

SEDIT( sedit_addcomment )
{
    char buf1[ MAX_STRING_LENGTH ];
    char buf2[ MAX_STRING_LENGTH ];
    char time[ MAX_STRING_LENGTH ];
    SONG_DATA *pSong;

    EDIT_SDATA( ch, pSong );

    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Nie poda<&³e¶/³a¶/³e¶> komentarza.\n\r", ch );
        return FALSE;
    }

    buf1[ 0 ] = '\0';

    strcpy( time, ( char * ) ctime( &current_time ) );
    time[ strlen( time ) - 1 ] = '\0';

    if ( !IS_NULLSTR( pSong->comments ) )
        strcpy( buf1, pSong->comments );
    smash_tilde( argument );
    sprintf( buf2, " - {R%s{x ({C%s{x)", capitalize( ch->name ), time );
    strcat( buf1, argument );
    strcat( buf1, buf2 );
    strcat( buf1, "\n\r" );

    free_string( pSong->comments );
    pSong->comments = str_dup( buf1 );

    send_to_char( "Komentarz dodany.\n\r", ch );

    return TRUE;
}

SEDIT( sedit_delcomment )
{
    SONG_DATA *pSong;

    EDIT_SDATA( ch, pSong );

    if ( argument[ 0 ] == '\0' || !is_number( argument ) )
    {
        send_to_char( "Podaj numer komentarza do wykasowania.\n\r", ch );
        return FALSE;
    }

    if ( !IS_NULLSTR( pSong->comments ) )
        pSong->comments = string_linedel( pSong->comments, atoi( argument ) );

    send_to_char( "Komentarz usuniêty.\n\r", ch );

    return TRUE;
}

SEDIT( sedit_name )
{
    SONG_DATA *pSong;

    EDIT_SDATA( ch, pSong );

    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Syntax:  name [string]\n\r", ch );
        return FALSE;
    }

    free_string( pSong->name );
    pSong->name = str_dup( argument );

    send_to_char( "Name set.\n\r", ch );
    return TRUE;
}

SEDIT( sedit_type )
{
    SONG_DATA *pSong;

    EDIT_SDATA( ch, pSong );

    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Syntax:  type [typ piosenki]\n\r", ch );
        return FALSE;
    }

    /*pSong->type = ;*/

    send_to_char( "Typ piosenki ustawiony.\n\r", ch );
    return TRUE;
}

SEDIT( sedit_slots )
{
    SONG_DATA *pSong;

    EDIT_SDATA( ch, pSong );

    if ( argument[ 0 ] == '\0' || !is_number( argument ) )
    {
        send_to_char( "Syntax:  slots [liczba]\n\r", ch );
        return FALSE;
    }

    pSong->slots = atoi( argument );

    send_to_char( "Liczba slotów zajmowanych przez piosenkê ustawiona.\n\r", ch );
    return TRUE;
}

SEDIT( sedit_target )
{
    SONG_DATA *pSong;

    EDIT_SDATA( ch, pSong );

    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Syntax:  target [cel]\n\r", ch );
        return FALSE;
    }

    /*pSong->target = atoi( argument );*/

    send_to_char( "Cel piosenki ustawiony.\n\r", ch );
    return TRUE;
}

SEDIT( sedit_perform )
{
    SONG_DATA *pSong;

    EDIT_SDATA( ch, pSong );

    send_to_char( "Cel piosenki ustawiony.\n\r", ch );
    return TRUE;
}
