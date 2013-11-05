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
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: olc.c 10996 2012-02-19 11:33:02Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/olc.c $
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
#include "olc.h"
#include "recycle.h"
#include "traps.h"
#include "projects.h"


/*
 * Local functions.
 */
AREA_DATA *get_area_data	args( ( ush_int vnum ) );
char *getline_olc args( ( char *str, char *buf ) );

/* Executed from comm.c. Minimizes compiling when changes are made. */
bool run_olc_editor( DESCRIPTOR_DATA *d )
{
	switch ( d->editor )
	{
		case ED_AREA:
			aedit( d->character, d->incomm );
			break;
		case ED_ROOM:
			redit( d->character, d->incomm );
			break;
		case ED_OBJECT:
			oedit( d->character, d->incomm );
			break;
		case ED_MOBILE:
			medit( d->character, d->incomm );
			break;
		case ED_MPCODE:
			mpedit( d->character, d->incomm );
			break;
		case ED_OPCODE:
			opedit( d->character, d->incomm );
			break;
		case ED_RPCODE:
			rpedit( d->character, d->incomm );
			break;
		case ED_HELP:
			hedit( d->character, d->incomm );
			break;
		case ED_TRAPDATA:
			tedit( d->character, d->incomm );
			break;
		case ED_RDESCDATA:
			rdedit( d->character, d->incomm );
			break;
		case ED_BSDATA:
			bsedit( d->character, d->incomm );
			break;
		case ED_SDATA:
			sedit( d->character, d->incomm );
			break;
		case ED_HBDATA:
			hbedit( d->character, d->incomm );
			break;
		default:
			return FALSE;
	}
	return TRUE;
}



char *olc_ed_name( CHAR_DATA *ch )
{
	static char buf[10];

	buf[0] = '\0';
	switch (ch->desc->editor)
	{
		case ED_AREA:
			sprintf( buf, "AEdit" );
			break;
		case ED_ROOM:
			sprintf( buf, "REdit" );
			break;
		case ED_OBJECT:
			sprintf( buf, "OEdit" );
			break;
		case ED_MOBILE:
			sprintf( buf, "MEdit" );
			break;
		case ED_MPCODE:
			sprintf( buf, "MPEdit" );
			break;
		case ED_OPCODE:
			sprintf( buf, "OPEdit" );
			break;
		case ED_RPCODE:
			sprintf( buf, "RPEdit" );
			break;
		case ED_HELP:
			sprintf( buf, "HEdit" );
			break;
		case ED_TRAPDATA:
			sprintf( buf, "TEdit" );
			break;
		case ED_RDESCDATA:
			sprintf( buf, "RDEdit" );
			break;
		case ED_BSDATA:
			sprintf( buf, "BSEdit" );
			break;
		case ED_SDATA:
			sprintf( buf, "SEdit" );
			break;
		case ED_HBDATA:
			sprintf( buf, "HBEdit" );
			break;
		default:
			sprintf( buf, " " );
			break;
	}
	return buf;
}



char *olc_ed_vnum( CHAR_DATA *ch )
{
	AREA_DATA *pArea;
	ROOM_INDEX_DATA *pRoom;
	OBJ_INDEX_DATA *pObj;
	MOB_INDEX_DATA *pMob;
	PROG_CODE *pMprog;
	PROG_CODE *pOprog;
	PROG_CODE *pRprog;
	HELP_DATA *pHelp;
	TRAP_DATA *pTrap;
	RAND_DESC_DATA *pDesc;
	BONUS_INDEX_DATA *pBonus;
	static char buf[MIL];

	buf[0] = '\0';
	switch ( ch->desc->editor )
	{
		case ED_AREA:
			pArea = (AREA_DATA *)ch->desc->pEdit;
			sprintf( buf, "%d", pArea ? pArea->vnum : 0 );
			break;
		case ED_ROOM:
			pRoom = ch->in_room;
			sprintf( buf, "%d", pRoom ? pRoom->vnum : 0 );
			break;
		case ED_OBJECT:
			pObj = (OBJ_INDEX_DATA *)ch->desc->pEdit;
			sprintf( buf, "%d", pObj ? pObj->vnum : 0 );
			break;
		case ED_MOBILE:
			pMob = (MOB_INDEX_DATA *)ch->desc->pEdit;
			sprintf( buf, "%d", pMob ? pMob->vnum : 0 );
			break;
		case ED_MPCODE:
			pMprog = (PROG_CODE *)ch->desc->pEdit;
			sprintf( buf, "%s", pMprog ? pMprog->name : 0 );
			break;
		case ED_OPCODE:
			pOprog = (PROG_CODE *)ch->desc->pEdit;
			sprintf( buf, "%s", pOprog ? pOprog->name : 0 );
			break;
		case ED_RPCODE:
			pRprog = (PROG_CODE *)ch->desc->pEdit;
			sprintf( buf, "%s", pRprog ? pRprog->name : 0 );
			break;
		case ED_HELP:
			pHelp = (HELP_DATA *)ch->desc->pEdit;
			sprintf( buf, "%s", pHelp ? pHelp->keyword : "" );
			break;
		case ED_TRAPDATA:
			pTrap = (TRAP_DATA *)ch->desc->pEdit;
			sprintf( buf, "%d", pTrap ? pTrap->vnum : 0 );
			break;
		case ED_RDESCDATA:
			pDesc = (RAND_DESC_DATA *)ch->desc->pEdit;
			sprintf( buf, "%d", pDesc ? pDesc->vnum : 0 );
			break;
		case ED_BSDATA:
			pBonus = (BONUS_INDEX_DATA *)ch->desc->pEdit;
			sprintf( buf, "%d", pBonus ? pBonus->vnum : 0 );
			break;
		default:
			sprintf( buf, " " );
			break;
	}

	return buf;
}



/*****************************************************************************
Name:		show_olc_cmds
Purpose:	Format up the commands from given table.
Called by:	show_commands(olc_act.c).
 ****************************************************************************/
void show_olc_cmds( CHAR_DATA *ch, const struct olc_cmd_type *olc_table )
{
	char buf [ MAX_STRING_LENGTH ];
	char buf1 [ MAX_STRING_LENGTH ];
	int cmd;
	int col;

	buf1[0] = '\0';
	col = 0;
	for (cmd = 0; olc_table[cmd].name != NULL; cmd++)
	{
		sprintf( buf, "%-15.15s", olc_table[cmd].name );
		strcat( buf1, buf );
		if ( ++col % 5 == 0 )
			strcat( buf1, "\n\r" );
	}

	if ( col % 5 != 0 )
		strcat( buf1, "\n\r" );

	send_to_char( buf1, ch );
	return;
}



/*****************************************************************************
Name:		show_commands
Purpose:	Display all olc commands.
Called by:	olc interpreters.
 ****************************************************************************/
bool show_commands( CHAR_DATA *ch, char *argument )
{
	switch (ch->desc->editor)
	{
		case ED_AREA:
			show_olc_cmds( ch, aedit_table );
			break;
		case ED_ROOM:
			show_olc_cmds( ch, redit_table );
			break;
		case ED_OBJECT:
			show_olc_cmds( ch, oedit_table );
			break;
		case ED_MOBILE:
			show_olc_cmds( ch, medit_table );
			break;
		case ED_MPCODE:
			show_olc_cmds( ch, mpedit_table );
			break;
		case ED_OPCODE:
			show_olc_cmds( ch, opedit_table );
			break;
		case ED_RPCODE:
			show_olc_cmds( ch, rpedit_table );
			break;
		case ED_HELP:
			show_olc_cmds( ch, hedit_table );
			break;
		case ED_TRAPDATA:
			show_olc_cmds( ch, tedit_table );
			break;
		case ED_RDESCDATA:
			show_olc_cmds( ch, rdedit_table );
			break;
		case ED_BSDATA:
			show_olc_cmds( ch, bsedit_table );
			break;
		case ED_SDATA:
			show_olc_cmds( ch, sedit_table );
			break;
		case ED_HBDATA:
			show_olc_cmds( ch, hbedit_table );
			break;
	}

	return FALSE;
}



/*****************************************************************************
 *                           Interpreter Tables.                             *
 *****************************************************************************/
const struct olc_cmd_type aedit_table[] =
{
	/*  {   command		function	}, */

	{	"age",			aedit_age		},
	{	"builder",		aedit_builder	},
	{	"commands",		show_commands	},
	{	"create",		aedit_create	},
	{	"filename",		aedit_file		},
	{	"name",			aedit_name		},
	{	"reset",		aedit_reset		},
	{	"resetage", 	aedit_resetage	},
	{	"security",		aedit_security	},
	{	"show",			aedit_show		},
	{	"vnum",			aedit_vnum		},
	{	"lvnum",		aedit_lvnum		},
	{	"uvnum",		aedit_uvnum		},
	{	"credits",		aedit_credits	},
	{	"region",		aedit_region	},
	{	"locked",		aedit_lock		},
	{	"?",			show_help		},
	{	"version",		show_version	},
	{	NULL,			0,				}
};



const struct olc_cmd_type redit_table[] =
{
	/*  {   command		function	}, */

	{	"commands",		show_commands		},
	{	"create",		redit_create		},
	{	"desc",			redit_desc			},
	{	"nightdesc",	redit_nightdesc		},
	{	"ed",			redit_ed			},
	{	"format",		redit_format		},
	{	"name",			redit_name			},
	{	"show",			redit_show			},
	{	"rent",			redit_rent			},
	{	"heal",			redit_heal			},
	{	"clan",			redit_clan			},
	{	"north",		redit_north			},
	{	"south",		redit_south			},
	{	"east",			redit_east			},
	{	"west",			redit_west			},
	{	"up",			redit_up			},
	{	"down",			redit_down			},
	{	"mreset",		redit_mreset		},
	{	"oreset",		redit_oreset		},
	{	"mshow",		redit_mshow			},
	{	"oshow",		redit_oshow			},
	{	"owner",		redit_owner			},
	{	"room",			redit_room			},
	{	"trap",			redit_trap			},
	{	"sector",		redit_sector		},
	{	"addprog",		redit_addprog		},
	{	"delprog",		redit_delprog		},
	{	"?",			show_help			},
	{	"version",		show_version		},
	{	"links",		redit_links			},
	{	"progs",		redit_progs			},
	{	"randday",		redit_randday		},
	{	"randnight",	redit_randnight		},
	{	"forroom",		redit_forroom		},
	{	"echoto",		redit_echoto		},
	{	"areapart",		redit_areapart		},
	{ "resource", redit_resource }, //rellik: mining
	{ "capacity", redit_capacity }, //rellik: mining
	{	NULL,			0,					}
};

