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
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: special.c 11460 2012-06-29 07:43:35Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/special.c $
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
#include "interp.h"
#include "magic.h"
#include "money.h"

/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN( spec_executioner  );
DECLARE_SPEC_FUN( spec_fido         );
DECLARE_SPEC_FUN( spec_guard        );
DECLARE_SPEC_FUN( spec_janitor      );
DECLARE_SPEC_FUN( spec_poison       );
DECLARE_SPEC_FUN( spec_thief        );
DECLARE_SPEC_FUN( spec_nasty        );
DECLARE_SPEC_FUN( spec_troll_member );
DECLARE_SPEC_FUN( spec_ogre_member  );
DECLARE_SPEC_FUN( spec_patrolman    );
DECLARE_SPEC_FUN( spec_hunter		);

CHAR_DATA *get_random_char	args( ( CHAR_DATA *mob, bool PC ) );
void	raw_damage		args( (CHAR_DATA *ch, CHAR_DATA *victim, int dam ) );
bool	check_increase_wounds	args( (CHAR_DATA *victim, int dam_type, int *dam) );

/* the function table */
const struct spec_type spec_table[] =
{
    {   "spec_executioner",     spec_executioner    },
    {   "spec_fido",            spec_fido           },
    {   "spec_guard",           spec_guard          },
    {   "spec_janitor",         spec_janitor        },
    {   "spec_poison",          spec_poison         },
    {   "spec_thief",           spec_thief          },
    {   "spec_nasty",           spec_nasty          },
    {   "spec_troll_member",    spec_troll_member   },
    {   "spec_ogre_member",     spec_ogre_member    },
    {   "spec_patrolman",       spec_patrolman      },
    {   "spec_hunter",          spec_hunter         },
    {   NULL,                   NULL                }
};

/*
 * Given a name, return the appropriate spec fun.
 */
SPEC_FUN *spec_lookup( const char *name )
{
    int i;

    for ( i = 0; spec_table[ i ].name != NULL; i++ )
    {
        if ( NOPOL( name[ 0 ] ) == NOPOL( spec_table[ i ].name[ 0 ] )
             && !str_prefix( name, spec_table[ i ].name ) )
            return spec_table[ i ].function;
    }

    return 0;
}

char *spec_name( SPEC_FUN *function )
{
    int i;

    for ( i = 0; spec_table[ i ].function != NULL; i++ )
    {
        if ( function == spec_table[ i ].function )
            return spec_table[ i ].name;
    }

    return NULL;
}

bool spec_troll_member( CHAR_DATA *ch )
{
    CHAR_DATA * vch, *victim = NULL;
    int count = 0;
    char *message;

    if ( !IS_AWAKE( ch ) || IS_AFFECTED( ch, AFF_CALM ) || ch->in_room == NULL
         || IS_AFFECTED( ch, AFF_CHARM ) || ch->fighting != NULL )
        return FALSE;

    /* find an ogre to beat up */
    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
        if ( !IS_NPC( vch ) || ch == vch )
            continue;

        if ( !str_cmp( race_table[ GET_RACE(vch) ].name, "ogr" )
             && ch->level > vch->level - 2 && !is_safe( ch, vch, TRUE ) )
        {
            if ( number_range( 0, count ) == 0 )
                victim = vch;

            count++;
        }
    }

    if ( victim == NULL )
        return FALSE;

    /* say something, then raise hell */
    switch ( number_range( 0, 6 ) )
    {
        default: message = NULL; break;
        case 0: message = "$n wrzeszczy 'Szuka³em ciê, ¶mieciu!'";
            break;
        case 1: message = "$n atakuje $C z szaleñczym wrzaskiem.";
            break;
        case 2: message =
                "$n mówi 'Co taki ¶mierdz±cy ogrzy pomiot jak ty tu w ogóle robi?'";
            break;
        case 3: message = "$n wy³amuje palce a¿ trzeszczy i mówi 'My¶lisz, ¿e masz szczêæie?'";
            break;
        case 4: message = "$n mówi 'Nie widzê tu stra¿ników którzy mogliby ciê ochroniæ tym razem!'";
            break;
        case 5: message = "$n mówi 'Czas do³±czyæ do swoich durnych braci w za¶wiatach.'";
            break;
        case 6: message = "$n mówi 'Gra muzyka...'";
            break;
    }

    if ( message != NULL )
        act( message, ch, NULL, victim, TO_ALL );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return TRUE;
}

