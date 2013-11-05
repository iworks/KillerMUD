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
 * Jaron Krzysztof       (chris.jaron@gmail.com           ) [Razor   ] *
 * Koper Tadeusz         (jediloop@go2.pl                 ) [Garloop ] *
 * Pietrzak Marcin       (marcin@iworks.pl                ) [Gurthg  ] *
 * Sawicki Tomasz        (furgas@killer-mud.net           ) [Furgas  ] *
 * Trebicki Marek        (maro@killer.radom.net           ) [Maro    ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl        ) [Agron   ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: tables.c 11124 2012-03-19 13:54:44Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/12.02/src/tables.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "music.h"

/* for position */
const struct position_type position_table[] =
{
	{	"dead",             "dead"  },
	{	"mortally wounded", "mort"  },
	{	"incapacitated",    "incap" },
	{	"stunned",          "stun"  },
	{	"sleeping",         "sleep" },
	{	"resting",          "rest"  },
	{	"sitting",          "sit"   },
	{	"fighting",         "fight" },
	{	"standing",         "stand" },
	{	NULL,               NULL    }
};

/* for sex */
const struct sex_type sex_table[] =
{
	{	"none"		},
	{	"male"		},
	{	"female"	},
	{	"either"	},
	{	NULL		}
};

/* for gender */
const struct gender_type gender_table[] =
{
	{	"nieokre¶lony"		}, // 0
	{	"nijaki"		}, // 1
	{	"mêski"			}, // 2
	{	"¿eñski"		}, // 3
	{	"mêskoosobowy"		}, // 4
	{	"¿eñskoosobowy"		}, // 5
	{	NULL			}
};

/* for sizes */
const struct size_type size_table[] =
{
	{	"tiny"		},
	{	"small" 	},
	{	"medium"	},
	{	"large"		},
	{	"huge", 	},
	{	"giant" 	},
	{	NULL		}
};

/* for stats */
const struct stat_type stat_table[] =
{
	{	"str"	},
	{	"int" 	},
	{	"wis"	},
	{	"dex"	},
	{	"con", 	},
	{	"cha" 	},
	{	"luc" 	},
	{	NULL	}
};

const struct ext_flag_type act_flags[] =
{
    { "npc",                     &ACT_IS_NPC,                  FALSE },
    { "sentinel",                &ACT_SENTINEL,                TRUE  },
    { "scavenger",               &ACT_SCAVENGER,               TRUE  },
    { "mountable",               &ACT_MOUNTABLE,               TRUE  },
    { "memory",                  &ACT_MEMORY,                  TRUE  },
    { "aggressive",              &ACT_AGGRESSIVE,              TRUE  },
    { "stay_area",               &ACT_STAY_AREA,               TRUE  },
    { "wimpy",                   &ACT_WIMPY,                   TRUE  },
    { "barbarian",               &ACT_BARBARIAN,               TRUE  },
    { "paladin",                 &ACT_PALADIN,                 TRUE  },
    { "practice",                &ACT_PRACTICE,                TRUE  },
    { "druid",                   &ACT_DRUID,                   TRUE  },
    { "rand_items",              &ACT_RAND_ITEMS,              TRUE  },
    { "raised",                  &ACT_RAISED,                  TRUE  },
    { "undead",                  &ACT_UNDEAD,                  TRUE  },
    { "no_exp",                  &ACT_NO_EXP,                  TRUE  },
    { "cleric",                  &ACT_CLERIC,                  TRUE  },
    { "mage",                    &ACT_MAGE,                    TRUE  },
    { "thief",                   &ACT_THIEF,                   TRUE  },
    { "warrior",                 &ACT_WARRIOR,                 TRUE  },
    { "noalign",                 &ACT_NOALIGN,                 TRUE  },
    { "nopurge",                 &ACT_NOPURGE,                 TRUE  },
    { "outdoors",                &ACT_OUTDOORS,                TRUE  },
    { "stay_sector",             &ACT_STAY_SECTOR,             TRUE  },
    { "indoors",                 &ACT_INDOORS,                 TRUE  },
    { "monk",                    &ACT_MONK,                    TRUE  },
    { "bard",                    &ACT_BARD,                    TRUE  },
    { "nofollow",                &ACT_NOFOLLOW,                TRUE  },
    { "update_always",           &ACT_UPDATE_ALWAYS,           TRUE  },
    { "black_knight",            &ACT_BLACK_KNIGHT,            TRUE  },
    { "run_progs_despite_charm", &ACT_RUN_PROGS_DESPITE_CHARM, TRUE  },
    { "boss",                    &ACT_BOSS,                    TRUE  },
    { "shaman",                  &ACT_SHAMAN,                  TRUE  },
    { "reward",                  &ACT_REWARD,                  TRUE  },
    { "sage",                    &ACT_SAGE,                    TRUE  },

	{	"unregistered",		&PLR_UNREGISTERED,	FALSE	},
	{	"autoexit",			&PLR_AUTOEXIT,		FALSE	},
	{	"pol_iso",			&PLR_POL_ISO,		FALSE	},
	{	"pol_win",			&PLR_POL_WIN,		FALSE	},
	{	"pol_nopol",		&PLR_POL_NOPOL,		FALSE	},
	{	"automem",			&PLR_AUTOMEM,		FALSE	},
	{   "show_position",    &PLR_SHOWPOS,		FALSE	},
	{   "show_position",    &PLR_SHOWPOSS,		FALSE	},
	{	"combat_prompt",	&PLR_COMBAT_PROMPT,	FALSE	},
	{	"autogold",			&PLR_AUTOGOLD,		FALSE	},
	{	"autosplit",		&PLR_AUTOSPLIT,		FALSE	},
	{	"revboard",			&PLR_REVBOARD,		FALSE	},
	{	"holylight",		&PLR_HOLYLIGHT,		FALSE	},
	{	"pl_commands",		&PLR_COMMANDSPL,	FALSE	},
	{	"accept_surrender",	&PLR_ACCEPTSURR,	FALSE	},
	{	"nofollow",			&PLR_NOFOLLOW,		FALSE	},
	{	"colour",			&PLR_COLOUR,		FALSE	},
	{	"permit",			&PLR_PERMIT,		FALSE	},
	{	"log",				&PLR_LOG,			FALSE	},
	{	"deny",				&PLR_DENY,			FALSE	},
	{	"freeze",			&PLR_FREEZE,		FALSE	},
	{	"thief",			&PLR_THIEF,			FALSE	},
	{	"killer",			&PLR_KILLER,		FALSE	},
	{	"styl1",			&PLR_STYL1,			FALSE	},
	{	"styl2",			&PLR_STYL2,			FALSE	},
	{	"styl3",			&PLR_STYL3,			FALSE	},
	{	"styl4",			&PLR_STYL4,			FALSE	},
	{	"smoked",			&PLR_SMOKED,		FALSE	},
	{	NULL,				0,					0		}
};

const struct ext_flag_type affect_flags[] =
{
	{	"blind",				&AFF_BLIND,							TRUE	},
	{	"invisible",			&AFF_INVISIBLE,						TRUE	},
	{	"detect_evil",			&AFF_DETECT_EVIL,					TRUE	},
	{	"detect_invis",			&AFF_DETECT_INVIS,					TRUE	},
	{	"detect_magic",			&AFF_DETECT_MAGIC,					TRUE	},
	{	"detect_hidden",		&AFF_DETECT_HIDDEN,					TRUE	},
	{	"detect_good",			&AFF_DETECT_GOOD,					TRUE	},
	{	"sanctuary",			&AFF_SANCTUARY,						TRUE	},
	{	"faerie_fire",			&AFF_FAERIE_FIRE,					TRUE	},
	{	"infrared",				&AFF_INFRARED,						TRUE	},
	{	"curse",				&AFF_CURSE,							TRUE	},
	{	"paralyze",				&AFF_PARALYZE,						TRUE	},
	{	"poison",				&AFF_POISON,						TRUE	},
	{	"protect_evil",			&AFF_PROTECT_EVIL,					TRUE	},
	{	"protect_good",			&AFF_PROTECT_GOOD,					TRUE	},
	{	"sneak",				&AFF_SNEAK,							TRUE	},
	{	"hide",					&AFF_HIDE,							TRUE	},
	{	"sleep",				&AFF_SLEEP,							TRUE	},
	{	"charm",				&AFF_CHARM,							TRUE	},
	{	"flying",				&AFF_FLYING,						TRUE	},
	{	"pass_door",			&AFF_PASS_DOOR,						TRUE	},
	{	"haste",				&AFF_HASTE,							TRUE	},
	{	"calm",					&AFF_CALM,							TRUE	},
	{	"plague",				&AFF_PLAGUE,						TRUE	},
	{	"weaken",				&AFF_WEAKEN,						TRUE	},
	{	"dark_vision",			&AFF_DARK_VISION,					TRUE	},
	{	"berserk",				&AFF_BERSERK,						TRUE	},
	{	"swim",					&AFF_SWIM,							TRUE	},
	{	"regeneration",			&AFF_REGENERATION,					TRUE	},
	{	"slow",					&AFF_SLOW,							TRUE	},
	{	"daze",					&AFF_DAZE,						    TRUE	},
	{	"undead_invis",			&AFF_UNDEAD_INVIS,					TRUE	},
	{	"animal_invis",			&AFF_ANIMAL_INVIS,					TRUE	},
	{	"resist_fire",			&AFF_RESIST_FIRE,					FALSE	},
	{	"resist_cold",			&AFF_RESIST_COLD,					FALSE	},
	{	"entangle",				&AFF_ENTANGLE,						TRUE	},
	{	"silence",				&AFF_SILENCE,						TRUE	},
	{	"dust_evil",			&AFF_DUST_EVIL,						FALSE	},
	{	"chant",				&AFF_CHANT,							FALSE	},
	{	"prayer",				&AFF_PRAYER,						FALSE	},
	{	"waterwalk",			&AFF_WATERWALK,						TRUE	},
	{	"water_breath",			&AFF_WATERBREATH,					TRUE	},
	{	"resist_lightning",		&AFF_RESIST_LIGHTNING,				FALSE	},
	{	"brave_cloak",			&AFF_BRAVE_CLOAK,					TRUE	},
	{	"confusion",			&AFF_CONFUSION,						TRUE	},
	{	"energy_shield",		&AFF_ENERGY_SHIELD,					TRUE	},
	{	"resist_magic",			&AFF_RESIST_MAGIC,					FALSE	},
	{	"bark_skin",			&AFF_BARK_SKIN,						TRUE	},
	{	"stone_skin",			&AFF_STONE_SKIN,					TRUE	},
	{	"mirror_image",			&AFF_MIRROR_IMAGE,					FALSE	},
	{	"free_action",			&AFF_FREE_ACTION,					TRUE	},
	{	"shield",				&AFF_SHIELD,						TRUE	},
	{	"web",					&AFF_WEB,							FALSE	},
	{	"deafness",				&AFF_DEAFNESS,				    	TRUE	},
	{	"fear",					&AFF_FEAR,			      			TRUE	},
	{	"resist_acid",			&AFF_RESIST_ACID,					FALSE	},
	{	"resist_normal_weapon",	&AFF_RESIST_NORMAL_WEAPON,			FALSE	},
	{	"resist_magic_weapon",	&AFF_RESIST_MAGIC_WEAPON,			FALSE	},
	{	"fireshield",			&AFF_FIRESHIELD,					TRUE	},
	{	"iceshield",			&AFF_ICESHIELD,		   			 	TRUE	},
	{	"reflect_spell",		&AFF_REFLECT_SPELL,	    		    TRUE	},
	{	"eyes_of_the_torturer",	&AFF_EYES_OF_THE_TORTURER,			TRUE	},
	{	"meditation",			&AFF_MEDITATION,	    		    FALSE	},
	{	"recuperate",			&AFF_RECUPERATE,	    		    FALSE	},
	{	"energize",				&AFF_ENERGIZE,						FALSE	},
	{	"ethereal_armor",		&AFF_ETHEREAL_ARMOR,				TRUE	},
	{	"increase_wounds",		&AFF_INCREASE_WOUNDS,				TRUE	},
	{	"armor",				&AFF_ARMOR,							TRUE	},
	{	"minor_globe",			&AFF_MINOR_GLOBE,   				TRUE	},
	{	"globe",				&AFF_GLOBE,							TRUE	},
	{	"troll_power",			&AFF_TROLL,		    				TRUE	},
	{	"comprehend_languages",	&AFF_COMPREHEND_LANGUAGES,			TRUE	},
	{	"blade_barrier",		&AFF_BLADE_BARRIER,					TRUE	},
	{	"shield_of_nature",		&AFF_SHIELD_OF_NATURE,					TRUE	},
	{	"immolate",				&AFF_IMMOLATE,						FALSE	},
	{	"resist_negative",		&AFF_RESIST_NEGATIVE,				FALSE	},
	{	"resist_fear",			&AFF_RESIST_FEAR,					FALSE	},
	{	"resist_summon",		&AFF_RESIST_SUMMON,					FALSE	},
	{	"illusion",				&AFF_ILLUSION,						TRUE	},
	{	"zakuty",				&AFF_ZAKUTY,		   		 		FALSE	},
	{	"soul",					&AFF_SOUL,						    TRUE	},
	{	"razorblade_hands",		&AFF_RAZORBLADED,	    			FALSE	},
	{	"piercing_sight",		&AFF_PIERCING_SIGHT,				FALSE	},
	{	"beast_claws",			&AFF_BEAST_CLAWS,					FALSE	},
	{	"halucynacje_dol",		&AFF_HALLUCINATIONS_POSITIVE,		TRUE	},
	{	"halucynacje_faza",		&AFF_HALLUCINATIONS_NEGATIVE,		TRUE	},
	{	"detect_aggresive",		&AFF_DETECT_AGGRESSIVE,		    	FALSE	},
	{	"summon_distortion",	&AFF_SUMMON_DISTORTION,				TRUE	},
	{	"force_field",			&AFF_FORCE_FIELD,		    		TRUE	},
	{	"confusion_shell",		&AFF_CONFUSION_SHELL,			   	TRUE	},
	{	"float",				&AFF_FLOAT,							TRUE	},
	{	"resist_weapon",		&AFF_RESIST_WEAPON,					FALSE	},
	{	"absolute_magic_protection",&AFF_ABSOLUTE_MAGIC_PROTECTION,	TRUE	},
	{	"major_globe",			&AFF_MAJOR_GLOBE,					TRUE	},
	{	"loyalty",				&AFF_LOYALTY,						FALSE	},
	{	"maze",					&AFF_MAZE,							FALSE	},
	{	"deflect_wounds",		&AFF_DEFLECT_WOUNDS,				TRUE	},
	{	"astral_journey",		&AFF_ASTRAL_JOURNEY,				FALSE	},
	{	"loop",					&AFF_LOOP,							TRUE	},
	{	"detect_undead",		&AFF_DETECT_UNDEAD,					TRUE	},
	{	"nondetection",			&AFF_NONDETECTION,					TRUE	},
	{	"stability",			&AFF_STABILITY,						TRUE	},
	{	"healing_touch",		&AFF_HEALING_TOUCH,					FALSE	},
	{	"perfect_senses",		&AFF_PERFECT_SENSES,				TRUE	},
	{	"cautious_sleep",		&AFF_CAUTIOUS_SLEEP,				FALSE	},
	{	"sense_life",			&AFF_SENSE_LIFE,					TRUE	},
	{	"subdue spirits",		&AFF_SUBDUE_SPIRITS,				TRUE	},
	{	"spirit walk",			&AFF_SPIRIT_WALK,					TRUE	},
	{	"healing salve",		&AFF_HEALING_SALVE,					TRUE	},
	{	"sense_fatigue",		&AFF_SENSE_FATIGUE,					TRUE	},
	{	"aura",				&AFF_AURA,						FALSE	},
	{	NULL,				    &AFF_NONE,					        0		}
};

const struct ext_flag_type off_flags[] =
{
	{	"area_attack",		&OFF_AREA_ATTACK,		TRUE	},
	{	"backstab",			&OFF_BACKSTAB,			TRUE	},
	{	"bash",				&OFF_BASH,				TRUE	},
	{	"berserk",			&OFF_BERSERK,			TRUE	},
	{	"disarm",			&OFF_DISARM,			TRUE	},
	{	"dodge",			&OFF_DODGE,				TRUE	},
	{	"devour",			&OFF_DEVOUR,			TRUE	},
	{	"fast",				&OFF_FAST,				TRUE	},
	{	"kick",				&OFF_KICK,				TRUE	},
	{	"charge",			&OFF_CHARGE,			TRUE	},
	{	"parry",			&OFF_PARRY,				TRUE	},
	{	"rescue",			&OFF_RESCUE,			TRUE	},
	{	"tail",				&OFF_TAIL,				TRUE	},
	{	"trip",				&OFF_TRIP,				TRUE	},
	{	"crush",			&OFF_CRUSH,				TRUE	},
	{	"assist_all",		&OFF_ASSIST_ALL,		TRUE	},
	{	"assist_align",		&OFF_ASSIST_ALIGN,		TRUE	},
	{	"assist_race",		&OFF_ASSIST_RACE,		TRUE	},
	{	"assist_players",	&OFF_ASSIST_PLAYERS,	TRUE	},
	{	"shield_block",		&OFF_SHIELD_BLOCK,		TRUE	},
	{	"assist_vnum",		&OFF_ASSIST_VNUM,		TRUE	},
	{	"stun",				&OFF_STUN,				TRUE	},
	{	"circle",			&OFF_CIRCLE,			TRUE	},
	{	"onehalf_attack",	&OFF_ONE_HALF_ATTACK,	TRUE	},
	{	"two_attack",		&OFF_TWO_ATTACK,		TRUE	},
	{	"three_attack",		&OFF_THREE_ATTACK,		TRUE	},
	{	"four_attack",		&OFF_FOUR_ATTACK,		TRUE	},
	{	"wardance",			&OFF_WARDANCE,			TRUE	},
	{	"damage_reduction",	&OFF_DAMAGE_REDUCTION,	TRUE	},
	{	"zero_attack",		&OFF_ZERO_ATTACK,		TRUE	},
	{	"overwhelming_strike",&OFF_OVERWHELMING_STRIKE,TRUE	},
	{	"cleave",			&OFF_CLEAVE,			TRUE	},
	{	"vertical_slash",	&OFF_VERTICAL_SLASH,	TRUE	},
	{	"smite_good",		&OFF_SMITE_GOOD,		TRUE	},
	{	"smite_evil",		&OFF_SMITE_EVIL,		TRUE	},
	{	"mighty_blow",		&OFF_MIGHTY_BLOW,		TRUE	},
	{	"power_strike",		&OFF_POWER_STRIKE,		TRUE	},
	{	"critical_strike",	&OFF_CRITICAL_STRIKE,	TRUE	},
	{	"disarmproof",		&OFF_DISARMPROOF,		TRUE	},
	{	"fire_breath",		&OFF_FIRE_BREATH,		TRUE	},
	{	"frost_breath",		&OFF_FROST_BREATH,		TRUE	},
	{	"acid_breath",		&OFF_ACID_BREATH,		TRUE	},
	{	"lightning_breath",	&OFF_LIGHTNING_BREATH,	TRUE	},
	{	"sap",				&OFF_SAP,				TRUE	},
	{	NULL,				0,						0		}
};

const struct flag_type imm_flags[] =
{
	{	"summon",		A,	TRUE	},
	{	"charm",		B,	TRUE	},
	{	"magic",		C,	TRUE	},
	{	"weapon",		D,	TRUE	},
	{	"bash",			E,	TRUE	},
	{	"pierce",		F,	TRUE	},
	{	"slash",		G,	TRUE	},
	{	"fire",			H,	TRUE	},
	{	"cold",			I,	TRUE	},
	{	"lightning",	J,	TRUE	},
	{	"acid",			K,	TRUE	},
	{	"poison",		L,	TRUE	},
	{	"negative",		M,	TRUE	},
	{	"holy",			N,	TRUE	},
	{	"energy",		O,	TRUE	},
	{	"mental",		P,	TRUE	},
	{	"disease",		Q,	TRUE	},
	{	"drowning",		R,	TRUE	},
	{	"light",		S,	TRUE	},
	{	"sound",		T,	TRUE	},
	{	"wood",			X,	TRUE	},
	{	"silver",		Y,	TRUE	},
	{	"iron",			Z,	TRUE	},
	{	NULL,			0,	0	}
};

const struct flag_type form_flags[] =
{
	{	"edible",		FORM_EDIBLE,		TRUE	},
	{	"poison",		FORM_POISON,		TRUE	},
	{	"magical",		FORM_MAGICAL,		TRUE	},
	{	"instant_decay",FORM_INSTANT_DECAY,	TRUE	},
	{	"other",		FORM_OTHER,			TRUE	},
	{	"animal",		FORM_ANIMAL,		TRUE	},
	{	"sentient",		FORM_SENTIENT,		TRUE	},
	{	"undead",		FORM_UNDEAD,		TRUE	},
	{	"construct",	FORM_CONSTRUCT,		TRUE	},
	{	"mist",			FORM_MIST,			TRUE	},
	{	"intangible",	FORM_INTANGIBLE,	TRUE	},
	{	"biped",		FORM_BIPED,			TRUE	},
	{	"centaur",		FORM_CENTAUR,		TRUE	},
	{	"insect",		FORM_INSECT,		TRUE	},
	{	"spider",		FORM_SPIDER,		TRUE	},
	{	"crustacean",	FORM_CRUSTACEAN,	TRUE	},
	{	"worm",			FORM_WORM,			TRUE	},
	{	"blob",			FORM_BLOB,			TRUE	},
	{	"mammal",		FORM_MAMMAL,		TRUE	},
	{	"bird",			FORM_BIRD,			TRUE	},
	{	"reptile",		FORM_REPTILE,		TRUE	},
	{	"snake",		FORM_SNAKE,			TRUE	},
	{	"dragon",		FORM_DRAGON,		TRUE	},
	{	"amphibian",	FORM_AMPHIBIAN,		TRUE	},
	{	"fish",			FORM_FISH ,			TRUE	},
	{	"cold_blood",	FORM_COLD_BLOOD,	TRUE	},
	{	"warm",			FORM_WARM,			TRUE	},
	{	NULL,			0,			0	}
};

const struct flag_type part_flags[] =
{
	{	"head",			PART_HEAD,		TRUE	},
	{	"arms",			PART_ARMS,		TRUE	},
	{	"legs",			PART_LEGS,		TRUE	},
	{	"heart",		PART_HEART,		TRUE	},
	{	"brains",		PART_BRAINS,	TRUE	},
	{	"guts",			PART_GUTS,		TRUE	},
	{	"hands",		PART_HANDS,		TRUE	},
	{	"feet",			PART_FEET,		TRUE	},
	{	"fingers",		PART_FINGERS,	TRUE	},
	{	"ear",			PART_EAR,		TRUE	},
	{	"eye",			PART_EYE,		TRUE	},
	{	"long_tongue",	PART_LONG_TONGUE,TRUE	},
	{	"eyestalks",	PART_EYESTALKS,	TRUE	},
	{	"tentacles",	PART_TENTACLES,	TRUE	},
	{	"fins",			PART_FINS,		TRUE	},
	{	"wings",		PART_WINGS,		TRUE	},
	{	"tail",			PART_TAIL,		TRUE	},
	{	"bones",		PART_BONES,		TRUE	},
	{	"claws",		PART_CLAWS,		TRUE	},
	{	"fangs",		PART_FANGS,		TRUE	},
	{	"horns",		PART_HORNS,		TRUE	},
	{	"scales",		PART_SCALES,	TRUE	},
	{	"tusks",		PART_TUSKS,		TRUE	},
	{	"skin",			PART_SKIN,		TRUE	},
	{	"feathers",    PART_FEATHERS, TRUE },
	{	NULL,			0,			0	}
};

const struct flag_type comm_flags[] =
{
	{	"quiet",		COMM_QUIET,			TRUE	},
	{   "deaf",			COMM_DEAF,			TRUE	},
	{   "nowiz",		COMM_NOWIZ,			TRUE	},
	{   "nomusic",		COMM_NOMUSIC,		TRUE	},
	{   "shoutsoff",	COMM_SHOUTSOFF,		TRUE	},
	{   "compact",		COMM_COMPACT,		TRUE	},
	{   "brief",		COMM_BRIEF,			TRUE	},
	{   "prompt",		COMM_PROMPT,		TRUE	},
	{   "combine",		COMM_COMBINE,		TRUE	},
	{   "telnet_ga",	COMM_TELNET_GA,		TRUE	},
	{   "show_affects",	COMM_SHOW_AFFECTS,	TRUE	},
	{   "noemote",		COMM_NOEMOTE,		FALSE	},
	{   "noshout",		COMM_NOSHOUT,		FALSE	},
	{   "notell",		COMM_NOTELL,		FALSE	},
	{   "nochannels",	COMM_NOCHANNELS,	FALSE	},
	{   "snoop_proof",	COMM_SNOOP_PROOF,	FALSE	},
	{   "afk",			COMM_AFK,			TRUE	},
#ifdef ENABLE_SHOW_LFG
	{   "lfg",          COMM_LFG,           TRUE    },
#endif
#ifdef ENABLE_SHOW_ON_WHO
	{   "showwho",      COMM_SHOW_ON_WHO,   TRUE},
#endif
	{	NULL,			0,			0	}
};

const struct ext_flag_type prog_flags[] =
{
	{	"act",				&TRIG_ACT,				TRUE	},
	{	"bribe",			&TRIG_BRIBE,			TRUE	},
	{	"death",			&TRIG_DEATH,			TRUE	},
	{	"entry",			&TRIG_ENTRY,			TRUE	},
	{	"fight",			&TRIG_FIGHT,			TRUE	},
	{	"give",				&TRIG_GIVE,				TRUE	},
	{	"greet",			&TRIG_GREET,			TRUE	},
	{	"grall",			&TRIG_GRALL,			TRUE	},
	{	"kill",				&TRIG_KILL,				TRUE	},
	{	"hpcnt",			&TRIG_HPCNT,			TRUE	},
	{	"random",			&TRIG_RANDOM,			TRUE	},
	{	"speech",			&TRIG_SPEECH,			TRUE	},
	{	"exit",				&TRIG_EXIT,				TRUE	},
	{	"exall",			&TRIG_EXALL,			TRUE	},
	{	"delay",			&TRIG_DELAY,			TRUE	},
	{	"surrender",		&TRIG_SURR,				TRUE	},
	{	"wear",				&TRIG_WEAR,				TRUE	},
	{	"prewear",			&TRIG_PREWEAR,			TRUE	},
	{	"unequip",			&TRIG_UNEQUIP,			TRUE	},
	{	"get",				&TRIG_GET,				TRUE	},
	{	"drop",				&TRIG_DROP,				TRUE	},
	{	"put",				&TRIG_PUT,				TRUE	},
	{	"remove",			&TRIG_REMOVE,			TRUE	},
	{	"preremove",		&TRIG_PREREMOVE,		TRUE	},
	{	"input",			&TRIG_INPUT,			TRUE	},
	{	"sit",				&TRIG_SIT,				TRUE	},
	{	"sleep",			&TRIG_SLEEP,			TRUE	},
	{	"rest",				&TRIG_REST,				TRUE	},
	{	"connect",			&TRIG_CONNECT,			TRUE	},
	{	"consume",			&TRIG_CONSUME,			TRUE	},
	{	"tell",				&TRIG_TELL,				TRUE	},
	{	"time",				&TRIG_TIME,				TRUE	},
	{	"reset",			&TRIG_RESET,			TRUE	},
	{	"ask",				&TRIG_ASK,				TRUE	},
	{	"flagoff",			&TRIG_FLAGOFF,			TRUE	},
	{	"use",				&TRIG_USE,				TRUE	},
	{	"objlock",			&TRIG_OBJLOCK,			TRUE	},
	{	"objunlock",		&TRIG_OBJUNLOCK,		TRUE	},
	{	"objclosek",		&TRIG_OBJCLOSE,			TRUE	},
	{	"objopen",			&TRIG_OBJOPEN,			TRUE	},
	{	"open",				&TRIG_OPEN,				TRUE	},
	{	"close",			&TRIG_CLOSE,			TRUE	},
	{	"lock",				&TRIG_LOCK,				TRUE	},
	{	"unlock",			&TRIG_UNLOCK,			TRUE	},
	{	"tickrandom",		&TRIG_TICK_RANDOM,		TRUE	},
	{	"onload",			&TRIG_ONLOAD,			TRUE	},
	{	"onehit",			&TRIG_ONEHIT,			TRUE	},
	{	"hit",				&TRIG_HIT,				TRUE	},
	{	"mount",			&TRIG_MOUNT,			TRUE	},
	{	"dismount",			&TRIG_DISMOUNT,			TRUE	},
	{	"precommand",		&TRIG_PRECOMMAND,		TRUE	},
	{	"stand",			&TRIG_STAND,			TRUE	},
	{	"horn",				&TRIG_HORN,				TRUE	},
	{	"preget",			&TRIG_PREGET,			TRUE	},
	{	"knock",			&TRIG_KNOCK,			TRUE	},
	{	NULL,				0,						TRUE	}
};

const struct flag_type area_flags[] =
{
	{	"none",			AREA_NONE,		FALSE	},
	{	"changed",		AREA_CHANGED,	TRUE	},
	{	"added",		AREA_ADDED,		TRUE	},
	{	"loading",		AREA_LOADING,	FALSE	},
	{	"random",		AREA_RANDOM,	FALSE	},
	{	NULL,			0,			0	}
};

const struct flag_type sex_flags[] =
{
	{	"male",			SEX_MALE,		TRUE	},
	{	"female",		SEX_FEMALE,		TRUE	},
	{	"neutral",		SEX_NEUTRAL,	TRUE	},
	{   "random",		3,				TRUE    },   /* ROM */
	{	"none",			SEX_NEUTRAL,	TRUE	},
	{	NULL,			0,			0	}
};



const struct flag_type exit_flags[] =
{
	{   "door",			EX_ISDOOR,		TRUE    },
	{	"closed",		EX_CLOSED,		TRUE	},
	{	"locked",		EX_LOCKED,		TRUE	},
	{	"hidden",		EX_HIDDEN,		TRUE	},
	{	"secret",		EX_SECRET,		TRUE	},
	{	"pickproof",	EX_PICKPROOF,	TRUE	},
	{   "nopass",		EX_NOPASS,		TRUE	},
	{   "easy",			EX_EASY,		TRUE	},
	{   "hard",			EX_HARD,		TRUE	},
	{	"infuriating",	EX_INFURIATING,	TRUE	},
	{	"noclose",		EX_NOCLOSE,		TRUE	},
	{	"nolock",		EX_NOLOCK,		TRUE	},
	{	"random",		EX_RANDOM,		TRUE	},
	{	"no_mob",		EX_NO_MOB,		TRUE	},
	{	"no_mount",		EX_NO_MOUNT,	TRUE	},
	{	"_trap_",		EX_TRAP,		FALSE	},
	{	"no_flee",		EX_NO_FLEE,		TRUE	},
	{	"broken",		EX_BROKEN,		TRUE	},
	{	"eatkey",		EX_EATKEY,		TRUE	},
	{	"wall_of_mist",	EX_WALL_OF_MIST,TRUE	},
	{	NULL,			0,			0	}
};

const struct flag_type door_resets[] =
{
	{	"open and unlocked",	0,		TRUE	},
	{	"closed and unlocked",	1,		TRUE	},
	{	"closed and locked",	2,		TRUE	},
	{	NULL,			0,		0	}
};



const struct ext_flag_type room_flags[] =
{
	{	"dark",				&ROOM_DARK,				TRUE	},
	{	"mem_cleric",		&ROOM_MEMCLE,			TRUE	},
	{	"no_mob",			&ROOM_NO_MOB,			TRUE	},
	{	"indoors",			&ROOM_INDOORS,			TRUE	},
	{	"mem_druid",		&ROOM_MEMDRU,			TRUE	},
	{	"mem_mage",			&ROOM_MEMMAG,			TRUE	},
	{	"no_magic",			&ROOM_NOMAGIC,			TRUE	},
	{	"light",			&ROOM_LIGHT,			TRUE	},
	{	"magicdark",		&ROOM_MAGICDARK,		TRUE	},
	{	"private",			&ROOM_PRIVATE,			TRUE	},
	{	"safe",				&ROOM_SAFE,				TRUE	},
	{	"solitary",			&ROOM_SOLITARY,			TRUE	},
	{	"pet_shop",			&ROOM_PET_SHOP,			TRUE	},
	{	"no_recall",		&ROOM_NO_RECALL,		TRUE	},
	{	"slippery_floor",	&ROOM_SLIPPERY_FLOOR,	TRUE	},
	{	"bounty_office",	&ROOM_BOUNTY_OFFICE,	TRUE	},
	{	"heroes_only",		&ROOM_HEROES_ONLY,		TRUE	},
	{	"newbies_only",		&ROOM_NEWBIES_ONLY,		TRUE	},
	{	"law",				&ROOM_LAW,				TRUE	},
	{	"nowhere",			&ROOM_NOWHERE,			TRUE	},
	{	"bank",				&ROOM_BANK,				TRUE	},
	{	"inn",				&ROOM_INN,				TRUE	},
	{   "hoard",             &ROOM_HOARD,             TRUE},
	{	"no_hide",			&ROOM_NOHIDE,			TRUE	},
	{	"trap",				&ROOM_TRAP,				TRUE	},
	{	"no_zecho",			&ROOM_NOZECHO,			TRUE	},
	{	"randdesc",			&ROOM_RANDDESC,			TRUE	},
	{	"no_herbs",			&ROOM_NO_HERBS,			TRUE	},
	{	"consecrate",		&ROOM_CONSECRATE,		TRUE	},
	{	"desecrate",		&ROOM_DESECRATE,		TRUE	},
	{	"mem_bard",			&ROOM_MEMBARD,			TRUE	},
	{	"no_weather",		&ROOM_NO_WEATHER,		TRUE	},
	{	"watch_tower",		&ROOM_WATCH_TOWER,		TRUE	},
	{	"mem_shaman",		&ROOM_MEMSHA,			TRUE	},
	{	NULL,				0,						0		}
};

const struct flag_type type_flags[] =
{
	{	"light",			ITEM_LIGHT,			TRUE	},
	{	"scroll",			ITEM_SCROLL,		TRUE	},
	{	"wand",				ITEM_WAND,			TRUE	},
	{	"staff",			ITEM_STAFF,			TRUE	},
	{	"weapon",			ITEM_WEAPON,		TRUE	},
	{	"treasure",			ITEM_TREASURE,		TRUE	},
	{	"armor",			ITEM_ARMOR,			TRUE	},
	{	"potion",			ITEM_POTION,		TRUE	},
	{	"clothing",			ITEM_CLOTHING,		TRUE	},
	{	"furniture",		ITEM_FURNITURE,		TRUE	},
	{	"trash",			ITEM_TRASH,				TRUE	},
	{	"container",		ITEM_CONTAINER,			TRUE	},
	{	"drinkcontainer",	ITEM_DRINK_CON,			TRUE	},
	{	"key",				ITEM_KEY,				TRUE	},
	{	"food",				ITEM_FOOD,				TRUE	},
	{	"money",			ITEM_MONEY,				TRUE	},
	{	"boat",				ITEM_BOAT,				TRUE	},
	{	"npccorpse",		ITEM_CORPSE_NPC,		TRUE	},
	{	"pc corpse",		ITEM_CORPSE_PC,			FALSE	},
	{	"fountain",			ITEM_FOUNTAIN,			TRUE	},
	{	"pill",				ITEM_PILL,				TRUE	},
	{	"protect",			ITEM_PROTECT,			FALSE	},
	{	"map",				ITEM_MAP,				TRUE	},
	{   "portal",			ITEM_PORTAL,			FALSE	},
	{	"roomkey",			ITEM_ROOM_KEY,			FALSE	},
	{ 	"gem",				ITEM_GEM,				TRUE	},
	{	"jewelry",			ITEM_JEWELRY,			TRUE	},
	{	"jukebox",			ITEM_JUKEBOX,			FALSE	},
	{	"board",			ITEM_BOARD,				TRUE	},
	{	"piece",			ITEM_PIECE,				TRUE	},
	{	"spellbook",		ITEM_SPELLBOOK,			TRUE	},
	{	"spellitem",		ITEM_SPELLITEM,			FALSE	},
	{	"envenomer",		ITEM_ENVENOMER,			TRUE	},
	{	"bandage",			ITEM_BANDAGE,			TRUE	},
	{	"turn item",		ITEM_TURN,				TRUE	},
	{	"herb",	    		ITEM_HERB,				TRUE	},
	{	"skin",				ITEM_SKIN,				TRUE	},
	{   "horn",				ITEM_HORN,				TRUE	},
	{   "musical_instrument",ITEM_MUSICAL_INSTRUMENT,TRUE	},
	{   "shield",			ITEM_SHIELD,			TRUE	},
	{	"trophy",			ITEM_TROPHY,			FALSE	},
	{	"pipe",			ITEM_PIPE,			TRUE	},
	{	"weed",			ITEM_WEED,			TRUE	},
	{	"notepaper",			ITEM_NOTEPAPER,			TRUE	},
	{	"monster_parts",	ITEM_MONSTER_PART,		FALSE	},
	{ "tool", ITEM_TOOL, TRUE }, //rellik: mining
	{ "totem",  ITEM_TOTEM, TRUE  },
	{	NULL,			0,			0	}
};


const struct ext_flag_type extra_flags[] =
{
    { "animal_invis",     &ITEM_ANIMAL_INVIS,     TRUE  },
    { "antievil",         &ITEM_ANTI_EVIL,        TRUE  },
    { "antigood",         &ITEM_ANTI_GOOD,        TRUE  },
    { "antineutral",      &ITEM_ANTI_NEUTRAL,     TRUE  },
    { "bless",            &ITEM_BLESS,            TRUE  },
    { "burn_proof",       &ITEM_BURN_PROOF,       TRUE  },
    { "cover",            &ITEM_COVER,            TRUE  },
    { "dark",             &ITEM_DARK,             TRUE  },
    { "double_grip",      &ITEM_DOUBLE_GRIP,      FALSE },
    { "draggable",        &ITEM_DRAGGABLE,        TRUE  },
    { "evil",             &ITEM_EVIL,             TRUE  },
    { "free_rent",        &ITEM_FREE_RENT,        TRUE  },
    { "glow",             &ITEM_GLOW,             TRUE  },
    { "hadtimer",         &ITEM_HAD_TIMER,        TRUE  },
    { "high_rent",        &ITEM_HIGH_RENT,        TRUE  },
    { "highrepaircost",   &ITEM_HIGH_REPAIR_COST, TRUE  },
    { "inventory",        &ITEM_INVENTORY,        TRUE  },
    { "invis",            &ITEM_INVIS,            TRUE  },
    { "magic",            &ITEM_MAGIC,            TRUE  },
    { "meltdrop",         &ITEM_MELT_DROP,        TRUE  },
    { "newbie_free_rent", &ITEM_NEWBIE_FREE_RENT, TRUE  },
    { "nodrop",           &ITEM_NODROP,           TRUE  },
    { "nolocate",         &ITEM_NOLOCATE,         TRUE  },
    { "nomagic",          &ITEM_NOMAGIC,          TRUE  },
    { "nopurge",          &ITEM_NOPURGE,          TRUE  },
    { "noremove",         &ITEM_NOREMOVE,         TRUE  },
    { "no_rent",          &ITEM_NO_RENT,          TRUE  },
    { "norepair",         &ITEM_NOREPAIR,         TRUE  },
    { "nouncurse",        &ITEM_NOUNCURSE,        TRUE  },
    { "rotdeath",         &ITEM_ROT_DEATH,        TRUE  },
    { "sellextract",      &ITEM_SELL_EXTRACT,     TRUE  },
    { "transmute",        &ITEM_TRANSMUTE,        FALSE },
    { "undead_invis",     &ITEM_UNDEAD_INVIS,     TRUE  },
    { "undestructable",   &ITEM_UNDESTRUCTABLE,   TRUE  },
    { "visdeath",         &ITEM_VIS_DEATH,        TRUE  },
    { NULL,               0,                      0     }
};

const struct ext_flag_type wear_flags2[] =
{
	{	"nocle",			&ITEM_NOCLE,			TRUE	},
	{	"nowar",			&ITEM_NOWAR,			TRUE	},
	{	"nomag",			&ITEM_NOMAG,			TRUE	},
	{	"nothi",			&ITEM_NOTHI,			TRUE	},
	{	"nodwarf",			&ITEM_NODWARF,			TRUE	},
	{	"noelf",			&ITEM_NOELF,			TRUE	},
	{	"nohuman",			&ITEM_NOHUMAN,			TRUE	},
	{	"nohalfelf",		&ITEM_NOHALFELF,		TRUE	},
	{	"nognom",			&ITEM_NOGNOM,			TRUE	},
	{	"nohalfling",		&ITEM_NOHALFLING,		TRUE	},
	{	"nopal",			&ITEM_NOPAL,			TRUE	},
	{	"nodruid",			&ITEM_NODRUID,			TRUE	},
	{	"nobarb",			&ITEM_NOBARBARIAN,		TRUE	},
	{	"onlywar",			&ITEM_ONLYWAR,			TRUE	},
	{	"onlymag",			&ITEM_ONLYMAG,			TRUE	},
	{	"onlythi",			&ITEM_ONLYTHI,			TRUE	},
	{	"onlydru",			&ITEM_ONLYDRU,			TRUE	},
	{	"onlycle",			&ITEM_ONLYCLE,			TRUE	},
	{	"onlypal",			&ITEM_ONLYPAL,			TRUE	},
	{	"onlybarb",			&ITEM_ONLYBAR,			TRUE	},
	{	"onlyhuman",		&ITEM_ONLYHUMAN,		TRUE	},
	{	"onlyelf",			&ITEM_ONLYELF,			TRUE	},
	{	"onlygnom",			&ITEM_ONLYGNOM,			TRUE	},
	{	"onlyhalfling",		&ITEM_ONLYHALFLING,		TRUE	},
	{	"onlydwarf",		&ITEM_ONLYDWARF,		TRUE	},
	{	"onlyhalfelf",		&ITEM_ONLYHALFELF,		TRUE	},
	{	"onlyhalforc",		&ITEM_ONLYHALFORC,		TRUE	},
	{	"nohalforc",		&ITEM_NOHALFORC,		TRUE	},
	{	"nobard",			&ITEM_NOBARD,			TRUE	},
	{	"onlybard",			&ITEM_ONLYBARD,			TRUE	},
	{	"onlyblackknight",	&ITEM_ONLYBLACKKNIGHT,	TRUE	},
	{	"noblackknight",	&ITEM_NOBLACKKNIGHT,	TRUE	},
	{	"noodrzucanie",		&ITEM_NOODRZUCANIE,		TRUE	},
	{	"noprzemiany",		&ITEM_NOPRZEMIANY,		TRUE	},
	{	"noprzywolania",	&ITEM_NOPRZYWOLANIA,	TRUE	},
	{	"nopoznanie",		&ITEM_NOPOZNANIE,		TRUE	},
	{	"nozauroczenia",	&ITEM_NOZAUROCZENIA,	TRUE	},
	{	"noiluzje",			&ITEM_NOILUZJE,			TRUE	},
	{	"noinwokacje",		&ITEM_NOINWOKACJE,		TRUE	},
	{	"nonekromancja",	&ITEM_NONEKROMANCJA,	TRUE	},
	{	"onlyodrzucanie",	&ITEM_ONLYODRZUCANIE,	TRUE	},
	{	"onlyprzemiany",	&ITEM_ONLYPRZEMIANY,	TRUE	},
	{	"onlyprzywolania",	&ITEM_ONLYPRZYWOLANIA,	TRUE	},
	{	"onlypoznanie",		&ITEM_ONLYPOZNANIE,		TRUE	},
	{	"onlyzauroczenia",	&ITEM_ONLYZAUROCZENIA,	TRUE	},
	{	"onlyiluzje",		&ITEM_ONLYILUZJE,		TRUE	},
	{	"onlyinwokacje",	&ITEM_ONLYINWOKACJE,	TRUE	},
	{	"onlynekromancja",	&ITEM_ONLYNEKROMANCJA,	TRUE	},
	{	"onlyogolny",		&ITEM_ONLYGENERAL,		TRUE	},
	{	"noogolny",			&ITEM_NOGENERAL,		TRUE	},
	{	"noshaman",			&ITEM_NOSHAMAN,		TRUE	},
	{	"onlysha",			&ITEM_ONLYSHA,		TRUE	},
	{	NULL,				0,						0		}
};


const struct flag_type wear_flags[] =
{
	{	"take",			ITEM_TAKE,		TRUE	},
	{	"finger",		ITEM_WEAR_FINGER,	TRUE	},
	{	"neck",			ITEM_WEAR_NECK,		TRUE	},
	{	"body",			ITEM_WEAR_BODY,		TRUE	},
	{	"head",			ITEM_WEAR_HEAD,		TRUE	},
	{	"legs",			ITEM_WEAR_LEGS,		TRUE	},
	{	"feet",			ITEM_WEAR_FEET,		TRUE	},
	{	"hands",		ITEM_WEAR_HANDS,	TRUE	},
	{	"arms",			ITEM_WEAR_ARMS,		TRUE	},
	{	"shield",		ITEM_WEAR_SHIELD,	TRUE	},
	{	"about",		ITEM_WEAR_ABOUT,	TRUE	},
	{	"waist",		ITEM_WEAR_WAIST,	TRUE	},
	{	"wrist",		ITEM_WEAR_WRIST,	TRUE	},
	{	"wield",		ITEM_WIELD,		TRUE	},
	{	"hold",			ITEM_HOLD,		TRUE	},
	{	"instrument",	ITEM_INSTRUMENT,		TRUE	},
	{	"wearfloat",		ITEM_WEAR_FLOAT,	TRUE	},
	{	"wieldsecond",		ITEM_WIELDSECOND,	TRUE	},
	{	"light",		ITEM_WEAR_LIGHT,	TRUE	},
	{	"ear",		ITEM_WEAR_EAR,	TRUE	},
	{	NULL,			0,			0	}
};

/*
 * Used when adding an affect to tell where it goes.
 * See addaffect and delaffect in act_olc.c
 */
const struct flag_type apply_flags[] =
{
	{	"none",			APPLY_NONE,			TRUE	},
	{	"strength",		APPLY_STR,			TRUE	},
	{	"dexterity",	APPLY_DEX,			TRUE	},
	{	"intelligence",	APPLY_INT,			TRUE	},
	{	"wisdom",		APPLY_WIS,			TRUE	},
	{	"constitution",	APPLY_CON,			TRUE	},
	{	"sex",			APPLY_SEX,			TRUE	},
	{	"class",		APPLY_CLASS,		FALSE	},
	{	"level",		APPLY_LEVEL,		FALSE	},
	{	"age",			APPLY_AGE,			FALSE	},
	{	"height",		APPLY_HEIGHT,		FALSE	},
	{	"weight",		APPLY_WEIGHT,		FALSE	},
	{	"resist",		APPLY_RESIST,		TRUE	},
	{	"hp",			APPLY_HIT,			TRUE	},
	{	"move",			APPLY_MOVE,			TRUE	},
	{	"gold",			APPLY_GOLD,			FALSE	},
	{	"experience",	APPLY_EXP,			FALSE	},
	{	"ac",			APPLY_AC,			TRUE	},
	{	"hitroll",		APPLY_HITROLL,		TRUE	},
	{	"damroll",		APPLY_DAMROLL,		TRUE	},
	{	"savingdeath",	APPLY_SAVING_DEATH,	TRUE	},
	{	"savingrod",	APPLY_SAVING_ROD,	FALSE	},
	{	"savingpetri",	APPLY_SAVING_PETRI,	FALSE	},
	{	"savingbreath",	APPLY_SAVING_BREATH,TRUE	},
	{	"savingspell",	APPLY_SAVING_SPELL,	TRUE	},
	{	"spellaffect",	APPLY_SPELL_AFFECT,	FALSE	},
	{	"skill",		APPLY_SKILL,		TRUE	},
	{	"spellmem",		APPLY_MEMMING,		TRUE	},
	{	"language",		APPLY_LANG,			TRUE	},
	{	"charisma",		APPLY_CHA,			TRUE	},
	{	"luck",			APPLY_LUC,			TRUE	},
	{	NULL,			0,			0	}
};

/*
 * What is seen.
 */
const struct flag_type wear_loc_strings[] =
{
	{	"in the inventory",	WEAR_NONE,	TRUE	},
	{	"jako ¶wiat³o",		WEAR_LIGHT,	TRUE	},
	{	"na lewym palcu",	WEAR_FINGER_L,	TRUE	},
	{	"na prawym palcu",	WEAR_FINGER_R,	TRUE	},
	{	"wokó³ szyi (1)",	WEAR_NECK_1,	TRUE	},
	{	"wokó³ szyi (2)",	WEAR_NECK_2,	TRUE	},
	{	"na korpusie",		WEAR_BODY,	TRUE	},
	{	"na g³owie",		WEAR_HEAD,	TRUE	},
	{	"na nogach",		WEAR_LEGS,	TRUE	},
	{	"na stopach",		WEAR_FEET,	TRUE	},
	{	"na d³oniach",		WEAR_HANDS,	TRUE	},
	{	"na ramionach",		WEAR_ARMS,	TRUE	},
	{	"jako tarcza",		WEAR_SHIELD,	TRUE	},
	{	"wokó³ cia³a",		WEAR_ABOUT,	TRUE	},
	{	"wokó³ pasa",		WEAR_WAIST,	TRUE	},
	{	"na lewym nadgarstku",	WEAR_WRIST_L,	TRUE	},
	{	"na prawym nadgarstku",	WEAR_WRIST_R,	TRUE	},
	{	"dobyte",			WEAR_WIELD,	TRUE	},
	{	"trzymane w d³oni",	WEAR_HOLD,	TRUE	},
	{	"unosz±cy siê obok",	WEAR_FLOAT,	TRUE	},
	{	"dualwielded",		WEAR_SECOND,	TRUE	},
	{	"trzymane w d³oniach",	WEAR_INSTRUMENT,	TRUE	},
	{	"w lewym uchu",		WEAR_EAR_L,	TRUE	},
	{	"w prawym uchu",	WEAR_EAR_R,	TRUE	},
	{	NULL,			0	      , 0	}
};


const struct flag_type wear_loc_flags[] =
{
	{	"none",		WEAR_NONE,	TRUE	},
	{	"light",	WEAR_LIGHT,	TRUE	},
	{	"lfinger",	WEAR_FINGER_L,	TRUE	},
	{	"rfinger",	WEAR_FINGER_R,	TRUE	},
	{	"neck1",	WEAR_NECK_1,	TRUE	},
	{	"neck2",	WEAR_NECK_2,	TRUE	},
	{	"body",		WEAR_BODY,	TRUE	},
	{	"head",		WEAR_HEAD,	TRUE	},
	{	"legs",		WEAR_LEGS,	TRUE	},
	{	"feet",		WEAR_FEET,	TRUE	},
	{	"hands",	WEAR_HANDS,	TRUE	},
	{	"arms",		WEAR_ARMS,	TRUE	},
	{	"shield",	WEAR_SHIELD,	TRUE	},
	{	"about",	WEAR_ABOUT,	TRUE	},
	{	"waist",	WEAR_WAIST,	TRUE	},
	{	"lwrist",	WEAR_WRIST_L,	TRUE	},
	{	"rwrist",	WEAR_WRIST_R,	TRUE	},
	{	"wielded",	WEAR_WIELD,	TRUE	},
	{	"hold",		WEAR_HOLD,	TRUE	},
	{	"floating",	WEAR_FLOAT,	TRUE	},
	{	"dualwielded",	WEAR_SECOND,	TRUE	},
	{	"instrument",	WEAR_INSTRUMENT,	TRUE	},
	{	"lear",	WEAR_EAR_L,	TRUE	},
	{	"rear",	WEAR_EAR_R,	TRUE	},
	{	NULL,		0,		0	}
};

const struct flag_type container_flags[] =
{
	{   "closeable",        CONT_CLOSEABLE,     TRUE    },
	{   "pickproof",        CONT_PICKPROOF,     TRUE    },
	{   "closed",           CONT_CLOSED,        TRUE    },
	{   "locked",           CONT_LOCKED,        TRUE    },
	{   "puton",            CONT_PUT_ON,        TRUE    },
	{   "broken",           CONT_BROKEN,        TRUE    },
	{   "hard",             CONT_HARD,          TRUE    },
	{   "easy",             CONT_EASY,          TRUE    },
	{   "eatkey",           CONT_EATKEY,        TRUE    },
	{   "single_obj",       CONT_SINGLE_OBJ,    TRUE    },
	{   "only_vnum",        CONT_ONLY_VNUM,     TRUE    },
	{   NULL,               0,                  0       }
};

/*****************************************************************************
  ROM - specific tables:
 ****************************************************************************/




const struct flag_type ac_type[] =
{
	{   "pierce",        AC_PIERCE,            TRUE    },
	{   "bash",          AC_BASH,              TRUE    },
	{   "slash",         AC_SLASH,             TRUE    },
	{   "exotic",        AC_EXOTIC,            TRUE    },
	{   NULL,              0,                    0       }
};


const struct flag_type size_flags[] =
{
	{   "tiny",          SIZE_TINY,            TRUE    },
	{   "small",         SIZE_SMALL,           TRUE    },
	{   "medium",        SIZE_MEDIUM,          TRUE    },
	{   "large",         SIZE_LARGE,           TRUE    },
	{   "huge",          SIZE_HUGE,            TRUE    },
	{   "giant",         SIZE_GIANT,           TRUE    },
	{   NULL,              0,                    0       },
};


const struct flag_type weapon_class[] =
{
	{   "exotic",	WEAPON_EXOTIC,		TRUE    },
	{   "sword",	WEAPON_SWORD,		TRUE    },
	{   "dagger",	WEAPON_DAGGER,		TRUE    },
	{   "spear",	WEAPON_SPEAR,		TRUE    },
	{   "mace",		WEAPON_MACE,		TRUE    },
	{   "axe",		WEAPON_AXE,		TRUE    },
	{   "flail",	WEAPON_FLAIL,		TRUE    },
	{   "whip",		WEAPON_WHIP,		TRUE    },
	{   "polearm",	WEAPON_POLEARM,		TRUE    },
	{   "staff",	WEAPON_STAFF,		TRUE    },
	{	"short-sword",	WEAPON_SHORTSWORD,	TRUE	},
	{ "claw-weapons", WEAPON_CLAWS, TRUE },
	{   NULL,		0,			0       }
};

const struct flag_type weapon_type2[] =
{
	{   "flaming",		WEAPON_FLAMING,		TRUE    },
	{   "frost",		WEAPON_FROST,		TRUE    },
	{   "vampiric",		WEAPON_VAMPIRIC,	TRUE    },
	{   "sharp",		WEAPON_SHARP,		TRUE    },
	{   "vorpal",		WEAPON_VORPAL,		TRUE    },
	{   "twohands",		WEAPON_TWO_HANDS,	TRUE    },
	{	"shocking",		WEAPON_SHOCKING,	TRUE    },
	{	"poison",		WEAPON_POISON,		TRUE	},
	{	"dispeller",	WEAPON_DISPEL,		TRUE	},
	{	"primary",		WEAPON_PRIMARY,		TRUE	},
	{	"toxic",		WEAPON_TOXIC,		TRUE	},
	{	"sacred",		WEAPON_SACRED,		TRUE	},
	{	"resonant",		WEAPON_RESONANT,	TRUE	},
	{	"injurious",	WEAPON_INJURIOUS,	TRUE	},
	{	"keen",	WEAPON_KEEN,	TRUE	},
   {	"thundering",	WEAPON_THUNDERING,	TRUE	},
   { "unbalanced",  WEAPON_UNBALANCED,  TRUE },
   { "wicked",      WEAPON_WICKED,      TRUE },
   { "heartseeker", WEAPON_HEARTSEEKER, TRUE },
   {   NULL,			0,					0       }
};

//rellik: mining
const struct tool_type tool_table[] =
{
		{ "kilof", TOOL_PICKAXE },
		{ "m³ot", TOOL_HAMMER },
		{ "kowad³o", TOOL_ANVIL },
		{ NULL, -1 }
};

//rellik: mining
const struct rawmaterial_type rawmaterial_table[] =
{
		{ "wêgiel", RAW_COAL, 0, OBJ_VNUM_PIECE_COAL, OBJ_VNUM_PIECE_BIG_COAL },
		{ "¿elazo", RAW_IRON, 10, OBJ_VNUM_PIECE_IRON, OBJ_VNUM_PIECE_BIG_IRON  },
		{ "mied¼", RAW_COPPER, 20, OBJ_VNUM_PIECE_COPPER, OBJ_VNUM_PIECE_BIG_COPPER },
		{ "srebro", RAW_SILVER, 30, OBJ_VNUM_PIECE_SILVER, OBJ_VNUM_PIECE_BIG_SILVER },
		{ "z³oto", RAW_GOLD, 40, OBJ_VNUM_PIECE_GOLD, OBJ_VNUM_PIECE_BIG_GOLD  },
		{ "mithril", RAW_MITHRIL, 50, OBJ_VNUM_PIECE_MITHRIL, OBJ_VNUM_PIECE_BIG_MITHRIL },
		{ "adamantyt", RAW_ADAMANTYT, 60, OBJ_VNUM_PIECE_ADAMANTYT, OBJ_VNUM_PIECE_BIG_ADAMANTYT },
		{ "kamienie szlachetne", RAW_GEMS, 70, -1, -1 }, //-1 = specjalna obs³uga
		{ NULL, -1, 0, -1, -1 }
};

const struct flag_type position_flags[] =
{
	{   "dead",           POS_DEAD,            FALSE   },
	{   "mortal",         POS_MORTAL,          FALSE   },
	{   "incap",          POS_INCAP,           FALSE   },
	{   "stunned",        POS_STUNNED,         FALSE   },
	{   "sleeping",       POS_SLEEPING,        TRUE    },
	{   "resting",        POS_RESTING,         TRUE    },
	{   "sitting",        POS_SITTING,         TRUE    },
	{   "fighting",       POS_FIGHTING,        FALSE   },
	{   "standing",       POS_STANDING,        TRUE    },
	{   NULL,              0,                    0       }
};

const struct flag_type position_trig_flags[] =
{
	{   "dead",         FP_DEAD,            FALSE   },
	{   "mort",         FP_MORTAL,          TRUE   },
	{   "incap",        FP_INCAP,           TRUE   },
	{   "stun",         FP_STUNNED,         TRUE   },
	{   "sleep",        FP_SLEEPING,        TRUE   },
	{   "rest",         FP_RESTING,         TRUE   },
	{   "sit",          FP_SITTING,         TRUE   },
	{   "fight",        FP_FIGHTING,        TRUE   },
	{   "stand",        FP_STANDING,        TRUE   },
	{   NULL,           0,                  0      }
};

const struct flag_type portal_flags[]=
{
	{   "normal_exit",	  GATE_NORMAL_EXIT,	TRUE	},
	{	"no_curse",	  GATE_NOCURSE,		TRUE	},
	{   "go_with",	  GATE_GOWITH,		TRUE	},
	{   "buggy",	  GATE_BUGGY,		TRUE	},
	{	"random",	  GATE_RANDOM,		TRUE	},
	{   NULL,		  0,			0	}
};

const struct flag_type furniture_flags[]=
{
	{   "stand_at",	  STAND_AT,		TRUE	},
	{	"stand_on",	  STAND_ON,		TRUE	},
	{	"stand_in",	  STAND_IN,		TRUE	},
	{	"sit_at",	  SIT_AT,		TRUE	},
	{	"sit_on",	  SIT_ON,		TRUE	},
	{	"sit_in",	  SIT_IN,		TRUE	},
	{	"rest_at",	  REST_AT,		TRUE	},
	{	"rest_on",	  REST_ON,		TRUE	},
	{	"rest_in",	  REST_IN,		TRUE	},
	{	"sleep_at",	  SLEEP_AT,		TRUE	},
	{	"sleep_on",	  SLEEP_ON,		TRUE	},
	{	"sleep_in",	  SLEEP_IN,		TRUE	},
	{	"put_at",	  PUT_AT,		TRUE	},
	{	"put_on",	  PUT_ON,		TRUE	},
	{	"put_in",	  PUT_IN,		TRUE	},
	{	"put_inside",	  PUT_INSIDE,		TRUE	},
	{	NULL,		  0,			0	}
};

const struct flag_type corpse_flags[]=
{
    { "no_skin",        NO_SKIN,       TRUE },
    { "static_weight",  STATIC_WEIGHT, TRUE },
    { "no_rot",         NO_ROT,        TRUE },
    { "slow_rot",       SLOW_ROT,      TRUE },
    { NULL,             0,             0    }
};

const	struct	flag_type	apply_types	[]	=
{
	{	"affects",	TO_AFFECTS,	TRUE	},
	{	"object",	TO_OBJECT,	TRUE	},
	{	"weapon",	TO_WEAPON,	TRUE	},
	{	NULL,		0,		TRUE	}
};

const	struct	bit_type	bitvector_type	[]	=
{
	{	NULL,			affect_flags,	"affect"	},
	{	apply_flags,	NULL,			"apply"		},
	{	imm_flags,		NULL, 			"imm"		},
	{	weapon_type2,	NULL,			"weapon"	}
};

const struct flag_type prewait_target[] =
{
	{	"self",	        TARGET_SELF,	TRUE	},
	{	"other",        TARGET_CHAR,	TRUE	},
	{	"object",       TARGET_OBJ,		TRUE	},
	{	"none",	        TARGET_NONE,	TRUE	},
	{	NULL,		0	      , 0	}
};

const struct flag_type dam_types[] =
{
	{   "none",         DAM_NONE,           FALSE   },
	{   "bash",         DAM_BASH,           TRUE    },
	{   "pierce",       DAM_PIERCE,         TRUE    },
	{   "slash",        DAM_SLASH,          TRUE    },
	{   "fire",         DAM_FIRE,           TRUE    },
	{   "cold",         DAM_COLD,           TRUE    },
	{   "lightning",    DAM_LIGHTNING,      TRUE    },
	{   "acid",         DAM_ACID,           TRUE    },
	{   "poison",       DAM_POISON,         TRUE    },
	{   "negative",     DAM_NEGATIVE,       TRUE    },
	{   "holy",         DAM_HOLY,           TRUE    },
	{   "energy",       DAM_ENERGY,         TRUE    },
	{   "mental",       DAM_MENTAL,         TRUE    },
	{   "disease",      DAM_DISEASE,        TRUE    },
	{   "drowning",     DAM_DROWNING,       TRUE    },
	{   "light",        DAM_LIGHT,          TRUE    },
	{   "other",        DAM_OTHER,          TRUE    },
	{   "harm",         DAM_HARM,           TRUE    },
	{   "charm",        DAM_CHARM,          TRUE    },
	{   "sound",        DAM_SOUND,          TRUE    },
	{    NULL,          0,                  0       }
};

const struct flag_type_desc instrument_class[] =
{
	{   "ocarina",	INSTRUMENT_OCARINA,		TRUE,   "okaryna"  },
	{   "flute",	INSTRUMENT_FLUTE,		TRUE,   "flet"     },
	{   "lute",	    INSTRUMENT_LUTE,		TRUE,   "lutnia"   },
	{   "harp", 	INSTRUMENT_HARP,		TRUE,   "harfa"    },
	{   "mandolin",	INSTRUMENT_MANDOLIN,	TRUE,   "mandolina"},
	{   "piano",	INSTRUMENT_PIANO,		TRUE,   "organy"   },
	{   "sing",	    INSTRUMENT_SING,		FALSE,  "spiew"    },
	{   NULL,		0,			0, NULL       }
};

const char * real_day_names[7][2] =
{
	{ "niedziela", "niedzielê" },
	{ "poniedzia³ek", "poniedzia³ek" },
	{ "wtorek", "wtorek" },
	{ "¶roda", "¶rodê" },
	{ "czwartek", "czwartek" },
	{ "pi±tek", "pi±tek" },
	{ "sobota", "sobotê" }
};

const char * real_month_names[12][2] =
{
	{ "styczeñ", "stycznia" },
	{ "luty", "lutego" },
	{ "marzec", "marca" },
	{ "kwiecieñ", "kwietnia" },
	{ "maj", "maja" },
	{ "czerwiec", "czerwca" },
	{ "lipiec", "lipca" },
	{ "sierpieñ", "sierpnia" },
	{ "wrzesieñ", "wrze¶nia" },
	{ "pa¼dziernik", "pa¼dziernika" },
	{ "listopad", "listopada" },
	{ "grudzieñ", "grudnia" }
};

const int wear_order [] =
{
	WEAR_FLOAT,
	WEAR_HEAD,
	WEAR_EAR_L,
	WEAR_EAR_R,
	WEAR_NECK_1,
	WEAR_NECK_2,
	WEAR_ABOUT,
	WEAR_ARMS,
	WEAR_WRIST_L,
	WEAR_WRIST_R,
	WEAR_HANDS,
	WEAR_FINGER_L,
	WEAR_FINGER_R,
	WEAR_BODY,
	WEAR_WAIST,
	WEAR_LEGS,
	WEAR_FEET,
	WEAR_SHIELD,
	WEAR_WIELD,
	WEAR_SECOND,
	WEAR_HOLD,
	WEAR_LIGHT,
	WEAR_INSTRUMENT,
	-1
};

/*
   Tutaj mamy tabelke z zielem fajkowym (ITEM_WEED), oczywiscie do fajek.
 */

const	struct	weed_type	weed_table	[]	=
{
	/* LEGENDA:
	   weed_name,   - mianownik
	   weed_name2,  - dopelniacz
	   weed_name3,  - celownik
	   weed_name4,  - biernik
	   weed_name5,  - narzednik
	   weed_name6,  - miejscownik
	   weed_color,  - kolor ziela
	   (moc halucynacji [0-4], szansa na halucyne [0-100], szansa na poison [0-100])
	 */

	// 0
	{
		"ziele fajkowe",
		"ziela fajkowego",
		"zielu fajkowemu",
		"ziele fajkowe",
		"zielem fajkowym",
		"zielu fajkowym",
		"zielony",
		{1, 5, 0 }
	},

	// 1
	{
		"lekkie ziele fajkowe",
		"lekkiego ziela fajkowego",
		"lekkiemu zielu fajkowemu",
		"lekkie ziele fajkowe",
		"lekkim zielem fajkowym",
		"lekkim zielu fajkowym",
		"turkusowy",
		{0, 0, 0 }
	},

	// 2
	{
		"mocne ziele fajkowe",
		"mocnego ziela fajkowego",
		"mocnemu zielu fajkowemu",
		"mocne ziele fajkowe",
		"mocnym zielem fajkowym",
		"mocnym zielu fajkowym",
		"ciemnozielony",
		{2, 20, 0 }
	},

	// 3
	{
		"truj±ce ziele fajkowe",
		"truj±cego ziela fajkowego",
		"truj±cemu zielu fajkowemu",
		"truj±ce ziele fajkowe",
		"truj±cym zielem fajkowym",
		"truj±cym zielu fajkowym",
		"jasnozó³ty",
		{2, 50, 70 }
	},

	// 4
	{
		"mokre ziele fajkowe",
		"mokrego ziela fajkowego",
		"mokremu zielu fajkowemu",
		"mokre ziele fajkowe",
		"mokrym zielem fajkowym",
		"mokrym zielu fajkowym",
		"jasnozó³ty",
		{0, 0, 0 }
	},

	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, {   0, 0,  0 }}
};

