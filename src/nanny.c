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
 * $Id: nanny.c 11146 2012-03-23 13:57:00Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/nanny.c $
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
#include "merc.h"
#include "recycle.h"
#include "nanny.h"
#include "name_check.h"
#include "interp.h"
#include "tables.h"
#include "lang.h"

extern const char echo_off_str [];
extern const char echo_on_str [];
extern const char go_ahead_str [];

extern bool wizlock;

//TODO: co¶ zrobiæ z level 1 (s± zapisywane na dysk), a mo¿e nic nie robiæ - jak siê zrobi aktywacjê postaci to nie bêdzie ich spamu, ew. daæ wywalanie postaci 1poziomych d³ugo nieaktywnych

/*
 * Parse a name for acceptability.
 */
bool check_parse_name( char *name )
{
	/*
	 * Reserved words.
	 */
	if ( is_exact_name( name,
	                    "all auto immortal self someone something the you loner none" ) )
	{
		return FALSE;
	}

	/*
	 * Length restrictions.
	 */

	if ( strlen( name ) < 2 )
		return FALSE;

#if defined(MSDOS)
	if ( strlen( name ) > 8 )
		return FALSE;
#endif

#if defined(macintosh) || defined(unix)
	if ( strlen( name ) > 12 )
		return FALSE;
#endif

	/*
	 * Alphanumerics only.
	 * Lock out IllIll twits.
	 */
	{
		char *pc;
		bool fIll, adjcaps = FALSE, cleancaps = FALSE;
		int total_caps = 0;

		fIll = TRUE;
		for ( pc = name; *pc != '\0'; pc++ )
		{
			if ( !isalpha( *pc ) )
				return FALSE;

			if ( isupper( *pc ) )   /* ugly anti-caps hack */
			{
				if ( adjcaps )
					cleancaps = TRUE;
				total_caps++;
				adjcaps = TRUE;
			}
			else
				adjcaps = FALSE;

			if ( NOPOL( *pc ) != 'I' && NOPOL( *pc ) != 'L' )
				fIll = FALSE;
		}

		if ( fIll )
			return FALSE;

		if ( cleancaps || ( total_caps > ( strlen( name ) ) / 2 && strlen( name ) < 3 ) )
			return FALSE;
	}

	/*
	 * Prevent players from naming themselves after mobs.
	 */
	{
		extern MOB_INDEX_DATA *mob_index_hash[ MAX_KEY_HASH ];
		MOB_INDEX_DATA *pMobIndex;
		int iHash;

		for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
		{
			for ( pMobIndex = mob_index_hash[ iHash ];
			      pMobIndex != NULL;
			      pMobIndex = pMobIndex->next )
			{
				if ( is_name( name, pMobIndex->player_name ) )
					return FALSE;
			}
		}
	}
	return TRUE;
}

bool check_host_broken_link( DESCRIPTOR_DATA *d )
{
	CHAR_DATA * ch, *new_ch;

	new_ch = d->character ? d->character : d->remote_char;

	if ( !new_ch )
		return FALSE;

	for ( ch = char_list; ch != NULL; ch = ch->next )
	{
		if ( !IS_NPC( ch ) && !ch->desc && ch != new_ch
		     && str_cmp( ch->name, new_ch->name ) && !str_cmp( ch->host, d->host ) )
			return TRUE;
	}
	return FALSE;
}
/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
	CHAR_DATA * ch;

	for ( ch = char_list; ch != NULL; ch = ch->next )
	{
		if ( !IS_NPC( ch )
		     && ( !fConn || ch->desc == NULL )
		     && !str_cmp( name, ch->name ) )
		{
			if ( fConn )
			{
				if ( d->remote_char )
				{
					free_char( d->remote_char );
					d->remote_char = NULL;
				}

				if ( d->character )
					free_char( d->character );

				d->character = ch;
				ch->desc	= d;
				ch->timer	= 0;
				send_to_char( "Wznowienie po³±czenia. Wpisz replay ¿eby sprawdziæ wiadomo¶ci.\n\r", ch );
				add_host( ch, d->host, TRUE );
				if ( !IS_NULLSTR( ch->pcdata->last_host ) )
					print_char( ch, "Poprzednie logowanie: %s\n\r", ch->pcdata->last_host );
				free_string( ch->pcdata->last_host );
				ch->pcdata->last_host    = str_dup( d->host );
				sprintf( log_buf, "%s@%s has connected.", ch->name, d->host );
				log_string( log_buf );
				wiznet( "$N wznawia po³±czenie.", ch, NULL, WIZ_LINKS, 0, 0 );
				d->connected = CON_PLAYING;
			}
			else
			{
				if ( d->remote_char )
				{
					free_string( d->remote_char->pcdata->pwd );
					d->remote_char->pcdata->pwd = str_dup( ch->pcdata->pwd );
				}
				else if ( d->character )
				{
					free_string( d->character->pcdata->pwd );
					d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
				}
			}

			return TRUE;
		}
	}

	return FALSE;
}


/*
 * Check if already playing.
 */
#ifdef NEW_NANNY

bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
	CHAR_DATA * vch;
	DESCRIPTOR_DATA * dold;

	for ( vch = char_list; vch; vch = vch->next )
	{
		if ( !IS_NPC( vch ) && !str_cmp( vch->name, name ) && vch->desc != d )
			return TRUE;
    }

	for ( dold = descriptor_list; dold; dold = dold->next )
	{
		if ( dold != d && dold->remote_char && !str_cmp( dold->remote_char->name, name ) )
			return TRUE;
    }

	return FALSE;
}

#else

bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
	DESCRIPTOR_DATA * dold;

	for ( dold = descriptor_list; dold; dold = dold->next )
	{
		if ( dold != d
		     && dold->character != NULL
		     && dold->connected != CON_GET_NAME
		     && dold->connected != CON_GET_OLD_PASSWORD
		     && dold->connected >= 0
		     && !str_cmp( name, dold->original
		                  ? dold->original->name : dold->character->name ) )
		{
			write_to_buffer( d, "Ta postaæ jest ju¿ w grze.\n\r", 0 );
			write_to_buffer( d, "Chcesz siê po³±czyæ mimo to? (T/N)", 0 );
			d->connected = CON_BREAK_CONNECT;
			return TRUE;
		}
	}

	return FALSE;
}

#endif

#ifdef NEW_NANNY

bool check_creating( DESCRIPTOR_DATA *d, char *name )
{
	DESCRIPTOR_DATA * dold;

	for ( dold = descriptor_list; dold; dold = dold->next )
	{
		if ( dold != d && dold->new_char_data != NULL && !str_cmp( dold->new_char_data->name, name ) )
			return TRUE;
    }

	return FALSE;
}

#else

bool check_creating( DESCRIPTOR_DATA *d, char *name )
{
	DESCRIPTOR_DATA * dold;

	for ( dold = descriptor_list; dold; dold = dold->next )
	{
		if ( dold != d
		     && dold->character != NULL
		     && dold->connected != CON_GET_NAME
		     && dold->connected != CON_GET_OLD_PASSWORD
		     && dold->connected > 0
		     && ( !str_cmp( name, dold->original ? dold->original->name : dold->character->name )
		          || !str_cmp( name, dold->original ? dold->original->old_name : dold->character->old_name ) ) )
		{

			switch ( dold->connected )
			{
				case CON_CONFIRM_NEW_NAME :
				case CON_NDENY_RSV_PASS :
				case CON_NDENY_GET_NAME :
				case CON_NDENY_GET_ODMIANA :
				case CON_NDENY_ACCEPT_ODMIANA :
				case CON_NDENY_CONFIRM_NAME :
				case CON_GET_NEW_PASSWORD :
				case CON_CONFIRM_NEW_PASSWORD :
				case CON_GET_NEW_RACE :
				case CON_GET_NEW_SEX :
				case CON_GET_NEW_CLASS :
				case CON_GET_ALIGNMENT :
				case CON_GET_ODMIANA :
				case CON_ACCEPT_ODMIANA :
					write_to_buffer( d, "Ta postaæ jest w³a¶nie tworzona. Wybierz inne imiê:", 0 );
					d->connected = CON_GET_NAME;
					return TRUE;
				default: return FALSE;
			}

		}
	}
	return FALSE;
}

#endif

void generate_password( char * result, int length )
{
    const char * chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890#%&*";
    int i;

    for( i = 0; i < length; i++ )
        result[i] = chars[number_range( 0, 65 )];
    result[length] = '\0';
}

bool check_password( DESCRIPTOR_DATA *d, char *indent, char * password )
{
    /**
     * Olewamy wymuszanie dzikiego has³a.
     * bo to bez sensu.
     * 2008-05-14 Gurthg
     */
    return TRUE;
    int i, length;
    int letters = 0;
    int digits = 0;
    int rest = 0;
    bool tilde = FALSE;
    bool result = TRUE;

    length = strlen( password );

    for( i = 0; i < length; i++ )
    {
        if ( isdigit( password[i] ) )
            digits++;
        else if ( isalpha( password[i] ) )
            letters++;
        else
            rest++;
        if ( password[i] == '~' )
            tilde = TRUE;
    }

    if ( length < 6 )
    {
        if ( d )
        {
            send_to_desc( d, indent, TRUE );
            send_to_desc( d, "Haslo musi miec przynajmniej 6 znakow.{x\n\r", TRUE );
        }

        result = FALSE;
    }

    if ( digits < 2 )
    {
        if ( d )
        {
            send_to_desc( d, indent, TRUE );
            send_to_desc( d, "Haslo musi zawierac przynajmniej 2 cyfry.{x\n\r", TRUE );
        }
        result = FALSE;
    }

    if ( rest < 1 )
    {
        if ( d )
        {
            send_to_desc( d, indent, TRUE );
            send_to_desc( d, "Haslo musi zawierac przynajmniej 1 znak inny niz litery i cyfry.{x\n\r", TRUE );
        }
        result = FALSE;
    }

    if ( tilde )
    {
        if ( d )
        {
            send_to_desc( d, indent, TRUE );
            send_to_desc( d, "Haslo nie moze zawierac znaku tyldy '~'.\n\r", TRUE );
        }
        result = FALSE;
    }

    return result;
}

#ifdef NEW_NANNY

const struct nanny_cmd_type nanny_table[] =
{
	{	NANNY_EXIT,						nanny_exit						},
	{	NANNY_FIRST_INPUT,				nanny_first_input				},
	{	NANNY_NEW_ACCOUNT,				nanny_new_account				},
	{	NANNY_NEWACC_PASSWD,			nanny_newacc_passwd				},
	{	NANNY_NEWACC_PASSWD_CONFIRM,	nanny_newacc_passwd_confirm		},
	{	NANNY_CHECK_PLAYER_PASSWD,		nanny_check_player_passwd		},
	{	NANNY_CHECK_ACCOUNT_PASSWD,		nanny_check_account_passwd		},
	{	NANNY_ACCOUNT_MENU,				nanny_account_menu				},
	{	NANNY_NEWCHAR_MENU,				nanny_newchar_menu				},
	{	NANNY_CHOOSE_PLAYER,			nanny_choose_player				},
	{	NANNY_PLAYER_SALVAGE,			nanny_player_salvage			},
	{	NANNY_CHECK_SALVAGE_PASSWD,		nanny_check_salvage_passwd		},
	{	NANNY_CHACC_PASSWD_OLD,			nanny_chacc_passwd_old			},
	{	NANNY_CHACC_PASSWD_NEW,			nanny_chacc_passwd_new			},
	{	NANNY_CHACC_PASSWD_CONFIRM,		nanny_chacc_passwd_confirm		},
	{	NANNY_DELETE_PLAYER,			nanny_delete_player				},
	{	NANNY_CONFIRM_DELETE_PLAYER,	nanny_confirm_delete_player		},
	{	NANNY_DELETE_ACCOUNT,			nanny_delete_account			},
	{	NANNY_CHEMAIL_PASSWD_CONFIRM,	nanny_chemail_passwd_confirm	},
	{	NANNY_CHANGE_EMAIL,				nanny_change_email				},
	{	NANNY_ACTIVATE_ACCOUNT,			nanny_activate_account			},
	{	NANNY_NEWCHAR_NAME,				nanny_newchar_name				},
	{	NANNY_NEWCHAR_RESERVED_PWD,		nanny_newchar_reserved_pwd		},
	{	NANNY_NEWCHAR_INFLECT,			nanny_newchar_inflect			},
	{	NANNY_NEWCHAR_CONFIRM_INFLECT,	nanny_newchar_confirm_inflect	},
	{	NANNY_NEWCHAR_SEX,				nanny_newchar_sex				},
	{	NANNY_NEWCHAR_RACE,				nanny_newchar_race				},
	{	NANNY_NEWCHAR_CONFIRM_RACE,		nanny_newchar_confirm_race		},
	{	NANNY_NEWCHAR_CLASS,			nanny_newchar_class				},
	{	NANNY_NEWCHAR_CONFIRM_CLASS,	nanny_newchar_confirm_class		},
	{	NANNY_NEWCHAR_SPEC,				nanny_newchar_spec				},
	{	NANNY_NEWCHAR_ALIGN,			nanny_newchar_align				},
	{	NANNY_NEWCHAR_PASSWORD,			nanny_newchar_password			},
	{	NANNY_NEWCHAR_PASSWORD_CONFIRM,	nanny_newchar_password_confirm	},
	{	NANNY_NEWCHAR_ACCEPT,			nanny_newchar_accept			},
	{	NANNY_NEWCHAR_RESIGN,			nanny_newchar_resign			},
	{	NANNY_PLAYER_MENU,				nanny_player_menu				},
	{	NANNY_PLAYER_CHANGE_NAME,		nanny_player_change_name		},
	{	NANNY_PLAYER_RESERVED_PWD,		nanny_player_reserved_pwd		},
	{	NANNY_PLAYER_INFLECT,			nanny_player_inflect			},
	{	NANNY_PLAYER_CONFIRM_INFLECT,	nanny_player_confirm_inflect	},
	{	0,								0								}
};

void disconnect_account_players( DESCRIPTOR_DATA *d, bool verbose )
{
	CHAR_DATA * vch;
	DESCRIPTOR_DATA *dclose;
	char buf[ MAX_STRING_LENGTH ];
	int count = 0;
	int dcount = 0;

	if ( !d || !d->account || IS_NULLSTR( d->account->email ) )
		return ;

	buf[0] = '\0';

	for ( vch = char_list; vch; vch = vch->next )
	{
		if ( d->character == vch || IS_NPC( vch ) || str_cmp( vch->pcdata->account_email, d->account->email ) )
			continue;

		if ( count > 0 )
			strcat( buf, ", " );
		strcat( buf, vch->name );
		save_char_obj( vch, FALSE, FALSE );
		extract_char( vch, TRUE );
		count++;
	}

	for ( dclose = descriptor_list; dclose; dclose = dclose->next )
	{
		if ( dclose == d || !dclose->account || str_cmp( dclose->account->email, d->account->email ) )
			continue;

		if ( dclose->character )
		{
			if ( count > 0 )
				strcat( buf, ", " );
			strcat( buf, dclose->character->name );
			save_char_obj( dclose->character, FALSE, FALSE );
			extract_char( dclose->character, TRUE );
			count++;
		}
		else if ( dclose->remote_char )
		{
			if ( count > 0 )
				strcat( buf, ", " );
			strcat( buf, dclose->remote_char->name );
			free_char( dclose->remote_char );
			dclose->remote_char = NULL;
			count++;
		}
		else if ( dclose->new_char_data )
		{
			free_new_char_data( dclose->new_char_data );
			dclose->new_char_data = NULL;
		}

		dcount++;
		close_socket( dclose, 0 );
	}

	if ( verbose )
	{
		if ( count > 0 || dcount > 0 )
		{
			if ( dcount > 0 )
				print_desc( d, TRUE, "\n\r%s  Roz³±czonych po³±czeñ z tego konta: {C%d{x.\n\r", d->spaces, dcount );
			if ( count > 0 )
				print_desc( d, TRUE, "\n\r%s  W tym wylogowano nastêpuj±ce postaci:\n\r%s  %s\n\r", d->spaces, d->spaces, buf );
		}
		else
			print_desc( d, TRUE, "\n\r%s  {RNie ma aktualnie innych po³±czeñ z tego konta.{x\n\r", d->spaces );
	}
	return ;
}

