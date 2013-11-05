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
 * Kamil Kulesza         (kamili@mud.pl                 ) [Kamili    ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: progs_engine.c 10864 2012-01-16 18:01:32Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/progs_engine.c $
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
#include <stdarg.h>
#include <ctype.h>
#include "merc.h"
#include "tables.h"
#include "progs.h"
#include "lang.h"
#include "interp.h"
#include "recycle.h"
#include "clans.h"

/*
 * Local functions.
 */
bool get_exit_number args ( (char buf[ MAX_INPUT_LENGTH ], CHAR_DATA * mob) );

CHAR_DATA *	supermob			= NULL;
PROG_ENV *	running_prog_env	= NULL;
MEMDAT *	memdat_list			= NULL;

const struct prog_check prog_trigs[] =
{
    {	"act",			TRUE,	TRUE,	TRUE	},
    {	"bribe",		TRUE, 	FALSE,	FALSE	},
    {	"death",		TRUE,	FALSE,	FALSE	},
    {	"entry",		TRUE,	FALSE,	TRUE	},
    {	"fight",		TRUE,	TRUE,	FALSE	},
    {	"give",			TRUE,	FALSE,	FALSE	},
    {	"greet",		TRUE,	FALSE,	FALSE	},
    {	"grall",		TRUE,	FALSE,	FALSE	},
    {	"kill",			TRUE,	FALSE,	FALSE	},
    {	"hpcnt",		TRUE,	TRUE,	FALSE	},
    {	"random",		TRUE,	TRUE,	TRUE	},
    {	"speech",		TRUE,	FALSE,	TRUE	},
    {	"exit",			TRUE,	FALSE,	TRUE	},
    {	"exall",		TRUE,	FALSE,	FALSE	},
    {	"delay",		TRUE,	FALSE,	TRUE	},
    {	"surrender",	TRUE,	FALSE,	FALSE	},
    {	"wear",			FALSE,	TRUE,	FALSE	},
    {	"get",			FALSE,	TRUE,	FALSE	},
    {	"drop",			FALSE,	TRUE,	FALSE	},
    {	"sac",			FALSE,	TRUE,	FALSE	},
    {	"put",			FALSE,	TRUE,	FALSE	},
    {	"remove",		FALSE,	TRUE,	FALSE	},
    {	"input",		FALSE,	TRUE,	TRUE	},
    {	"sit",			FALSE,	FALSE,	TRUE	},
    {	"sleep",		FALSE,	FALSE,	TRUE	},
    {	"rest",			FALSE,	FALSE,	TRUE	},
    {	"connect",		FALSE,	TRUE,	FALSE	},
    {	"consume",		FALSE,	TRUE,	FALSE	},
    {	"tell",			TRUE,	FALSE,	FALSE	},
    {	"time",			TRUE,	TRUE,  TRUE	},
    {	"reset",		FALSE,	FALSE,  TRUE	},
    {	"ask",			TRUE,	FALSE,  FALSE	},
    {	"flagoff",		FALSE,	TRUE,	FALSE	},
    {	"use",			FALSE,	TRUE,	FALSE	},
    {	"objlock",		FALSE,	TRUE,	FALSE	},
    {	"objunlock",	FALSE,	TRUE,	FALSE	},
    {	"objclose",		FALSE,	TRUE,	FALSE	},
    {	"objopen",		FALSE,	TRUE,	FALSE	},
    {	"lock",			FALSE,	FALSE,	TRUE	},
    {	"unlock",		FALSE,	FALSE,	TRUE	},
    {	"open",			FALSE,	FALSE,	TRUE	},
    {	"close",		FALSE,	FALSE,	TRUE	},
    {	"tickrandom",	FALSE,	FALSE,	TRUE	},
    {	"onload",		TRUE,	TRUE,	FALSE	},
    {	"onehit",		TRUE,	TRUE,	FALSE	},
    {	"hit",			TRUE,	TRUE,	FALSE	},
    {	"unequip",		FALSE,  TRUE,	FALSE	},
    {	"prewear",		FALSE,	TRUE,	FALSE	},
    {	"preremove",	FALSE,	TRUE,	FALSE	},
    {	"mount",    	TRUE,	FALSE,	FALSE	},
    {	"dismount", 	TRUE,	FALSE,	FALSE	},
    {	"precommand", 	TRUE,	TRUE,	FALSE	},
    {	"stand",		FALSE,	FALSE,	TRUE	},
    {	"horn",			FALSE,	TRUE,	FALSE	},
    {	"preget",		FALSE,	TRUE,	FALSE	},
    {	"knock",		FALSE,	FALSE,	TRUE	},
    {	NULL,			FALSE,	FALSE,	TRUE	}
};

const char * fn_keyword[] =
{
    "rand",		/* if rand 30		- if random number < 30 */
    "mobhere",		/* if mobhere fido	- is there a 'fido' here */
    "objhere",		/* if objhere bottle	- is there a 'bottle' here */
			/* if mobhere 1233	- is there mob vnum 1233 here */
			/* if objhere 1233	- is there obj vnum 1233 here */
    "mobexists",	/* if mobexists fido	- is there a fido somewhere */
    "objexists",	/* if objexists sword	- is there a sword somewhere */

    "people",		/* if people > 4	- does room contain > 4 people */
    "players",		/* if players > 1	- does room contain > 1 pcs */
    "mobs",		/* if mobs > 2		- does room contain > 2 mobiles */
    "clones",		/* if clones > 3	- are there > 3 mobs of same vnum here */
    "order",		/* if order == 0	- is mob the first in room */
    "hour",		/* if hour > 11		- is the time > 11 o'clock */


    "ispc",		/* if ispc $n 		- is $n a pc */
    "isnpc",		/* if isnpc $n 		- is $n a mobile */
    "isgood",		/* if isgood $n 	- is $n good */
    "isevil",		/* if isevil $n 	- is $n evil */
    "isneutral",	/* if isneutral $n 	- is $n neutral */
    "isimmort",		/* if isimmort $n	- is $n immortal */
    "ischarm",		/* if ischarm $n	- is $n charmed */
    "isfollow",		/* if isfollow $n	- is $n following someone */
    "isactive",		/* if isactive $n	- is $n's position > SLEEPING */
    "isdelay",		/* if isdelay $i	- does $i have mobprog pending */
    "isvisible",	/* if isvisible $n	- can mob see $n */
    "hastarget",	/* if hastarget $i	- does $i have a valid target */
    "istarget",		/* if istarget $n	- is $n mob's target */
    "exists",		/* if exists $n		- does $n exist somewhere */

    "affected",		/* if affected $n blind - is $n affected by blind */
    "act",		/* if act $i sentinel	- is $i flagged sentinel */
    "off",              /* if off $i berserk	- is $i flagged berserk */
    "imm",              /* if imm $i fire	- is $i immune to fire */
    "carries",		/* if carries $n sword	- does $n have a 'sword' */
			/* if carries $n 1233	- does $n have obj vnum 1233 */
    "wears",		/* if wears $n lantern	- is $n wearing a 'lantern' */
			/* if wears $n 1233	- is $n wearing obj vnum 1233 */
    "has",    		/* if has $n weapon	- does $n have obj of type weapon */
    "uses",		/* if uses $n armor	- is $n wearing obj of type armor */
    "name",		/* if name $n puff	- is $n's name 'puff' */
    "pos",		/* if pos $n standing	- is $n standing */
    "clan",		/* if clan $n 'whatever'- does $n belong to clan 'whatever' */
    "race",		/* if race $n dragon	- is $n of 'dragon' race */
    "class",		/* if class $n mage	- is $n's class 'mage' */
    "objtype",		/* if objtype $p scroll	- is $p a scroll */

    "vnum",		/* if vnum $i == 1233  	- virtual number check */
    "hpcnt",		/* if hpcnt $i > 30	- hit point percent check */
    "room",		/* if room $i == 1233	- room virtual number */
    "sex",		/* if sex $i == 0	- sex check */
    "level",		/* if level $n < 5	- level check */
    "align",		/* if align $n < -1000	- alignment check */
    "money",		/* if money $n */
    "objval0",		/* if objval0 > 1000 	- object value[] checks 0..4 */
    "objval1",
    "objval2",
    "objval3",
    "objval4",
    "grpsize",		/* if grpsize $n > 6	- group size check */
    "hasflag",          /* if hasflag $n costam - dynamiczne flagi*/
    "statstr",		/*stats and stuff*/
    "statint", 		/* if statint $n > 300 */
    "statwis",
    "statdex",
    "statcon",
    "exithere",
    "roomhasflag",
    "objflag",
    "roomflag",
    "charflag",
    "height",
    "weight",
    "magspec",
    "dayofweek",	/* if dayofweek == 1  - if today is the first day of week */
    "dayofmonth",	/* if dayofmonth == 20  - if today is 20th day of month */
    "month",		/* if month < 3  - if sctual month is first or second */
    "nameaffected",	/* do specjalnych affectow nadawanych z progow, reserved*/
    "isdarkhere",	// czy w roomie jest ciemno
    "sector",		// if sector $n pole - czy sector lokacji w ktorej stoi $n to pole
    "ismounted",	// if ismounted $n - czy mob $n jest dosiadany przez kogos
    "isriding",		// if isriding $n - czy $n jedzie na jakims koniu
    "wearsarte",	// if wearsarte $n vnum - czy ktos ma zlozony artefact o vnumie
    "hasarte",		// if hasarte $n vnum - czy ma wogole artefact (inv+equ)
    "canloadarte",	// if canloadarte vnum - czy mozna jeszcze zaladowac taki artefact(nie przekroczony limit
    "response",		// if response 1 - czy odpowiedz na responsedelay byla pierwsza z mozliwych (0 - minal czas odpowiedzi)
    "ishere",		// if ishere $n - czy $n jest w tym samym miejscu co mob
    "statcha",		//
    "statluc",		//
    "liquid",		//if liquid $p woda - czy w pojemniku jest jakakolwiek woda
    "movecnt",		/* if movecnt $i > 30	- move percent check */
    "move",		/* if move $i > 30	- move check */
    "objval5",
    "objval6",
    "objstcost", 	/* if objstcost $o > 5, sprawdzanie kosztu przedmiotu */
    "objstcondition",	/* if objstcondition $o < 55 , sprawdzanie condition przedmiotu */
    "speaking",		/* if speaking $n elfi - czy ma aktualnie speaking ustawiony na elfi */
    "lang",		/* if lang $n elfi - czy zna jêzyk elfi */
    "numeval",		/* if numeval 1 > 0 - czy 1 wieksze od 0 */
    				/* if numeval $_int > 0 - czy zmienna int wieksza od 0 */
    "streval",		/* if streval $_str eq "bleh" - czy zmienna str to ci±g "bleh" */
    				/* if streval $_str1 eq $_str2 - czy zmienna str1 jest rowna zmiennej str2 */
					/* if streval $_str pre "bleh" - czy zmienna str i ci±g "bleh" maj± wspólny poczatek lub s± rowne */
    				/* if streval $_str in "bleh" - czy w zmiennej str znajduje sie ci±g "bleh" */
    				/* if streval $_str reg "jakis regexp" - czy zmienna str pasuje do regexpa */
    "varisnull",		/* if varisnull $_room1 - czy zmienne room1 jest null */
	"ingroup",		/* if is in the group with $n */
	"canmove",		/* if canmove $n - czy $n mo¿e siê poruszaæ */
    "isdooropen",	/* if door <direction> is open */
    "isdoorlocked",	/* if door <direction> is closed */
    "isglobalflag",	/* if isglobalflag bleble - czy istnieje globalna flaga bleble */
    "globalflag",	/* if globalflag bleble > 10 - czy globalna flaga bleble ma warto¶c wiêksz± od 10 */
    "age",			/* if age $n > 10 - czy ma wiecej niz 10 lat */
    "glory",		/* if glory $n > 10 - czy ma wiecej niz 10 punktow chwaly */
    "hasqlog",		/* if hasqlog $n name - czy $n ma questloga o nazwie name */
    "iscarried",	/* if iscarried $o - czy $o jest noszone przez kogokolwiek (inv lub eq) */
     "isburnproof",	/* if isburnproof $o - czy $o jest chronione przeciw spaleniu (inv lub eq) */
	"\n"		/* Table terminator */
};

const char *fn_evals[] =
{
	"==",
	">=",
	"<=",
	">",
	"<",
	"!=",
	"\n"
};

const char *strfn_evals[] =
{
	"eq",
	"pre",
	"in",
	"reg",
	"\n"
};

const	struct	obj_cmd_type	obj_cmd_table	[] =
{
	{	"setflag",			do_osetflag			},
	{	"setobj",			do_osetobj			},
	{	"modflag",			do_omodflag			},
	{	"string",			do_ostring			},
	{	"remove",			do_oremove			},
	{	"unwear",			do_objunwear		},
	{	"wear",				do_objwear			},
	{	"substitute",		do_objsubstitute	},
	{	"",					0					}
};

const	struct	mob_cmd_type	mob_cmd_table	[] =
{
	{	"asound",			do_mpasound			},
	{	"gecho",			do_mpgecho			},
	{	"zecho",			do_mpzecho			},
	{	"kill",				do_mpkill			},
	{	"assist",			do_mpassist			},
	{	"junk",				do_mpjunk			},
	{	"echo",				do_mpecho			},
	{	"echoaround",		do_mpechoaround		},
	{	"echoat",			do_mpechoat			},
	{	"mload",			do_mpmload			},
	{	"oload",			do_mpoload			},
	{	"purge",			do_mppurge			},
	{	"goto",				do_mpgoto			},
	{	"at",				do_mpat				},
	{	"transfer",			do_mptransfer		},
	{	"gtransfer",		do_mpgtransfer		},
	{	"otransfer",		do_mpotransfer		},
	{	"force",			do_mpforce			},
	{	"gforce",			do_mpgforce			},
	{	"vforce",			do_mpvforce			},
	{	"cast",				do_mpcast			},
	{	"damage",			do_mpdamage			},
	{	"remember",			do_mpremember		},
	{	"forget",			do_mpforget			},
	{	"delay",			do_mpdelay			},
	{	"cancel",			do_mpcancel			},
	{	"call",				do_mpcall			},
	{	"flee",				do_mpflee			},
	{	"remove",			do_mpremove			},
	{	"setflag",			do_mpsetflag		},
	{	"removeflag",		do_mpremoveflag		},
	{	"equip",			do_mpequip			},
	{	"reward",			do_mpreward			},
	{	"lock",				do_mplock			},
	{	"unlock",			do_mpunlock			},
	{	"open",				do_mpopen			},
	{	"close",			do_mpclose			},
	{	"setrent",			do_mpsetrent		},
	{	"clearmem",			do_mpclrmem			},
	{	"sethhf",			do_mpsethhf			},
	{	"exit",				do_mpexit			},
	{	"setaff",			do_mpsetaff			},
	{	"mobstring",		do_mpmobstring		},
	{	"objstring",		do_mpobjstring		},
	{	"addaff",			do_mpaddaff			},
	{	"modflag",			do_mpmodflag		},
	{	"roomdesc",			do_mproomdesc		},
	{	"mesg",				do_mpmesg			},
	{	"onehitmod",		do_mponehit			},
	{	"addgroup",			do_mpaddgroup		},
	{	"damageex",			do_mpdamageex		},
	{	"delaff",			do_mpdelaff			},
	{	"applypoison",		do_mpapplypoison	},
	{	"aload",			do_mpaload			},
	{	"greward",			do_mpgreward		},
	{	"log",				do_mplog			},
	{	"say",				do_mpsay			},
	{	"command",			do_mpcommand		},
	{	"walk",				do_mpwalk			},
	{	"sayto",			do_mpsayto			},
	{	"setpos",			do_mpsetpos			},
	{	"setposdefault",	do_mpsetposdefaul	},
	{	"peace",			do_mppeace			},
//	{	"smoke",			do_mpsmoke			},
	{	"questlog",			do_mpquestlog		},
	{	"",					0					}
};

const	struct	trigger_to_pos_type	trigger_to_pos_table	[] =
{
	{	&TRIG_ACT,			FP_ACTIVE				},
	{	&TRIG_SPEECH,		FP_ACTIVE				},
	{	&TRIG_RANDOM,		FP_ACTIVE				},
	{	&TRIG_FIGHT,		FP_FIGHTING				},
	{	&TRIG_HPCNT,		FP_FIGHTING				},
	{	&TRIG_DEATH,		FP_ALL					},
	{	&TRIG_ENTRY,		FP_ACTIVE				},
	{	&TRIG_GREET,		FP_ACTIVE				},
	{	&TRIG_GRALL,		FP_ALL					},
	{	&TRIG_GIVE,			FP_ACTIVE				},
	{	&TRIG_BRIBE,		FP_ACTIVE				},
	{	&TRIG_KILL,			FP_ALL					},
	{	&TRIG_DELAY,		FP_ALL					},
	{	&TRIG_SURR,			FP_FIGHTING				},
	{	&TRIG_EXIT,			FP_ACTIVE				},
	{	&TRIG_EXALL,		FP_ACTIVE				},
	{	&TRIG_TELL,			FP_ACTIVE				},
	{	&TRIG_TIME,			FP_ACTIVE|FP_SLEEPING	},
	{	&TRIG_ASK,			FP_ACTIVE				},
	{	&TRIG_ONLOAD,		FP_ALL					},
	{	&TRIG_ONEHIT,		FP_FIGHTING				},
	{	&TRIG_HIT,			FP_FIGHTING				},
	{	&TRIG_MOUNT,		FP_STANDING				},
	{	&TRIG_DISMOUNT,		FP_ALL					},
	{	&TRIG_PRECOMMAND,	FP_ALL					},
	{	NULL,				0						},
};

int mprog_trigger_default_pos( BITVECT_DATA * type )
{
	int trig;

	for( trig = 0; trigger_to_pos_table[trig].trigger; trig++ )
	{
		if ( trigger_to_pos_table[trig].trigger == type )
			return trigger_to_pos_table[trig].position;
	}

    return FP_STANDING;
}

void init_supermob( void )
{
	ROOM_INDEX_DATA * office;

	supermob = create_mobile( get_mob_index( 3 ) );
	supermob->invis_level = 41;
	office = get_room_index ( 3 );
	char_to_room( supermob, office );
}

void set_supermob( OBJ_DATA *obj )
{
	ROOM_INDEX_DATA * room;
	OBJ_DATA *in_obj;

	if ( !supermob )
		init_supermob();


	if ( !obj )
		return ;

	for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj )
		;

	if ( in_obj->carried_by )
		room = in_obj->carried_by->in_room;
	else
		room = in_obj->in_room;

	if ( !room )
		return ;

	free_string( supermob->short_descr );
	supermob->short_descr = str_dup( obj->short_descr );

	char_from_room ( supermob );
	char_to_room( supermob, room );
	supermob->prog_target = obj->prog_target;
	supermob->invis_level = 0;
}

