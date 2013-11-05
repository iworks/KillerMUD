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
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Skrzetnicki Krzysztof (gtener@gmail.com              ) [Tener     ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: spells_dru.c 11502 2012-07-27 10:34:29Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/12.02/src/spells_dru.c $
 *
 */
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
#include "projects.h"
#include "money.h"

/*
 *
 * CZARY DRUIDYCZNE
 *
 */

extern char *	target_name;
bool saves_dispel	args( ( int dis_level, int spell_level, int duration ) );
bool check_dispel	args( ( int dis_level, CHAR_DATA *victim, int sn ) );
void set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	raw_kill	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
bool		check_blink	args( (CHAR_DATA *victim, CHAR_DATA *ch, bool unik) );
bool	remove_obj	args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );

/* lanca jednoro¿ca  cast 'alicorn lance' [cel]
 * utworzone: 24 marca 2002 - gurthg
 */
void spell_alicorn_lance( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dam, duration, modifier, save_mod = 2;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );
	bool use_components = spell_item_check( ch, sn, NULL );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) < 1 )
	{
		act( "Niematarialna lanca mija $c.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Srebrzysta, niematerialna lanca mija ciê w niewielkiej odleg³o¶ci!\n\r", victim );
		return;
	}

	dam = dice( 3, 12 ) + level / 4;
	dam = luck_dam_mod( ch, dam );
	if ( dam < 1 ) dam = 1;

	duration = 1 + level / 10;
	if ( number_range( 0, luck ) > 10 ) ++duration;
	if ( number_range( 0, luck ) < 3 ) --duration;
	if ( duration < 1 ) duration = 1;

	modifier = 12 + level / 7.5;
	if ( number_range( 0, luck ) > 10 ) ++modifier;
	if ( number_range( 0, luck ) < 3 ) --modifier;

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		if ( number_range( 0, get_curr_stat_deprecated( victim, STAT_LUC ) ) > 10 ) save_mod = 3;
		dam /= save_mod;
		modifier /= save_mod;
		duration /= save_mod;
	}

    if ( IS_NPC ( ch ) && ( ch->level > 25 ) && ( ch->level/6 ) > number_percent() )
    {
       use_components = TRUE;
       dam /= 2;
    }
    else if(IS_NPC ( ch ))
    {
    	use_components = FALSE;
    }

    if ( use_components )
    {
        act( "Srebrzysta, matarialna lanca uderza w $c.", victim, NULL, NULL, TO_ROOM );
        send_to_char( "Srebrzysta, materialna lanca uderza w twoje cia³o!\n\r", victim );
    }
    else
    {
        print_char( ch, "Wyzwalasz zaklêcie nie pos³uguj±c siê magicznym przedmiotem.\n\r" );
        act( "Srebrzysta, niematarialna lanca uderza w $c.", victim, NULL, NULL, TO_ROOM );
        send_to_char( "Srebrzysta, niematerialna lanca uderza w twoje cia³o!\n\r", victim );
	}

	if ( !is_affected( victim, sn ) )
	{
		af.where	= TO_AFFECTS;
		af.type	= sn;
		af.level	= level;
		af.duration = duration; af.rt_duration = 0;
		af.location	= APPLY_AC;
		af.modifier	= modifier;
		af.bitvector = &AFF_NONE;
		affect_to_char( victim, &af, NULL, TRUE );
        if ( spell_item_check( ch, sn, NULL ) )
        {
            act( "Srebrzysta lanca przelatuje przez $b tworz±c jasn± otoczkê.", victim, NULL, NULL, TO_ROOM );
            send_to_char( "Srebrzysta lanca przelatuej przez ciebie, tworz±c jasn± otoczkê.\n\r", victim );
        }
        else
        {
            act( "Srebrzysta lanca rozpryskuje siê na $b tworz±c jasn± otoczkê.", victim, NULL, NULL, TO_ROOM );
            send_to_char( "Srebrzysta lanca rozpryskuje siê wokó³ ciebie, tworz±c jasn± otoczkê.\n\r", victim );
        }
    }

    if ( use_components )
    {
        dam *= number_range(3, 20);
        spell_damage( ch, victim, dam,  sn, DAM_PIERCE, FALSE );
        /**
         * log use
         */
        if ( !IS_NPC( ch ) )
        {
            char buf [ MAX_STRING_LENGTH ];
            sprintf( buf, "[%5d] spell_alicorn_lance+component: caster: %s (%d) dam: %d.", ch->in_room->vnum, ch->name, level, dam );
            log_string( buf );
        }
    }
    else
    {
        dam = UMAX( 10, dam );
        spell_damage( ch, victim, dam, sn, DAM_ENERGY, FALSE );
    }
    return;
}

/* pocisk ¶wiat³a
 * sk³adnia: cast sunscorch [kto]
 *
 * 24 marca 2002 - gurthg
 */
void spell_sunscorch( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int dam;
    int luck = get_curr_stat_deprecated( ch, STAT_LUC );
    int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

    if ( number_range( 0, LUCK_BASE_MOD + luck - vluck) < 1 )
    {
        act( "S³oneczny pocisk mija $c.", victim, NULL, NULL, TO_ROOM );
        send_to_char( "S³oneczny pocisk mija ciê w niewielkiej odleg³o¶ci!\n\r", victim );
        return;
    }

    dam = number_range( 13, 20 ) + dice( level, 4 );
    dam = luck_dam_mod( ch, dam );

    act( "S³oneczny pocisk trafia w $c.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "S³oneczny pocisk trafia ciê prosto w pier¶.\n\r", victim );

    if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
    {
        if ( number_range( 0, get_curr_stat_deprecated( victim, STAT_LUC ) ) > 10 )
            dam /= 3;
        else
            dam /= 2;
    }
    else if ( !IS_AFFECTED( victim, AFF_PERFECT_SENSES ) && !is_undead(victim) )
    {
        af.where = TO_AFFECTS;
        af.type = sn;
        af.level = level;
        af.duration = dice( 0, 2 ); af.rt_duration = 0;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = &AFF_BLIND;

        affect_to_char( victim, &af, NULL, TRUE );

        switch ( victim->sex )
        {
            case 0:
                send_to_char( "Zosta³o¶ o¶lepione!\n\r", victim );
                act( "$n wygl±da na o¶lepione.", victim, NULL, NULL, TO_ROOM );
                break;
            case 2:
                send_to_char( "Zosta³a¶ o¶lepiona!\n\r", victim );
                act( "$n wygl±da na o¶lepiona.", victim, NULL, NULL, TO_ROOM );
                break;
            default :
                send_to_char( "Zosta³e¶ o¶lepiony!\n\r", victim );
                act( "$n wygl±da na o¶lepionego.", victim, NULL, NULL, TO_ROOM );
                break;
        }
    }
    dam = URANGE( 1, dam, 90 ); //Raszer - przyciêcie maks dmg sunscorcha

    if( !spell_item_check( ch, sn , "fire based spell" ) && dam > 6 )
    {
        dam -= dam/5;
    }

    spell_damage( ch, victim, dam, sn, DAM_FIRE, FALSE );
    // zmiana levelu na 1 :-) zobaczynmy jak pali graty
    fire_effect( victim, 1, dam, TARGET_CHAR );
    return;
}

void spell_produce_fire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * vch, *vch_next;
	int dam;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	save_debug_info("spells_dru.c => spell_produce_fire", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	dam = level + dice( 3, 8 );
	dam = luck_dam_mod( ch, dam );
	dam *= URANGE( 60, dice( 10 + get_curr_stat_deprecated( ch, STAT_WIS ) + 2 * get_curr_stat_deprecated( ch, STAT_INT ), 3 ) , 140 );
	dam /= 100;
	if ( dam < 1 )
	{
		dam = 1;
	}

	send_to_char( "Zgodnie z twoj± wol± z ziemi wyskakuj± jêzory ognia.\n\r", ch );
	act( "Zgodnie z wol± $z z ziemi wyskakuj± jêzory ognia.", ch, NULL, NULL, TO_ROOM );

	for ( vch = ch->in_room->people;vch;vch = vch_next )
	{
		vch_next = vch->next_in_room;

		if ( !is_same_group( ch, vch ) && !is_safe_spell( ch, vch, TRUE ) && !IS_AFFECTED( vch, AFF_FLYING ) && !IS_AFFECTED( vch, AFF_FLOAT ) && ( vch->master != ch ) && ( vch->leader != ch ) )
		{
			if ( IS_AFFECTED( vch, AFF_MINOR_GLOBE ) || IS_AFFECTED( vch, AFF_GLOBE ) || IS_AFFECTED( vch, AFF_MAJOR_GLOBE ) || IS_AFFECTED( vch, AFF_ABSOLUTE_MAGIC_PROTECTION ) )
			{
				act( "Jêzor ognia znika przy zetkniêciu ze sfer± otaczaj±c± $c.", vch, NULL, NULL, TO_ROOM );
				act( "Jêzor ognia znika przy zetkniêciu z otaczaj±c± ciê sfer±.\n\r", ch, NULL, vch, TO_VICT );
				continue;
			}

			if ( IS_AFFECTED( vch, AFF_REFLECT_SPELL ) )
			{
				print_char( ch, "Twoje zaklêcie odbija siê od ochronnej tarczy %s.\n\r", vch->name2 );
				print_char( vch, "Zaklêcie %s odbija siê od ochronnej tarczy.\n\r", ch->name2 );
				act( "Zaklêcie $z odbija siê od ochronnej tarczy $Z.", ch, NULL, vch, TO_NOTVICT );
				continue;
				//				nonsensem jest odbijanie tego czaru obszarowego - Tener
				//				wcale nie jest, tak ma byc, od tego jest reflect, ma nawet fragmenty fireballa odbijac - Kainti
				// to jest nonsens, bo p³omienie wydobywaj± siê z pod³o¿a. nie maj± siê w jakim *kierunku* odbiæ. To tak jakby odbijaæ greater_meteor w kosmos - Tener
			}


			send_to_char( "Ogarnia ciê strumieñ ognia!\n\r", vch );
			act( "$c ogarnia strumieñ ognia!", vch, NULL, NULL, TO_ROOM );

			if ( is_affected( vch, gsn_mirror_image ) )
			{
				affect_strip( vch, gsn_mirror_image );
				send_to_char( "Ogieñ niszczy wszystkie otaczaj±ce ciê lustrzane odbicia.\n\r", vch );
				act( "Ogieñ niszczy wszystkie lustrzane odbicia otaczaj±ce $c.", vch, NULL, NULL, TO_ROOM );
			}

			spell_damage( ch, vch, dam, sn, DAM_FIRE, FALSE );
			fire_effect( vch, level, dam, TARGET_CHAR );
		}
		continue;
	}
	return;
}

/* cast 'storm shell' [ktos]
 *
 * czarek pozwala na otulenie siê sfer± czystej enrgii, dziêki której druid
 * jest odporny na ataki typu: cold, lighting i acid (20-50)%
 *
 * created: 23 marca 2002 (gurthg)
 */
void spell_storm_shell( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck  - vluck) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_RESIST_ACID ) ||
	     IS_AFFECTED( victim, AFF_RESIST_COLD ) ||
	     IS_AFFECTED( victim, AFF_RESIST_LIGHTNING ) ||
	     IS_AFFECTED( victim, AFF_RESIST_FIRE ) )
	{
		switch ( victim->sex )
		{
			case 0:
				send_to_char( "Nie mo¿esz byæ bardziej chronione.\n\r", victim );
				break;
			case 2:
				send_to_char( "Nie mo¿esz byæ bardziej chroniona.\n\r", victim );
				break;
			default :
				send_to_char( "Nie mo¿esz byæ bardziej chroniony.\n\r", victim );
				break;
		}
		return;
	}

	af.where	= TO_AFFECTS;
	af.type	= sn;
	af.location	= APPLY_RESIST;

	mod = URANGE( 5, number_range( 0, luck ), 20 );
	af.level	= 5 + level + mod;
	af.duration = ( ( mod < 7 ) ? 0 : 2 ) + level / 10; af.rt_duration = 0;

	af.modifier	= RESIST_ACID;
	af.bitvector = &AFF_RESIST_ACID;
	affect_to_char( victim, &af, NULL, TRUE );

	mod = URANGE( 5, number_range( 0, luck ), 20 );
	af.level= 5 + level + mod;

	af.modifier	= RESIST_ELECTRICITY;
	af.bitvector = &AFF_RESIST_LIGHTNING;
	affect_to_char( victim, &af, NULL, TRUE );

	mod = URANGE( 5, number_range( 0, luck ), 20 );
	af.level	= 5 + level + mod;

	af.modifier		= RESIST_COLD;
	af.bitvector	= &AFF_RESIST_COLD;
	affect_to_char( victim, &af, NULL, TRUE );

	mod = URANGE( 5, number_range( 0, luck ), 20 );
	af.level	= 5 + level + mod;

	af.modifier		= RESIST_FIRE;
	af.bitvector	= &AFF_RESIST_FIRE;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Doko³a $Z pojawia siê ciemna sfera stworzona z czystej energii.", ch, NULL, victim, TO_NOTVICT );
	send_to_char( "Doko³a ciebie pojawia siê ciemna sfera stworzona z czystej energii.\n\r", victim );
	return;
}

extern EVENT_DATA * event_first;
/* napierdalacz dla druida */
void spell_call_lightning( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = NULL, *tch;
	EVENT_DATA *event = NULL;
	int lightning_count;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( !IS_OUTSIDE( ch ) || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER ) )
	{
		send_to_char( "Musisz byæ na zewn±trz.\n\r", ch );
		return;
	}

	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
		if ( is_same_group( tch, ch ) && tch->fighting )
		{
			victim = tch->fighting;
			break;
		}

	if ( !victim )
	{
		send_to_char( "Przecie¿ nie walczysz.\n\r", ch );
		return;
	}

	for ( event = event_first; event; event = event->next )
		if ( event->type == EVENT_CALL_LIGHTNING && ( CHAR_DATA * ) event->arg1 == ch && ( ROOM_INDEX_DATA * ) event->arg2 == ch->in_room )
		{
			print_char( ch, "Ju¿ wezwa³%s¶ na pomoc b³yskawice.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "e" );
			return;
		}

	lightning_count = URANGE( 3, number_range( 3, 5 ) + level / 7.5, 9 + ( ( get_curr_stat_deprecated( ch, STAT_LUC ) > 20 ) ? 1 : 0 ) );
	create_event( EVENT_CALL_LIGHTNING, number_range( 2, 3 ) * PULSE_VIOLENCE, ch, ch->in_room, 100*level + lightning_count );
	act( "Przez niebo przetacza siê potê¿ny grzmot, wysoko nad tob± tworz± siê ciemne, burzowe chmury.", ch, NULL, NULL, TO_ALL );
	return;
}