int check_account_players( DESCRIPTOR_DATA *d )
{
	CHAR_DATA * vch;
	int count = 0;

	if ( !d || !d->account || IS_NULLSTR( d->account->email ) )
		return 0;

	for ( vch = char_list; vch; vch = vch->next )
	{
		if ( d->character == vch || IS_NPC( vch ) || str_cmp( vch->pcdata->account_email, d->account->email ) )
			continue;

		count++;
	}

	return count;
}

bool new_char_data_to_char( NEW_CHAR_DATA * new_ch, CHAR_DATA * ch )
{
	if ( !new_ch || !ch || !ch->pcdata)
		return FALSE;

	free_string( ch->name );
	ch->name = str_dup( new_ch->name );

	free_string( ch->name2 );
	ch->name2 = str_dup( new_ch->name2 );

	free_string( ch->name3 );
	ch->name3 = str_dup( new_ch->name3 );

	free_string( ch->name4 );
	ch->name4 = str_dup( new_ch->name4 );

	free_string( ch->name5 );
	ch->name5 = str_dup( new_ch->name5 );

	free_string( ch->name6 );
	ch->name6 = str_dup( new_ch->name6 );

	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd = str_dup( new_ch->password);

	ch->sex = new_ch->sex;
	ch->pcdata->true_sex = new_ch->sex;
	ch->class = new_ch->class;
	ch->pcdata->mage_specialist = new_ch->mage_specialist;
	SET_RACE( ch, new_ch->race );
	ch->alignment = new_ch->alignment;

	return TRUE;
}

/*
 * Tworzy now± postaæ na podstawie informacji w nanny_char w deskryptorze
 */
int create_char( DESCRIPTOR_DATA * d, bool verbose )
{
    CHAR_DATA * ch;
    int i;
    int hp_mod, v_mod;

    if ( !d || !d->new_char_data || IS_NULLSTR( d->new_char_data->name ) )
        return NANNY_EXIT;

    if ( check_creating( d, d->new_char_data->name ) )
    {
        if ( verbose )
        {
            print_desc( d, TRUE, "\n\r%s  {RB³±d tworzenia postaci. Kto¶ inny tworzy postaæ o takim imieniu.\n\r{x", d->spaces );
        }
        free_string( d->new_char_data->name );
        SET_BIT( d->new_char_data->creating, CREATE_NAME );
        return NANNY_NEWCHAR_MENU;
    }

    if ( player_exists( d->new_char_data->name ) )
    {
        if ( verbose )
        {
            print_desc( d, TRUE, "\n\r%s  {RB³±d tworzenia postaci. Istnieje ju¿ postaæ o takim imieniu.\n\r{x", d->spaces );
        }
        free_string( d->new_char_data->name );
        SET_BIT( d->new_char_data->creating, CREATE_NAME );
        return NANNY_NEWCHAR_MENU;
    }

    ch = init_char( d->new_char_data->name );

    new_char_data_to_char( d->new_char_data, ch );

    //perm statsy
    //standardowa waga i wzrost
    roll_stats(ch);

    //jêzyki
    for ( i = 0; i < MAX_LANG; i++ )
    {
        if ( pc_race_table[ GET_RACE(ch) ].well_known_lang & lang_table[ i ].bit )
        {
            ch->pcdata->language[ i ] = number_range( 80, 100 );
        }

        if ( pc_race_table[ GET_RACE(ch) ].known_lang & lang_table[ i ].bit )
        {
            ch->pcdata->language[ i ] = number_range( 45, 75 );
        }
    }
    ch->speaking = 0;

    //affecty, form, parts, size
    ext_flags_clear( ch->affected_by );
    ext_flags_copy( ext_flags_sum( ch->affected_by, ext_flag_value( affect_flags, race_table[ GET_RACE( ch ) ].aff ) ), ch->affected_by );
    ch->form	= race_table[ GET_RACE( ch ) ].form;
    ch->parts	= race_table[ GET_RACE( ch ) ].parts;
    ch->size = pc_race_table[ GET_RACE( ch ) ].size;

    for ( i = 0; i < MAX_STATS;i++ )
    {
        ch->mod_stat[ i ]     = 0;
        ch->new_mod_stat[ i ] = 0;
    }

    ch->pcdata->perm_hit = ch->max_hit;
    ch->pcdata->perm_move = ch->max_move;
    ch->wimpy = ch->max_hit / 3;

    //nie ma wpisu o imieniu, flaga
    if ( is_allowed( ch->name ) == NAME_NOTEXIST || is_allowed( ch->name ) == NAME_NEW )
    {
        EXT_SET_BIT( ch->act, PLR_UNREGISTERED );
        save_file( NAME_NEW );
    }

    /**
     * ustawiamy skile bazowe
     */
    set_base_skills( ch );
    //TODO: aktywacja postaci

    if ( is_allowed( ch->name ) == NAME_NOTEXIST || is_allowed( ch->name ) == NAME_NEW )
    {
        EXT_SET_BIT( ch->act, PLR_UNREGISTERED );
        save_file( NAME_NEW );
    }

    //zapisujemy now± postaæ
    save_char_obj( ch, TRUE, TRUE );

    add_player_to_account( d->account, ch->name );

    //czy¶cimy po sobie
    free_new_char_data( d->new_char_data );
    d->new_char_data = NULL;

    return 0;
}

void show_header( DESCRIPTOR_DATA *d, char* header, char* plot, char* inside_plot, char colorh, char color1, char color2 )
{
	char buf[ MAX_STRING_LENGTH ];
	char center[ MAX_STRING_LENGTH ];
	char spaces[ MAX_STRING_LENGTH ];
	char plots[ MAX_STRING_LENGTH ];
	int count, i, j, len;

	//stuff do ¶laczków
	len = strlen( inside_plot );
	count = ( 79 - len ) / ( strlen( plot ) * 2 );

	if ( count > 1 )
		count--;

	plots[ 0 ] = '\0';
	if ( count > 0 )
	{
		for ( i = 0; i < count; i++ )
		{
			for ( j = 0; j < strlen( plot ); j++ )
				plots[ i * strlen( plot ) + j ] = plot[ j ];
		}
		plots[ count * strlen( plot ) ] = '\0';
	}
	//koniec stufu do ¶laczków

	if ( header )
	{
		write_to_buffer( d, "\n\r", 0 );
		sprintf( buf, "{%c%s{x", colorh, header );
		center_text( buf, center, 79 );
		send_to_desc( d, center, TRUE );
	}

	sprintf( buf, "{%c%s{%c %s {%c%s{x", color1, plots, color2, inside_plot, color1, plots );
	count = center_text( buf, center, 79 );
	send_to_desc( d, center, TRUE );

	send_to_desc( d, "\n\r", TRUE );

	for ( i = 0; i < count; i++ )
		spaces[ i ] = ' ';
	spaces[ count ] = '\0';

	free_string( d->spaces );
	d->spaces = str_dup( spaces );
	return ;
}

void show_account_menu( DESCRIPTOR_DATA *d )
{
	show_header( d, "MENU KONTA", "=<>=", d->account->email, 'Y', 'g', 'G' );

	print_desc( d, TRUE, "%s  {C%2d.{x Stwórz now± postaæ. %s\n\r", d->spaces, ACC_CREATE_PLAYER, IS_NULLSTR( d->account->activation ) ? "" : "({Rnieaktywne{x)" );
	print_desc( d, TRUE, "%s  {C%2d.{x Wy¶wietl listê postaci.\n\r", d->spaces, ACC_SHOW_CHARACTERS );
	print_desc( d, TRUE, "%s  {C%2d.{x Przejd¼ do menu postaci. %s\n\r", d->spaces, ACC_CHARACTER_MENU, IS_NULLSTR( d->account->activation ) ? "" : "({Rnieaktywne{x)" );
	print_desc( d, TRUE, "%s  {C%2d.{x Podepnij postaæ pod konto.%s\n\r", d->spaces, ACC_SALVAGE_CHARACTER, IS_NULLSTR( d->account->activation ) ? "" : "({Rnieaktywne{x)" );
	print_desc( d, TRUE, "%s  {C%2d.{x Usuñ postaæ. %s\n\r", d->spaces, ACC_DELETE_CHARACTER, IS_NULLSTR( d->account->activation ) ? "" : "({Rnieaktywne{x)" );
	print_desc( d, TRUE, "%s  {C%2d.{x Roz³±cz aktualne po³±czenia z tego konta.\n\r\n\r", d->spaces, ACC_LOGOUT_CHARACTERS );
	print_desc( d, TRUE, "%s  {C%2d.{x Zmieñ has³o do konta. %s\n\r", d->spaces, ACC_CHANGE_ACCOUNT_PASSWD, IS_NULLSTR( d->account->activation ) ? "" : "({Rnieaktywne{x)" );
	print_desc( d, TRUE, "%s  {C%2d.{x Przypomnij has³o na e-mail. %s\n\r", d->spaces, ACC_SEND_ACCOUNT_PASSWD, IS_NULLSTR( d->account->activation ) ? "" : "({Rnieaktywne{x)" );
	print_desc( d, TRUE, "%s  {C%2d.{x Zmieñ adres e-mail konta.\n\r", d->spaces, ACC_CHANGE_ACCOUNT_EMAIL );
	print_desc( d, TRUE, "%s  {C%2d.{x Skasuj konto. %s\n\r", d->spaces, ACC_DELETE_ACCOUNT, IS_NULLSTR( d->account->activation ) ? "" : "({Rnieaktywne{x)" );
	print_desc( d, TRUE, "%s  {C%2d.{x Zg³o¶ b³±d w koncie.\n\r\n\r", d->spaces, ACC_REPORT_ERROR );
	if ( !IS_NULLSTR( d->account->activation ) )
	{
		print_desc( d, TRUE, "%s  {C%2d.{x ^f{RAktywuj konto.{x\n\r", d->spaces, ACC_ACTIVATE_ACCOUNT );
		print_desc( d, TRUE, "%s  {C%2d.{x {RWy¶lij kod aktywacyjny na email.{x\n\r\n\r", d->spaces, ACC_SEND_ACTIVATION_CODE );
	}
	print_desc( d, TRUE, "%s  {C%2d.{x Zamknij po³±czenie.\n\r", d->spaces, ACC_LOGOUT );

	print_desc( d, TRUE, "\n\r%s{Y->{xWybierz opcjê:", d->spaces );
	return ;
}

int show_players_list( DESCRIPTOR_DATA *d, bool verbose )
{
	CHAR_DATA * rch;
	char name[ MAX_STRING_LENGTH ];
	char *char_list;
	int count = 0;
	int sh_count = 1;

	char_list = one_argument( d->account->characters, name );

	if ( verbose )
		show_header( d, "", "=<>=", "Lista postaci", 'Y', 'g', 'G' );

	if ( IS_NULLSTR( name ) )
	{
		if ( verbose )
		{
			print_desc( d, TRUE, "%s  {RZ kontem nie s± zwi±zane jeszcze ¿adne postaci.{x\n\r", d->spaces );
		}
	}
	else
	{
		do
		{
			if ( ( rch = load_char_remote( name ) ) != NULL )
			{
				if ( verbose )
				{
					if ( rch->level > 0 )
					{
						print_desc( d, TRUE, "%s  {C%d.{x %-12.12s - {c%s{x profesji {c%s{x na poziomie {c%d{x\n\r",
						         d->spaces, sh_count, rch->name, pc_race_table[ rch->race ].name,
						         get_class_name( rch ), rch->level );
					}
					else
					{
						print_desc( d, TRUE, "%s  {C%d.{x %-12.12s - {c%s{x profesji {c%s{x (nowa postaæ)\n\r",
						         d->spaces, sh_count, rch->name, pc_race_table[ rch->race ].name,
						         get_class_name( rch ) );
					}
				}
				free_char( rch );
				count++;
			}
			else
			{
				if ( verbose )
				{
					print_desc( d, TRUE, "%s  {C%d.{x %-12.12s - postaæ prawdopodobnie {Ruszkodzona{x\n\r",
					         d->spaces, sh_count, capitalize( name ) );
				}
			}
			char_list = one_argument( char_list, name );
			sh_count++;
		}
		while ( !IS_NULLSTR( name ) );
	}

	return count;
}

bool mail_passwd( DESCRIPTOR_DATA *d, int type )
{
	extern time_t str_boot_time;
	char msg[ MAX_STRING_LENGTH ];
	char date[ MAX_STRING_LENGTH ];
	char year[ 10 ];
	char hour[ 20 ];
	struct tm *btime;

	btime = localtime( &str_boot_time );
	strftime( year, 10, "%Y", btime );
	strftime( hour, 20, "%H:%M:%S", btime );
	sprintf( date, "%s %s, %d %s %s o godzinie %s",
	         btime->tm_wday == 2 ? "We" : "W",
	         real_day_names[ btime->tm_wday ][ 1 ],
	         btime->tm_mday,
	         real_month_names[ btime->tm_mon ][ 1 ],
	         year,
	         hour );

	if ( !d || !d->account || IS_NULLSTR( d->account->email ) )
		return FALSE;

/*	if ( menu )
		sprintf( menu_msg,
		         "Je¶li konto nale¿y do Ciebie, ale to nie Ty zg³asza³e¶(a¶) pro¶bê o wys³anie\n"
		         "has³a, prawdopodobnie kto¶ w³ama³ siê na Twoje konto i wybra³ t± opcjê\n"
		         "lub skorzysta³ z Twojej nieobecno¶ci przy komputerze w momencie kiedy by³e¶(a¶)\n"
		         "zalogowany(a) na koncie.\n" );
	else
		sprintf( menu_msg,
		         "Je¶li konto nale¿y do Ciebie, ale to nie Ty zglasza³e¶(a¶) pro¶bê o wys³anie\n"
		         "has³a, prawdopodobnie kto¶ wpisa³ 2 razy nieprawid³owo has³o przy logowaniu\n"
		         "i wyrazi³ zgodê na wys³anie has³a na Twój adres email.\n" );

	sprintf( msg, "%s zg³oszono pro¶bê o wys³anie has³a\n"
	         "do konta %s. Osoba zg³aszaj±ca ³±czy³a siê z adresu '%s'.\n\n"
	         "Has³o do konta %s jest nastêpuj±ce: %s\n\n"
	         "Je¶li nie wiesz o co chodzi w tym mailu, skasuj go i zignoruj.\n"
	         "%s"
	         "Podane wy¿ej czas i adres pomog± zidentyfikowaæ potencjalnego intruza.\n\n"
	         "¯yczymy wielu mile spêdzonych godzin w ¶wiecie Killera,\n"
	         "Za³oga muda Killer - %s\n"
	         "Strona WWW - %s\n"
	         "Adres muda - %s\n",
	         date, d->account->email, d->host, d->account->email, d->account->passwd, menu_msg,
	         MAIL_ADDRESS, WWW_ADDRESS, TELNET_ADDRESS );*/

	return send_mail_message( d->account->email, "[Killer Mud] Has³o do konta", msg );
}