bool spec_ogre_member( CHAR_DATA *ch )
{
    CHAR_DATA * vch, *victim = NULL;
    int count = 0;
    char *message;

    if ( !IS_AWAKE( ch ) || IS_AFFECTED( ch, AFF_CALM ) || ch->in_room == NULL
         || IS_AFFECTED( ch, AFF_CHARM ) || ch->fighting != NULL )
        return FALSE;

    /* find an troll to beat up */
    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
        if ( !IS_NPC( vch ) || ch == vch )
            continue;

        if ( !str_cmp( race_table[ GET_RACE(vch) ].name, "troll" )
             && ch->level > vch->level - 2 && !is_safe( ch, vch, TRUE ) )
        {
            if ( number_range( 0, count ) == 0 )
                victim = vch;

            count++;
        }
    }

    if ( victim == NULL )
        return FALSE;

    /* say something, then raise hell */
    switch ( number_range( 0, 6 ) )
    {
        default: message = NULL;	break;
        case 0: message = "$n wrzeszczy 'Szuka³em ciê, ¶mieciu!'";
            break;
        case 1: message = "$n atakuje $C z szaleñczym wrzaskiem.";
            break;
        case 2: message =
                "$n mówi 'Co taki ¶mierdz±cy, trollowaty pomiot jak ty tu w ogóle robi?'";
            break;
        case 3: message = "$n wy³amuje palce a¿ trzeszcz± i mówi 'My¶lisz, ¿e masz szczê¶cie?'";
            break;
        case 4: message = "$n mówi 'Nie widzê tu stra¿ników którzy mogliby ciê ochroniæ tym razem!'";
            break;
        case 5: message = "$n mówi 'Czas do³±czyæ do swoich durnych braci w za¶wiatach.'";
            break;
        case 6: message = "$n mówi 'I niech gra muzyka...'";
            break;
    }

    if ( message != NULL )
        act( message, ch, NULL, victim, TO_ALL );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return TRUE;
}

