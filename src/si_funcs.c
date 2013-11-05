/***********************************************************************
 *                                                                     *
 * KILLER MUD is copyright 2001-2011 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: si_funcs.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/si_funcs.c $
 *
 */
#define SI_FUNCS_C
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
#include "mount.h"

static struct si_spell_data	* memmed_spells[ MAX_SPELLS ];
void wear_obj_silent args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
void wield_weapon args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool primary ) );
void	say_spell	args( ( CHAR_DATA *ch, int sn ) );

/****  FUNKCJE DO AKCJI ****/
bool bash_caster( CHAR_DATA *ch, struct char_desc *desc )
{
    do_function( ch, &do_bash, get_target_by_name( ch, desc->ch ) );
    return TRUE;
}

bool charge_paralized( CHAR_DATA *ch, struct char_desc *desc )
{
    do_function( ch, &do_charge, get_target_by_name( ch, desc->ch ) );
    return TRUE;
}

bool just_assist( CHAR_DATA *ch, struct char_desc *desc )
{
    if ( ch->fighting )
        return TRUE;

    do_function( ch, &do_assist, "" );
    return TRUE;
}

bool just_stand( CHAR_DATA *ch, struct char_desc *desc )
{
    do_function( ch, &do_stand, "" );
    return TRUE;
}

bool rescue_friend( CHAR_DATA *ch, struct char_desc *desc )
{

    do_function( ch, &do_rescue, get_target_by_name( ch, desc->ch ) );
    return TRUE;
}

bool berserk( CHAR_DATA *ch, struct char_desc *desc )
{
    do_function( ch, &do_berserk, "" );
    return TRUE;
}

int choose_spell_for_reflect( CHAR_DATA *ch, struct char_desc *target )
{
    sh_int spell, spells_count = 0, si_roll = 0, reflect_chance = 0;

    si_roll = test_int( ch );

    switch ( si_roll )
    {
        case INT_GENIUS:
            reflect_chance = 98;
            break;
        case INT_VERY_HIGH:
            reflect_chance = 70;
            break;
        case INT_HIGH:
            reflect_chance = 50;
            break;
        case INT_AVERAGE:
            reflect_chance = 30;
            break;
        case INT_POOR:
            reflect_chance = 10;
            break;
        case INT_IDIOT:
            reflect_chance = 2;
            break;
    }

    if ( number_percent() < reflect_chance )
    {
        for ( spell = 0; si_spell_info[ HEALS ][ spell ].name; spell++ )
            if ( skill_table[ si_spell_info[ HEALS ][ spell ].sn ].target != TAR_CHAR_SELF &&
                 skill_table[ si_spell_info[ HEALS ][ spell ].sn ].minimum_position == POS_FIGHTING &&
                 SPELL_MEMMED( ch, si_spell_info[ HEALS ][ spell ].sn ) &&
                 ( !si_spell_info[ HEALS ][ spell ].check || !( ( *si_spell_info[ HEALS ][ spell ].check ) ( ch, target->ch, HEALS, spell ) ) ) )
                memmed_spells[ spells_count++ ] = &si_spell_info[ HEALS ][ spell ];

        for ( spell = 0; si_spell_info[ DEF_ANTITANK_SELF ][ spell ].name; spell++ )
            if ( skill_table[ si_spell_info[ DEF_ANTITANK_SELF ][ spell ].sn ].target != TAR_CHAR_SELF &&
                 skill_table[ si_spell_info[ HEALS ][ spell ].sn ].minimum_position == POS_FIGHTING &&
                 SPELL_MEMMED( ch, si_spell_info[ DEF_ANTITANK_SELF ][ spell ].sn ) &&
                 ( !si_spell_info[ DEF_ANTITANK_SELF ][ spell ].check || !( ( *si_spell_info[ DEF_ANTITANK_SELF ][ spell ].check ) ( ch, target->ch, DEF_ANTITANK_SELF, spell ) ) ) )
                memmed_spells[ spells_count++ ] = &si_spell_info[ DEF_ANTITANK_SELF ][ spell ];

        for ( spell = 0; si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].name; spell++ )
            if ( SPELL_MEMMED( ch, si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].sn ) &&
                 ( !si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].check || !( ( *si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].check ) ( ch, target->ch, DEF_ANTITANK_OTHER, spell ) ) ) )
                memmed_spells[ spells_count++ ] = &si_spell_info[ DEF_ANTITANK_OTHER ][ spell ];

        for ( spell = 0; si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].name; spell++ )
            if ( skill_table[ si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].sn ].target != TAR_CHAR_SELF &&
                 skill_table[ si_spell_info[ HEALS ][ spell ].sn ].minimum_position == POS_FIGHTING &&
                 SPELL_MEMMED( ch, si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].sn ) &&
                 ( !si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].check || !( ( *si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].check ) ( ch, target->ch, DEF_ANTIMAGIC_SELF, spell ) ) ) )
                memmed_spells[ spells_count++ ] = &si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ];

        for ( spell = 0; si_spell_info[ DEF_ANTIMAGIC_OTHER ][ spell ].name; spell++ )
            if ( SPELL_MEMMED( ch, si_spell_info[ DEF_ANTIMAGIC_OTHER ][ spell ].sn ) &&
                 ( !si_spell_info[ DEF_ANTIMAGIC_OTHER ][ spell ].check || !( ( *si_spell_info[ DEF_ANTIMAGIC_OTHER ][ spell ].check ) ( ch, target->ch, DEF_ANTIMAGIC_OTHER, spell ) ) ) )
                memmed_spells[ spells_count++ ] = &si_spell_info[ DEF_ANTIMAGIC_OTHER ][ spell ];
    }

    return spells_count;
}

bool do_some_healing( CHAR_DATA *ch, struct char_desc *desc )
{
    struct char_desc *target, *need_help = NULL;
    sh_int hp_loss, spell, spells_count = 0, si_roll, best_spell = -1;

    /* wybor celu - najbardziej ranny procentowo */
    for ( target = desc; target; target = target->next )
    {
        if ( !need_help )
        {
            need_help = target;
            continue;
        }

        if ( target->hp_percent < need_help->hp_percent )
            need_help = target;
    }

    hp_loss = get_max_hp(need_help->ch) - need_help->ch->hit;

    /* sprawdzanie dostepnych zaklec */
    for ( spell = 0; si_spell_info[ HEALS ][ spell ].name; spell++ )
        if ( SPELL_MEMMED( ch, si_spell_info[ HEALS ][ spell ].sn ) )
            memmed_spells[ spells_count++ ] = &si_spell_info[ HEALS ][ spell ];

    memmed_spells[ spells_count ] = NULL;

    if ( spells_count <= 0 )
        return TRUE;

    si_roll = test_int( ch );

    switch ( si_roll )
    {
            /* najlepsze do danej sytuacji */
        case INT_GENIUS:
        case INT_VERY_HIGH:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                /* jesli wystarczy */
                if ( memmed_spells[ spell ] ->extra > hp_loss &&
                     memmed_spells[ spell ] ->rating < memmed_spells[ best_spell ] ->rating )
                    best_spell = spell;
            }
            break;
            /* najlepsze jakie ma */
        case INT_HIGH:
        case INT_AVERAGE:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                /* jesli wystarczy */
                if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating )
                    best_spell = spell;
            }
            break;

            /* ktores z lepszych */
        case INT_POOR:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                /* na 30% wybierze lepsze */
                if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                     number_percent() < 30 )
                    best_spell = spell;
            }
            break;


            /* losowo */
        case INT_IDIOT:
            best_spell = number_range( 0, spells_count - 1 );
            break;
    }

    force_cast( ch, need_help->ch, memmed_spells[ best_spell ] ->sn );
    return TRUE;
}


