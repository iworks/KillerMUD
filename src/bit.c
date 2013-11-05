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
 * Grunai                (grunai.mud@gmail.com          ) [Grunai    ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com           ) [Razor   ] *
 * Koper Tadeusz         (jediloop@go2.pl                 ) [Garloop ] *
 * Pietrzak Marcin       (marcin@iworks.pl                ) [Gurthg  ] *
 * Sawicki Tomasz        (furgas@killer-mud.net           ) [Furgas  ] *
 * Trebicki Marek        (maro@killer.radom.net           ) [Maro    ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl        ) [Agron   ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: bit.c 12144 2013-03-16 15:32:15Z raszer $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/bit.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "projects.h"

BITVECT_DATA EXT_NONE							= {0,	 0};


/* affects */
BITVECT_DATA AFF_NONE							= {0,	 0};
BITVECT_DATA AFF_BLIND							= {0,	 A};
BITVECT_DATA AFF_INVISIBLE						= {0,	 B};
BITVECT_DATA AFF_DETECT_EVIL					= {0,	 C};
BITVECT_DATA AFF_DETECT_INVIS					= {0,	 D};
BITVECT_DATA AFF_DETECT_MAGIC					= {0,	 E};
BITVECT_DATA AFF_DETECT_HIDDEN					= {0,	 F};
BITVECT_DATA AFF_DETECT_GOOD					= {0,	 G};
BITVECT_DATA AFF_SANCTUARY						= {0,	 H};
BITVECT_DATA AFF_FAERIE_FIRE					= {0,	 I};
BITVECT_DATA AFF_INFRARED						= {0,	 J};
BITVECT_DATA AFF_CURSE							= {0,	 K};
BITVECT_DATA AFF_PARALYZE						= {0,	 L};
BITVECT_DATA AFF_POISON							= {0,	 M};
BITVECT_DATA AFF_PROTECT_EVIL					= {0,	 N};
BITVECT_DATA AFF_PROTECT_GOOD					= {0,	 O};
BITVECT_DATA AFF_SNEAK							= {0,	 P};
BITVECT_DATA AFF_HIDE							= {0,	 Q};
BITVECT_DATA AFF_SLEEP							= {0,	 R};
BITVECT_DATA AFF_CHARM							= {0,	 S};
BITVECT_DATA AFF_FLYING							= {0,	 T};
BITVECT_DATA AFF_PASS_DOOR						= {0,	 U};
BITVECT_DATA AFF_HASTE							= {0,	 V};
BITVECT_DATA AFF_CALM							= {0,	 W};
BITVECT_DATA AFF_PLAGUE							= {0,	 X};
BITVECT_DATA AFF_WEAKEN							= {0,	 Y};
BITVECT_DATA AFF_DARK_VISION					= {0,	 Z};
BITVECT_DATA AFF_BERSERK						= {0,	 aa};
BITVECT_DATA AFF_SWIM							= {0,	 bb};
BITVECT_DATA AFF_REGENERATION					= {0,	 cc};
BITVECT_DATA AFF_SLOW							= {0,	 dd};
BITVECT_DATA AFF_DAZE							= {0,	 ee};
BITVECT_DATA AFF_UNDEAD_INVIS					= {1,	 A};
BITVECT_DATA AFF_ANIMAL_INVIS					= {1,	 B};
BITVECT_DATA AFF_RESIST_FIRE					= {1,	 C};
BITVECT_DATA AFF_RESIST_COLD					= {1,	 D};
BITVECT_DATA AFF_ENTANGLE						= {1,	 E};
BITVECT_DATA AFF_SILENCE						= {1,	 F};
BITVECT_DATA AFF_DUST_EVIL						= {1,	 G};
BITVECT_DATA AFF_CHANT							= {1,	 H};
BITVECT_DATA AFF_PRAYER							= {1,	 I};
BITVECT_DATA AFF_WATERWALK						= {1,	 J};
BITVECT_DATA AFF_WATERBREATH					= {1,	 K};
BITVECT_DATA AFF_RESIST_LIGHTNING				= {1,	 L};
BITVECT_DATA AFF_BRAVE_CLOAK					= {1,	 M};
BITVECT_DATA AFF_CONFUSION						= {1,	 N};
BITVECT_DATA AFF_ENERGY_SHIELD					= {1,	 O};
BITVECT_DATA AFF_RESIST_MAGIC					= {1,	 P};
BITVECT_DATA AFF_BARK_SKIN						= {1,	 Q};
BITVECT_DATA AFF_STONE_SKIN						= {1,	 R};
BITVECT_DATA AFF_MIRROR_IMAGE					= {1,	 S};
BITVECT_DATA AFF_FREE_ACTION					= {1,	 T};
BITVECT_DATA AFF_SHIELD							= {1,	 U};
BITVECT_DATA AFF_WEB							= {1,	 V};
BITVECT_DATA AFF_DEAFNESS						= {1,	 W};
BITVECT_DATA AFF_FEAR							= {1,	 X};
BITVECT_DATA AFF_RESIST_ACID					= {1,	 Y};
BITVECT_DATA AFF_RESIST_NORMAL_WEAPON			= {1,	 Z};
BITVECT_DATA AFF_RESIST_MAGIC_WEAPON			= {1,	 aa};
BITVECT_DATA AFF_FIRESHIELD						= {1,	 bb};
BITVECT_DATA AFF_ICESHIELD						= {1,	 cc};
BITVECT_DATA AFF_REFLECT_SPELL					= {1,	 dd};
BITVECT_DATA AFF_EYES_OF_THE_TORTURER			= {1,	 ee};
BITVECT_DATA AFF_MEDITATION						= {2,	 A};
BITVECT_DATA AFF_RECUPERATE						= {2,	 B};
BITVECT_DATA AFF_ENERGIZE						= {2,	 C};
BITVECT_DATA AFF_ETHEREAL_ARMOR					= {2,	 D};
BITVECT_DATA AFF_INCREASE_WOUNDS				= {2,	 E};
BITVECT_DATA AFF_ARMOR							= {2,	 F};
BITVECT_DATA AFF_MINOR_GLOBE					= {2,	 G};
BITVECT_DATA AFF_GLOBE							= {2,	 H};
BITVECT_DATA AFF_TROLL							= {2,	 I};
BITVECT_DATA AFF_COMPREHEND_LANGUAGES			= {2,	 J};
BITVECT_DATA AFF_BLADE_BARRIER					= {2,	 K};
BITVECT_DATA AFF_IMMOLATE						= {2,	 L};
BITVECT_DATA AFF_RESIST_NEGATIVE				= {2,	 M};
BITVECT_DATA AFF_RESIST_FEAR					= {2,	 N};
BITVECT_DATA AFF_RESIST_SUMMON					= {2,	 O};
BITVECT_DATA AFF_ILLUSION						= {2,	 P};
BITVECT_DATA AFF_ZAKUTY							= {2,	 Q};
BITVECT_DATA AFF_SOUL							= {2,	 R};
BITVECT_DATA AFF_RAZORBLADED					= {2,	 S};
BITVECT_DATA AFF_PIERCING_SIGHT					= {2,	 T};
BITVECT_DATA AFF_BEAST_CLAWS					= {2,	 U};
BITVECT_DATA AFF_HALLUCINATIONS_POSITIVE		= {2,	 V};
BITVECT_DATA AFF_HALLUCINATIONS_NEGATIVE		= {2,	 W};
BITVECT_DATA AFF_DETECT_AGGRESSIVE				= {2,  Z};
BITVECT_DATA AFF_SUMMON_DISTORTION				= {2, aa};
BITVECT_DATA AFF_SHIELD_OF_NATURE				= {2, bb};
BITVECT_DATA AFF_AURA							= {2, cc};
BITVECT_DATA AFF_SUBDUE_SPIRITS				= {2, dd};
BITVECT_DATA AFF_SPIRIT_WALK					= {2, ee};
BITVECT_DATA AFF_FORCE_FIELD					= {3,	 A};
BITVECT_DATA AFF_CONFUSION_SHELL				= {3,	 B};
BITVECT_DATA AFF_FLOAT							= {3,	 C};
BITVECT_DATA AFF_RESIST_WEAPON					= {3,	 D};
//free {3,E};
BITVECT_DATA AFF_MAJOR_GLOBE					= {3,	 F};
BITVECT_DATA AFF_LOYALTY						= {3,	 G};
BITVECT_DATA AFF_MAZE							= {3,	 H};
BITVECT_DATA AFF_DEFLECT_WOUNDS					= {3,	 I};
BITVECT_DATA AFF_ASTRAL_JOURNEY					= {3,	 J};
BITVECT_DATA AFF_LOOP							= {3,	 K};
BITVECT_DATA AFF_DETECT_UNDEAD					= {3,	 L};
BITVECT_DATA AFF_NONDETECTION					= {3,	 M};
BITVECT_DATA AFF_STABILITY						= {3,	 N};
BITVECT_DATA AFF_HEALING_TOUCH					= {3,	 O};
BITVECT_DATA AFF_PERFECT_SENSES					= {3,	 P};
BITVECT_DATA AFF_CAUTIOUS_SLEEP					= {3,	 R};
BITVECT_DATA AFF_SENSE_LIFE 					= {3,	 S};
BITVECT_DATA AFF_HEALING_SALVE				= {3, T};
/*Szamanskie selfbuffy*/
BITVECT_DATA AFF_ANCESTORS_WISDOM			= {3, U};
BITVECT_DATA AFF_ANCESTORS_PROTECTION			= {3, V};
BITVECT_DATA AFF_ANCESTORS_FAVOR			= {3, W};
BITVECT_DATA AFF_ANCESTORS_VISION			= {3, Z};
BITVECT_DATA AFF_ANCESTORS_FURY			= {3, aa};
//Koniec selfbuffow
BITVECT_DATA AFF_SPIRIT_OF_LIFE			= {3, bb};
BITVECT_DATA AFF_BREATH_OF_LIFE			= {3, cc};
BITVECT_DATA AFF_SPIRITUAL_GUIDANCE			= {3, ee};
#ifdef STEEL_SKIN_ON
	//Nil: steel_skin
	BITVECT_DATA AFF_STEEL_SKIN						= {4, A};
#endif
BITVECT_DATA AFF_SENSE_FATIGUE					= {4, B};
BITVECT_DATA AFF_MAGIC_HASTE					= {4, C};
BITVECT_DATA AFF_SCRYING_SHIELD					= {4, D};
BITVECT_DATA AFF_FARSIGHT					= {4, E};
BITVECT_DATA AFF_SNEAK_INVIS					= {4, F};
BITVECT_DATA AFF_SEAL_OF_ATROCITY				= {4, G};
BITVECT_DATA AFF_SEAL_OF_DESPAIR				= {4, H};
BITVECT_DATA AFF_INVOKE 					= {4, I};
/*prosibym przy dodawaniu dowych affectow spelli power-upujacych
dodac je do check_si_dispel w pliku si_spells*/


/* act */
BITVECT_DATA ACT_IS_NPC                   = { 0,  A };
BITVECT_DATA ACT_SENTINEL                 = { 0,  B };
BITVECT_DATA ACT_SCAVENGER                = { 0,  C };
BITVECT_DATA ACT_MOUNTABLE                = { 0,  D };
BITVECT_DATA ACT_MEMORY                   = { 0,  E };
BITVECT_DATA ACT_AGGRESSIVE               = { 0,  F };
BITVECT_DATA ACT_STAY_AREA                = { 0,  G };
BITVECT_DATA ACT_WIMPY                    = { 0,  H };
BITVECT_DATA ACT_BARBARIAN                = { 0,  I };
BITVECT_DATA ACT_PALADIN                  = { 0,  J };
BITVECT_DATA ACT_PRACTICE                 = { 0,  K };
BITVECT_DATA ACT_DRUID                    = { 0,  L };
BITVECT_DATA ACT_RAND_ITEMS               = { 0,  M };
BITVECT_DATA ACT_RAISED                   = { 0,  N };
BITVECT_DATA ACT_UNDEAD                   = { 0,  O };
BITVECT_DATA ACT_NO_EXP                   = { 0,  P };
BITVECT_DATA ACT_CLERIC                   = { 0,  Q };
BITVECT_DATA ACT_MAGE                     = { 0,  R };
BITVECT_DATA ACT_THIEF                    = { 0,  S };
BITVECT_DATA ACT_WARRIOR                  = { 0,  T };
BITVECT_DATA ACT_NOALIGN                  = { 0,  U };
BITVECT_DATA ACT_NOPURGE                  = { 0,  V };
BITVECT_DATA ACT_OUTDOORS                 = { 0,  W };
BITVECT_DATA ACT_STAY_SECTOR              = { 0,  X };
BITVECT_DATA ACT_INDOORS                  = { 0,  Y };
BITVECT_DATA ACT_MONK                     = { 0,  Z };
BITVECT_DATA ACT_BARD                     = { 0, aa };
BITVECT_DATA ACT_NOFOLLOW                 = { 0, bb };
BITVECT_DATA ACT_UPDATE_ALWAYS            = { 0, cc };
BITVECT_DATA ACT_BLACK_KNIGHT             = { 0, dd };
BITVECT_DATA ACT_RUN_PROGS_DESPITE_CHARM  = { 0, ee };
BITVECT_DATA ACT_BOSS                     = { 2,  C };
BITVECT_DATA ACT_GOSSIP                   = { 2,  D };
BITVECT_DATA ACT_SHAMAN                   = { 2,  E };
BITVECT_DATA ACT_REWARD                   = { 2,  F };
BITVECT_DATA ACT_SAGE                     = { 2,  G };

BITVECT_DATA PLR_SMOKED							= {1,	 A};
BITVECT_DATA PLR_UNREGISTERED					= {1,	 B};
BITVECT_DATA PLR_BLOWS							= {1,	 C};
BITVECT_DATA PLR_AUTOEXIT						= {1,	 D};
BITVECT_DATA PLR_POL_ISO						= {1,	 F};
BITVECT_DATA PLR_POL_WIN						= {1,	 G};
BITVECT_DATA PLR_POL_NOPOL						= {1,	 H};
BITVECT_DATA PLR_AUTOMEM						= {1,	 I};
BITVECT_DATA PLR_COMBAT_PROMPT					= {1,	 J};
BITVECT_DATA PLR_AUTOGOLD						= {1,	 K};
BITVECT_DATA PLR_AUTOSPLIT						= {1,	 L};
BITVECT_DATA PLR_REVBOARD						= {1,	 M};
BITVECT_DATA PLR_HOLYLIGHT						= {1,	 N};
BITVECT_DATA PLR_COMMANDSPL						= {1,	 O};
BITVECT_DATA PLR_ACCEPTSURR						= {1,	 P};
BITVECT_DATA PLR_NOFOLLOW						= {1,	 R};
BITVECT_DATA PLR_COLOUR							= {1,	 T};
BITVECT_DATA PLR_PERMIT							= {1,	 U};
BITVECT_DATA PLR_LOG							= {1,	 W};
BITVECT_DATA PLR_DENY							= {1,	 X};
BITVECT_DATA PLR_FREEZE							= {1,	 Y};
BITVECT_DATA PLR_THIEF							= {1,	 Z};
BITVECT_DATA PLR_KILLER							= {1,	 aa};
BITVECT_DATA PLR_STYL1							= {1,	 bb};
BITVECT_DATA PLR_STYL2							= {1,	 cc};
BITVECT_DATA PLR_STYL3							= {1,	 dd};
BITVECT_DATA PLR_STYL4    						= {1,	 ee};
BITVECT_DATA PLR_SHOWPOS						= {2,	 A};
BITVECT_DATA PLR_SHOWPOSS						= {2,	 B};

/* off */
BITVECT_DATA OFF_AREA_ATTACK					= {0,	 A};
BITVECT_DATA OFF_BACKSTAB						= {0,	 B};
BITVECT_DATA OFF_BASH							= {0,	 C};
BITVECT_DATA OFF_BERSERK						= {0,	 D};
BITVECT_DATA OFF_DISARM							= {0,	 E};
BITVECT_DATA OFF_DODGE							= {0,	 F};
BITVECT_DATA OFF_DEVOUR							= {0,	 G};
BITVECT_DATA OFF_FAST							= {0,	 H};
BITVECT_DATA OFF_KICK							= {0,	 I};
BITVECT_DATA OFF_CHARGE							= {0,	 J};
BITVECT_DATA OFF_PARRY							= {0,	 K};
BITVECT_DATA OFF_RESCUE							= {0,	 L};
BITVECT_DATA OFF_TAIL							= {0,	 M};
BITVECT_DATA OFF_TRIP							= {0,	 N};
BITVECT_DATA OFF_CRUSH							= {0,	 O};
BITVECT_DATA OFF_ASSIST_ALL						= {0,	 P};
BITVECT_DATA OFF_ASSIST_ALIGN					= {0,	 Q};
BITVECT_DATA OFF_ASSIST_RACE					= {0,	 R};
BITVECT_DATA OFF_ASSIST_PLAYERS					= {0,	 S};
BITVECT_DATA OFF_SHIELD_BLOCK					= {0,	 T};
BITVECT_DATA OFF_ASSIST_VNUM					= {0,	 U};
BITVECT_DATA OFF_STUN							= {0,	 V};
BITVECT_DATA OFF_CIRCLE							= {0,	 W};
BITVECT_DATA OFF_ONE_HALF_ATTACK				= {0,	 X};
BITVECT_DATA OFF_TWO_ATTACK						= {0,	 Y};
BITVECT_DATA OFF_THREE_ATTACK					= {0,	 Z};
BITVECT_DATA OFF_FOUR_ATTACK					= {0,	 aa};
BITVECT_DATA OFF_WARDANCE						= {0,	 bb};
BITVECT_DATA OFF_DAMAGE_REDUCTION				= {0,	 cc};
BITVECT_DATA OFF_ZERO_ATTACK					= {0,	 dd};
BITVECT_DATA OFF_OVERWHELMING_STRIKE			= {1,	 A};
BITVECT_DATA OFF_CLEAVE							= {1,	 B};
BITVECT_DATA OFF_VERTICAL_SLASH					= {1,	 C};
BITVECT_DATA OFF_SMITE_GOOD						= {1,	 D};
BITVECT_DATA OFF_SMITE_EVIL						= {1,	 E};
BITVECT_DATA OFF_MIGHTY_BLOW					= {1,	 F};
BITVECT_DATA OFF_POWER_STRIKE					= {1,	 G};
BITVECT_DATA OFF_CRITICAL_STRIKE				= {1,	 H};
BITVECT_DATA OFF_DISARMPROOF					= {1,	 I};
BITVECT_DATA OFF_FIRE_BREATH					= {1,	 J};
BITVECT_DATA OFF_FROST_BREATH					= {1,	 K};
BITVECT_DATA OFF_ACID_BREATH					= {1,	 M};
BITVECT_DATA OFF_LIGHTNING_BREATH				= {1,	 L};
BITVECT_DATA OFF_SLAM							= {1,	 M};
BITVECT_DATA OFF_SAP							= {1,	 N};

/* extra flags */
BITVECT_DATA ITEM_GLOW             = { 0,  A };
BITVECT_DATA ITEM_BURN_PROOF       = { 0,  B };
BITVECT_DATA ITEM_DARK             = { 0,  C };
BITVECT_DATA ITEM_UNDESTRUCTABLE   = { 0,  D };
BITVECT_DATA ITEM_EVIL             = { 0,  E };
BITVECT_DATA ITEM_INVIS            = { 0,  F };
BITVECT_DATA ITEM_MAGIC            = { 0,  G };
BITVECT_DATA ITEM_NODROP           = { 0,  H };
BITVECT_DATA ITEM_BLESS            = { 0,  I };
BITVECT_DATA ITEM_ANTI_GOOD        = { 0,  J };
BITVECT_DATA ITEM_ANTI_EVIL        = { 0,  K };
BITVECT_DATA ITEM_ANTI_NEUTRAL     = { 0,  L };
BITVECT_DATA ITEM_NOREMOVE         = { 0,  M };
BITVECT_DATA ITEM_INVENTORY        = { 0,  N };
BITVECT_DATA ITEM_NOPURGE          = { 0,  O };
BITVECT_DATA ITEM_ROT_DEATH        = { 0,  P };
BITVECT_DATA ITEM_TRANSMUTE        = { 0,  Q };
BITVECT_DATA ITEM_VIS_DEATH        = { 0,  R };
BITVECT_DATA ITEM_HIGH_REPAIR_COST = { 0,  S };
BITVECT_DATA ITEM_NOLOCATE         = { 0,  T };
BITVECT_DATA ITEM_MELT_DROP        = { 0,  U };
BITVECT_DATA ITEM_HAD_TIMER        = { 0,  V };
BITVECT_DATA ITEM_SELL_EXTRACT     = { 0,  W };
BITVECT_DATA ITEM_DRAGGABLE        = { 0,  X };
BITVECT_DATA ITEM_NOUNCURSE        = { 0,  Y };
BITVECT_DATA ITEM_UNDEAD_INVIS     = { 0,  Z };
BITVECT_DATA ITEM_NOMAGIC          = { 0, aa };
BITVECT_DATA ITEM_ANIMAL_INVIS     = { 0, bb };
BITVECT_DATA ITEM_RAREITEM         = { 0, cc };
BITVECT_DATA ITEM_NO_RENT          = { 0, dd };
BITVECT_DATA ITEM_HIGH_RENT        = { 0, ee };
BITVECT_DATA ITEM_COVER            = { 1,  A };
BITVECT_DATA ITEM_NEWBIE_FREE_RENT = { 1,  B };
BITVECT_DATA ITEM_FREE_RENT        = { 1,  C };
BITVECT_DATA ITEM_DOUBLE_GRIP      = { 1,  D };
BITVECT_DATA ITEM_SACRED           = { 1,  E };
// FREE                            = { 1,  F };
BITVECT_DATA ITEM_NOREPAIR         = { 1,  G };
BITVECT_DATA ITEM_COMPONENTONLY    = { 1,  H };

/* wear_flags2 (exweary)*/
BITVECT_DATA ITEM_NOMAG							= {0,	 A};
BITVECT_DATA ITEM_NOCLE							= {0,	 B};
BITVECT_DATA ITEM_NOTHI							= {0,	 C};
BITVECT_DATA ITEM_NOWAR							= {0,	 D};
BITVECT_DATA ITEM_NODWARF						= {0,	 E};
BITVECT_DATA ITEM_NOELF							= {0,	 F};
BITVECT_DATA ITEM_NOHUMAN						= {0,	 G};
BITVECT_DATA ITEM_NOHALFELF						= {0,	 H};
BITVECT_DATA ITEM_NOGNOM						= {0,	 I};
BITVECT_DATA ITEM_NOHALFLING					= {0,	 J};
BITVECT_DATA ITEM_NOPAL							= {0,	 K};
BITVECT_DATA ITEM_NODRUID						= {0,	 L};
BITVECT_DATA ITEM_NOBARBARIAN					= {0,	 M};
BITVECT_DATA ITEM_ONLYWAR						= {0,	 N};
BITVECT_DATA ITEM_ONLYMAG						= {0,	 O};
BITVECT_DATA ITEM_ONLYTHI						= {0,	 P};
BITVECT_DATA ITEM_ONLYDRU						= {0,	 Q};
BITVECT_DATA ITEM_ONLYCLE						= {0,	 R};
BITVECT_DATA ITEM_ONLYPAL						= {0,	 S};
BITVECT_DATA ITEM_ONLYBAR						= {0,	 T};
BITVECT_DATA ITEM_ONLYHUMAN						= {0,	 U};
BITVECT_DATA ITEM_ONLYELF						= {0,	 V};
BITVECT_DATA ITEM_ONLYGNOM						= {0,	 W};
BITVECT_DATA ITEM_ONLYHALFLING					= {0,	 X};
BITVECT_DATA ITEM_ONLYDWARF						= {0,	 Y};
BITVECT_DATA ITEM_ONLYHALFELF					= {0,	 Z};
BITVECT_DATA ITEM_ONLYHALFORC					= {0,	 aa};
BITVECT_DATA ITEM_NOHALFORC						= {0,	 bb};
BITVECT_DATA ITEM_NOBARD						= {0,	 cc};
BITVECT_DATA ITEM_ONLYBARD						= {0,	 dd};
BITVECT_DATA ITEM_ONLYBLACKKNIGHT				= {0,	 ee};
BITVECT_DATA ITEM_NOBLACKKNIGHT					= {1,	 A};
BITVECT_DATA ITEM_NOODRZUCANIE					= {1,	 B};
BITVECT_DATA ITEM_NOPRZEMIANY					= {1,	 C};
BITVECT_DATA ITEM_NOPRZYWOLANIA					= {1,	 D};
BITVECT_DATA ITEM_NOPOZNANIE					= {1,	 E};
BITVECT_DATA ITEM_NOZAUROCZENIA					= {1,	 F};
BITVECT_DATA ITEM_NOILUZJE						= {1,	 G};
BITVECT_DATA ITEM_NOINWOKACJE					= {1,	 H};
BITVECT_DATA ITEM_NONEKROMANCJA					= {1,	 I};
BITVECT_DATA ITEM_ONLYODRZUCANIE	    		= {1,	 J};
BITVECT_DATA ITEM_ONLYPRZEMIANY	    			= {1,	 K};
BITVECT_DATA ITEM_ONLYPRZYWOLANIA				= {1,	 L};
BITVECT_DATA ITEM_ONLYPOZNANIE	    			= {1,	 M};
BITVECT_DATA ITEM_ONLYZAUROCZENIA				= {1,	 N};
BITVECT_DATA ITEM_ONLYILUZJE		    		= {1,	 O};
BITVECT_DATA ITEM_ONLYINWOKACJE	    			= {1,	 P};
BITVECT_DATA ITEM_ONLYNEKROMANCJA				= {1,	 Q};
BITVECT_DATA ITEM_ONLYGENERAL					= {1,	 R};
BITVECT_DATA ITEM_NOGENERAL						= {1,	 S};
BITVECT_DATA ITEM_NOSHAMAN						= {1,	 T};
BITVECT_DATA ITEM_ONLYSHA						= {1,	 U};

//triggery
BITVECT_DATA TRIG_ACT							= {0,	 A};
BITVECT_DATA TRIG_BRIBE							= {0,	 B};
BITVECT_DATA TRIG_DEATH							= {0,	 C};
BITVECT_DATA TRIG_ENTRY							= {0,	 D};
BITVECT_DATA TRIG_FIGHT							= {0,	 E};
BITVECT_DATA TRIG_GIVE							= {0,	 F};
BITVECT_DATA TRIG_GREET							= {0,	 G};
BITVECT_DATA TRIG_GRALL							= {0,	 H};
BITVECT_DATA TRIG_KILL							= {0,	 I};
BITVECT_DATA TRIG_HPCNT							= {0,	 J};
BITVECT_DATA TRIG_RANDOM						= {0,	 K};
BITVECT_DATA TRIG_SPEECH						= {0,	 L};
BITVECT_DATA TRIG_EXIT							= {0,	 M};
BITVECT_DATA TRIG_EXALL							= {0,	 N};
BITVECT_DATA TRIG_DELAY							= {0,	 O};
BITVECT_DATA TRIG_SURR							= {0,	 P};
BITVECT_DATA TRIG_WEAR							= {0,	 Q};
BITVECT_DATA TRIG_GET							= {0,	 R};
BITVECT_DATA TRIG_DROP							= {0,	 S};
BITVECT_DATA TRIG_PUT							= {0,	 T};
BITVECT_DATA TRIG_REMOVE						= {0,	 U};
BITVECT_DATA TRIG_REST							= {0,	 V};
BITVECT_DATA TRIG_SIT							= {0,	 W};
BITVECT_DATA TRIG_SLEEP							= {0,	 X};
BITVECT_DATA TRIG_INPUT							= {0,	 Y};
BITVECT_DATA TRIG_CONNECT						= {0,	 Z};
BITVECT_DATA TRIG_CONSUME						= {0,	 aa};
BITVECT_DATA TRIG_TELL							= {0,	 bb};
BITVECT_DATA TRIG_FLAGOFF						= {0,	 cc};
BITVECT_DATA TRIG_LOCK							= {0,	 dd};
BITVECT_DATA TRIG_USE							= {0,	 ee};
BITVECT_DATA TRIG_UNLOCK						= {1,	 A};
BITVECT_DATA TRIG_OBJUNLOCK						= {1,	 B};
BITVECT_DATA TRIG_OPEN							= {1,	 C};
BITVECT_DATA TRIG_OBJLOCK						= {1,	 D};
BITVECT_DATA TRIG_CLOSE							= {1,	 E};
BITVECT_DATA TRIG_OBJCLOSE						= {1,	 F};
BITVECT_DATA TRIG_TICK_RANDOM					= {1,	 G};
BITVECT_DATA TRIG_OBJOPEN						= {1,	 H};
BITVECT_DATA TRIG_RESET							= {1,	 I};
BITVECT_DATA TRIG_UNEQUIP						= {1,	 J};
BITVECT_DATA TRIG_PREWEAR						= {1,	 K};
BITVECT_DATA TRIG_PREREMOVE						= {1,	 L};
BITVECT_DATA TRIG_TIME							= {1,	 M};
BITVECT_DATA TRIG_ASK							= {1,	 N};
BITVECT_DATA TRIG_MOUNT							= {1,	 O};
BITVECT_DATA TRIG_ONLOAD						= {1,	 P};
BITVECT_DATA TRIG_DISMOUNT						= {1,	 Q};
BITVECT_DATA TRIG_STAND							= {1,	 R};
BITVECT_DATA TRIG_ONEHIT						= {1,	 S};
BITVECT_DATA TRIG_HIT							= {1,	 T};
BITVECT_DATA TRIG_PRECOMMAND					= {1,	 U};
BITVECT_DATA TRIG_HORN							= {1,	 V};
BITVECT_DATA TRIG_PREGET						= {1,	 W};
BITVECT_DATA TRIG_KNOCK							= {1,	 X};

//room flagi
BITVECT_DATA ROOM_DARK							= {0,	 A};
BITVECT_DATA ROOM_MEMCLE						= {0,	 B};
BITVECT_DATA ROOM_NO_MOB						= {0,	 C};
BITVECT_DATA ROOM_INDOORS						= {0,	 D};
BITVECT_DATA ROOM_MEMDRU						= {0,	 E};
BITVECT_DATA ROOM_MEMMAG						= {0,	 F};
BITVECT_DATA ROOM_NOMAGIC						= {0,	 G};
BITVECT_DATA ROOM_LIGHT							= {0,	 H};
BITVECT_DATA ROOM_MAGICDARK						= {0,	 I};
BITVECT_DATA ROOM_PRIVATE						= {0,	 J};
BITVECT_DATA ROOM_SAFE							= {0,	 K};
BITVECT_DATA ROOM_SOLITARY						= {0,	 L};
BITVECT_DATA ROOM_PET_SHOP						= {0,	 M};
BITVECT_DATA ROOM_NO_RECALL						= {0,	 N};
BITVECT_DATA ROOM_SLIPPERY_FLOOR				= {0,	 O};
BITVECT_DATA ROOM_BOUNTY_OFFICE					= {0,	 P};
BITVECT_DATA ROOM_HEROES_ONLY					= {0,	 Q};
BITVECT_DATA ROOM_NEWBIES_ONLY					= {0,	 R};
BITVECT_DATA ROOM_LAW							= {0,	 S};
BITVECT_DATA ROOM_NOWHERE						= {0,	 T};
BITVECT_DATA ROOM_BANK							= {0,	 U};
BITVECT_DATA ROOM_NOHIDE						= {0,	 V};
BITVECT_DATA ROOM_INN							= {0,	 W};
BITVECT_DATA ROOM_TRAP							= {0,	 X};
BITVECT_DATA ROOM_NOZECHO						= {0,	 Y};
BITVECT_DATA ROOM_RANDDESC						= {0,	 Z};
BITVECT_DATA ROOM_NO_HERBS						= {0,	 aa};
BITVECT_DATA ROOM_CONSECRATE					= {0,	 bb};
BITVECT_DATA ROOM_DESECRATE						= {0,	 cc};
BITVECT_DATA ROOM_MEMBARD						= {0,	 dd};
BITVECT_DATA ROOM_NO_WEATHER					= {0,	 ee};
BITVECT_DATA ROOM_WATCH_TOWER					= {1,	 A};
BITVECT_DATA ROOM_MEMSHA						= {1,	 B};
BITVECT_DATA ROOM_HOARD					    = {1,	 C};
BITVECT_DATA ROOM_INVOKE						= {1,	 D};

struct flag_stat_type
{
    const struct flag_type *structure;
    bool stat;
};

/*****************************************************************************
 Name:		flag_stat_table
 Purpose:	This table catagorizes the tables following the lookup
 		functions below into stats and flags.  Flags can be toggled
 		but stats can only be assigned.  Update this table when a
 		new set of flags is installed.
 ****************************************************************************/
const struct flag_stat_type flag_stat_table[] =
{
/*  {	structure		stat	}, */
    {	area_flags,		FALSE	},
    {   sex_flags,		TRUE	},
    {   exit_flags,		FALSE	},
    {   door_resets,		TRUE	},
    {   type_flags,		TRUE	},
    {   wear_flags,		FALSE	},
    {   apply_flags,		TRUE	},
    {   wear_loc_flags,		TRUE	},
    {   wear_loc_strings,	TRUE	},
    {   container_flags,	FALSE	},
    {   form_flags,             FALSE   },
    {   part_flags,             FALSE   },
    {   ac_type,                TRUE    },
    {   size_flags,             TRUE    },
    {   position_flags,         TRUE    },
    {   position_trig_flags,    FALSE    },
    {   imm_flags,              FALSE   },
    {   weapon_class,           TRUE    },
    {   weapon_type2,           FALSE   },
    {   apply_types,		TRUE	},
    {   prewait_target,		TRUE	},
    {   dam_types,		    TRUE	},
    {   instrument_class,           TRUE    },
    {   totem_table,        TRUE },
    {   0,			0	}
};

/*****************************************************************************
 Name:		is_stat( table )
 Purpose:	Returns TRUE if the table is a stat table and FALSE if flag.
 Called by:	flag_value and flag_string.
 Note:		This function is local and used only in bit.c.
 ****************************************************************************/
bool is_stat( const struct flag_type *flag_table )
{
    int flag;

    for (flag = 0; flag_stat_table[flag].structure; flag++)
    {
	if ( flag_stat_table[flag].structure == flag_table
	  && flag_stat_table[flag].stat )
	    return TRUE;
    }
    return FALSE;
}

/*****************************************************************************
 Name:		flag_value( table, flag )
 Purpose:	Returns the value of the flags entered.  Multi-flags accepted.
 Called by:	olc.c and olc_act.c.
 ****************************************************************************/
int flag_value( const struct flag_type *flag_table, char *argument)
{
    char word[MAX_INPUT_LENGTH];
    int  bit;
    int  marked = 0;
    bool found = FALSE;

    if ( is_stat( flag_table ) )
	return flag_lookup(argument, flag_table);

    /*
     * Accept multiple flags.
     */
    for (; ;)
    {
        argument = one_argument( argument, word );

        if ( word[0] == '\0' )
	    break;

        if ( ( bit = flag_lookup( word, flag_table ) ) != NO_FLAG )
        {
            SET_BIT( marked, bit );
            found = TRUE;
        }
    }

    if ( found )
	return marked;
    else
	return NO_FLAG;
}

/*****************************************************************************
 Name:		flag_string( table, flags/stat )
 Purpose:	Returns string with name(s) of the flags or stat entered.
 Called by:	act_olc.c, olc.c, and olc_save.c.
 ****************************************************************************/
char *flag_string( const struct flag_type *flag_table, int bits )
{
	static char buf[ 2 ][ 512 ];
	static int cnt = 0;
	int flag;

	if ( ++cnt > 1 )
		cnt = 0;

	buf[ cnt ][ 0 ] = '\0';

	for ( flag = 0; flag_table[ flag ].name != NULL; flag++ )
	{
		if ( !is_stat( flag_table ) && IS_SET( bits, flag_table[ flag ].bit ) )
		{
			strcat( buf[ cnt ], " " );
			strcat( buf[ cnt ], flag_table[ flag ].name );
		}
		else
			if ( flag_table[ flag ].bit == bits )
			{
				strcat( buf[ cnt ], " " );
				strcat( buf[ cnt ], flag_table[ flag ].name );
				break;
			}
	}
	return ( buf[ cnt ][ 0 ] != '\0' ) ? buf[ cnt ] + 1 : "none";
}


/*
 * Kopiowanie bit�w z flagi flags_from do flagi_to
 */
void ext_flags_copy( long * flags_from, long * flags_to )
{
	int i;

	if ( !flags_from || !flags_to )
		return;

	for( i = 0; i < MAX_VECT_BANK; i++ )
		*(flags_to + i) = *(flags_from + i);
}

/*
 * Zwraca cz�� wsp�ln� dw�ch flag, np. cz�� wsp�lna 1001101 i 1010100 to 1000100
 */
long * ext_flags_intersection( long * flags1, long * flags2 )
{
	int i;
	static long intersection[MAX_VECT_BANK];

	if ( !flags1 || !flags2 )
		return FALSE;

	for( i = 0; i < MAX_VECT_BANK; i++ )
		intersection[i] = *(flags1 + i) & *(flags2 + i);

	return &intersection[0];
}

/*
 * Zwraca sum� dw�ch flag, np. suma 1001101 i 1010100 to 1011101
 */
long * ext_flags_sum( long * flags1, const long * flags2 )
{
	int i;
	static long sum[MAX_VECT_BANK];

	if ( !flags1 || !flags2 )
		return FALSE;

	for( i = 0; i < MAX_VECT_BANK; i++ )
		sum[i] = *(flags1 + i) | *(flags2 + i);

	return &sum[0];
}

/*
 * Zwraca r�nic� dw�ch flag, np. r�nica 1001101 i 1010100 to 0001001
 * (gasi na pierwszej te kt�re na drugim s� zapalone)
 */
long * ext_flags_diff( long * flags1, const long * flags2 )
{
	int i;
	static long diff[MAX_VECT_BANK];

	if ( !flags1 || !flags2 )
		return FALSE;

	for( i = 0; i < MAX_VECT_BANK; i++ )
	{
		diff[i] = *(flags1 + i) & ~(*(flags2 + i));
	}

	return &diff[0];
}

/*
 * Sprawdza czy flagi s� takie same
 */
bool ext_flags_same( long * flags1, long * flags2 )
{
	int i;
	bool same = TRUE;

	if ( !flags1 || !flags2 )
		return FALSE;

	for( i = 0; i < MAX_VECT_BANK; i++ )
	{
		if ( *(flags1 + i) != *(flags2 + i) )
		{
			same = FALSE;
			break;
		}
	}

	return same;
}

/*
 * Sprawdza czy flaga ma zapalone jakiekolwiek bity
 * Zwraca TRUE je�li nie ma zapalone
 */
bool ext_flags_none( long * flags )
{
	int i;
	bool none = TRUE;

	if ( !flags )
		return TRUE;

	for( i = 0; i < MAX_VECT_BANK; i++ )
	{
		if ( *(flags + i) != 0 )
		{
			none = FALSE;
			break;
		}
	}

	return none;
}

/*
 * Zeruje wszystkie bity
 */
void ext_flags_clear( long * flags )
{
	int i;

	if ( !flags )
		return;

	for( i = 0; i < MAX_VECT_BANK; i++ )
	{
		*(flags + i) = 0;
	}

}

/*
 * Zwraca flagi z zapalonymi bitami dla podanych w fomie tekstowej opcji oddzielonych spacjami
 */
long * ext_flag_value( const struct ext_flag_type * ext_flag_table, char * argument )
{
    char word[MAX_INPUT_LENGTH];
    BITVECT_DATA * bit;
    static long  marked[MAX_VECT_BANK];
    bool found = FALSE;

	ext_flags_clear( marked );

	/*
     * Accept multiple flags.
     */
    for (; ;)
    {
        argument = one_argument( argument, word );

        if ( word[0] == '\0' )
	    break;

		bit = ext_flag_lookup( word, ext_flag_table );
        if ( bit != &EXT_NONE )
        {
            EXT_SET_BIT( marked, *bit );
            found = TRUE;
        }
    }

    if ( found )
		return &marked[0];
    else
		return NULL;
}

/*
 * Zwraca oddzielone spacjami nazwy zapalonych na fladze bit�w
 */
char * ext_flag_string( const struct ext_flag_type *ext_flag_table, long * ext_bits )
{
	static char buf[ 2 ][ 512 ];
	static int cnt = 0;
	int flag;

	if ( ++cnt > 1 )
		cnt = 0;

	buf[ cnt ][ 0 ] = '\0';

	for ( flag = 0; ext_flag_table[ flag ].name != NULL; flag++ )
	{
		if ( EXT_IS_SET( ext_bits, (*ext_flag_table[ flag ].bitvect) ) )
		{
			strcat( buf[ cnt ], " " );
			strcat( buf[ cnt ], ext_flag_table[ flag ].name );
		}
	}
	return ( buf[ cnt ][ 0 ] != '\0' ) ? buf[ cnt ] + 1 : "none";
}

/*
 * Zwraca nazw� pojedynczego bitu
 */
char * ext_bit_name( const struct ext_flag_type *ext_flag_table, BITVECT_DATA * ext_bit )
{
	int flag;

	for ( flag = 0; ext_flag_table[ flag ].name != NULL; flag++ )
	{
		if ( ext_bit == ext_flag_table[ flag ].bitvect )
		{
			return ext_flag_table[ flag ].name;
		}
	}
	return NULL;
}

BITVECT_DATA * vector_to_pointer( const struct ext_flag_type *ext_flag_table, int bank, int vector )
{
	int flag;

	for ( flag = 0; ext_flag_table[ flag ].name != NULL; flag++ )
	{
		if ( bank == (*ext_flag_table[ flag ].bitvect).bank && vector == (*ext_flag_table[ flag ].bitvect).vector )
		{
			return ext_flag_table[ flag ].bitvect;
		}
	}
	return &AFF_NONE;
}

//rellik, funkcje obs�ugi nowych prostych-dlugich flag [20080517]

//max ile flag mamy jak chcemy policzyc sami to musimy wiedziec na ilu bajtach jest int zapisany w naszym systemie
//dla 32 bitowych na 4 bajtach wi�c b�dzie 10(bankow) * 4(bajty w ka�dym) * 8(bitow na bajt)
const int MAX_BIGFLAG = sizeof( BIGFLAG ) * 8;

bool set_bigflag( BIGFLAG *flaga, int nrbitu )
{
	int bank;
	int bit;
	const char *call = "bit.c => set_bigflag";

	if ( nrbitu >= MAX_BIGFLAG )
	{
	 save_debug_info( call, NULL, "pr�ba operowania na fladze przekraczaj�cej pojemno�� BIGFLAG", DEBUG_PROJECT_BIGFLAG, DEBUG_LEVEL_ERROR, TRUE );
	 return FALSE;
	}
	bank = nrbitu / ( sizeof( flaga->bank[0] ) * 8 );
	bit = nrbitu % ( sizeof( flaga->bank[0] ) * 8 );
	flaga->bank[ bank ] |= 1<<bit;
	return TRUE;
}

bool rmv_bigflag( BIGFLAG *flaga, int nrbitu )
{
	int bank;
	int bit;
	const char *call = "bit.c => rmv_bigflag";

	if ( nrbitu >= MAX_BIGFLAG )
	{
	 save_debug_info( call, NULL, "pr�ba operowania na fladze przekraczaj�cej pojemno�� BIGFLAG", DEBUG_PROJECT_BIGFLAG, DEBUG_LEVEL_ERROR, TRUE );
	 return FALSE;
	}
	bank = nrbitu / ( sizeof( flaga->bank[0] ) * 8 );
	bit = nrbitu % ( sizeof( flaga->bank[0] ) * 8 );
	flaga->bank[ bank ] &= ~( 1<<bit );
	return TRUE;
}

bool tgl_bigflag( BIGFLAG *flaga, int nrbitu )
{
	int bank;
	int bit;
	const char *call = "bit.c => tgl_bigflag";

	if ( nrbitu >= MAX_BIGFLAG )
	{
	 save_debug_info( call, NULL, "pr�ba operowania na fladze przekraczaj�cej pojemno�� BIGFLAG", DEBUG_PROJECT_BIGFLAG, DEBUG_LEVEL_ERROR, TRUE );
	 return FALSE;
	}
	bank = nrbitu / ( sizeof( flaga->bank[0] ) * 8 );
	bit = nrbitu % ( sizeof( flaga->bank[0] ) * 8 );
	flaga->bank[ bank ] ^= (1<<bit);
	return TRUE;
}

bool chk_bigflag( BIGFLAG *flaga, int nrbitu )
{
	int bank;
	int bit;

	DEBUG_INFO ( "bit.c:chk_bigflag:start" );

	const char *call = "bit.c => chk_bigflag";
	if ( nrbitu >= MAX_BIGFLAG )
	{
	DEBUG_INFO ( "bit.c:chk_bigflag:MAX_BIGFLAG" );
        save_debug_info( call, NULL, "pr�ba operowania na fladze przekraczaj�cej pojemno�� BIGFLAG", DEBUG_PROJECT_BIGFLAG, DEBUG_LEVEL_ERROR, TRUE );
        return FALSE;
	}

	DEBUG_INFO ( "bit.c:chk_bigflag:2" );
	if ( !flaga || !flaga->bank ) {
	DEBUG_INFO ( "bit.c:chk_bigflag:flag" );
	    sprintf( log_buf, "BUG: [bit.c:chk_bigflag] flaga lub flaga->bank nie s� zdefiniowane!" );
		log_string( log_buf );
		return FALSE;
	}

	DEBUG_INFO ( "bit.c:chk_bigflag:3" );
	bank = nrbitu / ( sizeof( flaga->bank[0] ) * 8 );
	DEBUG_INFO ( "bit.c:chk_bigflag:4" );
	bit = nrbitu % ( sizeof( flaga->bank[0] ) * 8 );

	DEBUG_INFO ( "bit.c:chk_bigflag:bit_shift_and_and" );
	return ( ( ( flaga->bank[ bank ] & ( 1<<bit ) ) != 0 )? TRUE: FALSE );
}
