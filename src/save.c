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
 * $Id: save.c 12408 2013-06-12 12:29:02Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/save.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lang.h"
#include "clans.h"
#include "magic.h"
#include "shapeshifting.h"
#include "friend_who.h"
#include "projects.h"
#include "money.h"

void add_host( CHAR_DATA *ch, char * host, bool insert );

#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif

//rellik: do debugowania, nie wiem czy tu moze byc deklaracja funkcji (definicja w act_wiz)
void save_debug_info(const char *actual, const char *caller, char *txt,	int nr_proj, int debuglevel, bool include_info);
void mem_update_count ( CHAR_DATA *ch );

/* int rename(const char *oldfname, const char *newfname); viene en stdio.h */

char *print_flags(int flag)
{
    int count, pos = 0;
    static char buf[52];


    for (count = 0; count < 32;  count++)
    {
        if (IS_SET(flag,1<<count))
        {
            if (count < 26)
                buf[pos] = 'A' + count;
            else
                buf[pos] = 'a' + (count - 26);
            pos++;
        }
    }

    if (pos == 0)
    {
        buf[pos] = '0';
        pos++;
    }

    buf[pos] = '\0';

    return buf;
}

char *print_ext_flags( long *flags )
{
    int bank, bit;
    static char buf[100];
	char tmp_buf[10];
	bool was_bank;
	bool first_flag;

	buf[0] = '\0';

	if ( ext_flags_none( flags ) )
	{
		sprintf( buf, "0" );
		return buf;
	}

	first_flag = TRUE;
    for ( bank = 0; bank < MAX_VECT_BANK; bank++ )
    {
		was_bank = FALSE;
		for ( bit = 0; bit < 32; bit++ )
		{
			if ( IS_SET( *(flags+bank), 1 << bit ) )
			{
				if ( !was_bank )
				{
					if ( !first_flag )
						strcat( buf, "/" );

					sprintf( tmp_buf, "%d|", bank );
					strcat( buf, tmp_buf );
					was_bank = TRUE;
				}

				if ( bit < 26 )
					sprintf( tmp_buf, "%c", 'A' + bit );
				else
					sprintf( tmp_buf, "%c", 'a' + ( bit - 26 ) );

				strcat( buf, tmp_buf );

				first_flag = FALSE;
			}
		}
    }

    return buf;
}

//rellik: bigflagi, zapisywanie flag do string
char *print_big_flag( BIGFLAG *f )
{
	int nb_bank = BIGFLAG_CAPACITY;
	int i;
	char one[50];
	static char buf[MAX_STRING_LENGTH];

	buf[0] = '\0';

	for( i = 0; i < nb_bank; ++i )
	{
		sprintf( one, "%d ", f->bank[i] );
		strcat( buf, one );
	}
	return buf;
}

/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST	100
static	OBJ_DATA *	rgObjNest	[MAX_NEST];



/*
 * Local functions.
 */
void	fwrite_char	args( ( CHAR_DATA *ch,  FILE *fp, bool remote ) );
void	fwrite_obj	args( ( CHAR_DATA *ch,  OBJ_DATA  *obj,
			    FILE *fp, int iNest, bool pccorpse, bool remote ) );
bool	fread_char	args( ( CHAR_DATA *ch,  FILE *fp, bool remote ) );
void	fread_obj	args( ( CHAR_DATA *ch,  FILE *fp, bool pccorpse, bool remote ) );

CHAR_DATA* init_char( char *name )
{
	CHAR_DATA *ch;
    int stat;

	ch = new_char();
	ch->pcdata = new_pcdata();

	ch->name		= str_dup( name );
	ch->old_name	= str_dup( "" );
	ch->name2		= str_dup( "null" );
	ch->name3		= str_dup( "null" );
	ch->name4		= str_dup( "null" );
	ch->name5		= str_dup( "null" );
	ch->name6		= str_dup( "null" );
	ch->id			= get_pc_id();
	ch->race		= race_lookup("cz³owiek");
	ch->real_race	= ch->race;
	ext_flags_clear( ch->act );
	EXT_SET_BIT( ch->act, PLR_COLOUR );
	EXT_SET_BIT( ch->act, PLR_AUTOMEM );
	EXT_SET_BIT( ch->act, PLR_AUTOEXIT );
	EXT_SET_BIT( ch->act, PLR_AUTOGOLD );
	ch->comm		= COMM_COMBINE | COMM_PROMPT | COMM_HINT | COMM_NEWBIE;
	ch->prompt 		= str_dup(STANDARD_PROMPT);
	ch->memspell	    = NULL;
	ch->memming		= NULL;
	ch->memset       = NULL;
	ch->hoard       = NULL;
	ch->count_memspell = 0;
	ch->si_followed = NULL;
	ch->si_followed_cnt = 0;
	ch->no_standard_si = FALSE;
	mem_update_count( ch );

	ch -> statpointsleft = 0;
	ch -> statpointsspent = 0;

	for (stat =0; stat < MAX_RESIST; stat++)
	ch->resists[stat] = 0;

	for (stat =0; stat < MAX_RESIST; stat++)
	ch->healing_from[stat] = 0;

	ch->pcdata->ql_data = NULL;
	ch->pcdata->last_rent		= 0;
	ch->pcdata->pwd				= str_dup( "" );
	ch->pcdata->bamfin			= str_dup( "" );
	ch->pcdata->bamfout			= str_dup( "" );
	ch->pcdata->title			= str_dup( "" );
	ch->pcdata->new_title		= str_dup( "" );
	ch->pcdata->ignore			= str_dup( "" );
	ch->pcdata->last_host		= str_dup( "" );
	ch->pcdata->name_deny_txt	= str_dup( "" );
	ch->pcdata->hosts_count		= 0;
	ch->pcdata->afk_text		= NULL;
	ch->pcdata->wiz_conf		= 0;
	ch->pcdata->remote_char		= NULL;
	ch->pcdata->last_rent_cost	= 0;
	ch->pcdata->last_logoff		= current_time;

	ch->condition[COND_THIRST]	= 24;
	ch->condition[COND_FULL]	= 48;
	ch->condition[COND_HUNGER]	= 24;
	ch->condition[COND_SLEEPY]	= 48;

	ch->pcdata->text[0]				= ( NORMAL );
	ch->pcdata->text[1]				= ( WHITE );
	ch->pcdata->text[2]				= 0;
	ch->pcdata->auction[0]			= ( BRIGHT );
	ch->pcdata->auction[1]			= ( YELLOW );
	ch->pcdata->auction[2]			= 0;
	ch->pcdata->auction_text[0]		= ( BRIGHT );
	ch->pcdata->auction_text[1]		= ( WHITE );
	ch->pcdata->auction_text[2]		= 0;
	ch->pcdata->gossip[0]			= ( NORMAL );
	ch->pcdata->gossip[1]			= ( MAGENTA );
	ch->pcdata->gossip[2]			= 0;
	ch->pcdata->gossip_text[0]		= ( BRIGHT );
	ch->pcdata->gossip_text[1]		= ( MAGENTA );
	ch->pcdata->gossip_text[2]		= 0;
	ch->pcdata->music[0]			= ( NORMAL );
	ch->pcdata->music[1]			= ( RED );
	ch->pcdata->music[2]			= 0;
	ch->pcdata->music_text[0]		= ( BRIGHT );
	ch->pcdata->music_text[1]		= ( RED );
	ch->pcdata->music_text[2]		= 0;
	ch->pcdata->question[0]			= ( NORMAL );
	ch->pcdata->question[1]			= ( GREEN );
	ch->pcdata->question[2]			= 0;
	ch->pcdata->question_text[0]	= ( BRIGHT );
	ch->pcdata->question_text[1]	= ( GREEN );
	ch->pcdata->question_text[2]	= 0;
	ch->pcdata->answer[0]			= ( BRIGHT );
	ch->pcdata->answer[1]			= ( YELLOW );
	ch->pcdata->answer[2]			= 0;
	ch->pcdata->answer_text[0]		= ( BRIGHT );
	ch->pcdata->answer_text[1]		= ( WHITE );
	ch->pcdata->answer_text[2]		= 0;
	ch->pcdata->quote[0]			= ( NORMAL );
	ch->pcdata->quote[1]			= ( YELLOW );
	ch->pcdata->quote[2]			= 0;
	ch->pcdata->quote_text[0]		= ( NORMAL );
	ch->pcdata->quote_text[1]		= ( GREEN );
	ch->pcdata->quote_text[2]		= 0;
	ch->pcdata->immtalk_text[0]		= ( NORMAL );
	ch->pcdata->immtalk_text[1]		= ( CYAN );
	ch->pcdata->immtalk_text[2]		= 0;
	ch->pcdata->immtalk_type[0]		= ( NORMAL );
	ch->pcdata->immtalk_type[1]		= ( YELLOW );
	ch->pcdata->immtalk_type[2]		= 0;
	ch->pcdata->info[0]				= ( BRIGHT );
	ch->pcdata->info[1]				= ( YELLOW );
	ch->pcdata->info[2]				= 1;
	ch->pcdata->say[0]				= ( NORMAL );
	ch->pcdata->say[1]				= ( GREEN );
	ch->pcdata->say[2]				= 0;
	ch->pcdata->say_text[0]			= ( BRIGHT );
	ch->pcdata->say_text[1]			= ( GREEN );
	ch->pcdata->say_text[2]			= 0;
	ch->pcdata->tell[0]				= ( NORMAL );
	ch->pcdata->tell[1]				= ( GREEN );
	ch->pcdata->tell[2]				= 0;
	ch->pcdata->tell_text[0]		= ( BRIGHT );
	ch->pcdata->tell_text[1]		= ( GREEN );
	ch->pcdata->tell_text[2]		= 0;
	ch->pcdata->reply[0]			= ( NORMAL );
	ch->pcdata->reply[1]			= ( GREEN );
	ch->pcdata->reply[2]			= 0;
	ch->pcdata->reply_text[0]		= ( BRIGHT );
	ch->pcdata->reply_text[1]		= ( GREEN );
	ch->pcdata->reply_text[2]		= 0;
	ch->pcdata->gtell_text[0]		= ( BRIGHT );
	ch->pcdata->gtell_text[1]		= ( YELLOW );
	ch->pcdata->gtell_text[2]		= 0;
	ch->pcdata->gtell_type[0]		= ( NORMAL );
	ch->pcdata->gtell_type[1]		= ( YELLOW );
	ch->pcdata->gtell_type[2]		= 0;
	ch->pcdata->wiznet[0]			= ( NORMAL );
	ch->pcdata->wiznet[1]			= ( GREEN );
	ch->pcdata->wiznet[2]			= 0;
	ch->pcdata->room_title[0]		= ( NORMAL );
	ch->pcdata->room_title[1]		= ( CYAN );
	ch->pcdata->room_title[2]		= 0;
	ch->pcdata->room_text[0]		= ( NORMAL );
	ch->pcdata->room_text[1]		= ( WHITE );
	ch->pcdata->room_text[2]		= 0;
	ch->pcdata->room_exits[0]		= ( NORMAL );
	ch->pcdata->room_exits[1]		= ( GREEN );
	ch->pcdata->room_exits[2]		= 0;
	ch->pcdata->room_things[0]		= ( NORMAL );
	ch->pcdata->room_things[1]		= ( CYAN );
	ch->pcdata->room_things[2]		= 0;
	ch->pcdata->shout[0]			= ( NORMAL );
	ch->pcdata->shout[1]			= ( CYAN );
	ch->pcdata->shout[2]			= 0;
	ch->pcdata->shout_text[0]		= ( BRIGHT );
	ch->pcdata->shout_text[1]		= ( CYAN );
	ch->pcdata->shout_text[2]		= 0;
	ch->pcdata->yell[0]				= ( NORMAL );
	ch->pcdata->yell[1]				= ( CYAN );
	ch->pcdata->yell[2]				= 0;
	ch->pcdata->yell_text[0]		= ( BRIGHT );
	ch->pcdata->yell_text[1]		= ( CYAN );
	ch->pcdata->yell_text[2]		= 0;
	ch->pcdata->prompt[0]			= ( NORMAL );
	ch->pcdata->prompt[1]			= ( CYAN );
	ch->pcdata->prompt[2]			= 0;
	ch->pcdata->fight_death[0]		= ( BRIGHT );
	ch->pcdata->fight_death[1]		= ( RED );
	ch->pcdata->fight_death[2]		= 0;
	ch->pcdata->fight_yhit[0]		= ( NORMAL );
	ch->pcdata->fight_yhit[1]		= ( GREEN );
	ch->pcdata->fight_yhit[2]		= 0;
	ch->pcdata->fight_ohit[0]		= ( NORMAL );
	ch->pcdata->fight_ohit[1]		= ( YELLOW );
	ch->pcdata->fight_ohit[2]		= 0;
	ch->pcdata->fight_thit[0]		= ( NORMAL );
	ch->pcdata->fight_thit[1]		= ( RED );
	ch->pcdata->fight_thit[2]		= 0;
	ch->pcdata->fight_skill[0]		= ( BRIGHT );
	ch->pcdata->fight_skill[1]		= ( WHITE );
	ch->pcdata->fight_skill[2]		= 0;
	ch->pcdata->fight_trick[0]		= ( BRIGHT );
	ch->pcdata->fight_trick[1]		= ( WHITE );
	ch->pcdata->fight_trick[2]		= 0;
	ch->pcdata->fight_spell[0]		= ( BRIGHT );
	ch->pcdata->fight_spell[1]		= ( WHITE );
	ch->pcdata->fight_spell[2]		= 0;
	ch->pcdata->security			= 0;	/* OLC */
	ch->wiz_config					= WIZCFG_SEXCONV | WIZCFG_COLOURCONV;
	ch->pcdata->rolls_count			= 0;
	ch->pcdata->new_rolls_count		= 0;
	ch->pcdata->mind_in				= NULL;
	ch->ss_data						= NULL;
	ch->pcdata->friends				= NULL;
	ch->pcdata->introduced			= NULL;
	ch->age					= 0;
	ch->glory				= 0;
	return ch;
}

