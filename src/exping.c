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
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: exping.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/exping.c $
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
#include "tables.h"
#include "progs.h"
#include "projects.h"

/**
 * put value between 0 and 20, to change exp per level
 *
 *  0 - lowes
 * 20 - highiest
 */
int class_exp_mod[MAX_CLASS] =
{
    18, // CLASS_MAG
     9, // CLASS_CLERIC
     0, // CLASS_THIEF
     3, // CLASS_WARRIOR
     8, // CLASS_PALADIN
    13, // CLASS_DRUID
     1, // CLASS_BARBARIAN
    20, // CLASS_MONK
    20, // CLASS_BARD
    12,  // CLASS_BLACK_KNIGHT
    12, // CLASS_SHAMAN
};

int exp_table[30]=
{
   100, /*1*/
   200,
   450,
   900,
  1200, /*5*/
  1600,
  2000,
  2500,
  3500,
  4500, /*10*/
  5500,
  6500,
  7500,
  8000,
 10000, /*15*/
 12000,
 14000,
 16000,
 18000,
 20000, /*20*/
 23000,
 26000,
 29000,
 33000,
 36000,/*25*/
 39000,
 40000,
 42000,
 44000,
 46000
};

int trophy_check( CHAR_DATA *ch, CHAR_DATA *victim, int members );
int trophy_eval( int count );
void paladin_deed( CHAR_DATA *ch, CHAR_DATA *victim );
void message_when_experience_gain( CHAR_DATA *ch, int experience );

long exp_per_level( CHAR_DATA *ch, int level )
{
    double exp = 500;
    if ( IS_NPC( ch ) || level < 1 || level > LEVEL_HERO )
    {
        return 0;
    }
    if ( level > 1 )
    {
        int i;
        float modifier = 1;
        for ( i = 0; i < level; i++ )
        {
            modifier *= ( 1490 + class_exp_mod[ ch->class ] - i ) / ( (float) 1000 );
        }
        exp *= modifier;
        /**
         * more expirance need
         */
        exp *= 115;
        exp /= 100;
    }
    return (long) exp;
}

#define HP_AVER_0	20
#define HP_AVER_10	25
#define HP_AVER_20	30