void rset_supermob( ROOM_INDEX_DATA *room )
{
	if ( !supermob )
		init_supermob();

	if ( room )
	{
		free_string( supermob->short_descr );
		supermob->short_descr = str_dup( room->name );

		char_from_room ( supermob );
		char_to_room( supermob, room );

		supermob->invis_level = 0;
		supermob->prog_target = room->prog_target;
	}
	return ;
}

void pset_supermob( CHAR_DATA *ch )
{
    if ( !supermob ) init_supermob();

    if ( ch )
    {
        char_from_room ( supermob );
        char_to_room( supermob, ch->in_room );
        return ;
    }
    return ;
}

void release_supermob( void )
{
	if ( running_prog_env
	&& ( ( running_prog_env->obj && running_prog_env->type == PROG_TYPE_OBJ )
	  || ( running_prog_env->room && running_prog_env->type == PROG_TYPE_ROOM ) ) )
	{
		if ( running_prog_env->type == PROG_TYPE_OBJ )
		{
			free_string( supermob->short_descr );
			supermob->short_descr = str_dup( running_prog_env->obj->short_descr );

			if ( running_prog_env->obj->in_room )
			{
				char_from_room ( supermob );
				char_to_room( supermob, running_prog_env->obj->in_room );
			}
			supermob->prog_target = running_prog_env->obj->prog_target;
			supermob->invis_level = 0;
		}
		else
		{
			free_string( supermob->short_descr );
			supermob->short_descr = str_dup( running_prog_env->room->name );

			char_from_room ( supermob );
			char_to_room( supermob, running_prog_env->room );
			supermob->prog_target = running_prog_env->room->prog_target;
			supermob->invis_level = 0;
		}
	}
	else
	{
		supermob->invis_level = 41;
		char_from_room( supermob );
		char_to_room( supermob, get_room_index( 3 ) );
	}
}

bool check_trigger( int prog_type, char *name )
{
	int x;
	for ( x = 0; prog_trigs[ x ].name != NULL; x++ )
	{
		if ( FAST_STR_CMP( prog_trigs[ x ].name, name )
		     && ( ( prog_type == 0 && prog_trigs[ x ].mtrig )
		          || ( prog_type == 1 && prog_trigs[ x ].otrig )
		          || ( prog_type == 2 && prog_trigs[ x ].rtrig ) ) )
			return TRUE;
	}

	return FALSE;
}

/* Obs³uga MEMDAT */

/* Inicjalizacja nowego memdata - NIE DO£¡CZA DO LISTY memdat_list */
MEMDAT *new_mdat( void )
{
    MEMDAT * mdat;

	CREATE( mdat, MEMDAT, 1 );

    mdat->next      = NULL;
	mdat->prog_env	= NULL;
    mdat->response	= str_dup( "" );
    return mdat;
}

/* Zwolnienie memdata - USUWA Z LISTY memdat_list */
void free_mdat( MEMDAT * mdat )
{
	if ( !mdat || !memdat_list )
		return;

	if ( mdat == memdat_list )
	{
		memdat_list = mdat->next;
	}
	else
	{
		MEMDAT * prev;

		for ( prev = memdat_list; prev != NULL; prev = prev->next )
		{
			if ( prev->next == mdat )
			{
				prev->next = mdat->next;
				break;
			}
		}
	}

	free_string( mdat->response );
    DISPOSE( mdat );
}

MEMDAT * get_mob_memdat( CHAR_DATA * ch, CHAR_DATA *vch, char * prog_name, int type )
{
	MEMDAT * mdat;

	if ( !memdat_list || ch->has_memdat <= 0 )
		return NULL;

	for ( mdat = memdat_list; mdat; mdat = mdat->next )
	{
		if ( ( mdat->type == type || type == MEMDAT_ANY )
		&& mdat->prog_env && mdat->prog_env->ch == ch
		&& ( !vch || mdat->prog_env->vch == vch )
		&& ( IS_NULLSTR( prog_name ) || FAST_STR_CMP( mdat->prog_env->name, prog_name ) ) )
			return mdat;
	}

	return NULL;
}

MEMDAT * get_obj_memdat( OBJ_DATA * obj, CHAR_DATA * vch, char * prog_name, int type )
{
	MEMDAT * mdat;

	if ( !memdat_list || obj->has_memdat <= 0 )
		return NULL;

	for ( mdat = memdat_list; mdat; mdat = mdat->next )
	{
		if ( ( mdat->type == type || type == MEMDAT_ANY ) &&
		mdat->prog_env && mdat->prog_env->obj == obj &&
		( !vch || mdat->prog_env->vch == vch )
		&& ( IS_NULLSTR( prog_name ) || FAST_STR_CMP( mdat->prog_env->name, prog_name ) ) )
			return mdat;
	}

	return NULL;
}

MEMDAT * get_room_memdat( ROOM_INDEX_DATA * room, CHAR_DATA * vch, char * prog_name, int type )
{
	MEMDAT * mdat;

	if ( !memdat_list || room->has_memdat <= 0 )
		return NULL;

	for ( mdat = memdat_list; mdat; mdat = mdat->next )
	{
		if ( ( mdat->type == type || type == MEMDAT_ANY ) &&
		mdat->prog_env && mdat->prog_env->room == room &&
		( !vch || mdat->prog_env->vch == vch )
		&& ( IS_NULLSTR( prog_name ) || FAST_STR_CMP( mdat->prog_env->name, prog_name ) ) )
			return mdat;
	}

	return NULL;
}


GOTO_DATA *new_goto_data( void )
{
    GOTO_DATA *new_goto;
    int i;

    CREATE( new_goto, GOTO_DATA, 1 );

    new_goto->name       = str_dup("");
    new_goto->code_start = NULL;
    new_goto->level      = 0;

    for( i = 0; i < MAX_NESTED_LEVEL; i++ )
    {
	    new_goto->state[i] = -1;
   	    new_goto->cond[i]  = TRUE;
    }

    return new_goto;
}

void free_goto( GOTO_DATA *del_goto )
{
    if ( !del_goto )
        return;

	free_string( del_goto->name );
	DISPOSE( del_goto );

    return;
}

GOTO_DATA * find_goto( GOTO_DATA * gotos[], char * name )
{
	int i;
	for ( i = 0; i < MAX_GOTO; i++ )
	{
		if ( gotos[ i ] && gotos[ i ]->name[0] == name[0] && !str_cmp( gotos[ i ]->name, name ) )
			return gotos[ i ];
	}
	return NULL;
}

bool add_goto( GOTO_DATA * gotos[], GOTO_DATA * addgoto )
{
	int i;

	for ( i = 0; i < MAX_GOTO; i++ )
	{
		if ( !gotos[ i ] )
		{
			gotos[ i ] = addgoto;
			return TRUE;
		}
	}
	return FALSE;
}

GOTO_DATA * del_goto( GOTO_DATA * gotos[], char * name )
{
	int i;
	GOTO_DATA * result;

	for ( i = 0; i < MAX_GOTO; i++ )
	{
		if ( gotos[ i ] && gotos[ i ]->name[0] == name[0] && !str_cmp( gotos[ i ]->name, name ) )
		{
			result = gotos[ i ];
			gotos[ i ] = NULL;
			return result;
		}
	}
	return NULL;
}

/* Obs³uga PROG_ENV */

PROG_ENV* new_prog_env( void )
{
	PROG_ENV * penv;
	int i;

	CREATE( penv, PROG_ENV, 1 );

	penv->next			= NULL;
	penv->name 			= &str_empty[0];
	penv->source_ptr	= NULL;
	penv->ch			= NULL;
	penv->obj			= NULL;
	penv->room			= NULL;
	penv->vch			= NULL;
	penv->rch			= NULL;
	penv->arg1			= NULL;
	penv->arg2			= NULL;
	penv->type			= 0;
	penv->trigger		= NULL;
	penv->trig_param	= &str_empty[0];
	penv->goto_call		= 0;
	penv->vars			= NULL;
	penv->options		= 0;
	penv->other			= NULL;

	for ( i = 0; i < MAX_GOTO; i++ )
		penv->gotos[ i ] = NULL;

    for( i = 0; i < MAX_NESTED_LEVEL; i++ )
    {
	    penv->state[i]	= IN_BLOCK;
   	    penv->cond[i]	= TRUE;
    }

    penv->level		= 0;

	return penv;
}

void free_prog_env( PROG_ENV* penv )
{
	int i;

	if ( !penv )
		return;

	free_string( penv->name );
	free_string( penv->trig_param );

	if ( IS_SET( penv->options, PROG_OPT_VARCOPY ) || !penv->other )
		free_var_list( &penv->vars );
	else
	{
		if ( penv->other && penv->other != penv->vars )
			partial_free_var_list( &penv->vars, ( PROG_VAR * ) penv->other );
		penv->vars = NULL;
	}

	for ( i = 0; i < MAX_GOTO; i++ )
	{
		if ( penv->gotos[ i ] )
			free_goto( penv->gotos[ i ] );
	}

	if ( penv == running_prog_env )
	{
		running_prog_env = penv->next;
	}
	else
	{
		PROG_ENV * prev;

		for ( prev = running_prog_env; prev != NULL; prev = prev->next )
		{
			if ( prev->next == penv )
			{
				prev->next = penv->next;
				break;
			}
		}
	}

	DISPOSE( penv );
	return;
}