bool disable_caster( CHAR_DATA *ch, struct char_desc *desc )
{
    struct char_desc *enemy, * target = NULL;
    sh_int spell, spells_count = 0, si_roll, best_spell = -1;

    /* wybor celu - najwyzszy poziom */
    for ( enemy = desc; enemy; enemy = enemy->next )
    {
        if ( IS_AFFECTED( enemy->ch, AFF_REFLECT_SPELL ) )
            continue;

        if ( !target )
        {
            target = enemy;
            continue;
        }

        if ( enemy->ch->level > target->ch->level )
            target = enemy;
    }

    if ( !target )
        target = desc;

    /* wybor czaru dla castera */
    if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
        spells_count = choose_spell_for_reflect( ch, target );
    else
    {
        for ( spell = 0; si_spell_info[ OFF_ANTICASTER ][ spell ].name; spell++ )
        {
            if ( !SPELL_MEMMED( ch, si_spell_info[ OFF_ANTICASTER ][ spell ].sn ) )
                continue;

            if ( si_spell_info[ OFF_ANTICASTER ][ spell ].check &&
                 ( ( *si_spell_info[ OFF_ANTICASTER ][ spell ].check ) ( ch, target->ch, OFF_ANTICASTER, spell ) ) )
                continue;

            memmed_spells[ spells_count++ ] = &si_spell_info[ OFF_ANTICASTER ][ spell ];
        }
    }

    memmed_spells[ spells_count ] = NULL;

    if ( spells_count <= 0 )
        return FALSE;

    si_roll = test_int( ch );

    switch ( si_roll )
    {
            /* najwyzszy rating */
        case INT_GENIUS:
        case INT_VERY_HIGH:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating )
                        best_spell = spell;
                }
            }
            break;
            /* najwyzszy rating z szansa 50% */
        case INT_HIGH:
        case INT_AVERAGE:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) &&
                     number_percent() < 50 )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                         number_percent() < 50 )
                        best_spell = spell;
                }
            }
            break;

            /* najwyzszy rating z szansa 10% */
        case INT_POOR:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) &&
                     number_percent() < 10 )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                         number_percent() < 10 )
                        best_spell = spell;
                }
            }
            break;

            /* losowo */
        case INT_IDIOT:
            best_spell = number_range( 0, spells_count - 1 );
            break;
    }

    force_cast( ch, target->ch, memmed_spells[ best_spell ] ->sn );
    return TRUE;
}


bool frag_tank( CHAR_DATA *ch, struct char_desc *desc )
{
    struct char_desc *enemy, * target = NULL;
    sh_int spell, spells_count = 0, si_roll, best_spell = -1;
    CHAR_DATA *cel;
    int count = 0;

    /* wybor celu - najbardziej ranny procentowo */
    for ( enemy = desc; enemy; enemy = enemy->next )
    {
        if ( IS_AFFECTED( enemy->ch, AFF_REFLECT_SPELL ) )
            continue;

		if ( IS_AFFECTED( enemy->ch, AFF_CHARM ) )
		{
			for( cel = ch->in_room->people ; cel ; cel = cel->next_in_room )
			{
				if ( !IS_NPC(cel) && cel->fighting == ch && can_see( ch, cel ) && !is_safe( ch, cel ) )
					++count;
			}
			if( count )
				continue;
		}

        if ( !target )
        {
            target = enemy;
            continue;
        }

        if ( target->hp_percent > enemy->hp_percent )
            target = enemy;
    }

    if ( !target )
    {
    	for( cel = ch->in_room->people ; cel ; cel = cel->next_in_room )
		{
			if ( !IS_NPC(cel) && cel->fighting == ch && can_see( ch, cel ) && !is_safe( ch, cel ) )
			{
				target = create_desc( cel, get_caster(cel), check_tank( cel ), ( 100 * cel->hit ) / UMAX( 1, get_max_hp(cel) ), can_move( cel ) );
				break;
			}
		}
	}

    if ( !target )
        target = desc;

    /* wybor czaru dla castera */
    if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
        spells_count = choose_spell_for_reflect( ch, target );
    else
    {
        for ( spell = 0; si_spell_info[ FRAGS ][ spell ].name; spell++ )
            if ( SPELL_MEMMED( ch, si_spell_info[ FRAGS ][ spell ].sn ) &&
                 ( !si_spell_info[ FRAGS ][ spell ].check || !( ( *si_spell_info[ FRAGS ][ spell ].check ) ( ch, target->ch, FRAGS, spell ) ) ) )
                memmed_spells[ spells_count++ ] = &si_spell_info[ FRAGS ][ spell ];
    }

    memmed_spells[ spells_count ] = NULL;

    if ( spells_count <= 0 )
        return FALSE;

    si_roll = test_int( ch );

    switch ( si_roll )
    {
            /* najwyzszy rating */
        case INT_GENIUS:
        case INT_VERY_HIGH:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating )
                        best_spell = spell;
                }
            }
            break;
            /* najwyzszy rating z szansa 50% */
        case INT_HIGH:
        case INT_AVERAGE:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) &&
                     number_percent() < 50 )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                         number_percent() < 50 )
                        best_spell = spell;
                }
            }
            break;

            /* najwyzszy rating z szansa 10% */
        case INT_POOR:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) &&
                     number_percent() < 10 )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                         number_percent() < 10 )
                        best_spell = spell;
                }
            }
            break;

            /* losowo */
        case INT_IDIOT:
            best_spell = number_range( 0, spells_count - 1 );
            break;
    }

    force_cast( ch, target->ch, memmed_spells[ best_spell ] ->sn );
    return TRUE;
}

bool disable_tank( CHAR_DATA *ch, struct char_desc *desc )
{
    struct char_desc *enemy, * target = NULL;
    sh_int spell, spells_count = 0, si_roll, best_spell = -1;

    /* wybor celu - najmniej hp % */
    for ( enemy = desc; enemy; enemy = enemy->next )
    {
        if ( IS_AFFECTED( enemy->ch, AFF_REFLECT_SPELL ) )
            continue;

        if ( !target )
        {
            target = enemy;
            continue;
        }

        if ( enemy->hp_percent < target->hp_percent )
            target = enemy;
    }

    if ( !target )
        target = desc;

    /* wybor czaru dla castera */
    if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
        spells_count = choose_spell_for_reflect( ch, target );
    else
    {
        for ( spell = 0; si_spell_info[ OFF_ANTITANK ][ spell ].name; spell++ )
        {
            if ( !SPELL_MEMMED( ch, si_spell_info[ OFF_ANTITANK ][ spell ].sn ) )
                continue;

            if ( si_spell_info[ OFF_ANTITANK ][ spell ].check &&
                 ( ( *si_spell_info[ OFF_ANTITANK ][ spell ].check ) ( ch, target->ch, OFF_ANTITANK, spell ) ) )
                continue;

            memmed_spells[ spells_count++ ] = &si_spell_info[ OFF_ANTITANK ][ spell ];
        }
    }

    memmed_spells[ spells_count ] = NULL;

    if ( spells_count <= 0 )
        return FALSE;

    si_roll = test_int( ch );

    switch ( si_roll )
    {
            /* najwyzszy rating */
        case INT_GENIUS:
        case INT_VERY_HIGH:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating )
                        best_spell = spell;
                }
            }
            break;
            /* najwyzszy rating z szansa 50% */
        case INT_HIGH:
        case INT_AVERAGE:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) &&
                     number_percent() < 50 )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                         number_percent() < 50 )
                        best_spell = spell;
                }
            }
            break;

            /* najwyzszy rating z szansa 10% */
        case INT_POOR:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) &&
                     number_percent() < 10 )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                         number_percent() < 10 )
                        best_spell = spell;
                }
            }
            break;

            /* losowo */
        case INT_IDIOT:
            best_spell = number_range( 0, spells_count - 1 );
            break;
    }

    force_cast( ch, target->ch, memmed_spells[ best_spell ] ->sn );
    return TRUE;
}