int get_base_exp( CHAR_DATA *victim )
{
    MSPELL_DATA * spell;
    int base_exp = 0;
    int hp_mod;
    int percent_mod = 0, circle;

#ifdef INFO
    extern CHAR_DATA *supermob;
    CHAR_DATA *immo = get_char_world( supermob, "razor" );
#endif

    base_exp = exp_table[ URANGE( 0, victim->level - 1, 29 ) ];

#ifdef INFO
    print_char( immo, "BASE_EXP: %d\n\r", base_exp );
#endif

    if ( victim->level > 28 )
        base_exp += 1000 * URANGE( 1, victim->level - 28, 10 );

#ifdef INFO
    print_char( immo, "BASE_EXP: %d\n\r", base_exp );
#endif

    hp_mod = get_max_hp(victim) / UMAX( 1, victim->level );


    if ( victim->level > 20 )
    {
        percent_mod += 2 * ( hp_mod - HP_AVER_20 );
    }
    else if ( victim->level > 10 )
    {
        percent_mod += 2 * ( hp_mod - HP_AVER_10 );
    }
    else
    {
        percent_mod += 2 * ( hp_mod - HP_AVER_0 );
    }

#ifdef INFO
    print_char( immo, "PERCENT_MOD(1): %d\n\r", percent_mod );
#endif

    if ( EXT_IS_SET( victim->off_flags, OFF_BACKSTAB ) )
        percent_mod += 3;
    if ( EXT_IS_SET( victim->off_flags, OFF_BASH ) )
        percent_mod += 5;
    if ( EXT_IS_SET( victim->off_flags, OFF_SLAM ) )
        percent_mod += 5;
    if ( EXT_IS_SET( victim->off_flags, OFF_BERSERK ) )
        percent_mod += 10;
    if ( EXT_IS_SET( victim->off_flags, OFF_DISARM ) )
        percent_mod += 5;
    if ( EXT_IS_SET( victim->off_flags, OFF_DODGE ) )
        percent_mod += 5;
    if ( EXT_IS_SET( victim->off_flags, OFF_FAST ) )
        percent_mod += 5;
    if ( EXT_IS_SET( victim->off_flags, OFF_KICK ) )
        percent_mod += 5;
    if ( EXT_IS_SET( victim->off_flags, OFF_CHARGE ) )
        percent_mod += 3;
    if ( EXT_IS_SET( victim->off_flags, OFF_PARRY ) )
        percent_mod += 5;
    if ( EXT_IS_SET( victim->off_flags, OFF_STUN ) )
        percent_mod += 8;
    if ( EXT_IS_SET( victim->off_flags, OFF_CIRCLE ) )
        percent_mod += 10;
    if ( EXT_IS_SET( victim->off_flags, OFF_WARDANCE ) )
        percent_mod += 10;
    if ( EXT_IS_SET( victim->off_flags, OFF_DAMAGE_REDUCTION ) )
        percent_mod += 8;
    if ( EXT_IS_SET( victim->off_flags, OFF_DAMAGE_REDUCTION ) )
        percent_mod += 3;
    if ( EXT_IS_SET( victim->off_flags, OFF_ONE_HALF_ATTACK ) )
        percent_mod += 2;
    if ( EXT_IS_SET( victim->off_flags, OFF_TWO_ATTACK ) )
        percent_mod += 5;
    if ( EXT_IS_SET( victim->off_flags, OFF_THREE_ATTACK ) )
        percent_mod += 10;
    if ( EXT_IS_SET( victim->off_flags, OFF_FOUR_ATTACK ) )
        percent_mod += 15;
    if ( EXT_IS_SET( victim->off_flags, OFF_CRUSH ) )
        percent_mod += 7;
    if ( EXT_IS_SET( victim->off_flags, OFF_TAIL ) )
        percent_mod += 5;
    if ( EXT_IS_SET( victim->off_flags, OFF_FIRE_BREATH ) )
        percent_mod += 10;
    if ( EXT_IS_SET( victim->off_flags, OFF_FROST_BREATH ) )
        percent_mod += 10;
    if ( EXT_IS_SET( victim->off_flags, OFF_ACID_BREATH ) )
        percent_mod += 10;
    if ( EXT_IS_SET( victim->off_flags, OFF_LIGHTNING_BREATH ) )
        percent_mod += 10;

#ifdef INFO
    print_char( immo, "PERCENT_MOD(2): %d\n\r", percent_mod );
#endif

    for ( circle = 1; circle <= 9 ; circle++ )
        for ( spell = victim->memspell; spell != NULL; spell = spell->next )
        {
            if ( spell->circle != circle )
                continue;

            percent_mod += circle;
        }

#ifdef INFO
    print_char( immo, "PERCENT_MOD(3): %d\n\r", percent_mod );
#endif

    base_exp += base_exp * UMIN( percent_mod, 50 ) / 100;

#ifdef INFO
    print_char( immo, "BASE_EXP(4): %d\n\r", base_exp );
#endif

    return base_exp;
}