bool spec_patrolman( CHAR_DATA *ch )
{
    CHAR_DATA * vch, *victim = NULL;
    OBJ_DATA *obj;
    char *message;
    int count = 0;

    if ( !IS_AWAKE( ch ) || IS_AFFECTED( ch, AFF_CALM ) || ch->in_room == NULL
         || IS_AFFECTED( ch, AFF_CHARM ) || ch->fighting != NULL || !can_move( ch ) )
        return FALSE;

    /* look for a fight in the room */
    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
        if ( vch == ch )
            continue;

	/*No to tak
			- bije sie gracz vs gracz - nic nie robi
			- bije sie mob vs mob - nic nie robi
			- bije sie gracz vs mob z masterem albo charmem - nic nie robi
			- bije sie gracz vs mob z aggressivem - bije moba
			- bije sie gracz vs mob samotny - bije gracza*/

        if ( vch->fighting != NULL )   /* break it up! */
        {
            if ( number_range( 0, count ) == 0 )
            {
            	if ( ( IS_NPC(vch) && !IS_NPC(vch->fighting ) ) || ( !IS_NPC(vch) && IS_NPC(vch->fighting ) ) )
            	{
            		if ( IS_NPC(vch) )
            		{
            			if( EXT_IS_SET( vch->act, ACT_AGGRESSIVE ) && vch->master == NULL && !IS_AFFECTED(vch, AFF_CHARM ) )
            				victim = vch;
            			else if( vch->master == NULL && !IS_AFFECTED(vch, AFF_CHARM))
            				victim = vch->fighting;
            			else
            				victim = NULL;
            		}
            		else if ( IS_NPC(vch->fighting) )
            		{
            			if( EXT_IS_SET( vch->fighting->act, ACT_AGGRESSIVE ) && vch->fighting->master == NULL && !IS_AFFECTED(vch->fighting,AFF_CHARM) )
            				victim = vch->fighting;
            			if( vch->fighting->master == NULL && !IS_AFFECTED(vch->fighting,AFF_CHARM))
            				victim = vch;
            			else
            				victim = NULL;
            		}
            	}
				else
					victim = NULL;
			}
            count++;
        }
        else if ( EXT_IS_SET( vch->act, ACT_AGGRESSIVE ) && vch->master == NULL && !IS_AFFECTED(vch, AFF_CHARM ) )
        {
        	victim = vch;
        }
    }

    if ( victim == NULL || IS_NPC( victim ))
        return FALSE;

    if ( ( ( obj = get_eq_char( ch, WEAR_NECK_1 ) ) != NULL
           && obj->pIndexData->vnum == OBJ_VNUM_WHISTLE )
         || ( ( obj = get_eq_char( ch, WEAR_NECK_2 ) ) != NULL
              && obj->pIndexData->vnum == OBJ_VNUM_WHISTLE ) )
    {
        act( "Nabierasz g³êboko powietrza w p³uca i mocno dmuchasz w $g.", ch, obj, NULL, TO_CHAR );
        act( "$n mocno dmucha w $g, ***PFFFIIIIIIIIIIIIT***", ch, obj, NULL, TO_ROOM );

        for ( vch = char_list; vch != NULL; vch = vch->next )
        {
            if ( vch->in_room == NULL )
                continue;

            if ( vch->in_room != ch->in_room
                 && vch->in_room->area == ch->in_room->area )
                send_to_char( "S³yszysz dono¶ny, ¶wiszcz±cy d¼wiêk.\n\r", vch );
        }
    }

    switch ( number_range( 0, 6 ) )
    {
        default: message = NULL;	break;
        case 0: message = "$n wrzeszczy 'Co to za bójki?! Przerwaæ natychymiast tê walkê!'";
            break;
        case 1: message = "$n mówi 'To okropne ¿yæ w czasach, kiedy króluje chaos i przemoc...'";
            break;
        case 2: message = "$n mamrocze 'Cholerne dzieciaki, wszystko za³atwia³yby przemoc±!'";
            break;
        case 3: message = "$n wrzeszczy 'Natychmiast przestañ! Natychmiast przestaæ!!' i atakuje.";
            break;
        case 4: message = "$n prostuje siê, chrz±ka i idzie do pracy.";
            break;
        case 5: message = "$n wzdycha z rezygnacj± i próbuje przerwaæ walkê.";
            break;
        case 6: message = "$n mówi 'Uspokójcie siê! Banda ³obuzów!'";
            break;
    }

    if ( message != NULL )
        act( message, ch, NULL, NULL, TO_ALL );

    multi_hit( ch, victim, TYPE_UNDEFINED );

    return TRUE;
}


