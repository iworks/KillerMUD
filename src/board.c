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
 * KILLER MUD is copyright 1999-2013 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: board.c 11987 2013-01-23 13:56:44Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/board.c $
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
#include <time.h>
#include "merc.h"
#include "lang.h"
#include "clans.h"
#include "recycle.h"


bool fread_object args (( FILE *fp ));
#define BOARD_DIR "../boards"
#define T2000 -1                        /* Terminator for files... */

void	load_board( BOARD_DATA *board );
void save_board( CHAR_DATA * ch, unsigned int vnum );
void show_message( CHAR_DATA *ch , int mess_num, OBJ_DATA * obj );
void show_contents( CHAR_DATA * ch, OBJ_DATA * obj );
void show_message1( CHAR_DATA *ch , int mess_num, OBJ_DATA * obj );

void load_board( BOARD_DATA *board )
{
    FILE * board_file;
    char buf[ MAX_STRING_LENGTH ];
    char * word;
    char letter;
    time_t message_time;
    time_t expiry_time;
    MESSAGE_DATA *message, *tmpm = NULL;
    OBJ_INDEX_DATA *pObj;


    if ( board == NULL )
        return ;

    if ( ( pObj = get_obj_index( board->vnum ) ) == NULL )
    {
        bug( "Load_boards: no such board object!", 0 );
        return ;
    }

    sprintf( buf, "%s/board.%i", BOARD_DIR, board->vnum );

    if ( ( board_file = fopen( buf, "r" ) ) != NULL )
    {
        for ( ; ; )
        {
            if ( feof( board_file ) )
                break;

            word = fread_word( board_file );

            if ( !str_cmp( word, "Messages" ) )
            {
                fread_to_eol( board_file );
                break;
            }

        }


        if ( pObj->value[ 0 ] > 0 )
            expiry_time = time( NULL ) - ( pObj->value[ 0 ] ) * 3600 * 24;
        else
            expiry_time = 0;


        for ( ; ; )
        {
            if ( feof( board_file ) )
                break;

            letter = fread_letter( board_file );

            if ( letter == 'S' )
                break;

            if ( letter != 'M' )
            {
                bug( "Letter in message file not M", 0 );
                break;
            }


            message_time = ( time_t ) fread_number( board_file );

            if ( feof( board_file ) )
                break;

            if ( message_time < expiry_time )
            {
                char * dumpme;

                dumpme = fread_string( board_file );
                free_string( dumpme );
                dumpme = fread_string( board_file );
                free_string( dumpme );
                dumpme = fread_string( board_file );
                free_string( dumpme );
            }
            else
            {
                CREATE( message, MESSAGE_DATA, 1 );
                message->datetime = message_time;
                message->author = fread_string( board_file );
                message->title = fread_string( board_file );
                message->message = fread_string( board_file );
                message->next = NULL;
                message->vnum = pObj->vnum;

                if ( board->message == NULL )
                    board->message = message;
                else
                {
                    for ( tmpm = board->message;tmpm->next;tmpm = tmpm->next );
                    tmpm->next = message;
                }
            }
        }


        fclose( board_file );
        return ;
    }


    return ;
}


