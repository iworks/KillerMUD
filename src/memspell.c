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
 * KILLER MUD is copyright 1999-2013 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: memspell.c 12203 2013-03-29 13:35:28Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/memspell.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"
#include "magic.h"

void mem_done              ( CHAR_DATA *ch );
sh_int spell_circle        ( CHAR_DATA *ch, sh_int spell );
sh_int get_mspell_duration ( CHAR_DATA *ch, MSPELL_DATA * mspell );
void do_memorize           ( CHAR_DATA *ch, char *argument );
bool remove_mem            ( CHAR_DATA *ch, MSPELL_DATA * mspell );
bool add_mem               ( CHAR_DATA *ch, sh_int spell );
void load_mem              ( CHAR_DATA *ch, MSPELL_DATA *mem );
bool can_mem               ( CHAR_DATA *ch, sh_int circle );
bool spell_memmed          ( CHAR_DATA *ch, sh_int spell );
void mem_show              ( CHAR_DATA *ch );
int get_mem_slots          ( CHAR_DATA *ch, int circle, bool free );
sh_int get_caster          ( CHAR_DATA *ch );
int chance_to_learn_spell  ( CHAR_DATA *ch, sh_int spell );
void mem_update_count      ( CHAR_DATA *ch );

void    save_memset        ( CHAR_DATA *ch, char *memset_name, bool overwrite_old );
void    load_memset        ( CHAR_DATA *ch, char *memset_name );
bool    remove_memset      ( CHAR_DATA *ch, char *memset_name, bool quiet );
void    list_memsets       ( CHAR_DATA *ch );