bool mail_account_activation_code( DESCRIPTOR_DATA *d, int type, char *argument )
{
	extern time_t str_boot_time;
	char msg[ MAX_STRING_LENGTH ];
	char date[ MAX_STRING_LENGTH ];
	char year[ 10 ];
	char hour[ 20 ];
	struct tm *btime;

	btime = localtime( &str_boot_time );
	strftime( year, 10, "%Y", btime );
	strftime( hour, 20, "%H:%M:%S", btime );
	sprintf( date, "%s, %d %s %s, godzina %s",
	         real_day_names[ btime->tm_wday ][ 1 ],
	         btime->tm_mday,
	         real_month_names[ btime->tm_mon ][ 1 ],
	         year,
	         hour );

	if ( !d || !d->account || IS_NULLSTR( d->account->email ) )
		return FALSE;

	if ( type == MAIL_CHANGE_ACCOUNT )
	{
		sprintf( msg, 	"Wiadomo¶æ ta zosta³a automatycznie wygenerowana przez serwer muda Killer\n"
						"w zwi±zku ze zmian± adresu mail twojego konta.\n\n"
						"Stary adres email: %s\n"
						"Nowy adres email: %s\n"
						"Data operacji: %s\n"
						"Adres komputera wywo³uj±cego opcjê: %s\n"
						"Kod aktywacyjny: %s\n\n"
						"Po zalogowaniu siê na konto korzystaj±c z nowego adresu nale¿y uaktywniæ je\n"
						"wprowadzaj±c powy¿szy kod aktywacyjny (opcja '%d' w menu konta).\n\n"
						"Je¶li nie wiesz o co chodzi w tym mailu, skasuj go i zignoruj.\n"
						"W przeciwnym wypadku ¿yczymy wielu mile spêdzonych godzin w ¶wiecie Killera,\n\n"
						"-- "
						"Za³oga muda Killer - %s\n"
						"Strona WWW - %s\n"
						"Adres muda - %s\n",
						d->account->email, argument, date, d->host, d->account->activation, ACC_ACTIVATE_ACCOUNT,
						MAIL_ADDRESS, WWW_ADDRESS, TELNET_ADDRESS );

		return send_mail_message( argument, "[Killer Mud] Zmiana adresu email - kod aktywacyjny", msg );
	}

	if ( type == MAIL_REMIND_CODE )
	{
		sprintf( msg, 	"Wiadomo¶æ ta zosta³a automatycznie wygenerowana przez serwer muda Killer\n"
						"po wybraniu opcji przypomnienia kodu aktywacyjnego z menu konta.\n\n"
						"Konto: %s\n"
						"Data operacji: %s\n"
						"Adres komputera wywo³uj±cego opcjê: %s\n"
						"Kod aktywacyjny: %s\n\n"
						"W celu odblokowania funkcji konta nale¿y po³±czyæ siê z mudem, zalogowaæ siê\n"
						"na konto podaj±c e-mail i has³o wpisane przy jego stworzeniu, a nastêpnie\n"
						"wybraæ opcjê numer %d i wpisaæ powy¿szy kod aktywacyjny. Po aktywacji\n"
						"wszystkie funkcje w menu konta zostan± odblokowane.\n\n"
						"Je¶li nie wiesz o co chodzi w tym mailu, skasuj go i zignoruj.\n"
						"W przeciwnym wypadku ¿yczymy wielu mile spêdzonych godzin w ¶wiecie Killera,\n\n"
						"-- "
						"Za³oga muda Killer - %s\n"
						"Strona WWW - %s\n"
						"Adres muda - %s\n",
						d->account->email, date, d->host, d->account->activation, ACC_ACTIVATE_ACCOUNT,
						MAIL_ADDRESS, WWW_ADDRESS, TELNET_ADDRESS );

		return send_mail_message( d->account->email, "[Killer Mud] Kod aktywacyjny do konta", msg );
	}

	if ( type == MAIL_NEW_ACCOUNT )
	{
		sprintf( msg,   "Witamy w ¶wiecie muda Killer. Wiadomo¶æ ta zosta³a automatycznie wygenerowana\n"
						"przy stworzeniu nowego konta i zawiera kod aktywacyjny, którego trzeba u¿yæ\n"
						"by odblokowaæ wszystkie funkcje zwi±zane z kontem.\n\n"
						"Konto: %s\n"
						"Data stworzenia: %s\n"
						"Adres komputera tworz±cego konto: %s\n"
						"Kod aktywacyjny: %s\n\n"
						"W celu odblokowania funkcji konta nale¿y po³±czyæ siê z mudem, zalogowaæ siê\n"
						"na konto podaj±c e-mail i has³o wpisane przy jego stworzeniu, a nastêpnie\n"
						"wybraæ opcjê numer %d i wpisaæ powy¿szy kod aktywacyjny. Po aktywacji\n"
						"wszystkie funkcje w menu konta zostan± odblokowane. Teraz wystarczy stworzyæ\n"
						"pierwsz± postaæ i zag³êbiæ siê w naszym ¶wiecie.\n\n"
						"UWAGA:\n"
						"W przypadku zagubienia has³a do konta lub postaci nale¿y wybraæ opcjê wys³ania has³a\n"
						"na adres e-mail. Opcja dostêpna jest po trzykrotnym nieprawid³owym podaniu has³a przy\n"
						"logowaniu siê na mudzie. Przypominamy o nie udostêpnianiu swoich hase³ osobom trzecim.\n"
						"Nie rozpatrujemy ¿adnych wniosków o podanie hase³ do kont lub postaci. Has³a mo¿na zmieniæ\n"
						"w ka¿dej chwili z poziomu menu konta lub menu postaci.\n\n"
						"Je¶li nie wiesz o co chodzi w tym mailu, skasuj go i zignoruj.\n"
						"W przeciwnym wypadku ¿yczymy wielu mile spêdzonych godzin w ¶wiecie Killera,\n\n"
						"-- "
						"Za³oga muda Killer - %s\n"
						"Strona WWW - %s\n"
						"Adres muda - %s\n",
						d->account->email, date, d->host, d->account->activation, ACC_ACTIVATE_ACCOUNT,
						MAIL_ADDRESS, WWW_ADDRESS, TELNET_ADDRESS );

		return send_mail_message( d->account->email, "[Killer Mud] Kod aktywacyjny do konta", msg );
	}
	return 0;
}

/* centrum sterowania podfunkcjami nanny (tablica na pocz±tku pliku) */
void new_nanny( DESCRIPTOR_DATA *d, char *argument )
{
	int cmd;
	int result;

	while ( isspace( *argument ) )
		argument++;

	for ( cmd = 0; nanny_table[ cmd ].nanny_fun != 0; cmd++ )
	{
		if ( d->connected == nanny_table[ cmd ].state )
		{
			result = ( ( *nanny_table[ cmd ].nanny_fun ) ( d, argument ) );
			if ( IS_VALID( d ) )
				d->connected = result;
			break;
		}
	}
}

NANNY( nanny_exit )
{
	close_socket( d, 0 );
	return NANNY_EXIT;
}

NANNY( nanny_first_input )
{
	int i;
	bool fOldAcc;

	//scinanie duzych liter
	for ( i = 0; argument[ i ] != '\0'; i++ )
		argument[ i ] = LOWER( argument[ i ] );

	if ( validate_email( argument ) )
	{
		fOldAcc = load_account( &d->account, argument );
		if ( !fOldAcc )
		{
			print_desc(d, TRUE, "Nie istnieje konto o takim adresie e-mail.\n\rCzy chcesz zalozyc nowe konto zwiazane z adresem %s (T/N)? ", argument );
			return NANNY_NEW_ACCOUNT;
		}

		if ( IS_NULLSTR( d->account->email ) )
		{
			print_desc(d, TRUE, "Plik z kontem zostal uszkodzony. Skontaktuj sie z administratorami muda pod\n\r"
								 "adresem: %s\n\r\n\r"
								 "{RNacisnij Enter aby zamknac polaczenie z mudem.{x\n\r", MAIL_ADDRESS );
			return NANNY_EXIT;
		}

		send_to_desc( d, "Podaj haslo: ", TRUE );
		send_to_desc( d, echo_off_str, FALSE );
		d->pass_fail = 0;
		return NANNY_CHECK_ACCOUNT_PASSWD;
	}

	/* zle znaki */
	if ( !check_parse_name( argument ) )
	{
		send_to_desc( d, "Imie zawiera nieprawidlowa kombinacje znakow, sprobuj jeszcze raz.\n\r", TRUE );
		send_to_desc( d, "Podaj imiê swojej postaci lub adres e-mail: ", TRUE );
		return NANNY_FIRST_INPUT;
	}

	/* nie ma takiej postaci */
	if ( player_exists( argument ) == FALSE )
	{
		send_to_desc( d, "Nie ma postaci o takim imieniu. Zanim stworzysz nowa postac musisz zalozyc\n\r", TRUE );
		send_to_desc( d, "konto zwiazane z Twoim adresem e-mail.\n\r", TRUE );
		send_to_desc( d, "Podaj imiê swojej postaci lub adres e-mail: ", TRUE );
		return NANNY_FIRST_INPUT;
	}

	d->remote_char = load_char_remote( argument );

	send_to_desc( d, "Podaj has³o postaci: ", TRUE );
	send_to_desc( d, echo_off_str, FALSE );
	d->pass_fail = 0;

	return NANNY_CHECK_PLAYER_PASSWD;
}

NANNY( nanny_new_account )
{
	if ( !d->account )
		return NANNY_EXIT;

	switch ( argument[0] )
	{
		case 't':
		case 'T':
			send_to_desc( d, "\n\rTworze nowe konto.\n\r", TRUE );
			send_to_desc( d, "Wprowadz nowe haslo do konta: ", TRUE );
			send_to_desc( d, echo_off_str, FALSE );
			return NANNY_NEWACC_PASSWD;
		case 'n':
		case 'N':
			send_to_desc( d, "Podaj imiê swojej postaci lub adres e-mail: ", TRUE );
			return NANNY_FIRST_INPUT;
	}

	print_desc( d, TRUE, "Czy chcesz za³o¿yæ nowe konto zwi±zane z adresem %s (T/N)? ", d->account->email );
	return NANNY_NEW_ACCOUNT;
}

NANNY( nanny_newacc_passwd )
{
	if ( !check_password( d, "", argument ) )
	{
		send_to_desc( d, "\n\rWprowad¼ nowe has³o do konta:", TRUE );
		return NANNY_NEWACC_PASSWD;
	}

	free_string( d->tmp_passwd );
	d->tmp_passwd = str_dup( argument );

	send_to_desc( d, "\n\rPotwierd¼ nowe has³o do konta: ", TRUE );

	return NANNY_NEWACC_PASSWD_CONFIRM;
}

NANNY( nanny_newacc_passwd_confirm )
{
	char buf[ MAX_STRING_LENGTH ];

	if ( str_cmp( argument, d->tmp_passwd ) )
	{
		send_to_desc( d, "\n\rHas³a nie zgadzaj± siê.\n\r", TRUE );
		send_to_desc( d, "\n\rWprowad¼ jeszcze raz nowe has³o do konta: ", TRUE );
		return NANNY_NEWACC_PASSWD;
	}

	free_string( d->account->passwd );
	d->account->passwd = str_dup( d->tmp_passwd );

	generate_password( buf, 8 );

	free_string( d->account->activation );
	d->account->activation = str_dup( buf );

	mail_account_activation_code( d, MAIL_NEW_ACCOUNT, argument );
	save_account( d->account );

	sprintf( buf, "Zosta³o utworzone konto %s.", d->account->email );
	wiznet( buf, NULL, NULL, 0, 0, 0 );

	print_desc( d, TRUE, "\n\rKonto zosta³o pomy¶lnie stworzone. Teraz zostaniesz przeniesiony do menu\n\r"
						  "konta, w którym mo¿esz stworzyæ swoj± postaæ oraz wykonaæ wiele innych\n\r"
						  "czynno¶ci. Zanim jednak to zrobisz musisz aktywowaæ nowe konto za pomoc±\n\r"
						  "kodu, który zosta³ przes³any na adres e-mail:\n\r"
						  "%s\n\r"
						  "Aktywacji dokonuje sie z poziomu menu (opcja nr %d).\n\r", d->account->email, ACC_ACTIVATE_ACCOUNT );

	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby wy¶wietlic menu konta", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_ACCOUNT_MENU;
}

NANNY( nanny_check_player_passwd )
{
	CHAR_DATA * ch = d->remote_char;

	if ( !ch )
	{
		close_socket( d, 0 );
		return NANNY_EXIT;
	}

	if ( strcmp( argument, ch->pcdata->pwd ) )
	{
		if ( d->pass_fail > 1 )
		{
			send_to_desc( d, "Zle haslo.\n\r\n\r"
			              "{RNacisnij Enter aby zamknac polaczenie z mudem.{x\n\r", TRUE );
			return NANNY_EXIT;
		}
		else
		{
			send_to_desc( d, "\n\rZle haslo. Sprobuj jeszcze raz :", TRUE );
			d->pass_fail++;
			return NANNY_CHECK_PLAYER_PASSWD;
		}
	}

	send_to_desc( d, echo_on_str, FALSE );

	if ( !IS_NULLSTR( ch->pcdata->last_host ) )
		print_desc( d, TRUE, "\n\r* Poprzednie logowanie: %s\n\r", ch->pcdata->last_host );

	free_string( ch->pcdata->last_host );
	ch->pcdata->last_host    = str_dup( d->host );

	show_player_menu( d );
	return NANNY_PLAYER_MENU;
}

NANNY( nanny_check_account_passwd )
{
	if ( strcmp( argument, d->account->passwd ) )
	{
		if ( d->pass_fail > 1 )
		{
			send_to_desc( d, "Zle haslo.\n\r\n\r"
			              "{RNacisnij Enter aby zamknac polaczenie z mudem.{x\n\r", TRUE );
			return NANNY_EXIT;
		}
		else
		{
			send_to_desc( d, "\n\rZle haslo. Sprobuj jeszcze raz :", TRUE );
			d->pass_fail++;
			return NANNY_CHECK_ACCOUNT_PASSWD;
		}
	}

	d->pass_fail = 0;
	write_to_buffer( d, echo_on_str, 0 );

	show_account_menu( d );
	return NANNY_ACCOUNT_MENU;
}

