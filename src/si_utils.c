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
 * $Id: si_utils.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/si_utils.c $
 *
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

char * get_target_by_name(CHAR_DATA *ch, CHAR_DATA *victim)
{
    static char targetname[MAX_INPUT_LENGTH];
    char name[MAX_INPUT_LENGTH];
    CHAR_DATA * tch;
    int target=1;
    one_argument( victim->name, name );
    for(; target < 10; target++)
    {
        sprintf(targetname, "%d.%s", target, name );
        if(  (tch = get_char_room( ch, targetname )) == NULL )
        {
            break;
        }
        if( tch == victim)
        {
            return targetname;
        }
    }
    sprintf( targetname, "%s", victim->name );
    return targetname;
}

/**** test na inteligencje - badziewny ale wystarczy ****/
/* od 5 - 0, playerzy charmiesy losuja inta mastera */
int test_int(CHAR_DATA *ch)
{
int stat_int;

    if( !IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) && ch->master )
	stat_int = get_curr_stat_deprecated(ch->master, STAT_INT);
    else
	stat_int = get_curr_stat_deprecated(ch, STAT_INT);

    if( stat_int < 5)
	return INT_IDIOT;

    if( stat_int > 18 && number_range(18,28) < stat_int )
	return INT_GENIUS;

    if( stat_int > 15 && number_range(15,26) < stat_int )
	return INT_VERY_HIGH;

    if( stat_int > 12 && number_range(12,24) < stat_int )
	return INT_HIGH;

    if( stat_int > 9 && number_range(9,22) < stat_int )
	return INT_AVERAGE;

    if( stat_int > 7 && number_range(7,20) < stat_int )
	return INT_POOR;

    return INT_IDIOT;
}

void force_cast(CHAR_DATA *ch, CHAR_DATA *victim, sh_int sn)
{
static char buf[MAX_INPUT_LENGTH];
char * target;

    if(ch == victim)
    {
	sprintf(buf,"'%s' self",skill_table[sn].name);
	do_function(ch,&do_cast, buf );
        return;
    }

    if( (target = get_target_by_name(ch,victim)) == NULL)
    {
	sprintf(buf,"'%s' %s",skill_table[sn].name, victim ? victim->name : "");
	do_function(ch,&do_cast, buf );
    }
    else
    {
	sprintf(buf,"'%s' %s",skill_table[sn].name, target);
	do_function(ch,&do_cast, buf );
    }
    return;
}


/* czy ten mob sie nadaje do tankowania*/
int check_tank(CHAR_DATA *ch)
{
int average_hp, hp_percent;

    average_hp = get_max_hp(ch)/UMAX(1, ch->level);
    hp_percent = (100*ch->hit)/UMAX(1, get_max_hp(ch));

    /* jesli ma ponizej 30%hp */
    if( hp_percent < 30 )
	return 0;

    if(!IS_NPC(ch))
    {
        if( average_hp < 15)
	return 1;

	switch(average_hp)
	{
	    case 15:
	    case 16:
	    case 17:
		    return 2;
	    case 18:
    	    case 19:
	    case 20:
		    return 3;
	    case 21:
	    case 22:
	    case 23:
		    return 4;
	    default:
		    return 5;
	}
	return 0;
    }

    if( average_hp < 20)
	return 1;

    switch(average_hp)
    {
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
		    return 2;
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
		    return 3;
	case 30:
	case 31:
	case 32:
	case 33:
	case 34:
	case 35:
		    return 4;
	default:
		    return 5;
    }

    return 0;
}

/* sprawdzanie czy koles z grupki ma problem */
/* nie uzywane */
int need_help(CHAR_DATA *ch, CHAR_DATA *gch)
{
    int hp_percent, ret;

    hp_percent = (100*gch->hit)/get_max_hp(gch);

    if( hp_percent < 30 )
	ret = 5;
    else if( hp_percent < 45 )
	ret = 4;
    else if( hp_percent < 60 )
	ret = 2;
    else if( hp_percent < 75)
	ret = 1;
    else
	ret = 0;

    if(!can_move(gch) && ret > 0)
	ret++;

    return ret;
}


