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

//#define INFO

/* analiza celu: hp, tankowanie, castowanie, aktywnosc */
struct char_desc * check_target( CHAR_DATA *vch )
{
    int caster, tank, hp_percent, active = 5;
    struct char_desc * desc;

    tank = check_tank( vch );
    caster = get_caster( vch );
    hp_percent = ( 100 * vch->hit ) / UMAX( 1, get_max_hp(vch) );

    if ( !can_move( vch ) )
        active = 0;

    if ( caster >= 0 )
    {
        if ( IS_AFFECTED( vch, AFF_SILENCE ) )
            active = 0;
        else if ( IS_AFFECTED( vch, AFF_DEAFNESS ) )
            active = 2;
        else if ( EXT_IS_SET( vch->in_room->room_flags, ROOM_NOMAGIC ) )
            active = 0;

        if ( vch->position < POS_FIGHTING )
            active = 0;

        /* koles mial pecha i jest wylaczony z walki */
        if ( vch->fighting == NULL &&
             ( IS_AFFECTED( vch, AFF_BLIND ) ||
               IS_AFFECTED( vch, AFF_SLEEP ) ||
               IS_AFFECTED( vch, AFF_SILENCE ) ) )
            active = -1;
    }

    desc = create_desc( vch, caster, tank, hp_percent, active );
    return desc;
}

/* tworzy liste grupkowiczow */
struct char_desc * check_group( CHAR_DATA *ch )
{
    CHAR_DATA *vch;
    struct char_desc *group_list = NULL;
    bool fight = FALSE;
    int count;
  	save_debug_info("si_main.c => check_group", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

    for ( count = 0, vch = ch->in_room->people; vch && count < MAX_SEARCH; vch = vch->next_in_room, count++ )
    {
        if ( !is_same_group( vch, ch ) &&
             !( vch->master == ch && IS_AFFECTED( vch, AFF_CHARM ) ) &&
             !( ch->master == vch && IS_AFFECTED( ch, AFF_CHARM ) ) )
            continue;

        if ( !fight && vch->fighting )
            fight = TRUE;

        add_desc_list( &group_list, check_target( vch ) );
    }
    /* jesli nikt z grupki nie walczy to NULL */
    if ( !fight )
    {
        dispose_list ( &group_list );
        return NULL;
    }

    /* w przeciwnym razie...*/
    return group_list;
}

/* tworzy liste grupkowiczow, ogolnie bez sprawdzania walki */
struct char_desc * check_group_not_fight( CHAR_DATA *ch )
{
    CHAR_DATA *vch;
    struct char_desc *group_list = NULL;
    int count;
  	save_debug_info("si_main.c => check_group_not_fight", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

    if ( !ch || !ch->in_room )
        return NULL;

    for ( count = 0, vch = ch->in_room->people; vch && count < MAX_SEARCH; vch = vch->next_in_room, count++ )
    {
        if ( !is_same_group( vch, ch ) &&
             !( vch->master == ch && IS_AFFECTED( vch, AFF_CHARM ) ) &&
             !( ch->master == vch && IS_AFFECTED( ch, AFF_CHARM ) ) )
            continue;

        add_desc_list( &group_list, check_target( vch ) );
    }

    /* w przeciwnym razie...*/
    return group_list;
}

/* moze to byc ktos walczacy z grupa ch, lub czlonek grupy ktory nie walczy */
struct char_desc * check_enemy( CHAR_DATA *ch, struct char_desc *group_list )
{
    CHAR_DATA *vch;
    struct char_desc *enemy_list = NULL, *tmp;
    int count;
  	save_debug_info("si_main.c => check_enemy", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

    /* nowe */
    for ( tmp = group_list; tmp; tmp = tmp->next )
        if ( tmp->ch->fighting && can_see( ch, tmp->ch->fighting ) )
            add_desc_list( &enemy_list, check_target( tmp->ch->fighting ) );


    /* nikt nie walczy z nasza grupka -> koniec */
    if ( !enemy_list )
        return NULL;

    /* a teraz wszyscy ktorzy nie walcza ale sa w grupce z tymi powyzej */
    for ( count = 0, vch = ch->in_room->people; vch && count < MAX_SEARCH ; vch = vch->next_in_room, count++ )
    {
        if ( vch->fighting )
            continue;

        if ( !can_see( ch, vch ) || is_safe( ch, vch ) )
            continue;

        for ( tmp = enemy_list; tmp; tmp = tmp->next )
            if ( is_same_group( tmp->ch, vch ) )
            {
                add_desc_list( &enemy_list, check_target( vch ) );
                break;
            }


    }
    return enemy_list;
}

/* optimized hyhy, przy 200 mobach w jednym roomie
    obciazenie na celeronie 500
    bez walki: 0%, walka < 5%
   - sprawdzani sa wszyscy najpierw szukajac grupki
   - pozniej przeciwnicy grupki
   - a na koncu wybierana jest akcja
*/

void si_update( CHAR_DATA *ch )
{
    struct char_desc * group_list, *enemy_list, *tmp, *ch_desc = NULL;
    struct char_desc *action_table[ MAX_ACTION ];
    int action, fun, action_table_prior[ MAX_ACTION ];
    int best_action = -1, best_action_prior = 0;
    bool action_ok;
#ifdef INFO
    char buf[ 1024 ];
#endif

    DEBUG_INFO( "si_update:start_check" );

    if ( ( !IS_NPC( ch ) && !IS_AFFECTED( ch, AFF_CHARM ) )
         || ch->wait > 0
         || ch->hit <= 0
         || ch->level <= 0
         || !can_move( ch )
         || !ch->in_room
         || ch->no_standard_si
         || ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) ) )
        return ;