const struct olc_cmd_type oedit_table[] =
{
	/*  {   command		function	}, */

	{	"addaffect",	oedit_addaffect		},
	{	"addprog",		oedit_addoprog		},
	{	"delprog",		oedit_deloprog		},
	{	"addapply",		oedit_addapply		},
	{	"commands",		show_commands		},
	{	"cost",			oedit_cost			},
	{	"create",		oedit_create		},
	{	"delaffect",	oedit_delaffect		},
	{	"ed",			oedit_ed			},
	{	"long",			oedit_long			},
	{	"name",			oedit_name			},
	{	"short",		oedit_short			},
	{	"show",			oedit_show			},
	{	"v0",			oedit_value0		},
	{	"v1",			oedit_value1		},
	{	"v2",			oedit_value2		},
	{	"v3",			oedit_value3		},
	{	"v4",			oedit_value4		},
	{	"v5",			oedit_value5		},
	{	"v6",			oedit_value6		},
	{	"weight",		oedit_weight		},
	{	"extra",		oedit_extra			},
	{	"exwear",		oedit_exwear		},
	{	"wear",			oedit_wear			},
	{	"type",			oedit_type			},
	{	"material",		oedit_material		},
	{	"level",		oedit_level			},
	{	"condition",	oedit_condition		},
	{	"odmiana",		oedit_odmiana		},
	{	"clone",		oedit_clone			},
	{	"trap",			oedit_trap			},
	{	"repair",		oedit_repair		}, // repair limit

	{	"bonus",		oedit_bonus_set		},
	{	"desc",			oedit_desc			},
	{	"ident",		oedit_ident			},
	{	"hidden",		oedit_hidden		},
	{	"addcomment",	oedit_addcomment	},
	{	"delcomment",	oedit_delcomment	},
	{	"specdam",		oedit_specdam		},
	{	"liczba",		oedit_liczba_mnoga	},
	{	"rodzaj",		oedit_gender		},
	{ "komponent", oedit_item_spell }, //rellik: komponenty, defniowanie w³a¶ciwo¶ci komponentu
	{	"?",			show_help			},
	{	"version",	show_version			},
	{	NULL,	0,	}
};



const struct olc_cmd_type medit_table[] =
{
	/*  {   command		        function	        }, */

	{   "alignment",        medit_align         },
	{   "commands",         show_commands       },
	{   "create",           medit_create        },
	{   "corpse",           medit_corpse        },
	{   "desc",             medit_desc          },
	{   "level",            medit_level         },
	{   "long",             medit_long          },
	{   "name",             medit_name          },
	{   "shop",             medit_shop          },
	{   "repair",           medit_repair        },
	{   "bank",             medit_bank          },
	{   "short",            medit_short         },
	{   "show",             medit_show          },
	{   "spec",             medit_spec          },
	{   "sex",              medit_sex           },
	{   "act",              medit_act           },
	{   "affect",           medit_affect        },
	{   "armor",            medit_ac            },
	{   "stats",            medit_stats         },
	{   "odmiana",          medit_odmiana       },
	{   "form",             medit_form          },
	{   "part",             medit_part          },
	{   "resist",           medit_res           },
	{   "hfrom",		medit_heal_from     },
	{   "material",         medit_material      },
	{   "off",              medit_off           },
	{   "size",             medit_size          },
	{   "hitdice",          medit_hitdice       },
	{   "damdice",          medit_damdice       },
	{   "damweapon",        medit_damweapon     },
	{   "dammagic",         medit_dammagic      },
	{   "damflags",         medit_damflags      },
	{   "race",             medit_race          },
	{   "position",         medit_position      },
	{   "wealth",           medit_gold          },
	{   "hitroll",          medit_hitroll       },
	{   "damtype",          medit_damtype       },
	{   "group",            medit_group         },
	{   "addmprog",         medit_addmprog      },
	{   "delmprog",         medit_delmprog      },
	{   "languages",        medit_languages     },
	{   "speaking",         medit_speaking      },
	{   "spell",            medit_spell         },
	{   "clone",            medit_clone         },
	{   "progpos",          medit_progpos	    },
	{   "addcomment",       medit_addcomment    },
	{   "delcomment",       medit_delcomment    },
	{   "progpos",          medit_progpos       },
	{   "hpcalc",           medit_hpcalc        },
	{   "?",                show_help           },
	{   "version",          show_version        },
	{   "skin",				medit_skin	        },
	{   "exp",				medit_exp_multiplier	        },
	{   NULL,               0,                  }
};



/*****************************************************************************
 *                          End Interpreter Tables.                          *
 *****************************************************************************/



/*****************************************************************************
Name:		get_area_data
Purpose:	Returns pointer to area with given vnum.
Called by:	do_aedit(olc.c).
 ****************************************************************************/
AREA_DATA *get_area_data( ush_int vnum )
{
	AREA_DATA *pArea;

	for (pArea = area_first; pArea; pArea = pArea->next )
	{
		if (pArea->vnum == vnum)
			return pArea;
	}

	return 0;
}



/*****************************************************************************
Name:		edit_done
Purpose:	Resets builder information on completion.
Called by:	aedit, redit, oedit, medit(olc.c)
 ****************************************************************************/
bool edit_done( CHAR_DATA *ch )
{
	ch->desc->pEdit = NULL;
	ch->desc->editor = 0;
	return FALSE;
}



/*****************************************************************************
 *                              Interpreters.                                *
 *****************************************************************************/


