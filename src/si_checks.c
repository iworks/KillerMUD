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
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: si_checks.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/si_checks.c $
 */
#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "magic.h"
#include "interp.h"
#include "si.h"
#include "projects.h"

CHAR_DATA *get_char_area( CHAR_DATA *ch, char *argument );

/**** krotkie checki do akcji ****/
bool hp_check( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( desc->hp_percent < 75 )
        return TRUE;

    return FALSE;
}

bool is_caster( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( ch_desc->caster >= 0 )
        return TRUE;

    return FALSE;
}

bool can_cast( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( ch_desc->ch->position < POS_FIGHTING	||
         IS_AFFECTED( ch_desc->ch, AFF_SILENCE ) ||
         IS_AFFECTED( ch_desc->ch, AFF_FEAR ) ||
		 IS_AFFECTED( ch_desc->ch, AFF_RAZORBLADED )||
		 IS_AFFECTED( ch_desc->ch, AFF_BEAST_CLAWS )||
         !can_move( ch_desc->ch ) ||
         EXT_IS_SET( ch_desc->ch->in_room->room_flags, ROOM_NOMAGIC ) )
        return FALSE;

    return TRUE;
}

bool got_any_frags( struct char_desc *ch_desc, struct char_desc *desc )
{
    int spell;

    for ( spell = 0; si_spell_info[ FRAGS ][ spell ].name; spell++ )
        if ( SPELL_MEMMED( ch_desc->ch, si_spell_info[ FRAGS ][ spell ].sn ) )
            return TRUE;

    return FALSE;
}

bool got_problems( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( desc->ch->position == POS_DEAD || !desc->ch->in_room )
        return FALSE;

    if ( desc->active < 5 || desc->hp_percent < 65 )
        return TRUE;

    return FALSE;
}

bool is_wounded( struct char_desc *ch_desc, struct char_desc *desc )
{
    CHAR_DATA *tch;

    //sprawdzenie, co by nie siedzial przy graczach szykujacych sie do walki
    for ( tch = ch_desc->ch->in_room->people; tch; tch = tch->next_in_room )
    {
        if ( tch == ch_desc->ch )
            continue;

        if ( IS_NPC( tch ) && !IS_AFFECTED( tch, AFF_CHARM ) )
            continue;

        return FALSE;
    }

    if ( desc->ch->position == POS_DEAD || !desc->ch->in_room )
        return FALSE;

    if ( desc->hp_percent < 95 )
        return TRUE;

    return FALSE;
}

bool should_lay( struct char_desc *ch_desc, struct char_desc *desc )
{

	if ( ch_desc->ch->fighting != NULL )
		return FALSE;

    if ( ch_desc->ch->position <= POS_STUNNED ||
    	ch_desc->ch->position == POS_FIGHTING ||
    	!ch_desc->ch->in_room )
        return FALSE;

    if ( desc->hp_percent < 75 )
        return TRUE;

    return FALSE;
}

bool can_be_healed( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( desc->ch->position == POS_DEAD || !desc->ch->in_room )
        return FALSE;

    if ( IS_NPC( desc->ch ) && ( is_undead( desc->ch ) || IS_SET( desc->ch->form, FORM_CONSTRUCT ) ) )
        return FALSE;

    if ( desc->active < 5 || desc->hp_percent < 70 )
        return TRUE;

    return FALSE;
}

bool need_a_help( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( desc->hp_percent < 95 || desc->active < 5 )
        return TRUE;

    return FALSE;
}

bool knows_rescue( struct char_desc *ch_desc, struct char_desc *desc )
{
    sh_int skill;

    skill = IS_NPC( ch_desc->ch ) ? UMIN( 15, get_skill( ch_desc->ch, gsn_rescue ) ) : get_skill( ch_desc->ch, gsn_rescue );

    if ( skill > 0 )
        return TRUE;

    return FALSE;
}

bool knows_berserk( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( get_skill( ch_desc->ch, gsn_berserk ) > 0 )
        return TRUE;

    return FALSE;
}