    if ( get_max_hp(ch) == 0 )
        return ;

    DEBUG_INFO( "si_update:int_check" );
    switch ( test_int( ch ) )
    {
            /* a wysoko 10% */
        case INT_HIGH:
            if ( number_percent() < 10 )
                return ;
            /* sredniacy 25% */
        case INT_AVERAGE:
            if ( number_percent() < 25 )
                return ;
            /* malo inteligentni maja 35% na strate kolejki */
        case INT_POOR:
            if ( number_percent() < 35 )
                return ;
            /* a idioci na 65% straca kolejke */
        case INT_IDIOT:
            if ( number_percent() < 65 )
                return ;
        default: break;
    }

    DEBUG_INFO( "si_update:checking_group" );
    /* tworzymy liste przyjaciol */
    if ( ( group_list = check_group( ch ) ) == NULL )
        return ;

    DEBUG_INFO( "si_update:checking_enemy" );
    enemy_list = check_enemy( ch, group_list );

    if ( !enemy_list )
        return ;

    DEBUG_INFO( "si_update:searchin_for_desc" );
    /* szukamy swojego opisu na liscie */
    for ( tmp = group_list; tmp; tmp = tmp->next )
        if ( tmp->ch == ch )
        {
            ch_desc = tmp;
            break;
        }

    DEBUG_INFO( "si_update:exitin" );
    if ( !ch_desc )
    {
        dispose_list( &enemy_list );
        dispose_list( &group_list );
        return ;
    }

#ifdef INFO
    do_emote( ch, "CHECKING SELF CHEKCS" );
#endif


    DEBUG_INFO( "si_update:optimizing_self_checks" );
    /* wykonanie self_checkow */
    optimize_self_checks( ch_desc );

