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
 * Zdziech Tomasz        (tzdziech@gmail.com            ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: update.c 8611 2010-02-23 21:24:03Z void
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/update.c $
 *
 */
#if defined (macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "music.h"
#include "recycle.h"
#include "tables.h"
#include "progs.h"
#include "magic.h"
#include "name_check.h"
#include "todelete.h" // delayed ch delete - by Fuyara
#include "money.h"
#include "lang.h"

//#define INFO

/*rprogi*/
void 	rset_supermob		args ( (ROOM_INDEX_DATA *room));
void 	set_supermob		args ( (OBJ_DATA *obj));
bool 	rp_percent_trigger 	args ( (CHAR_DATA *mob, CHAR_DATA *ch,
const void *arg1, const void *arg2, int type));
void 	pwait_cast_update	args ( (CHAR_DATA *ch));
void 	mp_time_trigger		args ( (CHAR_DATA *mob, int time));
void	raw_kill	args( ( CHAR_DATA *victim , CHAR_DATA *ch ) );
void	check_malnutrition	args( ( CHAR_DATA *ch ) );
void	check_dehydration	args( ( CHAR_DATA *ch ) );
void	update_herbs		args( (const char *caller) );
void mp_onload_trigger args( ( CHAR_DATA *mob ) );
void extern_si_update();
/*
 * hint functions.
 */

void	hint_update	args( ( void ) );
/*
 * paladin_auras
 */

void char_update_paladin_auras args ( ( CHAR_DATA *ch ) );

/*
 * db.c
 */

int number_range_m args(( int from, int to ));

/**
 * act_move.c
 */
void turn_into_dust_objects_sensitive_to_light args( ( CHAR_DATA *ch, int dmg ) );

/*
 * Local functions.
 */
int		hit_gain	args ( (CHAR_DATA *ch));
int		move_gain	args ( (CHAR_DATA *ch));
void	mobile_update	args ( (void));
void	char_update	args ( (void));
void	regen_update	args ( (void));
void	obj_update	args ( (void));
void	aggr_update	args ( (void));
void 	event_update	args ( ());
void    use_spells	args ( (CHAR_DATA *mob,CHAR_DATA *vic));
void 	use_skills	args ( (CHAR_DATA *mob,CHAR_DATA *ch));
sh_int 	get_caster	args ( (CHAR_DATA *ch));
bool 	check_rent	args ( (CHAR_DATA *ch, OBJ_DATA *object, bool show));
char *  get_target_by_name (CHAR_DATA *ch, CHAR_DATA *victim);
void	update_poison	args ( (CHAR_DATA *victim));
void	si_update	args ( (CHAR_DATA *ch));
void	si_update_not_fight	args ( (CHAR_DATA *ch));
void    drop_objects    args( (CHAR_DATA *ch, OBJ_DATA *object, bool auto_rent) );
bool	write_to_descriptor	    args( ( DESCRIPTOR_DATA *d, char *txt, int length ) );
bool	write_to_descriptor_2	args( ( int desc, char *txt, int length ) );
void    save_misc_data      args( ( void ) );
void    check_healing_touch args( ( CHAR_DATA *ch ) );
void    check_acid_arrow      args( ( CHAR_DATA *ch ) );
void    check_burn      args( ( CHAR_DATA *ch ) );
void    check_damned_stuff      args( ( CHAR_DATA *ch ) );
void    check_shadow_swarm      args( ( CHAR_DATA *ch ) );
/* used for saving */
bool	check_nature_curse_aggressive	args ( (CHAR_DATA *ch, CHAR_DATA *wch));
bool	undead_resemblance	args ( (CHAR_DATA *ch, CHAR_DATA *victim));

int	save_number = 0;


/* losowanie statsow, zwraca sume */
#define ROLLS 2

/*eventy*/

EVENT2_DATA	*events2;

/* co¶ mi nie dzia³a w eventach, zamieniam na funkcjê - rellik
#define KILL_EVENT2 \
				if(!ev2_next)  {   free_event2(ev2); return; }  \
				if(ev2_last!=NULL) ev2_last->next=ev2_next;  \
				             else events2=ev2_next;  \
				free_event2(ev2);  \
				continue;
				*/

EVENT2_DATA *event2_find_last( EVENT2_DATA *current )
{
	EVENT2_DATA *last = NULL;
	for ( last = events2; last; last = last->next )
	{
		if ( last->next == current ) return last;
	}
	return NULL;
}

void KILL_EVENT2( EVENT2_DATA *ev2, EVENT2_DATA *ev2_next )
{
	EVENT2_DATA *ev2_last;
	ev2_last = event2_find_last( ev2 );

	if ( ev2 == events2 )
	{
		events2 = ev2->next;
		free_event2( ev2 );
		return;
	}

	ev2_last->next = ev2->next;

	free_event2(ev2);
	return;
}

/*
 * Handle events.
 */
void event2_update( void )
{
	EVENT2_DATA * ev2 = NULL, *ev2_next;
	CHAR_DATA *ch = NULL;
	/*      int timer;      */
	if ( !events2 ) return ;

	for ( ev2 = events2;ev2;ev2 = ev2_next )
	{
		ev2_next = ev2->next;
		if ( ev2->delay-- <= 0 )
		{
			switch ( ev2->action )
			{
				case ACTION_PRINT:
					if ( ( !ev2->args[ 0 ] ) || ( !ev2->argv[ 0 ] ) )
					{
						KILL_EVENT2( ev2, ev2_next );
						continue;
					}
					send_to_char( ev2->args[ 0 ], ev2->argv[ 0 ] );
					break;
				case ACTION_FUNCTION:
					if ( !ev2->args[ 0 ] )
					{
						KILL_EVENT2( ev2, ev2_next);
						continue;
					}
					do_function( ev2->argv[ 0 ], ev2->do_fun, ev2->args[ 0 ] );
					break;
				case ACTION_WAIT:
					if ( !ev2->argv[ 0 ] )
					{
						KILL_EVENT2( ev2, ev2_next);
						continue;
					}
					WAIT_STATE( ev2->to, PULSE_PER_SECOND * ev2->argi[ 0 ] );
					break;
				case ACTION_ACT:
					if ( !ev2->args[ 0 ] || !ev2->argv[ 0 ] )
					{
						KILL_EVENT2( ev2, ev2_next);
						continue;
					}
					act( ev2->args[ 0 ], ev2->argv[ 0 ], ev2->argv[ 1 ], ev2->argv[ 2 ], ev2->argi[ 0 ] );
					break;
				case ACTION_OBJ_TO_ROOM:
					if ( !ev2->argv[ 0 ] || !ev2->argv[ 1 ] )
					{
						KILL_EVENT2( ev2, ev2_next);
						continue;
					}
					obj_to_room( ev2->argv[ 0 ], ev2->argv[ 1 ] );
					break;
				case ACTION_OBJ_TO_CHAR:
					if ( !ev2->argv[ 0 ] || !ev2->argv[ 1 ] )
					{
						KILL_EVENT2( ev2, ev2_next);
						continue;
					}
					ch = ev2->argv[ 1 ];
					if ( ch->carry_number + get_obj_number( ev2->argv[ 0 ] ) > can_carry_n( ev2->argv[ 1 ] ) )
					{
						send_to_char( "Nie mo¿esz nosiæ wiecej rzeczy.\n\r", ev2->argv[ 1 ] );
						obj_to_room( ev2->argv[ 0 ], ch->in_room );
						break;
					}
					if ( ch->carry_weight + get_obj_weight( ev2->argv[ 0 ] ) > can_carry_w( ev2->argv[ 1 ] ) )
					{
						send_to_char( "Nie uniesiesz juz wiecej.\n\r", ev2->argv[ 1 ] );
						obj_to_room( ev2->argv[ 0 ], ch->in_room );
						break;

					}
					send_to_char( "Trofeum l±duje w twoim inwentarzu.\n\r", ev2->argv[ 1 ] );
					obj_to_char( ev2->argv[ 0 ], ev2->argv[ 1 ] );
					break;
				case ACTION_CHAR_TO_ROOM:
					if ( !ev2->argv[ 0 ] || !ev2->argv[ 1 ] )
					{
						KILL_EVENT2( ev2, ev2_next);
						continue;
					}
					char_to_room( ev2->argv[ 0 ], ev2->argv[ 1 ] );
					break;

			}
			KILL_EVENT2( ev2, ev2_next);
		}
	}
	return ;
}

/*
 * Advancement stuff.
 */

void new_room_update ( ROOM_INDEX_DATA *room, int type, bool tick )
{
    ROOM_UPDATE_TYPE * el, *list;

    if ( tick )
        list = room_update_list.tick_update_list;
    else
        list = room_update_list.pulse_update_list;

    /* szukamy czy juz jest w liscie cos podobnego*/
    for ( el = list; el ; el = el->next )
        if ( room == el->room && el->type == type )
        {
            el->active = TRUE;
            return ;
        }


    list = NULL;
    CREATE( el, ROOM_UPDATE_TYPE, 1 );
    el->room = room;
    el->active = TRUE;
    el->type = type;

    if ( tick )
    {
        el->next = room_update_list.tick_update_list;
        room_update_list.tick_update_list = el;
    }
    else
    {
        el->next = room_update_list.pulse_update_list;
        room_update_list.pulse_update_list = el;
    }
    return ;
}

void disable_room_update ( ROOM_INDEX_DATA * room, BITVECT_DATA * type )
{
	ROOM_UPDATE_TYPE * el, *list;
	int update_type;

	if ( type == &TRIG_RANDOM )
	{
		list = room_update_list.pulse_update_list;
		update_type = TYPE_PULSE_RANDOM;
	}

	else if ( type == &TRIG_TIME )
	{
		list = room_update_list.tick_update_list;
		update_type = TYPE_TIME;
	}

	else if ( type == &TRIG_TICK_RANDOM )
	{
		list = room_update_list.tick_update_list;
		update_type = TYPE_TICK_RANDOM;
	}
	else
		return ;

	/* szukamy czy juz jest w liscie cos podobnego*/
	for ( el = list; el ; el = el->next )
		if ( room == el->room && el->type == update_type )
		{
			el->active = FALSE;
			return ;
		}
	return ;
}

void enable_room_update ( ROOM_INDEX_DATA *room, BITVECT_DATA * type )
{
	ROOM_UPDATE_TYPE * el, *list;
	int update_type;
	bool tick = FALSE;

	if ( type == &TRIG_RANDOM )
	{
		list = room_update_list.pulse_update_list;
		update_type = TYPE_PULSE_RANDOM;
	}
	else if ( type == &TRIG_TIME )
	{
		list = room_update_list.tick_update_list;
		update_type = TYPE_TIME;
		tick = TRUE;
	}
	else if ( type == &TRIG_TICK_RANDOM )
	{
		list = room_update_list.tick_update_list;
		update_type = TYPE_TICK_RANDOM;
		tick = TRUE;
	}
	else
		return ;

	/* szukamy czy juz jest w liscie cos podobnego*/
	for ( el = list; el ; el = el->next )
		if ( room == el->room && el->type == update_type )
		{
			el->active = TRUE;
			return ;
		}

	list = NULL;
	CREATE( el, ROOM_UPDATE_TYPE, 1 );
	el->room = room;
	el->active = TRUE;
	el->type = update_type;

	if ( tick )
	{
		el->next = room_update_list.tick_update_list;
		room_update_list.tick_update_list = el;
	}
	else
	{
		el->next = room_update_list.pulse_update_list;
		room_update_list.pulse_update_list = el;
	}
	return ;
}

void stats_to_spend_check( CHAR_DATA *ch)// jeden punkty na 3 poziomy w sumie 10
{
	int punkty;

	punkty = ch->level / 3;

	if ( punkty > (ch->statpointsleft + ch->statpointsspent) )
	{
		ch->statpointsleft += 1;
		send_to_char( "{MZdoby³e¶ 1 punkt statystyk który mo¿esz wydaæ komend±: addstat{M'.{x\n\r", ch );
		stats_to_spend_check(ch);
	}
	return;

}

void advance_level ( CHAR_DATA *ch, bool hide )
{
    PFLAG_DATA *pflag, *pflag_next;
    int add_hp, hp_diff, i;//,licz1;
    char buf[MAX_STRING_LENGTH];
    int stat_roll[4][MAX_STATS];
    int bonus_code=0,bonus_race=0,bonus_class=0;

    /**
     * moby i ci ci maja wyssane doswiadczenie, nie zmieniaja poziomu
     */
    if ( IS_NPC( ch ) || is_affected (ch, gsn_energy_drain))
    {
        return;
    }

    ch->level += 1;
    stats_to_spend_check(ch);

    //kasowanie flag _levxxx
    for ( pflag = ch->pflag_list; pflag; pflag = pflag_next )
    {
        pflag_next = pflag->next;
        if ( str_prefix( "_lev", pflag->id ) )
        {
            continue;
        }
        removecharflag( ch, pflag->id );
    }

    sprintf( buf, "%s zdobywa %d", ch->name, ch->level );
    log_string( buf );

    sprintf( buf, "$N zdobywa poziom %d!", ch->level );
    wiznet( buf, ch, NULL, WIZ_LEVELS, 0, 0 );

    if ( !hide )
    {
        send_to_char( "{RZdobywasz poziom!{x\n\r", ch );
    }

    /**
     * jezyk magiczny dla magow
     */
    if ( ch->class == CLASS_MAG )
    {
        int language = get_langnum( "magical" );
        int lang_magical_level = knows_language( ch, language, NULL );
        if ( lang_magical_level > 0 )
        {
            if (  lang_magical_level < 90 )
            {
                ch->pcdata->language[ language ] += number_range( 0, 5 );
            }
        }
        else
        {
            ch->pcdata->language[ language ] = number_range( 0, 20 ) + get_curr_stat( ch, STAT_INT ) / 5;
        }
    }

    /*tu wstawiamy rollowanie*/
    if ( ch->level == 2 && ch->pcdata->points == 0 )
    {
        ch->pcdata->rolls_count = 1;
    }
    /********* nowe staty **********/
#ifdef INFO
    if ( IS_SET( ch->pcdata->wiz_conf, W6 ) )
    {
        if (ch->level == 2 && ch->pcdata->new_rolls_count == 0)
        {
            print_char( ch, "\n\r reszta=%d rolowan=%d poziom/2-1=%d\n\r",(ch->level)%2,ch->pcdata->new_rolls_count,(ch->level/2) - 1);
        }
    }
#endif
    //nowe staty
    if(
            ( ( ( ( ch->level ) % ROLL_PER_LEVEL ) == 0 ) && ( ch->pcdata->new_rolls_count == ( ( ch->level / ROLL_PER_LEVEL ) - 1 ) ) )
            //bonus dorolka na 31
            || ( ch->level == LEVEL_HERO && ch->pcdata->new_rolls_count == ROLL_COUNT )
      )
    {
        /**
         * inkrementacja licznika rolowan
         */
        ++ch->pcdata->new_rolls_count;
        sprintf
            (
             buf,
             "[ROLL_STAT] Postaæ: %s bonus_race: %s Klasa: %s Poziom: %d Dorolka: %d",
             ch-> name,
             race_table[ GET_RACE( ch ) ].name,
             class_table[ch->class].name,
             ch->level,
             ch->pcdata->new_rolls_count
            );
        log_string( buf );

        /**
         * losowanie dorolek
         */
        for(i=0;i < MAX_STATS;i++)
        {
            /**
             * losowanie skladowych
             */
            bonus_code  += stat_roll[0][i] = number_range(0,2);
            bonus_class += stat_roll[1][i] = number_range_m(class_table[ch->class].class_stat_mod[i][0],class_table[ch->class].class_stat_mod[i][1]);
            bonus_race  += stat_roll[2][i] = number_range_m(pc_race_table[GET_RACE( ch )].race_stat_mod[i][0],pc_race_table[GET_RACE( ch )].race_stat_mod[i][1]);

            stat_roll[3][i] = stat_roll[0][i] + stat_roll[1][i] + stat_roll[2][i];
            ch->perm_stat[i] += stat_roll[3][i];

            sprintf
                (
                 buf,
                 "[ROLL_STAT] [%d] SUMA %d, kod: %d, klasa: %d <%d,%d>, rasa: %d <%d,%d>",
                 i,
                 stat_roll[3][i],
                 stat_roll[0][i],
                 stat_roll[1][i], class_table[ch->class].class_stat_mod[i][0], class_table[ch->class].class_stat_mod[i][1],
                 stat_roll[2][i], pc_race_table[GET_RACE( ch )].race_stat_mod[i][0], pc_race_table[GET_RACE( ch )].race_stat_mod[i][1]
                );
            log_string( buf );
        }

        /* log */
        sprintf
            (
             buf,
             "[ROLL_STAT] SUMA: %d, kod: %d, klasa: %d, rasa: %d",
             bonus_code + bonus_class + bonus_race,
             bonus_code,
             bonus_class,
             bonus_race
            );
        log_string( buf );

#ifdef INFO
        if ( IS_SET( ch->pcdata->wiz_conf, W6 ) )
        {
            print_char( ch, "\n\rStaty na poziomie %d, rasa: %s, klasa: %s \n\r",ch->level, race_table[ GET_RACE( ch ) ].name, class_table[ch->class].name );
            for( i = 0; i < MAX_STATS; i++ )
            {
                print_char( ch, "\n\r %d \n\r", ch->perm_stat[i] );
            }
        }
#endif
    }
    /********koniec NOWE STATY*********/

    DEBUG_INFO("advance_level:add_name:0");
    if ( ch->level == 2 && EXT_IS_SET( ch->act, PLR_UNREGISTERED ) )
    {
        DEBUG_INFO("advance_level:add_name:0");
        add_name( ch->name, NAME_NEW );
    }

    //autoregister powyzej LEVEL_HERO
    if ( ch->level > LEVEL_HERO && EXT_IS_SET ( ch->act, PLR_UNREGISTERED ) )
    {
        EXT_REMOVE_BIT( ch->act, PLR_UNREGISTERED );
        add_name( ch->name, NAME_ALLOWED );
    }

    ch->pcdata->last_level = ( ch->played + (int) ( current_time - ch->logon ) ) / 3600;

    if ( ch->pcdata->perm_hit_per_level[ ch->level - 1 ] == 0 )
    {
        add_hp = UMAX( 1, number_range( class_table[ch->class].hp_min,class_table[ch->class].hp_max ) );
        ch->max_hit += add_hp;
        ch->pcdata->perm_hit += add_hp;
        ch->pcdata->perm_hit_per_level[ ch->level - 1 ] = ch->pcdata->perm_hit;
    }
    else
    {
        hp_diff = ch->pcdata->perm_hit_per_level[ ch->level - 1 ] - ch->pcdata->perm_hit;
        ch->max_hit += hp_diff;
        ch->pcdata->perm_hit = ch->pcdata->perm_hit_per_level[ ch->level - 1 ];
    }

    if ( ch->pcdata->spells_failed )
        clear_spells_failed( ch );

    if ( IS_SET( ch->comm, COMM_NEWBIE ) && ch->level > LEVEL_NEWBIE )
    {
        REMOVE_BIT( ch->comm, COMM_NEWBIE );
    }
    return;
}

/* zmniejsza poziom - np na skutek zejscia */
void delevel (CHAR_DATA *ch, bool hide)
{
    char buf[MAX_STRING_LENGTH];
    PFLAG_DATA * pflag, * pflag_next;
    int hp_diff;

    if ( IS_NPC( ch ) )
    {
        return;
    }

    /**
     * LEVEL_NEWBIE - nie delevelujemy !
     */
    if ( ch->level < LEVEL_NEWBIE )
    {
        return;
    }

    ch->level--;

    /**
     * jezyk magiczny dla magow
     */
    if ( ch->class == CLASS_MAG )
    {
        int language = get_langnum( "magical" );
        int lang_magical_level = knows_language( ch, language, NULL );
        if ( lang_magical_level > 0 )
        {
                ch->pcdata->language[ language ] = UMAX( 10, lang_magical_level - 4 );
        }
    }

    if ( ch->pcdata->perm_hit_per_level[ ch->level - 1 ] > 0 )
    {
        hp_diff = ch->pcdata->perm_hit - ch->pcdata->perm_hit_per_level[ ch->level - 1 ];
        ch->pcdata->perm_hit = ch->pcdata->perm_hit_per_level[ ch->level - 1 ];
        ch->max_hit -= hp_diff;
        ch->max_hit = UMAX( 1, ch->max_hit );
        ch->hit = UMIN ( get_max_hp( ch ), ch->hit );
    }
    else
    {
        ch->max_hit          -= class_table[ ch->class ].hp_max;
        ch->max_hit           = UMAX( 1, ch->max_hit );
        ch->hit               = UMIN ( get_max_hp( ch ), ch->hit );
        ch->pcdata->perm_hit -= class_table[ ch->class ].hp_max;
    }

    if ( ch->level < 2 )
    {
        sprintf( buf, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
        unlink( buf );
    }

    //kasowanie flag _levxxx
    for ( pflag = ch->pflag_list; pflag; pflag = pflag_next )
    {
        pflag_next = pflag->next;
        if ( str_prefix( "_lev", pflag->id ) )
        {
            continue;
        }
        removecharflag( ch, pflag->id );
    }

    if (!hide) {
        send_to_char ("Straci³<&e/a/o>¶ poziom do¶wiadczenia!\n\r", ch);
        return;
    }
}

int gain_exp (CHAR_DATA *ch, int gain, bool multiply)
{
    /**
     * nie liczymy punktow doswiadczenia:
     *
     * - mobom
     * - immortalom
     * - newbiakom, jezeli exp jest ujemny
     */
    if (IS_NPC (ch) || ch->level >= LEVEL_HERO || ( ch->level < LEVEL_NEWBIE && gain < 0 ) )
    {
        return 0;
    }

    /**
     * nie liczymy tez tym co maja wyssane doswiadczenie
     */
    if (is_affected (ch, gsn_energy_drain))
    {
        return 0;
    }

    if ( gain > 0 && multiply )
    {
        gain *= EXP_MULTIPLIER;
    }
    ch->exp += gain;

    while (ch->level < LEVEL_HERO && ch->exp >= exp_per_level (ch,ch->level))
    {
        advance_level (ch,FALSE);
        save_char_obj (ch, FALSE, FALSE);
    }
    return gain;
}

//no to przerabiamy na na update hp 10 razy w ciagu ticka
int hit_gain ( CHAR_DATA *ch )
{
    int gain, condition;
    if ( ch->in_room == NULL || ch->position == POS_FIGHTING || ch->fighting != NULL )
    {
        return 0;
    }
    condition = get_curr_stat_deprecated( ch, STAT_CON );
    if ( IS_NPC ( ch ) )
    {
        switch ( ch->position )
        {
            case POS_SLEEPING: gain = number_range ( 3, 4 ); break;
            case POS_RESTING:  gain = number_range ( 2, 3 ); break;
            case POS_FIGHTING: gain = number_range ( 0, 1 ); break;
            default:           gain = number_range ( 0, 1 ); break;
        }
        gain += URANGE ( 0, ch->level / 12, 2 );

        if ( IS_AFFECTED ( ch, AFF_RECUPERATE ) )
        {
            if( ch->level > 25 )
                gain += number_range(1,2);
            else if ( ch->level > 18 )
                gain += 1;
            else
                gain += number_range(0,1);
        }

        if ( IS_AFFECTED ( ch, AFF_HEALING_SALVE ) )
            ++gain;

        if ( IS_AFFECTED ( ch, AFF_SPIRIT_OF_LIFE ) )
            gain += number_range (2, 5);

        if ( IS_AFFECTED ( ch, AFF_BREATH_OF_LIFE ) )
            gain += number_range (3, 12);

        //Bonus dla bossow - bazowa regeneracja x4
        if ( EXT_IS_SET( ch->act, ACT_BOSS ) )
        {
            gain = gain * 3;
        }
        //Bonus z Troll Power - bazowa regeneracja x2 (dla bossów ju¿ jest)
        else if ( IS_AFFECTED(ch, AFF_TROLL) )
        {
            gain = gain * 2;
        }

        gain *= condition;
        gain /= URANGE(10, condition, 18);
    }
    else
    {
        switch ( ch->position )
        {
            case POS_SLEEPING: gain = number_range ( 2, 3 ); break;
            case POS_RESTING:  gain = number_range ( 1, 3 ); break;
            case POS_SITTING:  gain = number_range ( 0, 2 ); break;
            case POS_FIGHTING: gain = number_range ( 0, 1 ); break;
            default:           gain = number_range ( 0, 1 ); break;
        }
#ifdef INFO
        print_char( ch, "baza: %d.\n\r", gain );
#endif

        if ( IS_AFFECTED ( ch, AFF_RECUPERATE ) )
        {
            if( dice(1,2) == 1 )
            {
                ++gain;
            }
            if( dice(1,2) == 1 && get_skill(ch,gsn_recuperate) > 50 )
            {
                ++gain;
            }
            if( dice(1,2) == 1 && get_skill(ch,gsn_recuperate) > 90 )
            {
                ++gain;
            }
        }

        if ( IS_AFFECTED ( ch, AFF_SPIRIT_OF_LIFE ) )
            gain += number_range (2, 5);

        if ( IS_AFFECTED ( ch, AFF_BREATH_OF_LIFE ) )
            gain += number_range (3, 12);

#ifdef INFO
        print_char( ch, "+recup: %d.\n\r", gain );
#endif
        if( dice(1,5) > 2 )
        {
            if( dice(1,2) == 1 )
            {
                if( ch->class == CLASS_PALADIN || ch->class == CLASS_WARRIOR || ch->class == CLASS_BARBARIAN || ch->class == CLASS_BLACK_KNIGHT )
                    gain += con_app[ condition ].hith;
                else
                    gain += con_app[ condition ].hitl;
            }
            else
            {
                if( ch->class == CLASS_PALADIN || ch->class == CLASS_WARRIOR || ch->class == CLASS_BARBARIAN || ch->class == CLASS_BLACK_KNIGHT  )
                    gain += con_app[ condition ].hith/2;
                else
                    gain += con_app[ condition ].hitl/2;
            }
        }
#ifdef INFO
        print_char( ch, "+kondycha: %d.\n\r", gain );
#endif

        if( ch->level > 24 && dice(1,2) == 1 )
            ++gain;

#ifdef INFO
        print_char( ch, "+lev: %d.\n\r", gain );
#endif

        if ( ch->condition[ COND_HUNGER ] == 0 )
            gain /= 2;

        if ( ch->condition[ COND_THIRST ] == 0 )
            gain /= 2;

        if ( ch->condition[ COND_SLEEPY ] == 0 )
            gain /= 2;

#ifdef INFO
        print_char( ch, "-glodny/spragniony/senny: %d.\n\r", gain );
#endif
    }

    if ( is_affected ( ch, gsn_bandage ) )
    {
        AFFECT_DATA * aff_band = NULL;

        ++gain;

        if ( ( aff_band = affect_find ( ch->affected, gsn_bandage ) ) != NULL &&
                aff_band->level != 0 )
            gain += gain * aff_band->level / 100;

        if ( aff_band->modifier > 0 )
            gain += aff_band->modifier / 10;
        /* nowe rany nie znikaj± magicznie
        if( is_affected( ch, gsn_bleeding_wound ) )
        {
            if ( skill_table[ gsn_bleeding_wound ].msg_off )
            {
                send_to_char( skill_table[ gsn_bleeding_wound ].msg_off, ch );
                send_to_char( "\n\r", ch );
            }
            affect_strip( ch, gsn_bleeding_wound );
        }
        */
    }

#ifdef INFO
    print_char( ch, "+bandaz: %d.\n\r", gain );
#endif

    gain = gain * ch->in_room->heal_rate / 100;

    gain += sector_table[ ch->in_room->sector_type ].heal_bonus * gain / 100;

#ifdef INFO
    print_char( ch, "+room&sector_heal_rate: %d.\n\r", gain );
#endif

    // bonus consecrate i desecrate, tylko dla dobrych i z³ych
    if (EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE))
    {
        if(IS_GOOD(ch))
        {
            ++gain;
        }
        else if(IS_EVIL(ch) )
        {
            --gain;
        }
        // undedzi tutaj ... ZERO! do licha
        if ( is_undead( ch ) )
        {
            gain = 0;
        }
    }
    else if (EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE))
    {
        if(IS_GOOD(ch))
        {
            --gain;
        }
        else if(IS_EVIL(ch) )
        {
            ++gain;
        }
    }
#ifdef INFO
    print_char( ch, "+consectare/desecrate: %d.\n\r", gain );
#endif

    /*
     * aura of improve healing bonus
     */
    if ( is_affected( ch, gsn_aura_of_improved_healing ) )
    {
        gain *= 3;
        gain /= 2;
        if ( gain < 2 )
        {
            gain += 2;
        }
#ifdef INFO
        print_char( ch, "+aura of improve healing: %d.\n\r", gain );
#endif
    }

    if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE )
    {
        gain = gain * ch->on->value[ 3 ] / 100;
        if ( ch->on->value[ 3 ] > 130 )
        {
            bugf ("%s is on obj %d with %d healing rate [room: %d]", ch->name, ch->on->pIndexData->vnum, ch->on->value[ 3 ], ch->in_room->vnum);
        }
    }