bool knows_bash( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( get_skill( ch_desc->ch, gsn_bash ) > 0 )
        return TRUE;

    return FALSE;
}

bool knows_slam( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( get_skill( ch_desc->ch, gsn_slam ) > 0 )
        return TRUE;

    return FALSE;
}

bool knows_charge( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( get_skill( ch_desc->ch, gsn_charge ) > 0 )
        return TRUE;

    return FALSE;
}


bool can_charge( struct char_desc *ch_desc, struct char_desc *desc )
{
    CHAR_DATA *vch;
    OBJ_DATA *weapon;

    if ( ( weapon = get_eq_char( ch_desc->ch, WEAR_WIELD ) ) == NULL )
        return FALSE;

    if (weapon->value[ 0 ] == WEAPON_DAGGER ||
    	weapon->value[ 0 ] == WEAPON_SHORTSWORD ||
    	weapon->value[ 0 ] == WEAPON_WHIP ||
    	weapon->value[ 0 ] == WEAPON_CLAWS)
        return FALSE;

    if ( !IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS ) )
        return FALSE;

    for ( vch = ch_desc->ch->in_room->people; vch; vch = vch->next_in_room )
        if ( vch->fighting == ch_desc->ch && can_move( vch ) )
            return FALSE;

    return TRUE;
}


bool knows_any_skills( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( get_skill( ch_desc->ch, gsn_bash ) > 0 ||
         get_skill( ch_desc->ch, gsn_kick ) > 0 ||
         get_skill( ch_desc->ch, gsn_crush ) > 0 ||
         get_skill( ch_desc->ch, gsn_tail ) > 0 ||
         get_skill( ch_desc->ch, gsn_fire_breath ) > 0 ||
         get_skill( ch_desc->ch, gsn_frost_breath ) > 0 ||
         get_skill( ch_desc->ch, gsn_acid_breath ) > 0 ||
         get_skill( ch_desc->ch, gsn_lightning_breath ) > 0 ||
         get_skill( ch_desc->ch, gsn_disarm ) > 0 ||
         get_skill( ch_desc->ch, gsn_trip ) > 0 ||
         get_skill( ch_desc->ch, gsn_dodge ) > 0 ||
         get_skill( ch_desc->ch, gsn_parry ) > 0 ||
         get_skill( ch_desc->ch, gsn_mighty_blow ) > 0 ||
         get_skill( ch_desc->ch, gsn_torment ) > 0 ||
         get_skill( ch_desc->ch, gsn_power_strike ) > 0 ||
         get_skill( ch_desc->ch, gsn_critical_strike ) > 0 ||
         get_skill( ch_desc->ch, gsn_smite_good ) > 0 ||
         get_skill( ch_desc->ch, gsn_vertical_slash ) > 0 ||
         get_skill( ch_desc->ch, gsn_cleave ) > 0 ||
         get_skill( ch_desc->ch, gsn_overwhelming_strike ) > 0 ||
         get_skill( ch_desc->ch, gsn_smite ) > 0 ||
         get_skill( ch_desc->ch, gsn_stun ) > 0  ||
         get_skill( ch_desc->ch, gsn_slam ) > 0 )

        return TRUE;

    return FALSE;
}

bool can_do_some_magic( struct char_desc *ch_desc, struct char_desc *desc )
{
    int spell;

    for ( spell = 0; si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].name; spell++ )
        if ( SPELL_MEMMED( ch_desc->ch, si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].sn ) )
            return TRUE;

    for ( spell = 0; si_spell_info[ DEF_ANTIMAGIC_OTHER ][ spell ].name; spell++ )
        if ( SPELL_MEMMED( ch_desc->ch, si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].sn ) )
            return TRUE;

    return FALSE;
}

bool can_lay( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( get_skill( ch_desc->ch, gsn_lay ) <= 0 )
        return FALSE;

    if ( ch_desc->ch->counter[ 1 ] > 0 )
        return FALSE;

    return TRUE;
}