bool spec_nasty( CHAR_DATA *ch )
{
    CHAR_DATA * victim, *v_next;
    long copper;

    if ( !IS_AWAKE( ch ) )
    {
        return FALSE;
    }

    if ( ch->position != POS_FIGHTING )
    {
        for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
        {
            v_next = victim->next_in_room;
            if ( !IS_NPC( victim )
                 && ( victim->level > ch->level )
                 && ( victim->level < ch->level + 10 ) )
            {
                do_function( ch, &do_backstab, victim->name );
                if ( ch->position != POS_FIGHTING )
                {
                    do_function( ch, &do_kill, victim->name );
                }

                /* should steal some coins right away? :) */
                return TRUE;
            }
        }
        return FALSE;    /*  No one to attack */
    }

/* okay, we must be fighting... steal some coins and flee */
    if ( ( victim = ch->fighting ) == NULL )
    {
        return FALSE;   /* let's be paranoid... */
    }

    copper = money_count_copper( victim );
    if ( copper == 0 )
    {
        return FALSE;
    }

    switch ( number_bits( 2 ) )
    {
        case 0:
            act( "$n wpada na ciebie i zabiera twoj± sakiewkê!", ch, NULL, victim, TO_VICT );
            if ( ch->sex == 0 )
            {
                act( "Wpadasz na $N i zabierasz temu sakiewkê.", ch, NULL, victim, TO_CHAR );
                act( "$n wpada na $Z i zabiera temu sakiewke!", ch, NULL, victim, TO_NOTVICT );
            }
            else if ( ch->sex == 1 )
            {
                act( "Wpadasz na $N i zabierasz mu sakiewkê.", ch, NULL, victim, TO_CHAR );
                act( "$n wpada na $Z i zabiera mu sakiewke!", ch, NULL, victim, TO_NOTVICT );
            }
            else
            {
                act( "Wpadasz na $N i zabierasz jej sakiewkê.", ch, NULL, victim, TO_CHAR );
                act( "$n wpada na $Z i zabiera jej sakiewke!", ch, NULL, victim, TO_NOTVICT );
            }
            money_transfer_spec_nasty ( ch, victim );
            return TRUE;

        case 1:
            do_function( ch, &do_flee, "" );
            return TRUE;

        default:
            return FALSE;
    }
}

/*
 * Core procedure for dragons.
 */
bool dragon( CHAR_DATA *ch, char *spell_name )
{
    CHAR_DATA * victim;
    CHAR_DATA *v_next;
    int sn;

    if ( ch->position != POS_FIGHTING )
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits( 3 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;

    if ( ( sn = skill_lookup( spell_name ) ) < 0 )
        return FALSE;
    ( *skill_table[ sn ].spell_fun ) ( sn, ch->level, ch, victim, TARGET_CHAR );
    return TRUE;
}

bool spec_executioner( CHAR_DATA *ch )
{
    char buf[ MAX_STRING_LENGTH ];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

    if ( !IS_AWAKE( ch ) || ch->fighting != NULL )
        return FALSE;

    crime = "";
    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;

        if ( !IS_NPC( victim ) && EXT_IS_SET( victim->act, PLR_KILLER )
             && can_see( ch, victim ) )
        {
            crime = "KILLER"; break;
        }

        if ( !IS_NPC( victim ) && EXT_IS_SET( victim->act, PLR_THIEF )
             && can_see( ch, victim ) )
        {
            crime = "THIEF"; break;
        }
    }

    if ( victim == NULL )
        return FALSE;

    sprintf( buf, "%s is a %s!  BRONIÆ NIEWINNYCH!  KRRRRWII!!!",
             victim->name, crime );
    REMOVE_BIT( ch->comm, COMM_NOSHOUT );
    do_function( ch, &do_yell, buf );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return TRUE;
}

bool spec_fido( CHAR_DATA *ch )
{
    OBJ_DATA *corpse;
    OBJ_DATA *c_next;

    if ( !IS_AWAKE( ch ) )
        return FALSE;

	if ( ch->fighting != NULL )
		return FALSE;

    for ( corpse = ch->in_room->contents; corpse != NULL; corpse = c_next )
    {
        c_next = corpse->next_content;
        if ( corpse->item_type != ITEM_CORPSE_NPC && corpse->item_type != ITEM_CORPSE_PC )
            continue;

        act( "Nagle $n otwiera szeroko sw± pszczê i po¿era $h!", ch, corpse, NULL, TO_ROOM );
        extract_obj( corpse );
        return TRUE;
    }

    return FALSE;
}

