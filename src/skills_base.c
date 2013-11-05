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
 * $Id: skills_base.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/skills_base.c $
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
#include "magic.h"

/**
 * set_base_skills
 *
 * set_base_skills - setting base skils
 *
 * function set a base skill
 *
 * dla czarów pola: min i max nie maj¿ ¿adnego znaczenia
 *
 * struct base_skills
 * {
 * 	int ch_class;
 * 	int learn_chance;
 * 	char *skill_name;
 * 	int learn_min;
 * 	int learn_max;
 * };
 *
 */
static BASE_SKILLS base_skills_table[] =
{
    /**
     * barbarian
     */
    { CLASS_BARBARIAN, 30, "axe",              1, 20 },
    { CLASS_BARBARIAN, 30, "bandage",          9, 30 },
    { CLASS_BARBARIAN, 30, "dagger",           1, 30 },
    { CLASS_BARBARIAN, 30, "flail",            1, 10 },
    { CLASS_BARBARIAN, 30, "mace",             1, 10 },
    { CLASS_BARBARIAN, 30, "polearm",          1, 10 },
    { CLASS_BARBARIAN, 60, "riding",           9, 30 },
    { CLASS_BARBARIAN, 50, "short-sword",      1, 40 },
    { CLASS_BARBARIAN, 30, "spear",            1, 10 },
    { CLASS_BARBARIAN, 50, "staff",            1, 40 },
    { CLASS_BARBARIAN, 30, "sword",            1, 10 },
    { CLASS_BARBARIAN, 70, "trapper",          9, 30 },
    { CLASS_BARBARIAN, 30, "twohanded weapon", 1, 10 },
    { CLASS_BARBARIAN, 90, "claw-weapons",     1, 10 },

    /**
     * warrior
     */
    { CLASS_WARRIOR, 30, "axe",              1, 10 },
    { CLASS_WARRIOR, 30, "bandage",          9, 30 },
    { CLASS_WARRIOR, 30, "bash",             1, 20 },
    { CLASS_WARRIOR, 30, "dagger",           1, 30 },
    { CLASS_WARRIOR, 30, "flail",            1, 10 },
    { CLASS_WARRIOR, 30, "kick",             1, 20 },
    { CLASS_WARRIOR, 30, "mace",             1, 10 },
    { CLASS_WARRIOR, 30, "polearm",          1, 10 },
    { CLASS_WARRIOR, 50, "rescue",           9, 30 },
    { CLASS_WARRIOR, 60, "riding",           9, 30 },
    { CLASS_WARRIOR, 30, "short-sword",      1, 10 },
    { CLASS_WARRIOR, 30, "spear",            1, 10 },
    { CLASS_WARRIOR, 40, "staff",            1, 20 },
    { CLASS_WARRIOR, 30, "sword",            1, 10 },
    { CLASS_WARRIOR, 70, "trapper",          9, 30 },
    { CLASS_WARRIOR, 30, "twohanded weapon", 1, 10 },
    { CLASS_WARRIOR, 30, "whip",             1, 10 },

    /**
     * cleric :: skills
     */
    { CLASS_CLERIC, 20, "flail",                 1, 10 },
    { CLASS_CLERIC, 20, "mace",                  1, 10 },
    { CLASS_CLERIC, 30, "staff",                 1, 20 },
    { CLASS_CLERIC, 10, "riding",                5, 20 },
    { CLASS_CLERIC, 20, "meditation",            1, 20 },
    { CLASS_CLERIC, 90, "first aid",             1, 30 },
    { CLASS_CLERIC, 40, "twohanded weapon",      1, 19 },
    { CLASS_CLERIC, 20, "short-sword",           1, 10 },
    { CLASS_CLERIC, 80, "bandage",               9, 20 },
    { CLASS_CLERIC, 80, "turn undead",           5, 15 },
    { CLASS_CLERIC, 20, "trapper",               1, 20 },

    /**
     * cleric :: spells
     */
    { CLASS_CLERIC, 50, "bless",                 0, 0 },
    { CLASS_CLERIC, 20, "purify food",           0, 0 },
    { CLASS_CLERIC, 20, "cause light",           0, 0 },
    { CLASS_CLERIC, 40, "create food",           0, 0 },
    { CLASS_CLERIC, 10, "transmute staff",       0, 0 },
    { CLASS_CLERIC, 10, "create water",          0, 0 },
    { CLASS_CLERIC, 50, "cure light",            0, 0 },
    { CLASS_CLERIC,  5, "command",               0, 0 },
    { CLASS_CLERIC,  5, "comprehend languages",  0, 0 },
    { CLASS_CLERIC, 10, "bane",                  0, 0 },
    { CLASS_CLERIC, 50, "luck",                  0, 0 },
    { CLASS_CLERIC,  5, "create symbol",         0, 0 },
    { CLASS_CLERIC,  5, "change liquid",         0, 0 },
    { CLASS_CLERIC,  5, "spiritual light",       0, 0 },
    { CLASS_CLERIC,  5, "dismiss undead",        0, 0 },
    { CLASS_CLERIC,  5, "detect undead",         0, 0 },
    { CLASS_CLERIC,  5, "lore undead",           0, 0 },

    /**
     * palladin :: skils
     */
    { CLASS_PALADIN,  30, "axe",              1, 10 },
    { CLASS_PALADIN,  50, "bandage",          9, 30 },
    { CLASS_PALADIN,  30, "dagger",           1, 10 },
    { CLASS_PALADIN,  30, "flail",            1, 10 },
    { CLASS_PALADIN, 101, "holy prayer",      9, 11 }, // wymuszony skill na wysokim startowym poziomie
    { CLASS_PALADIN,  40, "lay",              1, 30 },
    { CLASS_PALADIN,  30, "mace",             1, 10 },
    { CLASS_PALADIN,  30, "polearm",          1, 10 },
    { CLASS_PALADIN,  50, "riding",           9, 40 },
    { CLASS_PALADIN,  30, "short-sword",      1, 10 },
    { CLASS_PALADIN,  30, "spear",            1, 10 },
    { CLASS_PALADIN,  30, "sword",            1, 10 },
    { CLASS_PALADIN,  70, "trapper",          9, 30 },
    { CLASS_PALADIN,  20, "twohanded weapon", 1, 30 },
    { CLASS_PALADIN,  30, "whip",             1, 10 },

    /**
     * palladin :: spells
     */
    { CLASS_PALADIN,  5, "bane",            0, 0 },
    { CLASS_PALADIN, 20, "bless",           0, 0 },
    { CLASS_PALADIN,  5, "cause light",     0, 0 },
    { CLASS_PALADIN,  5, "create food",     0, 0 },
    { CLASS_PALADIN,  5, "create water",    0, 0 },
    { CLASS_PALADIN,  5, "cure disease",    0, 0 },
    { CLASS_PALADIN,  5, "cure light",      0, 0 },
    { CLASS_PALADIN, 15, "detect evil",     0, 0 },
    { CLASS_PALADIN, 15, "detect undead",   0, 0 },
    { CLASS_PALADIN,  5, "know alignment",  0, 0 },
    { CLASS_PALADIN,  5, "light",           0, 0 },
    { CLASS_PALADIN,  5, "lore undead",     0, 0 },
    { CLASS_PALADIN, 10, "protection evil", 0, 0 },
    { CLASS_PALADIN,  5, "purify food",     0, 0 },

    /**
     * thief
     */
    { CLASS_THIEF, 80, "backstab",     5, 20 },
    { CLASS_THIEF,  5, "bandage",      1, 10 },
    { CLASS_THIEF, 30, "dagger",       5, 20 },
    { CLASS_THIEF, 10, "detect traps", 1, 20 },
    { CLASS_THIEF, 10, "disarm traps", 1, 20 },
    { CLASS_THIEF, 30, "flail",        1, 20 },
    { CLASS_THIEF, 40, "hide",         1, 20 },
    { CLASS_THIEF, 30, "mace",         1, 20 },
    { CLASS_THIEF, 80, "peek",         1, 20 },
    { CLASS_THIEF, 80, "pick lock",    1, 20 },
    { CLASS_THIEF,  5, "riding",       1, 10 },
    { CLASS_THIEF, 30, "short-sword",  1, 20 },
    { CLASS_THIEF, 80, "sneak",        1, 30 },
    { CLASS_THIEF, 80, "steal",        1, 20 },
    { CLASS_THIEF, 30, "sword",        1, 20 },
    { CLASS_THIEF,  5, "trapper",      1,  5 },
    { CLASS_THIEF, 30, "whip",         1, 20 },

    /**
     * black knight
     */
    { CLASS_BLACK_KNIGHT, 30, "axe",                1, 10 },
    { CLASS_BLACK_KNIGHT, 20, "bandage",            1, 10 },
    { CLASS_BLACK_KNIGHT, 30, "dagger",             1, 10 },
    { CLASS_BLACK_KNIGHT, 60, "flail",              1, 30 },
    { CLASS_BLACK_KNIGHT, 10, "kick",               1, 20 },
    { CLASS_BLACK_KNIGHT, 30, "mace",               1, 10 },
    { CLASS_BLACK_KNIGHT, 30, "polearm",            1, 10 },
    { CLASS_BLACK_KNIGHT, 20, "riding",             1, 30 },
    { CLASS_BLACK_KNIGHT, 30, "short-sword",        1, 15 },
    { CLASS_BLACK_KNIGHT, 30, "spear",              1, 10 },
    { CLASS_BLACK_KNIGHT, 30, "sword",              1, 10 },
    { CLASS_BLACK_KNIGHT, 30, "torment",            1, 30 },
    { CLASS_BLACK_KNIGHT, 70, "trapper",            9, 30 },
    { CLASS_BLACK_KNIGHT, 30, "twohanded weapon",   1, 20 },
    { CLASS_BLACK_KNIGHT, 50, "undead resemblance", 9, 30 },
    { CLASS_BLACK_KNIGHT, 30, "whip",               1, 20 },

    /**
     * druid :: spells
     */
    { CLASS_DRUID, 10, "burst of flame",   0, 0 },
    { CLASS_DRUID, 30, "cure animal",      0, 0 },
    { CLASS_DRUID, 30, "cure plant",       0, 0 },
    { CLASS_DRUID, 20, "dismiss animal",   0, 0 },
    { CLASS_DRUID,  5, "dismiss insect",   0, 0 },
    { CLASS_DRUID,  5, "dismiss plant",    0, 0 },
    { CLASS_DRUID, 10, "endure acid",      0, 0 },
    { CLASS_DRUID,  5, "endure cold",      0, 0 },
    { CLASS_DRUID, 20, "endure fire",      0, 0 },
    { CLASS_DRUID,  5, "endure lightning", 0, 0 },
    { CLASS_DRUID,  5, "faerie fire",      0, 0 },
    { CLASS_DRUID,  5, "firefly swarm",    0, 0 },
    { CLASS_DRUID,  5, "frost rift",       0, 0 },
    { CLASS_DRUID,  5, "nature ally I",    0, 0 },
    { CLASS_DRUID, 10, "purify food",      0, 0 },
    { CLASS_DRUID, 10, "shillelagh",       0, 0 },
    { CLASS_DRUID, 40, "spray of thorns",  0, 0 },

    /**
     * druid :: skills
     */
    { CLASS_DRUID, 70, "bandage",          9, 20 },
    { CLASS_DRUID, 20, "dagger",           1, 10 },
    { CLASS_DRUID, 20, "flail",            1, 10 },
    { CLASS_DRUID, 20, "mace",             1, 30 },
    { CLASS_DRUID, 20, "riding",           1, 10 },
    { CLASS_DRUID, 20, "short-sword",      1, 10 },
    { CLASS_DRUID, 20, "staff",            1, 40 },
    { CLASS_DRUID, 60, "trapper",          9, 30 },
    { CLASS_DRUID, 10, "twohanded weapon", 1, 10 },
    { CLASS_DRUID, 70, "claw weapons",     1, 10 },

    /**
     * mage :: skills
     */
    { CLASS_MAG,  5, "dagger",             1, 10 },
    { CLASS_MAG, 60, "staff",              1, 20 },
    { CLASS_MAG, 10, "riding",             1,  5 },
    { CLASS_MAG, 20, "twohanded weapon",   1, 15 },
    { CLASS_MAG, 30, "bandage",            1,  5 },
    { CLASS_MAG,  5, "trapper",            1,  5 },
//  { CLASS_MAG,  0, "undead resemblance", 0,  0 }, // Gurthg: wylaczam z losowania, skill specjalisyczny

    /**
     * mage :: spells
     */
    { CLASS_MAG, 80, "acid hands",           0, 0 },
    { CLASS_MAG, 80, "armor",                0, 0 },
    { CLASS_MAG, 10, "blur",                 0, 0 },
    { CLASS_MAG, 10, "bonelace",             0, 0 },
    { CLASS_MAG, 70, "comprehend languages", 0, 0 },
    { CLASS_MAG, 10, "confuse languages",    0, 0 },
    { CLASS_MAG, 50, "detect magic",         0, 0 },
    { CLASS_MAG,  5, "dismiss animal",       0, 0 },
    { CLASS_MAG,  5, "dismiss insect",       0, 0 },
    { CLASS_MAG,  5, "dismiss plant",        0, 0 },
    { CLASS_MAG, 20, "fire darts",           0, 0 },
    { CLASS_MAG, 70, "infravision",          0, 0 },
    { CLASS_MAG, 90, "light",                0, 0 },
    { CLASS_MAG, 10, "lore undead",          0, 0 },
    { CLASS_MAG, 70, "magic missile",        0, 0 },
    { CLASS_MAG,  5, "mind strike",          0, 0 },
    { CLASS_MAG,  5, "misfortune",           0, 0 },
    { CLASS_MAG, 70, "shield",               0, 0 },
    { CLASS_MAG,  5, "summon insect",        0, 0 },

        /**
     * shaman
     */
    { CLASS_SHAMAN, 30, "axe",              1, 20 },
    { CLASS_SHAMAN, 30, "bandage",          9, 30 },
    { CLASS_SHAMAN, 30, "dagger",           1, 30 },
    { CLASS_SHAMAN, 30, "flail",            1, 10 },
    { CLASS_SHAMAN, 30, "mace",             1, 10 },
    { CLASS_SHAMAN, 30, "polearm",          1, 10 },
    { CLASS_SHAMAN, 60, "riding",           9, 30 },
    { CLASS_SHAMAN, 30, "short-sword",      1, 40 },
    { CLASS_SHAMAN, 30, "spear",            1, 10 },
    { CLASS_SHAMAN, 30, "staff",            1, 40 },
    { CLASS_SHAMAN, 30, "sword",            1, 10 },
    { CLASS_SHAMAN, 70, "trapper",          9, 30 },
    { CLASS_SHAMAN, 30, "twohanded weapon", 1, 10 },
    { CLASS_SHAMAN, 30, "claw weapons",     1, 10 },

	{ 0, 0, NULL, 0, 0 } //zostawic jako ostatnia pozycje tabeli
};
/*
 * TODO
 *
 * #define CLASS_MONK					7
 * #define CLASS_BARD					8
 *
 */
void set_base_skills ( CHAR_DATA * ch )
{
    int sn, i; //licznik petli
    for ( i = 0; base_skills_table[i].skill_name; ++i ) //przelatuje przez wszystkie pozycje base_skills_table
    {
        if ( base_skills_table[i].ch_class == ch->class ) //jesli skill dla naszej profki
        {
            if ( number_percent() < base_skills_table[i].learn_chance ) //jesli mielismy szczescie
            {
                sn = skill_lookup ( base_skills_table[i].skill_name );
                /**
                 * to jest czar czy skill?
                 */
                if ( skill_table[ sn ].spell_fun == spell_null )
                {
                    ch->pcdata->learned[ sn ] = number_range( base_skills_table[i].learn_min, base_skills_table[i].learn_max );
                }
                else
                {
                    ch->pcdata->learned[ sn ] = 2; // 2 - zeby bylo widac, ze to "nauczone" z rolowania
                }
            }
        }
    }
}