bool is_dark_here( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( EXT_IS_SET( ch_desc->ch->in_room->room_flags, ROOM_MAGICDARK ) )
        return FALSE;

    if ( room_is_dark( ch_desc->ch, ch_desc->ch->in_room ) )
        return TRUE;

    return FALSE;
}

bool got_any_antitanks( struct char_desc *ch_desc, struct char_desc *desc )
{
    int spell;

    for ( spell = 0; si_spell_info[ OFF_ANTITANK ][ spell ].name; spell++ )
        if ( SPELL_MEMMED( ch_desc->ch, si_spell_info[ OFF_ANTITANK ][ spell ].sn ) )
            return TRUE;

    return FALSE;
}

bool can_heal( struct char_desc *ch_desc, struct char_desc *desc )
{
    int spell;

    for ( spell = 0; si_spell_info[ HEALS ][ spell ].name; spell++ )
        if ( SPELL_MEMMED( ch_desc->ch, si_spell_info[ HEALS ][ spell ].sn ) )
            return TRUE;

    return FALSE;
}

bool is_enemy_caster( struct char_desc *ch_desc, struct char_desc *desc )
{
    switch ( desc->caster )
    {
        case 0:
        case 1:
        case 5: return TRUE;
        case 4: return ( number_percent() < 30 ? TRUE : FALSE );
        default: return FALSE;
    }
    return FALSE;
}

bool got_anticasters( struct char_desc *ch_desc, struct char_desc *desc )
{
    int spell;

    for ( spell = 0; si_spell_info[ OFF_ANTICASTER ][ spell ].name; spell++ )
        if ( SPELL_MEMMED( ch_desc->ch, si_spell_info[ OFF_ANTICASTER ][ spell ].sn ) )
            return TRUE;

    return FALSE;
}

bool is_tank( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( ch_desc->tank > 0 )
        return TRUE;

    return FALSE;
}

bool can_tank( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( ch_desc->tank > 0 && ch_desc->hp_percent > 40 )
        return TRUE;

    return FALSE;
}


bool is_enemy_tank( struct char_desc *ch_desc, struct char_desc *desc )
{
    CHAR_DATA *vch;

    if ( desc->tank <= 0 )
        return FALSE;
  	save_debug_info("si_checks.c => is_enemy_tank", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

    /* jesli ktos z grupki z nim walczy */
    for ( vch = ch_desc->ch->in_room->people; vch; vch = vch->next_in_room )
        if ( is_same_group( ch_desc->ch, vch ) && vch->fighting == desc->ch )
            return TRUE;

    return FALSE;
}

bool is_defending( struct char_desc *ch_desc, struct char_desc *desc )
{
    CHAR_DATA *vch;

    /* jesli ktos go pierze */
    for ( vch = ch_desc->ch->in_room->people; vch; vch = vch->next_in_room )
        if ( vch->fighting == ch_desc->ch )
            return TRUE;

    return FALSE;
}


bool can_rescue( struct char_desc *ch_desc, struct char_desc *desc )
{
    CHAR_DATA *vch;

    if ( desc->ch == ch_desc->ch )
        return FALSE;

    for ( vch = ch_desc->ch->in_room->people; vch; vch = vch->next_in_room )
        if ( vch->fighting == desc->ch )
            return TRUE;

    return FALSE;
}

bool can_berserk( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( IS_AFFECTED( ch_desc->ch, AFF_BERSERK ) || is_affected( ch_desc->ch, gsn_berserk ) )
        return FALSE;

    if ( IS_AFFECTED( ch_desc->ch, AFF_CALM ) )
        return FALSE;

    return TRUE;
}

bool is_target_active( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( desc->active > 0 )
        return TRUE;

    return FALSE;
}

bool got_powerups_self( struct char_desc *ch_desc, struct char_desc *desc )
{
    int spell;

    for ( spell = 0; si_spell_info[ DEF_ANTITANK_SELF ][ spell ].name; spell++ )
        if ( SPELL_MEMMED( ch_desc->ch, si_spell_info[ DEF_ANTITANK_SELF ][ spell ].sn ) )
            return TRUE;

    for ( spell = 0; si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].name; spell++ )
        if ( SPELL_MEMMED( ch_desc->ch, si_spell_info[ DEF_ANTITANK_SELF ][ spell ].sn ) )
            return TRUE;

    return FALSE;
}