int count_running_progs( void )
{
	PROG_ENV * penv;
	int result = 0;

	for ( penv = running_prog_env; penv; penv = penv->next, result++ ) ;

	return result;
}

void add_prog_env( PROG_ENV * prog_env )
{
	prog_env->next		= running_prog_env;
	running_prog_env	= prog_env;
	return;
}

void create_mprog_env( char * name, char * source_ptr, CHAR_DATA * ch, CHAR_DATA * vch,
					   void * arg1, void * arg2, BITVECT_DATA * trigger, char * trig_param )
{
	PROG_ENV * prog_env;

	prog_env = new_prog_env( );

	free_string( prog_env->name );
	prog_env->name			= str_dup( name );
	prog_env->source_ptr	= source_ptr;
	prog_env->ch			= ch;
	prog_env->vch			= vch;
	prog_env->rch			= get_random_char( prog_env->ch, TRUE );
	prog_env->arg1			= arg1;
	prog_env->arg2			= arg2;
	prog_env->type			= PROG_TYPE_MOB;
	prog_env->trigger		= trigger;
	if ( IS_NULLSTR( trig_param ) )
		prog_env->trig_param	= str_dup( "" );
	else
	{
		prog_env->trig_param	= str_dup( trig_param );
		var_to_list( new_prog_var( "0", trig_param, PROG_VAR_STRING ), &prog_env->vars );
	}

	prog_env->next			= running_prog_env;
	running_prog_env		= prog_env;
	return;
}

void create_oprog_env( char * name, char * source_ptr, OBJ_DATA * obj, CHAR_DATA * vch,
					   void * arg1, void * arg2, BITVECT_DATA * trigger, char * trig_param )
{
	PROG_ENV * prog_env;

	prog_env = new_prog_env( );

	free_string( prog_env->name );
	prog_env->name			= str_dup( name );
	prog_env->source_ptr	= source_ptr;
	prog_env->ch			= supermob;
	prog_env->obj			= obj;
	prog_env->vch			= vch;
	prog_env->rch			= get_random_char( prog_env->ch, TRUE );
	prog_env->arg1			= arg1;
	prog_env->arg2			= arg2;
	prog_env->type			= PROG_TYPE_OBJ;
	prog_env->trigger		= trigger;
	if ( IS_NULLSTR( trig_param ) )
		prog_env->trig_param	= str_dup( "" );
	else
	{
		prog_env->trig_param	= str_dup( trig_param );
		var_to_list( new_prog_var( "0", trig_param, PROG_VAR_STRING ), &prog_env->vars );
	}

	prog_env->next			= running_prog_env;
	running_prog_env		= prog_env;
	return;
}

void create_rprog_env( char * name, char * source_ptr, ROOM_INDEX_DATA * room, CHAR_DATA * vch,
					   void * arg1, void * arg2, BITVECT_DATA * trigger, char * trig_param )
{
	PROG_ENV * prog_env;

	prog_env = new_prog_env( );

	free_string( prog_env->name );
	prog_env->name			= str_dup( name );
	prog_env->source_ptr	= source_ptr;
	prog_env->ch			= supermob;
	prog_env->room			= room;
	prog_env->vch			= vch;
	prog_env->rch			= get_random_char( prog_env->ch, TRUE );
	prog_env->arg1			= arg1;
	prog_env->arg2			= arg2;
	prog_env->type			= PROG_TYPE_ROOM;
	prog_env->trigger		= trigger;
	if ( IS_NULLSTR( trig_param ) )
		prog_env->trig_param	= str_dup( "" );
	else
	{
		prog_env->trig_param	= str_dup( trig_param );
		var_to_list( new_prog_var( "0", trig_param, PROG_VAR_STRING ), &prog_env->vars );
	}

	prog_env->next			= running_prog_env;
	running_prog_env		= prog_env;
	return;
}

void bug_prog_format( char * fmt, ...)
{
#ifdef PROG_LOGS
	char buf [2*MSL];
	char buf2 [2*MSL];
	extern int port;
	va_list args;

	va_start (args, fmt);
	vsprintf (buf, fmt, args);
	va_end (args);

	switch ( running_prog_env->type )
	{
		case PROG_TYPE_MOB:
			sprintf( buf2, "[*****] BUG MOBProg '%s' executed by %d: %s",
					 running_prog_env->name,
					 running_prog_env->ch && IS_NPC( running_prog_env->ch ) ? running_prog_env->ch->pIndexData->vnum : 0,
					 buf );
			break;
		case PROG_TYPE_OBJ:
			sprintf( buf2, "[*****] BUG OBJProg '%s' executed on %d: %s",
					 running_prog_env->name,
					 running_prog_env->obj ? running_prog_env->obj->pIndexData->vnum : 0,
					 buf );
			break;
		case PROG_TYPE_ROOM:
			sprintf( buf2, "[*****] BUG ROOMProg '%s' executed in %d: %s",
					 running_prog_env->name,
					 running_prog_env->room ? running_prog_env->room->vnum : 0,
					 buf );
			break;
		default:
			return;
	}

	log_string( buf2 );
	if ( port == 3000 )
		do_mpecho( buf2 );
#endif
	return;
}

/*
 * Return a valid keyword from a keyword table
 */
int keyword_lookup( const char **table, char *keyword )
{
    register int i;
    for( i = 0; table[i][0] != '\n'; i++ )
        if( !str_cmp( table[i], keyword ) )
            return( i );
    return -1;
}

/*
 * Perform numeric evaluation.
 * Called by cmd_eval()
 */
bool num_eval( int lval, int oper, int rval )
{
    switch( oper )
    {
        case EVAL_EQ:
             return ( lval == rval );
        case EVAL_GE:
             return ( lval >= rval );
        case EVAL_LE:
             return ( lval <= rval );
        case EVAL_NE:
             return ( lval != rval );
        case EVAL_GT:
             return ( lval > rval );
        case EVAL_LT:
             return ( lval < rval );
    }
	bug( "num_eval: invalid oper", 0 );
	return FALSE;
}

/*
 * Perform string evaluation.
 * Called by cmd_eval()
 */
bool str_eval( char * strlval, int oper, char * strrval )
{
	char buf [ MAX_STRING_LENGTH ];
    switch( oper )
    {
        case STREVAL_EQ:
             return !str_cmp( strlval, strrval );
        case STREVAL_PRE:
             return !str_prefix( strlval, strrval );
        case STREVAL_IN:
             return !str_infix( strlval, strrval );
        case STREVAL_REG:
        	 sprintf( buf, "%s\n", strlval );
             return regexp_match( strrval, buf );
    }
    bug( "str_eval: invalid oper", 0 );
    return FALSE;
}

/*
 * ---------------------------------------------------------------------
 * UTILITY FUNCTIONS USED BY CMD_EVAL()
 * ----------------------------------------------------------------------
 */

/*
 * Get a random char in the room (for $r parameter)
 */
CHAR_DATA *get_random_char( CHAR_DATA *mob, bool PC )
{
    CHAR_DATA *vch, *victim = NULL;
    int now = 0, highest = 0;
    for( vch = mob->in_room->people; vch; vch = vch->next_in_room )
    {
        if ( mob == vch  || !can_see( mob, vch ) ) continue;
        if ( PC && IS_NPC( vch ) ) continue;

        if ( ( now = number_percent() ) > highest )
        {
            victim = vch;
            highest = now;
        }
    }
    return victim;
}

/*
 * How many other players / mobs are there in the room
 * iFlag: 0: all, 1: players, 2: mobiles 3: mobs w/ same vnum 4: same group
 */
int count_people_room( CHAR_DATA *mob, int iFlag )
{
    CHAR_DATA *vch;
    int count;
    for ( count = 0, vch = mob->in_room->people; vch; vch = vch->next_in_room )
	if ( mob != vch
	&&   (iFlag == 0
	  || (iFlag == 1 && !IS_NPC( vch ))
	  || (iFlag == 2 && IS_NPC( vch ))
	  || (iFlag == 3 && IS_NPC( mob ) && IS_NPC( vch )
	     && mob->pIndexData->vnum == vch->pIndexData->vnum )
	  || (iFlag == 4 && is_same_group( mob, vch )) )
	&& can_see( mob, vch ) )
	    count++;
    return ( count );
}

/*
 * Get the order of a mob in the room. Useful when several mobs in
 * a room have the same trigger and you want only the first of them
 * to act
 */
int get_order( CHAR_DATA *ch )
{
    CHAR_DATA *vch;
    int i;

    if ( !IS_NPC(ch) )
	return 0;
    for ( i = 0, vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
	if ( vch == ch )
	    return i;
	if ( IS_NPC(vch)
	&&   vch->pIndexData->vnum == ch->pIndexData->vnum )
	    i++;
    }
    return 0;
}

/*
 * Check if ch has a given item or item type
 * vnum: item vnum or 0
 * item_type: item type or 0
 * fWear: TRUE: item must be worn, FALSE: don't care
 */
bool has_item( CHAR_DATA *ch, ush_int vnum, sh_int item_type, bool fWear )
{
    OBJ_DATA *obj;
    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( ( vnum == 0 || obj->pIndexData->vnum == vnum )
	&&   ( item_type == 0 || obj->item_type == item_type )
	&&   ( !fWear || obj->wear_loc != WEAR_NONE ) )
	    return TRUE;
    return FALSE;
}

bool carries_item( CHAR_DATA *ch, ush_int vnum, sh_int item_type )
{
    OBJ_DATA *obj;
    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if ( ( vnum == 0 || obj->pIndexData->vnum == vnum )
	&&   ( item_type == 0 || obj->item_type == item_type )
	&&   (  obj->wear_loc == WEAR_NONE ))
	    return TRUE;
    return FALSE;
}

//czy ma zalozony artef
bool wears_arte(CHAR_DATA *ch, ush_int vnum)
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
	{
	if( obj->pIndexData->vnum == vnum && is_artefact(obj) )
	    {
	    if( obj->wear_loc != WEAR_NONE )
		return TRUE;
	    }

	}

    return FALSE;
}

//czy ma artef
bool has_arte(CHAR_DATA *ch, ush_int vnum)
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj; obj = obj->next_content )
	if( obj->pIndexData->vnum == vnum && is_artefact(obj) )
	    return TRUE;


    return FALSE;
}

//czy mozna zaladowac artefakt
bool arte_can_load (ush_int vnum)
{
    ARTEFACT_DATA *art = NULL;

    for ( art = artefact_system; art; art = art->next )
    {
        if ( art->avnum == vnum )
        {
            if(art->count >= art->max_count)
            return FALSE;
            else
            return TRUE;
        }
    }

    return FALSE;
}

/*
 * Check if there's a mob with given vnum in the room
 */
bool get_mob_vnum_room( CHAR_DATA *ch, ush_int vnum )
{
    CHAR_DATA *mob;
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	if ( IS_NPC( mob ) && mob->pIndexData->vnum == vnum )
	    return TRUE;
    return FALSE;
}

/*
 * Check if there's a mob with given vnum in the world
 */
bool get_mob_vnum_world( ush_int vnum )
{
    CHAR_DATA *mob;
	for ( mob = char_list; mob != NULL ; mob = mob->next )
	{
		if ( IS_NPC( mob ) && ( mob->pIndexData->vnum == vnum ) )
			return TRUE;
	}
    return FALSE;
}

/*
 * Check if there's an object with given vnum in the room
 */
bool get_obj_vnum_room( CHAR_DATA *ch, ush_int vnum )
{
    OBJ_DATA *obj;
    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
	if ( obj->pIndexData->vnum == vnum )
	    return TRUE;
    return FALSE;
}



/* ---------------------------------------------------------------------
 * CMD_EVAL
 * This monster evaluates an if/or/and statement
 * There are five kinds of statement:
 * 1) keyword and value (no $-code)	    if random 30
 * 2) keyword, comparison and value	    if people > 2
 * 3) keyword and actor		    	    if isnpc $n
 * 4) keyword, actor and value		    if carries $n sword
 * 5) keyword, actor/value and actor/value  if strcmp $_str1 $_str2
 * 6) keyword, actor, comparison and value  if level $n >= 10
 * 7) keyword, actor/value, comparison and actor/value  if numeval $_int >= 10
 *
 *----------------------------------------------------------------------
 */