bool powerup_caster( CHAR_DATA *ch, struct char_desc *desc )
{
    struct char_desc *enemy;
    sh_int spell, spells_count = 0, si_roll, best_spell = -1;

    /* wybor czaru dla castera */
    for ( spell = 0; si_spell_info[ DEF_ANTITANK_SELF ][ spell ].name; spell++ )
    {
        if ( !SPELL_MEMMED( ch, si_spell_info[ DEF_ANTITANK_SELF ][ spell ].sn ) )
            continue;

        if ( si_spell_info[ DEF_ANTITANK_SELF ][ spell ].check &&
             ( ( *si_spell_info[ DEF_ANTITANK_SELF ][ spell ].check ) ( ch, ch, DEF_ANTITANK_SELF, spell ) ) )
            continue;

        memmed_spells[ spells_count++ ] = &si_spell_info[ DEF_ANTITANK_SELF ][ spell ];
    }

    /* jesli przeciwnik ma jakis casterow */
    for ( enemy = desc; enemy; enemy = enemy->next )
        if ( enemy->caster >= 0 && FALSE )
        {
            for ( spell = 0; spells_count < MAX_SPELL_INFO && si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].name; spell++ )
            {
                if ( !SPELL_MEMMED( ch, si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].sn ) )
                    continue;

                if ( si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].check &&
                     ( ( *si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].check ) ( ch, ch, DEF_ANTIMAGIC_SELF, spell ) ) )
                    continue;

                memmed_spells[ spells_count++ ] = &si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ];
            }

            break;
        }


    memmed_spells[ spells_count ] = NULL;


    if ( spells_count <= 0 )
        return FALSE;

    si_roll = test_int( ch );

    switch ( si_roll )
    {
            /* najwyzszy rating */
        case INT_GENIUS:
        case INT_VERY_HIGH:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                /* jesli wystarczy */
                if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating )
                    best_spell = spell;
            }
            break;
            /* najwyzszy rating z szansa 50% */
        case INT_HIGH:
        case INT_AVERAGE:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                /* jesli wystarczy */
                if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                     number_percent() < 50 )
                    best_spell = spell;
            }
            break;

            /* najwyzszy rating z szansa 10% */
        case INT_POOR:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                     number_percent() < 10 )
                    best_spell = spell;

            }
            break;

            /* losowo */
        case INT_IDIOT:
            best_spell = number_range( 0, spells_count - 1 );
            break;
    }

    force_cast( ch, ch, memmed_spells[ best_spell ] ->sn );
    return TRUE;
}

bool powerup_caster_not_fight( CHAR_DATA *ch, struct char_desc *desc )
{
    sh_int spell, spells_count = 0, si_roll, best_spell = -1;

    /* wybor czaru dla castera */
    for ( spell = 0; si_spell_info[ DEF_ANTITANK_SELF ][ spell ].name; spell++ )
    {
        if ( !SPELL_MEMMED( ch, si_spell_info[ DEF_ANTITANK_SELF ][ spell ].sn ) )
            continue;

        if ( si_spell_info[ DEF_ANTITANK_SELF ][ spell ].check &&
             ( ( *si_spell_info[ DEF_ANTITANK_SELF ][ spell ].check ) ( ch, ch, DEF_ANTITANK_SELF, spell ) ) )
            continue;

        memmed_spells[ spells_count++ ] = &si_spell_info[ DEF_ANTITANK_SELF ][ spell ];
    }

    for ( spell = 0; si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].name; spell++ )
    {
        if ( !SPELL_MEMMED( ch, si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].sn ) )
            continue;

        if ( si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].check &&
             ( ( *si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ].check ) ( ch, ch, DEF_ANTIMAGIC_SELF, spell ) ) )
            continue;

        memmed_spells[ spells_count++ ] = &si_spell_info[ DEF_ANTIMAGIC_SELF ][ spell ];
    }

    memmed_spells[ spells_count ] = NULL;


    if ( spells_count <= 0 )
        return FALSE;

    si_roll = test_int( ch );

    switch ( si_roll )
    {
            /* najwyzszy rating */
        case INT_GENIUS:
        case INT_VERY_HIGH:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                /* jesli wystarczy */
                if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating )
                    best_spell = spell;
            }
            break;
            /* najwyzszy rating z szansa 50% */
        case INT_HIGH:
        case INT_AVERAGE:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                /* jesli wystarczy */
                if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                     number_percent() < 50 )
                    best_spell = spell;
            }
            break;

            /* najwyzszy rating z szansa 10% */
        case INT_POOR:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                     number_percent() < 10 )
                    best_spell = spell;

            }
            break;

            /* losowo */
        case INT_IDIOT:
            best_spell = number_range( 0, spells_count - 1 );
            break;
    }

    force_cast( ch, ch, memmed_spells[ best_spell ] ->sn );
    return TRUE;
}


bool cast_stone_skin( CHAR_DATA *ch, struct char_desc *desc )
{
    force_cast( ch, ch, gsn_stone_skin );
    return TRUE;
}

bool frag_someone( CHAR_DATA *ch, struct char_desc *desc )
{
    struct char_desc *enemy, * target = NULL;
    sh_int spell, spells_count = 0, si_roll, best_spell = -1;
    CHAR_DATA *cel;
    int count = 0;

    /* wybor celu - najbardziej ranny procentowo */
    for ( enemy = desc; enemy; enemy = enemy->next )
    {
        if ( IS_AFFECTED( enemy->ch, AFF_REFLECT_SPELL ) )
            continue;

		if ( IS_AFFECTED( enemy->ch, AFF_CHARM ) )
		{
			for( cel = ch->in_room->people ; cel ; cel = cel->next_in_room )
			{
				if ( !IS_NPC(cel) && cel->fighting == ch && can_see( ch, cel ) && !is_safe( ch, cel ) )
					++count;
			}
			if( count )
				continue;
		}

        if ( !target )
        {
            target = enemy;
            continue;
        }

        if ( target->hp_percent < enemy->hp_percent )
            target = enemy;
    }

    if ( !target )
    {
    	for( cel = ch->in_room->people ; cel ; cel = cel->next_in_room )
		{
			if ( !IS_NPC(cel) && cel->fighting == ch && can_see( ch, cel ) && !is_safe( ch, cel ) )
			{
				target = create_desc( cel, get_caster(cel), check_tank( cel ), ( 100 * cel->hit ) / UMAX( 1, get_max_hp(cel) ), can_move( cel ) );
				break;
			}
		}
	}

	if ( !target )
		target = desc;

    /* wybor czaru dla castera */
    if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
        spells_count = choose_spell_for_reflect( ch, target );
    else
    {
        for ( spell = 0; si_spell_info[ FRAGS ][ spell ].name; spell++ )
            if ( SPELL_MEMMED( ch, si_spell_info[ FRAGS ][ spell ].sn ) &&
                 ( !si_spell_info[ FRAGS ][ spell ].check || !( ( *si_spell_info[ FRAGS ][ spell ].check ) ( ch, target->ch, FRAGS, spell ) ) ) )
                memmed_spells[ spells_count++ ] = &si_spell_info[ FRAGS ][ spell ];
    }

    memmed_spells[ spells_count ] = NULL;

    if ( spells_count <= 0 )
        return FALSE;

    si_roll = test_int( ch );

    switch ( si_roll )
    {
            /* najwyzszy rating */
        case INT_GENIUS:
        case INT_VERY_HIGH:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating )
                        best_spell = spell;
                }
            }
            break;
            /* najwyzszy rating z szansa 50% */
        case INT_HIGH:
        case INT_AVERAGE:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) &&
                         number_percent() < 50 )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                         number_percent() < 50 )
                        best_spell = spell;
                }
            }
            break;

            /* najwyzszy rating z szansa 10% */
        case INT_POOR:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) &&
                         number_percent() < 10 )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                         number_percent() < 10 )
                        best_spell = spell;
                }
            }
            break;

            /* losowo */
        case INT_IDIOT:
            best_spell = number_range( 0, spells_count - 1 );
            break;
    }

    force_cast( ch, target->ch, memmed_spells[ best_spell ] ->sn );
    return TRUE;
}

