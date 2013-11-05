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
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: spells_mag.c 12217 2013-04-03 21:27:31Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/12.02/src/spells_mag.c $
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
#include "lang.h"
#include "music.h"
#include "tables.h"
#include "projects.h"
#include "mount.h"
#include "money.h"
#ifdef STEEL_SKIN_ON
//Nil: steel_skin
	#include "steel_skin.h"
#endif /* STEEL_SKIN_ON */

/**
 *
 * Czary maga
 *
 */

extern char * target_name;
CHAR_DATA *get_char_area( CHAR_DATA *ch, char *argument );

bool check_dispel          args( ( int dis_level, CHAR_DATA *victim, int sn ) );
bool check_improve_strenth args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool verbose ) );
bool check_shaman_invoke args( ( CHAR_DATA *ch ) );
bool saves_dispel          args( ( int dis_level, int spell_level, int duration ) );
int	find_door	           args( ( CHAR_DATA *ch, char *arg ) );
sh_int 	get_caster		   args( ( CHAR_DATA *ch) );
void mp_onload_trigger     args( ( CHAR_DATA *mob ) );
void raw_damage            args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam ) );
void raw_kill              args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void set_fighting          args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void wear_obj_silent       args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
void wield_weapon_silent   args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
int mind_based_spell_bonus args( ( CHAR_DATA *ch, int bonus ) );

/* Stone skin - odpornosc na okreslona ilosc obrazen fizycznych
 * schodzi tylko kiedy siê oberwie odpowiednio duzo strzalow
 * nie daje odpornosci na magiczne ataki
 * oczywiscie bedzie dodatek do broni (vorpal) ktory bedzie siê przebijal
 * przez kamien
 */
void spell_stone_skin( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration = 10, luck, mod;

	if ( IS_AFFECTED( victim, AFF_BARK_SKIN ) )
	{
		if ( victim == ch )
			send_to_char( "Ju¿ masz skórê przemienion± w korê.\n\r", ch );
		else
			act( "Skóra $Z jest ju¿ przeminiona w korê.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_STONE_SKIN ) )
	{
		if ( victim == ch )
			send_to_char( "Twoja skóra jest ju¿ twarda jak ska³a.\n\r", ch );
		else
			act( "Skóra $Z jest ju¿ wystarczajaco twarda.", ch, NULL, victim, TO_CHAR );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê przemieniæ swojej skóry.\n\r", ch );
		else
			act( "Nie uda³o ci siê przemieniæ skóry $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	mod = 2 + ( level / 6 );

	if ( number_range( 0, luck ) > 15 ) mod++;
	if ( number_range( 0, luck ) < 5 ) mod--;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration += level/10;
			mod += level/10;
		}
	}

	if ( IS_NPC(ch) )
	{
		duration += 2;
		mod += 2;
		//dla bossow
		if( level > 30 )
		{
			mod += 5;
			duration += 5;
		}
		else if( level > 26 )
		{
			mod += 2;
			duration += 2;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = mod;
	af.bitvector = &AFF_STONE_SKIN;
	affect_to_char( victim, &af, NULL, TRUE );
	act( "Skóra $z zamienia siê w kamieñ.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Twoja skóra zamienia siê w kamieñ.\n\r", victim );
	return;
}

void spell_slow( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration = 3, mod, luck, vluck;

	if ( is_affected( victim, sn ) || IS_AFFECTED( victim, AFF_SLOW ) )
	{
		act( "$N rozgl±da siê ju¿ do¶æ powolnie.", ch, NULL, victim, TO_CHAR );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê siebie spowolniæ.\n\r", ch );
		else
			act( "Nie uda³o ci siê spowolniæ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		if ( victim != ch )
			send_to_char( "Nic siê nie sta³o.\n\r", ch );
		send_to_char( "Czujesz chwilowe os³abienie.\n\r", victim );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_HASTE ) )
	{
		if ( !check_dispel( level, victim, skill_lookup( "haste" ) ) )
		{
			if ( victim != ch )
				send_to_char( "Rzucenie zaklêcia siê nie powiod³o.\n\r", ch );
			send_to_char( "Chwilowo wszytko wokó³ ciebie przyspieszy³o.\n\r", victim );
			return;
		}

		act( "$n przestaje poruszaæ siê tak szybko.", victim, NULL, NULL, TO_ROOM );
		return;
	}

	if ( number_range( 0, luck ) > 15 ) duration++;
	if ( number_range( 0, luck ) < 5 ) duration--;

	if ( number_range( 0, vluck ) > 15 ) duration--;
	if ( number_range( 0, vluck ) < 5 ) duration++;

	mod = -1 - ( level >= 18 ) - ( level >= 25 );

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration += 2;
			mod = -2 - ( level >= 15 ) - ( level >= 20 ) - ( level >= 25 );
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_DEX;
	af.modifier = UMIN( -1, mod );
	af.bitvector = &AFF_SLOW;
	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Wszystko wokó³ ciebie znacznie przyspiesza...\n\r", victim );
	act( "$n zaczyna poruszaæ siê troszkê wolniej.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_sleep( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int chance, duration = 1, luck, vluck;

	if ( IS_AFFECTED( victim, AFF_SLEEP ) || is_undead(victim) )
		return;

	if ( victim->resists[ RESIST_MENTAL ] > 0 && number_percent() < victim->resists[ RESIST_MENTAL ] )
		return;

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	/* uwaga, szansa jest "odwrotna", czyli im mniejsza tym lepiej dla victima */

	chance = dice( 3, 6 );
	chance -= ( vluck - luck ) / 4;
	chance -= ( get_curr_stat_deprecated(victim, STAT_WIS) - get_curr_stat_deprecated(ch,STAT_INT) ) / 4;//taki niby rzut na sile woli
	chance += level/10;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration += 1 + level / 15;
			chance += level / 5;
		}
	}

	/* bonus dla ró¿nicy p³ci */

	if ( ( ch->sex == SEX_MALE && victim->sex == SEX_FEMALE ) || ( ch->sex == SEX_FEMALE && victim->sex != SEX_FEMALE ) )
		chance += 2;

	if ( chance < victim->level || level < victim->level )
	{
		switch (ch->sex)
		{
		case SEX_NEUTRAL:
			act( "$n próbowa³o ciê u¶piæ!", ch, NULL, victim, TO_VICT );
			break;
		case SEX_MALE:
			act( "$n próbowa³ ciê u¶piæ!", ch, NULL, victim, TO_VICT );
			break;
		case SEX_FEMALE:
		default:
			act( "$n próbowa³a ciê u¶piæ!", ch, NULL, victim, TO_VICT );
			break;
		}
		act("Nie uda³o ci siê u¶piæ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_SLEEP;
	affect_to_char( victim, &af, NULL, TRUE );

	if ( IS_AWAKE( victim ) )
	{
		/* opadanie na ziemiê, je¿eli jest siê pod wp³ywem czaru "fly" */
		if ( IS_AFFECTED( victim, AFF_FLYING ) || IS_AFFECTED( victim, AFF_FLOAT ) )
		{
			act( "$n opada na ziemiê.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Powoli opadasz na ziemiê.\n\r", victim );
			affect_strip( victim, gsn_fly );
			affect_strip( victim, gsn_float );
		}

		send_to_char( "Czujesz ogarniaj±c± ciê senno¶æ... chrrr...chrrr...\n\r", victim );
		act( "$n k³adzie siê i zasypia.", victim, NULL, NULL, TO_ROOM );
		victim->position = POS_SLEEPING;
	}
	return;
}

void spell_shocking_grasp( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam,

	/* luck */

	luck = get_curr_stat_deprecated( ch, STAT_LUC ),
	       vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê poraziæ siebie wi±zk± energii elektrycznej.\n\r", ch );
		else
			act( "Nie uda³o ci siê stworzyæ wi±zki energii elektrycznej.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dam = dice( 3, 5 ) + (level * 2)/3;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dam = dice( 3, 7 ) + level;
		}
	}

	dam = luck_dam_mod( ch, dam );

			if( spell_item_check( ch, sn , "lighting based spell" ) )
    {
      dam += 6;
    }

	spell_damage( ch, victim, dam, sn, DAM_LIGHTNING , TRUE );
	shock_effect( victim, level, dam, TARGET_CHAR );
	return;
}

void spell_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, duration, mod;

	if ( IS_AFFECTED( victim, AFF_SHIELD ) )
	{
		send_to_char( "Ju¿ jeste¶ otoczon<&y/a/e> magiczn± tarcz±.\n\r", ch );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê otoczyæ siê magiczn± tarcz±.\n\r", ch );
		else
			act( "Nie uda³o ci siê otoczyæ $Z magiczn± tarcz±.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 6 + level/5;
	mod = 10 + level/2;
	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration += 1 + level/8;
			mod += level/2;
		}
	}

	/* bonus od szczê¶cia */
	if ( number_range( 0, luck ) > 15 ) duration *= 2;
	if ( number_range( 0, luck ) <  5 ) duration /= 2;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration );
    af.rt_duration = 0;
	af.location = APPLY_AC;
	af.modifier = -mod;
	af.bitvector = &AFF_SHIELD;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case SEX_NEUTRAL:
			act( "$n zostaje otoczone magiczn± tarcz±.", victim, NULL, NULL, TO_ROOM );
			break;
		case SEX_MALE:
			act( "$n zostaje otoczony magiczn± tarcz±.", victim, NULL, NULL, TO_ROOM );
			break;
		case SEX_FEMALE:
		default:
			act( "$n zostaje otoczona magiczn± tarcz±.", victim, NULL, NULL, TO_ROOM );
			break;
	}
	if ( victim == ch )
    {
        send_to_char( "Otaczasz siê magiczn± tarcz±.\n\r", ch );
    }
	else
    {
        act( "$N otacza ciê magiczn± tarcz±.", ch, NULL, victim, TO_VICT );
    }
	return;
}

void spell_pass_door( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration = 1;

	if ( IS_AFFECTED( victim, AFF_PASS_DOOR ) )
	{
		if ( victim == ch )
			send_to_char( "Ju¿ to potrafisz.\n\r", ch );
		else
			act( "$N ju¿ to potrafi.", ch, NULL, victim, TO_CHAR );
		return;
	}

	/* zale¿no¶æ od szczê¶cia */
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê siebie przemieniæ.\n\r", ch );
		return;
	}

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration++;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_PASS_DOOR;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Stajesz siê pó³przezroczyst<&y/a/e>.\n\r", victim );
	switch ( ch->sex )
	{
		case SEX_NEUTRAL:
			act( "$n staje siê pó³przezroczyste.", victim, NULL, NULL, TO_ROOM );
			break;
		case SEX_MALE:
			act( "$n staje siê pó³przezroczysty.", victim, NULL, NULL, TO_ROOM );
			break;
		case SEX_FEMALE:
		default:
			act( "$n staje siê pó³przezroczysta.", victim, NULL, NULL, TO_ROOM );
			break;
	}
	return;
}

void spell_mass_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	CHAR_DATA *gch;
	int duration = 20;
    int luck = get_curr_stat_deprecated( ch, STAT_LUC );
    int count = 0;
    int af_level = level / 2;

	/* zale¿no¶æ od szczê¶cia */
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration += 4;
			af_level = UMAX( 1, level - 2 );
		}
	}

	/* bonus od szczê¶cia */
	if ( number_range( 0, luck ) > 15 ) duration++;
	if ( number_range( 0, luck ) < 5 ) duration--;

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group( gch, ch ) || IS_AFFECTED( gch, AFF_INVISIBLE ) )
			continue;

		act( "$n powoli znika.", gch, NULL, NULL, TO_ROOM );
		send_to_char( "Powoli znikasz.\n\r", gch );

		af.where = TO_AFFECTS;
		af.type = 65;
		af.level = af_level;
		af.duration = duration;
        af.rt_duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &AFF_INVISIBLE;
		affect_to_char( gch, &af, NULL, TRUE );

		count++;
	}

	if ( count > 0 )
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie.\n\r", ch );
	else
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie, ale nic siê nie sta³o.\n\r", ch );

	return;
}

void spell_magic_missile( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int shots, dam, vluck, luck, damage_bonus = 0;

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
  vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	/* zale¿no¶æ od szczê¶cia */
	if ( number_range( 0, luck + LUCK_BASE_MOD - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ magicznych pocisków.\n\r", ch );
		return;
	}

	shots = 1 + ( level - 1 ) / 6;

	if ( number_range( 0, luck ) > 15 ) shots++;
	if ( number_range( 0, luck ) < 5 ) shots--;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			shots += URANGE( 1, level / 5, 3 );
			damage_bonus = 1;
		}
	}

	shots = UMAX( 1, shots );

	if ( IS_AFFECTED( victim, AFF_SHIELD ) )
	{
		if ( shots > 1 )
		{
			print_char( ch, "Twoje magiczne pociski odbijaj± siê od ochronnej tarczy %s.\n\r", victim->name2 );
			print_char( victim, "Magiczne pociski %s odbijaj± siê od twojej ochronnej tarczy.\n\r", ch->name2 );
			act( "Magiczne pociski $z odbijaj± siê od ochronnej tarczy $Z.", ch, NULL, victim, TO_NOTVICT );
		}
		else
		{
			print_char( ch, "Twój magiczny pocisk odbija siê od ochronnej tarczy %s.\n\r", victim->name2 );
			print_char( victim, "Magiczny pocisk %s odbija siê od twojej ochronnej tarczy.\n\r", ch->name2 );
			act( "Magiczny pocisk $z odbija siê od ochronnej tarczy $Z.", ch, NULL, victim, TO_NOTVICT );
		}
		return;
	}

	for ( ; shots > 0; shots-- )
	{
		if ( !victim->in_room || ch->in_room != victim->in_room )
			return;
		dam = dice( 2, 5 ) + damage_bonus;
		dam = luck_dam_mod( ch, dam );
		spell_damage( ch, victim, dam, sn, DAM_ENERGY , TRUE );

		if( victim->hit - dam < -11 )
			make_head( victim );
	}
	return;
}

/* mocny, uniwersalny offense */
void spell_lightning_bolt( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, dam_eff, vluck, luck = get_curr_stat_deprecated( ch, STAT_LUC );

  vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD - vluck) == 0 )
	{
		act( "Nie uda³o ci siê stworzyæ b³yskawicy.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dam = number_range( 20, 25 ) + dice( ( level * 3 ) / 4, 4 );
	dam_eff = dam / 2;

	/* Bonus dla specjalisty i mobow */
	if ( IS_NPC(ch) || ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) ) )
	{
			dam = number_range( 20, 25 ) + dice( level, 4 );
			dam_eff = dam;
	}

	dam = luck_dam_mod( ch, dam );

	/* spell save modyfikator */
	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		dam_eff /= 2;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

			if( !spell_item_check( ch, sn , "lighting based spell" ) )
    {
      dam -= dam/5;
    }

	spell_damage( ch, victim, dam, sn, DAM_LIGHTNING , TRUE );
	shock_effect( victim, level, dam_eff, TARGET_CHAR );

	return;
}

void spell_force_bolt( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, vluck, luck = get_curr_stat_deprecated( ch, STAT_LUC );

  vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ strumienia energii.\n\r", ch );
		return;
	}

	dam = 35 + dice( ( level * 3 ) / 4, 5 );

	/* Bonus dla specjalisty i mobow */
	if ( IS_NPC(ch) || ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) ) )
	{
		dam = 35 + dice( level, 5 );
	}

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

	spell_damage( ch, victim, dam, sn, DAM_BASH , TRUE );
	return;
}

void spell_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj;
	AFFECT_DATA af;

	int duration = 12 + level/5, luck = get_curr_stat_deprecated( ch, STAT_LUC ), af_level = level / 2, mod = 0;

	/* zale¿no¶æ od szczê¶cia */
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck ) > 15 ) duration += number_range( 1, UMAX( 1, level / 7.5 ) );
	if ( number_range( 0, luck ) < 4 ) duration -= number_range( 1, UMAX( 1, level / 7 ) );

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration += URANGE( 2, level / 4, 8 );
			af_level = level;
			mod = 25;
		}
	}

	/* object invisibility */
	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;

		if ( IS_OBJ_STAT( obj, ITEM_INVIS ) )
		{
			act( "Ten przedmiot ju¿ jest niewidzialny.", ch, obj, NULL, TO_CHAR );
			return;
		}

		af.where		= TO_OBJECT;
		af.type			= sn;
		af.level		= af_level;
		af.duration		= UMAX( 1, duration );
		af.rt_duration = 0;
		af.location		= APPLY_NONE;
		af.modifier		= mod;
		af.bitvector	= &ITEM_INVIS;
		affect_to_obj( obj, &af );

		act( "$p znika.", ch, obj, NULL, TO_ALL );
		return;
	}

	/* character invisibility */
	victim = ( CHAR_DATA * ) vo;

	if ( IS_AFFECTED( victim, AFF_INVISIBLE ) )
	{
		if ( ch == victim )
			send_to_char( "Przecie¿ jeste¶ ju¿ niewidzialn<&y/a/e>.\n\r", ch );
		else
			switch ( victim->sex )
			{
				case SEX_NEUTRAL:
					act( "$N ju¿ jest niewidzialne.", ch, NULL, victim, TO_CHAR );
					break;
				case SEX_MALE:
					act( "$N ju¿ jest niewidzialny.", ch, NULL, victim, TO_CHAR );
					break;
				case SEX_FEMALE:
				default:
					act( "$N ju¿ jest niewidzialna.", ch, NULL, victim, TO_CHAR );
					break;
			}
		return;
	}

	act( "$n znika.", victim, NULL, NULL, TO_ROOM );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = af_level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = mod;
	af.bitvector = &AFF_INVISIBLE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Stajesz siê niewidzialn<&y/a/e>.\n\r", victim );

	return;
}

void spell_darkvision( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int luck, duration, af_level = level / 2;

	if ( IS_AFFECTED( victim, AFF_DARK_VISION ) )
	{
		if ( victim == ch )
			send_to_char( "Przecie¿ widzisz w ciemno¶ci.\n\r", ch );
		else
			act( "$N posiada ju¿ dar widzenia w ciemno¶ciach.\n\r", ch, NULL, victim, TO_CHAR );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	/* zaleznosc od szczescia */
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	duration = 2 + level / 6;

	if ( number_range( 0, luck ) > 15 ) duration++;
	if ( number_range( 0, luck ) < 5 ) duration--;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration += 2;
			af_level = level;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = af_level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_DARK_VISION;
	affect_to_char( victim, &af, NULL, TRUE );

	if ( ch != victim )
	{
		act( "Twoje zaklêcie sprawia, ¿e oczy $Z zaczynaj± lekko ¶wieciæ zielonym blaskiem.", ch, NULL, victim, TO_CHAR );
		act( "Czar $z sprawia, ¿e oczy $Z zaczynaj± lekko ¶wieciæ zielonym blaskiem.", ch, NULL, victim, TO_NOTVICT );
		send_to_char( "Twoje oczy zaczynaj± lekko ¶wieciæ zielonym blaskiem.\n\r", victim );
	}
	else
	{
		act( "Oczy $z zaczynaj± lekko ¶wieciæ zielonym blaskiem.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "Twoje oczy zaczynaj± lekko ¶wieciæ zielonym blaskiem.\n\r", ch );
	}
	return;
}

void spell_identify( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA * obj = ( OBJ_DATA * ) vo;
    char buf[ MAX_STRING_LENGTH ];
    AFFECT_DATA *paf;
    extern int port;
    int luck, hidden = 0, only_rasa = 0, only_profka = 0;
    bool mage_specialist_divination = FALSE; //czy jest poznaniakiem albo ma stosowne komponenty

    if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
    {
        mage_specialist_divination = TRUE;
    }
    if ( target == TARGET_SAGE ) // zabezpieczenie na wszelki wypadek, zeby sage nie mial mastera
    {
        mage_specialist_divination = FALSE;
    }

    if ( IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
    {
        if ( target == TARGET_SAGE )
        {
            send_to_char( "Nie dasz rady poznaæ w³a¶ciwo¶ci tego przedmiotu.\n\r", ch );
        }
        else
        {
            send_to_char( "Nie dasz rady zidentyfikowaæ tego przedmiotu.\n\r", ch );
        }
        return;
    }

    luck = get_curr_stat_deprecated( ch, STAT_LUC );

    /* zale¿no¶æ od szczê¶cia */
    if ( target != TARGET_SAGE && number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
    {
        send_to_char( "{RNie uda³o ci siê rzuciæ zaklêcia{x.\n\r", ch );
        return;
    }

    if ( !IS_NPC( ch ) && !mage_specialist_divination )
    {
        if ( spell_item_check( ch, sn, "ident jak poznaniak" ) )
        {
            mage_specialist_divination = TRUE;
            print_char( ch, "\n\r" );
        }
    }

    if ( target == TARGET_SAGE )
    {
        act( "{GPoznajesz niektóre z w³a¶ciwo¶ci $f{G.{x", ch, obj, NULL, TO_CHAR );
    }
    else
    {
        act ( "{GOstro¿nie dotykasz $f{G i zdajesz sobie sprawê, ¿e wiesz prawie wszystko o tym przedmiocie.{x", ch, obj, NULL, TO_CHAR );
    }

    if ( obj->ident_description && obj->ident_description[ 0 ] != '\0' )
    {
        send_to_char( obj->ident_description, ch );
        send_to_char( "\n\r", ch );
    }

    if ( obj->weight > 5 )
        print_char( ch, "Waga %s wynosi oko³o %.2f kg",
                obj->name2,
                obj->weight / 22.05 );
    else
        print_char( ch, "%s prawie nic nie wa¿y", capitalize( obj->short_descr ) );

    if ( obj->material > 0 )
    {
        print_char( ch, ", przedmiot ten wykonano z materia³u '%s'.\n\r",
                material_table[ obj->material ].name );
    }
    else
    {
        send_to_char( ".\n\r", ch );
    }

    if ( target != TARGET_SAGE ) // sage wartosci nie pokazuje, od tego jest value
    {
        if ( obj->cost <= 1 )
        {
            print_char(
                    ch,
                    "%sZa %s%s w sklepie nie dosta³%sby¶ nawet jednej, miedzianej monety.{x\n\r",
                    MONEY_COLOR( NOMINATION_COPPER ),
                    obj->name4,
                    MONEY_COLOR( NOMINATION_COPPER ),
                    ch->sex == SEX_FEMALE ? "a" : ch->sex == SEX_NEUTRAL ? "o" : ""
                    );
        }
        else if ( obj->cost <= 10 )
        {
            print_char(
                    ch,
                    "%sZa %s%s w sklepie dosta³%sby¶ kilka miedzianych monet.{x\n\r",
                    MONEY_COLOR( NOMINATION_COPPER ),
                    obj->name4,
                    MONEY_COLOR( NOMINATION_COPPER ),
                    ch->sex == SEX_FEMALE ? "a" : ch->sex == SEX_NEUTRAL ? "o" : ""
                    );
        }
        else if ( obj->cost < RATTING_SILVER )
        {
            print_char(
                    ch,
                    "%sZa %s%s w sklepie dosta³%sby¶ prawiê jedn± srebrn± monetê{x.\n\r",
                    MONEY_COLOR( NOMINATION_SILVER ),
                    obj->name4,
                    MONEY_COLOR( NOMINATION_SILVER ),
                    ch->sex == SEX_FEMALE ? "a" : ch->sex == SEX_NEUTRAL ? "o" : ""
                    );
        }
        else if ( obj->cost < RATTING_GOLD * 3 / 4 )
        {
            print_char(
                    ch,
                    "%sZa %s%s w sklepie dosta³%sby¶ parê srebrnych monet.{x\n\r",
                    MONEY_COLOR( NOMINATION_SILVER ),
                    obj->name4,
                    MONEY_COLOR( NOMINATION_SILVER ),
                    ch->sex == SEX_FEMALE ? "a" : ch->sex == SEX_NEUTRAL ? "o" : ""
                    );
        }
        else if ( obj->cost < RATTING_GOLD )
        {
            print_char(
                    ch,
                    "%sZa %s%s w sklepie dosta³%sby¶ prawiê jedn± z³ot± monetê.{x\n\r",
                    MONEY_COLOR( NOMINATION_GOLD ),
                    obj->name4,
                    MONEY_COLOR( NOMINATION_GOLD ),
                    ch->sex == SEX_FEMALE ? "a" : ch->sex == SEX_NEUTRAL ? "o" : ""
                    );
        }
        else if ( obj->cost <= RATTING_MITHRIL * 3 / 4 )
        {
            print_char(
                    ch,
                    "%sZa %s%s w sklepie dosta³%sby¶ kilka z³otych monet.{x\n\r",
                    MONEY_COLOR( NOMINATION_GOLD ),
                    obj->name4,
                    MONEY_COLOR( NOMINATION_GOLD ),
                    ch->sex == SEX_FEMALE ? "a" : ch->sex == SEX_NEUTRAL ? "o" : ""
                    );
        }
        else if ( obj->cost < RATTING_MITHRIL )
        {
            print_char(
                    ch,
                    "%sZa %s%s w sklepie dosta³%sby¶ prawie jedn± mithrilow± monetê.{x\n\r",
                    MONEY_COLOR( NOMINATION_MITHRIL ),
                    obj->name4,
                    MONEY_COLOR( NOMINATION_MITHRIL ),
                    ch->sex == SEX_FEMALE ? "a" : ch->sex == SEX_NEUTRAL ? "o" : ""
                    );
        }
        else if ( obj->cost < number_range( 3, 6) * RATTING_MITHRIL )
        {
            print_char(
                    ch,
                    "%sZa %s%s w sklepie dosta³%sby¶ kilka mithrilowych monet.{x\n\r",
                    MONEY_COLOR( NOMINATION_MITHRIL ),
                    obj->name4,
                    MONEY_COLOR( NOMINATION_MITHRIL ),
                    ch->sex == SEX_FEMALE ? "a" : ch->sex == SEX_NEUTRAL ? "o" : ""
                    );
        }
        else
        {
            print_char( ch, "{R%s{R to niezwykle cenny przedmiot.{x\n\r", capitalize( obj->short_descr ) );
        }
        /**
         * repair limits
         */
        if (
                !EXT_IS_SET( obj->extra_flags, ITEM_NOREPAIR )
                && obj->item_type != ITEM_GEM
                && obj->item_type != ITEM_TREASURE
                && obj->item_type != ITEM_POTION
                && obj->item_type != ITEM_FOOD
                && obj->item_type != ITEM_CORPSE_NPC
                && obj->item_type != ITEM_CORPSE_PC
                && obj->item_type != ITEM_PILL
                && obj->item_type != ITEM_HERB
           )
        {
            if ( obj->repair_condition < 1 )
            {
                print_char( ch, "{R%s{R nie da siê ju¿ wiêcej naprawiæ.{x\n\r", capitalize( obj->name2 ) );
            }
            else if ( obj->repair_limit > 0 )
            {
                int repair = obj->repair_limit - obj->repair_counter;
                if ( repair > 9 )
                {
                    print_char( ch, "{R%s{R mo¿na naprawiæ jeszcze wiele razy.{x\n\r", capitalize( obj->name4 ) );
                }
                else if ( repair > 1 )
                {
                    print_char( ch, "{R%s{R da siê naprawiæ kilka razy.{x\n\r", capitalize( obj->name4 ) );
                }
                else if ( repair == 1 )
                {
                    print_char( ch, "{R%s{R da siê naprawiæ ju¿ tylko raz.{x\n\r", capitalize( obj->short_descr ) );
                }
                else
                {
                    print_char( ch, "{R%s{R nie da siê ju¿ wiêcej naprawiæ.{x\n\r", capitalize( obj->name2 ) );
                }
            }
            else
            {
                print_char( ch, "{G%s{G mo¿na naprawiaæ jeszcze wiele razy.{x\n\r", capitalize( obj->name4 ) );
            }
        }
    }

    if ( IS_OBJ_STAT( obj, ITEM_UNDESTRUCTABLE ) )
        print_char( ch, "{G%s emanuje niezwyk³± magi±, ¿adna moc nie by³aby w stanie zniszczyc tego przedmiotu.{x\n\r",
                capitalize( obj->short_descr ) );

    if ( IS_OBJ_STAT( obj, ITEM_EVIL ) )
        print_char( ch, "{COd %s{C bije zimna aura z³a.{x\n\r", obj->name2 );

    if ( IS_OBJ_STAT( obj, ITEM_DARK ) )
        print_char( ch, "Od %s bije mroczna aura.\n\r", obj->name2 );

    if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
        print_char( ch, "Dotykaj±c %s odczuwasz delikatn±, magiczn± aurê.\n\r", obj->name4 );

    if ( IS_OBJ_STAT( obj, ITEM_BURN_PROOF ) )
        send_to_char( "{YPrzedmiot ten wydaje siê byæ odporny na jakiekolwiek oddzia³ywanie ognia.{x\n\r", ch );

    if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
        print_char( ch, "%s emanuje lekk±, koj±c± aur±.\n\r", obj->short_descr );

    if ( !IS_NPC( ch ) && ch->level > number_range(10,15) )//takie zabezpieczonko, by gracze nie robili sobie specow poznania tylko do identa, expiac tylko do 4 lewa
    {
        if ( mage_specialist_divination )
        {
            if ( obj->timer > 0 )
            {
                print_char( ch, "{RPrzedmiot ten rozsypie siê za %d %s.{x\n\r", obj->timer, hour_to_str( obj->timer ) );
            }

            if ( IS_OBJ_STAT( obj, ITEM_NODROP ) && IS_OBJ_STAT( obj, ITEM_NOREMOVE ) && IS_OBJ_STAT( obj, ITEM_NOUNCURSE ) )
            {
                send_to_char( "{mPrzedmiot zosta³ ob³o¿ony wieczn± kl±tw± uniemo¿liwiaj±c± wypuszczenie go z rêki oraz pozbycia siê go.{x\n\r", ch );
            }
            else if ( IS_OBJ_STAT( obj, ITEM_NODROP ) && IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
            {
                send_to_char( "{mPrzedmiot zosta³ ob³o¿ony kl±tw± uniemo¿liwiaj±c± wypuszczenie go z rêki oraz pozbycia siê go.{x\n\r", ch );
            }
            else
            {
                if ( IS_OBJ_STAT( obj, ITEM_NOUNCURSE ))
                {
                    if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
                        send_to_char( "{mPrzedmiot zosta³ ob³o¿ony wieczn± kl±tw± uniemo¿liwiaj±c± pozbycie siê go.{x\n\r", ch );
                    else if ( IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
                        send_to_char( "{mPrzedmiot zosta³ ob³o¿ony wieczn± kl±tw± uniemo¿liwiaj±c± wypuszczenie go z rêki.{x\n\r", ch );
                }
                else
                {
                    if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
                        send_to_char( "{mPrzedmiot zosta³ ob³o¿ony kl±tw± uniemo¿liwiaj±c± pozbycie siê go.{x\n\r", ch );
                    else if ( IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
                        send_to_char( "{mPrzedmiot zosta³ ob³o¿ony kl±tw± uniemo¿liwiaj±c± wypuszczenie go z rêki.{x\n\r", ch );
                }
            }

            if ( IS_OBJ_STAT( obj, ITEM_ANTI_GOOD ))
                send_to_char( "Przedmiotu tego nie mog± u¿ywaæ istoty dobre.\n\r", ch );

            if ( IS_OBJ_STAT( obj, ITEM_ANTI_EVIL ))
                send_to_char( "Przedmiotu tego nie mog± u¿ywaæ istoty z³e.\n\r", ch );

            if ( IS_OBJ_STAT( obj, ITEM_ANTI_NEUTRAL ))
                send_to_char( "Przedmiotu tego nie mog± u¿ywaæ istoty neutralne.\n\r", ch );

            if ( IS_OBJ_STAT( obj, ITEM_ROT_DEATH ))
                send_to_char( "Przedmiot ten wyparuje po ¶mierci w³a¶ciciela.\n\r", ch );

            if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ))
                send_to_char( "Przedmiot ten wyparuje po upuszczeniu go na ziemiê.\n\r", ch );

            if ( IS_OBJ_STAT( obj, ITEM_UNDEAD_INVIS ))
                send_to_char( "Przedmiot ten jest niewidzialny dla nieumar³ych.\n\r", ch );

            if ( IS_OBJ_STAT( obj, ITEM_ANIMAL_INVIS ))
                send_to_char( "Przedmiot ten jest niewidzialny dla zwierz±t.\n\r", ch );

            //i exweary
            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYHUMAN ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko ludzie.\n\r", ch );
                ++only_rasa;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYELF ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko elfy.\n\r", ch );
                ++only_rasa;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYDWARF ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko krasnoludy.\n\r", ch );
                ++only_rasa;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYGNOM ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko gnomy.\n\r", ch );
                ++only_rasa;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYHALFELF ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko pó³elfy.\n\r", ch );
                ++only_rasa;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYHALFLING ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko nizio³ki.\n\r", ch );
                ++only_rasa;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYHALFORC ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko pó³orki.\n\r", ch );
                ++only_rasa;
            }

            if( only_rasa > 1 )
                bugf( "Obiekt %s [%d] ma dwie exwear flagi typu only - rasa.\n\r", obj->name, obj->pIndexData->vnum );

            if( !only_rasa )
            {
                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOHUMAN ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ ludzie.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOELF ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ elfy.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NODWARF ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ krasnoludy.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOGNOM ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ gnomy.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOHALFELF ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ pó³elfy.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOHALFLING ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ nizio³ki.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOHALFORC ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ pó³orki.\n\r", ch );
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYMAG ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko magowie.\n\r", ch );
                ++only_profka;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYCLE ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko klerycy.\n\r", ch );
                ++only_profka;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYTHI ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko z³odzieje.\n\r", ch );
                ++only_profka;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYWAR ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko wojownicy.\n\r", ch );
                ++only_profka;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYPAL ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko paladyni.\n\r", ch );
                ++only_profka;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYDRU ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko druidzi.\n\r", ch );
                ++only_profka;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYBARD ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko bardowie.\n\r", ch );
                ++only_profka;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYBLACKKNIGHT ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko czarni rycerze.\n\r", ch );
                ++only_profka;
            }

            if ( EXT_IS_SET( obj->wear_flags2, ITEM_ONLYBAR ) )
            {
                send_to_char( "Przedmiot ten mog± u¿ywaæ tylko barbarzyñcy.\n\r", ch );
                ++only_profka;
            }

            if( only_rasa > 1 )
                bugf( "Obiekt %s [%d] ma dwie exwear flagi typu only - profesja.\n\r", obj->name, obj->pIndexData->vnum );

            if( !only_profka )
            {
                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOMAG ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ magowie.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOCLE ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ klerycy.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOTHI ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ z³odzieje.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOWAR ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ wojownicy.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOPAL ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ paladyni.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NODRUID ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ druidzi.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOBARD ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ bardowie.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOBLACKKNIGHT ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ czarni rycerze.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOBARBARIAN ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ barbarzyñcy.\n\r", ch );

                if ( EXT_IS_SET( obj->wear_flags2, ITEM_NOSHAMAN ) )
                    send_to_char( "Przedmiotu tego nie mog± u¿ywaæ szamani.\n\r", ch );
            }
        }
    }

    /**
     * ITEM_NOREPAIR
     */
    if ( EXT_IS_SET( obj->extra_flags, ITEM_NOREPAIR ) )
    {
        print_char( ch, "{mWszystko wskazuje na to, ¿e %s{m nie uda siê naprawiæ.{x\n\r", obj->name2 );
    }

    switch ( obj->item_type )
    {
        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
            if ( target != TARGET_SAGE ) // sage nie pokazuje magii
            {
                sprintf( buf, "Poziom %d czarów:", obj->value[ 0 ] );
                send_to_char( buf, ch );

                if ( obj->value[ 1 ] > 0 && obj->value[ 1 ] < MAX_SKILL )
                {
                    send_to_char( " '", ch );
                    send_to_char( skill_table[ obj->value[ 1 ] ].name, ch );
                    send_to_char( "'", ch );
                }

                if ( obj->value[ 2 ] > 0 && obj->value[ 2 ] < MAX_SKILL )
                {
                    send_to_char( " '", ch );
                    send_to_char( skill_table[ obj->value[ 2 ] ].name, ch );
                    send_to_char( "'", ch );
                }

                if ( obj->value[ 3 ] > 0 && obj->value[ 3 ] < MAX_SKILL )
                {
                    send_to_char( " '", ch );
                    send_to_char( skill_table[ obj->value[ 3 ] ].name, ch );
                    send_to_char( "'", ch );
                }

                if ( obj->value[ 4 ] > 0 && obj->value[ 4 ] < MAX_SKILL )
                {
                    send_to_char( " '", ch );
                    send_to_char( skill_table[ obj->value[ 4 ] ].name, ch );
                    send_to_char( "'", ch );
                }

                send_to_char( ".\n\r", ch );
            }
            break;

        case ITEM_STAFF:
            if ( target != TARGET_SAGE ) // sage nie pokazuje magii
            {
                sprintf
                    (
                     buf,
                     "Ma jeszcze %d ³adunków poziomu %d.\n\r",
                     obj->value[ 2 ],
                     obj->value[ 0 ]
                    );
                send_to_char( buf, ch );
                if ( obj->value[ 3 ] >= 0 && obj->value[ 3 ] < MAX_SKILL )
                {
                    send_to_char( " '", ch );
                    send_to_char( skill_table[ obj->value[ 3 ] ].name, ch );
                    send_to_char( "'", ch );
                }
                send_to_char( ".\n\r", ch );
            }
            break;

        case ITEM_DRINK_CON:
            if ( obj->value[ 1 ] > 0 )
            {
                sprintf( buf, "W tym jest %s (%s).\n\r",
                        liq_table[ obj->value[ 2 ] ].liq_name,
                        liq_table[ obj->value[ 2 ] ].liq_color );
                send_to_char( buf, ch );
            }
            break;

        case ITEM_CONTAINER:
            sprintf( buf, "Pojemno¶æ: %d#  Maksymalna no¶no¶æ: %d#  flagi: %s\n\r",
                    obj->value[ 0 ], obj->value[ 3 ], cont_bit_name( obj->value[ 1 ] ) );
            send_to_char( buf, ch );
            if ( obj->value[ 4 ] != 100 )
            {
                sprintf( buf, "Mno¿nik wagi: %d%%\n\r",
                        obj->value[ 4 ] );
                send_to_char( buf, ch );
            }
            break;

        case ITEM_WEAPON:
            send_to_char( "Typ broni: ", ch );
            switch ( obj->value[ 0 ] )
            {
                case( WEAPON_EXOTIC ) : send_to_char( "'nieznany'.\n\r", ch ); break;
                case( WEAPON_SWORD ) : send_to_char( "'miecz'.\n\r", ch ); break;
                case( WEAPON_DAGGER ) : send_to_char( "'sztylet'.\n\r", ch ); break;
                case( WEAPON_SPEAR ) : send_to_char( "'w³ócznia'.\n\r", ch ); break;
                case( WEAPON_MACE ) : send_to_char( "'maczuga'.\n\r", ch ); break;
                case( WEAPON_AXE ) : send_to_char( "'topór'.\n\r", ch ); break;
                case( WEAPON_FLAIL ) : send_to_char( "'korbacz'.\n\r", ch ); break;
                case( WEAPON_WHIP ) : send_to_char( "'bat'.\n\r", ch ); break;
                case( WEAPON_POLEARM ) : send_to_char( "'broñ drzewcowa'.\n\r", ch ); break;
                case( WEAPON_STAFF ) : send_to_char( "'laska'.\n\r", ch ); break;
                case( WEAPON_SHORTSWORD ) : send_to_char( "'krótki miecz'.\n\r", ch ); break;
                case( WEAPON_CLAWS )   :  send_to_char( "'szpony'.\n\r", ch ); break;
                default : send_to_char( "nieznanego.\n\r", ch ); break;
            }

            sprintf( buf, "Bonus do trafienia: %d.\n\r", obj->value[ 5 ] );
            send_to_char( buf, ch );

            sprintf( buf, "Obra¿enia zadawane %dd%d + %d (¶rednio %d).\n\r",
                    obj->value[ 1 ], obj->value[ 2 ], obj->value[ 6 ],
                    ( obj->value[ 1 ] * obj->value[ 2 ] + obj->value[ 1 ] + 2 * obj->value[ 6 ] ) / 2 );
            send_to_char( buf, ch );

            /**
             * WEAPON_UNBALANCED
             */
            if ( IS_SET( obj->value[ 4 ], WEAPON_UNBALANCED ) )
            {
                send_to_char ( "{m", ch );
                switch( obj->gender )
                {
                    case GENDER_MESKI: // stó³
                        print_char( ch, "%s wydaje siê byæ ciê¿ki i ¼le wywa¿ony.", capitalize( obj->name4 ) );
                        break;
                    case GENDER_ZENSKI: // noga
                        print_char( ch, "%s wydaje siê byæ ciê¿ka i ¼le wywa¿ona.", capitalize( obj->name4 ) );
                        break;
                    case GENDER_NIJAKI: // pismo
                    case GENDER_MESKOOSOBOWY: // narty
                    case GENDER_ZENSKOOSOBOWY: // slepia, lzy
                    case GENDER_NONE: // ??
                    default: // ??
                        print_char( ch, "%s wydaje siê byæ ciê¿kie i ¼le wywa¿one.", capitalize( obj->name4 ) );
                        break;
                }
                send_to_char ( "{x\n\r", ch );
            }
            /**
             * WEAPON_DAGGER && WEAPON_HEARTSEEKER
             */
            if ( obj->value[ 0 ] == WEAPON_DAGGER && IS_SET( obj->value[ 4 ], WEAPON_HEARTSEEKER ) )
            {
                print_char( ch, "{G%s lekko drga, usi³uj±c skierowaæ ostrze w stronê twojego serca.{x\n\r", capitalize( obj->name4 ) );
            }
            /**
             * WEAPON_WICKED
             */
            if ( IS_SET( obj->value[ 4 ], WEAPON_WICKED ) )
            {
                print_char( ch, "{mKa¿dorazowe u¿ycie %s{m powoduje utratê odrobiny ¿ycia.{x\n\r", obj->name2 );
            }

            if ( target != TARGET_SAGE ) // sage nie pokazuje bonusow
            {
                if ( obj->value[ 4 ] )
                {
                    /* Bonus dla specjalisty */
                    if ( !IS_NPC( ch ) )
                    {
                        if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
                        {
                            if ( IS_SET( obj->value[ 4 ], WEAPON_FLAMING ) )
                                print_char( ch, "%s okrywa ledwo widoczna, ognista aura.\n\r", capitalize( obj->name4 ) );

                            if ( IS_SET( obj->value[ 4 ], WEAPON_FROST ) )
                                print_char( ch, "%s okrywa ledwo widoczna, zamra¿aj±ca aura.\n\r", capitalize( obj->name4 ) );

                            if ( IS_SET( obj->value[ 4 ], WEAPON_TOXIC ) )
                                print_char( ch, "%s okrywa ledwo widoczna, ¿r±ca aura.\n\r", capitalize( obj->name4 ) );

                            if ( IS_SET( obj->value[ 4 ], WEAPON_SACRED ) )
                                print_char( ch, "%s okrywa potê¿na, ¶wiêta aura.\n\r", capitalize( obj->name4 ) );

                            if ( IS_SET( obj->value[ 4 ], WEAPON_SHOCKING ) )
                                print_char( ch, "W %s zgromadzono pote¿ny, elektryczny ³adunek.\n\r", obj->name5 );

                            if ( IS_SET( obj->value[ 4 ], WEAPON_VAMPIRIC ) )
                                print_char( ch, "Czujesz, jakby %s wysysa³o z otoczenia si³y witalne.\n\r", obj->name5 );

                            if ( IS_SET( obj->value[ 4 ], WEAPON_RESONANT ) )
                                print_char( ch, "Z %s wydobywaj± siê lekkie wibracje.\n\r", obj->name2 );

                        }
                        else
                        {
                            if
                                (
                                    IS_SET( obj->value[ 4 ], WEAPON_FLAMING  )
                                 || IS_SET( obj->value[ 4 ], WEAPON_FROST    )
                                 || IS_SET( obj->value[ 4 ], WEAPON_SHOCKING )
                                 || IS_SET( obj->value[ 4 ], WEAPON_TOXIC    )
                                 || IS_SET( obj->value[ 4 ], WEAPON_SACRED   )
                                 || IS_SET( obj->value[ 4 ], WEAPON_RESONANT )
                                 || IS_SET( obj->value[ 4 ], WEAPON_VAMPIRIC )
                                )
                                {
                                    hidden = 1;
                                }
                        }
                    }
                    if ( hidden )
                        print_char(
                                ch,
                                "Czujesz, ¿e jest jeszcze co¶ w %s czego nie odkry³%s¶.\n\r",
                                obj->name6,
                                ch->sex == SEX_FEMALE ? "a" : ch->sex == SEX_NEUTRAL ? "o" : "e"
                                );

                    if ( IS_SET( obj->value[ 4 ], WEAPON_SHARP ) )
                        print_char( ch, "Ostrze %s jest niezwykle ostre.\n\r", obj->name2 );

                    if ( IS_SET( obj->value[ 4 ], WEAPON_KEEN ) )
                        print_char( ch, "Trzymaj±c w d³oniach %s zauwa¿asz, ¿e broñ ta jest doskonale wywa¿ona.\n\r", obj->name2 );

                    if ( IS_SET( obj->value[ 4 ], WEAPON_THUNDERING ) )
                        print_char( ch, "%s wibruje lekko od zawartej w ¶rodku mocy.\n\r", obj->name2 );

                    if ( IS_SET( obj->value[ 4 ], WEAPON_VORPAL ) )
                        print_char(
                                ch,
                                "Ostrze %s jest niesamowicie ostre, z ³atwo¶ci± przeci%sby¶ t± broni± granitow± ska³ê.\n\r",
                                obj->name2,
                                ch->sex == SEX_FEMALE ? "e³a" : ch->sex == SEX_NEUTRAL ? "e³o" : "±³"
                                );
                    // WEAPON_INJURIOUS
                    if ( IS_SET( obj->value[ 4 ], WEAPON_INJURIOUS ) )
                    {
                        print_char( ch, "%s ma ostry i zakrzywiony czubek, idealny do zadawania bolesnych, krwawi±cych ran.\n\r", capitalize( obj->name4 ) );
                    }
                    // WEAPON_POISON
                    if ( IS_SET( obj->value[ 4 ], WEAPON_POISON ) )
                    {
                        print_char( ch, "Na %s znajdujesz kilka ciemnych plam po jakiej¶ ¶mierdzacej cieczy, ta broñ jest zatruta.\n\r", obj->name6 );
                    }
                }
            }
            break;

        case ITEM_SHIELD:
            sprintf( buf, "Typ tarczy: %s\n\rMaksymalny bonus do AC: %d\n\rMaksymalne przyjête obra¿enia: %d\n\rSzansa na zablokowanie drugiego napastnika: %d\n\r",
                    ShieldList[ obj->value[ 0 ] ].name, obj->value[ 1 ], obj->value[ 2 ], obj->value[ 3 ] );
            send_to_char( buf, ch );
            print_char( ch, "Przy uderzaniu przeciwnika %s ma %s do trafienia wynosz±c± %d, oraz %s do obra¿eñ wynosz±c± %d.\n\r",
                    obj->short_descr,
                    obj->value[4] >= 0 ? "premiê" : "karê",
                    obj->value[4],
                    obj->value[5] >= 0 ? "premiê" : "karê",
                    obj->value[5] );
            break;

        case ITEM_ARMOR:
            sprintf( buf,
                    "Klasa pancerza: %d k³uj±ce, %d obuchowe, %d ciêcie\n\r",
                    obj->value[ 0 ], obj->value[ 1 ], obj->value[ 2 ] );
            send_to_char( buf, ch );
            break;

        case ITEM_MUSICAL_INSTRUMENT:
            send_to_char( "Typ instrumentu muzycznego: ", ch );
            switch ( obj->value[ 0 ] )
            {
                case( INSTRUMENT_OCARINA ) : send_to_char( "'okaryna'.\n\r", ch ); break;
                case( INSTRUMENT_FLUTE ) : send_to_char( "'flet'.\n\r", ch ); break;
                case( INSTRUMENT_LUTE ) : send_to_char( "'lutnia'.\n\r", ch ); break;
                case( INSTRUMENT_HARP ) : send_to_char( "'harfa'.\n\r", ch ); break;
                case( INSTRUMENT_MANDOLIN ) : send_to_char( "'mandolina'.\n\r", ch ); break;
                case( INSTRUMENT_PIANO ) : send_to_char( "'fortepian'.\n\r", ch ); break;
                default : send_to_char( "nieznany.\n\r", ch ); break;
            }
            sprintf( buf, "Premia do umiejêtno¶ci: %d.\n\r", obj->value[ 1 ] );
            send_to_char( buf, ch );
            if ( obj->value[ 4 ] == 100 ) send_to_char( "Instrument jest doskonale nastrojony.\n\r", ch );
            else if ( obj->value[ 4 ] > 80 ) send_to_char( "Instrument jest dobrze nastrojony.\n\r", ch );
            else if ( obj->value[ 4 ] > 60 ) send_to_char( "Instrument jest do¶æ dobrze nastrojony.\n\r", ch );
            else if ( obj->value[ 4 ] > 40 ) send_to_char( "Instrument jest kiepsko nastrojony.\n\r", ch );
            else if ( obj->value[ 4 ] > 20 ) send_to_char( "Instrument jest do¶æ mocno rozstrojony.\n\r", ch );
            else send_to_char( "Instrument prawie nie nadaje siê do u¿ytku.\n\r", ch );
            break;

        case ITEM_TURN:
            if ( target != TARGET_SAGE ) // sage nie pokazuje magii
            {
                sprintf( buf, "Premia do umiejêtno¶ci: %d.\n\r", obj->value[ 0 ] );
                send_to_char( buf, ch );
                sprintf( buf, "Premia do obra¿eñ: %d.\n\r", obj->value[ 1 ] );
                send_to_char( buf, ch );
            }
            break;

    }

    //rellik: komponenty, ukazywanie komponentowo¶ci obiektu
    int precision, value;
    if ( mage_specialist_divination && obj->is_spell_item && ch->level > LEVEL_NEWBIE ) //jest mistrzem poznania i obiekt jest komponentem
    {
        send_to_char( "Obiekt ten jest niezbêdnym sk³adnikiem rzucania niektórych czarów.\n\r", ch );
        if ( ch->level > 14 )
        {
            precision = 40 - ch->level;
            precision = precision * 2; //minimum 18 procent b³êdu a maximum 52
            value = ( obj->spell_item_counter * precision ) / 100;
            value = number_range( obj->spell_item_counter - value, obj->spell_item_counter + value );
            if ( value < 0 )
            {
                print_char( ch, "Obiektu mo¿na u¿yæ jako komponent do czarów wiele razy.\n\r" );
            } else {
                print_char( ch, "Obiektu mo¿na u¿yæ jako komponent do czarów %d raz%s.\n\r", value, value != 1 ? "y" : "" );
            }
            value = ( obj->spell_item_timer * precision ) / 100;
            value = number_range( obj->spell_item_timer - value, obj->spell_item_timer + value );
            if ( value < 0 ) value = 0;
            print_char( ch, "Obiekt pozostanie jeszcze komponentem przez %d %s.\n\r", value, hour_to_str( value ) );
        }
    }

    if ( target != TARGET_SAGE ) // sage nie pokazuje magii oraz afektow
    {
        if ( !obj->enchanted )
        {
            for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
            {
                if ( paf->location != APPLY_NONE && paf->where != TO_WEAPON )
                {
                    if ( paf->location == APPLY_SKILL )
                        sprintf( buf, "Wp³ywa na umiejêtno¶æ '%s' o %d.\n\r",
                                skill_table[ paf->modifier ].name, paf->level );

                    else if ( paf->location == APPLY_LANG )
                        sprintf( buf, "Wp³ywa na znajomo¶æ jêzyka '%s' o %d.\n\r",
                                lang_table[ paf->modifier ].name, paf->level );

                    else if ( paf->location == APPLY_MEMMING )
                        sprintf( buf, "Wp³ywa na ilo¶æ mo¿liwych zakleæ do zapamietania z %d krêgu o %d.\n\r",
                                paf->modifier, paf->level );
                    else if ( paf->location == APPLY_RESIST )
                        sprintf( buf, "Wp³ywa na odporno¶æ na '%s' o %d.\n\r",
                                resist_name( paf->modifier ), paf->level );
                    else
                        sprintf( buf, "Wp³ywa na %s o %d.\n\r", affect_loc_name( paf->location ), paf->modifier );

                    send_to_char( buf, ch );

                    if ( paf->bitvector && paf->bitvector != &AFF_NONE )
                    {
                        switch ( paf->where )
                        {
                            case TO_AFFECTS:
                                sprintf( buf, "Dodaje %s.\n", affect_bit_name( NULL, paf->bitvector ) );
                                break;
                            case TO_OBJECT:
                                sprintf( buf, "Daje flagê %s.\n",
                                        ext_bit_name( extra_flags, paf->bitvector ) );
                                break;
                        }
                        send_to_char( buf, ch );
                    }

                    if ( !IS_NPC( ch ) && level > number_range(15,20 ) )
                    {
                        if ( mage_specialist_divination )
                        {
                            if( paf->duration >= 0 )
                            {
                                if ( level > LEVEL_HERO )
                                    print_char( ch, "Afekt ten wyga¶nie za dok³adnie %d %s.\n\r", paf->duration, hour_to_str( paf->duration ) );
                                else
                                {
                                    int tmp=number_range( paf->duration - (((40 - level)*paf->duration)/100), paf->duration + (((40 - level)*paf->duration)/100) );
                                    print_char( ch, "Afekt ten wyga¶nie za oko³o %d %s.\n\r", tmp, hour_to_str( tmp ) );
                                }
                            }
                        }
                    }
                }
            }
        }
        for ( paf = obj->affected; paf != NULL; paf = paf->next )
        {
            if ( paf->location != APPLY_NONE && paf->modifier != 0 && paf->where != TO_WEAPON )
            {
                if ( paf->location == APPLY_SKILL )
                    sprintf( buf, "Wp³ywa na umiejêtno¶æ '%s' o %d.\n\r",
                            skill_table[ paf->modifier ].name, paf->level );

                else if ( paf->location == APPLY_LANG )
                    sprintf( buf, "Wp³ywa na znajomo¶æ jêzyka '%s' o %d.\n\r",
                            lang_table[ paf->modifier ].name, paf->level );

                else if ( paf->location == APPLY_MEMMING )
                    sprintf( buf, "Wp³ywa na ilo¶æ mo¿liwych zakleæ do zapamietania z %d krêgu o %d.\n\r",
                            paf->modifier, paf->level );

                else if ( paf->location == APPLY_RESIST )
                    sprintf( buf, "Wp³ywa na odporno¶æ na '%s' o %d.\n\r",
                            resist_name( paf->modifier ), paf->level );

                else
                    sprintf( buf, "Wp³ywa na %s o %d.\n\r",
                            affect_loc_name( paf->location ), paf->modifier );

                send_to_char( buf, ch );

                if ( paf->bitvector && paf->bitvector != &AFF_NONE )
                {
                    switch ( paf->where )
                    {
                        case TO_AFFECTS:
                            sprintf( buf, "Wp³ywa na %s.\n",
                                    affect_bit_name( NULL, paf->bitvector ) );
                            break;
                        case TO_OBJECT:
                            sprintf( buf, "Dodaje flagê %s.\n",
                                    ext_bit_name( extra_flags, paf->bitvector ) );
                            break;
                        case TO_WEAPON:
                            sprintf( buf, "Dodaje flagê %s.\n",
                                    weapon_bit_name( paf->location) );
                            break;
                    }
                    send_to_char( buf, ch );
                }

                if ( !IS_NPC( ch ) && ch->level > number_range(15,20 ) )
                {
                    if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
                    {
                        if( paf->duration >= 0 )
                        {
                            if ( level > LEVEL_HERO )

                                print_char( ch, "Afekt ten wyga¶nie za dok³adnie %d %s.\n\r", paf->duration, hour_to_str( paf->duration ) );
                            else
                            {
                                int tmp=number_range( paf->duration - (((40 - level)*paf->duration)/100), paf->duration + (((40 - level)*paf->duration)/100) );
                                print_char( ch, "Afekt ten wyga¶nie za oko³o %d %s.\n\r", tmp, hour_to_str( tmp ) );
                            }
                        }
                    }
                }
            }
        }
    }
    return;
}
//endluck

void spell_haste( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int luck, duration, af_level = level / 2;

	if ( is_affected( victim, sn ) || IS_AFFECTED( victim, AFF_HASTE ) || EXT_IS_SET( victim->off_flags, OFF_FAST ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mozesz poruszaæ siê jeszcze szybciej!\n\r", ch );
		else
			act( "$N ju¿ porusza siê tak szybko jak tylko potrafi.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_SLOW ) )
	{
		if ( !check_dispel( level, victim, skill_lookup( "slow" ) ) )
		{
			if ( victim != ch )
				send_to_char( "Zaklêcie nie wysz³o.\n\r", ch );
			send_to_char( "Chwilowo ¶wiat wokó³ ciebie zwalnia.\n\r", victim );
			return;
		}
		act( "$n zaczyna poruszaæ siê nieco szybciej.", victim, NULL, NULL, TO_ROOM );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	/* zaleznosc od szczescia */
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê siebie przy¶pieszyæ.\n\r", ch );
		else
			act( "Nie uda³o ci siê przy¶pieszyæ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 5 + level / 5;

	if ( number_range( 0, luck ) > 15 ) duration++;
	if ( number_range( 0, luck ) < 5 ) duration--;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration += 1 + level / 7.5;
			af_level = level;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = af_level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_DEX;
	af.modifier = UMAX( 1, level/10 );
	af.bitvector = &AFF_HASTE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "¦wiat wokó³ ciebie zwalnia.\n\r", victim );
	act( "$n zaczyna poruszaæ siê niewiarygodnie szybko.", victim, NULL, NULL, TO_ROOM );

	return;
}

void spell_giant_strength( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, duration, mod;

	    	if (is_undead(victim) || IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		act( "Te zaklêcie nie zadzia³a na $C.", ch, NULL, victim, TO_CHAR );
		return;
	}

    if ( check_improve_strenth ( ch, victim, TRUE ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Jeste¶ ju¿ tak siln<&y/a/e> jak tylko potrafisz!\n\r", ch );
		}
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie mo¿e byæ ju¿ silniejsze.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N nie mo¿e byæ ju¿ silniejszy.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo¿e byæ ju¿ silniejsza.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	/* zaleznosc od szczescia */
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ swejej si³y.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ si³y $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 4 +  level/ 10;
	mod = number_range( 2, 4 );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_STR;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;

	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Twoje miê¶nie zaczynaj± dzia³aæ ze znacznie zwiêkszon± sprawno¶ci±!\n\r", victim );
	act( "Miê¶nie $z zaczynaj± dzia³aæ ze znacznie zwiêkszon± sprawno¶ci±.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_fly( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int luck, duration, af_level = level / 2;

	if ( IS_AFFECTED( victim, AFF_FLYING ) || IS_AFFECTED( victim, AFF_FLOAT ) )
	{
		if ( victim == ch )
			send_to_char( "Przecie¿ ju¿ unosisz siê w powietrzu.\n\r", ch );
		else
			act( "$N ju¿ siê unosi w powietrzu.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->mount )
	{
		if ( victim == ch )
			send_to_char( "Jak chcesz to zrobiæ bêd±c na wierzchowcu?\n\r", ch );
		else
			act( "$N siedzi na wierzchowcu, po co ma lataæ?", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->position != POS_FIGHTING && victim->position != POS_STANDING )
	{
		send_to_char( "Zaklêcie siê nie uda³o.\n\r", ch );
		return;
	}

	if ( get_carry_weight( victim ) > can_carry_w( victim ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Niestety po chwili opadasz, poniewa¿ jeste¶ zbyt mocno obci±¿on<&y/a/e>.\n\r", ch );
		}
		else
		{
			switch ( victim -> sex )
			{
				case 0:
					act( "Po chwili $N opada na ziemiê, poniewa¿ jest zbyt mocno obci±¿one.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "Po chwili $N opada na ziemiê, poniewa¿ jest zbyt mocno obci±¿ony.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "Po chwili $N opada na ziemiê, poniewa¿ jest zbyt mocno obci±¿ona.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	/* zaleznosc od szczescia */
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	duration = 3 + level / 3;

	if ( number_range( 0, luck ) > 15 ) duration++;
	if ( number_range( 0, luck ) < 5 ) duration--;

	/* Bonus dla specjalisty oraz ograniczenie dla niespecjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration = 4 + level / 2;
			af_level = level;
		}
		else
		{
			duration = UMIN( 11, duration );
		}
	}

	af.where = TO_AFFECTS;
	af.type = gsn_fly;
	af.level = af_level;
	af.duration = duration; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_FLYING;
	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Twoje stopy odrywaj± siê od ziemi.\n\r", victim );
	act( "Stopy $z odrywaj± siê od ziemi.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_fireball( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA *vch, *vch_next;
	int dam,

	/* luck */

	luck = get_curr_stat_deprecated( ch, STAT_LUC ),
  vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ ognistej kuli.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Nie dasz rady tego zrobiæ.\n\r", ch );
		return;
	}

	print_char( ch, "Twoja ognista kula trafia w %s i eksploduje zapalaj±c wszystko w pobli¿u.\n\r", victim->name4 );
	act( "Ognista kula $z trafia w $C i eksploduje rozsiewaj±c wko³o morze p³omieni.", ch, NULL, victim, TO_NOTVICT );

	for ( vch = ch->in_room->people;vch;vch = vch_next )
	{
		vch_next = vch->next_in_room;
		if ( is_same_group( ch, vch ) )
			continue;

		dam = dice( 5, 5 ) + dice( level / 3, 4 );

		/* Bonus dla specjalisty */
		if ( !IS_NPC( ch ) )
		{
			if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
			{
				dam = dice( 5, 5 ) + dice( level / 2, 5 );
			}
		}

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
      dam += 10;
    }

    	if ( IS_AFFECTED( vch, AFF_MINOR_GLOBE ) || IS_AFFECTED( vch, AFF_GLOBE ) || IS_AFFECTED( vch, AFF_MAJOR_GLOBE ) || IS_AFFECTED( vch, AFF_ABSOLUTE_MAGIC_PROTECTION ) )
    	{
			act( "Twoja fala p³omieni znika przy zetkniêciu ze sfer± otaczaj±c± $C.", ch, NULL, vch, TO_CHAR );
			act( "Fala p³omieni $z znika przy zetkniêciu z otaczaj±c± ciê sfer±.\n\r", ch, NULL, vch, TO_VICT );
			act( "Fala p³omieni $z znika przy zetkniêciu z otaczaj±c± $C sfer±.", ch, NULL, vch, TO_NOTVICT );
			return;
		}

		if ( IS_AFFECTED( vch, AFF_REFLECT_SPELL ))
		{
			print_char( ch, "Twoje zaklêcie odbija siê od ochronnej tarczy %s.\n\r", vch->name2 );
			print_char( vch, "Zaklêcie %s odbija siê od ochronnej tarczy.\n\r", ch->name2 );
			act( "Zaklêcie $z odbija siê od ochronnej tarczy $Z.", ch, NULL, vch, TO_NOTVICT );
			vch = ch;
		}

		if( is_affected( vch, gsn_mirror_image ) )
		{
			affect_strip( vch, gsn_mirror_image );
			send_to_char( "Ogieñ niszczy wszystkie otaczaj±ce ciê lustrzane odbicia.\n\r", vch );
			act( "Ogieñ niszczy wszystkie lustrzane odbicia otaczaj±ce $c.", vch, NULL, NULL, TO_ROOM );
		}

		send_to_char( "Przez chwilê otaczaj± ciê p³omienie, czujesz jak pal± ci siê w³osy.\n\r", vch );
//Brohacz: bylo typo:		act( "Fala p³omieni ogarnia $c", vch, NULL, NULL, TO_NOTVICT );
		act( "Fala p³omieni ogarnia $c.", vch, NULL, NULL, TO_NOTVICT );
		spell_damage( ch, vch, dam, sn, DAM_FIRE , FALSE );
		fire_effect( vch, level, dam, TARGET_CHAR );

	}
	return;
}

/*
 * Drain XP, MANA, HP.
 * Caster gains HP.
 */
//taki kombajn do wysysania expa, mozna walic z proga
//lepiej jak gracze nie beda mogli tego uzywac...
void spell_energy_drain( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	AFFECT_DATA *paf, *energy = NULL, *paf_next;
	int exp_percent_drain, exp;

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) ||
	     ( victim->resists[ RESIST_NEGATIVE ] > 0 && number_percent() < victim->resists[ RESIST_NEGATIVE ] ) )
	{
		send_to_char( "Czujesz dziwne dreszcze, które momentalnie ustêpuj±.\n\r", victim );
		return;
	}

	if ( victim->level < 2 || ( IS_IMMORTAL( victim ) && !IS_NPC( victim ) ) )
		return;

	//gracz na 31 levelu ma 50% szansy ¿e siê na niego nie uda rzuciæ
	if ( victim->level == LEVEL_HERO && number_percent() < 50 )
	{
		send_to_char( "Czujesz dziwne dreszcze, które momentalnie ustêpuj±.\n\r", victim );
		return;
	}

	exp_percent_drain = 10 + number_range( URANGE( 1, level / 2, 15 ), URANGE( 1, level, 30 ) );

	//go¶æ na 31 levelu ma exp obliczany z expa potrzebnego z levelu 30 na 31
	if ( victim->level == LEVEL_HERO )
	{
		exp = exp_per_level( victim, LEVEL_HERO - 1 ) - 1 - exp_per_level( victim, LEVEL_HERO - 2 );
		exp = exp_percent_drain * exp / 100;
	}
	else
	{
		exp = exp_per_level( victim, victim->level ) - 1 - exp_per_level( victim, victim->level - 1 );
		exp = exp_percent_drain * exp / 100;
	}

	send_to_char( "{RCzujesz jak up³ywa z ciebie ¿ycie!{x\n\r", victim );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 50;
	af.duration = 600; af.rt_duration = 0;
	af.location = APPLY_EXP;
	af.modifier = ( -exp );
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, "wyssanie do¶wiadczenia", TRUE );

	/* lekka kicha, jesli jest affect_to_char to wszystko jest ok,
	   tyle, ze postac bedzie miala po kazdym energy drain nowy
	   element w affected. jesli wykorzystac by affect_join
	   to z kazdym nowym energy drain kasowano by wyssane expy
	   i wysysano od nowa...spory spam o straceniu/zdobyciu poziomu
	   dlatego zrobie scieme, po nalozeniu affectow zsumuje je i
	   wywale ostatni*/

	/* szukamy pierwszego energy drain na liscie affectow */
	for ( paf = victim->affected; paf; paf = paf->next )
		if ( paf->type == sn )
		{
			energy = paf;
			paf = paf->next;
			break;
		}

	if ( !energy )
		return;

	/* szukamy kolejnych i sumujemy do starego*/
	for ( ; paf; paf = paf_next )
	{
		paf_next = paf->next;

		if ( paf->type == sn )
		{
			energy->modifier += paf->modifier;
			paf->modifier = 0;
			paf->location = APPLY_NONE;
			affect_remove( victim, paf );
		}
	}
	return;
}

//endluck
void spell_enchant_weapon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	int hit_bonus, dam_bonus, mod1, mod2;
	int maks_bk, maks_k1, maks_k2, intel, lucky;
	bool spec, ma_komponent2, ma_komponent1;

	/* moby nie enchantuj±, bo po co */
	if ( IS_NPC( ch ) )
		return;

	if ( obj->item_type != ITEM_WEAPON )
	{
		send_to_char( "To nie jest broñ.\n\r", ch );
		return;
	}

	if ( obj->wear_loc != -1 )
	{
		send_to_char( "Musisz to mieæ przy sobie.\n\r", ch );
		return;
	}

	if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) || number_percent() < 25 || IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
	{
		send_to_char( "Nic siê nie sta³o.\n\r", ch );
		return;
	}

	spec = FALSE;
	ma_komponent1 = FALSE;
	ma_komponent2 = FALSE;
	//sprawdzanie czy specjalista


	if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
    {
         spec = TRUE;
    }else
    {
         spec = FALSE;

    }

	//ustawianie maksymalnych umagicznien (maj±cych w miare normalne szanse)
	if (spec == TRUE)
    {
         maks_bk = 2;
         maks_k1 = 3;
         maks_k2 = 4;
    }
	else
    {
         maks_bk = 1;
         maks_k1 = 2;
         maks_k2 = 3;
    }
    //

	mod1 = number_range(0, ch->level);
    mod2 = number_range(0, ch->level);
    if((mod1 < 5) && (mod2 <5))return;

    //sprawdzanie komponentów

    //komponent 2
   	if ( spell_item_check( ch, sn, "enchant mocny" ))
    {
         ma_komponent2 = TRUE;
         if(spec == TRUE)
         {
              if(mod1 >= 25) {dam_bonus = 3;}else
              {
              if(mod1 >= 10) {dam_bonus = 2;}else
              { dam_bonus = 1;}
              }

               if(mod2 >= 23) {hit_bonus = 3;}else
              {
              if(mod2 >= 10) {hit_bonus = 2;}else
              { hit_bonus = 1;}
              }

         }
		 else
         {
              if(mod1 >= 31) {dam_bonus = 3;}else
              {
              if(mod1 >= 15) {dam_bonus = 2;}else
              { dam_bonus = 1;}
              }

               if(mod2 >= 27) {hit_bonus = 3;}else
              {
              if(mod2 >= 15) {hit_bonus = 2;}else
              { hit_bonus = 1;}
              }

         }


    }// koniec komponentu 2
    //komponent 1
	else if ( spell_item_check( ch, sn, "enchant sredni" ))
    {
         ma_komponent1 =TRUE;
         if(spec == TRUE)
         {
                 if(mod1 >= 30) {dam_bonus = 3;}else
              {
              if(mod1 >= 16) {dam_bonus = 2;}else
              { dam_bonus = 1;}
              }

               if(mod2 >= 28) {hit_bonus = 3;}else
              {
              if(mod2 >= 14) {hit_bonus = 2;}else
              { hit_bonus = 1;}
              }

         }else
         {

              if(mod1 >= 19) {dam_bonus = 2;}else
              { dam_bonus = 1;}

              if(mod2 >= 19) {hit_bonus = 2;}else
              { hit_bonus = 1;}

         }

    }// koniec komponentu 1
    else//brak komponentu
    {
         if(spec == TRUE)
         {
              if(mod1 >= 20) {dam_bonus = 2;}else
              { dam_bonus = 1;}

              if(mod2 >= 17) {hit_bonus = 2;}else
              { hit_bonus = 1;}
         }
		 else
         {
              dam_bonus = 1;
              hit_bonus = 1;
         }
    }//koniec brak komponentu
    //koniec sprawdzania komponentow

    //bonus od levela
    if ((ch->level) >= 31)
    {
       if(spec == TRUE)
       {
               if(number_range(0,4) == 4)
               {
                  dam_bonus++;
                  hit_bonus++;
               }
       }
	   else
       {
               if(number_range(0,5) == 5)
               {
                  dam_bonus++;
                  hit_bonus++;
               }
       }
    }
    //bonus od lucka i inta
    lucky = get_curr_stat_deprecated( ch, STAT_LUC );
    intel = get_curr_stat_deprecated( ch, STAT_INT );
    if((lucky + intel - 40) > 0)
    {
    if((lucky + intel) > 3)
    {
        if(number_range(0, (lucky + intel - 40)) >= 3)
        {
            dam_bonus++;
            hit_bonus++;
        }
    }
    }

    if(ma_komponent2 == TRUE)
	{
		dam_bonus=URANGE(0, dam_bonus, maks_k2);
		hit_bonus=URANGE(0, hit_bonus, maks_k2);
	}
	else
    {
            if(ma_komponent1 == TRUE)
			{
				dam_bonus=URANGE(0, dam_bonus, maks_k1);
				hit_bonus=URANGE(0, hit_bonus, maks_k1);
			}
			else
            {
				dam_bonus=URANGE(0, dam_bonus, maks_bk);
                hit_bonus=URANGE(0, hit_bonus, maks_bk);
            }
    }


    if ( hit_bonus == 0 && dam_bonus == 0 )
		{
			send_to_char( "Tracisz koncentracjê i czujesz, jak zebrana w tobie moc rozp³ywa siê bezpowrotnie.\n\r", ch );
			act( "Wokó³ $x rozb³yska sina otoczka mocy.", ch, NULL, NULL, TO_ROOM );
			return;
		}


    /* item destroyed */
	if ( number_range( 0, 50) == 0 )
	{
		AFFECT_DATA af;
		switch ( number_range( 0, 25 ) )
		{
			case 0:  //blidness
                if ( !IS_AFFECTED(ch,AFF_PERFECT_SENSES ) )
                {
                    if( obj->liczba_mnoga )
                    {
                        act( "$p wybuchaj± ci prosto w twoje oczy!", ch, obj, NULL, TO_CHAR );
                        act( "$p wybuchaj± $x prosto w $s oczy!", ch, obj, NULL, TO_ROOM );
                    }
                    else
                    {
                        act( "$p wybucha ci prosto w twoje oczy!", ch, obj, NULL, TO_CHAR );
                        act( "$p wybucha $x prosto w $s oczy!", ch, obj, NULL, TO_ROOM );
                    }
					af.where = TO_AFFECTS;
					af.type = 5;
					af.level = level;
					af.location = APPLY_NONE;
					af.modifier = 0;
					af.duration = UMAX( 1, hit_bonus + dam_bonus ); af.rt_duration = 0;
					af.bitvector = &AFF_BLIND;
					affect_to_char( ch, &af, NULL, TRUE );
					break;
				}

            case 1:  //weaken
                if( obj->liczba_mnoga )
                {
                    act( "$p wybuchaj± rani±c ciê i rozrywaj±c twoje cia³o!", ch, obj, NULL, TO_CHAR );
                    act( "$p wybuchaj± rani±c $x i rozrywaj±c $c cia³o!", ch, obj, NULL, TO_ROOM );
                }
                else
                {
                    act( "$p wybucha rani±c ciê i rozrywaj±c twoje cia³o!", ch, obj, NULL, TO_CHAR );
                    act( "$p wybucha rani±c $x i rozrywaj±c $c cia³o!", ch, obj, NULL, TO_ROOM );
                }
                af.where = TO_AFFECTS;
				af.type = 96;
				af.level = level;
				af.duration = UMAX( 1, hit_bonus + dam_bonus ); af.rt_duration = 0;
				af.location = APPLY_STR;
				af.modifier = - UMAX( 1, hit_bonus + dam_bonus );
				af.bitvector = &AFF_WEAKEN;
				affect_to_char( ch, &af, NULL, TRUE );

				if ( !ch->mount && get_carry_weight( ch ) > can_carry_w( ch ) && IS_AFFECTED( ch, AFF_FLYING ) )
				{
					act( "$n pod wp³ywem ciê¿aru opada na ziemiê.", ch, NULL, NULL, TO_ROOM );
					send_to_char( "Pod wp³ywem obci±¿enia opadasz na ziemiê.\n\r", ch );
					affect_strip( ch, gsn_float );
				}
				break;

			case 3: //deafness
				if ( !IS_AFFECTED(ch,AFF_PERFECT_SENSES ) )
				{
                    if( obj->liczba_mnoga )
                    {
                        act( "$p wybuchaj±, po czym wszystko dooko³a milknie!", ch, obj, NULL, TO_CHAR );
                        act( "$p wybuchaj± prosto w twarz $x!", ch, obj, NULL, TO_ROOM );
                    }
                    else
                    {
                        act( "$p wybucha, po czym wszystko dooko³a milknie!", ch, obj, NULL, TO_CHAR );
                        act( "$p wybucha prosto w twarz $x!", ch, obj, NULL, TO_ROOM );
                    }
					af.where = TO_AFFECTS;
					af.type = 169;
					af.level = level;
					af.duration = 4; af.rt_duration = 0;
					af.location = APPLY_NONE;
					af.modifier = 0;
					af.bitvector = &AFF_DEAFNESS;
					affect_to_char( ch, &af, NULL, TRUE );
					break;
				}
			default :
				if ( material_table[ obj->material ].flag == MAT_METAL )
                {
                    if( obj->liczba_mnoga )
                    {
                        act( "$p wybuchaj±, rani±c ciê od³amkami!", ch, obj, NULL, TO_CHAR );
                        act( "$p wybuchaj±, rani±c $z od³amkami!", ch, obj, NULL, TO_ROOM );
                    }
                    else
                    {
                        act( "$p wybucha, rani±c ciê od³amkami!", ch, obj, NULL, TO_CHAR );
                        act( "$p wybucha, rani±c $z od³amkami!", ch, obj, NULL, TO_ROOM );
                    }
                }
				else
				{
                    if( obj->liczba_mnoga )
                    {
                        act( "$p wybuchaj±, przysma¿aj±c ciê gor±cymi jêzykami!", ch, obj, NULL, TO_CHAR );
                        act( "$p wybuchaj±, przysma¿aj±c $z gor±cymi jêzykami!", ch, obj, NULL, TO_ROOM );
                    }
                    else
                    {
                        act( "$p wybucha, przysma¿aj±c ciê gor±cymi jêzykami!", ch, obj, NULL, TO_CHAR );
                        act( "$p wybucha, przysma¿aj±c $z gor±cymi jêzykami!", ch, obj, NULL, TO_ROOM );
                    }
				}
				break;
		}
		damage( ch, ch, 20 + dice( 10, hit_bonus + dam_bonus ), TYPE_UNDEFINED, DAM_BASH, FALSE );
		fire_effect( ch, level, 20 + dice( 10, hit_bonus + dam_bonus ), TARGET_CHAR );
		/*artefact*/
		if ( is_artefact( obj ) ) extract_artefact( obj );
		else extract_obj( obj );
		return;
	}


    if ( IS_GOOD( ch ) )
	{
		EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_NEUTRAL );
		EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
	}
	else if ( IS_EVIL( ch ) )
	{
		EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
		EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_NEUTRAL );
	}
	else
	{
		EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
		EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
	}

    // ogranicznik, ¿eby nie robiæ sztyletów z kosmicznymi bonusami
	obj->value[ 5 ] = UMAX(hit_bonus, obj->value[ 5 ]);
	obj->value[ 6 ] = UMAX(dam_bonus, obj->value[ 6 ]);

	if( number_range(0, level) > 20 )
    {
        if ( spec )
        {
            obj->timer = level * 30;
        }
        else
        {
            obj->timer = level * 15;
        }
    }
	else
    {
        if ( spec )
        {
            obj->timer = level * 24;
        }
        else
        {
            obj->timer = level * 12;
        }
    }

    EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );

    print_char( ch, "%s rozb³yskuj%s przez chwile jasnym ¶wiat³em.\n\r", capitalize( obj->short_descr), (obj->liczba_mnoga)? "±":"e" );
    return;

}//koniec spella enchant weapon

void spell_enchant_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	AFFECT_DATA *paf;
	int result, fail, ac_bonus, added, mod = 1;
	bool ac_found = FALSE, destroy = FALSE;
	bool enchanter = FALSE; // czy jest specem zauroczen

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			enchanter = TRUE;
		}
	}

	if ( obj->item_type != ITEM_ARMOR )
	{
		send_to_char( "To nie jest pancerz.\n\r", ch );
		return;
	}

	if ( obj->wear_loc != -1 )
	{
		send_to_char( "Musisz to mieæ przy sobie.\n\r", ch );
		return;
	}

	/* this means they have no bonus */
	ac_bonus = 0;

	/* base 25% chance of failure */
	fail = 25;

	/* find the bonuses */
	if ( !obj->enchanted )
		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
		{
			if ( paf->location == APPLY_AC )
			{
				ac_bonus = paf->modifier;
				ac_found = TRUE;
				fail += 5 * ( ac_bonus * ac_bonus );
			}
			else  /* things get a little harder */
				fail += 20;
		}

	for ( paf = obj->affected; paf != NULL; paf = paf->next )
	{
		if ( paf->location == APPLY_AC )
		{
			ac_bonus = paf->modifier;
			ac_found = TRUE;
			fail += 5 * ( ac_bonus * ac_bonus );
		}
		else /* things get a little harder */
			fail += 20;
	}

	for ( paf = obj->affected; paf != NULL; paf = paf->next )
	{
		if ( paf->location == APPLY_AC && paf->type == sn )
		{
			if( enchanter )
			{
				if( paf->modifier <= -11 )
					destroy = TRUE;
			}
			else
			{
				if( paf->modifier <= -9 )
					destroy = TRUE;
			}
		}
	}

	/* apply other modifiers */
	fail -= level;

	if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
		fail -= 15;
	if ( IS_OBJ_STAT( obj, ITEM_GLOW ) )
		fail -= 5;

	/* Bonus dla specjalisty */
	if ( enchanter )
	{
		fail = URANGE( 5, fail, 85 );
		mod = 2;
	}

	fail /= mod;
	fail = URANGE( 5, fail, 85 );

	result = number_percent();

	/* the moment of truth */
	/* item destroyed */
	if ( result < ( fail / 5 ) || destroy )
	{
		AFFECT_DATA af;
		int duration = 2 + number_range( 3 - mod, 2 );
		int dam = number_range( 4, 2 ) - mod;
		switch ( number_range( 0, 25 * mod ) )
		{
            case 0:  //blidness
                if ( !IS_AFFECTED(ch,AFF_PERFECT_SENSES ) )
                {
                    if( obj->liczba_mnoga )
                    {
                        act( "$p wybuchaj± ci prosto w twoje oczy!", ch, obj, NULL, TO_CHAR );
                        act( "$p wybuchaj± $x prosto w $s oczy!", ch, obj, NULL, TO_ROOM );
                    }
                    else
                    {
                        act( "$p wybucha ci prosto w twoje oczy!", ch, obj, NULL, TO_CHAR );
                        act( "$p wybucha $x prosto w $s oczy!", ch, obj, NULL, TO_ROOM );
                    }
                    af.where = TO_AFFECTS;
                    af.type = 5;
					af.level = level;
					af.location = APPLY_NONE;
					af.modifier = 0;
					af.duration = duration; af.rt_duration = 0;
					af.bitvector = &AFF_BLIND;
					affect_to_char( ch, &af, NULL, TRUE );
					break;
				}

            case 1:  //weaken
                if( obj->liczba_mnoga )
                {
                    act( "$p wybuchaj± rani±c ciê i rozrywaj±c twoje cia³o!", ch, obj, NULL, TO_CHAR );
                    act( "$p wybuchaj± rani±c $x i rozrywaj±c $c cia³o!", ch, obj, NULL, TO_ROOM );
                }
                else
                {
                    act( "$p wybucha rani±c ciê i rozrywaj±c twoje cia³o!", ch, obj, NULL, TO_CHAR );
                    act( "$p wybucha rani±c $x i rozrywaj±c $c cia³o!", ch, obj, NULL, TO_ROOM );
                }
                af.where = TO_AFFECTS;
				af.type = 96;
				af.level = level;
				af.duration = duration; af.rt_duration = 0;
				af.location = APPLY_STR;
				af.modifier = - dam;
				af.bitvector = &AFF_WEAKEN;
				affect_to_char( ch, &af, NULL, TRUE );

				if ( !ch->mount && get_carry_weight( ch ) > can_carry_w( ch ) && IS_AFFECTED( ch, AFF_FLYING ) )
				{
					act( "$n pod wp³ywem ciê¿aru opada na ziemiê.", ch, NULL, NULL, TO_ROOM );
					send_to_char( "Pod wp³ywem obci±¿enia opadasz na ziemiê.\n\r", ch );
					affect_strip( ch, gsn_float );
				}
				break;

			case 4:  //heat_metal
                if( obj->liczba_mnoga )
                {
                    act( "$p wybuchaj±, zasypuj±c ciê rozgrzanymi od³amkami!", ch, obj, NULL, TO_CHAR );
                    act( "$p wybuchaj±, zasypuj±c $z rozgrzanymi od³amkami!", ch, obj, NULL, TO_ROOM );
                }
                else
                {
                    act( "$p wybucha, zasypuj±c ciê rozgrzanymi od³amkami!", ch, obj, NULL, TO_CHAR );
                    act( "$p wybucha, zasypuj±c $z rozgrzanymi od³amkami!", ch, obj, NULL, TO_ROOM );
                }
				( *skill_table[ gsn_heat_metal ].spell_fun ) ( gsn_heat_metal, level, ch, ch, TARGET_CHAR );
				break;

			default :
				act( "$p wybucha, rani±c ciê od³amkami!", ch, obj, NULL, TO_CHAR );
				act( "$p wybucha, rani±c $z od³amkami!", ch, obj, NULL, TO_ROOM );
				break;
		}
		damage( ch, ch, 20 + dice( 10, dam ), TYPE_UNDEFINED, DAM_BASH, FALSE );
		fire_effect( ch, level, 20 + dice( 10, dam ), TARGET_CHAR );
		/*artefact*/
		if ( is_artefact( obj ) ) extract_artefact( obj );
		else extract_obj( obj );
		return;
	}

	/* item disenchanted */
	if ( result < ( fail / 3 ) )
	{
        AFFECT_DATA * paf_next;
        if( obj->liczba_mnoga )
        {
            act( "$p zaczynaj± promieniowaæ, ale na krótko... oops.", ch, obj, NULL, TO_CHAR );
            act( "$p zaczynaj± promieniowaæ, ale na krótko.", ch, obj, NULL, TO_ROOM );
        }
        else
        {
            act( "$p zaczyna promieniowaæ, ale na krótko... oops.", ch, obj, NULL, TO_CHAR );
            act( "$p zaczyna promieniowaæ, ale na krótko.", ch, obj, NULL, TO_ROOM );
        }
        obj->enchanted = TRUE;

		/* remove all affects */
		for ( paf = obj->affected; paf != NULL; paf = paf_next )
		{
			paf_next = paf->next;
			free_affect( paf );
		}
		obj->affected = NULL;

		/* clear all flags */
		ext_flags_clear( obj->extra_flags );
		return;
	}

	/* failed, no bad result */
	if ( result <= fail )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	/* okay, move all the old flags into new vectors if we have to */
	if ( !obj->enchanted )
	{
		AFFECT_DATA * af_new;
		obj->enchanted = TRUE;

		for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
		{
			af_new = new_affect();

			af_new->next = obj->affected;
			obj->affected = af_new;

			af_new->where = paf->where;
			af_new->type = UMAX( 0, paf->type );
			af_new->level = paf->level;
			af_new->duration = paf->duration;
			af_new->rt_duration = 0;
			af_new->location = paf->location;
			af_new->modifier = paf->modifier;
			af_new->bitvector = paf->bitvector;
		}
	}

	if ( result <= ( enchanter ? (80 - level ) : ( 90 - (level / 5 ))) )   /* success! */
	{
		if( obj->liczba_mnoga )
		{
			act( "$p zapalaj± siê z³otym ¶wiat³em.", ch, obj, NULL, TO_ALL );
		}
		else
		{
			act( "$p zapala siê z³otym ¶wiat³em.", ch, obj, NULL, TO_ALL );
		}
		EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );
		added = -1;
	}
	else  /* exceptional enchant */
	{
		if( obj->liczba_mnoga )
		{
			act( "$p rozb³yskuj± wspania³ym z³otym ¶wiat³em.", ch, obj, NULL, TO_ALL );
		}
		else
		{
			act( "$p rozb³yskuje wspania³ym z³otym ¶wiat³em.", ch, obj, NULL, TO_ALL );
		}
		EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );
		EXT_SET_BIT( obj->extra_flags, ITEM_GLOW );
		added = -2;
	}

	if ( ac_found )
	{
		for ( paf = obj->affected; paf != NULL; paf = paf->next )
		{
			if ( paf->location == APPLY_AC )
			{
				paf->type = sn;
				paf->modifier += added;
				paf->level = UMAX( paf->level, level );
			}
		}
	}
	else /* add a new affect */
	{
		paf = new_affect();
		paf->where = TO_OBJECT;
		paf->type = sn;
		paf->level = level;
		paf->duration = -1;
		paf->rt_duration = 0;
		paf->location = APPLY_AC;
		paf->modifier = added;
		paf->bitvector = &AFF_NONE;
		paf->next = obj->affected;
		obj->affected = paf;
	}

	if ( IS_GOOD( ch ) )
	{
		EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_NEUTRAL );
		EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
	}
	else if ( IS_EVIL( ch ) )
	{
		EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
		EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_NEUTRAL );
	}
	else
	{
		EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
		EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
	}

	obj->timer = (enchanter && number_range(0,2) == 0 ? level * 72 * mod : level * 48 * mod);
}

void spell_colour_spray( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dam, dur,

	/* luck */

	luck = get_curr_stat_deprecated( ch, STAT_LUC ),
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ chmury migocz±cych kolorów.\n\r", ch );
		return;
	}

	dam = dice( 3, 5 ) + level / 3;
	dur = dice( 1, 4 );
	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dam = dice( 3, 6 ) + level / 2;
			dur = dice( 2, 3 );
		}
	}

	dam = luck_dam_mod( ch, dam );

	act( "{5Rozpylasz w kierunku $Z drobne cz±steczki wszystkich ¿ywio³ów!{x", ch, NULL, victim, TO_CHAR );
	act( "{5$n rozpyla w twoim kierunku drobne cz±steczki wszystkich ¿ywio³ów!{x", ch, NULL, victim, TO_VICT );
	act( "{5$n rozpyla drobne cz±steczki wszystkich ¿ywio³ów w kierunku $Z!{x", ch, NULL, victim, TO_NOTVICT );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam /= 2;
	}
	else if ( number_range(1, 4) == 1 && !IS_AFFECTED( victim,AFF_PERFECT_SENSES ) )
	{
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = dur;
		af.rt_duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &AFF_BLIND;
		affect_to_char( victim, &af, NULL, TRUE );

		send_to_char( "Zosta³e¶ o¶lepiony!\n\r", victim );
		act( "$n wygl±da na o¶lepion$t.", victim, victim->sex == SEX_FEMALE ? "±" : "ego", NULL, TO_ROOM );
	}

	spell_damage( ch, victim, dam, sn, DAM_LIGHT, FALSE );
	return;
}

void spell_chill_touch( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dam, dur, mod,

	/* luck */

	luck = get_curr_stat_deprecated( ch, STAT_LUC ),
  vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê wyziêbiæ swego cia³a.\n\r", ch );
		else
			act( "Nie uda³o ci siê wyziêbiæ $Z swym zamra¿aj±cym dotykiem.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dam = dice( 3, 5 ) + UMAX( 1, level - 3 ) / 2;
	dur = 2;
	mod = -2;
	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dam = dice( 5, 5 ) + level / 2;
			dur = 3;
			mod = -3;
		}
	}

	if ( IS_NPC( ch ) )
	{
		dam = dice( 4, 5 ) + level / 2;
	}

	dam = luck_dam_mod( ch, dam );

	if ( !IS_AFFECTED( victim, AFF_WEAKEN ) )
	{
		act( "Doko³a $z pojawia siê niebieska sfera mrozu.", victim, NULL, NULL, TO_ROOM );
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = dur;
	af.rt_duration = 0;
		af.location = APPLY_STR;
		af.modifier = mod;
		af.bitvector = &AFF_WEAKEN;
		affect_to_char( victim, &af, NULL, TRUE );
	}

	act( "$n trzêsie siê z zimna pod wp³ywem mro¼nego dotkniêcia.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Zamra¿aj±ce dotkniêcie wywo³uje u ciebie dreszcze.\n\r", victim );

	spell_damage( ch, victim, dam, sn, DAM_COLD, FALSE );
	return;
}

void spell_charm_person( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int chance = 1, spec_bonus = 0, dur = 10 - UMAX( -4, get_curr_stat_deprecated( victim, STAT_INT ) - 16 );

	if ( !IS_NPC( ch ) && IS_IMMORTAL( victim ) )
		return;

	if ( is_safe( ch, victim ) )
		return;

	if ( victim == ch )
	{
		send_to_char( "Podobasz siê sobie jeszcze bardziej!\n\r", ch );
		return;
	}

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			spec_bonus = 4;
		}
	}

	if ( ( !IS_NPC( victim ) && !IS_NPC( ch ) )
	     || IS_AFFECTED( victim, AFF_CHARM )
	     || !IS_SET( race_table[ GET_RACE( victim ) ].type , PERSON )
	     || IS_AFFECTED( ch, AFF_CHARM )
	     || level + spec_bonus < victim->level
	     || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		return;

	if ( IS_AFFECTED( victim, AFF_CHARM ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "Rozpoznajesz, ¿e $N jest ju¿ zauroczone.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "Rozpoznajesz, ¿e $N jest ju¿ zauroczony.", ch, NULL, victim, TO_CHAR );
				break;
			default :
				act( "Rozpoznajesz, ¿e $N jest ju¿ zauroczona.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

    if ( IS_AFFECTED( victim, AFF_TROLL ) )
    {
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
    }

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

	if ( spec_bonus > 0 )
	{
		chance += chance/2;
		chance = URANGE(20,chance,100);
		dur *= 2;
	}

	if ( number_percent() > chance )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( !can_see( victim, ch ) || victim->position == POS_SLEEPING )
	{
		if ( victim->position == POS_SLEEPING )
			act( "$N rzuca siê przez sen, to pewnie jaki¶ koszmar.", ch, NULL, victim, TO_ALL );
		else
			act( "$N rozgl±da siê dooko³a, ale nie widzi swojego mistrza.", ch, NULL, victim, TO_ALL );
		return;
	}

	if ( IS_AFFECTED(victim,AFF_PARALYZE ))
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
	af.duration = UMAX(1,dur); af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( victim, &af, NULL, TRUE );
	stop_fighting( ch, FALSE );
	stop_fighting( victim, FALSE );

	add_follower( victim, ch, TRUE );

	if ( IS_AFFECTED( victim, AFF_TROLL ) )
		victim->affected_by[ AFF_TROLL.bank ] ^= AFF_TROLL.vector;

	act( "Czujesz nag³y przyp³yw mi³o¶ci do $z.", ch, NULL, victim, TO_VICT );
	act( "$N patrzy na ciebie z uwielbieniem.", ch, NULL, victim, TO_CHAR );
	act( "$N patrzy na $c z uwielbieniem.", ch, NULL, victim, TO_NOTVICT );

	if ( IS_NPC( victim ) && EXT_IS_SET( victim->act, ACT_AGGRESSIVE ) )
		EXT_REMOVE_BIT( victim->act, ACT_AGGRESSIVE );

	//kasowanie prewaitów
	if ( victim->wait_char )
	{
		free_pwait_char( victim->wait_char );
		victim->wait_char = NULL;
		victim->wait = 2;
	}

	stop_hating( victim, ch, FALSE );
	stop_hunting( victim );
	stop_fearing( victim, ch, FALSE );
	stop_hating( ch, victim, FALSE );
	stop_hunting( ch );
	stop_fearing( ch, victim, FALSE );
	return;
}

void spell_charm_monster( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int chance = 1, spec_bonus = 0, dur = 12 - UMAX( -4, get_curr_stat_deprecated( victim, STAT_INT ) - 16 );

	if ( !IS_NPC( ch ) && IS_IMMORTAL( victim ) )
		return;

	if ( is_safe( ch, victim ) )
		return;

	// je¿eli victim to gracz
	if ( !IS_NPC( victim ) )
	{
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Podobasz siê sobie jeszcze bardziej, potworze!\n\r", ch );
		return;
	}

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			spec_bonus = 4;
		}
	}

			if ( victim->level >25 && IS_NPC( victim ) )
	{
		send_to_char( "Nie jeste¶ w stanie zauroczyæ tak potêznej istoty.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_CHARM )
	     || !IS_SET( race_table[ GET_RACE( victim ) ].type , MONSTER )
	     || IS_AFFECTED( ch, AFF_CHARM )
	     || level + spec_bonus < victim->level
	     || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		return;

	if ( IS_AFFECTED( victim, AFF_CHARM ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "Rozpoznajesz, ¿e $N jest ju¿ zauroczone.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "Rozpoznajesz, ¿e $N jest ju¿ zauroczony.", ch, NULL, victim, TO_CHAR );
				break;
			default :
				act( "Rozpoznajesz, ¿e $N jest ju¿ zauroczona.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

    if ( IS_AFFECTED( victim, AFF_TROLL ) )
    {
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
    }

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

	if ( spec_bonus > 0 )
	{
		chance += chance/2;
		chance = URANGE(15,chance,100);
		dur *= 2;
	}

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
			act( "$N rozgl±da siê dooko³a, ale nie widzi swojego mistrza.", ch, NULL, victim, TO_ALL );
		return;
	}

	if ( IS_AFFECTED(victim,AFF_PARALYZE ))
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
	af.duration = UMAX(1,dur); af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( victim, &af, NULL, TRUE );

	add_follower( victim, ch, TRUE );

	if ( IS_AFFECTED( victim, AFF_TROLL ) )
		victim->affected_by[ AFF_TROLL.bank ] ^= AFF_TROLL.vector;

	act( "Czujesz nag³y przyp³yw mi³o¶ci do $z.", ch, NULL, victim, TO_VICT );
	act( "$N patrzy na ciebie z uwielbieniem.", ch, NULL, victim, TO_CHAR );
	act( "$N patrzy na $c z uwielbieniem.", ch, NULL, victim, TO_NOTVICT );

	stop_fighting( ch, FALSE );
	stop_fighting( victim, FALSE );

	if ( IS_NPC( victim ) && EXT_IS_SET( victim->act, ACT_AGGRESSIVE ) )
		EXT_REMOVE_BIT( victim->act, ACT_AGGRESSIVE );

	//kasowanie prewaitów
	if ( victim->wait_char )
	{
		free_pwait_char( victim->wait_char );
		victim->wait_char = NULL;
		victim->wait = 2;
	}

	stop_hating( victim, ch, FALSE );
	stop_hunting( victim );
	stop_fearing( victim, ch, FALSE );
	stop_hating( ch, victim, FALSE );
	stop_hunting( ch );
	stop_fearing( ch, victim, FALSE );
	return;
}

void spell_continual_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * light;
	char *tar = ( char * ) vo;
	int timer;

	if ( tar && tar[ 0 ] != '\0' )
	{
		light = get_obj_carry( ch, tar, ch );

		if ( light == NULL )
		{
			send_to_char( "Nie masz czego¶ takiego przy sobie.\n\r", ch );
			return;
		}

		if ( light->item_type != ITEM_LIGHT )
		{
			send_to_char( "To nie jest przedmiot typu ¶wiat³o.\n\r", ch );
			return;
		}

		if ( IS_OBJ_STAT( light, ITEM_MAGIC ) || IS_OBJ_STAT( light, ITEM_UNDESTRUCTABLE ) )
		{
			send_to_char( "Nie uda³o ci siê.\n\r", ch );
			return;
		}

		act( "Kierujesz kulê ¶wiat³a na $h.", ch, light, NULL, TO_CHAR );
		act( "$n kieruje kulê ¶wiat³a na $h.", ch, light, NULL, TO_ROOM );

		EXT_SET_BIT( light->extra_flags, ITEM_MAGIC );

		if ( light->value[ 2 ] < 0 )
		{
			act( "$p przez chwilê migocze, a po chwili wybucha z dono¶nym hukiem.", ch, light, NULL, TO_ROOM );
			act( "$p przez chwilê migocze, a po chwili wybucha z dono¶nym hukiem rani±c ciê od³amkami.", ch, light, NULL, TO_CHAR );
			damage( ch, ch, UMIN( ch->hit, number_range( 10, 10 + level ) ), TYPE_UNDEFINED, DAM_PIERCE, FALSE );
			if ( is_artefact( light ) ) extract_artefact( light );
			if ( light->contains ) extract_artefact_container( light );
			extract_obj( light );
			return;
		}

		if ( light->value[ 2 ] == 0 )
		{
			light->value[ 2 ] = 1;
			act( "$p przez chwilê migocze, by po chwili zap³on±æ s³abym ¶wiat³em.", ch, light, NULL, TO_ROOM );
			act( "$p przez chwilê migocze, by po chwili zap³on±æ s³abym ¶wiat³em.", ch, light, NULL, TO_CHAR );
			return;
		}

		if ( light->value[ 2 ] > 0 )
		{
			light->value[ 2 ] *= 2;
			/* Bonus dla specjalisty */
			if ( !IS_NPC( ch ) )
			{
				if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
				{
					light->value[ 2 ] += 8;
				}
			}
			act( "$p przez chwilê migocze aby po chwili zap³on±æ mocniejszym ¶wiat³em.", ch, light, NULL, TO_ROOM );
			act( "$p przez chwilê migocze aby po chwili zap³on±æ mocniejszym ¶wiat³em.", ch, light, NULL, TO_CHAR );
			return;
		}
	}
	light = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ), FALSE );
	obj_to_char( light, ch );

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			light->value[ 2 ] += 8;
		}
	}
	act( "$n porusza swoimi palcami i nagle pojawia siê $p.", ch, light, NULL, TO_ROOM );
	act( "Poruszasz palcami i pojawia siê $p.", ch, light, NULL, TO_CHAR );

	/* Niech kula rozpada sie po pewnym czasie */
	if( IS_IMMORTAL( ch ) )
	{
		light->value[ 2 ] = -1;
	}
	else
	{
		timer = 15 + level/5;
		light->timer = UMAX(1, timer);
	}

	return;
}

void spell_chain_lightning( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA *tmp_vict, *last_vict, *next_vict;
	bool found;
	int dam, luck, vluck;

	/* luck */

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ ³añcucha b³yskawic.\n\r", ch );
		return;
	}

	/* first strike */

	act( "B³yskawica wylatuje z r±k $z i leci w kierunku $Z.", ch, NULL, victim, TO_NOTVICT );
	act( "B³yskawica wylatuje z twoich r±k i leci w kierunku $Z.", ch, NULL, victim, TO_CHAR );
	act( "B³yskawica wylatuj±ca z r±k $z w twoim kierunku!", ch, NULL, victim, TO_VICT );

	dam = number_range(15,30) + dice( level, 5 );

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dam = number_range(22,34) + dice( level, 6 );
		}
	}

				if( !spell_item_check( ch, sn , "lighting based spell" ) )
    {
      dam -= dam/5 ;
    }

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

	spell_damage( ch, victim, dam, sn, DAM_LIGHTNING, TRUE );
	shock_effect( victim, level, dam , TARGET_CHAR );

	last_vict = victim;
	level -= 4;   /* decrement damage */

	if ( !ch->in_room )
		return;

	/* new targets */
	while ( level > 0 )
	{
		found = FALSE;
		for ( tmp_vict = ch->in_room->people;
		      tmp_vict != NULL;
		      tmp_vict = next_vict )
		{
			next_vict = tmp_vict->next_in_room;
			if ( !is_safe_spell( ch, tmp_vict, TRUE ) && tmp_vict != last_vict )
			{
				found = TRUE;
				last_vict = tmp_vict;
				act( "Blyskawica trafia $c!", tmp_vict, NULL, NULL, TO_ROOM );
				act( "Blyskawica trafia ciê!", tmp_vict, NULL, NULL, TO_CHAR );
				dam = dice( level, 5 );
				/* Bonus dla specjalisty */
				if ( !IS_NPC( ch ) )
				{
					if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
					{
						dam = dice( level, 6 );
					}
				}

				dam = luck_dam_mod( ch, dam );

				if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
					dam /= 2;

				spell_damage( ch, tmp_vict, dam, sn, DAM_LIGHTNING, TRUE );
				shock_effect( tmp_vict, level, dam, TARGET_CHAR );
				level -= 4;  /* decrement damage */
			}
		}   /* end target searching loop */

		if ( !found )  /* no target found, hit the caster */
		{
			if ( ch == NULL )
				return;

			if ( last_vict == ch )  /* no double hits */
			{
				act( "B³yskawica powoli zanika.", ch, NULL, NULL, TO_ALL );
				return;
			}

			last_vict = ch;
			act( "B³yskawica leci do $z...uuuuppppppssss!", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Zosta³e¶ trafiony przez swoj± b³yskawicê!\n\r", ch );
			dam = dice( level, 5 );
			/* Bonus dla specjalisty */
			if ( !IS_NPC( ch ) )
			{
				if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
				{
					dam = dice( level, 6 );
				}
			}

			dam = luck_dam_mod( ch, dam );

			if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
				dam /= 2;

			spell_damage( ch, ch, dam, sn, DAM_LIGHTNING, TRUE );
			shock_effect( ch, level, dam, TARGET_CHAR );

			level -= 4;  /* decrement damage */
			if ( !ch->in_room )
				return;
		}
		/* now go back and find more targets */
	}
}

/* dosc mocny ale z savem, sukces dam/2*/
void spell_burning_hands( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam,

	/* luck */

	luck = get_curr_stat_deprecated( ch, STAT_LUC ),
  vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê przemieniæ swych d³oni.\n\r", ch );
		return;
	}

	dam = dice( 2, 5 ) + (level*15)/10;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dam = dice( 2, 7 ) + 2 * level;
		}
	}

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}
	spell_damage( ch, victim, dam, sn, DAM_FIRE, TRUE );
	fire_effect( victim, level, dam, TARGET_CHAR );
	return;
}



void spell_acid_blast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam,

	/* luck */

	luck = get_curr_stat_deprecated( ch, STAT_LUC ),
    vluck = get_curr_stat_deprecated( victim, STAT_LUC );


	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ kwasowego podmuchu.\n\r", ch );
		return;
	}

	dam = 25 + dice( (level*3)/4, 4 );

	/* Bonus dla specjalisty i mobow */
	if ( IS_NPC(ch) || ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) ) )
	{
		dam = 25 + dice( level, 4 );
	}

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

		if( !spell_item_check( ch, sn , "acid based spell" ) )
    {
      dam -= dam/5;
    }

	spell_damage( ch, victim, dam, sn, DAM_ACID, TRUE );
	acid_effect( victim, level, dam, TARGET_CHAR );
	return;
}


void spell_mirror_image( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int count;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int dur = 3;

	if ( IS_AFFECTED( victim, AFF_MIRROR_IMAGE ) )
	{
		send_to_char( "Jeste¶ ju¿ pod wp³ywem tego zaklêcia.\n\r", ch );
		return;
	}

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ swych lustrzanych odbiæ.\n\r", ch );
		return;
	}

	if ( !IS_NPC( ch ) )
	{
		count = 3 + level/6;
		if ( ch->pcdata->mage_specialist == 5 ) count += 2;
		if ( number_range( 0, luck ) > 10 ) ++count;
		if ( number_range( 0, luck ) < 3 ) --count;
		if ( count < 1 ) count = 1;
	}
	else
    {
		count = UMIN( 3 + ( level / 6 ), 9 );
    }

	/* Bonus dla specjalisty 2 */

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 5;
		}
    }

    if ( !IS_NPC( ch ) )
    {
        if ( spell_item_check( ch, sn, NULL ) )
        {
            count = ( count * 120 ) / 100;
            ++dur;
        } else {
            count = ( count * 70 ) / 100;
        }
    }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = count;
	af.bitvector = &AFF_MIRROR_IMAGE;

	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			act( "Wokó³ $z pojawiaj± siê jego lustrzane odbicia.", victim, NULL, NULL, TO_ROOM );
			break;
		case 1:
			act( "Wokó³ $z pojawiaj± siê jego lustrzane odbicia.", victim, NULL, NULL, TO_ROOM );
			break;
		default :
			act( "Wokó³ $z pojawiaj± siê jej lustrzane odbicia.", victim, NULL, NULL, TO_ROOM );
			break;
	}
	send_to_char( "Tworzysz swoje lustrzane odbicia.\n\r", victim );
	return;
}

void spell_weaken( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, vluck, dur, mod;

	if ( IS_AFFECTED( victim, AFF_WEAKEN ) || ( victim != ch && saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) ) )
		return;
	/* luck */

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		act( "Nie uda³o ci siê os³abiæ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dur = 4 + level / 4;
	mod = - number_range( 2, 4 );

	/* Bonus dla specjalisty */

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 7 + level / 10;
			mod = - number_range( 3, 5 );
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_STR;
	af.modifier = mod;
	af.bitvector = &AFF_WEAKEN;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Czujesz, ¿e si³y ciê opuszczaj±.\n\r", victim );
	switch ( victim->sex )
	{
		case 0:
			act( "$n wyglada na s³absze.", victim, NULL, NULL, TO_ROOM );
			break;
		case 1:
			act( "$n wyglada na s³abszego.", victim, NULL, NULL, TO_ROOM );
			break;
		default:
			act( "$n wyglada na s³absz±.", victim, NULL, NULL, TO_ROOM );
			break;
	}

	/* opadanie na ziemiê, je¿li jest siê przeci±¿onym */
	if ( !victim->mount && get_carry_weight( victim ) > can_carry_w( victim ) && IS_AFFECTED( victim, AFF_FLYING ) )
	{
		act( "$n pod wp³ywem ciê¿aru opada na ziemiê.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Pod wp³ywem obci±¿enia opadasz na ziemiê.\n\r", victim );
		affect_strip( victim, gsn_float );
	}

	return;
}



/* taki sobie czarek na drugi krag maga */
void spell_strength( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	int dur = 2 + level / 10;

    if ( check_improve_strenth ( ch, victim, TRUE ) ) return;

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ w³asnej si³y.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ si³y $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	switch ( victim->class )
	{
		case CLASS_MAG:
			mod = 1;
			break;
		case CLASS_CLERIC:
		case CLASS_THIEF:
		case CLASS_BARD:
		case CLASS_DRUID:
			if ( get_curr_stat_deprecated( victim, STAT_STR ) > 20 )
				mod = 1;
			else
				mod = number_range( 1, 3 );
			break;
		case CLASS_WARRIOR:
		case CLASS_PALADIN:
		case CLASS_BARBARIAN:
		case CLASS_SHAMAN:
		case CLASS_MONK:
		case CLASS_BLACK_KNIGHT:
			if ( get_curr_stat_deprecated( victim, STAT_STR ) > 23 )
				mod = 1;
			else
				mod = number_range( 2, 3 );
			break;

		default: mod = 1;break;
	}

	if ( ch == victim ) ++mod;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 3 + level / 10;
			if ( number_range(1,2) == 1 )
			{
				++mod;
			}
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
    af.rt_duration = 0;
	af.location = APPLY_STR;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Czujesz dziwn± energiê rozchodz±c± siê po twoim ciele!\n\r", victim );

	if ( victim->sex == SEX_FEMALE )
		act( "$n wydaje siê byc silniejsza.", victim, NULL, NULL, TO_ROOM );
	else if ( victim->sex == SEX_MALE )
		act( "$n wydaje siê byc silniejszy.", victim, NULL, NULL, TO_ROOM );
	else
		act( "$n wydaje siê byc silniejsze.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_web( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, vluck, dur, mod1, mod2;

	if ( IS_AFFECTED( victim, AFF_WEB ) )
	{
		if ( victim == ch )
			send_to_char( "Jeste¶ ju¿ opl±tany magiczn± sieci±.\n\r", ch );
		else
			act( "$N jest ju¿ opl±tan$R magiczn± sieci±.", ch, NULL, victim, TO_CHAR );
		return;
	}

	/* luck */

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê opl±taæ siebie magiczn± sieci±.\n\r", ch );
		else
			act( "Nie uda³o ci siê opl±taæ $C magiczn± sieci±.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( ( victim != ch &&
	       can_move( victim ) &&
	       number_range(0,get_curr_stat_deprecated(victim,STAT_DEX)) > number_range(0,28) )//sejw na dexa
	     || IS_AFFECTED( victim, AFF_FREE_ACTION ) )
	{
		act( "$N zrêcznie odskakuje i twoja magiczna sieæ nie trafia.", ch, NULL, victim, TO_CHAR );
		act( "Zrêcznie odskakujesz i magiczna sieæ $z ciê nie trafia.", ch, NULL, victim, TO_VICT );
		act( "$N zrêcznie odskakuje i magiczna sieæ $z $M nie trafia.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	dur = 3 + level/10;
	mod1 = -3;
	mod2 = 25;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 4 + level/10;
			mod1 = -4;
			mod2 = 40;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.modifier = mod1;
	af.bitvector = &AFF_WEB;
	affect_to_char( victim, &af, NULL, TRUE );

	af.location = APPLY_AC;
	af.modifier = mod2;
	affect_to_char( victim, &af, NULL, TRUE );

	if ( victim != ch )
	{
		if( is_affected(victim,gsn_float) || is_affected(victim,gsn_fly))
		{
			act( "Twoja magiczna sieæ szczelnie opl±tuje siê wokó³ $Z i ¶ci±ga na ziemiê.", ch, NULL, victim, TO_CHAR );
			act( "Magiczna sieæ $z szczelnie ciê opl±tuje i ¶ci±ga ciê na ziemiê.", ch, NULL, victim, TO_VICT );
			act( "Magiczna sieæ $z szczelnie opl±tuje $C i ¶ci±ga na ziemiê.", ch, NULL, victim, TO_NOTVICT );
			affect_strip(victim,gsn_float);
			affect_strip(victim,gsn_fly);
		}
		else
		{
			act( "Twoja magiczna sieæ szczelnie opl±tuje siê wokó³ $Z.", ch, NULL, victim, TO_CHAR );
			act( "Magiczna sieæ $z szczelnie ciê opl±tuje.", ch, NULL, victim, TO_VICT );
			act( "Magiczna sieæ $z szczelnie opl±tuje $C.", ch, NULL, victim, TO_NOTVICT );
		}

	}
	else
	{
		act( "Oplatujesz siê w³asn± magiczn± sieci±.", ch, NULL, victim, TO_CHAR );
		act( "$n opl±tuje siê w³asn± magiczn± sieci±.", ch, NULL, victim, TO_ROOM );
	}
	return;
}

/* mending. cast mending <co>
   no to bedzie, jak mialo byc. naprawia 1% / level. jesli sprzet
   jest bardziej zniszczony niz 2x ile_moze_naprawic to gowno zrobi
   musi z tym isc do kowala. bo inaczej ludzie beda jechac tylko na
   tym czarze, a to troszke bez sensu. niech placa, jak zniszczyli!
   plus nie da rady gratow nomagic reperowac
*/

void spell_mending( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA * obj = ( OBJ_DATA * ) vo;
    bool use_components = spell_item_check( ch, sn, NULL );

    if ( IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
    {
        send_to_char( "Nic siê nie sta³o.\n\r", ch );
        return;
    }
    /**
     * ITEM_NOREPAIR
     */
    if ( EXT_IS_SET( obj->extra_flags, ITEM_NOREPAIR ) )
    {
        send_to_char( "Zab³ys³o, zawarcza³o, zgas³o i ucich³o.\n\r", ch );
        return ;
    }
    if ( obj->condition == 100 )
    {
        send_to_char( "Nic siê nie sta³o.\n\r", ch );
        return;
    }

    if (
            obj->condition < ( 100 - ( 2 * level ) )
            || ( !use_components && obj->condition < 50 )
            || ( use_components && obj->condition < 25 )
            || obj->repair_counter > obj->repair_limit )
    {
        send_to_char( "Ten przedmiot jest zbyt powa¿nie uszkodzony, nie dasz go rady naprawiæ.\n\r", ch );
        return;
    }
    /**
     * jakis peszek?
     */
    if ( number_range( 0, get_curr_stat( ch, STAT_LUC ) + get_curr_stat( ch, STAT_INT ) ) < 10 )
    {
        send_to_char( "Uda³o ci siê rzuciæ zaklêcie, ale co¶ posz³o nie tak.\n\r", ch );
        obj->condition = 1;
        if ( number_percent() < 50 )
        {
            spell_chill_metal( gsn_chill_metal, level, ch, obj, TARGET_OBJ );
        }
        else
        {
            spell_heat_metal( gsn_heat_metal, level, ch, obj, TARGET_OBJ );
        }
        return;
    }
    /**
     * repair limits
     */
    if ( use_components )
    {
        send_to_char( "Uda³o ci siê doskonale rzuciæ zaklêcie.\n\r", ch );
    }
    else
    {
        obj->repair_counter++;
        switch ( ch->sex )
        {
            case SEX_FEMALE:
                send_to_char( "Uda³o ci siê rzuciæ zaklêcie, choæ nie wyzwoli³a¶ jego pe³nej mocy.\n\r", ch );
                break;
            case SEX_NEUTRAL:
                send_to_char( "Uda³o ci siê rzuciæ zaklêcie, choæ nie wyzwoli³o¶ jego pe³nej mocy.\n\r", ch );
                break;
            case SEX_MALE:
            default:
                send_to_char( "Uda³o ci siê rzuciæ zaklêcie, choæ nie wyzwoli³e¶ jego pe³nej mocy.\n\r", ch );
                break;
        }
    }

    obj->condition = UMIN( obj->condition + ( 2 * level ), 100 );
    return;
}

/* skladnia: cast deafness <kto>
   gluchota... hmm i jak toto ma dzialac? pc nie bedzie slyszec jak
   ludzie mowia, a npc nie bedzie reagowac na tell/speach progi
   no innych pomyslow nie mam
   plus 50% szansy na zwalenie czaru
 */
void spell_deafness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, vluck, dur;

	if ( victim == ch )
	{
      if ( IS_AFFECTED( victim, AFF_DEAFNESS ) )
      {
        send_to_char( "Przecie¿ ju¿ nic nie s³yszysz.\n\r", ch );
      }else
      {
		af.where = TO_AFFECTS;
    	af.type = sn;
     	af.level = level;
      	af.duration = 4;
       	af.rt_duration = 0;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = &AFF_DEAFNESS;
        affect_to_char( victim, &af, NULL, TRUE );
       	send_to_char( "G³osy wokó³ ciebie nagle milkn±.\n\r", ch );

      }
      return;
	}

	/* luck */

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		if ( victim != ch )
		{
		act( "Nie uda³o ci siê odebraæ $Z s³uchu.", ch, NULL, victim, TO_CHAR );
		} else {
			send_to_char( "Mimo usilnych starañ nie og³uch³e¶.\n\r", ch );
		}
		return;
	}

	if ( IS_AFFECTED( victim, AFF_DEAFNESS ) )
	{
		if ( victim == ch ) send_to_char( "Przecie¿ ju¿ nic nie s³yszysz.\n\r", ch );
		if ( victim->sex == SEX_NEUTRAL )
		{
			act( "$N ju¿ jest g³uche.\n\r", ch, NULL, victim, TO_CHAR );
		}
		else if ( victim->sex == SEX_MALE )
		{
			act( "$N ju¿ jest g³uchy.\n\r", ch, NULL, victim, TO_CHAR );
		}
		else
		{
			act( "$N ju¿ jest g³ucha.\n\r", ch, NULL, victim, TO_CHAR );
		}
		return;
	}

	if ( IS_AFFECTED(victim,AFF_PERFECT_SENSES) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Nic z tego, nadal s³yszysz.\n\r", ch );
		} else {
			act( "Nie uda³o ci siê odebraæ $Z s³uchu.", ch, NULL, victim, TO_CHAR );
		}
		if ( ch->sex == SEX_NEUTRAL )
		{
			act( "$n próbowa³o odebraæ ci s³uch!", ch, NULL, victim, TO_VICT );
		}
		else if ( ch->sex == SEX_MALE )
		{
			act( "$n probowa³ odebraæ ci s³uch!", ch, NULL, victim, TO_VICT );
		}
		else
		{
			act( "$n probowa³a odebraæ ci s³uch!", ch, NULL, victim, TO_VICT );
		}
		return;
	}

	dur = 4;

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 6;
		}
	}
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_DEAFNESS;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "G³osy wokó³ ciebie nagle milkn±.\n\r", victim );
	act( "$n wydziera siê: Nic nie s³yszysz?!", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_darkness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	int luck;
	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
	{
		send_to_char( "Nie mo¿esz tego tu robiæ.\n\r", ch );
		return;
    }
    /**
     * "rzut" na szczescie
     */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê sprowadziæ ciemno¶ci.\n\r", ch );
		return;
	}
    /**
     * obsluga przedmiotów
     */
    if ( target == TARGET_OBJ )
    {
        AFFECT_DATA af;
        OBJ_DATA * obj = ( OBJ_DATA * ) vo;
        /**
         * sorawdz czy nie jest
         */
        if ( IS_OBJ_STAT( obj, ITEM_DARK ) )
        {
            switch(obj->gender)
            {
                case GENDER_MESKI:
                    act( "$p jest ju¿ otoczony ciemn± aur±.", ch, obj, NULL, TO_CHAR );
                    break;
                case GENDER_ZENSKI:
                    act( "$p jest ju¿ otoczona ciemn± aur±.", ch, obj, NULL, TO_CHAR );
                    break;
                case GENDER_NIJAKI:
                    act( "$p jest ju¿ otoczone ciemn± aur±.", ch, obj, NULL, TO_CHAR );
                    break;
                case GENDER_MESKOOSOBOWY:
                case GENDER_ZENSKOOSOBOWY:
                    act( "$p s± ju¿ otoczone ciemn± aur±.", ch, obj, NULL, TO_CHAR );
                    break;
                case GENDER_NONE:
                default:
                    send_to_char( "Ten przedmiot jest ju¿ otoczony ciemn± aur±.\n\r", ch );
                    break;
            }
            return;
        }
        /**
         * dodaj affect_to_obj
         */
        af.where = TO_OBJECT;
        af.type = sn;
        af.level = level;
        af.duration = 2 * level;
        af.rt_duration = 0;
        af.bitvector = &ITEM_DARK;
        af.location = APPLY_NONE;
        af.modifier = 0;
        affect_to_obj( obj, &af );
        act( "$p otacza siê ciemn± aur±.", ch, obj, NULL, TO_ALL );
        return;
    }

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_MAGICDARK ) )
	{
		send_to_char( "Tu ju¿ jest wystarczaj±co ciemno.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) )
	{
		send_to_char( "To miejsce jest po¶wiêcone i nie dasz rady zaabsorbowaæ jasno¶ci.\n\r", ch );
		return;
	}


	/*bonusik dla speca*/
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			EXT_SET_BIT( ch->in_room->room_flags, ROOM_MAGICDARK );
			send_to_char( "Sprawiasz, ¿e ¶wiat³o w pokoju blednie. Po chwili zapada mrok.\n\r", ch );
			act( "¦wiat³o nagle blednie, a po chwili zapada nieprzenikniona ciemno¶æ.", ch, NULL, NULL, TO_ROOM );
			create_event( EVENT_DARKNESS_END, PULSE_TICK * ( 3 + level / 6 ), ch->in_room, NULL, 0 );
			return;
		}
	}
	create_event( EVENT_DARKNESS_END, PULSE_TICK * ( 2 + level / 7.5 ), ch->in_room, NULL, 0 );
	EXT_SET_BIT( ch->in_room->room_flags, ROOM_MAGICDARK );
	send_to_char( "Sprawiasz, ¿e ¶wiat³o w pokoju blednie. Po chwili zapada mrok.\n\r", ch );
	act( "¦wiat³o nagle blednie, a po chwili zapada nieprzenikniona ciemno¶æ.", ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_resist_normal_weapon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration = 4;
	AFFECT_DATA af;

	if ( IS_AFFECTED( ch, AFF_RESIST_MAGIC_WEAPON ) ||
	     IS_AFFECTED( ch, AFF_RESIST_NORMAL_WEAPON ) ||
	     IS_AFFECTED( ch, AFF_RESIST_WEAPON ) )
	{
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie, ale niczego ono nie zmienia.\n\r", ch );
		return;
	}

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ pola chroni±cego przed zwyk³± broni±.\n\r", ch );
		return;
	}

	/* Bonus dla specjalisty */

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration = 6;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 1;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_MAGIC_WEAPON;
	af.bitvector = &AFF_RESIST_NORMAL_WEAPON;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Otacza ciê pole chroni±ce przed zwyk³± broni±.\n\r", ch );
	return;
}

void spell_resist_magic_weapon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int duration = 4;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( ch, AFF_RESIST_MAGIC_WEAPON ) ||
	     IS_AFFECTED( ch, AFF_RESIST_NORMAL_WEAPON ) ||
	     IS_AFFECTED( ch, AFF_RESIST_WEAPON ) )
	{
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie, ale niczego ono nie zmienia.\n\r", ch );
		return;
	}

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ pola chroni±cego przed magiczn± broni±.\n\r", ch );
		return;
	}

	/* Bonus dla specjalisty */

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration = 6;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 7;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_MAGIC_WEAPON;
	af.bitvector = &AFF_RESIST_MAGIC_WEAPON;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Otacza ciê pole chroni±ce przed magiczn± broni±.\n\r", ch );
	return;
}

void spell_resist_elements( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int dur = 4;
	int aflevel = 30+level/2;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ swej odporno¶ci na ¿ywio³y.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ odporno¶ci na ¿ywio³y $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	/* Bonus dla specjalisty */

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 6;
			aflevel += level/2;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = aflevel;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_RESIST;

	if ( !IS_AFFECTED( victim, AFF_RESIST_ACID ) )
	{
		af.modifier = RESIST_ACID;
		af.bitvector = &AFF_RESIST_ACID;
		affect_to_char( victim, &af, NULL, TRUE );
	}

	if ( !IS_AFFECTED( victim, AFF_RESIST_COLD ) )
	{
		af.modifier = RESIST_COLD;
		af.bitvector = &AFF_RESIST_COLD;
		affect_to_char( victim, &af, NULL, TRUE );
	}

	if ( !IS_AFFECTED( victim, AFF_RESIST_FIRE ) )
	{
		af.modifier = RESIST_FIRE;
		af.bitvector = &AFF_RESIST_FIRE;
		affect_to_char( victim, &af, NULL, TRUE );
	}

	if ( !IS_AFFECTED( victim, AFF_RESIST_LIGHTNING ) )
	{
		af.modifier = RESIST_ELECTRICITY;
		af.bitvector = &AFF_RESIST_LIGHTNING;
		affect_to_char( victim, &af, NULL, TRUE );
	}

	if ( victim->sex == SEX_NEUTRAL )
	{
		act( "$n staje siê bardziej odporne na oddzia³ywanie ¿ywio³ów.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( victim->sex == SEX_MALE )
	{
		act( "$n staje siê bardziej odporny na oddzia³ywanie ¿ywio³ów.", victim, NULL, NULL, TO_ROOM );
	}
	else
	{
		act( "$n staje siê bardziej odporna na oddzia³ywanie ¿ywio³ów.", victim, NULL, NULL, TO_ROOM );
	}

	send_to_char( "Jeste¶ teraz odporn<&y/a/e> na ¿ywio³y.\n\r", victim );

	return;
}

void spell_lesser_magic_resist( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_RESIST_MAGIC ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo¿esz byæ bardziej chronion<&y/a/e>.\n\r", ch );
		else
		{
			switch ( ch->sex )
			{
				case 0:
					act( "$N nie mo¿e byæ bardziej chronione.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N nie mo¿e byæ bardziej chroniona.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo¿e byæ bardziej chroniony.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch != victim )
			act( "Nie uda³o ci siê zwiêkszyæ odporno¶ci $Z na magiê.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda³o ci siê zwiêkszyæ swojej odporno¶ci na magiê.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 10 + level/2;
	af.duration = 3; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_MAGIC;
	af.bitvector = &AFF_RESIST_MAGIC;
	affect_to_char( victim, &af, NULL, TRUE );

	if ( victim != ch )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N jest teraz bardziej odporne na magiê.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N jest teraz bardziej odporna na magiê.", ch, NULL, victim, TO_CHAR );
				break;
			default :
				act( "$N jest teraz bardziej odporny na magiê.", ch, NULL, victim, TO_CHAR );
				break;
		}
	}
	else
	{
		send_to_char( "Czujesz, ¿e teraz jeste¶ bardziej odporn<&y/a/e> na magiê.\n\r", ch );
	}
	return;
}

void spell_fireshield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int luck, dur;

	if ( IS_AFFECTED( ch, AFF_FIRESHIELD ) )
	{
		send_to_char( "Jeste¶ ju¿ otoczon<&y/a/e> ognist± tarcz±.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_ICESHIELD ) )
	{
		send_to_char( "Jeste¶ ju¿ otoczon<&y/a/e> lodow± tarcz±.\n\r", ch );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê otoczyæ siebie ognist± tarcz±.\n\r", ch );
		return;
	}

	dur = 2 + level/4;
	/* Bonus dla specjalisty */

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 2 + level / 3;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_FIRE;
	af.bitvector = &AFF_FIRESHIELD;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Wokó³ ciebie tworzy siê ognista tarcza.\n\r", ch );
	act( "Wokó³ $z tworzy siê ognista tarcza.", ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_iceshield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int luck, dur;

	if ( IS_AFFECTED( ch, AFF_ICESHIELD ) )
	{
		send_to_char( "Jeste¶ ju¿ otoczon<&y/a/e> lodow± tarcz±.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_FIRESHIELD ) )
	{
		send_to_char( "Jeste¶ ju¿ otoczon<&y/a/e> ognist± tarcz±.\n\r", ch );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê otoczyæ siebie lodow± tarcz±.\n\r", ch );
		return;
	}

	dur = 2 + level / 4;
	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 2 + level / 3;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_COLD;
	af.bitvector = &AFF_ICESHIELD;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Wokó³ ciebie tworzy siê lodowa tarcza.\n\r", ch );
	act( "Wokó³ $z tworzy siê lodowa tarcza.", ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_reflect_spell_I( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int luck, dur;

	if ( IS_AFFECTED( ch, AFF_REFLECT_SPELL ) )
	{
		send_to_char( "Jeste¶ ju¿ otoczon<&y/a/e> tarcz± odbijaj±c±.\n\r", ch );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê otoczyæ siebie tarcz± odbijaj±c±.\n\r", ch );
		return;
	}

	/* Bonus dla specjalisty */
	dur = 2;
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 2 + level / 15;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 4;
	af.bitvector = &AFF_REFLECT_SPELL;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Czujesz jak wokó³ ciebie skupia siê magiczna energia.\n\r", ch );
	act( "Wokó³ $z skupia siê magiczna energia.", ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_reflect_spell_II( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int luck, dur;

	if ( IS_AFFECTED( ch, AFF_REFLECT_SPELL ) )
	{
		send_to_char( "Jeste¶ ju¿ otoczon<&y/a/e> tarcz± odbijaj±c±.\n\r", ch );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê otoczyæ siebie tarcz± odbijaj±c±.\n\r", ch );
		return;
	}

	/* Bonus dla specjalisty */
	dur = 2;
	if ( !IS_NPC( ch ) )
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
			dur = 2 + level / 15;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 8;
	af.bitvector = &AFF_REFLECT_SPELL;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Czujesz jak wokó³ ciebie skupia siê ogromna, magiczna energia.\n\r", ch );
	act( "Wokó³ $z skupia siê ogromna, magiczna energia.", ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_reflect_spell_III( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int luck, dur;

	if ( IS_AFFECTED( ch, AFF_REFLECT_SPELL ) )
	{
		send_to_char( "Jeste¶ ju¿ otoczon<&y/a/e> tarcz± odbijaj±c±.\n\r", ch );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê otoczyæ siebie tarcz± odbijaj±c±.\n\r", ch );
		return;
	}

	/* Bonus dla specjalisty */
	dur = 2;
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 2 + level / 15;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 12;
	af.bitvector = &AFF_REFLECT_SPELL;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Czujesz jak wokó³ ciebie skupia siê potê¿na, magiczna energia.\n\r", ch );
	act( "Wokó³ $z skupia siê potê¿na, magiczna energia.", ch, NULL, NULL, TO_ROOM );
	return;
}

/* mocny z savem, sukces dam/2*/
void spell_cone_of_cold( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam,

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC ),
	       vluck = get_curr_stat_deprecated( victim, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ sto¿ka zimna.\n\r", ch );
		return;
	}

	dam = number_range( 40, 60 ) + dice( level - 7, 4 );

	/* Bonus dla specjalisty i mobow */
	if ( IS_NPC(ch) || ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) ) )
	{
		dam = number_range( 50, 65 ) + dice( level - 5, 4 );
	}

	if ( IS_NPC( ch ) )
	{
		dam = number_range( 40, 65 ) + dice( URANGE( 1, level - 5, 30 ), 4 );
	}

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

				if( !spell_item_check( ch, sn , "cold based spell" ) )
    {
      dam -= dam/5;
    }

	spell_damage( ch, victim, dam, sn, DAM_COLD, TRUE );
	cold_effect( victim, level, dam, TARGET_CHAR );
	return;
}

void gwiazdki_statystyk(CHAR_DATA *ch, int stat)
{
	int zmienna,count = 0;

	print_char( ch, "[" );
	for(zmienna = stat ; zmienna > 60 ; zmienna = zmienna-10 )
	{
		print_char( ch, "*" );
		count++;
	}
	for (count ; count < 20 ; count++ ) print_char( ch, " " );

	print_char( ch, "]" );
	return;
}

void spell_lore( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int str =  get_curr_stat( victim, STAT_STR );
	int inte =  get_curr_stat( victim, STAT_INT );
	int wis =  get_curr_stat( victim, STAT_WIS );
	int dex =  get_curr_stat( victim, STAT_DEX );
	int con =  get_curr_stat( victim, STAT_CON );
	int cha =  get_curr_stat( victim, STAT_CHA );
	int luc =  get_curr_stat( victim, STAT_LUC );

	if ( victim->level < 4 )
	{
		print_char( vo, "Nie uda³o ci siê.\n\r" );
		return;
	}

	print_char( victim, "\n\n\rTwoje cechy:\n\r" );
	print_char( victim, "Si³a:			");
	gwiazdki_statystyk(victim, str);
	print_char( victim, "\n\r");

	print_char( victim, "Inteligencja:		" );
	gwiazdki_statystyk(victim, inte);
	print_char( victim, "\n\r");

	print_char( victim, "Wiedza:			" );
	gwiazdki_statystyk(victim, wis);
	print_char( victim, "\n\r");

	print_char( victim, "Zrêczno¶æ:		" );
	gwiazdki_statystyk(victim, dex);
	print_char( victim, "\n\r");

	print_char( victim, "Kondycja:		");
	gwiazdki_statystyk(victim, con);
	print_char( victim, "\n\r");

	print_char( victim, "Charyzma:		");
	gwiazdki_statystyk(victim, cha);
	print_char( victim, "\n\r");

	print_char( victim, "Szczê¶cie:		");
	gwiazdki_statystyk(victim, luc);
	print_char( victim, "\n\r");

	print_char( victim, "Wzrost:       %dcm\n\r", victim->height );
	print_char( victim, "Waga:         %d.%dkg\n\r", victim->weight / 10, victim->weight % 10 );
	print_char( victim, "Wiek:         %d lat\n\r", get_age( victim ) );

	return;
}

/* frag na 1 krag, podobnie jak burning hands */
void spell_acid_hands( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam,

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC ),
    vluck = get_curr_stat_deprecated( victim, STAT_LUC );

    if( level < 20 && !spell_item_check( ch, sn , "acid based spell" ) )
    {
        if ( level < LEVEL_NEWBIE )
        {
            send_to_char ( "{yPróbujesz rzuciæ zaklêcie, które wymaga uzycia {Ykomponentu{y, czyli przedmiotu który je wspomo¿e.{x\n\r", ch );
            send_to_char ( "{yDowiedzieæ siê o nim mo¿esz z ksiêgi czaru 'acid hands'. Ksiêgi owe widziano ostatnio u magów.{x\n\r", ch );
            send_to_char ( "{ySpróbuj zdobyæ lub kupiæ jedn± z nich oraz j± przestudiowaæ ( '{Rstudy magiczna ksiêga{y' ).{x\n\r", ch );
        }
        else
        {
            send_to_char( "Brakuje ci kwasu.\n\r", ch );
        }
		return;
    }

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê przemieniæ swych d³oni.\n\r", ch );
		return;
	}

	dam = dice( 2, 4 ) + URANGE( 1, level, 18 );

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dam = dice( 2, 6 ) + level;
		}
	}

	if ( IS_NPC( ch ) )
	{
		dam = dice( 2, 5 ) + level;
	}

	dam = luck_dam_mod( ch, dam );

	spell_damage( ch, victim, dam, sn, DAM_ACID, TRUE );
	acid_effect( victim, level, dam, TARGET_CHAR );
	return;
}

void spell_fire_darts( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int shots, damage_bonus = 0, luck = get_curr_stat_deprecated( ch, STAT_LUC ), vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ ognistych strza³ek.\n\r", ch );
		return;
	}

	shots = level / 4;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			shots = URANGE( 1, ( level - 2 ) / 2, 9 );
			damage_bonus = 1;
		}
	}
/*//Rysand: do czasu wyjasnienia zakomentowane

  //dla poziomow 1-4 z definicji shots==0, dodatkowe losowanie na jedna strzalke
	if ((shots == 0)  && (number_range( 0, LUCK_BASE_MOD + luck - vluck ) > 0) )
	{
		shots = 1;
	}
*/
  //jesli nie udalo sie stworzyc ani jednej strzalki - komunikat
	if(shots == 0)
	{
		send_to_char( "Nie uda³o ci siê stworzyæ ognistych strza³ek.\n\r", ch );
		return;
	}

	for ( ; shots > 0; shots-- )
	{
		if ( !victim->in_room || ch->in_room != victim->in_room )
		{
			send_to_char( "Nie widzisz celu swojego zaklêcia.\n\r", ch );
			return;
		}
		spell_damage( ch, victim, dice( 2, 3 ) + damage_bonus, sn, DAM_FIRE , TRUE );
	}

	return;
}

/* frag na 2 krag */
void spell_frost_hands( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam,

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC ),
	       vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê przemieniæ swych d³oni.\n\r", ch );
		return;
	}

	dam = dice( 2, 5 ) + level;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dam = dice( 2, 10 ) + level;
		}
	}

	dam = luck_dam_mod( ch, dam );

			if( spell_item_check( ch, sn , "cold based spell" ) )
    {
      dam += 6;
    }

	spell_damage( ch, victim, dam, sn, DAM_COLD, TRUE );
	cold_effect( victim, level, dam, TARGET_CHAR );
	return;
}

/*
 * daje na jeden celny atak bonus +3 do trafienia, tylko na 1 tick dzial
 */
/* krag 2 */
void spell_bladethirst( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;

	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int dur = 0; // dlugosc trwania affectu
	dur += ch->level / 7;  // standardowy licznik
	dur += ch->level / LEVEL_HERO; // bonus dla 31 levelu
	int mod = 0; // modyfikator do trafienia
	mod += ch->level / 10;
	mod += ch->level / 31;
	mod += 2;

	bool specialist = FALSE;

	if ( obj->item_type != ITEM_WEAPON )
	{
		send_to_char( "To nie jest broñ.\n\r", ch );
		return;
	}

	if ( obj->wear_loc != -1 )
	{
		send_to_char( "Musisz to mieæ przy sobie.\n\r", ch );
		return;
	}

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) || number_percent() < 25 || IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
	{
		send_to_char( "Nic siê nie sta³o.\n\r", ch );
		return;
	}

	if ( affect_find( obj->affected, sn ) )
	{
		send_to_char( "Nic siê nie sta³o.\n\r", ch );
		return;
	}

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur += 3;
			mod *= 2;
			specialist = TRUE;
		}
	}
// modyfikator dodatni
	af.where = TO_OBJECT;
	af.location = APPLY_HITROLL;
	af.modifier = mod+3;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.bitvector = &AFF_NONE;
/*	obj->value[ 5 ] += mod; */

	affect_to_obj( obj, &af );
// modyfikator ujemny
	af.where = TO_OBJECT;
	af.location = APPLY_HITROLL;
	af.modifier = -3;
	af.type = 0;
	af.level = 50; // coby sie nie dalo zdispelowac negatywnego efektu
	af.duration = dur+3; af.rt_duration = 0;
	af.bitvector = &AFF_NONE;

	affect_to_obj( obj, &af );

	if ( specialist )
		act( "$p rozb³yskuje {rkrwis{Rtocze{rrwonym{R ¶wiat³{rem{x.", ch, obj, NULL, TO_ALL );
	else
		act( "$p rozb³yskuje bia³ym ¶wiat³em.", ch, obj, NULL, TO_ALL );
	return;
}

/* level 2 frag */
void spell_cold_snap( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam,

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC ),
	       vluck = get_curr_stat_deprecated( victim, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ sopla lodu.\n\r", ch );
		return;
	}

	dam = dice( 2, 4 ) + ( level - 4 ) * 2;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dam = dice( 2, 6 ) + level * 2;
		}
	}

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

			if( spell_item_check( ch, sn , "cold based spell" ) )
    {
      dam += 6;
    }

	spell_damage( ch, victim, dam, sn, DAM_COLD, TRUE );
	cold_effect( victim, level, dam, TARGET_CHAR );
	return;
}

/* szybka regeneracja mv przez 3+ ticki,
   ale po tym na 3 ticki -5 str, -5 dex, -mv regen
*/

void spell_energize( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_ENERGIZE ) || !is_same_group( ch, victim ) )
	{
		if ( ch == victim )
		{
			print_char( ch, "Nie czujesz przecie¿ ¿adnego zmêczenia.\n\r" );
		}
		else
		{
			if ( victim->sex == SEX_NEUTRAL )
			{
				act( "$N wcale nie wygl±da na zmêczone.", ch, NULL, victim, TO_CHAR );
			}
			else if ( victim->sex == SEX_MALE )
			{
				act( "$N wcale nie wygl±da na zmêczonego.", ch, NULL, victim, TO_CHAR );
			}
			else
			{
				act( "$N wcale nie wygl±da na zmêczon±.", ch, NULL, victim, TO_CHAR );
			}
		}
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 2 + UMIN( 4, level / 3 ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_ENERGIZE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Czujesz jak zmêczenie ustêpuje i zaczyna rozsadzaæ ciê energia.\n\r", victim );
	if ( victim->sex == SEX_NEUTRAL )
	{
		act( "$n przeci±ga siê i nie wygl±da ju¿ na tak zmêczone.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( victim->sex == SEX_MALE )
	{
		act( "$n przeci±ga siê i nie wygl±da ju¿ na tak zmêczonego.", victim, NULL, NULL, TO_ROOM );
	}
	else
	{
		act( "$n przeci±ga siê i nie wygl±da ju¿ na tak zmêczon±.", victim, NULL, NULL, TO_ROOM );
	}

	if ( ch != victim )
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie.\n\r", ch );

	victim->move = UMAX( victim->move, victim->max_move / 2 );
	return;
}

/* usypia kogos z grupki na 5 tickow, po zejsciu czaru
   koles dostaje leczonko, maks 75% hp wyleczonych */
void spell_healing_sleep( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if (ch->class != CLASS_SHAMAN)
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		act( "Nie uda³o ci siê u¶piæ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_SLEEP ) )
	{
		act( "Przecie¿ $N ju¿ ¶pi.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( !is_same_group( ch, victim ) )
	{
		print_char( ch, "Dziwne, mimo ca³ych starañ zaklêcie ci nie wysz³o...\n\r" );
		return;
	}

	/* opadanie na ziemiê, jezeli jest siê pod wplywem czaru "fly" */
	if ( IS_AFFECTED( victim, AFF_FLYING ) )
	{
		act( "$n opada na ziemiê.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Powoli opadasz na ziemiê.\n\r", victim );
		affect_strip( victim, 52 );
	}

  if (ch->class == CLASS_SHAMAN)
  {
     if ( check_shaman_invoke ( ch ) == TRUE )
     {
      send_to_char( "Czujesz ogarniaj±c± ciê senno¶æ... chrrr...chrrr...\n\r", victim );
      do_function( victim, &do_sleep, "" );

      af.where = TO_AFFECTS;
      af.type = sn;
      af.level = level;
      af.duration = 3; af.rt_duration = 0;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.bitvector = &AFF_SLEEP;
      affect_to_char( victim, &af, NULL, TRUE );

         if ( victim->position != POS_SLEEPING )
         {
            bug("position != POS_SLEEPING !!!",0);
            return;
         }
         return;
      }
      else
      {
         send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
         return;
      }
  }

	send_to_char( "Czujesz ogarniaj±c± ciê senno¶æ... chrrr...chrrr...\n\r", victim );
//	act( "$n k³adzie siê i zasypia.", victim, NULL, NULL, TO_ROOM );

	do_function( victim, &do_sleep, "" );

	if ( victim->position != POS_SLEEPING )
	{
		bug("position != POS_SLEEPING !!!",0);
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 3; af.rt_duration = 0; //jak zmienisz ten parametr to zajrzyj tez do handler.c do oblicznia bonusu hp przy spadaniu czaru
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_SLEEP;
	affect_to_char( victim, &af, NULL, TRUE );

	return;

}

void spell_ethereal_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck;
	int dur = 2;

	if ( IS_AFFECTED( victim, AFF_ETHEREAL_ARMOR ) )
	{
		send_to_char( "Ju¿ jeste¶ chronion<&y/a/e> przez eteryczny pancerz.\n\r", ch );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê otoczyæ siebie eterycznym pancerzem.\n\r", ch );
		return;
	}

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 2 + level / 15;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.modifier = -3;
	af.location = APPLY_SAVING_ROD;
	af.bitvector = &AFF_ETHEREAL_ARMOR;
	affect_to_char( victim, &af, NULL, TRUE );

	af.bitvector = &AFF_NONE;
	af.location = APPLY_SAVING_PETRI;
	af.modifier = -3;
	affect_to_char( victim, &af, NULL, TRUE );

	af.bitvector = &AFF_NONE;
	af.location = APPLY_SAVING_SPELL;
	af.modifier = -3;
	affect_to_char( victim, &af, NULL, TRUE );

	af.bitvector = &AFF_NONE;
	af.location = APPLY_SAVING_BREATH;
	af.modifier = -3;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Tworzy siê wokó³ ciebie eteryczny pancerz.\n\r", victim );
	act( "Wokó³ $z tworzy siê eteryczny pancerz.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_domination( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, vluck, dur;

	if ( !IS_NPC( ch ) && IS_IMMORTAL( victim ) )
		return;

	if ( is_safe( ch, victim ) )
		return;

	if ( victim == ch )
	{
		send_to_char( "Podobasz siê sobie jeszcze bardziej!\n\r", ch );
		return;
	}

	/* luck */

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		act( "Nie uda³o ci siê uzyskaæ kontroli nad $V.", ch, NULL, victim, TO_CHAR );
		return;
	}

		if ( victim->level >25 && IS_NPC( victim ) )
	{
		send_to_char( "Nie jeste¶ w stanie zdominowaæ tak potêznej istoty.\n\r", ch );
		return;
	}

	if ( ( !IS_NPC( victim ) && !IS_NPC( ch ) )
	     || IS_AFFECTED( victim, AFF_CHARM )
	     || is_undead(victim)
	     || IS_AFFECTED( ch, AFF_CHARM )
	     || level < victim->level
	     || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Próbujesz, próbujesz, ale nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( victim->in_room->room_flags, ROOM_LAW ) )
	{
		send_to_char( "To siê tutaj nie uda.\n\r", ch );
		return;
	}

	if ( !add_charm( ch, victim, FALSE ) )
		return;

//to dotyczy tylko maga ogolnego
	dur = 5 + (level - 15)/2;
	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur *= 2;
		}
	}

	if( stat_throw( ch, STAT_INT) ) ++dur;
	if( stat_throw( ch, STAT_WIS) ) ++dur;
	if( stat_throw( ch, STAT_LUC) ) ++dur;
	if( !stat_throw( ch, STAT_INT) ) --dur;
	if( !stat_throw( ch, STAT_WIS) ) --dur;
	if( !stat_throw( ch, STAT_LUC) ) --dur;

	die_follower( victim, FALSE );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 5, dur ); af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( victim, &af, NULL, TRUE );

	add_follower( victim, ch, TRUE );

	act( "$n przejmuje kontrolê nad twoim umys³em.", ch, NULL, victim, TO_VICT );

	if ( ch != victim )
		act( "Przejmujesz kontrolê nad umys³em $Z.", ch, NULL, victim, TO_CHAR );
	act( "$n przejmuje kontrolê nad umys³em $Z.", ch, NULL, victim, TO_ROOM );

	stop_fighting( ch, FALSE );
	stop_fighting( victim, FALSE );

	if ( IS_AFFECTED( victim, AFF_TROLL ) )
		victim->affected_by[ AFF_TROLL.bank ] ^= AFF_TROLL.vector;

	if ( IS_NPC( victim ) && EXT_IS_SET( victim->act, ACT_AGGRESSIVE ) )
		EXT_REMOVE_BIT( victim->act, ACT_AGGRESSIVE );

	//kasowanie prewaitów
	if ( victim->wait_char )
	{
		free_pwait_char( victim->wait_char );
		victim->wait_char = NULL;
		victim->wait = 2;
	}

	stop_hating( victim, ch, FALSE );
	stop_hunting( victim );
	stop_fearing( victim, ch, FALSE );
	stop_hating( ch, victim, FALSE );
	stop_hunting( ch );
	stop_fearing( ch, victim, FALSE );
	return;
}

/* zaklecie powodujace zwiekszone obrazenia od fizycznych obrazen
 * PIERCE/SLASH/BASH
 * Nekromanci - 3 krag
 * od poziomu zalezy:
 * - ilosc ciosow ze zwiekszonymi obrazeniami
 *   (4 baza, +1 cios na kazde 5 poziomow
 * - % zwiekszone obrazenia (20% baza, +2% na poziom */
// Drake: Wprowadzanie ograniczenia na max zwiekszenie o 50% dla niespec i 60% dlaspec.

void spell_increase_wounds( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, vluck, dur, mod;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC ),
	       vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê otoczyæ wroga czerwon± pulsuj±c± aur±.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_INCREASE_WOUNDS )
	     || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	dur = 1 + level / 2;
	mod = 5 + ( 2 * level );
	if (mod > 50) mod = 50;
	/* specjalisci */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 5 + level / 2;
			mod = 10 + ( 2 * level );
			if (mod > 60) mod = 60;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;      /* ilosc ciosow */
	af.location = 0;
	af.modifier = mod;  /* mnoznik obrazen */
	af.bitvector = &AFF_INCREASE_WOUNDS;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Otacza ciê czerwona, pulsuj±ca aura.\n\r", victim );
	act( "Wokó³ $z pojawia siê czerwona, pulsuj±ca aura.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_ray_of_enfeeblement( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dur, luck, vluck;

	dur = 1 + level / 3;
	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê siebie os³abiæ.\n\r", ch );
		else
			act( "Nie uda³o ci siê os³abiæ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_affected( victim, sn ) ||
	     saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	/* specjalisci */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 2 + level / 3;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.bitvector = &AFF_NONE;

	if ( IS_SET( race_table[ GET_RACE( victim ) ].type , PERSON ) )
	{
		af.location = APPLY_STR;
		af.modifier = - 5;
		affect_to_char( victim, &af, NULL, TRUE );

		af.location = APPLY_HITROLL;
		af.modifier = -2;
		affect_to_char( victim, &af, NULL, TRUE );

		af.location = APPLY_DAMROLL;
		af.modifier = -1;
		affect_to_char( victim, &af, NULL, TRUE );
	}
	else
	{
		af.location = APPLY_HITROLL;
		af.modifier = -2;
		affect_to_char( victim, &af, NULL, TRUE );
	}

	send_to_char( "Czujesz ¿e si³y ciê opuszczaj±.\n\r", victim );
	if ( victim->sex == SEX_NEUTRAL )
	{
		act( "$n wygl±da na s³absze.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( victim->sex == SEX_MALE )
	{
		act( "$n wygl±da na s³abszego.", victim, NULL, NULL, TO_ROOM );
	}
	else
	{
		act( "$n wygl±da na s³absz±.", victim, NULL, NULL, TO_ROOM );
	}
	return;
}

void spell_flame_arrow( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int shots, dam, damage_bonus = 0, vluck, luck = get_curr_stat_deprecated( ch, STAT_LUC );

	vluck = get_curr_stat_deprecated( victim, STAT_LUC );
	if ( victim == ch )
	{
		send_to_char( "Nie dasz rady tego zrobiæ.\n\r", ch );
		return;
	}

	/* zale¿no¶æ od szczê¶cia */
	if ( number_range( 0, luck + LUCK_BASE_MOD - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ p³on±cych strza³.\n\r", ch );
		return;
	}

	shots = URANGE( 1, level / 6, 5 );

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			shots = URANGE( 1, level / 5, 6 );
			damage_bonus = 2;
		}
	}

	if ( IS_NPC( ch ) || IS_IMMORTAL(ch) )
	{
		shots = level / 5;
	}

	/* 2 damy, 1d8 pierce + 4d6 fire */
	for ( ; shots > 0; shots-- )
	{
		if ( !victim->in_room || ch->in_room != victim->in_room )
		{
			send_to_char( "Nie widzisz celu swojego zaklêcia.\n\r", ch );
			return;
		}
		act( "$N krzywi siê z bólu kiedy twoja p³on±ca strza³a wbija siê w $S cia³o.", ch, NULL, victim, TO_CHAR );
		act( "$N ciska w twoim kierunku p³on±c± strza³± która z ogromn± si³± wbija siê w twoje cia³o.", victim, NULL, ch, TO_CHAR );
		act( "$n ciska w kierunku $Z p³on±c± strza³ê która z ogromn± si³± wbija siê w $S cia³o.", ch, NULL, victim, TO_NOTVICT );

		spell_damage( ch, victim, dice( 2, 5 ) + damage_bonus , sn, DAM_PIERCE , FALSE );

		dam = dice( 4, 6 );

		dam = luck_dam_mod( ch, dam );

		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
			dam /= 2;

		spell_damage( ch, victim, dam + damage_bonus, sn, DAM_FIRE , FALSE );
		fire_effect( victim, level, dam, TARGET_CHAR );
	}
	return;
}

void spell_minor_globe( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

	/* luck */
	int luck, dur;
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ sfery niewra¿liwo¶ci.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_MINOR_GLOBE ) ||
	     IS_AFFECTED( ch, AFF_GLOBE ) ||
	     IS_AFFECTED( ch, AFF_MAJOR_GLOBE ) ||
	     IS_AFFECTED( ch, AFF_ABSOLUTE_MAGIC_PROTECTION ) )
	{
		if ( ch->sex == SEX_NEUTRAL )
		{
			send_to_char( "Ju¿ jeste¶ chronione przez sfer± niewra¿liwo¶ci.\n\r", ch );
		}
		else if ( ch->sex == SEX_MALE )
		{
			send_to_char( "Ju¿ jeste¶ chroniony przez sfer± niewra¿liwo¶ci.\n\r", ch );
		}
		else
		{
			send_to_char( "Ju¿ jeste¶ chroniona przez sfer± niewra¿liwo¶ci.\n\r", ch );
		}
		return;
	}

	dur = 6 + level / 5;
	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 8 + level / 5;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_MINOR_GLOBE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Wokó³ ciebie tworzy siê pó³przezroczysta, pulsuj±ca sfera.\n\r", victim );
	act( "Wokó³ $z tworzy siê pó³przezroczysta, pulsuj±ca sfera.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_globe( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, dur;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ sfery niewra¿liwo¶ci.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_MINOR_GLOBE ) ||
	     IS_AFFECTED( ch, AFF_GLOBE ) ||
	     IS_AFFECTED( ch, AFF_MAJOR_GLOBE ) ||
	     IS_AFFECTED( ch, AFF_ABSOLUTE_MAGIC_PROTECTION ) )
	{
		if ( ch->sex == SEX_NEUTRAL )
		{
			send_to_char( "Ju¿ jeste¶ chronione przez sferê niewra¿liwo¶ci.\n\r", ch );
		}
		else if ( ch->sex == SEX_MALE )
		{
			send_to_char( "Ju¿ jeste¶ chroniony przez sferê niewra¿liwo¶ci.\n\r", ch );
		}
		else
		{
			send_to_char( "Ju¿ jeste¶ chroniona przez sferê niewra¿liwo¶ci.\n\r", ch );
		}
		return;
	}

	dur = 7 + level / 5;
	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 10 + level / 5;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_GLOBE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Wokó³ ciebie tworzy siê pó³przezroczysta, pulsuj±ca bia³ym ¶wiat³em sfera.\n\r", victim );
	act( "Wokó³ $z tworzy siê pó³przezroczysta, pulsuj±ca bia³ym ¶wiat³em sfera.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_comprehend_languages( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, dur;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( is_affected( victim, sn ) || IS_AFFECTED( victim, AFF_COMPREHEND_LANGUAGES ) )
	{
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie, ale nic siê nie zmienia.\n\r", ch );
		return;
	}

	dur = 2;
	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 5;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_COMPREHEND_LANGUAGES;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Nagle twoj± g³owê wype³nia wiedza o wszystkich jêzykach ¶wiata.\n\r", victim );
	return;
}

void spell_power_word_blindness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int hp_percent, luck, vluck, mod, dur, mod2;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	mod = 39;//b ciezkie
	dur = 3;
	mod2 = 100;//hapkow

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			mod = 54;//ciezkie
			dur = 4;
			mod2 = 140;//hapkow
		}
	}

	hp_percent = 100 * victim->hit / UMAX( get_max_hp(victim), 1 );
	mod += get_curr_stat_deprecated( ch, STAT_INT ) - 22;
	mod2 += 2*(get_curr_stat_deprecated( ch, STAT_INT ) - 15);

	if ( victim->level + 10 < level )
	{
		mod += ( level - victim->level - 10 )*2;
		mod2 += ( level - victim->level - 10 )*6;
	}

	if ( IS_AFFECTED( victim, AFF_BLIND ) || IS_AFFECTED(victim,AFF_PERFECT_SENSES ) || ( hp_percent > mod && victim->hit > mod2 ) )
	{
		send_to_char( "Zaklêcie siê uda³o, ale nic siê nie dzieje.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.duration = dur;
	af.rt_duration = 0;
	af.bitvector = &AFF_BLIND;
	affect_to_char( victim, &af, NULL, TRUE );

	if ( victim->sex == SEX_NEUTRAL )
	{
		send_to_char( "Zosta³o¶ o¶lepione.\n\r", victim );
		act( "$n wygl±da na o¶lepione.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( victim->sex == SEX_MALE )
	{
		send_to_char( "Zosta³e¶ o¶lepiony.\n\r", victim );
		act( "$n wygl±da na o¶lepionego.", victim, NULL, NULL, TO_ROOM );
	}
	else
	{
		send_to_char( "Zosta³a¶ o¶lepiona.\n\r", victim );
		act( "$n wygl±da na o¶lepion±.", victim, NULL, NULL, TO_ROOM );
	}
	return;
}

void spell_power_word_kill( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int hp_percent, luck, vluck, mod, mod2;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	mod = 24;//ledwo stoi i nizej
	mod2 = 70;//hapkow

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			mod = 32;//polowa bardzo ciezkich ran
			mod2 = 100;//hapkow
		}
	}

	hp_percent = 100 * victim->hit / UMAX( 1, get_max_hp(victim) );
	mod += 2*(get_curr_stat_deprecated( ch, STAT_INT ) - 22)/3;
	mod2 += (3*(get_curr_stat_deprecated( ch, STAT_INT ) - 15))/2;

	if ( victim->level + 10 < level )
	{
		mod += ( level - victim->level - 10 );
		mod2 += ( level - victim->level - 10 )*3;
	}

	if ( ( hp_percent > mod && victim->hit > mod2 ) || ( !IS_NPC( victim ) && IS_IMMORTAL( victim ) ) )
	{
		send_to_char( "Zaklêcie siê uda³o, ale nic siê nie dzieje.\n\r", ch );
		return;
	}

	act( "Twoja magia pozbawia $C ¿ycia!", ch, NULL, victim, TO_CHAR );
	act( "Magia $z pozbawia ¿ycia $C!", ch, NULL, victim, TO_NOTVICT );

	if ( !IS_NPC( victim ) )
	{
		act( "Magia $z pozbawia ciê ¿ycia!", ch, NULL, victim, TO_VICT );
	}

	//dla bezpieczenstwa zadaje 20 + maks hp ofiary, raw_kill jak bylo to sie nei logowalo
	raw_damage( ch, victim, 20 + get_max_hp(victim) );
	return;
}

void spell_power_word_stun( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int hp_percent, luck, vluck, mod, mod2;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	mod = 39;//b ciezkie
	mod2 = 100;//hapkow

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			mod = 54;//ciezkie
			mod2 = 140;//hapkow
		}
	}

	hp_percent = 100 * victim->hit / UMAX( get_max_hp(victim), 1 );
	mod += get_curr_stat_deprecated( ch, STAT_INT ) - 22;
	mod2 += 2*(get_curr_stat_deprecated( ch, STAT_INT ) - 15);

	if ( victim->level + 10 < level )
	{
		mod += ( level - victim->level - 10 )*2;
		mod2 += ( level - victim->level - 10 )*6;
	}

	if ( ( hp_percent > mod && victim->hit > mod2 ) || ( !IS_NPC( victim ) && IS_IMMORTAL( victim ) ) )
	{
		send_to_char( "Zaklêcie siê uda³o, ale nic siê nie dzieje.\n\r", ch );
		return;
	}

	DAZE_STATE( victim, number_range(level*2,level*4) );

	act( "Twoja magia og³usza $C!", ch, NULL, victim, TO_CHAR );
	act( "Magia $z og³usza $C!", ch, NULL, victim, TO_NOTVICT );

	if ( !IS_NPC( victim ) )
	{
		act( "Magia $n oglusza cie!", ch, NULL, victim, TO_VICT );
	}

	return;
}

void spell_teleport( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	char *tar = ( char * ) vo;
	int luck;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( !tar || tar[ 0 ] == '\0' )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	victim = get_char_world( ch, tar );

	if ( !victim ||
	     ch->in_room == NULL ||
	     EXT_IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL ) ||
	     victim->in_room == NULL ||
	     IS_NPC( victim ) ||
	     victim->level >= LEVEL_IMMORTAL ||
	     EXT_IS_SET( victim->in_room->room_flags, ROOM_NOMAGIC ) ||
	     EXT_IS_SET( victim->in_room->room_flags, ROOM_PRIVATE ) ||
	     EXT_IS_SET( victim->in_room->room_flags, ROOM_SOLITARY ) ||
	     EXT_IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL ) )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	send_to_char( "Twoje otoczenie rozmywa siê i przybiera nowe kszta³ty!\n\r", ch );
	act( "$n znika!", ch, NULL, NULL, TO_ROOM );
	char_from_room( ch );
	char_to_room( ch, victim->in_room );
	act( "$n materializuje siê powoli.", ch, NULL, NULL, TO_ROOM );
	do_function( ch, &do_look, "auto" );
	return;
}

void spell_dimension_door( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{

	CHAR_DATA * victim;
	char *tar = ( char * ) vo;
	int luck;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( !tar || tar[ 0 ] == '\0' )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	victim = get_char_area( ch, target_name );

	if ( !victim )
	{
		send_to_char( "Nie uda³o ci siê odnale¼æ celu.\n\r", ch );
		return;
	}

	if ( ch == victim )
	{
		send_to_char( "Przed tob± pojawia siê migocz±ca brama, a po chwili, w rozb³ysku ¶wiat³a, znika.\n\r", ch );
		act( "Pojawia siê tutaj migocz±ca brama, a po chwili, w rozb³ysku ¶wiat³a, znika!", ch, NULL, NULL, TO_ROOM );
		return;
	}

	if ( ch->in_room == NULL ||
	     EXT_IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL ) ||
	     victim->in_room == NULL ||
	     IS_NPC( victim ) ||
	     ( !IS_NPC( victim ) && victim->level >= LEVEL_IMMORTAL ) ||
	     EXT_IS_SET( victim->in_room->room_flags, ROOM_NOMAGIC ) ||
	     EXT_IS_SET( victim->in_room->room_flags, ROOM_PRIVATE ) ||
	     EXT_IS_SET( victim->in_room->room_flags, ROOM_SOLITARY ) ||
	     EXT_IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL ) )
	{
		send_to_char( "Nie uda³o siê.\n\r", ch );
		return;
	}

	send_to_char( "Przed tob± pojawia siê migocz±ca brama, przechodzisz przez ni±.\n\r", ch );
	act( "Pojawia siê tutaj migocz±ca brama, $n przechodzi przez ni± i znika w rozb³ysku ¶wiat³a!", ch, NULL, NULL, TO_ROOM );
	char_from_room( ch );
	char_to_room( ch, victim->in_room );
	act( "Przed tob± pojawia siê migocz±ca brama, w rozb³ysku ¶wiat³a wychodzi z niej $n.", ch, NULL, NULL, TO_ROOM );
	do_function( ch, &do_look, "auto" );
	return;
}

void spell_vampiric_touch( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, drained;
	int luck, vluck;

	if ( is_undead(victim) && is_undead(ch) )
    {
		return;
    }

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		act( "Nie uda³o ci siê wyssaæ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

    // wy³aczamy dzia³anie je¿eli jest siê otoczonym polem energi
    // FS#4206 - nie wiem czy to bug ale jak ma sie energy shielda i walnie s...
	if ( IS_AFFECTED( ch, AFF_ENERGY_SHIELD ) )
	{
        send_to_char( "Nic siê nie udaje, tylko krêci ci siê trochê w g³owie.\n\r", ch );
        return;
    }

	if ( IS_NPC( ch ) )
	{
		dam = dice( 4, 5 ) + dice( level / 2, 10 );
		drained = UMIN( dam, victim->hit + 11 );
		drained = number_range( 65, 95 ) * drained / 100;
	}
	else if ( ch->pcdata->mage_specialist == 7 )
	{
		dam = dice( 3, 5 ) + dice( level / 2, 8 );
		drained = UMIN( dam, victim->hit + 11 );
		drained = number_range( 65, 95 ) * drained / 100;
	}
	else
	{
		dam = dice( 2, 5 ) + dice( level / 2, 7 );
		drained = UMIN( dam, victim->hit + 11 );
		drained = number_range( 30, 60 ) * drained / 100;
	}

	dam = luck_dam_mod( ch, dam );

	if ( is_undead(victim) )
	{//poprawka - dla vampirica w undeady sie wszystko odwraca
		ch->hit = URANGE( -10, ch->hit - drained, 125 * get_max_hp(ch) / 100 );
		heal_undead( ch, victim, (2*dam)/3 );
		return;
	}

	if ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 )
	{
		if ( spell_damage( ch, victim, dam, sn, DAM_NEGATIVE, FALSE ) )
			ch->hit = UMIN( ch->hit + drained, 125 * get_max_hp(ch) / 100 );
	}
	else
	{
		if ( spell_damage( ch, victim, dam, sn, DAM_NEGATIVE, TRUE ) )
			ch->hit = URANGE( -10, ch->hit + drained, 125 * get_max_hp(ch) / 100 );
	}

	return;
}

void spell_feeblemind( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    //nie dla graczy
    if ( !IS_NPC( ch ) || !IS_SET( race_table[ GET_RACE( victim ) ].type, PERSON ) )
    {
        return;
    }
    if ( is_affected( victim, sn ) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
    {
        return;
    }
    if ( victim->resists[ RESIST_MENTAL ] > 0 && number_percent() < victim->resists[ RESIST_MENTAL ] )
    {
        return;
    }
    af.where       = TO_AFFECTS;
    af.type        = sn;
    af.level       = level;
    af.duration    = -1;
    af.rt_duration = 0;
    af.location    = APPLY_INT;
    af.modifier    = -get_curr_stat( victim, STAT_INT );
    af.bitvector   = &AFF_NONE;
    affect_to_char( victim, &af, NULL, TRUE );
    send_to_char( "Nagle twoj umys³ okrywa mg³a...\n\r", victim );
    act( "$n staje nagle i rozgl±da siê woko³o z g³upkowatym wyrazem twarzy.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_animate_dead( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo, *item, *item_next;
	CHAR_DATA *skelet;
	MOB_INDEX_DATA *mob;
	AFFECT_DATA af;
	char buf[ MAX_INPUT_LENGTH ];
	int i, made_dur = 5;

	if ( obj->item_type != ITEM_CORPSE_NPC ||
	     obj->value[ 0 ] <= 0 ||
	     ( mob = get_mob_index( obj->value[ 0 ] ) ) == NULL )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	if ( !IS_SET( race_table[ mob->race ].parts, PART_BONES ) ||
	     mob->size < SIZE_SMALL ||
	     mob->size > SIZE_LARGE ||
	     ( !IS_SET( race_table[ mob->race ].type, PERSON ) &&
	       !IS_SET( race_table[ mob->race ].type, ANIMAL ) ) )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	if ( is_affected(ch,gsn_made_undead) )
	{
		send_to_char( "Straci³<&e/a/o>¶ koncentracje, musisz chwilê poczekaæ zanim ponownie stworzysz martwiaka.\n\r", ch );
		return;
	}

	if ( number_range( 1,1200 ) == 1 )
	{
		extract_obj( obj );
		summon_malfunction( ch, sn );
		return;
	}

	obj->value[ 0 ] = 0;


        //Sprawdzanie czy posiadamy komponent (jakikolwiek)
        bool have_component = spell_item_check( ch, sn , NULL );
        if ( have_component && spell_items_table[ active_spell_items.ind[1] ].key_number == 0 )
        {
                //b³±d na tym etapie wskazuje na puste miejsce w tabeli
                have_component = FALSE;
        }

      if ( have_component )
      {

            switch ( spell_items_table[ active_spell_items.ind[1] ].key_number )
         {
            case 42://Czaszka licza
               if ( ( skelet = create_mobile( get_mob_index( MOB_VNUM_SKELET7 ) ) ) == NULL )
            {
               print_char( ch, "Nic siê nie sta³o.\n\r" );
               sprintf( buf, "spell_animate_dead, brak moba[8]: [%5d].", MOB_VNUM_SKELET6 );
               log_string( buf );
               return;
            }
            break;
            case 43://K³y wampira
               if ( ( skelet = create_mobile( get_mob_index( MOB_VNUM_SKELET8 ) ) ) == NULL )
            {
               print_char( ch, "Nic siê nie sta³o.\n\r" );
               sprintf( buf, "spell_animate_dead, brak moba[9]: [%5d].", MOB_VNUM_SKELET6 );
               log_string( buf );
               return;
            }
            break;
            case 44://Banda¿e mumii
               if ( ( skelet = create_mobile( get_mob_index( MOB_VNUM_SKELET9 ) ) ) == NULL )
            {
               print_char( ch, "Nic siê nie sta³o.\n\r" );
               sprintf( buf, "spell_animate_dead, brak moba[10]: [%5d].", MOB_VNUM_SKELET6 );
               log_string( buf );
               return;
            }
            break;
            case 45://Upiorny py³
               if ( ( skelet = create_mobile( get_mob_index( MOB_VNUM_SKELET10 ) ) ) == NULL )
            {
               print_char( ch, "Nic siê nie sta³o.\n\r" );
               sprintf( buf, "spell_animate_dead, brak moba[11]: [%5d].", MOB_VNUM_SKELET6 );
               log_string( buf );
               return;
            }
            break;
         }
      }
//Je¿eli nie to ³adujemy normalnego moba.
   else
         {
            switch ( number_range( 1, 7 ) )
         {
            case 1:
               if ( ( skelet = create_mobile( get_mob_index( MOB_VNUM_SKELET ) ) ) == NULL )
            {
               print_char( ch, "Nic siê nie sta³o.\n\r" );
               sprintf( buf, "spell_animate_dead, brak moba[1]: [%5d].", MOB_VNUM_SKELET );
               log_string( buf );
               return;
            }

            /* making corpse */
            /* name */
            sprintf( buf, skelet->name, race_table[ mob->race ].name );
            free_string( skelet->name );
            skelet->name = str_dup( buf );

            /* short */
            sprintf( buf, skelet->short_descr, race_table[ mob->race ].name );
            free_string( skelet->short_descr );
            skelet->short_descr = str_dup( buf );

            /* long */
            sprintf( buf, skelet->long_descr, race_table[ mob->race ].name );
            free_string( skelet->long_descr );
            skelet->long_descr = str_dup( buf );
            break;
            case 2:
               if ( ( skelet = create_mobile( get_mob_index( MOB_VNUM_SKELET1 ) ) ) == NULL )
            {
               print_char( ch, "Nic siê nie sta³o.\n\r" );
               sprintf( buf, "spell_animate_dead, brak moba[2]: [%5d].", MOB_VNUM_SKELET1 );
               log_string( buf );
               return;
            }
            break;
            case 3:
               if ( ( skelet = create_mobile( get_mob_index( MOB_VNUM_SKELET2 ) ) ) == NULL )
            {
               print_char( ch, "Nic siê nie sta³o.\n\r" );
               sprintf( buf, "spell_animate_dead, brak moba[3]: [%5d].", MOB_VNUM_SKELET2 );
               log_string( buf );
               return;
            }
            break;
            case 4:
               if ( ( skelet = create_mobile( get_mob_index( MOB_VNUM_SKELET3 ) ) ) == NULL )
            {
               print_char( ch, "Nic siê nie sta³o.\n\r" );
               sprintf( buf, "spell_animate_dead, brak moba[4]: [%5d].", MOB_VNUM_SKELET3 );
               log_string( buf );
               return;
            }
            break;
            case 5:
            if ( ( skelet = create_mobile( get_mob_index( MOB_VNUM_SKELET4 ) ) ) == NULL )
            {
               print_char( ch, "Nic siê nie sta³o.\n\r" );
               sprintf( buf, "spell_animate_dead, brak moba[5]: [%5d].", MOB_VNUM_SKELET4 );
               log_string( buf );
               return;
            }
            break;
            case 6:
            if ( ( skelet = create_mobile( get_mob_index( MOB_VNUM_SKELET5 ) ) ) == NULL )
            {
               print_char( ch, "Nic siê nie sta³o.\n\r" );
               sprintf( buf, "spell_animate_dead, brak moba[6]: [%5d].", MOB_VNUM_SKELET5 );
               log_string( buf );
               return;
            }
            break;
            case 7:
               if ( ( skelet = create_mobile( get_mob_index( MOB_VNUM_SKELET6 ) ) ) == NULL )
            {
               print_char( ch, "Nic siê nie sta³o.\n\r" );
               sprintf( buf, "spell_animate_dead, brak moba[7]: [%5d].", MOB_VNUM_SKELET6 );
               log_string( buf );
               return;
            }
            break;
         }
      }


//Ustawianie statystyk przyzywanej istoty.

	skelet->spec_fun = NULL;
    money_reset_character_money( skelet );

	skelet->level = URANGE( 6, number_range( level / 2, 2 * level / 3 ), 15 );

	/* HP */
	for ( skelet->hit = 0, i = skelet->level; i >= 1; i-- )
		skelet->hit += 12 + number_range( 1, 5 );

	skelet->hit += level;
	skelet->max_hit = skelet->hit;
	/* STATS */
    skelet->perm_stat[ STAT_STR ] = number_range( 60,  90 );
	skelet->perm_stat[ STAT_DEX ] = number_range( 60,  96 );
	skelet->perm_stat[ STAT_WIS ] = number_range( 72,  96 );
	skelet->perm_stat[ STAT_INT ] = number_range( 20,  40 );
	skelet->perm_stat[ STAT_CON ] = number_range( 80, 100 );
    skelet->perm_stat[ STAT_CHA ] = number_range( 20,  40 );
    skelet->perm_stat[ STAT_LUC ] = number_range(  1, 100 );

	skelet->size = mob->size;

	skelet->hit = get_max_hp(skelet);

	switch ( skelet->size )
	{
		case SIZE_MEDIUM:
			skelet->weight = number_range( 650, 1200 ) / 2;
			skelet->height = number_range( 110, 210 );
			skelet->damage[ DICE_NUMBER ] = 2;
			skelet->damage[ DICE_TYPE ] = 5;
			made_dur = level > 25 ? 2 : 4;
			break;
		case SIZE_LARGE:
			skelet->weight = number_range( 850, 2200 ) / 2;
			skelet->height = number_range( 210, 260 );
			skelet->damage[ DICE_NUMBER ] = 2;
			skelet->damage[ DICE_TYPE ] = 6;
			made_dur = level > 25 ? 3 : 6;
			break;
		case SIZE_SMALL:
			skelet->weight = number_range( 250, 450 ) / 2;
			skelet->height = number_range( 40, 100 );
			skelet->damage[ DICE_NUMBER ] = 2;
			skelet->damage[ DICE_TYPE ] = 4;
			made_dur = level > 25 ? 1 : 3;
			break;

		default: break;
	}

	skelet->damroll += URANGE( 0, level / 7.5, 3 );

	for ( i = 0; i < 4; i++ )
		skelet->armor[ i ] = 100 - 10 * ( skelet->level / 2 );

    if ( !IS_NPC( ch ) && ch->pcdata->mage_specialist == 7 )
    {
        skelet->level += 2;
        skelet->hit += dice( skelet->level, 4 );
        skelet->perm_stat[ STAT_STR ] += number_range( 6, 24 );
        skelet->damroll += number_range( 1, 2 );
        if (have_component)
        {
            switch ( spell_items_table[ active_spell_items.ind[1] ].key_number )
            {
                case 42: //U¿ywaj±c czaszki licza przywo³ujemy szkieleta-czarodzieja, vnum moba: 3330
                    skelet->perm_stat[ STAT_INT ] += number_range(60, 100);
                    EXT_SET_BIT( skelet->act, ACT_MAGE );
                    break;
                case 43: //U¿ywaj±c k³ów wampira przywo³ujemy szkieleta-czarnego rycerza, vnum moba: 3331
                    skelet->hit += dice( skelet->level, 2 );
                    skelet->perm_stat[ STAT_STR ] += number_range( 1, 12 );
                    skelet->hitroll += 1;
                    EXT_SET_BIT( skelet->act, ACT_BLACK_KNIGHT );
                    EXT_SET_BIT( skelet->off_flags, OFF_SMITE_GOOD );
                    break;
                case 44: //U¿ywaj±c banda¿y mumii przywo³ujemy szkieleta-kap³ana, vnum moba: 3332
                    skelet->hit += skelet->level ;
                    skelet->perm_stat[ STAT_WIS ] += number_range( 10, 20 );
                    EXT_SET_BIT( skelet->act, ACT_CLERIC );
                    break;
                case 45: //U¿ywaj±c upiornego py³u przywo³ujemy szkieleta-wojownika, vnum moba: 3333
                    skelet->hit += skelet->level;
                    skelet->perm_stat[ STAT_STR ] += number_range(1,10);
                    skelet->hitroll += 1;
                    EXT_SET_BIT( skelet->act, ACT_WARRIOR );
                    EXT_SET_BIT( skelet->off_flags, OFF_RESCUE );
                    break;
            }
        }
    }
/*Drake:
  spell_items_table[ active_spell_items.komponenty[0] - zwraca ostatnio u¿yty komponent, a dalsza ci±g
  sprawdza czy zgadza siê z numerem spellowo-komponentowym czaru. Chyba. xD */
  else
  {
      if (have_component)
      {
          switch ( spell_items_table[ active_spell_items.ind[1] ].key_number )
          {
              case 42: //U¿ywaj±c czaszki licza przywo³ujemy szkieleta-czarodzieja, vnum moba: 3330
                  skelet->perm_stat[ STAT_INT ] += number_range(60, 90);
                  EXT_SET_BIT( skelet->act, ACT_MAGE );
                  break;
              case 43: //U¿ywaj±c k³ów wampira przywo³ujemy szkieleta-czarnego rycerza, vnum moba: 3331
                  skelet->level += 1;
                  skelet->hit += dice( skelet->level, 2 );
                  skelet->perm_stat[ STAT_STR ] += number_range( 6, 12 );
                  skelet->damroll += 1;
                  skelet->hitroll += 1;
                  EXT_SET_BIT( skelet->act, ACT_BLACK_KNIGHT );
                  EXT_SET_BIT( skelet->off_flags, OFF_SMITE_GOOD );
                  break;
              case 44: //U¿ywaj±c banda¿y mumii przywo³ujemy szkieleta-kap³ana, vnum moba: 3332
                  skelet->level += 1;
                  skelet->hit += skelet->level ;
                  skelet->perm_stat[ STAT_WIS ] += number_range( 12, 24 );
                  EXT_SET_BIT( skelet->act, ACT_CLERIC );
                  break;
              case 45: //U¿ywaj±c upiornego py³u przywo³ujemy szkieleta-wojownika, vnum moba: 3333
                  skelet->level += 1;
                  skelet->hit += dice( skelet->level, 2 );
                  skelet->perm_stat[ STAT_STR ] += number_range( 6, 12 );
                  skelet->damroll += 1;
                  skelet->hitroll += 1;
                  EXT_SET_BIT( skelet->act, ACT_WARRIOR );
                  EXT_SET_BIT( skelet->off_flags, OFF_RESCUE );
                  break;
          }
      }
  }

	char_to_room( skelet, ch->in_room );
	skelet->real_race = mob->race;
	EXT_SET_BIT( skelet->act, ACT_RAISED );
	EXT_SET_BIT( skelet->act, ACT_NO_EXP );

	act("$n powstaje z $f.", skelet, obj, NULL, TO_ROOM );

	for ( item = obj->contains; item != NULL; item = item_next )
	{
		OBJ_NEXT_CONTENT( item, item_next );
	    obj_from_obj( item );
	    obj_to_char( item, skelet );
	}

	for ( item = skelet->carrying; item != NULL; item = item->next_content )
	{
		OBJ_NEXT( item, item_next );
		if ( item->item_type == ITEM_WEAPON )
			wield_weapon_silent( skelet, item, TRUE );
		else
	    	wear_obj_silent( skelet, item, FALSE );
	}

	extract_obj(obj);

	if ( !add_charm( ch, skelet, TRUE ) )
	{
		EXT_SET_BIT( skelet->act, ACT_AGGRESSIVE );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_domination;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( skelet, &af, NULL, TRUE );

	af.where = TO_AFFECTS;
	af.type = gsn_made_undead;
	af.level = 50;//by sie nie dalo dispelnac
	af.duration = made_dur;
	af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	af.visible = FALSE;
	affect_to_char( ch, &af, NULL, FALSE );

	add_follower( skelet, ch, TRUE );

	if ( ch->fighting )
		set_fighting( skelet, ch->fighting );

	return;
}

void spell_create_lesser_undead( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo, *item, *item_next;
	CHAR_DATA *zombie;
	MOB_INDEX_DATA *mob;
	AFFECT_DATA af;
	char buf[ MAX_INPUT_LENGTH ];
	int i, made_dur = 5;

	if ( obj->item_type != ITEM_CORPSE_NPC ||
	     obj->value[ 0 ] <= 0 ||
	     ( mob = get_mob_index( obj->value[ 0 ] ) ) == NULL )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	if ( mob->size < SIZE_SMALL ||
	     mob->size > SIZE_LARGE ||
	     !IS_SET( race_table[ mob->race ].type, PERSON ) )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	if ( is_affected(ch,gsn_made_undead) )
	{
		switch (ch->sex)
		{
			case SEX_NEUTRAL:
				send_to_char( "Straci³o¶ koncentracje, musisz chwilê poczekaæ zanim ponownie stworzysz martwiaka.\n\r", ch );
				break;
			case SEX_MALE:
				send_to_char( "Straci³e¶ koncentracje, musisz chwilê poczekaæ zanim ponownie stworzysz martwiaka.\n\r", ch );
				break;
			case SEX_FEMALE:
			default:
				send_to_char( "Straci³a¶ koncentracje, musisz chwilê poczekaæ zanim ponownie stworzysz martwiaka.\n\r", ch );
				break;
		}
		return;
	}

	if ( !spell_item_check( ch, sn, NULL ) )
	{
		print_char( ch, "Cia³u które chcia³by¶ o¿ywiæ brakuje jeszcze ma³ej czê¶ci.\n\r" );
		return;
	}

	if ( number_range( 1,1000 ) == 1 )
	{
		extract_obj( obj );
		summon_malfunction( ch, sn );
		return;
	}

	obj->value[ 0 ] = 0;

	switch ( number_range( 1, 7 ) )
	{
		case 1:
			if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE ) ) ) == NULL )
			{
				print_char( ch, "Nic siê nie sta³o.\n\r" );
				return;
			}
			/* making corpse */
			/* name */
			sprintf( buf, zombie->name, race_table[ mob->race ].name );
			free_string( zombie->name );
			zombie->name = str_dup( buf );

			/* short */
			sprintf( buf, zombie->short_descr, race_table[ mob->race ].name );
			free_string( zombie->short_descr );
			zombie->short_descr = str_dup( buf );

			/* long */
			sprintf( buf, zombie->long_descr, race_table[ mob->race ].name );
			free_string( zombie->long_descr );
			zombie->long_descr = str_dup( buf );
			break;
		case 2:
			if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE1 ) ) ) == NULL )
			{
				print_char( ch, "Nic siê nie sta³o.\n\r" );
				return;
			}
			break;
		case 3:
			if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE2 ) ) ) == NULL )
			{
				print_char( ch, "Nic siê nie sta³o.\n\r" );
				return;
			}
			break;
		case 4:
			if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE3 ) ) ) == NULL )
			{
				print_char( ch, "Nic siê nie sta³o.\n\r" );
				return;
			}
			break;
		case 5:
			if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE4 ) ) ) == NULL )
			{
				print_char( ch, "Nic siê nie sta³o.\n\r" );
				return;
			}
			break;
		case 6:
			if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE5 ) ) ) == NULL )
			{
				print_char( ch, "Nic siê nie sta³o.\n\r" );
				return;
			}
			break;
		case 7:
			if ( ( zombie = create_mobile( get_mob_index( MOB_VNUM_ZOMBIE6 ) ) ) == NULL )
			{
				print_char( ch, "Nic siê nie sta³o.\n\r" );
				return;
			}
			break;
	}

	zombie->spec_fun = NULL;
    money_reset_character_money( zombie );

	zombie->level = URANGE( 8, number_range( level / 2, 2 * level / 3 ), 16 );

	/* HP */
	for ( zombie->hit = 0, i = zombie->level; i >= 1; i-- )
		zombie->hit += 16 + number_range( 1, 6 );

	zombie->hit += level;
	zombie->max_hit = zombie->hit;
	/* STATS */
	zombie->perm_stat[ STAT_STR ] = number_range(  60,  80 );
	zombie->perm_stat[ STAT_DEX ] = number_range(  60, 100 );
	zombie->perm_stat[ STAT_WIS ] = number_range(  60, 100 );
	zombie->perm_stat[ STAT_INT ] = number_range(  20,  40 );
	zombie->perm_stat[ STAT_CON ] = number_range( 100, 130 );
    zombie->perm_stat[ STAT_CHA ] = number_range(  20,  40 );
    zombie->perm_stat[ STAT_LUC ] = number_range(   1, 100 );

	zombie->size = mob->size;

	zombie->hit = get_max_hp(zombie);

	//bonus dla nekow
	if ( !IS_NPC( ch ) && ch->pcdata->mage_specialist == 7 )
	{
		zombie->level += 2;
		zombie->hit += dice( zombie->level, 4 );
		zombie->perm_stat[ STAT_STR ] += number_range( 5, 15 );
		zombie->damroll += number_range( 1, 2 );
		EXT_SET_BIT( zombie->act, ACT_WARRIOR );
	}

	switch ( zombie->size )
	{
		case SIZE_MEDIUM:
			zombie->weight = number_range( 650, 1200 );
			zombie->height = number_range( 140, 210 );
			zombie->damage[ DICE_NUMBER ] = 2;
			zombie->damage[ DICE_TYPE ] = 5;
			made_dur = level > 25 ? 2 : 4;
			break;
		case SIZE_LARGE:
			zombie->weight = number_range( 850, 2200 );
			zombie->height = number_range( 210, 260 );
			zombie->damage[ DICE_NUMBER ] = 2;
			zombie->damage[ DICE_TYPE ] = 6;
			made_dur = level > 25 ? 3 : 6;
			break;
		case SIZE_SMALL:
			zombie->weight = number_range( 250, 450 );
			zombie->height = number_range( 40, 100 );
			zombie->damage[ DICE_NUMBER ] = 2;
			zombie->damage[ DICE_TYPE ] = 4;
			made_dur = level > 25 ? 1 : 3;
			break;

		default: break;
	}

	zombie->damroll += URANGE( 0, level / 7.5, 3 );

	for ( i = 0; i < 4; i++ )
    {
		zombie->armor[ i ] = 100 - 10 * ( zombie->level / 3 );
    }

	char_to_room( zombie, ch->in_room );
	EXT_SET_BIT( zombie->act, ACT_RAISED );
	EXT_SET_BIT( zombie->act, ACT_NO_EXP );

	act("Nagle $p powstaje.", zombie, obj, NULL, TO_ROOM );

	for ( item = obj->contains; item != NULL; item = item_next )
	{
		OBJ_NEXT_CONTENT( item, item_next );
	    obj_from_obj( item );
	    obj_to_char( item, zombie );
	}

	for ( item = zombie->carrying; item != NULL; item = item->next_content )
	{
		OBJ_NEXT( item, item_next );
		if ( item->item_type == ITEM_WEAPON )
			wield_weapon_silent( zombie, item, TRUE );
		else
	    	wear_obj_silent( zombie, item, FALSE );
	}

	extract_obj(obj);

	if ( !add_charm( ch, zombie, FALSE ) )
	{
		EXT_SET_BIT( zombie->act, ACT_AGGRESSIVE );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_domination;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( zombie, &af, NULL, TRUE );

	af.where = TO_AFFECTS;
	af.type = gsn_made_undead;
	af.level = 50;//by sie nie dalo dispelnac
	af.duration = made_dur;
	af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	af.visible = FALSE;
	affect_to_char( ch, &af, NULL, FALSE );

	add_follower( zombie, ch, TRUE );

	return;
}

void spell_raise_ghul( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo, *item, *item_next;
	CHAR_DATA *ghul;
	MOB_INDEX_DATA *mob = NULL;
	AFFECT_DATA af;
	int i, made_dur = 5;

	if ( obj->item_type != ITEM_CORPSE_NPC ||
	     obj->value[ 0 ] <= 0 ||
	     ( mob = get_mob_index( obj->value[ 0 ] ) ) == NULL )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	if ( mob->size < SIZE_SMALL ||
	     mob->size > SIZE_LARGE ||
	     !IS_SET( race_table[ mob->race ].type, PERSON ) )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	if ( is_affected(ch,gsn_made_undead) )
	{
		send_to_char( "Straci³<&e/a/o>¶ koncentracje, musisz chwilê poczekaæ zanim ponownie stworzysz martwiaka.\n\r", ch );
		return;
	}

	if ( !spell_item_check( ch, sn, NULL) )
	{
		send_to_char( "Do o¿ywienia ghula potrzebne bêdzie co¶ z jego cia³a.\n\r", ch );
		return;
	}

	if ( number_range( 1,800 ) == 1 )
	{
		extract_obj( obj );
		summon_malfunction( ch, sn );
		return;
	}

	obj->value[ 0 ] = 0;

	switch ( number_range( 1, 3 ) )
	{
		case 1:
			if ( ( mob = get_mob_index( MOB_VNUM_GHUL1 ) ) )
				break;
		case 2:
			if ( ( mob = get_mob_index( MOB_VNUM_GHUL2 ) ) )
				break;
		case 3:
			if ( ( mob = get_mob_index( MOB_VNUM_GHUL3 ) ) )
				break;
		default:
			if ( ( mob = get_mob_index( MOB_VNUM_GHUL1 ) ) )
				break;
			if ( ( mob = get_mob_index( MOB_VNUM_GHUL2 ) ) )
				break;
			if ( ( mob = get_mob_index( MOB_VNUM_GHUL3 ) ) )
				break;
	}

	if ( !mob )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	if ( ( ghul = create_mobile( mob ) ) == NULL )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	ghul->spec_fun = NULL;
    money_reset_character_money ( ghul );

	ghul->level = URANGE( 8, number_range( level / 2, 2 * level / 3 ), 17 );

	/* HP */
	for ( ghul->hit = 0, i = ghul->level; i >= 1; i-- )
		ghul->hit += 18 + number_range( 1, 6 );

	ghul->hit += level;
	ghul->max_hit = ghul->hit;

	/* STATS */
    ghul->perm_stat[ STAT_STR ] = number_range(  90, 140 );
	ghul->perm_stat[ STAT_DEX ] = number_range(  96, 126 );
	ghul->perm_stat[ STAT_WIS ] = number_range(  60, 100 );
	ghul->perm_stat[ STAT_INT ] = number_range(  69,  90 );
	ghul->perm_stat[ STAT_CON ] = number_range( 108, 132 );
    ghul->perm_stat[ STAT_CHA ] = number_range(  40,  60 );
    ghul->perm_stat[ STAT_LUC ] = number_range(   1,  50 );

	ghul->size = mob->size;

	ghul->hit = get_max_hp(ghul);

	//bonus dla nekow
	if ( !IS_NPC( ch ) && ch->pcdata->mage_specialist == 7 )
	{
		ghul->level += 2;
		ghul->hit += dice( ghul->level, 5 );
		ghul->perm_stat[ STAT_STR ] += number_range( 6, 18 );
		ghul->damroll += number_range( 1, 2 );
		EXT_SET_BIT( ghul->act, ACT_WARRIOR );
	}

	switch ( ghul->size )
	{
		case SIZE_MEDIUM:
			ghul->weight = number_range( 650, 1200 );
			ghul->height = number_range( 140, 210 );
			ghul->damage[ DICE_NUMBER ] = 2;
			ghul->damage[ DICE_TYPE ] = 5;
			made_dur = level > 25 ? 3 : 5;
			break;
		case SIZE_LARGE:
			ghul->weight = number_range( 850, 2200 );
			ghul->height = number_range( 210, 260 );
			ghul->damage[ DICE_NUMBER ] = 2;
			ghul->damage[ DICE_TYPE ] = 6;
			made_dur = level > 25 ? 4 : 7;
			break;
		case SIZE_SMALL:
			ghul->weight = number_range( 250, 450 );
			ghul->height = number_range( 40, 100 );
			ghul->damage[ DICE_NUMBER ] = 2;
			ghul->damage[ DICE_TYPE ] = 4;
			made_dur = level > 25 ? 2 : 4;
			break;

		default: break;
	}

	ghul->damroll += URANGE( 0, level / 5, 5 );

	//stale ac
	for ( i = 0; i < 4; i++ )
		ghul->armor[ i ] = 40;

	char_to_room( ghul, ch->in_room );
	EXT_SET_BIT( ghul->act, ACT_RAISED );
	EXT_SET_BIT( ghul->act, ACT_NO_EXP );

	act("Nagle $p powstaje.", ghul, obj, NULL, TO_ROOM );

	for ( item = obj->contains; item != NULL; item = item_next )
	{
		OBJ_NEXT_CONTENT( item, item_next );
	    obj_from_obj( item );
	    obj_to_char( item, ghul );
	}

	for ( item = ghul->carrying; item != NULL; item = item->next_content )
	{
		OBJ_NEXT( item, item_next );
		if ( item->item_type == ITEM_WEAPON )
			wield_weapon_silent( ghul, item, TRUE );
		else
	    	wear_obj_silent( ghul, item, FALSE );
	}

	extract_obj(obj);

	if ( !add_charm( ch, ghul, TRUE ) )
	{
		EXT_SET_BIT( ghul->act, ACT_AGGRESSIVE );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_domination;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( ghul, &af, NULL, TRUE );

	af.where = TO_AFFECTS;
	af.type = gsn_made_undead;
	af.level = 50;//by sie nie dalo dispelnac
	af.duration = made_dur;
	af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	af.visible = FALSE;
	affect_to_char( ch, &af, NULL, FALSE );

	add_follower( ghul, ch, TRUE );

	if ( ch->fighting )
		set_fighting( ghul, ch->fighting );

	return;
}

void spell_raise_ghast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo, *item, *item_next;
	CHAR_DATA *ghast;
	MOB_INDEX_DATA *mob = NULL;
	AFFECT_DATA af;
	int i, made_dur = 5;
	int mal_chance = 600;

	if ( obj->item_type != ITEM_CORPSE_NPC ||
	     obj->value[ 0 ] <= 0 ||
	     ( mob = get_mob_index( obj->value[ 0 ] ) ) == NULL )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	if ( mob->size < SIZE_SMALL ||
	     mob->size > SIZE_LARGE ||
	     !IS_SET( race_table[ mob->race ].type, PERSON ) )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	if ( is_affected(ch,gsn_made_undead) )
	{
		send_to_char( "Straci³<&e/a/o>¶ koncentracje, musisz chwilê poczekaæ zanim ponownie stworzysz martwiaka.\n\r", ch );
		return;
	}

	if ( !spell_item_check( ch, sn, NULL) )
	{
		if ( number_range(1,3) == 1)
		{
			send_to_char( "Do o¿ywienia ghasta potrzebne bêdzie co¶ z jego cia³a.\n\r", ch );
			return;
		}
		mal_chance = 60;
	}


	if ( number_range( 1, mal_chance ) == 1 )
	{
		extract_obj( obj );
		summon_malfunction( ch, sn );
		return;
	}

	obj->value[ 0 ] = 0;

	switch ( number_range( 1, 3 ) )
	{
		case 1:
			if ( ( mob = get_mob_index( MOB_VNUM_GHAST1 ) ) )
				break;
		case 2:
			if ( ( mob = get_mob_index( MOB_VNUM_GHAST2 ) ) )
				break;
		case 3:
			if ( ( mob = get_mob_index( MOB_VNUM_GHAST3 ) ) )
				break;
		default:
			if ( ( mob = get_mob_index( MOB_VNUM_GHAST1 ) ) )
				break;
			if ( ( mob = get_mob_index( MOB_VNUM_GHAST2 ) ) )
				break;
			if ( ( mob = get_mob_index( MOB_VNUM_GHAST3 ) ) )
				break;
	}

	if ( !mob )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	if ( ( ghast = create_mobile( mob ) ) == NULL )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	ghast->spec_fun = NULL;
    money_reset_character_money( ghast );

	ghast->level = URANGE( 8, number_range( level / 2, 2 * level / 3 ), 19 );

	/* HP */
	for ( ghast->hit = 0, i = ghast->level; i >= 1; i-- )
		ghast->hit += 19 + number_range( 2, 4 );

	ghast->hit += level;
	ghast->max_hit = ghast->hit;

	/* STATS */
    ghast->perm_stat[ STAT_STR ] = number_range( 100, 140 );
	ghast->perm_stat[ STAT_DEX ] = number_range(  96, 126 );
	ghast->perm_stat[ STAT_WIS ] = number_range(  60,  96 );
	ghast->perm_stat[ STAT_INT ] = number_range(  60,  90 );
	ghast->perm_stat[ STAT_CON ] = number_range( 108, 132 );
    ghast->perm_stat[ STAT_CHA ] = number_range(  20,  40 );
    ghast->perm_stat[ STAT_LUC ] = number_range(   1, 100 );

	ghast->size = mob->size;

	ghast->hit = get_max_hp(ghast);

	//bonus dla nekow
	if ( !IS_NPC( ch ) && ch->pcdata->mage_specialist == 7 )
	{
		ghast->level += 2;
		ghast->hit += dice( ghast->level, 4 );
		ghast->perm_stat[ STAT_STR ] += number_range( 6, 18 );
		ghast->damroll += number_range( 1, 2 );
		EXT_SET_BIT( ghast->act, ACT_WARRIOR );
	}

	switch ( ghast->size )
	{
		case SIZE_MEDIUM:
			ghast->weight = number_range( 650, 1200 );
			ghast->height = number_range( 140, 210 );
			ghast->damage[ DICE_NUMBER ] = 2;
			ghast->damage[ DICE_TYPE ] = 5;
			made_dur = level > 25 ? 3 : 5;
			break;
		case SIZE_LARGE:
			ghast->weight = number_range( 850, 2200 );
			ghast->height = number_range( 210, 260 );
			ghast->damage[ DICE_NUMBER ] = 2;
			ghast->damage[ DICE_TYPE ] = 6;
			made_dur = level > 25 ? 4 : 7;
			break;
		case SIZE_SMALL:
			ghast->weight = number_range( 250, 450 );
			ghast->height = number_range( 40, 100 );
			ghast->damage[ DICE_NUMBER ] = 2;
			ghast->damage[ DICE_TYPE ] = 4;
			made_dur = level > 25 ? 2 : 4;
			break;

		default: break;
	}

	ghast->damroll += URANGE( 0, level / 5, 7 );

	//stale ac
	for ( i = 0; i < 4; i++ )
		ghast->armor[ i ] = 30;

	char_to_room( ghast, ch->in_room );
	EXT_SET_BIT( ghast->act, ACT_RAISED );
	EXT_SET_BIT( ghast->act, ACT_NO_EXP );

	act("Nagle $p powstaje.", ghast, obj, NULL, TO_ROOM );

	for ( item = obj->contains; item != NULL; item = item_next )
	{
		OBJ_NEXT_CONTENT( item, item_next );
	    obj_from_obj( item );
	    obj_to_char( item, ghast );
	}

	for ( item = ghast->carrying; item != NULL; item = item->next_content )
	{
		if ( item->item_type == ITEM_WEAPON )
			wield_weapon_silent( ghast, item, TRUE );
		else
	    	wear_obj_silent( ghast, item, FALSE );
	}

	extract_obj(obj);

	if ( !add_charm( ch, ghast, TRUE ) )
	{
		EXT_SET_BIT( ghast->act, ACT_AGGRESSIVE );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_domination;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( ghast, &af, NULL, TRUE );

	af.where = TO_AFFECTS;
	af.type = gsn_made_undead;
	af.level = 50;//by sie nie dalo dispelnac
	af.duration = made_dur;
	af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	af.visible = FALSE;
	affect_to_char( ch, &af, NULL, FALSE );

	add_follower( ghast, ch, TRUE );

	if ( ch->fighting )
		set_fighting( ghast, ch->fighting );

	return;
}

void spell_orb_of_entropy( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, vluck, luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		send_to_char( "Czar nie zadzia³a³.\n\r", ch );
		return;
	}

	vluck = ( ch != victim ) ? get_curr_stat_deprecated( victim, STAT_LUC ) : luck;

	// modyfikator dla strasznego pecha
	if ( number_range( 0, luck + LUCK_BASE_MOD) == 0 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		spell_damage( ch, victim, 0, sn, DAM_NEGATIVE , FALSE );
		return;
	}

	if ( !spell_item_check(ch, sn, NULL) )
	{
		print_char( ch, "Ten czar wymaga przedmiotu z którego móg³by pobraæ energiê.\n\r" );
		return;
	}

	dam = number_range( 5, 14 ) * level / 2;
	dam = luck_dam_mod( ch, dam );

	//undeadzi wchlaniaja negativa
	if ( is_undead(victim) )
	{
		heal_undead( ch,victim, dam/2 );
		return;
	}

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

	if( IS_NPC( ch ) )//takie zabezpieczenie, by moby nie czarowaly tym zbyt gownianie
		dam = UMAX( number_range( 70, 90 ), dam );

	spell_damage( ch, victim, dam, sn, DAM_NEGATIVE , TRUE );
	return;
}

/* bron cienia (skladnia: cast 'shadow weapon')
 *
 * Wlasciwie to bedzie to samo, co spirit_hammer
 */

void spell_shadow_weapon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = NULL;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int chance = number_range( 1, 100 - 2 * level );

	// nie mistrzowie przywolania nie mogz miec szansy mniejszej niz 5,
	// z wyjatkiem tych co sa zli i maja jedna szanse na to zeby przywolac
	// ciut mocniejsza bron.
	//
	// specjalisci natomiast otrzymuja dodatkowe przesuniscie szansy oraz
	// bonus do obrazen
	if ( !IS_NPC( ch ) && ch->pcdata->mage_specialist != 2 )
	{
		chance = UMAX( 5, chance );
		if ( ch->alignment < -250 && ( number_range( 0, luck ) > 10 ) )
			chance--;
	}
	else chance -= level / 2;
	/*
	szansy przy parametrach w ifie liczone dla magow niespecjalistow
	level 30
	 2       2.50%  vnum: 3005 ale uwaga... tyl;ko zli niespecjalisci maga ten vnum
	 5      10.00%  vnum: 3004
	17      40.00%  vnum: 3003
	reszta 47.50%   vnum: 3002

	dla specjalistow
	 2       4.00%  vnum: 3005
	 5      16.00%  vnum: 3004
	17      64.00%  vnum: 3003
	reszta 16.50%   vnum: 3002
	 */

	if ( !get_eq_char( ch, WEAR_WIELD ) )
	{
/*		if ( chance < 2 ) Dispeller nie dziala
			obj = create_object( get_obj_index( OBJ_VNUM_SHADOW_SHORT_SWORD_DISPELLER ), FALSE );
		else*/ if ( chance < 5 )
			obj = create_object( get_obj_index( OBJ_VNUM_SHADOW_SHORT_SWORD_VORPAL ), FALSE );
		else if ( chance < 17 )
			obj = create_object( get_obj_index( OBJ_VNUM_SHADOW_SHORT_SWORD ), FALSE );
		else
			obj = create_object( get_obj_index( OBJ_VNUM_SHADOW_DAGGER ), FALSE );

		if ( !obj )
		{
			send_to_char( "Co¶ zaczyna b³yszczeæ, a potem wygasa bez ¶ladu.\n\r", ch );
			bug( "Brak obiektu: OBJ_VNUM_SHADOW_XXXXXXX", 0 );
			return;
		}

		obj->timer = URANGE( 3, 1 + dice( 1, 3 ) + level / 10, 6 ) + ( ( number_range( 0, luck ) > 10 ) ? 1 : 0 ) + ( ( !IS_NPC( ch ) && ch->pcdata->mage_specialist == 2 ) ? 2 : 0 );
		obj->rent_cost = ( ( !IS_NPC( ch ) && ch->pcdata->mage_specialist == 2 ) ? 1 : 2 ) * level;

		obj->value[ 1 ] = 2 + ( ( number_range( 0, get_curr_stat_deprecated(ch,STAT_INT) ) > 10 ) ? 1 : 0 );
		obj->value[ 2 ] = 4 + ( ( number_range( 0, luck ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, get_curr_stat_deprecated(ch,STAT_INT) ) > 14 ) ? 1 : 0 ) + ( ( !IS_NPC( ch ) && ch->pcdata->mage_specialist == 2 ) ? 2 : 0 );
		obj->value[ 5 ] = URANGE( 1, level / 6, 4 ) + ( ( number_range( 0, luck ) > 10 ) ? 1 : 0 ) + ( ( !IS_NPC( ch ) && ch->pcdata->mage_specialist == 2 ) ? 1 : 0 );
		obj->value[ 6 ] = URANGE( 1, level / 6, 4 ) + ( ( number_range( 0, luck ) > 10 ) ? 1 : 0 ) + ( ( !IS_NPC( ch ) && ch->pcdata->mage_specialist == 2 ) ? 1 : 0 );

		obj_to_char( obj, ch );
		equip_char( ch, obj, WEAR_WIELD, TRUE );
		act( "W rêku $z pojawia siê ostrze z cienia.", ch, NULL, NULL, TO_ROOM );
		send_to_char( "W twej rêce pojawia siê ostrze z cienia.\n\r", ch );
		return;
	}
	send_to_char( "Przecie¿ trzymasz ju¿ co¶ w rêce.\n\r", ch );
	return;
}

void spell_might( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration = URANGE( 5, level / 2, 11 );
	int location, type, mod = 1;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( is_affected( victim, sn ) )
	{
		if ( victim == ch )
		{
			switch ( victim->sex )
			{
				case 0:
					send_to_char( "Nie mo¿esz byæ bardziej pobudzone.\n\r", ch );
					break;
				case 1:
					send_to_char( "Nie mo¿esz byæ bardziej pobudzony.\n\r", ch );
					break;
				default :
					send_to_char( "Nie mo¿esz byæ bardziej pobudzona.\n\r", ch );
					break;
			}
			return;
		}
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie mo¿e byæ ju¿ bardziej pobudzone.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N nie mo¿e byæ ju¿ bardziej pobudzony.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo¿e byæ ju¿ bardziej pobudzona.", ch, NULL, victim, TO_CHAR );
					break;
			}
			return;
		}
	}

	type = sn;

	switch ( number_range(1,7) )
	{
		case 1:
			if ( is_affected( victim, 167 /*2 kregowy strenght*/ ) || is_affected( victim, 58 /*8 kregowy giant strength*/ ) )
			{
				location = APPLY_STR;
				type = 167; //strenght 2 kregowy
				send_to_char( "Twoje miê¶nie zaczynaj± dzia³aæ ze zwiekszon± sprawno¶ci±!\n\r", victim );
				act( "Miê¶nie $z zaczynaj± dzia³aæ ze zwiekszon± sprawno¶ci±.", victim, NULL, NULL, TO_ROOM );
				break;
			}
		case 2:
			location = APPLY_DEX;
			send_to_char( "Wydaje ci siê, ¿e teraz uda siê ka¿da sztuczka!\n\r", victim );
			act( "W oku $z widaæ b³ysk zdradzaj±cy wiêksz± pewno¶æ siebie.", victim, NULL, NULL, TO_ROOM );
			break;
		case 3:
			location = APPLY_INT;
			send_to_char( "Twój umys³ zaczyna dzia³aæ ze zwiekszon± sprawno¶ci±!\n\r", victim );
			act( "Umys³ $z zaczyna dzia³aæ ze zwiekszon± sprawno¶ci±.", victim, NULL, NULL, TO_ROOM );
			break;
		case 4:
			location = APPLY_WIS;
			send_to_char( "Teraz wiesz wiêcej o tym ca³ym ¶wiecie!\n\r", victim );
			switch ( victim->sex )
			{
				case 0:
					act( "$n wygl±da na m±drzejsze.", victim, NULL, NULL, TO_ROOM );
					break;
				case 1:
					act( "$n wygl±da na m±drzejszego.", victim, NULL, NULL, TO_ROOM );
					break;
				default:
					act( "$n wygl±da na m±drzejsz±.", victim, NULL, NULL, TO_ROOM );
					break;
			}
			break;
		case 5:
			location = APPLY_CON;
			send_to_char( "Twoje cia³o zaczyna dzia³aæ ze zwiekszon± sprawno¶ci±!\n\r", victim );
			act( "Cia³o $z zaczyna dzia³aæ ze zwiekszon± sprawno¶ci±.", victim, NULL, NULL, TO_ROOM );
			break;
		case 6:
			location = APPLY_CHA;
			send_to_char( "Zaczynasz wygl±daæ wprost rewelacyjnie!\n\r", victim );
			act( "$n zaczyna wygl±daæ wprost rewelacyjnie.", victim, NULL, NULL, TO_ROOM );
			break;
		default:
			location = APPLY_LUC;
			send_to_char( "Czujesz, ¿e szczê¶cie zaczyna ci sprzyjaæ!\n\r", victim );
			act( "Nad $v rozb³yska na chwil± zielono-¿ó³ta otoczka szczê¶cia.", victim, NULL, NULL, TO_ROOM );
			break;
	}

	if ( number_range( 0, (luck + get_curr_stat_deprecated(ch,STAT_INT))/2 ) > 10 ) mod++;
	if ( number_range( 0, luck ) > 10 ) duration += duration / 4;

	af.where = TO_AFFECTS;
	af.type = type;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.modifier = mod;
	af.location = location;
	af.bitvector = &AFF_NONE;

	affect_to_char( victim, &af, NULL, TRUE );

	return;
}

/* usatawiony tak samo jak spell_armor, ale daje ac 4 na 8 tikow */
void spell_spirit_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int dam, modifier = 20 + level;
	int spirit_power = get_skill(ch, gsn_invoke_spirit)/3;

	switch ( ch->class )
	{
	case CLASS_MAG:
	/* luck */
	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_ARMOR ) )
	{
		if ( ch == victim )
			send_to_char( "Ju¿ jeste¶ chroniony przez magiczny pancerz.\n\r", ch );
		else
			act( "$N jest ju¿ chroniony przez magiczny pancerz.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dam = dice( 2, 5 );
	if ( number_range( 0, luck ) < 5 ) dam = ( dam * 105 ) / 100;
	if ( number_range( 0, luck ) > 15 ) dam = ( dam * 9 ) / 10;

	if ( number_range( 0, luck ) > 15 ) modifier += 5;
	if ( number_percent() == 1 ) modifier = 50;

	spell_damage( ch, victim, dam, sn, DAM_HARM , TRUE );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 7; af.rt_duration = 0;
	af.modifier = -modifier;
	af.location = APPLY_AC;
	af.bitvector = &AFF_ARMOR;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Tworzy siê wokó³ ciebie magiczno-duchowy pancerz.\n\r", victim );
	act( "Wokó³ $z tworzy siê magiczno-duchowy pancerz.", victim, NULL, NULL, TO_ROOM );
	return;
	break;

	case CLASS_SHAMAN:

	if ( IS_AFFECTED( victim, AFF_ARMOR ) )
	{
		if ( ch == victim )
			send_to_char( "Ju¿ jeste¶ chroniony przez duchowy pancerz.\n\r", ch );
		else
			act( "$N jest ju¿ chroniony przez duchowy pancerz.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

	if ( check_shaman_invoke ( ch ) == TRUE )
	{
   af.where = TO_AFFECTS;
	af.type = sn;
	af.level = spirit_power/3;
	af.duration = 4+(spirit_power/6); af.rt_duration = 0;
	af.modifier = -(20+spirit_power);
	af.location = APPLY_AC;
	af.bitvector = &AFF_ARMOR;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Czujesz, ¿e otaczaj± i chroni± ciê duchy przodków.\n\r", victim );
	act( "Wokó³ $z pojawiaj± siê na chwilkê blade, pó³prze¼roczyste sylwetki.", victim, NULL, NULL, TO_ROOM );
	return;
   }
   else
   {
		send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
		return;
	}
  break;

  default:
  break;
	}
}

/* zmiana p³ci celu */
void spell_change_sex( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	/* luck */
	int luck;
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		if ( victim == ch )
			send_to_char( "Twoja p³eæ jest ju¿ zmieniona.\n\r", ch );
		else
			act( "$N ma ju¿ zmienion± p³eæ.", ch, NULL, victim, TO_CHAR );
		return;
	}
	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 2 * level; af.rt_duration = 0;
	af.location = APPLY_SEX;

	do
	{
		af.modifier = number_range( 0, 2 ) - victim->sex;
	}
	while ( af.modifier == 0 );

	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Czujesz, ¿e twoja p³eæ ulega zmianie...\n\r", victim );
	act( "$n wygl±da jakby inaczej...", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_faerie_fog( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * ich;

	/* luck */
	int luck;
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ chmury purpurowego dymu.\n\r", ch );
		return;
	}

	act( "$n wypuszcza chmurê purpurowego dymu.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Tworzysz chmurê purpurowego dymu.\n\r", ch );

	for ( ich = ch->in_room->people; ich != NULL; ich = ich->next_in_room )
	{
		if ( ich->invis_level > 0 )
			continue;

		strip_invis( ich, FALSE, FALSE );

		switch ( ich->sex )
		{
			case 0:
				act( "$n zosta³o odkryte!", ich, NULL, NULL, TO_ROOM );
				send_to_char( "Zosta³o¶ odkryte!\n\r", ich );
				break;
			case 1:
				act( "$n zosta³ odkryty!", ich, NULL, NULL, TO_ROOM );
				send_to_char( "Zosta³e¶ odkryty!\n\r", ich );
				break;
			default:
				act( "$n zosta³a odkryta!", ich, NULL, NULL, TO_ROOM );
				send_to_char( "Zosta³a¶ odkryta!\n\r", ich );
				break;
		}
	}
	return;
}

void spell_floating_disc( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{

	OBJ_DATA * disc, *floating;

	/* luck */
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ lataj±cego dysku.\n\r", ch );
		return;
	}

	floating = get_eq_char( ch, WEAR_FLOAT );

	if ( floating != NULL && IS_OBJ_STAT( floating, ITEM_NOREMOVE ) )
	{
		act( "Nie mo¿esz zdj±æ $p.", ch, floating, NULL, TO_CHAR );
		return;
	}

	disc = create_object( get_obj_index( OBJ_VNUM_DISC ), FALSE );
	disc->value[ 0 ] = level * 6; /* Waga ca³kowita: 10 kilo na poziom */
	disc->value[ 3 ] = level * 1; /* Waga przedmiotu: 1 kilo na poziom */
	disc->value[ 4 ] = 100 - (level + get_curr_stat_deprecated( ch, STAT_INT ));   /* Waga przedmiotu po w³o¿eniu */
	disc->timer = 12;

	switch ( ch->sex )
	{
		case 0:
			act( "$n stworzy³o czarny lewituj±cy dysk.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Stworzy³o¶ czarny lewituj±cy dysk.\n\r", ch );
			break;
		case 1:
			act( "$n stworzy³ czarny lewituj±cy dysk.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Stworzy³e¶ czarny lewituj±cy dysk.\n\r", ch );
			break;
		default:
			act( "$n stworzy³a czarny lewituj±cy dysk.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Stworzy³a¶ czarny lewituj±cy dysk.\n\r", ch );
			break;
	}
	obj_to_char( disc, ch );
	wear_obj( ch, disc, TRUE );
	return;
}

/* RT ROM-style gate */
void spell_gate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	char *tar = ( char * ) vo;

	/* luck */
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( !tar || tar[ 0 ] == '\0' )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_world( ch, tar ) ) == NULL
	     || victim == ch
	     || victim->in_room == NULL
	     || !can_see_room( ch, victim->in_room )
	     || EXT_IS_SET( victim->in_room->room_flags, ROOM_SAFE )
	     || EXT_IS_SET( victim->in_room->room_flags, ROOM_PRIVATE )
	     || EXT_IS_SET( victim->in_room->room_flags, ROOM_SOLITARY )
	     || EXT_IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL )
	     || EXT_IS_SET( ch->in_room->room_flags, ROOM_NO_RECALL )
	     || victim->level >= level + 3
	     || ( !IS_NPC( victim ) && victim->level >= LEVEL_HERO )   /* NOT trust */
	     || ( IS_NPC( victim ) && saves_spell( level, victim, DAM_OTHER ) ) )
	{
		send_to_char( "Nie uda³o ci sie.\n\r", ch );
		return;
	}

	act( "$n przechodzi przez magiczn± bramê i znika.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Przechodzisz przez magiczn± bramê.\n\r", ch );
	char_from_room( ch );
	char_to_room( ch, victim->in_room );

	switch ( ch->sex )
	{
		case 0:
			act( "$n przyby³o tu przez magiczn± bramê.", ch, NULL, NULL, TO_ROOM );
			break;
		case 1:
			act( "$n przyby³ tu przez magiczn± bramê.", ch, NULL, NULL, TO_ROOM );
			break;
		default:
			act( "$n przyby³a tu przez magiczn± bramê.", ch, NULL, NULL, TO_ROOM );
			break;
	}
	do_function( ch, &do_look, "auto" );
}

/* RT really nasty high-level attack spell */
void spell_locate_object( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	char buf[ MAX_INPUT_LENGTH ];
	BUFFER *buffer;
	OBJ_DATA *obj;
	OBJ_DATA *in_obj;
	bool found;
	int number = 0, max_found, luck;
	char *tar = ( char * ) vo;
	CHAR_DATA *victim;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê zlokalizowaæ tego przedmiotu.\n\r", ch );
		return;
	}

	if ( !tar || tar[ 0 ] == '\0' )
	{
		send_to_char( "Co chcesz zlokalizowaæ?\n\r", ch );
		return;
	}

	found = FALSE;
	number = 0;
	max_found = IS_IMMORTAL( ch ) ? 200 : level / 2;

	buffer = new_buf();

	for ( obj = object_list; obj != NULL; obj = obj->next )
	{
		if ( !can_see_obj( ch, obj ) || !is_name( tar, obj->name ) || IS_OBJ_STAT( obj, ITEM_NOLOCATE ) || number_percent() > 2 * level )
			continue;

		found = TRUE;
		number++;

		for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
			;

        if ( in_obj->carried_by != NULL && can_see( ch, in_obj->carried_by ) )
        {
            victim = in_obj->carried_by;
            if (ch == victim)
            {
                sprintf( buf, "Posiadasz poszukiwany przedmiot.\n\r");
            }
            else if (!IS_NPC( victim ) && !IS_IMMORTAL( ch ) && ch->in_room != victim->in_room && ( ch->level > 10 || victim->level > 10))
            {
                sprintf( buf, "Kto¶ nosi ten przedmiot, ale nie za bardzo wiesz kto.\n\r");
            }
            else
            {
                sprintf( buf, "Przedmiot ten nosi %s.\n\r", PERS( victim, ch ) );
            }
        }
		else
		{
			if ( in_obj->in_room == NULL )
				continue;

			if ( IS_IMMORTAL( ch ) )
				sprintf( buf, "miejsce w ktorym jest to %s [Room %d]\n\r", in_obj->in_room->name, in_obj->in_room->vnum );
			else
				sprintf( buf, "miejsce w ktorym przedmiot jest to %s\n\r", in_obj->in_room->name );
		}

		sprintf( buf, "%s", capitalize( buf ) );
		add_buf( buffer, buf );

		if ( number >= max_found )
			break;
	}

	if ( !found )
		send_to_char( "Niczego takiego nie znaleziono.\n\r", ch );
	else
		page_to_char( buf_string( buffer ), ch );

	free_buf( buffer );
	return;
}

void spell_recharge( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	int chance, percent, luck;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê na³adowaæ tego przedmiotu.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_WAND && obj->item_type != ITEM_STAFF )
	{
		send_to_char( "Ten przedmiot siê do tego nie nadaje.\n\r", ch );
		return;
	}

	if ( obj->value[ 3 ] >= 3 * level / 2 )
	{
		send_to_char( "Twoje umiejêtno¶ci s± zbyt ma³e ¿eby to zrobiæ.\n\r", ch );
		return;
	}

	if ( obj->value[ 1 ] == 0 )
	{
		send_to_char( "Ten przedmiot ju¿ by³ raz uzupe³niany.\n\r", ch );
		return;
	}

	chance = 40 + 2 * level;

	chance -= obj->value[ 3 ]; /* harder to do high-level spells */
	chance -= ( obj->value[ 1 ] - obj->value[ 2 ] ) * ( obj->value[ 1 ] - obj->value[ 2 ] );

	chance = UMAX( level / 2, chance );

	percent = number_percent();

	if ( percent < chance / 2 )
	{
		act( "$p zaczyna ¶wieciæ ledwo widocznym ¶wiat³em.", ch, obj, NULL, TO_CHAR );
		act( "$p zaczyna ¶wieciæ ledwo widocznym ¶wiat³em.", ch, obj, NULL, TO_ROOM );
		obj->value[ 2 ] = UMAX( obj->value[ 1 ], obj->value[ 2 ] );
		obj->value[ 1 ] = 0;
		return;
	}

	else if ( percent <= chance )
	{
		int chargeback, chargemax;

		act( "$p zaczyna ¶wieciæ ledwo widocznym ¶wiat³em.", ch, obj, NULL, TO_CHAR );
		act( "$p zaczyna ¶wieciæ ledwo widocznym ¶wiat³em.", ch, obj, NULL, TO_CHAR );

		chargemax = obj->value[ 1 ] - obj->value[ 2 ];

		if ( chargemax > 0 )
			chargeback = UMAX( 1, chargemax * percent / 100 );
		else
			chargeback = 0;

		obj->value[ 2 ] += chargeback;
		obj->value[ 1 ] = 0;
		return;
	}
	else if ( percent <= UMIN( 95, 3 * chance / 2 ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		if ( obj->value[ 1 ] > 1 )
			obj->value[ 1 ] --;
		return;
	}

	else /* whoops! */
	{
		act( "$p zaczyna ¶wieciæ intensywnie i po chwili eksploduje!", ch, obj, NULL, TO_CHAR );
		act( "$p zaczyna ¶wieciæ intensywnie i po chwili eksploduje!", ch, obj, NULL, TO_ROOM );
		/*artefact*/
		if ( is_artefact( obj ) ) extract_artefact( obj );
		if ( obj->contains ) extract_artefact_container( obj );
		extract_obj( obj );
	}
}

void spell_summon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	ROOM_INDEX_DATA *pRoom;
	char *tar = ( char * ) vo;
	int luck;
	int chance;

//2008-12-08, Brohacz: wylaczam ten kod, to chyba uzyteczne tylko przy summon creature
	// jak kto¶ nie chce byæ followany
/*	if ( EXT_IS_SET( ch->act, PLR_NOFOLLOW ) )
	{
		send_to_char( "Przecie¿ nie chcesz ¿eby kto¶ za tob± chodzi³.\n\r", ch );
		return;
	}*/

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( !tar || tar[ 0 ] == '\0' )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( ( victim = get_char_area( ch, tar ) ) == NULL
			|| victim == ch
			|| victim->in_room == NULL
			|| EXT_IS_SET( ch->in_room->room_flags, ROOM_SAFE )
			|| EXT_IS_SET( victim->in_room->room_flags, ROOM_SAFE )
			|| EXT_IS_SET( victim->in_room->room_flags, ROOM_NOMAGIC )
			|| EXT_IS_SET( victim->in_room->room_flags, ROOM_SOLITARY )
			|| EXT_IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL )
			|| ( IS_NPC( victim ) && EXT_IS_SET( victim->act, ACT_AGGRESSIVE ) )
			|| ( IS_NPC( victim ) && EXT_IS_SET( victim->act, ACT_PRACTICE ) )
			|| level + 1  < victim->level
			|| ( !IS_NPC( victim ) && victim->level >= LEVEL_IMMORTAL )
			|| victim->fighting != NULL
			|| ( IS_NPC( victim ) && victim->pIndexData->pShop != NULL )
			|| IS_AFFECTED( victim, AFF_SANCTUARY )
			|| IS_AFFECTED( victim, AFF_FORCE_FIELD )
			|| ch->in_room == victim->in_room
			|| ( !IS_NPC( ch ) && !IS_NPC( victim ) && ch->pcdata->mage_specialist == -1 )
			|| ( !IS_NPC( ch ) && !IS_NPC( victim ) && ch->pcdata->mage_specialist >= 0 && !IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
	)
	{
		print_char( ch, "Nie uda³o ci siê przywo³aæ %s.\n\r", tar );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_PRIVATE ) )
	{
		send_to_char( "To siê tutaj nie uda, jest za ma³o miejsca.\n\r", ch );
		return;
	}

	if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_WATER ) )
	{
		if ( !IS_AFFECTED( victim, AFF_FLYING ) && !IS_AFFECTED( victim, AFF_WATERWALK ) )
		{
			act( "Nie uda³o ci siê tutaj przywo³aæ $Z.", ch, NULL, victim, TO_CHAR );
			return;
		}
	}

	if ( !IS_NPC( victim ) && !spell_item_check( ch, sn, "summon PC" ) )
	{
		send_to_char( "Czujesz, ¿e nie jeste¶ w stanie rzuciæ tego czaru z jakiego¶ niewyja¶nionego powodu.\n\r", ch );
		return;
	}

	ROOM_INDEX_DATA *room_ofiary = victim->in_room;

	if ( ( victim->resists[ RESIST_SUMMON ] > 0 && number_percent() < victim->resists[ RESIST_SUMMON ] ) )
	{
		send_to_char( "Próbujesz rzuciæ zaklêcie, ale kiedy czujesz, ¿e ju¿ prawie ci siê uda³o, co¶ rozprasza twój czar.\n\r", ch );
		act( "Widzisz jak $n nagle unosi siê lekko nad ziemiê i staje siê lekko przezroczyst$r, jednak po chwili opada bezpiecznie na ziemiê.", victim, NULL, NULL, TO_ROOM );
		act( "Czujesz jakby jaka¶ si³a próbowa³a ciê wyrwaæ z tego miejsca i przenie¶æ gdzie indziej, ale to szybko mija.", ch, NULL, victim, TO_VICT );
		return;
	}

	if ( level > 37 )//summoner ze zjednoczeniem
		chance = 140;
	else if ( level > 32 )//summoner na 31 lub niesummoner ze zjednoczeniem
		chance = 120;
	else if ( level > 30 )//niesummoner na 31 lub summoner na 29-30
		chance = 100;
	else if ( level < LEVEL_HERO && level >= 20 )
		chance = URANGE( 3, 15 + 5 * ( ( level - 2 ) - victim->level ) + get_curr_stat_deprecated( ch, STAT_LUC ) / 3, 75 );
	else if ( level < 20 && level >= 15 )
		chance = URANGE( 3, 15 + 5 * ( ( level - 2 ) - victim->level ) + get_curr_stat_deprecated( ch, STAT_LUC ) / 3, 95 );
	else if ( level < 15 && level >= 10 )
		chance = URANGE( 3, 20 + 5 * ( ( level - 3 ) - victim->level ) + get_curr_stat_deprecated( ch, STAT_LUC ) / 3, 95 );
	else if ( level < 10 && level >= 5 )
		chance = URANGE( 3, 20 + 5 * ( ( level - 4 ) - victim->level ) + get_curr_stat_deprecated( ch, STAT_LUC ) / 2, 95 );
	else if ( level < 5 )
		chance = URANGE( 3, 25 + 5 * ( ( level - 5 ) - victim->level ) + get_curr_stat_deprecated( ch, STAT_LUC ) / 2, 95 );

	if( !IS_NPC( victim ) )
		chance /= 2;

	if ( number_percent() > chance )
	{
		send_to_char( "Próbujesz rzuciæ zaklêcie, ale kiedy czujesz, ¿e ju¿ prawie ci siê uda³o, co¶ rozprasza twój czar.\n\r", ch );
		act( "Widzisz jak $n nagle unosi siê lekko nad ziemiê i staje siê lekko przezroczyst$r, jednak po chwili opada bezpiecznie na ziemiê.", victim, NULL, NULL, TO_ROOM );
		act( "Czujesz jakby jaka¶ si³a próbowa³a ciê wyrwaæ z tego miejsca i przenie¶æ gdzie indziej, ale to szybko mija.", ch, NULL, victim, TO_VICT );
		return;
	}
	else
	{
		act( "$n nagle znika.", victim, NULL, NULL, TO_ROOM );
		pRoom = victim->in_room;
		char_from_room( victim );
		if ( victim->mounting )
		{
			send_to_char( "Spadasz na ziemiê zyskuj±c parê nowych siniaków.\n\r", victim->mounting );
			do_dismount_body(victim);
		}
		if ( victim->mount )
		{
			char_to_room( victim->mount, pRoom );
			do_dismount_body(victim);
		}
		char_to_room( victim, ch->in_room );
		act( "$n pojawia siê tu nagle.", victim, NULL, NULL, TO_ROOM );
		switch ( ch->sex )
		{
		case 0:
			act( "$n przywo³a³o ciê tutaj!", ch, NULL, victim, TO_VICT );
			break;
		case 1:
			act( "$n przywo³a³ ciê tutaj!", ch, NULL, victim, TO_VICT );
			break;
		default:
			act( "$n przywo³a³a ciê tutaj!", ch, NULL, victim, TO_VICT );
			break;
		}
		do_function( victim, &do_look, "auto" );
	}

	if ( IS_AFFECTED( victim, AFF_SUMMON_DISTORTION ))
	{
		if ( number_percent() -70 < (victim->resists[ RESIST_SUMMON ] > 0 ? ch->resists [ RESIST_SUMMON ] : 0 ) )
		{
			send_to_char( "Nagle ¶wiat zamazuje siê w jedn± plamê, a jaka¶ si³a przenosi ciê w inne miejsce!\n\r" , ch );
			act( "$n nagle znika.", ch, NULL, NULL, TO_ROOM );
			pRoom = ch->in_room;
			char_from_room( ch );
			if ( ch->mounting )
			{
				send_to_char( "Spadasz na ziemiê zyskuj±c parê nowych siniaków.\n\r", ch->mounting );
				do_dismount_body(victim);
									}
			if ( ch->mount )
			{
				char_to_room( ch->mount, pRoom );
				do_dismount_body(victim);
									}
			char_to_room( ch, room_ofiary );
			act( "$n pojawia siê tu nagle.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Magia otaczaj±ca twoj± ofiarê spowodowa³a, ¿e pojawiasz siê tutaj!\n\r", ch);
			do_function( ch, &do_look, "auto" );
		}

	}
	return;
}

void spell_misfortune( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int vnum_luck = 272, luck;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, 25 + luck ) < 1 )
	{
		act( "Nie uda³o ci siê otoczyæ $Z aur± nieszczê¶cia.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_affected( victim, vnum_luck ) )
	{
		act( "Otaczaj±ca $x {Gzielono{x-{Yz³ota{x aura szczê¶cia zanika.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Otaczaj±ca ciê {Gzielono{x-{Yz³ota{x aura szczê¶cia zanika.\n\r", victim );
		affect_strip( victim, vnum_luck );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo¿esz mieæ mniej szczê¶cia!\n\r", ch );
		else
			act( "$N nie mo¿e mieæ mniej szczê¶cia.", ch, NULL, victim, TO_CHAR );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 1 + level / 4; af.rt_duration = 0;
	af.modifier = -level / 10;
	af.location = APPLY_LUC;
	af.bitvector = &AFF_NONE;

	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Czujesz, ¿e szczê¶cie przestaje ci sprzyjaæ!\n\r", victim );
	act( "Dooko³a $z rozb³yska {yzgni³o{x-{Gzielona{x otoczka nieszczê¶cia!", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_lower_resistance( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod, luck;

	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê obni¿yæ odporno¶ci na magiê $X.\n\r", ch );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		act( "Nie dasz rady bardziej obni¿yæ odporno¶ci na magiê $X.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->resists[ RESIST_MAGIC ] < 3 )
	{
		switch ( ch->sex )
		{
			case 0:
				act( "$N nie posiada odporno¶ci na magiê, któr± móg³oby¶ obni¿yæ.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N nie posiada odporno¶ci na magiê, któr± móg³by¶ obni¿yæ.", ch, NULL, victim, TO_CHAR );
				break;
			default:
				act( "$N nie posiada odporno¶ci na magiê, któr± móg³aby¶ obni¿yæ.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	mod = victim->resists[ RESIST_MAGIC ];
	mod *= 30 + level;
	mod /= 100;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = -mod;
	af.duration = 3; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_MAGIC;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Czujesz, ¿e twoja odporno¶æ na magiê siê zmniejszy³a!\n\r", victim );
	switch ( ch->sex )
	{
		case 0:
			act( "Zmniejszy³o¶ odporno¶æ $X na magiê.", ch, NULL, victim, TO_CHAR );
			break;
		case 1:
			act( "Zmniejszy³e¶ odporno¶æ $X na magiê.", ch, NULL, victim, TO_CHAR );
			break;
		default:
			act( "Zmniejszy³a¶ odporno¶æ $X na magiê.", ch, NULL, victim, TO_CHAR );
			break;
	}
	return;
}

void spell_force_missiles( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int i, dam, luck;

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	/* zale¿no¶æ od szczê¶cia */
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ pocisków mocy.\n\r", ch );
		return;
	}

	i = 1 + level / 4;

	if ( number_range( 0, luck ) > 15 ) i++;
	if ( number_range( 0, luck ) < 5 ) i = UMAX( 1, i - 1 );

	if ( IS_AFFECTED( victim, AFF_SHIELD ) )
	{
		if ( i > 1 )
		{
			print_char( ch, "Twoje pociski mocy odbijaj± siê od ochronnej tarczy %s.\n\r", victim->name2 );
			print_char( victim, "Pociski mocy %s odbijaj± siê od twojej ochronnej tarczy.\n\r", ch->name2 );
			act( "Pociski mocy $z odbijaj± siê od ochronnej tarczy $Z.", ch, NULL, victim, TO_NOTVICT );
		}
		else
		{
			print_char( ch, "Twój pocisk mocy odbija siê od ochronnej tarczy %s.\n\r", victim->name2 );
			print_char( victim, "Pocisk mocy %s odbija siê od twojej ochronnej tarczy.\n\r", ch->name2 );
			act( "Pocisk mocy $z odbija siê od ochronnej tarczy $Z.", ch, NULL, victim, TO_NOTVICT );
		}
		return;
	}

	for ( ; i > 0; i-- )
	{
		if ( !victim->in_room || ch->in_room != victim->in_room )
			return;

		dam = 2 + dice( 3, 5 );

		dam = luck_dam_mod( ch, dam );

		spell_damage( ch, victim, dam, sn, DAM_BASH , TRUE );
		if( victim->hit - dam < -11 )
			make_head( victim );
	}

}

void spell_thunder_bolt( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, luck, vluck;

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
  vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, luck - vluck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ wielkiego piorunu.\n\r", ch );
		return;
	}

	dam = number_range( 40, 50 ) + dice( level, 6 );

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

   	if( !spell_item_check( ch, sn , "lighting based spell" ) )
    {
      dam -= dam/5;
    }

	spell_damage( ch, victim, dam, sn, DAM_LIGHTNING , TRUE );
	shock_effect( victim, level, dam, TARGET_CHAR );
	return;

}
void spell_major_haste ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	CHAR_DATA *gch;
	int duration = 6 + level / 5, luck = get_curr_stat_deprecated( ch, STAT_LUC ), count = 0;

	/* zale¿no¶æ od szczê¶cia */
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	/* bonus od szczê¶cia */
	if ( number_range( 0, luck ) > 15 ) duration++;
	if ( number_range( 0, luck ) < 5 ) duration--;

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group( gch, ch ) || is_affected( gch, 60 ) || is_affected( gch, 300 ) || IS_AFFECTED( gch, AFF_HASTE ) || EXT_IS_SET( gch->off_flags, OFF_FAST ) )
			continue;

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = UMAX( 1, duration ); af.rt_duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &AFF_HASTE;

		if ( is_affected( gch, 90 ) )
		{
			act( "Szybko¶æ ruchów $z powraca do normy.", gch, NULL, NULL, TO_ROOM );
			send_to_char( "Szybko¶æ twych ruchów powraca do normy.\n\r", gch );
			affect_strip( gch, 90 );
			continue;
		}

		affect_to_char( gch, &af, NULL, TRUE );

		act( "$n zaczyna poruszaæ siê niewiarygodnie szybko!", gch, NULL, NULL, TO_ROOM );
		if ( gch == ch ) send_to_char( "¦wiat wokó³ ciebie zwalnia.\n\r", ch );
		else act( "Czar $z sprawia, ¿e ¶wiat wokó³ ciebie zwalnia.", ch, NULL, gch, TO_VICT );

		count++;
	}

	if ( count > 0 )
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie.\n\r", ch );
	else
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie, ale niewiele ono da³o...\n\r", ch );

	return;
}

void spell_daze( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), dur, cha;

	if ( victim->level > level + 5 )
	{
		act ( "Twoje zaklêcie nie zadzia³a³o na $C.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PARALYZE ) || IS_AFFECTED( victim, AFF_DAZE ) )
	{
		act( "Twoje zaklêcie nie zadzia³a teraz na $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->resists[ RESIST_MENTAL ] > 0 && number_percent() < victim->resists[ RESIST_MENTAL ] )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N wydaje siê byæ odporne na twoje zaklêcie.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N wydaje siê byæ odporna na twoje zaklêcie.", ch, NULL, victim, TO_CHAR );
				break;
			default:
				act( "$N wydaje siê byæ odporny na twoje zaklêcie.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	dur = level / 7.5;
	if ( ( cha = get_curr_stat_deprecated( ch, STAT_CHA ) ) > 15 )
		++dur;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.duration = UMAX( 1, dur ); af.rt_duration = 0;
	af.modifier = 0;
	af.bitvector = &AFF_DAZE;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			act( "$n rozgl±da siê dooko³a, lekko oszo³omione.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Czujesz siê lekko oszo³omione!\n\r", victim );
			break;
		case 2:
			act( "$n rozgl±da siê dooko³a, lekko oszo³omiona.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Czujesz siê lekko oszo³omiona!\n\r", victim );
			break;
		default:
			act( "$n rozgl±da siê dooko³a, lekko oszo³omiony.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Czujesz siê lekko oszo³omiony!\n\r", victim );
			break;
	}
	return;
}
extern EVENT_DATA * event_first;
void spell_summon_lesser_meteor ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = NULL, *tch;
	EVENT_DATA *event = NULL;
	int meteor_count;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( !IS_OUTSIDE( ch ) ||
	     IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER ) )
	{
		send_to_char( "Musisz byæ na zewn±trz by przywo³aæ meteoryt.\n\r", ch );
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
		if ( ( event->type == EVENT_SUMMON_LESSER_METEOR && ( CHAR_DATA * ) event->arg1 == ch && ( ROOM_INDEX_DATA * ) event->arg2 == ch->in_room ) || ( event->type == EVENT_SUMMON_GREATER_METEOR && ( CHAR_DATA * ) event->arg1 == ch && ( ROOM_INDEX_DATA * ) event->arg2 == ch->in_room ) )
		{
			print_char( ch, "Ju¿ wezwa³<&e¶/a¶/o¶> meteoryt.\n\r");
			return;
		}

	meteor_count = 1;
	create_event( EVENT_SUMMON_LESSER_METEOR, number_range( 2, 3 ) * PULSE_VIOLENCE, ch, ch->in_room, meteor_count );
	act( "Wysoko na niebie pojawia siê czerwona smuga zakoñczona powiêkszaj±c± siê coraz bardziej czerwon± kropk±.", ch, NULL, NULL, TO_ALL );
	return;
}

void spell_eyes_of_the_torturer ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	AFFECT_DATA af;
	CHAR_DATA *gch;
	int duration = 2 + (level-21)/5 , luck, count = 0;

//coby supermob nie rzucal na siebie
	if ( IS_NPC(ch) && ch->pIndexData->vnum == 3 )
	{
 		victim = ( CHAR_DATA * ) vo;
 	}
 	else
 	{
 		victim = ch;
 	}

	luck = get_curr_stat_deprecated( victim, STAT_LUC );

	/* zale¿no¶æ od szczê¶cia */
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	/* bonus od szczê¶cia */
	if ( number_range( 0, luck ) > 15 ) duration++;
	if ( number_range( 0, luck ) < 5 ) duration--;

	for ( gch = victim->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group( gch, victim ) || IS_AFFECTED( gch, AFF_EYES_OF_THE_TORTURER ) )
			continue;

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = duration; af.rt_duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &AFF_EYES_OF_THE_TORTURER;
		affect_to_char( gch, &af, NULL, TRUE );

		act( "Oczy $z zaczynaj± ¶wieciæ z³owieszczym czerwonym blaskiem.", gch, NULL, NULL, TO_ROOM );
		send_to_char( "Widzisz... widzisz o wiele wiêcej! Wszystkie s³abe punkty przeciwników nie s± ju¿ dla ciebie ukryte!\n\r", gch );
		count++;
	}

	if ( count > 0 )
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie.\n\r", ch );
	else
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie, ale niewiele ono da³o...\n\r", ch );

	return;
}

void spell_ghoul_touch( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dam, luck, vluck, duration, holdchance;

	/* luck */

	luck = get_curr_stat_deprecated( ch, STAT_LUC ),
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		else
			act( "Nie uda³o ci siê rzuciæ zaklêcia.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dam = dice( 3, 5 ) + ( level * 2 ) / 3;

	dam = luck_dam_mod( ch, dam );

	act( "Dotkniêcie ghula wywo³uje nieprzyjemne dreszcze na ciele $z.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Dotkniêcie ghula wywo³uje nieprzyjemne dreszcze na ca³ym twym ciele.\n\r", victim );

	spell_damage( ch, victim, dam, sn, DAM_NEGATIVE, FALSE );

	holdchance = number_range( 1, 4 );

	if ( holdchance != 1 )
	{
		if ( is_undead(victim) || IS_SET( victim->form, FORM_CONSTRUCT ) || IS_AFFECTED( victim, AFF_FREE_ACTION ) || IS_AFFECTED( victim, AFF_RESIST_NEGATIVE ) || IS_AFFECTED( victim, AFF_PARALYZE ) || IS_AFFECTED( victim, AFF_ENERGY_SHIELD ) )
			return;

		if ( IS_AFFECTED( victim, AFF_PARALYZE ) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
			return;

		duration = 1 + ( level / 10 );
		if ( number_range( 0, luck ) > 15 ) ++duration;
		if ( number_range( 0, luck ) < 5 ) --duration;

		if ( number_range( 0, vluck ) > 15 ) --duration;
		if ( number_range( 0, vluck ) < 5 ) ++duration;

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.location = APPLY_NONE;
		af.duration = UMAX( 1, duration ); af.rt_duration = 0;
		af.modifier = 0;
		af.bitvector = &AFF_PARALYZE;
		affect_to_char( victim, &af, NULL, TRUE );

		//kasowanie prewaitów
		if ( victim->wait_char )
		{
			free_pwait_char( victim->wait_char );
			victim->wait_char = NULL;
			victim->wait = 2;
		}

		switch ( victim->sex )
		{
			case 0:
				send_to_char( "Zosta³o¶ unieruchomione!\n\r", victim );
				break;
			case 2:
				send_to_char( "Zosta³a¶ unieruchomiona!\n\r", victim );
				break;
			default:
				send_to_char( "Zosta³e¶ unieruchomiony!\n\r", victim );
				break;
		}

		act( "$n zatrzymuje siê w miejscu.", victim, NULL, NULL, TO_ROOM );
		return;
	}
}

void spell_decay( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( victim == ch )
	{
		send_to_char( "Nie dasz rady tego zrobiæ.\n\r", ch );
		return;
	}

	if ( number_range( 0, 30 + luck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( is_undead(victim) || IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		act( "Te zaklêcie nie zadzia³a na $C.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dam = 30 + dice( level, 4 );

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

	spell_damage( ch, victim, dam, sn, DAM_NEGATIVE , TRUE );
	return;
}

void spell_transmute_liquid( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj;
	char buf [ MAX_STRING_LENGTH ];
	int liquid;

	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;
		if ( obj->item_type == ITEM_DRINK_CON )
		{
			// PUSTEGO nie bêdziemy transmutowaæ
			if ( obj->value[ 1 ] == 0 )
			{
				act( "Ten pojemnik jest pusty.", ch, NULL, NULL, TO_CHAR );
				return;
			}
			// ZATRUTEGO nie bêdziemy transmutowaæ
			if ( obj->value[ 3 ] )
			{
				sprintf( buf, "Nie dasz rady przemieniæ %s w %s.\n\r", liq_table[ obj->value[ 2 ] ].liq_name2, obj->name6 );
				send_to_char( buf, ch );
				return;
			}
			// losujemy nowy numerek
			liquid = number_range( 0, 42 );
			// co za pech... ten sam numerek
			if ( obj->value[ 2 ] == liquid )
			{
				act( "Nie uda³o ci siê.", ch, NULL, NULL, TO_CHAR );
				return;
			}
			// zamieniamy...
			sprintf( buf, "Zmieniasz %s w %s w %s.\n\r", liq_table[ obj->value[ 2 ] ].liq_name4, obj->name6, liq_table[ liquid ].liq_name4 );
			send_to_char( buf, ch );
			obj->value[ 2 ] = liquid;
			return;
		}
		else
		{
			act( "$p nie jest pojemnikiem na ciecz.", ch, obj, NULL, TO_CHAR );
			return;
		}
	}
	else
	{
		act( "W czym chcesz zmieniæ p³yn?", ch, NULL, NULL, TO_CHAR );
	}

}

void spell_horrid_wilting( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	OBJ_DATA *obj, *obj_next, *empty;
	char buf[ MAX_STRING_LENGTH ];
	int dam, luck = get_curr_stat_deprecated( ch, STAT_LUC ), liquid, wear_loc;

	if ( victim == ch )
	{
		send_to_char( "Nie dasz rady tego zrobiæ.\n\r", ch );
		return;
	}

	if ( number_range( 0, 30 + luck ) == 0 )
	{
		act( "Nie uda³o ci siê spowodowaæ odparowania wilgoci z cia³a $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( !IS_SET( victim->form, FORM_WARM ) || is_undead(victim) || IS_SET( victim->form, FORM_CONSTRUCT )  )
	{
		act( "Te zaklêcie nie zadzia³a na $C.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dam = 30 + dice( level, 5 );

	dam = luck_dam_mod( ch, dam );

	if ( !IS_NPC( victim ) )
	{
		victim->condition[ COND_THIRST ] = 0;
	}

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

	spell_damage( ch, victim, dam, sn, DAM_NEGATIVE , TRUE );

	if ( !victim->in_room )
		return;

	for ( obj = victim->carrying; obj != NULL; obj = obj_next )
	{
		if ( obj->item_type == ITEM_DRINK_CON && obj->value[ 1 ] > 0 )
		{
			liquid = obj->value[ 2 ];
			sprintf( buf, "%s wyparowywuje z $f.", liq_table[ liquid ].liq_name );
			act( buf, ch, obj, victim, TO_ALL );
			obj->value[ 1 ] = 0;

			if ( obj->value[ 4 ] > 0 && obj->value[ 1 ] <= 0 )
			{

				if ( ( empty = create_object( get_obj_index( obj->value[ 4 ] ), FALSE ) ) != NULL )
				{
					empty->value[ 0 ] = obj->value[ 0 ];
					empty->value[ 1 ] = 0;
					empty->value[ 2 ] = 0;
					empty->value[ 3 ] = 0;
					empty->value[ 4 ] = 0;

					SET_BIT( empty->wear_flags, obj->wear_flags );
					empty->material = obj->material;
					empty->weight = obj->weight;
					empty->condition = obj->condition;
					OBJ_NEXT_CONTENT( obj, obj_next );
					wear_loc = obj->wear_loc;
					obj_from_char( obj );
					obj_to_char( empty, victim );
					if ( wear_loc != WEAR_NONE )
						equip_char( victim, empty, wear_loc, FALSE );
					extract_obj ( obj );
				}
				else
					OBJ_NEXT_CONTENT( obj, obj_next );
			}
			else
				OBJ_NEXT_CONTENT( obj, obj_next );
		}
		else
			OBJ_NEXT_CONTENT( obj, obj_next );
	}

	return;
}

/*  sk³adnia:
    cast 'unholy fury' <cel>

    Czar podnosi si³ê i hp undeada oraz dodaje pol ataku, ale tylko
    dla undeada, którego masterem jest czaruj±cy.
 */

void spell_unholy_fury( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod, chance;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( ch == victim )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( !is_undead(victim) )
	{
		act( "$N nie jest nieumar³ym.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nieuda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		act( "$N jest ju¿ pod wp³ywem przeklêtej furii.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->master != ch )
	{
		act( "$N nie jest twoim martwiakiem.", ch, NULL, victim, TO_CHAR );
		return;
	}

	/* dodatek do si³y */
	chance = number_range( 1, get_curr_stat_deprecated(ch,STAT_INT) );
	mod = 0;
	if ( chance < 3 ) mod = -1;
	else if ( chance > 15 ) mod = 2;
	else if ( chance > 10 ) mod = 1;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_STR;
	af.duration = 4 + level / 4 + 2 * mod; af.rt_duration = 0;
	af.modifier = 1 + level / 10 + mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_HIT;
	af.duration = 4 + level / 4 + 2 * mod; af.rt_duration = 0;
	af.modifier = 50 + level * 3;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );
	victim->hit += 50 + level * 3;

	send_to_char( "Ogarnia ciê przeklêta furia!\n\r", victim );
	act( "$c ogarnia przeklêta furia.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_summon_distortion (int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
		if ( IS_AFFECTED( victim, AFF_RESIST_SUMMON ) || IS_AFFECTED( victim, AFF_SUMMON_DISTORTION ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Nie da rady, w ten spo¶ób siê bardziej nie zabezpieczysz.\n\r", ch );
		}
		else
		{
			act( "W ten sposób nie da rady bardziej ochroniæ $Z.", ch, NULL, victim, TO_CHAR );
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch == victim )
			act( "Nie uda³o ci siê ochroniæ $Z przed przywo³ywaniem.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda³o ci siê ochroniæ siebie przed przywo³ywaniem.\n\r", ch );
		return;
	}
//Affect bardzo podobny do protection from summon, ale kostki sa zmienione
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 20 + level;
	af.duration = 2 + level / 5; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_SUMMON;
	af.bitvector = &AFF_RESIST_SUMMON;
	affect_to_char( victim, &af, NULL, TRUE );

//kolejny affect, tym razem dajemy AFF_SUMMON_DISTORTION;
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 20 + level;
	af.duration = 2 + level / 5; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 2;
	af.bitvector = &AFF_SUMMON_DISTORTION;
	affect_to_char( victim, &af, NULL, TRUE );

  act( "Doko³a $Z pojawia siê opalizuj±ca sfera burz±cej siê magii.", ch, NULL, victim, TO_NOTVICT );
	send_to_char( "Doko³a ciebie pojawia siê opalizuj±ca sfera burz±cej siê magii.\n\r", victim );
	return;
}

void spell_protection_from_summon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_RESIST_SUMMON ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo¿esz byæ bardziej chronion<&y/a/e>.\n\r", ch );
		else
		{
			switch ( ch->sex )
			{
				case 0:
					act( "$N nie mo¿e byæ bardziej chronione.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N nie mo¿e byæ bardziej chroniona.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo¿e byæ bardziej chroniony.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch == victim )
			act( "Nie uda³o ci siê ochroniæ $Z przed przywo³ywaniem.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda³o ci siê ochroniæ siebie przed przywo³ywaniem.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 30 + level * 2;
	af.duration = 4 + level / 6; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_SUMMON;
	af.bitvector = &AFF_RESIST_SUMMON;
	affect_to_char( victim, &af, NULL, TRUE );

	if ( victim != ch )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N jest teraz czê¶ciowo chronione przed przywo³ywaniami.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N jest teraz czê¶ciowo chroniona przed przywo³ywaniami.", ch, NULL, victim, TO_CHAR );
				break;
			default :
				act( "$N jest teraz czê¶ciowo chroniony przed przywo³ywaniami.", ch, NULL, victim, TO_CHAR );
				break;
		}
	}
	else
	{
		switch ( victim->sex )
		{
			case 0:
				send_to_char( "Czujesz, ¿e teraz jeste¶ lepiej chronione przed przywo³ywaniami.\n\r", ch );
				break;
			case 2:
				send_to_char( "Czujesz, ¿e teraz jeste¶ lepiej chroniona przed przywo³ywaniami.\n\r", ch );
				break;
			default :
				send_to_char( "Czujesz, ¿e teraz jeste¶ lepiej chroniony przed przywo³ywaniami.\n\r", ch );
				break;
		}
	}
	return;
}

void spell_create_lesser_illusion( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * illusion;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), i, chance;

	// moby tego nie rzuca
	if ( IS_NPC( ch ) )
		return;

	// ni ma iluzji z graczy
	if ( !IS_NPC( victim ) )
	{
		send_to_char( "Naprawdê nie ma ju¿ z czego tworzyæ iluzji?\n\r", ch );
		return;
	}

	// ni ma iluzji z iluzji
	if ( IS_AFFECTED( victim, AFF_ILLUSION ) )
	{
		act( "Czy to iluzja, ¿e dasz radê stworzyæ iluzjê?", ch, NULL, victim, TO_CHAR );
		return;
	}

	// bez przegiec:P
	if ( EXT_IS_SET( victim->act, ACT_PRACTICE ) || IS_AFFECTED( victim, AFF_SANCTUARY ) || victim->pIndexData->pShop != NULL )
	{
		act( "Naprawdê nie ma ju¿ z czego tworzyæ iluzji?", ch, NULL, victim, TO_CHAR );
		return;
	}

	// jak lesser to lesser
	if ( victim->level >= (2*level)/3 || victim->size > SIZE_LARGE || victim->level >25 )
	{
		send_to_char( "Ten czar jest za s³aby by stworzyæ iluzjê tak silnej istoty.\n\r", ch );
		return;
	}

	// no jak stoii w private, to przecie gdzie to summ±æ?
	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_PRIVATE ) )
	{
		send_to_char( "To siê tutaj nie uda, jest za ma³o miejsca.\n\r", ch );
		return;
	}

	// pech i szansa!?
	// zminiejszamy szansê wraz ze wzrostem wielko¶ci celu
	chance = luck / 2 + 3 * level - 5 * victim->size;
	if ( number_range( 0, 100 ) > chance )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	illusion = create_mobile( victim->pIndexData );
	clone_mobile( victim, illusion );

	char_to_room( illusion, ch->in_room );
	EXT_SET_BIT( illusion->act, ACT_NO_EXP );
    money_reset_character_money ( illusion );
	illusion->level = ch->level-1; // by nie kradl expa
	illusion->max_hit = 1; // i tak sie rozplywa po jednym ciosie
	illusion->dam_type = 44; //Typ obrazen ktore zadaja iluzje to zawsze iluzoryczne uderzenie(mental).

	for ( i = 0;i < MAX_RESIST;i++ )
	{
		illusion->resists[ i ] = 0;
	}

	illusion->hit = get_max_hp(illusion);

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 50;
	af.duration = number_range( 2 * level, 4*level ) ; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_ILLUSION; // by wiedzialo ze zniknac
	affect_to_char( illusion, &af, NULL, TRUE );

	if ( !add_charm( ch, illusion, TRUE ) )
	{
		extract_char( illusion, TRUE );
		return;
	}

	send_to_char( "Zrêcznie tworzysz iluzjê.\n\r", ch );

	af.where = TO_AFFECTS;
	af.type = gsn_domination;
	af.bitvector = &AFF_CHARM;
	affect_to_char( illusion, &af, NULL, TRUE );

	af.type = sn;
	af.bitvector = &AFF_SILENCE; // by nie chodzily kosciane smoki z brethami po miastach
	affect_to_char( illusion, &af, NULL, TRUE );

	add_follower( illusion, ch, TRUE );

	return;
}

void spell_create_greater_illusion( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * illusion;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), i, chance, lvl_limit;

	// moby tego nie rzuca
	if ( IS_NPC( ch ) )
		return;

	// ni ma iluzji z graczy
	if ( !IS_NPC( victim ) )
	{
		send_to_char( "Naprawdê nie ma ju¿ z czego tworzyæ iluzji?\n\r", ch );
		return;
	}

	// ni ma iluzji z iluzji
	if ( IS_AFFECTED( victim, AFF_ILLUSION ) )
	{
		act( "Czy to iluzja, ¿e dasz radê stworzyæ iluzjê?", ch, NULL, victim, TO_CHAR );
		return;
	}

	// bez przegiec:P
	if ( EXT_IS_SET( victim->act, ACT_PRACTICE ) || IS_AFFECTED( victim, AFF_SANCTUARY ) || victim->pIndexData->pShop != NULL )
	{
		act( "Naprawdê nie ma ju¿ z czego tworzyæ iluzji?", ch, NULL, victim, TO_CHAR );
		return;
	}

    // j/w
    lvl_limit = 25;
    if(victim->level > 25 && victim->level < 29)
    {
                     if(spell_item_check(ch, sn, "cgilvl1"))
                     {
                     lvl_limit = 28;
                     }
    }
    if(victim->level > 28)
    {
         if(spell_item_check(ch, sn, "cgilvl2"))
         {
           lvl_limit = 30;
         }
    }

	if ( victim->level + 1 >= level || victim->level > lvl_limit )
	{
		send_to_char( "Ten czar jest za s³aby by stworzyæ iluzjê tak silnej istoty.\n\r", ch );
		return;
	}

    // no jak stoii w private, to przecie gdzie to summ±æ?
	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_PRIVATE ) )
	{
		send_to_char( "To siê tutaj nie uda, jest za ma³o miejsca.\n\r", ch );
		return;
	}

	// pech i szansa!?
	// zminiejszamy szansê wraz ze wzrostem wielko¶ci celu
	chance = luck / 2 + 3 * level - 5 * victim->size;
	if ( number_range( 0, 100 ) > chance )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	illusion = create_mobile( victim->pIndexData );
	clone_mobile( victim, illusion );

	char_to_room( illusion, ch->in_room );
	EXT_SET_BIT( illusion->act, ACT_NO_EXP );
    money_reset_character_money ( illusion );
	illusion->level = ch->level - 1; // by nie kradl expa
	illusion->max_hit = 1; // i tak sie rozplywa po jednym ciosie
	illusion->dam_type = 44; //Typ obrazen ktore zadaja iluzje to zawsze iluzoryczne uderzenie(mental).

	for ( i = 0;i < MAX_RESIST;i++ )
	{
		illusion->resists[ i ] = 0;
	}

	illusion->hit = get_max_hp(illusion);

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 50;
	af.duration = number_range( 2 * level - 20, 3*level ); af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_ILLUSION; // by wiedzialo ze zniknac
	affect_to_char( illusion, &af, NULL, TRUE );

	if ( !add_charm( ch, illusion, TRUE ) )
	{
		extract_char( illusion, TRUE );
		return;
	}

	send_to_char( "Zrêcznie tworzysz iluzjê.\n\r", ch );

	af.where = TO_AFFECTS;
	af.type = gsn_domination;
	af.bitvector = &AFF_CHARM;
	affect_to_char( illusion, &af, NULL, TRUE );

	af.type = sn;
	af.bitvector = &AFF_SILENCE; // by nie chodzily kosciane smoki z brethami po miastach
	affect_to_char( illusion, &af, NULL, TRUE );

	add_follower( illusion, ch, TRUE );

	return;
}

void spell_mass_fly ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	CHAR_DATA *gch;
	int duration = 4 + level / 2, luck = get_curr_stat_deprecated( ch, STAT_LUC ), count = 0;

	/* zale¿no¶æ od szczê¶cia */
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	/* bonus od szczê¶cia */
	if ( number_range( 0, luck ) > 15 ) duration++;
	if ( number_range( 0, luck ) < 5 ) duration--;

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group( gch, ch ) || IS_AFFECTED( gch, AFF_FLOAT ) || IS_AFFECTED( gch, AFF_FLYING ) || gch->position != POS_STANDING || get_carry_weight( gch ) > can_carry_w( gch ) )
			continue;

		af.where = TO_AFFECTS;
		af.type = gsn_fly;
		af.level = level;
		af.duration = duration; af.rt_duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &AFF_FLYING;

		affect_to_char( gch, &af, NULL, TRUE );

		act( "Stopy $z odrywaj± siê od ziemi.", gch, NULL, NULL, TO_ROOM );
		if ( gch == ch ) send_to_char( "Twoje stopy odrywaj± siê od ziemi.\n\r", ch );
		else act( "Czar $z sprawia, ¿e twoje stopy odrywaj± siê od ziemi.", ch, NULL, gch, TO_VICT );

		count++;
	}

	if ( count > 0 )
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie.\n\r", ch );
	else
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie, ale niewiele ono da³o...\n\r", ch );

	return;
}

void spell_embalm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;

	if ( obj->item_type != ITEM_CORPSE_NPC )
	{
		send_to_char( "Czar nie podzia³a³.\n\r", ch );
		return;
	}

	if ( obj->timer > level * 5 )
	{
		send_to_char( "Bardziej ju¿ siê nie da zabalsamowaæ tych zw³ok.\n\r", ch );
		return;
	}

	if ( number_range( 0, 100 ) > get_curr_stat_deprecated( ch, STAT_LUC ) + 3 * level )
	{
		obj->timer /= 2;
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	send_to_char( "Balsamujesz cia³o.\n\r", ch );
	obj->timer = UMIN( 2 * obj->timer, level * 5 );

	return;
}

void spell_soul_trap ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHARM_DATA * new_charm, *charm;
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	CHAR_DATA *soul;
	MOB_INDEX_DATA *mob;
	AFFECT_DATA af;
	char buf[ MAX_INPUT_LENGTH ];
	int pass = 0, luck = get_curr_stat_deprecated( ch, STAT_LUC ), wis = get_curr_stat_deprecated( ch, STAT_WIS ), duration;

	if ( IS_NPC( ch ) )
	{
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_PRIVATE ) )
	{
		send_to_char( "To siê tutaj nie uda, jest za ma³o miejsca.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->act, PLR_NOFOLLOW ) )
	{
		send_to_char( "Przecie¿ nie chcesz ¿eby kto¶ za tob± chodzi³.\n\r", ch );
		return;
	}

	if ( obj->item_type == ITEM_CORPSE_PC )
	{
		act( "Wyci±gasz swe rêce nad $j. Po chwili orientujesz siê, ¿e Bogowie zaopiekowali siê dusz± tej osoby.", ch, obj, NULL, TO_CHAR );
		act( "$n wyci±ga swe rêce nad $j. Po chwili odsuwa siê jednak od niego, a grymas niezadowolenia pojawia siê na $s twarzy.", ch, obj, NULL, TO_NOTVICT );
		return;
	}

	if ( obj->item_type != ITEM_CORPSE_NPC || obj->value[ 0 ] <= 0 || ( mob = get_mob_index( obj->value[ 0 ] ) ) == NULL )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	if ( is_affected(ch,gsn_made_undead) )
	{
		if ( ch->sex == SEX_NEUTRAL )
		{
			send_to_char( "Straci³o¶ koncentracje, musisz chwilê poczekaæ zanim ponownie stworzysz martwiaka.\n\r", ch );
		}
		else if ( ch->sex == SEX_MALE )
		{
			send_to_char( "Straci³e¶ koncentracje, musisz chwilê poczekaæ zanim ponownie stworzysz martwiaka.\n\r", ch );
		}
		else
		{
			send_to_char( "Straci³a¶ koncentracje, musisz chwilê poczekaæ zanim ponownie stworzysz martwiaka.\n\r", ch );
		}
		return;
	}

	if ( !IS_SET( race_table[ mob->race ].type, PERSON ) )
	{
		print_char( ch, "Ta istota nigdy nie mia³a duszy.\n\r" );
		return;
	}

	if ( obj->timer < 4 )
	{
		act( "Wyci±gasz swe rêce nad $j. Po chwili stwierdzasz ze smutkiem, ¿e ca³a si³a ¿yciowa tej osoby ju¿ siê ulotni³a.", ch, obj, NULL, TO_CHAR );
		act( "$n wyci±ga swe rêce nad $j. Po chwili cofa je ze smutkiem na twarzy.", ch, obj, NULL, TO_NOTVICT );
		return;
	}

	if ( level < mob->level + 2 )
	{
		send_to_char( "Ta dusza jest dla ciebie zbyt silna, nie dasz rady jej uwiêziæ.\n\r", ch );
		return;
	}

	sprintf(buf, "%s%s%s", "Zbeszczeszczone zw³oki ", race_table[ mob->race ].name2, " le¿± tutaj powoli gnij±c.");
	free_string(obj->description);
	obj->description = str_dup(buf);

	obj->value[ 0 ] = 0;

	if ( EXT_IS_SET( mob->act, ACT_PALADIN ) || EXT_IS_SET( mob->act, ACT_CLERIC ) )
	{
		if ( number_range( 1, 100 ) + mob->level > level * 2 )
		{
			send_to_char( "Bogowie przeszkodzili ci w uwiêzieniu duszy tej osoby.\n\r", ch );
			act( "Dusza ulatuje z $f prosto pod opiekuñcze skrzyd³o swego Boga.", ch, obj, NULL, TO_ALL );
			obj->value[ 0 ] = 0;
			af.where = TO_AFFECTS;
			af.type = gsn_made_undead;
			af.level = 50;//by sie nie dalo dispelnac
			af.duration = level > 25 ? 1 : 2; af.rt_duration = 0;
			af.location = 0;
			af.modifier = 0;
			af.bitvector = &AFF_NONE;
			af.visible = FALSE;
			affect_to_char( ch, &af, NULL, FALSE );
			return;
		}
		pass = 1;
	}

	if ( pass != 1 )
	{
		if ( level + wis + luck + number_range( 1, 100 ) < 75 )
		{
			send_to_char ( "W ostatniej chwili dusza wyrywa siê z twojej sieci.\n\r", ch );
			act( "Dusza ulatuje z $f prosto do swojego wymiaru.", ch, obj, NULL, TO_ALL );
			obj->value[ 0 ] = 0;
			af.where = TO_AFFECTS;
			af.type = gsn_made_undead;
			af.level = 50;//by sie nie dalo dispelnac
			af.duration = level > 25 ? 1 : 2; af.rt_duration = 0;
			af.location = 0;
			af.modifier = 0;
			af.bitvector = &AFF_NONE;
			af.visible = FALSE;
			affect_to_char( ch, &af, NULL, FALSE );
			return;
		}
	}

	if ( !get_mob_index( MOB_VNUM_SOUL ) )
	{
		print_char( ch, "Nic siê nie dzieje.\n\r" );
		return;
	}

	if ( ( soul = create_mobile( get_mob_index( MOB_VNUM_SOUL ) ) ) == NULL )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	/* making soul */
	/* name */
	sprintf( buf, soul->name, race_table[ mob->race ].name );
	free_string( soul->name );
	soul->name = str_dup( buf );
	/* short */
	sprintf( buf, soul->short_descr, race_table[ mob->race ].name2 );
	free_string( soul->short_descr );
	soul->short_descr = str_dup( buf );
	/* long */
	sprintf( buf, soul->long_descr, race_table[ mob->race ].name2 );
	free_string( soul->long_descr );
	soul->long_descr = str_dup( buf );
	/* desc */
	sprintf( buf, soul->description, race_table[ mob->race ].name );
	free_string( soul->description );
	soul->description = str_dup( buf );
	soul->spec_fun = NULL;
    money_reset_character_money ( soul );
	soul->level = mob->level;
	soul->max_hit = 1;
	soul->hit = get_max_hp(soul);
	soul->size = mob->size;
	soul->weight = 1;

	af.where = TO_AFFECTS;
	af.type = gsn_made_undead;
	af.level = 50;//by sie nie dalo dispelnac
	af.duration = level > 25 ? 2 : 4; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	af.visible = FALSE;
	affect_to_char( ch, &af, NULL, FALSE );

	for ( charm = ch->pcdata->charm_list; charm; charm = charm->next )
		if ( charm->victim == soul )
		{
			return;
		}

	new_charm = ( CHARM_DATA * ) malloc( sizeof( *new_charm ) );
	new_charm->type = 0;
	new_charm->victim = soul;
	new_charm->next = ch->pcdata->charm_list;
	ch->pcdata->charm_list = new_charm;
	char_to_room( soul, ch->in_room );
	act( "Z ciemnej chmury formuje siê $N i spogl±da na ciebie z wyrzutem.", ch, obj, soul, TO_CHAR );
	act( "$n ³apie ciê w pu³apkê! Teraz musisz mu s³u¿yæ...", ch, obj, soul, TO_VICT );
	act( "Z ciemnej chmury formuje siê $N i spogl±da z wyrzutem na $c.", ch, obj, soul, TO_NOTVICT );

	EXT_SET_BIT( soul->act, ACT_RAISED );
	EXT_SET_BIT( soul->act, ACT_NO_EXP );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( soul, &af, NULL, TRUE );

	af.modifier = 0;
	af.bitvector = &AFF_SOUL; // by wiedzialo ze zniknac
	affect_to_char( soul, &af, NULL, TRUE );

	add_follower( soul, ch, TRUE );

	switch ( mob->level )
	{
		case 1:
		case 2:
		case 3:
		case 4:
			soul->damage[ DICE_NUMBER ] = 2;
			soul->damage[ DICE_TYPE ] = 4;
			soul->damage[ DICE_BONUS ] = 1;
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			soul->damage[ DICE_NUMBER ] = 2;
			soul->damage[ DICE_TYPE ] = 6;
			soul->damage[ DICE_BONUS ] = 1;
			break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
			soul->damage[ DICE_NUMBER ] = 3;
			soul->damage[ DICE_TYPE ] = 4;
			soul->damage[ DICE_BONUS ] = 1;
			break;
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
			soul->damage[ DICE_NUMBER ] = 2;
			soul->damage[ DICE_TYPE ] = 7;
			soul->damage[ DICE_BONUS ] = 2;
			break;
		case 20:
		case 21:
		case 22:
		case 23:
			soul->damage[ DICE_NUMBER ] = 3;
			soul->damage[ DICE_TYPE ] = 5;
			soul->damage[ DICE_BONUS ] = 2;
			break;
		case 24:
		case 25:
		case 26:
			soul->damage[ DICE_NUMBER ] = 3;
			soul->damage[ DICE_TYPE ] = 6;
			soul->damage[ DICE_BONUS ] = 2;
			break;
		case 27:
		case 28:
		case 29:
			soul->damage[ DICE_NUMBER ] = 3;
			soul->damage[ DICE_TYPE ] = 8;
			soul->damage[ DICE_BONUS ] = 2;
			break;
		default:
			soul->damage[ DICE_NUMBER ] = 3;
			soul->damage[ DICE_TYPE ] = 3;
			soul->damage[ DICE_BONUS ] = 1;
			break;
	}

	if ( ch->fighting )
		set_fighting( soul, ch->fighting );

	duration = level * 100;

	create_event( EVENT_EXTRACT_SOUL, duration, soul, NULL, 0 );

	return;
}

void spell_summon_insects( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA *insect;
	AFFECT_DATA af;
	int insect_level, insect_hit, charisma, duration, vnum;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	char buf[ MAX_INPUT_LENGTH ];

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
		print_char( ch, "Nie dasz rady przywo³aæ ¿adnego insekta.\n\r" );
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

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ insekta.\n\r", ch );
		return;
	}

	if ( number_range( 1,1500 ) == 1 )
	{
		summon_malfunction( ch, sn );
		return;
	}

	duration = 6;
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) > 10 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 1;
	if ( number_range( 0, luck ) < 3 ) duration -= 1;
	duration = 240 * duration;

	insect_level = URANGE( 1, level / 4, 3 );
	if ( number_range( 0, luck ) > 10 ) ++insect_level;
	if ( number_range( 0, luck ) < 5 ) --insect_level;
	insect_level = UMAX( 1, insect_level );

	insect_hit = 20 + insect_level * number_range( 6, 10 );
	if ( number_range( 0, luck ) > 10 )
		insect_hit = ( insect_hit * 105 ) / 100;
	if ( number_range( 0, luck ) < 5 )
		insect_hit = ( insect_hit * 90 ) / 100;
	// losowanie tego jaki zwierzaczak ma zostaæ za³adowany
	switch ( dice( 1, 3 ) )
	{
		case 1:
			vnum = MOB_VNUM_WAZKA;
			break;
		case 2:
			vnum = MOB_VNUM_ZUK;
			insect_hit = ( insect_hit * 130 ) / 100;
			break;
		case 3:
			vnum = MOB_VNUM_OSA;
			break;
	}

	insect = create_mobile( get_mob_index( vnum ) );

	if (!IS_NPC(ch))
	{
		sprintf( buf, "spell_summon_insects: [%5d], %s, %d %s.", ch->in_room->vnum, ch->name, vnum, insect->name );
		log_string( buf );
	}

	ch->counter[4] = 1;

	char_to_room( insect, ch->in_room );
	EXT_SET_BIT( insect->act, ACT_NO_EXP );
    money_reset_character_money ( insect );
	insect->level = insect_level;
	insect->hit = UMAX( 10, insect_hit );
	insect->max_hit = insect->hit;
	insect->hit = get_max_hp(insect);
	act( "$N pojawia siê.", ch, NULL, insect, TO_ALL );

	if ( !add_charm( ch, insect, TRUE ) )
	{
		EXT_SET_BIT( insect->act, ACT_AGGRESSIVE );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( insect, &af, NULL, TRUE );

	add_follower( insect, ch, TRUE );

	create_event( EVENT_EXTRACT_CHAR, duration, insect, NULL, 0 );

	return;
}

void spell_summon_greenskins( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA *greenskin;
	AFFECT_DATA af;
	int greenskin_level, greenskin_hit, charisma, duration, vnum;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
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

	// jak kto¶ ma mniej ni¿ 9 charyzmy, to niech spada
	charisma = get_curr_stat_deprecated( ch, STAT_CHA );
	if ( charisma < 9 )
	{
		print_char( ch, "Nie dasz rady przywo³aæ ¿adnego zielonoskórego towarzysza.\n\r" );
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
		send_to_char( "Nie uda³o ci siê przywo³aæ ¿adnego zielonoskórego przyjaciela.\n\r", ch );
		return;
	}

	if ( number_range( 1,1200 ) == 1 )
	{
		summon_malfunction( ch, sn );
		return;
	}

	duration = 6;
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) > 10 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 1;
	if ( number_range( 0, luck ) < 3 ) duration -= 1;
	duration = 240 * duration;

	// ustawianie poziomu
	greenskin_level = URANGE( 3, level / 2, 6 );
	if ( number_range( 0, luck ) > 10 ) ++greenskin_level;
	if ( number_range( 0, luck ) < 5 ) --greenskin_level;
	// ustawianie hpków
	greenskin_hit = 40 + greenskin_level * number_range( 15, 20 );
	if ( number_range( 0, luck ) > 10 )
		greenskin_hit = ( greenskin_hit * 105 ) / 100;
	if ( number_range( 0, luck ) < 5 )
		greenskin_hit = ( greenskin_hit * 90 ) / 100;
	// losowanie tego jaki zwierzaczak ma zostaæ za³adowany
	switch ( dice( 1, 3 ) )
	{
		case 1:
			vnum = MOB_VNUM_HOBGOBLIN;
			break;
		case 2:
			vnum = MOB_VNUM_TASLOI;
			greenskin_hit = ( greenskin_hit * 120 ) / 100;
			greenskin_level = ( greenskin_level * 6 ) / 4;
			break;
		case 3:
			vnum = MOB_VNUM_GOBLIN;
			greenskin_level = ( greenskin_level * 4 ) / 6;
			greenskin_hit = ( greenskin_hit * 80 ) / 100;
			break;
	}

	greenskin = create_mobile( get_mob_index( vnum ) );

	if (!IS_NPC(ch))
	{
		sprintf( buf, "spell_summon_greenskins: [%5d], %s, %d %s.", ch->in_room->vnum, ch->name, vnum, greenskin->name );
		log_string( buf );
	}

	ch->counter[4] = 1;

	char_to_room( greenskin, ch->in_room );
	EXT_SET_BIT( greenskin->act, ACT_NO_EXP );
    money_reset_character_money ( greenskin );
	greenskin->level = greenskin_level;
	greenskin->hit = greenskin_hit ;
	greenskin->max_hit = greenskin->hit;
	greenskin->hit = get_max_hp(greenskin);
	act( "$N pojawia siê.", ch, NULL, greenskin, TO_ALL );

	if ( HAS_TRIGGER( greenskin, TRIG_ONLOAD ) )
	{
		mp_onload_trigger( greenskin );
	}

	if ( !add_charm( ch, greenskin, TRUE ) )
	{
		//EXT_SET_BIT( greenskin->act, ACT_AGGRESSIVE );
		//rellik, dodaje polowanie na summonera zrywanego moba, FS#4345
		strip_invis( ch, FALSE, TRUE );
		start_hunting( greenskin, ch );
		start_hating( greenskin, ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( greenskin, &af, NULL, TRUE );

	add_follower( greenskin, ch, TRUE );

	create_event( EVENT_EXTRACT_CHAR, duration, greenskin, NULL, 0 );

	return;
}

void spell_summon_flying_creatures( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA *flyer;
	AFFECT_DATA af;
	int flyer_level, flyer_hit, charisma, duration, vnum;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
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

	// jak kto¶ ma mniej ni¿ 10 charyzmy, to niech spada
	charisma = get_curr_stat_deprecated( ch, STAT_CHA );
	if ( charisma < 10 )
	{
		print_char( ch, "Nie dasz rady przywo³aæ ¿adnego lataj±cego towarzysza.\n\r" );
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

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ ¿adnego lataj±cego przyjaciela.\n\r", ch );
		return;
	}

	if ( number_range( 1,900 ) == 1 )
	{
		summon_malfunction( ch, sn );
		return;
	}

	duration = 6;
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) > 10 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 1;
	if ( number_range( 0, luck ) < 3 ) duration -= 1;
	duration = 240 * duration;

	// ustawianie poziomu
	flyer_level = URANGE( 8, level * 2 / 3, 11 );
	if ( number_range( 0, luck ) > 10 ) ++flyer_level;
	if ( number_range( 0, luck ) < 5 ) --flyer_level;

	flyer_hit = 47 + flyer_level * number_range(15,20);
	if ( number_range( 0, luck ) > 10 )
		flyer_hit = ( flyer_hit * 105 ) / 100;
	if ( number_range( 0, luck ) < 5 )
		flyer_hit = ( flyer_hit * 90 ) / 100;
	// losowanie tego jaki zwierzaczak ma zostaæ za³adowany
	switch ( dice( 1, 3 ) )
	{
		case 1:
			vnum = MOB_VNUM_GRYF;
			break;
		case 2:
			vnum = MOB_VNUM_MANTYKORA;
			break;
		case 3:
			vnum = MOB_VNUM_HARPIA;
			break;
	}

	flyer = create_mobile( get_mob_index( vnum ) );

	if (!IS_NPC(ch))
	{
		sprintf( buf, "spell_summon_flying_creatures: [%5d], %s, %d %s.", ch->in_room->vnum, ch->name, vnum, flyer->name );
		log_string( buf );
	}

	ch->counter[4] = 1;

	char_to_room( flyer, ch->in_room );
	EXT_SET_BIT( flyer->act, ACT_NO_EXP );
    money_reset_character_money ( flyer );
	flyer->level = flyer_level;
	flyer->hit = flyer_hit ;
	flyer->max_hit = flyer->hit;
	flyer->hit = get_max_hp(flyer);
	act( "$N pojawia siê.", ch, NULL, flyer, TO_ALL );

	if ( !add_charm( ch, flyer, TRUE ) )
	{
		EXT_SET_BIT( flyer->act, ACT_AGGRESSIVE );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( flyer, &af, NULL, TRUE );

	add_follower( flyer, ch, TRUE );

	create_event( EVENT_EXTRACT_CHAR, duration, flyer, NULL, 0 );

	return;
}

void spell_summon_strong_creatures( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA *creature;
	AFFECT_DATA af;
	int creature_level, creature_hit, charisma, duration, vnum;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
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

	// jak kto¶ ma mniej ni¿ 11 charyzmy, to niech spada
	charisma = get_curr_stat_deprecated( ch, STAT_CHA );
	if ( charisma < 11 )
	{
		print_char( ch, "Nie dasz rady przywo³aæ ¿adnego silnego towarzysza.\n\r" );
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

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ ¿adnego silnego przyjaciela.\n\r", ch );
		return;
	}

	if ( number_range( 1,700 ) == 1 )
	{
		summon_malfunction( ch, sn );
		return;
	}

	duration = 6;
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) > 10 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 1;
	if ( number_range( 0, luck ) < 3 ) duration -= 1;
	duration = 240 * duration;

	// ustawianie poziomu
	creature_level = URANGE( 15, level - 5, 20 );
	if ( number_range( 0, luck ) > 10 ) ++creature_level;
	if ( number_range( 0, luck ) < 5 ) --creature_level;
	// ustawianie hpków

	creature_hit = 50 + creature_level * number_range(18,23);
	if ( number_range( 0, luck ) > 10 )
		creature_hit = ( creature_hit * 105 ) / 100;
	if ( number_range( 0, luck ) < 5 )
		creature_hit = ( creature_hit * 90 ) / 100;
	// losowanie tego jaki zwierzaczak ma zostaæ za³adowany
	switch ( dice( 1, 3 ) )
	{
		case 1:
			vnum = MOB_VNUM_NAGA;
			break;
		case 2:
			vnum = MOB_VNUM_SAHUAGIN;
			creature_level = ( creature_level * 8 ) / 10;
			creature_hit = ( creature_hit * 8 ) / 10;
			break;
		case 3:
			vnum = MOB_VNUM_GITHYANKI;
			creature_level = ( creature_level * 11 ) / 10;
			creature_hit = ( creature_hit * 11 ) / 10;
			break;
	}

	creature = create_mobile( get_mob_index( vnum ) );

	if (!IS_NPC(ch))
	{
		sprintf( buf, "spell_summon_strong_creatures: [%5d], %s, %d %s.", ch->in_room->vnum, ch->name, vnum, creature->name );
		log_string( buf );
	}

	ch->counter[4] = 1;

	char_to_room( creature, ch->in_room );
	EXT_SET_BIT( creature->act, ACT_NO_EXP );
    money_reset_character_money ( creature );
	creature->level = creature_level;
	creature->hit = creature_hit ;
	creature->max_hit = creature->hit;
	creature->hit = get_max_hp(creature);
	act( "$N pojawia siê.", ch, NULL, creature, TO_ALL );

	if ( HAS_TRIGGER( creature, TRIG_ONLOAD ) )
	{
		mp_onload_trigger( creature );
	}

	if ( !add_charm( ch, creature, TRUE ) )
	{
		EXT_SET_BIT( creature->act, ACT_AGGRESSIVE );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( creature, &af, NULL, TRUE );

	add_follower( creature, ch, TRUE );

	create_event( EVENT_EXTRACT_CHAR, duration, creature, NULL, 0 );

	return;
}

void spell_summon_ancient_creatures( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA *creature;
	AFFECT_DATA af;
	int creature_level, creature_hit, charisma, duration, vnum, malf_chance;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
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

	// jak kto¶ ma mniej ni¿ 12 charyzmy, to niech spada
	charisma = get_curr_stat_deprecated( ch, STAT_CHA );
	if ( charisma < 12 )
	{
		print_char( ch, "Nie dasz rady przywo³aæ ¿adnego staro¿ytnego towarzysza.\n\r" );
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

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ ¿adnego staro¿ytnego przyjaciela.\n\r", ch );
		return;
	}

	if( level > 35 )//feat, mocniejszy mob, wieksza szansa na pomylke
		malf_chance = 330;
	else
		malf_chance = 500;

	if ( number_range( 1, malf_chance ) == 1 )
	{
		summon_malfunction( ch, sn );
		return;
	}

	duration = 6;
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) > 10 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 1;
	if ( number_range( 0, luck ) < 3 ) duration -= 1;
	duration = 240 * duration;

	creature_level = number_range( number_range( ch->level-5, ch->level), URANGE( ch->level, level-5, 34 ) );

	// ustawianie hpków
	creature_hit = 50 + creature_level * number_range(20,25);
	if ( number_range( 0, luck ) > 10 )
		creature_hit = ( creature_hit * 105 ) / 100;
	if ( number_range( 0, luck ) < 5 )
		creature_hit = ( creature_hit * 90 ) / 100;
	// losowanie tego jaki zwierzaczak ma zostaæ za³adowany
	switch ( dice( 1, 3 ) )
	{
		case 1:
			vnum = MOB_VNUM_JEDNOROZEC;
			creature_hit = ( creature_hit * 85 ) / 100;
			creature_level -= URANGE( 1, 30 - level, 5 );
			break;
		case 2:
			vnum = MOB_VNUM_HYDRA; //wiecej hp mniejszy lvl
			creature_hit = ( creature_hit * 115 ) / 100;
			creature_level -= URANGE( 1, 15 - ( level + 1 ) / 2 , 3 );
			break;
		case 3: //mniej hp bo caster
			vnum = MOB_VNUM_DZINN; //jako jedyny ze wszystkich summowanych potworkow zna czary
			creature_hit = ( creature_hit * 75 ) / 100;
			break;
	}

	creature = create_mobile( get_mob_index( vnum ) );

	if (!IS_NPC(ch))
	{
		sprintf( buf, "spell_summon_ancient_creatures: [%5d], %s, %d %s.", ch->in_room->vnum, ch->name, vnum, creature->name );
		log_string( buf );
	}

	ch->counter[4] = 1;

	char_to_room( creature, ch->in_room );
	EXT_SET_BIT( creature->act, ACT_NO_EXP );
    money_reset_character_money ( creature );
	creature->level = creature_level;
	creature->hit = creature_hit ;
	creature->max_hit = creature->hit;
	creature->hit = get_max_hp(creature);
	act( "$N pojawia siê.", ch, NULL, creature, TO_ALL );

	if ( !add_charm( ch, creature, TRUE ) )
	{
		EXT_SET_BIT( creature->act, ACT_AGGRESSIVE );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( creature, &af, NULL, TRUE );

	add_follower( creature, ch, TRUE );

	create_event( EVENT_EXTRACT_CHAR, duration, creature, NULL, 0 );

	return;
}

extern EVENT_DATA * event_first;
void spell_summon_greater_meteor ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = NULL, *tch;
	EVENT_DATA *event = NULL;
	int meteor_count;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( !IS_OUTSIDE( ch ) ||
	     IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER ) )
	{
		send_to_char( "Musisz byæ na zewn±trz by przywo³aæ meteoryt.\n\r", ch );
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
		if ( ( event->type == EVENT_SUMMON_LESSER_METEOR && ( CHAR_DATA * ) event->arg1 == ch && ( ROOM_INDEX_DATA * ) event->arg2 == ch->in_room ) || ( event->type == EVENT_SUMMON_GREATER_METEOR && ( CHAR_DATA * ) event->arg1 == ch && ( ROOM_INDEX_DATA * ) event->arg2 == ch->in_room ) )
		{
			print_char( ch, "Ju¿ wezwa³<&e¶/a¶/o¶> meteoryt.\n\r" );
			return;
		}

	meteor_count = 1;
	create_event( EVENT_SUMMON_GREATER_METEOR, number_range( 2, 3 ) * PULSE_VIOLENCE, ch, ch->in_room, meteor_count );
	act( "Wysoko na niebie pojawia siê czerwona smuga zakoñczona powiêkszaj±c± siê coraz bardziej czerwon± kropk±.", ch, NULL, NULL, TO_ALL );
	return;
}

/* taki fajny spellik, mistrzowi przemian zmieniaja sie lapki w ostrza, co z tym idzie
nie moze wykonywac prawie niczego z act_obj.c ani rzucac czarkow. Memowac moze */
void spell_razorblade_hands( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = NULL;
	OBJ_DATA *obj2 = NULL;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), intt = get_curr_stat_deprecated( ch, STAT_INT ), dur;

	if ( IS_AFFECTED( ch, AFF_RAZORBLADED ) )
	{
		send_to_char( "Twoje d³onie s± ju¿ przemienione w ostrza.\n\r", ch );
		return;
	}

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê przemieniæ swych d³oni w ostrza.\n\r", ch );
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

	if ( get_eq_char( ch, WEAR_HOLD ) || get_eq_char( ch, WEAR_SHIELD ) || get_eq_char( ch, WEAR_INSTRUMENT ) )
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
		obj = create_object( get_obj_index( OBJ_VNUM_NORMAL_RAZORBLADE ), FALSE );
	else if (level < 24 )
		obj = create_object( get_obj_index( OBJ_VNUM_SHARP_RAZORBLADE ), FALSE );
	else
		obj = create_object( get_obj_index( OBJ_VNUM_VORPAL_RAZORBLADE ), FALSE );

	obj2 = create_object( get_obj_index( OBJ_VNUM_FALSE_RAZORBLADE ), FALSE );

	if ( !obj )
		return;

	if ( !obj2 )
		return;

	dur = number_range( 2, 4 );

	if ( dice( 2, luck ) > 20 )
		++dur;

	if ( dice( 2, luck ) < 8 )
		--dur;

	if ( dice( 2, intt ) > 20 )
		++dur;

	if ( dice( 2, intt ) < 8 )
		--dur;
	/* umagicznienie */
	obj->value[ 5 ] = level / 15;
	obj2->value[ 5 ] = level / 15;
	obj->value[ 6 ] = level / 15;
	obj2->value[ 6 ] = level / 15;

	if ( dice( 2, luck ) > 29 )
	{
	  	++obj->value[ 5 ];
	  	++obj2->value[ 5 ];
	}

	obj->timer = dur + 1; //bo timer rozsypuje sie z przechodzenia z 1 na 0, a affect na 0 jeszcze trwa
	obj2->timer = dur + 1;
	obj_to_char( obj, ch );
	equip_char( ch, obj, WEAR_WIELD, TRUE );
	obj_to_char( obj2, ch );
	equip_char( ch, obj2, WEAR_SECOND, TRUE ); //dla wygladu, wg kodu bedzie walil dwa razy pierwsza reka

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, dur ); af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.modifier = number_range(2, 4);
	af.bitvector = &AFF_RAZORBLADED;
	affect_to_char( ch, &af, NULL, TRUE );

	act( "D³onie $z zamieniaj± siê w ostrza.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Twoje d³onie zamieniaj± siê w ostrza.\n\r", ch );
	return;
}

void spell_energy_strike( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	CHAR_DATA *victim = ( CHAR_DATA * ) vo;
	int dam, proceed;
    int luck = get_curr_stat_deprecated( ch, STAT_LUC );
    int intt = get_curr_stat_deprecated( ch, STAT_INT );

	if ( IS_NPC ( ch ) )
		return;

	if ( is_affected( ch, gsn_energy_strike ) )
	{
		send_to_char( "Próbujesz skupiæ wokó³ siebie energiê, jednak jeste¶ zbyt zmêczon<&y/a/e> i skupiona ju¿ w tobie energia ucieka na zewn±trz wype³niaj±c twe cia³o bólem!\n\r", ch );
		act( "Widzisz jak $n wykonuje dziwne, skomplikowane gesty, jednak po chwili zaprzestaje i g³o¶no jêczy z bólu.", ch, NULL, NULL, TO_ROOM );
		ch->hit = ch->hit - ( get_max_hp(ch) / 10 );
		ch->move = ch->move - ( ch->max_move / 10 );
		ch->condition[ COND_THIRST ] -= 4;
		ch->condition[ COND_HUNGER ] -= 4;
		if ( ch->condition[ COND_THIRST ] < 0 )
			ch->condition[ COND_THIRST ] = 0;
		if ( ch->condition[ COND_HUNGER ] < 0 )
			ch->condition[ COND_HUNGER ] = 0;
		if ( ch->hit <= 0 )
		{
			send_to_char( "Padasz wymêczon<&y/a/e> na ziemiê.\n\r", ch );
			act( "$n traci przytomno¶æ.", ch, NULL, NULL, TO_ROOM );
			ch->hit = 0;
			ch->move = 0;
			return;
		}
		if ( ch->hit <= 0 )
		{
			send_to_char( "Padasz wymêczon<&y/a/e> na ziemiê.\n\r", ch );
			act( "$n traci przytomno¶æ.", ch, NULL, NULL, TO_ROOM );
			ch->hit = 0;
			ch->move = 0;
			return;
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "W ostatniej chwili pope³niasz fataln± pomy³kê i zgromadzona w twym ciele energia rozp³ywa siê we wszystkie strony nape³niaj±c ciê bólem!\n\r", ch );
		act( "Widzisz jak $n wykonuje dziwne, skomplikowane gesty, jednak po chwili zaprzestaje i g³o¶no jêczy z bólu.", ch, NULL, NULL, TO_ROOM );
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = 0;
		af.duration = 5; af.rt_duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &AFF_NONE;
		affect_to_char( ch, &af, NULL, TRUE );
		ch->hit = ch->hit - ( get_max_hp(ch) / 10 );
		ch->move = ch->move - ( ch->max_move / 10 );
		ch->condition[ COND_THIRST ] -= 4;
		ch->condition[ COND_HUNGER ] -= 4;
		if ( ch->condition[ COND_THIRST ] < 0 )
			ch->condition[ COND_THIRST ] = 0;
		if ( ch->condition[ COND_HUNGER ] < 0 )
			ch->condition[ COND_HUNGER ] = 0;
		if ( ch->hit <= 0 )
		{
			send_to_char( "Padasz wymêczon<&y/a/e> na ziemiê.\n\r", ch );
			act( "$n traci przytomno¶æ.", ch, NULL, NULL, TO_ROOM );
			ch->hit = 0;
			ch->move = 0;
			return;
		}
		if ( ch->move <= 0 )
		{
			send_to_char( "Padasz wymêczon<&y/a/e> na ziemiê.\n\r", ch );
			act( "$n traci przytomno¶æ.", ch, NULL, NULL, TO_ROOM );
			ch->hit = 0;
			ch->move = 0;
			return;
		}
		return;
	}

	dam = 65 + dice( level, 7 );

	dam = luck_dam_mod( ch, dam );

	/*dam z inta*/
	if ( dice( 2, intt ) > 35 )
		dam += dam / 20;
	if ( dice( 2, intt ) < 10 )
	{
		dam /= 2;
		send_to_char( "Mylisz siê lekko w inkantacji, czê¶æ utworzonej przez ciebie energii ulatuje w nieznane...\n\r", ch );
	}

	if ( is_affected( ch, gsn_malnutrition ) )
	{
		dam /= 2;
		send_to_char( "Bolesne ssanie w ¿o³±dku lekko zak³uca tw± koncentracjê, czê¶æ utworzonej przez ciebie energii ulatuje w nieznane... \n\r", ch );
	}

	if ( is_affected( ch, gsn_dehydration ) )
	{
		dam /= 2;
		send_to_char( "Nieustaj±ce pragnienie lekko zak³uca tw± koncentracjê, czê¶æ utworzonej przez ciebie energii ulatuje w nieznane... \n\r", ch );
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 50;
	af.duration = 4; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	affect_to_char( ch, &af, NULL, TRUE );

	ch->hit = ch->hit - ( get_max_hp(ch) / 10 );
	ch->move = ch->move - ( ch->max_move / 10 );
	ch->condition[ COND_THIRST ] -= 4;
	ch->condition[ COND_HUNGER ] -= 4;
	if ( ch->condition[ COND_THIRST ] < 0 )
		ch->condition[ COND_THIRST ] = 0;
	if ( ch->condition[ COND_HUNGER ] < 0 )
		ch->condition[ COND_HUNGER ] = 0;
	if ( ch->hit <= 0 )
	{
		send_to_char( "Padasz wymêczon<&y/a/e> na ziemiê.\n\r", ch );
		act( "$n traci przytomno¶æ.", ch, NULL, NULL, TO_ROOM );
		ch->hit = 0;
		ch->move = 0;
		return;
	}
	if ( ch->move <= 0 )
	{
		send_to_char( "Padasz wymêczon<&y/a/e> na ziemiê.\n\r", ch );
		act( "$n traci przytomno¶æ.", ch, NULL, NULL, TO_ROOM );
		ch->hit = 0;
		ch->move = 0;
		return;
	}

	proceed = 1;
	if ( victim->hit - dam < -11 ) proceed = 0;

	spell_damage( ch, victim, dam, sn, DAM_BASH , TRUE );

	if( !IS_AFFECTED(victim,AFF_STABILITY))
	{
		victim->position = POS_SITTING;
		WAIT_STATE( victim, 36 );
	}

	if ( victim == NULL ) return;
	if ( proceed == 0 ) return;
	if ( victim->size >= SIZE_HUGE )
	{
		if( IS_AFFECTED(victim,AFF_STABILITY))
		{
			act("Nagle jaka¶ niewidzialna si³a podnosi $c i stawia na nogi.", victim,NULL,NULL,TO_ROOM );
			act("Nagle niewidzialna si³a podnosi ciê i stawia na nogi.", victim,NULL,NULL,TO_CHAR );
		}
		return;
	}

	if ( victim->in_room->sector_type == 0 || //wewnatrz
	     victim->in_room->sector_type == 14 ||
	     victim->in_room->sector_type == 15 || //te trzy to podziemia, jaskinia i podziemia naturalne
	     victim->in_room->sector_type == 17 )
	{
		act( "Impet uderzenia ciska $V jak piórkiem o ¶cianê, co wywo³uje kolejny grymas bólu na $S twarzy.", ch, NULL, victim, TO_CHAR );
		act( "Impet uderzenia ciska tob± jak piórkiem w ¶cianê powiêkszaj±c jeszcze bó³.", ch, NULL, victim, TO_VICT );
		act( "Impet uderzenia ciska $V jak piórkiem o ¶cianê, co wywo³uje kolejny grymas bólu na $S twarzy.", ch, NULL, victim, TO_NOTVICT );
		dam = dice( 3, victim->weight / 50 );
		spell_damage( ch, victim, dam, sn, DAM_BASH, FALSE );
		if( !IS_AFFECTED(victim,AFF_STABILITY))
			victim->position = POS_SITTING;
	}

	if( IS_AFFECTED(victim,AFF_STABILITY))
	{
		act("Nagle jaka¶ niewidzialna si³a podnosi $c i stawia na nogi.", victim,NULL,NULL,TO_ROOM );
		act("Nagle niewidzialna si³a podnosi ciê i stawia na nogi.", victim,NULL,NULL,TO_CHAR );
	}

	return;
}

void spell_eye_of_vision( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * eye;
	AFFECT_DATA *pAf;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), intt = get_curr_stat_deprecated( ch, STAT_INT ), timer;

	if ( get_eq_char( ch, WEAR_FLOAT ) != NULL )
	{
		send_to_char( "Ko³o ciebie unosi siê ju¿ jaki¶ przedmiot.\n\r", ch );
		return;
	}

	if ( number_range( 0, 25 + luck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ oka widzenia.\n\r", ch );
		return;
	}

	timer = (level * 2) / 5;
	if ( dice( 2, intt ) > 30 )
		timer += timer / 3;

	if ( dice( 2, intt ) < 10 )
		timer -= timer / 3;

	if ( dice( 2, luck ) > 25 )
		++timer;

	if ( dice( 2, luck ) < 8 )
		--timer;

	eye = create_object( get_obj_index( OBJ_VNUM_EYE_OF_VISION ), FALSE );

	if ( dice( 1, 10 ) > 5 )
	{
		pAf = new_affect();
		pAf->location = APPLY_NONE;
		pAf->modifier = 0;
		pAf->where = TO_AFFECTS;
		pAf->type = -1;
		pAf->duration = -1;
		pAf->rt_duration = 0;
		pAf->bitvector = &AFF_DETECT_INVIS;
		pAf->level = level;
		pAf->next = eye->affected;
		eye->affected = pAf;

		pAf = new_affect();
		pAf->location = APPLY_NONE;
		pAf->modifier = 0;
		pAf->where = TO_AFFECTS;
		pAf->type = -1;
		pAf->duration = -1;
		pAf->rt_duration = 0;
		pAf->bitvector = &AFF_DETECT_HIDDEN;
		pAf->level = level;
		pAf->next = eye->affected;
		eye->affected = pAf;
	}
	else
	{
		if ( dice( 1, 10 ) > 5 )
		{
			pAf = new_affect();
			pAf->location = APPLY_NONE;
			pAf->modifier = 0;
			pAf->where = TO_AFFECTS;
			pAf->type = -1;
			pAf->duration = -1;
			pAf->rt_duration = 0;
			pAf->bitvector = &AFF_DETECT_INVIS;
			pAf->level = level;
			pAf->next = eye->affected;
			eye->affected = pAf;
		}
		else
		{
			pAf = new_affect();
			pAf->location = APPLY_NONE;
			pAf->modifier = 0;
			pAf->where = TO_AFFECTS;
			pAf->type = -1;
			pAf->duration = -1;
			pAf->rt_duration = 0;
			pAf->bitvector = &AFF_DETECT_HIDDEN;
			pAf->level = level;
			pAf->next = eye->affected;
			eye->affected = pAf;
		}
	}

	if ( dice( 1, 5 ) == 5 )
	{
		pAf = new_affect();
		pAf->location = APPLY_NONE;
		pAf->modifier = 0;
		pAf->where = TO_AFFECTS;
		pAf->type = -1;
		pAf->duration = -1;
		pAf->rt_duration = 0;
		pAf->bitvector = &AFF_DETECT_MAGIC;
		pAf->level = level;
		pAf->next = eye->affected;
		eye->affected = pAf;
	}

	if ( dice( 1, 5 ) == 5 )
	{
		if ( dice( 1, 2 ) == 1 )
		{
			pAf = new_affect();
			pAf->location = APPLY_NONE;
			pAf->modifier = 0;
			pAf->where = TO_AFFECTS;
			pAf->type = -1;
			pAf->duration = -1;
			pAf->rt_duration = 0;
			pAf->bitvector = &AFF_DETECT_GOOD;
			pAf->level = level;
			pAf->next = eye->affected;
			eye->affected = pAf;
		}
		else
		{
			pAf = new_affect();
			pAf->location = APPLY_NONE;
			pAf->modifier = 0;
			pAf->where = TO_AFFECTS;
			pAf->type = -1;
			pAf->duration = -1;
			pAf->rt_duration = 0;
			pAf->bitvector = &AFF_DETECT_EVIL;
			pAf->level = level;
			pAf->next = eye->affected;
			eye->affected = pAf;
		}
	}

	eye->timer = timer;
	obj_to_char( eye, ch );
	equip_char( ch, eye, WEAR_FLOAT, TRUE );
	send_to_char( "Nad twoim ramieniem pojawia siê oko widzenia.\n\r", ch );
	act( "Ko³o g³owy $z pojawia siê tajemnicze oko.", ch, NULL, NULL, TO_ROOM );

	return;
}

void spell_dancing_lights( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{

	if ( !EXT_IS_SET( ch->in_room->room_flags, ROOM_MAGICDARK ) )
	{
		send_to_char( "Otaczaj± ciê tañcz±ce ¶wiat³a, ale nic siê nie dzieje.\n\r", ch );
		return;
	}
	EXT_REMOVE_BIT( ch->in_room->room_flags, ROOM_MAGICDARK );
	if ( ch->in_room->people )
		act( "Otaczaj± ciê tañcz±ce ¶wiat³a rozpraszaj±c magiczny mrok.", ch->in_room->people, NULL, NULL, TO_ALL );

	return;
}

void spell_enchant_instrument( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	int bonus, luck, intt;

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	intt = get_curr_stat_deprecated( ch, STAT_INT );
	/* moby nie enchantuj±, bo po co */
	if ( IS_NPC( ch ) )
		return;

	if ( number_range( 0, luck / 3 ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê ulepszyæ tego instrumentu.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_MUSICAL_INSTRUMENT )
	{
		send_to_char( "To nie jest instrument muzyczny.\n\r", ch );
		return;
	}

	if ( obj->wear_loc != -1 )
	{
		send_to_char( "Musisz to mieæ przy sobie.\n\r", ch );
		return;
	}

	if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) || IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
	{
		send_to_char( "Czujesz, ¿e twa magia nie jest w stanie ulepszyæ tego instrumentu.\n\r", ch );
		return;
	}

	bonus = URANGE( 1, number_range( level, level + 5 ) + number_range( intt - 5, intt ), 50 );

	/* item destroyed */
	if ( number_range( 0, 50 ) == 0 )
	{
		AFFECT_DATA af;
		switch ( number_range( 0, 10 ) )
		{
			case 0:  //blidness
				if ( !IS_AFFECTED(ch,AFF_PERFECT_SENSES ) )
				{
					act( "$p wybucha ci prosto w twoje oczy!", ch, obj, NULL, TO_CHAR );
					act( "$p wybucha $x prosto w $s oczy!", ch, obj, NULL, TO_ROOM );
					af.where = TO_AFFECTS;
					af.type = 5;
					af.level = level;
					af.location = APPLY_NONE;
					af.modifier = 0;
					af.duration = UMAX( 1, bonus ); af.rt_duration = 0;
					af.bitvector = &AFF_BLIND;
					affect_to_char( ch, &af, NULL, TRUE );
					break;
				}

			case 1:  //weaken
				act( "$p wybucha rani±c ciê i rozrywaj±c twoje cia³o!", ch, obj, NULL, TO_CHAR );
				act( "$p wybucha rani±c $x i rozrywaj±c $c cia³o!", ch, obj, NULL, TO_ROOM );
				af.where = TO_AFFECTS;
				af.type = 96;
				af.level = level;
				af.duration = UMAX( 1, bonus ); af.rt_duration = 0;
				af.location = APPLY_STR;
				af.modifier = - UMAX( 1, bonus );
				af.bitvector = &AFF_WEAKEN;
				affect_to_char( ch, &af, NULL, TRUE );

				if ( !ch->mount && get_carry_weight( ch ) > can_carry_w( ch ) && IS_AFFECTED( ch, AFF_FLYING ) )
				{
					act( "$n pod wp³ywem ciê¿aru opada na ziemiê.", ch, NULL, NULL, TO_ROOM );
					send_to_char( "Pod wp³ywem obci±¿enia opadasz na ziemiê.\n\r", ch );
					affect_strip( ch, 52 );
				}
				break;

			case 3: //deafness
				if ( !IS_AFFECTED(ch,AFF_PERFECT_SENSES ) )
				{
					act( "$p wybucha, po czym wszystko dooko³a milknie!", ch, obj, NULL, TO_CHAR );
					act( "$p wybucha prosto w twarz $x!", ch, obj, NULL, TO_ROOM );
					af.where = TO_AFFECTS;
					af.type = 169;
					af.level = level;
					af.duration = 4; af.rt_duration = 0;
					af.location = APPLY_NONE;
					af.modifier = 0;
					af.bitvector = &AFF_DEAFNESS;
					affect_to_char( ch, &af, NULL, TRUE );
					break;
				}

			default :
				act( "$p wybucha, rani±c ciê od³amkami!", ch, obj, NULL, TO_CHAR );
				act( "$p wybucha, rani±c $z od³amkami!", ch, obj, NULL, TO_ROOM );
				break;
		}
		damage( ch, ch, 20 + dice( 2, bonus ), TYPE_UNDEFINED, DAM_PIERCE, FALSE );
		fire_effect( ch, level, 20 + dice( 2, bonus ), TARGET_CHAR );
		/*artefact*/
		if ( is_artefact( obj ) ) extract_artefact( obj );
		else extract_obj( obj );
		return;
	}

	obj->value[ 1 ] += bonus;
	obj->timer = level * ( intt + bonus );
	EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );

	print_char( ch, "%s rozb³yskuje przez chwile jasnym ¶wiat³em.\n\r", capitalize( obj->short_descr ) );
	return;
}

void spell_alter_instrument( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	int luck, wis, intt;

	intt = get_curr_stat_deprecated( ch, STAT_INT );
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	wis = get_curr_stat_deprecated( ch, STAT_WIS );
	/* moby nie, bo po co */
	if ( IS_NPC( ch ) )
		return;

	if ( number_range( 0, ( intt + wis + luck ) / 10 ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê przemieniæ tego instrumentu.\n\r", ch );
		return;
	}

	if ( obj->item_type != ITEM_MUSICAL_INSTRUMENT )
	{
		send_to_char( "To nie jest instrument muzyczny.\n\r", ch );
		return;
	}

	if ( obj->wear_loc != -1 )
	{
		send_to_char( "Musisz to mieæ przy sobie.\n\r", ch );
		return;
	}

	if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) || IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
	{
		send_to_char( "Czujesz, ¿e twa magia nie jest w stanie przemieniæ tego instrumentu.\n\r", ch );
		return;
	}

	switch ( obj->value[ 0 ] )
	{
		case INSTRUMENT_OCARINA:
			print_char( ch, "%s rozb³yskuje przez chwilê jasnym ¶wiat³em i zmienia siê w ma³y flet!\n\r", capitalize( obj->short_descr ) );
			obj->value[ 0 ] = INSTRUMENT_FLUTE;
			obj->name = str_dup( "ma³y flet" );
			obj->name2 = str_dup( "ma³ego fletu" );
			obj->name3 = str_dup( "ma³emu fletowi" );
			obj->name4 = str_dup( "ma³y flet" );
			obj->name5 = str_dup( "ma³ym fletem" );
			obj->name6 = str_dup( "ma³ym flecie" );
			obj->short_descr = str_dup( "ma³y flet" );
			obj->description = str_dup( "Niedu¿y flecik pod³u¿ny le¿y tutaj." );
			obj->item_description = str_dup( "Jest to ma³y flet pod³u¿ny, czyli dêty instrument przypominaj±cy trochê szeroki patyk z dziur± wewn±trz i niewielkimi otworami na górze. U¿ywany jest zwykle w muzyce solowej oraz w orkiestrze do realizacji najwy¿szego g³osu." );
			obj->ident_description = str_dup( "Czujesz, ¿e materia, z której stworzony zosta³ ten flet, powsta³a z magii. Wydaje ci siê, ¿e pod jego pow³ok± dostrzegasz wizerunek jakiego¶ innego instrumentu." );
			obj->hidden_description = str_dup( "Nie widzisz nic specjalnego w ma³ym flecie." );
			break;
		case INSTRUMENT_FLUTE:
			if ( dice( 1, 2 ) == 1 )
			{
				print_char( ch, "%s rozb³yskuje przez chwilê jasnym ¶wiat³em i zmienia siê w okarynê!\n\r", capitalize( obj->short_descr ) );
				obj->value[ 0 ] = INSTRUMENT_OCARINA;
				obj->name = str_dup( "okaryna" );
				obj->name2 = str_dup( "okaryny" );
				obj->name3 = str_dup( "okarynie" );
				obj->name4 = str_dup( "okarynê" );
				obj->name5 = str_dup( "okaryn±" );
				obj->name6 = str_dup( "okarynie" );
				obj->short_descr = str_dup( "okaryna" );
				obj->description = str_dup( "Malutka okaryna le¿y tutaj." );
				obj->item_description = str_dup( "Jest to okaryna, ludowy instrument dêty, wargowy, wygl±dem przypomina naczynie z osmioma otworami bocznymi oraz wystaj±c± czê¶ci± przyustn± zaopatrzon± w wargê." );
				obj->ident_description = str_dup( "Czujesz, ¿e materia, z której stworzona zosta³a ta okaryna, powsta³a z magii. Wydaje ci siê, ¿e pod jej pow³ok± dostrzegasz wizerunek jakiego¶ innego instrumentu." );
				obj->hidden_description = str_dup( "Nie widzisz nic specjalnego w okarynie." );
			}
			else
			{
				print_char( ch, "%s rozb³yskuje przez chwilê jasnym ¶wiat³em i zmienia siê w piêkn± lutniê!\n\r", capitalize( obj->short_descr ) );
				obj->value[ 0 ] = INSTRUMENT_LUTE;
				obj->name = str_dup( "piêkna lutnia" );
				obj->name2 = str_dup( "piêknej lutni" );
				obj->name3 = str_dup( "piêknej lutni" );
				obj->name4 = str_dup( "piêkn± lutniê" );
				obj->name5 = str_dup( "piêkn± lutni±" );
				obj->name6 = str_dup( "piêknej lutni" );
				obj->short_descr = str_dup( "piêkna lutnia" );
				obj->description = str_dup( "Piêkna lutnia le¿y tutaj." );
				obj->item_description = str_dup( "Jest to lutnia, instrument strunowy szarpany, posiada mocno wypuk³e pud³o rezonansowe o przekroju gruszkowym, szyjkê z progami zakoñczon± wygiêt± do ty³u g³ówk± z kó³kami do naci±gu strun." );
				obj->ident_description = str_dup( "Czujesz, ¿e materia, z której stworzona zosta³a ta lutnia, powsta³a z magii. Wydaje ci siê, ¿e pod jej pow³ok± dostrzegasz wizerunek jakiego¶ innego instrumentu." );
				obj->hidden_description = str_dup( "Nie widzisz nic specjalnego w piêknej lutni." );
			}
			break;
		case INSTRUMENT_LUTE:
			if ( dice( 1, 2 ) == 1 )
			{
				print_char( ch, "%s rozb³yskuje przez chwilê jasnym ¶wiat³em i zmienia siê w mandolinê!\n\r", capitalize( obj->short_descr ) );
				obj->value[ 0 ] = INSTRUMENT_MANDOLIN;
				obj->name = str_dup( "mandolina" );
				obj->name2 = str_dup( "mandoliny" );
				obj->name3 = str_dup( "mandolinê" );
				obj->name4 = str_dup( "mandolinê" );
				obj->name5 = str_dup( "mandolin±" );
				obj->name6 = str_dup( "mandolinie" );
				obj->short_descr = str_dup( "mandolina" );
				obj->description = str_dup( "Mandolina le¿y tutaj." );
				obj->item_description = str_dup( "Jest to mandolina, instrument strunowy szarpany, troche wiêkszy od lutni. Ma korpus rezonansowy o przekroju gruszkowatym i wypuk³ym spodzie, oraz szyjkê z metalowymi progami, zakoñczon± odchylon± do ty³u desk± ko³ow±." );
				obj->ident_description = str_dup( "Czujesz, ¿e materia, z której stworzona zosta³a ta mandolina, powsta³a z magii. Wydaje ci siê, ¿e pod jej pow³ok± dostrzegasz wizerunek jakiego¶ innego instrumentu." );
				obj->hidden_description = str_dup( "Nie widzisz nic specjalnego w mandolinie." );
			}
			else
			{
			print_char( ch, "%s rozb³yskuje przez chwilê jasnym ¶wiat³em i zmienia siê w ma³y flet!\n\r", capitalize( obj->short_descr ) );
			obj->value[ 0 ] = INSTRUMENT_FLUTE;
			obj->name = str_dup( "ma³y flet" );
			obj->name2 = str_dup( "ma³ego fletu" );
			obj->name3 = str_dup( "ma³emu fletowi" );
			obj->name4 = str_dup( "ma³y flet" );
			obj->name5 = str_dup( "ma³ym fletem" );
			obj->name6 = str_dup( "ma³ym flecie" );
			obj->short_descr = str_dup( "ma³y flet" );
			obj->description = str_dup( "Niedu¿y flecik pod³u¿ny le¿y tutaj." );
			obj->item_description = str_dup( "Jest to ma³y flet pod³u¿ny, czyli dêty instrument przypominaj±cy trochê szeroki patyk z dziur± wewn±trz i niewielkimi otworami na górze. U¿ywany jest zwykle w muzyce solowej oraz w orkiestrze do realizacji najwy¿szego g³osu." );
			obj->ident_description = str_dup( "Czujesz, ¿e materia, z której stworzony zosta³ ten flet, powsta³a z magii. Wydaje ci siê, ¿e pod jego pow³ok± dostrzegasz wizerunek jakiego¶ innego instrumentu." );
			obj->hidden_description = str_dup( "Nie widzisz nic specjalnego w ma³ym flecie." );
			break;
			}
			break;
		case INSTRUMENT_HARP:
			if ( dice( 1, 2 ) == 1 )
			{
				print_char( ch, "%s rozb³yskuje przez chwilê jasnym ¶wiat³em i zmienia siê w mandolinê!\n\r", capitalize( obj->short_descr ) );
				obj->value[ 0 ] = INSTRUMENT_MANDOLIN;
				obj->name = str_dup( "mandolina" );
				obj->name2 = str_dup( "mandoliny" );
				obj->name3 = str_dup( "mandolinê" );
				obj->name4 = str_dup( "mandolinê" );
				obj->name5 = str_dup( "mandolin±" );
				obj->name6 = str_dup( "mandolinie" );
				obj->short_descr = str_dup( "mandolina" );
				obj->description = str_dup( "Mandolina le¿y tutaj." );
				obj->item_description = str_dup( "Jest to mandolina, instrument strunowy szarpany, troche wiêkszy od lutni. Ma korpus rezonansowy o przekroju gruszkowatym i wypuk³ym spodzie, oraz szyjkê z metalowymi progami, zakoñczon± odchylon± do ty³u desk± ko³ow±." );
				obj->ident_description = str_dup( "Czujesz, ¿e materia, z której stworzona zosta³a ta mandolina, powsta³a z magii. Wydaje ci siê, ¿e pod jej pow³ok± dostrzegasz wizerunek jakiego¶ innego instrumentu." );
				obj->hidden_description = str_dup( "Nie widzisz nic specjalnego w mandolinie." );
			}
			else
			{
				print_char( ch, "%s rozb³yskuje przez chwilê jasnym ¶wiat³em i zmienia siê w piêkn± lutniê!\n\r", capitalize( obj->short_descr ) );
				obj->value[ 0 ] = INSTRUMENT_LUTE;
				obj->name = str_dup( "piêkna lutnia" );
				obj->name2 = str_dup( "piêknej lutni" );
				obj->name3 = str_dup( "piêknej lutni" );
				obj->name4 = str_dup( "piêkn± lutniê" );
				obj->name5 = str_dup( "piêkn± lutni±" );
				obj->name6 = str_dup( "piêknej lutni" );
				obj->short_descr = str_dup( "piêkna lutnia" );
				obj->description = str_dup( "Piêkna lutnia le¿y tutaj." );
				obj->item_description = str_dup( "Jest to lutnia, instrument strunowy szarpany, posiada mocno wypuk³e pud³o rezonansowe o przekroju gruszkowym, szyjkê z progami zakoñczon± wygiêt± do ty³u g³ówk± z kó³kami do naci±gu strun." );
				obj->ident_description = str_dup( "Czujesz, ¿e materia, z której stworzona zosta³a ta lutnia, powsta³a z magii. Wydaje ci siê, ¿e pod jej pow³ok± dostrzegasz wizerunek jakiego¶ innego instrumentu." );
				obj->hidden_description = str_dup( "Nie widzisz nic specjalnego w piêknej lutni." );
			}
			break;
		case INSTRUMENT_MANDOLIN:
			if ( dice( 1, 2 ) == 1 )
			{
				print_char( ch, "%s rozb³yskuje przez chwilê jasnym ¶wiat³em i zmienia siê w harfê!\n\r", capitalize( obj->short_descr ) );
				obj->value[ 0 ] = INSTRUMENT_HARP;
				obj->name = str_dup( "harfa" );
				obj->name2 = str_dup( "harfy" );
				obj->name3 = str_dup( "harfie" );
				obj->name4 = str_dup( "harfê" );
				obj->name5 = str_dup( "harf±" );
				obj->name6 = str_dup( "harfie" );
				obj->short_descr = str_dup( "harfa" );
				obj->description = str_dup( "Harfa le¿y tutaj." );
				obj->item_description = str_dup( "Jest to harfa, instrument strunowy szarpany, posiada kszta³t trójk±ta - jeden bok stanowi rozszerzaj±ce siê ku do³owi pud³o rezonansowe, które harfista opiera o ramiê, górn± czê¶æ - wygiêta fali¶cie rama." );
				obj->ident_description = str_dup( "Czujesz, ¿e materia, z której stworzona zosta³a ta harfa, powsta³a z magii. Wydaje ci siê, ¿e pod jej pow³ok± dostrzegasz wizerunek jakiego¶ innego instrumentu." );
				obj->hidden_description = str_dup( "Nie widzisz nic specjalnego w harfie." );
			}
			else
			{
				print_char( ch, "%s rozb³yskuje przez chwilê jasnym ¶wiat³em i zmienia siê w piêkn± lutniê!\n\r", capitalize( obj->short_descr ) );
				obj->value[ 0 ] = INSTRUMENT_LUTE;
				obj->name = str_dup( "piêkna lutnia" );
				obj->name2 = str_dup( "piêknej lutni" );
				obj->name3 = str_dup( "piêknej lutni" );
				obj->name4 = str_dup( "piêkn± lutniê" );
				obj->name5 = str_dup( "piêkn± lutni±" );
				obj->name6 = str_dup( "piêknej lutni" );
				obj->short_descr = str_dup( "piêkna lutnia" );
				obj->description = str_dup( "Piêkna lutnia le¿y tutaj." );
				obj->item_description = str_dup( "Jest to lutnia, instrument strunowy szarpany, posiada mocno wypuk³e pud³o rezonansowe o przekroju gruszkowym, szyjkê z progami zakoñczon± wygiêt± do ty³u g³ówk± z kó³kami do naci±gu strun." );
				obj->ident_description = str_dup( "Czujesz, ¿e materia, z której stworzona zosta³a ta lutnia, powsta³a z magii. Wydaje ci siê, ¿e pod jej pow³ok± dostrzegasz wizerunek jakiego¶ innego instrumentu." );
				obj->hidden_description = str_dup( "Nie widzisz nic specjalnego w piêknej lutni." );
			}
			break;
		default:
			print_char( ch, "%s rozb³yskuje przez chwilê jasnym ¶wiat³em, ale nic siê nie dzieje.\n\r", capitalize( obj->short_descr ) );
			return;
			break;

	}

	obj->value[ 1 ] = UMAX( 1, obj->value[ 1 ] - URANGE( 1, ( 100 - ( 3 * wis + 3 * level ) ), 50 ) );
	obj->value[ 2 ] = UMAX( 1, obj->value[ 2 ] - URANGE( 1, ( 100 - ( 3 * intt + 3 * level ) ), 50 ) );
	EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );
	return;
}

void spell_floating_skull( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj2;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), intt = get_curr_stat_deprecated( ch, STAT_INT );
	int timer;
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	MOB_INDEX_DATA *mob = get_mob_index( obj->value[ 0 ] );
	char buf[ MAX_STRING_LENGTH ];

	if ( obj == NULL )
	{
		send_to_char( "Nie widzisz tu nic takiego.\n\r", ch );
		return;
	}

	if ( obj->item_type == ITEM_CORPSE_PC )
	{
		print_char( ch, "Co¶ jednak powstrzymywa³oby ciê przed tym barbarzyñskim ruchem.\n\r" );
		return;
	}

	if ( obj->item_type != ITEM_CORPSE_NPC )
	{
		send_to_char( "Czar nie zadzia³a³.\n\r", ch );
		return;
	}

	if ( !mob || !IS_SET( mob->parts, PART_HEAD ) || EXT_IS_SET( obj->extra_flags, ITEM_HIGH_RENT ) )
	{
		print_char( ch, "Chyba nie uda ci siê z tego stworzyæ lataj±cej czaszki.\n\r" );
		return;
	}

	if ( get_eq_char( ch, WEAR_FLOAT ) )
	{
		sprintf( buf, "Czaszka %s rozp³ywa siê w powietrzu.\n\r", race_table[ mob->race ].name2 );
		send_to_char( buf, ch );
		return;
	}

	if ( !IS_NPC( ch ) && number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "W ostatniej chwili rozpraszasz swoj± uwagê.\n\r", ch );
		return;
	}

	EXT_SET_BIT( obj->extra_flags, ITEM_HIGH_RENT ); //ustawianie ciala zastepczej,
	//nieuzywanej dla cial flagi. Highrent robi za 'czy ma jeszcze glowe' bez
	//wymyslania nowych extra:P

	timer = 12 + level / 10 + ( intt - 19 );
	if ( number_range( 0, luck ) > 15 ) timer += 2;
	if ( number_range( 0, luck ) < 5 ) timer -= 3;

	obj2 = create_object( get_obj_index( OBJ_VNUM_FLOATING_SKULL ), FALSE );
	obj_to_char( obj2, ch );
	equip_char( ch, obj2, WEAR_FLOAT, TRUE );
	obj2->timer = UMAX( 1, timer );

	/* name */
	sprintf( buf, obj2->name, race_table[ mob->race ].name2 );
	free_string( obj2->name );
	obj2->name = str_dup( buf );
	/* short */
	sprintf( buf, obj2->short_descr, race_table[ mob->race ].name2 );
	free_string( obj2->short_descr );
	obj2->short_descr = str_dup( buf );
	/* long */
	sprintf( buf, obj2->description, race_table[ mob->race ].name2 );
	free_string( obj2->description );
	obj2->description = str_dup( buf );
	/* desc */
	sprintf( buf, obj2->item_description, race_table[ mob->race ].name2 );
	free_string( obj2->item_description );
	obj2->item_description = str_dup( buf );

	if ( level > 24 && mob->level > 19 && dice( 2, ( intt - 15 ) ) > 4 )
		obj2->value[ 0 ] = 1;

	act( "Nad ramieniem $z zaczyna lewitowaæ groteskowa czaszka.", ch, obj2, NULL, TO_ROOM );
	act( "Nad twoim ramieniem zaczyna lewitowaæ groteskowa czaszka.", ch, obj2, NULL, TO_CHAR );
	return;
}

void spell_hallucinations( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, vluck, dur, mod, intt;

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );
	intt = get_curr_stat_deprecated( victim, STAT_INT );

	if ( is_safe( ch, victim ) )
		return;

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 ) //psucie z lucka
	{
		if ( victim != ch )
		{
			act( "W ostatniej chwili dekoncentrujesz siê i nie udaje ci siê sprowadziæ halucynacji na $C.", ch, NULL, victim, TO_CHAR );
			if ( IS_NPC ( victim ) && can_see( victim, ch ) && can_move( victim ) )
				multi_hit( victim, ch, TYPE_UNDEFINED );
		}
		else
		{
			send_to_char( "W ostatniej chwili dekoncentrujesz siê i nie udaje ci siê sprowadziæ na siebie halucynacji.\n\r", ch );
		}
		return;
	}

	if ( is_affected( victim, gsn_hallucinations ) )
	{
		if ( victim != ch )
		{
			act( "$X dokuczaj± ju¿ halucynacje.", ch, NULL, victim, TO_CHAR );
			if ( IS_NPC ( victim ) && can_see( victim, ch ) && can_move( victim ) )
				multi_hit( victim, ch, TYPE_UNDEFINED );
		}
		else
		{
			send_to_char( "Przecie¿ ju¿ dokuczaj± ci halucynacje.\n\r", ch );
		}
		return;
	}

	//jak rzuca na siebie nei ma sejwow
	if ( victim != ch )
	{
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) || ( victim->resists[ RESIST_FEAR ] > 0 && number_percent() < victim->resists[ RESIST_FEAR ] ) )
		{
			act( "$N patrzy na ciebie z politowaniem.", ch, NULL, victim, TO_CHAR );
			act( "Patrzysz z politowaniem na $c.", ch, NULL, victim, TO_VICT );
			act( "$N patrzy na $c z politowaniem.", ch, NULL, victim, TO_NOTVICT );
			if ( IS_NPC ( victim ) && can_see( victim, ch ) && can_move( victim ) && victim->fighting == NULL )
				multi_hit( victim, ch, TYPE_UNDEFINED );
			return;
		}
	}

	if ( number_range( 1, 2 ) == 2 )  //Kulanie czy positive czy negative
	{
		/*positive - koles ma dola, lepiej trafia, ma rozne ciekawe komunikaty
		oraz o 5% zwiekszona szanse na trafienie krytyczne, o polowe
		trudniej takiemu uciekac, szansa na mini-frenzy*/

		dur = level / 5;
		if ( dice( 2, intt - 20 ) > 5 ) ++dur;
		if ( dice( 2, intt - 18 ) == 2 ) --dur;
		mod = level / 6;

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = dur;
	af.rt_duration = 0;
		af.location = APPLY_HITROLL;
		af.modifier = mod;
		af.bitvector = &AFF_HALLUCINATIONS_POSITIVE;
		affect_to_char( victim, &af, NULL, TRUE );

		af.where = TO_AFFECTS;
		af.level = 100; //koles na 'dole' niczego sie nie boi, zgine? etam, no to co:P
		af.location = APPLY_RESIST;
		af.modifier = RESIST_FEAR;
		af.bitvector = &AFF_BRAVE_CLOAK; //do progow if affected
		affect_to_char( victim, &af, NULL, TRUE );

		if ( dice( 2, intt - 19 ) > 5 ) //szansa na jakby mini-frenzy
		{
			af.where = TO_AFFECTS;
			af.type = sn;
			af.level = level;
			af.location = APPLY_DAMROLL;
			af.modifier = 1;
			affect_to_char( victim, &af, NULL, TRUE );

			af.where = TO_AFFECTS;
			af.type = sn;
			af.level = level;
			af.location = APPLY_AC;
			af.modifier = 20;
			affect_to_char( victim, &af, NULL, TRUE );

		}
		if ( ch != victim )
		{
			act( "$N spuszcza mêtny ju¿ wzrok i wzdycha ciê¿ko.", ch, NULL, victim, TO_CHAR );
			act( "¦wiat dooko³a traci barwy, ogarnia ciê smutek i przygnêbienie.", ch, NULL, victim, TO_VICT );
		}
		else
		{
			act( "¦wiat dooko³a traci barwy, ogarnia ciê smutek i przygnêbienie.", ch, NULL, victim, TO_CHAR );
		}
		act( "$N spuszcza mêtny ju¿ wzrok i wzdycha ciê¿ko.", ch, NULL, victim, TO_NOTVICT );
		if ( IS_NPC ( victim ) && can_see( victim, ch ) && can_move( victim ) && victim->fighting == NULL )
			multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	else
	{
		/*i negative, koles ma faze, minus do hitrolla, szansa na odwrotnosc
		mini frenzy - skacze sobie wkolko jak najebany*/

		dur = level / 5;
		if ( dice( 2, intt - 20 ) > 5 ) ++dur;
		if ( dice( 2, intt - 18 ) == 2 ) --dur;
		mod = level / 10;

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = dur;
	af.rt_duration = 0;
		af.location = APPLY_HITROLL;
		af.modifier = -mod;
		af.bitvector = &AFF_HALLUCINATIONS_NEGATIVE;
		affect_to_char( victim, &af, NULL, TRUE );

		af.where = TO_AFFECTS;
		af.level = 100; //koles na 'fazie' niczego sie nie boi - nie wie co sie dzieje:P
		af.location = APPLY_RESIST;
		af.modifier = RESIST_FEAR;
		af.bitvector = &AFF_BRAVE_CLOAK; //do progow if affected
		affect_to_char( victim, &af, NULL, TRUE );

		if ( dice( 2, intt - 19 ) > 5 )
		{
			af.where = TO_AFFECTS;
			af.type = sn;
			af.level = level;
			af.location = APPLY_DAMROLL;
			af.modifier = -2;
			affect_to_char( victim, &af, NULL, TRUE );

			af.where = TO_AFFECTS;
			af.type = sn;
			af.level = level;
			af.location = APPLY_AC;
			af.modifier = -10;
			affect_to_char( victim, &af, NULL, TRUE );

		}
		if ( ch != victim )
		{
			act( "$N rozgl±da siê dooko³a, g³upawo siê u¶miechaj±c.", ch, NULL, victim, TO_CHAR );
			act( "Nagle ¶wiat doko³a ciebie rozb³yskuje têczowymi kolorami! Jak piêknie! Jak wspaniale!", ch, NULL, victim, TO_VICT );
		}
		else
		{
			act( "Nagle ¶wiat doko³a ciebie rozb³yskuje têczowymi kolorami! Jak piêknie! Jak wspaniale!", ch, NULL, victim, TO_CHAR );
		}
		act( "$N rozgl±da siê dooko³a, g³upawo siê u¶miechaj±c.", ch, NULL, victim, TO_NOTVICT );
		if ( IS_NPC ( victim ) && can_see( victim, ch ) && can_move( victim ) && victim->fighting == NULL )
			multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	return;
}

/* Swoboda dzialania (skladnia: cast 'free action' [kto])
 *
 * odpornosc na opetanie i holda (opetania jeszcze nie ma)
 *
 */

void spell_free_action( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), dur;

	if ( IS_AFFECTED( victim, AFF_FREE_ACTION ) )
	{
		if (ch == victim)
		{
			send_to_char( "Jeste¶ ju¿ pod wp³ywem tego czaru.\n\r", ch );
		}
		else
		{
			act( "$N znajduje siê ju¿ pod wp³ywem tego czaru.", ch, NULL, victim, TO_CHAR );
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	dur = 2 + level / 10;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 3 + level / 10;
		}
	}

	if ( number_range( 0, luck ) > 10 ) ++dur;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_FREE_ACTION;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Rzucony przez $z czar powoduje, ¿e czujesz siê nieco swobodniej.", ch, NULL, victim, TO_VICT );
	if ( victim == ch )
		send_to_char( "Czujesz siê nieco swobodniej!\n\r", victim );
	else
		act( "Uda³o ci siê rzuciæ zaklêcie na $Z.", ch, NULL, victim, TO_CHAR );

	return;
}

void spell_dismiss_animal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * vmaster;
	int chance, hp_bonus;

	DEBUG_INFO( "do_cast:spell_dismiss_animal:1");

	if ( victim == NULL )
	{
		send_to_char( "Kogo chcesz odes³aæ?", ch);
		return;
	}
	else if ( victim == ch )
	{
		send_to_char( "No co ty, sam<&/a/o> siebie chcesz odes³aæ?", ch);
		return;
	}

	DEBUG_INFO( "do_cast:spell_dismiss_animal:2");

	vmaster = victim->master;

	if ( vmaster == NULL )
		vmaster = victim;

	if ( !IS_SET( race_table[ victim->race ].type, ANIMAL ) )
	{
		act( "$N nie jest zwierzêciem.", ch, NULL, victim, TO_CHAR );
		return;
	}
	if ( !EXT_IS_SET( victim->act, ACT_NO_EXP ) || !IS_NPC( victim ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N nie zosta³o przywo³ane.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N nie zosta³ przywo³any.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N nie zosta³a przywo³ana.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	if ( vmaster == ch )
	{
		chance = URANGE(80,80+number_range(1,15),95);
	}
	else
	{
		chance = 55;
		chance += ( get_curr_stat_deprecated( ch, STAT_LUC ) - 10 ) / 2;
		chance -= ( get_curr_stat_deprecated( victim, STAT_LUC ) - 10 ) / 2;
		chance += level * 4;
		chance -= victim->level / 2;
		chance -= vmaster->level * 3;
		chance -= get_curr_stat_deprecated( vmaster, STAT_WIS ) - 10;
		chance += get_curr_stat_deprecated( ch, STAT_WIS ) - 10;
		hp_bonus = 50 - ( 100 * victim->hit / UMAX( get_max_hp(victim), 1 ) );
		chance += hp_bonus / 2;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance -= 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance -= 20;
		chance -= sector_table[ victim->in_room->sector_type ].memdru_bonus * 10;
		if ( !IS_NPC( ch ) )
		{
			if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
			{
				chance += chance/5;
			}
		}
		chance = URANGE( 5, chance, 75 );
	}

	if( ch->class == CLASS_DRUID )
	{
		act( "Koncentrujesz siê wzywaj±c moce natury by ciê wspomog³y, zamykasz oczy na chwilê po czym otwieraj±c je kierujesz moc w kierunku $Z.", ch, NULL, victim, TO_CHAR );
		act( "$n koncentruje siê przez chwilê po czym wymawia s³owa w dziwnym dialekcie. Zamyka na chwilê oczy po czym otwieraj±c je wypuszcza ze swych r±k promienie zielonej energii, które opl±tuj± $C.", ch, NULL, victim, TO_ROOM );

		if ( number_percent() < chance )
		{
			act( "$n znika w rozb³ysku ¶wiat³a!", victim, NULL, NULL, TO_ALL );
			extract_char( victim, TRUE );
		}
		else
			act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );
		return;
	}

	act( "Koncentruj±c siê przywo³ujesz wielkie, staro¿ytne moce, które kr±¿± wokó³ twej ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_CHAR );
	act( "$n przywo³uje wielkie, staro¿ytne moce, które kr±¿± wokó³ $s ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_ROOM );

	DEBUG_INFO( "do_cast:spell_dismiss_animal:4");

	if ( number_percent() < chance )
	{
		act( "W nag³ym rozb³ysku ¶wiat³a $n znika.", victim, NULL, NULL, TO_ALL );
		extract_char( victim, TRUE );
	}
	else
	{
		act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );
	}

	return;
}

void spell_dismiss_plant( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * vmaster;
	int chance, hp_bonus;

	DEBUG_INFO( "do_cast:spell_dismiss_plant:1");

	if ( victim == NULL )
	{
		send_to_char( "Kogo chcesz odes³aæ?", ch);
		return;
	}
	else if ( victim == ch )
	{
		send_to_char( "No co ty, sam<&/a/o> siebie chcesz odes³aæ?", ch);
		return;
	}

	DEBUG_INFO( "do_cast:spell_dismiss_plant:2");

	vmaster = victim->master;

	if ( vmaster == NULL )
		vmaster = victim;

	if ( !IS_SET( race_table[ victim->race ].type, PLANT ) )
	{
		act( "$N nie jest ro¶lin±.", ch, NULL, victim, TO_CHAR );
		return;
	}
	if ( !EXT_IS_SET( victim->act, ACT_NO_EXP ) || !IS_NPC( victim ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N nie zosta³o przywo³ane.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N nie zosta³ przywo³any.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N nie zosta³a przywo³ana.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	if ( vmaster == ch )
	{
		chance = URANGE(80,80+number_range(1,15),95);
	}
	else
	{
		chance = 55;
		chance += ( get_curr_stat_deprecated( ch, STAT_LUC ) - 10 ) / 2;
		chance -= ( get_curr_stat_deprecated( victim, STAT_LUC ) - 10 ) / 2;
		chance += level * 4;
		chance -= victim->level / 2;
		chance -= vmaster->level * 3;
		chance -= get_curr_stat_deprecated( vmaster, STAT_WIS ) - 10;
		chance += get_curr_stat_deprecated( ch, STAT_WIS ) - 10;
		hp_bonus = 50 - ( 100 * victim->hit / UMAX( get_max_hp(victim), 1 ) );
		chance += hp_bonus / 2;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance -= 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance -= 20;
		chance -= sector_table[ victim->in_room->sector_type ].memdru_bonus * 10;
		if ( !IS_NPC( ch ) )
		{
			if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
			{
				chance += chance/5;
			}
		}
		chance = URANGE( 5, chance, 75 );
	}

	if( ch->class == CLASS_DRUID )
	{
		act( "Koncentrujesz siê wzywaj±c moce natury by ciê wspomog³y, zamykasz oczy na chwilê po czym otwieraj±c je kierujesz moc w kierunku $Z.", ch, NULL, victim, TO_CHAR );
		act( "$n koncentruje siê przez chwilê po czym wymawia s³owa w dziwnym dialekcie. Zamyka na chwilê oczy po czym otwieraj±c je wypuszcza ze swych r±k promienie zielonej energii, które opl±tuj± $C.", ch, NULL, victim, TO_ROOM );

		if ( number_percent() < chance )
		{
			act( "$n znika w rozb³ysku ¶wiat³a!", victim, NULL, NULL, TO_ALL );
			extract_char( victim, TRUE );
		}
		else
			act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );
		return;
	}

	act( "Koncentruj±c siê przywo³ujesz wielkie, staro¿ytne moce, które kr±¿± wokó³ twej ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_CHAR );
	act( "$n przywo³uje wielkie, staro¿ytne moce, które kr±¿± wokó³ $s ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_ROOM );

	DEBUG_INFO( "do_cast:spell_dismiss_plant:4");

	if ( number_percent() < chance )
	{
		act( "W nag³ym rozb³ysku ¶wiat³a $n znika.", victim, NULL, NULL, TO_ALL );
		extract_char( victim, TRUE );
	}
	else
	{
		act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );
	}

	return;
}

void spell_perfect_self( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int dur = 10, luck;

	if ( is_affected( ch, gsn_perfect_self ) )
	{
		act( "Ju¿ jeste¶ ukryt<&y/a/e> pod pow³ok± iluzji.", ch, NULL, NULL, TO_CHAR );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		act( "Nie uda³o ci siê utworzyæ iluzorycznej pow³oki.", ch, NULL, NULL, TO_CHAR );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_perfect_self;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	affect_to_char( ch, &af, NULL, TRUE );

	act( "Co¶ siê zmienia w wygl±dzie $z.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Zas³aniasz siê iluzjami.\n\r", ch );
	return;
}

void spell_detect_aggressive( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dur = 12 + level / 4;

	if ( IS_AFFECTED( victim, AFF_DETECT_AGGRESSIVE ) )
	{
		if ( victim == ch )
			send_to_char( "Dostrzegasz ju¿ przecie¿ nienawi¶æ wype³niaj±c± serca z³ych istot.\n\r", ch );
		else
			act( "$N dostrzega ju¿ nienawi¶æ wype³niaj±c± serca z³ych istot.", ch, NULL, victim, TO_CHAR );
		return;
	}

    /* Bonus dla specjalisty */

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur *= 2;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_DETECT_AGGRESSIVE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Czujesz, ¿e twoje oczy lekko zmieniaj± siê.\n\r", victim );
	act( "Oczy $z lekko zmieniaj± siê." , victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_changestaff( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * golem;
	OBJ_DATA *staff = ( OBJ_DATA * ) vo;
	AFFECT_DATA af;
	char buf[ MAX_STRING_LENGTH ];
	int luck = get_curr_stat( ch, STAT_LUC );
	int intt = get_curr_stat( ch, STAT_INT );
	int charisma = get_curr_stat( ch, STAT_CHA );

	// jak kto¶ nie chce byæ followany
	if ( EXT_IS_SET( ch->act, PLR_NOFOLLOW ) )
	{
		send_to_char( "Przecie¿ nie chcesz ¿eby kto¶ za tob± chodzi³.\n\r", ch );
		return;
	}

	if ( staff->item_type != ITEM_WEAPON )
	{
		send_to_char( "To nie jest laska.\n\r", ch );
		return;
	}

	if ( staff->value[ 0 ] != WEAPON_STAFF && staff->value[ 0 ] != WEAPON_SPEAR && staff->value[ 0 ] != WEAPON_POLEARM && !IS_OBJ_STAT(staff, ITEM_UNDESTRUCTABLE) )
	{
		send_to_char( "Tej broni nie da siê o¿ywiæ.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê tego o¿ywiæ.\n\r", ch );
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

	if ( number_range( 1, intt/6 + level ) < 10 ) //Raszer , zmniejszenie szansy na nieudane rzucenie czaru
	{
		send_to_char( "W ostatniej chwili mylisz siê i kula materii ulega dezintegracji.\n\r", ch );
		act( "Kula materii przez chwilê migocze, po czym znika bez ¶ladu.", ch, NULL, NULL, TO_ROOM );
		extract_obj( staff );
		return;
	}

	// jak kto¶ ma mniej ni¿ 12 charyzmy, to niech spada
	if ( charisma < 72 )
	{
		print_char( ch, "Z tak± charyzm± nigdy nie dasz rady zmusiæ golema by ci s³u¿y³.\n\r" );
		extract_obj( staff );
		return;
	}

    if ( staff->material == 45 )
    {
        print_char( ch, "Puff ....\n\r" );
        extract_obj( staff );
        return;
    }

	if ( number_range( 1, 650 ) == 1 )
	{
		extract_obj( staff );
		summon_malfunction( ch, sn );
		return;
	}

	golem = create_mobile( get_mob_index( MOB_VNUM_GOLEM ) );
	char_to_room( golem, ch->in_room );
	EXT_SET_BIT( golem->act, ACT_NO_EXP );

	sprintf( buf, golem->name, material_table[ staff->material ].adjective );
	free_string( golem->name );
	golem->name = str_dup( buf );

	sprintf( buf, golem->short_descr, material_table[ staff->material ].adjective );
	free_string( golem->short_descr );
	golem->short_descr = str_dup( buf );

	sprintf( buf, golem->long_descr, material_table[ staff->material ].adjective );
	free_string( golem->long_descr );
	golem->long_descr = str_dup( buf );

	sprintf( buf, golem->description, material_table[ staff->material ].adjective );
	free_string( golem->description );
	golem->description = str_dup( buf );

    money_reset_character_money ( golem );
	golem->level = level / 3 + dice( staff->value[ 1 ], staff->value[ 2 ] ) + staff->value[ 6 ];
	golem->hit = 10 + number_range(18,24) * golem->level;
	golem->max_hit = golem->hit;
	golem->damage[ DICE_NUMBER ] = staff->value[ 1 ];
	golem->damage[ DICE_TYPE ] = staff->value[ 2 ];
	golem->damage[ DICE_BONUS ] = staff->value[ 6 ];
	golem->hitroll = staff->value[ 5 ] + level / 5;

    /**
     * afekty sa niezalezne. moze ich byc wicej
     */
    golem->attack_flags = 0;
    if
        (
         IS_WEAPON_STAT( staff, WEAPON_TOXIC )
         || IS_WEAPON_STAT( staff, WEAPON_POISON )
        )
        {
            ALT_FLAGVALUE_TOGGLE( golem->attack_flags, weapon_type2, "poison" );
        }
    if ( IS_WEAPON_STAT( staff, WEAPON_FLAMING ) )
    {
        ALT_FLAGVALUE_TOGGLE( golem->attack_flags, weapon_type2, "flaming" );
    }
    if ( IS_WEAPON_STAT( staff, WEAPON_FROST ) )
    {
        ALT_FLAGVALUE_TOGGLE( golem->attack_flags, weapon_type2, "frost" );
    }
    if ( IS_WEAPON_STAT( staff, WEAPON_SHOCKING ) )
    {
        ALT_FLAGVALUE_TOGGLE( golem->attack_flags, weapon_type2, "shocking" );
    }
    if ( IS_WEAPON_STAT( staff, WEAPON_VAMPIRIC ) )
    {
        ALT_FLAGVALUE_TOGGLE( golem->attack_flags, weapon_type2, "vampiric" );
    }

    /**
     * umagicznienie ataku
     */
    if( IS_OBJ_STAT( staff, ITEM_MAGIC ) )
    {
        golem->magical_damage = UMIN( staff->value[5], staff->value[6] );
        if ( IS_WEAPON_STAT( staff, WEAPON_VORPAL ) )
        {
            ++golem->magical_damage;
        }
    }
    /**
     * maks +4
     */
    golem->magical_damage = UMIN( golem->magical_damage, 4 );

	switch( staff->material )
	{
		case 0://do dupy
		case 6:
		case 12:
		case 17:
		case 18:
		case 20:
		case 24:
		case 25:
		case 26:
		case 29:
		case 30:
		case 35:
		case 37:
		case 38:
		case 46:
		case 47:
        case 51: // pierze
			golem->level = (golem->level*2)/3;
			golem->max_hit /= 2;
			--golem->damage[ DICE_NUMBER ];
			golem->damage[ DICE_TYPE ] /= 2;
			golem->damage[ DICE_BONUS ] /= 2;
			golem->hitroll /= 2;
			break;
		case 1://prawie do dupy
		case 11:
		case 15:
		case 19:
		case 31:
		case 39:
		case 44:
        case 50: // bursztyn
			golem->level = (golem->level*4)/5;
			--golem->damage[ DICE_TYPE ];
			--golem->damage[ DICE_BONUS ];
			--golem->hitroll;
			break;
		case 2://decent
		case 5:
		case 9:
		case 10:
		case 13:
		case 14:
		case 16:
		case 21:
		case 28:
		case 34:
		case 40:
		case 43:
		case 48: // drewno debowe
        case 52: // luska
		default:
			golem->max_hit += golem->hit/10;
			break;
		case 3://fine
		case 4:
		case 22:
		case 23:
		case 27:
		case 33:
			++golem->level;
			golem->max_hit += golem->hit/6 + 20;
			++golem->damage[ DICE_TYPE ];
			++golem->damage[ DICE_BONUS ];
			++golem->hitroll;
			break;
            /**
             * great
             */
		case  7: // adamantyt
		case  8: // mithril
		case 32: // damasceñska stal
        case 49: // stalodrzew
			golem->level += 3;
			golem->max_hit += golem->hit/5 + 25;
			++golem->damage[ DICE_NUMBER ];
			++golem->damage[ DICE_TYPE ];
			golem->damage[ DICE_BONUS ] += 2;
			golem->hitroll += 2;
			break;
            /**
             * super great
             */
		case 36: // diament
		case 41: // smocze luski
		case 42: // smoczy zab
			golem->level += 6;
			golem->max_hit += golem->hit/3 + 50;
			++golem->damage[ DICE_NUMBER ];
			golem->damage[ DICE_TYPE ] += 2;
			golem->damage[ DICE_BONUS ] += 4;
			golem->hitroll += 4;
            ++golem->magical_damage;
			break;
	}

	//Agron: dopalka przy udanym rzucie na int+char i poziom, przy sumie wiekszej od 260
	if( number_range(65,100) < (intt+charisma+ch->level)/4 )
	{
		golem->max_hit += number_range( 20,80); ;
		golem->damage[ DICE_BONUS ] += number_range(1,2);
		golem->hitroll += 1;
	}

	golem->hit = get_max_hp(golem);

	if ( material_table[ staff->material ].flag == MAT_METAL )
	{
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = material_table[ staff->material ].cold_res;
		af.duration = -1; af.rt_duration = 0;
		af.location = APPLY_RESIST;
		af.modifier = RESIST_COLD;
		af.bitvector = &AFF_RESIST_COLD;
		affect_to_char( golem, &af, NULL, FALSE );

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = -100;
		af.duration = -1; af.rt_duration = 0;
		af.location = APPLY_RESIST;
		af.modifier = RESIST_ELECTRICITY;
		af.bitvector = &AFF_RESIST_LIGHTNING;
		affect_to_char( golem, &af, NULL, FALSE );

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = material_table[ staff->material ].fire_res;
		af.duration = -1; af.rt_duration = 0;
		af.location = APPLY_RESIST;
		af.modifier = RESIST_FIRE;
		af.bitvector = &AFF_RESIST_FIRE;
		affect_to_char( golem, &af, NULL, FALSE );

        /**
         * PIERCE
         */
        af.where = TO_AFFECTS;
		af.type = sn;
		af.level = UMAX( 1, material_table[ staff->material ].hardness / 10 );
		af.duration = -1; af.rt_duration = 0;
		af.location = APPLY_RESIST;
		af.modifier = RESIST_PIERCE;
		af.bitvector = &AFF_NONE;
		affect_to_char( golem, &af, NULL, FALSE );
	}
	else if ( material_table[ staff->material ].flag == MAT_EASYBREAK )
	{
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = -35;
		af.duration = -1; af.rt_duration = 0;
		af.location = APPLY_RESIST;
		af.modifier = RESIST_BASH;
		af.bitvector = &AFF_NONE;
		affect_to_char( golem, &af, NULL, FALSE );

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = -10;
		af.duration = -1; af.rt_duration = 0;
		af.location = APPLY_RESIST;
		af.modifier = RESIST_SLASH;
		af.bitvector = &AFF_NONE;
		affect_to_char( golem, &af, NULL, FALSE );

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = -10;
		af.duration = -1; af.rt_duration = 0;
		af.location = APPLY_RESIST;
		af.modifier = RESIST_PIERCE;
		af.bitvector = &AFF_NONE;
		affect_to_char( golem, &af, NULL, FALSE );
	}
    /**
     * latwopalne
     */
    else if ( material_table[ staff->material ].fire_res == 0 )
	{
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = -100;
		af.duration = -1; af.rt_duration = 0;
		af.location = APPLY_RESIST;
		af.modifier = RESIST_FIRE;
		af.bitvector = &AFF_RESIST_FIRE;
		affect_to_char( golem, &af, NULL, FALSE );

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = -30;
		af.duration = -1; af.rt_duration = 0;
		af.location = APPLY_RESIST;
		af.modifier = RESIST_ACID;
		af.bitvector = &AFF_RESIST_ACID;
		affect_to_char( golem, &af, NULL, FALSE );
	}

	if ( !add_charm( ch, golem, TRUE ) )
	{
		extract_char( golem, TRUE );
		return;
	}

	act( "Z tej kulistej materii formuje siê $N i zaczyna s³u¿yæ $x.", ch, staff, golem, TO_ROOM );
	act( "Z tej kulistej materii formuje siê $N i zaczyna ci s³u¿yæ.", ch, staff, golem, TO_CHAR );

	af.where = TO_AFFECTS;
	af.type = gsn_domination;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( golem, &af, NULL, TRUE );

	add_follower( golem, ch, FALSE );

	ch->counter[4] = 1;

	if ( ch->fighting )
    {
		set_fighting( golem, ch->fighting );
    }

//	create_event( EVENT_EXTRACT_CHAR, 480 * number_range( ( level / 2 ), ( level / 2 ) + 6 ) , golem, NULL, 0 );
	extract_obj( staff );
	return;
}

/* Zamet (skladnia: cast confusion)
 *
 * no wiec kazdy z przeciwnikow dostaje affecta, ktory cos robi dopiero
 * w walce (traci atak, atakuje ch->next_in_room, atakuje siebie(czemu nie)
 * mag przestaje rzucac czar(?), odrzuca bron)
 */
void spell_confusion( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * vch;
	AFFECT_DATA af;
	int duration, mod = 0;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	bool success = FALSE;

	if( ch->fighting == NULL || ch->position != POS_FIGHTING )
	{
		send_to_char( "Ten czar mo¿esz rzuciæ tylko w trakcie walki.\n\r", ch );
		return;
	}

	duration = level / 5;
	if ( number_range( 0, luck ) > 15 ) ++duration;
	if ( number_range( 0, luck ) < 3 ) --duration;

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration += 3;
			mod = 10;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = get_curr_stat_deprecated( ch, STAT_WIS ) + mod;
	af.bitvector = &AFF_CONFUSION;

	for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
		if ( is_same_group( ch->fighting, vch ) && !IS_AFFECTED( vch, AFF_CONFUSION ) )
			if ( ch_vs_victim_stat_throw( ch, vch, STAT_INT ) && !saves_spell_new( vch, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) && number_percent() > vch->resists[ RESIST_MENTAL ] )
			{
				affect_to_char( vch, &af, NULL, TRUE );
				send_to_char( "Zaczynasz siê troszkê gubiæ.\n\r", vch );
				success = TRUE;
			}
	if ( success )
		send_to_char( "Siejesz zamêt w oddzia³ach wroga.\n\r", ch );
	else
		send_to_char( "Niczego nie osi±gne³<&e/a/o>¶.\n\r", ch );
	return;
}

void spell_animate_staff( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * snake;
	OBJ_DATA *staff = ( OBJ_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat( ch, STAT_LUC );
	int intt = get_curr_stat( ch, STAT_INT );
	int charisma = get_curr_stat( ch, STAT_CHA );
	// jak kto¶ nie chce byæ followany
	if ( EXT_IS_SET( ch->act, PLR_NOFOLLOW ) )
	{
		send_to_char( "Przecie¿ nie chcesz ¿eby kto¶ za tob± chodzi³.\n\r", ch );
		return;
	}

	if ( staff->item_type != ITEM_WEAPON )
	{
		send_to_char( "To nie jest laska.\n\r", ch );
		return;
	}

	if ( staff->value[ 0 ] != WEAPON_STAFF && staff->value[ 0 ] != WEAPON_SPEAR && staff->value[ 0 ] != WEAPON_POLEARM )
	{
		send_to_char( "Tej broni nie da siê o¿ywiæ.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê tego o¿ywiæ.\n\r", ch );
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

	if ( number_range( 1, intt/6 + level ) < 8 )
	{
		act( "W ostatniej chwili mylisz siê i $p ulega dezintegracji.\n\r", ch, staff, NULL, TO_CHAR );
		act( "$p przez chwilê migocze, po czym znika bez ¶ladu.", ch, staff, NULL, TO_ROOM );
		extract_obj( staff );
		return;
	}

    // Gurthg: 2007-09-05, zmieniam co nastêpuje:
    // szansa na potworka przed sprawdzaniem charyzmy, niech postaci które
    // ju¿ mia³y pecha (brak charyzmy, if ni¿ej) "szansê" na potwora
    if ( number_range( 1,1000 ) == 1 )
	{
		extract_obj( staff );
		summon_malfunction( ch, sn );
		return;
	}

	// jak kto¶ ma mniej ni¿ 10 charyzmy, to niech spada
	// if ( charisma < 10 )
    // Gurthg: 2007-09-05, zmieniam co nastêpuje:
	// jak kto¶ ma mniej ni¿ 14 charyzmy, to niech ca³o¶æ podlega fluktuacjom szans
    // a ie ma co pisaæ poni¿ej pewnych warto¶æi "spadaj", bo po co?
	if ( number_range( 48, 84 ) > charisma)
	{
		print_char( ch, "Tym razem nie dasz rady przekonaæ wê¿a do s³u¿by.\n\r" );
		extract_obj( staff );
		return;
	}

	snake = create_mobile( get_mob_index( MOB_VNUM_SNAKE ) );
	char_to_room( snake, ch->in_room );
	EXT_SET_BIT( snake->act, ACT_NO_EXP );

    money_reset_character_money ( snake );
	snake->level = URANGE( 2, dice( staff->value[ 1 ], staff->value[ 2 ] ) + staff->value[ 6 ], 8 );
	snake->hit = dice( 5 * snake->level, 5 );
	snake->max_hit = snake->hit;
	snake->hit = get_max_hp(snake);
	snake->damage[ DICE_NUMBER ] = staff->value[ 1 ];
	snake->damage[ DICE_TYPE ] = staff->value[ 2 ];
	snake->damage[ DICE_BONUS ] = staff->value[ 6 ];
	snake->hitroll = staff->value[ 5 ] + level / 5;

	if ( !add_charm( ch, snake, TRUE ) )
	{
		extract_char( snake, TRUE );
		return;
	}

	act( "Z $f formuje siê $N i zaczyna s³u¿yæ $x.", ch, staff, snake, TO_ROOM );
	act( "Z $f formuje siê $N i zaczyna ci s³u¿yæ.", ch, staff, snake, TO_CHAR );

	af.where = TO_AFFECTS;
	af.type = gsn_domination;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( snake, &af, NULL, TRUE );

	add_follower( snake, ch, FALSE );

	if ( ch->fighting )
    {
		set_fighting( snake, ch->fighting );
    }

	ch->counter[4] = 1;

	create_event( EVENT_EXTRACT_CHAR, 240 * number_range( level, level*2 )  , snake, NULL, 0 );
	extract_obj( staff );
	return;
}

void spell_force_field( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int luck = get_curr_stat( ch, STAT_LUC );
	int intt = get_curr_stat( ch, STAT_INT );
	int duration;

	if ( number_range( 0, LUCK_BASE_MOD + luck/6 ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ ochronnego klosza mocy.\n\r", ch );
		return;
	}

	duration = 2 + level / 10 + intt / 60;
	if ( number_range( 0, OLD_28_VALUE ) < luck ) ++duration;
	if ( number_range( 0, luck ) < 30 ) --duration;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_FORCE_FIELD;
	affect_to_char( ch, &af, NULL, TRUE );

	act( "Koñcz±c zaklêcie, $n opuszcza rêce a jego magiczna tarcza b³yska lekko i zastyga.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "W chwili gdy koñczysz budowê skorupy b³yska ona lekko i twardnieje, masz wra¿enie ¿e nic nie bêdzie ci ju¿ przeszkadzaæ.\n\r", ch );

	return;
}

void spell_confusion_shell( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), dur;
	AFFECT_DATA af;

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( is_affected( ch, sn ) || IS_AFFECTED( ch, AFF_CONFUSION_SHELL ) )
	{
		send_to_char( "Przecie¿ ju¿ otaczaj± ciê iluzje przera¿aj±cych potworów.\n\r", ch );
		return;
	}

	dur = 2 + level / 6;
	if ( dice( 2, luck ) > 24 ) dur += 2;
	else if ( dice( 2, luck ) > 19 ) ++dur;
	else if ( dice( 2, luck ) < 7 ) --dur;
	else dur -= 2;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_CONFUSION_SHELL;
	affect_to_char( ch, &af, NULL, TRUE );

	act( "Wokó³ $z pojawiaj± siê iluzje przera¿aj±cych potworów.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Tworzysz wokó³ siebie iluzje przera¿aj±cych potworów.\n\r", ch );
	return;
}

void spell_float( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, duration, af_level = level / 2;

	if ( IS_AFFECTED( victim, AFF_FLYING ) || IS_AFFECTED( victim, AFF_FLOAT ) )
	{
		if ( victim == ch )
			send_to_char( "Przecie¿ ju¿ unosisz siê w powietrzu.\n\r", ch );
		else
			act( "$N ju¿ siê unosi w powietrzu.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->mount )
	{
		if ( victim == ch )
			send_to_char( "Jak chcesz to zrobiæ bêd±c na wierzchowcu?\n\r", ch );
		else
			act( "$N siedzi na wierzchowcu, po co ma lataæ?", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->position != POS_FIGHTING && victim->position != POS_STANDING )
	{
		send_to_char( "Zaklêcie siê nie uda³o.\n\r", ch );
		return;
	}

	if ( get_carry_weight( victim ) > can_carry_w( victim ) )
	{
		if ( victim == ch )
			switch ( victim -> sex )
			{
				case 0:
					send_to_char( "Niestety po chwili opadasz, poniewa¿ jeste¶ zbyt mocno obci±¿one.\n\r", ch );
					break;
				case 1:
					send_to_char( "Niestety po chwili opadasz, poniewa¿ jeste¶ zbyt mocno obci±¿ony.\n\r", ch );
					break;
				default:
					send_to_char( "Niestety po chwili opadasz, poniewa¿ jeste¶ zbyt mocno obci±¿ona.\n\r", ch );
					break;
			}
		else
			switch ( victim -> sex )
			{
				case 0:
					act( "Po chwili $N opada na ziemiê, poniewa¿ jest zbyt mocno obci±¿one.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "Po chwili $N opada na ziemiê, poniewa¿ jest zbyt mocno obci±¿ony.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "Po chwili $N opada na ziemiê, poniewa¿ jest zbyt mocno obci±¿ona.", ch, NULL, victim, TO_CHAR );
					break;
			}
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	/* zaleznosc od szczescia */
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	duration = 3 + level / 3;

	if ( number_range( 0, luck ) > 15 ) duration++;
	if ( number_range( 0, luck ) < 5 ) duration--;

	/* Bonus dla specjalisty oraz ograniczenie dla niespecjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration = 4 + level / 2;
			af_level = level;
		}
		else
		{
			duration = UMIN( 11, duration );
		}
	}

	af.where = TO_AFFECTS;
	af.type = gsn_float;
	af.level = af_level;
	af.duration = duration; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_FLOAT;
	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Unosisz siê lekko nad ziemiê.\n\r", victim );
	act( "$n unosi siê lekko nad ziemiê.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_noble_look( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int dur = 2 + level / 5;

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ w³asnej urody.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ urody $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		if ( victim == ch )
			switch ( victim->sex )
			{
				case 0:
					send_to_char( "Jeste¶ ju¿ tak piêkne jak tylko siê da!\n\r", ch );
					break;
				case 1:
					send_to_char( "Jeste¶ ju¿ tak piêkny jak tylko siê da!\n\r", ch );
					break;
				default :
					send_to_char( "Jeste¶ ju¿ tak piêkna jak tylko siê da!\n\r", ch );
					break;
			}
		else
			send_to_char( "Uda³o ci siê rzuciæ zaklêcie.\n\r", ch );
		return;
	}

	mod = 1 + level / 10;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_CHA;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Czuj±c dzia³anie magii prostujesz siê dumnie i spogl±dasz na wszystkich z wy¿szo¶ci±.\n\r", victim );

	if ( victim->sex == SEX_FEMALE )
		act( "$n zaczyna siê zachowywaæ jako¶ inaczej, czujesz przed ni± dziwny respekt.", victim, NULL, NULL, TO_ROOM );
	else
		act( "$n zaczyna siê zachowywaæ jako¶ inaczej, czujesz przed nim dziwny respekt.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_resist_weapon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( ch, AFF_RESIST_MAGIC_WEAPON ) ||
	     IS_AFFECTED( ch, AFF_RESIST_NORMAL_WEAPON ) ||
	     IS_AFFECTED( ch, AFF_RESIST_WEAPON ) )
	{
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie, ale niczego ono nie zmienia.\n\r", ch );
		return;
	}

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ pola chroni±cego przed ka¿d± broni±.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 7;
	af.duration = 5; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_MAGIC_WEAPON;
	af.bitvector = &AFF_RESIST_WEAPON;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Otacza ciê pole chroni±ce przed ka¿d± broni±.\n\r", ch );
	return;
}

void spell_absolute_magic_protection( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int luck;
	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ kopu³y niewra¿liwo¶ci absolutnej.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_MINOR_GLOBE ) ||
	     IS_AFFECTED( ch, AFF_GLOBE ) ||
	     IS_AFFECTED( ch, AFF_MAJOR_GLOBE ) ||
	     IS_AFFECTED( ch, AFF_ABSOLUTE_MAGIC_PROTECTION ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 4 + level / 10; af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_ABSOLUTE_MAGIC_PROTECTION;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Wokó³ ciebie tworzy siê potê¿na kopu³a niewra¿liwo¶ci absolutnej.\n\r", ch );
	act( "Wokó³ $z tworzy siê potê¿na kopu³a niewra¿liwo¶ci absolutnej.", ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_mantle( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), dur;

	if ( IS_AFFECTED( ch, AFF_RESIST_MAGIC_WEAPON ) ||
	     IS_AFFECTED( ch, AFF_RESIST_NORMAL_WEAPON ) ||
	     IS_AFFECTED( ch, AFF_RESIST_WEAPON ) )
	{
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie, ale niczego ono nie zmienia.\n\r", ch );
		return;
	}

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ magicznej opoñczy.\n\r", ch );
		return;
	}

	if( ch->level < 26 )
		dur = 3 + ( level - 14 ) / 3.5;
	else
		dur = 3 + ( level - 14 ) / 4;

	if ( dice( 2, luck ) > 22 ) ++dur;
	if ( dice( 2, luck ) < 10 ) --dur;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 2;
	af.duration = URANGE( 2, dur, 9 ); af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_MAGIC_WEAPON;
	af.bitvector = &AFF_RESIST_WEAPON;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Otacza ciê magiczna opoñcza.\n\r", ch );
	act( "$n otacza siê magiczn± opoñcz±.", ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_major_globe( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int luck;
	/* luck */
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ sfery niewra¿liwo¶ci.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_MINOR_GLOBE ) ||
	     IS_AFFECTED( ch, AFF_GLOBE ) ||
	     IS_AFFECTED( ch, AFF_MAJOR_GLOBE ) ||
	     IS_AFFECTED( ch, AFF_ABSOLUTE_MAGIC_PROTECTION ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 10 + level / 6; af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_MAJOR_GLOBE;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Wokó³ ciebie tworzy siê pó³przezroczysta, pulsuj±ca bia³ym ¶wiat³em sfera.\n\r", ch );
	act( "Wokó³ $z tworzy siê pó³przezroczysta, pulsuj±ca bia³ym ¶wiat³em sfera.", ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_puppet_master( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo, *old_master;
	AFFECT_DATA af;
	int dur;
	char buficzek[ MAX_STRING_LENGTH ];

	if ( !IS_NPC( ch ) && IS_IMMORTAL( victim ) )
		return;

	if ( is_safe( ch, victim ) )
		return;

	if ( victim == ch || IS_AFFECTED( ch, AFF_CHARM ) )
	{
		send_to_char( "Nie mo¿esz rzuciæ tego czaru na siebie.\n\r", ch );
		return;
	}

	if ( !IS_AFFECTED( victim, AFF_CHARM )
	     || victim->master == NULL )
	{
		act( "$N nie s³u¿y przecie¿ nikomu.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->master == ch )
	{
		act( "$N przecie¿ ju¿ s³u¿y tobie.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if( level < LEVEL_HERO && IS_AFFECTED( victim, AFF_LOYALTY ))
	{
		if( victim->sex == SEX_FEMALE )
		{
			act("$N jest bardzo lojalna w stosunku do swego mistrza.", ch, NULL, victim, TO_CHAR );
		}
		else
		{
			act("$N jest bardzo lojalny w stosunku do swego mistrza.", ch, NULL, victim, TO_CHAR );
		}
		if( ch->sex == SEX_FEMALE )
		{
			act("$n chcia³a przej±æ kontrolê nad twym s³ug±.", ch, NULL, victim->master, TO_VICT );
		}
		else
		{
			act("$n chcia³ przej±æ kontrolê nad twym s³ug±.", ch, NULL, victim->master, TO_VICT );
		}
		return;
	}

	if ( level + 2 < victim->master->level
	     || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn )
	     || saves_spell_new( victim->master, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		sprintf( buficzek, "Szybkim ruchem kre¶lisz magiczne znaki w powietrzu i kierujesz je w stronê $Z. Po chwili zaciskasz d³oñ w piê¶æ i czujesz jak wola %s wypiera ciê z $S umys³u.", victim->master->name2 );
		act( buficzek, ch, NULL, victim, TO_CHAR );
		act( "Widzisz jak $n szybkim ruchem kre¶li magiczne znaki w powietrzu kierujac je w stronê $Z.", ch, NULL, victim, TO_ROOM );
		sprintf( buficzek, "Czujesz, jak $n stara siê przej±æ kontrolê nad twoim s³ug± - %s. Nie staraj±c siê o delikatno¶æ odpychasz jego zaklêcie swoj± wol±.", victim->name5 );
		act( buficzek, ch, NULL, victim->master, TO_VICT );
		if ( ch->sex == SEX_FEMALE )
			act( "Po chwili lekko chwieje siê ona na nogach, jakby otrzyma³a cios w g³owê.", ch, NULL, victim, TO_ROOM );
		else
			act( "Po chwili lekko chwieje siê on na nogach, jakby otrzyma³ cios w g³owê.", ch, NULL, victim, TO_ROOM );
		return;
	}

	old_master = victim->master;
	die_follower( victim, FALSE );

	if ( !can_see( victim, ch ) || victim->position == POS_SLEEPING )
	{
		if ( victim->position == POS_SLEEPING )
			act( "$N rzuca siê przez sen.", ch, NULL, victim, TO_ALL );
		else
			act( "$N rozgl±da siê dooko³a, ale nie widzi swojego nowego mistrza.", ch, NULL, victim, TO_ALL );
		return;
	}

	dur = 4 + level / 2 + URANGE( -5, get_curr_stat_deprecated( ch, STAT_INT ) - get_curr_stat_deprecated( old_master, STAT_INT ), 5 );

	sprintf( buficzek, "Szybkim ruchem kre¶lisz magiczne znaki w powietrzu i kierujesz je w stronê swego celu. Po chwili zaciskasz d³oñ w piê¶æ i i prze³amuj±c wolê %s przejmujesz kontrolê nad $V.", old_master->name2 );

	if ( !add_charm( ch, victim, FALSE ) )
		return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( victim, &af, NULL, TRUE );

	add_follower( victim, ch, TRUE );

	if ( IS_AFFECTED( victim, AFF_TROLL ) )
		victim->affected_by[ AFF_TROLL.bank ] ^= AFF_TROLL.vector;

	act( "$n przejmuje nad tob± kontrolê.", ch, NULL, victim, TO_VICT );
	act( buficzek, ch, NULL, victim, TO_CHAR );
	act( "Widzisz jak $n szybkim ruchem kre¶li magiczne znaki w powietrzu kierujac je w stronê $Z. Po chwili zaciska d³oñ w piê¶æ i u¶miecha siê z³owieszczo.", ch, NULL, victim, TO_NOTVICT );

	stop_fighting( ch, FALSE );
	stop_fighting( victim, FALSE );

	if ( IS_NPC( victim ) && EXT_IS_SET( victim->act, ACT_AGGRESSIVE ) )
		EXT_REMOVE_BIT( victim->act, ACT_AGGRESSIVE );

	//kasowanie prewaitów
	if ( victim->wait_char )
	{
		free_pwait_char( victim->wait_char );
		victim->wait_char = NULL;
		victim->wait = 2;
	}

	stop_hating( victim, ch, FALSE );
	stop_hunting( victim );
	stop_fearing( victim, ch, FALSE );
	stop_hating( ch, victim, FALSE );
	stop_hunting( ch );
	stop_fearing( ch, victim, FALSE );
	return;
}

void spell_dismiss_insect( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * vmaster;
	int chance, hp_bonus;

	if ( victim == NULL )
	{
		send_to_char( "Kogo chcesz odes³aæ?", ch);
		return;
	}
	else if ( victim == ch )
	{
		send_to_char( "No co ty, chyba nie jeste¶ owadem?", ch);
		return;
	}

	vmaster = victim->master;
	if( vmaster == NULL ) vmaster = victim;

	if ( !IS_SET( race_table[ victim->race ].type, INSECT ) )
	{
		act( "$N nie jest owadem.", ch, NULL, victim, TO_CHAR );
		return;
	}
	if ( !EXT_IS_SET( victim->act, ACT_NO_EXP ) || !IS_NPC( victim ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N nie zosta³o przywo³ane.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N nie zosta³ przywo³any.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N nie zosta³a przywo³ana.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	if ( vmaster == ch ) chance = 95;
	else
	{
		chance = 55;
		chance += ( get_curr_stat_deprecated( ch, STAT_LUC ) - 10 ) / 2;
		chance -= ( get_curr_stat_deprecated( victim, STAT_LUC ) - 10 ) / 2;
		chance += level * 4;
		chance -= victim->level / 2;
		chance -= vmaster->level * 3;
		chance -= get_curr_stat_deprecated( vmaster, STAT_WIS ) - 10;
		chance += get_curr_stat_deprecated( ch, STAT_WIS ) - 10;
		hp_bonus = 50 - ( 100 * victim->hit / UMAX( get_max_hp(victim), 1 ) );
		chance += hp_bonus / 2;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance -= 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance -= 20;
		chance -= sector_table[ victim->in_room->sector_type ].memdru_bonus * 10;
		if ( !IS_NPC( ch ) )
		{
			if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
			{
				chance += chance/5;
			}
		}
		chance = URANGE( 5, chance, 95 );
	}

	if( ch->class == CLASS_DRUID )
	{
		act( "Koncentrujesz siê wzywaj±c moce natury by ciê wspomog³y, zamykasz oczy na chwilê po czym otwieraj±c je kierujesz moc w kierunku $Z.", ch, NULL, victim, TO_CHAR );
		act( "$n koncentruje siê przez chwilê po czym wymawia s³owa w dziwnym dialekcie. Zamyka na chwilê oczy po czym otwieraj±c je wypuszcza ze swych r±k promienie zielonej energii, które opl±tuj± $C.", ch, NULL, victim, TO_ROOM );

		if ( number_percent() < chance )
		{
			act( "$n znika w rozb³ysku ¶wiat³a!", victim, NULL, NULL, TO_ALL );
			extract_char( victim, TRUE );
		}
		else
			act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );
		return;
	}

	act( "Koncentruj±c siê przywo³ujesz wielkie, staro¿ytne moce, które kr±¿± wokó³ twej ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_CHAR );
	act( "$n przywo³uje wielkie, staro¿ytne moce, które kr±¿± wokó³ $s ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_ROOM );

	if ( number_percent() < chance )
	{
		act( "W nag³ym rozb³ysku ¶wiat³a $n znika.", victim, NULL, NULL, TO_ALL );
		extract_char( victim, TRUE );
	}
	else
		act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );

	return;
}

void spell_dismiss_person( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * vmaster;
	int chance, hp_bonus;

	if ( victim == NULL )
	{
		send_to_char( "Kogo chcesz odes³aæ?", ch);
		return;
	}
	else if ( victim == ch )
	{
		send_to_char( "No co ty, sam<&/a/o> siebie chcesz odes³aæ?", ch);
		return;
	}

	vmaster = victim->master;
	if( vmaster == NULL ) vmaster = victim;

	if ( !IS_SET( race_table[ victim->race ].type, PERSON ) )
	{
		act( "$N nie jest osob±.", ch, NULL, victim, TO_CHAR );
		return;
	}
	if ( !IS_NPC(victim) || !EXT_IS_SET( victim->act, ACT_NO_EXP ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N nie zosta³o przywo³ane.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N nie zosta³ przywo³any.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N nie zosta³a przywo³ana.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	if ( vmaster == ch ) chance = 95;
	else
	{
		chance = 55;
		chance += ( get_curr_stat_deprecated( ch, STAT_LUC ) - 10 ) / 2;
		chance -= ( get_curr_stat_deprecated( victim, STAT_LUC ) - 10 ) / 2;
		chance += level * 4;
		chance -= victim->level / 2;
		chance -= vmaster->level * 3;
		chance -= get_curr_stat_deprecated( vmaster, STAT_WIS ) - 10;
		chance += get_curr_stat_deprecated( ch, STAT_WIS ) - 10;
		hp_bonus = 50 - ( 100 * victim->hit / UMAX( get_max_hp(victim), 1 ) );
		chance += hp_bonus / 2;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance -= 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance -= 20;
		if ( !IS_NPC( ch ) )
		{
			if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
			{
				chance += chance/5;
			}
		}
		chance = URANGE( 5, chance, 95 );
	}

	act( "Koncentruj±c siê przywo³ujesz wielkie, staro¿ytne moce, które kr±¿± wokó³ twej ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_CHAR );
	act( "$n przywo³uje wielkie, staro¿ytne moce, które kr±¿± wokó³ $s ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_ROOM );

	if ( number_percent() < chance )
	{
		act( "W nag³ym rozb³ysku ¶wiat³a $n znika.", victim, NULL, NULL, TO_ALL );
		extract_char( victim, TRUE );
	}
	else
		act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );

	return;
}

void spell_dismiss_monster( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * vmaster;
	int chance, hp_bonus;

	if ( victim == NULL )
	{
		send_to_char( "Kogo chcesz odes³aæ?", ch);
		return;
	}
	else if ( victim == ch )
	{
		send_to_char( "No co ty, chyba nie jeste¶ potworem?", ch);
		return;
	}

	vmaster = victim->master;
	if( vmaster == NULL ) vmaster = victim;

	if ( !IS_SET( race_table[ victim->race ].type, MONSTER ) )
	{
		act( "$N nie jest potworem.", ch, NULL, victim, TO_CHAR );
		return;
	}
	if ( !IS_NPC(victim) || !EXT_IS_SET( victim->act, ACT_NO_EXP ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N nie zosta³o przywo³ane.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N nie zosta³ przywo³any.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N nie zosta³a przywo³ana.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	if ( vmaster == ch ) chance = 95;
	else
	{
		chance = 55;
		chance += ( get_curr_stat_deprecated( ch, STAT_LUC ) - 10 ) / 2;
		chance -= ( get_curr_stat_deprecated( victim, STAT_LUC ) - 10 ) / 2;
		chance += level * 4;
		chance -= victim->level / 2;
		chance -= vmaster->level * 3;
		chance -= get_curr_stat_deprecated( vmaster, STAT_WIS ) - 10;
		chance += get_curr_stat_deprecated( ch, STAT_WIS ) - 10;
		hp_bonus = 50 - ( 100 * victim->hit / UMAX( get_max_hp(victim), 1 ) );
		chance += hp_bonus / 2;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance -= 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance -= 20;
		if ( !IS_NPC( ch ) )
		{
			if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
			{
				chance += chance/5;
			}
		}
		chance = URANGE( 5, chance, 95 );
	}

	act( "Koncentruj±c siê przywo³ujesz wielkie, staro¿ytne moce, które kr±¿± wokó³ twej ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_CHAR );
	act( "$n przywo³uje wielkie, staro¿ytne moce, które kr±¿± wokó³ $s ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_ROOM );

	if ( number_percent() < chance )
	{
		act( "W nag³ym rozb³ysku ¶wiat³a $n znika.", victim, NULL, NULL, TO_ALL );
		extract_char( victim, TRUE );
	}
	else
		act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );

	return;
}

void spell_dismiss_undead( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * vmaster;
	int chance, hp_bonus;

	if ( victim == NULL )
	{
		send_to_char( "Kogo chcesz odes³aæ?", ch);
		return;
	}
	else if ( victim == ch )
	{
		send_to_char( "No co ty, sam<&/a/o> siebie chcesz odes³aæ?", ch);
		return;
	}

	vmaster = victim->master;
	if( vmaster == NULL ) vmaster = victim;

	if ( !is_undead(victim) )
	{
		act( "$N nie jest nieumar³ym.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( !EXT_IS_SET( victim->act, ACT_NO_EXP ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N zosta³o stworzone zbyt dawno temu, by¶ by<&³/³a/³o> w stanie odes³aæ to tym zaklêciem.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N zosta³ stworzony zbyt dawno temu, by¶ by<&³/³a/³o> w stanie odes³aæ go tym zaklêciem.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N zosta³a stworzona zbyt dawno temu, by¶ by<&³/³a/³o> w stanie odes³aæ j± tym zaklêciem.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	if ( vmaster == ch ) chance = 95;
	else
	{
		chance = 55;
		chance += ( get_curr_stat_deprecated( ch, STAT_LUC ) - 10 ) / 2;
		chance -= ( get_curr_stat_deprecated( victim, STAT_LUC ) - 10 ) / 2;
		chance += level * 4;
		chance -= victim->level / 2;
		chance -= vmaster->level * 3;
		chance -= get_curr_stat_deprecated( vmaster, STAT_WIS ) - 10;
		chance += get_curr_stat_deprecated( ch, STAT_WIS ) - 10;
		hp_bonus = 50 - ( 100 * victim->hit / UMAX( get_max_hp(victim), 1 ) );
		chance += hp_bonus / 2;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance -= 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance -= 20;
		if ( IS_GOOD( ch ) ) chance += 10;
		if ( !IS_NPC( ch ) )
		{
			if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
			{
				chance += chance/5;
			}
		}
		chance = URANGE( 5, chance, 95 );
	}

	if( ch->class == CLASS_CLERIC || ch->class == CLASS_PALADIN )
	{
		act( "Koncentrujesz siê wzywaj±c moce swego boga by ciê wspomog³y, zamykasz oczy na chwilê po czym otwieraj±c je kierujesz moc w kierunku $Z.", ch, NULL, victim, TO_CHAR );
		act( "$n koncentruje siê przez chwilê po czym wymawia s³owa w dziwnym dialekcie. Zamyka na chwilê oczy po czym otwieraj±c je wypuszcza ze swych r±k promienie boskiej energii, które opl±tuj± $C.", ch, NULL, victim, TO_ROOM );

		if ( number_percent() < chance )
		{
			act( "$n znika w rozb³ysku ¶wiat³a!", victim, NULL, NULL, TO_ALL );
			extract_char( victim, TRUE );
		}
		else
			act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );
		return;
	}

	act( "Koncentruj±c siê przywo³ujesz wielkie, staro¿ytne moce, które kr±¿± wokó³ twej ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_CHAR );
	act( "$n przywo³uje wielkie, staro¿ytne moce, które kr±¿± wokó³ $s ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_ROOM );

	if ( number_percent() < chance )
	{
		act( "W nag³ym rozb³ysku ¶wiat³a $n znika.", victim, NULL, NULL, TO_ALL );
		extract_char( victim, TRUE );
	}
	else
		act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );

	return;
}

// czar spowoduje, ¿e cel nie bêdzie rozpoznwa³ kierunków, czyli ca³o¶æ
// do¶æ wredna, sens ma tylko i wy³±cznie dla PC, poniewa¿ moby nie
// ogl±daj± ekranu wy¶wietlaj±cego kierunki, oraz nie widz± komunikatu,
// ¿e nie maj± dok±d i¶æ ;-)
void spell_maze( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	int duration = 4 + level/4, luck;
	int mod = 1;

// wzmocnienie modyfikatora, dla specjalist
	if ( !IS_NPC( ch ) && ch->class == CLASS_MAG )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			mod = 2;
		}
	}

	if ( IS_AFFECTED(victim, AFF_MAZE))
	{
		if (victim == ch)
			send_to_char("Ju¿ i tak kiepsko orientujesz siê w przestrzeni.\n\r",ch);
		else
			act("$N ju¿ i tak kiepsko orientuje siê w przestrzeni.",ch,NULL,victim,TO_CHAR);
		return;
	}

        if( IS_AFFECTED( victim, AFF_PERFECT_SENSES ) )
        {
                switch(victim->sex )
                {
                        case SEX_MALE:
                                act( "$N wydaje siê byæ odporny na to zaklêcie.", ch, NULL, victim, TO_CHAR);
                                break;
                        case SEX_FEMALE:
                                act( "$N wydaje siê byæ odporna na to zaklêcie.", ch, NULL, victim, TO_CHAR);
                                break;
                        default:
                                act( "$N wydaje siê byæ odporne na to zaklêcie.", ch, NULL, victim, TO_CHAR);
                                break;
                }
                return;
        }

	luck = get_curr_stat_deprecated(ch, STAT_LUC);

	if (number_range(0, luck + LUCK_BASE_MOD) == 0 || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod - mod, ch, sn ))
	{
		if (victim == ch)
			send_to_char("Nie uda³o ci siê.\n\r",ch);
		else
			act("Nie uda³o ci siê wp³yn±æ na odczuwanie $Z.",ch,NULL,victim,TO_CHAR);
		return;
	}

	if (number_range(0, luck) > 90) duration++;
	if (number_range(0, luck) < 30) duration--;

// os³abienie dzia³ania czaru dla niespecjalist!
	if ( !IS_NPC( ch ) )
	{
		if (!( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) ))
		{
			duration /= 2;
		}
	}

	af.where     = TO_AFFECTS;
	af.type      = sn;
	af.level     = level;
	af.duration  = duration;
	af.rt_duration = 0;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = &AFF_MAZE;

	affect_to_char( victim, &af, NULL, TRUE );

	act("$n rozgl±da siê dooko³a, nierozpoznaj±c kierunków.", victim, NULL, NULL, TO_ROOM );
	send_to_char("Co¶ dziwnego dzieje siê z otaczaj±c± ciê przestrzeni±.\n\r", victim );
	return;
}

/* taki niby dopalacz dla charmiesow mistrza zauroczne, ale dopala to on niewiele
(w porownaniu do unholy fury prawie nic:P), bardziej chodzi o to, ze chroni przed
czarami puppet master i repayment, a takze ulatwia odcharmowanie swojego moba,
jak mu charm spadnie przed zejsciem loyalty.*/
void spell_loyalty( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	AFFECT_DATA *charm_af;
	int mod, chance, str_mod;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), intt = get_curr_stat_deprecated( ch, STAT_INT );

	if ( ch == victim || !IS_NPC(victim) || victim->master != ch )
	{
		send_to_char( "Ten czar mo¿esz rzucic tylko na swojego s³ugê.\n\r", ch );
		return;
	}

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nieuda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		act( "Lojalno¶æ $Z wzglêdem ciebie nie mo¿e byæ wiêksza.", ch, NULL, victim, TO_CHAR );
		return;
	}

		if ( victim->level >25 && IS_NPC( victim ) )
	{
		send_to_char( "Nie jeste¶ w stanie zauroczyæ tak potêznej osoby.\n\r", ch );
		return;
	}

	charm_af = affect_find( victim->affected, gsn_charm_person );
	if ( charm_af == NULL ) charm_af = affect_find( victim->affected, gsn_charm_monster );
	if ( charm_af == NULL ) charm_af = affect_find( victim->affected, gsn_domination );

	/* premie do walki */
	chance = number_range( 1, intt );
	mod = 0;
	if ( chance < 3 ) mod = -1;
	else if ( chance > 15 ) mod = 1;
	else if ( chance > 10 ) mod = 0;

	str_mod = mod;
	if( level > 20 ) ++str_mod;

	if ( charm_af != NULL ) charm_af->duration += 1 + level / 4 + 2 * mod;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_DAMROLL;
	af.duration = 1 + level / 4 + 2 * mod; af.rt_duration = 0;
	af.modifier = 1 + str_mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_HITROLL;
	af.duration = 1 + level / 4 + 2 * mod; af.rt_duration = 0;
	af.modifier = 1 + str_mod;
	af.bitvector = &AFF_LOYALTY;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Teraz bêdziesz jeszcze bardziej lojaln<&y/a/e> wzglêdem swego mistrza!\n\r", victim );
	act( "Lojalno¶æ $Z wzglêdem ciebie wzrasta!", ch, NULL, victim, TO_CHAR );
	act( "Lojalno¶æ $Z wzglêdem $z wzrasta!", ch, NULL, victim, TO_NOTVICT );
	return;
}

void spell_repayment( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * old_master;
	char buficzek[ MAX_STRING_LENGTH ];

	if ( !IS_NPC( ch ) && IS_IMMORTAL( victim ) )
		return;

	old_master = victim->master;

	if ( !IS_AFFECTED( victim, AFF_CHARM )
	     || old_master == NULL )
	{
		act( "$N nie s³u¿y przecie¿ nikomu.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( old_master == ch )
	{
		act( "Przecie¿ $N s³u¿y tylko tobie!", ch, NULL, victim, TO_CHAR );
		return;
	}

	if( is_safe( ch, victim ) || is_safe( victim, old_master ) )
		return;

	if ( victim == ch || IS_AFFECTED( ch, AFF_CHARM ) )
	{
		send_to_char( "Nie mo¿esz rzuciæ tego czaru na siebie.\n\r", ch );
		return;
	}

	if( level < LEVEL_HERO && IS_AFFECTED( victim, AFF_LOYALTY ))
	{
		if( victim->sex == SEX_FEMALE )
		{
			act("$N jest bardzo lojalna w stosunku do swego mistrza.", ch, NULL, victim, TO_CHAR );
		}
		else
		{
			act("$N jest bardzo lojalny w stosunku do swego mistrza.", ch, NULL, victim, TO_CHAR );
		}
		if( ch->sex == SEX_FEMALE )
		{
			act("$n chcia³a zbuntowaæ twego s³ugê przeciwko tobie.", ch, NULL, old_master, TO_VICT );
		}
		else
		{
			act("$n chcia³ zbuntowaæ twego s³ugê przeciwko tobie.", ch, NULL, old_master, TO_VICT );
		}
		return;
	}

	if ( level + 10 < old_master->level
	     || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn )
	     || saves_spell_new( old_master, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		sprintf( buficzek, "Nie uda³o ci siê uwolniæ %s z pod kontroli %s.", victim->name2, old_master->name2 );
		act( buficzek, ch, NULL, victim, TO_CHAR );
		sprintf( buficzek, "%s nie uda³o siê uwolniæ %s z pod kontroli %s.", ch->name3, victim->name2, old_master->name2 );
		act( buficzek, ch, NULL, old_master, TO_NOTVICT );
		sprintf( buficzek, "Czujesz, jak %s stara siê uwolniæ %s z pod twej kontroli. Nie pozwalasz na to.", ch->name, victim->name4 );
		act( buficzek, ch, NULL, old_master, TO_VICT );
		return;
	}

	die_follower( victim, FALSE );
	sprintf( buficzek, "%s uwalnia ciê z pod kontroli %s.", ch->name, old_master->name2 );
	act( buficzek, ch, NULL, victim, TO_VICT );
	sprintf( buficzek, "Uwalniasz %s z pod kontroli %s.", victim->name4, old_master->name2 );
	act( buficzek, ch, NULL, victim, TO_CHAR );
	sprintf( buficzek, "%s uwalnia %s z pod twej kontroli.", ch->name, victim->name4 );
	act( buficzek, ch, NULL, old_master, TO_VICT );

	stop_fighting( victim, FALSE );
	stop_fighting( ch, FALSE );
	victim->position = POS_STANDING;
	stop_hating( victim, ch, TRUE );

	if( victim->in_room == old_master->in_room )
	{
		act( "$N rzuca siê na ciebie!", old_master, NULL, victim, TO_CHAR );
		act( "$N rzuca siê na $c", old_master, NULL, victim, TO_NOTVICT );

		multi_hit( victim, old_master, TYPE_UNDEFINED );
	}

	if( IS_NPC(ch) )
	{
		stop_fighting( ch, FALSE );
		stop_hating( ch, victim, TRUE );
		multi_hit( ch, old_master, TYPE_UNDEFINED );
	}

	start_hunting( victim, old_master );

	return;
}

void spell_antimagic_manacles( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, dur;

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( victim == ch )
	{
		send_to_char( "Nie jeste¶ w stanie rzuciæ tego zaklêcia na siebie.", ch );
		return;
	}

	if ( get_caster(victim) == -1 )
	{
		act( "Przecie¿ $n nie posiada ¿adnej magicznej mocy.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_affected(victim, gsn_antimagic_manacles) )
	{
		act( "Magiczna moc $Z jest ju¿ ograniczona.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck + 25 ) == 0 || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		act( "Nie uda³o ci siê ograniczyæ magicznej mocy $Z.", ch, NULL, victim, TO_CHAR );
		act( "$x nie uda³o siê ograniczyæ twej magicznej mocy.", ch, NULL, victim, TO_VICT );
		act( "$x nie uda³o siê ograniczyæ magicznej mocy $Z.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	dur = 1 + level/10;

	af.where = TO_AFFECTS;
	af.type = gsn_antimagic_manacles;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Na twych d³oniach pojawiaj± siê pó³materialne kajdany. Czujesz, ¿e twe moce magiczne s³abn±.\n\r", victim );
	act( "Na d³oniach $z pojawiaj± siê pó³materialne kajdany.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_great_dispel( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
	CHAR_DATA *victim;
	AFFECT_DATA *aff, *aff_next;
	OBJ_DATA *obj, *obj_next;
	int count = 0;

	if (target == TARGET_OBJ)
	{
		obj = (OBJ_DATA *) vo;

		if ( IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
		{
			send_to_char( "Ten przedmiot jest odporny na magiê.\n\r", ch );
			return;
		}

		//i chuj, by nie enchantowali w nieskonczonosc
		if( obj->timer > 0 )
		{
			act( "$p rozpada siê na drobne kawa³eczki, które malowniczo rozpryskuj± sie dooko³a.", ch, obj, NULL, TO_CHAR );
			extract_obj( obj );
			return;
		}

		for ( aff = obj->affected; aff != NULL; aff = aff->next )
		{
			if( aff->duration == -1 )
				continue;
		    if( aff->level > 45 )
		    	continue;

			++count;
			affect_remove_obj( obj, aff );
		}

	    if ( count )
	    	act( "Rozpraszasz wszystkie zaklêcia wp³ywaj±ce na $h.", ch, obj, NULL, TO_CHAR );

		//jak przedmiot niezniszczalny to moze zdjac tylko czesc affectow
		if ( IS_OBJ_STAT( obj, ITEM_UNDESTRUCTABLE ) )
		{
			if ( IS_OBJ_STAT( obj, ITEM_DARK ) )
			{
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_DARK );
				act( "Mroczna aura otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
			}

			if ( IS_OBJ_STAT( obj, ITEM_EVIL ) )
			{
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_EVIL );
				act( "Zimna aura z³a otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
			}

			if ( IS_OBJ_STAT( obj, ITEM_INVIS ) )
			{
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_INVIS );
				act( "Aura niewidzialno¶ci otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
			}

			if ( IS_OBJ_STAT( obj, ITEM_UNDEAD_INVIS ) )
			{
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_UNDEAD_INVIS );
				act( "Aura niewidzialno¶ci dla nieumar³ych otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
			}

			if ( IS_OBJ_STAT( obj, ITEM_ANIMAL_INVIS ) )
			{
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_ANIMAL_INVIS );
				act( "Aura niewidzialno¶ci dla zwierz±t otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
			}
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_DARK ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_DARK );
			act( "Mroczna aura otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
		}

		if ( IS_OBJ_STAT( obj, ITEM_EVIL ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_EVIL );
			act( "Z³a aura otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
		}

		if ( IS_OBJ_STAT( obj, ITEM_INVIS ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_INVIS );
			act( "Aura niewidzialno¶ci otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
		}

		if ( IS_OBJ_STAT( obj, ITEM_UNDEAD_INVIS ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_UNDEAD_INVIS );
			act( "Aura niewidzialno¶ci dla nieumar³ych otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
		}

		if ( IS_OBJ_STAT( obj, ITEM_ANIMAL_INVIS ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_ANIMAL_INVIS );
			act( "Aura niewidzialno¶ci dla zwierz±t otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
		}

		if ( IS_OBJ_STAT( obj, ITEM_GLOW ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_GLOW );
			act( "$p ga¶nie.", ch, obj, NULL, TO_ALL );
		}

		if ( IS_OBJ_STAT( obj, ITEM_BURN_PROOF ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_BURN_PROOF );
			act( "Odporno¶c na ogien $f zostaje rozproszona.", ch, obj, NULL, TO_ALL );
		}

		if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_MAGIC );
			act( "Magiczna aura otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
		}

		if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_BLESS );
			act( "Aura b³ogos³awieñstwa otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
		}

		return;
	}

	victim = (CHAR_DATA *) vo;

	if ( ( number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) + LUCK_BASE_MOD ) == 0 ||
		number_percent() > ( 65 + ( level * 4 ) - ( victim->level * 4 ) + ( IS_NPC(ch) ? 15 : 5 )  )
	   ) && ch != victim )
	{
		act( "$N w ostatniej chwili chroni siê przed twym zaklêciem.", ch, NULL, victim, TO_CHAR );
		return;
	}

    for ( aff = victim->affected; aff; aff = aff_next )
	{
		aff_next = aff->next;

		if( aff->type <= 0 || aff->type >= MAX_SKILL )
		    continue;

		/* tylko spelle */
		if( skill_table[aff->type].spell_fun == spell_null )
	    	continue;

		if( aff->type == gsn_on_smoke )
			continue;

		//spelle resistowe maja % w polu level, czyli niedaloby sie ich zdispelowac przez to. Juz lepiej by sie dalo niezaleznie od lewa
		if( aff->level > 40 &&
			aff->type != gsn_resist_fire &&
			aff->type != 246 &&//stom shell
			aff->type != gsn_resist_lightning &&
			aff->type != gsn_resist_cold &&
			aff->type != 143 &&//resist acid
			aff->type != gsn_brave_cloak &&//resist acid
			aff->type != 178 &&//resist elements
			aff->type != 179 &&//lesser magic resist
#ifdef STEEL_SKIN_ON
//Nil: steel_skin
			aff->type != gsn_steel_skin &&
#endif /* STEEL_SKIN_ON */
			aff->type != gsn_energy_shield )
			continue;

		affect_remove( victim, aff );
		if ( skill_table[aff->type].msg_off )
		{
			send_to_char( skill_table[aff->type].msg_off, victim );
			send_to_char( "\n\r", victim );
		}

//		affect_strip( victim, aff->type );
	}

	for ( obj = victim->carrying; obj != NULL; obj = obj_next )
	{
		OBJ_NEXT_CONTENT( obj, obj_next );

		if ( obj->pIndexData->vnum == OBJ_VNUM_LIGHT_BALL || obj->pIndexData->vnum == OBJ_VNUM_GOOD_BALL || obj->pIndexData->vnum == OBJ_VNUM_NEUTRAL_BALL || obj->pIndexData->vnum == OBJ_VNUM_EVIL_BALL )
		{
			act( "Kula ¶wiat³a rozpada siê na kawa³eczki.", ch, NULL, NULL, TO_ALL );
			extract_obj(obj);
		}

		if ( obj->pIndexData->vnum == OBJ_VNUM_FLAMEBLADE )
		{
			act( "P³on±ce ostrze ga¶nie z lekkim sykiem i znika.", ch, NULL, NULL, TO_ALL );
			extract_obj(obj);
		}

		if ( obj->pIndexData->vnum == OBJ_VNUM_EYE_OF_VISION )
		{
			act( "Oko widzenia zamyka siê i znika.", ch, NULL, NULL, TO_ALL );
			extract_obj(obj);
		}

        if (
                0
                || obj->pIndexData->vnum == OBJ_VNUM_HOLY_HAMMER
                || obj->pIndexData->vnum == OBJ_VNUM_HOLY_FLAIL
                || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_STAFF
                || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_FLAIL
                || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_SHORTSWORD
                || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_HAMMER
                || obj->pIndexData->vnum == OBJ_VNUM_GOODBARRY
                || obj->pIndexData->vnum == OBJ_VNUM_MUSHROOM
           )
        {
            act( "$p migocze przez chwilê i znika.", ch, obj, NULL, TO_ALL );
            extract_obj(obj);
        }

		if ( obj->pIndexData->vnum == OBJ_VNUM_FLOATING_SKULL || obj->pIndexData->vnum == OBJ_VNUM_CREATE_SYMBOL_EVIL || obj->pIndexData->vnum == OBJ_VNUM_CREATE_SYMBOL_NEUTRAL || obj->pIndexData->vnum == OBJ_VNUM_CREATE_SYMBOL_GOOD )
		{
			act( "$p migocze przez chwilê i rozpda siê na kawa³eczki.", ch, obj, NULL, TO_ALL );
			extract_obj(obj);
		}

		if ( obj->pIndexData->vnum == OBJ_VNUM_SHADOW_DAGGER || obj->pIndexData->vnum == OBJ_VNUM_SHADOW_SHORT_SWORD || obj->pIndexData->vnum == OBJ_VNUM_SHADOW_SHORT_SWORD_VORPAL || obj->pIndexData->vnum == OBJ_VNUM_SHADOW_SHORT_SWORD_DISPELLER )
		{
			act( "Ostrze cienia migocze przez chwilê i znika.", ch, NULL, NULL, TO_ALL );
			extract_obj(obj);
		}

		if ( obj->pIndexData->vnum == OBJ_VNUM_FALSE_RAZORBLADE || obj->pIndexData->vnum == OBJ_VNUM_NORMAL_RAZORBLADE || obj->pIndexData->vnum == OBJ_VNUM_SHARP_RAZORBLADE || obj->pIndexData->vnum == OBJ_VNUM_VORPAL_RAZORBLADE || obj->pIndexData->vnum == OBJ_VNUM_FALSE_CLAW || obj->pIndexData->vnum == OBJ_VNUM_NORMAL_CLAW || obj->pIndexData->vnum == OBJ_VNUM_SHARP_CLAW || obj->pIndexData->vnum == OBJ_VNUM_VORPAL_CLAW )
		{
			act( "Twa d³oñ powraca do naturalnego stanu.", victim, NULL, NULL, TO_CHAR );
			act( "D³oñ $z powraca do naturalnego stanu.", victim, NULL, NULL, TO_ROOM );
			extract_obj(obj);
		}

		if ( obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_ARMOR_HEAD || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_ARMOR_BODY || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_ARMOR_LEGS || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_ARMOR_ARMS || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_ARMOR_HANDS || obj->pIndexData->vnum == OBJ_VNUM_SPIRITUAL_ARMOR_FEET || obj->pIndexData->vnum == OBJ_VNUM_DISC )
		{
			if ( obj->liczba_mnoga )
				act( "$p migocz± przez chwilê i rozpadaj± siê.", victim, obj, NULL, TO_ALL );
			else
				act( "$p migocze przez chwilê i rozpada siê.", victim, obj, NULL, TO_ALL );
			extract_obj(obj);
		}
	}

	act( "Rozpraszasz wszystkie zaklêcia wp³ywaj±ce na $C.", ch, NULL, victim, TO_CHAR );
	act( "$n rozprasza wszystkie wp³ywaj±ce na ciebie zaklêcia.", ch, NULL, victim, TO_VICT );
	act( "$n rozprasza wszystkie zaklêcia wp³ywaj±ce na $C.", ch, NULL, victim, TO_NOTVICT );
	return;
}

void spell_deflect_wounds( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), dur;

	if( is_affected(ch,sn))
	{
		send_to_char( "Jeste¶ ju¿ otoczon<&y/a/e> potê¿n± kopu³±.\n\r", ch );
		return;
	}

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ kopu³y chroni±cej czê¶ciowo przed ranami.\n\r", ch );
		return;
	}

	dur = (level-14)/4.5;
	if ( dice(2,luck) < 10 ) --dur;
	if ( dice(2,luck) > 20 ) ++dur;
	if ( dice(2,get_curr_stat_deprecated(ch,STAT_INT)) > 27 ) ++dur;
	if ( dice(2,get_curr_stat_deprecated(ch,STAT_INT)) < 21 ) --dur;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 0;
	af.duration = UMAX(1,dur); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_DEFLECT_WOUNDS;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Otacza ciê kopu³a, ochroniaj±ca czê¶ciowo przed ranami.\n\r", ch );
	act( "$c otacza potê¿na, migocz±ca kopu³a.", ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_imbue_with_element( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	AFFECT_DATA af;
	sh_int duration, affect_location = WEAPON_SHOCKING;
	int luck, intelligence;

	if (obj->item_type != ITEM_WEAPON )
	{
		send_to_char("To nie jest broñ.\n\r",ch);
		return;
	}

	if ( IS_WEAPON_STAT( obj, WEAPON_FLAMING) || IS_WEAPON_STAT( obj, WEAPON_FROST) || IS_WEAPON_STAT( obj, WEAPON_SHOCKING) )
	{
		send_to_char( "Ta broñ jest ju¿ nasycona energi± ¿ywio³u.\n\r",ch);
		return;
	}

	luck = get_curr_stat( ch, STAT_LUC );
	if ( EXT_IS_SET(obj->extra_flags, ITEM_NOMAGIC ) || number_range(0, luck/6 + LUCK_BASE_MOD) == 0)
	{
		send_to_char("Nie uda³o ci siê nasyciæ tej broni energi± ¿ywio³u.\n\r",ch);
		return;
	}


	intelligence = get_curr_stat( ch, STAT_INT );

	duration = 10 + level/3;

	if ( dice( 2, luck ) < 60 )
    {
        duration /= 2;
    }
    else if ( dice( 2, luck ) > OLD_28_VALUE )
    {
        duration *= 2;
    }

	if ( dice( 2, intelligence ) > OLD_28_VALUE )
    {
        duration += 10;
    }
    else if ( dice( 2, intelligence ) < 120 )
    {
        duration /= 2;
    }

	switch( number_range( 1,4 ) )
	{
        default:
		case 1:
			affect_location = WEAPON_SHOCKING;
			act("$p rozb³yska wy³adowaniami elektrycznymi.", ch, obj, NULL, TO_ALL);
			break;
		case 2:
			affect_location = WEAPON_FLAMING;
			act("$p zaczyna p³on±æ.", ch, obj, NULL, TO_ALL);
			break;
		case 3:
			affect_location = WEAPON_FROST;
			act("$p pokrywa siê lodem.", ch, obj, NULL, TO_ALL);
			break;
		case 4:
			affect_location = WEAPON_TOXIC;
			act("Powierzchniê $f pokrywa kwasowy osad.", ch, obj, NULL, TO_ALL);
			break;
	}

	af.where = TO_WEAPON;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 3, duration );
//    af.rt_duration = 10;
    af.rt_duration = 0;
	af.location = affect_location;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	affect_to_obj( obj, &af );

	if( !IS_OBJ_STAT( obj, ITEM_UNDESTRUCTABLE ) )
    {
        obj->condition = UMIN( 99, URANGE( 1, obj->condition - 35 + level, obj->condition - 1 ) );
    }

	return;
}

void spell_astral_journey( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int moves;

	if ( IS_AFFECTED( victim, AFF_ASTRAL_JOURNEY ) )
	{
		send_to_char( "Jeste¶ ju¿ pod wp³ywem tego zaklêcia.\n\r", victim );
		return;
	}

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", victim );
		return;
	}

	moves = number_range( 3, get_curr_stat_deprecated( ch, STAT_INT ) / 2 ) + number_range( 0, luck / 4 );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = moves;
	af.duration = 1; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_ASTRAL_JOURNEY;

	affect_to_char( victim, &af, NULL, TRUE );

	act( "$n pada na ziemiê bez czucia.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Twój umys³ powoli uwalnia siê z okowów cia³a. Po chwili widzisz swoje cia³o le¿±ce bezw³adnie na ziemi.\n\r", victim );

	ch->hit = UMAX( 1, ch->hit / 2 );

	mind_move_room( victim, victim->in_room );
	return;
}

void spell_resist_poison(int sn,int level,CHAR_DATA *ch,void *vo, int target)
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated(ch, STAT_LUC);

	if (is_affected(victim, sn) )
	{
	if(victim == ch)
	{
		switch (ch->sex)
		{
		case 0:
			send_to_char("Nie mo¿esz byæ bardziej chronione.\n\r",ch);
			break;
		case 2:
			send_to_char("Nie mo¿esz byæ bardziej chroniona.\n\r",ch);
			break;
		default :
			send_to_char("Nie mo¿esz byæ bardziej chroniony.\n\r",ch);
			break;
		}
	}
	else
	{
		switch (ch->sex)
		{
			case 0:
				act("$N nie mo¿e byæ bardziej chronione.",ch,NULL,victim,TO_CHAR);
				break;
			case 2:
				act("$N nie mo¿e byæ bardziej chroniona.",ch,NULL,victim,TO_CHAR);
				break;
			default :
				act("$N nie mo¿e byæ bardziej chroniony.",ch,NULL,victim,TO_CHAR);
				break;
		}
	}
	return;
	}

	if (number_range(0, luck + LUCK_BASE_MOD) == 0)
	{
		if (ch == victim)
			act("Nie uda³o ci siê zwiêkszyæ odporno¶ci $Z na trucizê.",ch,NULL,victim,TO_CHAR);
		else
			send_to_char("Nie uda³o ci siê zwiêkszyæ swojej odporno¶ci na truciznê.\n\r",ch);
		return;
	}

	af.where     = TO_AFFECTS;
	af.type      = sn;
	af.level     = 65;
	af.duration  = 4 + level/6;
	af.rt_duration = 0;
	af.location  = APPLY_RESIST;
	af.modifier  = RESIST_POISON;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE  );

	if(victim!=ch)
	{
		switch (victim->sex)
		{
		case 0:
			act("$N jest teraz bardziej odporne na truciznê.",ch,NULL,victim,TO_CHAR);
			break;
		case 2:
			act("$N jest teraz bardziej odporna na truciznê.",ch,NULL,victim,TO_CHAR);
			break;
		default :
			act("$N jest teraz bardziej odporny na truciznê.",ch,NULL,victim,TO_CHAR);
			break;
		}
	}
	else
	{
		switch (victim->sex)
		{
		case 0:
			send_to_char("Czujesz, ¿e teraz jeste¶ bardziej odporne na truciznê.\n\r",ch);
			break;
		case 2:
			send_to_char("Czujesz, ¿e teraz jeste¶ bardziej odporna na truciznê.\n\r",ch);
			break;
		default :
			send_to_char("Czujesz, ¿e teraz jeste¶ bardziej odporny na truciznê.\n\r",ch);
			break;
		}
	}
	return;
}

void spell_wizard_eye( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim;
    ROOM_INDEX_DATA *original;
    AFFECT_DATA af;
    char *tar = ( char * ) vo;
    int luck;
    bool have_component = FALSE;
    /**
     * sprawdz parametr
     */
    if ( !tar || tar[ 0 ] == '\0' )
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        return;
    }
    /**
     * znajdz cel
     */
    victim = get_char_world( ch, tar );
    if ( !victim ||
            ch->in_room == NULL ||
            victim->in_room == NULL ||
            IS_NPC( victim ) ||
            victim->level >= LEVEL_IMMORTAL ||
            EXT_IS_SET( victim->in_room->room_flags, ROOM_NOMAGIC ))
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        return;
    }
    /**
     * test szczescia
     */
    luck = get_curr_stat( ch, STAT_LUC );
    if ( number_range( 0, luck ) < 10 )
    {
        send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
        return;
    }
    /**
     * zabawa z komponenten, sprawdzamy dopiero prz drugim uzyciu
     */
    have_component = spell_item_check( ch, sn, "wac1" );
    if ( is_affected( ch, sn ) )
    {
        if ( !have_component )
        {
            send_to_char( "Nie masz wystarczaj±co mocy, ¿eby tak szybko rzuciæ ponownie ten czar.\n\r", ch );
            return;
        }
    }
    /**
     * nakladanie affektu, jezeli nie ma komponentu
     */
    if ( !have_component )
    {
        af.where = TO_AFFECTS;
        af.type = sn;
        af.level = 40;//no dispel
        af.duration = 6;
        af.rt_duration = 0;
        af.location = APPLY_AC;
        af.modifier = 0;
        af.bitvector = &AFF_NONE;
        affect_to_char( ch, &af, NULL, FALSE );
    }
    /**
     * popatrz tam gdzie trzeba
     */
    original = ch->in_room;
    char_from_room( ch );
    char_to_room( ch, victim->in_room );
    do_function( ch, &do_look, "auto" );
    char_from_room( ch );
    char_to_room( ch, original );
    return;
}

void spell_loop( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
	CHAR_DATA *victim = (CHAR_DATA *) vo;
	AFFECT_DATA af;
	int duration = level/7.5, luck;

	if ( IS_AFFECTED(victim, AFF_LOOP))
	{
		if (victim == ch)
			send_to_char("Ju¿ i ¶wiat wokó³ ciebie krêci siê w kó³ko.\n\r",ch);
		else
			act("$N ju¿ i tak widzi krêc±cy siê w kó³ko ¶wiat.",ch,NULL,victim,TO_CHAR);
		return;
	}

        if( IS_AFFECTED( victim, AFF_PERFECT_SENSES ) )
        {
                switch(victim->sex )
                {
                        case SEX_MALE:
                                act( "$N wydaje siê byæ odporny na to zaklêcie.", ch, NULL, victim, TO_CHAR);
                                break;
                        case SEX_FEMALE:
                                act( "$N wydaje siê byæ odporna na to zaklêcie.", ch, NULL, victim, TO_CHAR);
                                break;
                        default:
                                act( "$N wydaje siê byæ odporne na to zaklêcie.", ch, NULL, victim, TO_CHAR);
                                break;
                }
                return;
        }

	luck = get_curr_stat_deprecated(ch, STAT_LUC);

	if (number_range(0, luck + LUCK_BASE_MOD) == 0 || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char("Nie uda³o ci siê.\n\r",ch);
		send_to_char("¦wiat wokó³ ciebie przez chwilkê krêci siê w kó³ko, po chwili jednak wraca do normalnego stanu.\n\r", victim );
		return;
	}

	if (number_range(0, luck) > 15) ++duration;
	if (number_range(0, luck) < 4)  --duration;

	af.where     = TO_AFFECTS;
	af.type      = sn;
	af.level     = level;
	af.duration  = URANGE(1, duration, 3 );
	af.rt_duration = 0;
	af.location  = APPLY_DEX;
	af.modifier  = -level/4;//traci refleks, im wiekszy lew tym szybciej swiat sie kreci
	af.bitvector = &AFF_LOOP;

	affect_to_char( victim, &af, NULL, TRUE );

	act("$n rozgl±da siê dooko³a ³api±c za g³owê.", victim, NULL, NULL, TO_ROOM );
	send_to_char("¦wiat wokó³ ciebie zaczyna siê krêêêciiiiæ...\n\r", victim );
	return;
}

void spell_piercing_sight( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int dur;

	if ( IS_AFFECTED( ch, AFF_PIERCING_SIGHT ) )
	{
		send_to_char( "Twój wzrok nie mo¿e byæ bardziej przenikliwy.\n\r", ch );
		return;
	}

	dur = level / 4;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_PIERCING_SIGHT;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Twój wzrok staje siê nadzwyczajnie przenikliwy.\n\r", ch );
	act( "Oczy $z lekko zmieniaj± siê." , ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_acid_arrow( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dam, mod, lev;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluc = get_curr_stat_deprecated( victim, STAT_LUC );

//dam - pierwszy dam
//co pulse_regen (24 jednostki czegostam, 2 rundy walki)
//mod - tyle w sumie przywala _dodatkowe_ ciosy
//lev - tyle zdejmie nastepny pozostaly cios, z kazdym ciosem spada
//jak mod albo lev wyniesie 0 lub mniej affect jest stripowany


	if ( number_range( 0, 25 - vluc + luck) < 1 )
	{
		act( "Kwasowa strza³a mija $c.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Kwasowa strza³a mija ciê w niewielkiej odleg³o¶ci!\n\r", victim );
		return;
	}


	dam = dice(2,6) + ((level*5)/10);
	mod = dice( level, 4 );
	lev = number_range( level/2, level + level/2);

			if( spell_item_check( ch, sn , "acid based spell" ) )
    {
      dam += 4;
      mod += 4;
      lev += 2;
    }

	act( "Kwasowa strza³a wbija siê w cia³o $z.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Kwasowa strza³a wbija siê w twoje cia³o!\n\r", victim );

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam /= 2;
		mod /= 2;
		lev /= 2;
	}


//jak ma juz starszego affecta, to idzie spell od nowa
	if ( is_affected(victim, gsn_acid_arrow) )
		affect_strip( victim, gsn_acid_arrow);

	af.where = TO_AFFECTS;
	af.type = gsn_acid_arrow;
	af.level = lev;
	af.duration = -1; af.rt_duration = 0;//duration -1, bo to schodzi jak wystrzela juz caly dam
	af.location = APPLY_NONE;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	spell_damage( ch, victim, dam, sn, DAM_ACID, FALSE );
	acid_effect( victim, level, dam, TARGET_CHAR );
	return;
}

void spell_slippery_floor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	int luck, dur;

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
	{
		send_to_char( "Nie mo¿esz tego tu robiæ.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_SLIPPERY_FLOOR ) )
	{
		send_to_char( "Tutaj jest ju¿ wystarczaj±co ¶lisko.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) )
	{
		send_to_char( "To miejsce jest po¶wiêcone i nie dasz rady pokryæ pod³o¿a ¶lisk± i t³ust± powierzchni±.\n\r", ch );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê pokryæ pod³o¿a ¶lisk± i t³ust± powierzchni±.\n\r", ch );
		return;
	}

	switch ( ch->in_room->sector_type )
	{
		case 0: //sektor wewnatrz, stosunkowo d³ugo wytrzymuje, bo nie ma w co wsiakac
			dur = 7;
			break;
		case 14://zwykle naturalne podloze
		case 15:
		case 17:
		case 28:
		case 2:
		case 4:
		case 30:
		case 31:
		case 32:
		case 33:
		case 42:
		case 3:
		case 11:
		case 12:
		case 38:
			dur = 3;
			break;
		case 1: //sektory miejskie, drogowe itd gdzie jest jakies nienaturalne podloze
		case 8:
		case 18:
		case 36:
		case 41:
			dur = 6;
			break;
		case 10: //sektory silnie piaszczyste
		case 24:
		case 29:
		case 40:
			dur = 1;
			break;
		case 16: //tam gdzie snieg
		case 26:
		case 27:
			dur = 4;
			break;
		case 5: //gory, czesciowo skaliste podloze
		case 13:
		case 35:
			dur = 6;
			break;
		case 6: //sektory wodne
		case 7:
		case 19:
		case 20:
		case 21:
		case 22:
		case 25:
		case 34:
		case 37:
		case 39:
		case 23:
			send_to_char( "Nie mo¿esz tego tu robiæ.\n\r", ch );
			return;
			break;
		default:
			dur = 4;
			break;
	}

	if( number_range(0,luck) > 10 ) ++dur;
	if( number_range(0,luck) < 4 ) --dur;

	create_event( EVENT_SLIPPERY_FLOOR_END, UMAX( 10, PULSE_TICK * dur ), ch->in_room, NULL, 0 );
	EXT_SET_BIT( ch->in_room->room_flags, ROOM_SLIPPERY_FLOOR );
	send_to_char( "Pokrywasz pod³o¿e t³ust± i ¶lisk± substancj±.\n\r", ch );
	act( "$n pokrywa pod³o¿e t³ust± i ¶lisk± substancj±.", ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_detect_undead( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int dur = 12 + level / 4;

	if ( IS_AFFECTED( ch, AFF_DETECT_UNDEAD ) )
	{
		send_to_char( "Potrafisz ju¿ przecie¿ rozpoznawaæ niemar³ych.\n\r", ch );
		return;
	}

	/* Bonus dla specjalisty */

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur *= 2;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_DETECT_UNDEAD;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Czujesz, ¿e twoje oczy lekko zmieniaj± siê.\n\r", ch );
	act( "Oczy $z lekko zmieniaj± siê." , ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_confuse_languages( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, vluck, dur, langs;

	if ( is_affected( victim, sn ))
	{
		act( "Jêzyki $Z zosta³y ju¿ pomieszane.", ch, NULL, victim, TO_CHAR );
		return;
	}

	/* luck */

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( ch != victim && ( number_range(0, vluck - luck) > 5 || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn )))
	{
		act( "Nie uda³o ci siê pomieszaæ jêzyków $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dur = 4 + level / 10;

	for ( langs = 0; lang_table[langs].bit != LANG_UNKNOWN; langs++ )
	{
		if ( knows_language( victim, langs, victim ) )
		{

			af.where = TO_AFFECTS;
			af.type = sn;
			af.level = - knows_language( victim, langs, victim ) + URANGE(1, dice(2,get_curr_stat_deprecated(victim,STAT_WIS)) > 18 ? ((40-level)*2) : (40-level)  ,knows_language( victim, langs, victim ));
			af.duration = dur;
	af.rt_duration = 0;
			af.location = APPLY_LANG;
			af.modifier = langs;
			af.bitvector = &AFF_NONE;
			affect_to_char( victim, &af, NULL, TRUE );

		}
	}

	send_to_char( "Czujesz, jak znajomo¶æ jêzyków tajemniczo zanika w twej g³owie...\n\r", victim );
	act("$n ³apie siê za g³owê.", victim, NULL, NULL, TO_ROOM );

	return;
}

void spell_wall_of_mist( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;
	ROOM_INDEX_DATA * to_room;
	int luck, door, dur;
	char *tar = ( char * ) vo;
	char *door_name = ".";
	char buf[MAX_STRING_LENGTH];

	if ( !tar || tar[ 0 ] == '\0' )
	{
		send_to_char( "Gdzie chcesz utworzyæ chmurê mg³y?\n\r", ch );
		return;
	}

	if ( ( door = find_door( ch, tar ) ) < 0 )
	{
		print_char( ch, "Nie istnieje taki kierunek jak '%s'.\n\r", tar );
		return;
	}

	pexit = ch->in_room->exit[ door ];
	to_room = pexit->u1.to_room;
	pexit_rev = to_room->exit[ rev_dir[ door ] ];

	if ( IS_SET( pexit->exit_info, EX_WALL_OF_MIST ) )
	{
		send_to_char( "To nic nie zmieni.\n\r", ch );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê sprowadziæ chmury mg³y.\n\r", ch );
		return;
	}

	switch( door )
	{
		case 0:
			door_name = " na pó³nocy.";
			break;
		case 1:
			door_name = " na wschodzie.";
			break;
		case 2:
			door_name = " na po³udniu.";
			break;
		case 3:
			door_name = " na zachodzie.";
			break;
		case 4:
			door_name = " na górze.";
			break;
		case 5:
			door_name = " na dole.";
			break;
		default:
			door_name = ".";
			break;
	}

	dur = 1;

	switch ( ch->in_room->sector_type )
	{
		case 0://sektory wewnatrz, dlugo mgla stoi
		case 14:
		case 15:
		case 17:
		case 39:
		case 41:
			++dur;
			if( level > 10 ) ++dur;
			if( level > 15 ) ++dur;
			if( level > 20 ) ++dur;
			if( level > 27 ) ++dur;
			if( level > 35 ) ++dur;
			break;
		case 1://sektory jakos okryte, ale bez dachu
		case 3:
		case 11:
		case 12:
		case 38:
			++dur;
			if( level > 12 ) ++dur;
			if( level > 18 ) ++dur;
			if( level > 25 ) ++dur;
			if( level > 30 ) ++dur;
			if( level > 35 ) ++dur;
			break;
		case 2://po prostu odkryty teren
		case 8:
		case 10:
		case 16:
		case 18:
		case 24:
		case 26:
		case 27:
		case 28:
		case 30:
		case 31:
		case 32:
		case 33:
		case 36:
		case 40:
		case 42:
			if( level > 10 ) ++dur;
			if( level > 17 ) ++dur;
			if( level > 22 ) ++dur;
			if( level > 28 ) ++dur;
			if( level > 35 ) ++dur;
			break;
		case 4://gory itd gdzie wiatr mocno pizga
		case 5:
		case 9:
		case 13:
		case 35:
			if( level > 15 ) ++dur;
			if( level > 25 ) ++dur;
			if( level > 35 ) ++dur;
			break;
		default: //wodne itd, no nad woda mgla sie raczej lubi unosic
			++dur;
			if( level > 10 ) ++dur;
			if( level > 16 ) ++dur;
			if( level > 21 ) ++dur;
			if( level > 28 ) ++dur;
			if( level > 35 ) ++dur;
			break;
	}

	create_event( EVENT_WALL_OF_MIST_END, PULSE_TICK * dur, ch->in_room, NULL, door );
	SET_BIT( pexit->exit_info, EX_WALL_OF_MIST );
	if( pexit_rev != NULL && pexit_rev->u1.to_room == ch->in_room )
	{
		SET_BIT( pexit_rev->exit_info, EX_WALL_OF_MIST );
		create_event( EVENT_WALL_OF_MIST_END, PULSE_TICK * dur, to_room, NULL, rev_dir[ door ] );
	}
	print_char( ch, "Tworzysz chmurê mg³y, która zas³ania widoczno¶æ%s\n\r", door_name );
	sprintf( buf, "$n tworzy chmurê mg³y, która zas³ania widoczno¶æ%s.", door_name );
	act( buf, ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_alarm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	EXIT_DATA *pexit;
	EXIT_DATA *pexit_rev;
	ROOM_INDEX_DATA * to_room;
	int luck, door;
	char *tar = ( char * ) vo;
	char *door_name = ".";
	char buf[MAX_STRING_LENGTH];

	if ( !tar || tar[ 0 ] == '\0' )
	{
		send_to_char( "Gdzie chcesz za³o¿yæ magiczny alarm?\n\r", ch );
		return;
	}

	if ( ( door = find_door( ch, tar ) ) < 0 )
	{
		print_char( ch, "Nie istnieje taki kierunek jak '%s'.\n\r", tar );
		return;
	}

	pexit = ch->in_room->exit[ door ];
	to_room = pexit->u1.to_room;
	pexit_rev = to_room->exit[ rev_dir[ door ] ];

	if ( pexit->alarm != NULL )
	{
		send_to_char( "Tam jest ju¿ za³o¿ony magiczny alarm.\n\r", ch );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, 25 + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê za³o¿yæ magicznego alarmu.\n\r", ch );
		return;
	}

	switch( door )
	{
		case 0:
			door_name = " na pó³nocy.";
			break;
		case 1:
			door_name = " na wschodzie.";
			break;
		case 2:
			door_name = " na po³udniu.";
			break;
		case 3:
			door_name = " na zachodzie.";
			break;
		case 4:
			door_name = " na górze.";
			break;
		case 5:
			door_name = " na dole.";
			break;
		default:
			door_name = ".";
			break;
	}

	pexit->alarm = ch;
	if( pexit_rev != NULL && pexit_rev->u1.to_room == ch->in_room )
	{
		pexit_rev->alarm = ch;
	}
	print_char( ch, "Zak³adasz magiczny alarm%s\n\r", door_name );
	sprintf( buf, "$n zak³ada magiczny alarm%s.", door_name );
	act( buf, ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_blink( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int dur, szansa_1, szansa_2;

	if ( is_affected( ch, gsn_blink ) )
	{
		send_to_char( "Jeste¶ ju¿ pod wp³ywem tego zaklêcia.\n\r", ch );
		return;
	}

	if ( number_range( 0, 25 + get_curr_stat_deprecated(ch,STAT_DEX) ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	dur = 1 + level/4;
	if( number_range( 0, get_curr_stat_deprecated(ch,STAT_DEX) ) > 10 ) ++dur;
	if( number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) ) < 3 ) --dur;

	//jest to procentowa szansa na to, ze bedacemu pod wplywem nie powiedzie
	//sie rzucenie zaklecia lub wykonanie ataku wrecz.
	szansa_1 = URANGE(10, 32 - (level/2), 30);

	//jest to szansa na unikniecie czegos nieprzyjemnego
	szansa_2 = URANGE(33, 12 + level + get_curr_stat_deprecated(ch,STAT_DEX), 66 );

	if( dice(2, get_curr_stat_deprecated(ch,STAT_INT)) > 27 )
	{
		if( dice(1,2) == 1 )
		{
			szansa_1 -= (get_curr_stat_deprecated(ch,STAT_LUC)-5)/3;
		}
		else
		{
			szansa_2 += (get_curr_stat_deprecated(ch,STAT_LUC)-5)/3;
		}
	}
	if(!IS_NPC(ch))
	{
    	if ( spell_item_check( ch, sn, "longer blink" ))
	    {
         dur *= 2;

        }
     }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = URANGE( 10, szansa_1, 30);
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = URANGE( 33, szansa_2, 66);
	af.bitvector = &AFF_NONE;

	affect_to_char( ch, &af, NULL, TRUE );

	act( "Sylwetka $z zaczyna migotaæ.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Twe cia³o zaczyna migotaæ pomiêdzy ró¿nymi planami egzystencji.\n\r", ch );
	return;
}

void spell_banshees_howl( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA *vch, *vch_next;
	AFFECT_DATA af;
	int counter = 0,select = 0;

	strip_invis( ch, TRUE, TRUE );

	act("Powietrze rozdziera mro¿±ce krew w ¿y³ach wycie!", ch, NULL, NULL, TO_ALL );

	for ( vch = ch->in_room->people;vch;vch = vch_next )
	{
		vch_next = vch->next_in_room;

		if ( is_undead(vch) || IS_SET(vch->form, FORM_CONSTRUCT) || is_safe( ch, vch ) )
			continue;

	if ( ch == vch  )
			continue;

		if ( vch->level >= level )
		{
			if( vch->sex == SEX_FEMALE )
				act( "$n wydaje siê byæ odporna na lament banshee.", vch, NULL, NULL, TO_ROOM );
			else
				act( "$n wydaje siê byæ odporny na lament banshee.", vch, NULL, NULL, TO_ROOM );
			send_to_char( "Lament banshee nie wywiera na tobie ¿adnego wra¿enia.\n\r", vch );
			if ( IS_NPC(vch))
				start_hunting( vch, ch );
			if ( can_see( vch, ch ) && can_move( vch ) && vch->fighting == NULL )
				multi_hit( vch, ch, TYPE_UNDEFINED );
			continue;
		}

		if (
			( vch->resists[ RESIST_FEAR ] > 0 && number_percent() < vch->resists[ RESIST_FEAR ] ) ||
			( vch->resists[ RESIST_SOUND ] > 0 && number_percent() < vch->resists[ RESIST_SOUND ] ) ||
			IS_AFFECTED( vch, AFF_DEAFNESS )
			)
		{
			if( vch->sex == SEX_FEMALE )
				act( "$n wydaje siê byæ odporna na lament banshee.", vch, NULL, NULL, TO_ROOM );
			else
				act( "$n wydaje siê byæ odporny na lament banshee.", vch, NULL, NULL, TO_ROOM );
			send_to_char( "Lament banshee nie wywiera na tobie ¿adnego wra¿enia.\n\r", vch );
			if ( IS_NPC(vch))
				start_hunting( vch, ch );
			if ( can_see( vch, ch ) && can_move( vch ) && vch->fighting == NULL )
				multi_hit( vch, ch, TYPE_UNDEFINED );
			continue;
		}

		if ( saves_spell_new( vch, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			act( "$n opiera siê zawodzeniu banshee.", vch, NULL, NULL, TO_ROOM );
			send_to_char( "Opierasz siê zawodzeniu banshee.\n\r", vch );
			if ( IS_NPC(vch))
				start_hunting( vch, ch );
			if ( can_see( vch, ch ) && can_move( vch ) && vch->fighting == NULL )
				multi_hit( vch, ch, TYPE_UNDEFINED );
			continue;
		}

		//tutaj utrudnienie zabijania jednym strzalem silnych mobow przez featowcow ( ze zjednoczeniem i przebiciem)
		if( level - vch->level > 16 && vch->hit > number_range( 400, 550 ) )
		{
			if ( saves_spell_new( vch, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
			{
				act( "$n opiera siê zawodzeniu banshee.", vch, NULL, NULL, TO_ROOM );
				send_to_char( "Opierasz siê zawodzeniu banshee.\n\r", vch );
				if ( IS_NPC(vch))
						start_hunting( vch, ch );
				if ( can_see( vch, ch ) && can_move( vch ) && vch->fighting == NULL )
					multi_hit( vch, ch, TYPE_UNDEFINED );
				continue;
			}
		}

		if ( counter > level / 4.5 )
		{
			act( "$n opiera siê zawodzeniu banshee.", vch, NULL, NULL, TO_ROOM );
			send_to_char( "Opierasz siê zawodzeniu banshee.\n\r", vch );
			if ( IS_NPC(vch))
			{
				start_hunting( vch, ch );
				if ( can_see( vch, ch ) && can_move( vch ) && vch->fighting == NULL )
				{
					multi_hit( vch, ch, TYPE_UNDEFINED );
				}
			}
			continue;
		}

		sound_effect( vch, level, dice(level,3), TARGET_CHAR );

		select = UMAX( 1, level - vch->level - counter );

		switch( select )
		{
			case 1:
			case 2:
				switch( vch->sex )
				{
					case 0:
						act( "Skóra $z staje siê nagle blada, a $e rozgl±da siê woko³o lekko oszo³omione strachem.", vch, NULL, NULL, TO_ROOM );
						break;
					case 2:
						act( "Skóra $z staje siê nagle blada, a $e rozgl±da siê woko³o lekko oszo³omiona strachem.", vch, NULL, NULL, TO_ROOM );
						break;
					default:
						act( "Skóra $z staje siê nagle blada, a $e rozgl±da siê woko³o lekko oszo³omiony strachem.", vch, NULL, NULL, TO_ROOM );
						break;
				}
				send_to_char( "Momentalnie tw± g³owê wype³nia strach lekko ciê oszo³amiaj±c!\n\r", vch );
				DAZE_STATE( vch, URANGE(1, 1 + level/15, 3 ) * PULSE_VIOLENCE );
				spell_damage(ch, vch, dice(level,2), sn,DAM_SOUND,FALSE);
				break;
			case 3:
			case 4:
			case 5:
				switch( vch->sex )
				{
					case 0:
						act( "Skóra $z staje siê nagle blada, a $e rozgl±da siê woko³o lekko oszo³omione strachem.", vch, NULL, NULL, TO_ROOM );
						break;
					case 2:
						act( "Skóra $z staje siê nagle blada, a $e rozgl±da siê woko³o lekko oszo³omiona strachem.", vch, NULL, NULL, TO_ROOM );
						break;
					default:
						act( "Skóra $z staje siê nagle blada, a $e rozgl±da siê woko³o lekko oszo³omiony strachem.", vch, NULL, NULL, TO_ROOM );
						break;
				}
				send_to_char( "Momentalnie tw± g³owê wype³nia strach lekko ciê oszo³amiaj±c!\n\r", vch );
				spell_damage(ch, vch, dice(level,3), sn,DAM_SOUND,FALSE);
				DAZE_STATE( vch, URANGE(1, 1 + level/10, 5 ) * PULSE_VIOLENCE );
				break;
			case 6:
			case 7:
			case 8:
			case 9:
				act( "Skóra $z staje siê nagle bardzo blada, a $e rozgl±da siê woko³o z panik± w oczach.", vch, NULL, NULL, TO_ROOM );
				send_to_char( "Momentalnie tw± g³owê wype³nia nienaturalnie silny strach!\n\r", vch );

				af.where     = TO_AFFECTS;
				af.type      = gsn_fear;
				af.level     = level;
				af.location  = APPLY_NONE;
				af.duration  = 2;//fear nie jest round.hour
				af.rt_duration = 0;
				af.modifier	 = 0;
				af.bitvector = &AFF_FEAR;
				affect_to_char( vch, &af, NULL, TRUE );

				spell_damage(ch, vch, dice(level,3), sn,DAM_SOUND,FALSE);
				break;
			case 10:
			case 11:
			case 12:
			case 13:
				act( "Skóra $z staje siê nagle bardzo blada, a $e ³apie siê za g³owê rozgl±daj±c wokó³.", vch, NULL, NULL, TO_ROOM );
				send_to_char( "Momentalnie tw± g³owê wype³nia nienaturalnie silny strach osza³amiaj±c ciê!\n\r", vch );

				af.where     = TO_AFFECTS;
				af.type      = 304;//daze
				af.level     = level;
				af.location  = APPLY_NONE;
				af.duration  = URANGE(1, 1 + level/10, 5 );
				af.rt_duration = 0;
				af.modifier	 = 0;
				af.bitvector = &AFF_DAZE;
				affect_to_char( vch, &af, NULL, TRUE );

				af.where     = TO_AFFECTS;
				af.type      = gsn_fear;
				af.level     = level;
				af.location  = APPLY_NONE;
				af.duration  = 2;//fear nie jest round.hour
				af.rt_duration = 0;
				af.modifier	 = 0;
				af.bitvector = &AFF_FEAR;
				affect_to_char( vch, &af, NULL, TRUE );

				spell_damage(ch, vch, dice(level,4), sn,DAM_SOUND,FALSE);
				break;
			case 14:
			case 15:
			case 16:
			case 17:
				act( "Skóra $z staje siê nagle bardzo blada, a $e zatrzymuje siê w bardzo dziwnej pozycji.", vch, NULL, NULL, TO_ROOM );
				send_to_char( "Momentalnie twe cia³o wype³nia nienaturalnie silny strach powoduj±cy parali¿ miê¶ni!\n\r", vch );

				af.where     = TO_AFFECTS;
				af.type      = gsn_holdmonster;
				af.level     = level;
				af.location  = APPLY_NONE;
				af.duration  = URANGE(1, 1 + level/10, 5 );
				af.rt_duration = 0;
				af.modifier	 = 0;
				af.bitvector = &AFF_PARALYZE;
				affect_to_char( vch, &af, NULL, TRUE );

				af.where     = TO_AFFECTS;
				af.type      = gsn_fear;
				af.level     = level;
				af.location  = APPLY_NONE;
				af.duration  = 2;//fear nie jest round.hour
				af.rt_duration = 0;
				af.modifier	 = 0;
				af.bitvector = &AFF_FEAR;
				affect_to_char( vch, &af, NULL, TRUE );

				spell_damage(ch, vch, dice(level,4), sn,DAM_SOUND,FALSE);
				break;
			default:
				act( "Skóra $z staje siê nagle strasznie blada, po czym pada $e na ziemiê!", vch, NULL, NULL, TO_ROOM );
				send_to_char( "Momentalnie twe serce wype³nia nienaturalnie silny strach powoduj±c zawa³! Padasz na ziemiê trac±c oddech.\n\r", vch );

				if ( IS_NPC( vch ) && HAS_TRIGGER( vch, TRIG_DEATH ) )
				{
//					vch->position = POS_STANDING;
					mp_percent_trigger( vch, ch, NULL, NULL, &TRIG_DEATH );
//					vch->position = POS_DEAD;
				}
				raw_damage( ch, vch, 20 + get_max_hp(vch) );
				break;
		}

		if ( vch && IS_NPC ( vch ) )
		{
			start_hunting( vch, ch );
			if ( can_see( vch, ch ) && can_move( vch ) )
			{
				multi_hit( vch, ch, TYPE_UNDEFINED );
			}
		}
		counter++;
	}
	return;
}

void spell_nondetection( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	/* zale¿no¶æ od szczê¶cia */
	if ( number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_NONDETECTION ) )
	{
		if ( ch == victim )
			send_to_char( "Przecie¿ jeste¶ ju¿ niewidoczn<&y/a/e> dla prostych czarów Poznania.\n\r", ch );
		else
			switch ( victim->sex )
			{
				case 0:
					act( "$N ju¿ jest niewidoczne dla prostych czarów Poznania.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N ju¿ jest niewidoczny dla prostych czarów Poznania.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "$N ju¿ jest niewidoczna dla prostych czarów Poznania.", ch, NULL, victim, TO_CHAR );
					break;
			}
		return;
	}

	if( victim->sex == SEX_FEMALE )
		act( "$n staje siê niewidoczna dla prostych czarów Poznania.", victim, NULL, NULL, TO_ROOM );
	else
		act( "$n staje siê niewidoczny dla prostych czarów Poznania.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Stajesz siê niewidoczn<&y/a/e> dla prostych czarów Poznania.\n\r", victim );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = URANGE( 5, level/4, 10); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_NONDETECTION;
	affect_to_char( victim, &af, NULL, TRUE );
	return;
}

void spell_shadow_conjuration( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA *shadow;
	AFFECT_DATA af;
	int vnum;

	if ( IS_NPC( ch ) )
		return;

	if ( EXT_IS_SET( ch->act, PLR_NOFOLLOW ) )
	{
		send_to_char( "Przecie¿ nie chcesz ¿eby kto¶ za tob± chodzi³.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_PRIVATE ) )
	{
		send_to_char( "To siê tutaj nie uda, jest za ma³o miejsca.\n\r", ch );
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

	if (  get_curr_stat_deprecated( ch, STAT_CHA ) < 10 )
	{
		print_char( ch, "Nie dasz rady stworzyæ ¿adnej cienistej iluzji.\n\r" );
		return;
	}

	if ( number_range( 0, 25 + get_curr_stat_deprecated( ch, STAT_LUC ) ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê stworzyæ ¿adnej cienistej iluzji.\n\r", ch );
		return;
	}

	if( level > 27 )//czar specjalisty ma zawsze +2 do lvl
	{//silniejsze mobiki
		switch(dice(1,3))
		{
			case 1:
				vnum = MOB_VNUM_SHADOW_IFRYT;
				break;
			case 2:
				vnum = MOB_VNUM_SHADOW_FEYR;
				break;
			case 3:
				vnum = MOB_VNUM_SHADOW_OGR;
				break;
		}
	}
	else
	{//i slabsze mobiki
		switch(dice(1,3))
		{
			case 1:
				vnum = MOB_VNUM_SHADOW_MYKONID;
				break;
			case 2:
				vnum = MOB_VNUM_SHADOW_HAKOWA_POCZWARA;
				break;
			case 3:
				vnum = MOB_VNUM_SHADOW_JASZCZUROCZLEK;
				break;
		}
	}

	shadow = create_mobile( get_mob_index( vnum ) );

	if( !shadow )
	{
		send_to_char( "Co¶ siê jeb³o.\n\r", ch );
		return;
	}

	ch->counter[4] = 1;

	char_to_room( shadow, ch->in_room );
	EXT_SET_BIT( shadow->act, ACT_NO_EXP );
    money_reset_character_money ( shadow );
	shadow->level = ch->level - 1; // by nie kradl expa
	shadow->max_hit = 1; // i tak sie rozplywa po jednym ciosie

	if ( HAS_TRIGGER( shadow, TRIG_ONLOAD ) )
	{
		shadow->position = POS_STANDING;
		mp_onload_trigger( shadow );
	}

	shadow->hit = get_max_hp(shadow);

	af.where = TO_AFFECTS;
	af.type = gsn_domination;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( shadow, &af, NULL, TRUE );

	af.type = sn;
	af.bitvector = &AFF_ILLUSION; // by wiedzialo ze zniknac przy ciosie
	affect_to_char( shadow, &af, NULL, TRUE );

	af.bitvector = &AFF_SILENCE;
	affect_to_char( shadow, &af, NULL, TRUE );

	if ( !add_charm( ch, shadow, TRUE ) )
	{
		extract_char( shadow, TRUE );
		return;
	}

	add_follower( shadow, ch, TRUE );

	send_to_char( "Zrêcznie stwarzasz cienist± iluzjê.\n\r", ch );
	act( "$N materializuje siê z nico¶ci.", ch, NULL, shadow, TO_ROOM );

	return;
}

void spell_fetch( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * fetch;
	AFFECT_DATA af;
	AFFECT_DATA *aff, *aff_next;
	char long_d[ MAX_INPUT_LENGTH ];
	int i;
	bool self_fetch = FALSE;
	// spell graczy do rzucania na graczy
	if ( IS_NPC( ch ) || IS_NPC(victim) )
	{
		send_to_char("Ten czar mo¿esz rzuciæ tylko na graczy.\n\r", ch );
		return;
	}

	//do 25 lvl (czar speca to lvl to ch->level +2, dlatego 28) wlacznie robisz tylko sobowtory z siebie
	if ( victim != ch && level < 28 )
	{
		act( "Masz za ma³± moc, by stworzyæ sobowtór kogo¶ innego.", ch, NULL, victim, TO_CHAR );
		return;
	}

	// no jak stoi w private, to przecie gdzie to zrobiæ?
	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_PRIVATE ) )
	{
		send_to_char( "To siê tutaj nie uda, jest za ma³o miejsca.\n\r", ch );
		return;
	}

	if( victim == ch )
		self_fetch = TRUE;

	if ( !self_fetch && !is_same_group( ch, victim ) )
	{
		act( "$N nie nale¿y do twojej grupy.", ch, NULL, victim, TO_CHAR );
		return;
	}

	fetch = create_mobile( get_mob_index( MOB_VNUM_FETCH) );
	clone_mobile( victim, fetch );

	if( self_fetch )
	{
		send_to_char( "Zrêcznie tworzysz swój sobowtór.\n\r", ch );
		act( "$n zrêcznie tworzy swój sobowtór.", ch, NULL, NULL, TO_ROOM );
	}
	else
	{
		act( "Zrêcznie tworzysz sobowtór $Z.", ch, NULL, victim, TO_CHAR );
		act( "$n zrêcznie tworzy sobowtór $Z.", ch, NULL, victim, TO_NOTVICT );
		act( "$n zrêcznie tworzy twój sobowtór.", ch, NULL, victim, TO_VICT );
	}

	EXT_SET_BIT( fetch->act, ACT_IS_NPC );
	char_to_room( fetch, ch->in_room );
	EXT_SET_BIT( fetch->act, ACT_NO_EXP );
	EXT_SET_BIT( fetch->act, ACT_SENTINEL );
    money_reset_character_money ( fetch );
	fetch->level = 1;
	fetch->max_hit = 1;
	fetch->short_descr = str_dup(victim->name);
	sprintf( long_d, "%s%s%s", victim->name, victim->pcdata->title, " stoi tutaj." );
	fetch->long_descr = str_dup(long_d);
	fetch->start_pos = POS_STANDING;

	for ( i = 0;i < MAX_RESIST;i++ )
	{
		fetch->resists[ i ] = 0;
	}

	for ( aff = fetch->affected; aff; aff = aff_next )
	{
		aff_next = aff->next;

		affect_remove( fetch, aff );
	}

	af.where = TO_AFFECTS;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.type = sn;
	af.modifier = 0;
	af.bitvector = &AFF_ILLUSION; // by wiedzialo ze zniknac
	affect_to_char( fetch, &af, NULL, TRUE );

	af.bitvector = &AFF_SILENCE;
	affect_to_char( fetch, &af, NULL, TRUE );

	create_event( EVENT_EXTRACT_CHAR, 240*level/2, fetch, NULL, 0 );

	return;
}

void spell_draining_hands( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA edge;

	if( is_affected(ch, gsn_draining_hands ))
	{
		send_to_char( "Wokó³ twych d³oni jest ju¿ wysysaj±ca ¿ycie aura.\n\r", ch );
		return;
	}

	if( number_range( 0, 25 + get_curr_stat_deprecated(ch, STAT_LUC ) ) == 0 || number_range( 0, get_curr_stat_deprecated(ch, STAT_INT ) ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê skoncentrowaæ w swych d³oniach wampirycznej energii.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_WIELD ) || get_eq_char( ch, WEAR_SHIELD ) || get_eq_char( ch, WEAR_SECOND ) || get_eq_char( ch, WEAR_HOLD ) || get_eq_char( ch, WEAR_INSTRUMENT ))
	{
		send_to_char( "Do rzucenia tego zaklêcia potrzebne ci s± dwie wolne rêce.\n\r", ch );
		return;
	}

	edge.where = TO_AFFECTS;
	edge.type = sn;
	edge.duration = URANGE(1, 1 + level/3, 7 ); edge.rt_duration = 0;
	edge.level = level;
	edge.location = APPLY_NONE;
	edge.modifier = 0;
	edge.bitvector = &AFF_NONE;
	affect_to_char( ch, &edge, NULL, TRUE  );

	act( "Twe d³onie otacza wysysaj±ca ¿ycie aura.", ch, NULL, NULL, TO_CHAR );
	act( "D³onie $z otacza wysysaj±ca ¿ycie aura.", ch, NULL, NULL, TO_ROOM );

	return;
}

void spell_magic_hands( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA edge;

	if( is_affected(ch, gsn_magic_hands ))
	{
		send_to_char( "Wokó³ twych d³oni jest ju¿ magiczna aura.\n\r", ch );
		return;
	}

	if( number_range( 0, 25 + get_curr_stat_deprecated(ch, STAT_LUC ) ) == 0 || number_range( 0, get_curr_stat_deprecated(ch, STAT_INT ) ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê skoncentrowaæ w swych d³oniach magicznej energii.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_WIELD ) || get_eq_char( ch, WEAR_SHIELD ) || get_eq_char( ch, WEAR_SECOND ) || get_eq_char( ch, WEAR_HOLD ) || get_eq_char( ch, WEAR_INSTRUMENT ))
	{
		send_to_char( "Do rzucenia tego zaklêcia potrzebne ci s± dwie wolne rêce.\n\r", ch );
		return;
	}

	edge.where = TO_AFFECTS;
	edge.type = sn;
	edge.duration = URANGE(1, 1 + level/3, 7 ); edge.rt_duration = 0;
	edge.level = level;
	edge.location = APPLY_NONE;
	edge.modifier = 0;
	edge.bitvector = &AFF_NONE;
	affect_to_char( ch, &edge, NULL, TRUE  );

	act( "Twe d³onie otacza magiczna aura.", ch, NULL, NULL, TO_CHAR );
	act( "D³onie $z otacza magiczna aura.", ch, NULL, NULL, TO_ROOM );

	return;
}

void spell_stability( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	int duration;

	if ( IS_AFFECTED( ch, AFF_STABILITY ) )
	{
		send_to_char( "Nie mo¿esz mieæ wiêkszego wyczucia równowagi.\n\r", ch );
		return;
	}

	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê zwiêkszyæ swego wyczucia równowagi.\n\r", ch );
		return;
	}

	duration = level/6;
	if ( get_curr_stat_deprecated(ch,STAT_INT) > number_range(10,30) )
		++duration;
	if ( get_curr_stat_deprecated(ch,STAT_LUC) < number_range(1,10 ) )
		--duration;

	duration = URANGE( 2, duration, 10 );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_STABILITY;
	affect_to_char( ch, &af, NULL, TRUE );

	act( "$n zaczyna siê poruszaæ jako¶ inaczej.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Czujesz, ¿e bieganie po linach nad przepa¶ciami nie stanowi³oby dla ciebie problemu.\n\r", ch );
	return;
}

void spell_exile( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim, *vch_next, *vmaster;
	int chance, hp_bonus;

	act( "Koncentruj±c siê przywo³ujesz wielkie, staro¿ytne moce, które kr±¿± w powietrzu.", ch, NULL, NULL, TO_CHAR );
	act( "$n przywo³uje wielkie, staro¿ytne moce, które kr±¿± w powietrzu.", ch, NULL, NULL, TO_ROOM );

	for ( victim = ch->in_room->people;victim;victim = vch_next )
	{
		vch_next = victim->next_in_room;

		if ( victim == ch )
			return;

		vmaster = victim->master;
		if( vmaster == NULL ) vmaster = victim;

		if ( !EXT_IS_SET( victim->act, ACT_NO_EXP ) )
			continue ;

		if ( vmaster == ch ) chance = 95;
		else
		{
			chance = 70;
			chance += ( get_curr_stat_deprecated( ch, STAT_LUC ) - 10 ) / 2;
			chance -= ( get_curr_stat_deprecated( victim, STAT_LUC ) - 10 ) / 2;
			chance += level * 4;
			chance -= victim->level / 2;
			chance -= vmaster->level * 3;
			chance -= get_curr_stat_deprecated( vmaster, STAT_WIS ) - 10;
			chance += get_curr_stat_deprecated( ch, STAT_WIS ) - 10;
			hp_bonus = 50 - ( 100 * victim->hit / UMAX( get_max_hp(victim), 1 ) );
			chance += hp_bonus / 2;
			if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance -= 20;
			if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance += 20;
			if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance += 20;
			if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance -= 20;
			chance = URANGE( 5, chance, 95 );
		}

		if ( number_percent() < chance )
		{
			act( "W nag³ym rozb³ysku ¶wiat³a $n znika.", victim, NULL, NULL, TO_ALL );
			extract_char( victim, TRUE );
		}
		else
			act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );

	}

	return;
}

void spell_revive( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo, *item, *item_next;
	CHAR_DATA *undead;
	MOB_INDEX_DATA *mob = NULL;
	AFFECT_DATA af;
	int made_dur = 5, malf_chance;

	if ( obj->item_type != ITEM_CORPSE_NPC ||
	     obj->value[ 0 ] <= 0 ||
	     ( mob = get_mob_index( obj->value[ 0 ] ) ) == NULL )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	if ( is_affected(ch,gsn_made_undead) && !IS_IMMORTAL(ch) )
    {
        send_to_char( "Straci³<&e/a/o>¶ koncentracje, musisz chwilê poczekaæ zanim ponownie stworzysz martwiaka.\n\r", ch );
        return;
    }

    if ( ( IS_SET( race_table[ mob->race ].type, PLANT ) ) )
    {
        print_char( ch, "Nic siê nie sta³o.\n\r" );
        return;
    }

	mob = get_mob_index( obj->value[ 0 ] );

	obj->value[ 0 ] = 0;

	if ( !mob )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	malf_chance = mob->level;
	malf_chance *= 8;

	malf_chance = 400 - malf_chance;

	if ( IS_SET( mob->form, FORM_CONSTRUCT ) )
	{
		print_char( ch, "Przecie¿ %s nigdy nie %s ¿yw± istot±, nie jeste¶ w stanie %s o¿ywiæ.\n\r", mob->short_descr, ( mob->sex == SEX_MALE ? "by³" : ( mob->sex == SEX_FEMALE ? "by³a" : "by³o" ) ), ( mob->sex == SEX_FEMALE ? "jej" : "go" ) );
		return;
	}

	if ( number_range( 1, malf_chance ) == 1 )
	{
		extract_obj( obj );
		summon_malfunction( ch, sn );
		return;
	}

	if ( mob->level > level - 8 || mob->level > 25 )
	{
		send_to_char( "Masz za ma³± moc, by o¿ywiæ kogo¶ tak potê¿nego.\n\r", ch );
		return;
	}
	if ( ( undead = create_mobile( mob ) ) == NULL )
	{
		print_char( ch, "Nic siê nie sta³o.\n\r" );
		return;
	}

	made_dur = level > 29 ? 4 : 7;

	char_to_room( undead, ch->in_room );
	undead->spec_fun = NULL;
    money_reset_character_money ( undead );
	EXT_SET_BIT( undead->act, ACT_RAISED );
	EXT_SET_BIT( undead->act, ACT_NO_EXP );
	EXT_SET_BIT( undead->act, ACT_UNDEAD );
	SET_BIT( undead->form, FORM_UNDEAD );
	undead->alignment = -1000;
	undead->max_hit -= number_range( undead->max_hit/3, undead->max_hit/2 );
	undead->hit = get_max_hp(undead);

	act("Nagle $p powstaje.", undead, obj, NULL, TO_ROOM );

	for ( item = obj->contains; item != NULL; item = item_next )
	{
		OBJ_NEXT_CONTENT( item, item_next );
	    obj_from_obj( item );
	    obj_to_char( item, undead );
	}

	for ( item = undead->carrying; item != NULL; item = item->next_content )
	{
		if ( item->item_type == ITEM_WEAPON )
			wield_weapon_silent( undead, item, TRUE );
		else
	    	wear_obj_silent( undead, item, FALSE );
	}

	extract_obj(obj);

	if ( !add_charm( ch, undead, TRUE ) )
	{
		EXT_SET_BIT( undead->act, ACT_AGGRESSIVE );
		return;
	}

	if ( IS_AFFECTED( undead, AFF_TROLL ) )
		undead->affected_by[ AFF_TROLL.bank ] ^= AFF_TROLL.vector;

	undead->resists[RESIST_MAGIC_WEAPON] = 0;

	--undead->perm_stat[STAT_STR];
	--undead->perm_stat[STAT_CON];
	undead->perm_stat[STAT_DEX] /= 2;
	undead->perm_stat[STAT_WIS] /= 2;
	undead->perm_stat[STAT_INT] /= 2;
	undead->perm_stat[STAT_CHA] /= 2;

	af.where = TO_AFFECTS;
	af.type = gsn_domination;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_CHARM;
	affect_to_char( undead, &af, NULL, TRUE );

	af.where = TO_AFFECTS;
	af.type = gsn_silence;
	af.level = 50;//by sie nie dalo dispelnac
	af.duration = -1; af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_SILENCE;
	affect_to_char( undead, &af, NULL, TRUE );

	af.where = TO_AFFECTS;
	af.type = gsn_made_undead;
	af.level = 50;//by sie nie dalo dispelnac
	af.duration = made_dur;
	af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	af.visible = FALSE;
	affect_to_char( ch, &af, NULL, FALSE );

	add_follower( undead, ch, TRUE );
	return;
}

void spell_psionic_blast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;

	if( !IS_SET( victim->parts, PART_BRAINS ) )
		return;

	spell_damage( ch, victim, number_range( 200, 350 ), sn, DAM_MENTAL , TRUE );
	return;
}

void spell_inspire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af, *stare;
	int dur;

	if( is_affected(victim, sn ))
	{
		stare = affect_find( victim->affected, sn );
		if( stare->duration >= 3 )
		{
			if( victim == ch )
				send_to_char( "Twe ukryte pragnienia zosta³y juz pobudzone.\n\r", ch );
			else
				act( "Ukryte pragnienia $Z zosta³y ju¿ pobudzone.", ch, NULL, victim, TO_CHAR );
		}
		else
		{
			if( victim == ch )
				send_to_char( "Nie masz teraz na to nastroju.\n\r", ch );
			else if ( victim->sex == SEX_FEMALE )
				act( "$N jest zbyt za³amana, by to zaklêcie zadzia³a³o.", ch, NULL, victim, TO_CHAR );
			else if ( victim->sex == SEX_MALE )
				act( "$N jest zbyt za³amany, by to zaklêcie zadzia³a³o.", ch, NULL, victim, TO_CHAR );
			else
				act( "$N jest zbyt za³amane, by to zaklêcie zadzia³a³o.", ch, NULL, victim, TO_CHAR );
		}
		return;
	}

	if (number_range(0, get_curr_stat_deprecated(ch,STAT_LUC) + LUCK_BASE_MOD) == 0)
	{
		send_to_char("Nie uda³o ci siê wywo³aæ ukrytych pragnieñ.\n\r",ch);
		return;
	}

	dur = 5;
	if ( level > 28 )
		++dur;
	if ( get_curr_stat_deprecated(ch,STAT_INT)*3 + get_curr_stat_deprecated(ch,STAT_WIS)*2 + get_curr_stat_deprecated(ch,STAT_LUC) - 60 > 46 )
		++dur;

	af.where     = TO_AFFECTS;
	af.type      = sn;
	af.level     = level;
	af.duration  = dur;//duration ten jest wlasciwie krotszy o 3, gdyz przez ostatnie 3 ticki mody do dama i hita sa ujemne
	af.rt_duration = 0;
	af.location  = APPLY_NONE;
	af.modifier  = 0;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, "+Twoje ukryte pragnienia zosta³y pobudzone", TRUE );

	send_to_char( "Twoje ukryte pragnienia zosta³y pobudzone, a¿ chce ci siê walczyæ!\n\r", victim );
	act( "Zauwa¿asz, ¿e $n bardzo pali siê do walki.", victim, NULL, NULL, TO_ROOM );

	return;
}

void spell_hardiness( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	AFFECT_DATA af;
	int duration;

	if ( EXT_IS_SET(obj->extra_flags, ITEM_NOMAGIC ) )
	{
		send_to_char( "Ten przedmiot jest odporny na magiê.\n\r",ch );
		return;
	}

	if ( EXT_IS_SET(obj->extra_flags, ITEM_UNDESTRUCTABLE ) )
	{
		send_to_char( "Ten przedmiot jest ju¿ niezniszczalny.\n\r",ch );
		return;
	}

	if ( obj->condition < 100 )
	{
		send_to_char( "To zaklêcie mo¿esz rzuciæ tylko na przedmiot bêd±cy w idealnej kondycji.\n\r", ch );
		return;
	}

	if (number_range(0, get_curr_stat( ch, STAT_LUC )/6 + LUCK_BASE_MOD) == 0 )
	{
		send_to_char( "Nie uda³o ci siê uodporniæ tego przedmiotu na wszelkie zniszczenia.\n\r",ch);
		return;
	}

	duration = level/2;
	if ( dice(2,get_curr_stat(ch,STAT_LUC)) < 60 ) --duration;
	if ( dice(2,get_curr_stat(ch,STAT_LUC)) > 140 ) ++duration;
	if ( dice(2,get_curr_stat(ch,STAT_INT)) > OLD_28_VALUE ) ++duration;
	if ( dice(2,get_curr_stat(ch,STAT_INT)) < 120 ) --duration;

	act( "Przez chwilê $p rozb³yska siln±, niebiesk± aur±.", ch, obj, NULL, TO_ALL );

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = UMAX(5,duration); af.rt_duration = 0;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = &ITEM_UNDESTRUCTABLE;
	affect_to_obj( obj, &af );
	return;
}

void spell_mental_barrier( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	AFFECT_DATA af;
	char postac[ MAX_INPUT_LENGTH ];
	char nazwa[ MAX_INPUT_LENGTH ];
	int luck, dur, czarek;
	char *tar = ( char * ) vo;

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	tar = one_argument( tar, postac );
	tar = one_argument( tar, nazwa );

	if ( postac[0] == '\0' )
	{
		send_to_char( "Na kogo chcesz rzuciæ to zaklêcie?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room( ch, postac ) ) == NULL )
	{
		print_char( ch, "Nie ma tu nikogo takiego jak '%s'.\n\r", postac );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Nie jeste¶ w stanie rzuciæ tego zaklêcia na siebie.\n\r", ch );
		return;
	}

	if ( get_caster(victim) == -1 )
	{
		act( "Przecie¿ $n nie posiada ¿adnej magicznej mocy.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( nazwa[0] == '\0' )
	{
		send_to_char( "Rzucanie jakiego czaru chcesz zablokowaæ?\n\r", ch );
		return;
	}

	czarek = skill_lookup( nazwa );

	if ( czarek < 0 || skill_table[ czarek ].spell_fun == spell_null )
	{
		print_char( ch, "Nie istnieje takie zaklêcie jak '%s'.\n\r", nazwa );
		return;
	}

	if ( is_affected(victim, gsn_mental_barrier) )
	{
		act( "Umys³ $Z ogranicza ju¿ mentalna bariera.", ch, NULL, victim, TO_CHAR );
		return;
	}

    if ( IS_AFFECTED( victim, AFF_GLOBE ) || IS_AFFECTED( victim, AFF_MAJOR_GLOBE ) || IS_AFFECTED( victim, AFF_ABSOLUTE_MAGIC_PROTECTION ) )
    {
		act( "Twoje zaklêcie znika przy zetkniêciu ze sfer± otaczaj±c± $C.", ch, NULL, victim, TO_CHAR );
		act( "Zaklêcie $z znika przy zetkniêciu z otaczaj±c± ciê sfer±.\n\r", ch, NULL, victim, TO_VICT );
		act( "Zaklêcie $z znika przy zetkniêciu z otaczaj±c± $C sfer±.", ch, NULL, victim, TO_NOTVICT );

		if( victim->fighting == NULL && can_move(victim) && can_see( victim, ch ) && IS_AWAKE(victim) && !is_safe( victim, ch ) )
		{
			if( victim->position < POS_STANDING )
				do_stand( victim, "" );
			multi_hit( victim, ch, TYPE_UNDEFINED );
		}
		return;
	}

	if ( IS_AFFECTED( victim, AFF_REFLECT_SPELL ))
	{
		print_char( ch, "Twoje zaklêcie odbija siê od ochronnej tarczy %s.\n\r", victim->name2 );
		print_char( victim, "Zaklêcie %s odbija siê od ochronnej tarczy.\n\r", ch->name2 );
		act( "Zaklêcie $z odbija siê od ochronnej tarczy $Z.", ch, NULL, victim, TO_NOTVICT );

		if( victim->fighting == NULL && can_move(victim) && can_see( victim, ch ) && IS_AWAKE(victim) && !is_safe( victim, ch ) )
		{
			if( victim->position < POS_STANDING )
				do_stand( victim, "" );
			multi_hit( victim, ch, TYPE_UNDEFINED );
		}
		return;
	}

	if ( number_range( 0, luck + 25 ) == 0 || victim->level + number_range(-5,5) > level + number_range(-5,5) || ( victim->resists[ RESIST_MENTAL ] > 0 && number_percent() < victim->resists[ RESIST_MENTAL ] ) )
	{
		act( "Nie uda³o ci siê ograniczyæ zasobu zaklêæ $Z.", ch, NULL, victim, TO_CHAR );
		act( "$x nie uda³o siê ograniczyæ twego zasobu zaklêæ.", ch, NULL, victim, TO_VICT );
		act( "$x nie uda³o siê ograniczyæ zasobu zaklêæ $Z.", ch, NULL, victim, TO_NOTVICT );

		if( victim->fighting == NULL && can_move(victim) && can_see( victim, ch ) && IS_AWAKE(victim) && !is_safe( victim, ch ) )
		{
			if( victim->position < POS_STANDING )
				do_stand( victim, "" );
			multi_hit( victim, ch, TYPE_UNDEFINED );
		}
		return;
	}

	dur = 1 + level/5;

	if( get_curr_stat_deprecated(ch,STAT_INT) > number_range( 20, 25 ) )
		++dur;

	if( !stat_throw( ch, STAT_LUC ) && !stat_throw( ch, STAT_LUC ) )
		--dur;

	if( stat_throw( ch, STAT_LUC ) && stat_throw( ch, STAT_WIS ) )
		++dur;

	af.where = TO_AFFECTS;
	af.type = gsn_mental_barrier;
	af.level = level;
	af.duration = UMAX( 1, dur ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = czarek;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	print_char( victim, "W twoim umy¶le pojawia siê mentalna bariera, uniemo¿liwiaj±ca rzucenie zaklêcia '%s'.\n\r", skill_table[ czarek ].name );
	print_char( ch, "Tworzysz w umy¶le %s mentaln± barierê, uniemo¿liwiaj±c± rzucenie zaklêcia '%s'.\n\r", PERS( victim, ch ), skill_table[ czarek ].name );
	act( "$n tworzy w umy¶le $Z mentaln± barierê.", ch, NULL, victim, TO_NOTVICT );

	if( victim->fighting == NULL && can_move(victim) && can_see( victim, ch ) && IS_AWAKE(victim) && !is_safe( victim, ch ) )
	{
		if( victim->position < POS_STANDING )
			do_stand( victim, "" );
		multi_hit( victim, ch, TYPE_UNDEFINED );
	}
	return;
}

void spell_defense_curl( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	int total_absorbed_dam;
	int max_absorbed_dam_per_one_hit;
	int dur;
	AFFECT_DATA af;

	if( is_affected(ch,sn))
	{
		send_to_char("Jeste¶ ju¿ otoczon<&y/a/e> ochronn± spiral±.\n\r",ch);
		return;
	}

	if( number_range(0,get_curr_stat_deprecated(ch,STAT_LUC) == 0 ) && !stat_throw(ch,STAT_LUC))
	{
		send_to_char("Nie uda³o ci siê stworzyæ ochronnej spirali.\n\r",ch);
		return;
	}

	total_absorbed_dam = get_curr_stat_deprecated(ch,STAT_INT) + level;
	max_absorbed_dam_per_one_hit = level/4;
	dur = number_range(1,3) + level/5;
	if( stat_throw(ch,STAT_INT)) ++dur;
	if( stat_throw(ch,STAT_WIS)) ++dur;
	if( !stat_throw(ch,STAT_LUC)) --dur;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = max_absorbed_dam_per_one_hit;
	af.duration = UMAX( 1, dur ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = total_absorbed_dam;
	af.bitvector = &AFF_NONE;
	affect_to_char( ch, &af, NULL, TRUE );

	act( "Otaczasz siê ochronn± spiral±.", ch, NULL, NULL, TO_CHAR);
	act( "$n otacza siê ochronn± spiral±.", ch, NULL, NULL, TO_ROOM);

	return;
}

void spell_perfect_senses( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dur;

	if ( number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) ) == 0 && !stat_throw(ch,STAT_LUC ))
	{
		send_to_char( "Wyczuwasz, ¿e co¶ ci niestety nie wysz³o.\n\r", ch );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		if ( victim == ch )
			send_to_char( "Twoje zmys³y nie mog± byæ lepiej chronione.\n\r", ch );
		else
			act( "Zmys³y $Z nie mog± byæ lepiej chronione.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dur = 2 + ( level > 15 ? 1 : 0 ) + ( level > 25 ? 1 : 0 ) + ( level > 35 ? 1 : 0 );
	//spec rzuci troche lepiej niz ogolny
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur += number_range(1,1 + level/10);
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_PERFECT_SENSES;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Czujesz, jak czar zaczyna dzialaæ, a twe zmys³y s± chronione przed wszelkimi zaburzeniami.", victim, NULL, NULL, TO_CHAR );
	act( "Zmys³y $z s± teraz chronione przed wszelkimi zaburzeniami.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_cautious_sleep( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dur;

	if ( number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) ) == 0 && !stat_throw(ch,STAT_LUC ))
	{
		send_to_char( "Wyczuwasz, ¿e co¶ ci niestety nie wysz³o.\n\r", ch );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		if ( victim == ch )
			send_to_char( "Twój sen nie mo¿e byæ bezpieczniejszy.\n\r", ch );
		else
			act( "Sen $Z nie mo¿e byæ bezpieczniejszy.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dur = 2 + level / 5;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_CAUTIOUS_SLEEP;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Najmniejsze nawet zagro¿enie wyrwie ciê ze snu.", victim, NULL, NULL, TO_CHAR );
	act( "Najmniejsze nawet zagro¿enie wyrwie $c ze snu.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_mind_fortess( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dur;

	if ( number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) ) == 0 && !stat_throw(ch,STAT_LUC ))
	{
		send_to_char( "Wyczuwasz, ¿e co¶ ci niestety nie wysz³o.\n\r", ch );
		return;
	}

	if( ch == victim )
	{
		send_to_char( "Zaklêcie nie zadzia³a³o.\n\r", ch );
		return;
	}

	if ( is_affected(ch,sn))
	{
		send_to_char( "Mo¿esz utrzymywaæ opiekê tylko nad jednym umys³em naraz.\n\r", ch );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		act( "Kto¶ inny ju¿ utrzymuje opiekê nad umys³em $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dur = 2 + level / 6;

	if( stat_throw( ch,STAT_INT)) ++dur;
	if( stat_throw( ch,STAT_WIS)) ++dur;
	if( stat_throw( ch,STAT_LUC)) ++dur;
	if( !stat_throw( ch,STAT_INT)) --dur;
	if( !stat_throw( ch,STAT_WIS)) --dur;
	if( !stat_throw( ch,STAT_LUC)) --dur;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, dur); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 1;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	af.modifier = 2;
	af.visible = FALSE;
	affect_to_char( ch, &af, NULL, FALSE );

	act( "Roztaczasz opiekê nad umys³em $Z.", ch, NULL, victim, TO_CHAR );
	act( "Czujesz jak roztacza nad tob± opiekê przyjazny umys³ $Z.", ch, NULL, victim, TO_VICT );
	act( "$n roztacza opiekê nad umys³em $Z.", ch, NULL, victim, TO_NOTVICT );
	return;
}

void spell_fortitude( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dur, mod;

	if ( number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) ) == 0 && !stat_throw(ch,STAT_LUC ))
	{
		send_to_char( "Wyczuwasz, ¿e co¶ ci niestety nie wysz³o.\n\r", ch );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo¿esz byæ lepiej ochranian<&y/a/e>.\n\r", ch );
		else
			act( "$N nie mo¿e byæ lepiej ochranian$R.", ch, NULL, victim, TO_CHAR );
		return;
	}

	dur = 2 + level / 6;

	if( stat_throw( ch,STAT_INT)) ++dur;
	if( stat_throw( ch,STAT_WIS)) ++dur;
	if( stat_throw( ch,STAT_LUC)) ++dur;
	if( !stat_throw( ch,STAT_INT)) --dur;
	if( !stat_throw( ch,STAT_WIS)) --dur;
	if( !stat_throw( ch,STAT_LUC)) --dur;

	mod = 1 + URANGE( 0, level/10, 3 );
	if( stat_throw( ch,STAT_INT)) ++mod;
	if( !stat_throw( ch,STAT_INT)) --mod;
	if( stat_throw( victim,STAT_WIS)) ++mod;
	if( !stat_throw( victim,STAT_WIS)) --mod;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, dur); af.rt_duration = 0;
	af.location = APPLY_SAVING_DEATH;
	af.modifier = -mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	mod = 1 + URANGE( 0, level/10, 3 );
	if( stat_throw( ch,STAT_INT)) ++mod;
	if( !stat_throw( ch,STAT_INT)) --mod;
	if( stat_throw( victim,STAT_WIS)) ++mod;
	if( !stat_throw( victim,STAT_WIS)) --mod;

	af.location = APPLY_SAVING_ROD;
	af.modifier = -mod;
	affect_to_char( victim, &af, NULL, TRUE );

	mod = 1 + URANGE( 0, level/10, 3 );
	if( stat_throw( ch,STAT_INT)) ++mod;
	if( !stat_throw( ch,STAT_INT)) --mod;
	if( stat_throw( victim,STAT_WIS)) ++mod;
	if( !stat_throw( victim,STAT_WIS)) --mod;

	af.location = APPLY_SAVING_PETRI;
	af.modifier = -mod;
	affect_to_char( victim, &af, NULL, TRUE );

	mod = 1 + URANGE( 0, level/10, 3 );
	if( stat_throw( ch,STAT_INT)) ++mod;
	if( !stat_throw( ch,STAT_INT)) --mod;
	if( stat_throw( victim,STAT_WIS)) ++mod;
	if( !stat_throw( victim,STAT_WIS)) --mod;

	af.location = APPLY_SAVING_BREATH;
	af.modifier = -mod;
	affect_to_char( victim, &af, NULL, TRUE );

	mod = 1 + URANGE( 0, level/10, 3 );
	if( stat_throw( ch,STAT_INT)) ++mod;
	if( !stat_throw( ch,STAT_INT)) --mod;
	if( stat_throw( victim,STAT_WIS)) ++mod;
	if( !stat_throw( victim,STAT_WIS)) --mod;

	af.location = APPLY_SAVING_SPELL;
	af.modifier = -mod;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Czujesz siê w pewien niezwyk³y sposób bardziej pewn$r siebie.", victim, NULL, NULL, TO_CHAR );
	switch ( victim->sex )
	{
		case 0:
			act( "$n nagle staje siê bardzo pewne siebie.", victim, NULL, NULL, TO_ROOM );
			break;
		case 1:
			act( "$n nagle staje siê bardzo pewny siebie.", victim, NULL, NULL, TO_ROOM );
			break;
		default:
			act( "$n nagle staje siê bardzo pewna siebie.", victim, NULL, NULL, TO_ROOM );
			break;
	}
	return;
}

void spell_farsight( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	char * tar = ( char * ) vo;

	if ( IS_AFFECTED( ch, AFF_BLIND ) )
	{
		if ( ch->sex == SEX_NEUTRAL )
		{
			send_to_char( "Mo¿e pomog³oby jakby¶ w ogóle widzia³o...?\n\r", ch );
		}
		else if ( ch->sex == SEX_MALE )
		{
			send_to_char( "Mo¿e pomog³oby jakby¶ w ogóle widzia³...?\n\r", ch );
		}
		else
		{
			send_to_char( "Mo¿e pomog³oby jakby¶ w ogóle widzia³a...?\n\r", ch );
		}
		return;
	}

	if ( !tar || tar[ 0 ] == '\0' )
		do_function( ch, &do_scan, "" );
	else
		do_function( ch, &do_scan, tar );
}

void spell_portal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    helper_make_portal( ch, TRUE, sn ); //  magic.c
}

void spell_nexus( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    helper_make_portal( ch, FALSE, sn ); //  magic.c
}

void spell_bonelace( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, luck, vluck, duration;
	AFFECT_DATA af;

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		if (victim == ch)
		{
			send_to_char( "Twoje ko¶ci s± ju¿ przemienione.\n\r", ch );
		}
		else
		{
			act( "Ko¶ci $Z s± ju¿ przemienione.", ch, NULL, victim, TO_CHAR );
		}
		return;
	}

	dam = dice( 2, 6 ) + level / 2;

	// undeadzi
	if ( is_undead(victim) )
	{
		// posiadaj±cy ko¶ci dostan± wzmocnienie
                int mod_hp = 0;

		if ( IS_SET( race_table[ victim->race ].parts, PART_BONES ))
		{
			if ( number_range( 0, luck + LUCK_BASE_MOD ) > 2 )
			{
				mod_hp = ((level/2) * get_max_hp( victim ))/100;

				if (IS_NPC(victim))
				{
					duration = 2 + level/2;
				}
				else
				{
					duration = UMAX( 1, level/3 );
					mod_hp /= 2;
				}
				if ( number_range( 0, luck ) > 15 ) duration += 1;
				if ( number_range( 0, luck ) < 3 ) duration -= 1;

				af.where = TO_AFFECTS;
				af.type = sn;
				af.level = level;
				af.duration = UMAX( 1, duration ); af.rt_duration = 0;
				af.location = APPLY_HIT;
				af.modifier = mod_hp;
				af.bitvector = &AFF_NONE;
				affect_to_char( victim, &af, NULL, TRUE );
			}
			/* Bonus dla specjalisty */
			if ( !IS_NPC( ch ) )
			{
				if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
				{
					af.where = TO_AFFECTS;
					af.type = sn;
					af.level = 1+level/2;
					af.duration = UMAX( 1, duration ); af.rt_duration = 0;
					af.bitvector = &AFF_NONE;
					switch ( number_range(1,50) )
					{
						case 1:
						case 2:
						case 3:
							af.location = APPLY_HITROLL;
							af.modifier = 1;
							break;
						case 5:
						case 6:
						case 7:
							af.location = APPLY_DAMROLL;
							af.modifier = 1;
							break;
						case 8:
							af.location = APPLY_STR;
							af.modifier = 1;
							break;
						case 9:
							af.location = APPLY_INT;
							af.modifier = 1;
							break;
						case 10:
							af.location = APPLY_WIS;
							af.modifier = 1;
							break;
						case 11:
							af.location = APPLY_DEX;
							af.modifier = 1;
							break;
						case 12:
							af.location = APPLY_CON;
							af.modifier = 1;
							break;
						case 13:
							af.location = APPLY_CHA;
							af.modifier = 1;
							break;
						case 14:
							af.location = APPLY_LUC;
							af.modifier = 1;
							break;
						default:
							af.location = APPLY_AC;
							af.modifier = -10;
							break;
					}
					affect_to_char( victim, &af, NULL, TRUE );
				}
			}
			heal_undead( ch, victim, mod_hp );
		}
		else
		{
			send_to_char( "Nic siê nie dzieje.\n\r", ch );
		}
		return;
	}

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

	spell_damage( ch, victim, dam, sn, DAM_NEGATIVE , FALSE );
	act( "$n krzywi siê w okropnym bólu.", victim, NULL, NULL, TO_ROOM );
	act( "Czujesz, jak twoje ko¶ci rozsadza co¶ od ¶rodka!", victim, NULL, NULL, TO_CHAR );

	return;
}

void spell_shadow_swarm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luc = get_curr_stat_deprecated(ch,STAT_LUC), vluc = get_curr_stat_deprecated(victim,STAT_LUC), dur;

	strip_invis( ch, TRUE, TRUE );

	if( is_affected(victim, sn ) )
	{
		if( ch == victim )
			send_to_char( "Nawiedzaj± juz ciê cieniste iluzje.\n\r", ch );
		else
			act( "$N nawiedzaj± ju¿ cieniste iluzje.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if( ch != victim &&
		(number_range( 0, (luc - vluc)*4 + LUCK_BASE_MOD*2 == 1 ) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) ))
	{
		act( "Nie uda³o ci siê sprowadzic cienistych iluzji na $C.", ch, NULL, victim, TO_CHAR );
		if( ch->sex == SEX_FEMALE )
			act( "$n próbowa³a sprowadzic na ciebie cieniste iluzje.", ch, NULL, victim, TO_VICT );
		else
			act( "$n próbowa³ sprowadzic na ciebie cieniste iluzje.", ch, NULL, victim, TO_VICT );
		return;
	}

	dur = 2 + (level-25)/3;
	if( stat_throw( ch, STAT_INT) ) ++dur;
	if( stat_throw( ch, STAT_WIS) ) ++dur;
	if( stat_throw( ch, STAT_LUC) ) ++dur;
	if( stat_throw( victim, STAT_INT) ) --dur;
	if( stat_throw( victim, STAT_WIS) ) --dur;
	if( stat_throw( victim, STAT_LUC) ) --dur;
	if( ch->level > victim->level ) ++dur;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, dur); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	if( ch == victim )
		send_to_char( "Sprowadzasz na siebie cieniste iluzje.\n\r", ch );
	else
	{
		act( "Sprowadzasz cieniste iluzje na $C.", ch, NULL, victim, TO_CHAR );
		act( "Zaczynaj± nawiedzac ciê cieniste iluzje.", ch, NULL, victim, TO_VICT );
	}
	return;
}

void spell_group_unity( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA *gch;
	AFFECT_DATA af;
	int highest_stat = STAT_STR, modified_stat, dur, location = APPLY_NONE, modek, brak = 0;
	char msg_stat_plus[200], msg_stat_minus[200], msg_diff[200], toroom[200];

	for( gch = ch->in_room->people; gch; gch = gch->next_in_room )
	{
		if( ch == gch || is_same_group( ch, gch ) )
		{
			if( is_affected(gch,sn))
			{
				send_to_char( "Twoja grupa stanowi ju¿ jedno¶æ.\n\r", ch );
				return;
			}
		}
	}

	switch( dice(1,3) )
	{
		case 1:
			modified_stat = STAT_STR;
			location = APPLY_STR;
			sprintf( msg_stat_plus, "silniejsz" );
			sprintf( msg_stat_minus, "s³absz" );
			break;
		case 2:
			modified_stat = STAT_CON;
			location = APPLY_CON;
			sprintf( msg_stat_plus, "wytrzymalsz" );
			sprintf( msg_stat_minus, "w±tlejsz" );
			break;
		case 3:
			modified_stat = STAT_DEX;
			location = APPLY_DEX;
			sprintf( msg_stat_plus, "zrêczniejsz" );
			sprintf( msg_stat_minus, "niezdarniejsz" );
			break;
	}

	dur = 2 + (level-25)/3.5;
	if( stat_throw( ch, STAT_INT) ) ++dur;
	if( stat_throw( ch, STAT_WIS) ) ++dur;
	if( stat_throw( ch, STAT_LUC) ) ++dur;
	if( !stat_throw( ch, STAT_INT) ) --dur;
	if( !stat_throw( ch, STAT_WIS) ) --dur;
	if( !stat_throw( ch, STAT_LUC) ) --dur;

	//jak zjebiesz tekscik na lucka, to nie ma typowego nie udalo sie, tylko spell dziala odwrotnie
	if( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) + LUCK_BASE_MOD/2 + get_curr_stat_deprecated(ch,STAT_INT) ) < 4 )
	{
		switch( dice(1,4))
		{
			case 1:
				act( "W trakcie rzucania zaklêcia nieprawid³owo wymawiasz jedno ze s³ów i efekt jest nieciekawy...", ch, NULL, NULL, TO_CHAR );
				break;
			case 2:
				act( "W trakcie rzucania zaklêcia mylisz jedno ze s³ów i efekt jest nieciekawy...", ch, NULL, NULL, TO_CHAR );
				break;
			case 3:
				act( "W trakcie rzucania zaklêcia nag³y powiew wiatru rozprasza twoj± koncentracjê i efekt jest nieciekawy...", ch, NULL, NULL, TO_CHAR );
				break;
			case 4:
				act( "W trakcie rzucania zaklêcia ptak nasra³ ci na g³owê, przez co tracisz koncentracjê i efekt jest nieciekawy...", ch, NULL, NULL, TO_CHAR );
				break;
		}

		//pierw szukamy najgorszej wartosci wsrod grupki
		highest_stat = get_curr_stat_deprecated( ch, modified_stat );

		for( gch = ch->in_room->people; gch; gch = gch->next_in_room )
		{
			if( is_same_group( ch, gch ) )
			{
				if( get_curr_stat_deprecated( gch, modified_stat ) < highest_stat )
				{
					highest_stat = get_curr_stat_deprecated( gch, modified_stat );
				}
			}
		}

		++highest_stat;

		for( gch = ch->in_room->people; gch; gch = gch->next_in_room )
		{
			if( is_same_group( ch, gch ) )
			{
				modek = highest_stat - get_curr_stat_deprecated( gch, modified_stat );

				af.where = TO_AFFECTS;
				af.type = sn;
				af.level = level;
				af.duration = UMAX( 1, dur); af.rt_duration = 0;
				af.location = location;
				af.modifier = modek;
				af.bitvector = &AFF_NONE;
				affect_to_char( gch, &af, NULL, TRUE );

				switch( modek )
				{
					case 0:
						act( "Przez chwilê czujesz przep³ywaj±c± przez ciebie moc, jednak znika ona po chwili.", ch, NULL, gch, TO_VICT );
						break;
					default:
						switch( modek )
						{
							case 1:
							case 2:
							case -1:
							case -2:
								sprintf( msg_diff, " troszkê" );
								break;
							case 3:
							case 4:
							case -3:
							case -4:
								brak = 1;
								break;
							case 5:
							case 6:
							case -5:
							case -6:
								sprintf( msg_diff, " znacznie" );
								break;
							case 7:
							case 8:
							case -7:
							case -8:
								sprintf( msg_diff, " prawie dwa razy" );
								break;
							default:
								sprintf( msg_diff, " praktycznie dwa razy" );
								break;
						}
						print_char( gch, "Czujesz siê%s %s%s.\n\r", brak ? "" : msg_diff, modek > 0 ? msg_stat_plus : msg_stat_minus, ch->sex == SEX_FEMALE ? "a" : (ch->sex == SEX_MALE ? "y" : "e" ) );
						sprintf( toroom, "$n wygl±da na%s %s%s.",brak ? "" : msg_diff, modek > 0 ? msg_stat_plus : msg_stat_minus, ch->sex == SEX_FEMALE ? "±" : (ch->sex == SEX_MALE ? "ego" : "e" ) );
						act( toroom, gch, NULL, NULL, TO_ROOM );
				}
			}
		}
		return;
	}

	highest_stat = get_curr_stat_deprecated( ch, modified_stat );

	for( gch = ch->in_room->people; gch; gch = gch->next_in_room )
	{
		if( is_same_group( ch, gch ) )
		{
			if( get_curr_stat_deprecated( gch, modified_stat ) > highest_stat )
			{
				highest_stat = get_curr_stat_deprecated( gch, modified_stat );
			}
		}
	}

	--highest_stat;

	for( gch = ch->in_room->people; gch; gch = gch->next_in_room )
	{
		if( is_same_group( ch, gch ) )
		{
			modek = highest_stat - get_curr_stat_deprecated( gch, modified_stat );

			af.where = TO_AFFECTS;
			af.type = sn;
			af.level = level;
			af.duration = UMAX( 1, dur); af.rt_duration = 0;
			af.location = location;
			af.modifier = modek;
			af.bitvector = &AFF_NONE;
			affect_to_char( gch, &af, NULL, TRUE );

			switch( modek )
			{
				case 0:
					act( "Przez chwilê czujesz przep³ywaj±c± przez ciebie moc, jednak znika ona po chwili.", ch, NULL, gch, TO_VICT );
					break;
				default:
					switch( modek )
					{
						case 1:
						case 2:
						case -1:
						case -2:
							sprintf( msg_diff, " troszkê" );
							break;
						case 3:
						case 4:
						case -3:
						case -4:
							brak = 1;
							break;
						case 5:
						case 6:
						case -5:
						case -6:
							sprintf( msg_diff, " znacznie" );
							break;
						case 7:
						case 8:
						case -7:
						case -8:
							sprintf( msg_diff, " prawie dwa razy" );
							break;
						default:
							sprintf( msg_diff, " praktycznie dwa razy" );
							break;
					}
					print_char( gch, "Czujesz siê%s %s%s.\n\r", brak ? "" : msg_diff, modek > 0 ? msg_stat_plus : msg_stat_minus, gch->sex == SEX_FEMALE ? "a" : (gch->sex == SEX_MALE ? "y" : "e" ) );
					sprintf( toroom, "$n wygl±da na%s %s%s.",brak ? "" : msg_diff, modek > 0 ? msg_stat_plus : msg_stat_minus, gch->sex == SEX_FEMALE ? "±" : (gch->sex == SEX_MALE ? "ego" : "e" ) );
					act( toroom, gch, NULL, NULL, TO_ROOM );
			}
		}
	}
	return;
}

void spell_share_fitness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int modified_stat = STAT_STR, dur, location = APPLY_NONE, modek = 0, brak = 0, cus = 0, modek2;
	char msg_stat_plus[200], msg_stat_minus[200], msg_diff[200], toroom[200];

	if( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) + LUCK_BASE_MOD/2 + get_curr_stat_deprecated(ch,STAT_INT) ) == 1 )
	{
		act( "Nie uda³o ci siê poszieliæ z $V swoimi zdolno¶ciami.\n\r", ch, NULL, victim, TO_CHAR );
		return;
	}

	//i tu nie ma sprawdzania czy koles nie ma tego spella na sobie - tak ma zostac, ten czar ma byc mozna rzucic kilka razy

	switch( dice(1,3) )
	{
        default:
		case 1:
			modified_stat = STAT_STR;
			location = APPLY_STR;
			sprintf( msg_stat_plus, "silniejsz" );
			sprintf( msg_stat_minus, "s³absz" );
			break;
		case 2:
			modified_stat = STAT_CON;
			location = APPLY_CON;
			sprintf( msg_stat_plus, "wytrzymalsz" );
			sprintf( msg_stat_minus, "w±tlejsz" );
			break;
		case 3:
			modified_stat = STAT_DEX;
			location = APPLY_DEX;
			sprintf( msg_stat_plus, "zrêczniejsz" );
			sprintf( msg_stat_minus, "niezdarniejsz" );
			break;
	}

	dur = 4 + level/6;
	if( stat_throw( ch, STAT_INT) ) ++dur;
	if( stat_throw( ch, STAT_WIS) ) ++dur;
	if( stat_throw( ch, STAT_LUC) ) ++dur;
	if( !stat_throw( ch, STAT_INT) ) --dur;
	if( !stat_throw( ch, STAT_WIS) ) --dur;
	if( !stat_throw( ch, STAT_LUC) ) --dur;

	do
	{
		++cus;
		if( !modek )
			modek = number_range( 1, get_curr_stat_deprecated(ch, modified_stat ) - 4 );
		else
		{
			modek2 = number_range( 1, get_curr_stat_deprecated(ch, modified_stat ) - 4 );
			if( modek2 < modek )
				modek = modek2;
		}
	} while ( cus < 4 );

	if( get_curr_stat_deprecated( ch, modified_stat ) <= 3 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 50;
	af.duration = UMAX( 1, dur); af.rt_duration = 0;
	af.location = location;
	af.modifier = modek;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, "+Czujesz siê nadzwyczaj sprawn<&y/a/e>", TRUE );

	af.modifier = -modek;
	affect_to_char( ch, &af, "-Czujesz siê gorzej ni¿ zazwyczaj", TRUE );

	switch( modek )
	{
		case 1:
		case 2:
			sprintf( msg_diff, " troszkê" );
			break;
		case 3:
		case 4:
			brak = 1;
			break;
		case 5:
		case 6:
			sprintf( msg_diff, " znacznie" );
			break;
		case 7:
		case 8:
			sprintf( msg_diff, " prawie dwa razy" );
			break;
		default:
			sprintf( msg_diff, " praktycznie dwa razy" );
			break;
	}

	print_char( victim, "Czujesz siê%s %s%s.\n\r", brak ? "" : msg_diff, modek > 0 ? msg_stat_plus : msg_stat_minus, victim->sex == SEX_FEMALE ? "a" : (victim->sex == SEX_MALE ? "y" : "e" ) );
	sprintf( toroom, "$n wygl±da na%s %s%s.",brak ? "" : msg_diff, modek > 0 ? msg_stat_plus : msg_stat_minus, victim->sex == SEX_FEMALE ? "±" : (victim->sex == SEX_MALE ? "ego" : "e" ) );
	act( toroom, victim, NULL, NULL, TO_ROOM );

	modek = -modek;

	print_char( ch, "Czujesz siê%s %s%s.\n\r", brak ? "" : msg_diff, modek > 0 ? msg_stat_plus : msg_stat_minus, ch->sex == SEX_FEMALE ? "a" : (ch->sex == SEX_MALE ? "y" : "e" ) );
	sprintf( toroom, "$n wygl±da na%s %s%s.",brak ? "" : msg_diff, modek > 0 ? msg_stat_plus : msg_stat_minus, ch->sex == SEX_FEMALE ? "±" : (ch->sex == SEX_MALE ? "ego" : "e" ) );
	act( toroom, ch, NULL, NULL, TO_ROOM );

	update_pos ( ch );

	return;
}

void spell_dazzling_flash( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	char tekst[500];
	int dur, mod = number_range(-1000,1000);

	if( ch == victim )
	{
		send_to_char( "Przecie¿ to nie ma sensu.\n\r", ch );
		return;
	}

/*	if( is_affected( ch, sn ) )
	{
		send_to_char( "Zaklêciem tym mo¿esz mamiæ wzrok tylko jednej osoby naraz.\n\r", ch );
		return;
	}*/

	if( is_affected( victim, sn ) )
	{
		act( "Wzrok $Z jest ju¿ oszukiwany.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if( IS_AFFECTED( victim, AFF_PERFECT_SENSES ) )
	{
		switch(victim->sex )
		{
			case SEX_MALE:
				act( "$N wydaje siê byæ odporny na to zaklêcie.", ch, NULL, victim, TO_CHAR);
				break;
			case SEX_FEMALE:
				act( "$N wydaje siê byæ odporna na to zaklêcie.", ch, NULL, victim, TO_CHAR);
				break;
			default:
				act( "$N wydaje siê byæ odporne na to zaklêcie.", ch, NULL, victim, TO_CHAR);
				break;
		}
		return;
	}

	act( "Sprowadzasz iluzje na oczy $Z, by te nie mog³y ciê dostrzec.", ch, NULL, victim, TO_CHAR );
	act( "Nagle ca³y ¶wiat wokó³ ciebie rozb³yska, a gdy odzyskujesz wzrok nie widzisz nigdzie $z.", ch, NULL, victim, TO_VICT );
	act( "$N nagle ³apie siê za oczy, przeciera je, po czym patrzy pustym, zdziwionym wzrokiem na $c.", ch, NULL, victim, TO_NOTVICT);

	dur = 3 + level/7.5;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = 0;
	af.duration = UMAX( 1, dur); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, "-Co¶ jest nie tak z twoim wzrokiem", TRUE );

	sprintf( tekst, "+Oszukujesz wzrok %s", victim->name2 );
	af.level = 1;
	affect_to_char( ch, &af, tekst, TRUE );
	return;
}

void spell_brainwash( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	MSPELL_DATA * mspell, *mspell_next;
	int count = 0;

	if( get_caster(victim) < 0 )
	{
		act( "$N nie potrafi zapamiêtaæ nawet jednego zaklêcia.", ch, NULL, victim, TO_CHAR );
		print_char( victim, "%s %s przepraæ ci mó¿g, ale przecie¿ ty nie potrafisz zapamiêtywaæ czarów!", ch->name, ch->sex == SEX_FEMALE ? "próbowa³a" : ( ch->sex == SEX_MALE ? "próbowa³" : "próbowa³o" ) );
		return;
	}

	for ( mspell = victim->memspell; mspell; mspell = mspell_next )
	{
		mspell_next = mspell->next;

		if( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
			continue;

		if( !ch_vs_victim_stat_throw( ch, victim, STAT_INT ) )
			continue;

		if ( victim->resists[ RESIST_MENTAL ] > 0 && number_percent() < victim->resists[ RESIST_MENTAL ] )
			continue;

		remove_mem( victim, mspell );
		++count;
	}

	if( count )
	{
		send_to_char( "Twoja g³owa wydaje siê jako¶ bardziej pusta...\n\r", victim );
		act( "Wymazujesz czê¶æ zaklêæ z g³owy $Z.", ch, NULL, victim, TO_CHAR );
	}
	else
	{
		print_char( victim, "%s %s przepraæ tobie mózg, ale siê %s nie uda³o.", ch->name, ch->sex == SEX_FEMALE ? "próbowa³a" : ( ch->sex == SEX_MALE ? "próbowa³" : "próbowa³o" ), ch->sex == SEX_FEMALE ? "jej" : "mu" );
		act( "Nie uda³o ci siê wymazaæ ¿adnych zaklêæ z g³owy $Z.", ch, NULL, victim, TO_CHAR );
	}
}

void spell_flame_lace( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dam, mod, lev;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluc = get_curr_stat_deprecated( victim, STAT_LUC );

//dam - pierwszy dam
//co pulse_regen (24 jednostki czegostam, 2 rundy walki)
//mod - tyle w sumie przywala _dodatkowe_ ciosy
//lev - tyle zdejmie nastepny pozostaly cios, z kazdym ciosem spada
//jak mod albo lev wyniesie 0 lub mniej affect jest stripowany

	if ( number_range( 0, 25 - vluc + luck) < 1 )
	{
		act( "Ogniste liny mijaj± $c.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Ogniste liny mijaj± ciê o pare centymetrów, osmalaj±c ciê.\n\r", victim );
		return;
	}

	dam = 30 + dice( level, 4);
	mod = dice( level, 2 );
	lev = number_range( level/2, level);

   				if( spell_item_check( ch, sn , "fire based spell" ) )
    {
      dam += 10;
      mod += 2;
      lev += 2;
    }

	act( "Ogniste liny oplataj± cia³o $z.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Ogniste liny oplataj± twoje cia³o parz±c ciê dotkliwie!\n\r", victim );

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam /= 2;
		mod /= 2;
		lev /= 2;
	}

//jak ma juz starszego affecta, to idzie spell od nowa
	if ( is_affected(victim, gsn_burn) )
		affect_strip( victim, gsn_burn);

	af.where = TO_AFFECTS;
	af.type = gsn_burn;
	af.level = lev;
	af.duration = -1; af.rt_duration = 0;//duration -1, bo to schodzi jak wystrzela juz caly dam
	af.location = APPLY_NONE;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	spell_damage( ch, victim, dam, sn, DAM_FIRE, FALSE );
	fire_effect( victim, level, dam, TARGET_CHAR );
	return;
}

//Drake: Czar dla maga inwokacji, tworzy w zaleznosci od poziomu pare ostrzy, ktore beda uderzac we wroga. Czar konczy sie gdy skonczy sie
//przewidziana ilosc ostrzy ktora mag moze sprowadzic. Sproboje dodac jeszcze testowanie ac ofiary ( mozliwosc nie trafienia ostrzem ).
void spell_rain_of_blades( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = NULL, *tch;
	EVENT_DATA *event = NULL;
	int blade_count, shots;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, 25 + luck ) < 1 )
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

		shots = dice(1, 3) +  level / 8 ;

	if ( number_range( 0, luck ) > 15 ) shots++;
	if ( number_range( 0, luck ) < 5 ) shots--;

		shots = UMAX( 1, shots );

	for ( event = event_first; event; event = event->next )
		if ( event->type == EVENT_BLADE && ( CHAR_DATA * ) event->arg1 == ch && ( ROOM_INDEX_DATA * ) event->arg2 == ch->in_room )
		{
			print_char( ch, "Ju¿ przyzwa³e¶ deszcz ostrzy.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "e" );
			return;
		}

	blade_count = UMAX( 1,  level / 4 + ( ( get_curr_stat_deprecated( ch, STAT_LUC ) > 20 ) ? 1 : 0 ) );
	create_event( EVENT_BLADE, number_range( 1, 2 ) * PULSE_VIOLENCE, ch, ch->in_room, blade_count );
	send_to_char( "Gestykuluj±c zawziêcie przyzywasz deszcz ostrzy by zniszczyæ swych wrogów.\n\r", ch );
	act( "$n gestykuluj±c zawziêcie rzuca jaki¶ czar.", ch, NULL, victim, TO_NOTVICT );
	act( "$n gestykuluj±c zawziêcie rzuca jaki¶ czar.", ch, NULL, victim, TO_VICT );
	return;
}
//Frag inwokacji, zadaje losowy typ obrazeñ.
void spell_chaotic_shock( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, damage_bonus = 0, vluck, luck = get_curr_stat_deprecated( ch, STAT_LUC );

	vluck = get_curr_stat_deprecated( victim, STAT_LUC );
	if ( victim == ch )
	{
		send_to_char( "Nie dasz rady tego zrobiæ.\n\r", ch );
		return;
	}

	/* zale¿no¶æ od szczê¶cia */
	if ( number_range( 0, luck + LUCK_BASE_MOD - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ chaotycznej energii.\n\r", ch );
		return;
	}

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
			if ( IS_NPC(ch) || ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) ) )
		{

			damage_bonus = 5 + level/3;
		}
	}

		dam = 30 + dice(( level * 3 ) / 4, 5);

		dam = luck_dam_mod( ch, dam );

		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
			dam /= 2;

	switch ( number_range( 1 , 4 ))
	{
		case 1:
		spell_damage( ch, victim, dam + damage_bonus , sn, DAM_FIRE , FALSE );
		fire_effect( victim, level, dam, TARGET_CHAR );
		act( "$N zostaje popa¿ony mocami chaotycznej magii.", ch, NULL, victim, TO_CHAR );
		act( "$N wypuszcza promieñ chaotycznej mocy, które ogarniaj± ciê i pali ¿ywym ogniem.", victim, NULL, ch, TO_CHAR );
		act( "$n wypuszcza promieñ chaotycznej mocy w kierunku $Z.", ch, NULL, victim, TO_NOTVICT );
		break;

	  case 2:
		spell_damage( ch, victim, dam + damage_bonus, sn, DAM_LIGHTNING , FALSE );
		shock_effect( victim, level, dam, TARGET_CHAR );
		act( "$N zostaje pora¿ony mocami chaotycznej magii.", ch, NULL, victim, TO_CHAR );
		act( "$N wypuszcza promieñ chaotycznej mocy, które ogarniaj± ciê i pora¿a energi±.", victim, NULL, ch, TO_CHAR );
		act( "$n wypuszcza promieñ chaotycznej mocy w kierunku $Z.", ch, NULL, victim, TO_NOTVICT );
		break;

		case 3:
		spell_damage( ch, victim, dam + damage_bonus, sn, DAM_COLD , FALSE );
		cold_effect( victim, level, dam, TARGET_CHAR );
		act( "$N zostaje wych³odzony mocami chaotycznej magii.", ch, NULL, victim, TO_CHAR );
		act( "$N wypuszcza promieñ chaotycznej mocy, które ogarniaj± ciê i mro¼i twoje cia³o.", victim, NULL, ch, TO_CHAR );
		act( "$n wypuszcza promieñ chaotycznej mocy w kierunku $Z.", ch, NULL, victim, TO_NOTVICT );
		break;

		case 4:
		spell_damage( ch, victim, dam + damage_bonus, sn, DAM_ACID , FALSE );
		acid_effect( victim, level, dam, TARGET_CHAR );
		act( "$N zostaje zatopiony w mocach chaotycznej magii.", ch, NULL, victim, TO_CHAR );
		act( "$N wypuszcza promieñ chaotycznej mocy, które ogarniaj± ciê pali niczym ¿r±cy kwas.", victim, NULL, ch, TO_CHAR );
		act( "$n wypuszcza promieñ chaotycznej mocy w kierunku $Z.", ch, NULL, victim, TO_NOTVICT );
	  break;

	}
	return;
}

/*Frag zadaj±cy obra¿enia typu mental ze szko³y iluzji, s³abszy ni¿ inne czary bojowe na podobnych krêgach, ale dostaj±cy bonusy
za ka¿dy negatywny efekt oddzia³uj±cy na umys³ celu. Ta wersja czaru dzia³a obszarowo i jest dostêpna tylko dla specjalisty. */
void spell_psychic_scream( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA *vch, *vch_next;
	int dam, dam_bonus = 0, vluck, luck;

	/* luck */

  luck = get_curr_stat_deprecated( ch, STAT_LUC ),
  vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê zaatakowaæ umys³ów wrogów.\n\r", ch );
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Nie dasz rady tego zrobiæ.\n\r", ch );
		return;
	}

	print_char( ch, "Twój psychiczny krzyk uderza w %s i roznosi siê atakujac wszystkich wrogów w obszarze.\n\r", victim->name4 );
	act( "Moc $z trafia w $C i rozsiewa siê dooko³a, ogarniaj±c wszystkich wrogów w obszarze.", ch, NULL, victim, TO_NOTVICT );

	for ( vch = ch->in_room->people;vch;vch = vch_next )
	{
		vch_next = vch->next_in_room;
		if ( is_same_group( ch, vch ) )
			continue;

		dam = dice( 8, 4) + dice( level, 2 );

		dam = luck_dam_mod( ch, dam );

		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
			if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
			{
				dam -= dam / 4;
			}
		}

	if( IS_AFFECTED( victim, AFF_CONFUSION ) )
		{
		dam_bonus += 5;
		return;
		}

	if( IS_AFFECTED( victim, AFF_DAZE ) )
	{
		dam_bonus += 5;
		return;
	}

	if( IS_AFFECTED( victim, AFF_FEAR ) )
	{
		dam_bonus += 5;
		return;
	}

	if( IS_AFFECTED( victim, AFF_HALLUCINATIONS_NEGATIVE ) )
	{
		dam_bonus += 5;
		return;
	}

		if( IS_AFFECTED( victim, AFF_HALLUCINATIONS_POSITIVE ) )
	{
		dam_bonus += 5;
		return;
	}

			if( IS_AFFECTED( victim, AFF_CHARM ) )
	{
		dam_bonus += 5;
		return;
	}

    	if (IS_AFFECTED( vch, AFF_GLOBE ) || IS_AFFECTED( vch, AFF_MAJOR_GLOBE ) || IS_AFFECTED( vch, AFF_ABSOLUTE_MAGIC_PROTECTION  ))
    	{
			act( "Twój psychiczny krzyk znika przy zetkniêciu ze sfer± otaczaj±c± $C.", ch, NULL, vch, TO_CHAR );
			act( "Moc zaklêcia $z znika przy zetkniêciu z otaczaj±c± ciê sfer±.\n\r", ch, NULL, vch, TO_VICT );
			act( "Moc zaklêcia $z znika przy zetkniêciu z otaczaj±c± $C sfer±.", ch, NULL, vch, TO_NOTVICT );
			return;
	}

		if ( IS_AFFECTED( vch, AFF_REFLECT_SPELL ))
		{
			print_char( ch, "Twoje zaklêcie odbija siê od ochronnej tarczy %s.\n\r", vch->name2 );
			print_char( vch, "Zaklêcie %s odbija siê od ochronnej tarczy.\n\r", ch->name2 );
			act( "Zaklêcie $z odbija siê od ochronnej tarczy $Z.", ch, NULL, vch, TO_NOTVICT );
			vch = ch;
		}

		if (is_undead(victim) || IS_SET( victim->form, FORM_CONSTRUCT )  || !IS_SET( victim->parts, PART_BRAINS ) )
	{
		act( "Te zaklêcie nie zadzia³a na $C.", ch, NULL, victim, TO_CHAR );
		spell_damage( ch, vch, 0, sn, DAM_MENTAL , FALSE );
	}
		else
	{
		send_to_char( "Psychiczny krzyk rozbrzmiewa w twojej g³owie, przywo³uj±c najgorsze koszmary twojego ¿ycia.\n\r", vch );
		act( "Psychiczny krzyk ogarnia $c.", vch, NULL, NULL, TO_NOTVICT );
		spell_damage( ch, vch, dam+dam_bonus, sn, DAM_MENTAL , FALSE );
	}

	}
	return;
}

//przerobiony na nowe staty
void spell_bull_strength( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod;
	int luck = get_curr_stat( ch, STAT_LUC );
	int dur = 4 + level / 10;

    if ( check_improve_strenth ( ch, victim, TRUE ) ) return;

        	if (is_undead(victim) || IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		act( "Te zaklêcie nie zadzia³a na $C.", ch, NULL, victim, TO_CHAR );
		return;
	}

  	if ( number_range( 0, 25 + luck/6 ) < 1 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ w³asnej si³y.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ si³y $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

    switch ( ch->class )
    {
        case CLASS_MAG:
            mod = number_range( 3, 10 );
            break;
        case CLASS_CLERIC:
        case CLASS_DRUID:
            mod = number_range( 3, 10 );
            break;
        case CLASS_PALADIN:
            mod = number_range( 2, 6 );
            break;
        default:
            mod = number_range( 2, 6 );
            break;
    }

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 5 + level / 10;
			if ( number_range(1,2) == 1 )
			{
				mod = mod + number_range( 2, 5 );;
			}
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_STR;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Czujesz dziwn± energiê rozchodz±c± siê po twoim ciele!\n\r", victim );

	if ( victim->sex == SEX_FEMALE )
		act( "$n wydaje siê byc silniejsza.", victim, NULL, NULL, TO_ROOM );
	else if ( victim->sex == SEX_MALE )
		act( "$n wydaje siê byc silniejszy.", victim, NULL, NULL, TO_ROOM );
	else
		act( "$n wydaje siê byc silniejsze.", victim, NULL, NULL, TO_ROOM );
	return;
}

//przerobiony na nowe staty
void spell_cat_grace( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod;
	int luck = get_curr_stat( ch, STAT_LUC );
	int dur = 4 + level / 10;
	int spirit_power = get_skill(ch, gsn_invoke_spirit)/3;

	    	if (is_undead(victim) || IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		act( "Te zaklêcie nie zadzia³a na $C.", ch, NULL, victim, TO_CHAR );
		return;
	}

   if (ch->class != CLASS_SHAMAN)
	if ( number_range( 0, 25 + luck/4 ) < 1 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ w³asnej zrêczno¶ci.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ zrêczno¶ci $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_affected( victim, sn ) || is_affected( victim, 487 /*8 kregowy nimbleness*/ ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Jeste¶ ju¿ tak zrêczn<&y/a/e> jak tylko potrafisz!\n\r", ch );
		}
		else
		{
			send_to_char( "Uda³o ci siê rzuciæ zaklêcie.\n\r", ch );
		}
		return;
	}

	switch ( ch->class )
	{
		case CLASS_MAG:
			mod = number_range( 4, 10 );
			break;
		case CLASS_CLERIC:
		case CLASS_DRUID:
			mod = number_range( 4, 10 );
			break;
		case CLASS_PALADIN:
			mod = number_range( 3, 6 );
			break;
        case CLASS_SHAMAN:
			if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
			{
				spirit_power += 5;
			}
			if ( check_shaman_invoke ( ch ) == TRUE )
			{
				mod = number_range (4, 10);
				dur = 4 + spirit_power/10;
			}
			else
			{
				send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
				return;
			}
			break;
		default: mod = 3;break;
	}

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 5 + level / 10;
			if ( number_range(1,2) == 1 )
			{
				mod += number_range( 3, 6 );;
			}
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_DEX;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Wydaje ci siê, ¿e teraz uda siê ka¿da sztuczka!\n\r", victim );

	if ( victim->sex == SEX_FEMALE )
		act( "$n wydaje siê byc zrêczniejsza.", victim, NULL, NULL, TO_ROOM );
	else if ( victim->sex == SEX_MALE )
		act( "$n wydaje siê byc zrêczniejszy.", victim, NULL, NULL, TO_ROOM );
	else
		act( "$n wydaje siê byc zrêczniejsze.", victim, NULL, NULL, TO_ROOM );
	return;
}

//przerobiony na nowe staty
void spell_owl_wisdom( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod;
	int luck = get_curr_stat( ch, STAT_LUC );
	int dur = 4 + level / 10;

	if ( number_range( 0, 25 + luck/4 ) < 1 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ w³asnej m±dro¶ci.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ m±dro¶ci $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_affected( victim, sn ) || is_affected( victim, 488 /*8 kregowy draconic wisdom*/ ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Jeste¶ ju¿ tak madr<&y/a/e> jak tylko potrafisz!\n\r", ch );
		}
		else
		{
			send_to_char( "Uda³o ci siê rzuciæ zaklêcie.\n\r", ch );
		}
		return;
	}

	switch ( ch->class )
	{
		case CLASS_MAG:
			mod = number_range( 4, 10 );
			break;
		case CLASS_CLERIC:
		case CLASS_DRUID:
			mod = number_range( 4, 10 );
			break;
		case CLASS_PALADIN:
			mod = number_range( 4, 6 );
			break;

		default: mod = 3;break;
	}

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 5 + level / 10;
			if ( number_range(1,2) == 1 )
			{
				mod += number_range( 3, 6);
			}
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_WIS;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Teraz wiesz wiêcej o tym ca³ym ¶wiecie!\n\r", victim );

	if ( victim->sex == SEX_FEMALE )
		act( "$n wydaje siê byc m±drzejsza.", victim, NULL, NULL, TO_ROOM );
	else if ( victim->sex == SEX_MALE )
		act( "$n wydaje siê byc m±drzejszy.", victim, NULL, NULL, TO_ROOM );
	else
		act( "$n wydaje siê byc m±drzejsze.", victim, NULL, NULL, TO_ROOM );
	return;
}

//przerobiony na nowe staty
void spell_fox_cunning( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod;
	int luck = get_curr_stat( ch, STAT_LUC );
	int dur = 4 + level / 10;

	if ( number_range( 0, 25 + luck/4 ) < 1 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ w³asnej inteligencji.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ inteligencji $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_affected( victim, sn ) || is_affected( victim, 489 /*8 kregowy insight*/ ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Jeste¶ ju¿ tak inteligent<&y/a/e> jak tylko potrafisz!\n\r", ch );
		}
		else
		{
			send_to_char( "Uda³o ci siê rzuciæ zaklêcie.\n\r", ch );
		}
		return;
	}

	switch ( ch->class )
	{
		case CLASS_MAG:
			mod = number_range( 4, 10 );
			break;
		case CLASS_CLERIC:
		case CLASS_DRUID:
			mod = number_range( 4, 10 );
			break;
		case CLASS_PALADIN:
			mod = number_range( 3, 6 );
			break;

		default: mod = 3;break;
	}

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 5 + level / 10;
			if ( number_range(1,2) == 1 )
			{
				mod += number_range( 3, 6 );;
			}
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_INT;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Twój umys³ zaczyna dzia³aæ ze zwiekszon± sprawno¶ci±!\n\r", victim );

	if ( victim->sex == SEX_FEMALE )
		act( "$n wydaje siê byc inteligentniejsza.", victim, NULL, NULL, TO_ROOM );
	else if ( victim->sex == SEX_MALE )
		act( "$n wydaje siê byc inteligentniejszy.", victim, NULL, NULL, TO_ROOM );
	else
		act( "$n wydaje siê byc inteligentniejsze.", victim, NULL, NULL, TO_ROOM );
	return;
}

//przerobiony na nowe staty
void spell_bear_endurance( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod;
	int luck = get_curr_stat( ch, STAT_LUC );
	int dur = 4 + level / 10;
	int spirit_power = get_skill(ch, gsn_invoke_spirit)/3;

	if (is_undead(victim) || IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		act( "Te zaklêcie nie zadzia³a na $C.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if (ch->class != CLASS_SHAMAN)
		if ( number_range( 0, 25 + luck/4 ) < 1 )
		{
			if ( victim == ch ) send_to_char( "Nie uda³o ci siê zwiêkszyæ w³asnej kondycji.\n\r", ch );
			else act( "Nie uda³o ci siê zwiêkszyæ kondycji $Z.", ch, NULL, victim, TO_CHAR );
			return;
		}

	if ( is_affected( victim, sn ) || is_affected( victim, 490 /*8 kregowy behemot toughness*/ ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Jeste¶ ju¿ tak wytrzyma³<&y/a/e> jak tylko potrafisz!\n\r", ch );
		}
		else
		{
			send_to_char( "Uda³o ci siê rzuciæ zaklêcie.\n\r", ch );
		}
		return;
	}

	switch ( ch->class )
	{
		case CLASS_MAG:
			mod = number_range( 4, 10 );
			break;
		case CLASS_CLERIC:
		case CLASS_DRUID:
			mod = number_range( 4, 10 );
			break;
		case CLASS_PALADIN:
			mod = number_range( 3, 6 );
			break;
        case CLASS_SHAMAN:
			if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )spirit_power += 5;
			if ( check_shaman_invoke ( ch ) == TRUE )
			{
				mod = number_range (4, 10);
				dur = 4 + spirit_power/10;
			}
			else
			{
				send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
				return;
			}

			break;

		default: mod = 3;break;
	}

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 5 + level / 10;
			if ( number_range(1,2) == 1 )
			{
				mod += number_range( 3, 6 );
			}
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_CON;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Twoje cia³o zaczyna dzia³aæ ze zwiekszon± sprawno¶ci±!\n\r", victim );

	if ( victim->sex == SEX_FEMALE )
		act( "$n wydaje siê byc bardziej wytrzyma³a.", victim, NULL, NULL, TO_ROOM );
	else if ( victim->sex == SEX_MALE )
		act( "$n wydaje siê byc bardziej wytrzyma³y.", victim, NULL, NULL, TO_ROOM );
	else
		act( "$n wydaje siê byc bardziej wytrzyma³e.", victim, NULL, NULL, TO_ROOM );
	return;
}

//przerobiony na nowe staty
void spell_eagle_splendor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod;
	int luck = get_curr_stat( ch, STAT_LUC );
	int dur = 4 + level / 10;

	if ( number_range( 0, 25 + luck/4 ) < 1 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ w³asnej charyzmy.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ charyzmy $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_affected( victim, sn ) || is_affected( victim, 491 /*8 kregowy inspiring presence*/ ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Jeste¶ ju¿ tak charyzmatyczn<&y/a/e> jak tylko potrafisz!\n\r", ch );
		}
		else
		{
			send_to_char( "Uda³o ci siê rzuciæ zaklêcie.\n\r", ch );
		}
		return;
	}

	switch ( ch->class )
	{
		case CLASS_MAG:
			mod = number_range( 4, 10 );
			break;
		case CLASS_CLERIC:
		case CLASS_DRUID:
			mod = number_range( 4, 10 );
			break;
		case CLASS_PALADIN:
			mod = number_range( 3, 6 );;
			break;

		default: mod = 3;break;
	}

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur = 5 + level / 10;
			if ( number_range(1,2) == 1 )
			{
				mod += number_range( 3, 6 );;
			}
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_CHA;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Zaczynasz wygl±daæ wprost rewelacyjnie!\n\r", victim );

	if ( victim->sex == SEX_FEMALE )
		act( "$n wydaje siê byc bardziej charyzmatyczna.", victim, NULL, NULL, TO_ROOM );
	else if ( victim->sex == SEX_MALE )
		act( "$n wydaje siê byc bardziej charyzmatyczny.", victim, NULL, NULL, TO_ROOM );
	else
		act( "$n wydaje siê byc bardziej charyzmatyczne.", victim, NULL, NULL, TO_ROOM );
	return;
}

//przerobiony na nowe staty
void spell_nimbleness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, duration, mod;

	    	if (is_undead(victim) || IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		act( "Te zaklêcie nie zadzia³a na $C.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_affected( victim, sn ) || is_affected( victim, 482 /*2 kregowy cat grace*/ ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Jeste¶ ju¿ tak zrêczn<&y/a/e> jak tylko potrafisz!\n\r", ch );
		}
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie mo¿e byæ ju¿ zrêczniejsze.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N nie mo¿e byæ ju¿ zrêczniejszy.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo¿e byæ ju¿ zrêczniejsza.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	luck = get_curr_stat( ch, STAT_LUC );

	/* zaleznosc od szczescia */
	if ( number_range( 0, luck/4 + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ swej zrêczno¶ci.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ zrêczno¶ci $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 4 +  level/ 10;
	mod = number_range( 10, 18 );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_DEX;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;

	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Twój refleks zaczyna dzia³aæ ze znacznie zwiêkszon± sprawno¶ci±!\n\r", victim );
	act( "Refleks $z zaczynaj± dzia³aæ ze znacznie zwiêkszon± sprawno¶ci±.", victim, NULL, NULL, TO_ROOM );
	return;
}

//przerobiony na nowe staty
void spell_draconic_wisdom( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, duration, mod;

	if ( is_affected( victim, sn ) || is_affected( victim, 483 /*2 kregowy owl wisdom*/ ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Jeste¶ ju¿ tak m±dr<&y/a/e> jak tylko potrafisz!\n\r", ch );
		}
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie mo¿e byæ ju¿ m±drzejsze.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N nie mo¿e byæ ju¿ m±drzejszy.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo¿e byæ ju¿ m±drzejsza.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	luck = get_curr_stat( ch, STAT_LUC );

	/* zaleznosc od szczescia */
	if ( number_range( 0, luck/4 + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ swej m±dro¶ci.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ m±dro¶ci $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 4 +  level/ 10;
	mod = number_range( 10, 18 );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_WIS;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;

	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Twoj± g³owê wype³nia staro¿ytna wiedza, prawie równa tej nale¿±cej do staro¿ytnych smoków!\n\r", victim );
	act( "G³owê $z wype³niaj± m±dro¶ci staro¿ytnych.", victim, NULL, NULL, TO_ROOM );
	return;
}

//przerobiony na nowe staty
void spell_insight( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, duration, mod;

	if ( is_affected( victim, sn ) || is_affected( victim, 484 /*2 kregowy fox cunning*/ ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Jeste¶ ju¿ tak inteligentn<&y/a/e> jak tylko potrafisz!\n\r", ch );
		}
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie mo¿e byæ ju¿ inteligêtniejsze.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N nie mo¿e byæ ju¿ inteligêtniejszy.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo¿e byæ ju¿ inteligêtniejsza.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	luck = get_curr_stat( ch, STAT_LUC );

	/* zaleznosc od szczescia */
	if ( number_range( 0, luck/4 + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ swej inteligencji.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ inteligencji $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 4 +  level/ 10;
	mod = number_range( 10, 18 );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_INT;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;

	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Twój umys³ zaczyna dzia³aæ ze znacznie zwiêkszona sprawno¶cia!\n\r", victim );
	act( "Umys³ $z zaczynaj± dzia³aæ ze znacznie zwiêkszon± sprawno¶ci±.", victim, NULL, NULL, TO_ROOM );
	return;
}

//przerobiony na nowe staty
void spell_behemot_toughness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, duration, mod;

	    	if (is_undead(victim) || IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		act( "Te zaklêcie nie zadzia³a na $C.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_affected( victim, sn ) || is_affected( victim, 485 /*2 kregowy bear endurance*/ ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Jeste¶ ju¿ tak wytrzyma³<&y/a/e> jak tylko potrafisz!\n\r", ch );
		}
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie mo¿e byæ ju¿ wytrzymalsze.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N nie mo¿e byæ ju¿ wytrzymalszy.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo¿e byæ ju¿ wytrzymalsza.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	luck = get_curr_stat( ch, STAT_LUC );

	/* zaleznosc od szczescia */
	if ( number_range( 0, luck/4 + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ swej kondycji.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ kondycji $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 4 +  level/ 10;
	mod = number_range( 10, 18 );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_CON;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;

	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Twoje cia³o wype³nia dziwna si³a, czujesz siê o wiele lepiej!\n\r", victim );
	act( "Cia³o $z zaczynaj± dzia³aæ ze znacznie zwiêkszon± sprawno¶ci±.", victim, NULL, NULL, TO_ROOM );
	return;
}

//przerobiony na nowe staty
void spell_inspiring_presence( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod;
	int luck = get_curr_stat( ch, STAT_LUC );
	int dur = 4 + level / 10;

	if ( number_range( 0, 25 + luck/4 ) < 1 )
	{
		if ( victim == ch )
			send_to_char( "Nie uda³o ci siê zwiêkszyæ w³asnej charyzmy.\n\r", ch );
		else
			act( "Nie uda³o ci siê zwiêkszyæ charyzmy $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_affected( victim, sn ) || is_affected( victim, 486 /*2 kregowy eagle splendor*/ ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Jeste¶ ju¿ tak charyzmatyczn<&y/a/e> jak tylko potrafisz!\n\r", ch );
		}
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie mo¿e byæ ju¿ bardziej inspiruj±ce.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N nie mo¿e byæ ju¿ bardziej inspiruj±cy.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N nie mo¿e byæ ju¿ bardziej inspiruj±ca.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	mod = number_range( 10, 18 );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.location = APPLY_CHA;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Czuj±c dzia³anie magii prostujesz siê dumnie i spogl±dasz na wszystkich z wy¿szo¶ci±.\n\r", victim );

    switch ( victim->sex )
    {
        case SEX_FEMALE:
            act( "$n zaczyna siê zachowywaæ jako¶ inaczej, czujesz przed ni± dziwny respekt.", victim, NULL, NULL, TO_ROOM );
            break;
        case SEX_NEUTRAL:
            act( "$n zaczyna siê zachowywaæ jako¶ inaczej, czujesz przed tym dziwny respekt.", victim, NULL, NULL, TO_ROOM );
            break;
        case SEX_MALE:
        default:
            act( "$n zaczyna siê zachowywaæ jako¶ inaczej, czujesz przed nim dziwny respekt.", victim, NULL, NULL, TO_ROOM );
            break;
    }

	return;
}

void spell_mind_strike( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    int dam;
    /* luck */
    int luck = get_curr_stat_deprecated( ch, STAT_LUC );
    int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

    if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
    {
        send_to_char( "Nie uda³o ci siê wyzwoliæ mocy.\n\r", ch );
        return;
    }

    if (is_undead(victim) || IS_SET( victim->form, FORM_CONSTRUCT ) || !IS_SET( victim->parts, PART_BRAINS ) )
    {
        act( "Te zaklêcie nie zadzia³a na $C.", ch, NULL, victim, TO_CHAR );
        return;
    }

    dam = dice( 2, 5 ) + level/2;
    /* Bonus dla specjalisty */
    if ( !IS_NPC( ch ) )
    {
        if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
        {
            dam = dice( 2, 5 ) + level;
        }
    }
    dam += mind_based_spell_bonus ( victim, 2 );

    act( "Niewielki, purpurowy pocisk trafia w $c.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "Niewielki, purpurowy pocisk trafia ciê prosto w g³owê, powoduj±c olbrzymi ból.\n\r", victim );
    spell_damage( ch, victim, dam, sn, DAM_MENTAL, FALSE );

    return;
}

void spell_mind_blast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    int dam;
    /* luck */
    int luck = get_curr_stat_deprecated( ch, STAT_LUC );
    int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

    if ( number_range( 0, LUCK_BASE_MOD + luck - vluck ) == 0 )
    {
        send_to_char( "Nie uda³o ci siê wyzwoliæ mocy.\n\r", ch );
        return;
    }

    if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) || !IS_SET( victim->parts, PART_BRAINS ) )
    {
        act( "Te zaklêcie nie zadzia³a na $C.", ch, NULL, victim, TO_CHAR );
        return;
    }

    dam = dice(3, 5) + dice(level, 2);

    /* Bonus dla specjalisty */
    if ( !IS_NPC( ch ) )
    {
        if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
        {
            dam = dice(4, 5) + dice(level, 3);
        }
    }
    dam += mind_based_spell_bonus ( victim, 4 );

    act( "Sporej wielko¶ci purporowy pocisk wylatujê i trafia prosto w $c.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "Sporej wielko¶ci, purpurowy pocisk trafia ciê prosto w g³owê, powoduj±c nieopisany ból.\n\r", victim );
    spell_damage( ch, victim, dam, sn, DAM_MENTAL, FALSE );

    return;
}

void spell_doom( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  return;
}

void spell_finger_of_death ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, hp_percent, mod, mod2;

   if ( ch == victim  )
	{
		send_to_char( "Raczej nie dasz rady tego zrobiæ.\n\r", ch );
		return;
	}

	act( "$n wymawiaj±c s³owa staro¿ytne inkantacji celuje palcem i wypuszcza ciemnoczerwony promieñ w kierunku $C.", ch, NULL, victim, TO_NOTVICT );
	act( "$n wymawiaj±c s³owa staro¿ytnej inkantacji celuje palcem w ciebie, po czym wypuszcza ciemnoczerwony promieñ który trafia w ciebie sprawiaj±c nieopisany ból!", ch, NULL, victim, TO_VICT );
	act( "Przyzywaj±c moce staro¿ytnej magii celujesz palcem i wypuszczasz promieñ skondensowanej, negatywnej energii prosto w $C.", ch, NULL, victim, TO_CHAR );

	dam = number_range( 80, 120 ) + URANGE ( 1, dice( level, 8 ), 220);
	mod = 30;//Instant death od oko³o polowy bardzo ciezkich ran
	mod2 = 100;//Instant death je¿eli ilo¶æ hp jest mniejsza ni¿
   hp_percent = 100 * victim->hit / UMAX( 1, get_max_hp(victim) );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		mod -= 5;
		mod2 -= 25;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
			mod -= 5;
			mod2 -= 25;
		}
	}

	if ( hp_percent < mod && victim->hit < mod2 && !IS_AFFECTED( victim, AFF_ENERGY_SHIELD ) && !is_undead(victim)  )
   {
	act( "Twoja magia pozbawia $C ¿ycia!", ch, NULL, victim, TO_CHAR );
	act( "Magia $z pozbawia ¿ycia $C!", ch, NULL, victim, TO_NOTVICT );
	act( "Magia $z pozbawia ciê ¿ycia!", ch, NULL, victim, TO_VICT );
	raw_damage( ch, victim, 20 + get_max_hp(victim) );
	return;
   }
   else
	spell_damage( ch, victim, dam, sn, DAM_NEGATIVE, FALSE );
}

void spell_elemental_devastation ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    int dam, eff_lev;

    if ( ch == victim  )
    {
        send_to_char( "Raczej nie dasz rady tego zrobiæ.\n\r", ch );
        return;
    }

    dam = number_range( 120, 180 ) + URANGE( 80, dice(level, 8), 220 );
    eff_lev = level;

    if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
    {
        dam -= dam / 4;
        eff_lev -= eff_lev / 4;
        if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
        {
            dam -= dam / 4;
            eff_lev -= eff_lev / 4;
        }
    }
    act( "Wypowiadasz staro¿ytn± inkantacjê i uderzasz w $Z z ca³± moc± elementarnej energi.", ch, NULL, victim, TO_CHAR );
    switch ( number_range( 1 , 4 ))
    {
        case 1:
            if( !spell_item_check( ch, sn , "fire based spell" ) )
            {
                dam /= 3;
                act( "Niestety, czê¶æ zgromadzonej przez ciebie, elementarnej energii rozprasza siê dooko³a.", ch, NULL, victim, TO_CHAR );
            }
            act( "$n wypowiada s³owa staro¿ytnej inkantacji, dooko³a twoich stóp pojawias siê fontanna ognia, która w oka mgnieniu poch³ania ca³e twoje cia³o!", victim, NULL, ch, TO_CHAR );
            act( "$n wypowiada s³owa staro¿ytnej inkantacji, dooko³a stóp $Z pojawia siê naglê i wytryskuje w górê fontanna ognia!", ch, NULL, victim, TO_NOTVICT );
            fire_effect( victim, level, dam, TARGET_CHAR );
            spell_damage( ch, victim, dam, sn, DAM_FIRE , FALSE );
            break;

        case 2:
            if( !spell_item_check( ch, sn , "lighting based spell" ) )
            {
                dam /= 3;
                act( "Niestety, czê¶æ zgromadzonej przez ciebie, elementarnej energii rozprasza siê dooko³a.", ch, NULL, victim, TO_CHAR );
            }
            act( "$n wypowiada s³owa staro¿ytnej inkantacji, po chwili jakby z nik±d uderza w ciebie potê¿ny piorun!", victim, NULL, ch, TO_CHAR );
            act( "$n wypowiada s³owa staro¿ytnej inkantacji, po chwili jakby z nik±d w $Z uderza olbrzymi piorun.", ch, NULL, victim, TO_NOTVICT );
            shock_effect( victim, level, dam, TARGET_CHAR );
            spell_damage( ch, victim, dam, sn, DAM_LIGHTNING , FALSE );
            break;

        case 3:
            if( !spell_item_check( ch, sn , "cold based spell" ) )
            {
                dam /= 3;
                act( "Niestety, czê¶æ zgromadzonej przez ciebie, elementarnej energii rozprasza siê dooko³a.", ch, NULL, victim, TO_CHAR );
            }
            act( "$n wypowiada s³owa staro¿ytnej inkantacji, naglê powietrze dooko³a ciebie staje siê zimne niczym lód, czujesz jak krew prawie zamarza ci w ¿y³ach!", victim, NULL, ch, TO_CHAR );
            act( "$n wypowiada s³owa staro¿ytnej inkantacji, nagle powietrze dooko³a $Z staje siê zimne niczym lód, zauwa¿asz p³atki ¶niegu pojawiaj±ce siê dooko³a.", ch, NULL, victim, TO_NOTVICT );
            cold_effect( victim, level, dam, TARGET_CHAR );
            spell_damage( ch, victim, dam, sn, DAM_COLD , FALSE );
            break;

        case 4:
        default:
            if( !spell_item_check( ch, sn , "acid based spell" ) )
            {
                dam /= 3;
                act( "Niestety, czê¶æ zgromadzonej przez ciebie, elementarnej energii rozprasza siê dooko³a.", ch, NULL, victim, TO_CHAR );
            }
            act( "$n wypowiada s³owa staro¿ytnej inkantacji, w powietrzu dooko³a ciebie pojawiaj± siê niewielkie kulê kwasu, które uderzaj± w ciebie pal±c twoje cia³o!", victim, NULL, ch, TO_CHAR );
            act( "$n wypowiada s³owa staro¿ytnej inkantacji, w powietrzu dooko³a $Z pojawiaj± siê niewielkie kulê kwasu, które bezlito¶nie spadaj± na ofiarê.", ch, NULL, victim, TO_NOTVICT );
            spell_damage( ch, victim, dam, sn, DAM_ACID , FALSE );
            acid_effect( victim, level, dam, TARGET_CHAR );
            break;
    }
}

void spell_reverse_gravity( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam1, dam2;

      if ( ch == victim  )
	{
		send_to_char( "Raczej nie dasz rady tego zrobiæ.\n\r", ch );
		return;
	}

	dam1 = number_range( 60, 120 ) + URANGE(40, dice( level, 4 ), 120);
	dam2 = number_range( 40, 80 ) + URANGE(30, dice (level, 3), 90);

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam1 -= dam1/4;
		dam2 -= dam2/4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam1 -= dam1/4;
			dam2 -= dam2/4;
		}
	}

	if ( !EXT_IS_SET( ch->in_room->room_flags, ROOM_INDOORS ) )
	{
	act( "Nagle zaczynasz unosiæ siê, nie, raczej spadaæ w kierunku sufitu! Auu, potê¿nie uderzasz g³ow± w powieszchnie sufitu!", ch, NULL, victim, TO_VICT );
	act( "$N z niedowie¿aniem na twarzy zaczyna lecieæ w kierunku sufitu, uderzaj±c g³ow± potê¿nie!", ch, NULL, victim, TO_NOTVICT );
	spell_damage( ch, victim, dam2, sn, DAM_BASH, FALSE );
	act( "Po czym sytuacja siê odwraca, si³y grawitacji zaczynaj± dzia³aæ i spadasz na dó³ t³uk±æ siê bole¶nie!", ch, NULL, victim, TO_VICT );
	act( "Po czym zaczyna spadaæ w kierunku ziemi, uderzaj±c w ni± i t³uk±æ siê powa¿nie.", ch, NULL, victim, TO_NOTVICT );
	spell_damage( ch, victim, dam1, sn, DAM_BASH, FALSE );
	return;
	}
   else
	act( "Nagle zaczynasz unosiæ siê, wzlatujesz wbrew swej woli parê metrów do góry po czym opadasz w dó³ z wielk± prêdko¶ci±, t³uk±æ siê bole¶nie!", ch, NULL, victim, TO_VICT );
	act( "$N zaczyna nagle unosiæ siê, wzlatuj±c wbrew swej woli do góry po czym spada na ziemie, t³uk±æ siê bole¶nie!", ch, NULL, victim, TO_NOTVICT );
	spell_damage( ch, victim, dam1, sn, DAM_BASH, FALSE );
	return;
}

void spell_gas_breath( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, poi = 0;

	act( "$n wydmuchuje chmurê truj±cego dymu!", ch, NULL, NULL, TO_ROOM );
	act( "Wydmuchujesz chmurê truj±cego dymu.", ch, NULL, NULL, TO_CHAR );

	dam = number_range( 95, 160 ) + dice( level, 10 );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
		}
	}

	if ( is_affected( victim, gsn_mirror_image ) )
	{
		affect_strip( victim, gsn_mirror_image );
		send_to_char( "Chmura dymu ogarnia wszystkie otaczaj±ce ciê lustrzane odbicia niszcz±c je.\n\r", victim );
		act( "Chmura dymu ogarnia wszystkie lustrzane odbicia otaczaj±ce $c niszcz±c je.", victim, NULL, NULL, TO_ROOM );
	}
	poi = victim->hit - dam > -11 ? 1 : 0;
	if ( poi ) poison_to_char( victim, 1 + level / 10 );
	spell_damage( ch, victim, dam, sn, DAM_POISON, TRUE );
}

void spell_lightning_breath( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, hp_dam, dice_dam, hpch, eff_lev;

	act( "$n zieje na $C sto¿kiem b³yskawic.", ch, NULL, victim, TO_NOTVICT );
	act( "$n zieje sto¿kiem b³yskawic wprost na ciebie!", ch, NULL, victim, TO_VICT );
	act( "Ziejesz sto¿kiem b³yskawic na $C.", ch, NULL, victim, TO_CHAR );

	hpch = UMAX( 10, ch->hit );
	hp_dam = number_range( hpch / 9 + 1, hpch / 5 );
	dice_dam = dice( level, 20 );

	dam = UMAX( hp_dam + dice_dam / 10, dice_dam + hp_dam / 10 );
	eff_lev = level;

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		eff_lev -= eff_lev / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
			eff_lev -= eff_lev / 4;
		}
	}

	if ( is_affected( victim, gsn_mirror_image ) )
	{
		affect_strip( victim, gsn_mirror_image );
		send_to_char( "Chmura wy³adowañ elektrycznych ogarnia wszystkie otaczaj±ce ciê lustrzane odbicia niszcz±c je.\n\r", victim );
		act( "Chmura wy³adowañ elektrycznych ogarnia wszystkie lustrzane odbicia otaczaj±ce $c niszcz±c je.", victim, NULL, NULL, TO_ROOM );
	}

	spell_damage( ch, victim, dam, sn, DAM_LIGHTNING, TRUE );
	shock_effect( victim, eff_lev, dam, TARGET_CHAR );
}

/* przerobiony na modle AD&D
 * daje ac 6 na 10 tickow badz jesli ktos przypieprzy mocniej niz
 * 12+level wtedy znika
 */
void spell_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dur = 12, mod = 10;
	int luck, vluck;

	// is affected
    if ( IS_AFFECTED( victim, AFF_ARMOR ) )
    {
        if ( ch == victim )
        {
            print_char( ch, "Ju¿ jeste¶ chronion%s przez magiczny pancerz.\n\r", ch->sex == SEX_FEMALE ? "a" : ch->sex == SEX_NEUTRAL ? "e" : "y" );
        }
        else
        {
            switch ( ch->sex )
            {
                case 0:
                    act( "$N jest ju¿ chronione przez magiczny pancerz.", ch, NULL, victim, TO_CHAR );
                    break;
                case 1:
                    act( "$N jest ju¿ chroniony przez magiczny pancerz.", ch, NULL, victim, TO_CHAR );
                    break;
                default:
                    act( "$N jest ju¿ chroniona przez magiczny pancerz.", ch, NULL, victim, TO_CHAR );
                    break;
            }
        }
        return;
    }

	// luck
	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	vluck = get_curr_stat_deprecated( victim, STAT_LUC );
	if ( number_range( 0, LUCK_BASE_MOD + luck ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ magicznego pancerza.\n\r", ch );
		return;
	}
	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur *= 2;
			mod = 15;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur + level / 5;
	af.rt_duration = 0;
	af.modifier = mod + level;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_ARMOR;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Tworzy siê wokó³ ciebie magiczny pancerz.\n\r", victim );
	act( "Wokó³ $z tworzy siê magiczny pancerz.", victim, NULL, NULL, TO_ROOM );

	return;
}

void spell_blur( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration = 3 + level / 9;
	int mod = -15 - ( UMIN( level, 20 ) );
	AFFECT_DATA af;

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		if ( victim == ch )
			send_to_char( "Naprawdê chcesz siê tak doszczêtnie rozmyæ?\n\r", ch );
		else
			act( "Postaæ $Z wygl±da na wystarczaj±co rozmyt±.", ch, NULL, victim, TO_CHAR );
		return;
	}

	/* Bonus dla specjalisty */

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration *= 2;
            mod = (mod * 3 ) / 2;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_AC;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Postaæ $z rozmywa siê.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Twoja postaæ rozmywa siê.\n\r", victim );
	return;
}

void spell_detect_magic( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dur = 12 + level / 4;

	if ( IS_AFFECTED( ch, AFF_DETECT_MAGIC ) )
	{
		send_to_char( "Przecie¿ ju¿ dostrzegasz magiczn± aurê.\n\r", ch );
		return;
	}

	/* Bonus dla specjalisty */

	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur *= 2;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur;
	af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_DETECT_MAGIC;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Twoje oczy wibruja.\n\r", victim );
	return;
}

void spell_infravision( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int luck, duration, af_level = level / 2;

	if ( IS_AFFECTED( victim, AFF_INFRARED ) )
	{
		if ( victim == ch )
			send_to_char( "Przecie¿ ju¿ widzisz w ciemno¶ci.\n\r", ch );
		else
			act( "$N posiada ju¿ dar infrawizji.\n\r", ch, NULL, victim, TO_CHAR );
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );

	/* zale¿no¶æ od szczê¶cia */
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
		return;
	}

	duration = 12 + level / 5;

	if ( number_range( 0, luck ) > 15 ) duration *= 2;
	if ( number_range( 0, luck ) < 5 ) duration /= 2;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			duration += 4;
			af_level = level;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = af_level;
	af.duration = UMAX( 1, duration );
    af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_INFRARED;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Oczy $z zaczynaja lekko ¶wieciæ czerwonym blaskiem.\n\r", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Twoje oczy zaczynaja lekko ¶wieciæ czerwonym blaskiem.\n\r", victim );
	if ( ch != victim )
    {
		send_to_char( "Uda³o ci siê rzuciæ zaklêcie.\n\r", ch );
    }
	return;
}

/**
 * czar ten daje specowi niewielkiego ressita na ogien i kwas, ustawia ac na 30,
 * * a takze wchlania obrazenia ze zwyklych ciosow, tylko ze cios musi byc
 * silniejszy niz +number_range(najlepsze muska,najlepsze lekko rani),
 * i w sumie nie wiecej niz modifier
 */
void spell_steel_scarfskin( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    // czar jest poprawiany w branchu - gurthg - 2010-06-07
    return;
}

typedef int ROOM_WALK_FUN ( int distance, ROOM_INDEX_DATA * room );

void walk_rooms_bfs( ROOM_INDEX_DATA * start_room, ROOM_WALK_FUN for_each_room)
{
  int i;

  const int MAX_VISITED = 2000;
  ROOM_INDEX_DATA ** tovisit = calloc(sizeof(ROOM_INDEX_DATA*),MAX_VISITED+MAX_EXIT_COUNT);
  int * distance = calloc(sizeof(int),MAX_VISITED+MAX_EXIT_COUNT);
  tovisit[0] = start_room;
  distance[0] = 0;

  int next_free = 1;

  for(i = 0; i < MAX_VISITED; i++)
    {
      ROOM_INDEX_DATA * current = tovisit[i];
      int dist = distance[i];
      if (!current)
        break;
      if (FALSE == for_each_room(dist,current)) // wywolanie funkcji uruchamianej dla ka¿dego pomieszczenia. je¿eli zwróci nam FALSE to nie schodzimy g³êbiej.
        continue; // nie przetwarzamy g³êbiej, tj. nie dodajemy do kolejki roomów po³±czonych z rooma 'current'

      int j;
      for(j = 0; j < MAX_EXIT_COUNT; j++)
        {
          // bierzemy kolejne wej¶cie
          EXIT_DATA * exit = current->exit[j];

          if (!exit)
            continue;

          // sprawdzamy czy ju¿ nie byli¶my w danym roomie
          int k;
          ROOM_INDEX_DATA * next_room = exit->u1.to_room;
          for(k = 0; k < i && next_room; k++)
            {
              if (tovisit[k] == next_room)
                {
                  next_room = NULL;
                }
            }

          // nie byli¶my i przej¶cie nie prowadzi do nik±d
          if (next_room)
            {
              tovisit[next_free] = next_room;
              distance[next_free] = dist+1;
              next_free++;
            }
        }
    }

  free(distance);
  free(tovisit);
}


/**
 * Z forum: http://forum.mud.pl/viewtopic.php?p=210766
 *
 * Pokazuje okolicznych graczy, omijaj±c invisy i inne ukrycia.
 *
 * Obrona przed czarem: nondetection
 */
void spell_sense_presence( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  int dist_level_0 = 4;  // blisko
  int dist_level_1 = 7;  // niedaleko
  int dist_level_2 = 12; // daleko
  int dist_level_3 = 15; // niby-maksymalny-zasiêg
  int dist_level_4 = UMAX(15,level/2); // maks. zasiêg z bonusem od levela

  const int MAX_CHARS_FOUND = 1000;
  CHAR_DATA ** ch_level_0 = calloc(sizeof(CHAR_DATA*), MAX_CHARS_FOUND); int ch_found_level_0 = 0; // blisko
  CHAR_DATA ** ch_level_1 = calloc(sizeof(CHAR_DATA*), MAX_CHARS_FOUND); int ch_found_level_1 = 0; // w pobli¿u
  CHAR_DATA ** ch_level_2 = calloc(sizeof(CHAR_DATA*), MAX_CHARS_FOUND); int ch_found_level_2 = 0; // w pewnej odleg³o¶ci
  CHAR_DATA ** ch_level_3 = calloc(sizeof(CHAR_DATA*), MAX_CHARS_FOUND); int ch_found_level_3 = 0; // daleko
  CHAR_DATA ** ch_level_4 = calloc(sizeof(CHAR_DATA*), MAX_CHARS_FOUND); int ch_found_level_4 = 0; // bardzo daleko

  int each_room( int distance, ROOM_INDEX_DATA * room )
  {
    CHAR_DATA * gch = NULL;

    CHAR_DATA *** ch_level; // wska¼nik na któr±¶ z tablic
    int * ch_found;

    if (distance < dist_level_0)
      {
        ch_level = &ch_level_0;
        ch_found = &ch_found_level_0;
      }
    else if (distance < dist_level_1)
      {
        ch_level = &ch_level_1;
        ch_found = &ch_found_level_1;
      }
    else if (distance < dist_level_2)
      {
        ch_level = &ch_level_2;
        ch_found = &ch_found_level_2;
      }
    else if (distance < dist_level_3)
      {
        ch_level = &ch_level_3;
        ch_found = &ch_found_level_3;
      }
    else if (distance < dist_level_4)
      {
        ch_level = &ch_level_4;
        ch_found = &ch_found_level_4;
      }
    else
      {
         return FALSE; // po przekroczeniu odleg³o¶ci maksymalnej przerywamy
      }

    // sanity check
    if (*ch_found >= MAX_CHARS_FOUND)
      {
        return FALSE;
      }

    for ( gch = room->people; gch != NULL; gch = gch->next_in_room )
      {
        if (!IS_NPC(gch) && (gch != ch) && (!IS_AFFECTED(gch,AFF_NONDETECTION)))
          {
            (*ch_level)[*ch_found] = gch;
            (*ch_found)++;
            if (*ch_found >= MAX_CHARS_FOUND)
              {
                return FALSE;
              }
          }
      }
    return TRUE;
  }

  walk_rooms_bfs( ch->in_room, &each_room );

  {
    BUFFER *buffer = new_buf();
    int found_any = ch_found_level_0 +
                    ch_found_level_1 +
                    ch_found_level_2 +
                    ch_found_level_3 +
                    ch_found_level_4;

    void report_one_level( CHAR_DATA ** ch_level, int ch_found, char * distance_string )
    {
      add_buf( buffer, "{G" );
      add_buf( buffer, distance_string );
      add_buf( buffer, "{x: " );
      if (!ch_found)
        {
          add_buf( buffer, "{Rnikt{x\n\r");
          return;
        }

      int i;
      for(i = 0; i < ch_found; i++)
        {
          if (i)
            {
              add_buf(buffer, ", ");
            }
          add_buf( buffer, capitalize( ch_level[i]->name ) );
        }
      add_buf(buffer,"\n\r");
    }

    if (found_any)
      {
        add_buf( buffer, "Wyczuwasz obecno¶æ nastêpuj±cych osób:\n\r");
        add_buf( buffer, "--------------------------------------\n\r");

        report_one_level( ch_level_0, ch_found_level_0, "Blisko");
        report_one_level( ch_level_1, ch_found_level_1, "W pobli¿u");
        report_one_level( ch_level_2, ch_found_level_2, "W pewnej odleg³o¶ci");
        report_one_level( ch_level_3, ch_found_level_3, "Daleko");
        report_one_level( ch_level_4, ch_found_level_4, "Bardzo daleko");
        page_to_char( buf_string( buffer ), ch );
      }
    else
      {
        send_to_char("Oprócz ciebie nikogo nie ma w pobli¿u.\n\r", ch);
      }
    free_buf( buffer );
  }


  free(ch_level_0);
  free(ch_level_1);
  free(ch_level_2);
  free(ch_level_3);
  free(ch_level_4);

}

void spell_light_nova( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  OBJ_DATA * obj;
  int luck = get_curr_stat_deprecated( ch, STAT_LUC ),
      wis = get_curr_stat_deprecated( ch, STAT_WIS );
  int counter;

  if ( get_eq_char( ch, WEAR_FLOAT ) )
    {
      send_to_char( "Nie masz gdzie umie¶ciæ magicznej kuli.\n\r", ch );
      return;
    }

  if ( !IS_NPC( ch ) && number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
    {
      send_to_char( "Mylisz siê w zaklêciu i magiczna energia marnuje siê bezpowrotnie.\n\r", ch );
      return;
    }

  counter = 4;
  if ( number_range( 0, luck ) > 15 ) counter += 1;
  if ( number_range( 0, luck ) < 5 ) counter -= 2;

  obj = create_object( get_obj_index( OBJ_VNUM_LIGHT_NOVA_BALL ), FALSE );
  if (!obj)
    {
      send_to_char( "Czujesz przep³ywaj±c± przez ciebie moc, ale nic siê nie dzieje.\n\r", ch );
      return;
    }

  obj->timer = 30;
  EXT_SET_BIT( obj->extra_flags, ITEM_NOREMOVE );
  EXT_SET_BIT( obj->extra_flags, ITEM_ROT_DEATH );
  EXT_SET_BIT( obj->extra_flags, ITEM_UNDESTRUCTABLE );
  EXT_SET_BIT( obj->extra_flags, ITEM_NO_RENT );

  obj_to_char( obj, ch );
  equip_char( ch, obj, WEAR_FLOAT, TRUE );

  EXT_SET_BIT( obj->extra_flags, ITEM_TRANSMUTE ); //zastepcze

  act( "Ponad $N zaczyna unosiæ siê dr¿±ca lekko i po³yskuj±ca kula magicznego ¶wiat³a.", ch, obj, NULL, TO_ROOM );
  act( "Nad tob± rozb³yska kula magicznego ¶wiat³a dr¿±c lekko.", ch, obj, NULL, TO_CHAR );

  LIGHT_NOVA_DATA * data = malloc(sizeof(LIGHT_NOVA_DATA));
  data->specialist = FALSE;
  data->sn = sn;
  data->level = level;
  data->spell_level_bonus = level >= LEVEL_HERO;
  data->caster = ch;
  data->ball = obj;
  data->cast_room = ch->in_room;
  data->power = 1;
  data->countdown = counter;
  data->component = FALSE; // na razie bez komponentu

  if ( !IS_NPC( ch ) )
    {
      if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
        {
          data->specialist = TRUE;
        }
    }

  create_event( EVENT_LIGHT_NOVA, PULSE_VIOLENCE, data, NULL, 0 );
  return;
}

void spell_reflect_lightning( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
}

void spell_mend_golem( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
}