#ifdef INFO
    print_char( ch, "+meble: %d.\n\r", gain );
#endif

    if ( IS_AFFECTED ( ch, AFF_POISON ) )
    {
        if (!( is_undead( ch ) || IS_SET( ch->form, FORM_CONSTRUCT ) ))
        {
            gain /= 5;
        }
    }

    if ( IS_AFFECTED ( ch, AFF_PLAGUE ) )
    {
        if (!( is_undead( ch ) || IS_SET( ch->form, FORM_CONSTRUCT ) ))
        {
            gain /= 8;
        }
    }

    if ( IS_AFFECTED ( ch, AFF_HASTE ) || IS_AFFECTED ( ch, AFF_SLOW ) )
        gain /= 2 ;

    if ( is_affected( ch, gsn_energy_strike ) )
        gain = (gain*2)/3;

#ifdef INFO
    print_char( ch, "-poison/plague/haste/slow/energy_strike: %d.\n\r", gain );
#endif

    if ( !IS_NPC ( ch ) )
    {
        if ( IS_SET ( sector_table[ ch->in_room->sector_type ].flag, SECT_NOREST )
                || sector_table[ ch->in_room->sector_type ].max_dam > 0 )
        {
            gain = 0;

#ifdef INFO
            print_char( ch, "czy no_rest?: %d.\n\r", gain );
#endif
        }
    }

    if( ch->position == POS_STUNNED )
    {
        if( number_percent() > get_curr_stat_deprecated(ch,STAT_CON) )
            gain = 0;
        else
            gain = 1;
    }

    if( is_affected( ch, gsn_bleeding_wound ) )
        gain = 0;

    //coby go¶æ nie le¿a³ w nieskoñczono¶æ na stunned w sectorze norest
    if ( dice(1,20) == 1 && gain < 1 )
    {
        gain = 1;
    }

    if ( !IS_NPC(ch) && is_undead(ch) )//dla feata przemiana w undeada
    {
        gain *= 2;
    }

#ifdef INFO
    print_char( ch, "FINAL HP_REGEN: %d.\n\r", gain );
#endif
    //przerabiam na > 50 bo sieje po logu przy kazdym cure
    if ( !IS_NPC ( ch ) && gain > 50 )
    {
        bugf ( "%s got pretty big hp regen [%d] [room: %d]", ch->name, gain, ch->in_room->vnum );
    }

    return UMIN ( gain, get_max_hp(ch) - ch->hit );
}