    DEBUG_INFO( "si_update:checking_actions" );
    /* analiza mozliwych akcji + filtrowanie celow */
    for ( action = 0; action < MAX_ACTION; action++ )
    {
        action_table[ action ] = NULL;

        if ( actions[ action ].fun == NULL )
            continue;

        /* sprawdzanie 'ch' */
        action_ok = TRUE;

        if ( test_int( ch ) < actions[ action ].int_check )
            continue;

        /* zeby dla kazdej z akcji nie liczyc selfcheckow od nowa */
        for ( fun = 0; actions[ action ].ch_check[ fun ] >= 0 && fun < MAX_PRIOR_CHECK; fun++ )
            if ( !( actions[ action ].ch_check_cond[ fun ] == self_checks_table[ actions[ action ].ch_check[ fun ] ].value ) )
            {
                action_ok = FALSE;
                break;
            }

        if ( !action_ok )
            continue;

        if ( actions[ action ].group )
        {

            for ( tmp = group_list; tmp; tmp = tmp->next )
            {
                /* sprawdzanie czy spelnia kryteria akcji */
                action_ok = TRUE;

                for ( fun = 0; fun < MAX_PRIOR_CHECK && actions[ action ].check[ fun ]; fun++ )
                    if ( !( actions[ action ].check_cond[ fun ] == ( *actions[ action ].check[ fun ] ) ( ch_desc, tmp ) ) )
                    {
                        action_ok = FALSE;
                        break;
                    }

                if ( !action_ok )
                    continue;

                add_desc_list( &action_table[ action ], copy_desc( tmp ) );
            }
        }
        else
        {
            for ( tmp = enemy_list; tmp; tmp = tmp->next )
            {
                action_ok = TRUE;

                for ( fun = 0; fun < MAX_PRIOR_CHECK && actions[ action ].check[ fun ]; fun++ )
                    if ( !( actions[ action ].check_cond[ fun ] == ( *actions[ action ].check[ fun ] ) ( ch_desc, tmp ) ) )
                    {
                        action_ok = FALSE;
                        break;
                    }

                if ( !action_ok )
                    continue;

                add_desc_list( &action_table[ action ], copy_desc( tmp ) );
            }
        }
    }

    DEBUG_INFO( "si_update:checking_actions_priorytets" );
    /* wybor akcji - najwyzszy priorytet*/
    for ( action = 0; action < MAX_ACTION; action++ )
    {
        if ( action_table[ action ] )
        {
            action_table_prior[ action ] = number_range( actions[ action ].min, actions[ action ].max );
            action_table_prior[ action ] += number_range( 0, actions[ action ].variations );

            if ( action_table_prior[ action ] > best_action_prior )
            {
                best_action = action;
                best_action_prior = action_table_prior[ action ];
            }
        }
    }

    DEBUG_INFO( "si_update:checking_best_action" );
    if ( best_action < 0 )
    {
        DEBUG_INFO( "si_update:cleaning" );
        /* czyszczenie pamieci*/
        for ( action = 0; action < MAX_ACTION; action++ )
            if ( action_table[ action ] )
                dispose_list( &action_table[ action ] );

        dispose_list( &group_list );
        dispose_list( &enemy_list );
        return ;
    }

#ifdef INFO
    for ( action = 0; action < MAX_ACTION; action++ )
    {
        if ( !action_table[ action ] )
            continue;

        sprintf( buf, "%sACTION: %-15.15s [%d]{x", action == best_action ? "{R" : "{G", actions[ action ].name, action_table_prior[ action ] );
        do_emote( ch, buf );
    }
#endif