void do_write( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA * object;
	BOARD_DATA * board;
	MESSAGE_DATA * msg, *tmp;
	OBJ_INDEX_DATA * pObj;
	int vnum, intel, bloker = 0;

	char buf[ MAX_STRING_LENGTH ];

	char arg1[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, arg1 );

	if ( IS_NPC( ch ) )
	{
		send_to_char( "Tylko gracze moga mazac po tablicy.\n\r", ch );
		return ;
	}

	if ( arg1[ 0 ] == '\0' )
	{
		send_to_char( "Na czym chcesz pisaæ?\n\r", ch );
		return ;
	}
	else
	{
		object = get_obj_carry( ch, arg1, ch );

		// je¿eli obiekt nieistnieje w inventory, to mo¿e kto¶ co¶ takiego trzyma
		// w ³apce? z czego próbuje siê napiæ, warto sprawdziæ.

		if ( object == NULL ) object = get_obj_wear( ch, arg1, FALSE );

		if ( object == NULL )
		{
			if ( ( object = get_obj_list( ch, arg1, ch->in_room->contents ) ) == NULL )
			{
				send_to_char( "Huh? Na czym chcesz pisaæ?\n\r", ch );
				return ;
			}
		}
	}

	switch ( object->item_type )
	{
		default:
			send_to_char( "Na tym nic nie napiszesz.\n\r", ch );
			return ;

		case ITEM_BOARD:
			if ( argument[ 0 ] == '\0' )
			{
				send_to_char( "Jaki ma byæ tytu³ twojej wiadomo¶ci?\n\r", ch );
				return ;
			}

			pObj = object->pIndexData;
			vnum = pObj->vnum;

			for ( board = board_system; board != NULL; board = board->next )
			{
				if ( board->vnum == vnum )
					break;
			}

			if ( board == NULL )
			{
				send_to_char( "Ta tablica jest chwilowo niesprawna.\n\r.", ch );
				return ;
			}


			if ( pObj->value[ 2 ] > get_trust( ch ) )
			{
				send_to_char( "Nie mo¿esz pisaæ na tej tablicy.\n\r", ch );
				return ;
			}

			for ( tmp = board->message;tmp;tmp = tmp->next )
				bloker++;

			if ( bloker > 255 )
			{
				send_to_char( "Na tej tablicy nie ma ju¿ wolnego miejsca.\n\r", ch );
				bug( "TABLICA %d zapchana!!!", vnum );
				return ;
			}

			msg = new_msg();

			msg->datetime = time( NULL );

			sprintf( buf, "{B[%-32s]{x {W%-20s{x", argument, ( char * ) ctime( &current_time ) );

			if ( msg->title != NULL )
				free_string( msg->title );

			msg->title = str_dup( buf );

			if ( msg->author != NULL )
				free_string( msg->author );

			msg->author = str_dup( ch->name );
			msg->message = NULL;
			msg->next = NULL;
			msg->vnum = vnum;

			ch->desc->pEdit = ( MESSAGE_DATA * ) msg;
			ch->desc->editor = ED_MSG;

			string_append( ch, &msg->message );
			break ;

		case ITEM_NOTEPAPER:

			/* No, newbiasy nie beda pisac... */
			if ( ch->level <= 5 )
			{
				send_to_char( "Przed jakimikolwiek próbami literackimi zdob±dz trochê wiêcej do¶wiadczenia.\n\r", ch );
				return ;
			}

			/* Ani polinteligencji... */
			intel = get_curr_stat_deprecated(ch,STAT_INT);

			if ( intel <= 13 )
			{
				send_to_char( "Yyy... Nie masz pojêcia co mo¿esz z tym zrobiæ.\n\r", ch );
				return ;
			}

			/* Sprawdzamy - zapisane/niezapisane */
			if ( object->value[ 0 ] != 0 )
			{
				send_to_char( "Przecie¿ tu jest ju¿ co¶ napisane!\n\r", ch );
				return ;
			}

			smash_tilde(argument);

			/* Flagujemy notke, jako zapisana.  */
			object->value[ 0 ] = 1;

			object->value[ 1 ] = knows_language( ch, ch->speaking, NULL );
			object->value[ 2 ] = ch->speaking;

			/* Komunikaty.  */
			act( "Przygotowujesz siê do napisania czego¶ na $k.", ch, object, NULL, TO_CHAR );
			act( "$n spogl±da uwa¿nie na $h, chyba ma zamiar co¶ napisaæ.", ch, object, NULL, TO_ROOM );
			//speakswell = 2*intel;
			free_string( object->item_description );
			string_append( ch, &object->item_description );
			/*sbuf = object->item_description;
			sbuf = translate( speakswell, sbuf, "wspólny" );
			object->item_description = format_string( sbuf );*/
			break;
	}
}