sh_int mem_numb_mag[ 32 ][ 10 ] =
    {
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},    /*  1  1*/
        {2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {3, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {4, 1, 0, 0, 0, 0, 0, 0, 0, 0},    /*  4  2*/
        {4, 2, 0, 0, 0, 0, 0, 0, 0, 0},
        {4, 3, 0, 0, 0, 0, 0, 0, 0, 0},
        {4, 4, 1, 0, 0, 0, 0, 0, 0, 0},    /*  7  3*/
        {4, 4, 2, 0, 0, 0, 0, 0, 0, 0},
        {4, 4, 3, 0, 0, 0, 0, 0, 0, 0},
        {4, 4, 4, 1, 0, 0, 0, 0, 0, 0},    /* 10  4*/
        {4, 4, 4, 2, 0, 0, 0, 0, 0, 0},
        {4, 4, 4, 3, 0, 0, 0, 0, 0, 0},
        {5, 4, 4, 4, 1, 0, 0, 0, 0, 0},    /* 13  5*/
        {5, 4, 4, 4, 2, 0, 0, 0, 0, 0},
        {5, 4, 4, 4, 3, 0, 0, 0, 0, 0},
        {5, 5, 4, 4, 4, 1, 0, 0, 0, 0},    /* 16  6*/
        {5, 5, 4, 4, 4, 2, 0, 0, 0, 0},
        {5, 5, 4, 4, 4, 3, 0, 0, 0, 0},
        {5, 5, 5, 4, 4, 4, 1, 0, 0, 0},    /* 19  7*/
        {5, 5, 5, 4, 4, 4, 2, 0, 0, 0},
        {5, 5, 5, 4, 4, 4, 3, 0, 0, 0},
        {6, 5, 5, 5, 4, 4, 4, 1, 0, 0},    /* 22  8*/
        {6, 5, 5, 5, 4, 4, 4, 2, 0, 0},
        {6, 5, 5, 5, 4, 4, 4, 3, 0, 0},
        {6, 6, 5, 5, 5, 4, 4, 4, 1, 0},    /* 25  9*/
        {6, 6, 5, 5, 5, 4, 4, 4, 2, 0},
        {6, 6, 5, 5, 5, 4, 4, 4, 3, 0},
        {6, 6, 5, 5, 5, 5, 4, 4, 4, 0},    /* 28  10*/
        {6, 6, 5, 5, 5, 5, 5, 4, 4, 0},
        {6, 6, 5, 5, 5, 5, 5, 5, 4, 0},
        {6, 6, 5, 5, 5, 5, 5, 5, 5, 0},
        {8, 8, 8, 7, 7, 7, 7, 6, 6, 0}//moby z levelem 32+
    };

/* dla cle krag co 4 levele */

sh_int mem_numb_cle[ 32 ][ 8 ] =
    {
        {3, 0, 0, 0, 0, 0, 0, 0},    /*  1   */
        {3, 0, 0, 0, 0, 0, 0, 0},
        {3, 0, 0, 0, 0, 0, 0, 0},
        {4, 3, 0, 0, 0, 0, 0, 0},    /*  4  */
        {4, 3, 0, 0, 0, 0, 0, 0},
        {4, 3, 0, 0, 0, 0, 0, 0},
        {4, 3, 0, 0, 0, 0, 0, 0},
        {4, 4, 3, 0, 0, 0, 0, 0},    /*  8  */
        {4, 4, 3, 0, 0, 0, 0, 0},
        {4, 4, 3, 0, 0, 0, 0, 0},
        {4, 4, 3, 0, 0, 0, 0, 0},
        {5, 4, 4, 3, 0, 0, 0, 0},    /* 12  */
        {5, 4, 4, 3, 0, 0, 0, 0},
        {5, 4, 4, 3, 0, 0, 0, 0},
        {5, 4, 4, 3, 0, 0, 0, 0},
        {5, 5, 4, 4, 3, 0, 0, 0},    /* 16  */
        {5, 5, 4, 4, 3, 0, 0, 0},
        {5, 5, 4, 4, 3, 0, 0, 0},
        {5, 5, 4, 4, 3, 0, 0, 0},
        {5, 5, 5, 4, 4, 3, 0, 0},    /* 20  */
        {5, 5, 5, 4, 4, 3, 0, 0},
        {5, 5, 5, 4, 4, 3, 0, 0},
        {5, 5, 5, 4, 4, 3, 0, 0},
        {5, 5, 5, 4, 4, 4, 3, 0},    /* 24  */
        {5, 5, 5, 5, 4, 4, 3, 0},
        {5, 5, 5, 5, 4, 4, 3, 0},
        {5, 5, 5, 5, 4, 4, 3, 0},
        {5, 5, 5, 5, 4, 4, 4, 3},    /* 28  */
        {5, 5, 5, 5, 5, 4, 4, 3},
        {5, 5, 5, 5, 5, 5, 4, 4},
        {5, 5, 5, 5, 5, 5, 5, 4},
        {8, 8, 8, 7, 7, 7, 6, 5}//moby z levelem 32+
    };

sh_int mem_numb_brd[ 28 ][ 5 ] =    //ma lvl -5
    {
        {1, 0, 0, 0, 0},    /*  5  1*/
        {2, 0, 0, 0, 0},
        {2, 0, 0, 0, 0},
        {3, 0, 0, 0, 0},
        {4, 0, 0, 0, 0},
        {4, 1, 0, 0, 0},    /* 10  2*/
        {4, 2, 0, 0, 0},
        {4, 2, 0, 0, 0},
        {4, 3, 0, 0, 0},
        {4, 4, 0, 0, 0},
        {4, 4, 1, 0, 0},    /* 15  3*/
        {4, 4, 2, 0, 0},
        {4, 4, 3, 0, 0},
        {4, 4, 4, 0, 0},
        {5, 4, 4, 0, 0},
        {5, 4, 4, 1, 0},    /*  20  4*/
        {5, 4, 4, 2, 0},
        {5, 4, 4, 3, 0},
        {5, 4, 4, 4, 0},
        {5, 5, 4, 4, 0},
        {5, 5, 4, 4, 1},    /*  25  5*/
        {5, 5, 4, 4, 2},
        {5, 5, 4, 4, 3},
        {5, 5, 4, 4, 4},
        {5, 5, 5, 4, 4},    /*  29  7*/
        {5, 5, 5, 5, 4},
        {5, 5, 5, 5, 5},  /* 31    */
        {8, 7, 7, 6, 6}//moby z levelem 32+
    };

sh_int mem_numb_sha[ 23 ][ 4 ] =    //ma lvl -5
    {
        {1, 0, 0, 0},    /* 10  1*/
        {2, 0, 0, 0},
        {3, 0, 0, 0},
        {4, 0, 0, 0},
        {4, 0, 0, 0},
        {4, 1, 0, 0},    /* 15  2*/
        {4, 2, 0, 0},
        {4, 3, 0, 0},
        {4, 4, 0, 0},
        {4, 4, 0, 0},
        {5, 4, 1, 0},    /*  20  3*/
        {5, 4, 2, 0},
        {5, 4, 3, 0},
        {5, 4, 4, 0},
        {5, 4, 4, 0},
        {6, 5, 4, 1},    /*  25  4*/
        {6, 5, 4, 2},
        {6, 5, 4, 3},
        {6, 5, 4, 4},
        {7, 6, 5, 4},    /*  29  */
        {7, 6, 5, 4},
        {7, 6, 5, 4},  /* 31    */
        {8, 7, 6, 5}//moby z levelem 32+
    };

void get_next_memming( CHAR_DATA * ch )
{
	MSPELL_DATA * pre = NULL, * post = NULL;
	extern int port;

	if ( !ch || !ch->memspell )
		return ;

	for ( pre = ch->memspell; pre && pre != ch->memming; pre = pre->next )
	{
		if ( !pre->done )
			break;
	}

	if ( pre == ch->memming )
		pre = NULL;

	if ( ch->memming )
	{
		for ( post = ch->memming->next; post; post = post->next )
		{
			if ( !post->done )
				break;
		}
	}

	if ( pre && pre->done )
		pre = NULL;
	if ( post && post->done )
		post = NULL;

	if ( post )
		ch->memming = post;
	else if ( pre )
		ch->memming = pre;
	else
		ch->memming = NULL;

	if ( ch->memming && ch->memming->done )
		ch->memming = NULL;

	if ( ch->memming )
	{
		ch->memming->duration = get_mspell_duration( ch, ch->memming );
		if ( ch->position == POS_RESTING )
			echo_mem( ch, MEM_NEW );
		if ( port == 3000 && can_mem_now( ch ) )
        {
			print_char( ch, "{CCzas zapamiêtywania obliczony na {G%d{xs.\n\r", ch->memming->duration );
        }
	}
	else
	{
		echo_mem( ch, MEM_STOP_ALL );
		if ( IS_AFFECTED( ch, AFF_MEDITATION ) )
		{
			print_char( ch, "Koñczysz medytacje.\n\r" );
			affect_strip( ch, gsn_meditation );
		}
	}

	mem_update_count( ch );

	return ;
}

void mem_update_count( CHAR_DATA *ch )
{
	MSPELL_DATA * mspell;
	int i;

	if ( !ch )
		return ;

	ch->count_memspell = 0;

	for ( i = 0; i < MAX_SKILL;i++)
	   ch->memspell_cache[ i ] = 0;

	for ( mspell = ch->memspell; mspell; mspell = mspell->next )
	{
		if ( !mspell->done )
			ch->count_memspell++;

		if ( mspell->done )
		   ch->memspell_cache[ mspell->spell ] += 1;

	}

	return ;
}

/*
 * ustawia aktualnie zapamietywany czar na zapamietany
 * ustawia kolejny czar z listy do zapamietania
 */
void mem_done( CHAR_DATA *ch )
{
	if ( !ch || !ch->memspell || !ch->memming || ch->memming->done )
	{
        return ;
    }

    if ( IS_AFFECTED(ch, AFF_MEDITATION) )
    {
        check_trick( ch, ch, SN_TRICK_ENLIGHTENMENT );
    }
    
	ch->memming->done = TRUE;
	mem_update_count( ch );
	echo_mem( ch, MEM_DONE );

	get_next_memming( ch );

	return ;
}

void mem_done_all( CHAR_DATA *ch )
{
	MSPELL_DATA * for_mspell;
	int count = 0;

	if ( !ch || !ch->memspell )
		return ;

	for ( for_mspell = ch->memspell; for_mspell; for_mspell = for_mspell->next )
	{
		if ( !for_mspell->done )
		{
			for_mspell->done = TRUE;
			count++;
		}
	}
	mem_update_count( ch );

	ch->memming = NULL;

	if ( count > 0 )
		echo_mem( ch, MEM_DONE_ALL );

	return ;
}

int count_mspell( CHAR_DATA * ch, bool all, bool done )
{
	MSPELL_DATA * for_mspell;
	int count = 0;

	if ( !ch || !ch->memspell )
		return 0;

	for ( for_mspell = ch->memspell; for_mspell; for_mspell = for_mspell->next )
	{
		if ( all || ( for_mspell->done == done ) )
			count++;
	}

	return count;
}

int count_mspells_by_sn( CHAR_DATA * ch, sh_int sn, bool all, bool done )
{
	MSPELL_DATA * for_mspell;
	int count = 0;

	if ( !ch || !ch->memspell )
		return 0;

	for ( for_mspell = ch->memspell; for_mspell; for_mspell = for_mspell->next )
	{
		if ( for_mspell->spell == sn && ( all || ( for_mspell->done == done ) ) )
			count++;
	}

	return count;
}

void check_max_spells_memmed( CHAR_DATA * ch )
{
	sh_int caster = -1;
	sh_int max_circles;
	sh_int circle;
	int slots;
	MSPELL_DATA *tmp, *tmp_next;
	bool found;

    if ( !ch || !ch->memspell || ch->in_room == NULL )
    {
        return;
    }

	caster = get_caster( ch );

	/* sprawdzamy ile kregow ma koles*/
	switch ( caster )
	{
		case 0:
			max_circles = UMIN( 9, ( ch->level - 1 ) / 3 + 1 );
			break;
		case 1:
		case 5:
			max_circles = UMIN( 7, ch->level / 4 + 1 );
			break;
		case 4:
			if ( ch->level < 13 )
            {
				return ;
            }
            max_circles = UMIN( 4, ( ch->level - 13 ) / 4 + 1 );
			break;
		case 8:
			if ( ch->level < 5 )
				return ;

			max_circles = UMIN( 7, ( ch->level - 5 ) / 4 + 1 );
			break;

		case 10:
         if ( ch-> level < 10)
            return;

         max_circles = UMIN (4, ( ch->level - 10) / 5 + 1 );
			break;

		default: return ;
	}

	/* lecimy z kregami po kolei, jesli na ktoryms jest ujemna
	 * ilosc miejsca to kasujemy pierwsze z brzegu zaklecie(zaklecia)
	 * bez wzgledu czy jest zamemowane czy nie*/

	for ( circle = 1; circle <= max_circles; circle++ )
	{
		slots = 0 - get_mem_slots( ch, circle, TRUE );

		while ( slots > 0 )
		{
			found = FALSE;
			for ( tmp = ch->memspell; tmp; tmp = tmp_next )
			{
				tmp_next = tmp->next;

				if ( tmp->circle == circle )
				{
					remove_mem( ch, tmp );
					slots--;
					found = TRUE;
					break;
				}
			}
			if ( !found )
				break;
		}
	}

	return ;
}

/*
 * oblicza w przyblizeniu ktory krag
 */
sh_int spell_circle( CHAR_DATA *ch, sh_int spell )
{
	sh_int val = -1;
	sh_int caster = -1;

	caster = get_caster( ch );

	if ( caster < 0 )
		return -1;

	switch ( caster )
	{
		case CLASS_MAG:
			val = skill_table[ spell ].skill_level[ 0 ];

			if ( val < 32 )
			{
				val = ( ( val - 1 ) / 3 + 1 );
				return UMIN( 9, val );
			}
			break;

		case CLASS_CLERIC:
			val = skill_table[ spell ].skill_level[ 1 ];

			if ( val < 32 )
			{
				val = ( val / 4 + 1 );
				return UMIN( 8, val );
			}
			break;

		case CLASS_PALADIN:
			if ( ch->level > 12 )
			{
				val = skill_table[ spell ].skill_level[ 4 ];

				if ( val < 32 )
				{
					val = UMAX( 0, val - 13 );
					val = ( val / 4 + 1 );
                    val = UMIN( 4, val );
                    return val;
				}
			}
			break;

		case CLASS_DRUID:
			val = skill_table[ spell ].skill_level[ 5 ];

			if ( val < 32 )
			{
				val = ( val / 4 + 1 );
				return UMIN( 8, val );
			}
			break;

		case CLASS_BARD:
			if ( ch->level > 4 )
			{
				val = skill_table[ spell ].skill_level[ 8 ];

				if ( val < 32 )
				{
					val = UMAX( 0, val - 5 );
					val = ( val / 5 + 1 );
					return UMIN( 5, val );
				}
			}
			break;

		case CLASS_SHAMAN:
			if ( ch->level > 9 )
			{
				val = skill_table[ spell ].skill_level[ 10 ];

				if ( val < 32 )
				{
					val = UMAX( 0, val - 10 );
					val = ( val / 5 + 1 );
					return UMIN( 4, val );
				}
			}
			break;

		default:
			break;
	}

	return val;
}

/*
 * zwraca ilosc minidelayow potrzebnych do zapamietania spella
 */
sh_int get_mspell_duration( CHAR_DATA *ch, MSPELL_DATA * mspell )
{
	sh_int val = 0;
	int caster;
	int memtime;
	int circle, mod_circ, lew, statbonus, wand_mod = 0;
	OBJ_DATA *wand;

	if ( !ch || !mspell )
		return val;

	caster = get_caster( ch );

	if ( caster < 0 )
		return val;

	val = 0;

	circle = mspell->circle;
	mod_circ = circle;
	lew = ch->level;

	switch ( caster )
	{
        case CLASS_PALADIN:
            //lew -= 12;
            //Raszer zmiany w czasie memowania paladyna. Zmniejszenie mod circle do +2
            //Drake: A ja zwiekszam do +3, moim palem na 21 memuje cure moderate'y w ciagu 10 sekund. Czyli +120 hp co 40 sekund.
            mod_circ += 3;
			break;
		case CLASS_BARD:
			//lew -= 3;
			mod_circ += 1;
			break;

      case CLASS_SHAMAN:
      mod_circ += 2;
      break;

		default: break;
	}

	switch ( mod_circ )
	{
		case 1:
			val = 20 - ( lew < 8 ? lew/2 : 3*lew/5 );
			break;
		case 2:
			val = 24 - ( lew <= 11 ? 3*lew/5 : 2*lew/3 );
			break;
		case 3:
			val = 30 - ( lew <= 14 ? 3*lew/4 : 4*lew/5 );
			break;
		case 4:
			val = 38 - lew;
			break;
		case 5:
			val = 42 - lew;
			break;
		case 6:
			val = 46 - lew;
			break;
		case 7:
			val = 48 - lew;
			break;
		case 8:
			val = 50 - lew;
			break;
		default:
			val = 55 - lew;
			break;
	}

	val = UMAX( val, 1 );

	switch ( caster )
	{
		case CLASS_MAG:
		case CLASS_BARD:
			statbonus = get_curr_stat_deprecated( ch, STAT_INT ) - 21;
			statbonus *= 7;
			if ( statbonus < 0 ) statbonus /= 3;
			val -= ( val * statbonus ) / 100;
			break;

		case CLASS_CLERIC:
		case CLASS_PALADIN:
		case CLASS_DRUID:
      case CLASS_SHAMAN:
			statbonus = get_curr_stat_deprecated( ch, STAT_WIS ) - 21;
			statbonus *= 7;
			if ( statbonus < 0 ) statbonus /= 3;
			val -= ( val * statbonus ) / 100;
			break;

		default:
			return 0;
	}


	if ( sector_table[ ch->in_room->sector_type ].mem_bonus < 0 )
	{
		val *= ( 1 - sector_table[ ch->in_room->sector_type ].mem_bonus );
	}
	else if ( sector_table[ ch->in_room->sector_type ].mem_bonus > 0 )
	{
		val -= ( val * 25 * sector_table[ ch->in_room->sector_type ].mem_bonus ) / 100;
	}

	if ( caster == CLASS_DRUID )
	{
		if ( sector_table[ ch->in_room->sector_type ].memdru_bonus < 0 )
		{
			val *= ( 1 - sector_table[ ch->in_room->sector_type ].memdru_bonus );
		}
		else if ( sector_table[ ch->in_room->sector_type ].memdru_bonus > 0 )
		{
			val -= ( val * 25 * sector_table[ ch->in_room->sector_type ].memdru_bonus ) / 100;
		}
	}

	if ( !IS_NPC( ch ) )
	{
		if ( ch->condition[ COND_HUNGER ] == 0 )
			val += val / 2;
		if ( ch->condition[ COND_THIRST ] == 0 )
			val += val / 2;
	}

	if ( IS_AFFECTED( ch, AFF_POISON ) )
		val *= 2;

	if ( IS_AFFECTED( ch, AFF_PLAGUE ) )
		val *= 2;

	if ( ch->hit < ( get_max_hp( ch ) / 4 ) )
		val *= 2;

	if ( ch->hit < ( get_max_hp( ch ) / 2 ) )
		val += val / 2;

	switch ( caster )
	{
		case CLASS_MAG:
			if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_MEMMAG ) )
				val = UMAX( 0, 2 * val / 3 );
			break;

		case CLASS_CLERIC:
		case CLASS_PALADIN:
			if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_MEMCLE ) )
				val = UMAX( 0, 2 * val / 3 );
			break;

		case CLASS_DRUID:
			if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_MEMDRU ) )
				val = UMAX( 0, 2 * val / 3 );
			break;
		case CLASS_BARD:
			if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_MEMBARD ) )
				val = UMAX( 0, 2 * val / 3 );
			break;
		case CLASS_SHAMAN:
			if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_MEMSHA ) )
				val = UMAX( 0, 2 * val / 3 );
			break;

		default:
			break;
	}

	//i rozdzki
	wand = get_eq_char( ch, WEAR_HOLD );
	if( !IS_NPC(ch) && wand != NULL && wand->item_type == ITEM_WAND )
	{
		if( wand->value[0] == WAND_MEM_VALUE )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					wand_mod += wand->value[1];
				else
					wand_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					wand_mod += wand->value[1];
				else
					wand_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					wand_mod += wand->value[1];
				else
					wand_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					wand_mod += wand->value[1];
				else
					wand_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					wand_mod += wand->value[1];
				else
					wand_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					wand_mod += wand->value[1];
				else
					wand_mod -= wand->value[1];
			}
			else
				wand_mod += wand->value[1];
		}
		else if( wand->value[2] == WAND_MEM_VALUE )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					wand_mod += wand->value[3];
				else
					wand_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					wand_mod += wand->value[3];
				else
					wand_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					wand_mod += wand->value[3];
				else
					wand_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					wand_mod += wand->value[3];
				else
					wand_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					wand_mod += wand->value[3];
				else
					wand_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					wand_mod += wand->value[3];
				else
					wand_mod -= wand->value[3];
			}
			else
				wand_mod += wand->value[3];
		}
		else if( wand->value[4] == WAND_MEM_VALUE )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					wand_mod += wand->value[5];
				else
					wand_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					wand_mod += wand->value[5];
				else
					wand_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					wand_mod += wand->value[5];
				else
					wand_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					wand_mod += wand->value[5];
				else
					wand_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					wand_mod += wand->value[5];
				else
					wand_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					wand_mod += wand->value[5];
				else
					wand_mod -= wand->value[5];
			}
			else
				wand_mod += wand->value[5];
		}
	}

	val += wand_mod;

	wand_mod = 0;
	wand = get_eq_char( ch, WEAR_HOLD );
	if( !IS_NPC(ch) && wand != NULL && wand->item_type == ITEM_WAND )
	{
		if( wand->value[0] == WAND_MEM_PERCENT )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					wand_mod += (val*wand->value[1])/100;
				else
					wand_mod -= (val*wand->value[1])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					wand_mod += (val*wand->value[1])/100;
				else
					wand_mod -= (val*wand->value[1])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					wand_mod += (val*wand->value[1])/100;
				else
					wand_mod -= (val*wand->value[1])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					wand_mod += (val*wand->value[1])/100;
				else
					wand_mod -= (val*wand->value[1])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					wand_mod += (val*wand->value[1])/100;
				else
					wand_mod -= (val*wand->value[1])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					wand_mod += (val*wand->value[1])/100;
				else
					wand_mod -= (val*wand->value[1])/100;
			}
			else
				wand_mod += (val*wand->value[1])/100;
		}
		else if( wand->value[2] == WAND_MEM_PERCENT )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					wand_mod += (val*wand->value[3])/100;
				else
					wand_mod -= (val*wand->value[3])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					wand_mod += (val*wand->value[3])/100;
				else
					wand_mod -= (val*wand->value[3])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					wand_mod += (val*wand->value[3])/100;
				else
					wand_mod -= (val*wand->value[3])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					wand_mod +=(val*wand->value[3])/100;
				else
					wand_mod -= (val*wand->value[3])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					wand_mod += (val*wand->value[3])/100;
				else
					wand_mod -= (val*wand->value[3])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					wand_mod += (val*wand->value[3])/100;
				else
					wand_mod -= (val*wand->value[3])/100;
			}
			else
				wand_mod += (val*wand->value[3])/100;
		}
		else if( wand->value[4] == WAND_MEM_PERCENT )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					wand_mod += (val*wand->value[5])/100;
				else
					wand_mod -= (val*wand->value[5])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					wand_mod += (val*wand->value[5])/100;
				else
					wand_mod -= (val*wand->value[5])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					wand_mod += (val*wand->value[5])/100;
				else
					wand_mod -= (val*wand->value[5])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					wand_mod += (val*wand->value[5])/100;
				else
					wand_mod -= (val*wand->value[5])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					wand_mod += (val*wand->value[5])/100;
				else
					wand_mod -= (val*wand->value[5])/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					wand_mod += (val*wand->value[5])/100;
				else
					wand_mod -= (val*wand->value[5])/100;
			}
			else
				wand_mod += (val*wand->value[5])/100;
		}
	}

	val += wand_mod;

	memtime = UMAX( val, 1 );

	if ( IS_AFFECTED( ch, AFF_MEDITATION ) )
    {
		memtime -= memtime / 4;
    }

    memtime = UMAX( 1, memtime );

    // w rencie memujemy ... kiepskawo
	if ( !IS_NPC( ch ) && EXT_IS_SET( ch->in_room->room_flags, ROOM_INN ) )
	{
        memtime *= UMAX( 1, (int) ch->level * ch->level / LEVEL_HERO );
    }

	return memtime;
}