struct char_desc * create_desc(CHAR_DATA *ch, int caster, int tank, int hp_percent, int active)
{
    struct char_desc * desc;

    desc = malloc( sizeof(*desc) );

    desc->ch		= ch;
    desc->caster	= caster;
    desc->tank		= tank;
    desc->hp_percent	= hp_percent;
    desc->active	= active;
    desc->next		= NULL;
    return desc;
}


struct char_desc * copy_desc(struct char_desc *element)
{
    struct char_desc * desc;

    desc = malloc( sizeof(*desc) );

    desc->ch		= element->ch;
    desc->caster	= element->caster;
    desc->tank		= element->tank;
    desc->hp_percent	= element->hp_percent;
    desc->active	= element->active;
    desc->next		= NULL;
    return desc;
}

/* wstawianie na poczatek w jednokierunkowej liscie */
void add_desc_list(struct char_desc **list, struct char_desc *element)
{
    element->next = *list;
    *list = element;
    return;
}

void dispose_list(struct char_desc **list)
{
    struct char_desc *el, *tmp;

    for( el = *list; el;)
    {
	tmp = el;
	el = el->next;
	free(tmp);
    }

    *list = NULL;
    return;
}


/* tablica self_checkow  -  tylko tych na wykonujacego akcje*/

struct self_check self_checks_table[MAX_CHECKS] =
{
    { is_caster,	-1 },
    { can_cast,		-1 },
    { got_any_frags,-1 },
    { knows_rescue,	-1 },
    { knows_berserk,-1 },

    { knows_bash,	-1 },
    { knows_charge,	-1 },
    { can_charge,	-1 },
    { knows_any_skills,	-1 },
    { can_do_some_magic,-1 },

    { got_any_antitanks,-1 },
    { can_heal,		-1 },
    { got_anticasters,	-1 },
    { is_tank,		-1 },
    { can_tank,		-1 },

    { can_rescue,	-1 },
    { can_berserk,	-1 },
    { got_powerups_self,-1 },
    { not_fighting,	-1 },
    { fighting,		-1 },

    { is_down,		-1 },
    { is_defending,	-1 },
    { got_stone_skin,	-1 },
    { standing,		-1 },
    { knows_backstab,	-1 },

    { can_backstab,	-1 },
    { knows_wardance,	-1 },
    { can_wardance,	-1 },
    { wardancing,	-1 },
    { disarmed,		-1 },

    { can_target_master,-1 },
    { knows_damage_reduction, -1 },
    { can_do_damage_reduction, -1 },
    { got_any_holds,	-1 },
    { need_memming,	-1 },

    { is_resting,	-1 },
    { is_boring,	-1 },
    { can_lay, 		-1 },
    { is_dark_here,	-1 },
    { is_wounded,	-1 },

    { shopping,		-1 },
    { should_lay,	-1 },
    { can_summon,	-1 },
    { should_summon,-1 },
    { can_teleport, -1 },

    { should_teleport,		-1 },
    { default_pos_resting,	-1 },
    { is_sleeping_normal,	-1 },
    { default_pos_sleeping,	-1 },
    { can_call_friend,		-1 },

    { NULL, 		 0  }
};



/* wszystkie checki ktore sa wykonywane na mobie ktory
 * wybiera jakas akcje sa wykonywane raz przed analiza akcji
 * pozniej wyniki checkow sa pobierane z tablicy
 */
void optimize_self_checks( struct char_desc *ch_desc )
{
int x;

    for( x=0; self_checks_table[x].fun; x++)
	self_checks_table[x].value = ((*self_checks_table[x].fun)(ch_desc, ch_desc));

    /* coby wartosci value byly zawsze albo FALSE (0) albo TRUE (1),
     * poprzedni for moglby zwrocic jakas wartosc rozna od 1 w przypadku TRUE */
    for( x=0; self_checks_table[x].fun; x++)
    {
    	if ( self_checks_table[x].value != FALSE )
    	    self_checks_table[x].value = TRUE;
    }
    return;
}