/* Area Interpreter, called by do_aedit. */
void aedit( CHAR_DATA *ch, char *argument )
{
	AREA_DATA *pArea;
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	int  cmd;
	int  value;

	EDIT_AREA(ch, pArea);
	smash_tilde( argument );
	strcpy( arg, argument );
	argument = one_argument( argument, command );

	if ( !IS_BUILDER( ch, pArea ) )
	{
		send_to_char( "AEdit:  Niewystarczajace security zeby modyfikowac kraine.\n\r", ch );
		edit_done( ch );
		return;
	}

	if ( !str_cmp(command, "done") )
	{
		edit_done( ch );
		return;
	}

	if ( command[0] == '\0' )
	{
		aedit_show( ch, argument );
		return;
	}

	if ( ( value = flag_value( area_flags, command ) ) != NO_FLAG )
	{
		TOGGLE_BIT(pArea->area_flags, value);

		send_to_char( "Flag toggled.\n\r", ch );
		return;
	}

	/* Search Table and Dispatch Command. */
	for ( cmd = 0; aedit_table[cmd].name != NULL; cmd++ )
	{
		if ( !str_prefix( command, aedit_table[cmd].name ) )
		{
			if ( (*aedit_table[cmd].olc_fun) ( ch, argument ) )
			{
				SET_BIT( pArea->area_flags, AREA_CHANGED );
				append_file_format_daily( ch, BUILD_LOG_FILE, "-> aedit %5d: %s", pArea ? pArea->vnum : 0, arg );
			}
			return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret( ch, arg );
	return;
}



/* Room Interpreter, called by do_redit. */
void redit( CHAR_DATA *ch, char *argument )
{
	AREA_DATA *pArea;
	ROOM_INDEX_DATA *pRoom;
	char arg[MAX_STRING_LENGTH];
	char command[MAX_INPUT_LENGTH];
	int  cmd;

	EDIT_ROOM(ch, pRoom);
	pArea = pRoom->area;

	smash_tilde( argument );
	strcpy( arg, argument );
	argument = one_argument( argument, command );

	if ( !IS_BUILDER( ch, pArea ) )
	{
		send_to_char( "REdit: Niewystarczajace security zeby modyfikowac room.\n\r", ch );
		edit_done( ch );
		return;
	}

	if ( !str_cmp(command, "done") )
	{
		edit_done( ch );
		return;
	}

	if ( command[0] == '\0' )
	{
		redit_show( ch, argument );
		return;
	}

	/* Search Table and Dispatch Command. */
	for ( cmd = 0; redit_table[cmd].name != NULL; cmd++ )
	{
		if ( !str_prefix( command, redit_table[cmd].name ) )
		{
			if ( (*redit_table[cmd].olc_fun) ( ch, argument ) )
			{
				SET_BIT( pArea->area_flags, AREA_CHANGED );
				append_file_format_daily( ch, BUILD_LOG_FILE, "-> redit %5d: %s", pRoom ? pRoom->vnum : 0, arg );
			}
			return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret( ch, arg );
	return;
}

/* Object Interpreter, called by do_oedit. */
void oedit( CHAR_DATA *ch, char *argument )
{
	AREA_DATA *pArea;
	OBJ_INDEX_DATA *pObj;
	char arg[MAX_STRING_LENGTH];
	char command[MAX_INPUT_LENGTH];
	int  cmd;

	smash_tilde( argument );
	strcpy( arg, argument );
	argument = one_argument( argument, command );

	EDIT_OBJ(ch, pObj);
	pArea = pObj->area;

	if ( !IS_BUILDER( ch, pArea ) )
	{
		send_to_char( "OEdit: Niewystarczajace security zeby modyfikowac obiekt.\n\r", ch );
		edit_done( ch );
		return;
	}

	if ( !str_cmp(command, "done") )
	{
		edit_done( ch );
		return;
	}

	if ( command[0] == '\0' )
	{
		oedit_show( ch, argument );
		return;
	}

	/* Search Table and Dispatch Command. */
	for ( cmd = 0; oedit_table[cmd].name != NULL; cmd++ )
	{
		if ( !str_prefix( command, oedit_table[cmd].name ) )
		{
			if ( (*oedit_table[cmd].olc_fun) ( ch, argument ) )
			{
				SET_BIT( pArea->area_flags, AREA_CHANGED );
				append_file_format_daily( ch, BUILD_LOG_FILE, "-> oedit %5d: %s", pObj ? pObj->vnum : 0, arg );
			}
			return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret( ch, arg );
	return;
}



/* Mobile Interpreter, called by do_medit. */
void medit( CHAR_DATA *ch, char *argument )
{
	AREA_DATA *pArea;
	MOB_INDEX_DATA *pMob;
	char command[MAX_INPUT_LENGTH];
	char arg[MAX_STRING_LENGTH];
	int  cmd;

	smash_tilde( argument );
	strcpy( arg, argument );
	argument = one_argument( argument, command );

	EDIT_MOB(ch, pMob);
	pArea = pMob->area;

	if ( !IS_BUILDER( ch, pArea ) )
	{
		send_to_char( "MEdit: Niewystarczajace security zeby modyfikowac kraine.\n\r", ch );
		edit_done( ch );
		return;
	}

	if ( !str_cmp(command, "done") )
	{
		edit_done( ch );
		return;
	}

	if ( command[0] == '\0' )
	{
		medit_show( ch, argument );
		return;
	}

	/* Search Table and Dispatch Command. */
	for ( cmd = 0; medit_table[cmd].name != NULL; cmd++ )
	{
		if ( !str_prefix( command, medit_table[cmd].name ) )
		{
			if ( (*medit_table[cmd].olc_fun) ( ch, argument ) )
			{
				SET_BIT( pArea->area_flags, AREA_CHANGED );
				append_file_format_daily( ch, BUILD_LOG_FILE, "-> medit %5d: %s", pMob ? pMob->vnum : 0, arg );
			}
			return;
		}
	}

	/* Default to Standard Interpreter. */
	interpret( ch, arg );
	return;
}

const struct editor_cmd_type editor_table[] =
{
	/*  {   command		function	}, */

	{   "area",		do_aedit	},
	{   "room",		do_redit	},
	{   "object",	do_oedit	},
	{   "mobile",	do_medit	},
	{	"mpcode",	do_mpedit	},
	{	"opcode",	do_opedit	},
	{	"rpcode",	do_rpedit	},
	{	"hedit",	do_hedit	},
	{	"tedit",	do_tedit	},
	{	"bsedit",	do_bsedit	},
	{	"sedit",	do_sedit	},
	{	"hbedit",	do_hbedit	},
	{	NULL,		0,		}
};

/* Entry point for all editors. */
void do_olc( CHAR_DATA *ch, char *argument )
{
	char command[MAX_INPUT_LENGTH];
	int  cmd;

	if ( IS_NPC(ch) )
		return;

	argument = one_argument( argument, command );

	if ( command[0] == '\0' )
	{
		do_help( ch, "olc" );
		return;
	}

	/* Search Table and Dispatch Command. */
	for ( cmd = 0; editor_table[cmd].name != NULL; cmd++ )
	{
		if ( !str_prefix( command, editor_table[cmd].name ) )
		{
			(*editor_table[cmd].do_fun) ( ch, argument );
			return;
		}
	}

	/* Invalid command, send help. */
	do_help( ch, "olc" );
	return;
}



/* Entry point for editing area_data. */
void do_aedit( CHAR_DATA *ch, char *argument )
{
	AREA_DATA *pArea;
	int value;
	char arg[MAX_STRING_LENGTH];

	if ( IS_NPC(ch) )
		return;

	pArea	= ch->in_room->area;

	argument	= one_argument(argument,arg);

	if ( is_number( arg ) )
	{
		value = atoi( arg );
		if ( !( pArea = get_area_data( value ) ) )
		{
			send_to_char( "AEdit: Nie istnieje kraina o takim vnumie.\n\r", ch );
			return;
		}
	}
	else
		if ( !str_cmp( arg, "create" ) )
		{
			if ( ch->pcdata->security < 9 )
			{
				send_to_char( "AEdit: Niewystarczaj±ce security ¿eby tworzyæ krainê.\n\r", ch );
				return;
			}

			aedit_create( ch, "" );
			ch->desc->editor = ED_AREA;
			return;
		}

	if (!IS_BUILDER(ch,pArea))
	{
		send_to_char( "AEdit: Niewystarczaj±ce security ¿eby modyfikowaæ krainê.\n\r",ch);
		return;
	}

	ch->desc->pEdit = (void *)pArea;
	ch->desc->editor = ED_AREA;
	return;
}



/* Entry point for editing room_index_data. */
void do_redit( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *pRoom;
	char arg1[MAX_STRING_LENGTH];

	if ( IS_NPC(ch) )
		return;

	argument = one_argument( argument, arg1 );

	pRoom = ch->in_room;

	if ( !str_cmp( arg1, "reset" ) )	/* redit reset */
	{
		if ( !IS_BUILDER( ch, pRoom->area ) )
		{
			send_to_char( "REdit: Niewystarczaj±ce security zeby modyfikowaæ room.\n\r",ch);
			return;
		}

		/*artefact*/
		if(is_artefact_load_room(pRoom) == 1) load_artefact(pRoom,NULL,NULL);
		reset_room( pRoom );
		send_to_char( "Room reset.\n\r", ch );

		return;
	}
	else
		if ( !str_cmp( arg1, "create" ) )	/* redit create <vnum> */
		{
			if ( argument[0] == '\0' || atoi( argument ) == 0 )
			{
				send_to_char( "REdit: Sk³adnia: redit create [vnum]\n\r", ch );
				return;
			}

			if ( redit_create( ch, argument ) ) /* pEdit == nuevo cuarto */
			{
				ch->desc->editor = ED_ROOM;
				char_from_room( ch );
				char_to_room( ch, ch->desc->pEdit );
				SET_BIT( ((ROOM_INDEX_DATA *)ch->desc->pEdit)->area->area_flags, AREA_CHANGED );
			}

			return;
		}
		else if ( !IS_NULLSTR(arg1) )	/* redit <vnum> */
		{
			pRoom = get_room_index(atoi(arg1));

			if ( !pRoom )
			{
				send_to_char( "REdit: Argument nie istnieje.\n\r", ch );
				return;
			}

			if ( !IS_BUILDER(ch, pRoom->area) )
			{
				send_to_char( "REdit: Niewystarczaj±ce security zeby modyfikowaæ room.\n\r",ch);
				return;
			}

			char_from_room( ch );
			char_to_room( ch, pRoom );
		}

	if ( !IS_BUILDER(ch, pRoom->area) )
	{
		send_to_char( "REdit: Niewystarczaj±ce security zeby modyfikowaæ room.\n\r",ch);
		return;
	}

	ch->desc->pEdit	= (void *) pRoom;
	ch->desc->editor	= ED_ROOM;

	return;
}

//rellik: mining, wypisuje roomy które maj± capacity
void	capacity_uzywane( CHAR_DATA *ch, int v )
{
	if(!IS_SET( ch->pcdata->wiz_conf, W4))
	{
		print_char( ch, "Capacity mo¿e zmieniaæ stra¿nik, sêdzia i lord");
		return;
	}
	const char *call = "olc_law.c => capacity_uzywane";
	save_debug_info( call, NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );
	int i, col;
	ROOM_INDEX_DATA *room;

	print_char( ch, "Vnumy roomów z zasobno¶ci± powy¿ej %d: \n\r", v );
	col = 0;

	for ( i = 0; i <= top_vnum_room; i++ )
	{
		if ( ( room = get_room_index( i ) ) )
		{
			if( room->rawmaterial_capacity > v )
			{
				print_char(ch, "{G%-8d{x = {w%-6d{x", room->vnum, room->rawmaterial_capacity );
				if ( ++col % 4 == 0 ) print_char( ch, "\n\r" );
			}
		}
	}
}

//rellik: mining, wypisuje roomy w których wystêpuje dany minera³
void	resource_uzywane( CHAR_DATA *ch, int r )
{

	if(!IS_SET( ch->pcdata->wiz_conf, W4))
	{
		print_char( ch, "Resource mo¿e zmieniaæ stra¿nik, sêdzia i lord");
		return;
	}
	const char *call = "olc_law.c => resource_uzywane";
	save_debug_info( call, NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );
	int i, col;
	ROOM_INDEX_DATA *room;

	print_char( ch, "Vnumy roomów z surowcem: %s\n\r", rawmaterial_table[r].name );
	col = 0;

	for ( i = 0; i <= top_vnum_room; i++ )
	{
		if ( ( room = get_room_index( i ) ) )
		{
			if( room->rawmaterial[r] > 0 )
			{
				print_char(ch, "{G%-8d{x = {w%-6d{x", room->vnum, room->rawmaterial[r] );
				if ( ++col % 4 == 0 ) print_char( ch, "\n\r" );
			}
		}
	}
}

//rellik: mining, ustawianie surowców i pojemno¶ci dla zakresu roomów
void do_rresources( CHAR_DATA *ch, char *argument )
{
	if(!IS_SET( ch->pcdata->wiz_conf, W4))
	{
		print_char( ch, "Resource mo¿e zmieniaæ stra¿nik, sêdzia i lord");
		return;
	}

	ROOM_INDEX_DATA *pRoom;
	char arg1[MAX_STRING_LENGTH]; //resource/capacity
	char arg2[MAX_STRING_LENGTH]; //vnum_min
	char arg3[MAX_STRING_LENGTH]; //vnum_max
	char arg4[MAX_STRING_LENGTH]; //value
	int vmin, vmax, val, i, rsc;

	const char *call = "olc.c => do_rresources";
	save_debug_info( call, NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );


	if ( IS_NPC(ch) )
		return;

	argument = one_argument( argument, arg1 ); //resource {wegiel, zelazo ... } /capacity
	argument = one_argument( argument, arg2 ); //vnum_min
	argument = one_argument( argument, arg3 ); //vnum_max
	one_argument( argument, arg4 ); //value

	if ( !str_cmp( arg1, "help" ) || !str_cmp( arg1, "?" ) || arg1[0] == '\0' )
	{
		//tutaj wypisywaæ sk³adniê tego polecenia
		send_to_char( "U¿ycie: rresources <warto¶æ>\n\r", ch );
		send_to_char( "		Wypisuje room'y które maj± ustawione pojemno¶ci minera³ów wiêksze ni¿ <warto¶æ>.\n\r", ch );
		send_to_char( "U¿ycie: rresources: [nazwa minera³u] <vnum_min> <vnum_max> <warto¶æ>.\n\r", ch );
		send_to_char( "		Ustawia wystêpowanie minera³u dla zakresu <vnum_min> do <vnum_max>.\n\r", ch );
		send_to_char( "U¿ycie: rresources capacity <vnum_min> <vnum_max> <warto¶æ>.\n\r", ch );
		send_to_char( "		Ustawia zasobno¶æ w surowce roomy od <vnum_min> do <vnum_max>.\n\r", ch );
		send_to_char( "U¿ycie: rresources <nazwa_minera³u>.\n\r", ch );
		send_to_char( "		Wyswietla roomy które zawieraj± surowiec <name>.\n\r", ch );
		return;
	}

	if ( is_number( arg1) )
	{
		val = atoi( arg1 );
		if ( val < 0 ) val = 0;
		if ( val > 100000 ) val = 100000;
		capacity_uzywane( ch, val ); //wypisuje roomy które maj± ustawione capacity > val
		return;
	}


	if ( ( rsc = rawmaterial_lookup( arg1 ) ) == -1 && str_prefix( arg1, "capacity" ) )
	{
		print_char( ch, "Minera³ nie znaleziony.\n\r" );
		return;
	}

	if ( arg2[0] == '\0' )
	{
		resource_uzywane( ch, rsc ); //wypisuje roomy które maj± ustawiony dany surowiec
		return;
	}

	if ( is_number( arg2 ) )
	{
		vmin = atoi( arg2 );
	} else {
		send_to_char( "U¿ycie: musisz podaæ vnum_min.\n\r", ch );
		return;
	}

	if ( is_number( arg3 ) )
	{
		vmax = atoi( arg3 );
	} else {
		send_to_char( "U¿ycie: musisz podaæ vnum_max.\n\r", ch );
		return;
	}

	if ( is_number( arg4 ) )
	{
		val = atoi( arg4 );
		if ( str_prefix( arg1, "capacity" ) )
		{
			if ( val < 0 || val > 100 )
			{
				print_char( ch, "B³±d: Warto¶æ musi byæ z zakresu 0 do 100.\n\r" );
				return;
			}
		} else {
			if ( val < 0 ) val = 0;
		}
	} else {
		send_to_char( "U¿ycie: musisz podaæ warto¶æ.\n\r", ch );
		return;
	}

	for( i = vmin; i <= vmax; i++ )
	{
		pRoom = get_room_index(i);
		if ( !pRoom )
		{
			print_char( ch, "Brak lokacji o vnum = %d.\n\r", i );
			if ( i != vmin ) print_char( ch, "Lokacje o vnum od %d do %d zosta³y zmienione.\n\r", vmin, i - 1 );
			return;
		}
		if ( !IS_BUILDER(ch, pRoom->area) )
		{
			print_char( ch, "Niewystarczaj±ce security zeby modyfikowaæ lokacjê vnum = %d.\n\r", i);
			if ( i != vmin ) print_char( ch, "Lokacje o vnum od %d do %d zosta³y zmienione.\n\r", vmin, i - 1 );
			return;
		}
		if ( !str_prefix( arg1, "capacity" ) )
		{
			pRoom->rawmaterial_capacity = val;
			pRoom->rawmaterial_capacity_now = val;
			print_char( ch, "Room %d, vnum %d, pojemno¶æ %d\n\r", i, pRoom->vnum, val );
			SET_BIT( pRoom->area->area_flags, AREA_CHANGED );
			continue;
		}
		pRoom->rawmaterial[rsc] = val;
		print_char( ch, "Room %d, vnum %d, surowiec %s, wystêpowanie %d.\n\r", i, pRoom->vnum, rawmaterial_table[rsc].name, val );
		SET_BIT( pRoom->area->area_flags, AREA_CHANGED );
	}

	print_char( ch, "Lokacje o vnum od %d do %d zosta³y zmienione.\n\r", vmin, vmax );
}

/* Entry point for editing obj_index_data. */
void do_oedit( CHAR_DATA *ch, char *argument )
{
	OBJ_INDEX_DATA *pObj;
	AREA_DATA *pArea;
	char arg1[MAX_STRING_LENGTH];
	int value;

	if ( IS_NPC(ch) )
		return;

	argument = one_argument( argument, arg1 );

	if ( is_number( arg1 ) )
	{
		value = atoi( arg1 );
		if ( !( pObj = get_obj_index( value ) ) )
		{
			send_to_char( "OEdit: Obiekt z takim vnumie nie istnieje.\n\r", ch );
			return;
		}

		if ( !IS_BUILDER( ch, pObj->area ) )

		{
			send_to_char( "OEdit: Niewystarczaj±ce security ¿eby modyfikowaæ obiekt.\n\r",ch);
			return;
		}

		ch->desc->pEdit = (void *)pObj;
		ch->desc->editor = ED_OBJECT;
		return;
	}
	else
	{
		if ( !str_cmp( arg1, "create" ) )
		{
			value = atoi( argument );
			if ( argument[0] == '\0' || value == 0 )
			{
				send_to_char( "OEdit: Sk³adnia: oedit create [vnum]\n\r", ch );
				return;
			}

			pArea = get_vnum_area( value );

			if ( !pArea )
			{
				send_to_char( "OEdit:  Vnum nie jest przypisany do krainy.\n\r", ch );
				return;
			}

			if ( !IS_BUILDER( ch, pArea ) )
			{
				send_to_char( "OEdit: Niewystarczaj±ce security ¿eby modyfikowaæ obiekt.\n\r",ch);
				return;
			}

			if ( oedit_create( ch, argument ) )
			{
				SET_BIT( pArea->area_flags, AREA_CHANGED );
				ch->desc->editor = ED_OBJECT;
			}
			return;
		}
	}

	send_to_char ( "OEdit: Sk³adnia: oedit [vnum].\n\r", ch );
        send_to_char ( "OEdit: Sk³adnia: oedit create [vnum].\n\r", ch );
	return;
}



/* Entry point for editing mob_index_data. */
void do_medit( CHAR_DATA *ch, char *argument )
{
	MOB_INDEX_DATA *pMob;
	AREA_DATA *pArea;
	int value;
	char arg1[MAX_STRING_LENGTH];

	argument = one_argument( argument, arg1 );

	if ( IS_NPC(ch) )
		return;

	if ( is_number( arg1 ) )
	{
		value = atoi( arg1 );
		if ( !( pMob = get_mob_index( value ) ))
		{
			send_to_char( "MEdit:  Argument nie istnieje.\n\r", ch );
			return;
		}

		if ( !IS_BUILDER( ch, pMob->area ) )
		{
			send_to_char("Niewystarczajace security zeby modyfikowac moba.\n\r",ch);
			return;
		}

		ch->desc->pEdit = (void *)pMob;
		ch->desc->editor = ED_MOBILE;
		return;
	}
	else
	{
		if ( !str_cmp( arg1, "create" ) )
		{
			value = atoi( argument );
			if ( arg1[0] == '\0' || value == 0 )
			{
				send_to_char( "Syntax:  edit mobile create [vnum]\n\r", ch );
				return;
			}

			pArea = get_vnum_area( value );

			if ( !pArea )
			{
				send_to_char( "OEdit:  That vnum is not assigned an area.\n\r", ch );
				return;
			}

			if ( !IS_BUILDER( ch, pArea ) )
			{
				send_to_char("Niewystarczajace security zeby modyfikowac moba.\n\r",ch);
				return;
			}

			if ( medit_create( ch, argument ) )
			{
				SET_BIT( pArea->area_flags, AREA_CHANGED );
				ch->desc->editor = ED_MOBILE;
			}
			return;
		}
	}

	send_to_char( "MEdit:  Sk³adnia: medit <vnum>.\n\r", ch );
	return;
}



void display_resets( CHAR_DATA *ch )
{
	ROOM_INDEX_DATA	*pRoom;
	RESET_DATA		*pReset;
	MOB_INDEX_DATA	*pMob = NULL;
	char 		buf   [ MAX_STRING_LENGTH ];
	char 		final [ MAX_STRING_LENGTH ];
	int 		iReset = 0;

	EDIT_ROOM(ch, pRoom);
	final[0]  = '\0';

	send_to_char (
			" No.  Loads    Description    Location     [%%]   Vnum   Mx Mn Description"
			"\n\r"
			"==== ======== ============= ============== ==== ======== ===== ==========="
			"\n\r", ch );

	for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
	{
		OBJ_INDEX_DATA  *pObj;
		MOB_INDEX_DATA  *pMobIndex;
		OBJ_INDEX_DATA  *pObjIndex;
		OBJ_INDEX_DATA  *pObjToIndex;
		ROOM_INDEX_DATA *pRoomIndex;

		final[0] = '\0';
		sprintf( final, "[%2d] ", ++iReset );

		switch ( pReset->command )
		{
			default:
				sprintf( buf, "Bad reset command: %c.", pReset->command );
				strcat( final, buf );
				break;

			case 'M':
				if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
				{
					sprintf( buf, "Load Mobile - Bad Mob %d\n\r", pReset->arg1 );
					strcat( final, buf );
					continue;
				}

				if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
				{
					sprintf( buf, "Load Mobile - Bad Room %d\n\r", pReset->arg3 );
					strcat( final, buf );
					continue;
				}

				pMob = pMobIndex;
				sprintf( buf, "M[%5d] %-13.13s in room        %3d  R[%5d] %2d-%2d %-15.15s\n\r",
						pReset->arg1, pMob->short_descr,pReset->arg0, pReset->arg3,
						pReset->arg2, pReset->arg4, pRoomIndex->name );
				strcat( final, buf );

				/*
				 * Check for pet shop.
				 * -------------------
				 */
				{
					ROOM_INDEX_DATA *pRoomIndexPrev;

					pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
					if ( pRoomIndexPrev
							&& EXT_IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
						final[5] = 'P';
				}

				break;

			case 'O':
				if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
				{
					sprintf( buf, "Load Object - Bad Object %d\n\r",
							pReset->arg1 );
					strcat( final, buf );
					continue;
				}

				pObj       = pObjIndex;

				if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
				{
					sprintf( buf, "Load Object - Bad Room %d\n\r", pReset->arg3 );
					strcat( final, buf );
					continue;
				}

				sprintf( buf, "O[%5d] %-13.13s in room        %3d  "
						"R[%5d]       %-15.15s\n\r",
						pReset->arg1, pObj->short_descr,pReset->arg0,
						pReset->arg3, pRoomIndex->name );
				strcat( final, buf );

				break;

			case 'P':
				if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
				{
					sprintf( buf, "Put Object - Bad Object %d\n\r",
							pReset->arg1 );
					strcat( final, buf );
					continue;
				}

				pObj       = pObjIndex;

				if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
				{
					sprintf( buf, "Put Object - Bad To Object %d\n\r",
							pReset->arg3 );
					strcat( final, buf );
					continue;
				}

				sprintf( buf,
						"O[%5d] %-13.13s inside         %3d  O[%5d] %2d-%2d %-15.15s\n\r",
						pReset->arg1,
						pObj->short_descr,pReset->arg0,
						pReset->arg3,
						pReset->arg2,
						pReset->arg4,
						pObjToIndex->short_descr );
				strcat( final, buf );

				break;

			case 'G':
			case 'E':
				if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
				{
					sprintf( buf, "Give/Equip Object - Bad Object %d\n\r",
							pReset->arg1 );
					strcat( final, buf );
					continue;
				}

				pObj       = pObjIndex;

				if ( !pMob )
				{
					sprintf( buf, "Give/Equip Object - No Previous Mobile\n\r" );
					strcat( final, buf );
					break;
				}

				if ( pMob->pShop )
				{
					sprintf( buf,
							"O[%5d] %-13.13s inv S[%5d]   %3d     %-15.15s\n\r",
							pReset->arg1,
							pObj->short_descr,
							pMob->vnum,pReset->arg0,
							pMob->short_descr  );
				}
				else
					sprintf( buf,
							"O[%5d] %-13.13s %-14.14s %3d  M[%5d]       %-15.15s\n\r",
							pReset->arg1,
							pObj->short_descr,
							(pReset->command == 'G') ?
							flag_string( wear_loc_strings, WEAR_NONE )
							: flag_string( wear_loc_strings, pReset->arg3 ),pReset->arg0,
							pMob->vnum,
							pMob->short_descr );
				strcat( final, buf );

				break;

				/*
				 * Doors are set in rs_flags don't need to be displayed.
				 * If you want to display them then uncomment the new_reset
				 * line in the case 'D' in load_resets in db.c and here.
				 */
			case 'D':
				pRoomIndex = get_room_index( pReset->arg1 );
				sprintf( buf, "R[%5d] %s door of %-19.19s reset to %s\n\r",
						pReset->arg1,
						capitalize( dir_name[ pReset->arg2 ] ),
						pRoomIndex->name,
						flag_string( door_resets, pReset->arg3 ) );
				strcat( final, buf );

				break;
				/*
				 * End Doors Comment.
				 */
			case 'R':
				if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
				{
					sprintf( buf, "Randomize Exits - Bad Room %d\n\r",
							pReset->arg1 );
					strcat( final, buf );
					continue;
				}

				sprintf( buf, "R[%5d] Exits are randomized in %s\n\r",
						pReset->arg1, pRoomIndex->name );
				strcat( final, buf );

				break;
		}
		send_to_char( final, ch );
	}

	return;
}



/*****************************************************************************
Name:		add_reset
Purpose:	Inserts a new reset in the given index slot.
Called by:	do_resets(olc.c).
 ****************************************************************************/
void add_reset( ROOM_INDEX_DATA *room, RESET_DATA *pReset, int index )
{
	RESET_DATA *reset;
	int iReset = 0;

	if ( !room->reset_first )
	{
		room->reset_first	= pReset;
		room->reset_last	= pReset;
		pReset->next		= NULL;
		return;
	}

	index--;

	if ( index == 0 )	/* First slot (1) selected. */
	{
		pReset->next = room->reset_first;
		room->reset_first = pReset;
		return;
	}

	/*
	 * If negative slot( <= 0 selected) then this will find the last.
	 */
	for ( reset = room->reset_first; reset->next; reset = reset->next )
	{
		if ( ++iReset == index )
			break;
	}

	pReset->next	= reset->next;
	reset->next		= pReset;
	if ( !pReset->next )
		room->reset_last = pReset;
	return;
}



void do_resets( CHAR_DATA *ch, char *argument )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char arg5[MAX_INPUT_LENGTH];
	char arg6[MAX_INPUT_LENGTH];
	char arg7[MAX_INPUT_LENGTH];
	RESET_DATA *pReset = NULL;
	int chance=100, insert_loc;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
	argument = one_argument( argument, arg4 );
	argument = one_argument( argument, arg5 );
	argument = one_argument( argument, arg6 );
	argument = one_argument( argument, arg7 );



	if ( !IS_BUILDER( ch, ch->in_room->area ) )
	{
		send_to_char( "Resets: Masz niewystarczaj±ce security.\n\r",
				ch );
		return;
	}

	/*
	 * Display resets in current room.
	 * -------------------------------
	 */
	if ( arg1[0] == '\0' )
	{
		if ( ch->in_room->reset_first )
		{
			send_to_char(
					"Resets: M = mobile, R = room, O = object, "
					"P = pet, S = shopkeeper\n\r", ch );
			display_resets( ch );
		}
		else
			send_to_char( "W tym roomie nie zdefiniowano ¿adnych resetów (Sk³adnia: reset 0).\n\r", ch );
	}


	/*
	 * Take index number and search for commands.
	 * ------------------------------------------
	 */
	if ( is_number( arg1 ) )
	{
		ROOM_INDEX_DATA *pRoom = ch->in_room;

		/*wstawka do procentow*/
		if ( !str_cmp( arg2, "chance" ) )
		{
			if(!is_number(arg3))
			{
				send_to_char("Szansa musi byæ podana w liczbie (0-100).\n\r",ch);
				return;
			}
			chance=atoi(arg3);



			insert_loc = atoi( arg1 );

			if ( !ch->in_room->reset_first )
			{
				send_to_char( "No resets in this area.\n\r", ch );
				return;
			}

			if ( insert_loc <= 0 )
			{
				pReset = pRoom->reset_first;
				pRoom->reset_first = pRoom->reset_first->next;
				if ( !pRoom->reset_first )
					pRoom->reset_last = NULL;
			}
			else
			{
				int iReset = 0;


				for ( pReset = pRoom->reset_first;
						pReset;
						pReset = pReset->next )
				{
					if ( ++iReset == insert_loc )
						break;
				}

				if ( !pReset )
				{
					send_to_char( "Nie znaleziono resetu.\n\r", ch );
					return;
				}

			}

			pReset->arg0=chance;
			SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
			send_to_char( "Reset changed.\n\r", ch );
		}




		/*a tu jej koniec*/

		/*
		 * Delete a reset.
		 * ---------------
		 */
		else if ( !str_cmp( arg2, "delete" ) )
		{
			int insert_loc = atoi( arg1 );

			if ( !ch->in_room->reset_first )
			{
				send_to_char( "Nie ma resetów w tej krainie.\n\r", ch );
				return;
			}

			if ( insert_loc-1 <= 0 )
			{
				pReset = pRoom->reset_first;
				pRoom->reset_first = pRoom->reset_first->next;
				if ( !pRoom->reset_first )
					pRoom->reset_last = NULL;
			}
			else
			{
				int iReset = 0;
				RESET_DATA *prev = NULL;

				for ( pReset = pRoom->reset_first;
						pReset;
						pReset = pReset->next )
				{
					if ( ++iReset == insert_loc )
						break;
					prev = pReset;
				}

				if ( !pReset )
				{
					send_to_char( "Nie znaleziono resetu.\n\r", ch );
					return;
				}

				if ( prev )
					prev->next = prev->next->next;
				else
					pRoom->reset_first = pRoom->reset_first->next;

				for ( pRoom->reset_last = pRoom->reset_first;
						pRoom->reset_last->next;
						pRoom->reset_last = pRoom->reset_last->next );
			}

			free_reset_data( pReset );
			SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
			send_to_char( "Reset deleted.\n\r", ch );

		}
		else
			/*
			 * Add a reset.
			 * ------------
			 */
			if ( (!str_cmp( arg2, "mob" ) && is_number( arg3 ))
					|| (!str_cmp( arg2, "obj" ) && is_number( arg3 )) )
			{
				/*
				 * Check for Mobile reset.
				 * -----------------------
				 */
				if ( !str_cmp( arg2, "mob" ) )
				{
					if (get_mob_index( is_number(arg3) ? atoi( arg3 ) : 1 ) == NULL)
					{
						send_to_char("Nie ma moba o takim vnumie.\n\r",ch);
						return;
					}
					pReset = new_reset_data();
					pReset->command = 'M';
					pReset->arg0	=100;
					pReset->arg1    = atoi( arg3 );
					pReset->arg2    = is_number( arg4 ) ? atoi( arg4 ) : 1; /* Max # */
					pReset->arg3    = ch->in_room->vnum;
					pReset->arg4	= is_number( arg5 ) ? atoi( arg5 ) : 1; /* Min # */
				}
				else
					/*
					 * Check for Object reset.
					 * -----------------------
					 */
					if ( !str_cmp( arg2, "obj" ) )
					{
						pReset = new_reset_data();
						pReset->arg0    = 100;
						pReset->arg1    = atoi( arg3 );
						/*
						 * Inside another object.
						 * ----------------------
						 */
						if ( !str_prefix( arg4, "inside" ) )
						{
							OBJ_INDEX_DATA *temp;

							temp = get_obj_index(is_number(arg5) ? atoi(arg5) : 1);
							if ( ( temp->item_type != ITEM_CONTAINER ) &&
									( temp->item_type != ITEM_CORPSE_NPC ) )
							{
								send_to_char( "Objeto 2 no es container.\n\r", ch);
								return;
							}
							pReset->command = 'P';
							pReset->arg2    = is_number( arg6 ) ? atoi( arg6 ) : 1;
							pReset->arg3    = is_number( arg5 ) ? atoi( arg5 ) : 1;
							pReset->arg4    = is_number( arg7 ) ? atoi( arg7 ) : 1;
						}
						else
							/*
							 * Inside the room.
							 * ----------------
							 */
							if ( !str_cmp( arg4, "room" ) )
							{
								if (get_obj_index(atoi(arg3)) == NULL)
								{
									send_to_char( "Vnum nie istnieje.\n\r",ch);
									return;
								}
								pReset->command  = 'O';
								pReset->arg2     = 0;
								pReset->arg3     = ch->in_room->vnum;
								pReset->arg4     = 0;
							}
							else
								/*
								 * Into a Mobile's inventory.
								 * --------------------------
								 */
							{
								if ( flag_value( wear_loc_flags, arg4 ) == NO_FLAG )
								{
									send_to_char( "Resets: '? wear-loc'\n\r", ch );
									return;
								}
								if (get_obj_index(atoi(arg3)) == NULL)
								{
									send_to_char( "Vnum nie istnieje.\n\r",ch);
									return;
								}
								pReset->arg1 = atoi(arg3);
								pReset->arg3 = flag_value( wear_loc_flags, arg4 );
								if ( pReset->arg3 == WEAR_NONE )
									pReset->command = 'G';
								else
									pReset->command = 'E';
							}
					}

				add_reset( ch->in_room, pReset, atoi( arg1 ) );
				SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
				send_to_char( "Reset added.\n\r", ch );
			}
			else
				if (!str_cmp( arg2, "random") && is_number(arg3))

				{
					if (atoi(arg3) < 1 || atoi(arg3) > 6)
					{
						send_to_char("Invalid argument.\n\r", ch);
						return;
					}
					pReset = new_reset_data ();
					pReset->command = 'R';
					pReset->arg1 = ch->in_room->vnum;
					pReset->arg2 = atoi(arg3);
					add_reset( ch->in_room, pReset, atoi( arg1 ) );
					SET_BIT( ch->in_room->area->area_flags, AREA_CHANGED );
					send_to_char( "Random exits reset added.\n\r", ch);
				}
				else
				{
					send_to_char( "Sk³adnia: RESET <number> OBJ <vnum> <wear_loc>   - ubiera moba (? wear_loc)\n\r", ch );
					send_to_char( "          RESET <number> OBJ <vnum1> inside <vnum2> [limit] [count]   - ³aduje przedmiot(vnum1) do pojemnika(vnum2)\n\r", ch );
					send_to_char( "          RESET <number> OBJ <vnum> room   - ³aduje przedmiot w lokacji\n\r", ch );
					send_to_char( "          RESET <number> MOB <vnum> [max #x area] [max #x room]   - ³aduje moba\n\r", ch );
					send_to_char( "          RESET <number> DELETE   - usuwa reset\n\r", ch );
					send_to_char( "          RESET <number> RANDOM [#x exits]   - powoduje, ¿e wyj¶cia same zmieniaj± siê co reset\n\r", ch);
					send_to_char( "          RESET <number> CHANCE [1-100]   - szansa na odpalenie siê danego resetu\n\r", ch);
				}
	}

	return;
}



/*****************************************************************************
Name:		do_alist
Purpose:	Normal command to list areas and display area information.
Called by:	interpreter(interp.c)
 ****************************************************************************/
void do_alist( CHAR_DATA *ch, char *argument )
{
    char buf [ MAX_STRING_LENGTH ];
    char result [ MAX_STRING_LENGTH * 2 ];	/* May need tweaking. */
    char author [ MAX_STRING_LENGTH ];
    AREA_DATA *pArea, *area_min = NULL;
    int min_vnm = -1, vnm = -1;
    BUFFER *buffer;
    bool all = FALSE;
    bool found = FALSE;
    ush_int vnum, rooms, objects, mobiles;

    argument = one_argument( argument, author );

    if ( IS_NPC( ch ) )
        return ;

    if ( author[ 0 ] != '\0' )
        author[ 0 ] = UPPER( author[ 0 ] );

    while ( !all )
    {
        for ( pArea = area_first; pArea; pArea = pArea->next )
        {
            if ( vnm < 0 && pArea->min_vnum > min_vnm )
            {
                vnm = pArea->min_vnum;
                area_min = pArea;
            }
            else if ( pArea->min_vnum < vnm && pArea->min_vnum > min_vnm )
            {
                vnm = pArea->min_vnum;
                area_min = pArea;
            }
        }

        if ( area_min )
        {
            if ( author[ 0 ] != '\0'	&&
                    !strstr( area_min->builders, "All" ) &&
                    !strstr( area_min->builders, author ) )
            {
                vnm = -1;
                min_vnm = area_min->max_vnum;
                area_min = NULL;
                continue;
            }

            if ( !found )
            {
                found = TRUE;
                buffer = new_buf();

                sprintf( result, "[%3s] [%-27s] (%-5s-%5s) [%-10s] %3s [%-10s]\n\r",
                        "Num", "Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders" );

                add_buf( buffer, result );
            }

            sprintf( buf, "[%3d] %-*.*s {x(%-5d-%5d) %-12.12s [%d] [%-10.10s]\n\r",
                    area_min->vnum,
                    29 + count_colors( area_min->name, 0 ),
                    29 + count_colors( area_min->name, 0 ),
                    area_min->name,
                    area_min->min_vnum,
                    area_min->max_vnum,
                    area_min->file_name,
                    area_min->security,
                    area_min->builders );

            add_buf( buffer, buf );

            /* specjalne informacje: ile roomow, obiektow, etc */
            if ( argument[ 0 ] != '\0' )
            {
                for ( rooms = 0, objects = 0, mobiles = 0, vnum = area_min->min_vnum; vnum <= area_min->max_vnum; vnum++ )
                {
                    if ( get_mob_index( vnum ) )
                        mobiles++;
                    if ( get_room_index( vnum ) )
                        rooms++;
                    if ( get_obj_index( vnum ) )
                        objects++;
                }
                sprintf( buf, "      Rooms:   %3d\n\r      Objects: %3d\n\r      Mobiles: %3d\n\r",
                        rooms, objects, mobiles );
                add_buf( buffer, buf );
            }

            vnm = -1;
            min_vnm = area_min->max_vnum;
            area_min = NULL;
        }
        else
            all = TRUE;
    }

    if ( found )
    {
        page_to_char( buf_string( buffer ), ch );
        free_buf( buffer );
    }
    return ;
}

void do_regions( CHAR_DATA *ch, char *argument )
{
	char buf [ MAX_STRING_LENGTH ];
	AREA_DATA *pArea;
	AREA_DATA *sArea;
	BUFFER *buffer;

	if ( IS_NPC( ch ) )
		return ;

	buffer = new_buf();

	for ( pArea = area_first; pArea; pArea = pArea->next )
		SET_BIT( pArea->area_flags, AREA_LOADING );

	for ( pArea = area_first; pArea; pArea = pArea->next )
	{
		if ( !IS_SET( pArea->area_flags, AREA_LOADING ) || pArea->region == 0 )
			continue;

		sprintf( buf, "Region numer {C%d:{X\n\r", pArea->region );
		add_buf( buffer, buf );

		sprintf( buf, "[%3s] [%-27s] (%-5s-%5s) [%-10s] %3s [%-10s]\n\r",
				"Num", "Area Name", "lvnum", "uvnum", "Filename", "Sec", "Builders" );
		add_buf( buffer, buf );

		for ( sArea = area_first; sArea; sArea = sArea->next )
		{
			if ( !IS_SET( sArea->area_flags, AREA_LOADING ) || sArea->region == 0 || sArea->region != pArea->region )
				continue;

			REMOVE_BIT( sArea->area_flags, AREA_LOADING );

			sprintf( buf, "[%3d] %-*.*s {x(%-5d-%5d) %-12.12s [%d] [%-10.10s]\n\r",
					sArea->vnum,
					29 + count_colors( sArea->name, 0 ),
					29 + count_colors( sArea->name, 0 ),
					sArea->name,
					sArea->min_vnum,
					sArea->max_vnum,
					sArea->file_name,
					sArea->security,
					sArea->builders );
			add_buf( buffer, buf );
		}

		REMOVE_BIT( pArea->area_flags, AREA_LOADING );
		add_buf( buffer, "\n\r" );
	}

	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );
	return ;
}

void do_mlist( CHAR_DATA *ch, char *argument )
{
	MOB_INDEX_DATA	*pMobIndex;
	AREA_DATA		*pArea;
	char		buf  [ MAX_STRING_LENGTH   ];
	BUFFER		*buf1;
	char		arg  [ MAX_INPUT_LENGTH    ];
	bool fAll = FALSE, detailed = FALSE;
	bool found;
	ush_int vnum;
	int  colors;
	int  col = 0;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Syntax:  mlist <all/details/name>\n\r", ch );
		fAll = TRUE;
	}

	buf1 = new_buf();
	pArea = ch->in_room->area;
	if ( !fAll )
    {
		fAll = !str_cmp( arg, "all" );
    }
    found = FALSE;
    detailed = !str_cmp( arg, "details" );

	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
	{
		if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
		{
			if ( fAll || detailed || is_name( arg, pMobIndex->player_name ) )
			{
				found = TRUE;
				colors = count_colors(pMobIndex->short_descr, 17);

                if ( detailed )
                {
                    sprintf
                        (
                         buf,
                         "{x[%5d] %2d | {g%2dd%-3d+%4d{x | {r%2dd%-3d+%4d{x | %s (%s)\n\r",
                         pMobIndex->vnum,
                         pMobIndex->level,
                         pMobIndex->hit[ DICE_NUMBER ],
                         pMobIndex->hit[ DICE_TYPE ],
                         pMobIndex->hit[ DICE_BONUS ],
                         pMobIndex->damage[ DICE_NUMBER ],
                         pMobIndex->damage[ DICE_TYPE ],
                         pMobIndex->damage[ DICE_BONUS ],
                         pMobIndex->short_descr,
                         race_table[ pMobIndex->race ].name
                        );
                    add_buf( buf1, buf );
                }
                else
                {
                    sprintf
                        (
                         buf,
                         "{x[%5d] %-*.*s",
                         pMobIndex->vnum,
                         17+colors,
                         16+colors,
                         capitalize( pMobIndex->short_descr )
                        );
                    add_buf( buf1, buf );
                    if ( ++col % 3 == 0 )
                    {
                        add_buf( buf1, "\n\r" );
                    }
                }
			}
		}
	}

	if ( !found )
	{
		send_to_char( "Nie ma mobow w tej kraince...\n\r", ch);
		free_buf(buf1);
		return;
	}

	if ( col % 3 != 0 )
		add_buf( buf1, "\n\r" );

	page_to_char( buf_string(buf1), ch );
	free_buf(buf1);
	return;
}

void do_tlist( CHAR_DATA *ch, char *argument )
{
	AREA_DATA *pArea;
	TRAP_DATA *trap;
	char buffer[ MAX_STRING_LENGTH   ];
	BUFFER *buf;
	bool found = FALSE;
	ush_int vnum;
	OBJ_INDEX_DATA *pObj;
	int index, i;
	ROOM_INDEX_DATA *pRoom;
	int door;
	EXIT_DATA *pExit;


	buf = new_buf();

	pArea = ch->in_room->area;

	found   = FALSE;

	for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
	{
		if( (trap = get_trap_index( vnum )) == NULL )
			continue;

		if( !found )
		{
			found = TRUE;
			buf = new_buf();
		}

		sprintf( buffer, "Trap Vnum:%-5d  %-8s Level:%-2d Type:",
				trap->vnum,
				trap->active ? "ACTIVE" : "INACTIVE",
				trap->level);

		for( i = 0; trap_type_table[i].name; i++ )
			if( IS_SET(trap->type, trap_type_table[i].flag))
			{
				strcat(buffer, trap_type_table[i].name);
				strcat(buffer, " ");
			}

		strcat(buffer, "\r\n");
		add_buf( buf, buffer );

		for( index = 0; index < MAX_KEY_HASH; index++ )
			for( pObj = obj_index_hash[index]; pObj; pObj = pObj->next )
				if( pObj->trap == vnum )
				{
					sprintf(buffer,"    Obj: %-25.25s [%-5d] got this trap\n\r",
							pObj->short_descr, pObj->vnum);
					add_buf( buf, buffer );
				}

		for( index = 0; index < MAX_KEY_HASH; index++ )
			for( pRoom = room_index_hash[index]; pRoom; pRoom = pRoom->next )
			{
				if( pRoom->trap == vnum )
				{
					sprintf(buffer,"    Room [%-5d] got this trap\n\r",pRoom->vnum);
					add_buf( buf, buffer );
				}

				for( door = 0; door < 6; door++ )
				{
					pExit = pRoom->exit[door];

					if( !pExit )
						continue;

					if( pExit->trap == vnum )
					{
						sprintf(buffer,"    Exit %s in room [%-5d] got this trap\n\r",
								dir_name[door], pRoom->vnum);
						add_buf( buf, buffer );
					}
				}
			}

	}

	if ( !found )
	{
		send_to_char( "Nie ma pulapek w tej kraince...\n\r", ch);
		free_buf(buf);
		return;
	}

	page_to_char( buf_string(buf), ch );
	free_buf(buf);
	return;
}

void do_olist( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *pObjIndex;
    AREA_DATA *pArea;
    char buf [ MAX_STRING_LENGTH ];
    BUFFER *buf1;
    char arg [ MAX_INPUT_LENGTH ];
    bool fAll = FALSE, detailed = FALSE;
    bool found;
    ush_int vnum;
    int col = 0;
    int colors;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax:  olist <all/details/name/item_type>\n\r", ch );
        fAll = TRUE;
    }

    pArea = ch->in_room->area;
    buf1=new_buf();
    if ( !fAll )
    {
        fAll    = !str_cmp( arg, "all" );
    }
    found   = FALSE;
    detailed = !str_cmp( arg, "details" );

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
        if ( ( pObjIndex = get_obj_index( vnum ) ) )
        {
            if (
                    fAll ||
                    detailed ||
                    is_name( arg, pObjIndex->name ) ||
                    flag_value( type_flags, arg ) == pObjIndex->item_type
               )
            {
                found = TRUE;
                colors = count_colors(pObjIndex->short_descr, 17);

                if ( detailed )
                {
                    sprintf
                        (
                         buf,
                         "{x[%5d] %s T:{g%s{x, M:%s, W:{g%d/%.2f{x %d\n\r",
                         pObjIndex->vnum,
                         pObjIndex->short_descr,
                         flag_string( type_flags, pObjIndex->item_type ),
                         material_table[pObjIndex->material].name,
                         pObjIndex->weight, (float) pObjIndex->weight / 22.05,
                         pObjIndex->cost
                        );
                    add_buf( buf1, buf );
                }
                else
                {
                    sprintf
                        (
                         buf,
                         "{x[%5d] %-*.*s",
                         pObjIndex->vnum,
                         17+colors,
                         16+colors,
                         pObjIndex->short_descr
                        );
                    add_buf( buf1, buf );
                    if ( ++col % 3 == 0 )
                    {
                        add_buf( buf1, "\n\r" );
                    }
                }
            }
        }
    }

    if ( !found )
    {
        send_to_char( "Object(s) not found in this area.\n\r", ch);
        free_buf(buf1);
        return;
    }

    if ( col % 3 != 0 )
    {
        add_buf( buf1, "\n\r" );
    }
    page_to_char( buf_string(buf1), ch );
    free_buf(buf1);
    return;
}

void do_rlist( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *pRoomIndex;
    AREA_DATA *pArea;
    char buf [ MAX_STRING_LENGTH ];
    BUFFER *buf1;
    char arg [ MAX_INPUT_LENGTH ];
    bool found = FALSE, detailed = FALSE;;
    ush_int vnum, col = 0;
    int colors;

    one_argument( argument, arg );

    pArea = ch->in_room->area;
    buf1 = new_buf();

    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax:  olist [details]\n\r", ch );
    }
    detailed = !str_cmp( arg, "details" );

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
        if ( ( pRoomIndex = get_room_index( vnum ) ) )
        {
            found = TRUE;
            if ( detailed )
            {
                sprintf
                    (
                     buf,
                     "{x[%5d] %s S:{G%s{x\n\r",
                     vnum,
                     pRoomIndex->name,
                     sector_table[pRoomIndex->sector_type].name
                    );
                add_buf( buf1, buf );
            }
            else
            {
                colors = count_colors(pRoomIndex->name, 17);
                sprintf( buf, "{x[%5d] %-*.*s", vnum, 17 + colors, 16 + colors, capitalize( pRoomIndex->name ) );
                add_buf( buf1, buf );
                if ( ++col % 3 == 0 )
                {
                    add_buf( buf1, "\n\r" );
                }
            }
        }
    }

    if ( !found )
    {
        send_to_char( "Room(s) not found in this area.\n\r", ch);
        free_buf(buf1);
        return;
    }

    if ( col % 3 != 0 )
    {
        add_buf( buf1, "\n\r" );
    }

    page_to_char( buf_string(buf1), ch );
    free_buf(buf1);
    return;
}

