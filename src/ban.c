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
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"

BAN_DATA *ban_list;

void save_bans( void )
{
    BAN_DATA * pban;
    FILE *fp;
    bool found = FALSE;

    fclose( fpReserve );
    if ( ( fp = fopen( BAN_FILE, "w" ) ) == NULL )
    {
        perror( BAN_FILE );
    }

    for ( pban = ban_list; pban != NULL; pban = pban->next )
    {
        found = TRUE;
        fprintf( fp, "%s %s %s %ld %d %s\n",
                     pban->site,
                     print_flags( pban->ban_flags ),
                     pban->person,
                     pban->date_stamp,
                     pban->period,
                     pban->reason );
    }

    fprintf( fp, "#\n" );

    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    if ( !found )
        unlink( BAN_FILE );
}

void load_bans( void )
{
    FILE * fp;
    BAN_DATA *ban_last;
    char * site;

    if ( ( fp = fopen( BAN_FILE, "r" ) ) == NULL )
        return ;

    ban_last = NULL;
    for ( ; ; )
    {
        BAN_DATA *pban;
        if ( feof( fp ) )
        {
            fclose( fp );
            return ;
        }

        pban = new_ban();

        site = fread_word( fp );
        if ( !str_cmp( site, "#" ) )
        {
            fclose( fp );
            return ;
        }

        pban->site = str_dup( site );
        pban->ban_flags = fread_flag( fp );
        pban->person = str_dup( fread_word( fp ) );
        pban->date_stamp = fread_long_number( fp );
        pban->period = fread_number( fp );
        pban->reason = fread_string_eol( fp );

        if ( pban->period > 0 && pban->date_stamp + ( pban->period * 24 * 3600 ) <= current_time )
        {
            free_ban( pban );
            continue;
        }

        if ( ban_list == NULL )
            ban_list = pban;
        else
            ban_last->next = pban;
        ban_last = pban;
    }
}

bool check_ban( char *site, int type )
{
    BAN_DATA * pban;
    char host[ MAX_STRING_LENGTH ];

    strcpy( host, capitalize( site ) );
    host[ 0 ] = LOWER( host[ 0 ] );

    for ( pban = ban_list; pban != NULL; pban = pban->next )
    {
        if ( !IS_SET( pban->ban_flags, type ) )
            continue;

        if ( IS_SET( pban->ban_flags, BAN_PREFIX )
             && IS_SET( pban->ban_flags, BAN_SUFFIX )
             && strstr( pban->site, host ) != NULL )
            return TRUE;

        if ( IS_SET( pban->ban_flags, BAN_PREFIX )
             && !str_suffix( pban->site, host ) )
            return TRUE;

        if ( IS_SET( pban->ban_flags, BAN_SUFFIX )
             && !str_prefix( pban->site, host ) )
            return TRUE;
    }

    return FALSE;
}