int xp_compute( CHAR_DATA *ch, CHAR_DATA *victim, int highest_level, int members )
{
    int xp, base_exp, align;
    int trophy_mod = 100, exp_mod = 100;
    int difference = highest_level - ch->level;
    AFFECT_DATA *taff = NULL; // do playerdeath penalty

    if ( IS_NPC( ch ) )
    {
        return 0;
    }

    /**
     * jezeli obaj to gracze
     */
    if ( !IS_NPC( ch ) && !IS_NPC( victim ) )
    {
        if ( highest_level - victim->level > 5  )
        {
            return 0;
        }
    }

    /**
     *  za skille/spelle/staty
     */
    base_exp = get_base_exp( victim );

    if( IS_NPC( victim ) && victim->pIndexData->exp_multiplier != 0 )
    {
    	base_exp *= victim->pIndexData->exp_multiplier;
    	base_exp /= 100;
    }

    if( !IS_NPC(victim ) )
    {
        base_exp *= 3;
    }

    /**
     * modyfikacje ze wzgledu na roznice poziomu miedzy graczem liczonym,
     * a graczem o najzwyzszym poziomie w grupie
     */
    if ( difference > 0 )
    {
        switch ( difference )
        {
            case 1: exp_mod = 95;break;
            case 2: exp_mod = 90;break;
            case 3: exp_mod = 85;break;
            case 4: exp_mod = 75;break;
            case 5: exp_mod = 50;break;
            case 6: exp_mod = 30;break;
            default: exp_mod = 15;break;
        }
    }
    xp = exp_mod * base_exp;
    xp /= 100;

    /**
     * modyfikacja ze wzgledu na liczebnosc grupy
     */
    switch ( members )
    {
        case 1:
        case 2:
            exp_mod = 100;
            break;
        case 3:
        case 4:
            exp_mod = 95;
            break;
        case 5:
        case 6:
            exp_mod = 90;
            break;
        case 7:
        case 8:
            exp_mod = 80;
            break;
        default:
            exp_mod = UMAX( 40, 120 - members * 5 );
            break;
    }

    xp = exp_mod * xp / 100;

    align = victim->alignment - ch->alignment;

    if ( align < 0 )
    {
        align = -align;
    }

    /**
     * bonus za roznice w aligmencie, tylko wtedy, gdy celem
     * jest mob
     */
    if ( IS_NPC( victim ) && !EXT_IS_SET( victim->act, ACT_NOALIGN ) )
    {
		xp += ( xp * ( align / 4 ) ) / 2000; /* bonus liniowy, max +25% */
    }

    /**
     * obnizamy punkty za zabijanie w kolko tych samych mobow
     */
    trophy_mod = trophy_check( ch, victim, members );
    xp = ( xp * trophy_mod ) / 100;

    /**
     *  Tener: mniej expa za graczy z deathpenalty
     */
    if ( ( taff = affect_find( victim->affected, gsn_playerdeath ) ) != NULL )
    {
        int i = taff->level;
        while ( i >= 0 )
        {
            xp /= 5;
            i--;
        }
    }

    append_file_format_daily
        (
         ch,
         EXP_LOG_FILE,
         "l: %d/%d/%d xp: %d/%d mod: %d/%d",
         ch->level,
         highest_level,
         difference,
         base_exp,
         xp,
         exp_mod,
         trophy_mod
        );

    return xp;
}

/* Tener: zywotnosc artów. [04-01-2009]
   S³owa kluczowe: rt_art, gsn_art_destroy */

/* zyskiwanie przez arty nieco zywotnosci

   przelatujemy przez wszystkie obiekty w inv.
   pool to wskaznik na inta zawierajacego informacje o dostepnych jeszcze punktach.
   recurse_container okresla czy mamy zaglebiac sie w pojemniki */

// ujednolicenie wartosci boolowskich
// 2 != 1
// TO_BOOL( 2 ) == TO_BOOL( 1 )

#define TO_BOOL( x ) ( (x) ? TRUE : FALSE )