void do_mplist( CHAR_DATA *ch, char *argument )
{
	int count = 1;
	PROG_CODE *mprg;
	char buf[ MAX_STRING_LENGTH ];
	char desc[ MAX_STRING_LENGTH ];
	char inf[ MAX_STRING_LENGTH ];
	BUFFER *buffer;
	bool fAll;
	char blah;
	AREA_DATA *ad;
	AREA_DATA *area;
	char* min = "zz";
	char* actual = "00";

	argument = one_argument( argument, inf );
	fAll = !str_cmp( inf, "all" );

	if ( fAll )
		argument = one_argument( argument, inf );

	buffer = new_buf();

	area = ch->in_room->area;

	if ( !fAll )
	{
		while( str_cmp(min, "zzz") )
		{
			min = "zzz";
			for ( mprg = mprog_list; mprg != NULL; mprg = mprg->next )
			{
				if ( mprg->area == area )
				{
					if ( inf[0] != '\0' && str_infix( inf, mprg->name ) )
						continue;

					if ( strcmp(mprg->name, min) <= 0
							&& strcmp(mprg->name, actual) > 0 )
						min = mprg->name;
				}
			}

			actual = min;

			if ( strcmp( actual, "zzz" ) )
			{
				for ( mprg = mprog_list; mprg != NULL; mprg = mprg->next )
				{
					if ( mprg->area == area )
					{
						if ( inf[0] != '\0' && str_infix( inf, mprg->name ) )
							continue;

						if ( str_cmp( mprg->name, actual ) )
							continue;

						ad = mprg->area;

						if ( ad == NULL )
							blah = '?';
						else
							if ( IS_BUILDER( ch, ad ) )
								blah = '*';
							else
								blah = ' ';

						sprintf( buf, "{CArea{x: [%5d] %-15.15s {CName{x: {G%c%-15.15s{x\n\r",
								ad ? ad->vnum : 0,
								ad ? ad->name : "none",
								blah,
								mprg->name );
						add_buf( buffer, buf );

						if ( !IS_NULLSTR( mprg->description ) )
						{
							getline_olc( mprg->description, desc );
							sprintf( buf, "%s\n\r", desc );
							add_buf( buffer, buf );
						}

						count++;
					}
				}
			}
		}
	}
	else
	{
		for ( area = area_first; area; area = area->next )
		{
			min = "zz";
			actual = "00";
			while( str_cmp(min, "zzz") )
			{
				min = "zzz";
				for ( mprg = mprog_list; mprg != NULL; mprg = mprg->next )
				{
					if ( mprg->area == area )
					{
						if ( inf[0] != '\0' && str_infix( inf, mprg->name ) )
							continue;

						if ( strcmp(mprg->name, min) <= 0
								&& strcmp(mprg->name, actual) > 0 )
							min = mprg->name;
					}
				}

				actual = min;

				if ( strcmp( actual, "zzz" ) )
				{
					for ( mprg = mprog_list; mprg != NULL; mprg = mprg->next )
					{
						if ( mprg->area == area )
						{
							if ( inf[0] != '\0' && str_infix( inf, mprg->name ) )
								continue;

							if ( str_cmp( mprg->name, actual ) )
								continue;

							ad = mprg->area;

							if ( ad == NULL )
								blah = '?';
							else
								if ( IS_BUILDER( ch, ad ) )
									blah = '*';
								else
									blah = ' ';

							sprintf( buf, "{CArea{x: [%5d] %-15.15s {CName{x: {G%c%-15.15s{x\n\r",
									ad ? ad->vnum : 0,
									ad ? ad->name : "none",
									blah,
									mprg->name );
							add_buf( buffer, buf );

							if ( !IS_NULLSTR( mprg->description ) )
							{
								getline_olc( mprg->description, desc );
								sprintf( buf, "%s\n\r", desc );
								add_buf( buffer, buf );
							}

							count++;
						}
					}
				}
			}
		}
	}

	if ( count == 1 )
	{
		if ( fAll )
			send_to_char( "Nie ma takich mobprogów.\n\r", ch );
		else
			send_to_char( "Nie ma takich mobprogów w tej krainie.\n\r", ch );
		free_buf( buffer );
		return;
	}

	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );

	return;
}

