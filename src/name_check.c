/*
* $Id: name_check.c 7666 2009-07-03 12:04:22Z illi $
*/
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
#include "name_check.h"
#include "recycle.h"
#include "interp.h"

//tablica hashujaca
static NAME_LIST * name_table[ MAX_HASH ];

void save_file( int type );

//oblicza numer 'hasha'
int hash_number( char *ptr )
{
    int hash_value = 0, letter;

    for ( letter = 0; ptr[ letter ] != '\0'; letter++ )
        hash_value += ptr[ letter ];

    return ( hash_value % MAX_HASH );
}

//resetuje tablice i wczytuje z plikow dane
void initialize_name_table()
{
    int i;

    for ( i = 0; i < MAX_HASH; i++ )
        name_table[ i ] = NULL;

    load_files();
    return ;
}

// 1    -> jest dozwolone
// == 0 -> brak wpisu
// -1   -> niedozwolone
// 2    -> nowy wpis
int is_allowed( char *name )
{
    NAME_LIST * list;

    if ( IS_NULLSTR( name ) )
        return NAME_NOTEXIST;

    list = name_table[ hash_number( name ) ];
    for ( ; list; list = list->next )
        if ( NOPOL( name[ 0 ] ) == NOPOL( list->name[ 0 ] ) &&
             !str_cmp( name, list->name ) )
            return list->allowed;

    return NAME_NOTEXIST;
}

NAME_LIST * get_name_list( char *name )
{
    NAME_LIST * list;

    list = name_table[ hash_number( name ) ];

    for ( ; list; list = list->next )
        if ( NOPOL( name[ 0 ] ) == NOPOL( list->name[ 0 ] ) &&
             !str_cmp( name, list->name ) )
            return list;

    return NULL;
}

//dodanie nowego elementu
void add_name( char *name, int type )
{
    NAME_LIST * new_el = NULL, *ptr = NULL;
    int hash;

    if ( !name || name[ 0 ] == '\0' )
        return ;

    switch ( type )
    {
        case NAME_ALLOWED:
        case NAME_DISALLOWED:
        case NAME_NEW: break;
        default: return ;
    }

    switch ( is_allowed( name ) )
    {
        case NAME_NEW:
            if ( ( ptr = get_name_list( name ) ) == NULL )
                break;

            if ( type == NAME_NEW )
                return ;

            ptr->allowed = type;
            return ;
        case NAME_ALLOWED:
            if ( ( ptr = get_name_list( name ) ) == NULL )
                break;

            if ( type == NAME_NEW )
                return ;

            ptr->allowed = type;
            return ;
        case NAME_DISALLOWED:
            if ( ( ptr = get_name_list( name ) ) == NULL )
                break;

            if ( type == NAME_NEW )
                return ;

            ptr->allowed = type;
            return ;
        case NAME_NOTEXIST: break;
    }

    //wstawianie nowego elementu
    new_el	= ( NAME_LIST * ) malloc( sizeof( NAME_LIST ) );
    new_el->allowed	= type;
    new_el->name	= strdup( name );
    new_el->name[ 0 ] = LOWER( new_el->name[ 0 ] );
    hash	= hash_number( name );
    new_el->next	= name_table[ hash ];
    name_table[ hash ] = new_el;
    return ;
}