bool self( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( ch_desc->ch == desc->ch )
        return TRUE;

    return FALSE;
}

bool notself( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( ch_desc->ch != desc->ch )
        return TRUE;

    return FALSE;
}

bool standing( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( desc->ch->position >= POS_FIGHTING )
        return TRUE;

    return FALSE;
}

bool cant_move( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( !can_move( desc->ch ) )
        return TRUE;

    return FALSE;
}

bool not_fighting( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( ch_desc->ch->fighting == NULL )
        return TRUE;

    return FALSE;
}

bool fighting( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( ch_desc->ch->fighting )
        return TRUE;

    return FALSE;
}

bool is_down( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( ch_desc->ch->position < POS_FIGHTING &&
         ch_desc->ch->position > POS_STUNNED )
        return TRUE;

    return FALSE;
}

bool is_resting( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( ch_desc->ch->position == POS_RESTING || ch_desc->ch->position == POS_SITTING )
        return TRUE;

    return FALSE;
}

bool is_boring( struct char_desc *ch_desc, struct char_desc *desc )
{
//    CHAR_DATA *tch;

    if ( ch_desc->ch->memming &&
         !IS_AFFECTED( ch_desc->ch, AFF_BLIND ) &&
         !room_is_dark( ch_desc->ch, ch_desc->ch->in_room ) &&
         !IS_SET( sector_table[ ch_desc->ch->in_room->sector_type ].flag, SECT_NOMEM ) )
        return FALSE;

    //sprawdzenie, co by nie siedzial przy graczach szykujacych sie do walki
    //wywalam, co to za roznica czy siedzi czy ni?
/*    for ( tch = ch_desc->ch->in_room->people; tch; tch = tch->next_in_room )
    {
        if ( tch == ch_desc->ch )
            continue;

        if ( IS_NPC( tch ) && !IS_AFFECTED( tch, AFF_CHARM ) )
            continue;

        return TRUE;
    }*/

    if ( desc->hp_percent < 90 )
        return FALSE;

    return TRUE;
}

bool got_stone_skin( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( !SPELL_MEMMED( ch_desc->ch, gsn_stone_skin ) )
        return FALSE;

    if ( is_affected( ch_desc->ch, gsn_stone_skin ) )
        return FALSE;

    return TRUE;
}

bool knows_backstab( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( get_skill( ch_desc->ch, gsn_backstab ) > 0 )
        return TRUE;

    return FALSE;
}

bool can_backstab( struct char_desc *ch_desc, struct char_desc *desc )
{
    OBJ_DATA *weapon;

    if ( ( weapon = get_eq_char( ch_desc->ch, WEAR_WIELD ) ) == NULL )
        return FALSE;

    if ( weapon->value[ 0 ] != 2 )
        return FALSE;

    if ( !IS_SET( race_table[ GET_RACE( desc->ch ) ].type, PERSON ) )
        return FALSE;

    return TRUE;
}

bool knows_wardance( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( get_skill( ch_desc->ch, gsn_wardance ) > 0 )
        return TRUE;

    return FALSE;
}

bool can_wardance( struct char_desc *ch_desc, struct char_desc *desc )
{
    OBJ_DATA *weapon;

    if ( IS_AFFECTED( ch_desc->ch, AFF_BERSERK ) || is_affected( ch_desc->ch, gsn_wardance ) )
        return FALSE;

    if ( IS_AFFECTED( ch_desc->ch, AFF_CALM ) )
        return FALSE;

    if ( ( weapon = get_eq_char( ch_desc->ch, WEAR_WIELD ) ) == NULL )
        return FALSE;

    return TRUE;
}