void do_oplist( CHAR_DATA *ch, char *argument )
{
	int count = 1;
	PROG_CODE *oprg;
	char buf[ MAX_STRING_LENGTH ];
	char desc[ MAX_STRING_LENGTH ];
	char inf[ MAX_STRING_LENGTH ];
	BUFFER *buffer;
	bool fAll;
	char blah;
	AREA_DATA *ad;
	AREA_DATA *area;
	char* min = "zz";
	char* actual = "00";

	argument = one_argument( argument, inf );
	fAll = !str_cmp( inf, "all" );

	if ( fAll )
		argument = one_argument( argument, inf );

	buffer = new_buf();

	area = ch->in_room->area;

	if ( !fAll )
	{
		while( str_cmp(min, "zzz") )
		{
			min = "zzz";
			for ( oprg = oprog_list; oprg != NULL; oprg = oprg->next )
			{
				if ( oprg->area == area )
				{
					if ( inf[0] != '\0' && str_infix( inf, oprg->name ) )
						continue;

					if ( strcmp(oprg->name, min) <= 0
							&& strcmp(oprg->name, actual) > 0 )
						min = oprg->name;
				}
			}

			actual = min;

			if ( strcmp( actual, "zzz" ) )
			{
				for ( oprg = oprog_list; oprg != NULL; oprg = oprg->next )
				{
					if ( oprg->area == area )
					{
						if ( inf[0] != '\0' && str_infix( inf, oprg->name ) )
							continue;

						if ( str_cmp( oprg->name, actual ) )
							continue;

						ad = oprg->area;

						if ( ad == NULL )
							blah = '?';
						else
							if ( IS_BUILDER( ch, ad ) )
								blah = '*';
							else
								blah = ' ';

						sprintf( buf, "{CArea{x: [%5d] %-15.15s {CName{x: %c{G%-15.15s{x\n\r",
								ad ? ad->vnum : 0,
								ad ? ad->name : "none",
								blah,
								oprg->name );
						add_buf( buffer, buf );

						if ( !IS_NULLSTR( oprg->description ) )
						{
							getline_olc( oprg->description, desc );
							sprintf( buf, "%s\n\r", desc );
							add_buf( buffer, buf );
						}

						count++;
					}
				}
			}
		}
	}
	else
	{
		for ( area = area_first; area; area = area->next )
		{
			min = "zz";
			actual = "00";

			while( str_cmp(min, "zzz") )
			{
				min = "zzz";
				for ( oprg = oprog_list; oprg != NULL; oprg = oprg->next )
				{
					if ( oprg->area == area )
					{
						if ( inf[0] != '\0' && str_infix( inf, oprg->name ) )
							continue;

						if ( strcmp(oprg->name, min) <= 0
								&& strcmp(oprg->name, actual) > 0 )
							min = oprg->name;
					}
				}

				actual = min;

				if ( strcmp( actual, "zzz" ) )
				{
					for ( oprg = oprog_list; oprg != NULL; oprg = oprg->next )
					{
						if ( oprg->area == area )
						{
							if ( inf[0] != '\0' && str_infix( inf, oprg->name ) )
								continue;

							if ( str_cmp( oprg->name, actual ) )
								continue;

							ad = oprg->area;

							if ( ad == NULL )
								blah = '?';
							else
								if ( IS_BUILDER( ch, ad ) )
									blah = '*';
								else
									blah = ' ';

							sprintf( buf, "{CArea{x: [%5d] %-15.15s {CName{x: %c{G%-15.15s{x\n\r",
									ad ? ad->vnum : 0,
									ad ? ad->name : "none",
									blah,
									oprg->name );
							add_buf( buffer, buf );

							if ( !IS_NULLSTR( oprg->description ) )
							{
								getline_olc( oprg->description, desc );
								sprintf( buf, "%s\n\r", desc );
								add_buf( buffer, buf );
							}

							count++;
						}
					}
				}
			}
		}
	}

	if ( count == 1 )
	{
		if ( fAll )
			send_to_char( "Nie ma takich objprogów.\n\r", ch );
		else
			send_to_char( "Nie ma takich objprogów w tej krainie.\n\r", ch );
		free_buf( buffer );
		return;
	}

	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );

	return;
}

