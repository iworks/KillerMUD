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
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: handler.c 11986 2013-01-23 13:13:07Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/handler.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "progs.h"
#include "lang.h"
#include "music.h"
#include "clans.h"
#include "mount.h"
#include "money.h"
#include "paladin_auras.h"
#include "track.h"

bool check_illusion          args( ( CHAR_DATA *ch ) );
int calculate_spec_damage    args( ( CHAR_DATA *ch, CHAR_DATA *vch, OBJ_DATA *weapon, int spec_type ) );
int flag_lookup              args( ( const char *name, const struct flag_type *flag_table) );
int get_mem_slots            args( ( CHAR_DATA *ch, int circle, bool free) );
int weapon_skill_mod         args( ( CHAR_DATA *ch,bool primary,bool hitr) );
OBJ_DATA * find_boat         args( ( CHAR_DATA *ch) );
sh_int get_caster            args( ( CHAR_DATA *ch) );
void affect_modify           args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );
void bonus_remove            args( ( CHAR_DATA *ch, BONUS_DATA *bonus ) );
void bonus_to_char           args( ( CHAR_DATA *ch, BONUS_INDEX_DATA *pBonus ) );
void check_max_spells_memmed args( ( CHAR_DATA * ch ) );
void dispose_memory          args( ( CHAR_DATA *mob) );
void init_supermob();
void raw_damage              args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam ) );
void raw_kill                args( ( CHAR_DATA *victim , CHAR_DATA *ch ) );
void release_supermob();
void update_poison           args( ( CHAR_DATA *victim) );

int wear_to_itemwear[MAX_WEAR] =
{
    ITEM_WEAR_LIGHT,
    ITEM_WEAR_FINGER,
    ITEM_WEAR_FINGER,
    ITEM_WEAR_NECK,
    ITEM_WEAR_NECK,
    ITEM_WEAR_BODY,
    ITEM_WEAR_HEAD,
    ITEM_WEAR_LEGS,
    ITEM_WEAR_FEET,
    ITEM_WEAR_HANDS,
    ITEM_WEAR_ARMS,
    ITEM_WEAR_SHIELD,
    ITEM_WEAR_ABOUT,
    ITEM_WEAR_WAIST,
    ITEM_WEAR_WRIST,
    ITEM_WEAR_WRIST,
    ITEM_WIELD,
    ITEM_HOLD,
    ITEM_WEAR_FLOAT,
    ITEM_WIELDSECOND,
    ITEM_INSTRUMENT,
    ITEM_WEAR_EAR,
    ITEM_WEAR_EAR
};

/* friend stuff -- for NPC's mostly */
bool is_friend(CHAR_DATA *ch,CHAR_DATA *victim)
{
    if (is_same_group(ch,victim))
        return TRUE;

    if (!IS_NPC(ch))
        return FALSE;

    if (!IS_NPC(victim))
    {
        if (EXT_IS_SET(ch->off_flags,OFF_ASSIST_PLAYERS))
            return TRUE;
        else
            return FALSE;
    }

    if (IS_AFFECTED(ch,AFF_CHARM))
        return FALSE;

    if (EXT_IS_SET(ch->off_flags,OFF_ASSIST_ALL))
        return TRUE;

    if (ch->group && ch->group == victim->group)
        return TRUE;

    if (EXT_IS_SET(ch->off_flags,OFF_ASSIST_VNUM) && ch->pIndexData == victim->pIndexData)
        return TRUE;

    if (EXT_IS_SET(ch->off_flags,OFF_ASSIST_RACE) && GET_RACE(ch) == GET_RACE(victim))
        return TRUE;

    if (EXT_IS_SET(ch->off_flags,OFF_ASSIST_ALIGN)
            &&  !EXT_IS_SET(ch->act,ACT_NOALIGN) && !EXT_IS_SET(victim->act,ACT_NOALIGN)
            &&  ((IS_GOOD(ch) && IS_GOOD(victim))
                ||	 (IS_EVIL(ch) && IS_EVIL(victim))
                ||   (IS_NEUTRAL(ch) && IS_NEUTRAL(victim))))
        return TRUE;

    return FALSE;
}

/*IMPROVEMENT: Brohacz: zwraca slowo 'godzina' w odpowiedniej formie: (za 1) godzinê, (za 2) godziny...*/
const char* hour_to_str ( int value )
{
    if ( value==1 ) return "godzinê";
    if ( value % 10 < 5 && ( value % 100 ) / 10 != 1 && value % 10 > 1 ) return "godziny";
    return "godzin";
}

/* returns number of people on an object */
int count_users(OBJ_DATA *obj)
{
    CHAR_DATA *fch;
    int count = 0;

    if (obj->in_room == NULL)
        return 0;

    for (fch = obj->in_room->people; fch != NULL; fch = fch->next_in_room)
        if (fch->on == obj)
            count++;

    return count;
}

/* returns material number */
int material_lookup (const char *name)
{
    sh_int x;
    for(x=1;material_table[x].name;x++)
        if( NOPOL(name[0]) == NOPOL(material_table[x].name[0]) && !str_prefix(name,material_table[x].name) )
            return x;
    return 0; /*unknown itp*/
}

int weapon_lookup (const char *name)
{
    int type;

    for (type = 0; weapon_table[type].name != NULL; type++)
    {
        if (NOPOL(name[0]) == NOPOL(weapon_table[type].name[0])
                &&  !str_prefix(name,weapon_table[type].name))
            return type;
    }

    return -1;
}

int instrument_lookup (const char *name)
{
    int type;

    for (type = 0; instrument_table[type].name != NULL; type++)
    {
        if (NOPOL(name[0]) == NOPOL(instrument_table[type].name[0])
                &&  !str_prefix(name,instrument_table[type].name))
            return type;
    }

    return -1;
}

int weapon_type (const char *name)
{
    int type;

    for (type = 0; weapon_table[type].name != NULL; type++)
    {
        if (NOPOL(name[0]) == NOPOL(weapon_table[type].name[0])
                &&  !str_prefix(name,weapon_table[type].name))
            return weapon_table[type].type;
    }

    return WEAPON_EXOTIC;
}

int instrument_type (const char *name)
{
    int type;

    for (type = 0; instrument_table[type].name != NULL; type++)
    {
        if (NOPOL(name[0]) == NOPOL(instrument_table[type].name[0])
                &&  !str_prefix(name,instrument_table[type].name))
            return instrument_table[type].type;
    }

    return INSTRUMENT_OCARINA;
}

char *item_name(int item_type)
{
    int type;

    for (type = 0; item_table[type].name != NULL; type++)
        if (item_type == item_table[type].type)
            return item_table[type].name;

    return "none";
}

char *weapon_name( int weapon_type)
{
    int type;

    for (type = 0; weapon_table[type].name != NULL; type++)
        if (weapon_type == weapon_table[type].type)
            return weapon_table[type].name;

    return "exotic";
}

char *instrument_name( int instrument_type)
{
    int type;

    for (type = 0; instrument_table[type].name != NULL; type++)
        if (instrument_type == instrument_table[type].type)
            return instrument_table[type].name;

    return "okaryna";
}

int attack_lookup  (const char *name)
{
    int att;

    for ( att = 0; attack_table[att].name != NULL; att++)
    {
        if (NOPOL(name[0]) == NOPOL(attack_table[att].name[0])
                &&  !str_prefix(name,attack_table[att].name))
            return att;
    }

    return 0;
}

/* returns a flag for wiznet */
long wiznet_lookup (const char *name)
{
    int flag;

    for (flag = 0; wiznet_table[flag].name != NULL; flag++)
    {
        if (NOPOL(name[0]) == NOPOL(wiznet_table[flag].name[0])
                && !str_prefix(name,wiznet_table[flag].name))
            return flag;
    }

    return -1;
}

/* returns class number */
int class_lookup (const char *name)
{
    int class;

    for ( class = 0; class < MAX_CLASS; class++)
    {
        if (NOPOL(name[0]) == NOPOL(class_table[class].name[0])
                &&  !str_prefix( name,class_table[class].name))
            return class;
    }

    return -1;
}

/* for immunity, vulnerabiltiy, and resistant
   the 'globals' (magic and weapons) may be overriden
   three other cases -- wood, s.ilver, and iron -- are checked in fight.c */

int check_immune(CHAR_DATA *ch, int dam_type)
{
    int immune, def;

    immune = -1;
    def = IS_NORMAL;

    if (dam_type == DAM_NONE)
        return immune;

    return 0;
}

/* checks mob format */
bool is_old_mob(CHAR_DATA *ch)
{
    if (ch->pIndexData == NULL)
        return FALSE;
    else if (ch->pIndexData->new_format)
        return FALSE;

    return TRUE;
}

/* tylko do skilli, zwraca wytrenowania dla graczy i mobow(generowane)*/
int get_skill( CHAR_DATA *ch, int sn )
{
    int skill = 0;
    if ( sn == -1 )  /* shorthand for level based skills */
    {
        skill = ch->level * 5 / 2;
    }
    else if ( sn < -1 || sn > MAX_SKILL )
    {
        bug( "Bad sn %d in get_skill.", sn );
        return 0;
    }

    else if ( !IS_NPC( ch ) )
    {
        if ( ch->level < skill_table[ sn ].skill_level[ ch->class ] )
            skill = 0;
        else
            skill = ch->pcdata->learned[ sn ] > 0 ? ch->pcdata->learned[ sn ] + ch->pcdata->learn_mod[ sn ] : 0;
    }
    else /* mobiles */
    {
        if ( sn == gsn_hide || sn == gsn_sneak )
        {
            skill = EXT_IS_SET( ch->act, ACT_THIEF ) ? 30 + ch->level * 5 : 0;
        }
        else if ( sn == gsn_trip )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_TRIP ) )
                return 0;

            skill = EXT_IS_SET( ch->act, ACT_THIEF ) ? 30 + ch->level * 2 : 10 + 3 * ch->level / 2;
        }
        else if ( sn == gsn_backstab )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_BACKSTAB ) )
                return 0;

            skill = EXT_IS_SET( ch->act, ACT_THIEF ) ? 30 + ch->level * 2 : 10 + 3 * ch->level / 2;
        }
        else if ( sn == gsn_parry )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_PARRY ) )
                return 0;

            skill = ( EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) || EXT_IS_SET( ch->act, ACT_WARRIOR ) || EXT_IS_SET( ch->act, ACT_PALADIN ) ) ? 30 + ch->level * 2 : 10 + 3 * ch->level / 2;
        }
        else if ( sn == gsn_dodge )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_DODGE ) )
                return 0;

            skill = EXT_IS_SET( ch->act, ACT_THIEF ) ? 30 + ch->level * 2 : 10 + 3 * ch->level / 2;
        }
        else if ( sn == gsn_bash )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_BASH ) )
                return 0;

            skill = ( EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) || EXT_IS_SET( ch->act, ACT_WARRIOR ) || EXT_IS_SET( ch->act, ACT_PALADIN ) ) ? 30 + ch->level * 2 : 10 + 3 * ch->level / 2;
        }
        else if ( sn == gsn_slam )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_SLAM ) )
                return 0;

            skill = EXT_IS_SET( ch->act, ACT_BARBARIAN ) ? 30 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_devour )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_DEVOUR ) )
                return 0;

            skill = 40 + ch->level * 2;
        }
        else if ( sn == gsn_stun )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_STUN ) )
                return 0;

            skill = ( EXT_IS_SET( ch->act, ACT_WARRIOR ) || EXT_IS_SET( ch->act, ACT_PALADIN ) || EXT_IS_SET( ch->act, ACT_CLERIC ) ) ? 30 + ch->level * 2 : 10 + 3 * ch->level / 2;
        }
        else if ( sn == gsn_charge )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_CHARGE ) )
                return 0;

            skill = EXT_IS_SET( ch->act, ACT_BARBARIAN ) ? 30 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_recuperate )
        {
            skill = EXT_IS_SET( ch->act, ACT_BARBARIAN ) ? 45 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_kick )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_KICK ) )
                return 0;

            skill = ( EXT_IS_SET( ch->act, ACT_WARRIOR ) || EXT_IS_SET( ch->act, ACT_PALADIN ) ) ? 30 + ch->level * 2 : 10 + 3 * ch->level / 2;
        }
        else if ( sn == gsn_crush )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_CRUSH ) )
                return 0;

            skill = 100;
        }
        else if ( sn == gsn_call_avatar )
        {
            skill = EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) ? ch->level*3 : 0;
        }
        else if ( sn == gsn_tail )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_TAIL ) )
                return 0;

            skill = 100;
        }
        else if ( sn == gsn_fire_breath )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_FIRE_BREATH ) )
                return 0;

            skill = 100;
        }
        else if ( sn == gsn_frost_breath )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_FROST_BREATH ) )
                return 0;

            skill = 100;
        }
        else if ( sn == gsn_acid_breath )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_ACID_BREATH ) )
                return 0;

            skill = 100;
        }
        else if ( sn == gsn_lightning_breath )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_LIGHTNING_BREATH ) )
                return 0;

            skill = 100;
        }
        else if ( sn == gsn_lay )
        {
            skill = EXT_IS_SET( ch->act, ACT_PALADIN ) ? 30 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_turn )
        {
            skill = ( EXT_IS_SET( ch->act, ACT_PALADIN ) || EXT_IS_SET( ch->act, ACT_CLERIC ) ) ? ch->level * 3 : 0;
        }
        else if ( sn == gsn_torment )
        {
            skill = EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) ? 30 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_garhal )
        {
            skill = EXT_IS_SET( ch->act, ACT_DRUID ) ? 30 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_rescue )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_RESCUE ) )
                return 0;

            skill = ( EXT_IS_SET( ch->act, ACT_WARRIOR ) || EXT_IS_SET( ch->act, ACT_BARBARIAN ) || EXT_IS_SET( ch->act, ACT_PALADIN ) ) ? 40 + ch->level * 2 : 10 + 3 * ch->level / 2;
        }
        else if ( sn == gsn_smite_good )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_SMITE_GOOD ) || !IS_EVIL(ch) )
                return 0;

            skill = ( EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) ) ? 40 + ch->level * 2 : 10 +  ch->level;
        }
        else if ( sn == gsn_smite )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_SMITE_EVIL ) || !IS_GOOD(ch) )
                return 0;

            skill = ( EXT_IS_SET( ch->act, ACT_PALADIN ) ) ? 40 + ch->level * 2 : 10 +  ch->level;
        }
        else if ( sn == gsn_vertical_slash )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_VERTICAL_SLASH ) )
                return 0;

            skill = ( EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) || EXT_IS_SET( ch->act, ACT_WARRIOR ) ) ? 40 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_cleave )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_CLEAVE ) )
                return 0;

            skill = ( EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) || EXT_IS_SET( ch->act, ACT_WARRIOR ) ) ? 40 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_overwhelming_strike )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_OVERWHELMING_STRIKE ) )
                return 0;

            skill = ( EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) || EXT_IS_SET( ch->act, ACT_WARRIOR ) ) ? 40 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_disarm )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_DISARM ) )
                return 0;

            skill = ( EXT_IS_SET( ch->act, ACT_WARRIOR ) || EXT_IS_SET( ch->act, ACT_PALADIN ) ) ? 30 + ch->level * 2 : 10 + 3 * ch->level / 2;
        }
        else if ( sn == gsn_berserk )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_BERSERK ) )
                return 0;

            skill = EXT_IS_SET( ch->act, ACT_BARBARIAN ) ? 35 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_wardance )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_WARDANCE ) )
                return 0;

            skill = EXT_IS_SET( ch->act, ACT_WARRIOR ) ? 35 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_damage_reduction )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_DAMAGE_REDUCTION ) )
                return 0;

            skill = EXT_IS_SET( ch->act, ACT_BARBARIAN ) ? 35 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_shield_block )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_SHIELD_BLOCK ) )
                return 0;

            skill = EXT_IS_SET( ch->act, ACT_WARRIOR ) ? 35 + ch->level * 2 : 20 + 3 * ch->level / 2;
        }
        else if ( sn == gsn_mighty_blow )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_MIGHTY_BLOW ) )
                return 0;

            skill = ( EXT_IS_SET( ch->act, ACT_BARBARIAN ) || EXT_IS_SET( ch->act, ACT_SHAMAN) ) ? 35 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_power_strike )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_POWER_STRIKE ) )
                return 0;

            skill = EXT_IS_SET( ch->act, ACT_BARBARIAN ) ? 35 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_critical_strike )
        {
            if ( !EXT_IS_SET( ch->off_flags, OFF_CRITICAL_STRIKE ) )
                return 0;

            skill = EXT_IS_SET( ch->act, ACT_BARBARIAN ) ? 35 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_two_weapon_fighting )
        {
            skill = ( EXT_IS_SET( ch->act, ACT_PALADIN ) || EXT_IS_SET( ch->act, ACT_WARRIOR ) ) ? 30 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_dagger_mastery )
        {
            skill = EXT_IS_SET( ch->act, ACT_THIEF ) ? 30 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_twohander_fighting )
        {
            skill = ( EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) || EXT_IS_SET( ch->act, ACT_BARBARIAN ) || EXT_IS_SET( ch->act, ACT_WARRIOR ) ) ? 30 + ch->level * 2 : 0;
        }
        else if ( sn == gsn_target )
        {
            skill = ( EXT_IS_SET( ch->act, ACT_WARRIOR ) || EXT_IS_SET( ch->act, ACT_PALADIN ) ) ? 30 + ch->level * 2 : 10 + 3 * ch->level / 2;
        }
        else if ( sn == gsn_sword ||
                sn == gsn_dagger ||
                sn == gsn_shortsword ||
                sn == gsn_spear ||
                sn == gsn_mace ||
                sn == gsn_axe ||
                sn == gsn_flail ||
                sn == gsn_whip ||
                sn == gsn_polearm ||
                sn == gsn_claws)
        {
            skill = ( EXT_IS_SET( ch->act, ACT_WARRIOR ) || EXT_IS_SET( ch->act, ACT_BARBARIAN ) || EXT_IS_SET( ch->act, ACT_PALADIN ) || EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) ) ? 30 + ch->level * 2 : 10 + 3 * ch->level / 2;
        }
        /**
         * modyfikacja w zaleznosci od wiedzy moba
         */
        skill = URANGE( 25, get_curr_stat( ch, STAT_WIS ), 150 ) * skill / 100;
    }

    if ( skill == 0 )
    {
        return skill;
    }

    /* jesli ma daze to skille spadaja do 1/4 wartosci */
    if ( ch->daze > 0 && skill > 0 )
    {
        skill = UMAX( 1, skill / 4 );
    }
    if ( IS_AFFECTED( ch, AFF_SEAL_OF_ATROCITY ) && skill > 0 )
    {
        AFFECT_DATA *seal;
        for( seal = ch->affected ; seal; seal = seal->next )
        {
            if( seal->type == skill_lookup( "seal of atrocity" ) && seal->bitvector == &AFF_SEAL_OF_ATROCITY )
                skill = UMAX( 1, skill - skill * seal->modifier / 100 );
        }
    }
    return URANGE( 0, skill, 100 );
}

int weapon_sn(OBJ_DATA *weapon)
{
    int sn;

    if(!weapon || weapon->item_type != ITEM_WEAPON)
        return -1;

    switch (weapon->value[0])
    {
        default :               sn = -1;                break;
        case(WEAPON_SWORD):     sn = gsn_sword;         break;
        case(WEAPON_DAGGER):    sn = gsn_dagger;        break;
        case(WEAPON_SPEAR):     sn = gsn_spear;         break;
        case(WEAPON_MACE):      sn = gsn_mace;          break;
        case(WEAPON_AXE):       sn = gsn_axe;           break;
        case(WEAPON_FLAIL):     sn = gsn_flail;         break;
        case(WEAPON_WHIP):      sn = gsn_whip;          break;
        case(WEAPON_POLEARM):   sn = gsn_polearm;       break;
        case(WEAPON_STAFF):     sn = gsn_staff;       	break;
        case(WEAPON_SHORTSWORD):sn = gsn_shortsword;    break;
        case(WEAPON_CLAWS):     sn = gsn_claws;         break;
    }

    return sn;
}

int tool_sn( OBJ_DATA *tool )
{
    int sn;
    switch ( tool->value[2] )
    {
        case TOOL_PICKAXE:
            sn = skill_lookup( "mining" );
            break;
        default:
            sn = -1;
            break;
    }
    return sn;
}

int musical_instrument_sn(OBJ_DATA *musical_instrument)
{
    int sn;

    if(!musical_instrument || musical_instrument->item_type != ITEM_MUSICAL_INSTRUMENT)
        return -1;

    switch (musical_instrument->value[0])
    {
        default :               sn = -1;                break;
        case(INSTRUMENT_OCARINA):     sn = gsn_ocarina;         break ;
        case(INSTRUMENT_FLUTE):    sn = gsn_flute;        break;
        case(INSTRUMENT_LUTE):     sn = gsn_lute;         break;
        case(INSTRUMENT_HARP):      sn = gsn_harp;          break;
        case(INSTRUMENT_MANDOLIN):       sn = gsn_mandolin;           break;
        case(INSTRUMENT_PIANO):     sn = gsn_piano;         break;
    }

    return sn;
}
/* for returning weapon information */
int get_weapon_sn(CHAR_DATA *ch, bool primary)
{
    OBJ_DATA *wield;
    int sn;

    if(primary)
        wield = get_eq_char( ch, WEAR_WIELD );
    else
        wield = get_eq_char( ch, WEAR_SECOND );

    if (wield == NULL || wield->item_type != ITEM_WEAPON)
        return (-1);
    else
        sn = weapon_sn(wield);

    return sn;
}

#ifdef BUGGY_CODE
int get_musical_instrument_sn(CHAR_DATA *ch )
{
    OBJ_DATA *musical_instrument;
    int sn;

    if (musical_instrument == NULL || musical_instrument->item_type != ITEM_MUSICAL_INSTRUMENT )
        return (-1);
    else
        sn = musical_instrument_sn(musical_instrument);

    return sn;
}
#endif

int get_weapon_skill(CHAR_DATA *ch, int sn)
{
    return get_skill(ch, sn);
}

int get_musical_instrument_skill(CHAR_DATA *ch, int sn)
{
    return get_skill(ch, sn);
}

void reset_char_mods(CHAR_DATA *ch)
{
    int i;
    BONUS_DATA * bonus, * next_bonus;

    if (IS_NPC(ch))
        return;
    //hp/move/sex
    ch->sex			= ch->pcdata->true_sex;
    ch->max_hit 	= ch->pcdata->perm_hit;
    ch->max_move	= ch->pcdata->perm_move;

    //statsy
    for (i = 0; i < MAX_STATS; i++)
    {
        ch->new_mod_stat[i] = 0;
    }

    //pancerz
    for (i = 0; i < 4; i++)
        ch->armor[i]	= 100;

    //resisty
    for (i = 0; i < MAX_RESIST; i++)
    {
        ch->resists[i]	= 0;
        ch->healing_from[i] = 0;
    }

    ch->hitroll		= 0;
    ch->damroll		= 0;

    //rzuty obronne
    for (i = 0; i < 5; i++)
        ch->saving_throw[i]	= 0;

    //modyfikator jezykow
    for (i = 0; i < MAX_LANG; i++)
        ch->pcdata->lang_mod[i]	= 0;

    //mod skilli
    for (i = 0; i < MAX_SKILL; i++)
        ch->pcdata->learn_mod[i]	= 0;

    //topienie
    ch->pcdata->oxygen = 0;

    //mod mema
    for (i = 0; i < 9; i++)
        ch->mem_mods[i]	= 0;

    //affecty
    for( i = 0; i < MAX_VECT_BANK; i++)
        ch->affected_by[i] = 0;

    for( bonus = ch->bonus; bonus; bonus = next_bonus )
    {
        next_bonus = bonus->next;
        DISPOSE( bonus );
    }
    ch->bonus = NULL;
}

