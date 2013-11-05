/*********************************************************************
*                                                                   *
* KILLER MUD is copyright 2006-2010 Killer MUD Staff (alphabetical) *
*                                                                   *
*   Pietrzak Marcin     (marcin.pietrzak@mud.pl        ) [Gurthg  ] *
*                                                                   *
*********************************************************************/
/* $Id: paladin_auras.c 9300 2010-06-18 22:06:30Z illi $*/
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
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
/*
 *
 * AURY PALADYÑSKIE
 *
 */

extern char * target_name;
bool check_dispel               args ( ( int dis_level, CHAR_DATA *victim, int sn ) );
bool has_aura                   args ( ( CHAR_DATA *ch, bool show_messages ) );
bool remove_obj                 args ( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
bool saves_dispel               args ( ( int dis_level, int spell_level, int duration ) );
int  get_aura_modifier          args ( ( CHAR_DATA *ch ) );
int  get_aura_sn                args ( ( CHAR_DATA *ch ) );
void char_updiate_paladin_auras args ( ( CHAR_DATA *ch ) );
void raw_kil                    args ( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void set_aura_spell             args ( ( CHAR_DATA *ch, CHAR_DATA *victim, int sn, int modifier ) );
void set_fighting               args ( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

/**
 * colors
 *
 * aura of protection       - bladoniebieska
 * aura of precision        - bladopomarañczowa
 * aura of endurance        - jasnobia³a
 * aura of battle lust      - jasnoczerwona
 * aura of improved healing - jasnozielona
 * aura of vigor            - bladozielona
 *
 */

/**
 * Check
 */
bool has_aura( CHAR_DATA *ch, bool show_messages )
{
    if ( IS_AFFECTED( ch, AFF_AURA ) )
    {
        if ( show_messages )
        {
            switch ( ch->sex )
            {
                case 0 :
                    send_to_char( "Ju¿ jeste¶ otoczony aur±.\n\r", ch );
                    break;
                case 1 :
                    send_to_char( "Ju¿ jeste¶ otoczona aur±.\n\r", ch );
                    break;
                case 2 :
                default:
                    send_to_char( "Ju¿ jeste¶ otoczone aur±.\n\r", ch );
                    break;
            }
        }
        return TRUE;
    }
    return FALSE;
}

/**
 * set aura spell
 */
void set_aura_spell ( CHAR_DATA *ch, CHAR_DATA *victim, int sn, int modifier )
{
    AFFECT_DATA af;
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = ch->level;
    af.location  = APPLY_NONE;
    af.modifier  = modifier;
    af.bitvector = &AFF_AURA;
    /**
     * tylko rzucajacy paladyn na pelen czas,
     * reszta jako schodzacy dodatek
     */
    if ( ch == victim )
    {
        af.duration    = 20 + ch->level;
        af.rt_duration = 20 + ch->level;
    }
    else
    {
        af.duration    = 2;
        af.rt_duration = 0;
    }
    affect_to_char( victim, &af, NULL, TRUE );
}

/**
 * get SN if has aura
 */
int get_aura_sn ( CHAR_DATA *ch )
{
    AFFECT_DATA *af;
    if ( has_aura ( ch, FALSE ) )
    {
        for ( af = ch->affected; af != NULL; af = af->next )
        {
            if ( af->bitvector == &AFF_AURA )
            {
                return af->type;
            }
        }
    }
    return 0;
}

/**
 * get modifier if has aura
 */
int get_aura_modifier ( CHAR_DATA *ch )
{
    AFFECT_DATA *af;
    if ( has_aura ( ch, FALSE ) )
    {
        for ( af = ch->affected; af != NULL; af = af->next )
        {
            if ( af->bitvector == &AFF_AURA )
            {
                return af->modifier;
            }
        }
    }
    return 0;
}

/**
 * spell_aura_of_protection
 *
 * spell_aura_of_protection - protects group members, adding AC & some saves
 *
 * -2 AC (paladyn)
 * -1 AC (grupa)
 * +3 AC (evil)
 */
void spell_aura_of_protection( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    if ( has_aura ( ch, TRUE ) )
    {
        return;
    }
    set_aura_spell ( ch, ch, sn, -2 );
    act( "$n otacza siê bladoniebiesk± aur±.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "Otaczasz siê bladoniebiesk± aur± ochrony.\r\n", ch );
    return;
}

/**
 * +2 to hit (paladyn)
 * +1 to hit (grupa)
 */
void spell_aura_of_precision( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    if ( has_aura ( ch, TRUE ) )
    {
        return;
    }
    set_aura_spell ( ch, ch, sn, 2 );
    act( "$n otacza siê bladopomarañczow± aur±.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "Otaczasz siê bladopomarañczow± aur± precyzji.\r\n", ch );
    return;
}

/**
 * -10 to resist elements (paladyn)
 *  -5 to resist elements (grupa)
 *  50 to evil
 */
void spell_aura_of_endurance( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    if ( has_aura ( ch, TRUE ) )
    {
        return;
    }
    set_aura_spell ( ch, ch, sn, -10 );
    act( "$n otacza siê jasnobia³± aur±.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "Otaczasz siê jasnobia³± aur± ochrony.\r\n", ch );
    return;
}

/**
 * fast healing
 */
void spell_aura_of_improved_healing( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    if ( has_aura ( ch, TRUE ) )
    {
        return;
    }
    set_aura_spell ( ch, ch, sn, 1 );
    act( "$n otacza siê jasnozielon± aur±.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "Otaczasz siê jasnozielon± aur± ¿ycia.\r\n", ch );
    return;
}

/**
 * fast move recovery & -1 to move lose
 */
void spell_aura_of_vigor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    if ( has_aura ( ch, TRUE ) )
    {
        return;
    }
    set_aura_spell ( ch, ch, sn, 0 );
    act( "$n otacza siê bladozielon± aur±.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "Otaczasz siê bladozielon± aur± ¿ycia.\r\n", ch );
    return;
}

/**
 *  3 do obrazen ( paladyn )
 *  1 do obrazen ( grupa )
 * -5 do obrazen zadawanych przez zlych ( group )
 */
void spell_aura_of_battle_lust( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    if ( has_aura ( ch, TRUE ) )
    {
        return;
    }
    set_aura_spell ( ch, ch, sn, 3 );
    act( "$n otacza siê jasnoczerwon± aur±.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "Otaczasz siê jasnoczerwon± aur± zniszczenia.\r\n", ch );
    return;
}

/**
 * helper for:
 * char_update -> update.c
 */
void char_update_paladin_auras ( CHAR_DATA *ch )
{
    CHAR_DATA *gch;
    int sn;
    if ( !IS_NPC ( ch ) && ch->class == CLASS_PALADIN && has_aura( ch, FALSE ) )
    {
        sn = get_aura_sn( ch );
#ifdef INFO
        print_char( ch, "update aura: %d.\n\r", sn );
#endif
        if ( sn > 0 )
        {
            for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
            {
                if ( !is_same_group( gch, ch ) )
                {
                    continue;
                }
                if ( !has_aura( gch, FALSE ) && number_percent() > 80 - ch->level )
                {
                    switch ( sn )
                    {
                        case 462: // aura of protection
                            if ( IS_EVIL( gch ) )
                            {
                                set_aura_spell ( ch, gch, sn, 3 );
                            }
                            else
                            {
                                set_aura_spell ( ch, gch, sn, -1 );
                            }
                            act( "$n otacza siê bladoniebiesk± aur±.", gch, NULL, NULL, TO_ROOM );
                            send_to_char( "Otacza ciê bladoniebieska aura ochrony.\r\n", gch );
                            break;
                        case 463: // aura of precision
                            if ( !IS_EVIL( gch ) )
                            {
                                set_aura_spell ( ch, gch, sn, 1 );
                                act( "$n otacza siê bladopomarañczow± aur±.", gch, NULL, NULL, TO_ROOM );
                                send_to_char( "Otacza ciê bladopomarañczowa aura precyzji.\r\n", gch );
                            }
                            break;
                        case 464: // aura of endurance
                            if ( IS_EVIL( gch ) )
                            {
                                set_aura_spell ( ch, gch, sn, 50 );
                            }
                            else
                            {
                                set_aura_spell ( ch, gch, sn, -5 );
                            }
                            act( "$n otacza siê jasnobia³± aur±.", gch, NULL, NULL, TO_ROOM );
                            send_to_char( "Otacza ciê jasnobia³a aura ochrony.\r\n", gch );
                            break;
                        case 465: // aura of battle lust
                            if ( IS_EVIL( gch ) )
                            {
                                set_aura_spell ( ch, gch, sn, -5 );
                            }
                            else
                            {
                                set_aura_spell ( ch, gch, sn, 1 );
                            }
                            act( "$n otacza siê jasnoczerwon± aur±.", gch, NULL, NULL, TO_ROOM );
                            send_to_char( "Otacza ciê jasnoczerwona aura zniszczenia.\r\n", gch );
                            break;
                        case 466: // aura of improved healing
                            if ( !IS_EVIL( gch ) )
                            {
                                set_aura_spell ( ch, gch, sn, 1 );
                                act( "$n otacza siê jasnozielon± aur±.", gch, NULL, NULL, TO_ROOM );
                                send_to_char( "Otacza ciê jasnozielona aura ¿ycia.\r\n", gch );
                            }
                            break;
                        case 467: // aura of vigor
                            if ( !IS_EVIL( gch ) )
                            {
                                set_aura_spell ( ch, gch, sn, 0 );
                                act( "$n otacza siê bladozielon± aur±.", gch, NULL, NULL, TO_ROOM );
                                send_to_char( "Otacza ciê bladozielona aura ¿ycia.\r\n", gch );
                            }
                            break;
                    }
                }
            }
        }
    }
}
