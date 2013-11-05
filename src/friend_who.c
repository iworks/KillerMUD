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
 * $Id: friend_who.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/friend_who.c $
 *
 */
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "friend_who.h"

void friend_dodaj( CHAR_DATA *ch, char *name, time_t czas, bool introduced )
{
	FRIEND_LIST *wsk;
	if ( !ch || IS_NPC( ch ) ) return;
	if ( !name )
	{
		return;
	}
	wsk = (FRIEND_LIST*) malloc ( sizeof(FRIEND_LIST) );
	if ( !wsk )
	{
		bug ( "Blad alokacji pamieci w friend_who.c, dodaj", 0 );
		return;
	}
	wsk->next = ch->pcdata->friends;
	if ( ch->pcdata->friends ) ch->pcdata->friends->previous = wsk;
	wsk->previous = NULL;
	wsk->name = strdup( name );
	wsk->opis = strdup( "\0" );
	wsk->czas = czas;
	wsk->introduced = introduced;
	ch->pcdata->friends = wsk;
}

bool friend_usun( CHAR_DATA *ch, char *name )
{
	FRIEND_LIST *wsk;

	if ( !ch || IS_NPC( ch ) ) return FALSE;
	for( wsk = ch->pcdata->friends; wsk; wsk = wsk->next )
	{
		if ( !str_cmp( wsk->name, name ) )
		{
			break;
		}
	}

	if ( wsk )
	{
		if ( wsk->next ) wsk->next->previous = wsk->previous;
		if ( wsk->previous )
		{
			wsk->previous->next = wsk->next;

		}
		else
		{
			ch->pcdata->friends = wsk -> next;
		}
		free(wsk->opis);
		free(wsk->name);
		free(wsk);
		wsk = NULL;

		return TRUE;
	}

	return FALSE;
}

bool friend_exist( CHAR_DATA *ch, char *name, char *full_name, char *info, time_t *czas, bool *introduced )
{
	FRIEND_LIST *wsk;
	*introduced = FALSE;
	if ( info ) info[0] = '\0';
	if ( !ch || IS_NPC( ch ) ) return FALSE;
	for( wsk = ch->pcdata->friends; wsk; wsk = wsk->next )
	{
		if ( !str_prefix( name, wsk->name ) ) break;
	}
	if ( wsk )
	{
		if ( full_name ) strcpy( full_name, wsk->name );
		if ( info && strcmp( wsk->opis, "\0" ) ) strcpy( info, wsk->opis );
		*czas = wsk->czas;
		*introduced = wsk->introduced;
		return TRUE;
	}
	return FALSE;
}

void friend_czysc ( CHAR_DATA *ch )
{
	FRIEND_LIST *wsk, *wsk1;

	if ( !ch || IS_NPC( ch ) ) return;
	for( wsk = ch->pcdata->friends; wsk; )
	{
		wsk1 = wsk;
		wsk = wsk->next;
		free(wsk1->opis);
		free(wsk1->name);
		free(wsk1);
		wsk1 = NULL;
	}
	ch->pcdata->friends = NULL;
}