int cmd_eval( char *line, int check )
{
    CHAR_DATA * lval_char;
    CHAR_DATA * mob = running_prog_env->ch;
    OBJ_DATA * lval_obj = NULL;
    AFFECT_DATA *pAff = NULL;
    PFLAG_DATA *tmp, *found;
    char *original, buf[ MAX_INPUT_LENGTH ], extr[ MAX_INPUT_LENGTH ];
    int lval = 0, oper = 0, rval = -1, door;
    char *strlval = NULL, *strrval = NULL;
    EXIT_DATA *pexit;
    int sn = 0;
    original = line;
    line = one_argument( line, buf );

    //bezargumentowy check...
    if ( check == CHK_ISDARKHERE )
        return room_is_dark( NULL, mob->in_room );

    if ( buf[ 0 ] == '\0' || mob == NULL )
        return FALSE;

    switch ( check )
    {
            /*
             * Case 1: keyword and value
             */
        case CHK_RESPONSE:
            return ( atoi( buf ) == mob->prog_response );
        case CHK_RAND:
            return ( atoi( buf ) < number_percent() );
        case CHK_MOBHERE:
            if ( is_number( buf ) )
                return ( get_mob_vnum_room( mob, atoi( buf ) ) );
            else
                return ( ( bool ) ( get_char_room( mob, buf ) != NULL ) );
        case CHK_OBJHERE:
            if ( is_number( buf ) )
                return ( get_obj_vnum_room( mob, atoi( buf ) ) );
            else
                return ( ( bool ) ( get_obj_here( mob, buf ) != NULL ) );
        case CHK_MOBEXISTS:
			if ( is_number ( buf ) )
				return get_mob_vnum_world( atoi( buf ) );
			else
				return ( ( bool ) ( get_char_world( mob, buf ) != NULL ) );
        case CHK_EXISTS:
            return ( ( bool ) ( get_char_world( mob, buf ) != NULL ) );
        case CHK_OBJEXISTS:
            return ( ( bool ) ( get_obj_world( mob, buf ) != NULL ) );

        /*
         * EXITS checks
         */
        case CHK_EXITHERE:
            door = get_exit_number(buf, mob);
            if ( door < 0 || door > 5 || !mob->in_room->exit[ door ] || !mob->in_room->exit[ door ] -> u1.to_room )
            {
                return FALSE;
            }
            return TRUE;

        case CHK_ISDOOROPEN:
            door = get_exit_number(buf, mob);
            if ( door < 0 || door > 5 || !mob->in_room->exit[ door ] || !mob->in_room->exit[ door ] -> u1.to_room )
            {
                return FALSE;
            }
		    pexit = mob->in_room->exit[ door ];
            if ( IS_SET( pexit->exit_info, EX_CLOSED) )
            {
                return FALSE;
            }
            return TRUE;

        case CHK_ISDOORLOCKED:
            door = get_exit_number(buf, mob);
            if ( door < 0 || door > 5 || !mob->in_room->exit[ door ] || !mob->in_room->exit[ door ] -> u1.to_room )
            {
                return FALSE;
            }
		    pexit = mob->in_room->exit[ door ];
			if ( IS_SET( pexit->exit_info, EX_LOCKED ) )
            {
                return TRUE;
            }
            return FALSE;

        case CHK_ROOMHASFLAG:
            {
                if ( !mob || !mob->in_room || !mob->in_room->pflag_list )
                    return FALSE;

                return flag_exists( buf, mob->in_room->pflag_list );
            }
        case CHK_ISGLOBALFLAG:
            {
                if ( !misc.global_flag_list )
                    return FALSE;

                return flag_exists( buf, misc.global_flag_list );
            }
        case CHK_ACANLOAD:
            if ( is_number( buf ) )
                return ( arte_can_load( atoi( buf ) ) );
            else return FALSE;

            /*
             * Case 2 begins here: We sneakily use rval to indicate need
             * 		       for numeric eval...
             */
        case CHK_PEOPLE:
            rval = count_people_room( mob, 0 ); break;
        case CHK_PLAYERS:
            rval = count_people_room( mob, 1 ); break;
        case CHK_MOBS:
            rval = count_people_room( mob, 2 ); break;
        case CHK_CLONES:
            rval = count_people_room( mob, 3 ); break;
        case CHK_ORDER:
            rval = get_order( mob ); break;
        case CHK_HOUR:
            rval = time_info.hour; break;
        case CHK_DAYOFWEEK:
            rval = ( time_info.day + 1 ) % 7 + 1; break;
        case CHK_DAYOFMONTH:
            rval = time_info.day	 + 1; break;
        case CHK_MONTH:
            rval = time_info.month + 1; break;
        case CHK_NUMEVAL:
        	if ( is_number( buf ) )
        		rval = atoi( buf );
        	else if ( IS_VAR( buf ) )
        	{
        		char *value = var_to_string( get_var( buf + 2, running_prog_env->vars ) );
        		if ( IS_NULLSTR( value ) )
        			return FALSE;
        		rval = atoi( value );
        	}
        	line = one_argument( line, buf );
        	break;
        case CHK_STREVAL:
        	if ( IS_VAR( buf ) )
        	{
        		char *value = var_to_string( get_var( buf + 2, running_prog_env->vars ) );
        		if ( IS_NULLSTR( value ) )
        			return FALSE;
        		strrval = str_dup( value );
        	}
        	else
        		strrval = str_dup( buf );
        	line = one_argument( line, buf );
        	break;
		case CHK_VAR_IS_NULL:
			if ( IS_VAR( buf ) )
			{
				return var_is_null( get_var( buf + 2, running_prog_env->vars ) );
			}
			else
				return TRUE;
			break;
        case CHK_GLOBALFLAG:
            for ( tmp = misc.global_flag_list; tmp; tmp = tmp->next )
            {
                if ( NOPOL( buf[ 0 ] ) == NOPOL( tmp->id[ 0 ] ) && !str_cmp( tmp->id, buf ) )
                {
                    found = tmp;
                    break;
                }
            }
            if ( found )
                rval = found->duration;
            else
                return FALSE;

        	line = one_argument( line, buf );
        	break;
        default: ;
    }

    /*
     * Case 2 continued: evaluate expression
     */
    if ( rval >= 0 )
    {
        if ( ( oper = keyword_lookup( fn_evals, buf ) ) < 0 )
        {
        	bug_prog_format( "cmd_eval: syntax error in ifcheck (wrong comparision operator)" );
            return FALSE;
        }
        one_argument( line, buf );

        lval = rval;

    	if ( is_number( buf ) )
    		rval = atoi( buf );
    	else if ( IS_VAR( buf ) )
    	{
    		char *value = var_to_string( get_var( buf + 2, running_prog_env->vars ) );
    		if ( IS_NULLSTR( value ) )
    			return FALSE;
    		rval = atoi( value );
    	}
    	else
    		return FALSE;

        return ( num_eval( lval, oper, rval ) );
    }

    if ( strrval != NULL )
    {
    	bool result;
        if ( ( oper = keyword_lookup( strfn_evals, buf ) ) < 0 )
        {
        	bug_prog_format( "cmd_eval: syntax error in ifcheck (wrong string comparision operator)" );
        	free_string( strrval );
            return FALSE;
        }
        case_one_argument( line, buf );

        strlval = strrval;

    	if ( IS_VAR( buf ) )
    	{
    		char *value = var_to_string( get_var( buf + 2, running_prog_env->vars ) );
    		if ( IS_NULLSTR( value ) )
    		{
    			free_string( strrval );
    			return FALSE;
    		}
    		strrval = str_dup( value );
    	}
    	else
	  		strrval = str_dup( buf );

        result = str_eval( strlval, oper, strrval );
        free_string( strrval );
        free_string( strlval );
        return result;
    }

    /*
     * Case 3,4,5: Grab actors from $* codes
     */
    if ( buf[ 0 ] != '$' || buf[ 1 ] == '\0' )
    {
    	bug_prog_format( "cmd_eval: syntax error in ifcheck (no actor - $)" );
        return FALSE;
    }

	lval_char = resolve_char_arg( buf );
	lval_obj = resolve_obj_arg( buf );

	if ( !lval_char && !lval_obj )
    {
    	bug_prog_format( "cmd_eval: syntax error in ifcheck (wrong actor - $)" );
        return FALSE;
    }

    /*
     * Case 3: Keyword, comparison and value
     */
    switch ( check )
    {
        case CHK_ISHERE:
            return ( lval_char != NULL && lval_char->in_room && mob->in_room && lval_char->in_room == mob->in_room );
        case CHK_ISPC:
            return ( lval_char != NULL && !IS_NPC( lval_char ) );
        case CHK_ISNPC:
            return ( lval_char != NULL && IS_NPC( lval_char ) );
        case CHK_ISGOOD:
            return ( lval_char != NULL && IS_GOOD( lval_char ) );
        case CHK_ISEVIL:
            return ( lval_char != NULL && IS_EVIL( lval_char ) );
		case CHK_INGROUP:
            return ( lval_char != NULL && is_same_group( lval_char, running_prog_env->vch ) );
        case CHK_ISNEUTRAL:
            return ( lval_char != NULL && IS_NEUTRAL( lval_char ) );
        case CHK_ISIMMORT:
            return ( lval_char != NULL && IS_IMMORTAL( lval_char ) );
        case CHK_ISCHARM:  /* A relic from MERC 2.2 MOBprograms */
            return ( lval_char != NULL && IS_AFFECTED( lval_char, AFF_CHARM ) );
        case CHK_ISFOLLOW:
            return ( lval_char != NULL && lval_char->master != NULL
                     && lval_char->master->in_room == lval_char->in_room );
        case CHK_ISACTIVE:
            return ( lval_char != NULL && lval_char->position > POS_SLEEPING );
        case CHK_ISDELAY:
            return ( lval_char != NULL && lval_char->prog_delay > 0 );
        case CHK_ISVISIBLE:
        	if ( lval_char )
        		return can_see( mob, lval_char );
        	else if ( lval_obj )
        		return can_see_obj( mob, lval_obj );
        	else
        		return FALSE;
        case CHK_HASTARGET:
            return ( lval_char != NULL && lval_char->prog_target != NULL
                     && lval_char->in_room == lval_char->prog_target->in_room );
        case CHK_ISTARGET:
            return ( lval_char != NULL && mob->prog_target == lval_char );
        case CHK_ISMOUNTED:
            return ( lval_char != NULL && lval_char->mounting != NULL );
        case CHK_ISRIDING:
            return ( lval_char != NULL && lval_char->mount != NULL );
		case CHK_CANMOVE:
            return ( lval_char != NULL && can_move( lval_char ) );
        case CHK_ISCARRIED:
            return ( lval_obj != NULL && lval_obj->carried_by != NULL );
            
         case CHK_ISBURNPROOF:
            return ( lval_obj != NULL && IS_OBJ_STAT( lval_obj, ITEM_BURN_PROOF ) );    
            
        default: ;
    }

    /*
     * Case 4: Keyword, actor and value
     */
    line = one_argument( line, buf );
    switch ( check )
    {
        case CHK_AFFECTED:
            sn = skill_lookup( buf ); 
            return ( lval_char != NULL
                     && (IS_AFFECTED( lval_char, *ext_flag_lookup( buf, affect_flags ) ) || ( is_affected( lval_char, sn ) ) ));
        case CHK_NAMEAFFECTED:

            if ( !lval_char )
                return FALSE;

            for ( pAff = lval_char->affected; pAff; pAff = pAff->next )
            {
                if ( !pAff || !pAff->info )
                    continue;

                if ( !str_cmp( pAff->info, buf ) )
                    return TRUE;
            }

            return FALSE;

		case CHK_ACT:
            return ( lval_char != NULL
                     && EXT_IS_SET( lval_char->act, *ext_flag_lookup( buf, act_flags ) ) );

        case CHK_IMM:
            return FALSE;

        case CHK_OFF:
            return ( lval_char != NULL
                     && EXT_IS_SET( lval_char->off_flags, *ext_flag_lookup( buf, off_flags ) ) );
        case CHK_CARRIES:
            if ( is_number( buf ) )
                return ( lval_char != NULL && has_item( lval_char, atoi( buf ), 0, FALSE ) );
            else
                return ( lval_char != NULL && ( get_obj_carry( lval_char, buf, lval_char ) != NULL ) );
        case CHK_WEARS:
            if ( is_number( buf ) )
                return ( lval_char != NULL && has_item( lval_char, atoi( buf ), 0, TRUE ) );
            else
                return ( lval_char != NULL && ( get_obj_wear( lval_char, buf, FALSE ) != NULL ) );
        case CHK_HAS:
            return ( lval_char != NULL && has_item( lval_char, 0, item_lookup( buf ), FALSE ) );
        case CHK_AWEARS:
            return ( lval_char != NULL && wears_arte( lval_char, atoi( buf ) ) );
        case CHK_AHAS:
            return ( lval_char != NULL && has_arte( lval_char, atoi( buf ) ) );

        case CHK_USES:
            return ( lval_char != NULL && has_item( lval_char, 0, item_lookup( buf ), TRUE ) );
        case CHK_NAME:
        	if ( lval_char )
        		return is_name( buf, lval_char->ss_data ? lval_char->short_descr : lval_char->name );
        	else if ( lval_obj )
        		return is_name( buf, lval_obj->name );
        	else
        		return FALSE;

        case CHK_HASFLAG:
        	if ( lval_char )
        		return flag_exists( buf, lval_char->pflag_list );
        	else if ( lval_obj )
        		return flag_exists( buf, lval_obj->pflag_list );
        	else
        		return FALSE;

        case CHK_POS:
            return ( lval_char != NULL && lval_char->position == position_lookup( buf ) );
        case CHK_CLAN:
            return ( lval_char != NULL && lval_char->pcdata && lval_char->pcdata->clan && !str_cmp( lval_char->pcdata->clan->who_name, buf ) );
        case CHK_RACE:
            return ( lval_char != NULL && GET_RACE(lval_char) == race_lookup( buf ) );
        case CHK_CLASS:
            return ( lval_char != NULL && lval_char->class == class_lookup( buf ) );
        case CHK_OBJTYPE:
            return ( lval_obj != NULL && lval_obj->item_type == item_lookup( buf ) );
        case CHK_SECTOR:
            return ( lval_char->in_room != NULL && !str_cmp( sector_table[ lval_char->in_room->sector_type ].name, buf ) );
        case CHK_LIQUID:
            return ( lval_obj != NULL && !str_cmp( liq_table[ lval_obj->value[ 2 ] ].liq_name, buf ) && lval_obj->value[ 1 ] > 0 );
        case CHK_SPEAKING:
            return ( lval_char != NULL
                     && lval_char->speaking == language_lookup( buf ) );
        case CHK_LANG:
            return ( lval_char != NULL
                     && knows_language( lval_char, language_lookup( buf ), lval_char ) );
        case CHK_HASQLOG:
            return ( lval_char != NULL
		     && has_qlog( lval_char, buf ) );
        default: ;
    }

    /*
     * Case 5: Keyword, actor, comparison and value
     */
    if ( check == CHK_OBJFLAG || check == CHK_ROOMFLAG || check == CHK_CHARFLAG )
    {
        strcpy( extr, buf );
        line = one_argument( line, buf );
    }

    if ( ( oper = keyword_lookup( fn_evals, buf ) ) < 0 )
    {
       	bug_prog_format( "cmd_eval: syntax error in ifcheck (wrong comparision operator)" );
        return FALSE;
    }
    line = one_argument( line, buf );
    rval = atoi( buf );

    switch ( check )
    {
        case CHK_VNUM:
        	if ( lval_char && IS_NPC( lval_char ) )
        		lval = lval_char->pIndexData->vnum;
        	else if ( lval_obj )
        		lval = lval_obj->pIndexData->vnum;
        	else
        		lval = 0;
            break;
        case CHK_HPCNT:
            if ( lval_char != NULL ) lval = ( lval_char->hit * 100 ) / ( UMAX( 1, get_max_hp(lval_char) ) ); break;
        case CHK_MOVECNT:
            if ( lval_char != NULL ) lval = ( lval_char->move * 100 ) / ( UMAX( 1, lval_char->max_move ) ); break;
        case CHK_MOVE:
            if ( lval_char != NULL ) lval = lval_char->move;break;
        case CHK_ROOM:
            if ( lval_char != NULL && lval_char->in_room != NULL )
                lval = lval_char->in_room->vnum; break;
        case CHK_SEX:
            if ( lval_char != NULL ) lval = lval_char->sex; break;
        case CHK_LEVEL:
            if ( lval_char != NULL ) lval = lval_char->level; break;
        case CHK_ALIGN:
            if ( lval_char != NULL ) lval = lval_char->alignment; break;
        case CHK_AGE:
            if ( lval_char != NULL ) lval = lval_char->age; break;
        case CHK_GLORY:
            if ( lval_char != NULL ) lval = lval_char->glory; break;

        case CHK_STR:
            if ( lval_char != NULL ) lval = get_curr_stat_deprecated( lval_char, STAT_STR ); break;
        case CHK_INT:
            if ( lval_char != NULL ) lval = get_curr_stat_deprecated( lval_char, STAT_INT ); break;
        case CHK_CHA:
            if ( lval_char != NULL ) lval = get_curr_stat_deprecated( lval_char, STAT_CHA ); break;
        case CHK_LUC:
            if ( lval_char != NULL ) lval = get_curr_stat_deprecated( lval_char, STAT_LUC ); break;
        case CHK_WIS:
            if ( lval_char != NULL ) lval = get_curr_stat_deprecated( lval_char, STAT_WIS ); break;
        case CHK_DEX:
            if ( lval_char != NULL ) lval = get_curr_stat_deprecated( lval_char, STAT_DEX ); break;
        case CHK_CON:
            if ( lval_char != NULL ) lval = get_curr_stat_deprecated( lval_char, STAT_CON ); break;

        case CHK_HEIGHT:
            if ( lval_char != NULL ) lval = lval_char->height; break;
        case CHK_WEIGHT:
            if ( lval_char != NULL ) lval = lval_char->weight; break;
        case CHK_MAGSPEC:
            if ( lval_char != NULL && !IS_NPC( lval_char ) && lval_char->class == CLASS_MAG )
            {
                lval = lval_char->pcdata->mage_specialist;
                break;
            }
            else
                lval = -1;

            break;

        case CHK_MONEY:   /* Money is converted to silver... */
            if ( lval_char != NULL )
                lval = lval_char->silver; break;
        case CHK_OBJVAL0:
            if ( lval_obj != NULL ) lval = lval_obj->value[ 0 ]; break;
        case CHK_OBJVAL1:
            if ( lval_obj != NULL ) lval = lval_obj->value[ 1 ]; break;
        case CHK_OBJVAL2:
            if ( lval_obj != NULL ) lval = lval_obj->value[ 2 ]; break;
        case CHK_OBJVAL3:
            if ( lval_obj != NULL ) lval = lval_obj->value[ 3 ]; break;
        case CHK_OBJVAL4:
            if ( lval_obj != NULL ) lval = lval_obj->value[ 4 ]; break;
		case CHK_OBJVAL5:
            if ( lval_obj != NULL ) lval = lval_obj->value[ 5 ]; break;
		case CHK_OBJVAL6:
            if ( lval_obj != NULL ) lval = lval_obj->value[ 6 ]; break;
		case CHK_OBJSTCOST:
            if ( lval_obj != NULL ) lval = lval_obj->cost; break;
		case CHK_OBJSTCONDITION:
            if ( lval_obj != NULL ) lval = lval_obj->condition	; break;
        case CHK_GRPSIZE:
            if ( lval_char != NULL ) lval = count_people_room( lval_char, 4 ); break;
        case CHK_OBJFLAG:
            if ( lval_obj != NULL )
            {
                found = NULL;
                for ( tmp = lval_obj->pflag_list;tmp;tmp = tmp->next )
                    if ( NOPOL( extr[ 0 ] ) == NOPOL( tmp->id[ 0 ] ) && !str_cmp( tmp->id, extr ) )
                    {
                        found = tmp;
                        break;
                    }
                if ( found )
                    lval = found->duration;
                else
                    lval = -1;
            }
            break;
        case CHK_ROOMFLAG:
            if ( lval_char && lval_char->in_room )
            {
                found = NULL;
                for ( tmp = lval_char->in_room->pflag_list;tmp;tmp = tmp->next )
                    if ( NOPOL( extr[ 0 ] ) == NOPOL( tmp->id[ 0 ] ) && !str_cmp( tmp->id, extr ) )
                    {
                        found = tmp;
                        break;
                    }
                if ( found )
                    lval = found->duration;
                else
                    lval = -1;
            }
            break;
        case CHK_CHARFLAG:
            if ( lval_char )
            {
                found = NULL;
                for ( tmp = lval_char->pflag_list;tmp;tmp = tmp->next )
                    if ( NOPOL( extr[ 0 ] ) == NOPOL( tmp->id[ 0 ] ) && !str_cmp( tmp->id, extr ) )
                    {
                        found = tmp;
                        break;
                    }
                if ( found )
                    lval = found->duration;
                else
                    lval = -1;
            }
            break;
        default:
            return FALSE;
    }
	return ( num_eval( lval, oper, rval ) );
}