//eksperymentalny
void reset_char( CHAR_DATA *ch )
{
    int mod;
    OBJ_DATA *obj;
    AFFECT_DATA *af;
    int i;

    if ( IS_NPC( ch ) )
        return ;

    if ( ch->pcdata->perm_hit == 0
            || ch->pcdata->perm_move == 0 )
    {
        log_string("reset_char: perm_hit = 0");
        for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
        {
            if ( !obj || obj->wear_loc == WEAR_NONE )
                continue;

            if ( !obj->enchanted )
                for ( af = obj->pIndexData->affected; af != NULL; af = af->next )
                {
                    mod = af->modifier;

                    switch ( af->location )
                    {
                        case APPLY_SEX: ch->sex	-= mod;
                                        if ( ch->sex < 0 || ch->sex > 2 )
                                            ch->sex = IS_NPC( ch ) ? 0 : ch->pcdata->true_sex;
                                        break;
                        case APPLY_RESIST: ch->resists[ mod ] -= af->level;	break;
                        case APPLY_HIT: ch->max_hit	-= mod;	break;
                        case APPLY_MOVE: ch->max_move	-= mod;	break;
                    }
                }

            for ( af = obj->affected; af != NULL; af = af->next )
            {
                mod = af->modifier;

                switch ( af->location )
                {
                    case APPLY_SEX:
                        ch->sex -= mod;
                        if ( ch->sex < 0 || ch->sex > 2 )
                            ch->sex = IS_NPC( ch ) ? 0 : ch->pcdata->true_sex;
                        break;
                    case APPLY_RESIST: ch->resists[ mod ] -= af->level;	break;
                    case APPLY_HIT: ch->max_hit -= mod;	break;
                    case APPLY_MOVE: ch->max_move -= mod;	break;
                }
            }
        }

        /* now reset the permanent stats */
        ch->pcdata->perm_hit 	= ch->max_hit;
        ch->pcdata->perm_move	= ch->max_move;
        ch->pcdata->last_level	= ch->played / 3600;

        if ( ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2 )
        {
            if ( ch->sex > 0 && ch->sex < 3 )
                ch->pcdata->true_sex = ch->sex;
            else
                ch->pcdata->true_sex = 0;
        }
    }

    reset_char_mods( ch );

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
        if ( !obj || obj->wear_loc == WEAR_NONE )
            continue;

        for ( i = 0; i < 4; i++ )
            ch->armor[ i ] -= apply_ac( obj, obj->wear_loc, i );

        if ( !obj->enchanted )
            for ( af = obj->pIndexData->affected; af != NULL; af = af->next )
            {
                mod = af->modifier;

                switch ( af->location )
                {
                    case APPLY_EXP:
                    case APPLY_WEIGHT:
                    case APPLY_HEIGHT:
                    case APPLY_LEVEL:
                    case APPLY_CLASS:
                    case APPLY_AGE:
                    case APPLY_GOLD:
                        continue;
                    default: break;
                }
                affect_modify( ch, af, TRUE );
            }

        for ( af = obj->affected; af != NULL; af = af->next )
        {
            mod = af->modifier;

            switch ( af->location )
            {
                case APPLY_EXP:
                case APPLY_WEIGHT:
                case APPLY_HEIGHT:
                case APPLY_LEVEL:
                case APPLY_CLASS:
                case APPLY_AGE:
                case APPLY_GOLD:
                    continue;
                default: break;
            }

            affect_modify( ch, af, TRUE );
        }

        if ( obj->pIndexData && obj->pIndexData->bonus_set > 0 )
        {
            BONUS_INDEX_DATA * bonus_index;
            BONUS_DATA *bonus;
            OBJ_INDEX_DATA *pObj;
            OBJ_DATA *wear;
            AREA_DATA *pArea;
            int vnum;
            bool eq_check = FALSE;
            bool cont = FALSE;

            bonus_index = get_bonus_index( obj->pIndexData->bonus_set );
            pArea = obj->pIndexData->area;

            if ( !bonus_index )
                continue;

            for ( bonus = ch->bonus; bonus; bonus = bonus->next )
            {
                if ( bonus->index_data->vnum == bonus_index->vnum )
                {
                    cont = TRUE;
                    break;
                }
            }

            if ( cont )
                continue;

            /* sprawdzenie czy ma wszystko */
            for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
            {
                pObj = get_obj_index( vnum );

                if ( !pObj || pObj->bonus_set <= 0 || pObj->bonus_set != obj->pIndexData->bonus_set )
                    continue;

                eq_check = FALSE;

                for ( wear = ch->carrying; wear; wear = wear->next_content )
                {
                    if ( wear->wear_loc == WEAR_NONE )
                        continue;

                    if ( wear->pIndexData == pObj )
                        eq_check = TRUE;
                }

                if ( !eq_check )
                    break;
            }

            if ( !eq_check )
                continue;

            CREATE( bonus, BONUS_DATA, 1 );

            bonus->index_data = bonus_index;
            bonus->next       = ch->bonus;
            ch->bonus         = bonus;

            for ( af = bonus_index->affects; af != NULL; af = af->next )
            {
                switch ( af->location )
                {
                    case APPLY_EXP:
                    case APPLY_WEIGHT:
                    case APPLY_HEIGHT:
                    case APPLY_LEVEL:
                    case APPLY_CLASS:
                    case APPLY_AGE:
                    case APPLY_GOLD:
                    case APPLY_SEX:
                    case APPLY_SPELL_AFFECT:
                        continue;
                    default: break;
                }

                affect_modify( ch, af, TRUE );
            }
        }
    }

    /* now add back spell effects */
    for ( af = ch->affected; af != NULL; af = af->next )
    {
        mod = af->modifier;

        switch ( af->location )
        {
            case APPLY_EXP:
            case APPLY_WEIGHT:
            case APPLY_HEIGHT:
            case APPLY_LEVEL:
            case APPLY_CLASS:
            case APPLY_AGE:
            case APPLY_GOLD:
                continue;
            default: break;
        }

        affect_modify( ch, af, TRUE );

    }


    /* make sure sex is RIGHT!!!! */
    if ( ch->sex < 0 || ch->sex > 2 )
        ch->sex = ch->pcdata->true_sex;

    //ustawienie rasowych affectow
    ext_flags_copy( ext_flags_sum( ch->affected_by, ext_flag_value( affect_flags, race_table[ GET_RACE( ch ) ].aff ) ), ch->affected_by );
    return ;
}

/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust( CHAR_DATA *ch )
{
    if ( ch->desc != NULL && ch->desc->original != NULL )
        ch = ch->desc->original;

    if (ch->trust)
        return ch->trust;

    if ( IS_NPC(ch) && ch->level >= LEVEL_HERO )
        return LEVEL_HERO - 1;
    else
        return ch->level;
}

/*
 * Retrieve a character's age.
 */
int get_age( CHAR_DATA *ch )
{
    if (ch->age == 0)
        ch->age = number_range ( pc_race_table[ch->real_race].age[ 0 ], pc_race_table[ch->real_race].age[ 1 ]);
    return ch->age + ( (int) (ch->played + current_time - ch->logon) ) / 856800;
}

/* command for retrieving stats */
int get_curr_stat( CHAR_DATA *ch, int stat )
{
    return URANGE(1, ch->perm_stat[stat] + ch->new_mod_stat[stat], MAX_STAT_VALUE);
}
int get_curr_stat_deprecated( CHAR_DATA *ch, int stat )
{
    return URANGE( 3, get_curr_stat ( ch, stat ) / 6, 28 );
}

/*
 * Retrieve a character's carry capacity.
 */
int can_carry_n( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
    {
        return 1000;
    }

    return MAX_WEAR + 12 + (get_curr_stat_deprecated(ch,STAT_DEX)/3);
}


/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
    {
        return 10000000;
    }

    return 10*str_app[get_curr_stat(ch,STAT_STR)].carry;
}


/*
 * See if a string is one of the names of an object.
 */

bool is_name ( char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH], part[MAX_INPUT_LENGTH];
    char *list, *string;

    /* fix crash on NULL namelist */
    if (namelist == NULL || namelist[0] == '\0') return FALSE;

    /* fixed to prevent is_name on "" returning TRUE */
    if (str[0] == '\0')	return FALSE;

    string = str;
    /* we need ALL parts of string to match part of namelist */
    for ( ; ; )  /* start parsing string */
    {
        str = one_argument(str,part);

        if (part[0] == '\0' ) return TRUE;

        /* check to see if this is part of namelist */
        list = namelist;
        for ( ; ; )  /* start parsing namelist */
        {
            list = one_argument(list,name);
            if (name[0] == '\0')  /* this name was not found */
                return FALSE;

            if (!str_prefix(string,name))
                return TRUE; /* full pattern match */

            if (!str_prefix(part,name))
                break;
        }
    }
}

bool is_exact_name(char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH];

    if (namelist == NULL)
        return FALSE;

    for ( ; ; )
    {
        namelist = one_argument( namelist, name );
        if ( name[0] == '\0' )
            return FALSE;
        if ( !str_cmp( str, name ) )
            return TRUE;
    }
}

/* enchanted stuff for eq */
void affect_enchant(OBJ_DATA *obj)
{
    /* okay, move all the old flags into new vectors if we have to */
    if (!obj->enchanted)
    {
        AFFECT_DATA *paf, *af_new;
        obj->enchanted = TRUE;

        for (paf = obj->pIndexData->affected;
                paf != NULL; paf = paf->next)
        {
            af_new = new_affect();

            af_new->next = obj->affected;
            obj->affected = af_new;

            af_new->where	= paf->where;
            af_new->type        = UMAX(0,paf->type);
            af_new->level       = paf->level;
            af_new->duration    = paf->duration;
            af_new->rt_duration = paf->rt_duration;
            af_new->location    = paf->location;
            af_new->modifier    = paf->modifier;
            af_new->bitvector   = paf->bitvector;
        }
    }
}

/*
 * Apply or remove an affect to a character.
 */
void affect_modify( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd )
{
    OBJ_DATA * wield;
    int mod, i;

    mod = paf->modifier;

    if ( fAdd )
    {
        switch ( paf->where )
        {
            case TO_AFFECTS:
                if ( paf->bitvector )
                {
                    EXT_SET_BIT( ch->affected_by, *paf->bitvector );
                }
                break;
        }
    }
    else
    {
        switch ( paf->where )
        {
            case TO_AFFECTS:
                if ( paf->bitvector )
                    EXT_REMOVE_BIT( ch->affected_by, *paf->bitvector );
                break;
        }
        mod = 0 - mod;
    }

    switch ( paf->location )
    {
        default:
            bug( "Affect_modify: unknown location %d.", paf->location );
            return ;

        case APPLY_NONE:
        case APPLY_CLASS:
        case APPLY_LEVEL:
        case APPLY_EXP:
        case APPLY_AGE:
        case APPLY_GOLD:
        case APPLY_SPELL_AFFECT:
            break;

        case APPLY_STR:
            ch->new_mod_stat[ STAT_STR ] += mod;
            break;

        case APPLY_CHA:
            ch->new_mod_stat[ STAT_CHA ] += mod;
            break;

        case APPLY_LUC:
            ch->new_mod_stat[ STAT_LUC ] += mod;
            break;

        case APPLY_DEX:
            ch->new_mod_stat[ STAT_DEX ] += mod;
            break;

        case APPLY_INT:
            ch->new_mod_stat[ STAT_INT ] += mod;
            break;

        case APPLY_WIS:
            ch->new_mod_stat[ STAT_WIS ] += mod;
            break;

        case APPLY_CON:
            ch->new_mod_stat[ STAT_CON ] += mod;
            break;

        case APPLY_SEX:
            ch->sex += mod;
            break;

        case APPLY_HEIGHT:
            ch->height	+= mod;
            break;

        case APPLY_WEIGHT:
            ch->weight	+= mod;
            print_char( ch, "Waga: %d + %d\n\r", ch->weight, mod );
            break;

        case APPLY_HIT:
            ch->max_hit	+= mod;
            if ( paf->type == gsn_berserk && mod < 0 )
            {
                ch->hit = UMAX( 1, ch->hit + mod );
            }
            if ( paf->type == gsn_divine_power && mod < 0 )
            {
                if ( number_range( 20, 280 ) > get_curr_stat_deprecated( ch, STAT_WIS ) * 10 )
                {
                    ch->hit = UMAX( 1, ch->hit + mod );
                }
                else
                {
                    ch->hit = UMIN( ch->hit, get_max_hp(ch) );
                }
            }

            break;

        case APPLY_MOVE:
            ch->max_move	+= mod;
            break;

        case APPLY_AC:
            for ( i = 0; i < 4; i ++ )
                ch->armor[ i ] += mod;
            break;

        case APPLY_HITROLL:
            ch->hitroll	+= mod;
            break;

        case APPLY_DAMROLL:
            ch->damroll	+= mod;
            break;

        case APPLY_SAVING_DEATH:
            ch->saving_throw[ 0 ] -= mod;
            break;

        case APPLY_SAVING_ROD:
            ch->saving_throw[ 1 ] -= mod;
            break;

        case APPLY_SAVING_PETRI:
            ch->saving_throw[ 2 ] -= mod;
            break;

        case APPLY_SAVING_BREATH:
            ch->saving_throw[ 3 ] -= mod;
            break;

        case APPLY_SAVING_SPELL:
            ch->saving_throw[ 4 ] -= mod;
            break;

        case APPLY_MEMMING:
            if ( fAdd )
            {
                ch->mem_mods[ paf->modifier - 1 ] += paf->level;
            }
            else
            {
                ch->mem_mods[ paf->modifier - 1 ] -= paf->level;
            }
            check_max_spells_memmed( ch );
            break;

        case APPLY_RESIST:
            if ( fAdd )
            {
                ch->resists[ paf->modifier ] += paf->level;
            }
            else
            {
                ch->resists[ paf->modifier ] -= paf->level;
            }
            break;

        case APPLY_LANG:
            if ( !ch->pcdata )
            {
                break;
            }

            if ( fAdd )
            {
                ch->pcdata->lang_mod[ paf->modifier ] += paf->level;
            }
            else
            {
                ch->pcdata->lang_mod[ paf->modifier ] -= paf->level;
            }
            break;

        case APPLY_SKILL:
            if ( !ch->pcdata )
            {
                break;
            }
            if ( fAdd )
            {
                ch->pcdata->learn_mod[ paf->modifier ] += paf->level;
            }
            else
            {
                ch->pcdata->learn_mod[ paf->modifier ] -= paf->level;
            }
            break;
    }

    /*
     * Check for weapon wielding.
     * Guard against recursion (for weapons with affects).
     */
    if
        (
         !IS_NPC( ch )
         && ( wield = get_eq_char( ch, WEAR_WIELD ) ) != NULL
         && check_only_weight_cant_equip( ch, wield )
         && !IS_OBJ_STAT( wield, ITEM_NOREMOVE )
        )
        /*   get_obj_weight(wield) > (str_app[get_curr_stat(ch,STAT_STR)].wield*10))*/
    {
        static int depth;

        if ( depth == 0 )
        {
            depth++;
            act( "Upuszczasz $h.", ch, wield, NULL, TO_CHAR );
            act( "$n upuszcza $h.", ch, wield, NULL, TO_ROOM );

            obj_from_char( wield );

            if( IS_OBJ_STAT(wield, ITEM_NODROP ) )
            {
                obj_to_char( wield, ch );
            }
            else
            {
                if ( is_artefact( wield ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
                {
                    artefact_from_char( wield, ch );
                }
                obj_to_room( wield, ch->in_room );
            }

            depth--;
        }
    }

    return ;
}

/* find an effect in an affect list */
AFFECT_DATA  *affect_find(AFFECT_DATA *paf, int sn)
{
    AFFECT_DATA *paf_find;

    for ( paf_find = paf; paf_find != NULL; paf_find = paf_find->next )
    {
        if ( paf_find->type == sn )
            return paf_find;
    }

    return NULL;
}

/* fix object affects when removing one */
void affect_check( CHAR_DATA *ch, int where, BITVECT_DATA * vector )
{
    AFFECT_DATA * paf;
    OBJ_DATA *obj;

    if ( where == TO_OBJECT || where == TO_WEAPON )
        return ;

    ext_flags_copy( ext_flags_sum( ch->affected_by, ext_flag_value( affect_flags, race_table[ GET_RACE( ch ) ].aff ) ), ch->affected_by );

    for ( paf = ch->affected; paf != NULL; paf = paf->next )
        if ( paf->where == where && paf->bitvector == vector )
        {
            switch ( where )
            {
                case TO_AFFECTS:
                    if ( vector )
                        EXT_SET_BIT( ch->affected_by, *vector );
                    break;
            }
            return ;
        }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
        if ( obj->wear_loc == -1 )
            continue;

        for ( paf = obj->affected; paf != NULL; paf = paf->next )
            if ( paf->where == where && paf->bitvector == vector )
            {
                switch ( where )
                {
                    case TO_AFFECTS:
                        if ( vector )
                            EXT_SET_BIT( ch->affected_by, *vector );
                        break;
                }
                return ;
            }

        if ( obj->enchanted )
            continue;

        for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
            if ( paf->where == where && paf->bitvector == vector )
            {
                switch ( where )
                {
                    case TO_AFFECTS:
                        if ( vector )
                            EXT_SET_BIT( ch->affected_by, *vector );
                        break;
                }
                return ;
            }
    }
}

/*
 * Give an affect to a char.
 */
void affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf, char * info, bool visible )
{
    AFFECT_DATA * paf_new;

    paf_new = new_affect();
    *paf_new	= *paf;
    paf_new->visible	= visible;

    if ( !paf_new->bitvector )
        paf_new->bitvector = &AFF_NONE;

    if ( info )
        paf_new->info	= str_dup( info );
    else
        paf_new->info	= NULL;

    VALIDATE( paf );	/* in case we missed it when we set up paf */
    paf_new->next	= ch->affected;
    ch->affected	= paf_new;

    affect_modify( ch, paf_new, TRUE );
    return ;
}

/* give an affect to an object */
void affect_to_obj( OBJ_DATA *obj, AFFECT_DATA *paf )
{
    AFFECT_DATA * paf_new;

    paf_new = new_affect();

    *paf_new	= *paf;
    paf_new->info = NULL;

    if ( !paf_new->bitvector )
        paf_new->bitvector = &AFF_NONE;

    VALIDATE( paf );	/* in case we missed it when we set up paf */
    paf_new->next	= obj->affected;
    obj->affected	= paf_new;

    /* apply any affect vectors to the object's extra_flags */

    switch ( paf->where )
    {
        case TO_OBJECT:
            if ( paf->bitvector )
                EXT_SET_BIT( obj->extra_flags, *paf->bitvector );
            break;
        case TO_WEAPON:
            if ( obj->item_type == ITEM_WEAPON )
                SET_BIT( obj->value[ 4 ], paf->location );
            break;
    }

    return ;
}

/*
 * Remove an affect from a char.
 */
void affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    int where;
    BITVECT_DATA * vector;

    if ( ch->affected == NULL )
    {
        bug( "Affect_remove: no affect.", 0 );
        return ;
    }

    if ( ch->in_room )
    {
        if ( paf->bitvector == &AFF_ETHEREAL_ARMOR )
        {
            if (IS_IMMORTAL( ch ))
            {
                print_char ( ch, "Eteryczny pancerz znika.\n\r" );
            }
            else
            {
                int dam;
                dam = UMIN ( ch->hit, dice ( 3, 6 ) );
                print_char ( ch, "Czujesz potworny ból, kiedy eteryczny pancerz znika.\n\r" );
                act ( "$n skrzywia siê w potwornym bólu.", ch, NULL, NULL, TO_ROOM );
                raw_damage ( ch, ch, dam );
            }
        }
        else if ( !IS_NPC ( ch ) && paf->type == gsn_healing_sleep &&
                ch->position == POS_SLEEPING )
        {
            int heal;
            int duration;

            /* duration * 15% do duration * 25% max_hit regeneruje */
            duration = 3 - paf->duration;
            if (duration > 0)
            {
                heal = ( number_range ( duration * 15, duration * 25 ) * get_max_hp( ch ) ) / 100;
                ch->hit = UMIN ( ch->hit + heal, get_max_hp( ch ) );
            }
        }
        else if ( !IS_NPC ( ch ) && paf->type == gsn_energize && paf->level > 0 )
        {
            AFFECT_DATA af;

            af.where = TO_AFFECTS;
            af.type = gsn_energize;
            af.level = 0;
            af.duration = 3;
            af.rt_duration = 0;
            af.location = APPLY_STR;
            af.modifier = -5;
            af.bitvector = &AFF_ENERGIZE;
            affect_to_char ( ch, &af, "zmêczenie po zaklêciu 'energize'", TRUE );

            af.location = APPLY_DEX;
            affect_to_char ( ch, &af, NULL, FALSE );
            //Raszer: zmiana efektu po spadnieciu energize dla nieumarlych,slow i weaken zamiast zmeczenia i sennosci dla undeadow
            if(is_undead(ch))
            {
                af.where = TO_AFFECTS;
                af.type = skill_lookup( "weaken" );
                af.level = 15;
                af.duration = 4;
                af.rt_duration = 0;
                af.location = APPLY_STR;
                af.modifier = -1;
                af.bitvector = &AFF_WEAKEN;
                affect_to_char( ch, &af, NULL, TRUE );

                af.where = TO_AFFECTS;
                af.type = skill_lookup( "slow");
                af.level = 15;
                af.duration = 4; af.rt_duration = 0;
                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector = &AFF_SLOW;
                affect_to_char( ch, &af, NULL, TRUE );

            }else
            {
                ch->condition[ COND_SLEEPY ] = 0;
            }
            send_to_char ( "Czujesz jak zmêczenie powraca ze zdwojon± si³±.\n\r", ch );
            if ( ch->sex == 0 )
            {
                act ( "$n wygl±da tak, jakby siê mia³o przewróciæ.", ch, NULL, NULL, TO_ROOM );
            }
            else if ( ch->sex == 1 )
            {
                act ( "$n wygl±da tak, jakby siê mia³ przewróciæ.", ch, NULL, NULL, TO_ROOM );
            }
            else
            {
                act ( "$n wygl±da tak, jakby sie mia³a przewróciæ.", ch, NULL, NULL, TO_ROOM );
            }
        }
        else if ( !IS_NPC ( ch ) && paf->type == gsn_energize && paf->level <= 0 )
        {
            if ( ch->sex == 0 )
            {
                send_to_char ( "Czujesz siê mniej zmêczone.\n\r", ch );
            }
            else if ( ch->sex == 1 )
            {
                send_to_char ( "Czujesz siê mniej zmêczony.\n\r", ch );
            }
            else
            {
                send_to_char ( "Czujesz siê mniej zmêczona.\n\r", ch );
            }
        }
        else if ( paf->type == gsn_astral_journey )
        {
            if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
                mind_move_room( ch, NULL );
        }

    }

    affect_modify( ch, paf, FALSE );

    where = paf->where;

    vector = paf->bitvector;

    if ( paf == ch->affected )
    {
        ch->affected	= paf->next;
    }
    else
    {
        AFFECT_DATA *prev;

        for ( prev = ch->affected; prev != NULL; prev = prev->next )
        {
            if ( prev->next == paf )
            {
                prev->next = paf->next;
                break;
            }
        }

        if ( prev == NULL )
        {
            bug( "Affect_remove: cannot find paf.", 0 );
            return ;
        }
    }

    if ( paf->bitvector == &AFF_CHARM )
    {
        if ( ch->master && !IS_NPC( ch->master ) )
            remove_charm( ch->master, ch );

        ch->master = NULL;
        ch->leader = NULL;
    }

    if ( paf->bitvector == &AFF_COMPREHEND_LANGUAGES )
    {
        ch->speaking = 0;
    }


    if ( paf->bitvector == &AFF_HIDE )
    {
        if ( IS_AFFECTED(ch, AFF_HIDE) )
        {
            EXT_REMOVE_BIT( ch->affected_by, AFF_HIDE );
        }
    }

    free_affect( paf );
    affect_check( ch, where, vector );
    return ;
}

void affect_remove_obj( OBJ_DATA *obj, AFFECT_DATA *paf )
{
    int where;
    BITVECT_DATA * vector;

    if ( obj->affected == NULL )
    {
        bug( "Affect_remove_object: no affect.", 0 );
        return;
    }

    if (obj->carried_by != NULL && obj->wear_loc != -1)
        affect_modify( obj->carried_by, paf, FALSE );

    where = paf->where;
    vector = paf->bitvector;

    /* remove flags from the object if needed */
    if ( paf->bitvector )
    {
        switch( paf->where)
        {
            case TO_OBJECT:
                EXT_REMOVE_BIT( obj->extra_flags, *paf->bitvector );
                break;
            case TO_WEAPON:
                if ( obj->item_type == ITEM_WEAPON )
                    REMOVE_BIT( obj->value[4], paf->location );
                break;
        }
    }

    /*	if ( paf->type == gsn_bladethirst )
        obj->value[ 5 ] -= paf->modifier; */

    if ( paf == obj->affected )
    {
        obj->affected    = paf->next;
    }
    else
    {
        AFFECT_DATA *prev;

        for ( prev = obj->affected; prev != NULL; prev = prev->next )
        {
            if ( prev->next == paf )
            {
                prev->next = paf->next;
                break;
            }
        }

        if ( prev == NULL )
        {
            bug( "Affect_remove_object: cannot find paf.", 0 );
            return;
        }
    }

    free_affect(paf);

    if ( obj->carried_by != NULL && obj->wear_loc != -1 )
        affect_check( obj->carried_by, where, vector );
    return;
}


/*
 * Strip all affects of a given sn.
 */
void affect_strip( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
        paf_next = paf->next;
        if ( paf->type == sn )
        {
            affect_remove( ch, paf );
        }
    }

    return;
}

//¶ci±ganie czê¶ci affectów - wg. flag w extra_flags
void affect_strip_partial( CHAR_DATA *ch, int sn, int flags )
{
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
        paf_next = paf->next;
        if ( paf->type == sn )
        {
            paf->extra_flags &= ~flags; //wy³±czanie flag (z zanegowan± mask±)
            //je¶li wszystkie kawa³ki zdjête, to wywalenie affecta
            if(paf->extra_flags == 0)
                affect_remove( ch, paf );
        }
    }

    return;
}
/*
 * Strip all invis
 */
void strip_invis( CHAR_DATA *ch, bool show_message, bool strip_nondetection_spell )
{
    if ( IS_AFFECTED( ch, AFF_INVISIBLE ) || IS_AFFECTED( ch, AFF_UNDEAD_INVIS ) || IS_AFFECTED( ch, AFF_ANIMAL_INVIS ) )
    {
        affect_strip( ch, gsn_invis );
        affect_strip( ch, gsn_mass_invis );
        affect_strip( ch, gsn_animal_invis );
        affect_strip( ch, gsn_undead_invis );
        EXT_REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
        EXT_REMOVE_BIT( ch->affected_by, AFF_UNDEAD_INVIS );
        EXT_REMOVE_BIT( ch->affected_by, AFF_ANIMAL_INVIS );
        if (strip_nondetection_spell)
        {
            EXT_REMOVE_BIT( ch->affected_by, AFF_NONDETECTION );
        }
        if ( show_message )
        {
            act( "$n materializuje sie.", ch, NULL, NULL, TO_ROOM );
            send_to_char( "Przestajesz byæ niewidzialn<&y/a/e>.\n\r", ch );
        }
    }
    return;
}

/*
 * Return true if a char is affected by a spell.
 */
bool is_affected( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;

    DEBUG_INFO("is_artefact: enter");
    for ( paf = ch->affected; paf != NULL; paf = paf->next )
    {
        DEBUG_INFO("is_artefact: for");
        if ( paf->type == sn )
        {
            DEBUG_INFO("is_artefact: if");
            return TRUE;
        }
    }

    return FALSE;
}

//rellik: kary za zgon, sprawdzanie czy jest w szoku po¶miertnym
int after_death( CHAR_DATA *ch )
{
    AFFECT_DATA *paf;
    DEBUG_INFO("after_death");
    if ( ( paf = affect_find( ch->affected, gsn_playerdeath ) ) )
    {
        return paf->level;
    }

    return 0;
}

/*
 * Add or enhance an affect.
 */