/* move_gain */
int move_gain ( CHAR_DATA *ch )
{
    AFFECT_DATA * paf;
    int gain, condition = get_curr_stat_deprecated( ch, STAT_CON );

    if ( ch->in_room == NULL )
        return 0;

    if ( IS_NPC ( ch ) && !EXT_IS_SET(ch->act, ACT_MOUNTABLE))
    {
        gain = UMAX(10, ch->level);
    }
    else
    {
        switch ( ch->position )
        {
            case POS_SLEEPING: gain = number_range ( 2, 3 ); break;
            case POS_RESTING:  gain = number_range ( 1, 3 ); break;
            case POS_FIGHTING: gain = number_range ( 0, 1 ); break;
            default:           gain = number_range ( 0, 1 ); break;
        }
#ifdef INFO
        print_char( ch, "baza: %d.\n\r", gain );
#endif

        if ( IS_AFFECTED ( ch, AFF_RECUPERATE ) )
        {
            if( dice(1,2) == 1 )
                ++gain;
            if( dice(1,2) == 1 && get_skill(ch,gsn_recuperate) > 50 )
                ++gain;
            if( dice(1,2) == 1 && get_skill(ch,gsn_recuperate) > 90 )
                ++gain;
        }

#ifdef INFO
        print_char( ch, "+recup: %d.\n\r", gain );
#endif

        if( dice(1,2) == 1 )
        {
            if( ch->class == CLASS_PALADIN || ch->class == CLASS_WARRIOR || ch->class == CLASS_BARBARIAN || ch->class == CLASS_BLACK_KNIGHT  )
                gain += con_app[ condition ].hith;
            else
                gain += con_app[ condition ].hitl;
        }

#ifdef INFO
        print_char( ch, "+kondycha: %d.\n\r", gain );
#endif
        if( dice(1,3) != 1 )
            gain += URANGE ( 0, ch->level / 12, 2 );

#ifdef INFO
        print_char( ch, "+lev: %d.\n\r", gain );
#endif

        if ( ch->condition[ COND_HUNGER ] == 0 )
            gain /= 2;

        if ( ch->condition[ COND_THIRST ] == 0 )
            gain /= 2;

        if ( ch->condition[ COND_SLEEPY ] == 0 )
            gain /= 2;


#ifdef INFO
        print_char( ch, "-glodny/spragniony/senny: %d.\n\r", gain );
#endif

    }

    gain = gain * ch->in_room->heal_rate / 100;
    gain += sector_table[ ch->in_room->sector_type ].heal_bonus * gain / 100;

#ifdef INFO
    print_char( ch, "+room&sector_heal_rate: %d.\n\r", gain );
#endif

    // bonus consecrate i desecrate, tylko dla dobrych i z³ych
    if (EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE))
    {
        if(IS_GOOD(ch))
        {
            ++gain;
        }
        else if(IS_EVIL(ch) )
        {
            --gain;
        }
    }
    else if (EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE))
    {
        if(IS_GOOD(ch))
        {
            --gain;
        }
        else if(IS_EVIL(ch) )
        {
            ++gain;
        }
    }

#ifdef INFO
    print_char( ch, "+consectare/desecrate: %d.\n\r", gain );
#endif

    if ( ch->on != NULL && ch->on->item_type == ITEM_FURNITURE )
        gain = gain * ch->on->value[ 3 ] / 100;

#ifdef INFO
    print_char( ch, "+meble: %d.\n\r", gain );
#endif

    // teraz obcinamy
    if ( IS_AFFECTED ( ch, AFF_POISON ) )
        gain /= 5;

    if ( IS_AFFECTED ( ch, AFF_PLAGUE ) )
        gain /= 8;

    if ( IS_AFFECTED ( ch, AFF_HASTE ) || IS_AFFECTED ( ch, AFF_SLOW ) )
        gain /= 2 ;

    if ( is_affected( ch, gsn_energy_strike ) )
        gain = (gain*2)/3;

#ifdef INFO
    print_char( ch, "-poison/plague/haste/slow/energy_strike: %d.\n\r", gain );
#endif

    /*
     * aura of vigor bonus
     */
    if ( is_affected( ch, gsn_aura_of_vigor ) )
    {
        gain *= 2;
        gain = UMAX( gain, 5 );
#ifdef INFO
        print_char( ch, "+aura of vigor: %d.\n\r", gain );
#endif
    }

    // testujemy energize
    if ( !IS_NPC ( ch ) && IS_AFFECTED ( ch, AFF_ENERGIZE ) )
    {
        paf = affect_find ( ch->affected, gsn_energize );
        if ( paf->level > 0 )
            gain *= 2;
        else
            gain /= 6;

#ifdef INFO
        print_char( ch, "+/- energize: %d.\n\r", gain );
#endif
    }

    if( ch->position == POS_STUNNED ) gain /= 2;

    if ( dice(1,4) == 1 && gain < 1 )
        gain = 1;

#ifdef INFO
    print_char( ch, "FINAL MV_REGEN: %d.\n\r", gain );
#endif

    /* start newbie move gain - by Fuyara */
    if ( IS_NEWBIE ( ch ) )  // do 8 levelu jest newbie
    {
        gain += gain * ( LEVEL_NEWBIE - ch->level + 1 ) / 10; // dodatkowo X procent gain (od 100% na 1 lev do 10% na 10 lev)
    }
    /* end newbie move gain */

    return UMIN ( gain, ch->max_move - ch->move );
}


void check_malnutrition ( CHAR_DATA *ch )
{
    AFFECT_DATA aff, *pAff;
    int level;

	//je¶li koñ ma je¼d¼ca, to siê mêczy, chce mu siê piæ i je¶æ
	//w celu uproszczenia nie mêczy siê, je¶li nikt go nie dosiada
    if ( IS_NPC ( ch ) && !ch->mounting)
        return ;

    //jesli koles jest glodny i nie ma flagi, dajemy i out
    if ( ch->condition[ COND_HUNGER ] == 0 && !is_affected ( ch, gsn_malnutrition ) )
    {
        aff.where	= TO_AFFECTS;
        aff.type	= gsn_malnutrition;
        aff.level	= 0;
        aff.duration	= -1;
        aff.rt_duration = 0;
        aff.bitvector = &AFF_NONE;
        aff.location	= APPLY_NONE;
        aff.modifier	= 0;
        affect_to_char ( ch, &aff, NULL, FALSE );
        return ;
    }

    //jesli nie ma flagi out
    if ( !is_affected ( ch, gsn_malnutrition ) )
        return ;

    //jesli jakims cudem nie znalazlo affecta, out
    if ( ( pAff = affect_find ( ch->affected, gsn_malnutrition ) ) == NULL )
        return ;


    if ( ch->condition[ COND_HUNGER ] == 0 )
    {
        pAff->level++;
    }
    else
    {
        pAff->level -= 3;
    }

    //zadnych nowych affectow nie dajemy
    if ( pAff->level < 0 )
    {
        affect_strip ( ch, gsn_malnutrition );
        return ;
    }

    level = pAff->level;

    //zdejmujemy obecny affect
    affect_strip ( ch, gsn_malnutrition );

    aff.where	= TO_AFFECTS;
    aff.type	= gsn_malnutrition;
    aff.level	= level;
    aff.duration	= -1;
    aff.rt_duration = 0;
    aff.bitvector = &AFF_NONE;
    aff.location	= APPLY_STR;
    aff.modifier	= 0;

    if ( level > 36 )
    {
        aff.modifier	= -5;
    }
    else if ( level > 30 )
    {
        aff.modifier	= -4;
    }
    else if ( level > 24 )
    {
        aff.modifier	= -3;
    }
    else if ( level > 18 )
    {
        aff.modifier	= -2;
    }
    else if ( level > 12 )
    {
        aff.modifier	= -1;
    }
    else
    {
        aff.modifier	= 0;
    }

    affect_to_char ( ch, &aff, NULL, TRUE );
    return ;
}

void gain_condition(CHAR_DATA *ch, int iCond, int value)
{
	int condition;

	if (value == 0 || ch->level >= LEVEL_IMMORTAL)
		return;

	condition = ch->condition[ iCond ];

	if (condition == -1)
		return;

	if(IS_NPC ( ch ) && !EXT_IS_SET(ch->act, ACT_MOUNTABLE))
		return;

	ch->condition[ iCond ] = URANGE( 0, condition + value, 48);

	if(IS_NPC ( ch ))
		return;

	if (ch->condition[ iCond ] == 0 && ch->position > POS_SLEEPING)
	{
		switch (iCond)
		{
		case COND_HUNGER:
			send_to_char("Jeste¶ g³odn<&y/a/e>.\n\r", ch);
			break;

		case COND_THIRST:
			send_to_char("Jeste¶ spragnion<&y/a/e>.\n\r", ch);
			break;

		case COND_DRUNK:
			if (condition != 0)
				send_to_char("Masz kaca.\n\r", ch);
			break;

		case COND_SLEEPY:
			send_to_char("Jeste¶ zmêczon<&y/a/e> i senn<&y/a/e>.\n\r", ch);
			break;
		}
	}
	return;
}

/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update(void)
{
	CHAR_DATA *ch;
	CHAR_DATA *ch_next;
	EXIT_DATA *pexit;
	char *msg=NULL, *msg_tochar=NULL;
	int door;
	//    char buf[MAX_STRING_LENGTH];


	/* Examine all mobs. */
	for (ch = char_list; ch != NULL; ch = ch_next)
	{
		int rnum;

		ch_next = ch->next;

		if (!IS_NPC (ch) || ch->fighting || !can_move(ch) || IS_AFFECTED (ch, AFF_CHARM) || !ch->in_room || ch->wait > 0)
			continue;

		if (!EXT_IS_SET (ch->act, ACT_SENTINEL) && !ch->fighting && ch->hunting && ch->position
				== POS_STANDING && can_move(ch))
		{
            if ( number_range( 0, get_curr_stat_deprecated(ch, STAT_INT) + get_curr_stat_deprecated(ch, STAT_WIS) + get_curr_stat_deprecated(ch, STAT_DEX) ) < 42 )
            {
                WAIT_STATE (ch, PULSE_VIOLENCE);
            }
			hunt_victim(ch);
			continue;
		}

		if ( !ch->fighting && ch->walking != 0 && ch->position == POS_STANDING
				&& can_move(ch) )
		{
			WAIT_STATE ( ch, PULSE_VIOLENCE );
			char_walk(ch);
			continue;
		}

		/* jesli jest w pustej kraince, albo nie ma update_always */
		if (ch->in_room->area->empty && !EXT_IS_SET ( ch->act,ACT_UPDATE_ALWAYS ))
			continue;

		if (ch->mounting && EXT_IS_SET (ch->act, ACT_AGGRESSIVE))
		{
			if (number_percent()<5)
				switch (number_range(0, 4))
				{
				case 0:
					msg="$N próbuje siê wyrwaæ, ale $S je¼dziec na to nie pozwala.";
					msg_tochar
							="$N próbuje zrzuciæ ciê z grzbietu, ale trzymasz $M mocno.";
					break;
				case 1:
					msg
							="$N otwiera szeroko paszczê i wydaje z siebie d³ugi, gard³owy ryk.";
					msg_tochar
							="$N otwiera szeroko paszczê i wydaje z siebie d³ugi, gard³owy ryk.";
					break;
				case 2:
					msg="$N warczy g³o¶no, gêsta ¶lina kapie mu z pyska.";
					msg_tochar="$N warczy g³o¶no, a gêsta ¶lina kapie mu z pyska.";
					break;
				case 3:
					msg="$N próbuje ugry¼æ swojego je¼dzca.";
					msg_tochar="$N wykrêca g³owê i próbuje ciê ugry¼æ.";
					break;
				case 4:
					msg="$N rozgl±da siê powarkuj±c cicho.";
					msg_tochar="$N rozgl±da siê powarkuj±c cicho.";
					break;
				}

			act (msg ,ch->mounting, NULL, ch, TO_NOTVICT);
			act (msg_tochar,ch->mounting, NULL, ch, TO_CHAR);
		}

		/* Examine call for special procedure */

		if (ch->spec_fun != 0)
		{
			if ( (*ch->spec_fun)(ch))
				continue;
		}

		if (ch->pIndexData->pShop != NULL) /* give him some gold */
        {
			if ( money_count_copper( ch ) < ch->pIndexData->wealth )
            {
				money_gain( ch, ch->pIndexData->wealth * number_range(10, 25)/100 );
            }
        }

		/*
		 * Check triggers only if mobile still in default position
		 */

		/*if (ch->position == ch->pIndexData->default_pos)*/
		{
			/* Delay */
			if (HAS_TRIGGER (ch, TRIG_DELAY)
			&& ch->prog_delay> 0)
			{
				if (--ch->prog_delay <= 0)
				{
					mp_percent_trigger(ch, NULL, NULL, NULL, &TRIG_DELAY);
					continue;
				}
			}

			if (!ch || !ch->in_room)
				continue;

			if (HAS_TRIGGER (ch, TRIG_RANDOM))
			{
				if (mp_percent_trigger(ch, NULL, NULL, NULL, &TRIG_RANDOM))
					continue;
			}

			if (!ch || !ch->in_room)
				continue;

		}

		/* That's all for sleeping / busy monster, and empty zones */
		if (ch->position != POS_STANDING)
			continue;

		//sklepikarze nie podnosza dupereli
		if (EXT_IS_SET (ch->act, ACT_SCAVENGER)
		&& ch->pIndexData->pShop == NULL
		&& ch->in_room->contents != NULL
		&& number_bits (6) == 0)
		{
			OBJ_DATA *obj;
			OBJ_DATA *obj_best;
			int max;

			max = 1;
			obj_best = 0;
			for (obj = ch->in_room->contents; obj; obj = obj->next_content)
			{
				if (CAN_WEAR (obj, ITEM_TAKE) && obj->cost> max && obj->cost> 0)
				{
					obj_best = obj;
					max = obj->cost;
				}
			}

			if (obj_best)
			{
				obj_from_room(obj_best);
				obj_to_char(obj_best, ch);
				act ("$n podnosi $h.", ch, obj_best, NULL, TO_ROOM);
			}
		}

		if (!ch || !ch->in_room)
			continue;

		if (ch->hit < get_max_hp(ch) / 2)
			rnum = 3;
		else
			rnum = 5;

		/* Wander */
		if (!EXT_IS_SET (ch->act, ACT_SENTINEL) && !ch->mounting && !ch->fighting && !ch->master
				&& can_move(ch) && ch->has_memdat <= 0 && (door = number_bits(rnum))
				<= 5 && (pexit = ch->in_room->exit[door]) != NULL && pexit->u1.to_room
				!= NULL && !IS_SET (pexit->exit_info, EX_CLOSED) && (!IS_SET (sector_table[ch->in_room->sector_type].flag, SECT_UNDERWATER)
				|| IS_AFFECTED (ch, AFF_SWIM))

		&& !IS_SET (pexit->exit_info, EX_NO_MOB) && ! (IS_SET (pexit->exit_info, EX_CLOSED) ||
		(IS_SET (pexit->exit_info, EX_SECRET)&& !IS_SET (pexit->exit_info, EX_HIDDEN)) ||
		(IS_SET (pexit->exit_info, EX_HIDDEN)&& !IS_AFFECTED (ch,AFF_DETECT_HIDDEN))) && !EXT_IS_SET (pexit->u1.to_room->room_flags, ROOM_NO_MOB)
				&& (!EXT_IS_SET (ch->act, ACT_STAY_AREA) || pexit->u1.to_room->area
						== ch->in_room->area) && (!EXT_IS_SET (ch->act, ACT_STAY_SECTOR) || pexit->u1.to_room->sector_type
				== ch->in_room->sector_type) && (!EXT_IS_SET (ch->act, ACT_OUTDOORS) || !EXT_IS_SET (pexit->u1.to_room->room_flags,ROOM_INDOORS)) && (!EXT_IS_SET (ch->act, ACT_INDOORS) || EXT_IS_SET (pexit->u1.to_room->room_flags,ROOM_INDOORS)))
		{
			if (rnum == 3)
			{
				if (ch->sex==0)
				{
					act ("$n ucieka przestraszone!", ch, NULL, NULL, TO_ROOM);
				}
				else if (ch->sex==1)
				{
					act ("$n ucieka przestraszony!", ch, NULL, NULL, TO_ROOM);
				}
				else
				{
					act ("$n ucieka przestraszona!", ch, NULL, NULL, TO_ROOM);
				}
			}

			move_char(ch, door, FALSE, NULL);

			if (ch->position < POS_STANDING)
				continue;
		}

        if (rnum == 3 && !EXT_IS_SET (ch->act, ACT_SENTINEL) && !ch->fighting)
        {
            CHAR_DATA *rch;

            for (rch = ch->in_room->people; rch; rch = rch->next_in_room)
            {

                /* If NPC can't see PC it shouldn't feel fear - Zen */
                if (!IS_NPC (rch) && can_see(ch, rch))
                {
                    int direction;

                    door = -1;

                    /* Find an exit giving each one an equal chance */
                    for (direction = 0; direction < MAX_DIR; direction++)
                    {
                        if (ch->in_room->exit[direction] && number_range(0, direction) == 0)
                            door = direction;
                    }

                    /* If no exit, attack.  Else flee! */
                    if (door == -1 && can_see(ch, rch) && get_curr_stat_deprecated(ch, STAT_INT) < 5)
                        multi_hit(ch, rch, TYPE_UNDEFINED);
                    else if (door != -1)
                        move_char(ch, door, FALSE, NULL);
                    break;
                }
            }
        }

    }

    return;
}

