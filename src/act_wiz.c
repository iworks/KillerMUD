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
 * KILLER MUD is copyright 1999-2012 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Skrzetnicki Krzysztof (gtener@gmail.com              ) [Tener     ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: act_wiz.c 11364 2012-05-22 05:59:42Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/12.02/src/act_wiz.c $
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
#include <unistd.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "magic.h"
#include "lang.h"
#include "tables.h"
#include "progs.h"
#include "name_check.h"
#include "clans.h"
#include "telnet.h"
#include "music.h"
#include "shapeshifting.h"
#include "projects.h"
#include "money.h"

#define ADD_LINE_NUMBER(str_info) \
	do{ \
		sprintf((str_info), " (%s @ %d) ",__FILE__, __LINE__); \
	}while(0)

/*
* Local functions.
*/
ROOM_INDEX_DATA * find_location args( ( CHAR_DATA *ch, char *arg ) );
AREA_DATA * get_area_data args( ( ush_int vnum ) );
bool check_stat_range args( ( CHAR_DATA *ch, int value ) );

/*fstat, fset */
int get_langflag args( ( char *flag ) );
void mp_onload_trigger args( ( CHAR_DATA *mob ) );
void op_onload_trigger args( ( OBJ_DATA *obj ) );
int flag_lookup args( ( const char *name, const struct flag_type *flag_table ) );

/*
* do_copyover
*/
bool write_to_descriptor args( ( DESCRIPTOR_DATA *d, char *txt, int length ) );
bool write_to_descriptor_2 args( ( int desc, char *txt, int length ) );
int flag_value args( ( const struct flag_type *flag_table, char *argument ) );
void    drop_objects    args( (CHAR_DATA *ch, OBJ_DATA *object, bool auto_rent) );
bool 	check_rent	args ( (CHAR_DATA *ch, OBJ_DATA *object, bool show));

/*z artefact.c*/

void awhere args((CHAR_DATA *ch, char *arg));

/* olc_act.c */
void specdam_to_char args ( ( CHAR_DATA *ch, int count, SPEC_DAMAGE *specdam ) );

/* handler.c */
int get_curr_stat args ( ( CHAR_DATA *ch, int stat ) );

#ifdef MCCP
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "telnet.h"
const char eor_on_str4 [] =
{
	IAC, WILL, TELOPT_EOR, '\0'
};
const char compress_on_str4 [] =
{
	IAC, WILL, TELOPT_COMPRESS, '\0'
};
const char compress2_on_str4 [] =
{
	IAC, WILL, TELOPT_COMPRESS2, '\0'
};
#endif

void do_wizkomendy( CHAR_DATA *ch, char *argument )
{
	if ( IS_NPC( ch ) )
    {
		return;
    }

    /**
     * Gurthg: bardzo prosze koderow o nie umieszczanie
     * tutaj swoich nickow
     */
	if (
            FALSE
            || !str_cmp( ch->name, "Agron"  )
            || !str_cmp( ch->name, "Furgas" )
            || !str_cmp( ch->name, "Gurthg" )
       )
	{
		SET_BIT( ch->pcdata->wiz_conf, W1 );
		SET_BIT( ch->pcdata->wiz_conf, W2 );
		SET_BIT( ch->pcdata->wiz_conf, W3 );
		SET_BIT( ch->pcdata->wiz_conf, W4 );
        SET_BIT( ch->pcdata->wiz_conf, W5 );
        SET_BIT( ch->pcdata->wiz_conf, W6 );
        ch->pcdata->security = 9;
		send_to_char( "Ok.\n\r", ch );
		ch->level = 40;
		return;
	}
	else if ( ch->level > LEVEL_HERO )
	{
		SET_BIT( ch->pcdata->wiz_conf, W1 );
		send_to_char( "Ok.\n\r", ch );
		return;
	}
	else
    {
		send_to_char( "Huh?\n\r", ch );
    }
	return;
}


void do_wizstat( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * vch;
	char buf[ MSL ];

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "wizstat <imiê>\n\r", ch );
		return;
	}

	if ( ( vch = get_char_world( ch, argument ) ) == NULL )
	{
		send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
		return;
	}

	if ( IS_NPC( vch ) )
		return;


    sprintf
        (
         buf, "%s nale¿y do grup:\n\r%s%s%s%s%s%s\n\r",
         vch->name,
         IS_SET( vch->pcdata->wiz_conf, W1 ) ? "NOWY " : "",
         IS_SET( vch->pcdata->wiz_conf, W2 ) ? "TWÓRCA " : "",
         IS_SET( vch->pcdata->wiz_conf, W3 ) ? "STRA¯NIK " : "",
         IS_SET( vch->pcdata->wiz_conf, W4 ) ? "REGENT " : "",
         IS_SET( vch->pcdata->wiz_conf, W5 ) ? "SÊDZIA " : "",
         IS_SET( vch->pcdata->wiz_conf, W6 ) ? "LORD" : ""
        );
	send_to_char( buf, ch );
	return;
}


void do_wizset( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * vch;
	char arg1[ MIL ];
	char arg2[ MIL ];

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "wizset <imiê> <grupa>\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( ( vch = get_char_world( ch, arg1 ) ) == NULL )
	{
		send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
		return;
	}

	if ( IS_NPC( vch ) )
    {
		return;
    }

    if ( IS_NPC( ch ) && ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 ))
    {
        return;
    }

	if ( !str_prefix( arg2, "STANDARD" ) || !str_prefix( arg2, "standard" ) || !str_prefix( arg2, "NOWA" ) || !str_prefix( arg2, "NOWY" ) || !str_prefix( arg2, "nowa" ) || !str_prefix( arg2, "nowy" ) )
	{
		if ( IS_SET( vch->pcdata->wiz_conf, W1 ) )
		{
			vch->pcdata->wiz_conf = 0;
			send_to_char( "Komendy odebrane.\n\r", ch );
			send_to_char( "Zosta³<&e/a/o>¶ pozbawion<&y/a/e> wiz komend.\n\r", vch );
			return;
		}
		SET_BIT( vch->pcdata->wiz_conf, W1 );
		send_to_char( "Grupa NOWY przyznana.\n\r", ch );
		send_to_char( "Dosta³<&e/a/o>¶ komendy z grupy {RNOWY{x.\n\r", vch );
		return;
	}
	else
		if ( !str_prefix( arg2, "BUILDER" ) || !str_prefix( arg2, "builder" ) || !str_prefix( arg2, "twórca" ) || !str_prefix( arg2, "TWÓRCA" ) )
		{
			if ( IS_SET( vch->pcdata->wiz_conf, W2 ) )
			{
				vch->pcdata->wiz_conf = 0;
				send_to_char( "Komendy odebrane.\n\r", ch );
				send_to_char( "Zosta³<&e/a/o>¶ pozbawion<&y/a/e> wiz komend.\n\r", vch );
				return;
			}
			SET_BIT( vch->pcdata->wiz_conf, W1 );
			SET_BIT( vch->pcdata->wiz_conf, W2 );
			send_to_char( "Grupa TWÓRCA przyznana.\n\r", ch );
			send_to_char( "Dosta³<&e/a/o>¶ komendy z grupy {RTWÓRCA{x.\n\r", vch );
			return;
		}
		else
			if ( !str_prefix( arg2, "GUARD" ) || !str_prefix( arg2, "guard" ) || !str_prefix( arg2, "stra¿nik" ) || !str_prefix( arg2, "STRA¯NIK" ) )
			{
				if ( IS_SET( vch->pcdata->wiz_conf, W3 ) )
				{
					vch->pcdata->wiz_conf = 0;
					send_to_char( "Komendy odebrane.\n\r", ch );
					send_to_char( "Zosta³<&e/a/o>¶ pozbawiony komend wiz.\n\r", vch );
					return;
				}
				SET_BIT( vch->pcdata->wiz_conf, W1 );
				SET_BIT( vch->pcdata->wiz_conf, W2 );
				SET_BIT( vch->pcdata->wiz_conf, W3 );
				send_to_char( "Grupa STRA¯NIK przyznana.\n\r", ch );
				send_to_char( "Dosta³<&e/a/o>¶ komendy z grupy {RSTRA¯NIK{x.\n\r", vch );
				return;
			}
			else
				if ( !str_prefix( arg2, "REGENT" ) || !str_prefix( arg2, "regent" ) )
				{
					if ( IS_SET( vch->pcdata->wiz_conf, W4 ) )
					{
						vch->pcdata->wiz_conf = 0;
						send_to_char( "Komendy odebrane.\n\r", ch );
						send_to_char( "Zosta³<&e/a/o>¶ pozbawiony komend wiz.\n\r", vch );
						return;
					}
					SET_BIT( vch->pcdata->wiz_conf, W1 );
					SET_BIT( vch->pcdata->wiz_conf, W2 );
					SET_BIT( vch->pcdata->wiz_conf, W3 );
					SET_BIT( vch->pcdata->wiz_conf, W4 );
					send_to_char( "Grupa REGENT przyznana.\n\r", ch );
					send_to_char( "Dosta³<&e/a/o>¶ komendy z grupy {RREGENT{x.\n\r", vch );

					return;
				}
				else
					if ( !str_prefix( arg2, "JUDGE" ) || !str_prefix( arg2, "judge" ) || !str_prefix( arg2, "sêdzia" ) || !str_prefix( arg2, "SÊDZIA" ) )
					{
						if ( IS_SET( vch->pcdata->wiz_conf, W5 ) )
						{
							vch->pcdata->wiz_conf = 0;
							send_to_char( "Komendy odebrane.\n\r", ch );
							send_to_char( "Zosta³<&e/a/o>¶ pozbawiony komend wiz.\n\r", vch );
							return;
						}
						SET_BIT( vch->pcdata->wiz_conf, W1 );
						SET_BIT( vch->pcdata->wiz_conf, W2 );
						SET_BIT( vch->pcdata->wiz_conf, W3 );
						SET_BIT( vch->pcdata->wiz_conf, W4 );
						SET_BIT( vch->pcdata->wiz_conf, W5 );
						send_to_char( "Grupa SÊDZIA przyznana.\n\r", ch );
						send_to_char( "Dosta³<&e/a/o>¶ komendy z grupy {RSÊDZIA{x.\n\r", vch );
						return;
					}
					else
						if ( !str_prefix( arg2, "ADMIN" ) || !str_prefix( arg2, "admin" ) || !str_prefix( arg2, "lord" ) || !str_prefix( arg2, "LORD" ))
						{
							if ( IS_SET( vch->pcdata->wiz_conf, W6 ) )
							{
								vch->pcdata->wiz_conf = 0;
								send_to_char( "Komendy odebrane.\n\r", ch );
								send_to_char( "Zosta³<&e/a/o>¶ pozbawiony komend wiz.\n\r", vch );
								return;
							}
							SET_BIT( vch->pcdata->wiz_conf, W1 );
							SET_BIT( vch->pcdata->wiz_conf, W2 );
							SET_BIT( vch->pcdata->wiz_conf, W3 );
							SET_BIT( vch->pcdata->wiz_conf, W4 );
							SET_BIT( vch->pcdata->wiz_conf, W5 );
							SET_BIT( vch->pcdata->wiz_conf, W6 );
							send_to_char( "Grupa LORD przyznana.\n\r", ch );
							send_to_char( "Dosta³<&e/a/o>¶ komendy z grupy {RLORD{x.\n\r", vch );

							return;
						}
						else
						{
							send_to_char( "Dostêpne s± nastêpuj±ce grupy:\n\r", ch );
							send_to_char( "{RNOWY TWÓRCA STRA¯NIK REGENT SÊDZIA LORD{x\n\r", ch );
							/*send_to_char( " : immtalk names prefix secret\n\r\n\r", ch );
							send_to_char( "{RTWÓRCAx{x\n\r", ch );
							send_to_char( " alist goto holylight incognito poofin poofout vnum wizhelp wiznet\n\r\n\r", ch );
							send_to_char( "{RTWÓRCA{x:  (STANDARD+GO¦Æ)\n\r", ch );
							send_to_char( " advance aedit asave board changes clone edit fset fstat hedit idea imotd\n\r", ch );
							send_to_char( " load medit mpdump mpedit mpstat mwhere oedit opdump opedit opstat otype\n\r", ch );
							send_to_char( " owear owhere peace purge rdedit redit resets restore return rlookup\n\r", ch );
							send_to_char( " rpdump rpedit rpstat stat switch tedit transfer violate \n\r\n\r", ch );
							send_to_char( "{RSTRA¯NIK{x:    (STANDARD+GO¦Æ+TWÓRCA)\n\r", ch );
							send_to_char( " at disconnect echo force freeze gecho invis lstat pecho slay smote snoop\n\r", ch );
							send_to_char( " sockets string trophy wizinvis zecho\n\r\n\r", ch );
							send_to_char( "{RSÊDZIA{x:    (STANDARD+GO¦Æ+TWÓRCA+STRA¯NIK)\n\r", ch );
							send_to_char( " allow ban cstat deny moblookup nochannels noemote noshout notell\n\r", ch );
						send_to_char( " objlookup permban prewait protect rename roomlookup set setclan trust\n\r\n\r", ch );
							send_to_char( "{RLORD{x:    (STANDARD+GO¦Æ+TWÓRCA+STRA¯NIK+SÊDZIA)\n\r", ch );
							send_to_char( " amod aset astat copyover dump flag log lset memory newlock note reboo\n\r", ch );
							send_to_char( " reboot rmap shutdow shutdown test timeset wizlock wizset wizstat\n\r", ch );*/
							return;
						}

	return;
}



void do_wiznet( CHAR_DATA *ch, char *argument )
{
	int flag;
	char buf[ MAX_STRING_LENGTH ];

	if ( argument[ 0 ] == '\0' || !str_prefix( argument, "wiznet" ) )
	{
		if ( IS_SET( ch->wiznet, WIZ_ON ) )
		{
			send_to_char( "Wiznet zosta³ {Rwy³±czony{x.\n\r", ch );
			REMOVE_BIT( ch->wiznet, WIZ_ON );
		}
		else
		{
			send_to_char( "Wiznet zosta³ {Gw³±czony{x!\n\r", ch );
			SET_BIT( ch->wiznet, WIZ_ON );
		}
		return;
	}

	if ( !str_prefix( argument, "on" ) )
	{
		send_to_char( "Wiznet zosta³ {Gw³±czony{x!\n\r", ch );
		SET_BIT( ch->wiznet, WIZ_ON );
		return;
	}

	if ( !str_prefix( argument, "off" ) )
	{
		send_to_char( "Wiznet zosta³ {Rwy³±czony{x.\n\r", ch );
		REMOVE_BIT( ch->wiznet, WIZ_ON );
		return;
	}

	/* Zmodyfikowalem wiznet - wywalilem dwie pokrewne komendy status i show - polaczylem je w jedna, bardziej przejrzysta.
	   Stary wiznet nadal tu jest, skomentarzowany. Jezeli ktos chce, to mozna odzyskac :)
	   - Loop
	*/


	if ( !str_prefix( argument, "show" ) || !str_prefix( argument, "all" ) || !str_prefix( argument, "status" ))
	{
		send_to_char( "{b>==<>==<>==< {GWszystkie opcje wiznetu: {b>==<>==<>==<{x\n\r\n\r", ch );

		if (IS_SET( ch->wiznet, WIZ_ON ))
		 send_to_char( "Wiznet      [{Gw³±czony{x]\n\r", ch );
		else
		 send_to_char( "Wiznet      [{rwy³±czony{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_ARTEFACT ))
		 send_to_char( "Artefact    [{GV{x]\n\r", ch );
		else
		 send_to_char( "Artefact    [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_ARTEFACTLOAD ))
		 send_to_char( "Artefactload[{GV{x]\n\r", ch );
		else
		 send_to_char( "Artefactload[{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_COMM ))
		 send_to_char( "Comm        [{GV{x]\n\r", ch );
		else
		 send_to_char( "Comm        [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_DEATHS ))
		 send_to_char( "Deaths      [{GV{x]\n\r", ch );
		else
		 send_to_char( "Deaths      [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_FLAGS ))
		 send_to_char( "Flags       [{GV{x]\n\r", ch );
		else
		 send_to_char( "Flags       [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_LEVELS ))
		 send_to_char( "Levels      [{GV{x]\n\r", ch );
		else
		 send_to_char( "Levels      [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_LINKS ))
		 send_to_char( "Links       [{GV{x]\n\r", ch );
		else
		 send_to_char( "Links       [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_LOAD ))
		 send_to_char( "Load        [{GV{x]\n\r", ch );
		else
		 send_to_char( "Load        [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_LOGINS ))
		 send_to_char( "Logins      [{GV{x]\n\r", ch );
		else
		 send_to_char( "Logins      [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_MOBDEATHS ))
		 send_to_char( "Mobdeaths   [{GV{x]\n\r", ch );
		else
		 send_to_char( "Mobdeaths   [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_MXP ))
		 send_to_char( "MXP         [{GV{x]\n\r", ch );
		else
		 send_to_char( "MXP         [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_NEWBIE ))
		 send_to_char( "Newbies     [{GV{x]\n\r", ch );
		else
		 send_to_char( "Newbies     [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_PENALTIES ))
		 send_to_char( "Penalties   [{GV{x]\n\r", ch );
		else
		 send_to_char( "Penalties   [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_PREFIX ))
		 send_to_char( "Prefix      [{GV{x]\n\r", ch );
		else
		 send_to_char( "Prefix      [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_RESETS ))
		 send_to_char( "Resets      [{GV{x]\n\r", ch );
		else
		 send_to_char( "Resets      [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_RESTORE ))
		 send_to_char( "Restore     [{GV{x]\n\r", ch );
		else
		 send_to_char( "Restore     [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_SACCING ))
		 send_to_char( "Saccing     [{GV{x]\n\r", ch );
		else
		 send_to_char( "Saccing     [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_SECURE ))
		 send_to_char( "Secure      [{GV{x]\n\r", ch );
		else
		 send_to_char( "Secure      [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_SITES ))
		 send_to_char( "Sites       [{GV{x]\n\r", ch );
		else
		 send_to_char( "Sites       [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_SNOOPS ))
		 send_to_char( "Snoops      [{GV{x]\n\r", ch );
		else
		 send_to_char( "Snoops      [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_SPAM ))
		 send_to_char( "Spam        [{GV{x]\n\r", ch );
		else
		 send_to_char( "Spam        [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_SWITCHES ))
		 send_to_char( "Switches    [{GV{x]\n\r", ch );
		else
		 send_to_char( "Switches    [{r-{x]\n\r", ch );

		 if (IS_SET( ch->wiznet, WIZ_TICKS ))
		 send_to_char( "Ticks       [{GV{x]\n\r", ch );
		else
		 send_to_char( "Ticks       [{r-{x]\n\r", ch );

		 send_to_char( "\n\r{b>===<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>===<\n\r{x", ch );

		return;
	}

	flag = wiznet_lookup( argument );

	if ( flag == -1 || get_trust( ch ) < wiznet_table[ flag ].level )
	{
		send_to_char( "Brak takiej opcji wiznetu.\n\r", ch );
		return;
	}

	if ( IS_SET( ch->wiznet, wiznet_table[ flag ].flag ) )
	{
		sprintf( buf, "Opcja wiznetu: {R%s{x zosta³a {Rwy³±czona{x.\n\r",
				 wiznet_table[ flag ].name );
		send_to_char( buf, ch );
		REMOVE_BIT( ch->wiznet, wiznet_table[ flag ].flag );
		return;
	}
	else
	{
		sprintf( buf, "Opcja wiznetu: {G%s{x zosta³a {Gw³±czona{x.\n\r",
				 wiznet_table[ flag ].name );
		send_to_char( buf, ch );
		SET_BIT( ch->wiznet, wiznet_table[ flag ].flag );
		return;
	}

}

void wiznet( char *string, CHAR_DATA *ch, OBJ_DATA *obj,
			 long flag, long flag_skip, int min_level )
{
	DESCRIPTOR_DATA * d;

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( !d->character ) continue;

		if ( d->connected == CON_PLAYING
			 && IS_IMMORTAL( d->character )
			 && IS_SET( d->character->wiznet, WIZ_ON )
			 && ( !flag || IS_SET( d->character->wiznet, flag ) )
			 && ( !flag_skip || !IS_SET( d->character->wiznet, flag_skip ) )
			 && get_trust( d->character ) >= min_level
			 && d->character != ch )
		{
			if ( IS_SET( d->character->wiznet, WIZ_PREFIX ) )
				send_to_char( "{Z--> ", d->character );
			else
				send_to_char( "{Z", d->character );
			if ( *string == '-' )
			{
				send_to_char( string+1, d->character );
				send_to_char( "\n\r", d->character );
			}
			else
				act_new( string, d->character, obj, ch, TO_CHAR, POS_DEAD );
			send_to_char( "{x", d->character );
		}
	}

	return;
}

/* equips a character */


/* RT nochannels command, for those spammers */
void do_nochannels( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ], buf[ MAX_STRING_LENGTH ];
	CHAR_DATA *victim;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Nochannel whom?", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie ma go tutaj.\n\r", ch );
		return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
		send_to_char( "Nie uda³o ci sie...\n\r", ch );
		return;
	}

	if ( IS_SET( victim->comm, COMM_NOCHANNELS ) )
	{
		REMOVE_BIT( victim->comm, COMM_NOCHANNELS );
		send_to_char( "The gods have restored your channel priviliges.\n\r",
					  victim );
		send_to_char( "NOCHANNELS removed.\n\r", ch );
		sprintf( buf, "$N restores channels to %s", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}
	else
	{
		SET_BIT( victim->comm, COMM_NOCHANNELS );
		send_to_char( "The gods have revoked your channel priviliges.\n\r",
					  victim );
		send_to_char( "NOCHANNELS set.\n\r", ch );
		sprintf( buf, "$N revokes %s's channels.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	return;
}


void do_smote( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * vch;
	char *letter, *name;
	char last[ MAX_INPUT_LENGTH ], temp[ MAX_STRING_LENGTH ];
	int matches = 0;

	if ( !IS_NPC( ch ) && IS_SET( ch->comm, COMM_NOEMOTE ) )
	{
		send_to_char( "You can't show your emotions.\n\r", ch );
		return;
	}

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Emote what?\n\r", ch );
		return;
	}

	if ( strstr( argument, ch->name ) == NULL )
	{
		send_to_char( "You must include your name in an smote.\n\r", ch );
		return;
	}

	send_to_char( argument, ch );
	send_to_char( "\n\r", ch );

	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
	{
		if ( vch->desc == NULL || vch == ch )
			continue;

		if ( ( letter = strstr( argument, vch->name ) ) == NULL )
		{
			send_to_char( argument, vch );
			send_to_char( "\n\r", vch );
			continue;
		}

		strcpy( temp, argument );
		temp[ strlen( argument ) - strlen( letter ) ] = '\0';
		last[ 0 ] = '\0';
		name = vch->name;

		for ( ; *letter != '\0'; letter++ )
		{
			if ( *letter == '\'' && matches == strlen( vch->name ) )
			{
				strcat( temp, "r" );
				continue;
			}

			if ( *letter == 's' && matches == strlen( vch->name ) )
			{
				matches = 0;
				continue;
			}

			if ( matches == strlen( vch->name ) )
			{
				matches = 0;
			}

			if ( *letter == *name )
			{
				matches++;
				name++;
				if ( matches == strlen( vch->name ) )
				{
					strcat( temp, "you" );
					last[ 0 ] = '\0';
					name = vch->name;
					continue;
				}
				strncat( last, letter, 1 );
				continue;
			}

			matches = 0;
			strcat( temp, last );
			strncat( temp, letter, 1 );
			last[ 0 ] = '\0';
			name = vch->name;
		}

		send_to_char( temp, vch );
		send_to_char( "\n\r", vch );
	}

	return;
}

void do_bamfin( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];

	if ( !IS_NPC( ch ) )
	{
		smash_tilde( argument );

		if ( argument[ 0 ] == '\0' )
		{
			sprintf( buf, "Twój poofin to %s\n\r", ch->pcdata->bamfin );
			send_to_char( buf, ch );
			return;
		}

		if ( strstr( argument, ch->name ) == NULL )
		{
			send_to_char( "Musisz zawrzeæ swój nick.\n\r", ch );
			return;
		}

		free_string( ch->pcdata->bamfin );
		ch->pcdata->bamfin = str_dup( argument );

		sprintf( buf, "Twój poofin zosta³ zmieniony na %s\n\r", ch->pcdata->bamfin );
		send_to_char( buf, ch );
	}
	return;
}

void do_bamfout( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];

	if ( !IS_NPC( ch ) )
	{
		smash_tilde( argument );

		if ( argument[ 0 ] == '\0' )
		{
			sprintf( buf, "Twój poofout to %s\n\r", ch->pcdata->bamfout );
			send_to_char( buf, ch );
			return;
		}

		if ( strstr( argument, ch->name ) == NULL )
		{
			send_to_char( "Musisz zawrzeæ swój nick.\n\r", ch );
			return;
		}

		free_string( ch->pcdata->bamfout );
		ch->pcdata->bamfout = str_dup( argument );

		sprintf( buf, "Twój poofout zosta³ zmieniony na %s\n\r", ch->pcdata->bamfout );
		send_to_char( buf, ch );
	}
	return;
}

void do_deny( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ], buf[ MAX_STRING_LENGTH ];
	CHAR_DATA *victim;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Deny whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC( victim ) )
	{
		send_to_char( "Bo co ty? Mobowi chcesz deny daæ?\n\r", ch );
		return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
		send_to_char( "Nie udalo ci sie.\n\r", ch );
		return;
	}

	EXT_SET_BIT( victim->act, PLR_DENY );
	send_to_char( "Dosta³<&e/a/o>¶ {Rczerwon± kartkê{x, opu¶æ boisko!\n\r", victim );
	sprintf( buf, "$N da³ czerwon± kartkê %s", victim->name3 );
	wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	send_to_char( "OK.\n\r", ch );
	save_char_obj( victim, FALSE, FALSE );
	stop_fighting( victim, TRUE );
	do_function( victim, &do_quit, "" );
	return;
}

void do_disconnect( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;

	one_argument( argument, arg );
	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Disconnect whom?\n\r", ch );
		return;
	}

	if ( is_number( arg ) )
	{
		int desc;

		desc = atoi( arg );
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( !d->character || d->connected < 0 ) continue;

			if ( d->descriptor == desc )
			{
			  if ( ( get_trust( d->character ) >= get_trust( ch ) ) )
			    {
			      send_to_char( "Nic z tego.\n\r", ch );
			    }
			  else
			    {
			      close_socket( d, CS_NORECURSE );
			      send_to_char( "Ok.\n\r", ch );
			    }
			  return;
			}
		}
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim->desc == NULL )
	{
		act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
		return;
	}

	for ( d = descriptor_list; d != NULL; d = d->next )
	{
		if ( !d->character || d->connected < 0 ) continue;

		if ( d == victim->desc )
		{
		  if ( ( get_trust( d->character ) >= get_trust( ch ) ) )
		    {
		      send_to_char( "Nic z tego.\n\r", ch );
		    }
		  else
		    {
		      close_socket( d, CS_NORECURSE );
		      send_to_char( "Ok.\n\r", ch );
		    }
		  return;
		}
	}

	bug( "Do_disconnect: desc not found.", 0 );
	send_to_char( "Descriptor not found!\n\r", ch );
	return;
}

void do_echo( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA * d;

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Global echo what?\n\r", ch );
		return;
	}

	for ( d = descriptor_list; d; d = d->next )
	{
		if ( !d->character ) continue;

		if ( d->connected == CON_PLAYING )
		{
			if ( get_trust( d->character ) >= get_trust( ch ) )
				send_to_char( "global> ", d->character );
			send_to_char( argument, d->character );
			send_to_char( "\n\r", d->character );
		}
	}

	return;
}

void do_recho( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA * d;

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Local echo what?\n\r", ch );

		return;
	}

	for ( d = descriptor_list; d; d = d->next )
	{
		if ( !d->character || d->connected < 0 ) continue;
		if ( d->connected == CON_PLAYING
			 && d->character->in_room == ch->in_room )
		{
			if ( get_trust( d->character ) >= get_trust( ch ) )
				send_to_char( "local> ", d->character );
			send_to_char( argument, d->character );
			send_to_char( "\n\r", d->character );
		}
	}

	return;
}

void do_zecho( CHAR_DATA *ch, char *argument )
{
	DESCRIPTOR_DATA * d;

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Zone echo what?\n\r", ch );
		return;
	}

	for ( d = descriptor_list; d; d = d->next )
	{
		if ( !d->character ) continue;
		if ( d->connected == CON_PLAYING
			 && d->character->in_room != NULL && ch->in_room != NULL
			 && SAME_AREA( d->character->in_room->area, ch->in_room->area )
			 && SAME_AREA_PART( d->character, ch )
			 && !EXT_IS_SET( d->character->in_room->room_flags, ROOM_NOZECHO ) )
		{
			if ( get_trust( d->character ) >= get_trust( ch ) )
				send_to_char( "zone> ", d->character );
			send_to_char( argument, d->character );
			send_to_char( "\n\r", d->character );
		}
	}
}

void do_pecho( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	argument = one_argument( argument, arg );

	if ( argument[ 0 ] == '\0' || arg[ 0 ] == '\0' )
	{
		send_to_char( "Personal echo what?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "Target not found.\n\r", ch );
		return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) && get_trust( ch ) != MAX_LEVEL )
		send_to_char( "personal> ", victim );

	send_to_char( argument, victim );
	send_to_char( "\n\r", victim );
	send_to_char( "personal> ", ch );
	send_to_char( argument, ch );
	send_to_char( "\n\r", ch );
}

ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj;

	if ( is_number( arg ) )
		return get_room_index( atoi( arg ) );

	if ( ( victim = get_char_world( ch, arg ) ) != NULL )
		return victim->in_room;

	if ( ( obj = get_obj_world( ch, arg ) ) != NULL )
		return obj->in_room;

	return NULL;
}

void do_transfer( CHAR_DATA *ch, char *argument )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	ROOM_INDEX_DATA *location;
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[ 0 ] == '\0' )
	{
		send_to_char( "Transfer whom (and where)?\n\r", ch );
		return;
	}

	if ( !str_cmp( arg1, "all" ) )
	{
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( !d->character ) continue;

			if ( d->connected == CON_PLAYING
				 && d->character != ch
				 && d->character->in_room != NULL
				 && can_see( ch, d->character ) )
			{
				char buf[ MAX_STRING_LENGTH ];
				sprintf( buf, "%s %s", d->character->name, arg2 );
				do_function( ch, &do_transfer, buf );
			}
		}
		return;
	}

	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( victim->in_room == NULL )
	{
		send_to_char( "They are in limbo.\n\r", ch );
		return;
	}

	if ( arg2[ 0 ] == '\0' )
	{
		location = ch->in_room;
	}
	else if ( victim->previous_room && !str_prefix( arg2, "back" ) )
	{
		location = victim->previous_room;
	}
	else
	{
		if ( ( location = find_location( ch, arg2 ) ) == NULL )
		{
			send_to_char( "No such location.\n\r", ch );
			return;
		}

		if ( !is_room_owner( ch, location ) && room_is_private( location )
			 && get_trust( ch ) < MAX_LEVEL )
		{
			send_to_char( "That room is private right now.\n\r", ch );
			return;
		}
	}

	if ( victim->fighting != NULL )
		stop_fighting( victim, TRUE );
	act( "$n znika.", victim, NULL, NULL, TO_ROOM );
	char_from_room( victim );
	char_to_room( victim, location );
	act( "$n pojawia siê znik±d.", victim, NULL, NULL, TO_ROOM );
	if ( ch != victim )
		switch ( ch->sex )
		{
			case 0:
				act( "$n przeniós³o ciê.", ch, NULL, victim, TO_VICT );
				break;
			case 1:
				act( "$n przeniós³ ciê.", ch, NULL, victim, TO_VICT );
				break;
			default :
				act( "$n przeniós³a ciê.", ch, NULL, victim, TO_VICT );
				break;
		}
	do_function( victim, &do_look, "auto" );
	send_to_char( "Ok.\n\r", ch );
}

void do_at( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	ROOM_INDEX_DATA *location;
	ROOM_INDEX_DATA *original;
	OBJ_DATA *on;
	CHAR_DATA *wch;

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		send_to_char( "At where what?\n\r", ch );
		return;
	}

	if ( ( location = find_location( ch, arg ) ) == NULL )
	{
		send_to_char( "No such location.\n\r", ch );
		return;
	}

	if ( !is_room_owner( ch, location ) && room_is_private( location )
		 && get_trust( ch ) < MAX_LEVEL )
	{
		send_to_char( "That room is private right now.\n\r", ch );
		return;
	}

	original = ch->in_room;
	on = ch->on;
	char_from_room( ch );
	char_to_room( ch, location );
	interpret( ch, argument );

	/*
		* See if 'ch' still exists before continuing!
		* Handles 'at XXXX quit' case.
		*/
	for ( wch = char_list; wch != NULL; wch = wch->next )
	{
		if ( wch == ch )
		{
			char_from_room( ch );
			char_to_room( ch, original );
			ch->on = on;
			break;
		}
	}

	return;
}

void do_goto( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	ROOM_INDEX_DATA *location;
	CHAR_DATA *rch;
	AREA_DATA *pArea;
	int count = 0;
	int room_vnum;

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Goto where?\n\r", ch );
		return;
	}

	one_argument( argument, arg );

	if ( !str_cmp( arg, "area" ) )
	{
		argument = one_argument( argument, arg );

		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Podaj numer krainy lub czê¶æ jej nazwy.\n\r", ch );
			return;
		}

		if ( is_number( argument ) )
		{
			if ( !( pArea = get_area_data( atoi( argument ) ) ) )
			{
				send_to_char( "Nie ma krainy o takim numerze.\n\r", ch );
				return;
			}
		}
		else
		{
			for ( pArea = area_first; pArea; pArea = pArea->next )
				if ( !str_infix( argument, pArea->name ) ) break;

			if ( !pArea )
			{
				send_to_char( "Nie ma krainy o nazwie zawieraj±cej ten tekst.\n\r", ch );
				return;
			}
		}

		for ( room_vnum = pArea->min_vnum; room_vnum <= pArea->max_vnum; room_vnum++ )
			if ( ( location = get_room_index( room_vnum ) ) != NULL ) break;

		if ( !location )
		{
			send_to_char( "W tej krainie nie ma ¿adnych lokacji.\n\r", ch );
			return;
		}
	}
	else
	{
		if ( ch->previous_room && !str_prefix( arg, "back" ) )
			location = ch->previous_room;
		else
		if ( ( location = find_location( ch, argument ) ) == NULL )
		{
			send_to_char( "No such location.\n\r", ch );
			return;
		}
	}

	count = 0;
	for ( rch = location->people; rch != NULL; rch = rch->next_in_room )
		count++;

	if ( !is_room_owner( ch, location ) && room_is_private( location )
		 && ( count > 1 || get_trust( ch ) < MAX_LEVEL ) )
	{
		send_to_char( "That room is private right now.\n\r", ch );
		return;
	}

	if ( ch->fighting != NULL )
		stop_fighting( ch, TRUE );

	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( get_trust( rch ) >= ch->invis_level )
		{
			if ( ch->pcdata != NULL && ch->pcdata->bamfout[ 0 ] != '\0' )
				act( "$t", ch, ch->pcdata->bamfout, rch, TO_VICT );
			else
				act( "$n rozp³ywa siê i znika.", ch, NULL, rch, TO_VICT );
		}
	}

	char_from_room( ch );
	char_to_room( ch, location );


	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( get_trust( rch ) >= ch->invis_level )
		{
			if ( ch->pcdata != NULL && ch->pcdata->bamfin[ 0 ] != '\0' )
				act( "$t", ch, ch->pcdata->bamfin, rch, TO_VICT );
			else
				act( "$n materializuje siê przed tob±.", ch, NULL, rch, TO_VICT );
		}
	}

	do_function( ch, &do_look, "auto" );
	return;
}

void do_violate( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA * location;
	CHAR_DATA *rch;

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Goto where?\n\r", ch );
		return;
	}

	if ( ( location = find_location( ch, argument ) ) == NULL )
	{
		send_to_char( "No such location.\n\r", ch );
		return;
	}

	if ( !room_is_private( location ) )
	{
		send_to_char( "That room isn't private, use goto.\n\r", ch );
		return;
	}

	if ( ch->fighting != NULL )
		stop_fighting( ch, TRUE );

	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( get_trust( rch ) >= ch->invis_level )
		{
			if ( ch->pcdata != NULL && ch->pcdata->bamfout[ 0 ] != '\0' )
				act( "$t", ch, ch->pcdata->bamfout, rch, TO_VICT );
			else
				act( "$n rozp³ywa siê i znika.", ch, NULL, rch, TO_VICT );
		}
	}

	char_from_room( ch );
	char_to_room( ch, location );


	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( get_trust( rch ) >= ch->invis_level )
		{
			if ( ch->pcdata != NULL && ch->pcdata->bamfin[ 0 ] != '\0' )
				act( "$t", ch, ch->pcdata->bamfin, rch, TO_VICT );
			else
				act( "$n materializuje sie przed tob±.", ch, NULL, rch, TO_VICT );
		}
	}

	do_function( ch, &do_look, "auto" );
	return;
}

/* RT to replace the 3 stat commands */

void do_stat ( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	char *string;
	OBJ_DATA *obj;
	ROOM_INDEX_DATA *location;
	CHAR_DATA *victim;
	extern int port;

	string = one_argument( argument, arg );
	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Sk³adnia:\n\r", ch );
		send_to_char( "  stat <name>\n\r", ch );
		send_to_char( "  stat obj <name>\n\r", ch );
		send_to_char( "  stat mob <name>\n\r", ch );
		send_to_char( "  stat room <number>\n\r", ch );
		return;
	}

	if ( port == PORT_PLAYER && !IS_SET( ch->pcdata->wiz_conf, W5 ) )
	{
		send_to_char( "Na player-world moze to zrobiæ tylko sêdzia albo lord.\n\r", ch );
		return ;
	}

	if ( !str_cmp( arg, "room" ) )
	{
		do_function( ch, &do_rstat, string );
		return;
	}

	if ( !str_cmp( arg, "obj" ) )
	{
		do_function( ch, &do_ostat, string );
		return;
	}

	if ( !str_cmp( arg, "char" ) || !str_cmp( arg, "mob" ) )
	{
		do_function( ch, &do_mstat, string );
		return;
	}

	/* do it the old way */

	obj = get_obj_world( ch, argument );
	if ( obj != NULL )
	{
		do_function( ch, &do_ostat, argument );
		return;
	}

	victim = get_char_world( ch, argument );
	if ( victim != NULL )
	{
		do_function( ch, &do_mstat, argument );
		return;
	}

	location = find_location( ch, argument );
	if ( location != NULL )
	{
		do_function( ch, &do_rstat, argument );
		return;
	}

	send_to_char( "Nothing by that name found anywhere.\n\r", ch );
}

void do_rstat( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	ROOM_INDEX_DATA *location;
	OBJ_DATA *obj;
	CHAR_DATA *rch;
	int door;

	one_argument( argument, arg );
	location = ( arg[ 0 ] == '\0' ) ? ch->in_room : find_location( ch, arg );
	if ( location == NULL )
	{
		send_to_char( "No such location.\n\r", ch );
		return;
	}

	if ( !is_room_owner( ch, location ) && ch->in_room != location
		 && room_is_private( location ) && !IS_TRUSTED( ch, IMPLEMENTOR ) )
	{
		send_to_char( "That room is private right now.\n\r", ch );
		return;
	}

	sprintf( buf, "Name: '%s'\n\rArea: '%s'\n\r",
			 location->name,
			 location->area->name );
	send_to_char( buf, ch );

	sprintf( buf,
			 "Vnum: %d  Sector: %d  Light: %d  Healing: %d  Rent rate: %d\n\r",
			 location->vnum,
			 location->sector_type,
			 location->light,
			 location->heal_rate,
			 location->rent_rate );
	send_to_char( buf, ch );

	sprintf( buf,
			 "Room flags: %s.\n\rDescription:\n\r%s",
			 ext_flag_string( room_flags, location->room_flags ),
			 location->description );
	send_to_char( buf, ch );

	sprintf( buf,
			 "Night Description:\n\r%s",
			 location->nightdesc );
	send_to_char( buf, ch );


	if ( location->extra_descr != NULL )
	{
		EXTRA_DESCR_DATA * ed;

		send_to_char( "Extra description keywords: '", ch );
		for ( ed = location->extra_descr; ed; ed = ed->next )
		{
			send_to_char( ed->keyword, ch );
			if ( ed->next != NULL )
				send_to_char( " ", ch );
		}
		send_to_char( "'.\n\r", ch );
	}

	send_to_char( "Characters:", ch );
	for ( rch = location->people; rch; rch = rch->next_in_room )
	{
		if ( can_see( ch, rch ) )
		{
			send_to_char( " ", ch );
			one_argument( rch->name, buf );
			send_to_char( buf, ch );
		}
	}

	send_to_char( ".\n\rObjects:   ", ch );
	for ( obj = location->contents; obj; obj = obj->next_content )
	{
		send_to_char( " ", ch );
		one_argument( obj->name, buf );
		send_to_char( buf, ch );
	}
	send_to_char( ".\n\r", ch );

	for ( door = 0; door <= 5; door++ )
	{
		EXIT_DATA *pexit;

		if ( ( pexit = location->exit[ door ] ) != NULL )
		{
			sprintf( buf,
					 "Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'. Virtual name: '%s'.\n\rTextOut: '%s'.\n\rTextIn: '%s'.\n\rDesc: %sNightdesc: %s",

					 door,
					 ( pexit->u1.to_room == NULL ? -1 : pexit->u1.to_room->vnum ),
					 pexit->key,
					 pexit->exit_info,
					 pexit->keyword,
					 ( pexit->vName && pexit->vName[ 0 ] != '\0' ) ? pexit->vName : "(none)",
					 ( pexit->TextOut && pexit->TextOut[ 0 ] != '\0' ) ? pexit->TextOut : "(none)",
					 ( pexit->TextIn && pexit->TextIn[ 0 ] != '\0' ) ? pexit->TextIn : "(none)",
					 pexit->description[ 0 ] != '\0' ? pexit->description : "(none).\n\r",
					 ( pexit->nightdescription && pexit->nightdescription[ 0 ] != '\0' ) ? pexit->nightdescription : "(none).\n\r" );
			send_to_char( buf, ch );

			if ( pexit->alarm != NULL )
				print_char( ch, "Gracz '%s' na³o¿y³ na te wyjscie alarm.\n\r", pexit->alarm->name );
		}
	}

	return;
}

void do_ostat( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	AFFECT_DATA *paf;
    OBJ_DATA *obj;
    int value_number;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Stat what?\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_world( ch, argument ) ) == NULL )
	{
		send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
		return;
	}

	sprintf( buf, "Name(s): %s\n\r", obj->name );
	send_to_char( buf, ch );

    sprintf( buf, "{GArea{x:         [%5d] %s\n\r",
			!obj->pIndexData->area ? -1        : obj->pIndexData->area->vnum,
			!obj->pIndexData->area ? "No Area" : obj->pIndexData->area->name );
	send_to_char( buf, ch );

    sprintf(buf,
			"{GMiejscowniki{x: [%s] ({Gco to?{x)\n\r"
			"{GDope³niacz{x:   [%s] ({Gczego nie ma?{x)\n\r"
			"{GCelownik{x:     [%s] ({Gczemu siê przygl±dam?{x)\n\r"
			"{GBiernik{x:      [%s] ({Gco widzê?{x)\n\r"
			"{GNarzêdnik{x:    [%s] ({Gczym siê pos³ugujê?{x)\n\r"
			"{GMiejscownik{x:  [%s] ({Go czym mówiê?{x)\n\r",
			obj->name,obj->name2,obj->name3,obj->name4,obj->name5,obj->name6);
	send_to_char(buf,ch);

    sprintf( buf, "{GVnum{x:         [%5d]\n\r{GType{x:         [%s]\n\r",
			obj->pIndexData->vnum,
			flag_string( type_flags, obj->item_type ) );
	send_to_char( buf, ch );

	sprintf( buf, "{GWear flags{x:   [%s] {Y? wear-loc{x\n\r",
			flag_string( wear_flags, obj->wear_flags ) );
	send_to_char( buf, ch );

	sprintf( buf, "{GExtra flags{x:  [%s] {Y? extra{x\n\r",
			ext_flag_string( extra_flags, obj->extra_flags ) );
	send_to_char( buf, ch );

	sprintf( buf, "{GExwear flags{x: [%s] {Y? exwear{x\n\r",
			ext_flag_string( wear_flags2, obj->wear_flags2 ) );
	send_to_char( buf, ch );

	sprintf( buf, "{GMaterial{x:     [%s]  {Ymaterial{X\n\r",
			material_table[obj->material].name);
	send_to_char( buf, ch );

    sprintf
        (
         buf,
         "{GCondition{x:    [%5d]\n\r",
         obj->condition
        );
	send_to_char( buf, ch );

    /**
     * repair limits
     */
    sprintf( buf, "{GRepair limit{x: [%5d]  {Yrepair{x\n\r", obj->repair_limit ); send_to_char( buf, ch );
    sprintf( buf, "{GR. counter{x:   [%5d]\n\r", obj->repair_counter ); send_to_char( buf, ch );
    sprintf( buf, "{GR. penalty{x:   [%5d]\n\r", obj->repair_penalty ); send_to_char( buf, ch );
    sprintf( buf, "{GR. condition{x: [%5d]\n\r", obj->repair_condition ); send_to_char( buf, ch );

    sprintf
        (
         buf,
         "{GTimer{x:        [%5d]\n\r",
         obj->timer
        );
	send_to_char( buf, ch );

    /**
     * weight
     */
    sprintf ( buf, "{GWeight{x:       [%5d] ({G%.2f kg{x)\n\r", obj->weight, (float) obj->weight / 22.05);
    send_to_char( buf, ch );
    /**
     * cost
     */
    sprintf ( buf, "{GCost{x:         [%5d] %s\n\r", obj->cost, money_string_short( obj->cost ));
    send_to_char( buf, ch );
    /**
     * rent
     */
    sprintf( buf, "{GRent/day{x:     [%5d] %s - wyliczone jako %d%% ceny.\n\r", obj->rent_cost, money_string_short( obj->rent_cost ), RENT_COST_RATE );
    send_to_char( buf, ch );

	sprintf( buf, "{GLiczba{x:       [%s]\n\r", obj->liczba_mnoga == 0 ? "pojedyñcza" : "mnoga" );
	send_to_char( buf, ch );

    sprintf
        (
         buf,
         "{GRodzaj{x:       [%s]\n\r",
         gender_table[ obj->gender ].name
        );
    send_to_char( buf, ch );

    sprintf
        (
         buf,
         "{GShort desc{x:   %s\n\r{GLong desc{x:\n\r     %s\n\r",
         obj->short_descr,
         obj->description
        );
    send_to_char( buf, ch );

	//show_obj_values( ch, obj->pIndexData );
	switch( obj->item_type )
	{
		/* No values. */
		default:
			break;
		case ITEM_SPELLBOOK:
            if (ch->pcdata->wiz_conf & W5)
            {
                sprintf
                    (
                     buf,
                     "\n\r[v0] Klasa      : %s\n\r"
                     "[v1] Trudno¶æ   : %d\n\r"
                     "[v2] Nazwa czaru: %s\n\r"
                     "[v3] Nazwa czaru: %s\n\r"
                     "[v4] Nazwa czaru: %s\n\r"
                     "[v5] Nazwa czaru: %s\n\r"
                     "[v6] Nazwa czaru: %s\n\r\n\r",
                     class_table[obj->value[0]].name,
                     obj->value[1],
                     obj->value[2] == 0 ? "{Dnieustawiono{x" : skill_table[obj->value[2]].name,
                     obj->value[3] == 0 ? "{Dnieustawiono{x" : skill_table[obj->value[3]].name,
                     obj->value[4] == 0 ? "{Dnieustawiono{x" : skill_table[obj->value[4]].name,
                     obj->value[5] == 0 ? "{Dnieustawiono{x" : skill_table[obj->value[5]].name,
                     obj->value[6] == 0 ? "{Dnieustawiono{x" : skill_table[obj->value[6]].name
                    );
                send_to_char( buf, ch );
            }
			break;
    }

    sprintf
        (
         buf,
         "\n\rFormat: %s Resets: %d\n\r",
         obj->pIndexData->new_format ? "new" : "old",
         obj->pIndexData->reset_num
        );
    send_to_char( buf, ch );

    /*
       sprintf( buf, "EXWear bits: %s\n\r",ext_flag_string( wear_flags2, obj->wear_flags2 ) );
       send_to_char( buf, ch );

       sprintf( buf, "Number: %d/%d  Weight: %d/%d/%d (10th pounds)\n\r", 1, get_obj_number( obj ),
       obj->weight, get_obj_weight( obj ), get_true_weight( obj ) );
       send_to_char( buf, ch );
     */

	sprintf( buf, "On Ground: %d\n\r", obj->on_ground );
	send_to_char( buf, ch );


	if ( obj->is_spell_item )
	{
		print_char( ch, "Jest komponentem i ma %d u¿yæ oraz czas ¿ycia %d godzin mudowych.\n\r", obj->spell_item_counter, obj->spell_item_timer );
	}

	print_char( ch, "Material: %s   Liczba %s.\n\r", material_table[ obj->material ].name, obj->liczba_mnoga == 0 ? "pojedyñcza" : "mnoga" );

	sprintf( buf,
			 "In room: %d  In object: %s  Carried by: %s  Wear_loc: %d\n\r",
			 obj->in_room	== NULL	? 0 : obj->in_room->vnum,
			 obj->in_obj	== NULL	? "(none)" : obj->in_obj->short_descr,
			 obj->carried_by == NULL	? "(none)" :
			 can_see( ch, obj->carried_by ) ? obj->carried_by->name
			 : "someone",
			 obj->wear_loc );
	send_to_char( buf, ch );

	if ( obj->trap > 0 )
	{
		sprintf( buf, "Obj got trap:  vnum %d\n\r", obj->trap );
		send_to_char( buf, ch );
	}

    sprintf( buf, "Values: %d %d %d %d %d %d %d\n\r",
            obj->value[ 0 ], obj->value[ 1 ], obj->value[ 2 ], obj->value[ 3 ],
            obj->value[ 4 ], obj->value[ 5 ], obj->value[ 6 ] );
    send_to_char( buf, ch );
	/* now give out vital statistics as per identify */

	switch ( obj->item_type )
	{
		case ITEM_SCROLL:
		case ITEM_POTION:
		case ITEM_PILL:
			sprintf( buf, "Level %d spells of:", obj->value[ 0 ] );
            send_to_char( buf, ch );

            for (value_number = 1; value_number < 5; value_number++)
            {
                if ( obj->value[ value_number ] >= 0 && obj->value[ value_number ] < MAX_SKILL )
                {
                    send_to_char( " '", ch );
                    send_to_char( skill_table[ obj->value[ value_number ] ].name, ch );
                    send_to_char( "'", ch );
                }
            }

			send_to_char( ".\n\r", ch );
			break;

		case ITEM_WAND:
		case ITEM_STAFF:
			sprintf( buf, "Has %d(%d) charges of level %d",
					 obj->value[ 1 ], obj->value[ 2 ], obj->value[ 0 ] );
			send_to_char( buf, ch );

			if ( obj->value[ 3 ] >= 0 && obj->value[ 3 ] < MAX_SKILL )
			{
				send_to_char( " '", ch );
				send_to_char( skill_table[ obj->value[ 3 ] ].name, ch );
				send_to_char( "'", ch );
			}

			send_to_char( ".\n\r", ch );
			break;

		case ITEM_DRINK_CON:
			sprintf( buf, "It holds %s-colored %s.\n\r",
					 liq_table[ obj->value[ 2 ] ].liq_color,
					 liq_table[ obj->value[ 2 ] ].liq_name );
			send_to_char( buf, ch );
			break;


		case ITEM_WEAPON:
			send_to_char( "Weapon type is ", ch );
			switch ( obj->value[ 0 ] )
			{
				case( WEAPON_EXOTIC ) :
								send_to_char( "exotic\n\r", ch );
					break;
				case( WEAPON_SWORD ) :
								send_to_char( "sword\n\r", ch );
					break;
				case( WEAPON_DAGGER ) :
								send_to_char( "dagger\n\r", ch );
					break;
				case( WEAPON_SPEAR ) :
								send_to_char( "spear\n\r", ch );
					break;
				case( WEAPON_MACE ) :
								send_to_char( "mace\n\r", ch );
					break;
				case( WEAPON_AXE ) :
								send_to_char( "axe\n\r", ch );
					break;
				case( WEAPON_FLAIL ) :
								send_to_char( "flail\n\r", ch );
					break;
				case( WEAPON_WHIP ) :
								send_to_char( "whip\n\r", ch );
					break;
				case( WEAPON_POLEARM ) :
								send_to_char( "polearm\n\r", ch );
					break;
				case( WEAPON_STAFF ) :
								send_to_char( "staff\n\r", ch );
					break;
				case( WEAPON_SHORTSWORD ) :
								send_to_char( "short-sword\n\r", ch );
					break;
				case( WEAPON_CLAWS ) :
							  send_to_char( "claw\n\r", ch );
					break;
								default:
					send_to_char( "unknown\n\r", ch );
					break;
			}
			if ( obj->pIndexData->new_format )
				sprintf( buf, "Damage is %dd%d + %d (average %d)\n\r",
						 obj->value[ 1 ], obj->value[ 2 ], obj->value[ 6 ],
						 obj->value[ 6 ] + ( ( ( 1 + obj->value[ 2 ] ) * obj->value[ 1 ] ) / 2 ) );
			else
				sprintf( buf, "Damage is %d to %d (average %d)\n\r",
						 obj->value[ 1 ], obj->value[ 2 ],
						 ( obj->value[ 1 ] + obj->value[ 2 ] ) / 2 );
			send_to_char( buf, ch );

			sprintf( buf, "Damage noun is %s.\n\r",
					 ( obj->value[ 3 ] > 0 && obj->value[ 3 ] < MAX_DAMAGE_MESSAGE ) ?
					 attack_table[ obj->value[ 3 ] ].noun : "undefined" );
			send_to_char( buf, ch );

			if ( obj->value[ 5 ] > 0 )
	{
				sprintf( buf, "Bonus do trafienia : %d.\n\r", obj->value[ 5 ] );
				send_to_char( buf, ch );
			}

			if ( obj->value[ 4 ] )      /* weapon flags */
            {
                sprintf( buf, "Weapons flags: %s\n\r", flag_string( weapon_type2, obj->value[4] ) );
                /*
                   sprintf( buf, "Weapons flags: %s\n\r",
                   weapon_bit_name( obj->value[ 4 ] ) );
                 */
                send_to_char( buf, ch );
            }
			break;

		case ITEM_MUSICAL_INSTRUMENT:
			send_to_char( "Typ instrumentu muzycznego to ", ch );
			switch ( obj->value[ 0 ] )
			{
				case( INSTRUMENT_OCARINA ) :
								send_to_char( "ocarina\n\r", ch );
					break;
				case( INSTRUMENT_FLUTE ) :
								send_to_char( "flute\n\r", ch );
					break;
				case( INSTRUMENT_LUTE ) :
								send_to_char( "lute\n\r", ch );
					break;
				case( INSTRUMENT_HARP ) :
								send_to_char( "harp\n\r", ch );
					break;
				case( INSTRUMENT_MANDOLIN ) :
								send_to_char( "mandolin\n\r", ch );
					break;
				case( INSTRUMENT_PIANO ) :
								send_to_char( "piano\n\r", ch );
					break;
				default:
					send_to_char( "unknown\n\r", ch );
					break;
			}

			sprintf( buf, "Procentowy bonus do skilla: %d.\n\r", obj->value[ 1 ] );
			send_to_char( buf, ch );
			sprintf( buf, "Ten instrument jest nastrojony w %d %%.\n\r", obj->value[ 2 ] );
			send_to_char( buf, ch );
			break;

		case ITEM_ARMOR:
			sprintf( buf,
					 "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic\n\r",
					 obj->value[ 0 ], obj->value[ 1 ], obj->value[ 2 ], obj->value[ 3 ] );
			send_to_char( buf, ch );
			break;

		case ITEM_CONTAINER:
			sprintf( buf, "Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
					 obj->value[ 0 ], obj->value[ 3 ], cont_bit_name( obj->value[ 1 ] ) );
			send_to_char( buf, ch );
			if ( obj->value[ 4 ] != 100 )
			{
				sprintf( buf, "Weight multiplier: %d%%\n\r",
						 obj->value[ 4 ] );
				send_to_char( buf, ch );
			}
			break;

		case ITEM_TURN:
			sprintf( buf, "Premia do umiejêtno¶ci: %d.\n\r", obj->value[ 0 ] );
			send_to_char( buf, ch );
			sprintf( buf, "Premia do obra¿eñ: %d.\n\r", obj->value[ 1 ] );
			send_to_char( buf, ch );
			break;
	}


	if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
	{
		EXTRA_DESCR_DATA * ed;

		send_to_char( "Extra description keywords: '", ch );

		for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
		{
			send_to_char( ed->keyword, ch );
			if ( ed->next != NULL )
				send_to_char( " ", ch );
		}

		for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
		{
			send_to_char( ed->keyword, ch );
			if ( ed->next != NULL )
				send_to_char( " ", ch );
		}

		send_to_char( "'\n\r", ch );
	}

	for ( paf = obj->affected; paf != NULL; paf = paf->next )
	{
		sprintf( buf, "Affects %s by %d, level %d",
				 affect_loc_name( paf->location ), paf->modifier, paf->level );
		send_to_char( buf, ch );

		if ( paf->duration > -1 )
		  {
		    sprintf( buf, ", {G%d{x hours", paf->duration );
		    send_to_char( buf, ch );
		  }

		if ( paf->rt_duration > -1 )
		  {
		    sprintf( buf, ", {G%d{x rt_duration (in minutes)", paf->rt_duration );
		    send_to_char( buf, ch );
		  }

		send_to_char( ".\n\r", ch );

		if ( paf->bitvector && paf->bitvector != &AFF_NONE )
		{
			switch ( paf->where )
			{
				case TO_AFFECTS:
					sprintf( buf, "Adds %s affect.\n",
							 affect_bit_name( NULL, paf->bitvector ) );
					break;
				case TO_WEAPON:
					sprintf( buf, "Adds %s weapon flags.\n",
							 weapon_bit_name( paf->location ) );
					break;
				case TO_OBJECT:
					sprintf( buf, "Adds %s object flag.\n",
							 ext_bit_name( extra_flags, paf->bitvector ) );
					break;
			}
			send_to_char( buf, ch );
		}
	}

	if ( !obj->enchanted )
    {
		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
		{
			sprintf( buf, "Affects %s by %d, level %d.\n\r",
					 affect_loc_name( paf->location ), paf->modifier, paf->level );
			send_to_char( buf, ch );
			if ( paf->bitvector && paf->bitvector != &AFF_NONE )
			{
				switch ( paf->where )
				{
					case TO_AFFECTS:
						sprintf( buf, "Adds %s affect.\n",
								 affect_bit_name( NULL, paf->bitvector ) );
						break;
					case TO_OBJECT:
						sprintf( buf, "Adds %s object flag.\n",
								 ext_bit_name( act_flags, paf->bitvector ) );
						break;
				}
				send_to_char( buf, ch );
			}
		}
    }

    if ( obj->item_type == ITEM_WEAPON && obj->spec_dam )
	{
		int cnt;
        SPEC_DAMAGE *specdam;

		sprintf(buf, "\n\r{CSpec damages dla [%5d]{x:\n\r", obj->pIndexData->vnum);
		send_to_char( buf, ch );

		for ( cnt = 1, specdam = obj->spec_dam; specdam; specdam = specdam->next )
		{
			specdam_to_char( ch, cnt, specdam );
			cnt++;
		}
	}

	return;
}

void do_mstat( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];
    char arg[ MAX_INPUT_LENGTH ];
    AFFECT_DATA *paf;
    CHAR_DATA *victim;
    CHARM_DATA *charm;
    int i;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Czyje statystyki chcesz zobaczyæ?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie znaleziono szukanej osoby lub moba.\n\r", ch );
        return;
    }

    if ( !IS_NPC( victim ) )
    {
        extern int port;
        if ( port == PORT_PLAYER && !IS_SET( ch->pcdata->wiz_conf, W6 ) )
        {
            send_to_char( "Wybacz, ale na player-world moze to zrobiæ tylko Lord.\n\r", ch );
            return ;
        }
    }

    sprintf( buf, "Name: %s\n\r", victim->name );
    send_to_char( buf, ch );

    sprintf( buf,
            "Vnum: %d  Format: %s  Race: %s  Group: %d  Sex: %s  Room: %d\n\r",
            IS_NPC( victim ) ? victim->pIndexData->vnum : 0,
            IS_NPC( victim ) ? victim->pIndexData->new_format ? "new" : "old" : "pc",
            race_table[ GET_RACE( victim ) ].name,
            IS_NPC( victim ) ? victim->group : 0, sex_table[ victim->sex ].name,
            victim->in_room == NULL	? 0 : victim->in_room->vnum
           );
    send_to_char( buf, ch );

    if ( IS_NPC( victim ) )
    {
        sprintf( buf, "Count: %d  Killed: %d\n\r", victim->pIndexData->count, victim->pIndexData->killed );
        send_to_char( buf, ch );
        sprintf( buf, "{GFlagi obra¿eñ{x: [%s]   ", flag_string( weapon_type2, victim->attack_flags ) );
        send_to_char( buf, ch );
        sprintf( buf, "{GUmagicznienie{x: %d\n\r", victim->magical_damage) ;
        send_to_char( buf, ch );
    }

    /**
     * statystyki
     */
    sprintf
        (
         buf,
         "Str:%d(%d) Int:%d(%d) Wis:%d(%d) Dex:%d(%d) Con:%d(%d) Cha:%d(%d) Luc:%d(%d)\n\r",
         victim->perm_stat[ STAT_STR ],
         get_curr_stat( victim, STAT_STR ),
         victim->perm_stat[ STAT_INT ],
         get_curr_stat( victim, STAT_INT ),
         victim->perm_stat[ STAT_WIS ],
         get_curr_stat( victim, STAT_WIS ),
         victim->perm_stat[ STAT_DEX ],
         get_curr_stat( victim, STAT_DEX ),
         victim->perm_stat[ STAT_CON ],
         get_curr_stat( victim, STAT_CON ),
         victim->perm_stat[ STAT_CHA ],
         get_curr_stat( victim, STAT_CHA ),
         victim->perm_stat[ STAT_LUC ],
         get_curr_stat( victim, STAT_LUC )
        );
    send_to_char( buf, ch );

    /**
     * punkty
     */
    sprintf( buf, "Punkty do wydania: %d, Punkty wydane: %d \n\r", victim->statpointsleft,victim->statpointsspent);
    send_to_char( buf, ch );

    /**
     * wzrost i waga
     */
    sprintf
        (
         buf,
         "Wzrost: %d cm Waga: %d.%d kg\n\r",
         victim->height,
         victim->weight / 10,
         victim->weight % 10
        );
    send_to_char( buf, ch );

    sprintf( buf, "Hp: %d/%d  Move: %d/%d\n\r",
            victim->hit, get_max_hp(victim),
            victim->move, victim->max_move );
    send_to_char( buf, ch );

    sprintf( buf,
            "Lv: %d  Class: %s  Align: %d  Exp: %ld\n\r",
            victim->level,
            IS_NPC( victim ) ? "mobile" : get_class_name(victim),
            victim->alignment,
            victim->exp );
    send_to_char( buf, ch );

    /**
     * kasa
     */
    if ( IS_NPC( victim ) )
    {
        sprintf ( buf, "Wealth: %ld\n\r", victim->pIndexData->wealth );
        send_to_char( buf, ch );
    }
    sprintf
        (
         buf,
         "Money: %ldc %lds %ldg %ldm\n\r",
         victim->copper,
         victim->silver,
         victim->gold,
         victim->mithril
        );
    send_to_char( buf, ch );
    if ( !IS_NPC( victim ) )
    {
        sprintf ( buf, "Bank: %ld\n\r", victim->bank );
        send_to_char( buf, ch );
    }

    sprintf( buf, "Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
            GET_AC( victim, AC_PIERCE ), GET_AC( victim, AC_BASH ),
            GET_AC( victim, AC_SLASH ), GET_AC( victim, AC_EXOTIC ) );
    send_to_char( buf, ch );

    sprintf( buf,
            "Hit: %d  Dam: %d  Saves: [%d %d %d %d %d]\n\rSize: %s  Position: %s  Wimpy: %d\n\r",
            GET_HITROLL( victim ), GET_DAMROLL( victim, NULL ),
            victim->saving_throw[ 0 ], victim->saving_throw[ 1 ], victim->saving_throw[ 2 ],
            victim->saving_throw[ 3 ], victim->saving_throw[ 4 ],
            size_table[ victim->size ].name, position_table[ victim->position ].name,
            victim->wimpy );
    send_to_char( buf, ch );

    /*
     * TYLKO MOBY: Umagicznienie i Bonus na broñ
     */
    if ( IS_NPC ( victim ) )
    {
        sprintf ( buf,
                "Umagicznienie: %d  Bonus na broñ: %d\n\r",
                victim->magical_damage,
                victim->weapon_damage_bonus
                );
        send_to_char( buf, ch );
    }
    /**
     * TYLKO GRACZE: last rent
     */
    else
    {
        sprintf( buf, "Last rent: %d\n\r", victim->pcdata->last_rent );
        send_to_char( buf, ch );
    }

    // czy czego¶ dosiada
    if ( victim->mount )
    {
        sprintf( buf, "Mount: %s [%d]\n\r",  victim->mount->name, victim->mount->pIndexData->vnum);
    }
    else {
        sprintf( buf, "Mount: (none)\n\r");
    }
    send_to_char( buf, ch );

    if ( IS_NPC( victim ) && victim->pIndexData->new_format )
    {
        sprintf( buf, "Damage: %dd%d  Message:  %s\n\r",
                victim->damage[ DICE_NUMBER ], victim->damage[ DICE_TYPE ],
                attack_table[ victim->dam_type ].noun );
        send_to_char( buf, ch );
    }
    sprintf( buf, "Fighting: %s\n\r",
            victim->fighting ? victim->fighting->name : "(none)" );
    send_to_char( buf, ch );

    //	if ( !IS_NPC( victim ) )
    //	{
    sprintf( buf,
            "Thirst: %d  Hunger: %d Drunk: %d  Sleepy: %d\n\r",
            victim->condition[ COND_THIRST ],
            victim->condition[ COND_HUNGER ],
            victim->condition[ COND_DRUNK ],
            victim->condition[ COND_SLEEPY ] );
    send_to_char( buf, ch );
    //	}

    sprintf( buf, "Carry number: %d  Carry weight: %d\n\r",
            victim->carry_number, get_carry_weight( victim ) / 10 );
    send_to_char( buf, ch );


    if ( !IS_NPC( victim ) )
    {
        sprintf( buf,
                "Age: %d  Played: %d  Last Level: %d  Timer: %d  Glory: %d\n\r",
                get_age( victim ),
                ( int ) ( victim->played + current_time - victim->logon ) / 3600,
                victim->pcdata->last_level,
                victim->timer,
                victim->glory );
        send_to_char( buf, ch );


        for ( i = 0;i < MAX_COUNTER; i++ )
        {
            sprintf( buf, "Counter[%d]: %d\n\r", i, victim->counter[ i ] );
            send_to_char( buf, ch );
        }
    }

    sprintf( buf, "Act: %s\n\r", ext_flag_string( act_flags, victim->act ) );
    send_to_char( buf, ch );

    if ( victim->comm )
    {
        sprintf( buf, "Comm: %s\n\r", comm_bit_name( victim->comm ) );
        send_to_char( buf, ch );
    }

    if ( IS_NPC( victim ) && !ext_flags_none( victim->off_flags ) )
    {
        sprintf( buf, "Offense: %s\n\r", ext_flag_string( off_flags, victim->off_flags ) );
        send_to_char( buf, ch );
    }


    sprintf( buf, "Form: %s\n\rParts: %s\n\r",
            form_bit_name( victim->form ), part_bit_name( victim->parts ) );
    send_to_char( buf, ch );

    if ( victim->affected_by )
    {
        sprintf( buf, "Affected by %s\n\r",
                affect_bit_name( victim, &AFF_NONE ) );

        send_to_char( buf, ch );
    }

    sprintf( buf, "Master: %s  Leader: %s\n\r",
            victim->master	? victim->master->name : "(none)",
            victim->leader	? victim->leader->name : "(none)" );
    send_to_char( buf, ch );

    sprintf( buf, "Hunting: %s\n\r",
            victim->hunting ? victim->hunting->name : "(none)" );
    send_to_char( buf, ch );

    if ( !IS_NPC( victim ) )
    {
        //Brohacz: bounty: pobierane z globalnej listy, a nie z postaci
        BOUNTY_DATA *bounty;
        bounty = search_bounty( victim->name );
        sprintf( buf, "Security: %d.     Bounty: %d.\n\r", victim->pcdata->security, bounty ? bounty->value : 0 );
        send_to_char( buf, ch );
    }

    sprintf( buf, "Short description: %s\n\rLong  description: %s\n\r",
            victim->short_descr,
            victim->long_descr[ 0 ] != '\0' ? victim->long_descr : "(none)" );
    send_to_char( buf, ch );

    if ( IS_NPC( victim ) && victim->spec_fun != 0 )
    {
        sprintf( buf, "Mobile has special procedure %s.\n\r",
                spec_name( victim->spec_fun ) );
        send_to_char( buf, ch );
    }

    for ( paf = victim->affected; paf != NULL; paf = paf->next )
    {
        sprintf( buf,
                "Spell: '%s' modifies %s by %d for %d mud hours and %d realtime with bits %s, level %d.\n\r",
                skill_table[ ( int ) paf->type ].name,
                affect_loc_name( paf->location ),
                paf->modifier,
                paf->duration,
                paf->rt_duration,
                affect_bit_name( NULL, paf->bitvector ),
                paf->level
               );
        send_to_char( buf, ch );
    }

    for ( i = 0; i < MAX_RESIST; i++ )
    {
        if ( victim->resists[ i ] != 0 )
        {
            sprintf( buf, "{ROdpornosc na {C%-15.15s{R :%ld%%{x\n\r", resist_name( i ), UMIN( victim->resists[ i ], 100 ) );
            send_to_char( buf, ch );
        }
    }

    if ( !IS_NPC( victim ) )
    {
        char cl[MAX_STRING_LENGTH];
        char vl[MAX_STRING_LENGTH];
        MURDER_LIST * list;

        send_to_char( "\n\n\r", ch );
        print_char( ch, "Statystyki zabójstw i ¶mierci %s:\n\r", victim->name2 );
        print_char( ch, "Zgony:                           %d\n\r", victim->pcdata->death_statistics.deaths );
        print_char( ch, "Zgony z r±k graczy:              %d\n\r", victim->pcdata->death_statistics.pkdeaths );
        print_char( ch, "Zgony z r±k mobów:               %d\n\r",
                victim->pcdata->death_statistics.deaths - ( victim->pcdata->death_statistics.pkdeaths + victim->pcdata->death_statistics.suicides ) );
        print_char( ch, "Zgony przypadkowe i samobójstwa: %d\n\r", victim->pcdata->death_statistics.suicides );
        print_char( ch, "Zabite moby:                     %d\n\r", victim->pcdata->death_statistics.mob_kills );
        print_char( ch, "Zabici gracze:                   %d\n\r", victim->pcdata->death_statistics.player_kills );

        for ( list = victim->pcdata->death_statistics.pkills_list; list; list = list->next )
        {
            sprintf( cl, "(%d)", list->char_level );
            sprintf( vl, "(%d)", list->victim_level );
            print_char( ch, "%s %s %s %s %s [ Room: %-5d] %s\r",
                    victim->name,
                    list->char_level > 0 ? cl : "",
                    victim->sex == 2 ? "zabi³a" : victim->sex == 1 ? "zabi³" : "zabi³o",
                    list->name,
                    list->victim_level > 0 ? vl : "",
                    list->room,
                    ( char * ) ctime( &list->time ) );
        }
        for ( list = victim->pcdata->death_statistics.pkdeath_list; list; list = list->next )
        {
            sprintf( cl, "(%d)", list->char_level );
            sprintf( vl, "(%d)", list->victim_level );
            print_char( ch, "%s %s %s przez %s %s [ Room: %-5d ] %s\r",
                    victim->name,
                    list->victim_level > 0 ? vl : "",
                    victim->sex == 2 ? "zosta³a zabita" : victim->sex == 1 ? "zosta³ zabity" : "zosta³o zabite",
                    list->name,
                    list->char_level > 0 ? cl : "",
                    list->room,
                    ( char * ) ctime( &list->time ) );
        }
    }

    if ( IS_NPC( victim ) )
        return;

    print_char( ch, "\n\r{COdmiana imienia:{x\n\r"
            "Dope³niacz  :%s\n\r"
            "Celownik    :%s\n\r"
            "Biernik     :%s\n\r"
            "Narzednik   :%s\n\r"
            "Miejscownik :%s\n\r",
            victim->name2, victim->name3, victim->name4, victim->name5,
            victim->name6 );

    if ( victim->pcdata->charm_list )
    {
        print_char( ch, "\n\r{CLista zcharmowanych:{x\n\r" );
        for ( charm = victim->pcdata->charm_list; charm; charm = charm->next )
        {
            if ( IS_NPC( charm->victim ) )
                print_char( ch, "- mob %-25.25s [%5d] (poziom: %d)\n\r", charm->victim->short_descr, charm->victim->pIndexData->vnum, charm->victim->level );
            else
                print_char( ch, "- gracz %-25.25s (poziom: %d)\n\r", charm->victim->name, charm->victim->level );
        }
    }

    print_char( ch, "\n\r{CInne:{x\n\r"
            "Ilo¶æ rolowañ: %d.\n\r", victim->pcdata->rolls_count);
    print_char( ch, "\n\r{CInne:{x\n\r"
            "Ilo¶æ rolowañ nowych: %d.\n\r", victim->pcdata->new_rolls_count);
    return;
}

/* ofind and mfind replaced with vnum, vnum skill also added */

void do_vnum( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	char *string;

	string = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  vnum obj <name>\n\r", ch );
		send_to_char( "  vnum mob <name>\n\r", ch );
		send_to_char( "  vnum skill <skill or spell>\n\r", ch );
		send_to_char( "  vnum race <race>\n\r", ch );
		return;
	}

	if ( !str_cmp( arg, "obj" ) )
	{
		do_function( ch, &do_ofind, string );
		return;
	}

	if ( !str_cmp( arg, "mob" ) || !str_cmp( arg, "char" ) )
	{
		do_function( ch, &do_mfind, string );
		return;
	}

	if ( !str_cmp( arg, "skill" ) || !str_cmp( arg, "spell" ) )
	{
		do_function ( ch, &do_slookup, string );
		return;
	}

	if ( !str_cmp( arg, "race" ) )
	{
		int race;

		race = race_lookup( string );
		print_char( ch, "Nr: {C%3d{x Nazwa: {C%s{x\n\r", race, race_table[race].name );
		return;
	}
	/* do both */
	do_function( ch, &do_mfind, argument );
	do_function( ch, &do_ofind, argument );
}

void do_mfind( CHAR_DATA *ch, char *argument )
{
	extern unsigned int top_mob_index;
	char buf[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	MOB_INDEX_DATA *pMobIndex;
	ush_int vnum;
	int nMatch;
	bool fAll;
	bool found;

	one_argument( argument, arg );
	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Find whom?\n\r", ch );
		return;
	}

	fAll	= FALSE; /* !str_cmp( arg, "all" ); */
	found	= FALSE;
	nMatch	= 0;

	/*
		* Yeah, so iterating over all vnum's takes 10,000 loops.
		* Get_mob_index is fast, and I don't feel like threading another link.
		* Do you?
		* -- Furey
		*/
	for ( vnum = 0; nMatch < top_mob_index; vnum++ )
	{
		if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
		{
			nMatch++;
			if ( fAll || is_name( argument, pMobIndex->player_name ) )
			{
                found = TRUE;
                sprintf
                    (
                     buf,
                     "[%5d] %s [%s]\n\r",
                     pMobIndex->vnum,
                     pMobIndex->short_descr,
                     pMobIndex->area->name
                    );
                send_to_char( buf, ch );
			}
		}
	}

	if ( !found )
		send_to_char( "No mobiles by that name.\n\r", ch );

	return;
}

void do_ofind( CHAR_DATA *ch, char *argument )
{
	extern unsigned int top_obj_index;
	char buf[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	OBJ_INDEX_DATA *pObjIndex;
	ush_int vnum;
	int nMatch;
	bool fAll;
	bool found;

	one_argument( argument, arg );
	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Find what?\n\r", ch );
		return;
	}

	fAll	= FALSE; /* !str_cmp( arg, "all" ); */
	found	= FALSE;
	nMatch	= 0;

	/*
		* Yeah, so iterating over all vnum's takes 10,000 loops.
		* Get_obj_index is fast, and I don't feel like threading another link.
		* Do you?
		* -- Furey
		*/
	for ( vnum = 0; nMatch < top_obj_index; vnum++ )
	{
		if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
		{
			nMatch++;
			if ( fAll || is_name( argument, pObjIndex->name ) )
			{
				found = TRUE;
                sprintf
                    (
                     buf,
                     "[%5d] %s [%s]\n\r",
                     pObjIndex->vnum,
                     pObjIndex->short_descr,
                     pObjIndex->area->name
                    );
				send_to_char( buf, ch );
			}
		}
	}

	if ( !found )
		send_to_char( "No objects by that name.\n\r", ch );

	return;
}

void do_owhere( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_INPUT_LENGTH ];
	BUFFER *buffer = NULL;
	OBJ_DATA *obj;
	OBJ_DATA *in_obj;
	bool found;
	int number = 0, max_found;
	int vnum = -1;

	extern int port;

	if ( port == PORT_PLAYER && !IS_SET( ch->pcdata->wiz_conf, W5 ) )
	{
		send_to_char( "Na player-world moze to zrobiæ tylko sêdzia albo lord.\n\r", ch );
		return ;
	}

	found = FALSE;
	number = 0;
	max_found = 200;

	buffer = new_buf();

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Find what?\n\r", ch );
		return;
	}

	if ( is_number( argument ) )
		vnum = atoi( argument );

	for ( obj = object_list; obj != NULL; obj = obj->next )
	{
		if ( vnum > 0 )
		{
			if ( !can_see_obj( ch, obj ) || obj->pIndexData->vnum != vnum )
				continue;
		}
		else
		{
			if ( !can_see_obj( ch, obj ) || !is_name( argument, obj->name ) )
				continue;
		}

		found = TRUE;
		number++;

		for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
			;

		if ( in_obj->carried_by != NULL && can_see( ch, in_obj->carried_by )
				&& in_obj->carried_by->in_room != NULL )
			sprintf (
					buf,
					"%3d) %s [%5d] is carried by %s [Room %d]\n\r",
					number,
					obj->short_descr,
					obj->pIndexData->vnum,
					PERS( in_obj->carried_by, ch ),
					in_obj->carried_by->in_room->vnum
			       );
		else if ( in_obj->in_room != NULL && can_see_room( ch, in_obj->in_room ) )
			sprintf (
					buf,
					"%3d) %s [%5d] is in %s [Room %d]\n\r",
					number,
					obj->short_descr,
					obj->pIndexData->vnum,
					in_obj->in_room->name,
					in_obj->in_room->vnum
				);
		else if ( in_obj->hoarded_by != NULL && in_obj->hoarded_by->in_room != NULL && in_obj->vnum_hoard != NULL && can_see( ch, in_obj->hoarded_by ))
			sprintf (
					buf,
					"%3d) %s [%5d] is hoarded by %s [Hoard %d]\n\r",
					number,
					obj->short_descr,
					obj->pIndexData->vnum,
					PERS( in_obj->hoarded_by, ch ),
					in_obj->vnum_hoard
				);
		else
			sprintf (
					buf,
					"%3d) %s [%5d] is somewhere\n\r",
					number,
					obj->short_descr,
					obj->pIndexData->vnum
				);

		sprintf( buf, "%s", capitalize( buf ) );
		add_buf( buffer, buf );

		if ( number >= max_found )
			break;
	}

	if ( !found )
		send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
	else
		page_to_char( buf_string( buffer ), ch );

	free_buf( buffer );
}

void do_mwhere( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	BUFFER *buffer;
	CHAR_DATA *victim;
	bool found;
	int count = 0;
	int vnum;

	extern int port;

	if ( port == PORT_PLAYER && !IS_SET( ch->pcdata->wiz_conf, W5 ) )
	{
		send_to_char( "Na player-world moze to zrobiæ tylko sêdzia albo lord.\n\r", ch );
		return ;
	}

	if ( argument[ 0 ] == '\0' )
	{
		DESCRIPTOR_DATA * d;

		/* show characters logged */

		buffer = new_buf();
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( !d->character ) continue;

			if ( d->character != NULL && d->connected == CON_PLAYING
				 && d->character->in_room != NULL && can_see( ch, d->character )
				 && can_see_room( ch, d->character->in_room ) )
			{
				victim = d->character;
				count++;
				if ( d->original != NULL )
					sprintf( buf, "%3d) %s (in the body of %s) is in %s [%d]\n\r",
							 count, d->original->name, victim->short_descr,
							 victim->in_room->name, victim->in_room->vnum );
				else
					sprintf( buf, "%3d) %s is in %s [%d]\n\r",
							 count, victim->name, victim->in_room->name,
							 victim->in_room->vnum );
				add_buf( buffer, buf );
			}
		}

		page_to_char( buf_string( buffer ), ch );
		free_buf( buffer );
		return;
	}

	found = FALSE;
	buffer = new_buf();

	if ( is_number( argument ) )
	{
		vnum = atoi( argument );
		for ( victim = char_list; victim != NULL; victim = victim->next )
		{
			if ( IS_NPC( victim ) && victim->in_room != NULL
				 && victim->pIndexData->vnum == vnum )
			{
				found = TRUE;
				count++;
                sprintf
                    (
                     buf,
                     "%3d) [%5d] %-28s [%5d] %s (%s)\n\r",
                     count,
                     IS_NPC( victim ) ? victim->pIndexData->vnum : 0,
                     IS_NPC( victim ) ? victim->short_descr : victim->name,
                     victim->in_room->vnum,
                     victim->in_room->name,
                     victim->in_room->area->name
                    );
				add_buf( buffer, buf );
			}
		}
	}
	else
	{
		for ( victim = char_list; victim != NULL; victim = victim->next )
		{
			if ( victim->in_room != NULL
				 && is_name( argument, victim->ss_data ? victim->short_descr : victim->name ) )
			{
				found = TRUE;
				count++;
                sprintf
                    (
                     buf,
                     "%3d) [%5d] %-28s [%5d] %s (%s)\n\r",
                     count,
                     IS_NPC( victim ) ? victim->pIndexData->vnum : 0,
                     IS_NPC( victim ) ? victim->short_descr : victim->name,
                     victim->in_room->vnum,
                     victim->in_room->name,
                     victim->in_room->area->name
                    );
				add_buf( buffer, buf );
			}
		}
	}

	if ( !found )
    {
		act( "You didn't find any $T.", ch, NULL, argument, TO_CHAR );
    }
	else
    {
		page_to_char( buf_string( buffer ), ch );
    }

	free_buf( buffer );

	return;
}

void do_reboo( CHAR_DATA *ch, char *argument )
{
	send_to_char( "Wpisz pe³n± komendê REBOOT.\n\r", ch );
	return;
}

void do_reboot( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	extern bool merc_down;
	DESCRIPTOR_DATA *d, *d_next;
	CHAR_DATA *vch;

	if ( ch->invis_level < LEVEL_HERO )
	{
		sprintf( buf, "Reboot by %s.", ch->name );
		do_function( ch, &do_echo, buf );
	}

	save_clans();
	save_pccorpses();
	save_misc_data();

	if ( artefact_system != NULL )
	{
		reboot_artefact_list();
		do_aset( ch, "save" );
	}

	merc_down = TRUE;
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
		d_next = d->next;
		if ( d->connected < 0 ) continue;

		vch = d->original ? d->original : d->character;
		if ( vch != NULL )
			save_char_obj( vch, FALSE, FALSE );
		close_socket( d, CS_NORECURSE );
	}

	return;
}

void do_shutdow( CHAR_DATA *ch, char *argument )
{
	send_to_char( "Wpisz pe³n± komendê SHUTDOWN.\n\r", ch );
	return;
}

void do_shutdown( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	extern bool merc_down;
	DESCRIPTOR_DATA *d, *d_next;
	CHAR_DATA *vch;

#ifndef OFFLINE
	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Podaj has³o.\n\r", ch );
		return;
	}

	if ( str_cmp( argument, "taaa" ) )
	{
		send_to_char( "Z³e has³o, zosta³a ci jeszcze 1 próba i jesli ¼le podasz has³o twoja postaæ zostanie skasowana.\n\r", ch );
		return;
	}
#endif

	if ( ch->invis_level < LEVEL_HERO )
		sprintf( buf, "Shutdown by %s.", ch->name );

	append_file( ch, SHUTDOWN_FILE, buf );
	strcat( buf, "\n\r" );

	if ( ch->invis_level < LEVEL_HERO )
	{
		do_function( ch, &do_echo, buf );
	}
	merc_down = TRUE;

	save_clans();
	save_pccorpses();
	save_misc_data();

	if ( artefact_system != NULL )
	{
		reboot_artefact_list();
		do_aset( ch, "save" );
	}


	for ( d = descriptor_list; d != NULL; d = d_next )
	{
		d_next = d->next;
		if ( d->connected < 0 ) continue;

		vch = d->original ? d->original : d->character;
		if ( vch != NULL )
			save_char_obj( vch, FALSE, FALSE );
		close_socket( d, CS_NORECURSE );
	}
	return;
}

void do_protect( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * victim;

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Protect whom from snooping?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, argument ) ) == NULL )
	{
		send_to_char( "You can't find them.\n\r", ch );
		return;
	}

	if ( IS_SET( victim->comm, COMM_SNOOP_PROOF ) )
	{
		act_new( "$N is no longer snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD );
		//send_to_char("Your snoop-proofing was just removed.\n\r",victim);
		REMOVE_BIT( victim->comm, COMM_SNOOP_PROOF );
	}
	else
	{
		act_new( "$N is now snoop-proof.", ch, NULL, victim, TO_CHAR, POS_DEAD );
		//send_to_char("You are now immune to snooping.\n\r",victim);
		SET_BIT( victim->comm, COMM_SNOOP_PROOF );
	}
}

void do_snoop( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;
	char buf[ MAX_STRING_LENGTH ];

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Kogo chcesz snoopowaæ?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie ma nikogo takiego.\n\r", ch );
		return;
	}

	if ( victim->desc == NULL )
	{
		send_to_char( "Cel nie jest po³±czony z mudem.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Zatrzymywanie wszystkich snoopów.\n\r", ch );
		wiznet( "$N przestaje snoopowaæ.",
				ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust( ch ) );
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( !d->character || d->connected < 0 ) continue;

			if ( d->snoop_by == ch->desc )
				d->snoop_by = NULL;
		}
		return;
	}

	if ( victim->desc->snoop_by != NULL )
	{
		if ( victim->desc->snoop_by->character != ch )
			print_char( ch, "Ta postaæ jest snoopowana przez %s.\n\r",
						victim->desc->snoop_by->character ? victim->desc->snoop_by->character->name4 : "kogo¶" );
		else
		{
			print_char( ch, "Przestajesz snoopowaæ %s.\n\r", victim->name4 );
			sprintf( buf, "$N przestaje snoopowaæ %s.", victim->name4 );
			wiznet( buf, ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust( ch ) );
			victim->desc->snoop_by = NULL;
		}

		return;
	}

	if ( !is_room_owner( ch, victim->in_room ) && ch->in_room != victim->in_room
		 && room_is_private( victim->in_room ) && !IS_TRUSTED( ch, IMPLEMENTOR ) )
	{
		send_to_char( "Ta postaæ jest w prywatnej lokacji.\n\r", ch );
		return;
	}

	if ( get_trust( victim ) >= get_trust( ch )
		 || IS_SET( victim->comm, COMM_SNOOP_PROOF ) )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( ch->desc != NULL )
	{
		for ( d = ch->desc->snoop_by; d != NULL; d = d->snoop_by )
		{
			if ( d->character == victim || d->original == victim )
			{
				send_to_char( "Ta osoba snoopuje ciebie.\n\r", ch );
				return;
			}
		}
	}

	victim->desc->snoop_by = ch->desc;
	sprintf( buf, "$N starts snooping on %s",
			 ( IS_NPC( ch ) ? victim->short_descr : victim->name ) );
	wiznet( buf, ch, NULL, WIZ_SNOOPS, WIZ_SECURE, get_trust( ch ) );
	send_to_char( "Ok.\n\r", ch );
	return;
}

void do_switch( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ], buf[ MAX_STRING_LENGTH ];
    CHAR_DATA *victim;

    extern int port;

    if ( port == PORT_PLAYER && !IS_SET( ch->pcdata->wiz_conf, W5 ) )
    {
        send_to_char( "Na player-world moze to zrobiæ tylko sêdzia albo lord.\n\r", ch );
        return ;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Switch into whom?\n\r", ch );
        return;
    }

    if ( ch->desc == NULL ) return;

    if ( ch->desc->original != NULL )
    {
        send_to_char( "You are already switched.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Ok.\n\r", ch );
        return;
    }

    if ( !IS_NPC( victim ) )
    {
        send_to_char( "You can only switch into mobiles.\n\r", ch );
        return;
    }

    if (!IS_BUILDER( ch, victim->pIndexData->area ) )
    {
        send_to_char( "Masz niewystarczaj±ce uprawnienia.\n\r", ch );
        return;
    }

    if ( !is_room_owner( ch, victim->in_room ) && ch->in_room != victim->in_room
            && room_is_private( victim->in_room ) && !IS_TRUSTED( ch, IMPLEMENTOR ) )
    {
        send_to_char( "That character is in a private room.\n\r", ch );
        return;
    }

    if ( victim->desc != NULL )
    {
        send_to_char( "Character in use.\n\r", ch );
        return;
    }

    sprintf( buf, "$N switches into %s", victim->short_descr );
    wiznet( buf, ch, NULL, WIZ_SWITCHES, WIZ_SECURE, get_trust( ch ) );
    log_string( buf );

    ch->desc->character = victim;
    ch->desc->original = ch;
    victim->desc	= ch->desc;
    ch->desc	= NULL;
    /* change communications to match */
    if ( ch->prompt != NULL )
        victim->prompt = str_dup( ch->prompt );
    victim->comm = ch->comm;
    victim->lines = ch->lines;
    /* 0 hp bug */

    if ( victim->hit == 0 )
        victim->hit = victim->max_hit = 1;

    send_to_char( "Ok.\n\r", victim );
    return;
}

void do_return( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];

	if ( ch->desc == NULL )
		return;

	if ( ch->desc->original == NULL )
	{
		send_to_char( "You aren't switched.\n\r", ch );
		return;
	}

	send_to_char( "You return to your original body. Type replay to see any missed tells.\n\r", ch );
	if ( ch->prompt != NULL )
	{
		free_string( ch->prompt );
		ch->prompt = NULL;
	}

	sprintf( buf, "$N returns from %s.", ch->short_descr );
	wiznet( buf, ch->desc->original, 0, WIZ_SWITCHES, WIZ_SECURE, get_trust( ch ) );
	ch->desc->character	= ch->desc->original;
	ch->desc->original	= NULL;
	ch->desc->character->desc = ch->desc;
	ch->desc	= NULL;
	return;
}

/* trust levels for load and clone */
bool obj_check ( CHAR_DATA *ch, OBJ_DATA *obj )
{
	/*	if (IS_TRUSTED(ch,GOD)
		|| (IS_TRUSTED(ch,IMMORTAL) && obj->level <= 20 && obj->cost <= 1000)
		|| (IS_TRUSTED(ch,DEMI)		&& obj->level <= 10 && obj->cost <= 500)
		|| (IS_TRUSTED(ch,ANGEL)	&& obj->level <=  5 && obj->cost <= 250)
		|| (IS_TRUSTED(ch,AVATAR)   && obj->level ==  0 && obj->cost <= 100))
		return TRUE;
		else
		return FALSE;*/
	return TRUE;
}

/* for clone, to insure that cloning goes many levels deep */
void recursive_clone( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *clone )
{
	OBJ_DATA * c_obj, *t_obj;


	for ( c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content )
	{
		if ( obj_check( ch, c_obj ) )
		{
			/*artefact*/
			if ( is_artefact( c_obj ) )
			{
				act( "{RNIE MO¯NA KLONOWAÆ ARTEFAKTÓW!!!{x. [@3204]", ch, clone, NULL, TO_CHAR );
				continue;
			}
			t_obj = create_object( c_obj->pIndexData, FALSE );
			clone_object( c_obj, t_obj );
			obj_to_obj( t_obj, clone );
			recursive_clone( ch, c_obj, t_obj );
		}
	}
}

/* command that is similar to load */
void do_clone( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	char *rest;
	CHAR_DATA *mob;
	OBJ_DATA *obj;

	rest = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Clone what?\n\r", ch );
		return;
	}

	if ( !str_prefix( arg, "object" ) )
	{
		mob = NULL;
		obj = get_obj_here( ch, rest );
		if ( obj == NULL )
		{
			send_to_char( "You don't see that here.\n\r", ch );
			return;
		}
	}
	else if ( !str_prefix( arg, "mobile" ) || !str_prefix( arg, "character" ) )
	{
		obj = NULL;
		mob = get_char_room( ch, rest );
		if ( mob == NULL )
		{
			send_to_char( "You don't see that here.\n\r", ch );
			return;
		}
	}
	else /* find both */
	{
		mob = get_char_room( ch, argument );
		obj = get_obj_here( ch, argument );
		if ( mob == NULL && obj == NULL )
		{
			send_to_char( "You don't see that here.\n\r", ch );
			return;
		}
	}

	/* clone an object */
	if ( obj != NULL )
	{
		OBJ_DATA * clone;

		/*	if (!obj_check(ch,obj))
			{
				send_to_char(
				"Your powers are not great enough for such a task.\n\r",ch);
				return;
			}*/
		/*artefact*/
		if ( is_artefact( obj ) )
		{
			act( "{RNIE MO¯NA KLONOWAÆ ARTEFAKTÓW!!!{x. (Artefakt = $p) [@3276]", ch, obj, NULL, TO_CHAR );
			return;
		}

		clone = create_object( obj->pIndexData, FALSE );
		clone_object( obj, clone );
		if ( obj->carried_by != NULL )
		{
			/*artefact ?*/
			obj_to_char( clone, ch );
		}
		else
			obj_to_room( clone, ch->in_room );
		recursive_clone( ch, obj, clone );

		switch ( ch->sex )
		{
			case 0:
				act( "$n stworzy³o $p.", ch, clone, NULL, TO_ROOM );
				act( "Stworzy³o¶ $p.", ch, clone, NULL, TO_CHAR );
				break;
			case 1:
				act( "$n stworzy³ $p.", ch, clone, NULL, TO_ROOM );
				act( "Stworzy³e¶ $p.", ch, clone, NULL, TO_CHAR );
				break;
			default :
				act( "$n stworzy³a $p.", ch, clone, NULL, TO_ROOM );
				act( "Stworzy³a¶ $p.", ch, clone, NULL, TO_CHAR );
				break;
		}
		wiznet( "$N clones $p.", ch, clone, WIZ_LOAD, WIZ_SECURE, get_trust( ch ) );
		return;
	}
	else if ( mob != NULL )
	{
		CHAR_DATA * clone;
		OBJ_DATA *new_obj;
		char buf[ MAX_STRING_LENGTH ];

		if ( !IS_NPC( mob ) )
		{
			send_to_char( "You can only clone mobiles.\n\r", ch );
			return;
		}

		if ( ( mob->level > 20 && !IS_TRUSTED( ch, GOD ) )
			 || ( mob->level > 10 && !IS_TRUSTED( ch, IMMORTAL ) )
			 || ( mob->level > 5 && !IS_TRUSTED( ch, DEMI ) )
			 || ( mob->level > 0 && !IS_TRUSTED( ch, ANGEL ) )
			 || !IS_TRUSTED( ch, AVATAR ) )
		{
			send_to_char(
				"Your powers are not great enough for such a task.\n\r", ch );
			return;
		}

		clone = create_mobile( mob->pIndexData );
		clone_mobile( mob, clone );

		for ( obj = mob->carrying; obj != NULL; obj = obj->next_content )
		{
			if ( obj_check( ch, obj ) )
			{
				/*artefact*/
				if ( is_artefact( obj ) )
				{
					act( "{RNIE MO¯NA KLONOWAÆ ARTEFAKTÓW ($n,$p) !!!{x.[@3342]", ch, clone, NULL, TO_CHAR );
					continue;
				}
				new_obj = create_object( obj->pIndexData, FALSE );
				clone_object( obj, new_obj );
				recursive_clone( ch, obj, new_obj );
				/*artefact?*/
				obj_to_char( new_obj, clone );
				new_obj->wear_loc = obj->wear_loc;
			}
		}
		char_to_room( clone, ch->in_room );
		switch ( ch->sex )
		{
			case 0:
				act( "$n stworzy³o $N.", ch, NULL, clone, TO_ROOM );
				act( "Stworzy³o¶ $N.", ch, NULL, clone, TO_CHAR );
				break;
			case 1:
				act( "$n stworzy³ $N.", ch, NULL, clone, TO_ROOM );
				act( "Stworzy³e¶ $N.", ch, NULL, clone, TO_CHAR );
				break;
			default :
				act( "$n stworzy³a $N.", ch, NULL, clone, TO_ROOM );
				act( "Stworzy³a¶ $N.", ch, NULL, clone, TO_CHAR );
				break;
		}

//		act( "$n has created $N.", ch, NULL, clone, TO_ROOM );
//		act( "You clone $N.", ch, NULL, clone, TO_CHAR );
		sprintf( buf, "$N clones %s.", clone->short_descr );
		wiznet( buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust( ch ) );
		return;
	}
}

/* RT to replace the two load commands */

void do_load( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	extern int port;

	if ( port == PORT_PLAYER && !IS_SET( ch->pcdata->wiz_conf, W5 ) )
	{
		send_to_char( "Na player-world moze to zrobiæ tylko sêdzia albo lord.\n\r", ch );
		return ;
	}

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  load mob <vnum>\n\r", ch );
		send_to_char( "  load obj <vnum> <level>\n\r", ch );
		return;
	}

	if ( !str_cmp( arg, "mob" ) || !str_cmp( arg, "char" ) )
	{
		do_function( ch, &do_mload, argument );
		return;
	}

	if ( !str_cmp( arg, "obj" ) )
	{
		do_function( ch, &do_oload, argument );
		return;
	}
	/* echo syntax */
	do_function( ch, &do_load, "" );
}

void do_mload( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	MOB_INDEX_DATA *pMobIndex;
	CHAR_DATA *victim;
	char buf[ MAX_STRING_LENGTH ];

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || !is_number( arg ) )
	{
		send_to_char( "Syntax: load mob <vnum>.\n\r", ch );
		return;
	}

	if ( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
	{
		send_to_char( "No mob has that vnum.\n\r", ch );
		return;
	}

	victim = create_mobile( pMobIndex );
	char_to_room( victim, ch->in_room );
	victim->reset_vnum = ch->in_room->vnum;

	if ( HAS_TRIGGER( victim, TRIG_ONLOAD ) )
    {
		mp_onload_trigger( victim );
    }

	if ( !IS_IMMORTAL( ch ) || !IS_SET( ch->wiz_config, WIZCFG_QUIETMODE ) )
	{
		switch ( ch->sex)
		{
			case 0:
				act( "$n stworzy³o $N!", ch, NULL, victim, TO_ROOM );
				break;
			case 1:
				act( "$n stworzy³ $N!", ch, NULL, victim, TO_ROOM );
				break;
			default:
				act( "$n stworzy³a $N!", ch, NULL, victim, TO_ROOM );
				break;
		}
	}
	sprintf( buf, "$N loads %s.", victim->short_descr );
	wiznet( buf, ch, NULL, WIZ_LOAD, WIZ_SECURE, get_trust( ch ) );
	send_to_char( "Ok.\n\r", ch );
	return;
}

void do_oload( CHAR_DATA *ch, char *argument )
{
	char arg1[ MAX_INPUT_LENGTH ] , arg2[ MAX_INPUT_LENGTH ];
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	/*	int level;*/

	argument = one_argument( argument, arg1 );
	one_argument( argument, arg2 );

	if ( arg1[ 0 ] == '\0' || !is_number( arg1 ) )
	{
		send_to_char( "Syntax: load obj <vnum> <level>.\n\r", ch );
		return;
	}

	if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
	{
		send_to_char( "No object has that vnum.\n\r", ch );
		return;
	}


	obj = create_object( pObjIndex, FALSE );
	/*artefact*/
	if ( is_artefact( obj ) && !(ch->pcdata->wiz_conf & W6) )
	{
		act( "{RNie mo¿na ³adowaæ artefaktow!{x", ch, NULL, NULL, TO_CHAR );
		extract_obj( obj );
		return;
	}
	if ( CAN_WEAR( obj, ITEM_TAKE ) )
	{
		/*artefact*/
		if ( is_artefact( obj ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
			artefact_to_char( obj, ch );
		obj_to_char( obj, ch );
	}
	else
		obj_to_room( obj, ch->in_room );

	if ( !IS_IMMORTAL( ch ) || !IS_SET( ch->wiz_config, WIZCFG_QUIETMODE ) )
	{
		if ( ch->sex == 2 )
			act( "$n stworzy³a $p!", ch, obj, NULL, TO_ROOM );
		else
			act( "$n stworzy³ $p!", ch, obj, NULL, TO_ROOM );
	}
	wiznet( "$N loads $p.", ch, obj, WIZ_LOAD, WIZ_SECURE, get_trust( ch ) );

    if( obj->item_type == ITEM_SPELLBOOK)
    {
        append_file_format_daily
            (
             ch,
             BUILD_LOG_FILE,
             "%s loads spellbook %s[%d].",
             ch->name,
             obj->short_descr,
             obj->pIndexData->vnum
            );
    }

	if ( HAS_OTRIGGER( obj, TRIG_ONLOAD ) )
		op_onload_trigger( obj );

	send_to_char( "Ok.\n\r", ch );
	return;
}

void do_purge( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	char buf[ 100 ];
	CHAR_DATA *victim;
	OBJ_DATA *obj = NULL, *objx = NULL;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		/* 'purge' */
		CHAR_DATA * vnext;
		OBJ_DATA *obj_next;

		for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
		{
			vnext = victim->next_in_room;
			if ( IS_NPC( victim ) && !EXT_IS_SET( victim->act, ACT_NOPURGE )
				 && victim != ch /* safety precaution */ )
			{
				/*artefact*/
				for ( obj = victim->carrying;obj != NULL;obj = objx )
				{
					objx = obj->next_content;
					if ( is_artefact( obj ) ) extract_artefact( obj );
					if ( obj->contains ) extract_artefact_container( obj );
				}
				DEBUG_INFO( "do_purge:extracting_purge_everyone:pre" );
				extract_char( victim, TRUE );
				DEBUG_INFO( "do_purge:extracting_purge_everyone:post" );
			}
		}

		for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;
			if ( !IS_OBJ_STAT( obj, ITEM_NOPURGE ) )
			{
				/*artefact*/
				if ( is_artefact( obj ) ) extract_artefact( obj );
				if ( obj->contains ) extract_artefact_container( obj );
				extract_obj( obj );
			}
		}

		if ( !IS_IMMORTAL( ch ) || !IS_SET( ch->wiz_config, WIZCFG_QUIETMODE ) )
			act( "$n sprawia, ¿e okolica wraca do swojego pierwotnego wygl±du!", ch, NULL, NULL, TO_ROOM );

		send_to_char( "Ok.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( !IS_NPC( victim ) )
	{

		if ( ch == victim )
		{
			send_to_char( "Ho ho ho.\n\r", ch );
			return;
		}

		if ( get_trust( ch ) <= get_trust( victim ) )
		{
			send_to_char( "Maybe that wasn't a good idea...\n\r", ch );
			sprintf( buf, "%s tried to purge you!\n\r", ch->name );
			send_to_char( buf, victim );
			return;
		}

		if ( !IS_IMMORTAL( ch ) || !IS_SET( ch->wiz_config, WIZCFG_QUIETMODE ) )
			act( "$n unicestwia $N.", ch, 0, victim, TO_NOTVICT );

		if ( victim->level > 1 )
			save_char_obj( victim, FALSE, FALSE );

		extract_char( victim, TRUE );

		return;
	}

	if ( !IS_IMMORTAL( ch ) || !IS_SET( ch->wiz_config, WIZCFG_QUIETMODE ) )
		act( "$n unicestwia $N.", ch, NULL, victim, TO_NOTVICT );

	extract_char( victim, TRUE );
	return;
}

void do_advance( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;
	int level;
	int iLevel;
	int zm;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || !is_number( arg2 ) )
	{
		send_to_char( "Syntax: advance <char> <level>.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	{
		send_to_char( "That player is not here.\n\r", ch );
		return;
	}

	if ( IS_NPC( victim ) )
	{
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( ( level = atoi( arg2 ) ) < 1 || level > MAX_LEVEL )
	{
		sprintf( buf, "Level must be 1 to %d.\n\r", MAX_LEVEL );
		send_to_char( buf, ch );
		return;
	}

	if ( victim->level > ch->level )
	{
		send_to_char( "Nie dasz rady!\n\r", ch );
		return;
	}

	if ( level > get_trust( ch ) )
	{
		send_to_char( "Masz za niski poziom zaufania(trust).\n\r", ch );
		return;
	}

	if ( level <= victim->level )
	{
		send_to_char( "Obnizanie poziomu gracza!\n\r", ch );
		for ( zm = victim->level - level;zm > 0;zm-- )
		{
			delevel( victim, FALSE );
			victim->exp = exp_per_level( victim, victim->level - 1 );
		}
		return;
	}
	else
	{
		send_to_char( "Raising a player's level!\n\r", ch );
		send_to_char( "**** OOOO  TTTTAAAAKKKK ****\n\r", victim );
	}

	for ( iLevel = victim->level ; iLevel < level; iLevel++ )
	{
		advance_level( victim, TRUE );
	}

	sprintf( buf, "Masz teraz poziom %d.\n\r", victim->level );
	send_to_char( buf, victim );
	victim->exp = exp_per_level( victim, victim->level - 1 );

	victim->trust = 0;

	if ( victim->level < 2 )
	{
		sprintf( buf, "%s%s", PLAYER_DIR, capitalize( victim->name ) );
		unlink( buf );
	}

	save_char_obj( victim, FALSE, FALSE );
	return;
}

void do_trust( CHAR_DATA *ch, char *argument )
{
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char buf[ MAX_STRING_LENGTH ];
	CHAR_DATA *victim;
	int level;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || !is_number( arg2 ) )
	{
		send_to_char( "Syntax: trust <char> <level>.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	{
		send_to_char( "That player is not here.\n\r", ch );
		return;
	}

	if ( ( level = atoi( arg2 ) ) < 0 || level > MAX_LEVEL )
	{
		sprintf( buf, "Level must be 0 (reset) or 1 to %d.\n\r", MAX_LEVEL );
		send_to_char( buf, ch );
		return;
	}

	if ( level > get_trust( ch ) )
	{
		send_to_char( "Limited to your trust.\n\r", ch );
		return;
	}

	victim->trust = level;
	return;
}

void do_restore( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ], buf[ MAX_STRING_LENGTH ];
	CHAR_DATA *victim;
	CHAR_DATA *vch;
	DESCRIPTOR_DATA *d;

	one_argument( argument, arg );
	if ( arg[ 0 ] == '\0' || !str_cmp( arg, "room" ) )
	{
		/* cure room */

		for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
		{
			affect_strip( vch, gsn_plague );
			affect_strip( vch, gsn_poison );
			affect_strip( vch, gsn_blindness );
			affect_strip( vch, gsn_sleep );
			affect_strip( vch, gsn_curse );
			affect_strip( vch, gsn_feeblemind );
			affect_strip( vch, gsn_energy_drain );

			vch->hit = get_max_hp(vch);
			vch->move	= vch->max_move;

			if ( !IS_NPC (vch) && !IS_IMMORTAL(vch) )
			{
			    vch->condition[ COND_HUNGER ] = EAT_FULL;
			    vch->condition[ COND_THIRST ] = DRINK_FULL;
			    vch->condition[ COND_SLEEPY ] = 48;
			}

			mem_done_all( vch );

			update_pos( vch );
			if ( ch->sex == 2 )
				act( "$n uzdrowila cie.", ch, NULL, vch, TO_VICT );
			else
				act( "$n uzdrowil cie.", ch, NULL, vch, TO_VICT );
		}

		sprintf( buf, "$N restored room %d.", ch->in_room->vnum );
		wiznet( buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust( ch ) );

		send_to_char( "Room restored.\n\r", ch );
		return;

	}

	if ( get_trust( ch ) >= MAX_LEVEL - 1 && !str_cmp( arg, "all" ) )
	{
		/* cure all */

		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( !d->character || d->connected < 0 ) continue;

			victim = d->character;

			if ( victim == NULL || IS_NPC( victim ) )
				continue;

			affect_strip( victim, gsn_plague );
			affect_strip( victim, gsn_poison );
			affect_strip( victim, gsn_blindness );
			affect_strip( victim, gsn_sleep );
			affect_strip( victim, gsn_curse );
			affect_strip( victim, gsn_feeblemind );
			affect_strip( victim, gsn_energy_drain );

			victim->hit = get_max_hp(victim);
			victim->move	= victim->max_move;

			mem_done_all( victim );
			update_pos( victim );

			if ( victim->in_room != NULL )
			{
				if ( ch->sex == 2 )
					act( "$n uzdrowila cie.", ch, NULL, victim, TO_VICT );
				else
					act( "$n uzdrowil cie.", ch, NULL, victim, TO_VICT );
			}
		}
		send_to_char( "All active players restored.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	affect_strip( victim, gsn_plague );
	affect_strip( victim, gsn_poison );
	affect_strip( victim, gsn_blindness );
	affect_strip( victim, gsn_sleep );
	affect_strip( victim, gsn_curse );
	affect_strip( victim, gsn_feeblemind );
	affect_strip( victim, gsn_energy_drain );
	victim->hit = get_max_hp(victim);
	victim->move = victim->max_move;

    if ( !IS_NPC (victim) && !IS_IMMORTAL(victim) )
    {
        victim->condition[ COND_HUNGER ] = EAT_FULL;
        victim->condition[ COND_THIRST ] = DRINK_FULL;
        victim->condition[ COND_SLEEPY ] = 48;
    }

	mem_done_all( victim );

	update_pos( victim );
	act( "$n has restored you.", ch, NULL, victim, TO_VICT );
	sprintf( buf, "$N restored %s",
			 IS_NPC( victim ) ? victim->short_descr : victim->name );
	wiznet( buf, ch, NULL, WIZ_RESTORE, WIZ_SECURE, get_trust( ch ) );
	send_to_char( "Ok.\n\r", ch );
	return;
}

void do_freeze( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ], buf[ MAX_STRING_LENGTH ];
	CHAR_DATA *victim;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Freeze whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC( victim ) )
	{
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
		send_to_char( "Nie udalo ci sie.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( victim->act, PLR_FREEZE ) )
	{
		EXT_REMOVE_BIT( victim->act, PLR_FREEZE );
		send_to_char( "Mozesz znow grac.\n\r", victim );
		send_to_char( "FREEZE removed.\n\r", ch );
		sprintf( buf, "$N thaws %s.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}
	else
	{
		EXT_SET_BIT( victim->act, PLR_FREEZE );
		send_to_char( "Twoja postac zostala 'zamrozona'!\n\r", victim );
		send_to_char( "FREEZE set.\n\r", ch );
		sprintf( buf, "$N puts %s in the deep freeze.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	save_char_obj( victim, FALSE, FALSE );

	return;
}

void do_log( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Log whom?\n\r", ch );
		return;
	}

	if ( !str_cmp( arg, "all" ) )
	{
		if ( fLogAll )
		{
			fLogAll = FALSE;
			send_to_char( "Log ALL off.\n\r", ch );
		}
		else
		{
			fLogAll = TRUE;
			send_to_char( "Log ALL on.\n\r", ch );
		}
		return;
	}

	if ( !str_cmp( arg, "comm" ) )
	{
		if ( fLogComm )
		{
			fLogComm = FALSE;
			send_to_char( "Logowanie komend s³u¿±cych do komunikacji wy³±czone.\n\r", ch );
		}
		else
		{
			fLogComm = TRUE;
			send_to_char( "Logowanie komend s³u¿±cych do komunikacji w³±czone.\n\r", ch );
		}
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC( victim ) )
	{
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	}

	/*
		* No level check, gods can log anyone.
		*/
	if ( EXT_IS_SET( victim->act, PLR_LOG ) )
	{
		EXT_REMOVE_BIT( victim->act, PLR_LOG );
		send_to_char( "LOG removed.\n\r", ch );
	}
	else
	{
		EXT_SET_BIT( victim->act, PLR_LOG );
		send_to_char( "LOG set.\n\r", ch );
	}

	return;
}

void do_noemote( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ], buf[ MAX_STRING_LENGTH ];
	CHAR_DATA *victim;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Noemote whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}


	if ( get_trust( victim ) >= get_trust( ch ) )
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET( victim->comm, COMM_NOEMOTE ) )
	{
		REMOVE_BIT( victim->comm, COMM_NOEMOTE );
		send_to_char( "Znow mozesz okazywac swoje emocje.\n\r", victim );
		send_to_char( "NOEMOTE removed.\n\r", ch );
		sprintf( buf, "$N restores emotes to %s.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}
	else
	{
		SET_BIT( victim->comm, COMM_NOEMOTE );
		send_to_char( "Nie mozesz okazywac emocji!\n\r", victim );
		send_to_char( "NOEMOTE set.\n\r", ch );
		sprintf( buf, "$N revokes %s's emotes.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	return;
}

void do_noshout( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ], buf[ MAX_STRING_LENGTH ];
	CHAR_DATA *victim;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Noshout whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC( victim ) )
	{
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	if ( IS_SET( victim->comm, COMM_NOSHOUT ) )
	{
		REMOVE_BIT( victim->comm, COMM_NOSHOUT );
		send_to_char( "Znow mozesz krzyczec.\n\r", victim );
		send_to_char( "NOSHOUT removed.\n\r", ch );
		sprintf( buf, "$N restores shouts to %s.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}
	else
	{
		SET_BIT( victim->comm, COMM_NOSHOUT );
		send_to_char( "Nie mozesz krzyczec!\n\r", victim );
		send_to_char( "NOSHOUT set.\n\r", ch );
		sprintf( buf, "$N revokes %s's shouts.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	return;
}

void do_blocktell( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ], buf[ MAX_STRING_LENGTH ];
	CHAR_DATA *victim;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Komu chcesz zablokowaæ kana³ 'tell'?", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie ma nikogo takiego na ca³ym ¶wiecie!\n\r", ch );
		return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
		send_to_char( "Nie mo¿esz tego zrobiæ.\n\r", ch );
		return;
	}

	if ( IS_SET( victim->comm, COMM_NOTELL ) )
	{
		REMOVE_BIT( victim->comm, COMM_NOTELL );
		send_to_char( "Znow mozesz uzywac 'tell'.\n\r", victim );
		send_to_char( "BLOCKTELL zdjête.\n\r", ch );
		sprintf( buf, "$N {Gzdejmuje blokadê{x u¿ywania 'tell' przez: %s.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}
	else
	{
		SET_BIT( victim->comm, COMM_NOTELL );
		send_to_char( "Nie mozesz uzywac 'tell'!\n\r", victim );
		send_to_char( "BLOCKTELL ustawione.\n\r", ch );
		sprintf( buf, "$N {Rblokuje{x u¿ywanie 'tell' przez: %s.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	return;
}

//blokowanie kana³u newbie
void do_blocknewbie( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ], buf[ MAX_STRING_LENGTH ];
	CHAR_DATA *victim;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Komu chcesz zablokowaæ kana³ 'newbie'?", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie ma nikogo takiego na ca³ym ¶wiecie!\n\r", ch );
		return;
	}

	if ( get_trust( victim ) >= get_trust( ch ) )
	{
		send_to_char( "Nie mo¿esz tego zrobiæ.\n\r", ch );
		return;
	}

	if ( IS_SET( victim->comm, COMM_NEWBIE ) )
	{
		REMOVE_BIT( victim->comm, COMM_NEWBIE );
		send_to_char( "Nie mozesz uzywac 'newbie'!\n\r", victim );
		send_to_char( "BLOCNEWBIE ustawione.\n\r", ch );
		sprintf( buf, "$N {Rblokuje{x u¿ywanie 'newbie' przez: %s.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}
	else
	{
		SET_BIT( victim->comm, COMM_NEWBIE );
		send_to_char( "Znow mozesz uzywac 'newbie'.\n\r", victim );
		send_to_char( "BLOCKNEWBIE zdjête.\n\r", ch );
		sprintf( buf, "$N {Gzdejmuje blokadê{x u¿ywania 'newbie' przez: %s.", victim->name );
		wiznet( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
	}

	return;
}

void do_peace( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * rch;

	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( rch->fighting != NULL )
			stop_fighting( rch, TRUE );
		if ( IS_NPC( rch ) && EXT_IS_SET( rch->act, ACT_AGGRESSIVE ) )
			EXT_REMOVE_BIT( rch->act, ACT_AGGRESSIVE );

		stop_hating( rch, NULL, TRUE );
		stop_hunting( rch );
		stop_fearing( rch, NULL, TRUE );
	}

	send_to_char( "Ok.\n\r", ch );
	return;
}

void do_wizlock( CHAR_DATA *ch, char *argument )
{
	extern bool wizlock;
	wizlock = !wizlock;

	if ( wizlock )
	{
		wiznet( "$N has wizlocked the game.", ch, NULL, 0, 0, 0 );
		send_to_char( "Game wizlocked.\n\r", ch );
	}
	else
	{
		wiznet( "$N removes wizlock.", ch, NULL, 0, 0, 0 );
		send_to_char( "Game un-wizlocked.\n\r", ch );
	}

	return;
}

/* RT anti-newbie code */

void do_newlock( CHAR_DATA *ch, char *argument )
{
	extern bool newlock;
	newlock = !newlock;

	if ( newlock )
	{
		wiznet( "$N locks out new characters.", ch, NULL, 0, 0, 0 );
		send_to_char( "New characters have been locked out.\n\r", ch );
	}
	else
	{
		wiznet( "$N allows new characters back in.", ch, NULL, 0, 0, 0 );
		send_to_char( "Newlock removed.\n\r", ch );
	}

	return;
}

void do_slookup( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	char prof[ MAX_STRING_LENGTH ];
	char school[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	int sn, i;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Lookup which skill or spell?\n\r", ch );
		return;
	}

	if ( !str_cmp( arg, "all" ) )
	{
		for ( sn = 0; sn < MAX_SKILL; sn++ )
		{
			if ( skill_table[ sn ].name == NULL )
				break;

			prof[ 0 ] = '\0';
			for ( i = 0; i < MAX_CLASS; i++ )
			{
				if ( skill_table[ sn ].skill_level[ i ] < 32 )
				{
					strcat( prof, class_table[ i ].name );
					strcat( prof, " " );
				}
			}

			school[ 0 ] = '\0';
			for ( i = 0; i < MAX_SCHOOL; i++ )
			{
				if ( school_table[ i ].flag == skill_table[ sn ].school )
				{
					strcat( school, school_table[ i ].name );
					break;
				}
			}

			if ( skill_table[ sn ].spell_fun == spell_null )
				sprintf( buf, "Sn: {C%3d{x %s: {C%-31s{x  Profesje: {C%s{x\n\r",
						 sn,
						 skill_table[ sn ].spell_fun == spell_null ? "Skill" : "Spell",
						 skill_table[ sn ].name,
						 prof );
			else
				sprintf( buf, "Sn: {C%3d{x %s: {C%-31s{x  Szko³a: {C%-12s{x  Profesje: {C%s{x\n\r",
						 sn,
						 skill_table[ sn ].spell_fun == spell_null ? "Skill" : "Spell",
						 skill_table[ sn ].name,
						 school,
						 prof );
			send_to_char( buf, ch );
		}
	}
	else
	{
		if ( ( sn = skill_lookup( arg ) ) < 0 )
		{
			send_to_char( "Nie ma takiego skilla ani spella.\n\r", ch );
			return;
		}

		prof[ 0 ] = '\0';
		for ( i = 0; i < MAX_CLASS; i++ )
		{
			if ( skill_table[ sn ].skill_level[ i ] < 32 )
			{
				strcat( prof, class_table[ i ].name );
				strcat( prof, " " );
			}

		}

		school[ 0 ] = '\0';
		for ( i = 0; i < MAX_SCHOOL; i++ )
		{
			if ( school_table[ i ].flag == skill_table[ sn ].school )
			{
				strcat( school, school_table[ i ].name );
				break;
			}
		}

		if ( skill_table[ sn ].spell_fun == spell_null )
			sprintf( buf, "Sn: {C%3d{x %s: {C%-31s{x  Profesje: {C%s{x\n\r",
					 sn,
					 skill_table[ sn ].spell_fun == spell_null ? "Skill" : "Spell",
					 skill_table[ sn ].name,
					 prof );
		else
			sprintf( buf, "Sn: {C%3d{x %s: {C%-31s{x  Szko³a: {C%-12s{x  Profesje: {C%s{x\n\r",
					 sn,
					 skill_table[ sn ].spell_fun == spell_null ? "Skill" : "Spell",
					 skill_table[ sn ].name,
					 school,
					 prof );
		send_to_char( buf, ch );
		sprintf( buf, "%s '%s'", skill_table[ sn ].spell_fun == spell_null ? "skill" : "spell", arg );
		do_function( ch, &do_lstat, buf );
	}

	return;
}

//rellik: komponenty, ustawianie wiedzy o komponencie
void do_siset( CHAR_DATA *ch, char *argument )
{
	char arg1 [ MAX_INPUT_LENGTH ];
	char arg2 [ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;
	int i, nr;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);


	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char("Syntax:\n\r", ch);
		send_to_char("  set SIKnow <name> <nr slotu>\n\r", ch );
		send_to_char("  set SIKnow <name> all\n\r", ch );
		send_to_char("  <nr slotu> z tabeli wy¶wietlanej komend± SITable.\n\r", ch);
		send_to_char("  Zamienia wiedzê o komponencie <nr slotu> dla <name> lub ustawia wiedzê o wszystkich.\n\r", ch );
		return;
	}

	if ( (victim = get_char_world(ch, arg1) ) == NULL)
	{
		send_to_char("They aren't here.\n\r", ch);
		return;
	}

	if ( IS_NPC( victim ))
	{
		send_to_char("Not on NPC's.\n\r", ch);
		return;
	}

	if ( !str_cmp(arg2, "all") )
	{
		for( i=0; i < BIGFLAG_CAPACITY; ++i )
		{
			//TODO: (rellik#) czy da to 11111... ?
			victim->pcdata->spell_items_knowledge.bank[i] = -1;
		}
		print_char( ch, "%s ma teraz wiedzê o wszystkich mo¿liwych komponentach.", victim->name );
		print_char( ch, "Nawet o tych, których jeszcze nie ma.\n\r" );
		return;
	}

	if ( !is_number(arg2) )
	{
		send_to_char("<nr slotu> musi byæ warto¶ci± liczbow±.\n\r", ch);
		return;
	}

	nr = atoi(arg2);
	//nie mo¿e mniejsze ni¿ zero i nie mo¿e przekroczyæ indeksu tabeli
	if ( nr < 0 || nr > spell_items_table_count() )
	{
		print_char( ch, "Warto¶æ <nr slotu> spoza dopuszczalnego zakresu od 0 do %d.\n\r", spell_items_table_count() );
		return;
	}

	tgl_bigflag( &victim->pcdata->spell_items_knowledge, spell_items_table[nr].key_number );

	if ( chk_bigflag( &victim->pcdata->spell_items_knowledge, spell_items_table[nr].key_number ) )
	{
		print_char( ch, "%s ma teraz wiedzê o komponencie %s, który s³u¿y do czaru %s (slot %d, key %d).\n\r", victim->name, spell_items_table[nr].official_spell_item_name, spell_items_table[nr].spell_name, nr, spell_items_table[nr].key_number );
	} else {
		print_char( ch, "%s utraci³ teraz wiedzê o komponencie %s, który s³u¿y do czaru %s (slot %d, key %d).\n\r", victim->name, spell_items_table[nr].official_spell_item_name, spell_items_table[nr].spell_name, nr, spell_items_table[nr].key_number );
	}

	return;
}

/* RT set replaces sset, mset, oset, and rset */

void do_set( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  set character <name> <field> <value>\n\r", ch );
		send_to_char( "  set lang      <name> <lang> <value>\n\r", ch );
		send_to_char( "  set mob       <name> <field> <value>\n\r", ch );
		send_to_char( "  set obj       <name> <field> <value>\n\r", ch );
		send_to_char( "  set room      <room> <field> <value>\n\r", ch );
		send_to_char( "  set skill     <name> <spell or skill> <value>\n\r", ch );
		//rellik: komponenty, ustawianie wiedzy o komponencie
		send_to_char("  set SIKnow    <name> <nr slotu>\n\r", ch);
		send_to_char("  <nr slotu> - z tabeli wy¶wietlanej komend± SITable.\n\r", ch);
		return;
	}

	if ( !str_prefix( arg, "mobile" ) || !str_prefix( arg, "character" ) )
	{
		do_function( ch, &do_mset, argument );
		return;
	}

	if ( !str_prefix( arg, "skill" ) || !str_prefix( arg, "spell" ) )
	{
		do_function( ch, &do_sset, argument );
		return;
	}

	//rellik: komponenty, ustawianie wiedzy o komponencie
	if ( !str_prefix(arg, "SIKnow") )
	{
		do_siset( ch, argument );
		return;
	}

	if ( !str_prefix( arg, "language" ) )
	{
		do_function( ch, &do_langset, argument );
		return;
	}

	if ( !str_prefix( arg, "object" ) )
	{
		do_function( ch, &do_oset, argument );
		return;
	}

	if ( !str_prefix( arg, "room" ) )
	{
		do_function( ch, &do_rset, argument );
		return;
	}
	/* echo syntax */
	do_function( ch, &do_set, "" );
}

void do_sset( CHAR_DATA *ch, char *argument )
{
	char arg1 [ MAX_INPUT_LENGTH ];
	char arg2 [ MAX_INPUT_LENGTH ];
	char arg3 [ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;
	int value;
	int sn;
	bool fAll;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || arg3[ 0 ] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  set skill <name> <spell or skill> <value>\n\r", ch );
		send_to_char( "  set skill <name> all <value>\n\r", ch );
		send_to_char( "   (use the name of the skill, not the number)\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC( victim ) )
	{
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	}

	fAll = !str_cmp( arg2, "all" );
	sn = 0;
	if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
	{
		send_to_char( "No such skill or spell.\n\r", ch );
		return;
	}

	/*
		* Snarf the value.
		*/
	if ( !is_number( arg3 ) )
	{
		send_to_char( "Value must be numeric.\n\r", ch );
		return;
	}

	value = atoi( arg3 );
	if ( value < 0 || value > 100 )
	{
		send_to_char( "Value range is 0 to 100.\n\r", ch );
		return;
	}

	if ( fAll )
	{
		for ( sn = 0; sn < MAX_SKILL; sn++ )
		{
			if ( skill_table[ sn ].name != NULL )
				victim->pcdata->learned[ sn ] = value;

			if ( value == 0 ){
                 
				victim->pcdata->learning[ sn ] = value;
				victim->pcdata->learning_rasz[ sn ] = value;
            }
		}
	}
	else
	{
		victim->pcdata->learned[ sn ] = value;
	}

	return;
}

void do_langset( CHAR_DATA *ch, char *argument )
{
	char arg1 [ MAX_INPUT_LENGTH ];
	char arg2 [ MAX_INPUT_LENGTH ];
	char arg3 [ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;
	int value;
	int sn;
	bool fAll;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || arg3[ 0 ] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  set lang <char> <name> <value>\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( IS_NPC( victim ) )
	{
		send_to_char( "Not on NPC's.\n\r", ch );
		return;
	}

	fAll = !str_cmp( arg2, "all" );
	sn = 0;
	if ( !fAll && ( sn = get_langnum( arg2 ) ) < 0 )
	{
		send_to_char( "No such language.\n\r", ch );
		return;
	}

	/*
		* Snarf the value.
		*/
	if ( !is_number( arg3 ) )
	{
		send_to_char( "Value must be numeric.\n\r", ch );
		return;
	}

	value = atoi( arg3 );
	if ( value < 0 || value > 100 )
	{
		send_to_char( "Value range is 0 to 100.\n\r", ch );
		return;
	}

	if ( fAll )
	{
		for ( sn = 0; sn < MAX_LANG; sn++ )
		{
			if ( lang_table[ sn ].bit != LANG_UNKNOWN )
				victim->pcdata->language[ sn ] = value;
		}
	}
	else
	{
		victim->pcdata->language[ sn ] = value;
	}

	return;
}

void do_mset( CHAR_DATA *ch, char *argument )
{
    char arg1 [ MAX_INPUT_LENGTH ];
    char arg2 [ MAX_INPUT_LENGTH ];
    char arg3 [ MAX_INPUT_LENGTH ];
    char buf[ 100 ];
    CHAR_DATA *victim;
    int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || arg3[ 0 ] == '\0' )
    {
        send_to_char( "Syntax:\n\r", ch );
        send_to_char( "  set char <name> <field> <value>\n\r", ch );
        send_to_char( "  Field being one of:\n\r", ch );
        send_to_char( "        str int wis dex con luc cha\n\r", ch );
        send_to_char( "        sex class spec level align\n\r", ch );
        send_to_char( "        race group hp move\n\r", ch );
        send_to_char( "        thirst hunger drunk full sleepy\n\r", ch );
        send_to_char( "        security wait\n\r", ch );
        send_to_char( "        copper silver gold mithril bank glory\n\r", ch );
        send_to_char( "        addpoison rempoison\n\r", ch );

        return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    /* clear zones for mobs */
    victim->zone = NULL;

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "str" ) )
    {
        if ( value < 20 || value > MAX_STAT_VALUE )
        {
            sprintf( buf, "Strength range is 20 to %d\n\r.", MAX_STAT_VALUE );
            send_to_char( buf, ch );
            return;
        }
        victim->perm_stat[ STAT_STR ] = value;
        return;
    }

    if ( !str_cmp( arg2, "security" ) )    	/* OLC */
    {
        if ( IS_NPC( ch ) )
        {
            send_to_char( "Si, claro.\n\r", ch );
            return;
        }

        if ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 ){ return;}//nie mozna z proga


        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value > ch->pcdata->security || value < 0 )
        {
            if ( ch->pcdata->security != 0 )
            {
                sprintf( buf, "Valid security is 0-%d.\n\r",
                        ch->pcdata->security );
                send_to_char( buf, ch );
            }
            else
            {
                send_to_char( "Valid security is 0 only.\n\r", ch );
            }
            return;
        }
        victim->pcdata->security = value;
        return;
    }

    if ( !str_cmp( arg2, "int" ) )
    {
        if ( value < 3 || value > MAX_STAT_VALUE )
        {
            sprintf( buf, "Intelligence range is 3 to %d.\n\r", MAX_STAT_VALUE );
            send_to_char( buf, ch );
            return;
        }
        victim->perm_stat[ STAT_INT ] = value;
        sprintf( buf, "Twoja int :%d\n\r", victim->perm_stat[ STAT_INT ] );
        send_to_char( buf, ch );
        return;
    }

    if ( !str_cmp( arg2, "wis" ) )
    {
        if ( value < 3 || value > MAX_STAT_VALUE )
        {
            sprintf( buf, "Wisdom range is 3 to %d.\n\r", MAX_STAT_VALUE ); send_to_char( buf, ch );
            return;
        }
        victim->perm_stat[ STAT_WIS ] = value;
        return;
    }

    if ( !str_cmp( arg2, "dex" ) )
    {
        if ( value < 3 || value > MAX_STAT_VALUE )
        {
            sprintf( buf, "Dexterity range is 3 to %d.\n\r", MAX_STAT_VALUE );
            send_to_char( buf, ch );
            return;
        }

        victim->perm_stat[ STAT_DEX ] = value;
        return;
    }

    if ( !str_cmp( arg2, "con" ) )
    {
        if ( value < 3 || value > MAX_STAT_VALUE )
        {
            sprintf( buf, "Constitution range is 3 to %d.\n\r", MAX_STAT_VALUE );
            send_to_char( buf, ch );
            return;
        }

        victim->perm_stat[ STAT_CON ] = value;
        return;
    }

    if ( !str_cmp( arg2, "cha" ) )
    {
        if ( value < 3 || value > MAX_STAT_VALUE )
        {
            sprintf( buf, "Charisma range is 3 to %d.\n\r", MAX_STAT_VALUE );
            send_to_char( buf, ch );
            return;
        }
        victim->perm_stat[ STAT_CHA ] = value;
        return;
    }

    if ( !str_cmp( arg2, "luc" ) )
    {
        if ( value < 3 || value > MAX_STAT_VALUE )
        {
            sprintf( buf, "Luck range is 3 to %d.\n\r", MAX_STAT_VALUE );
            send_to_char( buf, ch );
            return;
        }
        victim->perm_stat[ STAT_LUC ] = value;
        return;
    }

    if ( !str_prefix( arg2, "sex" ) )
    {
        if ( value < 0 || value > 2 )
        {
            send_to_char( "Sex range is 0 to 2.\n\r", ch );
            return;
        }
        victim->sex = value;
        if ( !IS_NPC( victim ) )
            victim->pcdata->true_sex = value;
        return;
    }

    if ( !str_prefix( arg2, "class" ) )
    {
        int class;

        if ( IS_NPC( victim ) )
        {
            send_to_char( "Mobiles have no class.\n\r", ch );
            return;
        }

        class = class_lookup( arg3 );
        if ( class == -1 )
        {
            char buf[ MAX_STRING_LENGTH ];

            strcpy( buf, "Possible classes are: " );
            for ( class = 0; class < MAX_CLASS; class++ )
            {
                if ( class > 0 )
                    strcat( buf, " " );
                strcat( buf, class_table[ class ].name );
            }
            strcat( buf, ".\n\r" );

            send_to_char( buf, ch );
            return;
        }

        victim->class = class;
        return;
    }

    if ( !str_prefix( arg2, "level" ) )
    {
        if ( !IS_NPC( victim ) )
        {
            send_to_char( "Not on PC's.\n\r", ch );
            return;
        }

        if ( value < 0 || value > MAX_LEVEL )
        {
            sprintf( buf, "Level range is 0 to %d.\n\r", MAX_LEVEL );
            send_to_char( buf, ch );
            return;
        }
        victim->level = value;
        return;
    }

    if ( !str_prefix( arg2, "copper" ) )
    {
        victim->copper = UMAX( 0, value );
        return;
    }

    if ( !str_prefix( arg2, "silver" ) )
    {
        victim->silver = UMAX( 0, value );
        return;
    }

    if ( !str_prefix( arg2, "gold" ) )
    {
        victim->gold = UMAX( 0, value );
        return;
    }

    if ( !str_prefix( arg2, "mithril" ) )
    {
        victim->mithril = UMAX( 0, value );
        return;
    }

    if ( !str_prefix( arg2, "bank" ) )
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Moby na killerze nie mog± deponowaæ kasy w banku ;-)\n\r", ch );
            return;
        }
        victim->bank = UMAX( 0, value );
        return;
    }

    if ( !str_prefix( arg2, "wait" ) )
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Mobiles have no wait.\n\r", ch );
            return;
        }
        victim->wait = UMAX(0, value);
        return;
    }

    if ( !str_prefix( arg2, "hp" ) )
    {
        if ( value < -10 || value > 30000 )
        {
            send_to_char( "Hp range is -10 to 30,000 hit points.\n\r", ch );
            return;
        }
        victim->max_hit = value;
        if ( !IS_NPC( victim ) )
            victim->pcdata->perm_hit = value;
        return;
    }

    if ( !str_prefix( arg2, "move" ) )
    {
        if ( value < 0 || value > 30000 )
        {
            send_to_char( "Move range is 0 to 30,000 move points.\n\r", ch );
            return;
        }
        victim->max_move = value;
        if ( !IS_NPC( victim ) )
            victim->pcdata->perm_move = value;
        return;
    }

    /*	if ( !str_prefix( arg2, "practice" ) )
        {
        if ( value < 0 || value > 250 )
        {
        send_to_char( "Practice range is 0 to 250 sessions.\n\r", ch );
        return;
        }
        victim->practice = value;
        return;
        }

        if ( !str_prefix( arg2, "train" ))
        {
        if (value < 0 || value > 50 )
        {
        send_to_char("Training session range is 0 to 50 sessions.\n\r",ch);
        return;
        }
        victim->train = value;
        return;
        }*/

    if ( !str_prefix( arg2, "align" ) )
    {
        if ( value < -1000 || value > 1000 )
        {
            send_to_char( "Alignment range is -1000 to 1000.\n\r", ch );
            return;
        }
        victim->alignment = value;
        return;
    }

    if ( !str_prefix( arg2, "thirst" ) )
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value < -1 || value > 100 )
        {
            send_to_char( "Thirst range is -1 to 100.\n\r", ch );
            return;
        }

        victim->condition[ COND_THIRST ] = value;
        return;
    }

    if ( !str_prefix( arg2, "sleepy" ) )
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value < -1 || value > 100 )
        {
            send_to_char( "Thirst range is -1 to 100.\n\r", ch );
            return;
        }

        victim->condition[ COND_SLEEPY ] = value;
        return;
    }

    if ( !str_prefix( arg2, "drunk" ) )
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value < -1 || value > 100 )
        {
            send_to_char( "Drunk range is -1 to 100.\n\r", ch );
            return;
        }

        victim->condition[ COND_DRUNK ] = value;
        return;
    }

    if ( !str_prefix( arg2, "full" ) )
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value < -1 || value > 100 )
        {
            send_to_char( "Full range is -1 to 100.\n\r", ch );
            return;
        }

        victim->condition[ COND_FULL ] = value;
        return;
    }

    if ( !str_prefix( arg2, "hunger" ) )
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value < -1 || value > 100 )
        {
            send_to_char( "Full range is -1 to 100.\n\r", ch );
            return;
        }

        victim->condition[ COND_HUNGER ] = value;
        return;
    }

    if ( !str_prefix( arg2, "race" ) )
    {
        int race;

        race = race_lookup( arg3 );

        if ( race == 0 )
        {
            send_to_char( "That is not a valid race.\n\r", ch );
            return;
        }

        if ( !IS_NPC( victim ) && !race_table[ race ].pc_race )
        {
            send_to_char( "That is not a valid player race.\n\r", ch );
            return;
        }

        SET_RACE( victim, race );
        return;
    }

    if ( !str_prefix( arg2, "specialist" ) )
    {
        int spec = -1, school;

        if ( IS_NPC( victim ) )
        {
            send_to_char( "Tylko dla graczy.\n\r", ch );
            return;
        }

        if ( victim->class != CLASS_MAG )
        {
            send_to_char( "Tylko dla graczy magow.\n\r", ch );
            return;
        }

        if ( !str_cmp( arg3, "none" ) )
        {
            send_to_char( "Usunieto specjalizacje.\n\r", ch );
            victim->pcdata->mage_specialist = -1;
            return;
        }

        for ( school = 0; school < MAX_SCHOOL; school++ )
            if ( !str_prefix( arg3, school_table[ school ].name ) )
            {
                spec = school;
                break;
            }

        if ( school == -1 )
        {
            send_to_char( "Nie ma takiej specjalizacji.\n\r", ch );
            return;
        }
        victim->pcdata->mage_specialist = spec;
        return;
    }


    if ( !str_prefix( arg2, "group" ) )
    {
        if ( !IS_NPC( victim ) )
        {
            send_to_char( "Only on NPCs.\n\r", ch );
            return;
        }
        victim->group = value;
        return;
    }

    if ( !str_prefix( arg2, "glory" ) )
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

        victim->glory = value;
        return;
    }

    if ( !str_prefix( arg2, "addpo" ) )
    {
        poison_to_char(ch, (value == -1)? 1:value);
        return;
    }

    if ( !str_prefix( arg2, "rempo" ) )
    {
        affect_strip_partial(ch, gsn_poison, (value == -1 )? 0xFFF:value);
        return;
    }


    /**
     * new stats, range 1-255
     **/
    if ( !str_cmp( arg2, "str2" ) )
    {
        if (check_stat_range(ch, value)) return;
        victim->perm_stat[ STAT_STR ] = value;
        return;
    }
    if ( !str_cmp( arg2, "int2" ) )
    {
        if (check_stat_range(ch, value)) return;
        victim->perm_stat[ STAT_INT ] = value;
        return;
    }
    if ( !str_cmp( arg2, "wis2" ) )
    {
        if (check_stat_range(ch, value)) return;
        victim->perm_stat[ STAT_WIS] = value;
        return;
    }
    if ( !str_cmp( arg2, "dex2" ) )
    {
        if (check_stat_range(ch, value)) return;
        victim->perm_stat[ STAT_DEX ] = value;
        return;
    }
    if ( !str_cmp( arg2, "con2" ) )
    {
        if (check_stat_range(ch, value)) return;
        victim->perm_stat[ STAT_CON ] = value;
        return;
    }
    if ( !str_cmp( arg2, "luc2" ) )
    {
        if (check_stat_range(ch, value)) return;
        victim->perm_stat[ STAT_LUC ] = value;
        return;
    }
    if ( !str_cmp( arg2, "cha2" ) )
    {
        if (check_stat_range(ch, value)) return;
        victim->perm_stat[ STAT_CHA ] = value;
        return;
    }

    /*
     * Generate usage message.
     */
    do_function( ch, &do_mset, "" );
    return;
}

void do_string( CHAR_DATA *ch, char *argument )
{
	char type [ MAX_INPUT_LENGTH ];
	char arg1 [ MAX_INPUT_LENGTH ];
	char arg2 [ MAX_INPUT_LENGTH ];
	char arg3 [ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;
	OBJ_DATA *obj;

	smash_tilde( argument );
	argument = one_argument( argument, type );
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );

	if ( type[ 0 ] == '\0' || arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || arg3[ 0 ] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  string char <name> <field> <string>\n\r", ch );
		send_to_char( "        fields: name short long desc title spec\n\r", ch );
		send_to_char( "  string obj  <name> <field> <string>\n\r", ch );
		send_to_char( "        fields: name short long extended\n\r", ch );
		return;
	}

	if ( !str_prefix( type, "character" ) || !str_prefix( type, "mobile" ) )
	{
		if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
		{
			send_to_char( "They aren't here.\n\r", ch );
			return;
		}

		/* clear zone for mobs */
		victim->zone = NULL;

		/* string something */

		if ( !str_prefix( arg2, "name" ) )
		{
			if ( !IS_NPC( victim ) )
			{
				send_to_char( "Not on PC's.\n\r", ch );
				return;
			}
			free_string( victim->name );
			victim->name = str_dup( arg3 );
			return;
		}

		if ( !str_prefix( arg2, "description" ) )
		{
			free_string( victim->description );
			victim->description = str_dup( arg3 );
			return;
		}

		if ( !str_prefix( arg2, "short" ) )
		{
			free_string( victim->short_descr );
			victim->short_descr = str_dup( arg3 );
			return;
		}

		if ( !str_prefix( arg2, "long" ) )
		{
			free_string( victim->long_descr );
			victim->long_descr = str_dup( arg3 );
			return;
		}

		if ( !str_prefix( arg2, "title" ) )
		{
			if ( IS_NPC( victim ) )
			{
				send_to_char( "Not on NPC's.\n\r", ch );
				return;
			}

			set_title( victim, arg3 );
			return;
		}

		if ( !str_prefix( arg2, "spec" ) )
		{
			if ( !IS_NPC( victim ) )
			{
				send_to_char( "Not on PC's.\n\r", ch );
				return;
			}

			if ( ( victim->spec_fun = spec_lookup( arg3 ) ) == 0 )
			{
				send_to_char( "No such spec fun.\n\r", ch );
				return;
			}

			return;
		}
	}

	if ( !str_prefix( type, "object" ) )
	{
		/* string an obj */

		if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
		{
			send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
			return;
		}

		if ( !str_prefix( arg2, "name" ) )
		{
			free_string( obj->name );
			obj->name = str_dup( arg3 );
			return;
		}

		if ( !str_prefix( arg2, "short" ) )
		{
			free_string( obj->short_descr );
			obj->short_descr = str_dup( arg3 );
			return;
		}

		if ( !str_prefix( arg2, "long" ) )
		{
			free_string( obj->description );
			obj->description = str_dup( arg3 );
			return;
		}

		if ( !str_prefix( arg2, "ed" ) || !str_prefix( arg2, "extended" ) )
		{
			EXTRA_DESCR_DATA * ed;

			argument = one_argument( argument, arg3 );
			if ( argument == NULL )
			{
				send_to_char( "Syntax: oset <object> ed <keyword> <string>\n\r",
							  ch );
				return;
			}

			strcat( argument, "\n\r" );

			ed = new_extra_descr();

			ed->keyword	= str_dup( arg3	);
			ed->description	= str_dup( argument );
			ed->next	= obj->extra_descr;
			obj->extra_descr	= ed;
			return;
		}
	}


	/* echo bad use message */
	do_function( ch, &do_string, "" );
}

void do_oset( CHAR_DATA *ch, char *argument )
{
	char arg1 [ MAX_INPUT_LENGTH ];
	char arg2 [ MAX_INPUT_LENGTH ];
	char arg3 [ MAX_INPUT_LENGTH ];
	OBJ_DATA *obj;
	int value=0;


	smash_tilde( argument );
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );

	if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  set obj <object> <field> <value>\n\r", ch );
		send_to_char( "  Field being one of:\n\r", ch );
		send_to_char( "        value0 value1 value2 value3 value4 (v1-v4)\n\r", ch );
		//rellik: komponenty, dodajê mo¿liwo¶æ ustawiania komponentów
		send_to_char( "        extra wear weight cost timer condition description komponent komponent-counter komponent-timer rent_cost\n\r", ch );
		return;
	}

	if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
	{
		send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
		return;
	}

	/*
		* Snarf the value (which need not be numeric).
		*/
	if ( arg3[0] != '\0' && is_number( arg3 ) ) value = atoi( arg3 );

	/*
		* Set something.
		*/
	if ( !str_prefix( arg2, "komponent" ) )
	{
		if ( obj->is_spell_item )
		{
			obj->is_spell_item = FALSE;
			send_to_char( "Obiekt nie jest teraz komponentem.\n\r", ch );
		} else {
			obj->is_spell_item = TRUE;
			send_to_char( "Obiekt jest teraz komponentem.\n\r", ch );
		}
		return;
	}

	if ( !str_cmp( arg2, "komponent-counter" ) )
	{
		if( value == 0 ) return;
		obj->spell_item_counter = value;
		print_char( ch, "Obiekt ma teraz %d ³adunków jako komponent.\n\r", value );
		return;
	}

	if ( !str_cmp( arg2, "komponent-timer" ) )
	{
		if( value <= 0 ) return;
		obj->spell_item_timer = value;
		print_char( ch, "Obiekt ma teraz czas ¿ycia %d godzin mudowych.\n\r", value );
		return;
	}


	if ( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
	{
		obj->value[ 0 ] = UMIN( 50, value );
		return;
	}

	if ( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
	{
		obj->value[ 1 ] = value;
		return;
	}

	if ( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
	{
		obj->value[ 2 ] = value;
		return;
	}

	if ( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
	{
		obj->value[ 3 ] = value;
		return;
	}

	if ( !str_cmp( arg2, "value4" ) || !str_cmp( arg2, "v4" ) )
	{
		obj->value[ 4 ] = value;
		return;
	}

	if ( !str_cmp( arg2, "value5" ) || !str_cmp( arg2, "v5" ) )
	{
		obj->value[ 5 ] = value;
		return;
	}

	if ( !str_cmp( arg2, "value6" ) || !str_cmp( arg2, "v6" ) )
	{
		obj->value[ 6 ] = value;
		return;
	}

	if ( !str_prefix( arg2, "weight" ) )
	{
		obj->weight = value;
		return;
	}

	if ( !str_prefix( arg2, "cost" ) )
	{
		obj->cost = value;
		return;
	}

	if ( !str_prefix( arg2, "timer" ) )
	{
		obj->timer = value;
		return;
	}

	if ( !str_prefix( arg2, "condition" ) )
	{
		obj->condition = value;
		return;
	}

	if ( !str_prefix( arg2, "rent_cost" ) )
	{
		obj->rent_cost = value;
		return;
	}

	/*
		* Generate usage message.
		*/
	do_function( ch, &do_oset, "" );
	return;
}

void do_rset( CHAR_DATA *ch, char *argument )
{
	char arg1 [ MAX_INPUT_LENGTH ];
	char arg2 [ MAX_INPUT_LENGTH ];
	char arg3 [ MAX_INPUT_LENGTH ];
	ROOM_INDEX_DATA *location;

	smash_tilde( argument );
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	strcpy( arg3, argument );

	if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' || arg3[ 0 ] == '\0' )
	{
		send_to_char( "Syntax:\n\r", ch );
		send_to_char( "  set room <location> <field> <value>\n\r", ch );
		send_to_char( "  Field being one of:\n\r", ch );
		send_to_char( "        flags sector\n\r", ch );
		return;
	}

	if ( ( location = find_location( ch, arg1 ) ) == NULL )
	{
		send_to_char( "No such location.\n\r", ch );
		return;
	}

	if ( !is_room_owner( ch, location ) && ch->in_room != location
		 && room_is_private( location ) && !IS_TRUSTED( ch, IMPLEMENTOR ) )
	{
		send_to_char( "That room is private right now.\n\r", ch );
		return;
	}

	if ( !str_prefix( arg2, "flags" ) )
	{
		BITVECT_DATA * flag;

		flag = ext_flag_lookup( arg3, room_flags );

		if ( flag != &EXT_NONE )
			EXT_TOGGLE_BIT( location->room_flags, *flag );
		return;
	}

	if ( !str_prefix( arg2, "sector" ) )
	{
		if ( !is_number( arg3 ) )
		{
			send_to_char( "Value must be numeric.\n\r", ch );
			return;
		}

		location->sector_type	= atoi( arg3 );
		return;
	}

	/*
		* Generate usage message.
		*/
	do_function( ch, &do_rset, "" );
	return;
}

void do_sockets( CHAR_DATA *ch, char *argument )
{
	char buf[ 2 * MAX_STRING_LENGTH ];
	char buf2[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *ach;
	int count;
	char* min = "zz";
	char* actual = "00";
	bool repeat;

	count	= 0;
	buf[ 0 ] = '\0';

	one_argument( argument, arg );

    sprintf( buf + strlen( buf ), "{Y[ DS  CN  TI] %-15s %16s %5s %s{x\n\r", "NAME", "IP ADDRESS", "PORT", "HOSTNAME" );

    //sortowanie
	while ( str_cmp( min, "zzz" ) )
	{
		min = "zzz";
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( !d->character || d->connected < 0 ) continue;

			if ( d->character != NULL && can_see( ch, d->character )
				 && ( arg[ 0 ] == '\0' || is_name( arg, d->character->ss_data ? d->character->short_descr : d->character->name )
					  || ( d->original && is_name( arg, d->original->ss_data ? d->original->short_descr : d->original->name ) ) ) )
			{
				if ( strcmp( d->host, min ) <= 0
					 && strcmp( d->host, actual ) > 0 )
					min = d->host;
			}
		}
		actual = min;

		if ( strcmp( actual, "zzz" ) )
		{
			repeat = FALSE;
			for ( d = descriptor_list; d != NULL; d = d->next )
			{
				if ( !d->character || d->connected < 0 ) continue;

				if ( d->character != NULL && can_see( ch, d->character )
					 && !str_cmp( d->host, actual )
					 && ( arg[ 0 ] == '\0' || is_name( arg, d->character->ss_data ? d->character->short_descr : d->character->name )
						  || ( d->original && is_name( arg, d->original->ss_data ? d->original->short_descr : d->original->name ) ) ) )
				{
					count++;
					sprintf( buf + strlen( buf ), "[%3d %3d %3d] %s%-15s %16s %5d %s{x\n\r",
							 d->descriptor,
							 d->connected,
							 d->original ? d->original->timer :
							 d->character ? d->character->timer : -1,
							 repeat ? "{R" : "",
							 d->original ? d->original->name :
							 d->character ? d->character->name : "(none)",
                             d->ip_addr,
                             d->port,
							 d->host
						   );
					repeat = TRUE;
				}
			}
		}
	}

	if ( count == 0 )
	{
		send_to_char( "Nie ma w grze nikogo o takim imieniu.\n\r", ch );
		return;
	}

	sprintf( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
	strcat( buf, buf2 );
	page_to_char( buf, ch );

	min = "zz";
	actual = "00";

	count	= 0;
	buf[ 0 ] = '\0';

	//sortowanie
	while ( str_cmp( min, "zzz" ) )
	{
		min = "zzz";
		for ( ach = char_list; ach; ach = ach->next )
		{
			if ( IS_NPC( ach ) || ach->desc ) continue;

			if ( can_see( ch, ach ) )
			{
				if ( strcmp( ach->host, min ) <= 0
					 && strcmp( ach->host, actual ) > 0 )
					min = ach->host;
			}
		}
		actual = min;

		if ( strcmp( actual, "zzz" ) )
		{
			repeat = FALSE;
			for ( ach = char_list; ach; ach = ach->next )
			{
				if ( IS_NPC( ach ) || ach->desc ) continue;

				if ( can_see( ch, ach ) && !str_cmp( ach->host, actual ) )
				{
					for ( d = descriptor_list; d != NULL; d = d->next )
					{
						if ( !d->character || d->connected < 0 ) continue;
						if ( !str_cmp( d->host, ach->host ) )
						{
							repeat = TRUE;
							break;
						}
					}

					count++;
					sprintf( buf + strlen( buf ), "%s%-15s %s{x\n\r",
							 repeat ? "{R" : "",
							 ach->name,
							 ach->host
						   );
					repeat = TRUE;
				}
			}
		}
	}

	if ( count == 0 )
		return;

	send_to_char( "\n\r{CGracze z zerwanym linkiem:{x\n\r", ch );
	page_to_char( buf, ch );
	return;
}

/*
* Thanks to Grodyn for pointing out bugs in this function.
*/
void do_force( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
	{
		send_to_char( "Force whom to do what?\n\r", ch );
		return;
	}

	one_argument( argument, arg2 );

	if ( !str_cmp( arg2, "delete" ) || !str_prefix( arg2, "mob" ) )
	{
		send_to_char( "That will NOT be done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg, "all" ) )
	{
		CHAR_DATA * vch;
		CHAR_DATA *vch_next;

		if ( get_trust( ch ) < MAX_LEVEL - 3 )
		{
			send_to_char( "Not at your level!\n\r", ch );
			return;
		}

		for ( vch = char_list; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next;

			if ( !IS_NPC( vch ) && get_trust( vch ) < get_trust( ch ) )
			{
				if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && !IS_NPC( vch ) && !IS_IMMORTAL( vch ) && IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
				{
					sprintf( buf, "Nie¶miertelny zmusza cie do: '%s'.", argument );
				}
				else
				{
					sprintf( buf, "$n zmusza cie do: '%s'.", argument );
				}
				act( buf, ch, NULL, vch, TO_VICT );
				interpret( vch, argument );
			}
		}
	}
	else if ( !str_cmp( arg, "players" ) )
	{
		CHAR_DATA * vch;
		CHAR_DATA *vch_next;

		if ( get_trust( ch ) < MAX_LEVEL - 2 )
		{
			send_to_char( "Not at your level!\n\r", ch );
			return;
		}

		for ( vch = char_list; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next;

			if ( !IS_NPC( vch ) && get_trust( vch ) < get_trust( ch )
				 && vch->level < LEVEL_HERO )
			{
				if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && !IS_NPC( vch ) && !IS_IMMORTAL( vch ) && IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
				{
					sprintf( buf, "Nie¶miertelny zmusza cie do: '%s'.", argument );
				}
				else
				{
					sprintf( buf, "$n zmusza cie do: '%s'.", argument );
				}
				act( buf, ch, NULL, vch, TO_VICT );
				interpret( vch, argument );
			}
		}
	}
	else if ( !str_cmp( arg, "gods" ) )
	{
		CHAR_DATA * vch;
		CHAR_DATA *vch_next;

		if ( get_trust( ch ) < MAX_LEVEL - 2 )
		{
			send_to_char( "Not at your level!\n\r", ch );
			return;
		}

		for ( vch = char_list; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next;

			if ( !IS_NPC( vch ) && get_trust( vch ) < get_trust( ch )
				 && vch->level >= LEVEL_HERO )
			{
				sprintf( buf, "$n zmusza cie do: '%s'.", argument );
				act( buf, ch, NULL, vch, TO_VICT );
				interpret( vch, argument );
			}
		}
	}
	else
	{
		CHAR_DATA *victim;
		if ( ( victim = get_char_world( ch, arg ) ) == NULL )
		{
			send_to_char( "They aren't here.\n\r", ch );
			return;
		}

		if ( victim == ch )
		{
			send_to_char( "Nie pro¶ciej to zrobiæ? Po co siê zmuszaæ?\n\r", ch );
			return;
		}

		if ( !is_room_owner( ch, victim->in_room )
			 && ch->in_room != victim->in_room
			 && room_is_private( victim->in_room ) && !IS_TRUSTED( ch, IMPLEMENTOR ) )
		{
			send_to_char( "That character is in a private room.\n\r", ch );
			return;
		}

		if ( get_trust( victim ) >= get_trust( ch ) )
		{
			send_to_char( "Sam se to zrób!\n\r", ch );
			return;
		}

		if ( !IS_NPC( victim ) && get_trust( ch ) < MAX_LEVEL - 3 )
		{
			send_to_char( "Not at your level!\n\r", ch );
			return;
		}

		if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) && IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
		{
			sprintf( buf, "Nie¶miertelny zmusza cie do: '%s'.", argument );
		}
		else
		{
			sprintf( buf, "$n zmusza cie do: '%s'.", argument );
		}

		act( buf, ch, NULL, victim, TO_VICT );
		interpret( victim, argument );
	}

	send_to_char( "Ok.\n\r", ch );
	return;
}

/*
* New routines by Dionysos.
*/
void do_invis( CHAR_DATA *ch, char *argument )
{
	int level;
	char arg[ MAX_STRING_LENGTH ];

	/* RT code for taking a level argument */
	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
		/* take the default path */

		if ( ch->invis_level )
		{
			ch->invis_level = 0;
			act( "$n powoli materializuje sie.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Powoli materializujesz sie.\n\r", ch );
		}
		else
		{
			ch->invis_level = get_trust( ch );
			act( "$n powoli rozplywa sie.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Powoli rozplywasz sie.\n\r", ch );
		}
	else
		/* do the level thing */
	{
		level = atoi( arg );
		if ( level < 2 || ( level > get_trust( ch ) ) )
		{
			send_to_char( "Invis level must be between 2 and your level.\n\r", ch );
			return;
		}
		else
		{
			ch->reply = NULL;
			ch->invis_level = level;
			act( "$n powoli siê rozp³ywa.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Powoli siê rozp³ywasz.\n\r", ch );
		}
	}

	return;
}

void do_incognito( CHAR_DATA *ch, char *argument )
{
	int level;
	char arg[ MAX_STRING_LENGTH ];

	/* RT code for taking a level argument */
	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
		/* take the default path */

		if ( ch->incog_level )
		{
			ch->incog_level = 0;
			act( "$n is no longer cloaked.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "You are no longer cloaked.\n\r", ch );
		}
		else
		{
			ch->incog_level = get_trust( ch );
			act( "$n ukrywa swoja obecnosc.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Ukrywasz swoja obecnosc.\n\r", ch );
		}
	else
		/* do the level thing */
	{
		level = atoi( arg );
		if ( level < 2 || level > get_trust( ch ) )
		{
			send_to_char( "Incog level must be between 2 and your level.\n\r", ch );
			return;
		}
		else
		{
			ch->reply = NULL;
			ch->incog_level = level;
			act( "$n ukrywa swoja obecnosc.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Ukrywasz swoja obecnosc.\n\r", ch );
		}
	}

	return;
}

void do_holylight( CHAR_DATA *ch, char *argument )
{
	if ( IS_NPC( ch ) )
		return;

	if ( EXT_IS_SET( ch->act, PLR_HOLYLIGHT ) )
	{
		EXT_REMOVE_BIT( ch->act, PLR_HOLYLIGHT );
		send_to_char( "Holy light mode off.\n\r", ch );
	}
	else
	{
		EXT_SET_BIT( ch->act, PLR_HOLYLIGHT );
		send_to_char( "Holy light mode on.\n\r", ch );
	}

	return;
}

/* prefix command: it will put the string typed on each line typed */

void do_prefix ( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_INPUT_LENGTH ];

	if ( argument[ 0 ] == '\0' )
	{
		if ( ch->prefix[ 0 ] == '\0' )
		{
			send_to_char( "You have no prefix to clear.\r\n", ch );
			return;
		}

		send_to_char( "Prefix removed.\r\n", ch );
		free_string( ch->prefix );
		ch->prefix = str_dup( "" );
		return;
	}

	if ( ch->prefix[ 0 ] != '\0' )
	{
		sprintf( buf, "Prefix changed to %s.\r\n", argument );
		free_string( ch->prefix );
	}
	else
	{
		sprintf( buf, "Prefix set to %s.\r\n", argument );
	}

	ch->prefix = str_dup( argument );
}

#define CH(descriptor)  ((descriptor)->original ? (descriptor)->original : (descriptor)->character)

/* This file holds the copyover data */
#define COPYOVER_FILE "copyover.data"

/* This is the executable file */
#define EXE_FILE	  "../src/rom"

void save_misc_data args( ( void ) );

/*  Copyover - Original idea: Fusion of MUD++
	*  Adapted to Diku by Erwin S. Andreasen, <erwin@pip.dknet.dk>
	*  http://pip.dknet.dk/~pip1773
	*  Changed into a ROM patch after seeing the 100th request for it :)
	*/

void do_copyover ( CHAR_DATA *ch, char * argument )
{
	FILE * fp;
	DESCRIPTOR_DATA *d, *d_next;
	CHAR_DATA * vch, * vch_next;
	char buf [ 100 ], buf2[ 100 ], buf3[ 100 ];
	char arg[ MAX_INPUT_LENGTH ];
	extern int port, control, control2; /* db.c */
	int csec = 0;
	bool exit = TRUE;

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Sk³adnia:\n\r"
					  "copyover now [nosave]- wykonuje copyover od razu\n\r"
					  "copyover delayed [nosave] - wykona copyover gdy nie bêdzie graczy na mudzie\n\r"
					  "copyover delayed <sekundy> [nosave] - wykona copyover po danej liczbie sekund\n\r"
					  "copyover schedule <godziny> - zleca wykonywanie copyover co ile¶ godzin\n\r"
					  "copyover schedule off - wy³±cza copyover co ile¶ godzin\n\r"
					  "copyover status - pokazuje status automatycznego copyovera\n\r"
					  "copyover stop - anuluje automatyczny copyover\n\r"
					  "Parametrem 'nosave' mo¿esz okre¶liæ czy przed copyover mud ma zapisaæ krainy czy nie.\n\r", ch );
		return;
	}
	else
	{
		argument = one_argument( argument, arg );

		if ( !str_prefix( arg, "status" ) )
		{
			if ( misc.copyover_delayed < 0 )
			{
				if ( misc.copyover_scheduled < 0 )
					send_to_char( "Nie zosta³ zaplanowany ¿aden automatyczny copyover.\n\r", ch );
				else
					printf_to_char( ch, "Copyover bêdzie wykonywany co %ld godzin.\n\r", misc.copyover_period / ( 60 * 60 ) );
				return;
			}
			else if ( misc.copyover_delayed == 0 )
			{
				printf_to_char(
					ch,
					"Zosta³ zaplanowany automatyczny copyover gdy nie bêdzie graczy na mudzie.\n\r%s\n\r",
					misc.copyover_save ? "Krainy zostan± zapisane." : "Krainy nie beda zapisywane." );
				return;
			}
			else if ( misc.copyover_delayed > 0 )
			{
				printf_to_char(
					ch,
					"Zosta³ zaplanowany automatyczny copyover za %d sekund.\n\r%s\n\r",
					misc.copyover_delayed - current_time,
					misc.copyover_save ? "Krainy zostan± zapisane." : "Krainy nie beda zapisywane." );
				return;
			}
			return;
		}

		if ( !str_prefix( arg, "stop" ) )
		{
			if ( misc.copyover_delayed < 0 )
			{
				send_to_char( "Nie zosta³ zaplanowany ¿aden automatyczny copyover.\n\r", ch );
				return;
			}
			misc.copyover_delayed = -1;
			send_to_char( "Automatyczny copyover odwo³any.\n\r", ch );
			return;
		}

		if ( !str_prefix( arg, "scheduled" ) )
		{
			argument = one_argument( argument, arg );
			if ( arg[ 0 ] == '\0' )
			{
				misc.copyover_scheduled = current_time + 2 * 24 * 60 * 60;
				misc.copyover_period = 2 * 24 * 60 * 60;
				send_to_char( "Copyover bêdzie wykonywany co 48 godzin.\n\r", ch );
				save_misc_data();
				return;
			}

			if ( !str_cmp( arg, "off" ) )
			{
				misc.copyover_scheduled = -1;
				misc.copyover_period = -1;
				send_to_char( "Samoczynny copyover nie bêdzie wykonywany.\n\r", ch );
				save_misc_data();
				return;
			}

			if ( is_number( arg ) )
			{
				csec = atoi( arg );
				misc.copyover_scheduled = current_time + csec * 60 * 60;
				misc.copyover_period = csec * 60 * 60;
				printf_to_char( ch, "Copyover bêdzie wykonywany co %d godzin.\n\r", csec );
				save_misc_data();
				return;
			}

			send_to_char( "Podaj co ile godzin ma byæ wykonywany copyover.\n\r", ch );
			return;
		}

		if ( !str_cmp( arg, "delayed" ) )
		{
			misc.copyover_save = TRUE;
			argument = one_argument( argument, arg );
			if ( arg[ 0 ] == '\0' )
			{
				misc.copyover_delayed = 0;
				send_to_char( "Copyover zostanie automatycznie wykonany gdy nie bêdzie graczy na mudzie (z zapisem krain).\n\r", ch );
				return;
			}

			if ( is_number( arg ) )
			{
				csec = atoi( arg );
				argument = one_argument( argument, arg );
				if ( !str_cmp( arg, "nosave" ) ) misc.copyover_save = FALSE;
				misc.copyover_delayed = current_time + csec;
				printf_to_char( ch, "Copyover zostanie automatycznie wykonany za %d sekund (%s).\n\r", csec,
								misc.copyover_save ? "z zapisem krain" : "bez zapisu krain" );
				if ( csec < 60 )
				{
					for ( d = descriptor_list; d; d = d->next )
					{
						if ( !d->character ) continue;
						if ( d->connected == CON_PLAYING )
							printf_to_char( d->character, "^f{RPrze³adowanie ¶wiata za oko³o %d sekund!\n\r{x", csec );
					}
				}

				return;
			}

			if ( !str_cmp( arg, "nosave" ) )
			{
				misc.copyover_save = FALSE;
				misc.copyover_delayed = 0;
				send_to_char( "Copyover zostanie automatycznie wykonany gdy nie bêdzie graczy na mudzie (bez zapisu krain).\n\r", ch );
				return;
			}
		}

		if ( !str_cmp( arg, "now" ) )
		{
			exit = FALSE;
			argument = one_argument( argument, arg );
			if ( arg[ 0 ] == '\0' )
				misc.copyover_save = TRUE;
			else
				misc.copyover_save = FALSE;
		}

		if ( exit )
		{
			send_to_char( "Sk³adnia:\n\r"
						  "copyover now [nosave]- wykonuje copyover od razu\n\r"
						  "copyover delayed [nosave] - wykona copyover gdy nie bêdzie graczy na mudzie\n\r"
						  "copyover delayed <sekundy> [nosave] - wykona copyover po danej liczbie sekund\n\r"
						  "copyover schedule <godziny> - zleca wykonywanie copyover co ile¶ godzin\n\r"
						  "copyover schedule off - wy³±cza copyover co ile¶ godzin\n\r"
						  "copyover status - pokazuje status automatycznego copyovera\n\r"
						  "copyover stop - anuluje automatyczny copyover\n\r"
						  "Parametrem 'nosave' mo¿esz okre¶liæ czy przed copyover mud ma zapisaæ krainy czy nie.\n\r", ch );
			return;
		}
	}

	fp = fopen ( COPYOVER_FILE, "w" );

	if ( !fp )
	{
		send_to_char ( "Copyover file not writeable, aborted.\n\r", ch );
		logformat ( "Could not write to copyover file: %s", COPYOVER_FILE );
		perror ( "do_copyover:fopen" );
		return;
	}

	if ( misc.copyover_save && ch )
	{
	   // Tener:
	   // nie zmieniaæ tego na world, bo update krainek na playerze siê pochrzani!!!
		do_asave ( NULL, "changed" );
		do_lset ( ch, "save" );
		save_clans();
	}

	//w sumie to ma sie savowac zawsze
	if ( artefact_system != NULL )
	{
		reboot_artefact_list();
		do_aset( ch, "save" );
	}
	save_pccorpses();
	save_misc_data();

    if ( ch && !IS_NPC( ch ) )
    {
        sprintf ( buf, "\n\r *** COPYOVER by %s - proszê po³±czyæ siê za chwilê!\n\r", ch->name );
    }
    else
    {
        sprintf ( buf, "\n\r *** Nastepuje przeladowanie swiata - proszê po³±czyæ siê za chwilê!\n\r" );
    }

	for ( vch = char_list; vch; vch = vch_next )
	{
		vch_next = vch->next;

		if ( IS_NPC( vch ) || vch->desc )
			continue;

		if ( check_rent ( vch, vch->carrying, FALSE ) )
		{
			save_char_obj ( vch, FALSE, FALSE );
			do_function ( vch, &do_quit, "" );
		}
		else
		{
			if ( !IS_IMMORTAL ( vch ) )
				drop_objects( vch, vch->carrying, TRUE );

			save_char_obj ( vch, FALSE, FALSE );
			do_function ( vch, &do_quit, "" );
		}
	}

    /* For each playing descriptor, save its state */
    for ( d = descriptor_list; d ; d = d_next )
    {
        CHAR_DATA * och;
		d_next = d->next; /* We delete from the list , so need to save this */

		if ( d->connected < 0 ) continue;

		och = CH ( d );
		if ( !d->character ) continue;
#ifdef MCCP
		compressEnd( d );
#endif
		if ( !d->character || d->connected > CON_PLAYING )     /* drop those logging on */
		{
			write_to_descriptor ( d, "\n\rPrzepraszamy, ale mud jest w trakcie rebootu, wroc za chwile.\n\r", 0 );
			if ( d->descriptor != -1 ) close( d->descriptor ); /* throw'em out */
		}
		else
		{
			fprintf ( fp, "%d %s %s %s %d %s %d\n", d->descriptor, och->name, d->host, d->ip_addr, d->port, d->user, och->in_room != NULL ? och->in_room->vnum : -1);
			save_char_obj ( och, TRUE, FALSE );
			write_to_descriptor ( d, buf, 0 );
		}
	}

	fprintf ( fp, "-1\n" );
	fclose ( fp );

	/* Close reserve and other always-open files and release other resources */
	fclose ( fpReserve );
	/* exec - descriptors are inherited */

	sprintf ( buf, "%d", port );
	sprintf ( buf2, "%d", control );
	sprintf ( buf3, "%d", control2 );
	execl ( EXE_FILE, "rom", buf, "copyover", buf2, buf3, ( char * ) NULL );

	/* Failed - sucessful exec will not return */

	perror ( "do_copyover: execl" );

	if ( ch ) send_to_char ( "Copyover FAILED!\n\r", ch );

	/* Here you might want to reopen fpReserve */
	fpReserve = fopen ( NULL_FILE, "r" );
}

/* Recover from a copyover - load players */
void copyover_recover ()
{
	DESCRIPTOR_DATA * d;
	FILE *fp;
	ROOM_INDEX_DATA* pRoom = NULL;
	char name [ 100 ];
	char host[ MSL ];
    char ip_addr[ 17 ];
	char user[ MAX_INPUT_LENGTH ];
	char str_unlink[ MAX_INPUT_LENGTH ];
	//char buf[ MAX_STRING_LENGTH ];
	int room;
	int desc;
	int in_port;
	bool fOld;
	//int shape = 0, animal_vnum = 0;

	log_string ( "Copyover recovery initiated" );

	fp = fopen ( COPYOVER_FILE, "r" );

	if ( !fp )     /* there are some descriptors open which will hang forever then ? */
	{
		perror ( "copyover_recover:fopen" );
		log_string ( "Copyover file not found. Exitting.\n\r" );
		exit ( 1 );
	}

	unlink ( COPYOVER_FILE ); /* In case something crashes - doesn't prevent reading	*/

	for ( ;; )
	{
		fscanf ( fp, "%d %s %s %s %d %s %d\n", &desc, name, host, ip_addr, &in_port, user, &room);
//				fscanf ( fp, "%d %s %s %s %d %s %d %d %d\n", &desc, name, host, ip_addr, &in_port, user, &room, &shape, &animal_vnum );

		if ( desc == -1 )
			break;

		d = new_descriptor();
		d->descriptor = desc;

		d->host     = str_dup ( host );
        d->ip_addr  = str_dup ( ip_addr );
		d->user = str_dup ( user );
		d->port = in_port;
		d->next = descriptor_list;
		descriptor_list = d;
		d->connected = CON_COPYOVER_RECOVER; /* -15, so close_socket frees the char */

		/* Now, find the pfile */
		fOld = load_char_obj ( d, name, FALSE );

		/* Write something, and check if it goes error-free */
		if ( !write_to_descriptor ( d, "\n\rPrzywracanie polaczen z graczami...\n\r", 0 ) )
		{
			close_socket( d, CS_NORECURSE ); /* nope */
			continue;
		}

		if ( !fOld )     /* Player file not found?! */
		{
			write_to_descriptor ( d, "\n\rStalo sie cos dziwnego i zginela twoja postac. Przepraszamy.\n\r", 0 );
			close_socket ( d, CS_NORECURSE );
		}
		else /* ok! */
		{
			write_to_descriptor ( d, "\n\rPrzeladowanie swiata zakonczone. Mozna grac dalej.\n\r", 0 );
#ifdef MCCP
			write_to_buffer( d, compress2_on_str4, 0 );
#endif
			if ( room > 0 )
				pRoom = get_room_index( room );

			if ( pRoom != NULL )
				d->character->in_room = pRoom;

			/* Just In Case */
			if ( !d->character->in_room )
				d->character->in_room = get_room_index ( ROOM_VNUM_TEMPLE );

			/* Insert in the char_list */
			d->character->next = char_list;
			char_list = d->character;
			reset_char( d->character );

			char_to_room ( d->character, d->character->in_room );

			do_look ( d->character, "auto" );
			act ( "$n materializuje siê!", d->character, NULL, NULL, TO_ROOM );
			d->connected = CON_PLAYING;
/*
			if ( shape >= 0 )
			{
				sprintf( buf, "%d %d", shape, animal_vnum );
				do_shapeshift_silent ( d->character,  buf );
			}*/
			/*artefact*/
			restore_char_on_artefact_list( d->character );

			if ( d->character->level < 2 )
			{
				sprintf( str_unlink, "%s%s", PLAYER_DIR, capitalize( d->character->name ) );
				unlink( str_unlink );
			}

			if ( d->character->position == POS_RESTING && d->character->count_memspell > 0 )
			{
				echo_mem( d->character, MEM_START );
				get_next_memming( d->character );
			}

		}
	}
	fclose ( fp );
}

void do_otype( CHAR_DATA *ch, char *argument )
{
	OBJ_INDEX_DATA * obj;
	char arg1[ MAX_INPUT_LENGTH ];
	char buf[ MAX_INPUT_LENGTH ];
	BUFFER *buffer;
	int count = 1;
	short int xtype = -1;
	int x;
	ush_int vnum, match;
	int col = 0;

	extern int port;

	if ( port == PORT_PLAYER && !IS_SET( ch->pcdata->wiz_conf, W5 ) )
	{
		send_to_char( "Na player-world moze to zrobiæ tylko sêdzia albo lord.\n\r", ch );
		return ;
	}

	argument = one_argument( argument, arg1 );

	if ( arg1[ 0 ] == '\0' )
	{
		send_to_char( "usage: otype item_type  <level>\n\r", ch );
		send_to_char( "Mozliwe typy: \n\r", ch );

		for ( x = 0;item_table[ x ].name;x++ )
		{
			sprintf( buf, "%-12s", item_table[ x ].name );
			send_to_char( buf, ch );

			if ( ++col % 6 == 0 )
				send_to_char( "\n\r", ch );
		}
		return;
	}

	buffer = new_buf();
	/*znajdujemy jaki to type*/

	for ( x = 0;item_table[ x ].name;x++ )
	{
		if ( !str_prefix( arg1, item_table[ x ].name ) )
		{
			xtype = item_table[ x ].type;
			break;
		}
	}

	if ( xtype == -1 )
	{
		send_to_char( "Mozliwe typy: \n\r", ch );

		for ( x = 0;item_table[ x ].name;x++ )
		{
			sprintf( buf, "%-12s", item_table[ x ].name );
			send_to_char( buf, ch );

			if ( ++col % 6 == 0 )
				send_to_char( "\n\r", ch );
		}
		return;
	}

	count = 0;
	match = 0;
	for ( vnum = 0; match < top_obj_index; vnum++ )
	{

		if ( ( obj = get_obj_index( vnum ) ) != NULL )
		{
			match++;
			if ( xtype == obj->item_type )
			{
				sprintf( buf, "[%-3d] %-40s ------  VNUM : %-4d\n\r",
						 count, obj->short_descr,
						 obj->vnum );
				count++;
				add_buf( buffer, buf );
			}
		}
	}

	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );
	return;
}

void do_owear( CHAR_DATA *ch, char *argument )
{
	OBJ_INDEX_DATA * obj;
	char arg1[ MAX_INPUT_LENGTH ];
	char buf[ MAX_INPUT_LENGTH ];
	BUFFER *buffer;
	int count = 1;
	ush_int vnum;
	int match;
	int wear_flag = 0;
	int bit, x;
	int col = 0;

	extern int port;

	if ( port == PORT_PLAYER && !IS_SET( ch->pcdata->wiz_conf, W5 ) )
	{
		send_to_char( "Na player-world moze to zrobiæ tylko sêdzia albo lord.\n\r", ch );
		return ;
	}

	argument = one_argument( argument, arg1 );

	if ( arg1[ 0 ] == '\0' )
	{
		send_to_char( "usage: owear wear_type <level> \n\r", ch );
		return;
	}

	buffer = new_buf();
	count = 0;
	/*znajdujemy jaki to type*/

	for ( x = 0, bit = 2; x < 18 ; bit = bit << 1, x++ )
	{
		if ( !str_prefix( arg1, wear_bit_name( bit ) ) )
		{
			wear_flag = bit;
			break;
		}
	}

	if ( wear_flag == 0 )
	{
		send_to_char( "Mozliwe typy: \n\r", ch );

		for ( x = 0, bit = 2; x < 18 ; bit = bit << 1, x++ )
		{
			sprintf( buf, "%-12s", wear_bit_name( bit ) );
			send_to_char( buf, ch );

			if ( ++col % 6 == 0 )
				send_to_char( "\n\r", ch );
		}

		return;
	}

	match = 0;

	for ( vnum = 0; match < top_obj_index; vnum++ )
	{
		if ( ( obj = get_obj_index( vnum ) ) != NULL )
		{
			match++;

			if ( obj->wear_flags & wear_flag )
			{
				sprintf( buf, "%-45.45s ------  VNUM : %-4d\n\r",
						 obj->short_descr,
						 obj->vnum );

				count++;

				add_buf( buffer, buf );
			}
		}
	}




	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );
	return;
}

void do_fstat( CHAR_DATA *ch, char *argument )
{
	PFLAG_DATA * tmp;
	CHAR_DATA *victim = NULL;
	OBJ_DATA *obj = NULL;
	PFLAG_DATA *list = NULL;
	char buf[ MAX_INPUT_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	bool test = FALSE;

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Komenda s³u¿y do sprawdzanie dynamicznych flag u¿ywanych w progach.\n\r", ch );
		send_to_char( "Sk³adnia: fstat <nazwa_moba/nazwa_gracza>\n\r", ch );
		send_to_char( "Sk³adnia: fstat obj <nazwa obiektu>\n\r", ch );
		send_to_char( "Sk³adnia: fstat room\n\r", ch );
		send_to_char( "Sk³adnia: fstat global\n\r", ch );
		return;
	}
	argument = one_argument( argument, arg );

	if ( !str_prefix( arg, "global" ) )
	{
		list = misc.global_flag_list;
		sprintf( buf, "Flagi globalne:\n\r" );
	}
	else if ( !str_prefix( arg, "room" ) )
	{
		if ( ch->in_room )
		{
			list = ch->in_room->pflag_list;
			sprintf( buf, "%s %d\n\r", "Room", ch->in_room->vnum );
		}
	}
	else if ( !str_prefix( arg, "obj" ) )
	{
		if ( ( obj = get_obj_world( ch, argument ) ) == NULL )
		{
			send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
			return;
		}
		list = obj->pflag_list;
		sprintf( buf, "%s %d\n\r", obj->short_descr, obj->pIndexData->vnum );
	}
	else
	{
		if ( ( victim = get_char_world( ch, arg ) ) == NULL )
		{
			send_to_char( "Nie ma nikogo takiego.\n\r", ch );
			return;
		}
		list = victim->pflag_list;
		sprintf( buf, "%s %s\n\r", !IS_NPC( victim ) ? "Gracz" : "Mob", victim->name );
	}

	send_to_char( buf, ch );

	for ( tmp = list;tmp;tmp = tmp->next )
	{
		if ( !test )
			send_to_char( "-------\n\r", ch );
		sprintf( buf, "[Flaga]: %s [na czas:%d]\n\r", tmp->id, tmp->duration );
		send_to_char( buf, ch );
		test = TRUE;
	}

	if ( test )
	{
		send_to_char( "-------\n\r", ch );
		send_to_char( "To wszystkie flagi.\n\r", ch );
	}
	else
		send_to_char( "Brak flag.\n\r", ch );
	return;

}

void do_fset( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA * room;
	OBJ_DATA * obj;
	CHAR_DATA * victim;
	PFLAG_DATA **flag_list = NULL;
	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char arg3[ MAX_INPUT_LENGTH ];
	char arg4[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, arg1 );

	if ( arg1[ 0 ] == '\0' )
	{
		send_to_char( "Komenda s³u¿y do modyfikacji dynamicznych flag u¿ywanych w progach.\n\r", ch );
		send_to_char( "Sk³adnia: fset room <vnum> add/del <string> <warto¶c>\n\r", ch );
		send_to_char( "Sk³adnia: fset obj <nazwa obiektu> add/del <string> <warto¶æ>\n\r", ch );
		send_to_char( "Sk³adnia: fset mob <nazwa moba/gracza> add/del <string> <warto¶æ>\n\r", ch );
		send_to_char( "Sk³adnia: fset global add/del <string> <warto¶æ>\n\r", ch );
		return;
	}

	if ( !str_cmp( arg1, "room" ) )
	{
		argument = one_argument( argument, arg2 );

		if ( arg2[0] == '\0' || !is_number( arg2 ) )
		{
			send_to_char( "B³êdny vnum lokacji.\n\r", ch );
			return;
		}

		room = get_room_index( atoi( arg2 ) );
		if ( !room )
		{
			send_to_char( "Nie ma takiej lokacji.\n\r", ch );
			return;
		}
		flag_list = &room->pflag_list;
	}
	else if ( !str_cmp( arg1, "obj" ) )
	{
		argument = one_argument( argument, arg2 );

		if ( arg2[0] == '\0' )
		{
			send_to_char( "Nie podano obiektu.\n\r", ch );
			return;
		}

		if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
		{
			send_to_char( "Nie ma tu takiego obiektu.\n\r", ch );
			return;
		}
		flag_list = &obj->pflag_list;
	}
	else if ( !str_cmp( arg1, "mob" ) )
	{
		argument = one_argument( argument, arg2 );

		if ( arg2[0] == '\0' )
		{
			send_to_char( "Nie podano moba.\n\r", ch );
			return;
		}

		if ( ( victim = get_char_world( ch, arg2 ) ) == NULL )
		{
			send_to_char( "Nie ma takiego moba/gracza.\n\r", ch );
			return;
		}
		flag_list = &victim->pflag_list;
	}
	else if ( !str_cmp( arg1, "global" ) )
	{
		flag_list = &misc.global_flag_list;
	}
	else
	{
		do_function( ch, &do_fstat, "" );
		return;
	}

	argument = one_argument( argument, arg3 );

	if ( arg3[0] == '\0' )
	{
		send_to_char( "Nie podano typu operacji (add/del).\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg4 );

	if ( arg4[0] == '\0' )
	{
		send_to_char( "Nie podano nazwy flagi.\n\r", ch );
		return;
	}

	if ( !flag_list )
		return;

	if ( !str_prefix( arg3, "add" ) )
	{
		if ( argument[ 0 ] == '\0' || !is_number( argument ) )
		{
			send_to_char( "Nie poda³e¶ warto¶ci flagi.\n\r", ch );
			return;
		}

		if ( add_flag( flag_list, arg4, atoi( argument ) ) )
		{
			send_to_char( "Flaga dodana.\n\r", ch );
			save_misc_data();
			return;
		}
		else
		{
			send_to_char( "B³±d podczas dodawania flagi.\n\r", ch );
			print_char( ch, "Mo¿liwe b³êdy:\n\r[nazwa flagi dluzsza niz 32 znaki]\n\r" );
			return;
		}
	}
	else if ( !str_prefix( arg3, "del" ) )
	{
		if ( rem_flag( flag_list, arg4 ) )
		{
			send_to_char( "Flaga usuniêta.\n\r", ch );
			return;
		}
		else
		{
			send_to_char( "B³±d podczas usuwania flagi, prawdopodobnie nie ma takiej flagi.\n\r", ch );
			return;
		}
	}
	else
	{
		send_to_char( "Z³y typ operacji (add/del).\n\r", ch );
		return;
	}
	return;
}

/* rlookup mob|obj <vnum>
	* szuka gdzie w kraince w resetach ladowany jest
	* dany przedmiot/mob i wypisuje vnum pokoju z resetem
	*/

void do_rlookup( CHAR_DATA *ch, char *argument )
{
	ush_int vnum, min, max, i;
	ROOM_INDEX_DATA *pRoom;
	RESET_DATA *pReset;
	char arg1[ MAX_STRING_LENGTH ];
	char arg2[ MAX_STRING_LENGTH ];
	BUFFER *buffer;
	bool found = FALSE;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
	{
		send_to_char( "rlookup mob|obj <vnum>\n\r", ch );
		return;
	}

	if ( str_cmp( arg1, "mob" ) && str_cmp( arg1, "obj" ) )
	{
		send_to_char( "rlookup mob|obj <vnum>\n\r", ch );
		return;
	}

	if ( !is_number( arg2 ) )
	{
		send_to_char( "<vnum> must be a number.\n\r", ch );
		return;
	}

	vnum = atoi( arg2 );
	min = ch->in_room->area->min_vnum;
	max = ch->in_room->area->max_vnum;

	buffer = new_buf();

	sprintf( arg2, "|  room | type |  %3.3s  |\n\r", arg1 );
	add_buf( buffer, arg2 );
	add_buf( buffer, "------------------------\n\r" );

	for ( i = min;i <= max;i++ )
	{
		if ( ( pRoom = get_room_index( i ) ) == NULL )
			continue;

		for ( pReset = pRoom->reset_first;pReset;pReset = pReset->next )
			switch ( pReset->command )
			{
				case 'M':
					if ( str_cmp( arg1, "mob" ) )
						break;

					if ( vnum != pReset->arg1 )
						break;

					sprintf( arg2, "| %5d |   M  | %5d |\n\r", i, pReset->arg1 );
					add_buf( buffer, arg2 );
					found = TRUE;
					break;
				case 'O':
				case 'P':
				case 'G':
				case 'E':
					if ( str_cmp( arg1, "obj" ) )
						break;

					if ( vnum != pReset->arg1 )
						break;

					sprintf( arg2, "| %5d |   %c  | %5d |\n\r", i, pReset->command, pReset->arg1 );
					add_buf( buffer, arg2 );
					found = TRUE;
					break;
				default: break;
			}
	}

	if ( found )
		page_to_char( buf_string( buffer ), ch );
	else
		send_to_char( "Nie znaleziono resetow.\n\r", ch );

	free_buf( buffer );

	return;
}

void do_lstat( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	char prof[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	LEARN_DATA *ld = NULL;
	LEARN_LIST *ls = NULL;
	MOB_INDEX_DATA *mob;
	int vn;
	BUFFER *buffer = NULL;

	if ( IS_NPC( ch ) || !ch->pcdata )
		return;

	if ( !IS_SET( ch->pcdata->wiz_conf, W4 ) )
		return;

	if ( learn_system == NULL )
	{
		send_to_char( "Lista pusta.\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );

	if ( !str_cmp( arg, "skill" ) )
	{
		MOB_INDEX_DATA * pMob = NULL;
		int min_skill = 110;
		int prev_min = -1;
		int count = 0;
		int sn;
		int class = -1;
		BITVECT_DATA act = EXT_NONE;
		int colors;
		bool found;

		argument = one_argument( argument, arg2 );

		if ( ( sn = skill_only_lookup( arg2 ) ) < 0 )
		{
			send_to_char( "Nie ma takiego skilla.\n\r", ch );
			return;
		}

		buffer = new_buf();

		sprintf( buf, "Szukanie nauczycieli dla skilla '%s':\n\r\n\r", skill_table[ sn ].name );
		add_buf( buffer, buf );

		if ( argument[ 0 ] != '\0' )
		{
			class = class_lookup( argument );

			switch ( class )
			{
				case CLASS_MAG: act = ACT_MAGE; break;
				case CLASS_CLERIC: act = ACT_CLERIC; break;
				case CLASS_THIEF: act = ACT_THIEF; break;
				case CLASS_WARRIOR: act = ACT_WARRIOR; break;
				case CLASS_PALADIN: act = ACT_PALADIN; break;
				case CLASS_DRUID: act = ACT_DRUID; break;
				case CLASS_BARBARIAN: act = ACT_BARBARIAN; break;
				case CLASS_MONK: act = ACT_MONK; break;
				case CLASS_BARD: act = ACT_BARD; break;
				case CLASS_BLACK_KNIGHT: act = ACT_BLACK_KNIGHT; break;
				case CLASS_SHAMAN: act = ACT_SHAMAN; break;
			}
		}

		do
		{
			found = FALSE;
			min_skill = 110;
			for ( ld = learn_system; ld; ld = ld->next )
			{
				if ( ( pMob = get_mob_index( ld->vnum ) ) == NULL )
					continue;

				if ( !EXT_IS_EQUAL( act, EXT_NONE ) && !EXT_IS_SET( pMob->act, act ) )
					continue;

				for ( ls = ld->list; ls; ls = ls->next )
				{
					if ( pMob && ls->sn == sn && ls->min > prev_min && ls->min < min_skill )
					{
						min_skill = ls->min;
						found = TRUE;
					}
				}
			}

			prev_min = min_skill;

			if ( found )
			{
				for ( ld = learn_system; ld; ld = ld->next )
				{
					if ( ( pMob = get_mob_index( ld->vnum ) ) == NULL )
						continue;

					if ( !EXT_IS_EQUAL( act, EXT_NONE ) && !EXT_IS_SET( pMob->act, act ) )
						continue;

					for ( ls = ld->list; ls; ls = ls->next )
					{
						if ( pMob && ls->sn == sn && ls->min == min_skill )
						{
							colors = 15 + count_colors( pMob->short_descr, 0 );

							sprintf( buf, "%2d - %2d {C$:{x %-3d%% od %-2d {CMob{x: [%5d] %-*.*s {CArea{x: %10s\n\r",
									 ls->min,
									 ls->max,
									 ls->payment,
									 ls->pay_from,
									 pMob->vnum,
									 colors, colors,
									 pMob->short_descr,
									 pMob->area->name );
							add_buf( buffer, buf );
							count++;
						}
					}
				}
			}
		}
		while ( found );

		if ( count == 0 )
			sprintf( buf, "Nie znaleziono nauczycieli ucz±cych tego skilla.\n\r" );
		else
			sprintf( buf, "\n\rZnaleziono %d nauczycieli ucz±cych tego skilla.\n\r", count );
		add_buf( buffer, buf );

		if ( buffer != NULL )
		{
			page_to_char( buf_string( buffer ), ch );
			free_buf( buffer );
		}
		return;
	}
	// sprawdzanie tylko dla czarów
	else if ( !str_cmp( arg, "spell" ) )
	{
		MOB_INDEX_DATA * pMob = NULL;
		int min_vnum = 100000;
		int prev_min = -1;
		int count = 0;
		int sn;
		int colors;
		bool found;

		if ( ( sn = spell_only_lookup( argument ) ) < 0 )
		{
			send_to_char( "Nie ma takiego spella.\n\r", ch );
			return;
		}

		buffer = new_buf();
		sprintf( buf, "Szukanie nauczycieli dla spella '%s':\n\r\n\r", skill_table[ sn ].name );
		add_buf( buffer, buf );

		do
		{
			found = FALSE;
			min_vnum = 100000;
			for ( ld = learn_system; ld; ld = ld->next )
			{
				if ( ( pMob = get_mob_index( ld->vnum ) ) == NULL )
					continue;

				for ( ls = ld->list; ls; ls = ls->next )
				{
					if ( pMob && ls->sn == sn && pMob->vnum > prev_min && pMob->vnum < min_vnum )
					{
						min_vnum = pMob->vnum;
						found = TRUE;
					}
				}
			}

			prev_min = min_vnum;

			if ( found )
			{
				found = FALSE;
				for ( ld = learn_system; ld; ld = ld->next )
				{
					if ( ( pMob = get_mob_index( ld->vnum ) ) == NULL )
						continue;

					for ( ls = ld->list; ls; ls = ls->next )
					{
						if ( pMob && ls->sn == sn && pMob->vnum == min_vnum )
						{
							colors = 20 + count_colors( pMob->short_descr, 0 );

							sprintf( buf, "{CCh:{x %2d {CMob{x: [%5d] %-*.*s {CArea{x: %10s\n\r",
									 ls->chance,
									 pMob->vnum,
									 colors, colors,
									 pMob->short_descr,
									 pMob->area->name );
							add_buf( buffer, buf );
							count++;
							found = TRUE;
							break;
						}
					}
					if ( found ) break;
				}
				found = TRUE;
			}
		}
		while ( found );

		if ( count == 0 )
			sprintf( buf, "Nie znaleziono nauczycieli ucz±cych tego spella.\n\r" );
		else
			sprintf( buf, "\n\rZnaleziono %d nauczycieli ucz±cych tego spella.\n\r", count );
		add_buf( buffer, buf );

		if ( buffer != NULL )
		{
			page_to_char( buf_string( buffer ), ch );
			free_buf( buffer );
		}
		return;
	}
	// sprawdzanie dla klasy
	else if ( !str_cmp( arg, "class" ) )
	{
		MOB_INDEX_DATA * pMob = NULL;
		int min_vnum = 100000;
		int prev_min = -1;
		int count = 0;
		int class;
		BITVECT_DATA act;
		int colors;
		bool found;

		class = class_lookup( argument );

		if ( class == CLASS_MAG )
		{
			act = ACT_MAGE;
		}
		else if ( class == CLASS_CLERIC )
		{
			act = ACT_CLERIC;
		}
		else if ( class == CLASS_THIEF )
		{
			act = ACT_THIEF;
		}
		else if ( class == CLASS_WARRIOR )
		{
			act = ACT_WARRIOR;
		}
		else if ( class == CLASS_PALADIN )
		{
			act = ACT_PALADIN;
		}
		else if ( class == CLASS_DRUID )
		{
			act = ACT_DRUID;
		}
		else if ( class == CLASS_BARBARIAN )
		{
			act = ACT_BARBARIAN;
		}
		else if ( class == CLASS_MONK )
		{
			act = ACT_MONK;
		}
		else if ( class == CLASS_BARD )
		{
			act = ACT_BARD;
		}
		else if ( class == CLASS_BLACK_KNIGHT )
		{
			act = ACT_BLACK_KNIGHT;
		}
		else
		{
			send_to_char( "Nie ma takiej klasy.\n\r", ch );
			return;
		}

		buffer = new_buf();
		sprintf( buf, "Szukanie nauczycieli dla profesji '%s':\n\r\n\r", class_table[ class ].name );
		add_buf( buffer, buf );
		do
		{
			found = FALSE;
			min_vnum = 100000;
			for ( ld = learn_system; ld; ld = ld->next )
			{
				if ( ( pMob = get_mob_index( ld->vnum ) ) == NULL )
					continue;


				for ( ls = ld->list; ls; ls = ls->next )
				{
					if ( pMob && EXT_IS_SET( pMob->act, act ) && pMob->vnum > prev_min && pMob->vnum < min_vnum )
					{
						min_vnum = pMob->vnum;
						found = TRUE;
					}
				}
			}

			prev_min = min_vnum;

			if ( found )
			{
				found = FALSE;
				for ( ld = learn_system; ld; ld = ld->next )
				{
					if ( ( pMob = get_mob_index( ld->vnum ) ) == NULL )
						continue;

					for ( ls = ld->list; ls; ls = ls->next )
					{
						if ( pMob && EXT_IS_SET( pMob->act, act ) && pMob->vnum == min_vnum )
						{
							colors = 20 + count_colors( pMob->short_descr, 0 );

							sprintf( buf, "{CMob{x: [%5d] %-*.*s {CArea{x: %10s\n\r",
									 pMob->vnum,
									 colors, colors,
									 pMob->short_descr,
									 pMob->area->name );
							add_buf( buffer, buf );
							count++;
							found = TRUE;
							break;
						}
					}
					if ( found ) break;
				}
				found = TRUE;
			}
		}
		while ( found );

		if ( count == 0 )
			sprintf( buf, "Nie znaleziono nauczycieli ucz±cych tê profesjê.\n\r" );
		else
			sprintf( buf, "\n\rZnaleziono %d nauczycieli ucz±cych tê profesjê.\n\r", count );

		add_buf( buffer, buf );

		if ( buffer != NULL )
		{
			page_to_char( buf_string( buffer ), ch );
			free_buf( buffer );
		}
		return;
	}

	if ( is_number( arg ) )
	{
		vn = atoi( arg );

		if ( !get_mob_index( vn ) )
		{
			send_to_char( "Nie ma takiego moba.\n\r", ch );
			return;
		}

		for ( ld = learn_system; ld; ld = ld->next )
		{
			if ( ld->vnum == vn )
			{
				if ( ld->list == NULL )
				{
					send_to_char( "Nie zdefiniowano zadnych warto¶æi.\n\r", ch );
					return;
				}
				else
				{
					buffer = new_buf();
					sprintf( buf, "Skille/spelle ustawione dla moba: {C%s{x [{C%5d{x]\n\r\n\r",
							 get_mob_index( vn ) ->player_name,
							 vn );
					add_buf( buffer, buf );
					add_buf( buffer, "                                             Zakres/\n\r" );
					add_buf( buffer, "Rodzaj   Nazwa                               Szansa   % op³aty P³atne od\n\r" );

					for ( ls = ld->list; ls; ls = ls->next )
					{
						if ( skill_table[ ls->sn ].spell_fun == spell_null )
						{
							if ( ls->payment > 0 )
								sprintf( buf, "({Gskill{x) [%-30s] | {C%3d{x - {C%-3d{x |  {C%-3d%%{x  |   {C%d{x    |\n\r",
										 skill_table[ ls->sn ].name,
										 ls->min,
										 ls->max,
										 ls->payment,
										 ls->pay_from );
							else
								sprintf( buf, "({Gskill{x) [%-30s] | {C%3d{x - {C%-3d{x |   --   |    -    |\n\r",
										 skill_table[ ls->sn ].name,
										 ls->min,
										 ls->max );
							add_buf( buffer, buf );
						}
					}

					for ( ls = ld->list; ls; ls = ls->next )
					{
						if ( skill_table[ ls->sn ].spell_fun != spell_null )
						{
							sprintf( buf, "({Mspell{X) [%-30s] |    {C%-3d{x    |   --   |    -    |\n\r",
									 skill_table[ ls->sn ].name,
									 ls->chance );
							add_buf( buffer, buf );
						}
					}
				}
			}
		}

		if ( buffer != NULL )
		{
			page_to_char( buf_string( buffer ), ch );
			free_buf( buffer );
		}
		return;
	}
	else
	{
		if ( !str_prefix( arg, "all" ) )
		{
			if ( learn_system == NULL )
			{
				send_to_char( "Nie ma zadnych uczacych mobow.\n\r", ch );
				return;
			}

			buffer = new_buf();

			for ( ld = learn_system; ld; ld = ld->next )
			{
				if ( ( mob = get_mob_index( ld->vnum ) ) == NULL )
					continue;

				prof[ 0 ] = '\0';
				if ( EXT_IS_SET( mob->act, ACT_MAGE ) )
					strcat( prof, "mag " );
				if ( EXT_IS_SET( mob->act, ACT_CLERIC ) )
					strcat( prof, "kleryk " );
				if ( EXT_IS_SET( mob->act, ACT_THIEF ) )
					strcat( prof, "z³odziej " );
				if ( EXT_IS_SET( mob->act, ACT_WARRIOR ) )
					strcat( prof, "wojownik " );
				if ( EXT_IS_SET( mob->act, ACT_PALADIN ) )
					strcat( prof, "paladyn " );
				if ( EXT_IS_SET( mob->act, ACT_DRUID ) )
					strcat( prof, "druid " );
				if ( EXT_IS_SET( mob->act, ACT_BARBARIAN ) )
					strcat( prof, "barbarzyñca " );
				if ( EXT_IS_SET( mob->act, ACT_MONK ) )
					strcat( prof, "mnich " );
				if ( EXT_IS_SET( mob->act, ACT_BARD ) )
					strcat( prof, "bard " );
				if ( EXT_IS_SET( mob->act, ACT_BLACK_KNIGHT ) )
					strcat( prof, "czarny_rycerz " );
				if ( EXT_IS_SET( mob->act, ACT_SHAMAN ) )
					strcat( prof, "szaman " );

				sprintf( buf, "[%-5d] : %-31.31s {CProf{x: %s\n\r",
						 ld->vnum,
						 mob->player_name,
						 prof );

				add_buf( buffer, buf );
			}

			page_to_char( buf_string( buffer ), ch );
			free_buf( buffer );
			return;
		}
		else if ( !str_prefix( arg, "notused" ) )
		{
			int sn;
			bool found = FALSE;

			send_to_char( "Skille/spelle nigdzie nie przydzielone:\n\r", ch );
			buffer = new_buf();

			//lecimy po wszystkich skillach
			for ( sn = 1; sn < MAX_SKILL - 1; sn++ )
			{
				found = FALSE;
				for ( ld = learn_system; ld; ld = ld->next )
				{
					for ( ls = ld->list ;ls ; ls = ls->next )
					{
						if ( ls->sn != sn )
							continue;

						if ( !get_mob_index( ld->vnum ) )
							continue;

						found = TRUE;
						break;
					}
					if ( found ) break;
				}

				if ( !found )
				{
					sprintf( buf, "%-25s  (%s)\n\r",
							 skill_table[ sn ].name,
							 skill_table[ sn ].spell_fun == spell_null ? "skill" : "spell" );
					add_buf( buffer, buf );
				}

			}

			if ( buffer != NULL )
			{
				page_to_char( buf_string( buffer ), ch );
				free_buf( buffer );
			}
		}
		else
		{
			send_to_char( "Sk³adnia:\n\r"
						  "{Clstat all{x     - lista mobów ucz±cych czegokolwiek\n\r"
						  "{Clstat <vnum>{x  - lista tego co uczy mob o danym vnumie\n\r"
						  "{Clstat skill <skill> [<profesja>]{x - lista mobów ucz±cych danego skilla danej profesji (opcja)\n\r"
						  "{Clstat spell <spell>{x - lista mobów ucz±cych danego spella\n\r"
						  "{Clstat class <klasa>{x - lista mobów ucz±cych dan± profesjê\n\r"
						  "{Clstat notused{x - lista skilli/spelli, których nikt nie uczy\n\r", ch );
			return;
		}
	}

	return;
}

void do_ltstat( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	LEARN_TRICK_DATA *ld = NULL;
	LEARN_TRICK_LIST *ls = NULL;
	MOB_INDEX_DATA *mob;
	BUFFER *buffer = NULL;

	if ( IS_NPC( ch ) || !ch->pcdata )
		return;

	if ( !IS_SET( ch->pcdata->wiz_conf, W4 ) )
		return;

	if ( learn_trick_system == NULL )
	{
		send_to_char( "Lista pusta.\n\r", ch );
		return;
	}

	buffer = new_buf();

	for ( ld = learn_trick_system; ld; ld = ld->next )
	{
		if ( ( mob = get_mob_index( ld->vnum ) ) == NULL )
			continue;

		for ( ls = ld->list; ls; ls = ls->next )
		{
			if( ls == NULL )
				break;

			sprintf( buf, "[%-5d] : %-31.31s {CTriki{x: [%s,%d%%,%d$]\n\r",
				 ld->vnum,
				 mob->player_name,
				 trick_table[ ls->sn ].name,
				 ls->chance,
				 ls->payment);

			add_buf( buffer, buf );
		}
	}

	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );
	return;
}

void do_lset( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_STRING_LENGTH ];
	LEARN_DATA *ld = NULL, *tmpld = NULL;
	LEARN_LIST *ls = NULL, *tmpls = NULL, *tmpls_prev;
	MOB_INDEX_DATA *pMob;
	int vn;
	sh_int val1 = 0, val2 = 0, val3 = 0, val4 = 0, sn, x = 0;
	bool spell = FALSE;


	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Komenda sluzy do modyfikowania tablicy systemu cwiczenia skilli/spelli.\n\r"
					  "lset add  <vnum> <skill> <min> <max> [<od ilu>] [<% oplaty>]\n\r"
					  "lset add  <vnum> <spell> <szansa>\n\r"
					  "lset paid <vnum> <skill> <od ilu> <% oplaty>\n\r"
					  "lset del  <vnum> <skill>\n\r"
					  "lset save\n\r"
					  "lset help\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );

	if ( !str_cmp( arg, "help" ) )
	{
		send_to_char( "System cwiczeñ skilli/spelli jest oddzielony od OLC w celu\n\r"
					  "zwiêkszenia stabilno¶ci muda. Podczasz bootowania muda ³adowany\n\r"
					  "jest oddzielny plik zawierajacy dane które moby mog± uczyæ jakich\n\r"
					  "skill/spelli i w jakim zakresie.\n\r"
					  "\n\r"
					  "Dla spelli <szansa> to warto¶æ modyfikujaca szanse nauczenia siê danego\n\r"
					  "spellu przez gracze.\n\r"
					  "\n\r"
					  "Modyfikataro dla czerów mo¿e mieæ nastêpuj±ce warto¶ci:\n\r"
					  "mniejszy ni¿ 0 (max -25)  zmiejsza szansê nauczenia\n\r"
					  "równy 0                   pozostawia szansê na niezmienionym poziomie\n\r"
					  "wiêkszy ni¿ 0 (max 15)    zwiêksza szansê nauczenia\n\r"
					  "\n\r"
					  "Dla skilli <min> to minimum od jakiego mob moze trenowaæ gracze,\n\r"
					  "a <max> to maksimum do jakiego mob mo¿e wytrenowaæ gracza.\n\r"
					  "\n\r"
					  "W obu przypadkach mo¿na tak¿e podaæ czy mob ma pobieraæ op³atê za uczenie.\n\r"
					  "Komend± lset paid ustawimy od jakiej warto¶ci skilla mob ma pobieraæ op³aty\n\r"
					  "oraz czy mob kasuje dro¿ej czy taniej (podstawa op³aty wyliczana jest\n\r"
					  "niezale¿nie od skilla), przy czym 100 to 100%", ch );
		return;
	}

	if ( !str_cmp( arg, "add" ) )
	{
		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie podales vnuma moba.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		if ( is_number( arg ) )
			vn = atoi( arg );
		else
		{
			send_to_char( "Blednie podales vnum moba.\n\r", ch );
			return;
		}

		if ( ( pMob = get_mob_index( vn ) ) == NULL )
		{
			send_to_char( "Taki mob nie istnieje.\n\r", ch );
			return;
		}

		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie podales nazwy skilla/spella.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		if ( ( sn = skill_lookup( arg ) ) == -1 )
		{
			send_to_char( "Taki skill/spell nie istnieje.\n\r", ch );
			return;
		}

		if ( skill_table[ sn ].spell_fun != spell_null )
			spell = TRUE;

		if ( argument[ 0 ] == '\0' )
		{
			print_char( ch, "Nie podales parametru %s.\n\r", spell ? "<szansa>" : "<min>" );
			return;
		}

		argument = one_argument( argument, arg );

		if ( is_number( arg ) )
			val1 = atoi( arg );
		else
		{
			print_char( ch, "Parametr %s powinien byæ liczb±.\n\r", spell ? "<szansa>" : "<min>" );
			return;
		}

		if ( !spell )
		{
			argument = one_argument( argument, arg );

			if ( arg[ 0 ] == '\0' )
			{
				send_to_char( "Nie podales parametru <max>.\n\r", ch );
				return;
			}

			if ( is_number( arg ) )
				val2 = atoi( arg );
			else
			{
				send_to_char( "Parametr <max> powinien byæ liczb±.\n\r", ch );
				return;
			}
		}

		if ( argument[ 0 ] != '\0' )
		{
			argument = one_argument( argument, arg );

			if ( is_number( arg ) )
				val3 = UMAX( 0, atoi( arg ) );
			else
			{
				send_to_char( "Parametr <od_ilu> powinien byæ liczb±.\n\r", ch );
				return;
			}
		}

		if ( argument[ 0 ] != '\0' )
		{
			argument = one_argument( argument, arg );

			if ( is_number( arg ) )
				val4 = UMAX( 0, atoi( arg ) );
			else
			{
				send_to_char( "Parametr <%op³aty> powinien byæ liczb±.\n\r", ch );
				return;
			}
		}

		if ( !EXT_IS_SET( pMob->act, ACT_PRACTICE ) )
			EXT_SET_BIT( pMob->act, ACT_PRACTICE );

		ls = new_learn_list();
		ls->next = NULL;
		ls->sn = sn;

		print_char( ch, "%s: %s\n\r", spell ? "spell" : "skill", skill_table[ sn ].name );
		if ( spell )
		{
			ls->min = 0; /*dla spelli nieistotne*/
			ls->max = 0;
			ls->chance = val1;
			ls->payment = 0;
			ls->pay_from = 0;
		}
		else
		{
			ls->min = val1; /*dla spelli nieistotne*/
			ls->max = val2;
			ls->chance = 0;
			ls->payment = val4;
			ls->pay_from = val3;
		}


		for ( tmpld = learn_system; tmpld; tmpld = tmpld->next )
		{
			if ( tmpld->vnum == vn )
			{
				for ( tmpls = tmpld->list; tmpls; tmpls = tmpls->next )
				{
					if ( tmpls->sn == ls->sn )
					{
						tmpls->min = ls->min;
						tmpls->max = ls->max;
						tmpls->chance = ls->chance;
						send_to_char( "Skill/spell zmieniony.\n\r", ch );
						return;
					}

					if ( tmpls->next == NULL )
					{
						ls->next = NULL;
						tmpls->next = ls;
						send_to_char( "Skill/spell dodany.\n\r", ch );
						return;
					}
				}
			}

			if ( tmpld->next == NULL )
			{
				ld = new_learn_data();
				ld->vnum = vn;
				tmpld->next = ld;
				ld->list = ls;
				send_to_char( "Skill/spell dodany.\n\r", ch );
				return;
			}
		}

		if ( tmpld == NULL )
		{
			ld = new_learn_data();
			ld->vnum = vn;
			ld->next = NULL;
			ld->list = ls;
			learn_system = ld;
			send_to_char( "Skill/spell dodany.\n\r", ch );
			return;
		}
	}
	if ( !str_cmp( arg, "paid" ) )
	{
		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie podales vnuma moba.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		if ( is_number( arg ) )
			vn = atoi( arg );
		else
		{
			send_to_char( "Blednie podales vnum moba.\n\r", ch );
			return;
		}

		if ( ( pMob = get_mob_index( vn ) ) == NULL )
		{
			send_to_char( "Taki mob nie istnieje.\n\r", ch );
			return;
		}

		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie podales nazwy skilla/spella.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		if ( ( sn = skill_lookup( arg ) ) == -1 )
		{
			send_to_char( "Taki skill nie istnieje.\n\r", ch );
			return;
		}

		if ( skill_table[ sn ].spell_fun != spell_null )
		{
			send_to_char( "Op³atê mo¿na ustawiaæ tylko dla skillów.\n\r", ch );
			return;
		}

		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie podano parametru <od ilu>.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		if ( is_number( arg ) )
			val1 = atoi( arg );
		else
		{
			send_to_char( "Parametr <od ilu> powinien byæ liczb±.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		if ( arg[ 0 ] == '\0' )
		{
			send_to_char( "Nie podano parametru <% op³aty>.\n\r", ch );
			return;
		}

		if ( is_number( arg ) )
			val2 = atoi( arg );
		else
		{
			send_to_char( "Parametr <% op³aty> powinien byæ liczb±.\n\r", ch );
			return;
		}

		for ( tmpld = learn_system; tmpld; tmpld = tmpld->next )
		{
			if ( tmpld->vnum == vn )
			{
				for ( tmpls = tmpld->list; tmpls; tmpls = tmpls->next )
				{
					if ( tmpls->sn == sn )
					{
						if ( val1 > tmpls->max )
						{
							send_to_char( "Podany parametr <od ilu> jest wiêkszy ni¿ <max> dla tego skilla.\n\r", ch );
							return;
						}
						tmpls->payment = val2;
						tmpls->pay_from = val1;
						send_to_char( "Op³ata dla skilla ustawiona.\n\r", ch );
						return;
					}

					if ( tmpls->next == NULL )
					{
						send_to_char( "Ten mob nie ma takiego skilla.\n\r", ch );
						return;
					}
				}
			}

			if ( tmpld->next == NULL )
			{
				send_to_char( "Ten mob nie ma takiego skilla.\n\r", ch );
				return;
			}
		}

		if ( tmpld == NULL )
		{
			send_to_char( "Ten mob nie ma takiego skilla.\n\r", ch );
			return;
		}
	}
	else if ( !str_cmp( arg, "del" ) )
	{
		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie poda³e¶ vnuma moba.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		if ( is_number( arg ) )
			vn = atoi( arg );
		else
		{
			send_to_char( "B³êdnie poda³e¶ vnum moba.\n\r", ch );
			return;
		}

		if ( get_mob_index( vn ) == NULL )
		{
			send_to_char( "Taki mob nie istnieje.\n\r", ch );
			return;
		}

		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie poda³e¶ nazwy skilla/spella.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		if ( ( sn = skill_lookup( arg ) ) == -1 )
		{
			send_to_char( "Taki skill/spell nie istnieje.\n\r", ch );
			return;
		}

		tmpls_prev = NULL;

		for ( ld = learn_system; ld; ld = ld->next )
			if ( ld->vnum == vn )
			{
				for ( ls = ld->list; ls; ls = ls->next, x++ )
				{
					if ( ls->sn == sn )
					{
						if ( x == 0 )      /*pierwszy element do usuniecia*/
						{
							ld->list = ld->list->next;
							ls->next = NULL;
							free_learn_list( ls );
							send_to_char( "Skill usuniety.\n\r", ch );
							return;
						}
						else /*ktorys nastepny*/
						{
							tmpls_prev->next = ls->next;
							ls->next = NULL;
							free_learn_list( ls );
							send_to_char( "Skill usuniety.\n\r", ch );
							return;
						}
					}

					tmpls_prev = ls;
				}
			}
		send_to_char( "Cos nie tak.\n\r", ch );
		return;
	}
	else if ( !str_cmp( arg, "save" ) )
	{
		FILE * fp;

		if ( learn_system == NULL )
			return;
		fclose( fpReserve );

		if ( ( fp = fopen( LEARN_FILE, "w" ) ) == NULL )
			return;

		for ( ld = learn_system; ld; ld = ld->next )
		{
			if ( ld->list != NULL )
			{
				fprintf( fp, "m %d\n", ld->vnum );

				for ( ls = ld->list; ls; ls = ls->next )
				{
					if ( ls->payment > 0 )
						fprintf( fp, "p %d %d %d %ld %ld %s~\n",
								 ls->min,
								 ls->max,
								 ls->chance,
								 UMAX( 0, ls->payment ),
								 UMAX( 0, ls->pay_from ),
								 skill_table[ ls->sn ].name );
					else
						fprintf( fp, "s %d %d %d %s~\n",
								 ls->min,
								 ls->max,
								 ls->chance,
								 skill_table[ ls->sn ].name );
				}
			}
		}

		fclose( fp );
		fpReserve = fopen( NULL_FILE, "r" );
		send_to_char( "Lista zapisana\n\r", ch );
		return;
	}

	send_to_char( "Komenda sluzy do modyfikowania tablicy systemu cwiczenia skilli/spelli.\n\r"
				  "lset add <vnum> <skill> <min> <max>\n\r"
				  "lset add <vnum> <spell> <szansa>\n\r"
				  "lset paid <od ilu> <% oplaty>\n\r"
				  "lset del <vnum> <skill>\n\r"
				  "lset save\n\r"
				  "lset help\n\r", ch );
	return;
}

void do_ltset( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_STRING_LENGTH ];
	LEARN_TRICK_DATA *ld = NULL, *tmpld = NULL;
	LEARN_TRICK_LIST *ls = NULL, *tmpls = NULL, *tmpls_prev;
	MOB_INDEX_DATA *pMob;
	int vn;
	sh_int szansa = 0, cena = 0, sn, x = 0;


	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Komenda sluzy do modyfikowania tablicy systemu cwiczenia trików.\n\r"
					  "ltset add <vnum> <trick> <szansa> <cena>\n\r"
					  "ltset del  <vnum> <trick>\n\r"
					  "ltset save\n\r"
					  "ltset help\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );

	if ( !str_cmp( arg, "help" ) )
	{
		send_to_char( "System cwiczeñ trickow jest oddzielony od OLC w celu\n\r"
					  "zwiêkszenia stabilno¶ci muda. Podczasz bootowania muda ³adowany\n\r"
					  "jest oddzielny plik zawierajacy dane które moby mog± uczyæ jakich\n\r"
					  "trickow i w jakim zakresie.\n\r"
					  "\n\r"
					  "<vnum> to vnum moba - nauczyciela.\n\r"
					  "\n\r"
					  "<szansa> to procentowa szansa modyfikowana o statystyki postaci na nauczenie sie\n\r"
					  "tricka przez gracza. Jak mu sie nie uda, bedzie musial jeszcze raz placic.\n\r"
					  "\n\r"
					  "<cena> to ilosc srebrnych monet jakie gracz bedzie musial zaplacic za kazda probe\n\r"
					  "nauczenia sie trika.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg, "add" ) )
	{
		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie podales vnuma moba.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		if ( is_number( arg ) )
			vn = atoi( arg );
		else
		{
			send_to_char( "Blednie podales vnum moba.\n\r", ch );
			return;
		}

		if ( ( pMob = get_mob_index( vn ) ) == NULL )
		{
			send_to_char( "Taki mob nie istnieje.\n\r", ch );
			return;
		}

		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie podales nazwy trika.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		for ( sn = 0; sn < MAX_TRICKS; sn++ )
		{
			if ( trick_table[ sn ].name == NULL )
			{
				send_to_char( "Taki trik nie istnieje.\n\r", ch );
				return;
			}

			if( !str_cmp( arg, trick_table[ sn ].name ) )
				break;
		}

		if ( argument[ 0 ] == '\0' )
		{
			print_char( ch, "Nie podales parametru <szansa>.\n\r" );
			return;
		}

		argument = one_argument( argument, arg );

		if ( is_number( arg ) )
			szansa = atoi( arg );
		else
		{
			print_char( ch, "Parametr <szansa> powinien byæ liczb±.\n\r" );
			return;
		}

		argument = one_argument( argument, arg );

		if ( arg[ 0 ] == '\0' )
		{
			send_to_char( "Nie podales parametru <cena>.\n\r", ch );
			return;
		}

		if ( is_number( arg ) )
			cena = atoi( arg );
		else
		{
			send_to_char( "Parametr <cena> powinien byæ liczb±.\n\r", ch );
			return;
		}

		if ( !EXT_IS_SET( pMob->act, ACT_PRACTICE ) )
			EXT_SET_BIT( pMob->act, ACT_PRACTICE );

		ls = new_learn_trick_list();
		ls->next = NULL;
		ls->sn = sn;

		print_char( ch, "Trik: %s\n\r", trick_table[ sn ].name );
		ls->chance = szansa;
		ls->payment = cena;

		for ( tmpld = learn_trick_system; tmpld; tmpld = tmpld->next )
		{
			if ( tmpld->vnum == vn )
			{
				for ( tmpls = tmpld->list; tmpls; tmpls = tmpls->next )
				{
					if ( tmpls->sn == ls->sn )
					{
						tmpls->chance = ls->chance;
						tmpls->payment = ls->payment;
						send_to_char( "Trik zmieniony.\n\r", ch );
						return;
					}

					if ( tmpls->next == NULL )
					{
						ls->next = NULL;
						tmpls->next = ls;
						send_to_char( "Trik dodany1.\n\r", ch );
						return;
					}
				}
			}

			if ( tmpld->next == NULL )
			{
				ld = new_learn_trick_data();
				ld->vnum = vn;
				tmpld->next = ld;
				ld->list = ls;
				send_to_char( "Trik dodany2.\n\r", ch );
				return;
			}
		}

		if ( tmpld == NULL )
		{
			ld = new_learn_trick_data();
			ld->vnum = vn;
			ld->next = NULL;
			ld->list = ls;
			learn_trick_system = ld;
			send_to_char( "Trik dodany3.\n\r", ch );
			return;
		}
	}
	else if ( !str_cmp( arg, "del" ) )
	{
		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie poda³e¶ vnuma moba.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		if ( is_number( arg ) )
			vn = atoi( arg );
		else
		{
			send_to_char( "B³êdnie poda³e¶ vnum moba.\n\r", ch );
			return;
		}

		if ( get_mob_index( vn ) == NULL )
		{
			send_to_char( "Taki mob nie istnieje.\n\r", ch );
			return;
		}

		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie poda³e¶ nazwy trika.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		for ( sn = 0; sn < MAX_TRICKS; sn++ )
		{
			if ( trick_table[ sn ].name == NULL )
			{
				send_to_char( "Taki trik nie istnieje.\n\r", ch );
				return;
			}

			if( !str_cmp( arg, trick_table[ sn ].name ) )
				break;
		}

		tmpls_prev = NULL;

		for ( ld = learn_trick_system; ld; ld = ld->next )
			if ( ld->vnum == vn )
			{
				for ( ls = ld->list; ls; ls = ls->next, x++ )
				{
					if ( ls->sn == sn )
					{
						if ( x == 0 )      /*pierwszy element do usuniecia*/
						{
							ld->list = ld->list->next;
							ls->next = NULL;
							free_learn_trick_list( ls );
							send_to_char( "Trik usuniety.\n\r", ch );
							return;
						}
						else /*ktorys nastepny*/
						{
							tmpls_prev->next = ls->next;
							ls->next = NULL;
							free_learn_trick_list( ls );
							send_to_char( "Trik usuniety.\n\r", ch );
							return;
						}
					}

					tmpls_prev = ls;
				}
			}
		send_to_char( "Cos nie tak.\n\r", ch );
		return;
	}
	else if ( !str_cmp( arg, "save" ) )
	{
		FILE * fp;

		if ( learn_trick_system == NULL )
			return;
		fclose( fpReserve );

		if ( ( fp = fopen( LEARN_TRICK_FILE, "w" ) ) == NULL )
			return;

		for ( ld = learn_trick_system; ld; ld = ld->next )
		{
			if ( ld->list != NULL )
			{
				fprintf( fp, "m %d\n", ld->vnum );

				for ( ls = ld->list; ls; ls = ls->next )
				{
						fprintf( fp, "t %d %d %s~\n",
								 ls->chance,
								 ls->payment,
								 trick_table[ ls->sn ].name );
				}
			}
		}

		fclose( fp );
		fpReserve = fopen( NULL_FILE, "r" );
		send_to_char( "Lista zapisana\n\r", ch );
		return;
	}

		send_to_char( "Komenda sluzy do modyfikowania tablicy systemu cwiczenia trików.\n\r"
					  "ltset add <vnum> <trick> <szansa> <cena>\n\r"
					  "ltset del  <vnum> <trick>\n\r"
					  "ltset save\n\r"
					  "ltset help\n\r", ch );
	return;
}

/*artefact*/
void do_amod( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_STRING_LENGTH ];
	OBJ_INDEX_DATA *cobj = NULL;
	ROOM_INDEX_DATA *croom = NULL;

	ARTEFACT_DATA *art = NULL;
	ARTEFACT_LOADER *loader, *load = NULL, *prev = NULL;
	int type, pd, pu, vnum, num;

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Modyfikowanie listy ladowania sie artefaktow.\n\r", ch );
		send_to_char( "Sposob spozycia:\n\r", ch );
		send_to_char( "amod <vnum artefaktu> <add/del> <room/obj/mob> <vnum room'u/obj'ektu/moba> <prawd1> <prawd2>.\n\r", ch );
		send_to_char( "room/obj/mob okresla gdzie bedzie sie artefakt ladowal.\n\r", ch );
		send_to_char( "prawd1 i prawd2 okresla prawdopodobienstwo wystapienia czyli:\n\r", ch );
		send_to_char( "prawd1/prawd2, np: 2/11\n\r", ch );
		send_to_char( "np: amod 20 add room 150 1 10\n\r", ch );
		send_to_char( "czyli artefakt vnum=20, bedzie ladowany do roomu vnum=150 z prawd. 1/10\n\r", ch );
		send_to_char( "np: amod 20 del room 150\n\r", ch );
		send_to_char( "co oznacza skasowanie powyzszego\n\r", ch );
	}

	argument = one_argument( argument, arg );
	if ( is_number( arg ) )
	{
		num = atoi( arg );
	}
	else
	{
		send_to_char( "To nie numer artefaktu!!!\n\r", ch );
		return;
	}

	for ( art = artefact_system;art;art = art->next )
	{
		if ( art->avnum == num )
		{
			argument = one_argument( argument, arg );

			if ( !str_cmp( arg, "add" ) )
			{
				//jaki typ
				argument = one_argument( argument, arg );
				if ( !str_cmp( arg, "room" ) )
					type = 0;
				else if ( !str_cmp( arg, "obj" ) )
					type = 1;
				else if ( !str_cmp( arg, "mob" ) )
					type = 2;
				else
				{
					send_to_char( "Zle podales : <room/obj/mob>.\n\r", ch );
					return;
				}
				//vnum dodawanego loadera
				argument = one_argument( argument, arg );
				if ( is_number( arg ) )
				{
					vnum = atoi( arg );
				}
				else
				{
					send_to_char( "Zle podales : <vnum  room'a/obj'ektu/moba>.\n\r", ch );
					return;
				}

				switch ( type )
				{
					case 0:
						if ( ( croom = get_room_index( vnum ) ) == NULL )
						{
							send_to_char( "Taki room nie istnieje.\n\r", ch );
							return;
						}
						break;
					case 1:
						if ( ( cobj = get_obj_index( vnum ) ) == NULL )
						{
							send_to_char( "Taki objekt nie istnieje.\n\r", ch );
							return;
						}
						if ( cobj->item_type != ITEM_CONTAINER )
						{
							send_to_char( "Ten obiekt to nie CONTAINER.\n\r", ch );
							return;
						}
						break;
					case 2:
						if ( get_mob_index( vnum ) == NULL )
						{
							send_to_char( "Taki mob nie istnieje.\n\r", ch );
							return;
						}
						break;
				}

				//czytamy prawdopodobienstwo
				argument = one_argument( argument, arg );
				if ( is_number( arg ) )
				{
					pd = atoi( arg );
				}
				else
				{
					send_to_char( "Zle podales : prawd1.\n\r", ch );
					return;
				}
				argument = one_argument( argument, arg );
				if ( is_number( arg ) )
				{
					pu = atoi( arg );
				}
				else
				{
					send_to_char( "Zle podales : prawd2.\n\r", ch );
					return;
				}
				//lecim po liscie loaderow
				for ( load = art->loader;load;load = load->next )
				{
					//zmiana jesli istnieje
					if ( load->type == type && load->vnum == vnum )
					{
						load->probdown = pd;
						load->probup = pu;
						send_to_char( "Prawdopodobienstwo zmienione.\n\r", ch );
						return;
					}
					if ( load->next == NULL ) break;
				}
				//dodajemy
				loader = new_artefact_loader();
				if ( art->loader == NULL )
					art->loader = loader;
				else
					load->next = loader;

				loader->type = type;
				loader->vnum = vnum;
				loader->probdown = pd;
				loader->probup = pu;
				loader->next = NULL;

				send_to_char( "Dodany loader artefaktu.\n\r", ch );

				return;

			} //end add
			else if ( !str_cmp( arg, "del" ) )
			{
				//jaki typ
				argument = one_argument( argument, arg );
				if ( !str_cmp( arg, "room" ) ) type = 0;
				else if ( !str_cmp( arg, "obj" ) ) type = 1;
				else if ( !str_cmp( arg, "mob" ) ) type = 2;
				else
				{
					send_to_char( "Zle podales : <room/obj/mob>.\n\r", ch );
					return;
				}
				//vnum dodawanego loadera
				argument = one_argument( argument, arg );
				if ( is_number( arg ) )
				{
					vnum = atoi( arg );
				}
				else
				{
					send_to_char( "Zle podales : <vnum  room'a/obj'ektu/moba>.\n\r", ch );
					return;
				}
				//po liscie go teraz szukamy
				for ( load = art->loader;load;load = load->next )
				{
					if ( load->type == type && load->vnum == vnum )
					{
						if ( prev != NULL )
							prev->next = load->next;
						else
							art->loader = load->next;

						load->next = NULL;
						free_artefact_loader( load );
						send_to_char( "Loader usuniety z listy.\n\r", ch );
						return;
					}
					else
					{
						prev = load;
					}
				}
				send_to_char( "Taki loader nie istnieje na liscie.\n\r", ch );
				return;
			} //end del
			else
			{
				send_to_char( "Sposób spo¿ycia:\n\r", ch );
				send_to_char( "amod <vnum artefaktu> <add/del> <room/obj/mob> <room/obj/mob vnum> <prawd1> <prawd2>.\n\r", ch );
				return;
			}


		} //end to ten artefact
		else if ( art->next == NULL )
		{
			send_to_char( "Takiego artefaktu nie ma na li¶cie!\n\r", ch );
			return;
		}
	}


}

void do_aset( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_STRING_LENGTH ];
	ARTEFACT_DATA *ad, *tmp = NULL, *prev = NULL;
	ARTEFACT_OWNER *otmp = NULL;
	ARTEFACT_LOADER *ltmp = NULL;
	int vn, arg1;

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Komenda s³u¿y do modyfikowania listy artefaktów.\n\r", ch );
		send_to_char( "aset add <vnum> <ilo¶æ>\n\r", ch );
		send_to_char( "aset del <vnum>\n\r", ch );
		send_to_char( "aset day <vnum> <ilosc dni>\n\r", ch );
		send_to_char( "aset save\n\r", ch );
		send_to_char( "\n\r", ch );
		send_to_char( "Aby zmieniæ maksymaln± liczbê wyst±pieñ: aset add vnum <nowa liczba wyst±pieñ>.\n\r", ch );
		send_to_char( "Poleceniem <day> ustawiamy ilosci po ktorej artefakt znika nieaktywnemu graczowi.\n\r", ch);
		send_to_char( "Proszê pamiêtaæ, ¿e aset ustawia artefakt, a nie ustawia jego loadera.\n\r", ch );
		send_to_char( "Loader(y) artefaktu ustawiamy komend± {Mamod{x.\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );

	if ( !str_cmp( arg, "add" ) )
	{
		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie podano vnuma objektu.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		if ( is_number( arg ) )
			vn = atoi( arg );
		else
		{
			send_to_char( "Podano b³êdny vnum objektu.\n\r", ch );
			return;
		}

		if ( get_obj_index( vn ) == NULL )
		{
			send_to_char( "Przedmiot o takim vnum'ie nie istnieje.\n\r", ch );
			return;
		}

		for ( tmp = artefact_system;tmp ;tmp = tmp->next )
		{
			if ( tmp->avnum == vn )
			{
				send_to_char( "{RPrzedmiot jest ju¿ na li¶cie artefaktów!{x\n\r", ch );

				argument = one_argument( argument, arg );
				if ( is_number( arg ) )
				{
					arg1 = atoi( arg );
					tmp->max_count = arg1;
					send_to_char( "Zmieniono maksymaln± liczbê wyst±pieñ.\n\r", ch );
				}

				return;
			}

			if ( tmp->next == NULL )
			{
				argument = one_argument( argument, arg );
				if ( is_number( arg ) )
					arg1 = atoi( arg );
				else
				{
					send_to_char( "Podano b³êdn± MAKSYMALN¡ liczbê wyst±pieñ artefaktu.\n\r", ch );
					return;
				}

				ad = new_artefact_data();
				ad->avnum = vn;
				ad->max_count = arg1;
		ad->max_day = 20; //MAX_ARTEFACT_DAY

				ad->next = NULL;
				ad->first_owner = NULL;
				tmp->next = ad;
				send_to_char( "{GArtefakt zosta³ dodany do listy.{x\n\r", ch );
				return;
			}
		}
		if ( tmp == NULL )
		{
			argument = one_argument( argument, arg );
			if ( is_number( arg ) )
				arg1 = atoi( arg );
			else
			{
				send_to_char( "Podano b³êdn± MAKSYMALN¡ liczbê wyst±pieñ artefaktu.\n\r", ch );
				return;
			}
			ad = new_artefact_data();
			ad->avnum = vn;
			ad->max_count = arg1;
		ad->max_day = 20;//MAX_ARTEFACT_DAY

			ad->next = NULL;
			ad->first_owner = NULL;
			artefact_system = ad;
			send_to_char( "{RArtefakt zosta³ dodany do listy. (pierwszy){x\n\r", ch );
			return;
		}
	}

	else if ( !str_cmp( arg, "del" ) )
	{
		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie podano vnuma obiektu.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		if ( is_number( arg ) )
			vn = atoi( arg );
		else
		{
			send_to_char( "Podano b³êdny vnum obiektu.\n\r", ch );
			return;
		}

		if ( get_obj_index( vn ) == NULL )
		{
			send_to_char( "Przedmiot o takim vnumie nie istnieje.\n\r", ch );
			return;
		}


		for ( ad = artefact_system;ad;ad = ad->next )
		{
			if ( ad->avnum == vn )
			{

				if ( prev != NULL )
					prev->next = ad->next;

				if ( artefact_system->avnum == ad->avnum )
					artefact_system = ad->next;

				/*tu likwidujem ownerow*/
				if ( ad->first_owner != NULL )
				{
					otmp = ad->first_owner;
					ad->first_owner = NULL;
					for ( ;otmp;otmp = otmp->next )    //hmm nie wiem czy to dobrze
						free_artefact_owner( otmp );
				}
				/*i loaderow*/
				if ( ad->loader != NULL )
				{
					ltmp = ad->loader;
					ad->loader = NULL;
					for ( ;ltmp;ltmp = ltmp->next )    //hmm nie wiem czy to dobrze
						free_artefact_loader( ltmp );
				}

				ad->next = NULL;
				free_artefact_data( ad );
				send_to_char( "Przedmiot zosta³ usuniêty z listy artefaktów.\n\r", ch );
				return;
			}
			else
			{
				prev = ad;
			}
		}
		send_to_char( "Przedmiotu o podanym vnumie nie ma na li¶cie artefaktów.\n\r", ch );
		return;
	}
	else if (!str_cmp(arg,"day"))
	{
	if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Nie podano vnuma obiektu.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );

		if ( is_number( arg ) )
			vn = atoi( arg );
		else
		{
			send_to_char( "Podano b³êdny vnum obiektu.\n\r", ch );
			return;
		}

		if ( get_obj_index( vn ) == NULL )
		{
			send_to_char( "Przedmiot o podanym vnumie nie istnieje.\n\r", ch );
			return;
		}


	argument = one_argument( argument, arg );
			if ( is_number( arg ) ) arg1 = atoi( arg );
		else return;

		if (artefact_system != NULL)
		for(ad=artefact_system;ad;ad=ad->next)
		{
		if (ad->avnum == vn)
			ad->max_day = arg1;
		}

	return;

	}
	else if ( !str_cmp( arg, "save" ) )
	{


		FILE * fp;

		/*	if(artefact_system==NULL)
				return; */
		fclose( fpReserve );

		if ( ( fp = fopen( ARTEFACT_FILE, "w" ) ) == NULL )
			return;

		if ( artefact_system != NULL )
			for ( ad = artefact_system;ad;ad = ad->next )
			{
				fprintf( fp, "a %d %d %d %d\n", ad->avnum, ad->count, ad->max_count, ad->max_day );
				for ( otmp = ad->first_owner;otmp;otmp = otmp->next )
					fprintf( fp, "o %s~ %d\n", otmp->owner, otmp->status );
				for ( ltmp = ad->loader;ltmp;ltmp = ltmp->next )
					fprintf( fp, "l %d %d %d %d\n", ltmp->type, ltmp->vnum, ltmp->probdown, ltmp->probup );
			}
		else
			send_to_char( "LISTA JEST PUSTA!!!\n\r", ch );

		fclose( fp );
		fpReserve = fopen( NULL_FILE, "r" );
		send_to_char( "Lista artefaktow zosta³a zapisana.\n\r", ch );
		return;
	}

}

void do_astat( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	char arg[ MAX_STRING_LENGTH ];
	ARTEFACT_DATA *tmp = NULL;
	ARTEFACT_OWNER *otmp = NULL;
	ARTEFACT_LOADER *ltmp = NULL;
	char *type;
	int vn, czas, ii;

	BUFFER *buffer = NULL;
	bool znacznik = FALSE;

	if ( artefact_system == NULL )
	{
		send_to_char( "Lista artefaktów jest pusta.\n\r", ch );
		return;
	}


	if ( argument[ 0 ] == '\0' )
	{
		buffer = new_buf();
		sprintf( buf, "Sk³adnia:\n\r"
				 "astat all    - lista artefaktów\n\r"
				 "astat <vnum> - dok³adne informacje na temat artefaktu o danym vnumie\n\r"
				 "astat <imie> - jakie artefakty ma dany gracz\n\r"
				 "astat now    - artefakty aktualnie w grze\n\r" );
		add_buf( buffer, buf );
		page_to_char( buf_string( buffer ), ch );
		free_buf( buffer );
		return;
	}


	argument = one_argument( argument, arg );

	if ( is_number( arg ) )
	{
		vn = atoi( arg );

		if ( !get_obj_index( vn ) )
		{
			send_to_char( "Taki obiekt nie istnieje.\n\r", ch );
			return;
		}

		buffer = new_buf();
		for ( tmp = artefact_system;tmp;tmp = tmp->next )
		{
			if ( tmp->avnum == vn )
			{
				znacznik = TRUE;
				sprintf( buf, "Vnum: %d       %s\n\r", tmp->avnum, get_obj_index( tmp->avnum ) ->short_descr );
				add_buf( buffer, buf );
				sprintf( buf, "Ilo¶æ: %d/%d\n\r", tmp->count, tmp->max_count );
				add_buf( buffer, buf );
		sprintf( buf, "Maksymalnie mozna rentowac : %d dni", tmp->max_day);
		add_buf(buffer, buf);

				if ( tmp->first_owner != NULL )
				{
					sprintf( buf, "	{GPosiadacze{x:			ostatni log\n\r" );
					add_buf( buffer, buf );
				}
				else
				{
					sprintf( buf, "\n\r{RNIKT GO JESZCZE NIE MA!{x\n\r" );
					add_buf( buffer, buf );
				}

		ii = 1;
				for ( otmp = tmp->first_owner;otmp;otmp = otmp->next )
				{
			czas = (current_time - (otmp->last_logoff)) / (24*60*60);
					sprintf( buf, "       %d)   %s, %s	%d\n\r",ii, otmp->owner, otmp->status == 1 ? "gra" : "rent" , czas);
					add_buf( buffer, buf );
				ii++;
		}
				if ( tmp->loader != NULL )
				{
					sprintf( buf, "        £aduje siê w(na):\n\r" );
					add_buf( buffer, buf );
				}
				else
				{
					sprintf( buf, "Nigdzie siê nie ³aduje.\n\r" );
					add_buf( buffer, buf );
				}

				for ( ltmp = tmp->loader;ltmp;ltmp = ltmp->next )
				{
					if ( ltmp->type == 0 ) type = "room";
					else if ( ltmp->type == 1 ) type = "obj";
					else if ( ltmp->type == 2 ) type = "mob";
					sprintf( buf, "                %s : %d, prawdopodobieñstwo : %d/%d\n\r", type, ltmp->vnum, ltmp->probdown, ltmp->probup );
					add_buf( buffer, buf );
				}
			}
		}

		if ( znacznik )
		{
			sprintf( buf, "\n\r Wedlug funkcji {ROWHERE{x ten przedmiot znajduje sie:\n\r" );
			add_buf( buffer, buf );
			page_to_char( buf_string( buffer ), ch );
			free_buf( buffer );
			do_function( ch, &awhere, arg );
		}

		free_buf( buffer );
		return;
	}
	else if ( !str_cmp( arg, "old" ) )
	{
		buffer = new_buf();
		sprintf (buf, "Arty/rary które w najbli¿szym czasie znikn±:\n\r");
		add_buf (buffer, buf);

		for (tmp = artefact_system;tmp;tmp = tmp->next)
		{
			for (otmp = tmp->first_owner;otmp;otmp = otmp->next)
			{
				czas = (current_time - (otmp->last_logoff)) / (24*60*60);
				if ( czas >= tmp->max_day-1 )
				{
					sprintf ( buf, "  %s, %d(%d), %5s\n\r", otmp->owner, czas,tmp->max_day, get_obj_index( tmp->avnum ) ->short_descr);
					add_buf(buffer,buf);
				}
			}
		}
		page_to_char (buf_string (buffer), ch);
		free_buf (buffer);
		return;

	}
	else if ( !str_cmp( arg, "all" ) )
	{
		buffer = new_buf();
		sprintf( buf, "vnum  ilo¶æ/limit		max trzymanie(dni) nazwa								\n\r\n\r" );
		add_buf( buffer, buf );
		for ( tmp = artefact_system;tmp;tmp = tmp->next )
		{
			sprintf( buf, "[%5d] ", tmp->avnum );
			add_buf( buffer, buf );
			if ( tmp->max_count > 0 )
			{
				sprintf( buf, " Ilo¶æ: %2d/%2d		%d", tmp->count, tmp->max_count, tmp->max_day );
				add_buf( buffer, buf );
			}
			else
			{
				sprintf( buf, "{Gnikt go nie ma{x	%d, ", tmp->max_day);
				add_buf( buffer, buf);
			}
			sprintf( buf, "[%-50s] \n\r", get_obj_index( tmp->avnum ) ->short_descr );
			add_buf( buffer, buf );

	}

		if ( buffer != NULL )
		{
			page_to_char( buf_string( buffer ), ch );
			free_buf( buffer );
		}
		return;
	}
	else if ( !str_cmp( arg, "now" ) )
	{
		buffer = new_buf();
		sprintf( buf, "Lista osób w grze z artefaktami (tylko pc):\n\r" );
		add_buf( buffer, buf );
		for ( tmp = artefact_system;tmp;tmp = tmp->next )
		{
			sprintf( buf, " [%5d] %s:\n\r", tmp->avnum, get_obj_index( tmp->avnum ) ->short_descr );

			znacznik = FALSE;
			for ( otmp = tmp->first_owner;otmp;otmp = otmp->next )
			{
				if ( otmp->status == 1 )
				{
					if ( !znacznik )
					add_buf( buffer, buf );

					znacznik = TRUE;
					sprintf( buf, "%15s\n\r", otmp->owner );
					add_buf( buffer, buf );
				}
			}
		}

		page_to_char( buf_string( buffer ), ch );
		free_buf( buffer );
		return;
	}
	else
	{
		buffer = new_buf();
		sprintf( buf, "%s ma nastêpuj±ce artefakty:\n\r\n\r", arg );
		add_buf( buffer, buf );
	znacznik = FALSE;
		for ( tmp = artefact_system;tmp;tmp = tmp->next )
		{
			for ( otmp = tmp->first_owner;otmp;otmp = otmp->next )
			{
				if ( !str_cmp( arg, otmp->owner ) )
				{
			znacznik = TRUE;
				czas = (current_time - (otmp->last_logoff)) / (24*60*60);
					sprintf( buf, "[%5d] %-40s\n\r ", tmp->avnum, get_obj_index( tmp->avnum ) ->short_descr );
					add_buf( buffer, buf );
				}
			}

		}


		if ( buffer != NULL && znacznik )
		{
		sprintf(buf, "Ostatnio zalogowany {R%d{x dni temu", czas);
		add_buf(buffer,buf);
			page_to_char( buf_string( buffer ), ch );
			free_buf( buffer );
		}
		return;
	}

	if ( buffer != NULL )
	{
		page_to_char( buf_string( buffer ), ch );
		free_buf( buffer );
	}
}

//rellik: komponenty, wy¶wietlanie tabeli komponentów po numerze slotu, grupie, czarze, numerze kluczowym
void do_sitable( CHAR_DATA *ch, char *argument )
{
	char arg1[ MAX_STRING_LENGTH ];
	char arg2[ MAX_STRING_LENGTH ];
	int i, el;
	int nr = -1;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	el = spell_items_table_count();

	if ( arg1[0] == '\0' )
	{
		print_char( ch, "Sk³adnia:\n\r" );
		print_char( ch, "SITable    - lista u¿ywanych komponentów wg slotu, klucza, grupy lub czaru\n\r" );
		print_char( ch, "sitable [slot/key/group/sn/obj. name] <nr>\n\r" );
		print_char( ch, "sitable all\n\r" );
		print_char( ch, "[slot/key/group/sn] - wybraæ jedno.\n\r" );
		print_char( ch, "slot - numer slotu w tabeli, key - nr.klucz, group - numer grupy, sn - numer czaru, all - wszystkie, obj.name - nazwa obiektu.\n\r" );
		return;
	}

	if ( str_cmp( arg1, "all" ) &&  arg2[0] == '\0' )
	{
		print_char( ch, "Dla slot/key/group/sn oczekiwany jest parametr.\n\r" );
		return;
	}

	if ( str_cmp( arg1, "slot" ) && str_cmp( arg1, "key" ) && str_cmp( arg1, "group" ) && str_cmp( arg1, "sn" ) && str_cmp( arg1, "name" ) && str_cmp( arg1, "all" ) )
	{
		print_char( ch, "B³êdny parametr. Proszê podaæ slot lub key lub group lub sn lub all lub name.\n\r" );
		return;
	}

	if ( is_number( arg2 ) ) nr = atoi( arg2 );

	//najpierw slot
	if ( !str_cmp( arg1, "slot" ) )
	{
		if ( nr < 0 || nr > el )
		{
			print_char( ch, "Numer slotu musi byæ warto¶ci± z zakresu od 0 do %d.\n\r", el );
			return;
		}
		print_char( ch, "Komponent %d:	key: %d,	spell: %d,	group:  %d,	nazwa obiektu: %s,	nazwa w³asna: %s,\n\r", nr,
				spell_items_table[nr].key_number, spell_items_table[nr].spell_number, spell_items_table[nr].group_nb, spell_items_table[nr].spell_item_name,
				spell_items_table[nr].official_spell_item_name );
		print_char( ch, "nazwa w³asna czaru: %s, 	%s,\n\r",
				spell_items_table[nr].spell_name, spell_items_table[nr].item_blows? "znika po wyczerpaniu": "nie znika po wyczerpaniu" );
		print_char( ch, "Komunikat przy u¿yciu: %s", spell_items_table[nr].action_description );
		return;
	}

	print_char( ch, "slot,  key nr,	 sn,  group,  nazwa obiektu,  nazwa w³asna,  nazwa czaru\n\r" );
	for ( i = 0; spell_items_table[i].key_number != 0; ++i )
	{
		if ( !str_cmp( arg1, "all" )
				|| ( !str_cmp( arg1, "key" ) && spell_items_table[i].key_number == nr )
				|| ( !str_cmp( arg1, "group" ) && spell_items_table[i].group_nb == nr )
				|| ( !str_cmp( arg1, "sn" ) && spell_items_table[i].spell_number == nr )
				|| ( !str_cmp( arg1, "name" ) && !str_prefix( arg2, spell_items_table[i].spell_item_name ) )
				)
		{
			print_char( ch, "	%d,	%d, %d, %d, %s, %s, %s\n\r",
			i, spell_items_table[i].key_number, spell_items_table[i].spell_number, spell_items_table[i].group_nb, spell_items_table[i].spell_item_name,
			spell_items_table[i].official_spell_item_name, spell_items_table[i].spell_name );
			continue;
		}
	}
	return;
}

typedef struct lookup_filter
{
	int	parameter;
	int	val1;
	int	val2;
	int	val3;
	long * ext_val;
	char * str_val;
}
LOOKUP_FILTER;

static LOOKUP_FILTER *lookup_filter_list;
#define MAX_LOOKUP_PARAMETER 50

int check_objfilter( OBJ_INDEX_DATA *pObjIndex, int filter )
{
	AFFECT_DATA	* paf;
	int	result;

	if ( !pObjIndex )
		return FALSE;


	switch ( lookup_filter_list[ filter ].parameter )
	{
		case 1:     //type
			if ( lookup_filter_list[ filter ].val1 == 0 )
				return ( pObjIndex->item_type == lookup_filter_list[ filter ].val2 );
			else
				return ( pObjIndex->item_type != lookup_filter_list[ filter ].val2 );
		case 5:     //wear
			if ( lookup_filter_list[ filter ].val1 == 0 )
				return IS_SET( pObjIndex->wear_flags, lookup_filter_list[ filter ].val2 );
			else
				return !IS_SET( pObjIndex->wear_flags, lookup_filter_list[ filter ].val2 );
		case 6:     //extra
			if ( lookup_filter_list[ filter ].val1 == 0 )
				return ext_flags_same( lookup_filter_list[ filter ].ext_val, ext_flags_intersection( pObjIndex->extra_flags, lookup_filter_list[ filter ].ext_val ) );
			else
				return !ext_flags_same( lookup_filter_list[ filter ].ext_val, ext_flags_intersection( pObjIndex->extra_flags, lookup_filter_list[ filter ].ext_val ) );
		case 7:     //exwear
			if ( lookup_filter_list[ filter ].val1 == 0 )
				return ext_flags_same( lookup_filter_list[ filter ].ext_val, ext_flags_intersection( pObjIndex->wear_flags2, lookup_filter_list[ filter ].ext_val ) );
			else
				return !ext_flags_same( lookup_filter_list[ filter ].ext_val, ext_flags_intersection( pObjIndex->wear_flags2, lookup_filter_list[ filter ].ext_val ) );
		case 8:     //cost
			return ( num_eval( pObjIndex->cost, lookup_filter_list[ filter ].val1, lookup_filter_list[ filter ].val2 ) );
			break;
		case 9:     //weight
			return ( num_eval( pObjIndex->weight, lookup_filter_list[ filter ].val1, lookup_filter_list[ filter ].val2 ) );
			break;
		case 10:     //weight
			return ( num_eval( pObjIndex->rent_cost, lookup_filter_list[ filter ].val1, lookup_filter_list[ filter ].val2 ) );
			break;
		case 11:     //affect
			result = FALSE;
			for ( paf = pObjIndex->affected; paf; paf = paf->next )
			{
				if ( paf->location == lookup_filter_list[ filter ].val1 || lookup_filter_list[ filter ].val1 == 0 )
				{
					if ( num_eval ( paf->modifier, lookup_filter_list[ filter ].val2, lookup_filter_list[ filter ].val3 ) )
						result = TRUE;
				}
			}
			return result;
			break;
		case 12:     //bitvector
			for ( paf = pObjIndex->affected; paf; paf = paf->next )
			{
				if ( paf->bitvector && paf->bitvector != &AFF_NONE )
					return TRUE;
			}
			return FALSE;
			break;
		case 13:     //value
			return ( num_eval( pObjIndex->value[ lookup_filter_list[ filter ].val1 ], lookup_filter_list[ filter ].val2, lookup_filter_list[ filter ].val3 ) );
			break;
		case 14:     //null
			return ( strlen( pObjIndex->name ) == 0 || strlen( pObjIndex->name2 ) == 0 || strlen( pObjIndex->name3 ) == 0 || strlen( pObjIndex->name4 ) == 0 || strlen( pObjIndex->name5 ) == 0
					 || strlen( pObjIndex->name6 ) == 0 || strlen( pObjIndex->short_descr ) == 0 || strlen( pObjIndex->description ) == 0
					 || !str_cmp( pObjIndex->name, "no name" ) || !str_cmp( pObjIndex->name2, "null" ) || !str_cmp( pObjIndex->name3, "null" ) || !str_cmp( pObjIndex->name4, "null" )
					 || !str_cmp( pObjIndex->name5, "null" ) || !str_cmp( pObjIndex->name6, "null" ) || !str_cmp( pObjIndex->short_descr, "(no short description)" ) || !str_cmp( pObjIndex->description, "(no description)" ) );
			break;
		case 15:     //specdam
			return pObjIndex->spec_dam != NULL;
		case 16:     //bonus
			return pObjIndex->bonus_set == lookup_filter_list[ filter ].val1;
		case 17:     //name
			return is_name( lookup_filter_list[ filter ].str_val, pObjIndex->name );
		case 18:     //spell
			if ( pObjIndex->item_type != ITEM_SPELLBOOK )
				return FALSE;

			return ( pObjIndex->value[ 2 ] == lookup_filter_list[ filter ].val1 || pObjIndex->value[ 3 ] == lookup_filter_list[ filter ].val1 ||
					  pObjIndex->value[ 4 ] == lookup_filter_list[ filter ].val1 || pObjIndex->value[ 5 ] == lookup_filter_list[ filter ].val1 ||
					  pObjIndex->value[ 6 ] == lookup_filter_list[ filter ].val1 );
		case 19:     //spell_class_conflict
			if ( pObjIndex->item_type != ITEM_SPELLBOOK )
				return FALSE;

			int spell_book_class = pObjIndex->value[ 0 ];
			int counter;

			for ( counter = 2; counter < 7; counter++ )
			{
				if ( pObjIndex->value[ counter ] > 0 && pObjIndex->value[ counter ] < MAX_SKILL && skill_table[ pObjIndex->value[ counter ] ].spell_fun != spell_null )
				{
					if ( skill_table[ pObjIndex->value[ counter ] ].skill_level[ spell_book_class ] >= 32 )
						return TRUE;
				}
			}
			return FALSE;
	}
	return TRUE;
}

void do_objlookup( CHAR_DATA *ch, char *argument )
{
	const char * parameters[] =
		{
			"type", "start", "count", "area",
			"wear", "extra", "exwear", "cost",
			"weight", "rent", "affect", "bitvector",
			"value", "null", "specdam", "bonus", "name",
			"spell", "spell_class_conflict", NULL
		};
	char	buf [ MAX_STRING_LENGTH ];
	char	par [ MAX_INPUT_LENGTH ];
	char	val1 [ MAX_INPUT_LENGTH ];
	char	val2 [ MAX_INPUT_LENGTH ];
	char	val3 [ MAX_INPUT_LENGTH ];
	long *  ext_ival;
	OBJ_INDEX_DATA *pObjIndex;
	AREA_DATA	*pArea = NULL;
	BUFFER	*buffer = NULL;
	int	vnum;
	int	parameter;
	int	par_count = 0;
	int	ifilter;
	int	i;
	int	i2;
	int	ival1;
	int	ival2;
	bool	passed;
	int	start = 0;
	int	count = 0;

	argument = one_argument( argument, par );
	if ( par[ 0 ] == '\0' )
	{
		buffer = new_buf();
		add_buf( buffer, "Skladnia:\n\r" );
		add_buf( buffer, "{Cobjlookup #parametr1 warto¶æ warto¶æ #paramter2 warto¶æ warto¶æ...{x\n\r" );
		add_buf( buffer, "Parametry:\n\r" );
		add_buf( buffer, "{C#type [-]<typ przedmiotu>   {x- wy¶wietla przedmioty danego typu lub te, ktore\n\r" );
		add_buf( buffer, "                              nie maj± takiego typu ( '-' przed typem )\n\r" );
		add_buf( buffer, "{C#start <liczba>             {x- od jakiej pozycji listy zacz±æ wy¶wietlanie\n\r" );
		add_buf( buffer, "{C#count <liczba>             {x- ile przedmiotów wyswietlic\n\r" );
		add_buf( buffer, "{C#name <string>              {x- szukanie przedmiotów o okre¶lonej nazwie\n\r" );
		add_buf( buffer, "{C#area                       {x- przeszukiwanie tylko aktualnej krainy\n\r" );
		add_buf( buffer, "{C#wear [-]<flaga wear>       {x- szukanie przedmiotów o okre¶lonych flagach wear\n\r" );
		add_buf( buffer, "{C#extra [-]<flaga extra>     {x- szukanie przedmiotów o okre¶lonych flagach extra\n\r" );
		add_buf( buffer, "{C#exwear [-]<flaga exwear>   {x- szukanie przedmiotów o okre¶lonych flagach exwear\n\r" );
		add_buf( buffer, "{C#cost <operator> <liczba>   {x- szukanie przedmiotów o okre¶lonej cenie\n\r" );
		add_buf( buffer, "{C#weight <operator> <liczba> {x- szukanie przedmiotów o okre¶lonej wadze\n\r" );
		add_buf( buffer, "{C#rent <operator> <liczba>   {x- szukanie przedmiotów o okreslonym koszcie renta\n\r" );
		add_buf( buffer, "{C#affect any|<typ affecta> <operator> <liczba>\n\r" );
		add_buf( buffer, "                            {x- szukanie przedmiotów o okre¶lonych affectach\n\r" );
		add_buf( buffer, "{C#bitvector                  {x- przedmioty majace jakiekolwiek flagi affectow\n\r" );
		add_buf( buffer, "{C#specdam                    {x- przedmioty majace jakiekolwiek special damage\n\r" );
		add_buf( buffer, "{C#bonus <vnum>               {x- przedmioty nale¿±ce do seta o podanym vnumie\n\r" );
		add_buf( buffer, "{C#value <numer> <operator> <liczba>\n\r" );
		add_buf( buffer, "                            {x- szukanie przedmiotów o okre¶lonych warto¶æiach\n\r" );
		add_buf( buffer, "                              pol v0, v1 itd\n\r" );
		add_buf( buffer, "{C#spell <nazwa czaru>        {x- ksiêgi z okre¶lonym czarem\n\r" );
		add_buf( buffer, "{C#spell_class_conflict       {x- ksiêgi ze ¼le ustawion± profesj± w stosunku do\n\r" );
		add_buf( buffer, "                              czarów\n\r" );
		add_buf( buffer, "{C#null                       {x- szukanie przedmiotów, które nie maj± odmiany,\n\r" );
		add_buf( buffer, "                              shorta lub longa\n\r" );
		add_buf( buffer, "Przedmiot musi spe³niæ wszystkie warunki aby zosta³ wy¶wietlony.\n\r" );
		page_to_char( buf_string( buffer ), ch );
		free_buf( buffer );
		return;
	}

	if ( ch->lines == 0 )
	{
		send_to_char( "Proszê w³±czyæ buforowanie strony (komenda config scroll <liczba linii>).\n\r", ch );
		return;
	}

	if ( !lookup_filter_list )
	{
		if ( ( lookup_filter_list = malloc( MAX_LOOKUP_PARAMETER * sizeof * lookup_filter_list ) ) == NULL )
		{
			send_to_char( "Za ma³o pamieci do wykonania tej komendy.\n\r", ch );
			return;
		}

		for ( i = 0; i < MAX_LOOKUP_PARAMETER; i++ )
		{
			lookup_filter_list[ i ].parameter = 0;
			lookup_filter_list[ i ].val1 = 0;
			lookup_filter_list[ i ].val2 = 0;
			lookup_filter_list[ i ].val3 = 0;
			lookup_filter_list[ i ].str_val = &str_empty[0];
		}
	} else
	{
		for ( i = 0; i < MAX_LOOKUP_PARAMETER; i++ )
		{
			lookup_filter_list[ i ].parameter = 0;
			lookup_filter_list[ i ].val1 = 0;
			lookup_filter_list[ i ].val2 = 0;
			lookup_filter_list[ i ].val3 = 0;
			free_string( lookup_filter_list[ i ].str_val );
			lookup_filter_list[ i ].str_val = &str_empty[0];
		}
	}

	for ( ; ; )
	{
		if ( par[ 0 ] == '\0' ) break;
		if ( par[ 0 ] != '#' )
		{
			print_char( ch, "Nieprawid³owy parametr: %s\n\r", par );
			return;
		}

		parameter = 0;
		for ( i = 0; parameters[ i ]; i++ )
		{
			if ( !str_cmp( par + 1, parameters[ i ] ) )
			{
				parameter = i + 1;
				break;
			}
		}
		if ( parameter == 0 )
		{
			print_char( ch, "Nieprawid³owy parametr: %s\n\r", par );
			return;
		}

		switch ( parameter )
		{
			case 1:     //type
				argument = one_argument( argument, val2 );
				if ( val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				ival1 = 0;
				if ( val2[ 0 ] == '-' )
				{
					ival1 = 1;
					sprintf( val2, "%s", val2 + 1 );
				}

				if ( ( ival2 = flag_value( type_flags, val2 ) ) == NO_FLAG )
				{
					print_char( ch, "Nieprawid³owa typ przedmiotu: %s\n\r", val2 );
					return;
				}

                DEBUG_INFO("objlookup:parameter:1:lookup_filter_list:before");
				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].val2 = ival2;

				par_count++;
				break;
			case 2:     //start
				argument = one_argument( argument, val1 );
				if ( !is_number( val1 ) )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}
				start = UMAX( 0, atoi( val1 ) );
				break;
			case 3:     //count
				argument = one_argument( argument, val1 );
				if ( !is_number( val1 ) )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}
				count = UMAX( 0, atoi( val1 ) );
				break;
			case 4:     //area
				pArea = ch->in_room->area;
				break;
			case 5:     //wear
				argument = one_argument( argument, val2 );
				if ( val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				ival1 = 0;
				if ( val2[ 0 ] == '-' )
				{
					ival1 = 1;
					sprintf( val2, "%s", val2 + 1 );
				}

				if ( ( ival2 = flag_value( wear_flags, val2 ) ) == NO_FLAG )
				{
					print_char( ch, "Nieprawid³owa flaga wear: %s\n\r", val2 );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].val2 = ival2;

				par_count++;
				break;
			case 6:     //extra
				argument = one_argument( argument, val2 );
				if ( val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				ival1 = 0;
				if ( val2[ 0 ] == '-' )
				{
					ival1 = 1;
					sprintf( val2, "%s", val2 + 1 );
				}

				ext_ival = ext_flag_value( extra_flags, val2 );
				if ( ext_flags_none( ext_ival ) )
				{
					print_char( ch, "Nieprawid³owa flaga extra: %s\n\r", val2 );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].ext_val = ext_ival;

				par_count++;
				break;
			case 7:     //exwear
				argument = one_argument( argument, val2 );
				if ( val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				ival1 = 0;
				if ( val2[ 0 ] == '-' )
				{
					ival1 = 1;
					sprintf( val2, "%s", val2 + 1 );
				}

				ext_ival = ext_flag_value( wear_flags2, val2 );
				if ( ext_flags_none( ext_ival ) )
				{
					print_char( ch, "Nieprawid³owa flaga exwear: %s\n\r", val2 );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].ext_val = ext_ival;

				par_count++;
				break;
			case 8:     //cost
			case 9:     //weight
			case 10:     //rent
				argument = one_argument( argument, val1 );
				argument = one_argument( argument, val2 );

				if ( val1[ 0 ] == '\0' || val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				if ( !is_number( val2 ) || ( ival1 = keyword_lookup( fn_evals, val1 ) ) < 0 )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].val2 = atoi( val2 );

				par_count++;
				break;
			case 11:     //affect
				argument = one_argument( argument, val1 );
				argument = one_argument( argument, val2 );
				argument = one_argument( argument, val3 );

				if ( val1[ 0 ] == '\0' || val2[ 0 ] == '\0' || val3[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				if ( str_cmp( val1, "any" ) && ( ival1 = flag_value( apply_flags, val1 ) ) == NO_FLAG )
				{
					print_char( ch, "Nieprawid³owa flaga exwear: %s\n\r", val1 );
					return;
				}

				if ( !is_number( val3 ) || ( ival2 = keyword_lookup( fn_evals, val2 ) ) < 0 )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				if ( !str_cmp( val3, "any" ) ) ival1 = 0;

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].val2 = ival2;
				lookup_filter_list[ par_count ].val3 = atoi( val3 );

				par_count++;
				break;
			case 12:     //bitvector
				lookup_filter_list[ par_count ].parameter = parameter;
				par_count++;
				break;
			case 13:     //value
				argument = one_argument( argument, val1 );
				argument = one_argument( argument, val2 );
				argument = one_argument( argument, val3 );

				if ( val1[ 0 ] == '\0' || val2[ 0 ] == '\0' || val3[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				if ( !is_number( val1 ) || !is_number( val3 ) || ( ival2 = keyword_lookup( fn_evals, val2 ) ) < 0 )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				if ( atoi( val1 ) < 0 || atoi( val1 ) > 6 )
				{
					send_to_char( "Value musi zawierac sie pomiedzy 0 a 6 wlacznie.\n\r", ch );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = atoi( val1 );
				lookup_filter_list[ par_count ].val2 = ival2;
				lookup_filter_list[ par_count ].val3 = atoi( val3 );

				par_count++;
				break;
			case 14:     //null
			case 15:     //specdam
			case 19:     //spell_class_conflict
				lookup_filter_list[ par_count ].parameter = parameter;
				par_count++;
				break;
			case 16:     //bonus
				argument = one_argument( argument, val1 );

				if ( val1[ 0 ] == '\0' || !is_number( val1 ) )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = atoi( val1 );

				par_count++;
				break;
			case 17:     //name
				argument = one_argument( argument, val1 );

				if ( val1[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].str_val = str_dup( val1 );

				par_count++;
				break;
			case 18:     //spell
				argument = one_argument( argument, val1 );

				if ( val1[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				ival1 = spell_only_lookup( val1 );

				if ( ival1 < 0 )
				{
					print_char( ch, "Nieprawid³owa nazwa czaru: %s\n\r", val1 );
					return;
				}


				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;

				par_count++;
				break;
			default:
				print_char( ch, "Nieprawid³owy parametr: %s\n\r", par );
				return;
		}
		argument = one_argument( argument, par );
	}

	buffer = new_buf();

	if ( pArea )
	{
		i = 0;
		i2 = 0;
		for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
		{
			if ( count != 0 && i2 >= count )
				break;

			if ( !( pObjIndex = get_obj_index( vnum ) ) )
				continue;

			passed = TRUE;

			for ( ifilter = 0; ifilter < par_count; ifilter++ )
				if ( check_objfilter ( pObjIndex, ifilter ) == 0 )
					passed = FALSE;

			if ( passed )
			{
				if ( start == 0 || i >= start )
				{
					sprintf( buf, "%3d. [%6d] %s [%s]\n\r", i2, pObjIndex->vnum, pObjIndex->name, pArea->name );
					add_buf( buffer, buf );
					i2++;
				}
				i++;
			}
		}
	}
	else
	{
        int value_number;
		i = 0;
        i2 = 0;
        DEBUG_INFO("objlookup:print:for:before");
		for ( vnum = 1; vnum < top_vnum_obj; vnum++ )
		{
			if ( count != 0 && i2 >= count )
				break;

			if ( !( pObjIndex = get_obj_index( vnum ) ) )
				continue;

			passed = TRUE;

			for ( ifilter = 0; ifilter < par_count; ifilter++ )
				if ( check_objfilter ( pObjIndex, ifilter ) == 0 )
					passed = FALSE;

			if ( passed )
            {
                if ( start == 0 || i >= start )
                {
                    sprintf( buf, "%3d. [%6d] %s [%s]", i2, pObjIndex->vnum, pObjIndex->name, pObjIndex->area->name );
                    add_buf( buffer, buf );
                    switch ( pObjIndex->item_type )
                    {
                        case ITEM_SCROLL:
                        case ITEM_POTION:
                        case ITEM_PILL:
                            sprintf( buf, " {R(%d){D", pObjIndex->value[ 0 ] );
                            add_buf( buffer, buf);
                            for (value_number = 1;value_number<5;value_number++)
                            {
                                if ( pObjIndex->value[ value_number ] > 0 && pObjIndex->value[ value_number ] < MAX_SKILL )
                                {
                                    sprintf( buf, ", %s", skill_table[ pObjIndex->value[ value_number ] ].name );
                                    add_buf( buffer, buf );
                                }
                            }
                            break;
                    }
                    sprintf( buf, "{x\n\r" );
                    add_buf( buffer, buf );
                    i2++;
                }
                i++;
            }
		}
	}
	if ( i2 == 0 )
		sprintf( buf, "Znaleziono przedmiotów: %d.\n\r", i2 );
	else
		sprintf( buf, "\n\rZnaleziono przedmiotów: %d.\n\r", i2 );
	add_buf( buffer, buf );
	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );
	return;
}

int check_mobfilter( MOB_INDEX_DATA *pMobIndex, int filter )
{
	int c;
	if ( !pMobIndex )
		return FALSE;


	switch ( lookup_filter_list[ filter ].parameter )
	{
		case 1:     //act
			if ( lookup_filter_list[ filter ].val1 == 0 )
				return ext_flags_same( lookup_filter_list[ filter ].ext_val, ext_flags_intersection( pMobIndex->act, lookup_filter_list[ filter ].ext_val ) );
			else
				return !ext_flags_same( lookup_filter_list[ filter ].ext_val, ext_flags_intersection( pMobIndex->act, lookup_filter_list[ filter ].ext_val ) );
		case 5:     //race
			if ( lookup_filter_list[ filter ].val1 == 0 )
				return ( pMobIndex->race == lookup_filter_list[ filter ].val2 );
			else
				return ( pMobIndex->race != lookup_filter_list[ filter ].val2 );
		case 6:     //align
			return ( num_eval( pMobIndex->alignment, lookup_filter_list[ filter ].val1, lookup_filter_list[ filter ].val2 ) );
			break;
		case 7:     //hitroll
			return ( num_eval( pMobIndex->hitroll, lookup_filter_list[ filter ].val1, lookup_filter_list[ filter ].val2 ) );
			break;
		case 8:     //damtype
			if ( lookup_filter_list[ filter ].val1 == 0 )
				return ( pMobIndex->dam_type == lookup_filter_list[ filter ].val2 );
			else
				return ( pMobIndex->dam_type != lookup_filter_list[ filter ].val2 );
		case 9:     //minhp
			return ( num_eval( pMobIndex->hit[ DICE_NUMBER ] + pMobIndex->hit[ DICE_BONUS ], lookup_filter_list[ filter ].val1, lookup_filter_list[ filter ].val2 ) );
			break;
		case 10:     //mindam
			return ( num_eval( pMobIndex->damage[ DICE_NUMBER ] * 1 + pMobIndex->damage[ DICE_BONUS ] + str_app[ pMobIndex->stats[ 0 ] ].todam, lookup_filter_list[ filter ].val1, lookup_filter_list[ filter ].val2 ) );
			break;
		case 18:     //minhp
			return ( num_eval( pMobIndex->hit[ DICE_NUMBER ] * pMobIndex->hit[ DICE_TYPE ] + pMobIndex->hit[ DICE_BONUS ], lookup_filter_list[ filter ].val1, lookup_filter_list[ filter ].val2 ) );
			break;
		case 19:     //mindam
			return ( num_eval( pMobIndex->damage[ DICE_NUMBER ] * pMobIndex->damage[ DICE_TYPE ] + pMobIndex->damage[ DICE_BONUS ] + str_app[ pMobIndex->stats[ 0 ] ].todam, lookup_filter_list[ filter ].val1, lookup_filter_list[ filter ].val2 ) );
			break;
		case 11:     //stats
			if ( lookup_filter_list[ filter ].val1 == 10 )
			{
				for ( c = 0; c < MAX_STATS; c++ )
				{
					if ( num_eval( pMobIndex->stats[ c ], lookup_filter_list[ filter ].val2, lookup_filter_list[ filter ].val3 ) )
						return TRUE;
				}
				return FALSE;
			}
			else
				return ( num_eval( pMobIndex->stats[ lookup_filter_list[ filter ].val1 ], lookup_filter_list[ filter ].val2, lookup_filter_list[ filter ].val3 ) );
			break;
		case 12:     //armor
			if ( lookup_filter_list[ filter ].val1 == 10 )
			{
				for ( c = 0; c < 4; c++ )
				{
					if ( num_eval( pMobIndex->ac[ c ], lookup_filter_list[ filter ].val2, lookup_filter_list[ filter ].val3 ) )
						return TRUE;
				}
				return FALSE;
			}
			else
				return ( num_eval( pMobIndex->ac[ lookup_filter_list[ filter ].val1 ], lookup_filter_list[ filter ].val2, lookup_filter_list[ filter ].val3 ) );
			break;
		case 13:     //affected
			if ( lookup_filter_list[ filter ].val1 == 0 )
				return ext_flags_same( lookup_filter_list[ filter ].ext_val, ext_flags_intersection( pMobIndex->affected_by, lookup_filter_list[ filter ].ext_val ) );
			else
				return !ext_flags_same( lookup_filter_list[ filter ].ext_val, ext_flags_intersection( pMobIndex->affected_by, lookup_filter_list[ filter ].ext_val ) );
		case 14:     //act
			if ( lookup_filter_list[ filter ].val1 == 0 )
				return ext_flags_same( lookup_filter_list[ filter ].ext_val, ext_flags_intersection( pMobIndex->off_flags, lookup_filter_list[ filter ].ext_val ) );
			else
				return !ext_flags_same( lookup_filter_list[ filter ].ext_val, ext_flags_intersection( pMobIndex->off_flags, lookup_filter_list[ filter ].ext_val ) );
		case 15:     //wealth
			return ( num_eval( pMobIndex->wealth, lookup_filter_list[ filter ].val1, lookup_filter_list[ filter ].val2 ) );
			break;
		case 16:     //language
			if ( lookup_filter_list[ filter ].val2 == LANG_UNKNOWN )
			{
				if ( lookup_filter_list[ filter ].val1 == 0 )
					return ( pMobIndex->languages == 0 );
				else
					return ( pMobIndex->languages != 0 );
			}
			if ( lookup_filter_list[ filter ].val1 == 0 )
				return IS_SET( pMobIndex->languages, lang_table[ lookup_filter_list[ filter ].val2 ].bit );
			else
				return !IS_SET( pMobIndex->languages, lang_table[ lookup_filter_list[ filter ].val2 ].bit );
		case 17:     //speaking
			if ( lookup_filter_list[ filter ].val1 == 0 )
				return ( pMobIndex->speaking == lookup_filter_list[ filter ].val2 );
			else
				return ( pMobIndex->speaking != lookup_filter_list[ filter ].val2 );
		case 20:     //level
			return ( num_eval( pMobIndex->level, lookup_filter_list[ filter ].val1, lookup_filter_list[ filter ].val2 ) );
			break;
		case 21:
			return ( pMobIndex->pShop != NULL );
			break;
		case 22:
			return ( pMobIndex->pRepair != NULL );
			break;
		case 23:
			return ( strlen( pMobIndex->player_name ) == 0 || strlen( pMobIndex->name2 ) == 0 || strlen( pMobIndex->name3 ) == 0 || strlen( pMobIndex->name4 ) == 0 || strlen( pMobIndex->name5 ) == 0
					 || strlen( pMobIndex->name6 ) == 0 || strlen( pMobIndex->short_descr ) == 0 || strlen( pMobIndex->long_descr ) == 0
					 || !str_cmp( pMobIndex->player_name, "no name" ) || !str_cmp( pMobIndex->name2, "null" ) || !str_cmp( pMobIndex->name3, "null" ) || !str_cmp( pMobIndex->name4, "null" )
					 || !str_cmp( pMobIndex->name5, "null" ) || !str_cmp( pMobIndex->name6, "null" ) || !str_cmp( pMobIndex->short_descr, "(no short description)" ) || !str_cmp( pMobIndex->long_descr, "(no long description)" ) );
		case 24:     //name
			return is_name( lookup_filter_list[ filter ].str_val, pMobIndex->player_name );
	}
	return TRUE;
}

void do_moblookup( CHAR_DATA *ch, char *argument )
{
	const char * parameters[] =
		{
			"act", "start", "count", "area",
			"race", "align", "hitroll", "damtype",
			"minhp", "mindam", "stats", "armor",
			"affected", "off", "wealth", "language",
			"speaking", "maxhp", "maxdam", "level", "shop",
			"repair", "null", "name", "shoptype", NULL
		};
	const char *stats[] =
		{
			"str", "int", "wis", "dex", "con", "cha", "luc", NULL
		};
	const char *armor[] =
		{
			"klujace", "obuchowe", "tnace", "inne", NULL
		};
	char	buf [ MAX_STRING_LENGTH ];
	char	par [ MAX_INPUT_LENGTH ];
	char	val1 [ MAX_INPUT_LENGTH ];
	char	val2 [ MAX_INPUT_LENGTH ];
	char	val3 [ MAX_INPUT_LENGTH ];
	MOB_INDEX_DATA *pMobIndex;
	AREA_DATA	*pArea = NULL;
	BUFFER	*buffer;
	int	vnum;
	int	parameter;
	int	par_count = 0;
	int	ifilter;
	int	i;
	int	i2;
	int	ival1;
	int	ival2;
	long * ext_ival;
	bool	passed;
	int	start = 0;
	int	count = 0;

	argument = one_argument( argument, par );
	if ( par[ 0 ] == '\0' )
	{
		buffer = new_buf();
		add_buf( buffer, "Skladnia:\n\r" );
		add_buf( buffer, "{Cmoblookup #parametr1 warto¶æ warto¶æ #paramter2 warto¶æ warto¶æ...{x\n\r" );
		add_buf( buffer, "Parametry:\n\r" );
		add_buf( buffer, "{C#start <liczba>              {x- od jakiej pozycji listy zacz±æ wy¶wietlanie\n\r" );
		add_buf( buffer, "{C#count <liczba>              {x- ile mobów wyswietlic\n\r" );
		add_buf( buffer, "{C#area                        {x- przeszukiwanie tylko aktualnej krainy\n\r" );
		add_buf( buffer, "{C#name <string>               {x- szukanie mobów o okre¶lonej nazwie\n\r" );
		add_buf( buffer, "{C#align <operator> <liczba>   {x- szukanie mobów o okreslonym alignmencie\n\r" );
		add_buf( buffer, "{C#hitroll <operator> <liczba> {x- szukanie mobów o okreslonym hitrollu\n\r" );
		add_buf( buffer, "{C#minhp <operator> <liczba>   {x- szukanie mobów o okreslonym minimalnym hp\n\r" );
		add_buf( buffer, "{C#maxhp <operator> <liczba>   {x- szukanie mobów o okreslonym maksymalnym hp\n\r" );
		add_buf( buffer, "{C#mindam <operator> <liczba>  {x- szukanie mobów o okreslonym minimalnym damagu\n\r" );
		add_buf( buffer, "{C#mindam <operator> <liczba>  {x- szukanie mobów o okreslonym maxymalnym damagu\n\r" );
		add_buf( buffer, "{C#wealth <operator> <liczba>  {x- szukanie mobów o okre¶lonej ilosci kasy\n\r" );

		add_buf( buffer, "{C#act [-]<flaga act>          {x- wy¶wietla przedmioty z dana flaga act\n\r" );
		add_buf( buffer, "                               lub bez tej flagi ( '-' przed typem )\n\r" );
		add_buf( buffer, "{C#race [-]<rasa>              {x- szukanie mobów okre¶lonej rasy\n\r" );
		add_buf( buffer, "{C#damtype [-]none|<damtype>   {x- szukanie mobów o okre¶lonym dam type\n\r" );
		add_buf( buffer, "{C#affected [-]<affect>        {x- szukanie mobów z okre¶lonymi affectami\n\r" );
		add_buf( buffer, "{C#off [-]<off>                {x- szukanie mobów z okre¶lonymi typami ataku\n\r" );
		add_buf( buffer, "{C#language [-]unknown<jezyk>  {x- szukanie mobów rozumiej±cych okre¶lone jêzyki\n\r" );
		add_buf( buffer, "{C#speaking [-]<jezyk>         {x- szukanie mobów mowiacych danym jêzykiem\n\r" );

		add_buf( buffer, "{C#armor any|klujace|obuchowe|tnace|inne <operator> <liczba>\n\r" );
		add_buf( buffer, "                             {x- szukanie mobów o okre¶lonych warto¶æiach AC\n\r" );
		add_buf( buffer, "{C#stats any|str|int|wis|dex|con|cha|luc <operator> <liczba>\n\r" );
		add_buf( buffer, "                             {x- szukanie mobów o okre¶lonych warto¶æiach statów\n\r" );
		add_buf( buffer, "{C#shop                        {x- szukanie mobów-sklepikarzy\n\r" );
		add_buf( buffer, "{C#shoptype <typ obiektu>      {x- filtrowanie sklepikarzy po towarze\n\r" );
		add_buf( buffer, "{C#repair                      {x- szukanie reperuj±cych mobów\n\r" );

		add_buf( buffer, "{C#null                        {x- szukanie mobow, które nie maj± odmiany,\n\r" );
		add_buf( buffer, "                               shorta lub longa\n\r" );
		add_buf( buffer, "Mob musi spe³niæ wszystkie warunki aby zostac wyswietlony.\n\r" );
		page_to_char( buf_string( buffer ), ch );
		free_buf( buffer );
		return;
	}

	if ( !lookup_filter_list )
	{
		if ( ( lookup_filter_list = malloc( MAX_LOOKUP_PARAMETER * sizeof * lookup_filter_list ) ) == NULL )
		{
			send_to_char( "Za ma³o pamieci do wykonania tej komendy.\n\r", ch );
			return;
		}

		for ( i = 0; i < MAX_LOOKUP_PARAMETER; i++ )
		{
			lookup_filter_list[ i ].parameter = 0;
			lookup_filter_list[ i ].val1 = 0;
			lookup_filter_list[ i ].val2 = 0;
			lookup_filter_list[ i ].val3 = 0;
			lookup_filter_list[ i ].str_val = &str_empty[0];
		}
	} else
	{
		for ( i = 0; i < MAX_LOOKUP_PARAMETER; i++ )
		{
			lookup_filter_list[ i ].parameter = 0;
			lookup_filter_list[ i ].val1 = 0;
			lookup_filter_list[ i ].val2 = 0;
			lookup_filter_list[ i ].val3 = 0;
			free_string( lookup_filter_list[ i ].str_val );
			lookup_filter_list[ i ].str_val = &str_empty[0];
		}
	}

	for ( ; ; )
	{
		if ( par[ 0 ] == '\0' ) break;
		if ( par[ 0 ] != '#' )
		{
			print_char( ch, "Nieprawid³owy parametr: %s\n\r", par );
			return;
		}

		parameter = 0;
		for ( i = 0; parameters[ i ]; i++ )
		{
			if ( !str_cmp( par + 1, parameters[ i ] ) )
			{
				parameter = i + 1;
				break;
			}
		}
		if ( parameter == 0 )
		{
			print_char( ch, "Nieprawid³owy parametr: %s\n\r", par );
			return;
		}

		switch ( parameter )
		{
			case 1:     //act
				argument = one_argument( argument, val2 );
				if ( val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				ival1 = 0;
				if ( val2[ 0 ] == '-' )
				{
					ival1 = 1;
					sprintf( val2, "%s", val2 + 1 );
				}

				ext_ival = ext_flag_value( act_flags, val2 );
				if ( ext_flags_none( ext_ival ) )
				{
					print_char( ch, "Nieprawid³owa flaga act: %s\n\r", val2 );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].ext_val = ext_ival;

				par_count++;
				break;
			case 2:     //start
				argument = one_argument( argument, val1 );
				if ( !is_number( val1 ) )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}
				start = UMAX( 0, atoi( val1 ) );
				break;
			case 3:     //count
				argument = one_argument( argument, val1 );
				if ( !is_number( val1 ) )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}
				count = UMAX( 0, atoi( val1 ) );
				break;
			case 4:     //area
				pArea = ch->in_room->area;
				break;
			case 5:     //race
				argument = one_argument( argument, val2 );
				if ( val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				ival1 = 0;
				if ( val2[ 0 ] == '-' )
				{
					ival1 = 1;
					sprintf( val2, "%s", val2 + 1 );
				}

				if ( !str_cmp( val2, "unique" ) )
				{
					ival2 = 0;
				}
				else if ( ( ival2 = race_lookup( val2 ) ) == 0 )
				{
					print_char( ch, "Nieprawid³owa rasa: %s\n\r", val2 );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].val2 = ival2;

				par_count++;
				break;
			case 6:     //align
			case 7:     //hitroll
			case 9:     //minhp
			case 10:     //mindam
			case 18:     //maxhp
			case 19:     //maxdam
			case 15:     //wealth
			case 20:     //level
				argument = one_argument( argument, val1 );
				argument = one_argument( argument, val2 );

				if ( val1[ 0 ] == '\0' || val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				if ( !is_number( val2 ) || ( ival1 = keyword_lookup( fn_evals, val1 ) ) < 0 )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].val2 = atoi( val2 );

				par_count++;
				break;
			case 8:     //damtype
				argument = one_argument( argument, val2 );
				if ( val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				ival1 = 0;
				if ( val2[ 0 ] == '-' )
				{
					ival1 = 1;
					sprintf( val2, "%s", val2 + 1 );
				}

				if ( !str_cmp( val2, "none" ) )
					ival2 = 0;
				else
					if ( ( ival2 = attack_lookup( val2 ) ) == 0 )
					{
						print_char( ch, "Nieprawid³owy typ ataku: %s\n\r", val2 );
						return;
					}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].val2 = ival2;

				par_count++;
				break;
			case 11:     //stats
			case 12:     //armor
				argument = one_argument( argument, val1 );
				argument = one_argument( argument, val2 );
				argument = one_argument( argument, val3 );

				if ( val1[ 0 ] == '\0' || val2[ 0 ] == '\0' || val3[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				if ( !is_number( val3 ) || ( ival2 = keyword_lookup( fn_evals, val2 ) ) < 0 )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				if ( parameter == 11 )
				{
					ival1 = -1;
					if ( str_cmp( val1, "any" ) )
						for ( i = 0; stats[ i ]; i++ )
						{
							if ( !str_cmp( val1, stats[ i ] ) )
							{
								ival1 = i;
								break;
							}
						}
					else
						ival1 = 10;

					if ( ival1 < 0 )
					{
						send_to_char( "Parametr stats moze przyjmowac warto¶æi: any str int wis dex con cha luc.\n\r", ch );
						return;
					}
				}
				else
					if ( parameter == 12 )
					{
						ival1 = -1;
						if ( str_cmp( val1, "any" ) )
							for ( i = 0; armor[ i ]; i++ )
							{
								if ( !str_cmp( val1, armor[ i ] ) )
								{
									ival1 = i;
									break;
								}
							}
						else
							ival1 = 10;

						if ( ival1 < 0 )
						{
							send_to_char( "Parametr armor mo¿e przyjmowaæ warto¶ci: any klujace obuchowe tnace inne.\n\r", ch );
							return;
						}
					}


				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].val2 = ival2;
				lookup_filter_list[ par_count ].val3 = atoi( val3 );

				par_count++;
				break;
			case 13:     //affected
				argument = one_argument( argument, val2 );
				if ( val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				ival1 = 0;
				if ( val2[ 0 ] == '-' )
				{
					ival1 = 1;
					sprintf( val2, "%s", val2 + 1 );
				}

				ext_ival = ext_flag_value( affect_flags, val2 );
				if ( ext_flags_none( ext_ival ) )
				{
					print_char( ch, "Nieprawid³owa flaga affected: %s\n\r", val2 );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].ext_val = ext_ival;

				par_count++;
				break;
			case 14:     //off
				argument = one_argument( argument, val2 );
				if ( val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				ival1 = 0;
				if ( val2[ 0 ] == '-' )
				{
					ival1 = 1;
					sprintf( val2, "%s", val2 + 1 );
				}

				ext_ival = ext_flag_value( off_flags, val2 );
				if ( ext_flags_none( ext_ival ) )
				{
					print_char( ch, "Nieprawid³owa flaga off: %s\n\r", val2 );
					return;
				}


				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].ext_val = ext_ival;

				par_count++;
				break;
			case 16:     //language
			case 17:     //speaking
				argument = one_argument( argument, val2 );
				if ( val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				ival1 = 0;
				if ( val2[ 0 ] == '-' )
				{
					ival1 = 1;
					sprintf( val2, "%s", val2 + 1 );
				}

				if ( !str_cmp( val2, "unknown" ) )
					ival2 = LANG_UNKNOWN;
				else
					if ( ( ival2 = get_langnum( val2 ) ) == -1 )
					{
						print_char( ch, "Nieprawid³owy jezyk: %s\n\r", val2 );
						return;
					}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].val2 = ival2;

				par_count++;
				break;
			case 21:     //shop
				lookup_filter_list[ par_count ].parameter = parameter;
				par_count++;
				break;
			case 22:     //repair
				lookup_filter_list[ par_count ].parameter = parameter;
				par_count++;
				break;
			case 23:     //null
				lookup_filter_list[ par_count ].parameter = parameter;
				par_count++;
				break;
			case 24:     //name
				argument = one_argument( argument, val1 );

				if ( val1[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].str_val = str_dup( val1 );

				par_count++;
				break;
			case 25:     //shoptype
				argument = one_argument( argument, val1 );

				if ( val1[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}
                break;
			default:
				print_char( ch, "Nieprawid³owy parametr: %s\n\r", par );
				return;
		}
		argument = one_argument( argument, par );
	}

	buffer = new_buf();

	if ( pArea )
	{
		i = 0;
		i2 = 0;
		for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
		{
			if ( count != 0 && i2 >= count )
				break;

			if ( !( pMobIndex = get_mob_index( vnum ) ) )
				continue;

			passed = TRUE;

			for ( ifilter = 0; ifilter < par_count; ifilter++ )
				if ( check_mobfilter ( pMobIndex, ifilter ) == 0 )
					passed = FALSE;

			if ( passed )
			{
				if ( start == 0 || i >= start )
				{
					sprintf( buf, "%3d. [%6d] %s\n\r", i2, pMobIndex->vnum, pMobIndex->short_descr );
					add_buf( buffer, buf );
					i2++;
				}
				i++;
			}
		}
	}
	else
	{
		i = 0;
		i2 = 0;
		for ( vnum = 1; vnum < top_vnum_mob; vnum++ )
		{
			if ( count != 0 && i2 >= count )
				break;

			if ( !( pMobIndex = get_mob_index( vnum ) ) )
				continue;

			passed = TRUE;

			for ( ifilter = 0; ifilter < par_count; ifilter++ )
				if ( check_mobfilter ( pMobIndex, ifilter ) == 0 )
					passed = FALSE;

			if ( passed )
			{
				if ( start == 0 || i >= start )
				{
					sprintf( buf, "%3d. [%6d] %s [%s]\n\r", i2, pMobIndex->vnum, pMobIndex->short_descr, pMobIndex->area->name );
					add_buf( buffer, buf );
					i2++;
				}
				i++;
			}
		}
	}
	if ( i2 == 0 )
		sprintf( buf, "Znaleziono mobów: %d.\n\r", i2 );
	else
		sprintf( buf, "\n\rZnaleziono mobów: %d.\n\r", i2 );
	add_buf( buffer, buf );
	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );
	return;
}

int check_roomfilter( ROOM_INDEX_DATA *pRoomIndex, int filter, AREA_DATA *pArea )
{
	int dir;
	int exits;
        int vnum;
        int region;

	if ( !pRoomIndex )
		return FALSE;

	switch ( lookup_filter_list[ filter ].parameter )
	{
                case 3: // area
                        vnum = pRoomIndex->vnum;
                        return ((vnum >= pArea->min_vnum) && (vnum <= pArea->max_vnum));
                        break;
		case 4:     //sector
			if ( lookup_filter_list[ filter ].val1 == 0 )
				return ( pRoomIndex->sector_type == lookup_filter_list[ filter ].val2 );
			else
				return ( pRoomIndex->sector_type != lookup_filter_list[ filter ].val2 );
		case 5:     //flags
			if ( lookup_filter_list[ filter ].val1 == 0 )
				return ext_flags_same( lookup_filter_list[ filter ].ext_val, ext_flags_intersection( pRoomIndex->room_flags, lookup_filter_list[ filter ].ext_val ) );
			else
				return !ext_flags_same( lookup_filter_list[ filter ].ext_val, ext_flags_intersection( pRoomIndex->room_flags, lookup_filter_list[ filter ].ext_val ) );
		case 6:     //name
			return !str_infix( lookup_filter_list[ filter ].str_val, pRoomIndex->name );
		case 7:     //heal
			return ( num_eval( pRoomIndex->heal_rate, lookup_filter_list[ filter ].val1, lookup_filter_list[ filter ].val2 ) );
			break;
		case 8:     //noexits
			exits = 0;
			for (dir = 0; dir < 6; dir++)
			{
				if ( pRoomIndex->exit[dir] )
					exits++;
			}
			return ( exits == 0 );
		case 9:     //null
			return ( strlen( pRoomIndex->name ) == 0 || strlen( pRoomIndex->description ) == 0 );
                case 10: // region
                        region = pRoomIndex->area->region;
                        return (region && (region == lookup_filter_list[ filter ].val1));
                        break;


	}
	return TRUE;
}

void do_roomlookup( CHAR_DATA *ch, char *argument )
{
	const char * parameters[] =
		{
                  "start", "count", "area", "sector", "flags",
                  "name", "heal", "noexits", "null", "region",
                  "ubigraph", "graphviz", "total", NULL
		};
	char	buf [ MAX_STRING_LENGTH ];
	char	par [ MAX_INPUT_LENGTH ];
	char	val1 [ MAX_INPUT_LENGTH ];
	char	val2 [ MAX_INPUT_LENGTH ];
	ROOM_INDEX_DATA *pRoomIndex;
	AREA_DATA	*pArea = NULL;
        int     onlyTotal = FALSE;
	BUFFER	*buffer = NULL;
	long * ext_ival;
	int	vnum;
	int	parameter;
	int	par_count = 0;
	int	ifilter;
	int	i;
	int	i2;
	int	ival1;
	int	ival2;
	bool	passed;
	int	start = 0;
	int	count = 0;

	argument = one_argument( argument, par );
	if ( par[ 0 ] == '\0' )
	{
		buffer = new_buf();
		add_buf( buffer, "Skladnia:\n\r" );
		add_buf( buffer, "{Croomlookup #parametr1 warto¶æ warto¶æ #paramter2 warto¶æ warto¶æ...{x\n\r" );
		add_buf( buffer, "Parametry:\n\r" );
		add_buf( buffer, "{C#start <liczba>             {x- od jakiej pozycji listy zacz±æ wy¶wietlanie\n\r" );
		add_buf( buffer, "{C#count <liczba>             {x- ile lokacji wy¶wietliæ\n\r" );
		add_buf( buffer, "{C#area                       {x- przeszukiwanie tylko aktualnej krainy\n\r" );
		add_buf( buffer, "{C#region                     {x- przeszukiwanie tylko aktualnego regionu\n\r" );
		add_buf( buffer, "{C#sector [-]<sector>         {x- wy¶wietla lokacje z danym typem lub te, ktore\n\r" );
		add_buf( buffer, "                              nie maj± takiego sectora ( '-' przed sectorem )\n\r" );
		add_buf( buffer, "{C#flags [-]<flaga lokacji>   {x- szukanie lokacji z okre¶lonymi flagami\n\r" );
		add_buf( buffer, "{C#name <string>              {x- szukanie lokacji o okre¶lonej nazwie\n\r" );
		add_buf( buffer, "{C#heal <operator> <liczba>   {x- szukanie lokacji z okre¶lonym wspó³czynnikiem leczenia\n\r" );
		add_buf( buffer, "{C#noexits                    {x- szukanie lokacji bez wyj¶æ\n\r" );
		add_buf( buffer, "{C#null                       {x- szukanie lokacji, które nie maj± name i/lub opisu\n\r" );
		add_buf( buffer, "{C#total                      {x- wypisz jedynie liczbê znalezionych lokacji\n\r" );
		add_buf( buffer, "Lokacja musi spe³niæ wszystkie warunki aby zostac wyswietlona.\n\r" );
		page_to_char( buf_string( buffer ), ch );
		free_buf( buffer );
		return;
	}

	if ( ch->lines == 0 )
	{
		send_to_char( "Proszê w³±czyæ buforowanie strony (komenda config scroll <liczba linii>).\n\r", ch );
		return;
	}

	if ( !lookup_filter_list )
	{
		if ( ( lookup_filter_list = malloc( MAX_LOOKUP_PARAMETER * sizeof * lookup_filter_list ) ) == NULL )
		{
			send_to_char( "Za ma³o pamieci do wykonania tej komendy.\n\r", ch );
			return;
		}
	}

	for ( i = 0; i < MAX_LOOKUP_PARAMETER; i++ )
	{
		lookup_filter_list[ i ].parameter = 0;
		lookup_filter_list[ i ].val1 = 0;
		lookup_filter_list[ i ].val2 = 0;
		lookup_filter_list[ i ].val3 = 0;
	}

	for ( ; ; )
	{
		if ( par[ 0 ] == '\0' ) break;
		if ( par[ 0 ] != '#' )
		{
			print_char( ch, "Nieprawid³owy parametr: %s\n\r", par );
			return;
		}

		parameter = 0;
		for ( i = 0; parameters[ i ]; i++ )
		{
			if ( !str_cmp( par + 1, parameters[ i ] ) )
			{
				parameter = i + 1;
				break;
			}
		}
		if ( parameter == 0 )
		{
			print_char( ch, "Nieprawid³owy parametr: %s\n\r", par );
			return;
		}

		switch ( parameter )
		{
			case 1:     //start
				argument = one_argument( argument, val1 );
				if ( !is_number( val1 ) )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}
				start = UMAX( 0, atoi( val1 ) );
				break;
			case 2:     //count
				argument = one_argument( argument, val1 );
				if ( !is_number( val1 ) )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}
				count = UMAX( 0, atoi( val1 ) );
				break;
			case 3:     //area
				lookup_filter_list[ par_count ].parameter = parameter;
                                pArea = ch->in_room->area;
				par_count++;
				break;
			case 4:     //sector
				argument = one_argument( argument, val2 );
				if ( val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				ival1 = 0;
				if ( val2[ 0 ] == '-' )
				{
					ival1 = 1;
					sprintf( val2, "%s", val2 + 1 );
				}

				ival2 = -1;
				for ( i = 0; i < SECT_MAX ; i++ )
					if ( NOPOL( val2[ 0 ] ) == NOPOL( sector_table[ i ].name[ 0 ] )
						 && !str_prefix( val2, sector_table[ i ].name ) )
					{
						ival2 = i;
						break;
					}

				if ( ival2 < 0 )
				{
					print_char( ch, "Nieprawid³owy sector: %s\n\r", val2 );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].val2 = ival2;

				par_count++;
				break;
			case 5:     //flags
				argument = one_argument( argument, val2 );
				if ( val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				ival1 = 0;
				if ( val2[ 0 ] == '-' )
				{
					ival1 = 1;
					sprintf( val2, "%s", val2 + 1 );
				}

				ext_ival = ext_flag_value( room_flags, val2 );
				if ( ext_flags_none( ext_ival ) )
				{
					print_char( ch, "Nieprawid³owa flaga room: %s\n\r", val2 );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].ext_val = ext_ival;

				par_count++;
				break;
			case 6:     //name
				argument = one_argument( argument, val1 );

				if ( val1[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].str_val = str_dup( val1 );

				par_count++;
				break;
			case 7:     //heal
				argument = one_argument( argument, val1 );
				argument = one_argument( argument, val2 );

				if ( val1[ 0 ] == '\0' || val2[ 0 ] == '\0' )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				if ( !is_number( val2 ) || ( ival1 = keyword_lookup( fn_evals, val1 ) ) < 0 )
				{
					print_char( ch, "Nieprawid³owa sk³adnia parametru: %s\n\r", par );
					return;
				}

				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ival1;
				lookup_filter_list[ par_count ].val2 = atoi( val2 );

				par_count++;
				break;
			case 8:     //noexits
				lookup_filter_list[ par_count ].parameter = parameter;
				par_count++;
				break;
			case 9:     //null
				lookup_filter_list[ par_count ].parameter = parameter;
				par_count++;
				break;
                        case 10:   // region
				lookup_filter_list[ par_count ].parameter = parameter;
				lookup_filter_list[ par_count ].val1 = ch->in_room->area->region;
				par_count++;
                                break;
                        case 13: // total
                                onlyTotal = TRUE;
                                break;
			default:
				print_char( ch, "Nieprawid³owy parametr: %s\n\r", par );
				return;
		}
		argument = one_argument( argument, par );
	}




	buffer = new_buf();

        i = 0;
        i2 = 0;
        for ( vnum = 1; vnum < top_vnum_obj; vnum++ )
          {
            if ( count != 0 && i2 >= count )
              break;

            if ( !( pRoomIndex = get_room_index( vnum ) ) )
              continue;

            passed = TRUE;

            for ( ifilter = 0; ifilter < par_count; ifilter++ )
              if ( check_roomfilter ( pRoomIndex, ifilter, pArea ) == 0 )
                passed = FALSE;

            if ( passed )
              {
                if ( start == 0 || i >= start )
                  {
                    if ( !onlyTotal )
                      {
                        sprintf( buf, "%3d. [%6d] %s{D, area:{x %s\n\r", i2, pRoomIndex->vnum, pRoomIndex->name, pRoomIndex->area->name );
                        add_buf( buffer, buf );
                      }
                    i2++;
                  }
                i++;
              }
          }

	if ( i2 == 0 )
		sprintf( buf, "Znaleziono lokacji: %d.\n\r", i2 );
	else
		sprintf( buf, "\n\rZnaleziono lokacji: %d.\n\r", i2 );
	add_buf( buffer, buf );
	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );


	return;
}

/**
 * Zwraca maksymalna wizgrupe ustawiona dla goscia.
 *
 */
int max_wizgroup( CHAR_DATA *ch )
{
    if ( !ch || IS_NPC( ch ) )
    {
        return 0;
    }

    if ( ch->pcdata->wiz_conf & W6 )
    {
        return W6;
    }
    else if ( ch->pcdata->wiz_conf & W5 )
    {
        return W5;
    }
    else if ( ch->pcdata->wiz_conf & W4 )
    {
        return W4;
    }
    else if ( ch->pcdata->wiz_conf & W3 )
    {
        return W3;
    }
    else if ( ch->pcdata->wiz_conf & W2 )
    {
        return W2;
    }
    else if ( ch->pcdata->wiz_conf & W1 )
    {
        return W1;
    }
    return 0;
}

/*
	* Podaje odmiane gracza/moba lub ja ustawia.
	* Gracz nie posiadajacy co najmniej GUARD moze tylko obejrzec
	* czyjas odmiane. Dotyczy to takze zwyklych graczy. Umozliwienie
	* im podejrzenia czyjejs odmiany moze sie przydac gdy nie sa
	* pewni jakiej odmiany uzywac podczas rozmowy lub nie sa pewni
	* plci kogos.
	* Z tego powodu wizgrupa dla tej komendy nie jest ustawiana w interp.c
	* a sprawdzana tutaj.
	*
	*/
void do_inflect( CHAR_DATA *ch, char *argument )
{
	char arg [ MAX_INPUT_LENGTH ];
	char arg2 [ MAX_INPUT_LENGTH ];
	CHAR_DATA* victim;

	argument = one_argument( argument, arg );

	if ( max_wizgroup( ch ) >= W4 )
	{
		if ( arg[ 0 ] == '\0' )
		{
			send_to_char( "Sk³adnia:\n\r"
						  "inflect <imiê> - pokazuje aktualn± odmianê gracza/moba\n\r"
						  "inflect <imiê> <przypadek> <tekst> - ustawia odmianê dla danego przypadku\n\r", ch );
			return;
		}

		if ( ( victim = get_char_world( ch, arg ) ) == NULL )
		{
			send_to_char( "Nie ma takiego gracza ani moba.\n\r", ch );
			return;
		}

		argument = one_argument( argument, arg );
		one_argument( argument, arg2 );

		if ( arg[ 0 ] == '\0' || arg2[ 0 ] == '\0' )
		{

			print_char ( ch,
				"Aktualna odmiana:\n\r"
				"{CMianownik{x:   %s\n\r"
				"{CDope³niacz{x:  %s\n\r"
				"{CCelownik{x:    %s\n\r"
				"{CBiernik{x:     %s\n\r"
				"{CNarzêdnik{x:   %s\n\r"
				"{CMiejscownik{x: %s\n\r\n\r",
				IS_NPC( victim ) ? capitalize( victim->short_descr ) : victim->name,
				victim->name2,
				victim->name3,
				victim->name4,
				victim->name5,
				victim->name6
			);
			send_to_char( "Zmiana: inflect <imiê> <przypadek> <tekst>\n\r", ch );
			return;
		}

		if ( ch != victim && max_wizgroup( victim ) >= max_wizgroup( ch ) )
		{
			send_to_char( "Spróbuj na kim¶ ni¿szym w hierarchii muda.\n\r", ch );
			return;
		}

		if ( !str_prefix( arg, "dopelniacz" ) )
		{
			free_string( victim->name2 );
			victim->name2 = str_dup( capitalize( arg2 ) );

			if ( !IS_NPC( victim ) && victim->level > 1 )
				save_char_obj( victim, FALSE, FALSE );

			send_to_char( "Dope³niacz ustawiony. Postaæ zapisana.\n\r", ch );
			return;
		}
		else if ( !str_prefix( arg, "celownik" ) )
		{
			free_string( victim->name3 );
			victim->name3 = str_dup( capitalize( arg2 ) );

			if ( !IS_NPC( victim ) && victim->level > 1 )
				save_char_obj( victim, FALSE, FALSE );

			send_to_char( "Celownik ustawiony. Postaæ zapisana.\n\r", ch );
			return;
		}
		else if ( !str_prefix( arg, "biernik" ) )
		{
			free_string( victim->name4 );
			victim->name4 = str_dup( capitalize( arg2 ) );

			if ( !IS_NPC( victim ) && victim->level > 1 )
				save_char_obj( victim, FALSE, FALSE );

			send_to_char( "Biernik ustawiony. Postaæ zapisana.\n\r", ch );
			return;
		}
		else if ( !str_prefix( arg, "narzednik" ) )
		{
			free_string( victim->name5 );
			victim->name5 = str_dup( capitalize( arg2 ) );

			if ( !IS_NPC( victim ) && victim->level > 1 )
				save_char_obj( victim, FALSE, FALSE );

			send_to_char( "Narzêdnik ustawiony. Postaæ zapisana.\n\r", ch );
			return;
		}
		else if ( !str_prefix( arg, "miejscownik" ) )
		{
			free_string( victim->name6 );
			victim->name6 = str_dup( capitalize( arg2 ) );

			if ( !IS_NPC( victim ) && victim->level > 1 )
				save_char_obj( victim, FALSE, FALSE );

			send_to_char( "Miejscownik ustawiony. Postaæ zapisana.\n\r", ch );
			return;
		}
		else
		{
			send_to_char( "W jêzyku polskim nie ma takiego przypadku...\n\r", ch );
			return;
		}
		return;
	}
	else
	{
		if ( arg[ 0 ] == '\0' )
		{
			send_to_char( "Sk³adnia: inflect <imiê>\n\r", ch );
			return;
		}

		victim = get_char_world( ch, arg );

        if ( victim == NULL || IS_NPC( victim ) )
        {

            send_to_char( "Nie ma tutaj nikogo takiego.\n\r", ch );
            return;
        }

        if (ch->in_room != victim->in_room)
        {
            send_to_char( "Nie ma tutaj nikogo takiego.\n\r", ch );
            return;
        }
        if ( !can_see( ch, victim ) )
        {
            send_to_char( "Nie ma tutaj nikogo takiego.\n\r", ch );
            return;
        }

		print_char( ch, "Aktualna odmiana:\n\r"
					"{CMianownik{x:   %s\n\r"
					"{CDope³niacz{x:  %s\n\r"
					"{CCelownik{x:    %s\n\r"
					"{CBiernik{x:     %s\n\r"
					"{CNarzêdnik{x:   %s\n\r"
					"{CMiejscownik{x: %s\n\r",
					IS_NPC( victim ) ? capitalize( victim->short_descr ) : victim->name,
					victim->name2, victim->name3, victim->name4, victim->name5, victim->name6 );
	}
	return;
}

/*
	* do_rename zmienia imie gracza bedacego w grze.
	* Poprzedni plik zapisywany jest pod nazwa: backup.Stare_imie.Nowe_imie.Imie_tego_co_zmienia
	* zeby mozna bylo w razie czego przywrocic i od razu zobaczyc kto zmienil imie.
	* Gracz jest zapisywany do nowego pliku jesli ma lev > 1.
	* Sprawdzane jest czy imie jest legalne, czy nie jest disallowed i czy czasami juz
	* takiego nie ma.
	*/

bool check_parse_name ( char* name );  /* comm.c */

void do_rename ( CHAR_DATA* ch, char* argument )
{
	char old_name[ MAX_INPUT_LENGTH ];
	char new_name[ MAX_INPUT_LENGTH ];
	char strsave [ MAX_INPUT_LENGTH ];
	char sbackup [ MAX_INPUT_LENGTH ];
	char buf	[ MAX_STRING_LENGTH ];

	CHAR_DATA* victim;
	FILE*	file;

	argument = one_argument( argument, old_name );
	one_argument( argument, new_name );

	if ( old_name[ 0 ] == '\0' || new_name[ 0 ] == '\0' )
	{
		send_to_char ( "Sk³adnia:\n\r"
					   "rename <stare_imiê> <nowe_imiê>\n\r" , ch );
		return;
	}

	if ( !str_cmp( old_name, new_name ) )
	{
		send_to_char ( "Nowe imiê musi siê ró¿niæ od starego.\n\r" , ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, old_name ) ) == NULL )
	{
		send_to_char ( "Nie ma takiej osoby w tej chwili.\n\r", ch );
		return;
	}

	if ( IS_NPC( victim ) )
	{
		send_to_char ( "Ta komenda nie dzia³a na moby.\n\r", ch );
		return;
	}

	if ( victim != ch && max_wizgroup( victim ) >= max_wizgroup( ch ) )
	{
		send_to_char( "Spróbuj na kim¶ ni¿szym w hierarchii muda.\n\r", ch );
		return;
	}

	if ( !victim->desc || ( victim->desc->connected != CON_PLAYING ) )
	{
		send_to_char ( "Ten gracz straci³ po³±czenie z mudem lub jest w tej chwili zajêty.\n\r", ch );
		return;
	}

	if ( !check_parse_name( new_name ) )
	{
		send_to_char( "Nowe imiê jest nielegalne.\n\r", ch );
		return;
	}

	if ( is_allowed( argument ) == NAME_DISALLOWED )
	{
		send_to_char( "Podane nowe imiê jest zabronione.\n\r", ch );
		return;
	}

	/* Sprawdzamy czy juz taki gracz jest w katalogu */
	sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( new_name ) );

	fclose ( fpReserve );
	file = fopen ( strsave, "r" );

	if ( file )
	{
		send_to_char ( "Gracz o takim imieniu ju¿ istnieje!\n\r", ch );
		fclose ( file );
		fpReserve = fopen( NULL_FILE, "r" );
		return;
	}

	fpReserve = fopen( NULL_FILE, "r" );

	/* Sprawdzamy czasem taki gracz wlasnie nie gra ale nie jest zapisany - 1 level */
	if ( get_char_world( ch, new_name ) )
	{
		send_to_char ( "Gracz o takim imieniu ju¿ istnieje (1 level)!\n\r", ch );
		return;
	}

	/* Save the filename of the old name */
	sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( victim->name ) );
	sprintf( sbackup, "%sbackup%s.%s.%s", PLAYER_DIR, victim->name, capitalize( new_name ), ch->name );

	/* zapisujemy aktualny stan zeby backup byl aktualny */
	if ( victim->level > 1 )
		save_char_obj ( victim, FALSE, FALSE );

	sprintf( buf, "%s zmienia imie %s na %s.", ch->name, victim->name, capitalize( new_name ) );
	wiznet( buf, ch, NULL, WIZ_SECURE, 0, 0 );

	change_player_name( victim, capitalize( new_name ) );

	/* zapisujemy noew imie */
	if ( victim->level > 1 )
	{
		/* backup */
		sprintf( buf, "mv %s %s", strsave, sbackup );
		system( buf );
	}

	/* i w razie czego kasowanko starego */
	unlink ( strsave );

	send_to_char( "Imiê zmienione.\n\r", ch );

	if ( victim != ch )
	{
		victim->position = POS_STANDING;
		act ( "{RTwoje imiê zosta³o zmienione na $N!{x", ch, NULL, victim, TO_VICT );
	}
	return;
}

void do_timeset ( CHAR_DATA* ch, char* argument )
{
	char command[ MAX_INPUT_LENGTH ];
	char value[ MAX_INPUT_LENGTH ];
	int y_val, m_val, d_val, h_val;
	long lhour, lday, lmonth;


	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Sk³adnia:\n\r"
					  "timeset [year <value>] [month <value>] [day <value>] [hour <value>]\n\r", ch );
		return;
	}

	y_val = time_info.year;
	m_val = time_info.month;
	d_val = time_info.day;
	h_val = time_info.hour;

	for ( ; ; )
	{
		argument = one_argument( argument, command );

		if ( command[ 0 ] == '\0' )
			break;

		if ( !str_cmp( command, "year" ) )
		{
			argument = one_argument( argument, value );

			if ( value[ 0 ] == '\0' )
			{
				print_char( ch, "Nie poda<&³e¶/³a¶/³e¶> warto¶ci przy parametrze %s.\n\r", command );
				return;
			}

			if ( !is_number( value ) )
			{
				print_char( ch, "Warto¶ci przy parametrze %s nie jest liczb±.\n\r", command );
				return;
			}

			y_val = atoi( value );
			if ( y_val < 0 || y_val > 100000 )
			{
				print_char( ch, "Rok musi zawieraæ siê w przedziale od 0 do %d.\n\r", MAX_YEAR );
				return;
			}
		}
		else if ( !str_cmp( command, "month" ) )
		{
			argument = one_argument( argument, value );

			if ( value[ 0 ] == '\0' )
			{
				print_char( ch, "Nie poda<&³e¶/³a¶/³e¶> warto¶ci przy parametrze %s.\n\r", command );
				return;
			}

			if ( !is_number( value ) )
			{
				print_char( ch, "Warto¶ci przy parametrze %s nie jest liczb±.\n\r", command );
				return;
			}

			m_val = atoi( value );
			if ( m_val < 1 || m_val > MONTHS_PER_YEAR )
			{
				print_char( ch, "Miesi±c musi zawieraæ siê w przedziale od 1 do %d.\n\r", MONTHS_PER_YEAR );
				return;
			}
			m_val--;
		}
		else if ( !str_cmp( command, "day" ) )
		{
			argument = one_argument( argument, value );

			if ( value[ 0 ] == '\0' )
			{
				print_char( ch, "Nie poda<&³e¶/³a¶/³e¶> warto¶ci przy parametrze %s.\n\r", command );
				return;
			}

			if ( !is_number( value ) )
			{
				print_char( ch, "Warto¶ci przy parametrze %s nie jest liczb±.\n\r", command );
				return;
			}

			d_val = atoi( value );
			if ( d_val < 1 || d_val > DAYS_PER_MONTH )
			{
				print_char( ch, "Dzieñ musi zawieraæ siê w przedziale od 1 do %d.\n\r", DAYS_PER_MONTH );
				return;
			}
			d_val--;
		}
		else if ( !str_cmp( command, "hour" ) )
		{
			argument = one_argument( argument, value );

			if ( value[ 0 ] == '\0' )
			{
				print_char( ch, "Nie poda<&³e¶/³a¶/³e¶> warto¶ci przy parametrze %s.\n\r", command );
				return;
			}

			if ( !is_number( value ) )
			{
				print_char( ch, "Warto¶ci przy parametrze %s nie jest liczb±.\n\r", command );
				return;
			}

			h_val = atoi( value );
			if ( h_val < 0 || h_val > HOURS_PER_DAY )
			{
				send_to_char( "Godzina musi zawieraæ siê w przedziale od 0 do 24.\n\r", ch );
				return;
			}
		}
		else
		{
			print_char( ch, "Nieprawid³owy parametr: %s.\n\r", command );
			send_to_char( "Sk³adnia:\n\r"
						  "timeset [year <value>] [month <value>] [day <value>] [hour <value>]\n\r", ch );
			return;
		}
	}

	misc.world_time = ( y_val * 17 * 35 * 24 ) + ( m_val * 35 * 24 ) + ( d_val * 24 ) + h_val;
	lhour	= misc.world_time;
	time_info.hour	= lhour % HOURS_PER_DAY;
	lday	= lhour / HOURS_PER_DAY;
	time_info.day	= lday % DAYS_PER_MONTH;
	lmonth	= lday / DAYS_PER_MONTH;
	time_info.month	= lmonth % MONTHS_PER_YEAR;
	time_info.year	= lmonth / MONTHS_PER_YEAR;
	send_to_char( "Czas ¶wiata zmieniony.\n\r", ch );
	save_misc_data();
	return;
}

void show_wiznet_config( CHAR_DATA *ch )
{
	char buffer[ MAX_INPUT_LENGTH ];
	BUFFER *buf = NULL;

	if ( IS_NPC( ch ) )
		return;

	buf = new_buf();

	if ( !( ( ch->pcdata->wiz_conf & W6) || ( ch->pcdata->wiz_conf & W5) || ( ch->pcdata->wiz_conf & W4) || ( ch->pcdata->wiz_conf & W3) ) )
		SET_BIT( ch->wiz_config, WIZCFG_LIMITLEVEL );

	add_buf( buf, "\n\r{WAktualna konfiguracja wizowa:{x\n\n\r" );

	sprintf( buffer, "sexconv         [%s]\n\r",
			 IS_SET( ch->wiz_config, WIZCFG_SEXCONV ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "colourconv      [%s]\n\r",
			 IS_SET( ch->wiz_config, WIZCFG_COLOURCONV ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "limitlevel      [%s]\n\r",
			 IS_SET( ch->wiz_config, WIZCFG_LIMITLEVEL ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "wizspeech       [%s]\n\r",
			 IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "showvnums       [%s]\n\r",
			 IS_SET( ch->wiz_config, WIZCFG_SHOWVNUMS ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "quietmode       [%s]\n\r",
			 IS_SET( ch->wiz_config, WIZCFG_QUIETMODE ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "showyells       [%s]\n\r",
			 IS_SET( ch->wiz_config, WIZCFG_SHOWYELLS ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

    add_buf( buf, "\n\r" );

	page_to_char( buf_string( buf ), ch );
	free_buf( buf );
	return;
}

void do_wizconfig ( CHAR_DATA* ch, char* argument )
{
	char arg[ MAX_INPUT_LENGTH ];

	if ( IS_NPC( ch ) )
		return;

	if ( argument[ 0 ] == '\0' )
	{
		show_wiznet_config( ch );
		return;
	}

	argument = one_argument( argument, arg );


	if ( !str_prefix( arg, "sexconv" ) )
	{
		if ( IS_SET( ch->wiz_config, WIZCFG_SEXCONV ) )
		{
			send_to_char( "Sex konwerter zosta³ wy³±czony.\n\r", ch );
			REMOVE_BIT( ch->wiz_config, WIZCFG_SEXCONV );
		}
		else
		{
			send_to_char( "Sex konwerter zosta³ w³±czony.\n\r", ch );
			SET_BIT( ch->wiz_config, WIZCFG_SEXCONV );
		}
		return;
	}

	else if ( !str_prefix( arg, "colourconv" ) )
	{
		if ( IS_SET( ch->wiz_config, WIZCFG_COLOURCONV ) )
		{
			send_to_char( "Konwerter kolorów zosta³ wy³±czony.\n\r", ch );
			REMOVE_BIT( ch->wiz_config, WIZCFG_COLOURCONV );
		}
		else
		{
			send_to_char( "Konwerter kolorów zosta³ w³±czony.\n\r", ch );
			SET_BIT( ch->wiz_config, WIZCFG_COLOURCONV );
		}
		return;
	}

	else if ( !str_prefix( arg, "limitlevel" ) )
	{
		if ( ( ch->pcdata->wiz_conf & W6) || ( ch->pcdata->wiz_conf & W5) || ( ch->pcdata->wiz_conf & W4) || ( ch->pcdata->wiz_conf & W3) )
		{
			if ( IS_SET( ch->wiz_config, WIZCFG_LIMITLEVEL ) )
			{
				send_to_char( "Medit nie bêdzie limitowa³ ustawianego poziomu moba.\n\r", ch );
				REMOVE_BIT( ch->wiz_config, WIZCFG_LIMITLEVEL );
			}
			else
			{
				send_to_char( "Medit bêdzie limitowa³ ustawianie poziomu moba.\n\r", ch );
				SET_BIT( ch->wiz_config, WIZCFG_LIMITLEVEL );
			}
		}
		else
		{
			send_to_char( "Tylko Lordowie, Sêdziowie, Regenci i Stra¿nicy mog± omijaæ limity poziomów w medit.\n\r", ch );
			SET_BIT( ch->wiz_config, WIZCFG_LIMITLEVEL );
		}
		return;
	}

	else if ( !str_prefix( arg, "wizspeech" ) )
	{
		if ( IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
		{
			send_to_char( "Od tej pory gracze bêd± widzieli Twoje imiê (lub 'Kto¶') przy mówieniu do nich.\n\r", ch );
			REMOVE_BIT( ch->wiz_config, WIZCFG_WIZSPEECH );
		}
		else
		{
			send_to_char( "Od tej pory bêdziesz widoczny jako 'Nie¶miertelny' przy mówieniu do graczy.\n\r", ch );
			SET_BIT( ch->wiz_config, WIZCFG_WIZSPEECH );
		}
		return;
	}

	else if ( !str_prefix( arg, "showvnums" ) )
	{
		if ( IS_SET( ch->wiz_config, WIZCFG_SHOWVNUMS ) )
		{
			send_to_char( "Nie bêdziesz widzieæ vnumów obiektów i mobów przy look.\n\r", ch );
			REMOVE_BIT( ch->wiz_config, WIZCFG_SHOWVNUMS );
		}
		else
		{
			send_to_char( "Bêdziesz widzieæ vnumy obiektów i mobów przy look.\n\r", ch );
			SET_BIT( ch->wiz_config, WIZCFG_SHOWVNUMS );
		}
		return;
	}
	else if ( !str_prefix( arg, "quietmode" ) )
	{
		if ( IS_SET( ch->wiz_config, WIZCFG_QUIETMODE ) )
		{
			send_to_char( "Komendy purge i load bêd± wy¶wietlaæ teksty do lokacji.\n\r", ch );
			REMOVE_BIT( ch->wiz_config, WIZCFG_QUIETMODE );
		}
		else
		{
			send_to_char( "Komendy purge i load nie bêd± wy¶wietlaæ tekstów do lokacji.\n\r", ch );
			SET_BIT( ch->wiz_config, WIZCFG_QUIETMODE );
		}
		return;
	}
	else if ( !str_prefix( arg, "showyells" ) )
	{
		if ( IS_SET( ch->wiz_config, WIZCFG_SHOWYELLS ) )
		{
			send_to_char( "Nie bêdziez widzia³ wrzasków graczy.\n\r", ch );
			REMOVE_BIT( ch->wiz_config, WIZCFG_SHOWYELLS );
		}
		else
		{
			send_to_char( "Bêdziesz widzia³ wrzaski graczy.\n\r", ch );
			SET_BIT( ch->wiz_config, WIZCFG_SHOWYELLS );
		}
		return;
	}
	else
	{
		send_to_char( "Nie ma takiej opcji.\n\r", ch );
		return;
	}

	return;
}

void do_hosts( CHAR_DATA* ch, char* argument )
{
	CHAR_DATA * victim;
	char arg[ MAX_INPUT_LENGTH ];
	char buf[ MAX_STRING_LENGTH ];
	char buf1[ MAX_STRING_LENGTH ];
	int iHost;
	int count = 0;

	one_argument( argument, arg );

	if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
	{
		send_to_char ( "Nie ma takiej osoby w tej chwili.\n\r", ch );
		return;
	}

	if ( IS_NPC( victim ) || !victim->pcdata )
	{
		send_to_char ( "Ta komenda nie dzia³a na moby.\n\r", ch );
		return;
	}

	if ( victim->pcdata->hosts_count == 0 )
	{
		send_to_char ( "Lista adresów dla tej osoby jest pusta.\n\r", ch );
		return;
	}

	sprintf( buf, "Lista adresów, z których ³±czy³a siê osoba: {C%s{x.\n\r", victim->name );
	for ( iHost = 0; iHost < victim->pcdata->hosts_count; iHost++ )
	{
		if ( IS_NULLSTR( victim->pcdata->hosts[ iHost ] ) )
			continue;
		count++;
		sprintf( buf1, "{C%2d{x. %s\n\r", count, victim->pcdata->hosts[ iHost ] );
		strcat( buf, buf1 );
	}

	if ( count == 0 )
		send_to_char ( "Lista adresów dla tej osoby jest pusta.\n\r", ch );
	else
		send_to_char( buf, ch );

	return;
}

void do_flag( CHAR_DATA *ch, char *argument )
{
	char arg1[ MAX_INPUT_LENGTH ], arg2[ MAX_INPUT_LENGTH ], arg3[ MAX_INPUT_LENGTH ];
	char word[ MAX_INPUT_LENGTH ];
	CHAR_DATA * victim;
	OBJ_DATA * obj;
	long *flag;
	long pos;
	BITVECT_DATA * ext_pos;
	long *ext_flag = NULL;
	char is_ext_flag = FALSE;
	bool settable;
	const struct flag_type *flag_table;
	const struct ext_flag_type *ext_flag_table;
	bool set = FALSE;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( arg1[ 0 ] == '\0' )
	{
		send_to_char( "Syntax:\n\r"
					  "  flag mob  <name> <field> <flags>\n\r"
					  "  flag char <name> <field> <flags>\n\r"
					  "  flag obj  <name> <field> <flags>\n\r"
					  "  mob  flags: act,off,form,part\n\r"
					  "  char flags: plr,comm\n\r"
					  "  obj flags: wear,exwear,extra\n\r", ch );
		return;
	}

	if ( arg2[ 0 ] == '\0' )
	{
		send_to_char( "What do you wish to set flags on?\n\r", ch );
		return;
	}

	if ( arg3[ 0 ] == '\0' )
	{
		send_to_char( "You need to specify a flag to set.\n\r", ch );
		return;
	}

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Which flags do you wish to change?\n\r", ch );
		return;
	}

	if ( !str_prefix( arg1, "mob" ) || !str_prefix( arg1, "char" ) )
	{
		victim = get_char_world( ch, arg2 );
		if ( victim == NULL )
		{
			send_to_char( "Nie ma takiej postaci.\n\r", ch );
			return;
		}

		/* select a flag to set */
		if ( !str_prefix( arg3, "act" ) )
		{
			if ( !IS_NPC( victim ) )
			{
				send_to_char( "Mo¿na u¿ywaæ tylko na moby.\n\r", ch );
				return;
			}

			ext_flag = victim->act;
			ext_flag_table = act_flags;
			is_ext_flag = TRUE;
		}
		else if ( !str_prefix( arg3, "form" ) )
		{
			if ( !IS_NPC( victim ) )
			{
				send_to_char( "Form mo¿e byæ ustawiane tylko na NPCach.\n\r", ch );
				return;
			}

			flag = &victim->form;
			flag_table = form_flags;
		}
		else if ( !str_prefix( arg3, "parts" ) )
		{
			if ( !IS_NPC( victim ) )
			{
				send_to_char( "Parts mo¿e byæ ustawiane tylko na NPCach.\n\r", ch );
				return;
			}

			flag = &victim->parts;
			flag_table = part_flags;
		}
		else if ( !str_prefix( arg3, "comm" ) )
		{
			if ( IS_NPC( victim ) )
			{
				send_to_char( "Comm mo¿e byæ ustawiane tylko na NPCach.\n\r", ch );
				return;
			}

			flag = &victim->comm;
			flag_table = comm_flags;
		}
		else
		{
			send_to_char( "Nieprawid³owa warto¶æ.\n\r", ch );
			return;
		}

		victim->zone = NULL;
	}

	if ( !str_prefix( arg1, "obj" ) )
	{
		obj = get_obj_world( ch, arg2 );
		if ( !obj )
		{
			send_to_char( "Nie ma takiego obiektu.\n\r", ch );
			return;
		}

		/* select a flag to set */
		if ( !str_cmp( arg3, "extra" ) )
		{
			ext_flag = obj->extra_flags;
			ext_flag_table = extra_flags;
			is_ext_flag = TRUE;
		}
		else if ( !str_cmp( arg3, "wear" ) )
		{
			flag = &obj->wear_flags;
			flag_table = wear_flags;
		}
		else if ( !str_cmp( arg3, "exwear" ) )
		{
			ext_flag = obj->wear_flags2;
			ext_flag_table = wear_flags2;
			is_ext_flag = TRUE;
		}
		else
		{
			send_to_char( "Nieprawid³owa warto¶æ.\n\r", ch );
			return;
		}
	}

	if ( is_ext_flag )
	{
		for ( ; ; )
		{
			argument = one_argument( argument, word );

			if ( word[ 0 ] == '\0' )
				break;

			ext_pos = ext_flag_lookup( word, ext_flag_table );
			settable = ext_is_settable( word, ext_flag_table );

			if ( ext_pos == &EXT_NONE || !settable )
			{
				continue;
			}
			else
			{
				EXT_TOGGLE_BIT( ext_flag, *ext_pos );
				set = TRUE;
			}
		}
	}
	else
	{
		for ( ; ; )
		{
			argument = one_argument( argument, word );

			if ( word[ 0 ] == '\0' )
				break;

			pos = flag_lookup( word, flag_table );
			settable = is_settable( word, flag_table );

			if ( pos == NO_FLAG || !settable )
			{
				continue;
			}
			else
			{
				TOGGLE_BIT( *flag, pos );
				set = TRUE;
			}
		}
	}
	if ( set )
		print_char( ch, "Warto¶æ ustawiona.\n\rNowe flagi to: %s\n\r", ext_flag_string( extra_flags, ext_flag ) );
	else
		send_to_char( "Podano nieprawid³ow± flagê dla tej warto¶ci.\n\r", ch );
}

void do_match( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * ach, *bch;
	char buf[ MAX_STRING_LENGTH ];
	char outbuf[ MAX_STRING_LENGTH ];
	char same[ MAX_STRING_LENGTH ];
	int count = 0;

	buf[ 0 ] = outbuf[ 0 ] = '\0';

	for ( ach = char_list; ach; ach = ach->next )
	{
		if ( IS_NPC( ach ) || !ach->pcdata )
			continue;

		for ( bch = ach->next; bch; bch = bch->next )
		{
			if ( IS_NPC( bch ) || !bch->pcdata )
				continue;

			same[0] = '\0';

			if ( !str_cmp( ach->pcdata->pwd, bch->pcdata->pwd ) )
				strcat( same, " {Rhas³o{x" );

			if ( !str_cmp( ach->prompt, bch->prompt ) &&
				 str_cmp( ach->prompt, STANDARD_PROMPT ) )
			{
				if ( same[0] != '\0' )
					strcat( same, "," );
				strcat( same, " prompt" );
			}

			if ( same[0] != '\0' && ach->desc && bch->desc &&
				!str_cmp( ach->desc->host, bch->desc->host ) )
			{
				strcat( same, ", host" );
			}

			if ( same[0] != '\0' && is_same_group( ach, bch ) )
			{
				strcat( same, ", grupa" );
			}

			if ( same[0] != '\0' )
			{
				sprintf( buf, "%-15s%s = %-15s%s :%s\n\r",
						 ach->name,
						 ach->desc ? "       " : "(disco)",
						 bch->name,
						 bch->desc ? "       " : "(disco)",
						 same );
				strcat( outbuf, buf );
				count++;
			}
		}
	}

	if ( count == 0 )
	{
		send_to_char( "Dopasowanie postaci nie wskazuje nic niezwyk³ego.\n\r", ch );
		return;
	}

	send_to_char( "Lista graczy wskazuj±cych jakie¶ podobieñstwo (mo¿e byæ multi):\n\r", ch );
	send_to_char( outbuf, ch );
	return;
}

void do_grouplist( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * ach, *bch;
	char buf[ MAX_STRING_LENGTH ];
	int count = 0;

	for ( ach = char_list; ach; ach = ach->next )
	{
		if ( IS_NPC( ach ) || !ach->pcdata )
			continue;

		buf[0] = '\0';
		for ( bch = char_list; bch; bch = bch->next )
		{
			if ( IS_NPC( bch ) || !bch->pcdata || bch->leader != ach )
				continue;

			if ( buf[0] == '\0' )
			{
				sprintf( buf, "{G%s{x", ach->name );
				if ( count == 0 )
					send_to_char( "Lista grup w grze:\n\r", ch );
				count++;
			}

			strcat( buf, ", " );
			strcat( buf, bch->name );
		}
		if ( buf[0] != '\0' )
		{
			strcat( buf, "\n\r\n\r" );
			send_to_char( buf, ch );
		}
	}

	if ( count == 0 )
	{
		send_to_char( "Nie znaleziono ¿adnych grup.\n\r", ch );
		return;
	}

	return;
}

void do_destro ( CHAR_DATA* ch, char* argument )
{
	send_to_char( "U¿yj pe³nej nazwy komendy 'destroy'.\n\r", ch );
	return;
}

/*
 * Dla immo - kasacja postaci gracza
 */
void do_destroy ( CHAR_DATA* ch, char* argument )
{
	CLAN_DATA * clan;
	char name[ MAX_INPUT_LENGTH ];
	char strsave [ MAX_INPUT_LENGTH ];
	char strmove [ MAX_INPUT_LENGTH ];
	char buf	[ MAX_STRING_LENGTH ];

	CHAR_DATA* victim;

	one_argument( argument, name );

	if ( name[ 0 ] == '\0' )
	{
		send_to_char ( "Sk³adnia:\n\r"
					   "destroy <imiê>\n\r" , ch );
		return;
	}

	if ( ( victim = get_char_world ( ch, name ) ) == NULL )
		victim = load_char_remote( name );

	if ( !victim )
	{
		send_to_char( "Nie ma takiej postaci, ani w grze ani na dysku.\n\r", ch );
		return;
	}

	if ( IS_NPC( victim ) )
	{
		send_to_char ( "Ta komenda nie dzia³a na moby.\n\r", ch );
		return;
	}

	if ( victim != ch && max_wizgroup( victim ) >= max_wizgroup( ch ) )
	{
		send_to_char( "Spróbuj na kim¶ ni¿szym w hierarchii muda.\n\r", ch );
		return;
	}

	sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( victim->name ) );
	sprintf( strmove, "%sdeleted.%s.by.%s", PLAYER_DIR, victim->name, ch->name );

	sprintf( buf, "%s kasuje postaæ %s.", ch->name, victim->name );
	wiznet( buf, ch, NULL, WIZ_SECURE, 0, 0 );

	if ( victim->level > 1 )
	{
		sprintf( buf, "mv %s %s", strsave, strmove );
		system( buf );
	}

	unlink ( strsave );

	if ( ( clan = get_clan_by_member( victim->name ) ) != NULL )
	{
		remove_from_clan( clan, get_member( clan, victim->name ) );
		save_clans();
	}

	if ( victim->desc )
	{
		switch ( number_range( 1, 3 ) )
		{
			default:
				send_to_desc( victim->desc, "{RCzujesz jak gniew bogów wysysa z ciebie ca³e ¿ycie. Twoje istnienie zosta³o wymazane z kart historii tego ¶wiata.{x\n\r", TRUE );
				act( "$n przez chwilê wygl±da jakby cierpia³ ogromne katusze. Po chwili jego skóra sinieje, zapada siê i zostaje z niego zeschniête truch³o, które zaraz znika. A mo¿e go tam w ogóle nigdy nie by³o?\n\r", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				send_to_desc( victim->desc, "{RPrzez chwilê czujesz siê jako¶ dziwnie. ¦wiat doko³a powoli blaknie i tracisz ¶wiadomo¶æ. Nie ma ciê i nigdy ciê nie by³o.{x\n\r", TRUE );
				act( "Ze zdziwieniem zauwa¿asz, ¿e cia³o $z staje siê pó³przezroczyste i znika po chwili jakby go tam nigdy nie by³o.\n\r", victim, NULL, NULL, TO_ROOM );
				break;
			case 2:
				send_to_desc( victim->desc, "{RCzujesz dreszcze przebiegaj±ce po twoim ciele. Tu¿ nad twoj± g³ow± tworzy siê czarna, wiruj±ca chmura wy³adowañ i dymu. Ostatnie co pamiêtasz to uczucie pustki kiedy chmura opada na twoje cia³o. Bogowie w swoim gniewie wymazali twoje istnienie z tego ¶wiata.{x\n\r", TRUE );
				act( "Nad g³ow± $z pojawia siê nagle dziwna, czarna i wiruj±ca chmura wy³adowañ i dymu. S³yszysz jakie¶ niesamowite rozgniewane g³osy w czasie gdy chmura otacza jego cia³o. W serii trzasków i wy³adowañ z $z zostaje tylko kupka popio³u. Nikn±ca chmura rozwiewa nawet t± pozosta³o¶æ po $b.\n\r", victim, NULL, NULL, TO_ROOM );
				break;
		}

		extract_char( victim, TRUE );
	}
	else
		free_char( victim );

	send_to_char( "Postaæ skasowana.\n\r", ch );

	return;
}

void do_test ( CHAR_DATA *ch, char *argument )
{
	//generate_random_feat( ch );
	//return;

	CHAR_DATA *vch;
	char arg1[ MAX_INPUT_LENGTH ];
	char buf[ MAX_STRING_LENGTH ];
	int level;
	int iLevel;
	int zm;

	if (
		str_cmp( ch->name, "Furgas" )
	    && str_cmp( ch->name, "Agron" )
		//&& str_cmp( ch->name, "Builder" )
		)
		return;

	argument = one_argument( argument, arg1 );

	if ( ( vch = get_char_world( ch, arg1 ) ) == NULL )
	{
		send_to_char( "Nie ma nikogo takiego.\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg1 );

	if ( !str_cmp( arg1, "exp" ) )
	{
		if ( !is_number( argument ) )
			return;

		gain_exp( vch, UMAX( 0, atoi( argument ) ), FALSE );
	}
	else if ( !str_cmp( arg1, "level" ) )
	{
		if ( argument[ 0 ] == '\0' || !is_number( argument ) )
		{
			send_to_char( "Sk³adnia: test level <level>.\n\r", ch );
			return;
		}

		if ( ( level = atoi( argument ) ) < 1 || level > LEVEL_HERO )
		{
			send_to_char( "Level mo¿e byæ pomiêdzy 1 a 31 w³±cznie.\n\r", ch );
			return;
		}

		if ( level <= vch->level )
		{
			send_to_char( "Obnizanie poziomu!\n\r", ch );
			for ( zm = vch->level - level;zm > 0;zm-- )
			{
				delevel( vch, FALSE );
				vch->exp = exp_per_level( vch, vch->level - 1 );
			}
			return;
		}
		else
		{
			send_to_char( "Podwy¿szanie poziomu!\n\r", ch );
		}

		for ( iLevel = vch->level ; iLevel < level; iLevel++ )
		{
			advance_level( vch, TRUE );
		}

		print_char( ch, "Masz teraz poziom %d.\n\r", vch->level );
		vch->exp = exp_per_level( vch, vch->level - 1 );

		vch->trust = 0;
		if ( vch->level < 2 )
		{
#ifndef S_SPLINT_S
			sprintf( buf, "%s%s", PLAYER_DIR, capitalize( vch->name ) );
#endif
                        unlink( buf );
		}
		save_char_obj( vch, FALSE, FALSE );
	}
	else if ( !str_cmp( arg1, "skill" ) )
	{
		argument = one_argument( argument, arg1 );

		zm = skill_lookup( arg1 );

		if ( zm < 0 )
		{
			send_to_char( "Nie ma takiego skilla/spella.\n\r", ch );
			return;
		}

		if ( !is_number( argument ) )
		{
			send_to_char( "Drugi parametr ma byæ liczb± w zakresie 0-100.\n\r", ch );
			return;
		}


		level = atoi( argument );
		if ( level < 0 || level > 100 )
		{
			send_to_char( "Drugi parametr ma byæ liczb± w zakresie 0-100.\n\r", ch );
			return;
		}

		if ( skill_table[ zm ].spell_fun == spell_null )
		{
			vch->pcdata->learned[ zm ] = atoi( argument );

			vch->pcdata->learned[ zm ] = URANGE( 0, vch->pcdata->learned[ zm ], 100 );

			print_char( ch, "Zmieniono poziom wyuczenia skilla '%s' na %d.\n\r", skill_table[ zm ].name, vch->pcdata->learned[ zm ] );
			return;
		}
		else
		{
			if ( level > 0 )
			{
				vch->pcdata->learned[ zm ] = 1;
				print_char( ch, "Umiesz teraz czar '%s'.\n\r", skill_table[ zm ].name );
			}
			else
			{
				vch->pcdata->learned[ zm ] = 0;
				print_char( ch, "Nie umiesz ju¿ czaru '%s'.\n\r", skill_table[ zm ].name );
			}
		}
	}
	else if ( !str_cmp( arg1, "plus" ) )
	{
		argument = one_argument( argument, arg1 );

		zm = skill_lookup( arg1 );

		if ( zm < 0 )
		{
			send_to_char( "Nie ma takiego skilla/spella.\n\r", ch );
			return;
		}

		if ( !is_number( argument ) )
		{
			send_to_char( "Drugi parametr ma byæ liczb± w zakresie 0-100.\n\r", ch );
			return;
		}


		level = atoi( argument );
		if ( level < 0 || level > 100 )
		{
			send_to_char( "Drugi parametr ma byæ liczb± w zakresie 0-100.\n\r", ch );
			return;
		}

		if ( skill_table[ zm ].spell_fun == spell_null )
		{
			vch->pcdata->learning[ zm ] = atoi( argument );

			vch->pcdata->learning[ zm ] = URANGE( 0, vch->pcdata->learning[ zm ], 100 - vch->pcdata->learned[ zm ] );
            
            vch->pcdata->learning_rasz[ zm ] = atoi( argument );

			vch->pcdata->learning_rasz[ zm ] = URANGE( 0, vch->pcdata->learning_rasz[ zm ], 100 - vch->pcdata->learned[ zm ] );
			print_char( ch, "Zmieniono liczbê plusów przy skillu '%s' na %d.\n\r", skill_table[ zm ].name, vch->pcdata->learned[ zm ] );
			return;
		}
		else
		{
			send_to_char( "To nie jest skill.\n\r", ch );
		}
	}
	else if ( !str_cmp( arg1, "class" ) )
	{
		int class;

		class = class_lookup( argument );

		if ( class == -1 || class == CLASS_MONK || class == CLASS_BARD )
		{
			char buf[ MAX_STRING_LENGTH ];

			strcpy( buf, "Mo¿liwe profesje: " );
			for ( class = 0; class < MAX_CLASS; class++ )
			{
				if ( class == CLASS_MONK || class == CLASS_BARD )
					continue;

				if ( class > 0 )
					strcat( buf, " " );

				strcat( buf, class_table[ class ].name );
			}
			strcat( buf, ".\n\r" );

			send_to_char( buf, ch );
			return;
		}

		vch->class = class;
		send_to_char( "Zmieniono profesjê.\n\r", ch );
	}
	else if ( !str_cmp( arg1, "spec" ) )
	{
		int spec = -1, school;

		if ( vch->class != CLASS_MAG )
		{
			send_to_char( "Tylko dla graczy magow.\n\r", ch );
			return;
		}

		if ( !str_cmp( argument, "none" ) )
		{
			send_to_char( "Usuniêto specjalizacjê.\n\r", ch );
			vch->pcdata->mage_specialist = -1;
			return;
		}

		for ( school = 0; school < MAX_SCHOOL; school++ )
		{
			if ( !str_prefix( argument, school_table[ school ].name ) )
			{
				spec = school;
				break;
			}
		}

		if ( school == -1 )
		{
			send_to_char( "Nie ma takiej specjalizacji.\n\r", ch );
			return;
		}
		vch->pcdata->mage_specialist = spec;
		send_to_char( "Zmieniono specjalizacjê.\n\r", ch );
	}
	else if ( !str_cmp( arg1, "copper" ) )
	{
		if ( argument[0] == '\0' || !is_number( argument ) )
		{
			send_to_char( "Nie poda³e¶ liczby miedzi.\n\r", ch );
			return;
		}

		vch->copper = UMAX( 0, atoi( argument ) );
		print_char( ch, "Masz teraz %d miedzi.\n\r", vch->copper );
	}
	else if ( !str_cmp( arg1, "herb" ) )
	{
		HERB_IN_ROOM_DATA *temp;

        	print_char( ch, "Ziola:\n\r");
       		if( !(temp = vch->in_room->first_herb)) return;
        	//print_char( ch, "%d %s\n\r",temp->herb, herb_table[temp->herb].lname);
        	/*if (temp->next)
       		{
           		temp = temp->next;
           		print_char( ch, "%d %s\n\r",temp->herb, herb_table[temp->herb].lname);

        	}*/
	    	for(temp = vch->in_room->first_herb;temp;temp = temp->next)
	    	{
			//temp->herb_status = 1;
		    	print_char( ch, "%d %s szansa wystapienia:%d, status:%d, trudnosc:%d\n\r",temp->herb, herb_table[temp->herb].lname, herb_table[temp->herb].sectors[vch->in_room->sector_type],temp->herb_status,herb_table[temp->herb].difficult);

		}
        	print_char( ch, "reset_time: %d",vch->in_room->herb_update_timer);
	}
	else
	{
		send_to_char( "Sk³adnia:\n\r"
					  "test exp <liczba> - dodawanie/odejmowanie expa\n\r"
					  "test level <liczba> - ustawianie poziomu na podany\n\r"
					  "test skill <skill/spell> <liczba> - ustawianie znajomo¶ci skilla/spella na podan± liczbê (0-100)\n\r"
					  "test plus <skill/spell> <liczba> - ustawianie liczby plusów dla skilla/spella na podan± liczbê\n\r"
					  "test class <profesja> - ustawianie profesji na podan±\n\r"
					  "test spec <specjalizacja> - ustawianie specjalizacji na podan± (none - usuwa specjalizacjê)\n\r"
					  "test copper <kasa> - ustawianie posiadanej miedzi\n\r"
					  "test stat - pokazuje aktualne staty postaci\n\r"
					  "test stat roll - pokazuje przyk³adowe losowanie statów ({RNie przypisuje ich postaci{x)\n\r"
					  "test stat reset - po zej¶ciu na 1 level mo¿na zresetowaæ staty na poziom po za³o¿eniu postaci\n\r", ch );
	}

	return;
}

void do_timeshift ( CHAR_DATA* vch, char* argument )
{
	send_to_char("Przesuwam godzinê...\n\r",vch);
	weather_update( TRUE );
	return;
}

bool sifollow_remove_char( CHAR_DATA* ch, CHAR_DATA* victim )
{
	CHAR_LST *chlst;
	CHAR_LST *chlst_next;
	bool found = FALSE;
	for( chlst = ch->si_followed; chlst !=NULL; chlst = chlst_next )
	{
		chlst_next = chlst->next;

		if ( chlst->ch == victim )
		{
			found = TRUE;
			if ( chlst->previous != NULL )
				chlst->previous->next = chlst->next;
			if ( chlst == ch->si_followed )
				ch->si_followed = NULL;
			free( chlst );
			ch->si_followed_cnt--;
			break;
		}
	}
	return found;
}

void sifollow_add_char( CHAR_DATA* ch, CHAR_DATA* victim )
{
	CHAR_LST *new_el = malloc( sizeof(CHAR_LST) );
	if ( !new_el )
	{
		bug("sifollow_add_char: malloc", 1);
		return;
	}
	new_el->next = ch->si_followed;
	new_el->ch = victim;
	new_el->previous = NULL;
	if ( ch->si_followed != NULL )
		ch->si_followed->previous = new_el;
	ch->si_followed = new_el;
	ch->si_followed_cnt++;
}

void do_sifollow( CHAR_DATA *ch, char* argument )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	bool found = FALSE;

	DEBUG_INFO("do_sifollow");

	one_argument( argument, arg );
	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie ma nikogo takiego na ¶wiecie.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		// "sifol self" powoduje usuniecie wszystkiego z listy jezeli osoba
		// juz byla na swojej liscie, lub dodanie jej do swojej listy
		// jezeli jej jeszcze nie bylo. zachowanie to jest nieregularne
		// wiec nie bedziemy na nie pozwalaæ
		send_to_char( "¦ledzenie samego siebie nie jest dozwolone.\n\r", ch );
		return;
	}

	DEBUG_INFO("do_sifollow: remove char");
	found = sifollow_remove_char(ch, victim);
	DEBUG_INFO("do_sifollow: add char");
	if ( !found )
		sifollow_add_char( ch, victim );

	if ( found )
		send_to_char( "Usunieto osobê z listy.\n\r", ch );
	else
		send_to_char( "Dodano osobê do listy.\n\r", ch );

	DEBUG_INFO(NULL);
}

void do_nosi( CHAR_DATA *ch, char* argument )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument( argument, arg );
	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie ma nikogo takiego na ¶wiecie.\n\r", ch );
		return;
	}
	if ( !IS_NPC( victim ) )
	{
		send_to_char( "To nie jest NPC.\n\r", ch );
		return;
	}
	if ( victim->no_standard_si )
	{
		send_to_char( "W³±czam standardowe SI.\n\r", ch );
		victim->no_standard_si = FALSE;
	}
	else
	{
		send_to_char( "Wy³±czam standardowe SI.\n\r", ch );
		victim->no_standard_si = TRUE;
	}
	return;
}

void do_home( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	ROOM_INDEX_DATA *location;
	CHAR_DATA *rch;
	int count = 0;

    // builder's home location
    location = get_room_index( 10000 );
    if ( ! location )
       {
	  send_to_char("Nie ma takiej lokacji.\n\r", ch );
	  return;
       }

	one_argument( argument, arg );

    if ( ch->previous_room && !str_prefix( arg, "back" ) )
    {
        location = ch->previous_room;
    }

	count = 0;
	for ( rch = location->people; rch != NULL; rch = rch->next_in_room )
		count++;

	if ( !is_room_owner( ch, location ) && room_is_private( location )
		 && ( count > 1 || get_trust( ch ) < MAX_LEVEL ) )
	{
		send_to_char( "That room is private right now.\n\r", ch );
		return;
	}

	if ( ch->fighting != NULL )
    {
		stop_fighting( ch, TRUE );
    }

	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( get_trust( rch ) >= ch->invis_level )
		{
			if ( ch->pcdata != NULL && ch->pcdata->bamfout[ 0 ] != '\0' )
				act( "$t", ch, ch->pcdata->bamfout, rch, TO_VICT );
			else
				act( "$n rozp³ywa siê i znika.", ch, NULL, rch, TO_VICT );
		}
	}

	char_from_room( ch );
	char_to_room( ch, location );

	for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
	{
		if ( get_trust( rch ) >= ch->invis_level )
		{
			if ( ch->pcdata != NULL && ch->pcdata->bamfin[ 0 ] != '\0' )
				act( "$t", ch, ch->pcdata->bamfin, rch, TO_VICT );
			else
				act( "$n materializuje siê przed tob±.", ch, NULL, rch, TO_VICT );
		}
	}

	do_function( ch, &do_look, "auto" );
	return;
}

//rellik: do debugowania, w³±cza i wy³±cza zapisywanie informacji do debugowania
/*
Wykorzystywane przy ustalaniu czy save_debug_info ma zapisac informacje do pliku czy pominaæ to wystapienie.
 Ustawia poziom zapisywania informacji debugujacych 0 tylko b³edy, 1 podejrzane o bugi fragmenty kodu, 2 pozwala sledzic wykonywanie kodu, 3 zapisuje wszystkie wystapienia save_debug_info
*/
void do_debug( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_STRING_LENGTH ];
	const char *call = "act_wiz.c => do_debug";
	int i;
	int nmr, proj;
	char tmp[MAX_STRING_LENGTH ];

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Podaj parametry: debug <nr_projektu> <poziom_debugowania>\n\r", ch );
		send_to_char( "poziom_debugowania - warto¶æ od 0 (tylko b³êdy) do 3 (wszystko).\n\r", ch );
		send_to_char( "UWAGA! Poziom 3 nie jest zalecany, u¿ywaj je¶li wiesz co robisz!\n\r", ch );
		print_char( ch, "NR         Nazwa\n\r" );
		for ( i = 0; projects_table[i].name; ++i )
		{
			print_char( ch, "%d    %s\n\r", projects_table[i].number, projects_table[i].name );
		}
		print_char( ch, "\n\rObecnie debugowany jest projekt %s na poziomie %d.\n\r", projects_table[ debugged_project ].name, debuguj );
		print_char( ch, "W celu wy³±czenia debugowania ustaw: debuguj 0 0\n\r" );
		return;
	}
	argument = one_argument( argument, arg );

	if ( is_number( arg ) && is_number( argument ) )
	{
		proj = atoi( arg );
		nmr = atoi( argument );
	} else {
		send_to_char( "B³êdny parametr, u¿yj 'debug' bez parametrów aby zobaczyæ ich listê.\n\r", ch );
		return;
	}

	debugged_project = proj;
	debuguj = nmr;
	sprintf( tmp, "Debugowanie projektu %s ustawione na %d poziom", projects_table[ proj ].name, nmr );
	print_char( ch,"%s\n\r", tmp );
	save_debug_info( call, NULL, tmp, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ERROR, FALSE );
	return;
}
//rellik: do debugowania, ju¿ s± inne metody ale na moje potrzeby przynajmniej... zachêcam do stosowania
/*
	Parametry:
	actual - zazwyczaj funkcja w której sie znajduje odwo³anie
	caller - je¶li wiadomo, funkcja ktora wywo³a³a funkcjê w której znajduje siê odwo³anie
	txt - informacja dodatkowa np. o mo¿liwej przyczynie b³êdu lub jakies parametry
	debugged_project - numer projektu ktory jest aktualnie debugowany
	debuglevel - poziom debugowania w ktorym zapisywac informacje do pliku loga
	include_info - czy ustawiac zmienn± pamiêtaj±c± ostatni± komendê na actual czy nie
*/
void save_debug_info(const char *actual, const char *caller, char *txt,	int nr_proj, int debuglevel, bool include_info)
{
	char buf[MAX_STRING_LENGTH];
	ADD_LINE_NUMBER(buf);
	if ( ( debuguj >= debuglevel && nr_proj == debugged_project ) || debuglevel == DEBUG_LEVEL_ERROR )
	{
		if ( txt )
		{
		 sprintf( buf, "%s%s", buf, txt );
		}
		bugf( "INFO: Funkcja: %s, wywo³ana z: %s, info: %s, last_debug: %s, previous_debug: %s.\n",
				actual ? actual : "UNKNOWN",
				caller ? caller : "UNKNOWN",
				buf,
				last_command? last_command: "BRAK",
				previous_command? previous_command: "BRAK" );
	}
	if (actual && include_info)
	{
		if ( txt )
		{
			sprintf( buf, "%s%s, %s", buf, actual, txt );
			DEBUG_INFO(buf);
		} else {
			sprintf( buf, "%s%s", buf, actual );
			DEBUG_INFO(buf);
		}
	}
}
 /*
  * UWAGA: U¿ywaæ tylko w ostateczno¶ci i ostro¿nie, w przemy¶lanych miejscach, to potrafi rozdmuchaæ loga w kilka sekund do niebywa³ych rozmiarów
  */
//rellik, zakomentowuje bo jeszcze ktos to odpali i zostawi [20080518]
/*
void debug_dump()
{
	fprintf( stderr, "DEBUG DUMP: Ostatnia: %s, przedostatnia: %s\n", last_command? last_command: "BRAK DANYCH", previous_command? previous_command: "BRAK DANYCH");
}
*/
//rellik: komponenty, pokazuje obiekty z ustawion± komponentowo¶ci±
void do_silist( CHAR_DATA *ch, char *argument )
{
	extern unsigned int top_obj_index;
	OBJ_INDEX_DATA *pObjIndex;
	ush_int vnum;
	int nMatch=0;

	for (vnum = 0; nMatch < top_obj_index; vnum++)
	{
		if ( (pObjIndex = get_obj_index(vnum) ) != NULL)
		{
			nMatch++;
			if ( pObjIndex && pObjIndex->is_spell_item )
			{
				print_char( ch, "[%5d] %s\n\r", pObjIndex->vnum, pObjIndex->short_descr);
			}
		}
	}
}

/**
 * do_mset helper for checking range
 */
bool check_stat_range( CHAR_DATA *ch, int value )
{
    if ( value < 1 || value > MAX_STAT_VALUE )
    {
        char buf[ MAX_STRING_LENGTH ];
        sprintf( buf, "Strength range is 3 to %d\n\r.", MAX_STAT_VALUE );
        send_to_char( buf, ch );
        return TRUE;
    }
    return FALSE;
}