    DEBUG_INFO( "si_update:get_action_target" );
    /* wybor celu dla akcji */
    if ( !( *actions[ best_action ].fun ) ( ch, action_table[ best_action ] ) )
    {

#ifdef INFO
        sprintf( buf, "{RACTION: %-15.15s RETURNED FALSE{x", actions[ best_action ].name );
        do_emote( ch, buf );
#endif

        dispose_list( &action_table[ best_action ] );
        action_table[ best_action ] = NULL;
        best_action = -1;
        best_action_prior = 0;

        for ( action = 0; action < MAX_ACTION; action++ )
        {
            if ( action_table[ action ] )
            {
                action_table_prior[ action ] = number_range( actions[ action ].min, actions[ action ].max );
                action_table_prior[ action ] += number_range( 0, actions[ action ].variations );

                if ( action_table_prior[ action ] > best_action_prior )
                {
                    best_action = action;
                    best_action_prior = action_table_prior[ action ];
                }
            }
        }

        if ( best_action < 0 )
        {
            /* czyszczenie pamieci*/

#ifdef INFO
            sprintf( buf, "{RNO OTHER ACTIONS, EXITING{x" );
            do_emote( ch, buf );
#endif
            DEBUG_INFO( "si_update:cleaning" );
            for ( action = 0; action < MAX_ACTION; action++ )
                if ( action_table[ action ] )
                    dispose_list( &action_table[ action ] );

            dispose_list( &group_list );
            dispose_list( &enemy_list );
            return ;
        }



#ifdef INFO
        sprintf( buf, "{RUSING ANOTHER ACTION: %-15.15s{x", actions[ best_action ].name );
        do_emote( ch, buf );
#endif

        DEBUG_INFO( "si_update:running_secondary_action" );
        ( *actions[ best_action ].fun ) ( ch, action_table[ best_action ] );

    }

    DEBUG_INFO( "si_update:cleaning" );
    /* czyszczenie pamieci*/
    for ( action = 0; action < MAX_ACTION; action++ )
        if ( action_table[ action ] )
            dispose_list( &action_table[ action ] );