void spell_blade_barrier( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_BLADE_BARRIER ) )
	{
		send_to_char( "Otacza ciê ju¿ bariera ostrzy.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_SHIELD_OF_NATURE ) )
	{
		send_to_char( "Otacza ciê ju¿ bariera natury.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 2 + level / 3; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_BLADE_BARRIER;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Wokó³ ciebie tworzy siê bariera wiruj±cych ostrzy.\n\r", ch );
	act( "Wokó³ $z tworzy siê bariera wiruj±cych ostrzy.", ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_immolate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_IMMOLATE ) )
	{
		if ( ch == victim )
			send_to_char( "Dooko³a ciebie unosi siê ju¿ jarz±ca po¶wiata.\n\r", ch );
		else
			act( "Dooko³a $Z unosi siê ju¿ jarz±ca po¶wiata.", ch, NULL, victim, TO_CHAR );
		return;
	}

	af.where	= TO_AFFECTS;
	af.type = sn;
	af.level	= level;
	af.duration = duration_modifier_by_spell_type( 10 + level / 3, SECT_HOT, ch );
    af.rt_duration = 0;
	af.modifier = 10 + level;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_IMMOLATE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Dooko³a ciebie tworzy siê jarz±ca po¶wiata.\n\r", victim );
	act( "Dooko³a $z tworzy siê jarz±ca po¶wiata.", victim, NULL, NULL, TO_ROOM );
	return;
}

/* Lodowy deszcz (skladnia: cast "freezing rain")
 *
 * wali we wszystki na lokacji oprocz druida i jego grupki
 *
 */
void spell_freezing_rain( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * vch, *vch_next;
	AFFECT_DATA af;
	int mod, chance;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

    if ( !IS_OUTSIDE( ch ) || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER ) )
    {
        send_to_char( "Musisz byæ na zewn±trz.\n\r", ch );
        return;
    }

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	chance = dice( 1, 100 );
	if ( chance < 2 )
	{
		send_to_char( "Sprawiasz, ¿e z nieba zaczyna padaæ ¶nieg.\n\r", ch );
		act( "Czary $z sprawiaj±, ¿e z nieba zaczyna padaæ ¶nieg.", ch, NULL, NULL, TO_ROOM );
	}
	else
	{
		if ( ch->level > 28 )
		{
			send_to_char( "Sprawiasz, ¿e z nieba spadaj± potê¿ne lodowe kule.\n\r", ch );
			act( "Czary $z sprawiaj±, ¿e z nieba spadaj± potê¿ne lodowe kule.", ch, NULL, NULL, TO_ROOM );
		}
		else if ( ch->level > 24 )
		{
			send_to_char( "Sprawiasz, ¿e z nieba spadaj± lodowe kule.\n\r", ch );
			act( "Czary $z sprawiaj±, ¿e z nieba spadaj± lodowe kule.", ch, NULL, NULL, TO_ROOM );
		}
		else
		{
			send_to_char( "Sprawiasz, ¿e z nieba spadaj± lodowe kuleczki.\n\r", ch );
			act( "Czary $z sprawiaj±, ¿e z nieba spadaj± lodowe kuleczki.", ch, NULL, NULL, TO_ROOM );
		}

		for ( vch = ch->in_room->people;vch;vch = vch_next )
		{
			vch_next = vch->next_in_room;
			if ( vch != ch && !is_safe_spell( ch, vch, TRUE ) && !is_same_group( ch, vch ) )
			{
				if ( IS_AFFECTED( vch, AFF_ABSOLUTE_MAGIC_PROTECTION ) )
				{
					act( "Lodowe kule znikaj± przy zetkniêciu z otaczaj±c± ciê sfer±.\n\r", ch, NULL, vch, TO_VICT );
					act( "Lodowe kule znikaj± przy zetkniêciu z otaczaj±c± $c sfer±.", vch, NULL, NULL, TO_ROOM );
					return;
				}

				if ( IS_AFFECTED( vch, AFF_REFLECT_SPELL ) )
				{
					print_char( ch, "Twoje zaklêcie odbija siê od ochronnej tarczy %s.\n\r", vch->name2 );
					print_char( vch, "Zaklêcie %s odbija siê od ochronnej tarczy.\n\r", ch->name2 );
					act( "Zaklêcie $z odbija siê od ochronnej tarczy $Z.", ch, NULL, vch, TO_NOTVICT );
					vch = ch;
				}

				if ( is_affected( vch, gsn_mirror_image ) )
				{
					affect_strip( vch, gsn_mirror_image );
					send_to_char( "Lodowe kule rozwalaj± wszystkie otaczaj±ce ciê lustrzane odbicia.\n\r", vch );
					act( "Lodowe kule rozwalaj± wszystkie lustrzane odbicia otaczaj±ce $c.", vch, NULL, NULL, TO_ROOM );
				}

				spell_damage( ch, vch, dice(level/2,4) + dice( 3, 8 ), sn, DAM_COLD, FALSE );

				if ( dice( 5, 5 ) > get_curr_stat_deprecated( vch, STAT_DEX ) )
				{
					print_char( vch, "Pod naporem lodu, z impetem walisz siê na ziemiê.\n\r" );
					act( "Pod naporem lodu $n przewraca siê na ziemiê.", vch, NULL, NULL, TO_ROOM );
					vch->position = POS_SITTING;
					WAIT_STATE( ch, URANGE( 12, level, 24 ) );
				}

				mod = 100 - level - get_curr_stat_deprecated( ch, STAT_WIS ) - get_curr_stat_deprecated( ch, STAT_INT );
				chance = dice( 1, mod );

				if ( !IS_AFFECTED( vch, AFF_WEAKEN ) && chance < 5 )
				{
					print_char( vch, "Z lodu na twoim ciele tworzy siê niebieska sfera mrozu.\n\r" );
					act( "Z lodu na ciele $z tworzy siê niebieska sfera mrozu.", vch, NULL, NULL, TO_ROOM );
					af.where = TO_AFFECTS;
					af.type = sn;
					af.level = level;
					af.duration = 2; af.rt_duration = 0;
					af.location = APPLY_STR;
					af.modifier = -2;
					af.bitvector = &AFF_WEAKEN;
					affect_to_char( vch, &af, NULL, TRUE );
				}
				chance = dice( 1, mod );
				if ( chance < 5 )
					spell_chill_metal( gsn_chill_metal, level, ch, vch, TAR_CHAR_OFFENSIVE );
			}
		}
	}

	return;
}
/* Magiczny kostur ( skladnia: 'cast shillelagh' )
 *
 * Mala szansa na entangle z proga
 */

void spell_shillelagh( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = NULL;
	int duration, luck = get_curr_stat_deprecated( ch, STAT_LUC );
	AFFECT_DATA af;

	if ( is_affected( ch, gsn_shillelagh ) )
	{
		send_to_char( "Nie masz jeszcze tyle mocy, aby stworzyæ nasienie shillelagh.\n\r", ch );
		return;
	}

    /**
     * sprawdz rodzaj lokacji
     */
    if (
            !IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_DRUID )
            || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_THIRST )
            || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_WATER )
            || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER )
            || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_AIR )
       )
    {
        send_to_char( "Poszukaj jakiego¶ dogodniejszego terenu na zasianie shillelagh.\n\r", ch );
        return;
    }

	if ( number_range( 0, luck + LUCK_BASE_MOD + 10 ) == 0 )  // +10, spell latwy
	{
		act( "Ma³y pêd wyrasta z ziemi, jednak po chwili usycha.\n\r", ch, NULL, NULL, TO_ALL );
		return;
	}
	obj = create_object( get_obj_index( OBJ_VNUM_SHILLELAGH ), FALSE );
	if ( !obj )
	{
		bug( "Brak obiektu: OBJ_VNUM_SHILLELAGH.", 0 );
		return;
	}

	duration = URANGE( 2, 1 + dice( 1, 3 ) + level / 3, 11 );
	if ( number_range( 0, luck ) > 10 ) duration++;

	obj->timer = duration;
	obj->rent_cost = UMAX( 1, ( 2 * level ) / 3 );

	obj->value[ 1 ] = number_range( 1, 2 ) + ( ( number_range( 0, luck ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, level ) > 17 ) ? 1 : 0 );
	obj->value[ 2 ] = number_range( 2, 3 ) + ( ( number_range( 0, luck ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, level ) > 17 ) ? 1 : 0 );
	obj->value[ 5 ] = level / 15;
	obj->value[ 6 ] = level / 15;

	if ( number_range( 10, 62 ) < level )  // koles ma farta i przywolal lepsiejszy kostur
	{
		obj->value[ 5 ] += 1;
		obj->value[ 6 ] += 1;

		act( "Ziemia drz¿y lekko, wokó³ kosturu tworz± siê na ziemi pêkniêcia, które jednak po chwili znikaj±.\n\r", ch, NULL, NULL, TO_ROOM );
		send_to_char( "Wokó³ kosturu tworz± siê na ziemi pêkniêcia...\n\r", ch );
	}


	obj_to_room( obj, ch->in_room );
	act( "Z ziemi wyrasta ma³y pêd, który po chwili przeradza siê w gruby kostur.\n\r", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Z ziemi wyrasta ma³y pêd, który po chwili przeradza siê w gruby kostur.\n\r", ch );

	af.where = TO_AFFECTS;
	af.type = gsn_shillelagh;
	af.level = 50;
	af.duration = UMAX( 1, duration - 1 ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	affect_to_char( ch, &af, NULL, FALSE );

	return;
}

void spell_charm_animal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int chance = 1;

	if ( !IS_NPC( ch ) && IS_IMMORTAL( victim ) )
		return;

	if ( is_safe( ch, victim ) )
		return;

	if ( victim == ch )
	{
		send_to_char( "Podobasz siê sobie jeszcze bardziej zwierzaku!\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_CHARM ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "Rozpoznajesz, ¿e $N jest wierne komu¶ innemu.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "Rozpoznajesz, ¿e $N jest wierny komu¶ innemu.", ch, NULL, victim, TO_CHAR );
				break;
			default :
				act( "Rozpoznajesz, ¿e $N jest wierna komu¶ innemu.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

			if ( victim->level >25 && IS_NPC( victim ) )
	{
		send_to_char( "Nie jeste¶ w stanie zauroczyæ tak potêznego zwierzêcia.\n\r", ch );
		return;
	}

	if ( ( !IS_NPC( victim ) && !IS_NPC( ch ) )
	     || IS_AFFECTED( victim, AFF_CHARM )
	     || !IS_SET( race_table[ GET_RACE( victim ) ].type , ANIMAL )
	     || IS_AFFECTED( ch, AFF_CHARM )
	     || level < victim->level
	     || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		return;


	if ( EXT_IS_SET( victim->in_room->room_flags, ROOM_LAW ) )
		return;

	if ( level > 30 )
		chance = 100;
	else if ( level < LEVEL_HERO && level >= 20 )
		chance = URANGE( 3, 15 + 5 * ( ( level - 2 ) - victim->level ) + get_curr_stat_deprecated( ch, STAT_INT ) / 3, 75 );
	else if ( level < 20 && level >= 15 )
		chance = URANGE( 3, 15 + 5 * ( ( level - 2 ) - victim->level ) + get_curr_stat_deprecated( ch, STAT_INT ) / 3, 95 );
	else if ( level < 15 && level >= 10 )
		chance = URANGE( 3, 20 + 5 * ( ( level - 3 ) - victim->level ) + get_curr_stat_deprecated( ch, STAT_INT ) / 3, 95 );
	else if ( level < 10 && level >= 5 )
		chance = URANGE( 3, 20 + 5 * ( ( level - 4 ) - victim->level ) + get_curr_stat_deprecated( ch, STAT_INT ) / 2, 95 );
	else if ( level < 5 )
		chance = URANGE( 3, 25 + 5 * ( ( level - 5 ) - victim->level ) + get_curr_stat_deprecated( ch, STAT_INT ) / 2, 95 );

	if ( number_percent() > chance )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( !can_see( victim, ch ) || victim->position == POS_SLEEPING )
	{
		if ( victim->position == POS_SLEEPING )
			act( "$N rzuca siê przez sen.", ch, NULL, victim, TO_ALL );
		else
			act( "$N rozgl±da siê dooko³a, ale nie widzi swojego pana.", ch, NULL, victim, TO_ALL );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PARALYZE ) )
	{
		act( "$N wci±¿ stoi w bezruchu.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( !add_charm( ch, victim, FALSE ) )
		return;

	die_follower( victim, FALSE );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 12 - UMAX( -4, get_curr_stat_deprecated( victim, STAT_INT ) - 14 ); af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( victim, &af, NULL, TRUE );

	add_follower( victim, ch, TRUE );

	if ( IS_AFFECTED( victim, AFF_TROLL ) )
		victim->affected_by[ AFF_TROLL.bank ] ^= AFF_TROLL.vector;

	act( "Czujesz nag³y przyp³yw mi³o¶ci do $z.", ch, NULL, victim, TO_VICT );
	act( "$N patrzy na ciebie ze zwierzêcym oddaniem.", ch, NULL, victim, TO_CHAR );
	act( "$N patrzy na $c ze zwierzêcym oddaniem.", ch, NULL, victim, TO_ROOM );

	stop_fighting( ch, FALSE );
	stop_fighting( victim, FALSE );

	if ( IS_NPC( victim ) && EXT_IS_SET( victim->act, ACT_AGGRESSIVE ) )
		EXT_REMOVE_BIT( victim->act, ACT_AGGRESSIVE );

	stop_hating( victim, ch, FALSE );
	stop_hunting( victim );
	stop_fearing( victim, ch, FALSE );
	stop_hating( ch, victim, FALSE );
	stop_hunting( ch );
	stop_fearing( ch, victim, FALSE );
	return;
}

/*	sk³adnia:
	cast 'animal rage' <cel>

	Czar podnosi si³ê i zrêczno¶æ zwierzêcia, ale tylko takiego, którego
	masterem jest czaruj±cy.
 */

void spell_animal_rage( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int mod, chance, mod_moon = 0;
    int luck = get_curr_stat_deprecated( ch, STAT_LUC );

    if ( ch == victim )
    {
        send_to_char( "Przecie¿ nie jeste¶ zwierzêciem.\n\r", ch );
        return;
    }

    if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , ANIMAL ) )
    {
        act( "$N nie jest zwierzêciem.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
    {
        send_to_char( "Nic siê nie dzieje.\n\r", ch );
        return;
    }

    if ( is_affected( victim, sn ) )
    {
        send_to_char( "Zaklêcie nie podzia³a³o!\n\r", ch );
        return;
    }

    if ( victim->master != ch )
    {
        act( "$N nie jest twoim zwierzêciem.", ch, NULL, victim, TO_CHAR );
        return;
    }
    switch ( time_info.moon_phase )
    {
        case MOON_NEW_MOON:
            mod_moon = -1;
            break;
        case MOON_FULL_MOON:
            mod_moon = 1;
            break;
    }

    /* dodatek do si³y */
    chance = number_range( 0, luck );
    mod = 0;
    if ( chance < 3 ) mod = -1;
    else if ( chance > 15 ) mod = 2;
    else if ( chance > 10 ) mod = 1;

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level	= level;
    af.location = APPLY_STR;
    af.duration = 4 + level / 4 + 2 * ( mod + mod_moon );
    af.rt_duration = 0;
    af.modifier = 1 + level / 12 + mod + mod_moon;
    af.bitvector = &AFF_NONE;
    affect_to_char( victim, &af, NULL, TRUE );

    /* dodatek do zrêczno¶ci */
    chance = number_range( 0, luck );
    mod = 0;
    if ( chance < 3 ) mod = -1;
    else if ( chance > 15 ) mod = 2;
    else if ( chance > 10 ) mod = 1;

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level	= level;
    af.location = APPLY_DEX;
    af.duration = 4 + level / 4 + 2 * ( mod + mod_moon );
    af.rt_duration = 0;
    af.modifier = 1 + level / 12 + mod + mod_moon;
    af.bitvector = &AFF_NONE;
    affect_to_char( victim, &af, NULL, TRUE );

    send_to_char( "Ogarnia ciê zwierzêcy sza³!\n\r", victim );
    act( "$z ogarnia zwierzêcy sza³.", victim, NULL, NULL, TO_ROOM );
    return;
}

/* taki sobie offence dla druida */
void spell_burst_of_flame( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck  - vluck) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

    if ( ch == victim )
	{
		send_to_char( "Naprawdê chcesz siê przypaliæ?\n\r", ch );
		return;
	}

	dam = dice( 3, 5 ) + level;
	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

			if( spell_item_check( ch, sn , "fire based spell" ) )
    {
      dam += 3;
    }

	spell_damage( ch, victim, dam, sn, DAM_FIRE , TRUE );
	return;
}

/* taki sobie offence dla druida */
void spell_burst_of_fire ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck  - vluck) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

    if ( ch == victim )
	{
		send_to_char( "Naprawdê chcesz siê przypaliæ?\n\r", ch );
		return;
	}

	dam = dice( 5, 5 ) + level;
	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

		if( spell_item_check( ch, sn , "fire based spell" ) )
    {
      dam += 6;
    }

	spell_damage( ch, victim, dam, sn, DAM_FIRE , TRUE );
	return;
}

/* taki sobie offence dla druida, dmg troche wiekszy niz ice bolta */
void spell_hellfire ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck  - vluck) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

    if ( ch == victim )
	{
		send_to_char( "Naprawdê chcesz siê przypaliæ?\n\r", ch );
		return;
	}

	dam = number_range(18,40) + dice( level, 4 ) ;
	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

		if( !spell_item_check( ch, sn , "fire based spell" ) )
    {
      dam -= dam/5;
    }

	spell_damage( ch, victim, dam, sn, DAM_FIRE , TRUE );
	return;
}

