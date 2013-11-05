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
 *  File: string.c                                                     *
 *                                                                     *
 *  Much time and thought has gone into this software and you are      *
 *  benefitting.  We hope that you share your changes too.  What goes  *
 *  around, comes around.                                              *
 *                                                                     *
 *  This code was freely distributed with the The Isles 1.1 source     *
 *  code, and has been used here for OLC - OLC would not be what it is *
 *  without all the previous coders who released their source code.    *
 *                                                                     *
 ***********************************************************************
 *                                                                     *
 * KILLER MUD is copyright 1999-2011 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 *   ZMIENIA£E¦ CO¦? DOPISZ SIÊ!                                       *
 *                                                                     *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Koper Tadeusz         (garloop@killer-mud.net        ) [Garloop   ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Skrzetnicki Krzysztof (gtener@gmail.com              ) [Tener     ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (tzdziech@gmail.com            ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: string.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/string.c $
 *
 */

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
#include "pcre.h"
#include "merc.h"
#include "tables.h"
#include "olc.h"
#include "recycle.h"

char *string_linedel( char *, int );
char *string_lineadd( char *, char *, int );
char *numlineas( char * );
void menu_show	args( ( CHAR_DATA *ch ) );
/*****************************************************************************
 Name:		string_edit
 Purpose:	Clears string and puts player into editing mode.
 Called by:	none
 ****************************************************************************/
void string_edit( CHAR_DATA *ch, char **pString )
{
    send_to_char( "-===========- Tryb EDYCJI tekstu -============-\n\r", ch );
    send_to_char( " Wpisz .h na pocz±tku nowej linii - pomoc\n\r", ch );
    send_to_char( " Koñczenie - znak @ w nowej linii.\n\r", ch );
    send_to_char( "-=============================================-\n\r", ch );

    if ( *pString == NULL )
    {
        *pString = str_dup( "" );
    }
    else
    {
        **pString = '\0';
    }

    ch->desc->pString = pString;

    return ;
}



/*****************************************************************************
 Name:		string_append
 Purpose:	Puts player into append mode for given string.
 Called by:	(many)olc_act.c
 ****************************************************************************/
void string_append( CHAR_DATA *ch, char **pString )
{
    send_to_char( "-=========- Tryb DODAWANIA tekstu -===========-\n\r", ch );
    send_to_char( " Wpisz .h na pocz±tku nowej linii - pomoc\n\r", ch );
    send_to_char( " Koñczenie - znak @ w nowej linii.\n\r", ch );
    send_to_char( "-=============================================-\n\r", ch );

    if ( *pString == NULL )
    {
        *pString = str_dup( "" );
    }
    send_to_char( numlineas( *pString ), ch );

    /* numlineas entrega el string con \n\r */
    /*  if ( *(*pString + strlen( *pString ) - 1) != '\r' )
    	send_to_char( "\n\r", ch ); */

    ch->desc->pString = pString;

    return ;
}



/*****************************************************************************
 Name:		string_replace
 Purpose:	Substitutes one string for another.
 Called by:	string_add(string.c) (aedit_builder)olc_act.c.
 ****************************************************************************/
char * string_replace( char * orig, char * old, char * new )
{
    char xbuf[ MAX_STRING_LENGTH ];
    int i;

    xbuf[ 0 ] = '\0';
    strcpy( xbuf, orig );
    if ( strstr( orig, old ) != NULL )
    {
        i = strlen( orig ) - strlen( strstr( orig, old ) );
        xbuf[ i ] = '\0';
        strcat( xbuf, new );
        strcat( xbuf, &orig[ i + strlen( old ) ] );
        free_string( orig );
    }

    return str_dup( xbuf );
}



/*****************************************************************************
 Name:		string_add
 Purpose:	Interpreter for string editing.
 Called by:	game_loop_xxxx(comm.c).
 ****************************************************************************/
void string_add( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];

    /*
     * Thanks to James Seng
     */
    smash_tilde( argument );
    if ( *argument == '.' )
    {
        char arg1 [ MAX_INPUT_LENGTH ];
        char arg2 [ MAX_INPUT_LENGTH ];
        char arg3 [ MAX_INPUT_LENGTH ];
        char tmparg3 [ MAX_INPUT_LENGTH ];
        char aftercom [ MAX_INPUT_LENGTH ];

        argument = one_argument( argument, arg1 );
        strcpy( aftercom, argument );
        argument = first_arg( argument, arg2, FALSE );
        strcpy( tmparg3, argument );
        argument = first_arg( argument, arg3, FALSE );

        if ( !str_cmp( arg1, ".c" ) )
        {
            send_to_char( "Tekst wyczyszczony.\n\r", ch );
            free_string( *ch->desc->pString );
            *ch->desc->pString = str_dup( "" );
            return ;
        }

        if ( !str_cmp( arg1, ".s" ) )
        {
            send_to_char( "String so far:\n\r", ch );
            send_to_char( numlineas( *ch->desc->pString ), ch );
            return ;
        }

        if ( !str_cmp( arg1, ".e" ) )
        {
            if ( ch->desc->connected == CON_PLAYING )
                interpret ( ch, aftercom );
            return ;
        }

        if ( !str_cmp( arg1, ".r" ) )
        {
            if ( arg2[ 0 ] == '\0' )
            {
                send_to_char(
                    "usage:  .r \"old string\" \"new string\"\n\r", ch );
                return ;
            }

            *ch->desc->pString =
                string_replace( *ch->desc->pString, arg2, arg3 );
            sprintf( buf, "'%s' replaced with '%s'.\n\r", arg2, arg3 );
            send_to_char( buf, ch );
            return ;
        }

        if ( !str_cmp( arg1, ".f" ) )
        {
        	if ( ch->desc->editor == ED_MPCODE || ch->desc->editor == ED_OPCODE || ch->desc->editor == ED_RPCODE
						|| ch->desc->editor == ED_TRAPDATA )
        		*ch->desc->pString = format_code( *ch->desc->pString );
        	else
            	*ch->desc->pString = format_string( *ch->desc->pString );
            send_to_char( "Tekst sformatowany.\n\r", ch );
            return ;
        }

        if ( !str_cmp( arg1, ".ld" ) )
        {
            *ch->desc->pString = string_linedel( *ch->desc->pString, atoi( arg2 ) );
            send_to_char( "Linia usuniêta.\n\r", ch );
            return ;
        }

        if ( !str_cmp( arg1, ".li" ) )
        {
            *ch->desc->pString = string_lineadd( *ch->desc->pString, tmparg3, atoi( arg2 ) );
            send_to_char( "Linia wklejona.\n\r", ch );
            return ;
        }

        if ( !str_cmp( arg1, ".lr" ) )
        {
            *ch->desc->pString = string_linedel( *ch->desc->pString, atoi( arg2 ) );
            *ch->desc->pString = string_lineadd( *ch->desc->pString, tmparg3, atoi( arg2 ) );
            send_to_char( "Linia zastapiona.\n\r", ch );
            return ;
        }

        if ( !str_cmp( arg1, ".h" ) )
        {
            send_to_char( "Sedit help (commands on blank line):   \n\r", ch );
            send_to_char( ".r 'old' 'new'   - zastêpuje tekst 'old' 'new'\n\r", ch );
            send_to_char( "                   (requires '', \"\") \n\r", ch );
            send_to_char( ".h               - pomoc (to co w³a¶nie widzisz)\n\r", ch );
            send_to_char( ".s               - poka¿ tekst \n\r", ch );
            send_to_char( ".f               - (zawijanie) formatowanie tekstu\n\r", ch );
            send_to_char( ".c               - czyszczenie tekstu \n\r", ch );
            send_to_char( ".ld <num>        - usuwanie linii o numerze <num>\n\r", ch );
            send_to_char( ".li <num> <str>  - wklejanie tekstu <str> do <num> linii\n\r", ch );
            send_to_char( ".lr <num> <str>  - zastêpowanie linii <num> tekstem <str>\n\r", ch );
            send_to_char( ".e <komenda>     - wykona komendê mudow± (na przyk³ad look)\n\r", ch );
            send_to_char( "@                - koniec edycji     \n\r", ch );
            return ;
        }

        send_to_char( "SEdit:  Invalid dot command.\n\r", ch );
        return ;
    }

    if ( *argument == '@' )
    {
        /* oprog syf*/
        if ( ch->desc->editor == ED_OPCODE )
        {
            OBJ_INDEX_DATA * obj;
            int hash;
            PROG_LIST *opl;
            PROG_CODE *opc;

            EDIT_OPCODE( ch, opc );


            if ( opc != NULL )
                for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
                    for ( obj = obj_index_hash[ hash ]; obj; obj = obj->next )
                        for ( opl = obj->progs; opl; opl = opl->next )
                            if ( FAST_STR_CMP( opl->name, opc->name ) && opl->code != opc->code )
                            {
                                sprintf( buf, "Uaktualnianie obiektu %d.\n\r", obj->vnum );
                                send_to_char( buf, ch );
                                opl->code = opc->code;
                            }
        }

        /* room prog syf*/
        if ( ch->desc->editor == ED_RPCODE )
        {
            ROOM_INDEX_DATA * room; /*dodane*/
            int hash;
            /*dodane*/
            PROG_LIST *opl;
            PROG_CODE *opc;

            EDIT_RPCODE( ch, opc );


            if ( opc != NULL )
                for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
                    for ( room = room_index_hash[ hash ]; room; room = room->next )
                        for ( opl = room->progs; opl; opl = opl->next )
                            if ( FAST_STR_CMP( opl->name, opc->name ) && opl->code != opc->code )
                            {
                                sprintf( buf, "Uaktualnianie lokacji %d.\n\r", room->vnum );
                                send_to_char( buf, ch );
                                opl->code = opc->code;
                            }
        }


        /* mobprog syf */
        if ( ch->desc->editor == ED_MPCODE )
        {
            MOB_INDEX_DATA * mob;
            int hash;
            PROG_LIST *mpl;
            PROG_CODE *mpc;



            EDIT_MPCODE( ch, mpc );

            if ( mpc != NULL )
                for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
                    for ( mob = mob_index_hash[ hash ]; mob; mob = mob->next )
                        for ( mpl = mob->progs; mpl; mpl = mpl->next )
                            if ( FAST_STR_CMP( mpl->name, mpc->name ) && mpl->code != mpc->code )
                            {
                                sprintf( buf, "Uaktualnianie moba %d.\n\r", mob->vnum );
                                send_to_char( buf, ch );
                                mpl->code = mpc->code;
                            }
        }

        /* msg 1 */
        if ( ch->desc->editor == ED_MSG1 )
        {
            MESSAGE_DATA * msg = NULL;

            msg = ( MESSAGE_DATA * ) ch->desc->pEdit;


            ch->desc->editor = 0;
            ch->desc->pEdit = NULL;
            save_board( ch, msg->vnum );
        }

        if ( ch->desc->editor == ED_MSG )
        {
            MESSAGE_DATA * msg = NULL, *tmp = NULL;
            BOARD_DATA *brd = NULL;

            msg = ( MESSAGE_DATA * ) ch->desc->pEdit;
            send_to_char( msg->message, ch );

            if ( msg->message != NULL )
            {
                /*anulowanie pisania na tablicy*/
                if ( !str_cmp( msg->message, "" ) )
                {
                    free_msg( msg );
                    ch->desc->pEdit = NULL;
                    ch->desc->editor = 0;
                    ch->desc->pString = NULL;
                    return ;
                }
            }

            for ( brd = board_system;brd;brd = brd->next )
                if ( brd->vnum == msg->vnum )
                    break;

            if ( brd != NULL )
            {

                if ( brd->message == NULL )
                    brd->message = msg;
                else
                {
                    for ( tmp = brd->message;tmp->next;tmp = tmp->next );
                    tmp->next = msg;
                }

            }

            ch->desc->pEdit = NULL;
            ch->desc->editor = 0;
            save_board( ch, msg->vnum );
        }

        if ( ch->desc->editor == ED_MAP && !IS_NULLSTR( ch->desc->custom_map ) )
        {
            map_init();
            string_to_map( ch );
            show_map( ch );
            ch->desc->pEdit = NULL;
            ch->desc->editor = 0;
        }

        ch->desc->pString = NULL;
        if ( ch->desc->connected == CON_MENU )
        {
            save_char_obj( ch, FALSE, FALSE );
            menu_show( ch );
        }
        return ;
    }

    strcpy( buf, *ch->desc->pString );

    /*
     * Truncate strings to MAX_STRING_LENGTH.
     * --------------------------------------
     */
    if ( strlen( buf ) + strlen( argument ) >= ( MAX_STRING_LENGTH - 4 ) )
    {
        send_to_char( "String too long, last line skipped.\n\r", ch );

        /* Force character out of editing mode. */
        ch->desc->pString = NULL;
        return ;
    }

    /*
     * Ensure no tilde's inside string.
     * --------------------------------
     */
    smash_tilde( argument );

    strcat( buf, argument );
    strcat( buf, "\n\r" );
    free_string( *ch->desc->pString );
    *ch->desc->pString = str_dup( buf );
    return ;
}



/*
 * Thanks to Kalgen for the new procedure (no more bug!)
 * Original wordwrap() written by Surreality.
 */
/*****************************************************************************
 Name:		format_string
 Purpose:	Special string formating and word-wrapping.
 Called by:	string_add(string.c) (many)olc_act.c
 ****************************************************************************/
char *format_string( char *oldstring /*, bool fSpace */ )
{
    char xbuf[ MAX_STRING_LENGTH ];
    char xbuf2[ MAX_STRING_LENGTH ];
    char *rdesc;
    int i = 0;
    int j = 0;
    //  int k=0;
    bool cap = TRUE;

    xbuf[ 0 ] = xbuf2[ 0 ] = 0;

    i = 0;

    for ( rdesc = oldstring; *rdesc; rdesc++ )
    {
        if ( *rdesc == '\n' )
        {
            if ( xbuf[ i - 1 ] != ' ' )
            {
                xbuf[ i ] = ' ';
                i++;
            }
        }
        else if ( *rdesc == '\r' )
            ;
        else if ( *rdesc == ' ' )
        {
            if ( xbuf[ i - 1 ] != ' ' )
            {
                xbuf[ i ] = ' ';
                i++;
            }
        }
        else if ( *rdesc == ')' )
        {
            if ( xbuf[ i - 1 ] == ' ' && xbuf[ i - 2 ] == ' ' &&
                 ( xbuf[ i - 3 ] == '.' || xbuf[ i - 3 ] == '?' || xbuf[ i - 3 ] == '!' ) )
            {
                xbuf[ i - 2 ] = *rdesc;
                xbuf[ i - 1 ] = ' ';
                xbuf[ i ] = ' ';
                i++;
            }
            else
            {
                xbuf[ i ] = *rdesc;
                i++;
            }
        }
        else if ( ( *rdesc == '.' && *( rdesc + 1 ) != '\0' && *( rdesc + 1 ) != '.' ) || *rdesc == '?' || *rdesc == '!' )
        {
            if ( xbuf[ i - 1 ] == ' ' && xbuf[ i - 2 ] == ' ' &&
                 ( xbuf[ i - 3 ] == '.' || xbuf[ i - 3 ] == '?' || xbuf[ i - 3 ] == '!' ) )
            {
                xbuf[ i - 2 ] = *rdesc;
                if ( *( rdesc + 1 ) != '\"' )
                {
                    xbuf[ i - 1 ] = ' ';
                    xbuf[ i ] = ' ';
                    i++;
                }
                else
                {
                    xbuf[ i - 1 ] = '\"';
                    xbuf[ i ] = ' ';
                    xbuf[ i + 1 ] = ' ';
                    i += 2;
                    rdesc++;
                }
            }
            else
            {
                xbuf[ i ] = *rdesc;
                if ( *( rdesc + 1 ) != '\"' )
                {
                    xbuf[ i + 1 ] = ' ';
                    /*          xbuf[i+2]=' ';
                              i += 3;*/
                    i += 2;
                }
                else
                {
                    xbuf[ i + 1 ] = '\"';
                    xbuf[ i + 2 ] = ' ';
                    xbuf[ i + 3 ] = ' ';
                    i += 4;
                    rdesc++;
                }
            }
            cap = TRUE;
        }
        else
        {
            xbuf[ i ] = *rdesc;
            if ( cap )
            {
                cap = FALSE;
                xbuf[ i ] = UPPER( xbuf[ i ] );
            }
            i++;
        }
    }
    xbuf[ i ] = 0;
    strcpy( xbuf2, xbuf );

    rdesc = xbuf2;

    xbuf[ 0 ] = 0;

    for ( ; ; )
    {

        for ( j = 0, i = 0; j < 77; i++, j++ )
        {
            if ( !*( rdesc + i ) )
                break;
            if ( *( rdesc + i ) == '{' && *( rdesc + i + 1 ) != '{' && *( rdesc + i + 1 ) != '\0' )
                i += 2;
            if ( *( rdesc + i ) == '^' && *( rdesc + i + 1 ) != '^' && *( rdesc + i + 1 ) != '\0' )
                i += 2;
        }

        if ( i < 77 )
        {
            break;
        }

        for ( i = ( xbuf[ 0 ] ? i - 1 : i - 4 ) ; i ; i-- )
        {
            if ( *( rdesc + i ) == ' ' )
                break;
        }

        if ( i )
        {
            *( rdesc + i ) = 0;
            strcat( xbuf, rdesc );
            strcat( xbuf, "\n\r" );
            rdesc += i + 1;
            while ( *rdesc == ' ' ) rdesc++;
        }
        else
        {
            bug ( "No spaces", 0 );
            *( rdesc + 75 ) = 0;
            strcat( xbuf, rdesc );
            strcat( xbuf, "-\n\r" );
            rdesc += 76;
        }
    }
    while ( *( rdesc + i ) && ( *( rdesc + i ) == ' ' ||
                                *( rdesc + i ) == '\n' ||
                                *( rdesc + i ) == '\r' ) )
        i--;
    *( rdesc + i + 1 ) = 0;
    strcat( xbuf, rdesc );
    if ( xbuf[ strlen( xbuf ) - 2 ] != '\n' )
        strcat( xbuf, "\n\r" );

    free_string( oldstring );
    return ( str_dup( xbuf ) );
}

/*****************************************************************************
 Name:		format_code
 Purpose:	Formatowanie kodów programów (dok³adniej to wstawianie wciêæ)
 Called by:	string_add(string.c)
 ****************************************************************************/
char *format_code( char *oldstring )
{
	char keyword[ MAX_STRING_LENGTH ];
    char xbuf[ MAX_STRING_LENGTH ];
    char *rdesc;
    int i = 0;
    int j = 0;
    int spaces = 0;
    int indent = 0;
    bool new_line = TRUE;

    xbuf[ 0 ] = '\0';

    i = 0;

    for ( rdesc = oldstring; *rdesc; rdesc++ )
    {
    	while( new_line && *rdesc == ' ' )
    		rdesc++;

    	if ( new_line && *rdesc != '\n' && *rdesc != '\r' )
    	{
	    	indent = 0;
	    	one_argument( rdesc, keyword );

	    	if ( !str_cmp( keyword, "if" ) || !str_cmp( keyword, "foreachingroup" ) || !str_cmp( keyword, "foreachinroom" ) )
	    	{
	    		indent = -2;
	    		spaces += 2;
	    	}

	    	if ( !str_prefix( "endif", keyword ) || !str_prefix( "endfor", keyword ) )
	    		spaces -= 2;

	    	if ( !str_prefix( "else", keyword ) || !str_prefix( "and", keyword ) || !str_prefix( "or", keyword ))
	    		indent = -2;

	    	spaces = UMAX( 0, spaces );

			for ( j = 0; j < spaces+indent; j++ )
				xbuf[ i++ ] = ' ';

			new_line = FALSE;
		}

		xbuf[ i++ ] = *rdesc;


        if ( *rdesc == '\n' )
        	new_line = TRUE;
    }

    xbuf[ i ] = 0;

    free_string( oldstring );
    return ( str_dup( xbuf ) );
}

/*
 * Used above in string_add.  Because this function does not
 * modify case if fCase is FALSE and because it understands
 * parenthesis, it would probably make a nice replacement
 * for one_argument.
 */
/*****************************************************************************
 Name:		first_arg
 Purpose:	Pick off one argument from a string and return the rest.
 		Understands quates, parenthesis (barring ) ('s) and
 		percentages.
 Called by:	string_add(string.c)
 ****************************************************************************/
char *first_arg( char *argument, char *arg_first, bool fCase )
{
    char cEnd;

    while ( *argument == ' ' )
        argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"'
         || *argument == '%' || *argument == '(' )
    {
        if ( *argument == '(' )
        {
            cEnd = ')';
            argument++;
        }
        else cEnd = *argument++;
    }

    while ( *argument != '\0' )
    {
        if ( *argument == cEnd )
        {
            argument++;
            break;
        }
        if ( fCase ) * arg_first = LOWER( *argument );
        else *arg_first = *argument;
        arg_first++;
        argument++;
    }
    *arg_first = '\0';

    while ( *argument == ' ' )
        argument++;

    return argument;
}




/*
 * Used in olc_act.c for aedit_builders.
 */
char * string_unpad( char * argument )
{
    char buf[ MAX_STRING_LENGTH ];
    char *s;

    s = argument;

    while ( *s == ' ' )
        s++;

    strcpy( buf, s );
    s = buf;

    if ( *s != '\0' )
    {
        while ( *s != '\0' )
            s++;
        s--;

        while ( *s == ' ' )
            s--;
        s++;
        *s = '\0';
    }

    free_string( argument );
    return str_dup( buf );
}

/* Returns a string all lowercase - Fallon*/
char *lowercase( const char *str )
{
	static char strlow[MAX_STRING_LENGTH];
	int i;

	if(!str) return NULL;

	for ( i = 0; str[i] != '\0'; i++ )
	strlow[i] = LOWER(str[i]);
	strlow[i] = '\0';
	return strlow;
}

/* Returns a string with the first letter lowercased - Fallon */
char *decap( const char *str )
{
	static char firstlow[MAX_STRING_LENGTH];
	int i;

	for ( i = 0; str[i] != '\0'; i++ )
	firstlow[i] = str[i];
	firstlow[i] = '\0';
	firstlow[0] = LOWER(firstlow[0]);
	return firstlow;
}

/* Returns a string with returns inserted - Fallon */
char *wrapstr( CHAR_DATA *ch, const char *str )
{
	static char strwrap[MAX_STRING_LENGTH];
	int i;
	int count = strlen(IS_NPC(ch) ? ch->short_descr : ch->name);

	for ( i = 0; i < strlen(str); i++ )
	{
		count++;
		if ( count > 66 && str[i] == ' ' )
		{
			strwrap[i] = '\n';
			strwrap[i+1] = '\r';
			count = 0;
		}
		else
		{
			strwrap[i] = str[i];
		}
	}
	strwrap[i] = '\0';
	return strwrap;
}

/*
 * Same as capitalize but changes the pointer's data.
 * Used in olc_act.c in aedit_builder.
 */
char * string_proper( char * argument )
{
    char * s;

    s = argument;

    while ( *s != '\0' )
    {
        if ( *s != ' ' )
        {
            *s = UPPER( *s );
            while ( *s != ' ' && *s != '\0' )
                s++;
        }
        else
        {
            s++;
        }
    }

    return argument;
}

char *string_linedel( char *string, int line )
{
    char * strtmp = string;
    char buf[ MAX_STRING_LENGTH ];
    int cnt = 1, tmp = 0;

    buf[ 0 ] = '\0';

    for ( ; *strtmp != '\0'; strtmp++ )
    {
        if ( cnt != line )
            buf[ tmp++ ] = *strtmp;

        if ( *strtmp == '\n' )
        {
            if ( *( strtmp + 1 ) == '\r' )
            {
                if ( cnt != line )
                    buf[ tmp++ ] = *( ++strtmp );
                else
                    ++strtmp;
            }

            cnt++;
        }
    }

    buf[ tmp ] = '\0';

    free_string( string );
    return str_dup( buf );
}

char *string_lineadd( char *string, char *newstr, int line )
{
    char * strtmp = string;
    int cnt = 1, tmp = 0;
    bool done = FALSE;
    char buf[ MAX_STRING_LENGTH ];

    buf[ 0 ] = '\0';

    for ( ; *strtmp != '\0' || ( !done && cnt == line ); strtmp++ )
    {
        if ( cnt == line && !done )
        {
            strcat( buf, newstr );
            strcat( buf, "\n\r" );
            tmp += strlen( newstr ) + 2;
            cnt++;
            done = TRUE;
        }

        buf[ tmp++ ] = *strtmp;

        if ( done && *strtmp == '\0' )
            break;

        if ( *strtmp == '\n' )
        {
            if ( *( strtmp + 1 ) == '\r' )
                buf[ tmp++ ] = *( ++strtmp );

            cnt++;
        }

        buf[ tmp ] = '\0';
    }

    free_string( string );
    return str_dup( buf );
}

/* buf queda con la linea sin \n\r */
char *getline_olc( char *str, char *buf )
{
    int tmp = 0;
    bool found = FALSE;

    while ( *str )
    {
        if ( *str == '\n' )
        {
            found = TRUE;
            break;
        }

        buf[ tmp++ ] = *( str++ );
    }

    if ( found )
    {
        if ( *( str + 1 ) == '\r' )
            str += 2;
        else
            str += 1;
    } /* para que quedemos en el inicio de la prox linea */

    buf[ tmp ] = '\0';

    return str;
}

char *numlineas( char *string )
{
    int cnt = 1;
    static char buf[ MAX_STRING_LENGTH * 2 ];
    char buf2[ MAX_STRING_LENGTH ], tmpb[ MAX_STRING_LENGTH ];

    buf[ 0 ] = '\0';

    while ( *string )
    {
        string = getline_olc( string, tmpb );
        sprintf( buf2, "%2d. %s\n\r", cnt++, tmpb );
        strcat( buf, buf2 );
    }

	if ( strlen( buf ) >= MAX_STRING_LENGTH )
	{
		return str_dup("Ci±g jest za d³ugi aby go wy¶wietliæ z numerami linii.");
	}

    return buf;
}

int count_colors( char * string, int max )
{
    int i;
    int colors = 0;


    if ( *string == '\0' || *( string + 1 ) == '\0' ) return 0;

    i = 1;
    do
    {
        if ( *string == '{' )
        {
            string++;
            i++;

            if ( *string == '\0' )
            {
                colors++;
                i--;
                break;
            }

            if ( *string == '{' )
            {
                colors++;
                i--;
            }
            else
            {
                colors += 2;
                i -= 2;
            }
            if ( max > 0 && i >= max ) return colors;
        }
        string++;
        i++;
        if ( max > 0 && i >= max ) return colors;
    }
    while ( *string );

    return colors;
}

int center_text( char *source, char* dest, int width )
{
    int str_width;
    int spaces;
    int i;

    str_width = strlen( source ) - count_colors( source, 0 );

    spaces = ( width - str_width ) / 2;

    if ( spaces > width ) spaces = width;

    for ( i = 0; i < spaces; i++ )
    {
        *dest = ' ';
        dest++;
    }

    while ( *source )
        * dest++ = *source++;

    *dest = '\n';
    *++dest = '\r';
    *++dest = '\0';

    return spaces;
}

#define OVECCOUNT 30    /* should be a multiple of 3 */

bool regexp_match( char *pattern, char *string )
{
    char string_trim[MAX_STRING_LENGTH];
	const char *error;
    char *s, *p;
    int rc, i;
	int erroffset;
	pcre *re;
	int ovector[OVECCOUNT];

    strcpy( string_trim, string );
    s = string_trim;

    /* wycinamy spacje z przodu */
    while ( isspace( *s ) )
        s++;

    /* wycinamy spacje z tylu */
    for ( i = strlen( s ) - 1; isspace( *( s + i ) ); i-- );

    *( s + i + 1 ) = '\0';

    /* spolskawiamy polskie znaczki */
    p = s;
    while ( *p++ )
        *p = NOPOL( *p );

	re = pcre_compile( pattern, PCRE_CASELESS, &error, &erroffset, NULL);

	if ( re == NULL )
		return FALSE;

	rc = pcre_exec( re, NULL, s, (int)strlen(s), 0, 0, ovector, OVECCOUNT);

	if ( rc < 0 )
		return FALSE;

    return TRUE;
}

char* strip_colour_codes( char *string )
{
	static char buf[ MAX_STRING_LENGTH ];
	char *point;

	buf[0] = '\0';
	point = buf;

	while ( *string )
	{
		if ( ( *string == '{' && *(string + 1) == '{' ) || ( *string == '^' && *(string + 1) == '^' ) )
			string++;
		else if ( ( *string == '{' || *string == '^' ) && *(string + 1) && *(string + 2 ) )
			string += 2;

		*point++ = *string++;
	}

	*point++ = '\0';

	return buf;
}

char* strip_spaces( char *string )
{
    static char buf[ MAX_STRING_LENGTH ];
    char *point;

    buf[0] = '\0';
    point = buf;

    while ( *string )
    {
        if ( *string == ' ' )
        {
            string++;
        }
        *point++ = *string++;
    }
    *point++ = '\0';
    return buf;
}

char *strip_dupspaces ( char *string )
{
    static char buf[ MAX_STRING_LENGTH ];
    char *point;

    buf[0] = '\0';
    point = buf;

    while ( *string )
    {
        if ( isspace( *string ) )
        {
            if ( isspace( *( string + 1 ) ) )
                string++;
            else
                *point++ = *string++;
        }
        else
            *point++ = *string++;
    }

    *point = '\0';
    return buf;
}

char *rpg_control( char *string )
{
	static char buf[MAX_INPUT_LENGTH];
    char *pName;
    int iSyl;
    int length;
    bool found;

	struct rpg_syl_type
	{
		char *	old;
		char *	new;
	};

	static const struct rpg_syl_type rpg_syl_table[] =
	{
	{ " ",		" "		},
	{ ":P",		""		},
	{ ";P",		""		},
	{ "=P",		""		},
	{ ":p",		""		},
	{ ";p",		""		},
	{ "=p",		""		},
	{ ":D",		""		},
	{ ";D",		""		},
	{ "=D",		""		},
	{ ":d",		""		},
	{ ";d",		""		},
	{ "=d",		""		},
	{ ":)",		""		},
	{ ";)",		""		},
	{ "=)",		""		},
	{ ":]",		""		},
	{ ";]",		""		},
	{ "=]",		""		},
	{ ":(",		""		},
	{ ";(",		""		},
	{ "=(",		""		},
	{ ":[",		""		},
	{ ";[",		""		},
	{ "=[",		""		},
	{ ":/",		""		},
	{ ";/",		""		},
	{ "=/",		""		},
	{ ":|",		""		},
	{ ";|",		""		},
	{ "=|",		""		},
	{ ":*",		""		},
	{ ";*",		""		},
	{ "=*",		""		},
	{ ":>",		""		},
	{ ";>",		""		},
	{ "=>",		""		},
	{ ":<",		""		},
	{ ";<",		""		},
	{ "=E",		""		},
	{ ":E",		""		},
	{ ";E",		""		},
	{ "=<",		""		},
	{ ":-P",	""		},
	{ ";-P",	""		},
	{ "=-P",	""		},
	{ ":-p",	""		},
	{ ";-p",	""		},
	{ "=-p",	""		},
	{ ":-D",	""		},
	{ ";-D",	""		},
	{ "=-D",	""		},
	{ ":-d",	""		},
	{ ";-d",	""		},
	{ "=-d",	""		},
	{ ":-)",	""		},
	{ ";-)",	""		},
	{ "=-)",	""		},
	{ ":-]",	""		},
	{ ";-]",	""		},
	{ "=-]",	""		},
	{ ":-(",	""		},
	{ ";-(",	""		},
	{ "=-(",	""		},
	{ ":-[",	""		},
	{ ";-[",	""		},
	{ "=-[",	""		},
	{ ":-/",	""		},
	{ ";-/",	""		},
	{ "=-/",	""		},
	{ ":-|",	""		},
	{ ";-|",	""		},
	{ "=-|",	""		},
	{ ":-*",	""		},
	{ ";-*",	""		},
	{ "=-*",	""		},
	{ ":->",	""		},
	{ ";->",	""		},
	{ "=->",	""		},
	{ ":-<",	""		},
	{ ";-<",	""		},
	{ "=-<",	""		},
	{ ":-E",	""		},
	{ ";-E",	""		},
	{ "=-E",	""		},

	{ ": P",		""		},
	{ "; P",		""		},
	{ "= P",		""		},
	{ ": p",		""		},
	{ "; p",		""		},
	{ "= p",		""		},
	{ ": D",		""		},
	{ "; D",		""		},
	{ "= D",		""		},
	{ ": d",		""		},
	{ "; d",		""		},
	{ "= d",		""		},
	{ ": )",		""		},
	{ "; )",		""		},
	{ "= )",		""		},
	{ ": ]",		""		},
	{ "; ]",		""		},
	{ "= ]",		""		},
	{ ": (",		""		},
	{ "; (",		""		},
	{ "= (",		""		},
	{ ": [",		""		},
	{ "; [",		""		},
	{ "= [",		""		},
	{ ": /",		""		},
	{ "; /",		""		},
	{ "= /",		""		},
	{ ": |",		""		},
	{ "; |",		""		},
	{ "= |",		""		},
	{ ": *",		""		},
	{ "; *",		""		},
	{ "= *",		""		},
	{ ": >",		""		},
	{ "; >",		""		},
	{ "= >",		""		},
	{ ": <",		""		},
	{ "; <",		""		},
	{ "= E",		""		},
	{ ": E",		""		},
	{ "; E",		""		},

/*	{ "0",	"zero "		},
	{ "1",	"jeden "	},
	{ "2",	"dwa "		},
	{ "3",	"trzy "		},
	{ "4",	"cztery "	},
	{ "5",	"piêæ "		},
	{ "6",	"sze¶æ "	},
	{ "7",	"siedem "	},
	{ "8",	"osiem "	},
	{ "9",	"dziewiêæ "	},*/

	{ "@", " ma³peczka " 		}, 	{ "%", " procent "			}, 	{ "*", " razy " 			},
	{ "#", " kratka " 			}, 	{ "^", "" 					}, 	{ "(", " otwórz nawias " 	}, { "`", "" 					},
	{ "$", " srebrnych monet "	}, 	{ "&", " oraz " 			}, 	{ ")", " zamknij nawias " 	},
	{ "_", "" 					}, 	{ "+", " plus " 			},	{ "=", " równa siê		" 	}, { "|", ""					},
	{ "[", " otwórz nawias " 	}, 	{ "]", " zamknij nawias " 	}, 	{ "{", " otwórz nawias "	}, { "}", " zamknij nawias "	},
	{ "<", " otwórz nawias " 	}, 	{ ">", " zamknij nawias " 	},	{ "/", " ³amane na " 		},


	{ "", "" }
	};

    buf[0]	= '\0';
    for ( pName = string; *pName != '\0'; pName += length )
	{
		if ( strlen( buf ) >= MAX_INPUT_LENGTH - 100 )
			break;

        found = FALSE;
		for ( iSyl = 0; (length = strlen(rpg_syl_table[iSyl].old)) != 0; iSyl++ )
		{
		    if ( !str_prefix( rpg_syl_table[iSyl].old, pName ) )
	    	{
                found = TRUE;
				strcat( buf, rpg_syl_table[iSyl].new );
				break;
			}
	    }

        if ( !found )
        {
            strncat( buf, pName, 1 );
            length = 1;
        }

		if ( length == 0 )
		    length = 1;
	}

	return buf;
}

char *gen_random_str( int min_lenght, int max_lenght )
{
    static char alpha[] = "abcdefghijklmnopqrstuvxyz";
    static char rndstr[MAX_STRING_LENGTH];
    int varlength       = number_range( min_lenght, max_lenght );
    int i, step;
    char *point;

    rndstr[0] = '\0';

    for ( i = 0; i < varlength; i++ )
    {
        point   = alpha;
        step    = number_range(0, strlen(alpha)-1);
        point   += step;

        strncat( rndstr, point, 1 );
    }
    point   = rndstr + varlength;
    *point   = '\0';

    return rndstr;
}

