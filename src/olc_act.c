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
 *  File: olc_act.c                                                    *
 *                                                                     *
 *  Much time and thought has gone into this software and you are      *
 *  benefitting.  We hope that you share your changes too.  What goes  *
 *  around, comes around.                                              *
 *                                                                     *
 *  This code was freely distributed with the The Isles 1.1 source     *
 *  code, and has been used here for OLC - OLC would not be what it    *
 *  is without all the previous coders who released their source code. *
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
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: olc_act.c 11015 2012-02-23 08:45:48Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/olc_act.c $
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
#include "tables.h"
#include "progs.h"
#include "olc.h"
#include "recycle.h"
#include "lang.h"
#include "si.h"
#include "traps.h"
#include "herbs.h"
#include "projects.h"
#include "money.h"

char *	lang_bit_name		args( (int flag) );
int 	armor_lookup		args( (char *name) );
char * 	armor_name		    args( (int armor) );
char *  string_linedel      args( ( char *, int ) );
char *  string_lineadd      args( ( char *, char *, int ) );
char *  numlineas           args( ( char * ) );
void save_area	args( ( AREA_DATA *pArea ) );
char * jewlery2hands		args( (int type)  );
int    get_rep_type	args( ( char * name ) );


/* Return TRUE if area changed, FALSE if not. */
#define REDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define OEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define MEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )
#define AEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )


#define MAX_TYPE  6

/* start food state - by Fuyara */
struct food_state_type
{
    char *name_en;
    char *name_pl;
    int   state;
};

const struct food_state_type food_state_table[] =
{
    { "edible", 	"jadalne", 	FOOD_STATE_EDIBLE 	},
    { "raw", 		"surowe", 	FOOD_STATE_RAW 		},
    { "cooked", 	"gotowane",	FOOD_STATE_COOKED	},
    { "baked",		"pieczone",	FOOD_STATE_BAKED	},
    { "dried",		"suszone", 	FOOD_STATE_DRIED	},
    { "smoked",		"wêdzone", 	FOOD_STATE_SMOKED	},
    { "fried",		"sma¿one", 	FOOD_STATE_FRIED	}
};

int get_food_state_value( char *name )
{
    int length = sizeof( food_state_table ) / sizeof( food_state_table[0] );
    int i = 0;

    for ( i = 0; i < length; i++ )
    {
	if ( !str_cmp( food_state_table[i].name_pl, name )
	  || !str_cmp( food_state_table[i].name_en, name )
	  || food_state_table[i].state == atoi( name ) )
	{
	    return food_state_table[i].state;
	}
    }

    return -1;
}

char *get_food_state_name( int state, char lang )
{
    int length = sizeof( food_state_table ) / sizeof( food_state_table[0] );
    int i = 0;

    for ( i = 0; i < length; i++ )
    {
	if ( food_state_table[i].state == state )
	{
	    switch ( lang )
	    {
		case 'p': return food_state_table[i].name_pl; break;
		case 'e': return food_state_table[i].name_en; break;
	    }
	}
    }

    return "invalid state";
}

void show_food_state_table( CHAR_DATA *ch )
{
    int  length = sizeof( food_state_table ) / sizeof( food_state_table[0] );
    int  i = 0;
    char buf[MSL];

    send_to_char( "+-----+----------------------+----------------------+\n\r", ch );
    sprintf( buf, "| %-3s | %-20s | %-20s |\n\r", "val", "nazwa polska", "nazwa angielska" );
    send_to_char( buf, ch );
    send_to_char( "+-----+----------------------+----------------------+\n\r", ch );

    for ( i = 0; i < length; i++ )
    {
	sprintf( buf, "| %3d | %-20s | %-20s |\n\r", food_state_table[i].state, food_state_table[i].name_pl, food_state_table[i].name_en );
	send_to_char( buf, ch );
    }

    send_to_char( "+-----+----------------------+----------------------+\n\r", ch );

    return;
}
/* end food state */

/* start food type */
struct food_type
{
    char *name_en;
    char *name_pl;
    int   state;
};

const struct food_state_type food_type_table[] =
{
    { "mixed", 	"mieszane", FOOD_TYPE_MIXED },
    { "meat", 	"miêso", 	FOOD_TYPE_MEAT 	},
    { "plant", 	"ro¶lina",	FOOD_TYPE_PLANT	}
};

int get_food_type_value( char *name )
{
    int length = sizeof( food_type_table ) / sizeof( food_type_table[0] );
    int i = 0;

    for ( i = 0; i < length; i++ )
    {
    	if ( !str_cmp( food_type_table[i].name_pl, name )
    			|| !str_cmp( food_type_table[i].name_en, name )
    			|| food_type_table[i].state == atoi( name ) )
    	{
    		return food_type_table[i].state;
    	}
    }
    return -1;
}

char *get_food_type_name( int state, char lang )
{
    int length = sizeof( food_type_table ) / sizeof( food_type_table[0] );
    int i = 0;

    for ( i = 0; i < length; i++ )
    {
    	if ( food_type_table[i].state == state )
    	{
    		switch ( lang )
    		{
    		case 'p': return food_type_table[i].name_pl; break;
    		case 'e': return food_type_table[i].name_en; break;
    		}
    	}
    }
    return "invalid state";
}

void show_food_type_table( CHAR_DATA *ch )
{
    int  length = sizeof( food_type_table ) / sizeof( food_type_table[0] );
    int  i = 0;
    char buf[MSL];

    send_to_char( "+-----+----------------------+----------------------+\n\r", ch );
    sprintf( buf, "| %-3s | %-20s | %-20s |\n\r", "val", "nazwa polska", "nazwa angielska" );
    send_to_char( buf, ch );
    send_to_char( "+-----+----------------------+----------------------+\n\r", ch );

    for ( i = 0; i < length; i++ )
    {
	sprintf( buf, "| %3d | %-20s | %-20s |\n\r", food_type_table[i].state, food_type_table[i].name_pl, food_type_table[i].name_en );
	send_to_char( buf, ch );
    }

    send_to_char( "+-----+----------------------+----------------------+\n\r", ch );

    return;
}
struct lev_standard
{
	int		min_lev;
	int 	max_lev;
	int		min_hp;
	int		max_hp;
};

const struct lev_standard standard_table[MAX_TYPE]=
{
	{ 1,  5, 15, 20 },
	{ 6,  8, 18, 25 },
	{ 9, 12, 20, 25 },
	{13, 15, 20, 28 },
	{16, 18, 20, 30 },
	{19, 40, 25, 40 }
};


struct olc_help_type
{
	char *command;
	const void *structure;
	char *desc;
};



bool show_version( CHAR_DATA *ch, char *argument )
{
	send_to_char( VERSION, ch );
	send_to_char( "\n\r", ch );
	send_to_char( AUTHOR, ch );
	send_to_char( "\n\r", ch );
	send_to_char( DATE, ch );
	send_to_char( "\n\r", ch );
	send_to_char( CREDITS, ch );
	send_to_char( "\n\r", ch );
	return FALSE;
}

/* repair stuff */

int get_rep_type( char * name )
{
    int i;

    for( i = 0; repair_item_table[i].name; i++ )
        if( !str_prefix( name, repair_item_table[i].name) )
            return i;

    return -1;
}

/*
 * This table contains help commands and a brief description of each.
 * ------------------------------------------------------------------
 */
const struct olc_help_type help_table[] =
{
	{ "separator", NULL, "KRAINKA" },
	{ "area",          area_flags,          "W³asno¶ci krainki." },

	{ "separator", NULL, "LOKACJA" },
    { "capacity",      NULL,                "Definiowanie zasobno¶æ lokacji w minera³y w bry³kach." }, //rellik: mininig
    { "exit",          exit_flags,          "W³asno¶ci przej¶æ." },
    { "resource",      NULL,                "Definiowanie minera³ów dostêpnych w lokacji." }, //rellik: mininig
    { "room",          room_flags,          "W³asno¶ci lokacji." },
    { "sector",        sector_table,        "Rodzaj terenu w lokacji." },


	{ "separator", NULL, "MOB" },
	{ "act",           act_flags,           "W³asno¶ci moba." },
	{ "affect",        affect_flags,        "Affekty na mobie, czyli pod wp³ywem czego on siê znajduje." },
	{ "form",          form_flags,          "Forma moba - {Rnie u¿ywaæ{x." },
	{ "off",           off_flags,           "Ofensywne zachowania." },
	{ "part",          part_flags,          "Czê¶ci cia³a - {Rnie u¿ywaæ{x." },
	{ "position",      position_flags,      "Pozycje mobów." },
	{ "sex",           sex_flags,           "P³eæ." },
	{ "size",          size_flags,          "Wielko¶ci moba." },
	{ "spec",          spec_table,          "Zachowania specjalne." },
	{ "spells",        skill_table,         "Listê dostêpnych czarów." },
	{ "langs",         lang_table,          "Listê dostêpnych jêzyków." },

	{ "separator", NULL, "PRZEDMIOT" },
	{ "apply",         apply_flags,         "Lista statystyk, które mo¿e przedmiot modyfikowaæ." },
	{ "apptype",       apply_types,         "Apply types." },
	{ "armor",         ac_type,             "Typy klasy pancerza." },
	{ "armor_type",    armor_table,         "Rodzaje zbroji." },
	{ "container",     container_flags,     "Flagi pojemnika." },
	{ "corpse",        corpse_flags,        "W³a¶ciwo¶ci cia³." },
	{ "extra",         extra_flags,         "W³asno¶ci przedmiotów." },
	{ "exwear",        wear_flags2,         "Dodatkowa flaga do wear." },
	{ "foodstate",     food_state_table,    "Stan jedzenia." }, // food state by Fuyara
	{ "furniture",     furniture_flags,     "Lista tego co mo¿na zrobiæ z meblem." },
	{ "instrument",    instrument_class,    "Rodzaj instrumentu." },
	{ "liquid",        liq_table,           "Tabela z rodzajami p³ynów." },
	{ "weed",          weed_table,          "Tabela z rodzajami ziela do fajek." },
	{ "poison",        poison_table,        "Rodzaje trucizny s³u¿±cych do zatruwania broni." },
	{ "portal",        portal_flags,        "Portal types." },
	{ "type",          type_flags,          "Typy (rodzaje) przedmiotów." },
	{ "wclass",        weapon_class,        "Rodzaje mo¿liwych do stworzenia broni." },
	{ "weapon",        attack_table,        "Roczaj zadawanych obra¿eñ." },
	{ "wear",          wear_flags,          "Gdzie mo¿na przedmiot za³o¿yæ." },
	{ "wear-loc",      wear_loc_flags,      "Gdzie mo¿na mobikowi za³o¿yæ ubranie?" },
	{ "wtype",         weapon_type2,        "Specialne w³a¶ciwo¶ci lub ograniczenia broni." },
	{ "wand",          wand_class,          "W³a¶ciwo¶ci ro¿d¿ek." },
	{ "wand_extra",    wand_extra,          "Dodatkowe w³a¶ciwo¶ci ro¿d¿ek." },
	{ "komponent",     NULL,                "Definiowanie komponentowo¶ci." }, //rellik: komponenty, ¿eby by³o wiadomo
	{ "tool_type",     tool_table,          "Rodzaje narzêdzi." },
	{ "totem",         totem_table,         "Rodzaje totemicznych zwierz±t." },
    { "repair",        NULL,                "Limity napraw przedmiotu." }, // repair limits

	{ "separator", NULL, "PROGI" },
	{ "trig-position", position_trig_flags, "Mobile positions on triggers." },
	{ "trigger",       prog_flags,          "MobProgram flags." },
	{ "foodtype",      food_type_table,     "Typy jedzenia." },


	{ NULL, NULL, NULL }
};


/*****************************************************************************
Name:		show_flag_cmds
Purpose:	Displays settable flags and stats.
Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_flag_cmds( CHAR_DATA *ch, const struct flag_type *flag_table )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  flag;
    int  col;

    buf1[0] = '\0';
    col = 0;
    for (flag = 0; flag_table[flag].name != NULL; flag++)
    {
        if ( flag_table[flag].settable )
        {
            sprintf( buf, "%-19.18s", flag_table[flag].name );
            strcat( buf1, buf );
            if ( ++col % 4 == 0 )
            {
                strcat( buf1, "\n\r" );
            }
        }
    }

    if ( col % 4 != 0 )
    {
        strcat( buf1, "\n\r" );
    }

    send_to_char( buf1, ch );
    return;
}

void show_ext_flag_cmds( CHAR_DATA *ch, const struct ext_flag_type *flag_table )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  flag;
    int  col;

    buf1[0] = '\0';
    col = 0;
    for (flag = 0; flag_table[flag].name != NULL; flag++)
    {
        if ( flag_table[flag].settable )
        {
            sprintf( buf, "%-19.18s", flag_table[flag].name );
            strcat( buf1, buf );
            if ( ++col % 4 == 0 )
            {
                strcat( buf1, "\n\r" );
            }
        }
    }

    if ( col % 4 != 0 )
    {
        strcat( buf1, "\n\r" );
    }

    send_to_char( buf1, ch );
    return;
}

/*****************************************************************************
Name:		show_skill_cmds
Purpose:	Displays all skill functions.
Does remove those damn immortal commands from the list.
Could be improved by:
(1) Adding a check for a particular class.
(2) Adding a check for a level range.
Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_skill_cmds( CHAR_DATA *ch, int tar )
{
	char buf  [ MAX_STRING_LENGTH ];
	char buf1 [ MAX_STRING_LENGTH*2 ];
	int  sn;
	int  col;

	buf1[0] = '\0';
	col = 0;
	for (sn = 0; sn < MAX_SKILL; sn++)
	{
		if ( !skill_table[sn].name )
			break;

		if ( !str_cmp( skill_table[sn].name, "reserved" ) || skill_table[sn].spell_fun == spell_null )
			continue;

		if ( tar == -1 || skill_table[sn].target == tar )
		{
			sprintf( buf, "%-19.18s", skill_table[sn].name );
			strcat( buf1, buf );
			if ( ++col % 4 == 0 )
				strcat( buf1, "\n\r" );
		}
	}

	if ( col % 4 != 0 )
		strcat( buf1, "\n\r" );

	send_to_char( buf1, ch );
	return;
}

/*****************************************************************************
Name:		show_spec_cmds
Purpose:	Displays settable special functions.
Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_spec_cmds( CHAR_DATA *ch )
{
	char buf  [ MAX_STRING_LENGTH ];
	char buf1 [ MAX_STRING_LENGTH ];
	int  spec;
	int  col;

	buf1[0] = '\0';
	col = 0;
	send_to_char( "Preceed special functions with 'spec_'\n\r\n\r", ch );
	for (spec = 0; spec_table[spec].function != NULL; spec++)
	{
		sprintf( buf, "%-19.18s", &spec_table[spec].name[5] );
		strcat( buf1, buf );
		if ( ++col % 4 == 0 )
			strcat( buf1, "\n\r" );
	}

	if ( col % 4 != 0 )
		strcat( buf1, "\n\r" );

	send_to_char( buf1, ch );
	return;
}

/*****************************************************************************
Name:		show_languages_cmds
Purpose:	Displays table languages.
Called by:	show_help(olc_act.c).
 ****************************************************************************/
void show_languages_cmds( CHAR_DATA *ch )
{
	char buf  [ MAX_STRING_LENGTH ];
	char buf1 [ MAX_STRING_LENGTH ];
	int  i;
	int  col;

	buf1[0] = '\0';
	col = 0;
	send_to_char( "\n\rDostêpne jêzyki:\n\r", ch );
	for (i = 0; lang_table[i].name != NULL; i++)
	{
		sprintf( buf, "%-19.18s", lang_table[i].name );
		strcat( buf1, buf );
		if ( ++col % 4 == 0 )
			strcat( buf1, "\n\r" );
	}
	if ( col % 4 != 0 )
		strcat( buf1, "\n\r" );

	send_to_char( buf1, ch );
	return;
}

/*****************************************************************************
Name:		show_help
Purpose:	Displays help for many tables used in OLC.
Called by:	olc interpreters.
 ****************************************************************************/
bool show_help( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char spell[MAX_INPUT_LENGTH];
    int cnt;

    argument = one_argument( argument, arg );
    one_argument( argument, spell );

    /*
     * Display syntax.
     */
    if ( arg[0] == '\0' )
    {
        send_to_char( "Sk³adnia: ? [komenda]\n\r\n\r", ch );
        send_to_char( "komenda    - opis\n\r", ch );
        for (cnt = 0; help_table[cnt].command != NULL; cnt++)
        {
            if (str_cmp( help_table[cnt].command , "separator" ))
            {
                sprintf( buf, "%-10.10s - %s\n\r", capitalize( help_table[cnt].command ), help_table[cnt].desc );
            }
            else
            {
                sprintf( buf, "\n\r-- {W%-9.9s{x --------------------\n\r", help_table[cnt].desc );
            }
            send_to_char( buf, ch );
        }
        return FALSE;
    }

    /*
     * Find the command, show changeable data.
     * ---------------------------------------
     */
    for (cnt = 0; help_table[cnt].command != NULL; cnt++)
    {
        if (  arg[0] == help_table[cnt].command[0] && !str_prefix( arg, help_table[cnt].command ) )
        {
            if( help_table[ cnt ].structure == NULL )
            {
                return FALSE;
            }
            print_char( ch, "\n\r{R%s{x - {G%s{x\n\r\n\r", help_table[cnt].command, help_table[cnt].desc );
            if ( help_table[cnt].structure == spec_table )
            {
                show_spec_cmds( ch );
                return FALSE;
            }
            else if ( help_table[cnt].structure == lang_table )
            {
                show_languages_cmds( ch );
                return FALSE;
            }
            else if ( help_table[cnt].structure == liq_table )
            {
                show_liqlist( ch );
                return FALSE;
            }
            else if ( help_table[cnt].structure == weed_table )
            {
                show_weedlist( ch );
                return FALSE;
            }
            else if ( help_table[cnt].structure == attack_table )
            {
                show_damlist( ch );
                return FALSE;
            }
            else if ( help_table[cnt].structure == armor_table )
            {
                show_armor_list( ch );
                return FALSE;
            }
            else if ( help_table[cnt].structure == poison_table )
            {
                show_poison_list( ch );
                return FALSE;
            }
            else if ( help_table[cnt].structure == sector_table )
            {
                show_sector_list( ch );
                return FALSE;
            }
            /* start food state - by Fuyara */
            else if ( help_table[cnt].structure == food_state_table )
            {
                show_food_state_table( ch );
                return FALSE;
            }
            else if ( help_table[cnt].structure == food_type_table )
            {
                show_food_type_table( ch );
                return FALSE;
            }
            /* end food state */
            else if ( help_table[cnt].structure == skill_table )
            {
                if ( spell[0] == '\0' )
                {
                    send_to_char( "Sk³adnia: ? spells [ignore/attack/defend/self/object/all]\n\r", ch );
                    return FALSE;
                }

                if ( !str_prefix( spell, "all" ) ) show_skill_cmds( ch, -1 );
                else if ( !str_prefix( spell, "ignore" ) ) show_skill_cmds( ch, TAR_IGNORE );
                else if ( !str_prefix( spell, "attack" ) ) show_skill_cmds( ch, TAR_CHAR_OFFENSIVE );
                else if ( !str_prefix( spell, "defend" ) ) show_skill_cmds( ch, TAR_CHAR_DEFENSIVE );
                else if ( !str_prefix( spell, "self" ) ) show_skill_cmds( ch, TAR_CHAR_SELF );
                else if ( !str_prefix( spell, "object" ) ) show_skill_cmds( ch, TAR_OBJ_INV );
                else send_to_char( "Sk³adnia: ? spell [ignore/attack/defend/self/object/all]\n\r", ch );
                return FALSE;
            }
            /**
             * uwaga obsluga ext_flag_type
             */
            else if (
                    help_table[cnt].structure == affect_flags ||
                    help_table[cnt].structure == act_flags ||
                    help_table[cnt].structure == off_flags ||
                    help_table[cnt].structure == extra_flags ||
                    help_table[cnt].structure == wear_flags2 ||
                    help_table[cnt].structure == prog_flags ||
                    help_table[cnt].structure == room_flags
                    )
            {
                show_ext_flag_cmds( ch, help_table[cnt].structure );
                return FALSE;
            }
            else
            {
                show_flag_cmds( ch, help_table[cnt].structure );
                return FALSE;
            }
        }
    }
    show_help( ch, "" );
    return FALSE;
}

REDIT( redit_links )
{
	ROOM_INDEX_DATA	*pRoomIndex;
	AREA_DATA		*pArea;
	EXIT_DATA *pexit;
	char		buf  [ MAX_STRING_LENGTH   ];
	BUFFER		*buf1;
	bool found;
	ush_int vnum;
	int door;

	pArea = ch->in_room->area;
	buf1=new_buf();
	found   = FALSE;

	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
		if ( ( pRoomIndex = get_room_index( vnum ) ) )
		{
			for ( door = 0; door <= 5; door++ )
				if ( ( pexit = pRoomIndex->exit[door] ) != NULL
						&& ( pexit->u1.to_room->vnum < pArea->min_vnum
							|| pexit->u1.to_room->vnum > pArea->max_vnum ))
				{
					if( !found )
					{
						sprintf(buf,"Kraina: %s Vnums: [%-5d-%5d]\n\r", pArea->name,
								pArea->min_vnum, pArea->max_vnum);
						add_buf(buf1, buf
						       );
					}
					found = TRUE;
					sprintf( buf, "Room {C%5d{x ma wyj¶cie do {C%5d{x [%8.8s] -> %s\n\r",
							vnum, pexit->u1.to_room->vnum, dir_name[door],
							pexit->u1.to_room->area->name
					       );
					add_buf( buf1, buf );
				}
		}

	if ( !found )
	{
		send_to_char( "Kraina nie ma wyj¶æ do innych krain.\n\r", ch);
		free_buf(buf1);
		return FALSE;
	}

	page_to_char( buf_string(buf1), ch );
	free_buf(buf1);
	return FALSE;
}

REDIT( redit_progs )
{
	ROOM_INDEX_DATA	*pRoom;
	MOB_INDEX_DATA	*pMob;
	OBJ_INDEX_DATA	*pObj;
	AREA_DATA		*pArea;
	char		buf  [ MAX_STRING_LENGTH   ];
	BUFFER		*buffer;
	bool found;
	ush_int vnum;
	PROG_LIST  *prg;

	pArea	= ch->in_room->area;
	buffer	= new_buf();
	found	= FALSE;

	//Mobilesy
	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
	{
		if( ( pMob = get_mob_index( vnum ) ) == NULL )
			continue;

		for ( prg = pMob->progs; prg != NULL; prg = prg->next )
		{
			sprintf( buf, "MOB:[%5d]%-15.15s Name:[ %-15s] %-8s '%s'\n\r",
					pMob->vnum,
					pMob->short_descr,
					prg->name,
					ext_bit_name( prog_flags, prg->trig_type ),
					prg->trig_phrase);
			add_buf(buffer, buf);
			found = TRUE;
		}
	}

	//Rooms
	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
	{
		if( ( pRoom = get_room_index( vnum ) ) == NULL )
			continue;

		for ( prg = pRoom->progs; prg != NULL; prg = prg->next )
		{
			sprintf( buf, "ROOM:[%5d]               Name:[ %-15s] %-8s '%s'\n\r",
					pRoom->vnum,
					prg->name,
					ext_bit_name( prog_flags, prg->trig_type ),
					prg->trig_phrase);
			add_buf(buffer, buf);
			found = TRUE;
		}
	}

	//Objects
	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
	{
		if( ( pObj = get_obj_index( vnum ) ) == NULL )
			continue;

		for ( prg = pObj->progs; prg != NULL; prg = prg->next )
		{
			sprintf( buf, "OBJ:[%5d]%-15.15s Name:[ %-15s] %-8s '%s'\n\r",
					pObj->vnum,
					pObj->short_descr,
					prg->name,
					ext_bit_name( prog_flags, prg->trig_type ),
					prg->trig_phrase);
			add_buf(buffer, buf);
			found = TRUE;
		}
	}
	//Rooms
	if ( !found )
	{
		send_to_char( "Nie ma progów w tej kraince.\n\r", ch);
		free_buf(buffer);
		return FALSE;
	}


	page_to_char( buf_string(buffer), ch );
	free_buf(buffer);
	return FALSE;
}

REDIT( redit_mshow )
{

	MOB_INDEX_DATA *pMob;
	int value;

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: mshow <vnum>\n\r", ch );
		return FALSE;
	}

	if ( !is_number( argument ) )
	{
		send_to_char( "REdit: Ingresa un numero.\n\r", ch);
		return FALSE;
	}

	if ( is_number( argument ) )
	{
		value = atoi( argument );
		if ( !( pMob = get_mob_index( value ) ))
		{
			send_to_char( "REdit:  That mobile does not exist.\n\r", ch );
			return FALSE;
		}
        if ( !IS_BUILDER( ch, pMob->area ) )
        {
            send_to_char( "Masz za niski poziom security do ogladania szczegolow tego moba.\n\r", ch );
            return FALSE;
        }
		ch->desc->pEdit = (void *)pMob;
	}

	medit_show( ch, argument );
	ch->desc->pEdit = (void *)ch->in_room;
	return FALSE;
}

REDIT( redit_oshow )
{

	OBJ_INDEX_DATA *pObj;
	int value;

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: oshow <vnum>\n\r", ch );
		return FALSE;
	}

	if ( !is_number( argument ) )
	{
		send_to_char( "REdit: Ingresa un numero.\n\r", ch);
		return FALSE;
	}

	if ( is_number( argument ) )
	{
		value = atoi( argument );
		if ( !( pObj = get_obj_index( value ) ))
		{
			send_to_char( "REdit:  That object does not exist.\n\r", ch );
			return FALSE;
        }
        if ( !IS_BUILDER( ch, pObj->area ) )
        {
            send_to_char( "Masz za niski poziom security do ogladania szczegolow tego przedmiotu.\n\r", ch );
            return FALSE;
        }
		ch->desc->pEdit = (void *)pObj;
	}

	oedit_show( ch, argument );
	ch->desc->pEdit = (void *)ch->in_room;
	return FALSE;
}

REDIT( redit_forroom )
{
	char from[MAX_INPUT_LENGTH];
	char to[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *pRoom, *oldRoom;
	int i, ifrom, ito;

	argument = one_argument( argument, from );
	argument = one_argument( argument, to );

	if ( from[0] == '\0' || !is_number(from) || to[0] == '\0' || !is_number(to) || argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: forroom <vnum_od> <vnum_do> <komenda>\n\r", ch );
		return FALSE;
	}

	ifrom = atoi(from);
	ito = atoi(to);

	i = ifrom;
	if ( ifrom > ito )
	{
		ifrom = ito;
		ito = i;
	}

	if ( ( ifrom < ch->in_room->area->min_vnum && ito < ch->in_room->area->min_vnum )
			|| ( ifrom > ch->in_room->area->max_vnum && ito > ch->in_room->area->max_vnum ) )
	{
		send_to_char( "Przedzia³, który poda³<&e¶/a¶/e¶> nie nale¿y do krainy, któr± edytujesz.\n\r", ch );
		return FALSE;
	}

	if ( ifrom < ch->in_room->area->min_vnum )
	{
		ifrom = ch->in_room->area->min_vnum;
		send_to_char( "Dolna granica ograniczona minmalnym vnumem krainy.\n\r", ch );
	}

	if ( ito > ch->in_room->area->max_vnum )
	{
		ito = ch->in_room->area->max_vnum;
		send_to_char( "Dolna granica ograniczona maksymalnym vnumem krainy.\n\r", ch );
	}

	oldRoom = ch->in_room;

	for( i = ifrom; i <= ito; i++ )
	{
		print_char( ch, "Room %d -> ", i );
		pRoom = get_room_index( i );
		if ( !pRoom )
		{
			send_to_char( "{RNie ma takiej lokacji.{x\n\r", ch );
			continue;
		}

		char_from_room( ch );
		char_to_room( ch, pRoom );
		redit( ch, argument );
	}

	char_from_room( ch );
	char_to_room( ch, oldRoom );
	ch->desc->pEdit = (void *)ch->in_room;
	return TRUE;
}

REDIT ( redit_addprog )
{
	BITVECT_DATA * value;
	ROOM_INDEX_DATA *pRoom;
	PROG_LIST *list;
	PROG_CODE *code;
	char trigger[MAX_STRING_LENGTH];
	char phrase[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];

	EDIT_ROOM(ch, pRoom);
	argument=one_argument(argument, name);
	argument=one_argument(argument, trigger);
	argument=one_argument(argument, phrase);

	if ( name[0] == '\0' || trigger[0] =='\0' || phrase[0] =='\0' )
	{
		send_to_char("Sk³adnia:  addprog [vnum] [trigger] [phrase]\n\r",ch);
		return FALSE;
	}

	value = ext_flag_lookup ( trigger, prog_flags );
	if ( value == &EXT_NONE )
	{
		send_to_char("Valid flags are:\n\r",ch);
		show_help( ch, "trigger");
		return FALSE;
	}

	if(!check_trigger(2,trigger))
	{
		send_to_char("Valid flags are:\n\r",ch);
		show_help( ch, "trigger");
		return FALSE;
	}


	if ( ( code = get_rprog_index ( name ) ) == NULL)
	{
		send_to_char("No such ROOMProgram.\n\r",ch);
		return FALSE;
	}

	list                  = new_prog();
	list->name            = code->name;
	list->trig_type       = value;
	list->trig_phrase     = str_dup(phrase);
	list->code            = code->code;
	list->next            = pRoom->progs;
	pRoom->progs          = list;
	EXT_SET_BIT( pRoom->prog_flags,*value );

	send_to_char( "Rprog Added.\n\r",ch);


	enable_room_update(pRoom, value);
	return TRUE;
}



REDIT ( redit_delprog )
{
	ROOM_INDEX_DATA * pRoom;
	PROG_LIST * prog = NULL, * prog_tmp = NULL, * prog_prev = NULL;
	char oprog[ MAX_STRING_LENGTH ];
	int value;
	int cnt = 0;
	int to_rem = 0;
	BITVECT_DATA * type;

	EDIT_ROOM( ch, pRoom );

	one_argument( argument, oprog );
	if ( !is_number( oprog ) || oprog[0] == '\0' )
	{
		send_to_char( "Sk³adnia: delprog [#rprog]\n\r", ch );
		return FALSE;
	}

	value = atoi ( oprog );

	if ( value < 0 )
	{
		send_to_char( "Only non-negative rprog-numbers allowed.\n\r", ch );
		return FALSE;
	}

	if ( !pRoom->progs )
	{
		send_to_char( "REdit:  None exist rprog.\n\r", ch );
		return FALSE;
	}

	if ( value == 0 )
	{
		for ( prog = pRoom->progs; prog; prog = prog->next )
		{
			if ( prog->trig_type == pRoom->progs->trig_type )
				to_rem++;
		}

		if ( to_rem <= 1 )
			EXT_REMOVE_BIT( pRoom->prog_flags, *pRoom->progs->trig_type );

		type = pRoom->progs->trig_type;
		prog = pRoom->progs;
		pRoom->progs = prog->next;
		free_prog( prog );
	}
	else
	{
		for( prog = pRoom->progs; cnt < value && prog; prog = prog->next, cnt++ )
			prog_prev = prog;

		if ( prog )
		{
			for ( prog_tmp = pRoom->progs; prog_tmp; prog_tmp = prog_tmp->next )
				if ( prog_tmp->trig_type == prog->trig_type )
					to_rem++;

			if ( to_rem <= 1 )
				EXT_REMOVE_BIT( pRoom->prog_flags, *prog->trig_type );

			type = prog->trig_type;
			prog_prev->next = prog->next;
			free_prog( prog );
		}
		else
		{
			send_to_char( "No such rprog.\n\r", ch );
			return FALSE;
		}
	}

	send_to_char( "Rprog removed.\n\r", ch );
	/* wylaczanie w liscie */
	disable_room_update( pRoom, type );

	return TRUE;
}

/*****************************************************************************
Name:		check_range( lower vnum, upper vnum )
Purpose:	Ensures the range spans only one area.
Called by:	aedit_vnum(olc_act.c).
 ****************************************************************************/
bool check_range( int lower, int upper )
{
	AREA_DATA *pArea;
	int cnt = 0;

	for ( pArea = area_first; pArea; pArea = pArea->next )
	{
		/*
		 * lower < area < upper
		 */
		if ( ( lower <= pArea->min_vnum && pArea->min_vnum <= upper )
				||   ( lower <= pArea->max_vnum && pArea->max_vnum <= upper ) )
			++cnt;

		if ( cnt > 1 )
			return FALSE;
	}
	return TRUE;
}

AREA_DATA *get_vnum_area( ush_int vnum )
{
	AREA_DATA *pArea;

	for ( pArea = area_first; pArea; pArea = pArea->next )
	{
		if ( vnum >= pArea->min_vnum
				&& vnum <= pArea->max_vnum )
			return pArea;
	}

	return 0;
}

/*
 * Area Editor Functions.
 */
AEDIT( aedit_show )
{
	AREA_DATA *pArea, *sArea;
	char buf  [MAX_STRING_LENGTH];

	EDIT_AREA(ch, pArea);

	sprintf( buf, "{GName:{x     [%5d] %s\n\r", pArea->vnum, pArea->name );
	send_to_char( buf, ch );

	sprintf( buf, "{GFile:{x     %s\n\r", pArea->file_name );
	send_to_char( buf, ch );

	sprintf( buf, "{GVnums:{x    [%d-%d]\n\r", pArea->min_vnum, pArea->max_vnum );
	send_to_char( buf, ch );

	sprintf( buf, "{GAge:{x      [%d]\n\r",	pArea->age );
	send_to_char( buf, ch );

	sprintf( buf, "{GResetAge:{x [%d]\n\r",	pArea->resetage );
	send_to_char( buf, ch );

	sprintf( buf, "{GPlayers:{x  [%d]\n\r", pArea->nplayer );
	send_to_char( buf, ch );

	sprintf( buf, "{GSecurity:{x [%d]\n\r", pArea->security );
	send_to_char( buf, ch );

	sprintf( buf, "{GBuilders:{x [%s]\n\r", pArea->builders );
	send_to_char( buf, ch );

	if ( !IS_NULLSTR( pArea->locked ) )
	{
		sprintf( buf, "{RLocked{x :  [%s]\n\r", pArea->locked );
		send_to_char( buf, ch );
	}

	sprintf( buf, "{GFlags:{x    [%s]\n\r", flag_string( area_flags, pArea->area_flags ) );
	send_to_char( buf, ch );

	if ( !IS_NULLSTR( pArea->credits ) )
	{
		sprintf( buf, "{GCredits:{x\n\r%s\n\r", pArea->credits );
		send_to_char( buf, ch );
	}

	sprintf( buf, "{GRegion:{x   [%d]\n\r\n\r", pArea->region );
	send_to_char( buf, ch );

	if ( pArea->region != 0 )
	{
		send_to_char( "{CKrainy w tym regionie:{x\n\r\n\r", ch );
		print_char( ch, "[%3s] [%-27s] (%-5s-%5s) [%-10s] %3s [%-10s]\n\r",
				"Num", "Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders" );

		for ( sArea = area_first; sArea; sArea = sArea->next )
		{
			if ( sArea->region == 0 || sArea->region != pArea->region )
				continue;

			print_char( ch, "[%3d] %-*.*s {x(%-5d-%5d) %-12.12s [%d] [%-10.10s]\n\r",
					sArea->vnum,
					29 + count_colors( sArea->name, 0 ),
					29 + count_colors( sArea->name, 0 ),
					sArea->name,
					sArea->min_vnum,
					sArea->max_vnum,
					sArea->file_name,
					sArea->security,
					sArea->builders );
		}
	}

	return FALSE;
}

AEDIT( aedit_reset )
{
	AREA_DATA *pArea;

	EDIT_AREA(ch, pArea);

	reset_area( pArea );
	send_to_char( "Area reset.\n\r", ch );

	return FALSE;
}

AEDIT( aedit_create )
{
	AREA_DATA *pArea;

	pArea               =   new_area();
	area_last->next     =   pArea;
	area_last		=   pArea;
	ch->desc->pEdit     =   (void *)pArea;

	SET_BIT( pArea->area_flags, AREA_ADDED );
	send_to_char( "{GUtworzono now± krainê.{x\n\r", ch );
	return FALSE;
}

AEDIT( aedit_name )
{
	AREA_DATA *pArea;

	EDIT_AREA(ch, pArea);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia:  name [$name]\n\r", ch );
		return FALSE;
	}

	free_string( pArea->name );
	pArea->name = str_dup( argument );

	send_to_char( "{GUstawiono nazwê krainy.{x\n\r", ch );
	return TRUE;
}

AEDIT( aedit_credits )
{
	AREA_DATA *pArea;

	EDIT_AREA(ch, pArea);

	if ( argument[0] == '\0' )
	{
		string_append( ch, &pArea->credits );
		return TRUE;
	}

	send_to_char( "Sk³adnia: credits (edytor tekstowy)\n\r", ch );
	return FALSE;
}

AEDIT( aedit_lock )
{
	AREA_DATA *pArea;

	EDIT_AREA(ch, pArea);

	free_string( pArea->locked );
	pArea->locked = str_dup( argument );

	if ( argument[0] == '\0' )
		send_to_char( "Area unlocked.\n\r", ch );
	else
		send_to_char( "Area locked.\n\r", ch );
	save_area( pArea );
	return FALSE;
}

AEDIT( aedit_file )
{
	AREA_DATA *pArea, *pAList;
	char file[MAX_STRING_LENGTH];
	int i, length;

	EDIT_AREA(ch, pArea);

	one_argument( argument, file );	/* Forces Lowercase */

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: filename [$file]\n\r", ch );
		return FALSE;
	}

	/*
	 * Simple Syntax Check.
	 */
	length = strlen( argument );
	if ( length > 8 )
	{
		send_to_char( "{RW nazwie pliku mo¿e byæ maksymalnie osiem znaków.{x\n\r", ch );
		return FALSE;
	}

	/*
	 * Allow only letters and numbers.
	 */
	for ( i = 0; i < length; i++ )
	{
		if ( !isalnum( file[i] ) )
		{
			send_to_char( "{RNazwa pliku mo¿e zawieraæ tylko litery i cyfry.{x\n\r", ch );
			return FALSE;
		}
	}


	strcat( file, ".are" );

	for ( pAList = area_first; pAList; pAList = pAList->next )
		if ( !str_cmp( pAList->file_name, file ) )
		{
			send_to_char( "{RIstnieje ju¿ kraina z tak± nazw± pliku.{x\n\r", ch );
			return FALSE;
		}

	free_string( pArea->file_name );
	pArea->file_name = str_dup( file );

	send_to_char( "{GNazwa pliku krainy zosta³a zmieniona.{x\n\r", ch );
	return TRUE;
}

AEDIT( aedit_age )
{
	AREA_DATA *pArea;
	char age[MAX_STRING_LENGTH];

	EDIT_AREA(ch, pArea);

	one_argument( argument, age );

	if ( !is_number( age ) || age[0] == '\0' )
	{
		send_to_char( "Sk³adnia: age [#xage]\n\r", ch );
		return FALSE;
	}

	pArea->age = atoi( age );

	send_to_char( "{GCzas od¶wierzania krainy zosta³ zmieniony.{x\n\r", ch );
	return TRUE;
}