    dispose_list( &group_list );
    dispose_list( &enemy_list );
    return ;
}


void si_update_not_fight( CHAR_DATA *ch )
{
    struct char_desc * group_list, *tmp, *ch_desc = NULL;
    struct char_desc *action_table[ MAX_ACTION_NOT_FIGHT ];
    int action, fun, action_table_prior[ MAX_ACTION_NOT_FIGHT ];
    int best_action = -1, best_action_prior = 0;
    bool action_ok;
#ifdef INFO
    char buf[ 1024 ];
#endif

    DEBUG_INFO( "si_update_not_fight:start_check" );

    if ( ( !IS_NPC( ch ) && !IS_AFFECTED( ch, AFF_CHARM ) )
        || ch->wait > 0
        || ch->hit <= 0
        || ch->level <= 0
        || IS_AFFECTED( ch, AFF_PARALYZE )
		|| IS_AFFECTED( ch, AFF_DAZE )
		|| IS_AFFECTED( ch, AFF_ZAKUTY )
		|| IS_AFFECTED( ch, AFF_FORCE_FIELD )
		|| IS_AFFECTED( ch, AFF_ASTRAL_JOURNEY )
        || !ch->in_room
        || ch->no_standard_si
        || ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) ) )
        return ;

	if ( ch->no_standard_si )
		bug( "no_standard_si", 1 );

    DEBUG_INFO( "si_update_not_fight:int_check" );
    switch ( test_int( ch ) )
    {
        case INT_AVERAGE:
            if ( number_percent() < 5 )
                return ;
        case INT_POOR:
            if ( number_percent() < 10 )
                return ;
        case INT_IDIOT:
            if ( number_percent() < 15 )
                return ;
        default: break;
    }

    DEBUG_INFO( "si_update_not_fight:checking_group" );

    /* tworzymy liste przyjaciol */
    if ( ( group_list = check_group_not_fight( ch ) ) == NULL )
        return ;

    DEBUG_INFO( "si_update_not_fight:searchin_for_desc" );
    /* szukamy swojego opisu na liscie */
    for ( tmp = group_list; tmp; tmp = tmp->next )
        if ( tmp->ch == ch )
        {
            ch_desc = tmp;
            break;
        }

    DEBUG_INFO( "si_update_not_fight:exitin" );
    if ( !ch_desc )
    {
        dispose_list( &group_list );
        return ;
    }

    DEBUG_INFO( "si_update_not_fight:optimizing_self_checks" );
    /* wykonanie self_checkow */
    optimize_self_checks( ch_desc );

    DEBUG_INFO( "si_update_not_fight:checking_actions" );
    /* analiza mozliwych akcji + filtrowanie celow */
    for ( action = 0; action < MAX_ACTION_NOT_FIGHT; action++ )
    {
        action_table[ action ] = NULL;

        if ( actions_notfight[ action ].fun == NULL )
            continue;

        /* sprawdzanie 'ch' */
        action_ok = TRUE;

        if ( test_int( ch ) < actions_notfight[ action ].int_check )
            continue;

        /* zeby dla kazdej z akcji nie liczyc selfcheckow od nowa */
        for ( fun = 0; actions_notfight[ action ].ch_check[ fun ] >= 0 && fun < MAX_PRIOR_CHECK; fun++ )
            if ( !( actions_notfight[ action ].ch_check_cond[ fun ] == self_checks_table[ actions_notfight[ action ].ch_check[ fun ] ].value ) )
            {
                action_ok = FALSE;
                break;
            }

        if ( !action_ok )
            continue;

        for ( tmp = group_list; tmp; tmp = tmp->next )
        {
            /* sprawdzanie czy spelnia kryteria akcji */
            action_ok = TRUE;

            for ( fun = 0; fun < MAX_PRIOR_CHECK && actions_notfight[ action ].check[ fun ]; fun++ )
                if ( !( actions_notfight[ action ].check_cond[ fun ] == ( *actions_notfight[ action ].check[ fun ] ) ( ch_desc, tmp ) ) )
                {
                    action_ok = FALSE;
                    break;
                }

            if ( !action_ok )
                continue;

            add_desc_list( &action_table[ action ], copy_desc( tmp ) );
        }
    }

    DEBUG_INFO( "si_update_not_fight:checking_actions_priorytets" );
    /* wybor akcji - najwyzszy priorytet*/
    for ( action = 0; action < MAX_ACTION_NOT_FIGHT; action++ )
    {
        if ( action_table[ action ] )
        {
            action_table_prior[ action ] = number_range( actions_notfight[ action ].min, actions_notfight[ action ].max );
            action_table_prior[ action ] += number_range( 0, actions_notfight[ action ].variations );

            if ( action_table_prior[ action ] > best_action_prior )
            {
                best_action = action;
                best_action_prior = action_table_prior[ action ];
            }
        }
    }

    DEBUG_INFO( "si_update_not_fight:checking_best_action" );

    while ( TRUE )
    {

        if ( best_action < 0 )
            break;

#ifdef INFO
        for ( action = 0; action < MAX_ACTION_NOT_FIGHT; action++ )
        {
            if ( !action_table[ action ] )
                continue;

            sprintf( buf, "%sACTION: %-15.15s [%d]{x", action == best_action ? "{R" : "{G", actions_notfight[ action ].name, action_table_prior[ action ] );
            do_emote( ch, buf );
        }
#endif
        DEBUG_INFO( "si_update:running_action" );

        /* uruchamianie akcji */
        if ( !( *actions_notfight[ best_action ].fun ) ( ch, action_table[ best_action ] ) )
        {
            dispose_list( &action_table[ best_action ] );
            action_table[ best_action ] = NULL;
            best_action = -1;
            best_action_prior = 0;
        }
        else
            break;

        for ( action = 0; action < MAX_ACTION_NOT_FIGHT; action++ )
        {
            if ( action_table[ action ] )
            {
                if ( action_table_prior[ action ] > best_action_prior )
                {
                    best_action = action;
                    best_action_prior = action_table_prior[ action ];
                }
            }
        }
    }

    DEBUG_INFO( "si_update:cleaning" );
    /* czyszczenie pamieci*/
    for ( action = 0; action < MAX_ACTION_NOT_FIGHT; action++ )
        if ( action_table[ action ] )
            dispose_list( &action_table[ action ] );

    dispose_list( &group_list );
    return ;
}