/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj( CHAR_DATA *ch, bool save_newbies, bool remote )
{
	char strsave[ MAX_INPUT_LENGTH ];
	FILE *fp;

	if ( IS_NPC( ch ) )
		return ;

	if ( IS_NULLSTR( ch->name ) )
		return ;

	if ( ch->desc != NULL && ch->desc->original != NULL )
		ch = ch->desc->original;

	if ( ch->level < 2 && !save_newbies )
		return ;

#if defined(unix)
	/* create god log */
	if ( IS_IMMORTAL( ch ) || ch->level >= LEVEL_IMMORTAL )
	{
		sprintf( strsave, "%s%s", GOD_DIR, capitalize( ch->name ) );
		fclose( fpReserve );
		if ( ( fp = fopen( strsave, "w" ) ) == NULL )
		{
			fpReserve = fopen( NULL_FILE, "r" );
			bug( "Save_char_obj: fopen", 0 );
			perror( strsave );

			return;
		}

		fprintf( fp, "Lev %2d Trust %2d  %s%s\n",
		         ch->level, get_trust( ch ), ch->name, ch->pcdata->title );
		fclose( fp );
		fpReserve = fopen( NULL_FILE, "r" );
	}
#endif

	fclose( fpReserve );
	sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
	if ( ( fp = fopen( TEMP_FILE, "w" ) ) == NULL )
	{
		bug( "Save_char_obj: fopen", 0 );
		perror( strsave );
	}
	else
	{
        // shapeshifting, zanim zapiszemy postac przywracamy jej normala
        // forme, a po zapisie znow automagicznie przywracamy przemianê
		int max_hit = ch->max_hit;
        int hit = ch->hit;
        SHAPESHIFT_DATA* ss = NULL;

        if ( ch->ss_data )
            ss = do_reform_silent ( ch );


        ch->ss_data = NULL;

        fwrite_char( ch, fp, remote );
		if ( ch->carrying != NULL )
		    fwrite_obj( ch, ch->carrying, fp, 0, FALSE, remote );

        if ( ch->hoard != NULL )
		    fwrite_obj( ch, ch->hoard, fp, 0, FALSE, remote );


        if ( ss )
            do_shapeshift_silent( ch, ss );


        ch->max_hit = max_hit;
		ch->hit = hit;

        fprintf( fp, "#END\n" );
	}

	fflush( fp );
#ifndef CYGWIN
	fsync( fileno(fp) );
#endif
	fclose( fp );
	rename( TEMP_FILE, strsave );
	fpReserve = fopen( NULL_FILE, "r" );
	return ;
}

/*
 * Write the char.
 */