void friend_introduce( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;

	if ( !ch || IS_NPC( ch ) ) return;

	if ( ch->pcdata->introduced ) free( ch->pcdata->introduced );
	ch->pcdata->introduced = NULL;

	if ( argument[0] == '\0')
	{
		send_to_char( "Nie chcesz siê nikomu przedstawiæ?\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_SILENCE ) )
	{
		send_to_char( AFF_SILENCE_TEXT, ch );
		return ;
	}

	if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ) )
	{
		send_to_char( "Pod wod±? Nie da rady.\n\r", ch );
		switch ( ch->sex )
		{
		case 0:
			act( "$n probuje siê przedstawiæ, chmura b±belków powietrza wydobywa siê mu z ust.", ch, NULL, NULL, TO_ROOM );
			break;
		case 1:
			act( "$n probuje siê przedstawiæ, chmura b±belków powietrza wydobywa siê mu z ust.", ch, NULL, NULL, TO_ROOM );
			break;
		default:
			act( "$n probuje siê przedstawiæ, chmura b±belków powietrza wydobywa siê jej z ust.", ch, NULL, NULL, TO_ROOM );
			break;
		}
		return ;
	}

	if ( !str_cmp( argument, "all" ) )
	{
		switch ( ch->sex )
		{
		case SEX_MALE:
			act( "$n k³ania siê lekko i przedstawia siê wszystkim.", ch, NULL, NULL, TO_ROOM );
			act( "K³aniasz siê lekko i przedstawiasz siê wszystkim.", ch, NULL, NULL, TO_CHAR );
			break;
		case SEX_FEMALE:
			act( "$n dyga przepiêknie i przedstawia siê wszystkim.", ch, NULL, NULL, TO_ROOM );
			act( "Dygasz przepiêknie i przedstawiasz siê wszystkim.", ch, NULL, NULL, TO_CHAR );
			break;
		default:
			act( "$n k³ania siê lekko i przedstawia siê wszystkim.", ch, NULL, NULL, TO_ROOM );
			act( "K³aniasz siê lekko i przedstawiasz siê wszytstkim.", ch, NULL, NULL, TO_CHAR );
			break;
		}
		ch->pcdata->introduced = strdup( "#all" );
		return;
	}

	if ( ( victim = get_char_room( ch, argument ) ) == NULL )
	{
		print_char(ch, "Nie ma tutaj takiej osoby jak %s.\n\r", argument );
		act("$n próbuje siê komu¶ przedstawiæ, nie bardzo jednak wiadomo komu.", ch, NULL, NULL, TO_ROOM );
		return;
	}
	if ( IS_NPC( victim ) )
	{
		act( "$Z nie interesuje znajomo¶æ z Tob±.", ch, NULL, victim, TO_CHAR );
		return;
	}
	if ( !str_cmp( ch->name, victim->name ) )
	{
		send_to_char( "Przedstawiasz siê piêknie sobie.\n\r", ch );
		return;
	}
	switch ( ch->sex )
	{
	case SEX_MALE:
		act( "$n k³ania siê lekko i przedstawia siê $X.", ch, NULL, victim, TO_NOTVICT );
		act( "K³aniasz siê lekko i przedstawiasz siê $X.", ch, NULL, victim, TO_CHAR );
		act( "$n k³ania ci siê lekko i przedstawia siê.", ch, NULL, victim, TO_VICT );
		break;
	case SEX_FEMALE:
		act( "$n dyga przepiêknie i przedstawia siê $X.", ch, NULL, victim, TO_NOTVICT );
		act( "Dygasz przepiêknie i przedstawiasz siê $X.", ch, NULL, victim, TO_CHAR );
		act( "$n dyga przed Tob± przepiêknie i przedstawia siê.", ch, NULL, victim, TO_VICT );
		break;
	default:
		act( "$n k³ania siê lekko i przedstawia siê $X.", ch, NULL, victim, TO_NOTVICT );
		act( "K³aniasz siê lekko i przedstawiasz siê $X.", ch, NULL, victim, TO_CHAR );
		act( "$n k³ania ci siê lekko i przedstawia siê.", ch, NULL, victim, TO_VICT );
		break;
	}
		ch->pcdata->introduced = strdup( victim->name );
		return;
}

void friend_remember( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *znajomy;
	time_t czas;
	bool introduced;


	if ( ( znajomy = get_char_room( ch, argument ) ) == NULL
		|| IS_NPC( znajomy ) )
	{
		print_char(ch, "Próbujesz zapamiêtaæ rysy twarzy %s jednak nie udaje ci siê to.\n\r", argument );
		act("$n próbuje zapamiêtaæ kogo¶ lecz rozgl±da siê tylko nieco skonsternowany.", ch, NULL, NULL, TO_ROOM );
		return;
	}

	if ( !can_see( ch, znajomy ) )
	{
		print_char(ch, "Próbujesz zapamiêtaæ rysy twarzy %s jednak nie udaje ci siê to.\n\r", argument );
		act("$n próbuje zapamiêtaæ kogo¶ lecz rozgl±da siê tylko nieco skonsternowany.", ch, NULL, NULL, TO_ROOM );
		return;		
	}
	
	if ( !str_cmp( ch->name, znajomy->name ) )
	{
		send_to_char( "Znasz siebie ju¿ wystarczaj±co dobrze.\n\r", ch );
		return;
	}

	if ( friend_exist( ch, znajomy->name, NULL, NULL, &czas, &introduced ) )
	{
		if ( ( ( ( current_time - czas ) / 3600 ) > ( ( TIME_TO_FORGET_FRIEND * 24 ) * 2 ) / 3 ) || !introduced )
		{
			friend_usun( ch, znajomy->name );
		} else {
			act( "Ju¿ dobrze znasz $Z.", ch, NULL, znajomy, TO_CHAR );
			return;
		}
	}

	if ( !str_cmp( znajomy->pcdata->introduced, ch->name )
		|| !str_cmp( znajomy->pcdata->introduced, "#all" ) )
	{

		friend_dodaj( ch, znajomy->name, current_time, TRUE );

		if ( str_cmp( znajomy->pcdata->introduced, "#all" ) )
		{
			free( znajomy->pcdata->introduced );
			znajomy->pcdata->introduced = NULL;
		}

		switch ( ch->sex )
		{
		case SEX_MALE:
			act( "Zapamiêta³e¶ dobrze $C.", ch, NULL, znajomy, TO_CHAR );
			break;
		case SEX_FEMALE:
			act( "Zapamiêta³a¶ dobrze $C.", ch, NULL, znajomy, TO_CHAR );
			break;
		default:
			act( "Zapamiêta³e¶ dobrze $C.", ch, NULL, znajomy, TO_CHAR );
			break;
		}
		act( "$n u¶miecha siê do ciebie jak stary znajomy.", ch, NULL, znajomy, TO_VICT );
		act( "$n u¶miecha siê do $Z jak stary znajomy.", ch, NULL, znajomy, TO_NOTVICT );
	} else {
		friend_dodaj( ch, znajomy->name, current_time, FALSE );
		switch ( ch->sex )
		{
		case SEX_MALE:
			act( "Dobrze sobie zapamiêta³e¶ $C.", ch, NULL, znajomy, TO_CHAR );
			break;
		case SEX_FEMALE:
			act( "Dobrze sobie zapamiêta³a¶ $C.", ch, NULL, znajomy, TO_CHAR );
			break;
		default:
			act( "Dobrze sobie zapamiêta³o¶ $C.", ch, NULL, znajomy, TO_CHAR );
			break;
		}		
	}
	return;
}