void mem_list( CHAR_DATA *ch )
{
	MSPELL_DATA * mspell;
	char buf[ 100 ][ MAX_INPUT_LENGTH ];
	char tmp[ MAX_INPUT_LENGTH ];
	BUFFER *buffer = NULL;
	int count = 0, counter = 0;
	int i;
	int max = 0;

	if ( !ch->memspell )
	{
		send_to_char( "Lista zapamiêtywanych czarów jest pusta.\n\r", ch );
		return ;
	}

	buffer = new_buf();
	add_buf( buffer, "Lista zapamiêtywanych czarów wed³ug kolejno¶ci zapamiêtywania:\n\r\n\r" );

	for ( i = 0; i < 100; i++ )
		buf[ i ][ 0 ] = '\0';

	for ( mspell = ch->memspell; mspell; mspell = mspell->next, count++ );

	if ( count % 2 == 1 )
		count++;

	for ( mspell = ch->memspell; mspell; mspell = mspell->next, counter++ )
	{
		sprintf( buf[ counter ], "%d.%s %s%s{x (kr±g {C%d{x) {Y%s{x",
		         counter + 1,
		         counter + 1 < 10 ? " " : "",
		         mspell->done ? "{g" : "{r",
		         skill_table[ mspell->spell ].name,
		         mspell->circle,
		         mspell == ch->memming ? "<-" : "" );

		i = strlen( skill_table[ mspell->spell ].name );

		if ( i > max )
			max = i;
	}

	for ( i = 0; i < count / 2; i++ )
	{
		sprintf( tmp, "%-50.50s", buf[ i ] );
		add_buf( buffer, tmp );
		sprintf( tmp, "%-50.50s", buf[ count / 2 + i ] );
		add_buf( buffer, tmp );
		add_buf( buffer, "\n\r" );
	}

	add_buf( buffer, "\n\r" );
	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );
	return ;
}

/*
 * kobyla wyswietlajaca zapamietywane i zapamietane spelle
 */
void mem_show( CHAR_DATA *ch )
{
	MSPELL_DATA * tmp;
	char buf[ MAX_STRING_LENGTH ];
	struct mem_show_type
	{
		sh_int sn;
		sh_int count;
		bool done;
	}
	mem_show_list[ 9 ][ 20 ];
	sh_int mem_show_count[ 9 ];
	sh_int x = 0;
	int col = 0;
	sh_int caster = -1;
	bool test = FALSE;
	int i;

	for ( x = 0; x < 9 ; x++ )
	{
		for ( i = 0; i < 20; i++ )
		{
			mem_show_list[ x ][ i ].sn = 0;
			mem_show_list[ x ][ i ].count = 0;
		}
	}

	for ( x = 0; x < 9 ; x++ )
		mem_show_count[ x ] = 0;

	for ( tmp = ch->memspell; tmp != NULL; tmp = tmp->next )
	{
		x = tmp->circle - 1;
		test = FALSE;
		for ( i = 0; i < mem_show_count[ x ]; i++ )
		{
			if ( mem_show_list[ x ][ i ].sn == tmp->spell
			     && mem_show_list[ x ][ i ].done == tmp->done )
			{
				test = TRUE;
				break;
			}
		}

		if ( test )
			mem_show_list[ x ][ i ].count++;
		else
		{
			i = mem_show_count[ x ];
			mem_show_list[ x ][ i ].sn = tmp->spell;
			mem_show_list[ x ][ i ].count = 1;
			mem_show_list[ x ][ i ].done = tmp->done;
			mem_show_count[ x ] ++;
		}
	}

	sprintf( buf, "{b==<>==< {GCzary aktualnie zapamiêtane {b>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x" );
	send_to_char( buf, ch );

	for ( x = 0; x < 9 ; x++ )
	{
		col = 0;
		test = TRUE;
		for ( i = 0; i < mem_show_count[ x ]; i++ )
		{
			if ( mem_show_list[ x ][ i ].done )
			{
				if ( test )
					print_char( ch, "\n\r{wKr±g %d{x: ", x + 1 );

				test = FALSE;

				if ( col++ > 2 )
				{
					col = 0;
					send_to_char( "\n\r        ", ch );
				}

				sprintf( buf, "[ {G%d{x]{g%-18s{x",
				         mem_show_list[ x ][ i ].count,
				         skill_table[ mem_show_list[ x ][ i ].sn ].name );
				send_to_char( buf, ch );
			}
		}
	}


	if ( ch->memming != NULL )
	{
		sprintf( buf, "\n\r\n\r{wZapamiêtujesz:{x {W%s{x", skill_table[ ch->memming->spell ].name );
		send_to_char( buf, ch );
	}

	sprintf( buf, "\n\r\n\r{b==<>===< {GCzary do zapamiêtania{b >===<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x" );
	send_to_char( buf, ch );

	for ( x = 0; x < 9 ; x++ )
	{
		col = 0;
		test = TRUE;
		for ( i = 0; i < mem_show_count[ x ]; i++ )
		{
			if ( !mem_show_list[ x ][ i ].done )
			{
				if ( test )
					print_char( ch, "\n\r{wKr±g %d{x: ", x + 1 );

				test = FALSE;

				if ( col++ > 2 )
				{
					col = 0;
					send_to_char( "\n\r        ", ch );
				}

				sprintf( buf, "[ {r%d{x]{r%-18s{x",
				         mem_show_list[ x ][ i ].count,
				         skill_table[ mem_show_list[ x ][ i ].sn ].name );
				send_to_char( buf, ch );
			}
		}
	}

	send_to_char( "\n\r\n\r{b==<>==< {GZaklêcia wolne{x ({wkr±g{x-ilo¶æ){G{x {b>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x\n\r", ch );
	caster = get_caster( ch );

	if ( caster == CLASS_MAG )
	{
		for ( x = 1; x <= UMIN( 9, ( ( ( ch->level - 1 ) / 3 ) + 1 ) ); x++ )
		{
			sprintf( buf, "{w%d{x-%ld ", x, UMAX( 0 , get_mem_slots( ch, x, TRUE ) ) );
			send_to_char( buf, ch );
		}
	}
	else if ( caster == CLASS_CLERIC || caster == CLASS_DRUID )
	{
		for ( x = 1; x <= UMIN( 8, ( ch->level / 4 + 1 ) ); x++ )
		{
			sprintf( buf, "{w%d{x-%ld ", x, UMAX( 0 , get_mem_slots( ch, x, TRUE ) ) );
			send_to_char( buf, ch );
		}
	}
	else if ( caster == CLASS_PALADIN && ch->level > 12 )
	{
        int max = 4;
		for ( x = 1; x <= UMIN( max, ( ( ch->level - 13 ) / 4 + 1 ) ); x++ )
		{
			sprintf( buf, "{w%d{x-%ld ", x, UMAX( 0 , get_mem_slots( ch, x, TRUE ) ) );
			send_to_char( buf, ch );
		}
	}
	else if ( caster == CLASS_BARD && ch->level > 4 )
	{
		for ( x = 1; x <= UMIN( 5, ( ( ch->level - 5 ) / 5 + 1 ) ); x++ )
		{
			sprintf( buf, "{w%d{x-%ld ", x, UMAX( 0 , get_mem_slots( ch, x, TRUE ) ) );
			send_to_char( buf, ch );
		}
	}
		else if ( caster == CLASS_SHAMAN && ch->level > 9 )
	{
		for ( x = 1; x <= UMIN( 4, ( ( ch->level - 10 ) / 5 + 1 ) ); x++ )
		{
			sprintf( buf, "{w%d{x-%ld ", x, UMAX( 0 , get_mem_slots( ch, x, TRUE ) ) );
			send_to_char( buf, ch );
		}
	}

	send_to_char( "\n\r", ch );
	return ;
}