void update_condition(CHAR_DATA* ch)
{
	gain_condition(ch, COND_DRUNK, -1);
	gain_condition(ch, COND_THIRST, -1);
	gain_condition(ch, COND_HUNGER, ch->size > SIZE_MEDIUM ? -2 : -1);
	if ( !IS_IMMORTAL(ch))
	{
		check_malnutrition(ch);
		check_dehydration(ch);
	}

	if ( IS_SET ( sector_table[ ch->in_room->sector_type ].flag, SECT_THIRST ))
		ch->condition[ COND_THIRST ] = 0;

	if ( !IS_NPC ( ch ))
	{
		switch (ch->position)
		{
		case POS_RESTING:
			gain_condition(ch, COND_SLEEPY, 4);
			break;
		case POS_SLEEPING:
			gain_condition(ch, COND_SLEEPY, 8);
			break;
		default:
			gain_condition(ch, COND_SLEEPY, -1);
			break;
		}
	}
}
/*
 * Update all chars, including mobs.
 */
void char_update(void)
{
    CHAR_DATA * ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *ch_quit;
    AFFECT_DATA *inspire, nowe_inspire, *lastprayer;
    AFFECT_DATA * paf;
    AFFECT_DATA *paf_next;
    PFLAG_DATA *pflag, *pflag_next;
    int i;

    ch_quit = NULL;

    /* update save counter */
    save_number++;

    if (save_number > 29)
    {
        save_number = 0;
    }

    DEBUG_INFO("char_update::char_list::0");
    for (ch = char_list; ch != NULL; ch = ch_next)
    {
        DEBUG_INFO("char_update::char_list::1");
        ch_next = ch->next;
        if ( IS_NPC ( ch ) && HAS_TRIGGER ( ch, TRIG_TIME ))
        {
            DEBUG_INFO("char_update::char_list::2");
            mp_time_trigger(ch, time_info.hour);
        }

        /* jesli w progu koles zginal/nie jest w roomie zadnym */
        if ( !ch || !ch->in_room)
        {
            DEBUG_INFO("char_update: ch->in_room");
            continue;
        }

        if (ch->timer > 10 && ( IS_NPC ( ch ) || ( !IS_NPC ( ch ) && !IS_IMMORTAL ( ch ) ) ))
        {
            ch_quit = ch;
        }

        /* dekrementacja licznikow */
        DEBUG_INFO("char_update: before decrease");
        for (i = 0; i < MAX_COUNTER; i++)
        {
            if (ch->counter[ i ] > 0)
            {
                ch->counter[ i ] --;
            }
        }

        update_pos(ch);

        if (ch->position >= POS_STUNNED)
        {
            if ( IS_NPC ( ch ) &&
                    ch->zone != NULL &&
                    ch->zone != ch->in_room->area &&
                    ch->desc == NULL &&
                    ch->fighting == NULL &&
                    !IS_AFFECTED ( ch, AFF_CHARM ) &&
                    number_percent () < 75)
            {
                act ( "$n odchodzi w sin± dal.", ch, NULL, NULL, TO_ROOM );
                DEBUG_INFO("char_update:extract:pre");
                extract_char(ch, TRUE);
                DEBUG_INFO("char_update:extract:post");
                continue;
            }

            DEBUG_INFO("char_update: sector_table");
            if ( !IS_NPC ( ch ) && ch->in_room && sector_table[ ch->in_room->sector_type ].max_dam > 0)
            {
                int min, max;

                min = sector_table[ ch->in_room->sector_type ].min_dam;
                max = sector_table[ ch->in_room->sector_type ].max_dam;

                if (max > min)
                {
                    damage(ch, ch, number_range(min, max), TYPE_UNDEFINED, DAM_FIRE, FALSE);
                    /* zginal? */
                    if ( !ch || !ch->in_room)
                    {
                        continue;
                    }
                }
            }
        }

        if (ch->position == POS_STUNNED)
        {
            update_pos(ch);
        }

        DEBUG_INFO("char_update: checking lights");
        /* checking lights */
        if ( !IS_NPC ( ch ))
        {
            OBJ_DATA * obj;

            for (obj = ch->carrying; obj; obj = obj->next_content)
            {
                if (obj->wear_loc == WEAR_NONE)
                    continue;

                if (obj->item_type == ITEM_LIGHT && obj->value[ 2 ] > 0)
                {
                    if ( --obj->value[ 2 ] == 0 && ch->in_room != NULL)
                    {
                        --ch->in_room->light;
                        act ( "$p wypala siê.", ch, obj, NULL, TO_ROOM );
                        act ( "$p migocze przez chwilê i ga¶nie.", ch, obj, NULL, TO_CHAR );
                    }
                    else if (obj->value[ 2 ] <= 5 && ch->in_room != NULL)
                        act ( "$p migocze przez chwilê.", ch, obj, NULL, TO_CHAR );
                }
            }

            if ( ++ch->timer >= 12 && !IS_IMMORTAL ( ch ))
            {
                if (ch->was_in_room == NULL && ch->in_room != NULL)
                {
                    ch->was_in_room = ch->in_room;

                    if (ch->fighting != NULL)
                        stop_fighting(ch, TRUE);

                    send_to_char("Przenosisz siê do niebytu.\n\r", ch);
		    do_release_spirit(ch, NULL);
                    save_char_obj(ch, FALSE, FALSE);
                }
            }

            if ( !is_undead(ch) )
            {
                update_condition(ch);
            }
        }//!IS_NPC ( ch )
        else if(EXT_IS_SET(ch->act, ACT_MOUNTABLE)){
            //Je¶li to koñ, to modyfikacja pragnienia i g³odu
            if(ch->mounting)
                update_condition(ch);
        }

        for (paf = ch->affected; paf != NULL; paf = paf_next)
        {
            paf_next = paf->next;

            if (paf->duration > 0)
            {
                --paf->duration;
            }
            if (paf->rt_duration > 0)
            {
                --paf->rt_duration;
            }

            if (paf->duration == 0 && paf->rt_duration == 0)
            {
                if (paf->type > 0 && skill_table[ paf->type ].msg_off)
                {
                    send_to_char(skill_table[ paf->type ].msg_off, ch);
                    send_to_char("\n\r", ch);
                }
                affect_remove(ch, paf);
            }
        }

        if (is_affected(ch, gsn_inspire) )
        {
            inspire = affect_find(ch->affected, gsn_inspire);
            if (inspire->duration == 2)
            {
                nowe_inspire.where = inspire->where;
                nowe_inspire.type = inspire->type;
                nowe_inspire.level = inspire->level;
                nowe_inspire.duration = 2;
                nowe_inspire.rt_duration = 0;
                nowe_inspire.location = inspire->location;
                nowe_inspire.modifier = inspire->modifier;
                nowe_inspire.bitvector = &AFF_NONE;
                affect_to_char(ch, &nowe_inspire, "-Czujesz siê zawiedzion<&y/a/e>",
                        TRUE);
                send_to_char("Twój bojowy nastrój opada, czujesz wielki zawód.\n\r", ch);
                affect_remove(ch, inspire);
            }
        }

        if (is_affected(ch, gsn_prayer_last))
        {
            lastprayer = affect_find(ch->affected, gsn_prayer_last);
            lastprayer->modifier++;

            if (ch->position > POS_SLEEPING)
            {
                if (lastprayer->modifier == 24)
                {
                    send_to_char(
                            "Czujesz jak boska si³a powoli ga¶nie w twoim wnêtrzu. Najwy¿szy czas sie pomodliæ.\n\r",
                            ch);
                    switch (ch->sex)
                    {
                        case SEX_FEMALE:
                            act( "$n zatrzymuje siê na chwilê jakby zda³a sobie sprawê z czego¶ bardzo niepokoj±cego.", ch, NULL, NULL, TO_ROOM );
                            break;
                        case SEX_MALE:
                            act( "$n zatrzymuje siê na chwilê jakby zda³ sobie sprawê z czego¶ bardzo niepokoj±cego.", ch, NULL, NULL, TO_ROOM );
                            break;
                        case SEX_NEUTRAL:
                        default:
                            act( "$n zatrzymuje siê na chwilê jakby zda³o sobie sprawê z czego¶ bardzo niepokoj±cego.", ch, NULL, NULL, TO_ROOM );
                            break;
                    }
                }
                else if (lastprayer->modifier == 36)
                {
                    send_to_char(
                            "Czujesz, ze czego¶ zaczyna ci bardzo brakowac... jakby opuszcza³a cie boska si³a.\n\r",
                            ch);
                    switch (ch->sex)
                    {
                        case SEX_FEMALE:
                            act( "$n przystaje na chwilê i z niepokojem ogl±da swe d³onie. Czyni to z takim lêkiem w oczach jakby dostrzeg³a odp³ywaj±ce z nich ¿ycie.", ch, NULL, NULL, TO_ROOM );
                            break;
                        case SEX_MALE:
                            act( "$n przystaje na chwilê i z niepokojem ogl±da swe d³onie. Czyni to z takim lêkiem w oczach jakby dostrzeg³ odp³ywaj±ce z nich ¿ycie.", ch, NULL, NULL, TO_ROOM );
                            break;
                        case SEX_NEUTRAL:
                        default:
                            act( "$n przystaje na chwilê i z niepokojem ogl±da swe d³onie. Czyni to z takim lêkiem w oczach jakby dostrzeg³o odp³ywaj±ce z nich ¿ycie.", ch, NULL, NULL, TO_ROOM );
                            break;
                    }
                }
                else if (lastprayer->modifier == 48)
                {
                    print_char(
                            ch,
                            "Ogarnia ciê nagle uczucie tak straszliwej pustki, ¿e a¿ padasz na kolana.  Moc dobra opuszcza ciê... na w³asne ¿yczenie odwróci³%s¶ siê od ¦wiat³o¶ci.\n\r",
                            ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "e");
                    act( "$n gwa³townym ruchem ³apie siê obur±cz za g³owê poczym upada na kolana.", ch, NULL, NULL, TO_ROOM );
                    switch (ch->sex)
                    {
                        case SEX_FEMALE:
                            act( "Po krótkiej chwili wstaje, lecz widaæ, ¿e w jej oczach wygas³ ten charakterystyczny blask.", ch, NULL, NULL, TO_ROOM );
                            break;
                        case SEX_MALE:
                        case SEX_NEUTRAL:
                        default:
                            act( "Po krótkiej chwili wstaje, lecz widaæ, ¿e w jego oczach wygas³ ten charakterystyczny blask.", ch, NULL, NULL, TO_ROOM );
                            break;
                    }
                }
            }
            lastprayer->modifier = UMIN(50, lastprayer->modifier);
        }

        /*
         * paladin_auras
         */
        char_update_paladin_auras( ch );

        /*pflag system*/
        for (pflag = ch->pflag_list; pflag; pflag = pflag_next)
        {
            pflag_next = pflag->next;

            if ( NOPOL ( pflag->id[ 0 ] ) == 'R' &&
                    NOPOL ( pflag->id[ 1 ] ) == 'E' &&
                    NOPOL ( pflag->id[ 2 ] ) == 'G')
                continue;

            if (pflag->duration > 0)
            {
                pflag->duration--;

                if (pflag->duration == 0)
                    removecharflag(ch, pflag->id);
            }

        }
        /*************/

        if (is_affected(ch, gsn_plague) && ch != NULL)
        {
            MURDER_LIST * tmp_death;
            AFFECT_DATA *af, plague;
            CHAR_DATA *vch;
            int dam;
            int vnum;

            if (ch->in_room == NULL)
                continue;

            if ( !is_affected(ch, gsn_perfect_self))
                act ( "$n wije siê w agonii kiedy zaraza rozsadza wrzody na $s skórze.", ch, NULL, NULL, TO_ROOM );
            send_to_char(
                    "Wijesz siê w agonii do jakiej doprowadzi³a ciê zaraza.\n\r", ch);

            for (af = ch->affected; af != NULL; af = af->next)
            {
                if (af->type == gsn_plague)
                    break;
            }

            if (af == NULL)
            {
                EXT_REMOVE_BIT( ch->affected_by, AFF_PLAGUE );
                continue;
            }

            if (af->level == 1)
                continue;

            plague.where = TO_AFFECTS;
            plague.type = gsn_plague;
            plague.level = af->level - 1;
            plague.duration = number_range( 1, 2 * plague.level);
            plague.rt_duration = 0;
            plague.location = APPLY_STR;
            plague.modifier = -5;
            plague.bitvector = &AFF_PLAGUE;

            for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
            {
                if ( !is_safe(vch, ch, TRUE) && !saves_spell(plague.level - 2, vch,
                            DAM_DISEASE) && !IS_IMMORTAL ( vch ) && !IS_AFFECTED ( vch, AFF_PLAGUE ) && number_bits( 4) == 0)
                {
                    send_to_char("Masz gor±czkê i czujesz siê do¶æ kiepsko.\n\r", vch);
                    act ( "$n ma dreszcze i wgl±da do¶æ kiepsko.", vch, NULL, NULL, TO_ROOM );
                    affect_join(vch, &plague);
                }
            }

            dam = UMIN(ch->level, af->level / 5 + 1);
            ch->move = UMAX( 0, ch->move - dam);
            vnum = ch->in_room ? ch->in_room->vnum : 0;
            damage(ch, ch, dam, gsn_plague, DAM_DISEASE, FALSE);
            if ( !IS_NPC( ch ) && !ch->in_room)
            {
                tmp_death = malloc( sizeof( *tmp_death ));
                tmp_death->name = str_dup("plagê");
                tmp_death->char_level = ch->level;
                tmp_death->victim_level = 0;
                tmp_death->room = vnum;
                tmp_death->time = current_time;
                tmp_death->next = ch->pcdata->death_statistics.pkdeath_list;
                ch->pcdata->death_statistics.pkdeath_list = tmp_death;
            }
        }
        else if ( IS_AFFECTED ( ch, AFF_POISON ) && ch != NULL)
        {
            update_poison(ch);

            if ( !ch->in_room)
                continue;
        }
        else if (ch->position == POS_INCAP && number_range( 0, 1) == 0)
        {
            damage(ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, FALSE);
        }
        else if (ch->position == POS_MORTAL)
        {
            damage(ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, FALSE);
        }
        DEBUG_INFO("char_update: enter bleed is bleeding");
        /*krwawienie ran po tricku bleed*/
        if (is_affected(ch, gsn_bleed) )
        {
            DEBUG_INFO("char_update: enter bleed is bleeding (1)");
            if ( !is_affected(ch, gsn_bandage) && number_percent() < 50)
            {
                DEBUG_INFO("char_update: enter bleed is bleeding (2)");
                AFFECT_DATA * paf_bleed;
                paf_bleed = affect_find(ch->affected, gsn_bleed);
                if ( IS_NPC( ch ))
                {
                    DEBUG_INFO("char_update: enter bleed is bleeding (3)");
                    damage(ch, ch, dice( 2, -4 * paf_bleed->modifier), TYPE_UNDEFINED, DAM_NONE, FALSE);
                }
                else
                {
                    DEBUG_INFO("char_update: enter bleed is bleeding (4)");
                    damage(ch, ch, dice( 2, -4 * paf_bleed->modifier), TYPE_UNDEFINED, DAM_NONE, FALSE);
                    send_to_char("Twoje rany krwawi±.\n\r", ch);
                }
                act("Rany $z krwawi±.",ch, NULL, NULL, TO_ROOM );
            }
            else
            {
                send_to_char("Opatrunek powstrzymuje up³yw krwi.\n\r", ch);
            }
        }
        DEBUG_INFO("char_update: quit bleed is bleeding");
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    DEBUG_INFO("char_update: enter autosave and autoquit (0)");
    for (ch = char_list; ch != NULL; ch = ch_next)
    {
        DEBUG_INFO("char_update: enter autosave and autoquit (1)");
        ch_next = ch->next;

        if (ch->desc != NULL)
        {
            DEBUG_INFO("char_update: enter autosave and autoquit (2)");
            save_char_obj(ch, FALSE, FALSE);
        }

        /* tu trzeba zamotac */
        if (ch == ch_quit)
        {
            DEBUG_INFO("char_update: enter autosave and autoquit (3)");
            /* jesli moze zarentowac, to zapisujemy postac
               naliczamy renta i quit
             */
            if (check_rent(ch, ch->carrying, FALSE) )
            {
                DEBUG_INFO("char_update: enter autosave and autoquit (4)");
                save_char_obj(ch, FALSE, FALSE);
                do_function(ch, &do_quit, "");
            }
            else
            {
                DEBUG_INFO("char_update: enter autosave and autoquit (5)");
                if ( !IS_IMMORTAL ( ch ))
                {
                    DEBUG_INFO("char_update: enter autosave and autoquit (6)");
                    drop_objects(ch, ch->carrying, TRUE);
                }

                save_char_obj(ch, FALSE, FALSE);
                do_function(ch, &do_quit, "");
            }
        }
    }

    return;
}

void room_update ( bool tick )
{
    ROOM_UPDATE_TYPE * el, *el_next, *list;
    PFLAG_DATA *pflag, *pflag_next;
    CHAR_DATA *ch;
    bool exit = TRUE;

    if ( tick )
        list = room_update_list.tick_update_list;
    else
        list = room_update_list.pulse_update_list;

    for ( el = list ; el; el = el_next )
    {
        el_next = el->next;

        /* jesli nieaktywny */
        if ( !el->active )
            continue;

        switch ( el->type )
        {
            case TYPE_PFLAG:
                for ( pflag = el->room->pflag_list ; pflag ; pflag = pflag_next )
                {
                    pflag_next = pflag->next;

                    if ( NOPOL ( pflag->id[ 0 ] ) == 'R' &&
                         NOPOL ( pflag->id[ 1 ] ) == 'E' &&
                         NOPOL ( pflag->id[ 2 ] ) == 'G' )
                        continue;

                    if ( pflag->duration > 0 )
                    {
                        pflag->duration--;

                        if ( pflag->duration == 0 )
                        {
                            removeroomflag( el->room, pflag->id );
                            el->active = FALSE;
                        }
                    }
                }
                break;

                /* tylko jesli sa gracze w roomie */
            case TYPE_TICK_RANDOM:

                for ( ch = el->room->people; ch; ch = ch->next_in_room )
                    if ( !IS_NPC ( ch ) )
                        exit = FALSE;

                if ( exit )
                    break;

                if ( HAS_RTRIGGER ( el->room, TRIG_TICK_RANDOM ) )
                    rp_random_trigger ( el->room, &TRIG_TICK_RANDOM );
                break;

                /* tylko jesli sa gracze w roomie */
            case TYPE_PULSE_RANDOM:
                for ( ch = el->room->people; ch; ch = ch->next_in_room )
                    if ( !IS_NPC ( ch ) )
                        exit = FALSE;

                if ( exit )
                    break;

                if ( HAS_RTRIGGER ( el->room, TRIG_RANDOM ) )
                    rp_random_trigger ( el->room, &TRIG_RANDOM );
                break;

                /* moze byc pusty */
            case TYPE_TIME:
                if ( HAS_RTRIGGER ( el->room, TRIG_TIME ) )
                    rp_time_trigger ( el->room, time_info.hour );
                break;

            default : break;
        }
    }


    return ;
}

bool removeobjflag (OBJ_DATA *obj, char *argument);
void op_flagoff_trigger (char *argument, OBJ_DATA *obj);

void obj_update ( void )
{
    OBJ_DATA * obj, *in_obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf, *paf_next;
    bool on_ground_extract;
    bool found;
    CHAR_DATA *ch;

    DEBUG_INFO ( "obj_update:function start" );
    for ( obj = object_list; obj != NULL; obj = obj_next )
    {
        CHAR_DATA * rch;
        PFLAG_DATA *pflag;
        char *message;
        OBJ_NEXT( obj, obj_next );

        DEBUG_INFO ( "obj_update:standalone light" );
        //sprawdzanie pochodni zatknietych
        if ( ( obj->carried_by == NULL )
            && ( obj->in_room != NULL )
            && ( obj->item_type == ITEM_LIGHT )
            && ( obj->value[4] > 0 ) )
        {
            --obj->value[4];
            obj->value[2] = obj->value[4];
            ch = obj->in_room->people;
            if ( ( obj->value[4] == 0 )
                && ( obj->in_room->light > 0 ) )
            {
                --obj->in_room->light;
                if ( ch != NULL )
                {
                    act( "$p wypala siê.", ch, obj, NULL, TO_ALL );
                }
            }
            if ( !EXT_IS_SET( obj->in_room->room_flags, ROOM_INDOORS )
                || IS_SET( sector_table[ obj->in_room->sector_type ].flag, SECT_NOWEATHER ) )
            {
                if ( ( obj->value[3] == VUL_RAIN )
                    && ( weather_info[obj->in_room->sector_type].sky > 1 )
                    && ( obj->value[4] > 0 )
                    && ( number_percent() > 90 ) ) //10% szans na zgaszenie
                {
                    if ( ch != NULL )
                    {
                        act( "$p skwierczy przez chwilê i ga¶nie zupe³nie w deszczu.", ch, obj, NULL, TO_ALL );
                    }
                    obj->value[4] = 0;
                    obj->value[2] = 0;
                    if ( obj->in_room->light > 0 )
                        --obj->in_room->light;
                }
                if ( ( obj->value[3] == VUL_WIND )
                    && ( weather_info[obj->in_room->sector_type].sky = 1 )
                    && ( obj->value[4] > 0 )
                    && ( number_percent() > 90 ) ) //10% szans na zgaszenie
                {
                    if ( ch != NULL )
                    {
                        act( "Nag³y podmuch wiatru zgasi³ $p.", ch, obj, NULL, TO_ALL );
                    }
                    obj->value[4] = 0;
                    obj->value[2] = 0;
                    if ( obj->in_room->light > 0 )
                        --obj->in_room->light;
                }
            }
        }
        //koniec obslugi pochodni zatknietych

        //rellik: komponenty, aktualizowanie timera
        DEBUG_INFO ( "obj_update:spell items" );
        if ( obj->is_spell_item )
        {
            int slot;
            if ( obj->spell_item_timer > 0 )
            {
                --obj->spell_item_timer;
                if ( obj->spell_item_timer < 1 )
                {
                    obj->is_spell_item = FALSE;
                    slot = spell_item_get_slot_by_item_name( obj );
                    if ( slot > -1 && spell_items_table[slot].item_blows_timer )
                    {
                        spell_item_destroy( TRUE, obj, spell_items_table[slot].key_number );
                    }
                    else if ( obj->carried_by && !IS_NPC(obj->carried_by) && number_percent() < 30 && know_magic_of_item( obj->carried_by, obj ) )
                    {
                        print_char( obj->carried_by, "Czujesz, ¿e %s traci swoj± moc.", obj->short_descr );
                    }
                }
            }
        }

        DEBUG_INFO ( "obj_update:shapeshift items" );
        if ( obj->shapeshift == TRUE) continue; //graty ktore sa w shapeshif_carrying pomijamy

        DEBUG_INFO ( "obj_update:pflag" );
        for ( pflag = obj->pflag_list ; pflag ; pflag = pflag->next )
        {
            if ( NOPOL ( pflag->id[ 0 ] ) == 'R' &&
                 NOPOL ( pflag->id[ 1 ] ) == 'E' &&
                 NOPOL ( pflag->id[ 2 ] ) == 'G' )
                continue;

            if ( pflag->duration > 0 )
            pflag->duration--;
        }

        do
        {
            found = FALSE;
            for ( pflag = obj->pflag_list ; pflag ; pflag = pflag->next )
            {
                if ( NOPOL ( pflag->id[ 0 ] ) == 'R' &&
                     NOPOL ( pflag->id[ 1 ] ) == 'E' &&
                     NOPOL ( pflag->id[ 2 ] ) == 'G' )
                    continue;

                if ( pflag->duration == 0 )
                {
                    if ( HAS_OTRIGGER ( obj, TRIG_FLAGOFF ) )
                        op_flagoff_trigger ( pflag->id, obj );
                    if ( IS_VALID( obj ) )
                    {
                        removeobjflag ( obj, pflag->id );
                        found = TRUE;
                    }
                    break;
                }
            }
        }
        while ( found );

        if ( !IS_VALID( obj ) )
            continue;

        if ( HAS_TRIGGER ( obj, TRIG_TIME ) )
            op_time_trigger ( obj, time_info.hour );

        if ( !IS_VALID( obj ) )
            continue;

	/* begin brzydki kod */
	DEBUG_INFO ( "obj_update:art_destroy" );
	if ( check_rt_art_destroy( obj, 0 ) )
	   {

		char buf [MSL];

		sprintf( buf, "NISZCZENIE ARTA Z TIMERA: Posiadacz:%s Avnum:%s,%d", obj->carried_by ? obj->carried_by->name : "(nikt)", obj->name, obj->pIndexData->vnum );

		log_string( buf );
		wiznet( buf, NULL, NULL, WIZ_ARTEFACT, 0, 39 );

		append_file_format_daily(NULL, ARTEFACT_LOG_FILE, "%s -> NISZCZENIE ARTA Z TIMERA :%s vnum:%d (DEL)", obj->carried_by ? obj->carried_by->name : "(nikt)", obj->short_descr, obj->pIndexData->vnum);
		//

		obj->timer = 1; // o to jest bardzo brzydkie. poniewaz tutaj ustawiamy '1' to nieco nizej timer ten spadnie z 1 na 0 i obiekt zostanie zniszczony

	   }
	/* end brzydki kod */

	show_rt_art_destroy_info( obj );

        DEBUG_INFO ( "obj_update:affects" );
        for ( paf = obj->affected; paf != NULL; paf = paf_next )
        {
            paf_next = paf->next;

	    if (paf->rt_duration > 0)
	      {
		--paf->rt_duration;
	      }

            if ( paf->duration > 0 )
            {
                paf->duration--;
            }
            else if ( paf->duration < 0 )
                ;
            else
            {
                if ( paf_next == NULL
                     || paf_next->type != paf->type
                     || paf_next->duration > 0 )
                {
                    if ( paf->type > 0 && skill_table[ paf->type ].msg_obj )
                    {
                        if ( obj->carried_by != NULL )
                        {
                            rch = obj->carried_by;
                            act ( skill_table[ paf->type ].msg_obj, rch, obj, NULL, TO_CHAR );
                        }
                        if ( obj->in_room != NULL
                             && obj->in_room->people != NULL )
                        {
                            rch = obj->in_room->people;
                            act ( skill_table[ paf->type ].msg_obj,
                                  rch, obj, NULL, TO_ALL );
                        }
                    }
                }

                DEBUG_INFO ( "obj_update:hardiness" );
                if( paf->type == gsn_hardiness )
                	obj->condition = UMIN( obj->condition - 1 , UMAX( number_range( 10,20),number_range( paf->level*2, paf->level ) ) );

                affect_remove_obj ( obj, paf );
            }
        }

        DEBUG_INFO ( "obj_update:smoking" );
        if ( obj->item_type == ITEM_PIPE && obj->value[ 0 ] == 1 && obj->carried_by )
        {
        	obj->value[ 1 ] -= number_range( 1, 3 );
        	if ( obj->value[ 1 ] <= 0 )
			{
				act( "Ostatnie, tl±ce siê iskierki gasn±, kiedy wypali³a siê ca³a zawarto¶æ $f.", obj->carried_by, obj, NULL, TO_CHAR );
				act( "Widzisz jak $p $z ga¶nie z lekkim sykiem.", obj->carried_by, obj, NULL, TO_ROOM );
				obj->value[ 0 ] = 0;
				obj->value[ 1 ] = 0;
				obj->value [ 2 ] = 0;
			}
			else
			{
				switch ( number_range( 1, 7 ) )
				{
					case 1:
						act( "Nad $j trzyman± przez $z unosi siê w±ska stru¿ka dymu.", obj->carried_by, obj, NULL, TO_ROOM );
						act( "Nad $j unosi siê w±ska stru¿ka dymu.", obj->carried_by, obj, NULL, TO_CHAR );
						break;
					case 2:
						act( "Nad $j $z unosz± siê ma³e, ciemne chmurki dymu.", obj->carried_by, obj, NULL, TO_ROOM );
						act( "Nad $j unosz± siê ma³e, ciemne chmurki dymu.", obj->carried_by, obj, NULL, TO_CHAR );
						break;
					case 3:
						act( "Widzisz jak w $f $z tl± siê ma³e, jasne iskierki.", obj->carried_by, obj, NULL, TO_ROOM );
						act( "Widzisz kilka ma³ych, jasnych iskierek tl±cych siê w $k.", obj->carried_by, obj, NULL, TO_CHAR );
						break;
					case 4:
						act( "Dostrzegasz, ¿e nad $f trzyman± przez $z unosi siê gêsta smuga dymu.", obj->carried_by, obj, NULL, TO_ROOM );
						act( "Dosrzegasz unosz±c± siê nad $f gêst± smugê dymu.", obj->carried_by, obj, NULL, TO_CHAR );
						break;
					case 5:
						act( "S³yszysz jak zawarto¶æ $f $z lekko syczy tl±c siê.", obj->carried_by, obj, NULL, TO_ROOM );
						act( "S³yszysz jak ziele w $j lekko syczy tl±c siê.", obj->carried_by, obj, NULL, TO_CHAR );
						break;
					case 6:
						act( "Czujesz delikatny aromat jakoby zio³owego dymu, bij±cy od $z i jego $f.", obj->carried_by, obj, NULL, TO_ROOM );
						act( "Czujesz bij±cy od $f lekki, jakby zio³owy aromat.", obj->carried_by, obj, NULL, TO_CHAR );
						break;
					default:
						act( "Zawarto¶æ $f nale¿±cej do $z tli siê lekko.", obj->carried_by, obj, NULL, TO_ROOM );
						act( "Zawarto¶æ $f tli siê lekko.", obj->carried_by, obj, NULL, TO_CHAR );
						break;
				}
			}
	}

        DEBUG_INFO ( "obj_update:destroy_food:0" );
        if ( obj->item_type == ITEM_FOOD )
        {
            CHAR_DATA * keeper;
            OBJ_DATA *in_obj;

            /* tylko w posiadaniu graczy */
            for ( in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj );

            DEBUG_INFO ( "obj_update:destroy_food:1" );
            keeper = in_obj->carried_by;

            if ( keeper && !IS_NPC ( keeper ) )
            {
                DEBUG_INFO ( "obj_update:destroy_food:2" );
                if ( obj->value[ 1 ] > 0 )
                {
                    obj->value[ 1 ] --;
                    /* no i tu wstawianie poisona albo znikanie*/
                    if ( obj->value[ 1 ] <= 0 )
                    {

                        if ( number_percent () < 20 )
                        {
                            DEBUG_INFO ( "obj_update:destroy_food:3" );
                            if ( obj->carried_by )
                                act ( "$p rozk³ada siê.", obj->carried_by, obj, NULL, TO_CHAR );

                            if ( obj->in_obj )
                                obj_from_obj ( obj );
                            else if ( obj->in_room )
                                obj_from_room ( obj );
                            else if ( obj->carried_by )
                            {
                                DEBUG_INFO ( "obj_update:destroy_food:4" );
                                /*artefact*/
                                if ( is_artefact( obj ) && !IS_NPC( keeper ) && !IS_IMMORTAL( keeper ) )
                                    artefact_from_char( obj, keeper );
                                obj_from_char ( obj );
                            }
                            /*artefact*/
                            if ( is_artefact( obj ) ) extract_artefact( obj );
                            if ( obj->contains ) extract_artefact_container( obj );

                            extract_obj ( obj );
                            continue;
                        }
                        else
                        {
                            DEBUG_INFO ( "obj_update:destroy_food:5" );
                            obj->value[ 3 ] = 1;
                        }
                    }
                }
            }
        }

        DEBUG_INFO ( "obj_update:destroy_inside:0" );
        // czy w jakims pojemniku
        for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj );
        DEBUG_INFO ( "obj_update:destroy_inside:1" );

        on_ground_extract = FALSE;

        DEBUG_INFO ( "obj_update:destroy_inside:2" );
        if ( obj->on_ground > 0 && !IS_OBJ_STAT( obj, ITEM_UNDESTRUCTABLE ) )
        {
            DEBUG_INFO ( "obj_update:destroy_inside:2:a" );
            if ( in_obj->in_room == NULL )
            {
                DEBUG_INFO ( "obj_update:destroy_inside:2:b" );
                obj->on_ground = 0;
            }
            else if ( --obj->on_ground < 1 )
            {
                DEBUG_INFO ( "obj_update:destroy_inside:2:c" );
                on_ground_extract = TRUE;
            }
        }

        DEBUG_INFO ( "obj_update:destroy_inside:3" );
        //obiekt jest cialkiem npc i ktos je wlasnie oskórowywuje
        if ( obj->item_type == ITEM_CORPSE_NPC && obj->value[ 4 ] == 1 )
        {
            DEBUG_INFO ( "obj_update:destroy_inside:3:a" );
            continue;
        }

        DEBUG_INFO ( "obj_update:destroy_inside:4" );
        //jesli ma zostac usuniete bo za dlugo juz lezy
        //to i tak timer ma wyzszy priorytet, zniknie wtedy jak timer zliczy
        if ( !on_ground_extract && ( obj->timer < 1 || --obj->timer > 0 ) )
        {
            DEBUG_INFO ( "obj_update:destroy_inside:4:a" );
            continue;
        }

        DEBUG_INFO ( "obj_update:destroy_message" );
	if( obj->liczba_mnoga )
	{
		switch ( obj->item_type )
		{
			default: message = "$p rozsypuj± siê w proch."; break;
			case ITEM_FOUNTAIN: message = "$p wysychaj±."; break;
			case ITEM_CORPSE_NPC: message = "$p rozsypuj± siê w proch."; break;
			case ITEM_CORPSE_PC: message = "$p rozsypuj± siê w proch."; break;
			case ITEM_FOOD: message = "$p rozk³adaj± siê.";	break;
			case ITEM_POTION: message = "$p wyparowywuj±.";break;
			case ITEM_PORTAL: message = "$p rozmywaj± siê, po czym znikaj±."; break;
			case ITEM_HORN: message = "$p rozpadaj± siê na kawa³ki."; break;
			case ITEM_PIPE: message = "$p rozpadaj± siê na kawa³ki."; break;
			case ITEM_WEED: message = "$p rozk³adaj± siê."; break;
			case ITEM_NOTEPAPER: message = "$p rozk³adaj± siê."; break;
			case ITEM_ARMOR: message = "$p rozpadaj± siê."; break;
			case ITEM_MONEY: message = "Stare, skorodowane monety rozsypuj± siê w proch."; break;
			case ITEM_CONTAINER:
	                if ( CAN_WEAR ( obj, ITEM_WEAR_FLOAT ) )
                    	if ( obj->contains )
	                        message = "$p migocz± przez chwilê i znikaj± rozsypuj±c swoj± zawarto¶æ.";
                    	else
                        	message = "$p migocz± przez chwilê i znikaj±.";
                	else
	                    message = "$p rozsypuj± siê w proch.";
                	break;
        	}
        }
		else
		{
			switch ( obj->item_type )
			{
				default: message = "$p rozsypuje siê w proch."; break;
				case ITEM_FOUNTAIN: message = "$p wysycha."; break;
				case ITEM_CORPSE_NPC: message = "$p rozsypuje siê w proch."; break;
				case ITEM_CORPSE_PC: message = "$p rozsypuje siê w proch."; break;
				case ITEM_FOOD: message = "$p rozk³ada siê.";	break;
				case ITEM_POTION: message = "$p wyparowywuje.";break;
				case ITEM_PORTAL: message = "$p rozmywa siê, po czym znika."; break;
				case ITEM_HORN: message = "$p rozpada siê na kawa³ki."; break;
				case ITEM_PIPE: message = "$p rozpada siê na kawa³ki."; break;
				case ITEM_WEED: message = "$p rozk³ada siê."; break;
				case ITEM_NOTEPAPER: message = "$p rozk³ada siê."; break;
				case ITEM_ARMOR: message = "$p rozpada siê."; break;
				case ITEM_MONEY: message = "Stara, skorodowana moneta rozsypuje siê w proch."; break;
				case ITEM_CONTAINER:
					if ( CAN_WEAR ( obj, ITEM_WEAR_FLOAT ) )
						if ( obj->contains )
							message = "$p migocze przez chwilê i znika rozsypuj±c swoj± zawarto¶æ.";
						else
							message = "$p migocze przez chwilê i znika.";
					else
						message = "$p rozsypuje siê w proch.";
					break;
			}
        }

        DEBUG_INFO ( "obj_update:destroy_main" );
        if ( obj->carried_by != NULL )
        {
            if ( IS_NPC ( obj->carried_by ) && obj->carried_by->pIndexData->pShop != NULL )
            {
                obj->carried_by->silver += obj->cost / 5;
            }
            else if ( obj->pIndexData->vnum == OBJ_VNUM_FIREFLIES )
            {
               act( "Nagle otaczaj±ce ciê ¶wietliki rozlatuj± siê na wszystkie strony, opuszczaj±c ciê.", obj->carried_by, obj, NULL, TO_CHAR );
               act( "Nagle otaczaj±ce $c ¶wietliki rozlatuj± siê na wszystkie strony.", obj->carried_by, obj, NULL, TO_NOTVICT );
            }
            else
            {
                act ( message, obj->carried_by, obj, NULL, TO_CHAR );
                if ( obj->wear_loc == WEAR_FLOAT )
                    act ( message, obj->carried_by, obj, NULL, TO_ROOM );
            }
        }
        else if ( obj->in_room != NULL
                  && ( rch = obj->in_room->people ) != NULL )
        {
            if ( ! ( obj->in_obj && !CAN_WEAR ( obj->in_obj, ITEM_TAKE ) ) )
            {
                act ( message, rch, obj, NULL, TO_ROOM );
                act ( message, rch, obj, NULL, TO_CHAR );
            }
        }

        DEBUG_INFO ( "obj_update:destroy_corpse_or_wearfloat" );
        if ( ( obj->item_type == ITEM_CORPSE_PC ||
               obj->wear_loc == WEAR_FLOAT ||
               obj->item_type == ITEM_CORPSE_NPC )
             && obj->contains )
        {   /* save the contents */
            OBJ_DATA * t_obj, *next_obj;

            for ( t_obj = obj->contains; t_obj != NULL; t_obj = next_obj )
            {
                next_obj = t_obj->next_content;
                obj_from_obj ( t_obj );

                if ( obj->in_obj )  /* in another object */
                    obj_to_obj ( t_obj, obj->in_obj );

                else if ( obj->carried_by )   /* carried */
                    if ( obj->wear_loc == WEAR_FLOAT )
                        if ( obj->carried_by->in_room == NULL )
                        {
                            /*artefact*/
                            if ( is_artefact( obj ) ) extract_artefact( obj );
                            if ( obj->contains ) extract_artefact_container( obj );

                            extract_obj ( t_obj );
                        }
                        else
                            obj_to_room ( t_obj, obj->carried_by->in_room );
                    else
                        obj_to_char ( t_obj, obj->carried_by );

                else if ( obj->in_room == NULL )   /* destroy it */
                {
                    /*artefact*/
                    if ( is_artefact( obj ) ) extract_artefact( obj );
                    if ( obj->contains ) extract_artefact_container( obj );

                    extract_obj ( t_obj );
                }
                else /* to a room */
                {
                    obj_to_room ( t_obj, obj->in_room );
                    t_obj->on_ground = 15;
                }
            }
        }
        /*artefact*/
        if ( is_artefact( obj ) ) extract_artefact( obj );
        if ( obj->contains ) extract_artefact_container( obj );

        extract_obj ( obj );
    }
    DEBUG_INFO ( "obj_update:function end" );
    return ;
}