void do_rplist( CHAR_DATA *ch, char *argument )
{
	int count = 1;
	PROG_CODE *rprg;
	char buf[ MAX_STRING_LENGTH ];
	char desc[ MAX_STRING_LENGTH ];
	char inf[ MAX_STRING_LENGTH ];
	BUFFER *buffer;
	bool fAll;
	char blah;
	AREA_DATA *ad;
	AREA_DATA *area;
	char* min = "zz";
	char* actual = "00";

	argument = one_argument( argument, inf );
	fAll = !str_cmp( inf, "all" );

	if ( fAll )
		argument = one_argument( argument, inf );

	buffer = new_buf();

	area = ch->in_room->area;

	if ( !fAll )
	{
		while( str_cmp(min, "zzz") )
		{
			min = "zzz";
			for ( rprg = rprog_list; rprg != NULL; rprg = rprg->next )
			{
				if ( rprg->area == area )
				{
					if ( inf[0] != '\0' && str_infix( inf, rprg->name ) )
						continue;

					if ( strcmp(rprg->name, min) <= 0
							&& strcmp(rprg->name, actual) > 0 )
						min = rprg->name;
				}
			}

			actual = min;

			if ( strcmp( actual, "zzz" ) )
			{
				for ( rprg = rprog_list; rprg != NULL; rprg = rprg->next )
				{
					if ( rprg->area == area )
					{
						if ( inf[0] != '\0' && str_infix( inf, rprg->name ) )
							continue;

						if ( str_cmp( rprg->name, actual ) )
							continue;

						ad = rprg->area;

						if ( ad == NULL )
							blah = '?';
						else
							if ( IS_BUILDER( ch, ad ) )
								blah = '*';
							else
								blah = ' ';

						sprintf( buf, "{CArea{x: [%5d] %-15.15s {CName{x: %c{G%-15.15s{x\n\r",
								ad ? ad->vnum : 0,
								ad ? ad->name : "none",
								blah,
								rprg->name );
						add_buf( buffer, buf );

						if ( !IS_NULLSTR( rprg->description ) )
						{
							getline_olc( rprg->description, desc );
							sprintf( buf, "%s\n\r", desc );
							add_buf( buffer, buf );
						}

						count++;
					}
				}
			}
		}
	}
	else
	{
		for ( area = area_first; area; area = area->next )
		{
			min = "zz";
			actual = "00";

			while( str_cmp(min, "zzz") )
			{
				min = "zzz";
				for ( rprg = rprog_list; rprg != NULL; rprg = rprg->next )
				{
					if ( rprg->area == area )
					{
						if ( inf[0] != '\0' && str_infix( inf, rprg->name ) )
							continue;

						if ( strcmp(rprg->name, min) <= 0
								&& strcmp(rprg->name, actual) > 0 )
							min = rprg->name;
					}
				}

				actual = min;

				if ( strcmp( actual, "zzz" ) )
				{
					for ( rprg = rprog_list; rprg != NULL; rprg = rprg->next )
					{
						if ( rprg->area == area )
						{
							if ( inf[0] != '\0' && str_infix( inf, rprg->name ) )
								continue;

							if ( str_cmp( rprg->name, actual ) )
								continue;

							ad = rprg->area;

							if ( ad == NULL )
								blah = '?';
							else
								if ( IS_BUILDER( ch, ad ) )
									blah = '*';
								else
									blah = ' ';

							sprintf( buf, "{CArea{x: [%5d] %-15.15s {CName{x: %c{G%-15.15s{x\n\r",
									ad ? ad->vnum : 0,
									ad ? ad->name : "none",
									blah,
									rprg->name );
							add_buf( buffer, buf );

							if ( !IS_NULLSTR( rprg->description ) )
							{
								getline_olc( rprg->description, desc );
								sprintf( buf, "%s\n\r", desc );
								add_buf( buffer, buf );
							}

							count++;
						}
					}
				}
			}
		}
	}

	if ( count == 1 )
	{
		if ( fAll )
			send_to_char( "Nie ma takich roomprogów.\n\r", ch );
		else
			send_to_char( "Nie ma takich roomprogów w tej krainie.\n\r", ch );
		free_buf( buffer );
		return;
	}

	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );

	return;
}