void fwrite_char( CHAR_DATA *ch, FILE *fp, bool remote )
{
	AFFECT_DATA * paf;
	MSPELL_DATA *tmp;
	MEMSET_DATA *memset;
	PFLAG_DATA *pflag;
//	SPELL_FAILED *list;
	int sn, pos;
	int x;
	//rellik: friend_who
	FRIEND_LIST *wsk;

	fprintf( fp, "#%s\n", IS_NPC( ch ) ? "MOB" : "PLAYER"	);

	fprintf( fp, "Name %s~\n", ch->name	);
	fprintf( fp, "Todelete %ld\n", ch->pcdata->todelete ); // delayed ch delete - by Fuyara

	if ( ch->desc && ch->desc->account && !IS_NULLSTR( ch->desc->account->email ) )
		fprintf( fp, "Account %s~\n", ch->desc->account->email );
	else if ( ch->pcdata && !IS_NULLSTR( ch->pcdata->account_email ) )
		fprintf( fp, "Account %s~\n", ch->pcdata->account_email );

	fprintf( fp, "Odmiana %s~%s~%s~%s~%s~\n", ch->name2,
	         ch->name3, ch->name4, ch->name5, ch->name6 );

	fprintf( fp, "Id   %ld\n", ch->id	);
	if ( remote )
		fprintf( fp, "LogO %ld\n", ch->pcdata->last_logoff	);
	else
		fprintf( fp, "LogO %ld\n", current_time	);
	if ( remote )
		fprintf( fp, "Rent %d\n", ch->pcdata->last_rent_cost );
	else
		fprintf( fp, "Rent %d\n", get_rent_cost( ch, ch->carrying, FALSE ) + get_hoard_cost( ch, ch->hoard, FALSE ) );
	fprintf( fp, "Vers %d\n", 6	);

	if ( ch->short_descr[ 0 ] != '\0' )
		fprintf( fp, "ShD  %s~\n", ch->short_descr	);

	if ( ch->long_descr[ 0 ] != '\0' )
		fprintf( fp, "LnD  %s~\n", ch->long_descr	);

	if ( ch->description[ 0 ] != '\0' )
		fprintf( fp, "Desc %s~\n", ch->description	);

	if ( !IS_NULLSTR( ch->prompt ) )
		fprintf( fp, "Prom %s~\n", ch->prompt );

	fprintf( fp, "Race %s~\n", pc_race_table[ GET_REAL_RACE( ch ) ].name );

	fprintf( fp, "Sex  %d\n", ch->sex	);
	fprintf( fp, "Class  %d\n", ch->class	);

	if ( !IS_NPC( ch ) && ch->pcdata->mage_specialist >= 0 )
		fprintf( fp, "Spec  %d\n", ch->pcdata->mage_specialist );

	fprintf( fp, "Levl %d\n", ch->level	);


	if ( !IS_NPC( ch ) )
	{
		fprintf( fp, "HitGains %d", MAX_LEVEL );
		for ( x = 0; x < MAX_LEVEL; x++ )
			fprintf( fp, " %d", ch->pcdata->perm_hit_per_level[x] );
		fprintf( fp, "\n" );
	}

	if ( ch->trust != 0 )
		fprintf( fp, "Tru  %d\n", ch->trust	);

	fprintf( fp, "Sec  %d\n", ch->pcdata->security	);	/* OLC */
	if ( remote )
		fprintf( fp, "Plyd %d\n", ch->played );
	else
		fprintf( fp, "Plyd %d\n", ch->played + ( int ) ( current_time - ch->logon ) );

	fprintf( fp, "Scro %d\n", ch->lines	);
	fprintf( fp, "WizGr %s\n", print_flags( ch->pcdata->wiz_conf ) );

	if ( ch->in_room && EXT_IS_SET( ch->in_room->room_flags, ROOM_INN ) )
	{
		fprintf( fp, "Room %d\n", ch->in_room->vnum );
	}
	else
	{
		if ( IS_IMMORTAL( ch ) )
			fprintf( fp, "Room %d\n", ch->in_room ? ch->in_room->vnum :
			         ( ch->pcdata->last_rent > 0 ? ch->pcdata->last_rent : 1000 ) );
		else if ( !IS_IMMORTAL( ch ) && ch->pcdata->last_rent > 0 )
			fprintf( fp, "Room %d\n", ch->pcdata->last_rent );
		else
			fprintf( fp, "Room %d\n", 599 );
	}

	fprintf( fp, "HV  %d %d %d %d\n",
	         ch->hit, ch->max_hit, ch->move, ch->max_move );

	fprintf( fp, "HW %d %d\n", ch->height, ch->weight );

    /**
     * money
     */

    fprintf
        (
         fp,
         "Money %ld %ld %ld %ld %ld\n",
         ( ch->bank    > 0 )? ch->bank    : 0,
         ( ch->copper  > 0 )? ch->copper  : 0,
         ( ch->silver   > 0 )? ch->silver   : 0,
         ( ch->gold    > 0 )? ch->gold    : 0,
         ( ch->mithril > 0 )? ch->mithril : 0
        );

	fprintf( fp, "Exp  %ld\n", ch->exp	);

	fprintf( fp, "ActExt  %s\n", print_ext_flags( ch->act ) );

	fprintf( fp, "Counter %d %d %d %d\n", ch->counter[ 0 ], ch->counter[ 1 ],
	         ch->counter[ 2 ], ch->counter[ 3 ] );

	fprintf( fp, "Counter2 %d", MAX_COUNTER - 4 );
	for ( x = 4; x < MAX_COUNTER; x++ )
		fprintf( fp, " %d", ch->counter[ x ] );
	fprintf( fp, "\n" );

	fprintf( fp, "Affected %s\n", print_ext_flags( ch->affected_by ) );

	fprintf( fp, "Comm %s\n", print_flags( ch->comm ) );

	if ( ch->wiznet )
		fprintf( fp, "Wizn %s\n", print_flags( ch->wiznet ) );

	if ( ch->wiz_config )
		fprintf( fp, "WizCfg %s\n", print_flags( ch->wiz_config ) );

	if ( ch->invis_level )
		fprintf( fp, "Invi %d\n", ch->invis_level	);

	if ( ch->incog_level )
		fprintf( fp, "Inco %d\n", ch->incog_level );
	/*mount*/
	if ( ch->position == 9 )
		ch->position = 8;
	/********/

	fprintf( fp, "Pos  %d\n",
	         ch->position == POS_FIGHTING ? POS_STANDING : ch->position );

	for ( x = 0;x < 5;x++ )
		if ( ch->saving_throw[ x ] != 0 )
		{
			fprintf( fp, "Saving  %d %d %d %d %d\n",
			         ch->saving_throw[ 0 ], ch->saving_throw[ 1 ], ch->saving_throw[ 2 ],
			         ch->saving_throw[ 3 ], ch->saving_throw[ 4 ] );
			break;
		}

	fprintf( fp, "Alig  %d\n", ch->alignment	);

	if ( ch->hitroll != 0 )
		fprintf( fp, "Hit   %d\n", ch->hitroll	);

	if ( ch->damroll != 0 )
		fprintf( fp, "Dam   %d\n", ch->damroll	);

	fprintf( fp, "ACs %d %d %d %d\n",
	         ch->armor[ 0 ], ch->armor[ 1 ], ch->armor[ 2 ], ch->armor[ 3 ] );

	if ( ch->wimpy != 0 )
		fprintf( fp, "Wimp  %d\n", ch->wimpy	);

	fprintf( fp, "ANewStat %d %d %d %d %d %d %d\n",
	         ch->perm_stat[ STAT_STR ],
	         ch->perm_stat[ STAT_INT ],
	         ch->perm_stat[ STAT_WIS ],
	         ch->perm_stat[ STAT_DEX ],
	         ch->perm_stat[ STAT_CON ],
	         ch->perm_stat[ STAT_CHA ],
	         ch->perm_stat[ STAT_LUC ] );


	fprintf ( fp, "AModNewStat %d %d %d %d %d %d %d\n",
	          ch->new_mod_stat[ STAT_STR ],
	          ch->new_mod_stat[ STAT_INT ],
	          ch->new_mod_stat[ STAT_WIS ],
	          ch->new_mod_stat[ STAT_DEX ],
	          ch->new_mod_stat[ STAT_CON ],
	          ch->new_mod_stat[ STAT_CHA ],
	          ch->new_mod_stat[ STAT_LUC ] );

	fprintf ( fp, "StatPoints %d %d\n", ch->statpointsleft, ch->statpointsspent);

/* BEGIN: reward (autoquest) */
    if (ch->pcdata->hunt_time != 0)
    {
        fprintf( fp, "HuntTime %d\n",  ch->pcdata->hunt_time );
    }
    if (ch->pcdata->rewards != 0)
    {
        fprintf( fp, "Rewards %d\n",  ch->pcdata->rewards );
    }
    if (ch->pcdata->recovery != 0)
    {
        fprintf( fp, "Recovery %d\n",  ch->pcdata->recovery );
    }
    if (ch->pcdata->reward_obj != 0)
    {
        fprintf( fp, "RewardObj %d\n",  ch->pcdata->reward_obj );
    }
    if (ch->pcdata->reward_mob != 0)
    {
        fprintf( fp, "RewardMob %d\n",  ch->pcdata->reward_mob );
    }
    if (ch->pcdata->rewarder != 0)
    {
        fprintf( fp, "Rewarder %d\n",  ch->pcdata->rewarder );
    }
/* END: reward (autoquest) */

	if ( !IS_NPC( ch ) )
	{
		for ( tmp = ch->memspell;tmp != NULL;tmp = tmp->next )
		{
			fprintf( fp, "Mem %d %d %d\n", tmp->spell, tmp->circle, tmp->done );
		}

		for ( memset = ch->memset; memset; memset = memset->next )
		{
		    fprintf( fp, "MemSet %s", memset->set_name);
		    for ( x = 0 ; x < MAX_SKILL ; x++ )
		        if (  memset->set_spells[x] > 0 )
		            fprintf( fp, " %d %d", x, memset->set_spells[x]);

		    fprintf( fp, "\n");
		}

	}

	/* trophy etc */
	if ( !IS_NPC( ch ) )
    {
        for ( x = 0 ; x < TROPHY_SIZE ; x++ )
        {
            if ( ch->pcdata->trophy[ x ][ 0 ] )
            {
                fprintf( fp, "Trophy %d %d %d\n", x, ch->pcdata->trophy[ x ][ 0 ], ch->pcdata->trophy[ x ][ 1 ] );
            }
            else
            {
                fprintf( fp, "Trophy %d 0 0\n", x );
            }
        }
    }

	//rellik: komponenty, zapis wiedzy o komponentach
	if ( !IS_NPC( ch ))
	{
		fprintf( fp, "SIknows %s\n", print_big_flag( &ch->pcdata->spell_items_knowledge ) );
	}

	if ( !IS_NPC( ch ) )
	{
		QL_DATA * tmp;
		if ( ch->pcdata->ql_data != NULL )
		{
			for ( tmp = ch->pcdata->ql_data;tmp != NULL;tmp = tmp->next )
			{
			   fprintf( fp, "QLog %s~%s~%s~%s~%d %d\n", tmp->qname, tmp->title, tmp->text, (tmp->currentdesc?tmp->currentdesc:"null"), tmp->state, tmp->date );
			}
		}
	}

	if ( IS_NPC( ch ) )
	{
		fprintf( fp, "Vnum %d\n", ch->pIndexData->vnum	);
	}
	else
	{
		fprintf( fp, "Pass %s~\n", ch->pcdata->pwd	);
		if ( ch->pcdata->bamfin[ 0 ] != '\0' )
			fprintf( fp, "Bin  %s~\n", ch->pcdata->bamfin );
		if ( ch->pcdata->bamfout[ 0 ] != '\0' )
			fprintf( fp, "Bout %s~\n", ch->pcdata->bamfout );

		if ( ch->pcdata->afk_text != NULL )
			fprintf( fp, "Afktext %s~\n", ch->pcdata->afk_text );


		fprintf( fp, "Speaking %d\n", ch->speaking	);
		fprintf( fp, "Titl %s~\n", ch->pcdata->title	);
		if ( !IS_NULLSTR( ch->pcdata->new_title ) )
			fprintf( fp, "NewTitl %s~\n", ch->pcdata->new_title	);
		fprintf( fp, "Ignore %s~\n", ch->pcdata->ignore	);
		fprintf( fp, "Pnts %d\n", ch->pcdata->points );
		fprintf( fp, "TSex %d\n", ch->pcdata->true_sex	);
		fprintf( fp, "LLev %d\n", ch->pcdata->last_level	);
		fprintf( fp, "HMVP %d %d %d\n", ch->pcdata->perm_hit, 0, ch->pcdata->perm_move );

		/*	fprintf( fp, "Cnd  %d %d %d %d\n",
			    ch->condition[0],
			    ch->condition[1],
			    ch->condition[2],
			    ch->condition[3] );*/

		fprintf( fp, "Cnd_NEW  %d %d %d %d %d\n",
		         ch->condition[ 0 ],
		         ch->condition[ 1 ],
		         ch->condition[ 2 ],
		         ch->condition[ 3 ],
		         ch->condition[ 4 ] );

		/*
		 * Write Colour Config Information.
		 */
		fprintf( fp, "Coloura     %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d\n",
		         ch->pcdata->text[ 2 ],
		         ch->pcdata->text[ 0 ],
		         ch->pcdata->text[ 1 ],
		         ch->pcdata->auction[ 2 ],
		         ch->pcdata->auction[ 0 ],
		         ch->pcdata->auction[ 1 ],
		         ch->pcdata->gossip[ 2 ],
		         ch->pcdata->gossip[ 0 ],
		         ch->pcdata->gossip[ 1 ],
		         ch->pcdata->music[ 2 ],
		         ch->pcdata->music[ 0 ],
		         ch->pcdata->music[ 1 ],
		         ch->pcdata->question[ 2 ],
		         ch->pcdata->question[ 0 ],
		         ch->pcdata->question[ 1 ] );
		fprintf( fp, "Colourb     %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d\n",
		         ch->pcdata->answer[ 2 ],
		         ch->pcdata->answer[ 0 ],
		         ch->pcdata->answer[ 1 ],
		         ch->pcdata->quote[ 2 ],
		         ch->pcdata->quote[ 0 ],
		         ch->pcdata->quote[ 1 ],
		         ch->pcdata->quote_text[ 2 ],
		         ch->pcdata->quote_text[ 0 ],
		         ch->pcdata->quote_text[ 1 ],
		         ch->pcdata->immtalk_text[ 2 ],
		         ch->pcdata->immtalk_text[ 0 ],
		         ch->pcdata->immtalk_text[ 1 ],
		         ch->pcdata->immtalk_type[ 2 ],
		         ch->pcdata->immtalk_type[ 0 ],
		         ch->pcdata->immtalk_type[ 1 ] );
		fprintf( fp, "Colourc     %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d\n",
		         ch->pcdata->info[ 2 ],
		         ch->pcdata->info[ 0 ],
		         ch->pcdata->info[ 1 ],
		         ch->pcdata->tell[ 2 ],
		         ch->pcdata->tell[ 0 ],
		         ch->pcdata->tell[ 1 ],
		         ch->pcdata->reply[ 2 ],
		         ch->pcdata->reply[ 0 ],
		         ch->pcdata->reply[ 1 ],
		         ch->pcdata->gtell_text[ 2 ],
		         ch->pcdata->gtell_text[ 0 ],
		         ch->pcdata->gtell_text[ 1 ],
		         ch->pcdata->gtell_type[ 2 ],
		         ch->pcdata->gtell_type[ 0 ],
		         ch->pcdata->gtell_type[ 1 ] );
		fprintf( fp, "Colourd     %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d\n",
		         ch->pcdata->room_title[ 2 ],
		         ch->pcdata->room_title[ 0 ],
		         ch->pcdata->room_title[ 1 ],
		         ch->pcdata->room_text[ 2 ],
		         ch->pcdata->room_text[ 0 ],
		         ch->pcdata->room_text[ 1 ],
		         ch->pcdata->room_exits[ 2 ],
		         ch->pcdata->room_exits[ 0 ],
		         ch->pcdata->room_exits[ 1 ],
		         ch->pcdata->room_things[ 2 ],
		         ch->pcdata->room_things[ 0 ],
		         ch->pcdata->room_things[ 1 ],
		         ch->pcdata->prompt[ 2 ],
		         ch->pcdata->prompt[ 0 ],
		         ch->pcdata->prompt[ 1 ] );
		fprintf( fp, "Coloure     %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d\n",
		         ch->pcdata->fight_death[ 2 ],
		         ch->pcdata->fight_death[ 0 ],
		         ch->pcdata->fight_death[ 1 ],
		         ch->pcdata->fight_yhit[ 2 ],
		         ch->pcdata->fight_yhit[ 0 ],
		         ch->pcdata->fight_yhit[ 1 ],
		         ch->pcdata->fight_ohit[ 2 ],
		         ch->pcdata->fight_ohit[ 0 ],
		         ch->pcdata->fight_ohit[ 1 ],
		         ch->pcdata->fight_thit[ 2 ],
		         ch->pcdata->fight_thit[ 0 ],
		         ch->pcdata->fight_thit[ 1 ],
		         ch->pcdata->fight_skill[ 2 ],
		         ch->pcdata->fight_skill[ 0 ],
		         ch->pcdata->fight_skill[ 1 ],
		         ch->pcdata->fight_trick[ 2 ],
		         ch->pcdata->fight_trick[ 0 ],
		         ch->pcdata->fight_trick[ 1 ],
		         ch->pcdata->fight_spell[ 2 ],
		         ch->pcdata->fight_spell[ 0 ],
		         ch->pcdata->fight_spell[ 1 ] );
		fprintf( fp, "Colourf     %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d\n",
		         ch->pcdata->wiznet[ 2 ],
		         ch->pcdata->wiznet[ 0 ],
		         ch->pcdata->wiznet[ 1 ],
		         ch->pcdata->say[ 2 ],
		         ch->pcdata->say[ 0 ],
		         ch->pcdata->say[ 1 ],
		         ch->pcdata->say_text[ 2 ],
		         ch->pcdata->say_text[ 0 ],
		         ch->pcdata->say_text[ 1 ],
		         ch->pcdata->tell_text[ 2 ],
		         ch->pcdata->tell_text[ 0 ],
		         ch->pcdata->tell_text[ 1 ],
		         ch->pcdata->reply_text[ 2 ],
		         ch->pcdata->reply_text[ 0 ],
		         ch->pcdata->reply_text[ 1 ] );
		fprintf( fp, "Colourg     %d%d%d %d%d%d %d%d%d %d%d%d %d%d%d\n",
		         ch->pcdata->auction_text[ 2 ],
		         ch->pcdata->auction_text[ 0 ],
		         ch->pcdata->auction_text[ 1 ],
		         ch->pcdata->gossip_text[ 2 ],
		         ch->pcdata->gossip_text[ 0 ],
		         ch->pcdata->gossip_text[ 1 ],
		         ch->pcdata->music_text[ 2 ],
		         ch->pcdata->music_text[ 0 ],
		         ch->pcdata->music_text[ 1 ],
		         ch->pcdata->question_text[ 2 ],
		         ch->pcdata->question_text[ 0 ],
		         ch->pcdata->question_text[ 1 ],
		         ch->pcdata->answer_text[ 2 ],
		         ch->pcdata->answer_text[ 0 ],
		         ch->pcdata->answer_text[ 1 ] );
		fprintf( fp, "Colourh     %d%d%d %d%d%d %d%d%d %d%d%d\n",
		         ch->pcdata->shout[ 2 ],
		         ch->pcdata->shout[ 0 ],
		         ch->pcdata->shout[ 1 ],
		         ch->pcdata->shout_text[ 2 ],
		         ch->pcdata->shout_text[ 0 ],
		         ch->pcdata->shout_text[ 1 ],
		         ch->pcdata->yell[ 2 ],
		         ch->pcdata->yell[ 0 ],
		         ch->pcdata->yell[ 1 ],
		         ch->pcdata->yell_text[ 2 ],
		         ch->pcdata->yell_text[ 0 ],
		         ch->pcdata->yell_text[ 1 ] );

		/* write alias */
		for ( pos = 0; pos < MAX_ALIAS; pos++ )
		{
			if ( ch->pcdata->alias[ pos ] == NULL
			     || ch->pcdata->alias_sub[ pos ] == NULL )
				break;

			fprintf( fp, "Alias %s %s~\n", ch->pcdata->alias[ pos ],
			         ch->pcdata->alias_sub[ pos ] );
		}

		if ( !IS_NULLSTR( ch->pcdata->name_deny_txt ) )
			fprintf( fp, "NameDenyTxt %s~\n", ch->pcdata->name_deny_txt );

		if ( !IS_NULLSTR( ch->pcdata->last_host ) )
			fprintf( fp, "LastHost %s~\n", ch->pcdata->last_host );

		for ( pos = 0; pos < ch->pcdata->hosts_count; pos++ )
		{
			if ( IS_NULLSTR( ch->pcdata->hosts[ pos ] ) )
				continue;

			fprintf( fp, "Host %s~\n", ch->pcdata->hosts[ pos ] );
		}

		for ( sn = 0; sn < MAX_SKILL; sn++ )
		{
			if ( skill_table[ sn ].name == NULL )
				break;

				if ( ( skill_table[ sn ].name != NULL ) && ( ( ch->pcdata->learned[ sn ] > 0 ) || ( ch->pcdata->learning[ sn ] > 0 ) || (ch->pcdata->learning_rasz[ sn ] > 0) ))
				   fprintf( fp, "Sk %d '%s' %d %d %d\n", ch->pcdata->learned[ sn ], skill_table[ sn ].name, ch->pcdata->learning[ sn ], ch->pcdata->learning_rasz[ sn ], ch->pcdata->learning_old[ sn ] );
		}


		for ( sn = 0; sn < MAX_LANG; sn++ )
			if ( ch->pcdata->language[ sn ] > 0
			     && lang_table[ sn ].bit != LANG_UNKNOWN )
				fprintf( fp, "Lang '%s' %d\n",
				         lang_table[ sn ].name,
				         ch->pcdata->language[ sn ] );

		for ( sn = 0; sn < MAX_TRICKS; sn++ )
		{
			if ( trick_table[ sn ].name == NULL )
				break;

			if ( ( trick_table[ sn ].name != NULL ) && (  ch->pcdata->tricks[ sn ] > 0 ) )
				fprintf( fp, "Trick %d\n", sn );
		}
	}

  //rellik: nowa wersja affectow z polem real time [20080704]
	for ( paf = ch->affected; paf != NULL; paf = paf->next )
	{
		if ( paf->type < 0 || paf->type >= MAX_SKILL )
			continue;

		if ( paf->type == gsn_astral_journey )
			continue;

		fprintf( fp, "Affcxxn '%s' %3d %3d %3d %3d %3d %3d %3d %3d %d %s~\n",
		         skill_table[ paf->type ].name,
		         paf->where,
		         paf->level,
		         paf->duration,
		         paf->rt_duration,
		         paf->modifier,
		         paf->location,
		         paf->bitvector ? (*paf->bitvector).bank : 0,
		         paf->bitvector ? (*paf->bitvector).vector : 0,
		         paf->visible,
		         paf->info ? paf->info : "" );
	}

	for ( pflag = ch->pflag_list; pflag; pflag = pflag->next )
	{
		if ( pflag->duration != 0 || !str_prefix( "reg", pflag->id ) )
			fprintf( fp, "Pflag %s %d~\n", pflag->id, pflag->duration );
	}
/*
	for ( list = ch->pcdata->spells_failed;list; list = list->next )
		fprintf( fp, "Spell_failed %d %d\n", list->vnum, list->spell );
*/
	if ( !IS_NPC( ch ) )
	{
		MURDER_LIST * tmp;

		fprintf( fp, "Death_stats %d %d %d %d %d\n",
		         ch->pcdata->death_statistics.deaths,
		         ch->pcdata->death_statistics.pkdeaths,
		         ch->pcdata->death_statistics.suicides,
		         ch->pcdata->death_statistics.mob_kills,
		         ch->pcdata->death_statistics.player_kills );

		for ( tmp = ch->pcdata->death_statistics.pkills_list; tmp; tmp = tmp->next )
			fprintf( fp, "Pkills2 %s~ %d %d %d %d\n",
			         tmp->name,
			         tmp->char_level,
			         tmp->victim_level,
			         tmp->room,
			         ( int ) tmp->time );

		for ( tmp = ch->pcdata->death_statistics.pkdeath_list; tmp; tmp = tmp->next )
			fprintf( fp, "Pkdeath2 %s~ %d %d %d %d\n",
			         tmp->name,
			         tmp->char_level,
			         tmp->victim_level,
			         tmp->room,
			         ( int ) tmp->time );

	}

	fprintf( fp, "RollsCount %d\n", ch->pcdata->rolls_count );
	fprintf( fp, "RollsCountNew %d\n", ch->pcdata->new_rolls_count );


//Brohacz: bounty: bounty na globalnej liscie
//	fprintf( fp, "Bounty %d\n", ch->pcdata->bounty );

	/* rellik: friend_who, zapis listy znajomych */
	if ( !IS_NPC( ch ) )
	{
		for ( wsk = ch->pcdata->friends; wsk; wsk = wsk->next )
		{
			fprintf( fp, "FRIEND1: %s~%s~%d %d\n", wsk->name, wsk->opis, (int) wsk->czas, (int) wsk->introduced );
		}
	}

	if ( !IS_NPC( ch ) )
	{
		fprintf( fp, "Age %d\n", ch->age );
		fprintf( fp, "Glory %d\n", ch->glory );
	}

	fprintf( fp, "End\n\n" );
	return ;
}