bool wardancing( struct char_desc *ch_desc, struct char_desc *desc )
{
    return ( IS_AFFECTED( ch_desc->ch, AFF_BERSERK ) ? TRUE : FALSE );
}

bool disarmed( struct char_desc *ch_desc, struct char_desc *desc )
{
    OBJ_DATA *weapon, *obj;
    int weapon_rating, skill_max = 0, skill, weapon_rating_max = -1;

    if ( !ch_desc || !ch_desc->ch || !IS_NPC( ch_desc->ch ) || !ch_desc->ch->in_room || !can_move( ch_desc->ch ) )
        return FALSE;

    if ( ( weapon = get_eq_char( ch_desc->ch, WEAR_WIELD ) ) != NULL )
        return FALSE;

    if ( EXT_IS_SET( ch_desc->ch->in_room->room_flags, ROOM_SAFE ) ||
         EXT_IS_SET( ch_desc->ch->act, ACT_PRACTICE ) ||
         ch_desc->ch->pIndexData->pRepair != NULL ||
         ch_desc->ch->pIndexData->pShop != NULL )
        return FALSE;

    if ( ch_desc->ch->fighting &&
         number_percent() > 10 )
        return FALSE;

    /* wybiera bron ktora mu najbardziej pasuje */
    for ( obj = ch_desc->ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
        if ( obj->item_type == ITEM_WEAPON )
        {
            skill = get_weapon_skill( ch_desc->ch, weapon_sn( obj ) );
            weapon_rating = ( obj->value[ 1 ] * obj->value[ 2 ] + obj->value[ 1 ] + 2 * obj->value[ 6 ] ) / 2;

            if ( skill >= skill_max && weapon_rating >= weapon_rating_max )
            {
                skill_max = skill;
                weapon_rating_max = weapon_rating;
                weapon = obj;
            }
        }
    }

    if ( !weapon )
        return FALSE;

    get_obj( ch_desc->ch, weapon, NULL );

    if ( weapon->carried_by == NULL )
        return FALSE;

    if ( ( IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS ) )
         && ( get_hand_slots( ch_desc->ch, WEAR_WIELD ) >= 1 ) )
        return TRUE;
    else if ( get_hand_slots( ch_desc->ch, WEAR_WIELD ) >= 2 )
        return TRUE;

    if ( ( IS_OBJ_STAT( weapon, ITEM_ANTI_EVIL ) && IS_EVIL( ch_desc->ch ) )
         || ( IS_OBJ_STAT( weapon, ITEM_ANTI_GOOD ) && IS_GOOD( ch_desc->ch ) )
         || ( IS_OBJ_STAT( weapon, ITEM_ANTI_NEUTRAL ) && IS_NEUTRAL( ch_desc->ch ) ) )
        return TRUE;

    if ( weapon->pIndexData->progs )
        return TRUE;

    if ( ch_desc->ch->pIndexData->damage[ DICE_NUMBER ] * ch_desc->ch->pIndexData->damage[ DICE_TYPE ]
         + ch_desc->ch->pIndexData->damage[ DICE_BONUS ] <
         weapon->value[ 1 ] * weapon->value[ 2 ] + weapon->value[ 6 ] + ch_desc->ch->weapon_damage_bonus )
        wield_weapon( ch_desc->ch, weapon, TRUE );
    return TRUE;
}

bool can_target_master( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( ch_desc->ch->fighting	&&
         IS_AFFECTED( ch_desc->ch->fighting, AFF_CHARM ) &&
         ch_desc->ch->fighting->master	&&
         can_see( ch_desc->ch, ch_desc->ch->fighting->master ) )
        return TRUE;

    return FALSE;
}

bool knows_damage_reduction( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( get_skill( ch_desc->ch, gsn_damage_reduction ) > 0 )
        return TRUE;

    return FALSE;
}