/*
 * prosta funkcyjka sprawdzajaca czy spell jest zapamietany 100% niezawodnosci hehe :)
 */
bool spell_memmed( CHAR_DATA *ch, sh_int spell )
{

   return ch->memspell_cache[ spell ];

/* 	MSPELL_DATA * tmp; */

/* 	if ( ch->memspell == NULL ) */
/* 		return FALSE; */

/* 	for ( tmp = ch->memspell; tmp != NULL; tmp = tmp->next ) */
/* 		if ( tmp->done && tmp->spell == spell ) */
/* 			return TRUE; */

/* 	return FALSE; */
}

/*
 * zwraca czy mozna zapamietac dany spell
 */
bool can_mem( CHAR_DATA *ch, sh_int circle )
{
	if ( get_mem_slots( ch, circle, TRUE ) > 0 )
		return TRUE;

	return FALSE;
}

/*
 * prostsza wersja add_mema do ladowania mem zapisanych w pliku gracza
 */
void load_mem( CHAR_DATA *ch, MSPELL_DATA *mem )
{
	MSPELL_DATA * mspel, *tmp;

	if ( IS_NPC( ch ) )
		return ;

	if ( !can_mem( ch, mem->circle ) )
		return ;

	mspel = new_mspell();
	mspel->spell = mem->spell; /*a to sn spella*/
	mspel->circle = mem->circle; /*krag...moze sie przyda*/
	mspel->done = mem->done;
	mspel->next = NULL;
	mspel->sort_next = NULL;

	if ( ch->memspell == NULL )
	{
		ch->memspell = mspel;
		return ;
	}

	for ( tmp = ch->memspell; tmp; tmp = tmp->next )
	{
		if ( !tmp->next )
		{
			tmp->next = mspel;
			mem_update_count( ch );
			return ;
		}
	}
}

/*
 * kombajn sprawdzajacy czy mozna zapamietac spella i dodajacy go do listy
 */
bool add_mem( CHAR_DATA *ch, sh_int spell )
{
	MSPELL_DATA * mspel, *tmp;
	sh_int circle;


	if ( ( circle = spell_circle( ch, spell ) ) == -1 )
		return FALSE;

	if ( !can_mem( ch, circle ) )
		return FALSE;


	mspel = new_mspell();
	mspel->spell = spell; /*a to sn spella*/
	mspel->circle = circle; /*krag...moze sie przyda*/
	mspel->done = FALSE;
	mspel->duration = 0;
	mspel->next = NULL;

	if ( ch->memspell == NULL )
	{
		ch->memspell = mspel;
		mem_update_count( ch );
		return TRUE;
	}

	for ( tmp = ch->memspell;tmp != NULL;tmp = tmp->next )
	{
		if ( tmp->next == NULL )
		{
			tmp->next = mspel;
			mem_update_count( ch );
			return TRUE;
		}
	}

	mem_update_count( ch );
	return FALSE;
}

MSPELL_DATA * get_mspell( CHAR_DATA * ch, char * argument )
{
	MSPELL_DATA * mspell;
	int num = 0, count = 1;
	int sn = 0;

	if ( !ch || !ch->memspell || IS_NULLSTR( argument ) )
		return NULL;

	if ( is_number( argument ) )
		num = atoi( argument );
	else
	{
		if ( ( sn = find_spell_new( ch, argument, TRUE ) ) < 1 ||
		     skill_table[ sn ].spell_fun == spell_null ||
		     !knows_spell( ch, sn ) )
			return NULL;
	}

	for ( mspell = ch->memspell; mspell; mspell = mspell->next, count++ )
	{
		if ( ( num > 0 && num == count ) || mspell->spell == sn )
			return mspell;
	}

	return NULL;
}

MSPELL_DATA * get_mspell_type( CHAR_DATA * ch, char * argument, bool type )
{
	MSPELL_DATA * mspell;
	int num = 0, count = 1;
	int sn = 0;

	if ( !ch || !ch->memspell || IS_NULLSTR( argument ) )
		return NULL;

	if ( is_number( argument ) )
		num = atoi( argument );
	else
	{
		if ( ( sn = find_spell_new( ch, argument, TRUE ) ) < 1 ||
		     skill_table[ sn ].spell_fun == spell_null ||
		     !knows_spell( ch, sn ) )
			return NULL;
	}

	for ( mspell = ch->memspell; mspell; mspell = mspell->next, count++ )
	{
		if ( ( ( num > 0 && num == count ) || mspell->spell == sn ) && mspell->done == type )
			return mspell;
	}

	return NULL;
}

MSPELL_DATA * get_mspell_by_sn( CHAR_DATA * ch, int sn, bool done )
{
	MSPELL_DATA * mspell;

	for ( mspell = ch->memspell; mspell; mspell = mspell->next )
	{
		if ( mspell->spell == sn && mspell->done == done )
			return mspell;
	}

	return NULL;
}

/*
 * usuwa czar z mema
 * jesli ch cos zapamietuje przypisuje mu nowy do zapamietywania
 */
bool remove_mem( CHAR_DATA *ch, MSPELL_DATA * mspell )
{
	MSPELL_DATA * tmp, *tmp_prev;
	sh_int x = 0;

	if ( !ch || !ch->memspell || !mspell )
		return FALSE;

	tmp_prev = NULL;

	for ( tmp = ch->memspell;tmp != NULL;tmp = tmp->next, x++ )
	{
		if ( tmp == mspell )
		{
			if ( x == 0 )    /*pierwszy element do wywalenia*/
			{
				if ( tmp == ch->memming )
				{
					echo_mem( ch, MEM_STOP );
					get_next_memming( ch );
				}

				ch->memspell = tmp->next;

				if ( !tmp->done )
				{
					tmp->done = TRUE;
				}

				mem_update_count( ch );
				free_mspell( tmp );
				return TRUE;
			}
			else
			{
				if ( tmp == ch->memming )
				{
					echo_mem( ch, MEM_STOP );
					get_next_memming( ch );
				}

				tmp_prev->next = tmp->next;

				if ( !tmp->done )
				{
					tmp->done = TRUE;
				}

				mem_update_count( ch );
				free_mspell( tmp );
				return TRUE;
			}
		}

		tmp_prev = tmp; /*przechodzimy na aktualny z drugim tempem*/
	}

	mem_update_count( ch );

	return FALSE;
}

/*
 * usuwa z mema czary nieznane postaci (przy roszadach z czarami)
 */
void remove_not_known( CHAR_DATA *ch )
{
	MSPELL_DATA * mspell, * mspell_next;

	if ( ch->memspell == NULL )
		return ;

	for ( mspell = ch->memspell; mspell != NULL; mspell = mspell_next )
	{
		mspell_next = mspell->next;

		if ( !knows_spell( ch, mspell->spell ) )
			remove_mem( ch, mspell );
	}

	mem_update_count( ch );

	return ;
}

/*
 * Czy postaæ mo¿e w obecnej chwili memowac (odpoczywa, nie walczy,
 * nie jest ciemno, nie jest slepa, nie jest w no_mem)
 */
bool can_mem_now( CHAR_DATA * ch )
{
	if ( ch->position != POS_RESTING
	     || IS_AFFECTED( ch, AFF_PARALYZE )
	     || ch->fighting
	     || ( IS_AFFECTED ( ch, AFF_BLIND ) && !IS_AFFECTED(ch,AFF_PERFECT_SENSES) )
	     || room_is_dark ( ch, ch->in_room )
	     || IS_SET ( sector_table[ ch->in_room->sector_type ].flag, SECT_NOMEM ) )
		return FALSE;

	return TRUE;
}

/*
 * uzywana tylko w przypadku smierci
 * kasuje cala liste memspelli
 */
void clear_mem( CHAR_DATA *ch )
{
	MSPELL_DATA * tmp, *tmp_prev;

	if ( ch->memspell == NULL )
		return ;

	if ( ch->memming )
	{
		echo_mem( ch, MEM_STOP );
		echo_mem( ch, MEM_STOP_ALL );
		if ( IS_AFFECTED( ch, AFF_MEDITATION ) )
		{
			print_char( ch, "Koñczysz medytacje.\n\r" );
			affect_strip( ch, gsn_meditation );
		}
	}

	tmp = ch->memspell;
	ch->memspell = NULL;
	ch->memming = NULL;
	tmp_prev = tmp;

	do
	{

		tmp = tmp->next;
		free_mspell( tmp_prev );
		tmp_prev = tmp;

	}
	while ( tmp != NULL ); /*wyjdzie na ostatnim elemencie*/

	mem_update_count( ch );

	return ;
}