AEDIT( aedit_resetage )
{
	AREA_DATA *pArea;
	char resetage[MAX_STRING_LENGTH];

	EDIT_AREA(ch, pArea);

	one_argument( argument, resetage );

	if ( !is_number( resetage ) || resetage[0] == '\0' )
	{
		send_to_char( "Sk³adnia: resetage [#xresetage]\n\r", ch );
		return FALSE;
	}

	pArea->resetage = atoi( resetage );

	send_to_char( "Reset age set.\n\r", ch );
	return TRUE;
}

AEDIT( aedit_security )
{
	AREA_DATA *pArea;
	char sec[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int  value;

	EDIT_AREA(ch, pArea);

	one_argument( argument, sec );

	if ( !is_number( sec ) || sec[0] == '\0' )
	{
		send_to_char( "Sk³adnia: security [#xlevel]\n\r", ch );
		return FALSE;
	}

	value = atoi( sec );

	if ( value > ch->pcdata->security || value < 0 )
	{
		if ( ch->pcdata->security != 0 )
		{
			sprintf( buf, "Security is 0-%d.\n\r", ch->pcdata->security );
			send_to_char( buf, ch );
		}
		else
			send_to_char( "Security is 0 only.\n\r", ch );
		return FALSE;
	}

	pArea->security = value;

	send_to_char( "Security set.\n\r", ch );
	return TRUE;
}

AEDIT( aedit_region )
{
	AREA_DATA *pArea, *sArea;

	EDIT_AREA( ch, pArea );

	if ( !is_number( argument ) || argument[ 0 ] == '\0' )
	{
		send_to_char( "Sk³adnia: region [#region number]\n\r"
				"Je¶li chcesz po³±czyæ kilka krain w jedn± ca³o¶æ je¶li chodzi o komendy track\n\r"
				"i yell to ustaw im ten sam numer regionu. Ustaw region jako 0 je¶li nie\n\r"
				"nie chcesz ³±czyæ tej krainy z innymi.\n\r", ch );
		return FALSE;
	}

	pArea->region = atoi( argument );

	if ( pArea->region == 0 )
	{
		send_to_char( "Region cleared.\n\r", ch );
		return TRUE;
	}

	send_to_char( "Region set.\n\r\n\r", ch );

	send_to_char( "{CKrainy w tym regionie:{x\n\r\n\r", ch );
	print_char( ch, "[%3s] [%-27s] (%-5s-%5s) [%-10s] %3s [%-10s]\n\r",
			"Num", "Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders" );

	for ( sArea = area_first; sArea; sArea = sArea->next )
	{
		if ( sArea->region == 0 || sArea->region != pArea->region )
			continue;

		print_char( ch, "[%3d] %-*.*s {x(%-5d-%5d) %-12.12s [%d] [%-10.10s]\n\r",
				sArea->vnum,
				29 + count_colors( sArea->name, 0 ),
				29 + count_colors( sArea->name, 0 ),
				sArea->name,
				sArea->min_vnum,
				sArea->max_vnum,
				sArea->file_name,
				sArea->security,
				sArea->builders );
	}
	return TRUE;
}

AEDIT( aedit_builder )
{
	AREA_DATA *pArea;
	char name[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];

	EDIT_AREA(ch, pArea);

	one_argument( argument, name );

	if ( name[0] == '\0' )
	{
		send_to_char( "Sk³adnia: builder [$name]  -toggles builder\n\r", ch );
		send_to_char( "Sk³adnia: builder All      -allows everyone\n\r", ch );
		return FALSE;
	}

	name[0] = UPPER( name[0] );

	if ( strstr( pArea->builders, name ) != '\0' )
	{
		pArea->builders = string_replace( pArea->builders, name, "\0" );
		pArea->builders = string_unpad( pArea->builders );

		if ( pArea->builders[0] == '\0' )
		{
			free_string( pArea->builders );
			pArea->builders = str_dup( "None" );
		}
		send_to_char( "Builder removed.\n\r", ch );
		return TRUE;
	}
	else
	{
		buf[0] = '\0';
		if ( strstr( pArea->builders, "None" ) != '\0' )
		{
			pArea->builders = string_replace( pArea->builders, "None", "\0" );
			pArea->builders = string_unpad( pArea->builders );
		}

		if (pArea->builders[0] != '\0' )
		{
			strcat( buf, pArea->builders );
			strcat( buf, " " );
		}
		strcat( buf, name );
		free_string( pArea->builders );
		pArea->builders = string_proper( str_dup( buf ) );

		send_to_char( "Builder added.\n\r", ch );
		send_to_char( pArea->builders,ch);
		send_to_char( "\n\r", ch); //Brohacz: typo
		return TRUE;
	}

	return FALSE;
}

AEDIT( aedit_vnum )
{
	AREA_DATA *pArea;
	char buf  [MAX_STRING_LENGTH];
	char lower[MAX_STRING_LENGTH];
	char upper[MAX_STRING_LENGTH];
	int  ilower;
	int  iupper;

	EDIT_AREA(ch, pArea);

	argument = one_argument( argument, lower );
	one_argument( argument, upper );

	if ( !is_number( lower ) || lower[0] == '\0'
			|| !is_number( upper ) || upper[0] == '\0' )
	{
		send_to_char( "Sk³adnia: vnum [#xlower] [#xupper]\n\r", ch );
		return FALSE;
	}

	if ( ( ilower = atoi( lower ) ) > ( iupper = atoi( upper ) ) )
	{
		send_to_char( "AEdit:  Upper must be larger then lower.\n\r", ch );
		return FALSE;
	}

	if( ilower <= 0 || iupper > 65535)
	{
		send_to_char( "AEdit:  Range must be from 0 - 65535.\n\r", ch );
		return FALSE;
	}

	if ( !check_range( atoi( lower ), atoi( upper ) ) )
	{
		send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
		return FALSE;
	}

	if ( get_vnum_area( ilower )
			&& get_vnum_area( ilower ) != pArea )
	{
		send_to_char( "AEdit:  Lower vnum already assigned.\n\r", ch );
		return FALSE;
	}

	pArea->min_vnum = ilower;
	send_to_char( "Lower vnum set.\n\r", ch );

	if ( get_vnum_area( iupper )
			&& get_vnum_area( iupper ) != pArea )
	{
		send_to_char( "AEdit:  Upper vnum already assigned.\n\r", ch );
		return TRUE;	/* The lower value has been set. */
	}

	pArea->max_vnum = iupper;
	send_to_char( "Upper vnum set.\n\r", ch );
        sprintf( buf, "Setnieto %d vnumow.\n\r", iupper-ilower+1 );
	send_to_char( buf ,ch );

	return TRUE;
}

AEDIT( aedit_lvnum )
{
	AREA_DATA *pArea;
	char lower[MAX_STRING_LENGTH];
	int  ilower;
	int  iupper;

	EDIT_AREA(ch, pArea);

	one_argument( argument, lower );

	if ( !is_number( lower ) || lower[0] == '\0' )
	{
		send_to_char( "Sk³adnia: min_vnum [#xlower]\n\r", ch );
		return FALSE;
	}

	if ( ( ilower = atoi( lower ) ) > ( iupper = pArea->max_vnum ) )
	{
		send_to_char( "AEdit:  Value must be less than the max_vnum.\n\r", ch );
		return FALSE;
	}

	if ( !check_range( ilower, iupper ) )
	{
		send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
		return FALSE;
	}

	if ( get_vnum_area( ilower )
			&& get_vnum_area( ilower ) != pArea )
	{
		send_to_char( "AEdit:  Lower vnum already assigned.\n\r", ch );
		return FALSE;
	}

	pArea->min_vnum = ilower;
	send_to_char( "Lower vnum set.\n\r", ch );
	return TRUE;
}



AEDIT( aedit_uvnum )
{
	AREA_DATA *pArea;
	char upper[MAX_STRING_LENGTH];
	int  ilower;
	int  iupper;

	EDIT_AREA(ch, pArea);

	one_argument( argument, upper );

	if ( !is_number( upper ) || upper[0] == '\0' )
	{
		send_to_char( "Sk³adnia: max_vnum [#xupper]\n\r", ch );
		return FALSE;
	}

	if ( ( ilower = pArea->min_vnum ) > ( iupper = atoi( upper ) ) )
	{
		send_to_char( "AEdit:  Upper must be larger then lower.\n\r", ch );
		return FALSE;
	}

	if ( !check_range( ilower, iupper ) )
	{
		send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
		return FALSE;
	}

	if ( get_vnum_area( iupper )
			&& get_vnum_area( iupper ) != pArea )
	{
		send_to_char( "AEdit:  Upper vnum already assigned.\n\r", ch );
		return FALSE;
	}

	pArea->max_vnum = iupper;
	send_to_char( "Upper vnum set.\n\r", ch );

	return TRUE;
}



/*
 * Room Editor Functions.
 */
REDIT( redit_show )
{
	ROOM_INDEX_DATA	*pRoom;
	char		buf  [MAX_STRING_LENGTH];
	char		buf1 [2*MAX_STRING_LENGTH];
	OBJ_DATA		*obj;
	CHAR_DATA		*rch;
	int			door;
	bool		fcnt;
	PROG_LIST *list;
	int cnt = 0;
	int i;

/* wywla muda
    if ( !IS_BUILDER( ch, pRoom->area ) )
    {
        send_to_char( "Masz za niski poziom security do ogladania szczegolow tej lokacji.\n\r", ch );
        return FALSE;
    }
*/

	EDIT_ROOM(ch, pRoom);

	buf1[0] = '\0';

	sprintf( buf, "{GDescription{x:\n\r%s", pRoom->description );
	strcat( buf1, buf );

	if ( pRoom->nightdesc[0] != '\0' )
	{
		sprintf( buf, "{GNight Description{x:\n\r%s", pRoom->nightdesc );
		strcat( buf1, buf );
	}

	sprintf( buf, "{GName{x:       [%s]\n\r{GArea{x:       [%5d] %s\n\r",
		pRoom->name, pRoom->area->vnum, pRoom->area->name );
	strcat( buf1, buf );

	sprintf( buf, "{GVnum{x:       [%5d]\n\r{GSector{x:     [%s]\n\r",
		pRoom->vnum, sector_table[pRoom->sector_type].name);
	strcat( buf1, buf );

	sprintf( buf, "{GRoom flags{x: [%s]\n\r",
		ext_flag_string( room_flags, pRoom->room_flags ) );
	strcat( buf1, buf );

	if ( pRoom->heal_rate != 100 )
	{
		sprintf( buf, "{GHealth rec{x: [%d]\n\r",
			pRoom->heal_rate);
		strcat( buf1, buf );
	}

	if ( pRoom->echo_to.room )
	{
		sprintf( buf, "{GEcho to{x:    [%d]\n\r", pRoom->echo_to.room->vnum );
		strcat( buf1, buf );
	}

	if ( pRoom->rent_rate != 100 )
	{
		sprintf( buf, "{GRent rate{x: [%d]\n\r",
			pRoom->rent_rate);
		strcat( buf1, buf );
	}

	if ( pRoom->day_rand_desc > 0 )
	{
		sprintf( buf, "{GDay Rand desc{x: [%d]",
			pRoom->day_rand_desc);
		strcat( buf1, buf );
	}

	if ( pRoom->night_rand_desc > 0 )
	{
		sprintf( buf, "   {GNight Rand desc{x: [%d]\n\r",
			pRoom->night_rand_desc);
		strcat( buf1, buf );
	} else
		if ( pRoom->day_rand_desc > 0 )
			strcat( buf1, "\n\r" );

	if ( !IS_NULLSTR(pRoom->owner) )
	{
		sprintf( buf, "{GOwner{x     : [%s]\n\r", pRoom->owner );
		strcat( buf1, buf );
	}

	if ( pRoom->extra_descr )
	{
		EXTRA_DESCR_DATA *ed;

		strcat( buf1, "{GExDesc Kwd{x: [" );
		for ( ed = pRoom->extra_descr; ed; ed = ed->next )
		{
			strcat( buf1, ed->keyword );
			if ( ed->next )
				strcat( buf1, " " );
		}
		strcat( buf1, "]\n\r" );
	}

	strcat( buf1, "{GCharacters{x: [" );
	fcnt = FALSE;
	for ( rch = pRoom->people; rch; rch = rch->next_in_room )
	{
		one_argument( rch->name, buf );
		strcat( buf1, buf );
		strcat( buf1, " " );
		fcnt = TRUE;
	}

	if ( fcnt )
	{
		int end;

		end = strlen(buf1) - 1;
		buf1[end] = ']';
		strcat( buf1, "\n\r" );
	}
	else
		strcat( buf1, "none]\n\r" );

	strcat( buf1, "{GObjects{x:    [" );
	fcnt = FALSE;
	for ( obj = pRoom->contents; obj; obj = obj->next_content )
	{
		one_argument( obj->name, buf );
		strcat( buf1, buf );
		strcat( buf1, " " );
		fcnt = TRUE;
	}

	if ( fcnt )
	{
		int end;

		end = strlen(buf1) - 1;
		buf1[end] = ']';
		strcat( buf1, "\n\r" );
	}
	else
		strcat( buf1, "none]\n\r" );


	if( pRoom->trap > 0 )
	{
		sprintf(buf,"{GTrap vnum{x:  [%4d]\n\r", pRoom->trap);
		strcat(buf1, buf);
	}

	//rellik: mining
	buf[0] = '\0';
	strcat( buf1, "{GMinera³y:{x \n\r" );
	for ( i = 0; rawmaterial_table[i].name; ++i )
	{
		if ( pRoom->rawmaterial[i] > 0 )
		{
			sprintf( buf, "%s: [%d]       ", rawmaterial_table[i].name, pRoom->rawmaterial[i] );
			strcat( buf1, buf );
		}
	}
	sprintf( buf, "\n\r{GZasobno¶æ ¿y³y:{x     [%d]\n\r", pRoom->rawmaterial_capacity );
	strcat( buf1, buf );


	for ( door = 0; door < MAX_DIR; door++ )
	{
		EXIT_DATA *pexit;

		if ( ( pexit = pRoom->exit[door] ) )
		{
			char word[MAX_INPUT_LENGTH];
			char reset_state[MAX_STRING_LENGTH];
			char *state;
			int i, length;

			sprintf( buf, "- {C%-8s{x to [%5d] {GKey{x: [%5d] ",
				capitalize(dir_name[door]),
				pexit->u1.to_room ? pexit->u1.to_room->vnum : 0,      /* ROM OLC */
				pexit->key );

			strcat( buf1, buf );

			/*
			* Format up the exit info.
			* Capitalize all flags that are not part of the reset info.
			*/
			strcpy( reset_state, flag_string( exit_flags, pexit->rs_flags ) );
			state = flag_string( exit_flags, pexit->exit_info );
			strcat( buf1, " {GExit flags{x: [" );
			for (;;)
			{
				state = one_argument( state, word );

				if ( word[0] == '\0' )
				{
					int end;

					end = strlen(buf1) - 1;
					buf1[end] = ']';
					strcat( buf1, "\n\r" );
					break;
				}

				if ( str_infix( word, reset_state ) )
				{
					length = strlen(word);
					for (i = 0; i < length; i++)
						word[i] = UPPER(word[i]);
				}
				strcat( buf1, word );
				strcat( buf1, " " );
			}

			if ( pexit->keyword && pexit->keyword[0] != '\0' )
			{
				sprintf( buf, "{GKeyword{x: %s\n\r", pexit->keyword );
				strcat( buf1, buf );
			}
			if( pexit->biernik && pexit->biernik[0] != '\0' )
			{
				sprintf( buf, "{GBiernik{x: %s\n\r", pexit->biernik );
				strcat( buf1, buf );
			}
			if ( pexit->vName && pexit->vName[0] != '\0' )
			{
				sprintf( buf, "{GVirtual name{x: %s\n\r", pexit->vName );
				strcat( buf1, buf );
			}
			if ( pexit->TextOut && pexit->TextOut[0] != '\0' )
			{
				sprintf( buf, "{GTextOut{x: %s\n\r", pexit->TextOut );
				strcat( buf1, buf );
			}
			if ( pexit->TextIn && pexit->TextIn[0] != '\0' )
			{
				sprintf( buf, "{GTextIn{x: %s\n\r", pexit->TextIn );
				strcat( buf1, buf );
			}
			if ( pexit->description && pexit->description[0] != '\0' )
			{
				sprintf( buf, "{GDesc{x: %s", pexit->description );
				strcat( buf1, buf );
			}
			if ( pexit->nightdescription && pexit->nightdescription[0] != '\0' )
			{
				sprintf( buf, "{GNightdesc{x: %s", pexit->nightdescription );
				strcat( buf1, buf );
			}
			if( pexit->trap > 0 )
			{
				sprintf( buf, "{GTrap{x:[ %5d]\n\r", pexit->trap);
				strcat( buf1, buf );
			}
			if( IS_SET( pexit->exit_info, EX_ISDOOR ) )
			{
				if( pexit->liczba_mnoga )
				{
					sprintf( buf, "Nazwa wyj¶cia odmienia siê w liczbie mnogiej.\n\r" );
					strcat( buf1, buf );
				}
				else
				{
					sprintf( buf, "Nazwa wyj¶cia odmienia siê w liczbie pojedyñczej.\n\r" );
					strcat( buf1, buf );
				}
			}
			strcat( buf1, "\n\r" );
			cnt++;
		}
	}

	send_to_char( buf1, ch );

	if ( pRoom->progs )
	{

		if ( cnt == 0 )
			send_to_char( "\n\r", ch );

		cnt = 0;

		sprintf(buf, "{CROOMPrograms for [%5d]{x:\n\r", pRoom->vnum);
		send_to_char( buf, ch );

		for (cnt=0, list=pRoom->progs; list; list=list->next)
		{
			if (cnt ==0)
			{
				send_to_char ( " Number Name            Trigger    Phrase\n\r", ch );
				send_to_char ( " ------ --------------- ---------- ---------\n\r", ch );
			}

			sprintf(buf, "[%5d] %-15s {C%-10s{x %s\n\r", cnt,
				list->name,ext_bit_name( prog_flags, list->trig_type ),
				list->trig_phrase);
			send_to_char( buf, ch );
			cnt++;
		}
	}
	return FALSE;
}




/* Local function. */
bool change_exit( CHAR_DATA *ch, char *argument, int door )
{
	ROOM_INDEX_DATA *pRoom;
	EXIT_DATA *pExit;
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char fullarg[MAX_INPUT_LENGTH];
	int  value;

	EDIT_ROOM(ch, pRoom);

	sprintf( fullarg, "%s", argument );

	/*
	 * Now parse the arguments.
	 */
	argument = one_argument( argument, command );
	one_argument( argument, arg );

	if ( command[0] == '\0' && argument[0] == '\0' )	/* Move command. */
	{
		move_char( ch, door, TRUE, NULL );                    /* ROM OLC */
		return FALSE;
	}

	if ( command[0] == '?' )
	{
		do_help( ch, "EXIT" );
		return FALSE;
	}

	if ( !str_cmp( command, "delete" ) )
	{
		ROOM_INDEX_DATA *pToRoom;
		sh_int rev;                                     /* ROM OLC */

		if ( !pRoom->exit[door] )
		{
			send_to_char( "REdit:  Nie mo¿esz usun±æ wyj¶cia, którego nie ma.\n\r", ch );
			return FALSE;
		}

		/*
		 * Remove ToRoom Exit.
		 */
		rev = rev_dir[door];
		pToRoom = pRoom->exit[door]->u1.to_room;       /* ROM OLC */

		if ( pToRoom->exit[rev] )
		{
			free_exit( pToRoom->exit[rev] );
			pToRoom->exit[rev] = NULL;
			SET_BIT( pToRoom->area->area_flags, AREA_CHANGED );
		}

		/*
		 * Remove this exit.
		 */
		free_exit( pRoom->exit[door] );
		pRoom->exit[door] = NULL;

		send_to_char( "Po³±czenie zosta³o usuniête.\n\r", ch );
		return TRUE;
	}

	if ( !str_cmp( command, "link" ) )
	{
		EXIT_DATA *pExit;
		ROOM_INDEX_DATA *toRoom;

		if ( arg[0] == '\0' || !is_number( arg ) )
		{
			send_to_char( "REdit: Sk³adnia: [kierunek] link [vnum]\n\r", ch );
			return FALSE;
		}

		value = atoi( arg );

		if ( ! (toRoom = get_room_index( value )) )
		{
			send_to_char( "REdit: Room o podanym vnumie nie istnieje.\n\r", ch );
			return FALSE;
		}

		if ( !IS_BUILDER( ch, toRoom->area ) )
		{
			send_to_char( "REdit:  Vnum nie nale¿y do krainki. Masz za ma³e security.\n\r", ch );
			return FALSE;
		}

		if ( toRoom->exit[rev_dir[door]] )
		{
			send_to_char( "REdit:  Podany kierunek ma ju¿ wyj¶cie.\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
			pRoom->exit[door] = new_exit();

		pRoom->exit[door]->u1.to_room = toRoom;
		pRoom->exit[door]->orig_door = door;

		door                    = rev_dir[door];
		pExit                   = new_exit();
		pExit->u1.to_room       = pRoom;
		pExit->orig_door	= door;
		toRoom->exit[door]       = pExit;
		SET_BIT( toRoom->area->area_flags, AREA_CHANGED );

		send_to_char( "Po³±czenie dwustronne zosta³o stworzone.\n\r", ch );
		return TRUE;
	}

	if ( !str_cmp( command, "dig" ) )
	{
		char buf[MAX_STRING_LENGTH];

		if ( arg[0] == '\0' || !is_number( arg ) )
		{
			send_to_char( "REdit: Sk³adnia: [kierunek] dig <vnum>\n\r", ch );
			return FALSE;
		}

		redit_create( ch, arg );
		sprintf( buf, "link %s", arg );
		change_exit( ch, buf, door);
		return TRUE;
	}

	if ( !str_cmp( command, "trap" ) )
	{
		if ( arg[0] == '\0' || !is_number( arg ) )
		{
			send_to_char( "REdit: Sk³adnia: [kierunek] trap <vnum>\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
		{
			send_to_char("REdit: Nie znaleziono wyj¶cia.\n\r",ch);
			return FALSE;
		}

		if( !IS_SET(pRoom->exit[door]->rs_flags, EX_ISDOOR ))
		{
			send_to_char("REdit: Exit musi mieæ flagê 'door'.\n\r",ch);
			return FALSE;
		}


		value = UMAX(0, atoi( arg ));

		if ( value > 0 && !get_trap_index( value ) )
		{
			send_to_char( "REdit: Pu³apka nie istnieje.\n\r", ch );
			return FALSE;
		}

		pRoom->exit[door]->trap = value;
		SET_BIT(pRoom->exit[door]->exit_info, EX_TRAP);
		send_to_char("Trap set.\n\r", ch);
		return TRUE;
	}

	if ( !str_cmp( command, "room" ) )
	{
		ROOM_INDEX_DATA *toRoom;

		if ( arg[0] == '\0' || !is_number( arg ) )
		{
			send_to_char( "Sk³adnia: [kierunek] room [vnum]\n\r", ch );
			return FALSE;
		}

		value = atoi( arg );

		if ( !(toRoom = get_room_index( value )) )
		{
			send_to_char( "REdit:  Nie mo¿na utworzyæ po³±czenia z roomem który nie istnieje.\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
			pRoom->exit[door] = new_exit();

		pRoom->exit[door]->u1.to_room = toRoom;    /* ROM OLC */
		pRoom->exit[door]->orig_door = door;

		send_to_char( "Po³±czenie jednostronne zosta³o stworzone.\n\r", ch );
		return TRUE;
	}

	if ( !str_cmp( command, "key" ) )
	{
		OBJ_INDEX_DATA *key;

		if ( arg[0] == '\0' || !is_number( arg ) )
		{
			send_to_char( "Sk³adnia: [direction] key [vnum]\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
		{
			send_to_char("Nie znaleziono wyj¶cia.\n\r",ch);
			return FALSE;
		}

		value = atoi( arg );

		if ( !(key = get_obj_index( value )) )
		{
			send_to_char( "REdit:  Klucz nie istnieje.\n\r", ch );
			return FALSE;
		}

		if ( key->item_type != ITEM_KEY )
		{
			send_to_char( "REdit:  Przedmiot nie jest kluczem.\n\r", ch );
			return FALSE;
		}

		pRoom->exit[door]->key = value;

		send_to_char( "Klucz wymagany do drzwi ustawiony.\n\r", ch );
		return TRUE;
	}

	if ( !str_cmp( command, "name" ) )
	{
		if ( arg[0] == '\0' )
		{
			do_help( ch, "EXIT_name" );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
		{
			send_to_char("Wybrany kierunek nie istnieje, wiêc nie mo¿esz tego ustawiæ.\n\r",ch);
			return FALSE;
		}

		free_string( pRoom->exit[door]->keyword );

		if (str_cmp(arg,"none"))
			pRoom->exit[door]->keyword = str_dup( arg );
		else
			pRoom->exit[door]->keyword = str_dup( "" );

		send_to_char( "Exit name set.\n\r", ch );
		return TRUE;
	}

	if ( !str_cmp( command, "biernik" ) )
	{
		if ( arg[0] == '\0' )
		{
			send_to_char( "Sk³adnia: [kierunek] biernik [string]\n\r", ch );
			send_to_char( "         [kierunek] biernik none\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
		{
			send_to_char("Wybrany kierunek nie istnieje, wiêc nie mo¿esz tego ustawiæ.\n\r",ch);
			return FALSE;
		}

		free_string( pRoom->exit[door]->biernik );

		if (str_cmp(arg,"none"))
			pRoom->exit[door]->biernik = str_dup( arg );
		else
			pRoom->exit[door]->biernik = str_dup( "" );

		send_to_char( "Exit biernik set.\n\r", ch );
		return TRUE;
	}

	if ( !str_cmp( command, "liczba" ) )
	{
		if ( arg[0] == '\0' )
		{
			send_to_char( "Sk³adnia: [kierunek] liczba mnoga\n\r", ch );
			send_to_char( "          [kierunek] liczba pojedyñcza\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
		{
			send_to_char("Wybrany kierunek nie istnieje, wiêc nie mo¿esz tego ustawiæ.\n\r",ch);
			return FALSE;
		}

		if (!str_prefix(arg,"mnoga"))
		{
			pRoom->exit[door]->liczba_mnoga = TRUE;
			send_to_char( "Liczba mnoga ustawiona.\n\r", ch );
		}
		else
		{
			pRoom->exit[door]->liczba_mnoga = FALSE;
			send_to_char( "Liczba pojedyñcza ustawiona.\n\r", ch );
		}
		return TRUE;
	}

	if ( !str_cmp( command, "vname" ) )
	{
		if ( arg[0] == '\0' )
		{
			send_to_char( "Sk³adnia: [kierunek] vname [string]\n\r", ch );
			send_to_char( "          [kierunek] vname none\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
		{
			send_to_char("Nie ma wyj¶cia w tym kierunku.\n\r",ch);
			return FALSE;
		}

		free_string( pRoom->exit[door]->vName );

		if (str_cmp(argument,"none"))
			pRoom->exit[door]->vName = str_dup( argument );
		else
			pRoom->exit[door]->vName = str_dup( "" );

		send_to_char( "Vname ustawiony.\n\r", ch );
		return TRUE;
	}

	if ( !str_cmp( command, "textout" ) )
	{
		if ( arg[0] == '\0' )
		{
			send_to_char( "Sk³adnia: [kierunek] textout [string]\n\r"
					"         [kierunek] textout none\n\r"
					"Tekst wy¶wietlany stoj±cym w lokacji je¶li kto¶ pójdzie w tym kierunku.\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
		{
			send_to_char("Wybrany kierunek nie istnieje, wiêc nie mo¿esz tego ustawiæ.\n\r",ch);
			return FALSE;
		}

		free_string( pRoom->exit[door]->TextOut );

		if (str_cmp(argument,"none"))
			pRoom->exit[door]->TextOut = str_dup( argument );
		else
			pRoom->exit[door]->TextOut = str_dup( "" );

		send_to_char( "TextOut ustawiony.\n\r", ch );
		return TRUE;
	}

	if ( !str_cmp( command, "textin" ) )
	{
		if ( arg[0] == '\0' )
		{
			send_to_char( "Sk³adnia: [direction] textin [string]\n\r"
					"         [direction] textin none\n\r"
					"Tekst wy¶wietlany stoj±cym w lokacji docelowej je¶li kto¶ pójdzie w tym kierunku.\n\r", ch );
			return FALSE;
		}

		if ( !pRoom->exit[door] )
		{
			send_to_char("Wybrany kierunek nie istnieje, wiêc nie mo¿esz tego ustawiæ.\n\r",ch);
			return FALSE;
		}

		pExit = pRoom->exit[door];
		free_string( pExit->TextIn );

		if (str_cmp(argument,"none"))
			pRoom->exit[door]->TextIn = str_dup( argument );
		else
			pRoom->exit[door]->TextIn = str_dup( "" );

		send_to_char( "TextIn ustawiony.\n\r", ch );
		return TRUE;
	}

	if ( !str_prefix( command, "description" ) )
	{
		if ( arg[0] == '\0' )
		{
			if ( !pRoom->exit[door] )
			{
				send_to_char("Wybrany kierunek nie istnieje, wiêc nie mo¿esz tego ustawiæ.\n\r",ch);
				return FALSE;
			}

			string_append( ch, &pRoom->exit[door]->description );
			return TRUE;
		}
		send_to_char( "Sk³adnia: [kierunek] desc\n\r", ch );
		return FALSE;
	}

	if ( !str_prefix( command, "nightdescription" ) )
	{
		if ( arg[0] == '\0' )
		{
			if ( !pRoom->exit[door] )
			{
				send_to_char("Wybrany kierunek nie istnieje, wiêc nie mo¿esz tego ustawiæ.\n\r",ch);
				return FALSE;
			}

			string_append( ch, &pRoom->exit[door]->nightdescription );
			return TRUE;
		}

		send_to_char( "Sk³adnia: [kierunek] nightdesc\n\r", ch );
		return FALSE;
	}

	/*
	 * Set the exit flags, needs full argument.
	 * ----------------------------------------
	 */


	if ( ( value = flag_value( exit_flags, fullarg ) ) != NO_FLAG )
	{
		ROOM_INDEX_DATA *pToRoom;
		sh_int rev;                                    /* ROM OLC */

		if ( !pRoom->exit[door] )
		{
			send_to_char("No such exit.\n\r",ch);
			return FALSE;
		}

		/*
		 * This room.
		 */
		TOGGLE_BIT(pRoom->exit[door]->rs_flags,  value);
		/* Don't toggle exit_info because it can be changed by players. */
		pRoom->exit[door]->exit_info = pRoom->exit[door]->rs_flags;

		/*
		 * Connected room.
		 */
		pToRoom = pRoom->exit[door]->u1.to_room;     /* ROM OLC */
		rev = rev_dir[door];

		if (pToRoom->exit[rev] != NULL)
		{
			pToRoom->exit[rev]->rs_flags = pRoom->exit[door]->rs_flags;
			pToRoom->exit[rev]->exit_info = pRoom->exit[door]->exit_info;
			SET_BIT( pToRoom->area->area_flags, AREA_CHANGED );
		}

		send_to_char( "Exit flag toggled.\n\r", ch );
		return TRUE;
	}

	return FALSE;
}



REDIT( redit_north )
{
	if ( change_exit( ch, argument, DIR_NORTH ) )
		return TRUE;

	return FALSE;
}



REDIT( redit_south )
{
	if ( change_exit( ch, argument, DIR_SOUTH ) )
		return TRUE;

	return FALSE;
}



REDIT( redit_east )
{
	if ( change_exit( ch, argument, DIR_EAST ) )
		return TRUE;

	return FALSE;
}



REDIT( redit_west )
{
	if ( change_exit( ch, argument, DIR_WEST ) )
		return TRUE;

	return FALSE;
}



REDIT( redit_up )
{
	if ( change_exit( ch, argument, DIR_UP ) )
		return TRUE;

	return FALSE;
}



REDIT( redit_down )
{
	if ( change_exit( ch, argument, DIR_DOWN ) )
		return TRUE;

	return FALSE;
}



REDIT( redit_ed )
{
	ROOM_INDEX_DATA *pRoom;
	EXTRA_DESCR_DATA *ed;
	char command[MAX_INPUT_LENGTH];
	char keyword[MAX_INPUT_LENGTH];

	EDIT_ROOM(ch, pRoom);

	argument = one_argument( argument, command );
	one_argument( argument, keyword );

	if ( command[0] == '\0' || keyword[0] == '\0' )
	{
		send_to_char( "Sk³adnia: ed add [keyword]\n\r", ch );
		send_to_char( "         ed edit [keyword]\n\r", ch );
		send_to_char( "         ed delete [keyword]\n\r", ch );
		send_to_char( "         ed format [keyword]\n\r", ch );
		return FALSE;
	}

	if ( !str_cmp( command, "add" ) )
	{
		if ( keyword[0] == '\0' )
		{
			send_to_char( "Sk³adnia: ed add [keyword]\n\r", ch );
			return FALSE;
		}

		ed			=   new_extra_descr();
		ed->keyword		=   str_dup( keyword );
		ed->description		=   str_dup( "" );
		ed->next		=   pRoom->extra_descr;
		pRoom->extra_descr	=   ed;

		string_append( ch, &ed->description );

		return TRUE;
	}


	if ( !str_cmp( command, "edit" ) )
	{
		if ( keyword[0] == '\0' )
		{
			send_to_char( "Sk³adnia: ed edit [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pRoom->extra_descr; ed; ed = ed->next )
		{
			if ( is_name( keyword, ed->keyword ) )
				break;
		}

		if ( !ed )
		{
			send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
			return FALSE;
		}

		string_append( ch, &ed->description );

		return TRUE;
	}


	if ( !str_cmp( command, "delete" ) )
	{
		EXTRA_DESCR_DATA *ped = NULL;

		if ( keyword[0] == '\0' )
		{
			send_to_char( "Sk³adnia: ed delete [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pRoom->extra_descr; ed; ed = ed->next )
		{
			if ( is_name( keyword, ed->keyword ) )
				break;
			ped = ed;
		}

		if ( !ed )
		{
			send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
			return FALSE;
		}

		if ( !ped )
			pRoom->extra_descr = ed->next;
		else
			ped->next = ed->next;

		free_extra_descr( ed );

		send_to_char( "Extra description deleted.\n\r", ch );
		return TRUE;
	}


	if ( !str_cmp( command, "format" ) )
	{
		if ( keyword[0] == '\0' )
		{
			send_to_char( "Sk³adnia: ed format [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pRoom->extra_descr; ed; ed = ed->next )
		{
			if ( is_name( keyword, ed->keyword ) )
				break;
		}

		if ( !ed )
		{
			send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
			return FALSE;
		}

		ed->description = format_string( ed->description );

		send_to_char( "Extra description formatted.\n\r", ch );
		return TRUE;
	}

	redit_ed( ch, "" );
	return FALSE;
}



REDIT( redit_create )
{
	AREA_DATA *pArea;
	ROOM_INDEX_DATA *pRoom;
	int value;
	int iHash;
	int i;

	EDIT_ROOM(ch, pRoom);

	value = atoi( argument );

	if ( argument[0] == '\0' || value <= 0 )
	{
		send_to_char( "Sk³adnia: create [vnum > 0]\n\r", ch );
		return FALSE;
	}

	pArea = get_vnum_area( value );
	if ( !pArea )
	{
		send_to_char( "REdit:  Vnum nie jest przypisany do krainy.\n\r", ch );
		return FALSE;
	}

	if ( !IS_BUILDER( ch, pArea ) )
	{
		send_to_char( "REdit:  Vnum nale¿y do innej krainy. Za ma³e security.\n\r", ch );
		return FALSE;
	}

	if ( !IS_NULLSTR( pArea->locked ) )
	{
		print_char(ch, "Edycja tej krainy jest aktualnie zablokowana. Powód:\n\r%s\n\r", pArea->locked );
		return FALSE;
	}

	if ( get_room_index( value ) )
	{
		send_to_char( "REdit:  Room o takim vnumie ju¿ istnieje.\n\r", ch );
		return FALSE;
	}

	pRoom			= new_room_index();
	pRoom->area			= pArea;
	pRoom->vnum			= value;

	if ( value > top_vnum_room )
		top_vnum_room = value;

	iHash			= value % MAX_KEY_HASH;
	pRoom->next			= room_index_hash[iHash];
	room_index_hash[iHash]	= pRoom;
	ch->desc->pEdit		= (void *)pRoom;

	//rellik: mining, tak by siê upewniæ ¿e nie bêdzie losowych minera³ów
	for( i = 0; rawmaterial_table[i].name; ++i )
	{
		pRoom->rawmaterial[i] = 0;
	}
	pRoom->rawmaterial_capacity = 0;

	send_to_char( "Room created.\n\r", ch );
	return TRUE;
}



REDIT( redit_name )
{
	ROOM_INDEX_DATA *pRoom;

	EDIT_ROOM(ch, pRoom);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: name [name]\n\r", ch );
		return FALSE;
	}

	free_string( pRoom->name );
	pRoom->name = str_dup( argument );

	send_to_char( "Name set.\n\r", ch );
	return TRUE;
}

REDIT( redit_areapart )
{
	ROOM_INDEX_DATA *pRoom;

	EDIT_ROOM(ch, pRoom);

	if ( !is_number( argument ) || argument[ 0 ] == '\0' )
	{
		send_to_char( "Sk³adnia: areapart [#part number]\n\r", ch );
		return FALSE;
	}

	pRoom->area_part = atoi( argument );

	if ( pRoom->area_part == 0 )
	{
		send_to_char( "Area part cleared.\n\r", ch );
		return TRUE;
	}

	send_to_char( "Area part set.\n\r", ch );

	return FALSE;
}

REDIT( redit_desc )
{
	ROOM_INDEX_DATA *pRoom;

	EDIT_ROOM(ch, pRoom);

	if ( argument[0] == '\0' )
	{
		string_append( ch, &pRoom->description );
		return TRUE;
	}

	send_to_char( "Sk³adnia: desc\n\r", ch );
	return FALSE;
}

REDIT( redit_nightdesc )
{
	ROOM_INDEX_DATA *pRoom;

	EDIT_ROOM(ch, pRoom);

	if ( argument[0] == '\0' )
	{
		string_append( ch, &pRoom->nightdesc );
		return TRUE;
	}

	send_to_char( "Sk³adnia: nightdesc\n\r", ch );
	return FALSE;
}


REDIT( redit_heal )
{
	ROOM_INDEX_DATA *pRoom;

	EDIT_ROOM(ch, pRoom);

	if (is_number(argument))
	{
		pRoom->heal_rate = atoi ( argument );
		send_to_char ( "Heal rate set.\n\r", ch);
		return TRUE;
	}

	send_to_char ( "Syntax : heal <#xnumber>\n\r", ch);
	return FALSE;
}

REDIT( redit_echoto )
{
	ROOM_INDEX_DATA *pRoom;
	ROOM_INDEX_DATA *echo_room;
	int room;

	EDIT_ROOM( ch, pRoom );

	if ( is_number( argument ) )
	{
		room = atoi( argument );

		if ( room == 0 )
		{
			pRoom->echo_to.room = NULL;
			send_to_char ( "Echo To room removed.\n\r", ch);
			return TRUE;
		}
		else
		{
			if ( ( echo_room = get_room_index( room ) ) == NULL )
			{
				send_to_char( "Nie ma lokacji o takim vnumie.\n\r", ch );
				return FALSE;
			}
			if ( echo_room == pRoom )
			{
				send_to_char( "Nie mo¿esz ustawiæ tej samej lokacji. Je¿eli chcesz usun±æ ten ficzer podaj jako argument 0.", ch);
				return FALSE;
			}

			pRoom->echo_to.room = echo_room;
		}
		send_to_char ( "Echo To room set.\n\r", ch);
		return TRUE;
	}

	send_to_char ( "Syntax : echoto <#xnumber>\n\r", ch);
	return FALSE;
}

REDIT( redit_randday )
{
	ROOM_INDEX_DATA *pRoom;

	EDIT_ROOM(ch, pRoom);

	if (is_number(argument))
	{

		if ( atoi( argument ) != 0 && !get_rdesc_index( atoi( argument ) ) )
		{
			send_to_char( "Nie rand desca o takim vnumie.\n\r", ch );
			return FALSE;
		}

		pRoom->day_rand_desc = atoi ( argument );
		send_to_char ( "Day rand desc set.\n\r", ch);
		return TRUE;
	}

	send_to_char ( "Syntax : randday <#vnum>\n\r", ch);
	return FALSE;
}

REDIT( redit_randnight )
{
	ROOM_INDEX_DATA *pRoom;

	EDIT_ROOM(ch, pRoom);

	if (is_number(argument))
	{
		if ( atoi( argument ) != 0 && !get_rdesc_index( atoi( argument ) ) )
		{
			send_to_char( "Nie rand desca o takim vnumie.\n\r", ch );
			return FALSE;
		}

		pRoom->night_rand_desc = atoi ( argument );
		send_to_char ( "Night rand desc set.\n\r", ch);
		return TRUE;
	}

	send_to_char ( "Syntax : randnight <#vnum>\n\r", ch);
	return FALSE;
}

REDIT( redit_rent )
{
	ROOM_INDEX_DATA *pRoom;

	EDIT_ROOM( ch, pRoom );

	if ( is_number( argument ) && EXT_IS_SET( pRoom->room_flags, ROOM_INN ) )
	{
		pRoom->rent_rate = UMAX( 0, atoi ( argument ) );
		send_to_char ( "Rent rate set.\n\r", ch );
		return TRUE;
	}

	send_to_char ( "Skladnia: rent <number>\n\r"
			"Okresla o ile ma siê roznic cena renta w tym miejscu.\n\r"
			"Ustawienie wartosci 100 nie spowoduje zmiany ceny renta.\n\r"
			"Lokacja musi miec flage 'inn'.\n\r", ch );
	return FALSE;
}

REDIT( redit_clan )
{
	ROOM_INDEX_DATA *pRoom;

	EDIT_ROOM(ch, pRoom);

	return FALSE;
	//pRoom->clan = clan_lookup(argument);

	send_to_char ( "Clan set.\n\r", ch);
	return TRUE;
}

REDIT( redit_format )
{
	ROOM_INDEX_DATA *pRoom;

	EDIT_ROOM(ch, pRoom);

	pRoom->description = format_string( pRoom->description );

	send_to_char( "String formatted.\n\r", ch );
	return TRUE;
}

REDIT( redit_mreset )
{
	ROOM_INDEX_DATA	*pRoom;
	MOB_INDEX_DATA	*pMobIndex;
	CHAR_DATA		*newmob;
	char		arg [ MAX_INPUT_LENGTH ];
	char		arg2 [ MAX_INPUT_LENGTH ];

	RESET_DATA		*pReset;
	char		output [ MAX_STRING_LENGTH ];

	EDIT_ROOM(ch, pRoom);

	argument = one_argument( argument, arg );
	argument = one_argument( argument, arg2 );

	if ( arg[0] == '\0' || !is_number( arg ) )
	{
		send_to_char ( "Sk³adnia: mreset <vnum> <max #x> <mix #x>\n\r", ch );
		return FALSE;
	}

	if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
	{
		send_to_char( "REdit: Mob o takim vnumie nie istnieje.\n\r", ch );
		return FALSE;
	}

	if ( pMobIndex->area != pRoom->area )
	{
		send_to_char( "REdit: No such mobile in this area.\n\r", ch );
		return FALSE;
	}

	/*
	 * Create the mobile reset.
	 */
	pReset              = new_reset_data();
	pReset->command	= 'M';
	pReset->arg1	= pMobIndex->vnum;
	pReset->arg2	= is_number( arg2 ) ? atoi( arg2 ) : MAX_MOB;
	pReset->arg3	= pRoom->vnum;
	pReset->arg4	= is_number( argument ) ? atoi (argument) : 1;
	add_reset( pRoom, pReset, 0/* Last slot*/ );

	/*
	 * Create the mobile.
	 */
	newmob = create_mobile( pMobIndex );
	char_to_room( newmob, pRoom );

	sprintf( output, "%s (%d) has been loaded and added to resets.\n\r"
			"There will be a maximum of %d loaded to this room.\n\r",
			capitalize( pMobIndex->short_descr ),
			pMobIndex->vnum,
			pReset->arg2 );
	send_to_char( output, ch );
	act( "$n has created $N!", ch, NULL, newmob, TO_ROOM );
	return TRUE;
}

//rellik: mining
REDIT( redit_resource )
{
	ROOM_INDEX_DATA *pRoom;
	int i, x;
	char buf1[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];

	EDIT_ROOM( ch, pRoom );
	buf[0]='\0'; buf1[0]='\0';
	if ( argument[0] == '\0' )
	{
		print_char( ch, "Sk³adnia: \n\r" );
		print_char( ch, "resource [nazwa minera³u] [szansa wydobycia]\n\r" );
		print_char( ch, "Gdzie: \n\r" );
		print_char( ch, "[szansa wydobycia] - liczba od 0 do 100 okre¶laj±ca szansê wydobycia danego minera³u, 0 oznacza, ¿e nie wystêpuje.\n\r" );
		print_char( ch, "Mo¿liwe do ustawienia minera³y: \n\r" );
		for ( i = 0; rawmaterial_table[i].name; ++i )
		{
			sprintf( buf, "%s; ", rawmaterial_table[i].name );
			strcat( buf1, buf );
		}
		print_char( ch, "%s\n\r", buf1 );
		return FALSE;
	}

	argument = one_argument( argument, buf );
	if ( ( i = rawmaterial_lookup( buf ) ) == -1 )
	{
		print_char( ch, "Minera³ nie znaleziony.\n\r" );
		return FALSE;
	}

  if ( is_number( argument ) )
  {
  	x = atoi( argument );
  } else {
  	print_char( ch, "Drugi parametr powinien byæ liczb±.\n\r" );
  	return FALSE;
  }

  if ( x < 0 || x > 100 )
  {
  	print_char( ch, "Drugi parametr powinien byæ z zakresu 0 - 100.\n\r" );
  	return FALSE;
  }

  pRoom->rawmaterial[i] = x;
  if ( x == 0 )
  {
  	print_char( ch, "Wydobycie zasobu %s z tej lokacji bêdzie niemo¿liwe.\n\r", rawmaterial_table[i].name );
  } else {
  	print_char( ch, "Wydobycie zasobu %s z tej lokacji ustawione z szans± %d.\n\r", rawmaterial_table[i].name, x );
  }

	return TRUE;
}

//rellik: mining
REDIT( redit_capacity )
{
	ROOM_INDEX_DATA *pRoom;
	int x;

	EDIT_ROOM( ch, pRoom );

	if ( !is_number( argument ) )
	{
		print_char( ch, "Spodziewany argument powinien byæ liczb± dodatni±.\n\r" );
		return FALSE;
	} else {
		x = atoi( argument );
	}
	if ( x < 0 ) x = 0;

	pRoom->rawmaterial_capacity = x;
	pRoom->rawmaterial_capacity_now = x;
	print_char( ch, "Zasobno¶æ z³o¿a ustawiona na %d.\n\r", x );

	return TRUE;
}

struct wear_type
{
	int	wear_loc;
	int	wear_bit;
};



const struct wear_type wear_table[] =
{
	{	WEAR_NONE,	ITEM_TAKE		},
	{	WEAR_LIGHT,	ITEM_LIGHT		},
	{	WEAR_FINGER_L,	ITEM_WEAR_FINGER	},
	{	WEAR_FINGER_R,	ITEM_WEAR_FINGER	},
	{	WEAR_NECK_1,	ITEM_WEAR_NECK		},
	{	WEAR_NECK_2,	ITEM_WEAR_NECK		},
	{	WEAR_BODY,	ITEM_WEAR_BODY		},
	{	WEAR_HEAD,	ITEM_WEAR_HEAD		},
	{	WEAR_LEGS,	ITEM_WEAR_LEGS		},
	{	WEAR_FEET,	ITEM_WEAR_FEET		},
	{	WEAR_HANDS,	ITEM_WEAR_HANDS		},
	{	WEAR_ARMS,	ITEM_WEAR_ARMS		},
	{	WEAR_SHIELD,	ITEM_WEAR_SHIELD	},
	{	WEAR_ABOUT,	ITEM_WEAR_ABOUT		},
	{	WEAR_WAIST,	ITEM_WEAR_WAIST		},
	{	WEAR_WRIST_L,	ITEM_WEAR_WRIST		},
	{	WEAR_WRIST_R,	ITEM_WEAR_WRIST		},
	{	WEAR_WIELD,	ITEM_WIELD		},
	{	WEAR_HOLD,	ITEM_HOLD		},
	{	WEAR_INSTRUMENT,	ITEM_INSTRUMENT		},
	{	WEAR_EAR_L,	ITEM_WEAR_EAR		},
	{	WEAR_EAR_R,	ITEM_WEAR_EAR		},
	{	NO_FLAG,	NO_FLAG			}
};



/*****************************************************************************
Name:		wear_loc
Purpose:	Returns the location of the bit that matches the count.
1 = first match, 2 = second match etc.
Called by:	oedit_reset(olc_act.c).
 ****************************************************************************/
int wear_loc(int bits, int count)
{
	int flag;

	for (flag = 0; wear_table[flag].wear_bit != NO_FLAG; flag++)
	{
		if ( IS_SET(bits, wear_table[flag].wear_bit) && --count < 1)
			return wear_table[flag].wear_loc;
	}

	return NO_FLAG;
}



/*****************************************************************************
Name:		wear_bit
Purpose:	Converts a wear_loc into a bit.
Called by:	redit_oreset(olc_act.c).
 ****************************************************************************/
int wear_bit(int loc)
{
	int flag;

	for (flag = 0; wear_table[flag].wear_loc != NO_FLAG; flag++)
	{
		if ( loc == wear_table[flag].wear_loc )
			return wear_table[flag].wear_bit;
	}

	return 0;
}



REDIT( redit_oreset )
{
	ROOM_INDEX_DATA	*pRoom;
	OBJ_INDEX_DATA	*pObjIndex;
	OBJ_DATA		*newobj;
	OBJ_DATA		*to_obj;
	CHAR_DATA		*to_mob;
	char		arg1 [ MAX_INPUT_LENGTH ];
	char		arg2 [ MAX_INPUT_LENGTH ];
	int			olevel = 0;

	RESET_DATA		*pReset;
	char		output [ MAX_STRING_LENGTH ];

	EDIT_ROOM(ch, pRoom);

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || !is_number( arg1 ) )
	{
		send_to_char ( "Sk³adnia: oreset <vnum> <args>\n\r", ch );
		send_to_char ( "        -no_args               = into room\n\r", ch );
		send_to_char ( "        -<obj_name>            = into obj\n\r", ch );
		send_to_char ( "        -<mob_name> <wear_loc> = into mob\n\r", ch );
		return FALSE;
	}

	if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
	{
		send_to_char( "REdit: Przedmiot o takim vnumie nie istnieje.\n\r", ch );
		return FALSE;
	}

	if ( pObjIndex->area != pRoom->area )
	{
		send_to_char( "REdit: No such object in this area.\n\r", ch );
		return FALSE;
	}

	/*
	 * Load into room.
	 */
	if ( arg2[0] == '\0' )
	{
		pReset		= new_reset_data();
		pReset->command	= 'O';
		pReset->arg1	= pObjIndex->vnum;
		pReset->arg2	= 0;
		pReset->arg3	= pRoom->vnum;
		pReset->arg4	= 0;
		add_reset( pRoom, pReset, 0/* Last slot*/ );

		newobj = create_object( pObjIndex, FALSE );
		obj_to_room( newobj, pRoom );

		sprintf( output, "%s (%d) has been loaded and added to resets.\n\r",
				capitalize( pObjIndex->short_descr ),
				pObjIndex->vnum );
		send_to_char( output, ch );
	}
	else
		/*
		 * Load into object's inventory.
		 */
		if ( argument[0] == '\0'
				&& ( ( to_obj = get_obj_list( ch, arg2, pRoom->contents ) ) != NULL ) )
		{
			pReset		= new_reset_data();
			pReset->command	= 'P';
			pReset->arg1	= pObjIndex->vnum;
			pReset->arg2	= 0;
			pReset->arg3	= to_obj->pIndexData->vnum;
			pReset->arg4	= 1;
			add_reset( pRoom, pReset, 0/* Last slot*/ );

			newobj = create_object( pObjIndex, FALSE );
			newobj->cost = 0;
			obj_to_obj( newobj, to_obj );

			sprintf( output, "%s (%d) has been loaded into "
					"%s (%d) and added to resets.\n\r",
					capitalize( newobj->short_descr ),
					newobj->pIndexData->vnum,
					to_obj->short_descr,
					to_obj->pIndexData->vnum );
			send_to_char( output, ch );
		}
		else
			/*
			 * Load into mobile's inventory.
			 */
			if ( ( to_mob = get_char_room( ch, arg2 ) ) != NULL )
			{
				int	wear_loc;

				/*
				 * Make sure the location on mobile is valid.
				 */
				if ( (wear_loc = flag_value( wear_loc_flags, argument )) == NO_FLAG )
				{
					send_to_char( "REdit: Invalid wear_loc.  '? wear-loc'\n\r", ch );
					return FALSE;
				}

				/*
				 * Disallow loading a sword(WEAR_WIELD) into WEAR_HEAD.
				 */
				if ( !IS_SET( pObjIndex->wear_flags, wear_bit(wear_loc) ) )
				{
					sprintf( output,
							"%s (%d) has wear flags: [%s]\n\r",
							capitalize( pObjIndex->short_descr ),
							pObjIndex->vnum,
							flag_string( wear_flags, pObjIndex->wear_flags ) );
					send_to_char( output, ch );
					return FALSE;
				}

				/*
				 * Can't load into same position.
				 */
				if ( get_eq_char( to_mob, wear_loc ) )
				{
					send_to_char( "REdit:  Object already equipped.\n\r", ch );
					return FALSE;
				}

				pReset		= new_reset_data();
				pReset->arg1	= pObjIndex->vnum;
				pReset->arg2	= wear_loc;
				if ( pReset->arg2 == WEAR_NONE )
					pReset->command = 'G';
				else
					pReset->command = 'E';
				pReset->arg3	= wear_loc;

				add_reset( pRoom, pReset, 0/* Last slot*/ );

				olevel  = URANGE( 0, to_mob->level - 2, LEVEL_HERO );
				newobj = create_object( pObjIndex, FALSE );

				if ( to_mob->pIndexData->pShop )	/* Shop-keeper? */
				{
					switch ( pObjIndex->item_type )
					{
						default:		olevel = 0;				break;
						case ITEM_PILL:	olevel = number_range(  0, 10 );	break;
						case ITEM_POTION:	olevel = number_range(  0, 10 );	break;
						case ITEM_SCROLL:	olevel = number_range(  5, 15 );	break;
						case ITEM_WAND:		olevel = number_range( 10, 20 );	break;
						case ITEM_STAFF:	olevel = number_range( 15, 25 );	break;
						case ITEM_ARMOR:	olevel = number_range(  5, 15 );	break;
						case ITEM_WEAPON:	if ( pReset->command == 'G' )
										olevel = number_range( 5, 15 );
									else
										olevel = number_fuzzy( olevel );
									break;
					}

					newobj = create_object( pObjIndex, FALSE );
					if ( pReset->arg2 == WEAR_NONE )
						EXT_SET_BIT( newobj->extra_flags, ITEM_INVENTORY );
				}
				else
					newobj = create_object( pObjIndex, FALSE);

				obj_to_char( newobj, to_mob );
				if ( pReset->command == 'E' )
					equip_char( to_mob, newobj, pReset->arg3, TRUE );

				sprintf( output, "%s (%d) has been loaded "
						"%s of %s (%d) and added to resets.\n\r",
						capitalize( pObjIndex->short_descr ),
						pObjIndex->vnum,
						flag_string( wear_loc_strings, pReset->arg3 ),
						to_mob->short_descr,
						to_mob->pIndexData->vnum );
				send_to_char( output, ch );
			}
			else	/* Display Syntax */
			{
				send_to_char( "REdit:  Nie ma tu takiego moba.\n\r", ch );
				return FALSE;
			}

	act( "$n has created $p!", ch, newobj, NULL, TO_ROOM );
	return TRUE;
}



/*
 * Object Editor Functions.
 */
void show_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *obj )
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];

	switch( obj->item_type )
	{
		/* No values. */
		default:
			return;
			break;

		case ITEM_LIGHT:
			// czy ¶wiat³o ¶wieci pod wod±?
			sprintf( buf, "[v0] ¦wiat³o podwodne: %s{x\n\r", obj->value[0] == 1 ? "{GTak" : "{RNie" );
			send_to_char( buf, ch );

			if ( obj->value[2] == -1 || obj->value[2] == 999 ) /* ROM OLC */
			{
				send_to_char( "[v2] Czas ¶wiecenia:   nieskoñczony[-1]\n\r", ch );
			} else {
				sprintf( buf, "[v2] Czas ¶wiecenie:   [%d]\n\r", obj->value[2] );
			}
			if ( obj->value[1] == 0 )
				send_to_char( "[v1] Sposob zamocowania: nie mozna zamocowac\n\r", ch );
			if ( IS_SET(obj->value[1], LIGHT_LIE ) )
				send_to_char( "[v1] Sposob zamocowania: mozna polozyc na ziemi\n\r", ch );
			if ( IS_SET(obj->value[1], LIGHT_HANG ) )
				send_to_char( "[v1] Sposob zamocowania: mozna powiesic\n\r", ch );
			if ( IS_SET(obj->value[1], LIGHT_STICK ) )
				send_to_char( "[v1] Sposob zamocowania: mozna wbic w ziemie\n\r", ch );
			if ( obj->value[3] == VUL_NONE )
				send_to_char( "[v3] Wrazliwe na: nic\n\r", ch );
			if ( obj->value[3] == VUL_RAIN )
				send_to_char( "[v3] Wrazliwe na: deszcz\n\r", ch );
			if ( obj->value[3] == VUL_WIND )
				send_to_char( "[v3] Wrazliwe na: wiatr\n\r", ch );
			send_to_char( "[v4] ZAREZERWOWANE\n\r", ch );
			break;

		case ITEM_STAFF:
			sprintf( buf,
					"[v0] Poziom czaru:        [%d]\n\r"
					"[v1] Wszystkich ³adunków: [%d]\n\r"
					"[v2] Ilo¶æ ³adunków:      [%d]\n\r"
					"[v3] Nazwa czaru:         %s\n\r",
					obj->value[0],
					obj->value[1],
					obj->value[2],
					obj->value[3] != -1 ? skill_table[obj->value[3]].name : "nieustawiono"
			       );
			break;

		case ITEM_WAND:
			sprintf( buf,
					"[v0] Pierwszy efekt - typ:     %s   {Y? wand{x\n\r"
					"[v1] Pierwszy efekt - warto¶æ: [%d]\n\r"
					"[v2] Drugi efekt - typ:        %s   {Y? wand{x\n\r"
					"[v3] Drugi efekt - warto¶æ:    [%d]\n\r"
					"[v4] Trzeci efekt - typ:       %s   {Y? wand{x\n\r"
					"[v5] Trzeci efekt - warto¶æ:   [%d]\n\r"
					"[v6] Dodatkowe w³a¶ciwo¶ci:    %s   {Y? wand_extra{x\n\r",
					wand_table[obj->value[0]].name,
					obj->value[1],
					wand_table[obj->value[2]].name,
					obj->value[3],
					wand_table[obj->value[4]].name,
					obj->value[5],
					flag_string( wand_extra, obj->value[6] ) );

			break;

		case ITEM_PORTAL:
			sprintf( buf,
					"[v0] £adunków:           [%d]\n\r"
					"[v1] Flagi wej¶cia:      %s   {Y? exit{x\n\r"
					"[v2] Flagi portalu:      %s   {Y? portal{x\n\r"
					"[v3] Prowadzi do (vnum): [%d]\n\r",
					obj->value[0],
					flag_string( exit_flags, obj->value[1]),
					flag_string( portal_flags , obj->value[2]),
					obj->value[3]
			       );
			break;

		case ITEM_FURNITURE:
			sprintf( buf,
					"[v0] Max u¿ytkowników:  [%d]\n\r"
					"[v1] Max obci±¿enie:    [%d]\n\r"
					"[v2] Flagi mebla:       %s   {Y? furniture{x\n\r"
					"[v3] Bonus do leczenia: [%d]   {rpodajemy w procentach 100 - oznacza normalne leczenie{x\n\r"
					"[v4] Bonus do many:     [%d]   {rna killerze parametr nieu¿ywany{x\n\r",
					obj->value[0],
					obj->value[1],
					flag_string( furniture_flags, obj->value[2]),
					obj->value[3],
					obj->value[4] );
			break;

		case ITEM_CORPSE_NPC:
			sprintf( buf,
					"[v5] Flagi cia³a:        [%s]   {Y? corpse{x\n\r"
					"[v6] Vnum skóry z cia³a: [%d]\n\r",
					flag_string( corpse_flags, obj->value[5]),
					obj->value[6]);
			break;

		case ITEM_SCROLL:
		case ITEM_POTION:
		case ITEM_PILL:
			sprintf( buf,
					"[v0] Poziom:      [%d]\n\r"
					"[v1] Nazwa czaru: %s\n\r"
					"[v2] Nazwa czaru: %s\n\r"
					"[v3] Nazwa czaru: %s\n\r"
					"[v4] Nazwa czaru: %s\n\r",
					obj->value[0],
					obj->value[1] != -1 ? skill_table[obj->value[1]].name : "nieustawiono",
					obj->value[2] != -1 ? skill_table[obj->value[2]].name : "nieustawiono",
					obj->value[3] != -1 ? skill_table[obj->value[3]].name : "nieustawiono",
					obj->value[4] != -1 ? skill_table[obj->value[4]].name : "nieustawiono" );
			break;

		case ITEM_ARMOR:
			// Dla rêkawic wprawadzamy parametr w którym ustawiamy w jaki sposób mo¿na
			// nosiæ pier¶cionki.
			if (obj->wear_flags & ITEM_WEAR_HANDS)
			{
				sprintf( buf,
						"[v0] Ac pierce     [%d]\n\r"
						"[v1] Ac bash       [%d]\n\r"
						"[v2] Ac slash      [%d]\n\r"
						"[v3] Ac exotic     [%d]\n\r"
						"[v4] Typ pancerza  [%s]   {Y? armor_type{x\n\r"
						"[v5] Bi¿uteria     [%s]   {Rtylko dla rêkawic{x\n\r",
						obj->value[0],
						obj->value[1],
						obj->value[2],
						obj->value[3],
						armor_name(obj->value[4]),
						jewlery2hands(obj->value[5])
				       );
			}
			else
			{
				sprintf( buf,
						"[v0] Ac pierce     [%d]\n\r"
						"[v1] Ac bash       [%d]\n\r"
						"[v2] Ac slash      [%d]\n\r"
						"[v3] Ac exotic     [%d]\n\r"
						"[v4] Typ pancerza  [%s]   {Y? armor_type{x\n\r",
						obj->value[0],
						obj->value[1],
						obj->value[2],
						obj->value[3],
						armor_name(obj->value[4])
				       );
			}
			break;

		case ITEM_SPELLBOOK:
            if (ch->pcdata->wiz_conf & W5)
            {
                sprintf( buf,
                        "[v0] Dostêpne tylko dla klasy  : %s\n\r"
                        "[v1] Trudno¶æ nauczenia (0-10) : [%d]\n\r"
                        "[v2] Nazwa czaru:                %s\n\r"
                        "[v3] Nazwa czaru:                %s\n\r"
                        "[v4] Nazwa czaru:                %s\n\r"
                        "[v5] Nazwa czaru:                %s\n\r"
                        "[v6] Nazwa czaru:                %s\n\r",
                        class_table[obj->value[0]].name,
                        obj->value[1],
                        obj->value[2] == 0 ? "nieustawiono" : skill_table[obj->value[2]].name,
                        obj->value[3] == 0 ? "nieustawiono" : skill_table[obj->value[3]].name,
                        obj->value[4] == 0 ? "nieustawiono" : skill_table[obj->value[4]].name,
                        obj->value[5] == 0 ? "nieustawiono" : skill_table[obj->value[5]].name,
                        obj->value[6] == 0 ? "nieustawiono" : skill_table[obj->value[6]].name
                       );
            }
            else
            {
                sprintf( buf, "Brak uprawnieñ do w³a¶ciwo¶ci.\n\r");
            }
			break;

		case ITEM_SPELLITEM:
			sprintf( buf,
					"[v0] Dostêpne tylko dla klasy  :  %s\n\r"
					"[v1] Zaklêcie                  :  %s\n\r"
					"[v2] Limit wykorzystania         [%d]\n\r"
					"[v3] Ryzyko             (1-10) : [%d]\n\r"
					"[v4] Min. level                : [%d]\n\r",
					class_table[obj->value[0]].name,
					skill_table[obj->value[1]].name,
					obj->value[2], obj->value[3],obj->value[4]
			       );
			break;

		case ITEM_BOARD:
			sprintf( buf,
					"[v0] Czas wygasania wiadomo¶ci, podawany w dniach:    [%d]\n\r"
					"[v1] Minimalny wymagany poziom potrzebny do czytania: [%d]\n\r"
					"[v2] Minimalny wymagany poziom potrzebny do pisania:  [%d]\n\r",
					obj->value[0],
					obj->value[1],
					obj->value[2]
			       );
			break;

		case ITEM_PIECE:
			sprintf( buf,
					"[v0] Vnum poprzedniej czê¶ci           [%d]\n\r"
					"[v1] Vnum nastepnej czê¶ci             [%d]\n\r"
					"[v2] Vnum nowego przedmiotu            [%d]\n\r",
					obj->value[0],
					obj->value[1],
					obj->value[2]);
			break;

		case ITEM_ENVENOMER:
			sprintf( buf,
					"[v0] Rodzaj trucizny: %s   {y? poison{x\n\r"
					"[v1] Liczba zatruæ:   %d.\n\r",
					capitalize(poison_table[URANGE(0, obj->value[0], MAX_POISON)].name),
					obj->value[1]);
			break;

      case ITEM_TOTEM:
         sprintf( buf,
               "[v0] Rodzaj ducha totemicznego: %s\n\r"
               "[v1] Moc totemu: %d\n\r.",
               flag_string( totem_table, obj->value[0] ),
					obj->value[1]);
         break;

		case ITEM_TURN:
			sprintf( buf,
					"[v0] Bonus do umiejêtno¶ci: %d.\n\r"
					"[v1] Bonus do obra¿eñ:       %d.\n\r",
					obj->value[0],
					obj->value[1]);
			break;

		case ITEM_BANDAGE:
			sprintf( buf,
					"[v0] Liczba banda¿owañ: %d\n\r"
					"[v1] Bonus leczenia:    %d%%\n\r",
					obj->value[0],
					obj->value[1]);
			break;

		case ITEM_SHIELD:
			sprintf( buf,
					"[v0] Rodzaj tarczy:                                      %s\n\r"
					"[v1] Maksymalny bonus pancerza:                          %d\n\r"
					"[v2] Maksymalny blokowany cios:                          %d\n\r"
					"[v3] Szansa zablokowania wiêcej ni¿ jednego przeciwnika: %d\n\r"
					"[v4] Premia do trafienia w przypadku uderzania tarcz±:   %d\n\r"
					"[v5] Premia do obra¿eñ w przypadku uderzania tarcz±:     %d\n\r",
					ShieldList[obj->value[0]].name,
					obj->value[1],
					obj->value[2],
					obj->value[3],
					obj->value[4],
					obj->value[5]
			       );
			break;


			/* WEAPON changed in ROM: */
			/* I had to split the output here, I have no idea why, but it helped -- Hugin */
			/* It somehow fixed a bug in showing scroll/pill/potions too ?! */
		case ITEM_WEAPON:
			sprintf( buf,
					"[v0] Klasa broni:               %s   {Y? wclass{x\n\r"
					"[v1] Liczba kostek:             [%d]\n\r"
					"[v2] Liczba ¶cian kostki:       [%d]\n\r"
					"[v3] Rodzaj zadawanych obra¿eñ: %s   {Y? weapon{x\n\r"
					"[v4] W³a¶ciwo¶ci specjalne:     %s   {Y? wtype{x\n\r"
					"[v5] Bonus do trafienia:        [%d]\n\r"
					"[v6] Bonus do obra¿eñ:          [%d]\n\r",
					flag_string( weapon_class, obj->value[0] ),
					obj->value[1],
					obj->value[2],
					attack_table[obj->value[3]].name,
					flag_string( weapon_type2, obj->value[4] ),
					obj->value[5],
					obj->value[6]
			       );
			break;

		case ITEM_MUSICAL_INSTRUMENT:
			sprintf( buf,
					"[v0] Typ instrumentu:          %s   {Y? instrument{x\n\r"
					"[v1] Bonus do umiejêtno¶ci:   [%d]\n\r"
					"[v2] Nastrojenie instrumentu: [%d]%%\n\r",
					flag_string( instrument_class, obj->value[0] ), obj->value[1], obj->value[2]
			       );
			break;

		case ITEM_CONTAINER:
			sprintf( buf2, "%d", obj->value[5] );
			if ( obj->value[5] == ITEM_WEAPON )
			{
				sprintf( buf,
						"[v0] Maks. waga pojemnika+zawarto¶ci:             [%d kg]\n\r"
						"[v1] Flagi:                                       [%s]   {Y? container{x\n\r"
						"[v2] Klucz:                                       [%d] %s\n\r"
						"[v3] Maks. waga pojedynczego przedmiotu:          [%d kg]\n\r"
						"[v4] Waga przedmiotu po w³o¿eniu:                 [%d%%]\n\r"
						"[v5] Typ lub vnum przedmiotu, który mo¿na w³o¿yæ: [%s]   {Y? type{x\n\r"
						"[v6] Rodzaj broni:                                [%s]   {Y? wclass{x\n\r",
						obj->value[0],
						flag_string( container_flags, obj->value[1] ),
						obj->value[2],
						get_obj_index(obj->value[2])? get_obj_index(obj->value[2])->short_descr : "nieustawiono",
						obj->value[3],
						obj->value[4],
						obj->value[5] < 50 ? flag_string( type_flags, obj->value[5] ) : buf2,
						obj->value[6] == -1 ? "dowolne" : flag_string( weapon_class, obj->value[6] )
				       );
			}
			else
			{
				sprintf( buf,
						"[v0] Maks. waga pojemnika+zawarto¶ci:             [%d kg]\n\r"
						"[v1] Flagi:                                       [%s]   {Y? container{x\n\r"
						"[v2] Klucz:                                       [%d] %s\n\r"
						"[v3] Maks. waga pojedynczego przedmiotu:          [%d kg]\n\r"
						"[v4] Waga przedmiotu po w³o¿eniu:                 [%d%%]\n\r"
						"[v5] Typ lub vnum przedmiotu, który mo¿na w³o¿yæ: [%s]   {Y? type{x\n\r",
						obj->value[0],
						flag_string( container_flags, obj->value[1] ),
						obj->value[2],
						get_obj_index(obj->value[2])? get_obj_index(obj->value[2])->short_descr : "nieustawiono",
						obj->value[3],
						obj->value[4],
						obj->value[5] < 50 ? flag_string( type_flags, obj->value[5] ) : buf2 );
			}
			break;

		case ITEM_DRINK_CON:
			sprintf( buf,
					"[v0] Pojemno¶æ:                     [%d]\n\r"
					"[v1] Ilo¶æ p³ynu:                   [%d]\n\r"
					"[v2] Rodzaj p³ynu:                  %s   {Y? liquid{x\n\r"
					"[v3] Czy zatruty:                   %s{x\n\r"
					"[v4] Vnum opró¿nionego przedmiotu:  [%d]{x\n\r",
					obj->value[0],
					obj->value[1],
					liq_table[obj->value[2]].liq_name,
					obj->value[3] != 0 ? "{GTak" : "{RNie",
					obj->value[4] );
			break;

		case ITEM_FOUNTAIN:
			sprintf( buf,
					"[v0] Pojemno¶æ:    [%d]\n\r"
					"[v1] Ilo¶æ p³ynu:  [%d]   {Y? liquid{x\n\r"
					"[v2] Rodzaj p³ynu: %s\n\r",
					obj->value[0],
					obj->value[1],
					liq_table[obj->value[2]].liq_name );
			break;

		case ITEM_FOOD:
			sprintf( buf,
					"[v0] Po¿ywno¶æ (kês - 6):  [%d]\n\r"
					"[v1] Psuje siê po (ticki): [%d]\n\r"
					"[v2] Jedzenie jest:        %s{x   {Y? foodstate{x\n\r" // food state - by Fuyara
					"[v3] Zatrute:              %s{x\n\r"
					"[v4] Typ :                 %s{x   {Y? foodtype{x\n\r",
					obj->value[0],
					obj->value[1],
					get_food_state_name( obj->value[2], 'p' ), // food state - by Fuyara
					obj->value[3] != 0 ? "{GTak" : "{RNie",
					get_food_type_name(obj->value[4], 'p')
			);
			break;

		case ITEM_MONEY:
            sprintf
                (
                 buf,
                 "[v0] Liczba miedzianych monet:  [%d]\n\r"
                 "[v1] Liczba srebrnych monet:    [%d]\n\r"
                 "[v2] Liczba zlotych monet:      [%d]\n\r"
                 "[v3] Liczba mithrilowych monet: [%d]\n\r",
                 obj->value[ 0 ],
                 obj->value[ 1 ],
                 obj->value[ 2 ],
                 obj->value[ 3 ]
                );
			break;

		case ITEM_PIPE:
			sprintf( buf,
					//"[v0] Zapalona?: 	    %s{x\n\r"
					"[v1] Ilo¶æ ziela:	    [%d]\n\r"
					"[v2] Typ ziela:             %s{x   {Y? weed{x\n\r",
					//obj->value[0] != 0 ? "{GTak" : "{RNie",
					obj->value[1],
					weed_table[obj->value[2]].weed_name );
			break;

		case ITEM_WEED:
			sprintf( buf,
					"[v0] Ilo¶c ziela: 	    [%d]\n\r"
					"[v1] Typ ziela:             %s{x   {Y? weed{x\n\r",
					obj->value[0],
					weed_table[obj->value[1]].weed_name );
			break;

		case ITEM_NOTEPAPER:
			sprintf( buf,
					"[v0] Zapisane?: 	    %s{x\n\r",
					obj->value[0] != 0 ? "{GTak" : "{RNie" );
			break;
		case ITEM_TOOL: //rellik: mining
			sprintf( buf,
					"[v0] Jako¶æ narzêdzia 1..9:     						 [%d]\n\r"
					"[v1] Poziom skilla od jakigo mo¿na u¿yæ:     [%d]\n\r"
					"[v2] Typ narzêdzia: 		%s       {Y? tool_type{x\n\r",
					obj->value[0], obj->value[1], tool_table[ obj->value[2] ].name );
			break;
	}
	send_to_char( buf, ch );
	return;
}

bool set_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, int value_num, char *argument )
{
	int armor, tmp;

	switch ( pObj->item_type )
	{
		default:
			break;

		case ITEM_LIGHT:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_LIGHT" );
					return FALSE;
				case 0:
					send_to_char( "WATERLIGHT TOGGLED.\n\r\n\r", ch );
					pObj->value[ 0 ] = ( pObj->value[ 0 ] == 0 ) ? 1 : 0;
					break;
				case 2:
					send_to_char( "HOURS OF LIGHT SET.\n\r\n\r", ch );
					pObj->value[ 2 ] = atoi( argument );
					break;
				case 1:
					send_to_char( "WAY OF MOUNTING SET.\n\r\n\r", ch );
					pObj->value[ 1 ] = atoi( argument );
					send_to_char( "Pomoc: \n\r" , ch );
					send_to_char( " 0 - nie mozna zamocowac \n\r", ch );
					send_to_char( " 1 - mozna polozyc\n\r", ch );
					send_to_char( " 2 - mozna wbic\n\r", ch );
					send_to_char( " 4 - mozna powiesic\n\r\n\r", ch );
					send_to_char( " Wartosci mozna laczyc np 1 i 4 da 5 - mozna polozyc lub powiesic.\n\r", ch );
					break;
				case 3:
					send_to_char( "VULNERABILITY OF WEATHER SET.\n\r\n\r", ch );
					pObj->value[ 3 ] = atoi( argument );
					send_to_char( "Pomoc: \n\r" , ch );
					send_to_char( " 0 - nie wrazliwe na pogode\n\r", ch );
					send_to_char( " 1 - wrazliwe na deszcz\n\r", ch );
					send_to_char( " 2 - wrazliwe na wiatr\n\r", ch );
				case 4:
					send_to_char( "v4 jest zarezerwowane, nie ustawiam\n\r", ch);
			}
			break;

		case ITEM_STAFF:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_STAFF_WAND" );
					return FALSE;
				case 0:
					send_to_char( "SPELL LEVEL SET.\n\r\n\r", ch );
					pObj->value[ 0 ] = atoi( argument );
					break;
				case 1:
					send_to_char( "TOTAL NUMBER OF CHARGES SET.\n\r\n\r", ch );
					pObj->value[ 1 ] = atoi( argument );
					break;
				case 2:
					send_to_char( "CURRENT NUMBER OF CHARGES SET.\n\r\n\r", ch );
					pObj->value[ 2 ] = atoi( argument );
					break;
				case 3:
					send_to_char( "SPELL TYPE SET.\n\r", ch );
					pObj->value[ 3 ] = skill_lookup( argument );
					break;
			}
			break;

		case ITEM_WAND:
			switch ( value_num )
			{
				case 0:
					send_to_char( "Pierwszy typ efektu ustawiony.\n\r\n\r", ch );
					ALT_FLAGVALUE_SET( pObj->value[ 0 ], wand_class, argument );
					break;
				case 2:
					send_to_char( "Drugi typ efektu ustawiony.\n\r\n\r", ch );
					ALT_FLAGVALUE_SET( pObj->value[ 2 ], wand_class, argument );
					break;
				case 4:
					send_to_char( "Trzeci typ efektu ustawiony.\n\r\n\r", ch );
					ALT_FLAGVALUE_SET( pObj->value[ 4 ], wand_class, argument );
					break;
				case 1:
					send_to_char( "Warto¶æ pierwszego efektu ustawiona.\n\r\n\r", ch );
					pObj->value[ 1 ] = atoi( argument );
					break;
				case 3:
					send_to_char( "Warto¶æ drugiego efektu ustawiona.\n\r\n\r", ch );
					pObj->value[ 3 ] = atoi( argument );
					break;
				case 5:
					send_to_char( "Warto¶æ trzeciego efektu ustawiona.\n\r\n\r", ch );
					pObj->value[ 5 ] = atoi( argument );
					break;
				case 6:
					send_to_char( "Dodatkowa w³a¶ciwo¶æ ustalona.\n\r", ch );
					ALT_FLAGVALUE_TOGGLE( pObj->value[ 6 ], wand_extra, argument );
					send_to_char(   "Pomoc:\n\r"
									"{Yalign_good{x - kiedy ró¿d¿kê bêdzie u¿ywaæ kto¶ nie-dobry...\n\r"
									"{Yalign_neutral{x - kiedy ró¿d¿kê bêdzie u¿ywaæ kto¶ nie-neutralny...\n\r"
									"{Yalign_evil{x - kiedy ró¿d¿kê bêdzie u¿ywaæ kto¶ nie-z³y...\n\r"
									"{Yclass_mage{x - kiedy ró¿dzkê bêdzie u¿ywaæ nie-mag...\n\r"
									"{Yclass_cleric{x - kiedy ró¿dzkê bêdzie u¿ywaæ nie-kleryk...\n\r"
									"{Yclass_druid{x - kiedy ró¿dzkê bêdzie u¿ywaæ nie-druid...\n\r"
									"... to wszystkie warto¶ci ró¿d¿ki zadzia³aj± odwrotnie. Czyli jak jest ustawione np +10% do dama i align_evil, to dla neutralnych i dobrych bêdzie to -10% do dama.\n\r", ch );
					send_to_char( "W przypadku wybrania specjalnej w³a¶ciwo¶ci dotycz±cej szko³y magii, to efekt pierwszy bêdzie dotyczy³ zaklêæ tej w³a¶nie szko³y magii, a pozosta³e efekty zaklêæ pozosta³ych szkó³ magii. Je¿eli chcemy, by drugi i trzeci efekt dzia³a³y tylko dla jednej szko³y magii, to musimy wybraæ wszystkie pozosta³e szko³y. Dodatkowe w³a¶ciwo¶ci 'szko³a...' nie dzia³aj± w typach efektu mem_liczbowo i mem_procentowo. W przypadku ustawienia jednocze¶nie w³a¶ciwo¶ci 'align..' i 'class..', w³a¶ciwo¶æ 'class..' nie bêdzie dzia³aæ. Nienalezy tez w jakikolwiek sposob powielac jednego efektu, gdyz wtedy zadziala tylko pierwszy.\n\r\n\r", ch );
				default:
   					break;
			}
			break;

		case ITEM_SCROLL:
		case ITEM_POTION:
		case ITEM_PILL:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_SCROLL_POTION_PILL" );
					return FALSE;
				case 0:
					send_to_char( "SPELL LEVEL SET.\n\r\n\r", ch );
					pObj->value[ 0 ] = atoi( argument );
					break;
				case 1:
					send_to_char( "SPELL TYPE 1 SET.\n\r\n\r", ch );
					pObj->value[ 1 ] = skill_lookup( argument );
					break;
				case 2:
					send_to_char( "SPELL TYPE 2 SET.\n\r\n\r", ch );
					pObj->value[ 2 ] = skill_lookup( argument );
					break;
				case 3:
					send_to_char( "SPELL TYPE 3 SET.\n\r\n\r", ch );
					pObj->value[ 3 ] = skill_lookup( argument );
					break;
				case 4:
					send_to_char( "SPELL TYPE 4 SET.\n\r\n\r", ch );
					pObj->value[ 4 ] = skill_lookup( argument );
					break;
			}
			break;

			/* ARMOR for ROM: */


		case ITEM_ARMOR:
			armor = 10;

			if ( is_number( argument ) )
			{
				armor = atoi( argument );

				if ( armor < armor_table[ pObj->value[ 4 ] ].min_ac[ value_num ] )
					armor = armor_table[ pObj->value[ 4 ] ].min_ac[ value_num ];
				else if ( armor > 10 )
					armor = 10;
				else if ( armor > ( armor_table[ pObj->value[ 4 ] ].min_ac[ value_num ] + 3 ) )
					armor = UMIN( 10, armor_table[ pObj->value[ 4 ] ].min_ac[ value_num ] + 3 );
			}

			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_ARMOR" );
					return FALSE;
				case 0:
					send_to_char( "AC PIERCE SET.\n\r\n\r", ch );
					pObj->value[ 0 ] = armor;
					break;
				case 1:
					send_to_char( "AC BASH SET.\n\r\n\r", ch );
					pObj->value[ 1 ] = armor;
					break;
				case 2:
					send_to_char( "AC SLASH SET.\n\r\n\r", ch );
					pObj->value[ 2 ] = armor;
					break;
				case 3:
					send_to_char( "AC EXOTIC SET.\n\r\n\r", ch );
					pObj->value[ 3 ] = armor;
					break;

				case 4:
					if ( ( armor = armor_lookup( argument ) ) < 0 )
					{
						show_armor_list( ch );
						return FALSE;
					}

					send_to_char( "AC ARMOR TYPE SET.\n\n\r", ch );
					pObj->value[ 4 ] = armor;

					for ( armor = 0; armor < 4; armor++ )
					{
						if ( pObj->value[ armor ] < armor_table[ pObj->value[ 4 ] ].min_ac[ armor ] )
							pObj->value[ armor ] = armor_table[ pObj->value[ 4 ] ].min_ac[ armor ];
						if ( pObj->value[ armor ] > armor_table[ pObj->value[ 4 ] ].min_ac[ armor ] + 3 )
							pObj->value[ armor ] = armor_table[ pObj->value[ 4 ] ].min_ac[ armor ] + 3;
					}
					break;
				case 5:
					if ( pObj->wear_flags & ITEM_WEAR_HANDS )
					{
						if ( is_number( argument ) )
						{
							armor = atoi( argument );
							if ( armor < 0 || armor > 3)
							{
								pObj->value[ 5 ] = 0;
							}
							else
							{
								pObj->value[ 5 ] = armor;
							}
						}
						else if ( !str_cmp( argument, "pod" ) || !str_cmp( argument, "brak" ) || !str_cmp( argument, "nieustawiono" ) || !str_cmp( argument, "none" )  || !str_cmp( argument, "zero" ) )
						{
							pObj->value[ 5 ] = 0;
						}
						else if ( !str_cmp( argument, "na" ) )
						{
							pObj->value[ 5 ] = 1;
						}
						else if ( !str_cmp( argument, "pod i na" ) )
						{
							pObj->value[ 5 ] = 2;
						}
						else if ( !str_cmp( argument, "nie" ) || !str_cmp( argument, "nie mo¿na" ))
						{
							pObj->value[ 5 ] = 3;
						}
					}
					else {
						do_help( ch, "ITEM_ARMOR" );
						return FALSE;
					}
					break;
			}
			break;


		case ITEM_BOARD:
			switch ( value_num )
			{
				default:
					return FALSE;
				case 0:
					send_to_char( "Expiry time set.\n\r\n\r", ch );
					pObj->value[ 0 ] = atoi( argument );
					break;
				case 1:
					send_to_char( "Minlevel set.\n\r\n\r", ch );
					pObj->value[ 1 ] = atoi( argument );
					break;
				case 2:
					send_to_char( "Minlevel set.\n\r\n\r", ch );
					pObj->value[ 2 ] = atoi( argument );
					break;
			}
			break;

		case ITEM_SPELLBOOK:
			switch ( value_num )
			{
				default:
					return FALSE;
				case 0:
					tmp = class_lookup( argument );

					if ( tmp == -1 )
					{
						send_to_char( "Nie ma takiej profesji.\n\r\n\r", ch );
						return FALSE;
					}

					if ( class_table[ tmp ].caster < 0 )
					{
						send_to_char( "Wybierz profesje poslugujaca sie magia.\n\r\n\r", ch );
						return FALSE;
					}

					pObj->value[ 0 ] = tmp;
					send_to_char( "Klasa ustawiona.\n\r\n\r", ch );

					int counter;

					for ( counter = 2; counter < 7; counter++ )
					{
						if ( pObj->value[ counter ] > 0 && pObj->value[ counter ] < MAX_SKILL && skill_table[ pObj->value[ counter ] ].spell_fun != spell_null )
						{
							if ( skill_table[ pObj->value[ counter ] ].skill_level[ tmp ] >= 32 )
							{
								send_to_char( "{RUWAGA:{x Niektórych zaklêæ w ksiêdze ustawiona profesja nie bêdzie mog³a siê nauczyæ.\n\r\n\r", ch );
								break;
							}
						}
					}
					break;

                case 1:
                    if ( !is_number( argument ) )
                    {
                        print_char
                            (
                             ch,
                             "Podaj liczbe z zakresu %d (bardzo latwe) do %d (bardzo trudne)\n\r",
                             SPELLBOOK_LEVEL_MIN,
                             SPELLBOOK_LEVEL_MAX
                            );
                        return FALSE;
                    }
                    tmp = atoi( argument );
                    if ( tmp < SPELLBOOK_LEVEL_MIN || tmp > SPELLBOOK_LEVEL_MAX )
                    {
                        print_char
                            (
                             ch,
                             "Podaj liczbe z zakresu %d (bardzo latwe) do %d (bardzo trudne)\n\r",
                             SPELLBOOK_LEVEL_MIN,
                             SPELLBOOK_LEVEL_MAX
                            );
                        return FALSE;
                    }
                    send_to_char( "Trudno¶æ ustawiona.\n\r\n\r", ch );
                    pObj->value[ 1 ] = tmp;
                    break;

				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
					tmp = spell_only_lookup( argument );

					if ( tmp == -1 )
					{
						if ( is_number( argument ) )
						{
							tmp = atoi( argument );
							if ( tmp == 0 )
							{
								pObj->value[ value_num ] = 0;
								send_to_char( "Usuniêto zaklecie.\n\r\n\r", ch );
								break;
							}
						}
						else if ( !str_cmp( argument, "brak" ) || !str_cmp( argument, "nieustawiono" ) || !str_cmp( argument, "none" )  || !str_cmp( argument, "zero" ) )
						{
							pObj->value[ value_num ] = 0;
							send_to_char( "Usuniêto zaklecie.\n\r\n\r", ch );
							break;
						}
						send_to_char( "Nie ma takiego zaklêcia.\n\r\n\r", ch );
						return FALSE;
					}

					if ( skill_table[ tmp ].skill_level[ pObj->value[ 0 ] ] >= 32 )
					{
						send_to_char( "Zaklêcie nie jest dostêpne dla profesji, dla której przeznaczona jest ta ksiêga.\n\r", ch );
						return FALSE;
					}

					pObj->value[ value_num ] = tmp;
					send_to_char( "Zaklêcie ustawione.\n\r\n\r", ch );
					break;
			}
			break;

		case ITEM_SPELLITEM:
			switch ( value_num )
			{
				default:
					return FALSE;
				case 0:
					tmp = class_lookup( argument );

					if ( tmp == -1 )
					{
						send_to_char( "Nie ma takiej profesji.\n\r\n\r", ch );
						return FALSE;
					}
					pObj->value[ 0 ] = tmp;
					send_to_char( "Klasa ustawiona.\n\r\n\r", ch );
					break;
				case 1:

					tmp = skill_lookup( argument );

					if ( tmp == -1 )
					{
						send_to_char( "Nie ma takiego zaklecia.\n\r\n\r", ch );
						return FALSE;
					}

					if ( tmp > 0 && skill_table[ tmp ].spell_fun == spell_null )
					{
						send_to_char( "Nie ma takiego zaklecia.\n\r\n\r", ch );
						return FALSE;
					}

					pObj->value[ 1 ] = tmp;
					send_to_char( "Zaklecie ustawione.\n\r\n\r", ch );
					break;
				case 2:
					tmp = atoi( argument );

					if ( tmp < 0 )
					{
						send_to_char( "Niewlasciwy zakres (0..n).\n\r\n\r", ch );
						return FALSE;
					}

					pObj->value[ 2 ] = tmp;
					send_to_char( "Limit ustawiony.\n\r\n\r", ch );

					break;
				case 3:
					tmp = atoi( argument );

					if ( tmp < 1 || tmp > 10 )
					{
						send_to_char( "Niewlasciwy zakres (1..10).\n\r\n\r", ch );
						return FALSE;
					}

					pObj->value[ 3 ] = tmp;
					send_to_char( "Ryzyko sutawione.\n\r\n\r", ch );
					break;
				case 4:

					tmp = atoi( argument );
					if ( tmp < 1 || tmp > MAX_LEVEL )
					{
						send_to_char( "Niewlasciwy zakres.\n\r\n\r", ch );
						return FALSE;
					}

					pObj->value[ 4 ] = tmp;
					send_to_char( "Min. level ustawiony.\n\r\n\r", ch );
					break;
			}
			break;


		case ITEM_PIECE:
			switch ( value_num )
			{
				default:
					return FALSE;
				case 0:
					send_to_char( "VNum set.\n\r\n\r", ch );
					pObj->value[ 0 ] = atoi( argument );
					break;
				case 1:
					send_to_char( "Vnum set.\n\r\n\r", ch );
					pObj->value[ 1 ] = atoi( argument );
					break;
				case 2:
					send_to_char( "Vnum set.\n\r\n\r", ch );
					pObj->value[ 2 ] = atoi( argument );
					break;
			}
			break;

		case ITEM_BANDAGE:
			switch ( value_num )
			{
				default:
					return FALSE;
				case 0:
					send_to_char( "Value set.\n\r\n\r", ch );
					pObj->value[ 0 ] = UMAX( 1, atoi( argument ) );
					break;
				case 1:
					send_to_char( "Healing bonus set.\n\r\n\r", ch );
					pObj->value[ 1 ] = URANGE( 0, atoi( argument ), 50 );
					break;
			}
			break;

		case ITEM_SHIELD:
			switch ( value_num )
			{
				default:
					return FALSE;
				case 0:
					{
						int type, num = -1;
						if( argument[0] != '\0' )
						{
							if(is_number( argument ) )
								num = UMAX( 0, atoi( argument ) );

							for( type = 0; ShieldList[type].name; type++ )
								if( ( num && num == type ) || !str_prefix( ShieldList[type].name, argument) )
								{
									pObj->value[ 0 ] = type;
                                    print_char( ch,  "Ustawiono typ tarczy: %s.\n\r", ShieldList[type].name );
                                    /**
                                     * dla zmiany typu ustaw równie¿ ograniczenia
                                     */
                                    pObj->value[ 1 ] = URANGE( 1, pObj->value[ 1 ] , ShieldList[pObj->value[0]].maxAC       );
                                    pObj->value[ 2 ] = URANGE( 0, pObj->value[ 2 ] , ShieldList[pObj->value[0]].maxDam      );
                                    pObj->value[ 3 ] = URANGE( 0, pObj->value[ 3 ] , ShieldList[pObj->value[0]].otherChance );
									return TRUE;
								}
						}
						send_to_char( "Typ tarczy\n\r", ch );
						for( type = 0; ShieldList[type].name; type++ )
							print_char(ch, "%-12.12s Bonus do pancerza: %d Maksymalne obra¿enia: %d Szansa na blok:%d\r\n",
									ShieldList[type].name,
									ShieldList[type].maxAC,
									ShieldList[type].maxDam,
									ShieldList[type].otherChance);

						return FALSE;
					}
					break;

				case 1:
					send_to_char( "Bonus AC set.\n\r\n\r", ch );
					pObj->value[ 1 ] = URANGE( 1, atoi( argument ), ShieldList[pObj->value[0]].maxAC);
                                    print_char( ch,  "Ustawiono bonus do pancerza: %d.\n\r", pObj->value[ 1 ] );
					break;

				case 2:
					pObj->value[ 2 ] = URANGE( 0, atoi( argument ), ShieldList[pObj->value[0]].maxDam );
                                    print_char( ch, "Ustawiono maksymalne obra¿enia: %d.\n\r", pObj->value[ 2 ] );
					break;

				case 3:
					send_to_char( "Chance to block set.\n\r\n\r", ch );
					pObj->value[ 3 ] = URANGE( 0, atoi( argument ), ShieldList[pObj->value[0]].otherChance );
                                    print_char( ch,  "Ustawiono szansê na blok: %d.\n\r", pObj->value[ 3 ] );
					break;

				case 4:
					pObj->value[ 4 ] = URANGE( -6, atoi( argument ), 6 );
                                    print_char( ch, "Ustawiono modyfikator trafienia: %d.\n\r", pObj->value[ 4 ] );
					break;

				case 5:
					pObj->value[ 5 ] = URANGE( -6, atoi( argument ), 6 );
                                    print_char( ch, "Ustawiono modyfikator obra¿eñ: %d.\n\r", pObj->value[ 5 ] );
					break;

			}
			break;

		case ITEM_ENVENOMER:
			switch ( value_num )
			{
				default:
					return FALSE;
				case 1:
					send_to_char( "Max envenoming set.\n\r\n\r", ch );
					tmp = atoi( argument );
					pObj->value[ 1 ] = URANGE( 0, tmp, 20 );
					break;
				case 0:
					if ( argument[ 0 ] == '\0' ||
							!is_number( argument ) ||
							( tmp = atoi( argument ) ) < 0 ||
							tmp > MAX_POISON )
					{
						show_poison_list(ch);
						return FALSE;
					}
					send_to_char( "Poison set.\n\r\n\r", ch );
					pObj->value[ 0 ] = URANGE( 0, tmp, MAX_POISON );
					break;
			}
			break;

		case ITEM_TURN:
			switch ( value_num )
			{
				default:
					return FALSE;
				case 1:
					send_to_char( "Bonus damage set.\n\r\n\r", ch );
					tmp = atoi( argument );
					pObj->value[ 1 ] = URANGE( -50, tmp, 50 );
					break;
				case 0:
					send_to_char( "Bonus skill set.\n\r\n\r", ch );
					tmp = atoi( argument );
					pObj->value[ 0 ] = URANGE( -25, tmp, 25 );
					break;
			}
			break;

      case ITEM_TOTEM:
         switch ( value_num )
         {
            default:
               do_help ( ch, "ITEM_TOTEM" );
               return FALSE;
            case 0:
               send_to_char( "Typ totemu ustawiony.\n\r\n\r", ch );
               ALT_FLAGVALUE_SET( pObj->value[ 0 ], totem_table, argument );
               break;
            case 1:
               send_to_char( "Moc totemu ustawiona.\n\r\n\r", ch );
					tmp = atoi( argument );
					pObj->value[ 1 ] = URANGE( 1, tmp, 9 );
					break;
         }
         break;

			/* WEAPONS changed in ROM */
		case ITEM_WEAPON:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_WEAPON" );
					return FALSE;
				case 0:
					send_to_char( "WEAPON CLASS SET.\n\r\n\r", ch );
					ALT_FLAGVALUE_SET( pObj->value[ 0 ], weapon_class, argument );
					break;
				case 1:
					send_to_char( "NUMBER OF DICE SET.\n\r\n\r", ch );
					pObj->value[ 1 ] = atoi( argument );
					break;
				case 2:
					send_to_char( "TYPE OF DICE SET.\n\r\n\r", ch );
					pObj->value[ 2 ] = atoi( argument );
					break;
				case 3:
					send_to_char( "WEAPON TYPE SET.\n\r\n\r", ch );
					pObj->value[ 3 ] = attack_lookup( argument );
					break;
				case 4:
					send_to_char( "SPECIAL WEAPON TYPE TOGGLED.\n\r\n\r", ch );
					ALT_FLAGVALUE_TOGGLE( pObj->value[ 4 ], weapon_type2, argument );
					break;
				case 5:
					send_to_char( "TO HIT SET.\n\r\n\r", ch );
					pObj->value[ 5 ] = atoi( argument );
					break;
				case 6:
					send_to_char( "TO DAM SET.\n\r\n\r", ch );
					pObj->value[ 6 ] = atoi( argument );
					break;
			}
			break;

		case ITEM_MUSICAL_INSTRUMENT:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_MUSICAL_INSTRUMENT" );
					return FALSE;
				case 0:
					send_to_char( "INSTRUMENT CLASS SET.\n\r\n\r", ch );
					ALT_FLAGVALUE_SET( pObj->value[ 0 ], instrument_class, argument );
					break;
				case 1:
					send_to_char( "SKILL BONUS SET.\n\r\n\r", ch );
					pObj->value[ 1 ] = atoi( argument );
					break;
				case 2:
					send_to_char( "TUNING SET.\n\r\n\r", ch );
					pObj->value[ 2 ] = URANGE(1,atoi( argument ),100);
					break;
			}
			break;

		case ITEM_PORTAL:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_PORTAL" );
					return FALSE;

				case 0:
					send_to_char( "CHARGES SET.\n\r\n\r", ch );
					pObj->value[ 0 ] = atoi ( argument );
					break;
				case 1:
					send_to_char( "EXIT FLAGS SET.\n\r\n\r", ch );
					ALT_FLAGVALUE_SET( pObj->value[ 1 ], exit_flags, argument );
					break;
				case 2:
					send_to_char( "PORTAL FLAGS SET.\n\r\n\r", ch );
					ALT_FLAGVALUE_SET( pObj->value[ 2 ], portal_flags, argument );
					break;
				case 3:
					send_to_char( "EXIT VNUM SET.\n\r\n\r", ch );
					pObj->value[ 3 ] = atoi ( argument );
					break;
			}
			break;

		case ITEM_FURNITURE:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_FURNITURE" );
					return FALSE;

				case 0:
					send_to_char( "NUMBER OF PEOPLE SET.\n\r\n\r", ch );
					pObj->value[ 0 ] = atoi ( argument );
					break;
				case 1:
					send_to_char( "MAX WEIGHT SET.\n\r\n\r", ch );
					pObj->value[ 1 ] = atoi ( argument );
					break;
				case 2:
					send_to_char( "FURNITURE FLAGS TOGGLED.\n\r\n\r", ch );
					ALT_FLAGVALUE_TOGGLE( pObj->value[ 2 ], furniture_flags, argument );
					break;
				case 3:
					send_to_char( "HEAL RATE SET.\n\r\n\r", ch );
					pObj->value[ 3 ] = atoi ( argument );
					break;
				case 4:
					send_to_char( "MANA RATE SET.\n\r\n\r", ch );
					pObj->value[ 4 ] = atoi ( argument );
					break;
			}
			break;

		case ITEM_CORPSE_NPC:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_CORPSE_NPC" );
					return FALSE;
				case 5:
					send_to_char( "NPC CORPSE FLAG SET.\n\r\n\r", ch );
					ALT_FLAGVALUE_TOGGLE( pObj->value[ 5 ], corpse_flags, argument );
					break;
				case 6:
					send_to_char( "SKIN VNUM SET.\n\r\n\r", ch );
					pObj->value[ 6 ] = atoi ( argument );
					break;
			}
			break;

		case ITEM_CONTAINER:
			switch ( value_num )
			{
				int value;

				default:
				do_help( ch, "ITEM_CONTAINER" );
				return FALSE;
				case 0:
				send_to_char( "WEIGHT CAPACITY SET.\n\r\n\r", ch );
				pObj->value[ 0 ] = atoi( argument );
				break;
				case 1:
				if ( ( value = flag_value( container_flags, argument ) ) != NO_FLAG )
					TOGGLE_BIT( pObj->value[ 1 ], value );
				else
				{
					do_help ( ch, "ITEM_CONTAINER" );
					return FALSE;
				}
				send_to_char( "CONTAINER TYPE SET.\n\r\n\r", ch );
				break;
				case 2:
				if ( atoi( argument ) != 0 )
				{
					if ( !get_obj_index( atoi( argument ) ) )
					{
						send_to_char( "THERE IS NO SUCH ITEM.\n\r\n\r", ch );
						return FALSE;
					}

					if ( get_obj_index( atoi( argument ) ) ->item_type != ITEM_KEY )
					{
						send_to_char( "THAT ITEM IS NOT A KEY.\n\r\n\r", ch );
						return FALSE;
					}
				}
				send_to_char( "CONTAINER KEY SET.\n\r\n\r", ch );
				pObj->value[ 2 ] = atoi( argument );
				break;
				case 3:
				send_to_char( "CONTAINER MAX WEIGHT SET.\n\r", ch );
				pObj->value[ 3 ] = atoi( argument );
				break;
				case 4:
				send_to_char( "WEIGHT MULTIPLIER SET.\n\r\n\r", ch );
				pObj->value[ 4 ] = atoi ( argument );
				break;
				case 5:
				if ( is_number( argument ) )
				{
					if ( atoi(argument) < 1 ) {
						value = 0;
					}
					else {
						if ( !get_obj_index( atoi( argument ) ) )
						{
							send_to_char( "Nie ma takiego przedmiotu.\n\r", ch );
							return FALSE;
						}
						value = atoi( argument );
					}
				}
				else
				{
					if ( !str_cmp( argument, "wszystkie" ) || !str_cmp( argument, "dowolne" ) || !str_cmp( argument, "ró¿ne" ) || !str_cmp( argument, "none" )  || !str_cmp( argument, "brak" ) )
					{
						value = 0;
					}
					else if ( ( value = flag_value( type_flags, argument ) ) == NO_FLAG )
					{
						send_to_char( "Nie ma takiego typu.\n\r", ch );
						return FALSE;
					}
				}
				pObj->value[5] = value;
				pObj->value[6] = -1;
				send_to_char( "Parametr ustawiony.\n\r\n\r", ch );
				break;
				case 6:
				if ( pObj->value[5] == ITEM_WEAPON )
				{
					if ( !str_cmp( argument, "wszystkie" ) )
						pObj->value[6] = -1;
					else
						ALT_FLAGVALUE_SET( pObj->value[6], weapon_class, argument );
					send_to_char( "Typ broni ustawiony.\n\r\n\r", ch );
				}
				break;
			}
			break;

		case ITEM_DRINK_CON:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_DRINK" );
					/* OLC		    do_help( ch, "liquids" );    */
					return FALSE;
				case 0:
					send_to_char( "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
					pObj->value[ 0 ] = atoi( argument );
					break;
				case 1:
					send_to_char( "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
					pObj->value[ 1 ] = atoi( argument );
					break;
				case 2:
					send_to_char( "LIQUID TYPE SET.\n\r\n\r", ch );
					pObj->value[ 2 ] = ( liq_lookup( argument ) != -1 ?
							liq_lookup( argument ) : 0 );
					break;
				case 3:
					send_to_char( "POISON VALUE TOGGLED.\n\r\n\r", ch );
					pObj->value[ 3 ] = ( pObj->value[ 3 ] == 0 ) ? 1 : 0;
					break;
				case 4:
					{
						OBJ_INDEX_DATA *empty;
						int vnum;

						if ( !is_number( argument ) )
						{
							send_to_char( "{RPodaj vnum przedmiotu, który pojawi siê po opró¿nieniu tego z jego\n\r"
									"oryginalnej zawarto¶ci.{x\n\r", ch );
							break;
						}

						vnum = atoi( argument );

						if ( vnum != 0 )
						{
							if ( ( empty = get_obj_index( vnum ) ) == NULL )
							{
								send_to_char( "{RNie ma przedmiotu o takim vnumie.{x\n\r", ch );
								break;
							}

							if ( empty->item_type != ITEM_DRINK_CON )
							{
								send_to_char( "{RPrzedmiot musi byc typu drinkcontainer.{x\n\r", ch );
								break;
							}

							send_to_char( "{GVnum opró¿nionego przedmiotu ustawiony.{x\n\r\n\r", ch );
						} else
							send_to_char( "{GVnum opró¿nionego przedmiotu usuniêty.{x\n\r\n\r", ch );

						pObj->value[ 4 ] = vnum;
					}
					break;
			}
			break;

		case ITEM_FOUNTAIN:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_FOUNTAIN" );
					/* OLC		    do_help( ch, "liquids" );    */
					return FALSE;
				case 0:
					send_to_char( "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
					pObj->value[ 0 ] = atoi( argument );
					break;
				case 1:
					send_to_char( "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
					pObj->value[ 1 ] = atoi( argument );
					break;
				case 2:
					send_to_char( "LIQUID TYPE SET.\n\r\n\r", ch );
					pObj->value[ 2 ] = ( liq_lookup( argument ) != -1 ?
							liq_lookup( argument ) : 0 );
					break;
			}
			break;

		case ITEM_FOOD:
			tmp = 0;
			if ( is_number( argument ) ) tmp = UMAX( 0, atoi( argument ) );
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_FOOD" );
					return FALSE;
				case 0:
					print_char( ch, "Po¿ywno¶æ ustawiona. Jedzenia wystarczy na %d kêsów.\n\r\n\r", 1 + ( tmp - 1 ) / 6 );
					pObj->value[ 0 ] = tmp;
					break;
				case 1:
					print_char( ch, "Jedzonko zepsuje siê po %d tickach.\n\r\n\r", tmp );
					pObj->value[ 1 ] = tmp;
					break;
				/* start food state - by Fuyara */
				case 2:
				{
				    int  state = get_food_state_value( argument );
				    char buf[MSL];

				    if ( state == -1 )
				    {
					sprintf( buf, "Nie ma takiego stanu jedzenia jak '%s'.\n\r\n\r", argument );
					show_food_state_table( ch );
				    }
				    else
				    {
					sprintf( buf, "Stan jedzenia ustawiono na: %s.\n\r\n\r", argument );
					pObj->value[2] = state;
				    }

				    send_to_char( buf, ch );
				}
				break;
				/* end food state */
				case 3:
				{
					if ( pObj->value[ 3 ] == 1 )
						send_to_char( "Jedzenie nie bêdzie zatrute.\n\r\n\r", ch );
					else
						send_to_char( "Jedzenie bêdzie zatrute.\n\r\n\r", ch );
					pObj->value[ 3 ] = ( pObj->value[ 3 ] == 0 ) ? 1 : 0;
				}
				break;
				case 4:
				{
				    int  state = get_food_type_value( argument );
				    char buf[MSL];

				    if ( state == -1 )
				    {
					sprintf( buf, "Nie ma takiego typu jedzenia jak '%s'.\n\r\n\r", argument );
					show_food_type_table( ch );
				    }
				    else
				    {
					sprintf( buf, "Typ jedzenia ustawiono na: %s.\n\r\n\r", argument );
					pObj->value[4] = state;
				    }

				    send_to_char( buf, ch );
				}
				break;
			}
			break;

		case ITEM_MONEY:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_MONEY" );
					return FALSE;
				case 0:
					send_to_char( "COPPER AMOUNT SET.\n\r\n\r", ch );
					pObj->value[ 0 ] = atoi( argument );
					break;
				case 1:
					send_to_char( "SILVER AMOUNT SET.\n\r\n\r", ch );
					pObj->value[ 1 ] = atoi( argument );
					break;
				case 2:
					send_to_char( "GOLD AMOUNT SET.\n\r\n\r", ch );
					pObj->value[ 2 ] = atoi( argument );
					break;
				case 3:
					send_to_char( "MITHRIL AMOUNT SET.\n\r\n\r", ch );
					pObj->value[ 3 ] = atoi( argument );
					break;
			}
			break;

		case ITEM_PIPE:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_PIPE" );
					return FALSE;
					/*case 0:
					  if ( pObj->value[ 0 ] == 1 )
					  send_to_char( "Fajka bêdzie zgaszona.\n\r\n\r", ch );
					  else
					  send_to_char( "Fajka bêdzie zapalona.\n\r\n\r", ch );
					  pObj->value[ 0 ] = ( pObj->value[ 0 ] == 0 ) ? 1 : 0;
					  break;*/
				case 1:
					send_to_char( "Ilo¶æ ziela w fajce.\n\r\n\r", ch );
					pObj->value[ 1 ] = atoi( argument );
					break;
				case 2:
					send_to_char( "Typ ziela w fajce.\n\r\n\r", ch );
					pObj->value[ 2 ] = ( weed_lookup( argument ) != -1 ?
							weed_lookup( argument ) : 0 );
					break;

			}
			break;

		case ITEM_WEED:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_WEED" );
					return FALSE;
				case 0:
					send_to_char( "Ilo¶æ ziela w fajce.\n\r\n\r", ch );
					pObj->value[ 0 ] = atoi( argument );
					break;
				case 1:
					send_to_char( "Typ ziela w fajce.\n\r\n\r", ch );
					pObj->value[ 1 ] = ( weed_lookup( argument ) != -1 ?
							weed_lookup( argument ) : 0 );
					break;

			}
			break;
		case ITEM_NOTEPAPER:
			switch ( value_num )
			{
				default:
					do_help( ch, "ITEM_NOTEPAPER" );
					return FALSE;
				case 0:
					  if ( pObj->value[ 0 ] == 1 )
					  send_to_char( "Notatka ustawiona jako niezapisana.\n\r\n\r", ch );
					  else
					  send_to_char( "Notatka ustawiona jako zapisana.\n\r\n\r", ch );
					  pObj->value[ 0 ] = ( pObj->value[ 0 ] == 0 ) ? 1 : 0;
					  break;


			}
			break;
			case ITEM_TOOL: //rellik: mining
				tmp = 0;
				if ( is_number( argument ) ) tmp = atoi( argument );
				switch ( value_num )
				{
				default:
					do_help( ch, "ITEM_TOOL" );
					return FALSE;
				case 0:
					if ( tmp >0 && tmp <= 9 )
					{
						print_char( ch, "Jako¶æ narzêdzia ustawione na: %d.\n\r\n\r", tmp );
						pObj->value[0] = tmp;
					} else {
						print_char( ch, "Jako¶æ narzêdzia musi byæ z przedzia³u 1..9.\n\r\n\r" );
						return FALSE;
					}
					break;
				case 1:
					if ( tmp >= 0 && tmp <=100 )
					{
						print_char( ch, "Poziom umiejêtno¶ci wymagany do stosowania ustawiony na: %d.\n\r\n\r", tmp );
						pObj->value[1] = tmp;
					} else {
						print_char( ch, "Poziom umiejêtno¶ci wymagany do stosowania musi byæ z przedzia³u 0..100.\n\r\n\r" );
						return FALSE;
					}
					break;
				case 2:
					if ( ( tmp = tool_lookup( argument ) ) != -1 )
					{
						print_char( ch, "Typ narzêdzia ustawiony na: %s.\n\r\n\r", tool_table[tmp].name );
						pObj->value[2] = tool_table[tmp].bit;
					}
                    else
                    {
                        sh_int x;
                        char buf[MAX_STRING_LENGTH];
                        print_char( ch, "Typ narzêdzia nie znaleziony. Istnieja nastepujace typy narzedzi:\n\r" );
                        for(x=0;tool_table[x].name;x++)
                        {
                            sprintf( buf, "%s\n\r", tool_table[x].name );
                            send_to_char( buf, ch );
                        }
                        print_char( ch, "\n\r" );
                        return FALSE;
                    }
					break;
				}

	}

	show_obj_values( ch, pObj );

	return TRUE;
}

void specdam_to_char( CHAR_DATA *ch, int count, SPEC_DAMAGE *specdam )
{
	char spbuf1[MAX_STRING_LENGTH];
	char spbuf2[MAX_STRING_LENGTH];
	char spbuf3[MAX_STRING_LENGTH];

	char *specdam_target_type;

	if ( !ch || !specdam )
		return;

	spbuf1[0] = spbuf2[0] = spbuf3[0] = '\0';

	specdam_target_type = specdam->target_type ? "Umagicznienie" : "Obra¿enia";

	switch ( specdam->type )
	{
		case SD_TYPE_MAGIC:
			sprintf( spbuf1, "{CMAGIC{x Zmienia: {C%s{X Rodzaj: {C%s{X Bonus: {C%d{x Szansa: {C%d{x\n\r",
                                        specdam_target_type,
					dam_types[specdam->param1].name,
					specdam->bonus,
					specdam->chance );
			break;
		case SD_TYPE_RACE:
			sprintf( spbuf1, "{CRACE{x Zmienia: {C%s{X Cel: {C%s{x Atakuj±cy: {C%s{x Bonus: {C%d{x Szansa: {C%d{x\n\r",
                                        specdam_target_type,
					specdam->param1 == SD_PARAM_ANY ? "any" : race_table[specdam->param1].name,
					specdam->param2 == SD_PARAM_ANY ? "any" : race_table[specdam->param2].name,
					specdam->bonus,
					specdam->chance );
			break;
		case SD_TYPE_ALIGN:
			if ( specdam->param1 == SD_PARAM_ANY )
				sprintf( spbuf2, "any" );
			else
				sprintf( spbuf2, specdam->param1 == 0 ? "evil" : specdam->param1 == 1 ? "good" : "neutral" );

			if ( specdam->param2 == SD_PARAM_ANY )
				sprintf( spbuf3, "any" );
			else
				sprintf( spbuf3, specdam->param2 == 0 ? "evil" : specdam->param2 == 1 ? "good" : "neutral" );

			sprintf( spbuf1, "{CALIGN{x Zmienia: {C%s{X Cel: {C%s{x Atakuj±cy: {C%s{x Bonus: {C%d{x Szansa: {C%d{x\n\r",
                                        specdam_target_type,
					spbuf2,
					spbuf3,
					specdam->bonus,
					specdam->chance );
			break;
		case SD_TYPE_SIZE:
			sprintf( spbuf1, "{CSIZE{x Zmienia: {C%s{X Cel: {C%s{x Atakuj±cy: {C%s{x Bonus: {C%d{x Szansa: {C%d{x\n\r",
                                        specdam_target_type,
					specdam->param1 == SD_PARAM_ANY ? "any" : size_table[specdam->param1].name,
					specdam->param2 == SD_PARAM_ANY ? "any" : size_table[specdam->param2].name,
					specdam->bonus,
					specdam->chance );
			break;
		case SD_TYPE_SELFSTAT:
			sprintf( spbuf1, "{CSELFSTAT{x Zmienia: {C%s{X Stat: {C%s{x Warto¶æ: {C%s %d{x Bonus: {C%d{x Szansa: {C%d{x\n\r",
                                        specdam_target_type,
					stat_table[specdam->param1].name,
					fn_evals[specdam->param2],
					specdam->param3,
					specdam->bonus,
					specdam->chance );
			break;
		case SD_TYPE_TARGETSTAT:
			sprintf( spbuf1, "{CTARGETSTAT{x Zmienia: {C%s{X Stat: {C%s{x Warto¶æ: {C%s %d{x Bonus: {C%d{x Szansa: {C%d{x\n\r",
                                        specdam_target_type,
					stat_table[specdam->param1].name,
					fn_evals[specdam->param2],
					specdam->param3,
					specdam->bonus,
					specdam->chance );
			break;
		case SD_TYPE_CLASS:
			sprintf( spbuf1, "{CCLASS{x Zmienia: {C%s{X Profesja atakuj±cego: {C%s{x Bonus: {C%d{x Szansa: {C%d{x\n\r",
                                        specdam_target_type,
					specdam->param1 == SD_PARAM_ANY ? "any" : class_table[specdam->param1].name,
					specdam->bonus,
					specdam->chance );
			break;
		case SD_TYPE_ACT:
			sprintf( spbuf1, "{CACT{x Zmienia: {C%s{X Act celu: {C%s{x Bonus: {C%d{x Szansa: {C%d{x\n\r",
                                        specdam_target_type,
					ext_flag_string( act_flags, specdam->ext_param ),
					specdam->bonus,
					specdam->chance );
	}

	printf_to_char( ch, "[%5d] ", count );
	send_to_char( spbuf1, ch );
	return;
}

OEDIT( oedit_show )
{
	OBJ_INDEX_DATA *pObj;
	SPEC_DAMAGE    *specdam;
	char buf[MAX_STRING_LENGTH];
	AFFECT_DATA *paf;
	PROG_LIST *list;
	int cnt;
	float kg;

	EDIT_OBJ(ch, pObj);

	sprintf( buf, "{GName{x:         [%s]\n\r{GArea{x:         [%5d] %s\n\r",
			pObj->name,
			!pObj->area ? -1        : pObj->area->vnum,
			!pObj->area ? "No Area" : pObj->area->name );
	send_to_char( buf, ch );


	sprintf(buf,
			"{GDope³niacz{x:   [%s] ({Gczego nie ma?{x)\n\r"
			"{GCelownik{x:     [%s] ({Gczemu siê przygl±dam?{x)\n\r"
			"{GBiernik{x:      [%s] ({Gco widzê?{x)\n\r"
			"{GNarzêdnik{x:    [%s] ({Gczym siê pos³ugujê?{x)\n\r"
			"{GMiejscownik{x:  [%s] ({Go czym mówiê?{x)\n\r",
			pObj->name2,pObj->name3,pObj->name4,pObj->name5,pObj->name6);
	send_to_char(buf,ch);

	sprintf( buf, "{GVnum{x:         [%5d]\n\r{GType{x:         [%s] {Y? type{x\n\r",
			pObj->vnum,
			flag_string( type_flags, pObj->item_type ) );
	send_to_char( buf, ch );

	// levelu nie ma sensu pokazywaæ, bo i tak do niczego nie s³u¿y
	//	sprintf( buf, "{GLevel{x:        [%5d]\n\r", pObj->level );
	//	send_to_char( buf, ch );

	sprintf( buf, "{GWear flags{x:   [%s] {Y? wear-loc{x\n\r", flag_string( wear_flags, pObj->wear_flags ) );
	send_to_char( buf, ch );

	sprintf( buf, "{GExtra flags{x:  [%s] {Y? extra{x\n\r", ext_flag_string( extra_flags, pObj->extra_flags ) );
	send_to_char( buf, ch );

	sprintf( buf, "{GExwear flags{x: [%s] {Y? exwear{x\n\r", ext_flag_string( wear_flags2, pObj->wear_flags2 ) );
	send_to_char( buf, ch );

	sprintf( buf, "{GMaterial{x:     [%s]  {Ymaterial{X\n\r", material_table[pObj->material].name);
	send_to_char( buf, ch );

	sprintf( buf, "{GCondition{x:    [%5d]\n\r", pObj->condition );
	send_to_char( buf, ch );

    /**
     * repair limits
     */
    sprintf( buf, "{GRepair limit{x: [%5d]  {Yrepair{x\n\r", pObj->repair_limit ); send_to_char( buf, ch );
    sprintf( buf, "{GR. counter{x:   [%5d]\n\r", pObj->repair_counter ); send_to_char( buf, ch );
    sprintf( buf, "{GR. penalty{x:   [%5d]\n\r", pObj->repair_penalty ); send_to_char( buf, ch );
    sprintf( buf, "{GR. condition{x: [%5d]\n\r", pObj->repair_condition ); send_to_char( buf, ch );

    /**
     * weight
     */
    kg = (float) pObj->weight / 22.05;
    sprintf( buf, "{GWeight{x:       [%5d] ({G%.2f kg{x)\n\r", pObj->weight, kg );
    send_to_char( buf, ch );
    /**
     * cost
     */
    sprintf( buf, "{GCost{x:         [%5d] %s\n\r", pObj->cost, money_string_short( pObj->cost ) );
    send_to_char( buf, ch );
    /**
     * bonus
     */
    if ( pObj->bonus_set > 0 )
    {
        sprintf( buf, "{GBonus set{x:    [%5d]\n\r", pObj->bonus_set );
        send_to_char( buf, ch );
    }
    /**
     * rent
     */
    sprintf( buf, "{GRent/day{x:     [%5d] %s - wyliczone jako %d%% ceny.\n\r", pObj->rent_cost, money_string_short( pObj->rent_cost ), RENT_COST_RATE );
    send_to_char( buf, ch );

    //rellik: komponenty, pokazywanie w³a¶ciwo¶ci je¶li jest komponentem
    if ( pObj->is_spell_item )
    {
    	send_to_char( "{GKomponent do czarów{x:     tak\n\r", ch );
    	if ( pObj->spell_item_counter < 0 )
    	{
    		send_to_char( "{GIlo¶æ u¿yæ komponentu{x:     bez limitu\n\r", ch );
    	} else {
    		sprintf( buf, "{GIlo¶æ u¿yæ komponentu{x:     [%5d]\n\r", pObj->spell_item_counter );
    		send_to_char( buf, ch );
    	}
  		sprintf( buf, "{GCzas przydatno¶ci komponentu{x:   [%5d]\n\r", pObj->spell_item_timer );
  		send_to_char( buf, ch );
    }

	sprintf( buf, "{GLiczba{x:       [%s]\n\r",
			pObj->liczba_mnoga == 0 ? "pojedyñcza" : "mnoga" );
	send_to_char( buf, ch );

	sprintf( buf, "{GRodzaj{x:       [%s]\n\r",
			gender_table[ pObj->gender ].name );
	send_to_char( buf, ch );

	if( pObj->trap > 0)
	{
		sprintf(buf,"{GTrap vnum{x:    [%5d]\n\r", pObj->trap);
		send_to_char(buf, ch);
	}

	if ( pObj->extra_descr )
	{
		EXTRA_DESCR_DATA *ed;

		send_to_char( "{GEx desc kwd{x:  ", ch );

		for ( ed = pObj->extra_descr; ed; ed = ed->next )
		{
			send_to_char( "[", ch );
			send_to_char( ed->keyword, ch );
			send_to_char( "]", ch );
		}

		send_to_char( "\n\r", ch );
	}

	sprintf( buf, "{GShort desc{x:   %s\n\r{GLong desc{x:\n\r     %s\n\r",
			pObj->short_descr, pObj->description );
	send_to_char( buf, ch );


	if(pObj->item_description)
	{
		char long_buf[MIL];
		sprintf( long_buf, "{GItem desc{x:\n\r%.255s\n\r", pObj->item_description);
		send_to_char( long_buf, ch );
	}

	if(pObj->ident_description)
	{
		char long_buf[MIL];
		sprintf( long_buf, "{GIdent desc{x:\n\r%.255s\n\r", pObj->ident_description );
		send_to_char( long_buf, ch );
	}

	if(pObj->hidden_description)
	{
		char long_buf[MIL];
		sprintf( long_buf, "{GHidden desc{x:\n\r%.255s\n\r", pObj->hidden_description );
		send_to_char( long_buf, ch );
	}


	for ( cnt = 0, paf = pObj->affected; paf; paf = paf->next )
	{
		if ( cnt == 0 )
		{
			send_to_char( "{CAffects{x:\n\r", ch );
			send_to_char( "Number Modifier Affects\n\r", ch );
			send_to_char( "------ -------- -------\n\r", ch );
		}

		switch(paf->location)
		{
			case APPLY_SKILL:
				sprintf( buf, "[%4d] skill %s o %d ", cnt,
						skill_table[paf->modifier].name,paf->level);
				break;
			case APPLY_LANG:
				sprintf( buf, "[%4d] jêzyk %s o %d ", cnt,
						lang_table[paf->modifier].name,paf->level);
				break;
			case APPLY_MEMMING:
				sprintf( buf, "[%4d] memming krag %d o %d ", cnt,
						paf->modifier,paf->level);
				break;
			case APPLY_RESIST:
				sprintf( buf, "[%4d] odpornosc na %s o %d ", cnt,
						resist_name(paf->modifier),paf->level);
				break;
			default :
				sprintf( buf, "[%4d] %-8d %s ", cnt,
						paf->modifier,
						flag_string( apply_flags, paf->location ) );
				break;

		}

		if ( paf->bitvector && paf->bitvector != &AFF_NONE )
		{
			strcat( buf,"[" );
			strcat( buf, affect_bit_name( NULL, paf->bitvector ) );
			strcat( buf,"]\n\r" );
		}
		else
			strcat( buf,"\n\r" );


		send_to_char( buf, ch );
		cnt++;
	}

	show_obj_values( ch, pObj );

	if ( pObj->progs )
	{
		int cnt;

		sprintf(buf, "\n\r{COBJProgramy dla [%5d]{x:\n\r", pObj->vnum);
		send_to_char( buf, ch );

		for (cnt=0, list=pObj->progs; list; list=list->next)
		{
			if (cnt ==0)
			{
				send_to_char( "{CValues{x:\n\r", ch );
				send_to_char ( " Number Name            Trigger    Phrase\n\r", ch );
				send_to_char ( " ------ --------------- ---------- ---------\n\r", ch );
			}

			sprintf(buf, "[%5d] %-15s {C%-10s{x %s\n\r", cnt,
					list->name, ext_bit_name( prog_flags, list->trig_type ),
					list->trig_phrase);
			send_to_char( buf, ch );
			cnt++;
		}
	}

	if ( pObj->item_type == ITEM_WEAPON && pObj->spec_dam )
	{
		int cnt;

		sprintf(buf, "\n\r{CSpec damages dla [%5d]{x:\n\r", pObj->vnum);
		send_to_char( buf, ch );

		for ( cnt = 1, specdam = pObj->spec_dam; specdam; specdam = specdam->next )
		{
			specdam_to_char( ch, cnt, specdam );
			cnt++;
		}
	}

	if ( !IS_NULLSTR( pObj->comments ) )
	{
		send_to_char( "\n\r{CKomentarze{x:\n\r", ch );
		send_to_char( numlineas( pObj->comments ), ch );
	}

	return FALSE;
}


/*
 * Need to issue warning if flag isn't valid. -- does so now -- Hugin.
 */

OEDIT( oedit_odmiana )
{
	OBJ_INDEX_DATA *pObj;
	char arg1[MAX_INPUT_LENGTH];

	do
	{
		if ( argument[0] == '\0' )
			break;

		EDIT_OBJ(ch, pObj);
		argument = one_argument( argument, arg1);

		if ( argument[0] == '\0' )
			break;


		if(!str_prefix(arg1,"dopelniacz"))
		{
			free_string( pObj->name2 );
			pObj->name2=str_dup(argument);
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}

		if(!str_prefix(arg1,"celownik"))
		{
			free_string( pObj->name3 );
			pObj->name3=str_dup(argument);
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}

		if(!str_prefix(arg1,"biernik"))
		{
			free_string( pObj->name4 );
			pObj->name4=str_dup(argument);
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}


		if(!str_prefix(arg1,"narzednik"))
		{
			free_string( pObj->name5 );
			pObj->name5=str_dup(argument);
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}

		if(!str_prefix(arg1,"miejscownik"))
		{
			free_string( pObj->name6 );
			pObj->name6=str_dup(argument);
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}
		send_to_char( "Odmiana bledna.\n\r", ch );
		return TRUE;
	} while ( FALSE );

	send_to_char( "Sk³adnia: odmiana <przypadek> <string>\n\r" , ch );
	return FALSE;
}

OEDIT( oedit_addaffect )
{
	int value, sn=-1;
	OBJ_INDEX_DATA *pObj;
	AFFECT_DATA *pAf;
	char loc[MAX_STRING_LENGTH];
	char mod[MAX_STRING_LENGTH];
	char arg_last[MAX_STRING_LENGTH];

	EDIT_OBJ(ch, pObj);

	argument = one_argument( argument, loc );
	argument = one_argument( argument, mod );

	if ( loc[0] == '\0' || mod[0] == '\0')
	{
		send_to_char( "Sk³adnia: addaffect [location] [#xmod]\n\r", ch );
		return FALSE;
	}

	if ( ( value = flag_value( apply_flags, loc ) ) == NO_FLAG ) /* Hugin */
	{
		send_to_char( "Valid affects are:\n\r", ch );
		show_help( ch, "apply" );
		return FALSE;
	}

	if(value == APPLY_SKILL)
	{
		sn=skill_lookup(mod);
		one_argument( argument, arg_last );

		if(sn < 0 || arg_last[0]=='\0' || !is_number(arg_last) || skill_table[sn].spell_fun!=spell_null)
		{
			send_to_char( "Sk³adnia: addaffect skill [nazwa skilla] [wartosc]\n\r", ch );
			return FALSE;
		}
	}

	else if(value == APPLY_LANG)
	{
		sn=get_langnum(mod);
		one_argument( argument, arg_last );

		if(sn < 0 || sn > MAX_LANG || arg_last[0]=='\0' || !is_number(arg_last))
		{
			send_to_char( "Sk³adnia: addaffect lang [nazwa jezyka] [mod]\n\r", ch );
			return FALSE;
		}
	}
	else if(value == APPLY_RESIST)
	{
		sn=resist_number(mod);
		one_argument( argument, arg_last );

		if(sn < 0 || sn > MAX_RESIST || arg_last[0]=='\0' || !is_number(arg_last))
		{
			send_to_char( "Sk³adnia: addaffect resist [odpornosc] [wartosc%]\n\r", ch );
			return FALSE;
		}
	}

	else if(value == APPLY_MEMMING)
	{
		one_argument( argument, arg_last );

		if(!is_number(mod) || arg_last[0]=='\0' || !is_number(arg_last))
		{
			send_to_char( "Sk³adnia: addaffect spellmem [krag] [wartosc]\n\r", ch );
			return FALSE;
		}

		sn=atoi(mod);

		if(sn < 1 || sn > 11)
		{
			send_to_char( "Sk³adnia: addaffect spellmem [krag] [wartosc]\n\r", ch );
			return FALSE;
		}

	}

	else if(!is_number(mod))
	{
		send_to_char( "Sk³adnia: addaffect [location] [#xmod]\n\r", ch );
		return FALSE;
	}

	pAf             =   new_affect();
	pAf->location   =   value;

	if(sn < 0)
	{
		pAf->modifier   =   atoi( mod );
		pAf->level      =	pObj->level;
	}
	else
	{
		pAf->modifier   =   sn;
		pAf->level      =	atoi(arg_last);
	}
	pAf->where	    =   TO_OBJECT;
	pAf->type       =   -1;
	pAf->duration   =   -1;
	pAf->rt_duration =	0;
	pAf->bitvector  =   &AFF_NONE;
	pAf->next       =   pObj->affected;
	pObj->affected  =   pAf;

	send_to_char( "Affect added.\n\r", ch);
	return TRUE;
}

OEDIT( oedit_addapply )
{
	int value,typ;
	OBJ_INDEX_DATA *pObj;
	AFFECT_DATA *pAf;
	char loc[MAX_STRING_LENGTH];
	char mod[MAX_STRING_LENGTH];
	char type[MAX_STRING_LENGTH];
	char bvector[MAX_STRING_LENGTH];
	BITVECT_DATA * vector;

	EDIT_OBJ(ch, pObj);

	argument = one_argument( argument, type );
	argument = one_argument( argument, loc );
	argument = one_argument( argument, mod );
	one_argument( argument, bvector );

	if ( type[0] == '\0' || ( typ = flag_value( apply_types, type ) ) == NO_FLAG )
	{
		send_to_char( "Invalid apply type. Valid apply types are:\n\r", ch);
		show_help( ch, "apptype" );
		return FALSE;
	}

	if ( loc[0] == '\0' || ( value = flag_value( apply_flags, loc ) ) == NO_FLAG )
	{
		send_to_char( "Valid applys are:\n\r", ch );
		show_help( ch, "apply" );
		return FALSE;
	}

	if ( bvector[0] == '\0' )
	{
		send_to_char( "Invalid bitvector type.\n\r", ch );
		send_to_char( "Valid bitvector types are:\n\r", ch );
		show_help( ch, bitvector_type[typ].help );
		return FALSE;
	}

	vector = ext_flag_lookup( bvector, affect_flags );

	if( vector ==  &EXT_NONE )
	{
		send_to_char( "Invalid bitvector type.\n\r", ch );
		send_to_char( "Valid bitvector types are:\n\r", ch );
		show_help( ch, bitvector_type[typ].help );
		return FALSE;
	}


	if ( mod[0] == '\0' || !is_number( mod ) )
	{
		send_to_char( "Sk³adnia: addapply [type] [location] [#xmod] [bitvector]\n\r", ch );
		return FALSE;
	}

	pAf             =   new_affect();
	pAf->location   =   value;
	pAf->modifier   =   atoi( mod );
	pAf->where	    =   apply_types[typ].bit;
	pAf->type	    =	-1;
	pAf->duration   =   -1;
	pAf->rt_duration = 0;
	pAf->bitvector  =   vector;
	pAf->level      =	pObj->level;
	pAf->next       =   pObj->affected;
	pObj->affected  =   pAf;

	send_to_char( "Apply added.\n\r", ch);
	return TRUE;
}

/*
 * My thanks to Hans Hvidsten Birkeland and Noam Krendel(Walker)
 * for really teaching me how to manipulate pointers.
 */
OEDIT( oedit_delaffect )
{
	OBJ_INDEX_DATA *pObj;
	AFFECT_DATA *pAf;
	AFFECT_DATA *pAf_next;
	char affect[MAX_STRING_LENGTH];
	int  value;
	int  cnt = 0;

	EDIT_OBJ(ch, pObj);

	one_argument( argument, affect );

	if ( !is_number( affect ) || affect[0] == '\0' )
	{
		send_to_char( "Sk³adnia: delaffect [#xaffect]\n\r", ch );
		return FALSE;
	}

	value = atoi( affect );

	if ( value < 0 )
	{
		send_to_char( "Only non-negative affect-numbers allowed.\n\r", ch );
		return FALSE;
	}

	if ( !( pAf = pObj->affected ) )
	{
		send_to_char( "OEdit:  Non-existant affect.\n\r", ch );
		return FALSE;
	}

	if( value == 0 )	/* First case: Remove first affect */
	{
		pAf = pObj->affected;
		pObj->affected = pAf->next;
		free_affect( pAf );
	}
	else		/* Affect to remove is not the first */
	{
		while ( ( pAf_next = pAf->next ) && ( ++cnt < value ) )
			pAf = pAf_next;

		if( pAf_next )		/* See if it's the next affect */
		{
			pAf->next = pAf_next->next;
			free_affect( pAf_next );
		}
		else                                 /* Doesn't exist */
		{
			send_to_char( "No such affect.\n\r", ch );
			return FALSE;
		}
	}

	send_to_char( "Affect removed.\n\r", ch);
	return TRUE;
}



OEDIT( oedit_name )
{
	OBJ_INDEX_DATA *pObj;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: name [string]\n\r", ch );
		return FALSE;
	}

	free_string( pObj->name );
	pObj->name = str_dup( argument );

	send_to_char( "Name set.\n\r", ch);
	return TRUE;
}



OEDIT( oedit_short )
{
	OBJ_INDEX_DATA *pObj;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: short [string]\n\r", ch );
		return FALSE;
	}

	free_string( pObj->short_descr );
	pObj->short_descr = str_dup( argument );
	//pObj->short_descr[0] = LOWER( pObj->short_descr[0] );

	send_to_char( "Short description set.\n\r", ch);
	return TRUE;
}



OEDIT( oedit_long )
{
	OBJ_INDEX_DATA *pObj;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: long [string]\n\r", ch );
		return FALSE;
	}

	free_string( pObj->description );
	pObj->description = str_dup( argument );
	sprintf( pObj->description, "%s", capitalize(pObj->description) );

	send_to_char( "Long description set.\n\r", ch);
	return TRUE;
}

OEDIT( oedit_desc )
{
	OBJ_INDEX_DATA *pObj;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' )
	{
		string_append( ch, &pObj->item_description );
		return TRUE;
	}

	send_to_char( "Sk³adnia: desc    - line edit\n\r", ch );
	return TRUE;
}

OEDIT( oedit_ident )
{
	OBJ_INDEX_DATA *pObj;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' )
	{
		string_append( ch, &pObj->ident_description );
		return TRUE;
	}

	send_to_char( "Sk³adnia: ident    - line edit\n\r", ch );
	return TRUE;
}

OEDIT( oedit_hidden )
{
	OBJ_INDEX_DATA *pObj;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' )
	{
		string_append( ch, &pObj->hidden_description );
		return TRUE;
	}

	send_to_char( "Sk³adnia: hidden    - line edit\n\r", ch );
	return TRUE;
}


bool set_value( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, char *argument, int value )
{
	if ( argument[0] == '\0' )
	{
		set_obj_values( ch, pObj, -1, "" );     /* '\0' changed to "" -- Hugin */
		return FALSE;
	}

	if ( set_obj_values( ch, pObj, value, argument ) )
		return TRUE;

	return FALSE;
}



/*****************************************************************************
Name:		oedit_values
Purpose:	Finds the object and sets its value.
Called by:	The four valueX functions below. (now five -- Hugin )
 ****************************************************************************/
bool oedit_values( CHAR_DATA *ch, char *argument, int value )
{
    OBJ_INDEX_DATA *pObj;
    EDIT_OBJ(ch, pObj);
    // je¿eli co¶ jes
    if (
            pObj->item_type == ITEM_SPELLBOOK
            || pObj->item_type == ITEM_PORTAL
       )
    {
        if (!(ch->pcdata->wiz_conf & W5))
        {
            return FALSE;
        }
    }
    if ( set_value( ch, pObj, argument, value ) )
    {
        return TRUE;
    }
    return FALSE;
}


OEDIT( oedit_value0 )
{
	if ( oedit_values( ch, argument, 0 ) )
		return TRUE;

	return FALSE;
}



OEDIT( oedit_value1 )
{
	if ( oedit_values( ch, argument, 1 ) )
		return TRUE;

	return FALSE;
}



OEDIT( oedit_value2 )
{
	if ( oedit_values( ch, argument, 2 ) )
		return TRUE;

	return FALSE;
}



OEDIT( oedit_value3 )
{
	if ( oedit_values( ch, argument, 3 ) )
		return TRUE;

	return FALSE;
}



OEDIT( oedit_value4 )
{
	if ( oedit_values( ch, argument, 4 ) )
		return TRUE;

	return FALSE;
}


OEDIT( oedit_value5 )
{
	if ( oedit_values( ch, argument, 5 ) )
		return TRUE;

	return FALSE;
}

OEDIT( oedit_value6 )
{
	if ( oedit_values( ch, argument, 6 ) )
		return TRUE;

	return FALSE;
}

OEDIT( oedit_weight )
{
	OBJ_INDEX_DATA *pObj;
	float kg;
	EDIT_OBJ(ch, pObj);
	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: weight [number]\n\r", ch );
		return FALSE;
	}
	pObj->weight = UMAX(1, atoi( argument ) );
	kg = (float) pObj->weight / 22.05;
	print_char( ch, "Waga ustawiona na %.2f kg.\n\r", kg );
	return TRUE;
}

OEDIT( oedit_cost )
{
    OBJ_INDEX_DATA *pObj;
    EDIT_OBJ(ch, pObj);
    if ( argument[0] == '\0' || !is_number( argument ) )
    {
        send_to_char( "Sk³adnia: cost [number]\n\r", ch );
        return FALSE;
    }
    pObj->cost = atoi( argument );
    pObj->rent_cost = RENT_COST( pObj->cost );
    send_to_char( "Cost set.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_trap )
{
	OBJ_INDEX_DATA *pObj;
	int trap;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: trap [number]\n\r", ch );
		return FALSE;
	}

	trap = UMAX(0, atoi( argument ));

	if( trap > 0 && !get_trap_index( trap ) )
	{
		send_to_char("Pu³apka nie istnieje.\n\r", ch);
		return FALSE;
	}

	pObj->trap = trap;
	send_to_char( "Trap set.\n\r", ch);
	return TRUE;
}

OEDIT( oedit_bonus_set )
{
	OBJ_INDEX_DATA *pObj;
	BONUS_INDEX_DATA *pBonus;
	int val;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: bonus [number]\n\r", ch );
		return FALSE;
	}

	val = atoi( argument );

	if ( val < 0 )
	{
		send_to_char( "Sk³adnia: bonus [number]\n\r", ch );
		return FALSE;
	}

	if(val == 0)
	{
		pObj->bonus_set = 0;
		send_to_char( "Bonus deleted.\n\r", ch );
		return TRUE;
	}

	if ( ( pBonus = get_bonus_index( val ) ) == NULL )
	{
		send_to_char( "Nie ma bonus setu o taki vnumie. U¿yj komendy bsedit by go stworzyæ.\n\r", ch );
		return FALSE;
	}

	if ( pBonus->area != pObj->area )
	{
		send_to_char( "Bonus set musi byc w tej samej krainie co wszystkie czesci kompletu.\n\r", ch );
		return FALSE;
	}

	pObj->bonus_set = val;

	send_to_char( "Bonus ok.\n\r", ch);
	return TRUE;
}



OEDIT( oedit_create )
{
	OBJ_INDEX_DATA *pObj;
	AREA_DATA *pArea;
	int  value;
	int  iHash;

	value = atoi( argument );
	if ( argument[0] == '\0' || value == 0 )
	{
		send_to_char( "Sk³adnia: oedit create [vnum]\n\r", ch );
		return FALSE;
	}

	pArea = get_vnum_area( value );
	if ( !pArea )
	{
		send_to_char( "OEdit:  That vnum is not assigned an area.\n\r", ch );
		return FALSE;
	}

	if ( !IS_BUILDER( ch, pArea ) )
	{
		send_to_char( "OEdit: Vnum nie nale¿y do krainy. Brak security.\n\r", ch );
		return FALSE;
	}

	if ( get_obj_index( value ) )
	{
		send_to_char( "OEdit: Przedmiot o takim vnumie ju¿ istnieje.\n\r", ch );
		return FALSE;
	}

    if ( !( ch->pcdata->wiz_conf & W6 ) )
    {
        send_to_char( "Tworzenie nowych przedmiotow zostalo zablokowane.\n\r", ch );
        send_to_char( "Zglos sie do Lorda jezeli naprawde potrzebujesz nowego.\n\r", ch );
        send_to_char( "http://forum.mud.pl/viewtopic.php?p=220416#220416\n\r", ch );
        return FALSE;
    }

	pObj			= new_obj_index();
	pObj->vnum			= value;
	pObj->area			= pArea;
    /**
     * repair limits
     */
    pObj->repair_limit     = 20;
    pObj->repair_counter   = 0;
    pObj->repair_penalty   = 1;
    pObj->repair_condition = 100;

	if ( value > top_vnum_obj )
		top_vnum_obj = value;

	iHash			= value % MAX_KEY_HASH;
	pObj->next			= obj_index_hash[iHash];
	obj_index_hash[iHash]	= pObj;
	ch->desc->pEdit		= (void *)pObj;

	send_to_char( "Object Created.\n\r", ch );
	return TRUE;
}



OEDIT( oedit_ed )
{
	OBJ_INDEX_DATA *pObj;
	EXTRA_DESCR_DATA *ed;
	char command[MAX_INPUT_LENGTH];
	char keyword[MAX_INPUT_LENGTH];

	EDIT_OBJ(ch, pObj);

	argument = one_argument( argument, command );
	one_argument( argument, keyword );

	if ( command[0] == '\0' )
	{
		send_to_char( "Sk³adnia: ed add [keyword]\n\r", ch );
		send_to_char( "         ed delete [keyword]\n\r", ch );
		send_to_char( "         ed edit [keyword]\n\r", ch );
		send_to_char( "         ed format [keyword]\n\r", ch );
		return FALSE;
	}

	if ( !str_cmp( command, "add" ) )
	{
		if ( keyword[0] == '\0' )
		{
			send_to_char( "Sk³adnia: ed add [keyword]\n\r", ch );
			return FALSE;
		}

		ed                  =   new_extra_descr();
		ed->keyword         =   str_dup( keyword );
		ed->next            =   pObj->extra_descr;
		pObj->extra_descr   =   ed;

		string_append( ch, &ed->description );

		return TRUE;
	}

	if ( !str_cmp( command, "edit" ) )
	{
		if ( keyword[0] == '\0' )
		{
			send_to_char( "Sk³adnia: ed edit [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pObj->extra_descr; ed; ed = ed->next )
		{
			if ( is_name( keyword, ed->keyword ) )
				break;
		}

		if ( !ed )
		{
			send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
			return FALSE;
		}

		string_append( ch, &ed->description );

		return TRUE;
	}

	if ( !str_cmp( command, "delete" ) )
	{
		EXTRA_DESCR_DATA *ped = NULL;

		if ( keyword[0] == '\0' )
		{
			send_to_char( "Sk³adnia: ed delete [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pObj->extra_descr; ed; ed = ed->next )
		{
			if ( is_name( keyword, ed->keyword ) )
				break;
			ped = ed;
		}

		if ( !ed )
		{
			send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
			return FALSE;
		}

		if ( !ped )
			pObj->extra_descr = ed->next;
		else
			ped->next = ed->next;

		free_extra_descr( ed );

		send_to_char( "Extra description deleted.\n\r", ch );
		return TRUE;
	}


	if ( !str_cmp( command, "format" ) )
	{
		EXTRA_DESCR_DATA *ped = NULL;

		if ( keyword[0] == '\0' )
		{
			send_to_char( "Sk³adnia: ed format [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pObj->extra_descr; ed; ed = ed->next )
		{
			if ( is_name( keyword, ed->keyword ) )
				break;
			ped = ed;
		}

		if ( !ed )
		{
			send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
			return FALSE;
		}

		ed->description = format_string( ed->description );

		send_to_char( "Extra description formatted.\n\r", ch );
		return TRUE;
	}

	oedit_ed( ch, "" );
	return FALSE;
}

/* ROM object functions : */

OEDIT( oedit_extra)
{
	OBJ_INDEX_DATA * pObj;
	BITVECT_DATA * value;

	if ( argument[ 0 ] != '\0' )
	{
		EDIT_OBJ( ch, pObj );
		value = ext_flag_lookup( argument, extra_flags );

		if ( value != &EXT_NONE )
		{
			EXT_TOGGLE_BIT( pObj->extra_flags, *value );

			send_to_char( "Extra flag toggled.\n\r", ch );
			return TRUE;
		}
	}

	send_to_char( "Sk³adnia: extra [flag]\n\rType '? extra' for a list of flags.\n\r", ch );
	return FALSE;
}

OEDIT( oedit_exwear)
{
	OBJ_INDEX_DATA * pObj;
	BITVECT_DATA * value;

	if ( argument[ 0 ] != '\0' )
	{
		EDIT_OBJ( ch, pObj );
		value = ext_flag_lookup( argument, wear_flags2 );

		if ( value != &EXT_NONE )
		{
			EXT_TOGGLE_BIT( pObj->wear_flags2, *value );

			send_to_char( "EXWear flag toggled.\n\r", ch );
			return TRUE;
		}
	}

	send_to_char( "Sk³adnia: exwear [flag]\n\r{YWpisz'? exwear' ¿eby zobaczyæ listê dostêpnych flag.{x\n\r", ch );
	return FALSE;
}

OEDIT( oedit_wear )
{
	OBJ_INDEX_DATA * pObj;
	int value;

	if ( argument[ 0 ] != '\0' )
	{
		EDIT_OBJ( ch, pObj );

		if ( ( value = flag_value( wear_flags, argument ) ) != NO_FLAG )
		{
			TOGGLE_BIT( pObj->wear_flags, value );

			send_to_char( "Wear flag toggled.\n\r", ch );
			return TRUE;
		}
	}

	send_to_char( "Sk³adnia: wear [flag]\n\rType '? wear' for a list of flags.\n\r", ch );
	return FALSE;
}

OEDIT( oedit_type )
{
	OBJ_INDEX_DATA *pObj;
	int value;

	if ( argument[0] != '\0' )
	{
		EDIT_OBJ(ch, pObj);

		if ( ( value = flag_value( type_flags, argument ) ) != NO_FLAG )
		{

			switch( value )
			{
				case ITEM_PILL:
				case ITEM_CORPSE_PC:
				case ITEM_SCROLL:
				case ITEM_WAND:
				case ITEM_POTION:
				case ITEM_SPELLBOOK:
				case ITEM_SPELLITEM:
					if( !ch->pcdata || ch->pcdata->security < 9 )
					{
						send_to_char("Na te typy obiektow nalozono ograniecznia, wymagane security 9.\n\r", ch);
						return FALSE;
					}
				default:	break;

			}

			pObj->item_type = value;

			send_to_char( "Type set.\n\r", ch);

			/*
			 * Clear the values.
			 */
			pObj->value[0] = 0;
			pObj->value[1] = 0;
			pObj->value[2] = 0;
			pObj->value[3] = 0;
			pObj->value[4] = 0;     /* ROM */

			switch( value )
			{
				case ITEM_FURNITURE:
					pObj->value[3] = 100;
					pObj->value[4] = 100;
					break;
				case ITEM_CONTAINER:
					pObj->value[4] = 100;
					pObj->value[6] = -1;
					break;
			}

			while ( pObj->spec_dam )
			   {
			      SPEC_DAMAGE * sd = pObj->spec_dam;
			      pObj->spec_dam = pObj->spec_dam->next;
			      free_spec_damage( sd );
			   }

			return TRUE;
		}
	}

	send_to_char( "Sk³adnia: type [flag]\n\r"
			"Type '? type' for a list of flags.\n\r", ch );
	return FALSE;
}

OEDIT( oedit_material )
{
	OBJ_INDEX_DATA *pObj;
	char buf[MAX_INPUT_LENGTH];
	sh_int x;

	EDIT_OBJ(ch, pObj);


	if ( argument[0] == '\0' || !material_lookup(argument))
	{
		send_to_char( "Sk³adnia: material [string]\n\r", ch );
		send_to_char("NAME            HRD FIRE ACID ELECTR COLD OTHER       SPECIAL\n\r", ch);

		for(x=1;material_table[x].name;x++)
		{
            if (ch->pcdata->wiz_conf & W5)
            {
                sprintf
                    (
                     buf,
                     "%-15.15s %3d  %3d  %3d    %3d  %3d   %3d      ",
                     material_table[x].name,
                     material_table[x].hardness,
                     material_table[x].fire_res,
                     material_table[x].acid_res,
                     material_table[x].shock_res,
                     material_table[x].cold_res,
                     material_table[x].other_res
                    );
            }
            else
            {
                sprintf( buf, "%-15.15s", material_table[x].name );
            }

			if( IS_SET(material_table[x].flag, MAT_METAL) )
				strcat(buf, "[metal] ");

			if( IS_SET(material_table[x].flag, MAT_EASYBREAK) )
				strcat(buf, "[latwo peka] ");

			strcat(buf, "\n\r");
			send_to_char( buf, ch );
		}
		return FALSE;
	}

	pObj->material = material_lookup(argument);

	send_to_char( "Material set.\n\r", ch);
	return TRUE;
}

OEDIT( oedit_level )
{
	OBJ_INDEX_DATA *pObj;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: level [number]\n\r", ch );
		return FALSE;
	}

	pObj->level = atoi( argument );

	send_to_char( "Level set.\n\r", ch);
	return TRUE;
}

OEDIT( oedit_liczba_mnoga )
{
	OBJ_INDEX_DATA *pObj;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' || ( str_prefix( argument, "pojedyñcza" ) && str_prefix( argument, "mnoga" ) ) )
	{
		send_to_char( "Sk³adnia: liczba [pojedyñcza/mnoga]\n\r", ch );
		return FALSE;
	}

	if( !str_prefix( argument, "pojedyñcza" ) )
		pObj->liczba_mnoga = 0;
	else if ( !str_prefix( argument, "mnoga" ) )
		pObj->liczba_mnoga = 1;
	else
	{
		send_to_char( "Sk³adnia: liczba [pojedyñcza/mnoga]\n\r", ch );
		return FALSE;
	}

	print_char( ch, "Liczba %s setnieta.\n\r", pObj->liczba_mnoga == 0 ? "pojedyñcza" : "mnoga" );
	return TRUE;
}

OEDIT( oedit_gender )
{
	OBJ_INDEX_DATA *pObj;
	int gender;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' || ( gender = gender_lookup( argument ) ) < 0 )
	{
		send_to_char( "Sk³adnia: rodzaj [nijaki/mêski/¿eñski] - dla liczby pojedynczej\n\r"
		              "          rodzaj [mêskoosobowy/¿eñskoosobowy] - dla liczby mnogiej\n\r", ch );
		return FALSE;
	}

	pObj->gender = gender;

	print_char( ch, "Rodzaj %s ustawiony.\n\r", gender_table[ gender ].name );
	return TRUE;
}

//rellik: komponenty, definiowanie w³a¶ciwo¶ci komponentu
OEDIT( oedit_item_spell )
{
	OBJ_INDEX_DATA *pObj;
	char arg_counter[ MAX_STRING_LENGTH ];
	char arg_timer[ MAX_STRING_LENGTH ];
	int timer;

	save_debug_info( "olc_act.c =>> oedit_item_spell", NULL, NULL, DEBUG_PROJECT_SPELL_ITEMS, DEBUG_LEVEL_ALL, TRUE );

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '?' || argument[0] == '\0')
	{
		send_to_char( "Ustawianie przedmiotu jako komponentu do czarów.\n\r", ch );
		send_to_char( "Sk³adnia:\n\r", ch );
		send_to_char( "komponent [counter] [timer]\n\r", ch );
		send_to_char( "Podanie 'komponent' bez parametrów w³±cza i wy³±cza t± w³a¶ciwo¶æ przedmiotu.\n\r", ch );
		send_to_char( "counter - ilo¶æ u¿yæ, warto¶æ ujemna oznacza nielimitowan± ilo¶æ u¿yæ.\n\r", ch );
		send_to_char( "timer - czas (w godzinach mudowych) po jakim przedmiot przestanie byæ komponentem.\n\r", ch );
		send_to_char( "Je¶li nie podano parametrów counter bêdzie ustawiony na 1, a timer na 525600.\n\r\n\r", ch );
		if ( argument[0] == '?' ) return FALSE;
	}

	if ( argument[0] == '\0' )
	{
		if ( pObj->is_spell_item == TRUE )
		{
			pObj->is_spell_item = FALSE;
			send_to_char( "U¿yteczno¶æ tego przedmiotu jako komponentu zosta³a wy³±czona.\n\r", ch );
		} else {
			pObj->is_spell_item = TRUE;
			pObj->spell_item_counter = 1;
			pObj->spell_item_timer = 525600;
			send_to_char( "U¿yteczno¶æ tego przedmiotu jako komponentu zosta³a w³±czona z domy¶lnymi warto¶ciami.\n\r", ch );
		}
		return TRUE;
	}

	argument = one_argument( argument, arg_counter );
	argument = one_argument( argument, arg_timer );

	if ( arg_counter[0] == '\0' || !is_number( arg_counter ) )
	{
		send_to_char( "Parametr musi byæ liczb±, wpisz 'komponent ?' aby zobaczyæ pomoc.\n\r", ch );
		return FALSE;
	}

	pObj->is_spell_item = TRUE;
	pObj->spell_item_counter = atoi( arg_counter );

	print_char( ch, "Obiekt jest teraz komponentem z liczb± u¿yæ %d ", pObj->spell_item_counter );

	if ( arg_timer[0] != '\0' && is_number( arg_timer ) )
	{
		timer = atoi( arg_timer );
		if ( timer <= 0 )
		{
			send_to_char( "\n\rtimer powinien byc ustawiony na warto¶æ wiêksz± od zera, pozostawiam dotychczasow± warto¶æ.\n\r", ch );
		  return TRUE;
		}
		pObj->spell_item_timer = timer;
		print_char( ch, "i czasem trwania %d.\n\r", pObj->spell_item_timer );
	}
	send_to_char( "\n\r", ch );
	return TRUE;
}

OEDIT( oedit_clone )
{
	OBJ_INDEX_DATA *pObj, *pOObj;
	int value, i;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' || !is_number( argument ) ||
			(value = atoi( argument )) < 0 || !( pOObj = get_obj_index( value ) ))
	{
		send_to_char( "Sk³adnia: clone [vnum of cloning obj]\n\r", ch );
		return FALSE;
	}

	if ( !IS_BUILDER( ch, pOObj->area ) )
	{
		send_to_char("Masz niewystarczaj±ce security do klonowania tego obiektu.\n\r",ch);
		return FALSE;
	}

	free_string(pObj->name);
	pObj->name = str_dup(pOObj->name);

	free_string(pObj->name2);
	pObj->name2 = str_dup(pOObj->name2);

	free_string(pObj->name3);
	pObj->name3 = str_dup(pOObj->name3);

	free_string(pObj->name4);
	pObj->name4 = str_dup(pOObj->name4);

	free_string(pObj->name5);
	pObj->name5 = str_dup(pOObj->name5);

	free_string(pObj->name6);
	pObj->name6 = str_dup(pOObj->name6);

	free_string(pObj->short_descr);
	pObj->short_descr = str_dup(pOObj->short_descr);

	free_string(pObj->description);
	pObj->description = str_dup(pOObj->description);

	free_string(pObj->item_description);
	pObj->item_description = str_dup(pOObj->item_description);

	free_string(pObj->ident_description);
	pObj->ident_description = str_dup(pOObj->ident_description);

	free_string(pObj->hidden_description);
	pObj->hidden_description = str_dup(pOObj->hidden_description);

	pObj->material = pOObj->material;
	pObj->item_type = pOObj->item_type;
	ext_flags_copy( pOObj->extra_flags, pObj->extra_flags );
	pObj->wear_flags = pOObj->wear_flags;
	ext_flags_copy( pOObj->wear_flags2, pObj->wear_flags2 );
	pObj->level = pOObj->level;
	pObj->condition = pOObj->condition;
	pObj->weight = pOObj->weight;
	pObj->cost = pOObj->cost;
	pObj->rent_cost = pOObj->rent_cost;
	pObj->gender = pOObj->gender;
	pObj->liczba_mnoga = pOObj->liczba_mnoga;

	for(i=0;i<7;i++)
		pObj->value[i] = pOObj->value[i];

	send_to_char("Ok.\n\r", ch);
	return TRUE;
}

OEDIT( oedit_condition )
{
	OBJ_INDEX_DATA *pObj;
	int value;

	if ( argument[0] != '\0'
			&& ( value = atoi (argument ) ) >= 0
			&& ( value <= 100 ) )
	{
		EDIT_OBJ( ch, pObj );

		pObj->condition = value;
		send_to_char( "Condition set.\n\r", ch );

		return TRUE;
	}

	send_to_char(
			"Sk³adnia: condition [number]\n\r"
			"Wybierz odpowiedni± warto¶æ miêdzy 0 (przedmiot zniszony) do 100\n\r"
			"(doskona³y)\n\r"
			"{RUwaga:{x nie zaleca siê stosowania wiêcej ni¿ 85.\n\r",
			ch
		    );
	return FALSE;
}

OEDIT ( oedit_deloprog )
{
	OBJ_INDEX_DATA * pObj;
	PROG_LIST *prog = NULL, *prog_tmp = NULL, *prog_prev = NULL;
	char oprog[ MAX_STRING_LENGTH ];
	int value;
	int cnt = 0;
	int to_rem = 0;

	EDIT_OBJ( ch, pObj );

	one_argument( argument, oprog );
	if ( !is_number( oprog ) || oprog[ 0 ] == '\0' )
	{
		send_to_char( "Sk³adnia: delprog [#oprog]\n\r", ch );
		return FALSE;
	}

	value = atoi ( oprog );

	if ( value < 0 )
	{
		send_to_char( "Only non-negative oprog-numbers allowed.\n\r", ch );
		return FALSE;
	}

	if ( !pObj->progs )
	{
		send_to_char( "OEdit:  None exist oprog.\n\r", ch );
		return FALSE;
	}

	if ( value == 0 )
	{
		for ( prog = pObj->progs; prog; prog = prog->next )
		{
			if ( prog->trig_type == pObj->progs->trig_type )
				to_rem++;
		}

		if ( to_rem <= 1 )
			EXT_REMOVE_BIT( pObj->prog_flags, *pObj->progs->trig_type );

		prog = pObj->progs;
		pObj->progs = prog->next;
		free_prog( prog );
	}
	else
	{
		for( prog = pObj->progs; cnt < value && prog; prog = prog->next, cnt++ )
			prog_prev = prog;

		if ( prog )
		{
			for ( prog_tmp = pObj->progs; prog_tmp; prog_tmp = prog_tmp->next )
				if ( prog_tmp->trig_type == prog->trig_type )
					to_rem++;

			if ( to_rem <= 1 )
				EXT_REMOVE_BIT( pObj->prog_flags, *prog->trig_type );

			prog_prev->next = prog->next;
			free_prog( prog );
		}
		else
		{
			send_to_char( "No such oprog.\n\r", ch );
			return FALSE;
		}
	}

	send_to_char( "Oprog removed.\n\r", ch );
	return TRUE;
}

OEDIT ( oedit_addoprog )
{
	BITVECT_DATA * value;
	OBJ_INDEX_DATA *pObj;
	PROG_LIST *list;
	PROG_CODE *code;
	char trigger[MAX_STRING_LENGTH];
	char phrase[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];

	EDIT_OBJ(ch, pObj);
	argument=one_argument(argument, name);
	argument=one_argument(argument, trigger);
	argument=one_argument(argument, phrase);

	if ( name[0] == '\0' || trigger[0] =='\0' || phrase[0] =='\0' )
	{
		send_to_char("Sk³adnia:  addprog [vnum] [trigger] [phrase]\n\r",ch);
		return FALSE;
	}

	value = ext_flag_lookup( trigger, prog_flags );
	if ( value == &EXT_NONE )
	{
		send_to_char("Valid flags are:\n\r",ch);
		show_help( ch, "trigger");
		return FALSE;
	}

	if ( !check_trigger( 1, trigger ) )
	{
		send_to_char( "Valid flags are:\n\r", ch );
		show_help( ch, "trigger" );
		return FALSE;
	}


	if ( ( code = get_oprog_index ( name ) ) == NULL)
	{
		send_to_char("No such OBJProgram.\n\r",ch);
		return FALSE;
	}

	list                  = new_prog();
	list->name            = code->name;
	list->trig_type       = value;
	list->trig_phrase     = str_dup(phrase);
	list->code            = code->code;
	list->next            = pObj->progs;
	pObj->progs          = list;
	EXT_SET_BIT( pObj->prog_flags, *value );

	send_to_char( "Oprog Added.\n\r",ch);
	return TRUE;
}

OEDIT( oedit_repair )
{
    OBJ_INDEX_DATA *pObj;
	char command[MAX_INPUT_LENGTH], arg[MAX_INPUT_LENGTH];
    bool show_help = FALSE;

    argument = one_argument( argument, command );
    argument = one_argument( argument, arg );
    
    EDIT_OBJ(ch, pObj);
    if ( command[0] == '\0' || arg[0] == '\0' || !is_number( arg ) )
    {
        show_help = TRUE;
    }
    else if ( !str_cmp( command, "limit" ) )
    {
        pObj->repair_limit = UMAX( 0, atoi( arg ) );
        send_to_char( "Ustawiono maksymalna liczbe napraw.\n\r", ch);
    }
    else if ( !str_cmp( command, "counter" ) )
    {
        pObj->repair_counter = UMAX( 0, atoi( arg ) );
        send_to_char( "Przestawiono liczbe napraw.\n\r", ch);
    }
    else if ( !str_cmp( command, "penalty" ) )
    {
        pObj->repair_penalty = UMAX( 0, atoi( arg ) );
        send_to_char( "Ustawiono utrate maksymalnej naprawy.\n\r", ch);
    }
    else if ( !str_cmp( command, "condition" ) )
    {
        pObj->repair_condition = URANGE( 0, atoi( arg ), 100 );
        send_to_char( "Ustawiono aktualny limit naprawy.\n\r", ch);
    }
    if ( show_help )
    {
        send_to_char( "Sk³adnia: repair [limit/counter/penalty/condition] [liczba]\n\r\n\r", ch );
        send_to_char("limit     - maksymalna liczba napraw, 0 - brak limitu\n\r", ch);
        send_to_char("counter   - aktualna liczba napraw, raczej ustawiac zero\n\r", ch);
        send_to_char("penalty   - wielkosc utraty gornego limitu przy naprawie\n\r", ch);
        send_to_char("condition - aktualne maksimum do jakiego mozna naprawic przedmiot,\n\r", ch);
        send_to_char("            raczej ustawiac 100\n\r", ch);
        return FALSE;
    }
    return TRUE;
}


MEDIT( oedit_addcomment )
{
	char buf1[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char time[MAX_STRING_LENGTH];
	OBJ_INDEX_DATA *pObj;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Nie poda<&³e¶/³a¶/³e¶> komentarza.\n\r", ch );
		return FALSE;
	}

	buf1[0] = '\0';

	strcpy( time,  (char *) ctime( &current_time ) );
	time[ strlen( time ) - 1 ] = '\0';

	if ( !IS_NULLSTR( pObj->comments ) )
		strcpy( buf1, pObj->comments );
	smash_tilde( argument );
	sprintf( buf2, " - {R%s{x ({C%s{x)", capitalize(ch->name), time );
	strcat( buf1, argument );
	strcat( buf1, buf2 );
	strcat( buf1, "\n\r");

	free_string( pObj->comments);
	pObj->comments = str_dup( buf1 );

	send_to_char( "Komentarz dodany.\n\r", ch );

	return TRUE;
}

MEDIT( oedit_delcomment )
{
	OBJ_INDEX_DATA *pObj;

	EDIT_OBJ(ch, pObj);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Podaj numer komentarza do wykasowania.\n\r", ch );
		return FALSE;
	}

	if ( !IS_NULLSTR( pObj->comments ) )
		pObj->comments = string_linedel( pObj->comments, atoi(argument) );

	send_to_char( "Komentarz usuniêty.\n\r", ch );

	return TRUE;
}

OEDIT( oedit_specdam )
{
	OBJ_INDEX_DATA *pObj;
	SPEC_DAMAGE    *specdam, *prev;
	char arg1 [MAX_INPUT_LENGTH];
	int i, val;
	BITVECT_DATA * ext_val;

	EDIT_OBJ(ch, pObj);

	if ( pObj->item_type != ITEM_WEAPON )
	{
		send_to_char( "Specjalne obra¿enia mog± byæ dodawane tylko dla obiektów typu 'weapon'.\n\r", ch );
		return FALSE;
	}

	specdam = new_spec_damage();

	argument = one_argument( argument, arg1 );

	if ( arg1[0] != '\0' )
	{
		if ( !str_prefix( arg1, "delete" ) )
		{
			free_spec_damage( specdam );

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Nieprawid³owa liczba.\n\r", ch );
				return FALSE;
			}

			if ( !pObj->spec_dam )
			{
				send_to_char( "Lista specjalnych obra¿eñ jest pusta.\n\r", ch );
				return FALSE;
			}

			val = atoi( arg1 );
			i = 1;
			prev = NULL;
			for ( specdam = pObj->spec_dam; specdam; specdam = specdam->next )
			{
				if ( i == val )
				{
					if ( !prev )
						pObj->spec_dam = specdam->next;
					else
					{
						prev->next = specdam->next;
						free_spec_damage( specdam );
					}
					send_to_char( "Specjalne obra¿enie usuniête.\n\r", ch );
					return TRUE;
				}
				i++;
				prev = specdam;
			}

			send_to_char( "Nie ma takiego numeru na li¶cie.\n\r", ch );
			return FALSE;
		} else if ( !str_prefix( arg1, "magic" ) )
		{
			specdam->type = SD_TYPE_MAGIC;

			argument = one_argument( argument, arg1 );
			val = -1;
			if ( !str_cmp( arg1, "any" ) )
				val = SD_PARAM_ANY;
			else
				if ( arg1[0] != '\0' )
				{
					for ( i = 0; dam_types[i].name != NULL; i++ )
					{
						if ( !str_cmp( arg1, dam_types[i].name ) )
						{
							val = i;
							break;
						}
					}
				}
			if ( val == -1 )
			{
				send_to_char( "Z³y parametr rodzaju ataku. Prawid³owe to:\n\r"
						"bash     pierce slash  fire   cold    lightning acid  poison.\n\r"
						"negative holy   energy mental disease drowning  light other\n\r"
						"harm     charm  sound\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->param1 = val;

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Bonus musi byæ liczb±.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->bonus = atoi( arg1 );

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Szansa musi byæ liczb± z zakresu 0 - 100.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->chance = URANGE( 0, atoi( arg1 ), 100 );

			specdam->next  = pObj->spec_dam;
			pObj->spec_dam = specdam;
			send_to_char( "Specjalne obra¿enie dodane.\n\r", ch );
			return TRUE;
		}
		else if ( !str_prefix( arg1, "align" ) )
		{
			specdam->type = SD_TYPE_ALIGN;

			argument = one_argument( argument, arg1 );
			if ( !str_cmp( arg1, "any" ) )
				specdam->param1 = SD_PARAM_ANY;
			else if ( !str_cmp( arg1, "evil" ) )
				specdam->param1 = 0;
			else if ( !str_cmp( arg1, "good" ) )
				specdam->param1 = 1;
			else if ( !str_cmp( arg1, "neutral" ) )
				specdam->param1 = 2;
			else
			{
				send_to_char( "Z³y parametr align celu. Prawid³owe to: evil good neutral any.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}

			argument = one_argument( argument, arg1 );
			if ( !str_cmp( arg1, "any" ) )
				specdam->param2 = SD_PARAM_ANY;
			else if ( !str_cmp( arg1, "evil" ) )
				specdam->param2 = 0;
			else if ( !str_cmp( arg1, "good" ) )
				specdam->param2 = 1;
			else if ( !str_cmp( arg1, "neutral" ) )
				specdam->param2 = 2;
			else
			{
				send_to_char( "Z³y parametr align atakuj±cego. Prawid³owe to: evil good neutral any.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Bonus musi byæ liczb±.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->bonus = atoi( arg1 );

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Szansa musi byæ liczb± z zakresu 0 - 100.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->chance = URANGE( 0, atoi( arg1 ), 100 );

			specdam->next  = pObj->spec_dam;
			pObj->spec_dam = specdam;
			send_to_char( "Specjalne obra¿enie dodane.\n\r", ch );
			return TRUE;
		}
		else if ( !str_prefix( arg1, "race" ) )
		{
			specdam->type = SD_TYPE_RACE;
			argument = one_argument( argument, arg1 );

			val = -1;

			if ( !str_cmp( arg1, "any" ) )
				val = SD_PARAM_ANY;
			else if ( arg1[0] != '\0' )
				val = race_lookup( arg1 );

			if ( val == -1 || val == 0 )
			{
				send_to_char( "Z³y parametr rasy celu.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}

			specdam->param1 = val;

			argument = one_argument( argument, arg1 );
			val = -1;

			if ( !str_cmp( arg1, "any" ) )
				val = SD_PARAM_ANY;
			else if ( arg1[0] != '\0' )
				val = race_lookup( arg1 );

			if ( val == -1 || val == 0
			   )
			{
				send_to_char( "Z³y parametr rasy atakuj±cego.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}

			specdam->param2 = val;

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Bonus musi byæ liczb±.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->bonus = atoi( arg1 );

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Szansa musi byæ liczb± z zakresu 0 - 100.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->chance = URANGE( 0, atoi( arg1 ), 100 );

			specdam->next  = pObj->spec_dam;
			pObj->spec_dam = specdam;
			send_to_char( "Specjalne obra¿enie dodane.\n\r", ch );
			return TRUE;
		}
		else if ( !str_prefix( arg1, "class" ) )
		{
			specdam->type = SD_TYPE_CLASS;
			argument = one_argument( argument, arg1 );

			val = -1;

			if ( arg1[0] != '\0' )
				val = class_lookup( arg1 );

			if ( val == -1 || val == 0 )
			{
				send_to_char( "Z³y parametr profesji atakuj±cego.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}

			specdam->param1 = val;

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Bonus musi byæ liczb±.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->bonus = atoi( arg1 );

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Szansa musi byæ liczb± z zakresu 0 - 100.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->chance = URANGE( 0, atoi( arg1 ), 100 );

			specdam->next  = pObj->spec_dam;
			pObj->spec_dam = specdam;
			send_to_char( "Specjalne obra¿enie dodane.\n\r", ch );
			return TRUE;
		}
		else if ( !str_prefix( arg1, "act" ) )
		{
			specdam->type = SD_TYPE_ACT;

			val = 0;

			for(;; )
			{
				argument = one_argument( argument, arg1 );
				if ( arg1[0] == '\0' || is_number( arg1 ) )
					break;

				ext_val = ext_flag_lookup( arg1, act_flags );

				if ( ext_val == &EXT_NONE )
				{
					continue;
				}
				else
					EXT_TOGGLE_BIT( specdam->ext_param, *ext_val );

			};

			if ( ext_flags_none( specdam->ext_param ) )
			{
				send_to_char( "Nie podano flag act.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}

			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Bonus musi byæ liczb±.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->bonus = atoi( arg1 );

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Szansa musi byæ liczb± z zakresu 0 - 100.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->chance = URANGE( 0, atoi( arg1 ), 100 );

			specdam->next  = pObj->spec_dam;
			pObj->spec_dam = specdam;
			send_to_char( "Specjalne obra¿enie dodane.\n\r", ch );
			return TRUE;
		}
		else if ( !str_prefix( arg1, "size" ) )
		{
			specdam->type = SD_TYPE_SIZE;

			argument = one_argument( argument, arg1 );
			val = -1;
			if ( !str_cmp( arg1, "any" ) )
				val = SD_PARAM_ANY;
			else
				if ( arg1[0] != '\0' )
				{
					for ( i = 0; i < 6; i++ )
					{
						if ( !str_cmp( arg1, size_table[i].name ) )
						{
							val = i;
							break;
						}
					}
				}
			if ( val == -1 )
			{
				send_to_char( "Z³y parametr rozmiaru celu. Prawid³owe to: any tiny small medium large huge giant.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->param1 = val;

			argument = one_argument( argument, arg1 );
			val = -1;
			if ( !str_cmp( arg1, "any" ) )
				val = SD_PARAM_ANY;
			else
				if ( arg1[0] != '\0' )
				{
					for ( i = 0; i < 6; i++ )
					{
						if ( !str_cmp( arg1, size_table[i].name ) )
						{
							val = i;
							break;
						}
					}
				}
			if ( val == -1 )
			{
				send_to_char( "Z³y parametr rozmiaru atakuj±cego. Prawid³owe to: any tiny small medium large huge giant.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->param2 = val;

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Bonus musi byæ liczb±.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->bonus = atoi( arg1 );

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Szansa musi byæ liczb± z zakresu 0 - 100.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->chance = URANGE( 0, atoi( arg1 ), 100 );

			specdam->next  = pObj->spec_dam;
			pObj->spec_dam = specdam;
			send_to_char( "Specjalne obra¿enie dodane.\n\r", ch );
			return TRUE;
		}
		else if ( !str_prefix( arg1, "selfstat" ) || !str_prefix( arg1, "targetstat" ) )
		{
			if ( !str_prefix( arg1, "selfstat" ) )
				specdam->type = SD_TYPE_SELFSTAT;
			else
				specdam->type = SD_TYPE_TARGETSTAT;

			argument = one_argument( argument, arg1 );
			val = -1;
			if ( arg1[0] != '\0' )
			{
				for ( i = 0; i < MAX_STATS; i++ )
				{
					if ( !str_cmp( arg1, stat_table[i].name ) )
					{
						val = i;
						break;
					}
				}
			}
			if ( val == -1 )
			{
				send_to_char( "Z³a nazwa statystyki. Prawid³owe to: str int wis dex con cha luc.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->param1 = val;

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || ( val = keyword_lookup( fn_evals, arg1 ) ) < 0 )
			{
				send_to_char( "Nieprawid³owy parametr porównania. Prawid³owe to: == >= <= > < !=\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->param2 = val;

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Parametr statystyki musi byæ liczb±.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->param3 = atoi( arg1 );

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Bonus musi byæ liczb±.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->bonus = atoi( arg1 );

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Szansa musi byæ liczb± z zakresu 0 - 100.\n\r", ch );
				free_spec_damage( specdam );
				return FALSE;
			}
			specdam->chance = URANGE( 0, atoi( arg1 ), 100 );

			specdam->next  = pObj->spec_dam;
			pObj->spec_dam = specdam;
			send_to_char( "Specjalne obra¿enie dodane.\n\r", ch );
			return TRUE;
		}
		else if ( !str_prefix( arg1, "targettype" ) )
		{
			free_spec_damage( specdam );

			argument = one_argument( argument, arg1 );
			if ( arg1[0] == '\0' || !is_number( arg1 ) )
			{
				send_to_char( "Nieprawid³owa liczba.\n\r", ch );
				return FALSE;
			}

			if ( !pObj->spec_dam )
			{
				send_to_char( "Lista specjalnych obra¿eñ jest pusta.\n\r", ch );
				return FALSE;
			}

			val = atoi( arg1 );
			i = 1;
			prev = NULL;
			for ( specdam = pObj->spec_dam; specdam; specdam = specdam->next )
			{
				if ( i == val )
				{
					specdam->target_type = specdam->target_type ? SD_TARGET_DAM : SD_TARGET_MAG;
					send_to_char( "Zmieniony typ celu specjalnego obra¿enia.\n\r", ch );
					return TRUE;
				}
				i++;
				prev = specdam;
			}

			send_to_char( "Nie ma takiego numeru na li¶cie.\n\r", ch );
			return FALSE;

		}

	}

	send_to_char( "Sk³adnia:\n\r"
			"specdam magic <typ obra¿eñ> <bonus> <szansa>\n\r"
			"specdam race  <rasa celu|any> <rasa atakuj±cego|any> <bonus> <szansa>\n\r"
			"specdam class <profesja atakuj±cego> <bonus> <szansa>\n\r"
			"specdam align <align celu|any> <align atakuj±cego|any> <bonus> <szansa>\n\r"
			"specdam act   <flagi act celu> <bonus> <szansa>\n\r"
			"specdam size  <rozmiar celu|any> <rozmiar atakuj±cego|any> <bonus> <szansa>\n\r"
			"specdam selfstat   <str|dex|...> <operator> <warto¶æ min> <bonus> <szansa>\n\r"
			"specdam targetstat <str|dex|...> <operator> <warto¶æ max> <bonus> <szansa>\n\r"
		        "specdam targettype <numer>\n\r"
			"specdam delete <numer>\n\r"
			"Align: evil, neutral, good\n\r"
			"Rozmiary: tiny, small, medium, large, huge, giant\n\r", ch );
	free_spec_damage( specdam );
	return FALSE;
}

MEDIT( medit_show )
{
	MOB_INDEX_DATA * pMob;
	char buf[ MAX_STRING_LENGTH ];
	PROG_LIST *list;
	BUFFER *buffer;
	int i;

	EDIT_MOB( ch, pMob );

	buffer = new_buf();

	sprintf( buf, "{GArea{x:          [%5d] %s\n\r", !pMob->area ? -1 : pMob->area->vnum, !pMob->area ? "Brak krainy" : pMob->area->name );
	add_buf( buffer, buf );

	sprintf( buf, "{GName{x:          [%s]\n\r", pMob->player_name);
	add_buf( buffer, buf );

	sprintf( buf, "{GAct{x:           [%s]\n\r", ext_flag_string( act_flags, pMob->act ) );
	add_buf( buffer, buf );

	sprintf( buf, "{GVnum{x:          [%5d] {GPlec{x:   [%s]   {GRasa{x: [%s]  {GGrupa{x: [%d]\n\r",
			pMob->vnum,
			pMob->sex == SEX_MALE ? "samiec " :
			pMob->sex == SEX_FEMALE ? "samica " :
			pMob->sex == 3 ? "losowo " : "neutral",
			race_table[ pMob->race ].name, pMob->group );
	add_buf( buffer, buf );

	sprintf( buf, "{GPoziom{x:        [%2d]    {GAlign{x: [%4d]      {GHitroll{x: [%2d]\n\r",
			pMob->level, pMob->alignment,
			pMob->hitroll);
	add_buf( buffer, buf );

	sprintf( buf, "{GPunkty ¿ycia{x:  [%2dd%-3d+%4d] [Min: %-3d (%3ld/lev) Max: %-3d (%3ld/lev)]\n\r",
			pMob->hit[ DICE_NUMBER ],
			pMob->hit[ DICE_TYPE ],
			pMob->hit[ DICE_BONUS ],
			pMob->hit[ DICE_NUMBER ] + pMob->hit[ DICE_BONUS ],
			( pMob->hit[ DICE_NUMBER ] + pMob->hit[ DICE_BONUS ] ) / UMAX( pMob->level, 1 ),
			pMob->hit[ DICE_NUMBER ] * pMob->hit[ DICE_TYPE ] + pMob->hit[ DICE_BONUS ],
			( pMob->hit[ DICE_NUMBER ] * pMob->hit[ DICE_TYPE ] + pMob->hit[ DICE_BONUS ] ) / UMAX( pMob->level, 1 ) );
	add_buf( buffer, buf );

	sprintf( buf, "{GObra¿enia{x:     [%2dd%-3d+%4d] (%2d-%-2d )   {Ydamdice{x\n\r",
			pMob->damage[ DICE_NUMBER ],
			pMob->damage[ DICE_TYPE ],
			pMob->damage[ DICE_BONUS ],
			pMob->damage[ DICE_NUMBER ] * 1 + pMob->damage[ DICE_BONUS ] + str_app[ pMob->stats[ 0 ] ].todam,		//jak bedzie nowa tabele to usunac /6
			pMob->damage[ DICE_NUMBER ] * pMob->damage[ DICE_TYPE ] + pMob->damage[ DICE_BONUS ] + str_app[ pMob->stats[ 0 ] ].todam		//jak bedzie nowa tabela to suunac /6
	       );
	add_buf( buffer, buf );

	sprintf( buf, "{GUmagicznienie{x: [%2d]   {Ydammagic{x\n\r",pMob->magical_damage);
	add_buf( buffer, buf );

	sprintf( buf, "{GBonus na broñ{x: [%2d]   {Ydamweapon{x\n\r", pMob->weapon_damage_bonus );
	add_buf( buffer, buf );

	sprintf( buf, "{GRodzaj obra¿eñ{x:[%s (%s)]   {Ydamtype{x\n\r", attack_table[ pMob->dam_type ].name, attack_table[ pMob->dam_type ].noun );
	add_buf( buffer, buf );

	sprintf( buf, "{GFlagi obra¿eñ{x: [%s]   {Ydamflags{x\n\r", flag_string( weapon_type2, pMob->attack_flags ) );
	add_buf( buffer, buf );

	sprintf( buf, "{GCechy{x:         [str: %d  int: %d  wis: %d  dex: %d  con: %d  cha: %d  luc: %d]\n\r",
			pMob->stats[ 0 ],
			pMob->stats[ 1 ],
			pMob->stats[ 2 ],
			pMob->stats[ 3 ],
			pMob->stats[ 4 ],
			pMob->stats[ 5 ],
			pMob->stats[ 6 ]
	       );
	add_buf( buffer, buf );


	sprintf( buf, "{GPancerz{x:       [k³uj±ce: %d  obuchowe: %d  tn±ce: %d  inne: %d]\n\r",
			pMob->ac[ AC_PIERCE ], pMob->ac[ AC_BASH ],
			pMob->ac[ AC_SLASH ], pMob->ac[ AC_EXOTIC ] );
	add_buf( buffer, buf );

	sprintf( buf, "{GForm{x:          [%s]   {Y? form{x\n\r", flag_string( form_flags, pMob->form ) );
	add_buf( buffer, buf );

	sprintf( buf, "{GParts{x:         [%s]   {Y? parts{x\n\r", flag_string( part_flags, pMob->parts ) );
	add_buf( buffer, buf );

	sprintf( buf, "{GAffected by{x:   [%s]   {Y? affect{x\n\r", ext_flag_string( affect_flags, pMob->affected_by ) );
	add_buf( buffer, buf );

	sprintf( buf, "{GOff{x:           [%s]      {Y? off{x\n\r", ext_flag_string( off_flags, pMob->off_flags ) );
	add_buf( buffer, buf );

	sprintf( buf, "{GWielko¶æ{x:      [%s]      {Y? size{x   ", flag_string( size_flags, pMob->size ) );
	add_buf( buffer, buf );

	sprintf( buf, "{GExp multiplier{x:      [%d]{x\n\r", pMob->exp_multiplier  );
	add_buf( buffer, buf );

	sprintf( buf, "{GMateria³{x:      [%s]   ", pMob->material );
	add_buf( buffer, buf );

	sprintf( buf, "{GWealth{x: [%4ld]\n\r", pMob->wealth );
	add_buf( buffer, buf );

	sprintf( buf, "{GStart pos{x:     [%s]    ",
			flag_string( position_flags, pMob->start_pos ) );
	add_buf( buffer, buf );

	sprintf( buf, "{GDefault pos{x:   [%s]   {Y? position{x\n\r",
			flag_string( position_flags, pMob->default_pos ) );
	add_buf( buffer, buf );

	/* ROM values end */

	if ( pMob->spec_fun )
	{
		sprintf( buf, "{GSpec fun{x:      [%s]   {Y? spec{x\n\r", spec_name( pMob->spec_fun ) );
	}
	else
	{
		sprintf( buf, "{GSpec fun{x:      [brak specjalnych zachowañ]   {Y? spec{x\n\r");
	}
	add_buf( buffer, buf );

	sprintf( buf, "{GLanguages{x:     [%s]   {Y? langs{x\n\r", lang_bit_name( pMob->languages ) );
	add_buf( buffer, buf );

	sprintf( buf, "{GSpeaking{x:      [%s]   {Y? langs{x\n\r", lang_table[ pMob->speaking ].name );
	add_buf( buffer, buf );

	if ( pMob->corpse_vnum > 0 )
		sprintf( buf, "{GCorpse vnum{x:   [%d]    ", pMob->corpse_vnum );
	else
		sprintf( buf, "{GCorpse vnum{x:   [%s]    ", pMob->corpse_vnum == 0 ? "domy¶lny" : "brak cia³a" );


	add_buf( buffer, buf );

	if ( pMob->skin_multiplier > 0 )
		sprintf( buf, "{GSkin multiplier{x: [%d%%]\n\r", pMob->skin_multiplier );
	else
		sprintf( buf, "{GSkin Multiplier{x: [%s%%]\n\r", pMob->skin_multiplier == 0 ? "0%" : "no skin" );
	add_buf( buffer, buf );

	//TODO Rysand: Dodaæ flagê dot. co mo¿e je¶æ mob (dla mountów) - na razie wszystko

	if ( pMob->spells[ 0 ] != 0 )
	{
		strcpy( buf, "{CSpells{x:       [ " );

		for ( i = 0; pMob->spells[ i ] > 0 && i < 16; i++ )
		{
			strcat( buf, " '" );
			strcat( buf, skill_table[ pMob->spells[ i ] ].name );
			strcat( buf, "'" );
		}
		strcat( buf, " ]\n\r" );

		add_buf( buffer, buf );
	}
	else
	{
		sprintf( buf, "{GSpells{x:        [brak czarów]\n\r");
		add_buf( buffer, buf );
	}

	sprintf( buf, "{GShort descr{x:   %s\n\r{GLong descr{x:\n\r%s\n\r",
			pMob->short_descr,
			pMob->long_descr );
	add_buf( buffer, buf );

	sprintf( buf, "{GDescription{x:\n\r%s", pMob->description );
	add_buf( buffer, buf );

	sprintf( buf,
			"{GDope³niacz{x:    [%-20.20s] ({Gkogo nie ma?{x)\n\r"
			"{GCelownik{x:      [%-20.20s] ({Gkomu siê przygl±dam?{x)\n\r"
			"{GBiernik{x:       [%-20.20s] ({Gkogo widzê?{x)\n\r"
			"{GNarzêdnik{x:     [%-20.20s] ({Gz kim idê?{x)\n\r"
			"{GMiejscownik{x:   [%-20.20s] ({Go kim mówiê?{x)\n\r",
			pMob->name2, pMob->name3, pMob->name4, pMob->name5, pMob->name6 );
	add_buf( buffer, buf );

	for ( i = 0; i < MAX_RESIST; i++ )
	{
		if ( pMob->resists[ i ] != 0 )
		{
			sprintf( buf, "{ROdporno¶æ na %-15.15s :%d%%{x\n\r", resist_name( i ), pMob->resists[ i ] );
			add_buf( buffer, buf );
		}
	}

	for ( i = 0; i < MAX_RESIST; i++ )
	{
		if ( pMob->healing_from[ i ] != 0 )
		{
			sprintf( buf, "{RLeczony przez obra¿enia od: %-15.15s - %d%%{x\n\r", resist_name( i ), pMob->healing_from[ i ] );
			add_buf( buffer, buf );
		}
	}


	if ( pMob->pShop )
	{
		SHOP_DATA * pShop;
		int iTrade;

		pShop = pMob->pShop;

		sprintf( buf,
				"\n\r{GShop data for{x [%5d]:\n\r"
				"  {GMarkup for purchaser{x: %d%%\n\r"
				"  {GMarkdown for seller{x:  %d%%\n\r",
				pShop->keeper, pShop->profit_buy, pShop->profit_sell );

		add_buf( buffer, buf );

		sprintf( buf, "  {GHours{x: %d to %d.\n\r",
				pShop->open_hour, pShop->close_hour );
		add_buf( buffer, buf );

		sprintf( buf, "  {GShop room{x: %d.\n\r",
				pShop->shop_room );
		add_buf( buffer, buf );

		for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
		{
			if ( pShop->buy_type[ iTrade ] != 0 )
			{
				if ( iTrade == 0 )
				{
					add_buf( buffer, "  Number Trades Type\n\r" );
					add_buf( buffer, "  ------ -----------\n\r" );
				}
				sprintf( buf, "  [%4d] %s\n\r", iTrade,
						flag_string( type_flags, pShop->buy_type[ iTrade ] ) );
				add_buf( buffer, buf );
			}
		}
	}

	if ( pMob->pBank )
	{
		BANK_DATA * pBank;

		pBank = pMob->pBank;

		sprintf( buf,
				"\n\r{GBank data for{x [%5d]:\n\r"
				"  {GProvision{x: %d%%\n\r",
				pBank->banker, pBank->provision );

		add_buf( buffer, buf );

		sprintf( buf, "  {GHours{x: %d to %d.\n\r",
				pBank->open_hour, pBank->close_hour );
		add_buf( buffer, buf );
	}

	if ( pMob->pRepair )
	{
		REPAIR_DATA * rep;
		int i;

		sprintf( buf, "\n\r{GRepair hours{x: %d to %d.\n\r",
				pMob->pRepair->repair_open_hour, pMob->pRepair->repair_close_hour );
		add_buf( buffer, buf );
		sprintf( buf, "\n\r{GRepair room{x: %d.\n\r",
				pMob->pRepair->repair_room );
		add_buf( buffer, buf );
		for ( i = 1, rep = pMob->pRepair; rep; rep = rep->next, i++ )
		{
			sprintf( buf, "[%d] Repair: [%-17.17s] Max cost:[%d] Repair cost mod:[%d] Min cond:[%d]\n\r",
					i, repair_item_table[ rep->item_type ].name, rep->max_cost, rep->repair_cost_mod, rep->min_condition );

			add_buf( buffer, buf );
		}
	}

	if ( pMob->progs )
	{
		int cnt;

		sprintf( buf, "\n\r{CMOBPrograms for [%5d]{x:\n\r", pMob->vnum );
		add_buf( buffer, buf );

		for ( cnt = 0, list = pMob->progs; list; list = list->next )
		{
			if ( cnt == 0 )
			{
				add_buf( buffer, " Number Name            Trigger    Positions\n\r" );
				add_buf( buffer, " ------ --------------- ---------- ---------\n\r" );
			}

			sprintf( buf, "[%5d] %-15s {C%-10s{x %s\n\r                        Phrase: {G%s{x\n\r", cnt,
					list->name, ext_bit_name( prog_flags, list->trig_type ),
					flag_string( position_trig_flags, list->valid_positions ),
					list->trig_phrase );
			add_buf( buffer, buf );
			cnt++;
		}
	}

	if ( !IS_NULLSTR( pMob->comments ) )
	{
		add_buf( buffer, "\n\r{CKomentarze{x:\n\r" );
		add_buf( buffer, numlineas( pMob->comments ) );
	}

	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );
	return FALSE;
}

MEDIT( medit_create )
{
	MOB_INDEX_DATA *pMob;
	AREA_DATA *pArea;
	int  value;
	int  iHash;

	value = atoi( argument );
	if ( argument[0] == '\0' || value == 0 )
	{
		send_to_char( "Sk³adnia: medit create [vnum]\n\r", ch );
		return FALSE;
	}

	pArea = get_vnum_area( value );

	if ( !pArea )
	{
		send_to_char( "MEdit:  That vnum is not assigned an area.\n\r", ch );
		return FALSE;
	}

	if ( !IS_BUILDER( ch, pArea ) )
	{
		send_to_char( "MEdit:  Vnum in an area you cannot build in.\n\r", ch );
		return FALSE;
	}

	if ( get_mob_index( value ) )
	{
		send_to_char( "MEdit:  Mob o takim vnumie ju¿ istnieje.\n\r", ch );
		return FALSE;
	}

	pMob			= new_mob_index();
	pMob->vnum			= value;
	pMob->area			= pArea;


	if ( value > (int)top_vnum_mob )
		top_vnum_mob = value;

	ext_flags_clear( pMob->act );
	EXT_SET_BIT( pMob->act, ACT_IS_NPC );
	iHash			= value % MAX_KEY_HASH;
	pMob->next			= mob_index_hash[iHash];
	mob_index_hash[iHash]	= pMob;
	ch->desc->pEdit		= (void *)pMob;

	send_to_char( "Mobile Created.\n\r", ch );
	return TRUE;
}



MEDIT( medit_spec )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: spec [special function]\n\r", ch );
		return FALSE;
	}


	if ( !str_cmp( argument, "none" ) )
	{
		pMob->spec_fun = NULL;

		send_to_char( "Spec removed.\n\r", ch);
		return TRUE;
	}

	if ( spec_lookup( argument ) )
	{
		pMob->spec_fun = spec_lookup( argument );
		send_to_char( "Spec set.\n\r", ch);
		return TRUE;
	}

	send_to_char( "MEdit: No such special function.\n\r", ch );
	return FALSE;
}

MEDIT( medit_damtype )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: damtype [damage message]\n\r", ch );
		send_to_char( "Brak parametru - wpisz '? weapon'.\n\r", ch );
		return FALSE;
	}

	pMob->dam_type = attack_lookup(argument);
	send_to_char( "Damage type set.\n\r", ch);
	return TRUE;
}


MEDIT( medit_align )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: alignment [number]\n\r", ch );
		return FALSE;
	}

	pMob->alignment = atoi( argument );

	send_to_char( "Alignment set.\n\r", ch);
	return TRUE;
}


MEDIT( medit_corpse )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: corpse [vnum]\n\r", ch );
		return FALSE;
	}


	pMob->corpse_vnum = atoi( argument );
	send_to_char( "Corpse vnum set.\n\r", ch);
	return TRUE;
}

MEDIT( medit_skin )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: skin [jaki mno¿nik]\n\r", ch );
		return FALSE;
	}


	pMob->skin_multiplier = atoi( argument );
	send_to_char( "Skin multiplier set.\n\r", ch);
	return TRUE;
}

MEDIT( medit_exp_multiplier )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: exp_multiplier [jaki mno¿nik procentowy] (100 badz 0 to bez zmian, 100%, by za moba nie bylo expa ustaw act no_exp)\n\r", ch );
		return FALSE;
	}

	pMob->exp_multiplier = atoi( argument );
	send_to_char( "Exp multiplier set.\n\r", ch);
	return TRUE;
}

int max_hp_lev( int level, int power )
{
	int i;

	for ( i = 0; i < MAX_TYPE; i++ )
	{
		if ( level >= standard_table[i].min_lev
				&&   level <= standard_table[i].max_lev )
			return level * ( standard_table[i].max_hp + ( 5 * power ) );
	}

	return 0;
}

int min_hp_lev( int level, int power )
{
	int i;

	for ( i = 0; i < MAX_TYPE; i++ )
	{
		if ( level >= standard_table[i].min_lev
				&&   level <= standard_table[i].max_lev )
			return level * ( standard_table[i].min_hp + ( 5 * power ) );
	}

	return 0;
}

MEDIT( medit_hpcalc )
{
	int     level = 0;
	int     div = 0;
	int     power = 0;
	int     min = 0;
	int     max = 0;
	char    arg[MAX_INPUT_LENGTH];

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Sk³adnia:\n\r"
				"hpcalc <level> <liczba ¶cian kostek> [<moc moba>]\n\r"
				"<moc moba> =\n\r -2 - leszcz\n\r -1 - s³aby\n\r  0 - normalny\n\r  1 - mocny\n\r  2 - killer\n\r", ch );
		return FALSE;
	}

	if ( !is_number( arg ) )
	{
		send_to_char( "Level powinien byc liczb±.\n\r", ch );
		return FALSE;
	}
	level = atoi(arg);

	if ( level < 1 || level > MAX_LEVEL )
	{
		print_char( ch, "Level powinien byæ liczb± od 1 do %d.\n\r", MAX_LEVEL );
		return FALSE;
	}

	argument = one_argument( argument, arg );
	if ( !is_number( arg ) )
	{
		send_to_char( "Liczba ¶cian kostek powinna byæ liczb±.\n\r", ch );
		return FALSE;
	}
	div = atoi(arg);

	one_argument( argument, arg );
	power = 0;
	if ( arg[0] != '\0' )
	{
		if ( !is_number( arg ) )
		{
			send_to_char( "Moc moba powinna byæ liczb±.\n\r", ch );
			return FALSE;
		}
		power = atoi(arg);
	}

	if ( power < -2 || power > 2 )
	{
		send_to_char( "Dostêpne moce mobów:\n\r -2 - leszcz\n\r -1 - s³aby\n\r  0 - normalny\n\r  1 - mocny\n\r  2 - killer\n\r", ch );
		return FALSE;
	}

	min = min_hp_lev( level, power );
	max = max_hp_lev( level, power );

	if ( div < 1 || div > min )
	{
		print_char( ch, "Liczba ¶cian kostek powinna zawieraæ siê od 1 do %d.\n\r", min );
		return FALSE;
	}

	print_char( ch, "Min hp na level %d: %d\n\r", level, min );
	print_char( ch, "Max hp na level %d: %d\n", level, max );
	print_char( ch, "Przyk³adowy hitdice: %dd%d+%d\n", ( max-min+div )/div, div, min-div );
	return FALSE;
}

MEDIT( medit_level )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: level [number]\n\r", ch );
		return FALSE;
	}

	if ( !IS_SET( ch->wiz_config, WIZCFG_LIMITLEVEL ) )
		pMob->level = UMAX( 1, atoi( argument ) );
	else
	{
		if ( race_table[pMob->race].max_level > 0 )
			pMob->level = URANGE( race_table[pMob->race].min_level, atoi( argument ), race_table[pMob->race].max_level );
		else
			pMob->level = UMAX( 1, atoi( argument ) );
	}

	print_char( ch, "Level ustawiony na %d.\n\r", pMob->level );
	return TRUE;
}



MEDIT( medit_desc )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' )
	{
		string_append( ch, &pMob->description );
		return TRUE;
	}

	send_to_char( "Sk³adnia: desc    - line edit\n\r", ch );
	return FALSE;
}




MEDIT( medit_long )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: long [string]\n\r", ch );
		return FALSE;
	}

	free_string( pMob->long_descr );
	pMob->long_descr = str_dup( argument );
	sprintf( pMob->long_descr, "%s", capitalize(pMob->long_descr) );

	send_to_char( "Long description set.\n\r", ch);
	return TRUE;
}



MEDIT( medit_short )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: short [string]\n\r", ch );
		return FALSE;
	}

	free_string( pMob->short_descr );
	pMob->short_descr = str_dup( argument );

	send_to_char( "Short description set.\n\r", ch);
	return TRUE;
}



MEDIT( medit_name )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: name [string]\n\r", ch );
		return FALSE;
	}

	free_string( pMob->player_name );
	pMob->player_name = str_dup( argument );

	send_to_char( "Name set.\n\r", ch);
	return TRUE;
}