void affect_join( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_old;
    bool found;
    bool visible=FALSE;
    char *info=NULL;

    found = FALSE;
    for ( paf_old = ch->affected; paf_old != NULL; paf_old = paf_old->next )
    {
        if ( paf_old->type == paf->type )
        {
            paf->level = (paf->level += paf_old->level) / 2;
            paf->duration += paf_old->duration;
            paf->rt_duration += paf_old->rt_duration;
            paf->modifier += paf_old->modifier;
            visible 	   = paf_old->visible;

            if(paf_old->info)
                info = str_dup(paf_old->info);

            affect_remove( ch, paf_old );
            break;
        }
    }

    affect_to_char( ch, paf, info, visible );
    return;
}

/*
 * do czarku mistrza poznania, mo¿na room podaæ NULL (przy zakonczeniu dzialania czaru)
 */
void char_from_room( CHAR_DATA * ch )
{
    OBJ_DATA * obj;
    CHAR_DATA *mount = NULL;

    //je¶li cia³o wychodzi z rooma tzn ze dzieje siê co¶ co przerywa dzia³anie czaru
    mind_move_room( ch, NULL );

    if ( ch->in_room == NULL )
    {
        bug( "Char_from_room: NULL.", 0 );
        return ;
    }

    if ( ch->mount )
    {
        mount = ch->mount;

        if ( mount && ( !mount->in_room || mount->in_room != ch->in_room ) )
        {
            ch->mount = NULL;
            ch->position = POS_STANDING;
            mount = NULL;
        }

        if ( mount && ch != mount->mounting )
            bug( "Mount system error!!!", 0 );
    }

    if ( !IS_NPC( ch ) )
        --ch->in_room->area->nplayer;

    /* to tu */
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
        if ( obj->wear_loc == WEAR_NONE )
            continue;

        if ( ( obj->item_type == ITEM_LIGHT )
                && obj->value[ 2 ] != 0
                && ch->in_room->light > 0 )
            --ch->in_room->light;
    }

    if ( ch == ch->in_room->people )
    {
        ch->in_room->people = ch->next_in_room;
    }
    else
    {
        CHAR_DATA *prev;

        for ( prev = ch->in_room->people; prev; prev = prev->next_in_room )
        {
            if ( prev->next_in_room == ch )
            {
                prev->next_in_room = ch->next_in_room;
                break;
            }
        }
    }

    if ( mount )
    {
        if ( mount == mount->in_room->people )
            mount->in_room->people = mount->next_in_room;
        else
        {
            CHAR_DATA *prev;

            for ( prev = mount->in_room->people; prev; prev = prev->next_in_room )
            {
                if ( prev->next_in_room == mount )
                {
                    prev->next_in_room = mount->next_in_room;
                    break;
                }
            }
        }
    }

    if ( mount )
    {
        mount->in_room = NULL;
        mount->next_in_room = NULL;
        mount->on = NULL;
    }

    ch->previous_room = ch->in_room;
    ch->in_room = NULL;
    ch->next_in_room = NULL;
    ch->on = NULL;
    return ;
}

void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    OBJ_DATA * obj;
    CHAR_DATA *mount = NULL;

    if ( ch->mount )
        mount = ch->mount;

    if ( mount && ch != mount->mounting )
    {
        ch->mount = NULL;
        bug( "Mount system sie pierdoli!", 0 );
    }

#ifdef ENABLE_NEW_TRACK
    save_track_data(ch, pRoomIndex, ch->previous_room, 1);
#endif

    if ( pRoomIndex == NULL )
    {
        ROOM_INDEX_DATA * room;

        if ( ( room = get_room_index( ROOM_VNUM_TEMPLE ) ) != NULL )
            char_to_room( ch, room );

        if ( mount )
            char_to_room( mount, room );

        return ;
    }

    ch->in_room	= pRoomIndex;
    ch->next_in_room	= pRoomIndex->people;
    pRoomIndex->people	= ch;

    ch->precommand_pending = FALSE;
    ch->precommand_fun = NULL;
    ch->precommand_arg = NULL;

    if ( mount )
    {
        mount->in_room	= pRoomIndex;
        mount->next_in_room = pRoomIndex->people;
        pRoomIndex->people = mount;
    }

    if ( !IS_NPC( ch ) )
    {
        if ( ch->in_room->area->empty )
        {
            ch->in_room->area->empty = FALSE;
            ch->in_room->area->age = 0;
        }
        ++ch->in_room->area->nplayer;
    }

    /* to tu */
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
        if ( obj->wear_loc == WEAR_NONE || obj->item_type != ITEM_LIGHT )
            continue;

        if ( !IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ) )
        {
            if ( obj->value[ 2 ] != 0 )
                ++ch->in_room->light;
        }

        if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ) )
        {
            if ( obj->value[ 0 ] == 1 )
                ++ch->in_room->light;
            else
            {
                if ( obj->pIndexData->vnum == OBJ_VNUM_FIREFLIES )
                {
                    act( "Nagle otaczaj±ce ciê ¶wietliki rozlatuj± siê na wszystkie strony, opuszczaj±c ciê.", ch, obj, NULL, TO_CHAR );
                    act( "Nagle otaczaj±ce $c ¶wietliki rozlatuj± siê na wszystkie strony.", ch, obj, NULL, TO_NOTVICT );
                    if ( is_artefact( obj ) ) extract_artefact( obj );
                    if ( obj->contains ) extract_artefact_container( obj );

                    extract_obj ( obj );
                }
                else if ( obj->value[ 2 ] != 0 )
                {
                    obj->value[ 2 ] = 0;
                    send_to_char( "Noszone przez ciebie w rêku ¼ród³o ¶wiat³a ga¶nie z lekkim sykiem.\n\r", ch );
                    act( "¬ród³o ¶wiat³a noszone przez $c w rêku ga¶nie z lekkim sykiem.", ch, NULL, NULL, TO_ROOM );
                }
            }
        }

    }

    for ( obj = ch->carrying; obj; obj = obj->next_content )
    {
        if ( obj->item_type != ITEM_PIPE )
            continue;

        if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ) )
        {
            if ( obj->value[ 0 ] == 1 )
            {
                obj->value[ 0 ] = 0;
                obj->value[ 2 ] = 4;
                act( "Twoja $p ga¶nie z lekkim sykiem.", ch, obj, NULL, TO_CHAR );
                act( "Wygl±da na to, ¿e $p $c ga¶nie z lekkim sykiem.", ch, obj, NULL, TO_ROOM );
            }

        }

    }

    if ( IS_AFFECTED( ch, AFF_PLAGUE ) )
    {
        AFFECT_DATA * af, plague;
        CHAR_DATA *vch;

        for ( af = ch->affected; af != NULL; af = af->next )
        {
            if ( af->type == gsn_plague )
                break;
        }

        if ( af == NULL )
        {
            EXT_REMOVE_BIT( ch->affected_by, AFF_PLAGUE );
            return ;
        }

        if ( af->level == 1 )
            return ;

        plague.where	= TO_AFFECTS;
        plague.type = gsn_plague;
        plague.level = af->level - 1;
        plague.duration = number_range( 1, 2 * plague.level ); plague.rt_duration = 0;
        plague.location	= APPLY_STR;
        plague.modifier = -5;
        plague.bitvector = &AFF_PLAGUE;

        for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
        {
            if ( !saves_spell( plague.level - 2, vch, DAM_DISEASE )
                    && !IS_IMMORTAL( vch ) &&
                    !IS_AFFECTED( vch, AFF_PLAGUE ) && number_bits( 6 ) == 0 )
            {
                send_to_char( "Czujesz siê s³abo i zaczynasz mieæ dreszcze.\n\r", vch );
                act( "$n trzêsiê siê jak osika i zaczyna wygl±daæ do¶æ nieciekawie.", vch, NULL, NULL, TO_ROOM );
                affect_join( vch, &plague );
            }
        }
    }

    if ( !IS_NPC( ch ) && ch->pcdata->oxygen > 0 &&
            !IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ) )
    {
        ch->pcdata->oxygen = 0;
        send_to_char( "Z ulg± wci±gasz powietrze do p³uc.\n\r", ch );
        act( "$n przystaje na chwilê i g³êboko wci±ga powietrze do p³uc.", ch, NULL, NULL, TO_ROOM );
    }
    return ;
}

/*
 * Give an obj to a char.
 */
void obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
    obj->next_content	 = ch->carrying;
    ch->carrying	 = obj;
    obj->carried_by	 = ch;
    obj->in_room	 = NULL;
    obj->in_obj		 = NULL;
    ch->carry_number	+= get_obj_number( obj );
    ch->carry_weight	+= get_obj_weight( obj );
    obj->on_ground 	 = 0;
}

/*
 * Take an obj from its character.
 */
void obj_from_char( OBJ_DATA *obj )
{
    CHAR_DATA *ch;

    if ( ( ch = obj->carried_by ) == NULL )
    {
        bug( "Obj_from_char: null ch.", 0 );
        return;
    }

    if ( obj->wear_loc != WEAR_NONE )
        unequip_char( ch, obj );

    if ( ch->carrying == obj )
    {
        ch->carrying = obj->next_content;
    }
    else
    {
        OBJ_DATA *prev;

        for ( prev = ch->carrying; prev != NULL; prev = prev->next_content )
        {
            if ( prev->next_content == obj )
            {
                prev->next_content = obj->next_content;
                break;
            }
        }

        if ( prev == NULL )
            bug( "Obj_from_char: obj not in list.", 0 );
    }

    obj->carried_by	 = NULL;
    obj->next_content	 = NULL;
    ch->carry_number	-= get_obj_number( obj );
    ch->carry_weight	-= get_obj_weight( obj );

    if( IS_OBJ_STAT( obj, ITEM_DOUBLE_GRIP ) && obj->item_type == ITEM_WEAPON )
    {
        REMOVE_BIT( obj->value[4], WEAPON_TWO_HANDS );
        EXT_REMOVE_BIT( obj->extra_flags, ITEM_DOUBLE_GRIP );
    }

    return;
}


/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac( OBJ_DATA *obj, int iWear, int type )
{
    int armor;

    if(obj->item_type != ITEM_ARMOR)
        return 0;

    /*ok tu zmieniamy :)*/
    /*bedzie normalnie */
    armor=100-(10*obj->value[type]);

    switch ( iWear )
    {
        case WEAR_BODY:   return ((armor*4)/10);
        case WEAR_HEAD:   return (armor/10);
        case WEAR_LEGS:   return ((armor*2)/10);
        case WEAR_FEET:   return (armor/20);
        case WEAR_HANDS:  return (armor/20);
        case WEAR_ARMS:   return ((armor*2)/10);
        case WEAR_SHIELD: return 10; /*tarzcza daje +1 ac*/
    }

    return 0;
}


/*
 * Find a piece of eq on a character.
 */
OBJ_DATA *get_eq_char( CHAR_DATA *ch, int iWear )
{
    OBJ_DATA *obj;

    if (ch == NULL)
        return NULL;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
        if ( obj->wear_loc == iWear )
            return obj;
    }

    return NULL;
}

/*
 * Check if char can equip object.
 */
bool can_equip_obj( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
    int sn;
    int race, class, specka;
    char buf[MAX_STRING_LENGTH];

    race = GET_RACE( ch );

    if ( iWear != WEAR_TRY && get_eq_char( ch, iWear ) != NULL )
        return FALSE;

    if ( iWear != WEAR_TRY && !IS_SET( race_table[ race ].wear_flag, wear_to_itemwear[ iWear ] ) )
    {
        send_to_char( "Nie za bardzo wiesz gdzie to za³o¿yæ.\n\r", ch );
        return FALSE;
    }

    if ( IS_OBJ_STAT( obj, ITEM_BLESS ) && ch->class == CLASS_BLACK_KNIGHT )
    {
        send_to_char( "Nie mo¿esz u¿ywaæ pob³ogos³awionych przedmiotów.\n\r", ch );
        return FALSE;
    }

    if ( IS_OBJ_STAT( obj, ITEM_EVIL ) && ch->class == CLASS_PALADIN )
    {
        send_to_char( "Nie mo¿esz u¿ywaæ przedmiotów przesi±kniêtych z³em.\n\r", ch );
        return FALSE;
    }

    if ( ( obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_TOOL ) && IS_WEAPON_STAT( obj, WEAPON_VAMPIRIC ) && IS_GOOD(ch))
    {
        send_to_char( "Wstrêtna, wysysaj±ca ¿ycie aura otaczaj±ca ten przedmiot jest tak odpychaj±ca, ¿e nie mo¿esz go u¿yæ.\n\r", ch );
        return FALSE;
    }

    if ( ( obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_TOOL ) && IS_WEAPON_STAT( obj, WEAPON_SACRED ) && IS_EVIL(ch))
    {
        send_to_char( "Wstrêtna aura ¶wiêto¶ci otaczaj±ca ten przedmiot jest tak odpychaj±ca, ¿e nie mo¿esz go u¿yæ.\n\r", ch );
        return FALSE;
    }

    if ( ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL ) && IS_EVIL( ch ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD ) && IS_GOOD( ch ) )
            || ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL ) && IS_NEUTRAL( ch ) ) )
    {
        if ( IS_NPC( ch ) )
        {
            act( "Nagle czujesz straszliwy ból rozdzieraj±cy twoje cia³o, zdejmujesz i odrzucasz $h.", ch, obj, NULL, TO_CHAR );
            act( "$n krzywi siê z bólu, b³yskawicznie zdejmuje i odrzuca $h.", ch, obj, NULL, TO_ROOM );
        }
        else
        {
            act( "Nagle czujesz straszliwy ból rozdzieraj±cy twoje cia³o i zdejmujesz $h.", ch, obj, NULL, TO_CHAR );
            act( "$n krzywi siê z bólu i b³yskawicznie zdejmuje $h.", ch, obj, NULL, TO_ROOM );
        }

        if ( !IS_NPC( ch ) && iWear != WEAR_TRY )
        {
            /*artefact*/
            if ( is_artefact( obj ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
            {
                artefact_from_char( obj, ch );
            }
            obj_from_char( obj );

            obj_to_room( obj, ch->in_room );
        }
        return FALSE;
    }

    class = get_class( ch, -1 );
    if ( class == CLASS_MAG && EXT_IS_SET( ch->act, ACT_WARRIOR ) )
        class = CLASS_WARRIOR;

    /* sprawdzanie exwear NORASA */
    if ( ( EXT_IS_SET( obj->wear_flags2, ITEM_NOHUMAN ) && race == 1 ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NOELF ) && race == 2 ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NODWARF ) && race == 3 ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NOGNOM ) && race == 4 ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NOHALFELF ) && race == 5 ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NOHALFLING ) && race == 6 ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NOHALFORC ) && race == 7 ) )
    {
        act( "Niestety, twoja rasa nie mo¿e u¿ywaæ $f.", ch, obj, NULL, TO_CHAR );
        act( "$n próbuje za³o¿yæ $h.", ch, obj, NULL, TO_ROOM );
        return FALSE;
    }

    /* sprawdzanie exwear ONLYRASA*/
    if ( ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYHUMAN ) && race != 1 ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYELF ) && !(race == 2  || race == 58) ) || //drow to te¿ elf, prawda?
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYDWARF ) && race != 3 ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYGNOM ) && race != 4 ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYHALFELF ) && race != 5 ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYHALFLING ) && race != 6 ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYHALFORC ) && race != 7 ) )
    {
        act( "Niestety, twoja rasa nie mo¿e u¿ywaæ $f.", ch, obj, NULL, TO_CHAR );
        act( "$n próbuje za³o¿yæ $h.", ch, obj, NULL, TO_ROOM );
        return FALSE;
    }

    if ( ( iWear == WEAR_WIELD || iWear == WEAR_TRY ) && ( obj->item_type == ITEM_WEAPON ) && class != -1 )
    {
        sn = weapon_sn( obj );

        if ( sn == -1 )
            return FALSE;

        if ( skill_table[ sn ].skill_level[ class ] > ch->level )
        {
            send_to_char( "Nie wiesz jak siê tego u¿ywa.\n\r", ch );
            return FALSE;
        }
    }

    //rellik: tools
    if ( ( iWear == WEAR_TRY || iWear == WEAR_HOLD ) && ( obj->item_type == ITEM_TOOL ) && class != -1 )
    {
        //Rysand - moby mog± ubraæ dowolny kilof - nie maj± skilli!
        if(IS_NPC(ch))
        {
            return TRUE;
        }

        sn = tool_sn( obj );
        if ( sn == -1 )	return FALSE;

        if ( skill_table[sn].skill_level[class] > ch->level	)
        {
            send_to_char( "Nie wiesz jak siê tego u¿ywa.\n\r", ch );
            return FALSE;
        }
        if ( obj->value[1] > get_skill( ch, sn ) )
        {
            print_char( ch, "Nie jeste¶ wystarczaj±co bieg³y w fachu aby u¿ywaæ tego narzêdzia.\n\r" );
            buf[0] = '\0';
            strcat( buf, "To narzêdzie bêdziesz móg³ u¿ywaæ je¶li wyszkolisz umiejêtno¶æ na " );
            switch ( obj->value[1] / 10 )
            {
                case 0:
                    strcat( buf, "bardzo s³abo" );
                    break;
                case 1:
                    strcat( buf, "s³abo" );
                    break;
                case 2:
                    strcat( buf, "kiepsko" );
                    break;
                case 3:
                    strcat( buf, "poni¿ej ¶redniej" );
                    break;
                case 4:
                    strcat( buf, "¶rednio" );
                    break;
                case 5:
                    strcat( buf, "nie¼le" );
                    break;
                case 6:
                    strcat( buf, "dobrze" );
                    break;
                case 7:
                    strcat( buf, "bardzo dobrze" );
                    break;
                case 8:
                    strcat( buf, "mistrzowsko" );
                    break;
                case 9:
                    strcat( buf, "najwy¿szy poziom" );
                    break;
            }

            print_char( ch, "%s.\n\r", buf );
            return FALSE;
        }
    }


    if ( ( iWear == WEAR_INSTRUMENT || iWear == WEAR_TRY ) && ( obj->item_type == ITEM_MUSICAL_INSTRUMENT ) && class != -1 )
    {
        sn = musical_instrument_sn( obj );

        if ( sn == -1 )
            return FALSE;

        if ( skill_table[ sn ].skill_level[ class ] > ch->level )
        {
            send_to_char( "Nie wiesz jak siê tego u¿ywa.\n\r", ch );
            return FALSE;
        }
    }

    if ( ( EXT_IS_SET( obj->wear_flags2, ITEM_NOMAG ) && class == CLASS_MAG ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NOCLE ) && class == CLASS_CLERIC ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NOTHI ) && class == CLASS_THIEF ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NOWAR ) && class == CLASS_WARRIOR ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NOPAL ) && class == CLASS_PALADIN ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NODRUID ) && class == CLASS_DRUID ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NOBARD ) && class == CLASS_BARD ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NOBLACKKNIGHT ) && class == CLASS_BLACK_KNIGHT ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NOBARBARIAN ) && class == CLASS_BARBARIAN ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_NOSHAMAN ) && class == CLASS_SHAMAN ) )
    {
        switch (obj -> gender)
        {
            case 4:
            case 5:
                act( "Niestety, $p zbytnio ciê uwieraj±.", ch, obj, NULL, TO_CHAR );
                break;
            default :
                act( "Niestety, $p zbytnio ciê uwiera.", ch, obj, NULL, TO_CHAR );
                break;
        }
        act( "$n próbuje za³o¿yæ $h.", ch, obj, NULL, TO_ROOM );
        return FALSE;
    }

    if ( ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYMAG ) && class != CLASS_MAG ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYCLE ) && class != CLASS_CLERIC ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYTHI ) && class != CLASS_THIEF ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYWAR ) && class != CLASS_WARRIOR ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYPAL ) && class != CLASS_PALADIN ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYDRU ) && class != CLASS_DRUID ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYBARD ) && class != CLASS_BARD ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYBLACKKNIGHT ) && class != CLASS_BLACK_KNIGHT ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYBAR ) && class != CLASS_BARBARIAN ) ||
            ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYSHA ) && class != CLASS_SHAMAN ) )
    {
        switch (obj -> gender)
        {
            case 4:
            case 5:
                act( "Niestety, $p zbytnio ciê uwieraj±.", ch, obj, NULL, TO_CHAR );
                break;
            default :
                act( "Niestety, $p zbytnio ciê uwiera.", ch, obj, NULL, TO_CHAR );
                break;
        }
        act( "$n próbuje za³o¿yæ $h.", ch, obj, NULL, TO_ROOM );
        return FALSE;
    }

    if ( !IS_NPC(ch) && ch->class == CLASS_MAG )
    {
        specka = ch->pcdata->mage_specialist;
        if (( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYODRZUCANIE	    ) && specka != 0 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYPRZEMIANY	    ) && specka != 1 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYPRZYWOLANIA	) && specka != 2 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYPOZNANIE	    ) && specka != 3 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYZAUROCZENIA	) && specka != 4 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYILUZJE		    ) && specka != 5 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYINWOKACJE	    ) && specka != 6 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYNEKROMANCJA	) && specka != 7 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYGENERAL		) && specka != -1 ) )
        {
            switch (obj -> gender)
            {
                case 4:
                case 5:
                    act( "Niestety, $p zbytnio ciê uwieraj±.", ch, obj, NULL, TO_CHAR );
                    break;
                default :
                    act( "Niestety, $p zbytnio ciê uwiera.", ch, obj, NULL, TO_CHAR );
                    break;
            }
            act( "$n próbuje za³o¿yæ $h.", ch, obj, NULL, TO_ROOM );
            return FALSE;
        }

        if (( EXT_IS_SET( obj->wear_flags2, ITEM_NOODRZUCANIE	    ) && specka == 0 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_NOPRZEMIANY		) && specka == 1 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_NOPRZYWOLANIA      ) && specka == 2 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_NOPOZNANIE		    ) && specka == 3 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_NOZAUROCZENIA    	) && specka == 4 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_NOILUZJE		    ) && specka == 5 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_NOINWOKACJE		) && specka == 6 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_NONEKROMANCJA     	) && specka == 7 ) ||
                ( EXT_IS_SET( obj->wear_flags2, ITEM_NOGENERAL			) && specka == -1 ) )
        {
            switch (obj -> gender)
            {
                case 4:
                case 5:
                    act( "Niestety, $p zbytnio ciê uwieraj±.", ch, obj, NULL, TO_CHAR );
                    break;
                default :
                    act( "Niestety, $p zbytnio ciê uwiera.", ch, obj, NULL, TO_CHAR );
                    break;
            }
            act( "$n próbuje za³o¿yæ $h.", ch, obj, NULL, TO_ROOM );
            return FALSE;
        }
    }

    if ( obj->item_type == ITEM_ARMOR &&
            obj->value[ 4 ] >= 0 &&
            obj->value[ 4 ] < 16 &&
            class != -1 &&
            armor_table[ obj->value[ 4 ] ].can_wear[ class ] == 0 )
    {
        switch (obj -> gender)
        {
            case 4:
            case 5:
                act( "Niestety, $p zbytnio ciê uwieraj±.", ch, obj, NULL, TO_CHAR );
                break;
            default :
                act( "Niestety, $p zbytnio ciê uwiera.", ch, obj, NULL, TO_CHAR );
                break;
        }
        act( "$n próbuje za³o¿yæ $h.", ch, obj, NULL, TO_ROOM );
        return FALSE;
    }

    if ( IS_SET ( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER )
            && obj->item_type == ITEM_LIGHT && obj->value[ 0 ] != 1 )
    {
        act( "$n próbuje zapaliæ $h, ale nic z tego nie wychodzi.", ch, obj, NULL, TO_ROOM );
        act( "Próbujesz zapaliæ $h, ale nic z tego nie wychodzi.", ch, obj, NULL, TO_CHAR );
        return FALSE;
    }

    if ( check_only_weight_cant_equip( ch, obj ) )
    {
        return FALSE;
    }

    return TRUE;
}

/*
 * Equip a char with an obj.
 */
bool equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear, bool checkwear )
{
    AFFECT_DATA * paf;
    int i;

    if ( checkwear )
    {
        if ( !can_equip_obj( ch, obj, iWear ) )
        {
            return FALSE;
        }
    }

    for ( i = 0; i < 4; i++ )
        ch->armor[ i ] -= apply_ac( obj, iWear, i );

    obj->wear_loc = iWear;

    if ( !obj->enchanted )
        for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
            if ( paf->location != APPLY_SPELL_AFFECT )
                affect_modify( ch, paf, TRUE );

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
        if ( paf->location == APPLY_SPELL_AFFECT )
            affect_to_char ( ch, paf, NULL, TRUE );
        else
            affect_modify( ch, paf, TRUE );

    if ( ( obj->item_type == ITEM_LIGHT )
            && obj->value[ 2 ] != 0
            && ch->in_room != NULL )
        ++ch->in_room->light;

    /* set stuff */
    if ( obj->pIndexData->bonus_set > 0 )
    {
        BONUS_INDEX_DATA * bonus_index;
        OBJ_INDEX_DATA *pObj;
        OBJ_DATA *wear;
        AREA_DATA *pArea;
        int vnum;
        bool eq_check = FALSE;

        bonus_index = get_bonus_index( obj->pIndexData->bonus_set );
        pArea = obj->pIndexData->area;

        if ( !bonus_index )
            return TRUE;

        /* sprawdzenie czy ma wszystko */
        for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
        {
            pObj = get_obj_index( vnum );

            if ( !pObj || pObj->bonus_set <= 0 || pObj->bonus_set != obj->pIndexData->bonus_set )
                continue;

            eq_check = FALSE;

            for ( wear = ch->carrying; wear; wear = wear->next_content )
            {
                if ( wear->wear_loc == WEAR_NONE )
                    continue;

                if ( wear->pIndexData == pObj )
                    eq_check = TRUE;
            }

            if ( !eq_check )
                return TRUE;
        }

        if ( !eq_check )
            return TRUE;

        bonus_to_char( ch, bonus_index );
    }
    return TRUE;
}