/*
 * Write an object and its contents.
 */
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest, bool pccorpse, bool remote )
{
	EXTRA_DESCR_DATA * ed;
	AFFECT_DATA *paf;
	PFLAG_DATA *pflag;

	/*
	 * Slick recursion to write lists backwards,
	 *   so loading them will load in forwards order.
	 */
	if ( !pccorpse && obj->next_content != NULL )
		fwrite_obj( ch, obj->next_content, fp, iNest, pccorpse, remote );

	/*
	 * Castrate storage characters.
	 */
	/*    if (obj->item_type == ITEM_KEY
	    ||   (obj->item_type == ITEM_MAP && !obj->value[0]))
		return;*/

	fprintf( fp, "#O\n" );
	fprintf( fp, "Vnum %d\n", obj->pIndexData->vnum );

	if ( !obj->pIndexData->new_format )
		fprintf( fp, "Oldstyle\n" );

	if ( obj->enchanted )
		fprintf( fp, "Enchanted\n" );

	fprintf( fp, "Nest %d\n", iNest	);

	/* these data are only used if they do not match the defaults */
	if ( obj->vnum_hoard != 0 )
		fprintf( fp, "Hoard %d\n", obj->vnum_hoard );

	if ( obj->name != obj->pIndexData->name )
		fprintf( fp, "Name %s~\n", obj->name	);

	if ( obj->short_descr != obj->pIndexData->short_descr )
		fprintf( fp, "ShD  %s~\n", obj->short_descr	);

	if ( obj->description != obj->pIndexData->description )
		fprintf( fp, "Desc %s~\n", obj->description	);

	if ( !ext_flags_same( obj->extra_flags, obj->pIndexData->extra_flags ) )
		fprintf( fp, "ExtFExt %s\n", print_ext_flags( obj->extra_flags ) );

	if ( obj->name2 != obj->pIndexData->name2 )
		fprintf( fp, "Nam2 %s~\n", obj->name2 );

	if ( obj->name3 != obj->pIndexData->name3 )
		fprintf( fp, "Nam3 %s~\n", obj->name3 );

	if ( obj->name4 != obj->pIndexData->name4 )
		fprintf( fp, "Nam4 %s~\n", obj->name4 );

	if ( obj->name5 != obj->pIndexData->name5 )
		fprintf( fp, "Nam5 %s~\n", obj->name5 );

	if ( obj->name6 != obj->pIndexData->name6 )
		fprintf( fp, "Nam6 %s~\n", obj->name6 );

	if ( obj->wear_flags != obj->pIndexData->wear_flags )
		fprintf( fp, "WeaF %ld\n", obj->wear_flags	);

	if ( !ext_flags_same( obj->wear_flags2, obj->pIndexData->wear_flags2 ) )
		fprintf( fp, "Wear2Ext %s\n", print_ext_flags( obj->wear_flags2 ) );

	if ( obj->item_type != obj->pIndexData->item_type )
		fprintf( fp, "Ityp %d\n", obj->item_type	);

	if ( obj->weight != obj->pIndexData->weight )
		fprintf( fp, "Wt   %d\n", obj->weight	);

	if ( obj->condition != obj->pIndexData->condition )
		fprintf( fp, "Cond %d\n", obj->condition	);

	if ( obj->material != obj->pIndexData->material )
		fprintf( fp, "Material %s~\n", material_table[ obj->material ].name );

	if ( obj->liczba_mnoga != obj->pIndexData->liczba_mnoga )
		fprintf( fp, "LiczbaMnoga %d~\n", obj->liczba_mnoga );

	if ( obj->gender != obj->pIndexData->gender )
		fprintf( fp, "Gender %d~\n", obj->gender );

  //rellik: komponenty, zapisywanie w definicji obiektu
  if ( obj->is_spell_item )
  {
  	fprintf( fp, "Komponent %d %d\n", obj->spell_item_counter, obj->spell_item_timer );
  }
  //jak ma zmieniony rent_cost w stosunku do definicji to go zapiszemy
  if ( obj->rent_cost != obj->pIndexData->rent_cost ) fprintf( fp, "Rent_cost %d\n", obj->rent_cost );

	/* variable data */

	fprintf( fp, "Wear %d\n", obj->wear_loc );
	if ( obj->timer != 0 )
		fprintf( fp, "Time %d\n", obj->timer	);

	if ( obj->trap > 0 )
		fprintf( fp, "Trap %d\n", obj->trap	);

	if ( pccorpse && obj->in_room )
		fprintf( fp, "Room %d\n", obj->in_room->vnum );

	fprintf( fp, "Cost %d\n", obj->cost	);
	if ( obj->value[ 0 ] != obj->pIndexData->value[ 0 ]
	     || obj->value[ 1 ] != obj->pIndexData->value[ 1 ]
	     || obj->value[ 2 ] != obj->pIndexData->value[ 2 ]
	     || obj->value[ 3 ] != obj->pIndexData->value[ 3 ]
	     || obj->value[ 4 ] != obj->pIndexData->value[ 4 ]
	     || obj->value[ 5 ] != obj->pIndexData->value[ 5 ]
	     || obj->value[ 6 ] != obj->pIndexData->value[ 6 ] )
		fprintf( fp, "Val  %d %d %d %d %d %d %d\n",
		         obj->value[ 0 ], obj->value[ 1 ], obj->value[ 2 ], obj->value[ 3 ],
		         obj->value[ 4 ], obj->value[ 5 ], obj->value[ 6 ] );
    /**
     * repair limits
     */
    fprintf
        (
         fp,
         "Rep %d %d %d %d\n",
         obj->repair_limit,
         obj->repair_counter,
         obj->repair_penalty,
         obj->repair_condition
        );

    switch ( obj->item_type )
	{
		case ITEM_POTION:
		case ITEM_SCROLL:
		case ITEM_PILL:
			if ( obj->value[ 1 ] > 0 )
			{
				fprintf( fp, "Spell 1 '%s'\n",
				         skill_table[ obj->value[ 1 ] ].name );
			}

			if ( obj->value[ 2 ] > 0 )
			{
				fprintf( fp, "Spell 2 '%s'\n",
				         skill_table[ obj->value[ 2 ] ].name );
			}

			if ( obj->value[ 3 ] > 0 )
			{
				fprintf( fp, "Spell 3 '%s'\n",
				         skill_table[ obj->value[ 3 ] ].name );
			}

			break;

		case ITEM_STAFF:
		case ITEM_WAND:
			if ( obj->value[ 3 ] > 0 )
			{
				fprintf( fp, "Spell 3 '%s'\n",
				         skill_table[ obj->value[ 3 ] ].name );
			}
			break;
	}

	fprintf( fp, "Length %d\n", obj->length);

	//rellik: nowa wersja affectów z polem real time [20080704]
	for ( paf = obj->affected; paf != NULL; paf = paf->next )
	{
		if ( paf->type < 0 || paf->type >= MAX_SKILL )
			continue;
		fprintf( fp, "Affcn '%s' %3d %3d %3d %3d %3d %3d %3d %3d\n",
		         skill_table[ paf->type ].name,
		         paf->where,
		         paf->level,
		         paf->duration,
		         paf->rt_duration,
		         paf->modifier,
		         paf->location,
		         paf->bitvector ? (*paf->bitvector).bank : 0,
		         paf->bitvector ? (*paf->bitvector).vector : 0
		       );
	}

	for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
	{
		fprintf( fp, "ExDe %s~ %s~\n",
		         ed->keyword, ed->description );
	}

	for ( pflag = obj->pflag_list; pflag; pflag = pflag->next )
	{
		if ( pflag->duration != 0 || !str_prefix( "reg", pflag->id ) )
			fprintf( fp, "Pflag %s %d~\n", pflag->id, pflag->duration );
	}

	fprintf( fp, "End\n\n" );

	if ( obj->contains != NULL )
		fwrite_obj( ch, obj->contains, fp, iNest + 1, FALSE, remote );

	return ;
}

CHAR_DATA* load_char_remote( char *name )
{
	DESCRIPTOR_DATA * tdesc;
	CHAR_DATA * cret = NULL;
	tdesc = new_descriptor();

	if ( load_char_obj( tdesc, name, TRUE ) )
	{
		cret = tdesc->character;
		cret->desc = NULL;
		tdesc->character = NULL;
	}
	free_descriptor( tdesc );
	return cret;
}

/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj( DESCRIPTOR_DATA *d, char *name, bool remote )
{
	char strsave[ MAX_INPUT_LENGTH ];
	//char buf[ 100 ];
	CHAR_DATA *ch;
	FILE *fp;
	bool found;
	bool rent = TRUE;
	int free_rent = 0;

	ch = init_char( name );
	d->character = ch;
	ch->desc = d;

	found = FALSE;
	fclose( fpReserve );

	sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( name ) );
	if ( ( fp = fopen( strsave, "r" ) ) != NULL )
	{
		int iNest;

		for ( iNest = 0; iNest < MAX_NEST; iNest++ )
			rgObjNest[ iNest ] = NULL;

		found = TRUE;
		for ( ; ; )
		{
			char letter;
			char *word;

			letter = fread_letter( fp );
			if ( letter == '*' )
			{
				fread_to_eol( fp );
				continue;
			}

			if ( letter != '#' )
			{
				bug( "Load_char_obj: # not found.", 0 );
				break;
			}

			word = fread_word( fp );
			if ( !str_cmp( word, "PLAYER" ) ) rent = fread_char ( ch, fp, remote );
			else if ( !str_cmp( word, "OBJECT" ) ) fread_obj ( ch, fp, FALSE, remote );
			else if ( !str_cmp( word, "O" ) ) fread_obj ( ch, fp, FALSE, remote );
			else if ( !str_cmp( word, "END" ) ) break;
			else
			{
				bug( "Load_char_obj: bad section.", 0 );
				break;
			}
		}
		fclose( fp );

		remove_not_known( ch );

		if ( !rent )
		{
			OBJ_DATA * obj, *obj_next;
			obj = ch->carrying;
			/*artefact*/
			remove_char_from_artefact_list( ch );

			for ( ; obj != NULL; obj = obj_next )
			{
				OBJ_NEXT_CONTENT( obj, obj_next );
				/*artefact*/
				if ( is_artefact( obj ) ) extract_artefact( obj );
				if ( obj->contains ) extract_artefact_container( obj );
				if ( ( IS_OBJ_STAT( obj, ITEM_NEWBIE_FREE_RENT ) && ch->level <= 4 ) || IS_OBJ_STAT( obj, ITEM_FREE_RENT ) )
					++free_rent;
				else
				{
					extract_obj( obj );
					free_obj( obj );
				}
			}
			//Raszer - dont forget about hoarded items
			for ( obj = ch->hoard; obj != NULL; obj = obj->next_content ) {
			    if ( is_artefact( obj ) ) extract_artefact( obj );
			    if ( obj->contains ) extract_artefact_container( obj );
                extract_obj( obj );
				free_obj( obj );
			}
			ch->hoard = NULL;

            money_reset_character_money( ch );
			ch->bank = 0;
			if ( !free_rent ) ch->carrying = NULL;
		}
	}

	fpReserve = fopen( NULL_FILE, "r" );

    if ( !found )
    {
        return found;
    }

    /* initialize race */
    /*int i;*/

    if ( GET_RACE( ch ) == 0 )
    {
        SET_RACE( ch, race_lookup( "human" ) );
    }

    ch->size = pc_race_table[ GET_REAL_RACE( ch ) ].size;
    ch->dam_type = 17; /*punch */

    ext_flags_copy( ext_flags_sum( ch->affected_by, ext_flag_value( affect_flags, race_table[ GET_RACE( ch ) ].aff ) ), ch->affected_by );
    ch->form	= race_table[ GET_RACE( ch ) ].form;
    ch->parts	= race_table[ GET_RACE( ch ) ].parts;

    if ( ch->version < 6 )
    {
        ch->wiz_config = WIZCFG_SEXCONV | WIZCFG_COLOURCONV;
    }

    /* fix levels */
    if ( ch->version < 3 && ( ch->level > 35 || ch->trust > 35 ) )
    {
        switch ( ch->level )
        {
            case( 40 ) : ch->level = 60;	break;  /* imp -> imp */
            case( 39 ) : ch->level = 58; break;	/* god -> supreme */
            case( 38 ) : ch->level = 56; break;	/* deity -> god */
            case( 37 ) : ch->level = 53; break;	/* angel -> demigod */
        }

        switch ( ch->trust )
        {
            case( 40 ) : ch->trust = 60; break;	/* imp -> imp */
            case( 39 ) : ch->trust = 58; break;	/* god -> supreme */
            case( 38 ) : ch->trust = 56; break;	/* deity -> god */
            case( 37 ) : ch->trust = 53; break;	/* angel -> demigod */
            case( 36 ) : ch->trust = 51; break;	/* hero -> hero */
        }
    }

    /* ream gold */

    return found;
}