MEDIT( medit_shop )
{
	MOB_INDEX_DATA * pMob;
	char command[ MAX_INPUT_LENGTH ];
	char arg1[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, command );
	argument = one_argument( argument, arg1 );

	EDIT_MOB( ch, pMob );

	if ( command[ 0 ] == '\0' )
	{
		send_to_char( "Sk³adnia: shop hours <#xopening> <#xclosing>\n\r", ch );
		send_to_char( "         shop profit <#xbuying%> <#xselling%>\n\r", ch );
		send_to_char( "         shop type <#x0-4> <item type>\n\r", ch );
		send_to_char( "         shop room <room vnum>\n\r", ch );
		send_to_char( "         shop assign\n\r", ch );
		send_to_char( "         shop remove\n\r", ch );
		return FALSE;
	}


	if ( !str_cmp( command, "hours" ) )
	{
		if ( arg1[ 0 ] == '\0' || !is_number( arg1 )
				|| argument[ 0 ] == '\0' || !is_number( argument ) )
		{
			send_to_char( "Sk³adnia: shop hours [#xopening] [#xclosing]\n\r", ch );
			return FALSE;
		}

		if ( !pMob->pShop )
		{
			send_to_char( "MEdit:  Debes crear un shop primero (shop assign).\n\r", ch );
			return FALSE;
		}

		pMob->pShop->open_hour = atoi( arg1 );
		pMob->pShop->close_hour = atoi( argument );

		send_to_char( "Shop hours set.\n\r", ch );
		return TRUE;
	}


	if ( !str_cmp( command, "profit" ) )
	{
		if ( arg1[ 0 ] == '\0' || !is_number( arg1 )
				|| argument[ 0 ] == '\0' || !is_number( argument ) )
		{
			send_to_char( "Sk³adnia: shop profit [#xbuying%] [#xselling%]\n\r", ch );
			return FALSE;
		}

		if ( !pMob->pShop )
		{
			send_to_char( "MEdit:  Debes crear un shop primero (shop assign).\n\r", ch );
			return FALSE;
		}

		pMob->pShop->profit_buy = atoi( arg1 );
		pMob->pShop->profit_sell = atoi( argument );

		send_to_char( "Shop profit set.\n\r", ch );
		return TRUE;
	}

	if ( !str_cmp( command, "room" ) )
	{
		if ( arg1[ 0 ] == '\0' || !is_number( arg1 ) )
		{
			send_to_char( "Sk³adnia: shop room <vnum>\n\r", ch );
			return FALSE;
		}

		if ( !pMob->pShop )
		{
			send_to_char( "MEdit:  Najpierw przydziel sklep (shop assign).\n\r", ch );
			return FALSE;
		}

		if ( !get_room_index( atoi( arg1 ) ) )
		{
			send_to_char( "Nie ma takiej lokacji.\n\r", ch );
			return FALSE;
		}

		pMob->pShop->shop_room = atoi( arg1 );

		send_to_char( "Shop room set.\n\r", ch );
		return TRUE;
	}

	if ( !str_cmp( command, "type" ) )
	{
		char buf[ MAX_INPUT_LENGTH ];
		int value;

		if ( arg1[ 0 ] == '\0' || !is_number( arg1 )
				|| argument[ 0 ] == '\0' )
		{
			send_to_char( "Sk³adnia: shop type [#x0-4] [item type]\n\r", ch );
			return FALSE;
		}

		if ( atoi( arg1 ) >= MAX_TRADE )
		{
			sprintf( buf, "MEdit:  May sell %d items max.\n\r", MAX_TRADE );
			send_to_char( buf, ch );
			return FALSE;
		}

		if ( !pMob->pShop )
		{
			send_to_char( "MEdit:  Debes crear un shop primero (shop assign).\n\r", ch );
			return FALSE;
		}

		if ( ( value = flag_value( type_flags, argument ) ) == NO_FLAG )
		{
			send_to_char( "MEdit:  That type of item is not known.\n\r", ch );
			return FALSE;
		}

		pMob->pShop->buy_type[ atoi( arg1 ) ] = value;

		send_to_char( "Shop type set.\n\r", ch );
		return TRUE;
	}

	/* shop assign && shop delete by Phoenix */

	if ( !str_prefix( command, "assign" ) )
	{
		if ( pMob->pShop )
		{
			send_to_char( "Mob already has a shop assigned to it.\n\r", ch );
			return FALSE;
		}

		pMob->pShop	= new_shop();
		if ( !shop_first )
			shop_first	= pMob->pShop;
		if ( shop_last )
			shop_last->next	= pMob->pShop;
		shop_last	= pMob->pShop;

		pMob->pShop->keeper	= pMob->vnum;

		send_to_char( "New shop assigned to mobile.\n\r", ch );
		return TRUE;
	}

	if ( !str_prefix( command, "remove" ) )
	{
		SHOP_DATA * pShop;

		pShop	= pMob->pShop;
		pMob->pShop	= NULL;

		if ( pShop == shop_first )
		{
			if ( !pShop->next )
			{
				shop_first = NULL;
				shop_last = NULL;
			}
			else
				shop_first = pShop->next;
		}
		else
		{
			SHOP_DATA *ipShop;

			for ( ipShop = shop_first; ipShop; ipShop = ipShop->next )
			{
				if ( ipShop->next == pShop )
				{
					if ( !pShop->next )
					{
						shop_last = ipShop;
						shop_last->next = NULL;
					}
					else
						ipShop->next = pShop->next;
				}
			}
		}

		free_shop( pShop );

		send_to_char( "Mobile is no longer a shopkeeper.\n\r", ch );
		return TRUE;
	}

	medit_shop( ch, "" );
	return FALSE;
}