bool frag_caster( CHAR_DATA *ch, struct char_desc *desc )
{
    struct char_desc *enemy, * target = NULL;
    sh_int spell, spells_count = 0, si_roll, best_spell = -1;

    /* wybor celu - najbardziej ranny procentowo */
    for ( enemy = desc; enemy; enemy = enemy->next )
    {
        if ( IS_AFFECTED( enemy->ch, AFF_REFLECT_SPELL ) )
            continue;

        if ( !target )
        {
            target = enemy;
            continue;
        }

        if ( target->hp_percent < enemy->hp_percent )
            target = enemy;
    }

    if ( !target )
        target = desc;

    /* wybor czaru dla castera */
    if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
    {
        spells_count = choose_spell_for_reflect( ch, target );
    }
    else
    {
        for ( spell = 0; si_spell_info[ FRAGS ][ spell ].name; spell++ )
            if ( SPELL_MEMMED( ch, si_spell_info[ FRAGS ][ spell ].sn ) &&
                 ( !si_spell_info[ FRAGS ][ spell ].check || !( ( *si_spell_info[ FRAGS ][ spell ].check ) ( ch, target->ch, FRAGS, spell ) ) ) )
                memmed_spells[ spells_count++ ] = &si_spell_info[ FRAGS ][ spell ];
    }

    memmed_spells[ spells_count ] = NULL;

    if ( spells_count <= 0 )
        return FALSE;

    si_roll = test_int( ch );

    switch ( si_roll )
    {
            /* najwyzszy rating */
        case INT_GENIUS:
        case INT_VERY_HIGH:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating )
                        best_spell = spell;
                }
            }
            break;
            /* najwyzszy rating z szansa 50% */
        case INT_HIGH:
        case INT_AVERAGE:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) &&
                         number_percent() < 50 )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                         number_percent() < 50 )
                        best_spell = spell;
                }
            }
            break;

            /* najwyzszy rating z szansa 10% */
        case INT_POOR:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( IS_AFFECTED( target->ch, AFF_REFLECT_SPELL ) )
                {
                    if ( spell_circle( ch, memmed_spells[spell]->sn ) > spell_circle( ch, memmed_spells[best_spell]->sn ) &&
                         number_percent() < 10 )
                        best_spell = spell;
                }
                else
                {
                    if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                         number_percent() < 10 )
                        best_spell = spell;
                }
            }
            break;

            /* losowo */
        case INT_IDIOT:
            best_spell = number_range( 0, spells_count - 1 );
            break;
    }

    force_cast( ch, target->ch, memmed_spells[ best_spell ] ->sn );
    return TRUE;
}


/* use skills...all stuff */

#define SK_KICK			0
#define SK_BASH			1
#define SK_DISARM		2
#define SK_PARRY		3
#define SK_TRIP			4
#define SK_DODGE		5
#define SK_STUN			6
#define SK_MIGHT		7
#define SK_POWER		8
#define SK_CRITICAL		9
#define SK_CRUSH		10
#define SK_TAIL 		11
#define SK_TORMENT 		12
#define SK_SMITE_EVIL 	13
#define SK_SMITE_GOOD 	14
#define SK_CLEAVE 		15
#define SK_OVERWHELMING 16
#define SK_VERTICAL		17
#define SK_FIRE_BREATH 18
#define SK_FROST_BREATH 19
#define SK_ACID_BREATH 20
#define SK_LIGHTNING_BREATH 21
#define SK_SLAM      22
#define SK_MAX			23
/* MOBY moga uzywac skilli podczas berserka!*/
void mod_sk_prior( int skill_table[], int skill, int value )
{
    if ( skill_table[ skill ] != 0 )
        skill_table[ skill ] += value;
    return ;
}