/*
 * Unequip a char with an obj.
 */
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
    AFFECT_DATA * paf = NULL;
    AFFECT_DATA *lpaf = NULL;
    AFFECT_DATA *lpaf_next = NULL;
    int i;

    if ( obj->wear_loc == WEAR_NONE )
        return ;

    if( IS_OBJ_STAT( obj, ITEM_DOUBLE_GRIP ) && obj->item_type == ITEM_WEAPON )
    {
        REMOVE_BIT( obj->value[4], WEAPON_TWO_HANDS );
        EXT_REMOVE_BIT( obj->extra_flags, ITEM_DOUBLE_GRIP );
    }

    if ( ch->in_room && HAS_OTRIGGER( obj, TRIG_UNEQUIP ) )
        op_common_trigger( ch, obj, &TRIG_UNEQUIP );

    /*    if( !ch->in_room )
          {
          bugf("%s zginal na progu UNEQUIP?!?", ch->name);
          return;
          }*/

    for ( i = 0; i < 4; i++ )
        ch->armor[ i ] += apply_ac( obj, obj->wear_loc, i );
    obj->wear_loc	= -1;

    if ( !obj->enchanted )
    {
        for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
            if ( paf->location == APPLY_SPELL_AFFECT )
            {
                for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
                {
                    lpaf_next = lpaf->next;
                    if ( ( lpaf->type == paf->type ) &&
                            ( lpaf->level == paf->level ) &&
                            ( lpaf->location == APPLY_SPELL_AFFECT ) )
                    {
                        affect_remove( ch, lpaf );
                        lpaf_next = NULL;
                    }
                }
            }
            else
            {
                affect_modify( ch, paf, FALSE );
                affect_check( ch, paf->where, paf->bitvector );
            }
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
        if ( paf->location == APPLY_SPELL_AFFECT )
        {
            bug ( "Norm-Apply: %d", 0 );
            for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
            {
                lpaf_next = lpaf->next;
                if ( ( lpaf->type == paf->type ) &&
                        ( lpaf->level == paf->level ) &&
                        ( lpaf->location == APPLY_SPELL_AFFECT ) )
                {
                    bug ( "location = %d", lpaf->location );
                    bug ( "type = %d", lpaf->type );
                    affect_remove( ch, lpaf );
                    lpaf_next = NULL;
                }
            }
        }
        else
        {
            affect_modify( ch, paf, FALSE );
            affect_check( ch, paf->where, paf->bitvector );
        }

    /* to tu */
    if ( ( obj->item_type == ITEM_LIGHT )
            && obj->value[ 2 ] != 0
            && ch->in_room != NULL
            && ch->in_room->light > 0 )
        --ch->in_room->light;

    if ( obj->pIndexData->bonus_set > 0 )
    {
        BONUS_DATA * bonus = NULL;
        BONUS_INDEX_DATA * pBonus = NULL;

        if ( ( pBonus = get_bonus_index( obj->pIndexData->bonus_set ) ) == NULL )
            return;

        for ( bonus = ch->bonus; bonus; bonus = bonus->next )
        {
            if ( bonus->index_data == pBonus )
                break;
        }

        if ( bonus )
            bonus_remove( ch, bonus );
    }

    return ;
}

/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list( OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list )
{
    OBJ_DATA *obj;
    int nMatch;

    nMatch = 0;

    for ( obj = list; obj != NULL; obj = obj->next_content )
    {
        if ( obj->pIndexData == pObjIndex )
            nMatch++;
    }

    return nMatch;
}


/*
 * Move an obj out of a room.
 */
void obj_from_room( OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *in_room;
    CHAR_DATA *ch;

    if ( !obj )
        return;

    if ( ( in_room = obj->in_room ) == NULL )
    {
        bug( "obj_from_room: NULL.", 0 );
        return;
    }

    for (ch = in_room->people; ch != NULL; ch = ch->next_in_room)
        if (ch->on == obj)
            ch->on = NULL;

    if ( obj == in_room->contents )
    {
        in_room->contents = obj->next_content;
    }
    else
    {
        OBJ_DATA *prev;

        for ( prev = in_room->contents; prev; prev = prev->next_content )
        {
            if ( prev->next_content == obj )
            {
                prev->next_content = obj->next_content;
                break;
            }
        }

        if ( prev == NULL )
        {
            bug( "Obj_from_room: obj not found.", 0 );
            return;
        }
    }

    obj->in_room      = NULL;
    obj->next_content = NULL;
    return;
}


/*
 * Move an obj into a room.
 */
void obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex )
{
    int crash_protect = 0;

    if ( !obj || !pRoomIndex )
    {
        return;
    }

    obj->next_content    = pRoomIndex->contents;
    pRoomIndex->contents = obj;
    obj->in_room         = pRoomIndex;
    obj->carried_by      = NULL;
    obj->in_obj          = NULL;

    /**
     * Falling items snippet coded by Thales, co-imp
     * of Islands of Exile mud: funcity.org 9999
     */
    if ( IS_SET( sector_table[ pRoomIndex->sector_type ].flag, SECT_AIR ) && ( pRoomIndex->exit[DIR_DOWN] != NULL ) )
    {
        crash_protect++;
        if ( crash_protect == 10 )
        {
            return;
        }
        act( "$t spada z góry.\n\r", pRoomIndex->exit[DIR_DOWN]->u1.to_room->people, obj->short_descr, NULL, TO_ROOM);
        act( "$t spada z góry.\n\r", pRoomIndex->exit[DIR_DOWN]->u1.to_room->people, obj->short_descr, NULL, TO_CHAR);
        act( "$t spada w dó³.\n\r", pRoomIndex->people, obj->short_descr, NULL, TO_ROOM);
        act( "$t spada w dó³.\n\r", pRoomIndex->people, obj->short_descr, NULL, TO_CHAR);
        obj_from_room( obj );
        obj_to_room( obj, pRoomIndex->exit[DIR_DOWN]->u1.to_room);
    }
    crash_protect = 0;

    return;
}


/*
 * Move an object into an object.
 */
void obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to )
{
    if ( !obj || !obj_to )
        return;

    obj->next_content		= obj_to->contains;
    obj_to->contains		= obj;
    obj->in_obj			= obj_to;
    obj->in_room		= NULL;
    obj->carried_by		= NULL;

    /*    if (obj_to->pIndexData->vnum == OBJ_VNUM_PIT)
          obj->cost = 0; */

    if( IS_SET( obj_to->value[1], CONT_COMP ) )
    {
        obj->spell_item_timer = obj->spell_item_timer * WEIGHT_MULT(obj_to) / 100;
    }

    for ( ; obj_to != NULL; obj_to = obj_to->in_obj )
    {
        if ( obj_to->carried_by != NULL )
        {
            obj_to->carried_by->carry_number += get_obj_number( obj );
            obj_to->carried_by->carry_weight += get_obj_weight( obj )
                * WEIGHT_MULT(obj_to) / 100;
        }
    }

    return;
}


/*
 * Move an object out of an object.
 */
void obj_from_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_from;

    if ( !obj )
        return;

    if ( ( obj_from = obj->in_obj ) == NULL )
    {
        bug( "Obj_from_obj: null obj_from.", 0 );
        return;
    }

    if ( obj == obj_from->contains )
    {
        obj_from->contains = obj->next_content;
    }
    else
    {
        OBJ_DATA *prev;

        for ( prev = obj_from->contains; prev; prev = prev->next_content )
        {
            if ( prev->next_content == obj )
            {
                prev->next_content = obj->next_content;
                break;
            }
        }

        if ( prev == NULL )
        {
            bug( "Obj_from_obj: obj not found.", 0 );
            return;
        }
    }

    if(IS_SET( obj_from->value[1], CONT_COMP)) 
    {
        obj->spell_item_timer = UMAX(1, (obj->spell_item_timer * 100) / WEIGHT_MULT(obj_from));
    }

    obj->next_content = NULL;
    obj->in_obj       = NULL;

    for ( ; obj_from != NULL; obj_from = obj_from->in_obj )
    {
        if ( obj_from->carried_by != NULL )
        {
            obj_from->carried_by->carry_number -= get_obj_number( obj );
            obj_from->carried_by->carry_weight -= get_obj_weight( obj )
                * WEIGHT_MULT(obj_from) / 100;
        }
    }

    return;
}


/*
 * Extract an obj from the world.
 */
void extract_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_content;
    OBJ_DATA *obj_next;
    EVENT_DATA *event;

    if ( obj->hoarded_by != NULL )
        obj_from_hoard( obj );

    if ( obj->in_room != NULL )
        obj_from_room( obj );
    else if ( obj->carried_by != NULL )
        obj_from_char( obj );
    else if ( obj->in_obj != NULL )
        obj_from_obj( obj );

    for ( obj_content = obj->contains; obj_content; obj_content = obj_next )
    {
        OBJ_NEXT_CONTENT( obj_content, obj_next );
        extract_obj( obj_content );
    }

    obj->contains = NULL;

    // Tener: nie ma potrzeby zapuszczaæ tego kombajnu dla ka¿dego przedmiotu

    if ( ( obj->pIndexData->vnum == OBJ_VNUM_TREE1 ) ||
            ( obj->pIndexData->vnum == OBJ_VNUM_TREE2 ) ||
            ( obj->pIndexData->vnum == OBJ_VNUM_TREE3 ) )
    {
        for ( event = event_first; event; event = event->next )
        {
            if ( event->deleted )
                continue;

            if ( ( event->type == EVENT_TREE_FRUITS || event->type == EVENT_TREE_EXTRACT )
                    && ( OBJ_DATA * ) event->arg2 == obj )
                event->deleted = TRUE;
        }
    }

    if ( object_list == obj )
    {
        object_list = obj->next;
    }
    else
    {
        OBJ_DATA *prev;

        for ( prev = object_list; prev != NULL; prev = prev->next )
        {
            if ( prev->next == obj )
            {
                prev->next = obj->next;
                break;
            }
        }

        if ( prev == NULL )
        {
            return;
        }
    }

    --obj->pIndexData->count;
    free_obj(obj);
    return;
}

/*
 * Extract a char from the world.
 */
void extract_char( CHAR_DATA *ch, bool fPull )
{
    CHAR_DATA * wch;
    EVENT_DATA *event = NULL;

    DEBUG_INFO( "[extract_char]:die_follower" );

    die_follower( ch, TRUE );

    if ( ch->mount && ch->mount->mounting )
    {
        do_dismount_body(ch);
    }

    if ( ch->mounting && ch->mounting->mount )
    {
        do_dismount_body(ch);
    }

    DEBUG_INFO( "[extract_char]:sifollow_remove_char");
    // ciezkie, bo musi przeleciec przez wszystkie osoby na mudzie
    CHAR_DATA * chr;
    for( chr = char_list; chr != NULL; chr = chr->next )
    {
        sifollow_remove_char( chr, ch );
    }

    DEBUG_INFO( "[extract_char]:stop_fighting" );

    if ( IS_NPC( ch ) )
        stop_fighting( ch, TRUE );

    DEBUG_INFO( "[extract_char]:char_from_char" );
    if ( ch->in_room != NULL )
        char_from_room( ch );

    if ( IS_NPC( ch ) )
        --ch->pIndexData->count;

    DEBUG_INFO( "[extract_char]:do_return" );
    if ( ch->desc != NULL && ch->desc->original != NULL )
    {
        do_function( ch, &do_return, "" );
        ch->desc = NULL;
    }

    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
        DEBUG_INFO( "[extract_char]:loop_cancel_reply" );
        if ( wch->reply == ch )
            wch->reply = NULL;

        DEBUG_INFO( "[extract_char]:loop_cancel_prog_target" );
        if ( wch->prog_target == ch )
            wch->prog_target = NULL;

        if ( wch->master == ch )
            wch->master = NULL;

        if ( wch->leader == ch )
            wch->leader = NULL;

        DEBUG_INFO( "[extract_char]:stop_hhf" );
        if ( is_hating( wch, ch ) )
            stop_hating( wch, ch, FALSE );

        if ( wch->hunting && wch->hunting == ch )
            stop_hunting( wch );

        if ( is_fearing( wch, ch ) )
            stop_fearing( wch, ch, FALSE );
    }

    for ( event = event_first; event; event = event->next )
    {
        if ( event->deleted )
            continue;

        if ( ( event->type == EVENT_EXTRACT_CHAR || event->type == EVENT_EXTRACT_SOUL )
                && ( CHAR_DATA * ) event->arg1 == ch )
            event->deleted = TRUE;
    }

    del_spirit( ch );

    DEBUG_INFO( "[extract_char]:removing_from_list" );
    if ( ch == char_list )
    {
        char_list = ch->next;
    }
    else
    {
        CHAR_DATA *prev;

        for ( prev = char_list; prev != NULL; prev = prev->next )
        {
            if ( prev->next == ch )
            {
                prev->next = ch->next;
                break;
            }
        }
    }

    DEBUG_INFO( "[extract_char]:ch=NULL" );
    if ( ch->desc != NULL )
    {
        close_socket( ch->desc, 0 );
    }
    else
    {
        DEBUG_INFO( "[extract_char]:free_char" );
        free_char( ch );
        DEBUG_INFO( "[extract_char]:done" );
    }
    return ;
}

/*
 * Find a char in the room.
 */
CHAR_DATA *get_char_room( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA * in_room;
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *rch;
    int number;
    int count;

    number = number_argument( argument, arg );

    if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
    {
        in_room = ch->pcdata->mind_in;

        if ( !str_cmp( arg, "self" ) && ch->in_room == ch->pcdata->mind_in )
            return ch;
    }
    else
    {
        in_room = ch->in_room;

        if ( !str_cmp( arg, "self" ) )
            return ch;
    }

    /*
       if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && !str_cmp( arg, "remote" ) && ch->desc && d->desc->remote_char )
       return d->desc->remote_char;
     */



    count  = 0;

    for ( rch = in_room->people; rch != NULL; rch = rch->next_in_room )
    {
        if ( !can_see( ch, rch ) )
            continue;

        if ( !is_name( arg, rch->ss_data ? rch->short_descr : rch->name ) )
            continue;

        if ( ++count == number )
            return rch;
    }

    /* Tener: aby mo¿na siê by³o odwo³ywaæ do poddanych przez n.follower */
    count = 0;
    if( !str_prefix( arg, "follower" ) )
    {
        for ( rch = char_list; rch != NULL; rch = rch->next )
        {
            if ( IS_NPC( rch )
                    && IS_AFFECTED( rch, AFF_CHARM )
                    && rch->master == ch )
                count++;
            if ( count == number )
            {
                if ( can_see( ch, rch ) && ( ch->in_room == rch->in_room ) )
                    return rch;
                else
                    return NULL;
            }
        }
    }


    return NULL;
}



/*
 * Find a char in the world.
 */
CHAR_DATA *get_char_world( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *wch;
    int number;
    int count;

    if ( ( wch = get_char_room( ch, argument ) ) != NULL )
        return wch;

    number = number_argument( argument, arg );
    count  = 0;
    for ( wch = char_list; wch != NULL ; wch = wch->next )
    {
        if ( wch->in_room == NULL || !can_see( ch, wch )
                ||   !is_name( arg, wch->ss_data ? wch->short_descr : wch->name ) )
            continue;
        if ( ++count == number )
            return wch;
    }

    return NULL;
}

CHAR_DATA *get_char_area( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * ach;
    char arg [ MAX_INPUT_LENGTH ];
    int number;
    int count;

    if ( ( ach = get_char_room( ch, argument ) ) )
        return ach;

    number = number_argument( argument, arg );
    count = 0;
    for ( ach = char_list; ach ; ach = ach->next )
    {
        if ( ach->in_room && ch->in_room
                && SAME_AREA( ach->in_room->area, ch->in_room->area )
                && SAME_AREA_PART( ach, ch )
                && can_see( ch, ach ) )
        {
            if ( !is_name( arg, ach->ss_data ? ach->short_descr : ach->name ) )
                continue;
            if ( ++count == number )
                return ach;
        }
    }

    return NULL;
}

/*
 * Find some object with a given index data.
 * Used by area-reset 'P' command.
 */
OBJ_DATA *get_obj_type( ROOM_INDEX_DATA *pRoom, OBJ_INDEX_DATA *pObjIndex )
{
    OBJ_DATA *obj;

    for ( obj = pRoom->contents; obj != NULL; obj = obj->next_content )
    {
        if ( obj->pIndexData == pObjIndex )
            return obj;
    }

    return NULL;
}

/*
 * Find an obj in a list.
 */
OBJ_DATA *get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
    {
        if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
        {
            if ( ++count == number )
                return obj;
        }
    }

    return NULL;
}


/*
 * Find an obj in player's inventory.
 */
OBJ_DATA *get_obj_carry( CHAR_DATA *ch, char *argument, CHAR_DATA *viewer )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
        if ( obj->wear_loc == WEAR_NONE
                &&   (can_see_obj( viewer, obj ) )
                &&   is_name( arg, obj->name ) )
        {
            if ( ++count == number )
                return obj;
        }
    }

    return NULL;
}

/*
 * Find an obj in player's equipment.
 */
OBJ_DATA *get_obj_wear( CHAR_DATA *ch, char *argument, bool dont_check_can_see )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;
    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
        if ( obj->wear_loc != WEAR_NONE && ( dont_check_can_see || can_see_obj( ch, obj ) ) &&   is_name( arg, obj->name ) )
        {
            if ( ++count == number )
            {
                return obj;
            }
        }
    }
    return NULL;
}


/*
 * Find an obj in the room or in inventory. Old, buggy version below. (cut by Maro. *flex*)
 */
OBJ_DATA *get_obj_here( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
    {
        obj = get_obj_list( ch, argument, ch->pcdata->mind_in->contents );
        return obj;
    }

    if ( ( obj = get_obj_list( ch, argument, ch->in_room->contents ) ) != NULL )
        return obj;

    if ( ( obj = get_obj_carry( ch, argument, ch ) ) != NULL )
        return obj;

    if ( ( obj = get_obj_wear( ch, argument, FALSE ) ) != NULL )
        return obj;

    return NULL;
}

/*
 * Find an obj in the inventory and then in room.
 */
OBJ_DATA *get_obj_here_alt( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    if ( ( obj = get_obj_carry( ch, argument, ch ) ) != NULL )
        return obj;

    if ( ( obj = get_obj_wear( ch, argument, FALSE ) ) != NULL )
        return obj;

    if ( ( obj = get_obj_list( ch, argument, ch->in_room->contents ) ) != NULL )
        return obj;

    return NULL;
}

/*
 * Find an obj in the world.
 */
OBJ_DATA *get_obj_world( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    if ( ( obj = get_obj_here( ch, argument ) ) != NULL )
        return obj;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
        if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
        {
            if ( ++count == number )
                return obj;
        }
    }

    return NULL;
}

/*
 * Create a 'money' obj.
 */
OBJ_DATA *create_money( int copper, int silver, int gold, int mithril )
{
    OBJ_DATA *obj;
    if ( copper < 1 && silver < 1 && gold < 1 && mithril < 1 )
    {
        bug( "Create_money: zero or negative money.",0);
        copper = 1;
        silver = gold = mithril = 0;
    }

    if ( copper == 1 && silver == 0 && gold == 0 && mithril == 0 )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_COIN_COPPER ), FALSE);
    }
    else if ( copper == 0 && silver == 1 && gold == 0 && mithril == 0 )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_COIN_SILVER ), FALSE);
    }
    else if ( copper == 0 && silver == 0 && gold == 1 && mithril == 0 )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_COIN_GOLD ), FALSE);
    }
    else if ( copper == 0 && silver == 0 && gold == 0 && mithril == 1 )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_COIN_MITHRIL ), FALSE);
    }
    else
    {
        obj = create_object( get_obj_index( OBJ_VNUM_COINS ), FALSE );
        obj->value[0]     = copper;
        obj->value[1]     = silver;
        obj->value[2]     = gold;
        obj->value[3]     = mithril;
        obj->cost         = money_to_copper ( copper, silver, gold, mithril );
        obj->weight	      = money_weight ( copper, silver, gold, mithril );
        obj->liczba_mnoga = 1;
    }
    return obj;
}

/*
 * Return # of objects which an object counts as.
 * Thanks to Tony Chamberlain for the correct recursive code here.
 */
int get_obj_number( OBJ_DATA *obj )
{
    int number;

    if (obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_MONEY
            ||  obj->item_type == ITEM_GEM || obj->item_type == ITEM_JEWELRY
            || obj->item_type == ITEM_BANDAGE || obj->item_type == ITEM_HERB)
        number = 0;
    else
        number = 1;

    for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
        number += get_obj_number( obj );

    return number;
}

/*
 * Return weight of an object, including weight of contents.
 */
int get_obj_weight( OBJ_DATA *obj )
{
    int weight;
    OBJ_DATA *tobj;

    weight = obj->weight;
    for ( tobj = obj->contains; tobj != NULL; tobj = tobj->next_content )
        weight += get_obj_weight( tobj ) * WEIGHT_MULT(obj) / 100;

    return (weight);
}

int get_true_weight(OBJ_DATA *obj)
{
    int weight;

    weight = obj->weight;
    for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
        weight += get_obj_weight( obj );

    return (weight);
}

/*
 * True if room is dark.
 */
extern CHAR_DATA *supermob;
bool room_is_dark( CHAR_DATA * ch, ROOM_INDEX_DATA *pRoomIndex )
{
    if ( ch && IS_AFFECTED( ch, AFF_BLIND ) && !IS_AFFECTED( ch, AFF_PERFECT_SENSES ) )
        return FALSE;

    if ( ch && IS_AFFECTED( ch, AFF_DARK_VISION ) )
        return FALSE;

    if ( ch && is_undead( ch ) )
        return FALSE;

    if ( EXT_IS_SET( pRoomIndex->room_flags, ROOM_MAGICDARK ) )
        return TRUE;

    if ( pRoomIndex->light > 0 )
        return FALSE;

    if ( EXT_IS_SET( pRoomIndex->room_flags, ROOM_DARK ) )
        return TRUE;

    if ( EXT_IS_SET( pRoomIndex->room_flags, ROOM_LIGHT ) )
        return FALSE;

    if ( IS_SET( sector_table[pRoomIndex->sector_type].flag,SECT_LIGHT ) )
        return FALSE;

    if ( IS_SET( sector_table[pRoomIndex->sector_type].flag,SECT_NOWEATHER ) )
        return TRUE;

    if ( time_info.hour == HOUR_SUNSET )
        return FALSE;

    if ( weather_info[pRoomIndex->sector_type].sunlight == SUN_SET
            ||   weather_info[pRoomIndex->sector_type].sunlight == SUN_DARK )
        return TRUE;

    return FALSE;
}

bool is_room_owner(CHAR_DATA *ch, ROOM_INDEX_DATA *room)
{
    if (room->owner == NULL || room->owner[0] == '\0')
        return FALSE;

    return is_name(ch->name,room->owner);
}

/*
 * True if room is private.
 */
bool room_is_private( ROOM_INDEX_DATA *pRoomIndex )
{
    CHAR_DATA *rch;
    int count;

    if (pRoomIndex->owner != NULL && pRoomIndex->owner[0] != '\0')
        return TRUE;

    count = 0;
    for ( rch = pRoomIndex->people; rch != NULL; rch = rch->next_in_room )
        count++;

    if ( EXT_IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE)  && count >= 2 )
        return TRUE;

    if ( EXT_IS_SET(pRoomIndex->room_flags, ROOM_SOLITARY) && count >= 1 )
        return TRUE;

    return FALSE;
}

/* visibility on a room -- for entering and exits */
bool can_see_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    if ( EXT_IS_SET( pRoomIndex->room_flags, ROOM_HEROES_ONLY )
            &&  !IS_IMMORTAL( ch ) )
        return FALSE;

    if ( EXT_IS_SET( pRoomIndex->room_flags, ROOM_NEWBIES_ONLY )
            && ch->level > 5 && !IS_IMMORTAL( ch ) )
        return FALSE;

    return TRUE;
}


/*
 * True if char can see victim.
 */
bool can_see( CHAR_DATA *ch, CHAR_DATA *victim )
{
    AFFECT_DATA *dazzling;
    /* RT changed so that WIZ_INVIS has levels */
    if ( !ch || !victim )
        return FALSE;

    if ( ch == victim )
        return TRUE;

    /*supermob widzi wsio*/
    if(IS_NPC(ch) && ch->pIndexData->vnum==3)
        return TRUE;

    /*ale jego nikt nie widzi*/
    if(IS_NPC(victim) && victim->pIndexData->vnum==3)
        return FALSE;

    if( get_spirit( ch ) || get_spirit ( victim ) )
       return FALSE;

    if ( get_trust(ch) < victim->invis_level)
        return FALSE;

    if (get_trust(ch) < victim->incog_level && ch->in_room != victim->in_room)
        return FALSE;

    if ( (!IS_NPC( ch ) && EXT_IS_SET( ch->act, PLR_HOLYLIGHT ) ) )
        return TRUE;

    if ( IS_AFFECTED(ch, AFF_BLIND) && !IS_AFFECTED(ch,AFF_PERFECT_SENSES))
        return FALSE;

    if( !ch->in_room || !victim->in_room )
        return FALSE;

    if ( IS_AFFECTED( ch, AFF_DETECT_UNDEAD ) && is_undead(victim) )
        return TRUE;

    if ( room_is_dark( ch, ch->in_room ) && IS_AFFECTED(ch, AFF_INFRARED) && !IS_AFFECTED(ch, AFF_DARK_VISION) && !IS_SET(victim->form, FORM_WARM) )
        return FALSE;

    if( room_is_dark( ch, ch->in_room ) && !IS_AFFECTED( ch, AFF_DARK_VISION))
    {
        if( !IS_AFFECTED(ch, AFF_INFRARED) || !IS_SET(victim->form, FORM_WARM) )
            return FALSE;
    }

    if ( (  is_undead(ch) && !IS_AFFECTED( ch, AFF_DETECT_INVIS ) && IS_AFFECTED( victim, AFF_UNDEAD_INVIS ) )
            ||   ( !is_undead(ch) && !IS_AFFECTED( ch, AFF_DETECT_INVIS ) && IS_AFFECTED( victim, AFF_INVISIBLE ) ) )
        return FALSE;

    if ( IS_AFFECTED(victim, AFF_ANIMAL_INVIS) && IS_SET(race_table[GET_RACE(ch)].type, ANIMAL) && !IS_AFFECTED(ch, AFF_DETECT_INVIS) )
        return FALSE;

    if ( IS_AFFECTED(victim, AFF_HIDE) && !IS_AFFECTED(ch, AFF_DETECT_HIDDEN) && victim->fighting == NULL)
        return FALSE;

    if ( ( IS_AFFECTED(victim,AFF_HIDE) || ( IS_AFFECTED(victim,AFF_INVISIBLE) && !is_undead(ch) ) || ( IS_AFFECTED(victim,AFF_UNDEAD_INVIS) && is_undead(ch) ) || ( IS_AFFECTED(victim, AFF_ANIMAL_INVIS) && IS_SET(race_table[GET_RACE(ch)].type, ANIMAL) ) )
            && IS_AFFECTED(victim,AFF_NONDETECTION)
            && !IS_AFFECTED(ch,AFF_PIERCING_SIGHT))
        return FALSE;

    if( is_affected( ch, gsn_dazzling_flash) && affect_find( ch->affected, gsn_dazzling_flash)->level == 0 &&
            is_affected( victim, gsn_dazzling_flash) &&
            !IS_AFFECTED( ch, AFF_PERFECT_SENSES) && ( !IS_AFFECTED(ch,AFF_DETECT_INVIS) || IS_AFFECTED(victim,AFF_NONDETECTION)) )
    {
        for( dazzling = victim->affected ; dazzling; dazzling = dazzling->next )
        {
            if( dazzling->level == 1 && affect_find( ch->affected, gsn_dazzling_flash)->modifier == dazzling->modifier )
                return FALSE;
        }
    }
    
    if( affect_find( ch->affected, gsn_scrying_shield) && affect_find( ch->affected, gsn_scrying_shield)->level == 1 && IS_AFFECTED( victim, AFF_SCRYING_SHIELD) )
    {
        return FALSE;
    }

    return TRUE;
}