//komenda wizowska do wstawiania dozwolonych/niedozwolonych nazw
void do_names( CHAR_DATA *ch, char *argument )
{
	char arg [ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char buf[ MAX_STRING_LENGTH ];
	int allowed;
	NAME_LIST *ptr;
	BUFFER *buffer = NULL;

	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Komenda s³u¿y do zatwierdzenia lub odrzucenia imienia.\n\r", ch );
		send_to_char( "names allow 'name'            - dane imiê jest poprawne\n\r", ch );
		send_to_char( "names deny 'name' [komentarz] - dane imiê nie jest poprawne\n\r", ch );
		send_to_char( "list                          - lista nowych imion\n\r", ch );
		return ;
	}

	if ( !str_prefix( arg, "allow" ) )
	{
		if ( arg2[ 0 ] == '\0' )
		{
			send_to_char( "Sk³adnia:\n\r", ch );
			send_to_char( "names allow <name>            - dane imiê jest poprawne\n\r", ch );
			send_to_char( "names allow all               - akceptuje wszystkie nowe imiona\n\r", ch );
			send_to_char( "names deny <name> [komentarz] - dane imiê nie jest poprawne\n\r", ch );
			return ;
		}

		if ( !str_cmp( arg2, "all" ) )
		{
			CHAR_DATA * vch;
			int i, count = 0;

			for ( i = 0; i < MAX_HASH; i++ )
			{
				for ( ptr = name_table[ i ]; ptr; ptr = ptr->next )
				{
					if ( ptr->allowed != NAME_NEW )
						continue;

					add_name( ptr->name, NAME_ALLOWED );

					if ( ( vch = get_char_world( ch, ptr->name ) ) != NULL && !IS_NPC( vch ) )
					{
						EXT_REMOVE_BIT( vch->act, PLR_UNREGISTERED );
						send_to_char( "{GTwoje imiê zosta³o zaakceptowane{x.\n\r", vch );
					}
					count++;
				}
			}

			save_file( NAME_ALLOWED );
			save_file( NAME_NEW );
			print_char( ch, "Zaakceptowano %d nowych imion.\n\r", count );
			return;
		}

		allowed = is_allowed( arg2 );

		if ( allowed == NAME_NOTEXIST )
		{
			CHAR_DATA * vch;

			print_char( ch, "Dodano nowy wpis.\n\r" );
			add_name( arg2, NAME_ALLOWED );
			save_file( NAME_ALLOWED );

			if ( ( vch = get_char_world( ch, arg2 ) ) != NULL && !IS_NPC( vch )
					&& EXT_IS_SET( vch->act, PLR_UNREGISTERED ) )
			{
				EXT_REMOVE_BIT( vch->act, PLR_UNREGISTERED );
				send_to_char( "{GTwoje imie zostalo zaakceptowane{x.\n\r", vch );
			}
			return ;
		}
		else if ( allowed == NAME_NEW )
		{
			CHAR_DATA * vch;

			print_char( ch, "Zaakceptowano nowe imiê '%s'.\n\r", arg2 );
			add_name( arg2, NAME_ALLOWED );
			save_file( NAME_ALLOWED );
			save_file( NAME_NEW );

			if ( ( vch = get_char_world( ch, arg2 ) ) != NULL && !IS_NPC( vch ) )
			{
				EXT_REMOVE_BIT( vch->act, PLR_UNREGISTERED );
				send_to_char( "{GTwoje imiê zosta³o zaakceptowane{x.\n\r", vch );
			}
		}
		else
		{
			ptr = get_name_list( arg2 );

			if ( !ptr )
				return ;

			if ( ptr->allowed == NAME_DISALLOWED )
			{
				print_char( ch, "%s teraz jest dozwolone.\n\r", arg2 );
				add_name( arg2, NAME_ALLOWED );
				save_file( NAME_DISALLOWED );
				save_file( NAME_ALLOWED );
			}
			else
				print_char( ch, "%s jest ju¿ dozwolone.\n\r", arg2 );
		}
	}
	else if ( !str_prefix( arg, "deny" ) )
	{

		if ( arg2[ 0 ] == '\0' )
		{
			send_to_char( "Sk³adnia:\n\r", ch );
			send_to_char( "names allow 'name'            - dane imiê jest poprawne\n\r", ch );
			send_to_char( "names deny 'name' [komentarz] - dane imiê nie jest poprawne\n\r", ch );
			return ;
		}

		allowed = is_allowed( arg2 );

		if ( allowed == NAME_NOTEXIST )
		{
			CHAR_DATA * vch;

			print_char( ch, "Dodano nowy wpis. \n\r" );
			add_name( arg2, NAME_DISALLOWED );
			save_file( NAME_DISALLOWED );

			if ( ( vch = get_char_world( ch, arg2 ) ) != NULL && !IS_NPC( vch ) && vch->level < 30 )
			{
				send_to_char( "{RTwoje imiê nie zosta³o zaakceptowane!\n\r", vch );
				if ( argument[ 0 ] != '\0' )
					print_char( vch, "{RKomentarz: %s{x\n\r", argument );
				if ( vch->level > 1 )
					send_to_char( "{RPo³±cz siê jeszcze raz na t± postaæ aby zmieniæ imiê na inne.{x\n\r", vch );

				free_string( vch->pcdata->name_deny_txt );
				vch->pcdata->name_deny_txt = str_dup( argument );

				save_char_obj( vch, FALSE, FALSE );
				stop_fighting( vch, TRUE );
				do_function( vch, &do_quit, "" );
				return ;
			}

			if ( ( vch = load_char_remote( arg2 ) ) != NULL )
			{
				free_string( vch->pcdata->name_deny_txt );
				vch->pcdata->name_deny_txt = str_dup( argument );
				save_char_obj( vch, FALSE, TRUE );
				free_char( vch );
			}
			return ;
		}
		else if ( allowed == NAME_NEW )
		{
			CHAR_DATA * vch;

			print_char( ch, "Nie zaakceptowano nowego imienia '%s'.\n\r", arg2 );
			add_name( arg2, NAME_DISALLOWED );
			save_file( NAME_DISALLOWED );
			save_file( NAME_NEW );

			if ( ( vch = get_char_world( ch, arg2 ) ) != NULL && !IS_NPC( vch ) && vch->level < 30 )
			{
				send_to_char( "{RTwoje imiê nie zosta³o zaakceptowane!\n\r", vch );
				if ( argument[ 0 ] != '\0' )
					print_char( vch, "{RKomentarz: %s{x\n\r", argument );
				if ( vch->level > 1 )
					send_to_char( "{RPo³±cz siê jeszcze raz na t± postaæ aby zmieniæ imiê na inne.{x\n\r", vch );

				free_string( vch->pcdata->name_deny_txt );
				vch->pcdata->name_deny_txt = str_dup( argument );

				save_char_obj( vch, FALSE, FALSE );
				stop_fighting( vch, TRUE );
				do_function( vch, &do_quit, "" );
				return ;
			}

			if ( ( vch = load_char_remote( arg2 ) ) != NULL )
			{
				free_string( vch->pcdata->name_deny_txt );
				vch->pcdata->name_deny_txt = str_dup( argument );
				save_char_obj( vch, FALSE, TRUE );
				free_char( vch );
			}

			return ;
		}
		else
		{
			ptr = get_name_list( arg2 );

			if ( !ptr )
				return ;

			if ( ptr->allowed == NAME_ALLOWED )
			{
				print_char( ch, "%s teraz jest zabronione.\n\r", arg2 );
				add_name( arg2, NAME_DISALLOWED );
				save_file( NAME_DISALLOWED );
				save_file( NAME_ALLOWED );
			}
			else
				print_char( ch, "%s jest ju¿ zabronione.\n\r", arg2 );

		}
	}
	else if ( !str_prefix( arg, "list" ) )
	{
		NAME_LIST * ptr;
		int i;
		int col = 0;
		bool allowed = FALSE, disallowed = FALSE, namenew = TRUE;

//              BUGFIX. Mysle jednak, ze nie ma sensu wyswietlac listy imion allowed i deny, bo sa za dlugie, wiec wylaczam od razu.
/*		if ( arg2[ 0 ] != '\0' )
		{
			if ( !str_prefix( arg2, "deny" ) )
				allowed = FALSE, namenew = FALSE, disallowed=TRUE;
			else if ( !str_prefix( arg2, "allow" ) )
				disallowed = FALSE, namenew = FALSE, allowed=TRUE;
			else if ( !str_prefix( arg2, "new" ) )
				disallowed = FALSE, allowed = FALSE, namenew=TRUE;
		}
*/
		if ( allowed )
		{

			if ( !buffer )
				buffer = new_buf();

			sprintf( buf, "{GDozwolone imiona:\n\r{x" );
			add_buf( buffer, buf );

			for ( i = 0; i < MAX_HASH; i++ )
				for ( ptr = name_table[ i ]; ptr; ptr = ptr->next )
				{
					if ( ptr->allowed != NAME_ALLOWED )
						continue;

					sprintf( buf, "%-20.20s ", ptr->name );
					add_buf( buffer, buf );

					col++;

					if ( col > 2 )
					{
						add_buf( buffer, "\n\r" );
						col = 0;
					}
				}

			if ( col != 0 )
			{
				add_buf( buffer, "\n\r" );
				col = 0;
			}
		}


		if ( disallowed )
		{

			if ( !buffer )
				buffer = new_buf();

			sprintf( buf, "{RNiedozwolone imiona:{x\n\r" );
			add_buf( buffer, buf );

			for ( i = 0; i < MAX_HASH; i++ )
				for ( ptr = name_table[ i ]; ptr; ptr = ptr->next )
				{
					if ( ptr->allowed != NAME_DISALLOWED )
						continue;

					sprintf( buf, "%-20.20s ", ptr->name );
					add_buf( buffer, buf );

					col++;

					if ( col > 2 )
					{
						add_buf( buffer, "\n\r" );
						col = 0;
					}
				}

			if ( col != 0 )
			{
				add_buf( buffer, "\n\r" );
				col = 0;
			}
		}


		if ( namenew )
		{
			if ( !buffer )
				buffer = new_buf();

			sprintf( buf, "{RNowe imiona:{x\n\r" );
			add_buf( buffer, buf );

			for ( i = 0; i < MAX_HASH; i++ )
				for ( ptr = name_table[ i ]; ptr; ptr = ptr->next )
				{
					if ( ptr->allowed != NAME_NEW )
						continue;

					sprintf( buf, "%-20.20s ", ptr->name );
					add_buf( buffer, buf );
					col++;

					if ( col > 2 )
					{
						add_buf( buffer, "\n\r" );
						col = 0;
					}
				}

			if ( col != 0 )
			{
				add_buf( buffer, "\n\r" );
				col = 0;
			}
		}

		if ( buffer )
		{
			page_to_char( buf_string( buffer ), ch );
			free_buf( buffer );
		}
	}
	else
	{
		send_to_char( "Sk³adnia:\n\r", ch );
		send_to_char( "names allow 'name'            - dane imiê jest poprawne\n\r", ch );
		send_to_char( "names deny 'name' [komentarz] - dane imiê nie jest poprawne\n\r", ch );
		return ;
	}
}