bool can_do_damage_reduction( struct char_desc *ch_desc, struct char_desc *desc )
{

    if ( is_affected( ch_desc->ch, gsn_damage_reduction ) )
        return FALSE;

    if ( ch_desc->ch->hit >= 9*get_max_hp(ch_desc->ch) / 10 )
        return FALSE;

    return TRUE;
}

bool got_any_holds( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( SPELL_MEMMED( ch_desc->ch, gsn_holdperson ) ||
         SPELL_MEMMED( ch_desc->ch, gsn_holdmonster ) ||
         SPELL_MEMMED( ch_desc->ch, gsn_holdanimal ) ||
         SPELL_MEMMED( ch_desc->ch, gsn_holdplant ) ||
         SPELL_MEMMED( ch_desc->ch, gsn_holdevil ) )
        return TRUE;

    return FALSE;
}

bool nasty_holdable( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( desc->ch->fighting )
        return FALSE;

    if ( IS_AFFECTED( desc->ch, AFF_FREE_ACTION ) )
        return FALSE;

    if ( IS_AFFECTED( desc->ch, AFF_PARALYZE ) )
        return FALSE;


    if ( !IS_SET( race_table[ GET_RACE( desc->ch ) ].type, PERSON ) )
        return FALSE;

    return TRUE;
}

bool need_memming( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( ch_desc->ch->count_memspell > 0 &&
         !IS_AFFECTED( ch_desc->ch, AFF_BLIND ) &&
         !room_is_dark( ch_desc->ch, ch_desc->ch->in_room ) &&
         !IS_SET( sector_table[ ch_desc->ch->in_room->sector_type ].flag, SECT_NOMEM ) )
        return TRUE;

    return FALSE;
}

bool shopping( struct char_desc *ch_desc, struct char_desc *desc )
{
    CHAR_DATA *tch;

    if ( !ch_desc->ch->pIndexData || ( !ch_desc->ch->pIndexData->pShop && !ch_desc->ch->pIndexData->pRepair ) )
        return FALSE;

    for ( tch = ch_desc->ch->in_room->people; tch; tch = tch->next_in_room )
    {
        if ( !IS_NPC( tch ) )
            return TRUE;
    }

    return FALSE;
}

bool can_summon( struct char_desc *ch_desc, struct char_desc *desc )
{

	if( !EXT_IS_SET( ch_desc->ch->act, ACT_MAGE ))
		return FALSE;

	if ( SPELL_MEMMED( ch_desc->ch, 94 ) )//sn summona
	{
		return TRUE;
	}

    return FALSE;
}

bool can_teleport( struct char_desc *ch_desc, struct char_desc *desc )
{

	if( !EXT_IS_SET( ch_desc->ch->act, ACT_MAGE ))
		return FALSE;

	if ( SPELL_MEMMED( ch_desc->ch, 218 ) )//sn dimension door
	{
		return TRUE;
	}

    return FALSE;
}