/* taki ma³y resist na fire */
void spell_endure_fire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    int mod, luck = get_curr_stat_deprecated( ch, STAT_LUC );

    if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
    {
        send_to_char( "Nic siê nie dzieje.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( victim, AFF_RESIST_FIRE ) )
    {
        switch ( victim->sex )
        {
            case 0:
                if ( ch == victim )
                    send_to_char( "Nie mo¿esz byæ bardziej chronione przed ogniem.\n\r", ch );
                else
                    act( "$N nie mo¿e byæ bardziej chronione przed ogniem.", ch, NULL, victim, TO_CHAR );
                break;
            case 2:
                if ( ch == victim )
                    send_to_char( "Nie mo¿esz byæ bardziej chroniona przed ogniem.\n\r", ch );
                else
                    act( "$N nie mo¿e byæ bardziej chroniony przed ogniem.", ch, NULL, victim, TO_CHAR );
                break;
            default :
                if ( ch == victim )
                    send_to_char( "Nie mo¿esz byæ bardziej chroniony przed ogniem.\n\r", ch );
                else
                    act( "$N nie mo¿e byæ bardziej chroniona przed ogniem.", ch, NULL, victim, TO_CHAR );
                break;
        }
        return;
    }

    if( !spell_item_check( ch, sn , "fire based spell" ) )
    {
        send_to_char( "Brakuje ci agatu ognistego.\n\r", ch );
        return;
    }

    mod = 10 + level / 3;

    /* Bonus dla specjalisty */
    if ( !IS_NPC( ch ) )
    {
        if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
        {
            mod += level/3;
        }
    }

    af.where       = TO_AFFECTS;
    af.type        = sn;
    af.level       = mod;
    af.duration    = duration_modifier_by_spell_type( 5 + level, SECT_HOT, ch );
    af.rt_duration = 0;
    af.location    = APPLY_RESIST;
    af.modifier    = RESIST_FIRE;
    af.bitvector   = &AFF_RESIST_FIRE;
    affect_to_char( victim, &af, NULL, TRUE );

    act( "Dooko³a $z rozb³yskuje czerwona otoczka, która po chwili ga¶nie.", victim, NULL, NULL, TO_ROOM );
    act( "Dookola ciebie rozb³yskuje czerwona otoczka, która po chwili ga¶nie.", victim, NULL, NULL, TO_CHAR );

    return;
}
/* taki ma³y resist na zimno */
void spell_endure_cold( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int mod, luck = get_curr_stat_deprecated( ch, STAT_LUC );

    if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
    {
        send_to_char( "Nic siê nie dzieje.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( victim, AFF_RESIST_COLD ) )
    {
        switch ( victim->sex )
        {
            case 0:
                if ( ch == victim )
                    send_to_char( "Nie mo¿esz byæ bardziej chronione przed zimnem.\n\r", ch );
                else
                    act( "$N nie mo¿e byæ bardziej chronione przed zimnem.", ch, NULL, victim, TO_CHAR );
                break;
            case 2:
                if ( ch == victim )
                    send_to_char( "Nie mo¿esz byæ bardziej chroniona przed zimnem.\n\r", ch );
                else
                    act( "$N nie mo¿e byæ bardziej chroniony przed zimnem.", ch, NULL, victim, TO_CHAR );
                break;
            default :
                if ( ch == victim )
                    send_to_char( "Nie mo¿esz byæ bardziej chroniony przed zimnem.\n\r", ch );
                else
                    act( "$N nie mo¿e byæ bardziej chroniona przed zimnem.", ch, NULL, victim, TO_CHAR );
                break;
        }
        return;
    }

    if( !spell_item_check( ch, sn , "cold based spell" ) )
    {
        send_to_char( "Brakuje ci srebrnego proszku.\n\r", ch );
        return;
    }

    mod = 10 + level / 3;

    /* Bonus dla specjalisty */
    if ( !IS_NPC( ch ) )
    {
        if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
        {
            mod += level/3;
        }
    }

    af.where       = TO_AFFECTS;
    af.type        = sn;
    af.level       = mod;
    af.duration    = duration_modifier_by_spell_type( 5 + level, SECT_COLD, ch );
    af.rt_duration = 0;
    af.location    = APPLY_RESIST;
    af.modifier    = RESIST_COLD;
    af.bitvector   = &AFF_RESIST_COLD;

    affect_to_char( victim, &af, NULL, TRUE );

    act( "Dooko³a $z rozb³yskuje niebieska otoczka, która po chwili ga¶nie.", victim, NULL, NULL, TO_ROOM );
    act( "Dookola ciebie rozb³yskuje niebieska otoczka, która po chwili ga¶nie.", victim, NULL, NULL, TO_CHAR );

    return;
}

/* taki ma³y resist na kwas */
void spell_endure_acid( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod, luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_RESIST_ACID ) )
	{
		switch ( victim->sex )
		{
			case 0:
				if ( ch == victim )
					send_to_char( "Nie mo¿esz byæ bardziej chronione przed kwasem.\n\r", ch );
				else
					act( "$N nie mo¿e byæ bardziej chronione przed kwasem.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				if ( ch == victim )
					send_to_char( "Nie mo¿esz byæ bardziej chroniona przed kwasem.\n\r", ch );
				else
					act( "$N nie mo¿e byæ bardziej chroniony przed kwasem.", ch, NULL, victim, TO_CHAR );
				break;
			default :
				if ( ch == victim )
					send_to_char( "Nie mo¿esz byæ bardziej chroniony przed kwasem.\n\r", ch );
				else
					act( "$N nie mo¿e byæ bardziej chroniona przed kwasem.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

    if( !spell_item_check( ch, sn , "acid based spell" ) )
    {
		send_to_char( "Brakuje ci kwasu.\n\r", ch );
		return;
    }

	mod = 10 + level / 3;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			mod += level/3;
		}
	}

    af.where       = TO_AFFECTS;
    af.type        = sn;
    af.level       = mod;
    af.duration    = 5 + level;
    af.rt_duration = 0;
    af.location    = APPLY_RESIST;
    af.modifier    = RESIST_ACID;
    af.bitvector   = &AFF_RESIST_ACID;

    affect_to_char( victim, &af, NULL, TRUE );

	act( "Dooko³a $z rozb³yskuje zielona otoczka, która po chwili ga¶nie.", victim, NULL, NULL, TO_ROOM );
	act( "Dookola ciebie rozb³yskuje zielona otoczka, która po chwili ga¶nie.", victim, NULL, NULL, TO_CHAR );

	return;
}

/* taki ma³y piorunochron */
void spell_endure_lightning( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod, luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_RESIST_LIGHTNING ) )
	{
		switch ( victim->sex )
		{
			case 0:
				if ( ch == victim )
					send_to_char( "Nie mo¿esz byæ bardziej chronione przed elektryczno¶ci±.\n\r", ch );
				else
					act( "$N nie mo¿e byæ bardziej chronione przed elektryczno¶ci±.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				if ( ch == victim )
					send_to_char( "Nie mo¿esz byæ bardziej chroniona przed elektryczno¶ci±.\n\r", ch );
				else
					act( "$N nie mo¿e byæ bardziej chroniony przed elektryczno¶ci±.", ch, NULL, victim, TO_CHAR );
				break;
			default :
				if ( ch == victim )
					send_to_char( "Nie mo¿esz byæ bardziej chroniony przed elektryczno¶ci±.\n\r", ch );
				else
					act( "$N nie mo¿e byæ bardziej chroniona przed elektryczno¶ci±.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

    if( !spell_item_check( ch, sn , "lighting based spell" ) )
    {
		send_to_char( "Brakuje ci kawaleczka bursztynu.\n\r", ch );
		return;
    }

	mod = 10 + level / 3;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			mod += level/3;
		}
	}

	af.where	   = TO_AFFECTS;
	af.type        = sn;
	af.level	   = mod;
	af.duration    = 5 + level;
    af.rt_duration = 0;
	af.location	  = APPLY_RESIST;
	af.modifier	  = RESIST_ELECTRICITY;
	af.bitvector  = &AFF_RESIST_LIGHTNING;

	affect_to_char( victim, &af, NULL, TRUE );

	act( "Dooko³a $z rozb³yskuje bia³o-niebieska otoczka, która po chwili ga¶nie.", victim, NULL, NULL, TO_ROOM );
	act( "Dookola ciebie rozb³yskuje bia³o-niebieska otoczka, która po chwili ga¶nie.", victim, NULL, NULL, TO_CHAR );

	return;
}
//smashing wave
void spell_smashing_wave( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    int dam;
    int luck = get_curr_stat_deprecated( ch, STAT_LUC );
    int vluck = get_curr_stat_deprecated( victim, STAT_LUC );
    if ( number_range( 0, LUCK_BASE_MOD + luck  - vluck) < 1 )
    {
        send_to_char( "Nic siê nie dzieje.\n\r", ch );
        return;
    }
    dam = number_range( 10, 15 ) + dice( level / 2, 2 );
    dam += victim->weight / 20 + victim->height / 50;
    dam = luck_dam_mod( ch, dam );
    /**
     * modyfikacja obrazen wzgledem typu lokacji
     */
    dam = damage_modifier_by_spell_type( dam, SECT_WATER, ch );
    /**
     * modyfikacja obrazen wzgledem fazy ksiezyca
     */
    dam = damage_modifier_by_moon_phase( dam );
    /**
     * ustaw minimum
     */
    dam = UMAX( level/2, dam );
    act( "Potê¿na fala wody uderza w $c.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "Potê¿na fala wody uderza w ciebie!\n\r", victim );
    spell_damage( ch, victim, dam, sn, DAM_BASH, FALSE );
    /**
     * przewroc jezeli sie tak trafi
     */
    if ( number_range( 1, get_curr_stat( victim, STAT_DEX ) ) < 5 )
    {
        act( "{5Potê¿na fala wody powala $z na ziemiê.{x", victim, NULL, NULL, TO_ROOM );
        send_to_char( "{5Potê¿na fala powala ciê na ziemiê.{x\n\r", victim );
        victim->position = POS_SITTING;
        WAIT_STATE( victim, 3 * PULSE_VIOLENCE );
    }
    return;
}

void spell_summon_animals( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * monster;
	AFFECT_DATA af;
	ush_int vnum = 50;
	int animal_level, animal_hit, charisma, duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), wis = get_curr_stat_deprecated( ch, STAT_WIS );
	char buf [ MAX_STRING_LENGTH ];

	// jak kto¶ nie chce byæ followany
	if ( EXT_IS_SET( ch->act, PLR_NOFOLLOW ) )
	{
		send_to_char( "Przecie¿ nie chcesz ¿eby kto¶ za tob± chodzi³.\n\r", ch );
		return;
	}

	// no jak stoii w private, to przecie gdzie to summ±æ?
	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_PRIVATE ) )
	{
		send_to_char( "To siê tutaj nie uda, jest za ma³o miejsca.\n\r", ch );
		return;
	}

	// jak kto¶ ma mniej ni¿ 8 charyzmy, to niech spada
	charisma = get_curr_stat_deprecated( ch, STAT_CHA );
	if ( charisma < 8 )
	{
		print_char( ch, "Nie dasz rady przywo³aæ ¿adnego zwierzêcia.\n\r" );
		return;
	}

	if ( ch->counter[ 4 ] != 0 )
	{
		if ( ch->counter[ 4 ] == 1 )
			send_to_char( "Musisz jeszcze trochê odpocz±æ przed przywo³aniem nastêpnej istoty.\n\r", ch );
		else
			send_to_char( "Niefortunne przywo³anie tego potwora zabra³o ci zbyt du¿o si³, odpocznij trochê.\n\r", ch );
		return;
	}

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( number_range( 1, UMAX( ( 50 - level ) * 30, 500 ) ) == 1 )
	{
		summon_malfunction( ch, sn );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_ANIMAL_INVIS ) )
	{
		affect_strip( ch, gsn_animal_invis );
		EXT_REMOVE_BIT( ch->affected_by, AFF_ANIMAL_INVIS );
		send_to_char( "Przestajesz byæ niewidzialn<&y/a/e> dla zwierz±t.\n\r", ch );
	}

	// czas trwania czaru
	// duration = 3600 to +/- 15 ticków
	duration = 40;
	if ( number_range( 0, luck ) > 15 ) duration += 2;
	if ( number_range( 0, luck ) > 10 ) duration += 4;
	if ( number_range( 0, luck ) < 5 ) duration -= 5;
	if ( number_range( 0, luck ) < 3 ) duration -= 3;
	duration = 2300 + duration * level;

	// ustawianie poziomu
	animal_level = level / 2;
	if ( wis > 23 ) ++animal_level;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_INT ) ) > 12 ) ++animal_level;
	if ( number_range( 0, luck ) > 15 ) ++animal_level;
	if ( number_range( 0, luck ) < 3 ) --animal_level;
	animal_level = URANGE( 1, animal_level, 20 );

	// ustawianie hpków
	animal_hit = 18 + animal_level * number_range( 14, 19 );
	if ( number_range( 0, luck ) > 10 )
		animal_hit = ( animal_hit * 11 ) / 10;
	if ( number_range( 0, luck ) < 3 )
		animal_hit = ( animal_hit * 9 ) / 10;
	if ( number_range( 0, wis ) > 18 )
		animal_hit = ( animal_hit * 11 ) / 10;
	if ( number_range( 0, wis ) < 5 )
		animal_hit = ( animal_hit * 9 ) / 10;


	switch ( animal_level )
	{
		case 1:
			vnum = MOB_VNUM_WIEWIORKA;
			break;
		case 2:
			vnum = MOB_VNUM_KRUK;
			break;
		case 3:
		case 4:
			vnum = MOB_VNUM_BORSUK;
			break;
		case 5:
		case 6:
			vnum = MOB_VNUM_WIEWIORKA_M;
			break;
		case 7:
			vnum = MOB_VNUM_DZIK;
			break;
		case 8:
		case 9:
			vnum = MOB_VNUM_WOLF;
			break;
		case 10:
		case 11:
			vnum = MOB_VNUM_BEAR;
			break;
		case 12:
			vnum = MOB_VNUM_TIGER;
			break;
		case 13:
			vnum = MOB_VNUM_ORZEL_M;
			break;
		case 14:
			vnum = MOB_VNUM_WILK_M;
			break;
		case 15:
		case 16:
			vnum = MOB_VNUM_BEAR_M;
			break;
		case 17:
			vnum = MOB_VNUM_TYGRYS_M;
			break;
		case 18:
		case 19:
		case 20:
			vnum = MOB_VNUM_DUSICIEL_M;
			break;
	}

	ch->counter[ 4 ] = 1;

	monster = create_mobile( get_mob_index( vnum ) );
	if (!monster)
	  {
	    bug("spell_summon_animals failed: no mob",0);
	    return;
	  }

	if (!IS_NPC(ch))
	{
		sprintf( buf, "spell_summon_animals: [%5d], %s, %d %s.", ch->in_room->vnum, ch->name, vnum, monster->name );
		log_string( buf );
	}

	char_to_room( monster, ch->in_room );
	EXT_SET_BIT( monster->act, ACT_NO_EXP );
    money_reset_character_money( monster );
	monster->level = animal_level;
	monster->hit = animal_hit;
	monster->max_hit = monster->hit;

	monster->perm_stat[ STAT_STR ] = number_range( 70, 100 ) + (animal_level-2) / 2;
	monster->perm_stat[ STAT_DEX ] = number_range( 70,  90 ) + (animal_level-2) / 2 ;
	monster->perm_stat[ STAT_WIS ] = number_range( 10,  40 ) + 3 * animal_level;
	monster->perm_stat[ STAT_INT ] = number_range(  5,  30 ) + 2 * animal_level;
	monster->perm_stat[ STAT_CON ] = number_range(  0,  20 ) + 6 * animal_level;
	monster->perm_stat[ STAT_CHA ] = number_range( 50,  90 );
	monster->perm_stat[ STAT_LUC ] = number_range( 50,  90 );

	monster->damage[ DICE_NUMBER ] = URANGE( 1, number_range( animal_level-2 , animal_level+1 ) / 4, 4 );
	monster->damage[ DICE_TYPE ] = URANGE( 3, 3 + ( number_range( 10, level ) > 12 ? 1 : 0 ) + ( number_range( 10, level ) > 24 ? 1 : 0 ), 5 );
	monster->damage[ DICE_BONUS ] = ( wis - number_range(16,14) ) / number_range(3,4);
	monster->hitroll = ( wis - 15 ) / number_range(3,4);
	EXT_SET_BIT( monster->act, ACT_WARRIOR );
	EXT_SET_BIT( monster->off_flags, OFF_RESCUE );
	monster->hit = get_max_hp( monster );

    if ( level > number_range(14,20) )
    {
        EXT_SET_BIT( monster->off_flags, OFF_FAST );
    }

	act( "$N pojawia siê.", ch, NULL, monster, TO_ROOM );

	if ( !add_charm( ch, monster, TRUE ) )
	{
		EXT_SET_BIT( monster->act, ACT_AGGRESSIVE );
		create_event( EVENT_EXTRACT_CHAR, duration/2, monster, NULL, 0 );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level	= level;
	af.duration = -1;
    af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( monster, &af, NULL, TRUE );

	add_follower( monster, ch, TRUE );

	create_event( EVENT_EXTRACT_CHAR, duration, monster, NULL, 0 );

	return;
}

