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
 * Koper Tadeusz         (garloop@killer-mud.net        ) [Garloop   ] *
 * Pietrzak Marcin       (marcin.pietrzak@mud.pl        ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: const_races.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/const_races.c $
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
#include "magic.h"
#include "interp.h"
#include "lang.h"
#include "music.h"

const struct race_type race_table [] =
{
    /*
       {
       name, pc_race?,
       act bits, aff_by bits, off bits,
       form, parts
       minlevel, maxlevel
       },
     */
    {
        "unique", "unique", "unique", "unique", "unique", "unique",  // name2", "name3", "name4", "name5", "name6
        FALSE,                                                       // pc_race
        "",                                                          // act bits
        100,                                                         // skin multiplier
        "",                                                          // affected_by bits
        "",                                                          // off
        0,                                                           // form
        0,                                                           // parts
        0,                                                           // ANIMAL|PERSON|MONSTER|OUTSIDER
        {"","",""},                                                  // move in
        {"","",""},                                                  // move out
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0},                      // resists
        0,                                                           // wear
        0,                                                           // min level
        40,                                                          // max level
        "mówi",                                                      // text to be displayed instead of "mówi"
        "pyta"                                                       // text to be displayed instead of "pyta"
        "Mówisz",                                                    // text to be displayed instead of "Mówisz", first letter should be capital
    },
    /*
       RESIST_ALL           0
       RESIST_PIERCE        1
       RESIST_SLASH         2
       RESIST_BASH          3
       RESIST_MAGIC         4
       RESIST_FIRE          5
       RESIST_MAGIC_FIRE    6
       RESIST_ELECTRICITY   7
       RESIST_COLD          8
       RESIST_ACID          9
       RESIST_POISON       10
       RESIST_MAGIC_WEAPON 11
       RESIST_CHARM        12
       RESIST_FEAR         13
       RESIST_SUMMON       14
       RESIST_NEGATIVE     15
       RESIST_MENTAL       16
       RESIST_HOLY         17
       RESIST_SOUND        18
     */

    {
        "cz³owiek", "cz³owieka", "cz³owiekowi", "cz³owieka", "cz³owiekiem", "cz³owieku",
            TRUE,  "", 100,
            "", "", A|H|M|V|F,
            A|B|C|D|E|F|G|H|I|J|K|R,PERSON|MONSTER,
            {"przysz³o","przyszed³","przysz³a"},
            {"odchodzi","odchodzi","odchodzi"},
            {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|ITEM_WEAR_EAR|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT|ITEM_INSTRUMENT,
            0, 40,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "elf", "elfa", "elfowi", "elfa", "elfem", "elfie",
        TRUE, "", 100,
        "infrared", "",  A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R,PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|ITEM_WEAR_EAR|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT|ITEM_INSTRUMENT,
        0, 40,
        "nuci",
        "nuci",
        "Nucisz"
    },

    {
        "krasnolud", "krasnoluda", "krasnoludowi", "krasnoluda", "krasnoludem", "krasnoludzie",
        TRUE, "", 100,
        "infrared", "", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R, PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|ITEM_WEAR_EAR|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT|ITEM_INSTRUMENT,
        0, 40,
        "dudni",
        "dudni",
        "Dudnisz"
    },

    {
        "gnom", "gnoma", "gnomowi", "gnoma", "gnomem", "gnomie",
        TRUE, "", 100,
        "infrared", "", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R, PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|ITEM_WEAR_EAR|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT|ITEM_INSTRUMENT,
        0, 40,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "pó³elf", "pó³elfa", "pó³elfowi", "pó³elfa", "pó³elfem", "pó³elfie",
        TRUE, "", 100,
        "infrared", "", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R, PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|ITEM_WEAR_EAR|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT|ITEM_INSTRUMENT,
        0, 40,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "nizio³ek", "nizio³ka", "nizio³owi", "nizio³ka", "nizio³kiem", "nizio³ku",
        TRUE, "", 100,
        "infrared", "", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R, PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|ITEM_WEAR_EAR|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT|ITEM_INSTRUMENT,
        0, 40,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "pó³ork", "pó³orka", "pó³orkowi", "pó³orka", "pó³orkiem", "pó³orku",
        TRUE,  "", 100,
        "infrared", "", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R, PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|ITEM_WEAR_EAR|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT|ITEM_INSTRUMENT,
        0, 40,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "gigant", "giganta", "gigantowi", "giganta", "gigantem", "gigancie",
        FALSE, "", 100,
        "",  "kick", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R, MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|ITEM_WEAR_EAR|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        8, 40,
        "grzmi",
        "pyta",
        "Grzmisz"
    },

    {
        "nietoperz", "nietoperza", "nietoperzowi", "nietoperza", "nietoperzem", "nietoperzu",
        FALSE, "", 20,
        "detect_hidden flying dark_vision", "dodge", A|G|V|F,
        A|C|D|E|F|H|J|K|P|R|S, ANIMAL|MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        0, 2,
        "piszczy",
        "piszczy",
        "Piszczysz"
    },

    {
        "nied¼wied¼", "nied¼wiedzia", "nied¼wiedziowi", "nied¼wiedzia", "nied¼wiedziem", "nied¼wiedziu",
        FALSE, "", 100,
        "dark_vision", "berserk", A|G|V|F,
        A|B|C|D|E|F|H|J|K|U|V|R|S, ANIMAL|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        6, 12,
        "warczy",
        "warczy",
        "Warczysz"
    },

    {
        "kot", "kota", "kotu", "kota", "kotem", "kocie",
        FALSE, "", 70,
        "detect_hidden dark_vision sneak", "dodge", A|G|V|F,
        A|C|D|E|F|H|J|K|Q|U|V|R|S, ANIMAL|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        0, 2,
        "miauczy",
        "miauczy",
        "Miauczysz"
    },

    {
        "stonoga", "stonogi", "stonodze", "stonogê", "stonog±", "stonodze",
        FALSE, "", 100,
        "dark_vision",  "",  A|B|G|O,
        A|C|K, MONSTER|INSECT,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 40,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "pies", "psa", "psu", "psa", "psem", "psie",
        FALSE, "", 70,
        "detect_hidden dark_vision",  "", A|G|V|F,
        A|C|D|E|F|H|J|K|U|V|R|S, ANIMAL|MONSTER,
        {"przybiegl","przybiegl","przybiegla"},
        {"biegnie","biegnie","biegnie"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        0, 3,
        "szczeka",
        "szczeka",
        "Szczekasz"
    },

    {
        "ghast", "ghasta", "ghastowi", "ghasta", "ghastem", "ghascie",
        FALSE, "undead", 100,
        "", "assist_race",
        I|M|F,  A|B|C|F|G|H|I|J|K|R,  UNDEAD|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        6, 9,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "smok", "smoka", "smokowi", "smoka", "smokiem", "smoku",
        FALSE, "mage warrior", 300,
        "infrared flying detect_invis", "assist_race", A|H|Z,
        A|C|D|E|F|G|H|I|J|K|P|Q|U|V|X|R|S, MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_NECK,
        11, 32,
        "ryczy",
        "pyta",
        "Mówisz"
    },

    {
        "fido", "fida", "fidu", "fida", "fidem", "fidzie",
        FALSE, "", 100,
        "",  "dodge assist_race", A|B|G|V|F,
        A|C|D|E|F|H|J|K|Q|S|V, MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_NECK,
        0, 40,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "lis", "lisa", "lisowi", "lisa", "lisem", "lisie",
        FALSE, "", 100,
        "dark_vision", "", A|G|V|F,
        A|C|D|E|F|H|J|K|Q|V|R|S, ANIMAL|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        0, 2,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "zwierze", "zwierzêcia", "zwierzêciu", "zwierzê", "zwierzêciem", "zwierzêciu",
        FALSE, "", 100,
        "dark_vision detect_hidden", "", A|G|V|F,
        A|C|D|E|F|H|J|K|Q|V|R|S, ANIMAL|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        0, 40,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "goblin", "goblina", "goblinowi", "goblina", "goblinem", "goblinie",
        FALSE, "", 100,
        "infrared", "assist_race", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R, PERSON|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|ITEM_WEAR_EAR|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        2, 4,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "hobgoblin", "hobgoblina", "hobgoblinowi", "hobgoblina", "hobgoblinem", "hobgoblinie",
        FALSE, "", 100,
        "infrared", "assist_race", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|Y|R, PERSON|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_EAR|ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        3, 6,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "kobold", "kobolda", "koboldowi", "kobolda", "koboldem", "koboldzie",
        FALSE, "", 100,
        "infrared", "assist_race", A|B|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R, PERSON|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_EAR|ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        2, 4,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "gad", "gada", "gadowi", "gada", "gadem", "gadzie",
        FALSE, "", 20,
        "detect_hidden",  "", A|G|X|cc,
        A|C|D|E|F|H|K|Q|V|R|S, ANIMAL|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_NECK,
        0, 40,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "modron", "modrona", "modronowi", "modrona", "modronem", "modronie",
        FALSE, "", 100,
        "infrared", "assist_race assist_align", H,
        A|B|C|G|H|J|K|R, UNKNOWN,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 40,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "ork", "orka", "orkowi", "orka", "orkiem", "orku",
        FALSE, "", 100,
        "infrared", "assist_race", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R, PERSON|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_EAR|ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        2, 10,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "¶winia", "¶wini", "¶wini", "¶winiê", "¶wini±", "¶wini",
        FALSE, "", 70,
        "",  "", A|G|V|F,
        A|C|D|E|F|H|J|K|R|S, ANIMAL|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        0, 3,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "zaj±c", "zaj±ca", "zaj±cu", "zaj±ca", "zaj±cem", "zaj±cu",
        FALSE, "", 70,
        "detect_hidden",  "", A|G|V|F,
        A|C|D|E|F|H|J|K|R|S, ANIMAL|MONSTER,
        {"przykica³o","przykica³","przykica³a"},
        {"odkica³o","odkica³","odkica³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        0, 2,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "ettin", "ettina", "ettinowi", "ettina", "ettinem", "ettinie",
        FALSE, "", 100,
        "", "assist_race kick bash",
        A|E|M|V|F, A|B|C|D|E|F|G|H|I|J|K|U|V|R, MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            10, 20,
            "grzmi",
            "pyta",
            "Grzmisz"
    },

    {
        "w±¿", "wê¿a", "wê¿owi", "wê¿a", "wê¿em", "wê¿u",
        FALSE, "", 120,
        "detect_hidden",  "", A|G|X|Y|cc,
        A|D|E|F|K|L|Q|V|X|R|S, ANIMAL|MONSTER,
        {"przypelz³o","przype³z³","przype³z³a"},
        {"odpe³z³o","odpe³z³","odpe³z³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 15,
        "syczy",
        "syczy",
        "Syczysz"
    },

    {
        "ptak", "ptaka", "ptaka", "ptaka", "ptakiem", "ptaku",
        FALSE, "", 20,
        "flying detect_hidden", "", A|G|W|F,
        A|C|D|E|F|H|K|P|R|S|T, ANIMAL|MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 12,
        "¶piewa",
        "¶piewa",
        "¦piewasz"
    },

    {
        "ptak ¶piewaj±cy", "ptaka ¶piewaj±cego", "ptaka ¶piewaj±cego", "ptaka ¶piewaj±cego", "ptakiem ¶piewaj±cym", "ptaku ¶piewaj±cym",
        FALSE, "", 20,
        "flying", "", A|G|W|F,
        A|C|D|E|F|H|K|P|R|S|T, ANIMAL,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 5,
        "¶piewa",
        "¶piewa",
        "¦piewasz"
    },

    {
        "troll", "trolla", "trollowi", "trolla", "trollem", "trollu",
        FALSE, "", 100,
        "troll_power regeneration infrared", "assist_race", A|B|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|U|V|R, MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_EAR|ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        6, 19,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "duergar", "duergara", "duergarowi", "duergara", "duergarem", "duergarze",
        TRUE, "", 100,
        "infrared", "", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R, PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,10, 0,0,0,0,0, 0,0,0,0,0, 0,0,}, //resist magic 10%
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|ITEM_WEAR_EAR|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT|ITEM_INSTRUMENT,
        0, 40,
        "dudni",
        "dudni",
        "Dudnisz"
    },

    {
        "wilk", "wilka", "wilkowi", "wilka", "wilkiem", "wilku",
        FALSE, "", 100,
        "dark_vision detect_hidden", "assist_race", A|G|V|F,
        A|C|D|E|F|J|K|Q|V|R|S, ANIMAL|MONSTER,
        {"przybieg³o","przybieg³","przybieg³a"},
        {"odbieg³o","odbieg³","odbieg³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        5, 10,
        "warczy",
        "warczy",
        "Warczysz"
    },

    {
        "wywerna", "wywerny", "wywernie", "wywernê", "wywern±", "wywernie",
        FALSE, "", 250,
        "flying detect_invis detect_hidden", "", A|B|G|Z|F,
        A|C|D|E|F|H|J|K|Q|V|X|R|S, MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_NECK,
        10, 12
    },


    {
        "d¿in", "d¿ina", "d¿inowi", "d¿ina", "d¿inem", "d¿inie",
        FALSE, "", 100,
        "flying", "", C|F,
        A|B|C|D|G|H|I|J|K, MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        13, 33,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "duch", "ducha", "duchowi", "ducha", "duchem", "duchu",
        FALSE, "undead", 100,
        "pass_door flying",  "", I|L,
        A|B|C|G|H|I, UNDEAD|OUTSIDER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_FLOAT|ITEM_WIELD|ITEM_WIELDSECOND,
        0, 18,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "zombi", "zombi", "zombi", "zombi", "zombi", "zombi",
        FALSE, "undead", 100,
        "",  "", A|B|I|cc,
        A|B|C|G|H|I|R, UNDEAD,
        {"","",""},
        {"","",""},
        {0,10,10,10,0, 0,0,0,0,0, 0,0,0,0,0, 0,0},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_EAR|ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        4, 10,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "ankheg", "ankhega", "ankhegowi", "ankhega", "ankhegiem", "ankhegu",
        FALSE, "", 100,
        "", "assist_race", O|R,
        A|B|D|E|F|J|K|Q|U|X, ANIMAL|MONSTER|INSECT,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        6, 10
    },

    {
        "svirneblin", "svirneblina", "svirneblinowi", "svirneblina", "svirneblinem", "svirneblinie",
        TRUE, "", 100,
        "infrared", "", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R, PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|ITEM_WEAR_EAR|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT|ITEM_INSTRUMENT,
        0, 40,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "baatezu", "baatezu", "baatezu", "baatezu", "baatezu", "baatezu",
        FALSE, "mage warrior", 100,
        "free_action regeneration infrared detect_magic", "", C|D|E|M,
        A|B|C|D|E|F|H|J|K|P|Q|U|V|W|X, MONSTER|DEMON|OUTSIDER,
        {"przybylo","przybyl","przybyla"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,100,0,0, 0,0}, //immune charm,sleep
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        26, 37,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "banshee", "banshee", "banshee", "banshee", "banshee", "banshee",
        FALSE, "undead", 100,
        "pass_door detect_invis detect_hidden infrared", "", C|I|K|M,
        A|B|C|G|H|I|J|K, UNDEAD|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,100,0, 0,1,100,0,0, 0,0,}, //immune charm,cold,sleep,non_magic_weapon
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        13, 15,
        "zawodzi",
        "pyta",
        "Zawodzisz"
    },

    {
        "bazyliszek", "bazyliszka", "bazyliszkowi", "bazyliszka", "bazyliszkiem", "bazyliszku",
        FALSE, "", 200,
        "infrared", "assist_race", G|X,
        A|C|D|E|F|H|J|K|Q|U|R|S, MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,100,0,0, 0,0,},
        ITEM_WEAR_NECK,
        10, 19,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "wypatrywacz", "wypatrywacza", "wypatrywaczowi", "wypatrywacza", "wypatrywaczem", "wypatrywaczu",
        FALSE, "mage", 200,
        "regeneration detect_hidden detect_invis", "", C|E,
        A|F|J|K|M|N|S,  MONSTER,
        {"przybylo","przybyl","przybyla"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,20, 0,0,0,0,0,0 ,2,0,0,0, 0,50,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        7, 30,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "centaur", "centaura", "centaurowi", "centaura", "centaurem", "centaurze",
        FALSE, "", 100,
        "", "kick assist_race", A|H|N|V|F,
        A|B|C|D|E|F|G|I|J|K|Q|R, ANIMAL|PERSON,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|
            ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_WAIST|
            ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        6, 11,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "chimera", "chimery", "chimerze", "chimerê", "chimer±", "chimerze",
        FALSE, "", 170,
        "infrared detect_hidden detect_magic", "",
        G|R|W,  A|B|C|D|E|F|J|K|P|Q|U|V|W|X|R|S, ANIMAL|MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        15, 17,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "kuroliszek", "kuroliszka", "kuroliszkowi", "kuroliszka", "kuroliszkiem", "kuroliszku",
        FALSE, "", 100,
        "", "",
        A|G|W|F, A|D|E|F|H|J|K|P|Q|U|R|S,  ANIMAL|MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 100,75,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        9, 11,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "krokodyl", "krokodyla", "krokodylowi", "krokodyla", "krokodylem", "krokodylu",
        FALSE, "", 150,
        "", "assist_race",
        A|G|X,  A|B|C|D|F|H|J|K|Q|U|R|S,  ANIMAL,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        4, 7,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "krokodyl olbrzymi", "krokodyla olbrzymiego", "krokodylowi olbrzymiemu", "krokodyla olbrzymiego", "krokodylem olbrzymim", "krokodylu olbrzymim",
        FALSE, "", 180,
        "", "assist_race",
        A|G|X,  A|B|C|D|F|H|J|K|Q|U|R|S,  ANIMAL,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        10, 12,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "skorupiak olbrzymi", "skorupiaka olbrzymiego", "skorupiakowi olbrzymiemu", "skorupiaka olbrzymiego", "skorupiakiem olbrzymim", "skorupiaku olbrzymim",
        FALSE, "", 100,
        "", "",
        M|Q|V,  A|B|C|D|G|H|I|J|M,  ANIMAL|MONSTER|INSECT,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        5, 7,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "kryptowiec", "kryptowca", "kryptowcowi", "kryptowca", "kryptowcem", "kryptowcu",
        FALSE, "undead warrior", 100,
        "infrared", "",
        E|M|X,  A|B|C|G|H|I|J|K|Q|U,  UNDEAD|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,100,0, 0,0,100,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_EAR|ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        10, 11,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "upiorny rycerz", "upiornego rycerza", "upiornemu rycerzowi", "upiornego rycerza", "upiornym rycerzem", "upiornym rycerzu",
        FALSE, "undead black_knight", 100,
        "", "",
        E|M|V, A|B|C|D|E|F|G|H|I|J|K,  UNDEAD,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,75, 0,0,0,0,0, 0,0,0,0,0, 0,0,} //magic 75%
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_EAR|ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            18, 27,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "mykonid", "mykonida", "mykonidowi", "mykonida", "mykonidem", "mykonidzie",
        FALSE, "", 100,
        "", "",
        A|E|M|dd, A|K|R|B|C|D|E|F|G|H|I,    PLANT|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_SHIELD|ITEM_WIELD|ITEM_HOLD|
            ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        6, 12,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "przemieszczacz", "przemieszczacza", "przemieszczaczowi", "przemieszczacza", "przemieszczaczem", "przemieszczaczu",
        FALSE, "", 200,
        "detect_magic blink", "",
        A|G,  A|B|C|D|E|F|H|I|J|K|N|Q|U|V, ANIMAL|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,} ,
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        11, 16,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "delfin", "delfina", "delfinowi", "delfina", "delfinem", "delfinie",
        FALSE, "", 150,
        "", "assist_race assist_align",
        A|H|V|bb|F,  A|D|F|J|K|O|Q|R|S,  ANIMAL,
        {"przyplynelo","przyplynal","przyplynela"},
        {"odplynelo","odplynal","odplynela"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        4, 5,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "sobowtór", "sobowtóra", "sobowtórowi", "sobowtóra", "sobowtórem", "sobowtóre",
        FALSE, "", 100,
        "", "assist_race",
        C|E|M|V|F,  A|B|C|D|E|F|D|H|I|J|K|R, MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,100,0,0, 0,0,} //immune sleep, charm
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            7, 9,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "drakolicz", "drakolicza", "drakoliczowi", "drakolicza", "drakoliczem", "drakoliczu",
        FALSE, "undead", 100,
        "detect_magic", "assist_race",
        I|Z,  A|B|C|F|G|H|I|J|K|P|V,  MONSTER|UNDEAD,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,100,100,0, 0,0,100,0,0, 0,0,} //imm charm,sleep,polymorph,cold,electricity,hold,insanity,death_spells
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            13, 33,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "pseudosmok", "pseudosmoka", "pseudosmokowi", "pseudosmoka", "pseudosmokiem", "pseudosmoku",
        FALSE, "", 150,
        "", "",
        Z,  A|B|C|D|E|F|H|I|J|K|P|Q|U|X|R|S, MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,35, 0,0,0,0,0, 0,0,0,0,0, 0,0,} //magic 35%
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            0, 10,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "drow", "drowa", "drowu", "drowa", "drowem", "drowie",
        FALSE, "", 100,
        "infrared detect_hidden", "",
        A|H|M|V|F, A|B|C|D|E|F|G|H|I|J|K|L|R, PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,15, 0,0,0,0,0, 0,0,0,0,0, 0,0,} //magic 15%
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            0, 40,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "driada", "driady", "driadzie", "driady", "driad±", "driadzie",
        FALSE, "druid", 100,
        "hide sneak detect_hidden", "assist_race",
        E|M|F,  A|B|C|G|H|I|J|K|R,  MONSTER|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,50, 0,0,0,0,0, 0,0,0,0,0, 0,0,} //magic 50%
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            9, 11,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "¿ywio³", "¿ywio³u", "¿ywio³owi", "¿ywio³", "¿ywio³em", "¿ywiole",
        FALSE, "", 100,
        "", "",
        C|E|M,  A|B|C|G|H|I|J|K,  MONSTER|OUTSIDER,
        {"przybylo","przybyl","przybyla"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        12, 38,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "s³oñ", "s³onia", "s³oniowi", "s³onia", "s³oniem", "s³oniu",
        FALSE, "", 150,
        "", "bash",
        G|F,  A|C|D|E|F|H|J|K|R|S,  ANIMAL|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_NECK,
        11, 18,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "martwy wypatrywacz", " martwego wypatrywacza", "matrwemu wypatrywaczowi", "martwego wypatrywacza", "martwym wypatrywaczem", "martwym wypatrywaczu",
        FALSE, "mage undead", 200,
        "regeneration detect_hidden detect_invis", "", C|E|I,
        A|F|J|K|M|N,  MONSTER|UNDEAD,
        {"przybylo","przybyl","przybyla"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,20, 0,0,0,0,0,100 ,2,0,0,0, 0,100,}, //20% na magie, resist_weapon 2, charm 100%
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        7, 30,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "ryba", "ryby", "rybie", "rybê", "ryb±", "rybie",
        FALSE, "", 100,
        "", "",
        A|G|bb, A|D|F|O|Q|R|X,   ANIMAL|MONSTER,
        {"przyplynelo","przyplynal","przyplynela"},
        {"odplynelo","odplynal","odplynela"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 18
    },


    {
        "¿aba", "¿aby", "¿abie", "¿abê", "¿ab±", "¿abie",
        FALSE, "", 20,
        "", "",
        A|G,  A|C|D|F|G|H|I|J|K|L|R|S,  ANIMAL|MONSTER,
        {"przyskoczylo","przyskoczyl","przyskoczyla"},
        {"odskoczylo","odskoczyl","odskoczyla"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 7,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "grzyb", "grzyba", "grzybu", "grzyb", "grzybem", "grzybie",
        FALSE, "", 100,
        "", "",
        A|E|M|dd, A|K|R,    PLANT,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        5, 12,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "gargulec", "gargulca", "gargulcowi", "gargulca", "gargulcem", "gargulcu",
        FALSE, "", 100,
        "infrared", "assist_race",
        E|M,  A|R|N|D|E|F|G|H|I|J|K|P|Q|U|V|R, MONSTER,
        {"przybylo","przybyl","przybyla"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        8, 10,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "ghul", "ghula", "ghulowi", "ghula", "ghulem", "ghulu",
        FALSE, "undead", 100,
        "", "assist_race",
        I|M|F,  A|B|C|F|G|H|I|J|K|R,  UNDEAD|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        6, 9,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "gith", "githa", "githowi", "githa", "githem", "githu",
        FALSE, "", 100,
        "infrared", "",
        A|M|V|X|F, A|B|C|D|E|F|G|H|I|J|K|U|R, PERSON|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        8, 20,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "githyanki", "githyanki", "githyanki", "githyanki", "githyanki", "githyanki",
        FALSE, "", 100,
        "infrared", "",
        A|M|V|F, A|B|C|D|E|F|G|H|I|J|K|R,  PERSON|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        8, 20,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "cieñ", "cienia", "cieniu", "cienia", "cieniem", "cieniu",
        FALSE, "undead", 100,
        "hide pass_door", "",
        C|I|K,  A|B|C|G|H|I|J|K|U|V,  UNDEAD,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},  //imm sleep,charm,cold
        {0,0,0,0,0, 0,0,0,100,0, 0,0,100,0,0, 0,0,},
        ITEM_WEAR_FLOAT,
        7, 9,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "gnoll", "gnolla", "gnollowi", "gnolla", "gnollem", "gnollu",
        FALSE, "", 100,
        "", "assist_race",
        A|G|M|V|F, A|B|C|D|E|F|G|H|I|J|K|Q|U|V|R, ANIMAL|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        3, 8,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "golem", "golema", "golemowi", "golema", "golemem", "golemie",
        FALSE, "", 100,
        "", "crush",   //zalezy od rodzaju golema
        C|E|M|J, A|B|C|G|H|I|J|K,  MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,} //w zaleznosci od rodzaju golemow rozne
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WEAR_FLOAT,
            7, 26,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "gremlin", "gremlina", "gremlinowi", "gremlina", "gremlinem", "gremlinie",
        FALSE, "", 110,
        "", "",
        A|G|M|V|F, A|B|C|D|E|F|G|H|I|J|K|R|S, ANIMAL|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        2, 9,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "gryf", "gryfa", "gryfowi", "gryfa", "gryfem", "gryfie",
        FALSE, "", 170,
        "detect_hidden flying", "",
        A|G|V|W|F, A|B|C|D|E|F|D|H|I|J|K|P|Q|U|V|R|S|T, ANIMAL|MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_WEAR_EAR|ITEM_WEAR_NECK|ITEM_WEAR_FLOAT,
            9, 11,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "slaad", "slaada", "slaadowi", "slaada", "slaadem", "slaadzie",
        FALSE, "", 100,
        "infrared",
        "assist_race",
        C, //forms
        A|B|C|D|E|F|G|H|I|J|K|Q|U|V|R, // parts
        DEMON|MONSTER|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        14, 20,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "harpia", "harpii", "harpii", "harpiê", "harpi±", "harpii",
        FALSE, "", 150,
        "", "",
        A|G|M|V|W|F, A|B|C|D|E|F|G|H|I|J|K|P|Q|U|V|R|S, ANIMAL|MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|
            ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|ITEM_WEAR_WAIST|ITEM_WIELD|ITEM_HOLD|
            ITEM_WEAR_EAR|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        10, 12,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "piekielny ogar", "piekielnego ogara", "piekielnemu ogarowi", "piekielnego ogara", "piekielnym ogarem", "piekielnym ogarze",
        FALSE, "", 130,
        "detect_magic detect_hidden", "",
        C|G|V|F, A|C|D|E|F|H|I|J|K|Q|U|V|R|S, ANIMAL|MONSTER|DEMON|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,10, 75,25,0,0,0, 0,0,0,0,0, 0,0,} //magic 10%
        ,
            ITEM_WEAR_EAR|ITEM_WEAR_NECK,
            8, 12,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "hydra", "hydry", "hydrze", "hydrê", "hydr±", "hydrze",
        FALSE, "", 150,
        "", "",
        C|G|R,  A|C|D|F|H|I|J|K|Q|U|V|S,  MONSTER,
        {"przypelz³o","przype³z³","przype³z³a"},
        {"odpe³z³o","odpe³z³","odpe³z³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_NECK|ITEM_WEAR_HEAD,
        17, 21,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "imp", "impa", "impowi", "impa", "impem", "impie",
        FALSE, "", 100,
        "detect_good detect_magic", "",
        A|M|V,  A|B|C|D|E|F|G|H|I|J|K,  ANIMAL|MONSTER|DEMON|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,25, 0,0,0,0,0, 0,0,0,0,0, 0,0,} //magic 25%
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            7, 9,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "niewidzialny my¶liwy", "niewidzialnego my¶liwego", "niewidzialnemu my¶liwemu", "niewidzialnego my¶liwgo", "niewidzialnym my¶liwym", "niewidzialnym my¶liwym",
        FALSE, "", 100,
        "invisible detect_invis", "",
        C|E|K|M, A|B|C|G|H|I|J|K|Q|U|V,  MONSTER|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,1,0,0,0, 0,0,} //non_magic_weapon
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            12, 14,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "ro¶lina", "ro¶linie", "ro¶liny", "ro¶linê", "ro¶lin±", "ro¶linie",
        FALSE, "", 100,
        "", "",
        A|aa,  V|X|Y|Z, PLANT,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        3, 25,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "thri-kreen", "thri-kreena", "thri-kreenowii", "thri-kreena", "thri-kreenem", "thri-kreenie",
        FALSE, "", 100,
        "", "",
        O|P,  A|B|C|D|E|F|G|I|K, INSECT|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, -10,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_HANDS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        5, 20,
        "wydaje d¼wiêk",
        "pyta",
        "Wydajesz d¼wiêk"
    },

    {
        "leprechaun", "leprechauna", "leprechaunowi", "leprechauna", "leprechaunem", "leprechaunie",
        FALSE, "", 100,
        "", "",
        E|M|V|F, A|B|C|D|E|D|H|I|J|K|P|R,  PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        5, 7,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "licz", "licza", "liczu", "licza", "liczem", "liczu",
        FALSE, "undead", 100,
        "", "",
        C|I|M,  A|B|C|G|H|I|J|K,   UNDEAD,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,100,100,0, 0,4,100,0,0, 0,0,} //imm charm, cold, sleep, polymorph, electricity, insanity, death_spells, magic_weapon_<_4
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            18, 21,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "¿yj±ca ¶ciana", "¿yj±cej ¶ciany", "¿yj±cej ¶cianie", "¿yj±c± ¶cianê", "¿yj±c± ¶cian±", "¿yj±cej ¶cianie",
        FALSE, "", 100,
        "", "",
        C/*|dd*/,  A|B|K|J,   PLANT,
        {"","",""},
        {"","",""},
        {0,0,0,0,20, 0,0,0,0,0, 0,0,0,0,0, 0,0,} //magic 20%
        ,
            0,
            12, 38,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "jaszczurocz³owiek", "jaszczurocz³owieka", "jaszczurocz³owiekowi", "jaszczurocz³owieka", "jaszczurocz³owiekiem", "jaszczurocz³owieku",
        FALSE, "", 100,
        "infrared", "",
        A|M|V|X, A|B|C|D|E|F|G|H|I|J|K|Q|U|V|R, PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_EAR|ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            4, 10,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "zwisak", "zwisaka", "zwisakowi", "zwisaka", "zwisakiem", "zwisaku",
        FALSE, "", 130,
        "", "",
        G|P,  A|C|D|F|H|K|S,   ANIMAL|MONSTER|INSECT,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            11, 13,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "likantrop", "likantropa", "likantropowi", "likantropa", "likantropem", "likantropie",
        FALSE, "", 100,
        "", "",
        A|G|J|M|V|bb|F, A|B|C|D|E|F|G|H|I|J|K|Q|V|R, MONSTER,
        {"przybylo","przybyl","przybyla"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            7, 12,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "mantykora", "mantykory", "mantykorze", "mantykorê", "mantykor±", "mantykorze",
        FALSE, "", 150,
        "", "",
        A|G|J|V|F, A|B|C|D|E|F|G|H|I|J|K|P|Q|U|V|R|S, MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_WEAR_NECK,
            9, 12,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "meduza", "meduzy", "meduzie", "meduzê", "meduz±", "meduzie",
        FALSE, "", 150,
        "", "",
        E|M|V,  A|R|C|D|E|F|G|H|I|J|K|N, MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_WEAR_EAR|ITEM_WEAR_NECK,
            4, 15,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "mimik", "mimika", "mimikowi", "mimika", "mimikiem", "mimiku",
        FALSE, "", 100,
        "hide", "",
        C|G|W,  A|C|D|E|F|H|J|K|P|U,  ANIMAL|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            10, 13,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "illithid", "illithida", "illithidowi", "illithida", "illithidem", "illithidzie",
        FALSE, "mage", 100,
        "infrared", "",
        E|M|V|Z, A|B|C|D|E|F|G|H|I|J|K,  PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,90, 0,0,0,0,0, 0,0,0,0,0, 0,0,} //magic 90%
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            13, 17,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "minotaur", "minotaura", "minotaurowi", "minotaura", "minotaurem", "minotaurze",
        FALSE, "warrior", 150,
        "", "bash",
        A|M|V|F, A|B|C|D|E|F|G|H|I|J|K|W|R|S, MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            11, 20,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "b³otny cz³owiek", "b³otnego cz³owieka", "b³otnemu cz³owiekowi", "b³otnego cz³owieka", "b³otnym cz³owiekiem", "b³otnym cz³owieku",
        FALSE, "", 100,
        "", "",
        E|M,  A|B|C|G|H|I|K|N,  PERSON|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 100,0,100,0,0, 0,0,} //imm poison,hold,charm,sleep
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            6, 12,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "mumia", "mumii", "mumiê", "mumiê", "mumi±", "mumii",
        FALSE, "undead", 100,
        "", "",
        C|I|M|J, A|B|C|G|H|I|J|K,  UNDEAD,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,100,0, 0,0,100,0,0, 0,0,} //imm sleep,charm,hold,cold
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            12, 18
    },

    {
        "mumia wiêksza", "mumii wiêkszej", "mumiê wiêksz±", "mumiê wiêksz±", "mumi± wiêksz±", "mumii wiêkszej",
        FALSE, "undead", 100,
        "", "",
        C|I|M|J, A|B|C|G|H|I|J|K,  UNDEAD,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,100,0, 0,0,100,0,0, 0,0,} //imm sleep,charm,hold,cold
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            18, 25,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "naga", "nagi", "nagê", "nagê", "nag±", "nadze",
        FALSE, "", 150,
        "infrared", "",
        A|J|M|V|Y, A|B|C|D|E|F|G|H|I|J|K|Q|U|R|S, MONSTER|OUTSIDER,
        {"przypelz³o","przype³z³","przype³z³a"},
        {"odpe³z³o","odpe³z³","odpe³z³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|
                ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            13, 17,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "ogr", "ogra", "ogrowi", "ogra", "ogrem", "ogrze",
        FALSE, "", 100,
        "", "assist_race kick bash",
        A|E|M|V|F, A|B|C|D|E|F|G|H|I|J|K|U|V|R, MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            10, 20,
            "grzmi",
            "pyta",
            "Grzmisz"
    },

    /*szlam,sluz,galareta*/
    {
        "ooze", "ooze", "ooze", "ooze", "ooze", "ooze",
        FALSE, "", 100,
        "water_breath", "",
        C|E|S,  B|C|K|N,   UNKNOWN,
        {"","",""},
        {"","",""},
        {0,70,100,50,5, 0,0,0,0,0, 0,1,0,0,0, 0,0,} //magic 5%, imm slash, pierce(70%), bash (50%), non_magic_weapon
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            0, 40,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "sowinied¼wied¼", "sowinied¼wiedzia", "sowinied¼wiedziowi", "sowinied¼wiedzia", "sowinied¼wiedziem", "sowinied¼wiedziu",
        FALSE, "", 150,
        "", "",
        A|C|G|J|V|W|F, A|C|D|E|F|H|I|J|K|P|Q|U|V|R|S|T, ANIMAL|MONSTER,
        {"przybieg³o","przybieg³","przybieg³a"},
        {"odbieg³o","odbieg³","odbieg³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_WEAR_NECK,
            7, 9,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "pegaz", "pegaza", "pegazowi", "pegaza", "pegazem", "pegazie",
        FALSE, "mountable", 200,
        "detect_good detect_evil flying", "assist_race",
        A|C|G|J|V|W|F, A|C|D|E|F|J|K|P|Q|R|S|T,  ANIMAL|MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,10, 0,0,0,0,0, 0,0,0,0,0, 0,0,} //magic 10%
        ,
            ITEM_WEAR_EAR|ITEM_WEAR_NECK,
            8, 10,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "feniks", "feniksa", "feniksowi", "feniksa", "feniksem", "feniksie",
        FALSE, "", 100,
        "detect_evil detect_magic protect_evil flying", "",
        C|W|F,  A|C|D|E|F|H|J|K|P|Q|U|R|T,  MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,50, 100,100,0,0,0, 0,3,0,0,0, 0,0,} //magic_weapon(+3 to hit), magic 50%, imm fire i magic fire
        ,
            ITEM_WEAR_NECK,
            25, 27,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "rakszasza", "rakszaszy", "rakszaszê", "rakszaszê", "rakszasz±", "rakszaszie",
        FALSE, "", 100,
        "", "",
        C|M|V|Y|F, A|B|C|D|E|F|G|H|I|J|K|Q|U|R, MONSTER|DEMON|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,1,0,0,0, 0,0,} //non_magic_weapon
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            13, 21,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "rdzawnik", "rdzawnika", "rdzawnikowi", "rdzawnika", "rdzawnikiem", "rdzawniku",
        FALSE, "", 100,
        "", "",
        C|G|O,  A|C|D|K,   ANIMAL|MONSTER,
        {"przypelz³o","przype³z³","przype³z³a"},
        {"odpe³z³o","odpe³z³","odpe³z³a"},
        {0,30,30,30,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,} //hard_body -> resist normal damage 30%
        ,
            ITEM_WEAR_NECK,
            5, 7,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "sahuagin", "sahuagina", "sahuaginowi", "sahuagina", "sahuaginem", "sahuaginie",
        FALSE, "", 100,
        "water_breath", "",
        A|M|V|bb|F, A|B|C|D|E|F|G|H|I|J|K|O|Q|U|R, PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            6, 13,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "satyr", "satyra", "satyrowi", "satyra", "satyrem", "satyrze",
        FALSE, "", 100,
        "", "",
        A|J|M|V|F, A|B|C|D|E|F|G|I|J|K|W/*|Z*/, PERSON|MONSTER|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,50, 0,0,0,0,0, 0,0,0,0,0, 0,0,} //magic 50%
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            10, 12,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "skorpion", "skorpiona", "skorpionowi", "skorpiona", "skorpionem", "skorpionie",
        FALSE, "", 100,
        "", "",
        G|O,  A|B|C|D|E|I|K|Q|U,  ANIMAL|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        6, 9,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "cieniskrzyd³y", "cieniskrzyd³ego", "cieniskrzyd³emu", "cieniskrzyd³ego", "cieniskrzyd³ym", "cieniskrzyd³ym",
        FALSE, "", 100,
        "infrared", "",
        A|R|W,  A|C|D|E|F|H|I|J|K|P|Q|U|V, MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        10, 16,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "syrena", "syreny", "syrenie", "syrenie", "syren±", "syrenie",
        FALSE, "", 100,
        "water_breath", "",
        A|J|M|V|bb, A|B|C|D|E|F|G|H|I|J|K|O|T,  MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,20, 0,0,0,0,0, 0,0,0,0,0, 0,0,} //magic 20%
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_HEAD|
                ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            4, 8,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "szkielet", "szkieleta", "szkieletowi", "szkielet", "szkieletem", "szkielecie",
        FALSE, "undead", 100,
        "", "",
        C|I|J,  A|B|C|G|H|I|J|K|R,  UNDEAD,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,100,0,0, 0,0,}, //imm sleep,charm,hold
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        4, 10,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "szkielet wojownik", "szkieletu wojownika", "szkieleta wojownika", "szkielet wojownik", "szkieletem wojownika", "szkielecie wojownika",
        FALSE, "undead", 100,
        "", "",
        C|I|J,  A|B|C|G|H|I|J|K|R,  UNDEAD,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,100,0,0, 0,0,}, //imm sleep,charm,hold
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        11, 16,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "paj±k", "paj±ka", "paj±kowi", "paj±ka", "paj±kiem", "paj±ku",
        FALSE, "", 100,
        "", "",
        G|P,  A|C|K,    ANIMAL|MONSTER|INSECT,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 17,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "widmo", "widma", "widmu", "widmo", "widmem", "widmie",
        FALSE, "undead", 100,
        "pass_door flying", "",
        C|I|M|W, A|B|C|G|H|I|J|K|P|U,  UNDEAD|OUTSIDER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,100,0, 0,1,100,0,0, 0,0,} //imm cold, charm, hold, non_magic_weapon
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            12, 14,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "tanar'ri", "tanar'ri", "tanar'ri", "tanar'ri", "tanar'ri", "tanar'ri",
        FALSE, "", 100,
        "detect_evil detect_good detect_magic", "",
        C|E|V, A|B|C|D|E|F|G|H|I|J|K|P|U|V, MONSTER|DEMON|OUTSIDER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,2,0,0,0, 0,0,} //imm non_&_low_magic_weapon(+2),
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            33, 34,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "tarask", "taraska", "taraskowi", "taraska", "taraskiem", "tarasku",
        FALSE, "", 100,
        "regeneratation haste detect_evil detect_good detect_magic detect_invis detect_hidden", "",
        C|G|X,  A|B|C|D|E|F|H|I|J|K|Q|U|V|X, MONSTER|DEMON|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 100,75,0,0,0, 0,3,0,0,0, 0,0,}, //imm fire,non_magic_weapon(+3)
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        37, 37,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "tasloi", "tasloi", "tasloi", "tasloi", "tasloi", "tasloi",
        FALSE, "", 100,
        "sneak  infrared", "",
        E|G|M|V, A|B|C|D|E|F|G|H|I|J|K,  MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        3, 8,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "tytan", "tytana", "tytanowi", "tytana", "tytanem", "tytanie",
        FALSE, "", 100,
        "protect_evil", "", //bless - nie ma
        A|M|V|F, A|B|C|D|E|F|G|H|I|J|K,  MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,50, 0,0,0,0,0, 0,0,0,0,0, 0,0,} //magic 50%
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_HEAD|
                ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_BODY|ITEM_WEAR_FEET|ITEM_WEAR_LEGS|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            25, 30,
            "grzmi",
            "pyta",
            "Grzmisz"
    },

    {
        "ropucha", "ropuchy", "ropusze", "ropuchê", "ropusze", "ropuch±",
        FALSE, "", 20,
        "", "",
        G,  A|C|D|E|F|G|H|I|J|K|L|R|S,  ANIMAL|MONSTER,
        {"przyskoczylo","przyskoczyl","przyskoczyla"},
        {"odskoczylo","odskoczyl","odskoczyla"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 4,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "ropucha olbrzymia", "ropuchy olbrzymiej", "ropusze olbrzymiej", "ropuchê olbrzymi±", "ropuch± olbrzymi±", "ropusze olbrzymiej",
        FALSE, "", 20,
        "", "",
        G, A|C|D|E|F|G|H|I|J|K|L|R|S,  ANIMAL|MONSTER,
        {"przyskoczylo","przyskoczyl","przyskoczyla"},
        {"odskoczylo","odskoczyl","odskoczyla"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        4, 7,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "drzewiec", "drzewca", "drzewcowi", "drzewca", "drzewcem", "drzewcu",
        FALSE, "", 100,
        "", "",
        C|E/*|dd*/, J|K/*|S|T*/,   PLANT,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND,
        12, 17,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "troglodyta", "troglodyty", "troglodycie", "troglodytê", "troglodyt±", "troglodycie",
        FALSE, "", 100,
        "", "",
        A|M|V,  A|B|C|D|E|F|G|H|I|J|K|R, MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            5, 9,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "krowa", "krowy", "krowie", "krowê", "krow±", "krowie",
        FALSE, "", 100,
        "", "kick",
        A|F|G|V, A|B|C|D|E|J|K|R|S, ANIMAL|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_WEAR_NECK,
            2, 5,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "umberkolos", "umberkolosa", "umberkolosowi", "umberkolosa", "umberkolosem", "umberkolosie",
        FALSE, "", 150,
        "infrared", "",
        G|M|O|V, A|B|C|D|E|F|G|H|I|J|K|U|Y|S, MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            11, 15,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "jednoro¿ec", "jednoro¿ca", "jednoro¿cowi", "jednoro¿ca", "jednoro¿cem", "jednoro¿cu",
        FALSE, "", 200,
        "detect_evil", "",
        A|C|G|V|F, A|C|D|E|F|J|K|Q|Z|R|S,  ANIMAL|MONSTER,
        {"przytruchta³o","przytruchta³","przytruchta³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_WEAR_EAR|ITEM_WEAR_NECK,
            7, 10,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "wampir", "wampira", "wampirowi", "wampira", "wampirem", "wampirze",
        FALSE, "undead", 100,
        "detect_hidden", "",
        C|I|M|V, A|B|C|D|G|H|I|J|K|R,  UNDEAD,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,1,0,0,0, 0,0,} //imm charm,fear, non_magic_weapon
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            17, 20,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "zmora", "zmory", "zmorze", "zmorê", "zmor±", "zmorze",
        FALSE, "undead", 100,
        "pass_door", "",
        I|M|V,  A|B|C|G|H|I|J|K|U|V,  UNDEAD|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,1,100,0,0, 0,0,} //imm non_magic_weapon,sleep,charm,hold
        ,
            ITEM_WEAR_NECK,
            10, 12,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "b³êdny ognik", "b³êdnego ognika", "b³êdnemu ognikowi", "b³êdnego ognika", "b³êdnym ognikiem", "b³êdnym ogniku",
        FALSE, "", 100,
        "protectn_evil", "",
        C|S|F,  K,    OUTSIDER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,5, 0,0,0,0,0, 0,0,0,0,0, 0,0,}, //magic 5%
        0,
        12, 15,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "³akowilk", "³akowilka", "³akowilkowi", "³akowilka", "³akowilkiem", "³akowilku",
        FALSE, "", 100,
        "detect_hidden", "",
        G|M|V|F, A|B|C|D|E|F|G|H|I|J|K|U|V|R, MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,10, 0,0,0,0,0, 0,1,0,0,0, 0,0,} //im non_magic_weapon, magic 10%
        ,
            ITEM_WEAR_EAR|ITEM_WEAR_NECK,
            10, 12,
            "warczy",
            "warczy",
            "Warczysz"
    },

    {
        "czerw", "czerwia", "czerwiowi", "czerwia", "czerwiem", "czerwiu",
        FALSE, "", 100,
        "", "",
        A|G|R,  A|D|K|Q||S|V|X,   ANIMAL|MONSTER|INSECT,
        {"przypelz³o","przype³z³","przype³z³a"},
        {"odpe³z³o","odpe³z³","odpe³z³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            0,
            0, 24,
            "piszczy",
            "piszczy",
            "Piszczysz"
    },

    {
        "upiór", "upióra", "upiórowi", "upióra", "upiórem", "upiórze",
        FALSE, "undead", 100,
        "pass_door", "",
        I|K|M,  A|B|C|G|H|I|J|K,  UNDEAD|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,1,100,0,0, 0,0,} //imm non_magic_weapon,sleep,charm,hold
        ,
            ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
                ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
                ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
            11, 13,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "koñ", "konia", "koniowi", "konia", "koniem", "koniu",
        FALSE, "", 100,
        "", "",
        A|F|G|V, A|C|D|E|F|J|K|Q|R|S, ANIMAL|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,}
        ,
            ITEM_WEAR_EAR|ITEM_WEAR_NECK,
            2, 5,
            "mówi",
            "pyta",
            "Mówisz"
    },

    {
        "yuan-ti", "yuan-ti", "yuan-ti", "yuan-ti", "yuan-ti", "yuan-ti",
        FALSE, "", 100,
        "", "",
        A|G|J|M|V|Y|F, A|B|D|E|F|G|I|J|K|Q|U|R,  MONSTER,
        {"przype³z³o","przype³z³","przype³z³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,70, 0,0,0,0,0, 0,0,}, //poison 70%
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        11, 19,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "¿uk", "¿uka", "¿ukowi", "¿uka", "¿ukiem", "¿uku",
        FALSE, "", 100,
        "", "",
        G|R, A|C|U, ANIMAL|INSECT,
        {"przypelz³o","przype³z³","przype³z³a"},
        {"odpe³z³o","odpe³z³","odpe³z³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 3,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "owad pe³zaj±cy", "owada pe³zaj±cego", "owadowi pe³zaj±cemu", "owada pe³zaj±cego", "owadem pe³zaj±cym", "owadzie pe³zaj±cym",
        FALSE, "", 100,
        "", "",
        G|R,  A|C,   ANIMAL|INSECT,
        {"przypelz³o","przype³z³","przype³z³a"},
        {"odpe³z³o","odpe³z³","odpe³z³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 10,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "owad lataj±cy", "owada lataj±cego", "owadowi lataj±cemu", "owada lataj±cego", "owadem lataj±cym", "owadzie lataj±cym",
        FALSE, "", 100,
        "flying", "",
        O,  A|C|P,   ANIMAL|INSECT,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,} ,
        0,
        0, 10,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "wa¿ka", "wa¿ki", "wa¿ce", "wa¿kê", "wa¿k±", "wa¿ce",
        FALSE, "", 100,
        "flying", "",
        O,  A|C|P,   ANIMAL|INSECT,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,} ,
        0,
        0, 3,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "dzik",  "dzika", "dzikowi", "dzika", "dzikiem", "dziku",
        FALSE, "memory", 130,
        "infrared detect_hidden", "assist_race", A|G|F|V,
        A|C|D|E|F|J|K|R|S, ANIMAL|MONSTER,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        0, 4,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "wiewiórka", "wiewiórki", "wiewiórce", "wiewiórkê", "wiewiórk±", "wiewiórce",
        FALSE, "wimpy", 20,
        "dark_vision detect_hidden",  "", A|G|V|F,
        A|C|D|E|F|J|K|Q|R|S, ANIMAL,
        {"przykica³o","przykica³","przykica³a"},
        {"odkica³o","odkica³","odkica³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_NECK,
        0, 2,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "tygrys", "tygrysa", "tygrysowi", "tygrysa", "tygrysem", "tygrysie",
        FALSE, "mountable memory", 150,
        "dark_vision infrared detect_hidden", "berserk", A|G|V|F,
        A|C|D|E|F|J|K|Q|R|S|V, ANIMAL|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,30,50,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        2, 10,
        "ryczy",
        "ryczy",
        "Ryczysz"
    },

    {
        "mysz", "myszy", "myszy", "mysz", "mysz±", "myszy",
        FALSE, "wimpy", 20,
        "dark_vision detect_hidden",  "", A|G|V|F,
        A|C|D|E|F|J|K|Q|R|S, ANIMAL,
        {"przysz³o","przyszed³","przysz³a"},
        {"odesz³o","odszed³","odesz³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_NECK,
        0, 1,
        "piszczy",
        "piszczy",
        "Piszczysz"
    },

    {
        "szczur", "szczura", "szczurowi", "szczura", "szczurem", "szczurze",
        FALSE, "", 20,
        "dark_vision detect_hidden",  "", A|G|V|F,
        A|C|D|E|F|J|K|Q|R|S, ANIMAL,
        {"przysz³o","przyszed³","przysz³a"},
        {"odesz³o","odszed³","odesz³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_NECK,
        0, 2,
        "piszczy",
        "piszczy",
        "Piszczysz"
    },

    {
        "behemot", "behemota", "behemotowi", "behemota", "behemotem", "behemocie",
        FALSE, "memory", 200,
        "regeneration detect_hidden", "berserk crush bash", A|G|V|F,
        A|B|C|D|E|F|I|J|K|R|Y|S, MONSTER|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,40,60,0, 0,0,},
        ITEM_WEAR_NECK,
        20, 32,
        "grzmi",
        "grzmi",
        "Grzmisz"
    },

    {
        "yeti", "yeti", "yeti", "yeti", "yeti", "yeti",
        FALSE,  "", 150,
        "detect_hidden", "", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R|S, MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,50,0,0, 0,0,0,0,0, 0,0,},//50% resist na cold
        ITEM_TAKE|ITEM_WEAR_NECK|
            ITEM_WEAR_EAR|ITEM_WEAR_ARMS|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_HOLD|ITEM_WEAR_FLOAT,
        7, 9,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "smokowiec", "smokowca", "smokowcu", "smokowca", "smokowcem", "smokowcu",
        FALSE, "", 100,
        "infrared", "assist_race", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|J|K|R, PERSON|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        4, 8,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "demon", "demona", "demonowi", "demona", "demonem", "demonie",
        FALSE, "", 100,
        "detect_invis detect_hidden infrared fly",
        "fast two_attack",
        C, //forms
        A|B|C|D|E|F|G|H|I|J|K|R, // parts
        DEMON|MONSTER|OUTSIDER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,20,20,20,5, 10,10,10,10,10, 10,1,100,100,100, 10,10,}, //no to w koñcu demon jest, nie?
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        24, 28,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "rój owadów", "róju owadów", "rójowi owadów", "rój owadów", "rójem owadów", "róju owadów",
        FALSE, "", 100,
        "", "",
        0, 0, ANIMAL|INSECT,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 5,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "celestial", "celestiala", "celestialowi", "celestiala", "celestialem", "celestialu",
        FALSE,  "", 100,
        "fly detect_invis detect_hidden infrared", "", A|H|M|V|F,
        A|B|C|D|E|F|G|H|I|K|R|P, PERSON|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odesz³o","odszed³","odesz³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_EAR|ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        22, 36,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "królik", "królika", "królikowi", "królika", "królikiem", "króliku",
        FALSE, "",  70,
        "detect_hidden",  "", A|G|V|F,
        A|C|D|E|F|H|J|K|R|S, ANIMAL,
        {"przykica³o","przykica³","przykica³a"},
        {"odkica³o","odkica³","odkica³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_NECK,
        0, 2,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "¶nie¿ny wilk", "¶nie¿nego wilka", "¶nie¿nemu wilkowi", "¶nie¿nego wilka", "¶nie¿nym wilkiem", "¶nie¿nym wilku",
        FALSE, "", 170,
        "dark_vision detect_hidden resist_cold", "assist_race", A|G|V|F,
        A|C|D|E|F|J|K|Q|V|R|S, ANIMAL|MONSTER,
        {"przybieg³o","przybieg³","przybieg³a"},
        {"odbieg³o","odbieg³","odbieg³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        5, 15,
        "warczy",
        "warczy",
        "Mówisz"
    },

    {
        "jeleñ", "jelenia", "jeleniowi", "jelenia", "jeleniem", "jeleniu",
        FALSE, "", 110,
        "detect_hidden fear", "assist_race", A|G|V|F,
        A|C|D|E|F|J|K|Q|V|R|S, ANIMAL,
        {"przybieg³o","przybieg³","przybieg³a"},
        {"odbieg³o","odbieg³","odbieg³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        5, 10,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "daniel", "daniela", "danielowi", "daniela", "danielem", "danielu",
        FALSE, "", 110,
        "detect_hidden fear", "assist_race", A|G|V|F,
        A|C|D|E|F|J|K|Q|V|R|S, ANIMAL,
        {"przybieg³o","przybieg³","przybieg³a"},
        {"odbieg³o","odbieg³","odbieg³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        3, 8
    },
    {
        "ry¶", "rysia", "rysiowi", "rysia", "rysiem", "rysiu",
        FALSE, "", 100,
        "dark_vision detect_hidden sneak", "assist_race", A|G|V|F,
        A|C|D|E|F|J|K|Q|V|R|S, ANIMAL,
        {"przybieg³o","przybieg³","przybieg³a"},
        {"odbieg³o","odbieg³","odbieg³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_EAR|ITEM_WEAR_NECK,
        5, 8,
        "ryczy",
        "ryczy",
        "Ryczysz"
    },

    {
        "mefit", "mefita", "mefitowi", "mefita", "mefitem", "meficie",
        FALSE, "", 100,
        "detect_hidden flying dark_vision", "dodge", A|C|G|V|F,
        A|C|D|E|F|H|J|K|P|R, ANIMAL|MONSTER|DEMON|OUTSIDER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_WEAR_NECK,
        2, 6,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "ptak nielot", "ptaka nielota", "ptakowi nielotowi", "ptaka nielota", "ptakiem nielotem", "ptaku nielocie",
        FALSE, "", 100,
        "detect_hidden", "", A|G|W|F,
        A|C|D|E|F|H|K|P|R|S|T, ANIMAL,
        {"przysz³o","przyszed³","przysz³a"},
        {"odesz³o","odszed³","odesz³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 10,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "necrophidius", "necrophidiusa", "necrophidiusowi", "necrophidiusa", "necrophidiusem", "necrophidiusie",
        FALSE, "", 100,
        "detect_hidden",
        "",
        C|J, // to nie jest undead, ale konstrukt - golem z kosci (kregoslup + czaszka)
        A|Q, // ten stworek to taki niby-waz - ogon + glowa
        MONSTER,
        {"przype³z³o","przype³z³","przype³z³a"},
        {"odpe³z³o","odpe³z³","odpe³z³a"},
        {0,0,0,0,0, 0,0,0,0,100, 0,0,0,0,0, 0,100,}, //resist poison 100% i mental 100%
        0,
        15,
        25,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "¿ywio³ak", "¿ywio³aka", "¿ywio³akowi", "¿ywio³aka", "¿ywio³akiem", "¿ywio³aku",
        FALSE, "", 100,
        "", "", C,
        A|B|C|G|H|I, MONSTER|OUTSIDER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odesz³o","odszed³","odesz³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,100,}, //resist mental 100%
        0,
        10, 22,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "kura", "kury", "kurze", "kurê", "kur±", "kurze",
        FALSE, "", 50,
        "detect_hidden", "", A|G|W|F,
        A|C|D|E|F|H|K|P|R|S|T, ANIMAL|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odesz³o","odszed³","odesz³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 10,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "kogut", "koguta", "kogutowi", "koguta", "kogutem", "kogucie",
        FALSE, "", 100,
        "detect_hidden", "", A|G|W|F,
        A|C|D|E|F|H|K|P|R|S|T, ANIMAL|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odesz³o","odszed³","odesz³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 10,
        "pieje",
        "pieje",
        "Piejesz"
    },

    {
        "gê¶", "gêsi", "gêsi", "gê¶", "gêsi±", "gêsi",
        FALSE, "", 100,
        "detect_hidden", "", A|G|W|F,
        A|C|D|E|F|H|K|P|R|S|T, ANIMAL|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odesz³o","odszed³","odesz³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 10,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "kaszcz", "kaszcza", "kaszczowi", "kaszcza", "kaszczem", "kaszczu",
        FALSE, "aggressive", 100,
        "detect_hidden detect_hidden dark_vision", "", A|H|M|V|cc,
        C|D|E|F|G|H|I|K|R|U, MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odesz³o","odszed³","odesz³a"},
        {0,0,0,0,10, -3,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_LEGS|ITEM_WEAR_FLOAT|ITEM_WIELD,
        8, 18,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "ghast", "ghasta", "ghastowi", "ghasta", "ghastem", "ghascie",
        FALSE, "undead", 100,
        "", "assist_race",
        I|M|F,  A|B|C|F|G|H|I|J|K|R,  UNDEAD|MONSTER,
        {"przysz³o","przyszed³","przysz³a"},
        {"odchodzi","odchodzi","odchodzi"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT,
        6, 9,
        "mówi",
        "pyta",
        "Mówisz"
    },

    {
        "sêp", "sêpa", "sêpowi", "sêpa", "sêpem", "sêpie",
        FALSE, "", 20,
        "flying infrared sneak", "dodge", A|G|W|F,
        A|C|D|E|F|H|K|P|R|S|T, ANIMAL|MONSTER,
        {"przylecia³o","przylecia³","przylecia³a"},
        {"odlecia³o","odlecia³","odlecia³a"},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,},
        0,
        0, 12,
        "skrzeczy",
        "skrzeczy",
        "¦piewasz"
    },

    {
        NULL, NULL, NULL, NULL, NULL, NULL, 0, "", 100, "", "", 0, 0, 0,
        {"","",""},
        {"","",""},
        {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,} ,
        ITEM_TAKE|ITEM_WEAR_FINGER|ITEM_WEAR_NECK|ITEM_WEAR_BODY|ITEM_WEAR_HEAD|ITEM_WEAR_LEGS|
            ITEM_WEAR_FEET|ITEM_WEAR_HANDS|ITEM_WEAR_ARMS|ITEM_WEAR_SHIELD|ITEM_WEAR_ABOUT|ITEM_WEAR_EAR|
            ITEM_WEAR_WAIST|ITEM_WEAR_WRIST|ITEM_WIELD|ITEM_HOLD|ITEM_WIELDSECOND|ITEM_WEAR_LIGHT|ITEM_WEAR_FLOAT|ITEM_INSTRUMENT,
        0, 40,
        "mówi",
        "pyta",
        "Mówisz"
    }

};