bool artefact_prolong_pool( OBJ_DATA *inv, int* pool, bool check_inventory )
{
  bool art_found = FALSE;

  OBJ_DATA * obj;
  AFFECT_DATA * aff;


  for ( obj = inv; obj != NULL; obj = obj->next_content )
    {
      /* tutaj zakladamy ze przedmioty w pojemniku trzymanym w rece maja wear_loc == WEAR_NONE. Dzieki temu przy
	 rekurencyjnym wywolaniu funkcji na kontenerze trzymanym w rece artefakt taki po prostu nie dostanie punkcikow */
      /* sprawdzenie czy przedmiot jest w odpowiedniej czesci ekwipunku */

      if (TO_BOOL(check_inventory) != TO_BOOL((obj->wear_loc == WEAR_NONE)))
	continue;

      /* recurse containers */
      if ( (obj->item_type == ITEM_CONTAINER ) && (obj->contains) && (*pool > 0) ) // istotny jest pierwszy warunek, reszta to optymalizacja
	{
	  bool tmp = artefact_prolong_pool( obj->contains, pool, check_inventory );
	  art_found |= tmp;
	}

      /* find affect */
      aff = affect_find( obj->affected, gsn_art_destroy );
      if ( aff )
	{
	  art_found = TRUE;

	  /* to dla uzyskania rownomiernego (statystycznie) rozlozenia pomiedzy obiektami w ekwipunku */
	  if ( dice(1,5) != 1)
	    continue;

	  int val = UMIN( dice(10,4), *pool );
	  *pool -= val;
	  aff->rt_duration += val;
	  aff->rt_duration = UMIN( aff->rt_duration, MAX_ARTEFACT_RT_DURATION );
	}
    }

  return art_found;

}

#undef TO_BOOL

void artefact_prolong_life( CHAR_DATA* ch, CHAR_DATA* victim, int members_num )
{
            int pool;
	    int total_points;
	    bool art_found;

	    /* to jest bardzo istotny fragment: okre¶la za jakie potwory arty bêd± dostawaæ ¿ywotno¶ci */
	    {
	      int level = victim->level; // by makro wygladalo ladniej
	      total_points = 0;
	      total_points += RT_POINTS_FOR_LEVEL;
	      if ( EXT_IS_SET( victim->act, ACT_BOSS ) )
		{
		  total_points += RT_POINTS_FOR_BOSS;
		}

	      /* za graczy nie ma nic. haha. jak mob przetrzymuje arta to tez nic. */
	      if ( !IS_NPC( victim ) || IS_NPC( ch ) )
		{
		  total_points = 0;
		}

	      total_points /= members_num;
	    }

	    /* uwagi odno¶nie semantyki tej funkcji.

	       na pocz±tku przydzielane s± punkty artefaktom niesionym w inventory.
	       maksymalnie przydzielanych jest 25% punktów. funkcja zag³êbia siê w pojemniki.

	       niewykorzystane punkty u¿yte s± do przed³u¿enia ¿ycia artów które znajduj± siê w u¿ywanym ekwipunku.
	       przechowywane s± one w zmiennej pool - dlatego musimy przekazywaæ j± jako wska¼nik.

	       ca³a reszta punktów przekazywana jest dla artów noszonych przez gracza. ALE: w tym momencie funkcja nie
	       zag³êbia siê ju¿ w pojemniki. Czyli arty noszone w worku trzymanym w rêce NIE dostan± nic.

	       Efektem takiego dzialania funkcji jest tez fakt, ¿e je¿eli w ekwipunku gracza nie ma artów,
	       to nadmiarowe punkty zostan± przekazane na arty zalozone. Ale nie odwrotnie.
	    */

	    if ( !total_points )
	      return;

	    {
	      char buf[MSL];
	      sprintf(buf, "%s : przedluzanie czasu zycia artow. postac: %s. Punkty do przydzielenia [1]: %d", __func__, ch->name, total_points );
	      log_string( buf );
	    }

	    /* przedluzanie zycia artow w ekwipunku */
	    {
	      pool = (total_points * RT_POINTS_FOR_INVENTORY) / 100;
	      total_points -= pool;

	      art_found = FALSE;
	      do
		{
		  if ( artefact_prolong_pool( ch->carrying, &pool, TRUE ) )
		    art_found = TRUE;
		}
	      while ( art_found && (pool > 0) );
	    }

	    {
	      char buf[MSL];
	      sprintf(buf, "%s : przedluzanie czasu zycia artow. postac: %s. Punkty do przydzielenia [2]: %d", __func__, ch->name, (pool + total_points) );
	      log_string( buf );
	    }

	    /* przedluzanie zycia artow zalozonych */
	    {
	      pool += total_points;

	      art_found = FALSE;
	      do
		{
		  if ( artefact_prolong_pool( ch->carrying, &pool, FALSE ) )
		    art_found = TRUE;
		}
	      while ( art_found && (pool > 0) );
	    }

	    {
	      char buf[MSL];
	      sprintf(buf, "%s : przedluzanie czasu zycia artow. postac: %s. Punkty do przydzielenia [3]: %d", __func__, ch->name, pool );
	      log_string( buf );
	    }

}