bool use_skills( CHAR_DATA *ch, struct char_desc *desc )
{
    CHAR_DATA *victim = ch->fighting, *tch;
    OBJ_DATA *weapon;
    int number, crowd = 0;
    int skills[ SK_MAX ];
    int max_prior = 0, skill = -1;

    for ( number = 0; number < SK_MAX; number++ )
        skills[ number ] = 0;

    if ( get_skill( ch, gsn_bash ) > 0 && victim->position > POS_SITTING )
        skills[ SK_BASH ] = 5;

    if ( get_skill( ch, gsn_slam ) > 0 && victim->position > POS_SITTING )
        skills[ SK_SLAM ] = 4;

    if ( get_skill( ch, gsn_stun ) > 0 &&
		( ( weapon = get_eq_char(ch,WEAR_WIELD)) != NULL ) &&
		( weapon->value[0] == WEAPON_MACE || weapon->value[0] == WEAPON_FLAIL || weapon->value[0] == WEAPON_STAFF ) &&
    	IS_SET( race_table[ GET_RACE( victim ) ].type, PERSON ) )
        skills[ SK_STUN ] = 5;

    if ( get_skill( ch, gsn_disarm ) > 0 &&
         ( get_eq_char( victim, WEAR_WIELD ) || get_eq_char( victim, WEAR_SECOND ) ) )
        skills[ SK_DISARM ] = 5;

    if ( get_skill( ch, gsn_parry ) > 0 &&
         ( get_eq_char( ch, WEAR_WIELD ) || get_eq_char( ch, WEAR_SECOND ) ) )
        skills[ SK_PARRY ] = 5;

    if ( get_skill( ch, gsn_dodge ) > 0 )
        skills[ SK_DODGE ] = 5;

    if ( get_skill( ch, gsn_kick ) > 0 )
        skills[ SK_KICK ] = 5;

    if ( get_skill( ch, gsn_crush ) > 0 )
        skills[ SK_CRUSH ] = 5;

    if ( get_skill( ch, gsn_tail ) > 0 )
        skills[ SK_TAIL ] = 5;

    if ( get_skill( ch, gsn_trip ) && !IS_AFFECTED( victim, AFF_FLYING ) && !IS_AFFECTED( victim, AFF_FLOAT ) && victim->position == POS_FIGHTING )
        skills[ SK_TRIP ] = 5;

    if ( get_skill( ch, gsn_mighty_blow ) > 0 &&
         get_eq_char( ch, WEAR_WIELD ) )
        skills[ SK_MIGHT ] = 5;

    if ( get_skill( ch, gsn_power_strike ) > 0 &&
         get_eq_char( ch, WEAR_WIELD ) )
        skills[ SK_POWER ] = 5;

    if ( get_skill( ch, gsn_critical_strike ) > 0 &&
         get_eq_char( ch, WEAR_WIELD ) )
        skills[ SK_CRITICAL ] = 5;

    if ( get_skill( ch, gsn_torment ) > 0 &&
         !is_affected( ch, gsn_torment ) &&
         !IS_EVIL(victim) )
        skills[ SK_TORMENT ] = 5;

    if ( get_skill( ch, gsn_smite_good ) > 0 &&
         ch->counter[ 4 ] == 0 &&
         get_eq_char( ch, WEAR_WIELD ) &&
         IS_GOOD(victim) )
        skills[ SK_SMITE_GOOD ] = 5;

    if ( get_skill( ch, gsn_smite ) > 0 &&
         ch->counter[ 4 ] == 0 &&
         get_eq_char( ch, WEAR_WIELD ) &&
         IS_EVIL(victim) )
        skills[ SK_SMITE_EVIL ] = 5;

	if ( get_skill( ch, gsn_overwhelming_strike ) > 0 &&
		( ( weapon = get_eq_char(ch,WEAR_WIELD)) != NULL ) &&
		IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS ) &&
		weapon->value[0] != WEAPON_DAGGER &&
		weapon->value[0] != WEAPON_WHIP &&
		weapon->value[0] != WEAPON_SHORTSWORD &&
		weapon->value[0] != WEAPON_CLAWS)
		skills[ SK_OVERWHELMING ] = 5;

	if ( get_skill( ch, gsn_vertical_slash ) > 0 &&
		( ( weapon = get_eq_char(ch,WEAR_WIELD)) != NULL ) &&
		IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS ) &&
		weapon->value[0] != WEAPON_DAGGER &&
		weapon->value[0] != WEAPON_WHIP &&
		weapon->value[0] != WEAPON_MACE &&
		weapon->value[0] != WEAPON_STAFF &&
		weapon->value[0] != WEAPON_SPEAR &&
		weapon->value[0] != WEAPON_FLAIL &&
		weapon->value[0] != WEAPON_SHORTSWORD &&
		weapon->value[0] != WEAPON_CLAWS )
		skills[ SK_VERTICAL ] = 5;

	if ( get_skill( ch, gsn_cleave) > 0 &&
		( ( weapon = get_eq_char(ch,WEAR_WIELD)) != NULL ) &&
		IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS ) &&
		weapon->value[0] != WEAPON_DAGGER &&
		weapon->value[0] != WEAPON_WHIP &&
		weapon->value[0] != WEAPON_MACE &&
		weapon->value[0] != WEAPON_STAFF &&
		weapon->value[0] != WEAPON_SPEAR &&
		weapon->value[0] != WEAPON_FLAIL &&
		weapon->value[0] != WEAPON_SHORTSWORD &&
		weapon->value[0] != WEAPON_CLAWS )
	{
		for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
			if ( tch->fighting == ch )
				crowd++;

		if ( crowd > 1 )
			skills[ SK_CLEAVE ] = 5;
	}

   if ( get_skill( ch, gsn_fire_breath ) > 0 )
        skills[ SK_FIRE_BREATH ] = 5;

   if ( get_skill( ch, gsn_frost_breath ) > 0 )
        skills[ SK_FROST_BREATH ] = 5;

   if ( get_skill( ch, gsn_acid_breath ) > 0 )
        skills[ SK_ACID_BREATH ] = 5;

   if ( get_skill( ch, gsn_lightning_breath ) > 0 )
        skills[ SK_LIGHTNING_BREATH ] = 5;

    switch ( get_caster( victim ) )
    {
        case 0:
            mod_sk_prior( skills, SK_BASH, number_range( 25, 50 ) );
            mod_sk_prior( skills, SK_SLAM, number_range( 25, 50 ) );
            mod_sk_prior( skills, SK_TRIP, number_range( 25, 50 ) );
            mod_sk_prior( skills, SK_KICK, number_range( 15, 30 ) );
            mod_sk_prior( skills, SK_STUN, number_range( 25, 75 ) );
            mod_sk_prior( skills, SK_MIGHT, number_range( 25, 45 ) );
            mod_sk_prior( skills, SK_POWER, number_range( 35, 65 ) );
            mod_sk_prior( skills, SK_CRITICAL, number_range( 35, 75 ) );
            mod_sk_prior( skills, SK_CRUSH, number_range( 35, 75 ) );
            mod_sk_prior( skills, SK_TAIL, number_range( 25, 75 ) );
            mod_sk_prior( skills, SK_TORMENT, number_range( 60, 95 ) );
            mod_sk_prior( skills, SK_SMITE_EVIL, number_range( 45, 85 ) );
            mod_sk_prior( skills, SK_SMITE_GOOD, number_range( 45, 85 ) );
            mod_sk_prior( skills, SK_CLEAVE, number_range( 25, 75 ) );
            mod_sk_prior( skills, SK_OVERWHELMING, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_VERTICAL, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_FIRE_BREATH, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_FROST_BREATH, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_ACID_BREATH, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_LIGHTNING_BREATH, number_range( 40, 75 ) );
            break;
        case 1:
            mod_sk_prior( skills, SK_BASH, number_range( 25, 60 ) );
            mod_sk_prior( skills, SK_SLAM, number_range( 25, 60 ) );
            mod_sk_prior( skills, SK_TRIP, number_range( 25, 50 ) );
            mod_sk_prior( skills, SK_KICK, number_range( 15, 40 ) );
            mod_sk_prior( skills, SK_STUN, number_range( 25, 75 ) );
            mod_sk_prior( skills, SK_MIGHT, number_range( 25, 45 ) );
            mod_sk_prior( skills, SK_POWER, number_range( 35, 65 ) );
            mod_sk_prior( skills, SK_CRITICAL, number_range( 35, 75 ) );
            mod_sk_prior( skills, SK_CRUSH, number_range( 35, 75 ) );
            mod_sk_prior( skills, SK_TAIL, number_range( 25, 75 ) );
            mod_sk_prior( skills, SK_TORMENT, number_range( 60, 95 ) );
            mod_sk_prior( skills, SK_SMITE_EVIL, number_range( 45, 85 ) );
            mod_sk_prior( skills, SK_SMITE_GOOD, number_range( 45, 85 ) );
            mod_sk_prior( skills, SK_CLEAVE, number_range( 25, 75 ) );
            mod_sk_prior( skills, SK_OVERWHELMING, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_VERTICAL, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_FIRE_BREATH, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_FROST_BREATH, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_ACID_BREATH, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_LIGHTNING_BREATH, number_range( 40, 75 ) );
            break;
        case 5:
            mod_sk_prior( skills, SK_BASH, number_range( 25, 60 ) );
            mod_sk_prior( skills, SK_SLAM, number_range( 25, 60 ) );
            mod_sk_prior( skills, SK_TRIP, number_range( 25, 50 ) );
            mod_sk_prior( skills, SK_KICK, number_range( 15, 40 ) );
            mod_sk_prior( skills, SK_STUN, number_range( 25, 75 ) );
            mod_sk_prior( skills, SK_MIGHT, number_range( 25, 45 ) );
            mod_sk_prior( skills, SK_POWER, number_range( 35, 65 ) );
            mod_sk_prior( skills, SK_CRITICAL, number_range( 35, 75 ) );
            mod_sk_prior( skills, SK_CRUSH, number_range( 35, 75 ) );
            mod_sk_prior( skills, SK_TAIL, number_range( 25, 75 ) );
            mod_sk_prior( skills, SK_TORMENT, number_range( 60, 95 ) );
            mod_sk_prior( skills, SK_SMITE_EVIL, number_range( 45, 85 ) );
            mod_sk_prior( skills, SK_SMITE_GOOD, number_range( 45, 85 ) );
            mod_sk_prior( skills, SK_CLEAVE, number_range( 25, 75 ) );
            mod_sk_prior( skills, SK_OVERWHELMING, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_VERTICAL, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_FIRE_BREATH, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_FROST_BREATH, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_ACID_BREATH, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_LIGHTNING_BREATH, number_range( 40, 75 ) );
            break;
        case 4:
        case - 1:
            if ( ch->level > ch->fighting->level )
            {
                mod_sk_prior( skills, SK_BASH, number_range( 25, 50 ) );
                mod_sk_prior( skills, SK_SLAM, number_range( 25, 50 ) );
                mod_sk_prior( skills, SK_TRIP, number_range( 15, 50 ) );
                mod_sk_prior( skills, SK_KICK, number_range( 15, 60 ) );
                mod_sk_prior( skills, SK_DISARM, number_range( 15, 60 ) );
                mod_sk_prior( skills, SK_PARRY, number_range( 35, 70 ) );
                mod_sk_prior( skills, SK_DODGE, number_range( 35, 70 ) );
                mod_sk_prior( skills, SK_STUN, number_range( 25, 75 ) );
            }
            else
            {
                mod_sk_prior( skills, SK_BASH, number_range( 10, 35 ) );
                mod_sk_prior( skills, SK_SLAM, number_range( 10, 35 ) );
                mod_sk_prior( skills, SK_TRIP, number_range( 15, 40 ) );
                mod_sk_prior( skills, SK_KICK, number_range( 15, 60 ) );
                mod_sk_prior( skills, SK_DISARM, number_range( 15, 60 ) );
                mod_sk_prior( skills, SK_PARRY, number_range( 35, 60 ) );
                mod_sk_prior( skills, SK_DODGE, number_range( 45, 85 ) );
                mod_sk_prior( skills, SK_STUN, number_range( 25, 75 ) );
            }

            mod_sk_prior( skills, SK_MIGHT, number_range( 25, 45 ) );
            mod_sk_prior( skills, SK_POWER, number_range( 35, 75 ) );
            mod_sk_prior( skills, SK_CRITICAL, number_range( 35, 75 ) );
            mod_sk_prior( skills, SK_CRUSH, number_range( 35, 75 ) );
            mod_sk_prior( skills, SK_TAIL, number_range( 25, 75 ) );
            mod_sk_prior( skills, SK_TORMENT, number_range( 60, 95 ) );
            mod_sk_prior( skills, SK_SMITE_EVIL, number_range( 45, 85 ) );
            mod_sk_prior( skills, SK_SMITE_GOOD, number_range( 45, 85 ) );
            mod_sk_prior( skills, SK_CLEAVE, number_range( 25, 75 ) );
            mod_sk_prior( skills, SK_OVERWHELMING, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_VERTICAL, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_FIRE_BREATH, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_FROST_BREATH, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_ACID_BREATH, number_range( 40, 75 ) );
            mod_sk_prior( skills, SK_LIGHTNING_BREATH, number_range( 40, 75 ) );

            break;

        default: break;
    }


    for ( number = 0; number < SK_MAX; number++ )
        if ( skills[ number ] > max_prior )
        {
            skill = number;
            max_prior = skills[ number ];
        }


    switch ( skill )
    {
        case SK_KICK:
            do_function( ch, &do_kick, "" );
            return TRUE;
        case SK_DODGE:
            do_function( ch, &do_dodge, "" );
            return TRUE;
        case SK_TRIP:
            do_function( ch, &do_trip, "" );
            return TRUE;
        case SK_PARRY:
            do_function( ch, &do_parry, "" );
            return TRUE;
        case SK_BASH:
            do_function( ch, &do_bash, "" );
            return TRUE;
        case SK_SLAM:
            do_function( ch, &do_slam, "" );
            return TRUE;
        case SK_DISARM:
            do_function( ch, &do_disarm, "" );
            return TRUE;
        case SK_STUN:
            do_function( ch, &do_stun, "" );
            return TRUE;
        case SK_MIGHT:
            do_function( ch, &do_might, "" );
            return TRUE;
        case SK_POWER:
            do_function( ch, &do_power, "" );
            return TRUE;
        case SK_CRITICAL:
            do_function( ch, &do_critical, "" );
            return TRUE;
        case SK_CRUSH:
            do_function( ch, &do_crush, "" );
            return TRUE;
        case SK_TAIL:
            do_function( ch, &do_tail, "" );
            return TRUE;
        case SK_TORMENT:
            do_function( ch, &do_torment, "" );
            return TRUE;
        case SK_SMITE_GOOD:
            do_function( ch, &do_smite, "" );
            return TRUE;
        case SK_SMITE_EVIL:
            do_function( ch, &do_smite, "" );
            return TRUE;
        case SK_OVERWHELMING:
            do_function( ch, &do_overwhelming, "" );
            return TRUE;
        case SK_VERTICAL:
            do_function( ch, &do_vertical_slash, "" );
            return TRUE;
        case SK_CLEAVE:
            do_function( ch, &do_cleave, "" );
            return TRUE;
        case SK_FIRE_BREATH:
            do_function( ch, &do_fire_breath, "" );
            return TRUE;
        case SK_FROST_BREATH:
            do_function( ch, &do_frost_breath, "" );
            return TRUE;
        case SK_ACID_BREATH:
            do_function( ch, &do_acid_breath, "" );
            return TRUE;
        case SK_LIGHTNING_BREATH:
            do_function( ch, &do_lightning_breath, "" );
            return TRUE;
        default: break;
    }

    return FALSE;
}