const	struct	flag_type	wand_class	[]	=
{
	{ "none",							WAND_NONE,			FALSE,	},
	{ "dam_procentowo",					WAND_DAM_PERCENT,	TRUE	},
	{ "dam_liczbowo",					WAND_DAM_VALUE,		TRUE	},
	{ "wait",							WAND_POSTWAIT,		TRUE	},
	{ "mem_procentowo",					WAND_MEM_PERCENT,	TRUE	},
	{ "mem_liczbowo",					WAND_MEM_VALUE,		TRUE	},
	{ "level_procentowo",				WAND_LEV_PERCENT,	TRUE	},
	{ "level_liczbowo",					WAND_LEV_VALUE,		TRUE	},
	{ "przebicie_sava",					WAND_SAVE_PIERCE,	TRUE	},
	{ NULL,								0,                  FALSE	}
};

const	struct	wand_type	wand_table	[]	=
{
	{ "none",							WAND_NONE			},
	{ "dam_procentowo",					WAND_DAM_PERCENT	},
	{ "dam_liczbowo",					WAND_DAM_VALUE 		},
	{ "wait",							WAND_POSTWAIT		},
	{ "mem_procentowo",					WAND_MEM_PERCENT	},
	{ "mem_liczbowo",					WAND_MEM_VALUE		},
	{ "level_procentowo",				WAND_LEV_PERCENT	},
	{ "level_liczbowo",					WAND_LEV_VALUE		},
	{ "przebicie_sava",					WAND_SAVE_PIERCE	},
	{ NULL,								0,					}
};