void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA *gch;
    int xp;
    int members;
    int highest_level;
    CHAR_DATA *rch = NULL;

    save_debug_info("exping.c => group_gain", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );
    /* jesli koles sam siebie zabil (poison albo reflect spell)
     * expa i tak dostaja ci z nim walczyli
     */
    if ( victim == ch )
    {
        for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
        {
            if ( gch != ch && gch->fighting == ch )
            {
                rch = gch;
                break;
            }
        }
        if ( !rch )
        {
            return ;
        }
    }

    members = 0;
    highest_level = 0;

    if ( !rch )
    {
        rch = ch;
    }

    for ( gch = rch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
        if ( is_same_group( gch, rch ) ||
             gch->fighting == victim ||
             ( rch->master && IS_AFFECTED( rch, AFF_CHARM ) && is_same_group( rch->master, gch ) ) ||
             ( gch->master && IS_AFFECTED( gch, AFF_CHARM ) && is_same_group( gch->master, rch ) ) )
        {
            members++;

            if ( highest_level < gch->level )
                highest_level = gch->level;
        }
    }

    if ( members == 0 )
    {
        bug( "Group_gain: members.", members );
        members = 1;
        highest_level = ch->level;
    }

    for ( gch = rch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
        if ( is_same_group( gch, rch ) ||
             gch->fighting == victim ||
             ( rch->master && IS_AFFECTED( rch, AFF_CHARM ) && is_same_group( rch->master, gch ) ) ||
             ( gch->master && IS_AFFECTED( gch, AFF_CHARM ) && is_same_group( gch->master, rch ) ) )
        {
            if ( IS_NPC( gch ) )
            {
                continue;
            }
            xp = xp_compute( gch, victim, highest_level, members );
            xp = UMIN( exp_per_level( gch, gch->level ) / EXP_SINGLE_CAP, xp );

            artefact_prolong_life( gch, victim, members ); // do zwiekszania zywotnosci artefaktow

            if ( !IS_IMMORTAL( gch ) && !IS_NPC( gch ) )
            {
                /* zabija go jego wlasna grupa */
                if ( gch == victim )
                {
                    xp = 0;
                }

                if ( IS_NPC( victim ) && EXT_IS_SET( victim->act, ACT_NO_EXP ) )
                {
                    xp = 0;
                }
                if ( gch == ch && is_affected ( ch, gsn_energy_drain ) )
                {
                    xp = 0;
                }
                if ( xp > 0 )
                {
                    xp = gain_exp( gch, xp, TRUE );
                    paladin_deed( gch, victim );
                    if ( !IS_NPC( gch ) )
                    {
                        if ( IS_NPC( victim ) )
                        {
                            append_file_format_daily( gch, EXP_LOG_FILE,
                                    "-> %d (%d) - exp za zabicie moba [%d], atakuj±cych: [%d]",
                                    gch->exp, xp,
                                    victim->pIndexData->vnum, members );
                        }
                        else
                        {
                            append_file_format_daily( gch, EXP_LOG_FILE,
                                    "-> %d (%d) - exp za zabicie gracza [%s], atakuj±cych: [%d]",
                                    gch->exp, xp,
                                    victim->name, members );
                        }
                    }
                }
                message_when_experience_gain( gch, xp );
            }
        }
    }
    return;
}