CHAR_DATA* resolve_char_arg( char *format )
{
	CHAR_DATA *random = NULL;
	char arg[ MAX_INPUT_LENGTH ];
    char *str;
    int liczba;

    /*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[0] == '\0' )
        return NULL;

    str     = format;
    while ( *str != '\0' )
    {
    	if ( *str != '$' )
        {
            str++;
            continue;
        }

		str++;

		one_argument( str, arg );

		switch ( arg[0] )
		{
			default:
				return NULL;

			case '_':
				return get_char_var( get_var( arg + 1, running_prog_env->vars ) );

			case 'd':
			case 'D':
				random = get_random_char( running_prog_env->ch, FALSE );
				for( liczba = 20; random == running_prog_env->vch && liczba > 0; --liczba)
					random = get_random_char( running_prog_env->ch, FALSE );

				return random;

			case 'i':
			case 'I':
				return running_prog_env->ch;

			case 'n':
			case 'N':
				return running_prog_env->vch;

			case 't':
			case 'T':
				return ( CHAR_DATA * ) running_prog_env->arg2;

			case 'r':
			case 'R':
				return running_prog_env->rch;

			case 'q':
			case 'Q':
				switch ( running_prog_env->type )
				{
					case PROG_TYPE_MOB:
						return running_prog_env->ch->prog_target;
					case PROG_TYPE_OBJ:
						return running_prog_env->obj->prog_target;
					case PROG_TYPE_ROOM:
						return running_prog_env->room->prog_target;
				}
		}

		break;
    }

    return NULL;
}

OBJ_DATA* resolve_obj_arg( char *format )
{
	char arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj1 = (OBJ_DATA  *) running_prog_env->arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) running_prog_env->arg2;
    char *str;

    /*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[0] == '\0' )
        return NULL;

    str     = format;
    while ( *str != '\0' )
    {
    	if ( *str != '$' )
        {
            str++;
            continue;
        }

		str++;

		one_argument( str, arg );

		switch ( arg[0] )
		{
			default:
				return NULL;

			case '_':
				return get_obj_var( get_var( arg + 1, running_prog_env->vars ) );

			case 'o':
			case 'O':
				switch ( running_prog_env->type )
				{
					case PROG_TYPE_MOB:
					case PROG_TYPE_ROOM:
						return obj1;
					case PROG_TYPE_OBJ:
						return running_prog_env->obj;
				}
				break;

			case 'p':
			case 'P':
				switch ( running_prog_env->type )
				{
					case PROG_TYPE_MOB:
					case PROG_TYPE_ROOM:
						return obj2;
					case PROG_TYPE_OBJ:
						return obj1;
				}
				break;
		}

		break;
    }

    return NULL;
}

ROOM_INDEX_DATA* resolve_room_arg( char *format )
{
	char arg[ MAX_INPUT_LENGTH ];
    char *str;

    /*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[0] == '\0' )
        return NULL;

    str     = format;
    while ( *str != '\0' )
    {
    	if ( *str != '$' )
        {
            str++;
            continue;
        }

		str++;

		one_argument( str, arg );

		switch ( arg[0] )
		{
			default:
				return NULL;

			case '_':
				return get_room_var( get_var( arg + 1, running_prog_env->vars ) );
		}
		break;
    }

    return NULL;
}


/*
 * ------------------------------------------------------------------------
 * EXPAND_ARG
 * This is a hack of act() in comm.c. I've added some safety guards,
 * so that missing or invalid $-codes do not crash the server
 * ------------------------------------------------------------------------
 */
void expand_arg( char *buf, char *format )
{
    char *someone = "kto¶";
    char *something = "co¶";
    static char * const someonefull [] =
        { "kto¶", "kogo¶", "komu¶", "kogo¶", "kim¶", "kim¶"
        };
    static char * const somethingfull [] =
        { "co¶", "czego¶", "czemu¶", "co¶", "czym¶", "czym¶"
        };

    char fname[ MAX_INPUT_LENGTH ];
    char var_name[ MAX_INPUT_LENGTH ];
	CHAR_DATA *ch;
	OBJ_DATA *obj;
    CHAR_DATA *vch = running_prog_env->vch;
    char *str;
    char *i;
    char *point;
    bool advance = TRUE;

    /*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[ 0 ] == '\0' )
        return ;

    point = buf;
    str = format;
    while ( *str != '\0' )
    {
		advance = TRUE;
        if ( *str != '$' )
        {
            *point++ = *str++;
            continue;
        }

		ch = resolve_char_arg( str );
		obj = resolve_obj_arg( str );

        ++str;

        switch ( *str )
        {
            default: bug( "Expand_arg: bad code %d.", *str );
                i = " <@@@> "; break;
            case '_':
            	str = grab_var_name( str + 1, var_name );
            	i = var_to_string( get_var( var_name, running_prog_env->vars ) );
				if ( i == NULL )
					i = " <@@@> ";
				advance = FALSE;
				break;

            case 'N':
            case 'n':
			case 'I':
			case 'i':
			case 'T':
			case 't':
			case 'Q':
			case 'q':
			case 'R':
			case 'r':
			case 'D':
			case 'd':
                i = someone;
                if ( ch != NULL && ( running_prog_env->ch == ch || can_see( running_prog_env->ch, ch ) ) )
                {
                    str++;
                    switch ( *str )
                    {
                        case 'n':
                        case 'N': i = IS_NPC( ch )  ? ch->short_descr : ch->name; break;
                        case 'Z':
                        case 'z': i = ch->name2;break;
                        case 'X':
                        case 'x': i = ch->name3;break;
                        case 'C':
                        case 'c': i = ch->name4;break;
                        case 'V':
                        case 'v': i = ch->name5;break;
                        case 'B':
                        case 'b': i = ch->name6;break;
						default :
                            i = get_zaimek( ch, ch, *str );
							if ( IS_NULLSTR( i ) )
							{
								one_argument( ch->name, fname );
								i = capitalize( fname );
                            }
							str--;
                            break;
                    }
                }
                else
                {
                    str++;
                    switch ( *str )
                    {
                        case 'n':
                        case 'N': i = someonefull[ 0 ];break;
                        case 'Z':
                        case 'z': i = someonefull[ 1 ];break;
                        case 'X':
                        case 'x': i = someonefull[ 2 ];break;
                        case 'C':
                        case 'c': i = someonefull[ 3 ];break;
                        case 'V':
                        case 'v': i = someonefull[ 4 ];break;
                        case 'B':
                        case 'b': i = someonefull[ 5 ];break;
                        default :
                            i = someone;
                            str--;
                            break;
                    }
                }
                break;
            case 'O':
			case 'o':
			case 'P':
			case 'p':
                i = something;
                if ( obj != NULL && ( obj == running_prog_env->obj || can_see_obj( running_prog_env->ch, obj ) ) )
                {
                    str++;
                    switch ( *str )
                    {
                        case 'n':
                        case 'N': i = obj->short_descr;break;
                        case 'Z':
                        case 'z': i = obj->name2;break;
                        case 'X':
                        case 'x': i = obj->name3;break;
                        case 'C':
                        case 'c': i = obj->name4;break;
                        case 'V':
                        case 'v': i = obj->name5;break;
                        case 'B':
                        case 'b': i = obj->name6;break;
                        default :
                            one_argument( obj->name, fname );
                            i = fname;
                            str--;
                            break;
                    }
                }
                else
                {
                    str++;
                    switch ( *str )
                    {
                        case 'n':
                        case 'N': i = somethingfull[ 0 ];break;
                        case 'Z':
                        case 'z': i = somethingfull[ 1 ];break;
                        case 'X':
                        case 'x': i = somethingfull[ 2 ];break;
                        case 'C':
                        case 'c': i = somethingfull[ 3 ];break;
                        case 'V':
                        case 'v': i = somethingfull[ 4 ];break;
                        case 'B':
                        case 'b': i = somethingfull[ 5 ];break;
                        default :
                            i = something;
                            str--;
                            break;
                    }
                }
                break;
            case 'z':
                i = someonefull[ 1 ];
                if ( vch != NULL && can_see( running_prog_env->ch, vch ) )
                {
                    one_argument( vch->name2, fname );
                    i = capitalize( fname );
                }
                break;
            case 'Z':
                i = ( vch != NULL && can_see( running_prog_env->ch, vch ) )
                    ? vch->name2 : someonefull[ 2 ]; break;

            case 'x':
                i = someonefull[ 2 ];
                if ( vch != NULL && can_see( running_prog_env->ch, vch ) )
                {
                    one_argument( vch->name3, fname );
                    i = capitalize( fname );
                }
                break;
            case 'X':
                i = ( vch != NULL && can_see( running_prog_env->ch, vch ) )
                    ? vch->name3 : someonefull[ 2 ]; break;


            case 'c':
                i = someonefull[ 3 ];
                if ( vch != NULL && can_see( running_prog_env->ch, vch ) )
                {
                    one_argument( vch->name4, fname );
                    i = capitalize( fname );
                }
                break;
            case 'C':
                i = ( vch != NULL && can_see( running_prog_env->ch, vch ) )
                    ? vch->name4 : someonefull[ 3 ]; break;

            case 'v':
                i = someonefull[ 4 ];
                if ( vch != NULL && can_see( running_prog_env->ch, vch ) )
                {
                    one_argument( vch->name5, fname );
                    i = capitalize( fname );
                }
                break;
            case 'V':
                i = ( vch != NULL && can_see( running_prog_env->ch, vch ) )
                    ? vch->name5 : someonefull[ 4 ]; break;

            case 'b':
                i = someonefull[ 5 ];
                if ( vch != NULL && can_see( running_prog_env->ch, vch ) )
                {
                    one_argument( vch->name6, fname );
                    i = capitalize( fname );
                }
                break;
            case 'B':
                i = ( vch != NULL && can_see( running_prog_env->ch, vch ) )
                    ? vch->name6 : someonefull[ 5 ]; break;
        }

		if ( advance )
        	++str;

        while ( ( *point = *i ) != '\0' )
            ++point, ++i;

    }
    *point = '\0';

    return ;
}

void act_prog( char *format, CHAR_DATA *ch, CHAR_DATA *victim, int type, int min_pos )
{
	CHAR_DATA	*to;
	OBJ_DATA	*obj;
    CHAR_DATA	*vch;
	CHAR_DATA * next_in_room = NULL;
	char 		*str;
	char 		*i = NULL;
	char 		*point;
	char 		*pbuff;
	char 		buffer[ MAX_STRING_LENGTH*2 ];
	char 		buf[ MAX_STRING_LENGTH   ];
	char 		fname[ MAX_INPUT_LENGTH  ];
	char 		var_name[ MAX_INPUT_LENGTH  ];
	char		color = '\0';
	bool		advance = TRUE;
	bool		fColour = FALSE;
	bool		roomenabled = TRUE;
	int 		minds = 0;
    char *someone = "kto¶";
    char *something = "co¶";
    char * const someonefull [] = { "kto¶", "kogo¶", "komu¶", "kogo¶", "kim¶", "kim¶" };
    char * const somethingfull [] = { "co¶", "czego¶", "czemus", "co¶", "czym¶", "czym¶" };

    /*
    * Discard null and zero-length messages.
    */
    if ( IS_NULLSTR( format ) )
    	return;

    /* discard null rooms and chars */
    if ( !ch || !ch->in_room )
    	return;

    to = ch->in_room->people;

    if ( type == TO_VICT )
    {
        if( !victim )
        {
            bug( "Act_prog: null victim with TO_VICT.", 0 );
            return;
        }

        if( !victim->in_room )
        return;

        to = victim->in_room->people;
    }

    for ( ; to ; to = next_in_room )
    {
		if ( minds > 0 )
		{
			next_in_room = to->next_mind_in_room;
			minds = 2;
			min_pos = 0;
		}
		else
			next_in_room = to->next_in_room;

		if ( minds == 0 && !next_in_room && to->in_room && to->in_room->minds )
		{
			next_in_room = to->in_room->minds;
			minds = 1;
		}

		if ( (!IS_NPC(to) && !to->desc )
        ||   ( IS_NPC(to) && !HAS_TRIGGER(to, TRIG_ACT) && !to->desc )
        ||	to->position < min_pos )
        continue;

		if ( !IS_NPC( to ) && to->pcdata->mind_in && minds < 2 )
		continue;
		if ( !IS_NPC( to ) && to->pcdata->mind_in && ( type == TO_CHAR || type == TO_VICT ) )
		continue;
        if ( (type == TO_CHAR) && to != ch )
        continue;
        if ( type == TO_VICT && ( to != victim || to == ch ) )
        continue;
        if ( type == TO_ROOM && to == ch )
        continue;
        if ( type == TO_NOTVICT && (to == ch || to == victim ) )
        continue;

        point   = buf;
        str	 = format;

        while ( *str != '\0' )
        {
            if ( *str == '{' && *(str+1) != '\0' && *(str+1) != '{' )
            color = *(str+1);

            if ( *str != '$' )
            {
                *point++ = *str++;
                continue;
            }

			vch = resolve_char_arg( str );
			obj = resolve_obj_arg( str );

            fColour = TRUE;
            advance = TRUE;
            ++str;
            i = " <@@@> ";
            {
                switch ( *str )
                {
		            case '_':
		            	str = grab_var_name( str + 1, var_name );
            			i = var_to_string( get_var( var_name, running_prog_env->vars ) );
						if ( i == NULL )
							i = " <@@@> ";
						advance = FALSE;
						break;

                    case 'i':
            	    case 'I':
					case 'n':
					case 'N':
					case 't':
					case 'T':
					case 'q':
					case 'Q':
					case 'r':
					case 'R':
					case 'd':
					case 'D':
            	        if ( vch && can_see( to, vch ) )
                        {
                            i = someone;
                            str++;
                            switch(*str)
                            {
                                case 'n':
                                case 'N':i = IS_NPC(vch) ? vch->short_descr : vch->name;break;
                                case 'Z':
                                case 'z':i = vch->name2;break;
                                case 'X':
                                case 'x':i = vch->name3;break;
                                case 'C':
                                case 'c':i = vch->name4;break;
                                case 'V':
                                case 'v':i = vch->name5;break;
                                case 'B':
                                case 'b':i = vch->name6;break;
								default:
									i = get_zaimek( vch, vch, *str );
									if ( IS_NULLSTR( i ) )
									{
										one_argument(vch->name, fname );
										i = capitalize( fname );
                                    	str--;
									}
                                    break;
                            }
                            break;
                        } else
                        {
                            str++;
                            switch(*str)
                            {
                                case 'n':
                                case 'N':i = someonefull[0];break;
                                case 'Z':
                                case 'z':i = someonefull[1];break;
                                case 'X':
                                case 'x':i = someonefull[2];break;
                                case 'C':
                                case 'c':i = someonefull[3];break;
                                case 'V':
                                case 'v':i = someonefull[4];break;
                                case 'B':
                                case 'b':i = someonefull[5];break;
                                default :
                                        i = someone;
                                        str--;
                                        break;
                            }
                        }
                        break;

                    case 'z':
                    case 'Z':
                		i = someonefull[1];
                		if ( running_prog_env->vch && can_see( to, running_prog_env->vch ) )
                            i = running_prog_env->vch->name2;
                		break;


                    case 'x':
                    case 'X':
                		i = someonefull[2];
                		if ( running_prog_env->vch && can_see( to, running_prog_env->vch ) )
                            i = running_prog_env->vch->name3;
                		break;


                    case 'c':
                    case 'C':
                		i = someonefull[3];
                		if ( running_prog_env->vch && can_see( to, running_prog_env->vch ) )
                            i = running_prog_env->vch->name4;
                		break;

                    case 'v':
                    case 'V':
                		i = someonefull[4];
                		if ( running_prog_env->vch && can_see( to, running_prog_env->vch ) )
                            i = running_prog_env->vch->name5;
                		break;


                    case 'b':
                    case 'B':
                		i = someonefull[5];
                		if ( running_prog_env->vch && can_see( to, running_prog_env->vch ) )
                            i = running_prog_env->vch->name6;
                		break;

            	    case 'o':
					case 'O':
					case 'p':
					case 'P':
                            i = something;
                            if ( obj && can_see_obj( to, obj ) )
                            {
                                str++;
                                switch(*str)
                                {
                                    case 'n':
                                    case 'N':i = obj->short_descr;break;
                                    case 'Z':
                                    case 'z':i = obj->name2;break;
                                    case 'X':
                                    case 'x':i = obj->name3;break;
                                    case 'C':
                                    case 'c':i = obj->name4;break;
                                    case 'V':
                                    case 'v':i = obj->name5;break;
                                    case 'B':
                                    case 'b':i = obj->name6;break;
                                    default :
                                        one_argument( obj->name, fname );
                                        i = capitalize( fname );
                                        str--;
                                        break;
                                }
                            } else
                            {
                                str++;
                                switch(*str)
                                {
                                    case 'n':
                                    case 'N':i = somethingfull[0];break;
                                    case 'Z':
                                    case 'z':i = somethingfull[1];break;
                                    case 'X':
                                    case 'x':i = somethingfull[2];break;
                                    case 'C':
                                    case 'c':i = somethingfull[3];break;
                                    case 'V':
                                    case 'v':i = somethingfull[4];break;
                                    case 'B':
                                    case 'b':i = somethingfull[5];break;
                                    default :
                                    i = something;
                                    str--;
                                    break;
                                }
                            }
                            break;
                }
            }

			if ( advance )
            	++str;

            while ( ( *point = *i ) != '\0' )
            ++point, ++i;

            if ( color != '\0' )
            {
                *point++ = '{';
                *point++ = color;
            }
        }

        *point++ = '\n';
        *point++ = '\r';
        *point   = '\0';

        sprintf( buf, "%s", capitalize(buf) );

        if ( to->desc != NULL )
        {
            pbuff	 = buffer;
            colourconv( pbuff, buf, to );
            write_to_buffer( to->desc, buffer, 0 );
        }
        else if ( MOBtrigger )
            mp_act_trigger( buf, to, ch, NULL, victim, &TRIG_ACT );

        if ( OBJtrigger && to->desc )
        {
            op_act_trigger( buf, to, roomenabled );
            roomenabled = FALSE;
        }

    }

    if(!IS_NPC(ch) && HAS_RTRIGGER(ch->in_room,TRIG_ACT) && type==TO_ROOM)
    rp_act_trigger( buf, ch, &TRIG_ACT);

    return;
}

/*
 * ------------------------------------------------------------------------
 *  PROGRAM_FLOW
 *  This is the program driver. It parses the mob program code lines
 *  and passes "executable" commands to interpret()
 *  Lines beginning with 'mob' are passed to mob_interpret() to handle
 *  special mob commands (in mob_cmds.c)
 *-------------------------------------------------------------------------
 */

void program_flow( void )
{
	MEMDAT * mdat;
	char *code, *line;
	char buf[ MAX_STRING_LENGTH ];
	char control[ MAX_INPUT_LENGTH ], data[ MAX_STRING_LENGTH ];
	bool not;

	int eval, check;
	int i;

	CHAR_DATA* forgr_ch = NULL;
	CHAR_DATA* forgr_lider = NULL;
	CHAR_DATA* forgr_actual = NULL;
	CHAR_DATA* forgr_list = NULL;
	CHAR_DATA* forgr_tmp = NULL;
	char* forgr_line = NULL;
	bool forgr_skip = FALSE;
	int forgr_level = 0;
	int forgr_type = 0;
	char forgr_state[ MAX_NESTED_LEVEL ];
	char forgr_cond[ MAX_NESTED_LEVEL ];

	//je¶li nie ustawiono ¶rodowiska uruchomieniowego lub brak kodu proga to abort
	if ( !running_prog_env || IS_NULLSTR( running_prog_env->source_ptr ) )
		ABORT_PROG()

	//je¶li nie ustawiono "wlasciciela" proga to abort
	switch( running_prog_env->type )
	{
		case PROG_TYPE_MOB:
			if ( !running_prog_env->ch )
				ABORT_PROG()
			break;
		case PROG_TYPE_OBJ:
			if ( !running_prog_env->obj )
				ABORT_PROG()
			break;
		case PROG_TYPE_ROOM:
			if ( !running_prog_env->room )
				ABORT_PROG()
			break;
	}

	//jesli to mobprog i mob jest charmniety to nie wykonuje progów, chyba ze ma odpowiedniego acta
	if ( running_prog_env->type == PROG_TYPE_MOB )
	{
		if( !EXT_IS_SET( running_prog_env->ch->act, ACT_RUN_PROGS_DESPITE_CHARM ) )
		{
			if( IS_AFFECTED( running_prog_env->ch, AFF_CHARM ) &&
				!IS_SET( running_prog_env->ch->pIndexData->affected_by[AFF_CHARM.bank], AFF_CHARM.vector ) )
				ABORT_PROG()

			if( IS_AFFECTED( running_prog_env->ch, AFF_ILLUSION ) )
				ABORT_PROG()

			if( EXT_IS_SET( running_prog_env->ch->act, ACT_RAISED ) )
				ABORT_PROG()
		}
	}

	//jesli to mobprog i moba nie ma w roomie to out
	if ( running_prog_env->type == PROG_TYPE_MOB && !running_prog_env->ch->in_room )
		ABORT_PROG()

	//jesli to mobprog i mob jest pod dzia³aniem mob walk i jeszcze nie doszed³ na miejsce
	if ( running_prog_env->type == PROG_TYPE_MOB && running_prog_env->ch->walking != 0 &&
	     running_prog_env->ch->in_room->vnum != running_prog_env->ch->walking && running_prog_env->ch->in_room->vnum != -1 * running_prog_env->ch->walking )
		ABORT_PROG()

	//zabezpieczenie przed pêtalmi typu call i zapetlaniem sie triggerow
	if ( count_running_progs() > MAX_CALL_LEVEL )
	{
		bug_prog_format( "program_flow: MAX_CALL_LEVEL exceeded" );
		ABORT_PROG()
	}

	//jesli gracz wywo³a³ ten sam program w czasie dzia³ania poprzedniego (delaye) to abort
	switch( running_prog_env->type )
	{
		case PROG_TYPE_MOB:
			if ( ( mdat = get_mob_memdat( running_prog_env->ch, running_prog_env->vch, running_prog_env->name, MEMDAT_ANY ) ) != NULL && mdat->prog_env != running_prog_env )
				ABORT_PROG()
			break;
		case PROG_TYPE_OBJ:
			if ( ( mdat = get_obj_memdat( running_prog_env->obj, running_prog_env->vch, running_prog_env->name, MEMDAT_ANY ) ) != NULL && mdat->prog_env != running_prog_env )
				ABORT_PROG()
			break;
		case PROG_TYPE_ROOM:
			if ( ( mdat = get_room_memdat( running_prog_env->room, running_prog_env->vch, running_prog_env->name, MEMDAT_ANY ) ) != NULL && mdat->prog_env != running_prog_env )
				ABORT_PROG()
			break;
	}

	code = running_prog_env->source_ptr;

	/*
	 * Parsowanie kodu proga
	 */
	while ( *code )
	{
		bool first_arg = TRUE;
		char *b = buf, *c = control, *d = data;

		while ( isspace( *code ) && *code ) code++;

		while ( *code )
		{
			if ( *code == '\n' || *code == '\r' )
				break;

			else if ( isspace( *code ) )
			{
				if ( first_arg )
					first_arg = FALSE;
				else
					*d++ = *code;
			}
			else
			{
				if ( first_arg )
					* c++ = *code;
				else
					*d++ = *code;
			}
			*b++ = *code++;
		}
		*b = *c = *d = '\0';

		if ( buf[ 0 ] == '\0' )
			break;

		if ( buf[ 0 ] == '*' || buf[ 0 ] == '#' )   /* Comment */
			continue;


		line = data;

		if ( forgr_skip )
		{
			if ( !str_cmp( control, "endfor" ) )
				forgr_skip = FALSE;
			continue;
		}

		//sprawdzanie czy czasem nie znik³ nam w miêdzyczasie mob lub obiekt z naszym progiem
		switch( running_prog_env->type )
		{
			case PROG_TYPE_MOB:
				if ( !running_prog_env->ch )
					ABORT_PROG()
				break;
			case PROG_TYPE_OBJ:
				if ( !running_prog_env->obj )
					ABORT_PROG()
				break;
		}

		//dopasowanie komend
		if ( !str_cmp( control, "if" ) )
		{
			if ( running_prog_env->state[ running_prog_env->level ] == BEGIN_BLOCK )
			{
				bug_prog_format( "program_flow: misplaced if statement" );
				ABORT_PROG()
			}

			running_prog_env->state[ running_prog_env->level ] = BEGIN_BLOCK;

			if ( ++running_prog_env->level >= MAX_NESTED_LEVEL )
			{
				bug_prog_format( "program_flow: max nested level exceeded" );
				ABORT_PROG()
			}

			if ( running_prog_env->level && running_prog_env->cond[ running_prog_env->level - 1 ] == FALSE )
			{
				running_prog_env->cond[ running_prog_env->level ] = FALSE;
				continue;
			}

			line = one_argument( line, control );

			not = FALSE;
			if ( control[ 0 ] == '!' )
			{
				not = TRUE;
				memmove( control, control + 1, strlen(control) + 1 );
			}

			if ( ( check = keyword_lookup( fn_keyword, control ) ) >= 0 )
			{
				running_prog_env->cond[ running_prog_env->level ] = cmd_eval( line, check );
				if ( not ) running_prog_env->cond[ running_prog_env->level ] = !running_prog_env->cond[ running_prog_env->level ];
			}
			else
			{
				bug_prog_format( "program_flow: invalid if_check (%s)", control );
				ABORT_PROG()
			}
			running_prog_env->state[ running_prog_env->level ] = END_BLOCK;
		}
		else if ( !str_cmp( control, "or" ) )
		{
			if ( !running_prog_env->level || running_prog_env->state[ running_prog_env->level - 1 ] != BEGIN_BLOCK )
			{
				bug_prog_format( "program_flow: or without if" );
				ABORT_PROG()
			}
			if ( running_prog_env->level && running_prog_env->cond[ running_prog_env->level - 1 ] == FALSE ) continue;
			line = one_argument( line, control );

			not = FALSE;
			if ( control[ 0 ] == '!' )
			{
				not = TRUE;
				memmove( control, control + 1, strlen(control) + 1 );
			}

			if ( ( check = keyword_lookup( fn_keyword, control ) ) >= 0 )
			{
				eval = cmd_eval( line, check );
				if ( not ) eval = !eval;
			}
			else
			{
				bug_prog_format( "program_flow: invalid if_check (%s)", control );
				ABORT_PROG()
			}
			running_prog_env->cond[ running_prog_env->level ] = ( eval == TRUE ) ? TRUE : running_prog_env->cond[ running_prog_env->level ];
		}
		else if ( !str_cmp( control, "and" ) )
		{
			if ( !running_prog_env->level || running_prog_env->state[ running_prog_env->level - 1 ] != BEGIN_BLOCK )
			{
				bug_prog_format( "program_flow: and without if" );
				ABORT_PROG()
			}
			if ( running_prog_env->level && running_prog_env->cond[ running_prog_env->level - 1 ] == FALSE ) continue;
			line = one_argument( line, control );

			not = FALSE;
			if ( control[ 0 ] == '!' )
			{
				not = TRUE;
				memmove( control, control + 1, strlen(control) + 1 );
			}

			if ( ( check = keyword_lookup( fn_keyword, control ) ) >= 0 )
			{
				eval = cmd_eval( line, check );
				if ( not ) eval = !eval;
			}
			else
			{
				bug_prog_format( "program_flow: invalid if_check (%s)", control );
				ABORT_PROG()
			}
			running_prog_env->cond[ running_prog_env->level ] = ( running_prog_env->cond[ running_prog_env->level ] == TRUE ) && ( eval == TRUE ) ? TRUE : FALSE;
		}
		else if ( !str_cmp( control, "endif" ) )
		{
			if ( !running_prog_env->level || running_prog_env->state[ running_prog_env->level - 1 ] != BEGIN_BLOCK )
			{
				bug_prog_format( "program_flow: endif without if" );
				ABORT_PROG()
			}
			running_prog_env->cond[ running_prog_env->level ] = TRUE;
			running_prog_env->state[ running_prog_env->level ] = IN_BLOCK;
			running_prog_env->state[ --running_prog_env->level ] = END_BLOCK;
		}
		else if ( !str_cmp( control, "else" ) )
		{
			if ( !running_prog_env->level || running_prog_env->state[ running_prog_env->level - 1 ] != BEGIN_BLOCK )
			{
				bug_prog_format( "program_flow: else without if" );
				ABORT_PROG()
			}
			if ( running_prog_env->level && running_prog_env->cond[ running_prog_env->level - 1 ] == FALSE ) continue;
			running_prog_env->state[ running_prog_env->level ] = IN_BLOCK;
			running_prog_env->cond[ running_prog_env->level ] = ( running_prog_env->cond[ running_prog_env->level ] == TRUE ) ? FALSE : TRUE;
		}
		else if ( running_prog_env->cond[ running_prog_env->level ] == TRUE
		          && ( !str_cmp( control, "break" ) || !str_cmp( control, "end" ) ) )
		{
			ABORT_PROG()
		}
		else if ( ( !running_prog_env->level || running_prog_env->cond[ running_prog_env->level ] == TRUE ) && buf[ 0 ] != '\0' )
		{
			running_prog_env->state[ running_prog_env->level ] = IN_BLOCK;

			strcpy( data, buf );

			if ( control[ 0 ] == ':' )
			{
				GOTO_DATA * new_goto = NULL;

				new_goto = find_goto( running_prog_env->gotos, control + 1 );

				if ( new_goto && new_goto->code_start != code )
				{
					bug_prog_format( "program_flow: duplicate label name (%s)", control + 1 );
					ABORT_PROG()
				}

				new_goto = new_goto_data();
				new_goto->name = str_dup( control + 1 );
				new_goto->code_start = code;
				new_goto->level = running_prog_env->level;

				for ( i = 0; i < MAX_NESTED_LEVEL; i++ )
				{
					new_goto->state[ i ] = running_prog_env->state[ i ];
					new_goto->cond[ i ] = running_prog_env->cond[ i ];
				}

				if ( !add_goto( running_prog_env->gotos, new_goto ) )
				{
					bug_prog_format( "program_flow: too man labels" );
					free_goto( new_goto );
					ABORT_PROG()
				}
				continue;
			}
			else if ( IS_VAR( control ) ) //$_var = costam parametr1 parametr2
			{
				PROG_VAR * pvar, *opvar;
				char buf[ MAX_INPUT_LENGTH ];

				if ( count_vars( running_prog_env->vars ) > MAX_VAR )
				{
					bug_prog_format( "program flow: too many variables (>%d)", MAX_VAR );
					ABORT_PROG()
				}

				line = one_argument( data, buf );
				line = one_argument( line, buf );

				if ( str_cmp( buf, "=" ) )
					continue;

				line = param_one_argument( line, buf );

				opvar = get_var( control + 2, running_prog_env->vars );
				if ( !opvar )
				{
					opvar = new_prog_var( control + 2, NULL, PROG_VAR_STRING );
					var_to_list( opvar, &running_prog_env->vars );
				}

				if ( IS_NUMBER( buf ) )
				{
					var_set_value( opvar, buf, PROG_VAR_INT );
				}
				else if ( IS_VAR( buf ) )
				{
					var_copy_value( opvar, get_var( buf + 2, running_prog_env->vars ) );
				}
				else if ( IS_BUILT_IN_VAR( buf ) )
				{
					CHAR_DATA * var_char = resolve_char_arg( buf );
					OBJ_DATA * var_obj = resolve_obj_arg( buf );

					if ( var_char )
					{
						var_set_value( opvar, var_char, PROG_VAR_CHAR );
					}
					else if ( var_obj )
						var_set_value( opvar, var_obj, PROG_VAR_OBJ );
				}
				else if ( is_var_cmd( buf ) )
				{
					pvar = new_prog_var( control + 2, NULL, PROG_VAR_STRING );
					var_dispatch_cmd( pvar, buf, line );

					var_copy_value( opvar, pvar );
					free_prog_var( pvar );
					pvar = NULL;
				}
				else
				{
					buf[strlen( buf ) - 1] = '\0';
					var_set_value( opvar, buf+1, PROG_VAR_STRING );
				}
			}
			else if ( !str_cmp( control, "goto" ) )
			{
				GOTO_DATA * goto_to = NULL;

				if ( running_prog_env->goto_call > MAX_GOTO_CALL )
				{
					bug_prog_format( "program_flow: possible infinite goto-loop" );
					ABORT_PROG()
				}

				line = one_argument( data, control );

				if ( ( goto_to = find_goto( running_prog_env->gotos, line ) ) == NULL )
				{
					PROG_CODE * prog;
					char *str, *gn, *str2;
					bool found = FALSE;
					char gname[ MAX_INPUT_LENGTH ];

					str = str2 = gn = NULL;

					switch ( running_prog_env->type )
					{
						case PROG_TYPE_MOB:
							prog = get_mprog_index( running_prog_env->name );
							break;
						case PROG_TYPE_OBJ:
							prog = get_oprog_index( running_prog_env->name );
							break;
						case PROG_TYPE_ROOM:
							prog = get_rprog_index( running_prog_env->name );
							break;
   					        default:
						  bug_prog_format( "program_flow: unknown prog type." );
  						  ABORT_PROG()
						  break;
					}

					if ( !prog )
						continue;

					str = prog->code;

					do
					{
						found = FALSE;
						gn = gname;
						*gn = '\0';

						while ( *str )
						{
							if ( *str == ':' )
								break;
							str++;
						};
						str2 = str;
						if ( *str )
						{
							str++;
							while ( *str )
							{
								if ( isspace( *str ) || *str == '\n' )
									break;
								*gn = *str;
								gn++;
								str++;
							};
							*gn = '\0';
							if ( !str_cmp( line, gname ) )
							{
								found = TRUE;
								break;
							}
						}
					}
					while ( *str );

					if ( found )
					{
						code = str2;
						running_prog_env->level = 0;

						for ( i = 0; i < MAX_NESTED_LEVEL; i++ )
						{
							running_prog_env->state[ i ] = IN_BLOCK;
							running_prog_env->cond[ i ] = TRUE;
						}
						running_prog_env->goto_call++;
					}
					continue;
				}

				code = goto_to->code_start;
				running_prog_env->level = goto_to->level;

				for ( i = 0; i < MAX_NESTED_LEVEL; i++ )
				{
					running_prog_env->state[ i ] = goto_to->state[ i ];
					running_prog_env->cond[ i ] = goto_to->cond[ i ];
				}
				running_prog_env->goto_call++;
				continue;
			}
			else if ( !str_cmp( control, "foreachingroup" ) )
			{

				if ( forgr_lider )
				{
					bug_prog_format( "program_flow: nested foreachin*" );
					ABORT_PROG()
				}

				line = one_argument( data, control );

				forgr_lider = resolve_char_arg( line );

				if ( !forgr_lider )
				{
					forgr_skip = TRUE;
					continue;
				}

				forgr_type = 0;
				forgr_ch = running_prog_env->vch;
				running_prog_env->vch = forgr_lider;
				forgr_actual = forgr_lider;
				forgr_list = char_list;
				forgr_line = code;
				forgr_level = running_prog_env->level;
				for ( i = 0; i < MAX_NESTED_LEVEL; i++ )
				{
					forgr_state[ i ] = running_prog_env->state[ i ];
					forgr_cond[ i ] = running_prog_env->cond[ i ];
				}
				continue;
			}
			else if ( !str_cmp( control, "foreachinroom" ) )
			{

				if ( forgr_lider )
				{
					bug_prog_format( "program_flow: nested foreachin*" );
					ABORT_PROG()
				}

				line = one_argument( data, control );

				forgr_lider = resolve_char_arg( line );

				if ( !forgr_lider || !forgr_lider->in_room )
				{
					forgr_skip = TRUE;
					continue;
				}

				forgr_type = 1;
				forgr_ch = running_prog_env->vch;

				forgr_actual = forgr_lider->in_room->people;
				if ( IS_NPC( forgr_actual ) && forgr_actual->pIndexData->vnum == 3 )
					forgr_actual = forgr_actual->next_in_room;

				if ( !forgr_actual )
				{
					forgr_skip = TRUE;
					continue;
				}

				running_prog_env->vch = forgr_actual;
				forgr_list = forgr_actual->next_in_room;

				forgr_line = code;
				forgr_level = running_prog_env->level;
				for ( i = 0; i < MAX_NESTED_LEVEL; i++ )
				{
					forgr_state[ i ] = running_prog_env->state[ i ];
					forgr_cond[ i ] = running_prog_env->cond[ i ];
				}
				continue;
			}
			else if ( !str_cmp( control, "endfor" ) || !str_cmp( control, "nextfor" ) || !str_cmp( control, "breakfor" ) )
			{

				if ( !forgr_actual )
				{
					bug_prog_format( "program_flow: endfor or nextfor without foreachin*" );
					ABORT_PROG()
				}

				if ( !str_cmp( control, "endfor" ) && forgr_level != running_prog_env->level )
				{
					bug_prog_format( "program_flow: jaki¶ if zaczyna siê przed i konczy w, albo zaczyna w i konczy poza pêtl± foreachin*" );
					ABORT_PROG()
				}

				if ( !str_cmp( control, "nextfor" ) || !str_cmp( control, "breakfor" ) )
				{
					for ( i = 0; i < MAX_NESTED_LEVEL; i++ )
					{
						running_prog_env->state[ i ] = forgr_state[ i ];
						running_prog_env->cond[ i ] = forgr_cond[ i ];
					}
					running_prog_env->level = forgr_level;
				}

				if ( !str_cmp( control, "breakfor" ) )
				{
					forgr_skip = TRUE;
					running_prog_env->vch = forgr_ch;
					forgr_ch = forgr_lider = forgr_actual = forgr_list = forgr_tmp = NULL;
					forgr_line = NULL;
					continue;
				}

				forgr_tmp = NULL;
				switch ( forgr_type )
				{
					case 0:
						for ( forgr_tmp = forgr_list; forgr_tmp; forgr_tmp = forgr_tmp->next )
						{
							if ( forgr_tmp != forgr_actual && forgr_tmp != forgr_lider && is_same_group( forgr_lider, forgr_tmp ) )
								break;
						}
						break;

					case 1:
						for ( forgr_tmp = forgr_list; forgr_tmp; forgr_tmp = forgr_tmp->next )
						{
							if ( IS_NPC( forgr_tmp ) && forgr_tmp->pIndexData->vnum == 3 )
								continue;
							break;
						}
						break;
				}

				if ( !forgr_tmp )
				{
					if ( !str_cmp( control, "nextfor" ) || !str_cmp( control, "breakfor" ) )
						forgr_skip = TRUE;
					running_prog_env->vch = forgr_ch;
					forgr_ch = forgr_lider = forgr_actual = forgr_list = forgr_tmp = NULL;
					forgr_line = NULL;
					continue;
				}

				forgr_actual = forgr_tmp;

				running_prog_env->vch = forgr_actual;
				switch ( forgr_type )
				{
					case 0:
						forgr_list = forgr_actual->next;
						break;

					case 1:
						forgr_list = forgr_actual->next_in_room;
						break;
				}
				code = forgr_line;
				continue;
			}
			else if ( !str_cmp( control, "mob" ) )
			{
				/*
				 * Found a mob restricted command, pass it to mob interpreter
				 */
				line = one_argument( data, control );
				mob_interpret( line );
			}
			else if ( !str_cmp( control, "char" ) )
			{
				CHAR_DATA *tmp_ch;
				/*
				 * Found a char version of a restricted command, pass it to mob interpreter
				 */
				line = one_argument( data, control );
				tmp_ch = running_prog_env->ch;
				running_prog_env->ch = running_prog_env->vch;
				mob_interpret( line );
				running_prog_env->ch = tmp_ch;
			}
			else if ( !str_cmp( control, "obj" ) )
			{
				OBJ_DATA * tmp_obj;
				int tmp_type;

				/*
				 * Found a obj restricted command, pass it to obj interpreter
				 */
				line = one_argument( data, control );

				//jesli to nie jest obj prog to bierzemy arg1 jako parametr do obj_interpret
				if ( running_prog_env->type != PROG_TYPE_OBJ && running_prog_env->arg1 )
				{
					tmp_obj = running_prog_env->obj;
					tmp_type = running_prog_env->type;
					running_prog_env->obj = ( OBJ_DATA * ) running_prog_env->arg1;
					running_prog_env->type = PROG_TYPE_OBJ;
					obj_interpret( line );
					running_prog_env->obj = tmp_obj;
					running_prog_env->type = tmp_type;
				}
				else
					obj_interpret( line );
			}
			else if ( !str_cmp( control, "call" ) )
			{
				char arg_type[ 32 ], arg_name[ 32 ], arg_opt[ 32 ], arg_par1[ 32 ], arg_par2[ 32 ], *args;
				PROG_CODE *prg;
				PROG_VAR *vars = running_prog_env->vars;
				void * arg1 = NULL;
				void * arg2 = NULL;
				bool copy_var = FALSE;

				//pobieranie argumentow;
				args = one_argument( data, control );
				args = one_argument( args, arg_type );
				args = one_argument( args, arg_name );
				args = one_argument( args, arg_par1 );
				args = one_argument( args, arg_par2 );
				one_argument( args, arg_opt );

				if ( arg_par1[0] == 'v' || arg_par2[0] == 'v' || arg_opt[0] == 'v' )
					copy_var = TRUE;

				if ( arg_type[ 0 ] == '\0' || arg_name[ 0 ] == '\0' )
					continue;

				arg1 = resolve_char_arg( arg_par1 );
				if ( !arg1 )
					arg1 = resolve_obj_arg( arg_par1 );
				if ( !arg1 )
					arg1 = resolve_room_arg( arg_par1 );

				if ( arg1 )
				{
					arg2 = resolve_char_arg( arg_par2 );
					if ( !arg2 )
						arg2 = resolve_obj_arg( arg_par2 );
				}

				if ( !str_prefix( arg_type, "mobprog" ) )
				{
					if ( ( prg = get_mprog_index( arg_name ) ) == NULL )
						continue;

					create_mprog_env( prg->name, prg->code, arg1 ? ( CHAR_DATA * ) arg1 : running_prog_env->ch, arg2 ? ( CHAR_DATA * ) arg2 : running_prog_env->vch, running_prog_env->arg1, running_prog_env->arg2, NULL, NULL );
					if ( copy_var )
					{
						copy_var_list( vars, &running_prog_env->vars );
						SET_BIT( running_prog_env->options, PROG_OPT_VARCOPY );
					}
					else
					{
						running_prog_env->vars = vars;
						running_prog_env->other = vars;
					}

					program_flow();
				}
				else if ( !str_prefix( arg_type, "objprog" ) )
				{
					if ( ( prg = get_oprog_index( arg_name ) ) == NULL )
						continue;
					
					/* set_supermob ma check NULL, wiec dajemy w ciemno probe setowania */
					
					set_supermob( arg1 ? ( OBJ_DATA * ) arg1 : running_prog_env->obj );
					create_oprog_env( prg->name, prg->code, arg1 ? ( OBJ_DATA * ) arg1 : running_prog_env->obj, arg2 ? ( CHAR_DATA * ) arg2 : running_prog_env->vch, running_prog_env->arg1, running_prog_env->arg2, NULL, NULL );
					if ( copy_var )
					{
						copy_var_list( vars, &running_prog_env->vars );
						SET_BIT( running_prog_env->options, PROG_OPT_VARCOPY );
					}
					else
					{
						running_prog_env->vars = vars;
						running_prog_env->other = vars;
					}

					program_flow();
					release_supermob();
				}
				else if ( !str_prefix( arg_type, "roomprog" ) )
				{
					if ( ( prg = get_rprog_index( arg_name ) ) == NULL )
						continue;
					
					rset_supermob( arg1 ? ( ROOM_INDEX_DATA * ) arg1 : running_prog_env->room );
					create_rprog_env( prg->name, prg->code, arg1 ? ( ROOM_INDEX_DATA * ) arg1 : running_prog_env->room, arg2 ? ( CHAR_DATA * ) arg2 : running_prog_env->vch, running_prog_env->arg1, running_prog_env->arg2, NULL, NULL );
					if ( copy_var )
					{
						copy_var_list( vars, &running_prog_env->vars );
						SET_BIT( running_prog_env->options, PROG_OPT_VARCOPY );
					}
					else
					{
						running_prog_env->vars = vars;
						running_prog_env->other = vars;
					}

					program_flow();
					release_supermob();
				}

				continue;
			}
			else if ( !str_cmp( control, "delay" ) || !str_cmp( control, "minidelay" ) )
			{
				char num[ MAX_STRING_LENGTH ];
				char* argument;
				char expand[ MAX_STRING_LENGTH ];

				PROG_VAR * vars = NULL;
				if ( forgr_lider )
					continue;

				expand_arg( expand, line );
				argument = expand;
				argument = one_argument( argument, num );
						   one_argument( argument, num );

				if ( is_number( num ) )
				{
					mdat = new_mdat();

					if ( control[ 0 ] == 'd' )
					{
						mdat->delay = atoi( num ) * PULSE_MOBILE;
						mdat->type = MEMDAT_DELAY;
					}
					else
					{
						mdat->delay = atoi( num ) * PULSE_FIGHT_SI;
						mdat->type = MEMDAT_MINIDELAY;
					}

					running_prog_env->source_ptr = code;
					switch( running_prog_env->type )
					{
						case PROG_TYPE_MOB:
							if ( running_prog_env->ch )
								running_prog_env->ch->has_memdat++;
							break;
						case PROG_TYPE_OBJ:
							if ( running_prog_env->obj )
								running_prog_env->obj->has_memdat++;
							break;
						case PROG_TYPE_ROOM:
							if ( running_prog_env->room )
								running_prog_env->room->has_memdat++;
							break;
					}

					if ( !IS_SET( running_prog_env->options, PROG_OPT_VARCOPY ) && running_prog_env->other )
					{
						copy_var_list( running_prog_env->vars, &vars );
						if ( running_prog_env->other != running_prog_env->vars )
						{
							partial_free_var_list( &running_prog_env->vars, ( PROG_VAR * ) running_prog_env->other );
						}
						running_prog_env->vars = vars;
						SET_BIT( running_prog_env->options, PROG_OPT_VARCOPY );
					}


					mdat->prog_env = running_prog_env;
					free_string( mdat->response );
					mdat->response = NULL;

					mdat->next = memdat_list;
					memdat_list = mdat;

					running_prog_env = running_prog_env->next;
					return;
				}
				else
				{
					bug_prog_format( "program_flow: (mini)delay without duration" );
					ABORT_PROG()
				}
			}
			else if ( !str_cmp( control, "responsedelay" ) )
			{
				PROG_VAR * vars = NULL;
				char expand[ MAX_INPUT_LENGTH ];

				line = one_argument( data, control );
				line = one_argument( line, control );

				if ( forgr_lider )
					continue;

				if ( running_prog_env->type != PROG_TYPE_MOB )
				{
					bug_prog_format( "program_flow: responsedelay applicable only for mob programs - skipping" );
					ABORT_PROG()
				}

				if ( line[ 0 ] == '\0' )
				{
					bug_prog_format( "program_flow: incomplete responsedelay command" );
					ABORT_PROG()
				}

				if ( is_number( control ) )
				{
					mdat = new_mdat();

					mdat->delay = atoi( control ) * PULSE_MOBILE;
					mdat->type = MEMDAT_RESPONSEDELAY;

					running_prog_env->source_ptr = code;
					running_prog_env->ch->has_memdat++;

					if ( !IS_SET( running_prog_env->options, PROG_OPT_VARCOPY ) && running_prog_env->other )
					{
						copy_var_list( running_prog_env->vars, &vars );
						if ( running_prog_env->other != running_prog_env->vars )
						{
							partial_free_var_list( &running_prog_env->vars, ( PROG_VAR * ) running_prog_env->other );
						}
						running_prog_env->vars = vars;
						SET_BIT( running_prog_env->options, PROG_OPT_VARCOPY );
					}

					mdat->prog_env = running_prog_env;
					free_string( mdat->response );

					expand_arg( expand, line );

					mdat->response = str_dup( expand );


					mdat->next = memdat_list;
					memdat_list = mdat;

					running_prog_env = running_prog_env->next;
					return ;
				}
				else
				{
					bug_prog_format( "program_flow: responsedelay without duration" );
					ABORT_PROG()
				}
			}
			else if ( !str_cmp( control, "say" ) )
			{
				line = one_argument( data, control );
				do_mpsay( line );
			}
			else if ( !str_cmp( control, "sayto" ) )
			{
				line = one_argument( data, control );
				do_mpsayto( line );
			}
			else
			{
				expand_arg( data, buf );
				interpret( running_prog_env->ch, data );
			}
		}
	}
	ABORT_PROG()
}

bool get_exit_number (char buf[ MAX_INPUT_LENGTH ], CHAR_DATA * mob)
{
    int door = 999;
    bool found_dir = FALSE;
    if ( is_number( buf ) )
    {
        door = atoi( buf );
    }
    else if ( buf[0] != '\0' )
    {
        for ( door = 0; door < 6; door++ )
        {
            if ( !str_cmp( dir_name[door], buf ) )
            {
                found_dir = TRUE;
                break;
            }
        }
        if ( !found_dir )
        {
            for ( door = 0; door < 6; door++ )
            {
                if ( !str_cmp( eng_dir_name[door], buf ) )
                {
                    break;
                }
            }
        }
    }
    return door;
}