bool charge( CHAR_DATA *ch, struct char_desc *desc )
{
    struct char_desc *target, *victim = NULL;

    /* wybor celu - najbardziej ranny procentowo */
    for ( target = desc; target; target = target->next )
    {
        if ( !victim )
        {
            victim = target;
            continue;
        }

        if ( target->hp_percent < victim->hp_percent )
            victim = target;
    }


    do_function( ch, &do_charge, get_target_by_name( ch, victim->ch ) );
    return TRUE;
}

bool backstab( CHAR_DATA *ch, struct char_desc *desc )
{
    struct char_desc *target, *victim = NULL;

    /* wybor celu - najbardziej ranny procentowo */
    for ( target = desc; target; target = target->next )
    {
        if ( !victim )
        {
            victim = target;
            continue;
        }

        if ( target->hp_percent > victim->hp_percent )
            victim = target;
    }


    do_function( ch, &do_backstab, get_target_by_name( ch, victim->ch ) );
    return TRUE;
}

bool flee_and_hunt( CHAR_DATA *ch, struct char_desc *desc )
{
    ROOM_INDEX_DATA *was_in;
    CHAR_DATA *victim;
    EXIT_DATA *pexit;
    int door, attempt;

    if ( ( was_in = ch->in_room ) == NULL )
        return FALSE;

    if ( IS_AFFECTED( ch, AFF_BERSERK ) )
        return FALSE;

    for ( attempt = 0; attempt < 6; attempt++ )
    {
        door = number_door( );

        if ( ( pexit = was_in->exit[ door ] ) == 0
             || pexit->u1.to_room == NULL
             || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER )
             || IS_SET( pexit->exit_info, EX_CLOSED )
             || IS_SET( pexit->exit_info, EX_HIDDEN )
             || IS_SET( pexit->exit_info, EX_SECRET )
             || ( IS_NPC( ch )
                  && ( EXT_IS_SET( pexit->u1.to_room->room_flags, ROOM_NO_MOB )
                       || IS_SET( pexit->exit_info, EX_NO_MOB ) ) ) )
            continue;

        victim = ch->fighting;
        stop_fighting( ch, TRUE );
        act( "$n wycofuje sie i ucieka!", ch, NULL, NULL, TO_ROOM );
        move_char( ch, door, FALSE, NULL );
        start_hunting( ch, victim );

        if ( ch->level > number_range( 1, 25 ) )
        {
            EXT_SET_BIT( ch->affected_by, AFF_HIDE );
            EXT_SET_BIT( ch->affected_by, AFF_SNEAK );
        }
        return TRUE;
    }
    return FALSE;
}