void do_read( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA * obj = NULL;
    BOARD_DATA *brd = NULL;
    char arg1[ MAX_STRING_LENGTH ];
    sh_int msg_num;

    if ( IS_NPC( ch ) )
        return ;


    argument = one_argument( argument, arg1 );


    if ( argument[ 0 ] == '\0' )
    {
        if ( !is_number( arg1 ) )
        {
            send_to_char( "Co chcesz przeczytaæ?\n\r", ch );
            return ;
        }
    }
    else
    {
        obj = get_obj_carry( ch, arg1, ch );
        if ( !obj )
            obj = get_obj_wear( ch, arg1, FALSE );
        if ( !is_number( argument ) || ( obj == NULL ) )
        {
            send_to_char( "Co chcesz przeczytaæ?\n\r", ch );
            return ;
        }
    }

    if ( obj )
    {
        for ( brd = board_system;brd;brd = brd->next )
            if ( brd->vnum == obj->pIndexData->vnum )
                break;

        if ( brd == NULL || brd->message == NULL )
            return ;

        msg_num = atoi( argument );
        msg_num++;

        if ( msg_num <= 1 )
            return ;

        show_message1( ch, msg_num, obj );
        return ;
    }



    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
        if ( obj->item_type == ITEM_BOARD && !IS_SET( obj->wear_flags, ITEM_TAKE ) )
            break;
    }

    if ( obj == NULL )
    {
        send_to_char( "Nie ma tutaj ¿adnej tablicy.\n\r", ch );
        return ;
    }


    for ( brd = board_system;brd;brd = brd->next )
        if ( brd->vnum == obj->pIndexData->vnum )
            break;

    if ( brd == NULL )
    {
        send_to_char( "Tablica chwilowo niesprawna.\n\r", ch );
        bug( "Board system: brak tablicy w board_system", 0 );
        return ;
    }
    if ( brd->message == NULL )
    {
        send_to_char( "Na tej tablicy nie ma ¿adnych wiadomo¶ci.\n\r", ch );
        return ;
    }

    show_message( ch, atoi( arg1 ), obj );
    return ;
}

//kasowanie dla graczy tylko swoich wiadomosci
void do_deletemesg( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA * object = NULL;
    BOARD_DATA * board = NULL;
    MESSAGE_DATA * msg = NULL, *msg_prev = NULL;
    int vnum, num, count = 0;

    if ( IS_NPC( ch ) )
        return ;

    if ( argument[ 0 ] == '\0' || !is_number( argument ) )
    {
        send_to_char( "Sk³adnia: delete numer.\n\r", ch );
        return ;
    }

    num = atoi( argument );
    num--;

    for ( object = ch->in_room->contents ; object != NULL; object = object -> next_content )
    {
        if ( object->item_type == ITEM_BOARD )
            break;
    }

    if ( object == NULL )
    {
        send_to_char( "Nie ma tutaj ¿adnej tablicy.\n\r.", ch );
        return ;
    }

    vnum = object->pIndexData->vnum;

    /* szukamy wpisu na liscie talbic*/
    for ( board = board_system; board != NULL; board = board->next )
    {
        if ( board->vnum == vnum )
            break;
    }

    if ( board == NULL )
    {
        send_to_char( "Tablica nieprawid³owa. Proszê powiadomiæ Lorda.\n\r.", ch );
        bug( "Board system: brak wpisu tablicy w board_system", 0 );
        return ;
    }
    /*ok mamy element w liscie tablic*/
    else
    {
        if ( !board->message )
        {
            send_to_char( "Ta tablica jest pusta.\n\r", ch );
            return ;
        }

        if ( num == 0 )  /*pierwszy element do usuniecia*/
        {
            msg = board->message;
            if ( str_cmp( msg->author, ch->name ) )
            {
                send_to_char( "Mo¿esz kasowaæ tylko wiadomo¶ci napisane przez siebie.\n\r", ch );
                return ;
            }

            board->message = board->message->next;
            free_msg( msg );
            msg = NULL;
            send_to_char( "Wiadomo¶æ usuniêta.\n\r", ch );
            save_board( ch, vnum );
            return ;
        }

        msg_prev = NULL;
        msg = board->message;
        msg_prev = msg;

        for ( ;msg;msg = msg->next, count++ )
        {
            if ( count == num )  /*element do usuniecia*/
            {
                if ( str_cmp( msg->author, ch->name ) )
                {
                    send_to_char( "Mo¿esz kasowaæ tylko wiadomo¶ci napisane przez siebie.\n\r", ch );
                    return ;
                }
                msg_prev->next = msg->next;
                msg->next = NULL;
                free_msg( msg );
                msg = NULL;
                send_to_char( "Wiadomo¶c usuniêta.\n\r", ch );
                save_board( ch, vnum );
                return ;
            }
            msg_prev = msg;
        }
        send_to_char( "Nie ma takiej wiadomo¶ci.\n\r", ch );
        return ;
    }
    return ;
}

