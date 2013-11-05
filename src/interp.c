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
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Skrzetnicki Krzysztof (tener@tenet.pl                ) [Tener     ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: interp.c 12408 2013-06-12 12:29:02Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/interp.c $
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
#include "interp.h"
#include "shapeshifting.h"

bool check_social args( ( CHAR_DATA *ch, char *command, char *argument ) );
int  go_door      args( ( CHAR_DATA *ch, char *argument, bool verbose ) );
bool is_ignoring( CHAR_DATA *ch, char *argument );

/*
 * Command logging types.
 */
#define LOG_NORMAL	 0
#define LOG_ALWAYS	 1
#define LOG_NEVER	 2

/*
 * Log-all switch.
 */
bool fLogAll = FALSE;
/*
 * Log all sommunication.
 */
bool fLogComm = FALSE;

/*
 * Command table.
 */
const struct cmd_type cmd_table [] =
{
   /*
    * Komendy 'prywatne' do komunikacji z proxy
    */
   //   {"__set_address__","__set_address__", set_proxy_address, POS_DEAD, 0, LOG_ALWAYS, OPT_HIDEHELP|OPT_INFORMATION|OPT_MIND_MOVE },

    /*
     * Common movement commands.
     */
    { "n",          "n,",           do_north,       POS_STANDING,   0,  LOG_NEVER,  OPT_HIDEHELP|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },
    { "e",          "e",            do_east,        POS_STANDING,   0,  LOG_NEVER,  OPT_HIDEHELP|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },
    { "s",          "s",            do_south,       POS_STANDING,   0,  LOG_NEVER,  OPT_HIDEHELP|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },
    { "w",          "w",            do_west,        POS_STANDING,   0,  LOG_NEVER,  OPT_HIDEHELP|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },
    { "u",          "u",            do_up,          POS_STANDING,   0,  LOG_NEVER,  OPT_HIDEHELP|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },
    { "d",          "d",            do_down,        POS_STANDING,   0,  LOG_NEVER,  OPT_HIDEHELP|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },

    { "north",      "polnoc,",      do_north,       POS_STANDING,   0,  LOG_NEVER,  OPT_HIDEHELP|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },
    { "east",       "wschod",       do_east,        POS_STANDING,   0,  LOG_NEVER,  OPT_HIDEHELP|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },
    { "south",      "poludnie",     do_south,       POS_STANDING,   0,  LOG_NEVER,  OPT_HIDEHELP|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },
    { "west",       "zachod",       do_west,        POS_STANDING,   0,  LOG_NEVER,  OPT_HIDEHELP|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },
    { "up",         "gora",         do_up,          POS_STANDING,   0,  LOG_NEVER,  OPT_HIDEHELP|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },
    { "down",       "dol",          do_down,        POS_STANDING,   0,  LOG_NEVER,  OPT_HIDEHELP|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },
    { "enter",      "wejdz",        do_enter,       POS_STANDING,   0,  LOG_NEVER,  OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "cast",       "czaruj",       do_cast,        POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED|OPT_DZIALANIE_OFENSYWNE },
    { "buy",        "kup",          do_buy,         POS_RESTING,    0,  LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
    { "colour",     "kolory",       do_colour,      POS_DEAD,       0,  LOG_NORMAL, OPT_INFORMATION|OPT_NIEDLAZAKUTYCH },
    { "color",      "kolory",       do_colour,      POS_DEAD,       0,  LOG_NORMAL, OPT_INFORMATION|OPT_NIEDLAZAKUTYCH },
    { "exits",      "wyjscia",      do_exits,       POS_RESTING,    0,  LOG_NORMAL, OPT_MAY_REM_LISTEN },
    { "get",        "wez",          do_get,         POS_RESTING,    0,  LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
    { "group",      "grupa",        do_group,       POS_SLEEPING,   0,  LOG_NORMAL, 0 },
    { "hit",        "uderz",        do_kill,        POS_STANDING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "assist",     "wspomoz",      do_assist,      POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "inventory",  "inwentarz",    do_inventory,   POS_DEAD,       0,  LOG_NORMAL, OPT_MAY_REM_LISTEN|NIE_DLA_SHAPESHIFTED|OPT_NIEDLAZAKUTYCH },
    { "kill",       "zabij",        do_kill,        POS_STANDING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "look",       "patrz",        do_look,        POS_RESTING,    0,  LOG_NORMAL, OPT_MIND_MOVE },
    { "order",      "rozkaz",       do_order,       POS_RESTING,    0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "learn",      "ucz",          do_learn,       POS_STANDING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "introduce",  "poznaj",       do_introduce,   POS_STANDING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "memorize",   "zapamietaj",   do_memorize,    POS_RESTING,    0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN },
    { "mount",      "dosiadz",      do_mount,       POS_STANDING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
    { "dismount",   "zsiadz",       do_dismount,    POS_SLEEPING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "write",      "napisz",       do_write,       POS_SLEEPING,   0,  LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "read",       "przeczytaj",   do_read,        POS_SLEEPING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "delete",     "skasuj",       do_deletemesg,  POS_SLEEPING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "peek",       "podgladnij",   do_peek,        POS_RESTING,    0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED|OPT_DZIALANIE_OFENSYWNE },
    { "rest",       "odpocznij",    do_rest,        POS_SLEEPING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH },
    { "sit",        "usiadz",       do_sit,         POS_SLEEPING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH },
    { "stand",      "wstan",        do_stand,       POS_SLEEPING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_RECUPERATE },
    { "tell",       "powiedz",      do_tell,        POS_RESTING,    0,  LOG_NORMAL, OPT_NOT_IN_FORCE_FIELD|OPT_COMMUNICATION },
    { "unlock",     "odklucz",      do_unlock,      POS_RESTING,    0,  LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
    { "examine",    "obejrzyj",     do_examine,     POS_RESTING,    0,  LOG_NORMAL, OPT_MAY_REM_LISTEN|OPT_MIND_MOVE|OPT_NIEDLAZAKUTYCH },
    { "wake",       "obudz",        do_wake,        POS_SLEEPING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_RECUPERATE },
    { "evcheck",    "evcheck",      do_evcheck,     POS_SLEEPING,   W1, LOG_NORMAL, OPT_INFORMATION },
    { "study",      "studiuj",      do_study,       POS_RESTING,    0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN },
    { "addstat",    "addstat",      do_addstat,     POS_SLEEPING,   0,  LOG_NORMAL, OPT_INFORMATION },
    //shapeshifting
    { "shapeshift", "zmien forme",	do_shapeshift_list,	POS_RESTING,		W1,	LOG_NORMAL,	OPT_NIEDLAZAKUTYCH|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
    { "reform", 	"powroc",		do_reform,			POS_RESTING,		W1,	LOG_NORMAL,	OPT_NIEDLAZAKUTYCH|OPT_NOT_IN_FORCE_FIELD },
    { "throw",        "rzuæ",          do_throw,         POS_RESTING,    0,  LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
    { "knock",        "zapukaj",          do_knock,         POS_RESTING,    0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
    { "release",   "uwolnij",       do_release_spirit,   POS_DEAD,   0, LOG_NORMAL, OPT_MINIHOLD },

    /**
     * Informational commands.
     */
    { "affects",    "afekty",       do_affects,     POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION },
    { "bug",        "blad",         do_bug,         POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE},
    { "ide",        "pomys",        do_ide,         POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE},
    { "idea",       "pomysl",       do_idea,        POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE},
    { "commands",   "komendy",      do_commands,    POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "consider",   "ocen",         do_consider,    POS_RESTING,    0, LOG_NORMAL, OPT_MAY_REM_LISTEN|OPT_MIND_MOVE },
    { "credits",    "credits",      do_credits,     POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "equipment",  "ekwipunek",    do_equipment,   POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MAY_REM_LISTEN|NIE_DLA_SHAPESHIFTED },
    { "help",       "pomoc",        do_help,        POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "levels",     "poziomy",      do_levels,      POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "motd",       "motd",         do_motd,        POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "rules",      "zasady",       do_rules,       POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "score",      "postac",       do_score,       POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION },
    { "condition",  "kondycja",     do_condition,   POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION },
    { "claninfo",   "claninfo",     do_claninfo,    POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "title",      "tytul",        do_title,       POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION },
    { "skills",     "umiejetnosci", do_skills,      POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION },
    { "socials",    "emocje",       do_socials,     POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "spells",     "zaklecia",     do_spells,      POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION },
    { "story",      "historia",     do_story,       POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "time",       "czas",         do_time,        POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "mudinfo",    "mudinfo",      do_mudinfo,     POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
	{ "typ",        "typ",          do_typ,         POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "typo",       "literowka",    do_typo,        POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "who",        "kto",          do_who,         POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "tricks",     "triki",        do_tricks,      POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION },
    { "inflect",    "odmiana",      do_inflect,     POS_DEAD,       0, LOG_NORMAL, OPT_INFORMATION },
    { "friend",     "znajomy",      do_friend,      POS_RESTING,    0, LOG_NORMAL, OPT_COMMUNICATION|NIE_DLA_SHAPESHIFTED },
    { "trophy",     "trofea",       do_trophy,      POS_DEAD,       0, LOG_ALWAYS, OPT_INFORMATION },

    /*
     * Configuration commands.
     */
    { "alia",       "alia",         do_alia,        POS_DEAD,       0,  LOG_NORMAL, OPT_INFORMATION|OPT_HIDEHELP|OPT_MIND_MOVE },
    { "alias",      "alias",        do_alias,       POS_DEAD,       0,  LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "config",     "opcje",        do_config,      POS_DEAD,       0,  LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "description","opis",         do_description, POS_DEAD,       0,  LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "prompt",     "prompt",       do_prompt,      POS_DEAD,       0,  LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "ignore",     "ignoruj",      do_ignore,      POS_DEAD,       0,  LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "unalias",    "unalias",      do_unalias,     POS_DEAD,       0,  LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },

    /*
     * Communication commands.
     */
    { "afk",        "afk",          do_afk,         POS_SLEEPING,   W1,  LOG_NORMAL, OPT_INFORMATION|OPT_HIDEHELP },
    { "emote",      "emote",        do_emote,       POS_RESTING,    0,  LOG_NORMAL, OPT_HIDEHELP|OPT_COMMUNICATION },
//  { "pmote",      "pmote",        do_pmote,       POS_RESTING,    0,  LOG_NORMAL, 0 },
//  { ",",          ",",            do_emote,       POS_RESTING,    0,  LOG_NORMAL, OPT_HIDEHELP|OPT_COMMUNICATION },
    { "gtell",      "gtell",        do_gtell,       POS_RESTING,    0,  LOG_NORMAL, OPT_NOT_IN_FORCE_FIELD|OPT_COMMUNICATION },
    { "ctell",      "ctell",        do_clantell,    POS_RESTING,    0,  LOG_NORMAL, OPT_COMMUNICATION },
    { "clantell",   "gtell",        do_clantell,    POS_RESTING,    0,  LOG_NORMAL, OPT_COMMUNICATION },
    { ",",          ",",            do_clantell,    POS_RESTING,    0,  LOG_NORMAL, OPT_COMMUNICATION },
    { ";",          ";",            do_gtell,       POS_RESTING,    0,  LOG_NORMAL, OPT_HIDEHELP|OPT_NOT_IN_FORCE_FIELD|OPT_COMMUNICATION },
    { "reply",      "odpowiedz",    do_reply,       POS_SLEEPING,   0,  LOG_NORMAL, OPT_NOT_IN_FORCE_FIELD|OPT_COMMUNICATION },
    { "replay",     "replay",       do_replay,      POS_SLEEPING,   0,  LOG_NORMAL, OPT_NOT_IN_FORCE_FIELD },
    { "say",        "mow",          do_say,         POS_RESTING,    0,  LOG_NORMAL, OPT_NOT_IN_FORCE_FIELD|OPT_COMMUNICATION },
    { "sayto",      "mowdo",        do_sayto,       POS_RESTING,    0,  LOG_NORMAL, OPT_NOT_IN_FORCE_FIELD|OPT_COMMUNICATION },
    { "ask",        "spytaj",       do_ask,         POS_RESTING,    0,  LOG_NORMAL, OPT_NOT_IN_FORCE_FIELD|OPT_COMMUNICATION },
    { "/",          "/",            do_sayto,       POS_RESTING,    0,  LOG_NORMAL, OPT_HIDEHELP|OPT_NOT_IN_FORCE_FIELD|OPT_COMMUNICATION },
    { "'",          "'",            do_say,         POS_RESTING,    0,  LOG_NORMAL, OPT_HIDEHELP|OPT_NOT_IN_FORCE_FIELD|OPT_COMMUNICATION },
    { "shout",      "krzyknij",     do_shout,       POS_RESTING,    0,  LOG_NORMAL, OPT_NOT_IN_FORCE_FIELD|OPT_COMMUNICATION },
    { "yell",       "wrzasnij",     do_yell,        POS_RESTING,    0,  LOG_NORMAL, OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_COMMUNICATION },
    { "speak",      "rozmawiaj",    do_speak,       POS_RESTING,    0,  LOG_NORMAL, OPT_INFORMATION|OPT_MAY_REM_LISTEN },
    { "languages",  "jezyki",       do_languages,   POS_RESTING,    0,  LOG_NORMAL, OPT_INFORMATION },
    { "newbie",		"pocz±tkuj±cy",     do_newbie,      POS_MORTAL,     0,  LOG_NORMAL, OPT_INFORMATION|OPT_COMMUNICATION },
    { ".",			".",     do_newbie,      POS_MORTAL,     0,  LOG_NORMAL, OPT_INFORMATION|OPT_COMMUNICATION },

    /*
     * Object manipulation commands.
     */
    { "brandish",   "potrzasnij",   do_brandish,    POS_RESTING,    0, LOG_NORMAL, OPT_MINIHOLD|OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "close",      "zamknij",      do_close,       POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "drink",      "wypij",        do_drink,       POS_RESTING,    0, LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN },
    { "scan",       "rozejrzyj",    do_scan,        POS_RESTING,    0, LOG_NORMAL, OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE},
    { "drop",       "upusc",        do_drop,        POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "display",    "pokaz",        do_display,     POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN },
    { "empty",      "oproznij",     do_empty,       POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "eat",        "zjedz",        do_eat,         POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN },
    { "envenom",    "zatruj",       do_envenom,     POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN },
    { "fill",       "napelnij",     do_fill,        POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "devour",     "pozryj",       do_devour,      POS_STANDING,   0, LOG_NORMAL, 0 },
    { "give",       "daj",          do_give,        POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS |OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD},
    { "hold",       "chwyc",        do_hold,        POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN },
    { "list",       "cennik",       do_list,        POS_RESTING,    0, LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "lock",       "zaklucz",      do_lock,        POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "open",       "otworz",       do_open,        POS_RESTING,    0, LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "pick",       "wlam",         do_pick,        POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "pour",       "przelej",      do_pour,        POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN },
    { "put",        "wloz",         do_put,         POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "quaff",      "polknij",      do_quaff,       POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN },
    { "recite",     "recytuj",      do_recite,      POS_RESTING,    0, LOG_NORMAL, OPT_MINIHOLD|OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_DZIALANIE_OFENSYWNE },
    { "remove",     "zdejmij",      do_remove,      POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|NIE_DLA_SHAPESHIFTED },
    { "sell",       "sprzedaj",     do_sell,        POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "repair",     "zreperuj",     do_repair,      POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "estimate",   "oszacuj",      do_estimate,    POS_RESTING,    0, LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "try",        "przymierz",    do_try,         POS_STANDING,   0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
    { "take",       "wez",          do_get,         POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
    { "value",      "wycen",        do_value,       POS_RESTING,    0, LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "wear",       "zaloz",        do_wear,        POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|NIE_DLA_SHAPESHIFTED },
    { "weather",    "pogoda",       do_weather,     POS_RESTING,    0, LOG_NORMAL, OPT_MIND_MOVE },
    { "zap",        "potrzyj",      do_zap,         POS_RESTING,    0, LOG_NORMAL, OPT_MINIHOLD|OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "connect",    "polacz",       do_connect,     POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN },
    { "drag",       "ciagnij",      do_drag,        POS_STANDING,   0, LOG_NORMAL, OPT_NEEDHANDS|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "use",        "uzyj",         do_use,         POS_RESTING,    0, LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "wield",      "dzierz",       do_wield,       POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|NIE_DLA_SHAPESHIFTED },
    { "feed",       "nakarm",       do_feed,        POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "water",      "napoi",        do_water,       POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "horn",       "dmij",         do_horn,        POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "grip",       "uchwyæ",       do_double_grip, POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|NIE_DLA_SHAPESHIFTED },
    { "ungrip",     "puœ¶æ",         do_ungrip,      POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|NIE_DLA_SHAPESHIFTED },
    { "pipe",       "fajka",        do_pipe,        POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|NIE_DLA_SHAPESHIFTED },
    { "uncloak",    "zdemaskuj",    do_uncloak,     POS_RESTING,    0, LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|NIE_DLA_SHAPESHIFTED },
    { "illuminate", "rozswietl",    do_illuminate,  POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|NIE_DLA_SHAPESHIFTED },
    { "map",        "mapa",         do_map,         POS_SLEEPING,   0, LOG_NORMAL, 0 },
    { "reverse",    "odwróæ",       do_reverse,     POS_SLEEPING,   0, LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|NIE_DLA_SHAPESHIFTED },
    { "hoard",      "przechowaj",   do_hoard,       POS_STANDING,   0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "unhoard",    "odbierz",      do_unhoard,     POS_STANDING,   0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "identify",   "identyfikuj",  do_identify,    POS_STANDING,   0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },

    /*
     * Combat commands.
     */
    { "backstab",   "zadzgaj",      do_backstab,    POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "circle" ,    "circle",       do_circle,      POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "bash",       "powal",        do_bash,        POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "bs",         "bs",           do_backstab,    POS_FIGHTING,   0,  LOG_NORMAL, OPT_HIDEHELP|OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "berserk",    "szal",         do_berserk,     POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "wardance",   "wardance",     do_wardance,    POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "damage",     "damage",       do_damage,      POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "disarm",     "rozbroj",      do_disarm,      POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "flee",       "uciekaj",      do_flee,        POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "flurry",     "flurry",       do_flurry,      POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "kick",       "kopnij",       do_kick,        POS_FIGHTING,   0,	LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "charge",     "szarzuj",      do_charge,      POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "parry",      "paruj",        do_parry,       POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "dodge",      "unik",         do_dodge,       POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "rescue",     "ratuj",        do_rescue,      POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "surrender",  "poddaj",       do_surrender,   POS_RESTING,    0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN },
    { "stun",       "oglusz",       do_stun,        POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "smite",      "smite",        do_smite,       POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "trip",       "przewroc",     do_trip,        POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "mighty",     "mighty",       do_might,       POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "power",      "power",        do_power,       POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "critical",   "critical",     do_critical,    POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "crush",      "druzgocz",     do_crush,       POS_FIGHTING,   0,	LOG_NORMAL, OPT_MINIHOLD|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE|OPT_HIDEHELP },
    { "tail",       "tail",         do_tail,        POS_FIGHTING,   0,	LOG_NORMAL, OPT_MINIHOLD|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE|OPT_HIDEHELP },
    { "torment",    "torturuj",     do_torment,     POS_FIGHTING,   0,	LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "overwhelming","przyt³aczaj±ce",do_overwhelming,POS_FIGHTING, 0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "cleave",     "rozp³ataj",    do_cleave,      POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
	{ "vertical",	"pionowe",		do_vertical_slash,POS_FIGHTING, 0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
	{ "hustle",     "wepchnij",     do_hustle,      POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "fire breath", "ziej ogniem", do_fire_breath, POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE|OPT_HIDEHELP },
    { "frost breath", "ziej lodem", do_frost_breath, POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE|OPT_HIDEHELP },
    { "acid breath", "ziej kwasem", do_acid_breath, POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE|OPT_HIDEHELP },
    { "lightning breath", "ziej piorunami", do_lightning_breath, POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE|OPT_HIDEHELP },
    { "slam",       "uderz",        do_slam,        POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "sap",        "oglusz",       do_sap,         POS_FIGHTING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },


    /*
     * other commands.
     */

    { "lay",        "uzdrow",       do_lay,             POS_STANDING,   0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "healing",    "ulecz",        do_healing,         POS_STANDING,   0, LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "sharpen",    "naostrz",      do_sharpen,         POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN },
    { "lore",       "zbadaj",       do_lore,            POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN },
    { "recuperate", "letarg",       do_recuperate,      POS_RESTING,    0, LOG_NORMAL, OPT_REM_LISTEN },
    { "mend",       "napraw",       do_mend,            POS_STANDING,   0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN },
    { "skin",       "oprawiaj",     do_skin,            POS_FIGHTING,   0, LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
//  { "ucho",       "ucho",         do_ucho,            POS_FIGHTING,   0, LOG_NORMAL, OPT_HIDEHELP|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|NIE_DLA_SHAPESHIFTED },
    { "carve",       "wytnij",      do_carve,           POS_STANDING,   0, LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED }, /* CARVE */
    { "herbs",       "ziola",       do_herb,            POS_STANDING,   0, LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
// chyba na razie nie dla graczy?
//  { "garhal",     "garhal",       do_garhal,          POS_STANDING,   0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "turn",       "odpedzaj",     do_turn,            POS_FIGHTING,   0, LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "aura",       "aura",         do_demon_aura,      POS_STANDING,   0, LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "damn",       "przeklnij",    do_damn,            POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN },
    { "call",       "przywo³aj",    do_call_avatar,     POS_RESTING,    0, LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "control",    "kontroluj",    do_control_undead,  POS_STANDING,   0, LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "aid",        "pomó¿",        do_first_aid,       POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "touch",      "dotknij",      do_healing_touch,   POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "pray",       "modl",         do_pre_holy_prayer, POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "sanct",      "uswiec",       do_sanct,           POS_RESTING,    0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "mine",       "wydobadz",     do_mine,            POS_STANDING,   0, LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
    { "invoke",     "przyzwij",     do_invoke,          POS_RESTING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN },

     /*
     * Mob command interpreter (placed here for faster scan...)
     */
    { "mob",        "mob",          do_mob,         POS_DEAD,       0,  LOG_NEVER,  OPT_INFORMATION },

    /*
     * Miscellaneous commands.
     */
    { "track",       "trop",        do_track,       POS_STANDING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN },
    { "follow",      "podazaj",     do_follow,      POS_RESTING,    0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "go",          "idz",         do_go,          POS_STANDING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_MIND_MOVE },
    { "hide",        "ukryj",       do_hide,        POS_RESTING,    0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN },
    { "qui",         "wyj",         do_qui,         POS_DEAD,       0,  LOG_NORMAL, OPT_HIDEHELP|OPT_NIEDLAZAKUTYCH },
    { "quit",        "wyjdz",       do_quit2menu,   POS_DEAD,       0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN },
    { "offer",       "oferta",      do_offer,       POS_DEAD,       0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "rent",        "wynajmij",    do_rent,        POS_DEAD,       0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
    { "save",        "zapisz",      do_save,        POS_DEAD,       0,  LOG_NORMAL, OPT_INFORMATION|OPT_NIEDLAZAKUTYCH|OPT_MIND_MOVE },
    { "sleep",       "spij",        do_sleep,       POS_SLEEPING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN },
    { "sneak",       "skradaj",     do_sneak,       POS_STANDING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "unsneak",     "nieskradaj",  do_unsneak,     POS_STANDING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "split",       "podziel",     do_split,       POS_RESTING,    0,  LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "steal",       "ukradnij",    do_steal,       POS_STANDING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_MAY_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "visible",     "widzialnosc", do_visible,     POS_SLEEPING,   0,  LOG_NORMAL, 0 },
    { "deposit",     "wplac",       do_deposit,     POS_RESTING,    0,  LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "withdraw",    "wyplac",      do_withdraw,    POS_RESTING,    0,  LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|NIE_DLA_SHAPESHIFTED },
    { "reservation", "rezerwacja",  do_reservation, POS_SLEEPING,   0,  LOG_NORMAL, OPT_INFORMATION|OPT_MIND_MOVE },
    { "wizkomendy",  "wizkomendy",  do_wizkomendy,  POS_DEAD,       0,  LOG_NORMAL, OPT_INFORMATION },
    { "questlog",    "dziennik",    do_questlog,    POS_SLEEPING,  W1,  LOG_NORMAL, OPT_INFORMATION },
//  { "herb",        "ziola",       do_herb,        POS_STANDING,   0,  LOG_NORMAL, OPT_NEEDHANDS },
    { "bounty",      "nagroda",     do_bounty,      POS_RESTING,    0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },
    { "reward",      "zdobycz",     do_reward,      POS_RESTING,    0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD },

    /* Klanowe komendy */
    { "leav",       "leav",         do_leav,        POS_DEAD,       0,  LOG_NORMAL, 0 },
    { "leave",      "leave",        do_leave,       POS_DEAD,       0,  LOG_ALWAYS, 0 },
    { "clans",      "clans",        do_clans,       POS_DEAD,       0,  LOG_NORMAL, OPT_INFORMATION },
    { "initiate",   "initiate",     do_initiate,    POS_RESTING,    0,  LOG_NORMAL, 0 },
    { "demote",     "demote",       do_demote,      POS_RESTING,    0,  LOG_ALWAYS, 0 },
    { "promote",    "promote",      do_promote,     POS_RESTING,    0,  LOG_ALWAYS, 0 },
    { "exil",       "exil",         do_exil,        POS_RESTING,    0,  LOG_NORMAL, 0 },
    { "exile",      "exile",        do_exile,       POS_RESTING,    0,  LOG_ALWAYS, 0 },
    { "clanstate",  "clanstate",    do_clanstate,   POS_RESTING,    0,  LOG_ALWAYS, 0 },

    /* Skille barda */
    { "listen",     "s³uchaj",      do_listen,      POS_RESTING,    0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NOT_IN_FORCE_FIELD },
    { "remlisten",  "remlisten",    do_remlisten,   POS_FIGHTING,   W1, LOG_NORMAL, OPT_HIDEHELP },
    { "persuasion", "perswazja",    do_persuasion,  POS_STANDING,   0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NOT_IN_FORCE_FIELD|OPT_DZIALANIE_OFENSYWNE },
    { "play",       "graj",         do_play,        POS_RESTING,    0,  LOG_NORMAL, OPT_MINIHOLD|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN|OPT_NEEDHANDS|OPT_NOT_IN_FORCE_FIELD },
    { "tune",       "nastrój",      do_tune,        POS_RESTING,    0,  LOG_NORMAL, OPT_NEEDHANDS|OPT_NIEDLAZAKUTYCH|OPT_REM_LISTEN },

    /*
     * Immortal commands.
     */
    //NOWY
	{ ":",          ":",            do_immtalk,     POS_DEAD,      W1,  LOG_NORMAL, 0|OPT_HIDEHELP },
	{ "aedit",      "aedit",        do_aedit,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "alist",      "alist",        do_alist,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "asave",      "asave",        do_asave,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "board",      "board",        do_board,       POS_SLEEPING,  W1,  LOG_NORMAL, 0 },
	{ "clone",      "clone",        do_clone,       POS_DEAD,      W1,  LOG_ALWAYS, 0 },
	{ "edit",       "edit",         do_olc,         POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "fset",       "fset",         do_fset,        POS_SLEEPING,  W1,  LOG_NORMAL, 0 },
	{ "fstat",      "fstat",        do_fstat,       POS_SLEEPING,  W1,  LOG_NORMAL, 0 },
	{ "fvlist",     "fvlist",       do_fvlist,      POS_DEAD,      W1,  LOG_NORMAL, 0 }, // FREE VNUM LISTING by The Mage (c) 1998
	{ "holylight",  "holylight",    do_holylight,   POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "home",       "home",         do_home,        POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "immtalk",    "immtalk",      do_immtalk,     POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "load",       "load",         do_load,        POS_DEAD,      W1,  LOG_ALWAYS, 0 },
	{ "medit",      "medit",        do_medit,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "memory",     "memory",       do_memory,      POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "mlist",      "mlist",        do_mlist,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "moblookup",  "moblookup",    do_moblookup,   POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "mpdump",     "mpdump",       do_mpdump,      POS_DEAD,      W1,  LOG_NEVER,  0 },
	{ "mpedit",     "mpedit",       do_mpedit,      POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "mplist",     "mplist",       do_mplist,      POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "mpstat",     "mpstat",       do_mpstat,      POS_DEAD,      W1,  LOG_NEVER,  0 },
	{ "mwhere",     "mwhere",       do_mwhere,      POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "nosi",		"nosi",			do_nosi,		POS_DEAD,	   W1,  LOG_NORMAL, 0 },
	{ "objlookup",  "objlookup",    do_objlookup,   POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "oedit",      "oedit",        do_oedit,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "olist",      "olist",        do_olist,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "opdump",     "opdump",       do_opdump,      POS_DEAD,      W1,  LOG_NEVER,  0 },
	{ "opedit",     "opedit",       do_opedit,      POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "oplist",     "oplist",       do_oplist,      POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "opstat",     "opstat",       do_opstat,      POS_DEAD,      W1,  LOG_NEVER,  0 },
	{ "otype",      "otype",        do_otype,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "owear",      "owear",        do_owear,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "owhere",     "owhere",       do_owhere,      POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "peace",      "peace",        do_peace,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "prefix",     "prefix",       do_prefix,      POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "purge",      "purge",        do_purge,       POS_DEAD,      W1,  LOG_ALWAYS, 0 },
	{ "randtest",	"randtest",		do_randtest,    POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "rdedit",     "rdedit",       do_rdedit,      POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "redit",      "redit",        do_redit,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "resets",     "resets",       do_resets,      POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "restore",    "restore",      do_restore,     POS_DEAD,      W1,  LOG_ALWAYS, 0 },
	{ "return",     "return",       do_return,      POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "rlist",      "rlist",        do_rlist,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "rlookup",    "rlookup",      do_rlookup,     POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "roomlookup", "roomlookup",   do_roomlookup,  POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "rpdump",     "rpdump",       do_rpdump,      POS_DEAD,      W1,  LOG_NEVER,  0 },
	{ "rpedit",     "rpedit",       do_rpedit,      POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "rplist",     "rplist",       do_rplist,      POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "rpstat",     "rpstat",       do_rpstat,      POS_DEAD,      W1,  LOG_NEVER,  0 },
	{ "secret",     "secret",       do_secret,      POS_RESTING,   W1,  LOG_NORMAL, 0 },
	{ "sifollow",	"sifollow",		do_sifollow, 	POS_DEAD,	   W1,  LOG_NORMAL, 0 },
	{ "stat",       "stat",         do_stat,        POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "tedit",      "tedit",        do_tedit,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "tlist",      "tlist",        do_tlist,       POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "wizconfig",  "wizconfig",    do_wizconfig,   POS_DEAD,      W1,  LOG_NORMAL, 0 },
	{ "wizhelp",    "wizhelp",      do_wizhelp,     POS_DEAD,      W1,  LOG_NORMAL, 0 },

//TWÓRCA
	{ "at",         "at",           do_at,          POS_DEAD,      W2,  LOG_NORMAL, 0 },
	{ "bsedit",     "bsedit",       do_bsedit,      POS_DEAD,      W2,  LOG_NORMAL, 0 },
	{ "bslist",     "bslist",       do_bslist,      POS_DEAD,      W2,  LOG_NORMAL, 0 },
	{ "goto",       "goto",         do_goto,        POS_DEAD,      W2,  LOG_NORMAL, 0 },
	{ "grouplist",  "grouplist",    do_grouplist,   POS_DEAD,      W2,  LOG_NORMAL, 0 },
	{ "poofin",     "poofin",       do_bamfin,      POS_DEAD,      W2,  LOG_NORMAL, 0 },
	{ "poofout",    "poofout",      do_bamfout,     POS_DEAD,      W2,  LOG_NORMAL, 0 },
	{ "regions",    "regions",      do_regions,     POS_DEAD,      W2,  LOG_NORMAL, 0 },
	{ "rmap",       "rmap",         do_rmap,        POS_DEAD,      W2,  LOG_NORMAL, 0 },
	{ "switch",     "switch",       do_switch,      POS_DEAD,      W2,  LOG_ALWAYS, 0 },
	{ "teleport",   "teleport",     do_transfer,    POS_DEAD,      W2,  LOG_ALWAYS, 0 },
	{ "transfer",   "transfer",     do_transfer,    POS_DEAD,      W2,  LOG_ALWAYS, 0 },
	{ "violate",    "violate",      do_violate,     POS_DEAD,      W2,  LOG_ALWAYS, 0 },
	{ "vnum",       "vnum",         do_vnum,        POS_DEAD,      W2,  LOG_NORMAL, 0 },
	{ "wiznet",     "wiznet",       do_wiznet,      POS_DEAD,      W2,  LOG_NORMAL, 0 },
//STRA¯NIK
	{ "advance",    "advance",      do_advance,     POS_DEAD,      W3,  LOG_ALWAYS, 0 },
	{ "disconnect", "disconnect",   do_disconnect,  POS_DEAD,      W3,  LOG_ALWAYS, 0 },
	{ "echo",       "echo",         do_recho,       POS_DEAD,      W3,  LOG_ALWAYS, 0 },
	{ "force",      "force",        do_force,       POS_DEAD,      W3,  LOG_ALWAYS, 0 },
	{ "freeze",     "freeze",       do_freeze,      POS_DEAD,      W3,  LOG_ALWAYS, 0 },
	{ "gecho",      "gecho",        do_echo,        POS_DEAD,      W3,  LOG_ALWAYS, 0 },
	{ "hbedit",     "hbedit",       do_hbedit,      POS_DEAD,      W3,  LOG_NORMAL, 0 },
	{ "hedit",      "hedit",        do_hedit,       POS_DEAD,      W3,  LOG_NORMAL, 0 },
	{ "imotd",      "imotd",        do_imotd,       POS_DEAD,      W3,  LOG_NORMAL, 0 },
	{ "names",      "names",        do_names,       POS_DEAD,      W3,  LOG_ALWAYS, 0 },
	{ "pecho",      "pecho",        do_pecho,       POS_DEAD,      W3,  LOG_ALWAYS, 0 },
	{ "snoop",      "snoop",        do_snoop,       POS_DEAD,      W3,  LOG_ALWAYS, 0 },
	{ "sockets",    "sockets",      do_sockets,     POS_DEAD,      W3,  LOG_NORMAL, 0 },
	{ "string",     "string",       do_string,      POS_DEAD,      W3,  LOG_ALWAYS, 0 },
	{ "zecho",      "zecho",        do_zecho,       POS_DEAD,      W3,  LOG_ALWAYS, 0 },
	{ "rresources", "rresources", 	do_rresources, 	POS_DEAD, 		 W3,  LOG_ALWAYS, 0 },

//REGENT
	{ "allow",      "allow",        do_allow,       POS_DEAD,      W4,  LOG_ALWAYS, 0 },
	{ "unban",      "unban",        do_allow,       POS_DEAD,      W4,  LOG_ALWAYS, 0 },
	{ "ban",        "ban",          do_permban,     POS_DEAD,      W4,  LOG_ALWAYS, 0 },
	{ "blocktell",  "blocktell",    do_blocktell,   POS_DEAD,      W4,  LOG_ALWAYS, 0 },
	{ "blocknewbie", "blocknewbie", do_blocknewbie, POS_DEAD,      W4,  LOG_ALWAYS, 0 },
	{ "deny",       "deny",         do_deny,        POS_DEAD,      W4,  LOG_ALWAYS, 0 },
	{ "destro",     "destro",       do_destro,      POS_DEAD,      W4,  LOG_NORMAL, 0 },
	{ "destroy",    "destroy",      do_destroy,     POS_DEAD,      W4,  LOG_ALWAYS, 0 },
	{ "hosts",      "hosts",        do_hosts,       POS_DEAD,      W4,  LOG_NORMAL, 0 },
	{ "incognito",  "incognito",    do_incognito,   POS_DEAD,      W4,  LOG_NORMAL, 0 },
	{ "invis",      "invis",        do_invis,       POS_DEAD,      W4,  LOG_NORMAL, 0 },
	{ "match",      "match",        do_match,       POS_DEAD,      W4,  LOG_NORMAL, 0 },
	{ "newlock",    "newlock",      do_newlock,     POS_DEAD,      W4,  LOG_ALWAYS, 0 },
	{ "nochannels", "nochannels",   do_nochannels,  POS_DEAD,      W4,  LOG_ALWAYS, 0 },
	{ "noemote",    "noemote",      do_noemote,     POS_DEAD,      W4,  LOG_ALWAYS, 0 },
	{ "noshout",    "noshout",      do_noshout,     POS_DEAD,      W4,  LOG_ALWAYS, 0 },
	{ "passmatch",  "passmatch",    do_match,       POS_DEAD,      W4,  LOG_NORMAL, 0 },
	{ "permban",    "permban",      do_permban,     POS_DEAD,      W4,  LOG_ALWAYS, 0 },
	{ "sla",        "sla",          do_sla,         POS_DEAD,      W4,  LOG_NORMAL, OPT_HIDEHELP },
	{ "slay",       "slay",         do_slay,        POS_DEAD,      W4,  LOG_ALWAYS, 0 },
	{ "smote",      "smote",        do_smote,       POS_DEAD,      W4,  LOG_NORMAL, 0 },
	{ "wizinvis",   "wizinvis",     do_invis,       POS_DEAD,      W4,  LOG_NORMAL, 0 },

//SÊDZIA
	{ "copyover",   "copyover",     do_copyover,    POS_DEAD,      W5,  LOG_ALWAYS, 0 },
	{ "cstat",      "cstat",        do_cstat,       POS_DEAD,      W5,  LOG_NORMAL, 0 },
	{ "lstat",      "lstat",        do_lstat,       POS_SLEEPING,  W5,  LOG_NORMAL, 0 },
	{ "ltstat",     "ltstat",       do_ltstat,      POS_SLEEPING,  W5,  LOG_NORMAL, 0 },
	{ "prewait",    "prewait",      do_prewait,     POS_DEAD,      W5,  LOG_ALWAYS, 0 },
	{ "sedit",      "sedit",        do_sedit,       POS_DEAD,      W5,  LOG_NORMAL, 0 },
	{ "set",        "set",          do_set,         POS_DEAD,      W5,  LOG_ALWAYS, 0 },
	{ "setclan",    "setclan",      do_setclan,     POS_DEAD,      W5,  LOG_ALWAYS, 0 },
	{ "setclan",    "setclan",      do_setclan,     POS_DEAD,      W5,  LOG_ALWAYS, 0 },
	{ "timeshift",  "timeshift",    do_timeshift,   POS_DEAD,      W5,  LOG_ALWAYS, 0 },
	{ "trust",      "trust",        do_trust,       POS_DEAD,      W5,  LOG_ALWAYS, 0 },

//LORD
	{ "amod",       "amod",         do_amod,        POS_DEAD,      W6,  LOG_NORMAL, 0 },
	{ "aset",       "aset",         do_aset,        POS_DEAD,      W6,  LOG_ALWAYS, 0 },
	{ "astat",      "astat",        do_astat,       POS_DEAD,      W6,  LOG_NORMAL, 0 },
	{ "sitable",		"sitable",			do_sitable,			POS_DEAD,			 W6,	LOG_NORMAL, 0 },
	{ "silist",			"silist",				do_silist,			POS_DEAD,			 W6,	LOG_NORMAL, 0 },
	{ "dump",       "dump",         do_dump,        POS_DEAD,      W6,  LOG_ALWAYS, 0 },
	{ "flag",       "flag",         do_flag,        POS_DEAD,      W6,  LOG_ALWAYS, 0 },
	{ "log",        "log",          do_log,         POS_DEAD,      W6,  LOG_ALWAYS, 0 },
	{ "lset",       "lset",         do_lset,        POS_SLEEPING,  W6,  LOG_NORMAL, 0 },
	{ "ltset",      "ltset",        do_ltset,       POS_SLEEPING,  W6,  LOG_NORMAL, 0 },
	{ "protect",    "protect",      do_protect,     POS_DEAD,      W6,  LOG_ALWAYS, 0 },
	{ "reboo",      "reboo",        do_reboo,       POS_DEAD,      W6,  LOG_NORMAL, OPT_HIDEHELP },
	{ "reboot",     "reboot",       do_reboot,      POS_DEAD,      W6,  LOG_ALWAYS, 0 },
	{ "rename",     "rename",       do_rename,      POS_DEAD,      W6,  LOG_ALWAYS, 0 },
	{ "shutdow",    "shutdow",      do_shutdow,     POS_DEAD,      W6,  LOG_NORMAL, OPT_HIDEHELP },
	{ "shutdown",   "shutdown",     do_shutdown,    POS_DEAD,      W6,  LOG_ALWAYS, 0 },
	{ "timeset",    "timeset",      do_timeset,     POS_DEAD,      W6,  LOG_ALWAYS, 0 },
	{ "wizlock",    "wizlock",      do_wizlock,     POS_DEAD,      W6,  LOG_ALWAYS, 0 },
	{ "wizset",     "wizset",       do_wizset,      POS_DEAD,      W6,  LOG_ALWAYS, 0 },
	{ "wizstat",    "wizstat",      do_wizstat,     POS_DEAD,      W6,  LOG_NORMAL, 0 },
	{ "debug",			"debug",			do_debug,			POS_DEAD,			 W6,	LOG_NORMAL, 0 },

//pozostale
	{ "meditation", "medytuj",      do_meditate,    POS_RESTING,    0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH },
	{ "bandage",    "bandazuj",     do_bandage,     POS_RESTING,    0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_NEEDHANDS },
	{ "trap",       "pulapka",      do_trap,        POS_STANDING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_NEEDHANDS },
	{ "target",     "cel",          do_target,      POS_FIGHTING,   0,  LOG_NORMAL, OPT_NIEDLAZAKUTYCH|OPT_DZIALANIE_OFENSYWNE },

    /*
     * End of list.
     */
    { "",           "",             0,              POS_DEAD,       0,  LOG_NORMAL, 0 }
};

/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */

void interpret( CHAR_DATA *ch, char *argument )
{
    char command[ MAX_INPUT_LENGTH ];
    char logline[ MAX_INPUT_LENGTH ];
    char buf[ MAX_INPUT_LENGTH ];
    int cmd;
    int wiz_conf = 0;
    bool found, check;
    int position;
    int dir, skill;
    EXIT_DATA* pexit;

    /*
     * Strip leading spaces.
     */
    while ( isspace( *argument ) )
        argument++;

    if ( argument[ 0 ] == '\0' )
        return ;

	if ( !IS_NPC(ch) && IS_IMMORTAL(ch) && IS_SET( ch->pcdata->wiz_conf, W4 ) && !IS_SET( ch->pcdata->wiz_conf, W5 ) )
    {
        append_file_format_daily( ch, REGENT_LOG_FILE, argument );
    }

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC( ch )
         && ( ( EXT_IS_SET( ch->act, PLR_FREEZE ) ||
                ( IS_AFFECTED( ch, AFF_PARALYZE ) && !IS_IMMORTAL( ch ) ) ) ) )
    {
        print_char( ch, "Jeste¶ ca³kowicie sparali¿owan%c!\n\r", ch->sex == 2 ? 'a' : ch->sex == 0 ? 'e' : 'y' );
        return ;
    }

	//wyjscia wirtualne
    if ( ch->in_room )
    {
        found = FALSE;
        for ( dir = 0; dir < MAX_DIR; dir++ )
        {
            if ( ( pexit = ch->in_room->exit[ dir ] ) != NULL
                 && pexit->vName != NULL
                 && pexit->vName[ 0 ] != '\0' )
            {
                found = TRUE;
                break;
            }
        }

        if ( found
             && go_door( ch, argument, FALSE ) >= 0 )
        {
            sprintf( buf, "%s", argument );
            if ( !IS_NPC( ch ) && EXT_IS_SET( ch->act, PLR_COMMANDSPL ) )
                sprintf( argument, "idz %s", buf );
            else
                sprintf( argument, "go %s", buf );
        }
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );

    if ( !isalpha( argument[ 0 ] ) && !isdigit( argument[ 0 ] ) )
    {
        command[ 0 ] = argument[ 0 ];
        command[ 1 ] = '\0';
        argument++;
        while ( isspace( *argument ) )
            argument++;
    }
    else
    {
        argument = one_argument( argument, command );
    }

    /*
     * Look for command in command table.
     */

    found = FALSE;

    if ( ch->desc != NULL && ch->desc->original != NULL )
        wiz_conf = ch->desc->original->pcdata->wiz_conf;
    else
        wiz_conf = ch->pcdata ? ch->pcdata->wiz_conf : 0;

    if ( ( !IS_NPC( ch ) && EXT_IS_SET( ch->act, PLR_COMMANDSPL ) )
         || ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) && ch->master && !IS_NPC( ch->master ) && EXT_IS_SET( ch->master->act, PLR_COMMANDSPL ) ) )
    {
        for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
        {
            if ( NOPOL( command[ 0 ] ) == NOPOL( cmd_table[ cmd ].name_pl[ 0 ] )
                 && !str_prefix( command, cmd_table[ cmd ].name_pl )
                 && (
                     ( cmd_table[ cmd ].level > 2
                   && IS_SET( wiz_conf, cmd_table[ cmd ].level ) )
                     || cmd_table[ cmd ].level < 3 )
               )
            {
                found = TRUE;
                break;
            }
        }

    }
    else
    {
        for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
        {
            if ( NOPOL( command[ 0 ] ) == NOPOL( cmd_table[ cmd ].name[ 0 ] )
                 && !str_prefix( command, cmd_table[ cmd ].name )
                 && (
                     ( cmd_table[ cmd ].level > 2 &&
                       ( !IS_NPC( ch ) || ( ch->desc != NULL && ch->desc->original != NULL ) )
                       && IS_SET( wiz_conf, cmd_table[ cmd ].level )
                       )
                     || cmd_table[ cmd ].level < 3 )
               )
            {
                found = TRUE;
                break;
            }
        }
    }

    /*
     * Log and snoop.
     */
    if ( cmd_table[ cmd ].log != LOG_NEVER )
    {
        if ( ( !IS_NPC( ch ) && EXT_IS_SET( ch->act, PLR_LOG ) )
             || ( !IS_NPC( ch ) && fLogAll )
             || cmd_table[ cmd ].log == LOG_ALWAYS )
        {
            sprintf( log_buf, "-Log [%5d] %s: %s", ch->in_room ? ch->in_room->vnum : 0, ch->name, logline );
            wiznet( log_buf, ch, NULL, WIZ_SECURE, 0, get_trust( ch ) );
            log_string( log_buf+1 );
        }
    }

    /* dirty hack: We log everything */
    if ( !IS_NPC( ch ) )
       {
	  sprintf( log_buf, "-CMD [%5d] %s (%s)", ch->in_room ? ch->in_room->vnum : 0, ch->name, logline );
	  log_string( log_buf+1 );
       }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
        sprintf( buf, "%s%% ", ch->name );
        write_to_buffer( ch->desc->snoop_by, buf, 0 );
        write_to_buffer( ch->desc->snoop_by, logline, 0 );
        write_to_buffer( ch->desc->snoop_by, "\n\r", 2 );
    }

    if ( !found )
    {
		if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
		{
			send_to_char( "Huh?\n\r", ch );
			return;
		}
		else if ( !check_social( ch, command, argument ) )
		{
            send_to_char( "Huh?\n\r", ch );
		}
        else if ( !IS_IMMORTAL( ch ) )
        {
            if ( IS_AFFECTED( ch, AFF_MEDITATION ) )
            {
                print_char( ch, "Przerywasz medytacje.\n\r" );
                affect_strip( ch, gsn_meditation );
            }

            if ( IS_SET( ch->comm, COMM_AFK ) && !IS_IMMORTAL( ch ) )
            {
                send_to_char( "Tryb AFK wy³±czony. Wpisz {Rreplay{x, ¿eby sprawdziæ czy kto¶ co¶ chcia³.\n\r", ch );
                REMOVE_BIT( ch->comm, COMM_AFK );
            }

            EXT_REMOVE_BIT( ch->affected_by, AFF_HIDE );
        }

        return;
    }

	if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
	{
		static char bufor[ MAX_INPUT_LENGTH + 19 ];
		if ( !IS_SET( cmd_table[ cmd ].options, OPT_MIND_MOVE ) )
		{
			send_to_char( "W obecnym stanie nie jeste¶ w stanie kontrolowaæ swojego cia³a.\n\r", ch );
			return;
		}

		sprintf( bufor, "%s (astral journey) ", cmd_table[ cmd ].name );
		DEBUG_INFO( bufor );
		( *cmd_table[ cmd ].do_fun ) ( ch, argument );
		DEBUG_INFO( NULL );

		return;
	}

    if ( get_spirit( ch ) && cmd_table[ cmd ].do_fun != do_release_spirit )
    {
        return;
    }


    if ( !IS_IMMORTAL( ch ) && !IS_SET( cmd_table[ cmd ].options, OPT_INFORMATION ) )
    {
        if ( IS_AFFECTED( ch, AFF_MEDITATION ) )
        {
            print_char( ch, "Przerywasz medytacje.\n\r" );
            affect_strip( ch, gsn_meditation );
        }

        if ( IS_AFFECTED( ch, AFF_INVOKE ) )
        {
            print_char( ch, "Przerywasz rytual.\n\r" );
            AFFECT_DATA *paf;
            AFFECT_DATA *paf_next;

            for ( paf = ch->affected; paf != NULL; paf = paf_next )
            {
               paf_next = paf->next;
                if ( paf->bitvector == &AFF_INVOKE )
                {
                    affect_remove( ch, paf );
                }
            }
        }

        if ( IS_SET( ch->comm, COMM_AFK ) && !IS_IMMORTAL( ch ) )
        {
            send_to_char( "Tryb AFK wy³±czony. Wpisz {Rreplay{x, ¿eby sprawdziæ czy kto¶ co¶ chcia³.\n\r", ch );
            REMOVE_BIT( ch->comm, COMM_AFK );
        }

        if (
                TRUE
                && cmd_table[ cmd ].do_fun != do_backstab
                && cmd_table[ cmd ].do_fun != do_hide
                && cmd_table[ cmd ].do_fun != do_steal
           )
        {
            EXT_REMOVE_BIT( ch->affected_by, AFF_HIDE );
        }

        if (
                TRUE
                && cmd_table[ cmd ].do_fun != do_north
                && cmd_table[ cmd ].do_fun != do_east
                && cmd_table[ cmd ].do_fun != do_south
                && cmd_table[ cmd ].do_fun != do_west
                && cmd_table[ cmd ].do_fun != do_up
                && cmd_table[ cmd ].do_fun != do_down
                && cmd_table[ cmd ].do_fun != do_go
            )
        {
            EXT_REMOVE_BIT( ch->affected_by, AFF_SNEAK_INVIS ); 
        }
    }

    if (( IS_AFFECTED( ch, AFF_HEALING_TOUCH ) ) && !IS_SET( cmd_table[ cmd ].options, OPT_INFORMATION ))
        do_function( ch, &abort_healing_touch, "" );

    if ( is_affected(ch, gsn_listen) && IS_SET( cmd_table[ cmd ].options, OPT_REM_LISTEN ))
    {
        wait_function(ch, 1, &do_remlisten, "");
        send_to_char( skill_table[gsn_listen].msg_off, ch );
		send_to_char( "\n\r", ch );
	}

	if ( is_affected(ch, gsn_listen) && IS_SET( cmd_table[ cmd ].options, OPT_MAY_REM_LISTEN ))
    {
        if ( ( skill = get_skill( ch, gsn_listen ) )*3/2 < number_range(1,100) )
        {
            wait_function(ch, 1, &do_remlisten, "");
            send_to_char( skill_table[gsn_listen].msg_off, ch );
		    send_to_char( "\n\r", ch );
		}
		else
		{
		    check_improve(ch, NULL, gsn_listen, TRUE, 85 );
		}
	}

// tym co maj± na sobie recuperate, pozwalamy na niewiele rzeczy
// komentuje, recup to nie medytacja! - Kainti 19 czerw 2005
/*	if ( !IS_IMMORTAL( ch ) && IS_AFFECTED( ch, AFF_RECUPERATE )
		&& !( IS_SET( cmd_table[ cmd ].options, OPT_RECUPERATE ) || IS_SET( cmd_table[ cmd ].options, OPT_INFORMATION ) )
	   )
	{
		print_char( ch, "Jeste¶ tak odprê¿on%c, ¿e nie chcesz tego zrobiæ.\n\r", ch->sex == 2 ? 'a' : ch->sex == 0 ? 'e' : 'y' );
		return;
	}*/

    if ( !IS_IMMORTAL( ch ) && IS_AFFECTED( ch, AFF_DAZE ) && IS_SET( cmd_table[ cmd ].options, OPT_MINIHOLD ) )
    {
		print_char( ch, "Jeste¶ zbyt oszo³omion%c, ¿eby to zrobiæ!\n\r", ch->sex == 2 ? 'a' : ch->sex == 0 ? 'e' : 'y' );
		return;
    }

    if ( !IS_IMMORTAL( ch ) && IS_AFFECTED( ch, AFF_FEAR ) && IS_SET( cmd_table[ cmd ].options, OPT_DZIALANIE_OFENSYWNE ) )
    {
		print_char( ch, "Jeste¶ zbyt przestraszon%c, ¿eby to zrobiæ!\n\r", ch->sex == 2 ? 'a' : ch->sex == 0 ? 'e' : 'y' );
		return;
    }

    if ( (!IS_IMMORTAL( ch ) && IS_AFFECTED( ch, AFF_RAZORBLADED ) && IS_SET( cmd_table[ cmd ].options, OPT_NEEDHANDS )) ||
	 (!IS_IMMORTAL( ch ) && IS_AFFECTED( ch, AFF_BEAST_CLAWS ) && IS_SET( cmd_table[ cmd ].options, OPT_NEEDHANDS )))
    {
		send_to_char( "W tym momencie nie jeste¶ w stanie tego zrobiæ, twoje d³onie s± niesprawne.\n\r", ch );
		return;
    }

    if ( !IS_IMMORTAL( ch ) && IS_AFFECTED( ch, AFF_ZAKUTY ) && IS_SET( cmd_table[ cmd ].options, OPT_NIEDLAZAKUTYCH ))
    {
		send_to_char( "Jeste¶ zakut<&y/a/e>, ³añcuchy uniemo¿liwiaj± ci wykonanie tej czynno¶ci.\n\r", ch );
		return;
    }

    if ( !IS_IMMORTAL( ch ) && IS_AFFECTED( ch, AFF_FORCE_FIELD ) && IS_SET( cmd_table[ cmd ].options, OPT_NOT_IN_FORCE_FIELD ))
    {
		send_to_char( "Jeste¶ wewn±trz kopu³y mocy, nie jeste¶ w stanie tego zrobiæ.\n\r", ch );
		return;
    }
    if ( !IS_IMMORTAL( ch ) && ch->ss_data && IS_SET( cmd_table[ cmd ].options, NIE_DLA_SHAPESHIFTED ))
	{
		send_to_char( "Jeste¶ przemienion<&y/a/e> w inn± formê, nie jeste¶ w stanie tego zrobiæ.\n\r", ch);
		return;
	}

	if ( !IS_NPC( ch ) && IS_SET( cmd_table[ cmd ].options, OPT_COMMUNICATION ) )
	{
		sprintf( log_buf, "-Log %s [%5d]: %s", ch->name, ch->in_room ? ch->in_room->vnum : 0, logline );
		wiznet( log_buf, ch, NULL, WIZ_COMM, 0, get_trust( ch ) );
		log_string( log_buf+1 );
	}
    /*
     * Character not in position for command?
     */

    /* zjebany na maxa patch, position podczas walki sie wypierdala...*/
    if ( ch->position == POS_STANDING && ch->fighting )
        position = POS_FIGHTING;
    else
        position = ch->position;

    if ( position < cmd_table[ cmd ].position )
    {
        switch ( position )
        {
            case POS_DEAD:
                if ( ch->sex == 0 )
                {
                    send_to_char( "Le¿ spokojnie; Jeste¶ MARTWE.\n\r", ch );
                }
                else if ( ch->sex == 1 )
                {
                    send_to_char( "Le¿ spokojnie; Jeste¶ MARTWY.\n\r", ch );
                }
                else
                {
                    send_to_char( "Le¿ spokojnie; Jeste¶ MARTWA.\n\r", ch );
                }
                break;

            case POS_MORTAL:
            case POS_INCAP:
                if ( ch->sex == 0 )
                {
                    send_to_char( "Jeste¶ zbyt ranne ¿eby to zrobiæ.\n\r", ch );
                }
                else if ( ch->sex == 1 )
                {
                    send_to_char( "Jeste¶ zbyt ranny ¿eby to zrobiæ.\n\r", ch );
                }
                else
                {
                    send_to_char( "Jeste¶ zbyt ranna ¿eby to zrobiæ.\n\r", ch );
                }
                break;

            case POS_STUNNED:
                if ( ch->sex == 0 )
                {
                    send_to_char( "Jeste¶ zbyt oszo³omione ¿eby to zrobiæ.\n\r", ch );
                }
                else if ( ch->sex == 1 )
                {
                    send_to_char( "Jeste¶ zbyt oszo³omiony ¿eby to zrobiæ.\n\r", ch );
                }
                else
                {
                    send_to_char( "Jeste¶ zbyt oszo³omiona ¿eby to zrobiæ.\n\r", ch );
                }
                break;

            case POS_SLEEPING:
                send_to_char( "W snach czy co?\n\r", ch );
                break;

            case POS_RESTING:
            case POS_SITTING:
                send_to_char( "Najpierw wstañ.\n\r", ch );
                break;

            case POS_FIGHTING:
                send_to_char( "Nie da rady! Ci±gle walczysz!\n\r", ch );
                break;

        }
        return ;
    }

    /*
     * Dispatch the command.
     */

    DEBUG_INFO( cmd_table[ cmd ].name );
    ( *cmd_table[ cmd ].do_fun ) ( ch, argument );
    DEBUG_INFO( NULL );

    tail_chain( );
    return ;
}

/* function to keep argument safe in all commands -- no static strings */
void do_function (CHAR_DATA *ch, DO_FUN *do_fun, char *argument)
{
    char *command_string;

    /* copy the string */
    command_string = str_dup(argument);

    /* dispatch the command */
    (*do_fun) (ch, command_string);

    /* free the string */
    free_string(command_string);
}

bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	SOCIAL_DATA *social;
	CHAR_DATA *victim;
	bool found;

	if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master && !IS_NPC( ch->master ) )
		return FALSE;

	found = FALSE;
	for ( social = social_list; social; social = social->next )
	{
		if ( NOPOL( command[ 0 ] ) == NOPOL( social->name[ 0 ] )
		     && !str_prefix( command, social->name ) )
		{
			found = TRUE;
			break;
		}
	}

	if ( !found )
		return FALSE;

	if ( !IS_NPC( ch ) && IS_SET( ch->comm, COMM_NOEMOTE ) )
	{
		send_to_char( "Nie mo¿esz okazywaæ emocji!\n\r", ch );
		return TRUE;
	}

	switch ( ch->position )
	{
		case POS_DEAD:
			send_to_char( "Le¿ spokojnie, nie ¿yjesz!!!\n\r", ch );
			return TRUE;

		case POS_INCAP:
		case POS_MORTAL:
			print_char( ch, "Jeste¶ na to zbyt rann%s.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "e" : "y" );
			return TRUE;

		case POS_STUNNED:
			print_char( ch, "Jeste¶ zbyt oszo³omion%s ¿eby to zrobiæ.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "e" : "y" );
			return TRUE;

		case POS_SLEEPING:
			/*
			 * I just know this is the path to a 12" 'if' statement.  :(
			 * But two players asked for it already!  -- Furey
			 */
			if ( !str_cmp( social->name, "chrap" ) )
				break;

			send_to_char( "W snach, czy co?\n\r", ch );
			return TRUE;

	}

	one_argument( argument, arg );
	victim = NULL;
	if ( arg[ 0 ] == '\0' )
	{
		act( social->others_no_arg, ch, NULL, victim, TO_ROOM );
		act( social->char_no_arg, ch, NULL, victim, TO_CHAR );
	}
	else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
	}
	else if ( victim == ch )
	{
		act( social->others_auto, ch, NULL, victim, TO_ROOM );
		act( social->char_auto, ch, NULL, victim, TO_CHAR );
	}
	else
	{
		if ( is_ignoring( victim, ch->name ) )
		{
			send_to_char( "Ta osoba ciê ignoruje.\n\r", ch );
			return TRUE;
		}

		act( social->others_found, ch, NULL, victim, TO_NOTVICT );
		act( social->char_found, ch, NULL, victim, TO_CHAR );
		act( social->vict_found, ch, NULL, victim, TO_VICT );
	}

	return TRUE;
}

/*
 * Return true if an argument is completely numeric.
 */
bool is_number ( char *arg )
{
    if ( arg == NULL )
        return FALSE;

    if ( *arg == '\0' )
        return FALSE;

    if ( *arg == '+' || *arg == '-' )
        arg++;

    for ( ; *arg != '\0'; arg++ )
	{
        if ( !isdigit( *arg ) )
            return FALSE;
	}

    return TRUE;
}

/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int number;

    if ( argument == NULL )
    {
        arg[0] = '\0';
        return 1;
    }

    for ( pdot = argument; *pdot != '\0'; pdot++ )
	{
	if ( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}
	}

    strcpy( arg, argument );
    return 1;
}

/*
 * Given a string like 14*foo, return 14 and 'foo'
*/
int mult_argument(char *argument, char *arg)
{
    char *pdot;
    int number;

    if ( argument == NULL )
    {
        arg[0] = '\0';
        return 1;
    }

    for ( pdot = argument; *pdot != '\0' ; pdot++ )
	{
        if ( *pdot == '*' )
    	{
            *pdot = '\0';
            number = atoi( argument );
            *pdot = '*';
            strcpy( arg, pdot+1 );
            return number;
    	}
	}

    strcpy( arg, argument );
    return 1;
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    if ( argument == NULL )
    {
        arg_first[0] = '\0';
        return str_dup( "" );
    }

    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
	{
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = LOWER(*argument);
	arg_first++;
	argument++;
	}
    *arg_first = '\0';

    while ( isspace(*argument) )
	argument++;

    return argument;
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *case_one_argument( char *argument, char *arg_first )
{
    char cEnd;

    if ( argument == NULL )
    {
        arg_first[0] = '\0';
        return str_dup( "" );
    }

    while ( isspace(*argument) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
	{
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = *argument;
	arg_first++;
	argument++;
	}
    *arg_first = '\0';

    while ( isspace(*argument) )
	argument++;

    return argument;
}

/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col=0;
    char* min = "zz";
    char* actual = "aa";

	//sortowanie
    while( str_cmp(min, "zzz") )
	{
        min = "zzz";
        for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    	{
            if(EXT_IS_SET(ch->act, PLR_COMMANDSPL))
        	{
                if ( cmd_table[cmd].level == 0 && !IS_SET( cmd_table[cmd].options, OPT_HIDEHELP ) )
            	{
                    if ( strcmp(cmd_table[cmd].name_pl, min) <= 0
                    && strcmp(cmd_table[cmd].name_pl, actual) > 0 )
                        min = cmd_table[cmd].name_pl;
            	}
        	}
            else
        	{
                if ( cmd_table[cmd].level == 0 && !IS_SET( cmd_table[cmd].options, OPT_HIDEHELP ) )
            	{
                    if ( strcmp(cmd_table[cmd].name, min) <= 0
                    && strcmp(cmd_table[cmd].name, actual) > 0 )
                        min = cmd_table[cmd].name;
            	}
        	}
    	}

        actual = min;

        if ( strcmp( actual, "zzz" ) )
    	{
            sprintf(buf, "%-12s", actual );
            send_to_char( buf, ch );
            if ( ++col % 6 == 0 )
            send_to_char( "\n\r", ch );
    	}
	}

    if ( ++col % 6 != 0 )
    send_to_char( "\n\r", ch );
    return;
}

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	int cmd;
	int col = 0;
	char* min = "zz";
	char* actual = "aa";

	if ( IS_NPC(ch) )
	{
		return;
	}

	//sortowanie
	while( str_cmp(min, "zzz") )
	{
		min = "zzz";
		for(cmd=0;cmd_table[cmd].name[0]!='\0';cmd++)
		{
			if( cmd_table[cmd].level > 2
			&&  IS_SET(ch->pcdata->wiz_conf,cmd_table[cmd].level)
			&&  !IS_SET( cmd_table[cmd].options, OPT_HIDEHELP ) )
			{
				if ( strcmp(cmd_table[cmd].name, min) <= 0
				&& strcmp(cmd_table[cmd].name, actual) > 0 )
					min = cmd_table[cmd].name;
			}
		}
		actual = min;

		if ( strcmp( actual, "zzz" ) )
		{
			sprintf(buf, "%-12s", actual );
			send_to_char( buf, ch );
			if ( ++col % 6 == 0 )
				send_to_char( "\n\r", ch );
		}
	}

	if ( col % 6 != 0 )
	{
		send_to_char( "\n\r", ch );
	}

	return;
}