void spell_magic_fang( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod, position;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( ch == victim )
	{
		send_to_char( "Przecie¿ nie jeste¶ zwierzêciem.\n\r", ch );
		return;
	}

	if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , ANIMAL ) )
	{
		act( "$N nie jest zwierzêciem.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( !IS_SET( race_table[ GET_RACE( victim ) ].parts, PART_FANGS ) || is_affected( victim, sn ) )
	{
		send_to_char( "Zaklêcie nie podzia³a³o!\n\r", ch );
		return;
	}

	if ( victim->master != ch )
	{
		act( "$N nie jest twoim zwierzêciem.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}
	position = victim->position;

	if ( position < 5 )
	{
		act( "Zaklêcie nie podzia³a³o na $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	mod = ( number_range( 0, luck ) > 10 ) ? 2 : 1;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level	= level;
	af.location = APPLY_DAMROLL;
	af.duration = mod + 3 + level / 5; af.rt_duration = 0;
	af.modifier = mod + level / 15 + (level/LEVEL_HERO); /* z bonusem dla 31 leva */
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	af.location = APPLY_HITROLL;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Czujesz, ¿e z pyska zaczynaj± odpadaæ ci kawa³ki piany.\n\r", victim );

	switch ( position )
	{
		case POS_RESTING:
		case POS_SITTING:
			act( "$n toczy pianê z pyska po czym wstaje.", victim, NULL, NULL, TO_ROOM );
			victim->position = POS_STANDING;
			break;
		case POS_FIGHTING:
			act( "$n toczy pianê z pyska, po czym atakuje ze zdwojon± si³±.", victim, NULL, NULL, TO_ROOM );
			break;
		default :
			act( "$n toczy pianê z pyska.", victim, NULL, NULL, TO_ROOM );
			break;
	}

	return;
}

/* taki sobie offence z ma³± szans± na holda :-) */
void spell_frost_rift( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dam, duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );
	int chance = ( level + luck ) / 15 + 1;
	/*
	dla wzoru (level + luck)/15 + 1 chance przybiera warto¶ci
		9	12	16	20	24	28 (luck)
	 1	1	 1	 2	 2	 2	 2
	 6	2	 2	 2	 2	 3	 3
	11	2	 2	 2	 3	 3	 3
	16	2	 2	 3	 3	 3	 3
	21	3	 3	 3	 3	 4	 4
	26	3	 3	 3	 4	 4	 4
	31	3	 3	 4	 4	 4	 4
	(poziom)
	*/
	if ( number_range( 0, LUCK_BASE_MOD + luck  - vluck) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

    if( spell_item_check( ch, sn , "cold based spell" ) )
    {
      chance += 3;
    }

    if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_COLD ) )
    {
        chance += 2;
    }

	dam = dice( 3, 5 ) + level;
	dam = luck_dam_mod( ch, dam );
	spell_damage( ch, victim, dam, sn, DAM_COLD , TRUE );

	if (
	    number_percent() < chance
	    && !IS_AFFECTED( victim, AFF_FREE_ACTION )
	    && !IS_AFFECTED( victim, AFF_PARALYZE )
	)
	{

		duration = 1 + ( level / 10 );
		if ( number_range( 0, luck ) > 15 ) ++duration;
		if ( number_range( 0, luck ) < 5 ) --duration;

		if ( number_range( 0, vluck ) > 15 ) --duration;
		if ( number_range( 0, vluck ) < 5 ) ++duration;

		af.where = TO_AFFECTS;
		af.type = gsn_holdperson;
		af.level = level;
		af.location = APPLY_NONE;
		af.duration = UMAX( 1, duration ); af.rt_duration = 0;
		af.modifier	= 0;
		af.bitvector = &AFF_PARALYZE;
		affect_to_char( victim, &af, NULL, TRUE );

		send_to_char( "Czujesz, jak zamarazaj± twoje ko¶ci!\n\r", victim );
		act( "$n zatrzymuje siê w miejscu.", victim, NULL, NULL, TO_ROOM );
	}

	return;
}

/* Tener
   mass refresh - bezczelne wywo³anie refresha na ka¿dej z osób z roomie
*/
void spell_mass_refresh( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA *gch;

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group( gch, ch ) )
			continue;

		spell_refresh( sn, level+8, ch, gch, target );
	}

	return;
}


void spell_refresh( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int val;

	/* min 50% zwraca maksa mv victima */
	val = number_range( 35, 50 );
	val += ( level / 4 ) * number_range( 2, 6 );

	victim->move = UMIN( victim->move + val, victim->max_move );

	if ( victim->max_move == victim->move )
	{
		send_to_char( "Zmêczenie ustepuje ca³kowicie.\n\r", victim );
		if ( ch != victim )
		{
			switch ( victim->sex )
			{
				case 0 :
					act( "$n wygl±da na ca³kowicie wypoczête.", victim, NULL, NULL, TO_ROOM );
					break;
				case 2 :
					act( "$n wygl±da na ca³kowicie wypoczêt±.", victim, NULL, NULL, TO_ROOM );
					break;
				default :
					act("$n wygl±da na ca³kowicie wypoczêtego.",victim, NULL, NULL, TO_ROOM );
					break;
			}
		}
	}
	else
	{
		send_to_char( "Czujesz siê mniej zmêczon<&y/a/e>.\n\r", victim );
		if ( ch != victim )
		{
			switch ( victim->sex )
			{
				case 0 :
					act( "$n wygl±da na mniej zmêczone.", victim, NULL, NULL, TO_ROOM );
					break;
				case 2 :
					act( "$n wygl±da na mniej zmêczon±.", victim, NULL, NULL, TO_ROOM );
					break;
				default :
					act( "$n wygl±da na mniej zmêczonego.", victim, NULL, NULL, TO_ROOM );
					break;
			}
		}
	}
	return;
}

void spell_lava_bolt ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = NULL, *tch;
	EVENT_DATA *event = NULL;
	int lava_count;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
		if ( is_same_group( tch, ch ) && tch->fighting )
		{
			victim = tch->fighting;
			break;
		}

	if ( !victim )
	{
		send_to_char( "Przecie¿ nie walczysz.\n\r", ch );
		return;
	}

	for ( event = event_first; event; event = event->next )
		if ( event->type == EVENT_LAVA && ( CHAR_DATA * ) event->arg1 == ch && ( ROOM_INDEX_DATA * ) event->arg2 == ch->in_room )
		{
			print_char( ch, "Ju¿ przyzwa³%s¶ ¿ywio³y ognia i ziemi.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "e" );
			return;
		}

	lava_count = UMAX( 1, ( level - 8 ) / 3 + ( ( get_curr_stat_deprecated( ch, STAT_LUC ) > 20 ) ? 1 : 0 ) );
	create_event( EVENT_LAVA, number_range( 1, 2 ) * PULSE_VIOLENCE, ch, ch->in_room, 100*level + lava_count );
	act( "Ziemia dr¿y i w potwornym huku pod twoje nogi wylewa siê lawa.", ch, NULL, NULL, TO_ALL );
	return;
}

void spell_ice_bolt ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dam;
	int luck = get_curr_stat( ch, STAT_LUC );
	int vluck = get_curr_stat( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + (luck - vluck)/6 ) < 1 )
	{
		act( "Lodowy pocisk mija $c.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Lodowy pocisk mija ciê w niewielkiej odleg³o¶ci!\n\r", victim );
		return;
	}

	dam = number_range( 18, 28 ) + dice( level, 4 );
	dam = luck_dam_mod( ch, dam );

	act( "Lodowy pocisk z ogromn± prêdko¶ci± uderza w $c rozrzucaj±c po okolicy krzyszta³ki lodu.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Lodowy pocisk trafia ciê z ogromn± prêdko¶ci± prosto w pier¶ rozsypuj±c po okolicy kryszta³ki lodu.\n\r", victim );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		if ( number_range( 0, get_curr_stat( victim, STAT_LUC ) ) > 60 )
			dam /= 3;
		else
			dam /= 2;
	}
	else
	{
		af.where = TO_AFFECTS;
		af.type = 90;
		af.level = level;
		af.duration = duration_modifier_by_spell_type( dice( 2, 2 ), SECT_COLD, ch );
        af.rt_duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &AFF_SLOW;

		affect_to_char( victim, &af, NULL, TRUE );

		send_to_char( "¦wiat wokó³ ciebie znacznie przy¶piesza...\n\r", victim );
		act( "$n zaczyna poruszaæ siê znacznie wolniej.", victim, NULL, NULL, TO_ROOM );
	}

    dam = URANGE(1, dam, 100); //Raszer przyciecie maks dam z ice bolta

    if( !spell_item_check( ch, sn , "cold based spell" ) )
    {
      dam -= dam/5;
    }

	spell_damage( ch, victim, dam, sn, DAM_COLD, FALSE );
	// zmiana levelu na 1 :-) zobaczynmy jak pali (zamraza?:P) graty
	cold_effect( victim, 1, dam, TARGET_CHAR );
	return;
}