void friend_forget( CHAR_DATA *ch, char *argument )
{
	if ( !ch || IS_NPC( ch ) ) return;
	if ( friend_usun( ch, argument ) )
	{
		print_char( ch, "Nie chcesz znaæ takiej osoby jak %s.\n\r", argument );
	} else {
		send_to_char( "Próbujesz zapomnieæ o kim¶, kogo nie pamiêtasz?\n\r", ch );
	}
	return;
}

void friend_list( CHAR_DATA *ch, char *argument )
{
	FRIEND_LIST *wsk;
	char buf[ MAX_STRING_LENGTH ];
	BUFFER *output;
	output = new_buf();
	char *name;
	char *opis;
	time_t czas;
	bool introduced;
	
    if ( !ch || IS_NPC( ch ) ) return;

	name = strdup( " " );
	opis = strdup( "#!#" );

	if ( !ch ) return;

	if ( argument[0] != '\0' )
	{
		if ( friend_exist( ch, argument, name, opis, &czas, &introduced ) )
		{
			print_char( ch, "%s, to imiê brzmi jako¶ znajomo.\n\r", name );
			if ( strcmp( opis, "#!#" ) )
			{
				print_char( ch, "Z tym imieniem kojarzy ci siê: %s.\n\r", opis );
			}
			//current_time - czas ) / 360 => czas znajomosci w godzinach
			// TIME_TO_FORGET_FRIEND * 24 => czas do zapomnienia w godzinach bo TIME_TO... jest w dniach
			if ( ( ( current_time - czas ) / 3600 ) > ( ( TIME_TO_FORGET_FRIEND * 24 ) * 2 ) / 3 )
			{
				print_char( ch, "Zaczynasz powoli zapominaæ t± osobê.\n\r");
			}
		} else {
			print_char( ch, "Nie mo¿esz sobie przypomnieæ %s.\n\r", argument );
		}
		return;
	}

	send_to_char( "\n\r{gOsoby, które znasz\n\r--------------------------\n\r{x", ch );
	for( wsk = ch->pcdata->friends; wsk; wsk = wsk->next )
	{
		//current_time - czas ) / 360 => czas znajomosci w godzinach
		// TIME_TO_FORGET_FRIEND * 24 => czas do zapomnienia w godzinach bo TIME_TO... jest w dniach
		if ( ( ( current_time - wsk->czas ) / 3600 ) > ( ( TIME_TO_FORGET_FRIEND * 24 ) * 2 ) / 3)
		{
			sprintf( buf, "{s*{x" );
			add_buf( output, buf );
		}
		sprintf( buf, "{g%s{x", wsk->name );
		add_buf( output, buf );
		sprintf( buf, "	%c 		 	", wsk->introduced? 'v':'x' );
		add_buf( output, buf );
		sprintf( buf, "{G%s{x\n\r", wsk->opis );
		add_buf( output, buf );
	}
	page_to_char( buf_string( output ), ch );
	send_to_char( "{g--------------------------\n\r{x", ch );
	free_buf( output );
	return;
}