void do_memorize( CHAR_DATA *ch, char *argument )
{
    MSPELL_DATA * mspell;
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char arg3[ MAX_INPUT_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    sh_int caster;
    int sn;

    if ( ch->position == POS_SLEEPING )
    {
        send_to_char( "¦nisz o potê¿nych zaklêciach do zapamiêtania.\n\r", ch );
        return ;
    }

    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
    {
        send_to_char( "Bogowie nie musz± bawiæ siê w zapamietywanie.\n\r", ch );
        return ;
    }

    caster = get_caster( ch );

    if ( caster < 0 )
    {
        if ( ch->sex == 0 )
        {
            send_to_char( "Czy co¶ chcia³oby¶ sobie zapamiêtaæ?\n\r", ch );
        }
        else if ( ch->sex == 1 )
        {
            send_to_char( "Czy co¶ chcia³by¶ sobie zapamiêtaæ?\n\r", ch );
        }
        else
        {
            send_to_char( "Czy co¶ chcia³aby¶ sobie zapamiêtaæ?\n\r", ch );
        }
        return ;
    }

    argument = one_argument( argument, arg1 );

    if ( arg1[ 0 ] == '\0' )
    {
        mem_show( ch );
        return ;
    }

    if ( !str_cmp( arg1, "sort" ) )
    {
        MSPELL_DATA * sort_list[ 9 ];
        MSPELL_DATA * for_mspell;
        int i, circle;
        char * low_name;
        bool inserted, first;
        bool reverse = FALSE;
        one_argument( argument, arg2 );

        if ( arg2[ 0 ] != '\0' )
        {
            reverse = TRUE;
        }

        for ( i = 0; i < 9; i++ )
        {
            sort_list[ i ] = NULL;
        }

        for ( mspell = ch->memspell; mspell; mspell = mspell->next )
        {
            mspell->sort_next = NULL;
            circle = mspell->circle - 1;

            if ( !sort_list[ circle ] )
            {
                sort_list[ circle ] = mspell;
                continue;
            }

            low_name = skill_table[ mspell->spell ].name;

            if ( strcmp( low_name, skill_table[ sort_list[ circle ] ->spell ].name ) <= 0 )
            {
                mspell->sort_next = sort_list[ circle ];
                sort_list[ circle ] = mspell;
                continue;
            }

            inserted = FALSE;
            for ( for_mspell = sort_list[ circle ]; for_mspell->sort_next; for_mspell = for_mspell->sort_next )
            {
                if ( strcmp( low_name, skill_table[ for_mspell->sort_next->spell ].name ) <= 0 )
                {
                    mspell->sort_next = for_mspell->sort_next;
                    for_mspell->sort_next = mspell;
                    inserted = TRUE;
                    break;
                }
            }

            if ( !inserted )
            {
                for_mspell->sort_next = mspell;
            }
        }

        ch->memspell = NULL;
        mspell = NULL;

        if ( !reverse )
        {
            for ( i = 0; i < 9; i++ )
            {
                if ( !sort_list[ i ] )
                {
                    continue;
                }

                first = TRUE;
                if ( !sort_list[ i ] ->sort_next )
                {
                    sort_list[ i ] ->next = NULL;

                    if ( !ch->memspell )
                    {
                        ch->memspell = sort_list[ i ];
                    }

                    if ( mspell )
                    {
                        mspell->next = sort_list[ i ];
                    }

                    mspell = sort_list[ i ];
                }
                else
                {
                    for ( for_mspell = sort_list[ i ]; for_mspell->sort_next; for_mspell = for_mspell->sort_next )
                    {
                        if ( !ch->memspell )
                        {
                            ch->memspell = for_mspell;
                        }

                        if ( first && mspell )
                        {
                            mspell->next = for_mspell;
                        }

                        for_mspell->next = for_mspell->sort_next;
                        first = FALSE;
                    }

                    mspell = for_mspell;
                }
            }
            if ( mspell )
            {
                mspell->next = NULL;
            }
            send_to_char( "Lista zapamiêtywanych czarów posortowana wed³ug krêgów (rosn±co).\n\r", ch );
        }
        else
        {
            for ( i = 8; i >= 0; i-- )
            {
                if ( !sort_list[ i ] )
                {
                    continue;
                }

                first = TRUE;
                if ( !sort_list[ i ] ->sort_next )
                {
                    sort_list[ i ] ->next = NULL;

                    if ( !ch->memspell )
                    {
                        ch->memspell = sort_list[ i ];
                    }

                    if ( mspell )
                    {
                        mspell->next = sort_list[ i ];
                    }

                    mspell = sort_list[ i ];
                }
                else
                {
                    for ( for_mspell = sort_list[ i ]; for_mspell->sort_next; for_mspell = for_mspell->sort_next )
                    {
                        if ( !ch->memspell )
                        {
                            ch->memspell = for_mspell;
                        }

                        if ( first && mspell )
                        {
                            mspell->next = for_mspell;
                        }

                        for_mspell->next = for_mspell->sort_next;
                        first = FALSE;
                    }

                    mspell = for_mspell;
                }
            }
            if ( mspell )
            {
                mspell->next = NULL;
            }
            send_to_char( "Lista zapamiêtywanych czarów posortowana wed³ug krêgów (malej±co).\n\r", ch );
        }
    }
    else if ( !str_cmp( arg1, "clear" ) )
    {
        if ( !ch->memspell )
        {
            send_to_char( "Lista zapamiêtywanych czarów jest ju¿ pusta.\n\r", ch );
            return ;
        }

        clear_mem( ch );
        send_to_char( "Lista zapamiêtywanych czarów jest teraz pusta.\n\r", ch );
    }
    else if ( !str_cmp( arg1, "remove" ) )
    {
        one_argument( argument, arg2 );

        if ( arg2[ 0 ] == '\0' )
        {
            send_to_char( "Które zaklêcie chcesz usun±c z listy?\n\r", ch );
            return ;
        }

        mspell = get_mspell( ch, arg2 );

        if ( !mspell )
        {
            send_to_char( "Nie masz takiego zaklêcia na li¶cie.\n\r", ch );
            return ;
        }

        sn = mspell->spell;

        sprintf( buf, "Zaklêcie '%s' usuniête z listy.\n\r", skill_table[ sn ].name );
        send_to_char( buf, ch );
        remove_mem( ch, mspell );
    }
    else if ( !str_cmp( arg1, "stop" ) )
    {
        one_argument( argument, arg2 );

        if ( arg2[ 0 ] == '\0' )
        {
            send_to_char( "Którego zaklêcia nie chcesz zapamiêtaæ?\n\r", ch );
            return ;
        }

        mspell = get_mspell_type( ch, arg2, FALSE );

        if ( !mspell )
        {
            send_to_char( "Nie masz takiego zaklêcia na li¶cie do zapamiêtania.\n\r", ch );
            return ;
        }

        sn = mspell->spell;

        sprintf( buf, "Zaklêcie '%s' usuniête z listy czarów do zapamiêtania.\n\r", skill_table[ sn ].name );
        send_to_char( buf, ch );
        remove_mem( ch, mspell );
    }
    else if ( !str_cmp( arg1, "forget" ) )
    {
        one_argument( argument, arg2 );

        if ( arg2[ 0 ] == '\0' )
        {
            send_to_char( "Które zaklêcie chcesz zapomnieæ?\n\r", ch );
            return ;
        }

        mspell = get_mspell_type( ch, arg2, TRUE );

        if ( !mspell )
        {
            send_to_char( "Nie masz takiego zaklêcia na li¶cie zapamiêtanych.\n\r", ch );
            return ;
        }

        sn = mspell->spell;

        sprintf( buf, "Zaklêcie '%s' usuniête z listy zapamiêtanych.\n\r", skill_table[ sn ].name );
        send_to_char( buf, ch );
        remove_mem( ch, mspell );
    }
    else if ( !str_cmp( arg1, "skip" ) )
    {
        if ( !ch->memming )
        {
            send_to_char( "Aktualnie nie zapamiêtujesz ¿adnych czarów.\n\r", ch );
            return ;
        }

        one_argument( argument, arg2 );

        if ( arg2[ 0 ] == '\0' )
        {
            get_next_memming( ch );
            return ;
        }

        mspell = get_mspell_type( ch, arg2, FALSE );

        if ( !mspell )
        {
            send_to_char( "Nie masz takiego zaklêcia na li¶cie zapamiêtywanych lub ju¿ zapamiêta³<&e¶/a¶/o¶> to zaklêcie.\n\r", ch );
            return ;
        }

        if ( ch->memming == mspell )
        {
            send_to_char( "Jeste¶ ju¿ w trakcie zapamiêtywania tego zaklêcia.\n\r", ch );
            return ;
        }

        if ( ch->memming )
        {
            echo_mem( ch, MEM_STOP );
        }

        ch->memming = mspell;

        if ( ch->memming->done )
        {
            get_next_memming( ch );
        }
        else
        {
            ch->memming->duration = get_mspell_duration( ch, ch->memming );
            if ( ch->position == POS_RESTING )
                echo_mem( ch, MEM_NEW );
        }
    }
    else if ( !str_cmp( arg1, "move" ) )
    {
        MSPELL_DATA * prev_mspell, * next_mspell, * for_mspell, * insert_mspell;
        int num;

        argument = one_argument( argument, arg2 );

        if ( arg2[ 0 ] == '\0' )
        {
            send_to_char( "Od którego zaklêcia chcesz zacz±æ zapamiêtywanie?\n\r", ch );
            return ;
        }

        mspell = get_mspell( ch, arg2 );

        if ( !mspell )
        {
            send_to_char( "Nie masz takiego zaklêcia na li¶cie zapamiêtywanych.\n\r", ch );
            return ;
        }

        prev_mspell = NULL;
        next_mspell = mspell->next;

        for ( for_mspell = ch->memspell; for_mspell; for_mspell = for_mspell->next )
        {
            if ( for_mspell->next == mspell )
            {
                prev_mspell = for_mspell;
                break;
            }
        }

        one_argument( argument, arg2 );

        if ( is_number( arg2 ) )
        {
            num = atoi( arg2 );

            if ( num <= 1 )  //tak jak first
            {
                if ( prev_mspell )
                {
                    prev_mspell->next = next_mspell;
                    mspell->next = ch->memspell;
                    ch->memspell = mspell;
                }
            }
            else if ( num > count_mspell( ch, TRUE, FALSE ) )  //tak jak last
            {
                for ( for_mspell = ch->memspell; for_mspell->next; for_mspell = for_mspell->next ) ;

                if ( ch->memspell == mspell )
                    ch->memspell = next_mspell;
                else
                    prev_mspell->next = next_mspell;
                for_mspell->next = mspell;
                mspell->next = NULL;
            }
            else
            {
                if ( ch->memspell == mspell )
                    ch->memspell = next_mspell;
                else
                    prev_mspell->next = next_mspell;

                insert_mspell = NULL;

                for ( for_mspell = ch->memspell; for_mspell && num > 1; insert_mspell = for_mspell, for_mspell = for_mspell->next, num-- ) ;

                insert_mspell->next = mspell;
                mspell->next = for_mspell;
            }

            send_to_char( "Czar przesuniêty.\n\r", ch );
            return ;
        }
        else if ( !str_cmp( arg2, "first" ) )
        {
            if ( !prev_mspell )
            {
                send_to_char( "To zaklêcie jest ju¿ na pocz±tku listy zaklêæ.\n\r", ch );
                return ;
            }

            prev_mspell->next = next_mspell;
            mspell->next = ch->memspell;
            ch->memspell = mspell;
            send_to_char( "Czar przesuniêty na pocz±tek listy.\n\r", ch );
        }
        else if ( !str_cmp( arg2, "last" ) )
        {
            if ( !next_mspell )
            {
                send_to_char( "To zaklêcie jest ju¿ na koñcu listy zaklêæ.\n\r", ch );
                return ;
            }

            for ( for_mspell = ch->memspell; for_mspell->next; for_mspell = for_mspell->next ) ;

            if ( ch->memspell == mspell )
                ch->memspell = next_mspell;
            else
                prev_mspell->next = next_mspell;
            for_mspell->next = mspell;
            mspell->next = NULL;
            send_to_char( "Czar przesuniêty na koniec listy.\n\r", ch );
        }
    }
    else if ( !str_cmp( arg1, "list" ) )
    {
        mem_list( ch );
    }
    else if ( !str_cmp( arg1, "set" ) )
    {
        argument = one_argument( argument, arg2 );

        if ( arg2[ 0 ] == '\0' ) {
            list_memsets( ch );
            return ;
        }

        argument = one_argument( argument, arg3 );

        if ( arg3[ 0 ] == '\0' ) {
            load_memset( ch, arg2 );
            return;
        }
        else if ( !str_cmp( arg3, "save" )) {
	    save_memset( ch, arg2, FALSE );
            return;
        }
        else if ( !str_cmp( arg3, "remove" )) {
            remove_memset( ch, arg2, FALSE );
            return;
        }
        else if ( !str_cmp( arg3, "update" )) {
            save_memset( ch, arg2, TRUE );
            return;
        }
        else {
            send_to_char( "Sk³adnia dla zestawów czarów:\n\r"
                "memorize set - lista zestawów zaklêæ\n\r"
                "memorize set 'nazwa zestawu' - ustawienie listy zapamiêtywanch czarów jako zestaw 'nazwa zestawu'\n\r"
                "memorize set 'nazwa zestawu' save - ustawienie aktualnej listy czarów jako zestaw 'nazwa zestawu'\n\r"
                "memorize set 'nazwa zestawu' update - ustawienie aktualnej listy czarów jako zestaw 'nazwa zestawu' i skasowanie starego zestawu o tej nazwie\n\r"
                "memorize set 'nazwa zestawu' remove - usuniêcie danego zestawu 'nazwa zestawu'\n\r", ch );
        }
    }
    else if ( !str_cmp( arg1, "help" ) )
    {
        send_to_char( "Sk³adnia:\n\r"
                "memorize list - lista zapamiêtywanych zaklêæ\n\r"
                "memorize 'czar' - dodanie czaru na pocz±tku listy zapamiêtywanych\n\r"
                "memorize remove 'czar lub numer' - usuniêcie danego czaru z listy\n\r"
                "memorize stop 'czar lub numer' - usuniêcie danego nie zapamiêtanego czaru z listy\n\r"
                "memorize forget 'czar lub numer' - usuniêcie danego zapamiêtanego czaru z listy\n\r"
                "memorize clear - usuniêcie wszystkich czarów z listy\n\r"
                "memorize skip - przerwanie zapamiêtywania aktualnego czaru i przej¶cie do nastepnego\n\r"
                "memorize skip 'czar lub numer' - przerwanie zapamiêtywania aktualnego czaru i przej¶cie do podanego\n\r"
                "memorize move 'czar lub numer' 'numer' - przesuniêcie danego czaru na konkretne miejsce na li¶cie\n\r"
                "memorize move 'czar lub numer' first - przesuniêcie danego czaru na pierwsze miejsce w li¶cie\n\r"
                "memorize move 'czar lub numer' last - przesuniêcie danego czaru na ostatnie miejsce w li¶cie\n\r"
                "memorize sort - posortowanie czarów na li¶cie wed³ug krêgów (rosn±co).\n\r"
                "memorize sort reverse - jak wy¿ej, ale malej±co.\n\r"
                "memorize set - lista zestawów zaklêæ\n\r"
                "memorize set 'nazwa zestawu' - ustawienie listy zapamiêtywanch czarów jako zestaw 'nazwa zestawu'\n\r"
                "memorize set 'nazwa zestawu' save - ustawienie aktualnej listy czarów jako zestaw 'nazwa zestawu'\n\r"
                "memorize set 'nazwa zestawu' remove - usuniêcie danego zestawu 'nazwa zestawu'\n\r"
                "memorize where 'czar' - pokazuje kr±g na którym znajduje siê dany czar\n\r", ch );
    }
    else if ( !str_cmp( arg1, "where" ) )
    {
        argument = one_argument( argument, arg2 );
        if (
                ( sn = find_spell_new( ch, arg2, TRUE ) ) >= 1
                && skill_table[ sn ].spell_fun != spell_null
                && knows_spell( ch, sn )
           )
        {
            print_char( ch, "\n\r{GKr±g %d{x: %s\n\r", spell_circle( ch, sn ), skill_table[ sn ].name );
        }
        else
        {
            send_to_char( "Nie znasz takiego zaklêcia.\n\r", ch );
        }
    }
    else
    {
        if (
                ( sn = find_spell_new( ch, arg1, TRUE ) ) >= 1
                && skill_table[ sn ].spell_fun != spell_null
                && knows_spell( ch, sn )
           )
        {
            if ( add_mem( ch, sn ) )
            {
                print_char( ch, "Dodajesz zaklêcie '%s' do listy zapamiêtywanych.\n\r", skill_table[ sn ].name );
                if ( !ch->memming && ch->position == POS_RESTING )
                {
                    echo_mem( ch, MEM_START );
                    get_next_memming( ch );
                }
            }
            else
            {
                send_to_char( "Nie dasz rady tego zapamietaæ.\n\r", ch );
            }
            return ;
        }
        else if ( ch->level < skill_table[ sn ].skill_level[ caster ] )
        {
            send_to_char( "Jeszcze nie mo¿esz zapamietywaæ zaklêæ z tego krêgu.\n\r", ch );
        }
        else
        {
            send_to_char( "Nie znasz takiego zaklêcia.\n\r", ch );
        }
    }
    return ;
}

int get_mem_slots( CHAR_DATA *ch, int circle, bool free )
{
	int val, x = 0, mod;
	MSPELL_DATA *tmp;
	sh_int caster = -1;

	if ( circle > 9 || circle < 0 )
		return -1;

	caster = get_caster( ch );

	if ( caster < 0 ) return -1;

	switch ( caster )
	{
		case CLASS_MAG:
			val = mem_numb_mag[ UMIN( ( ch->level - 1 ), LEVEL_HERO ) ][ circle - 1 ];

			/* specjalisci maja +1 bonus */
			if ( !IS_NPC( ch ) && (ch->pcdata))
			    if (ch->pcdata->mage_specialist >= 0 )
				    val++;

			break;

		case CLASS_CLERIC:
		case CLASS_DRUID:
			if ( circle > 8 )
				return -1;

			val = mem_numb_cle[ UMIN( ( ch->level - 1 ), LEVEL_HERO ) ][ circle - 1 ];
			val += wis_app[ get_curr_stat_deprecated( ch, STAT_WIS ) ].bonus_spells - ( ( circle * 3 ) / 4 );
			break;

		case CLASS_PALADIN:
            mod = 0;
			if ( circle > ( 4 + mod ) || ch->level < 13 )
				return -1;

			val = mem_numb_cle[ URANGE( 0, ch->level - 14, LEVEL_HERO ) ][ circle - 1 ];
			val += wis_app[ get_curr_stat_deprecated( ch, STAT_WIS ) ].bonus_spells - ( ( circle * 3 ) / 4 );
			break;
		case CLASS_BARD:
			if ( circle > 5 || ch->level < 5 )
				return -1;
			val = mem_numb_brd[ URANGE( 0, ch->level - 5, LEVEL_HERO ) ][ circle - 1 ];
			break;

      case CLASS_SHAMAN:
         if ( circle > 4 || ch->level <10 )
            {
               return -1;
            }

         val = mem_numb_sha[ URANGE( 0, ch->level - 10, LEVEL_HERO ) ][ circle - 1];
         break;

		default: return -1;
	}

	val += ch->mem_mods[ circle - 1 ];

	val = UMAX( 0, val );

	if ( free )
	{
		for ( x = 0, tmp = ch->memspell;tmp != NULL;tmp = tmp->next )
			if ( tmp->circle == circle )
				x++;

		return ( val -x );
	}

	return val;
}

sh_int get_caster( CHAR_DATA *ch )
{
    if ( IS_NPC( ch ) )
    {
        if ( EXT_IS_SET( ch->act, ACT_MAGE ) )
            return CLASS_MAG;
        else if ( EXT_IS_SET( ch->act, ACT_PALADIN ) )
            return CLASS_PALADIN;
        else if ( EXT_IS_SET( ch->act, ACT_DRUID ) )
            return CLASS_DRUID;
        else if ( EXT_IS_SET( ch->act, ACT_BARD ) )
            return CLASS_BARD;
        else if ( EXT_IS_SET( ch->act, ACT_CLERIC ) || EXT_IS_SET( ch->act, ACT_UNDEAD ) )//to ma byc na koncu ze wzgledu na to undead
            return CLASS_CLERIC;
        else if ( EXT_IS_SET( ch->act, ACT_SHAMAN ) )
            return CLASS_SHAMAN;
        else
            return -1;
    }
    else
    {
        return class_table[ ch->class ].caster;
    }
    return -1;
}

int chance_to_learn_spell( CHAR_DATA *ch, sh_int spell )
{
	int caster = -1, chance, circle;
    bool check;

	if ( IS_NPC( ch ) || spell <= 0 || spell > MAX_SKILL || skill_table[ spell ].spell_fun == spell_null )
		return 0;

	if ( ( caster = get_caster( ch ) ) < 0 )
		return 0;

	if ( ( circle = spell_circle( ch, spell ) ) < 0 )
		return 0;

	//baza szansy ( min 30, max 95)
	//dalem umax coby szanse wyrownac innych casterow
	chance = URANGE( 30, int_app[ UMAX( get_curr_stat_deprecated( ch, STAT_WIS ), get_curr_stat_deprecated( ch, STAT_INT ) ) ].learn + 20, 95 );

	switch ( circle )
	{
		case 0: break;
		case 1: chance -= 10;	break;
		case 2: chance -= 15;	break;
		case 3: chance -= 20;	break;
		case 4: chance -= 25;	break;
		case 5: chance -= 30;	break;
		case 6: chance -= 36;	break;
		case 7: chance -= 42;	break;
		case 8: chance -= 48;	break;
		case 9: chance -= 56;	break;
		default: chance -= 65;	break;
	}

	switch ( caster )
	{
		case CLASS_CLERIC:
		case CLASS_DRUID:
		case CLASS_MAG:
			chance += ( 3 * ch->level ) / 4;
			break;
		case CLASS_PALADIN:
            check = FALSE;
			if ( check )
				chance += ( 3 * ch->level ) / 4;
			else
				chance += URANGE( 0, 3 * ( ch->level - 13 ) / 4, 15 );
			break;
		case CLASS_BARD:
      case CLASS_SHAMAN:
			chance += UMAX( 0, 3 * ( ch->level - 5 ) / 4 );
		default: break;
	}

	/* no i mo¿e do czego¶ by lucka wykorzystaæ? */
	if ( number_range( 8, 20 ) < get_curr_stat_deprecated( ch, STAT_LUC ) )
		chance += get_curr_stat_deprecated( ch, STAT_LUC ) / 2;

	chance = URANGE( 10, chance, 95 );

	return chance;
}

bool can_learn_here( CHAR_DATA *ch, CHAR_DATA *mob, sh_int spell )
{
	SPELL_FAILED * list;

	if ( IS_NPC( ch ) || !IS_NPC( mob ) || spell <= 0 || spell > MAX_SKILL )
		return FALSE;

	if ( !ch->pcdata->spells_failed )
		return TRUE;

	for ( list = ch->pcdata->spells_failed; list; list = list->next )
		if ( mob->pIndexData->vnum == list->vnum && spell == list->spell )
			return FALSE;

	return TRUE;
}

void add_spells_failed( CHAR_DATA *ch, CHAR_DATA *mob, sh_int spell )
{
	SPELL_FAILED * new_element;

	if ( IS_NPC( ch ) || !IS_NPC( mob ) || spell <= 0 || spell > MAX_SKILL )
		return ;

	new_element = new_spell_failed();
	new_element->vnum = mob->pIndexData->vnum;
	new_element->spell = spell;
	new_element->next = ch->pcdata->spells_failed;
	ch->pcdata->spells_failed = new_element;
	return ;
}

void load_spells_failed( CHAR_DATA *ch, unsigned int mob_vnum, sh_int spell )
{
	SPELL_FAILED * new_element;

	new_element = new_spell_failed();
	new_element->vnum = mob_vnum;
	new_element->spell = spell;
	new_element->next = ch->pcdata->spells_failed;
	ch->pcdata->spells_failed = new_element;
	return ;
}

void clear_spells_failed( CHAR_DATA *ch )
{
	SPELL_FAILED * list, *tmp;

	if ( IS_NPC( ch ) )
		return ;

	if ( !ch->pcdata->spells_failed )
		return ;

	list = ch->pcdata->spells_failed;

	do
	{
		tmp = list;
		list = list->next;
		free_spell_failed( tmp );
	}
	while ( list );

	ch->pcdata->spells_failed = NULL;
	return ;
}

#ifdef NEW_NANNY
bool new_can_be_specialist( NEW_CHAR_DATA * new_ch, int school )
{
	if ( school < 0 || school > MAX_SCHOOL )
		return FALSE;

	if ( ( school_table[ school ].race[ 0 ] != 0 && school_table[ school ].race[ 0 ] == new_ch->race ) ||
	     ( school_table[ school ].race[ 1 ] != 0 && school_table[ school ].race[ 1 ] == new_ch->race ) ||
	     ( school_table[ school ].race[ 2 ] != 0 && school_table[ school ].race[ 2 ] == new_ch->race ) )
		return TRUE;

	return FALSE;
}

bool new_can_be_any_specialist( NEW_CHAR_DATA * new_ch )
{
	int school;

	for ( school = 0 ; school < MAX_SCHOOL ; school++ )
	{
		if ( new_can_be_specialist( new_ch, school ) )
			return TRUE;
	}
	return FALSE;
}
#endif

bool can_be_specialist( CHAR_DATA *ch, int school )
{
	if ( school < 0 || school > MAX_SCHOOL )
		return FALSE;

	if ( ( school_table[ school ].race[ 0 ] != 0 && school_table[ school ].race[ 0 ] == GET_RACE( ch ) ) ||
	     ( school_table[ school ].race[ 1 ] != 0 && school_table[ school ].race[ 1 ] == GET_RACE( ch ) ) ||
	     ( school_table[ school ].race[ 2 ] != 0 && school_table[ school ].race[ 2 ] == GET_RACE( ch ) ) )
		return TRUE;

	return FALSE;
}

bool can_be_any_specialist( CHAR_DATA *ch )
{
	int school;

	for ( school = 0 ; school < MAX_SCHOOL ; school++ )
	{
		if ( can_be_specialist( ch, school ) )
			return TRUE;
	}
	return FALSE;
}

bool can_learn_spell( CHAR_DATA *ch, sh_int spell )
{
	if ( IS_NPC( ch ) || spell <= 0 || spell > MAX_SKILL )
		return FALSE;

	/* sprawdzanie specjalistow */
	if ( ch->class == CLASS_MAG &&
		ch->pcdata->mage_specialist >= 0 &&
	    skill_table[ spell ].school != 0 &&
	    IS_SET( school_table[ ch->pcdata->mage_specialist ].reverse_school, skill_table[ spell ].school ) )
		return FALSE;

	if ( ch->class == CLASS_MAG && IS_SET( skill_table[ spell ].school, SpellSpec ) )
	{
		if ( ch->pcdata->mage_specialist < 0 )
			return FALSE;

		if ( !( school_table[ ch->pcdata->mage_specialist ].flag & skill_table[ spell ].school ) )
			return FALSE;
	}

	if ( ch->class == CLASS_MAG && IS_SET( skill_table[ spell ].school, SpellSpecAndGeneral ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && !( school_table[ ch->pcdata->mage_specialist ].flag & skill_table[ spell ].school ) )
			return FALSE;
	}

	return TRUE;
}


/*
 * funkcja generujaca teksty only
 * action ==
 * 0: zaczyna
 * 1: konczy
 * 2: przerywa
 */
void echo_mem( CHAR_DATA *ch, int action )
{
	int caster;

	if ( IS_NPC( ch ) && !IS_SET( race_table[ GET_RACE( ch ) ].type, PERSON ) )
		return ;

    /**
     * sprawdzamy czy mozemy memowac
     * trzeba to zrobic ponownie, bo dotyczy automema
     */
    if ( !can_mem_now( ch ) )
    {
        return;
    }

	caster = get_caster( ch );

	if ( action == MEM_START )
	{
		switch ( caster )
		{
			case 0:
			case 8:
				send_to_char( "Wyjmujesz ksiêgê czarów i zaczynasz zapamiêtywanie zaklêæ.\n\r", ch );
				if ( !is_affected( ch, gsn_perfect_self ) ) act( "$n wyjmuje magiczn± ksiêgê i zaczyna j± studiowaæ.", ch, NULL, NULL, TO_ROOM );
				break;
			case 1:
			case 4:
				send_to_char( "Wyjmujesz modlitewnik i zag³êbiasz siê w modlitwie.\n\r", ch );
				act( "$n wyjmuje modlitewnik i zaczyna modlitwê.", ch, NULL, NULL, TO_ROOM );
				break;
			case 5:
				send_to_char( "Wyjmujesz modlitewnik i zag³êbiasz siê w modlitwie.\n\r", ch );
				act( "$n wyjmuje modlitewnik i zaczyna modlitwê.", ch, NULL, NULL, TO_ROOM );
				break;
			case 10:
				send_to_char( "Zag³êbiasz siê w medytacji wzywaj±c duchy przodków.\n\r", ch );
				act( "$n krzy¿uje nogi i zamiera w bezruchu.", ch, NULL, NULL, TO_ROOM );
				break;
			default:
				return ;
		}
		return ;
	}
	else if ( action == MEM_DONE && ch->memming )
	{
		print_char( ch, "Zapamiêta³<&e¶/a¶/o¶> czar '%s'.\n\r", skill_table[ ch->memming->spell ].name );
		return ;
	}
	else if ( action == MEM_NEW && ch->memming )
	{
		print_char( ch, "Zaczynasz zapamiêtywaæ czar '%s'.\n\r", skill_table[ ch->memming->spell ].name );
		return ;
	}
	else if ( action == MEM_STOP && ch->memming )
	{
		print_char( ch, "Przerywasz zapamiêtywanie czaru '%s'.\n\r", skill_table[ ch->memming->spell ].name );
		return ;
	}
	else if ( action == MEM_DONE_ALL )
	{
		switch ( caster )
		{
			case 0:
			case 8:
				send_to_char( "Skonczy³<&e/a/o>¶ zapamietywanie zaklêæ.\n\r", ch );
				if ( !is_affected( ch, gsn_perfect_self ) ) act( "$n zamyka i odk³ada magiczn± ksiêgê.", ch, NULL, NULL, TO_ROOM );
				break;
			case 1:
			case 4:
				send_to_char( "Skonczy³<&e/a/o>¶ modlitwê.\n\r", ch );
				act( "$n zamyka i odk³ada modlitewnik.", ch, NULL, NULL, TO_ROOM );
				break;
			case 5:
				send_to_char( "Skonczy³<&e/a/o>¶ modlitwê.\n\r", ch );
				act( "$n zamyka i odk³ada modlitewnik.", ch, NULL, NULL, TO_ROOM );
				break;
			case 10:
				send_to_char( "Skonczy³<&e/a/o>¶ wzywaæ duchy przodków.\n\r", ch );
				act( "$n otrz±sa siê z bezruchu.", ch, NULL, NULL, TO_ROOM );
				break;
			default:
				break;
		}
	}
	else
	{
		if ( ch->position == POS_RESTING || ch->position == POS_SITTING )
		{
			switch ( caster )
			{
				case 0:
				case 8:
					send_to_char( "Zamykasz i chowasz swoj± ksiêgê magiczn±.\n\r", ch );
					if ( !is_affected( ch, gsn_perfect_self ) ) act( "$n odrywa siê od magicznej ksiêgi, zamyka i odk³ada j±.", ch, NULL, NULL, TO_ROOM );
					break;
				case 1:
				case 4:
				case 5:
					send_to_char( "Zamykasz i chowasz swój modlitewnik.\n\r", ch );
					act( "$n przerywa modlitwê, po czym zamyka i odk³ada modlitewnik.", ch, NULL, NULL, TO_ROOM );
					break;
			case 10:
				send_to_char( "Przerywasz swoj± duchow± medytacje.\n\r", ch );
				act( "$n otrz±sa siê z bezruchu.", ch, NULL, NULL, TO_ROOM );
				break;
				default:
					break;
			}
		}
	}
	return ;
}

void mspell_update( void )
{
	CHAR_DATA * ch;

	for ( ch = char_list; ch; ch = ch->next )
	{
		if ( !ch->memspell || !ch->memming )
			continue;

		if ( !can_mem_now( ch ) )
		{
			ch->memming->duration = get_mspell_duration( ch, ch->memming );
			continue;
		}

		ch->memming->duration--;
		if ( ch->memming->duration <= 0 )
			mem_done( ch );
	}
}


void save_memset( CHAR_DATA *ch, char *memset_name, bool overwrite_old )
{
    MEMSET_DATA * memset;
    MSPELL_DATA * mspell;
    char allspell_cache[MAX_SKILL];
    int i;
    int count_memset;

    if ( !ch || memset_name[0] == '\0' )
        return;

    count_memset = 0;

    if ( overwrite_old ) // kasujemy stary memset, czyli kto¶ wywo³a³ "memorize set 'nazwa' update"
      {
	if ( !remove_memset( ch, memset_name, TRUE ) )
	  {
	      print_char( ch, "{nNie pamiêtasz zestawu czarów o takiej nazwie '{G%s{n'.{x\n\r", memset_name);
	      return;
	  }
      }
    else // sprawdzamy czy nie nadpiszemy starego i czy mamy miejsce
      {
	if ( ch->memset )
	  {
	    for ( memset = ch->memset; memset; memset = memset->next ) {
	      if ( !str_cmp( memset->set_name, memset_name ) ) {
                print_char( ch, "{nPamiêtasz ju¿ zestaw czarów o nazwie '{G%s{n'.{x\n\r", memset_name);
                return;
	      }
	      count_memset++;
	    }

	    if ( count_memset == MAX_MEMSETS ) {
	      print_char( ch, "Nie mo¿esz zapamiêtaæ wiêcej zestawów czarów.\n\r");
	      return;
	    }
	  }
      }

    memset = new_memset();

    if ( memset )
    {
        memset->set_name = str_dup(memset_name);

        for ( i = 0; i < MAX_SKILL; i++)
            allspell_cache[ i ] = 0;

        if ( ch->memspell )
            for ( mspell = ch->memspell; mspell; mspell = mspell->next )
                allspell_cache[ mspell->spell ] += 1;

        for ( i = 0; i < MAX_SKILL; i++ )
            memset->set_spells[ i ] = allspell_cache[ i ];

        memset->next = ch->memset;
        ch->memset   = memset;
        print_char( ch, "{nZapamiêtujesz aktualny zestaw czarów pod nazw± '{G%s{n'.{x\n\r", memset_name);

    } else
        print_char( ch, "Nie uda³o cie sie zapamiêtaæ nowego zestawu czarów.\n\r");
}

void load_memset( CHAR_DATA *ch, char *memset_name )
{
    MEMSET_DATA * memset;
    MSPELL_DATA * mspell;
    int i;
    char allspell_cache[MAX_SKILL];
    int mem_differences[MAX_SKILL];

    if ( !ch || memset_name[0] == '\0' )
        return;

    if ( !ch->memset ) {
        print_char( ch, "Aktualnie nie pamiêtasz ¿adnych zestawów czarów.\n\r");
        return;
    }

    for ( memset = ch->memset; memset; memset = memset->next ) {
        if ( !str_cmp( memset->set_name, memset_name ) ) {

            for ( i = 0; i < MAX_SKILL; i++)
                allspell_cache[ i ] = 0;

            if ( ch->memspell )
                for ( mspell = ch->memspell; mspell; mspell = mspell->next )
                    allspell_cache[ mspell->spell ] += 1;

            /* ujemne forget, dodatnie mem */

            for ( i = 0; i < MAX_SKILL; i++ )
                mem_differences[ i ] = memset->set_spells[ i ] - allspell_cache[ i ];

            for ( i = 0; i < MAX_SKILL; i++ ) {
                while ( mem_differences[ i ] < 0 ) {
                    mspell = get_mspell_by_sn( ch, i, FALSE );
                    if ( mspell ) {
                        print_char( ch, "Zaklêcie '%s' usuniête z listy czarów do zapamiêtania.\n\r", skill_table[ i ].name );
                    } else {
                        mspell = get_mspell_by_sn( ch, i, TRUE );
                        print_char( ch, "Zaklêcie '%s' usuniête z listy zapamiêtywanych czarów.\n\r", skill_table[ i ].name );
                    }
                    remove_mem( ch, mspell );
                    mem_differences[i]++;
                }
            }

            for ( i = 0; i < MAX_SKILL; i++ ) {
                while ( mem_differences[ i ] > 0 ) {
                    if ( add_mem( ch, i ) ) {
                        print_char( ch, "Dodajesz zaklêcie '%s' do listy zapamiêtywanych czarów.\n\r", skill_table[ i ].name );
                    } else {
                        print_char(ch, "Nie dasz rady zapamietaæ zaklêcia '%s'.\n\r", skill_table[ i ].name );
                    }
                    mem_differences[i]--;
                }
            }

            print_char( ch, "\n\r{nAktualny zestaw czarów ustawiony zgodnie z zestawem '{G%s{n'.{x\n\r", memset_name);
            return;
        }
    }

    print_char( ch, "{nNie pamiêtasz zestawu czarów o nazwie '{G%s{n'.{x\n\r", memset_name);
}

bool remove_memset( CHAR_DATA *ch, char *memset_name, bool quiet )
{
    MEMSET_DATA * memset;
    MEMSET_DATA * prev_memset;

    if ( !ch || memset_name[0] == '\0' )
        return FALSE;

    if ( !ch->memset ) {
      if ( !quiet )
	{
	  print_char( ch, "Aktualnie nie pamiêtasz ¿adnych zestawów czarów.\n\r");
	}
      return FALSE;
    }

    prev_memset = NULL;
    for ( memset = ch->memset; memset; memset = memset->next ) {
        if ( !str_cmp( memset->set_name, memset_name ) ) {
            if ( !prev_memset ) {
                ch->memset = memset->next;
            } else {
                prev_memset->next = memset->next;
            }
            free_memset( memset );
	    if ( !quiet )
	      {
		print_char( ch, "{nZapominasz zestaw czarów '{G%s{n'.{x\n\r", memset_name);
	      }
            return TRUE;
        }
        prev_memset = memset;
    }

    if ( !quiet )
      {
	print_char( ch, "Nie pamiêtasz takiego zestawu czarów.\n\r");
      }

    return FALSE;
}

void list_memsets( CHAR_DATA *ch )
{
    MEMSET_DATA * memset;
    MSPELL_DATA * mspell;
    int i;
    char allspell_cache[MAX_SKILL];
    int mem_differences[MAX_SKILL][2];
    int circle;
    bool first_tomem_occurence, first_frommem_occurence, first_circle_occurence;

    if ( !ch )
        return;

    if ( !ch->memset ) {
        print_char( ch, "Aktualnie nie pamiêtasz ¿adnych zestawów czarów.\n\r");
        return;
    }

    for ( i = 0; i < MAX_SKILL; i++)
        allspell_cache[ i ] = 0;

    if ( ch->memspell )
        for ( mspell = ch->memspell; mspell; mspell = mspell->next )
            allspell_cache[ mspell->spell ] += 1;

    print_char( ch, "{UAktualnie zapamiêtane zestawy czarów:{x\n\r");

    for ( memset = ch->memset; memset; memset = memset->next ) {
        print_char( ch, "\n\r{nZestaw '{G%s{n':{x", memset->set_name);

        for ( i = 0; i < MAX_SKILL; i++ ) {
            mem_differences[i][0] = memset->set_spells[ i ] - allspell_cache[ i ];
            if ( mem_differences[i][0] != 0 )
                 mem_differences[i][1] = spell_circle( ch, i );
        }

        /* dodatnie do zapamietania, ujemne do zapomnienia */

        first_tomem_occurence = TRUE;
        for ( circle = 1; circle < 10; circle++ ) {
            first_circle_occurence = TRUE;
            for ( i = 0; i < MAX_SKILL; i++ ) {
                if ( (mem_differences[i][0] > 0 ) && (mem_differences[i][1] == circle )) {
                    if ( first_circle_occurence ) {
                        if ( first_tomem_occurence ) {
                            print_char( ch, "\n\r{b==<>===< {GCzary do zapamiêtania{b >===<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x\n\r" );
                            first_tomem_occurence = FALSE;
                        }
                        print_char( ch, "Kr±g %d: ", circle );
                        first_circle_occurence = FALSE;
                    } else
                        print_char( ch, "        " );

                    print_char( ch, "[ {G%d{x]{g%-18s{x\n\r", mem_differences[i][0], skill_table[ i ].name  );
                }
            }
        }

        first_frommem_occurence = TRUE;
        for ( circle = 1; circle < 10; circle++ ) {
            first_circle_occurence = TRUE;
            for ( i = 0; i < MAX_SKILL; i++ ) {
                if ( (mem_differences[i][0] < 0 ) && (mem_differences[i][1] == circle )) {
                    if ( first_circle_occurence ) {
                        if ( first_frommem_occurence ) {
                            print_char( ch, "\n\r{b==<>===< {GCzary do zapomnienia{b  >===<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x\n\r" );
                            first_frommem_occurence = FALSE;
                        }
                        print_char( ch, "Kr±g %d: ", circle );
                        first_circle_occurence = FALSE;
                    } else
                        print_char( ch, "        " );

                    print_char( ch, "[ {r%d{x]{r%-18s{x\n\r", -(mem_differences[i][0]), skill_table[ i ].name  );
                }
            }
        }

        if ( first_tomem_occurence && first_frommem_occurence )
             print_char( ch, "{U(aktualny zestaw)\n\r");

        print_char( ch, "\n\r");
    }
}