bool make_some_magic( CHAR_DATA *ch, struct char_desc *desc )
{
    struct char_desc *target, *need_help = NULL;
    sh_int spell, spells_count = 0, si_roll, best_spell = -1;

    for ( target = desc; target; target = target->next )
    {
        if ( !need_help )
        {
            need_help = target;
            continue;
        }

        if ( target->hp_percent < need_help->hp_percent )
            need_help = target;
    }

    /* wybor czaru dla castera */
    for ( spell = 0; si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].name; spell++ )
    {
        if ( !SPELL_MEMMED( ch, si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].sn ) )
            continue;

        if ( si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].check &&
             ( ( *si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].check ) ( ch, need_help->ch, DEF_ANTITANK_OTHER, spell ) ) )
            continue;

        memmed_spells[ spells_count++ ] = &si_spell_info[ DEF_ANTITANK_OTHER ][ spell ];
    }

    memmed_spells[ spells_count ] = NULL;

    if ( spells_count == 0 )
        return FALSE;

    si_roll = test_int( ch );

    switch ( si_roll )
    {
            /* najwyzszy rating */
        case INT_GENIUS:
        case INT_VERY_HIGH:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                /* jesli wystarczy */
                if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating )
                    best_spell = spell;
            }
            break;
            /* najwyzszy rating z szansa 50% */
        case INT_HIGH:
        case INT_AVERAGE:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                /* jesli wystarczy */
                if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                     number_percent() < 50 )
                    best_spell = spell;
            }
            break;

            /* najwyzszy rating z szansa 10% */
        case INT_POOR:
            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            {
                if ( best_spell < 0 )
                {
                    best_spell = spell;
                    continue;
                }

                if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                     number_percent() < 10 )
                    best_spell = spell;

            }
            break;

            /* losowo */
        case INT_IDIOT:
            best_spell = number_range( 0, spells_count - 1 );
            break;
    }

    force_cast( ch, need_help->ch, memmed_spells[ best_spell ] ->sn );
    return TRUE;
}

//wrzuca z automatu co tam ma na pierwszego z brzegu...
//gupie ale dziala
bool make_some_magic_not_fight( CHAR_DATA *ch, struct char_desc *desc )
{
    struct char_desc *target;
    sh_int spell;

    for ( spell = 0; si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].name; spell++ )
    {
        if ( !SPELL_MEMMED( ch, si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].sn ) )
            continue;

        if ( !si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].check )
            continue;

        for ( target = desc; target; target = target->next )
        {
            if ( ( *si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].check ) ( ch, target->ch, DEF_ANTITANK_OTHER, spell ) )
                continue;

            force_cast( ch, target->ch, si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].sn );
            return TRUE;
        }
    }

    for ( spell = 0; si_spell_info[ DEF_ANTIMAGIC_OTHER ][ spell ].name; spell++ )
    {
        if ( !SPELL_MEMMED( ch, si_spell_info[ DEF_ANTIMAGIC_OTHER ][ spell ].sn ) )
            continue;

        if ( !si_spell_info[ DEF_ANTITANK_OTHER ][ spell ].check )
            continue;

        for ( target = desc; target; target = target->next )
        {
            if ( ( *si_spell_info[ DEF_ANTIMAGIC_OTHER ][ spell ].check ) ( ch, target->ch, DEF_ANTIMAGIC_OTHER, spell ) )
                continue;

            force_cast( ch, target->ch, si_spell_info[ DEF_ANTIMAGIC_OTHER ][ spell ].sn );
            return TRUE;
        }
    }


    return FALSE;
}

bool wardance( CHAR_DATA *ch, struct char_desc *desc )
{
    do_function( ch, &do_wardance, "" );
    return TRUE;
}

bool damage_reduction( CHAR_DATA *ch, struct char_desc *desc )
{
    do_function( ch, &do_damage, "" );
    return TRUE;
}

bool target_master( CHAR_DATA *ch, struct char_desc *desc )
{
	CHAR_DATA *victim, *cel = NULL;

    if ( !ch->fighting ||
         !ch->fighting->master )
        return FALSE;

	victim = ch->fighting->master;

	if ( victim->in_room != ch->in_room )
	{
		for( victim = ch->in_room->people ; victim ; victim = victim->next_in_room )
		{
			if( !IS_NPC(victim) && can_see(ch,victim) && !is_safe( ch, victim ) && victim->fighting == ch && get_caster(victim) == -1 )
			{
				cel = victim;
				break;
			}
		}
	}
	else
		cel = victim;

	if( cel == NULL )
		return FALSE;

    do_function( ch, &do_target, get_target_by_name( ch, cel ));
    return TRUE;
}


bool nasty_hold( CHAR_DATA *ch, struct char_desc *desc )
{
    struct char_desc *enemy, * target = NULL;
    /* wybor celu - najwyzszy levelem */
    for ( enemy = desc; enemy; enemy = enemy->next )
    {
        if ( !target )
        {
            target = enemy;
            continue;
        }

        if ( target->ch->level < enemy->ch->level )
            target = enemy;
    }

    if ( SPELL_MEMMED( ch, gsn_holdperson ) )
    {
        force_cast( ch, target->ch, gsn_holdperson );
        return TRUE;
    }

    if ( SPELL_MEMMED( ch, gsn_holdmonster ) )
    {
        force_cast( ch, target->ch, gsn_holdmonster );
        return TRUE;
    }


    return FALSE;
}

bool just_rest( CHAR_DATA *ch, struct char_desc *desc )
{
    if( ch->mount == NULL && ch->mounting == NULL )
    {
        do_rest( ch, "" );
        return TRUE;
    }
    else
        return FALSE;

}

bool hp_regen( CHAR_DATA *ch, struct char_desc *desc )
{
    if( ch->mount == NULL && ch->mounting == NULL )
    {
        do_rest( ch, "" );

        if ( get_skill( ch, gsn_recuperate ) > 0 )
            do_recuperate( ch, "" );

        return TRUE;
    }
    else
        return FALSE;
}

bool lay_friend( CHAR_DATA *ch, struct char_desc *desc )
{
    struct char_desc *target, *need_help = NULL;

    if( ch->position == POS_SLEEPING || ch->position == POS_SITTING || ch->position == POS_RESTING )
    	do_stand( ch, "" );

    /* wybor celu - najbardziej ranny procentowo */
    for ( target = desc; target; target = target->next )
    {
        if ( !need_help )
        {
            need_help = target;
            continue;
        }

        if ( target->hp_percent < need_help->hp_percent )
            need_help = target;
    }

    do_lay( ch, get_target_by_name( ch, need_help->ch ) );
    return TRUE;
}