void show_message( CHAR_DATA *ch , int mess_num, OBJ_DATA * obj )
{

    BOARD_DATA * board;
    OBJ_INDEX_DATA * pObj;
    int vnum;
    MESSAGE_DATA * msg;
    int cnt = 0;
    bool mfound = FALSE;
    char buf[ MAX_STRING_LENGTH ];
    char to_check[ MAX_INPUT_LENGTH ];
    char * to_person;
    char private_name[ MAX_INPUT_LENGTH ];
    BUFFER *buffer = NULL;

    pObj = obj->pIndexData;
    vnum = pObj->vnum;

    for ( board = board_system; board != NULL; board = board->next )
        if ( board->vnum == vnum )
            break;

    if ( board == NULL || board->message == NULL )
        return ;

    if ( pObj->value[ 1 ] > get_trust( ch ) )
    {
        send_to_char( "Nie mo¿esz czytaæ og³oszeñ z tej tablicy.\n\r", ch );
        return ;
    }


    buffer = new_buf();

    for ( msg = board->message; msg != NULL; msg = msg->next )
    {
        if ( ++cnt == mess_num )
        {
            mfound = TRUE;

            to_person = one_argument( msg->title, to_check );
            if ( !str_cmp( to_check + 3, "toclan:" ) &&
                 ( !ch->pcdata->clan ||
                 str_prefix( to_person, ch->pcdata->clan->who_name ) ) &&
                 str_cmp( msg->author, ch->name ) )
            {
                send_to_char( "To jest wiadomo¶æ klanu, do którego nie nale¿ysz.\n\r", ch );
                break;
            }
            else
            {
                to_person = one_argument( to_person, private_name );

                if ( !str_cmp( to_check + 3, "to:" )
                     && str_prefix( private_name, ch->name )
                     && str_cmp( msg->author, ch->name ) )
                {
                    send_to_char( "To jest prywatna wiadomo¶æ.\n\r", ch );
                    break;
                }
            }
            sprintf( buf, "{W[%d]{x {G%10s{x : %s\n\r\n\r%s\n\r",
                     cnt, msg->author, msg->title , msg->message );
            add_buf( buffer, buf );
            page_to_char( buf_string( buffer ), ch );
            break;
        }
    }

    if ( !mfound )
    {
        send_to_char( "Nie ma takiej wiadomo¶ci!\n\r", ch );
    }

    if ( buffer != NULL )
        free_buf( buffer );

    return ;
}

void show_message1( CHAR_DATA *ch , int mess_num, OBJ_DATA * obj )
{

    BOARD_DATA * board;
    OBJ_INDEX_DATA * pObj;
    int vnum;
    MESSAGE_DATA * msg;
    int cnt = 0;
    bool mfound = FALSE;
    /*   char           buf[MAX_STRING_LENGTH*2];*/
    BUFFER *buffer = NULL;

    pObj = obj->pIndexData;
    vnum = pObj->vnum;

    for ( board = board_system; board != NULL; board = board->next )
        if ( board->vnum == vnum )
            break;

    if ( board == NULL || board->message == NULL )
        return ;

    if ( pObj->value[ 1 ] > get_trust( ch ) )
    {
        send_to_char( "Próbujesz co¶ przeczytaæ, ale nie jeste¶ w stanie nic zrozumieæ.\n\r", ch );
        return ;
    }



    buffer = new_buf();

    for ( msg = board->message; msg != NULL; msg = msg->next )
    {
        if ( ++cnt == mess_num )
        {
            mfound = TRUE;
            add_buf( buffer, msg->message );
            page_to_char( buf_string( buffer ), ch );
            break;
        }
    }

    if ( !mfound )
        send_to_char( "Nie ma nic do czytania!\n\r", ch );

    if ( buffer != NULL )
        free_buf( buffer );

    return ;
}