/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

/* provided to free strings */
#if defined(KEYS)
#undef KEYS
#endif

#define KEYS( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    free_string(field);			\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

bool fread_char( CHAR_DATA *ch, FILE *fp, bool remote )
{
	MSPELL_DATA * mspell;
	MEMSET_DATA * memset;
	char memset_eol;
	bool test = TRUE; /*do renta*/

	char buf[ MAX_STRING_LENGTH ];
	char *word;
	bool fMatch;
	int count = 0;
	int percent;
	char *tmp;
	char *tmp1;
	time_t czas;
	int i;
	bool introduced;
	int bounty_val = 0; //Brohacz: bounty


	if ( !remote )
	{
		sprintf( buf, "Loading %s.", ch->name );
		log_string( buf );
	}

	for ( ; ; )
	{
		word = feof( fp ) ? "End" : fread_word( fp );
		fMatch = FALSE;

		switch ( UPPER( word[ 0 ] ) )
		{
			case '*':
				fMatch = TRUE;
				fread_to_eol( fp );
				break;

			case 'A':
				if ( !str_cmp( word, "Act" ) )
				{
					fread_to_eol( fp );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "ActExt" ) )
				{
					fread_ext_flags( fp, ch->act );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "Account" ) )
				{
					char * account;
					account = fread_string( fp );

#ifdef NEW_NANNY
					if ( ch->desc )
						load_account( &ch->desc->account, account );

					free_string( ch->pcdata->account_email );
					ch->pcdata->account_email = str_dup( account );
#endif

					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "Affvec" ) )
				{
					int vec, bank;

					bank = fread_number( fp );
					vec = fread_flag( fp );

					if ( bank != AFF_ZAKUTY.bank || vec != AFF_ZAKUTY.vector )
						ch->affected_by[ bank ] = vec;

					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "Affected" ) )
				{
					fread_ext_flags( fp, ch->affected_by );
					//od rasy
					ext_flags_copy( ext_flags_sum( ch->affected_by, ext_flag_value( affect_flags, race_table[ GET_RACE( ch ) ].aff ) ), ch->affected_by );

					fMatch = TRUE;
					break;
				}

				KEY( "Alig", ch->alignment, fread_number( fp ) );

				if ( !str_cmp( word, "Alia" ) )
				{
					if ( count >= MAX_ALIAS )
					{
						fread_to_eol( fp );
						fMatch = TRUE;
						break;
					}

					ch->pcdata->alias[ count ] = str_dup( fread_word( fp ) );
					ch->pcdata->alias_sub[ count ] = str_dup( fread_word( fp ) );
					count++;
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "Alias" ) )
				{
					if ( count >= MAX_ALIAS )
					{
						fread_to_eol( fp );
						fMatch = TRUE;
						break;
					}

					ch->pcdata->alias[ count ] = str_dup( fread_word( fp ) );
					ch->pcdata->alias_sub[ count ] = fread_string( fp );
					count++;
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "AC" ) || !str_cmp( word, "Armor" ) )
				{
					fread_to_eol( fp );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "ACs" ) )
				{
					int i;

					for ( i = 0; i < 4; i++ )
						ch->armor[ i ] = fread_number( fp );
					fMatch = TRUE;
					break;
				}

				//rellik: odczyt affectów w starej wersji [20080704]
				if ( !str_cmp( word, "Affcxx" ) )
				{
					AFFECT_DATA * paf;
					int sn;
					char *info;
					int bank, vector;

					paf = new_affect();

					sn = skill_lookup( fread_word( fp ) );
					if ( sn < 0 )
						bug( "Fread_char: unknown skill.", 0 );
					else
						paf->type = sn;

					paf->where = fread_number( fp );
					paf->level = fread_number( fp );
					paf->duration = fread_number( fp );
					paf->modifier = fread_number( fp );
					paf->location = fread_number( fp );
					bank = fread_number( fp );
					vector =  fread_number( fp );
					paf->bitvector = vector_to_pointer( affect_flags, bank, vector );
					paf->visible = fread_number( fp );
					info = fread_string( fp );

					if ( info[ 0 ] != '\0' )
						paf->info	= info;
					else
						paf->info	= NULL;

					if (sn == gsn_noob_killer)
						paf->visible = FALSE;

					paf->next = ch->affected;
					ch->affected = paf;
					fMatch = TRUE;
					break;
				}

				//rellik: odczyt affectow w nowej wersji [20080704]
				if ( !str_cmp( word, "Affcxxn" ) )
				{
					AFFECT_DATA * paf;
					int sn;
					char *info;
					int bank, vector;

					paf = new_affect();

					sn = skill_lookup( fread_word( fp ) );
					if ( sn < 0 )
						bug( "Fread_char: unknown skill.", 0 );
					else
						paf->type = sn;

					paf->where = fread_number( fp );
					paf->level = fread_number( fp );
					paf->duration = fread_number( fp );
					paf->rt_duration = fread_number( fp );
					paf->modifier = fread_number( fp );
					paf->location = fread_number( fp );
					bank = fread_number( fp );
					vector =  fread_number( fp );
					paf->bitvector = vector_to_pointer( affect_flags, bank, vector );
					paf->visible = fread_number( fp );
					info = fread_string( fp );

					paf->rt_duration = paf->rt_duration - ( ( current_time - ch->pcdata->last_logoff ) / 60 );
					if ( paf->rt_duration < 0 ) paf->rt_duration = 0;

					if ( info[ 0 ] != '\0' )
						paf->info	= info;
					else
						paf->info	= NULL;

					if (sn == gsn_noob_killer)
						paf->visible = FALSE;

					paf->next = ch->affected;
					ch->affected = paf;
					fMatch = TRUE;
					break;
				}

				KEY( "Age", ch->age, fread_number( fp ) );
				KEYS( "Afktext", ch->pcdata->afk_text, fread_string( fp ) );

				if ( !str_cmp( word, "ANewStat" ) )
				{
					int stat;

					for ( stat = 0; stat < MAX_STATS; stat++ )
						ch->perm_stat[ stat ] = fread_number(fp);


					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "AModNewStat" ) )
				{
					int stat;

					for ( stat = 0; stat < MAX_STATS; stat ++ )
						ch->new_mod_stat[ stat ] = fread_number( fp );

					fMatch = TRUE;
					break;
				}
				break;

			case 'B':
				KEYS( "Bin", ch->pcdata->bamfin, fread_string( fp ) );
				KEYS( "Bout", ch->pcdata->bamfout, fread_string( fp ) );
				KEY( "BSilv", ch->bank, RATTING_SILVER * fread_number( fp ) ); /* deprecated */
				if ( !str_cmp( word, "Bounty" ) )
				{
					bounty_val = fread_number( fp );

					fMatch = TRUE;
					break;
				}

				break;

			case 'C':
				KEY( "Class", ch->class, fread_number( fp ) );
				KEY( "Comm", ch->comm, fread_flag( fp ) );

				if ( !str_cmp( word, "Condition" ) || !str_cmp( word, "Cond" ) )
				{
					ch->condition[ 0 ] = fread_number( fp );
					ch->condition[ 1 ] = fread_number( fp );
					ch->condition[ 2 ] = fread_number( fp );
					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "Cnd" ) )
				{
					ch->condition[ 0 ] = fread_number( fp );
					ch->condition[ 1 ] = fread_number( fp );
					ch->condition[ 2 ] = fread_number( fp );
					ch->condition[ 3 ] = fread_number( fp );
					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "Cnd_NEW" ) )
				{
					ch->condition[ 0 ] = fread_number( fp );
					ch->condition[ 1 ] = fread_number( fp );
					ch->condition[ 2 ] = fread_number( fp );
					ch->condition[ 3 ] = fread_number( fp );
					ch->condition[ 4 ] = fread_number( fp );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "Coloura" ) )
				{
					LOAD_COLOUR( text )
					LOAD_COLOUR( auction )
					LOAD_COLOUR( gossip )
					LOAD_COLOUR( music )
					LOAD_COLOUR( question )
					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "Colourb" ) )
				{
					LOAD_COLOUR( answer )
					LOAD_COLOUR( quote )
					LOAD_COLOUR( quote_text )
					LOAD_COLOUR( immtalk_text )
					LOAD_COLOUR( immtalk_type )
					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "Colourc" ) )
				{
					LOAD_COLOUR( info )
					LOAD_COLOUR( tell )
					LOAD_COLOUR( reply )
					LOAD_COLOUR( gtell_text )
					LOAD_COLOUR( gtell_type )
					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "Colourd" ) )
				{
					LOAD_COLOUR( room_title )
					LOAD_COLOUR( room_text )
					LOAD_COLOUR( room_exits )
					LOAD_COLOUR( room_things )
					LOAD_COLOUR( prompt )
					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "Coloure" ) )
				{
					LOAD_COLOUR( fight_death )
					LOAD_COLOUR( fight_yhit )
					LOAD_COLOUR( fight_ohit )
					LOAD_COLOUR( fight_thit )
					LOAD_COLOUR( fight_skill )
					LOAD_COLOUR( fight_trick )
					LOAD_COLOUR( fight_spell )
					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "Colourf" ) )
				{
					LOAD_COLOUR( wiznet )
					LOAD_COLOUR( say )
					LOAD_COLOUR( say_text )
					LOAD_COLOUR( tell_text )
					LOAD_COLOUR( reply_text )
					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "Colourg" ) )
				{
					LOAD_COLOUR( auction_text )
					LOAD_COLOUR( gossip_text )
					LOAD_COLOUR( music_text )
					LOAD_COLOUR( question_text )
					LOAD_COLOUR( answer_text )
					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "Colourh" ) )
				{
					LOAD_COLOUR( shout )
					LOAD_COLOUR( shout_text )
					LOAD_COLOUR( yell )
					LOAD_COLOUR( yell_text )
					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "Counter" ) )
				{
					int i;

					for ( i = 0;i < MAX_COUNTER; i++ )
						ch->counter[ i ] = 0;

					for ( i = 0;i < 4; i++ )
						ch->counter[ i ] = fread_number( fp );

					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "Counter2" ) )
				{
					int i, max;

					max = 4 + fread_number( fp );

					for ( i = 4;i < max; i++ )
						ch->counter[ i ] = fread_number( fp );

					fMatch = TRUE;
					break;
				}

				break;

			case 'D':
				KEY( "Dam", ch->damroll, fread_number( fp ) );
				KEYS( "Desc", ch->description, fread_string( fp ) );
				if ( !str_cmp( word, "Death_stats" ) )
				{
					ch->pcdata->death_statistics.deaths = fread_number( fp );
					ch->pcdata->death_statistics.pkdeaths = fread_number( fp );
					ch->pcdata->death_statistics.suicides = fread_number( fp );
					ch->pcdata->death_statistics.mob_kills = fread_number( fp );
					ch->pcdata->death_statistics.player_kills = fread_number( fp );

					ch->pcdata->death_statistics.deaths = UMAX( 0, ch->pcdata->death_statistics.deaths );
					ch->pcdata->death_statistics.pkdeaths = UMAX( 0, ch->pcdata->death_statistics.pkdeaths );
					ch->pcdata->death_statistics.suicides = UMAX( 0, ch->pcdata->death_statistics.suicides );
					ch->pcdata->death_statistics.mob_kills = UMAX( 0, ch->pcdata->death_statistics.mob_kills );
					ch->pcdata->death_statistics.player_kills = UMAX( 0, ch->pcdata->death_statistics.player_kills );

					fMatch = TRUE;
					break;
				}
				break;

			case 'E':
				if ( !str_cmp( word, "End" ) )
				{
					CLAN_DATA * clan;
					CLAN_MEMBER * member;
					int cost;
					int period_d;
					int period_h;
					int period_m;

					clan = get_clan_by_member( ch->name );
					if ( clan && clan->active )
					{
						member = get_member( clan, ch->name );
						if ( member )
						{
							ch->pcdata->clan = clan;
							ch->pcdata->clan_rank = member->rank;
							if ( !remote && current_time > clan->last_entered )
								clan->last_entered = current_time;
						}
					}

                    if ( bounty_val >= 500 )
                    {
                        add_bounty( &misc.bounty_list, ch->name, bounty_val );
                    }
                    if ( remote )
                    {
                        return TRUE;
                    }

					cost = ch->pcdata->last_rent_cost;

					if ( cost < 0 )
                    {
						cost = 0;
                    }

					period_d = ( current_time - ch->pcdata->last_logoff ) / ( 24 * 60 * 60 );
					period_h = ( current_time - ch->pcdata->last_logoff ) / ( 60 * 60 );
					period_m = ( current_time - ch->pcdata->last_logoff ) / 60;

					if ( period_d >= 1 )
						cost = UMAX( 0, cost * period_d );
					else if ( period_h >= 1 )
						cost = UMAX( 0, ( cost * period_h ) / 24 );
					else if ( period_m >= 1 )
						cost = UMAX( 0, ( cost * period_m ) / ( 24 * 60 ) );
					else cost = 1;

					if ( IS_IMMORTAL( ch ) || ( ch->desc && ch->desc->connected == CON_COPYOVER_RECOVER ) )
					{
						test = TRUE;
					}
					else
					{
                        long int copper = money_count_copper_all ( ch );
						if ( copper < cost )
						{
							test = FALSE;
						}
						else
						{
							int prev_copper = money_count_copper( ch );
							int prev_bank = ch->bank;
							if ( prev_copper < cost )
							{
                                money_reset_character_money( ch );
								ch->bank += prev_copper;
								ch->bank -= cost;
							}
							else
                            {
                                money_reduce ( ch, cost );
                            }

                            copper = money_count_copper( ch );
							if ( copper - prev_copper != 0 || ch->bank - prev_bank != 0 )
                            {
                                append_file_format_daily
                                    (
                                     ch,
                                     MONEY_LOG_FILE,
                                     "-> S: %d %d (%d), B: %d %d (%d) - op³ata za rent",
                                     prev_copper,
                                     copper,
                                     copper - prev_copper,
                                     prev_bank,
                                     ch->bank,
                                     ch->bank - prev_bank
                                    );
                            }
						}
					}
					/* ustawiamy sleepy na fulla jesli dluzej go nie bylo niz 30 minut*/
					if ( current_time - ch->pcdata->last_logoff > 30 * 60 )
						ch->condition[ COND_SLEEPY ] = 48;

					percent = ( current_time - ch->pcdata->last_logoff ) * 25 / ( 2 * 60 * 60 );

					percent = UMIN( percent, 100 );

					if ( percent > 0 && !IS_AFFECTED( ch, AFF_POISON )
					     && !IS_AFFECTED( ch, AFF_PLAGUE ) )
					{
						ch->hit	+= ( get_max_hp( ch ) - ch->hit ) * percent / 100;
						ch->move += ( ch->max_move - ch->move ) * percent / 100;
					}
					//TODO (rellik#) wy³±czyæ za jaki¶ czas, teraz po to by w³±czyæ postaciom newbie które ju¿ s±
					/*
					if ( ch->level <= LEVEL_NEWBIE )
						SET_BIT( ch->comm, COMM_NEWBIE );
					*/

					return test;
				}
				KEY( "Exp", ch->exp, fread_number( fp ) );
				break;

			case 'F':
				/* rellik: friend_who, odczytywanie */
				//TODO: (rellik#) za jaki¶ czas wy³±czyæ bo jest FRIEND1
				if ( !str_cmp( word, "FRIEND:" ) )
				{
					tmp = fread_string( fp );
					tmp1 = fread_string( fp );
					czas = fread_number( fp );
					if ( czas == 0 ) czas = current_time;
					if ( ( ( current_time - czas ) / 3600 ) <= ( TIME_TO_FORGET_FRIEND * 24 ) )
					{
						friend_dodaj( ch, tmp, czas, TRUE );
						friend_note( ch, tmp, tmp1, TRUE );
					}
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "FRIEND1:" ) )
				{
					tmp = fread_string( fp );
					tmp1 = fread_string( fp );
					czas = fread_number( fp );
					introduced = (bool) fread_number( fp );
					if ( czas == 0 ) czas = current_time;
					if ( ( ( current_time - czas ) / 3600 ) <= ( TIME_TO_FORGET_FRIEND * 24 ) )
					{
						friend_dodaj( ch, tmp, czas, introduced );
						friend_note( ch, tmp, tmp1, TRUE );
					}
					fMatch = TRUE;
					break;
				}

				break;

			case 'G':
				KEY( "Glory", ch->glory, fread_number( fp ) );
				break;
				/*            if ( !str_cmp( word, "Group" )  || !str_cmp(word,"Gr"))
				            {
				                int gn;
				                char *temp;

				                temp = fread_word( fp ) ;
				                gn = group_lookup(temp);
				                if ( gn < 0 )
				                {
				                    fprintf(stderr,"%s",temp);
				                    bug( "Fread_char: unknown group. ", 0 );
				                }
				                else
						    gn_add(ch,gn);
				                fMatch = TRUE;
				            }
					    break;*/

			case 'H':
				KEY( "Hitroll", ch->hitroll, fread_number( fp ) );
                KEY( "Hit", ch->hitroll, fread_number( fp ) );

                /* BEGIN: reward (autoquest) */
                KEY( "HuntTime", ch->pcdata->hunt_time, fread_number( fp ) );
                /* END: reward (autoquest) */

				if ( !str_cmp( word, "HpManaMove" ) || !str_cmp( word, "HMV" ) )
				{
					ch->hit	= fread_number( fp );
					ch->max_hit	= fread_number( fp );
					fread_number( fp );
					fread_number( fp );
					ch->move	= fread_number( fp );
					ch->max_move	= fread_number( fp );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "HitGains" ) )
				{
					int max_level;
					int x;

					max_level	= fread_number( fp );
					for ( x = 0; x < max_level; x++ )
						ch->pcdata->perm_hit_per_level[ x ]	= fread_number( fp );

					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "HV" ) )
				{
					ch->hit	= fread_number( fp );
					ch->max_hit	= fread_number( fp );
					ch->move	= fread_number( fp );
					ch->max_move	= fread_number( fp );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "HpManaMovePerm" ) || !str_cmp( word, "HMVP" ) )
				{
					ch->pcdata->perm_hit	= fread_number( fp );
					fread_number( fp );
					ch->pcdata->perm_move = fread_number( fp );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "HW" ) )
				{
					ch->height	= fread_number( fp );
					ch->weight	= fread_number( fp );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "Host" ) )
				{
					add_host( ch, fread_string( fp ), FALSE );
					fMatch = TRUE;
					break;
				}
				break;

			case 'I':
				KEY( "Id", ch->id, fread_number( fp ) );
				KEY( "InvisLevel", ch->invis_level, fread_number( fp ) );
				KEY( "Inco", ch->incog_level, fread_number( fp ) );
				KEY( "Invi", ch->invis_level, fread_number( fp ) );
				KEYS( "Ignore", ch->pcdata->ignore, fread_string( fp ) );
				break;

			case 'L':

				if ( !str_cmp( word, "Lang" ) )
				{
					int sn;
					int value;
					char *temp;

					temp = fread_word( fp ) ;
					value = fread_number( fp );

					sn = get_langnum( temp );

					if ( sn < 0 )
					{
						fprintf( stderr, "%s", temp );
						bug( "Fread_char: unknown lang. ", 0 );
					}

					ch->pcdata->language[ sn ] = value;

					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "Lang_mod" ) )
				{
					int sn;
					int value;
					char *temp;

					temp = fread_word( fp ) ;
					value = fread_number( fp );

					sn = get_langnum( temp );

					if ( sn < 0 )
					{
						fprintf( stderr, "%s", temp );
						bug( "Fread_char: unknown lang. ", 0 );
					}

					ch->pcdata->lang_mod[ sn ] = value;

					fMatch = TRUE;
					break;
				}



				KEY( "LLev", ch->pcdata->last_level, fread_number( fp ) );
				KEY( "Levl", ch->level, fread_number( fp ) );
				KEY( "LogO", ch->pcdata->last_logoff, fread_number( fp ) );
				KEYS( "LnD", ch->long_descr, fread_string( fp ) );
				KEYS( "LastHost", ch->pcdata->last_host, fread_string( fp ) );

				break;

			case 'N':
				KEYS( "Name", ch->name, fread_string( fp ) );
				KEYS( "NameDenyTxt", ch->pcdata->name_deny_txt, fread_string( fp ) );
				if ( !str_cmp( word, "NewTitl" ) )
				{
					ch->pcdata->new_title = fread_string( fp );
					fMatch = TRUE;
					break;
				}
				break;

			case 'M':

				if ( !str_cmp( word, "Mem" ) )
				{
					mspell = new_mspell();
					mspell->spell = fread_number( fp );
					mspell->circle = fread_number( fp );
					mspell->done = fread_number( fp );
					load_mem( ch, mspell );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "MemSet" ) )
				{
					memset = new_memset();
					memset->set_name = str_dup(fread_word( fp ));

					memset_eol = getc( fp );

					while ( memset_eol != '\n' && memset_eol != '\r' ) {
					    ungetc( memset_eol, fp );
					    i = fread_number( fp );
					    memset->set_spells[ i ] = fread_number( fp );
					    memset_eol = getc( fp );
					}

                    memset->next = ch->memset;
                    ch->memset   = memset;
                    fMatch = TRUE;
                    break;
				}

				if ( !str_cmp( word, "Memmods" ) )
				{
					int x;

					for ( x = 0;x < 9;x++ )
                    {
                        ch->mem_mods[ x ] = fread_number( fp );
                    }

					fMatch = TRUE;
					break;
				}

                if ( !str_cmp( word, "Money" ) )
                {
                    ch->bank    = fread_number( fp );
                    ch->copper  = fread_number( fp );
                    ch->silver   = fread_number( fp );
                    ch->gold    = fread_number( fp );
                    ch->mithril = fread_number( fp );
                    fMatch = TRUE;
                    break;
				}

				break;

			case 'O':
				if ( !str_cmp( word, "Odmiana" ) )
				{
					ch->name2 = fread_string( fp );
					ch->name3 = fread_string( fp );
					ch->name4 = fread_string( fp );
					ch->name5 = fread_string( fp );
					ch->name6 = fread_string( fp );
					fMatch = TRUE;
					break;
				}
				break;

			case 'P':
				KEYS( "Pass", ch->pcdata->pwd, fread_string( fp ) );
				KEY( "Plyd", ch->played, fread_number( fp ) );
				KEY( "Pnts", ch->pcdata->points, fread_number( fp ) );
				KEY( "Pos", ch->position, fread_number( fp ) );

				KEYS( "Prompt", ch->prompt, fread_string( fp ) );
				KEYS( "Prom", ch->prompt, fread_string( fp ) );
				if ( !str_cmp( word, "Pflag" ) )
				{
					setcharflag( ch, fread_string( fp ) );
					fMatch = TRUE;
					break;
				}
				else if ( !str_cmp( word, "Pkdeath" ) )
				{
					MURDER_LIST * tmp;
					char * name = str_dup( "NULL" );
					int time = 0 ;
					unsigned short room = 0;

					name = fread_string( fp );
					room = fread_number( fp );
					time = fread_number( fp );

					if ( ( int ) current_time - time > ( 3600 * 24 * 5 ) )
					{
						fMatch = TRUE;
						break;
					}

					tmp = malloc( sizeof( *tmp ) );
					tmp->name = name;
					tmp->time = time;
					tmp->room = room;
					tmp->char_level = 0;
					tmp->victim_level = 0;

					tmp->next = ch->pcdata->death_statistics.pkdeath_list;
					ch->pcdata->death_statistics.pkdeath_list = tmp;

					fMatch = TRUE;
					break;
				}
				else if ( !str_cmp( word, "Pkills" ) )
				{
					MURDER_LIST * tmp;
					char * name = str_dup( "NULL" );
					int time = 0;
					unsigned short room = 0;

					name = fread_string( fp );
					room = fread_number( fp );
					time = fread_number( fp );

					if ( ( int ) current_time - time > ( 3600 * 24 * 5 ) )
					{
						fMatch = TRUE;
						break;
					}

					tmp = malloc( sizeof( *tmp ) );
					tmp->name = name;
					tmp->time = time;
					tmp->room = room;
					tmp->char_level = 0;
					tmp->victim_level = 0;

					tmp->next = ch->pcdata->death_statistics.pkills_list;
					ch->pcdata->death_statistics.pkills_list = tmp;

					fMatch = TRUE;
					break;
				}
				else if ( !str_cmp( word, "Pkdeath2" ) )
				{
					MURDER_LIST * tmp;
					char * name = str_dup( "NULL" );
					int time = 0, cl, vl;
					unsigned short room = 0;

					name = fread_string( fp );
					cl = fread_number( fp );
					vl = fread_number( fp );
					room = fread_number( fp );
					time = fread_number( fp );

					if ( ( int ) current_time - time > ( 3600 * 24 * 5 ) )
					{
						fMatch = TRUE;
						break;
					}

					tmp = malloc( sizeof( *tmp ) );
					tmp->name = name;
					tmp->time = time;
					tmp->room = room;
					tmp->char_level = cl;
					tmp->victim_level = vl;

					tmp->next = ch->pcdata->death_statistics.pkdeath_list;
					ch->pcdata->death_statistics.pkdeath_list = tmp;

					fMatch = TRUE;
					break;
				}
				else if ( !str_cmp( word, "Pkills2" ) )
				{
					MURDER_LIST * tmp;
					char * name = str_dup( "NULL" );
					int time = 0, cl, vl;
					unsigned short room = 0;

					name = fread_string( fp );
					cl = fread_number( fp );
					vl = fread_number( fp );
					room = fread_number( fp );
					time = fread_number( fp );

					if ( ( int ) current_time - time > ( 3600 * 24 * 5 ) )
					{
						fMatch = TRUE;
						break;
					}

					tmp = malloc( sizeof( *tmp ) );
					tmp->name = name;
					tmp->time = time;
					tmp->room = room;
					tmp->char_level = cl;
					tmp->victim_level = vl;

					tmp->next = ch->pcdata->death_statistics.pkills_list;
					ch->pcdata->death_statistics.pkills_list = tmp;

					fMatch = TRUE;
					break;
				}

				break;

			case 'R':
				KEY( "Rent", ch->pcdata->last_rent_cost, fread_number( fp ) );
				KEY( "RollsCount", ch->pcdata->rolls_count, fread_number( fp ) );
                KEY( "RollsCountNew", ch->pcdata->new_rolls_count, fread_number( fp ) );

                /* BEGIN: reward (autoquest) */
                KEY( "Recovery",    ch->pcdata->recovery,    fread_number( fp ) );
                KEY( "Rewarder",    ch->pcdata->rewarder,    fread_number( fp ) );
                KEY( "Rewards",        ch->pcdata->rewards,    fread_number( fp ) );
                KEY( "RewardMob",    ch->pcdata->reward_mob,    fread_number( fp ) );
                KEY( "RewardObj",    ch->pcdata->reward_obj,    fread_number( fp ) );
                /* END: reward (autoquest) */


				if ( !str_cmp( word, "Race" ) )
				{
					SET_RACE( ch, race_lookup( fread_string( fp ) ) );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "Room" ) )
				{
					ROOM_INDEX_DATA * pRoom;

					pRoom = get_room_index( fread_number( fp ) );

					if ( pRoom == NULL )
						pRoom = get_room_index( ROOM_VNUM_LIMBO );

					if ( !IS_IMMORTAL( ch ) && EXT_IS_SET( pRoom->room_flags, ROOM_INN ) )
						ch->pcdata->last_rent = pRoom->vnum;
					else if ( IS_IMMORTAL( ch ) )
						ch->pcdata->last_rent = pRoom->vnum;
					fMatch = TRUE;
					break;
				}

				break;

			case 'S':
				if ( !str_cmp( word, "Saving" ) )
				{
					int x;

					for ( x = 0;x < 5;x++ )
						ch->saving_throw[ x ] = fread_number( fp );

					fMatch = TRUE;
					break;
				}

				//rellik: komponenty, odczytywanie wiedzy o komponentach
				if ( !str_cmp( word, "SIknows" ) )
				{
						int i;
						int nb_bank = BIGFLAG_CAPACITY;

						for( i = 0; i < nb_bank; ++i )
						{
							ch->pcdata->spell_items_knowledge.bank[i] = fread_number( fp );
						}
						fMatch = TRUE;
						break;
				}

				KEY( "Scro", ch->lines, fread_number( fp ) );
				if ( ch->lines == 0 ) ch->lines = 30;
				if ( ch->lines > 60 ) ch->lines = 60;
				KEY( "Sex", ch->sex, fread_number( fp ) );
				KEYS( "ShD", ch->short_descr, fread_string( fp ) );
				KEY( "Sec", ch->pcdata->security, fread_number( fp ) );	/* OLC */
				KEY( "Silv", ch->silver, fread_number( fp ) ); /* deprecated */
				KEY( "Speaking", ch->speaking, fread_number( fp ) );

				if ( !str_cmp( word, "Skill" ) || !str_cmp( word, "Sk" ) )
				{
					int sn;
					int value;
					char *temp;

					value = fread_number( fp );
					temp = fread_word( fp ) ;
					sn = skill_lookup( temp );
					/* sn    = skill_lookup( fread_word( fp ) ); */
					if ( sn < 0 )
					{
						fprintf( stderr, "%s", temp );
						bug( "Fread_char: unknown skill. ", 0 );
					}
					ch->pcdata->learning[ sn ] = fread_number( fp );
					if ( ch->pcdata->learning[ sn ] < 0 )
						log_string( "fread_char: bug z learning" );
					ch->pcdata->learning[ sn ] = UMAX( 0, ch->pcdata->learning[ sn ] );
                    //nie wiemy do konca jak wtracic swoje pare groszy bez naruszania dalszego ciagu pliku, wiec zaczynamy oszukiwac!
                    //jesli koles ma inna ilosc starych plusów ni¿ 123, to znaczy ze to jest stara postac, bo nowa ma stare plusy ustawiane na 123 przy nauce umiejki!
                    //ustawiamy mu wiec stare plusy na 123 i dajemy do pamieci learning_rasz 0!
                    //jak bedziemy chcieli wrocic do starego systemu, to bedzie trza wyzerowac stare plusy
                    //stare plusy przenosimy do learning old

                    if(ch->pcdata->learning[ sn ] != 123){
                                             ch->pcdata->learning_old[ sn ] =  ch->pcdata->learning[ sn ];
                                             ch->pcdata->learning[ sn ] = 123;
                                             ch->pcdata->learning_rasz[ sn ] = 0;
                    }else
                    {
                    // liczba starych plusow wynosi 123, czyli jedziemy nowym systemem
					ch->pcdata->learning_rasz[ sn ] = fread_number( fp );
					if ( ch->pcdata->learning_rasz[ sn ] < 0 )
						log_string( "fread_char: bug z learning_rasz" );
					ch->pcdata->learning_old[ sn ] = fread_number( fp );
					if ( ch->pcdata->learning_old[ sn ] < 0 )
						log_string( "fread_char: bug z learning_old" );
                    }

					if ( ch->pcdata->learning_rasz[ sn ] > 13)//maks nowych plusow to 12
                    {
                          ch->pcdata->learning_rasz[ sn ] = 0;
                    };

					ch->pcdata->learning_rasz[ sn ] = UMAX( 0, ch->pcdata->learning_rasz[ sn ] );
					ch->pcdata->learned[ sn ] = value;

					fMatch = TRUE;
					break;
				}

				if ( !str_cmp(word, "StatPoints"))
				{
						ch->statpointsleft = fread_number (fp);
						ch->statpointsspent = fread_number (fp);
						stats_to_spend_check(ch);

						fMatch = TRUE;
						break;
				}
				else if ( !str_cmp( word, "Spell_failed" ) )
				{
					sh_int spell;
					unsigned int mob;

					mob = fread_number( fp );
					spell = fread_number( fp );
					load_spells_failed( ch, mob, spell );
					fMatch = TRUE;
					break;
				}
				KEY( "Spec", ch->pcdata->mage_specialist, fread_number( fp ) );
				break;

			case 'T':
				KEY( "Todelete", ch->pcdata->todelete, fread_number( fp ) ); // delayed ch delete - by Fuyara
				KEY( "TSex", ch->pcdata->true_sex, fread_number( fp ) );
				KEY( "Tru", ch->trust, fread_number( fp ) );

				if ( !str_cmp( word, "Titl" ) )
				{
					ch->pcdata->title = fread_string( fp );
					if ( ch->pcdata->title[ 0 ] != '.' && ch->pcdata->title[ 0 ] != ','
					     && ch->pcdata->title[ 0 ] != '!' && ch->pcdata->title[ 0 ] != '?' )
					{
						sprintf( buf, " %s", ch->pcdata->title );
						free_string( ch->pcdata->title );
						ch->pcdata->title = str_dup( buf );
					}
					fMatch = TRUE;
					break;
				}
				else if ( !str_cmp( word, "Trophy" ) )
				{
					int i, vnum, count;

					i = fread_number( fp );
					vnum = fread_number( fp );
					count = fread_number( fp );

                    if ( vnum > 0 && count > 0 )
                    {
                        ch->pcdata->trophy[ i ][ 0 ] = vnum;
                        ch->pcdata->trophy[ i ][ 1 ] = count;
                    }

					fMatch = TRUE;
					break;
				}
				else if ( !str_cmp( word, "Trick" ))
				{
					int sn;

					sn = fread_number( fp );
					if ( sn < 0 )
					{
						bug( "Fread_char: unknown trick. ", 0 );
					}

					ch->pcdata->tricks[ sn ] = 1;

					fMatch = TRUE;
					break;
				}
				break;

			case 'V':
				KEY( "Version", ch->version, fread_number ( fp ) );
				KEY( "Vers", ch->version, fread_number ( fp ) );
				if ( !str_cmp( word, "Vnum" ) )
				{
					ch->pIndexData = get_mob_index( fread_number( fp ) );
					fMatch = TRUE;
					break;
				}
				break;

			case 'W':
				KEY( "Wimpy", ch->wimpy, fread_number( fp ) );
				KEY( "Wimp", ch->wimpy, fread_number( fp ) );
				KEY( "Wizn", ch->wiznet, fread_flag( fp ) );
				KEY( "WizCfg", ch->wiz_config, fread_flag( fp ) );
				KEY( "WizGr", ch->pcdata->wiz_conf, fread_flag( fp ) );
				break;
			case 'Q':
				if ( !str_cmp( word, "QLog" ) )
				{
					extern bool add_questlog( CHAR_DATA * ch, char * qname, char * text, int state, int date  );
					extern bool title_questlog( CHAR_DATA * ch, char * qname, char * title );
					extern void sort_questlogs( CHAR_DATA * ch );
					char *qname = fread_string( fp );
					char *qtitle = fread_string( fp );
					char *qtext = fread_string( fp );
					char *qcurrentdesc = fread_string( fp );
					int qstate = fread_number( fp );
					int qdate = fread_number( fp );
                   
					add_questlog( ch, qname , qtext, qstate, qdate );
            	    title_questlog( ch, qname, qtitle );
            	    currentdesc_questlog( ch, qname, qcurrentdesc );
					sort_questlogs( ch );

					fMatch = TRUE;
					break;
				}
				break;

		}

		if ( !fMatch )
		{
			bugf( "Fread_char: no match '%s'.", word );
			fread_to_eol( fp );
		}
	}
}