/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update ( void )
{
    CHAR_DATA * wch;
    CHAR_DATA *wch_next;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *victim;
    char *target;
    OBJ_DATA *weapon;


    for ( wch = char_list; wch != NULL; wch = wch_next )
    {
        wch_next = wch->next;

        if ( IS_NPC ( wch )
             || wch->level >= LEVEL_IMMORTAL
             || wch->in_room == NULL
             || wch->in_room->area->empty )
            continue;

        for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
        {
            int count;

            ch_next	= ch->next_in_room;


            if ( !IS_NPC ( ch )
                 || ( !EXT_IS_SET ( ch->act, ACT_AGGRESSIVE ) && !is_hating ( ch, wch ) && !check_nature_curse_aggressive( ch, wch ))
                 || !can_see ( ch, wch )
                 || is_safe ( ch, wch, TRUE )
                 || IS_AFFECTED ( ch, AFF_CALM )
                 || ch->fighting != NULL
                 || IS_AFFECTED ( ch, AFF_CHARM )
                 || !IS_AWAKE ( ch )
                 || ( EXT_IS_SET ( ch->act, ACT_WIMPY ) && IS_AWAKE ( wch ) )
                 || number_bits ( 1 ) == 0
                 || !can_move ( ch )
                 || !wch->in_room 
                 || ( EXT_IS_SET ( wch->affected_by, AFF_SNEAK_INVIS ) && !EXT_IS_SET( ch->affected_by, AFF_PERFECT_SENSES )))
                continue;

			if ( undead_resemblance( wch, ch ) )
				continue;

            if ( is_hating ( ch, wch ) )
            {
                found_prey ( ch, wch );
                continue;
            }
            /*
             * Ok we have a 'wch' player character and a 'ch' npc aggressor.
             * Now make the aggressor fight a RANDOM pc victim in the room,
             *   giving each 'vch' an equal chance of selection.
             */
            count	= 0;
            victim	= NULL;

            for ( vch = wch->in_room->people; vch != NULL; vch = vch_next )
            {
                vch_next = vch->next_in_room;

                if ( !IS_NPC ( vch )
                     && vch->level < LEVEL_IMMORTAL
                     /*		&&   ch->level >= vch->level - 5 */
                     && ( !EXT_IS_SET ( ch->act, ACT_WIMPY ) || !IS_AWAKE ( vch ) )
                     && can_see ( ch, vch ) )
                {
                    if ( number_range ( 0, count ) == 0 )
                        victim = vch;
                    count++;
                }
            }

            if ( victim == NULL )
                continue;

            if ( victim->in_room == NULL )
                continue;


            if ( ( weapon = get_eq_char ( ch, WEAR_WIELD ) ) != NULL )
            {
                if ( ( target = get_target_by_name ( ch, victim ) ) == NULL )
                    target = victim->name;

                if ( weapon->value[ 0 ] == WEAPON_DAGGER && get_skill ( ch, gsn_backstab ) > 0 )
                    do_function ( ch, &do_backstab, target );
                else if ( get_skill(ch, gsn_charge) > 0 && IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS ) && weapon->value[0] != WEAPON_WHIP && weapon->value[0] != WEAPON_SHORTSWORD && weapon->value[0] != WEAPON_DAGGER )
                    do_function ( ch, &do_charge, target );
                else
                    multi_hit ( ch, victim, TYPE_UNDEFINED );

                continue;
            }

            multi_hit ( ch, victim, TYPE_UNDEFINED );
        }
    }
    return ;
}