MEDIT( medit_repair )
{
	MOB_INDEX_DATA * pMob;
	static char * syntax = "Skladnia:\n\rdodanie nowego elementu    : repair new 'nazwa typu'\n\r"
		"usuniecie elementu         : repair delete 'numer'\n\r"
		"modyfikacja elementu       : repair 'numer' 'cond | cost | mod' 'liczba'\n\r"
		"repair hour <otwarcie> <zamkniecie>\n\r"
		"repair room <vnum_lokacji>\n\r"
		"repair 1 cost 200 - typ 1 bedzie naprawiany ale tylko gdy cost < 200\n\r"
		"repair 1 cond 50  - typ 1 bedzie naprawiany ale tylko gdy condition >= 50\n\r"
		"repair 1 mod 250  - typ 1 bedzie naprawiany ale koszt naprawy zwiekszony o 150%\n\r";
	char command[ MAX_INPUT_LENGTH ];
	char arg1[ MAX_INPUT_LENGTH ];
	REPAIR_DATA *repair, *prep;
	int type, number, i;

	argument = one_argument( argument, command );
	argument = one_argument( argument, arg1 );

	EDIT_MOB( ch, pMob );

	if ( command[ 0 ] == '\0' )
	{
		send_to_char( syntax, ch );
		return FALSE;
	}


	//dodawanie nowego elementy, na koncu listy
	if ( !str_prefix( command, "new" ) )
	{

		if ( ( type = get_rep_type( arg1 ) ) < 0 )
		{
			print_char( ch, "Type: %d\n\r", type );
			send_to_char( syntax, ch );
			send_to_char( "\n\r{GMozliwe typy:{x\n\r", ch );

			for ( i = 0; repair_item_table[ i ].name; i++ )
				print_char( ch, "{g%-17.17s{x - %s\n\r", repair_item_table[ i ].name, repair_item_table[ i ].info );

			return FALSE;
		}

		//sprawdzenie czy juz taki typ jest
		for ( prep = pMob->pRepair; prep; prep = prep->next )
			if ( prep->item_type == type )
			{
				send_to_char( "Taki typ juz jest.\n\r", ch );
				return FALSE;
			}

		repair = new_repair();
		repair->item_type	= type;
		repair->max_cost	= 0;
		repair->repair_cost_mod = 100;
		repair->min_condition	= 1;
		repair->next	= NULL;

		//dodanie do listy moba
		if ( !pMob->pRepair )
			pMob->pRepair	= repair;
		else
		{
			for ( prep = pMob->pRepair; prep->next; prep = prep->next );
			prep->next	= repair;
		}

		send_to_char( "Ok\n\r", ch );
		return TRUE;
	}
	else if ( !str_prefix( command, "delete" ) )
	{
		//wstepne sprawdzenie argumentu
		if ( !is_number( arg1 ) || ( number = atoi( arg1 ) ) < 0 )
		{
			send_to_char( syntax, ch );
			return FALSE;
		}

		if ( !pMob->pRepair )
		{
			send_to_char( "Nie ma ¿adnych typów do usuniêcia.\n\r", ch );
			return FALSE;
		}

		//pierwszy element do usuniecia
		if ( number == 1 )
		{
			prep = pMob->pRepair;
			pMob->pRepair = pMob->pRepair->next;
			free( prep );
			send_to_char( "Usunieto typ.\n\r", ch );
			return TRUE;
		}
		else
		{
			for ( i = 1, prep = pMob->pRepair; prep; prep = prep->next, i++ )
				if ( i == number - 1 )
				{
					repair = prep->next;
					prep->next = repair->next;
					free( repair );
					send_to_char( "Usunieto typ.\n\r", ch );
					return TRUE;
				}

			send_to_char( "Taki typ nie istnieje.\n\r", ch );
			return FALSE;
		}
	}
	else if ( !str_prefix( command, "hours" ) )
	{
		if ( arg1[ 0 ] == '\0' || !is_number( arg1 )
				|| argument[ 0 ] == '\0' || !is_number( argument ) )
		{
			send_to_char( syntax, ch );
			return FALSE;
		}

		if ( !pMob->pRepair )
		{
			send_to_char( "Nie ma zadnego typu zdefiniowanego.\n\r", ch );
			return FALSE;
		}

		pMob->pRepair->repair_open_hour = atoi( arg1 );
		pMob->pRepair->repair_close_hour = atoi( argument );

		send_to_char( "Repair hours set.\n\r", ch );
		return TRUE;
	}
	else if ( !str_prefix( command, "room" ) )
	{
		if ( arg1[ 0 ] == '\0' || !is_number( arg1 ) )
		{
			send_to_char( syntax, ch );
			return FALSE;
		}

		if ( !pMob->pRepair )
		{
			send_to_char( "Nie ma zadnego typu zdefiniowanego.\n\r", ch );
			return FALSE;
		}

		if ( !get_room_index( atoi( arg1 ) ) )
		{
			send_to_char( "Nie ma takiej lokacji.\n\r", ch );
			return FALSE;
		}

		pMob->pRepair->repair_room = atoi( arg1 );

		send_to_char( "Repair room set.\n\r", ch );
		return TRUE;
	}
	else if ( is_number( command ) )
	{
		if ( ( number = atoi( command ) ) < 0 )
		{
			send_to_char( syntax, ch );
			return FALSE;
		}

		repair = NULL;

		for ( i = 1, prep = pMob->pRepair; prep; prep = prep->next, i++ )
			if ( i == number )
			{
				repair = prep;
				break;
			}

		if ( !repair )
		{
			send_to_char( "Nie ma takiego elementu.\n\r", ch );
			send_to_char( syntax, ch );
			return FALSE;
		}

		if ( !is_number( argument ) || ( number = atoi( argument ) ) < 0 )
		{
			send_to_char( syntax, ch );
			return FALSE;
		}

		if ( !str_prefix( arg1, "cost" ) )
		{
			repair->max_cost = UMAX( 0, number );
			send_to_char( "Ok.\n\r", ch );
			return TRUE;
		}
		else if ( !str_prefix( arg1, "mod" ) )
		{
			repair->repair_cost_mod = UMAX( 1, number );
			send_to_char( "Ok.\n\r", ch );
			return TRUE;
		}
		else if ( !str_prefix( arg1, "cond" ) )
		{
			repair->min_condition = URANGE( 1, number, 99 );
			send_to_char( "Ok.\n\r", ch );
			return TRUE;
		}
	}

	send_to_char( syntax, ch );
	return FALSE;
}