/*
 * True if char can see obj.
 */
bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    OBJ_DATA *in_obj;

    if ( !ch || !obj )
        return FALSE;

    if ( !IS_NPC(ch) && EXT_IS_SET(ch->act, PLR_HOLYLIGHT) )
        return TRUE;

    if ( IS_OBJ_STAT(obj, ITEM_INVIS)
            &&   !IS_AFFECTED(ch, AFF_DETECT_INVIS) )
        return FALSE;

    if( IS_NPC(ch) && ch->pIndexData->vnum == 3 )
        return TRUE;

    if ( IS_OBJ_STAT(obj,ITEM_VIS_DEATH))
        return FALSE;

    if ( IS_AFFECTED( ch, AFF_BLIND ) && !IS_AFFECTED(ch,AFF_PERFECT_SENSES) && obj->item_type != ITEM_POTION)
        return FALSE;

    if ( IS_OBJ_STAT(obj, ITEM_INVIS)
            &&   !IS_AFFECTED(ch, AFF_DETECT_INVIS) )
        return FALSE;

    if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
        return TRUE;

    if ( IS_OBJ_STAT(obj,ITEM_GLOW))
        return TRUE;

    for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj );

    if( in_obj->carried_by == ch )
        return TRUE;

    if ( ch->in_room && room_is_dark(ch, ch->in_room ) && !IS_AFFECTED(ch, AFF_DARK_VISION) )
        return FALSE;

    return TRUE;
}

bool check_nodrop_containers(CHAR_DATA *ch, OBJ_DATA *obj_list)
{
    OBJ_DATA *obj;
    bool can_drop = TRUE;

    for(obj = obj_list; obj; obj=obj->next_content)
    {
        if (obj->item_type != ITEM_CONTAINER &&
                IS_OBJ_STAT(obj, ITEM_NODROP) )
            can_drop = FALSE;

        if(obj->item_type == ITEM_CONTAINER)
            can_drop |= check_nodrop_containers(ch,obj->contains);
    }

    return can_drop;
}

/*
 * True if char can drop obj.
 */
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if(!IS_NPC(ch) && obj->item_type == ITEM_CONTAINER)
        return check_nodrop_containers(ch, obj->contains);

    if ( !IS_OBJ_STAT(obj, ITEM_NODROP) )
        return TRUE;

    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
        return TRUE;

    return FALSE;
}

/*
 * Return ascii name of an affect location.
 */
char *affect_loc_name( int location )
{
    switch ( location )
    {
        case APPLY_NONE:		return "co¶";
        case APPLY_STR:			return "si³ê";
        case APPLY_CHA:			return "charyzmê";
        case APPLY_LUC:			return "szczê¶cie";
        case APPLY_DEX:			return "zrêczno¶æ";
        case APPLY_INT:			return "inteligencjê";
        case APPLY_WIS:			return "wiedzê";
        case APPLY_CON:			return "konydycjê";
        case APPLY_SEX:				return "p³eæ";
        case APPLY_CLASS:			return "klasê";
        case APPLY_LEVEL:			return "poziom";
        case APPLY_AGE:				return "wiek";
        case APPLY_RESIST:			return "odporno¶æ";
        case APPLY_HIT:				return "punkty ¿ycia";
        case APPLY_MOVE:			return "punkty ruchu";
        case APPLY_GOLD:			return "z³oto";
        case APPLY_EXP:				return "do¶wiadczenie";
        case APPLY_AC:				return "klasê pancerza";
        case APPLY_HITROLL:			return "szansê trafienia";
        case APPLY_DAMROLL:			return "zadawane obra¿enia";

        case APPLY_SAVING_DEATH:	return "ochronê przeciw ¶mierci";
        case APPLY_SAVING_ROD:		return "ochronê przed ró¿d¿kami";
        case APPLY_SAVING_PETRI:	return "ochronê przeciw skamienieniu";
        case APPLY_SAVING_BREATH:	return "ochronê przed wyziewami";
        case APPLY_SAVING_SPELL:	return "ochronê przed czarami";

        case APPLY_SPELL_AFFECT:	return "co¶";
        case APPLY_SKILL:			return "umiejêtno¶æ";
        case APPLY_MEMMING:			return "ilo¶æ zapamiêtywanych czarów";
        case APPLY_LANG:			return "znajomo¶æ jêzyka";
        case APPLY_HEIGHT:			return "wielko¶æ";
        case APPLY_WEIGHT:			return "wagê";
                                    /*
                                       zmieni³em na polskie, w razie czego zostawiam wersjê orginaln±
                                       case APPLY_STR:		return "strength";
                                       case APPLY_CHA:		return "charisma";
                                       case APPLY_LUC:		return "luck";
                                       case APPLY_DEX:		return "dexterity";
                                       case APPLY_INT:		return "intelligence";
                                       case APPLY_WIS:		return "wisdom";
                                       case APPLY_CON:		return "constitution";
                                       case APPLY_SEX:		return "sex";
                                       case APPLY_CLASS:		return "class";
                                       case APPLY_LEVEL:		return "level";
                                       case APPLY_AGE:		return "age";
                                       case APPLY_RESIST:		return "resist";
                                       case APPLY_HIT:		return "hp";
                                       case APPLY_MOVE:		return "moves";
                                       case APPLY_GOLD:		return "gold";
                                       case APPLY_EXP:		return "experience";
                                       case APPLY_AC:		return "armor class";
                                       case APPLY_HITROLL:		return "hit roll";
                                       case APPLY_DAMROLL:		return "damage roll";

                                       case APPLY_SAVING_DEATH:	return "save vs death";
                                       case APPLY_SAVING_ROD:	return "save vs rod";
                                       case APPLY_SAVING_PETRI:	return "save vs petrification";
                                       case APPLY_SAVING_BREATH:	return "save vs breath";
                                       case APPLY_SAVING_SPELL:	return "save vs spell";

                                       case APPLY_SPELL_AFFECT:	return "none";
                                       case APPLY_SKILL:		return "skill";
                                       case APPLY_MEMMING:		return "spellmem";
                                       case APPLY_LANG:		return "language";
                                       case APPLY_HEIGHT:		return "height";
                                       case APPLY_WEIGHT:		return "weight";
                                     */
    }

    bug( "Affect_location_name: unknown location %d.", location );
    //	return "(unknown)";
    return "co¶";
}


/*
 * Return ascii name of an affect bit vector.
 */