void spell_wind_shield ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	if ( is_affected( victim, sn ) )
	{
		if ( victim == ch )
			send_to_char( "Ju¿ jeste¶ pod wp³ywem tego czaru.\n\r", ch );
		else
			send_to_char( "Cel jest ju¿ pod wp³ywem tego zaklêcia.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = duration_modifier_by_spell_type( level / 5, SECT_AIR, ch );
    af.rt_duration = 0;
	af.location = APPLY_AC;
	af.modifier = -20 - (15 * URANGE( 0, level, 40 ))/10;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Nagle $c otacza tarcza z wiatru.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Nagle otacza ciê tarcza z wiatru.\n\r", victim );
	return;
}

/* taki fajny spellik, druidow zmieniaja sie lapki w szpony, co z tym idzie
nie moze wykonywac prawie niczego z act_obj.c ani rzucac czarkow. Memowac moze */
void spell_beast_claws( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = NULL;
	OBJ_DATA *obj2 = NULL;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), wis = get_curr_stat_deprecated( ch, STAT_WIS ), dur;


	if ( IS_AFFECTED( ch, AFF_BEAST_CLAWS ) )
	{
		send_to_char( "Twoje d³onie s± ju¿ przemienione w szpony.\n\r", ch );
		return;
	}

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê przemieniæ swych d³oni w szpony.\n\r", ch );
		return;
	}

	if ( get_hand_slots( ch, WEAR_WIELD ) >= 2 )
	{
		send_to_char( "Masz zajête rêce, zdejmij co¶.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_WIELD ) )
	{
		send_to_char( "Nosisz jak±¶ broñ, zdejmij j±.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_HOLD ) || get_eq_char( ch, WEAR_INSTRUMENT ) || get_eq_char( ch, WEAR_SHIELD ) )
	{
		send_to_char( "Trzymasz co¶ w rêku, zdejmij to.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_HANDS ) )
	{
		send_to_char( "Masz co¶ za³o¿one na rêce, zdejmij to.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_WRIST_L ) )
	{
		send_to_char( "Masz co¶ za³o¿one na nadgarstku, zdejmij to.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_WRIST_R ) )
	{
		send_to_char( "Masz co¶ za³o¿one na nadgarstku, zdejmij to.\n\r", ch );
		return;
	}

	if (level < 18 )
		obj = create_object( get_obj_index( OBJ_VNUM_NORMAL_CLAW ), FALSE );
	else if (level < 24 )
		obj = create_object( get_obj_index( OBJ_VNUM_SHARP_CLAW ), FALSE );
	else
		obj = create_object( get_obj_index( OBJ_VNUM_VORPAL_CLAW ), FALSE );
	obj2 = create_object( get_obj_index( OBJ_VNUM_FALSE_CLAW ), FALSE );
	if ( !obj )
		return;

	if ( !obj2 )
		return;

	dur = number_range( 2, 4 );

	if ( dice( 2, luck ) > 20 )	++dur;
	if ( dice( 2, luck ) < 8 ) 	--dur;
	if ( dice( 2, wis ) > 20 )		++dur;
	if ( dice( 2, wis ) < 8 )		--dur;

	/* umagicznienie */
  obj->value[ 5 ] = level / 9;
  obj->value[ 6 ] = level / 9;

  if ( dice( 2, luck ) > 20 )
  {
  	++obj->value[ 5 ];
  }
  if ( dice( 2, luck ) > 20 )
  {
  	++obj->value[ 6 ];
  }
	obj->timer = dur + 1; //bo timer rozsypuje sie z przechodzenia z 1 na 0, a affect na 0 jeszcze trwa
	obj2->timer = dur + 1;
	obj_to_char( obj, ch );
	equip_char( ch, obj, WEAR_WIELD, TRUE );
	obj_to_char( obj2, ch );
	equip_char( ch, obj2, WEAR_SECOND, TRUE ); //dla wygladu, wg kodu bedzie walil dwa razy pierwsza reka

	af.where = TO_AFFECTS;
	af.type = gsn_beast_claws;
	af.level = level;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.modifier = number_range(2, 3);
	af.bitvector = &AFF_BEAST_CLAWS;
	affect_to_char( ch, &af, NULL, TRUE );

	act( "D³onie $z zamieniaj± siê w szpony.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Twoje d³onie zamieniaj± siê w szpony.\n\r", ch );
	return;
}

void spell_wind_charger( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int chance = 0, dam; //proceed nie sprawdzalo sie u mobow z ujemna odpornoscia
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck  - vluck) < 1 )
	{
		send_to_char( "Nie uda³o ci siê nak³oniæ wiatru do swej woli.\n\r", ch );
		return;
	}

	if ( victim == ch)
	{
		send_to_char( "Nic z tego, nie pofruniesz sobie.\n\r", ch);
		return;
	}

	if( IS_AFFECTED(victim, AFF_STONE_SKIN ) )
	{
		act( "Z twych r±k wylatuje silny podmuch wiatru, który oplata $C, i lekko rozwiewa $S w³osy.", ch, NULL, victim, TO_CHAR );
		act( "Widzisz jak $n celuje w ciebie rêkami po czym nagle pomiêdzy wami tworzy siê jaki¶ nienaturalny wiatr, który rozwiewa twe w³osy.", ch, NULL, victim, TO_VICT );
		act( "Widzisz jak $n celuje w $C rêkoma, po czym naglê pomiêdzy nimi pojawia siê jaki¶ nienaturalny podmuch wiatru, który tylko rozwiewa $X w³osy.", ch, NULL, victim, TO_NOTVICT );
	}
	else if ( check_blink( victim, ch, TRUE ) )
	{
		act( "Z twych r±k wylatuje silny podmuch wiatru. Przez moment wydaje ci siê, ¿e trafia w $C i jakby odrzuca do ty³u, jednak to tylko z³udzenie. Nienaturalnie rozwiana sylwetka przeciwnika z powrotem materializuje siê w miejscu w którym sta³a przed chwil±.", ch, NULL, victim, TO_CHAR );
		act( "Widzisz jak $n celuje w ciebie rêkoma, po czym nagle pomiêdzy wami pojawia siê nienaturalny wiatr, który stara siê ciebie odrzuciæ do ty³u. P³ynnie przechodzisz do innej sfery astralnej, czujesz przez moment jak wiatr stara siê rozwi±æ twoj± postaæ, ale po chwili ponownie pojawiasz siê w miejscu w którym sta³<&e¶/a¶/o¶>.", ch, NULL, victim, TO_VICT );
		act( "Widzisz jak $n celuje rêkoma w $C i po pomiêdzy nimi pojawia siê nienaturalnie silny wiatr, który przez moment rozwiewa sylwetkê $Z. Po chwili jednak pojawia siê ona ponownie w miejscu, w którym by³a przed chwil±.", ch, NULL, victim, TO_NOTVICT );
		return;
	}
	else
	{
		switch ( victim->size )
		{
			case SIZE_TINY:
				act( "Z twych r±k wylatuje silny podmuch wiatru, który trafia w $C, podrzuca do góry, po czym z g³o¶nym ³oskotem $N upada na ziemiê.", ch, NULL, victim, TO_CHAR );
				act( "Widzisz jak $n celuje w ciebie rêkami po czym nagle pomiêdzy wami tworzy siê jaki¶ nienaturalny wiatr, który z ³atwo¶ci± podrzuca ciê w górê, po czym z g³o¶nym ³oskotem upadasz na ziemiê.", ch, NULL, victim, TO_VICT );
				act( "Widzisz jak $n celuje w $C rêkoma, po czym naglê pomiêdzy nimi pojawia siê jaki¶ nienaturalny podmuch wiatru, który unosi $C wysoko, po czym, machaj±c rêkami w akcie desperacji, spada $E na ziemiê z g³o¶nym ³oskotem.", ch, NULL, victim, TO_NOTVICT );
				dam = dice( 1, UMAX( 10, ( 100 - ( victim->weight / 10 ) ) ) );
				dam = luck_dam_mod( ch, dam );
				spell_damage( ch, victim, dam, sn, DAM_BASH, FALSE );
				chance = 1;
				break;
			case SIZE_SMALL:
				if ( dice( 2, level ) > 14 )
				{
					act( "Z twych r±k wylatuje silny podmuch wiatru, który trafia w $C, podrzuca do góry, po czym z g³o¶nym ³oskotem $N upada na ziemiê.", ch, NULL, victim, TO_CHAR );
					act( "Widzisz jak $n celuje w ciebie rêkami po czym nagle pomiêdzy wami tworzy siê jaki¶ nienaturalny wiatr, który z ³atwo¶ci± podrzuca ciê w górê, po czym z g³o¶nym ³oskotem upadasz na ziemiê.", ch, NULL, victim, TO_VICT );
					act( "Widzisz jak $n celuje w $C rêkoma, po czym naglê pomiêdzy nimi pojawia siê jaki¶ nienaturalny podmuch wiatru, który unosi $C wysoko, po czym, machaj±c rêkami w akcie desperacji, spada $E na ziemiê z g³o¶nym ³oskotem.", ch, NULL, victim, TO_NOTVICT );
					dam = dice( 1, UMAX( 10 , ( 100 - ( victim->weight / 10 ) ) ) );
					dam = luck_dam_mod( ch, dam );
					spell_damage( ch, victim, dam, sn, DAM_BASH, FALSE );
					chance = 1;
				}
				else
				{
					act( "Z twych r±k wylatuje silny podmuch wiatru, który oplata $C, i lekko rozwiewa $S w³osy.", ch, NULL, victim, TO_CHAR );
					act( "Widzisz jak $n celuje w ciebie rêkami po czym nagle pomiêdzy wami tworzy siê jaki¶ nienaturalny wiatr, który rozwiewa twe w³osy.", ch, NULL, victim, TO_VICT );
					act( "Widzisz jak $n celuje w $C rêkoma, po czym naglê pomiêdzy nimi pojawia siê jaki¶ nienaturalny podmuch wiatru, który tylko rozwiewa $X w³osy.", ch, NULL, victim, TO_NOTVICT );
				}
				break;
			case SIZE_MEDIUM:
			case SIZE_LARGE:
				if ( number_range( dice( 10, level ) - 30, dice( 10, level ) + 10 ) > victim->weight / 10 )
				{
					act( "Z twych r±k wylatuje silny podmuch wiatru, który trafia w $C, podrzuca do góry, po czym z g³o¶nym ³oskotem $N upada na ziemiê.", ch, NULL, victim, TO_CHAR );
					act( "Widzisz jak $n celuje w ciebie rêkami po czym nagle pomiêdzy wami tworzy siê jaki¶ nienaturalny wiatr, który z ³atwo¶ci± podrzuca ciê w górê, po czym z g³o¶nym ³oskotem upadasz na ziemiê.", ch, NULL, victim, TO_VICT );
					act( "Widzisz jak $n celuje w $C rêkoma, po czym naglê pomiêdzy nimi pojawia siê jaki¶ nienaturalny podmuch wiatru, który unosi $C wysoko, po czym, machaj±c rêkami w akcie desperacji, spada $E na ziemiê z g³o¶nym ³oskotem.", ch, NULL, victim, TO_NOTVICT );
					dam = dice( 1, UMAX( 10 , ( 150 - ( victim->weight / 10 ) ) ) / 2 );
					dam = luck_dam_mod( ch, dam );
					spell_damage( ch, victim, dam, sn, DAM_BASH, FALSE );
					chance = 1;
				}
				else
				{
					act( "Z twych r±k wylatuje silny podmuch wiatru, który oplata $C, i lekko rozwiewa $S w³osy.", ch, NULL, victim, TO_CHAR );
					act( "Widzisz jak $n celuje w ciebie rêkami po czym nagle pomiêdzy wami tworzy siê jaki¶ nienaturalny wiatr, który rozwiewa twe w³osy.", ch, NULL, victim, TO_VICT );
					act( "Widzisz jak $n celuje w $C rêkoma, po czym naglê pomiêdzy nimi pojawia siê jaki¶ nienaturalny podmuch wiatru, który tylko rozwiewa $X w³osy.", ch, NULL, victim, TO_NOTVICT );
				}
				break;
			default:
				{
					act( "Z twych r±k wylatuje silny podmuch wiatru, który oplata $C.", ch, NULL, victim, TO_CHAR );
					act( "Widzisz jak $n celuje w ciebie rêkami po czym nagle pomiêdzy wami tworzy siê jaki¶ nienaturalny wiatr, nios±cy dla ciebie przyjemn± ¶wie¿o¶æ.", ch, NULL, victim, TO_VICT );
					act( "Widzisz jak $n celuje w $C rêkoma, po czym naglê pomiêdzy nimi pojawia siê jaki¶ nienaturalny podmuch wiatru, nios±cy dla $Z przyjemn± ¶wie¿o¶æ.", ch, NULL, victim, TO_NOTVICT );
				}
				break;
		}
	}

    if ( !victim || !victim->in_room )
    {
        return;
    }

    switch ( victim->in_room->sector_type )
    {
        case 0:  //sektory wewnatrz - wiatr wali victimem o sciane
        case 14:
        case 15:
        case 17:
            if ( chance == 1 && number_range( 0, level ) > number_range( 0, victim->level / 2 ) )  //jak go wiatr nie ruszyl to o sciane nie walnie
            {
                act( "Si³a podmuchu wiatru uderza $V o ¶cianê.", ch, NULL, victim, TO_CHAR );
                act( "Si³a podmuchu wiatru uderza tob± o ¶cianê!", ch, NULL, victim, TO_VICT );
                act( "Si³a podmuchu wiatru uderza $V o ¶cianê.", ch, NULL, victim, TO_NOTVICT );
                dam = dice( 3, victim->weight / 40 );
                spell_damage( ch, victim, dam, sn, DAM_BASH, FALSE );
            }
            break;
        case 1:  //sektory miejskie, drogowe itd gdzie jest troche pylu i piachu
        case 8:
        case 18:
        case 28:
        case 35:
        case 36:
        case 41:
            if ( dice( 1, 3 ) == 1 && !IS_AFFECTED( victim, AFF_PERFECT_SENSES ) )  //na 33% oslepia na krotko
            {
                act( "Drobinki py³u i kurzu wpadaj± do oczu $Z.", ch, NULL, victim, TO_CHAR );
                act( "Drobinki py³u i kurzu wpadaj± ci do oczu!", ch, NULL, victim, TO_VICT );
                act( "Drobinki py³u i kurzu wpadaj± do oczu $Z.", ch, NULL, victim, TO_NOTVICT );
                af.where = TO_AFFECTS;
                af.type	= 5;
                af.level = 20;
                af.duration = dice( 2, 2 ); af.rt_duration = 0;
                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector = &AFF_BLIND;
                affect_to_char( victim, &af, NULL, TRUE	);
                spell_damage( ch, victim, number_range(1, 5), sn, DAM_BASH, FALSE );
            }
            else if ( dice( 1, 3 ) == 3 )  //jezeli nie oslepi to 33% na stuna
            {
                act( "$N miota siê otoczony drobinkami kurzu i py³u.", ch, NULL, victim, TO_CHAR );
                act( "Drobinki kurzu i py³u otaczaj± ciê ze wszystkich stron, czujesz siê lekko oszo³omio<&ny/na/ne>!", ch, NULL, victim, TO_VICT );
                act( "$N miota siê otoczony drobinkami kurzu i py³u.", ch, NULL, victim, TO_NOTVICT );
                DAZE_STATE( victim, dice( 2, 2 ) * PULSE_VIOLENCE );
                spell_damage( ch, victim, number_range(1, 5), sn, DAM_BASH, FALSE );
            }
            break;
        case 2:   //sektory polne, trawiaste, gdzie trawy
        case 4:
        case 30:
        case 31:
        case 32:
        case 33:
        case 42:
            if ( chance == 1 && dice( 1, 2 ) == 1 )  //jak wywrocilo to latwiej straci orientacje w wysokich trawach
            {
                act( "$N miota siê otoczony wysokimi trawami.", ch, NULL, victim, TO_CHAR );
                act( "Padasz w otaczaj±c± ciê wysok± trawê i tracisz orientacjê.", ch, NULL, victim, TO_VICT );
                act( "$N miota siê otoczony wysokimi trawami.", ch, NULL, victim, TO_NOTVICT );
                DAZE_STATE( victim, dice( 2, 2 ) * PULSE_VIOLENCE );
                spell_damage( ch, victim, number_range(1, 5), sn, DAM_BASH, FALSE );
            }
            else if ( dice( 1, 5 ) == 5 )
            {
                act( "$N miota siê otoczony wysokimi trawami.", ch, NULL, victim, TO_CHAR );
                act( "Wichura krêci tob± dooko³a, tracisz orientacjê w trawach.", ch, NULL, victim, TO_VICT );
                act( "$N miota siê otoczony wysokimi trawami.", ch, NULL, victim, TO_NOTVICT );
                DAZE_STATE( victim, dice( 2, 2 ) * PULSE_VIOLENCE );
                spell_damage( ch, victim, number_range(1, 5), sn, DAM_BASH, FALSE );
            }
            break;
        case 3:   //sektory li¶ciaste
        case 11:
        case 12:
        case 38:
            if ( ( chance == 1 && dice( 1, 2 ) == 1 ) || dice( 1, 4 ) == 4 )  //jak wywrocilo to latwiej straci orientacje przysypany liscmi
            {
                act( "$C zasypuje chmura li¶ci i ga³±zek.", ch, NULL, victim, TO_CHAR );
                act( "Zasypuje ciê chmura li¶ci i ga³±zek, tracisz orientacjê.", ch, NULL, victim, TO_VICT );
                act( "$C zasypuje chmura li¶ci i ga³±zek.", ch, NULL, victim, TO_NOTVICT );
                af.where = TO_AFFECTS;
                af.type	= 304;
                af.level = 20;
                af.duration = dice( 2, 2 ); af.rt_duration = 0;
                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector = &AFF_DAZE;
                affect_to_char( victim, &af, NULL, TRUE	);
                spell_damage( ch, victim, number_range(1, 5), sn, DAM_BASH, FALSE );
            }
            else if ( dice( 1, 2 ) == 1 )   //moze byc tez stunowate daze:P
            {
                act( "$C zasypuje chmura li¶ci i ga³±zek.", ch, NULL, victim, TO_CHAR );
                act( "Zasypuje ciê chmura li¶ci i ga³±zek, tracisz orientacjê.", ch, NULL, victim, TO_VICT );
                act( "$C zasypuje chmura li¶ci i ga³±zek.", ch, NULL, victim, TO_NOTVICT );
                DAZE_STATE( victim, dice( 2, 2 ) * PULSE_VIOLENCE );
                spell_damage( ch, victim, number_range(1, 5), sn, DAM_BASH, FALSE );
            }
            break;
        case 10:  //sektory silnie piaszczyste
        case 24:
        case 29:
        case 40:
            act( "Chmury piachu zasypuj± $C i wpadaj± $O do oczu.", ch, NULL, victim, TO_CHAR );
            act( "Uderzaj± w ciebie chmury piachu wpadaj±c do oczu, tracisz poczucie kierunków.", ch, NULL, victim, TO_VICT );
            act( "Chmury piachu zasypuj± $C i wpadaj± $O do oczu.", ch, NULL, victim, TO_NOTVICT );
            if ( dice( 1, 2 ) == 1 )  //mocne daze jak piach po oczach na 50%
            {
                af.where = TO_AFFECTS;
                af.type	= 304;
                af.level = 20;
                af.duration = dice( 2, 2 ); af.rt_duration = 0;
                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector = &AFF_DAZE;
                affect_to_char( victim, &af, NULL, TRUE	);
            }
            else //a jak nie mocne to slabe
            {
                DAZE_STATE( victim, dice( 2, 2 ) * PULSE_VIOLENCE );
            }

            //i zawsze blind
            if ( !IS_AFFECTED( victim, AFF_PERFECT_SENSES ) )
            {
                af.where = TO_AFFECTS;
                af.type	= 5;
                af.level = 20;
                af.duration = dice( 2, 2 ); af.rt_duration = 0;
                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector = &AFF_BLIND;
                affect_to_char( victim, &af, NULL, TRUE	);
            }
            spell_damage( ch, victim, number_range(1, 5), sn, DAM_BASH, FALSE );
            break;
        case 16:  //tam gdzie snieg
        case 26:
        case 27:
            if ( chance == 1 && dice( 1, 2 ) == 1 )  //szansa na stuna jak padnie w sniegi i go one przysypia
            {
                act( "$N wpada po g³owê w le¿±cy nieopodal ¶nieg.", ch, NULL, victim, TO_CHAR );
                act( "Wpadasz w po g³owê w ¶nieg, przez co tracisz orientacjê.", ch, NULL, victim, TO_VICT );
                act( "$N wpada po g³owê w le¿±cy nieopodal ¶nieg.", ch, NULL, victim, TO_NOTVICT );
                DAZE_STATE( victim, dice( 2, 2 ) * PULSE_VIOLENCE );
                spell_damage( ch, victim, number_range(1, 5), sn, DAM_BASH, FALSE );
            }
            break;
        case 5:  //sektory kamieniste
        case 13:
            act( "Chmura kamieni i ¿wiru zasypuje $C.", ch, NULL, victim, TO_CHAR );
            act( "Chmura kamieni i ¿wiru wpada na ciebie bole¶nie rani±c.", ch, NULL, victim, TO_VICT );
            act( "Chmura kamieni i ¿wiru zasypuje $C.", ch, NULL, victim, TO_NOTVICT );
            dam = 15 + dice( level, 5 ); //dostaje po buzi kamykami
            spell_damage( ch, victim, dam / 2, sn, DAM_BASH, FALSE );
            spell_damage( ch, victim, dam / 2, sn, DAM_PIERCE, FALSE );
            break;
        case 6:  //sektory wodne
        case 7:
        case 19:
        case 20:
        case 21:
        case 22:
        case 25:
        case 34:
        case 37:
        case 39:
            act( "Fala wody uderza w $C.", ch, NULL, victim, TO_CHAR );
            act( "Fala wody uderza w ciebie!", ch, NULL, victim, TO_VICT );
            act( "Fala wody uderza w $C.", ch, NULL, victim, TO_NOTVICT );
            if ( chance != 1 && !IS_AFFECTED( victim, AFF_STABILITY ) )  //jak wiatr nie polozyl to fala wody polozy
            {
                victim->position = POS_SITTING;
                WAIT_STATE( victim, 36 );
            }
            dam = 15 + dice( level, 5 ); //dostaje po buzi fala wody
            spell_damage( ch, victim, dam, sn, DAM_BASH, FALSE );
            break;
        case 23:  //lawa... uuuuu...
            act( "Fala lawy ca³kowicie zalewa $C, po chwili po $S ciele zostaj± tylko zwêglone kosteczki.", ch, NULL, victim, TO_CHAR );
            act( "Wichura wytwarza potê¿n± falê lawy, która zalewa ciê. Po chwili nie czujesz ju¿ nic.", ch, NULL, victim, TO_VICT );
            act( "Fala lawy ca³kowicie zalewa $C, po chwili po $S ciele zostaj± tylko zwêglone kosteczki.", ch, NULL, victim, TO_NOTVICT );
            spell_damage( ch, victim, 1000, sn, DAM_FIRE, FALSE );
            break;
        default:
            break;
    }

	if ( chance == 1 && victim && victim->in_room )
	{
		if ( IS_AFFECTED( victim, AFF_STABILITY ) )
		{
			act( "Nagle jaka¶ niewidzialna si³a podnosi $c i stawia na nogi.", victim, NULL, NULL, TO_ROOM );
			act( "Nagle niewidzialna si³a podnosi ciê i stawia na nogi.", victim, NULL, NULL, TO_CHAR );
		}
		else
		{
			victim->position = POS_SITTING;
			WAIT_STATE( victim, URANGE( 12, level, 36 ) );
		}

	    /* Zdejmujemy mirrory */
            if( is_affected( victim, gsn_mirror_image ) )
            {
                affect_strip( victim, gsn_mirror_image );
                send_to_char( "Wszystkie twoje lustrzane odbicia znikaj±.\n\r", victim );
                act( "Wszystkie lustrzane odbicia otaczaj±ce $c migocz± i znikaj±.", victim, NULL, NULL, TO_ROOM );
	    }
        }
	return;
}

/**
 * czar przyzywania roju
 */
void spell_firefly_swarm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA * obj;
    int luck = get_curr_stat( ch, STAT_LUC ), wis = get_curr_stat( ch, STAT_WIS );
    int timer;
    /**
     * sprawdz czy cos ubrane w slocie WEAR_FLOAT
     */
    if ( get_eq_char( ch, WEAR_FLOAT ) )
    {
        send_to_char( "¦wietliki kr±¿± przez chwilê wokó³ ciebie, po czym rozlatuj± siê na wszystkie strony.\n\r", ch );
        return;
    }
    /**
     * bazowa szansa na niepowodzenie
     */
    if ( !IS_NPC( ch ) && number_range( 0, luck ) < LEVEL_HERO - level )
    {
        send_to_char( "W ostatniej chwili rozpraszasz swoj± uwagê i ¶wietliki odlatuj±.\n\r", ch );
        return;
    }
    /**
     * sprawdz rodzaj lokacji
     */
    if (
            !IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_DRUID )
            || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_THIRST )
            || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_WATER )
            || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER )
            || ch->in_room->sector_type == 25 // gorace zrodla
            || ch->in_room->sector_type == 26 // lodowiec
            || ch->in_room->sector_type == 27 // arktyczny lad
       )
    {
        send_to_char( "Jako¶ nie mo¿esz wezwaæ tutaj ¶wietlików.\n\r", ch );
        return;
    }
    /**
     * stworz obiekt i go sprawdz
     */
    obj = create_object( get_obj_index( OBJ_VNUM_FIREFLIES ), FALSE );
    if ( !obj )
    {
        send_to_char( "Co¶ zaczyna b³yszczeæ, a potem wygasa bez ¶ladu.\n\r", ch );
        bug( "Brak obiektu: OBJ_VNUM_FIREFLIES", 0 );
        return;
    }
    timer = UMAX( 4, 7 + ( level + wis ) / 8 );
    if ( number_range( 0, luck ) > 100 ) timer *= 2;
    if ( number_range( 0, luck ) < 50  ) timer /= 2;
    obj->timer = UMAX( 3, timer );
    obj_to_char( obj, ch );
    equip_char( ch, obj, WEAR_FLOAT, TRUE );
    EXT_SET_BIT( obj->extra_flags, ITEM_TRANSMUTE ); //zastepcze
    act( "Po chwili punkciki zlatuj± siê w ciasn± gromadkê, a owymi punktami okazuj± siê byæ... ¶wietliki, które szybko formuj± siê w sprawny rój o¶wietlaj±cy s³udze natury drogê.", ch, obj, NULL, TO_ROOM );
    act( "Owe zb³±kane ¶wietliki szybko formuj± siê w rój, gotowe do o¶wietlania ci drogi.", ch, obj, NULL, TO_CHAR );
    return;
}

/* taki sobie offence dla druida */
void spell_spray_of_thorns( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam;

	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		act( "Nie uda³o ci siê przywo³aæ k³uj±cych cierni.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dam = dice( 5, 5 ) + ( level * 2 ) / 3;

	dam = luck_dam_mod( ch, dam );

	spell_damage( ch, victim, dam, sn, DAM_PIERCE , TRUE );
	return;
}

/* Kontrolowanie pogody (skladnia: cast 'control weather' <lepiej/gorzej>)
 *
 * zmienia pogode na lepsza/gorsza (bylo)
 *
 */

void spell_control_weather( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	char * tar = ( char * ) vo;

	/* jakis komunikat o tym, ze spell nie dziala */
	send_to_char("Próbujesz zmieniæ pogodê, lecz w dziwny sposób opiera siê ona twej woli.\n\r", ch);
	return;

	if ( !tar || tar[ 0 ] == '\0' )
	{
		send_to_char ( "Chcesz ¿eby by³o lepiej czy gorzej?\n\r", ch );
		return;
	}

	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( !str_cmp( tar, "lepiej" ) || !str_cmp( tar, "better" ) )
		weather_info[ ch->in_room->sector_type ].change += 100 * dice( level / 3, 4 );
	else if ( !str_cmp( tar, "gorzej" ) || !str_cmp( tar, "worse" ) )
		weather_info[ ch->in_room->sector_type ].change -= 100 * dice( level / 3, 4 );
	else
	{
		send_to_char ( "Chcesz ¿eby by³o lepiej czy gorzej?\n\r", ch );
		return;
	}

	send_to_char( "Widzisz jak natura ugina siê przed si³± twojej woli.\n\r", ch );
	weather_update( FALSE );

	return;
}

/* Trzesienie ziemii (skladnia: cast earthquake)
 *
 * wali we wszystki na lokacji oprocz kaplana, jego grupka dostaje dam/2
 * a latajacym to zwisa i powiewa
 *
 */
void spell_earthquake( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * vch, *vch_next;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê wywo³aæ trzêsienia ziemi.\n\r", ch );
		return;
	}

	send_to_char( "Sprawiasz, ¿e ziemia zaczyna siê trz±¶æ!\n\r", ch );
	act( "Czary $z sprawiaj±, ¿e ziemia zaczyna siê trz±¶æ.", ch, NULL, NULL, TO_ROOM );

	for ( vch = ch->in_room->people;vch;vch = vch_next )
	{
		vch_next = vch->next_in_room;

		if ( vch != ch && !is_safe_spell( ch, vch, TRUE ) )
			if ( !IS_AFFECTED( vch, AFF_FLYING ) && !IS_AFFECTED( vch, AFF_FLOAT ) )
			{
				if ( is_same_group( ch, vch ) )
				{
					if ( !IS_NPC( ch ) )
						spell_damage( ch, vch, level / 2 + dice( 2, 5 ), sn, DAM_BASH, FALSE );
				}
				else
					spell_damage( ch, vch, level + dice( 3, 8 ), sn, DAM_BASH, FALSE );

				if ( dice( 5, 5 ) > get_curr_stat_deprecated( vch, STAT_DEX ) )
				{
					print_char( vch, "Z trudem utrzymujesz równowagê, nagle potê¿ny wstrz±s podrzuca ciê do góry. Z ca³ym impetem walisz siê na ziemiê.\n\r" );

					switch ( vch->sex )
					{
						case 0 :
							act( "$n z trudem utrzymuje równowagê, nagle potê¿ny wstrz±s podrzuca to do góry. Z g³uchym stêkniêciem wali siê na ziemie.", vch, NULL, NULL, TO_ROOM );
							break;
						case 2 :
							act( "$n z trudem utrzymuje równowagê, nagle potê¿ny wstrz±s podrzuca j± do góry. Z g³uchym stêkniêciem wali siê na ziemie.", vch, NULL, NULL, TO_ROOM );
							break;
						default :
							act( "$n z trudem utrzymuje równowagê, nagle potê¿ny wstrz±s podrzuca go do góry. Z g³uchym stêkniêciem wali siê na ziemie.", vch, NULL, NULL, TO_ROOM );
							break;
					}

					vch->position = POS_SITTING;
					WAIT_STATE( vch, number_range(3,5)*PULSE_VIOLENCE );
				}
				else
				{
					print_char( vch, "Z trudem utrzymujesz równowagê, nagle potê¿ny wstrz±s podrzuca ciê do góry. Ale szczê¶liwie l±dujesz na swoich nogach.\n\r" );
					switch ( vch->sex )
					{
						case 0 :
							act( "$n z trudem utrzymuje równowagê.", vch, NULL, NULL, TO_ROOM );
							break;
						case 2 :
							act( "$n z trudem utrzymuje równowagê.", vch, NULL, NULL, TO_ROOM );
							break;
						default :
							act( "$n z trudem utrzymuje równowagê.", vch, NULL, NULL, TO_ROOM );
							break;
					}
				}

			}
		continue;

		if ( SAME_AREA( vch->in_room->area, ch->in_room->area ) && SAME_AREA_PART( vch, ch ) )
			send_to_char( "Ziemia trzêsie siê.\n\r", vch );
	}
	return;
}

void spell_create_spring( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * spring;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	AFFECT_DATA af;

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ magicznego ¼ród³a.\n\r", ch );
		return;
	}

    if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_HOT ) )
    {
        send_to_char( "Stworzone magiczne ¼ród³o wyparowywuje w mgnieniu oka.\n\r", ch );
        return;
    }

	if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_WATER ) || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_CANDRINK ) || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ) )
	{
		send_to_char( "Czy¿by by³o tutaj za ma³o wody?\n\r", ch );
		return;
	}

	if ( !IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_DRUID ) )
	{
		if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER ) && ch->in_room->sector_type != 14 ) /* 14 - podziemie - np. u gornikow */
		{
			if ( number_percent() > 50 )
				send_to_char( "Co¶ sprawia, ¿e nie mo¿esz wezwaæ wody, która mog³aby wytrysn±æ tworz±c magiczny strumyk.\n\r", ch );
			else
				send_to_char( "W tym miejscu woda jest zbyt g³êboko, ¿eby wyp³yne³a na twoje ¿±danie.\n\r", ch );
			return;
		}
		else /* tu trafi jak nie jest SECT_NOWEATHER, i nie jest SECT_DRUID - wtedy to jest m.in. miasto(2) i podziemie(14), ale go jako wyjatek dodalem do IFa */
			if ( number_percent() < 30 ) {
				send_to_char( "Co¶ sprawia, ¿e tym razem nie mo¿esz wezwaæ wody, która mog³aby wytrysn±æ tworz±c magiczny strumyk.\n\r", ch );
				return;
			}
	}


	// szansa na wytry¶niêcie ¼róde³ka z wod± lecz±c±
	if ( !is_affected( ch, gsn_create_spring ) && level + luck > number_range( 1, 1000 ) )
	   {
	      spring = create_object( get_obj_index( OBJ_VNUM_SPRING_HEAL ), FALSE );
	      spring->timer = level / 3; // na krócej
	      act( "$p wyp³ywa nagle spod ziemi rozb³yskuj±c têcz± kolorów.", ch, spring, NULL, TO_ALL );
	   }
	else
	   {
	      spring = create_object( get_obj_index( OBJ_VNUM_SPRING ), FALSE );
	      spring->timer = level / 2;
	      act( "$p wyp³ywa nagle spod ziemi.", ch, spring, NULL, TO_ALL );
	   }

	obj_to_room( spring, ch->in_room );

	/* myk ¿eby ludzie nie próbowali tworzyæ ¼róde³ka tak d³ugo, a¿ wyjdzie lecz±ce */
	if ( !is_affected( ch, gsn_create_spring ) )
	   {
	      af.where = TO_AFFECTS;
	      af.type = gsn_create_spring;
	      af.level = 50;
	      af.duration = level/2 ; af.rt_duration = 0;
	      af.location = APPLY_NONE;
	      af.modifier = 0;
	      af.bitvector = &AFF_NONE;
	      affect_to_char( ch, &af, NULL, FALSE );
	   }

	return;
}