MEDIT( medit_bank )
{
	MOB_INDEX_DATA * pMob;
	char command[ MAX_INPUT_LENGTH ];
	char arg1[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, command );
	argument = one_argument( argument, arg1 );

	EDIT_MOB( ch, pMob );

	if ( command[ 0 ] == '\0' )
	{
		send_to_char( "Sk³adnia: bank hours <#xopening> <#xclosing>\n\r", ch );
		send_to_char( "         bank provision <#x%>\n\r", ch );
		send_to_char( "         bank room <vnum lokacji>\n\r", ch );
		send_to_char( "         bank assign\n\r", ch );
		send_to_char( "         bank remove\n\r", ch );
		return FALSE;
	}


	if ( !str_cmp( command, "hours" ) )
	{
		if ( arg1[ 0 ] == '\0' || !is_number( arg1 )
				|| argument[ 0 ] == '\0' || !is_number( argument ) )
		{
			send_to_char( "Sk³adnia: bank hours [#xopening] [#xclosing]\n\r", ch );
			return FALSE;
		}

		if ( !pMob->pBank )
		{
			send_to_char( "MEdit:  Najpierw przydziel bank mobowi (bank assign).\n\r", ch );
			return FALSE;
		}

		pMob->pBank->open_hour = atoi( arg1 );
		pMob->pBank->close_hour = atoi( argument );

		send_to_char( "Bank hours set.\n\r", ch );
		return TRUE;
	}

	else if ( !str_cmp( command, "room" ) )
	{
		if ( arg1[ 0 ] == '\0' || !is_number( arg1 ) )
		{
			send_to_char( "Sk³adnia: bank room <vnum lokacji>\n\r", ch );
			return FALSE;
		}

		if ( !pMob->pBank )
		{
			send_to_char( "MEdit:  Najpierw przydziel bank mobowi (bank assign).\n\r", ch );
			return FALSE;
		}

		if ( !get_room_index( atoi( arg1 ) ) )
		{
			send_to_char( "Nie ma takiej lokacji.\n\r", ch );
			return FALSE;
		}

		pMob->pBank->bank_room = atoi( arg1 );

		send_to_char( "Bank room set.\n\r", ch );
		return TRUE;
	}

	if ( !str_cmp( command, "provision" ) )
	{
		if ( arg1[ 0 ] == '\0' || !is_number( arg1 ) )
		{
			send_to_char( "Sk³adnia: bank provision [#x%]\n\r", ch );
			return FALSE;
		}

		if ( !pMob->pBank )
		{
			send_to_char( "MEdit:  Najpierw przydziel bank mobowi (bank assign).\n\r", ch );
			return FALSE;
		}

		pMob->pBank->provision = atoi( arg1 );

		send_to_char( "Bank provision set.\n\r", ch );
		return TRUE;
	}


	/* bank assign && bank delete by Phoenix */

	if ( !str_prefix( command, "assign" ) )
	{
		if ( pMob->pBank )
		{
			send_to_char( "Ten mob ma juz przydzielony bank.\n\r", ch );
			return FALSE;
		}

		pMob->pBank	= new_bank();
		if ( !bank_first )
			bank_first	= pMob->pBank;
		if ( bank_last )
			bank_last->next	= pMob->pBank;
		bank_last	= pMob->pBank;

		pMob->pBank->banker	= pMob->vnum;

		send_to_char( "Nowy bank przydzielony do moba.\n\r", ch );
		return TRUE;
	}

	if ( !str_prefix( command, "remove" ) )
	{
		BANK_DATA * pBank;

		pBank	= pMob->pBank;
		pMob->pBank	= NULL;

		if ( pBank == bank_first )
		{
			if ( !pBank->next )
			{
				bank_first = NULL;
				bank_last = NULL;
			}
			else
				bank_first = pBank->next;
		}
		else
		{
			BANK_DATA *ipBank;

			for ( ipBank = bank_first; ipBank; ipBank = ipBank->next )
			{
				if ( ipBank->next == pBank )
				{
					if ( !pBank->next )
					{
						bank_last = ipBank;
						bank_last->next = NULL;
					}
					else
						ipBank->next = pBank->next;
				}
			}
		}

		free_bank( pBank );

		send_to_char( "Bank usuniety z moba.\n\r", ch );
		return TRUE;
	}

	medit_bank( ch, "" );
	return FALSE;
}