char *affect_bit_name( CHAR_DATA *ch, BITVECT_DATA * vector )
{
    static char buf[512];

    if ( !ch && ( !vector || vector == &AFF_NONE ) ) return "none";

    buf[0] = '\0';

    if ( ch )
        return ext_flag_string( affect_flags, ch->affected_by );
    else
        return ext_bit_name( affect_flags, vector );

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *comm_bit_name(int comm_flags)
{
    static char buf[512];

    buf[0] = '\0';

    if (comm_flags & COMM_QUIET		) strcat(buf, " quiet");
    if (comm_flags & COMM_DEAF		) strcat(buf, " deaf");
    if (comm_flags & COMM_NOWIZ		) strcat(buf, " no_wiz");
    if (comm_flags & COMM_NOMUSIC	) strcat(buf, " no_music");
    if (comm_flags & COMM_COMPACT	) strcat(buf, " compact");
    if (comm_flags & COMM_BRIEF		) strcat(buf, " brief");
    if (comm_flags & COMM_PROMPT	) strcat(buf, " prompt");
    if (comm_flags & COMM_COMBINE	) strcat(buf, " combine");
    if (comm_flags & COMM_NOEMOTE	) strcat(buf, " no_emote");
    if (comm_flags & COMM_NOSHOUT	) strcat(buf, " no_shout");
    if (comm_flags & COMM_NOTELL	) strcat(buf, " no_tell");
    if (comm_flags & COMM_HINT	    ) strcat(buf, " hint" );
#ifdef ENABLE_SHOW_LFG
    if (comm_flags & COMM_LFG	    ) strcat(buf, " lfg" );
#endif
#ifdef ENABLE_SHOW_ON_WHO
    if (comm_flags & COMM_SHOW_ON_WHO) strcat(buf, " showwho" );
#endif
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *wear_bit_name(int wear_flags)
{
    static char buf[512];

    buf [0] = '\0';
    if (wear_flags & ITEM_TAKE		) strcat(buf, " take");
    if (wear_flags & ITEM_WEAR_FINGER	) strcat(buf, " finger");
    if (wear_flags & ITEM_WEAR_NECK	) strcat(buf, " neck");
    if (wear_flags & ITEM_WEAR_BODY	) strcat(buf, " torso");
    if (wear_flags & ITEM_WEAR_HEAD	) strcat(buf, " head");
    if (wear_flags & ITEM_WEAR_LEGS	) strcat(buf, " legs");
    if (wear_flags & ITEM_WEAR_FEET	) strcat(buf, " feet");
    if (wear_flags & ITEM_WEAR_HANDS	) strcat(buf, " hands");
    if (wear_flags & ITEM_WEAR_ARMS	) strcat(buf, " arms");
    if (wear_flags & ITEM_WEAR_SHIELD	) strcat(buf, " shield");
    if (wear_flags & ITEM_WEAR_ABOUT	) strcat(buf, " body");
    if (wear_flags & ITEM_WEAR_WAIST	) strcat(buf, " waist");
    if (wear_flags & ITEM_WEAR_WRIST	) strcat(buf, " wrist");
    if (wear_flags & ITEM_WIELD		) strcat(buf, " wield");
    if (wear_flags & ITEM_HOLD		) strcat(buf, " hold");
    if (wear_flags & ITEM_WEAR_LIGHT	) strcat(buf, " light");
    if (wear_flags & ITEM_WEAR_FLOAT	) strcat(buf, " float");
    if (wear_flags & ITEM_INSTRUMENT	) strcat(buf, " instrument");
    if (wear_flags & ITEM_WEAR_EAR	) strcat(buf, " ear");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *form_bit_name(int form_flags)
{
    static char buf[512];

    buf[0] = '\0';
    if (form_flags & FORM_POISON	) strcat(buf, " poison");
    else if (form_flags & FORM_EDIBLE	) strcat(buf, " edible");
    if (form_flags & FORM_MAGICAL	) strcat(buf, " magical");
    if (form_flags & FORM_INSTANT_DECAY	) strcat(buf, " instant_rot");
    if (form_flags & FORM_OTHER		) strcat(buf, " other");
    if (form_flags & FORM_ANIMAL	) strcat(buf, " animal");
    if (form_flags & FORM_SENTIENT	) strcat(buf, " sentient");
    if (form_flags & FORM_UNDEAD	) strcat(buf, " undead");
    if (form_flags & FORM_CONSTRUCT	) strcat(buf, " construct");
    if (form_flags & FORM_MIST		) strcat(buf, " mist");
    if (form_flags & FORM_INTANGIBLE	) strcat(buf, " intangible");
    if (form_flags & FORM_BIPED		) strcat(buf, " biped");
    if (form_flags & FORM_CENTAUR	) strcat(buf, " centaur");
    if (form_flags & FORM_INSECT	) strcat(buf, " insect");
    if (form_flags & FORM_SPIDER	) strcat(buf, " spider");
    if (form_flags & FORM_CRUSTACEAN	) strcat(buf, " crustacean");
    if (form_flags & FORM_WORM		) strcat(buf, " worm");
    if (form_flags & FORM_BLOB		) strcat(buf, " blob");
    if (form_flags & FORM_MAMMAL	) strcat(buf, " mammal");
    if (form_flags & FORM_BIRD		) strcat(buf, " bird");
    if (form_flags & FORM_REPTILE	) strcat(buf, " reptile");
    if (form_flags & FORM_SNAKE		) strcat(buf, " snake");
    if (form_flags & FORM_DRAGON	) strcat(buf, " dragon");
    if (form_flags & FORM_AMPHIBIAN	) strcat(buf, " amphibian");
    if (form_flags & FORM_FISH		) strcat(buf, " fish");
    if (form_flags & FORM_COLD_BLOOD 	) strcat(buf, " cold_blooded");
    if (form_flags & FORM_WARM	 	) strcat(buf, " warm");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *part_bit_name(int part_flags)
{
    static char buf[512];

    buf[0] = '\0';
    if (part_flags & PART_HEAD		) strcat(buf, " head");
    if (part_flags & PART_ARMS		) strcat(buf, " arms");
    if (part_flags & PART_LEGS		) strcat(buf, " legs");
    if (part_flags & PART_HEART		) strcat(buf, " heart");
    if (part_flags & PART_BRAINS	) strcat(buf, " brains");
    if (part_flags & PART_GUTS		) strcat(buf, " guts");
    if (part_flags & PART_HANDS		) strcat(buf, " hands");
    if (part_flags & PART_FEET		) strcat(buf, " feet");
    if (part_flags & PART_FINGERS	) strcat(buf, " fingers");
    if (part_flags & PART_EAR		) strcat(buf, " ears");
    if (part_flags & PART_EYE		) strcat(buf, " eyes");
    if (part_flags & PART_LONG_TONGUE	) strcat(buf, " long_tongue");
    if (part_flags & PART_EYESTALKS	) strcat(buf, " eyestalks");
    if (part_flags & PART_TENTACLES	) strcat(buf, " tentacles");
    if (part_flags & PART_FINS		) strcat(buf, " fins");
    if (part_flags & PART_WINGS		) strcat(buf, " wings");
    if (part_flags & PART_TAIL		) strcat(buf, " tail");
    if (part_flags & PART_BONES		) strcat(buf, " bones");
    if (part_flags & PART_CLAWS		) strcat(buf, " claws");
    if (part_flags & PART_FANGS		) strcat(buf, " fangs");
    if (part_flags & PART_HORNS		) strcat(buf, " horns");
    if (part_flags & PART_SCALES	) strcat(buf, " scales");
    if (part_flags & PART_SKIN		) strcat(buf, " skin");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *weapon_bit_name(int weapon_flags)
{
    static char buf[512];

    buf[0] = '\0';
    if (weapon_flags & WEAPON_FLAMING	) strcat(buf, " flaming");
    if (weapon_flags & WEAPON_FROST	) strcat(buf, " frost");
    if (weapon_flags & WEAPON_VAMPIRIC	) strcat(buf, " vampiric");
    if (weapon_flags & WEAPON_SHARP	) strcat(buf, " sharp");
    if (weapon_flags & WEAPON_VORPAL	) strcat(buf, " vorpal");
    if (weapon_flags & WEAPON_TWO_HANDS ) strcat(buf, " two-handed");
    if (weapon_flags & WEAPON_SHOCKING 	) strcat(buf, " shocking");
    if (weapon_flags & WEAPON_POISON	) strcat(buf, " poison");
    if (weapon_flags & WEAPON_DISPEL    ) strcat(buf, " dispeller");
    if (weapon_flags & WEAPON_PRIMARY	) strcat(buf, " primary");
    if (weapon_flags & WEAPON_TOXIC		) strcat(buf, " toxic");
    if (weapon_flags & WEAPON_SACRED	) strcat(buf, " sacred");
    if (weapon_flags & WEAPON_RESONANT	) strcat(buf, " resonant");
    if (weapon_flags & WEAPON_KEEN ) strcat(buf, " keen");
    if (weapon_flags & WEAPON_THUNDERING ) strcat (buf, " thundering");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *cont_bit_name( int cont_flags)
{
    static char buf[512];

    buf[0] = '\0';

    if (cont_flags & CONT_CLOSEABLE	) strcat(buf, " closable");
    if (cont_flags & CONT_PICKPROOF	) strcat(buf, " pickproof");
    if (cont_flags & CONT_CLOSED	) strcat(buf, " closed");
    if (cont_flags & CONT_LOCKED	) strcat(buf, " locked");

    return (buf[0] != '\0' ) ? buf+1 : "none";
}


/*
 * Config Colour stuff
 */
void default_colour( CHAR_DATA *ch )
{
    if( IS_NPC( ch ) )
        return;

    if( !ch->pcdata )
        return;

    ch->pcdata->text[1]		= ( WHITE );
    ch->pcdata->auction[1]	= ( YELLOW );
    ch->pcdata->auction_text[1]	= ( WHITE );
    ch->pcdata->gossip[1]	= ( MAGENTA );
    ch->pcdata->gossip_text[1]	= ( MAGENTA );
    ch->pcdata->music[1]	= ( RED );
    ch->pcdata->music_text[1]	= ( RED );
    ch->pcdata->question[1]	= ( GREEN );
    ch->pcdata->question_text[1] = ( GREEN );
    ch->pcdata->answer[1]	= ( YELLOW );
    ch->pcdata->answer_text[1]	= ( WHITE );
    ch->pcdata->quote[1]	= ( GREEN );
    ch->pcdata->quote_text[1]	= ( GREEN );
    ch->pcdata->immtalk_text[1]	= ( CYAN );
    ch->pcdata->immtalk_type[1]	= ( YELLOW );
    ch->pcdata->info[1]		= ( YELLOW );
    ch->pcdata->tell[1]		= ( GREEN );
    ch->pcdata->tell_text[1]	= ( GREEN );
    ch->pcdata->say[1]		= ( GREEN );
    ch->pcdata->say_text[1]	= ( GREEN );
    ch->pcdata->reply[1]	= ( GREEN );
    ch->pcdata->reply_text[1]	= ( GREEN );
    //ch->pcdata->gtell_text[1]	= ( GREEN );
    //ch->pcdata->gtell_type[1]	= ( RED );
    ch->pcdata->gtell_text[1]	= ( YELLOW );
    ch->pcdata->gtell_type[1]	= ( YELLOW );
    ch->pcdata->wiznet[1]	= ( GREEN );
    ch->pcdata->room_title[1]	= ( CYAN );
    ch->pcdata->room_text[1]	= ( WHITE );
    ch->pcdata->room_exits[1]	= ( GREEN );
    ch->pcdata->room_things[1]	= ( CYAN );
    ch->pcdata->shout[1]		= ( CYAN );
    ch->pcdata->shout_text[1]	= ( CYAN );
    ch->pcdata->yell[1]		= ( CYAN );
    ch->pcdata->yell_text[1]	= ( CYAN );
    ch->pcdata->prompt[1]	= ( CYAN );
    ch->pcdata->fight_death[1]	= ( RED );
    ch->pcdata->fight_yhit[1]	= ( GREEN );
    ch->pcdata->fight_ohit[1]	= ( YELLOW );
    ch->pcdata->fight_thit[1]	= ( RED );
    ch->pcdata->fight_skill[1]	= ( WHITE );
    ch->pcdata->fight_trick[1]  = ( WHITE );
    ch->pcdata->fight_spell[1]  = ( WHITE );
    ch->pcdata->text[0]		= ( NORMAL );
    ch->pcdata->auction[0]	= ( BRIGHT );
    ch->pcdata->auction_text[0]	= ( BRIGHT );
    ch->pcdata->gossip[0]	= ( NORMAL );
    ch->pcdata->gossip_text[0]	= ( BRIGHT );
    ch->pcdata->music[0]	= ( NORMAL );
    ch->pcdata->music_text[0]	= ( BRIGHT );
    ch->pcdata->question[0]	= ( NORMAL );
    ch->pcdata->question_text[0] = ( BRIGHT );
    ch->pcdata->answer[0]	= ( BRIGHT );
    ch->pcdata->answer_text[0]	= ( BRIGHT );
    ch->pcdata->quote[0]	= ( NORMAL );
    ch->pcdata->quote_text[0]	= ( BRIGHT );
    ch->pcdata->immtalk_text[0]	= ( NORMAL );
    ch->pcdata->immtalk_type[0]	= ( NORMAL );
    ch->pcdata->info[0]		= ( NORMAL );
    ch->pcdata->say[0]		= ( NORMAL );
    ch->pcdata->say_text[0]	= ( BRIGHT );
    ch->pcdata->tell[0]		= ( NORMAL );
    ch->pcdata->tell_text[0]	= ( BRIGHT );
    ch->pcdata->reply[0]	= ( NORMAL );
    ch->pcdata->reply_text[0]	= ( BRIGHT );
    ch->pcdata->gtell_text[0]	= ( BRIGHT );
    ch->pcdata->gtell_type[0]	= ( NORMAL );
    ch->pcdata->wiznet[0]	= ( NORMAL );
    ch->pcdata->room_title[0]	= ( NORMAL );
    ch->pcdata->room_text[0]	= ( NORMAL );
    ch->pcdata->room_exits[0]	= ( NORMAL );
    ch->pcdata->room_things[0]	= ( NORMAL );
    ch->pcdata->shout[0]		= ( NORMAL );
    ch->pcdata->shout_text[0]	= ( BRIGHT );
    ch->pcdata->yell[0]		= ( NORMAL );
    ch->pcdata->yell_text[0]	= ( BRIGHT );
    ch->pcdata->prompt[0]	= ( NORMAL );
    ch->pcdata->fight_death[0]	= ( NORMAL );
    ch->pcdata->fight_yhit[0]	= ( NORMAL );
    ch->pcdata->fight_ohit[0]	= ( NORMAL );
    ch->pcdata->fight_thit[0]	= ( NORMAL );
    ch->pcdata->fight_skill[0]	= ( NORMAL );
    ch->pcdata->fight_trick[0]  = ( NORMAL );
    ch->pcdata->fight_spell[0]  = ( NORMAL );
    ch->pcdata->text[2]		= 0;
    ch->pcdata->auction[2]	= 0;
    ch->pcdata->auction_text[2]	= 0;
    ch->pcdata->gossip[2]	= 0;
    ch->pcdata->gossip_text[2]	= 0;
    ch->pcdata->music[2]	= 0;
    ch->pcdata->music_text[2]	= 0;
    ch->pcdata->question[2]	= 0;
    ch->pcdata->question_text[2] = 0;
    ch->pcdata->answer[2]	= 0;
    ch->pcdata->answer_text[2]	= 0;
    ch->pcdata->quote[2]	= 0;
    ch->pcdata->quote_text[2]	= 0;
    ch->pcdata->immtalk_text[2]	= 0;
    ch->pcdata->immtalk_type[2]	= 0;
    ch->pcdata->info[2]		= 1;
    ch->pcdata->say[2]		= 0;
    ch->pcdata->say_text[2]	= 0;
    ch->pcdata->tell[2]		= 0;
    ch->pcdata->tell_text[2]	= 0;
    ch->pcdata->reply[2]	= 0;
    ch->pcdata->reply_text[2]	= 0;
    ch->pcdata->gtell_text[2]	= 0;
    ch->pcdata->gtell_type[2]	= 0;
    ch->pcdata->wiznet[2]	= 0;
    ch->pcdata->room_title[2]	= 0;
    ch->pcdata->room_text[2]	= 0;
    ch->pcdata->room_exits[2]	= 0;
    ch->pcdata->room_things[2]	= 0;
    ch->pcdata->prompt[2]	= 0;
    ch->pcdata->fight_death[2]	= 0;
    ch->pcdata->fight_yhit[2]	= 0;
    ch->pcdata->fight_ohit[2]	= 0;
    ch->pcdata->fight_thit[2]	= 0;
    ch->pcdata->fight_skill[2]	= 0;
    ch->pcdata->fight_trick[2]  = 0;
    ch->pcdata->fight_spell[2]  = 0;

    return;
}

void all_colour( CHAR_DATA *ch, char *argument )
{
    char	buf[  100 ];
    char	buf2[ 100 ];
    int		colour;
    int		bright;

    if( IS_NPC( ch ) || !ch->pcdata )
        return;

    if( !*argument )
        return;

    if( !str_prefix( argument, "red" ) )
    {
        colour = ( RED );
        bright = NORMAL;
        sprintf( buf2, "Red" );
    }
    if( !str_prefix( argument, "hi-red" ) )
    {
        colour = ( RED );
        bright = BRIGHT;
        sprintf( buf2, "Red" );
    }
    else if( !str_prefix( argument, "green" ) )
    {
        colour = ( GREEN );
        bright = NORMAL;
        sprintf( buf2, "Green" );
    }
    else if( !str_prefix( argument, "hi-green" ) )
    {
        colour = ( GREEN );
        bright = BRIGHT;
        sprintf( buf2, "Green" );
    }
    else if( !str_prefix( argument, "yellow" ) )
    {
        colour = ( YELLOW );
        bright = NORMAL;
        sprintf( buf2, "Yellow" );
    }
    else if( !str_prefix( argument, "hi-yellow" ) )
    {
        colour = ( YELLOW );
        bright = BRIGHT;
        sprintf( buf2, "Yellow" );
    }
    else if( !str_prefix( argument, "blue" ) )
    {
        colour = ( BLUE );
        bright = NORMAL;
        sprintf( buf2, "Blue" );
    }
    else if( !str_prefix( argument, "hi-blue" ) )
    {
        colour = ( BLUE );
        bright = BRIGHT;
        sprintf( buf2, "Blue" );
    }
    else if( !str_prefix( argument, "magenta" ) )
    {
        colour = ( MAGENTA );
        bright = NORMAL;
        sprintf( buf2, "Magenta" );
    }
    else if( !str_prefix( argument, "hi-magenta" ) )
    {
        colour = ( MAGENTA );
        bright = BRIGHT;
        sprintf( buf2, "Magenta" );
    }
    else if( !str_prefix( argument, "cyan" ) )
    {
        colour = ( CYAN );
        bright = NORMAL;
        sprintf( buf2, "Cyan" );
    }
    else if( !str_prefix( argument, "hi-cyan" ) )
    {
        colour = ( CYAN );
        bright = BRIGHT;
        sprintf( buf2, "Cyan" );
    }
    else if( !str_prefix( argument, "white" ) )
    {
        colour = ( WHITE );
        bright = NORMAL;
        sprintf( buf2, "White" );
    }
    else if( !str_prefix( argument, "hi-white" ) )
    {
        colour = ( WHITE );
        bright = BRIGHT;
        sprintf( buf2, "White" );
    }
    else if( !str_prefix( argument, "grey" ) )
    {
        colour = ( BLACK );
        bright = BRIGHT;
        sprintf( buf2, "White" );
    }
    else
    {
        send_to_char_bw( "Unrecognised colour, unchanged.\n\r", ch );
        return;
    }

    ch->pcdata->text[1]		= colour;
    ch->pcdata->auction[1]	= colour;
    ch->pcdata->gossip[1]	= colour;
    ch->pcdata->music[1]	= colour;
    ch->pcdata->question[1]	= colour;
    ch->pcdata->answer[1]	= colour;
    ch->pcdata->quote[1]	= colour;
    ch->pcdata->quote_text[1]	= colour;
    ch->pcdata->immtalk_text[1]	= colour;
    ch->pcdata->immtalk_type[1]	= colour;
    ch->pcdata->info[1]		= colour;
    ch->pcdata->say[1]		= colour;
    ch->pcdata->say_text[1]	= colour;
    ch->pcdata->tell[1]		= colour;
    ch->pcdata->tell_text[1]	= colour;
    ch->pcdata->reply[1]	= colour;
    ch->pcdata->reply_text[1]	= colour;
    ch->pcdata->gtell_text[1]	= colour;
    ch->pcdata->gtell_type[1]	= colour;
    ch->pcdata->wiznet[1]	= colour;
    ch->pcdata->room_title[1]	= colour;
    ch->pcdata->room_text[1]	= colour;
    ch->pcdata->room_exits[1]	= colour;
    ch->pcdata->room_things[1]	= colour;
    ch->pcdata->prompt[1]	= colour;
    ch->pcdata->fight_death[1]	= colour;
    ch->pcdata->fight_yhit[1]	= colour;
    ch->pcdata->fight_ohit[1]	= colour;
    ch->pcdata->fight_thit[1]	= colour;
    ch->pcdata->fight_skill[1]	= colour;
    ch->pcdata->fight_trick[1]  = colour;
    ch->pcdata->fight_spell[1]  = colour;
    ch->pcdata->text[0]		= bright;
    ch->pcdata->auction[0]	= bright;
    ch->pcdata->gossip[0]	= bright;
    ch->pcdata->music[0]	= bright;
    ch->pcdata->question[0]	= bright;
    ch->pcdata->answer[0]	= bright;
    ch->pcdata->quote[0]	= bright;
    ch->pcdata->quote_text[0]	= bright;
    ch->pcdata->immtalk_text[0]	= bright;
    ch->pcdata->immtalk_type[0]	= bright;
    ch->pcdata->info[0]		= bright;
    ch->pcdata->say[0]		= bright;
    ch->pcdata->say_text[0]	= bright;
    ch->pcdata->tell[0]		= bright;
    ch->pcdata->tell_text[0]	= bright;
    ch->pcdata->reply[0]	= bright;
    ch->pcdata->reply_text[0]	= bright;
    ch->pcdata->gtell_text[0]	= bright;
    ch->pcdata->gtell_type[0]	= bright;
    ch->pcdata->wiznet[0]	= bright;
    ch->pcdata->room_title[0]	= bright;
    ch->pcdata->room_text[0]	= bright;
    ch->pcdata->room_exits[0]	= bright;
    ch->pcdata->room_things[0]	= bright;
    ch->pcdata->prompt[0]	= bright;
    ch->pcdata->fight_death[0]	= bright;
    ch->pcdata->fight_yhit[0]	= bright;
    ch->pcdata->fight_ohit[0]	= bright;
    ch->pcdata->fight_thit[0]	= bright;
    ch->pcdata->fight_skill[0]	= bright;
    ch->pcdata->fight_trick[0]  = bright;
    ch->pcdata->fight_spell[0]  = bright;

    sprintf( buf, "All Colour settings set to %s.\n\r", buf2 );
    send_to_char_bw( buf, ch );

    return;
}

bool can_move( CHAR_DATA *ch )
{
    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
        return TRUE;

    if ( IS_AFFECTED( ch, AFF_PARALYZE ) || !IS_AWAKE( ch )
            || IS_AFFECTED( ch, AFF_DAZE ) || IS_AFFECTED( ch, AFF_ZAKUTY )
            || IS_AFFECTED( ch, AFF_FORCE_FIELD ) || IS_AFFECTED( ch, AFF_ASTRAL_JOURNEY ) )
        return FALSE;

    return TRUE;
}

//Brohacz: funkcja sprawdzajaca, czy znamy czar, uzywana tylko do wylistowania ksiegi zaklec
bool knows_spell_book(CHAR_DATA *ch, sh_int sn )
{
    int class = -1, x;

    if(sn < 0 || sn > MAX_SKILL)
        return FALSE;

    if ( skill_table[sn].name == NULL || skill_table[sn].spell_fun == spell_null )
        return FALSE;

    class = get_caster(ch);

    if( class < 0 )
        return FALSE;

    if ( IS_NPC( ch ) )
    {
        for(x=0; x < 16; x++)
            if(ch->pIndexData->spells[x] == sn)
                return TRUE;

        return FALSE;
    }

    if( class == CLASS_MAG )
    {
        //szkola przeciwna
        if( ch->pcdata->mage_specialist >= 0 &&	school_table[ch->pcdata->mage_specialist].reverse_school & skill_table[sn].school )
            return FALSE;

        //specjalnosc mistrzostwa
        if( IS_SET( skill_table[sn].school, SpellSpec) )
        {
            if( ch->pcdata->mage_specialist < 0 )
                return FALSE;

            if( !( school_table[ch->pcdata->mage_specialist].flag & skill_table[sn].school ) )
                return FALSE;
        }

        //czarek tylko dla specjalistow i maga ogolnego
        if( IS_SET( skill_table[sn].school, SpellSpecAndGeneral) )
        {
            if( ch->pcdata->mage_specialist >= 0 && !( school_table[ch->pcdata->mage_specialist].flag & skill_table[sn].school ) )
                return FALSE;
        }
    }

    if(ch->pcdata->learned[sn] > 0)
        return TRUE;

    return FALSE;
}

//Funkcja sprawdzajaca, czy znamy czar i mamy wystarczajacy poziom, by moc go rzucic
bool knows_spell(CHAR_DATA *ch, sh_int sn )
{
    int class = -1, x;

    if(sn < 0 || sn > MAX_SKILL)
        return FALSE;

    if ( skill_table[sn].name == NULL || skill_table[sn].spell_fun == spell_null )
        return FALSE;

    class = get_caster(ch);

    if( class < 0 )
        return FALSE;

    if ( ch->level < skill_table[sn].skill_level[class] )
        return FALSE;

    if (IS_NPC(ch))
    {
        for(x=0; x < 16; x++)
            if(ch->pIndexData->spells[x] == sn)
                return TRUE;

        return FALSE;
    }

    if( class == CLASS_MAG )
    {
        //szkola przeciwna
        if( ch->pcdata->mage_specialist >= 0 &&	school_table[ch->pcdata->mage_specialist].reverse_school & skill_table[sn].school )
            return FALSE;

        //specjalnosc mistrzostwa
        if( IS_SET( skill_table[sn].school, SpellSpec) )
        {
            if( ch->pcdata->mage_specialist < 0 )
                return FALSE;

            if( !( school_table[ch->pcdata->mage_specialist].flag & skill_table[sn].school ) )
                return FALSE;
        }

        //czarek tylko dla specjalistow i maga ogolnego
        if( IS_SET( skill_table[sn].school, SpellSpecAndGeneral) )
        {
            if( ch->pcdata->mage_specialist >= 0 && !( school_table[ch->pcdata->mage_specialist].flag & skill_table[sn].school ) )
                return FALSE;
        }
    }

    if(ch->pcdata->learned[sn] > 0)
        return TRUE;

    return FALSE;
}

/* funkcja sprawdzajaca odpornosci na typy obrazen
 * przeliczajaca DAM_COSTAM na RESIST_COSTAM
 * i zmodyfikowana ilosc obrazen
 */
int check_resist(CHAR_DATA *ch, int dam_type, int dam)
{
    CHAR_DATA *tch;
    int damage, resist=-1;
    int res_dam = 0, res_magic = 0;
    bool magic = TRUE, type_damage = TRUE, found = FALSE;

    /* taa konwersja */
    switch (dam_type)
    {
        case(DAM_NONE):		return dam;
        case(DAM_BASH):		resist = RESIST_BASH;magic = FALSE;break;
        case(DAM_PIERCE):	resist = RESIST_PIERCE;magic = FALSE;break;
        case(DAM_SLASH):	resist = RESIST_SLASH;magic = FALSE;break;
        case(DAM_FIRE):		resist = RESIST_FIRE;break;
        case(DAM_COLD):		resist = RESIST_COLD;break;
        case(DAM_LIGHTNING):resist = RESIST_ELECTRICITY;break;
        case(DAM_ACID):		resist = RESIST_ACID;break;
        case(DAM_POISON):	resist = RESIST_POISON;break;
        case(DAM_NEGATIVE):	resist = RESIST_NEGATIVE;break;
        case(DAM_HOLY):		resist = RESIST_HOLY;break;
        case(DAM_ENERGY):	resist = RESIST_MAGIC;break;
        case(DAM_HARM):		resist = RESIST_MAGIC;break;
        case(DAM_MENTAL):	resist = RESIST_MENTAL;magic = FALSE;break;
        case(DAM_DISEASE):	resist = RESIST_MAGIC;break;
        case(DAM_DROWNING):	return dam;
        case(DAM_LIGHT):	resist = RESIST_MAGIC;break;
        case(DAM_CHARM):	resist = RESIST_CHARM;type_damage = FALSE;break;
        case(DAM_SOUND):	resist = RESIST_SOUND;break;
        default:		return dam;
    }

    if( IS_AFFECTED(ch, AFF_TROLL) && ch->hit <= 0 && resist != RESIST_FIRE && resist != RESIST_ACID )
    {
        return 0;
    }

    /* jesli magiczne to najpierw rzut na odpornosc magiczna*/
    if(magic)
    {
        res_magic = UMAX(ch->resists[RESIST_MAGIC],ch->resists[RESIST_ALL]);
        res_magic = UMIN( res_magic, 100 );
        if(res_magic > 0 && number_percent() < res_magic)
        {
            return 0;
        }
    }

    //ci co nie maja uszu nie dostaja od dzwieku
    if (dam_type == DAM_SOUND &&
            ( !IS_SET( race_table[ GET_RACE( ch ) ].parts, PART_EAR ) ||
              IS_SET( race_table[ GET_RACE( ch ) ].form, FORM_MAGICAL ) ||
              is_undead(ch) ||
              IS_SET( race_table[ GET_RACE( ch ) ].form, FORM_CONSTRUCT ) ||
              IS_SET( race_table[ GET_RACE( ch ) ].form, FORM_MIST ) ||
              IS_SET( race_table[ GET_RACE( ch ) ].form, FORM_BLOB ) ||
              IS_SET( race_table[ GET_RACE( ch ) ].form, FORM_DRAGON ) ) )
    {
        return 0;
    }

    //undeadzi nie dostaja od choroby i trucizny
    if (( dam_type == DAM_DISEASE || dam_type == DAM_POISON ) && is_undead(ch) )
    {
        return 0;
    }

    //undeadzi wchlaniaja negativa
    if (dam_type == DAM_NEGATIVE && is_undead(ch) )
    {
        if( IS_NPC(ch) )
        {
            heal_undead( ch, ch, (2*dam)/3 );
        }
        else
        {
            for( tch = ch->in_room->people; tch; tch = tch->next_in_room )
            {
                if( IS_NPC( tch ) && tch->pIndexData->vnum == MOB_VNUM_AVATAR && tch->fighting == ch )
                    found = TRUE;
            }

            if( !found )
                heal_undead( ch, ch, (2*dam)/3 );
        }
        return 0;
    }

    res_dam = UMAX(ch->resists[resist],ch->resists[RESIST_ALL]);
    res_dam = UMIN( res_dam, 100 );
    //naturalny resist 50% na dam_holy nie-undeadow
    if (dam_type == DAM_HOLY && !is_undead(ch) )
    {
        res_dam = UMIN( UMAX( res_dam, 50 ), 100 );
    }

    /* jesli to charm badz inne... to tylko TRUE/FALSE*/
    if(!type_damage)
    {
        if(res_dam > 0 && number_percent() < res_dam)
        {
            return 1;
        }
        return 0;
    }

    /* sprawdzane odpornosci i modyfikacja obrazen*/
    if(res_dam >= 0)
        res_dam = UMIN( res_dam, 100 );
    else
        res_dam = res_dam*5;

    if(res_dam == 100)
        damage = 0;
    else if(res_dam > 0)
        damage = UMAX(1,dam*(100-res_dam)/100);
    else if(res_dam == 0)
        return dam;
    else
        damage = dam*(100-res_dam)/100;

#ifdef INFO
    print_char( ch, "check_resist:damage: %d\n\r", damage );
#endif

    /**
     * aura of endurance
     */
    if ( resist == RESIST_FIRE || resist == RESIST_COLD || resist == RESIST_ELECTRICITY || resist == RESIST_ACID )
    {
        if ( has_aura( ch, FALSE ) )
        {
            if ( get_aura_sn( ch ) == 464 )
            {
                damage -= get_aura_modifier( ch );
#ifdef INFO
                print_char( ch, "check_resist:damage (aura of endurance): %d\n\r", damage );
#endif
            }
        }
    }

    return damage;
}

char *resist_name(int type)
{
    switch (type)
    {
        case(RESIST_ALL):	return "wszystko";
        case(RESIST_BASH):	return "bronie obuchowe";
        case(RESIST_PIERCE):	return "bronie k³uj±ce";
        case(RESIST_SLASH):	return "bronie tn±ce";
        case(RESIST_FIRE):	return "ogieñ";
        case(RESIST_COLD):	return "zimno";
        case(RESIST_ELECTRICITY):return "elektryczno¶æ";
        case(RESIST_ACID):	return "kwas";
        case(RESIST_POISON):	return "truciznê";
        case(RESIST_NEGATIVE):	return "energiê negatywn±";
        case(RESIST_MENTAL):	return "ataki mentalne";
        case(RESIST_HOLY):	return "energie pozytywn±";
        case(RESIST_CHARM):	return "zauroczenia";
        case(RESIST_SOUND):	return "d¼wiêk";
        case(RESIST_FEAR):	return "strach";
        case(RESIST_SUMMON):	return "przywo³ania";
        case(RESIST_MAGIC_FIRE):return "magiczny ogieñ";
        case(RESIST_MAGIC):	return "magiê";
        case(RESIST_MAGIC_WEAPON):return "magiczne bronie";
        default : return "nieznane";
    }
}

int resist_number(char *name)
{

    if(!str_prefix(name,"all"))
        return RESIST_ALL;
    else if(!str_prefix(name,"slash"))
        return RESIST_SLASH;
    else if(!str_prefix(name,"bash"))
        return RESIST_BASH;
    else if(!str_prefix(name,"pierce"))
        return RESIST_PIERCE;
    else if(!str_prefix(name,"magic"))
        return RESIST_MAGIC;
    else if(!str_prefix(name,"fire"))
        return RESIST_FIRE;
    else if(!str_prefix(name,"magic_fire"))
        return RESIST_MAGIC_FIRE;
    else if(!str_prefix(name,"electricity"))
        return RESIST_ELECTRICITY;
    else if(!str_prefix(name,"cold"))
        return RESIST_COLD;
    else if(!str_prefix(name,"acid"))
        return RESIST_ACID;
    else if(!str_prefix(name,"poison"))
        return RESIST_POISON;
    else if(!str_prefix(name,"magic_weapon"))
        return RESIST_MAGIC_WEAPON;
    else if(!str_prefix(name,"charm"))
        return RESIST_CHARM;
    else if(!str_prefix(name,"fear"))
        return RESIST_FEAR;
    else if(!str_prefix(name,"summon"))
        return RESIST_SUMMON;
    else if(!str_prefix(name,"negative"))
        return RESIST_NEGATIVE;
    else if(!str_prefix(name,"mental"))
        return RESIST_MENTAL;
    else if(!str_prefix(name,"holy"))
        return RESIST_HOLY;
    else if(!str_prefix(name,"sound"))
        return RESIST_SOUND;
    else
        return -1;
}

int check_magic_attack(CHAR_DATA *ch, CHAR_DATA *vch, OBJ_DATA *weapon )
{
    int magic=0;
    int to_hit=0, to_dam = 0;
    int specdam=0;

    if( !weapon )
    {
        if( !IS_NPC( ch ) )
        {
            if( is_affected(ch,gsn_magic_hands))
                return URANGE(1, ch->level/5, 5 );
            else
                return 0;
        }

        if( is_affected( ch, gsn_magic_fang ) )
            magic = 1;

        return UMAX( magic, ch->magical_damage );
    }

    /*
       Tener, 26-04-2008:
       specdam magic: bo czasami bronie s± magiczne czasami, np. na smoki
     */

    if ( weapon->item_type == ITEM_WEAPON )
        specdam = calculate_spec_damage( ch, vch, weapon, SD_TARGET_MAG );

    if( !IS_OBJ_STAT(weapon, ITEM_MAGIC) )
        return specdam;

    if( weapon->item_type == ITEM_WEAPON )
    {
        to_hit = weapon->value[5];
        to_dam = weapon->value[6];
        magic = UMAX( specdam, UMIN(to_hit, to_dam) );
    }
    else if ( weapon->item_type == ITEM_SHIELD )
    {
        to_hit = weapon->value[4];
        to_dam = weapon->value[5];
        magic = UMIN(to_hit, to_dam);
    }
    else
        magic = 0;

    return magic;
}

void rysand_poison_to_char(CHAR_DATA *victim, int poison)
{
    if( poison < 0 || poison > MAX_POISON )
        return;

    if ( !victim )
        return;

    /* umarlaki i golemy s± raczej odporne */
    if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) )
        return;

    /* jesli ma resista i udal sie resist, to trucizna nie zadziala */
    if(
            victim->resists[RESIST_POISON] > 0
            &&
            number_percent() < victim->resists[RESIST_POISON]
      )
        return;

    //poison wejdzie - komunikat:
    switch (number_range(0, 5))
    {
        case 0:
            send_to_char("Zauwa¿asz dziwne, ci±gle powiêkszaj±ce siê, plamy na skórze.\n\r",victim);
            break;
        case 1:
            send_to_char("Krêci ci siê w g³owie i czujesz nudno¶ci.\n\r",victim);
            break;
        case 2:
            send_to_char("Potwornie boli ciê brzuch. Czujesz, ¿e za chwilê wybuchnie.\n\r",victim);
            break;
        case 3:
            send_to_char("Czujesz, ¿e twoja krew zaczyna siê burzyæ, a ¿y³y staj± siê fioletowe\n\r",victim);
            break;
        case 4:
            send_to_char("Twoje oczy zaczynaj± ³zawiæ, a ze wszystkich otworów cia³a zaczyna s±czyæ siê ropa.\n\r",victim);
            break;
        case 5:
            send_to_char("Czujesz siê okropnie. Miê¶nie zaczynaj± paliæ ¿ywym ogniem, a skóra staje siê trupioblada.\n\r",victim);
            break;
    }

    AFFECT_DATA *paf = affect_find(victim->affected, gsn_poison);


    /* nie ma na sobie trucizny? luzik, juz dodajemy */
    if(paf == NULL )
    {
        AFFECT_DATA aff;
        paf = &aff;
    }
    else
    {
        //aktualizacja znalezionej trucizny - dodatkowe efekty, przed³u¿enie trwania
        //ze wzglêdu na pojedynczy affect efekty sumuj± siê, a czas trwania wszystkich
        //to czas trwania najd³u¿szego
        paf->level = UMAX(paf->level, poison);
        paf->duration = UMAX(paf->duration, dice(poison_table[poison].duration[0], poison_table[poison].duration[1]) + poison_table[poison].duration[2]);
        paf->rt_duration = 0;
        paf->extra_flags |= poison_table[poison].special_effects;

        if( IS_SET(paf->extra_flags, POISON_WEAKEN) )
        {
            AFFECT_DATA aff_poison;
            aff_poison.where	= TO_AFFECTS;
            aff_poison.type	= gsn_poison;
            aff_poison.level	= poison;
            aff_poison.duration = paf->duration;
            aff_poison.rt_duration = paf->rt_duration;
            aff_poison.location = APPLY_STR;
            aff_poison.modifier = - dice(2, 3);
            aff_poison.bitvector = &AFF_POISON;
            affect_to_char( victim, &aff_poison, NULL, TRUE  );
        }
        return;
    }

    //nowa trucizna
    paf->where	= TO_AFFECTS;
    paf->type	= gsn_poison;
    paf->level	= poison;
    paf->duration = dice(poison_table[poison].duration[0], poison_table[poison].duration[1]) + poison_table[poison].duration[2];
    paf->rt_duration = 0;
    paf->location = 0;
    paf->modifier = 0;
    paf->bitvector = &AFF_POISON;
    paf->extra_flags = poison_table[poison].special_effects;

    // weaken wchodzi od razu
    //Rysand - ustawiam na 3,5 - w koñcu to silna trucizna
    if( IS_SET(poison_table[poison].special_effects, POISON_WEAKEN) )
    {
        paf->location = APPLY_STR;
        paf->modifier = -dice(3, 5);
    }

    affect_to_char( victim, paf, NULL, TRUE  );

    update_poison(victim);
    return;
}

void rysand_update_poison(CHAR_DATA *victim)
{
    AFFECT_DATA *aff, *poison_aff = NULL, new_aff;
    int poison_damage = 0;
    int weight = 1;

    if( !IS_AFFECTED(victim, AFF_POISON ) )
        return;

    // umarlaki i golemy s± niewra¿liwe
    if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) )
    {
        affect_strip( victim, gsn_poison );
        return;
    }

    for( aff = victim->affected; aff; aff = aff->next )
    {
        if( aff->type == gsn_poison &&
                aff->bitvector == &AFF_POISON )
        {
            poison_aff = aff;
            break;
        }
    }

    int poison = 0;
    int counter = 0;
    for(counter = 0; counter <= MAX_POISON; counter++)
    {
        if(!poison_aff)
        {
            //je¶li nie ma affecta, to trucizna s³aba i jeden przebieg
            poison = 1;
            counter = MAX_POISON;
        }
        else
        {
            //brzydkie, ale nie wymaga przepisywania ca³o¶ci...
            if(
                    ((counter == 0) && IS_SET(poison_aff->extra_flags, POISON_SLOWERED))
                    ||
                    ((counter == 1) && IS_SET(poison_aff->extra_flags, POISON_WEAK))
                    ||
                    ((counter == 2) && IS_SET(poison_aff->extra_flags, POISON_NORMAL))
                    ||
                    ((counter == 3) && IS_SET(poison_aff->extra_flags, POISON_STRONG))
                    ||
                    ((counter == 4) && IS_SET(poison_aff->extra_flags, POISON_STRONGEST))
                    ||
                    ((counter == 5) && IS_SET(poison_aff->extra_flags, POISON_PARALIZING))
                    ||
                    ((counter == 6) && IS_SET(poison_aff->extra_flags, POISON_DEATH))
                    ||
                    ((counter == 7) && IS_SET(poison_aff->extra_flags, POISON_WEAKEN))
                    ||
                    ((counter == 8) && IS_SET(poison_aff->extra_flags, POISON_BLIND))
                    ||
                    ((counter == 9) && IS_SET(poison_aff->extra_flags, POISON_SLEEP))
              )
              {
                  poison = counter;
              }
            else //przelatujemy przez WSZYSTKIE trucizny
            {
                continue;
            }
        }

        if( !saves_spell_new(victim, skill_table[gsn_poison].save_type, skill_table[gsn_poison].save_mod , NULL, gsn_poison ))
        {
            int duration = 0;
            if(poison_aff)
            {
                duration = poison_aff->duration;
            }
            else
            {
                duration = dice(poison_table[poison].duration[0], poison_table[poison].duration[1]) + poison_table[poison].duration[2];
            }

            if( IS_SET(poison_table[poison].special_effects, POISON_DEATH) )
            {
                send_to_char("Czujesz potworny ból w klatce piersiowej, próbujesz z trudem z³apaæ oddech. Trucizna b³yskawicznie atakuje twoje cia³o, padasz na kolana. Nagle wszystko ciemnieje, a ból ... odchodzi.\n\r",	    victim);
                act("$n g³o¶no charcze, sinieje na twarzy nie mog±c z³apaæ oddechu. Nagle pada wydaj±c z siebie przera¿aj±cy jêk i nieruchomieje.",victim,NULL,NULL,TO_ROOM);
                int vnum = victim->in_room ? victim->in_room->vnum : 0;
                raw_kill(victim, victim);
                if ( !IS_NPC( victim ) )
                {
                    MURDER_LIST *tmp_death;
                    tmp_death = malloc( sizeof( *tmp_death ) );
                    tmp_death->name = str_dup( "¶mierteln± truciznê" );
                    tmp_death->char_level = 0;
                    tmp_death->victim_level = victim->level;
                    tmp_death->room = vnum;
                    tmp_death->time = current_time;
                    tmp_death->next = victim->pcdata->death_statistics.pkdeath_list;
                    victim->pcdata->death_statistics.pkdeath_list = tmp_death;
                }
                return;
            }

            if( IS_SET(poison_table[poison].special_effects, POISON_SLEEP) &&
                    !IS_AFFECTED(victim, AFF_SLEEP))
            {
                new_aff.where     = TO_AFFECTS;
                new_aff.type      = gsn_poison;
                new_aff.level     = 0;
                new_aff.duration  = duration;
                new_aff.rt_duration = 0;
                new_aff.location  = APPLY_NONE;
                new_aff.modifier  = 0;
                new_aff.bitvector = &AFF_SLEEP;
                affect_to_char( victim, &new_aff, NULL, TRUE  );

                if ( IS_AWAKE(victim) )
                {
                    victim->position = POS_SLEEPING;
                    send_to_char("¦wiat wokó³ ciebie zawirowa³, tracisz przytomno¶æ.\n\r", victim);

                    switch (victim->sex)
                    {
                        case 0:
                            act("$n pada na ziemiê nieprzytomne.",victim,NULL,NULL,TO_ROOM);
                            break;
                        case 1:
                            act("$n pada na ziemiê nieprzytomny.",victim,NULL,NULL,TO_ROOM);
                            break;
                        default:
                            act("$n pada na ziemiê nieprzytomna.",victim,NULL,NULL,TO_ROOM);
                            break;
                    }
                }
                return;
            }

            if( IS_SET(poison_table[poison].special_effects, POISON_BLIND) &&
                    !IS_AFFECTED(victim, AFF_BLIND)&&
                    !IS_AFFECTED(victim,AFF_PERFECT_SENSES))
            {
                new_aff.where     = TO_AFFECTS;
                new_aff.type      = gsn_poison;
                new_aff.level     = 0;
                new_aff.duration  = duration;
                new_aff.rt_duration = 0;
                new_aff.location  = APPLY_NONE;
                new_aff.modifier  = 0;
                new_aff.bitvector = &AFF_BLIND;
                affect_to_char( victim, &new_aff, NULL, TRUE );

                switch (victim->sex)
                {
                    case 0:
                        send_to_char("Nagle co¶ dzieje siê z twoimi oczami, wszystko ciemnieje... O¶lep³o¶!\n\r", victim);
                        act("$n wygl±da na o¶lepione.",victim,NULL,NULL,TO_ROOM);
                        break;
                    case 1:
                        send_to_char("Nagle co¶ dzieje siê z twoimi oczami, wszystko ciemnieje... O¶lep³e¶!\n\r", victim);
                        act("$n wygl±da na o¶lepionego.",victim,NULL,NULL,TO_ROOM);
                        break;
                    default:
                        send_to_char("Nagle co¶ dzieje siê z twoimi oczami, wszystko ciemnieje... O¶lep³a¶!\n\r", victim);
                        act("$n wygl±da na o¶lepion±.",victim,NULL,NULL,TO_ROOM);
                        break;
                }


                return;
            }

            if( IS_SET(poison_table[poison].special_effects, POISON_PARALIZING) &&
                    !IS_AFFECTED(victim, AFF_PARALYZE))
            {
                new_aff.where     = TO_AFFECTS;
                new_aff.type      = gsn_poison;
                new_aff.level     = 0;
                new_aff.duration  = duration;
                new_aff.rt_duration = 0;
                new_aff.location  = APPLY_NONE;
                new_aff.modifier  = 0;
                new_aff.bitvector = &AFF_PARALYZE;
                affect_to_char( victim, &new_aff, NULL, TRUE );

                send_to_char("Nagle tracisz kontrolê nad swoim cia³em.\n\r", victim);

                act("$n zatrzymuje siê w miejscu.",victim,NULL,NULL,TO_ROOM);
                return;
            }

            poison_damage = (dice(poison_table[poison].full_damage[1], poison_table[poison].full_damage[0]) + (poison_table[poison].full_damage[2])  );
        }
        else
        {
            poison_damage = (dice(poison_table[poison].saved_damage[1], poison_table[poison].saved_damage[0] ) + poison_table[poison].saved_damage[2]);
        }

        if(!poison_aff){
            //nie mam czasu na pierdolenie siê z debugowaniem
            //na builderze ¿arcie siê nie psuje?!
            //stary kod - dla trucizny z ¿arcia by³ poison = 1
            poison = 1;
            poison_damage = (dice(poison_table[poison].saved_damage[1], poison_table[poison].saved_damage[0] ) + poison_table[poison].saved_damage[2]);
        }
        else
        {
            //¶rednia wa¿ona (*100 w celu zachowania dok³adno¶ci)
            poison_damage = (int)((poison_damage *100.0) / (weight * 100.0));
        }
        //	char buf[200];
        //	sprintf(buf, "poison damage: %d", poison_damage);
        //	DEBUG_INFO(buf);

        poison_damage = URANGE(0, poison_damage, (get_max_hp(victim) * 30) /100);

        //	sprintf(buf, "final poison damage: %d", poison_damage);
        //	DEBUG_INFO(buf);


        if ( !is_affected(victim,gsn_perfect_self))
        {
            act( "$n trzêsie siê i cierpi.", victim, NULL, NULL, TO_ROOM );
        }

        send_to_char( "Trzêsiesz siê i cierpisz.\n\r", victim );
        int vnum = victim->in_room ? victim->in_room->vnum : 0;
        damage(victim, victim, poison_damage, gsn_poison, DAM_POISON, FALSE);

        if ( IS_AFFECTED( victim, AFF_MEDITATION ) )
        {
            print_char( victim, "Trucizna kr±¿±ca w krwi zak³óca medytacjê.\n\r" );
            affect_strip( victim, gsn_meditation );
        }
        if ( !IS_NPC( victim ) && !victim->in_room )
        {
            MURDER_LIST *tmp_death;
            tmp_death = malloc( sizeof( *tmp_death ) );
            tmp_death->name = str_dup( "truciznê" );
            tmp_death->char_level = 0;
            tmp_death->victim_level = victim->level;
            tmp_death->room = vnum;
            tmp_death->time = current_time;
            tmp_death->next = victim->pcdata->death_statistics.pkdeath_list;
            victim->pcdata->death_statistics.pkdeath_list = tmp_death;
        }
        return;
    }
}

/* zapodanie ch trucizny
 * jesli ma slabsza trucizne na sobie, zamieniamy na ta silniejsza
 * jesli smiertelna trucizna i nie udal sie rzut, die
 * jesli wyszedl rzut na resist, nie zapodajemy trucizny
 * dodawany paraliz, weaken
 */
void poison_to_char(CHAR_DATA *victim, int poison)
{
	AFFECT_DATA aff_poison, *paf;
	int chance;

	if( poison < 0 || poison > MAX_POISON )
		return;

	if ( !victim )
		return;

/* umarlaki i golemy s± raczej odporne */
	if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) )
			return;

/* jesli ma resista u udal sie resist, to trucizna nie zadziala */
	if( victim->resists[RESIST_POISON] > 0 &&
	number_percent() < victim->resists[RESIST_POISON] )
	return;

    /* nie ma na sobie trucizny? luzik, juz dodajemy */
	if( (paf = affect_find(victim->affected, gsn_poison) ) == NULL )
		paf = &aff_poison;  /* ma jakas kiepska trucizne juz? no to niech ma ta silniejsza */
    else
	{
		/* no ma juz silniejsza, dobrze jest */
		if( paf->level >= poison )
			return;

		paf->level	= poison;
		paf->duration = dice(poison_table[poison].duration[0], poison_table[poison].duration[1]) + poison_table[poison].duration[2];
		paf->rt_duration = 0;

		if( IS_SET(poison_table[poison].special_effects, POISON_WEAKEN) )
		{
		    aff_poison.where	= TO_AFFECTS;
	    	aff_poison.type	= gsn_poison;
	    	aff_poison.level	= poison;
	    	aff_poison.duration = paf->duration;
	    	aff_poison.rt_duration = paf->rt_duration;
	    	aff_poison.location = APPLY_STR;
	    	aff_poison.modifier = - dice(2, 3);
	    	aff_poison.bitvector = &AFF_POISON;
	    	affect_to_char( victim, &aff_poison, NULL, TRUE  );
        }

		return;
	}

    paf->where	= TO_AFFECTS;
    paf->type	= gsn_poison;
    paf->level	= poison;
    paf->duration = dice(poison_table[poison].duration[0], poison_table[poison].duration[1]) + poison_table[poison].duration[2];
    paf->rt_duration = 0;
    paf->location = 0;
    paf->modifier = 0;
    paf->bitvector = &AFF_POISON;

    /* weaken wchodzi od razu */
	if( IS_SET(poison_table[poison].special_effects, POISON_WEAKEN) )
	{
		paf->location = APPLY_STR;
		paf->modifier = - dice(2, 3);
	}

    affect_to_char( victim, paf, NULL, TRUE  );

	chance = number_range(0, 5);
/* Losowanie wp³ywu cech z ró¿nymi wspó³czynnikami, przyje³em, ¿e 15 to ¶rednia */
	chance +=   get_curr_stat(victim, STAT_LUC);
	chance += 3*get_curr_stat(victim, STAT_INT);
	chance += 2*get_curr_stat(victim, STAT_WIS);
	chance -= 90;

	chance = URANGE(0, chance, 5);

	switch (chance)
	{
		case 0:
			send_to_char("Co¶ ci przeszkadza, ale zupe³nie nie wiesz co.\n\r",victim);
			break;
		case 1:
			send_to_char("Czujesz siê jako¶ dziwnie.\n\r",victim);
			break;
		case 2:
			send_to_char("Czujesz, ¿e co¶ z tob± nie tak.\n\r",victim);
			break;
		case 3:
			send_to_char("Czujesz, ¿e twoja krew zaczyna siê burzyæ.\n\r",victim);
			break;
		case 4:
			send_to_char("Czujesz siê dziwnie i zaczynasz podejrzewaæ, ¿e to trucizna.\n\r",victim);
			break;
		case 5:
			send_to_char("Czujesz jak trucizna zaczyna kr±¿yæ w twoim organizmie.\n\r",victim);
			break;
	}
	update_poison(victim);
	return;
}