void spell_create_tree( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * tree, *fruit;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), fruit_count;

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		act( "Drzewko powoli wiêdnie, po czym rozsypuje siê w proch.", ch, NULL, NULL, TO_ALL );
		return;
	}

	if ( 
	     ch->in_room->sector_type == 10 ||
	     ch->in_room->sector_type == 23 ||
	     ch->in_room->sector_type == 24 ||
	     ch->in_room->sector_type == 25
       )
	{
    }
    if (
            !IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_DRUID )
            || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER )
            || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_WATER )
            || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER )
            || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_AIR )
            || ch->in_room->sector_type == 12 // bagno
            || ch->in_room->sector_type == 13 // wysokie góry
       )
    {
        act( "Drzewko nie mog±c znale¼æ miejsca by zapu¶ciæ korzenie rozsypuje siê w proch.", ch, NULL, NULL, TO_ALL );
        return;
    }
    if (
            IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_THIRST )
            || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_HOT )
       )
    {
        act( "Drzewko nie mog±c znale¼æ wody po chwili usycha.", ch, NULL, NULL, TO_ALL );
        tree = create_object( get_obj_index( OBJ_VNUM_TREE_HOT ), FALSE );
        obj_to_room( tree, ch->in_room );
        return;
    }
    if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_COLD ))
    {
        act( "Drzewko nie mog±c znale¼æ wody po chwili zamarza.", ch, NULL, NULL, TO_ALL );
        tree = create_object( get_obj_index( OBJ_VNUM_TREE_COLD ), FALSE );
        obj_to_room( tree, ch->in_room );
        return;
    }

	fruit_count = UMAX( 1, number_range( ( level / 2 ) - 2, ( level / 2 ) + 2 ) );
	if ( number_range( 0, luck ) > 12 ) fruit_count += 2;
	if ( number_range( 0, luck ) < 4 ) fruit_count -= 2;

	switch ( dice( 1, 3 ) )
	{
		case 1:
			tree = create_object( get_obj_index( OBJ_VNUM_TREE1 ), FALSE );
			obj_to_room( tree, ch->in_room );
			for ( ; fruit_count > 0; fruit_count-- )
			{
				fruit = create_object( get_obj_index( OBJ_VNUM_TREE1_FRUIT ), FALSE );
				obj_to_obj( fruit, tree );
			}
			break;
		case 2:
			tree = create_object( get_obj_index( OBJ_VNUM_TREE2 ), FALSE );
			obj_to_room( tree, ch->in_room );
			for ( ; fruit_count > 0; fruit_count-- )
			{
				fruit = create_object( get_obj_index( OBJ_VNUM_TREE2_FRUIT ), FALSE );
				obj_to_obj( fruit, tree );
			}
			break;
		case 3:
			tree = create_object( get_obj_index( OBJ_VNUM_TREE3 ), FALSE );
			obj_to_room( tree, ch->in_room );
			for ( ; fruit_count > 0; fruit_count-- )
			{
				fruit = create_object( get_obj_index( OBJ_VNUM_TREE3_FRUIT ), FALSE );
				obj_to_obj( fruit, tree );
			}
			break;
	}

	act( "Kwiaty pokrywaj±ce $h przekwitaj±, i na ich miejscu wyrastaj± piêkne owoce.", ch, tree, NULL, TO_ALL );

	create_event( EVENT_TREE_FRUITS, 2 * 240, tree, NULL, level / 2 );
	create_event( EVENT_TREE_FRUITS, 4 * 240, tree, NULL, level / 2 );
	create_event( EVENT_TREE_FRUITS, 6 * 240, tree, NULL, level / 2 );
	create_event( EVENT_TREE_EXTRACT, 8 * 240, tree, NULL, level / 2 );
	return;
}

void spell_flare( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration, mod;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_BLIND ) ||
	     is_affected( victim, sn ) ||
	     IS_AFFECTED( victim, AFF_PERFECT_SENSES ) ||
	     saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		return;

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ kuli bia³ego ¶wiat³a.\n\r", ch );
		return;
	}

	duration = 4 + level / 7;
	if ( number_range( 0, luck ) > 15 ) ++duration;
	if ( number_range( 0, luck ) < 4 ) --duration;

	if ( number_range( 0, vluck ) > 15 ) --duration;
	if ( number_range( 0, vluck ) < 4 ) ++duration;

	mod = -5;
	if ( number_range( 0, luck ) > 15 ) --mod;
	if ( number_range( 0, luck ) < 5 ) ++mod;

	if ( number_range( 0, vluck ) > 15 ) ++mod;
	if ( number_range( 0, vluck ) < 5 ) --mod;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_HITROLL;
	af.modifier = UMIN( 0, mod );
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.bitvector = &AFF_BLIND;
	affect_to_char( victim, &af, NULL, TRUE );

	mod = 20;
	if ( number_range( 0, luck ) > 15 ) mod += 5;
	if ( number_range( 0, luck ) < 5 ) mod -= 7;

	if ( number_range( 0, vluck ) > 15 ) mod -= 5;
	if ( number_range( 0, vluck ) < 5 ) mod += 7;

	af.location = APPLY_AC;
	af.modifier = mod;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Przed twoimi oczami eksploduje kula bia³ego swiat³a, przez chwilê niczego nie widzisz!\n\r", victim );
	act( "Przed oczami $z eksploduje kula bia³ego swiat³a.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_bark_skin( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration, mod;

	if ( IS_AFFECTED( victim, AFF_STONE_SKIN ) )
	{
		if ( victim == ch )
			send_to_char( "Twoja skóra jest ju¿ twarda jak ska³a.\n\r", ch );
		else
			act( "Skóra $Z jest ju¿ przeminiona w ska³ê.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_BARK_SKIN ) )
	{
		if ( victim == ch )
			send_to_char( "Ju¿ masz skórê przemienion± w korê.\n\r", ch );
		else
			print_char( ch, " Chcesz z %s zrobiæ drzewo?\n\r", victim->name2 );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch ) send_to_char( "Nie uda³o ci siê przemieniæ swojej skóry.\n\r", ch );
		else act( "Nie uda³o ci siê przemieniæ skóry $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 2 + level / 10;
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 4 ) duration -= 1;

	mod = 10 + level / 2;
	if ( number_range( 0, luck ) > 15 ) mod += 5;
	if ( number_range( 0, luck ) < 4 ) mod -= 7;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_AC;
	af.modifier = -mod;
	af.bitvector = &AFF_BARK_SKIN;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Widzisz jak skóra $z zmienia siê w korê.", victim, NULL, NULL, TO_ROOM );
	if ( victim == ch ) send_to_char( "Twoja skóra zamienia siê w korê.\n\r", victim );
	else act( "Czar $z sprawia, ¿e twoja skóra zamienia siê w korê.", ch, NULL, victim, TO_VICT );

	return;
}

void spell_ring_of_vanion( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	int luck;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( ch->in_room == NULL || EXT_IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL ) )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
    }

    if ( !spell_item_check( ch, sn, NULL ) )
    {
        send_to_char( "\n\rCzujesz, ¿e czego¶ ci brakuje.\n\r", ch );
        return;
    }

	send_to_char( "Twoje otoczenie rozmywa siê i przybiera nowe kszta³ty!\n\r", ch );
	act( "$n znika!", ch, NULL, NULL, TO_ROOM );
	char_from_room( ch );
	char_to_room( ch, get_room_index( ROOM_VNUM_DRUID_VANION - 10 + number_range( 0, 20) ) );
	act( "$n materializuje siê powoli.", ch, NULL, NULL, TO_ROOM );
	do_function( ch, &do_look, "auto" );
	return;
}

void spell_circle_of_vanion( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * vch, *vch_next;
	int luck;
	//	int gluck;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( ch->in_room == NULL || EXT_IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL ) )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

    if ( !spell_item_check( ch, sn, NULL ) )
    {
        send_to_char( "\n\rCzujesz, ¿e czego¶ ci brakuje.\n\r", ch );
        return;
    }

	for ( vch = ch->in_room->people;vch;vch = vch_next )
	{
		vch_next = vch->next_in_room;
		// sprawdzamy czy NPC ma mastera który jest castuj±cym ;-)
		if ( IS_NPC( vch ) )
		{
			if ( !is_same_group( vch, ch) )
				if ( vch->master != ch )
				{
					continue;
				}
		}
		else
		{
			// sprawdzamy czy PC jest w gupce
			if ( !is_same_group( vch, ch ) || vch == ch )
			{
				continue;
			}
		}
		if ( !IS_NPC( vch ) )
		{
			send_to_char( "Twoje otoczenie rozmywa siê i przybiera nowe kszta³ty!\n\r", vch );
		}
		act( "$n znika!", vch, NULL, NULL, TO_ROOM );
		char_from_room( vch );
		char_to_room( vch, get_room_index( ROOM_VNUM_DRUID_VANION ) );
		act( "$n materializuje siê powoli.", vch, NULL, NULL, TO_ROOM );
		if ( !IS_NPC( vch ) )
		{
			do_function( vch, &do_look, "auto" );
		}
	}
	send_to_char( "Twoje otoczenie rozmywa siê i przybiera nowe kszta³ty!\n\r", ch );
	act( "$n znika!", ch, NULL, NULL, TO_ROOM );
	char_from_room( ch );
	char_to_room( ch, get_room_index( ROOM_VNUM_DRUID_VANION ) );
	act( "$n materializuje siê powoli.", ch, NULL, NULL, TO_ROOM );
	do_function( ch, &do_look, "auto" );
	return;
}