/* ROM medit functions: */

MEDIT( medit_sex )
{
	MOB_INDEX_DATA * pMob;
	int value;

	if ( argument[ 0 ] != '\0' )
	{
		EDIT_MOB( ch, pMob );

		if ( ( value = flag_value( sex_flags, argument ) ) != NO_FLAG )
		{
			pMob->sex = value;

			send_to_char( "Sex set.\n\r", ch );
			return TRUE;
		}
	}

	send_to_char( "Sk³adnia: sex [sex]\n\r"
			"Wpisz: '? sex' ¿eby zobaczyæ dostêpne p³ci.\n\r", ch );
	return FALSE;
}


MEDIT( medit_act )
{
	MOB_INDEX_DATA * pMob;
	BITVECT_DATA * value;

	if ( argument[ 0 ] != '\0' )
	{
		EDIT_MOB( ch, pMob );
		value = ext_flag_lookup( argument, act_flags );

		if ( value != &EXT_NONE )
		{
			EXT_TOGGLE_BIT( pMob->act, *value );
			EXT_SET_BIT( pMob->act, ACT_IS_NPC );

			send_to_char( "Act flag toggled.\n\r", ch );
			return TRUE;
		}
	}

	send_to_char( "Sk³adnia: act [flag]\n\rType '? act' for a list of flags.\n\r", ch );
	return FALSE;
}