void save_file( int type )
{
    FILE * fp;
    int hash;
    NAME_LIST *list;
    char *filename;
    bool empty = TRUE;

    fclose( fpReserve );

    switch ( type )
    {
        case NAME_ALLOWED: filename = ALLOWED_NAMES_FILE;	break;
        case NAME_DISALLOWED: filename = DISALLOWED_NAMES_FILE;	break;
        case NAME_NEW: filename = NEW_NAMES_FILE;	break;
        default: return ;
    }
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
        return ;

    for ( hash = 0; hash < MAX_HASH; hash++ )
        for ( list = name_table[ hash ]; list; list = list->next )
            if ( list->allowed == type )
            {
                //if( strlen(list->name) < 4)
                //    continue;

                fprintf( fp, "%s\n", list->name );

                if ( empty )
                    empty = FALSE;
            }

    fclose( fp );

    //lata, jesli nic nie bylo w pliku, to nie zapisywac
    if ( empty )
        remove( filename );

    fpReserve = fopen( NULL_FILE, "r" );
    return ;
}

// wczytywanie obu plikow
void load_files()
{
    FILE * fp;
    char *name;

    if ( ( fp = fopen( DISALLOWED_NAMES_FILE, "r" ) ) != NULL )
    {
        for ( ; ; )
        {
            if ( feof( fp ) )
                break;

            name = fread_word( fp );
            fread_to_eol( fp );
            add_name( name, NAME_DISALLOWED );
        }
        fclose( fp );
    }

    if ( ( fp = fopen( ALLOWED_NAMES_FILE, "r" ) ) != NULL )
    {
        for ( ; ; )
        {
            if ( feof( fp ) )
                break;

            name = fread_word( fp );
            fread_to_eol( fp );
            add_name( name, NAME_ALLOWED );
        }
        fclose( fp );
    }

    if ( ( fp = fopen( NEW_NAMES_FILE, "r" ) ) != NULL )
    {
        for ( ; ; )
        {
            if ( feof( fp ) )
                break;

            name = fread_word( fp );
            fread_to_eol( fp );
            add_name( name, NAME_NEW );
        }
        fclose( fp );
    }

    return ;
}
