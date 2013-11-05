#ifdef NEW_NANNY
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
#include <stdarg.h>
#include "merc.h"
#include "recycle.h"
#include "nanny.h"

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )             \
                if ( !str_cmp( word, literal ) ) \
                {                                \
                    field  = value;              \
                    fMatch = TRUE;               \
                    break;                       \
                }

bool load_account( ACCOUNT_DATA **account, char * email )
{
	char strsave[ MAX_INPUT_LENGTH ];
    char characters[ MAX_STRING_LENGTH ];
    FILE * acc_fp;
    char * word;
    bool fMatch;

    CREATE( (*account), ACCOUNT_DATA, 1 );
    sprintf( characters, " " );

    (*account)->email      = str_dup( email );
    (*account)->passwd     = str_dup( "" );
    (*account)->characters = str_dup( " " );
    (*account)->activation = str_dup( "" );

    sprintf( strsave, "%s%s", ACCOUNT_DIR, uncapitalize( email ) );
    if ( ( acc_fp = fopen( strsave, "r" ) ) != NULL )
    {
        for ( ; ; )
        {
            word   = feof( acc_fp ) ? "End" : fread_word( acc_fp );
            fMatch = FALSE;

            if ( !str_cmp( word, "End" ) )
            {
                fclose( acc_fp );
                free_string( (*account)->characters );
                (*account)->characters = str_dup( characters );
                return TRUE;
            }

            switch ( UPPER(word[0]) )
            {
                case '*':
                    fMatch = TRUE;
                    fread_to_eol( acc_fp );
                    break;
                case 'A':
                    KEY( "Activation", (*account)->activation, fread_string( acc_fp ) );
                    break;
                case 'C':
                    if ( !str_cmp( word, "Char" ) )
                    {
                        sprintf( characters, "%s%s ", characters, fread_word( acc_fp ) );
                        fMatch = TRUE;
                        break;
                    }
                    break;
                case 'P':
                    KEY( "Pass", (*account)->passwd, fread_string( acc_fp ) );
                    break;
            }

            if ( !fMatch )
            {
                bug( "Load_account: no match.", 0 );
                bug( word, 0 );
                fread_to_eol( acc_fp );
            }
        }
        fclose( acc_fp );
    }

    return FALSE;
}

void save_account( ACCOUNT_DATA * account )
{
    char strsave[ MAX_INPUT_LENGTH ];
    char *characters;
    char one_char[ MAX_STRING_LENGTH ];
    FILE *fp;

    if ( !account || IS_NULLSTR( account->email ) )
        return;

    fclose( fpReserve );

    sprintf( strsave, "%s%s", ACCOUNT_DIR, uncapitalize( account->email ) );
    if ( ( fp = fopen( TEMP_FILE, "w" ) ) == NULL )
    {
        bug( "Save_account: fopen", 0 );
        perror( strsave );
    }
    else
    {
        fprintf( fp, "Pass    %s~\n", account->passwd );
        if ( !IS_NULLSTR( account->activation ) )
            fprintf( fp, "Activation    %s~\n", account->activation );

        characters = one_argument( account->characters, one_char );
        while ( !IS_NULLSTR( one_char ) )
        {
            fprintf( fp, "Char    %s\n", capitalize( one_char ) );
            characters = one_argument( characters, one_char );
        }
        fprintf( fp, "End\n" );
    }

    fclose( fp );
    rename( TEMP_FILE, strsave );
    fpReserve = fopen( NULL_FILE, "r" );
    return ;
}