void fight_ai_update ()
{
    CHAR_DATA * wch, *wch_next;

    for ( wch = char_list; wch != NULL; wch = wch_next )
    {
        wch_next = wch->next;

        si_update ( wch );
    }
    return ;
}

void memdat_update( void )
{
	MEMDAT * mdat, *mdat_next;

	for ( mdat = memdat_list; mdat; mdat = mdat_next )
	{
		mdat_next = mdat->next;

		mdat->delay -= PULSE_PER_SECOND;

		if ( !mdat->prog_env || mdat->delay > 0 )
			continue;

		switch ( mdat->prog_env->type )
		{
			case PROG_TYPE_MOB:
				if ( mdat->prog_env->ch )
				{
					mdat->prog_env->ch->has_memdat--;
					mdat->prog_env->ch->has_memdat = UMAX( 0, mdat->prog_env->ch->has_memdat );
					add_prog_env( mdat->prog_env );
					program_flow();
				}
				break;
			case PROG_TYPE_OBJ:
				if ( mdat->prog_env->obj )
				{
					mdat->prog_env->obj->has_memdat--;
					mdat->prog_env->obj->has_memdat = UMAX( 0, mdat->prog_env->obj->has_memdat );
					set_supermob( mdat->prog_env->obj );
					add_prog_env( mdat->prog_env );
					program_flow();
					release_supermob();
				}
				break;
			case PROG_TYPE_ROOM:
				if ( mdat->prog_env->room )
				{
					mdat->prog_env->room->has_memdat--;
					mdat->prog_env->room->has_memdat = UMAX( 0, mdat->prog_env->room->has_memdat );
					rset_supermob( mdat->prog_env->room );
					add_prog_env( mdat->prog_env );
					program_flow();
					release_supermob();
				}
				break;
		}

		free_mdat( mdat );
	}
	return;
}

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */

void update_handler ( void )
{
    CHAR_DATA       *ch;
    CHAR_DATA *ch_next;
    DESCRIPTOR_DATA *d;
    static int pulse_area;
    static int pulse_mobile;
    static int pulse_violence;
    static int pulse_point;
    static int pulse_music;
    static int pulse_fight_si;
    static int pulse_copyover;
    static int pulse_hint;
    static int pulse_event2;
    static int pulse_regen;
    static int pulse_todelete; // delayed ch delete - by Fuyara
    time_t     delta = 0;
    int        count = 0;
		//rellik: do debugowania
		const char *call = "update.c => update_handler";

    if ( ( misc.copyover_delayed >= 0 || misc.copyover_scheduled >= 0 ) && --pulse_copyover <= 0 )
    {
        pulse_copyover	= PULSE_PER_SECOND;
        if ( misc.copyover_scheduled > 0 )
        {
            delta = misc.copyover_scheduled - current_time;
            if ( delta < 300 )
            {
                misc.copyover_delayed = current_time + 300;
                misc.copyover_save = TRUE;
                misc.copyover_scheduled = -1;
            }
        }

        if ( misc.copyover_delayed == 0 )
        {
            for ( d = descriptor_list; d ; d = d->next )
                if ( d->character && !IS_IMMORTAL( d->character )
                     && ( d->connected == CON_PLAYING
                          || ( d->connected >= CON_MENU && d->connected <= CON_MENU_NEWPASSWD2 ) ) )
                    count++;

            if ( count == 0 )
                misc.copyover_delayed = current_time;
        }

        if ( misc.copyover_delayed > 0 )
        {
            delta = misc.copyover_delayed - current_time;
            if ( delta > 0 && ( delta % 60 == 0 || delta <= 10 ) )
            {
                for ( d = descriptor_list; d; d = d->next )
                {
                    if ( !d->character ) continue;
                    if ( d->connected == CON_PLAYING )
                    {
                        if ( delta > 10 )
                            printf_to_char( d->character, "^f{RPrze³adowanie ¶wiata za oko³o %d sekund!\n\r{x", delta );
                        else
                            printf_to_char( d->character, "{R%d\n\r{x", delta );
                    }
                }
            }

            if ( delta <= 0 )
            {
                char buf[ MAX_STRING_LENGTH ];

                for ( d = descriptor_list; d ; d = d->next )
                    if ( d->character && IS_IMMORTAL( d->character ) && d->connected == CON_PLAYING )
                    {
                        sprintf( buf, "\n\r ****************************************************\n\r" );
                        write_to_descriptor ( d, buf, 0 );
                        sprintf( buf, " **  Automatyczny Copyover                         **\n\r" );
                        write_to_descriptor ( d, buf, 0 );
                        sprintf( buf, " ****************************************************\n\r" );
                        write_to_descriptor ( d, buf, 0 );
                    }

                if ( !supermob )
                    init_supermob();

                free_string( supermob->name );
                supermob->name = str_dup( "*** SYSTEM ***" );
                do_copyover ( supermob, misc.copyover_save ? "now" : "now nosave" );
            }
        }
    }

    /* start delayed ch delete - by Fuyara */
    if ( --pulse_todelete <= 0 )
    {
	pulse_todelete = PULSE_TODELETE;
	delete_todelete( );
    }
    /* end delayed ch delete */

	if ( --pulse_hint <= 0)
      {
      pulse_hint = PULSE_HINT;
      hint_update ( );
      }


    if ( --pulse_area <= 0 )
    {
        pulse_area	= PULSE_AREA;
        /* number_range (PULSE_AREA / 2, 3 * PULSE_AREA / 2); */
        DEBUG_INFO ( "area_update" );
        area_update	();
        DEBUG_INFO ( NULL );
    }


    if ( --pulse_event2        <= 0 )
    {
        pulse_event2             = PULSE_EVENT2;
        event2_update();
    }


	if ( --pulse_music	<= 0 )
    {
        pulse_music	= PULSE_MUSIC;
    }

    if ( --pulse_mobile <= 0 )
    {
        OBJ_DATA * obj, *obj_next;
        pulse_mobile	= PULSE_MOBILE;


        DEBUG_INFO ( "mobile_update" );
        mobile_update	();
        DEBUG_INFO ( "room_update (false)" );
        room_update ( FALSE );


        DEBUG_INFO ( "petla_obj_random_trig" );
        for ( obj = object_list; obj != NULL; obj = obj_next )
        {
            OBJ_NEXT( obj, obj_next );

            if ( HAS_OTRIGGER ( obj, TRIG_RANDOM ) )
                op_common_trigger ( NULL, obj, &TRIG_RANDOM );
        }
        DEBUG_INFO ( NULL );
    }

    if ( --pulse_violence <= 0 )
    {
        CHAR_DATA * wch, *wch_next;

        pulse_violence	= PULSE_VIOLENCE;
        DEBUG_INFO ( "violence_update" );
        violence_update	();
        DEBUG_INFO ( "extern_si_update" );
        extern_si_update();
        DEBUG_INFO ( NULL );

        for ( wch = char_list; wch != NULL; wch = wch_next )
        {
            wch_next = wch->next;
            si_update_not_fight ( wch );
        }
    }

    if ( --pulse_regen <= 0 )
    {
    	pulse_regen = 2 * PULSE_VIOLENCE;
    	DEBUG_INFO ( "regen_update" );
    	regen_update();
    }

    if ( --pulse_point <= 0 )
    {
        misc.world_time++;
        save_misc_data();
        wiznet ( "TICK!", NULL, NULL, WIZ_TICKS, 0, 0 );
        pulse_point = ( 50 + number_range ( 0, 20 ) ) * PULSE_TICK / 60;
        DEBUG_INFO ( "weather_update" );
        weather_update	( TRUE );
        DEBUG_INFO ( "char_update" );
        char_update	();
        DEBUG_INFO ( "obj_update" );
        obj_update	();
        DEBUG_INFO ( "room_update" );
        room_update	( TRUE );
        DEBUG_INFO ( "update:bans" );
        update_bans();
		DEBUG_INFO ( "update:herbs" );
        update_herbs(call);
		DEBUG_INFO ( "update:global_flags" );
        update_global_flags();
        DEBUG_INFO ( NULL );
    }

    /* full speed stuff */

    DEBUG_INFO ( "wait:daze+stuff_pwait_cast" );
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
        DEBUG_INFO ( "wait:daze+stuff_pwait_cast(1)" );
        ch_next = ch->next;
        if ( IS_NPC ( ch ) && ch->pIndexData->vnum == 3 )
        {
            ch->wait = 0;
            ch->daze = 0;
            continue;
        }
        DEBUG_INFO ( "wait:daze+stuff_pwait_cast(2)" );

        if ( ch->daze < 0 )
        {
            ch->daze = 0;
        }
        else if ( ch->daze > 0 )
        {
        DEBUG_INFO ( "wait:daze+stuff_pwait_cast(3)" );
            --ch->daze;
        }

        if ( ch->wait > 0 )
        {
        DEBUG_INFO ( "wait:daze+stuff_pwait_cast(4)" );
            --ch->wait;
            pwait_cast_update ( ch );
            continue;
        }
    }

    DEBUG_INFO ( "fight_si:before" );
	if ( --pulse_fight_si <= 0 )
    {
        pulse_fight_si	= PULSE_FIGHT_SI;
        DEBUG_INFO ( "fight_si" );
        fight_ai_update ();
        DEBUG_INFO ( "memdat" );
        memdat_update();
        DEBUG_INFO ( NULL );
		DEBUG_INFO( "mspell update" );
		mspell_update();
    }

    DEBUG_INFO ( "event_update" );
    event_update ();
    DEBUG_INFO ( "aggr_update" );
    aggr_update ();
    DEBUG_INFO ( NULL );
    return ;
}

// Tym, którzy tego chc±, wysy³amy informacjê o statystykach graczy w li¶cie
void extern_si_update()
{
	#define ADD_NEWLINE( buf ) strcat( (buf), "\n\r" )
	DEBUG_INFO("extern_si_update");
	CHAR_DATA *ch;
	CHAR_DATA *ch_next;
	int i;
	char buf[MAX_STRING_LENGTH];
	char bufik[MAX_STRING_LENGTH];
	for ( ch = char_list; ch != NULL; ch = ch_next )
	{

		ch_next = ch->next;

		CHAR_LST *chlst;
		CHAR_LST *chlst_next;
		// interesuj± nas:
		// imiê
		// hp, hpmax
		// mv, mvmax
		// skille/spelle
		if ( ch->si_followed )
		{
			sprintf( buf, "SIFOLLOW\n\r%d", ch->si_followed_cnt );
			ADD_NEWLINE( buf );
			send_to_char( buf, ch );
		}
		for( chlst = ch->si_followed ; chlst != NULL; chlst = chlst_next )
		{
			if ( chlst->ch == NULL )
			{
				bug( "[extern_si_update]:NULL chlst->ch!\n\r", 1 );
				return;
			}
			if ( chlst->ch->name == NULL )
			{
				bug ( "[extern_si_update]:chlst->ch->name == NULL", 1);
				return;
			}

			chlst_next = chlst->next;
			sprintf( buf, "%s\n\r%d\n\r%d\n\r%d\n\r%d\n\r", chlst->ch->name, chlst->ch->hit,
				get_max_hp(chlst->ch), chlst->ch->move, chlst->ch->max_move );
			send_to_char( buf, ch );

			DEBUG_INFO("extern_si_update: zliczanie spelli");
			MSPELL_DATA * tmp;
			int memcount[ MAX_SKILL ];
			int j;
			for( j=0; j<MAX_SKILL; j++)
				memcount[ j ] = 0;

			for ( tmp = chlst->ch->memspell; tmp != NULL; tmp = tmp->next )
			{
				if ( tmp->done )
					memcount[ tmp->spell ] += 1;
			}

			DEBUG_INFO("extern_si_update: skill/spell stuff");
			sprintf( buf, "%d", MAX_SKILL );
			ADD_NEWLINE( buf );
			for(i=0;i < MAX_SKILL; i++)
			{
				int val=0;
				if ( skill_table[ i ].spell_fun == NULL ||
					 skill_table[ i ].spell_fun == spell_null ) // skill
				{
					val = get_skill( chlst->ch, i ) <= 0 ? 0 : 1;
				}
				else // spell
				{
					if ( knows_spell( chlst->ch, i ) )
						val = memcount[ i ];
					if (   ( (i == gsn_create_symbol) && is_affected( chlst->ch, gsn_create_symbol ))
						|| ( (i == gsn_holy_weapons) && is_affected( chlst->ch, gsn_holy_weapons ))
						|| ( (i == gsn_energy_strike) && is_affected( chlst->ch, gsn_energy_strike ))
						|| ( (i == gsn_shillelagh) && is_affected( chlst->ch, gsn_shillelagh )))
						val = 0;
				}
				sprintf( bufik, "%d", val );
				strcat( buf, bufik);
				ADD_NEWLINE( buf );
			}
			send_to_char( buf, ch );
		}
	}
}

bool check_nature_curse_aggressive	( CHAR_DATA *ch, CHAR_DATA *wch )
{
	if ( !is_affected(wch, gsn_nature_curse ))
		return FALSE;

	if( ch->in_room->sector_type != 3 &&//las
		ch->in_room->sector_type != 11 &&//puszcza
		ch->in_room->sector_type != 12 &&//bagno
		ch->in_room->sector_type != 30 &&//eden
		ch->in_room->sector_type != 38 )//park
		return FALSE;

	if ( IS_SET( race_table[ ch->race ].type, ANIMAL ))
		return TRUE;

	if ( IS_SET( race_table[ ch->race ].type, INSECT ))
		return TRUE;

	return FALSE;
}