extern	OBJ_DATA	*obj_free;
bool setobjflag( OBJ_DATA *obj, char *argument );

void fread_obj( CHAR_DATA *ch, FILE *fp, bool pccorpse, bool remote )
{
	OBJ_DATA * obj;
	char *word;
	int iNest;
	bool fMatch;
	bool fNest;
	bool fVnum;
	bool first;
	bool new_format;
	bool make_new;
	unsigned int room_vnum = 1;
	ROOM_INDEX_DATA* pRoom;

	fVnum = FALSE;
	obj = NULL;
	first = TRUE;  /* used to counter fp offset */
	new_format = FALSE;
	make_new = FALSE;

	word = feof( fp ) ? "End" : fread_word( fp );
	if ( !str_cmp( word, "Vnum" ) )
	{
		unsigned int vnum;
		first = FALSE;  /* fp will be in right place */

		vnum = fread_number( fp );
		if ( get_obj_index( vnum ) == NULL )
		{
			bugf( "Fread_obj: bad vnum %d ch: %s.", vnum, ch->name );
		}
		else
		{
			obj = create_object( get_obj_index( vnum ), remote );
			new_format = TRUE;
			//rellik: komponenty, wy³±czam komponentowo¶æ ¿eby nie zostawa³a w przypadku ³adowania wypalonych ju¿ komponentów
			obj->is_spell_item = FALSE;
			obj->rent_cost = obj->pIndexData->rent_cost; //zeby nie bylo losowej wartosci jakby nie bylo zapisane w pliku
		}
	}

	if ( obj == NULL )   /* either not found or old style */
	{
		obj = new_obj();
		obj->name	= str_dup( "" );
		obj->short_descr	= str_dup( "" );
		obj->description	= str_dup( "" );
	}

	fNest	= FALSE;
	fVnum	= TRUE;
	iNest	= 0;

	for ( ; ; )
	{
		if ( first )
			first = FALSE;
		else
			word = feof( fp ) ? "End" : fread_word( fp );
		fMatch = FALSE;

		switch ( UPPER( word[ 0 ] ) )
		{
			case '*':
				fMatch = TRUE;
				fread_to_eol( fp );
				break;

			case 'A':
				//rellik: odczyt affectów w starej wersji [20080704]
				if ( !str_cmp( word, "Affc" ) )
				{
					AFFECT_DATA * paf;
					int sn;
					int bank, vector;

					paf = new_affect();

					sn = skill_lookup( fread_word( fp ) );
					if ( sn < 0 )
						bug( "Fread_obj: unknown skill.", 0 );
					else
						paf->type = sn;

					paf->where	= fread_number( fp );
					paf->level = fread_number( fp );
					paf->duration = fread_number( fp );
					paf->modifier = fread_number( fp );
					paf->location = fread_number( fp );
					bank = fread_number( fp );
					vector =  fread_number( fp );
					paf->bitvector = vector_to_pointer( affect_flags, bank, vector );
					paf->next = obj->affected;

					obj->affected = paf;
					fMatch = TRUE;
					break;
				}
				//rellik: odczyt affectów w nowej wersji [20080704]
				if ( !str_cmp( word, "Affcn" ) )
				{
					AFFECT_DATA * paf;
					int sn;
					int bank, vector;

					paf = new_affect();

					sn = skill_lookup( fread_word( fp ) );
					if ( sn < 0 )
						bug( "Fread_obj: unknown skill.", 0 );
					else
						paf->type = sn;

					paf->where	= fread_number( fp );
					paf->level = fread_number( fp );
					paf->duration = fread_number( fp );
					paf->rt_duration = fread_number( fp );
					paf->modifier = fread_number( fp );
					paf->location = fread_number( fp );
					bank = fread_number( fp );
					vector =  fread_number( fp );
					paf->bitvector = vector_to_pointer( affect_flags, bank, vector );
					paf->next = obj->affected;

					if ( pccorpse == FALSE )
					{
						paf->rt_duration = paf->rt_duration - ( ( current_time - ch->pcdata->last_logoff ) / 60 );
						if ( paf->rt_duration < 0 ) paf->rt_duration = 0;
					}

					obj->affected = paf;
					fMatch = TRUE;
					break;
				}
				break;

			case 'C':
				KEY( "Cond", obj->condition, fread_number( fp ) );
				KEY( "Cost", obj->cost, fread_number( fp ) );
				break;

			case 'D':
				KEYS( "Description", obj->description, fread_string( fp ) );
				KEYS( "Desc", obj->description, fread_string( fp ) );
				break;

			case 'E':

				if ( !str_cmp( word, "Enchanted" ) )
				{
					obj->enchanted = TRUE;
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "ExtF" ) )
				{
					fread_to_eol( fp );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "ExtF2" ) )
				{
					fread_to_eol( fp );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "ExtFExt" ) )
				{
					fread_ext_flags( fp, obj->extra_flags );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "ExtraDescr" ) || !str_cmp( word, "ExDe" ) )
				{
					EXTRA_DESCR_DATA * ed;

					ed = new_extra_descr();

					ed->keyword	= fread_string( fp );
					ed->description	= fread_string( fp );
					ed->next	= obj->extra_descr;
					obj->extra_descr	= ed;
					fMatch = TRUE;
				}

				if ( !str_cmp( word, "End" ) )
				{
					if ( !fNest || ( fVnum && obj->pIndexData == NULL ) )
					{
						bugf( "Fread_obj: incomplete object: %d", fVnum );
						free_obj( obj );
						return ;
					}
					else
					{
						if ( !fVnum )
						{
							free_obj( obj );
							obj = create_object( get_obj_index( OBJ_VNUM_DUMMY ), remote );
						}

						if ( !new_format && !remote )
						{
							obj->next	= object_list;
							object_list	= obj;
							obj->pIndexData->count++;
						}

						if ( !obj->pIndexData->new_format
						     && obj->item_type == ITEM_ARMOR
						     && obj->value[ 1 ] == 0 )
						{
							obj->value[ 1 ] = obj->value[ 0 ];
							obj->value[ 2 ] = obj->value[ 0 ];
						}
						if ( make_new )
						{
							int wear;

							wear = obj->wear_loc;
							/*artefact*/
							if ( is_artefact( obj ) ) extract_artefact( obj );
							if ( obj->contains ) extract_artefact_container( obj );
							extract_obj( obj );

							obj = create_object( obj->pIndexData, remote );
							obj->wear_loc = wear;
						}
						if ( iNest == 0 || rgObjNest[ iNest ] == NULL )
						{
							if ( pccorpse )
							{
								pRoom = get_room_index( room_vnum );
								if ( pRoom ) obj_to_room( obj, pRoom );
							}
							else
							{
								obj_to_char( obj, ch );
								if ( obj->vnum_hoard > 0 )
								    carring_to_hoard( obj, ch, FALSE );
							}
						}
						else
							obj_to_obj( obj, rgObjNest[ iNest - 1 ] );
						return ;
					}
				}
				break;

			case 'G':
				KEY( "Gender", obj->gender, fread_number( fp ) );
				break;
			case 'H':
				KEY( "Hoard", obj->vnum_hoard, fread_number( fp ) );
				break;
			case 'I':
				KEY( "Ityp", obj->item_type, fread_number( fp ) );
				break;
			case 'K':
      					//rellik: komponenty, ³adowanie komponentowo¶ci do definicji obiektu
          			if ( !str_cmp( word, "Komponent" ) )
          			{
          				obj->is_spell_item = TRUE;
          				obj->spell_item_counter = 1;
          				obj->spell_item_timer = 525600;
          				obj->spell_item_counter = fread_number( fp );
          				obj->spell_item_timer = fread_number( fp );
          				fMatch = TRUE;
          				break;
          			}
          			break;

			case 'L':
				KEY( "LiczbaMnoga", obj->liczba_mnoga, fread_number( fp ) );
				if ( !str_cmp( word, "Length" ) )
				{
					obj->length = fread_number( fp );
					fMatch = TRUE;
				}
                break;

			case 'M':
				KEY( "Material", obj->material, material_lookup( fread_string( fp ) ) );
				break;
			case 'N':
				KEYS( "Name", obj->name, fread_string( fp ) );
				KEYS( "Nam2", obj->name2, fread_string( fp ) );
				KEYS( "Nam3", obj->name3, fread_string( fp ) );
				KEYS( "Nam4", obj->name4, fread_string( fp ) );
				KEYS( "Nam5", obj->name5, fread_string( fp ) );
				KEYS( "Nam6", obj->name6, fread_string( fp ) );

				if ( !str_cmp( word, "Nest" ) )
				{
					iNest = fread_number( fp );
					if ( iNest < 0 || iNest >= MAX_NEST )
					{
						bug( "Fread_obj: bad nest %d.", iNest );
					}
					else
					{
						rgObjNest[ iNest ] = obj;
						fNest = TRUE;
					}
					fMatch = TRUE;
				}
				break;

			case 'O':
				if ( !str_cmp( word, "Oldstyle" ) )
				{
					if ( obj->pIndexData != NULL && obj->pIndexData->new_format )
						make_new = TRUE;
					fMatch = TRUE;
				}
				break;

			case 'P':
				if ( !str_cmp( word, "Pflag" ) )
				{
					setobjflag( obj, fread_string( fp ) );
					fMatch = TRUE;
					break;
				}

				break;
			case 'R':
				KEY( "Room", room_vnum, fread_number( fp ) );
				if ( !str_cmp( word, "Rent_cost" ) )
				{
					obj->rent_cost = fread_number( fp );
					fMatch = TRUE;
					break;
				}
				if ( !str_cmp( word, "Rep" ) )
				{
					obj->repair_limit     = fread_number( fp );
					obj->repair_counter   = fread_number( fp );
					obj->repair_penalty   = fread_number( fp );
					obj->repair_condition = fread_number( fp );
					fMatch = TRUE;
					break;
				}
				break;
			case 'S':
				KEYS( "ShD", obj->short_descr, fread_string( fp ) );

				if ( !str_cmp( word, "Spell" ) )
				{
					int iValue;
					int sn;

					iValue = fread_number( fp );
					sn = skill_lookup( fread_word( fp ) );
					if ( iValue < 0 || iValue > 3 )
					{
						bug( "Fread_obj: bad iValue %d.", iValue );
					}
					else if ( sn < 0 )
					{
						bug( "Fread_obj: unknown skill.", 0 );
					}
					else
					{
						obj->value[ iValue ] = sn;
					}
					fMatch = TRUE;
					break;
				}
				break;

			case 'T':
				KEY( "Timer", obj->timer, fread_number( fp ) );
				KEY( "Time", obj->timer, fread_number( fp ) );
				KEY( "Trap", obj->trap, fread_number( fp ) );
				break;

			case 'V':
				if ( !str_cmp( word, "Values" ) || !str_cmp( word, "Vals" ) )
				{
					obj->value[ 0 ] = fread_number( fp );
					obj->value[ 1 ] = fread_number( fp );
					obj->value[ 2 ] = fread_number( fp );
					obj->value[ 3 ] = fread_number( fp );

					if ( obj->item_type == ITEM_WEAPON && obj->value[ 0 ] == 0 )
						obj->value[ 0 ] = obj->pIndexData->value[ 0 ];
					fMatch	= TRUE;
					break;
				}

				if ( !str_cmp( word, "Val" ) )
				{
					obj->value[ 0 ] = fread_number( fp );
					obj->value[ 1 ] = fread_number( fp );
					obj->value[ 2 ] = fread_number( fp );
					obj->value[ 3 ] = fread_number( fp );
					obj->value[ 4 ] = fread_number( fp );
					obj->value[ 5 ] = fread_number( fp );
					obj->value[ 6 ] = fread_number( fp );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "Vnum" ) )
				{
					unsigned int vnum;

					vnum = fread_number( fp );
					if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL )
						bug( "Fread_obj: bad vnum %d.", vnum );
					else
						fVnum = TRUE;
					fMatch = TRUE;
					break;
				}
				break;

			case 'W':
				KEY( "WearFlags", obj->wear_flags, fread_number( fp ) );
				KEY( "WeaF", obj->wear_flags, fread_number( fp ) );
				KEY( "WearLoc", obj->wear_loc, fread_number( fp ) );
				KEY( "Wear", obj->wear_loc, fread_number( fp ) );
				KEY( "Weight", obj->weight, fread_number( fp ) );
				KEY( "Wt", obj->weight, fread_number( fp ) );

				if ( !str_cmp( word, "Wear2" ) )
				{
					fread_to_eol( fp );
					fMatch = TRUE;
					break;
				}

				if ( !str_cmp( word, "Wear2Ext" ) )
				{
					fread_ext_flags( fp, obj->wear_flags2 );
					fMatch = TRUE;
					break;
				}
				break;
            case '~':
                fMatch = TRUE;
                break;
		}

		if ( !fMatch )
		{
			bugf( "Fread_obj: no match '%s'.", word );
			fread_to_eol( fp );
		}
	}
}