int trophy_check( CHAR_DATA *ch, CHAR_DATA *victim, int members )
{
    int i;
    DEBUG_INFO( "trophy_check:" );
    /* jesli mob ch albo gracz victim return */
    if ( !IS_NPC( victim ) || IS_NPC( ch ) )
    {
        return 100;
    }
    /**
     * jesli victim ma level mniejszy nic ch level -7 return 100...
     * nie wpisujemy do trophy
     */
    if ( victim->level < ( ch->level - 7 ) )
    {
        return 100;
    }

    DEBUG_INFO( "trophy_check::trophy_check" );
    for ( i = 0; i < TROPHY_SIZE; i++ )
    {
        if ( !ch->pcdata->trophy[ i ][ 0 ] )
        {
            break;
        }

        /* jest juz wpis */
        if ( ch->pcdata->trophy[ i ][ 0 ] == victim->pIndexData->vnum )
        {
            DEBUG_INFO( "trophy_check::trophy_check::found" );
            ch->pcdata->trophy[ i ][ 1 ] += UMAX ( 1, TROPHY_BASE / members );
            /**
             * bossy liczymy duzo lagodniej i na dodatek liniowo
             */
            if ( IS_NPC( victim ) && EXT_IS_SET( victim->act, ACT_BOSS ) )
            {
                return 100 - ch->pcdata->trophy[ i ][ 1 ] / TROPHY_BASE;
            }
            /**
             * graczom ponizej LEVEL_NEWBIE nie obnizamy punktow doswiadczenia
             */
            if ( ch->level < LEVEL_NEWBIE )
            {
                return 100;
            }
            /**
             * za trophy mniejsze od TROPHY_BASE nie naliczamy
             */
            if ( ch->pcdata->trophy[ i ][ 1 ] < TROPHY_BASE )
            {
                return 100;
            }
            /**
             * na sam koniec, policzmy ile zabrac za trophy
             */
            return trophy_eval( ch->pcdata->trophy[ i ][ 1 ] );
        }
    }
    /* w [i] jest ostatni element... albo TROPHY_SIZE */
    DEBUG_INFO( "trophy_check::move_elements" );
    for ( i = TROPHY_SIZE - 1 ; i > 0; i-- )
    {
        ch->pcdata->trophy[ i ][ 0 ] = ch->pcdata->trophy[ i - 1 ][ 0 ];
        ch->pcdata->trophy[ i ][ 1 ] = ch->pcdata->trophy[ i - 1 ][ 1 ];
    }

    ch->pcdata->trophy[ 0 ][ 0 ] = victim->pIndexData->vnum;
    ch->pcdata->trophy[ 0 ][ 1 ] = UMAX ( 1, TROPHY_BASE / members );
    return 100;
}

/* wyliczamy sciecie expa ze wzgledu na ilosc killsow [procentowo]
 * pieprze to, wale bezczelnego switcha, nie chce mi sie rysowac krzywych
 * i bawic sie w przyblizenia liniowe
 */
int trophy_eval( int count )
{
    count /= TROPHY_SIZE;
    switch ( count )
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10: return 100;
        case 11: return 99;
        case 12: return 98;
        case 13: return 97;
        case 14: return 96;
        case 15: return 95;
        case 16: return 93;
        case 17: return 90;
        case 18: return 85;
        case 19: return 75;
        case 20: return 65;
        case 21: return 50;
        case 22: return 30;
        case 23: return 10;
        default: return 5;
    }
}