bool should_summon( struct char_desc *ch_desc, struct char_desc *desc )
{
	MEM_DATA * tmp;
	CHAR_DATA *victim = NULL;
	char tar[MAX_STRING_LENGTH];

	if ( ch_desc->ch->fighting != NULL )
		return FALSE;

    if ( ch_desc->ch->position <= POS_STUNNED ||
    	ch_desc->ch->position == POS_FIGHTING ||
    	!ch_desc->ch->in_room )
        return FALSE;

    if ( ch_desc->hp_percent < 30 )
       	return FALSE;

	if ( !ch_desc->ch || !ch_desc->ch->memory )
		return FALSE;

	for ( tmp = ch_desc->ch->memory;tmp;tmp = tmp->next )
		if ( tmp->who != NULL && tmp->reaction == MEM_HATE )
		{
			victim = tmp->who;
			break;
		}

	if ( !victim ) victim = ch_desc->ch->hunting;

	if ( !victim || !victim->in_room )
		return FALSE;

	if ( victim == ch_desc->ch )
		return FALSE;

	sprintf( tar, "%s", victim->name );

	if ( !SAME_AREA( ch_desc->ch->in_room->area, victim->in_room->area )
	     || victim == ch_desc->ch
	     || victim->in_room == NULL
	     || EXT_IS_SET( ch_desc->ch->in_room->room_flags, ROOM_SAFE )
	     || EXT_IS_SET( victim->in_room->room_flags, ROOM_SAFE )
	     || EXT_IS_SET( victim->in_room->room_flags, ROOM_NOMAGIC )
	     || EXT_IS_SET( victim->in_room->room_flags, ROOM_SOLITARY )
	     || EXT_IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL )
	     || victim->level > ch_desc->ch->level
	     || ( !IS_NPC( victim ) && victim->level >= LEVEL_IMMORTAL )
	     || victim->fighting != NULL
	     || IS_AFFECTED( victim, AFF_SANCTUARY )
	     || IS_AFFECTED( victim, AFF_FORCE_FIELD )
	     || ch_desc->ch->in_room == victim->in_room
	     || EXT_IS_SET( ch_desc->ch->in_room->room_flags, ROOM_PRIVATE ) )
		return FALSE;

    return TRUE;
}

bool should_teleport( struct char_desc *ch_desc, struct char_desc *desc )
{
	MEM_DATA * tmp;
	CHAR_DATA *victim = NULL;
	char tar[MAX_STRING_LENGTH];

	if ( ch_desc->ch->fighting != NULL )
		return FALSE;

    if ( ch_desc->ch->position <= POS_STUNNED ||
    	ch_desc->ch->position == POS_FIGHTING ||
    	!ch_desc->ch->in_room )
        return FALSE;

    if ( ch_desc->hp_percent < 30 )
       	return FALSE;

	if ( !ch_desc->ch || !ch_desc->ch->memory )
		return FALSE;

	for ( tmp = ch_desc->ch->memory;tmp;tmp = tmp->next )
		if ( tmp->who != NULL && tmp->reaction == MEM_HATE )
		{
			victim = tmp->who;
			break;
		}

	if ( !victim ) victim = ch_desc->ch->hunting;

	if ( !victim || !victim->in_room )
		return FALSE;

	sprintf( tar, "%s", victim->name );

	if ( !SAME_AREA( ch_desc->ch->in_room->area, victim->in_room->area )
	     || victim == ch_desc->ch
	     || victim->in_room == NULL
	     || EXT_IS_SET( victim->in_room->room_flags, ROOM_SAFE )
	     || EXT_IS_SET( victim->in_room->room_flags, ROOM_NOMAGIC )
	     || EXT_IS_SET( victim->in_room->room_flags, ROOM_SOLITARY )
	     || EXT_IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL )
	     || EXT_IS_SET( ch_desc->ch->in_room->room_flags, ROOM_NO_RECALL )
	     || ( !IS_NPC( victim ) && victim->level >= LEVEL_IMMORTAL )
	     || IS_AFFECTED( victim, AFF_SANCTUARY )
	     || IS_AFFECTED( victim, AFF_FORCE_FIELD )
	     || ch_desc->ch->in_room == victim->in_room
	     || EXT_IS_SET( victim->in_room->room_flags, ROOM_PRIVATE ) )
		return FALSE;

    return TRUE;
}

bool default_pos_resting( struct char_desc *ch_desc, struct char_desc *desc )
{
	//jak ma pozycje domyslna resting albo sitting to nie wstaje
	if ( !ch_desc || !ch_desc->ch || !ch_desc->ch->pIndexData )
        return FALSE;

    if ( ch_desc->ch->default_pos == POS_RESTING || ch_desc->ch->default_pos == POS_SITTING  || EXT_IS_SET(ch_desc->ch->act, ACT_MOUNTABLE))// Raszer, proba rozwiazania problemu z wstawaniem wierzchowcow
        return FALSE;

    return TRUE;
}