void save_pccorpses( void )
{
	OBJ_DATA * obj, *obj_next;
	FILE* fp;
	char strsave[ MAX_INPUT_LENGTH ];

	fclose( fpReserve );

	if ( ( fp = fopen( TEMP_FILE, "w" ) ) == NULL )
	{
		fpReserve = fopen( NULL_FILE, "r" );
		bug( "Save_pccorpses: fopen", 0 );
		perror( strsave );

		return;
	}

	for ( obj = object_list; obj != NULL; obj = obj_next )
	{
		obj_next = obj->next;
		if ( obj->item_type != ITEM_CORPSE_PC
		     || !obj->in_room
		     || !obj->contains
		     || obj->value[ 0 ] < 2 )
			continue;

		fprintf( fp, "#PCCORPSE\n" );
		fwrite_obj( NULL, obj, fp, 0, TRUE, FALSE );
	}

	fprintf( fp, "#END\n" );

	fclose( fp );
	rename( TEMP_FILE, PCCORPSES_FILE );
	fpReserve = fopen( NULL_FILE, "r" );
}

void load_pccorpses( void )
{
	FILE * fp;

	fclose( fpReserve );

	if ( ( fp = fopen( PCCORPSES_FILE, "r" ) ) != NULL )
	{
		int iNest;

		for ( ; ; )
		{
			char letter;
			char *word;

			letter = fread_letter( fp );
			if ( letter == '*' )
			{
				fread_to_eol( fp );
				continue;
			}

			if ( letter != '#' )
			{
				bug( "Load_char_obj: # not found.", 0 );
				break;
			}

			word = fread_word( fp );
			if ( !str_cmp( word, "PCCORPSE" ) )
			{
				for ( iNest = 0; iNest < MAX_NEST; iNest++ )
					rgObjNest[ iNest ] = NULL;
			}
			else if ( !str_cmp( word, "OBJECT" ) ) fread_obj ( NULL, fp, TRUE, FALSE );
			else if ( !str_cmp( word, "O" ) ) fread_obj ( NULL, fp, TRUE, FALSE );
			else if ( !str_cmp( word, "END" ) ) break;
			else
			{
				bugf( "Load_pccorpses: bad section %s.", word );
				break;
			}
		}
		fclose( fp );
	}
	unlink( PCCORPSES_FILE );
	fpReserve = fopen( NULL_FILE, "r" );
}