void update_poison(CHAR_DATA *victim)
{
    MURDER_LIST *tmp_death;
    int poison = 0;
    AFFECT_DATA *aff, *poison_aff = NULL, new_aff;
    int dam = 0;
    int vnum = 0;

	if( !IS_AFFECTED(victim, AFF_POISON ) )
		return;

// umarlaki i golemy s± niewra¿liwe
	if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) )
	{
		affect_strip( victim, gsn_poison );
		return;
	}

	for( aff = victim->affected; aff; aff = aff->next )
	if( aff->type == gsn_poison &&
	    aff->bitvector == &AFF_POISON )
	{
	    poison_aff = aff;
	    break;
	}

	if( !poison_aff )
		poison = 1;
	else
		poison = URANGE(0, poison_aff->level, MAX_POISON);

	if( !saves_spell_new(victim, skill_table[gsn_poison].save_type, skill_table[gsn_poison].save_mod , NULL, gsn_poison ))
	{
	if( IS_SET(poison_table[poison].special_effects, POISON_DEATH) )
	{
		send_to_char("Czujesz potworny ból w klatce piersiowej, próbujesz z trudem z³apaæ oddech. Trucizna b³yskawicznie atakuje twoje cia³o, padasz na kolana. Nagle wszystko ciemnieje, a ból ... odchodzi.\n\r",	    victim);
		act("$n g³o¶no charcze, sinieje na twarzy nie mog±c z³apaæ oddechu. Nagle pada wydaj±c z siebie przera¿aj±cy jêk i nieruchomieje.",victim,NULL,NULL,TO_ROOM);
		vnum = victim->in_room ? victim->in_room->vnum : 0;
		raw_kill(victim, victim);
		if ( !IS_NPC( victim ) )
		{
            tmp_death = malloc( sizeof( *tmp_death ) );
            tmp_death->name = str_dup( "¶mierteln± truciznê" );
            tmp_death->char_level = 0;
            tmp_death->victim_level = victim->level;
            tmp_death->room = vnum;
            tmp_death->time = current_time;
            tmp_death->next = victim->pcdata->death_statistics.pkdeath_list;
            victim->pcdata->death_statistics.pkdeath_list = tmp_death;
        }
		return;
	}

	if( IS_SET(poison_table[poison].special_effects, POISON_SLEEP) &&
	    !IS_AFFECTED(victim, AFF_SLEEP))
	    {
		new_aff.where     = TO_AFFECTS;
		new_aff.type      = gsn_poison;
		new_aff.level     = 0;
		new_aff.duration  = poison_aff->duration;
		new_aff.rt_duration = 0;
		new_aff.location  = APPLY_NONE;
		new_aff.modifier  = 0;
		new_aff.bitvector = &AFF_SLEEP;
		affect_to_char( victim, &new_aff, NULL, TRUE  );

		if ( IS_AWAKE(victim) )
		{
		    victim->position = POS_SLEEPING;
		    send_to_char("¦wiat wokó³ ciebie zawirowa³, tracisz przytomno¶æ.\n\r",	    victim);

			switch (victim->sex)
			{
				case 0:
					act("$n pada na ziemiê nieprzytomne.",victim,NULL,NULL,TO_ROOM);
					break;
				case 1:
					act("$n pada na ziemiê nieprzytomny.",victim,NULL,NULL,TO_ROOM);
					break;
				default:
					act("$n pada na ziemiê nieprzytomna.",victim,NULL,NULL,TO_ROOM);
					break;
			}
		}
		return;
	    }

	    if( IS_SET(poison_table[poison].special_effects, POISON_BLIND) &&
		!IS_AFFECTED(victim, AFF_BLIND)&&
		!IS_AFFECTED(victim,AFF_PERFECT_SENSES))
	    {
		new_aff.where     = TO_AFFECTS;
		new_aff.type      = gsn_poison;
		new_aff.level     = 0;
		new_aff.duration  = poison_aff->duration;
		new_aff.rt_duration = 0;
		new_aff.location  = APPLY_NONE;
		new_aff.modifier  = 0;
		new_aff.bitvector = &AFF_BLIND;
		affect_to_char( victim, &new_aff, NULL, TRUE );

		switch (victim->sex)
		{
			case 0:
				send_to_char("Nagle co¶ dzieje siê z twoimi oczami, wszystko ciemnieje... O¶lep³o¶!\n\r", victim);
				act("$n wygl±da na o¶lepione.",victim,NULL,NULL,TO_ROOM);
				break;
			case 1:
				send_to_char("Nagle co¶ dzieje siê z twoimi oczami, wszystko ciemnieje... O¶lep³e¶!\n\r", victim);
				act("$n wygl±da na o¶lepionego.",victim,NULL,NULL,TO_ROOM);
				break;
			default:
				send_to_char("Nagle co¶ dzieje siê z twoimi oczami, wszystko ciemnieje... O¶lep³a¶!\n\r", victim);
				act("$n wygl±da na o¶lepion±.",victim,NULL,NULL,TO_ROOM);
				break;
		}
		return;
	}

	    if( IS_SET(poison_table[poison].special_effects, POISON_PARALIZING) &&
		!IS_AFFECTED(victim, AFF_PARALYZE))
	    {
		new_aff.where     = TO_AFFECTS;
		new_aff.type      = gsn_poison;
		new_aff.level     = 0;
		new_aff.duration  = poison_aff->duration;
		new_aff.rt_duration = 0;
		new_aff.location  = APPLY_NONE;
		new_aff.modifier  = 0;
		new_aff.bitvector = &AFF_PARALYZE;
		affect_to_char( victim, &new_aff, NULL, TRUE );

		send_to_char("Nagle tracisz kontrolê nad swoim cia³em.\n\r", victim);

		act("$n zatrzymuje siê w miejscu.",victim,NULL,NULL,TO_ROOM);
		return;
	    }

	dam = dice(poison_table[poison].full_damage[1], poison_table[poison].full_damage[0] ) + poison_table[poison].full_damage[2] ;
	}
    else
	{
	dam = dice(poison_table[poison].saved_damage[1], poison_table[poison].saved_damage[0] ) + poison_table[poison].saved_damage[2] ;
	}

    if ( !is_affected(victim,gsn_perfect_self)) act( "$n trzêsie siê i cierpi.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "Trzêsiesz siê i cierpisz.\n\r", victim );
    vnum = victim->in_room ? victim->in_room->vnum : 0;
    damage(victim, victim, dam, gsn_poison, DAM_POISON, FALSE);
		if ( IS_AFFECTED( victim, AFF_MEDITATION ) )
		{
			print_char( victim, "Przerywasz medytacje, bo ¼le siê czujesz.\n\r" );
			affect_strip( victim, gsn_meditation );
		}
	if ( !IS_NPC( victim ) && !victim->in_room )
	{
        tmp_death = malloc( sizeof( *tmp_death ) );
        tmp_death->name = str_dup( "truciznê" );
        tmp_death->char_level = 0;
        tmp_death->victim_level = victim->level;
        tmp_death->room = vnum;
        tmp_death->time = current_time;
        tmp_death->next = victim->pcdata->death_statistics.pkdeath_list;
        victim->pcdata->death_statistics.pkdeath_list = tmp_death;
	}
	return;
}
bool can_add_new_char( CHAR_DATA *ch, CHAR_DATA *victim)
{
    CHARM_DATA *charm;
    int charmies = 0, value;
    int max_charmies = 1;

    if ( IS_NPC(ch) )
        return TRUE;

    for( charm = ch->pcdata->charm_list; charm; charm = charm->next )
    {
        if ( IS_NPC( charm->victim ) && charm->victim->pIndexData &&
                ( charm->victim->pIndexData->vnum == MOB_VNUM_GITHYANKI ||
                  charm->victim->pIndexData->vnum == MOB_VNUM_HYDRA ||
                  charm->victim->pIndexData->vnum == MOB_VNUM_DZINN ) )
        {
            charmies += 2;
        }
        else
            charmies++;
    }

    //domyslnie w zaleznosci od charyzmy
    value = get_curr_stat_deprecated( ch, STAT_CHA );

    //iluzjonista zaleznie od sredniej wisa i inta
    if ( ch->class == CLASS_MAG && ch->pcdata->mage_specialist == 5 && victim && IS_AFFECTED( victim, AFF_ILLUSION ))
    {
        value = (get_curr_stat_deprecated(ch,STAT_WIS)+get_curr_stat_deprecated(ch,STAT_INT))/2;
    }

    //przemian mo
    if ( ch->class == CLASS_MAG && ch->pcdata->mage_specialist == 1 && victim && IS_NPC(victim) &&
            ( victim->pIndexData->vnum == MOB_VNUM_SNAKE || victim->pIndexData->vnum == MOB_VNUM_GOLEM ) )
    {
        value = (get_curr_stat_deprecated(ch,STAT_WIS)+get_curr_stat_deprecated(ch,STAT_INT))/2;
    }

    //druid i kap³an w zale¿no¶ci od wisa i charyzmy
    if ( ch->class == CLASS_DRUID || ch->class == CLASS_CLERIC )
    {
        value = (get_curr_stat_deprecated(ch,STAT_WIS)+get_curr_stat_deprecated(ch,STAT_CHA))/2; //Raszer, zbilansowanie
    }

    if ( value > 22 )
        max_charmies = 3;
    else if (  value > 15 )
        max_charmies = 2;
    else if (  value > 8 )
        max_charmies = 1;
    else
        max_charmies = 0;



    //nekro zawsze dwa
    if ( ch->class == CLASS_MAG && ch->pcdata->mage_specialist == 7 )
        max_charmies = 2;


    if( victim->pIndexData->vnum == MOB_VNUM_GITHYANKI ||
            victim->pIndexData->vnum == MOB_VNUM_HYDRA ||
            victim->pIndexData->vnum == MOB_VNUM_DZINN )
    {
        ++charmies;
    }

    if( charmies >= max_charmies )
    {
        print_char(ch,"Nie mo¿esz ze sob± prowadzic wiêcej 'oczarowanych' przyjació³.\n\r");
        return FALSE;
    }

    return TRUE;
}

bool add_charm( CHAR_DATA *ch, CHAR_DATA *victim, bool ignore_resist )
{
    CHARM_DATA * new_charm, *charm;

    //npc nie maja limitow
    if ( IS_NPC( ch ) )
        return TRUE;

    if ( !ignore_resist && IS_NPC( victim ) && victim->resists[ RESIST_CHARM ] > 0 &&
            number_percent() < victim->resists[ RESIST_CHARM ] )
        return FALSE;

    if ( EXT_IS_SET( ch->act, PLR_NOFOLLOW ) )
    {
        send_to_char( "Przecie¿ nie chcesz ¿eby kto¶ za tob± chodzi³.\n\r", ch );
        return FALSE;
    }

    if ( !can_add_new_char( ch, victim ) )
        return FALSE;

    for ( charm = ch->pcdata->charm_list; charm; charm = charm->next )
        if ( charm->victim == victim )
            return FALSE;

    EXT_REMOVE_BIT( victim->act, ACT_AGGRESSIVE );

    new_charm = ( CHARM_DATA * ) malloc( sizeof( *new_charm ) );
    new_charm->type	= 0;
    new_charm->victim	= victim;
    new_charm->next	= ch->pcdata->charm_list;
    ch->pcdata->charm_list	= new_charm;
    return TRUE;
}

void remove_charm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHARM_DATA *charm, *charm_prev = NULL;

    if( IS_NPC(ch) || !ch->pcdata->charm_list )
        return ;

    charm = ch->pcdata->charm_list;

    if( charm->victim == victim )
    {
        ch->pcdata->charm_list = charm->next;
        free( charm );
        return;
    }

    for( charm_prev = charm, charm = charm->next; charm ; charm = charm->next )
    {
        if( charm->victim == victim )
        {
            charm_prev->next = charm->next;
            free( charm );
            return;
        }
        charm_prev = charm;
    }
    return;
}

bool stat_throw( CHAR_DATA *ch, int stat )
{
    return number_range( 0, 28 ) < get_curr_stat_deprecated( ch, stat );
}

bool ch_vs_victim_stat_throw( CHAR_DATA *ch, CHAR_DATA *victim, int stat )
{
    return number_range( 0, get_curr_stat_deprecated( ch, stat ) ) > number_range( 0, get_curr_stat_deprecated( victim, stat ) );
}

bool player_exists( char * name )
{
    char strsave [ MAX_INPUT_LENGTH ];
    FILE * file;

    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( name ) );

    fclose ( fpReserve );
    file = fopen ( strsave, "r" );

    if ( file )
    {
        fclose ( file );
        fpReserve = fopen( NULL_FILE, "r" );
        return TRUE;
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return FALSE;
}

void bonus_remove( CHAR_DATA * ch, BONUS_DATA * bonus )
{
    BONUS_INDEX_DATA * pBonus;
    char buf[MAX_STRING_LENGTH];

    if ( !bonus || !bonus->index_data )
        return;

    pBonus = bonus->index_data;

    if ( !IS_NULLSTR( pBonus->remove_prog ) )
    {
        if ( !supermob )
            init_supermob();

        char_from_room ( supermob );
        char_to_room( supermob, ch->in_room );

        sprintf( buf, "remove-bonus%d", pBonus->vnum );
        create_mprog_env( buf, pBonus->remove_prog, supermob, ch, NULL, NULL, NULL, NULL );
        program_flow();
        release_supermob();
    }

    if ( pBonus->affects )
    {
        AFFECT_DATA * paf, * lpaf, * lpaf_next;

        for ( paf = pBonus->affects; paf != NULL; paf = paf->next )
        {
            if ( paf->location == APPLY_SPELL_AFFECT )
            {
                for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
                {
                    lpaf_next = lpaf->next;
                    if ( ( lpaf->type == paf->type ) &&
                            ( lpaf->level == paf->level ) &&
                            ( lpaf->location == APPLY_SPELL_AFFECT ) )
                    {
                        affect_remove( ch, lpaf );
                        lpaf_next = NULL;
                    }
                }
            }
            else
            {
                affect_modify( ch, paf, FALSE );
                affect_check( ch, paf->where, paf->bitvector );
            }
        }
    }

    if ( bonus == ch->bonus )
    {
        ch->bonus	= bonus->next;
    }
    else
    {
        BONUS_DATA *prev;

        for ( prev = ch->bonus; prev != NULL; prev = prev->next )
        {
            if ( prev->next == bonus )
            {
                prev->next = bonus->next;
                break;
            }
        }

        if ( prev == NULL )
            return;
    }

    DISPOSE( bonus );
    return;
}

void bonus_to_char( CHAR_DATA *ch, BONUS_INDEX_DATA *pBonus )
{
    BONUS_DATA * bonus;
    AFFECT_DATA * paf;
    char buf [MAX_STRING_LENGTH];

    for( bonus = ch->bonus; bonus; bonus = ch->bonus->next )
    {
        if ( bonus->index_data == pBonus )
            return;
    }

    CREATE( bonus, BONUS_DATA, 1 );

    bonus->index_data = pBonus;
    bonus->next       = ch->bonus;
    ch->bonus         = bonus;

    for ( paf = pBonus->affects; paf != NULL; paf = paf->next )
        if ( paf->location != APPLY_SPELL_AFFECT )
            affect_modify( ch, paf, TRUE );

    if ( !IS_NULLSTR( pBonus->wear_prog ) )
    {
        if ( !supermob )
            init_supermob();

        char_from_room ( supermob );
        char_to_room( supermob, ch->in_room );

        sprintf( buf, "wear-bonus%d", pBonus->vnum );
        create_mprog_env( buf, pBonus->wear_prog, supermob, ch, NULL, NULL, NULL, NULL );
        program_flow();
        release_supermob();
    }
}

void change_player_name( CHAR_DATA * ch, char * new_name )
{
    CLAN_DATA * clan;

    if ( !ch || IS_NULLSTR( new_name ) )
        return;

    if ( ( clan = get_clan_by_member( ch->name ) ) != NULL )
    {
        change_member_name( clan, ch->name, new_name );
        save_clans();
    }

    free_string ( ch->name );
    ch->name = str_dup ( capitalize( new_name ) );

    if ( ch->level > 1 )
        save_char_obj ( ch, FALSE, FALSE );

    return;
}