void friend_clear( CHAR_DATA *ch )
{
    if ( !ch || IS_NPC( ch ) ) return;
	friend_czysc( ch );
	switch ( ch->sex )
	{
	case SEX_MALE:
		send_to_char( "Zapomnia³e¶ o wszystkich, czujesz siê taki samotny.\n\r", ch );
		break;
	case SEX_FEMALE:
		send_to_char( "Zapomnia³a¶ o wszystkich, czujesz siê taka samotna.\n\r", ch );
		break;
	default:
		send_to_char( "Zapomnia³e¶ o wszystkich, czujesz siê taki samotny.\n\r", ch );
		break;
	}
	return;
}

void friend_note( CHAR_DATA *ch, char *o_kim, char *co, bool quiet)
{
	FRIEND_LIST *wsk;

	if ( !ch || IS_NPC( ch ) ) return;
	for( wsk = ch->pcdata->friends; wsk; wsk = wsk->next )
	{
		if ( !str_prefix( o_kim, wsk->name ) ) break;
	}
	if ( wsk )
	{
		if ( !wsk->opis ) free( wsk->opis );
		wsk->opis = NULL;
		wsk->opis = strdup( co );
		if ( !quiet )
		{
			switch ( ch->sex )
			{
			case SEX_MALE:
				print_char( ch, "Zapamiêta³e¶ informacje o osobie %s.\n\r", wsk->name );
				break;
			case SEX_FEMALE:
				print_char( ch, "Zapamiêta³a¶ informacje o osobie %s.\n\r", wsk->name );
				break;
			default:
				print_char( ch, "Zapamiêta³e¶ informacje o osobie %s.\n\r", wsk->name );
				break;
			}
			return;
		}
	}
	if ( !quiet ) print_char( ch, "Nie masz znajomego o imieniu %s.\n\r", o_kim );
	return;
}

void do_friend( CHAR_DATA *ch, char *argument )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char *notka;
    if ( !ch || IS_NPC( ch ) ) return;

	/*
		possible commands
		friend ktos
		friend list [ktos]
		friend clear all
		friend forget ktos
		friend remember ktos
		friend introduce ktos
		friend note ktos nota
	*/

  argument = one_argument( argument, arg1 );
	notka = one_argument( argument, arg2 );
	if ( notka == NULL )

	if ( arg1[ 0 ] == '\0' )
	{
		friend_list( ch, arg1 );
		return;
	}
	if ( !str_prefix( arg1, "list" ) )
	{
		friend_list( ch, arg2 );
		return;
	}
	if ( !str_prefix( arg1, "clear" ) )
	{
		if ( !str_cmp( "all", arg2 ) )
		{
			friend_clear( ch );
		}
		else {
			send_to_char( "Je¶li chcesz zapomnieæ wszystkich znajomych u¿yj 'CLEAR ALL'.\n\r", ch );
		}
		return;
	}
	if ( !str_prefix( arg1, "introduce" ) )
	{

		if ( ch->position < POS_STANDING )
		{
			send_to_char( "By³oby nieuprzejmym przedstawiaæ siê w takiej pozycji.\n\r", ch );
			return;
		}
		friend_introduce( ch, arg2 );
		return;
	}
	if ( !str_prefix( arg1, "remember" ) )
	{
		if ( arg2[0] != '\0')
		{
			if ( ch->position != POS_STANDING && ch->position != POS_RESTING )
			{
				send_to_char( "Co¶ nie mo¿esz siê skupiæ.\n\r", ch );
			}
			friend_remember( ch, arg2 );
		}
		else {
			send_to_char( "Kogo chcia³by¶ zapamiêtaæ?\n\r", ch );
		}
		return;
	}
	if ( !str_prefix( arg1, "forget" ) )
	{
		if ( arg2[0] != '\0')
		{
			friend_forget( ch, arg2 );
		}
		else {
			send_to_char( "O kim wola³by¶ zapomnieæ?\n\r", ch );
		}
		return;
	}
	if ( !str_prefix( arg1, "note" ) )
	{
		if ( arg2[0] != '\0')
		{
			friend_note( ch, arg2, notka, FALSE );
		}
		else {
			send_to_char( "O kim chcesz zapamiêtaæ?\n\r", ch );
		}
		return;
	}
	friend_list( ch, arg1 );
	return;
}