// metalowe graty rdzewiej±, czyli technicznie rzecz ujmuj±c trac± na
// swojej wytrzyma³o¶ci
void spell_corrode( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	OBJ_DATA *obj, *objects[ 10 ];
	int items_corroded = 0, items, i;

	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;
		if (
		    IS_OBJ_STAT( obj, ITEM_UNDESTRUCTABLE ) ||
		    IS_OBJ_STAT( obj, ITEM_NOMAGIC ) ||
		    ( IS_OBJ_STAT( obj, ITEM_BLESS ) && number_percent() < 25 ) ||
		    ( IS_OBJ_STAT( obj, ITEM_MAGIC ) && number_percent() < 75 ) ||
		    (
		        obj->material != 2 &&
		        obj->material != 3 &&
		        obj->material != 4 &&
		        obj->material != 13 &&
		        obj->material != 32
		    )   // materia³y nieodporne na korozjê ;-)
		)
		{
			return;
		}
		if ( obj->condition < 2 )
		{
			act( "$p ju¿ bardziej nie zardzewieje.", ch, obj, NULL, TO_ALL );
			return;
		}
		obj->condition /= 2;
		act( "Na powierzchni $f pojawia siê warstewka rdzy.", ch, obj, NULL, TO_ALL );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_SANCTUARY ) ||
	     IS_AFFECTED( victim, AFF_MINOR_GLOBE ) ||
	     IS_AFFECTED( victim, AFF_GLOBE ) ||
	     IS_AFFECTED( victim, AFF_MAJOR_GLOBE ) ||
	     IS_AFFECTED( victim, AFF_ABSOLUTE_MAGIC_PROTECTION ) ||
	     ( !IS_NPC( ch ) && 2 * victim->level > 3 * level )
	   )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
	}

	for ( i = 0; i < 10; i++ ) objects[ i ] = NULL;
	items_corroded = URANGE( 0, 1 + level / 10, 4 );
	if ( level > 30 ) items_corroded++;

	for ( items = 0, obj = victim->carrying; obj != NULL; obj = obj->next_content )
	{

		// przedmioty w inventory - omijamy
		if ( obj->wear_loc == WEAR_NONE )
			continue;

		// materia³y nieodporne na korozjê - omijamy
		if ( obj->material != 2 &&
		     obj->material != 3 &&
		     obj->material != 4 &&
		     obj->material != 13 &&
		     obj->material != 32
		   )
			continue;

		// przedmioty zupe³nie nowe te¿ mo¿emy pomin±æ
		if ( obj->condition == 100 )
			continue;

		// przedmioty niezniszczalne omijamy
		if ( IS_OBJ_STAT( obj, ITEM_UNDESTRUCTABLE ) )
			continue;

		// przedmioty na które nie dzia³a magia - omijamy
		if ( IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
			continue;

		// przedmiotom po¶wiêconym dajemy 25% szansê na ominiêcie
		if ( IS_OBJ_STAT( obj, ITEM_BLESS ) && number_percent() < 25 )
			continue;

		// przedmiotom magicznym dajemy 75% szansê na ominiêcie
		if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) && number_percent() < 75 )
			continue;

		// materia³ów o wytrzyma³o¶ci mniejszej ni¿ 2 nie bierzemy
		if ( obj->condition < 2 )
			continue;

		// jak ju¿ jest wiêcej. lub tyle samo co 10, to dalej nie bierzemy
		if ( items >= 10 )
			break;

		// dodajemu obiekt do tablicy obiektów
		objects[ items++ ] = obj;
	}


	if ( items == 0 )
	{
		act( "Twoje zaklêcie nie podzia³a³o na przedmioty noszone przez $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( items_corroded > items )
		items_corroded = items;

	for ( i = 0; i < items_corroded; i++ )
	{
		obj = objects[ number_range( 0, items - 1 ) ];
		act( "Na powierzchni $f pojawia siê warstewka rdzy.", ch, obj, NULL, TO_ALL );
		if ( obj->condition > 1 )
		{
			obj->condition /= 2;
		}
		if ( items == 1 )
			break;
	}
	return;
}

void spell_nature_curse( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration;

	if ( is_affected( victim, sn ) )
	{
		send_to_char( "Na $C jest ju¿ na³o¿ona kl±twa natury.\n\r", ch );
		return;
	}

	if ( victim->class == CLASS_DRUID )
	{
		send_to_char( "Ta kl±twa nie zadzia³a na druida.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD - ( get_curr_stat_deprecated( victim, STAT_LUC ) / 4 ) ) == 0 &&
	     saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		act( "Nie uda³o ci siê przekl±æ $Z.", ch, NULL, victim, TO_CHAR );
		act( "$x nie uda³o siê ciebie przekl±æ.", ch, NULL, victim, TO_VICT );
		act( "$x nie uda³o siê przekl±æ $Z.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	duration = 2 + level / 10;
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 4 ) duration -= 1;

	af.where = TO_AFFECTS;
	af.type = gsn_nature_curse;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );
	act( "Nature curse wlazlo.", ch, NULL, victim, TO_ALL );
	return;
}

void spell_wood_master( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê nak³oniæ natury do swej woli.\n\r", ch );
		return;
	}

	//czy znajduje siê w lesie? jak nie to spell nic nie robi
	if ( victim->in_room->sector_type != 3 && //las
	     victim->in_room->sector_type != 11 && //puszcza
	     victim->in_room->sector_type != 38 ) //park
	{
		send_to_char( "Musisz znajdowaæ siê w lesie, by to zaklêcie zadzia³a³o.\n\r", ch );
		return;
	}

	dam = number_range( 10, ( get_curr_stat_deprecated( ch, STAT_WIS ) - 15 ) * 5 ) + dice( level, 7 );
	dam = luck_dam_mod( ch, dam );
	if ( dice( 2, get_curr_stat_deprecated( ch, STAT_INT ) ) > 24 ) dam += dam / 10;
	if ( dice( 2, get_curr_stat_deprecated( ch, STAT_INT ) ) < 15 ) dam -= dam / 9;
	if ( dam < 1 ) dam = 1;

	spell_damage( ch, victim, dam, sn, DAM_BASH, TRUE );
	return;
}

//mniejsze obrazenia niz wood master, bo gor i podziemi wiecej :)
void spell_stone_master( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê nak³oniæ natury do swej woli.\n\r", ch );
		return;
	}

	//czy znajduje siê w gorach albo podziemiach naturalnych? jak nie to spell nic nie robi
	if ( victim->in_room->sector_type != 4 && //wzgorza
	     victim->in_room->sector_type != 5 && //gory
	     victim->in_room->sector_type != 13 && //wys gory
	     victim->in_room->sector_type != 15 && //jaskinia
	     victim->in_room->sector_type != 17 && //podziemia naturalne
	     victim->in_room->sector_type != 35 ) //gorska sciezka
	{
		send_to_char( "Musisz znajdowaæ siê w górach albo naturalnych jaskiniach, by to zaklêcie zadzia³a³o.\n\r", ch );
		return;
	}

	dam = number_range( 10, ( get_curr_stat_deprecated( ch, STAT_WIS ) - 15 ) * 5 ) + dice( level, 6 );
	dam = luck_dam_mod( ch, dam );
	if ( dice( 2, get_curr_stat_deprecated( ch, STAT_INT ) ) > 24 ) dam += dam / 10;
	if ( dice( 2, get_curr_stat_deprecated( ch, STAT_INT ) ) < 15 ) dam -= dam / 9;
	if ( dam < 1 ) dam = 1;

	spell_damage( ch, victim, dam, sn, DAM_BASH, TRUE );
	return;
}

void spell_singing_ruff( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{

	OBJ_DATA * ruff, *obj, *obj_next;
	ROOM_INDEX_DATA * room = ch->in_room;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), ruff_tm, healing_rate = ch->in_room->heal_rate;

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
		if ( obj->pIndexData->vnum == OBJ_VNUM_RUFF1 ||
		     obj->pIndexData->vnum == OBJ_VNUM_RUFF2 ||
		     obj->pIndexData->vnum == OBJ_VNUM_RUFF3 ||
		     obj->pIndexData->vnum == OBJ_VNUM_RUFF4 ||
		     obj->pIndexData->vnum == OBJ_VNUM_RUFF5 ||
		     obj->pIndexData->vnum == OBJ_VNUM_RUFF6 )
		{
			act( "Nie udaje ci siê. Nie mo¿na nadu¿ywaæ ¿yciodajnych mocy przyrody. Zapamiêtaj to dobrze!", ch, NULL, NULL, TO_CHAR );
			act( "$n próbuje rzuciæ jaki¶ czar, ale nic siê nie dzieje.", ch, NULL, NULL, TO_ROOM );
			return;
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		act( "Udaje ci siê powo³aæ do ¿ycia malutk± ro¶linkê. Wiêdnie ona powoli po czym opada powoli na ziemiê, szybko blednie i znika.", ch, NULL, NULL, TO_CHAR );
		act( "$n udaje siê powo³aæ do ¿ycia malutk± ro¶linkê. Wiêdnie ona powoli po czym opada powoli na ziemiê, szybko blednie i znika.", ch, NULL, NULL, TO_ROOM );
		return;
	}

	ruff_tm = 3;

	if ( level == LEVEL_HERO )
	{
		ruff_tm += 6;
	}

	if ( level >= 25 )
	{
		ruff_tm += 3;
	}

	if ( number_range( 0, luck ) >= 18 )
	{
		ruff_tm += 5;
	}

	if ( number_range( 1, 100 ) <= 3 )
	{
		ruff = create_object( get_obj_index( OBJ_VNUM_RUFF4 ), FALSE );
		obj_to_room( ruff, ch->in_room );
		act( "Jakie¶ RZADKIE drzewo z dupy siê pojawia.", ch, ruff, NULL, TO_ALL );
		create_event( EVENT_RUFF, number_range( 1, 1 ) * PULSE_VIOLENCE, ruff, room, healing_rate );
	}
	else
	{
		ruff = create_object( get_obj_index( OBJ_VNUM_RUFF1 ), FALSE );
		obj_to_room( ruff, ch->in_room );
		act( "Jakie¶ drzewo z dupy sie pojawia.", ch, ruff, NULL, TO_ALL );
		create_event( EVENT_RUFF, number_range( 1, 1 ) * PULSE_VIOLENCE, ruff, room, healing_rate );
		ch->in_room->heal_rate *= 15;
		ch->in_room->heal_rate /= 10;
	}
	return;
}