const struct flag_type wand_extra[] =
{
	{	"align_good",			WAND_EXTRA_ALIGN_GOOD,		TRUE	},
	{	"align_neutral",		WAND_EXTRA_ALIGN_NEUTRAL,	TRUE	},
	{	"align_evil",			WAND_EXTRA_ALIGN_EVIL,		TRUE	},
	{	"class_mage",			WAND_EXTRA_CLASS_MAG,		TRUE	},
	{	"class_cleric",			WAND_EXTRA_CLASS_CLERIC,	TRUE	},
	{	"class_druid",			WAND_EXTRA_CLASS_DRUID,		TRUE	},
	{   "szkola_odrzucanie",	WAND_EXTRA_SCHOOL_0,		TRUE    },
	{	"szkola_przemiany",		WAND_EXTRA_SCHOOL_1,		TRUE	},
	{	"szkola_przywolania",	WAND_EXTRA_SCHOOL_2,		TRUE	},
	{	"szkola_poznanie",		WAND_EXTRA_SCHOOL_3,		TRUE	},
	{	"szkola_zauroczenia",	WAND_EXTRA_SCHOOL_4,		TRUE	},
	{	"szkola_iluzje",		WAND_EXTRA_SCHOOL_5,		TRUE	},
	{	"szkola_inwokacje",		WAND_EXTRA_SCHOOL_6,		TRUE	},
	{	"szkola_nekromancja",	WAND_EXTRA_SCHOOL_7,		TRUE	},
	{	NULL,			0,			0	}
};

const struct flag_type totem_table[] =
{
  { "ry¶",  TOTEMIC_LYNX,   TRUE    },
  { "wilk",  TOTEMIC_WOLF,     TRUE  },
  { "niedzwiedz", TOTEMIC_BEAR,   TRUE },
  { "rosomak", TOTEMIC_WOLVERINE,  TRUE  },
  { "orze³",   TOTEMIC_EAGLE,  TRUE  },
  { NULL,   0,    0  }
};

//tabelka do haszowania statów - dzia³a dla nowych i starych
//suma w linijce powinna wynosic 100
//warto¶æ na danej pozycji okre¶la ilo¶æ danej statystyki w get_curr_stat_deprecated
const int stat_hash [MAX_STATS][MAX_STATS] = {
/*STR*/		{100, 0, 0, 0, 0, 0, 0},
/*INT*/		{0, 100, 0, 0, 0, 0, 0},
/*WIS*/		{0, 0, 100, 0, 0, 0, 0},
/*DEX*/		{0, 0, 0, 100, 0, 0, 0},
/*CON*/		{0, 0, 0, 0, 100, 0, 0},
/*CHA*/		{0, 0, 0, 0, 0, 100, 0},
/*LUC*/		{0, 0, 0, 0, 0, 0, 100}
};