bool default_pos_sleeping( struct char_desc *ch_desc, struct char_desc *desc )
{
	if ( !ch_desc || !ch_desc->ch || !ch_desc->ch->pIndexData )
        return FALSE;

	//jak ma pozycje domyslna sleeping to nie wstaje
    if ( ch_desc->ch->default_pos == POS_SLEEPING )
        return FALSE;

    return TRUE;
}

bool is_sleeping_normal( struct char_desc *ch_desc, struct char_desc *desc )
{
    if ( ch_desc->ch->position != POS_SLEEPING )
        return FALSE;

	if ( IS_AFFECTED(ch_desc->ch, AFF_SLEEP ) || IS_AFFECTED(ch_desc->ch, AFF_PARALYZE ) || IS_AFFECTED(ch_desc->ch, AFF_DAZE ))
		return FALSE;

    return TRUE;
}

bool should_turn( struct char_desc *ch_desc, struct char_desc *desc )
{
	CHAR_DATA *victim = NULL;
	AFFECT_DATA *paf;
	int cel = 0;

	if ( get_skill( ch_desc->ch, gsn_turn ) <= 0 )
		return FALSE;

    if ( ch_desc->ch->position != POS_FIGHTING )
        return FALSE;

	if ( IS_AFFECTED( ch_desc->ch, AFF_SILENCE ) )
		return FALSE;

	if ( is_undead( ch_desc->ch ) )
		return FALSE;

	paf = affect_find( ch_desc->ch->affected, gsn_turn );
	if ( paf && paf->level > ch_desc->ch->level / 5 )
		return FALSE;

	for( victim = ch_desc->ch->in_room->people; victim; victim = victim->next_in_room )
	{
		if ( !is_undead(victim) )
			continue;

		if ( victim->fighting != ch_desc->ch && ch_desc->ch->fighting != victim )
			continue;

		if (  victim->level > ch_desc->ch->level + 4 )
			continue;

		++cel;
	}


	if ( cel )
	    return TRUE;
	else
		return FALSE;
}

bool can_call_friend( struct char_desc *ch_desc, struct char_desc *desc )
{
	int spell;

	if ( ch_desc->ch->counter[4] != 0 )
		return FALSE;

	if( ch_desc->caster >= 0 )
	{
		for ( spell = 0; si_spell_info[ CALLING ][ spell ].name; spell++ )
	        if ( SPELL_MEMMED( ch_desc->ch, si_spell_info[ CALLING ][ spell ].sn ) )
            	return TRUE;
	}
	else if( EXT_IS_SET( ch_desc->ch->act, ACT_BLACK_KNIGHT ) && !EXT_IS_SET( ch_desc->ch->act, ACT_PRACTICE ))
	{
		if( !is_affected(ch_desc->ch, gsn_call_avatar ))
			return TRUE;
	}

	return FALSE;
}

bool should_call_friend( struct char_desc *ch_desc, struct char_desc *desc )
{
	CHAR_DATA *mobik = NULL;
	int liczba_mobikow = 0, max_mobikow = 1;
	//coby moby nie rozjebaly muda iloscia summonow - ograniczenie tutaj, bo w add_charm nie ma
	for( mobik = ch_desc->ch->in_room->people; mobik; mobik = mobik->next_in_room )
	{
		if( mobik->master == ch_desc->ch )
			++liczba_mobikow;
	}

	if ( get_curr_stat_deprecated( ch_desc->ch, STAT_CHA ) > 22 )
		max_mobikow = 4;
	else if ( get_curr_stat_deprecated( ch_desc->ch, STAT_CHA ) > 15 )
		max_mobikow = 3;
	else if ( get_curr_stat_deprecated ( ch_desc->ch, STAT_CHA ) > 8 )
		max_mobikow = 2;
	else
		max_mobikow = 1;

	if( liczba_mobikow >= max_mobikow )
		return FALSE;

	if ( EXT_IS_SET( ch_desc->ch->in_room->room_flags, ROOM_PRIVATE ) )
		return FALSE;

	return TRUE;
}