void regen_update ( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
	int hp_gain, i;
	bool bonus, bonus_used;

    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next;
		bonus = FALSE;
		bonus_used = FALSE;
		//wklejam tutaj sprawdzanie adamantytowych przedmiotow, zeby bylo czesciej niz raz na tick - Raszer
		turn_into_dust_objects_sensitive_to_light (ch, 2);
		/* Szansa na ma³y bonus regeneracji dla druidów */
		if ( ( ch->class == CLASS_DRUID ) && IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_DRUID ))
		{
			if ( number_range(0,1000) < ( ch->level + (ch->level / 31)*10) )
			{
				bonus = TRUE;
			}
		}



        /* jesli w progu koles zginal/nie jest w roomie zadnym */
        if ( !ch || !ch->in_room )
            continue;

			if( ch->position > POS_INCAP )
			{
		      	if ( ch->hit < get_max_hp(ch) )
				{
					hp_gain = hit_gain ( ch );
					if ( bonus )
					{
					        hp_gain = UMAX( 5, dice( hp_gain, 3) );
						bonus_used = TRUE;
					}
					ch->hit += hp_gain;
					if ( ch->hit > get_max_hp(ch) )
						ch->hit = get_max_hp(ch);
					/* decrease damage counters */
					for ( i = 0; i <= 2; i++ )
					{
						if ( hp_gain <= ch->counter[i] )
						{
							ch->counter[i] -= hp_gain;
							hp_gain = 0;
						}
						else
						{
							hp_gain -= ch->counter[i];
							ch->counter[i] = 0;
						}
					}
				}

		else
		{
			ch->hit = get_max_hp(ch);
			/* zero damage counters */
			for ( i = 0; i <= 2; i++ )
				ch->counter[i] = 0;
		}

		if( ch->hit == get_max_hp(ch) && is_affected( ch, gsn_bleeding_wound ) )
		{
			if ( skill_table[ gsn_bleeding_wound ].msg_off )
			{
				send_to_char( skill_table[ gsn_bleeding_wound ].msg_off, ch );
				send_to_char( "\n\r", ch );
			}
			affect_strip( ch, gsn_bleeding_wound );
		}

		if ( ch->hit == get_max_hp(ch) && is_affected ( ch, gsn_bandage ) )
		{
			affect_strip ( ch, gsn_bandage );
			if ( ch->position == POS_SLEEPING )
			{
				act ( "Przez niespokojny sen $n zrywa z siebie banda¿e.", ch, NULL, NULL, TO_ROOM );
			}
			else
			{
				send_to_char ( "Zrywasz banda¿e.\n\r", ch );
				act ( "$n zrywa z siebie banda¿e.", ch, NULL, NULL, TO_ROOM );
				if ( IS_AFFECTED( ch, AFF_HIDE ) ) affect_strip( ch, gsn_hide );
			}
		}

		if ( ch->move < ch->max_move )
		{
			int mv_gain = move_gain( ch );
			if ( bonus )
			{
				mv_gain = dice( mv_gain, 3);
				bonus_used = TRUE;
			}
			ch->move += mv_gain;
		}
		/* komunikat o zwiekszonej regeneracji dla druida */
		if ( bonus_used && !is_undead(ch) )
		{
			send_to_char("Czujesz na sobie czu³y dotyk {GNatury{x.\n\r", ch );
		}

		if ( ch->move > ch->max_move )
		{
			ch->move = ch->max_move;
		}

		/* if ( !IS_NPC(ch) && is_undead(ch) )
			ch->move = ch->max_move; - Raszer: ju¿ undeadom sie nie maximuje mv*/

		if (ch->move < 0)
		{
			ch->move = 0;
		}

            	if ( ch->hit == get_max_hp(ch) && ch->move == ch->max_move &&
                 	IS_AFFECTED ( ch, AFF_RECUPERATE ) )
                	affect_strip ( ch, gsn_recuperate );
			}
			else if ( ch->position == POS_INCAP )
			{
				if( is_affected(ch,gsn_bleeding_wound) )
				{
					if ( dice(1,4) == 1 ) --ch->hit;
				}
				else
				{
					if ( dice(1,14) == 1 ) --ch->hit;
				}
			}
			else if ( ch->position == POS_MORTAL )
			{
				if( is_affected(ch,gsn_bleeding_wound) )
				{
					if ( dice(1,2) == 1 ) --ch->hit;
				}
				else
				{
					if ( dice(1,8) == 1 ) --ch->hit;
				}
			}
			else if ( ch->position == POS_DEAD )
			{
				raw_kill(ch,ch);
				continue;
			}

            if ( IS_AFFECTED( ch, AFF_HEALING_TOUCH ))
                check_healing_touch( ch );

			if ( IS_AFFECTED(ch, AFF_SLEEP) && ch->position != POS_SLEEPING )
				affect_strip( ch, gsn_sleep );

			check_acid_arrow( ch );
			check_burn( ch);
			check_damned_stuff( ch );
			check_shadow_swarm( ch );
        	update_pos(ch);

            /* Tener: przeniesienie check_improve skilli pasywnych tutaj. Odpowiednio dostosowane szanse na wej¶cie [20080513] */
            if ( is_affected( ch, gsn_meditation ) && IS_AFFECTED( ch, AFF_MEDITATION ) )
            {
                check_improve( ch, NULL, gsn_meditation, TRUE, 80 );
            }
            if ( IS_AFFECTED ( ch, AFF_RECUPERATE ) )
            {
                if ( number_range( 0, 1 ) == 0 ) // Aby szansa by³a mniejsza ni¿ przy multiplier = 1
                {
                    check_improve( ch, NULL, gsn_recuperate, TRUE, 75 );
                }
            }
            if ( IS_AFFECTED( ch, AFF_HIDE ) )
            {
                check_improve(ch, NULL, gsn_hide, TRUE, 80 );
            }
    }

    return ;
}
void check_healing_touch ( CHAR_DATA *ch )
{
    CHAR_DATA *vch = 0, *vch_next = 0;
    AFFECT_DATA *aff_ch = 0, *aff_vch = 0;
    int hp_gain, mod = 1;

    aff_ch = affect_find( ch->affected, gsn_healing_touch );
    if ( aff_ch->modifier > 10000 )
    {
        //find caster (vch)
        for ( vch = ch->in_room->people;vch;vch = vch_next )
        {
            vch_next = vch->next_in_room;
            if ( vch != ch && IS_AFFECTED( vch, AFF_HEALING_TOUCH ))
            {
                aff_vch = affect_find( vch->affected, gsn_healing_touch );
                if ( aff_vch->modifier == (aff_ch->modifier - 10000) )
                    break;
                else
                    aff_vch = NULL;
            }
        }

        if ( aff_vch == NULL )
        {
            affect_strip( ch, gsn_healing_touch );
            print_char( ch, "Lecznicze ciep³o opuszcza twoje cia³o.\n\r" );
            return;
        }

		if ( ch->position == POS_FIGHTING || ch->fighting != NULL )
        {
            EXT_REMOVE_BIT( vch->affected_by, AFF_HEALING_TOUCH );
            affect_strip( ch, gsn_healing_touch );
            return;
        }

        if ( (get_skill( vch, gsn_healing_touch ) + 400) < ( number_range(0, 500) ) )
        {
            act( "$N najwyrazniej myli s³owa modlitwy i przerywa inkantacje.", ch, NULL, vch, TO_CHAR);
            act( "Myl± ci siê s³owa modlitwy i przerywasz inkantacje.", ch, NULL, vch, TO_VICT );
            act( "$N najwyrazniej myli s³owa modlitwy i przerywa inkantacje.", ch, NULL, vch, TO_NOTVICT );
            EXT_REMOVE_BIT( vch->affected_by, AFF_HEALING_TOUCH );
            affect_strip( ch, gsn_healing_touch );
            return;
        }

        hp_gain = number_range(0.9*aff_ch->level, 1.1*aff_ch->level);

        ch->hit = UMIN( ch->hit + hp_gain, get_max_hp(ch) );
        update_pos( ch );

        if (number_range(0, vch->level) > 20)  mod--;
        if (number_range(0, get_curr_stat_deprecated(vch, STAT_LUC)) < 3) mod++;

        aff_ch->level   -= mod;
        aff_vch->level  -= mod;

        if ( aff_ch->level == 0 )
        {
            act( "$N z wyczerpania przerywa modlitwe i odsuwa d³onie od twojego cia³a.", ch, NULL, vch, TO_CHAR);
            act( "Z wyczerpania przerywasz i odsuwasz d³onie od cia³a $z.", ch, NULL, vch, TO_VICT );
            act( "$N przerywa z wyczerpania modlitwe i odsuwa d³onie od cia³a $z.", ch, NULL, vch, TO_NOTVICT );
            EXT_REMOVE_BIT( vch->affected_by, AFF_HEALING_TOUCH );
            affect_strip( ch, gsn_healing_touch );
        }
        else if ( ch->hit == get_max_hp(ch))
        {
            act( "$N koñczy modlitwe i odsuwa d³onie od twojego cia³a.", ch, NULL, vch, TO_CHAR);
            act( "Koñczysz modlitwe i odsuwasz d³onie od cia³a $z.", ch, NULL, vch, TO_VICT );
            act( "$N koñczy modlitwe i odsuwa d³onie od cia³a $z.", ch, NULL, vch, TO_NOTVICT );
            EXT_REMOVE_BIT( vch->affected_by, AFF_HEALING_TOUCH );
            affect_strip( ch, gsn_healing_touch );
        }
        else
        {
            act( "Ciep³o bij±ce z d³oni $Z wype³nia twoje cia³o.", ch, NULL, vch, TO_CHAR);
            act( "Lecznicze ciep³o przep³ywa z twych d³oni do cia³a $z.", ch, NULL, vch, TO_VICT );
            act( "Koj±ce ciep³o rozchodz±ce siê z d³oni $Z wype³nia cia³o $z.", ch, NULL, vch, TO_NOTVICT );
        }
        // Tener: wprowadzenie check improve w trakcie dzia³ania skilla [20080513]
        check_improve( ch, NULL, gsn_healing_touch, TRUE, 80 );
    }
    else
    {
        //find target (vch)
        for ( vch = ch->in_room->people;vch;vch = vch_next )
        {
            vch_next = vch->next_in_room;
            if ( vch != ch && IS_AFFECTED( vch, AFF_HEALING_TOUCH ))
            {
                aff_vch = affect_find( vch->affected, gsn_healing_touch );
                if ( aff_vch->modifier == (aff_ch->modifier + 10000) )
                    break;
                else
                    aff_vch = NULL;
            }
        }

        if ( aff_vch == NULL )
        {
            EXT_REMOVE_BIT( ch->affected_by, AFF_HEALING_TOUCH );
            print_char( ch, "Przerywasz modlitwê.\n\r" );
            return;
        }
    }
    return;

}
void check_acid_arrow ( CHAR_DATA *ch )
{
	AFFECT_DATA *arrow;
	int dam;

	if( !is_affected(ch, gsn_acid_arrow ))
		return;

	arrow = affect_find( ch->affected, gsn_acid_arrow );

	dam = UMIN( arrow->modifier, arrow->level);

	spell_damage( ch, ch, dam, gsn_acid_arrow, DAM_ACID, FALSE );

	send_to_char( "Auæ! Kwas p³ywa w twoich ¿y³ach!\n\r", ch );
	act( "Na twarzy $z widzisz grymas bólu.", ch, NULL, NULL, TO_ROOM );

	arrow->modifier -= dam;
	arrow->level /= 2;

	if( arrow->level < 1 || arrow->modifier < 1 )
		affect_strip( ch, gsn_acid_arrow );

	return;
}

//gsn_burn jako dodatek do czaru dla evokera flame_lace

void check_burn ( CHAR_DATA *ch )
{
	AFFECT_DATA *burn;
	int dam;

	if( !is_affected(ch, gsn_burn ))
		return;

	burn = affect_find( ch->affected, gsn_burn );

	dam = UMIN( burn->modifier, burn->level);

	spell_damage( ch, ch, dam, gsn_burn, DAM_FIRE, FALSE );

	send_to_char( "Ogniste liny oplataj±ce ciê rozb³yskuj± ¿arem.", ch );
	act( "Ogniste liny oplataj±ce $z rozb³yskuj± ¿arem.", ch, NULL, NULL, TO_ROOM );

	burn->modifier -= dam;
	burn->level /= 2;

	if( burn->level < 1 || burn->modifier < 1 )
		affect_strip( ch, gsn_burn );

	return;
}

void check_damned_stuff ( CHAR_DATA *ch )
{
	OBJ_DATA *weapon, *armor;
	AFFECT_DATA *damnation;
	int dam = 0;

	weapon = get_eq_char( ch, WEAR_WIELD );
	armor = get_eq_char( ch, WEAR_BODY );

	if ( weapon && number_range( 1, 25 ) == 1 )
		for ( damnation = weapon->affected; damnation != NULL; damnation = damnation->next )
			if ( damnation->type == gsn_damn_weapon )
			{
				dam = dice( 3, damnation->level/3 );
				if (dam>0)
				{
					act( "Przeklêta dusza wype³niaj±ca $h wysysa z ciebie ¿ycie!", ch, weapon, NULL, TO_CHAR );
					act( "Przeklêta dusza wype³niaj±ca $h wysysa ¿ycie z $z.", ch, weapon, NULL, TO_ROOM );
					spell_damage( ch, ch, dam, gsn_damn_weapon, DAM_NEGATIVE, FALSE );
				}
				break;
			}

	if ( armor && number_range( 1, 25 ) == 1 )
		for ( damnation = armor->affected; damnation != NULL; damnation = damnation->next )
			if ( damnation->type == gsn_damn_armor )
			{
				dam = dice( 3, damnation->level/3 );
				if (dam>0)
				{
					act( "Przeklêta dusza wype³niaj±ca $h wysysa z ciebie ¿ycie!", ch, armor, NULL, TO_CHAR );
					act( "Przeklêta dusza wype³niaj±ca $h wysysa ¿ycie z $z.", ch, armor, NULL, TO_ROOM );
					spell_damage( ch, ch, dam, gsn_damn_armor, DAM_NEGATIVE, FALSE );
				}
				break;
			}

	return;
}

bool undead_resemblance( CHAR_DATA *ch, CHAR_DATA *victim )
{
	int skill, value;
	AFFECT_DATA af;
	//true - nie atakuje
	if ( !is_undead(victim) || victim->level > 27 )
		return FALSE;
	if ( is_undead( ch ) )
		return TRUE;
	if ( ( skill = get_skill(ch,gsn_undead_resemblance )) <= 0 )
		return FALSE;
	if ( !( (ch->class == CLASS_MAG && ch->pcdata->mage_specialist == 7 ) || ch->class == CLASS_BLACK_KNIGHT ) )
		return FALSE;
	if ( is_affected(ch, gsn_undead_resemblance ))
		return TRUE;

	value = ch->level - victim->level;
	value *= 4;
	skill += value;
	value = number_range( 0, 18 - get_curr_stat_deprecated(victim,STAT_INT));
	skill += value;
	value = number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC)/3);
	skill += value;
	value = (get_age(ch) - 25);
	skill += value;

	skill = URANGE( 0, skill, 95 );

	if( number_percent() < skill )
	{
		check_improve( ch, victim, gsn_undead_resemblance, TRUE, 20 );
		af.where = TO_AFFECTS;
		af.type = gsn_undead_resemblance;
		af.level = 1;
		af.duration = 1;
		af.rt_duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &AFF_NONE;
		af.visible = FALSE;
		affect_to_char(	ch,	&af, NULL, FALSE );
		return TRUE;
	}
	else
	{
		check_improve( ch, victim, gsn_undead_resemblance, FALSE, 40 );
		return FALSE;
	}

	return FALSE;
}

void check_dehydration( CHAR_DATA *ch )
{
	AFFECT_DATA aff, *pAff;
	int level;

	if(!ch)
		return;

	//je¶li koñ ma je¼d¼ca, to siê mêczy, chce mu siê piæ i je¶æ
	//w celu uproszczenia nie mêczy siê, je¶li nikt go nie dosiada
	if ( IS_NPC ( ch ) && !ch->mounting)
		return ;

	//jesli koles jest glodny i nie ma flagi, dajemy i out
	if ( ch->condition[ COND_THIRST ] == 0 && !is_affected ( ch, gsn_dehydration ) )
	{
		aff.where	= TO_AFFECTS;
		aff.type	= gsn_dehydration;
		aff.level	= 0;
		aff.duration	= -1;
		aff.rt_duration = 0;
		aff.bitvector = &AFF_NONE;
		aff.location	= APPLY_NONE;
		aff.modifier	= 0;
		affect_to_char ( ch, &aff, NULL, FALSE );
		return ;
	}

	//jesli nie ma flagi out
	if ( !is_affected ( ch, gsn_dehydration ) )
	{
		return ;
	}

	//jesli jakims cudem nie znalazlo affecta, out
	if ( ( pAff = affect_find ( ch->affected, gsn_dehydration ) ) == NULL )
	{
		return ;
	}


	if ( ch->condition[ COND_THIRST ] == 0 )
	{
		pAff->level++;
	}
	else
	{
		pAff->level -= 3;
	}

	//zadnych nowych affectow nie dajemy
	if ( pAff->level < 0 )
	{
		affect_strip ( ch, gsn_dehydration );
		return ;
	}

	level = pAff->level;

	//zdejmujemy obecny affect
	affect_strip ( ch, gsn_dehydration );

	aff.where	= TO_AFFECTS;
	aff.type	= gsn_dehydration;
	aff.level	= level;
	aff.duration	= -1;
	aff.rt_duration = 0;
	aff.bitvector = &AFF_NONE;
	aff.location	= APPLY_CON;
	aff.modifier = UMIN(0,1-level/6);

	affect_to_char ( ch, &aff, NULL, TRUE );
	return ;
}

void check_shadow_swarm( CHAR_DATA *ch )
{
	CHAR_DATA *shadow;
	AFFECT_DATA af;

	if( !is_affected( ch, gsn_shadow_swarm ) )
		return;

	if( number_percent() > affect_find(ch->affected,gsn_shadow_swarm)->level )
		return;

	if( number_range(0,2000) < get_curr_stat_deprecated(ch,STAT_INT) + get_curr_stat_deprecated(ch,STAT_WIS) )
	{
		act( "Udaje ci siê silnym wysi³kiem woli i umys³u uwolniæ od nawiedzaj±cych ciê cienistych iluzji.", ch, NULL, NULL, TO_CHAR );
		affect_strip(ch,gsn_shadow_swarm);
		return;
	}

	switch( number_range(1,12) )
	{
		case 1:
			shadow = create_mobile( get_mob_index( MOB_VNUM_SHADOW_MSCICIEL ) );
			break;
		case 2:
			shadow = create_mobile( get_mob_index( MOB_VNUM_SHADOW_NIETOPERZ ) );
			break;
		case 3:
			shadow = create_mobile( get_mob_index( MOB_VNUM_SHADOW_JEZDZIEC ) );
			break;
		case 4:
			shadow = create_mobile( get_mob_index( MOB_VNUM_SHADOW_WAZ ) );
			break;
		case 5:
			shadow = create_mobile( get_mob_index( MOB_VNUM_SHADOW_ZJAWA ) );
			break;
		case 6:
			shadow = create_mobile( get_mob_index( MOB_VNUM_SHADOW_IMP ) );
			break;
		case 7:
			shadow = create_mobile( get_mob_index( MOB_VNUM_SHADOW_PAJAK ) );
			break;
		case 8:
			shadow = create_mobile( get_mob_index( MOB_VNUM_SHADOW_OGAR ) );
			break;
        default:
		case 9:
			shadow = create_mobile( get_mob_index( MOB_VNUM_SHADOW_KRUK ) );
			break;
		case 10:
			shadow = create_mobile( get_mob_index( MOB_VNUM_SHADOW_SKORPION ) );
			break;
		case 11:
			shadow = create_mobile( get_mob_index( MOB_VNUM_SHADOW_LEPRECHAUN ) );
			break;
		case 12:
			shadow = create_mobile( get_mob_index( MOB_VNUM_SHADOW_DZIN ) );
			break;
	}

	ch->counter[4] = 1;

	char_to_room( shadow, ch->in_room );
	EXT_SET_BIT( shadow->act, ACT_NO_EXP );
    money_reset_character_money( shadow );
	shadow->level = affect_find(ch->affected,gsn_shadow_swarm)->level - 4;
	shadow->max_hit = 1; // i tak sie rozplywa po jednym ciosie

	if ( HAS_TRIGGER( shadow, TRIG_ONLOAD ) )
	{
		shadow->position = POS_STANDING;
		mp_onload_trigger( shadow );
	}

	af.where = TO_AFFECTS;
	af.type = gsn_silence;
	af.level = 50;
	af.duration = -1;
	af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_ILLUSION;
	affect_to_char( shadow, &af, NULL, TRUE );

	af.bitvector = &AFF_SILENCE;
	affect_to_char( shadow, &af, NULL, TRUE );

	shadow->hit = get_max_hp(shadow);

	act("Z otaczaj±cych ciê cieni formuje siê $N.", ch, NULL, shadow, TO_CHAR );
	act("Z otaczaj±cych $c cieni formuje siê $N.", ch, NULL, shadow, TO_NOTVICT );

	shadow->hunting = ch;

	multi_hit( shadow, ch, TYPE_UNDEFINED );

	return;
}