bool account_exists( char * account )
{
    char strsave [ MAX_INPUT_LENGTH ];
    FILE * file;

    sprintf( strsave, "%s%s", ACCOUNT_DIR, uncapitalize( account ) );

    fclose ( fpReserve );
    file = fopen ( strsave, "r" );

    if ( file )
    {
        fclose ( file );
        fpReserve = fopen( NULL_FILE, "r" );
        return TRUE;
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return FALSE;
}

bool validate_email( const char* email )
{
    char * cat = index( email, '@' );
    char * cdot = index( email, '.' );
    int len = strlen( email );
    int i, at, dot;
    const char * chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-.@";

    if ( len < 6 )
        return FALSE;

    if ( cat != rindex( email, '@' ) )
        return FALSE;

    if ( cat )
        at = cat - email;
    else
        return FALSE;

    if ( cdot )
        dot = cdot - email;
    else
        return FALSE;

    for( i = 0; i < len; i++ )
    {
        if ( !index( chars, email[i] ) )
            return FALSE;
    }

    if ( at <= 0 || at > len - 4 )
        return FALSE;

    if ( !isalnum( email[at-1] ) || !isalnum( email[at+1] ) )
        return FALSE;

    if ( email[0] == '.' || email[len-1] == '.' || email[0] == '-' || email[len-1] == '-' )
        return FALSE;

    return TRUE;
}

bool send_mail_message( const char* to, const char* subject, const char* text )
{
    FILE* f;
    char command[1024];
    char arg[MAX_INPUT_LENGTH];

    sprintf(command,"/usr/bin/sendmail %s", to);
    f = popen(command,"w");
    if ( !f )
        return FALSE;
    sprintf( arg, "From: noreply@%s\n", GENERIC_ADDRESS );
    fputs( arg, f );
    sprintf( arg, "To: %s\n", to );
    fputs( arg, f );
    sprintf( arg, "Subject: %s\n", subject );
    fputs( arg, f );
    sprintf( arg, "Content-Type: text/plain; charset=iso-8859-2\n" );
    fputs( arg, f );

    fputs(text,f);

    pclose(f);
    return TRUE;
}

bool add_player_to_account( ACCOUNT_DATA *account, char *name )
{
    CHAR_DATA *rch;
    char char_list[MAX_STRING_LENGTH];

    if ( ( rch = load_char_remote( name ) ) != NULL )
    {
        if ( !rch->pcdata )
        {
            free_char( rch );
            return FALSE;
        }

        sprintf( char_list, "%s %s ", account->characters, name );
        free_string( account->characters );
        account->characters = str_dup( char_list );
        save_account( account );
        free_string( rch->pcdata->account_email );
        rch->pcdata->account_email = str_dup( account->email );
        save_char_obj( rch, TRUE, TRUE );
        free_char( rch );
        return TRUE;
    }

    return FALSE;
}

void remove_player_from_account( ACCOUNT_DATA *account, char *name )
{
    char new_char_list[MAX_STRING_LENGTH];
    char character[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char *char_list;

    sprintf( new_char_list, " " );
    char_list = account->characters;
    char_list = one_argument( char_list, character );

   if ( character[0] == '\0' )
        return;

    do
    {
        if ( str_cmp( name, character ) )
        {
            sprintf( buf, "%s ", character );
            strcat( new_char_list, buf );
        }
        char_list = one_argument( char_list, character );
    }
    while ( !IS_NULLSTR( character ) );

    free_string( account->characters );
    account->characters = str_dup( new_char_list );
    save_account( account );

    return;
}

void change_account_email( ACCOUNT_DATA *account, char *new_email )
{
    CHAR_DATA *rch;
    char character[MAX_STRING_LENGTH];
    char *char_list;

    free_string( account->email );
    account->email = str_dup( new_email );

    save_account( account );

    char_list = account->characters;
    char_list = one_argument( char_list, character );

    if ( character[0] == '\0' )
        return;

    do
    {
        rch = load_char_remote( character );

        if ( !rch || !rch->pcdata )
            continue;

        free_string( rch->pcdata->account_email );
        rch->pcdata->account_email = str_dup( new_email );

        save_char_obj( rch, FALSE, TRUE );
        free_char( rch );

        char_list = one_argument( char_list, character );
    }
    while ( !IS_NULLSTR( character ) );

    return;
}

/* sprawdzenie czy z mudem s± po³±czone inne postaci z konta dcheck->account */
char* check_account_playing( DESCRIPTOR_DATA *dcheck )
{
    char name[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *vch;

    if ( !dcheck || !dcheck->character || !dcheck->account || IS_NULLSTR( dcheck->account->email ) )
        return NULL;

    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected < 0 || d == dcheck || !d->character || !d->account || IS_NULLSTR( d->account->email ) )
            continue;

        if ( !str_cmp( d->account->email, dcheck->account->email ) )
            return d->character->name;
    }

    for ( vch = char_list; vch; vch = vch->next )
    {
        if ( IS_NPC( vch ) || vch == dcheck->character || IS_NULLSTR( vch->name ) || ( vch->desc && vch->desc->connected <= NANNY_FIRST_INPUT ) )
            continue;

        sprintf( name, " %s ", vch->name );
        if ( !str_infix( name, dcheck->account->characters ) )
            return vch->name;
    }
    return NULL;
}
#endif