void show_contents( CHAR_DATA * ch, OBJ_DATA * obj )
{
    MESSAGE_DATA * msg;
    BOARD_DATA *board = NULL, *new_board = NULL;
    char buf[ MAX_INPUT_LENGTH ];
    int cnt = 0;
    BUFFER *buffer = NULL; /*to samo co w show_message*/

    for ( board = board_system; board != NULL; board = board->next )
        if ( board->vnum == obj->pIndexData->vnum )
            break;

    if ( board == NULL )
    {
        for ( board = board_system;board->next;board = board->next );
        CREATE( new_board, BOARD_DATA, 1 );

        new_board->vnum = obj->pIndexData->vnum;
        new_board->message = NULL;
        new_board->next = NULL;
        board->next = new_board;
        board = new_board;
        return ;
    }


    /*po tym beda rozrozniane tablice od ksiag*/

    if ( CAN_WEAR( obj, ITEM_TAKE ) )
    {
        if ( obj->pIndexData->value[ 1 ] > get_trust( ch ) )
        {
            send_to_char( "Próbujesz co¶ wyczytaæ, ale nie jeste¶ w stanie nic zrozumieæ.\n\r", ch );
            return ;
        }
        show_message1( ch, 1, obj );
        return ;
    }
    else
    {
        if ( obj->pIndexData->value[ 1 ] > get_trust( ch ) )
        {
            send_to_char( "Próbujesz co¶ wyczyta¶, ale nie jeste¶ w stanie nic zrozumieæ.\n\r", ch );
            return ;
        }

    }



    send_to_char( "Wpisz READ <num> ¿eby przeczytaæ wiadomo¶æ.\n\r", ch );
    send_to_char( "Wpisz WRITE <tablica> <tytu³> ¿eby zapisaæ wiadomo¶æ.\n\r", ch );
    send_to_char( "Wpisz WRITE <tablica> to: <imiê_gracza> tylko do konkretnej osoby.\n\r", ch );
    send_to_char( "Wpisz WRITE <tablica> toclan: <nazwa klanu na who> tylko do konkretnego klanu.\n\r", ch );
    send_to_char( "Wpisz DELETE <num> aby skasowaæ notkê napisan± przez siebie.\n\r", ch );
    send_to_char(
        "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\r", ch );


    for ( msg = board->message; msg != NULL; msg = msg->next )
    {
        if ( cnt == 0 )
            buffer = new_buf();

        cnt++;
        sprintf( buf, "{R[%3d] {G%10s{x : %-20s",
                 cnt, msg->author, msg->title );

        /*tu wstawka do odwrotnego wyswietlania tablic*/
        if ( EXT_IS_SET( ch->act, PLR_REVBOARD ) )
            add_bufx( buffer, buf );
        else
            add_buf( buffer, buf );

    }

    if ( cnt == 0 )  /* then there were no messages here */
    {
        send_to_char( "         Nie ma ¿adnej wiadomo¶ci!\n\r", ch );
        send_to_char(
            "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\r", ch );

    }
    else
    {
        add_buf( buffer, "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\r" );
        page_to_char( buf_string( buffer ), ch );
        free_buf( buffer );
    }
    return ;
}



void save_board( CHAR_DATA * ch, unsigned int vnum )
{
    char buf[ MAX_STRING_LENGTH ];
    FILE * board_file;
    MESSAGE_DATA * msg = NULL;
    BOARD_DATA * brd = NULL;

    for ( brd = board_system; brd ; brd = brd->next )
        if ( brd->vnum == vnum )
            break;

    if ( brd == NULL )
    {
        bug( "Board system : unknow vnum %d", vnum );
        return ;
    }

    if ( brd->message == NULL )
    {
        bug( "Board system : empty message list", 0 );
        return ;
    }

    fclose( fpReserve );


    sprintf( buf, "%s/board.%i", BOARD_DIR, brd->vnum );

    if ( ( board_file = fopen( buf, "w" ) ) == NULL )
    {
        bug( "Board system : Could not save file board.%i.", brd->vnum );
        fpReserve = fopen( NULL_FILE, "r" );
        return ;
    }

    /* Now print messages */
    fprintf( board_file, "Messages\n" );

    for ( msg = brd->message; msg; msg = msg->next )
    {
        fprintf( board_file, "M%i\n", ( int ) ( msg->datetime ) );

        strcpy( buf, msg->author ); /* Must do copy, not allowed to change string directly */
        smash_tilde( buf );
        fprintf( board_file, "%s~\n", buf );

        strcpy( buf, msg->title );
        smash_tilde( buf );
        fprintf( board_file, "%s~\n", buf );

        strcpy( buf, msg->message );
        smash_tilde( buf );
        fprintf( board_file, "%s~\n", buf );

    }

    fprintf( board_file, "S\n" );
    fclose( board_file );
    fpReserve = fopen( NULL_FILE, "r" );
    return ;
}