//taa, maly trick, wszyscy casterzy znaja light
//i rzucaja bez zamemowania ;)
//jesli jest ciemno i nie ma kulki - robi kilki - return
//jesli jest ciemno i ma kulke - zaklada ja ;)
bool make_light( CHAR_DATA *ch, struct char_desc *desc )
{
	int sn;
	OBJ_DATA *obj;

	//jesli ktos ma to niech wywala z tej funkcji
	if( IS_AFFECTED(ch, AFF_INFRARED) || IS_AFFECTED( ch, AFF_DARK_VISION) )
	{
		return TRUE;
	}

	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->pIndexData->vnum == 21 && obj->wear_loc == WEAR_NONE )
		{
			wear_obj( ch, obj, TRUE );
			return TRUE;
		}
	}

	sn = skill_lookup( "light" );
	if ( EXT_IS_SET( ch->act, ACT_DRUID ) )
	{
		sn = skill_lookup( "firefly swarm" );
	}


	say_spell( ch, sn );
	( *skill_table[ sn ].spell_fun ) ( sn, ch->level, ch, NULL, skill_table[ sn ].target );

	return TRUE;
}

bool summon_player( CHAR_DATA *ch, struct char_desc *desc )
{
    CHAR_DATA *victim;
    MEM_DATA * tmp;

    for ( tmp = ch->memory;tmp;tmp = tmp->next )
    {
        if ( tmp->who != NULL && tmp->reaction == MEM_HATE )
        {
            victim = tmp->who;
            break;
        }
    }
    if ( !victim  )
    {
        victim = ch->hunting;
    }
    if ( !victim )
    {
        bug( "Cos sie zjeba這 z checkami w si z summonem.", 0 );
        return FALSE;
    }
    if ( victim == ch )
    {
        bug( "Cos sie zjeba這 z checkami w si z summonem blad typu 2.", 0 );
        return FALSE;
    }
    /**
     * zabezpieczenie przed spell_fetch
     */
    if ( IS_NPC( victim ) )
    {
        victim = get_char_world( ch, victim->name );
        if ( !victim )
        {
            bug( "Cos sie zjeba這 z checkami w si z summonem (spell_fetch).", 0 );
            return FALSE;
        }
    }
    force_cast( ch, victim, 94 );
    return TRUE;
}

bool teleport_to_player( CHAR_DATA *ch, struct char_desc *desc )
{
	CHAR_DATA *victim;
	MEM_DATA * tmp;

	for ( tmp = ch->memory;tmp;tmp = tmp->next )
		if ( tmp->who != NULL && tmp->reaction == MEM_HATE )
		{
			victim = tmp->who;
			break;
		}

	if ( !victim  )
		victim = ch->hunting;

	if ( !victim )
	{
		bug( "Cos sie zjeba這 z checkami w si z dimensionem.", 0 );
		return FALSE;
	}

	force_cast( ch, victim, 218 );
    return TRUE;
}

bool just_wake( CHAR_DATA *ch, struct char_desc *desc )
{
    do_function( ch, &do_wake, "" );
    return TRUE;
}

bool turn_undead( CHAR_DATA *ch, struct char_desc *desc )
{
	OBJ_DATA *item_turner = NULL, *obj = NULL;

    if ( ch->mount )
    {
    	do_dismount_body(ch);
		ch->position = POS_FIGHTING;
	}

	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->item_type == ITEM_TURN )
		{
			switch ( obj->wear_loc )
			{
				case WEAR_HOLD:
				case WEAR_NECK_1:
				case WEAR_NECK_2:
					item_turner = obj;
					break;
				default: break;
			}
			if ( item_turner )
				break;
		}
	}

	if ( !item_turner )
	{
		obj = NULL;

		if ( IS_GOOD(ch) )
		{
			obj = create_object( get_obj_index( OBJ_VNUM_CREATE_SYMBOL_GOOD ), FALSE );
		}
		else if ( IS_EVIL(ch) )
		{
			obj = create_object( get_obj_index( OBJ_VNUM_CREATE_SYMBOL_EVIL ), FALSE );
		}
		else
		{
			obj = create_object( get_obj_index( OBJ_VNUM_CREATE_SYMBOL_NEUTRAL ), FALSE );
		}

		obj->value[ 1 ] = 2;
		obj->value[ 2 ] = 4;
		obj->value[ 5 ] = 4;
		obj->value[ 6 ] = 4;

		EXT_SET_BIT( obj->extra_flags, ITEM_UNDESTRUCTABLE 	);
		EXT_SET_BIT( obj->extra_flags, ITEM_NOREMOVE 		);
		EXT_SET_BIT( obj->extra_flags, ITEM_NODROP 			);
		EXT_SET_BIT( obj->extra_flags, ITEM_NOUNCURSE 		);
		EXT_SET_BIT( obj->extra_flags, ITEM_ROT_DEATH 		);

		obj_to_char( obj, ch );
		wear_obj_silent( ch, obj, TRUE );
		item_turner = obj;
	}

	do_function( ch, &do_turn, "" );
    return TRUE;
}

bool call_friend( CHAR_DATA *ch, struct char_desc *desc )
{
    sh_int spell, spells_count = 0, si_roll, best_spell = -1;

	if( EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ))
	{
		do_function( ch, &do_call_avatar, "" );
	}
	else
	{
    	/* wybor czaru dla castera */
    	for ( spell = 0; si_spell_info[ CALLING ][ spell ].name; spell++ )
    	{
	        if ( !SPELL_MEMMED( ch, si_spell_info[ CALLING ][ spell ].sn ) )
            	continue;

        	if ( si_spell_info[ CALLING ][ spell ].check &&
             	( ( *si_spell_info[ CALLING ][ spell ].check ) ( ch, ch, CALLING, spell ) ) )
            	continue;

        	memmed_spells[ spells_count++ ] = &si_spell_info[ CALLING ][ spell ];
    	}

    	memmed_spells[ spells_count ] = NULL;

	    if ( spells_count <= 0 )
        	return FALSE;

	    si_roll = test_int( ch );

    	switch ( si_roll )
    	{
            	/* najwyzszy rating */
        	case INT_GENIUS:
        	case INT_VERY_HIGH:
	            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            	{
	                if ( best_spell < 0 )
                	{
	                    best_spell = spell;
                    	continue;
                	}

                	/* jesli wystarczy */
                	if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating )
	                    best_spell = spell;
            	}
            	break;
            	/* najwyzszy rating z szansa 50% */
        	case INT_HIGH:
        	case INT_AVERAGE:
	            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            	{
	                if ( best_spell < 0 )
                	{
	                    best_spell = spell;
                    	continue;
                	}

                	/* jesli wystarczy */
                	if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                     	number_percent() < 50 )
                    	best_spell = spell;
            	}
            	break;

            	/* najwyzszy rating z szansa 10% */
        	case INT_POOR:
	            for ( spell = 0; memmed_spells[ spell ]; spell++ )
            	{
	                if ( best_spell < 0 )
                	{
	                    best_spell = spell;
                    	continue;
                	}

                if ( memmed_spells[ spell ] ->rating > memmed_spells[ best_spell ] ->rating &&
                     number_percent() < 10 )
                    best_spell = spell;

            	}
            	break;

            	/* losowo */
        	case INT_IDIOT:
	            best_spell = number_range( 0, spells_count - 1 );
            	break;
    	}

    	if( memmed_spells[ best_spell ] ->sn == 94 )
    		return FALSE;

	    force_cast( ch, ch, memmed_spells[ best_spell ] ->sn );
	}

    return TRUE;
}
#undef SI_FUNCS_C