//do okreslania dynamicznego hp
int get_max_hp( CHAR_DATA *ch )
{
    int hp, con_mod;

    if( IS_NPC(ch) )
    {
        if (EXT_IS_SET(ch->act, ACT_PALADIN) ||
                EXT_IS_SET(ch->act, ACT_WARRIOR) ||
                EXT_IS_SET(ch->act, ACT_BARBARIAN) ||
                EXT_IS_SET(ch->act, ACT_BLACK_KNIGHT)||
                EXT_IS_SET(ch->act, ACT_MONK) ||
                EXT_IS_SET(ch->act, ACT_SHAMAN) )
        {
            con_mod = con_app[get_curr_stat_deprecated(ch,STAT_CON)].hith;
        }
        else
        {
            con_mod = con_app[get_curr_stat_deprecated(ch,STAT_CON)].hitl;
        }
    }
    else
    {
        if (ch->class==CLASS_PALADIN ||
                ch->class==CLASS_WARRIOR ||
                ch->class==CLASS_BARBARIAN ||
                ch->class==CLASS_BLACK_KNIGHT ||
                ch->class==CLASS_MONK ||
                ch->class==CLASS_SHAMAN)
        {
            con_mod = con_app[get_curr_stat_deprecated(ch,STAT_CON)].hith;
        }
        else
        {
            con_mod = con_app[get_curr_stat_deprecated(ch,STAT_CON)].hitl;
        }
    }

    con_mod *= ch->level;

    if ( ch->max_hit == 0 && ch->hit != 0 )
        hp = ch->hit + con_mod;
    else
        hp = ch->max_hit + con_mod;

    return UMAX(1,hp);
}

#ifdef NEW_NANNY
char *new_get_class_name( NEW_CHAR_DATA * new_ch )
{
    static char class_name[MAX_STRING_LENGTH];

    if ( new_ch->class == CLASS_MAG && new_ch->mage_specialist >= 0 )
    {
        if ( new_ch->sex == 2 )
            sprintf( class_name, school_table[ URANGE( 0, new_ch->mage_specialist, 7 ) ].specialist_female_name);
        else
            sprintf( class_name, school_table[ URANGE( 0, new_ch->mage_specialist, 7 ) ].specialist_name);
    }
    else
    {
        if ( new_ch->sex == 2 )
            sprintf( class_name, class_table[ new_ch->class ].female_name);
        else
            sprintf( class_name, class_table[ new_ch->class ].name);
    }

    return class_name;
}
#endif

char *get_class_name( CHAR_DATA *ch )
{
    static char class_name[MAX_STRING_LENGTH];
    if ( ch->class == CLASS_MAG && ch->pcdata->mage_specialist >= 0 )
    {
        if( ch->sex == 2 )
            if( ch->level > 30 )
                sprintf( class_name, "%s", school_table[ URANGE( 0, ch->pcdata->mage_specialist, 7 ) ].specialist_female_name_31);
            else
                sprintf( class_name, "%s", school_table[ URANGE( 0, ch->pcdata->mage_specialist, 7 ) ].specialist_female_name);
        else
            if( ch->level > 30 )
                sprintf( class_name, "%s", school_table[ URANGE( 0, ch->pcdata->mage_specialist, 7 ) ].specialist_name_31);
            else
                sprintf( class_name, "%s", school_table[ URANGE( 0, ch->pcdata->mage_specialist, 7 ) ].specialist_name);
    }
    else
    {
        if( ch->sex == 2 )
            if( ch->level > 30 )
                sprintf( class_name, "%s", class_table[ ch->class ].female_name_31);
            else
                sprintf( class_name, "%s", class_table[ ch->class ].female_name);
        else
            if( ch->level > 30 )
                sprintf( class_name, "%s", class_table[ ch->class ].name_31);
            else
                sprintf( class_name, "%s", class_table[ ch->class ].name);
    }
    return class_name;
}

int get_class( CHAR_DATA *ch, int default_class )
{
    int class;

    if ( IS_NPC( ch ) )
    {
        if ( EXT_IS_SET( ch->act, ACT_MAGE ) ) class = 0;
        else if ( EXT_IS_SET( ch->act, ACT_CLERIC ) ) class = 1;
        else if ( EXT_IS_SET( ch->act, ACT_THIEF ) ) class = 2;
        else if ( EXT_IS_SET( ch->act, ACT_WARRIOR ) ) class = 3;
        else if ( EXT_IS_SET( ch->act, ACT_PALADIN ) ) class = 4;
        else if ( EXT_IS_SET( ch->act, ACT_DRUID ) ) class = 5;
        else if ( EXT_IS_SET( ch->act, ACT_BARBARIAN ) ) class = 6;
        else if ( EXT_IS_SET( ch->act, ACT_MONK ) ) class = 7;
        else if ( EXT_IS_SET( ch->act, ACT_BARD ) ) class = 8;
        else if ( EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) ) class = 9;
        else if ( EXT_IS_SET( ch->act, ACT_SHAMAN ) ) class = 10;

        else class = default_class;
    }
    else
        class = ch->class;

    return class;
}

int GET_DAMROLL( CHAR_DATA *ch, OBJ_DATA *weapon )
{
    int damroll;
    AFFECT_DATA *inspire;

    damroll = ch->damroll;


    if ( weapon == NULL )
    {
        damroll += str_app[get_curr_stat(ch,STAT_STR)].todam;
        if ( get_eq_char( ch, WEAR_WIELD ) == NULL && get_eq_char( ch, WEAR_SECOND ) )
        {
            if( is_affected(ch,gsn_magic_hands ) )
                damroll += URANGE( 1, ch->level/5, 5 );
            else if ( is_affected(ch,gsn_draining_hands) )
                damroll += URANGE( 0, ch->level/10, 3 );
        }
    }
    else if ( weapon->item_type == ITEM_WEAPON )
    {
        if ( weapon == get_eq_char( ch, WEAR_WIELD ) && IS_WEAPON_STAT(weapon,WEAPON_TWO_HANDS ))
        {
            damroll += (3*str_app[get_curr_stat(ch,STAT_STR)].todam)/2;
        }
        else if ( weapon == get_eq_char( ch, WEAR_SECOND ) )
        {
            damroll += (str_app[get_curr_stat(ch,STAT_STR)].todam)/2;
        }
        else
        {
            damroll += str_app[get_curr_stat(ch,STAT_STR)].todam;
        }
    }
    else if ( weapon->item_type == ITEM_SHIELD )
    {
        damroll += (str_app[get_curr_stat(ch,STAT_STR)].todam)/2;
    }

    if( is_affected(ch,gsn_inspire))
    {
        inspire = affect_find(ch->affected, gsn_inspire );
        if( inspire->duration >= 3 )
        {
            if( inspire->level > 38 )
                damroll += 6;
            else if ( inspire->level > 32 )
                damroll += 5;
            else if ( inspire->level > 28 )
                damroll += 4;
            else if ( inspire->level > 26 )
                damroll += 3;
            else if ( inspire->level > 23 )
                damroll += 2;
            else
                damroll += 1;
        }
        else
        {
            if( inspire->level > 38 )
                damroll -= 6;
            else if ( inspire->level > 32 )
                damroll -= 5;
            else if ( inspire->level > 28 )
                damroll -= 4;
            else if ( inspire->level > 26 )
                damroll -= 3;
            else if ( inspire->level > 23 )
                damroll -= 2;
            else
                damroll -= 1;
        }
    }

    return damroll;
}

int GET_SKILL_DAMROLL( CHAR_DATA *ch, OBJ_DATA *weapon )
{
    OBJ_DATA *bron = NULL;
    int damroll = 0, weapon_mastery = 0;
    bool second = 0;

    if ( weapon == NULL )
        return 0;

    if ( weapon == get_eq_char( ch, WEAR_SECOND ) )
        second = 1;

    if((ch->class == CLASS_PALADIN ||
                ch->class == CLASS_WARRIOR ) &&
            get_skill( ch, gsn_mace_mastery ) )
    {
        if ( ( !second && ( bron = get_eq_char( ch, WEAR_WIELD ) ) && weapon->value[ 0 ] == WEAPON_MACE ) ||
                ( second && ( bron = get_eq_char( ch, WEAR_SECOND ) ) && weapon->value[ 0 ] == WEAPON_MACE ) )
        {
            weapon_mastery = get_skill( ch, gsn_mace_mastery);
        }
    }

    if((ch->class == CLASS_PALADIN ||
                ch->class == CLASS_WARRIOR ) &&
            get_skill( ch, gsn_flail_mastery ) )
    {
        if ( ( !second && ( bron = get_eq_char( ch, WEAR_WIELD ) ) && weapon->value[ 0 ] == WEAPON_FLAIL ) ||
                ( second && ( bron = get_eq_char( ch, WEAR_SECOND ) ) && weapon->value[ 0 ] == WEAPON_FLAIL ) )
        {
            weapon_mastery = get_skill( ch, gsn_flail_mastery);
        }
    }

    if((ch->class == CLASS_WARRIOR ||
                ch->class == CLASS_THIEF ) &&
            get_skill( ch, gsn_dagger_mastery ) )
    {
        if ( ( !second && ( bron = get_eq_char( ch, WEAR_WIELD ) ) && weapon->value[ 0 ] == WEAPON_DAGGER ) ||
                ( second && ( bron = get_eq_char( ch, WEAR_SECOND ) ) && weapon->value[ 0 ] == WEAPON_DAGGER ) )
        {
            weapon_mastery = get_skill( ch, gsn_dagger_mastery);
        }
    }

    if((ch->class == CLASS_WARRIOR ||
                ch->class == CLASS_PALADIN ||
                ch->class == CLASS_BLACK_KNIGHT ) &&
            get_skill( ch, gsn_sword_mastery ) )
    {
        if ( ( !second && ( bron = get_eq_char( ch, WEAR_WIELD ) ) && weapon->value[ 0 ] == WEAPON_SWORD ) ||
                ( second && ( bron = get_eq_char( ch, WEAR_SECOND ) ) && weapon->value[ 0 ] == WEAPON_SWORD ) )
        {
            weapon_mastery = get_skill( ch, gsn_sword_mastery);
        }
    }

    if((ch->class == CLASS_WARRIOR ||
                ch->class == CLASS_BLACK_KNIGHT ) &&
            get_skill( ch, gsn_axe_mastery ) )
    {
        if ( ( !second && ( bron = get_eq_char( ch, WEAR_WIELD ) ) && weapon->value[ 0 ] == WEAPON_AXE ) ||
                ( second && ( bron = get_eq_char( ch, WEAR_SECOND ) ) && weapon->value[ 0 ] == WEAPON_AXE ) )
        {
            weapon_mastery = get_skill( ch, gsn_axe_mastery);
        }
    }

    if(	ch->class == CLASS_WARRIOR && get_skill( ch, gsn_spear_mastery ) )
    {
        if ( ( !second && ( bron = get_eq_char( ch, WEAR_WIELD ) ) && weapon->value[ 0 ] == WEAPON_SPEAR ) ||
                ( second && ( bron = get_eq_char( ch, WEAR_SECOND ) ) && weapon->value[ 0 ] == WEAPON_SPEAR ) )
        {
            weapon_mastery = get_skill( ch, gsn_spear_mastery);
        }
    }

    if(	ch->class == CLASS_WARRIOR && get_skill( ch, gsn_staff_mastery ) )
    {
        if ( ( !second && ( bron = get_eq_char( ch, WEAR_WIELD ) ) && weapon->value[ 0 ] == WEAPON_STAFF ) ||
                ( second && ( bron = get_eq_char( ch, WEAR_SECOND ) ) && weapon->value[ 0 ] == WEAPON_STAFF ) )
        {
            weapon_mastery = get_skill( ch, gsn_staff_mastery);
        }
    }

    if(	ch->class == CLASS_WARRIOR && get_skill( ch, gsn_shortsword_mastery ) )
    {
        if ( ( !second && ( bron = get_eq_char( ch, WEAR_WIELD ) ) && weapon->value[ 0 ] == WEAPON_SHORTSWORD ) ||
                ( second && ( bron = get_eq_char( ch, WEAR_SECOND ) ) && weapon->value[ 0 ] == WEAPON_SHORTSWORD ) )
        {
            weapon_mastery = get_skill( ch, gsn_shortsword_mastery);
        }
    }

    if((ch->class == CLASS_WARRIOR ||
                ch->class == CLASS_BLACK_KNIGHT ) &&
            get_skill( ch, gsn_whip_mastery ) )
    {
        if ( ( !second && ( bron = get_eq_char( ch, WEAR_WIELD ) ) && weapon->value[ 0 ] == WEAPON_WHIP ) ||
                ( second && ( bron = get_eq_char( ch, WEAR_SECOND ) ) && weapon->value[ 0 ] == WEAPON_WHIP ) )
        {
            weapon_mastery = get_skill( ch, gsn_whip_mastery);
        }
    }

    if((ch->class == CLASS_WARRIOR ||
                ch->class == CLASS_BLACK_KNIGHT ) &&
            get_skill( ch, gsn_polearm_mastery ) )
    {
        if ( ( !second && ( bron = get_eq_char( ch, WEAR_WIELD ) ) && weapon->value[ 0 ] == WEAPON_POLEARM ) ||
                ( second && ( bron = get_eq_char( ch, WEAR_SECOND ) ) && weapon->value[ 0 ] == WEAPON_POLEARM ) )
        {
            weapon_mastery = get_skill( ch, gsn_polearm_mastery);
        }
    }

    if ( IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS ) && number_range(1,25) < get_curr_stat_deprecated(ch,STAT_DEX) )
        damroll += URANGE( 0, (get_skill(ch, gsn_twohander_fighting) + get_skill(ch, gsn_two_hands_fighting ))/90, 2 );

    damroll += weapon_skill_mod( ch, !second, FALSE );

    if ( get_eq_char(ch,WEAR_WIELD ) && get_eq_char(ch,WEAR_SECOND ) && get_skill(ch,gsn_two_weapon_fighting) > 50 )
        ++damroll;

    return damroll;
}

int GET_HITROLL( CHAR_DATA *ch )
{
    int hitroll = 0;
    AFFECT_DATA *inspire;
    OBJ_DATA *weapon = get_eq_char(ch,WEAR_WIELD);

    hitroll = ch->hitroll;

    if( !weapon || IS_WEAPON_STAT(weapon,WEAPON_TWO_HANDS))
        hitroll += str_app[get_curr_stat(ch,STAT_STR)].tohit;
    else
        hitroll += UMAX( str_app[get_curr_stat(ch,STAT_STR)].tohit, str_app[get_curr_stat_deprecated(ch,STAT_DEX)].tohit );


    if( is_affected(ch,gsn_inspire))
    {
        inspire = affect_find(ch->affected, gsn_inspire );
        if( inspire->duration >= 3 )
        {
            if( inspire->level > 32 )
                hitroll += 5;
            else if ( inspire->level > 28 )
                hitroll += 4;
            else if ( inspire->level > 26 )
                hitroll += 3;
            else if ( inspire->level > 23 )
                hitroll += 2;
            else
                hitroll += 1;
        }
        else
        {
            if( inspire->level > 32 )
                hitroll -= 5;
            else if ( inspire->level > 28 )
                hitroll -= 4;
            else if ( inspire->level > 26 )
                hitroll -= 3;
            else if ( inspire->level > 23 )
                hitroll -= 2;
            else
                hitroll -= 1;
        }
    }

    return hitroll;
}

void destroy_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if( obj->liczba_mnoga)
        act( "$p rozpadaj± siê na kawa³ki!", ch, obj, NULL, TO_ALL);
    else
        act( "$p rozpada siê na kawa³ki!", ch, obj, NULL, TO_ALL);

    extract_obj( obj );

    return;
}

bool obj_fall_into_water( CHAR_DATA *ch, OBJ_DATA *obj )
{
    switch ( ch->in_room->sector_type )
    {
        case 6:  //woda p³yw
        case 7:  //woda niep
            if ( obj->item_type != ITEM_BOAT )
            {
                if( obj->liczba_mnoga )
                    act( "$p pogr±¿aj± siê w g³êbokiej wodzie.", ch, obj, NULL, TO_ALL );
                else
                    act( "$p pogr±¿a siê w g³êbokiej wodzie.", ch, obj, NULL, TO_ALL );
                if ( is_artefact( obj ) ) extract_artefact( obj );
                if ( obj->contains ) extract_artefact_container( obj );
                extract_obj( obj );
                return TRUE;
            }
            break;
        case 19:  //rzeka
            act( "$h porywa silny nurt.", ch, obj, NULL, TO_ALL );
            if ( is_artefact( obj ) ) extract_artefact( obj );
            if ( obj->contains ) extract_artefact_container( obj );
            extract_obj( obj );
            return TRUE;
            break;
        case 20:  //jezioro
        case 39:  //podziemne jezioro
            if ( obj->item_type != ITEM_BOAT )
            {
                if( obj->liczba_mnoga )
                    act( "$p znikaj± w odmêtach jeziora.", ch, obj, NULL, TO_ALL );
                else
                    act( "$p znika w odmêtach jeziora.", ch, obj, NULL, TO_ALL );
                if ( is_artefact( obj ) ) extract_artefact( obj );
                if ( obj->contains ) extract_artefact_container( obj );
                extract_obj( obj );
                return TRUE;
            }
            break;
        case 21:  //morze
            if ( obj->item_type != ITEM_BOAT )
            {
                if ( obj->liczba_mnoga )
                    act( "$p znikaj± pod morskimi falami.", ch, obj, NULL, TO_ALL );
                else
                    act( "$p znika pod morskimi falami.", ch, obj, NULL, TO_ALL );
                if ( is_artefact( obj ) ) extract_artefact( obj );
                if ( obj->contains ) extract_artefact_container( obj );
                extract_obj( obj );
                return TRUE;
            }
            break;
        case 22:  //ocean
            if ( obj->item_type != ITEM_BOAT )
            {
                if ( obj->liczba_mnoga )
                    act( "$p znikaj± w¶ród grzywaczy.", ch, obj, NULL, TO_ALL );
                else
                    act( "$p znika w¶ród grzywaczy.", ch, obj, NULL, TO_ALL );
                if ( is_artefact( obj ) ) extract_artefact( obj );
                if ( obj->contains ) extract_artefact_container( obj );
                extract_obj( obj );
                return TRUE;
            }
            break;
        case 23:  //lawa
            if ( obj->liczba_mnoga )
                act( "$p skwiercz± z gor±ca, a za chwilê rozpadaj± siê.", ch, obj, NULL, TO_ALL );
            else
                act( "$p skwierczy z gor±ca, a za chwilê rozpada siê.", ch, obj, NULL, TO_ALL );
            if ( is_artefact( obj ) ) extract_artefact( obj );
            if ( obj->contains ) extract_artefact_container( obj );
            extract_obj( obj );
            return TRUE;
            break;
        case 24:  //ruchome piaski
            if ( obj->liczba_mnoga )
                act( "$p znikaj± w mia³kim piasku.", ch, obj, NULL, TO_ALL );
            else
                act( "$p znika w mia³kim piasku.", ch, obj, NULL, TO_ALL );
            if ( is_artefact( obj ) ) extract_artefact( obj );
            if ( obj->contains ) extract_artefact_container( obj );
            extract_obj( obj );
            return TRUE;
            break;
        case 25:  //gor±ce ¼ród³o
            if ( obj->item_type != ITEM_BOAT )
            {
                if ( obj->liczba_mnoga )
                    act( "$p pogr±¿aj± siê w ¼ródle.", ch, obj, NULL, TO_ALL );
                else
                    act( "$p pogr±¿a siê w ¼ródle.", ch, obj, NULL, TO_ALL );
                if ( is_artefact( obj ) ) extract_artefact( obj );
                if ( obj->contains ) extract_artefact_container( obj );
                extract_obj( obj );
                return TRUE;
            }
            break;
        case 34:  //pod wod±
            if ( obj->liczba_mnoga )
                act( "$p ton± w jeszcze g³êbszej wodzie.", ch, obj, NULL, TO_ALL );
            else
                act( "$p tonie w jeszcze g³êbszej wodzie.", ch, obj, NULL, TO_ALL );
            if ( is_artefact( obj ) ) extract_artefact( obj );
            if ( obj->contains ) extract_artefact_container( obj );
            extract_obj( obj );
            return TRUE;
            break;
        case 37:  //ciemna woda
            if ( obj->item_type != ITEM_BOAT )
            {
                if ( obj->liczba_mnoga )
                    act( "$p pogr±¿aj± siê w ciemnej wodzie.", ch, obj, NULL, TO_ALL );
                else
                    act( "$p pogr±¿a siê w ciemnej wodzie.", ch, obj, NULL, TO_ALL );
                if ( is_artefact( obj ) ) extract_artefact( obj );
                if ( obj->contains ) extract_artefact_container( obj );
                extract_obj( obj );
                return TRUE;
            }
            break;
    }

    return FALSE;
}

bool is_undead( CHAR_DATA *ch )
{
    if ( !IS_NPC( ch ) )
        return FALSE;

    if ( EXT_IS_SET( ch->act, ACT_UNDEAD ) )
        return TRUE;

    if ( IS_SET( ch->form, FORM_UNDEAD ) )
        return TRUE;

    if ( IS_SET( race_table[ GET_RACE( ch ) ].form, FORM_UNDEAD ) )
        return TRUE;

    if ( IS_SET( race_table[ GET_RACE( ch ) ].type , UNDEAD ) )
        return TRUE;

    return FALSE;
}

int GET_RACE( CHAR_DATA *ch )
{
    int race = 0;
    if( ch->real_race == 0 || ch->ss_data )
    {
        race = ch->race;
    }
    else
    {
        race = ch->real_race;
    }
    return race;
}

int GET_REAL_RACE( CHAR_DATA *ch )
{
    return( ch->real_race == 0 )? ch->race : ch->real_race;
}

OBJ_DATA * find_boat(CHAR_DATA * ch)
{
    OBJ_DATA * obj = NULL;
    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
        if ( obj->item_type == ITEM_BOAT )
            break ;
    return obj ;
}

bool check_illusion( CHAR_DATA *ch )
{
    OBJ_DATA *obj, *obj_next;

    if ( !IS_AFFECTED( ch, AFF_ILLUSION ) )
        return FALSE;

    act( "$n rozp³ywa siê w powietrzu.", ch, NULL, NULL, TO_ROOM );

    /* itemy co je trzymala iluzja opadaja na ziemie*/
    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
        OBJ_NEXT_CONTENT( obj, obj_next );

        if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
            continue;
        if ( obj->wear_loc != WEAR_NONE )
            unequip_char( ch, obj );

        obj_from_char( obj );
        act( "$p upada na ziemiê.", ch, obj, NULL, TO_ROOM );
        obj_to_room( obj, ch->in_room );

        if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
        {
            switch ( number_range( 1, 3 ) )
            {
                case 1:
                    act( "$p rozp³ywa siê w powietrzu.", ch, obj, NULL, TO_ROOM );
                    break;
                case 2:
                    act( "$p znika bez ¶ladu.", ch, obj, NULL, TO_ROOM );
                    break;
                default:
                    act( "$p rozp³ywa siê pozostawiaj±c tylko chmurê dymu.", ch, obj, NULL, TO_ROOM );
                    break;
            }
        }
        else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
        {
            switch ( number_range( 1, 3 ) )
            {
                case 1:
                    act( "$p toczy siê przez chwilê, po czym rozpryskujê siê na kwa³eczki.", ch, obj, NULL, TO_ROOM );
                    break;
                case 2:
                    act( "$p pêka z hukiem i rozpryskuje siê dooko³a.", ch, obj, NULL, TO_ROOM );
                    break;
                default:
                    act( "$p rozpryskuje siê na kawa³eczki.", ch, obj, NULL, TO_ROOM );
                    break;
            }
        }
    }

    long int copper = money_count_copper ( ch );
    if ( copper > 0 )
    {
        obj_to_room( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), ch->in_room );
        if ( copper > 1 ) act( "Kupka monet upada na ziemiê.", ch, NULL, NULL, TO_ROOM );
        else act( "Jedna moneta upada na ziemiê.", ch, NULL, NULL, TO_ROOM );
        money_reset_character_money ( ch );
    }

    extract_char( ch, TRUE );

    return TRUE;
}

/**
 * check affects to stay
 */
bool check_noremovable_affects( AFFECT_DATA * aff )
{
    if (
            0
            || aff->type == gsn_garhal
            || aff->type == gsn_glorious_impale
            || aff->type == gsn_healing_touch
            || aff->type == gsn_lay
            || aff->type == gsn_lore
            || aff->type == gsn_playerdeath
            || aff->type == gsn_playerdeathlearnpenalty
            || aff->type == gsn_playerdeathpenalty
            || aff->type == gsn_torment
       )
    {
        return TRUE;
    }
    return FALSE;
}

SPIRIT_DATA * add_spirit( CHAR_DATA *ch, OBJ_DATA *corpse )
{
    SPIRIT_DATA * duch;
    for ( duch = spirits; duch != NULL; duch = duch->next )
    {
        if ( duch->ch == ch )
        {
            break;
        }
    }
    if ( duch != NULL )
    {
        return duch;
    }
    
    duch = new_spirit();
    duch->ch = ch;
    duch->corpse = corpse;
    duch->next = spirits;
    spirits = duch;
    return duch;
}

void del_spirit	( CHAR_DATA *ch )
{
    SPIRIT_DATA * duch = spirits;
    if ( ch == NULL || IS_NPC( ch ) || !duch )
    {
         return;
    }

    if ( duch->ch == ch )
    {
        spirits = duch->next;
    }
    else
    {
        SPIRIT_DATA *prev;
        for ( prev = spirits; prev != NULL; prev = prev->next )
        {
            duch = prev->next;
            if ( duch && duch->ch == ch )
            {
                prev->next = duch->next;
                duch->next = NULL;
                free_spirit( duch );
                duch = NULL;
                break;
            }
        }
        if ( prev == NULL )
        {
            bug( "del_spirit: ch not found.", 0 );
            return;
        }
    }

}

SPIRIT_DATA * get_spirit ( CHAR_DATA *ch )
{
    if( IS_NPC( ch ) )
        return FALSE;

    SPIRIT_DATA * duch;
    for ( duch = spirits; duch != NULL; duch = duch->next )
    {
        if ( duch->ch == ch )
        {
            return duch;
        }
    }
    return FALSE;
}