void do_levels( CHAR_DATA *ch, char *argument )
{
    int level;
    long level_exp_now, level_exp_previous;

    if ( IS_NPC( ch ) )
    {
        return ;
    }

    for ( level = 0; level < LEVEL_HERO; level++ )
    {
        if ( ch->level == level + 1 )
        {
            print_char( ch, "{R" );
        }
        if ( level < 10 )
        {
            print_char( ch, "%d\t%s{x\n\r", level + 1, level_info_common[ level ] );
        }
        else
        {
            switch ( ch->class )
            {
                case CLASS_MAG:
                    print_char( ch, "%d\t%s{x\n\r", level + 1, level_info_mag[ level - 10 ] );
                    break;
                case CLASS_CLERIC:
                    print_char( ch, "%d\t%s{x\n\r", level + 1, level_info_cleric[ level - 10 ] );
                    break;
                case CLASS_THIEF:
                    print_char( ch, "%d\t%s{x\n\r", level + 1, level_info_thief[ level - 10 ] );
                    break;
                case CLASS_WARRIOR:
                    print_char( ch, "%d\t%s{x\n\r", level + 1, level_info_warrior[ level - 10 ] );
                    break;
                case CLASS_PALADIN:
                    print_char( ch, "%d\t%s{x\n\r", level + 1, level_info_paladin[ level - 10 ] );
                    break;
                case CLASS_DRUID:
                    print_char( ch, "%d\t%s{x\n\r", level + 1, level_info_druid[ level - 10 ] );
                    break;
                case CLASS_BARBARIAN:
                    print_char( ch, "%d\t%s{x\n\r", level + 1, level_info_barbarian[ level - 10 ] );
                    break;
                case CLASS_MONK:
                    print_char( ch, "%d\t%s{x\n\r", level + 1, level_info_monk[ level - 10 ] );
                    break;
                case CLASS_BARD:
                    print_char( ch, "%d\t%s{x\n\r", level + 1, level_info_bard[ level - 10 ] );
                    break;
                case CLASS_BLACK_KNIGHT:
                    print_char( ch, "%d\t%s{x\n\r", level + 1, level_info_black_knight[ level - 10 ] );
                    break;
                case CLASS_SHAMAN:
                    print_char( ch, "%d\t%s{x\n\r", level + 1, level_info_shaman[ level - 10 ] );
                    break;
                default:
                    log_string( "cos nie tak w do_score, w switchu od info o klasie" );
            }
        }
    }

    return;

    print_char( ch, "Liczba punktów do¶wiadczenia dla profesji: %s\n\r", ch->sex == 2 ? class_table[ ch->class ].female_name : class_table[ ch->class ].name );

    level_exp_previous = 0;

    for ( level = 1; level < LEVEL_HERO ; level++ )
    {
        level_exp_now = exp_per_level ( ch, level );

        print_char
            (
             ch,
             "Poziom %2d: %9ld-%-9ld ( %8ld)\n\r",
             level,
             level_exp_previous,
             level_exp_now - 1,
             level_exp_now - 1 - level_exp_previous
            );
        level_exp_previous = level_exp_now;
    }

    return ;
}