bool spec_guard( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    CHAR_DATA *ech;
    char *crime;
    int max_evil;

    if ( !IS_AWAKE( ch ) || ch->fighting != NULL )
        return FALSE;

    max_evil = 300;
    ech = NULL;
    crime = "";

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;

        if ( victim->fighting != NULL
        &&   victim->fighting != ch
        &&   victim->fighting != victim
        &&   victim->spec_fun != spec_guard
        &&	!IS_NPC( victim )
        &&   victim->alignment < max_evil )
        {
            max_evil = victim->alignment;
            ech = victim;
        }
    }

    if ( ech != NULL )
    {
        act( "$n krzyczy 'Chroniæ niewinnych!!  Broñ siê $N!!", ch, NULL, ech, TO_ROOM );
        multi_hit( ch, ech, TYPE_UNDEFINED );
        return TRUE;
    }

    return FALSE;
}

bool spec_janitor( CHAR_DATA *ch )
{
    OBJ_DATA * trash;
    OBJ_DATA *trash_next;

    if ( !IS_AWAKE( ch ) )
        return FALSE;

    for ( trash = ch->in_room->contents; trash != NULL; trash = trash_next )
    {
        trash_next = trash->next_content;
        if ( !IS_SET( trash->wear_flags, ITEM_TAKE ) )
            continue;
        if ( trash->item_type == ITEM_DRINK_CON
             || trash->item_type == ITEM_TRASH
             || trash->item_type == ITEM_FOOD
             || trash->cost < 40 )
        {
            act( "$n podnosi jakie¶ ¶miecie.", ch, NULL, NULL, TO_ROOM );
            obj_from_room( trash );
            obj_to_char( trash, ch );
            return TRUE;
        }
    }

    return FALSE;
}

bool spec_poison( CHAR_DATA *ch )
{
    return FALSE;
/*
    CHAR_DATA * victim;
    int liczba;


    victim = get_random_char( ch, TRUE );

	for( liczba = 20; victim == ch && liczba > 0; --liczba)
		victim = get_random_char( ch, TRUE );

	if ( victim == ch )
		return FALSE;

	if ( number_percent() > ch->level * 3 )
		return FALSE;

    act( "Gryziesz $Z!", ch, NULL, victim, TO_CHAR );
    act( "$n gryzie $Z!", ch, NULL, victim, TO_NOTVICT );
    act( "$n gryzie ciê!", ch, NULL, victim, TO_VICT );
    poison_to_char( victim, ch->level/10 );
    return TRUE;*/
}

bool spec_thief( CHAR_DATA *ch )
{
    CHAR_DATA * victim;
    CHAR_DATA *v_next;

    if ( ch->position != POS_STANDING )
    {
        return FALSE;
    }

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;

        if ( IS_NPC( victim )
             || is_safe( ch, victim, TRUE )
             || victim->level >= LEVEL_IMMORTAL
             || number_bits( 5 ) != 0
             || !can_see( ch, victim )
	     || money_count_copper( victim ) == 0 )
        {
            continue;
        }

        if ( IS_AWAKE( victim ) && number_range( 0, ch->level ) == 0 )
        {
            act( "Odkrywasz rêke $z w swojej kieszeni!", ch, NULL, victim, TO_VICT );
            act( "$N odkrywa rêke $z w swojej kieszeni!", ch, NULL, victim, TO_NOTVICT );
            return TRUE;
        }
        else
        {
            money_steal( ch, victim );
            return TRUE;
        }
    }

    return FALSE;
}