void spell_entangle( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration, vluck, modifier;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( victim == ch )
	{
		send_to_char( "A tak poza tym, to wszystko w porzadku?\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_ENTANGLE ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N jest ju¿ oplatane.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N jest ju¿ oplatany.", ch, NULL, victim, TO_CHAR );
				break;
			default:
				act( "$N jest ju¿ oplatana.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	vluck = get_curr_stat_deprecated( victim, STAT_LUC ) / 2;
	if ( number_range( 0, luck - (vluck/4) + LUCK_BASE_MOD  ) == 0 )
	{
		act( "Nie uda³o ci siê opl±taæ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_FLYING ) )
	{
		act( "Przywo³ane przez ciebie ro¶liny próbuj± opl±taæ nogi $Z, jednak $E szybko odlatuje na bok.", ch, NULL, victim, TO_CHAR );
		act( "Z ziemi wyrastaj± ro¶liny i próbuj± opl±taæ twoje nogi, jednak ty szybko odlatujesz na bok.", ch, NULL, victim, TO_VICT );
		act( "Z ziemi wyrastaj± ro¶liny i próbuj± opl±taæ nogi $Z, jednak $E szybko odlatuje na bok.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	duration = 3 + level / ( 10 + ( ( number_range( 0, luck ) > 10 ) ? -1 : 3 ) );
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 2;

	if ( number_range( 0, vluck ) > 15 ) duration -= 2;
	if ( number_range( 0, vluck ) < 5 ) duration += 1;

	modifier = -4;
	if ( number_range( 0, luck ) > 15 ) modifier -= 2;
	if ( number_range( 0, luck ) < 5 ) modifier += 2;

	if ( number_range( 0, vluck ) > 15 ) modifier += 1;
	if ( number_range( 0, vluck ) < 5 ) modifier -= 1;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.modifier = UMIN( -1, modifier );
	af.bitvector = &AFF_ENTANGLE;
	affect_to_char( victim, &af, NULL, TRUE );

	modifier = 25;
	if ( number_range( 0, luck ) > 15 ) modifier += 5;
	if ( number_range( 0, luck ) < 5 ) modifier -= 7;

	if ( number_range( 0, vluck ) > 15 ) modifier -= 5;
	if ( number_range( 0, vluck ) < 5 ) modifier += 7;

	af.location = APPLY_AC;
	af.modifier = modifier;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Ro¶liny przywo³ane przez $Z zaczynaja oplatywac $z.", victim, NULL, ch, TO_NOTVICT );
	act( "Ro¶liny przywo³ane przez $z zaczynaja opl±tywaæ twoje nogi.", ch, NULL, victim, TO_VICT );
	act( "Przywo³ane przez ciebie ro¶liny zaczynaja opl±tywaæ $Z.", ch, NULL, victim, TO_CHAR );

	if ( is_affected( victim, gsn_float ) )
		affect_strip( victim, gsn_float );

	return;
}

void spell_shield_of_nature( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_SHIELD_OF_NATURE ) )
	{
		send_to_char( "Otacza ciê ju¿ bariera natury.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_BLADE_BARRIER ) )
	{
		send_to_char( "Otacza ciê ju¿ bariera ostrzy.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 2 + level / 3; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_SHIELD_OF_NATURE;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Wokó³ ciebie pojawiaj± siê wiruj±ce punkty ¶wiat³a.\n\r", ch );
	act( "Wokó³ $z pojawiaj± siê wiruj±ce punkty ¶wiat³a.", ch, NULL, NULL, TO_ROOM );
	return;
}


/*
 * Spell: fury of the wild
 *
 * Bonus do ataku dla ca³ej grupki druida - ale tylko dla zwierz±t lub druidow
 */
void spell_fury_of_the_wild( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA *vch;
	AFFECT_DATA af;
	int duration=0, mod=0;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		act( "Nie uda³o ci siê przywo³aæ dzikiej furii.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 4 + dice(level,2);
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 2;
	mod = 1 + level/15 + (level/LEVEL_HERO); /* z bonusem dla 31 leva */
	if ( number_range( 0, luck ) > 15 ) mod += 1;
	if ( number_range( 0, luck ) < 5 ) mod -= 2;


	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX(1,duration); af.rt_duration = 0;
	af.bitvector = &AFF_NONE;

	for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
	{
		/* spell dotyka druidów i zwierzeta */
		if ( !is_affected( vch, sn ) && is_same_group( vch, ch ) &&
		( ch->class == CLASS_DRUID || ( IS_SET(race_table[GET_RACE(ch)].type , ANIMAL ))  ||
		( IS_SET(race_table[GET_RACE(ch)].type , INSECT )) ||
		( IS_SET(race_table[GET_RACE(ch)].type , PLANT )) )  )
		{
			af.modifier = mod;
			af.location = APPLY_DAMROLL;
			affect_to_char( vch, &af, NULL, TRUE );

			af.location = APPLY_HITROLL;
			af.modifier = -mod;
			affect_to_char( vch, &af, NULL, TRUE );

			act( "Czujesz w sobie rosn±c± furiê.", vch, NULL, NULL, TO_CHAR );
			act( "Gwa³townie wci±ga powietrze.", vch, NULL, vch, TO_ROOM );
		}
	}
	return;
}


/* Drake: Nowa seria summonow dla druida, summon nature ally od I do IV. */
/* Tener: Refaktoring kodu Drake'a */

void spell_nature_ally_Nth( int sn, int level, CHAR_DATA *ch, void *vo, int target, int bonus_level_at_wis,
			    int bonus_level_at_int, int animal_level_range, char spell_name[], int fast_above_level,
			    bool fixed_perm_stats, int low_lev_limit, int hi_lev_limit, const int vnum_table[],
			    int hp_base, int hp_range_low, int hp_range_hi )
{
	CHAR_DATA * monster;
	AFFECT_DATA af;
	ush_int vnum = 50;
	int animal_level, animal_hit, charisma, duration;
	int luck = get_curr_stat( ch, STAT_LUC ), wis = get_curr_stat( ch, STAT_WIS );
	char buf [ MAX_STRING_LENGTH ];
	MOB_INDEX_DATA* mob_index = NULL;

	// jak kto¶ nie chce byæ followany
	if ( EXT_IS_SET( ch->act, PLR_NOFOLLOW ) )
	{
		send_to_char( "Przecie¿ nie chcesz ¿eby kto¶ za tob± chodzi³.\n\r", ch );
		return;
	}

	// no jak stoii w private, to przecie gdzie to summ±æ?
	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_PRIVATE ) )
	{
		send_to_char( "To siê tutaj nie uda, jest za ma³o miejsca.\n\r", ch );
		return;
	}

	// jak kto¶ ma mniej ni¿ 8 charyzmy, to niech spada
	charisma = get_curr_stat( ch, STAT_CHA );
	if ( charisma < 44 )
	{
		print_char( ch, "Nie dasz rady przywo³aæ ¿adnego zwierzêcia.\n\r" );
		return;
	}

	if ( ch->counter[ 4 ] != 0 )
	{
		if ( ch->counter[ 4 ] == 1 )
			send_to_char( "Musisz jeszcze trochê odpocz±æ przed przywo³aniem nastêpnej istoty.\n\r", ch );
		else
			send_to_char( "Niefortunne przywo³anie tego potwora zabra³o ci zbyt du¿o si³, odpocznij trochê.\n\r", ch );
		return;
	}

	if ( number_range( 0, LUCK_BASE_MOD + luck/6 ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( number_range( 1, UMAX( ( 50 - level ) * 30, 500 ) ) == 1 )
	{
		summon_malfunction( ch, sn );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_ANIMAL_INVIS ) )
	{
		affect_strip( ch, gsn_animal_invis );
		EXT_REMOVE_BIT( ch->affected_by, AFF_ANIMAL_INVIS );
		send_to_char( "Przestajesz byæ niewidzialn<&y/a/e> dla zwierz±t.\n\r", ch );
	}

	// czas trwania czaru
	// duration = 3600 to +/- 15 ticków
	duration = 40;
	if ( number_range( 0, luck ) > 90 ) duration += 2;
	if ( number_range( 0, luck ) > 60 ) duration += 4;
	if ( number_range( 0, luck ) < 30 ) duration -= 5;
	if ( number_range( 0, luck ) < 18 ) duration -= 3;
	duration = 2300 + duration * level;

	// ustawianie poziomu
	animal_level = level / 2;
	if ( wis > bonus_level_at_wis ) ++animal_level;
	if ( number_range( 0, get_curr_stat( ch, STAT_INT ) ) > bonus_level_at_int ) ++animal_level;
	if ( number_range( 0, luck ) > 90 ) ++animal_level;
	if ( number_range( 0, luck ) < 18 ) --animal_level;

	vnum = vnum_table[ UMIN( number_range( low_lev_limit, animal_level*2 ), hi_lev_limit ) - low_lev_limit ];

	animal_level = URANGE( 1, animal_level, animal_level_range );

	// ustawianie hpków
	animal_hit = hp_base + animal_level * number_range( hp_range_low, hp_range_hi );
	if ( number_range( 0, luck ) > 60 )
		animal_hit = ( animal_hit * 11 ) / 10;
	if ( number_range( 0, luck ) < 18 )
		animal_hit = ( animal_hit * 9 ) / 10;
	if ( number_range( 0, wis ) > 108 )
		animal_hit = ( animal_hit * 11 ) / 10;
	if ( number_range( 0, wis ) < 30 )
		animal_hit = ( animal_hit * 9 ) / 10;

	ch->counter[ 4 ] = 1;

	mob_index = get_mob_index(vnum);
	if (!mob_index)
	   {
	      send_to_char("Ju¿ wydawa³o ci siê, ¿e czar siê uda³ - ale nic siê nie dzieje.\n\r", ch );
	      bugf("nature_ally_nth: NULL mob_index for vnum %d", vnum );
	      return;
	   }


	monster = create_mobile( mob_index );

	if (!IS_NPC(ch))
	{
	  sprintf( buf, "spell_nature_ally_%s: [%5d], %s, %d %s.", spell_name, ch->in_room->vnum, ch->name, vnum, monster->name );
		log_string( buf );
	}

	char_to_room( monster, ch->in_room );
	EXT_SET_BIT( monster->act, ACT_NO_EXP );
    money_reset_character_money( monster );
	monster->level = animal_level;
	monster->hit = animal_hit;
	monster->max_hit = monster->hit;
    if ( fixed_perm_stats )
    {
        monster->perm_stat[ STAT_STR ] = (animal_level-2) * 3 + number_range( 72, 96 );
        monster->perm_stat[ STAT_DEX ] = number_range(72, 90) + (animal_level-2) *3 ;
        monster->perm_stat[ STAT_WIS ] = 30 + animal_level * 3;
        monster->perm_stat[ STAT_INT ] = 18 + animal_level / 4 * 6;
        monster->perm_stat[ STAT_CON ] = animal_level * 6 + number_range(0, 18);
        monster->perm_stat[ STAT_CHA ] = 78;
        monster->perm_stat[ STAT_LUC ] = 78;

        monster->damage[ DICE_NUMBER ] = URANGE( 1, number_range( animal_level-2 , animal_level+1 ) / 4, 4 );
        monster->damage[ DICE_TYPE ] = URANGE( 3, 3 + ( number_range( 10, level ) > 12 ? 1 : 0 ) + ( number_range( 10, level ) > 24 ? 1 : 0 ), 5 );
        monster->damage[ DICE_BONUS ] = ( wis/6 - number_range(16,14) ) / number_range(3,4);
        monster->hitroll = ( wis/6 - 15 ) / number_range(3,4);
    }
	EXT_SET_BIT( monster->act, ACT_WARRIOR );
	EXT_SET_BIT( monster->off_flags, OFF_RESCUE );
	monster->hit = get_max_hp( monster );

	if ( level > fast_above_level )
    {
        EXT_SET_BIT( monster->off_flags, OFF_FAST );
    }

	act( "$N pojawia siê.", ch, NULL, monster, TO_ROOM );

	if ( !add_charm( ch, monster, TRUE ) )
	{
		EXT_SET_BIT( monster->act, ACT_AGGRESSIVE );
		create_event( EVENT_EXTRACT_CHAR, duration/2, monster, NULL, 0 );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level	= level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( monster, &af, NULL, TRUE );

	add_follower( monster, ch, TRUE );

	create_event( EVENT_EXTRACT_CHAR, duration, monster, NULL, 0 );

	return;
}

void spell_nature_ally_I( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  const int tab[] = { MOB_VNUM_WIEWIORKA,
		      MOB_VNUM_KRUK,
		      MOB_VNUM_BORSUK, MOB_VNUM_BORSUK,
		      MOB_VNUM_WIEWIORKA_M, MOB_VNUM_WIEWIORKA_M,
		      MOB_VNUM_DZIK };

  spell_nature_ally_Nth( sn, level, ch, vo, target, 138, 72,  10, "I", 16, TRUE, 1, 7, tab, 18, 10, 14 );
}
void spell_nature_ally_II( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  const int tab[] = { MOB_VNUM_WOLF, MOB_VNUM_WOLF,
		      MOB_VNUM_BEAR,
		      MOB_VNUM_TIGER,
		      MOB_VNUM_ORZEL_M };

  spell_nature_ally_Nth( sn, level, ch, vo, target, 144, 1000, 16, "II", 20, TRUE, 7, 11, tab, 18, 11, 15 );
}
void spell_nature_ally_III( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  const int tab[] = { MOB_VNUM_ORZEL_M,
		      MOB_VNUM_WILK_M, MOB_VNUM_WILK_M,
		      MOB_VNUM_BEAR_M,
		      MOB_VNUM_TYGRYS_M,
		      MOB_VNUM_DUSICIEL_M,
		      MOB_VNUM_PAJAK_M };

  spell_nature_ally_Nth( sn, level, ch, vo, target, 150, 1000, 22, "III", 24, TRUE, 11, 16, tab, 18, 12, 16 );
}
void spell_nature_ally_IV( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  int tab[] = { MOB_VNUM_ELEMENTAL_P, MOB_VNUM_ELEMENTAL_P,
		MOB_VNUM_ELEMENTAL_W, MOB_VNUM_ELEMENTAL_W,
		MOB_VNUM_ELEMENTAL_Z, MOB_VNUM_ELEMENTAL_Z,
		MOB_VNUM_ELEMENTAL_O, MOB_VNUM_ELEMENTAL_O };

  spell_nature_ally_Nth( sn, level, ch, vo, target, 156, 1000, 25, "IV", 28, FALSE, 16, 22, tab, 40, 13, 17 );
}


// Tener (8-05-2008):
// refaktoring kodu dla czarów przywo³uj±cych o¿ywione drzewa

void summon_ent( int sn, int level, CHAR_DATA *ch, void *vo, int target,
		 char too_low_charisma_msg[],
		 char no_trees_msg[],
		 char cast_fail_msg[],
		 char msg_summon_to_room[],
		 int lev_limit_low,
		 int lev_limit_hi,
		 int mob_vnum )
{
	CHAR_DATA * pet;
	AFFECT_DATA af;
	int luck = get_curr_stat( ch, STAT_LUC ), charisma, lev_reduction = 0;

	if ( IS_NPC( ch ) )
		return;

	// jak kto¶ nie chce byæ followany
	if ( EXT_IS_SET( ch->act, PLR_NOFOLLOW ) )
	{
		send_to_char( "Przecie¿ nie chcesz ¿eby kto¶ za tob± chodzi³.\n\r", ch );
		return;
	}

	charisma = get_curr_stat( ch, STAT_CHA );
	if ( charisma < 66 )
	{
		print_char( ch, too_low_charisma_msg );
		return;
	}

	if ( ch->in_room->sector_type != 3 &&
	     ch->in_room->sector_type != 4 &&
	     ch->in_room->sector_type != 11 &&
	     ch->in_room->sector_type != 30 &&
	     ch->in_room->sector_type != 38 )
	{
		send_to_char( no_trees_msg , ch );
		return;
	}

	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) < 1 )
	{
		send_to_char( cast_fail_msg , ch );
		return;
	}

	//rellik: komponenty, sprawdzanie, tutaj ma³e zamieszanie bo funkcja wywo³ywana jest z 3 czarów, ale powinien po sn rozpoznawaæ
	if( !spell_item_check( ch, sn , "liveoak mocny" ) )
	{
		if ( !spell_item_check( ch, sn , "liveoak slaby" ) )
		{
			print_char( ch, "Nie ma z czego stworzyæ drzewa.\n\r" );
			return;
		} else {
			lev_reduction = 4;
		}
	}


	pet = create_mobile( get_mob_index( mob_vnum ) );

	char_to_room( pet, ch->in_room );
	EXT_SET_BIT( pet->act, ACT_NO_EXP );
    money_reset_character_money( pet );
	pet->level = UMAX( lev_limit_low, URANGE( lev_limit_low, number_range( level - 7, level ), lev_limit_hi  ) - lev_reduction );
	if ( number_range( 0, luck ) > 90 ) pet->level += 1;
	if ( number_range( 0, luck ) < 18 ) pet->level -= 1;
	pet->hit = 40 + pet->level * number_range( 15, 22 );
	pet->max_hit = pet->hit;
	pet->damage[ DICE_NUMBER ] = pet->level / 4;
	pet->damage[ DICE_TYPE ] = pet->level / 4;
	pet->damage[ DICE_BONUS ] = pet->level / 6;
	pet->hitroll = pet->level / 9;
	pet->hit = get_max_hp( pet );

	if ( !add_charm( ch, pet, TRUE ) )
	{
		extract_char( pet, TRUE );
		return;
	}
	act( msg_summon_to_room , ch, NULL, pet, TO_ROOM );
	act( "$N bêdzie ci teraz s³u¿yæ.", ch, NULL, pet, TO_CHAR );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( pet, &af, NULL, TRUE );

	add_follower( pet, ch, TRUE );

	if ( ch->fighting )
		set_fighting( pet, ch->fighting );

	create_event( EVENT_EXTRACT_CHAR, 20 * get_curr_stat( ch, STAT_WIS ), pet, NULL, 0 );

	return;
}

void spell_bark_guardian( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   summon_ent( sn, level, ch, vo, target,
	       "Nie dasz rady przekonaæ korowego stra¿nika by ci s³u¿y³.\n\r",
	       "Nie ma w pobli¿u ¿adnych drzew.\n\r",
	       "Nie uda³o ci siê o¿ywiæ korowego stra¿nika.\n\r",
	       "$n pobudza jedno z okolicznych drzew do ¿ycia.",
	       8, 12,
	       MOB_VNUM_BARKGUARD );
}

void spell_liveoak( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   summon_ent( sn, level, ch, vo, target,
	       "Nie dasz rady przekonaæ dêbu by ci s³u¿y³.\n\r",
	       "Nie ma w pobli¿u ¿adnych dêbów.\n\r",
	       "Nie uda³o ci siê o¿ywiæ dêbu.\n\r",
	       "$n pobudza jedno z okolicznych drzew do ¿ycia.",
	       12, 18,
	       MOB_VNUM_LIVEOAK );
}

void spell_wildthorn( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   summon_ent( sn, level, ch, vo, target,
	       "Nie dasz rady przekonaæ dzikiego ciernia by ci s³u¿y³.\n\r",
	       "Nie ma w pobli¿u ¿adnych drzew.\n\r",
	       "Nie uda³o ci siê o¿ywiæ dzikiego ciernia.\n\r",
	       "$n pobudza jedno z okolicznych drzew do ¿ycia.",
	       18, 22,
	       MOB_VNUM_WILDTHORN );
}

/* Tener: refaktoring czarów lecz±cych i poprawienie komunikatów [20080523] */

void heal_specific_race_type( int sn, int level, CHAR_DATA *ch, void *vo, int target,
			      const char comm_bad_race[], int target_type,
			      int dice_rolls, int dice_sides, int level_mod,
			      const char heal_msg_table[6][MAX_STRING_LENGTH] )
{

	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), heal_value;
	int heal_percent;

	if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , target_type ) )
	{
		act( comm_bad_race, ch, NULL, victim, TO_CHAR );
		return;
	}

	heal_value = dice( dice_rolls, dice_sides ) + level*level_mod;

	if ( level > 30 ) heal_value += 10;

	// modyfikator zale¿ny od szczê¶cia dodatni
	if ( number_range( 0, luck ) > 15 ) heal_value = ( heal_value * 105 ) / 100;
	// modyfikator zale¿ny od szczê¶cia ujemny
	if ( number_range( 0, luck + LUCK_BASE_MOD ) < 3 ) heal_value = ( heal_value * 9 ) / 10;
	// modyfikator dla strasznego pecha
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}


	if( victim->hit == get_max_hp( victim ) )
	   return;

	heal_percent = UMIN( get_max_hp( victim ) + 11 - victim->hit, heal_value );
	heal_percent = 100 * heal_percent / get_max_hp( victim );

	victim->hit = UMIN( victim->hit + heal_value, get_max_hp( victim ) );


	if ( victim->hit == victim->max_hit )
	{
		switch (victim->sex)
		{
			case 0:
				act( "$z wygl±da na w pe³ni zdrowe.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$z wygl±da na w pe³ni zdrowego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
			case 2:
				act( "$z wygl±da na w pe³ni zdrow±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
	}
	else if ( heal_percent > 75 )
	{
		act( heal_msg_table[0], victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_percent > 50 )
	{
		act( heal_msg_table[1], victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_percent > 35 )
	{
		act( heal_msg_table[2], victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_percent > 20 )
	{
		act( heal_msg_table[3], victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_percent > 5 )
	{
		act( heal_msg_table[4], victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_percent > 0 )
	{
		act( heal_msg_table[5], victim, NULL, NULL, TO_ROOM );
	}

	update_pos( victim );
	return;

}

/* komunikaty wykorzystywane w leczeniach. ma byæ ich tyle, ile wynika z kodu w heal_specific_race_type */

const char heal_plant_msg_table [6][MAX_STRING_LENGTH] = {
   "Niemal wszystkie uszkodzenia $z goj± siê",
   "Wiekszo¶æ masywnych zniszczeñ $z goi siê.",
   "Kilka du¿ych uszkodzeñ $z goi siê.",
   "Kilka uszkodzeñ $z goi siê.",
   "Kilka zranieñ $z znika.",
   "Niektóre z zadrapañ $z znikaj±."
};

const char heal_animal_msg_table [6][MAX_STRING_LENGTH] = {
   "Prawie wszystkie rany $z goj± siê.",
   "Wiêkszo¶æ ran $z goi siê.",
   "Kilka g³êbokich ran $z goi siê.",
   "Kilka ran $z goi siê.",
   "Kilka zranieñ $z znika.",
   "Niektóre z siniaków $z znikaj±."
};



void spell_cure_animal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   heal_specific_race_type( sn, level, ch, vo, target, "$N chyba nie jest zwierzêciem?", ANIMAL,
			    4, 6, 1, heal_animal_msg_table );
}

void spell_reinvigore_animal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   heal_specific_race_type( sn, level, ch, vo, target, "$N chyba nie jest zwierzêciem?", ANIMAL,
			    8, 6, 2, heal_animal_msg_table );
}

void spell_heal_animal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   heal_specific_race_type( sn, level, ch, vo, target, "$N chyba nie jest zwierzêciem?", ANIMAL,
			    12, 6, 3, heal_animal_msg_table );
}

void spell_cure_plant( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   heal_specific_race_type( sn, level, ch, vo, target, "$N chyba nie jest ro¶lin±?", PLANT,
			    4, 6, 1, heal_plant_msg_table );
}

void spell_reinvigore_plant( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   heal_specific_race_type( sn, level, ch, vo, target, "$N chyba nie jest ro¶lin±?", PLANT,
			    8, 6, 2, heal_plant_msg_table );
}

void spell_heal_plant( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   heal_specific_race_type( sn, level, ch, vo, target, "$N chyba nie jest ro¶lin±?", PLANT,
			    12, 6, 3, heal_plant_msg_table );
}

/* koniec leczeñ ro¶lin i zwierz±t */

void spell_sense_fatigue( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    if ( IS_AFFECTED( ch, AFF_SENSE_FATIGUE ) )
    {
        send_to_char( "Potrafisz ju¿ przecie¿ wyczuwaæ zmêczenie.\n\r", ch );
        return;
    }
    AFFECT_DATA af;
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level / 2;
    af.rt_duration = 0;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector = &AFF_SENSE_FATIGUE;
    affect_to_char( ch, &af, NULL, TRUE );
    send_to_char( "Mimowolnie zaciskasz oczy, które zdaj± siê zamarzaæ. W chwilê pó¼niej przechodzi ciê fala ciep³a i unosisz powieki, zachwycaj±c siê widokiem teraz dostrzegalnej dla ciebie energii ¶wiata.\n\r", ch );
    act( "$n z dziwnym grymasem zaciska na chwilê powieki, a kiedy otwiera je z wyra¼n± ulg±, widzisz, ¿e jego oczy zyska³y niezwyk³± g³êbiê koloru." , ch, NULL, NULL, TO_ROOM );
    return;
}