void ban_site( CHAR_DATA *ch, char *argument )
{
	char date[ 50 ];
	struct tm *btime;
    char buf[ MAX_STRING_LENGTH ], buf2[ MAX_STRING_LENGTH ], buf3[ MAX_STRING_LENGTH ];
    char arg1[ MAX_INPUT_LENGTH ], arg2[ MAX_INPUT_LENGTH ], arg3[ MAX_INPUT_LENGTH ];
    char *name;
    BUFFER *buffer;
    BAN_DATA *pban, *prev;
    bool prefix = FALSE, suffix = FALSE;
    int type;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[ 0 ] == '\0' )
    {
        if ( ban_list == NULL )
        {
            send_to_char( "Nie ma ¿adnych banów.\n\r", ch );
            return ;
        }
        buffer = new_buf();

        add_buf( buffer, "{CAdres                                       Kto        Typ{x\n\r" );
        for ( pban = ban_list;pban != NULL;pban = pban->next )
        {
            sprintf( buf2, "%s%s%s",
                     IS_SET( pban->ban_flags, BAN_PREFIX ) ? "*" : "",
                     pban->site,
                     IS_SET( pban->ban_flags, BAN_SUFFIX ) ? "*" : "" );

            btime = localtime( &pban->date_stamp );
            strftime( date, 20, "%d-%m-%Y %H:%M:%S", btime );

            if ( pban->period <= 0 )
                sprintf( buf3, "od %s na sta³e", date );
            else if ( pban->period == 1 )
                sprintf( buf3, "od %s na 1 dzieñ", date );
            else
                sprintf( buf3, "od %s na %d dni", date, pban->period );

            sprintf( buf, "{Y%-40s{x    %-10s %-7s\n\r{CCzas{x: %s\n\r{CPowód{x: %s\n\r",
                     buf2, pban->person,
                     IS_SET( pban->ban_flags, BAN_NEWBIES ) ? "newbies" :
                     IS_SET( pban->ban_flags, BAN_PERMIT ) ? "permit" :
                     IS_SET( pban->ban_flags, BAN_ALL ) ? "all"	: "",
                     buf3,
                     IS_NULLSTR(pban->reason) ? "nie podano" : pban->reason );
            add_buf( buffer, buf );
            if ( pban->next )
            add_buf( buffer, "\n\r" );
        }

        add_buf( buffer, "\n\rDodanie nowego bana:\n\r" );
        add_buf( buffer, "permban [*]<adres>[*] all|newbies|permit <ile dni> [<powod>]\n\r" );
        add_buf( buffer, "- podanie 0 przy liczbie dni oznacza ban na sta³e\n\r" );
        page_to_char( buf_string( buffer ), ch );
        free_buf( buffer );
        return ;
    }

    /* find out what type of ban */
    if ( !str_prefix( arg2, "permit" ) )
        type = BAN_PERMIT;
    else if ( !str_prefix( arg2, "newbies" ) )
        type = BAN_NEWBIES;
    else if ( !str_prefix( arg2, "all" ) )
        type = BAN_ALL;
    else
    {
        send_to_char( "Dopuszczalne typy bana to: all, newbies, permit.\n\r", ch );
        return ;
    }

    name = arg1;

    if ( name[ 0 ] == '*' )
    {
        prefix = TRUE;
        name++;
    }

    if ( name[ strlen( name ) - 1 ] == '*' )
    {
        suffix = TRUE;
        name[ strlen( name ) - 1 ] = '\0';
    }

    if ( strlen( name ) == 0 )
    {
        send_to_char( "Nie poda³e¶ adresu do zbanowania.\n\r", ch );
        return ;
    }

    if ( arg3[0] == '\0' || !is_number( arg3 ) )
    {
        send_to_char( "Podaj liczbê dni przez jak± ma trwaæ ban (0 - ca³y czas).\n\r", ch );
        return;
    }

    prev = NULL;
    for ( pban = ban_list; pban != NULL; prev = pban, pban = pban->next )
    {
        if ( !str_cmp( name, pban->site ) )
        {
            if ( prev == NULL )
                ban_list = pban->next;
            else
                prev->next = pban->next;
            free_ban( pban );
        }
    }

    pban = new_ban();
    pban->site   = str_dup( name );
    pban->person = str_dup( ch->name );
    pban->date_stamp = current_time;
    pban->period = UMAX( 0, atoi( arg3 ) );
    pban->reason = str_dup( argument );

    /* set ban type */
    pban->ban_flags = type;

    if ( prefix )
        SET_BIT( pban->ban_flags, BAN_PREFIX );
    if ( suffix )
        SET_BIT( pban->ban_flags, BAN_SUFFIX );

    pban->next = ban_list;
    ban_list = pban;
    save_bans();
    sprintf( buf, "Adres %s zosta³ dodany do listy banów.\n\r", pban->site );
    send_to_char( buf, ch );
    return ;
}

void do_permban( CHAR_DATA *ch, char *argument )
{
    ban_site( ch, argument );
}

void do_allow( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    BAN_DATA *prev;
    BAN_DATA *curr;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Który ban usun±æ z listy?\n\r", ch );
        return ;
    }

    prev = NULL;
    for ( curr = ban_list; curr != NULL; prev = curr, curr = curr->next )
    {
        if ( !str_cmp( arg, curr->site ) )
        {
            if ( prev == NULL )
                ban_list = ban_list->next;
            else
                prev->next = curr->next;

            free_ban( curr );
            sprintf( buf, "Ban na adres %s zdjêty.\n\r", arg );
            send_to_char( buf, ch );
            save_bans();
            return ;
        }
    }

    send_to_char( "Tego adresu nie ma na li¶cie banów.\n\r", ch );
    return ;
}

void update_bans()
{
    BAN_DATA *prev;
    BAN_DATA *curr;

    prev = NULL;
    for ( curr = ban_list; curr != NULL; prev = curr, curr = curr->next )
    {
        if ( curr->period > 0 && curr->date_stamp + ( curr->period * 24 * 3600 ) <= current_time )
        {
            if ( prev == NULL )
                ban_list = ban_list->next;
            else
                prev->next = curr->next;

            free_ban( curr );
            save_bans();
            return ;
        }
    }

    return ;
}