MEDIT( medit_affect )

{

	MOB_INDEX_DATA * pMob;

	BITVECT_DATA * value;

	if ( argument[ 0 ] != '\0' )

	{

		EDIT_MOB( ch, pMob );

		value = ext_flag_lookup( argument, affect_flags );

		if ( value != &EXT_NONE )

		{

			EXT_TOGGLE_BIT( pMob->affected_by, *value );

			if ( EXT_IS_SET( pMob->affected_by, *value ) )

				send_to_char( "Affect dodany.\n\r", ch );

			else

				send_to_char( "Affect zabrany.\n\r", ch );

			return TRUE;

		}

		return FALSE;

	}

	send_to_char( "Sk³adnia: affect [flag]\n\r"

	              "Wpisz: '? affect' ¿eby zobaczyæ listê affectów.\n\r", ch );

	return FALSE;

}


MEDIT( medit_ac )
{
	MOB_INDEX_DATA *pMob;
	char arg[MAX_INPUT_LENGTH];
	int pierce, bash, slash, exotic;

	do   /* So that I can use break and send the syntax in one place */
	{
		if ( argument[0] == '\0' )  break;

		EDIT_MOB(ch, pMob);
		argument = one_argument( argument, arg );

		if ( !is_number( arg ) )  break;
		pierce = atoi( arg );
		argument = one_argument( argument, arg );

		if ( arg[0] != '\0' )
		{
			if ( !is_number( arg ) )  break;
			bash = atoi( arg );
			argument = one_argument( argument, arg );
		}
		else
			bash = pMob->ac[AC_BASH];

		if ( arg[0] != '\0' )
		{
			if ( !is_number( arg ) )  break;
			slash = atoi( arg );
			argument = one_argument( argument, arg );
		}
		else
			slash = pMob->ac[AC_SLASH];

		if ( arg[0] != '\0' )
		{
			if ( !is_number( arg ) )  break;
			exotic = atoi( arg );
		}
		else
			exotic = pMob->ac[AC_EXOTIC];

		pMob->ac[AC_PIERCE] = pierce;
		pMob->ac[AC_BASH]   = bash;
		pMob->ac[AC_SLASH]  = slash;
		pMob->ac[AC_EXOTIC] = exotic;

		send_to_char( "Ac set.\n\r", ch );
		return TRUE;
	} while ( FALSE );    /* Just do it once.. */

	send_to_char( "Sk³adnia: ac [ac-pierce [ac-bash [ac-slash [ac-exotic]]]]\n\r"
			"help MOB_AC  gives a list of reasonable ac-values.\n\r", ch );
	return FALSE;
}

/*************************/
MEDIT( medit_stats )
{
	MOB_INDEX_DATA *pMob;
	char arg[MAX_INPUT_LENGTH];
	int str_=78, int_=78, wis_=78, dex_=78, con_=78, cha_=78, luc_=78;

	do
	{
		if ( argument[0] == '\0' )  break;

		EDIT_MOB(ch, pMob);
		argument = one_argument( argument, arg );

		if ( !is_number( arg ) )
			break;

		str_ = URANGE(3, atoi( arg ), MAX_STAT_VALUE );

		argument = one_argument( argument, arg );

		if ( arg[0] != '\0' )
		{
			if ( !is_number( arg ) )
				break;
			int_ = URANGE(3, atoi( arg ),MAX_STAT_VALUE);
			argument = one_argument( argument, arg );
		}

		if ( arg[0] != '\0' )
		{
			if ( !is_number( arg ) )
				break;
			wis_ = URANGE(3, atoi( arg ),MAX_STAT_VALUE);
			argument = one_argument( argument, arg );
		}

		if ( arg[0] != '\0' )
		{
			if ( !is_number( arg ) )
				break;
			dex_ = URANGE(3, atoi( arg ), MAX_STAT_VALUE);
			argument = one_argument( argument, arg );
		}

		if ( arg[0] != '\0' )
		{
			if ( !is_number( arg ) )
				break;
			con_ = URANGE(3, atoi( arg ), MAX_STAT_VALUE);
			argument = one_argument( argument, arg );
		}

		if ( arg[0] != '\0' )
		{
			if ( !is_number( arg ) )
				break;
			cha_ = URANGE(3, atoi( arg ), MAX_STAT_VALUE);
			argument = one_argument( argument, arg );
		}

		if ( arg[0] != '\0' )
		{
			if ( !is_number( arg ) )
				break;
			luc_ = URANGE(3, atoi( arg ), MAX_STAT_VALUE);
		}


		pMob->stats[0] = str_;
		pMob->stats[1] = int_;
		pMob->stats[2] = wis_;
		pMob->stats[3] = dex_;
		pMob->stats[4] = con_;
		pMob->stats[5] = cha_;
		pMob->stats[6] = luc_;

		send_to_char( "Statystyki moba ustawione.\n\r", ch );
		return TRUE;
	} while ( FALSE );

	send_to_char( "Sk³adnia: stats [str int wis dex con cha luc]\n\r" , ch );
	return FALSE;
}

MEDIT( medit_odmiana )
{
	MOB_INDEX_DATA *pMob;
	char arg1[MAX_INPUT_LENGTH];

	do
	{
		if ( argument[0] == '\0' )  break;

		EDIT_MOB(ch, pMob);
		argument = one_argument( argument, arg1);

		if ( argument[0] == '\0' )  break;


		if(!str_prefix(arg1,"dopelniacz"))
		{
			free_string( pMob->name2 );
			pMob->name2=str_dup(argument);
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}

		if(!str_prefix(arg1,"celownik"))
		{
			free_string( pMob->name3 );
			pMob->name3=str_dup(argument);
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}

		if(!str_prefix(arg1,"biernik"))
		{
			free_string( pMob->name4 );
			pMob->name4=str_dup(argument);
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}


		if(!str_prefix(arg1,"narzednik"))
		{
			free_string( pMob->name5 );
			pMob->name5=str_dup(argument);
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}

		if(!str_prefix(arg1,"miejscownik"))
		{
			free_string( pMob->name6 );
			pMob->name6=str_dup(argument);
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}
		send_to_char( "Odmiana bledna.\n\r", ch );
		return TRUE;
	} while ( FALSE );

	send_to_char( "Sk³adnia: odmiana <przypadek> <string>\n\r" , ch );
	return FALSE;
}


/*****************/

MEDIT( medit_form )
{
	MOB_INDEX_DATA *pMob;
	int value;

	if ( argument[0] != '\0' )
	{
		EDIT_MOB( ch, pMob );

		if ( ( value = flag_value( form_flags, argument ) ) != NO_FLAG )
		{
			pMob->form ^= value;
			send_to_char( "Form toggled.\n\r", ch );
			return TRUE;
		}
	}

	send_to_char( "Sk³adnia: form [flags]\n\r"
			"Type '? form' for a list of flags.\n\r", ch );
	return FALSE;
}

MEDIT( medit_part )
{
	MOB_INDEX_DATA *pMob;
	int value;

	if ( argument[0] != '\0' )
	{
		EDIT_MOB( ch, pMob );

		if ( ( value = flag_value( part_flags, argument ) ) != NO_FLAG )
		{
			pMob->parts ^= value;
			send_to_char( "Parts toggled.\n\r", ch );
			return TRUE;
		}
	}

	send_to_char( "Sk³adnia: part [flags]\n\r"
			"Type '? part' for a list of flags.\n\r", ch );
	return FALSE;
}


MEDIT( medit_res )
{
	MOB_INDEX_DATA *pMob;
	char arg[MAX_INPUT_LENGTH];
	int value, type;

	argument = one_argument(argument,arg);

	if( argument[0] == '\0' 	||
			arg[0]=='\0'		||
			!is_number(argument) 	||
			(type = resist_number(arg)) < 0  ||
			(value = atoi(argument)) > 100 ||
			value < -100   )
	{
		send_to_char("Sk³adnia : resist 'nazwa' warto¶æ_procentowa (-100..100).\n\rDostêpne odporno¶ci:\n\r",ch);
		send_to_char("- all          (na wzystko)\n\r",ch);
		send_to_char("- slash        (ataki/bronie tn±ce)\n\r",ch);
		send_to_char("- pierce       (ataki/bronie k³uj±ce)\n\r",ch);
		send_to_char("- bash         (ataki/bronie obuchowe)\n\r",ch);
		send_to_char("- magic        (na magiê)\n\r",ch);
		send_to_char("- fire         (na ogieñ zwyk³y)\n\r",ch);
		send_to_char("- magic_fire   (na magiczny ogieñ)\n\r",ch);
		send_to_char("- electricity  (na elektryczno¶æ)\n\r",ch);
		send_to_char("- cold         (na zimno/lód)\n\r",ch);
		send_to_char("- acid         (na kwas)\n\r",ch);
		send_to_char("- poison       (na trucizny)\n\r",ch);
		send_to_char("- magic_weapon (na magiczne bronie)\n\r",ch);
		send_to_char("- charm        (na zauroczenia)\n\r",ch);
		send_to_char("- fear         (na strach/przera¿enie)\n\r",ch);
		send_to_char("- summon       (na przywo³ania)\n\r",ch);
		send_to_char("- negative     (na negatywn± energiê)\n\r",ch);
		send_to_char("- mental       (na ataki mentalne - psionika)\n\r",ch);
		send_to_char("- holy         (na pozytywn± energiê)\n\r",ch);
		send_to_char("- sound        (na dzwiêk)\n\r",ch);
		return FALSE;
	}

	switch(type)
	{
		case RESIST_MAGIC_WEAPON:
			if(value > 5 || value < 0)
			{
				send_to_char("Ta odpornosc jest inaczej traktowana.\n\r",ch);
				send_to_char("Zakres wartosc : (1..5)\n\r",ch);
				send_to_char("Odpornosc okresla jakie jest minimalne umagicznienie\n\r",ch);
				send_to_char("broni skuteczne przeciwko danej postaci.\n\r", ch);
				return FALSE;
			}

		case RESIST_ALL:
			send_to_char("Przy sprawdzaniu odpornosci w przypadku\n\r", ch);
			send_to_char("niezerowej odpornosci na wszystko, uwzgledniana\n\r", ch);
			send_to_char("jest korzystniejsza odpornosc.\n\r",ch);
			break;
		default:
			if(value < 0)
			{
				send_to_char("Ujemna wartosc w przypadku atakow powodujacych obrazenia\n\r", ch);
				send_to_char("zwieksza obrazenia. Kazdy punkt przemnazany jest przez '5', wiec\n\r",ch);
				send_to_char("ujemna odpornosc '10' przeliczana jest na '50' i zwieksza obrazenia\n\r",ch);
				send_to_char("o 50%",ch);
			}
			break;
	}

	EDIT_MOB( ch, pMob );
	pMob->resists[type % MAX_RESIST] = URANGE(-100, value, 100);
	return TRUE;
}

MEDIT( medit_heal_from )
{
	MOB_INDEX_DATA *pMob;
	char arg[MAX_INPUT_LENGTH];
	int value, type;

	argument = one_argument(argument,arg);

	if( argument[0] == '\0' 	||
			arg[0]=='\0'		||
			!is_number(argument) 	||
			(type = resist_number(arg)) < 0  ||
			(value = atoi(argument)) > 100 ||
			value < -100   )
	{
		send_to_char("Sk³adnia : hfrom 'nazwa' warto¶æ_procentowa (-100..100).\n\rDostêpne typy oddzia³ywañ:\n\r",ch);
		//		send_to_char("- all          (na wzystko)\n\r",ch);
		//send_to_char("- slash        (ataki/bronie tn±ce)\n\r",ch);
		//send_to_char("- pierce       (ataki/bronie k³uj±ce)\n\r",ch);
		//send_to_char("- bash         (ataki/bronie obuchowe)\n\r",ch);
		send_to_char("- magic        (na magiê)\n\r",ch);
		send_to_char("- fire         (na ogieñ zwyk³y)\n\r",ch);
		send_to_char("- magic_fire   (na magiczny ogieñ)\n\r",ch);
		send_to_char("- electricity  (na elektryczno¶æ)\n\r",ch);
		send_to_char("- cold         (na zimno/lód)\n\r",ch);
		send_to_char("- acid         (na kwas)\n\r",ch);
		//send_to_char("- poison       (na trucizny)\n\r",ch);
		send_to_char("- magic_weapon (na magiczne bronie)\n\r",ch);
		//send_to_char("- charm        (na zauroczenia)\n\r",ch);
		//send_to_char("- fear         (na strach/przera¿enie)\n\r",ch);
		//send_to_char("- summon       (na przywo³ania)\n\r",ch);
		send_to_char("- negative     (na negatywn± energiê)\n\r",ch);
		send_to_char("- mental       (na ataki mentalne - psionika)\n\r",ch);
		send_to_char("- holy         (na pozytywn± energiê)\n\r",ch);
		send_to_char("- sound        (na dzwiêk)\n\r",ch);
		return FALSE;
	}

	switch(type)
	{
	case RESIST_SLASH:
	case RESIST_PIERCE:
	case RESIST_BASH:
	case RESIST_POISON:
	case RESIST_CHARM:
	case RESIST_FEAR:
	case RESIST_SUMMON:
	   send_to_char("Nie mo¿na wybraæ tego typu oddzia³ywania lub te¿ nie jest ono jeszcze zaimplementowane.\n\r", ch );
	   return FALSE;
	default:
	   if ( value <= 0 )
	      {
		 send_to_char("Ujemne warto¶ci nie s± obs³ugiwane. Zamiast tego u¿yj resist z ujemn± warto¶ci±.\n\r", ch );
		 return FALSE;
	      }
	}

	EDIT_MOB( ch, pMob );
	pMob->healing_from[type % MAX_RESIST] = URANGE(1, value, 1000);
	return TRUE;
}


MEDIT( medit_material )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: material [string]\n\r", ch );
		return FALSE;
	}

	free_string( pMob->material );
	pMob->material = str_dup( argument );

	send_to_char( "Material ustawiony.\n\r", ch);
	return TRUE;
}

MEDIT( medit_off )
{
	MOB_INDEX_DATA *pMob;
	BITVECT_DATA * value;

	if ( argument[ 0 ] != '\0' )
	{
		EDIT_MOB( ch, pMob );
		value = ext_flag_lookup( argument, off_flags );

		if ( value != &EXT_NONE )
		{
			EXT_TOGGLE_BIT( pMob->off_flags, *value );
			if (EXT_IS_SET(pMob->off_flags, *value))
            {
				send_to_char( "Umiejêtno¶æ ofensywna {Gdodana{x.\n\r", ch );
            }
			else
            {
				send_to_char( "Umiejêtno¶æ ofensywna {Rusuniêta{x.\n\r", ch );
            }
			return TRUE;
		}
	}

	send_to_char( "Sk³adnia: off [flags]\n\rWpisz: '? off' ¿eby zobaczyæ listê umiejêtno¶ci.\n\r", ch );
	return FALSE;
}

MEDIT( medit_size )
{
	MOB_INDEX_DATA *pMob;
	int value;

	if ( argument[0] != '\0' )
	{
		EDIT_MOB( ch, pMob );

		if ( ( value = flag_value( size_flags, argument ) ) != NO_FLAG )
		{
			pMob->size = value;
			send_to_char( "Size set.\n\r", ch );
			return TRUE;
		}
	}

	send_to_char(
			"Sk³adnia: size [size]\n\r"
			"Wpisz: '? size' ¿eby zobaczyæ mo¿liwe rozmiary mobów.\n\r",
			ch
		    );
	return FALSE;
}