/*komenda wizoska*/
void do_board( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA * object = NULL;
    BOARD_DATA * board = NULL;
    MESSAGE_DATA * msg = NULL, *msg_prev = NULL;
    int vnum, num, count = 0;

    char arg[ MAX_STRING_LENGTH ];

    if ( IS_NPC( ch ) )
        return ;

    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "board <edit|delete> numer.\n\r", ch );
        return ;
    }

    argument = one_argument( argument, arg );

    if ( str_prefix( arg, "delete" ) && str_prefix( arg, "edit" ) )
    {
        send_to_char( "board <edit|delete> numer.\n\r", ch );
        return ;
    }

    if ( argument[ 0 ] == '\0' )
    {
			send_to_char( "Nie poda³<&e/a/o>¶ drugiego argumentu.\n\r", ch );
      return ;
    }

    if ( !is_number( argument ) )
    {
        send_to_char( "Drugi argument powinien byæ cyfr±.\n\r", ch );
        return ;
    }

    num = atoi( argument );
    num--;

    for ( object = ch->in_room->contents ; object != NULL; object = object -> next_content )
    {
        if ( object->item_type == ITEM_BOARD )
            break;
    }

    if ( object == NULL )
    {
        send_to_char( "Nie ma tutaj ¿adnej tablicy.\n\r.", ch );
        return ;
    }

    if ( !IS_BUILDER( ch, object->pIndexData->area ) )
    {
        send_to_char( "No way!!!\n\r", ch );
        return ;
    }


    vnum = object->pIndexData->vnum;

    /* szukamy wpisu na liscie talbic*/
    for ( board = board_system; board != NULL; board = board->next )
    {
        if ( board->vnum == vnum )
            break;
    }

    if ( board == NULL )
    {
        send_to_char( "Tablica nieprawid³owa. Proszê powiadomiæ Lorda.\n\r.", ch );
        bug( "Board system: brak wpisu tablicy w board_system", 0 );
        return ;
    }
    /*ok mamy element w liscie tablic*/
    else if ( !str_prefix( arg, "delete" ) )
    {
        if ( !board->message )
        {
            send_to_char( "Ta tablica jest pusta.\n\r", ch );
            return ;
        }

        if ( num == 0 )  /*pierwszy element do usuniecia*/
        {
            msg = board->message;
            board->message = board->message->next;
            free_msg( msg );
            msg = NULL;
            send_to_char( "Msg deleted.\n\r", ch );
            save_board( ch, vnum );
            return ;
        }

        msg_prev = NULL;
        msg = board->message;
        msg_prev = msg;

        for ( ;msg;msg = msg->next, count++ )
        {
            if ( count == num )  /*element do usuniecia*/
            {
                msg_prev->next = msg->next;
                msg->next = NULL;
                free_msg( msg );
                msg = NULL;
                send_to_char( "Wiadomo¶c zosta³a skasowana.\n\r", ch );
                save_board( ch, vnum );
                return ;
            }
            msg_prev = msg;
        }
        send_to_char( "Nie ma takiej wiadomo¶ci.\n\r", ch );
        return ;
    }
    else
    {
        for ( msg = board->message;msg;msg = msg->next, count++ )
        {
            if ( count == num )
            {
                ch->desc->pEdit = ( MESSAGE_DATA * ) msg;
                ch->desc->editor = ED_MSG1;
                string_append( ch, &msg->message );
                return ;
            }
        }
        send_to_char( "Nie ma takiej wiadomo¶ci.\n\r", ch );
        return ;
    }
    return ;
}