void do_bslist( CHAR_DATA *ch, char *argument )
{
	BONUS_INDEX_DATA * bonus;
	OBJ_INDEX_DATA *pObjIndex;
	char buf [ MAX_STRING_LENGTH ];
	BUFFER *buffer;
	bool found = FALSE;
	int count1 = 0;
	int count2 = 0;
	int colors;
	ush_int vnum;
	bool fAll;

	buffer = new_buf();
	fAll = !str_cmp( argument, "all" );

	for ( bonus = bonus_list; bonus; bonus = bonus->next )
	{
		if ( fAll || bonus->area == ch->in_room->area )
		{
			found  = TRUE;
			count1 = 0;
			count2++;
			for ( vnum = bonus->area->min_vnum; vnum <= bonus->area->max_vnum; vnum++ )
			{
				pObjIndex = get_obj_index( vnum );
				if ( pObjIndex && pObjIndex->bonus_set == bonus->vnum )
				{
					count1++;
				}
			}

			colors = 40 + count_colors( bonus->name, 0 );
			sprintf( buf, "[%5d] %-*.*s przedmiotów w komplecie: %d\n\r",
					bonus->vnum,
					colors, colors,
					IS_NULLSTR( bonus->name ) ? "no name" : bonus->name,
					count1 );
			add_buf( buffer, buf );
		}
	}

	if ( !found )
	{
		if ( fAll )
		{
			add_buf( buffer, "Na mudzie nie zdefiniowano ¿adnych bonus setów.\n\r" );
		}
		else
		{
			add_buf( buffer, "W tej krainie nie zdefiniowano ¿adnych bonus setów.\n\r" );
		}
	}
	else
	{
		sprintf( buf, "Razem: %d\n\r", count2 );
		add_buf( buffer, buf );
	}

	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );
	return ;
}