bool spec_hunter( CHAR_DATA *ch )
{
    CHAR_DATA * vch, *victim = NULL;
    int dam, chance;

    if ( !IS_AWAKE( ch ) || IS_AFFECTED( ch, AFF_CALM ) || ch->in_room == NULL
         || IS_AFFECTED( ch, AFF_CHARM ) || ch->fighting != NULL )
        return FALSE;

	if ( (ch->hit*100)/get_max_hp( ch ) < 50 )
		return FALSE;

    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
        if ( !IS_NPC( vch ) || ch == vch )
            continue;

		if( is_safe( ch, vch, TRUE ) )
			continue;

		if( vch->fighting )
			continue;

		if( !can_see( ch, vch ) )
			continue;

        if ( ch->level - 2 < vch->level )
        	continue;

        if ( IS_AFFECTED( vch, AFF_CHARM ) || vch->master )
        	continue;

        if ( IS_SET( race_table[ GET_RACE( vch ) ].type, ANIMAL ) )
        {
        	victim = vch;
        	break;
        }
        continue;
    }

    if ( victim == NULL )
        return FALSE;

	chance = 65;
	chance += ch->level*2;
	chance -= victim->level*2;
	chance += get_curr_stat_deprecated(ch,STAT_DEX)-10;
	chance -= get_curr_stat_deprecated(victim,STAT_DEX)-10;

	if( victim->size == SIZE_TINY )
		chance /= 4;
	else if( victim->size == SIZE_SMALL )
		chance /= 2;
	else if( victim->size == SIZE_LARGE )
		chance += chance/2;
	else if( victim->size == SIZE_HUGE )
		chance *= 2;
	else if( victim->size == SIZE_GIANT )
		chance *= 4;

	if( number_percent() > chance )
	{
		act( "$n wychyla siê nieco z ukrycia, wymierza ze swojego ³uku w $C, po czym strzela. Strza³a jednak przelatuje kilka centymetrów obok celu.", ch, NULL, victim, TO_ALL );
		multi_hit( victim, ch, TYPE_UNDEFINED );
        return TRUE;
	}

	dam = ch->level * 7;
	dam += (get_curr_stat_deprecated(ch, STAT_DEX)-10)*3;
	dam += (get_curr_stat_deprecated(ch, STAT_STR)-10)*3;
	dam += (get_curr_stat_deprecated(ch, STAT_LUC)-10);
	dam -= (get_curr_stat_deprecated(victim, STAT_CON)-10)*5;
	dam -= (get_curr_stat_deprecated(victim, STAT_LUC)-10);

	check_increase_wounds( victim, DAM_PIERCE, &dam );
	dam = check_resist( victim, DAM_PIERCE, dam );

	if( victim->hit - dam > -10 )
	{
	        bug("FIXME: uwzglêdnienie deflect wounds w sposób nie-bugogenny",0);
		if( victim->sex == SEX_FEMALE )
	    	act( "$n wychyla siê nieco z ukrycia, wymierza ze swojego ³uku w $C, po czym strzela trafiaj±c j± w tu³ów. Strza³a przelatuje na wylot, a ze zranionego korpusu $Z bucha czerwona posoka.", ch, NULL, victim, TO_ALL );
	    else
	    	act( "$n wychyla siê nieco z ukrycia, wymierza ze swojego ³uku w $C, po czym strzela trafiaj±c go w tu³ów. Strza³a przelatuje na wylot, a ze zranionego korpusu $Z bucha czerwona posoka.", ch, NULL, victim, TO_ALL );
	}
	else
	{
		if( victim->sex == SEX_FEMALE )
	    	act( "$n wychyla siê nieco z ukrycia, wymierza ze swojego ³uku w $C, po czym strzela trafiaj±c j± prosto w serce. Strza³a przelatuje na wylot, a z rozerwanych têtnic $Z bucha strumieniami czerwona posoka. $N chwieje siê przez chwilê po czym wywraca na ziemiê i nieruchomieje.", ch, NULL, victim, TO_ALL );
	    else
	    	act( "$n wychyla siê nieco z ukrycia, wymierza ze swojego ³uku w $C, po czym strzela trafiaj±c go prosto w serce. Strza³a przelatuje na wylot, a z rozerwanych têtnic $Z bucha strumieniami czerwona posoka. $N chwieje siê przez chwilê po czym wywraca na ziemiê i nieruchomieje.", ch, NULL, victim, TO_ALL );
	}

	raw_damage( ch, victim, UMAX( 30, dam ) );
	multi_hit( victim, ch, TYPE_UNDEFINED );

    return TRUE;
}