NANNY( nanny_account_menu )
{
	int count;
	int option;
	extern bool newlock;

	if ( d->ignore_input )
	{
		show_account_menu( d );
		d->ignore_input = FALSE;
		return NANNY_ACCOUNT_MENU;
	}

	if ( !is_number( argument ) )
		option = -1;
	else
		option = atoi( argument );

	if ( !IS_NULLSTR( d->account->activation ) &&
	     option != ACC_SHOW_CHARACTERS && option != ACC_LOGOUT_CHARACTERS &&
	     option != ACC_CHANGE_ACCOUNT_EMAIL && option != ACC_REPORT_ERROR &&
	     option != ACC_LOGOUT && option != ACC_ACTIVATE_ACCOUNT &&
	     option != ACC_SEND_ACTIVATION_CODE )
	{
		print_desc( d, TRUE, "\n\r%s  {RMusisz najpierw aktywowaæ swoje konto (opcja nr %d).{x\n\r", d->spaces, ACC_ACTIVATE_ACCOUNT );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	switch ( option )
	{
		case ACC_LOGOUT:
			print_desc( d, TRUE, "%s{RDo zobaczenia!\n\r{x", d->spaces );
			close_socket( d, 0 );
			return NANNY_EXIT;

		case ACC_CREATE_PLAYER:
			if ( newlock )
			{
				print_desc( d, TRUE, "\n\r%s  {RChwilowo nie mo¿na robiæ nowych postaci.{x\n\r", d->spaces );
				print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu konta", d->spaces );
				d->ignore_input = TRUE;
				return NANNY_ACCOUNT_MENU;
			}

			if ( check_ban( d->host, BAN_NEWBIES ) )
			{
				print_desc( d, TRUE, "\n\r%s  {RTworzenie postaci z tego komputera jest zabronione.{x\n\r", d->spaces );
				print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu konta", d->spaces );
				d->ignore_input = TRUE;
				return NANNY_ACCOUNT_MENU;
			}

			//TODO: ban na tworzenie postaci na konto
			//TODO: jaki¶ maximum nowych postaci na koncie?

			if ( d->new_char_data )
				free_new_char_data( d->new_char_data );
			d->new_char_data = new_new_char_data();
			d->new_char_data->creating = CREATE_NAME | CREATE_INFLECT	| CREATE_RACE | CREATE_CLASS |
	        		                  CREATE_SEX | CREATE_SPEC | CREATE_ALIGN | CREATE_PASSWORD;

			send_to_desc( d, "\n\rBla bla bla asd aslah dkjha kdjh ajkdhk ajdhiuh qiudbnjkasd kasjldhaklj\n\r"
							 "dkjahdhh sjdh ashaksjh  ajshdakdh kjdh aksjh asjdh adgh asgdasgagkg ahg\n\r"
							 "jkah gkajs djka bdakj bdkaj dbak jdbak sjd\n\r", TRUE );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby przej¶æ do tworzenia postaci", d->spaces );
			d->ignore_input = TRUE;
			return NANNY_NEWCHAR_MENU;

		case ACC_SHOW_CHARACTERS:
			show_players_list( d, TRUE );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			return NANNY_ACCOUNT_MENU;

		case ACC_CHARACTER_MENU:
			count = show_players_list( d, TRUE );
			if ( count > 0 )
			{
				print_desc( d, TRUE, "\n\r%s{Y->{xWybierz postaæ (lub ENTER by wróciæ do menu): ", d->spaces );
				return NANNY_CHOOSE_PLAYER;
			}
			else
			{
				print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
				d->ignore_input = TRUE;
				return NANNY_ACCOUNT_MENU;
			}

		case ACC_SALVAGE_CHARACTER:
			print_desc( d, TRUE, "\n\r%sOpcja ta s³u¿y do przy³±czania swoich starych postaci do tego konta.\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s{Y->{xPodaj imiê postaci, któr± chcesz przy³±czyæ: ", d->spaces );
			return NANNY_PLAYER_SALVAGE;

		case ACC_DELETE_CHARACTER:
			print_desc( d, TRUE, "\n\r%sOperacja kasowania postaci jest ^f{RNIEDWRACALNA{x.\n\r", d->spaces );

			count = show_players_list( d, TRUE );

			if ( count > 0 )
			{
				print_desc( d, TRUE, "\n\r%s{Y->{xWybierz postaæ do skasowania (lub ENTER by wróciæ do menu): ", d->spaces );
				return NANNY_DELETE_PLAYER;
			}
			else
			{
				print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
				d->ignore_input = TRUE;
				return NANNY_ACCOUNT_MENU;
			}

		case ACC_LOGOUT_CHARACTERS:
			disconnect_account_players( d, TRUE );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			return NANNY_ACCOUNT_MENU;

		case ACC_CHANGE_ACCOUNT_PASSWD:
			print_desc( d, TRUE, "\n\r%s{Y->{xPodaj aktualne has³o do konta: ", d->spaces );
			send_to_desc( d, echo_off_str, FALSE );
			return NANNY_CHACC_PASSWD_OLD;

		case ACC_SEND_ACCOUNT_PASSWD:
			if ( mail_passwd( d, TRUE ) )
				print_desc( d, TRUE, "\n\r%s  {GHas³o zosta³o wys³ane pomy¶lnie.{x\n\r", d->spaces );
			else
				print_desc( d, TRUE, "\n\r%s  {RWys³anie has³a nie uda³o siê. Spróbuj za jaki¶ czas.{x\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			return NANNY_ACCOUNT_MENU;

		case ACC_CHANGE_ACCOUNT_EMAIL:
			count = check_account_players( d );

			if ( count > 0 )
			{
				print_desc( d, TRUE, "\n\r%s  {RAby zmieniæ adres email konta wyjd¼ z gry postaciami z tego konta.{x\n\r", d->spaces );
			}
			else
			{
				print_desc( d, TRUE, "\n\r%sAby zweryfikowaæ poprawno¶æ nowego adresu email po jego zmianie wys³any zostanie\n\r", d->spaces );
				print_desc( d, TRUE, "%skod aktywacyjny. Dopiero po jego prawid³owym wprowadzeniu w menu konta odblokowana\n\r", d->spaces );
				print_desc( d, TRUE, "%szostanie mo¿liwo¶æ wej¶cia do gry postaciami z tego konta.\n\r", d->spaces );
				print_desc( d, TRUE, "\n\r%s{Y->{xPodaj aktualne has³o do konta: ", d->spaces );
				send_to_desc( d, echo_off_str, FALSE );
				return NANNY_CHEMAIL_PASSWD_CONFIRM;
			}

			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			return NANNY_ACCOUNT_MENU;

		case ACC_DELETE_ACCOUNT:
			count = show_players_list( d, FALSE );

			if ( count > 0 )
			{
				print_desc( d, TRUE, "\n\r%s  {RAby skasowaæ konto musisz najpierw skasowaæ wszystkie zwi±zane z nim postaci.{x\n\r", d->spaces );
			}
			else
			{
				print_desc( d, TRUE, "\n\r%sOperacja kasowania konta jest ^f{RNIEDWRACALNA{x.\n\r", d->spaces );
				print_desc( d, TRUE, "\n\r%s{Y->{xPodaj has³o konta: ", d->spaces );
				send_to_desc( d, echo_off_str, FALSE );
				d->pass_fail = 0;
				return NANNY_DELETE_ACCOUNT;
			}

			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			return NANNY_ACCOUNT_MENU;

		case ACC_REPORT_ERROR:
			//TODO
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			return NANNY_ACCOUNT_MENU;

		case ACC_ACTIVATE_ACCOUNT:
			if ( IS_NULLSTR( d->account->activation ) )
			{
				show_account_menu( d );
				return NANNY_ACCOUNT_MENU;
			}

			print_desc( d, TRUE, "\n\r%s{Y->{xPodaj kod aktywacyjny otrzymany na email:", d->spaces );
			return NANNY_ACTIVATE_ACCOUNT;

		case ACC_SEND_ACTIVATION_CODE:
			if ( IS_NULLSTR( d->account->activation ) )
			{
				show_account_menu( d );
				return NANNY_ACCOUNT_MENU;
			}

			if ( mail_account_activation_code( d, MAIL_REMIND_CODE, "" ) )
				print_desc( d, TRUE, "\n\r%s  {GWys³ano kod aktywacyjny na adres %s.{x\n\r", d->spaces, d->account->email );
			else
				print_desc( d, TRUE, "\n\r%s  {RWys³anie kodu aktywacyjnego nie powiod³o siê. Spróbuj pó¼niej.{x\n\r", d->spaces );

			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			return NANNY_ACCOUNT_MENU;

		default:
			show_account_menu( d );
			break;
	}

	return NANNY_ACCOUNT_MENU;
}

NANNY( nanny_choose_player )
{
	char buf[ MAX_STRING_LENGTH ];
	char name[ MAX_STRING_LENGTH ];
	char* char_list;
	int n;
	int count = 1;

	if ( IS_NULLSTR( argument ) )
	{
		show_account_menu( d );
		return NANNY_ACCOUNT_MENU;
	}

	if ( is_number( argument ) )
	{
		n = atoi( argument );
		char_list = one_argument( d->account->characters, name );

		for ( ; ; )
		{
			if ( IS_NULLSTR( name ) || n == count )
				break;

			char_list = one_argument( char_list, name );
			count++;
		}

		if ( IS_NULLSTR( name ) )
		{
			print_desc( d, TRUE, "\n\r%s  {RPodano z³y numer postaci.{x\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			return NANNY_ACCOUNT_MENU;
		}
	}
	else
		sprintf( name, argument );

	sprintf( buf, " %s ", name );

	if ( str_infix( buf, d->account->characters ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RZ tym kontem nie jest zwi±zana taka postaæ.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	if ( check_playing( d, name ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RTa postaæ jest aktualnie u¿ywana.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	d->remote_char = load_char_remote( name );

	if ( d->remote_char != NULL )
	{
		show_player_menu( d );
		return NANNY_PLAYER_MENU;
	}
	else
	{
		print_desc( d, TRUE, "\n\r%s  {RNast±pi³ b³±d przy ³adowaniu postaci {C%s{x{R.{x\n\r", d->spaces, capitalize( name ) );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}
}

NANNY( nanny_player_salvage )
{
	CHAR_DATA * rch;
	char buf[ MAX_STRING_LENGTH ];

	if ( !check_parse_name( argument ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RPodane imiê zawiera nieprawid³owe znaki.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	if ( check_playing( d, argument ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RTa postaæ jest aktualnie u¿ywana.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	sprintf( buf, " %s ", argument );

	if ( !str_infix( buf, d->account->characters ) )
	{
		print_desc( d, TRUE, "\n\r%s  {GTa postaæ jest ju¿ pod³±czona pod Twoje konto.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	if ( ( rch = load_char_remote( argument ) ) == NULL )
	{
		print_desc( d, TRUE, "\n\r%s  {RNie ma takiej postaci.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	if ( !rch->pcdata || !IS_NULLSTR( rch->pcdata->account_email ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RPostaæ jest ju¿ przy³±czona do innego konta.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		free_char( rch );
		return NANNY_ACCOUNT_MENU;
	}

	free_string( d->tmp_passwd );
	d->tmp_passwd = str_dup( rch->pcdata->pwd );

	free_string( d->other );
	d->other = str_dup( rch->name );

	free_char( rch );

	print_desc( d, TRUE, "\n\r%s{Y->{xPodaj has³o postaci, któr± chcesz przy³±czyæ: ", d->spaces );
	send_to_desc( d, echo_off_str, FALSE );
	d->pass_fail = 0;

	return NANNY_CHECK_SALVAGE_PASSWD;
}

NANNY( nanny_check_salvage_passwd )
{
	if ( strcmp( argument, d->tmp_passwd ) )
	{
		if ( d->pass_fail > 1 )
		{
			print_desc( d, TRUE, "\n\r%s  {RZ³e has³o. Przy³±czenie postaci nieudane.{x\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			d->pass_fail = 0;
			return NANNY_ACCOUNT_MENU;
		}
		else
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xZ³e has³o. Spróbuj jeszcze raz:{x", d->spaces );
			d->pass_fail++;
			return NANNY_CHECK_SALVAGE_PASSWD;
		}
	}

	d->pass_fail = 0;

	if ( add_player_to_account( d->account, d->other ) )
		print_desc( d, TRUE, "\n\r%s  {GPrzy³±czenie postaci zakoñczone sukcesem.{x\n\r", d->spaces );
	else
		print_desc( d, TRUE, "\n\r%s  {GPrzy³±czenie postaci nieudane.{x\n\r", d->spaces );

	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_ACCOUNT_MENU;
}

NANNY( nanny_chacc_passwd_old )
{
	if ( strcmp( argument, d->account->passwd ) )
	{
		if ( d->pass_fail > 1 )
		{
			print_desc( d, TRUE, "\n\r%s  {RZ³e has³o. Zmiana has³a do konta nieudana.{x\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			d->pass_fail = 0;
			write_to_buffer( d, echo_on_str, 0 );
			return NANNY_ACCOUNT_MENU;
		}
		else
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xZ³e has³o. Spróbuj jeszcze raz:{x", d->spaces );
			d->pass_fail++;
			return NANNY_CHACC_PASSWD_OLD;
		}
	}

	d->pass_fail = 0;

	print_desc( d, TRUE, "\n\r%s{Y->{xWprowad¼ nowe has³o do konta:", d->spaces );
	return NANNY_CHACC_PASSWD_NEW;
}

NANNY( nanny_chacc_passwd_new )
{
	char buf[ MAX_STRING_LENGTH ];

	sprintf( buf, "%s  {R", d->spaces );

	if ( !check_password( d, buf, argument ) )
	{
		print_desc( d, TRUE, "\n\r%s{Y->{xWprowad¼ nowe has³o do konta:", d->spaces );
		return NANNY_CHACC_PASSWD_NEW;
	}

	free_string( d->tmp_passwd );
	d->tmp_passwd = str_dup( argument );

	print_desc( d, TRUE, "\n\r%s{Y->{xPotwierd¼ nowe has³o do konta:", d->spaces );

	return NANNY_CHACC_PASSWD_CONFIRM;
}

NANNY( nanny_chacc_passwd_confirm )
{
	if ( str_cmp( argument, d->tmp_passwd ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RHas³a nie zgadzaj± siê.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xWprowad¼ jeszcze raz nowe has³o do konta:", d->spaces );
		return NANNY_CHACC_PASSWD_NEW;
	}

	free_string( d->account->passwd );
	d->account->passwd = str_dup( d->tmp_passwd );
	save_account( d->account );

	print_desc( d, TRUE, "\n\r%s  {GHas³o do konta zmienione pomy¶lnie.{x\n\r", d->spaces );
	send_to_desc( d, echo_on_str, FALSE );

	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_ACCOUNT_MENU;
}

NANNY( nanny_delete_player )
{
	CHAR_DATA * rch;
	char buf[ MAX_STRING_LENGTH ];
	char name[ MAX_STRING_LENGTH ];
	char* char_list;
	int n;
	int count = 1;

	if ( IS_NULLSTR( argument ) )
	{
		show_account_menu( d );
		return NANNY_ACCOUNT_MENU;
	}

	if ( is_number( argument ) )
	{
		n = atoi( argument );
		char_list = one_argument( d->account->characters, name );

		for ( ; ; )
		{
			if ( IS_NULLSTR( name ) || n == count )
				break;

			char_list = one_argument( char_list, name );
			count++;
		}

		if ( IS_NULLSTR( name ) )
		{
			print_desc( d, TRUE, "\n\r%s  {RPodano z³y numer postaci.{x\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			return NANNY_ACCOUNT_MENU;
		}
	}
	else
		sprintf( name, argument );

	if ( check_playing( d, argument ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RTa postaæ jest aktualnie u¿ywana. Nie mo¿esz jej teraz skasowaæ.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	sprintf( buf, " %s ", name );

	if ( str_infix( buf, d->account->characters ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RZ tym kontem nie jest zwi±zana taka postaæ.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	if ( ( rch = load_char_remote( name ) ) == NULL )
	{
		print_desc( d, TRUE, "\n\r%s  {RNast±pi³ b³±d przy ³adowaniu postaci {C%s{x{R.{x\n\r", d->spaces, capitalize( name ) );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	free_string( d->tmp_passwd );
	d->tmp_passwd = str_dup( rch->pcdata->pwd );

	free_string( d->other );
	d->other = str_dup( rch->name );

	free_char( rch );

	print_desc( d, TRUE, "\n\r%s{Y->{xPodaj has³o postaci %s: ", d->spaces, d->other );
	send_to_desc( d, echo_off_str, FALSE );
	d->pass_fail = 0;

	return NANNY_CONFIRM_DELETE_PLAYER;
}

NANNY( nanny_confirm_delete_player )
{
	char buf[ MAX_STRING_LENGTH ];
	char strsave[ MAX_INPUT_LENGTH ];

	if ( strcmp( argument, d->tmp_passwd ) )
	{
		if ( d->pass_fail > 1 )
		{
			print_desc( d, TRUE, "\n\r%s  {RZ³e has³o. Kasowanie postaci anulowane.{x\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			d->pass_fail = 0;
			send_to_desc( d, echo_on_str, FALSE );
			return NANNY_ACCOUNT_MENU;
		}
		else
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xZ³e has³o. Spróbuj jeszcze raz:{x", d->spaces );
			d->pass_fail++;
			return NANNY_CONFIRM_DELETE_PLAYER;
		}
	}

	d->pass_fail = 0;
	send_to_desc( d, echo_on_str, FALSE );

	if ( check_playing( d, d->other ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RTa postaæ jest aktualnie u¿ywana. Nie mo¿esz jej teraz skasowaæ.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( d->other ) );

	sprintf( buf, "Postaæ %s zosta³a skasowana z konta %s.", d->other, d->account->email );
	wiznet( buf, NULL, NULL, 0, 0, 0 );

	log_string( buf );
	unlink( strsave );

	remove_player_from_account( d->account, d->other );

	print_desc( d, TRUE, "\n\r%s  {GPostaæ %s skasowana pomy¶lnie.{x\n\r", d->spaces, capitalize( d->other ) );
	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_ACCOUNT_MENU;
}

NANNY( nanny_delete_account )
{
	char buf[ MAX_STRING_LENGTH ];
	char strsave[ MAX_INPUT_LENGTH ];

	if ( strcmp( argument, d->account->passwd ) )
	{
		if ( d->pass_fail > 1 )
		{
			print_desc( d, TRUE, "\n\r%s  {RZ³e has³o. Kasowanie konta anulowane.{x\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			d->pass_fail = 0;
			send_to_desc( d, echo_on_str, FALSE );
			return NANNY_ACCOUNT_MENU;
		}
		else
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xZ³e has³o. Spróbuj jeszcze raz:{x", d->spaces );
			d->pass_fail++;
			return NANNY_DELETE_ACCOUNT;
		}
	}

	d->pass_fail = 0;
	send_to_desc( d, echo_on_str, FALSE );

	sprintf( strsave, "%s%s", ACCOUNT_DIR, d->account->email );

	sprintf( buf, "Konto %s zosta³o skasowane.", d->account->email );
	wiznet( buf, NULL, NULL, 0, 0, 0 );

	log_string( buf );
	unlink( strsave );

	print_desc( d, TRUE, "\n\r%s  {GKonto %s skasowane pomy¶lnie.{x\n\r", d->spaces, d->account->email );

	free_string( d->account->email );
	free_string( d->account->characters );
	free_string( d->account->passwd );
	DISPOSE( d->account );

	print_desc( d, TRUE, "\n\r%s{RDo zobaczenia!\n\r\n\r{x"
	         "%s{RNacisnij Enter aby zamknac polaczenie z mudem.{x\n\r", d->spaces, d->spaces );
	return NANNY_EXIT;
}

NANNY( nanny_chemail_passwd_confirm )
{
	char buf[ MAX_STRING_LENGTH ];

	if ( strcmp( argument, d->account->passwd ) )
	{
		if ( d->pass_fail > 1 )
		{
			print_desc( d, TRUE, "\n\r%s  {RZ³e has³o. Zmiana adresu email anulowana.{x\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			send_to_desc( d, buf, TRUE );
			d->ignore_input = TRUE;
			d->pass_fail = 0;
			send_to_desc( d, echo_on_str, FALSE );
			return NANNY_ACCOUNT_MENU;
		}
		else
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xZ³e has³o. Spróbuj jeszcze raz:{x", d->spaces );
			d->pass_fail++;
			return NANNY_CHEMAIL_PASSWD_CONFIRM;
		}
	}

	d->pass_fail = 0;
	send_to_desc( d, echo_on_str, FALSE );

	print_desc( d, TRUE, "\n\r%s{Y->{xPodaj nowy email dla konta: ", d->spaces );

	return NANNY_CHANGE_EMAIL;
}

NANNY( nanny_change_email )
{
	char buf[ MAX_STRING_LENGTH ];
	char strsave[ MAX_INPUT_LENGTH ];
	int i;

	for ( i = 0; argument[ i ] != '\0'; i++ )
		argument[ i ] = LOWER( argument[ i ] );

	if ( !str_cmp( d->account->email, argument ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RWpisany adres email jest taki sam jak aktualny.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	if ( !validate_email( argument ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RNieprawid³owy format adresu email.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	if ( account_exists( argument ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RKonto z takim adresem e-mail ju¿ istnieje w systemie.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
		d->ignore_input = TRUE;
		return NANNY_ACCOUNT_MENU;
	}

	generate_password( buf, 8 );

	free_string( d->account->activation );
	d->account->activation = str_dup( buf );

	mail_account_activation_code( d, MAIL_CHANGE_ACCOUNT, argument );

	sprintf( strsave, "%s%s", ACCOUNT_DIR, d->account->email );

	sprintf( buf, "Konto %s zosta³o zmienione na %s.", d->account->email, argument );
	wiznet( buf, NULL, NULL, 0, 0, 0 );

	change_account_email( d->account, argument );

	log_string( buf );
	unlink( strsave );

	print_desc( d, TRUE, "\n\r%s  {GAdres email konta zosta³ pomy¶lnie zmieniony.{x\n\r", d->spaces );
	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_ACCOUNT_MENU;
}

NANNY( nanny_activate_account )
{
	if ( strcmp( argument, d->account->activation ) )
	{
		if ( d->pass_fail > 1 )
		{
			print_desc( d, TRUE, "\n\r%s  {RZ³y kod aktywacyjny. Aktywacja konta nie powiod³a siê.{x\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			d->pass_fail = 0;
			return NANNY_ACCOUNT_MENU;
		}
		else
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xZ³y kod aktywacyjny. Spróbuj jeszcze raz:{x", d->spaces );
			d->pass_fail++;
			return NANNY_ACTIVATE_ACCOUNT;
		}
	}

	d->pass_fail = 0;

	free_string( d->account->activation );
	d->account->activation = str_dup( "" );
	save_account( d->account );

	print_desc( d, TRUE, "\n\r%s  {GKonto zosta³o pomy¶lnie aktywowane.{x\n\r", d->spaces );
	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_ACCOUNT_MENU;
}

void show_create_menu( DESCRIPTOR_DATA *d )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;
	char detail[ MAX_STRING_LENGTH ];

	show_header( d, "", "=<>=", "Nowa postaæ", 'Y', 'c', 'C' );

	if ( !IS_SET( new_ch->creating, CREATE_NAME ) )
		sprintf( detail, "({G%s{x)", new_ch->name );
	else
		 sprintf( detail, "({Rnieustawione{x)" );

	print_desc( d, TRUE, "%s  {C%2d.{x Imiê. %s\n\r", d->spaces, CRE_NAME, detail );

	if ( !IS_SET( new_ch->creating, CREATE_NAME ) )
	{
		if ( !IS_SET( new_ch->creating, CREATE_INFLECT ) )
			 sprintf( detail, "({Gustawiona{x)" );
		else
			 sprintf( detail, "({Rnieustawiona{x)" );

		print_desc( d, TRUE, "%s  {C%2d.{x Odmiana imienia. %s\n\r", d->spaces, CRE_INFLECT, detail );
	}

	if ( !IS_SET( new_ch->creating, CREATE_SEX ) )
		 sprintf( detail, "({G%s{x)", new_ch->sex == 1 ? "mê¿czyzna" : "kobieta" );
	else
		 sprintf( detail, "({Rnieustawiona{x)" );

	print_desc( d, TRUE, "%s  {C%2d.{x P³eæ. %s\n\r", d->spaces, CRE_SEX, detail );

	if ( !IS_SET( new_ch->creating, CREATE_RACE ) )
		 sprintf( detail, "({G%s{x)", race_table[ new_ch->race ].name );
	else
		 sprintf( detail, "({Rnieustawiona{x)" );

	print_desc( d, TRUE, "%s  {C%2d.{x Rasa. %s\n\r", d->spaces, CRE_RACE, detail );

	if ( !IS_SET( new_ch->creating, CREATE_RACE ) )
	{
		if ( !IS_SET( new_ch->creating, CREATE_CLASS ) )
			 sprintf( detail, "({G%s{x)", new_get_class_name( new_ch ) );
		else
			 sprintf( detail, "({Rnieustawiona{x)" );

		print_desc( d, TRUE, "%s  {C%2d.{x Profesja. %s\n\r", d->spaces, CRE_CLASS, detail );
	}

	if ( !IS_SET( new_ch->creating, CREATE_CLASS ) && new_ch->class == CLASS_MAG && new_can_be_any_specialist( new_ch ) )
	{
		if ( !IS_SET( new_ch->creating, CREATE_SPEC ) )
			 sprintf( detail, "({G%s{x)", new_ch->mage_specialist >= 0 ? school_table[ URANGE( 0, new_ch->mage_specialist, 7 ) ].name : "mag ogólny" );
		else
			 sprintf( detail, "({Rnieustawiona{x)" );

		print_desc( d, TRUE, "%s  {C%2d.{x Specjalizacja. %s\n\r", d->spaces, CRE_SPEC, detail);
	}

	if ( !IS_SET( new_ch->creating, CREATE_ALIGN ) )
		 sprintf( detail, "({G%s{x)", ALIGN_STR( new_ch ) );
	else
		 sprintf( detail, "({Rnieustawiona{x)" );

	print_desc( d, TRUE, "%s  {C%2d.{x Charakter. %s\n\r", d->spaces, CRE_ALIGN, detail );

	if ( !IS_SET( new_ch->creating, CREATE_CHARACTERISTIC ) )
		 sprintf( detail, "({Gustawiony{x)" );
	else
		 sprintf( detail, "({Rnieustawiony{x)" );

	print_desc( d, TRUE, "%s  {C%2d.{x Wygl±d. %s\n\r", d->spaces, CRE_CHARACTERISTIC, detail );

	if ( !IS_SET( new_ch->creating, CREATE_PASSWORD ) )
		 sprintf( detail, "({Gustawione{x)" );
	else
		 sprintf( detail, "({Rnieustawione{x)" );

	print_desc( d, TRUE, "%s  {C%2d.{x Has³o. %s\n\r\n\r", d->spaces, CRE_PASSWORD, detail );

	//TODO: a mo¿e przy tworzeniu postaci ustawiaæ tak¿e miejsce jej startu (co¶ w stylu miejsca urodzenia)

	if ( new_ch->creating == 0 )
		print_desc( d, TRUE, "%s  {C%2d.{x Zatwierd¼ now± postaæ.\n\r", d->spaces, CRE_ACCEPT );

	print_desc( d, TRUE, "%s  {C%2d.{x Rezygnuj z tworzenia nowej postaci.\n\r", d->spaces, CRE_RESIGN );

	print_desc( d, TRUE, "\n\r%s{Y->{xWybierz opcjê:", d->spaces );

	return ;
}

void show_races( DESCRIPTOR_DATA *d )
{
	char buf[ MAX_STRING_LENGTH ];
	int iClass;
	int race;

	if ( !d->new_char_data )
		return;

	print_desc( d, TRUE, "\n\r%s  Dostêpne rasy:\n\r", d->spaces );

	for ( race = 1; race_table[ race ].name != NULL; race++ )
	{
		if ( !race_table[ race ].pc_race )
			break;

		sprintf( buf, "%s  {M%d{x. %-9.9s [ ", d->spaces, race, race_table[ race ].name );

		for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
		{
			if ( iClass == CLASS_MONK || iClass == CLASS_BARD || iClass == CLASS_SHAMAN )
				continue;

			if ( IS_SET( pc_race_table[ race ].class_flag, ( 1 << iClass ) ) )
			{
				strcat( buf, class_table[ iClass ].name );
				strcat( buf, " " );
			}
		}

		strcat( buf, "]\n\r" );
		send_to_desc( d, buf, TRUE );
	}
	print_desc( d, TRUE, "\n\r%s{Y->{xJak± rasê wybierasz? ", d->spaces );
}

void show_classes( DESCRIPTOR_DATA *d )
{
	int iClass;
	int race;

	if ( !d->new_char_data )
		return;

	print_desc( d, TRUE, "\n\r%s  Dostêpne profesje:\n\r", d->spaces );

	for ( race = 1, iClass = 0; iClass < MAX_CLASS; iClass++ )
	{
		if ( iClass == CLASS_MONK || iClass == CLASS_BARD )
		continue;

		if ( new_class_ok( d->new_char_data, iClass ) )
		{
			print_desc( d, TRUE, "%s  {M%d{x. %s\n\r", d->spaces, race++, class_table[ iClass ].name );
		}
	}

	print_desc( d, TRUE, "\n\r%s{Y->{xJak± profesjê wybierasz? ", d->spaces );
}

void show_spec( DESCRIPTOR_DATA *d )
{
	int x;
	int school;

	if ( !d->new_char_data )
		return;

	print_desc( d, TRUE, "\n\r%s  Mo¿esz wybraæ specjalizacjê w jednej z poni¿szych szkó³ magii:\n\r", d->spaces );

	print_desc( d, TRUE, "%s  {M0{x. bez specjalizacji    (Mag ogólny   )\n\r", d->spaces );
	for ( x = 1, school = 0 ; school < MAX_SCHOOL ; school++ )
	{
		if ( new_can_be_specialist( d->new_char_data, school ) )
		{
			print_desc( d, TRUE, "%s  {M%d{x. %-20s (%-13s)\n\r", d->spaces, x++, school_table[ school ].specialist_name, school_table[ school ].name );
		}
	}

	print_desc( d, TRUE, "\n\r%s{Y->{xJak± specjalizacjê wybierasz? ", d->spaces );
}

void show_align( DESCRIPTOR_DATA *d )
{

	if ( !d->new_char_data )
		return;

	print_desc( d, TRUE, "\n\r%s  Twoja postaæ mo¿e mieæ jeden z nastêpuj±cych charakterów:\n\r", d->spaces );

	if ( d->new_char_data->class == CLASS_PALADIN )
		print_desc( d, TRUE, "%s  {M1{x. dobry\n\r", d->spaces );
	else if ( d->new_char_data->class == CLASS_DRUID )
		print_desc( d, TRUE, "%s  {M1{x. neutralny\n\r", d->spaces );
	else if ( d->new_char_data->class == CLASS_BLACK_KNIGHT )
		print_desc( d, TRUE, "%s  {M1{x. z³y\n\r", d->spaces );
	else if ( school_table[ d->new_char_data->mage_specialist ].flag == Nekromancja )
		print_desc( d, TRUE, "%s  {M1{x. z³y\n\r", d->spaces );
	else
	{
		print_desc( d, TRUE, "%s  {M1{x. dobry\n\r", d->spaces );
		print_desc( d, TRUE, "%s  {M2{x. neutralny\n\r", d->spaces );
		print_desc( d, TRUE, "%s  {M3{x. z³y\n\r", d->spaces );
	}

	print_desc( d, TRUE, "\n\r%s{Y->{xJaki charakter wybierasz? ", d->spaces );
}

void help_to_desc( DESCRIPTOR_DATA *d, char* argument )
{
	HELP_DATA * pHelp;

	/* this parts handles help a b so that it returns help 'a b' */

	for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
	{
		if ( is_name( argument, pHelp->keyword ) )
		{
			if ( pHelp->text[ 0 ] == '.' )
				send_to_desc(d, pHelp->text + 1, TRUE );
			else
				send_to_desc(d, pHelp->text, TRUE );

			break;
		}
	}
}

NANNY( nanny_newchar_menu )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;
	int option;

	if ( d->ignore_input )
	{
		show_create_menu( d );
		d->ignore_input = FALSE;
		return NANNY_NEWCHAR_MENU;
	}

	if ( !is_number( argument ) )
		option = -1;
	else
		option = atoi( argument );

	switch ( option )
	{
		case CRE_NAME:
			//TODO: opis
			send_to_desc( d, "\n\rBla bla bla asd aslah dkjha kdjh ajkdhk ajdhiuh qiudbnjkasd kasjldhaklj\n\r"
							 "dkjahdhh sjdh ashaksjh  ajshdakdh kjdh aksjh asjdh adgh asgdasgagkg ahg\n\r"
							 "jkah gkajs djka bdakj bdkaj dbak jdbak sjd\n\r", TRUE );
			print_desc( d, TRUE, "\n\r%s{Y->{xPodaj imiê dla nowej postaci: ", d->spaces );
			return NANNY_NEWCHAR_NAME;

		case CRE_INFLECT:
			if ( IS_SET( new_ch->creating, CREATE_NAME ) )
			{
				show_create_menu( d );
				break;
			}

			free_string( new_ch->name2 );
			free_string( new_ch->name3 );
			free_string( new_ch->name4 );
			free_string( new_ch->name5 );
			free_string( new_ch->name6 );
			new_ch->name2 = str_dup( "null" );
			new_ch->name3 = str_dup( "null" );
			new_ch->name4 = str_dup( "null" );
			new_ch->name5 = str_dup( "null" );
			new_ch->name6 = str_dup( "null" );
            send_to_desc( d, "\n\rTeraz musisz podaæ odmianê swojego imienia. Potrzebne jest to do poprawnej\n\r"
							 "interakcji Twojej postaci z otoczeniem, dlatego prosimy o szczególn± uwagê\n\r"
							 "przy jej wpisywaniu.\n\r", 0 );
			print_desc( d, TRUE, "\n\r%s  Wpisz odmianê swojego imienia:", d->spaces );
			print_desc( d, TRUE, "\n\r%s  Mianownik (kto?): %s\n\r", d->spaces, new_ch->name );
			print_desc( d, TRUE, "\n\r%s{Y->{xDope³niacz (kogo nie ma?): ", d->spaces );
			return NANNY_NEWCHAR_INFLECT;

		case CRE_SEX:
			//TODO: opis
			send_to_desc( d, "\n\rBla bla bla asd aslah dkjha kdjh ajkdhk ajdhiuh qiudbnjkasd kasjldhaklj\n\r"
							 "dkjahdhh sjdh ashaksjh  ajshdakdh kjdh aksjh asjdh adgh asgdasgagkg ahg\n\r"
							 "jkah gkajs djka bdakj bdkaj dbak jdbak sjd\n\r", TRUE );
			print_desc( d, TRUE, "\n\r%s{Y->{xWybierz p³eæ swojej postaci (K/M): ", d->spaces );
			return NANNY_NEWCHAR_SEX;

		case CRE_RACE:
			send_to_desc( d, "\n\rTeraz stoi przed Tob± zadanie wybrania rasy swojej postaci. Jest to\n\r"
							 "bardzo wa¿ny punkt tworzenia postaci, gdy¿ to jak± rasê wybierzesz bêdzie\n\r"
							 "wp³ywaæ na wiele ró¿nych cech Twojej postaci. Pamiêtaj tak¿e, i¿ nie wybór\n\r"
							 "profesji tak¿e uzale¿niony jest od wybranej rasy.\n\r"
							 "Po wybraniu rasy wy¶wietli siê krótka informacja na jej temat.\n\r", TRUE );

			show_races( d );
			return NANNY_NEWCHAR_RACE;

		case CRE_CLASS:
			if ( IS_SET( new_ch->creating, CREATE_RACE ) )
			{
				show_create_menu( d );
				break;
			}

            send_to_desc( d, "\n\rProfesja to najwa¿niejsza cecha Twojej postaci. Wp³ywa na to jakich\n\r"
							 "umiejêtno¶ci bêdzie mog³a siê nauczyæ oraz jak± rolê bêdzie odgrywaæ w grze.\n\r"
							 "Dla czê¶ci profesji jedyn± drog± prze¿ycia jest pos³ugiwanie siê magi±. Inne\n\r"
							 "znakomicie radz± sobie ca³kowicie bez niej. S± i takie, które swoj± fizyczn±\n\r"
							 "si³ê wspomagaj± odrobin± magicznej mocy.\n\r", TRUE );

            show_classes( d );
			return NANNY_NEWCHAR_CLASS;

		case CRE_SPEC:
			if ( IS_SET( new_ch->creating, CREATE_CLASS ) || new_ch->class != CLASS_MAG || !new_can_be_any_specialist( new_ch ) )
			{
				show_create_menu( d );
				break;
			}
			//TODO: opis
			send_to_desc( d, "\n\rBla bla bla asd aslah dkjha kdjh ajkdhk ajdhiuh qiudbnjkasd kasjldhaklj\n\r"
							 "dkjahdhh sjdh ashaksjh  ajshdakdh kjdh aksjh asjdh adgh asgdasgagkg ahg\n\r"
							 "jkah gkajs djka bdakj bdkaj dbak jdbak sjd\n\r", TRUE );

			show_spec( d );

			return NANNY_NEWCHAR_SPEC;

		case CRE_ALIGN:
			send_to_desc( d,"\n\rStoi przed tob± zadanie wyboru charakteru swojej postaci. Mo¿esz zadeklarowac\n\r"
							"czy chcesz byæ z natury postaci± dobr± i zwalczaæ pleni±ce siê po ¶wiecie z³o\n\r"
							"albo postaci± lubuj±c± siê w niecno¶ciach i nienawidzac± wszelkich przejawów\n\r"
							"dobrej woli. Je¶li nie mo¿esz siê zdecydowaæ co do charakteru postaci masz\n\r"
							"mo¿liwo¶æ wybrania moralnej neutralno¶ci.\n\r"
							"Pamiêtaj: paladyni mog± mieæ tylko dobry charakter, druida mog± byæ tylko\n\r"
							"neutralni a magowi o specjalizacji nekromanty mog± byæ tylko ¼li.\n\r" , TRUE );

			show_align( d );
			return NANNY_NEWCHAR_ALIGN;

		case CRE_PASSWORD:
			//TODO: opis
			send_to_desc( d, "\n\rBla bla bla asd aslah dkjha kdjh ajkdhk ajdhiuh qiudbnjkasd kasjldhaklj\n\r"
							 "dkjahdhh sjdh ashaksjh  ajshdakdh kjdh aksjh asjdh adgh asgdasgagkg ahg\n\r"
							 "jkah gkajs djka bdakj bdkaj dbak jdbak sjd\n\r", TRUE );
			print_desc( d, TRUE, "\n\r%s{Y->{xWprowad¼ has³o do postaci: ", d->spaces );
			send_to_desc( d, echo_off_str, FALSE );
			return NANNY_NEWCHAR_PASSWORD;

		case CRE_ACCEPT:
			if ( new_ch->creating != 0 )
			{
				show_create_menu( d );
				break;
			}
			//TODO: wy¶wielenie podsumowania postaci
			print_desc( d, TRUE, "\n\r\n\r%s{Y->{xCzy chcesz zakoñczyæ tworzenie tej postaci (t/n)? ", d->spaces );
			return NANNY_NEWCHAR_ACCEPT;

		case CRE_RESIGN:
			print_desc( d, TRUE, "\n\r\n\r%s{Y->{xCzy na pewno chcesz zrezygnowaæ z tworzenia tej postaci (t/n)? ", d->spaces );
			return NANNY_NEWCHAR_RESIGN;

		default:
			show_create_menu( d );
			break;
	}

	return NANNY_NEWCHAR_MENU;
}

NANNY( nanny_newchar_name )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;
	char * reserv_pwd;
	int i;

	//scinanie duzych liter
	for ( i = 0; argument[ i ] != '\0'; i++ )
		argument[ i ] = LOWER( argument[ i ] );

	//pierwsza wielka
	argument[ 0 ] = UPPER( argument[ 0 ] );

	if ( !check_parse_name( argument ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RZabronione imiê.\n\r{x", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xPodaj inne imiê: {x", d->spaces );
		return NANNY_NEWCHAR_NAME;
	}

	if ( strlen( argument ) < 4 )
	{
		print_desc( d, TRUE, "\n\r%s  {RImiê musi mieæ co najmniej cztery litery.\n\r{x", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xPodaj inne imiê: {x", d->spaces );
		return NANNY_NEWCHAR_NAME;
	}

	if ( player_exists( argument ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RIstnieje ju¿ postaæ o takim imieniu.\n\r{x", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xPodaj inne imiê: {x", d->spaces );
		return NANNY_NEWCHAR_NAME;
	}

	if ( is_allowed( argument ) == NAME_DISALLOWED )
	{
		print_desc( d, TRUE, "\n\r%s  {RImiê nieakceptowalne.\n\r{x", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xPodaj inne imiê: {x", d->spaces );
		return NANNY_NEWCHAR_NAME;
	}

	if ( ( reserv_pwd = is_name_reserved( argument ) ) != NULL )
	{
		print_desc( d, TRUE, "\n\r%s  {RTo imiê jest zarezerwowane.\n\r{x", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xPodaj has³o rezerwacji: {x", d->spaces );
		send_to_desc( d, echo_off_str, FALSE );
		d->pass_fail = 0;
		free_string( d->tmp_passwd );
		d->tmp_passwd = reserv_pwd;
		return NANNY_NEWCHAR_RESERVED_PWD;
	}

	free_string( new_ch->name );
	new_ch->name = str_dup( argument );

	REMOVE_BIT( new_ch->creating, CREATE_NAME );
	print_desc( d, TRUE, "\n\r%s  {GTwoje imiê: %s.{x\n\r", d->spaces, new_ch->name );
	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_NEWCHAR_MENU;
}

NANNY( nanny_newchar_reserved_pwd )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;

	if ( strcmp( argument, d->tmp_passwd ) )
	{
		if ( d->pass_fail > 1 )
		{
			print_desc( d, TRUE, "\n\r%s  {RZ³e has³o.{x\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			d->pass_fail = 0;
			send_to_desc( d, echo_on_str, FALSE );
			return NANNY_NEWCHAR_MENU;
		}
		else
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xZ³e has³o. Spróbuj jeszcze raz:{x", d->spaces );
			d->pass_fail++;
			return NANNY_NEWCHAR_RESERVED_PWD;
		}
	}

	d->pass_fail = 0;
	send_to_desc( d, echo_on_str, FALSE );

	free_string( new_ch->name );
	new_ch->name = str_dup( argument );

	REMOVE_BIT( new_ch->creating, CREATE_NAME );
	print_desc( d, TRUE, "\n\r%s  {GTwoje imiê: %s.{x\n\r", d->spaces, new_ch->name );
	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_NEWCHAR_MENU;
}

NANNY( nanny_newchar_inflect )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;
	char arg[ MAX_INPUT_LENGTH ];

	if ( !str_cmp( new_ch->name2, "null" ) )
	{
		one_argument( argument, arg );
		smash_tilde( arg );

		if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( new_ch->name[ 0 ] ) || !check_all_alpha ( arg ) )
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xDope³niacz (kogo nie ma?): ", d->spaces );
			return NANNY_NEWCHAR_INFLECT;
		}

		arg[ 0 ] = UPPER( arg[ 0 ] );

		free_string( new_ch->name2 );
		new_ch->name2 = str_dup( arg );

		print_desc( d, TRUE, "\n\r%s{Y->{xCelownik (komu siê przygl±dam?): ", d->spaces );
		return NANNY_NEWCHAR_INFLECT;
	}

	if ( !str_cmp( new_ch->name3, "null" ) )
	{
		one_argument( argument, arg );
		smash_tilde( arg );

		if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( new_ch->name[ 0 ] ) || !check_all_alpha ( arg ) )
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xCelownik (komu siê przygl±dam?): ", d->spaces );
			return NANNY_NEWCHAR_INFLECT;
		}

		arg[ 0 ] = UPPER( arg[ 0 ] );

		free_string( new_ch->name3 );
		new_ch->name3 = str_dup( arg );

		print_desc( d, TRUE, "\n\r%s{Y->{xBiernik (kogo widzê?): ", d->spaces );
		return NANNY_NEWCHAR_INFLECT;
	}

	if ( !str_cmp( new_ch->name4, "null" ) )
	{
		one_argument( argument, arg );
		smash_tilde( arg );

		if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( new_ch->name[ 0 ] ) || !check_all_alpha ( arg ) )
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xBiernik (kogo widzê?): ", d->spaces );
			return NANNY_NEWCHAR_INFLECT;
		}

		arg[ 0 ] = UPPER( arg[ 0 ] );

		free_string( new_ch->name4 );
		new_ch->name4 = str_dup( arg );

		print_desc( d, TRUE, "\n\r%s{Y->{xNarzêdnik (z kim idê?): ", d->spaces );
		return NANNY_NEWCHAR_INFLECT;
	}

	if ( !str_cmp( new_ch->name5, "null" ) )
	{
		one_argument( argument, arg );
		smash_tilde( arg );

		if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( new_ch->name[ 0 ] ) || !check_all_alpha ( arg ) )
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xNarzêdnik (z kim idê?): ", d->spaces );
			return NANNY_NEWCHAR_INFLECT;
		}

		arg[ 0 ] = UPPER( arg[ 0 ] );

		free_string( new_ch->name5 );
		new_ch->name5 = str_dup( arg );

		print_desc( d, TRUE, "\n\r%s{Y->{xMiejscownik (o kim mówiê?): ", d->spaces );
		return NANNY_NEWCHAR_INFLECT;
	}

	if ( !str_cmp( new_ch->name6, "null" ) )
	{
		one_argument( argument, arg );
		smash_tilde( arg );

		if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( new_ch->name[ 0 ] ) || !check_all_alpha ( arg ) )
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xMiejscownik (o kim mówiê?): ", d->spaces );
			return NANNY_NEWCHAR_INFLECT;
		}

		arg[ 0 ] = UPPER( arg[ 0 ] );

		free_string( new_ch->name6 );
		new_ch->name6 = str_dup( arg );


		print_desc( d, TRUE, "\n\r%s  Ustawiona odmiana:\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s  Mianownik (kto?):                {G%s{x", d->spaces, new_ch->name );
		print_desc( d, TRUE, "\n\r%s  Dope³niacz (kogo nie ma?):       {G%s{x", d->spaces, new_ch->name2 );
		print_desc( d, TRUE, "\n\r%s  Celownik (komu siê przygl±dam?): {G%s{x", d->spaces, new_ch->name3 );
		print_desc( d, TRUE, "\n\r%s  Biernik (kogo widzê?):           {G%s{x", d->spaces, new_ch->name4 );
		print_desc( d, TRUE, "\n\r%s  Narzêdnik (z kim idê?):          {G%s{x", d->spaces, new_ch->name5 );
		print_desc( d, TRUE, "\n\r%s  Miejscownik (o kim mówiê?):      {G%s{x", d->spaces, new_ch->name6 );
		print_desc( d, TRUE, "\n\r\n\r%s{Y->{xCzy odmiana jest poprawna (t/n)? ", d->spaces );
		return NANNY_NEWCHAR_CONFIRM_INFLECT;
	}

	return NANNY_NEWCHAR_INFLECT;
}

NANNY( nanny_newchar_confirm_inflect )
{
	NEW_CHAR_DATA *new_ch = d->new_char_data;

	switch( argument[0] )
	{
		case 't':
		case 'T':
			REMOVE_BIT( new_ch->creating, CREATE_INFLECT );
			show_create_menu( d );
			return NANNY_NEWCHAR_MENU;

		case 'n':
		case 'N':
			print_desc( d, TRUE, "\n\r%s  Wpisz odmianê swojego imienia:", d->spaces );
			print_desc( d, TRUE, "\n\r%s  Mianownik (kto?): %s\n\r", d->spaces, new_ch->name );
			print_desc( d, TRUE, "\n\r%s{Y->{xDope³niacz (kogo nie ma?): ", d->spaces );
			free_string( new_ch->name2 );
			free_string( new_ch->name3 );
			free_string( new_ch->name4 );
			free_string( new_ch->name5 );
			free_string( new_ch->name6 );
			new_ch->name2 = str_dup( "null" );
			new_ch->name3 = str_dup( "null" );
			new_ch->name4 = str_dup( "null" );
			new_ch->name5 = str_dup( "null" );
			new_ch->name6 = str_dup( "null" );
			return NANNY_NEWCHAR_INFLECT;

		default:
			print_desc( d, TRUE, "\n\r\n\r%s  Ustawiona odmiana:\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s  Mianownik (kto?):                {G%s{x", d->spaces, new_ch->name );
			print_desc( d, TRUE, "\n\r%s  Dope³niacz (kogo nie ma?):       {G%s{x", d->spaces, new_ch->name2 );
			print_desc( d, TRUE, "\n\r%s  Celownik (komu siê przygl±dam?): {G%s{x", d->spaces, new_ch->name3 );
			print_desc( d, TRUE, "\n\r%s  Biernik (kogo widzê?):           {G%s{x", d->spaces, new_ch->name4 );
			print_desc( d, TRUE, "\n\r%s  Narzêdnik (z kim idê?):          {G%s{x", d->spaces, new_ch->name5 );
			print_desc( d, TRUE, "\n\r%s  Miejscownik (o kim mówiê?):      {G%s{x", d->spaces, new_ch->name6 );
			print_desc( d, TRUE, "\n\r\n\r%s{Y->{xCzy odmiana jest poprawna (t/n)? ", d->spaces );
			return NANNY_NEWCHAR_CONFIRM_INFLECT;
	}

	return NANNY_NEWCHAR_MENU;
}

NANNY( nanny_newchar_sex )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;

	switch ( argument[0] )
	{
		case 'm':
		case 'M':
			new_ch->sex = SEX_MALE;
			break;
		case 'k':
		case 'K':
			new_ch->sex = SEX_FEMALE;
			break;

		default:
			print_desc( d, TRUE, "\n\r%s  {RTo nie jest prawid³owa p³eæ.\n\r{x", d->spaces );
			print_desc( d, TRUE, "\n\r%s{Y->{xWybierz p³eæ swojej postaci (K/M): {x", d->spaces );
			return NANNY_NEWCHAR_SEX;
	}

	REMOVE_BIT( new_ch->creating, CREATE_SEX );
	print_desc( d, TRUE, "\n\r%s  {GTwoja p³eæ: %s.{x\n\r", d->spaces, new_ch->sex == 1 ? "mê¿czyzna" : "kobieta" );
	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_NEWCHAR_MENU;
}

NANNY( nanny_newchar_race )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;
	char arg[ MAX_INPUT_LENGTH ];
	int race;

	one_argument( argument, arg );

	if ( is_number( arg ) )
	{
		race = atoi( arg );

		if ( race <= 0 || race > 7 || !race_table[ race ].pc_race )
			race = 0;
	}
	else
		race = race_lookup( arg );

	if ( race == 0 || !race_table[ race ].pc_race )
	{
		print_desc( d, TRUE, "\n\r%s  {RNieprawid³owa rasa.\n\r{x", d->spaces );
		show_races( d );
		return NANNY_NEWCHAR_RACE;
	}

	new_ch->race = race;

	send_to_desc( d, "\n\r", TRUE );
	help_to_desc( d, race_table[ race ].name );

	print_desc( d, TRUE, "\n\r%s{Y->{xCzy chcesz wybraæ rasê %s (t/n)? ", d->spaces, race_table[ race ].name );
	return NANNY_NEWCHAR_CONFIRM_RACE;
}

NANNY( nanny_newchar_confirm_race )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;

	switch ( argument[0] )
	{
		case 't':
		case 'T':
			if ( !new_class_ok( new_ch, new_ch->class ) )
			{
				SET_BIT( new_ch->creating, CREATE_CLASS );
				SET_BIT( new_ch->creating, CREATE_SPEC );
			}

			if ( new_ch->class == CLASS_MAG && !new_can_be_specialist( new_ch, new_ch->mage_specialist ) )
			{
				SET_BIT( new_ch->creating, CREATE_SPEC );
			}

			break;
		case 'n':
		case 'N':
			new_ch->race = 0;
			show_races( d );
			return NANNY_NEWCHAR_RACE;

		default:
			print_desc( d, TRUE, "\n\r\n\r%s{Y->{xCzy chcesz wybraæ rasê %s (t/n)? ", d->spaces, race_table[ new_ch->race ].name );
			return NANNY_NEWCHAR_CONFIRM_RACE;
	}

	REMOVE_BIT( new_ch->creating, CREATE_RACE );
	print_desc( d, TRUE, "\n\r%s  {GTwoja rasa: %s.{x\n\r", d->spaces, race_table[ new_ch->race ].name );
	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_NEWCHAR_MENU;
}

NANNY( nanny_newchar_class )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;
	char arg[ MAX_INPUT_LENGTH ];
	int iClass = -1;
	int i;

	one_argument( argument, arg );

	if ( is_number( arg ) )
	{
		int new_class = atoi( argument );

		if ( new_class < 0 || new_class > MAX_CLASS )
		{
			iClass = -1;
		}
		else
		{
			for ( i = 1, iClass = 0; iClass < MAX_CLASS; iClass++ )
				if ( new_class_ok( new_ch, iClass ) && i++ == new_class )
					break;
			if ( ( i - 1 ) != new_class )
				iClass = -1;
		}

		if ( iClass == CLASS_MONK || iClass == CLASS_BARD || iClass == CLASS_SHAMAN )
		{
			iClass = -1;
		}
	}
	else
	{
		iClass = class_lookup( argument );

		if ( iClass == CLASS_MONK || iClass == CLASS_BARD || iClass == CLASS_SHAMAN )
		{
			iClass = -1;
		}

		if ( iClass != -1 && !new_class_ok( new_ch, iClass ) )
		{
			iClass = -1;
		}
	}

	if ( iClass == -1 )
	{
		print_desc( d, TRUE, "\n\r%s  {RNieprawid³owa profesja.\n\r{x", d->spaces );
		show_classes( d );
		return NANNY_NEWCHAR_CLASS;
	}

	send_to_desc( d, "\n\r", TRUE );
	help_to_desc( d, class_table[ iClass ].name );
	new_ch->class = iClass;

	print_desc( d, TRUE, "\n\r%s{Y->{xCzy chcesz wybraæ profesjê %s (t/n)? ", d->spaces, new_get_class_name( new_ch ) );
	return NANNY_NEWCHAR_CONFIRM_CLASS;
}

NANNY( nanny_newchar_confirm_class )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;

	switch ( argument[ 0 ] )
	{
		case 't':
		case 'T':
			/* niektórzy nie wybieraja charakteru*/
			if ( new_ch->class == CLASS_DRUID )
				SET_BIT( new_ch->creating, CREATE_ALIGN );
			else if ( new_ch->class == CLASS_PALADIN )
				SET_BIT( new_ch->creating, CREATE_ALIGN );
			else if ( new_ch->class == CLASS_BLACK_KNIGHT )
				SET_BIT( new_ch->creating, CREATE_ALIGN );

			if ( new_ch->class != CLASS_MAG )
			{
				new_ch->mage_specialist = -1;
				REMOVE_BIT( new_ch->creating, CREATE_SPEC );
			}
			else
			{
				if ( new_ch->mage_specialist == -1 )
					SET_BIT( new_ch->creating, CREATE_SPEC );
			}
			break;
		case 'n':
		case 'N':
			new_ch->class = 0;
			show_classes( d );
			return NANNY_NEWCHAR_CLASS;

		default:
			print_desc( d, TRUE, "\n\r\n\r%s{Y->{xCzy chcesz wybraæ profesjê %s (t/n)? ", d->spaces, new_get_class_name( new_ch ) );
			show_spec( d );
			return NANNY_NEWCHAR_CONFIRM_CLASS;
	}

	REMOVE_BIT( new_ch->creating, CREATE_CLASS );
	print_desc( d, TRUE, "\n\r%s  {GTwoja profesja: %s.{x\n\r", d->spaces, new_get_class_name( new_ch ) );
	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_NEWCHAR_MENU;
}

NANNY( nanny_newchar_spec )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;
	char arg[ MAX_INPUT_LENGTH ];
    int x;
    int school;
    int num;
    int wrong = FALSE;
    int found = FALSE;

	argument = one_argument( argument, arg );

    if ( arg[ 0 ] != '\0' )
    {
        /* jesli wpisal nazwe slownie */
        if ( !is_number( arg ) )
        {
            for ( school = 0; school < MAX_SCHOOL; school++ )
            {
                if ( !str_prefix( arg, school_table[ school ].name ) )
                {
                    if ( new_can_be_specialist( new_ch, school ) )
                        found = TRUE;
                    break;
                }
            }
        }
        /* a moze wpisal cyferkie */
        else
        {
            num = atoi( arg );
            if ( num == 0 )
            {
            	school = -1;
            	found = TRUE;
        	}
        	else if ( num < 0 || num > MAX_SCHOOL )
            {
            	wrong = TRUE;
            }
			else
			{
	            for ( x = 1, school = 0; school < MAX_SCHOOL; school++ )
	            {
	                if ( new_can_be_specialist( new_ch, school ) )
	                {
	                    if ( x == num )
	                    {
	                    	found = TRUE;
	                        break;
	                    }
	                    x++;
	                }
	            }
	        }
        }
    }
    else wrong = TRUE;

	if ( wrong || !found )
	{
		print_desc( d, TRUE, "\n\r%s  {RNieprawid³owa specjalizacja.\n\r{x", d->spaces );
		show_spec( d );
		return NANNY_NEWCHAR_SPEC;
	}

	new_ch->mage_specialist = school;

	if ( school_table[ new_ch->mage_specialist ].flag == Nekromancja )
	{
		SET_BIT( new_ch->creating, CREATE_ALIGN );
	}

	REMOVE_BIT( new_ch->creating, CREATE_SPEC );
	print_desc( d, TRUE, "\n\r%s  {GTwoja specjalizacja magiczna: %s.{x\n\r", d->spaces, new_ch->mage_specialist >= 0 ? school_table[ URANGE( 0, new_ch->mage_specialist, 7 ) ].name : "mag ogólny" );

	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_NEWCHAR_MENU;
}

NANNY( nanny_newchar_align )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;
	char arg[ MAX_INPUT_LENGTH ];
	bool wrong = FALSE;
	int num = 0;

	argument = one_argument( argument, arg );

	if ( arg[0] != '\0' )
	{
		if ( is_number( arg ) )
		{
			num = atoi( arg );

			switch ( num )
			{
				case 1:
					if ( new_ch->class == CLASS_DRUID )
						new_ch->alignment = 0;
					else if ( new_ch->class == CLASS_PALADIN )
						new_ch->alignment = 1000;
					else if ( new_ch->class == CLASS_BLACK_KNIGHT )
						new_ch->alignment = -1000;
					else if ( school_table[ new_ch->mage_specialist ].flag == Nekromancja )
						new_ch->alignment = -1000;
					else
						new_ch->alignment = 1000;
					break;
				case 2:
					if ( new_ch->class == CLASS_DRUID || new_ch->class == CLASS_BLACK_KNIGHT || new_ch->class == CLASS_PALADIN || school_table[ new_ch->mage_specialist ].flag == Nekromancja )
					{
						wrong = TRUE;
						break;
					}
					new_ch->alignment = 0;
					break;
				case 3:
					if ( new_ch->class == CLASS_DRUID || new_ch->class == CLASS_BLACK_KNIGHT || new_ch->class == CLASS_PALADIN || school_table[ new_ch->mage_specialist ].flag == Nekromancja )
					{
						wrong = TRUE;
						break;
					}
					new_ch->alignment = -1000;
					break;
				default:
					wrong = TRUE;
					break;
			}
		}
		else
		{
			switch ( arg[0] )
			{
				case 'd':
				case 'D':
					if ( new_ch->class == CLASS_DRUID || new_ch->class == CLASS_BLACK_KNIGHT || school_table[ new_ch->mage_specialist ].flag == Nekromancja )
					{
						wrong = TRUE;
						break;
					}
					new_ch->alignment = 1000;
					break;
				case 'n':
				case 'N':
					if ( new_ch->class == CLASS_PALADIN || new_ch->class == CLASS_BLACK_KNIGHT || school_table[ new_ch->mage_specialist ].flag == Nekromancja )
					{
						wrong = TRUE;
						break;
					}
					new_ch->alignment = 0;
					break;
				case 'z':
				case 'Z':
					if ( new_ch->class == CLASS_PALADIN || new_ch->class == CLASS_DRUID )
					{
						wrong = TRUE;
						break;
					}
					new_ch->alignment = -1000;
					break;
				default:
					wrong = TRUE;
					break;
			}
		}
	}
	else wrong = TRUE;

	if ( wrong )
	{
		print_desc( d, TRUE, "\n\r%s  {RNieprawid³owy charakter.\n\r{x", d->spaces );
		show_align( d );
		return NANNY_NEWCHAR_ALIGN;
	}

	REMOVE_BIT( new_ch->creating, CREATE_ALIGN );
	print_desc( d, TRUE, "\n\r%s  {GTwój charakter: %s.{x\n\r", d->spaces, ALIGN_STR( new_ch ) );
	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_NEWCHAR_MENU;
}

NANNY( nanny_newchar_password )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;
	char buf[ MAX_STRING_LENGTH ];

	sprintf( buf, "%s  {R", d->spaces );

	if ( !check_password( d, buf, argument ) )
	{
		print_desc( d, TRUE, "\n\r%s{Y->{xWprowad¼ has³o do postaci:", d->spaces );
		return NANNY_NEWCHAR_PASSWORD;
	}

	free_string( new_ch->password );
	new_ch->password = str_dup( argument );

	print_desc( d, TRUE, "\n\r%s{Y->{xPotwierd¼ has³o do postaci:", d->spaces );

	return NANNY_NEWCHAR_PASSWORD_CONFIRM;
}

NANNY( nanny_newchar_password_confirm )
{
	NEW_CHAR_DATA * new_ch = d->new_char_data;

	if ( str_cmp( argument, new_ch->password ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RHas³a nie zgadzaj± siê.{x\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xWprowad¼ jeszcze raz has³o do postaci:", d->spaces );
		return NANNY_NEWCHAR_PASSWORD;
	}

	print_desc( d, TRUE, "\n\r%s  {GHas³o postaci ustawione.{x\n\r", d->spaces );
	send_to_desc( d, echo_on_str, FALSE );

	REMOVE_BIT( new_ch->creating, CREATE_PASSWORD );
	print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
	d->ignore_input = TRUE;
	return NANNY_NEWCHAR_MENU;
}

NANNY( nanny_newchar_accept )
{
	int result;

	switch( argument[0] )
	{
		case 't':
		case 'T':
			//TODO: stworzenie postaci
			result = create_char( d, TRUE );

			if ( result == 0 )
			{
				show_account_menu( d );
				return NANNY_ACCOUNT_MENU;
			}
			else
				return result;
			break;

		case 'n':
		case 'N':
			show_create_menu( d );
			return NANNY_NEWCHAR_MENU;

		default:
			print_desc( d, TRUE, "\n\r\n\r%s{Y->{xCzy chcesz zakoñczyæ tworzenie tej postaci (t/n)? ", d->spaces );
			return NANNY_NEWCHAR_ACCEPT;
	}

	return NANNY_NEWCHAR_MENU;
}

NANNY( nanny_newchar_resign )
{
	switch( argument[0] )
	{
		case 't':
		case 'T':
			free_new_char_data( d->new_char_data );
			d->new_char_data = NULL;
			show_account_menu( d );
			return NANNY_ACCOUNT_MENU;

		case 'n':
		case 'N':
			show_create_menu( d );
			return NANNY_NEWCHAR_MENU;

		default:
			print_desc( d, TRUE, "\n\r\n\r%s{Y->{xCzy na pewno chcesz zrezygnowaæ z tworzenia tej postaci (t/n)? ", d->spaces );
			return NANNY_NEWCHAR_RESIGN;
	}

	return NANNY_NEWCHAR_MENU;
}

void show_player_menu( DESCRIPTOR_DATA *d )
{
	CHAR_DATA * ch = d->remote_char;

	show_header( d, "MENU POSTACI", "=<>=", ch->name, 'Y', 'b', 'B' );

	if ( is_allowed( ch->name ) == NAME_DISALLOWED )
		print_desc( d, TRUE, "%s  {C%2d.{x Zmieñ niezaakceptowane imiê.\n\r", d->spaces, CHA_PLAY_OR_CHANGE_NAME );
	else
		print_desc( d, TRUE, "%s  {C%2d.{x Wejd¼ do gry.\n\r", d->spaces, CHA_PLAY_OR_CHANGE_NAME );

	print_desc( d, TRUE, "%s  {C%2d.{x Poka¿ dane postaci.\n\r\n\r", d->spaces, CHA_SCORE );
	print_desc( d, TRUE, "%s  {C%2d.{x Zamknij po³±czenie.\n\r", d->spaces, CHA_LOGOUT );
	print_desc( d, TRUE, "\n\r%s{Y->{xWybierz opcjê:", d->spaces );


	//TODO
	//aktywacja postaci
	//wejd¼ do gry
	//names deny
	//recconect?
	//przypomnij haslo
	//zmien haslo do postaci
	//pokaz postaæ (jak score w grze)
	//zmien przydomek postaci
	//zmien opis postaci
	//pokaz pozostale dni do renta
	//zgloszenie bledu w postaci
	//opcje dotyczace rozgrywki
	//podepnij postac pod konto
	return ;
}

NANNY( nanny_player_menu )
{
	CHAR_DATA * ch = d->remote_char;
	int option;

	if ( d->ignore_input )
	{
		show_player_menu( d );
		d->ignore_input = FALSE;
		return NANNY_PLAYER_MENU;
	}

	if ( !is_number( argument ) )
		option = -1;
	else
		option = atoi( argument );

	switch ( option )
	{
		case CHA_PLAY_OR_CHANGE_NAME:
			if ( is_allowed( ch->name ) == NAME_DISALLOWED )
			{
				print_desc( d, TRUE, "\n\r%sTwoje imiê zosta³o odrzucone. Powód odrzucenia imienia:\n\r", d->spaces );
				print_desc( d, TRUE, "\n\r%s  {R%s\n\r{x", d->spaces, ch->pcdata->name_deny_txt );
				print_desc( d, TRUE, "\n\r%s{Y->{xPodaj nowe imiê: {x", d->spaces );
				return NANNY_PLAYER_CHANGE_NAME;
			}
			else
			{
				//TODO: wej¶cie do gry
			}
		case CHA_LOGOUT:
			print_desc( d, TRUE, "%s{RDo zobaczenia!\n\r{x", d->spaces );
			close_socket( d, 0 );
			return NANNY_EXIT;

		default: show_player_menu( d ); break;
	}

	return NANNY_PLAYER_MENU;
}

NANNY( nanny_player_change_name )
{
	CHAR_DATA * ch = d->remote_char;
	char * reserv_pwd;
	int i;

	//scinanie duzych liter
	for ( i = 0; argument[ i ] != '\0'; i++ )
		argument[ i ] = LOWER( argument[ i ] );

	//pierwsza wielka
	argument[ 0 ] = UPPER( argument[ 0 ] );

	if ( !check_parse_name( argument ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RZabronione imiê.\n\r{x", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xPodaj inne imiê: {x", d->spaces );
		return NANNY_PLAYER_CHANGE_NAME;
	}

	if ( strlen( argument ) < 4 )
	{
		print_desc( d, TRUE, "\n\r%s  {RImiê musi mieæ co najmniej cztery litery.\n\r{x", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xPodaj inne imiê: {x", d->spaces );
		return NANNY_PLAYER_CHANGE_NAME;
	}

	if ( check_creating( d, argument ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RKto¶ tworzy postaæ o takim imieniu.\n\r{x", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xPodaj inne imiê: {x", d->spaces );
		return NANNY_PLAYER_CHANGE_NAME;
	}

	if ( player_exists( argument ) )
	{
		print_desc( d, TRUE, "\n\r%s  {RIstnieje ju¿ postaæ o takim imieniu.\n\r{x", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xPodaj inne imiê: {x", d->spaces );
		return NANNY_PLAYER_CHANGE_NAME;
	}

	if ( is_allowed( argument ) == NAME_DISALLOWED )
	{
		print_desc( d, TRUE, "\n\r%s  {RImiê nieakceptowalne.\n\r{x", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xPodaj inne imiê: {x", d->spaces );
		return NANNY_PLAYER_CHANGE_NAME;
	}

	if ( ( reserv_pwd = is_name_reserved( argument ) ) != NULL )
	{
		print_desc( d, TRUE, "\n\r%s  {RTo imiê jest zarezerwowane.\n\r{x", d->spaces );
		print_desc( d, TRUE, "\n\r%s{Y->{xPodaj has³o rezerwacji: {x", d->spaces );
		send_to_desc( d, echo_off_str, FALSE );
		d->pass_fail = 0;
		free_string( d->tmp_passwd );
		d->tmp_passwd = reserv_pwd;
		return NANNY_PLAYER_RESERVED_PWD;
	}

	free_string( ch->old_name );
	ch->old_name = str_dup( ch->name );
	free_string( ch->name );
	ch->name = str_dup( argument );

	free_string( ch->name2 );
	free_string( ch->name3 );
	free_string( ch->name4 );
	free_string( ch->name5 );
	free_string( ch->name6 );
	ch->name2 = str_dup( "null" );
	ch->name3 = str_dup( "null" );
	ch->name4 = str_dup( "null" );
	ch->name5 = str_dup( "null" );
	ch->name6 = str_dup( "null" );
	print_desc( d, TRUE, "\n\r%s  Wpisz odmianê swojego nowego imienia:", d->spaces );
	print_desc( d, TRUE, "\n\r%s  Mianownik (kto?): %s\n\r", d->spaces, ch->name );
	print_desc( d, TRUE, "\n\r%s{Y->{xDope³niacz (kogo nie ma?): ", d->spaces );
	return NANNY_PLAYER_INFLECT;
}

NANNY( nanny_player_reserved_pwd )
{
	CHAR_DATA * ch = d->remote_char;

	if ( strcmp( argument, d->tmp_passwd ) )
	{
		if ( d->pass_fail > 1 )
		{
			print_desc( d, TRUE, "\n\r%s  {RZ³e has³o.{x\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			d->pass_fail = 0;
			send_to_desc( d, echo_on_str, FALSE );
			return NANNY_PLAYER_MENU;
		}
		else
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xZ³e has³o. Spróbuj jeszcze raz:{x", d->spaces );
			d->pass_fail++;
			return NANNY_PLAYER_RESERVED_PWD;
		}
	}

	d->pass_fail = 0;
	send_to_desc( d, echo_on_str, FALSE );

	free_string( ch->old_name );
	ch->old_name = str_dup( ch->name );
	free_string( ch->name );
	ch->name = str_dup( argument );

	free_string( ch->name2 );
	free_string( ch->name3 );
	free_string( ch->name4 );
	free_string( ch->name5 );
	free_string( ch->name6 );
	ch->name2 = str_dup( "null" );
	ch->name3 = str_dup( "null" );
	ch->name4 = str_dup( "null" );
	ch->name5 = str_dup( "null" );
	ch->name6 = str_dup( "null" );
	print_desc( d, TRUE, "\n\r%s  Wpisz odmianê swojego nowego imienia:", d->spaces );
	print_desc( d, TRUE, "\n\r%s  Mianownik (kto?): %s\n\r", d->spaces, ch->name );
	print_desc( d, TRUE, "\n\r%s{Y->{xDope³niacz (kogo nie ma?): ", d->spaces );
	return NANNY_PLAYER_INFLECT;
}

NANNY( nanny_player_inflect )
{
	CHAR_DATA * ch = d->remote_char;
	char arg[ MAX_INPUT_LENGTH ];

	if ( !str_cmp( ch->name2, "null" ) )
	{
		one_argument( argument, arg );
		smash_tilde( arg );

		if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( ch->name[ 0 ] ) )
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xDope³niacz (kogo nie ma?): ", d->spaces );
			return NANNY_PLAYER_INFLECT;
		}

		arg[ 0 ] = UPPER( arg[ 0 ] );

		free_string( ch->name2 );
		ch->name2 = str_dup( arg );

		print_desc( d, TRUE, "\n\r%s{Y->{xCelownik (komu siê przygl±dam?): ", d->spaces );
		return NANNY_PLAYER_INFLECT;
	}

	if ( !str_cmp( ch->name3, "null" ) )
	{
		one_argument( argument, arg );
		smash_tilde( arg );

		if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( ch->name[ 0 ] ) )
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xCelownik (komu siê przygl±dam?): ", d->spaces );
			return NANNY_PLAYER_INFLECT;
		}

		arg[ 0 ] = UPPER( arg[ 0 ] );

		free_string( ch->name3 );
		ch->name3 = str_dup( arg );

		print_desc( d, TRUE, "\n\r%s{Y->{xBiernik (kogo widzê?): ", d->spaces );
		return NANNY_PLAYER_INFLECT;
	}

	if ( !str_cmp( ch->name4, "null" ) )
	{
		one_argument( argument, arg );
		smash_tilde( arg );

		if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( ch->name[ 0 ] ) )
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xBiernik (kogo widzê?): ", d->spaces );
			return NANNY_PLAYER_INFLECT;
		}

		arg[ 0 ] = UPPER( arg[ 0 ] );

		free_string( ch->name4 );
		ch->name4 = str_dup( arg );

		print_desc( d, TRUE, "\n\r%s{Y->{xNarzêdnik (z kim idê?): ", d->spaces );
		return NANNY_PLAYER_INFLECT;
	}

	if ( !str_cmp( ch->name5, "null" ) )
	{
		one_argument( argument, arg );
		smash_tilde( arg );

		if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( ch->name[ 0 ] ) )
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xNarzêdnik (z kim idê?): ", d->spaces );
			return NANNY_PLAYER_INFLECT;
		}

		arg[ 0 ] = UPPER( arg[ 0 ] );

		free_string( ch->name5 );
		ch->name5 = str_dup( arg );

		print_desc( d, TRUE, "\n\r%s{Y->{xMiejscownik (o kim mówiê?): ", d->spaces );
		return NANNY_PLAYER_INFLECT;
	}

	if ( !str_cmp( ch->name6, "null" ) )
	{
		one_argument( argument, arg );
		smash_tilde( arg );

		if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( ch->name[ 0 ] ) )
		{
			print_desc( d, TRUE, "\n\r%s{Y->{xMiejscownik (o kim mówiê?): ", d->spaces );
			return NANNY_PLAYER_INFLECT;
		}

		arg[ 0 ] = UPPER( arg[ 0 ] );

		free_string( ch->name6 );
		ch->name6 = str_dup( arg );


		print_desc( d, TRUE, "\n\r%s  Ustawiona odmiana:\n\r", d->spaces );
		print_desc( d, TRUE, "\n\r%s  Mianownik (kto?):                {G%s{x", d->spaces, ch->name );
		print_desc( d, TRUE, "\n\r%s  Dope³niacz (kogo nie ma?):       {G%s{x", d->spaces, ch->name2 );
		print_desc( d, TRUE, "\n\r%s  Celownik (komu siê przygl±dam?): {G%s{x", d->spaces, ch->name3 );
		print_desc( d, TRUE, "\n\r%s  Biernik (kogo widzê?):           {G%s{x", d->spaces, ch->name4 );
		print_desc( d, TRUE, "\n\r%s  Narzêdnik (z kim idê?):          {G%s{x", d->spaces, ch->name5 );
		print_desc( d, TRUE, "\n\r%s  Miejscownik (o kim mówiê?):      {G%s{x", d->spaces, ch->name6 );
		print_desc( d, TRUE, "\n\r\n\r%s{Y->{xCzy odmiana jest poprawna (t/n)? ", d->spaces );
		return NANNY_PLAYER_CONFIRM_INFLECT;
	}

	return NANNY_PLAYER_INFLECT;
}

NANNY( nanny_player_confirm_inflect )
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *ch = d->remote_char;

	switch( argument[0] )
	{
		case 't':
		case 'T':
			free_string(ch->pcdata->name_deny_txt);
			ch->pcdata->name_deny_txt = &str_empty[0];
			save_char_obj( ch, FALSE, TRUE );

			add_player_to_account( d->account, ch->name );
			remove_player_from_account( d->account, ch->old_name );

			sprintf( buf, "%s%s", PLAYER_DIR, capitalize( ch->old_name ) );
			unlink( buf );
			if ( ch->level >= 2 && is_allowed( ch->name ) == NAME_NOTEXIST )
			{
				add_name( ch->name, NAME_NEW );
				EXT_SET_BIT( ch->act, PLR_UNREGISTERED );
			}
			print_desc( d, TRUE, "\n\r%s{Y->{xNaci¶nij ENTER aby powróciæ do menu", d->spaces );
			d->ignore_input = TRUE;
			return NANNY_PLAYER_MENU;

		case 'n':
		case 'N':
			print_desc( d, TRUE, "\n\r%s  Wpisz odmianê swojego imienia:", d->spaces );
			print_desc( d, TRUE, "\n\r%s  Mianownik (kto?): %s\n\r", d->spaces, ch->name );
			print_desc( d, TRUE, "\n\r%s{Y->{xDope³niacz (kogo nie ma?): ", d->spaces );
			free_string( ch->name2 );
			free_string( ch->name3 );
			free_string( ch->name4 );
			free_string( ch->name5 );
			free_string( ch->name6 );
			ch->name2 = str_dup( "null" );
			ch->name3 = str_dup( "null" );
			ch->name4 = str_dup( "null" );
			ch->name5 = str_dup( "null" );
			ch->name6 = str_dup( "null" );
			return NANNY_PLAYER_INFLECT;

		default:
			print_desc( d, TRUE, "\n\r\n\r%s  Ustawiona odmiana:\n\r", d->spaces );
			print_desc( d, TRUE, "\n\r%s  Mianownik (kto?):                {G%s{x", d->spaces, ch->name );
			print_desc( d, TRUE, "\n\r%s  Dope³niacz (kogo nie ma?):       {G%s{x", d->spaces, ch->name2 );
			print_desc( d, TRUE, "\n\r%s  Celownik (komu siê przygl±dam?): {G%s{x", d->spaces, ch->name3 );
			print_desc( d, TRUE, "\n\r%s  Biernik (kogo widzê?):           {G%s{x", d->spaces, ch->name4 );
			print_desc( d, TRUE, "\n\r%s  Narzêdnik (z kim idê?):          {G%s{x", d->spaces, ch->name5 );
			print_desc( d, TRUE, "\n\r%s  Miejscownik (o kim mówiê?):      {G%s{x", d->spaces, ch->name6 );
			print_desc( d, TRUE, "\n\r\n\r%s{Y->{xCzy odmiana jest poprawna (t/n)? ", d->spaces );
			return NANNY_PLAYER_CONFIRM_INFLECT;
	}

	return NANNY_PLAYER_MENU;
}

/*
NANNY( nanny_player_enter )
{
	char *result;

	//deny
	if ( EXT_IS_SET( d->character->act, PLR_DENY ) )
	{
		sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
		log_string( log_buf );
		send_to_desc( d, "Postac ta ma zabroniony dostep do naszego swiata.\n\r\n\r"
		              "{RNacisnij Enter aby zamknac polaczenie z mudem.{x\n\r", TRUE );
		return NANNY_EXIT;
	}

	//zbanowany adres?
	if ( check_ban( d->host, BAN_PERMIT ) && !EXT_IS_SET( d->character->act, PLR_PERMIT ) )
	{
		send_to_desc( d, "Polaczenie z Twojego adresu zostaly zabronione.\n\r\n\r"
		              "{RNacisnij Enter aby zamknac polaczenie z mudem.{x\n\r", TRUE );
		return NANNY_EXIT;
	}

	//wizlock
	if ( wizlock && !IS_IMMORTAL( d->character ) )
	{
		send_to_desc( d, "Chwilowo mud jest zamkniêty dla graczy, zapraszamy za chwilê.\n\r\n\r"
		              "{RNacisnij Enter aby zamknac polaczenie z mudem.{x\n\r", TRUE );
		return NANNY_EXIT;
	}

	//sprawdzanie czy nie ma juz postaci z tego konta w grze
	if ( ( result = check_account_playing( d ) ) != NULL )
	{
		sprintf( buf, "Z mudem polaczona jest juz inna postac z tego konta: %s. Prosze najpierw sie\n\r"
		         "nia wylogowac. W przypadku zerwanego polaczenia nalezy polaczyc sie na nia\n\r"
		         "ponownie i wyjsc calkowicie z gry (rent lub quit, a nastepnie wyjscie z gry\n\r"
		         "z menu postaci). Zmuszenie do wylogowania postaci z tego konta mozliwe jest\n\r"
		         "takze z poziomu menu konta.\n\r\n\r"
		         "{RNacisnij Enter aby zamknac polaczenie z mudem.{x\n\r", result );
		send_to_desc( d, buf, TRUE );
		return NANNY_EXIT;
	}

	//artefact
	if ( !IS_IMMORTAL ( ch ) ) restore_char_on_artefact_list( ch );

	sprintf( log_buf, "%s %s@%s has connected.", new_ch->name, d->user, d->host );
	log_string( log_buf );
	wiznet( log_buf, NULL, NULL, WIZ_SITES, 0, 0 );
}
*/
#endif