void add_host( CHAR_DATA *ch, char * host, bool insert )
{
	int i;

	return ;
	if ( !ch || IS_NULLSTR( host ) )
		return ;

	if ( ch->pcdata->hosts_count == 0 )
	{
		ch->pcdata->hosts[ 0 ] = str_dup( host );
		ch->pcdata->hosts_count++;
		return ;
	}

	for ( i = 0; i < ch->pcdata->hosts_count; i++ )
		if ( !IS_NULLSTR( ch->pcdata->hosts[ i ] ) && !str_cmp( ch->pcdata->hosts[ i ], host ) )
			return ;

	if ( !insert )
	{
		if ( ch->pcdata->hosts_count < MAX_HOST )
		{
			free_string( ch->pcdata->hosts[ ch->pcdata->hosts_count ] );
			ch->pcdata->hosts[ ch->pcdata->hosts_count ] = str_dup( host );
			ch->pcdata->hosts_count++;
		}
	}
	else
	{
		for ( i = ch->pcdata->hosts_count - 1; i >= 0; i-- )
			if ( i + 1 < MAX_HOST )
				ch->pcdata->hosts[ i + 1 ] = ch->pcdata->hosts[ i ];

		ch->pcdata->hosts[ 0 ] = str_dup( host );
		ch->pcdata->hosts_count = UMAX( MAX_HOST, ch->pcdata->hosts_count + 1 );
	}
	return ;
}