/**
 * Version 1.0
 * FREE VNUM LISTING by The Mage (c) 1998
 * This routine I places in act_wiz.c for my builders. It simply lists
 * all free vnums in an area based on mob, obj, or room.
 *
 * The file is released under the GNU license. Feel free to use it.
 * Give me credit if you want.
 *
 * The Mage IMP of The Mage's Lair (lordmage.erols.com port 4000)
 *
 * NOTE: This assumes you know how to place things in interp.c and interp.h
 * and it uses Lope's Colour code.
 *
 * show a list of all used AreaVNUMS
 *
 * By The Mage
 */
void do_fvlist (CHAR_DATA *ch, char *argument)
{
    int i,j = 0, column = 10;
    char arg[MAX_INPUT_LENGTH];
    char *string;
    string = one_argument(argument,arg);
    if (arg[0] == '\0')
    {
        send_to_char("Syntax:\n\r",ch);
        send_to_char("  fvlist obj\n\r",ch);
        send_to_char("  fvlist mob\n\r",ch);
        send_to_char("  fvlist room\n\r",ch);
        return;
    }
    if (!str_cmp(arg,"obj"))
    {
        printf_to_char(ch,"{WFree {C%s{W vnum listing for area {C%s{x\n\r", arg, ch->in_room->area->name );
        printf_to_char(ch,"{Y=============================================================================={x\n\r");
        for (i = ch->in_room->area->min_vnum; i <= ch->in_room->area->max_vnum; i++)
        {
            if (get_obj_index(i) == NULL)
            {
                printf_to_char(ch,"%6d ",i);
                if ( j == column )
                {
                    send_to_char("\n\r",ch);
                    j=0;
                }
                else j++;
            }
        }
    }
    else if (!str_cmp(arg,"mob"))
    {
        printf_to_char(ch,"{WFree {C%s {Wvnum listing for area {C%s{x\n\r", arg, ch->in_room->area->name);
        printf_to_char(ch,"{Y=============================================================================={x\n\r");
        for (i = ch->in_room->area->min_vnum; i <= ch->in_room->area->max_vnum; i++)
        {
            if (get_mob_index(i) == NULL)
            {
                printf_to_char(ch,"%6d ",i);
                if (j == column )
                {
                    send_to_char("\n\r",ch);
                    j=0;
                }
                else j++;
            }
        }
    }

    else if (!str_cmp(arg,"room"))
    {
        printf_to_char(ch,"{WFree {C%s {Wvnum listing for area {C%s{x\n\r", arg, ch->in_room->area->name);
        printf_to_char(ch,"{Y=============================================================================={x\n\r");
        for (i = ch->in_room->area->min_vnum; i <= ch->in_room->area->max_vnum; i++)
        {
            if (get_room_index(i) == NULL)
            {
                printf_to_char( ch, "%6d ", i );
                if ( j == column ) {
                    send_to_char("\n\r",ch);
                    j=0;
                }
                else j++;
            }
        }
    }
    else
    {
        send_to_char("Syntax:\n\r",ch);
        send_to_char("  fvlist obj\n\r",ch);
        send_to_char("  fvlist mob\n\r",ch);
        send_to_char("  fvlist room",ch);
    }
    send_to_char("\n\r",ch);
}