void paladin_deed( CHAR_DATA *ch, CHAR_DATA *victim )
{
    AFFECT_DATA af, *paf;
    int pomocnicza;
    int hs;
    if ( IS_NPC( ch ) || ch->class != CLASS_PALADIN )
        return;

    if ( !is_affected( ch, gsn_prayer_last ) )
        return;

    if ( !is_affected( ch, gsn_deeds_pool ) )
    {
        af.where    = TO_AFFECTS;
        af.type        = gsn_deeds_pool;
        af.level    = 50;
        af.duration = -1;
        af.rt_duration = 0;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = &AFF_NONE;
        af.visible     = FALSE;

        affect_to_char( ch, &af, NULL, FALSE );
    }
    hs = get_skill(ch,gsn_holy_prayer);
    paf = affect_find( ch->affected, gsn_deeds_pool );
 
 if ( IS_EVIL( victim ) )
    {
        if ( number_percent() < victim->level * 2  &&  paf->modifier < 30 )
        {
           pomocnicza = ch->level - victim->level;
           if (pomocnicza > 5) pomocnicza = 1;
           if (pomocnicza < -3) pomocnicza = 7;
           if (-3 <= pomocnicza && pomocnicza <= 3) pomocnicza = 4;

           if (victim->level > 19) pomocnicza += 3;
           if (victim->level > 26) pomocnicza += 3;
           if (victim->level > 30) pomocnicza += 5;
           paf->modifier += pomocnicza;
           if (paf->modifier > 30) paf->modifier = 30;
          //print_char( ch, "Pomocnicza: %d\n\r", pomocnicza );
           //print_char( ch, "Ilosc deedow: %d\n\r", paf->modifier );
           switch( pomocnicza )
        {
            case 0:
            case 1:
                  send_to_char( "Zabicie tego przeciwnika spowodowa³o, ¿e czujesz siê trochê pewniej.\n\r", ch );
                  break;
            case 2:
            case 3:
            case 5:
                  send_to_char( "Zabicie tego pomiotu z³a wype³nia ciê si³± i pewnosci± siebie.\n\r", ch );
                  break;

           default:
                  send_to_char( "Czujesz jak zabicie tego pomiotu z³a wype³nia ciê swiêt± moc±.\n\r", ch );
                  break;
        }

        }
    }
    if ( IS_GOOD( victim ) )
    {
        paf->modifier-=2;
        if (hs > 10) paf->modifier-=2;
        if (hs > 20) paf->modifier-=2;
        if (hs > 30) paf->modifier-=2;
        if (hs > 40) paf->modifier-=2;
        if (hs > 50) paf->modifier-=2;
        if (hs > 60) paf->modifier-=2;
        if (hs > 70) paf->modifier-=2;
        if (hs > 80) paf->modifier-=2;
        if (hs > 90) paf->modifier-=2;

        if (paf->modifier < -60) paf->modifier = -60;
        send_to_char( "Zabicie tej niewinnej istoty sprawia, ¿e czujesz siê podle.\n\r", ch );
       // print_char( ch, "Ilosc deedow: %d\n\r", paf->modifier );
      
    }


}
void message_when_experience_gain( CHAR_DATA *ch, int experience )
{
    if ( experience > 0 )
    {
        if ( experience * ( EXP_SINGLE_CAP + 2 ) > exp_per_level( ch, ch->level ) )
        {
            send_to_char( "{GZdobywasz du¿o punktów do¶wiadczenia.", ch );
        }
        else
        {
            if ( experience * 1000 >  exp_per_level( ch, ch->level ) )
            {
                send_to_char( "{g", ch );
            }
            send_to_char( "Zdobywasz punkty do¶wiadczenia.", ch );
        }
    }
    else
    {
        send_to_char( "{R", ch );
        switch ( number_range( 1, 2 ) )
        {
            case 1:
                send_to_char( "Nie zdobywasz punktów do¶wiadczenia.", ch );
                break;
            case 2:
            default:
                send_to_char( "Nie nauczy³<&e¶/a¶/o¶> siê niczego nowego.\n\r", ch );
                break;
        }
    }
    send_to_char( "{x\n\r", ch );
}

/**
 * funkcja zwraca liczbe od 0 do 9 w zaleznosci od posiadanych punktow
 * doswiadczenia
 */
int get_percent_exp ( CHAR_DATA *ch )
{
    long level_next          = exp_per_level( ch, ch->level );
    long level_previous      = exp_per_level( ch, ch->level -1 );
    long level_base          = ( level_next - level_previous ) / 10 ;
    long experience_to_level = level_next - ch->exp;
    if ( level_base > 0 )
    {
        return (int) experience_to_level / level_base;
    }
    return 0;
}