MEDIT( medit_hitdice )
{
	static char syntax[] = "Sk³adnia: hitdice <number> d <type> + <bonus>\n\r";
	char *num, *type, *bonus, *cp;
	MOB_INDEX_DATA *pMob;

	EDIT_MOB( ch, pMob );

	if ( argument[0] == '\0' )
	{
		send_to_char( syntax, ch );
		return FALSE;
	}

	num = cp = argument;

	while ( isdigit( *cp ) ) ++cp;
	while ( *cp != '\0' && !isdigit( *cp ) )  *(cp++) = '\0';

	type = cp;

	while ( isdigit( *cp ) ) ++cp;
	while ( *cp != '\0' && !isdigit( *cp ) ) *(cp++) = '\0';

	bonus = cp;

	while ( isdigit( *cp ) ) ++cp;
	if ( *cp != '\0' ) *cp = '\0';

	if ( ( !is_number( num   ) || atoi( num   ) < 1 )
			||   ( !is_number( type  ) || atoi( type  ) < 1 )
			||   ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
	{
		send_to_char( syntax, ch );
		return FALSE;
	}

	pMob->hit[DICE_NUMBER] = atoi( num   );
	pMob->hit[DICE_TYPE]   = atoi( type  );
	pMob->hit[DICE_BONUS]  = atoi( bonus );

	send_to_char( "Hitdice set.\n\r", ch );
	return TRUE;
}


MEDIT( medit_damdice )
{
	static char syntax[] = "Sk³adnia: damdice <number> d <type> +/- <bonus>\n\r";
	char *num, *type, *bonus, *cp;
	MOB_INDEX_DATA *pMob;
	int znak = 1;
	int numer, typ, modyfikator;

	EDIT_MOB( ch, pMob );

	if ( argument[0] == '\0' )
	{
		send_to_char( syntax, ch );
		return FALSE;
	}

	num = cp = argument;

	while ( isdigit( *cp ) ) ++cp;
	while ( *cp != '\0' && !isdigit( *cp ) )  *(cp++) = '\0';

	type = cp;

	while ( isdigit( *cp ) ) ++cp;
	while ( *cp != '\0' && !isdigit( *cp ) && *cp != '+' && *cp != '-' ) *(cp++) = '\0';

	if ( *cp == '-' )
	{
		znak = -1;
		*(cp++)='\0';
	} else if ( *cp == '+' ) {
		*(cp++)='\0';
	}

	bonus = cp;

	while ( isdigit( *cp ) ) ++cp;
	if ( *cp != '\0' )  *cp = '\0';

	if ( !( is_number( num ) && is_number( type ) && is_number( bonus ) ) )
	{
		send_to_char( syntax, ch );
		return FALSE;
	}

	numer = atoi( num );
	typ = atoi( type );
	modyfikator = atoi( bonus );

	if ( numer < 1 || typ < 1 )
	{
		send_to_char( syntax, ch );
		return FALSE;
	}

	if ( numer + modyfikator * znak < 0 )
	{
		send_to_char( syntax, ch );
		send_to_char( "Bonus jest mniejszy od number. Damage nie mo¿e byæ ujemny, ustawiam bonus na najmniejszy dopuszczalny\n\r", ch);
		while( numer < modyfikator ) modyfikator--;
	}

	pMob->damage[DICE_NUMBER] = numer;
	pMob->damage[DICE_TYPE]   = typ;
	pMob->damage[DICE_BONUS]  = znak * modyfikator;

	print_char( ch, "Damdice set to %d d %d %s%d.\n\r", numer, typ, znak == 1? "+ ": "- ", modyfikator );
	return TRUE;
}


bool alph_order( char * str, char *bstr )
{
	char *ptr, *bptr;

	for( ptr = str, bptr = bstr; *ptr != '\0' || *bptr != '\0'; ptr++, bptr++)
	{
		if( *ptr < *bptr )
			return TRUE;
		else if( *ptr > *bptr )
			return FALSE;
	}

	if( *ptr == '\0' )
		return TRUE;

	return FALSE;
}

MEDIT( medit_race )
{
	MOB_INDEX_DATA *pMob;
	int race,i;
	int lev, min, max;

	if ( argument[0] == '?' )
	{
		int min_race = 0;
		char *max_race_name = NULL, *min_race_name;

		send_to_char( "Dostepne rasy:", ch );

		i = 0;

		while( TRUE )
		{

			min_race = 0;
			min_race_name = NULL;

			for ( race = 0; race_table[race].name; race++ )
			{
				if( (!min_race_name || alph_order(race_table[race].name, min_race_name)) &&
						(!max_race_name || !alph_order(race_table[race].name, max_race_name)))
				{
					min_race_name = race_table[race].name;
					min_race = race;
				}
			}

			if( min_race_name != NULL &&
					(!max_race_name || !alph_order(min_race_name, max_race_name )))
			{
				max_race_name = min_race_name;

				if ( ( i % 3 ) == 0 )
					send_to_char( "\n\r", ch );

				print_char( ch, " %3d)%-18.18s", i, race_table[min_race].name );
				i++;
			}
			else
				break;

		}
		return FALSE;
	}

	if ( argument[0] != '\0' && argument[0] != '?')
	{
		if( is_number( argument ) )
		{
			int min_race = 0;
			char *max_race_name = NULL, *min_race_name;
			int race_arg;

			race_arg = atoi( argument );

			if( race_arg < 0 )
			{
				send_to_char( "Nie ma takiej rasy.\n\r", ch );
				return FALSE;
			}

			for( i = 0; race_table[i].name; i++ );

			if( i-1 < race_arg )
			{
				send_to_char( "Nie ma takiej rasy.\n\r", ch );
				return FALSE;
			}

			i = 0;

			while( TRUE )
			{
				min_race = 0;
				min_race_name = NULL;

				for ( race = 0; race_table[race].name; race++ )
				{
					if( (!min_race_name || alph_order(race_table[race].name, min_race_name)) &&
							(!max_race_name || !alph_order(race_table[race].name, max_race_name)))
					{
						min_race_name = race_table[race].name;
						min_race = race;
					}
				}

				if( min_race_name != NULL &&
						(!max_race_name || !alph_order(min_race_name, max_race_name )))
				{
					max_race_name = min_race_name;

					if( race_arg == i )
					{
						race = min_race;
						break;
					}
					i++;


				}
				else
				{
					send_to_char( "Nie ma takiej rasy.\n\r", ch );
					return FALSE;
				}
			}
		}
		else
		{
			if( (race = race_lookup( argument ) ) == 0 )
			{
				send_to_char( "Nie ma takiej rasy.\n\r", ch );
				return FALSE;
			}
		}
        /**
         * Nr:  15 Nazwa: smok
         * Nr: 124 Nazwa: jednoro¿ec
         */

        if (
                !IS_TRUSTED(ch,IMPLEMENTOR) &&
                (
                 race == 15 ||
                 race == 124
                )
           )
        {
            send_to_char( "Popros Lorda o ustawienie takiej rasy.\n\r", ch );
            return FALSE;
        }


		EDIT_MOB( ch, pMob );

		/* czyszczenie resists po ostatniej rasie */
		if(pMob->race >= 0)
		{
			for(i = 0; i < MAX_RESIST; i++)
				if( pMob->resists[i] == race_table[pMob->race].resists[i])
					pMob->resists[i] = 0;
		}

		ext_flags_copy( ext_flags_diff( pMob->act, ext_flag_value( act_flags, race_table[ pMob->race ].act ) ), pMob->act );
		ext_flags_copy( ext_flags_diff( pMob->off_flags, ext_flag_value( off_flags, race_table[ pMob->race ].off ) ), pMob->off_flags );
		pMob->form        &= ~race_table[pMob->race].form;
		pMob->parts       &= ~race_table[pMob->race].parts;

		pMob->race = race;

		ext_flags_copy( ext_flags_sum( pMob->act, ext_flag_value( act_flags, race_table[race].act ) ), pMob->act );
		ext_flags_copy( ext_flags_sum( pMob->off_flags, ext_flag_value( off_flags, race_table[race].off ) ), pMob->off_flags );
		pMob->form        |= race_table[race].form;
		pMob->parts       |= race_table[race].parts;
		pMob->skin_multiplier = race_table[race].skin_multi;

		for(i = 0; i < MAX_RESIST; i++)
			if( pMob->resists[i] == 0 && race_table[race].resists[i] != 0)
				pMob->resists[i] = race_table[race].resists[i];

		send_to_char( "Rasa ustawiona.\n\r", ch );

		if ( IS_SET( ch->wiz_config, WIZCFG_LIMITLEVEL ) )
		{
			min = race_table[pMob->race].min_level;
			max = race_table[pMob->race].max_level;
			lev = pMob->level;

			if ( max > 0 )
			{
				if ( pMob->level > max )
					lev = max;
				if ( pMob->level < min )
					lev = min;

				if ( pMob->level != lev )
				{
					pMob->level = lev;
					print_char( ch, "Level moba zmieniony na %d.\n\r", lev );
				}
			}
		}

		return TRUE;
	}

	if ( argument[0] == '?' )
	{
		char buf[MAX_STRING_LENGTH];

		send_to_char( "Dostepne rasy:", ch );

		for ( race = 0; race_table[race].name != NULL; race++ )
		{
			if ( ( race % 3 ) == 0 )
				send_to_char( "\n\r", ch );

			sprintf( buf, " %3d)%-15.15s", race, race_table[race].name );
			send_to_char( buf, ch );
		}

		send_to_char( "\n\r", ch );
		return FALSE;
	}


	send_to_char( "Sk³adnia: race [race]\n\r"
		      "Wpisz: 'race ?' ¿eby zobaczyæ dostêpne rasy.\n\r", ch );
	return FALSE;
}


MEDIT( medit_position )
{
	MOB_INDEX_DATA *pMob;
	char arg[MAX_INPUT_LENGTH];
	int value;

	argument = one_argument( argument, arg );

	switch ( arg[0] )
	{
		default:
			break;

		case 'S':
		case 's':
			if ( str_prefix( arg, "start" ) )
				break;

			if ( ( value = flag_value( position_flags, argument ) ) == NO_FLAG )
				break;

			EDIT_MOB( ch, pMob );

			pMob->start_pos = value;
			send_to_char( "Start position set.\n\r", ch );
			return TRUE;

		case 'D':
		case 'd':
			if ( str_prefix( arg, "default" ) )
				break;

			if ( ( value = flag_value( position_flags, argument ) ) == NO_FLAG )
				break;

			EDIT_MOB( ch, pMob );

			pMob->default_pos = value;
			send_to_char( "Pozycja w której bêdzie przebywa³ mob zosta³a zmieniona.\n\r", ch );
			return TRUE;
	}

	send_to_char( "Sk³adnia: position [start/default] [position]\n\r"
			"Wpisz: '? position' ¿eby zobaczyæ listê pozycji.\n\r", ch );
	return FALSE;
}


MEDIT( medit_gold )
{
	MOB_INDEX_DATA *pMob;
    long int wealth;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: wealth [number]\n\r", ch );
		return FALSE;
	}

    wealth = atoi( argument );

    if ( wealth > MAX_MOB_WEALTH )
    {
		print_char( ch, "Nie mozna ustawic wiecej niz: %d\n\r", MAX_MOB_WEALTH );
		return FALSE;
    }

	pMob->wealth = wealth;

	send_to_char( "Wealth set.\n\r", ch);
	return TRUE;
}

MEDIT( medit_hitroll )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: hitroll [number]\n\r", ch );
		return FALSE;
	}

	pMob->hitroll = atoi( argument );

	send_to_char( "Hitroll set.\n\r", ch);
	return TRUE;
}

void show_liqlist(CHAR_DATA *ch)
{
	int liq;
	BUFFER *buffer;
	char buf[MAX_STRING_LENGTH];

	buffer = new_buf();

	for ( liq = 0; liq_table[liq].liq_name != NULL; liq++)
	{
		if ( (liq % 21) == 0 ) {
			if ( liq > 0 )
				add_buf(buffer,"\n\r");
			add_buf(buffer,"Name                 Color              Proof   Full Thirst   Food   Size\n\r");
		}
		if ( (liq % 2) == 0 )
			add_buf(buffer, "{W");

		sprintf(buf, "%-19s %-19s %5d %6d %6d %6d %6d{x\n\r",
				liq_table[liq].liq_name,liq_table[liq].liq_color,
				liq_table[liq].liq_affect[0],liq_table[liq].liq_affect[1],
				liq_table[liq].liq_affect[2],liq_table[liq].liq_affect[3],
				liq_table[liq].liq_affect[4] );
		add_buf(buffer,buf);
	}

	page_to_char(buf_string(buffer),ch);
	free_buf(buffer);

	return;
}

void show_weedlist(CHAR_DATA *ch)
{
	int weed;
	BUFFER *buffer;
	char buf[MAX_STRING_LENGTH];

	buffer = new_buf();

	for ( weed = 0; weed_table[weed].weed_name != NULL; weed++)
	{
		if ( (weed % 21) == 0 ) {
			if ( weed > 0 )
				add_buf(buffer,"\n\r");
			add_buf(buffer,"Name                 Color          Moc/szansa halucynacji Poison\n\r");
		}
		if ( (weed % 2) == 0 )
			add_buf(buffer, "{W");

		sprintf(buf, "%-19s %-19s %5d %6d %6d{x\n\r",
				weed_table[weed].weed_name,weed_table[weed].weed_color,
				weed_table[weed].weed_affect[0],weed_table[weed].weed_affect[1],
				weed_table[weed].weed_affect[2] );
		add_buf(buffer,buf);
	}

	page_to_char(buf_string(buffer),ch);
	free_buf(buffer);

	return;
}

void show_damlist(CHAR_DATA *ch)
{
	int att;
	BUFFER *buffer;
	char buf[MAX_STRING_LENGTH];

	buffer = new_buf();

	for ( att = 0; attack_table[att].name != NULL; att++)
	{
		if ( (att % 21) == 0 )
			add_buf(buffer,"Name                 Noun\n\r");

		sprintf(buf, "%-20s %-20s\n\r",
				attack_table[att].name,attack_table[att].noun );
		add_buf(buffer,buf);
	}

	page_to_char(buf_string(buffer),ch);
	free_buf(buffer);

	return;
}

void show_armor_list(CHAR_DATA *ch)
{
	int i, cl;
	print_char( ch, "\n\rDostepne typy zbroi:\n\r" );
	print_char( ch, "NAZWA       [MIN AC] B P S E  CLASS CANT WEAR\n\r" );
	for ( i = 0; armor_table[ i ].name; i++ )
	{
		print_char( ch, "%-20s %d %d %d %2d   ",
				armor_table[ i ].name,
				armor_table[ i ].min_ac[ 0 ],
				armor_table[ i ].min_ac[ 1 ],
				armor_table[ i ].min_ac[ 2 ],
				armor_table[ i ].min_ac[ 3 ] );
		for ( cl = 0; cl < MAX_CLASS; cl++ )
			if ( !armor_table[ i ].can_wear[ cl ] )
				print_char( ch, "%s ", class_table[ cl ].who_name );
		send_to_char( "\n\r", ch );
	}
	return;
}

void show_sector_list( CHAR_DATA *ch )
{
	int i;
	for ( i = 0; sector_table[ i ].name != NULL; i++ ) {
		print_char(
		    ch,
		    "%-20.20s  Mv_loss:[%d]  Heal:[%3d%%]  Mem:[%2d]  Wait:[%d]\n\r",
		    sector_table[ i ].name, sector_table[ i ].mv_loss, sector_table[ i ].heal_bonus,
		    sector_table[ i ].mem_bonus, sector_table[ i ].wait
		);
	}
	return ;
}

void show_poison_list(CHAR_DATA *ch)
{
	int poison;
	print_char( ch, "\n\rlp. Name                  Full dam Saved Dam Duration Special\n\r" );
	for ( poison = 0; poison <= MAX_POISON; poison++ )
	{
		print_char( ch, "%d) %-23.23s %2dd%-2d+%-2d %2dd%-2d+%-2d ",
				poison,
				poison_table[ poison ].name,
				poison_table[ poison ].full_damage[ 0 ],
				poison_table[ poison ].full_damage[ 1 ],
				poison_table[ poison ].full_damage[ 2 ],
				poison_table[ poison ].saved_damage[ 0 ],
				poison_table[ poison ].saved_damage[ 1 ],
				poison_table[ poison ].saved_damage[ 2 ] );

		if ( poison_table[ poison ].duration[ 0 ] == 0 )
			send_to_char( "permanentna ", ch );
		else
			print_char( ch, "%2dd%-2d+%-2d ",
					poison_table[ poison ].duration[ 0 ],
					poison_table[ poison ].duration[ 1 ],
					poison_table[ poison ].duration[ 2 ] );
		if ( IS_SET( poison_table[ poison ].special_effects, POISON_DEATH ) )
			send_to_char( "zabija ", ch );
		if ( IS_SET( poison_table[ poison ].special_effects, POISON_SLEEP ) )
			send_to_char( "usypia ", ch );
		if ( IS_SET( poison_table[ poison ].special_effects, POISON_WEAKEN ) )
			send_to_char( "os³abia ", ch );
		if ( IS_SET( poison_table[ poison ].special_effects, POISON_PARALIZING ) )
			send_to_char( "parali¿uje ", ch );
		if ( IS_SET( poison_table[ poison ].special_effects, POISON_BLIND ) )
			send_to_char( "o¶lepia ", ch );
		send_to_char( "\n\r", ch );
	}
}

MEDIT( medit_group )
{
	MOB_INDEX_DATA *pMob;
	MOB_INDEX_DATA *pMTemp;
	char arg[MAX_STRING_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int temp;
	BUFFER *buffer;
	bool found = FALSE;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: group [number]\n\r", ch);
		send_to_char( "        group show [number]\n\r", ch);
		return FALSE;
	}

	if (is_number(argument))
	{
		pMob->group = atoi(argument);
		send_to_char( "Group set.\n\r", ch );
		return TRUE;
	}

	argument = one_argument( argument, arg );

	if ( !str_cmp( arg, "show" ) && is_number( argument ) )
	{
		if (atoi(argument) == 0)
		{
			send_to_char( "Are you crazy?\n\r", ch);
			return FALSE;
		}

		buffer = new_buf ();

		for (temp = 0; temp < 65536; temp++)
		{
			pMTemp = get_mob_index(temp);
			if ( pMTemp && ( pMTemp->group == atoi(argument) ) )
			{
				found = TRUE;
				sprintf( buf, "[%5d] %s\n\r", pMTemp->vnum, pMTemp->player_name );
				add_buf( buffer, buf );
			}
		}

		if (found)
			page_to_char( buf_string(buffer), ch );
		else
			send_to_char( "No mobs in that group.\n\r", ch );

		free_buf( buffer );
		return FALSE;
	}

	return FALSE;
}

MEDIT( medit_speaking )
{
	MOB_INDEX_DATA *pMob;
	int speaking;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || (speaking=get_langnum(argument)) < 0 ||
			! (pMob->languages & lang_table[speaking].bit))
	{
		send_to_char( "Sk³adnia: speaking [language known]\n\rWpisz: '? lang' aby wy¶wietliæ listê dostêpnych jêzyków.\n\r", ch );
		return FALSE;
	}

	pMob->speaking = speaking;

	send_to_char( "Speaking set.\n\r", ch);
	return TRUE;
}

MEDIT( medit_languages )
{
	MOB_INDEX_DATA *pMob;
	int speaking;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || (speaking=get_langnum(argument)) < 0 )
	{
		send_to_char( "Sk³adnia: language [language]\n\rWpisz: '? lang' aby wy¶wietliæ listê dostêpnych jêzyków.\n\r", ch );
		return FALSE;
	}

	pMob->languages ^= lang_table[speaking].bit;

	if ( !(lang_table[pMob->speaking].bit & pMob->languages ))
	{
		pMob->languages |= lang_table[pMob->speaking].bit;
		send_to_char( "Language cannot be removed.\n\r", ch);
		return TRUE;
	}


	send_to_char( "Language set.\n\r", ch);
	return TRUE;
}



MEDIT( medit_spell )
{
	MOB_INDEX_DATA *pMob;
	int i,last=-1;
	int type, spell, sn;
	bool ok=FALSE;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || (sn=skill_lookup(argument)) < 0 ||
			skill_table[sn].spell_fun == spell_null)
	{
		send_to_char( "Sk³adnia: spell [name of spell]\n\r", ch );
		return FALSE;
	}

	for( type = 0; type < 11; type++)
		for(spell=0; si_spell_info[type][spell].name; spell++)
			if(sn==si_spell_info[type][spell].sn)
				ok=TRUE;

	if(!ok)
	{
		int col=0;

		send_to_char( "To zaklecie jest zbyt kiepskie zeby mogly sie nim moby bawic.", ch );

		for( type = 0; type < 11; type++ )
		{
			send_to_char("\n\rZaklecia typu ", ch);
			switch(type)
			{
				case HEALS:
					send_to_char("'LECZENIA' :\n\r", ch);
					break;
				case FRAGS:
					send_to_char("'FRAGI' :\n\r", ch);
					break;
				case DEF_ANTITANK_SELF:
					send_to_char("'DEF ANTITANK SELF' :\n\r", ch);
					break;
				case DEF_ANTITANK_OTHER:
					send_to_char("'DEF ANTITANK OTHER' :\n\r", ch);
					break;
				case OFF_ANTITANK:
					send_to_char("'OFF ANTITANK' :\n\r", ch);
					break;
				case OFF_ANTICASTER:
					send_to_char("'OFF ANTICASTER' :\n\r", ch);
					break;
				case DEF_ANTIMAGIC_SELF:
					send_to_char("'DEF ANTIMAGIC SELF' :\n\r", ch);
					break;
				case DEF_ANTIMAGIC_OTHER:
					send_to_char("'DEF ANTIMAGIC OTHER' :\n\r", ch);
					break;
				case SAVE_LIFE:
					send_to_char("'SAVE LIFE' :\n\r", ch);
					break;
				case NASTY:
					send_to_char("'NASTY' :\n\r", ch);
					break;
				case CALLING:
					send_to_char("'CALLING' :\n\r", ch);
					break;
				default:
					send_to_char("'UNKNOWN' :\n\r", ch);
					break;
			}

			for ( spell = 0; si_spell_info[type][spell].name; spell++ )
			{
				print_char(ch, " %-25.25s", si_spell_info[type][spell].name);
				col++;

				if ( col > 2 && ( si_spell_info[type][spell+1].name ) )
				{
					col=0;
					send_to_char ( "\n\r", ch );
				}

			}

			send_to_char("\n\r", ch);
			col = 0;
		}

		print_char(ch,"\n\r{GJesli ktos chce zeby jakies jeszcze spelle byly,\n\r to spoko moge dodac.[resor]{x\n\r");
		return FALSE;
	}


	for( i=0; i < 16; i++ )
	{
		int x;

		if( pMob->spells[i] == 0 )
		{
			last = i;
			break;
		}


		if( pMob->spells[i] == sn )
		{
			for( x = i; x < 16; x++ )
				if( pMob->spells[x]==0 )
				{
					pMob->spells[i] = pMob->spells[x-1];
					pMob->spells[x-1] = 0;
					print_char( ch, "Usuniêto czar '%s'.\n\r", skill_table[sn].name );
					return TRUE;
				}
			pMob->spells[i] = pMob->spells[15];
			pMob->spells[15] = 0;
			print_char( ch, "Usuniêto czar '%s'.\n\r", skill_table[sn].name );
			return TRUE;
		}

	}

	if(last < 0)
	{
		send_to_char( "Mob nie mo¿e znaæ wiêkszej liczby zaklêæ (max 16).\n\r", ch );
		return FALSE;
	}

	pMob->spells[last]=sn;
	print_char( ch, "Dodano czar '%s'.\n\r", skill_table[sn].name );
	return TRUE;
}


MEDIT( medit_clone )
{
	MOB_INDEX_DATA *pMob, *pOMob;
	int value, i;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || !is_number( argument ) ||
			(value = atoi( argument )) < 0 || !( pOMob = get_mob_index( value ) ))
	{
		send_to_char( "Sk³adnia: clone [vnum of cloning mob]\n\r", ch );
		return FALSE;
	}

	if ( !IS_BUILDER( ch, pOMob->area ) )
	{
		send_to_char("Masz niewystarczaj±ce security ¿eby sklonowanowaæ tego moba.\n\r",ch);
		return FALSE;
	}

	free_string(pMob->player_name);
	pMob->player_name = str_dup(pOMob->player_name);

	free_string(pMob->material);
	pMob->material = str_dup(pOMob->material);

	free_string(pMob->name2);
	pMob->name2 = str_dup(pOMob->name2);
	free_string(pMob->name3);
	pMob->name3 = str_dup(pOMob->name3);
	free_string(pMob->name4);
	pMob->name4 = str_dup(pOMob->name4);
	free_string(pMob->name5);
	pMob->name5 = str_dup(pOMob->name5);
	free_string(pMob->name6);
	pMob->name6 = str_dup(pOMob->name6);
	free_string(pMob->short_descr);
	pMob->short_descr = str_dup(pOMob->short_descr);
	free_string(pMob->long_descr);
	pMob->long_descr = str_dup(pOMob->long_descr);
	free_string(pMob->description);
	pMob->description = str_dup(pOMob->description);

	ext_flags_copy( pOMob->act, pMob->act );

	for(i=0;i<MAX_VECT_BANK;i++)
		pMob->affected_by[i]=pOMob->affected_by[i];

	pMob->alignment=pOMob->alignment;
	pMob->level=pOMob->level;
	pMob->hitroll=pOMob->hitroll;

	for(i=0;i<3;i++)
	{
		pMob->hit[i]=pOMob->hit[i];
		pMob->damage[i]=pOMob->damage[i];
	}

	for(i=0;i<4;i++)
		pMob->ac[i]=pOMob->ac[i];

	for(i=0;i<7;i++)
		pMob->stats[i]=pOMob->stats[i];

	pMob->dam_type=pOMob->dam_type;
	ext_flags_copy( pOMob->off_flags, pMob->off_flags );
	pMob->start_pos=pOMob->start_pos;
	pMob->default_pos=pOMob->default_pos;
	pMob->sex=pOMob->sex;
	pMob->race=pOMob->race;
	pMob->wealth=pOMob->wealth;
	pMob->form=pOMob->form;
	pMob->parts=pOMob->parts;
	pMob->size=pOMob->size;
	pMob->languages=pOMob->languages;
	pMob->speaking=pOMob->speaking;

	for(i=0;i<16;i++)
		pMob->spells[i]=pOMob->spells[i];

	send_to_char("Ok.\n\r", ch);
	return TRUE;
}



REDIT( redit_owner )
{
	ROOM_INDEX_DATA *pRoom;

	EDIT_ROOM(ch, pRoom);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Sk³adnia: owner [owner]\n\r", ch );
		send_to_char( "         owner none\n\r", ch );
		return FALSE;
	}

	free_string( pRoom->owner );
	if (!str_cmp(argument, "none"))
		pRoom->owner = str_dup("");
	else
		pRoom->owner = str_dup( argument );

	send_to_char( "Owner set.\n\r", ch );
	return TRUE;
}

MEDIT ( medit_addmprog )
{
	BITVECT_DATA * value;
	MOB_INDEX_DATA *pMob;
	PROG_LIST *list;
	PROG_CODE *code;
	char trigger[MAX_STRING_LENGTH];
	char phrase[MAX_STRING_LENGTH];
	char name[MAX_STRING_LENGTH];

	EDIT_MOB(ch, pMob);
	argument=one_argument(argument, name);
	argument=one_argument(argument, trigger);
	argument=one_argument(argument, phrase);

	if ( name[0] == '\0' || trigger[0] =='\0' || phrase[0] =='\0' )
	{
		send_to_char("Sk³adnia:  addmprog [vnum] [trigger] [phrase]\n\r",ch);
		return FALSE;
	}

	value = ext_flag_lookup( trigger, prog_flags );
	if ( value == &EXT_NONE )
	{
		send_to_char("Valid flags are:\n\r",ch);
		show_help( ch, "trigger");
		return FALSE;
	}

	if ( !check_trigger( 0, trigger ) )
	{
		send_to_char( "Valid flags are:\n\r", ch );
		show_help( ch, "trigger" );
		return FALSE;
	}


	if ( ( code =get_mprog_index ( name ) ) == NULL)
	{
		send_to_char("No such MOBProgram.\n\r",ch);
		return FALSE;
	}

	list                  = new_prog();
	list->name            = code->name;
	list->trig_type       = value;
	list->trig_phrase     = str_dup(phrase);
	list->code            = code->code;
	list->valid_positions = mprog_trigger_default_pos(value);
	list->next            = pMob->progs;
	pMob->progs          = list;
	EXT_SET_BIT( pMob->prog_flags, *value );

	send_to_char( "Mprog Added.\n\r",ch);
	return TRUE;
}

MEDIT( medit_progpos )
{
	MOB_INDEX_DATA *pMob;
	PROG_LIST *list, *prog;
	char mprog[MAX_STRING_LENGTH];
	int value;
	int cnt = 0;

	EDIT_MOB(ch, pMob);

	one_argument( argument, mprog );

	if (!is_number( mprog ) || mprog[0] == '\0' || argument[0] == '\0' )
	{
		send_to_char("Sk³adnia: progpos [#mprog] <positions>\n\r",ch);
		show_help( ch, "trig-position" );
		return FALSE;
	}

	value = atoi ( mprog );

	if ( value < 0 )
	{
		send_to_char("Only non-negative mprog-numbers allowed.\n\r",ch);
		return FALSE;
	}

	if ( !pMob->progs )
	{
		send_to_char("Ten mob nie ma ¿adnych progów.\n\r",ch);
		return FALSE;
	}

	prog = NULL;
	for ( cnt = 0, list = pMob->progs; list; list = list->next, cnt++ )
		if ( cnt == value ) prog = list;

	if ( !prog )
	{
		send_to_char( "Ten mob nie posiada tylu progów.\n\r", ch );
		return FALSE;
	}


	if ( ( value = flag_value( position_trig_flags, argument ) ) != NO_FLAG )
	{
		prog->valid_positions ^= value;
		send_to_char( "Pozycje w których uruchomi siê prog ustawione.\n\r", ch);
		return TRUE;
	}

	send_to_char("Sk³adnia: progpos [#mprog] <positions>\n\r",ch);
	show_help( ch, "trig-position" );
	return FALSE;
}

MEDIT ( medit_delmprog )
{
	MOB_INDEX_DATA * pMob;
	PROG_LIST *prog = NULL, *prog_tmp = NULL, *prog_prev = NULL;
	char mprog[ MAX_STRING_LENGTH ];
	int value;
	int cnt = 0;
	int to_rem = 0;

	EDIT_MOB( ch, pMob );

	one_argument( argument, mprog );
	if ( !is_number( mprog ) || mprog[ 0 ] == '\0' )
	{
		send_to_char( "Sk³adnia: delprog [#mprog]\n\r", ch );
		return FALSE;
	}

	value = atoi ( mprog );

	if ( value < 0 )
	{
		send_to_char( "Only non-negative mprog-numbers allowed.\n\r", ch );
		return FALSE;
	}

	if ( !pMob->progs )
	{
		send_to_char( "MEdit: Mobprog nie istnieje.\n\r", ch );
		return FALSE;
	}

	if ( value == 0 )
	{
		for ( prog = pMob->progs; prog; prog = prog->next )
		{
			if ( prog->trig_type == pMob->progs->trig_type )
				to_rem++;
		}

		if ( to_rem <= 1 )
			EXT_REMOVE_BIT( pMob->prog_flags, *pMob->progs->trig_type );

		prog = pMob->progs;
		pMob->progs = prog->next;
		free_prog( prog );
	}
	else
	{
		for( prog = pMob->progs; cnt < value && prog; prog = prog->next, cnt++ )
			prog_prev = prog;

		if ( prog )
		{
			for ( prog_tmp = pMob->progs; prog_tmp; prog_tmp = prog_tmp->next )
				if ( prog_tmp->trig_type == prog->trig_type )
					to_rem++;

			if ( to_rem <= 1 )
				EXT_REMOVE_BIT( pMob->prog_flags, *prog->trig_type );

			prog_prev->next = prog->next;
			free_prog( prog );
		}
		else
		{
			send_to_char( "No such mprog.\n\r", ch );
			return FALSE;
		}
	}

	send_to_char( "Mprog removed.\n\r", ch );
	return TRUE;
}

MEDIT( medit_dammagic )
{
	MOB_INDEX_DATA *pMob;


	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: dammagic [liczba z zakresu 0..5]\n\r", ch );
		return FALSE;
	}

	pMob->magical_damage = URANGE(0, atoi( argument ), 5);
	print_char(ch,  "Umagicznienie ataku ustawione na:%d\n\r", pMob->magical_damage );
	return TRUE;
}

MEDIT( medit_damflags )
{
    MOB_INDEX_DATA *pMob;
    EDIT_MOB(ch, pMob);
    if (
            argument[0] != '\0' &&
            (
             !str_prefix(argument, "flaming")  ||
             !str_prefix(argument, "frost")    ||
             !str_prefix(argument, "vampiric") ||
             !str_prefix(argument, "sharp")    ||
             !str_prefix(argument, "vorpal")   ||
             !str_prefix(argument, "shock")    ||
             !str_prefix(argument, "poison")
            )
       )
    {
        ALT_FLAGVALUE_TOGGLE(pMob->attack_flags, weapon_type2, argument);
        send_to_char("Damflag set.\n\r", ch);
        return TRUE;
    }

    send_to_char
        (
         "Sk³adnia: damflags flaming\n\r"
         "          damflags frost\n\r"
         "          damflags vampiric\n\r"
         "          damflags sharp\n\r"
         "          damflags vorpal\n\r"
         "          damflags shocking\n\r"
         "          damflags poison\n\r",
         ch
        );
    return FALSE;
}

MEDIT( medit_damweapon )
{
	MOB_INDEX_DATA *pMob;


	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: damweapon [liczba wieksza badz rowna 0]\n\r", ch );
		send_to_char( "Wartosc damweapon jest dodawana do obrazen zadanych z broni.\n\r", ch);
		return FALSE;
	}

	pMob->weapon_damage_bonus = UMAX(0, atoi( argument ));
	print_char(ch,  "Bonus do obrazen z broni:%d\n\r", pMob->weapon_damage_bonus);
	return TRUE;
}

MEDIT( medit_addcomment )
{
	char buf1[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char time[MAX_STRING_LENGTH];
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' )
	{
		send_to_char( "Nie poda<&³e¶/³a¶/³e¶> komentarza.\n\r", ch );
		return FALSE;
	}

	buf1[0] = '\0';

	strcpy( time,  (char *) ctime( &current_time ) );
	time[ strlen( time ) - 1 ] = '\0';

	if ( !IS_NULLSTR( pMob->comments ) )
		strcpy( buf1, pMob->comments );
	smash_tilde( argument );
	sprintf( buf2, " - {R%s{x ({C%s{x)", capitalize(ch->name), time );
	strcat( buf1, argument );
	strcat( buf1, buf2 );
	strcat( buf1, "\n\r");

	free_string( pMob->comments);
	pMob->comments = str_dup( buf1 );

	send_to_char( "Komentarz dodany.\n\r", ch );

	return TRUE;
}

MEDIT( medit_delcomment )
{
	MOB_INDEX_DATA *pMob;

	EDIT_MOB(ch, pMob);

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Podaj numer komentarza do wykasowania.\n\r", ch );
		return FALSE;
	}

	if ( !IS_NULLSTR( pMob->comments ) )
		pMob->comments = string_linedel( pMob->comments, atoi(argument) );

	send_to_char( "Komentarz usuniêty.\n\r", ch );

	return TRUE;
}

REDIT( redit_room )
{
    ROOM_INDEX_DATA *room;
    BITVECT_DATA * value;
    if ( argument[ 0 ] != '\0' )
    {
        EDIT_ROOM( ch, room );
        value = ext_flag_lookup( argument, room_flags );
        if ( value != &EXT_NONE )
        {
            /**
             * ROOM_INN only for Lords
             */
            if ( value == &ROOM_INN && !( ch->pcdata->wiz_conf & W6 ) )
            {
                send_to_char( "{RNie masz wystarczaj±cych uprawnieñ, ¿eby stworzyæ karczmê, zg³o¶ siê do Lorda.{x\n\r", ch );
                return FALSE;
            }
            EXT_TOGGLE_BIT( room->room_flags, *value );
            send_to_char( "Room flags toggled.\n\r", ch );
            return TRUE;
        }
    }
    send_to_char( "Sk³adnia: room [flag]\n\rWpisz: '? room' ¿eby zobaczyæ listê dostêpnych flag.\n\r", ch );
    return FALSE;
}

REDIT( redit_trap )
{
	ROOM_INDEX_DATA *room;
	int trap;

	EDIT_ROOM( ch, room );

	if ( argument[0] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Sk³adnia: trap [number]\n\r", ch );
		return FALSE;
	}

	trap = UMAX( 0, atoi( argument ) );

	if ( trap > 0 && !get_trap_index( trap ) )
	{
		send_to_char( "{RPu³apka nie istnieje.{x\n\r", ch );
		return FALSE;
	}

	if ( trap > 0 )
		EXT_SET_BIT(room->room_flags, ROOM_TRAP );

	room->trap = trap;
	send_to_char( "Room trap set.\n\r", ch );
	return TRUE;
}

REDIT( redit_sector )
{
	ROOM_INDEX_DATA * room;
	int x, value = -1;

	EDIT_ROOM( ch, room );


	if ( argument[ 0 ] == '\0' || !str_cmp( argument, "help" ) )
	{
		show_sector_list( ch );
		return FALSE;
	}
	else
	{
		for ( x = 0; sector_table[ x ].name != NULL; x++ )
		{
			if ( NOPOL( argument[ 0 ] ) == NOPOL( sector_table[ x ].name[ 0 ] )
			     && !str_prefix( argument, sector_table[ x ].name ) )
			{
				value = x;
				break;
			}
		}
	}


	if ( value < 0 )
	{
		print_char( ch, "Nie ma takiego sektora, wpisz: sector.\n\r" );
		return FALSE;
	}

    /**
     * 30 - eden
     */

    if (
            !IS_TRUSTED(ch,IMPLEMENTOR) &&
            (
             value == 30 ||
             value == 124
            )
       )
    {
        send_to_char( "Popros Lorda o ustawienie takiego typu lokacji.\n\r", ch );
        return FALSE;
    }

	room->sector_type = value;
	send_to_char( "Sektor ustawiony.\n\r", ch );

	return TRUE;
}

int armor_lookup( char *name )
{
	int i;
	for ( i = 0; armor_table[ i ].name; i++ )
    {
		if ( NOPOL( name[ 0 ] ) == NOPOL( armor_table[ i ].name[ 0 ] ) && !str_prefix( name, armor_table[ i ].name ) )
        {
			return i;
        }
    }
	return -1;
}

char * armor_name( int armor )
{
	if ( armor < 16 )
		return armor_table[ armor ].name;
	return "NONE";
}

char *lang_bit_name( int flag )
{
	static char buf[ 512 ];
	int lang;
	buf[ 0 ] = '\0';
	for ( lang = 0;lang_table[ lang ].bit != LANG_UNKNOWN; lang++ )
		if ( flag & lang_table[ lang ].bit )
		{
			strcat( buf, " " );
			strcat( buf, lang_table[ lang ].name );
		}
	return ( buf[ 0 ] != '\0' ) ? buf + 1 : "none";
}

char * jewlery2hands (int type)
{
	switch ( type )
	{
		case  GLOVES_ON:
            return "noszona na";
            break;
		case  GLOVES_NONE:
            return "nie mo¿na nosiæ";
            break;
		case  GLOVES_UNDER:
        default:
             return "noszona pod";
             break;
	}
	return "noszona pod";
}

