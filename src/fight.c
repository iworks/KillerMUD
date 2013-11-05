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
 * Brosig Micha³         (brohacz@gmail.com             ) [Brohacz   ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Skrzetnicki Krzysztof (gtener@gmail.com              ) [Tener     ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: fight.c 12183 2013-03-25 11:58:55Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/12.02/src/fight.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "monk_kit.h"
#include "tables.h"
#include "progs.h"
#include "clans.h"
#include "magic.h"
#include "shapeshifting.h"
#include "projects.h"
#include "mount.h"
#include "money.h"
#include "paladin_auras.h"

//#define INFO
/*
 * Local functions.
 */

void		op_fight_trigger args( (CHAR_DATA *ch));
void		check_assist	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void		check_killer	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool		check_parry	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, OBJ_DATA *parowana ) );
void		dam_message	 args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, bool immune ) );
void		death_cry	args( ( CHAR_DATA *ch ) );
void		group_gain	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool		mob_can_attack	args( ( CHAR_DATA * ch ) );
bool		is_safe		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void		make_corpse	args( ( CHAR_DATA *ch ) );
void		one_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool second ) );
void		mob_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void		raw_kill	args( ( CHAR_DATA *victim , CHAR_DATA *ch ) );
void		set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void		disarm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
OBJ_DATA*	randomize_item	args( (int points) );
OBJ_DATA* generate_random_spellbook args ( ( CHAR_DATA *ch, int counter ) );
sh_int		get_caster	args( (CHAR_DATA *ch) );
DECLARE_DO_FUN(	do_dismount	);
void		extract_dead_player	args( ( CHAR_DATA *ch ) );
void		update_obj_cond		args( (CHAR_DATA *ch, OBJ_DATA *weapon, int base_dam, int dam, CHAR_DATA *victim));
SPELL_MSG*	msg_lookup		args( (sh_int sn) );
int		 	weapon_skill_mod	args( (CHAR_DATA *ch, bool primary, bool hitr) );
bool		check_stone_skin	args( (CHAR_DATA *ch, CHAR_DATA *victim, int dam_type, int *dam, OBJ_DATA *weapon) );
bool		check_mirror_image	args( (CHAR_DATA *victim, CHAR_DATA *ch) );
bool		check_blink	args( (CHAR_DATA *victim, CHAR_DATA *ch, bool unik) );
bool		still_fighting		args( (CHAR_DATA *ch) );
int			compute_tohit		args( (CHAR_DATA *ch, bool primary) );
void		raw_damage		args( (CHAR_DATA *ch,CHAR_DATA *victim,int dam ) );
int		 	check_magic_attack	args( (CHAR_DATA *ch, CHAR_DATA *vch, OBJ_DATA *weapon) );
bool		mp_onehit_trigger	args( ( CHAR_DATA *mob, CHAR_DATA *victim ) );
bool		op_onehit_trigger	args( (OBJ_DATA *obj, CHAR_DATA *ch) );
void		delevel			args( ( CHAR_DATA *ch, bool hide ) );
bool		mp_hit_trigger	args( ( CHAR_DATA *mob, CHAR_DATA *victim ) );
bool		op_hit_trigger	args( (OBJ_DATA *obj, CHAR_DATA *ch) );

void		check_player_death	args( ( CHAR_DATA *ch, CHAR_DATA *killer) );
void		generate_onehit_msg	args( (CHAR_DATA *ch, CHAR_DATA *victim, int dam,int dt,bool immune ) );
void		reorganize_mobile_group args( (CHAR_DATA *mob) );
void		onehit_kill_check	args( (CHAR_DATA *mob, CHAR_DATA *ch) );
bool		check_increase_wounds	args( (CHAR_DATA *victim, int dam_type, int *dam) );
void		check_armor_spell	args( (CHAR_DATA *victim, int dam) );
int		 	parry_mod		args( (OBJ_DATA *weapon) );
bool		special_damage_message	args( (CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *wield, int dam, int dt, bool immune) );
void		check_berserk_rescue	args( (CHAR_DATA * ch) );
bool		check_dodge		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int ac, int th_roll, int thac0, bool critic ) );
bool		check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim, int ac, int th_roll, int thac0, bool critic, int * dam );
void		update_death_statistic	args( (CHAR_DATA *victim, CHAR_DATA *ch) );
void		check_special_unarmed_hit	args( (CHAR_DATA *ch, CHAR_DATA *victim, int base_dam) );
void		one_hit_monk			args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void		strip_fight_affects		args( ( CHAR_DATA * ch ) );
void		check_defensive_spells		args( (CHAR_DATA * ch, CHAR_DATA * victim ) );
void		remove_mirror		args( (CHAR_DATA * ch, CHAR_DATA * victim, int how_many ) );
void		all_artefact_from_char  args( (CHAR_DATA *ch)  );
bool 		form_check( CHAR_DATA *ch, CHAR_DATA *vch, OBJ_DATA* obj, sh_int sn );
int 		get_backstabbed_dam args ( ( CHAR_DATA * ch, CHAR_DATA * victim, int old_dam ) );
bool		check_illusion		args( (CHAR_DATA *ch ) );
int			check_critical_hit args( ( CHAR_DATA *ch, CHAR_DATA *victim, int *diceroll, int *dt, OBJ_DATA *wield,int *base_dam, int old_dam, int *twohander_fight_style, int *weapon_mastery, bool *critical ) );
int         one_hit_check_mastery args ( ( CHAR_DATA *ch, CHAR_DATA *victim, int gsn, int check_improve_level ) );
bool check_noremovable_affects args ( ( AFFECT_DATA *aff ) );

int calculate_final_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam);

/* GADANIE MOBOW */
void mob_chat_fight args ( ( CHAR_DATA *ch ) );

/* part droper */
void part_dropper_wraper args ( ( CHAR_DATA *ch ) ) ;

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */

bool strip_illusion( CHAR_DATA *ch )
{
	OBJ_DATA *obj, *obj_next;

	if ( !IS_NPC(ch) ) return FALSE;

	act( "$n rozp³ywa siê w powietrzu.", ch, NULL, NULL, TO_ROOM );

	/* itemy co je trzymala iluzja opadaja na ziemie*/
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
		OBJ_NEXT_CONTENT( obj, obj_next );

		if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
        {
			continue;
        }

		if ( obj->wear_loc != WEAR_NONE )
        {
			unequip_char( ch, obj );
        }

		obj_from_char( obj );
		act( "$p upada na ziemiê.", ch, obj, NULL, TO_ROOM );
		obj_to_room( obj, ch->in_room );

		if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
		{
			switch ( number_range( 1, 3 ) )
			{
				case 1:
					act( "$p rozp³ywa siê w powietrzu.", ch, obj, NULL, TO_ROOM );
					break;
				case 2:
					act( "$p znika bez ¶ladu.", ch, obj, NULL, TO_ROOM );
					break;
				default:
					act( "$p rozp³ywa siê pozostawiaj±c tylko chmurê dymu.", ch, obj, NULL, TO_ROOM );
					break;
			}
		}
		else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
		{
			switch ( number_range( 1, 3 ) )
			{
				case 1:
					act( "$p toczy siê przez chwilê, po czym rozpryskujê siê na kwa³eczki.", ch, obj, NULL, TO_ROOM );
					break;
				case 2:
					act( "$p pêka z hukiem i rozpryskuje siê dooko³a.", ch, obj, NULL, TO_ROOM );
					break;
				default:
					act( "$p rozpryskuje siê na kawa³eczki.", ch, obj, NULL, TO_ROOM );
					break;
			}
		}
	}

    long int copper = money_count_copper_all( ch );
	if ( copper > 0 )
	{
		obj_to_room( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), ch->in_room );
		if ( copper > 1 ) act( "Kupka monet upada na ziemiê.", ch, NULL, NULL, TO_ROOM );
		else act( "Jedna moneta upada na ziemiê.", ch, NULL, NULL, TO_ROOM );
        money_reset_character_money( ch );
	}

	return TRUE;
}

void violence_update( void )
{
	CHAR_DATA * ch;
	CHAR_DATA *ch_next;
	CHAR_DATA *victim;
	AFFECT_DATA *paf, *af, *af_do_counta, *dazzling;
	AFFECT_DATA *paf_next;
	OBJ_DATA *obj, *weapon, *obj_next;
	CHAR_DATA *tch;
	bool found_impale = FALSE;
	int af_count = 0;

	/* petla dla normali */
	for ( ch = char_list; ch != NULL; ch = ch_next )
	{
		ch_next	= ch->next;

		/* nie walczymy z supermobem ! */
		DEBUG_INFO( "viol_update:supermob_stuff" );
		if ( IS_NPC( ch ) && ch->fighting && ch->pIndexData->vnum == 3 )
		{
			stop_fighting( ch, TRUE );
			ch->hunting = NULL;
			ch->hit = get_max_hp(ch);
			continue;
		}
		
		//stripowanie sneak_invis, moby zorientowaly sie ze gracz stoi w lokacji
        EXT_REMOVE_BIT( ch->affected_by, AFF_SNEAK_INVIS ); 

		/* moby z summonow znikaja, jak nie maja pana, wylaczajac sytuacje - summowali a nie mogli kontrolowac
		   (act aggressive) albo jak stracili pana po czarku repayment (czy huntuje pc)*/
		DEBUG_INFO( "viol_update:mob_noexp_extract" );
   	//Drake: By³ tu jeszcze && !IS_AFFECTED(ch,AFF_CHARM), ale po wyj¶ciu z gry w karczmie charm na mobie nie znika, wiêc i sam mob nie znika.
		if ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_NO_EXP ) && !EXT_IS_SET( ch->act, ACT_AGGRESSIVE )  && !ch->master && ch->pIndexData->vnum != MOB_VNUM_FETCH )
		{
			if( ( ch->hunting && !IS_NPC( ch->hunting ) ) || ( ch->fighting && !IS_NPC( ch->fighting ) ) )
				;
			else
			{
				stop_fighting( ch, TRUE );

				if( IS_NPC(ch) && ( ch->pIndexData->vnum == MOB_VNUM_GOLEM ||
							ch->pIndexData->vnum == MOB_VNUM_LIVEOAK ||
							ch->pIndexData->vnum == MOB_VNUM_BARKGUARD ||
							ch->pIndexData->vnum == MOB_VNUM_WILDTHORN ||
							ch->pIndexData->vnum == MOB_VNUM_BEAR ||
							ch->pIndexData->vnum == MOB_VNUM_WOLF ||

							EXT_IS_SET( ch->act, ACT_RAISED ) ||
							( ch->pIndexData->vnum > 10 && ch->pIndexData->vnum < 20 ) || //ghule, ghasty
							( ch->pIndexData->vnum > 3000 && ch->pIndexData->vnum < 3064 ) ||
							( ch->pIndexData->vnum > 10904 && ch->pIndexData->vnum < 10910 ) ) )//rozne moby z summonow
				{
					for ( obj = ch->carrying; obj != NULL; obj = obj_next )
					{
						OBJ_NEXT_CONTENT( obj, obj_next );

						if ( IS_OBJ_STAT( obj, ITEM_NODROP ) || IS_OBJ_STAT( obj, ITEM_ROT_DEATH ))
							continue;

						if ( obj->wear_loc != WEAR_NONE )
							unequip_char( ch, obj );

						obj_from_char( obj );

						act( "$p upada na ziemie.", ch, obj, NULL, TO_ROOM );
						obj_to_room( obj, ch->in_room );

						if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
						{
							switch ( number_range( 1, 3 ) )
							{
								case 1:
									act( "$p rozp³ywa siê w powietrzu.", ch, obj, NULL, TO_ROOM );
									break;
								case 2:
									act( "$p znika bez ¶ladu.", ch, obj, NULL, TO_ROOM );
									break;
								default:
									act( "$p rozp³ywa siê pozostawiaj±c tylko chmurê dymu.", ch, obj, NULL, TO_ROOM );
									break;
							}
							/*artefact*/
							if ( is_artefact( obj ) ) extract_artefact( obj );
							if ( obj->contains ) extract_artefact_container( obj );
							extract_obj( obj );
						}
						else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
						{
							switch ( number_range( 1, 3 ) )
							{
								case 1:
									act( "$p toczy siê przez chwilê, po czym rozpryskujê siê na kwa³eczki.", ch, obj, NULL, TO_ROOM );
									break;
								case 2:
									act( "$p pêka z hukiem i rozpryskuje siê dooko³a.", ch, obj, NULL, TO_ROOM );
									break;
								default:
									act( "$p rozpryskuje siê na kawa³eczki.", ch, obj, NULL, TO_ROOM );
									break;
							}
							/*artefact*/
							if ( is_artefact( obj ) ) extract_artefact( obj );
							if ( obj->contains ) extract_artefact_container( obj );
							extract_obj( obj );
						}
					}

                    long int copper = money_count_copper_all( ch );
					if ( copper > 0 )
					{
						obj_to_room( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), ch->in_room );
                        append_file_format_daily
                            (
                             ch,
                             MONEY_LOG_FILE,
                             "-> S: %d %d (%d), B: %d %d (%d) - quit yes",
                             copper, 0, -copper,
                             ch->bank,
                             ch->bank,
                             0
                            );
						if ( copper > 1 ) act( "Kupka monet upada na ziemiê.", ch, NULL, NULL, TO_ROOM );
						else act( "Jedna moneta upada na ziemiê.", ch, NULL, NULL, TO_ROOM );
                        money_reset_character_money( ch );
					}
				}

				if ( ch->in_room )
				{
					if( EXT_IS_SET( ch->act, ACT_RAISED ) )
						act( "$n rozsypuje siê w proch.", ch, NULL, NULL, TO_ROOM );
					else
							strip_illusion( ch );
				}
				extract_char( ch, TRUE );
			}
		}

		DEBUG_INFO( "viol_update:mirrorfall_strip" );
		if( is_affected(ch,gsn_mirrorfall ) )
			affect_strip(ch, gsn_mirrorfall);

		DEBUG_INFO( "viol_update:impale" );
		if( is_affected(ch,gsn_glorious_impale) )
		{
			if( affect_find(ch->affected,gsn_glorious_impale)->modifier == 0 )
			{
				if( !get_eq_char(ch,WEAR_WIELD) || get_eq_char(ch,WEAR_WIELD)->value[0] != WEAPON_SPEAR )
				{
					affect_strip(ch,gsn_glorious_impale);
				}
			}
			else
			{
				for( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if( tch->fighting == ch && is_affected(tch,gsn_glorious_impale) && get_eq_char(tch,WEAR_WIELD) && get_eq_char(tch,WEAR_WIELD)->value[0] == WEAPON_SPEAR )
						++found_impale;
				}
				if( !found_impale )
					affect_strip(ch,gsn_glorious_impale);
			}
		}

		DEBUG_INFO( "viol_update:overload" );
		/* opadanie na ziemiê, je¿li jest siê przeci±¿onym */
		if ( (!ch->mount && get_carry_weight( ch ) > can_carry_w( ch ) && IS_AFFECTED(ch, AFF_FLYING) && !IS_NPC (ch)) ||
				( !ch->mount && get_carry_weight( ch ) > can_carry_w( ch ) && IS_AFFECTED(ch, AFF_FLOAT) && !IS_NPC (ch)))
		{
			act("$n pod wp³ywem ciê¿aru opada na ziemiê.",ch,NULL,NULL,TO_ROOM);
			send_to_char( "Pod wp³ywem obci±¿enia opadasz na ziemiê.\n\r", ch );
			affect_strip(ch, gsn_fly);
			affect_strip(ch, gsn_float);
		}

		DEBUG_INFO( "viol_update:regen" );
		/* regeneracja co runde walki */
		if ( IS_AFFECTED( ch, AFF_REGENERATION ) && ch->hit < get_max_hp(ch) )
		{
			AFFECT_DATA * paf_regen;
			int min, max, level;
            bool check;

			paf_regen = affect_find( ch->affected, gsn_regenerate );

			if ( paf_regen )
				level = paf_regen->level;
			else
				level = 2;

			switch ( level )
			{
				case 1:
				case 2:
					min = 1;max = 2;break;
				case 3:
					min = 1;max = 3;break;
				case 4:
					min = 2;max = 4;break;
				case 5:
					min = 2;max = 5;break;
				case 6:
					min = 3;max = 6;break;
				case 7:
					min = 3;max = 7;break;
				case 8:
					min = 4;max = 7;break;
				default:
					min = 4;max = 8;break;
			}
			update_pos( ch );
		}

		//sprawdzenie heat/chill metal
		for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
		{
			if ( obj->wear_loc == WEAR_NONE )
				continue;

			if ( affect_find( obj->affected, gsn_heat_metal ) )
			{
				int dam = 0;

				switch ( obj->wear_loc )
				{
					case WEAR_FEET: dam = dice( 1, 4 );	break;
					case WEAR_LEGS: dam = dice( 2, 4 );	break;
					case WEAR_ARMS: dam = dice( 1, 6 );	break;
					case WEAR_HANDS: dam = dice( 1, 4 );	break;
					case WEAR_BODY: dam = dice( 2, 6 );	break;
					case WEAR_HEAD: dam = dice( 2, 4 );	break;
					case WEAR_SHIELD: dam = dice( 1, 4 );	break;
					case WEAR_WIELD: dam = dice( 1, 4 );	break;
					case WEAR_SECOND: dam = dice( 1, 4 );	break;
					default: break;
				}

				switch ( obj->wear_loc )
				{
					case WEAR_FEET:
					case WEAR_LEGS:
					case WEAR_HANDS:
					case WEAR_ARMS:
						act( "$p pal± twoj± skórê!", ch, obj, NULL, TO_CHAR );
						act( "$p pali skórê $z!", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p pali twoj± skórê!", ch, obj, NULL, TO_CHAR );
						act( "$p pali skórê $z!", ch, obj, NULL, TO_ROOM );
						break;
				}

				spell_damage( ch, ch, dam, gsn_heat_metal, DAM_FIRE , FALSE );

				if (!IS_AFFECTED(ch, AFF_PARALYZE))
				{
					if ( !IS_OBJ_STAT(obj, ITEM_NOREMOVE) && IS_NPC( ch ))
					{
						if (obj->item_type == ITEM_ARMOR )
							act("$n jêczy i w wielkim po¶piechu zdejmuje $h!", ch,obj,NULL,TO_ROOM);
						else if (obj->item_type == ITEM_WEAPON || obj->item_type == ITEM_SHIELD )
							act("$p wypada $x z d³oni parz±c j±.", ch,obj,NULL,TO_ROOM);
						unequip_char( ch, obj );
					}
				}
				else if (number_percent() > 50)
				{
					for ( af = ch->affected; af != NULL; af = af->next )
					{
						if ( af->bitvector == &AFF_PARALYZE )
						{
							affect_strip(ch,af->type);
							if (!IS_NPC( ch ) )
								send_to_char( "Z powodu bólu, odzyskujesz kontrolê nad swoim cia³em\n\r", ch );
						}
					}
					act("Z powodu bólu, $n odzyskuje kontrolê nad swoim cia³em.", ch, NULL, NULL, TO_ROOM);
				}

				if ( !ch->in_room )
					return;
			}
			else if ( affect_find( obj->affected, gsn_chill_metal ) )
			{
				int dam = 0;

				switch ( obj->wear_loc )
				{
					case WEAR_FEET: dam = dice( 1, 4 );	break;
					case WEAR_LEGS: dam = dice( 2, 4 );	break;
					case WEAR_ARMS: dam = dice( 1, 6 );	break;
					case WEAR_HANDS: dam = dice( 1, 4 );	break;
					case WEAR_BODY: dam = dice( 2, 6 );	break;
					case WEAR_HEAD: dam = dice( 2, 4 );	break;
					case WEAR_SHIELD: dam = dice( 1, 4 );	break;
					case WEAR_WIELD: dam = dice( 1, 4 );	break;
					case WEAR_SECOND: dam = dice( 1, 4 );	break;
					default: break;
				}

				switch ( obj->wear_loc )
				{
					case WEAR_FEET:
					case WEAR_LEGS:
					case WEAR_HANDS:
					case WEAR_ARMS:
						act( "$p mro¿± na ko¶æ twoje cia³o!", ch, obj, NULL, TO_CHAR );

						if ( ch->sex == 2 )
							act( "$p mro¿± na ko¶æ jej cia³o!", ch, obj, NULL, TO_ROOM );
						else
							act( "$p mro¿± na ko¶æ jego cia³o!", ch, obj, NULL, TO_ROOM );
						break;

					default:
						act( "$p mrozi na ko¶æ twoje cia³o!", ch, obj, NULL, TO_CHAR );

						if ( ch->sex == 2 )
							act( "$p mrozi na ko¶æ jej cia³o!", ch, obj, NULL, TO_ROOM );
						else
							act( "$p mrozi na ko¶æ jego cia³o!", ch, obj, NULL, TO_ROOM );
						break;
				}


				spell_damage( ch, ch, dam, gsn_heat_metal, DAM_COLD, FALSE );

				if ( !ch->in_room )
					return;
			}
		}

		DEBUG_INFO( "viol_update:underwater_update" );
        /* dziwne rzeczy pod woda*/

        if
            (
             !IS_NPC( ch ) &&
             ch->in_room &&
             IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ) &&
             !IS_AFFECTED( ch, AFF_WATERBREATH ) &&
             !is_undead( ch ) &&
             !IS_IMMORTAL( ch )
            )
        {
			int oxygen;

			ch->pcdata->oxygen++;

			oxygen = UMAX( 0, ch->pcdata->oxygen - 3 - ( get_curr_stat_deprecated( ch, STAT_CON ) / 6 ) );

			//jesli zejdzie,to oxygen = 6, zeby ladny tekst wyskoczyl
			if ( ch->hit + 11 < oxygen * get_max_hp(ch) / 10 )
				oxygen = 6;

			switch ( oxygen )
			{
				case 0:
					break;
				case 1:
					send_to_char( "Czujesz narastaj±cy ból w klatce piersiowej, zaczyna brakowaæ ci powietrza.\n\r", ch );
					switch ( ch->sex )
					{
						case 0:
							act( "$n sinieje na twarzy, chyba brakuje temu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						case 1:
							act( "$n sinieje na twarzy, chyba brakuje mu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						default :
							act( "$n sinieje na twarzy, chyba brakuje jej powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
					}
					break;
				case 2:
				case 3:
					send_to_char( "Czujesz narastaj±cy ból w klatce piersiowej, zaczyna brakowaæ ci powietrza.\n\r", ch );
					switch ( ch->sex )
					{
						case 0:
							act( "$n sinieje na twarzy, chyba brakuje temu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						case 1:
							act( "$n sinieje na twarzy, chyba brakuje mu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						default :
							act( "$n sinieje na twarzy, chyba brakuje jej powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
					}
					damage( ch, ch, oxygen * get_max_hp(ch) / 10, TYPE_UNDEFINED, DAM_NONE, FALSE );
					break;
				case 4:
					send_to_char( "Ból w klatce narasta, musisz jak najszybciej siê wynurzyæ.\n\r", ch );
					switch ( ch->sex )
					{
						case 0:
							act( "$n sinieje na twarzy, chyba brakuje temu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						case 1:
							act( "$n sinieje na twarzy, chyba brakuje mu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						default :
							act( "$n sinieje na twarzy, chyba brakuje jej powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
					}
					damage( ch, ch, oxygen * get_max_hp(ch) / 10, TYPE_UNDEFINED, DAM_NONE, FALSE );
					break;
				case 5:
				case 6:
					send_to_char( "Przed oczami zaczynaj± ci przep³ywaæ kolorowe plamy, ogarnia ciê potworne zmêczenie.\n\r", ch );
					switch ( ch->sex )
					{
						case 0:
							act( "$n sinieje na twarzy, chyba brakuje temu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						case 1:
							act( "$n sinieje na twarzy, chyba brakuje mu powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
						default :
							act( "$n sinieje na twarzy, chyba brakuje jej powietrza.", ch, NULL, NULL, TO_ROOM );
							break;
					}
					damage( ch, ch, oxygen * get_max_hp(ch) / 10, TYPE_UNDEFINED, DAM_NONE, FALSE );
					break;
				default:
					send_to_char( "Przestajesz cokolwiek widzieæ, nie czujesz ju¿ bólu. To juz chyba koniec.\n\r", ch );
					act( "$n zamyka oczy i nieruchomieje. Z $s otwartych ust unosza siê ku górze b±belki powietrza.", ch, NULL, NULL, TO_ROOM );
					damage( ch, ch, get_max_hp(ch) + 12, TYPE_UNDEFINED, DAM_NONE, FALSE );
					break;
			}

			if ( !ch->in_room )
				continue;
		}

		DEBUG_INFO( "viol_update:hallucinations" );
		if ( IS_AFFECTED( ch, AFF_HALLUCINATIONS_POSITIVE ) && number_range(1,20) == 1 )
		{
			if ( ch->fighting != NULL )
			{
				switch( number_range(1,10))
				{
					case 1:
						send_to_char( "Dok³adnie widzisz ka¿dy ruch przeciwnika.\n\r", ch );
						act("$n wyprowadza nadwyraz trafione ciosy.",ch,NULL,NULL,TO_ROOM);
						break;
					case 2:
						act("$N puchnie i rusza siê coraz wolniej.",ch,NULL,ch->fighting,TO_CHAR);
						act("$n bije coraz celniej.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 3:
						act("¦wiat³o wokó³ $Z blednie, widzisz dok³adnie ka¿dy $S kontur.",ch,NULL,ch->fighting,TO_CHAR);
						act("$n nienaturalnie wytrzeszcza oczy.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 4:
						act("$N ciê nienawidzi!",ch,NULL,ch->fighting,TO_CHAR);
						act("$n mamrocze co¶ pod nosem.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 5:
						act("Czujesz narastaj±c± nienawi¶æ do $Z.",ch,NULL,ch->fighting,TO_CHAR);
						act("$n warczy gro¼nie.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 6:
						act("$N to twój najwiêkszy wróg!",ch,NULL,ch->fighting,TO_CHAR);
						act("$n coraz bardziej anga¿uje siê w walkê.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 7:
						act("Z ust $Z wyp³ywa czarna ma¼.",ch,NULL,ch->fighting,TO_CHAR);
						act("$n cofa siê z wyrazem obrzydzenia na twarzy.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 8:
						act("$N ¶mieje siê z ciebie podle.",ch,NULL,ch->fighting,TO_CHAR);
						act("$n wpada w sza³!",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 9:
						act("Orientujesz siê, ¿e $N to dzikie zwierzê!",ch,NULL,ch->fighting,TO_CHAR);
						act("$n be³kocze co¶ g³o¶no.",ch,NULL,ch->fighting,TO_ROOM);
						break;
					case 10:
						act("Po swej prawicy zauwa¿asz siebie walcz±c<&ego/±/e> z $V.",ch,NULL,ch->fighting,TO_CHAR);
						act("$n nerwowo przewraca oczami.",ch,NULL,ch->fighting,TO_ROOM);
						break;
				}
			}
			else
			{
				switch(number_range(1,8))
				{
					case 1:
						if ( !IS_OUTSIDE( ch ) || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER ) )
						{
							send_to_char( "Niebo przykrywaj± czarne burzowe chmury.\n\r", ch );
							act("$n spogl±da ze strachem na niebo.",ch,NULL,NULL,TO_ROOM);
							break;
						}
					case 2:
						send_to_char( "Dooko³a ciebie zaczynaj± pe³zaæ jadowite wê¿e!\n\r", ch );
						act("$n patrzy na ziemiê i wydaje z siebie jêk obrzydzenia.",ch,NULL,NULL,TO_ROOM);
						break;
					case 3:
						send_to_char( "Powoli zaczynasz zapadaæ siê w ziemiê.\n\r", ch );
						act("$n mamrocze co¶ pod nosem tupi±c g³o¶no.",ch,NULL,NULL,TO_ROOM);
						break;
					case 4:
						if (( weapon = get_eq_char( ch, WEAR_WIELD )) != NULL )
						{
							act("$p warczy na ciebie!", ch, weapon, NULL, TO_CHAR );
							act("$n patrzy przera¿onym wzrokiem przed siebie.",ch,NULL,NULL,TO_ROOM);
							break;
						}
					case 5:
						victim = get_random_char( ch, FALSE );
						if ( victim != NULL )
						{
							act("Spogl±dasz na $C i czujesz nienawi¶æ!", ch, NULL, victim, TO_CHAR );
							act("$n patrzy na ciebie z nienawi¶ci±.", ch, NULL, victim, TO_VICT );
							act("$n patrzy z nienawi¶ci± na $C.", ch, NULL, victim, TO_NOTVICT);
							break;
						}
					case 6:
						send_to_char( "Robi siê zimno i nieprzyjemnie.\n\r", ch );
						act("$n trzêsie siê z zimna.",ch,NULL,NULL,TO_ROOM);
						break;
					case 7:
						send_to_char( "Nagle stajesz w p³omieniach! Palisz siê!\n\r", ch );
						act("$n rzuca siê na ziemie i zaczyna siê turlaæ.",ch,NULL,NULL,TO_ROOM);
						break;
					case 8:
						send_to_char( "Zastanawiasz siê nad sensem swojego marnego ¿ycia.\n\r", ch );
						act("$n patrzy przed siebie bezbarwnym wzrokiem.",ch,NULL,NULL,TO_ROOM);
						break;
				}
			}
		}
		else if ( IS_AFFECTED( ch, AFF_HALLUCINATIONS_NEGATIVE ) && number_range(1,20) == 1 )
		{
			if ( ch->fighting != NULL )
			{
				switch(number_range(1,8))
				{
					case 1:
						act("$N znika i pojawia siê w innym miejscu! Ale ¶mieszne!", ch, NULL, ch->fighting, TO_CHAR );
						act("$n nagle wybucha ¶miechem.",ch,NULL,NULL,TO_ROOM);
						break;
					case 2:
						act("$N wyrastaj± dodatkowe rêce!", ch, NULL, ch->fighting, TO_CHAR );
						act("$n kiwa g³ow± na boki ¶miej±c siê g³upio.",ch,NULL,NULL,TO_ROOM);
						break;
					case 3:
						act("$N dwoi siê i troi!", ch, NULL, ch->fighting, TO_CHAR );
						act("$n rozgl±da siê z wyrazem zdenerwowania na twarzy.",ch,NULL,NULL,TO_ROOM);
						break;
					case 4:
						if ( !IS_OUTSIDE( ch ) || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER ) )
						{
							act("$N robi przysiad, wyskok i znika gdzie¶ miêdzy ob³okami. Nagle orientujesz siê, ¿e stoi obok ciebie!", ch, NULL, ch->fighting, TO_CHAR );
							act("$n z wyrazem os³upienia gapi siê na niebo.",ch,NULL,NULL,TO_ROOM);
							break;
						}
					case 5:
						act("Zabawa z $V jest taka przyjemna!", ch, NULL, ch->fighting, TO_CHAR );
						act("$n klaszcze w d³onie szczerz±c zêby.",ch,NULL,NULL,TO_ROOM);
						break;
					case 6:
						act("$N skacze jak wiewiórka!", ch, NULL, ch->fighting, TO_CHAR );
						act("$n kiwa g³ow± w górê i w dó³.",ch,NULL,NULL,TO_ROOM);
						break;
					case 7:
						act("$n robi salto w powietrzu.", ch, NULL, ch->fighting, TO_CHAR );
						act("$n patrzy przed siebie z niedowierzaniem.",ch,NULL,NULL,TO_ROOM);
						break;
					case 8:
						act("$N nagle zapada siê pod ziemiê, aby po chwili wyskoczyæ w innym miejscu.", ch, NULL, ch->fighting, TO_CHAR );
						act("$n rozgl±da siê po ziemi.",ch,NULL,NULL,TO_ROOM);
						break;
				}
			}
			else
			{
				switch(number_range(1,8))
				{
					case 1:
						if ( !IS_OUTSIDE( ch ) || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER ) )
						{
							send_to_char( "Na niebie przelatuje piêkny szkar³atno fioletowy smok!\n\r", ch );
							act("$n spogl±da z g³upawym u¶miechem na niebo.",ch,NULL,NULL,TO_ROOM);
							break;
						}
					case 2:
						send_to_char( "Dooko³a ciebie zaczynaj± biegaæ weso³o ¶liczne czerwone zaj±czki!\n\r", ch );
						act("$n patrzy na ziemiê i ¶mieje siê nie wiadomo z czego.",ch,NULL,NULL,TO_ROOM);
						break;
					case 3:
						send_to_char( "Nagle zaczynasz siê wznosiæ nad okolic±! Ty latasz! Jak piêknie!\n\r", ch );
						act("$n zaczyna machaæ rêkoma na¶laduj±c lot ptaka.",ch,NULL,NULL,TO_ROOM);
						break;
					case 4:
						if (( weapon = get_eq_char( ch, WEAR_WIELD )) != NULL )
						{
							act("$p u¶miecha siê weso³o!", ch, weapon, NULL, TO_CHAR );
							act("$n patrzy bezbarwnym wzrokiem przed siebie usmiechaj±c siê szeroko.",ch,NULL,NULL,TO_ROOM);
							break;
						}
					case 5:
						victim = get_random_char( ch, FALSE );
						if ( victim != NULL )
						{
							switch (victim->sex)
							{
								case 0:
									act("Spogl±dasz na $C i widzisz jakie jest kochane!", ch, NULL, victim, TO_CHAR );
									break;
								case 1:
									act("Spogl±dasz na $C i widzisz jaki jest kochany!", ch, NULL, victim, TO_CHAR );
									break;
								default:
									act("Spogl±dasz na $C i widzisz jaka jest kochana!", ch, NULL, victim, TO_CHAR );
									break;
							}

							act("$n u¶miecha siê dziwnie patrz±c na ciebie.", ch, NULL, victim, TO_VICT );
							act("$n u¶miecha siê dziwnie patrz±c na $C.", ch, NULL, victim, TO_NOTVICT);
							break;
						}
					case 6:
						send_to_char( "Jest ci ciep³o i przyjemnie!\n\r", ch );
						act("$n u¶miecha siê b³ogo.",ch,NULL,NULL,TO_ROOM);
						break;
					case 7:
						send_to_char( "Dooko³a ciebie wszêdzie le¿± srebrne monety! Bêdziesz bogat<&y/a/e>!\n\r", ch );
						act("$n rzuca siê na ziemie i zaczyna czego¶ szukaæ ¶miej±c siê g³o¶no.",ch,NULL,NULL,TO_ROOM);
						break;
					case 8:
						send_to_char( "Jeste¶ ma³ym, czerwonym kwiatuszkiem na ¶rodku zielonej ³±ki.\n\r", ch );
						act("$n patrzy przed siebie bezbarwnym wzrokiem.",ch,NULL,NULL,TO_ROOM);
						break;
				}
			}
		}

		DEBUG_INFO( "viol_update:demon_aura" );
		if( is_affected( ch, gsn_demon_aura ) )
		{
			for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
			{
                if (tch->position > POS_SLEEPING)
                {
                    if( tch == ch)
                    {
                        if ( number_range(1,50) == 1 )
                        {
                            switch( number_range(1,6))
                            {
                                case 1:
                                    send_to_char( "Czujesz jak otaczaj±ca ciê aura oblepia w przyjemny, ciep³y sposób ca³e twoje cia³o, masz wra¿enie, jakby¶ by<&³/³a/³o> zanurzon<&y/a/e> w zbiorniku wype³nionym ¶wie¿± krwi±.\n\r", tch );
                                    break;
                                case 2:
                                    send_to_char( "Otaczaj±ca ciê aura s³abnie na chwilê, tylko po to aby otoczyæ ciê ze zdwojon± moc±.\n\r",tch);
                                    break;
                                case 3:
                                    send_to_char( "Wydaje ci siê, ¿e dziêki otaczaj±cej ciê aurze widzisz wiêcej i lepiej ni¿ normalnie. Tak¿e pozosta³e zmys³y wyostrzy³y siê nieznacznie.\n\r", tch );
                                    break;
                                case 4:
                                    send_to_char( "Krew przep³ywaj±ca przez twoje cia³o za spraw± aury zaczyna p³yn±æ szybciej. Twoje ¿y³y pulsuj±, a ca³e cia³o zaczyna wrêcz emanowaæ ciep³em.\n\r", tch );
                                    break;
                                case 5:
                                    send_to_char( "Przez chwile twoje oczy zalewa krew... nie, to tylko z³udzenie. Jednak ogl±danie ¶wiata przez krwaw± zas³onê by³o mi³ym wydarzeniem.\n\r", tch );
                                    break;
                                case 6:
                                    send_to_char( "Czujesz jak twoje miê¶nie zaczynaj± same siê napinaæ, pulsowaæ delikatnie i widaæ naprê¿one na nich ¿y³y.\n\r", tch );
                                    break;
                            }
                        }
                    }
                    else if( is_same_group(ch,tch) && number_range(1,40) == 1 )
                    {
                        if ( IS_EVIL(tch))
                        {
                            switch(number_range(1,5))
                            {
                                case 1:
                                    if(ch->sex == 2)
                                        act( "Widzisz jak $c otacza piêkna krwistoczerwona, demoniczna aura, która czyni j± majestatyczn± i gro¼n± postaci±.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Widzisz jak $c otacza piêkna krwistoczerwona, demoniczna aura, która czyni go majestatyczn± i gro¼n± postaci±.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 2:
                                    act( "Z podziwem przygl±dasz siê przez chwilê $x, a w szczególno¶ci $s fascynuj±cej, pulsuj±cej aurze z³a.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 3:
                                    if(ch->sex == 2)
                                        act( "Widz±c mroczn± aurê otaczaj±c± $c jeste¶ pew<&ien/na/ne>, ¿e posz<&ed³by¶/³aby¶/³oby¶> za ni± na koniec ¶wiata!", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Widz±c mroczn± aurê otaczaj±c± $c jeste¶ pew<&ien/na/ne>, ¿e posz<&ed³by¶/³aby¶/³oby¶> za nim na koniec ¶wiata!", ch, NULL, tch, TO_VICT );
                                    break;
                                case 4:
                                    if(ch->sex == 2)
                                        act( "Wydaje ci siê, ¿e $n wygl±da jak prawdziwa demonica, za spraw± otaczaj±cej j± mrocznej aury, rozpalaj±cej w twoim sercu z³owieszcze ognie z³a.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Wydaje ci siê, ¿e $n wygl±da jak prawdziwy demon, za spraw± otaczaj±cej go mrocznej aury, rozpalaj±cej w twoim sercu z³owieszcze ognie z³a.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 5:
                                    act( "Czujesz jak lepkie wici demonicznej aury $z oplataj± ciê delikatnie i dziel± siê z tob± czê¶ci± swojej mocy! ", ch, NULL, tch, TO_VICT );
                                    break;
                            }
                        }
                        else if ( IS_GOOD(tch))
                        {
                            switch(number_range(1,5))
                            {
                                case 1:
                                    if(ch->sex == 2)
                                        act( "Z przera¿eniem obserwujesz jak $n smieje siê z³owieszczo delektuj±c siê otaczaj±c± j± krwistoczerwon± aur±!", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Z przera¿eniem obserwujesz jak $n smieje siê z³owieszczo delektuj±c siê otaczaj±c± go krwistoczerwon± aur±!", ch, NULL, tch, TO_VICT );
                                    break;
                                case 2:
                                    act( "Czujesz jak jakie¶ z³e moce odrywaj± siê od otaczaj±cej $c po¶wiaty i próbuj± zapu¶ciæ siê wewn±trz twojej duszy. Czujesz jak wype³niaj± ciê ohydne uczucia trwogi, zw±tpienia, strachu, gniewu i nienawi¶ci! Brrr...", ch, NULL, tch, TO_VICT );
                                    break;
                                case 3:
                                    if(ch->sex == 2)
                                        act( "Mdli ciê na sam± my¶l o tym, ¿e obok ciebie stoi $n, przes±czona krwi± i ¶mierci±, otoczona demoniczn± aur± bólu, strachu i nienawi¶ci.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Mdli ciê na sam± my¶l o tym, ¿e obok ciebie stoi $n, przes±czony krwi± i ¶mierci±, otoczony demoniczn± aur± bólu, strachu i nienawi¶ci.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 4:
                                    if(ch->sex == 2)
                                        act( "Wydaje ci siê jakby $n patrzy³a na ciebie z ogromn± pogard± i nienawisci±, dok³adnie tak, jakby chcia³a przenikn±æ i wyssaæ z ciebie duszê. A mo¿e to tylko z³udzenie?", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Wydaje ci siê jakby $n patrzy³ na ciebie z ogromn± pogard± i nienawisci±, dok³adnie tak, jakby chcia³ przenikn±æ i wyssaæ z ciebie duszê. A mo¿e to tylko z³udzenie?", ch, NULL, tch, TO_VICT );
                                    break;
                                case 5:
                                    if(ch->sex == 2)
                                        act( "Zaczynasz w±tpiæ w to, czy po³±czenie si³ z $v by³o dobrym pomys³em. Otaczaj±ca j± aura jest bez w±tpienia przesi±kniêta ¶mierci± i z³em, a ona sama nie mo¿e mieæ ¿adnych dobrych intencji.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Zaczynasz w±tpiæ w to, czy po³±czenie si³ z $v by³o dobrym pomys³em. Otaczaj±ca go aura jest bez w±tpienia przesi±kniêta ¶mierci± i z³em, a on sam nie mo¿e mieæ ¿adnych dobrych intencji.", ch, NULL, tch, TO_VICT );
                                    break;
                            }
                        }
                        else
                        {
                            switch(number_range(1,5))
                            {
                                case 1:
                                    if(ch->sex == 2)
                                        act( "Spogl±dasz obojêtnie na emanuj±c± mrokiem $z, która z powazn±, acz wype³nion± gniewem twarz± wydaje siê byæ naprawdê potê¿n± osob±.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Spogl±dasz obojêtnie na emanuj±cego mrokiem $z, który z powazn±, acz wype³nion± gniewem twarz± wydaje siê byæ naprawdê potê¿n± osob±.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 2:
                                    if(ch->sex == 2)
                                        act( "Zaczynasz w±tpiæ, czy $n jest dobr± towarzyszk± wyprawy, bowiem otaczaj±ca j± krwawa aura ¶wiadczy o tym, ¿e raczej nie mo¿na jej ufaæ.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Zaczynasz w±tpiæ, czy $n jest dobrym towarzyszem wyprawy, bowiem otaczaj±ca go krwawa aura ¶wiadczy o tym, ¿e raczej nie mo¿na mu ufaæ.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 3:
                                    act( "Bez wiêkszego wra¿enia obserwujesz k±tem oka otaczaj±c± $c kristoczerwon±, demoniczn± aurê.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 4:
                                    act( "Czujesz jak otaczaj±ca $c demoniczna aura zapuszcza siê wewn±trz twojej duszy, próbuj±c zasiaæ w niej strach i zw±tpienie.", ch, NULL, tch, TO_VICT );
                                    break;
                                case 5:
                                    if(ch->sex == 2)
                                        act( "Z gniewem obserwujesz jak $n z rado¶ci± delektuje siê otaczaj±c± j± demoniczn± aur±. Masz wra¿enie jakby jakie¶ pierwotne instynkty próbowa³y przej±æ kontrolê nad twoim umys³em, jednak udaje ci siê zachowaæ spokój.", ch, NULL, tch, TO_VICT );
                                    else
                                        act( "Z gniewem obserwujesz jak $n z rado¶ci± delektuje siê otaczaj±c± go demoniczn± aur±. Masz wra¿enie jakby jakie¶ pierwotne instynkty próbowa³y przej±æ kontrolê nad twoim umys³em, jednak udaje ci siê zachowaæ spokój.", ch, NULL, tch, TO_VICT );
                                    break;
                            }
                        }
                    }
                    continue;
                }
            }
        }

		DEBUG_INFO( "viol_update:check_fight" );
		/* jesli nie walczy albo nie jest w roomie: zdejmij flage
		   plus sprawdz czy dalej walczy-zdejmij berserk*/
		if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
		{
			DEBUG_INFO( "viol_update:check_fight(1)" );
			strip_fight_affects( ch );
			DEBUG_INFO( "viol_update:check_fight(2)" );
			continue;
		}

		DEBUG_INFO( "viol_update:check_fight(3)" );
		if ( victim )
		{
			if ( ch->position == POS_STANDING )
				ch->position = POS_FIGHTING;
			if ( victim->position == POS_STANDING )
				victim->position = POS_FIGHTING;
		}

		DEBUG_INFO( "viol_update:check_fight(4)" );
		if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_MONK ) ) ||
				( !IS_NPC( ch ) && ch->class == CLASS_MONK ) )
			ch->num_attacks = 0;

		DEBUG_INFO( "viol_update:confusion" );
		//confusion
		paf = affect_find( ch->affected, gsn_confusion );
		if ( paf && paf->duration > 0 && number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) < number_range( 0, paf->modifier ) )
		{
			switch ( number_bits( 1 ) )
			{
				case 0:
					victim = get_random_char( ch, FALSE );
					if ( victim && !is_safe( ch, victim ) && ch != victim && victim != ch->fighting )
					{
						paf->duration = UMAX( 0, paf->duration - 1 ); paf->rt_duration = 0;
						ch->fighting = victim;
						send_to_char( "Czujesz siê trochê zdezorientowan<&y/a/e> i zaczynasz atakowaæ kogo¶ innego!\n\r", ch );
						act( "$n przez chwilê wygl±da na $t i po chwili zaczyna atakowaæ kogo¶ innego!",
								ch, ch->sex == 2 ? "zdezorientowan±" : "zdezorientowanego", victim, TO_NOTVICT );
					}
					else
						victim = ch->fighting;
					break;
				case 1:
					if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) != NULL && !IS_OBJ_STAT( weapon, ITEM_NOREMOVE ) )
					{
						paf->duration = UMAX( 0, paf->duration - 1 ); paf->rt_duration = 0;
						send_to_char( "Czujesz siê trochê zdezorientowan<&y/a/e> i wypuszczasz broñ z d³oni!\n\r", ch );
						act( "$n przez chwilê wygl±da na $t i po chwili wypuszcza broñ z d³oni!",
								ch, ch->sex == 2 ? "zdezorientowan±" : "zdezorientowanego", victim, TO_NOTVICT );
						obj_from_char( weapon );
						if ( IS_OBJ_STAT( weapon, ITEM_NODROP ) || IS_OBJ_STAT( weapon, ITEM_INVENTORY ) )
							obj_to_char( weapon, ch );
						else
							obj_to_room( weapon, ch->in_room );
					}
					break;
			}
		}

		DEBUG_INFO( "viol_update:berserk" );
		if( !IS_NPC(ch) && is_affected(ch,gsn_berserk ) && get_skill(ch,gsn_berserk) < number_range(-10,40) )
		{
			if( number_range(0,2*(get_curr_stat_deprecated(ch,STAT_INT) + get_curr_stat_deprecated(ch,STAT_WIS))) == 0 )
			{
                bool allow_attack = TRUE;
				victim = get_random_char( ch, FALSE );
                if(
                        victim
                        && ch->fighting != victim
                        && victim != ch
                        && !is_affected( victim, gsn_sanctuary )
                  )
                {
                    /**
                     * moby ucz±ce oraz ciê¿ko pracuj±ce nie mog± byæ tak traktowane, prawda?
                     */
                    if (
                            IS_NPC( victim)
                            && (
                                EXT_IS_SET( victim->act, ACT_PRACTICE )
                                || victim->pIndexData->pShop != NULL
                               )
                       )
                    {
                        allow_attack = FALSE;
                    }
                    if ( allow_attack )
                    {
                        act( "Niespodziewanie, w twojej duszy budzi siê jaki¶ prymitywny instynkt. Pragniesz mordowaæ, zabijaæ, siekaæ! Rzucasz siê z impetem na stoj±c± najbli¿ej ciebie osobê.", ch, NULL, victim, TO_CHAR );
                        act( "Nagle dzieje siê co¶ niespodziewanego. $n zaczyna sapaæ dziko, zdaje siê p³on±æ jak±¶ prymitwyn± si³±, i nagle rzuca siê na ciebie!", ch, NULL, victim, TO_VICT );
                        act( "Nie mo¿esz uwierzyæ w³asnym oczom! $n zaczyna zachowywaæ siê nad wyraz dziko i w morderczym szale rzuca siê na $C.", ch, NULL, victim, TO_NOTVICT );
                        stop_fighting( ch, FALSE );
                        multi_hit( ch, victim, TYPE_UNDEFINED );
                    }
				}
			}
		}

		DEBUG_INFO( "viol_update:check_fight2" );
		if ( ( victim = ch->fighting ) == NULL )
		{
			if ( !still_fighting( ch ) )
			{
				strip_fight_affects( ch );
				continue;
			}
		}

		DEBUG_INFO( "viol_update:strip_dazzling_flash" );
		if( victim && is_affected( victim, gsn_dazzling_flash) && affect_find( victim->affected, gsn_dazzling_flash)->level == 0 &&
			ch && is_affected( ch, gsn_dazzling_flash) &&
			!IS_AFFECTED( victim, AFF_PERFECT_SENSES) && ( !IS_AFFECTED(victim,AFF_DETECT_INVIS) || IS_AFFECTED(ch,AFF_NONDETECTION)) )
		{
			for( dazzling = ch->affected ; dazzling; dazzling = dazzling->next )
			{
				if( dazzling && dazzling->type == gsn_dazzling_flash && dazzling->level == 1 && affect_find( victim->affected, gsn_dazzling_flash)->modifier == dazzling->modifier )
				{
					affect_remove( ch, dazzling );
					affect_strip( victim, gsn_dazzling_flash );
					act("$N materializuje siê nagle.", victim, NULL, ch, TO_CHAR );
					act("Dostrzegasz zaskoczenie w oczach $z, twoje zaklêcie przesta³o dzia³aæ.", victim, NULL, ch, TO_VICT );
					break;
				}
			}
		}

		DEBUG_INFO( "viol_update:multi_hit" );
		if ( !can_move( ch ) && !can_move( victim ) )
			stop_fighting( ch, FALSE );

		if ( IS_AWAKE( ch ) && ch->in_room == victim->in_room )
		{
			if ( !IS_SET( ch->fight_data, FIGHT_DISARM ) &&
					!IS_SET( ch->fight_data, FIGHT_PARRY ) &&
					!IS_SET( ch->fight_data, FIGHT_CASTING ) &&
					!IS_SET( ch->fight_data, FIGHT_DODGE ) )
				multi_hit( ch, victim, TYPE_UNDEFINED );

			if ( ch->in_room && ch->wait < 4 )
				ch->wait = 4;
		}
		else
			stop_fighting( ch, FALSE );


		DEBUG_INFO( "viol_update:check_fight3" );
		if ( ( victim = ch->fighting ) == NULL )
		{
			if ( !still_fighting( ch ) )
			{
				strip_fight_affects( ch );
				continue;
			}
		}
		/*
		 * Fun for the whole family!
		 */


		DEBUG_INFO( "viol_update:check_assist" );
		check_assist( ch, victim );

		DEBUG_INFO( "viol_update:percent_hp_trig" );
		if ( IS_NPC( ch ) )
		{
			if ( HAS_TRIGGER( ch, TRIG_FIGHT ) )
				mp_percent_trigger( ch, victim, NULL, NULL, &TRIG_FIGHT );
			if ( HAS_TRIGGER( ch, TRIG_HPCNT ) )
				mp_hprct_trigger( ch, victim );
		}

		DEBUG_INFO( "viol_update:obj_fight_trig" );
		op_fight_trigger( ch );


		DEBUG_INFO( "viol_update:check_affect" );
		for ( paf = ch->affected; paf != NULL; paf = paf_next )
		{
			paf_next	= paf->next;

			if ( skill_table[ paf->type ].round_hour )
			{
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
						if ( paf->type > 0 && skill_table[ paf->type ].msg_off )
						{
							for( af_do_counta = ch->affected; af_do_counta; af_do_counta = af_do_counta->next )
							{
								if ( af_do_counta->type == paf->type )
									++af_count;
							}
/*							if( af_count == 1 ) narazie wywalam bo nie dziala i nie mam pojecia dlaczego
							{*/
								send_to_char( skill_table[ paf->type ].msg_off, ch );
								send_to_char( "\n\r", ch );
//							}
						}
						if ( paf->type == gsn_glorious_impale )
						{
							if( paf->modifier )
							{
								if( ch->fighting && is_affected( ch->fighting, gsn_glorious_impale ) && IS_AFFECTED( ch, AFF_PARALYZE ) && get_eq_char( ch->fighting,WEAR_WIELD) && get_eq_char( ch->fighting, WEAR_WIELD)->value[0] == WEAPON_SPEAR )
								{
									act( "$n gwa³townie wyrywa $h z twojego cia³a powiêkszaj±c jeszcze ranê.", ch->fighting, get_eq_char(ch->fighting,WEAR_WIELD), ch, TO_VICT );
									act( "$n gwa³townie wyrywa $h z cia³a $Z powiêkszaj±c jeszcze ranê.", ch->fighting, get_eq_char(ch->fighting,WEAR_WIELD), ch, TO_NOTVICT );
								}
							}
							else
							{
								if( ch->fighting && is_affected( ch->fighting, gsn_glorious_impale ) && IS_AFFECTED( ch->fighting, AFF_PARALYZE ) && get_eq_char( ch,WEAR_WIELD) && get_eq_char( ch, WEAR_WIELD)->value[0] == WEAPON_SPEAR )
								{
									act( "Gwa³townie wyrywasz $h z cia³a $Z powiêkszaj±c jeszcze ranê.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_CHAR );
									damage( ch, ch->fighting, 2*(dice(get_eq_char(ch,WEAR_WIELD)->value[1],get_eq_char(ch,WEAR_WIELD)->value[2])+get_eq_char(ch,WEAR_WIELD)->value[6]+GET_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))+GET_SKILL_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))),gsn_glorious_impale, DAM_PIERCE, FALSE );
								}
							}
						}
					}
					affect_remove( ch, paf );
				}
			}
		}
	}


	DEBUG_INFO( "viol_update:other_loop" );
	/* petla dla parowcow i reszty*/
	for ( ch = char_list; ch != NULL; ch = ch_next )
	{
		ch_next	= ch->next;

		DEBUG_INFO( "viol_update:check_casting" );

		if ( IS_SET( ch->fight_data, FIGHT_CASTING ) )
		{
			REMOVE_BIT( ch->fight_data, FIGHT_CASTING );
			continue;
		}

		DEBUG_INFO( "viol_update:berserk_rescue" );
		/* rescue automatyczny dla berserkerow nie tankujacych */
		check_berserk_rescue( ch );

		DEBUG_INFO( "viol_update:check_fight4" );
		if ( ( victim = ch->fighting ) == NULL ||
				ch->in_room == NULL ||
				victim->in_room == NULL ||
				!( IS_SET( ch->fight_data, FIGHT_PARRY ) ||
					IS_SET( ch->fight_data, FIGHT_DISARM ) ||
					IS_SET( ch->fight_data, FIGHT_DODGE ) ) )
		{
			if ( !still_fighting( ch ) )
				strip_fight_affects( ch );

			continue;
		}

		DEBUG_INFO( "viol_update:multi_hit(2)" );
		if ( IS_AWAKE( ch ) && ch->in_room == victim->in_room )
		{
			multi_hit( ch, victim, TYPE_UNDEFINED );

			if ( ch->wait < 4 )
				ch->wait = 4;
		}
		else
			stop_fighting( ch, FALSE );

		DEBUG_INFO( "viol_update:check_fight_last" );
		if ( ( victim = ch->fighting ) == NULL )
		{
			if ( !still_fighting( ch ) )
			{
				strip_fight_affects( ch );
				continue;
			}
		}

		/*
		 * Fun for the whole family!
		 */


		DEBUG_INFO( "viol_update:check_assist(2)" );
		check_assist( ch, victim );

		DEBUG_INFO( "viol_update:percent_hp_trig(2)" );
		if ( IS_NPC( ch ) )
		{
			if ( HAS_TRIGGER( ch, TRIG_FIGHT ) )
				mp_percent_trigger( ch, victim, NULL, NULL, &TRIG_FIGHT );
			if ( HAS_TRIGGER( ch, TRIG_HPCNT ) )
				mp_hprct_trigger( ch, victim );
		}

		DEBUG_INFO( "viol_update:obj_fight_trig" );
		op_fight_trigger( ch );

		DEBUG_INFO( "viol_update:rem_fight_data_flags" );
		REMOVE_BIT( ch->fight_data, FIGHT_PARRY );
		REMOVE_BIT( ch->fight_data, FIGHT_DISARM );
		REMOVE_BIT( ch->fight_data, FIGHT_DODGE );
	}

	return;
}

/* for auto assisting */
void check_assist( CHAR_DATA *ch, CHAR_DATA *victim )
{
	CHAR_DATA * rch, *rch_next;

	if ( !ch || !victim )
		return;

	for ( rch = ch->in_room->people; rch != NULL; rch = rch_next )
	{
		rch_next = rch->next_in_room;
		DEBUG_INFO( "void:check_assist(1)" );

		if ( number_bits(1) == 0 )
			continue;

		DEBUG_INFO( "void:check_assist(2)" );
		if ( IS_NPC( rch) && IS_AWAKE( rch )
				&& rch->fighting == NULL && can_move( rch )
				&& !IS_AFFECTED( rch, AFF_CHARM ) && mob_can_attack( rch ) )
		{
			DEBUG_INFO( "void:check_assist(3)" );
			if ( !IS_NPC( ch ) && EXT_IS_SET( rch->off_flags, OFF_ASSIST_PLAYERS ) )
			{
				DEBUG_INFO( "void:check_assist(4)" );
				if ( !IS_NPC( victim ) )
				{
					continue;
				}
				rch->position = POS_STANDING;
				switch(number_range(0,2))
				{
					case 1:
						do_function( rch, &do_emote, "w³±cza siê do walki!" );
						break;
					case 2:
						do_function( rch, &do_emote, "anga¿uje siê w wir walki!" );
						break;
					default:
						do_function( rch, &do_emote, "przy³±cza siê do walki!" );
						break;
				}
				multi_hit( rch, victim, TYPE_UNDEFINED );
				continue;
			}
			else if ( IS_NPC( ch ) && !IS_AFFECTED( ch, AFF_CHARM ) )
			{
				DEBUG_INFO( "void:check_assist(5)" );
				if ( EXT_IS_SET( rch->off_flags, OFF_ASSIST_ALL )
						|| ( rch->group && rch->group == ch->group )
						|| ( GET_RACE( rch ) == GET_RACE( ch ) && EXT_IS_SET( rch->off_flags, OFF_ASSIST_RACE ) )
						|| ( EXT_IS_SET( rch->off_flags, OFF_ASSIST_ALIGN ) && IS_SAME_ALIGN( rch, ch ) )
						|| ( rch->pIndexData->vnum == ch->pIndexData->vnum && EXT_IS_SET( rch->off_flags, OFF_ASSIST_VNUM ) ) )
				{
					CHAR_DATA * vch;
					CHAR_DATA *target;
					int number;


					target = NULL;
					number = 0;
					DEBUG_INFO( "void:check_assist(6)" );
					for ( vch = ch->in_room->people; vch; vch = vch->next )
					{
						if ( can_see( rch, vch )
								&& is_same_group( vch, victim )
								&& number_range( 0, number ) == 0 )
						{
							target = vch;
							number++;
						}
					}

					DEBUG_INFO( "void:check_assist(7)" );
					if ( target != NULL )
					{

						rch->position = POS_STANDING;
						switch(number_range(0,2))
						{
							case 1:
								do_function( rch, &do_emote, "w³±cza siê do walki!" );
								break;
							case 2:
								do_function( rch, &do_emote, "anga¿uje siê w wir walki!" );
								break;
							default:
								do_function( rch, &do_emote, "przy³±cza siê do walki!" );
								break;
						}
						multi_hit( rch, target, TYPE_UNDEFINED );
					}
				}
			}
		}
	}
}

/*
 * Do one group of attacks.
 */
void monk_multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
	if ( ch->position < POS_SITTING || !can_move( ch ) || ch->wait_char )
		return;
	DEBUG_INFO( "multi_hit:one_hit" );

	for ( ; ch->num_attacks < max_monk_hits( ch ); ch->num_attacks++ )
	{
		one_hit_monk( ch, victim, dt );

		if ( ch->fighting != victim )
			return;
	}


	/* haste daje 50% na dodatkowy atak z prawej reki */
	if ( (IS_AFFECTED( ch, AFF_HASTE ) && number_percent() < 50 ) || is_affected(ch, 300 ) )
		one_hit_monk( ch, victim, dt );

	return;
}

/*Do one group of attacks*/
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    AFFECT_DATA *aff, *abide, *abide2;
    CHAR_DATA *tch;
    OBJ_DATA * wield, * second, * spear;
    char msg[MAX_STRING_LENGTH];
    bool stop = FALSE, check, check_dual_wielding = FALSE;
    int vertical_skill, weapon_mastery, crowd = 0;
    int cleave_count, remembered_hp = victim->hit; //do rozplatania, jezeli cos nie zrani to sie zatrzymuje
    int speed = 0, speed_multiplier = 100, weapon_skill, i = 1;
    /*Bazowa szybko¶æ w walce jest 0 (definiuje dodatkowe ataki).
      Bazowny mno¿nik prêdko¶ci to 100 (czyli 100%). */

    // Na wszelki wypadek ustawiamy pierwszy znak na 0
    msg[0] = '\0';

    if ( ch->position < POS_SITTING || !can_move( ch ) || ch->wait_char )
    {
        return;
    }
    /**
     * moby ucz±ce oraz ciê¿ko pracuj±ce nie mog± byæ tak traktowane, prawda?
     */
    if (
            IS_NPC( victim)
            && (
                EXT_IS_SET( victim->act, ACT_PRACTICE )
                || victim->pIndexData->pShop != NULL
               )
       )
    {
        return;
    }

    //rzut na confusion
    aff = affect_find( ch->affected, gsn_confusion );
    if ( aff && aff->duration > 0 && number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > number_range( 0, aff->modifier ) )
    {
        switch(number_range(0,3))
        {
            case 1:
                send_to_char( "Czujesz siê dziwnie, bardzo dziwnie, w³a¶ciwie nie wiesz o co chodzi.\n\r", ch );
                break;
            case 2:
                send_to_char( "Ca³y ¶wiat staje na g³owie, a ty czujesz siê nieco dziwnie.\n\r", ch );
                break;
            case 3:
                send_to_char( "Nagle tracisz orientacje i nie wiesz co jest czym.\n\r", ch );
                break;
            default:
                send_to_char( "Czujesz siê trochê zdezorientowan<&y/a/e>.\n\r", ch );
                break;
        }
        aff->duration = UMAX( 0, aff->duration - 1 ); aff->rt_duration = 0;
        return;
    }

    DEBUG_INFO( "multi_hit:monk_hit" );

    if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_MONK ) ) ||
            ( !IS_NPC( ch ) && ch->class == CLASS_MONK ) )
    {
        monk_multi_hit( ch, victim, dt );
        return;
    }

    if ( IS_NPC( ch ) )
    {
        mob_chat_fight( ch );
        mob_hit( ch, victim, dt );
        return;
    }

    DEBUG_INFO( "multi_hit:glorious_impale" );
    if( is_affected( ch, gsn_glorious_impale ) )
    {
        if( !ch->fighting )
        {
            affect_strip( ch, gsn_glorious_impale );
        }
        else if ( !IS_AFFECTED( ch->fighting, AFF_PARALYZE ) || !is_affected( ch->fighting, gsn_glorious_impale ) )
        {
            act( "Gwa³townie wyrywasz $h z cia³a $Z powiêkszaj±c jeszcze ranê.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_CHAR );
            act( "$n gwa³townie wyrywa $h z twojego cia³a powiêkszaj±c jeszcze ranê.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_VICT );
            act( "$n gwa³townie wyrywa $h z cia³a $Z powiêkszaj±c jeszcze ranê.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_NOTVICT );
            affect_strip( ch->fighting, gsn_glorious_impale );
            damage( ch, ch->fighting, 2*(dice(get_eq_char(ch,WEAR_WIELD)->value[1],get_eq_char(ch,WEAR_WIELD)->value[2])+get_eq_char(ch,WEAR_WIELD)->value[6]+GET_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))+GET_SKILL_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))),gsn_glorious_impale, DAM_PIERCE, FALSE );
        }
        else
            return;
    }

    abide = affect_find( ch->affected, gsn_weapon_wrench );

    DEBUG_INFO( "multi_hit:trick_cyclone" );
    if( get_eq_char( ch, WEAR_WIELD ) && get_eq_char( ch, WEAR_SECOND ) && check_trick( ch, victim, SN_TRICK_CYCLONE ) && !abide)
        return;

    spear = get_eq_char(ch, WEAR_WIELD );
    DEBUG_INFO( "multi_hit:trick_dragon_strike" );
    if( spear && spear->item_type == ITEM_WEAPON && spear->value[0] == WEAPON_SPEAR && IS_WEAPON_STAT(spear,WEAPON_TWO_HANDS) && check_trick( ch, victim, SN_TRICK_DRAGON_STRIKE ) && !abide)
        return;

    spear = get_eq_char(ch, WEAR_WIELD );
    DEBUG_INFO( "multi_hit:trick_ravaging_orb" );
    if( spear && spear->item_type == ITEM_WEAPON && spear->value[0] == WEAPON_FLAIL && IS_WEAPON_STAT(spear,WEAPON_TWO_HANDS) && check_trick( ch, victim, SN_TRICK_RAVAGING_ORB ) && !abide )
        return;

    DEBUG_INFO( "multi_hit:one_hit" );

    if( IS_SET( ch->fight_data, FIGHT_HTH_CHARGE ))
        REMOVE_BIT( ch->fight_data, FIGHT_HTH_CHARGE );
    else
    {
        if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
            one_hit( ch, victim, dt, FALSE );
        else
        {
            for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
            {
                if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                {
                    print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                    print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                    sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                    act( msg, ch, NULL, victim, TO_NOTVICT );
                }
            }
            act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
        }
    }

    /* jesli z pierwszego ataku zszedl, koniec */
    if ( ch->fighting != victim )
        return;

    if( is_affected( ch, gsn_glorious_impale ) )
    {
        if( !ch->fighting || !IS_AFFECTED( ch->fighting, AFF_PARALYZE ) || !is_affected( ch->fighting, gsn_glorious_impale ) )
            affect_strip( ch, gsn_glorious_impale );
        else
            return;
    }

    DEBUG_INFO( "multi_hit:dualwield_hit" );
    check = FALSE;
    if ( get_eq_char ( ch, WEAR_SECOND ) && get_eq_char(ch,WEAR_SECOND)->item_type == ITEM_WEAPON &&
            (
             check
             || ch->class == CLASS_THIEF
             || ch->class == CLASS_WARRIOR
             || ch->class == CLASS_PALADIN
             || ch->class == CLASS_BARD
             || ch->class == CLASS_BLACK_KNIGHT
             || ch->class == CLASS_BARBARIAN
             || ch->class == CLASS_SHAMAN
             ||
             (
              ch->class == CLASS_DRUID
              && get_eq_char( ch, WEAR_WIELD)
              && get_eq_char( ch, WEAR_WIELD)->value[0] == WEAPON_CLAWS
              && get_eq_char( ch,WEAR_SECOND)->value[0] == WEAPON_CLAWS
             )
            )
       )
    {
        if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
            one_hit( ch, victim, dt, TRUE );
        else
        {
            for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
            {
                if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                {
                    print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                    print_char( victim, "%s próbuje zadaæ cios cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                    sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                    act( msg, ch, NULL, victim, TO_NOTVICT );
                }
            }
            act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
        }
    }

    if ( ch->fighting != victim )
        return;

    if( is_affected( ch, gsn_glorious_impale ) )
    {
        if( !ch->fighting || !IS_AFFECTED( ch->fighting, AFF_PARALYZE ) || !is_affected( ch->fighting, gsn_glorious_impale ) )
            affect_strip( ch, gsn_glorious_impale );
        else
            return;
    }

    DEBUG_INFO( "multi_hit:vertical_slash" );

    /**
     * wezmij bronie
     */
    wield = get_eq_char(ch,WEAR_WIELD);
    second = get_eq_char(ch,WEAR_SECOND);

    //i tutaj vertical slash black knighta. Wczesniejszy atak musial trafic
    if ( wield && IS_SET( ch->fight_data, FIGHT_VERTICAL_SLASH ) && victim->hit != remembered_hp )
    {
        REMOVE_BIT( ch->fight_data, FIGHT_VERTICAL_SLASH );
        if ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) &&
                wield->value[0] != WEAPON_DAGGER &&
                wield->value[0] != WEAPON_WHIP &&
                wield->value[0] != WEAPON_MACE &&
                wield->value[0] != WEAPON_STAFF &&
                wield->value[0] != WEAPON_SPEAR &&
                wield->value[0] != WEAPON_FLAIL &&
                wield->value[0] != WEAPON_SHORTSWORD &&
                wield->value[0] != WEAPON_CLAWS )
        {
            vertical_skill = get_skill( ch, gsn_vertical_slash );
            vertical_skill += (get_curr_stat_deprecated(ch,STAT_DEX)-10)*2;
            vertical_skill -= (get_curr_stat_deprecated(victim,STAT_DEX)-10)*2;
            if ( IS_AFFECTED( ch, AFF_SLOW ) ) vertical_skill /= 4;
            if ( IS_AFFECTED( ch, AFF_HASTE ) )	vertical_skill += 10;

            switch ( victim->size )//dla size tiny i giant nigdy nie wejdzie
            {
                case SIZE_SMALL:
                    vertical_skill -= 10;
                    break;
                case SIZE_LARGE:
                    vertical_skill -= 10;
                    break;
                case SIZE_HUGE:
                    vertical_skill -= 25;
                    break;
                default:
                    break;
            }

            if( victim->size == SIZE_TINY )
            {
                if( victim->sex == 2 )
                    act( "$N jest za ma³a, by próbowaæ na niej takich sztuczek.", ch, NULL, victim, TO_CHAR );
                else
                    act( "$N jest za ma³y, by próbowaæ na nim takich sztuczek.", ch, NULL, victim, TO_CHAR );
            }
            else if ( victim->size == SIZE_GIANT )
            {
                if( victim->sex == 2 )
                    act( "$N jest zbyt wielka, by próbowaæ na niej takich sztuczek.", ch, NULL, victim, TO_CHAR );
                else
                    act( "$N jest zbyt wielki, by próbowaæ na nim takich sztuczek.", ch, NULL, victim, TO_CHAR );
            }
            else if ( number_percent() < vertical_skill || !can_move(victim))
            {
                switch( number_range(1,5))
                {
                    case 1:
                        act( "Po zadaniu ciosu czujesz, ¿e lekko oszo³omiony przeciwnik nie jest gotowy na ¿adn± obronê, co w sposób bezwzglêdny wykorzystujesz wyprowadzaj±c natychmiast kolejne ciêcie, tym razem pionowo z do³u w górê.", ch, wield, victim, TO_CHAR );
                        act( "Cios $z lekko ciê oszo³omi³, co $e w sposób bezwzglêdny wykorzystuje wyprowadzaj±c natychmiast kolejne ciêcie, tym razem pionowo z do³u w górê.", ch, wield, victim, TO_VICT );
                        act( "Cios $z lekko oszo³omi³ $C, co zostaje w sposób bezwzglêdny wykorzystane wyprowadzeniem natychmiast kolejnego ciêcie, tym razem pionowo z do³u w górê.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 2:
                        act( "Poprawiasz szybko swój chwyt na rêkoje¶ci $f i momentalnie tniesz ponownie z do³u w górê.", ch, wield, victim, TO_CHAR );
                        act( "$n poprawia szybko swój chwyt na rêkoje¶ci $f i momentalnie tnie ciê ponownie z do³u w górê.", ch, wield, victim, TO_VICT );
                        act( "$n poprawia szybko swój chwyt na rêkoje¶ci $f i momentalnie tnie ponownie z do³u w górê.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 3:
                        act( "Wykorzystuj±c impet pierwszego ciosu i natychmiast tniesz po tej samej lini co poprzednio prowadz±c ostrze swojej broni ku górze.", ch, wield, victim, TO_CHAR );
                        act( "$n wykorzystuj±c impet pierwszego ciosu natychmiast tnie ciê po tej samej lini co poprzednio prowadz±c ostrze swojej broni ku górze.", ch, wield, victim, TO_VICT );
                        act( "$n wykorzystuj±c impet pierwszego ciosu natychmiast tnie po tej samej lini co poprzednio prowadz±c ostrze swojej broni ku górze.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 4:
                        act( "Zaskoczywszy przeciwnika zupe³nie pierwszym ciosem z góry do do³u, wyprowadzasz kolejne potê¿ne ciêcie w lini pionowej w odwrotnym kierunku.", ch, wield, victim, TO_CHAR );
                        act( "Zaskoczywszy ciê zupe³nie pierwszym ciosem z góry do do³u, $n wyprowadza kolejne potê¿ne ciêcie w lini pionowej w odwrotnym kierunku.", ch, wield, victim, TO_VICT );
                        act( "Zaskoczywszy $C zupe³nie pierwszym ciosem z góry do do³u, $n wyprowadza kolejne potê¿ne ciêcie w lini pionowej w odwrotnym kierunku.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 5:
                        act( "Udaje ci siê po mistrzowsku wykorzystaæ sprzyjaj±c± sytuacjê po zadaniu pierwszego ciosu aby w ¶lad za nim uderzyæ po raz kolejny tn±c wroga od stóp do g³owy.", ch, wield, victim, TO_CHAR );
                        act( "$x udaje siê wykorzystaæ sprzyjaj±c± sytuacjê po zadaniu pierwszego ciosu aby w ¶lad za nim uderzyæ ciê po raz kolejny tn±c od stóp do g³owy.", ch, wield, victim, TO_VICT );
                        act( "$x udaje siê wykorzystaæ sprzyjaj±c± sytuacjê po zadaniu pierwszego ciosu aby w ¶lad za nim uderzyæ $C po raz kolejny tn±c od stóp do g³owy.", ch, wield, victim, TO_NOTVICT );
                        break;
                }

                one_hit( ch, victim, gsn_vertical_slash , FALSE );
                check_improve( ch, victim, gsn_vertical_slash, TRUE, 40 );
            }
            else if( number_percent() > URANGE( 50, get_curr_stat_deprecated(ch,STAT_DEX)*4 + vertical_skill/3, 95) )
            {
                switch( number_range(1,5))
                {
                    case 1:
                        act( "Zaatakowawszy ponownie po wyprowadzeniu pierwszego ciosu stwierdzasz ze zgroz±, ¿e przeciwnik w ostatniej chwili zd±zy³ siê uchyliæ. Twoja broñ tnie powietrze, a ty sa<&m/ma/mo> lecisz do przodu na spotkanie gruntu.", ch, wield, victim, TO_CHAR );
                        act( "Wykonujesz g³adki unik przed niezgrabnym atakiem $z, $s broñ tnie powietrze i leci $e do przodu na spotkanie gruntu.", ch, wield, victim, TO_VICT );
                        act( "$N wykonuje g³adki unik przed niezgrabnym atakiem $z, $s broñ tnie powietrze i leci $e do przodu na spotkanie gruntu.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 2:
                        act( "Próbujesz zmieniæ chwyt na rêkoje¶ci swej broni, jednak robisz to tak nieudolnie, ¿e wypada ci ona z r±k. Szybko próbujesz naprawiæ sytuacje i przerywasz p³ynny ruch, który wykonywa³<&e¶/a¶/o¶>, dziêki czemu udaje ci siê natychmiast z³apaæ $h, ale kosztem utraty równowagi.", ch, wield, victim, TO_CHAR );
                        act( "$n próbuje zmieniæ chwyt na rêkoje¶ci swej broni, jednak robi to tak nieudolnie, ¿e wypada ona z $s r±k. Szybko próbuje $e naprawiæ sytuacje i przerywa p³ynny ruch, który mia³ ci pewnie zrobiæ krzywdê. Udaje siê $o natychmiast z³apaæ $h, ale kosztem utraty równowagi.", ch, wield, victim, TO_VICT );
                        act( "$n próbuje zmieniæ chwyt na rêkoje¶ci swej broni, jednak robi to tak nieudolnie, ¿e wypada ona z $s r±k. Szybko próbuje $e naprawiæ sytuacje i przerywa p³ynny ruch, który mia³ pewnie zrobiæ krzywdê $X. Udaje siê $o natychmiast z³apaæ $h, ale kosztem utraty równowagi.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 3:
                        act( "Po udanym pierwszym ciosie wydawa³o ci siê, ¿e zadanie kolejnego momentalnie po pierwszym nie bêdzie trudne. Sztuka ta jednak ciê przeros³a i w trakcie zamachu straci³<&e¶/a¶/o¶> równowagê wywracaj±c siê na ziemie.", ch, wield, victim, TO_CHAR );
                        act( "Po udanym pierwszym ciosie $x wydawa³o siê, ¿e zadanie ci kolejnego momentalnie po pierwszym nie bêdzie trudne. Na szczê¶cie sztuka ta przeros³a $s umiejêtno¶ci i w trakcie zamachu równowaga zosta³a stracona, przez co $n le¿y teraz na ziemi.", ch, wield, victim, TO_VICT );
                        act( "Po udanym pierwszym ciosie $x wydawa³o siê, ¿e zadanie kolejnego momentalnie po pierwszym nie bêdzie trudne. Sztuka ta przeros³a jednak $s umiejêtno¶ci i w trakcie zamachu równowaga zosta³a stracona, przez co $n le¿y teraz na ziemi.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 4:
                        act( "Kiedy twoje pierwsze ciêcie wyprowadzone znad g³owy przynios³o wyj±tkowo dobry efekt próbujesz powtórzyæ cios, tym razem bior±c zamach od do³u. Pechowo jednak zahaczasz koñcem $f o pod³o¿e, co ca³kowicie wyprowadza ciê z rytmu i równowagi. Upadasz na ziemiê.", ch, wield, victim, TO_CHAR );
                        if( ch->sex == 2 )
                        {
                            act( "$n próbuje powtórzyæ cios, tym razem bior±c zamach od do³u. Na szczê¶cie jednak zahacza koñcem $f o pod³o¿e, co ca³kowicie wyprowadza j± z rytmu i równowagi. Upada ona na ziemiê.", ch, wield, victim, TO_VICT );
                            act( "$n próbuje powtórzyæ cios, tym razem bior±c zamach od do³u, zahacza jednak koñcem $f o pod³o¿e, co ca³kowicie wyprowadza j± z rytmu i równowagi. Upada ona na ziemiê.", ch, wield, victim, TO_NOTVICT );
                        }
                        else
                        {
                            act( "$n próbuje powtórzyæ cios, tym razem bior±c zamach od do³u. Na szczê¶cie jednak zahacza koñcem $f o pod³o¿e, co ca³kowicie wyprowadza go z rytmu i równowagi. Upada on na ziemiê.", ch, wield, victim, TO_VICT );
                            act( "$n próbuje powtórzyæ cios, tym razem bior±c zamach od do³u, zahacza jednak koñcem $f o pod³o¿e, co ca³kowicie wyprowadza go z rytmu i równowagi. Upada on na ziemiê.", ch, wield, victim, TO_NOTVICT );
                        }
                        break;
                    case 5:
                        act( "Zadawszy pierwszy cios wpadasz w dziki sza³, istny amok, i nie my¶l±c zbyt du¿o natychmiast chcesz powtórzyæ poprzedni wyczyn, jednak w swym zapamiêtaniu wypadasz z rytmu i ostatecznie walisz siê na ziemiê jak d³ug<&i/a/ie>.", ch, wield, victim, TO_CHAR );
                        act( "Zadawszy pierwszy cios $n wpada w dziki sza³, istny amok, i nie my¶l±c zbyt du¿o natychmiast chce powtórzyæ poprzedni wyczyn, na szczê¶cie w swym zapamiêtaniu wypada z rytmu i ostatecznie wali siê na ziemiê.", ch, wield, victim, TO_VICT );
                        act( "Zadawszy pierwszy cios $n wpada w dziki sza³, istny amok, i nie my¶l±c zbyt du¿o natychmiast chce powtórzyæ poprzedni wyczyn, jednak w swym zapamiêtaniu wypada z rytmu i ostatecznie wali siê na ziemiê.", ch, wield, victim, TO_NOTVICT );
                        break;
                }

                if ( ch->position > POS_SITTING )
                    ch->position = POS_SITTING;
                check_improve( ch, victim, gsn_vertical_slash, FALSE, 50 );
            }
            else
            {
                switch( number_range(1,5))
                {
                    case 1:
                        act( "Przez moment wydaje ci siê, ¿e przeciwnik nie zd±zy³ pozbieraæ siê po pierwszym ciosie i próbujesz wyprowadziæ natychmiast kolejny, $E jednak uchyla siê w ostatniej chwili.", ch, wield, victim, TO_CHAR );
                        act( "$x przez moment wydaje siê, ¿e nie zd±zy³<&e¶/a¶/o¶> pozbieraæ siê po pierwszym ciosie i próbuje wyprowadziæ natychmiast kolejny, ty jednak uchylasz siê bez k³opotu.", ch, wield, victim, TO_VICT );
                        act( "$n próbuje wyprowadziæ natychmiast kolejny cios, $N jednak uchyla siê bez k³opotu.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 2:
                        act( "Przepe³nion<&y/a/e> dzikim sza³em po perfekcyjnym pierwszym ciêciu w euforii starasz siê bezzw³ocznie zaatakowaæ po raz kolejny, jednak pud³ujesz haniebnie.", ch, wield, victim, TO_CHAR );
                        act( "Po perfekcyjnym pierwszym ciêciu $n w euforii stara siê bezzw³ocznie zaatakowaæ ciê po raz kolejny, jednak pud³uje haniebnie.", ch, wield, victim, TO_VICT );
                        act( "Po perfekcyjnym pierwszym ciêciu $n w euforii stara siê bezzw³ocznie zaatakowaæ $C po raz kolejny, jednak pud³uje haniebnie.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 3:
                        act( "Wyprowadziwszy silne ciêcie znad g³owy starasz siê poprawiæ swój chwyt na rêkoje¶ci $f, ta jednak wymyka ci siê na chwilê z r±k i tracisz kilka chwil, które mog³<&e¶/a¶/o¶> wykorzystaæ na zadanie kolejnego ataku.", ch, wield, victim, TO_CHAR );
                        act( "$n wyprowadziwszy silne ciêcie znad g³owy stara siê poprawiæ swój chwyt na rêkoje¶ci $f, ta jednak wymyka siê na chwilê z $s r±k i traci $e kilka chwil, które mog³y zostaæ wykorzystaæ na zadanie ci kolejnego ciosu.", ch, wield, victim, TO_VICT );
                        act( "$n wyprowadziwszy silne ciêcie znad g³owy stara siê poprawiæ swój chwyt na rêkoje¶ci $f, ta jednak wymyka siê na chwilê z $s r±k i traci $e kilka chwil, które mog³y zostaæ wykorzystaæ na zadanie $X kolejnego ciosu.", ch, wield, victim, TO_NOTVICT );
                        break;
                    case 4:
                        act( "Starasz siê wyprowadziæ kolejny atak po zadaniu pierwszego, jednak przeciwnik przewidzia³ twoje zamiary i w sprytny sposób odskakuje lekko na bok.", ch, wield, victim, TO_CHAR );
                        if( ch->sex != 2 )
                        {
                            act( "$n stara³ siê wyprowadziæ kolejny atak po zadaniu pierwszego, jednak przewidzia³<&e¶/a¶/o¶> $s zamiary i w sprytny sposób odskakujesz lekko na bok.", ch, wield, victim, TO_VICT );
                            act( "$n stara³ siê wyprowadziæ kolejny atak po zadaniu pierwszego, jednak $N w sprytny sposób odskakuje lekko na bok.", ch, wield, victim, TO_NOTVICT );
                        }
                        else
                        {
                            act( "$n stara³a siê wyprowadziæ kolejny atak po zadaniu pierwszego, jednak przewidzia³<&e¶/a¶/o¶> $s zamiary i w sprytny sposób odskakujesz lekko na bok.", ch, wield, victim, TO_VICT );
                            act( "$n stara³a siê wyprowadziæ kolejny atak po zadaniu pierwszego, jednak $N w sprytny sposób odskakuje lekko na bok.", ch, wield, victim, TO_NOTVICT );
                        }
                        break;
                    case 5:
                        act( "Po wyprowadzeniu ciosu masz ochotê ponowiæ atak, jednak wahasz siê o jedn± chwilê za d³ugo i sprzyjaj±ca sytuacja mija.", ch, wield, victim, TO_CHAR );
                        act( "Po wyprowadzeniu ciosu $n ma ochotê ponowiæ atak, jednak waha siê o jedn± chwilê za d³ugo i sprzyjaj±ca sytuacja mija.", ch, wield, victim, TO_VICT );
                        act( "Po wyprowadzeniu ciosu $n ma ochotê ponowiæ atak, jednak waha siê o jedn± chwilê za d³ugo i sprzyjaj±ca sytuacja mija.", ch, wield, victim, TO_NOTVICT );
                        break;
                }
                check_improve( ch, victim, gsn_vertical_slash, FALSE, 50 );
            }
        }
    }

    if ( ch->fighting != victim )
        return;

    DEBUG_INFO( "multi_hit: Additional_attacks" );
    // Drake: Generalnie teraz to moja dzia³ka muahahahaha

    /* Drake: Wy³aczam na razie.
    //Tylko klasy kwalifikuj±ce siê mniej lub bardziej pod 'zbrojnych' zamiast 'casterów'.
    if (
    ch->class == CLASS_THIEF
    || ch->class == CLASS_WARRIOR
    || ch->class == CLASS_PALADIN
    || ch->class == CLASS_BARD
    || ch->class == CLASS_BLACK_KNIGHT
    || ch->class == CLASS_BARBARIAN
    || ch->class == CLASS_SHAMAN )

     */
    //Najpierw idzie umiejêtno¶æ pos³ugiwania siê broni±
    //Pierwsza rêka
    if ( !IS_NPC( ch ) )
    {
        DEBUG_INFO( "Additional_attacks: Skill_modifier" );
        if ( get_eq_char ( ch, WEAR_WIELD ) && get_eq_char(ch,WEAR_WIELD)->item_type == ITEM_WEAPON )
        {
            wield = get_eq_char(ch,WEAR_WIELD);
            if (
             (wield->value[0] == WEAPON_SWORD      && ( weapon_skill = get_skill(ch, gsn_sword)      ) > 0 )||
             (wield->value[0] == WEAPON_DAGGER     && ( weapon_skill = get_skill(ch, gsn_dagger)     ) > 0 )||
             (wield->value[0] == WEAPON_SPEAR      && ( weapon_skill = get_skill(ch, gsn_spear)      ) > 0 )||
             (wield->value[0] == WEAPON_MACE       && ( weapon_skill = get_skill(ch, gsn_mace)       ) > 0 )||
             (wield->value[0] == WEAPON_AXE        && ( weapon_skill = get_skill(ch, gsn_axe)        ) > 0 )||
             (wield->value[0] == WEAPON_FLAIL      && ( weapon_skill = get_skill(ch, gsn_flail)      ) > 0 )||
             (wield->value[0] == WEAPON_WHIP       && ( weapon_skill = get_skill(ch, gsn_whip)       ) > 0 )||
             (wield->value[0] == WEAPON_POLEARM    && ( weapon_skill = get_skill(ch, gsn_polearm)    ) > 0 )||
             (wield->value[0] == WEAPON_STAFF      && ( weapon_skill = get_skill(ch, gsn_staff)      ) > 0 )||
             (wield->value[0] == WEAPON_SHORTSWORD && ( weapon_skill = get_skill(ch, gsn_shortsword) ) > 0 )||
             (wield->value[0] == WEAPON_CLAWS      && ( weapon_skill = get_skill(ch, gsn_claws)      ) > 0 )
            )
            {
                speed += weapon_skill /5 ; //Od 0-20 dodatkowego speeda od wyszkolenia broni.
            }
        }

        //Opcjonalnie: Druga rêka
        if ( get_eq_char ( ch, WEAR_SECOND ) && get_eq_char(ch,WEAR_SECOND)->item_type == ITEM_WEAPON )
        {
            check_dual_wielding = TRUE;
            if (
                    (second->value[0] == WEAPON_SWORD      && ( weapon_skill = get_skill(ch, gsn_sword)      ) > 0 )||
                    (second->value[0] == WEAPON_DAGGER     && ( weapon_skill = get_skill(ch, gsn_dagger)     ) > 0 )||
                    (second->value[0] == WEAPON_SPEAR      && ( weapon_skill = get_skill(ch, gsn_spear)      ) > 0 )||
                    (second->value[0] == WEAPON_MACE       && ( weapon_skill = get_skill(ch, gsn_mace)       ) > 0 )||
                    (second->value[0] == WEAPON_AXE        && ( weapon_skill = get_skill(ch, gsn_axe)        ) > 0 )||
                    (second->value[0] == WEAPON_FLAIL      && ( weapon_skill = get_skill(ch, gsn_flail)      ) > 0 )||
                    (second->value[0] == WEAPON_WHIP       && ( weapon_skill = get_skill(ch, gsn_whip)       ) > 0 )||
                    (second->value[0] == WEAPON_POLEARM    && ( weapon_skill = get_skill(ch, gsn_polearm)    ) > 0 )||
                    (second->value[0] == WEAPON_STAFF      && ( weapon_skill = get_skill(ch, gsn_staff)      ) > 0 )||
                    (second->value[0] == WEAPON_SHORTSWORD && ( weapon_skill = get_skill(ch, gsn_shortsword) ) > 0 )||
                    (second->value[0] == WEAPON_CLAWS      && ( weapon_skill = get_skill(ch, gsn_claws)      ) > 0 )
               )
            {
                speed += weapon_skill /5 ; //Od 0-20 dodatkowego speeda od wyszkolenia broni.
            }
            speed /= 2; //Gdy u¿ywamy dwóch broni to sumujemy szybko¶æ wyszkolenia obu broni i dzielimy przez dwa.
        }

        //Sprawdzanie dexa - generalnie niezrêczniakom dziêkujemy
        speed +=  get_curr_stat(ch, STAT_DEX) / 5  ; // do 42 dodatkowego speeda od zrêczno¶ci.

        //Sprawdzanie masterki - pierwsza rêka (max +6)
        if ( get_eq_char ( ch, WEAR_WIELD ) && get_eq_char(ch,WEAR_WIELD)->item_type == ITEM_WEAPON )
        {
            wield = get_eq_char(ch,WEAR_WIELD);
            if (
             (wield->value[0] == WEAPON_SWORD      && ( weapon_mastery = get_skill(ch, gsn_sword_mastery)      ) > 0 )||
             (wield->value[0] == WEAPON_DAGGER     && ( weapon_mastery = get_skill(ch, gsn_dagger_mastery)     ) > 0 )||
             (wield->value[0] == WEAPON_SPEAR      && ( weapon_mastery = get_skill(ch, gsn_spear_mastery)      ) > 0 )||
             (wield->value[0] == WEAPON_MACE       && ( weapon_mastery = get_skill(ch, gsn_mace_mastery)       ) > 0 )||
             (wield->value[0] == WEAPON_AXE        && ( weapon_mastery = get_skill(ch, gsn_axe_mastery)        ) > 0 )||
             (wield->value[0] == WEAPON_FLAIL      && ( weapon_mastery = get_skill(ch, gsn_flail_mastery)      ) > 0 )||
             (wield->value[0] == WEAPON_WHIP       && ( weapon_mastery = get_skill(ch, gsn_whip_mastery)       ) > 0 )||
             (wield->value[0] == WEAPON_POLEARM    && ( weapon_mastery = get_skill(ch, gsn_polearm_mastery)    ) > 0 )||
             (wield->value[0] == WEAPON_STAFF      && ( weapon_mastery = get_skill(ch, gsn_staff_mastery)      ) > 0 )||
             (wield->value[0] == WEAPON_SHORTSWORD && ( weapon_mastery = get_skill(ch, gsn_shortsword_mastery) ) > 0 )
            )
            {
                speed += weapon_mastery / 15; //Masterke mo¿emy mieæ jedn±, wiêæ nie ma potrzeby dzieliæ.
            }
        }
        else if ( get_eq_char ( ch, WEAR_SECOND ) && get_eq_char(ch,WEAR_SECOND)->item_type == ITEM_WEAPON )
        {
            second = get_eq_char(ch,WEAR_SECOND);
            if (
             (second->value[0] == WEAPON_SWORD      && ( weapon_mastery = get_skill(ch, gsn_sword)      ) > 0 )||
             (second->value[0] == WEAPON_DAGGER     && ( weapon_mastery = get_skill(ch, gsn_dagger)     ) > 0 )||
             (second->value[0] == WEAPON_SPEAR      && ( weapon_mastery = get_skill(ch, gsn_spear)      ) > 0 )||
             (second->value[0] == WEAPON_MACE       && ( weapon_mastery = get_skill(ch, gsn_mace)       ) > 0 )||
             (second->value[0] == WEAPON_AXE        && ( weapon_mastery = get_skill(ch, gsn_axe)        ) > 0 )||
             (second->value[0] == WEAPON_FLAIL      && ( weapon_mastery = get_skill(ch, gsn_flail)      ) > 0 )||
             (second->value[0] == WEAPON_WHIP       && ( weapon_mastery = get_skill(ch, gsn_whip)       ) > 0 )||
             (second->value[0] == WEAPON_POLEARM    && ( weapon_mastery = get_skill(ch, gsn_polearm)    ) > 0 )||
             (second->value[0] == WEAPON_STAFF      && ( weapon_mastery = get_skill(ch, gsn_staff)      ) > 0 )||
             (second->value[0] == WEAPON_SHORTSWORD && ( weapon_mastery = get_skill(ch, gsn_shortsword) ) > 0 )
            )
            {
                speed += weapon_mastery / 20;
            }
        }

        //Dodatek dla szamanow pod wp³ywem dzia³ania Ancestor's Fury (max +12)
        if (ch->class == CLASS_SHAMAN && IS_AFFECTED ( ch, AFF_ANCESTORS_FURY))
        {
            speed +=  get_skill(ch, gsn_invoke_spirit)/8;
        }

        if ( IS_AFFECTED( ch, AFF_SLOW ) )
        {
            speed /= 2;
        }

        // Kara dla 'casterów', ¿eby nie bili za dobrze.
        if (
                ch->class == CLASS_MAG
                || ch->class == CLASS_CLERIC
                || ch->class == CLASS_DRUID
                )
        {
            speed /= 2;
        }

        // Lekka kara dla z³odzieji. Ot, ¿eby nie mieli za dobrze.
        if ( ch->class == CLASS_THIEF )
        {
            speed -= speed/8;
        }

        // Przewróceni s± wolniejsi.
        if ( ch->position == POS_SITTING )
        {
            speed -= speed/3;
        }

        // Ludzie z tarczami s± wolniejsi.
        if ( get_eq_char( victim, WEAR_SHIELD ) != NULL )
        {
            speed -= speed/10;
        }

        /**
         * gruba dopalka od haste'a
         */
        if ( is_affected( ch, skill_lookup( "haste" ) ) )
        {
            speed *= 3;
            speed /= 2;
        }

        speed_multiplier = 100;

        DEBUG_INFO( "Additional_attacks: weight_modifier" );
        //Waga broni/12 zmniejsza szybko¶æ o 1%, ka¿dy punkt si³y powy¿ej 18 zwiêksza szybko¶æ o 1%.
        if ( wield && wield->item_type == ITEM_WEAPON )
        {
            DEBUG_INFO( "Additional_attacks: weight_modifier: 1" );
            speed_multiplier -= wield->weight / 12;
            if ( second && second->item_type == ITEM_WEAPON )
            {
                DEBUG_INFO( "Additional_attacks: weight_modifier: 2" );
                speed_multiplier -= second->weight/10;
            }
        }
        speed_multiplier += get_curr_stat( ch, STAT_STR ) / 10;
        DEBUG_INFO( "Additional_attacks: weight_modifier: 5" );

        //Lets Start this Opera Sh*t! - odpalamy pêtle

        speed = ( speed * UMIN(speed_multiplier, 100) ) / 100;

        if ( ch->pcdata->wiz_conf & W6 )
        {
            print_char( ch, "Speed - po odliczeniu: %d\n\r", speed );
            print_char( ch, "Speed - modyfikator: %d\n\r", speed_multiplier );
        }

        DEBUG_INFO( "Additional_attacks: for_loop" );
        for ( i = 1; i < 4; i++ ) //Max 4 ataki, albo jeden nieudany.
        {
            //Generalnie to sprawdzamy czy wyprowadzi³ dodatkowy atak
            if (number_percent() < URANGE (0, speed + (number_range(0, get_curr_stat_deprecated(ch, STAT_LUC)) - number_range(0, get_curr_stat_deprecated(victim, STAT_LUC))), 95))
            {
							//Sprawdzamy czy u¿ywa tarczy. Jak tak, to szansa, ¿e walnie tarcz±.
							if( get_eq_char( ch, WEAR_SHIELD ) != NULL && number_percent() < 40 )
              one_hit_shield( ch, victim, dt );
                //Sprawdzamy czy u¿ywa dwóch broni, je¶li tak to dajemy 60% na atak z pierwszej ³apy, jak nie wejdzie to wyprowadzamy atak z drugiej ³apy.
                else if ( check_dual_wielding )
                {
                    if (number_percent() < 60 )
                        if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
                            one_hit( ch, victim, dt , FALSE );
                        else
                        {
                            for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                            {
                                if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                                {
                                    print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                                    print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                                    sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                                    act( msg, ch, NULL, victim, TO_NOTVICT );
                                }
                            }
                            act( "Nie jeste¶ w stanie wykonaæ $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
                        }

                    else
                        if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
                            one_hit( ch, victim, dt , TRUE );
                        else
                        {
                            for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                            {
                                if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                                {
                                    print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                                    print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                                    sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                                    act( msg, ch, NULL, victim, TO_NOTVICT );
                                }
                            }
                            act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
                        }

                }
                else if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
                        one_hit( ch, victim, dt , FALSE );
                    else
                    {
                        for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                        {
                            if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                            {
                                print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                                print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                                sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                                act( msg, ch, NULL, victim, TO_NOTVICT );
                            }
                        }
                        act( "Nie jeste¶ w stanie wykonaæ $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
                    }

                //Odejmujemy speeda po wyprowadzonym ataku, je¶li speed jest mniejszy ni¿ 0 to dajemy na 0.
                //Je¶li nie ma wroga to dodatkowo koñczymy pêtle, by ataki nie sz³y w pró¿nie.
                speed -= speed/4+8;
                if (speed < 0) speed = 0;
                if ( ch->fighting != victim ) i = 4;
                if ( ch->pcdata->wiz_conf & W6 )
                {
                    print_char( ch, "Speed: %d\n\r", speed );
                }
            }
            else
                //Je¶li mu siê nie uda³o setujemy i na 4, koncz±c pêtle.
            {
                i = 4;
            }
        }
    }
    DEBUG_INFO( "multi_hit:haste_hit" );
    /* haste daje szane na dodatkowy atak z losowej reki */
    if ( ( IS_AFFECTED( ch, AFF_HASTE ) && number_percent() < 50 ) || is_affected(ch, 300 ) )
    {
        if( get_eq_char( ch, WEAR_SHIELD ) != NULL && number_percent() < 50 )
            one_hit_shield( ch, victim, dt );
        else if( get_eq_char( ch, WEAR_SECOND ) != NULL && number_percent() < 50 )
        {
            if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
                one_hit( ch, victim, dt, TRUE );
            else
            {
                for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                {
                    if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                    {
                        print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                        print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                        sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                        act( msg, ch, NULL, victim, TO_NOTVICT );
                    }
                }
                act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
            }
        }
        else
        {
            if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
                one_hit( ch, victim, dt, FALSE );
            else
            {
                for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                {
                    if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                    {
                        print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                        print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                        sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                        act( msg, ch, NULL, victim, TO_NOTVICT );
                    }
                }
                act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
            }
        }
    }

    if ( ch->fighting != victim )
    {
        return;
    }

    if( is_affected( ch, gsn_glorious_impale ) )
    {
        if( !ch->fighting || !IS_AFFECTED( ch->fighting, AFF_PARALYZE ) || !is_affected( ch->fighting, gsn_glorious_impale ) )
        {
            affect_strip( ch, gsn_glorious_impale );
        }
        else
        {
            return;
        }
    }

    /* w sektorach naturalnych druidzi maja mala szanse na dodatkowy atak, na 31 levie +1% szansy */
    if ( ( ch->class == CLASS_DRUID ) && IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_DRUID ))
    {
        if ( number_range(0,1000) < ( ch->level + (ch->level / LEVEL_HERO)*10) )
        {
            send_to_char( "{gCzujesz nag³y przyp³yw si³y i korzystaj±c z niego zadajesz jeszcze jeden cios.{x\n\r", ch );
            if( get_eq_char( ch, WEAR_SECOND ) != NULL && get_eq_char( ch, WEAR_SECOND )->item_type == ITEM_WEAPON && number_percent() < 50 )
            {
                if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
                    one_hit( ch, victim, dt, TRUE );
                else
                {
                    for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                    {
                        if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                        {
                            print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                            print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                            sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                            act( msg, ch, NULL, victim, TO_NOTVICT );
                        }
                    }
                    act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
                }
            }
            else
            {
                if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
                    one_hit( ch, victim, dt, FALSE );
                else
                {
                    for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                    {
                        if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
                        {
                            print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
                            print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                            sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
                            act( msg, ch, NULL, victim, TO_NOTVICT );
                        }
                    }
                    act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
                }
            }
        }
    }

    if ( ch->fighting != victim )
    {
        return;
    }

    DEBUG_INFO( "multi_hit:feat_tarczownik_hit" );

    if ( ch->fighting != victim )
    {
        return;
    }

    DEBUG_INFO( "multi_hit:trick_staff" );
    wield = get_eq_char(ch,WEAR_WIELD);

    if ( wield &&
            wield->item_type == ITEM_WEAPON &&
            wield->value[0] == WEAPON_STAFF &&
            IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ))
        check_trick(ch, victim, SN_TRICK_STAFF_SWIRL );

    DEBUG_INFO( "multi_hit:cleave_hit" );
    // rozp³atanie black knighta
    // tutaj nie ma od trika weapon wrench, bo w takim wypadku atak z cleave nie jest mozliwy
    wield = get_eq_char(ch,WEAR_WIELD);
    if ( wield && IS_SET( ch->fight_data, FIGHT_CLEAVE ) && victim->hit != remembered_hp )
    {
        REMOVE_BIT( ch->fight_data, FIGHT_CLEAVE );
        if ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) &&
                wield->value[0] != WEAPON_DAGGER &&
                wield->value[0] != WEAPON_WHIP &&
                wield->value[0] != WEAPON_MACE &&
                wield->value[0] != WEAPON_STAFF &&
                wield->value[0] != WEAPON_SPEAR &&
                wield->value[0] != WEAPON_FLAIL &&
                wield->value[0] != WEAPON_SHORTSWORD &&
                wield->value[0] != WEAPON_CLAWS )
        {

            cleave_count = 0;

            for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                if ( tch->fighting == ch || ch->fighting == tch )
                    crowd++;

            if( crowd == 1 )
            {
                for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                {
                    if ( tch->fighting == ch && ch->fighting == tch && IS_AFFECTED( ch->fighting, AFF_MIRROR_IMAGE ) && is_affected(ch->fighting, gsn_mirror_image) )
                    {
                        while( ( cleave_count == 0 || cleave_count * 24 < get_skill(ch,gsn_cleave ) ) && is_affected(ch->fighting, gsn_mirror_image) )
                        {
                            switch( dice(1,10))
                            {
                                case 1:
                                    act( "Obracasz siê szybko i wyprowadzasz gwa³towny cios w innego oponenta!", ch, NULL, tch, TO_CHAR );
                                    act( "$n obraca siê szybko i wyprowadza gwa³towny cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
                                    act( "$n obraca siê szybko i wyprowadza gwa³towny cios w innego oponenta!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 2:
                                    act( "Odchylasz siê lekko zarazem atakuj±c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_CHAR );
                                    act( "$n odchyla siê lekko zarazem atakuj±c ciê niespodziewanie!", ch, NULL, tch, TO_VICT );
                                    act( "$n odchyla siê lekko zarazem atakuj±c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 3:
                                    act( "Wyprowadzasz mocny cios w stoj±cego obok przeciwnika.", ch, NULL, tch, TO_CHAR );
                                    act( "$n wyprowadza mocny cios w twoim kierunku.", ch, NULL, tch, TO_VICT );
                                    act( "$n wyprowadza mocny cios w stoj±cego obok przeciwnika.", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 4:
                                    act( "Robisz delikatny zwód i rzucasz siê na innego rywala!", ch, NULL, tch, TO_CHAR );
                                    act( "$n robi delikatny zwód i rzuca siê na ciebie!", ch, NULL, tch, TO_VICT );
                                    act( "$n robi delikatny zwód i rzuca siê na innego rywala!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 5:
                                    act( "Markujesz jedno mocne uderzenie po czym wyprowadzasz rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_CHAR );
                                    act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
                                    act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 6:
                                    act( "Bierzesz szeroki zamach i jednocze¶nie atakujesz innego wroga!", ch, NULL, tch, TO_CHAR );
                                    act( "$n bierze szeroki zamach i jednocze¶nie atakuje ciebie!", ch, NULL, tch, TO_VICT );
                                    act( "$n bierze szeroki zamach i jednocze¶nie atakuje innego wroga!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 7:
                                    act( "Przez chwile obserwujesz sytuacje dooko³a ciebie, a gdy przeciwnicy ustawiaj± siê blisko siebie próbujesz jednym mocnym ciosem zraniæ wiêcej ni¿ jednego.", ch, NULL, tch, TO_CHAR );
                                    act( "$n przez chwile obserwuje sytuacje dooko³a, a gdy $s przeciwnicy ustawiaj± siê blisko siebie wyprowadza jedno zamaszyste ciêcie próbuj±c zraniæ wiêcej ni¿ jednego.", ch, NULL, tch, TO_VICT );
                                    act( "$n przez chwile obserwuje sytuacje dooko³a, a gdy $s przeciwnicy ustawiaj± siê blisko siebie wyprowadza jedno zamaszyste ciêcie próbuj±c zraniæ wiêcej ni¿ jednego.", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 8:
                                    act( "Wykonujesz zamaszysty pó³obrót atakuj±c przy okazji kolejn± ofiarê!", ch, NULL, tch, TO_CHAR );
                                    act( "$n wykonuje zamaszysty pó³obrót atakuj±c ciebie przy okazji!", ch, NULL, tch, TO_VICT );
                                    act( "$n wykonuje zamaszysty pó³obrót atakuj±c przy okazji kolejn± ofiarê!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 9:
                                    act( "Odczekawszy chwilê wykorzystujesz sprzyjaj±cy moment aby za jednym razem zaatakowaæ wiêcej ni¿ jednego nieprzyjaciela!", ch, NULL, tch, TO_CHAR );
                                    act( "$n odczekawszy chwilê wykorzystuje sprzyjaj±cy moment aby za jednym razem zaatakowaæ wiêcej ni¿ jednego nieprzyjaciela!", ch, NULL, tch, TO_VICT );
                                    act( "$n odczekawszy chwilê wykorzystuje sprzyjaj±cy moment aby za jednym razem zaatakowaæ wiêcej ni¿ jednego nieprzyjaciela!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 10:
                                    act( "Robisz zdecydowany zwód markuj±c seriê ciosów w jednego przeciwnika, jednak w ostatniej chwili odskakujesz i atakujesz innego rywala!", ch, NULL, tch, TO_CHAR );
                                    act( "$n robi zdecydowany zwód markuj±c seriê ciosów w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje ciê!", ch, NULL, tch, TO_VICT );
                                    act( "$n robi zdecydowany zwód markuj±c seriê ciosów w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje innego rywala!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                            }

                            act( "Twój cios ze ¶wistem przebija siê przez cia³o $Z, które nie stawi³o ¿adnego oporu.", ch, NULL, tch, TO_CHAR );
                            act( "Cios $z ze ¶wistem przebija siê przez twoje lustrzane odbicie, które znika zaraz potem.", ch, NULL, tch, TO_VICT );
                            act( "Cios $z ze ¶wistem przebija siê przez cia³o $Z, które nie stawi³o ¿adnego oporu.", ch, NULL, tch, TO_NOTVICT );
                            remove_mirror( ch, tch, 1 );

                            ++cleave_count;
                        }
                    }
                }
            }
            else
            {
                for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
                {
                    if ( tch->fighting == ch && ( ch->fighting != tch || ( IS_AFFECTED( ch->fighting, AFF_MIRROR_IMAGE ) && is_affected(ch->fighting, gsn_mirror_image) ) ) )
                    {
                        if( !stop && ( cleave_count == 0 || cleave_count * 24 < get_skill(ch,gsn_cleave ) ) )
                        {
                            switch( dice(1,10))
                            {
                                case 1:
                                    act( "Obracasz siê szybko i wyprowadzasz gwa³towny cios w innego oponenta!", ch, NULL, tch, TO_CHAR );
                                    act( "$n obraca siê szybko i wyprowadza gwa³towny cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
                                    act( "$n obraca siê szybko i wyprowadza gwa³towny cios w innego oponenta!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 2:
                                    act( "Odchylasz siê lekko zarazem atakuj±c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_CHAR );
                                    act( "$n odchyla siê lekko zarazem atakuj±c ciê niespodziewanie!", ch, NULL, tch, TO_VICT );
                                    act( "$n odchyla siê lekko zarazem atakuj±c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 3:
                                    act( "Wyprowadzasz mocny cios w stoj±cego obok przeciwnika.", ch, NULL, tch, TO_CHAR );
                                    act( "$n wyprowadza mocny cios w twoim kierunku.", ch, NULL, tch, TO_VICT );
                                    act( "$n wyprowadza mocny cios w stoj±cego obok przeciwnika.", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 4:
                                    act( "Robisz delikatny zwód i rzucasz siê na innego rywala!", ch, NULL, tch, TO_CHAR );
                                    act( "$n robi delikatny zwód i rzuca siê na ciebie!", ch, NULL, tch, TO_VICT );
                                    act( "$n robi delikatny zwód i rzuca siê na innego rywala!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 5:
                                    act( "Markujesz jedno mocne uderzenie po czym wyprowadzasz rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_CHAR );
                                    act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
                                    act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 6:
                                    act( "Bierzesz szeroki zamach i jednocze¶nie atakujesz innego wroga!", ch, NULL, tch, TO_CHAR );
                                    act( "$n bierze szeroki zamach i jednocze¶nie atakuje ciebie!", ch, NULL, tch, TO_VICT );
                                    act( "$n bierze szeroki zamach i jednocze¶nie atakuje innego wroga!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 7:
                                    act( "Przez chwile obserwujesz sytuacje dooko³a ciebie, a gdy przeciwnicy ustawiaj± siê blisko siebie próbujesz jednym mocnym ciosem zraniæ wiêcej ni¿ jednego.", ch, NULL, tch, TO_CHAR );
                                    act( "$n przez chwile obserwuje sytuacje dooko³a, a gdy $s przeciwnicy ustawiaj± siê blisko siebie wyprowadza jedno zamaszyste ciêcie próbuj±c zraniæ wiêcej ni¿ jednego.", ch, NULL, tch, TO_VICT );
                                    act( "$n przez chwile obserwuje sytuacje dooko³a, a gdy $s przeciwnicy ustawiaj± siê blisko siebie wyprowadza jedno zamaszyste ciêcie próbuj±c zraniæ wiêcej ni¿ jednego.", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 8:
                                    act( "Wykonujesz zamaszysty pó³obrót atakuj±c przy okazji kolejn± ofiarê!", ch, NULL, tch, TO_CHAR );
                                    act( "$n wykonuje zamaszysty pó³obrót atakuj±c ciebie przy okazji!", ch, NULL, tch, TO_VICT );
                                    act( "$n wykonuje zamaszysty pó³obrót atakuj±c przy okazji kolejn± ofiarê!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 9:
                                    act( "Odczekawszy chwilê wykorzystujesz sprzyjaj±cy moment aby za jednym razem zaatakowaæ wiêcej ni¿ jednego nieprzyjaciela!", ch, NULL, tch, TO_CHAR );
                                    act( "$n odczekawszy chwilê wykorzystuje sprzyjaj±cy moment aby za jednym razem zaatakowaæ wiêcej ni¿ jednego nieprzyjaciela!", ch, NULL, tch, TO_VICT );
                                    act( "$n odczekawszy chwilê wykorzystuje sprzyjaj±cy moment aby za jednym razem zaatakowaæ wiêcej ni¿ jednego nieprzyjaciela!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                                case 10:
                                    act( "Robisz zdecydowany zwód markuj±c seriê ciosów w jednego przeciwnika, jednak w ostatniej chwili odskakujesz i atakujesz innego rywala!", ch, NULL, tch, TO_CHAR );
                                    act( "$n robi zdecydowany zwód markuj±c seriê ciosów w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje ciê!", ch, NULL, tch, TO_VICT );
                                    act( "$n robi zdecydowany zwód markuj±c seriê ciosów w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje innego rywala!", ch, NULL, tch, TO_NOTVICT );
                                    break;
                            }

                            remembered_hp = tch->hit;
                            if( tch != ch->fighting )
                            {
                                if( check_trick( ch, tch, SN_TRICK_THOUSANDSLAYER ) )
                                {
                                    SET_BIT( ch->fight_data, FIGHT_THOUSANDSLAYER );
                                    one_hit( ch, tch, gsn_cleave, FALSE );
                                }
                                else
                                    one_hit( ch, tch, gsn_cleave, FALSE );
                            }
                            else
                            {
                                act( "Twój cios ze ¶wistem przebija siê przez cia³o $Z, które nie stawi³o ¿adnego oporu.", ch, NULL, tch, TO_CHAR );
                                act( "Cios $z ze ¶wistem przebija siê przez twoje lustrzane odbicie, które znika zaraz potem.", ch, NULL, tch, TO_VICT );
                                act( "Cios $z ze ¶wistem przebija siê przez cia³o $Z, które nie stawi³o ¿adnego oporu.", ch, NULL, tch, TO_NOTVICT );
                                remove_mirror( ch, tch, 1 );
                                remembered_hp -= 1;//by nie przerwalo
                            }
                            ++cleave_count;
                            if( remembered_hp == tch->hit )
                            {
                                stop = TRUE;
                            }

                        }
                    }
                }
                if( IS_SET( ch->fight_data, FIGHT_THOUSANDSLAYER ) )
                {
                    act( "Na zakoñczenie zadajesz jeszcze jeden cios swojemu przeciwnikowi.", ch, NULL, victim, TO_CHAR );
                    act( "Na zakoñczenie $n zadaje ci jeszcze jeden cios.", ch, NULL, victim, TO_VICT );
                    act( "Na zakoñczenie $n zadaje $Z jeszcze jeden cios.", ch, NULL, victim, TO_NOTVICT );
                    one_hit( ch, victim, gsn_cleave, FALSE );
                    REMOVE_BIT( ch->fight_data, FIGHT_THOUSANDSLAYER );
                }
            }
        }
    }

    return;
}

/* procedure for all mobile attacks */
void mob_hit ( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    AFFECT_DATA *abide, *abide2;
    CHAR_DATA * vch, *vch_next, *tch;
    bool slowed = FALSE, stop = FALSE;
    char *msg = (char*)calloc(512, sizeof(char));
    OBJ_DATA *weapon = NULL, *wield = NULL;
    int remembered_hp = victim->hit, vertical_skill, cleave_count, crowd = 0;

    DEBUG_INFO( "mob_hit:zero_attack" );
    if ( EXT_IS_SET( ch->off_flags, OFF_ZERO_ATTACK ) )
    {
        return;
    }

    DEBUG_INFO( "mob_hit:abide:1" );
    abide = affect_find( ch->affected, gsn_weapon_wrench );

	DEBUG_INFO( "mob_hit:area_attack" );
	/* Area attack -- BALLS nasty! */
	/*****************************************/
	if ( EXT_IS_SET( ch->off_flags, OFF_AREA_ATTACK ) )
	{
		for ( vch = ch->in_room->people; vch != NULL; vch = vch_next )
		{
			vch_next = vch->next_in_room;
			if ( ( vch != ch && vch->fighting == ch ) )
            {
				one_hit( ch, vch, dt , FALSE );
            }
		}
	}
	else
	{
		if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
        {
			one_hit( ch, victim, dt, FALSE );
        }
		else
		{
			for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
			{
				if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
				{
					print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
					print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
					sprintf( msg, "%s próbuje zdaæ ci cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
					act( msg, ch, NULL, victim, TO_NOTVICT );
				}
			}
			act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
		}
	}

	if ( ch->fighting != victim )
    {
		return;
    }

	DEBUG_INFO( "mob_hit:second_weapon" );
	/* druga broñ */
	if ( ( weapon = get_eq_char ( ch, WEAR_SECOND ) ) != NULL &&
			( EXT_IS_SET( ch->act, ACT_WARRIOR ) ||
			  EXT_IS_SET( ch->act, ACT_PALADIN ) ||
			  EXT_IS_SET( ch->act, ACT_BARBARIAN ) ||
			  EXT_IS_SET( ch->act, ACT_BARD ) ||
			  EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) ||
           EXT_IS_SET( ch->act, ACT_SHAMAN ) ||
			  EXT_IS_SET( ch->act, ACT_THIEF ) ) )
	{
		if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
        {
			one_hit( ch, victim, dt, TRUE );
        }
		else
		{
			for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
			{
				if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
				{
					print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
					print_char( victim, "%s próbuje zadaæ cios cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
					sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
					act( msg, ch, NULL, victim, TO_NOTVICT );
				}
			}
			act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
		}
	}

	if ( ch->fighting != victim )
    {
		return;
    }

	wield = get_eq_char(ch,WEAR_WIELD);
	//i tutaj vertical slash black knighta. Wczesniejszy atak musial trafic
	if ( wield && IS_SET( ch->fight_data, FIGHT_VERTICAL_SLASH ) && victim->hit != remembered_hp )
	{
		REMOVE_BIT( ch->fight_data, FIGHT_VERTICAL_SLASH );
		if ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) &&
				wield->value[0] != WEAPON_DAGGER &&
				wield->value[0] != WEAPON_WHIP &&
				wield->value[0] != WEAPON_MACE &&
				wield->value[0] != WEAPON_STAFF &&
				wield->value[0] != WEAPON_SPEAR &&
				wield->value[0] != WEAPON_FLAIL &&
				wield->value[0] != WEAPON_SHORTSWORD &&
				wield->value[0] != WEAPON_CLAWS )
		{
			vertical_skill = get_skill( ch, gsn_vertical_slash );
			vertical_skill += (get_curr_stat_deprecated(ch,STAT_DEX)-10)*2;
			vertical_skill -= (get_curr_stat_deprecated(victim,STAT_DEX)-10)*2;
			if ( IS_AFFECTED( ch, AFF_SLOW ) ) vertical_skill /= 4;
			if ( IS_AFFECTED( ch, AFF_HASTE ) )	vertical_skill += 10;

			switch ( victim->size )//dla size tiny i giant nigdy nie wejdzie
			{
				case SIZE_SMALL:
					vertical_skill -= 10;
					break;
				case SIZE_LARGE:
					vertical_skill -= 10;
					break;
				case SIZE_HUGE:
					vertical_skill -= 25;
					break;
				default:
					break;
			}

			if( victim->size == SIZE_TINY )
			{
				if( victim->sex == 2 )
					act( "$N jest za ma³a, by próbowaæ na niej takich sztuczek.", ch, NULL, victim, TO_CHAR );
				else
					act( "$N jest za ma³y, by próbowaæ na nim takich sztuczek.", ch, NULL, victim, TO_CHAR );
			}
			else if ( victim->size == SIZE_GIANT )
			{
				if( victim->sex == 2 )
					act( "$N jest zbyt wielka, by próbowaæ na niej takich sztuczek.", ch, NULL, victim, TO_CHAR );
				else
					act( "$N jest zbyt wielki, by próbowaæ na nim takich sztuczek.", ch, NULL, victim, TO_CHAR );
			}
			else if ( number_percent() < vertical_skill || !can_move(victim))
			{
				switch( number_range(1,5))
				{
					case 1:
						act( "Po zadaniu ciosu czujesz, ¿e lekko oszo³omiony przeciwnik nie jest gotowy na ¿adn± obronê, co w sposób bezwzglêdny wykorzystujesz wyprowadzaj±c natychmiast kolejne ciêcie, tym razem pionowo z do³u w górê.", ch, wield, victim, TO_CHAR );
						act( "Cios $z lekko ciê oszo³omi³, co $e w sposób bezwzglêdny wykorzystuje wyprowadzaj±c natychmiast kolejne ciêcie, tym razem pionowo z do³u w górê.", ch, wield, victim, TO_VICT );
						act( "Cios $z lekko oszo³omi³ $C, co zostaje w sposób bezwzglêdny wykorzystane wyprowadzeniem natychmiast kolejnego ciêcie, tym razem pionowo z do³u w górê.", ch, wield, victim, TO_NOTVICT );
						break;
					case 2:
						act( "Poprawiasz szybko swój chwyt na rêkoje¶ci $f i momentalnie tniesz ponownie z do³u w górê.", ch, wield, victim, TO_CHAR );
						act( "$n poprawia szybko swój chwyt na rêkoje¶ci $f i momentalnie tnie ciê ponownie z do³u w górê.", ch, wield, victim, TO_VICT );
						act( "$n poprawia szybko swój chwyt na rêkoje¶ci $f i momentalnie tnie ponownie z do³u w górê.", ch, wield, victim, TO_NOTVICT );
						break;
					case 3:
						act( "Wykorzystuj±c impet pierwszego ciosu i natychmiast tniesz po tej samej lini co poprzednio prowadz±c ostrze swojej broni ku górze.", ch, wield, victim, TO_CHAR );
						act( "$n wykorzystuj±c impet pierwszego ciosu natychmiast tnie ciê po tej samej lini co poprzednio prowadz±c ostrze swojej broni ku górze.", ch, wield, victim, TO_VICT );
						act( "$n wykorzystuj±c impet pierwszego ciosu natychmiast tnie po tej samej lini co poprzednio prowadz±c ostrze swojej broni ku górze.", ch, wield, victim, TO_NOTVICT );
						break;
					case 4:
						act( "Zaskoczywszy przeciwnika zupe³nie pierwszym ciosem z góry do do³u, wyprowadzasz kolejne potê¿ne ciêcie w lini pionowej w odwrotnym kierunku.", ch, wield, victim, TO_CHAR );
						act( "Zaskoczywszy ciê zupe³nie pierwszym ciosem z góry do do³u, $n wyprowadza kolejne potê¿ne ciêcie w lini pionowej w odwrotnym kierunku.", ch, wield, victim, TO_VICT );
						act( "Zaskoczywszy $C zupe³nie pierwszym ciosem z góry do do³u, $n wyprowadza kolejne potê¿ne ciêcie w lini pionowej w odwrotnym kierunku.", ch, wield, victim, TO_NOTVICT );
						break;
					case 5:
						act( "Udaje ci siê po mistrzowsku wykorzystaæ sprzyjaj±c± sytuacjê po zadaniu pierwszego ciosu aby w ¶lad za nim uderzyæ po raz kolejny tn±c wroga od stóp do g³owy.", ch, wield, victim, TO_CHAR );
						act( "$x udaje siê wykorzystaæ sprzyjaj±c± sytuacjê po zadaniu pierwszego ciosu aby w ¶lad za nim uderzyæ ciê po raz kolejny tn±c od stóp do g³owy.", ch, wield, victim, TO_VICT );
						act( "$x udaje siê wykorzystaæ sprzyjaj±c± sytuacjê po zadaniu pierwszego ciosu aby w ¶lad za nim uderzyæ $C po raz kolejny tn±c od stóp do g³owy.", ch, wield, victim, TO_NOTVICT );
						break;
				}

				one_hit( ch, victim, gsn_vertical_slash , FALSE );
				check_improve( ch, victim, gsn_vertical_slash, TRUE, 50 );
			}
			else if( number_percent() > URANGE( 50, get_curr_stat_deprecated(ch,STAT_DEX)*4 + vertical_skill/3, 95) )
			{
				switch( number_range(1,5))
				{
					case 1:
						act( "Zaatakowawszy ponownie po wyprowadzeniu pierwszego ciosu stwierdzasz ze zgroz±, ¿e przeciwnik w ostatniej chwili zd±zy³ siê uchyliæ. Twoja broñ tnie powietrze, a ty sa<&m/ma/mo> lecisz do przodu na spotkanie gruntu.", ch, wield, victim, TO_CHAR );
						act( "Wykonujesz g³adki unik przed niezgrabnym atakiem $z, $s broñ tnie powietrze i leci $e do przodu na spotkanie gruntu.", ch, wield, victim, TO_VICT );
						act( "$N wykonuje g³adki unik przed niezgrabnym atakiem $z, $s broñ tnie powietrze i leci $e do przodu na spotkanie gruntu.", ch, wield, victim, TO_NOTVICT );
						break;
					case 2:
						act( "Próbujesz zmieniæ chwyt na rêkoje¶ci swej broni, jednak robisz to tak nieudolnie, ¿e wypada ci ona z r±k. Szybko próbujesz naprawiæ sytuacje i przerywasz p³ynny ruch, który wykonywa³<&e¶/a¶/o¶>, dziêki czemu udaje ci siê natychmiast z³apaæ $h, ale kosztem utraty równowagi.", ch, wield, victim, TO_CHAR );
						act( "$n próbuje zmieniæ chwyt na rêkoje¶ci swej broni, jednak robi to tak nieudolnie, ¿e wypada ona z $s r±k. Szybko próbuje $e naprawiæ sytuacje i przerywa p³ynny ruch, który mia³ ci pewnie zrobiæ krzywdê. Udaje siê $o natychmiast z³apaæ $h, ale kosztem utraty równowagi.", ch, wield, victim, TO_VICT );
						act( "$n próbuje zmieniæ chwyt na rêkoje¶ci swej broni, jednak robi to tak nieudolnie, ¿e wypada ona z $s r±k. Szybko próbuje $e naprawiæ sytuacje i przerywa p³ynny ruch, który mia³ pewnie zrobiæ krzywdê $X. Udaje siê $o natychmiast z³apaæ $h, ale kosztem utraty równowagi.", ch, wield, victim, TO_NOTVICT );
						break;
					case 3:
						act( "Po udanym pierwszym ciosie wydawa³o ci siê, ¿e zadanie kolejnego momentalnie po pierwszym nie bêdzie trudne. Sztuka ta jednak ciê przeros³a i w trakcie zamachu straci³<&e¶/a¶/o¶> równowagê wywracaj±c siê na ziemie.", ch, wield, victim, TO_CHAR );
						act( "Po udanym pierwszym ciosie $x wydawa³o siê, ¿e zadanie ci kolejnego momentalnie po pierwszym nie bêdzie trudne. Na szczê¶cie sztuka ta przeros³a $s umiejêtno¶ci i w trakcie zamachu równowaga zosta³a stracona, przez co $n le¿y teraz na ziemi.", ch, wield, victim, TO_VICT );
						act( "Po udanym pierwszym ciosie $x wydawa³o siê, ¿e zadanie kolejnego momentalnie po pierwszym nie bêdzie trudne. Sztuka ta przeros³a jednak $s umiejêtno¶ci i w trakcie zamachu równowaga zosta³a stracona, przez co $n le¿y teraz na ziemi.", ch, wield, victim, TO_NOTVICT );
						break;
					case 4:
						act( "Kiedy twoje pierwsze ciêcie wyprowadzone znad g³owy przynios³o wyj±tkowo dobry efekt próbujesz powtórzyæ cios, tym razem bior±c zamach od do³u. Pechowo jednak zahaczasz koñcem $f o pod³o¿e, co ca³kowicie wyprowadza ciê z rytmu i równowagi. Upadasz na ziemiê.", ch, wield, victim, TO_CHAR );
						if( ch->sex == 2 )
						{
							act( "$n próbuje powtórzyæ cios, tym razem bior±c zamach od do³u. Na szczê¶cie jednak zahacza koñcem $f o pod³o¿e, co ca³kowicie wyprowadza j± z rytmu i równowagi. Upada ona na ziemiê.", ch, wield, victim, TO_VICT );
							act( "$n próbuje powtórzyæ cios, tym razem bior±c zamach od do³u, zahacza jednak koñcem $f o pod³o¿e, co ca³kowicie wyprowadza j± z rytmu i równowagi. Upada ona na ziemiê.", ch, wield, victim, TO_NOTVICT );
						}
						else
						{
							act( "$n próbuje powtórzyæ cios, tym razem bior±c zamach od do³u. Na szczê¶cie jednak zahacza koñcem $f o pod³o¿e, co ca³kowicie wyprowadza go z rytmu i równowagi. Upada on na ziemiê.", ch, wield, victim, TO_VICT );
							act( "$n próbuje powtórzyæ cios, tym razem bior±c zamach od do³u, zahacza jednak koñcem $f o pod³o¿e, co ca³kowicie wyprowadza go z rytmu i równowagi. Upada on na ziemiê.", ch, wield, victim, TO_NOTVICT );
						}
						break;
					case 5:
						act( "Zadawszy pierwszy cios wpadasz w dziki sza³, istny amok, i nie my¶l±c zbyt du¿o natychmiast chcesz powtórzyæ poprzedni wyczyn, jednak w swym zapamiêtaniu wypadasz z rytmu i ostatecznie walisz siê na ziemiê jak d³ug<&i/a/ie>.", ch, wield, victim, TO_CHAR );
						act( "Zadawszy pierwszy cios $n wpada w dziki sza³, istny amok, i nie my¶l±c zbyt du¿o natychmiast chce powtórzyæ poprzedni wyczyn, na szczê¶cie w swym zapamiêtaniu wypada z rytmu i ostatecznie wali siê na ziemiê.", ch, wield, victim, TO_VICT );
						act( "Zadawszy pierwszy cios $n wpada w dziki sza³, istny amok, i nie my¶l±c zbyt du¿o natychmiast chce powtórzyæ poprzedni wyczyn, jednak w swym zapamiêtaniu wypada z rytmu i ostatecznie wali siê na ziemiê.", ch, wield, victim, TO_NOTVICT );
						break;
				}

				if ( ch->position > POS_SITTING )
					ch->position = POS_SITTING;
				check_improve( ch, victim, gsn_vertical_slash, FALSE, 50 );
			}
			else
			{
				switch( number_range(1,5))
				{
					case 1:
						act( "Przez moment wydaje ci siê, ¿e przeciwnik nie zd±zy³ pozbieraæ siê po pierwszym ciosie i próbujesz wyprowadziæ natychmiast kolejny, $E jednak uchyla siê w ostatniej chwili.", ch, wield, victim, TO_CHAR );
						act( "$x przez moment wydaje siê, ¿e nie zd±zy³<&e¶/a¶/o¶> pozbieraæ siê po pierwszym ciosie i próbuje wyprowadziæ natychmiast kolejny, ty jednak uchylasz siê bez k³opotu.", ch, wield, victim, TO_VICT );
						act( "$n próbuje wyprowadziæ natychmiast kolejny cios, $N jednak uchyla siê bez k³opotu.", ch, wield, victim, TO_NOTVICT );
						break;
					case 2:
						act( "Przepe³nion<&y/a/e> dzikim sza³em po perfekcyjnym pierwszym ciêciu w euforii starasz siê bezzw³ocznie zaatakowaæ po raz kolejny, jednak pud³ujesz haniebnie.", ch, wield, victim, TO_CHAR );
						act( "Po perfekcyjnym pierwszym ciêciu $n w euforii stara siê bezzw³ocznie zaatakowaæ ciê po raz kolejny, jednak pud³uje haniebnie.", ch, wield, victim, TO_VICT );
						act( "Po perfekcyjnym pierwszym ciêciu $n w euforii stara siê bezzw³ocznie zaatakowaæ $C po raz kolejny, jednak pud³uje haniebnie.", ch, wield, victim, TO_NOTVICT );
						break;
					case 3:
						act( "Wyprowadziwszy silne ciêcie znad g³owy starasz siê poprawiæ swój chwyt na rêkoje¶ci $f, ta jednak wymyka ci siê na chwilê z r±k i tracisz kilka chwil, które mog³<&e¶/a¶/o¶> wykorzystaæ na zadanie kolejnego ataku.", ch, wield, victim, TO_CHAR );
						act( "$n wyprowadziwszy silne ciêcie znad g³owy stara siê poprawiæ swój chwyt na rêkoje¶ci $f, ta jednak wymyka siê na chwilê z $s r±k i traci $e kilka chwil, które mog³y zostaæ wykorzystaæ na zadanie ci kolejnego ciosu.", ch, wield, victim, TO_VICT );
						act( "$n wyprowadziwszy silne ciêcie znad g³owy stara siê poprawiæ swój chwyt na rêkoje¶ci $f, ta jednak wymyka siê na chwilê z $s r±k i traci $e kilka chwil, które mog³y zostaæ wykorzystaæ na zadanie $X kolejnego ciosu.", ch, wield, victim, TO_NOTVICT );
						break;
					case 4:
						act( "Starasz siê wyprowadziæ kolejny atak po zadaniu pierwszego, jednak przeciwnik przewidzia³ twoje zamiary i w sprytny sposób odskakuje lekko na bok.", ch, wield, victim, TO_CHAR );
						if( ch->sex != 2 )
						{
							act( "$n stara³ siê wyprowadziæ kolejny atak po zadaniu pierwszego, jednak przewidzia³<&e¶/a¶/o¶> $s zamiary i w sprytny sposób odskakujesz lekko na bok.", ch, wield, victim, TO_VICT );
							act( "$n stara³ siê wyprowadziæ kolejny atak po zadaniu pierwszego, jednak $N w sprytny sposób odskakuje lekko na bok.", ch, wield, victim, TO_NOTVICT );
						}
						else
						{
							act( "$n stara³a siê wyprowadziæ kolejny atak po zadaniu pierwszego, jednak przewidzia³<&e¶/a¶/o¶> $s zamiary i w sprytny sposób odskakujesz lekko na bok.", ch, wield, victim, TO_VICT );
							act( "$n stara³a siê wyprowadziæ kolejny atak po zadaniu pierwszego, jednak $N w sprytny sposób odskakuje lekko na bok.", ch, wield, victim, TO_NOTVICT );
						}
						break;
					case 5:
						act( "Po wyprowadzeniu ciosu masz ochotê ponowiæ atak, jednak wahasz siê o jedn± chwilê za d³ugo i sprzyjaj±ca sytuacja mija.", ch, wield, victim, TO_CHAR );
						act( "Po wyprowadzeniu ciosu $n ma ochotê ponowiæ atak, jednak waha siê o jedn± chwilê za d³ugo i sprzyjaj±ca sytuacja mija.", ch, wield, victim, TO_VICT );
						act( "Po wyprowadzeniu ciosu $n ma ochotê ponowiæ atak, jednak waha siê o jedn± chwilê za d³ugo i sprzyjaj±ca sytuacja mija.", ch, wield, victim, TO_NOTVICT );
						break;
				}
				check_improve( ch, victim, gsn_vertical_slash, FALSE, 45 );
			}
		}
	}

	if ( ch->fighting != victim )
		return;

	DEBUG_INFO( "mob_hit:berserk" );
	if ( is_affected( ch, gsn_berserk ) && number_percent() < 40 )
	{
		if( get_eq_char( ch, WEAR_SECOND ) != NULL && number_percent() < 50 )
		{
			if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
				one_hit( ch, victim, dt, TRUE );
			else
			{
				for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
					{
						print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
						print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						act( msg, ch, NULL, victim, TO_NOTVICT );
					}
				}
				act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
			}
		}
		else
		{
			if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
				one_hit( ch, victim, dt, FALSE );
			else
			{
				for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
					{
						print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
						print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						act( msg, ch, NULL, victim, TO_NOTVICT );
					}
				}
				act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
			}
		}
	}

	if ( is_affected( ch, gsn_wardance ) && number_percent() < 50 )
	{
		if( get_eq_char( ch, WEAR_SECOND ) != NULL && number_percent() < 50 )
		{
			if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
				one_hit( ch, victim, dt, TRUE );
			else
			{
				for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
					{
						print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
						print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						act( msg, ch, NULL, victim, TO_NOTVICT );
					}
				}
				act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
			}
		}
		else
		{
			if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
				one_hit( ch, victim, dt, FALSE );
			else
			{
				for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
					{
						print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
						print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
						act( msg, ch, NULL, victim, TO_NOTVICT );
					}
				}
				act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
			}
		}
	}

	if ( ch->fighting != victim )
		return;

	DEBUG_INFO( "mob_hit:haste/fast/slow" );
	if ( ( IS_AFFECTED( ch, AFF_HASTE ) || EXT_IS_SET( ch->off_flags, OFF_FAST ) ) && !IS_AFFECTED( ch, AFF_SLOW ) )
	{
		if (( !IS_AFFECTED(ch,AFF_CHARM ) || number_percent() < 50 ) || is_affected(ch, 300 ))
		{
			if( get_eq_char( ch, WEAR_SECOND ) != NULL && number_percent() < 50 )
			{
				if( !abide || abide->modifier == 0 || (abide->modifier == 1 && abide->level != 51 ) )
					one_hit( ch, victim, dt, TRUE );
				else
				{
					for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
					{
						if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
						{
							print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
							print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
							sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
							act( msg, ch, NULL, victim, TO_NOTVICT );
						}
					}
					act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_SECOND ), NULL, TO_CHAR );
				}
			}
			else
			{
				if( !abide || abide->modifier == -1 || (abide->modifier == 1 && abide->level == 51 ) )
					one_hit( ch, victim, dt, FALSE );
				else
				{
					for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
					{
						if( (abide2 = affect_find(tch->affected, gsn_weapon_wrench ) ) != NULL && abide2->modifier != 1 && tch != ch )
						{
							print_char( ch, "Próbujesz zadaæ cios %s %s, ale %s owiniêty wokó³ twej broni uniemo¿lwia ci to.\n\r", victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr );
							print_char( victim, "%s próbuje zadaæ ci cios %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.\n\r", ch->name, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
							sprintf( msg, "%s próbuje zadaæ cios %s %s, ale %s owiniêty wokó³ %s broni uniemo¿lwia %s to.", ch->name, victim->name3, ( get_eq_char(ch,WEAR_WIELD) )->name5, ( abide2->modifier == -1 ? get_eq_char( tch, WEAR_SECOND ) : get_eq_char( tch, WEAR_WIELD ) )->short_descr, ch->sex == SEX_FEMALE ? "jej" : "jego", ch->sex == SEX_FEMALE ? "jej" : "mu" );
							act( msg, ch, NULL, victim, TO_NOTVICT );
						}
					}
					act( "Nie jeste¶ w stanie wykonaæ ataku $j.", ch, get_eq_char( ch, WEAR_WIELD ), NULL, TO_CHAR );
				}
			}
		}
	}

	DEBUG_INFO( "mob_hit:razorblade_hands/beast_claws" );
	if ( IS_AFFECTED( ch, AFF_RAZORBLADED ) ||
			IS_AFFECTED( ch, AFF_BEAST_CLAWS ) )
		one_hit( ch, victim, dt , FALSE );

	/* ilosci atakow nie dzialaja jesli uzywa broni - anticharmies stuff */
	if ( weapon )
		return;

	if ( IS_AFFECTED( ch, AFF_SLOW ) )
		slowed = TRUE;

	if ( ch->fighting != victim )
		return;


	DEBUG_INFO( "mob_hit:multi_attacks" );
	if ( EXT_IS_SET( ch->off_flags, OFF_ONE_HALF_ATTACK ) )
	{
		if ( number_percent() < 50 && !slowed )
		{
			one_hit( ch, victim, dt , FALSE );

			if ( ch->fighting != victim )
				return;
		}

	}
	else if ( EXT_IS_SET( ch->off_flags, OFF_TWO_ATTACK ) )
	{
		if ( !slowed || number_percent() < 50 )
		{
			one_hit( ch, victim, dt , FALSE );

			if ( ch->fighting != victim )
				return;
		}
	}
	else if ( EXT_IS_SET( ch->off_flags, OFF_THREE_ATTACK ) )
	{
		if ( !slowed )
		{
			one_hit( ch, victim, dt , FALSE );
			if ( ch->fighting != victim )
				return;
		}
		one_hit( ch, victim, dt , FALSE );
		if ( ch->fighting != victim )
			return;
	}
	else if ( EXT_IS_SET( ch->off_flags, OFF_FOUR_ATTACK ) )
	{
		if ( !slowed )
		{
			one_hit( ch, victim, dt , FALSE );
			if ( ch->fighting != victim )
				return;
		}

		one_hit( ch, victim, dt , FALSE );
		if ( ch->fighting != victim )
			return;

		one_hit( ch, victim, dt , FALSE );
		if ( ch->fighting != victim )
			return;
	}

	if ( ch->fighting != victim )
		return;

	// rozp³atanie black knighta
	wield = get_eq_char(ch,WEAR_WIELD);
	if ( !slowed && wield && IS_SET( ch->fight_data, FIGHT_CLEAVE ) && victim->hit != remembered_hp )
	{
		REMOVE_BIT( ch->fight_data, FIGHT_CLEAVE );
		if ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) &&
				wield->value[0] != WEAPON_DAGGER &&
				wield->value[0] != WEAPON_WHIP &&
				wield->value[0] != WEAPON_MACE &&
				wield->value[0] != WEAPON_STAFF &&
				wield->value[0] != WEAPON_SPEAR &&
				wield->value[0] != WEAPON_FLAIL &&
				wield->value[0] != WEAPON_SHORTSWORD &&
				wield->value[0] != WEAPON_CLAWS
			 )
		{

			cleave_count = 0;

			for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				if ( tch->fighting == ch || ch->fighting == tch )
					crowd++;

			if( crowd == 1 )
			{
				for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if ( tch->fighting == ch && ch->fighting == tch && IS_AFFECTED( ch->fighting, AFF_MIRROR_IMAGE ) && is_affected(ch->fighting, gsn_mirror_image) )
					{
						while( ( cleave_count == 0 || cleave_count * 24 < get_skill(ch,gsn_cleave ) ) && is_affected(ch->fighting, gsn_mirror_image) )
						{
							switch( dice(1,10))
							{
								case 1:
									act( "Obracasz siê szybko i wyprowadzasz gwa³towny cios w innego oponenta!", ch, NULL, tch, TO_CHAR );
									act( "$n obraca siê szybko i wyprowadza gwa³towny cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
									act( "$n obraca siê szybko i wyprowadza gwa³towny cios w innego oponenta!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 2:
									act( "Odchylasz siê lekko zarazem atakuj±c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_CHAR );
									act( "$n odchyla siê lekko zarazem atakuj±c ciê niespodziewanie!", ch, NULL, tch, TO_VICT );
									act( "$n odchyla siê lekko zarazem atakuj±c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 3:
									act( "Wyprowadzasz mocny cios w stoj±cego obok przeciwnika.", ch, NULL, tch, TO_CHAR );
									act( "$n wyprowadza mocny cios w twoim kierunku.", ch, NULL, tch, TO_VICT );
									act( "$n wyprowadza mocny cios w stoj±cego obok przeciwnika.", ch, NULL, tch, TO_NOTVICT );
									break;
								case 4:
									act( "Robisz delikatny zwód i rzucasz siê na innego rywala!", ch, NULL, tch, TO_CHAR );
									act( "$n robi delikatny zwód i rzuca siê na ciebie!", ch, NULL, tch, TO_VICT );
									act( "$n robi delikatny zwód i rzuca siê na innego rywala!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 5:
									act( "Markujesz jedno mocne uderzenie po czym wyprowadzasz rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_CHAR );
									act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
									act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 6:
									act( "Bierzesz szeroki zamach i jednocze¶nie atakujesz innego wroga!", ch, NULL, tch, TO_CHAR );
									act( "$n bierze szeroki zamach i jednocze¶nie atakuje ciebie!", ch, NULL, tch, TO_VICT );
									act( "$n bierze szeroki zamach i jednocze¶nie atakuje innego wroga!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 7:
									act( "Przez chwile obserwujesz sytuacje dooko³a ciebie, a gdy przeciwnicy ustawiaj± siê blisko siebie próbujesz jednym mocnym ciosem zraniæ wiêcej ni¿ jednego.", ch, NULL, tch, TO_CHAR );
									act( "$n przez chwile obserwuje sytuacje dooko³a, a gdy $s przeciwnicy ustawiaj± siê blisko siebie wyprowadza jedno zamaszyste ciêcie próbuj±c zraniæ wiêcej ni¿ jednego.", ch, NULL, tch, TO_VICT );
									act( "$n przez chwile obserwuje sytuacje dooko³a, a gdy $s przeciwnicy ustawiaj± siê blisko siebie wyprowadza jedno zamaszyste ciêcie próbuj±c zraniæ wiêcej ni¿ jednego.", ch, NULL, tch, TO_NOTVICT );
									break;
								case 8:
									act( "Wykonujesz zamaszysty pó³obrót atakuj±c przy okazji kolejn± ofiarê!", ch, NULL, tch, TO_CHAR );
									act( "$n wykonuje zamaszysty pó³obrót atakuj±c ciebie przy okazji!", ch, NULL, tch, TO_VICT );
									act( "$n wykonuje zamaszysty pó³obrót atakuj±c przy okazji kolejn± ofiarê!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 9:
									act( "Odczekawszy chwilê wykorzystujesz sprzyjaj±cy moment aby za jednym razem zaatakowaæ wiêcej ni¿ jednego nieprzyjaciela!", ch, NULL, tch, TO_CHAR );
									act( "$n odczekawszy chwilê wykorzystuje sprzyjaj±cy moment aby za jednym razem zaatakowaæ wiêcej ni¿ jednego nieprzyjaciela!", ch, NULL, tch, TO_VICT );
									act( "$n odczekawszy chwilê wykorzystuje sprzyjaj±cy moment aby za jednym razem zaatakowaæ wiêcej ni¿ jednego nieprzyjaciela!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 10:
									act( "Robisz zdecydowany zwód markuj±c seriê ciosów w jednego przeciwnika, jednak w ostatniej chwili odskakujesz i atakujesz innego rywala!", ch, NULL, tch, TO_CHAR );
									act( "$n robi zdecydowany zwód markuj±c seriê ciosów w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje ciê!", ch, NULL, tch, TO_VICT );
									act( "$n robi zdecydowany zwód markuj±c seriê ciosów w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje innego rywala!", ch, NULL, tch, TO_NOTVICT );
									break;
							}

							act( "Twój cios ze ¶wistem przebija siê przez cia³o $Z, które nie stawi³o ¿adnego oporu.", ch, NULL, tch, TO_CHAR );
							act( "Cios $z ze ¶wistem przebija siê przez twoje lustrzane odbicie, które znika zaraz potem.", ch, NULL, tch, TO_VICT );
							act( "Cios $z ze ¶wistem przebija siê przez cia³o $Z, które nie stawi³o ¿adnego oporu.", ch, NULL, tch, TO_NOTVICT );
							remove_mirror( ch, tch, 1 );

							++cleave_count;
						}
					}
				}
			}
			else
			{
				for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
				{
					if ( tch->fighting == ch && ( ch->fighting != tch || ( IS_AFFECTED( ch->fighting, AFF_MIRROR_IMAGE ) && is_affected(ch->fighting, gsn_mirror_image) ) ) )
					{
						if( !stop && ( cleave_count == 0 || cleave_count * 24 < get_skill(ch,gsn_cleave ) ) )
						{
							switch( dice(1,10))
							{
								case 1:
									act( "Obracasz siê szybko i wyprowadzasz gwa³towny cios w innego oponenta!", ch, NULL, tch, TO_CHAR );
									act( "$n obraca siê szybko i wyprowadza gwa³towny cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
									act( "$n obraca siê szybko i wyprowadza gwa³towny cios w innego oponenta!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 2:
									act( "Odchylasz siê lekko zarazem atakuj±c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_CHAR );
									act( "$n odchyla siê lekko zarazem atakuj±c ciê niespodziewanie!", ch, NULL, tch, TO_VICT );
									act( "$n odchyla siê lekko zarazem atakuj±c niespodziewanie kolejnego wroga!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 3:
									act( "Wyprowadzasz mocny cios w stoj±cego obok przeciwnika.", ch, NULL, tch, TO_CHAR );
									act( "$n wyprowadza mocny cios w twoim kierunku.", ch, NULL, tch, TO_VICT );
									act( "$n wyprowadza mocny cios w stoj±cego obok przeciwnika.", ch, NULL, tch, TO_NOTVICT );
									break;
								case 4:
									act( "Robisz delikatny zwód i rzucasz siê na innego rywala!", ch, NULL, tch, TO_CHAR );
									act( "$n robi delikatny zwód i rzuca siê na ciebie!", ch, NULL, tch, TO_VICT );
									act( "$n robi delikatny zwód i rzuca siê na innego rywala!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 5:
									act( "Markujesz jedno mocne uderzenie po czym wyprowadzasz rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_CHAR );
									act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w twoim kierunku!", ch, NULL, tch, TO_VICT );
									act( "$n markuje jedno mocne uderzenie po czym wyprowadza rzeczywisty cios w innego przeciwnika!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 6:
									act( "Bierzesz szeroki zamach i jednocze¶nie atakujesz innego wroga!", ch, NULL, tch, TO_CHAR );
									act( "$n bierze szeroki zamach i jednocze¶nie atakuje ciebie!", ch, NULL, tch, TO_VICT );
									act( "$n bierze szeroki zamach i jednocze¶nie atakuje innego wroga!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 7:
									act( "Przez chwile obserwujesz sytuacje dooko³a ciebie, a gdy przeciwnicy ustawiaj± siê blisko siebie próbujesz jednym mocnym ciosem zraniæ wiêcej ni¿ jednego.", ch, NULL, tch, TO_CHAR );
									act( "$n przez chwile obserwuje sytuacje dooko³a, a gdy $s przeciwnicy ustawiaj± siê blisko siebie wyprowadza jedno zamaszyste ciêcie próbuj±c zraniæ wiêcej ni¿ jednego.", ch, NULL, tch, TO_VICT );
									act( "$n przez chwile obserwuje sytuacje dooko³a, a gdy $s przeciwnicy ustawiaj± siê blisko siebie wyprowadza jedno zamaszyste ciêcie próbuj±c zraniæ wiêcej ni¿ jednego.", ch, NULL, tch, TO_NOTVICT );
									break;
								case 8:
									act( "Wykonujesz zamaszysty pó³obrót atakuj±c przy okazji kolejn± ofiarê!", ch, NULL, tch, TO_CHAR );
									act( "$n wykonuje zamaszysty pó³obrót atakuj±c ciebie przy okazji!", ch, NULL, tch, TO_VICT );
									act( "$n wykonuje zamaszysty pó³obrót atakuj±c przy okazji kolejn± ofiarê!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 9:
									act( "Odczekawszy chwilê wykorzystujesz sprzyjaj±cy moment aby za jednym razem zaatakowaæ wiêcej ni¿ jednego nieprzyjaciela!", ch, NULL, tch, TO_CHAR );
									act( "$n odczekawszy chwilê wykorzystuje sprzyjaj±cy moment aby za jednym razem zaatakowaæ wiêcej ni¿ jednego nieprzyjaciela!", ch, NULL, tch, TO_VICT );
									act( "$n odczekawszy chwilê wykorzystuje sprzyjaj±cy moment aby za jednym razem zaatakowaæ wiêcej ni¿ jednego nieprzyjaciela!", ch, NULL, tch, TO_NOTVICT );
									break;
								case 10:
									act( "Robisz zdecydowany zwód markuj±c seriê ciosów w jednego przeciwnika, jednak w ostatniej chwili odskakujesz i atakujesz innego rywala!", ch, NULL, tch, TO_CHAR );
									act( "$n robi zdecydowany zwód markuj±c seriê ciosów w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje ciê!", ch, NULL, tch, TO_VICT );
									act( "$n robi zdecydowany zwód markuj±c seriê ciosów w jednego przeciwnika, jednak w ostatniej chwili odskakuje i atakuje innego rywala!", ch, NULL, tch, TO_NOTVICT );
									break;
							}

							remembered_hp = tch->hit;
							if( tch != ch->fighting )
								one_hit( ch, tch, gsn_cleave, FALSE );
							else
							{
								act( "Twój cios ze ¶wistem przebija siê przez cia³o $Z, które nie stawi³o ¿adnego oporu.", ch, NULL, tch, TO_CHAR );
								act( "Cios $z ze ¶wistem przebija siê przez twoje lustrzane odbicie, które znika zaraz potem.", ch, NULL, tch, TO_VICT );
								act( "Cios $z ze ¶wistem przebija siê przez cia³o $Z, które nie stawi³o ¿adnego oporu.", ch, NULL, tch, TO_NOTVICT );
								remove_mirror( ch, tch, 1 );
								remembered_hp -= 1;//by nie przerwalo
							}
							++cleave_count;
							if( remembered_hp == tch->hit ) stop = TRUE;
						}
					}
				}
			}
		}
	}


	return;
}

char specdam_text_ch[MAX_INPUT_LENGTH];
char specdam_text_victim[MAX_INPUT_LENGTH];
char specdam_text_room[MAX_INPUT_LENGTH];
/*
 * Calculating bonus from spec weapon damages
 */
int calculate_spec_damage( CHAR_DATA *ch, CHAR_DATA *vch, OBJ_DATA *weapon, int spec_target )
{
	SPEC_DAMAGE *specdam;
	int bonus = 0;
	bool check1, check2;

	for ( specdam = weapon->spec_dam; specdam; specdam = specdam->next )
	{
	        if ( specdam->target_type != spec_target )
		  continue;
		switch ( specdam->type )
		{
			/*			case SD_TYPE_MAGIC:
						if ( number_percent() <= specdam->chance )
						{
						tmp = check_resist( vch, specdam->param1, specdam->bonus );
						if ( tmp <= 0 )
						continue;
						bonus += tmp;
						switch ( specdam->param1 )
						{
						case DAM_FIRE:
						sprintf( specdam_text_ch,	 "Przypalasz $j cia³o $Z." );
						sprintf( specdam_text_victim, "$n przypala ci skórê $j." );
						sprintf( specdam_text_room,   "$n przypala $j cia³o $Z." );
						break;
						}
						}
						break;*/
			case SD_TYPE_RACE:
				if ( !vch )
					break;
				if ( ( specdam->param1 == SD_PARAM_ANY || GET_RACE(vch) == specdam->param1 ) &&
						( specdam->param2 == SD_PARAM_ANY ||  GET_RACE(ch) == specdam->param2 ) &&
						( number_percent() <= specdam->chance ) )
					bonus += specdam->bonus;
				break;
			case SD_TYPE_ALIGN:
				check1 = check2 = FALSE;
				if ( vch && (( specdam->param1 == SD_PARAM_ANY ) ||
						( specdam->param1 == 0 && IS_EVIL( vch ) ) ||
						( specdam->param1 == 1 && IS_GOOD( vch ) ) ||
						( specdam->param1 == 2 && IS_NEUTRAL( vch ) )) )
					check1 = TRUE;
				if ( ( specdam->param2 == SD_PARAM_ANY ) ||
						( specdam->param2 == 0 && IS_EVIL( ch ) ) ||
						( specdam->param2 == 1 && IS_GOOD( ch ) ) ||
						( specdam->param2 == 2 && IS_NEUTRAL( ch ) ) )
					check2 = TRUE;

				if ( check1 && check2 && number_percent() <= specdam->chance )
					bonus += specdam->bonus;

				break;
			case SD_TYPE_SIZE:
				if ( ( specdam->param1 == SD_PARAM_ANY || ( vch && (vch->size == specdam->param1) ) ) &&
						( specdam->param2 == SD_PARAM_ANY ||  ch->size == specdam->param2 ) &&
						( number_percent() <= specdam->chance ) )
					bonus += specdam->bonus;
				break;
			case SD_TYPE_SELFSTAT:
				if ( num_eval( get_curr_stat_deprecated( ch, specdam->param1 ), specdam->param2, specdam->param3 ) &&
						number_percent() <= specdam->chance )
					bonus += specdam->bonus;
				break;
			case SD_TYPE_TARGETSTAT:
				if ( !vch )
					break;
				if ( num_eval( get_curr_stat_deprecated( vch, specdam->param1 ), specdam->param2, specdam->param3 ) &&
						number_percent() <= specdam->chance )
					bonus += specdam->bonus;
				break;
			case SD_TYPE_CLASS:
				if ( !IS_NPC( ch ) && ch->class == specdam->param1 && (number_percent() <= specdam->chance) )
					bonus += specdam->bonus;
				break;
			case SD_TYPE_ACT:
				if ( vch && IS_NPC( vch ) && ext_flags_same( ext_flags_intersection( vch->act, specdam->ext_param ), specdam->ext_param ) && (number_percent() <= specdam->chance) )
					bonus += specdam->bonus;
				break;
		}
	}
	return bonus;
}

void weapon_dispeller( CHAR_DATA *ch, CHAR_DATA *victim, int base_dam )
{
	AFFECT_DATA * aff;
	AFFECT_DATA * aff_next;
	OBJ_DATA * obj;
	OBJ_DATA * obj_next;
	AFFECT_DATA	* spells_found[ 12 ];
	int spells_found_count = 0, i, gsn;
	bool greater = FALSE;

	i = number_percent();
	if (i > 20)
	{
		return;
	}

	if (i <= 5)
    {
		greater = TRUE;
    }

	if (!greater)
    {
		for ( i = 0; i < 12; i++ )
        {
			spells_found[ i ] = 0;
        }
		for ( aff = victim->affected; aff; aff = aff->next )
		{
			/* tylko te z tablicy spelli, nie wiem czy mo¿e siê zdarzyc inaczej */
			if ( aff->type <= 0 || aff->type >= MAX_SKILL )
				continue;
			/* tylko spelle */
			if ( skill_table[ aff->type ].spell_fun == spell_null )
				continue;

			if ( aff->level > 40 ||
			     aff->type == gsn_energy_drain ||
			     aff->type == gsn_on_smoke ||
			     aff->type == gsn_feeblemind ||
			     aff->type == gsn_bleeding_wound ||
			     aff->type == gsn_life_transfer ||
			     aff->type == gsn_energy_strike ||
			     aff->type == 329 || //razorblade hands
			     aff->type == 331 ) //beast claws
				continue;

			spells_found[ spells_found_count++ ] = aff;

			if ( spells_found_count > 11 )
				break;
		}

		if ( spells_found_count == 0 )
		{
			return ;
		}

		i = number_range( 0, spells_found_count - 1 );

		if ( ch->level + number_range( 1, 20 ) <= 12 + spells_found[ i ] ->level )
		{
			return ;
		}

		gsn = spells_found[ i ] ->type;
		affect_strip( victim, gsn );

		if ( skill_table[ gsn ].msg_off )
		{
			send_to_char( skill_table[ gsn ].msg_off, victim );
			send_to_char( "\n\r", victim );
		}
	}

	else

	{

//	if (greater) {
	    for ( aff = victim->affected; aff; aff = aff_next )
		{
			aff_next = aff->next;


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
				aff->type != gsn_steel_scarfskin &&
				aff->type != gsn_energy_shield )
				continue;

			affect_remove( victim, aff );
			spells_found_count ++;

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
                    FALSE
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

		if (spells_found_count)
		{
			act( "Rozpraszasz wszystkie zaklêcia wp³ywaj±ce na $C.", ch, NULL, victim, TO_CHAR );
			act( "$n rozprasza wszystkie wp³ywaj±ce na ciebie zaklêcia.", ch, NULL, victim, TO_VICT );
			act( "$n rozprasza wszystkie zaklêcia wp³ywaj±ce na $C.", ch, NULL, victim, TO_NOTVICT );
		}
	}
	if ( ( !victim->mount && get_carry_weight( victim ) > can_carry_w( victim ) && IS_AFFECTED( victim, AFF_FLYING ) ) ||
	     ( ( !victim->mount && get_carry_weight( victim ) > can_carry_w( victim ) && IS_AFFECTED( victim, AFF_FLOAT ) ) ) )
	{
		act( "$n pod wp³ywem ciê¿aru opada na ziemiê.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Pod wp³ywem obci±¿enia opadasz na ziemiê.\n\r", victim );
		affect_strip( victim, gsn_fly );
		affect_strip( victim, gsn_float );
	}
	return;
}


/*
 * Hit one guy once.
 */
char fight_text_ch[MAX_INPUT_LENGTH];
char fight_text_victim[MAX_INPUT_LENGTH];
char fight_text_room[MAX_INPUT_LENGTH];
int to_hit_bonus;
int to_dam_bonus;
bool accident = FALSE;
/* do_one_hit*/
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool second )
{
	OBJ_DATA * wield;
	OBJ_DATA * obj, *obj_next, *damned_armor, *damned_weapon;
	CHAR_DATA *tch;
	AFFECT_DATA *paf;
	AFFECT_DATA af;
	int holy_mod1 = 0, holy_mod2 = 0;
	int victim_ac;
	int thac0;
	int dam, base_dam = 0, unarmed_base_dam = 0;
	int diceroll;
	int sn = -1, skill;
	int dam_type;
	bool result = TRUE, immune = FALSE, critical = FALSE, prog_mod = FALSE, dreadful = FALSE, check;
	int twohander_fight_style = 0;
	AFFECT_DATA *scarfskin, *damned, bleeding_wound, *defense_curl;
	int weapon_mastery = 0, overwhelming_skill = 0, over_chance = 0, over_hap;
	OBJ_DATA *body;
	OBJ_DATA *arms;
	OBJ_DATA *legs;
	OBJ_DATA *hands;
	OBJ_DATA *feet;
	OBJ_DATA *head;
	OBJ_DATA *bron;

	DEBUG_INFO( "one_hit:checks" );
	if ( victim == ch || ch == NULL || victim == NULL )
		return;

	if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
		return;

	if ( check_blink( ch, victim, FALSE ) )
	{
		send_to_char( "Próbujesz wyprowadziæ cios, jednak przeskakujesz na chwilkê do innego planu egzystencji i przeciwnik znika ci z oczu.\n\r", ch );
		return;
	}

    if ( !second )
    {
        wield = get_eq_char( ch, WEAR_WIELD );
    }
    else
    {
        wield = get_eq_char( ch, WEAR_SECOND );
    }

	DEBUG_INFO( "onehit:twohander_fighting" );
	if ( (!second && wield && IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS)))
	{
		twohander_fight_style = get_skill( ch, gsn_twohander_fighting );

		if ( twohander_fight_style > 0 )
			check_improve( ch, victim, gsn_twohander_fighting, TRUE, 180 );

		if( get_skill( ch, gsn_two_hands_fighting ) > 0 )
			check_improve( ch, victim, gsn_two_hands_fighting, TRUE, 210 );
	}

	DEBUG_INFO( "onehit:dual_fighting" );
	if ( ( second && wield && get_eq_char( ch, WEAR_WIELD ) ) ||
			( !second && wield && get_eq_char( ch, WEAR_SECOND ) ) )
	{

		if((ch->class == CLASS_WARRIOR ||
					ch->class == CLASS_BARBARIAN ||
					ch->class == CLASS_BARD ||
					ch->class == CLASS_PALADIN ) &&
				get_skill( ch, gsn_two_weapon_fighting ) )
		{
			check_improve( ch, victim, gsn_two_weapon_fighting, TRUE, 210 );
		}
	}

    //uczenie sie i podstawianie masterow
    OBJ_DATA * weapon = NULL;
    if (
           ( !second && ( weapon = get_eq_char( ch, WEAR_WIELD  ) ) ) ||
           (  second && ( weapon = get_eq_char( ch, WEAR_SECOND ) ) )
       )
    {
        switch ( ch->class )
        {
            case CLASS_BLACK_KNIGHT:
                switch ( weapon->value[0] )
                {
                    case WEAPON_AXE:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_axe_mastery, 16 );
                        break;
                    case WEAPON_POLEARM:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_polearm_mastery, 16 );
                        break;
                    case WEAPON_SWORD:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_sword_mastery, 16 );
                        break;
                    case WEAPON_WHIP:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_whip_mastery, 16 );
                        break;
                }
                break;
            case CLASS_PALADIN:
                switch ( weapon->value[0] )
                {
                    case WEAPON_FLAIL:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_flail_mastery, 16 );
                        break;
                    case WEAPON_MACE:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_mace_mastery, 16 );
                        break;
                    case WEAPON_SWORD:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_sword_mastery, 16 );
                        break;
                }
                break;
            case CLASS_THIEF:
                if ( weapon->value[0] == WEAPON_DAGGER )
                {
                    weapon_mastery = one_hit_check_mastery( ch, victim, gsn_dagger_mastery, 16 );
                }
                break;
            case CLASS_WARRIOR:
                switch ( weapon->value[0] )
                {
                    case WEAPON_AXE:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_axe_mastery, 16 );
                        break;
                    case WEAPON_DAGGER:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_dagger_mastery, 16 );
                        break;
                    case WEAPON_FLAIL:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_flail_mastery, 16 );
                        break;
                    case WEAPON_MACE:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_mace_mastery, 16 );
                        break;
                    case WEAPON_POLEARM:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_polearm_mastery, 16 );
                        break;
                    case WEAPON_SHORTSWORD:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_shortsword_mastery, 16 );
                        break;
                    case WEAPON_SPEAR:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_spear_mastery, 16 );
                        break;
                    case WEAPON_STAFF:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_staff_mastery, 16 );
                        break;
                    case WEAPON_SWORD:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_sword_mastery, 16 );
                        break;
                    case WEAPON_WHIP:
                        weapon_mastery = one_hit_check_mastery( ch, victim, gsn_whip_mastery, 16 );
                        break;
                }
                break;
        }
    }

	if ( dt == TYPE_UNDEFINED )
	{
		dt = TYPE_HIT;
		if ( wield != NULL && wield->item_type == ITEM_WEAPON )
			dt += wield->value[ 3 ];
		else
			dt += ch->dam_type;
	}

	if ( dt < TYPE_HIT )
	{
		if ( wield != NULL )
		{
			switch ( wield->value[ 0 ] )
			{
				case WEAPON_SWORD: dam_type = DAM_SLASH;	break;
				case WEAPON_MACE: dam_type = DAM_BASH;	break;
				case WEAPON_DAGGER: dam_type = DAM_PIERCE;	break;
				case WEAPON_AXE: dam_type = DAM_SLASH;	break;
				case WEAPON_SPEAR: dam_type = DAM_PIERCE;	break;
				case WEAPON_FLAIL: dam_type = DAM_BASH;	break;
				case WEAPON_WHIP: dam_type = DAM_SLASH;	break;
				case WEAPON_POLEARM: dam_type = DAM_SLASH;	break;
				case WEAPON_STAFF: dam_type = DAM_BASH;	break;
				case WEAPON_SHORTSWORD: dam_type = DAM_SLASH;	break;
				case WEAPON_CLAWS: dam_type = DAM_SLASH; break;
				default: dam_type = DAM_BASH;	break;
			}
			//dam_type = attack_table[wield->value[3]].damage;
		}
		else
        {
            dam_type = attack_table[ ch->dam_type ].damage;
        }
    }
    else
    {
        dam_type = attack_table[ dt - TYPE_HIT ].damage;
    }

    if ( dam_type == -1 )
    {
        dam_type = DAM_BASH;
    }

	DEBUG_INFO( "onehit:get_weapon_skill" );
	sn = get_weapon_sn( ch, !second );
	skill = get_weapon_skill( ch, sn );


	DEBUG_INFO( "onehit:get_AC" );
	switch ( dam_type )
	{
		case( DAM_PIERCE ) : victim_ac = GET_AC( victim, AC_PIERCE ) / 10;	break;
		case( DAM_BASH ) : victim_ac = GET_AC( victim, AC_BASH ) / 10;	break;
		case( DAM_SLASH ) : victim_ac = GET_AC( victim, AC_SLASH ) / 10;	break;
		default: victim_ac = GET_AC( victim, AC_EXOTIC ) / 10;	break;
	};

#ifdef INFO
    print_char( ch, "victim_ac: %d\n\r", victim_ac );
#endif

    DEBUG_INFO( "onehit:ac_modifiers" );
    if ( !can_see( ch, victim ) )
    {
        victim_ac -= 4;
#ifdef INFO
        print_char( ch, "victim_ac (can_see): %d\n\r", victim_ac );
#endif
    }

	if ( !IS_EVIL(ch) && victim->class == CLASS_BLACK_KNIGHT && ( damned_armor = get_eq_char(victim,WEAR_BODY)) != NULL )
	{
		for ( damned = damned_armor->affected; damned != NULL; damned = damned->next )
			if ( damned->type == gsn_damn_armor )
            {
                victim_ac -= URANGE( 1, damned->level/12 +  get_skill(victim,gsn_damn_armor)/33, 4 );
				break;
			}
	}

	if ( victim->position < POS_FIGHTING )
    {
		victim_ac += 3;
    }

	if ( victim->position < POS_RESTING )
    {
		victim_ac += 6;
    }
#ifdef INFO
    print_char( ch, "victim_ac (position): %d\n\r", victim_ac );
#endif

    if ( IS_SET( victim->fight_data, FIGHT_CASTING ) )
    {
        victim_ac += 3;
#ifdef INFO
        print_char( ch, "victim_ac (FIGHT_CASTING): %d\n\r", victim_ac );
#endif
    }

    if ( IS_SET( victim->fight_data, FIGHT_VERTICAL_SLASH ) )
    {
        victim_ac += 6;
#ifdef INFO
        print_char( ch, "victim_ac (FIGHT_VERTICAL_SLASH): %d\n\r", victim_ac );
#endif
    }

    if ( victim->daze > 0 )
    {
        victim_ac += 4;
#ifdef INFO
        print_char( ch, "victim_ac (daze): %d\n\r", victim_ac );
#endif
    }

    /**
     * aura of protection
     */
    if ( has_aura( victim, FALSE ) )
    {
        if ( get_aura_sn( victim ) == 462 )
        {
            victim_ac += get_aura_modifier( victim );
#ifdef INFO
            print_char( ch, "victim_ac (aura of protection): %d\n\r", victim_ac );
#endif
        }
    }

	DEBUG_INFO( "onehit:compute_thac0" );
	thac0 = compute_tohit( ch, !second );

	if ( wield )
	{
		if ( dt == gsn_charge )
			thac0 -= 2;
		else if ( dt == gsn_backstab )
        {
            thac0 -= URANGE( 4, get_skill(ch,gsn_backstab)/10, 10 );
            /**
             * bonus ze sztyletu z flaga WEAPON_HEARTSEEKER
             */
            if ( !second && IS_SET( wield->value[ 4 ], WEAPON_HEARTSEEKER ) )
            {
                thac0 -= 2;
            }
        }
		else if ( dt == gsn_smite || dt == gsn_smite_good )
			thac0 -= 2;
		else if ( dt == gsn_stun )
			thac0 += 4;
		else if ( dt == gsn_cleave )
			thac0 -= 3;//bo koles nie jest przygotowany na atak z twojej strony
		else if ( dt == gsn_vertical_slash )
			thac0 += 2;
	}
	DEBUG_INFO( "onehit:thac0_conse/desecrate_mod" );
	if (EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE))
	{
		if( IS_EVIL(ch))
			thac0 += number_range(1,3);
		if( IS_GOOD(ch))
			thac0 -= number_range(1,3);
	}

	if (EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE))
	{
		if( IS_EVIL(ch))
			thac0 -= number_range(1,3);
		if( IS_GOOD(ch))
			thac0 += number_range(1,3);
	}

	//premie i kary za rozmiary
	DEBUG_INFO( "onehit:thac0_size_mod" );
	//tu jak jest wiekszy - kara za bicie w malego
	if ( ch->size - victim->size == 1 )
		thac0 += 2;
	else if ( ch->size - victim->size == 2 )
		thac0 += 3;
	else if ( ch->size - victim->size == 3 )
		thac0 += 4;
	else if ( ch->size - victim->size >= 4 )
		thac0 += 5;

	//jak jest mniejszy - poczatkowo premia, ale jak jest za duzy to tez kara
	if ( ch->size - victim->size == -1 )
		thac0 -= 2;
	else if ( ch->size - victim->size == -2 )
		;
	else if ( ch->size - victim->size == -3 )
		thac0 += 2;
	else if ( ch->size - victim->size <= -4 )
		thac0 += 4;

	if ( !IS_EVIL(victim) && ch->class == CLASS_BLACK_KNIGHT && ( damned_weapon = get_eq_char(ch,WEAR_WIELD)) != NULL )
	{
		for ( damned = damned_weapon->affected; damned != NULL; damned = damned->next )
			if ( damned->type == gsn_damn_weapon )
			{
				if ( IS_GOOD( victim ) && !IS_AFFECTED( victim, AFF_PROTECT_EVIL ))
					thac0 -= URANGE( 1, damned->level/10 +  get_skill(victim,gsn_damn_weapon)/24, 6 );
				else
					thac0 -= URANGE( 1, damned->level/12 +  get_skill(victim,gsn_damn_weapon)/33, 4 );
				break;
			}
	}

	//goody w grupce z death knightem z demon aura maja minusy do trafien
	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	{
		if( IS_GOOD(ch) && is_affected(tch,gsn_demon_aura) && is_same_group(ch,tch))
        {
			thac0 += URANGE( 1, get_skill(tch,gsn_demon_aura)/16, 6);
        }
	}
	tch = NULL;

	if ( is_affected(ch,gsn_magic_hands) && !get_eq_char(ch,WEAR_WIELD) )
		thac0 -= URANGE(1, ch->level/5, 5);

	DEBUG_INFO( "onehit:trig_onehit" );
	to_hit_bonus = 0;
	to_dam_bonus = 0;
	fight_text_ch[ 0 ] = '\0';
	fight_text_victim[ 0 ] = '\0';
	fight_text_room[ 0 ] = '\0';

	/*onehit trigger for mobiles*/
	if ( !wield && IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_ONEHIT ) )
		prog_mod = mp_onehit_trigger( ch, victim );
	else if ( wield && HAS_OTRIGGER( wield, TRIG_ONEHIT ) )
		prog_mod = op_onehit_trigger( wield, ch );

	if ( !victim->in_room )
		return;

	//mo¿e nie mieæ ju¿ broni (prog onehit)
    if ( !second )
    {
        wield = get_eq_char( ch, WEAR_WIELD );
    }
    else
    {
        wield = get_eq_char( ch, WEAR_SECOND );
    }

    if ( prog_mod && to_hit_bonus != 0 )
    {
        thac0 -= to_hit_bonus;
    }

	check_defensive_spells( ch, victim );

    if ( ch->position < POS_RESTING || !ch->in_room )
    {
        return;
    }

	while ( ( diceroll = number_bits( 5 ) ) >= 20 );

	/* standardowe rzeczy przeklejone z damage(...)*/

	DEBUG_INFO( "onehit:standard_checks" );
	if ( victim->position == POS_DEAD )
		return;

	if ( victim != ch )
	{
		DEBUG_INFO( "onehit:is_safe" );
		if ( is_safe( ch, victim ) )
			result = FALSE;

		DEBUG_INFO( "onehit:trig_kill" );
		if ( victim->position > POS_STUNNED )
		{
			if ( victim->fighting == NULL )
			{
				set_fighting( victim, ch );

				if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
					mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_KILL );

				if ( !ch->in_room )
					return;

				//mo¿e nie mieæ ju¿ broni (prog kill)
				if ( !second )
					wield = get_eq_char( ch, WEAR_WIELD );
				else
					wield = get_eq_char( ch, WEAR_SECOND );
			}
		}

		DEBUG_INFO( "onehit:set_fighting" );
		if ( ch->position > POS_STUNNED )
		{
			if ( ch->fighting == NULL )
				set_fighting( ch, victim );
		}

		DEBUG_INFO( "onehit:charm_stop_follower" );
		if ( victim->master == ch && !accident )
		{
			if( IS_NPC( victim ) && IS_AFFECTED(victim,AFF_CHARM) )
			{
				act( "Wiê¼ miêdzy tob± a $V zostaje brutalnie przerwana!", ch, NULL, victim, TO_CHAR );
				raw_damage( ch, ch, number_range( ch->level/2, ch->level+ch->level/2 ) );
				if ( ch->position <= POS_STUNNED )
					return;
			}
			stop_follower( victim );
		}

		DEBUG_INFO( "onehit:strip_hide/invis" );
		if ( IS_AFFECTED( ch, AFF_HIDE ) )
			affect_strip( ch, gsn_hide );

		strip_invis( ch, TRUE, TRUE );

		DEBUG_INFO( "onehit:start_hating" );
		if ( IS_NPC( victim ) && !is_fearing( victim, ch ) && !accident && !is_hating( victim, ch ) )
		{
			if ( !EXT_IS_SET( victim->act, ACT_SENTINEL ) )
            {
				start_hunting( victim, ch );
            }
			start_hating( victim, ch );
		}
	}
	/* koniec standardowych rzeczy*/

#ifdef INFO
    print_char( ch, "diceroll: %d\n\r", diceroll );
#endif
    /**
     * aura of precision
     */
    if ( has_aura( ch, FALSE ) )
    {
        if ( get_aura_sn( ch ) == 463 )
        {
            diceroll += get_aura_modifier( ch );
#ifdef INFO
            print_char( ch, "DAM + aura of precision: %d\n\r", diceroll );
#endif
        }
    }

    /**
     * WEAPON_HEARTSEEKER
     */
    DEBUG_INFO( "onehit:WEAPON_HEARTSEEKER" );
    if ( wield && IS_SET( wield->value[ 4 ], WEAPON_HEARTSEEKER ) )
    {
        diceroll += 1;
#ifdef INFO
        print_char( ch, "WEAPON_HEARTSEEKER: %d\n\r", diceroll );
#endif
    }

    /**
     * WEAPON_UNBALANCED
     */
    DEBUG_INFO( "onehit:WEAPON_UNBALANCED" );
    if ( wield && IS_SET( wield->value[ 4 ], WEAPON_UNBALANCED ) )
    {
        if ( number_percent() < 15 )
        {
            diceroll = 0;
        }
#ifdef INFO
        print_char( ch, "WEAPON_UNBALANCED: %d\n\r", diceroll );
#endif
    }

    DEBUG_INFO( "onehit:miss" );
    /*czesc kodu przy nietrafieniu kogostam*/
    if ( ( diceroll == 0 || ( can_move( victim ) && diceroll != 19 && diceroll < thac0 - victim_ac ) ) )
    {
        if ( !special_damage_message( ch, victim, wield, 0, dt, FALSE ) )
        {
            generate_onehit_msg( ch, victim, 0, 0, FALSE );
        }
        if ( sn != -1 )
        {
            check_improve( ch, victim, sn, FALSE, 200 );
        }
        return;
    }
	/* koniec czesci kodu przy nie-trafieniu kogostam */

	DEBUG_INFO( "onehit:trick_weapon_wrench" );
	if( wield &&
		wield->value[0] == WEAPON_WHIP &&
		( get_eq_char( victim, WEAR_WIELD) || get_eq_char( victim, WEAR_SECOND ) ) )
	{
		if( second )
			SET_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );

		if( !is_affected( victim, gsn_weapon_wrench ) && !is_affected( ch, gsn_weapon_wrench ) && check_trick( ch, victim, SN_TRICK_WEAPON_WRENCH ) )
		{
			if( IS_SET( ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
				REMOVE_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
			return;
		}

		if( IS_SET( ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
			REMOVE_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
	}

	DEBUG_INFO( "onehit:mob_hitting_without_weapon" );
	/* mobile wali z reki */
	if ( IS_NPC( ch ) && wield == NULL )
	{
		dam = dice( ch->damage[ DICE_NUMBER ], ch->damage[ DICE_TYPE ] ) + ch->damage[ DICE_BONUS ];

		unarmed_base_dam = dam;

		/* critical mobilesa*/
		if ( diceroll == 19 )
		{
			dam += dice( ch->damage[ DICE_NUMBER ], ch->damage[ DICE_TYPE ] ) + ch->damage[ DICE_BONUS ];
			critical = TRUE;
		}
	}
	else
	{
		DEBUG_INFO( "onehit:hitting_with_weapon" );
		/* jesli konkretna umiejetnosc broni to sprawdzamy czy siê nauczyl czegos*/
		if ( sn != -1 )
        {
			check_improve( ch, victim, sn, TRUE, 200 );
        }

		/* jesli uzywa jakiegos narzedzia*/
		if ( wield != NULL )
		{
			dam = dice( UMIN( wield->value[ 1 ], 8 ), UMIN( wield->value[ 2 ], 20 ) ) + UMIN( wield->value[ 6 ], 6 );
			dam += calculate_spec_damage( ch, victim, wield, SD_TARGET_DAM );
			base_dam = dam;

#ifdef INFO
			print_char( ch, "BASE_DAM:%d\n\r", base_dam );
#endif

			/* sharpness! */
			if ( IS_WEAPON_STAT( wield, WEAPON_SHARP ) && number_percent() <= ( skill / 2 ) )
			{
				dam += dam / 4;
#ifdef INFO
				print_char( ch, "+SHARPNESS: %d\n\r", dam );
#endif
			}

			if ( IS_WEAPON_STAT( wield, WEAPON_VORPAL ) )
			{
				if ( number_percent() <= ( skill / 2 ) )
					dam += dam / 3;
				else
					dam += dam / 4;
#ifdef INFO
				print_char( ch, "+VORPAL: %d\n\r", dam );
#endif
			}

			if ( IS_SET( ch->fight_data, FIGHT_MIGHTY_BLOW ) )
			{
				dam += 4;
				if( number_range(0, 100 - get_skill(ch,gsn_mighty_blow) ) < 5 ) dam += 8; //crit
#ifdef INFO
				print_char( ch, "+MIGHTY_BLOW: %d\n\r", dam );
#endif
				send_to_char( "{5Bierzesz potê¿ny zamach.{x\n\r", ch );
				act( "{5$n z potê¿nym zamachem rzuca siê na ciebie.{x", ch, victim, victim, TO_VICT );
				act( "{5$n z potê¿nym zamachem rzuca siê na $C.{x", ch, victim, victim, TO_NOTVICT );
				REMOVE_BIT( ch->fight_data, FIGHT_MIGHTY_BLOW );
			}
			else if ( IS_SET( ch->fight_data, FIGHT_POWER_STRIKE ) )
			{
				dam += 8;
				if( number_range(0, 100 - get_skill(ch,gsn_power_strike) ) < 5 ) dam += 16; //crit
#ifdef INFO
				print_char( ch, "+POWER_STRIKE: %d\n\r", dam );
#endif
				print_char( ch, "{5Wznosisz %s nad g³owê i wyprowadzasz potê¿ny atak.{x\n\r", wield->name4 );
				act( "{5$n wznosi $h nad g³owê i atakuje ciê z potworn± si³±.{x", ch, wield, victim, TO_VICT );
				act( "{5$n wznosi $h nad g³owê i wyprowadza potê¿ny atak.{x", ch, wield, victim, TO_NOTVICT );
				REMOVE_BIT( ch->fight_data, FIGHT_POWER_STRIKE );
			}
			else if ( IS_SET( ch->fight_data, FIGHT_CRITICAL_STRIKE ) )
			{
				if( wield->value[0] == WEAPON_AXE &&
						4*(ch->height)/3 >= victim->height &&
						IS_SET( victim->parts, PART_HEAD ) &&
						( victim->resists[ RESIST_MAGIC_WEAPON ] == 0 || check_magic_attack( ch, victim, wield ) >= victim->resists[ RESIST_MAGIC_WEAPON ] ) &&
						check_trick( ch, victim, SN_TRICK_DECAPITATION ) )
					return;

				dam += 3;
				if( number_range(0, 100 - get_skill(ch,gsn_critical_strike) ) < 5 ) dam += 6; //crit
				send_to_char( "{5Z dono¶nym okrzykiem atakujesz z ca³ych si³.{x\n\r", ch );
				act( "{5$n z dono¶nym okrzykiem wyprowadza morderczy cios.{x", ch, victim, victim, TO_VICT );
				act( "{5$n z dono¶nym okrzykiem zadaje $X morderczy cios.{x", ch, victim, victim, TO_NOTVICT );
				//poza tym symuluje krytyka
			}
			else if ( IS_SET( ch->fight_data, FIGHT_OVERWHELMING_STRIKE ) && IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS ))
			{
				dam += 3 + get_skill(ch,gsn_overwhelming_strike)/10;
				if( number_range(0, 100 - get_skill(ch,gsn_overwhelming_strike) ) < 5 ) dam += 10; //crit
				//no i tutaj walimy czeka na trik dreadful strike, jak zaskoczy to pomijamy opis ktory jest w funkcji triku
				//ale dam dalej naliczamy, ktory normalnie zadamy, + obrazenia od tormenta + jakis bonusik
				if( !is_undead( victim ) &&
					!is_affected(ch,gsn_torment ) &&
					get_eq_char(ch,WEAR_WIELD) &&
					check_trick( ch, victim, SN_TRICK_DREADFUL_STRIKE ) )
				{
					dreadful = TRUE;
				}
				else
				{
					act( "{5Wk³adaj±c w ten cios wszystkie swe si³y rzucasz siê na $C trzymaj±c $h w morderczym u¶cisku.{x", ch, get_eq_char( ch, WEAR_WIELD ), victim, TO_CHAR );
					act( "{5$n z potworn± si³± naciera na ciebie trzymaj±c $h w morderczym u¶cisku.{x", ch, get_eq_char( ch, WEAR_WIELD ), victim, TO_VICT );
					act( "{5$n z potworn± si³± naciera na $C trzymaj±c $h w morderczym u¶cisku.{x", ch, get_eq_char( ch, WEAR_WIELD ), victim, TO_NOTVICT );
				}
				//poza tym bedzie troche nizej szansa na wywrocenie
			}
			else if ( IS_SET( ch->fight_data, FIGHT_THOUSANDSLAYER ) && IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS ))
			{
				dam += get_skill(ch,gsn_overwhelming_strike)/17.5;
				act( "{5Wk³adaj±c w ten cios wszystkie swe si³y rzucasz siê na $C trzymaj±c $h w morderczym u¶cisku.{x", ch, get_eq_char( ch, WEAR_WIELD ), victim, TO_CHAR );
				act( "{5$n z potworn± si³± naciera na ciebie trzymaj±c $h w morderczym u¶cisku.{x", ch, get_eq_char( ch, WEAR_WIELD ), victim, TO_VICT );
				act( "{5$n z potworn± si³± naciera na $C trzymaj±c $h w morderczym u¶cisku.{x", ch, get_eq_char( ch, WEAR_WIELD ), victim, TO_NOTVICT );
				//poza tym bedzie troche nizej szansa na wywrocenie
				//poza tym symuluje krytyka
			}

			/* critical */
			dam = check_critical_hit( ch, victim, &diceroll, &dt, wield, &base_dam, dam, &twohander_fight_style, &weapon_mastery, &critical );

#ifdef INFO
            if( critical )
            {
                print_char( ch, "CRITICAL: %d\n\r", dam );
            }
#endif

			/* modyfikator obrazen ze wzgledu na uszkodzenie broni */
			//niezbyt pikne
			if ( wield->condition > 89 )
				;
			else if ( wield->condition > 84 )
				dam = UMAX( ( ( 95 * dam ) / 100 ), 1 );
			else if ( wield->condition > 74 )
				dam = UMAX( ( ( 90 * dam ) / 100 ), 1 );
			else if ( wield->condition > 64 )
				dam = UMAX( ( ( 70 * dam ) / 100 ), 1 );
			else if ( wield->condition > 44 )
				dam = UMAX( ( ( 65 * dam ) / 100 ), 1 );
			else
				dam = UMAX( dam / 2, 1 );

#ifdef INFO
			print_char( ch, "DAM -WEAPON_CONDITION: %d\n\r", dam );
#endif
		}
		else
		{
			dam = number_range( 1, 4 );
			base_dam = dam;
			/* damage z reki: 1-4 */
		}
	}

	if ( prog_mod && to_dam_bonus != 0 )
		dam += to_dam_bonus;

	//bonus do walki bronia
	if ( IS_NPC( ch ) && wield )
		dam += ch->weapon_damage_bonus;

	if ( !IS_EVIL(victim) && ch->class == CLASS_BLACK_KNIGHT && ( damned_weapon = get_eq_char(ch,WEAR_WIELD)) != NULL )
	{
		for ( damned = damned_weapon->affected; damned != NULL; damned = damned->next )
		{
			if ( damned->type == gsn_damn_weapon )
			{
				if( IS_GOOD(victim) && !IS_AFFECTED( victim, AFF_PROTECT_EVIL ))
					dam += URANGE( 1, damned->level/10 +  get_skill(victim,gsn_damn_weapon)/24, 6 );
				else
					dam += URANGE( 1, damned->level/12 +  get_skill(victim,gsn_damn_weapon)/33, 4 );
				break;
			}
		}
	}

	//dobzi slabiej bija jak sa w grupce z death knightem z demoniczna aura
	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	{
		if( IS_GOOD(ch) && is_affected(tch,gsn_demon_aura) && is_same_group(ch,tch))
			dam -= URANGE( 1, get_skill(tch,gsn_demon_aura)/24, 4);
	}

	//dodajemy premie za weapon_skille, weapon_mastery i style walki bronia dwureczna (przed beesem)
	dam += GET_SKILL_DAMROLL( ch, wield );

#ifdef INFO
	print_char( ch, "DAM +SKILL_DAMROLL: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit:backstab_mod" );

	/* cios w plecy dam + pierdoly ze specjalnymi rasami*/
	if ( dt == gsn_backstab && wield != NULL )
	{
		dam = get_backstabbed_dam( ch, victim, dam );
	}

	DEBUG_INFO( "onehit:conse/desecrate_mod" );
	if (EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE))
	{
		if( IS_EVIL(ch))
			dam -= number_range(1,3);
		if( IS_GOOD(ch))
			dam += number_range(1,3);
	}

	if (EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE))
	{
		if( IS_EVIL(ch))
			dam += number_range(1,3);
		if( IS_GOOD(ch))
			dam -= number_range(1,3);
	}

	DEBUG_INFO( "onehit:GET_DAMROLL_mod" );
	/* dodajemy reszte dodatkowych obrazen (po beesie) */
	dam += GET_DAMROLL( ch, wield );

#ifdef INFO
	print_char( ch, "DAM +DAMROLL: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit:charge_mod" );
	/* bonus do obrazen: critical + bonus (mnozony damroll) */
	if ( dt == gsn_charge && wield )
	{
		switch ( wield->value[ 0 ] )
		{
			case WEAPON_SWORD:
				dam += ( 20 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			case WEAPON_AXE:
				dam += ( 16 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			case WEAPON_POLEARM:
				dam += ( 21 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			case WEAPON_SPEAR:
				dam += ( 25 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			case WEAPON_MACE:
				dam += ( 15 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			case WEAPON_STAFF:
				dam += ( 10 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			case WEAPON_FLAIL:
				dam += ( 22 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
			default:
				dam += ( 5 + skill / 4 + get_skill( ch, gsn_charge ) ) * dam / 100;
				break;
		}

#ifdef INFO
		print_char( ch, "DAM +CHARGE: %d\n\r", dam );
#endif

	}
	else if ( ( dt == gsn_smite && wield ) || ( dt == gsn_smite_good && wield ) )
		{
	    int level_bonus = 0;
        //send_to_char( "Jestesmy w fight.c. \n\r", ch );
        dam += ( ch->level + UMAX(0,(get_curr_stat_deprecated( ch, STAT_CHA )-15)*4 ));

        //holy_prayer
        if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL ) //jesli ma wlaczonego skilla
        {
            if ( paf->modifier < 30 ) //jesli sie ostatnio modlil
            {
                 if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL ) //jesli znaleziono pule
                 {
                    if((paf->modifier > 50) && (number_percent() < (paf->modifier)/3) ) // sacred
                    {
                    //send_to_char( "{5Wzmocnienie z tytu³u holy prayer.{x\n\r", ch );
                     if ( IS_WEAPON_STAT( wield, WEAPON_FLAMING) || IS_WEAPON_STAT( wield, WEAPON_FROST) || IS_WEAPON_STAT( wield, WEAPON_SHOCKING) || IS_WEAPON_STAT( wield, WEAPON_SACRED ) || EXT_IS_SET(wield->extra_flags, ITEM_NOMAGIC ) || IS_WEAPON_STAT( wield, WEAPON_TOXIC ) )
	                  {

                      }else
                      {
                        //send_to_char( "{5Bron nie ma flagi, mozna dac sacred.{x\n\r", ch );

                            holy_mod1 = paf->modifier/4;
                            holy_mod2 = paf->modifier/4;
                            af.where = TO_WEAPON;
                         	af.type = sn;
                          	af.level = holy_mod1;
                           	af.duration = holy_mod2;
                      //    af.rt_duration = 10;
                            af.rt_duration = 0;
                          	af.location = WEAPON_SACRED;
                          	af.modifier = 0;
                          	af.bitvector = &AFF_NONE;
                         	affect_to_obj( wield, &af );
                            send_to_char( "Twoje ramiê otacza jasna po¶wiata, która nastêpnie wnika w tw± broñ!\n\r", ch );
                            act( "Broñ $z wydaje siê przez chwilê rozb³yskaæ niezwyk³ym ¶wiat³em!", ch, NULL, NULL, TO_ROOM );
                           paf->modifier -= 7;

                      }

                      if (paf->modifier > 30 && number_percent() < paf->modifier/3)
                      {

                        level_bonus = 3;
                        dam += dam/3;
                        send_to_char( "Czujesz jak twe cia³o rozpiera potêga ¶wiat³a, wk³adasz niesamowit± si³ê w cios!\n\r", ch );
                        act( "$n wydaje siê przez chwilê rozb³yskaæ niezwyk³ym ¶wiat³em!", ch, NULL, NULL, TO_ROOM );
                        //send_to_char( "{5Bron ma juz jakas flage.{x\n\r", ch );
                        paf->modifier -= 7;

                      }
                      if (paf->modifier < 0 && number_percent() < (-paf->modifier))
                      {

                        level_bonus = -2;
                        dam -= dam/3;


                      }

                       if (paf->modifier < 0 && number_percent() < (-paf->modifier))
                      {

                        level_bonus = -2;
                        dam -= dam/3;

                      }
                      if(level_bonus < 0)
                      {
                      send_to_char( "Twe nieczyste sumienie przeszkadza ci w zebraniu si³!\n\r", ch );
                      act( "$n wydaje siê wachaæ przy zadaniu ciosu.", ch, NULL, NULL, TO_ROOM );
                      }
                    }
                 }//koniec sprawdzania puli
            }//koniec sprawdzania modlitwy
        }//koniec wlaczonego skilla
        //koniec holy prayera

		//smite moze duzo slabszych dobic jednym strzalem
		//send_to_char( "Sprawdzamy czy mozemy zabic jednym strzalem. \n\r", ch );
        if ( ( victim->level + 10 < ch->level + level_bonus ) && ((victim->hit - dam)*100)/victim->max_hit < UMAX( get_skill(ch,gsn_smite),get_skill(ch,gsn_smite_good) )*( IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS) ? 3 : 2 )/4 )
        {
            dam += victim->hit - dam + 12;
        }


#ifdef INFO
		print_char( ch, "DAM +SMITE: %d\n\r", dam );
#endif
	}

	DEBUG_INFO( "onehit:bashed_mod" );
	/* jesli koles nie stoi */
	if ( victim->position < POS_FIGHTING )
	{
		/* nieprzytomy/spiacy obrazenia razy 2, reszta +25%*/
		if ( !IS_AWAKE( victim ) )
			dam *= 2;
		else
			dam += dam / 4;
#ifdef INFO
		print_char( ch, "DAM + POSITION_BONUS: %d\n\r", dam );
#endif

	}

	DEBUG_INFO( "onehit:palladin_align_mod" );
    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
    {
             holy_mod1 = 0;
             holy_mod2 = 0;
             if(IS_EVIL(victim))
             {
                 if(paf->modifier > 50 && number_percent() < paf->modifier/10)
                 {
                     holy_mod1 += dam/3;
                     if(number_percent() < 40)
                     {
                         paf->modifier -= 1;
                     }
                 }

                 if(paf->modifier > 0 && number_percent() < paf->modifier/5)
                 {
                     holy_mod1 += dam/3;
                     if(number_percent() < 30)
                     {
                         paf->modifier -= 1;
                     }

                 }
             }
             /*
              * rasy zwierzece wykluczamy
              */
             else if ( !IS_SET( race_table[ GET_RACE(victim) ].type, ANIMAL ) )
             {
                 if(paf->modifier < 0 && number_percent() < (-paf->modifier/5))
                 {
                     holy_mod1 -= dam/3;

                 }

                 if(paf->modifier < -50 && number_percent() < (-paf->modifier/5))
                 {
                     holy_mod1 -= dam/3;

                 }
             }

             if(holy_mod1 > 0)
             {
                          switch ( number_range( 1, 9 ) )
					{
						case 1:
							act( "Wznosisz rêke do ciosu i czujesz, ¿e to jest TO, to jest Twoje przeznaczenie! Uderzasz z potworn± si³a.", ch, NULL, victim, TO_CHAR );
							break;
						case 2:
							act( "Czujesz jak twe ramie wype³nia Boska moc, uderzasz $C z ca³ych si³.", ch, NULL, victim, TO_CHAR );
							break;
						case 3:
							act( "Czujesz jak z³o niemal kapie z $Z, pewno¶æ i¿ jest on z³y wype³nia ciê Gniewem Sprawiedliwych.", ch, NULL, victim, TO_CHAR );
							break;
						case 4:
							act( "Masz wizjê p³on±cej wioski w której to niewinnych morduj± takie potwory jak $N! To napawa ciê nadludzk± si³a z która atakujesz $C.", ch, NULL, victim, TO_CHAR );
							break;
						case 5:
							act( "Walcz±c z $V czujesz jak wiele zale¿y od twojego sukcesu, nie zawiedziesz niewinnych, nie porzucisz ¶lubów, nie poddasz tej walki!", ch, NULL, victim, TO_CHAR );
							break;
						case 6:
							act( "Wype³nia ciê niesamowita si³a, walka ze s³ugami Ciemno¶ci to twoja specjalno¶æ!", ch, NULL, victim, TO_CHAR );
							break;
						case 7:
							act( "Wiesz, ¿e tysi±ce ludzi liczy na to, i¿ ty i tobie podobni zwyciê¿ycie. Z pewno¶ci± ich nie zawiedziesz.", ch, NULL, victim, TO_CHAR );
							break;
						case 8:
							act( "Wiedz±c, ¿e $N jest z³y, atakujesz z ca³ych si³.", ch, NULL, victim, TO_CHAR );
							break;
						case 9:
							act( "Czujesz jak Bogwie ¦wiat³o¶ci wype³niaj± ciê wielk± si³±.", ch, NULL, victim, TO_CHAR );
							break;
					}
					switch ( number_range( 1, 6 ) )
					{
						case 1:
							act( "Widzisz jak $n z b³yskiem uniesienia w oczach wyprowadza potê¿ny cios.", ch, NULL, victim, TO_NOTVICT );
							break;
						case 2:
							act( "Nie wiesz co wst±pi³o w $c, ten cios by³ naprawdê potê¿ny.", ch, NULL, victim, TO_NOTVICT );
							break;
						case 3:
							act( "Wydaje ci siê, ¿e dla $z ca³y ¶wiat przesta³ istnieæ, walczy jakby istnia³ tylko $N.", ch, NULL, victim, TO_NOTVICT );
							break;
						case 4:
							act( "Ciosy $z zostawiaj± za sob± ¶wiec±c± smuge a ich si³a jest niewyobra¿alna.", ch, NULL, victim, TO_NOTVICT );
							break;
						case 5:
							act( "Niemal wspó³czujesz $X, gdy $n z natchnionym wyrazem twarzy rzuca sie na $C i atakuje go z potworn± si³±.", ch, NULL, victim, TO_NOTVICT );
							break;
						case 6:
							act( "$n atakuje tak jakby to by³a walka ¿ycia, ciosy s± pewne i bardzo silne.", ch, NULL, victim, TO_NOTVICT );
							break;
					}


             }

             if(holy_mod1 < 0)
             {
               	switch ( number_range( 1, 11 ) )
				{
					case 1:
						act( "Czujesz jak przez umys³ przebiegaj± ci obrazy z ¿ycia legendarnych Paladynów, co sob± reprezentowali, co chcieli zrobiæ, ONI nie atakowaliby $Z!", ch, NULL, victim, TO_CHAR );
						break;
					case 2:
						act( "Gdy wznosisz rêkê do ciosu czujesz siê podle, przecie¿ tylko niegodny cnót Paladyna zbir wznosi rêke na niewinnych!", ch, NULL, victim, TO_CHAR );
						break;
					case 3:
						act( "Tracisz ochotê do walki. Wiesz, ¿e $N nie jest z³y, czujesz to ca³ym swoim cia³em, co ty wogóle wyprawiasz ?!?", ch, NULL, victim, TO_CHAR );
						break;
					case 4:
						act( "Przed oczami staje ci obraz kobiety w b³yszcz±cej zbroi z dwoma ¶wietlistymi m³otami w rêkach. Spogl±da nagle na ciebie, jej oczy zachodz± ³zami i opuszcza g³owê. Czujesz siê naprawde podle.", ch, NULL, victim, TO_CHAR );
						break;
					case 5:
						act( "Twe d³onie dr¿± gdy atakujesz $C z trudem utrzymujesz broñ w rêku.", ch, NULL, victim, TO_CHAR );
						break;
					case 6:
						act( "Targaj± tob± wyrzuty sumienia, jak to siê sta³o, ¿e walczysz z $V ? Z pewno¶ci± mo¿na by³o tego unikn±c!", ch, NULL, victim, TO_CHAR );
						break;
					case 7:
						act( "S³yszysz tysi±ce szeptów, nasilaj± siê w twojej g³owie, ka¿dy z nich krzyczy - Morderca!", ch, NULL, victim, TO_CHAR );
						break;
					case 8:
						act( "Anielski spiew dobiega z oddali, czujesz jak ogarnia ciê fala dobra i mocy. Wznosisz rêkê na $C i nagle spiewy cichn± a moc odp³ywa od ciebie... czujesz siê bardzo ¼le.", ch, NULL, victim, TO_CHAR );
						break;
					case 9:
						act( "Czujesz ¿e walka z $V nie jest dobrym pomys³em, przecie¿ nie do tego ca³e ¿ycie siê przygotowywa³e¶, czy tak postêpuj± stró¿e Dobra i Porz±dku ?!?", ch, NULL, victim, TO_CHAR );
						break;
					case 10:
						act( "Czujesz siê bardzo ¼le walcz±c z $V.", ch, NULL, victim, TO_CHAR );
						break;
					case 11:
						act( "Ca³a ta walka nie ma sensu, na Bogów co ty wyprawiasz?", ch, NULL, victim, TO_CHAR );
						break;
				}
				switch ( number_range( 1, 4 ) )
				{
					case 1:
						act( "Widzisz jak $n wacha siê zadaj±c cios, wyra¼nie nie mo¿e siê skoncentrowaæ.", ch, NULL, victim, TO_NOTVICT );
						break;
					case 2:
						act( "Widzisz jak d³onie $z dr¿± gdy walczy z $V.", ch, NULL, victim, TO_NOTVICT );
						break;
					case 3:
						act( "$n rozgl±da siê dooko³a jakby poszukuj±c wyj¶cia z tej niewygodnej dla siebie walki.", ch, NULL, victim, TO_NOTVICT );
						break;
					case 4:
						act( "Widaæ, ¿e $x ta walka nie odpowiada, najwyra¼niej wstrzymuje sie od ciosu. ", ch, NULL, victim, TO_NOTVICT );
						break;
				}


             }
             dam += holy_mod1;
        }//koniec wplywu holy prayer


#ifdef INFO
	print_char( ch, "DAM + PAL_ALIGN_BONUS: %d\n\r", dam );
#endif

	//rozplatanie - spada sila w kolejnych przeciwnikow
	if( dt == gsn_cleave && !IS_SET( ch->fight_data, FIGHT_THOUSANDSLAYER ))
	{
		dam -= dam/4;
	}

	/* sparowany atak??*/
	if ( dt != gsn_backstab && dt != gsn_charge && !dreadful )
	{
		DEBUG_INFO( "onehit:parry" );
		if ( check_parry( ch, victim, dam, wield ) )
			return;

		DEBUG_INFO( "onehit:dodge" );
		/* po dodge, jest szansa ze cios pojdzie w kogos obok */
		if ( check_dodge( ch, victim, victim_ac , diceroll, thac0, critical ) )
		{
			CHAR_DATA * vch = NULL;

			if ( number_percent() < 15 )
			{
				for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
					if ( vch != ch && vch->fighting == victim )
						break;

				if ( vch )
				{
					accident = TRUE;
					one_hit( ch, vch, dt, second );
					accident = FALSE;
					return;
				}
			}

			return;
		}

		if ( check_shield_block( ch, victim, victim_ac , diceroll, thac0, critical, &dam  ) )
			return;
	}

	/* jesli trafilismy to obrazenia minimum 1*/
	if ( dam < 1 )
    {
		dam = 1;
    }
    /**
     * aura of battle lust
     */
    if ( has_aura( ch, FALSE ) )
    {
        if ( get_aura_sn( ch ) == 465 )
        {
            dam += get_aura_modifier( ch );
#ifdef INFO
            print_char( ch, "DAM + aura of battle lust: %d\n\r", dam );
#endif
        }
    }

    /**
     * ogolnie koniec, teraz obsluga roznych rzeczy,
     * na poczatek zmniejszamy obrazenia wojownika
     */
   /* if ( !IS_NPC( ch ) && ch->class == CLASS_WARRIOR && dam > 3 )
    {
        dam *= 3;
        dam /= 4;
    }
   */

	if ( dam > 70 && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && dt == TYPE_UNDEFINED )
	{
		bugf( "Gracz: %s co¶ podejrzanie mocno bije z %s [%d]!", ch->name,
				( wield ? wield->short_descr : "reki" ),
				( wield ? wield->pIndexData->vnum : 0 ) );
	}

	DEBUG_INFO( "onehit:blink" );
	if ( check_blink( victim, ch, TRUE ) && dt != gsn_bash && !dreadful && dt != gsn_slam )
	{
		act( "Kiedy twój cios ju¿ ma trafiæ w $C, $S sylwetka chwilowo znika.", ch, NULL, victim, TO_CHAR );
		act( "Kiedy cios $z ma ju¿ w ciebie trafiæ znikasz na chwilkê z tego planu egzystencji.", ch, NULL, victim, TO_VICT );
		act( "Kiedy cios $z ma ju¿ trafiæ w $C, $S sylwetka chwilowo znika.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit:mirror_image" );
	if ( !dreadful && dt != gsn_crush && dt != gsn_bash && check_mirror_image( victim, ch ) && dt != gsn_slam )
	{
		print_char( ch, "Twój cios trafia, %s znika.\n\r", PERS( victim, ch ) );
		print_char( victim, "%s trafia jedno z twoich lustrzanych odbiæ.\n\r", PERS( ch, victim ) );
		act( "$n trafia jedno z lustrzanych odbiæ $Z.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit:soul" );
	if ( IS_AFFECTED( victim, AFF_SOUL ) )
	{
		print_char( ch, "Twój cios trafia, uwolniona dusza ulatuje do swojego wymiaru.\n\r", PERS( victim, ch ) );
		print_char( victim, "%s trafia ciê, ulatujesz do swojego wymiaru.\n\r", PERS( ch, victim ) );
		act( "$n trafia, dusza $Z ulatuje do swojego wymiaru.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala dusza opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemiê.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp³ywa siê w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez ¶ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp³ywa siê pozostawiaj±c tylko chmurê dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy siê przez chwilê, po czym rozpryskujê siê na kwa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p pêka z hukiem i rozpryskuje siê dooko³a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje siê na kawa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}
		extract_char( victim, TRUE );
		return;
	}

	DEBUG_INFO( "onehit:steel_scarfskin" );
		if( is_affected( victim, gsn_steel_scarfskin ) && !dreadful &&
				dam <= number_range( 6, UMAX(14,affect_find(victim->affected,gsn_steel_scarfskin)->level ) ) &&
				( !wield || ( wield && !IS_WEAPON_STAT(wield, WEAPON_SHARP) && !IS_WEAPON_STAT(wield, WEAPON_VORPAL) ) ) )
		{
			scarfskin = affect_find( victim->affected, gsn_steel_scarfskin );

			if ( scarfskin->modifier > 0 )
			{
				scarfskin->modifier -= dam;
				dam = 0;

				if ( wield )
				{
					print_char( ch, "Twoja broñ odbija siê jakby¶ uderzy³%s w stal.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
					print_char( victim, "%s trafia cie, jednak %s broñ odskakuje jakby uderzy³%s w stal.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : ch->sex == 0 ? "tego" : "jego", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
					switch ( victim->sex )
					{
						case 0:
							act( "$n trafia $C, jednak $s broñ odskakuje jakby $N by³o ze stali.", ch, NULL, victim, TO_NOTVICT );
							break;
						case 1:
							act( "$n trafia $C, jednak $s broñ odskakuje jakby $N by³ ze stali.", ch, NULL, victim, TO_NOTVICT );
							break;
						default :
							act( "$n trafia $C, jednak $s broñ odskakuje jakby $N by³a ze stali.", ch, NULL, victim, TO_NOTVICT );
							break;
					}
				}
				else
				{
					print_char( ch, "Twój cios jest nieskuteczny, jakby¶ uderzy³%s w stal.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
					print_char( victim, "%s trafia cie, jednak %s cios jest nieskuteczny.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : "jego", ch->sex == 2 ? "uderzy³a" : "uderzy³" );
					act( "$n trafia $C, jednak $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
				}
				return;
			}
		}

	DEBUG_INFO( "onehit:stone_skin" );
	if ( check_stone_skin( ch, victim, dam_type, &dam, wield ) && !dreadful )
	{
		if ( dam == 0 )
		{
			if ( wield )
			{
				print_char( ch, "Twoja broñ odbija siê jakby¶ uderzy³%s w kamieñ.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
				print_char( victim, "%s trafia cie, jednak %s broñ odskakuje jakby uderzy³%s w kamieñ.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : ch->sex == 0 ? "tego" : "jego", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
				switch ( victim->sex )
				{
					case 0:
						act( "$n trafia $C, jednak $s broñ odskakuje jakby $N by³o z kamienia.", ch, NULL, victim, TO_NOTVICT );
						break;
					case 1:
						act( "$n trafia $C, jednak $s broñ odskakuje jakby $N by³ z kamienia.", ch, NULL, victim, TO_NOTVICT );
						break;
					default :
						act( "$n trafia $C, jednak $s broñ odskakuje jakby $N by³a z kamienia.", ch, NULL, victim, TO_NOTVICT );
						break;
				}
			}
			else
			{
				print_char( ch, "Twój cios jest nieskuteczny, jakby¶ uderzy³%s w kamieñ.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
				print_char( victim, "%s trafia cie, jednak %s cios jest nieskuteczny.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : "jego", ch->sex == 2 ? "uderzy³a" : "uderzy³" );
				act( "$n trafia $C, jednak $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
			}
			return;
		}
		else
		{
			if ( wield )
			{
				print_char( ch, "Twoja broñ przebija siê przez kamienn± skorê pokrywaj±c± %s.\n\r", PERS2( victim, ch ) );
				print_char( victim, "Broñ %s przebija siê przez twoj± kamienn± skórê.\n\r", PERS2( ch, victim ) );
				act( "Broñ $z trafia $C przebijaj±c siê przez kammien± skórê.", ch, NULL, victim, TO_NOTVICT );
			}
			else
			{
				print_char( ch, "Twój cios przebija siê przez kamienn± skórê pokrywaj±c± %s.\n\r", PERS4( victim, ch ) );
				print_char( victim, "Cios %s przebija siê przez twoj± kamienn± skórê.\n\r", PERS2( ch, victim ) );
				act( "Cios $z trafia $C przebijaj±c siê przez kamienn± skórê.", ch, NULL, victim, TO_NOTVICT );
			}
		}
	}

	DEBUG_INFO( "onehit:magic_weapon" );
	if ( victim->resists[ RESIST_MAGIC_WEAPON ] > 0 &&
			check_magic_attack( ch, victim, wield ) < victim->resists[ RESIST_MAGIC_WEAPON ] )
	{
		if( check_magic_attack( ch, victim, wield ) == 0 && IS_AFFECTED(victim, AFF_RESIST_MAGIC_WEAPON))
		{
			;
		}
		else
		{
			if( !dreadful )//jak bk udaza z trika dreadful strike to atak jest umagiczniony negativem (zuzywa tormenta)
			{
                if((check_magic_attack( ch, victim, wield ) + 1) == victim->resists[ RESIST_MAGIC_WEAPON ] && check_magic_attack( ch, victim, wield ) > 0)
                {
                  dam /= 3; //Raszer - jesli mamy bron o slabszym o 1 umagicznieniu niz potrzebowalismy na bossa, to zadajemy polowe normalnych obrazen
                }else
                {
				immune = TRUE;
				dam = 0;
                }
			}
		}
	}

	if ( immune || dam == 0 )
	{
		print_char( ch, "Twój atak jest nieskuteczny, cios nic %s nie robi.\n\r", PERS3( victim, ch ) );
		print_char( victim, "Atak %s jest nieskuteczny, %s cios niczego ci nie robi.\n\r", PERS2( ch, victim ), ( ch->sex == 2 ) ? "jej" : "jego" );
		act( "$n trafia, jednak $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit:check_resist" );
	dam = check_resist( victim, dam_type, dam );

#ifdef INFO
	print_char( ch, "DAM -RESISTS: %d\n\r", dam );
#endif

	if ( immune || dam == 0 )
	{
		print_char( ch, "Twój atak jest nieskuteczny, cios nic %s nie robi.\n\r", PERS3( victim, ch ) );
		print_char( victim, "Atak %s jest nieskuteczny, %s cios niczego ci nie robi.\n\r", PERS2( ch, victim ), ch->sex == 2 ? "jej" : "jego" );
		act( "Atak $z nie robi wrazenia na $B, $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );


		if ( wield )
		{
			if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_FLAMING ) )
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
				act( "$N parzy siê przy zetkniêciu z twoja $j.", ch, wield, victim, TO_CHAR );
				act( "$N parzy siê przy zetkniêciu z $j $z.", ch, wield, victim, TO_NOTVICT );
				act( "$p lekko cie przysma¿a.", ch, wield, victim, TO_VICT );
				fire_effect( ( void * ) victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_FIRE, FALSE );
			}

			if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_FROST ) )
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
				act( "$p zamra¿a $c.", victim, wield, NULL, TO_ROOM );
				act( "Zimny dotyk $f zamienia ciê w bry³ê lodu.", ch, wield, victim, TO_VICT );
				cold_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_COLD, FALSE );
			}

			if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_SHOCKING ) )
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
				act( "B³yskawica z $f trafia $c wywo³uj±c u niego niekontrolowane drgawki.", victim, wield, NULL, TO_ROOM );
				act( "$p aplikuje ci seriê elektrowstrzasów.", victim, wield, NULL, TO_CHAR );
				shock_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_LIGHTNING, FALSE );
			}

			if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_TOXIC ) )
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
				act( "Pokrywaj±cy $h kwas wy¿era skórê $z.", victim, wield, NULL, TO_ROOM );
				act( "Pokrywaj±cy $h kwas wy¿era tw± skórê!", victim, wield, NULL, TO_CHAR );
				acid_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_ACID, FALSE );
			}

			if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_RESONANT ) )
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
                if (IS_SET(victim->parts, PART_HEAD ) )
                {
                    act( "Fala ultrad¼wiêków z $f wdziera siê do g³owy $z.", victim, wield, NULL, TO_ROOM );
                    act( "Fala ultrad¼wiêków z $f wdziera siê do twojej g³owy wraz z pulsuj±cym bólem.", victim, wield, NULL, TO_CHAR );
                }
                else
                {
                    act( "Fala ultrad¼wiêków z $f wdziera siê do cia³a $z.", victim, wield, NULL, TO_ROOM );
                    act( "Fala ultrad¼wiêków z $f wdziera siê do twojego cia³a wraz z pulsuj±cym bólem.", victim, wield, NULL, TO_CHAR );
                }
				sound_effect( victim, 1, dam, TARGET_CHAR );
				damage( ch, victim, dam, 0, DAM_SOUND, FALSE );
			}

			//nie-undeadom zada polowe tego
			if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_SACRED ) )
			{
				dam = number_range( base_dam / 4, base_dam / 2 );
				dam = UMAX( dam, 1 );
				act( "Niewielki strumieñ ¶wiêtej mocy z $f trafia w $c.", victim, wield, NULL, TO_ROOM );
				act( "Niewielki strumieñ ¶wiêtej mocy z $f trafia ciê prosto w pier¶.", victim, wield, NULL, TO_CHAR );
				damage( ch, victim, dam, 0, DAM_HOLY, FALSE );
				if( IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_AVATAR )
				{
					act( "{5W krótkim rozb³ysku ¶wiat³a $n zamienia siê w kupkê dymi±cego popio³u.{x", victim, NULL, NULL, TO_ROOM );
					raw_damage( ch, victim, 20 + get_max_hp(victim) );
				}
			}

			if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_DISPEL ) )
			{
				weapon_dispeller(ch, victim, base_dam);
			}
			return;
		}
		else if ( IS_NPC( ch ) && ch->attack_flags != 0 )
			check_special_unarmed_hit( ch, victim, unarmed_base_dam );

		return;
	}

	DEBUG_INFO( "onehit:increase_wounds" );
	/* spell zwiekszajacy obrazenia slash/pierce/bash */
	check_increase_wounds( victim, dam_type, &dam );

#ifdef INFO
	print_char( ch, "DAM +INCREASE_WOUNDS: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit:troll_power" );
	if ( IS_AFFECTED( victim, AFF_TROLL ) && victim->hit > 0 && victim->hit - dam < 0 )
	{
		dam = victim->hit + 1;
		create_event( EVENT_TROLL_POWER_REGEN, number_range( 3, 5 ) * PULSE_VIOLENCE, victim, NULL, 0 );
	}

	DEBUG_INFO( "onehit:damage_reduction" );

	if ( is_affected(victim,gsn_demon_aura))
	{
        int mod = 20;
		if( IS_GOOD(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura) / mod;
        }
		else if ( !IS_EVIL(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura)/30;
        }
	}

	if ( is_affected(victim,gsn_defense_curl))
	{
		defense_curl = affect_find(victim->affected,gsn_defense_curl );
		defense_curl->modifier -= UMIN( defense_curl->level, dam );
		dam -= UMIN( defense_curl->level, dam );
		if( defense_curl->modifier <= 0 )
		{
			affect_remove( victim, defense_curl );
			act( "Otaczaj±ca ciê ochronna spirala zwija siê, a po chwili zanika.", victim,NULL,NULL,TO_CHAR);
		}
	}

	if ( is_affected( victim, gsn_damage_reduction ) && dam > 0 )
	{
		AFFECT_DATA * pAff;

		if ( ( pAff = affect_find( victim->affected, gsn_damage_reduction ) ) != NULL )
		{
			if ( pAff->level <= 16 )
				dam -= 1;
			else if ( pAff->level <= 20 )
				dam -= 2;
			else if ( pAff->level <= 25 )
				dam -= 3;
			else if ( pAff->level <= 30 )
				dam -= 4;
			else
				dam -= 5;

			dam = UMAX( dam, 1 );
		}
#ifdef INFO
		print_char( ch, "DAM - DAMAGE_REDUCTION: %d\n\r", dam );
#endif
	}

#ifdef INFO
	print_char( ch, "FINAL DAMAGE: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit:trick_entwine" );
	//tutaj zobaczymy, czy bijac z whipa nie wlazl trik entwine
	if( wield &&
		wield->value[0] == WEAPON_WHIP &&
		victim->position > POS_SITTING &&
		IS_SET(victim->parts, PART_LEGS ) )
	{
		if( second )
			SET_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );

        check_trick( ch, victim, SN_TRICK_ENTWINE );

		if( IS_SET( ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
			REMOVE_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
	}

	DEBUG_INFO( "onehit:trick_crushing_mace" );
	if( wield && wield->value[0] == WEAPON_MACE )
	{
		if( second )
        {
			SET_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
        }

        check_trick( ch, victim, SN_TRICK_CRUSHING_MACE );

		if( IS_SET( ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
        {
			REMOVE_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
        }

		if( IS_SET( ch->fight_data, FIGHT_CRUSHING_MACE_DAM ) )
		{
			dam *= 2;
			REMOVE_BIT( ch->fight_data, FIGHT_CRUSHING_MACE_DAM );
		}
	}

    DEBUG_INFO( "onehit:trick_bleed" );
    if
        (
         wield
         && !IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS )
         && !second
         && ( wield->value[0] == WEAPON_SWORD || wield->value[0] == WEAPON_SHORTSWORD )
         && !is_affected( victim, gsn_bleed )
         && victim->size <= SIZE_LARGE
         && ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
        )
        {
            if( second )
                SET_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );

            check_trick( ch, victim, SN_TRICK_BLEED );

            if( IS_SET( ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
            {
                REMOVE_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
            }
        }

	DEBUG_INFO( "onehit:trick_thigh_jab" );
	if( wield &&
		wield->value[0] == WEAPON_DAGGER &&
		!is_affected(victim, gsn_thigh_jab ) &&
		victim->size <= SIZE_LARGE &&
		victim->size >= SIZE_SMALL &&
		IS_SET(victim->parts, PART_LEGS ) )
	{
		if( second )
			SET_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );

		check_trick( ch, victim, SN_TRICK_THIGH_JAB );

		if( IS_SET( ch->fight_data, FIGHT_TRICK_WIELDSECOND ) )
        {
			REMOVE_BIT( ch->fight_data, FIGHT_TRICK_WIELDSECOND );
        }
	}

	DEBUG_INFO( "onehit:dam_mesg" );
	/* walimy opis ciosu*/
	if( wield &&
		wield->value[0] == WEAPON_SPEAR &&
		IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) &&
		victim->size < SIZE_HUGE &&
		victim->size > SIZE_TINY &&
		check_trick( ch, victim, SN_TRICK_GLORIOUS_IMPALE ) )
	{
		if( victim->sex == SEX_FEMALE )
		{
			act( "Trafiasz $C prosto w brzuch, $N wisi nadziana na $h.", ch, wield, victim, TO_CHAR );
			act( "$n trafia ciê prosto w brzuch, wisisz nadziana na $h.", ch, wield, victim, TO_VICT );
			act( "$n trafia $C prosto w brzuch, $N wisi nadziana na $h.", ch, wield, victim,TO_NOTVICT );
		}
		else if( victim->sex == SEX_MALE )
		{
			act( "Trafiasz $C prosto w brzuch, $N wisi nadziany na $h.", ch, wield, victim, TO_CHAR );
			act( "$n trafia ciê prosto w brzuch, wisisz nadziany na $h.", ch, wield, victim, TO_VICT );
			act( "$n trafia $C prosto w brzuch, $N wisi nadziany na $h.", ch, wield, victim,TO_NOTVICT );
		}
		else
		{
			act( "Trafiasz $C prosto w brzuch, $N wisi nadziane na $h.", ch, wield, victim, TO_CHAR );
			act( "$n trafia ciê prosto w brzuch, wisisz nadziane na $h.", ch, wield, victim, TO_VICT );
			act( "$n trafia $C prosto w brzuch, $N wisi nadziane na $h.", ch, wield, victim,TO_NOTVICT );
		}
		dam *= 3;
	}
	else if ( dreadful )//opis z trika dreadful strike jest w kodzie trika
		;
	else if ( !special_damage_message( ch, victim, wield, dam, dt, FALSE ) )
		dam_message( ch, victim, dam, dt, immune );

	DEBUG_INFO( "onehit:raw_damage" );
	raw_damage( ch, victim, dam );

	//trick divine impact
	if( dt == gsn_smite || dt == gsn_smite_good )
    {
        check_trick( ch, victim, SN_TRICK_DIVINE_IMPACT );
	}

	/* koniec ciekawostek dla umierajacych*/
	//noo moze nie do konca, jak koles z featem do berka kogos upioerdoli
	//a jego grupka dalej walczy to robi automatycznie assista
	if ( ch->fighting != victim || !victim->in_room )
    {
        if ( IS_SET( ch->fight_data, FIGHT_OVERWHELMING_STRIKE ) )
        {
            REMOVE_BIT( ch->fight_data, FIGHT_OVERWHELMING_STRIKE );
        }
        return;
    }

	DEBUG_INFO( "onehit:check_armor" );
	/* dla tych co przezyli, sprawdzamy armora */
	check_armor_spell( victim, dam );

	//overwhelming strike reszta bajerow
	DEBUG_INFO( "onehit:overwhelming_strike" );
	if ( wield && (IS_SET( ch->fight_data, FIGHT_OVERWHELMING_STRIKE ) || IS_SET( ch->fight_data, FIGHT_THOUSANDSLAYER ) )&& IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) )
	{
		overwhelming_skill = get_skill(ch, gsn_overwhelming_strike );

		//i tutaj mechanizm wag z basha, troche utrudniony
		overwhelming_skill -= 6 * URANGE( 0, get_curr_stat_deprecated( victim, STAT_CON ) - 18, 10 );

		if ( victim->weight > 0 && ch->weight > 0 )
		{
			if ( ch->weight > victim->weight )
			{
				overwhelming_skill += 5 * ch->weight / victim->weight;
			}
			else
			{
				if ( ch->weight < victim->weight / 2 )
					overwhelming_skill = UMIN( overwhelming_skill, 5 );
				else
					overwhelming_skill = overwhelming_skill * ch->weight / victim->weight;
			}
		}

		/* speed */
		if ( EXT_IS_SET( ch->off_flags, OFF_FAST ) || IS_AFFECTED( ch, AFF_HASTE ) )
			overwhelming_skill += 15;

		if ( EXT_IS_SET( victim->off_flags, OFF_FAST ) || IS_AFFECTED( victim, AFF_HASTE ) )
			overwhelming_skill -= 15;

		if ( dam < number_range( 27, 35 ) )
			overwhelming_skill -= ( 40 - dam )*2;

		if ( victim->position <= POS_SITTING )
			overwhelming_skill = 0;

		overwhelming_skill = URANGE( 0, overwhelming_skill, 95 );

		if ( number_percent() < overwhelming_skill / 2 && !IS_AFFECTED(victim,AFF_STABILITY) && form_check( ch, victim, NULL, gsn_bash ) && dt != gsn_slam)
		{
			act( "$n uderza z potworn± si³±, $N przewraca sie.", ch, NULL, victim, TO_NOTVICT );
			act( "$N trafia cie z potworn± si³±, probujesz z³apaæ rownowagê jednak przewracasz sie.", victim, wield, ch, TO_CHAR );
			print_char( ch, "W³o¿y³%s w ten cios sporo energii, %s przewraca siê na ziemiê.\n\r", ch->sex == 2 ? "a¶" : "e¶", PERS( victim, ch ) );
			WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
			victim->position = POS_SITTING;
		}

		//z obuchowej broni moze dac daze_state
		if (!( IS_AFFECTED( victim, AFF_PARALYZE ) || IS_AFFECTED( victim, AFF_DAZE ) ))
		{
			if ( wield->value[0] == WEAPON_MACE || wield->value[0] == WEAPON_FLAIL || wield->value[0] == WEAPON_STAFF )
			{
				if ( number_percent() < get_skill(ch,gsn_overwhelming_strike) && !stat_throw(victim, STAT_CON ) && !stat_throw(victim, STAT_WIS ) )
				{
					DAZE_STATE( victim, get_skill(ch,gsn_overwhelming_strike)/2 );
					send_to_char( "Dostajesz zawrotów g³owy!\n\r", victim );
					if( victim->sex == 2 )
						act( "$n wygl±da na lekko oszo³omion±.", victim, NULL, NULL, TO_ROOM );
					else
						act( "$n wygl±da na lekko oszo³omionego.", victim, NULL, NULL, TO_ROOM );
				}
			}
		}

		//z klujacej broni moze dziurawic
		if ( wield->value[0] == WEAPON_SPEAR && !is_undead(victim) && ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) ) )
		{
			//tutaj szansa na dziurke typu instant kill
			if ( victim->level < ch->level/3 ||
					( victim->level-1 <= ch->level/3 && IS_WEAPON_STAT(wield, WEAPON_SHARP ) ) ||
					( victim->level <= ch->level/2 && IS_WEAPON_STAT(wield, WEAPON_VORPAL ) ) )
			{
				if( number_percent() < get_skill(ch,gsn_overwhelming_strike ) && !stat_throw(victim,STAT_CON ) )
				{
					make_blood(victim);
					act( "$p przebija na wylot korpus $z wywo³uj±c fontannê krwii z rozerwanych têtnic. Cia³o $z osuwa siê bezw³adnie na ziemiê.", victim, wield, NULL, TO_ROOM );
					act( "$p przebija na wylot twój korpus wywo³uj±c fontannê krwii z rozerwanych têtnic. Osuwasz siê na ziemiê i umierasz.", victim, wield, NULL, TO_CHAR );
					raw_damage( ch, victim, victim->hit + 12 );
					return;
				}
			}//tutaj kill dobijajacy rannego
			else if ( ch->level - number_range( 4, 7 ) > victim->level )
			{
				over_chance = get_curr_stat_deprecated( ch, STAT_STR ) - 10;
				over_chance *= 2;
				if( get_curr_stat_deprecated(ch,STAT_DEX) > 20 ) over_chance += URANGE( 1, (get_curr_stat_deprecated(ch,STAT_DEX)-20)/2, 4 );
				if( get_curr_stat_deprecated(victim,STAT_DEX) > 20 ) over_chance -= URANGE( 1, (get_curr_stat_deprecated(victim,STAT_DEX)-20)/2, 4 );
				if( IS_WEAPON_STAT( wield, WEAPON_SHARP ) ) over_chance += 8;
				if( IS_WEAPON_STAT( wield, WEAPON_VORPAL ) ) over_chance += 15;

				over_hap = ( victim->hit * 100 )/get_max_hp(victim);

				if( over_hap < over_chance && !stat_throw(victim,STAT_CON) && number_percent() < get_skill(ch,gsn_overwhelming_strike ) )
				{
					make_blood(victim);
					act( "$p przebija na wylot korpus $z wywo³uj±c fontannê krwii z rozerwanych têtnic. Cia³o $z osuwa siê bezw³adnie na ziemiê.", victim, wield, NULL, TO_ROOM );
					act( "$p przebija na wylot twój korpus wywo³uj±c fontannê krwii z rozerwanych têtnic. Osuwasz siê na ziemiê i umierasz.", victim, wield, NULL, TO_CHAR );
					raw_damage( ch, victim, victim->hit + 12 );
					return;
				}
			}
			else if( number_percent() < get_skill(ch,gsn_overwhelming_strike ) && !is_affected(victim,gsn_bleeding_wound ) )
			{//a tutaj bleeding wound
				act( "G³êbokie d¼gniêcie zadane $j pozostawia na ciele $z krwawi±c± ranê!", victim, wield, NULL, TO_ROOM );
				act( "G³êbokie d¼gniêcie zadane $j pozostawia na twoim ciele krwawi±c± ranê!", victim, wield, NULL, TO_CHAR );
				bleeding_wound.where = TO_AFFECTS;
				bleeding_wound.type = gsn_bleeding_wound;
				bleeding_wound.level = 50;
				bleeding_wound.duration = URANGE( 3, (30-get_curr_stat_deprecated(victim,STAT_CON))/2, 10 );
				bleeding_wound.rt_duration = 0;
				bleeding_wound.location = APPLY_NONE;
				bleeding_wound.modifier = 0;
				bleeding_wound.bitvector = &AFF_NONE;
				affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
			}
		}

		//a tutaj crushowanie sprzetu, tylko duzo slabsze niz z do_crush

		body = get_eq_char( victim, WEAR_BODY );
		arms = get_eq_char( victim, WEAR_ARMS );
		legs = get_eq_char( victim, WEAR_LEGS );
		hands = get_eq_char( victim, WEAR_HANDS );
		feet = get_eq_char( victim, WEAR_FEET );
		head = get_eq_char( victim, WEAR_HEAD );
		bron = get_eq_char( victim, WEAR_WIELD );

		switch (dice(1,10))
		{
			case 1:
			case 2:
			case 3:
				if ( body == NULL || IS_OBJ_STAT(body, ITEM_UNDESTRUCTABLE) )
					break;
				if ( body->material == 11 || body->material == 15 || body->material == 31 )
				{
					destroy_obj( body->carried_by, body );
					break;
				}
				body->condition -= URANGE( 1, dam/5, 15 );
				if (body->condition < 1 )
				{
					destroy_obj( body->carried_by, body );
				}
				break;
			case 4:
				if ( feet == NULL || IS_OBJ_STAT(feet, ITEM_UNDESTRUCTABLE) )
					break;
				if ( feet->material == 11 || feet->material == 15 || feet->material == 31 )
				{
					destroy_obj( feet->carried_by, feet );
					break;
				}
				feet->condition -= URANGE( 1, dam/5, 15 );
				if (feet->condition < 1 )
				{
					destroy_obj( feet->carried_by, feet );
				}
				break;
			case 5:
				if ( head == NULL || IS_OBJ_STAT(head, ITEM_UNDESTRUCTABLE) )
					break;
				if ( head->material == 11 || head->material == 15 || head->material == 31 )
				{
					destroy_obj( head->carried_by, head );
					break;
				}
				head->condition -= URANGE( 1, dam/5, 15 );
				if (head->condition < 1 )
				{
					destroy_obj( head->carried_by, head );
				}
				break;
			case 6:
			case 7:
				if ( arms == NULL || IS_OBJ_STAT(arms, ITEM_UNDESTRUCTABLE))
					break;
				if ( arms->material == 11 || arms->material == 15 || arms->material == 31 )
				{
					destroy_obj( arms->carried_by, arms );
					break;
				}
				arms->condition -= URANGE( 1, dam/5, 15 );
				if (arms->condition < 1 )
				{
					destroy_obj( arms->carried_by, arms );
				}
				break;
			case 8:
			case 9:
				if ( legs == NULL || IS_OBJ_STAT(legs, ITEM_UNDESTRUCTABLE) )
					break;
				if ( legs->material == 11 || legs->material == 15 || legs->material == 31 )
				{
					destroy_obj( legs->carried_by, legs );
					break;
				}
				legs->condition -= URANGE( 1, dam/5, 15 );
				if (legs->condition < 1 )
				{
					destroy_obj( legs->carried_by, legs );
				}
				break;
			case 10:
				if ( hands == NULL || IS_OBJ_STAT(hands, ITEM_UNDESTRUCTABLE) )
					break;
				if ( hands->material == 11 || hands->material == 15 || hands->material == 31 )
				{
					destroy_obj( hands->carried_by, hands );
					break;
				}
				hands->condition -= URANGE( 1, dam/5, 15 );
				if (hands->condition < 1 )
				{
					destroy_obj( hands->carried_by, hands );
				}
				break;
		}

		REMOVE_BIT( ch->fight_data, FIGHT_OVERWHELMING_STRIKE );
	}

	DEBUG_INFO( "onehit:power_charge" );
	/* wyjatkowo mocne pierdolniecie chargem kladzie kolesia na glebie */
	if ( dt == gsn_charge && wield && !IS_AFFECTED(victim,AFF_STABILITY) )
    {
        //Raszer - zmniejszenie szansy na wywalenie charge z skill/2 do skill/3
        if ( victim->position >= POS_FIGHTING &&
                ( IS_SET( race_table[ GET_RACE(victim) ].type, PERSON ) || IS_SET( race_table[ GET_RACE(victim) ].type, ANIMAL ) ) &&
                number_percent() < get_skill( ch, gsn_charge ) / 3 &&
                ( victim->size <= ch->size ) )
        {
            act( "$n uderza z potworn± si³±, $N przewraca sie.", ch, wield, victim, TO_NOTVICT );
            act( "$N trafia cie z potworn± si³±, probujesz z³apaæ rownowagê jednak przewracasz sie.", victim, wield, ch, TO_CHAR );
            print_char( ch, "Wlozyl%s w ten cios sporo energii, %s przewraca siê na ziemiê.\n\r", ch->sex == 2 ? "a¶" : "e¶", PERS( victim, ch ) );
            check_trick( ch, victim, SN_TRICK_STRUCKING_WALLOP );
            victim->position = POS_SITTING;
            WAIT_STATE( victim, 1.5 * PULSE_VIOLENCE );
        }
    }
	else if ( dt == gsn_stun && dam > 0 )
	{
		int daze_pulse = 1;

		if ( IS_NPC( ch ) )
			daze_pulse = URANGE( 2, 2 + ch->level / 7, 7 );
		else
		{
			if ( ch->class == CLASS_PALADIN || ch->class == CLASS_WARRIOR || ch->class == CLASS_SHAMAN )
				daze_pulse = number_range( 1, URANGE( 1, 1 + get_skill( ch, gsn_stun ) / 30, 4 ) );
			else if ( ch->class == CLASS_CLERIC )
				daze_pulse = number_range( 1, URANGE( 1, get_skill( ch, gsn_stun ) / 40, 2 ) );

			if ( critical )
				daze_pulse++;
		}

		if ( IS_AFFECTED( victim, AFF_BERSERK ) )
		{
			//szansa taka jaki dam, ale na polowe czasu
			if ( number_percent() < dam )
			{
				daze_pulse = UMAX( 1, daze_pulse / 2 );
				DAZE_STATE( victim, daze_pulse * PULSE_VIOLENCE );
			}
		}
		else
			DAZE_STATE( victim, daze_pulse * PULSE_VIOLENCE );
	}

	//mo¿e nie mieæ ju¿ broni (trick strucking_wallop)
	if ( !second )
		wield = get_eq_char( ch, WEAR_WIELD );
	else
		wield = get_eq_char( ch, WEAR_SECOND );

	DEBUG_INFO( "onehit:update_obj_condition" );
	if ( result )
		update_obj_cond( ch, wield, base_dam, dam, victim );

	//mo¿e nie mieæ ju¿ broni jak siê rozpad³a
	if ( !second )
		wield = get_eq_char( ch, WEAR_WIELD );
	else
		wield = get_eq_char( ch, WEAR_SECOND );

	DEBUG_INFO( "onehit:spell_draining_hands" );
	if ( ch->fighting == victim && is_affected(ch,gsn_draining_hands) && !is_undead( victim ) && !IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		if (dam>0)
		{
			act( "Twój dotyk wysysa ¿ycie z $Z.", ch, NULL, victim, TO_CHAR );
			act( "Czujesz jak dotyk $z wysysa z ciebie energiê.", ch, NULL, victim, TO_VICT );
			act( "Dotyk $z wysysa ¿ycie z $Z.", ch, NULL, victim, TO_NOTVICT );
			if (ch->hit + dam > 125 * ch->hit / 100)
			{
				act( "Energia wyssana z $Z ucieka bezpowrotnie.", ch, NULL, victim, TO_CHAR );
			}
			else
			{
				ch->hit = UMIN( ch->hit + dam, 125 * ch->hit / 100 );
			}
		}
	}

	DEBUG_INFO( "onehit:funky_weapon" );
	/* but do we have a funky weapon? */
	if ( result && wield != NULL )
	{
		int dam;

		if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_POISON ) )
		{
			int level;
			AFFECT_DATA *poison;

			if ( ( poison = affect_find( wield->affected, gsn_poison ) ) == NULL )
				level = 1;
			else
				level = URANGE( 0, poison->level, MAX_POISON );

			poison_to_char( victim, level );
			/* weaken the poison if it's temporary */
			if ( poison != NULL )
			{
				poison->duration = UMAX( 0, poison->duration - 1 ); poison->rt_duration = 0;

				if ( wield && poison->duration == 0 )
				{
					act( "Trucizna pokrywaj±ca ostrze $f wysycha.", ch, wield, NULL, TO_CHAR );
					affect_remove_obj( wield, poison );
				}
			}
		}

		/* wysysanie, nie zadaje obrazen bo juz zadalo wczesniej */
		if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_VAMPIRIC ) && !is_undead( victim ) && !IS_SET( victim->form, FORM_CONSTRUCT ) )
		{
			dam = number_range( base_dam / 4, base_dam / 2 );
			if (dam>0)
			{
                if (wield->gender < GENDER_ZENSKI) {
                    act( "$p wysysa ¿ycie z $z.", victim, wield, NULL, TO_ROOM );
                    act( "Czujesz jak $p wysysa z ciebie energiê.", victim, wield, NULL, TO_CHAR );
                }
                else {
                    act( "$p wysysaj± ¿ycie z $z.", victim, wield, NULL, TO_ROOM );
                    act( "Czujesz jak $p wysysaj± z ciebie energiê.", victim, wield, NULL, TO_CHAR );
                }
				if (ch->hit + dam > 125 * ch->hit / 100)
				{
					act( "Energia wyssana z $Z ucieka bezpowrotnie.", ch, NULL, victim, TO_CHAR );
				}
				else
				{
					ch->hit = UMIN( ch->hit + dam, 125 * ch->hit / 100 );
				}
			}
		}

		if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_FLAMING ) )
		{
			dam = number_range( base_dam / 4, base_dam / 2 );
			dam = UMAX( dam, 1 );
			act( "$p lekko przysma¿a $C.", ch, wield, victim, TO_CHAR );
			act( "Parzysz siê przy zetkniêciu z $j.", ch, wield, victim, TO_VICT );
			act( "$N parzy siê przy zetkniêciu z $j.", ch, wield, victim, TO_NOTVICT );
			fire_effect( ( void * ) victim, 1, dam, TARGET_CHAR );
			damage( ch, victim, dam, 0, DAM_FIRE, FALSE );
		}

		if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_FROST ) )
		{
			dam = number_range( base_dam / 4, base_dam / 2 );
			dam = UMAX( dam, 1 );
			act( "$p zamra¿a $c.", victim, wield, NULL, TO_ROOM );
			act( "Zimny dotyk $f zamienia ciê w bry³ê lodu.", victim, wield, NULL, TO_CHAR );
			cold_effect( victim, 1, dam, TARGET_CHAR );
			damage( ch, victim, dam, 0, DAM_COLD, FALSE );
		}

		if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_SHOCKING ) )
		{
			dam = number_range( base_dam / 4, base_dam / 2 );
			dam = UMAX( dam, 1 );
			act( "B³yskawica z $f trafia $c wywo³uj±c u niego niekontrolowane drgawki.", victim, wield, NULL, TO_ROOM );
			act( "$p aplikuje ci seriê elektrowstrzasów.", victim, wield, NULL, TO_CHAR );
			shock_effect( victim, 1, dam, TARGET_CHAR );
			damage( ch, victim, dam, 0, DAM_LIGHTNING, FALSE );
		}

		if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_TOXIC ) )
		{
			dam = number_range( base_dam / 4, base_dam / 2 );
			dam = UMAX( dam, 1 );
			act( "Pokrywaj±cy $h kwas wy¿era skórê $z.", victim, wield, NULL, TO_ROOM );
			act( "Pokrywaj±cy $h kwas wy¿era tw± skórê!", victim, wield, NULL, TO_CHAR );
			acid_effect( victim, 1, dam, TARGET_CHAR );
			damage( ch, victim, dam, 0, DAM_ACID, FALSE );
		}

		if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_RESONANT ) )
        {
            dam = number_range( base_dam / 4, base_dam / 2 );
            dam = UMAX( dam, 1 );
            if (IS_SET(victim->parts, PART_HEAD ) )
            {
                act( "Fala ultrad¼wiêków z $f wdziera siê do g³owy $z.", victim, wield, NULL, TO_ROOM );
                act( "Fala ultrad¼wiêków z $f wdziera siê do twojej g³owy wraz z pulsuj±cym bólem.", victim, wield, NULL, TO_CHAR );
            }
            else
            {
                act( "Fala ultrad¼wiêków z $f wdziera siê do cia³a $z.", victim, wield, NULL, TO_ROOM );
                act( "Fala ultrad¼wiêków z $f wdziera siê do twojego cia³a wraz z pulsuj±cym bólem.", victim, wield, NULL, TO_CHAR );
            }
            sound_effect( victim, 1, dam, TARGET_CHAR );
            damage( ch, victim, dam, 0, DAM_SOUND, FALSE );
        }

		//nie-undeadom zada polowe tego
		if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_SACRED ) )
		{
			dam = number_range( base_dam / 4, base_dam / 2 );
			dam = UMAX( dam, 1 );
			act( "Niewielki strumieñ ¶wiêtej mocy z $f trafia $c.", victim, wield, NULL, TO_ROOM );
			act( "Niewielki strumieñ ¶wiêtej mocy z $f trafia ciê prosto w pier¶.", victim, wield, NULL, TO_CHAR );
			damage( ch, victim, dam, 0, DAM_HOLY, FALSE );
			if( IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_AVATAR )
			{
				act( "{5W krótkim rozb³ysku ¶wiat³a $n zamienia siê w kupkê dymi±cego popio³u.{x", victim, NULL, NULL, TO_ROOM );
				raw_damage( ch, victim, 20 + get_max_hp(victim) );
			}
		}

		if ( ch->fighting == victim && !is_affected(victim,gsn_bleeding_wound ) && !is_undead(victim) &&
				( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) ) &&
				( IS_WEAPON_STAT( wield, WEAPON_SHARP ) || IS_WEAPON_STAT( wield, WEAPON_VORPAL ) || IS_WEAPON_STAT( wield, WEAPON_INJURIOUS ) ) )
		{
			//szansa na zranienie
			if( IS_WEAPON_STAT( wield, WEAPON_SHARP ) && !stat_throw( victim, STAT_CON ) && number_percent() <= 15 )
			{
				act( "Cios zadany $j pozostawia na ciele $z krwawi±c± ranê!", victim, wield, NULL, TO_ROOM );
				act( "Cios zadany $j pozostawia na twoim ciele krwawi±c± ranê!", victim, wield, NULL, TO_CHAR );
				bleeding_wound.where = TO_AFFECTS;
				bleeding_wound.type = gsn_bleeding_wound;
				bleeding_wound.level = 50;
				bleeding_wound.duration = number_range(2,4);
				bleeding_wound.rt_duration = 0;
				bleeding_wound.location = APPLY_NONE;
				bleeding_wound.modifier = 0;
				bleeding_wound.bitvector = &AFF_NONE;
				affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
			}
			else if( IS_WEAPON_STAT( wield, WEAPON_VORPAL ) && !stat_throw( victim, STAT_CON ) && number_percent() <= 35 )
			{
				act( "Cios zadany $j pozostawia na ciele $z krwawi±c± ranê!", victim, wield, NULL, TO_ROOM );
				act( "Cios zadany $j pozostawia na twoim ciele krwawi±c± ranê!", victim, wield, NULL, TO_CHAR );
				bleeding_wound.where = TO_AFFECTS;
				bleeding_wound.type = gsn_bleeding_wound;
				bleeding_wound.level = 50;
				bleeding_wound.duration = number_range(3,5);
				bleeding_wound.rt_duration = 0;
				bleeding_wound.location = APPLY_NONE;
				bleeding_wound.modifier = 0;
				bleeding_wound.bitvector = &AFF_NONE;
				affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
			}
			else if( IS_WEAPON_STAT( wield, WEAPON_INJURIOUS ) && number_percent() <= 25 )
			{
				act( "Cios zadany $j pozostawia na ciele $z krwawi±c± ranê!", victim, wield, NULL, TO_ROOM );
				act( "Cios zadany $j pozostawia na twoim ciele krwawi±c± ranê!", victim, wield, NULL, TO_CHAR );
				bleeding_wound.where = TO_AFFECTS;
				bleeding_wound.type = gsn_bleeding_wound;
				bleeding_wound.level = 50;
				bleeding_wound.duration = number_range(7,10);
				bleeding_wound.rt_duration = 0;
				bleeding_wound.location = APPLY_NONE;
				bleeding_wound.modifier = 0;
				bleeding_wound.bitvector = &AFF_NONE;
				affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
			}
		}

		if ( ch->fighting == victim && IS_WEAPON_STAT( wield, WEAPON_DISPEL ) )
		{
			weapon_dispeller(ch, victim, base_dam);
		}

	}

    /**
     * WEAPON_WICKED
     */
    if ( wield && IS_SET( wield->value[ 4 ], WEAPON_WICKED ) )
    {
        print_char( ch, "{r%s{m wysysa z ciebie odrobinê ¿ycia.{x\n\r", capitalize( wield->short_descr ) );
        raw_damage( ch, ch, number_range( 1, 3 ) );
    }

	if ( result && IS_NPC( ch ) && ch->attack_flags != 0 )
		check_special_unarmed_hit( ch, victim, unarmed_base_dam );

	if ( !wield && IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_HIT ) )
		mp_hit_trigger( ch, victim );
	else if ( wield && HAS_OTRIGGER( wield, TRIG_HIT ) )
		op_hit_trigger( wield, ch );

	tail_chain( );

    /**
     * drop parts
     */
    part_dropper_wraper( victim );

	return;
}

/*
 * Inflict damage from a hit (do_damage).
 */
bool damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, int dam_type, bool show )
{
	OBJ_DATA * obj, *obj_next;
	bool immune = FALSE, check;

	save_debug_info("fight.c => damage", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );


	if ( dam < 0 )
	{
		victim->hit = UMIN( get_max_hp(victim), victim->hit - dam );
		update_pos( victim );
		return TRUE;
	}

	if ( victim->position == POS_DEAD )
		return FALSE;

	if ( victim != ch )
	{
		if ( is_safe( ch, victim ) )
			return FALSE;

		if ( victim->position > POS_STUNNED )
		{
			if ( victim->fighting == NULL )
			{
				//ZMIANA
				if ( !can_move( victim ) &&
						( ch->fighting == victim || ch->fighting == NULL ) )
					set_fighting( victim, ch );

				if ( can_move( victim ) )
					set_fighting( victim, ch );

				if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
					mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_KILL );
			}
		}

		if ( ch->position > POS_STUNNED )
		{
			if ( ch->fighting == NULL )
				set_fighting( ch, victim );
		}


		if ( victim->master == ch )
		{
			if( IS_NPC( victim ) && IS_AFFECTED(victim,AFF_CHARM) )
			{
				act( "Wiê¼ miêdzy tob± a $V zostaje brutalnie przerwana!", ch, NULL, victim, TO_CHAR );
				raw_damage( ch, ch, number_range( ch->level/2, ch->level+ch->level/2 ) );
				if ( ch->position <= POS_STUNNED )
					return FALSE;
			}
			stop_follower( victim );
		}
	}

	/*
	 * Inviso attacks ... not.
	 */
	if ( IS_AFFECTED( ch, AFF_HIDE ) )
		affect_strip( ch, gsn_hide );

	strip_invis( ch, TRUE, TRUE );

	if ( IS_NPC( victim ) && !is_fearing( victim, ch ) )
	{
		if ( !EXT_IS_SET( victim->act, ACT_SENTINEL ) )
			start_hunting( victim, ch );

		start_hating( victim, ch );
	}

	if ( dam == 0 )
	{
		if ( show )
			dam_message( ch, victim, dam, dt, immune );

		return FALSE;
	}

	if ( ( dam_type == DAM_BASH ||
				dam_type == DAM_PIERCE ||
				dam_type == DAM_SLASH ) &&
			victim->resists[ RESIST_MAGIC_WEAPON ] > 0 && !IS_AFFECTED( victim, AFF_RESIST_MAGIC_WEAPON))
	{
		if ( (dt == gsn_kick && get_eq_char( ch, WEAR_FEET ) != NULL && IS_OBJ_STAT( get_eq_char( ch, WEAR_FEET ), ITEM_MAGIC) && victim->resists[ RESIST_MAGIC_WEAPON ] <= (get_curr_stat_deprecated(ch,STAT_STR)-6)/10 && dice( 2, get_curr_stat_deprecated(ch,STAT_STR)) > 14  ))
        {
			;
        }
		else
		{
			immune = TRUE;
			dam = 0;

			if ( show )
				dam_message( ch, victim, dam, dt, immune );
			return FALSE;
		}
	}

	immune = FALSE;

	if ( dt != gsn_bash && dt != gsn_crush && dt != gsn_slam && dt != gsn_trip )
	{
		immune = check_blink( victim, ch, TRUE );

		if( immune )
		{
			if ( show )
			dam_message( ch, victim, dam, dt, immune );

			act( "Sylwetka chwilowo znika unikaj±c twego ataku.", ch, NULL, victim, TO_CHAR );
		act(	 "Znikasz na chwilkê z tego planu egzystencji unikaj±c ataku $z.\n\r", ch, NULL, victim, TO_VICT );
			act( "Sylwetka $Z chwilowo znika, unikaj±c ataku $z.", ch, NULL, victim, TO_NOTVICT );
			return FALSE;
		}
	}

	if ( dam_type != DAM_NONE && dam_type != DAM_SOUND && dt != gsn_bash && dt != gsn_crush && dt != gsn_turn && dt != gsn_slam && dt != gsn_trip )
	{
		immune = check_mirror_image( victim, ch );

		if ( immune )
		{
			if ( show )
				dam_message( ch, victim, dam, dt, immune );

			print_char( ch, "Lustrzane odbicie %s migocze przez chwilê i znika.\n\r", PERS4( victim, ch ) );
			send_to_char( "Jedno z twoich lustrzanych odbiæ migocze przez chwilê i znika.\n\r", victim );
			act( "Lustrzane odbicie $Z migocze przez chwilê i znika.", ch, NULL, victim, TO_NOTVICT );
			return FALSE;
		}
	}

	immune = IS_AFFECTED( victim, AFF_ILLUSION );

	if ( immune )
	{
		if ( show )
			dam_message( ch, victim, dam, dt, immune );

		print_char( ch, "Twój cel rozp³ywa siê w powietrzu i znika.\n\r", PERS4( victim, ch ) );
		send_to_char( "Rozp³ywasz siê w powietrzu.\n\r", victim );
		act( "$N rozp³ywa siê w powietrzu.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala iluzja opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemiê.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp³ywa siê w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez ¶ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp³ywa siê pozostawiaj±c tylko chmurê dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy siê przez chwilê, po czym rozpryskujê siê na kwa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p pêka z hukiem i rozpryskuje siê dooko³a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje siê na kawa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}
		extract_char( victim, TRUE );

		return FALSE;
	}

	immune = IS_AFFECTED( victim, AFF_SOUL );

	if ( immune )
	{
		if ( show )
			dam_message( ch, victim, dam, dt, immune );

		print_char( ch, "Uwalniasz duszê %s, która ulatuje do swojego wymiaru\r", PERS4( victim, ch ) );
		send_to_char( "Ulatujesz do swojego wymiaru.\n\r", victim );
		act( "$n uwalnia duszê $Z, która ulatuje do swojego wymiaru.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala dusza opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemiê.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp³ywa siê w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez ¶ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp³ywa siê pozostawiaj±c tylko chmurê dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy siê przez chwilê, po czym rozpryskujê siê na kwa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p pêka z hukiem i rozpryskuje siê dooko³a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje siê na kawa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}

		extract_char( victim, TRUE );

		return FALSE;
	}

	/* Stone skin - wstawka
	 *
	 */
	immune = check_stone_skin( NULL, victim, dam_type, &dam, NULL );

	if ( dam == 0 )
	{
		if ( show )
			dam_message( ch, victim, dam, dt, immune );

		return FALSE;
	}

	dam = check_resist( victim, dam_type, dam );

	if ( dam == 0 )
	{
		immune = TRUE;

		if ( show )
			dam_message( ch, victim, dam, dt, immune );

		return FALSE;
	}

	if ( IS_AFFECTED( victim, AFF_TROLL ) && victim->hit > 0 && victim->hit - dam < 0 )
	{
		dam = victim->hit + 1;

		create_event( EVENT_TROLL_POWER_REGEN, number_range( 3, 5 ) * PULSE_VIOLENCE, victim, NULL, 0 );
	}

	if ( show )
    {
		dam_message( ch, victim, dam, dt, immune );
    }

	raw_damage( ch, victim, dam );

	return TRUE;
}

bool mob_can_attack( CHAR_DATA * ch )
{
	if ( !IS_NPC( ch ) )
		return FALSE;

	if ( ch->pIndexData->pShop || ch->pIndexData->pRepair )
		return FALSE;

	return TRUE;
}

bool is_safe( CHAR_DATA *ch, CHAR_DATA *victim )
{
	if ( ch != victim && is_affected( ch, gsn_sanctuary ) )
		affect_strip( ch, gsn_sanctuary );

	if ( ch->master && IS_AFFECTED( ch, AFF_CHARM ) && is_affected( ch->master, gsn_sanctuary ) )
		affect_strip( ch->master, gsn_sanctuary );

	if ( victim->in_room == NULL || ch->in_room == NULL )
		return TRUE;

	if ( victim->fighting == ch || victim == ch )
		return FALSE;

	if ( IS_IMMORTAL( ch ) && ch->level > LEVEL_IMMORTAL )
		return FALSE;

	if ( EXT_IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
		return TRUE;

	if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
		return TRUE;

	if ( IS_AFFECTED( victim, AFF_FORCE_FIELD ) )
		return TRUE;

	/* killing mobiles */
	if ( IS_NPC( victim ) )
	{
		if ( victim->pIndexData->pShop != NULL )
		{
			send_to_char( "Mo¿e lepiej by¶ co¶ kupi³<&/a/o>?\n\r", ch );
			return TRUE;
		}

		if ( victim->pIndexData->pRepair != NULL )
		{
			send_to_char( "Mo¿e lepiej by¶ co¶ naprawi³<&/a/o>?\n\r", ch );
			return TRUE;
		}

		/* no killing healers, trainers, etc */
		if ( EXT_IS_SET( victim->act, ACT_PRACTICE ) || EXT_IS_SET( victim->act, ACT_REWARD ) )
		{
			send_to_char( "Naprawdê nie ma co juz zabijaæ?\n\r", ch );
			return TRUE;
		}
	}
	return FALSE;
}

bool is_safe_spell( CHAR_DATA *ch, CHAR_DATA *victim, bool area )
{
	save_debug_info("fight.c => is_safe_spell", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );
	if ( ch != victim && is_affected( ch, gsn_sanctuary ) )
		affect_strip( ch, gsn_sanctuary );

	if ( victim->in_room == NULL || ch->in_room == NULL )
		return TRUE;

	if ( victim == ch && area )
		return TRUE;

	if ( victim->fighting == ch || victim == ch )
		return FALSE;

	if ( IS_IMMORTAL( ch ) && ch->level > LEVEL_IMMORTAL && !area )
		return FALSE;

	if ( EXT_IS_SET( victim->in_room->room_flags, ROOM_SAFE ) )
		return TRUE;

	if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
		return TRUE;

	/* killing mobiles */
	if ( IS_NPC( victim ) )
	{
		if ( victim->pIndexData->pShop != NULL )
			return TRUE;

		if ( EXT_IS_SET( victim->act, ACT_PRACTICE ) )
			return TRUE;

		if ( IS_NPC( ch ) )
		{
			/* area effect spells do not hit other mobs */
			if ( area && !is_same_group( victim, ch->fighting ) )
				return TRUE;
		}
	}
	/* killing players */
	else
	{
		if ( area && IS_IMMORTAL( victim ) && victim->level > LEVEL_IMMORTAL )
			return TRUE;

		/* NPC doing the killing */
		if ( IS_NPC( ch ) )
		{
			/* charmed mobs and pets cannot attack players while owned */
			if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master != NULL
					&& ch->master->fighting != victim )
				return TRUE;

		}

		/* player doing the killing */
		else
		{
			;
		}

	}
	return FALSE;
}

/*
 * See if an attack justifies a KILLER flag.
 */
void check_killer( CHAR_DATA *ch, CHAR_DATA *victim )
{
	// zdejmujemy atakuj±cemu sanctuary
	if ( is_affected( ch, gsn_sanctuary ) )
	{
		affect_strip( ch, gsn_sanctuary );
	}

	// zdejmujemy zaatakowanemu healing sleep
	if ( is_affected( victim, gsn_healing_sleep ) )
	{
		affect_strip( victim, gsn_healing_sleep );
	}

    // logowanie ataków
    if (IS_NPC( ch ) && ch->master )
    {
        sprintf( log_buf, "[%d] attacker: %s (%d), defender (%s): %s (%d), master %s (%d)",
                victim->in_room ? victim->in_room->vnum : 0,
                ( IS_NPC( ch ) ? ch->short_descr : ch->name ),
                ch->level,
                ( IS_NPC( victim ) )? "npc":"pc",
                ( IS_NPC( victim ) ? victim->short_descr : victim->name ),
                victim->level,
                ch->master->name,
                ch->master->level
               );
    }
    else
    {
        sprintf( log_buf, "[%d] attacker: %s (%d), defender (%s): %s (%d)",
                victim->in_room ? victim->in_room->vnum : 0,
                ( IS_NPC( ch ) ? ch->short_descr : ch->name ),
                ch->level,
                ( IS_NPC( victim ) )? "npc":"pc",
                ( IS_NPC( victim ) ? victim->short_descr : victim->name ),
                victim->level
               );
    }
    log_string( log_buf );
	return;
}

/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim, int dam, OBJ_DATA *parowana )
{
	int parry_skill, dex_mod = 0, chance = 0, percent;
	int max_dam = 0, prim_skill = 0, offhand_skill = 0, i = 0;
	OBJ_DATA *prim = NULL, *offhand = NULL;
    bool check;

	if (!IS_SET( victim->fight_data, FIGHT_PARRY ) )
	{
        return FALSE;
	}

	if ( !IS_AWAKE( victim ) ||
			!can_move( victim ) ||
			!can_see( victim, ch ) )
    {
		return FALSE;
    }

	parry_skill = get_skill( victim, gsn_parry );

	/*bez broni nie da rady*/
	prim = get_eq_char( victim, WEAR_WIELD );
	offhand = get_eq_char( victim, WEAR_SECOND );

	if ( !prim && !offhand )
	{
		return FALSE;
	}

	dex_mod = (2*(get_curr_stat_deprecated( victim, STAT_DEX )-10))/3;
	chance = parry_skill / 3;

	if( is_affected( ch, gsn_wardance ))
	{
		chance += get_skill(ch, gsn_wardance)/10;
	}

	if ( prim )
	{
		max_dam = dice( prim->value[ 1 ], prim->value[ 2 ] ) + prim->value[ 6 ];
		max_dam += GET_SKILL_DAMROLL( victim, prim );
		prim_skill = get_weapon_skill( victim, get_weapon_sn( victim, TRUE ) );

		if ( offhand )
        {
            offhand_skill = get_weapon_skill( victim, get_weapon_sn( victim, FALSE ) );
            chance += ( parry_mod( prim ) + parry_mod( offhand ) ) / 2;
            chance += dex_mod;

            max_dam += ( dice( offhand->value[ 1 ], offhand->value[ 2 ] ) + offhand->value[ 6 ] ) / 2;
            max_dam += GET_SKILL_DAMROLL( victim, offhand )/2;
            chance += ( prim_skill + offhand_skill ) / 5;
        }
		else
		{
			chance += prim_skill / 3;
			chance += parry_mod( prim );
			chance += dex_mod;
		}

	}
	else if ( offhand )
    {
        offhand_skill = get_weapon_skill( victim, get_weapon_sn( victim, FALSE ) );
        max_dam += ( dice( offhand->value[ 1 ], offhand->value[ 2 ] ) + offhand->value[ 6 ] ) / 2;
        max_dam += GET_SKILL_DAMROLL( victim, offhand )/2;
        chance += offhand_skill / 5;
        chance += parry_mod( offhand );
        chance += dex_mod;
    }

	max_dam += URANGE( 0, 2*(get_curr_stat_deprecated(victim,STAT_DEX)-17)/3,7);

	if ( chance > number_range( 50, 70 ) && victim->level >= ch->level )
    {
		chance -= ( victim->level + 3 - ch->level )*2;
    }

	//i tutaj od rozmiarow - nie sparujesz trzepniecia lapy smoka, ktora jest od ciebie dwa razy wieksza
	if ( victim->size + 2 < ch->size )
    {
		return FALSE;
    }
	else if ( victim->size + 1 < ch->size )
	{
		if( victim->size == SIZE_SMALL )
        {
			chance = URANGE( 3, 3*chance/4, 75 );
        }
		else
        {
			chance = URANGE( 3, chance/2, 45 );
        }
	}

	chance = URANGE(3,chance,95);

	//tutaj trick weapon wrench osoby ktora jest parowana, niezaleznie od tego czy parry wejdzie czy nie
	//jak trik wejdzie to parry nie wejdzie
	//szansa na wejscie trika 10 razy wieksza niz przy zwyklym ciosie
	if( parowana &&	parowana->item_type == ITEM_WEAPON && parowana->value[0] == WEAPON_WHIP )
	{
		//nawet jak trik nie wejdzie, to bicz naprawde w cholere trudno sparowac
		chance /= 4;

		if( parowana == get_eq_char(victim,WEAR_SECOND) )
			SET_BIT( victim->fight_data, FIGHT_TRICK_WIELDSECOND );

		for( i = 0; i < 10; ++i )//10 razy wieksza szansa, czyli dziesiec razy testuje trika, czyli 25%
		{
			if( !is_affected(victim, gsn_weapon_wrench) && !is_affected(ch, gsn_weapon_wrench) && check_trick( victim, ch, SN_TRICK_WEAPON_WRENCH ) )
			{
				if( IS_SET( victim->fight_data, FIGHT_TRICK_WIELDSECOND ) )
					REMOVE_BIT( victim->fight_data, FIGHT_TRICK_WIELDSECOND );
				return FALSE;
			}
		}

		if( IS_SET( victim->fight_data, FIGHT_TRICK_WIELDSECOND ) )
			REMOVE_BIT( victim->fight_data, FIGHT_TRICK_WIELDSECOND );
	}

#ifdef INFO
	print_char( victim, "Max_dam = %d\n\r", max_dam );
	print_char( victim, "Chance to parry = %d\n\r", chance );
#endif

	if ( max_dam < 3 * dam / 4 )
	{
		return FALSE;
	}

	percent = number_percent();
	if ( percent > chance )
	{
		return FALSE;
	}

	// teraz przeliczamy to i normalizujemy, tak ¿eby widzieæ "si³ê" parowania
	// i na tej podstawie obliczymy jak "skuteczne" by³o parowanie, dziêki
	// czemu zróznicujemy komunikaty na "s³abe", "normalne" i "silne" parowanie.

	// najpierw normalizacja wyniku w zakresie 0-100, opieramy siê na tym, ¿e
	// chance >= percent (wynika to z warunku, który zwraca false), nadpisujemy
	// zmienn± chance, poniewa¿ nie bêdzie ona ju¿ w tej funkcji potrzebna

	chance = (percent * 100) / chance;

	// nastêpnie wk³adamy warunki i opisy

	if (chance > 21 ) // tutaj to co jest "mocarnym sparowaniem")
	{
		switch(number_range(0,4))
		{
			case 1:
				act( "{5Bez ¿adnego problemu parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N bez ¿adnego problemu paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N bez ¿adnego problemu paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 2:
				act( "{5Bez wysi³ku parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N bez wysi³ku paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N bez wysi³ku paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 3:
				act( "{5Bez k³opotu parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N bez k³opotu paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N bez k³opotu paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 4:
				act( "{5Z gracj± parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N z gracj± paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N z gracj± paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			default:
				act( "{5Z gracj± parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N z gracj± paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N z gracj± paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
		}
	}
	else if (chance < 76) // sparowanie s³abe
	{
		switch(number_range(0,5))
		{
			case 1:
				act( "{5Prawie w ostatniej chwili parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5Prawie w ostatniej chwili $N paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5Prawie w ostatniej chwili $N paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 2:
				act( "{5Nadzwyczajnym wysi³kiem parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N nadzwyczajnym wysi³kiem paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N nadzwyczajnym wysi³kiem paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 3:
				act( "{5Ledwie udaje ci siê sparowaæ atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N ledwo paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N ledwo paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 4:
				act( "{5Przypadkowo parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N przypadkowo paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N przypadkowo paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 5:
				act( "{5Z trudem parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N z trudem paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N z trudem paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			default:
				act( "{5Z trudem parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N z trudem paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N z trudem paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
		}
	}
	else // sparowanie normalne
	{
		switch(number_range(0,4))
		{
			case 1:
				act( "{5Zwinnie parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N zwinnie paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N zwinnie paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 2:
				act( "{5Bez problemu parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N bez problemu paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N bez problemu paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 3:
				act( "{5Zrêcznie parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N zrêcznie paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N zrêcznie paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			case 4:
				act( "{5P³ynnie parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N p³ynnie paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N p³ynnie paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
				break;
			default:
				act( "{5Zrêcznie parujesz atak $z.{x", ch, NULL, victim, TO_VICT );
				act( "{5$N z zaskakuj±c± zrêczno¶ci± paruje twój atak.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$N z zaskakuj±c± zrêczno¶ci± paruje atak $z.{x", ch, NULL, victim, TO_NOTVICT );
		}
	}
    check_trick( victim, ch, SN_TRICK_RIPOSTE );
	return TRUE;
}

/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    if (victim->position == POS_FIGHTING)
    {
        if (victim->fighting == NULL || victim->in_room != victim->fighting->in_room )
        {
            victim->position = POS_STANDING;
        }
    }

	if ( victim->hit > 0 )
	{
		if ( victim->position <= POS_STUNNED )
		{

			victim->position = POS_SITTING;
			//do_function(victim, &do_rest, "");
			return;
		}

		return;
	}

	if ( victim->hit <= -11 )
	{
		victim->position = POS_DEAD;
		return;
	}

	if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
	else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
	else victim->position = POS_STUNNED;

	return;
}

/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
	if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
	{
		send_to_char( "Czujesz, ¿e twoje cia³o znajduje siê w niebezpieczeñstwie. ", ch );
		mind_move_room( ch, NULL );
	}

	if ( ch->fighting != NULL )
	{
		return;
	}

	if ( ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 ) || ( IS_NPC( victim ) && victim->pIndexData->vnum == 3 ) )
	{
		return;
	}

	if ( IS_AFFECTED( ch, AFF_SLEEP ) )
	{
		affect_strip( ch, gsn_sleep );
	}

	if ( IS_AFFECTED( ch, AFF_RECUPERATE ) )
	{
		affect_strip( ch, gsn_recuperate );
	}

	if( IS_AFFECTED(victim, AFF_MEDITATION) )
	{
		affect_strip( ch, gsn_meditation );
	}

	if (  can_move( ch ) )
	{
		if ( ch->position == POS_SITTING || ch->position == POS_RESTING )
			act( "$n wstaje.", ch, NULL, NULL, TO_ROOM );
		ch->position = POS_FIGHTING;
	}
	else if ( ch->position == POS_SLEEPING )
	{
		act( "$n budzi siê i siada.", ch, NULL, NULL, TO_ROOM );
		act_new( "$N brutalnie przerywa twój sen.", ch, NULL, victim, TO_CHAR, POS_SLEEPING );
		ch->position = POS_SITTING;
	}

	ch->fighting = victim;

	return;
}

/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
	CHAR_DATA * fch;

	/* heh, kurwa nie wiem co tu siê dzieje...
	 * jesli jest fBoth = FALSE to po chuj przegladac wszystkie
	 * postacie na liscie...wystarczy tego kolesia z argumentu
	 * wykorzystac...jakis kretyn to pisal
	 */
	ch->fighting = NULL;

	if ( ch->position >= POS_RESTING )
	{
		if ( ch->position == POS_SITTING ||  ch->position == POS_RESTING )
	if(IS_NPC(ch))	act( "$n wstaje.", ch, NULL, NULL, TO_ROOM );//Raszer, tutaj tez powinien byc warunek, 31.05.2009
				if(IS_NPC(ch)) ch->position = ch->default_pos; //Raszer, wylaczenie automatycznego wstawania po walce
    //		act( "$n wstaje.", ch, NULL, NULL, TO_ROOM );
	//	ch->position	= IS_NPC( ch ) ? ch->default_pos : POS_STANDING;
	}


	update_pos( ch );

	if ( !fBoth )
		return;

	for ( fch = char_list; fch != NULL; fch = fch->next )
		if ( fch->fighting == ch )
		{
			fch->fighting	= NULL;

			if ( fch->position >= POS_RESTING )
			{
				if ( fch->position == POS_SITTING ||  fch->position == POS_RESTING )
				if(IS_NPC(fch))	act( "$n wstaje.", fch, NULL, NULL, TO_ROOM );//Raszer, tutaj tez powinien byc warunek, 31.05.2009
				if(IS_NPC(fch)) fch->position = fch->default_pos; //Raszer, wylaczenie automatycznego wstawania po walce
				fch->fighting = NULL;
			}

			update_pos( fch );
		}
	return;
}

/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch )
{
    char buf[ MAX_STRING_LENGTH ];
    OBJ_DATA *corpse = NULL;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *random_obj = NULL;
    char *name;
    int corpse_vnum;
    bool custom_body = FALSE;
    int i;
    OBJ_DATA *random_obj_spellbook;

    if ( ch->ss_data )
    {
        SHAPESHIFT_DATA* ss;
        ss = do_reform_silent ( ch );
        free_shape_data( ss );
        ch->ss_data = NULL;
    }

	if ( IS_NPC( ch ) )
    {
		name = ch->name2;
		corpse_vnum = ch->pIndexData->corpse_vnum;

		if ( EXT_IS_SET( ch->act, ACT_RAND_ITEMS ) && number_percent() < ch->level / 2 )
        {
			random_obj = randomize_item( UMIN( ch->level / 5, 7 ) );
        }

		if ( corpse_vnum > 0 && get_obj_index( corpse_vnum ) )
		{
			corpse = create_object( get_obj_index( corpse_vnum ), FALSE );
			custom_body = TRUE;
		}
		else if ( corpse_vnum == 0 )
        {
			corpse = create_object( get_obj_index( OBJ_VNUM_CORPSE_NPC ), FALSE );
        }
		else
        {
			corpse = NULL;
        }

        if ( corpse )
        {
            corpse->cost  = 0;
            corpse->timer = number_range( 4, 8 );
            /**
             * slow rot
             */
            if ( IS_SET( corpse->value[ 5 ], SLOW_ROT ) )
            {
                corpse->timer *= number_range( 10, 50 );
                corpse->timer += number_percent();
            }
            /**
             * no rot
             */
            if ( IS_SET( corpse->value[ 5 ], NO_ROT ) )
            {
                corpse->timer = 0;
            }
            /**
             * ustaw wage, jezeli nie jest ona zablokowana przez ustawienia
             * uwaga na kompatybilnosc wagi
             */
            if ( !IS_SET( corpse->value[ 5 ], STATIC_WEIGHT ) )
            {
                corpse->weight = 2*ch->weight;
            }

            /* do robienia szkieletkow i innego szajsu*/
            if ( corpse->item_type == ITEM_CORPSE_NPC && !EXT_IS_SET( ch->act, ACT_RAISED ))
            {
                corpse->value[ 0 ] = ch->pIndexData->vnum;
            }
            corpse->value[ 2 ] = ch->pIndexData->level;
        }

        /*
         * dorzucanie randomowych ksiag
         */
        i = UMAX( 1, ch->level/12 );
        while ( i > 0 )
        {
            random_obj_spellbook = generate_random_spellbook ( ch, i-- );
            if ( random_obj_spellbook == NULL )
            {
                continue;
            }
            if ( corpse )
            {
                switch ( corpse->item_type )
                {
                    case ITEM_CORPSE_NPC:
                    case ITEM_CORPSE_PC:
                    case ITEM_CONTAINER:
                        obj_to_obj( random_obj_spellbook, corpse );
                        break;
                    default:
                        obj_to_room( random_obj_spellbook, ch->in_room );
                        break;
                }
            }
            else
            {
                obj_to_room( random_obj_spellbook, ch->in_room );
            }
        }

        /**
         * przeniesienie kasy
         */
        long int copper = money_count_copper_all( ch );
        if ( copper > 0 )
        {
            if ( corpse )
            {
                switch ( corpse->item_type )
                {
                    case ITEM_CORPSE_NPC:
                    case ITEM_CORPSE_PC:
                    case ITEM_CONTAINER:
                        obj_to_obj( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), corpse );
                        break;
                    default:
                        obj_to_room( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), ch->in_room );
                        break;
                }
                /*
                 * dorzucanie przedmiotow randomowych celowo zapiete tylko dla mobow ze srebrem
                 */
                if ( random_obj )
                    switch ( corpse->item_type )
                    {
                        case ITEM_CORPSE_NPC:
                        case ITEM_CORPSE_PC:
                        case ITEM_CONTAINER:
                            obj_to_obj( random_obj, corpse );
                            break;
                        default:
                            obj_to_room( random_obj, ch->in_room );
                            break;
                    }
            }
            else
            {
                obj_to_room( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), ch->in_room );
                if ( random_obj )
                {
                    obj_to_room( random_obj, ch->in_room );
                }
            }
            money_reset_character_money( ch );
        }
    }
	else
    {
        /*artefact*/
        //all_artefact_from_char( ch );

        name           = ch->name2;
        corpse         = create_object( get_obj_index( OBJ_VNUM_CORPSE_PC ), FALSE );
        corpse->weight = 2 * ch->weight;
        corpse->timer  = number_range( 40, 65 );
        corpse->owner  = NULL;

        //rellik: na potrzeby wycinania ucha tutaj przechowujê imiê w³a¶ciciela cia³a i rasê
        corpse->hidden_description = strdup(ch->name2);

        corpse->value[ 0 ] = ch->level;
        corpse->value[ 1 ] = 0;
        corpse->value[ 2 ] = 0;
        corpse->value[ 3 ] = 0;
        corpse->value[ 4 ] = 0;
        corpse->value[ 5 ] = 0;
        corpse->value[ 6 ] = GET_RACE( ch );

        corpse->cost = 0;

        long int copper = money_count_copper_all( ch );
        if ( copper > 1 )
        {
            obj_to_obj( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), corpse );
            append_file_format_daily
                (
                 ch,
                 MONEY_LOG_FILE,
                 "-> S: %ld %ld (%ld) /%d/%d/%d/%d/, B: %ld %ld (%ld) - ¶mieræ",
                 copper,
                 0,
                 -copper,
                 ch->copper,
                 ch->silver,
                 ch->gold,
                 ch->mithril,
                 money_count_copper_all( ch ),
                 money_count_copper_all( ch ),
                 0
                );
            money_reset_character_money ( ch );
        }
    }

	if ( corpse && !custom_body )
	{
		sprintf( buf, corpse->short_descr, name );
		free_string( corpse->short_descr );
		corpse->short_descr = str_dup( buf );

		sprintf( buf, corpse->description, name );
		free_string( corpse->description );
		corpse->description = str_dup( buf );
	}

	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
		bool floating = FALSE;

		OBJ_NEXT_CONTENT( obj, obj_next );

		if ( obj->wear_loc == WEAR_FLOAT )
			floating = TRUE;

		/*artefact*/
		if ( is_artefact( obj ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
			artefact_from_char( obj, ch );
		obj_from_char( obj );

		if ( obj->item_type == ITEM_POTION )
			obj->timer = number_range( 500, 1000 );

		if ( obj->item_type == ITEM_SCROLL )
			obj->timer = number_range( 1000, 2500 );

		if ( IS_OBJ_STAT( obj, ITEM_ROT_DEATH ) )
		{
			if ( obj->contains != NULL )
			{
				OBJ_DATA * in, *in_next;

				act( "$p wyparowuje, pozostawiaj±c swoj± zawarto¶¶ wszêdzie dooko³a.",
						ch, obj, NULL, TO_ROOM );

				for ( in = obj->contains; in != NULL; in = in_next )
				{
					in_next = in->next_content;
					obj_from_obj( in );
					obj_to_room( in, ch->in_room );
				}
			}
			else
				act( "$p wyparowuje.", ch, obj, NULL, TO_ROOM );

			/*artefact*/
			if ( is_artefact( obj ) ) extract_artefact( obj );
			if ( obj->contains ) extract_artefact_container( obj );
			extract_obj( obj );
			continue;
		}

		EXT_REMOVE_BIT( obj->extra_flags, ITEM_VIS_DEATH );

		/* tylko mobiki */
		if ( IS_NPC( ch ) && obj->item_type == ITEM_SPELLBOOK )
		{
			int x, tmp;
			bool out = FALSE;

			for ( tmp = 2; tmp < 7; tmp++ )
				if ( obj->value[ 2 ] != 0 )
				{
					out = TRUE;
					break;
				}

			if ( !out )
				for ( tmp = 2, x = 0; x < 16; x++ )
				{
					if ( ch->pIndexData->spells[ x ] == 0 )
						break;

					if ( number_percent() < 50 )
					{
						obj->value[ tmp ] = ch->pIndexData->spells[ x ];
						tmp++;

						if ( tmp > 6 )
							break;
					}
				}
		}
		/* Ciala wypadaja na ziemie z pojemnikow... */
		if ( obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC )
		{
			if ( obj->contains != NULL )
			{
				OBJ_DATA * in, *in_next;
				for ( in = obj->contains; in != NULL; in = in_next )
				{
					in_next = in->next_content;
					if ( in->item_type == ITEM_CORPSE_PC || in->item_type == ITEM_CORPSE_NPC )
					{
						obj_from_obj( in );
						obj_to_room( in, ch->in_room );
						act( "$p upada na ziemiê.", ch, in, NULL, TO_ROOM );
					}
				}
			}
		}

		if ( IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
		{
			/*artefact*/
			if ( is_artefact( obj ) ) extract_artefact( obj );
			if ( obj->contains ) extract_artefact_container( obj );
			extract_obj( obj );
		}
		else if ( floating )
		{
			obj_to_room( obj, ch->in_room );
			act( "$p upada na ziemiê.", ch, obj, NULL, TO_ROOM );
		}
		else if ( obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC )
		{
			obj_to_room( obj, ch->in_room );
			act( "$p upada na ziemiê.", ch, obj, NULL, TO_ROOM );
		}
		else
		{
			if ( !corpse )
				obj_to_room( obj, ch->in_room );
			else
			{
				switch ( corpse->item_type )
				{
					case ITEM_CORPSE_NPC:
					case ITEM_CORPSE_PC:
					case ITEM_CONTAINER:
						obj_to_obj( obj, corpse );
						break;
					default:
						obj_to_room( obj, ch->in_room );
						break;
				}
			}
		}
	}


	if ( corpse )
		obj_to_room( corpse, ch->in_room );

	return;
}

/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
	ROOM_INDEX_DATA * was_in_room;
	char *msg;
	int door;

	if ( IS_NPC( ch ) )
		msg = "Powietrze przecina rozdzieraj±cy krzyk, chyba ktos w³a¶nie umar³...";
	else
		msg = "Ryk bólu prawie rani twoje uszy, zdaje siê, ¿e kto¶ lub co¶ ponios³o smieræ...";


	was_in_room = ch->in_room;
	for ( door = 0; door <= 5; door++ )
	{
		EXIT_DATA *pexit;

		if ( ( pexit = was_in_room->exit[ door ] ) != NULL
				&& pexit->u1.to_room != NULL
				&& pexit->u1.to_room != was_in_room )
		{
			ch->in_room = pexit->u1.to_room;
			act( msg, ch, NULL, NULL, TO_ROOM );
		}
	}
	ch->in_room = was_in_room;
	return;
}

void skill_loss( CHAR_DATA *ch, int handicap )
{
	int sn;
	int i;
	int wis_or_int = UMAX( get_curr_stat_deprecated( ch, STAT_WIS ), get_curr_stat_deprecated( ch, STAT_INT ) );

	for ( sn = 0; sn < MAX_SKILL; sn++ )
		if ( ( skill_table[sn].name != NULL )
				&&  ( ch->pcdata->learned[sn] > 0 ) && ( ch->pcdata->learning_rasz[sn] > 0 ) )
		{
			for ( i = 0; i < handicap; i++ )
				if ( ( ch->pcdata->learning_rasz[sn] * handicap ) + number_range( 20, 280 ) > wis_or_int * 10 )
				{
					ch->pcdata->learning_rasz[sn] = UMAX( 0, ch->pcdata->learning_rasz[sn]-1 );
					if ( ch->pcdata->learning_rasz[sn] < 0 )
						log_string( "skill_loss: bug z learning" );
				}
		}
	return;
}
void raw_kill( CHAR_DATA *victim, CHAR_DATA *ch )
{
	int i;
	AFFECT_DATA *aff, *aff_next, noob_new, *noob_old;
	bool has_silver = FALSE;
	OBJ_DATA *head, *head_next;
	char *name;
	char buf[ MAX_STRING_LENGTH ];

    /**
     * logowanie zabitych bossow
     */
    if ( EXT_IS_SET( victim->act, ACT_BOSS ) )
    {
        sprintf( buf, "[%d][boss death] %s (%d) killed by %s (%d) (%d) w %s.",
					victim->in_room ? victim->in_room->vnum : 0,
					victim->name,
					victim->level,
					( IS_NPC( ch ) ? ch->short_descr : ch->name ),
					ch->level,
					( IS_NPC( ch ) ? -1 : ch->class ),
					victim->in_room ? victim->in_room->name : "unknown"
			       );
        log_string( buf );
    }

	if ( victim->copper > 0 || victim->silver > 0 || victim->gold > 0 || victim->mithril > 0 )
    {
		has_silver = TRUE;
    }

	//tutaj robimy flage noob_killer by zliczala ile noobow zabil
	if( !IS_NPC(victim) && !IS_NPC(ch) && victim->level <= LEVEL_NEWBIE && ch->level > victim->level )
	{
		if( is_affected(ch,gsn_noob_killer) && ( noob_old = affect_find( ch->affected, gsn_noob_killer ) ) != NULL )
		{
			++noob_old->modifier;
			noob_old->duration = 10080; //im wiêcej, tym lepiej...tydzien jest ok
			noob_old->rt_duration = 0;
#ifdef NOOB_STAT_REMOVER
			//Rysand - walka z chamstwem - tymczasowe ciêcie statów
			if(number_range(0, 100) < (5 * (noob_old->modifier - 3)))
			{
				AFFECT_DATA af;
				af.where = TO_AFFECTS;
				af.type = gsn_none;
				af.level= 50;
				af.duration = 60*24;
				af.rt_duration = 0;
				af.location = number_range(1, 5);
				af.modifier = -noob_old->modifier; //ile noobów zabi³ taki minus do stata
				af.bitvector = &AFF_NONE;
				affect_to_char( ch, &af, NULL, TRUE );

				send_to_char( "Czujesz, ¿e dosiêgn±³ ciê gniew Bogów!\n\r", ch );
			}
#endif
		}
		else
		{
			noob_new.where = TO_AFFECTS;
			noob_new.type = gsn_noob_killer;
			noob_new.level = 50;
			noob_new.duration = 100;
			noob_new.rt_duration = 0;
			noob_new.location = APPLY_NONE;
			noob_new.modifier = 1;
			noob_new.bitvector = &AFF_NONE;
			affect_to_char( ch, &noob_new, NULL, FALSE );
		}
	}

	BOUNTY_DATA *bounty;
	//tutaj robimy trophy od bounty
	if ( !IS_NPC(ch) && !IS_NPC(victim) && victim->in_room && ( bounty = search_bounty ( victim->name ) ) != NULL )
	{
		name = victim->name2;

		head = create_object( get_obj_index( OBJ_VNUM_SEVERED_HEAD ), FALSE );
		head->timer	= number_range( 40, 70 );

		sprintf( buf, head->short_descr, name );
		free_string( head->short_descr );
		head->short_descr = str_dup( buf );

		sprintf( buf, "g³owy %s", victim->name2 );
		free_string( head->name2 );
		head->name2 = str_dup( buf );

		sprintf( buf, "g³owie %s", victim->name2 );
		free_string( head->name3 );
		head->name3 = str_dup( buf );

		sprintf( buf, "g³owê %s", victim->name2 );
		free_string( head->name4 );
		head->name4 = str_dup( buf );

		sprintf( buf, "g³ow± %s", victim->name2 );
		free_string( head->name5 );
		head->name5 = str_dup( buf );

		sprintf( buf, "g³owie %s", victim->name2 );
		free_string( head->name6 );
		head->name6 = str_dup( buf );

		sprintf( buf, head->description, name );
		free_string( head->description );
		head->description = str_dup( buf );
		head->item_type = ITEM_TROPHY;
		head->value[0] = bounty->value;
		rem_bounty( &misc.bounty_list, victim->name );
		obj_to_room( head, victim->in_room );

		//i tutaj czyszczonko coby przypadkiem nie zrobilo dwoch glow, ajkby juz jakas byla z make head
		for( head = victim->in_room->contents; head; head = head_next )
		{
			head_next = head->next_content;
			if( head->item_type == ITEM_TRASH && head->pIndexData->vnum == OBJ_VNUM_SEVERED_HEAD && head->value[5] == -17 )
				extract_obj( head );
		}
	}

	DEBUG_INFO( "raw_kill:group_gain" );
	group_gain( ch, victim );
	DEBUG_INFO( "raw_kill:stop_fighting" );
	stop_fighting( victim, TRUE );
	DEBUG_INFO( "raw_kill:death_cry" );
	death_cry( victim );
	DEBUG_INFO( "raw_kill:make_corpse" );
	make_corpse( victim );
	DEBUG_INFO( "raw_kill:clear_mem" );
	clear_mem( victim );
	DEBUG_INFO( "raw_kill:death_stats" );
	update_death_statistic( victim, ch );

	if ( IS_NPC( victim ) && !IS_NPC( ch ) && has_silver )
	{
		if ( EXT_IS_SET( ch->act, PLR_AUTOGOLD ) )
        {
			do_function( ch, &do_get, "monety cia³o" );
        }
	}

	if ( IS_NPC( victim ) )
	{
		DEBUG_INFO( "raw_kill:reorganize_group" );
		reorganize_mobile_group( victim );
		victim->pIndexData->killed++;
		kill_table[ URANGE( 0, victim->level, MAX_LEVEL - 1 ) ].killed++;
		DEBUG_INFO( "raw_kill:extract:pre" );
		extract_char( victim, TRUE );
		DEBUG_INFO( "raw_kill:extract:post" );
		return;
	}

	DEBUG_INFO( "raw_kill:remove_affects" );
	/* zdejmuje wszystkie affecty, oprocz zwiazanych ze smiercia*/
    for ( aff = victim->affected; aff; aff = aff_next )
    {
        aff_next = aff->next;
        if ( check_noremovable_affects( aff ) )
        {
            continue;
        }
        affect_remove( victim, aff );
    }

	DEBUG_INFO( "raw_kill:reseting_char" );
	for ( i = 0;i < MAX_COUNTER;i++ )
		victim->counter[ i ] = 0;

	for ( i = 0;i < MAX_VECT_BANK;i++ )
		victim->affected_by[ i ] = 0;

	for ( i = 0;i < MAX_LANG;i++ )
		victim->pcdata->lang_mod[ i ] = 0;

	for ( i = 0;i < MAX_SKILL;i++ )
		victim->pcdata->learn_mod[ i ] = 0;

	ext_flags_copy( ext_flags_sum( victim->affected_by, ext_flag_value( affect_flags, race_table[ GET_RACE(victim) ].aff ) ), victim->affected_by );

	for ( i = 0; i < 4; i++ )
		victim->armor[ i ] = 100;

	victim->position	= POS_RESTING;
	victim->hit	= UMAX( 1, victim->hit );
	victim->move	= victim->max_move / 2;
	victim->fight_data	= 0;
	check_player_death( victim, ch );
	DEBUG_INFO( "raw_kill:extract_dead_players" );
	extract_dead_player( victim );
	return;
}

void dam_message( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, bool immune )
{
	char buf1[ 256 ], buf2[ 256 ], buf3[ 256 ];
	const char *vs;
	const char *vp;
	const char *attack;
	char punct;
	int perfect_self;

	dam = calculate_final_damage( ch, victim, dam );

	if ( !IS_NPC(ch) )
	{
		dam *= 3;
		dam /= 2;
 	}

	save_debug_info("fight.c => dam_message", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

	if( IS_AFFECTED(ch,AFF_DEFLECT_WOUNDS)) dam /= 2;

	if ( ch == NULL || victim == NULL )
		return;

	if (is_affected( ch, gsn_perfect_self))
		perfect_self = number_range(5,30);
	else
		perfect_self = 0;

	if ( dt > 0 && dt < MAX_SKILL )
	{
		SPELL_MSG * msg;

		msg = msg_lookup( dt );
		if ( msg )
		{
			if ( immune )
			{
				act( msg->caster[ 4 ], ch, NULL, victim, TO_CHAR );
				act( msg->around[ 4 ], ch, NULL, victim, TO_ROOM );
			}
			else
			{
				if ( ch == victim )
				{
					if ( ch->hit - dam < -11 )
					{
						act_new( msg->caster[ 2 ], ch, NULL, victim, TO_CHAR, POS_DEAD );
						act( msg->around[ 2 ], ch, NULL, victim, TO_ROOM );
					}
					else
					{
						act_new( msg->caster[ 0 ], ch, NULL, victim, TO_CHAR, POS_DEAD );
						act( msg->around[ 0 ], ch, NULL, victim, TO_ROOM );
					}
				}
				else
				{
					if ( victim->hit - dam < -11 )
					{
						act( msg->caster[ 3 ], ch, NULL, victim, TO_CHAR );
						act_new( msg->victim[ 3 ], ch, NULL, victim, TO_VICT, POS_DEAD );
						act( msg->around[ 3 ], ch, NULL, victim, TO_NOTVICT );
					}
					else
					{
						act( msg->caster[ 1 ], ch, NULL, victim, TO_CHAR );
						act_new( msg->victim[ 1 ], ch, NULL, victim, TO_VICT, POS_DEAD );
						act( msg->around[ 1 ], ch, NULL, victim, TO_NOTVICT );
					}
				}
			}
			return;
		}
		else if ( special_damage_message( ch, victim, NULL, dam, dt, immune ) )
		{
			return;
		}
	}

	if ( dam + perfect_self == 0 )
	{
		vs = "chybiasz"; vp = "chybia";
	}
	else if ( dam + perfect_self <= 2 )
	{
		vs = "siniaczysz"; vp = "siniaczy";
	}
	else if ( dam + perfect_self <= 6 )
	{
		vs = "muskasz"; vp = "muska";
	}
	else if ( dam + perfect_self <= 10 )
	{
		vs = "ledwie ranisz"; vp = "ledwie rani";
	}
	else if ( dam + perfect_self <= 14 )
	{
		vs = "lekko ranisz"; vp = "lekko rani";
	}
	else if ( dam + perfect_self <= 18 )
	{
		vs = "ranisz"; vp = "rani";
	}
	else if ( dam + perfect_self <= 22 )
	{
		vs = "mocno ranisz"; vp = "mocno rani";
	}
	else if ( dam + perfect_self <= 26 )
	{
		vs = "dotkliwie ranisz"; vp = "dotkliwie rani";
	}
	else if ( dam + perfect_self <= 30 )
	{
		vs = "powa¿nie ranisz"; vp = "powa¿nie rani";
	}
	else if ( dam + perfect_self <= 34 )
	{
		vs = "masakrujesz"; vp = "masakruje";
	}
	else if ( dam + perfect_self <= 38 )
	{
		vs = "rozpruwasz"; vp = "rozpruwa";
	}
	else if ( dam + perfect_self <= 44 )
	{
		vs = "dewastujesz"; vp = "dewastuje";
	}
	else if ( dam + perfect_self <= 50 )
	{
		vs = "grzmocisz"; vp = "grzmoci";
	}
	else if ( dam + perfect_self <= 55 )
	{
		vs = "niszczysz"; vp = "niszczy";
	}
	else if ( dam + perfect_self <= 60 )
	{
		vs = "NISZCZYSZ"; vp = "NISZCZY";
	}
	else if ( dam + perfect_self <= 67 )
	{
		vs = "DRUZGOCZESZ"; vp = "DRUZGOCZE";
	}
	else if ( dam + perfect_self <= 75 )
	{
		vs = "ROZPRUWASZ"; vp = "ROZPRUWA";
	}
	else if ( dam + perfect_self <= 84 )
	{
		vs = "ROZRYWASZ"; vp = "ROZRYWA";
	}
	else if ( dam + perfect_self <= 100 )
	{
		vs = "ROZBEBESZASZ"; vp = "ROZBEBESZA";
	}
	else if ( dam + perfect_self <= 115 )
	{
		vs = "DEKAPITUJESZ"; vp = "DEKAPITUJE";
	}
	else if ( dam + perfect_self <= 130 )
	{
		vs = "EKSTYRPUJESZ"; vp = "EKSTYRPUJE";
	}
	else if ( dam + perfect_self <= 145 )
	{
		vs = "ANIHILUJESZ"; vp = "ANIHILUJE";
	}
	else if ( dam + perfect_self <= 200 )
	{
		vs = "U¦MIERCASZ"; vp = "U¦MIERCA";
	}
	else
	{
		vs = "UNICESTWIASZ"; vp = "UNICESTWIA";
	}

	punct = ( dam <= 24 ) ? '.' : '!';

	if ( dt == TYPE_HIT )
	{
		if ( ch == victim )
		{
			sprintf( buf1, "{3$n{3 %s {3sam siebie%c{x", vp, punct );
			sprintf( buf2, "{2%s sam siebie %c{x", vs, punct );
		}
		else
		{
			sprintf( buf1, "{3$n {3%s $C{3%c{x", vp, punct );
			sprintf( buf2, "{2%s $C{2%c{x", capitalize( vs ), punct );
			sprintf( buf3, "{4$n {4%s cie%c{x", vp, punct );
		}
	}
	else
	{
		if ( dt >= 0 && dt < MAX_SKILL )
			attack	= skill_table[ dt ].noun_damage;
		else if (( dt >= TYPE_HIT
				&& dt < TYPE_HIT + MAX_DAMAGE_MESSAGE ) )
			attack	= attack_table[ dt - TYPE_HIT ].noun;
		else
		{
			bug( "Dam_message: bad dt %d.", dt );
			dt = TYPE_HIT;
			attack = attack_table[ 0 ].name;
		}

		if ( immune )
		{
			if ( ch == victim )
			{
				switch ( ch->sex )
				{
				case 0:
					sprintf( buf1, "{3$n jest odporne na swój w³asny czar %s.{x", attack );
					sprintf( buf2, "{2Co za szczê¶cie, jeste¶ na to odporne.{x" );
					break;
				case 1:
					sprintf( buf1, "{3$n jest odporny na swój w³asny czar %s.{x", attack );
					sprintf( buf2, "{2Co za szczê¶cie, jeste¶ na to odporny.{x" );
					break;
				default:
					sprintf( buf1, "{3$n jest odporna na swój w³asny czar %s.{x", attack );
					sprintf( buf2, "{2Co za szczê¶cie, jeste¶ na to odporna.{x" );
					break;
				}
			}
			else
			{
				sprintf( buf1, "{3%s $z nic nie robi $X!{x", capitalize( attack ) );
				sprintf( buf2, "{2%s nic nie robi $X!{x", capitalize( attack ) );
				sprintf( buf3, "{4%s $z nic ci nie robi.{x", capitalize( attack ) );
			}
		}
		else
		{
			if ( ch == victim )
			{
				sprintf( buf1, "{3%s $z {3%s $m%c{x", capitalize( attack ), vp, punct );
				sprintf( buf2, "{2Twoje %s %s cie%c{x", attack, vp, punct );
			}
			else
			{
				sprintf( buf1, "{3%s $z {3%s $C%c{x", capitalize( attack ), vp, punct );
				sprintf( buf2, "{2Twoje %s %s $C{2%c{x", attack, vp, punct );
				sprintf( buf3, "{4%s $z {4%s cie%c{x", capitalize( attack ), vp, punct );
			}
		}
	}

	if ( ch == victim )
	{
		act( buf1, ch, NULL, NULL, TO_ROOM );
		act( buf2, ch, NULL, NULL, TO_CHAR );
	}
	else
	{
		act( buf1, ch, NULL, victim, TO_NOTVICT );
		act( buf2, ch, NULL, victim, TO_CHAR );
		act( buf3, ch, NULL, victim, TO_VICT );
	}

	return;
}

/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
	OBJ_DATA * obj;
	AFFECT_DATA *paf;

	if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
		obj = get_eq_char( victim, WEAR_SECOND );

	if ( obj == NULL ) return;


	if ( IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
	{
		act( "{5$S broñ nawet nie drgnie!{x", ch, NULL, victim, TO_CHAR );
		act( "{5$n próbuje ciê rozbroiæ, ale twoja broñ nawet nie drgnê³a!{x",
				ch, NULL, victim, TO_VICT );
		act( "{5$n próbuje rozbroiæ $C, ale mu siê nie udaje.{x", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	act( "{5$n ROZBRAJA ciê! Twoja broñ upada na ziemiê!{x",
			ch, NULL, victim, TO_VICT );
	char buf[ 256 ];
	sprintf(buf, "{5Zrêcznym ruchem rozbrajasz $C, wytr±caj±c %s z r±k!{x", can_see_obj(ch, obj) ? obj->name4 : "co¶");
	act( buf, ch, NULL, victim, TO_CHAR );
	act( "{5$n szybkim i zrêcznym ruchem rozbraja $C!{x", ch, NULL, victim, TO_NOTVICT );
	WAIT_STATE( victim, PULSE_VIOLENCE );

	/*artefact*/
	if ( is_artefact( obj ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
		artefact_from_char( obj, victim );
	obj_from_char( obj );
	if ( IS_OBJ_STAT( obj, ITEM_NODROP ) || IS_OBJ_STAT( obj, ITEM_INVENTORY ) )
	{
		/*artefact*/
		if ( is_artefact( obj ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
			artefact_to_char( obj, victim );
		obj_to_char( obj, victim );
	}
	else
	{
		obj_to_room( obj, victim->in_room );
		if ( IS_NPC( victim ) && victim->wait == 0 && can_see_obj( victim, obj ) )
			get_obj( victim, obj, NULL );
	}

	if( (paf = affect_find(ch->affected,gsn_weapon_wrench) ) )
	{
		affect_remove(ch,paf);
	}

	return;
}

void do_kill( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *victim;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Kogo zabiæ?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
#ifdef ENABLE_WAIT_AFTER_MISS
		WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
#endif
		return;
	}

	if ( victim == ch )
	{
		send_to_char( "Targasz siê na swoje ¿ycie!\n\r", ch );
		multi_hit( ch, ch, TYPE_UNDEFINED );
		return;
	}

	if ( is_safe( ch, victim ) )
		return;

	if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
	{
		act( "$N jest twoim ukochanym mistrzem.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( ch->position == POS_FIGHTING )
	{
		send_to_char( "Starasz siê jak tylko mo¿esz!\n\r", ch );
		return;
	}
	WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
	check_killer( ch, victim );
	multi_hit( ch, victim, TYPE_UNDEFINED );
	return;
}

void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA * was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;

    if ( ( victim = ch->fighting ) == NULL )
    {
        if ( ch->position == POS_FIGHTING )
        {
            ch->position = POS_STANDING;
        }
        send_to_char( "Z nikim nie walczysz.\n\r", ch );
        return;
    }

    if ( ch->position < POS_FIGHTING )
    {
        send_to_char( "Najpierw wstañ!\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_PARALYZE ) )
    {
        send_to_char( "Nie mo¿esz siê ruszyæ.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_BERSERK ) )
    {
        send_to_char( "Lepiej skup siê na walce.\n\r", ch );
        return;
    }

    if( IS_AFFECTED(ch, AFF_ENTANGLE ) )
    {
        send_to_char( "Ro¶liny opl±tuj± twe nogi, nie mo¿esz siê ruszyæ!\n\r", ch );
        return;
    }

    if( IS_AFFECTED(ch, AFF_WEB ) )
    {
        send_to_char( "Magiczna pajêczyna sklei³a twe nogi, nie mo¿esz siê ruszyæ!\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_HALLUCINATIONS_POSITIVE ) && number_range(1,8) != 1 )
    {
        switch(number_range(1,7))
        {
            case 1:
                send_to_char( "Uciekaæ? ale po co? kiedy¶ i tak zginiesz... To wszystko nie ma sensu...\n\r", ch );
                break;
            case 2:
                send_to_char( "¦ciany... dooko³a ciebie ¶ciany... Coraz bli¿ej... Zaraz ciê zgniot±.. Nie masz gdzie uciec!\n\r", ch );
                break;
            case 3:
                send_to_char( "Wielki jak stodo³a ogr zagradza ci drogê machaj±c ogromn± maczug±!\n\r", ch );
                break;
            case 4:
                send_to_char( "Zap³akany duch twej mamusi ukazuje ci siê i prosi, by¶ do niego do³±czy<&³/³a/³o>, bo jemu tak smutno... Bêdziesz walczyæ do koñca!\n\r", ch );
                break;
            case 5:
                send_to_char( "Ogarnia ciê taki ¿al, ¿e nagle odechciewa ci siê uciekaæ.\n\r", ch );
                break;
            case 6:
                send_to_char( "¦wiat jest tak niemo¿ebnie okrótny, ¿e nie ma dok±d i po co uciekaæ.\n\r", ch );
                break;
            case 7:
                send_to_char( "Muzyka stali przera¿a ciê do szpiku ko¶ci i nie dasz rady teraz uciec.\n\r", ch );
                break;
        }
        return;
    }

    if ( IS_AFFECTED( ch, AFF_HALLUCINATIONS_NEGATIVE ) && number_range(1,8) != 1 )
    {
        switch(number_range(1,7))
        {
            case 1:
                send_to_char( "Ale tu jest tak fajnie.. Nie masz ochoty nigdzie i¶æ.\n\r", ch );
                break;
            case 2:
                send_to_char( "Ju¿ masz zamiar uciec, ale zauwa¿asz ¶licznego motylka i zaczynasz za nim biec.\n\r", ch );
                break;
            case 3:
                send_to_char( "Jeste¶ zbyt zajêt<&y/a/e> zbieraniem kwiatków.\n\r", ch );
                break;
            case 4:
                send_to_char( "Czemu mamy przerywaæ zabawê? Ten du¿y pan jest taki milutki...\n\r", ch );
                break;
            case 5:
                send_to_char( "Doskonale siê bawisz, wiêc wcale tego nie chcesz.\n\r", ch );
                break;
            case 6:
                send_to_char( "Przecie¿ jest tutaj tak fajnie, dok±d i po co uciekaæ?\n\r", ch );
                break;
            case 7:
                send_to_char( "Muzyka stali wci±ga ciê doszczêtnie i nie chcesz sobie st±d i¶æ.\n\r", ch );
                break;
        }
        return;
    }

    // Gurthg: 2005-09-07, Nico, wielki czarnych twierdzi, ¿e to pomo¿e, bo torment, to wielka dopa³a
    // Kainti 2005 09 08 - To mialo blokowac flee tylko z walki, w ktorej zostal uzyty, a nie tak, ze gwiazka
    // przy tormencie blokuje flee wogole. Poprawiam.
    //	if ( is_affected(ch,gsn_torment ))
    if ( IS_SET( ch->fight_data, FIGHT_TORMENT_NOFLEE ) )
    {
        send_to_char( "Nie chcesz uciekaæ, chcesz zabijaæ ...\n\r", ch );
        return;
    }

    //Drake: Szamañski czar 'Ancestors Fury' blokuje uciekanie z walki przez ca³y czas swojego trwania. =D
    if ( IS_AFFECTED( ch, AFF_ANCESTORS_FURY) )
    {
        send_to_char( "Furia przodków wype³niaj±ca twoje ¿y³y domaga siê krwi!\n\r", ch );
        return;
    }

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
        EXIT_DATA *pexit;
        int door;

        door = number_door( );
        if ( ( pexit = was_in->exit[ door ] ) == 0
                || pexit->u1.to_room == NULL
                || IS_SET( pexit->exit_info, EX_CLOSED )
                || IS_SET( pexit->exit_info, EX_HIDDEN )
                || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER )
                || IS_SET( pexit->exit_info, EX_SECRET )
                || number_range( 0, ch->daze ) != 0
                || ( IS_AFFECTED(ch, AFF_LOOP) && number_range( 0, 12 ) != 0 )
                || IS_SET( pexit->exit_info, EX_NO_FLEE )
                || ( IS_NPC( ch )
                    && ( EXT_IS_SET( pexit->u1.to_room->room_flags, ROOM_NO_MOB )
                        || IS_SET( pexit->exit_info, EX_NO_MOB ) ) ) )
        {
            continue;
        }

        if ( is_affected(ch,gsn_glorious_impale) && get_eq_char(ch,WEAR_WIELD) && get_eq_char(ch,WEAR_WIELD)->value[0] == WEAPON_SPEAR )
        {
            affect_strip( ch, gsn_glorious_impale );
            if ( ch->fighting && IS_AFFECTED( ch->fighting, AFF_PARALYZE) && is_affected(ch->fighting, gsn_glorious_impale ) )
            {
                act( "Gwa³townie wyrywasz $h z cia³a $Z powiêkszaj±c jeszcze ranê.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_CHAR );
                act( "$n gwa³townie wyrywa $h z twojego cia³a powiêkszaj±c jeszcze ranê.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_VICT );
                act( "$n gwa³townie wyrywa $h z cia³a $Z powiêkszaj±c jeszcze ranê.", ch, get_eq_char( ch,WEAR_WIELD), ch->fighting, TO_NOTVICT );
                affect_strip( ch->fighting, gsn_glorious_impale );
                damage( ch, ch->fighting, 2*(dice(get_eq_char(ch,WEAR_WIELD)->value[1],get_eq_char(ch,WEAR_WIELD)->value[2])+get_eq_char(ch,WEAR_WIELD)->value[6]+GET_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))+GET_SKILL_DAMROLL( ch, get_eq_char(ch,WEAR_WIELD))),gsn_glorious_impale, DAM_PIERCE, FALSE );
            }
        }

        move_char( ch, door, FALSE, NULL );
        if ( ( now_in = ch->in_room ) == was_in )
        {
            continue;
        }

        ch->in_room = was_in;

        switch(number_range(1,11))
        {
            case 1:
                act( "$n wycofuje siê i wieje!", ch, NULL, NULL, TO_ROOM );
                break;
            case 2:
                act( "$n wycofuje siê z tego miejsca!", ch, NULL, NULL, TO_ROOM );
                break;
            case 3:
                act( "$n ucieka st±d!", ch, NULL, NULL, TO_ROOM );
                break;
            case 4:
                act( "$n bierze nogi za pas!", ch, NULL, NULL, TO_ROOM );
                break;
            case 5:
                act( "$n daje dyla!", ch, NULL, NULL, TO_ROOM );
                break;
            case 6:
                act( "$n pierzcha stad!", ch, NULL, NULL, TO_ROOM );
                break;
            case 7:
                act( "$n orientuje siê w sytuacji i czmycha niepostrze¿enie!", ch, NULL, NULL, TO_ROOM );
                break;
            case 8:
                act( "$n wycofuje siê i uchodzi!", ch, NULL, NULL, TO_ROOM );
                break;
            case 9:
                act( "$n wycofuje siê i umyka!", ch, NULL, NULL, TO_ROOM );
                break;
            case 10:
                act( "$n po krótkiej ocenie sytuacji ulatnia siê st±d!", ch, NULL, NULL, TO_ROOM );
                break;
            case 11:
            default:
                act( "$n wycofuje siê i ucieka!", ch, NULL, NULL, TO_ROOM );
                break;
        }

        ch->in_room = now_in;

        if ( !IS_NPC( ch ) )
        {
            send_to_char( "Uciekasz z walki!\n\r", ch );
            if ( ( ch->class == CLASS_THIEF ) && ( number_percent() < 3 * ( ch->level / 2 ) ) )
            {
                send_to_char( "Wycofujesz siê bezpiecznie.\n\r", ch );
            }
            else
            {
                send_to_char( "Tracisz troszkê punktów do¶wiadczenia.\n\r", ch );
                gain_exp( ch, - number_range( 5, 20 ), FALSE );
            }
        }

        if ( IS_NPC( ch ) && ch->hunting )
        {
            ch->hunting = NULL;
            start_fearing( ch, ch->fighting );

        }

        stop_fighting( ch, TRUE );
        return;
    }

    send_to_char( "WPADASZ W PANIKÊ! Nie uda³o ci siê uciec!\n\r", ch );
    return;
}

void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim;
    OBJ_DATA *obj;
    OBJ_DATA *wield, *second;
    int chance, ch_weapon, vict_weapon, ch_vict_weapon, mastery_bonus = 0;

    if ( ( chance = get_skill( ch, gsn_disarm ) ) == 0 )
    {
        send_to_char( "Nie znasz siê na tym.\n\r", ch );
        return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char( "Z nikim nie walczysz.\n\r", ch );
        return;
    }

    /*nie rozbroi sie golymi rekoma*/
    if ( get_eq_char( ch, WEAR_WIELD ) == NULL )
    {
        send_to_char( "Musisz trzymaæ broñ je¶li chcesz kogo¶ rozbroiæ.\n\r", ch );
        return;
    }

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL &&
            ( obj = get_eq_char( victim, WEAR_SECOND ) ) == NULL )
    {
        send_to_char( "Twój przeciwnik nie ma ¿adnej broni.\n\r", ch );
        return;
    }

    if( is_affected(victim,gsn_glorious_impale) && !affect_find(victim->affected,gsn_glorious_impale)->modifier && get_eq_char(victim,WEAR_WIELD) && get_eq_char(victim,WEAR_WIELD)->value[0] == WEAPON_SPEAR )
    {
        send_to_char( "To siê nie uda.\n\r", ch );
        return;
    }

    if ( IS_NPC( victim ) )
        chance /= 2;
    else
        chance /= 3;

    /* find weapon skills */
    ch_weapon = get_weapon_skill( ch, get_weapon_sn( ch, TRUE ) );
    vict_weapon = get_weapon_skill( victim, get_weapon_sn( victim, TRUE ) );
    ch_vict_weapon = get_weapon_skill( ch, get_weapon_sn( victim, TRUE ) );

    chance += ch_weapon/6;

    /*no i powiedzmy jakis plus jesil masz 2  bronie*/
    if ( get_eq_char( ch, WEAR_SECOND ) != NULL )
    {
        chance += 8;
    }

    /*oraz bonusik dla whip'ow*/
    /*i niewielki dla short swordow*/
    wield = get_eq_char( ch, WEAR_WIELD );
    second = get_eq_char( ch, WEAR_SECOND );

    if ( wield->value[ 0 ] == WEAPON_WHIP && second && second->value[ 0 ] == WEAPON_WHIP )
    {
        chance += 15;
    }
    else if ( wield->value[ 0 ] == WEAPON_WHIP )
    {
        chance += 10;
    }
    else if ( second && second->value[ 0 ] == WEAPON_WHIP )
    {
        chance += 5;
    }

    if ( wield->value[ 0 ] == WEAPON_SHORTSWORD && second && second->value[ 0 ] == WEAPON_SHORTSWORD )
    {
        chance += 5;
    }
    else if ( wield->value[ 0 ] == WEAPON_SHORTSWORD )
    {
        chance += 3;
    }
    else if ( second && second->value[ 0 ] == WEAPON_SHORTSWORD )
    {
        chance += 2;
    }

    /* jak disarmujacy umie walczyc bronia przeciwnika, to dobrze :) */
    if( ch_vict_weapon > vict_weapon )
    {
        chance += ( ch_vict_weapon - vict_weapon ) / 4;
    }
    else
    {
        chance -= ( vict_weapon - ch_vict_weapon ) / 4;
    }

    /**
     * bonus od masterki
     */
    switch ( wield->value[ 0 ] )
    {
        case WEAPON_SWORD:
            mastery_bonus = get_skill( ch, gsn_sword_mastery );
            break;
        case WEAPON_DAGGER:
            mastery_bonus = get_skill( ch, gsn_dagger_mastery );
            break;
        case WEAPON_SPEAR:
            mastery_bonus = get_skill( ch, gsn_spear_mastery );
            break;
        case WEAPON_MACE:
            mastery_bonus = get_skill( ch, gsn_mace_mastery );
            break;
        case WEAPON_AXE:
            mastery_bonus = get_skill( ch, gsn_axe_mastery );
            break;
        case WEAPON_FLAIL:
            mastery_bonus = get_skill( ch, gsn_flail_mastery );
            break;
        case WEAPON_WHIP:
            mastery_bonus = get_skill( ch, gsn_whip_mastery );
            break;
        case WEAPON_POLEARM:
            mastery_bonus = get_skill( ch, gsn_polearm_mastery );
            break;
        case WEAPON_STAFF:
            mastery_bonus = get_skill( ch, gsn_staff_mastery );
            break;
        case WEAPON_SHORTSWORD:
            mastery_bonus = get_skill( ch, gsn_shortsword_mastery );
            break;
    }
    if ( mastery_bonus > 0 )
    {
        if ( second && wield->value[ 0 ] == second->value[0] )
        {
            mastery_bonus *= 4;
            mastery_bonus /= 3;
        }
        chance += mastery_bonus/10;
    }

    /* dex disarmujacego vs. strength disarmowanego */
    chance += (get_curr_stat_deprecated( ch, STAT_DEX )-10)*2;
    chance -= (get_curr_stat_deprecated( victim, STAT_STR )-10)*2;

    /* roznica leveli */
    chance += ch->level;
    chance -= victim->level;

    if( IS_WEAPON_STAT( obj, WEAPON_TWO_HANDS ) )
    {
        chance -= chance/4;
    }

    chance = URANGE( 4, chance, 95 );

    if( IS_NPC(victim ) && EXT_IS_SET( victim->off_flags, OFF_DISARMPROOF ) )
    {
        chance = 0;
    }

    /* and now the attack */
    if ( chance && number_percent() < chance )
    {
        WAIT_STATE( ch, skill_table[ gsn_disarm ].beats );
        disarm( ch, victim );
        SET_BIT( ch->fight_data, FIGHT_DISARM );
        check_improve( ch, NULL, gsn_disarm, TRUE, 50 );
    }
    else
    {
        WAIT_STATE( ch, skill_table[ gsn_disarm ].beats );
        act( "{5Nie uda³o ci siê rozbroiæ $Z.{x", ch, NULL, victim, TO_CHAR );
        act( "{5$n próbuje ciê rozbroiæ, bez skutku.{x", ch, NULL, victim, TO_VICT );
        switch ( ch->sex )
        {
            case 0:
                act( "{5$n próbuje rozbroiæ $C, ale temu siê nie udaje.{x", ch, NULL, victim, TO_NOTVICT );
                break;
            case 1:
                act( "{5$n próbuje rozbroiæ $C, ale mu siê nie udaje.{x", ch, NULL, victim, TO_NOTVICT );
                break;
            default :
                act( "{5$n próbuje rozbroiæ $C, ale jej siê nie udaje.{x", ch, NULL, victim, TO_NOTVICT );
                break;
        }
        check_improve( ch, NULL, gsn_disarm, FALSE, 55 );
    }
    check_killer( ch, victim );
    return;
}

void do_surrender( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * mob;

	if ( ( mob = ch->fighting ) == NULL )
	{
		send_to_char( "Z nikim nie walczysz!\n\r", ch );
		return;
	}


	act( "Rzucasz siê na kolana i b³agasz $C o lito¶æ!", ch, NULL, mob, TO_CHAR );
	WAIT_STATE( ch, 14 );
	if ( IS_AFFECTED( ch, AFF_SILENCE ) )
	{
		act( "$n pada na kolana w pokornym ge¶cie poddania!", ch, NULL, mob, TO_VICT );
		act( "$n rzuca siê na kolana przed $V w pokornym ge¶cie poddania!", ch, NULL, mob, TO_NOTVICT );
	}
	else
	{
		act( "$n pada na kolana i b³aga o lito¶æ!", ch, NULL, mob, TO_VICT );
		act( "$n rzuca siê na kolana i b³aga $C o lito¶æ!", ch, NULL, mob, TO_NOTVICT );
	}
	//stop_fighting( ch, TRUE );

	if ( !IS_NPC( ch ) && IS_NPC( mob )
			&& ( !HAS_TRIGGER( mob, TRIG_SURR )
				|| !mp_percent_trigger( mob, ch, NULL, NULL, &TRIG_SURR ) ) )
	{
		act( "$N nie zwraca na to uwagi!", ch, NULL, mob, TO_CHAR );
		multi_hit( mob, ch, TYPE_UNDEFINED );
	}
	else if ( !IS_NPC( ch ) && !IS_NPC( mob ) && !EXT_IS_SET( mob->act, PLR_ACCEPTSURR ) )
	{
		act( "$N nie zwraca na to uwagi!", ch, NULL, mob, TO_CHAR );
		multi_hit( mob, ch, TYPE_UNDEFINED );
	}
	else if ( IS_NPC(ch) && IS_NPC(mob) && ch->master != mob->master )
	{
		multi_hit( mob, ch, TYPE_UNDEFINED );
	}
	else
	{
		stop_fighting( ch, TRUE );
	}
	return;
}

void do_sla( CHAR_DATA *ch, char *argument )
{
	send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
	return;
}

void do_slay( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * victim;
	char arg[ MAX_INPUT_LENGTH ];

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Slay whom?\n\r", ch );
		return;
	}

	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
		send_to_char( "They aren't here.\n\r", ch );
		return;
	}

	if ( ch == victim )
	{
		send_to_char( "U¶miercasz sa<&m/ma/me> siebie!\n\r", ch );
		raw_kill( victim, ch );
		return;
	}

	if ( !IS_NPC( victim ) && victim->level >= get_trust( ch ) )
	{
		send_to_char( "You failed.\n\r", ch );
		return;
	}

	act( "{1U¶miercasz $M z zimn± krwi±!{x", ch, NULL, victim, TO_CHAR );
	act( "{1$n u¶mierca ciê z zimn± krwi±!{x", ch, NULL, victim, TO_VICT );
	act( "{1$n u¶mierca $C z zimn± krwi±!{x", ch, NULL, victim, TO_NOTVICT );
	raw_kill( victim, ch );
	return;
}

void update_obj_cond( CHAR_DATA *ch, OBJ_DATA *weapon, int base_dam, int dam, CHAR_DATA *victim )
{
    int material = 0;
    if ( weapon && weapon->item_type == ITEM_WEAPON && !IS_OBJ_STAT( weapon, ITEM_UNDESTRUCTABLE ) )
    {
        /* bazowa szansa prz kazdym uderzeniu*/
        int chance = 2;
        int x;
        int hardness = 0;

        material = UMAX( 0, weapon->material );
        hardness = UMIN( 95, material_table[ material ].hardness );

        for ( x = 0; material_table[ x ].name; x++ )
        {
            if ( x == material )
            {
                break;
            }
        }

        if ( material > x )
        {
            return;
        }

        chance += ( 100 - hardness ) / 10;

        /* jesli uderzenie bylo wieksze niz 3/4 maksa obrazen z broni szansa wieksza*/
        if ( base_dam > ( 75 * ( weapon->value[ 1 ] * weapon->value[ 2 ] + weapon->value[ 6 ] ) / 100 ) )
        {
            chance += number_range(1,3);
        }

        if ( EXT_IS_SET( weapon->extra_flags, ITEM_NOREPAIR ) )
        {
            chance--;
        }

        if ( is_artefact( weapon ) && number_percent() > 20 )
        {
            chance--;
        }

        if ( chance > 0 && number_percent() < chance && number_percent() > hardness  )
        {
            weapon->condition -= ( number_percent() > hardness )? number_range( 1, 3 ) : 1;
            if ( weapon->condition < 1 )
            {
                /*artefact*/
                if ( is_artefact( weapon ) ) extract_artefact( weapon );
                destroy_obj( weapon->carried_by, weapon );
                return;
            }
        }
    }

    if ( victim && dam > 0 )
    {
        int where = -1, chance, x;
        OBJ_DATA *armor;

        switch ( number_range( 1 , 20 ) )
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                where = WEAR_SHIELD;
                if ( get_eq_char( victim, where ) )
                    break;
            case 6:
            case 7:
            case 8:
            case 9:
            case 10: where = WEAR_BODY;break;
            case 11:
            case 12:
            case 13: where = WEAR_ARMS;break;
            case 14:
            case 15:
            case 16: where = WEAR_LEGS;break;
            case 17:
            case 18: where = WEAR_HEAD;break;
            case 19:
            case 20: where = WEAR_HANDS;break;
            default: break;
        }

        if ( where != -1 )
        {
            armor = get_eq_char( victim, where );
            if ( armor && !IS_OBJ_STAT( armor, ITEM_UNDESTRUCTABLE ) && ( armor->item_type == ITEM_ARMOR || armor->item_type == ITEM_CLOTHING ) )
            {
                chance = dam / 2;

                material = UMAX( 0, armor->material );

                for ( x = 0; material_table[ x ].name; x++ )
                    if ( x == material )
                        break;

                if ( material > x )
                    return;

                if ( is_artefact( armor ) )
                {
                    chance /= 2;
                }

                if ( number_percent() < chance &&
                        number_percent() > material_table[ armor->material ].hardness )
                {
                    armor->condition -= ( number_percent() > material_table[ armor->material ].hardness )? number_range( 1, 3 ) : 1;
                    if ( armor->condition < 1 )
                    {
                        /*artefact*/
                        if ( is_artefact( armor ) ) extract_artefact( armor );
                        destroy_obj( armor->carried_by, armor );
                        return;
                    }
                }
            }
        }
    }

    return;
}

SPELL_MSG* msg_lookup(sh_int sn)
{
	SPELL_MSG *tmp;

	for(tmp=spellmsg_list;tmp;tmp=tmp->next)
		if(tmp->sn==sn)
			return tmp;

	return NULL;
}

static sh_int skill_weapon_mod_table[MAX_CLASS][2][6]=
{
	{//Mag
		{ -2, 0,  1,  1,  2,  2 },
		{ -1, 0,  0,  1,  1,  2 }
	},

	{//Kleryk
		{ -1,  0,  1,  1,  2,  3 },
		{ -1,  0,  1,  2,  2,  3 }
	},

	{//Zlodziej
		{ -1,  0,  1,  2,  3,  3 },
		{ -1,  0,  1,  2,  2,  3 }
	},

	{//Wojownik
		{  0,  1,  2,  3,  4,  5 },
		{ -1,  0,  1,  2,  3,  3 }
	},

	{//Paladyn
		{  0,  0,  2,  2,  3,  3 },
		{ -1,  0,  1,  2,  2,  3 }
	},

	{//Druid
		{ -1,  0,  1,  1,  2,  3 },
		{ -1,  0,  1,  2,  2,  3 }
	},

	{//Barbarzynca
		{ -1,  0,  1,  1,  2,  2 },
		{ -1,  1,  2,  3,  4,  4 }
	},

	{//Mnich
		{  0,  1,  2,  3,  4,  5 },
		{ -2,  1,  1,  1,  2,  3 }
	},

	{//Bard
		{ -1,  0,  1,  2,  3,  3 },
		{ -1,  0,  1,  2,  2,  2 }
	},

	{//Czarny rycerz
		{  0,  0,  2,  2,  3,  3 },
		{ -1,  0,  1,  2,  2,  3 }
	},

	{//Szaman
		{ -1,  0,  1,  1,  2,  3 },
		{ -1,  0,  1,  2,  2,  3 }
	},

};

int weapon_skill_mod( CHAR_DATA *ch, bool primary, bool hitr )
{
	sh_int sn, skill, val = -1;

	if ( !ch )
		return 0;

	if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_MONK ) ) ||
			( !IS_NPC( ch ) && ch->class == CLASS_MONK ) )
	{
		skill = get_skill( ch, gsn_unarmed_strike );

		if ( skill >= 100 )
			val = 5;
		else if ( skill >= 85 )
			val = 4;
		else if ( skill >= 65 )
			val = 3;
		else if ( skill >= 45 )
			val = 2;
		else if ( skill >= 25 )
			val = 1;
		else if ( skill >= 15 )
			;
		else
			val = 0;

		if ( val < 0 )
			return 0;

		if ( hitr )
			return skill_weapon_mod_table[ ch->class ][ 0 ][ val ];
		else
			return skill_weapon_mod_table[ ch->class ][ 1 ][ val ];
	}

	sn = get_weapon_sn( ch, primary );

	if ( sn < 0 )
		return 0;

	skill = get_weapon_skill( ch, sn );

	if ( primary )
	{
		if( IS_WEAPON_STAT( get_eq_char( ch, WEAR_WIELD ), WEAPON_TWO_HANDS ) )
		{
			skill += get_skill( ch, gsn_two_hands_fighting );
			skill /= 2;
		}
	}

	if ( skill >= 100 )
		val = 5;
	else if ( skill >= 85 )
		val = 4;
	else if ( skill >= 65 )
		val = 3;
	else if ( skill >= 45 )
		val = 2;
	else if ( skill >= 25 )
		val = 1;
	else if ( skill >= 15 )
		;
	else
		val = 0;

	if ( val < 0 )
		return 0;

	if ( hitr )
		return skill_weapon_mod_table[ ch->class ][ 0 ][ val ];
	else
		return skill_weapon_mod_table[ ch->class ][ 1 ][ val ];
};

bool check_stone_skin( CHAR_DATA *ch, CHAR_DATA *victim, int dam_type, int *dam, OBJ_DATA *weapon )
{
	AFFECT_DATA *stone = NULL;
	int d;
	bool ret = FALSE;

	if ( !IS_AFFECTED( victim, AFF_STONE_SKIN ) || !is_affected( victim, gsn_stone_skin ) )
		return FALSE;

	if ( dam_type != DAM_BASH && dam_type != DAM_PIERCE && dam_type != DAM_SLASH )
		return FALSE;

	d = *dam;

	stone = affect_find( victim->affected, gsn_stone_skin );

	if ( !stone )
		return FALSE;

	if ( stone->modifier > 0 )
	{
		if( !IS_NPC(victim) && victim->pcdata->mage_specialist == 1 )
		{
			if ( ( weapon && IS_WEAPON_STAT( weapon, WEAPON_VORPAL ) ) ||
					( ch && IS_NPC( ch ) && IS_SET( ch->attack_flags, WEAPON_VORPAL ) ) )
				d = d / 2;
			else if ( d > 70 )
				d = d / 3;
			else
			{
				d = 0;
				ret = TRUE;
			}
		}
		else
		{
			if ( ( weapon && IS_WEAPON_STAT( weapon, WEAPON_VORPAL ) ) ||
					( ch && IS_NPC( ch ) && IS_SET( ch->attack_flags, WEAPON_VORPAL ) ) )
				;
			else if ( d > 50 )
				d = d / 2;
			else
			{
				d = 0;
				ret = TRUE;
			}
		}
	}

	stone->modifier--;

	if ( stone->modifier <= 0 )
	{
		if ( stone->type > 0 && skill_table[ stone->type ].msg_off )
		{
			send_to_char( skill_table[ stone->type ].msg_off, victim );
			send_to_char( "\n\r", victim );
		}

		affect_remove( victim, stone );
		affect_strip( victim, gsn_stone_skin );
	}

	*dam = d;
	return ret;
}


bool check_increase_wounds( CHAR_DATA *victim, int dam_type, int *dam )
{
	AFFECT_DATA * paf, *spell = NULL;
	int d, level;

	if ( !IS_AFFECTED( victim, AFF_INCREASE_WOUNDS ) )
		return FALSE;

	if ( dam_type != DAM_BASH &&
			dam_type != DAM_PIERCE &&
			dam_type != DAM_SLASH )
		return FALSE;

	d = *dam;

	for ( paf = victim->affected; paf != NULL; paf = paf->next )
		if ( paf->type == gsn_increase_wounds )
		{
			spell = paf;
			break;
		}

	/* jesli ma flage, to tylko obrazenia +25% */
	if ( !spell )
		level = 25;
	else
	{
		level = spell->modifier;

		spell->duration--;

		if ( spell->duration < 0 )
		{
			if ( spell->type > 0 && spell->type < MAX_SKILL && skill_table[ spell->type ].msg_off )
			{
				send_to_char( skill_table[ spell->type ].msg_off, victim );
				send_to_char( "\n\r", victim );
			}

			affect_remove( victim, spell );
		}
	}

	d += ( ( d * level ) / 100 );
	*dam = d;
	return TRUE;
}

bool check_mirror_image( CHAR_DATA *victim, CHAR_DATA *ch )
{
	AFFECT_DATA *paf, *mirror = NULL;
	AFFECT_DATA aff;
	bool ret = FALSE;
	int chance;
	int cyferka_bijacego = 0, cyferka_omirrorowanego = 0;

	if ( IS_AFFECTED( ch, AFF_PIERCING_SIGHT ) )
		return FALSE;

	if ( !IS_AFFECTED( victim, AFF_MIRROR_IMAGE ) || ch == victim )
		return FALSE;

	for ( paf = victim->affected; paf != NULL; paf = paf->next )
		if ( paf->type == gsn_mirror_image )
		{
			mirror = paf;
			break;
		}

	if ( !mirror )
		return FALSE;

	chance = mirror->modifier;

	cyferka_bijacego += number_range( 1, get_curr_stat_deprecated( ch, STAT_LUC ) );
	cyferka_bijacego += number_range( 1, get_curr_stat_deprecated( ch, STAT_WIS ) );
	cyferka_bijacego += number_range( 1, get_curr_stat_deprecated( ch, STAT_INT ) );
	cyferka_bijacego += number_range( 1, ch->level );

	cyferka_omirrorowanego += number_range( 1, get_curr_stat_deprecated( victim, STAT_LUC ) );
	cyferka_omirrorowanego += number_range( 1, get_curr_stat_deprecated( victim, STAT_INT ) );
	cyferka_omirrorowanego += number_range( 1, mirror->level );
	cyferka_omirrorowanego += chance;

	if( !IS_NPC(victim) && victim->class == CLASS_MAG && victim->pcdata->mage_specialist == 5 )
		cyferka_omirrorowanego += number_range( 5, 8 );

	if ( cyferka_bijacego > cyferka_omirrorowanego )
		return FALSE;

	if ( mirror->modifier > 0 )
	{
		print_char( ch, "Jedno z lustrzanych odbiæ %s zniknê³o.\n\r", PERS2( victim, ch ) );
		send_to_char( "Jedno z twoich odbiæ zniknê³o.\n\r", victim );

		if ( is_affected(victim, gsn_mirrorfall) )
			affect_strip(victim, gsn_mirrorfall);
		aff.where	= TO_AFFECTS;
		aff.type	= gsn_mirrorfall;
		aff.level	= 1;
		aff.duration	= 1;
		aff.rt_duration = 0;
		aff.bitvector = &AFF_NONE;
		aff.location	= APPLY_NONE;
		aff.modifier	= 0;
		affect_to_char( victim, &aff, NULL, FALSE );

		ret = TRUE;
	}

	mirror->modifier--;


	if ( mirror->modifier <= 0 )
	{
		if ( mirror->type > 0 && skill_table[ mirror->type ].msg_off )
		{
			send_to_char( skill_table[ mirror->type ].msg_off, victim );
			send_to_char( "\n\r", victim );
		}

		affect_remove( victim, mirror );
	}

	return ret;
}

bool check_blink( CHAR_DATA *victim, CHAR_DATA *ch, bool unik )
{
	AFFECT_DATA * paf, *blink = NULL;

	if ( !is_affected( victim, gsn_blink ) || ch == victim )
		return FALSE;

	for ( paf = victim->affected; paf != NULL; paf = paf->next )
		if ( paf->type == gsn_blink )
		{
			blink = paf;
			break;
		}

	if ( !blink )
		return FALSE;

	if ( unik )
	{
		if ( number_percent() < blink->modifier )
			return TRUE;
	}
	else
	{
		if ( number_percent() < blink->level )
			return TRUE;
	}

	return FALSE;
}

bool still_fighting( CHAR_DATA *ch )
{
	CHAR_DATA * vch;

	if ( !ch->in_room )
		return FALSE;

	if ( ch->fighting )
		return TRUE;

	if ( ch->position < POS_RESTING )
		return FALSE;

	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
		if ( vch->fighting && vch->fighting == ch )
		{
			ch->fighting = vch;
			return TRUE;
		}

	return FALSE;
}


/*
 * Inflict magic damage from a spell. */
bool spell_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int sn, int dam_type, bool show )
{
	OBJ_DATA * obj, *obj_next, *wand;
	AFFECT_DATA *defense_curl;
	bool immune, wand_pierwszy_val = FALSE;
	int dam_mod = 0, dam_base = dam;

	//sciecie sily wszelkich fragow graczy 28 luty 2005 - Kainti
	//i przypakowanie dla mobow
	//2008-11-20, Brohacz: wywalam sciecie fragow graczy
	if ( IS_NPC( ch ) )
		dam += dam/8;

	//i tutaj nowe rozdzki, troche tego kodu jest :/
	wand = get_eq_char( ch,WEAR_HOLD );
	if( !IS_NPC(ch) && wand != NULL && wand->item_type == ITEM_WAND )
	{
		if( wand->value[0] == WAND_DAM_PERCENT )
		{
			wand_pierwszy_val = TRUE;
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					dam_mod += (wand->value[1]*dam)/100;
				else
					dam_mod -= (wand->value[1]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					dam_mod += (wand->value[1]*dam)/100;
				else
					dam_mod -= (wand->value[1]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					dam_mod += (wand->value[1]*dam)/100;
				else
					dam_mod -= (wand->value[1]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					dam_mod += (wand->value[1]*dam)/100;
				else
					dam_mod -= (wand->value[1]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					dam_mod += (wand->value[1]*dam)/100;
				else
					dam_mod -= (wand->value[1]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					dam_mod += (wand->value[1]*dam)/100;
				else
					dam_mod -= (wand->value[1]*dam)/100;
			}
			else
				dam_mod += (wand->value[1]*dam)/100;
		}
		else if( wand->value[2] == WAND_DAM_PERCENT )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					dam_mod += (wand->value[3]*dam)/100;
				else
					dam_mod -= (wand->value[3]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					dam_mod += (wand->value[3]*dam)/100;
				else
					dam_mod -= (wand->value[3]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					dam_mod += (wand->value[3]*dam)/100;
				else
					dam_mod -= (wand->value[3]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					dam_mod += (wand->value[3]*dam)/100;
				else
					dam_mod -= (wand->value[3]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					dam_mod += (wand->value[3]*dam)/100;
				else
					dam_mod -= (wand->value[3]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					dam_mod += (wand->value[3]*dam)/100;
				else
					dam_mod -= (wand->value[3]*dam)/100;
			}
			else
				dam_mod += (wand->value[3]*dam)/100;
		}
		else if( wand->value[4] == WAND_DAM_PERCENT )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					dam_mod += (wand->value[5]*dam)/100;
				else
					dam_mod -= (wand->value[5]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					dam_mod += (wand->value[5]*dam)/100;
				else
					dam_mod -= (wand->value[5]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					dam_mod += (wand->value[5]*dam)/100;
				else
					dam_mod -= (wand->value[5]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					dam_mod += (wand->value[5]*dam)/100;
				else
					dam_mod -= (wand->value[5]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					dam_mod += (wand->value[5]*dam)/100;
				else
					dam_mod -= (wand->value[5]*dam)/100;
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					dam_mod += (wand->value[5]*dam)/100;
				else
					dam_mod -= (wand->value[5]*dam)/100;
			}
			else
				dam_mod += (wand->value[5]*dam)/100;
		}

		if ( dam_mod != 0 && !IS_NPC(ch) )
		{
			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && !IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && !IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && !IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && !IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && !IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && !IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && !IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && !IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}
		}

		dam += dam_mod;
		dam_mod = 0;

		wand_pierwszy_val = FALSE;
		if( wand->value[0] == WAND_DAM_VALUE )
		{
			wand_pierwszy_val = TRUE;
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					dam_mod += wand->value[1];
				else
					dam_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					dam_mod += wand->value[1];
				else
					dam_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					dam_mod += wand->value[1];
				else
					dam_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					dam_mod += wand->value[1];
				else
					dam_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					dam_mod += wand->value[1];
				else
					dam_mod -= wand->value[1];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					dam_mod += wand->value[1];
				else
					dam_mod -= wand->value[1];
			}
			else
				dam_mod += wand->value[1];
		}
		else if( wand->value[2] == WAND_DAM_VALUE )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					dam_mod += wand->value[3];
				else
					dam_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					dam_mod += wand->value[3];
				else
					dam_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					dam_mod += wand->value[3];
				else
					dam_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					dam_mod += wand->value[3];
				else
					dam_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					dam_mod += wand->value[3];
				else
					dam_mod -= wand->value[3];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					dam_mod += wand->value[3];
				else
					dam_mod -= wand->value[3];
			}
			else
				dam_mod += wand->value[3];
		}
		else if( wand->value[4] == WAND_DAM_VALUE )
		{
			if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
			{
				if( IS_GOOD( ch ) )
					dam_mod += wand->value[5];
				else
					dam_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
			{
				if( IS_NEUTRAL( ch ) )
					dam_mod += wand->value[5];
				else
					dam_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
			{
				if( IS_EVIL( ch ) )
					dam_mod += wand->value[5];
				else
					dam_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
			{
				if( ch->class == CLASS_MAG )
					dam_mod += wand->value[5];
				else
					dam_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
			{
				if( ch->class == CLASS_CLERIC )
					dam_mod += wand->value[5];
				else
					dam_mod -= wand->value[5];
			}
			else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
			{
				if( ch->class == CLASS_DRUID )
					dam_mod += wand->value[5];
				else
					dam_mod -= wand->value[5];
			}
			else
				dam_mod += wand->value[5];
		}

		if ( dam_mod != 0 && !IS_NPC(ch) )
		{
			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
			{
				if ( !wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && !IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && !IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && !IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && !IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && !IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && !IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && !IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && !IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
			{
				if ( wand_pierwszy_val )
					dam_mod = 0;
			}
		}
	}
	dam += dam_mod;

	if( dam <= 0 )
		dam = 1;

	if ( victim->position == POS_DEAD )
		return FALSE;

	if ( victim != ch )
	{
		if ( is_safe( ch, victim ) )
			return FALSE;


		if ( victim->position > POS_STUNNED )
		{
			if ( victim->fighting == NULL )
			{
				set_fighting( victim, ch );

				if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
					mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_KILL );
			}
		}

		if ( victim->master == ch )
		{
			if( IS_NPC( victim ) && IS_AFFECTED(victim,AFF_CHARM) )
			{
				act( "Wiê¼ miêdzy tob± a $V zostaje brutalnie przerwana!", ch, NULL, victim, TO_CHAR );
				raw_damage( ch, ch, number_range( ch->level/2, ch->level+ch->level/2 ) );
				if ( ch->position <= POS_STUNNED )
					return FALSE;
			}
			stop_follower( victim );
		}
	}

	/*
	 * Inviso attacks ... not.
	 */
	if ( IS_AFFECTED( ch, AFF_HIDE ) )
		affect_strip( ch, gsn_hide );

	strip_invis( ch, TRUE, TRUE );

	if ( IS_NPC( victim ) && !is_fearing( victim, ch ) )
	{
		if ( !EXT_IS_SET( victim->act, ACT_SENTINEL ) )
			start_hunting( victim, ch );

		start_hating( victim, ch );
	}

	immune = check_blink( victim, ch, TRUE );

	if( !immune )
		immune = check_mirror_image( victim, ch );

	if ( dam == 0 || immune )
	{
		if ( show )
			dam_message( ch, victim, dam, sn, immune );

		return FALSE;
	}

	immune = IS_AFFECTED( victim, AFF_ILLUSION );

	if ( immune )
	{
		if ( show )
			dam_message( ch, victim, dam, sn, immune );

		print_char( ch, "Twój cel rozp³ywa siê w powietrzu i znika.\n\r", PERS4( victim, ch ) );
		send_to_char( "Rozp³ywasz siê w powietrzu.\n\r", victim );
		act( "$N rozp³ywa siê w powietrzu.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala iluzja opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemiê.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp³ywa siê w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez ¶ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp³ywa siê pozostawiaj±c tylko chmurê dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy siê przez chwilê, po czym rozpryskujê siê na kwa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p pêka z hukiem i rozpryskuje siê dooko³a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje siê na kawa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}

		extract_char( victim, TRUE );

		return FALSE;
	}

	immune = IS_AFFECTED( victim, AFF_SOUL );

	if ( immune )
	{
		if ( show )
			dam_message( ch, victim, dam, sn, immune );
		print_char( ch, "Uwalniasz duszê %s, która ulatuje do swojego wymiaru\r", PERS4( victim, ch ) );
		send_to_char( "Ulatujesz do swojego wymiaru.\n\r", victim );
		act( "$n uwalnia duszê $Z, która ulatuje do swojego wymiaru.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala dusza opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemiê.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp³ywa siê w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez ¶ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp³ywa siê pozostawiaj±c tylko chmurê dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy siê przez chwilê, po czym rozpryskujê siê na kwa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p pêka z hukiem i rozpryskuje siê dooko³a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje siê na kawa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}

		extract_char( victim, TRUE );

		return FALSE;
	}

	immune = check_stone_skin( NULL, victim, dam_type, &dam, NULL );

	if ( dam == 0 || immune )
	{
		if ( show )
			dam_message( ch, victim, dam, sn, immune );

		return FALSE;
	}

	dam = check_resist( victim, dam_type, dam );

	if ( dam == 0 )
	{
		if ( show )
			dam_message( ch, victim, dam, sn, TRUE );

		return FALSE;
	}

	if ( IS_AFFECTED( victim, AFF_TROLL ) && victim->hit > 0 && victim->hit - dam < 0 )
	{
		dam = victim->hit + 1;
		create_event( EVENT_TROLL_POWER_REGEN, number_range( 3, 5 ) * PULSE_VIOLENCE, victim, NULL, 0 );
	}

	DEBUG_INFO( "spell_damage:damage_reduction" );

	if ( is_affected(victim,gsn_demon_aura))
	{
		if( IS_GOOD(ch))
        {
            dam -= get_skill(victim,gsn_demon_aura)/6;
        }
		else if ( !IS_EVIL(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura)/12;
        }
	}

	if( dam < 0 ) dam = 1;

	if ( is_affected(victim,gsn_defense_curl))
	{
		defense_curl = affect_find(victim->affected,gsn_defense_curl );
		defense_curl->modifier -= UMIN( defense_curl->level, dam );
		dam -= UMIN( defense_curl->level, dam );
		if( defense_curl->modifier <= 0 )
		{
			affect_remove( victim, defense_curl );
			act( "Otaczaj±ca ciê ochronna spirala zwija siê, a po chwili zanika.", victim,NULL,NULL,TO_CHAR);
		}
	}

	if ( is_affected( victim, gsn_damage_reduction ) && dam > 0 )
	{
		AFFECT_DATA * pAff;

		if ( ( pAff = affect_find( victim->affected, gsn_damage_reduction ) ) != NULL )
		{
			if ( pAff->level <= 16 )
				dam -= 1;
			else if ( pAff->level <= 20 )
				dam -= 2;
			else if ( pAff->level <= 25 )
				dam -= 3;
			else if ( pAff->level <= 30 )
				dam -= 4;
			else
				dam -= 5;

			dam = UMAX( dam, 1 );
		}
#ifdef INFO
		print_char( ch, "DAM - DAMAGE_REDUCTION: %d\n\r", dam );
#endif

	}

	if ( show )
    {
		dam_message( ch, victim, dam, sn, immune );
    }

    /**
     * logowanie wilkosci obrazen
     */
    append_file_format_daily
        (
         ch,
         DAM_LOG_FILE,
         "%s/%d/%s victim: %s/%d spell: %s/%s/%d/%d/%d",
         /* ch */
         IS_NPC(ch) ? "mob":"pc",
         ch->level,
         IS_NPC( ch ) ? "-" : class_table[ ch->class ].name,
         /* victim */
         IS_NPC( victim ) ? strip_colour_codes( victim->short_descr ) : victim->name,
         victim->level,
        /* spell */
         skill_table[ sn ].name,
         attack_table[ dam_type ].name,
         dam_base,
         dam_mod,
         dam
        );

	raw_damage( ch, victim, dam );
	return TRUE;
}


int compute_tohit( CHAR_DATA *ch, bool primary )
{
	int thac0_00, thac0_32, thac0;
	OBJ_DATA *wield;
	int sn;
	int fight_style_skill = 0;
    bool check;

	if ( primary )
		wield = get_eq_char( ch, WEAR_WIELD );
	else
		wield = get_eq_char( ch, WEAR_SECOND );

	sn = get_weapon_sn( ch, TRUE );

	if ( IS_NPC( ch ) )
	{
		thac0_00 = 20;
		if ( EXT_IS_SET( ch->act, ACT_WARRIOR ) )
			thac0_32 = 0;
		if ( EXT_IS_SET( ch->act, ACT_MONK ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_PALADIN ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_BARBARIAN ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_THIEF ) )
			thac0_32 = 4;
		else if ( EXT_IS_SET( ch->act, ACT_BARD ) )
			thac0_32 = 4;
		else if ( EXT_IS_SET( ch->act, ACT_CLERIC ) )
			thac0_32 = 4;
		else if ( EXT_IS_SET( ch->act, ACT_MAGE ) )
			thac0_32 = 8;
		else if ( EXT_IS_SET( ch->act, ACT_SHAMAN ) )
			thac0_32 = 4;
		else
			thac0_32 = 2;
	}
	else
	{
		thac0_00 = class_table[ ch->class ].thac0_00;
		thac0_32 = class_table[ ch->class ].thac0_32;
	}

	thac0 = interpolate( ch->level, thac0_00, thac0_32 );

	thac0 -= GET_HITROLL( ch );

	if ( wield )
		thac0 -= UMIN( wield->value[ 5 ], 6 );

	//lata na dagger_mastery
	if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_THIEF ) ) ||
			( !IS_NPC( ch ) && ch->class == CLASS_THIEF ) )
	{
		OBJ_DATA *weapon;

		if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) != NULL &&
				weapon->value[ 0 ] == 2 &&
				( weapon = get_eq_char( ch, WEAR_SECOND ) ) != NULL &&
				weapon->value[ 0 ] == 2 )
			fight_style_skill = get_skill( ch, gsn_dagger_mastery );
		else
			fight_style_skill = 0;

		if ( !primary )
        {
            thac0 += 1 + 8 * ( 100 - fight_style_skill ) / 100;
		}
		else
		{
			if ( get_eq_char( ch, WEAR_SECOND ) != NULL )
			{
				thac0 += 4 * ( 100 - fight_style_skill ) / 100;
			}
		}
	}
	else if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) ) ||
			( !IS_NPC( ch ) && ch->class == CLASS_BLACK_KNIGHT ) )
	{
		OBJ_DATA *weapon;

		if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) != NULL &&
				weapon->value[ 0 ] == WEAPON_WHIP &&
				( weapon = get_eq_char( ch, WEAR_SECOND ) ) != NULL &&
				weapon->value[ 0 ] == WEAPON_WHIP )
			fight_style_skill = get_skill( ch, gsn_whip_mastery );
		else
			fight_style_skill = 0;

		if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) != NULL &&
				weapon->value[ 0 ] == WEAPON_CLAWS &&
				( weapon = get_eq_char( ch, WEAR_SECOND ) ) != NULL &&
				weapon->value[ 0 ] == WEAPON_CLAWS )
			fight_style_skill = (get_skill( ch, gsn_claws )*2)/3;
		else
			fight_style_skill = 0;

		if ( !primary )
        {
            thac0 += 1 + 8 * ( 100 - fight_style_skill ) / 100;
		}
		else
		{
			if ( get_eq_char( ch, WEAR_SECOND ) != NULL )
			{
				thac0 += 4 * ( 100 - fight_style_skill ) / 100;
			}
		}
	}
	else if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_BARBARIAN ) ) ||
			( !IS_NPC( ch ) && ch->class == CLASS_BARBARIAN ) ||
			( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_BARD ) ) ||
			( !IS_NPC( ch ) && ch->class == CLASS_BARD ) )
	{
		fight_style_skill = get_skill( ch, gsn_two_weapon_fighting );
		fight_style_skill += UMIN( (get_curr_stat_deprecated(ch,STAT_DEX)-18)*3, 0 );

		if ( !primary )
		{
			thac0 += 1 + 8 * ( 100 - fight_style_skill/3 ) / 100;
		}
		else
		{
			if ( get_eq_char( ch, WEAR_SECOND ) != NULL )
			{
				thac0 += 4 * ( 100 - fight_style_skill/2 ) / 100;
			}
			else if ( wield && ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS) ))
			{
				fight_style_skill = get_skill( ch, gsn_twohander_fighting );
				thac0 -= 3 * ( fight_style_skill ) / 100;
			}
		}
	}
	else
	{
		fight_style_skill = get_skill( ch, gsn_two_weapon_fighting );
		fight_style_skill += UMIN( (get_curr_stat_deprecated(ch,STAT_DEX)-15)*3, 0 );
		if ( !primary )
		{
			thac0 += 1 + 8 * ( 100 - fight_style_skill ) / 100;
		}
		else
		{
			if ( get_eq_char( ch, WEAR_SECOND ) != NULL )
			{
				thac0 += 4 * ( 100 - fight_style_skill ) / 100;
			}
			else if ( wield && ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS) ))
			{
				fight_style_skill = get_skill( ch, gsn_twohander_fighting );
				thac0 -= 3 * ( fight_style_skill ) / 100;
			}
		}
	}

	thac0 -= weapon_skill_mod( ch, primary, TRUE );

	if ( !IS_NPC( ch ) && ch->condition[ COND_SLEEPY ] == 0 )
		thac0 += 4;

	if ( IS_SET( ch->fight_data, FIGHT_DISARM ) )
		thac0 += 4;

	if ( IS_SET( ch->fight_data, FIGHT_DODGE ) )
		thac0 += 2;

	if ( IS_SET( ch->fight_data, FIGHT_PARRY ) )
		thac0 += 6;

	if ( IS_SET( ch->fight_data, FIGHT_MIGHTY_BLOW ) )
		thac0 += 4;

	if ( IS_SET( ch->fight_data, FIGHT_POWER_STRIKE ) )
		thac0 += 8;

	if ( IS_SET( ch->fight_data, FIGHT_CRITICAL_STRIKE ) )
		thac0 += 10;

	if ( IS_SET( ch->fight_data, FIGHT_OVERWHELMING_STRIKE ) )
		thac0 += get_curr_stat_deprecated(ch,STAT_STR) > 21 ? 8 : 6;

	if ( ch->daze > 0 )
		thac0 += 6;

	if(ch->level == 1)
		thac0 = URANGE(0, thac0, number_range(10, 15));
	return thac0;
}

int calculate_final_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam)
{
  if ( IS_AFFECTED(victim,AFF_DEFLECT_WOUNDS ) && dam > 0 )
    dam /= 2;

  /* nowe modyfikatory obra¿eñ, dla wersji > 2.104 */
  /* Ogólnie: PvM 50%, reszta bez zmian. */
  /*
  if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
      dam /= 2;
    }
	*/
  return dam;
}

/* do_raw_damage */
void raw_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam )
{
    int i;
    AFFECT_DATA *paf;

    if( check_illusion( victim ) )
    {
        return;
    }

    if ( victim->position == POS_DEAD || !victim->in_room )
    {
        return;
    }

    DEBUG_INFO( "raw_damage:gain_exp_from_damage" );

    /* exp za robienie szkody*/
    if ( !IS_NPC( ch ) && ch != victim && dam > 0 )
    {
        int xval;
        dam = UMIN( victim->hit + 11, dam );
        xval = number_range( dam * 1, dam * 3 );
        xval = UMIN( number_range(100, 300), xval );
        gain_exp( ch, xval, TRUE );
    }

    if ( IS_AFFECTED(victim,AFF_DEFLECT_WOUNDS ) && dam > 0 )
        dam /= 2;

    //gracz vs. mob mnozone x150%
    if ( !IS_NPC(ch) )
    {
        dam *=DAMAGE_MODIFIER_PLAYER_VS_MOB;
        //dam *= 3;
        //dam /= 2;
    }


    DEBUG_INFO( "raw_damage:negative_dam" );
    if ( dam < 0 )
    {
        victim->hit = UMIN( get_max_hp(victim), victim->hit - dam );
        return;
    }

    victim->hit -= dam;

    DEBUG_INFO( "raw_damage:mount1" );

    if ( victim->mount && dam > number_range(5, 8) )//coby nie spadali po byle gownie
    {
        do_dismount_body(victim);
        send_to_char( "Tracisz rownowagê i spadasz z wierzchowca.\n\r", victim );
        act( "$n traci równowagê i spada z wierzchwca.", victim, NULL, NULL, TO_ROOM );
        victim->position = POS_SITTING;
        WAIT_STATE( victim, 6 );
    }
    else if ( victim->mounting )
    {
        strip_invis( victim->mounting, TRUE, TRUE );

        switch ( victim->mounting->position )
        {
            case POS_DEAD:
                act( "Trup $z zostaje zrzucony przez $N na ziemiê.", victim, NULL, victim->mounting, TO_NOTVICT );
                break;
            case POS_MORTAL:
            case POS_INCAP:
            case POS_STUNNED:
                send_to_char( "Osuwasz siê ze swojego wierzchowca.\n\r", victim->mounting );
                act( "$n zrzuca $C na ziemiê.", victim, NULL, victim->mounting, TO_NOTVICT );
                break;
            case POS_SLEEPING:
                act( "$z spada z $Z na ziemiê.", victim, NULL, victim->mounting, TO_NOTVICT );
                break;
            case POS_FIGHTING:
                send_to_char( "Twój wierzchowiec zrzuca ciê w ferworze walki na ziemiê.\n\r", victim->mounting );
                act( "$n zrzuca w ferworze walki $C na ziemiê.", victim, NULL, victim->mounting, TO_NOTVICT );
                break;
            case POS_RESTING:
            case POS_SITTING:
            case POS_STANDING:
            default :
                send_to_char( "Twój wierzchowiec zrzuca ciê na ziemiê.\n\r", victim->mounting );
                act( "$n zrzuca $C na ziemiê.", victim, NULL, victim->mounting, TO_NOTVICT );
                break;
        }
        victim->mounting->position = POS_SITTING;
        WAIT_STATE( victim->mounting, 6 );
        do_dismount_body(victim);
    }

    /* zapisz obra¿enia wiêksze ni¿ 100 */
    if (dam > 100)
    {
        sprintf( log_buf, "[%d][damlog] %s (%d) zadano obra¿enia <%d> przez %s (%d) (%d) w %s.",
                victim->in_room ? victim->in_room->vnum : 0,
                victim->name,
                victim->level,
                dam,
                ( IS_NPC( ch ) ? ch->short_descr : ch->name ),
                ch->level,
                ( IS_NPC( ch ) ? -1 : ch->class ),
                victim->in_room ? victim->in_room->name : "unknown"
               );
        log_string( log_buf );
    }

    /* zapisz ostatnie dam do countera */
    for ( i = 2; i > 0; i--)
    {
        victim->counter[ i ] = victim->counter[ i-1 ];
    }

    victim->counter[ 0 ] = dam;

    DEBUG_INFO( "raw_damage:wait_char" );
    if ( victim->wait_char )
    {
        if ( victim->wait >= 0 )
            victim->wait = skill_table[ victim->wait_char->sn ].beats;

        free_pwait_char( victim->wait_char );
        victim->wait_char = NULL;
        print_char( victim, "Straci³%s¶ koncentracjê.\n\r", victim->sex == 2 ? "a" : victim->sex == 1 ? "e" : "o" );
    }

    /* immortal wiadomo */
    if ( !IS_NPC( victim )
            && victim->level >= LEVEL_IMMORTAL
            && victim->hit < 1 )
    {
        victim->hit = 1;
    }

    //holy prayer, szansa na "revive" przy prawie pelnej puli, revive czysci pule
    if ( !IS_NPC( victim ) && victim->hit < 1 )
    {
        if ( ( paf = affect_find( victim->affected, gsn_prayer_last )) != NULL )
        {
            if(paf->modifier < 20)
            {
                if ( ( paf = affect_find( victim->affected, gsn_holy_pool )) != NULL)
                {
                    if(paf->modifier > 80 && number_percent() < (paf->modifier/4))
                    {

                        victim->hit = get_max_hp(victim)/2;
                        paf->modifier = 0;
                        victim->position = POS_STANDING;
                        switch ( victim->sex )
                        {
                            case 0:
                                send_to_char( "¦mierelnie ranne, trac±c si³y, osuwasz siê powoli na ziemiê. Gdy twoja twarz ma uderzyæ o ziemiê, czujesz jak zgromadzona w tobie ¶wiêta moc wzburza siê i rezonuje, otaczaj±c twoje rany. Z niedowierzaniem patrzysz na to co siê dzieje. Na chwilê czas zdaje siê przesta³ istnieæ. Gdy otrz±sujesz siê z zdziwienia, wszystko wraca do normy, a ty orientujesz siê, ¿e znowu stoisz na nogach, twoje najpowa¿niejsze rany siê zasklpei³y, a wype³niaj±ca ciê ¶wiêta moc wygas³a.\n\r", victim );
                                act( "Widzisz jak $n osuwa siê nieprzytomnie na kolana, gdy nagle jego cia³o wybucha niesamowitym, o¶lepiaj±cym ¶wiat³em! Gdy otwierasz oczy, zauwa¿asz, ¿e czê¶æ ran $z siê zasklepi³a, a otaczaj±ce go niezwyk³e ¶wiat³o zupe³nie wygas³o.", victim, NULL, NULL, TO_ROOM );
                                break;
                            case 1:
                                send_to_char( "¦mierelnie ranny, trac±c si³y, osuwasz siê powoli na ziemiê. Gdy twoja twarz ma uderzyæ o ziemiê, czujesz jak zgromadzona w tobie ¶wiêta moc wzburza siê i rezonuje, otaczaj±c twoje rany. Z niedowierzaniem patrzysz na to co siê dzieje. Na chwilê czas zdaje siê przesta³ istnieæ. Gdy otrz±sujesz siê z zdziwienia, wszystko wraca do normy, a ty orientujesz siê, ¿e znowu stoisz na nogach, twoje najpowa¿niejsze rany siê zasklpei³y, a wype³niaj±ca ciê ¶wiêta moc wygas³a.\n\r", victim );
                                act( "Widzisz jak $n osuwa siê nieprzytomnie na kolana, gdy nagle jego cia³o wybucha niesamowitym, o¶lepiaj±cym ¶wiat³em! Gdy otwierasz oczy, zauwa¿asz, ¿e czê¶æ ran $z siê zasklepi³a, a otaczaj±ce go niezwyk³e ¶wiat³o zupe³nie wygas³o.", victim, NULL, NULL, TO_ROOM );
                                break;
                            default :
                                send_to_char( "¦mierelnie ranna, trac±c si³y, osuwasz siê powoli na ziemiê. Gdy twoja twarz ma uderzyæ o ziemiê, czujesz jak zgromadzona w tobie ¶wiêta moc wzburza siê i rezonuje, otaczaj±c twoje rany. Z niedowierzaniem patrzysz na to co siê dzieje. Na chwilê czas zdaje siê przesta³ istnieæ. Gdy otrz±sujesz siê z zdziwienia, wszystko wraca do normy, a ty orientujesz siê, ¿e znowu stoisz na nogach, twoje najpowa¿niejsze rany siê zasklpei³y, a wype³niaj±ca ciê ¶wiêta moc wygas³a.\n\r", victim );
                                act( "Widzisz jak $n osuwa siê nieprzytomnie na kolana, gdy nagle jej cia³o wybucha niesamowitym, o¶lepiaj±cym ¶wiat³em! Gdy otwierasz oczy, zauwa¿asz, ¿e czê¶æ ran $z siê zasklepi³a, a otaczaj±ce j± niezwyk³e ¶wiat³o zupe³nie wygas³o.", victim, NULL, NULL, TO_ROOM );
                                break;
                        }
                    }


                }
            }
        }
    }



    //koniec holy prayer

    DEBUG_INFO( "raw_damage:update_pos" );
    update_pos( victim );

    switch ( victim->position )
    {
        case POS_MORTAL:
            if ( IS_NPC( victim ) && IS_SET( victim->form, FORM_CONSTRUCT ) )
            {
                switch ( victim->sex )
                {
                    case 0:
                        act( "$n jest unieruchomione.", victim, NULL, NULL, TO_ROOM );
                        break;
                    case 1:
                        act( "$n jest unieruchomiony.", victim, NULL, NULL, TO_ROOM );
                        break;
                    default :
                        act( "$n jest unieruchomiona.", victim, NULL, NULL, TO_ROOM );
                        break;
                }
                break;
            }

            switch ( victim->sex )
            {
                case 0:
                    act( "$n jest ¶miertelnie ranne, umrze je¶li nikt temu nie pomo¿e.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste¶ ¶miertelnie ranne, umrzesz je¶li nikt ci nie pomo¿e.\n\r", victim );
                    break;
                case 1:
                    act( "$n jest ¶miertelnie ranny, umrze je¶li nikt mu nie pomo¿e.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste¶ ¶miertelnie ranny, umrzesz je¶li nikt ci nie pomo¿e.\n\r", victim );
                    break;
                default :
                    act( "$n jest ¶miertelnie ranna, umrze je¶li nikt jej nie pomo¿e.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste¶ ¶miertelnie ranna, umrzesz je¶li nikt ci nie pomo¿e.\n\r", victim );
                    break;
            }
            break;

        case POS_INCAP:
            if ( IS_NPC( victim ) && IS_SET( victim->form, FORM_CONSTRUCT ) )
            {
                switch ( victim->sex )
                {
                    case 0:
                        act( "$n jest unieruchomione.", victim, NULL, NULL, TO_ROOM );
                        break;
                    case 1:
                        act( "$n jest unieruchomiony.", victim, NULL, NULL, TO_ROOM );
                        break;
                    default :
                        act( "$n jest unieruchomiona.", victim, NULL, NULL, TO_ROOM );
                        break;
                }
                break;
            }

            switch ( victim->sex )
            {
                case 0:
                    act( "$n jest unieruchomione i umrze je¶li temu nikt nie pomo¿e.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste¶ unieruchomione i umrzesz je¶li ci nikt nie pomo¿e.\n\r", victim );
                    break;
                case 1:
                    act( "$n jest unieruchomiony i umrze je¶li mu nikt nie pomo¿e.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste¶ unieruchomiony i umrzesz je¶li ci nikt nie pomo¿e.\n\r", victim );
                    break;
                default :
                    act( "$n jest unieruchomiona i umrze je¶li jej nikt nie pomo¿e.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste¶ unieruchomiona i umrzesz je¶li ci nikt nie pomo¿e.\n\r", victim );
                    break;
            }
            break;

        case POS_STUNNED:
            if ( IS_NPC( victim ) && IS_SET( victim->form, FORM_CONSTRUCT ) )
            {
                switch ( victim->sex )
                {
                    case 0:
                        act( "$n jest unieruchomione.", victim, NULL, NULL, TO_ROOM );
                        break;
                    case 1:
                        act( "$n jest unieruchomiony.", victim, NULL, NULL, TO_ROOM );
                        break;
                    default :
                        act( "$n jest unieruchomiona.", victim, NULL, NULL, TO_ROOM );
                        break;
                }
                break;
            }

            switch ( victim->sex )
            {
                case 0:
                    act( "$n jest oszo³omione, ale prawdopodobnie wyjdzie z tego.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste¶ oszo³omione, ale prawdopodobnie wyjdziesz z tego.\n\r", victim );
                    break;
                case 1:
                    act( "$n jest oszo³omiony, ale prawdopodobnie wyjdzie z tego.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste¶ oszo³omiony, ale prawdopodobnie wyjdziesz z tego.\n\r", victim );
                    break;
                default :
                    act( "$n jest oszo³omiona, ale prawdopodobnie wyjdzie z tego.", victim, NULL, NULL, TO_ROOM );
                    send_to_char( "Jeste¶ oszo³omiona, ale prawdopodobnie wyjdziesz z tego.\n\r", victim );
                    break;
            }
            break;

        case POS_DEAD:

            /* zeby triger DEAD odpalal siê wczesniej */
            if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_DEATH ) )
            {
                victim->position = POS_STANDING;
                mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_DEATH );
                victim->position = POS_DEAD;
            }
            else
            {
                if ( !( IS_NPC( victim ) && IS_SET( victim->form, FORM_CONSTRUCT ) ) )
                {
                    act( "$n nie ¿yje!!", victim, 0, 0, TO_ROOM );
                    switch ( victim->sex )
                    {
                        case 0:
                            act( "$n pada na ziemiê... MARTWE.", victim, NULL, NULL, TO_ROOM );
                            break;
                        case 1:
                            act( "$n pada na ziemiê... MARTWY.", victim, NULL, NULL, TO_ROOM );
                            break;
                        default :
                            act( "$n pada na ziemiê... MARTWA.", victim, NULL, NULL, TO_ROOM );
                            break;
                    }
                }
                else
                {
                    act( "$n rozpada siê na kawa³eczki.", victim, 0, 0, TO_ROOM );
                }
            }
            send_to_char( "Nie ¿yjesz, co za pech!!!\n\r\n\r", victim );
            break;

        default:
            if ( dam > get_max_hp(victim) / 4 )
            {
                send_to_char( "{RTO naprawdê bola³o!{x\n\r", victim );
                if( dam > 35 && number_percent() > con_app[get_curr_stat_deprecated(victim,STAT_CON)].shock )
                {
                    DAZE_STATE( victim, URANGE( 12, (dam - (get_max_hp(victim)/4)) + ( ( (MAX_STAT_VALUE - get_curr_stat_deprecated(victim,STAT_CON)/2)/11 ) ), 48) );
                    send_to_char( "Ból nie do wytrzymania chwilowo parali¿uje twoje miê¶nie!\n\r", victim );
                    act( "$n wydaje z siebie pe³en bólu ryk! To musia³o boleæ...", victim, NULL, NULL, TO_ROOM );
                }
            }
            if ( victim->hit < get_max_hp(victim) / 5 )
            {
                send_to_char( "{RPowa¿nie KRWAWISZ{x!\n\r", victim );
            }

            if ( !IS_NPC( victim )
                    && victim->hit > 0
                    && victim->hit <= victim->wimpy
                    && victim->wait == 0 )
            {
                do_flee( victim, "" );
            }

            break;
    }

    DEBUG_INFO( "raw_damage:one_hit_kill_check" );
    /* zeby grupkowicze zalapali ze im kogos sprzatneli */
    if ( IS_NPC( victim ) )
        onehit_kill_check( victim, ch );

    if ( !IS_AWAKE( victim ) )
        stop_fighting( victim, FALSE );

    DEBUG_INFO( "raw_damage:pos_dead_stuff" );
    if ( victim->position == POS_DEAD )
    {
        //group_gain( ch, victim );
        if ( !IS_NPC( victim ) && victim != ch )
        {
            sprintf( log_buf, "[%d] [player death] %s (%d) zosta³%s zabity przez %s (%d) w %s.",
                    victim->in_room ? victim->in_room->vnum : 0,
                    victim->name,
                    victim->level,
                    victim->sex == 2 ? "a" : victim->sex == 1 ? "" : "o",
                    ( IS_NPC( ch ) ? ch->short_descr : ch->name ),
                    ch->level,
                    victim->in_room ? victim->in_room->name : "unknown"
                   );
            log_string( log_buf );
        }
        /**
         * prepare wiznet string
         */
        sprintf( log_buf, "[%d] %s%s (%d) rozwalon%s przez %s (%d) w %s.",
                victim->in_room ? victim->in_room->vnum : 0,
                IS_NPC( victim )? "NPC ":"",
                IS_NPC( victim )? victim->short_descr : victim->name,
                victim->level,
                victim->sex == 2 ? "a" : victim->sex == 1 ? "y" : "e",
                ch->name4,
                ch->level,
                victim->in_room ? victim->in_room->name : "unknown"
               );
        if ( IS_NPC( victim ) )
        {
            wiznet( log_buf, NULL, NULL, WIZ_MOBDEATHS, 0, 0 );
        }
        else
        {
            wiznet( log_buf, NULL, NULL, WIZ_DEATHS, 0, 0 );
        }

        raw_kill( victim, ch );
        return;
    }

    return;
}

/*
 * sprawdza czy powinien stracic expa, ile traci i czy nie traci tez poziomu
 * dodatkowo, nadawany affect 'player death' po smierci
 */

void check_player_death( CHAR_DATA *ch, CHAR_DATA *killer )
{
	int exp_loss, szanse = - MAX_LEVEL, scinka;
	AFFECT_DATA *taff, aff;
	const int ROZNICA_LEV = 4; //ró¿nica lev powy¿ej której mniej nas boli zgon
	const int LEVEL_PROTECTED = 18; //poziom do którego mniej nas boli zgon

	if ( IS_NPC( ch ) || ch->level == 1 || IS_IMMORTAL( ch ) )
    {
        return;
    }

//Brohacz: poprawia w plikach postaci, bo na ostatnim levelu exp rosl przy padzie
	if ( ch->level == LEVEL_HERO && ch->exp > exp_per_level( ch, 30 ) )
		ch->exp = exp_per_level( ch, 30 );

    /**
     * 2008-05-10 - do po³owy LEVEL_NEWBIE - nie traci siê expa za zgon
     */
	if ( ch->level < LEVEL_NEWBIE / 2 )
		return;

	/* Tener: je¿eli zabi³ mob na charmie, to prawdziwym killerem jest jego master */
	if ( IS_AFFECTED( killer, AFF_CHARM ) && killer->master )
	   killer = killer->master;

	//rellik: kary za zgon, obliczanie roznicy pomiedzy graczami pozwoli uzale¿niæ kary od niej
	if ( !IS_NPC( killer ) ) szanse = killer->level - ch->level;

    if ( !is_affected( ch, gsn_playerdeathlearnpenalty ) )
    {
        aff.where       = TO_AFFECTS;
        aff.type        = gsn_playerdeathlearnpenalty;
        aff.level       = 0;
        aff.duration    = 2 + dice(2, 4);
        aff.rt_duration = 2 * aff.duration;
        aff.location    = APPLY_NONE;
        aff.modifier    = 0;
        aff.bitvector   = &AFF_NONE;
        affect_to_char( ch, &aff, NULL, FALSE );
    }

	if ( is_affected( ch, gsn_playerdeath ) )
	{
/*		if ( IS_NPC( killer ) )
			exp_loss = ( number_range( 18, 30 ) * ( exp_per_level( ch, ch->level ) - exp_per_level( ch, ch->level - 1 ) ) ) / 100;
		else
			exp_loss = ( number_range( 10, 22 ) * ( exp_per_level( ch, ch->level ) - exp_per_level( ch, ch->level - 1 ) ) ) / 100;
*/

		// testowe zmniejszenie traconego expa!
		// Gurthg: 2007-05-09
	//Brohacz: BUGFIX: go¶æ na 31 levelu ma exp obliczany z expa potrzebnego z levelu 30 na 31
	if ( ch->level == LEVEL_HERO )
		exp_loss = ( exp_per_level( ch, 30 ) - exp_per_level( ch, 29 ) ) / 6;
	else
		exp_loss = ( exp_per_level( ch, ch->level ) - exp_per_level( ch, ch->level - 1 ) ) / 10;

        if ( ch->level <  LEVEL_IMMORTAL)
        {
            ch->exp -= exp_loss;
        }

        if ( exp_per_level( ch, ch->level - 1 ) > ch->exp )
        {
            delevel( ch, FALSE );
        }

		if ( ( taff = affect_find( ch->affected, gsn_playerdeath ) ) == NULL )
			return;

		switch ( taff->level )
		{
			/* teraz juz mozna zdelevelowac */
			//rellik: kary za zgon, drugi zgonik no...
		case 0:
		case 1:
			if ( ch->level < LEVEL_PROTECTED || szanse > ROZNICA_LEV )
			{
				taff->level = 2;
			} else {
				taff->level = 3;
			}
			taff->duration += ( 1 * 60 ); //rellik: kary za zgon, drugi zgon przed³u¿a dzia³anie afektów, dodamy godzinkê
			taff->rt_duration += ( 2 * 60 ); //rellik: i dorzucamy jeszcze godzinkê (mo¿e byæ poza gr±)
			skill_loss( ch, 1 );
			aff.where	= TO_AFFECTS;
			aff.type	= gsn_playerdeathpenalty;
			aff.level	= 0;
			aff.duration	= ( 1 * 58 ); //rellik: godzinkê na mudzie, aby zgubiæ II zgon (zostanie odpracowanie resztê z pierwszego)
			aff.rt_duration = ( 2 * 59 ); //rellik: i 2 godzinki czasu obojêtnie czy w grze (czyli 2 godziny z tego jedn± mo¿e byæ poza gr±)
			aff.bitvector = &AFF_NONE;
			aff.location	= APPLY_LUC;
			aff.modifier	= -3;
			affect_to_char( ch, &aff, "II zgon", FALSE );
			aff.location = number_range( APPLY_STR, APPLY_DEX );
			aff.modifier = -number_range( 0, 2 );
			affect_to_char( ch, &aff, "II zgon", FALSE );
			return;
			/* tu tez mozna zdelevelowac, i co gorsza
			* dodawane sa minusy do str, dex, con
			*/
		case 2:
		case 3:
			//rellik: kary za zgon, po trzecim zgonie ju¿ os³abiamy
			if ( ch->level < LEVEL_PROTECTED || szanse > ROZNICA_LEV )
			{
				scinka = 2;
			} else {
				scinka = 3;
			}
			taff->level = 4;
			taff->duration += ( 1 * 60 ); //rellik: dok³adamy kolejn± godzinkê gry
			taff->rt_duration += ( 4 * 60 ); //rellik: i ... sady¶ci ... 3 godziny, które mo¿e wykorzystaæ na pisanie do nas za¿aleñ na d³ugo¶æ trwania kar
			aff.where	= TO_AFFECTS;
			aff.type	= gsn_playerdeathpenalty;
			aff.level	= 0;
			aff.duration	= ( 1 * 60 ); //rellik: jeszcze jedn± godzinkê
			aff.rt_duration = ( 4 * 59 ); //rellik: tak tak... 3 godziny z kary mo¿e po¶wiêciæ na granie inn±  postaci±
			aff.bitvector = &AFF_NONE;
			aff.location	= APPLY_STR;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			aff.location	= APPLY_DEX;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			aff.location	= APPLY_CON;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			aff.location	= APPLY_LUC;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			aff.location	= APPLY_INT;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			aff.location	= APPLY_WIS;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			aff.location	= APPLY_CHA;
			aff.modifier	= -scinka;
			affect_to_char( ch, &aff, "III zgon", FALSE );
			skill_loss( ch, 2 );
			return;
		default:
			return;
		}
		return;
	}
	else
	{
		/* pierwszy zgon od conajmniej x godzin
		  	exp spada gora do 1 potrzebnej na tym poziomie
		*/
		//rellik: kary za zgon, spadek luck
		aff.where	= TO_AFFECTS;
		aff.type	= gsn_playerdeath;
		if ( ch->level < LEVEL_PROTECTED || szanse > ROZNICA_LEV )
		{
			aff.level = 0;
		} else {
			aff.level	= 1;
		}
		if ( IS_NPC( killer ) && ch->level == LEVEL_HERO ) //Brohacz: zmniejsza bezkarnosc pierwszego pada od moba na 31 levelu
		{
			aff.duration	= (1 * 90); //1,5 godzinki w stanie zwiêkszonego zagro¿enia, ¿e trafi siê drugi zgon...
			aff.rt_duration = (4 * 60); //2,5 godzinki mo¿e przesiedzieæ poza mudem
		}
		else
		{
			aff.duration	= (1 * 60); //1 godzinka w stanie zwiêkszonego zagro¿enia, ¿e trafi siê drugi zgon...
			aff.rt_duration = (2 * 60); //drug± mo¿e przesiedzieæ poza mudem
		}
		aff.bitvector = &AFF_NONE;
		aff.location	= APPLY_LUC;
		aff.modifier	= -3;
		affect_to_char( ch, &aff, "I zgon", FALSE );

		//rellik: kary za zgon, wywali³em stratê '+' przy pierwszym zgonie

/*		if ( IS_NPC( killer ) )
			exp_loss = ( number_range( 13, 25 ) * ( exp_per_level( ch, ch->level ) - exp_per_level( ch, ch->level - 1 ) ) ) / 100;
		else
			exp_loss = ( number_range( 8, 15 ) * ( exp_per_level( ch, ch->level ) - exp_per_level( ch, ch->level - 1 ) ) ) / 100;
*/

		// testowe zmniejszenie traconego expa!
		// Gurthg: 2007-05-09
	//Brohacz: BUGFIX: go¶æ na 31 levelu ma exp obliczany z expa potrzebnego z levelu 30 na 31
        if ( ch->level == LEVEL_HERO )
        {
            exp_loss = ( exp_per_level( ch, 30 ) - exp_per_level( ch, 29 ) ) / 6;
        }
        else
        {
            exp_loss = ( exp_per_level( ch, ch->level ) - exp_per_level( ch, ch->level - 1 ) ) / 10;
        }

        if ( ch->level <  LEVEL_IMMORTAL)
        {
            ch->exp = UMAX( exp_per_level( ch, ch->level - 1 ), ch->exp - exp_loss );
        }

        return;
    }

    return;
}

void generate_onehit_msg( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt, bool immune )
{
	char buf[ 256 ];
	const char *ch_msg = "";
	const char *victim_msg = "";
	const char *room_msg = "";
	const char *attack;

	if ( ch == NULL || victim == NULL )
		return;

	if ( immune )
	{
		/* jakis tekscik wygenerowac*/
		return;
	}

	if ( dam == 0 )
	{
		switch ( number_range( 0, 6 ) )
		{
			case 0:
				ch_msg = "{mPróbujesz wyprowadziæ cios, ale chybiasz $C haniebnie.{x";
				victim_msg = "{m$n próbuje wyprowadziæ cios, ale chybia cie haniebnie.{x";
				room_msg = "{m$n próbuje wyprowadziæ cios, ale chybia $C haniebnie.{x";
				break;
			case 1:
				ch_msg = "{mAtakujesz, ale w ostatniej chwili tracisz rownowagê i chybiasz $C.{x";
				victim_msg = "{m$n atakuje ciê, ale w ostatniej chwili traci rownowagê i chybia.{x";
				room_msg = "{m$n atakuje $C, ale w ostatniej chwili traci rownowagê i chybia.{x";
				break;
			case 2:
				ch_msg = "{mChybiasz $C.{x";
				victim_msg = "{m$n chybia ciê.{x";
				room_msg = "{m$n chybia $C.{x";
				break;
			case 3:
				ch_msg = "{mNiestety mimo ca³ego wysi³ku w³o¿onego w uderzenie chybiasz $C.{x";
				victim_msg = "{m$n wk³ada sporo wysi³ku w ten cios, jednak chybia ciê.{x";
				room_msg = "{m$n wk³ada sporo wysi³ku w ten cios, jednak chybia $C.{x";
				break;
			case 4:
				ch_msg = "{mTwoje uderzenie przecina z sykiem powietrze chybiaj±c $C o w³os.{x";
				victim_msg = "{mUderzenie $z przecina z sykiem powietrze chybiaj±c ciê o w³os.{x";
				room_msg = "{mUderzenie $z przecina z sykiem powietrze chybiaj±c $C o w³os.{x";
				break;
			case 5:
				ch_msg = "{mBalansujesz cia³em próbuj±c trafiæ $C, ale chybiasz.{x";
				victim_msg = "{m$n balansuje cia³em próbuj±c ciê trafiæ, ale chybia.{x";
				room_msg = "{m$n balansuje cia³em próbuj±c trafiæ $C, ale chybia.{x";
				break;
			case 6:
				ch_msg = "{mPróbujesz waln±æ $C, ale mijasz siê ze swoim celem.{x";
				victim_msg = "{m$n próbuje ciê waln±æ, ale mija siê z tob±.{x";
				room_msg = "{m$n próbuje waln±æ $C, ale mija siê ze swoim celem.{x";
				break;
		}
		act( ch_msg, ch, NULL, victim, TO_CHAR );
		act( victim_msg, ch, NULL, victim, TO_VICT );
		act( room_msg, ch, NULL, victim, TO_NOTVICT );
		return;
	}
	else if ( dam <= 3 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s cie i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 7 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 11 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 15 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 19 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 23 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 27 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 31 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 35 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 39 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 45 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 50 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 60 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else if ( dam <= 75 )
	{
		switch ( number_range( 0, 1 ) )
		{
			case 0:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
			case 1:
				ch_msg = "Trafiasz %s i koniec.";
				victim_msg = "$n trafia %s ciê i koniec.";
				room_msg = "$n trafia %s i koniec.";
				break;
		}
	}
	else
		return;

	if ( dt >= TYPE_HIT && dt < TYPE_HIT + MAX_DAMAGE_MESSAGE )
		attack	= attack_table[ dt - TYPE_HIT ].noun;
	else
	{
		bug( "Dam_message: bad dt %d.", dt );
		attack = attack_table[ 0 ].name;
	}

	sprintf( buf, ch_msg, attack );
	act( buf, ch, NULL, victim, TO_CHAR );
	sprintf( buf, victim_msg, attack );
	act( buf, ch, NULL, victim, TO_VICT );
	sprintf( buf, room_msg, attack );
	act( buf, ch, NULL, victim, TO_NOTVICT );
	return;
}

/* zeby grupka zalapala atak po ktorym ktos z grupki zejdzie */
void onehit_kill_check( CHAR_DATA *mob, CHAR_DATA *ch )
{
	CHAR_DATA * vch;
	save_debug_info("fight.c => onehit_kill_check", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );
	if ( mob == ch )
		return;

	for ( vch = mob->in_room->people; vch; vch = vch->next_in_room )
		if ( vch != mob && is_same_group( vch, mob ) && !vch->fighting && IS_NPC(vch) && !IS_AFFECTED(vch, AFF_CHARM ) )
			vch->fighting = ch;
	return;
}

void reorganize_mobile_group( CHAR_DATA *mob )
{
	CHAR_DATA * group = NULL, *vch;

	/* najpierw sprawdzamy czy ktos mial leader na mob */
	for ( vch = mob->in_room->people; vch; vch = vch->next_in_room )
	{
		if ( vch->leader == mob )
		{
			if ( mob->leader )
			{
				vch->leader = mob->leader;
				vch->master = mob->master;
			}
			else
			{
				if ( !group )
				{
					group = vch;
					continue;
				}
				else
				{
					vch->leader = group;
					vch->master = group;
				}
			}
		}
	}
}

/* zamiast makra */
int GET_AC( CHAR_DATA *ch, int type )
{
	OBJ_DATA /** weapon,*/ *shield;
	AFFECT_DATA *paf;
	int armor = 100;
	//int twohander_style = 0;

	/* dla eterycznego trzeba przeliczac AC ciut inaczej
	   bazowo AC 1 plus zrecznosc i czarki
	   nie licza siê bonusy na gratach...z lenistwa
	 */
	if ( IS_AFFECTED( ch, AFF_ETHEREAL_ARMOR ) )
	{
		armor = 10;
	}
	else if ( is_affected( ch, gsn_steel_scarfskin ) )
	{
 	        armor = UMIN( armor, 30 );
	}
	else if ( IS_AFFECTED( ch, AFF_ARMOR ) )
	{
	        armor = UMIN( armor, 60 );
	}
	else if ( IS_AFFECTED( ch, AFF_IMMOLATE ) )
	{
                armor = UMIN( armor, 95 );

		for ( paf = ch->affected; paf; paf = paf->next )
			if ( paf->type == gsn_immolate )
			{
				armor -= paf->modifier;
				break;
			}
	}

	for ( paf = ch->affected; paf; paf = paf->next )
	   if ( paf->location == APPLY_AC )
	      armor += paf->modifier;

	armor = armor < ch->armor[ type ] ? armor : ch->armor[ type ];

	if ( ( shield = get_eq_char( ch, WEAR_SHIELD ) ) != NULL )
	{
		if(	shield->item_type == ITEM_SHIELD )
		{
			//rellik, przerabiam bo trochê sztywne by³o (tarcza ac 4 i 5 by³y takie same), [20080507]
			armor -= URANGE( 10, shield->value[1]*7, 40 ) ;
		}
        else if ( shield->item_type == ITEM_WEAPON )
        {
			armor -= 10;
		}
	}

	armor += ( IS_AWAKE( ch ) ? dex_app[ get_curr_stat_deprecated( ch, STAT_DEX ) ].defensive : 0 );

	/*
	   if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) != NULL &&
	   IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS) &&
	   ( twohander_style = get_skill( ch, gsn_twohander_fighting ) ) > 0 )
	   armor -= ( ch->class == CLASS_BARBARIAN ? 40 : 30 ) * twohander_style / 100;
	 */

	if ( ( IS_NPC( ch ) && EXT_IS_SET( ch->act, ACT_MONK ) ) ||
			( !IS_NPC( ch ) && ch->class == CLASS_MONK ) )
		armor -= 10 * wis_app[ get_curr_stat_deprecated( ch, STAT_WIS ) ].mod;

	return armor;
}

void check_armor_spell( CHAR_DATA *victim, int dam )
{
	AFFECT_DATA * paf;
	if ( !IS_AFFECTED( victim, AFF_ARMOR ) )
		return;

	for ( paf = victim->affected; paf; paf = paf->next )
		if ( paf->bitvector == &AFF_ARMOR &&
				dam > paf->modifier )
		{
			if ( skill_table[ paf->type ].msg_off && skill_table[ paf->type ].msg_off != '\0' )
			{
				send_to_char( skill_table[ paf->type ].msg_off, victim );
				send_to_char( "\n\r", victim );
			}

			affect_remove( victim, paf );
			break;
		}


	return;
}

void make_blood( CHAR_DATA *ch )
{
	OBJ_DATA *obj;

	for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
		if ( obj->pIndexData->vnum == OBJ_VNUM_SPILLED_BLOOD )
			break;

	if ( !obj)
	{
		obj = create_object( get_obj_index( OBJ_VNUM_SPILLED_BLOOD ), FALSE );
		obj->timer	= number_range( 12, 16 );
		obj->value[0] = 5;
		obj->value[1] = 5;
		obj_to_room( obj, ch->in_room );
	}
	else
	{
		obj->timer += number_range( 2, 5 );
		obj->timer = UMIN( 16, obj->timer );
		obj->value[0] += 5;
		obj->value[1] += 5;
		obj->value[0] = UMIN( 40, obj->value[0] );
		obj->value[1] = UMIN( obj->value[1], obj->value[0] );
	}
	return;
}

void make_brains( CHAR_DATA *ch )
{
	OBJ_DATA *obj;

	obj = create_object( get_obj_index( OBJ_VNUM_BRAINS ), FALSE);
	obj->timer	= number_range( 12, 24 );
	obj_to_room( obj, ch->in_room );
}

/* kretynstwo do robiebia glowki w przypadku dekapitacji*/
void make_head( CHAR_DATA *ch )
{
	OBJ_DATA * obj;
	char buf[ MIL ], *name;

	name = ch->name2;

	obj = create_object( get_obj_index( OBJ_VNUM_SEVERED_HEAD ), FALSE );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, "g³owy %s", ch->name2 );
	free_string( obj->name2 );
	obj->name2 = str_dup( buf );

	sprintf( buf, "g³owie %s", ch->name2 );
	free_string( obj->name3 );
	obj->name3 = str_dup( buf );

	sprintf( buf, "g³owê %s", ch->name2 );
	free_string( obj->name4 );
	obj->name4 = str_dup( buf );

	sprintf( buf, "g³ow± %s", ch->name2 );
	free_string( obj->name5 );
	obj->name5 = str_dup( buf );

	sprintf( buf, "g³owie %s", ch->name2 );
	free_string( obj->name6 );
	obj->name6 = str_dup( buf );


	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->item_type = ITEM_TRASH;
	obj->value[5] = -17;//by nie bylo dwoch glow w przypadku bounty hunting
	obj_to_room( obj, ch->in_room );
}

void make_leg( CHAR_DATA *ch )
{
	OBJ_DATA * obj;
	char buf[ MIL ], *name;

	name = ch->name2;

	obj = create_object( get_obj_index( OBJ_VNUM_SLICED_LEG ), FALSE );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, "nogi %s", ch->name2 );
	free_string( obj->name2 );
	obj->name2 = str_dup( buf );

	sprintf( buf, "nodze %s", ch->name2 );
	free_string( obj->name3 );
	obj->name3 = str_dup( buf );

	sprintf( buf, "nogê %s", ch->name2 );
	free_string( obj->name4 );
	obj->name4 = str_dup( buf );

	sprintf( buf, "nog± %s", ch->name2 );
	free_string( obj->name5 );
	obj->name5 = str_dup( buf );

	sprintf( buf, "nodze %s", ch->name2 );
	free_string( obj->name6 );
	obj->name6 = str_dup( buf );


	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->item_type = ITEM_TRASH;
	obj_to_room( obj, ch->in_room );
}

void make_arm( CHAR_DATA *ch )
{
	OBJ_DATA * obj;
	char buf[ MIL ], *name;

	name = ch->name2;

	obj = create_object( get_obj_index( OBJ_VNUM_SLICED_ARM ), FALSE );
	obj->timer	= number_range( 4, 7 );

	sprintf( buf, obj->short_descr, name );
	free_string( obj->short_descr );
	obj->short_descr = str_dup( buf );

	sprintf( buf, "rêki %s", ch->name2 );
	free_string( obj->name2 );
	obj->name2 = str_dup( buf );

	sprintf( buf, "rêce %s", ch->name2 );
	free_string( obj->name3 );
	obj->name3 = str_dup( buf );

	sprintf( buf, "rêkê %s", ch->name2 );
	free_string( obj->name4 );
	obj->name4 = str_dup( buf );

	sprintf( buf, "rêk± %s", ch->name2 );
	free_string( obj->name5 );
	obj->name5 = str_dup( buf );

	sprintf( buf, "rêce %s", ch->name2 );
	free_string( obj->name6 );
	obj->name6 = str_dup( buf );


	sprintf( buf, obj->description, name );
	free_string( obj->description );
	obj->description = str_dup( buf );
	obj->item_type = ITEM_TRASH;
	obj_to_room( obj, ch->in_room );
}

int parry_mod( OBJ_DATA *weapon )
{
	if ( !weapon || weapon->item_type != ITEM_WEAPON )
    {
		return 0;
    }

	switch ( weapon->value[ 0 ] )
	{
		case WEAPON_SWORD: return 10;
		case WEAPON_SHORTSWORD: return 5;
		case WEAPON_AXE:
					if ( IS_SET( weapon->value[ 4 ], WEAPON_TWO_HANDS ) )
						return -10;
					else
						return -50;
		case WEAPON_DAGGER:
		case WEAPON_CLAWS: return -20;
		case WEAPON_SPEAR:
		case WEAPON_POLEARM:
		case WEAPON_STAFF:
				    if ( IS_SET( weapon->value[ 4 ], WEAPON_TWO_HANDS ) )
					    return 10;
				    else
					    return -30;
		case WEAPON_MACE: return -40;
		case WEAPON_FLAIL: return -60;
		case WEAPON_WHIP: return -60;
		default: return 0;
	}
	return 0;
}

bool special_damage_message(CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *wield, int dam, int dt, bool immune)
{
	save_debug_info("fight.c => special_damage_message", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );
	if ( dt != gsn_backstab &&
			dt != gsn_charge &&
			dt != gsn_kick &&
			dt != gsn_stun &&
			dt != gsn_trip &&
			dt != gsn_smite &&
			dt != gsn_smite_good &&
			dt != gsn_crush &&
			dt != gsn_tail)
		return FALSE;

	if( IS_AFFECTED(ch,AFF_DEFLECT_WOUNDS)) dam /= 2;

	//lata, jesli backstab idzie z damage, wtedy nie podaje ptr broni
	if ( !wield )
		wield = get_eq_char( ch, WEAR_WIELD );

	/* KICK TEXT */
	if ( dt == gsn_kick )
	{
		if ( immune )
		{
			act( "{5Odsuwasz siê lekko od $Z i kopiesz $M tak mocno jak mo¿esz, jednak $E nawet tego nie zauwa¿a.{x", ch, NULL, victim, TO_CHAR );
			act( "{5$n odsuwa siê lekko od ciebie i kopie ciê tak mocno jak tylko mo¿e, jednak nawet tego nie zauwa¿asz.{x", ch, NULL, victim, TO_VICT );
			act( "{5$n odsuwa siê od $Z i kopie tak mocno jak tylko mo¿e, jednak $E nawet tego nie zauwa¿a.{x", ch, NULL, victim, TO_NOTVICT );
			return TRUE;
		}
		else if ( dam <= 0 )
		{
			switch ( number_range( 0, 3 ) )
			{
				case 0:
					act( "{5Bierzesz zamach i próbujesz kopn±æ $C, ale wyra¼nie ci to nie wychodzi, chybiasz $M chyba o pó³ metra.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$x wyra¼nie nie wychodzi kopniak, chybia ciê o jakies pó³ metra.{x", ch, NULL, victim, TO_VICT );
					act( "{5$x wyra¼nie nie wychodzi kopniak, chybia $C o jakie¶ po³ metra.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 1:
					act( "{5Wyprowadzasz prostego kopniaka, w tym czasie jednak $N robi lekki unik i cios ze¶lizguje siê po $S ciele.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n wyprowadza kopniaka, lekki unik jednak wystarcza, aby potencjalnie gro¼ny cios ze¶lizn±³ ci siê bez szwanku po ciele.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n wyprowadza kopniaka, lekki unik jednak wystarcza, aby potencjalnie gro¼ny cios ze¶lizn±³ siê bez szwanku po ciele $Z.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 2:
					act( "{5Próbujesz kopn±æ $C, ale tracisz na sekundê równowagê i chybiasz haniebnie.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n próbuje ciê kopn±æ, ale traci na sekundê równowagê i chybia haniebnie.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n próbuje kopn±æ $C, ale traci na sekundê równowagê i chybia haniebnie.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 3:
					act( "{5Nie wiesz jak ci siê to uda³o, ale próbuj±c kopn±æ $C prawie $t obrót o pó³ osi.{x", ch, ch->sex == 2 ? "zrobi³a¶" : "zrobi³e¶", victim, TO_CHAR );
					act( "{5$n robi co¶ przedziwnego, do kopniaka bierze tak têgi zamach, ¿e kiedy nie trafia obraca siê o pó³ osi i staje do ciebie na u³amek sekundy plecami.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n robi co¶ przedziwnego, do kopniaka bierze tak têgi zamach, ¿e kiedy nie trafia obraca siê o pó³ osi i staje do $Z na u³amek sekundy plecami.{x", ch, NULL, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
        else if ( victim->hit - dam < -11 )
        {
            if ( !( IS_NPC( victim ) && IS_SET( victim->form, FORM_CONSTRUCT ) ) )
            {
                switch ( number_range( 0, 2 ) )
                {
                    case 0:
                        act( "{5Próbujesz wyprowadziæ kopniak, zatrzymujesz na chwilê nogê w powietrzu po czym uderzasz $C. Cios by³ skuteczny jak diabli, $N pada na ziemiê, rzêzi chwilê i umiera.{x", ch, NULL, victim, TO_CHAR );
                        act( "{5$n próbuje chyba wyprowadziæ kopniak, zawiesza jednak na chwilê noge w powietrzu jakby $t zamiar po czym kopie ciê prosto w krtañ. ¦wiat wiruje ci przed oczyma, padasz na ziemiê i umierasz.{x", ch, ch->sex == 2 ? "zmieni³a" : "zmieni³", victim, TO_VICT );
                        act( "{5$n próbuje chyba wyprowadziæ kopniak, zawiesza jednak na chwilê nogê w powietrzu jakby $t zamiar po czym kopie $C. Cios okazuje siê niezwykle skuteczny, $N pada na ziemiê, rzêzi chwilê i umiera.{x", ch, ch->sex == 2 ? "zmieni³a" : "zmieni³", victim, TO_NOTVICT );
                        break;

                    case 1:
                        act( "{5Kopiesz $C wk³adaj±c w to ca³± si³ê. $N pada na ziemiê zupe³nie oszo³omion$R, dostrzegasz tylko przez moment gasn±ce w $Q ¿ycie, a ju¿ po sekundzie jest po wszystkim.{x", ch, NULL, victim, TO_CHAR );
                        act( "{5$n kopie ciê potê¿nie, przewracasz siê na ziemiê i tracisz oddech, oczy zachodz± ci mg³±, umierasz.{x", ch, NULL, victim, TO_VICT );
                        act( "{5$n kopie $C wk³adaj±c w to chyba ca³± si³ê. $N pada zupe³nie oszo³omion$R, dostrzegasz tylko przez moment gasn±ce w $Q ¿ycie, a ju¿ po sekundzie jest po wszystkim.{x" , ch, NULL, victim, TO_NOTVICT );
                        break;

                    case 2:
                        act( "{5$N próbuje robiæ jakie¶ uniki, dostaje jednak solidnego kopniaka nie bed±c na to w ogóle $t. S³ychaæ dono¶ny chrupot po czym $N pada na ziemiê, zwija siê chwilê z bólu i nieruchomieje.{x", ch, victim->sex == 2 ? "przygotowana" : "przygotowany", victim, TO_CHAR );
                        act( "{5Dostajesz potê¿nego kopniaka od $z prosto w splot s³oneczny, czujesz chrupot pêkaj±cych ¿eber, chyba jakie¶ przebija twoje serce... umierasz w jednej chwili.{x", ch, NULL, victim, TO_VICT );
                        act( "{5$N próbuje robiæ uniki, nawet mu to wychodzi, po chwili jednak dostaje od $c naprawdê solidnego kopniaka, s³ychaæ tylko dono¶ny chrupot pêkaj±cych ko¶ci, wali siê bez czucia na ziemiê i umiera.{x", ch, NULL, victim, TO_NOTVICT );
                        break;
                }
            }
            else
            {
                        act( "{5$N dostaje solidnego kopniaka. S³ychaæ dono¶ny chrupot po czym $N rozpada siê na kawlki.{x", ch, NULL, victim, TO_ROOM );
                        act( "{5$N próbuje robiæ uniki, nawet mu to wychodzi, po chwili jednak dostaje od $c naprawdê solidnego kopniaka, s³ychaæ tylko dono¶ny chrupot pêkaj±cych ko¶ci, wali siê bez czucia na ziemiê i umiera.{x", ch, NULL, victim, TO_NOTVICT );
                act( "$n rozpada sie na kawleczki", victim, 0, 0, TO_ROOM );
            }
			return TRUE;
		}
		else
		{
			if (victim->position < POS_FIGHTING )
			{
				act( "{5Kopiesz $C z ca³ej si³y.{x",   ch, NULL, victim, TO_CHAR );
				act( "{5$n kopie ciê z ca³ej si³y.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n kopie $C z ca³ej si³y.{x",  ch, NULL, victim, TO_NOTVICT );
			}
			else
			{
				switch ( number_range( 0, 3 ) )
				{
					case 0:
						act( "{5Wyczekujesz odpowiedniego momentu i kopiesz $C z ca³ej si³y.{x", ch, NULL, victim, TO_CHAR );
						switch (victim->sex)
						{
							case 0:
								act( "{5$x udaje siê znale¼æ odpowiedni moment, kiedy jeste¶ ods³oniête i kopie ciê z ca³ej si³y.{x", ch, NULL, victim, TO_VICT );
								act( "{5$x udaje siê znale¼æ odpowiedni moment, kiedy $N jest ods³oniête i kopie $M z ca³ej si³y.{x", ch, NULL, victim, TO_NOTVICT );
								break;
							case 1:
								act( "{5$x udaje siê znale¼æ odpowiedni moment, kiedy jeste¶ ods³oniêty i kopie ciê z ca³ej si³y.{x", ch, NULL, victim, TO_VICT );
								act( "{5$x udaje siê znale¼æ odpowiedni moment, kiedy $N jest ods³oniêty i kopie $M z ca³ej si³y.{x", ch, NULL, victim, TO_NOTVICT );
								break;
							case 2:
							default:
								act( "{5$x udaje siê znale¼æ odpowiedni moment, kiedy jeste¶ ods³oniêta i kopie ciê z ca³ej si³y.{x", ch, NULL, victim, TO_VICT );
								act( "{5$x udaje siê znale¼æ odpowiedni moment, kiedy $N jest ods³oniêta i kopie $M z ca³ej si³y.{x", ch, NULL, victim, TO_NOTVICT );
								break;
						}
						break;

					case 1:
						act( "{5Odchodzisz lekko od $Z i kopiesz $M tak mocno jak tylko mo¿esz.{x", ch, NULL, victim, TO_CHAR );
						act( "{5$n odchodzi lekko od ciebie i kopie ciê tak mocno jak tylko mo¿e.{x", ch, NULL, victim, TO_VICT );
						act( "{5$n odchodzi lekko od $Z i kopie $M tak mocno jak tylko mo¿e.{x", ch, NULL, victim, TO_NOTVICT );
						break;

					case 2:
						act( "{5Czekasz chwilê a¿ $N siê trochê ods³oni i kiedy to siê staje kopiesz $M.{x", ch, NULL, victim, TO_CHAR );
						act( "{5$n chyba na co¶ czeka, przez przypadek ods³aniasz siê i po chwili dostajesz $s solidnym kopniakiem.{x", ch, victim->sex == 2 ? "poczêstowana" : "poczêstowany", victim, TO_VICT );
						act( "{5$n obserwuje uwa¿nie $C jakby na co¶ czekaj±c i solidnie $M kopie kiedy $t siê ods³ania.{x", ch, victim->sex == 2 ? "tamta" : "tamten", victim, TO_NOTVICT );
						break;

					case 3:
						act( "{5Ooo, udaje ci siê wyprowadziæ niez³y cios, kopiesz $C z pó³obrotu. Efektownie, efektownie, no no.{x", ch, NULL, victim, TO_CHAR );
						act( "{5Na twoje nieszczê¶cie $x wychodzi ca³kiem niez³y cios. Niespecjalnie wa¿ne jest jak $o siê to uda³o, istotne jest solidne trafienie z pó³obrotu w zêby. Ouuuuuuuæææ!!{x", ch, NULL, victim, TO_VICT );
						act( "{5$x udaje siê wyprowadziæ naprawdê niez³y, efektowny cios. Trafia $C wykopem z pó³obrotu.{x", ch, wield, victim, TO_NOTVICT );
						break;
				}
			}
			return TRUE;
		}
	}
	/* BACKSTAB OPISY */
	else if ( dt == gsn_backstab && wield )
	{
		if ( immune )
		{
			act( "{5Zachodzisz $C od ty³u i wbijasz $O $p na wysoko¶ci barków prawie po rêkoje¶æ, jednak $E nie reaguje na to.{x", ch, wield, victim, TO_CHAR );
			act( "{5$n zachodzi ciê od ty³u i wbija ci $p w plecy, jednak nie reagujesz na to.{x", ch, wield, victim, TO_VICT );
			act( "{5$n zachodzi $C od ty³u i wbija $O $p w plecy, jednak $E nie reaguje na to.{x", ch, wield, victim, TO_NOTVICT );
			return TRUE;
		}
		else if ( dam <= 0 )
		{//opisy specjalne dla potworow o duzym cielsku (ale nie bardzo duzym like smoki)
			if( !str_cmp( race_table[ GET_RACE(victim) ].name, "wywerna" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "hydra" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "mantykora" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "smok" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "bazyliszek" ))
			{
				act( "{5Staraj±c siê zachowaæ ostro¿no¶æ podchodzisz od ty³u do $Z. Niestety, z t± ostro¿no¶ci± nie do koñca siê uda³o, niedosz³a ofiara zauwa¿a ciê kontem ¶lepia, odwraca siê i atakuje.{x", ch, wield, victim, TO_CHAR );
				act( "{5$n staraj±c siê zachowaæ wszelkie ¶rodki ostro¿no¶ci podchodzi od ty³u to $Z. Z t± ostro¿no¶ci± jednak nie do koñca $m wysz³o, niedosz³a ofiara zauwa¿a $s kontem ¶lepia, odwraca siê i atakuje.{x", ch, wield, victim, TO_NOTVICT );
				return TRUE;
			}

			switch ( number_range( 0, 3 ) )
			{
				case 0:
					act( "{5Bezszelestnie zakradasz siê do $Z, chyba jednak nie uwa¿asz nadto dobrze. $N dostrzega ciê, uchyla siê i unika twojego ciosu.{x", ch, wield, victim, TO_CHAR );
					act( "{5Dostrzegasz $c zakrad±jacego siê w twoim kierunku z jakim¶ ostrzem w rêku, robisz b³yskawiczny unik i $s cios tnie powietrze.{x", ch, wield, victim, TO_VICT );
					act( "{5Widzisz jak $n zakrada siê do $Z z jakim¶ ostrzem w rêku, $t jednak robi b³yskawiczny unik i cios tnie powietrze.{x", ch, victim->sex == 2 ? "ta" : "ten", victim, TO_NOTVICT );
					break;

				case 1:
					act( "{5Podchodzisz do $Z od ty³u i próbujesz $M d¼gn±æ $j, jednak nie do¶æ mocno go chwytasz i ostrze niegro¼nie ze¶lizguje siê po $S plecach.{x", ch, wield, victim, TO_CHAR );
					act( "{5Otrzymujesz cios w plecy, jednak ostrze ze¶lizguje ci siê tylko po nich. Co za szczê¶cie, $n tym razem nie $t.{x", ch, ch->sex == 2 ? "trafi³a" : "trafi³", victim, TO_VICT );
					act( "{5$n podchodzi do $Z od ty³u, zamierza siê z jakims ostrzem do ciosu i zadaje go. ¬le chyba jednak trzyma $h, bo ostrze niegro¼nie ze¶lizguje siê po plecach $Z.{x", ch, wield, victim, TO_NOTVICT );
					break;

				case 2:
					act( "{5Zachodzisz $C po ma³ym ³uku od ty³u, robisz wypad i próbujesz d¼gn±æ $j, jednak trafiasz $M tylko lekko w ramiê.{x", ch, wield, victim, TO_CHAR );
					act( "{5Czujesz dra¶niêcie w lewe ramiê, kto¶ próbowa³ chyba d¼gn±æ ciê w³asnie w plecy. Odwracasz lekko g³owê i dostrzegasz $c.{x", ch, wield, victim, TO_VICT );
					act( "{5$n zachodzi $C po ma³ym ³uku od ty³u, robi wypad i próbuje d¼gn±æ $M $j, jednak trafia tylko w lewe ramiê.{x", ch, wield, victim, TO_NOTVICT );
					break;

				case 3:
					act( "{5W kilku skokach próbujesz pokonaæ odleg³o¶æ do $Z i d¼gn±æ $M $j, jednak potykasz siê i zamiast tego wpadasz $O na plecy.{x", ch, wield, victim, TO_CHAR );
					switch( NOPOL(wield->name5[0]) )
					{
						case 'Z':
						case 'S':
							act( "{5S³yszysz kroki za plecami a po chwili kto¶ wpada na ciebie. Odwracasz siê i dostrzegasz $c ze $j w rêku. Tym razem nie uda³o siê $o zadaæ ciosu.{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n w paru skokach podbiega do $Z, ze $j w rêku chc±c $M d¼gn±æ. Jednak potyka siê i zamiast tego wpada $O na plecy.{x", ch, wield, victim, TO_NOTVICT );
							break;
						default:
							act( "{5S³yszysz kroki za plecami a po chwili kto¶ wpada na ciebie. Odwracasz siê i dostrzegasz $c z $j w rêku. Tym razem nie uda³o siê $o zadaæ ciosu.{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n w paru skokach podbiega do $Z, z $j w rêku chc±c $M d¼gn±æ. Jednak potyka siê i zamiast tego wpada $O na plecy.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					break;
			}
			return TRUE;
		}
		else if ( victim->hit - dam < -11 )
		{
			if ( victim->position <= POS_SLEEPING )
			{
				act( "{5Podchodzisz do $Z, ¶ciskasz mocno $p i z ca³ej si³y wbijasz swoj± broñ w $S plecy po sam± rekoje¶æ. Cia³o $Z wyprê¿a siê i po chwili nieruchomieje.{x", ch, wield, victim, TO_CHAR );
				act( "{5Przeszywa ciê straszny ból. Przez sparali¿owany nim mózg przebija siê jedna my¶l. Kto¶ chyba d¼gn±³ ciê w plecy. Nie starcza ci czasu ¿eby pomy¶leæ o czym¶ wiêcej. Umierasz.{x", ch, wield, victim, TO_VICT );
				act( "{5$n podchodzi do $Z ¶ciskaj±c mocno $p i z ca³ej si³y wbija swoj± broñ w $S plecy po sam± rêkoje¶æ. Cia³o $Z wyprê¿a siê i po chwili nieruchomieje.{x", ch, wield, victim, TO_NOTVICT );
				return TRUE;
			}

			if ( IS_SET( race_table[ GET_RACE(victim) ].type, ANIMAL ) )
			{
				act( "{5Zakradasz siê od ty³u do $Z i d¼gasz tam, gdzie jak s±dzisz znajduje siê serce. Przera¼liwe charczenie towarzyszy ostatnim podrygom $Z. W cokolwiek trafi³e¶, cios okaza³ siê zabójczy.{x", ch, wield, victim, TO_CHAR );
				act( "{5$n zakrada siê od ty³u do $Z i mocno d¼ga. Ostatnim podrygom $Z towarzyszy przera¼liwe charczenie. W cokolwiek $n trafi³, cios okaza³ siê zabójczy.{x", ch, wield, victim, TO_NOTVICT );
				return TRUE;
			}
			//opisy specjalne dla humanoidalnych potworow duzych rozmiarow
			if( !str_cmp( race_table[ GET_RACE(victim) ].name, "ogr" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "troll" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "umberkolos" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "yeti" ))
			{
				if ( ch->size == SIZE_SMALL ||
						ch->size == SIZE_TINY )
				{
					act( "{5Wyj±tkowo dobrze uda³o ci siê zgraæ swoje ruchy. Pojawiasz siê za ciê¿ko ju¿ rannym przeciwnikiem i z ca³ej si³y wbijasz mu $h w prawe udo, po czym pomagaj±c sobie drug± rêk± przeci±gasz otrze w dó³ rozrywaj±c ¶ciêgna kolana i miê¶nie ³ydki. Cios ten dope³ni³ los $Z, wali siê $E na ziemiê i wykrwawia na ¶mieræ.{x", ch, wield, victim, TO_CHAR );
					act( "{5$n pojawia siê b³yskawicznie za ciê¿ko ju¿ rannym przeciwnikiem i z ca³ej si³y wbija mu $h w prawe udo, po czym pomagaj±c sobie drug± rêk± przeci±ga ostrze w dó³ rozrywaj±c kolana i miê¶nie ³ydki. Cios ten dope³ni³ los $Z, wali siê $E na ziemiê i wykrwawia na ¶mieræ.{x" , ch, wield, victim, TO_NOTVICT );
				}
				else
				{
					act( "{5Zakradasz siê do $Z od ty³u zupe³nie nie zauwazon<&y/a/e>. Z satysfakcj± przystajesz na chwilê, ¶ciskasz mocno rêkoje¶æ $f, wyci±gasz rêkê w górê by siêgn±æ wysoko¶ci serca potwora i wbijasz w $S plecy swój orê¿ tak g³êboko jak tylko mo¿esz. $N krztusi siê jaki¶ czas, po czym pada g³ucho jak spróchnia³a k³oda na ziemiê.{x", ch, wield, victim, TO_CHAR );
					act( "{5$n zakrada siê cichutko do $Z, przystaje na chwilê, wyci±ga rêkê w górê i d¼ga z ca³ej si³y w $S plecy na wysoko¶ci serca potwora. $N charczy przez chwilê i nagle w zupe³nej ciszy osuwa siê martw$R na ziemiê.{x" , ch, wield, victim, TO_NOTVICT );
				}
				return TRUE;
			}
			//opisy specjalne dla potworow o duzym cielsku (ale nie bardzo duzym like smoki)
			if( !str_cmp( race_table[ GET_RACE(victim) ].name, "wywerna" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "hydra" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "smok" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "mantykora" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "bazyliszek" ))
			{
				act( "{5Ostro¿nie podchodzisz do zajêtego teraz czym¶ innym potwora, bierzesz zamach i wykorzystuj±c ca³± sw± si³ê wbijasz $h w jego cielsko. Musia³<&e¶/a¶>o¶> trafiæ w jaki¶ czu³y punkt $Z, gdy¿ wydaje $E z siebie przera¼liwy kwik, a z niewielkiej rany bucha fontanna krwi. Monstrum jeszcze przez chwilê t³ucze swym opas³ym cielskiem o ziemiê i stara siê wyra¼nie zrobiæ ci krzywdnê, lecz nic z tego nie wychodzi. Po chwili ¶lepia $Z gasn± na wieki.{x", ch, wield, victim, TO_CHAR );
				act( "{5$n ostro¿nie podchodzi do zajêtego teraz czym¶ innym potwora, bierze zamach i wykorzystuj±c ca³± sw± si³ê wbija $h w jego cielsko. Widocznie orê¿ wbi³ siê w jaki¶ czu³y punkt $Z, gdy¿ wydaje $E z siebie przera¼liwy kwik, a z niewielkiej rany bucha fontanna krwi. Monstrum jeszcze przez chwilê t³ucze swym opas³ym cielskiem o ziemiê i stara sie wyra¼nie zrobiæ $x krzywdê, lecz gasn±ce powoli ¶lepia ¶wiadcz±, ¿e nic z tego nie wyjdzie.{x", ch, wield, victim, TO_NOTVICT );
				return TRUE;
			}

			switch ( number_range( 0, 3 ) )
			{
				case 0:
					act( "{5Wyj±tkowo dobrze uda³o ci siê zgraæ swoje ruchy. Pojawiasz siê za plecami $Z jak b³yskawica i trafiasz $M $j miêdzy ³opatki. Po chwili $S zwiotcza³e cia³o osuwa siê po tobie na ziemiê.{x", ch, wield, victim, TO_CHAR );
					act( "{5Niespodziewanie otrzymujesz bardzo silny cios w plecy. ¦wiat wiruje ci przed oczyma. Dostrzegasz ostatnim spojrzeniem czyj±¶ twarz... tttooo chhhhhybbbaa $n. Hrrhrhrh...{x", ch, wield, victim, TO_VICT );
					act( "{5$N nagle otrzymuje od $z bardzo silne d¼gniecie $j w plecy i osuwa siê powoli po $s ramionach ostatnim spojrzeniem dostrzegaj±c twarz zabójcy.{x" , ch, wield, victim, TO_NOTVICT );
					break;

				case 1:
					act( "{5Zakradasz siê do $Z od ty³u zupe³nie nie zauwazon$r. Z satysfakcj± przystajesz na chwilê, ¶ciskasz mocno rêkoje¶æ $f i d¼gasz $M w plecy na wysoko¶ci serca. $N pada g³ucho jak spróchnia³a k³oda na ziemiê.{x", ch, wield, victim, TO_CHAR );
					act( "{5Przeszywa ciê straszny ból. Przez sparali¿owany nim mózg przebija siê jedna my¶l. Kto¶ chyba d¼gn±³ ciê w plecy. Nie starcza ci czasu ¿eby pomy¶leæ o czym¶ wiêcej. Osuwasz siê martw$R na ziemiê.{x", ch, wield, victim, TO_VICT );
					switch (ch->sex)
					{
						case 0:
							act( "{5$n zakrada siê nie zauwa¿one do $Z, przystaje na chwilê i d¼ga z ca³ej si³y w plecy. $N charczy przez chwilê i nagle w zupe³nej ciszy osuwa siê martw$R na ziemiê.{x" , ch, wield, victim, TO_NOTVICT );
							break;
						case 2:
							act( "{5$n zakrada siê nie zauwa¿ona do $Z, przystaje na chwilê i d¼ga z ca³ej si³y w plecy. $N charczy przez chwilê i nagle w zupe³nej ciszy osuwa siê martw$R na ziemiê.{x" , ch, wield, victim, TO_NOTVICT );
							break;
						default:
							act( "{5$n zakrada siê nie zauwa¿ony do $Z, przystaje na chwilê i d¼ga z ca³ej si³y w plecy. $N charczy przez chwilê i nagle w zupe³nej ciszy osuwa siê martw$R na ziemiê.{x" , ch, wield, victim, TO_NOTVICT );
							break;
					}
					break;

				case 2:
					act( "{5Podbiegasz w kilku skokach do $Z nie zwracaj±c uwagi na to, ¿e $E dostrzeg$U ciê przez ramiê. Wbijasz $O z ca³ej si³y $p w plecy. $N ³apie ciê jedn± rêk± za g³owê a drug± rozdrapuje ci twarz. Osuwa siê powoli i po chwili ju¿ nie ¿yje.{x", ch, wield, victim, TO_CHAR );
					act( "{5K±tem oka dostrzegasz jak $n podbiega w kilku skokach do ciebie trzymaj±c $p w rêku. Zaczynasz siê odwracaæ w $s stronê, ale nie zd±¿asz. $n z ca³ej si³y wbija ci ostrze w plecy. £apiesz go jedn± rêk± za g³owê a drug± rozdrapujesz $o twarz, jednak na nic wiêcej nie masz ju¿ si³y. Padasz na ziemiê i umierasz!{x", ch, wield, victim, TO_VICT );
					act( "{5Dostrzegasz jak $n podbiega w kilku skokach do $Z trzymaj±c $p w rêku. $N zaczyna siê odwracaæ w $s stronê, ale nie zd±¿a. $n z ca³ej si³y wbija $O ostrze w plecy. $N jeszcze ³apie $z jedn± rêka za g³owê a drug± rozdrapuje $o twarz, jednak na nic wiêcej nie ma si³y. Pada na ziemiê i umiera!{x" , ch, wield, victim, TO_NOTVICT );
					break;

				case 3:
					act( "{5W paru sprawnych, kocich ruchach zakradasz siê do $Z i ustawiasz siê za $S plecami. Szybko wyci±gasz $p i mierz±c moment d¼gasz tu¿ przy szyi, wbijaj±c ostrze prawie po sam± rêkoje¶æ. $N harczy, toczy trochê piany z ust i umiera u twoich stóp.{x", ch, wield, victim, TO_CHAR );
					act( "{5Z realnego ¶wiata wyrywa ciê potworny, przeszywaj±cy od stóp do g³ów ból. Kto¶ d¼gn±³ ciê potê¿nie w okolicach szyi jakim¶ ostrym narzêdziem. Harczysz tocz±c pianê z ust i osuwasz siê. Kiedy padasz na ziemiê twoje gasn±ce oczy dostrzegaj± twarz $z.{x", ch, wield, victim, TO_VICT );
					act( "{5$n paroma szybkimi, kocimi ruchami zbli¿a siê do $Z i ustawia siê za $S plecami. Wyci±ga szybko $p i mierz±c przez moment d¼ga tu¿ przy szyi, wbijaj±c ostrze prawie po sam± rêkoje¶æ. $N harczy, toczy trochê piany z ust i oddaje ducha u stop $z.{x", ch, wield, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
		else
		{

			if ( IS_SET( race_table[ GET_RACE(victim) ].type, ANIMAL ) )
			{
				act( "{5Zakradasz siê od ty³u do $Z i d¼gasz tam, gdzie jak s±dzisz znajduje siê serce.{x", ch, wield, victim, TO_CHAR );
				act( "{5$n zakrada siê od ty³u do $Z i mocno d¼ga.{x", ch, wield, victim, TO_NOTVICT );
				return TRUE;
			}
			//opisy specjalne dla humanoidalnych potworow duzych rozmiarow
			if( !str_cmp( race_table[ GET_RACE(victim) ].name, "ogr" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "troll" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "umberkolos" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "yeti" ))
			{
				if ( ch->size == SIZE_SMALL ||
						ch->size == SIZE_TINY )
				{
					switch(number_range(1,2))
					{
						case 1:
							act( "{5Staraj±c siê poruszaæ jak najciszej zachodzisz $C od ty³u, a gdy $E nie zauwa¿a twojej obecno¶ci, wbijasz $h g³êboko w $S nogê uszkadzaj±c staw kolanowy. $N wydaje z siebie cichy i ¿a³osny jêk bólu, a $S koñczyna z lekkim chrupotem zgina siê wbrew woli w³a¶ciciela, który ze stêkiem l±duje na ziemi.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n staraj±c siê poruszaæ jak najciszej zachodzi $C od ty³u, a gdy $E nie zauwa¿a $s obecno¶ci, wbija $e $h g³êboko w $S nogê uszkadzaj±c staw kolanowy. $N wydaje z siebie cichy i ¿a³osny jêk bólu, a $S koñczyna z lekkim chrupotem zgina siê wbrew woli w³a¶ciciela, który ze stêkaniem l±duje na ziemi.{x", ch, wield, victim, TO_NOTVICT );
							WAIT_STATE( victim, 18 ); //1,5 pulse violenca
							victim->position = POS_SITTING;
							break;
						case 2:
							act( "{5Delikatnie skradasz siê do $Z, $E nawet nie podejrzewa, i¿ mozesz byæ za $S plecami. Jednak $S nag³y krok w ty³ powoduje, i¿ o ma³o co nie zosta³<&by¶/aby¶/oby¶> przygniecion<&y/a/e>. Na szczê¶cie sprawny odskok ujawnia doskona³y punkt do uderzenia! Nie namy¶laj±c siê d³ugo, chwytasz $h w obie rêce i potê¿nym pchniêciem wbijasz ostrze tu¿ poni¿ej kolana bestii. S³yszysz ohydny mlask i zgrzyt ko¶ci gdy $h zg³êbia siê w ciele. Wyszarpujesz broñ i cudem unikasz drugiej nogi zmierzaj±cej na spotkanie z twoj± twarz±, po czym w kilku sprawnych skokach wycofujesz siê na bezpieczn± odleg³o¶æ.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n delikatnie skrada siê do $Z, $E widocznie nie podejrzewa, i¿ mo¿e $e znajdowac siê tu¿ za $S plecami. Nagle $N wykonuje nag³y krok w ty³ prawie przygniataj±c $c, $e jednak wykonuje sprawny odskok i chwytaj±c $h w obie rêce wykonuje mordercze pchniêcie. S³yszysz ohydny mlask i zgrzyt ko¶ci gdy $h zg³êbia siê w ciele. $n sprawnym ruchem wyszarpuje broñ i odskakuje na bezpieczn± odleg³o¶æ.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
				}
				else
				{
					switch(number_range(1,2))
					{
						case 1:
							act( "{5Cichym truchtem podbiegasz do niczego nie spodziewaj±cego siê potwora, mierzysz jego plecy bystrym wzrokiem i z nieprawdopodobn± prêdko¶ci± wyskakujesz lekko i wbijasz $h obok krêgos³upa w miejscu, w którym powinny byc p³uca. Nie trafiasz doskonale, jednak ryk bestii i $p zg³ebiaj±cy siê bez oporu pokazuj±, i¿ jednak trafi³<&e¶/a¶/o¶> w co¶ czó³ego. Wyci±gasz broñ i odskakujesz na bezpieczn± odleg³o¶æ.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n cichym truchtem podbiega do niczego nie spodziewaj±cego siê potwora, mierzy jego plecy bystrym wzrokiem i z nieprawdopodobn± prêdko¶ci± wyskakuje lekko i wbija $h obok krêgos³upa w miejscu, w któym powinny byæ p³uca. Nie trafia doskonale, jednak ryk bestii i orê¿ zag³êbiaj±cy siê bez oporu pokazuje, i¿ jednak trafiono $C w co¶ czu³ego. $n wyci±ga $h i odskakuje na bezpieczn± odleg³o¶æ.{x" , ch, wield, victim, TO_NOTVICT );
							break;
						case 2:
							act( "{5Szybkim i ostro¿nym krokiem zblizasz sie do $Z, mierzysz $S plecy bacznym spojrzeniem w poszukiwaniu s³abego punktu, który po chwili odnajdujesz. Wyci±gasz rêce i z ca³ej si³y wbijasz $h w... po¶ladek $Z! Z $S gardzieli wydobywa siê okropny ryk bólu, który rozgleg³ siê w okolicy. Jednak ty juz dawno odskoczy³<&e¶/a¶/o¶> na bezpieczn± odleg³o¶æ.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n szybkim i ostro¿nym krokiem zbli¿a siê do $z i mierzy $S plecy bacznym spojrzeniem. Nagle wyci±ga $e rêce i z ca³ej si³y wbija $h w... po¶ladek $Z! Z gardzieli ofiary wydobywa siê okropny ryk bólu, który rozlega siê po okolicy.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
				}
				return TRUE;
			}
			//opisy specjalne dla potworow o duzym cielsku (ale nie bardzo duzym like smoki)
			if( !str_cmp( race_table[ GET_RACE(victim) ].name, "wywerna" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "hydra" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "mantykora" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "smok" ) ||
					!str_cmp( race_table[ GET_RACE(victim) ].name, "bazyliszek" ))
			{
				act( "{5Cichutko przebieraj±c nó¿kami zakradasz siê za $C, staraj±c siê pozostaæ niezauwa¿on<&ym/±/e>. Zauwa¿aj±c dogodn± okazjê jednym susem podskakujesz do spas³ego cielska potwora i wbijasz w nie $h najg³êbiej jak tylko potrafisz. Ostrze twej broni przecina miêsiste cia³o $Z jakby to by³o mas³o, po czym z chlupotem wyskakuje, ca³e w krwi.{x",ch, wield, victim, TO_CHAR );
				act( "{5$n cichutko przebieraj±c nó¿kami zakrada siê za $C. Wypatruj±c dogodn± okazjê jednym susem doskakuje do spas³ego cielska potwora i g³êboko w nie wbija $h. Ostrze $s broni przecina miêsite cia³o $Z, jakby by³o one z mas³a, poczym z chlupotem wyskakuje, ca³e we krwi.{x", ch, wield, victim, TO_NOTVICT );
				return TRUE;
			}

			switch ( number_range( 0, 3 ) )
			{
				case 0:
					act( "{5Zachodzisz $C od ty³u i wbijasz $O $p na wysoko¶ci barków prawie po rêkoje¶æ.{x", ch, wield, victim, TO_CHAR );
					act( "{5Czujesz przez zaledwie chwilê ciep³o czyjego¶ oddechu za sob± po czym czujesz rozdzieraj±cy ból przechodz±cy przez twoje cia³o, który prawie pozbawia ciê przytomno¶ci. Przez g³owê przechodzi ci strzêp my¶li: SZTYLET!{x", ch, wield, victim, TO_VICT );
					act( "{5$n zachodzi $C od ty³u i wbija $O $p w plecy. Przez u³amek chwili widzisz jak $N zamiera z bólu.{x", ch, wield, victim, TO_NOTVICT );
					break;
				case 1:
					act( "{5Zakradasz siê cicho do $Z i kiedy jeste¶ ju¿ tu¿ za $S plecami bierzesz lekki zamach i d¼gasz $M $j pod lew± ³opatkê.{x", ch, wield, victim, TO_CHAR );
					act( "{5Wydaje ci sie, ¿e kto¶ jest za tob±, po czym s³yszysz cichy syk przecinanego czym¶ powietrza i czujesz uderzenie ostrza w okolicach ³opatki. Cooo za BÓL!{x", ch, wield, victim, TO_VICT );
					act( "{5$n bardzo szybko zakrada siê do $Z i kiedy jest ju¿ za $Y, nie trac±c czasu d¼ga $M pod lew± ³opatkê.{x" , ch, wield, victim, TO_NOTVICT );
					break;
				case 2:
					act( "{5W kilku szybkich krokach podbiegasz do $Z i wbijasz $O $p kilka palców poni¿ej szyi, o szeroko¶æ kciuka mijaj±c krêgos³up.{x", ch, wield, victim, TO_CHAR );
					act( "{5Sylwetka $z miga za twoimi plecami. Kto¶ doskakuje do ciebie i wpycha ci zimne ostrze trochê poni¿ej szyi. Przez moment chwiejesz siê na nogach z trudem ³api±c równowagê.{x", ch, wield, victim, TO_VICT );
					act( "{5$n doskakuje w paru krokach do $Z i wbija $O jakie¶ ostrze trochê poni¿ej szyi. Widzisz jak $N chwieje siê chwilê, ale - o dziwo - nie osuwa siê na ziemiê.{x", ch, wield, victim, TO_NOTVICT );
					break;
				case 3:
					act( "{5Próbujesz cicho podkra¶æ siê do $Z, jednak nie do koñca ci siê to udaje. Robisz trochê ha³asu i $E ciê zauwa¿a, jednak jest ju¿ za pó¼no na reakcjê. Twój $p l±duje miêdzy $S ³opatkami niemal w tym samym momencie.{x", ch, wield, victim, TO_CHAR );
					act( "{5K±tem oka dostrzegasz jak $n zakrada siê tu¿ za twoimi plecami z czym¶ w rêku. Próbujesz instynktownie siê uchyliæ, ale jest ju¿ za pó¼no. Ostrze $f l±duje miêdzy twymi ³opatkami.{x", ch, wield, victim, TO_VICT );
					act( "{5$n zakrada siê za plecami $Z, robi trochê ha³asu i zostaje przez $A w porê dostrze¿on$r. Jest ju¿ jednak za pó¼no na jakielowiek uniki, $p $z l±duje miêdzy ³opatkami $Z.{x", ch, wield, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
		return TRUE;
	}

	/* CHARGE OPISY */
	else if ( dt == gsn_charge && wield )
	{

		if ( immune )
		{
			act( "{5$p ze ¶wistem przecina powietrze, trafia $C jednak $E nawet tego nie zauwa¿a.{x", ch, wield, victim, TO_CHAR );
			act( "{5$p $z ze ¶wistem przecina powietrze, trafia ciê jednak nawet tego nie zauwa¿asz.{x", ch, wield, victim, TO_VICT );
			act( "{5$p $z ze ¶wistem przecina powietrze, trafia $C jednak $E nawet tego nie zauwa¿a.{x", ch, wield, victim, TO_NOTVICT );
		}

		switch ( wield->value[ 0 ] )
		{
			case WEAPON_AXE:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wiêksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Wpatrujesz siê intensywnie w sylwetkê przeciwnika. Bierzesz potê¿ny zamach i z ca³ych si³ nacierasz na $C, jednak przeciwnik jest zbyt szybki, b³yskawicznie uchyla siê od twojego ciosu z szyderczym warkotem. Ostrze $f o centymetry ca³e mija cel poci±gaj±c ciê za sob±. Bogowie! Ten b³±d móg³ kosztowaæ ciê ¿ycie.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $z kieruje swoj± broñ na przeciwnika wraz z potê¿nym zamachem. $N b³yskawicznym ruchem wykonuje unik, a cios atakuj±cego jest tak potê¿ny, ¿e poci±ga go za sob±. $n o ma³o nie nadziewa siê na atak wroga.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Warcz±c z w¶ciek³o¶ci przez zaci¶niête zêby przez zmru¿one oczy wpatrujesz siê w $C. Zaciskaj±c z nienawi¶ci palce na stylisku $f wznosisz go nad g³owê. Nie dbaj±c o w³asne bezpieczeñstwo z ca³ych swoich si³ tniesz prosto w czaszkê przeciwnika, $p wbija siê z g³o¶nym chrzêstem w cia³o nieumar³ego prawie go rozpo³awiaj±c. $N opada z bolesnym i ¶miertelnym skrzekiem. Fontanna cuchn±cej cieczy i oderwane kawa³ki galaretowatego cia³a unosz± siê w powietrze gdy wyszarpujesz sw± broñ.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wyprowadza w¶ciek³y, morderczy cios zaciskaj±c palce na stylisku $f a¿ do bia³o¶ci. Tak potê¿ne uderzenia nale¿± do rzadko¶ci. Z zachwytem spogl±dasz jak ¶miertelne r±bniêcie rozrywa cuchn±ce cia³o $Z prawie na dwie czê¶ci. ¦cierwo powoli osuwa siê z bolesnym skrzekiem, a z cia³a tryska fontanna cuchn±cej, t³ustej cieczy.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5¦ciskasz mocniej stylisko $f. B³yskawicznie oceniasz odleg³o¶æ, bierzesz zamach i r±biesz z ca³ych si³, $h zatacza szeroki ³uk i wbija siê z mokrym mla¶niêciem g³êboko w korpus $Z. Cuchn±ca ciecz wydobywaj±ca siê z martwego cia³a obryzguj±c ciebie i wszystko dooko³a. $N zatacza siê od si³y twojego ciosu skaml±c ¿a³o¶nie. Wydobywasz z siebie ryk w¶ciek³o¶ci i nacierasz ze zdwojon± si³±!{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n precyzyjnie ocenia odleg³o¶æ miêdzy sob± a $V. Jego $p zatacza ¶wietlisty ³uk i z potworn± si³± zag³êbia siê w martwe cia³o przeciwnika. W ostatnim momencie odskakujesz przed bryzgaj±cym, cuchn±cym trupim jadem. Nieumar³y zatacza siê skaml±c pod ciosem $z.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Bez ostrze¿enia, b³yskawicznym ruchem kierujesz $p w kierunku $Z. Ostrze tnie powietrze z przera¿aj±cym ¶wistem ¶mierci i zag³êbia siê g³êboko w korpus nieumar³ego wyrywaj±c przy tym poka¼ny kawa³ gnij±cego miêsa.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak b³yskawicznym ruchem $n kieruje $h w stronê $Z. Ostrze $s broni za ¶wistem wbija siê w korpus bestii i z mokrym chrzêstem kawa³ galaretowego miêsa odrywa siê z torsu $Z i szybuje w powietrze ods³aniaj±c przegni³e wnêtrzno¶ci.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cieñ" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiór" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Tniesz z ca³ych si³ $C wykorzystuj±c chwilê nieuwagi przeciwnika, $p ze ¶wistem tnie powietrze i opada na kark wroga. Zatapia siê w niematerialne cia³o, jednak nie czujesz ¿adnego oporu. G³adko przechodzi przez pó³przezroczysty, ods³oniêty kark i wêdruje dalej w powietrze nie czyni±c $X jakiejkolwiek krzywdy. Z przera¿eniem ws³uchujesz siê w rozbrzmiewaj±cy w twym chorym ze strachu umy¶le upiorny, szyderczy chichot.{x", ch, wield, victim, TO_CHAR );
						act( "{5Powietrze przeszywa dono¶ny ¶wist ostrza $f, pêdz±cego w kierunku ods³oniêtego karku $Z. To $n wykorzystuje sytuacjê i atakuje. Broñ zag³êbia siê w niematerialnym ciele przeciwnika. Ostrze $f przesz³o g³adko przez grdykê i poszybowa³o dalej tn±c ze ¶wistem powietrze. Jedynie szarawa mgie³ka wokó³ karku $Z dowodzi, ¿e zetknê³a siê z nim zimna stal. Bardziej odczuwasz ni¿ s³yszysz przera¿aj±cy, upiorny, szyderczy chichot unosz±cy siê w powietrzu.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Szalony, ob³±kañczy wrzask wydobywa siê z twojego gard³a gdy zadajesz cios. Z ca³ych si³ wyprowadzasz potê¿ne ciêcie, ¶wiat wiruje przed twoimi oczami. Skupiasz siê tylko na celu. P³ywaj±cy w powietrzu rozmazany wizerunek $Z spoziera na ciebie z nienawi¶ci±. Twoja broñ wnika g³êboko, bez oporu w klatkê piersiow± przeciwnika. Szerokie smugi jaskrawoczerwonego ¶wiat³a strzelaj± w powietrze. Upiorne, dono¶ne wycie rani twe uczy, gdy wyszarpujesz $h. ¦wiat³o z sekundy na sekundê staje siê bardziej intensywne i z czasem ciemnoczerwone. $N znika w o¶lepiaj±cym rozb³ysku szkar³atnego blasku. Upiorny, pe³en potêpieñczego bólu wrzask jeszcze d³ugo unosi siê w powietrzu odbijany echem.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wrzeszcz±c ob³±kañczo z ca³ych si³ wyprowadza potê¿ne ciêcie w klatkê piersiow± $Z. Szerokie ostrze $f wnika g³êboko w niematerialne cia³o. W powietrze strzelaj± szerokie, jaskrawoczerwone smugi ¶wiat³a. Twoje uszy wype³nia pe³en bólu opêtañczy wrzask $Z, a po¶wiata nabiera g³êbszego, ciemniejszego koloru. Pó³przezroczyst± istotê zalewa purpurowe ¶wiat³o, po czym niknie ona w o¶lepiaj±cym rozb³ysku. Mru¿±c oczy ws³uchujesz siê w upiorne echo szaleñczego krzyku.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Wpatrujesz siê w rozmazan±, pó³materialn± sylwetkê przeciwnika. Zataczasz $j szerokie krêgi wyczekuj±c odpowiedniego momentu do zadania ciosu. Nie masz pojêcia czy to gdzie trafisz ma jakiekolwiek znaczenie. Przera¼liwe, pe³ne cierpienia jêki $Z przyprawiaj± ciê o dreszcze. Nie wytrzymuj±c napiêcia, bierzesz szeroki zamach i wyprowadzasz potê¿ny cios na o¶lep. Szerokie ostrze $f zag³êbia siê w pó³przezroczyste cia³o i zwalnia. Twe uszy rani przera¿aj±cy ryk jakby tysi±ca g³osów, w panice wyszarpujesz broñ. W astralnej pow³oce przeciwnika widnieje szeroka smuga od której bije czerwonawa po¶wiata.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wolno podchodzi do $Z wyczekuj±c odpowiedniego momentu do zadania ciosu. W pewnym momencie dr¿±cymi rêkoma bierze potê¿ny zamach kieruj±c ostrze $f w niematerialne cia³o. Broñ bezg³o¶nie zatapia siê w pó³przezroczystej sylwetce i nagle zwalnia jakby trafiaj±c na opór. Potworny wrzask bólu i cierpienia rozbrzmiewa po okolicy, jakby tysi±ce garde³ krzyknê³o w jednej sekundzie. Z szerokiej szramy w ciele $Z bije o¶lepiaj±ce, jaskrawoczerwone ¶wiat³o.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5¦ciskaj±c mocno stylisko $f obserwujesz p³ynne ruchy przeciwnika. P³ynie on nad ziemi± prosto w twoim kierunku. Gdy $N znajduje siê dostatecznie blisko ciebie zadajesz potê¿ny cios w bok niematerialnej istoty. Potworny jêk cierpienia rozlega siê dono¶nie po okolicy, gdy twoje ostrze nacina pó³prze¼roczysty korpus. Wyszarpuj±c broñ zauwa¿asz w boku astralnej istoty szerok± wyrwê, z której bije smuga czerwonego ¶wiat³a. Na twarzy $Z widaæ wyra¼nie nieme cierpienie ¶wiadcz±ce o skuteczno¶ci uderzenia.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n mocno ¶ciskaj±c stylisko $f czeka a¿ p³yn±cy w jego kierunku przeciwnik zbli¿y siê na dostateczn± odleg³o¶æ. Kiedy to nastêpuje wyprowadza potê¿ny cios w jego bok. Broñ zag³êbia siê w niematerialnym korpusie, pozostawiaj±c za sob± olbrzymi± wyrwê, z której promieniuje jaskrawy, czerwony strumieñ ¶wiat³a.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Wyprowadzasz potê¿ne, boczne ciêcie na korpus $Z. ¦wist powietrza, klekotanie ko¶ci i twój zduszony warkot rozbrzmiewaj± ci w uszach. Ostrze zatacza szeroki, zamaszysty ³uk b³yszcz±c przy tym w¶ciekle, $p zbli¿a siê z zastraszaj±c± prêdko¶ci± w kierunku celu, jednak dzieje siê co¶ dziwnego. Pech chcia³, ¿e rozpêdzona broñ przelatuje z miêdzy ¿ebrami przeciwnika nie czyni±c mu ¿adnej krzywdy. W zdumieniu próbujesz odzyskaæ równowagê odskakuj±c jak najdalej od nacieraj±cego szaleñczo przeciwnika.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n warcz±c opêtañczo bierze szeroki zamach i wyprowadza precyzyjne boczne ciêcie na korpus $Z. Ostrze b³yszczy w¶ciekle zataczaj±c szeroki ³uk. Z potê¿n± si³± $p pruje powietrze, jednak staje siê co¶ dziwnego. Broñ ze ¶wistem przelatuje miêdzy ¿ebrami $Z nie czyni±c ¿adnej krzywdy. Z wyrazem w¶ciek³o¶ci i zdumienia na twarzy $n odskakuje jak najdalej od wroga próbuj±c odzyskaæ równowagê.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Rycz±c w¶ciekle, tocz±c pianê z ust nacierasz na zaskoczonego przeciwnika. R±biesz z potê¿n± si³± swych napiêtych do granic ramion z do³u ku górze, ostrze $f z chrupniêciem trafia w krêgos³up na wysoko¶ci klatki piersiowej $Z posy³aj±c go wysoko w górê. Nim $N zd±¿a opa¶æ na ziemiê unosisz swoj± zabójcz± broñ i wyprowadzasz na o¶lep b³yskawiczne ciêcie. Z gruchotem ³amanych ko¶ci ostrze trafia w cel przepo³awiaj±c wroga. Obie czê¶ci $Z szybuj± w powietrzu parê metrów i wal± siê z ³oskotem na ziemiê. Zmasakrowany $N w chmurze zmursza³ych, po¿ó³k³ych od³amków i cuchn±cego, stêch³ego py³u rozpada siê na tysi±ce kawa³ków. U twych stóp toczy siê oderwana czaszka, któr± mia¿d¿ysz jednym uderzeniem buta.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n tocz±c pianê w¶ciek³o¶ci z rozchylonych ust naciera na $C. R±bniecie jest tak potê¿ne, ¿e z g³uchym chrupniêciem posy³a przeciwnika wysoko w górê. $n unosi swoje b³yszcz±ce, ¶mierciono¶ne ostrze i tnie z ca³ych si³ w lawiruj±cego w powietrzu przeciwnika, a $p trafia bezb³êdnie w cel przepo³awiaj±c go. Obie czê¶ci po¿ó³k³ego, zmursza³ego o¿ywieñca szybuj± parê metrów obracaj±c siê we wszystkich kierunkach i wal± siê z ³oskotem na pod³o¿e. Nieumar³y rozpada siê na tysi±ce czê¶ci wywo³uj±c chmurê od³amków kostnych i wiekowego, cuchn±cego py³u. $n z suchym trzaskiem wdeptuje w ziemiê tocz±c± siê u jego stóp czaszkê.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Wykrzykuj±c imiê swego boga r±biesz $j z ca³ych si³ w czaszkê przeciwnika. Broñ z prêdko¶ci± b³yskawicy opada na g³owê $Z. Zimne ostrze z suchym chrupotem zag³êbia siê w nagiej ko¶ci i wêdruje od góry czaszki, przez ziej±cy pustk± oczodó³ koñcz±c swój szlak zniszczenia tu¿ nad górn± szczêk±. Czê¶æ pustej, zmursza³ej twarzy wroga opada ciê¿ko na pod³o¿e rozbijaj±c siê w drobny mak. $N chwiej±c siê po otrzymanym uderzeniu ¶widruje ciê z nienawi¶ci± ocala³ym oczodo³em i atakuje ze zdwojon± si³±.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wykrzykuj±c z w¶ciek³o¶ci± jakie¶ nieznane ci imiê wznosi $h wysoko nad g³owê i opuszcza z szybko¶ci± pioruna w kierunku $Z. B³ysk ostrza i suchy, niemi³y chrupot ko¶ci towarzysz± temu potê¿nemu r±bniêciu. Broñ zag³êbia siê w zmursza³ej ko¶ci tn±c ze zgrzytem pust±, po¿ó³k³± twarz $Z. Jej lewa czê¶æ odrywa siê tu¿ nad rozwart± w niemym cierpieniu górn± szczêk± i z ³oskotem opada na pod³o¿e rozpadaj±c siê w mgnieniu oka w drobny mak. $N z widoczn± nienawi¶ci± b³yszcz±c± w ocala³ym, zion±cym pustk± oczodole atakuje $c ze zdwojon± sil±.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Powoli zbli¿asz siê do przeciwnika ¶ciskaj±c mocno stylisko $f. $N jest na razie zbyt czujny i nie spostrzegasz luki w jego obronie. Wyczekujesz odpowiedniego momentu do zadania ostatecznego ciosu. Nagle przeciwnik potyka siê i chwieje przez chwilê, próbuj±c odzyskaæ równowagê odchyla zmursza³y ko¶ciec lekko w ty³. Wykorzystuj±c sytuacjê unosisz $h i tniesz z ca³ych si³ na ukos. Broñ z trzaskiem opada na ¿ebra ³ami±c kilka z nich. Kawa³ki ko¶ci lec± w powietrze we wszystkich kierunkach, $N odzyskuj±c równowagê w¶ciekle rusza w twoim kierunku.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n okr±¿a ostro¿nie $C czekaj±c na odpowiedni moment do ataku. W pewnym momencie $s wróg potyka i chwieje przez chwilê próbuj±c odzyskaæ równowagê. $n b³yskawicznie zapiera siê nogami i wyprowadza szerokie ciêcie na korpus przeciwnika, $s $p zatacza zabójczy ³uk i opada z trzaskiem na dolne ¿ebra przeciwnika. Kawa³ki odciêtych ko¶ci strzelaj± w powietrze w ka¿dym z kierunków, jednak szkielet odzyskuje równowagê i w¶ciekle rusza w kierunku $z wyprowadzaj±c ¶mierciono¶ne ataki.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(race_table[GET_RACE(victim)].type , ANIMAL))
				{
					if ( dam <= 0 )
					{
						act( "{5Pêdzisz z przera¼liwym wrzaskiem w kierunku przeciwnika z uniesion± broni±. Ostrze $f zatacza nad twoj± g³ow± b³yszcz±ce, szerokie krêgi. W d³ugich susach, ¶lizgaj±c siê po pod³o¿u zbli¿asz siê z ka¿d± chwil± do $Z. Wyprowadzasz p³ynne ciêcie na ukos, orê¿ wrêcz frunie w powietrzu rzucaj±c jasne refleksy, jednak $N jest zbyt czujny, $S zwierzêcy instynkt i zabójczy refleks po raz kolejny ratuj± $S ¿agro¿one ¿ycie. Odsuwa siê w ostatniej chwili na bezpieczn± odleg³o¶æ z b³yskiem paniki w ¶lepiach.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wrzeszcz±c szaleñczo w d³ugich susach zbli¿a siê do $Z. Ostrze $f zatacza w powietrzu szerokie, ¶wietliste krêgi po czym opada uko¶nie w kierunku celu, jednak instynkt i b³yskawiczny refleks ratuj± ofiarê z opresji, $N o w³os unika zrêcznie wymierzonego na korpus r±bniêcia.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Z rykiem w¶ciek³o¶ci r±biesz z ca³ych si³ w korpus przeciwnika. Cios jest tak silny, ¿e wnika w tkankê g³êboko tn±c miêso, ko¶ci i ¿y³y. Fontanny krwi tryskaj± wysoko zalewaj±c wszystko dooko³a. W powietrze szybuj± rozerwane wnêtrzno¶ci. $N patrzy na ciebie z wyrzutem z iskr± gasn±cego ¿ywota w oczach, a ty ¶miej±c siê jak szalon<&y/a/e> wyrywasz ostrze i z rozdartych têtnic po raz wtóry buchaj± rzeki ciep³awej posoki. U twoich stóp le¿y ju¿ teraz kupa por±banego miêsa w olbrzymiej ka³u¿y ciemnoczerwonej krwi. ¦lepia $Z zachodz± mg³± i gasn±.{x", ch, wield, victim, TO_CHAR );
						act( "{5Przera¿aj±cy ryk w¶ciek³o¶ci wydobywaj±cy siê z gard³a $z unosi siê w powietrze i niesie daleko echem, r±bie $e z ca³ych si³ $j w $C. Odg³os pêkaj±cych ko¶ci, r±banego miêsa, rozrywanych ¿y³ i wnêtrzno¶ci miesza siê z ob³±kañczym okrzykiem. W górê strzelaj± gejzery ciep³ej, paruj±cej jeszcze krwi zalewaj±c wszystko strumieniami. Z szaleñczym chichotem $n wyrywa swój orê¿ ze zmasakrowanego cia³a, u $s stóp widnieje ju¿ tylko por±bane ¶cierwo, które drga jeszcze przez chwilê i nieruchomieje.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Chwytasz obur±cz stylisko swej broni i unosisz j± wysoko w górê. Bierzesz potê¿ny zamach znad g³owy i mierz±c dok³adnie opuszczasz orê¿ z zastraszaj±c± prêdko¶ci± i si³±. Ostrze tnie powietrze ze ¶miertelnym ¶wistem i opada na cel. $N kul±c siê ze strachu instynktownie próbuje unikn±æ ¶mierciono¶nego ciêcia, jednak na pró¿no. Broñ z paskudnym mlaskiem i odg³osem rwanej skóry oraz ³amanych ko¶ci zag³êbia siê w cia³o. Ciep³a, lepka posoka chlupie prosto na ciebie oraz na pod³o¿e, a powietrze rozdziera przera¼liwy kwik.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n unosi sw± broñ wysoko w górê, bierze potê¿ny zamach i opuszcza j± z zastraszaj±c± si³± na $Z, $E kul±c siê ze strachu próbuje instynktownie uskoczyæ przed morderczym r±bniêciem. Na pró¿no jednak. Ostrze zag³êbia siê w $S boku z dono¶nym trzaskiem ³amanych ko¶ci, darcia skóry oraz rozrywanego miêsa. Z rozdygotanego cia³a tryska gejzer krwi zalewaj±c wszystko dooko³a, a powietrze przecina dono¶ny, przera¼liwy kwik zwierzêcia.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Doskakujesz b³yskawicznie do przeciwnika i wyprowadzasz szybkie, krótkie r±bniêcie skierowane w sam ¶rodek korpusu. Twój orê¿ opada za ¶wistem na zdezorientowany i oszo³omiony cel. Ostrze $f szybuje za ¶wistem rzucaj±c mordercze refleksy. Jednak b³yskawiczny atak ma równie¿ swoje s³abe strony, nie zdo³a³<&e¶/a¶/o¶> odpowiednio oceniæ odleg³o¶ci i nadaæ odpowiedniego impetu broni. Czubek ostrza zahacza o bok $Z tn±c skórê. W tym samym momencie powietrze rozdziera kwikniêcie przera¿enia i bólu. Ciemna posoka chlupie dooko³a barwi±c wszystko na czerwono.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n doskakuje b³yskawicznie do $Z w morderczym ataku. Wyprowadza szybkie, krótkie r±bniêcie w korpus $Z chc±c zakoñczyæ walkê w u³amku sekundy, jednak zbyt ma³a si³a ciosu i ¼le wymierzona odleg³o¶æ powoduje, i¿ jedynie koniec ostrza zahacza o bok $Z tn±c $M skórê z odg³osem darcia. W powietrze unosi siê przera¼liwe kwikniêcie strachu i bólu.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
			case WEAPON_SWORD:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wiêksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Pêdzisz wprost na $Z w d³ugich susach wznos±c nad g³ow± $p. Broñ twa ko³ysze siê miarowo w rytm szaleñczego biegu. Opuszczasz g³owniê z zastraszaj±c± si³±, lekko pod skosem kieruj±c j± wprost w kark przeciwnika, jednak w ostatnim momencie tracisz równowagê. Ostrze przecina ze ¶wistem powietrze obok o¿ywieñca. Jeste¶ teraz niebezpiecznie blisko wroga i nie mo¿esz z³apaæ równowagi tak szybko jakby¶ chcia<&³/³a/³o>. Wci±gasz w nozdrza odór, oddalonego od ciebie zaledwie o cale, gnij±cego cia³a, od którego krêci ci siê w g³owie jeszcze bardziej.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak w kilku susach $n dopada $Z wznosz±c $h nad g³ow±, jednak zadaj±c ciêcie skierowane widocznie ku szyi przeciwnika traci równowagê i chybia. Próbuj±c odzyskaæ równowagê szar¿uj±cy z trudno¶ci± unika gradu ciosów.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Chwytasz obur±cz $h za rêkoje¶æ niczym sztylet i unosisz nad g³owê. B³yszcz±ca g³ownia skierowana jest w dó³. Ramiona dr¿± od wysi³ku, lecz cios ten wymaga ogromnej krzepy. W przegni³ym mózgu $Z pojawia instynktowna my¶l o rych³ym jego koñcu. Ze ¶wistem opuszczasz klingê. Wbijasz j± tu¿ poni¿ej karku ozywieñca z zastraszaj±c± si³±, ostrze penetruje cia³o pionowo w dó³ na wylot przez ca³y korpus. Sztych przechodzi g³adko, z g³o¶nym mla¶niêciem przez pachwinê o¿ywieñca i wbija siê w pod³o¿e. Strumienie cuchn±cej cieczy tworz± ogromne ka³u¿e nad podryguj±cym jeszcze resztk± nienaturalnego ¿ycia cia³em. $N wydaje z siebie ostatni przepe³niony bólem skrzek i kona.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n chwyta nienaturalnie $h dzier¿±c go jak sztylet. B³yskawicznym ruchem unosi broñ do góry i jeszcze szybciej opuszcza w dó³ z zastraszaj±c± si³±. Cia³em $Z wstrz±sa nag³y skurcz. G³ownia przesz³a g³adko na wylot z paskudnym, dono¶nym chrzêstem wbijaj±c siê w ziemiê. Nadziany jak na pal o¿ywieniec wydaje z siebie ostatni pe³en bólu i rozpaczy skrzek, po czym powoli osuwa siê po ostrzu na ziemiê, prosto w ogromn± ka³u¿ê w³asnych p³ynów.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Okr±¿asz $C ¶ciskaj±c mocno $h i wyczekujesz odpowiedniego momentu by uderzyæ. Przeciwnik sunie w twoim kierunku mechanicznymi, makabrycznymi ruchami wydobywaj±c z martwych p³uc skrzek, po którym przechodz± ci ciarki po grzbiecie. Zbli¿aj±c siê do ciebie pochyla siê, aby zadaæ cios. ¦ciskaj±c mocniej trzpieñ $f odskakujesz o krok i wykonujesz potê¿ne pchniêcie. G³ownia rzucaj±c ¶wietlne refleksy uderza z mordercz± si³± b³yskawicy. Wbijasz w brzuch nieumar³ego szerokie ostrze na ca³y sztych. Z rzê¿±cych ust przeciwnika chlupie rzeka przezroczystej cieczy kiedy bezlito¶nie przekrêcasz mu miecz w ciele i wyszarpujesz go.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n szuka luki w obronie przeciwnika. Kiedy $N ods³ania siê by zaatakowaæ, $n b³yskawicznie robi krok w ty³, cofa lekko $h, by w nastêpnej sekundzie nadziaæ na niego sw± ofiarê. Ostrze wbija siê a¿ na sztych w brzuch nieumar³ego. Z paszczy o¿ywieñca bucha cuchn±ca jucha, a przera¿aj±ce, pe³ne cierpienia charczenie dociera do twych uszu. $n z grymasem szaleñstwa na twarzy naciera ponownie.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Nacierasz zdecydowanie na $Z. Bierzesz potê¿ny zamach i kierujesz szerok± g³owniê $f z góry ku do³owi lekko zakrzywiaj±c tor ciêcia, które przesz³o na ukos przez klatkê piersiow± i rozora³o tu³ów o¿ywieñca a¿ po dolne ¿ebra. Nieumar³y podtrzymuj±c wyprute wnêtrzno¶ci, wyj±c niemi³osiernie i chlapi±c cuchn±c± ciecz± zdwoi³ w ¶lepej furii si³ê swych ataków.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl±dasz na scenê, która wywo³uje u ciebie torsje. $n zadaje potê¿ne ciêcie $X, które patroszy go niczym prosiaka u rze¼nika. Grube wêz³y przegni³ych flaków o¿ywieñca zwisaj± mu z szerokiej, paskudnej szramy na korpusie. Hektolitry cuchn±cej cieczy bryzga na wszystko wokó³, jednak $N nadal stoi chwiejnie na nogach i stawia opór.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cieñ" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiór" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Wyskakujesz do góry, celuj±c $j w $C, jednak pech sprawi³ i¿ twój cel uchyli³ siê i twa broñ wbi³a siê w ziemie tu¿ obok niematerialnego cia³a $Z.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n wyskakuje do góry i celuje sw± broni± w $C, jednak cel uchyli³ siê i $f $z wbija siê w ziemiê, tu¿ obok niematerialnego cia³a.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Bierzesz potê¿ny rozbieg i z w¶ciek³o¶ci± rzucasz siê na $C. Twa broñ zatacza krêgi wzniesiona wysoko i b³yszczy upiornie gdy twój rze¼nicki cios spada na próbuj±cego siê os³oniæ przeciwnika. Ostrze $f g³adko wchodzi w eteryczne cia³o tn±c je i rozrywaj±c, jego kawa³ki odpadaj± i znikaj± po chwili. Suniesz g³adko, od g³owy, przez rêce po sam dó³. Wstajesz z klêczków i z satysfakcj± patrzysz jak dwie czêsci $Z rozp³ywaj± siê powoli a potêpiencze jêki gasn± w niebycie.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze potê¿ny rozbieg i z w¶ciek³ym rykiem rzuca siê na $C. Ostrze $f zatacza krêgi, wznie¶ione wysoko upiornie b³yszczy gdy rzê¼nicki cios spada na próbuj±cego siê os³oniæ ducha. Broñ $z g³adko wchodzi w eteryczne cia³o, tn±c je, rozrywaj±c. Odpadaj±ce kawa³ki znikaj± po chwili, roztapiaj± siê od³±czone od swego niematerialnego cia³a. Cios przeszed³ od góry po sam dó³. Po chwili $n wstaje i z satysfakcj± patrzy jak powoli rozp³ywaj± siê dwie czêsci ducha a potêpiencze jêki cichn± coraz bardziej.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Bierzesz krótki rozbieg i odbijasz siê od ziemi. Twa broñ z olbrzymi± prêdko¶ci± trafia w pó³materialne cia³o $Z i przeszywaj±c je na wylot jak nó¼ wbija siê w mas³o. $N wydaje z siebie potêpieczy jêk, jakby siê do czego¶ szykowa³, jednak szybko wyci±gasz $h z $S powoduj±c wielkie zniszczenia.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze krótki rozbieg i odbija siê od ziemi. Jego broñ bezb³êdnie trafia w niematerialne cia³o $Z przeszywaj±c je na wylot i czyni±c wielkie zniszczenia. $N wydaje z siebie potêpienczy jêk i probowa³ uderzyæ $c, jednak niezd±¿a siê nawet powa¿nie zamachn±æ kiedy przeciwnik wyci±ga z sw± broñ rani±c $C jeszcze powa¿niej.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Z olbrzymi± prêdkosci± i dzikim rykiem rzucasz siê na $C, wymachuj±c $j. Przeciwnik próbowa³ uchyliæ siê, lecz twa broñ przyszpili³± go do ziemi z ogromn± si³±. Potêpienczy jêk, jaki wyda³ duch sprawi³, ¿e a¿ zabola³y ciê uszy. Jednak z g³o¶nym warkotem wyszarpno³e¶ broñ z niematerialnego, szamocz±cego siê bez³adnie cia³a. $N rzuca siê do walki z wsciek³o¶ci±.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z olbrzymi± prêdkosci± i dzikim rykiem rzuca siê na $C, wymachuj±c dooko³a $j. Unik, jaki próbowa³ zrobiæ cel, nie uda³ siê i broñ $z przyszpili³a go do ziemi z wielk± si³±. $N wrzasn±³ potêpiêñczo, po czym $n sprawnym ruchem wyszarpn±³ broñ z szamocz±cego siê, niematerialnego cia³a, które jednak podnios³o siê i rzuci³o do walki z wsciek³osci±. Wszystko sta³o siê w jednej chwili.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Bierzesz silny i szeroki zamach zamierzaj±c wykonaæ potê¿ne ciêcie znad g³owy w ods³oniêt± czaszkê $Z. Opuszczasz ze ¶wistem szerok± klingê $f, prowadz±c j± precyzyjnie ku obranemu celowi. Jednak ¶liska od potu rêkoje¶æ przekrêca ci siê w d³oniach i o ma³o co nie wypuszczasz broni z r±k. Ostrze opada z zastraszaj±c± si³± i uderza p³azem w tward± czaszkê powoduj±c zdrêtwienie ramion. G³ownia z g³uchym brzêkiem odbija siê od ko¶cianej g³owy nie czyni±c przeciwnikowi ¿adnej krzywdy.{x", ch, wield, victim, TO_CHAR );
						act( "{5Obserwujesz jak $n unosi nad g³owê b³yszcz±ce ostrze $f w potê¿nym zamachu. Kieruje klingê na ods³oniêt± czaszkê $Z ze ¶wistem tn±c powietrze. Do twych uszu dociera dono¶ny, g³uchy brzêk kiedy szeroka g³ownia miecza odskakuje od czaszki nie wyrz±dzaj±c o¿ywieñcowi ¿adnej krzywdy. $n haniebnie uderzy³ p³azem, widocznie niedostatecznie mocno z³apana rêkoje¶æ wy¶lizgnê³a siê i ostrze przekrêci³o siê w szaleñczym locie.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5W szaleñczych susach zbli¿asz siê do $Z z $j w rêkach. Mijaj±c przeciwnika zataczasz broni± ciasny, morderczy ³uk i tniesz z ca³ych si³. Ostrze opada ze ¶wistem na podstawê czaszki, odr±bana ko¶ciana g³owa szkieletu zatacza szeroki ³uk w powietrzu obracaj±c siê wokó³ w³asnej osi. Nastêpnie przyklêkasz plecami do przeciwnika i wyprowadzasz kolejne mordercze ciêcie wykonuj±c nag³y skrêt cia³a. Tym razem g³ownia z trzaskiem opada na krêgos³up tu¿ nad miednic± oddzielaj±c korpus od nóg. Odr±bane czê¶ci szkieletu z ³oskotem wal± siê na ziemiê w tym samym czasie otaczaj±c tw± nieruchom±, skulon± sylwetkê.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n b³yskawicznie zbli¿a siê do $Z w szaleñczym biegu. Kiedy zrównuje siê ze szkieletem g³ownia $s $f zatacza ciasny ³uk i opada z zastraszaj±c± si³± na podstawê czaszki. Odciêta g³owa $Z szybuje wysoko w powietrze obracaj±c siê wokó³ w³asnej osi. $n mija przeciwnika i przyklêka zaraz za nim wyprowadzaj±c kolejne precyzyjne ciêcie. Wykonuj±c nag³y skrêt cia³a przecina krêgos³up tu¿ nad miednic± przeciwnika odr±buj±c korpus od nóg. Poæwiartowane czê¶ci szkieletu szybuj± przez chwilê w powietrzu i opadaj± równocze¶nie z ³oskotem wokó³ nieruchomej sylwetki $z.{x", ch, wield, victim, TO_NOTVICT );
						make_head( victim );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Mocniej ¶ciskasz $h patrz±c na klekocz±cy przy ka¿dym ruchu szkielet. Unosisz b³yskawicznie b³yszcz±ce ostrze w górê i mocno tniesz na ukos przez klatkê piersiow± $Z. Zimna stal zgrzyta o suche, zmursza³e ko¶ci w piekielnym ha³asie, w powietrze szybuj± odciête, ostre kawa³ki zmia¿d¿onych ¿eber wraz z tumanem stêch³ego, wiekowego py³u. $N chwieje siê przez chwilê na ko¶cistych nogach, a twoja uniesiona ponownie klinga b³yszczy w morderczych refleksach.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n unosi wysoko w górê $h. Jego ciêcie dosiêga klatki piersiowej $Z i biegnie na ukos koñcz±c sw± mordercz± wêdrówkê po przek±tnej ko¶cianego korpusu zadaj±c mordercze obra¿enia. Dono¶ny zgrzyt przyprawia ciê o zimne dreszcze. W niemym cierpieniu $N odchyla sw± czaszkê w ty³ szeroko rozchylaj±c szczêkê. Ostre kawa³ki po³amanych ¿eber oraz chmura stêch³ego py³u wêdruj± w powietrze tworz±c mglist± zas³onê. Nieumar³y chwieje siê na ko¶cianych nogach pod si³± uderzenia.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Biegn±c w kierunku $Z wykonujesz szaleñczy skok, szykuj±c siê do morderczego pchniêcia, potykasz siê jednak i na o¶lep zamachujesz siê $j rozpaczliwie próbuj±c odzyskaæ równowagê. Ostrze $f, bardziej przez przypadek ni¿ w zamierzonym ciosie, trafia w miednicê szkieletu zag³êbiaj±c siê tam z cichym zgrzytem i brzêkiem. Wielki kawa³ od³upanej ko¶ci z g³uchym ³oskotem l±duje na pod³o¿u. Wyszarpuj±c poszczerbione ostrze z trudem unikasz b³yskawicznej kontry przeciwnika.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n odbija siê i wykonuje szaleñczy skok w kierunku $Z. L±duj±c na ziemi stopa atakuj±cego niebezpiecznie wygina siê ku ¶rodkowi gro¿±c skrêceniem kostki. Aby tego unikn±æ $n przerzuca ciê¿ar cia³a na drug± koñczynê machaj±c na o¶lep mieczem dla lepszego odzyskania równowagi. Ostrze przez przypadek trafia w zmursza³± miednicê zaskoczonego szkieletu, wielki kawa³ od³upanej ko¶ci z ³oskotem opada na pod³o¿e. Wyszarpuj±c $h $n z trudem unika w¶ciek³ych ciosów $Z.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(victim->form, FORM_CONSTRUCT))
				{
					if ( dam <= 0 )
					{
						act( "{5Z b³yskiem w oku wyprowadzasz potê¿ne pchniêcie. Ostrze pêdzi z niebywa³± prêdko¶ci± w kierunku sylwetki $Z. Klinga tn±c powietrze dociera do klatki piersiowej celu, jednak nie by³a to dobra strategia. G³ownia uderza w tors wroga prawie rozpadaj±c siê na kawa³ki i odbija z metalicznym brzêkiem. Cios nie wyrz±dzi³ przeciwnikowi praktycznie ¿adnej szkody. Potrz±saj±c g³ow± zataczasz siê rozpaczliwie jak najdalej od wyci±gniêtych w twym kierunku ramion chc±c unikn±æ potê¿nego kontrataku.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wyprowadza mordercze pchniecie w tors przeciwnika. Ostrze tn±c powietrze osi±ga bez problemu swój cel, jednak odbija siê nie czyni±c $X ¿adnej szkody. Si³a uderzenia jest tak mocna, ¿e ramionami $z wstrz±sa bolesny dreszcz a g³ownia rozpada siê prawie na kawa³ki. Gdyby ostrze trafi³o na organiczne tkanki by³oby praktycznie po pojedynku, jednak tak siê nie sta³o. $n rozpaczliwie zataczaj±c siê jak najdalej od wyci±gniêtych ramion wroga odzyskuje równowagê i rozlu¼nia rêce, aby pozbyæ siê natarczywego, narastaj±cego w nich odrêtwienia.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Z furi± ob³±kañca nacierasz na $C. W szaleñczym biegu krêcisz m³yñca nad g³ow± $j i dopadaj±c nieruchomego przeciwnika, po czym w u³amku sekundy tniesz z góry z ca³ych si³. Ostrze opada z mia¿d¿±c± si³± na kark wroga i wdziera siê w twarde cia³o ze zgrzytem. Lawina iskier zasypuje twoj± rozw¶cieczon± twarz przes³aniaj±c na chwilê widoczno¶æ. Odciêty ³eb wraz z prawym ramieniem i kawa³kiem torsu opada z g³uchym ³oskotem na ziemiê, ko³ysze siê chwilê i nieruchomieje. Jednak to ci nie wystarcza. Aby daæ upust szaleñczej furii tniesz po raz wtóry. Tym razem ostrze z metalicznym brzêkiem opada na dolne koñczyny odrywaj±c je z paskudnym zgrzytem na wysoko¶ci kolan. Cia³o $Z wali siê na ziemiê i rozpada na kawa³ki.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z wyrazem szaleñstwa i w¶ciek³o¶ci na twarzy szar¿uje na bezdusznego przeciwnika. Uniesiony orê¿ zatacza w powietrzu b³yszcz±ce, mordercze krêgi, a barbarzyñca w u³amku sekundy dopada $Z i tnie z ca³ych si³. Klinga $f ze zgrzytem zag³êbia siê na ukos w twardym, nienaturalnym karku, a w powietrze strzela lawina iskier poch³aniaj±c obu przeciwników. Opada jednak na czas by ods³oniæ wal±cy siê ze stukotem czerep wraz z rêk± i kawa³kiem torsu $Z. $x to jednak nie wystarcza. Z rykiem w¶ciek³o¶ci tnie ponownie, ostrze opada na dolne koñczyny $s ofiary odrywaj±c je od reszty zmasakrowanego cia³a. Sylwetka $Z wali siê z g³uchym ³oskotem na pod³o¿e i kruszy siê na kawa³ki.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5¦ciskaj±c orê¿ zbli¿asz siê do przeciwnika w b³yskawicznym doskoku, $p zatacza w powietrzu ¶mierciono¶ny ³uk i ze ¶wistem opada na korpus $Z. Ostrze ze zgrzytem sunie po nienaturalnie twardym ciele, rozgrzane odpryski materia³u, z którego jest wykonane strzelaj± w powietrze tworz±c istny grad.{x", ch, wield, victim, TO_CHAR );
						act( "{5Rozpêdzona klinga $f $z zatacza szeroki ³uk i ze ¶wistem opada na korpus $Z. Z g³uchym stukotem ostrze dochodzi celu i ze zgrzytem sunie po nienaturalnym ciele wywo³uj±c grad odprysków.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5G³ownia $f zatacza szeroki, ¶wietlisty ³uk opadaj±c z zastraszaj±c± si³± na g³owê przeciwnika. Ostrze ze ¶wistem uderza nienaturalny ³eb $Z, jednak¿e ze¶lizguje siê bez szkody po kr±g³o¶ciach czerepu wywo³uj±c fontannê iskier. Sztych miecza zsuwaj±c siê z impetem l±duje na prawym ramieniu $Z i zag³êbia siê w nienaturalnym ciele. Wyszarpujesz ze zgrzytem orê¿ potê¿nym szarpniêciem i odskakujesz rozpypuj±c wydarte kawa³ki sztucznego cia³a.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n zamierza siê na $C bior±c potê¿ny zamach. Ostrze $f zatacza szeroki ³uk i opada z zastraszaj±c± si³± na czerep celu, jednak ze¶lizguje siê bez szkody po ³bie i l±duje na ramieniu zag³êbiaj±c siê w twardym korpusie. $n odskakuj±c wyszarpuje b³yszcz±ce ostrze z paskudnym zgrzytem w fontannie iskier i wyszarpniêtych kawa³ków sztucznego cia³a.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(race_table[GET_RACE(victim)].type , ANIMAL))
				{
					if ( dam <= 0 )
					{
						act( "{5Nie czekaj±c ani chwili d³u¿ej, wydajesz z siebie g³o¶ny okrzyk i rozpoczynasz bieg w stronê $Z, wysoko wznosz±c $h. Na twe nieszczê¶cie, ¼le wyczu³e¶ sytuacjê, gdy¿ twej ofierze wystarczy³ jedynie lekki unik, by usun±æ siê z pola ra¿enia. Twoja broñ, spadaj±ca z wielk± prêdko¶ci± tnie powietrze, a sam z trudem utrzymujesz równowagê.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n nie zastanawiaj±c siê nad lepsz± sposobno¶ci± wyprowadza szar¿ê w stronê $Z. Tym razem najwyra¼niej odpowiedni moment nie zosta³ wyczuty i zwierzak jednym, lekkim unikiem schodzi z pola ra¿enia spadaj±cego z impetem miecza, tym samym prawie posy³aj±c $c na ziemiê.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Nuc±c sw± plemienn± pie¶ñ wojenn±, rzucasz siê z okrzykiem na $C, a $p zaczyna wirowaæ w zatrwa¿aj±cym tempie. Czujesz, jak twe cia³o jest wprost rozdzierane przez wytê¿one w nieludzkim wysi³ku miê¶nie, a pulsuj±ce ¿y³y przebijaj± siê przez skórê. Bêd±c u kresu drogi, krêcisz nad g³ow± jeszcze ostatniego m³yñca g³o¶no ¶wiszcz±cym orê¿em i wymierzasz potê¿ny cios w poprzek cia³a zwierzaka. Ostrze przechodzi na wylot, a dwie po³ówki opadaj± w powiêkszaj±c± siê ka³u¿ê krwi, wci±¿ jeszcze drgaj±c.{x", ch, wield, victim, TO_CHAR );
						act( "{5Z ust $z dobywa siê zapewne pie¶ñ bitewna, w nieznanym ci narzeczu, a $e bierze rozpêd i rzuca siê na $C, wymachuj±c $j w ob³±kañczym tempie. Widoczna teraz doskonale góra miê¶ni zaci¶niêta przez sznury niebieskich, pulsuj±cych ¿y³ wydaje siê wprost rozdzieraæ $s skórê. Bêd±c u celu zmusza siê do jeszcze jednego nadludzkiego wysi³ku i wymierza potê¿ny cios, tn±c w poprzek cia³a $Z. Dwie po³ówki wstrz±sane ostatnimi drgawkami, opadaj± w ka³u¿ê krwi i nieruchomiej±.{x", ch, wield, victim, TO_NOTVICT );
						make_blood(victim);
					}
					else if (number_range(1,2) == 2 && IS_SET( ch->parts, PART_LEGS ) )
					{
						act( "{5Pod¶wiadomie wyczuwaj±c odpowiedni moment, poprawiasz chwyt na rêkoje¶ci $f i rozpoczynasz i¶cie sprinterski bieg w stronê nagle ods³aniaj±cego siê zwierzaka. U celu bierzesz g³êboki wdech, wydajesz z siebie szaleñczy ryk i tniesz precyzyjnie, podcinaj±c ¶ciêgna jednej z koñczyn $Z. Twój miecz przechodzi przez miêsieñ jak przez mas³o, wiêc udaje ci siê jeszcze wycofaæ i uchyliæ przed kontratakiem.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n ni st±d, ni zow±d, zaczyna szaleñczo wymachiwaæ $j, wtóruj±c mu przy tym prêdko¶ci± pracy nóg. Bacznym wzrokiem wpatruje siê w sylwetkê $Z. Kiedy nagle zwierzê siê ods³ania, dwurêczny orê¿ spada na koñczynê zwierzaka i podcina jeden z miê¶ni.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Z krzykiem rzucasz siê na ods³oniête cia³o $Z i starasz siê wymierzyæ koñcz±cy cios wiruj±c± niesamowicie szybko kling± $f, celuj±c w poprzek linii karku. Twa domniemana ofiara robi jednak gwa³towny unik, a miecz spada tylko na jej korpus rozcinaj±c skórê i arterie.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z krzykiem rzuca siê na ods³oniête cia³o $Z i wymachuj±c szaleñczo $f tnie, celuj±c zapewne w kark ofiary. Zwierzê jednak, przewiduj±c $s zamiary, stara siê odskoczyæ jak najdalej, co udaje siê tylko po³owicznie. Ostrze spada na korpus $Z rozcinaj±c skórê i arterie.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if ( dam <= 0 )
				{
					act( "{5$p ze ¶wistem przecina powietrze o w³os chybiaj±c $C.{x", ch, wield, victim, TO_CHAR );
					act( "{5$p $z ze ¶wistem przecina powietrze o w³os ciê chybiaj±c.{x", ch, wield, victim, TO_VICT );
					act( "{5$p $z ze ¶wistem przecina powietrze o w³os chybiaj±c $C.{x", ch, wield, victim, TO_NOTVICT );
					return TRUE;
				}
				else if ( victim->hit - dam < -11 )
				{
					int death_mesg[ 5 ], i;

					for ( i = 0; i < 5;i++ )
						death_mesg[ i ] = 0;

					i = 1;

					death_mesg[ 0 ] = 0;

					if ( IS_SET( victim->parts, PART_HEAD ) )
						death_mesg[ i++ ] = 1;
					if ( IS_SET( victim->parts, PART_BRAINS ) )
						death_mesg[ i++ ] = 2;
					if ( IS_SET( victim->parts, PART_HEART ) && wield->value[ 0 ] != WEAPON_AXE )
						death_mesg[ i++ ] = 3;
					if ( !IS_NPC( victim ) && victim->size <= SIZE_MEDIUM )
						death_mesg[ i++ ] = 4;

					switch ( death_mesg[ number_range( 0, i - 1 ) ] )
					{
						case 0:
							act( "{5$p ze ¶wistem spada na $C g³êboko wbijaj±c siê w $S cia³o.{x", ch, wield, victim, TO_CHAR );
							act( "{5$p $z ze ¶wistem spada na ciebie g³êboko wbijaj±c siê w twoje cia³o.{x", ch, wield, victim, TO_VICT );
							act( "{5$p $z ze ¶wistem spada na $C g³êboko wbijaj±c siê w $S cia³o.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 1:
							act( "{5Uderzenie siêga g³owy $Z, odcinaj±c j± i odrzucaj±c jej zmasakrowane szcz±tki na ziemiê.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n wyprowadza morderczy cios, przez chwilê ¶wiat wiruje po czym twoje bezg³owe cia³o osuwa siê na ziemiê.{x", ch, wield, victim, TO_VICT );
							act( "{5$p $z ze ¶wistem spada na g³owê $Z odcinaj±c j± od reszty cia³a.{x", ch, wield, victim, TO_NOTVICT );
							if ( IS_NPC( victim ) && victim->pIndexData->corpse_vnum < 0 )
								act( "{5Odciêta g³owa $Z po chwili znika.{x", ch, wield, victim, TO_ROOM );
							else
								make_head( victim );
							break;

						case 2:
							act( "{5Cios wychodzi ci wyj±tkowo dobrze - rozp³atujesz czaszkê $Z i rozchlapujesz $S mózg.{x", ch, wield, victim, TO_CHAR );
							act( "{5To nie jest twoja szczê¶liwa chwila, $n uderza bardzo mocno rozp³atuj±c ci czaszkê i rozchlapuj±c twój mózg.{x", ch, wield, victim, TO_VICT );
							act( "{5To nie by³a szczê¶liwa chwila dla $Z. Uderzenie $z rozp³atuje $O czaszkê rozchlapuj±c mózg.{x", ch, NULL, victim, TO_NOTVICT );
							make_brains(victim);
							break;

						case 3:
							act( "{5Dobrze wyprowadzony sztych nadziewa $C na ostrze przechodz±c przez samo serce.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n wbija ci ostrze swojej broni prawie po rêkoje¶æ, przebijaj±c serce.{x", ch, wield, victim, TO_VICT );
							act( "{5$n wbija $p w $C prawie po rêkoje¶æ przeszywaj±c $S serce.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 4:
							act( "{5Z pe³nego zamachu tniesz $C w po³owie, rozr±buj±c $S cia³o na dwie czê¶ci. Poodr±bywane koñczyny z chlupotem opadaj± na ziemiê, a ciebie zalewa fontanna krwii{x", ch, wield, victim, TO_CHAR );
							act( "{5$n z pe³nego zamachu tnie ciê na wysoko¶ci brzucha, rozr±buj±c na pó³.{x", ch, wield, victim, TO_VICT );
							act( "{5Zamaszyste ciêcie $z spada na $C, rozr±buj±c $S cia³o na pó³. Poodr±bywane koñczyny z chlupotem opadaj± na ziemiê, a fontanna krwii zalewa $c.{x", ch, NULL, victim, TO_NOTVICT );
							make_arm(victim);
							make_leg(victim);
							make_blood(victim);
							break;
					}
					return TRUE;
				}
				else
				{
					act( "{5$p ze ¶wistem spada na $C g³êboko wbijaj±c siê w $S cia³o.{x", ch, wield, victim, TO_CHAR );
					act( "{5$p $z ze ¶wistem spada na ciebie g³êboko wbijaj±c siê w twoje cia³o.{x", ch, wield, victim, TO_VICT );
					act( "{5$p $z ze ¶wistem spada na $C g³êboko wbijaj±c siê w $S cia³o.{x", ch, wield, victim, TO_NOTVICT );
					return TRUE;
				}
				break;

			case WEAPON_POLEARM:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wiêksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Bez uprzedniego zastanowienia, próbujesz jak najszybciej zadaæ bolesny cios niepokoj±cemu ciê martwiakowi. Bierzesz du¿y rozpêd, wznosisz nad g³owê $h i wykonuj±c potê¿ny zamach, starasz siê trafiæ w tors $Z. Niestety, albo wybra³<&e¶/a¶/o¶> sobie z³ego przeciwnika, albo brakuje ci wprawy, albo niezbyt siê do tego ataku przygotowa³e¶. Pud³ujesz o dobre pó³ metra, a na dodatek z³ego walisz siê jak bela na ziemiê. Nie zastanawiaj±c siê d³u¿ej podnosisz siê na nogi i kontynuujesz walkê.{x", ch, wield, victim, TO_CHAR );
						act( "{5Dostrzegasz, jak $n bez uprzedniego zastanowienia, próbuje jak najszybciej zadaæ cios niepokoj±cemu go, natrêtnemu martwiakowi. Bierze du¿y rozpêd, wznosi nad g³owê $h i wykonuj±c potê¿ny zamach, stara siê trafiæ w tors $Z. Niestety, brakuje $m wprawy, albo niezbyt siê do tego ataku przyk³ada, bo pud³uje o dobre pó³ metra, a na dodatek wali siê jak bela na ziemiê. Nie zastanawiaj±c siê d³u¿ej podnosi siê na nogi i kontynuuje walkê.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5K±tem oka spogl±dasz na sw± przysz³± ofiarê i ¶ciskaj±c kurczowo $h rozpoczynasz szaleñczy bieg. W twych oczach zapala siê iskra ob³êdu, z piersi wydobywa siê duszony dot±d skrzêtnie krzyk. Z sekundy na sekundy nabierasz prêdko¶ci, a twe miê¶nie zaczynaj± pracowaæ z nadludzk± si³±. Gdy dobiegasz do $Z skupiasz siê na swoim celu jak tylko mo¿esz i z wielkim impetem, wprawnym ruchem $f dokonujesz ostatecznego ciosu i nie trac±c na szybko¶ci biegniesz dalej. Momentalnie do twych uszu dobiega piekielny, ¶miertelny wrzask i dwa g³uche mla¶niêcia. Obracasz siê na piêcie i dostrzegasz w ka³uzê krwi, ¶mierdz±ce trupim jadem wnêtrzno¶ci i cia³o $Z rozpo³owione na dwie czê¶ci.{x", ch, wield, victim, TO_CHAR );
						act( "{5Bacznie obserwuj±c ca³e zaj¶cie dostrzegasz jak $n ¶ciskaj±c kurczowo w obu d³oniach $h koncentruje na $B ca³± sw± uwagê i rozpoczyna szaleñczy bieg. Widzisz, jak w $s oczach zapala siê iskra ob³êdu, a do twych uszu dolatuje wydobywaj±cy siê z piersi atakuj±cego przeci±g³y, og³uszaj±cy krzyk. Z sekundy na sekundê nabieraj±c prêdko¶ci skupia $e siê najbardziej jak tylko mo¿e na bezb³êdnym wykonaniu potê¿nej szar¿y. W koñcu, gdy dobiega do swego celu z wielkim impetem, wprawnym ruchem rêki dokonuje potê¿nego, finalnego ciosu i nie trac±c na szybko¶ci biegnie dalej. Momentalnie do twych uszu dobiega piekielny, ¶miertelny wrzask a twym oczom ukazuje siê wprost makabryczny widok. $N chwiej±c siê na nogach, w przeci±gu sekundy dos³ownie traci ca³y korpus, który z g³uchym pla¶niêciem opada na ziemiê. Po chwili w jego ¶lady id± utrzymuj±ce siê dot±d jakim¶ cudem pionowo nogi.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Trzymaj±c obur±cz $h wydajesz z siebie d³ugi, szaleñczy wrzask i rozpoczynasz ów straszliwy bieg, który wykonuj± wielcy wojownicy gdy pragn± rozpocz±æ lub zakoñczyæ walkê w ¶wietnym stylu. Twe stopy miarowo, aczkolwiek bardzo szybko uderzaj± o posadzkê, odg³os twego dono¶nego warczenia odbija siê zewsz±d dono¶nym, g³uchym echem. Wznosisz wysoko nad g³owê swój orê¿ i bêd±c u celu dokonujesz makabrycznego, potê¿nego ciêcia. Ostrze $f ze ¶wistem przecina powietrze i rozkraja cia³o martwiaka z niesamowit± lekko¶ci±. $N wydaje potworny wrzask bólu, a z wielkiej rany bucha ¶mierdz±ca, przeklêta krew i trupi jad.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl±dasz z ukosa na $c. Trzymaj±c obur±cz $h wydaje $e z siebie d³ugi, szaleñczy wrzask i rozpoczyna ów straszliwy bieg, który wykonuj± wielcy wojownicy, pragn±c rozpocz±æ lub zakoñczyæ walkê w ¶wietnym stylu. S³yszysz jak $s stopy miarowo, aczkolwiek bardzo szybko uderzaj± o posadzkê, a odg³os okropnego warczenia odbija siê zewsz±d dono¶nym, g³uchym echem. Spogl±dasz nañ ponownie. Teraz wznosi wysoko nad g³owê swój orê¿ i bêd±c u celu dokonuje makabrycznego, potê¿nego ciêcia. Ostrze $s $f ze ¶wistem przecina powietrze i rozkraja cia³o $Z z niesamowit± lekko¶ci±. O¿ywieniec wydaje potworny wrzask bólu.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Wci±¿ panuj±c nad sytuacj±, dostrzegasz moment, w którym $C w ogóle nie obchodzi co teraz robisz. Nie czekaj±c na kolejn± tak± dobr± sposobno¶æ, oceniasz odleg³o¶æ miêdzy twoj± osob±, a t± przebrzyd³±, mroczn± kreatur± i rozpoczynasz szar¿ê. Bêd±c wystarczaj±co blisko przeciwnika, zni¿asz siê nieco, po czym wykonujesz efektywny ¶lizg pomiêdzy jego nogami unosz±c jak najwy¿ej $j. S³yszysz, jak twa broñ z ohydnym mlaskiem przebija siê przez nieroz³o¿on± jeszcze, ¿yw± tkankê martwiaka, po czym do twych uszu dochodzi g³o¶ne chrupniêcie, ¶wiadcz±ce o naruszeniu ko¶ci u nogi. Ogl±dasz siê za siebie i widzisz, ¿e $N w miejscu krocza ma poka¼nych rozmiarów dziurê, a jego noga zwisa zaledwie na paru ¶ciêgnach.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl±daj±c na $c i zauwa¿asz, i¿ na $s ustach pojawia siê szeroki u¶miech a twarz nieco siê rozja¶nia. Widocznie widzi co¶, co $m bardzo w tej chwili pasuje... B³yskawicznie, nie czekaj±c na drugi taki moment rozpoczyna szar¿ê. Gdy jest ju¿ wystarczaj±co blisko przeciwnika, zni¿a siê nieco, po czym wykonuje efektowny ¶lizg pomiêdzy jego nogami z unosz±c jak najwy¿ej $j. Po chwili s³yszysz, jak broñ $z z ohydnym mlaskiem przebija siê przez nieroz³o¿on± jeszcze, prawie ¿yw± tkankê $Z, po czym do twych uszu dochodzi g³o¶ne chrupniêcie, ¶wiadcz±ce o naruszeniu ko¶ci u nogi. Nagle, dostrzega¿ jak porz±dny kawa³ cia³a $Z, urzêduj±cy dot±d w miejscu krocza oddziela siê od reszty i z g³o¶nym plaskiem upada na ziemiê.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cieñ" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiór" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Z wyci±gniêt± przed siebie broni± pêdzisz w d³ugich susach w kierunku $Z. Dopadasz do $S sylwetki w kilku szaleñczych skokach, unosisz orê¿ i tniesz z ca³ych si³a znad g³owy. Przeciwnik b³yskawicznie uchodzi w bok unikaj±c rozpêdzonego ostrza, które wbija siê g³êboko w pod³o¿e. Z ca³ych si³ szarpiesz drzewce próbuj±c uwolniæ zaklinowan± broñ. Niematerialna sylwetka zbli¿a siê do ciebie b³yskawicznie z szaleñczym wyciem triumfu rozlegaj±cym siê w twym umy¶le. Zataczaj±c siê od og³uszaj±cego wrzasku w ostatniej chwili, resztk± si³ wyszarpujesz $h unikaj±c potê¿nych ciosów.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n w kilku szaleñczych skokach dopada $C z $j wysuniêt± przed siebie. Unosi swój orê¿ i z szybko¶ci± wichru opuszcza ostrze. Pó³materialny przeciwnik uchodzi b³yskawicznie w bok unikaj±c po¿êtego ciosu, a rozpêdzona broñ wbija siê z dono¶nym, metalicznym brzêkiem w pod³o¿e. $n szarpie z ca³ych si³ drzewce $f próbuj±c uwolniæ zaklinowany orê¿.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Wymachuj±c w¶ciekle $j ruszasz w stronê $Z, $E cofa siê straciwszy orientacjê pod naporem twojej furii. B³yszcz±ce ostrze ze ¶wistem opada na pó³materialn± szyjê przeciwnika i przechodzi przez ni± bezszelestnie. Odciêta g³owa unosi siê chwilê nad reszta astralnego cia³a i niknie po chwili w purpurowym rozb³ysku. Niematerialna sylwetka opada powoli na ziemiê wal±c siê na kolana. Z bezg³owego cia³a tryska gejzer czystej, duchowej energii o¶lepiaj±c ciê. Potrz±saj±c g³ow± mocniej ¶ciskasz drzewce $f i tniesz w korpus od góry. Przy zetkniêciu ostrza z cia³em uwalnia siê reszta purpurowej energii i przeciwnik niknie w piekielnych konwulsjach, w jaskrawej, bezg³o¶nej eksplozji.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n naciera z furi± na $C, $s $h zatacza ¶wietliste krêgi gdy zbli¿a siê do cofaj±cego siê przeciwnika. Szerokie ostrze w potê¿nym ciêciu opada na ods³oniêt±, pó³materialn± szyjê. Przechodzi przez ni± bezszelestnie odcinaj±c pó³przezroczyst± g³owê. Unosi siê ona przez mgnienie oka nad reszt± cia³a po czym niknie w jasnym, purpurowym rozb³ysku. Niematerialna sylwetka powoli opada na pod³o¿e osuwaj±c siê na kolana. Z bezg³owego cia³a tryska gejzer duchowej energii ra¿±c oczy. Po chwili widzisz jak $n poprawia chwyt swej broni i tnie od góry z ca³ych si³ nieruch± sylwetkê $Z. Astralne cia³o po zetkniêciu z rozpêdzonym ostrzem niknie, szarpi±c siê w¶ciekle w jaskrawej bezg³o¶nej eksplozji upiornego, purpurowego blasku.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Zbli¿asz siê szybkimi krokami do przeciwnika mocno ¶ciskaj±c drzewce swej broni. W ka¿dej chwili jeste¶ gotow<&y/a/e> do zadania decyduj±cego ciosu. Czekasz na odpowiedni moment. Lawiruj±c w powietrzu $N ¶widruje ciê na wskro¶ potêpieñczym, pe³nym bólu i nienawi¶ci wzrokiem. Wyci±ga pó³materialne ramiona w twoim kierunku próbuj±c zadaæ cios ods³aniaj±c siê przy tym. Korzystaj±c z okazji w mgnieniu oka unosisz $h, bierzesz szeroki zamach i tniesz z ca³ych si³ równocze¶nie schodz±c z linii ataku przeciwnika. Ostrze zag³êbia siê bezszelestnie w pó³przezroczystym ciele. W twej czaszce rozbrzmiewa pe³en rozpaczy i cierpienia, niezno¶ny wrzask.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n i $N ¶widruj± siê na wskro¶ pe³nym nienawi¶ci wzrokiem. Pó³materialna sylwetka z gracj± lawiruje tu¿ nad ziemi± okr±¿aj±c $c. Przeciwnicy obserwuj± siê nawzajem i wyczekuj± nawzajem odpowiedniej chwili by zaatakowaæ. $N nagle rzuca siê z wyci±gniêtymi ramionami na wroga. $n w mgnieniu oka unosi $h i tnie z ca³ych si³. ¦wiszcz±ce ostrze bezg³o¶nie zatapia siê w niematerialnym ciele, a o¶lepiaj±cy, czerwony blask wydobywaj±cy siê z rozerwanego korpusu razi twe oczy.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Odskakujesz krok w ty³ oddalaj±c siê nieznacznie od $Z. Bêd±c poza zasiêgiem ¶mierciono¶nych, niematerialnych ramion wykorzystujesz zasiêg swej broni i wyprowadzasz mordercze ciêcie. Ca³a sytuacja trwa zaledwie u³amek sekundy i pó³przezroczysty przeciwnik nie ma czasu, aby zej¶æ z toru ciosu. Szerokie ostrze zahacza o korpus wroga zostawiaj±c g³êbokie, w±skie naciêcie na klatce piersiowej. Z rany s±czy siê purpurowa po¶wiata tworz±c fantastyczne rozb³yski. Wyj±c z w¶ciek³o¶ci i bólu $N rzuca siê w twoim kierunku.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n odskakuje o krok od $Z zwiêkszaj±c dystans. Wykorzystuj±c zasiêg swej broni bierze szeroki zamach i wyprowadza mordercze ciêcie. Ca³a sytuacja trwa zaledwie kilka u³amków sekundy. Niematerialny przeciwnik lawiruje w powietrzu uchodz±c nieznacznie w bok, jednak szerokie ostrze zag³êbia siê w $S korpusie pozostawiaj±c po sobie g³êbokie, szerokie naciêcie. Z klatki piersiowej s±czy siê purpurowa po¶wiata, która niknie po chwili.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Starasz siê wykonaæ $j potê¿ny atak celuj±c w korpus $Z, ale $S unik jest szybszy. Odskakuje $E przed ciêciem i naciera na zza twoich pleców. Staraj±c siê nie oberwaæ, kontynuujesz walkê.{x", ch, wield, victim, TO_CHAR );
						act( "{5K±tem oka widzisz, jak $n wybiega naprzeciw $Z i stara siê wykonaæ potê¿ny atak. Wzniesiona wysoko, wypolerowana broñ b³yszczy siê o¶lepiaj±c± jasno¶ci±. Niestety, spadaj±c na wrog± muska zaledwie jego ramiê, a ten z kolei wymijaj±c zrêcznie jej ostrze zaczyna nacieraæ na $c od ty³u.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Znajduj±c bardzo wygodn± dla siebie pozycjê na wykonanie potê¿nej szar¿y bierzesz g³êboki oddech i wznosz±c wysoko $h rozpoczynasz natarcie. Stopniowo nabierasz na prêdko¶ci, a twe miê¶nie pracuj± coraz szybciej i wydajniej. Bêd±c u celu zaskakujesz $C zwinnym unikiem i wykonuj±c efektowny obrót dokonujesz makabrycznego ciêcia. W przeci±gu sekundy odciêta czaszka niezorientowanego w sytuacji $Z spada na ziemiê i z g³uchym szczêkiem toczy siê jeszcze parê metrów po posadzce, po czym podobnie jak reszta ju¿ bezu¿ytecznych ko¶ci zamiera w bezruchu.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze g³êboki oddech i rozpoczyna szaleñczy bieg. Unikaj±c zrêcznie ataków, wykonuje efektowne ciêcie $j z pó³obrotu. Pusta czaszka, odciêta od reszty cia³a spada swobodnie w dó³ i z g³uchym szczêkiem toczy siê jeszcze parê metrów po ziemi. Reszta ko¶æca z ³oskotem wali siê na posadzkê i rozsypuje siê w drobny mak.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Zaciskaj±c mocniej swe d³onie na trzonku $f rozpoczynasz szaleñczy bieg i zmuszasz swój organizm do wytworzenia dodatkowej dawki adrenaliny. Bêd±c u celu bierzesz potê¿ny zamach i wyprowadzasz atak skierowany na nogi $Z. S³ychaæ trzask nad³amywanych ko¶ci i niemi³osierne piszczenie orê¿a. Twój przeciwnik kuleje, a ty ponownie nacierasz.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz, jak $n zaciska najmocniej jak tylko mo¿e d³onie na trzonku $f, po czym rozpoczyna szaleñczy bieg. Bêd±c u celu bierze $e potê¿ny zamach i wyprowadza silny atak skierowany na nogi $Z. S³ychaæ trzask nad³amywanych ko¶ci i niemi³osierne piszczenie orê¿a.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Bez namys³u, z wielkim zapa³em i bojowym krzykiem rzucasz siê na $C. Pokonuj±c paroma wielkimi susami dziel±c± was odleg³o¶æ, skupiasz siê na celu jak najbardziej tylko mo¿esz, a w twych oczach zapala siê iskra ob³êdu. Bierzesz potê¿ny zamach i tniesz jak najsilniej tylko mo¿esz korpus $Z, $p wbija siê stosunkowo g³êboko pomiêdzy ko¶ci i wyszarpuje parê z nich, inne tn±c lub wybijaj±c ze stawów.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bez namys³u i z wielkim, bojowym zapa³em rzuca siê na $C. Pokonuj±c paroma susami dziel±c± ich odleg³o¶æ wznosi wysoko nad g³owê $h i jak tylko najsilniej mo¿e tnie korpus $Z. Jego $p wbija siê stosunkowo g³êboko pomiêdzy ko¶ci i wyszarpuje parê z nich, inne tn±c lub wybijaj±c ze stawów.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(victim->form, FORM_CONSTRUCT))
				{
					if ( dam <= 0 )
					{
						act( "{5Szar¿ujesz na $C z wyci±gniêt± przed siebie broni±, $E stoi nieruchomo wpatruj±c siê w ciebie bezmy¶lnie. Zbli¿asz siê do celu z ka¿dym krokiem. Nie zatrzymuj±c biegu wykonujesz zabójcze pchniêcie w klatkê piersiow± nieruchomego wroga, jednak ten unosi w górê mocarne ramiê odbijaj±c drzewce $f. Ze zduszonym jêkiem, próbuj±c wyhamowaæ szaleñczy pêd zderzasz siê ze sztuczn± sylwetk±. Odbijasz siê od przeciwnika i oszo³omion<&y/a/e> zataczasz parê metrów w ty³.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n pêdzi szaleñczo na stoj±cego nieruchomo przeciwnika z wyci±gniêtym przed siebie orê¿em. Nie zwalniaj±c biegu wyprowadza mordercze pchniêcie skierowane w klatkê piersiow± $Z, $E jednak unosi w górê mocarne ramiê i z zastraszaj±c± si³a odbija drzewce $f. $n w panice próbuje os³abiæ impet biegu, jednak na pró¿no, zderza siê potê¿nie ze sztuczn± sylwetk± $Z i odbija parê metrów w ty³.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Wykorzystuj±c zasiêg $f wyprowadzasz potê¿ne ciêcie skierowane w poprzek korpusu przeciwnika. Twoje ramiona napinaj± siê jak postronki kiedy zaciskasz je z potworn± si³± na d³ugim, wy¶lizganym drzewcu. Ostrze zatacza szeroki, morderczy ³uk b³yszcz±c w¶ciekle i opada z ³oskotem tu¿ powy¿ej linii bioder $Z. Orê¿ wnika g³êboko w sztuczny korpus z dono¶nym trzaskiem. Kaskady iskier i od³amków strzelaj± gêstymi snopami na wiele metrów wokó³ zasypuj±c wszystko, a $p tnie kad³ub z nieprzyjemnym zgrzytem i przechodzi na wylot. Odskakujesz na bezpieczn± odleg³o¶æ i obserwujesz jak $N l±duje na ziemi z ³oskotem i rozsypuje siê na kilkadziesi±t czê¶ci.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wykorzytsuj±c zasiêg $f wyprowadza potê¿ne ciêcie na ukos, przez korpus przeciwnika. Ramiona $s napinaj± siê jak postronki kiedy ¶ciska z ca³ych si³ wy¶lizgane drzewce. Rozpêdzone ostrze tn±c powietrze ze ¶wistem z potworn± si³± zag³êbia siê w sztuczne cia³o tu¿ nad lini± bioder $Z. Kaskady iskier i od³amków strzelaj± grubymi snopami na wiele metrów wokó³. Orê¿ ze zgrzytem i metalicznym brzêkiem przechodzi na wylot. $n odskakuje na bezpieczn± odleg³o¶æ i z satysfakcj± obserwuje swoje dzie³o zniszczenia. Kad³ub przeciwnika machaj±c niezdarnie ramionami zsuwa siê powoli wzd³u¿ linii ciêcia, opada z ³oskotem na pod³o¿e i rozpada na kilkadziesi±t kawa³ków. Po chwili, ko³ysz±c siê przez u³amek sekundy, dolne koñczyny $Z dziel± ten sam los.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Krêc±c m³yñca $j nad g³ow± w kilku susach dobiegasz do przeciwnika. Wykonuj±c nag³y skrêt cia³a mijasz go, zapierasz nogami o pod³o¿e i stajesz przodem do jego pleców. Wnosisz sw± broñ wysoko nad g³owê i wyprowadzasz szaleñcze ciêcie prosto miêdzy ³opatki $Z. Uderzeniu towarzyszy og³uszaj±cy, zgrzytliwy ³oskot. Snopy iskier i od³amków zasypuj± ci twarz i o¶lepiaj± na chwilê. Odzyskuj±c wzrok wyszarpujesz ostrze broni, które tkwi³o g³êboko w korpusie wroga.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n szaleñczo wymachuj±c $j nad g³ow± w d³ugich susach zbli¿a siê do $Z. Wykonuj±c nag³y skrêt cia³a mija przeciwnika i staje przodem do jego pleców. Wznosi swój orê¿ w potê¿nym ciêciu. Ostrze b³yszcz±c w¶ciekle zatacza szeroki ³uk i ze ¶wistem opada prosto miêdzy ³opatki, a eksplozja iskier i od³amków zasypuje sylwetkê $z. Kiedy opadaj±, napastnik wyszarpuje orê¿ z ha³a¶liwym zgrzytem.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Wyczekujesz odpowiedniego momentu do zadania ciosu. $N stawiaj±c kolejny ociê¿a³y krok ods³ania siê na chwilê. Nie czekaj±c d³u¿ej b³yskawicznie wyprowadzasz potê¿ne pchniêcie w bok przeciwnika. Wyrzucone ostrze tn±c ze ¶wistem powietrze pêdzi ku przeznaczeniu z ogromn± prêdko¶ci±, jednak przeciwnik w porê zauwa¿a zagro¿enie i zas³ania siê ramieniem. Orê¿ zag³êbia siê w nim z ha³a¶liwym zgrzytem przechodz±c na wylot. Wyszarpuj±c broñ tworzysz spor± wyrwê w przedramieniu wroga.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wyczekuje odpowiedniego momentu do zadania ciosu. Kiedy $N stawia kolejny niezdarny krok, barbarzyñca z b³yskiem w oku wyprowadza potê¿ne pchniêcie w bok wroga. Ostrze tn±c powietrze z zastraszaj±c± si³± wêdruje ku przeznaczeniu, jednak $N w porê unosi szerokie ramiê w obronnym ge¶cie i zas³ania siê. Orê¿ wbija siê w nie ze zgrzytem i przechodzi na wylot. $n wyszarpuj±c broñ tworzy spor± wyrwê w przedramieniu przeciwnika.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if ( dam <= 0 )
				{
					act( "{5Szar¿ujesz na $C jednak nie udaje ci siê wyprowadziæ silnego ciosu, drzewce broni wy¶lizguje ci siê lekko i z trudem zamachujesz siê i wykonujesz ciêcie.{x", ch, wield, victim, TO_CHAR );
					act( "{5$n szar¿uje na ciebie, jednak nie udaje $O siê wyprowadziæ silnego ciosu, z trudem zamachuje siê i wykonuje ciêcie.{x", ch, wield, victim, TO_VICT );
					act( "{5Widzisz jak $n szar¿uje na $C, jednak nie udaje $o siê wyprowadziæ silnego ciosu, z trudem zamachuje siê i wykonuje ciêcie.{x", ch, wield, victim, TO_NOTVICT );
					return TRUE;
				}
				else if ( victim->hit - dam < -11 )
				{
					switch ( number_range( 0, 3 ) )
					{
						case 0:
							act( "{5Patrzysz na swojego wroga i wiesz, ¿e to ju¿ jego koniec. Nabierasz rozpêdu wykonuj±c kilka pe³nych obrotów. Twoja broñ zaczyna obracaæ siê razem z tob±, a gdy zbli¿asz siê do $Z bez trudu odcinasz $S g³owê nie trac±c nawet impetu. Po chwili uspokajasz siê.{x", ch, wield, victim, TO_CHAR );
							act( "{5Dostrzegasz na sobie parali¿uj±ce spojrzenie $z, któr$r nabiera rozpêdu wykonuj±c kilka obrotów. Czas zwalnia kiedy ostrze przecina najpierw skórê, miê¶nie i gard³o a ca³e twoje ¿ycie przelatuje ci przed oczyma. Ostatni± rzecz jak± dostrzegasz to zbli¿aj±ca siê ziemiê.{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n nabiera rozpêdu wykonuj±c kilka pe³nych obrotów. Gdy zbli¿a siê do $Z bez trudu odcina $O g³owê nie trac±c nawet impetu.{x", ch, wield, victim, TO_NOTVICT );
							if ( IS_NPC( victim ) && victim->pIndexData->corpse_vnum < 0 )
								act( "{5Odciêta g³owa $Z po chwili znika.{x", ch, wield, victim, TO_ROOM );
							else
								make_head( victim );
							break;

						case 1:
							if ((IS_SET(victim->parts, PART_ARMS) && IS_SET( victim->form, FORM_WARM )) ||
									(IS_SET(victim->parts, PART_ARMS) && IS_SET( victim->form, FORM_COLD_BLOOD )))
							{
								act( "{5Z niesamowit± zwinno¶ci± podskakujesz do $Z, wykonujesz pe³ny zamach i trafiasz prosto w $S ramiê, ma³o brakowa³o a by³aby to szyja. Jednak¿e i taki cios jest skuteczny, ostrze gruchocze ko¶ci ramienia $Z, przecina p³uca, ¿o³±dek i w±trobê, a zatrzymuje siê dopiero na miednicy. Odr±bana rêka $Z upada na ziemiê, a z kikutu ramienia sika krew jak z wodospadu.{x", ch, wield, victim, TO_CHAR );
								act( "{5$n w ¶miertelnym tañcu podskakuje do ciebie i wykonuj±c pe³ny zamach trafia prosto w twe ramiê, ma³o brakowa³o a by³aby to szyja. Jednak i taki cios wystarcza, by¶ straci³ wszystko, co posiadasz. Robi ci siê ciemno przed oczami, czujesz tylko dziwn± lekko¶æ po prawej stronie cia³a. Starasz siê zaczerpn±æ powietrze, ale jaki¶ p³yn chyba wype³ni³ twe p³uca, gdy¿ tylko siê krztusisz. Potem ju¿ nic nie czujesz.{x", ch, wield, victim, TO_VICT );
								act( "{5Widzisz jak $n w szalonym tañcu podskakuje do $Z i wykonuj±c pe³ny zamach trafia prosto w $S ramiê, ma³o brakowa³o a by³aby to szyja. Jednak¿e i taki cios jest skuteczny, ostrze gruchocze ko¶ci ramienia $Z, przecina p³uca, ¿o³±dek i w±trobê, a zatrzymuje siê dopiero na miednicy. Odr±bana rêka $Z upada na ziemiê, a z kikutu ramienia sika krew jak z wodospadu..{x", ch, wield, victim, TO_NOTVICT );
								make_arm(victim);
								break;
							}
						case 2:
							if ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
							{
								act( "{5Z niesamowit± zwinno¶ci± podskakujesz do $Z, wykonujesz pe³ny zamach rozcinaj±c jedn± z têtnic. $N upada i w kilka chwil wykrwawia siê na ¶mieræ.{x", ch, wield, victim, TO_CHAR );
								act( "{5$n w ¶miertelnym tañcu podskakuje do ciebie i wykonuj±c pe³ny zamach rozcina ci jedn± z têtnic. Upadasz czuj±c przera¼liwy ból i up³ywaj±c± krew z uda. Chwile wyd³u¿aj± siê po czym robi siê ciemno i zimno.{x", ch, wield, victim, TO_VICT );
								act( "{5Widzisz jak $n w szalonym tañcu podskakuje do $Z i wykonuj±c pe³ny zamach rozcina jedn± z têtnic. $N upada i w kilka chwil wykrwawia siê na ¶mieræ.{x", ch, wield, victim, TO_NOTVICT );
								make_blood(victim);
								break;
							}
						case 3:
							if ((IS_SET(victim->parts, PART_GUTS)))
							{
								act( "{5Rozpêdzasz siê i wykonujesz wspania³e ciêcie! Brzuch $Z rozrywa siê okazale prezentuj±c ostatni posi³ek nieboszczyka, flaki i inne bli¿ej niezidentyfikowane obiekty.{x", ch, wield, victim, TO_CHAR );
								act( "{5Widzisz jak $n rozpêdza siê i wykonuje, na twoje nieszczê¶cie, doskona³e ciêcie! Czujesz siê bardzo lekko, powoli odp³ywa od ciebie ¶wiadomo¶æ i ¿ycie...{x", ch, wield, victim, TO_VICT );
								act( "{5Widzisz jak $n rozpêdza siê i wykonuje wspania³e ciêcie! Brzuch $Z rozrywa siê okazale prezentuj±c ostatni posi³ek nieboszczyka, flaki i inne bli¿ej niezidentyfikowane obiekty.{x", ch, wield, victim, TO_NOTVICT );
								break;
							}
						default:
								act( "{5Rozpêdzasz siê i wykonujesz wspania³e ciêcie!{x", ch, wield, victim, TO_CHAR );
								act( "{5Widzisz jak $n rozpêdza siê i wykonuje, na twoje nieszczê¶cie, doskona³e ciêcie! Czujesz, ¿e powoli odp³ywa od ciebie ¶wiadomo¶æ i ¿ycie...{x", ch, wield, victim, TO_VICT );
								act( "{5Widzisz jak $n rozpêdza siê i wykonuje wspania³e ciêcie!{x", ch, wield, victim, TO_NOTVICT );
								break;
					}
					return TRUE;
				}
				else
				{
					int types = 1;

					if ( ch->class == CLASS_BARBARIAN && is_affected(ch, gsn_berserk ) )
						types = 2;

					switch ( number_range( 0, types ) )
					{
						case 0:
							act( "{5Szar¿ujesz na $C mocno trzymaj±c drzewce $f, w biegu wykonujesz pe³en obrót i twój cios trafia z potê¿nym impetem rozrywaj±c $C!{x", ch, wield, victim, TO_CHAR );
							act( "{5$n szar¿uje na ciebie, w biegu wykonuje pe³en obrót i jego cios trafia ciê z potê¿nym impetem! Czujesz przera¼liwy ból gdy $p rozrywa twoje cia³o.{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n szar¿uje na $C wykonuj±c przy tym pe³en obrót. $s cios trafia z potê¿nym impetem rozrywaj±c $C.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 1:
							act( "{5Szar¿ujesz na $C utrzymuj±c $p nad g³ow±, zatrzymujesz siê nagle tu¿ przed swoim przeciwnikiem wykonuj±c potê¿ny zamach! Twój cios dewastuje $C trafiaj±c centralnie w klatkê piersiow±.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n szar¿uje na ciebie trzymaj±æ $p wysoko nad g³ow±, zamachuje siê potê¿nie trafiaj±c ciê w klatkê piersiow±! Prawie mdlejesz gdy potworny ból przechodzi przez twoje cia³o.{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n szar¿uje na $C trzymaj±c $p wysoko nad g³ow±, zatrzymuje siê nagle przed swoim przeciwnikiem wykonuj±c potê¿ny zamach! $s cios trafia i dewastuje $C trafiaj±c w klatkê piersiow±.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 2:
							act( "{5W kilku dziwacznych podskokach zbli¿asz siê do $Z, wykonujesz krótki obrót ca³ym cia³em i precyzyjnie trafiasz $j rozcinaj±c tkanki.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n w kilku dziwacznych podskokach zbli¿a siê do ciebie, wykonuje krótki obrót ca³ym cia³em i precyzyjnie trafia ciê rozcinaj±c tkanki!{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n zbli¿a siê w kilku dziwacznych podskokach do $Z, wykonuje krótki obrót ca³ym cia³em i precyzyjnie trafia $j rozcinaj±c tkanki.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				break;

			case WEAPON_SPEAR:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wiêksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Spogl±dasz na miotaj±c± siê sylwetkê przeciwnika zimno kalkuluj±c sytuacjê. Wyczekujesz odpowiedniego momentu, aby zadaæ mordercze pchniêcie $j. Kiedy tylko nadarza siê okazja d¼gasz z ca³ych si³ kieruj±c cios w ods³oniêty brzuch przeciwnika. Ten wykonuje b³yskawiczny skrêt cia³a i twój grot mija swój cel tn±c ze ¶wistem powietrze poci±gaj±c ciê za sob±. $N bierze potê¿ny zamach i wyrzuca w twym kierunku z potworn± si³± swe przegni³e ramiê w mia¿d¿±cym ciosie. Widzisz to jedynie k±tem oka i lekko uchylasz g³owê, pêdz±c wci±¿ naprzód.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n w¶ciekle d¼ga $C wykorzystuj±c lukê w $S obronie. Jednak przeciwnik wykonuje b³yskawiczny unika i odpowiada morderczym ciosem. Tak gwa³towne ruchy powoduj±, ¿e gubi przy tym p³aty cia³a, przez które prze¶wituj± po¿ó³k³e ko¶ci. Impet niecelnego d¼gniêcia $j powoduje, ¿e nacieraj±cy niezgrabnie sunie naprzód zrównuj±c siê z przeciwnikiem.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Warcz±c w¶ciekle bierzesz potê¿ny zamach i wykonujesz od do³u potê¿ne pchniêcie w brzuch przeciwnika. Grot wchodzi g³adko i wychodzi przez ramiê $Z wbijaj±c siê dodatkowo w przekrzywion± g³owê. Potworne konwulsje wstrz±saj± cia³em o¿ywieñca. Z $S ust wydobywa siê zgrzytliwy, dono¶ny, przed¶miertny skrzek. Porusza w¶ciekle koñczynami w ostatnich drgawkach. Zapierasz siê z ca³ych si³, unosisz $h w powietrze poci±gaj±c tym samym cuchn±ce ¶cierwo przeciwnika. Nag³ym, silnym ruchem t³uczesz nim o ziemiê. Uderzenie jest tak mocne, ¿e przy zetkniêciu cia³a z pod³o¿em s³yszysz d¼wiêk pêkaj±cych ko¶ci i wnêtrzno¶ci oraz chrzêst rozrywanych miê¶ni i ¶ciêgien. Zalewa ciê fontanna plugawych trupich soków. Twoje wycie triumfu rozlega siê po okolicy.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n przykuca i wykonuje ³ukowe d¼gniêcie skierowane z do³u, ku górze. Cios dosiêga brzucha $Z i przechodzi przez ca³y korpus koñcz±c sw± mordercz± wêdrówkê w gnij±cej czaszce. Nieumar³y szarpie siê w ostatnich podrygach. Z jego wykrzywionych bólem i w¶ciek³o¶ci± warg wydobywa siê zgrzytliwy skrzek rani±cy uszy. $n wytê¿a swe ramiona i unosi $h w powietrze wraz ze ¶cierwem ofiary. Nagle opuszcza j± z ca³ych si³ i t³ucze o pod³o¿e. Impet walniêcia jest tak silny, ¿e dos³ownie mia¿d¿y martwe cia³o. Fontanna popêkanych wnêtrzno¶ci, ko¶ci oraz cuchn±cych p³ynów zalewa dos³ownie wszystko. Z gard³a $z wydobywa siê potworne wycie triumfu.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Warcz±c w¶ciekle patrzysz w przegni³± twarz $Z. Zapierasz siê mocno nogami w pod³o¿e i wykonujesz b³yskawiczne pchniêcie prosto w klatkê piersiow± przeciwnika. Grot $f wbija siê g³êboko z mokrym chrzêstem w toczone zgnilizn± cia³o wroga. Szarpiesz broni± na lewo i prawo dewastuj±c rozmiêk³e wnêtrzno¶ci o¿ywieñca. Twym morderczym zabiegom towarzyszy przera¼liwe wycie. $N miota siê w swym szaleñczym cierpieniu chwytaj±c rozszarpuj±ce mu cia³o drzewce. Próbuje zatrzymaæ w¶ciekle tañcz±cy w jego wnêtrzno¶ciach grot. Zostawiaj±c wrogowi w piersiach ziej±c± jamê, z której bucha strumieñ trupiego jadu wyszarpujesz $h.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n staje w lekkim wykroku i z mordercz± si³± oraz precyzj± wykonuje pchniecie $j. Cios trafia w klatkê piersiow± $Z. Grot broni wbija siê z chrzêstem w plugawe cia³o mieszaj±c siê ze skrzekliwym wyciem nieumar³ego. $n szarpie $j na wszystkie strony miotaj±c martwym cia³em przeciwnika. O¿ywieniec chwyta drzewce broni chc±c zatrzymaæ jej mordercz± wêdrówkê, na pró¿no jednak. W jego korpusie zionie olbrzymia dziura z wystaj±cymi, po³amanymi ko¶æmi oraz zwisaj±cymi, popêkanymi wnêtrzno¶ciami. Atakuj±cy z mokrym mla¶niêciem wyszarpuje sw± bron i szykuje siê do kolejnego natarcia.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Pêdzisz szaleñczo na $C opieraj±c koniec drzewca $f przy prawym biodrze. Spowalnia to nieco bieg, lecz masz przez to lepszy punkt oparcia i mo¿esz manewrowaæ sw± broni±. Wbijasz grot g³êboko w cuchn±ce zgnilizn± cia³o przeciwnika i przeci±gasz go silnym szarpniêciem w górê rozrywaj±c przegni³y korpus o¿ywieñca. Z olbrzymiego rozciêcia wygl±daj± poszarpane wnêtrzno¶ci oraz chlupie cuchn±ca ciecz. Skrzekliwym warkotem $N daje znaæ, ¿e odczu³ ten cios. U¶miechaj±c siê dziko chwytasz lu¼no sw± broñ.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz szaleñczy bieg $z szar¿uj±cego na $C. Kiedy z impetem wbija $h w przegni³e cia³o przeciwnika na jego twarzy pojawia siê grymas bólu. Szeroki grot broni zag³êbia siê w brzuchu nieumar³ego. Atakuj±cy szarpie $j kieruj±c j± ku górze co powoduje rozleg³e obra¿enia na korpusie o¿ywieñca. Ze szramy $Z tryska pó³przezroczysta ciecz oraz strzelaj± poszarpane wnêtrzno¶ci zwisaj±c lu¼no. Z gard³a $z wydobywa siê szaleñczy ¶miech.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cieñ" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiór" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Wyskakujesz do góry, celuj±c $j w $C, jednak pech sprawi³ i¿ twój cel uchyli³ siê i twa broñ wbi³a siê w ziemie tu¿ obok niematerialnego cia³a $Z.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n wyskakuje do góry i celuje sw± broni± w $C, jednak cel uchyli³ siê i $f $z wbija siê w ziemiê, tu¿ obok niematerialnego cia³a.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Bierzesz potê¿ny rozbieg i z w¶ciek³o¶ci± rzucasz siê na $C. Twa broñ zatacza krêgi wzniesiona wysoko i b³yszczy upiornie gdy twój rze¼nicki cios spada na próbuj±cego siê os³oniæ przeciwnika. Grot $f g³adko wchodzi w eteryczne cia³o tn±c je i rozrywaj±c, jego kawa³ki odpadaj± i znikaj± po chwili. Suniesz g³adko, od g³owy, przez rêce po sam dó³. Wstajesz z klêczków i z satysfakcj± patrzysz jak dwie czêsci $Z rozp³ywaj± siê powoli a potêpiencze jêki gasn± w niebycie.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze potê¿ny rozbieg i z w¶ciek³ym rykiem rzuca siê na $C. Grot $f zatacza krêgi, wznie¶ione wysoko upiornie b³yszczy gdy rzê¼nicki cios spada na próbuj±cego siê os³oniæ ducha. Broñ $z g³adko wchodzi w eteryczne cia³o, tn±c je, rozrywaj±c. Odpadaj±ce kawa³ki znikaj± po chwili, roztapiaj± siê od³±czone od swego niematerialnego cia³a. Cios przeszed³ od góry po sam dó³. Po chwili $n wstaje i z satysfakcj± patrzy jak powoli rozp³ywaj± siê dwie czêsci ducha a potêpiencze jêki cichn± coraz bardziej.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Bierzesz krótki rozbieg i odbijasz siê od ziemi. Twa broñ z olbrzymi± prêdko¶ci± trafia w pó³materialne cia³o $Z i przeszywaj±c je na wylot jak nó¼ wbija siê w mas³o. $N wydaje z siebie potêpieczy jêk, jakby siê do czego¶ szykowa³, jednak szybko wyci±gasz $h z $S powoduj±c wielkie zniszczenia.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze krótki rozbieg i odbija siê od ziemi. Jego broñ bezb³êdnie trafia w niematerialne cia³o $Z przeszywaj±c je na wylot i czyni±c wielkie zniszczenia. $N wydaje z siebie potêpienczy jêk i probowa³ uderzyæ $c, jednak niezd±¿a siê nawet powa¿nie zamachn±æ kiedy przeciwnik wyci±ga z sw± broñ rani±c $C jeszcze powa¿niej.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Z olbrzymi± prêdkosci± i dzikim rykiem rzucasz siê na $C, wymachuj±c $j. Przeciwnik próbowa³ uchyliæ siê, lecz twa broñ przyszpili³± go do ziemi z ogromn± si³±. Potêpienczy jêk, jaki wyda³ duch sprawi³, ¿e a¿ zabola³y ciê uszy. Jednak z g³o¶nym warkotem wyszarpno³e¶ broñ z niematerialnego, szamocz±cego siê bez³adnie cia³a. $N rzuca siê do walki z wsciek³o¶ci±.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z olbrzymi± prêdkosci± i dzikim rykiem rzuca siê na $C, wymachuj±c dooko³a $j. Unik, jaki próbowa³ zrobiæ cel, nie uda³ siê i broñ $z przyszpili³a go do ziemi z wielk± si³±. $N wrzasn±³ potêpiêñczo, po czym $n sprawnym ruchem wyszarpn±³ broñ z szamocz±cego siê, niematerialnego cia³a, które jednak podnios³o siê i rzuci³o do walki z wsciek³osci±. Wszystko sta³o siê w jednej chwili.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
				{
					if ( dam <= 0 )
					{
						act( "{5$N z b³yszcz±c± w pustych oczodo³ach furi± naciera na ciebie. B³yskawicznie blokujesz atak drzewcem $f i wyprowadzasz kontrê z potworn± si³±. Grot z szybko¶ci± atakuj±cego wê¿a wêdruje w kierunku przeciwnika b³yszcz±c w¶ciekle. Natychmiastowy kontratak jest jednak nieprecyzyjny, ma³a ilo¶æ czasu, aby dok³adnie wycelowaæ powoduje, ¿e broñ muska zmursza³e, po¿ó³k³e ramiê i ze¶lizguje siê z metalicznym brzêkiem nie czyni±c szkody przeciwnikowi.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $N z furi± naciera na $c wyprowadzaj±c potê¿ne ciosy. Barbarzyñca zas³ania siê drzewcem $f skutecznie blokuj±c b³yskawiczne ataki i w mgnieniu oka wyprowadza kontrê. Grot $s broni z prêdko¶ci± pioruna szybuje tn±c powietrze w kierunku przeciwnika. Jednak b³yskawiczny atak nie jest zbyt dok³adnie wymierzony i $p muska zmursza³e, po¿ó³k³e ramiê szkieletu i ze¶lizguje siê po nim nie wyrz±dzaj±c ¿adnej szkody.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Biegniesz rozpêdzon<&y/a/e> w kierunku przeciwnika zataczaj±c szerokie krêgi $j nad g³ow±. W kilku susach dopadasz go i wykonuj±c szeroki ³uk podcinasz drzewcem swej broni zmursza³e dolne koñczyny wroga. $N chwieje siê przez chwilê próbuj±c odzyskaæ równowagê, po czym wali siê z ³oskotem na pod³o¿e. Skaczesz mu na klatkê piersiow± z impetem, mia¿d¿±c przy tym z paskudnym trzaskiem wszystkie ¿ebra przeciwnika. Unosisz wysoko $h po czym opuszczasz z potworna si³±. Szeroki grot opada ze ¶wistem prosto na czaszkê przebijaj±c j± na wylot. $N drga jeszcze przez chwilê, po czym nieruchomieje.{x", ch, wield, victim, TO_CHAR );
						act( "{5Podziwiasz jak $n z rozpêdem dopada przeciwnika zataczaj±c szerokie krêgi nad g³ow± $j. B³yskawicznym manewrem $n podcina d³ugim drzewcem broni zmursza³e koñczyny $Z, a ten chwieje siê przez chwilê próbuj±c odzyskaæ równowagê, po czym wali siê z ³oskotem na pod³o¿e. $n nie czeka ani sekundy. B³yskawicznie skacze na klatkê piersiow± przeciwnika mia¿d¿±c mu przy tym wszystkie ¿ebra z paskudnym trzaskiem. Unosi wysoko $h i opuszcza j± ze ¶wistem na po¿ó³k³± czaszkê. Szeroki grot z potworn± si³± przebija j± na wylot. $N w ostatnim podrygu próbuje siê uwolniæ, po czym nieruchomieje.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Biegniesz szaleñczo ¶ciskaj±c mocno drzewce swej broni z b³yszcz±cym grotem skierowanym w przeciwnika. $N z nienawi¶ci± wpatruje siê w ciebie próbuj±c zej¶æ z linii potê¿nej szar¿y, jednak bez wiêkszego szczê¶cia. Sprytnie manewrujesz $f, skutecznie udaremniaj±c przeciwnikowi jak±kolwiek szansê obrony. Szeroki grot z suchym trzaskiem zag³êbia siê w ¶rodek czaszki wroga, tu¿ nad lini± oczodo³ów z potworn± si³±. Prawie oderwana od krêgos³upa, popêkana ko¶ciana g³owa odchyla siê w ty³ k³api±c szczêk± z bólu. Potê¿nym szarpniêciem, z cichym zgrzytem uwalniasz grot.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl±dasz jak $n w szaleñczym biegu kieruje $h w stronê przeciwnika. $N próbuj±c zej¶æ z toru ataku przemieszcza siê niezdarnie to w lewo, to w prawo. Szar¿uj±cy przemy¶la³ dobrze swój atak, tak manewruje broni± oraz cia³em, ¿e skutecznie udaremnia wrogowi jak±kolwiek szansê obrony. Szeroki, w¶ciekle b³yszcz±cy grot z suchym trzaskiem zag³êbia siê w samym ¶rodku ko¶ci czo³owej przeciwnika, tu¿ nad lini± oczodo³ów. Prawie oderwana od krêgos³upa, ko¶ciana g³owa k³apie szczêk± w niemym cierpieniu.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Wymierzasz precyzyjne pchniêcie w krêgos³up nacieraj±cego na ciebie przeciwnika. Oceniasz odleg³o¶æ, wykonujesz lekki wykrok i bierzesz potê¿ny zamach $j. Ju¿ jeste¶ gotow<&y/a/e> do zadania ¶miertelnego ciosu, gdy $n nieoczekiwanie doskakuje do ciebie zbli¿aj±c siê na niebezpiecznie ma³± odleg³o¶æ. Wypuszczaj±c z sykiem powietrze, nie maj±c czasu na nic wiêcej, wyprowadzasz pchniêcie na o¶lep w kierunku wroga. Szeroki grot z g³uchym trzaskiem zag³êbia siê w miednicê przechodz±c na wylot. Odskakuj±c od $Z wyszarpujesz broñ i obserwujesz wybity, szeroki otwór w kostnym ciele.{x", ch, wield, victim, TO_CHAR );
						act( "{5Obserwujesz jak $n mierzy odleg³o¶æ miêdzy sob± a przeciwnikiem, nastêpnie wykonuje lekki wykrok i bierze szeroki zamach zamierzaj±c zadaæ ¶miertelne pchniêcie. Jednak $s plany spe³zaj± na niczym, gdy¿ $N w nag³ym zrywie doskakuje swego wroga. $n na o¶lep wyrzuca przed siebie $h, grot z g³uchym trzaskiem zag³êbia siê w ods³oniêtej miednicy przeciwnika przechodz±c na wylot.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(victim->form, FORM_CONSTRUCT))
				{
					if ( dam <= 0 )
					{
						act( "{5Wyprowadzasz potê¿ny cios kieruj±c swój orê¿ z do³u ku górze szerokim ³ukiem. Grot zataczaj±c ¶wietlisty pó³okr±g szybuje nad ziemi± i wznosi siê z zastraszaj±c± si³± i prêdko¶ci±. Sprawnym szarpniêciem kierujesz $h w tors przeciwnika. Zaskoczony $N stoi nieruchomo niezdolny do wykonania jakiegokolwiek ruchu. Broñ b³yskawicznie dociera do celu i trze z dono¶nym zgrzytem o twardy korpus, jednak grot wzbija tylko kaskady rozgrzanych od³amków i ze¶lizguje siê z tu³owia wroga, po czym szybuje w górê nie zostawiaj±c najmniejszej nawet rysy na twardym ciele.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n kieruj±c swój orê¿ z do³u ku górze wyprowadza mordercze pchniêcie w tors $Z. ¦wietlisty grot szybuj±c nad ziemi± zatacza szeroki pó³okr±g i wznosi siê w powietrze z zastraszaj±c± prêdko¶ci±. $n sprawnie manewruj±c orê¿em prowadzi go b³yskawicznie ku przeznaczeniu, $p dociera do celu i zgrzyta o twardy korpus znieruchomia³ego przeciwnika wywo³uj±c kaskady iskier i od³amków sztucznego cia³a, jednak grot ze¶lizguje siê z klatki piersiowej i wêdruje w powietrze nie pozostawiaj±c najmniejszej rysy na ciele $Z.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Z okrzykiem bojowym na ustach wyprowadzasz mordercze pchniêcie z góry do do³u. Rozpêdzony grot trafia $Z w brzuch tu¿ nad kroczem z dono¶nym zgrzytem. Fontanny iskier i od³amków sztucznego cia³a strzelaj± z twardego korpusu i szybuj± parê metrów w powietrze gdy orê¿ wbija siê g³êboko w twarde cia³o przechodz±c na wylot. Wokó³ wyrwy pojawiaj± siê liczne rysy i spêkania. Widocznie trafi³<&e¶/a¶/o¶> w najs³absz± czê¶æ konstrukcji, bo pêkniêcia z ka¿d± sekund± rozszerzaj± siê i rozprzestrzeniaj± praktycznie na ca³e nienaturalne cia³o. W kilka sekund twardy korpus rozpada siê na kilkadziesi±t kawa³ków. Wszystkie jego czê¶ci w jednej sekundzie wal± siê z dono¶nym ³oskotem na pod³o¿e. Ko³ysz±cy siê na ziemi u³amek twarzy $Z patrzy na ciebie lewym okiem, jakby z wyrzutem.{x", ch, wield, victim, TO_CHAR );
						act( "{5Przera¿aj±cy okrzyk bojowy wzbija siê dono¶nym g³osem w powietrze i odbija echem. $n wyprowadza mordercze pchniêcie $j kieruj±c swój orê¿ z nad g³owy w dó³. ¦wiszcz±cy grot wbija siê g³êboko tu¿ nad kroczem w brzuch $Z, z dono¶nym zgrzytem wywo³uj±c kaskady iskier i od³amków sztucznego materia³u, które wzbijaj± siê wysoko i szybuj± parê metrów. Wokó³ powsta³ej wyrwy w twardym korpusie tworz± siê minimalne pêkniêcia i rysy. Prawdopodobnie to najbardziej czu³e miejsce konstrukcji gdy¿ spêkania rozszerzaj± siê z ka¿d± sekund± i rozprzestrzeniaj± na ca³e nienaturalne cia³o z cichym odg³osem. W kilka chwil sylwetka $Z rozpada siê, a jej czê¶ci w jednej chwili wal± siê z ³oskotem na pod³o¿e. Od³amana lewa strona twarzy $Z patrzy na $z, jakby z wyrzutem, ko³ysz±c siê miarowo.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Z wyci±gniêt± przed siebie broni± nacierasz z ogromnym impetem na przeciwnika. Ko³ysz±cy siê w rytm twego biegu grot $f tnie powietrze z nieprawdopodobn± si³±. $N beznamiêtnie wpatruj±c siê w ciebie wyci±ga ramiona w obronnym ge¶cie, znajdujesz jednak lukê w zas³onie przeciwnika i orê¿ ze ¶wistem dociera do celu. Szeroki grot z g³uchym odg³osem zag³êbia siê w sztucznym korpusie wywo³uj±c kaskady  iskier i od³amków.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n naciera z potwornym impetem na $C, $s wyci±gniêty orê¿ tnie powietrze z przera¼liwym ¶wistem ko³ysz±c siê w rytm kroków. $N wyci±gaj±c ramiona w obronnym ge¶cie beznamiêtnie wpatruje siê w szar¿uj±c± sylwetkê, która jednak sprytnie manewruje sw± broni± i szeroki grot z paskudnym, g³uchym jêkiem wbija siê w korpus przeciwnika wywo³uj±c kaskady iskier i od³amków.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Zatrzymujesz siê nagle w miejscu i wyprowadzasz b³yskawiczne pchniêcie w brzuch przeciwnika. Wyrzucony do przodu orê¿ mknie w prêdko¶ci± wichru w kierunku sztucznego korpusu, jednak $N wykonuj±c nag³y zryw cia³a schodzi czê¶ciowo z linii ciosu. Szeroki grot zachacza o bok celu i przebija siê przez niego. Odzyskuj±c równowagê stwierdzasz z satysfakcj±, ¿e twój atak nie poszed³ na marne. W boku przeciwnika widnieje g³êboka wyrwa.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n zatrzymuje siê w miejscu i b³yskawicznym ruchem wyrzuca swój orê¿ przed siebie wyprowadzaj±c potê¿ne pchniêcie w brzuch przeciwnika. Broñ ¶wiszczy w powietrzu z nieprawdopodobn± intensywno¶ci± zbli¿aj±c siê do sylwetki $Z. Z niespotykanym u takich istot refleksem, wykonuje $E nag³y zryw cia³a schodz±c czê¶ciowo z linii morderczego ciosu. Szeroki grot zgrzyta o bok przeciwnika wywo³uj±c kaskady iskier i od³amków po czym ze¶lizguje siê i szybuje w powietrze. Odzyskuj±c równowagê $n odskakuje na bezpieczn± odleg³o¶æ z satysfakcj± obserwuj±c wyrwê w boku $Z.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				if ( dam <= 0 )
				{
					act( "{5Rozpêdzasz siê w kierunku $Z, jednak przez z³e oszacowanie odleg³o¶ci wpadasz na $A z trudem wyprowadzaj±c cios.{x", ch, wield, victim, TO_CHAR );
					act( "{5$n odchyla siê do ty³u po czym rusza do przodu z potê¿n± szar¿±, jednak przez z³e oszacowanie odleg³o¶ci wpada na ciebie z trudem wyprowadzaj±c cios.{x", ch, wield, victim, TO_VICT );
					act( "{5$n odchyla siê do ty³u po czym rusza w kierunku $Z z potê¿n± szar¿±, jednak przez z³e oszacowanie odleg³o¶ci wpada na $A z trudem wyprowadzaj±c cios.{x", ch, wield, victim, TO_NOTVICT );
					return TRUE;
				}
				else if ( victim->hit - dam < -11 )
				{
					int types = 1;

					if ( ch->class == CLASS_BARBARIAN && is_affected(ch, gsn_berserk ) )
						types = 2;

					switch ( number_range( 0, types ) )
					{
						case 0:
							if ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
							{
								act( "{5Twoje pchniêcie by³o wyj±tkowo celne, trafiasz w samo serce $Z! $N spogl±da ostatni raz na ciebie martwym wzrokiem po czym upada na ziemiê. Buchaj±ca z cia³a fontanna {Rkrwi{5 barwi wszystko dooko³a jasn± czerwieni±.{x", ch, wield, victim, TO_CHAR );
								act( "{5$p $z trafia ciê w samo serce! Czujesz przera¼liwy ch³ód, po czym co¶ uderza ciê z potworn± si³± w plecy - w ostatnim przeb³ysku my¶li dochodzisz do wniosku, ¿e to ziemia.{x", ch, wield, victim, TO_VICT );
								act( "{5$p $z trafia $C w samo serce! $N chwieje siê przez chwilê po czym upada na ziemiê. Buchaj±ca z cia³a fontanna {Rkrwi{5 barwi wszystko dooko³a jasn± czerwieni±.{x", ch, wield, victim, TO_NOTVICT );
								make_blood( victim );
								break;
							}

						case 1:
							act( "{5To by³o mistrzowskie pchniêcie! $p trafia $C w g³owê i przechodzi przez oko rozrywaj±c $S czaszkê na dwie czê¶ci!{x", ch, wield, victim, TO_CHAR );
							act( "{5Dostrzegasz bardzo szybko zbli¿aj±cy siê przedmiot. To $p $z trafia ciê w oko! Czujesz rozrywaj±cy ból! Jednak chwilê pó¼niej jest ci ju¿ niesamowicie lekko i ca³e cierpienie ustêpuje.{x", ch, wield, victim, TO_VICT );
							act( "{5$p $z trafia $C w oko! S³yszysz zgrzyt roztrzaskiwanej czaszki, po czym twoim oczom ukazuje siê rozdarta na pó³ g³owa $Z. Co za ohydny widok!{x", ch, wield, victim, TO_NOTVICT );
							make_brains(victim);
							break;
						case 2:
							act( "{5Nabijasz $C na w³óczniê! Gdy grot przebija $S cia³o, $N krzyczy z bólu! Podnosisz $M wysoko w górê, przytrzymujesz przez chwilê ponad swoj± g³ow±, a nastêpnie przerzucasz w ty³. $N umiera tocz±c krwist± pianê z ust.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n nabija ciê na $h! Gdy grot przebija twoje cia³o, czujesz straszliwy ból! $n podnosi ciê wysoko w górê, przytrzymuje przez chwilê ponad swoj± g³ow±, a nastêpnie przerzuca w ty³. Ostatnie co pamiêtasz to uderzenie o ziemiê.{x", ch, wield, victim, TO_VICT );
							act( "{5$n nabija $C na $h! $N krzyczy z bólu gdy grot przebija $S cia³o! $n podnosi $M wysoko w górê, przytrzymuje przez chwilê ponad swoj± g³ow±, a nastêpnie przerzuca w ty³. $N umiera tocz±c krew z ust.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				else
				{
					int types = 1;

					if ( ch->class == CLASS_BARBARIAN && is_affected(ch, gsn_berserk ) )
						types = 2;

					switch ( number_range( 0, types ) )
					{
						case 0:
							act( "{5Wykonujesz precyzyjne pchniêcie, $p g³êboko wbija siê w cia³o $Z rozrywaj±c $S tkanki.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n wykonuje precyzyjne pchniêcie, $s $p g³êboko wbija siê w twoje cia³o rozrywaj±c tkanki.{x", ch, wield, victim, TO_VICT );
							act( "{5$n wykonuje precyzyjne d¼gniêcie, $s $p g³êboko wbija siê w cia³o $Z rozrywaj±c tkanki.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 1:
							act( "{5Wyprowadzasz potê¿ne pchniêcie $j w $C, twoja broñ bez trudu wynajduje lukê w obronie $Z i dewastuje $M bezlito¶nie!{x", ch, wield, victim, TO_CHAR );
							act( "{5$n wyprowadza w ciebie potê¿ne pchniêcie $j. Impet by³ tak potê¿ny, ¿e twoje próby os³oniêcia siê spe³z³y na niczym. Potworny ból rozdziera twoje cia³o, gdy $p przebija skórê.{x", ch, wield, victim, TO_VICT );
							act( "{5$n wyprowadza potê¿ne pchniêcie $j w $C, $s broñ bez trudu wynajduje lukê w obronie $Z i dewastuje $M bezlito¶nie!{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 2:
							act( "{5W szalonym tañcu wykonujesz pchniêcie z pó³obrotu w bok $Z. $p bez trudu wbija siê g³êboko w $S cia³o szerz±c powa¿ne zniszczenia.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n w szalonym tañcu wykonuje pchniêcie z pó³obrotu w twój bok. $S $p bez trudu wbija siê w twoje cia³o szerz±c powa¿ne zniszczenia.{x", ch, wield, victim, TO_VICT );
							act( "{5$n w szalonym tañcu wykonuje pchniêcie z pó³obrotu w bok $Z. $p $z bez trudu wbija siê g³êboko w cia³o $Z szerz±c powa¿ne zniszczenia.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				break;

			case WEAPON_MACE:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wiêksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5S³yszysz mro¿±cy krew w ¿y³ach ¶wist gdy $p opada z zastraszaj±c± szybko¶ci± na ³eb $Z. W wyobra¼ni ju¿ widzisz rozchlapuj±c± siê wszêdzie cuchn±c±, trupi± ciecz oraz rozpaækan± galaretowat± masê na obuchu $f. Twa broñ opada, jednak czujesz, ¿e co¶ posz³o nie tak. T³uczesz w ziemiê mijaj±c szkaradny ³eb i nie czyni±c wrogowi najmniejszej krzywdy, musia³<&e¶/a¶/o¶> ¼le oceniæ odleg³o¶æ. Od uderzenia drêtwiej± ci rêce, lecz nie masz czasu, aby siê nad tym, zastanawiaæ. Ca³± uwagê po¶wiêcasz teraz na unikaniu zajad³ych ciosów przeciwnika.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz na spadaj±cy ze ¶wistem obuch. Widzisz jak $n zatrzymuje siê w miejscu i wyprowadza potê¿ny cios na czaszkê $Z. Jednak, ku twemu zaskoczeniu, $p z g³uchym odg³osem t³ucze w ziemiê. $n prawie pada na kolana przed rozw¶cieczonym nieumar³ym, jednak udaje $m siê utrzymaæ na nogach.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Z twej piersi wydobywa siê ryk w¶ciek³o¶ci. Napinasz miê¶nie ramion i bierzesz silny zamach. Przez gnij±c± twarz $Z przebieg³ jakby cieñ zrozumienia, jakby przeczu³ zbli¿aj±c± siê z ka¿d± sekund± zgubê. Obuch twojego $f z zastraszaj±c± si³± opada na czaszkê przeciwnika, jego g³owa z cichym chrupotem rozpada siê na kawa³ki. Z przegni³ych oczodo³ów wylewaj± siê ga³ki oczne, odpadaj± p³aty miêsa oraz pozosta³o¶ci rysów twarzy – policzki, nos, uszy. Z twarzy nieumar³ego zosta³a tylko krwawa papka. Nastêpnie $p zag³êbia siê g³êboko w korpus przeciwnika dos³ownie wbijaj±c go w pod³o¿e. Z cichym mlaskiem broñ koñczy sw± mordercz± wêdrówkê.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n szaleñczo rycz±c wyprowadza ¶miertelne uderzenie celuj±c w g³owê $Z. Cios jest tak potê¿ny, ¿e w chwili uderzenia $f ³eb nieumar³ego pêka jak skorupka jajka. Obuch nie koñczy na tym swej krwawej wêdrówki, zag³êbia siê od góry w szyjê, klatkê piersiow± oraz brzuch przeciwnika. Impet uderzenia mia¿d¿y wszystko co napotka na swej drodze, nawet przegni³e koñczyny ³ami± siê jak suche patyki po sam± si³± uderzenia. Kiedy pozosta³e narz±dy hamuj± impet uderzenia po przeciwniku w zasadzie nie ma ju¿ ¶ladu. Na ziemi zostaje jedynie ka³u¿± cuchn±cej, b±belkuj±cej cieczy, pl±tanina flaków i miêsa oraz po³amanych ko¶ci.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Pêdzisz na z³amanie karku wymachuj±c szaleñczo $j. Z szybko¶ci± b³yskawicy, w paru skokach dobiegasz do $Z i z potê¿nym wymachem opuszczasz obuch swej broni prosto w plecy przeciwnika. Do twych uszu dociera niemi³y d¼wiêk strzaskanych ko¶ci oraz pêkaj±cych, wydobywaj±cych siê wnêtrzno¶ci. Z ziej±cej dziury w torsie nieumar³ego wydobywa siê chmura cuchn±cego gazu. Si³a ciosu odrzuca go na parê metrów, jednak $N szybko otrz±sa siê z otrzymanego uderzenia i szykuje siê do ataku.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl±dasz na rozgrywaj±c± siê przed tob± scenê. Widzisz jak $n w paru szybkich susach podbiega do $Z, wymija swój cel i skrêtem cia³a wyprowadza potê¿ny cios. Z klatki piersiowej nieumar³ego strzela fontanna przegni³ych flaków oraz wydobywaj± siê cuchn±ce opary zgnilizny. Nieumar³y wydobywa z siebie przera¿aj±cy, dono¶ny skrzek i sunie po ziemi parê metrów. Nie przewraca siê jednak, a potrz±saj±c ³bem, gubi±c kawa³ki cia³a szykuje siê do ataku.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5¦ciskaj±c mocno trzon $f bierzesz potê¿ny zamach. Obuch wêdruje w powietrzu i z potworn± si³± uderza w ramiê $Z. Cios jest tak silny, ¿e prawie wyrywa rêkê nieumar³emu. Odg³os darcia siê ¶ciêgien i w³ókien miê¶ni rozlega siê g³o¶no i rani twe uszy, jednak $N chwiej±c siê na nogach rusza do ataku.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak potworny cios $z wêdruje w kierunku ramienia $Z. Obuch $f ze ¶wistem opada na rêkê przeciwnika i prawie mu j± wyrywa. Koszmarny d¼wiêk dartej skóry, miê¶ni oraz ¶ciêgien rozlega siê po okolicy. $N chwiejnym krokiem rusza jednak do ataku kontynuuj±c walkê.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet"))
				{
					if ( dam <= 0 )
					{
						act( "{5Nacierasz b³yskawicznie na przeciwnika. Kierujesz $h z zastraszaj±c± si³± na korpus o¿ywieñca. Broñ ze ¶wistem tnie powietrze pod±¿aj±c w nadanym przez ciebie kierunku. $N, patrz±c na ciebie beznamiêtnie, stoi w bezruchu jakby nie zdawa³ sobie sprawy z gro¿±cego mu niebezpieczeñstwa. Obuch twej broni o centymetry mija klatkê piersiow± nieruchomego przeciwnika. ¬le wymierzona odleg³o¶æ powoduje, ¿e chybiasz haniebnie, a wróg wykorzystuj±c sytuacjê, klekocz±c ko¶æmi przy ka¿dym b³yskawicznym ruchu, wyprowadza potê¿ne ataki.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl±dasz na wpatruj±cych siê w siebie wyzywaj±co przeciwników. $n wyprowadza potê¿ny cios kieruj±c $h na klatkê piersiow± wroga. O dziwo $N stoi nieruchomo jak szkaradny, kamienny pos±g szyderczo ¶widruj±c pustymi oczodo³ami sylwetkê nacieraj±cego $z. Czy¿by zmursza³a, wyschniêta, pozbawiona skóry i miê¶ni twarz przybra³a kpi±cy wyraz? Wyrzucony w powietrze obuch zatacza szeroki ³uk i mija haniebnie cel, w którym pod±¿a³, poci±gaj±c za sob± dier¿±cego. Klekocz±c dono¶nie ko¶æmi, przy ka¿dym z b³yskawicznych ruchów szkielet wyprowadza potê¿n± kontrê.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Ze w¶ciek³ym rykiem wyprowadzasz morderczy cios skierowany w po³yskuj±c± czaszkê przeciwnika. Ten jednak nag³ym zrywem odchyla siê lekko do ty³u. Rozpêdzony obuch $f ze ¶wistem opada na miednicê. Si³a ciosu jest tak wielka, ¿e rozpada siê ona w mgnieniu oka z g³uchym odg³osem na tysi±ce drobnych kawa³ków oddzielaj±c zmursza³e nogi szkieletu od korpusu. Chmura wiekowego, stêch³ego py³y zas³ania ci widoczno¶æ. Jak przez mg³ê spogl±dasz na opadaj±cy, wymachuj±cy w powietrzu rêkoma korpus przeciwnika. W jednej sekundzie unosisz broñ z morderczym b³yskiem w oku i opuszczasz j± raz za razem wgniataj±c o¿ywieñca w pod³o¿e. Ze $Z nie pozostaje nic oprócz porozrzucanych wszêdzie kostnych drobin i tañcz±cej w powietrzu chmury szarawego py³u. Ocieraj±c pot z czo³a warczysz w¶ciekle rozgl±daj±c siê po okolicy.{x", ch, wield, victim, TO_CHAR );
						act( "{5Do twych uszu dociera potworny ryk $Z, szar¿uje $e w¶ciekle na przeciwnika. Potê¿ne walniêcie z zastraszaj±c± prêdko¶ci± opada na ods³oniêt±, po³yskuj±c± czaszkê $Z, jednak o¿ywieniec jest zbyt czujny i odchyla siê b³yskawicznie do ty³u, lecz rozpêdzony obuch opada na zmursza³± miednicê nieumar³ego, która z g³uchym, pustym ³oskotem rozpada siê na tysi±ce drobin kostnych oddzielaj±c dolne koñczyny od korpusu szkieletu. Wzbita przy tym chmura wiekowego py³u zas³ania widoczno¶æ. Kiedy opada widzisz sylwetkê $z t³uk±c± bez opamiêtania w le¿±cy korpus przeciwnika. Kiedy milknie odg³os ostatniego uderzenia po $X zostaje jedynie walaj±ca siê wszêdzie kupka od³amków kostnych.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5W kilku skokach dopadasz przeciwnika. Bierzesz potê¿ny zamach i t³uczesz $j z ca³ych si³ w ods³oniêty korpus $Z. Walniêcie dosiêga ¿eber powoduj±c seriê zgrzytliwych, suchych trzasków. Wywo³ujesz istn± burzê kostnych, ostrych jak sztylety od³amków i wzniecasz chmurê wiekowego, szarawego py³u. W niemej, ukrytej w pustych oczodo³ach nienawi¶ci, $N spogl±da na wybit± w swym boku olbrzymi± wyrwê.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz pêdz±cego $z w kierunku $Z. Dopada wroga w u³amku sekundy, bierze potê¿ny zamach i wyprowadza potê¿ny cios $j w bok szkieletu. Zderzenie $f z suchymi ko¶æmi wywo³uje burzê kostnych, ostrych odprysków oraz wznieca chmurê wiekowego py³u. Do twych uszu dobiega zgrzytliwy d¼wiêk mia¿d¿onych ¿eber. W boku przeciwnika widnieje olbrzymia wyrwa.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Pêdzisz ca³ym impetem na $C unosz±c w górê $h. Przeciwnik w porê zauwa¿a gro¿±ce mu niebezpieczeñstwo i wyprowadza b³yskawiczny i potê¿ny kontratak na twój korpus. Robisz szybki unik schodz±c z toru ciosu zmursza³ego, wyschniêtego ramienia. Hamuj±c zaciekle suniesz kilka metrów po pod³o¿u ci±gniêty impetem szaleñczego biegu, opuszczaj±c przy tym broñ dla lepszego zachowania równowagi. Mijaj±c $Z spostrzegasz nadarzaj±c± siê okazjê do wyprowadzenia niezdarnego ciosu. Wykonujesz energiczne, rozpaczliwe pchniecie w bok wroga, po czym do twych uszu dociera pusty odg³os pêkaj±cych, kruchych ko¶ci.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n pêdzi jak rozjuszony tur w kierunku przeciwnika wysoko unosz±c $h. W porê jednak $N zauwa¿a gro¿±ce mu, straszliwe niebezpieczeñstwo. Wyprowadza b³yskawiczny kontratak na korpus szar¿uj±cego, ale $n zgrabnie unika zagro¿enia wykonuj±c nag³y skrêt cia³a, opuszczaj±c przy tym broñ. Sunie parê metrów po pod³o¿u pchany impetem zapieraj±c siê rozpaczliwie nogami. Mijaj±c $C wyprowadza opuszczon± broni± rozpaczliwe pchniêcie w ¿ebra przeciwnika wy³amuj±c wiele z nich.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cieñ" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiór" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Bierzesz potê¿ny zamach i wyprowadzasz szybkie, potê¿ne walniêcie w niematerialn± g³owê przeciwnika. $N przewiduje jednak twój zamiar i wykonuje b³yskawiczny unik w bok unosz±c siê lekko w nad ziemi±. Obuch mija pó³przezroczyst± postaæ o centymetry t³uk±c g³ucho w pod³o¿e. Niematerialna sylwetka pod wp³ywem podmuchu powietrza wywo³anego ciosem zamazuje siê lekko i faluje przed twoimi oczami. Pokonuj±c potworny ból ramion powoli unosisz ponownie $h. Przez twój umys³ przebiega niezrozumia³y ¶liski, cichy, drwi±cy szept w jakim¶ prastarym jêzyku, którego nie rozumiesz.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze potê¿ny zamach i wyprowadza morderczy cios na g³owê przeciwnika, jednak $N wykonuje b³yskawiczny zryw w bok lawiruj±c lekko nad ziemi±. Rozpêdzony obuch t³ucze z potê¿nym, g³uchym ³oskotem o centymetry od niematerialnego wroga. Sylwetka przeciwnika rozmazuje siê lekko i faluje pod wp³ywem podmuchu powietrza. $n o ma³o nie upuszczaj±c broni unosi j± ponownie dr¿±cymi, zdrêtwia³ymi d³oñmi z grymasem bólu na twarzy.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5$N naciera b³yskawicznie na ciebie upiornie wyj±c. Spodziewa³<&e¶/a¶/o¶> siê tego i z zimnym, bezlitosnym u¶miechem na twarzy wykonujesz b³yskawiczny skrêt cia³a i ustawiasz siê przodem do pleców przeciwnika. Z szyderczym rykiem unosisz $h i z w¶ciek³o¶ci± opuszczasz obuch miêdzy ³opatki wroga. Broñ z potworn± si³± i precyzj± siêga celu tworz±c w niematerialnym ciele rozleg³± wyrwê. $N odchyla g³owê w ty³ w szaleñczym cierpieniu. Z oczu, uszu, rozwartych szeroko ust oraz szerokiej rany w korpusie przeciwnika strzelaj± jaskrawe lance purpurowego ¶wiat³a. Po chwili niematerialna sylwetka rozp³ywa siê w niemej eksplozji czerwonego blasku. Wokó³ panuje niezm±cona niczym, b³oga cisza.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $N naciera b³yskawicznie na $C, jednak $e w u³amku sekundy wykonuje skrêt cia³a ustawiaj±c siê przodem do pleców przeciwnika i unosi wysoko $h, kieruj±c obuch w sam ¶rodek niematerialnego korpusu przeciwnika. Broñ ze ¶wistem opada zgodnie ze swym przeznaczeniem tworz±c olbrzymi± wyrwê w po³przezroczym korpusie. $N odchyla g³owê w ty³ w szaleñczym cierpieniu, a z $S ust, uszu, szeroko rozwartych oczu i olbrzymiej rany buchaj± szerokie smugi purpurowego ¶wiat³a. Po chwili niematerialna sylwetka niknie w niemej eksplozji czerwonego ¶wiat³a. Wokó³ panuje grobowa cisza.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Nie okazuj±c strachu kroczysz pewnie ¶ciskaj±c $h w kierunku $Z. W twym umy¶le rozchodzi siê nieustaj±co przera¿aj±cy, sycz±cy szept przeciwnika. Na zmarszczonym czole pojawiaj± siê strugi zimnego potu. Musisz to zakoñczyæ. Pozbyæ siê o¶liz³ych g³osów rozbrzmiewaj±cych w twym udrêczonym umy¶le. Natychmiast! Ze zduszonym jêkiem wyprowadzasz potê¿ny cios, rozpêdzony obuch opada ze straszliw± si³± na klatkê piersiow± $Z wydobywaj±c z pó³materialnego cia³a szerokie smugi jaskrawoczerwonego ¶wiat³a. Piekielny szept milknie na chwilê, zastêpuje go opêtañczy wrzask.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n kroczy pewnym krokiem w kierunku $Z mocno zaciskaj±c rêce na trzonie swej opuszczonej broni. Twarz atakuj±cego ma zatrwa¿aj±cy wyraz. Mieszaj± siê na niej w¶ciek³o¶æ i przera¿enie. Zbli¿aj±c siê do $Z wznosi wysoko w górê $h i opuszcza z zastraszaj±c± si³± i szybko¶ci± trafiaj±c w pó³przezroczyst± klatkê piersiow±. W chwili zetkniêcia siê broni z niematerialnym cia³em spod obucha wydobywaj± siê czerwone lance jaskrawego ¶wiat³a. Do twego umys³u dociera cieñ opêtañczego wrzasku.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5¦widruj±cy mózg, ¶liski szept rozbrzmiewaj±cy w twym umy¶le przyprawia ciê prawie o ob³êd. Czerwone, rozbiegane p³aty w¶ciek³o¶ci przes³aniaj± ci widoczno¶æ. Jak przez mg³ê szaleñstwa widzisz unosz±c± siê nad ziemi± rozmazan± sylwetkê przeciwnika. Ze w¶ciek³ym rykiem kierujesz obuch $f w brzuch $Z, jednak przeciwnik jakim¶ sposobem odbija, nie wykonuj±c nawet jednego gestu, rozpêdzon± broñ i cios ze ¶witem opada na koñczynê wroga. Z rozdartej, pó³materialnej nogi strzela promieñ jaskrawoczerwonego ¶wiat³a ra¿±c twe oczy.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl±dasz na $n. Kieruje siê $e w stronê $Z na dr¿±cych nogach. Ze w¶ciek³ym rykiem unosi $h i z zastraszaj±c± prêdko¶ci± stara siê uderzyæ w brzuch przeciwnika, jednak broñ odbija siê jakby od niewidzialnej tarczy mijaj±c cel i ze ¶wistem opada na doln± koñczynê $Z, tworz±c w niej spor± wyrwê. Z pó³przezroczystej nogi strzela smuga jasnego czerwonego ¶wiat³a i blednie tak szybko jak siê pojawi³a.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(victim->form, FORM_CONSTRUCT))
				{
					if ( dam <= 0 )
					{
						act( "{5Starasz siê wykonaæ efektown± szar¿ê na $C. Niestety, przeceni³<&e¶/a¶/o¶> swe mo¿liwo¶ci i niezbyt dobrze siê przygotowa³<&e¶/a¶/o¶> i $p zaledwie muska powierzchniê $Z, a $E potê¿nym ruchem rêki prawie wytr±ca ci orê¿ z d³oni.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n próbuje potê¿ny cios $X. Bierze du¿y rozpêd, wznosi nad g³owê $h i wyprowadza cios. Niestety, $N okazuje siê byæ o niebo szybszy, usuwa siê z trajektorii $f i sprawnym ruchem potê¿nej ³apy prawie wytr±ca $x broñ z rêki.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Ruszasz naprzód, ¶ciskaj±c obur±cz $h i nie kryj±c przy tym swych intencji. Twe cia³o nabiera rozpêdu, w twe serce wlewa siê odwaga, na twych wargach rozbrzmiewa pie¶ñ zwyciêstwa! Gdy dobiegasz do $Z nie zastanawiaj±c siê ani trochê co robisz, bierzesz potê¿ny zamach i twój $p z niesamowit± prêdko¶ci± uderza w sam ¶rodek potê¿nego torsu wroga. Ten z kolei, wydaj±c og³uszaj±cy, bêbni±cy d¼wiêk efektownie rozlatuje siê na kawa³ki, które to znowu rozsypuj± siê w drobny mak.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z piekielnym wrzaskiem na ustach rusza naprzód, ¶ciskaj±c obur±cz $h i nie kryj±c przy ty ¿adnych intencji. Dostrzegasz, jak $s cia³o nabiera rozpêdu, a w oczach pojawia siê ¿±dza niszczenia. Gdy dobiega do $Z nie zastanawiaj±c siê ani trochê co robi, bierze potê¿ny zamach a $s $p z niesamowit± prêdko¶ci± uderza w sam ¶rodek potê¿nego torsu $Z, a $E z kolei, wydaj±c og³uszaj±cy, bêbni±cy d¼wiêk, efektownie rozlatuje siê na kawa³ki, które to rozsypuj± siê w drobny mak.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Wci±¿ nie spuszczaj±c z oczu potê¿nej sylwetki $Z dok³adnie planujesz swe uderzenie. ¦ciskaj±c jak tylko mo¿esz najmocniej grub± rêkoje¶æ $f i wykonuj±c efektowny obrót m³ócisz przeciwnika prosto w ¶rodek pleców, po czym widzisz jak od jego cielska odpada parê p³atów dziwnego materia³u. Szybko wycofujesz siê na bezpieczn± odleg³o¶æ i z nowym zapa³em kontynuujesz walkê.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n gwa³townie wci±ga powietrze i rozpoczyna desperacki bieg. Zauwa¿asz, jak nie zpuszczaj±c z pola widzenia $Z planuje dok³adnie potê¿n± szar¿ê. Wykonuj±c efektowny obrót m³óci przeciwnika prosto w ¶rodek pleców i z wielk± przyjemno¶ci± podziwia, jak od jego cielska odpada parê p³atów dziwnego materia³u.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Rozpoczynasz szaleñczy, desperacki bieg w kierunku $C. Z zai¶cie wspania³± zwinno¶ci± unikasz ciosów $S monstrualnych r±k i wci±gaj±c gwa³townie powietrze unosz±c $h wysoko w górê. Bierzesz seriê potê¿nych zamachów i m³ócisz kilkukrotnie w stopê wroga najmocniej jak tylko mo¿esz. Po chwili dostrzegasz, jak $n kuleje, a od jego stopy stopniowo odpadaj± zmasakrowane palce.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n trzymaj±c $h nisko przy ziemi biegnie wprost na $C. Nabieraj±c prêdko¶ci doskakuje w ¶wietnym stylu do nogi przeciwnika i bior±c seriê potê¿nych zamachów m³óci w jego stopê jak tylko mo¿e, odbijaj±c od reszty cia³a parê poka¼nych wielko¶ci paluchów.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(race_table[GET_RACE(victim)].type , ANIMAL))
				{
					if ( dam <= 0 )
					{
						act( "{5Rzucasz siê z krzykiem na $C i rozpoczynasz szaleñczy bieg. Bior±c jednak potê¿ny zamach w celu wykonania koñcowego ciosu, stwierdzasz, ¿e co¶ jest nie tak. Nawet nie poczu³<&e¶/a¶/o¶>, jak w czasie biegu zgubi³<&e¶/a¶/o¶> $h! Szybko podnosisz sw± broñ i z wstydem kontynuujesz walkê.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n rzuca siê z krzykiem na $C i rozpoczyna szaleñczy bieg. Nagle jednak $p wypada z $s z rêki... ale có¿ to? Ta ³amaga biegnie dalej! W koñcu dopiero dostrzega swój b³±d i wraca po porzucony orê¿, z wymalowanym na twarzy poczuciem wstydu.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Bierzesz rozpêd i z szaleñczym rykiem rzucasz siê na $C, szaleñczo wymachuj±c $j. Na twarzy czujesz podmuchy powietrza, wzbudzonego przez orê¿, gdy ten niebezpiecznie wiruje wokó³ twej g³owy. Bezbronne wobec przejawu twej agresji zwierzê próbuje siê uchyliæ, lecz jest ju¿ za pó¼no. Na $C ze ¶wistem spada $h, wgniataj±c $S cia³o w ziemiê i przy okazji zamieniaj±c wszystkie organy w zbitkê miêsa. Krwista posoka obryzguje wszystko dooko³a, a ty z zadowoleniem spogl±dasz na swe dzie³o.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze rozpêd i z szaleñczym rykiem rzuca siê na $C, szaleñczo wymachuj±c $j. Ciarki przechodz± ciê po plecach, gdy podziwiasz niebezpiecznie wiruj±cy orê¿, a do twych uszu dochodzi ¶wist wzbudzonego wiatru. Zwierzê, bezbronne wobec przejawu agresji $z, próbuje siê uchyliæ, lecz jest ju¿ dla niego za pó¼no. Obuch $f spada na niego ze ¶wistem, wgniataj±c cia³o w ziemiê i przy okazji zamieniaj±c wszystkie organy zbitkê miêsa. Krwista posoka obryzguje wszystko dooko³±, ochlapuj±c ciê ca³kowicie.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Dzier¿±c obur±cz $h, wydobywasz z p³uc bitewny okrzyk i rzucasz siê na $C, a $E, ca³kowicie zaskoczon$R, próbuje siê jeszcze uchyliæ. Na $S nieszczê¶cie jeste¶ o wiele szybsz$r. Wiruj±ca dot±d szaleñczo broñ spada z impetem na odkryte cia³o $Z i zwala $S z nóg, dotkliwie przy tym rani±c.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n dzier¿±c obur±cz $h, wydobywa z p³uc bitewny okrzyk i rzuca siê na $C, a $E, ca³kowicie zaskoczon$R, próbuje siê jeszcze uchyliæ, bez efektu. Wiruj±ca szaleñczo broñ z potê¿n± si³± spada na $C i zniekszta³ca czê¶æ cia³a, mia¿d¿±c miê¶nie i ko¶ci.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Wydobywasz z siebie g³o¶ny ryk i zmuszasz swe nogi do szaleñczego biegu w stronê $Z, wymachuj±c p³ynnie $j. Sylwetka odkrytego zwierzêcia zbli¿a siê z ka¿d± sekund±, a ty wci±¿ nabierasz pêdu. U celu bierzesz szeroki rozmach i z ca³ej si³y uderzasz od spodu. Wyrzucon$R w powietrze $N spada parê metrów dalej z g³uchym ³upniêciem, a do twych uszu dociera ni to pisk, ni to stêkniêcie.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wydobywa z siebie g³o¶ny ryk i zmusza swe nogi do szaleñczego biegu w stronê $Z, wymachuj±c p³ynnie $j. Sylwetka odkrytego zwierzêcia zbli¿a siê z ka¿d± sekund±, a $e wci±¿ nabiera pêdu. U celu zatacza sw± broni± ³uk i z ca³ej si³y uderza od spodu. Biedne zwierzê ulatuje w powietrze, spada parê metrów dalej z g³uchym piskiem i z trudem podnosi siê na nogi.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
			case WEAPON_STAFF:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wiêksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Stoisz naprzeciw $Z gotuj±c siê do wymierzenia ciosu. Przeciwnik naciera na ciebie z impetem, wymachuj±c rozpadaj±cymi siê ramionami. Uchylasz siê i wymierzasz potê¿ny cios w bok o¿ywieñca. Niestety, wróg jest ostro¿ny i chwyta obur±cz koniec $f unieruchamiaj±c go. Szarpiesz z ca³ej si³y próbuj±c wyrwaæ broñ z z¿artych zgnilizn± d³oni, jednak nie doceni³<&e¶/a¶/o¶> nadnaturalnej krzepy nieumar³ego. Si³ujesz siê d³u¿sz± chwilê z przeciwnikiem, nie przynosi to jednak oczekiwanego rezultatu. Nagle wykonujesz nag³y wykrok. Zapierasz siê nog± o korpus wroga i wyrywasz broñ z cuchn±cych r±k trac±c równowagê. Wydobywaj±c z siebie przeci±g³y skrzek $N w¶ciekle naciera na ciebie.{x", ch, wield, victim, TO_CHAR );
						act( "{5Obserwujesz jak $n rzuca siê wyj±c na $C, $e przykuca unikaj±c morderczych ramion o¿ywieñca i wymierza niezdarny cios w ods³oniêty bok wroga. Jednak nieumar³y zadziwiaj±co zrêcznym i szybkim ruchem chwyta obur±cz pêdz±cy w jego kierunku obuch $f unieruchamiaj±c go. $n rzuca siê w ty³ próbuj±c wyszarpaæ swoj± broñ, bezskutecznie. Przeciwnicy szarpi± siê chwilê w niemych zmaganiach. Nagle $n wyrzuca nogê w przód, zapiera siê o korpus $Z i potê¿nym szarpniêciem odzyskuje $h.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Pêdzisz szaleñczo szar¿uj±c na $C ze w¶ciek³ym rykiem. W biegu podrzucasz $h, ³apiesz obur±cz za jeden z koñców, b³yskawicznie oceniasz odleg³o¶æ i uderzasz z ca³ych si³ w g³owê przeciwnika. Broñ niebezpiecznie zatrzeszcza³a, ale wytrzyma³a impet uderzenia. W przeciwieñstwie do czaszki o¿ywieñca. Jego g³owa prys³a w jednej sekundzie wyrzucaj±c rozpaækany mózg i hektolitry cuchn±cej, gêstej, ¿ó³toszarej mazi. Ponownie pierzesz szeroki zamach i t³uczesz oszo³omionego nieumar³ego potê¿nie w ods³oniête plecy. Si³a ciosu odrzuca go na parê metrów. Wykonujesz potê¿ny, b³yskawiczny skok podpieraj±c siê na $j i skaczesz na wroga powalaj±c go ostatecznie na ziemiê. Wbijasz z potê¿n± si³± koniec swej broni miêdzy ³opatki przeciwnika przyszpilaj±c i unieruchamiaj±c drgaj±ce jeszcze cia³o. Wszystko to trwa³o niepe³na parê sekund. Wyszarpujesz $h z cuchn±cego ¶cierwa wyj±c przy tym dono¶nie.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n pêdzi szaleñczym tempem w stronê $Z. Nie zwalniaj±c biegu szar¿uj±cy podrzuca $h wysoko w górê po czym ³apie broñ obur±cz za jeden z koñców i t³ucze od góry w czaszkê wroga, która rozpryskuje siê praktycznie w chwili zetkniêcia siê z broni±. B³yskawicznym ruchem $N odskakuje, bierze zamach i wali z ca³ych si³a w ods³oniête plecy przeciwnika, którego impet ciosu odrzuca na parê metrów. Podpieraj±c siê niczym na tyczce atakuj±cy wybija siê wysoko w górê i opada prosto na $C zwalaj±c wroga z nóg. $n wymierza miêdzy ³opatki potê¿ne pchniêcie i unieruchamia drgaj±ce jeszcze cia³o. Wyrywaj±c broñ barbarzyñca wyje w szaleñczym triumfie.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Obracasz leniwym ruchem sw± broñ w d³oniach okr±¿aj±c przy tym przeciwnika. Koñce $f zataczaj± szerokie krêgi,  $N próbuje przedrzeæ siê przez stworzon± tak zas³onê, jednak bezskutecznie. Nagle, niezdarnym pchniêciem, pozorujesz atak na brzuch o¿ywieñca. Wróg uchyla siê, zasadzka siê powiod³a. Potê¿nymi, szybkimi, precyzyjnymi ruchami wyprowadzasz seriê mocnych ciosów w ods³oniête czê¶ci rozk³adaj±cego siê cia³a. Uderzenia trafiaj± po kolei w g³owê, ¿ebra i dolne koñczyny przeciwnika. Gêsta, cuchn±ca, zielono¿ó³ta ma¼ bryzga przy ka¿dym z potê¿nych uderzeñ. S³ychaæ trzask ³amanych ko¶ci. Nieumar³y chwieje siê na nogach pod ciosami wyj±c przera¼liwie. Spogl±dasz na skutki swoich zabiegów i u¶miechasz siê z zadowoleniem. Z na wpó³ zmia¿d¿onej g³owy wycieka mózg, z boku wyzieraj± po³amane ¿ebra, a z kolana sterczy ostra, pozbawiona szpiku ko¶æ. $N utykaj±c rusza w twoj± stronê skrzecz±c w¶ciekle.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n okr±¿a powoli przeciwnika zataczaj±c przy tym krêgi $j tworz±c zas³onê nie do przebycia przed ciosami $Z. Nagle zatrzymuj±c siê w miejscu kieruje jeden z koñców swej broni powolnym, niezdarnym ruchem w kierunku nieumar³ego. $N uchyla siê przez co s³abnie $S obrona. $n ze ¶wistem wypuszczaj±c powietrze wyprowadza seriê mia¿d¿±cych ciosów. Trafia w g³owê, która z paskudnym chrzêstem ods³ania przegni³y mózg, ¿ebra oraz nogê. Ze zmia¿d¿onego boku przez skórê przebijaj± siê ostre kawa³ki ¿eber. Z nogi wystaje strzaskana ko¶æ piszczelowa. Chrzêstowi ³amanych ko¶ci towarzyszy upiorne wycie. Przy ka¿dym z ciosów z cia³a o¿ywieñca tryskaj± strugi zielono¿ó³tej, cuchn±cej ropy. Chwiej±c siê na nogach $N ponownie naciera na $c.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Trzymaj±c obur±cz $h wymierzasz precyzyjny cios w praw± stronê twarzy $Z. Przeciwnik unosi b³yskawicznym ruchem rozpadaj±c± siê rêkê i uderzenie trafia w przedramiê o¿ywieñca. Z cichym zgrzytem ze¶lizguje siê po wilgotnej od trupiego jadu ko¶ci i trafia w szczêkê wroga wywo³uj±c pe³en bólu skrzek oraz mokre odg³osy odrywanego cia³a i wyp³ywaj±cych ga³ek ocznych. Do twych uszu dociera trzask wybijanych zêbów i ³amanej ¿uchwy. Nieumar³y obraca zmasakrowan± twarz w twoim kierunku. Dolna szczêka wisi bezw³adnie na kilku w³óknach przegni³ych miê¶ni, a oczodo³y ¶wiec± czarn± pustk±. Z jego gard³a wydobywa siê cichy, w¶ciek³y gulgot. Strumieñ cuchn±cej posoki bije teraz z jego gard³a bez opamiêtania. Nie zastanawiaj±c siê d³u¿ej ¶ciskasz mocniej sw± broñ i czekasz na kontratak.{x", ch, wield, victim, TO_CHAR );
						act( "{5Patrzysz jak $n wyprowadza precyzyjny cios w g³owê przeciwnika. $N wyrzuca jednak w górê przegni³e przedramiê i blokuje uderzenie, $p ze¶lizguje siê po wilgotnej od trupiego jadu i ropy ko¶ci trafiaj±c w twarz o¿ywieñca. Wywo³uje to fontannê cuchn±cej posoki, trzask ³amanych ko¶ci oraz mokre mla¶niêcia odrywanego cia³a i wyp³ywaj±cych ga³ek ocznych szybuj±cych w powietrze. Dolna szczêka $Z ko³ysze siê bezw³adnie przy ka¿dym ruchu rozk³adaj±cej siê g³owy. Puste oczodo³y ra¿± bij±c± z nich pustk±. Z gard³a nieumar³ego wyciekaj± galony cuchn±cej ropy. Z cichym, pe³nym w¶ciek³o¶ci rzê¿eniem $N rzuca siê na $c.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cieñ" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiór" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Chwytaj±c obur±cz za jeden z koñców swego $f bierzesz potê¿ny zamach i wyprowadzasz potê¿ne walniêcie. Niestety $N b³yskawicznie robi unik i schodzi z toru ciosu, a orê¿ tnie powietrze ze ¶wistem i uderza g³ucho w pod³o¿e tu¿ obok przeciwnika. Przez twe napiête ramiona przebiega dreszcz ostrego bólu i odrêtwienia. Na domiar z³ego potykasz siê i walisz na kolana w niezdarnej próbie odzyskania równowagi. Turlaj±c siê po ziemi i zrywasz siê na równe nogi.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n chwyta obur±cz $h, unosi wysoko do góry i wyprowadza potê¿ne walniêcie na korpus $Z. Niematerialny przeciwnik robi w u³amku sekundy b³yskawiczny unik i potê¿ny cios nie trafia. Orê¿ z g³uchym odg³osem t³ucze o pod³o¿e. Z wyrazem bólu na twarzy $n traci równowagê i pada na kolana, po czym turlaj±c siê po ziemi oddala siê od przeciwnika i zrywa na równe nogi.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Biegniesz szaleñczo w kierunku przeciwnika ¶ciskaj±c obur±cz $h. Zbli¿asz siê do lawiruj±cej w powietrzu postaci $Z z zastraszaj±c± szybko¶ci±. Twój orê¿ bezg³o¶nie zatapia siê w samym ¶rodku niematerialnej klatce piersiowej przeciwnika przebijaj±c j± na wylot. Wokó³ zatopionego g³êboko w pó³przezroczystym ciele orê¿a tryskaj± gejzery purpurowego ¶wiat³a. Na pocz±tku w±ska wyrwa w astralnym korpusie rozszerza siê teraz przybieraj±c coraz wiêksze rozmiary. Jaskrawy blask poch³ania teraz ju¿ prawie ca³± sylwetkê przeciwnika. Nag³a, bezg³o¶na, o¶lepiaj±ca ¶wietlna eksplozja o¶lepia ciê. Po chwili po $B nie pozostaje ¿aden ¶lad. Unosz±cy siê w powietrzu, piekielny wrzask bólu równie¿ siê rozp³ywa.{x", ch, wield, victim, TO_CHAR );
						act( "{5Spogl±dasz na zbli¿aj±cych siê do siebie z zastraszaj±c± prêdko¶ci± przeciwników. $n biegnie szaleñczo wyci±gaj±c przed siebie swój orê¿, a $N z wyci±gniêtymi ramionami sunie b³yskawicznie nad ziemi± wyj±c szaleñczo. Przy zetkniêciu wrogów $p zag³êbia siê bezszelestnie w niematerialnej klatce piersiowej przebijaj±c j± na wylot. Chwilê pó¼niej obie sylwetki poch³ania eksplozja purpurowego, o¶lepiaj±cego ¶wiat³a. Gdy odzyskujesz wzrok po $B nie ma ju¿ najmniejszego ¶ladu. Potworny ryk bólu równie¿ niknie w oddali zostawiaj±c po sobie dreszcz przerazenia.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Z bojowym okrzykiem na ustach rzucasz siê na $C, $E lawiruj±c niepewnie w powietrzu próbuje uj¶æ twojej szar¿y, jednak nadaremnie. ¦ciskaj±c obur±cz $h wyprowadzasz obiema koñcami seriê b³yskawicznych ciosów w g³owê i korpus przeciwnika. Ka¿demu zetkniêciu orê¿a z niematerialnym cia³em towarzyszy urywany wrzask rozlegaj±cy siê zgrzytliwie w twej g³owie. Odskakuj±c od $Z pozostawiasz kilkana¶cie wyrw w pó³przezroczystej pow³oce z których bij± w±skie smugi czerwonego ¶wiat³a.{x", ch, wield, victim, TO_CHAR );
						act( "{5Przera¼liwy okrzyk wojenny $z rozlega siê dono¶nie gdy rzuca siê $e w stronê $Z. Pó³materialna sylwetka lawiruje niepewnie w powietrzu próbuj±c zej¶æ z linii ataku, na pró¿no. Zataczaj±c szerokie krêgi $j $n wyprowadza seriê celnych, mocnych ciosów na g³owê i korpus przeciwnika. Przy ka¿dym bezg³o¶nym uderzeniu bardziej czuæ ni¿ s³ychaæ urywane wrzaski bólu. Na pó³przezroczystym ciele zauwa¿asz kilkana¶cie wyrw, z których bij± w±skie smugi o¶lepiaj±cego, czerwonego ¶wiat³a.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Atakuj±c $C przykucasz na moment i wybijasz siê wysoko w powietrze. ¦ciskasz obur±cz $h i wyprowadzasz cios z góry w g³owê przeciwnika. Opadasz z ogromn± prêdko¶ci± na pó³przezroczyst± sylwetkê z wyci±gniêtym przed siebie orê¿em. Zbli¿aj±c siê do wroga, s³ysz±c jedynie ¶wist powietrza w uszach wykonujesz krótki zamach sw± broni±, lecz $p mija haniebnie wyznaczony cel zahaczaj±c jednak o korpus $Z. Przeciwnik sunie w powietrzu parê metrów pchany impetem ciosu, a z jego pó³materialnego cia³a strzela jaskrawoczerwony strumieñ ¶wiat³a.{x", ch, wield, victim, TO_CHAR );
						act( "{5Obserwujesz jak $n przykuca na chwilê, odbija siê mocno od pod³o¿a i wykonuje wysoki, d³ugi skok w kierunku przeciwnika zamierzaj±c zaatakowaæ od góry. Z zastraszaj±c± prêdko¶ci± zbli¿a siê $e do $Z, wyci±ga przed siebie $h i zatacza krótki ³uk zamierzaj±c uderzyæ w g³owê. Jak na z³o¶æ orê¿ haniebnie mija wyznaczony cel i zahacza o niematerialny korpus. $N lawiruje w powietrzu parê metrów przez impet ciosu. Z wyrwy $S w klatce piersiowej strzela snop czerwonego ¶wiat³a.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Wydajesz z siebie og³uszaj±cy krzyk bitewny i nacierasz na $C najagresywniej, jak tylko mo¿esz. Ju¿ nawet planujesz kolejne kroki twego ataku, niestety ko¶ciotrup delikatnie schodzi ci z drogi i $p przecina tylko powietrze.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wydaje z siebie og³uszaj±cy, bitewny krzyk po czym zaciekle naciera na $C, ale $E widz±c to, schodzi z linii szar¿y, i $p przecina tylko powietrze.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Bierzesz g³êboki oddech i rozpoczynasz szar¿ê pêdz±c na $C z niesamowit± prêdko¶ci± i wymachuj±c w powietrzu $j. Bêd±c u celu wydajesz z siebie og³uszaj±cy okrzyk i wyprowadzasz seriê potê¿nych ciosów na czaszkê i korpus przeciwnika. Ostatecznie bierzesz potê¿ny zamach i wbijasz z wielk± si³± koniec $f w $S oczodó³, od³upuj±c czaszkê od reszty ko¶æca i koñcz±c tym samym zaciêt± walkê.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bierze g³êboki oddech i rozpoczyna potê¿n± szar¿ê pêdz±c na $C z niesamowit± prêdko¶ci± i wymachuj±c w powietrzu $f. Bêd±c u celu wydaje og³uszaj±cy okrzyk i wyprowadza seriê potê¿nych ciosów na czaszkê i korpus przeciwnika, wbijaj±c finalnie koniec $f w oczodó³, od³upuj±c czaszkê od reszty ko¶æca i koñcz±c ju¿ i tak nieco wyd³u¿ony ¿ywot $Z.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Dzier¿±c obur±cz $h, utkwiwszy wzrok w przeciwniku, bez najmniejszego zastanowienia rozpoczynasz potê¿n± szar¿ê. Biegn±æ wprost na $C, wymachujesz szaleñczo wokó³ g³owy sw± broni±. Bêd±c u celu bierzesz potê¿ny zamach i m³ócisz bez opanowania kul±cego siê z bólu przeciwnika wy³amuj±c jego ¿ebra.{x", ch, wield, victim, TO_CHAR );
						act( "{5Dostrzegasz, jak $n, dzier¿±c obur±cz $h, utkwiwszy wzrok w przeciwniku, bez najmniejszego zastanowienia rozpoczyna potê¿n± szar¿ê, wymachuj±c szaleñczo wokó³ g³owy sw± broni±. Zmusza swe miê¶nie do nadludzkiego wysi³ku i w koñcu bêd±c u celu bierze potê¿ny zamach i m³óci bez opanowania wij±cego siê z bólu przeciwnika wy³amuj±c jego ¿ebra.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5¦ciskaj±c w obu d³oniach $p i nie spuszczaj±c wzroku z $Z próbujesz wykonaæ potê¿ny atak. Biegniesz w stronê bezbronnego w tej sytuacji przeciwnika najszybciej jak tylko mo¿esz, a bêd±c ju¿ wystarczaj±co blisko, bierzesz têgi zamach i wyprowadzasz seriê mro¿±cych krew w ¿y³ach trza¶niêæ na korpus przeciwnika. Ka¿demu uderzeniu towarzyszy odg³os ³amanych ¿eber i nadkruszanych ko¶ci.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n, ¶ciskaj±c w obu d³oniach $h i nie spuszczaj±c wzroku z $Z próbuje wykonaæ potê¿ny atak. Biegnie szybko w stronê bezbronnego w tej sytuacji przeciwnika, a gdy znajduje siê ju¿ wystarczaj±co blisko umarlaka, bierze têgi zamach i wyprowadza seriê mro¿±cych krew w ¿y³ach trza¶niêæ skierowanych na korpus przeciwnika. Co chwila s³ychaæ nieprzyjemne zgrzyty i trzaski, a stos od³amanych ko¶ci spada z gruchotem na ziemiê.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(victim->form, FORM_CONSTRUCT))
				{
					if ( dam <= 0 )
					{
						act( "{5¦ciskaj±c $h w d³ugich susach zbli¿asz siê do przeciwnika. Z ka¿d± sekunda dystans miêdzy wami maleje. Chwytasz $h za jeden z koñców, poprawiasz chwyt i bierzesz potê¿ny zamach. Twoja broñ opada z przera¿aj±cym ¶wistem na ods³oniêty ³eb $Z. W momencie zderzenia orê¿a z celem do twych uszu dobiega og³uszaj±cy ³oskot, a przez twe ramiona przebiega dreszcz niezno¶nego bólu i odrêtwienia. Broñ odbija siê od twardej g³owy nie czyni±c przeciwnikowi ¿adnej szkody. Odskakujesz na bezpieczna odleg³o¶æ rozlu¼niaj±c ramiona.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n pêdzi szaleñczo na przeciwnika ¶ciskaj±c $h w rêkach. Odleg³o¶æ miêdzy nimi zmniejsza siê z sekundy na sekundê. $n dopadaj±c $Z bierze potê¿ny, szeroki zamach i t³ucze z ca³ych si³ w ods³oniêty ³eb wroga. Kiedy broñ styka siê z czerepem do twych uszu dobiega og³uszaj±cy trzask i zduszony jêk bólu, to $p odbija siê od twardego czo³a nie czyni±c $X najmniejszej krzywdy. $n wykrzywia twarz w cierpieniu i odskakuje parê metrów rozlu¼niaj±c obola³e ramiona.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Z rykiem w¶ciek³o¶ci rzucasz siê na przeciwnika. Unosisz wysoko w górê $h i bierzesz potê¿ny zamach. Orê¿ ze ¶wistem tnie powietrze i opada ³ukowatym torem wprost ku celowi. Czerwone p³aty ob³êdu lataj±ce ci przed oczami zas³aniaj± nieco widoczno¶æ lecz teraz nie ma to ju¿ znaczenia. Broñ opada z potworn± si³± na kark $Z krusz±c go w dono¶nym huku. Kaskady iskier i od³amków strzelaj± w powietrze na wiele metrów wokó³. Mocarne uderzenie posy³a oderwan± g³owê wysoko w powietrze. Czerep zatacza szeroki ³uk lawiruj±c powoli i opada z ³oskotem na pod³o¿e. Bezg³owy korpus ko³ysze siê jeszcze chwilê po czym opada z potwornym hukiem na ziemiê i rozpada siê.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z rykiem w¶ciek³o¶ci rzuca siê na $C unosz±c $h wysoko w powietrze. Bierze potê¿ny zamach i wyprowadza potworne walniêcie w ods³oniêty kark swej ofiary. Rozpêdzony orê¿ opada na cel krusz±c go w jednej sekundzie w dono¶nym huku pêkaj±cego cia³a. Kaskady ostrych od³amków strzelaj± w powietrze na wiele metrów wokó³. Oderwany czerep lawiruje chwilê zataczaj±c szeroki ³uk i l±duje z ³oskotem na ziemi. Bezg³owe cia³o chwieje siê kilka sekund, po czym dopada na pod³o¿e rozpadaj±c siê w u³amku sekundy.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Wyczekujesz odpowiedniego momentu i wyprowadzasz silne pchniêcie $f w ods³oniêty korpus $Z, orê¿ z trzaskiem t³ucze z potworn± si³± w $S bok. Z g³uchym trzaskiem spory kawa³ materia³u, z którego zbudowany jest przeciwnik, odrywa siê od reszty cia³a i wali z ³oskotem na ziemiê. W boku wroga zieje olbrzymia wyrwa. $N chwiej±c siê na nogach kontynuuje powolny, mechaniczny i nienaturalny pochód w twoim kierunku.{x", ch, wield, victim, TO_CHAR );
						act( "{5Nagle $n wydaj±c zduszony okrzyk wyprowadza potê¿ne pchniêcie w ods³oniêty korpus $Z, $s $h t³ucze z zastraszaj±c± si³± w bok ofiary, który kruszy siê z trzaskiem, a jego fragment odrywa siê od reszty cia³a i l±duje z ³oskotem na ziemi. W tu³owiu $Z widnieje teraz olbrzymia, postrzêpiona wyrwa.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Przekrêcasz cia³o raz w lewo raz w prawo i wyczekujesz odpowiedniego momentu na zadanie ciosu. W pewnej chwili odchylaj±c do ty³u sylwetkê zauwa¿asz, ¿e $N ods³ania siê na u³amek sekundy. Wykonujesz szeroki zamach $j i prostuj±c swe cia³o t³uczesz z ca³ych si³ w kolana wroga. Ten chwiej±c siê chwilê ledwo odzyskuje równowagê i kulej±c mocno zbli¿a siê w twym kierunku wymachuj±c w¶ciekle ramionami.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wygina swój korpus w ³uk i wykorzystuj±c lukê w obronie $Z bierze szeroki zamach i t³ucze $j w $S ods³oniête kolana. Ten chwieje siê chwilê, ale odzyskuje równowagê, i mocno kulej±c rusza w kierunku $z wymachuj±c w¶ciekle ramionami.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if ( dam <= 0 )
				{
					act( "{5Biegniesz ze prosto na $Z szaleñczo wymachuj±c $j, niestety broñ wypada ci z r±k i ledwo zd±¿asz j± podnie¶æ.{x", ch, wield, victim, TO_CHAR );
					act( "{5$n biegnie prosto na ciebie szaleñczo wymachuj±c $j, na szczê¶cie broñ wypada mu z r±k i ledwo zd±¿a j± podnie¶æ.{x", ch, wield, victim, TO_VICT );
					act( "{5$n biegnie ze sw± broni± prosto na $Z, jednak $p wypada z $s r±k i ledwo zd±¿a j± podnie¶æ.{x", ch, wield, victim, TO_NOTVICT );
					return TRUE;
				}
				else if ( victim->hit - dam < -11 )
				{
					int types = 1;

					if ( ch->class == CLASS_BARBARIAN && is_affected(ch, gsn_berserk ) )
						types = 2;

					switch ( number_range( 0, types ) )
					{
						case 0:
							if ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
							{
								act( "{5Spogl±dasz na $C i ju¿ wiesz, ¿e to $S koniec. Z furi± rzucasz siê na wroga, wal±c go swym $j prosto w skroñ. $S czaszka rozpada siê na kawa³ki, a twoj± twarz zalewa fala krwi i wyp³ywaj±cy mózg!{x", ch, wield, victim, TO_CHAR );
								act( "{5$n widz±c, ¿e nie czujesz siê najlepiej, z furi± rzuca siê na ciebie, wal±c z ogromn± si³± $j prosto w tw± skroñ. Przez chwilkê czujesz potworny ból, a potem nie czujesz ju¿ nic.{x", ch, wield, victim, TO_VICT );
								act( "{5Dostrzegasz z³owieszczy b³ysk w oku $z. Widzisz jak z furi± rzuca siê $e na $C, wal±c swym $j prosto w $S skroñ. Czaszka $Z rozpada siê na kawa³ki, a twarz zabójcy zalewa fala krwi i wyp³ywaj±cy mózg!{x", ch, wield, victim, TO_NOTVICT );
								make_brains( victim );
								break;
							}

						case 1:
							act( "{5Wrzeszcz±c g³o¶no rzucasz siê na $C celuj±c pod brodê. Twoje potê¿ne uderzenie urywa $S g³owê, która po kilku sekundach l±duje na ziemi!{x", ch, wield, victim, TO_CHAR );
							act( "{5$n wrzeszczy g³o¶no i biegnie prosto na ciebie, wal±c ciê pod brodê. Ból jest niewyobra¿alny, ale po chwili na szczê¶cie nie czujesz ju¿ nic.{x", ch, wield, victim, TO_VICT );
							act( "{5$n wrzeszcz±c g³o¶no rzuca siê na $C, i z olbrzymim impetem trafia pod $S brodê. G³owa $Z wylatuje wysoko w powietrze i upada na ziemiê kilka metrów obok!{x", ch, wield, victim, TO_NOTVICT );
							if ( IS_NPC( victim ) && victim->pIndexData->corpse_vnum < 0 )
								act( "{5Odr±bana g³owa $Z po chwili znika.{x", ch, wield, victim, TO_ROOM );
							else
								make_head( victim );
							break;

						case 2:
							act( "{5Ogarnia ciê sza³ i w zaskakuj±cym tempie zbli¿asz siê do $Z. W ostatniej chwili bierzesz potê¿ny zamach i zadajesz $j potê¿ny cios prosto w $S pier¶. $N pó³przytomny stoi przez chwilê, po czym osuwa siê na ziemiê.{x", ch, wield, victim, TO_CHAR );
							act( "{5$c ogarnia sza³ i w zaskakuj±cym tempie biegnie w twoim kierunku. W ostatniej chwili bierze potê¿ny zamach i wali ciê $j w klatkê piersiow±. Osuwasz siê pó³przytomny z bólu na ziemiê po czym tracisz ¶wiadomo¶æ...{x", ch, wield, victim, TO_VICT );
							act( "{5$c ogarnia sza³ i w zaskakuj±cym tempie zbli¿a siê do $Z. W ostatniej chwili bierze potê¿ny zamach i wali $j w $S pier¶. Przez chwile panuje cisza, po czym $N pada na ziemiê i umiera...{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				else
				{
					int types = 2;

					switch ( number_range( 0, types ) )
					{
						case 0:
							act( "{5Po wielkim rozpêdzie rzucasz siê na $C trzymaj±c wysoko nad g³ow± $h. Twoja broñ z wielk± si³± spada na $S ³eb. $N na chwilê traci ¶wiadomo¶æ i wykonuje jakie¶ dziwne, nieskoordynowane ruchy.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n po wielkim rozpêdzie rzuca siê na ciebie, trzymaj±c $h wysoko nad g³ow±. Wali ciê prosto w ³eb, przez chwilê nie wiesz gdzie jeste¶ i co siê z tob± dzieje, a ból wype³nia ca³e twe cia³o!{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n po wielkim rozpêdzie rzuca siê na $C, trzymaj±c $h wysoko nad g³ow±. Potê¿ne hukniêcie $z pozbawia $C ¶wiadomo¶ci, chwieje siê $E przez chwilê i wykonuje dziwne, nieskoordynowane ruchy.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 1:
							if ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
							{
								act( "{5Rozpêdzasz siê, trzymaj±c $h mocno w rêkach. Gdy jeste¶ ju¿ wystarczaj±co blisko $Z bierzesz szeroki zamach i grzmocisz bezlito¶nie jego szczêkê. Po chwili z zadowoleniem spogl±dasz jak $N spluwa krwi±.{x", ch, wield, victim, TO_CHAR );
								act( "{5Widzisz jak $n rozpêdza siê szaleñczo, trzymaj±c $h mocno w rêkach. Gdy jest juz blisko ciebie bierze szeroki zamach i wali ciê prosto w szczêkê!{x", ch, wield, victim, TO_VICT );
								act( "{5$n rozpêdza siê szaleñczo, trzymaj±c obur±cz $h. Bêd±c ju¿ blisko swojego celu bierze szeroki zamach i bezlito¶nie grzmoci szczêkê $Z, przez co $e soczy¶cie spluwa krwi±.{x", ch, wield, victim, TO_NOTVICT );
								make_blood( victim );
								break;
							}

						case 2:
							switch (victim->sex)
							{
								case 0:
									act( "{5Cofasz siê kilka metrów do ty³u, po czym biegniesz z $j w kierunku $Z. Nie wie $E co robiæ, a ty wykorzystujesz to i walisz $j pod $S ¿ebra. $N jest zamroczony, a z $S gêby wyp³ywa bia³a piana.{x", ch, wield, victim, TO_CHAR );
									break;
								case 1:
									act( "{5Cofasz siê kilka metrów do ty³u, po czym biegniesz z $j w kierunku $Z. Nie wie $E co robiæ, a ty wykorzystujesz to i walisz $j pod $S ¿ebra. $N jest zamroczona, a z $S gêby wyp³ywa bia³a piana.{x", ch, wield, victim, TO_CHAR );
									break;
								default:
									act( "{5Cofasz siê kilka metrów do ty³u, po czym biegniesz z $j w kierunku $Z. Nie wie $E co robiæ, a ty wykorzystujesz to i walisz $j pod $S ¿ebra. $N jest zamroczone, a z $S gêby wyp³ywa bia³a piana.{x", ch, wield, victim, TO_CHAR );
									break;
							}
							act( "{5Obserwujesz dziwne zachowanie $z, a $e nagle cofa siê kilka metrów, by po chwili ruszyæ w twoim kierunku. Nie wiesz co robiæ, a $n wykorzystuje to i wali ciê pod ¿ebra. Przez chwilê czujesz siê zamroczony, a z twoich ust wydostaje siê bia³a piana.{x", ch, wield, victim, TO_VICT );
							act( "{5$n cofa siê o kilka metrów, po czym biegnie wymachuj±c $j w kierunku $Z. $N jest zak³opotany, a $n wykorzystuje to i wali pod $S ¿ebra. Poszkodowany chwieje siê zamroczony, a z $S ust wydostaje siê bia³a piana.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				break;
			case WEAPON_FLAIL:
				if (!str_cmp( race_table[ GET_RACE(victim) ].name, "zombi" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "kryptowiec" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiorny rycerz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghul" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "ghast" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "mumia wiêksza" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "wampir" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Zbli¿aj±c siê do przeciwnika oceniasz zimno sytuacjê. Mierzysz minimalny dystans potrzebny do zadania ostatecznego ciosu. $N ostro¿nie sunie w twoj± stronê pow³ócz±c nogami. Nagle stajesz w lekkim wykroku i zapieraj±c siê silnie nogami o pod³o¿e unosisz w górê $h i opuszczasz z zastraszaj±c± si³± i prêdko¶ci±. O¿ywieniec unosi w górê ramiê w obronnym ge¶cie. Bijak ze ¶wistem mija uniesion± d³oñ, a brzêcz±ce cepigi owijaj± siê wokó³ górnej koñczyny nie czyni±c krzywdy przeciwnikowi. W ostatniej chwili odchylasz g³owê unikaj±c powracaj±cego w kierunku twojej twarzy bijaka. Szarpi±c za dzier¿ak uwalniasz $h jednym ruchem.{x", ch, wield, victim, TO_CHAR );
						act( "{5Obserwujesz jak $n zimno kalkuluje szansê zadania ostatecznego ciosu. W pewnym momencie zapiera siê silnie nogami o pod³o¿e, unosi w górê $h i opuszcza z zastraszaj±c± si³±. Rozpêdzony bijak tn±c powietrze wêdruje z prêdko¶ci± wichru w kierunku $Z. O¿ywieniec unosi jednak przegni³e ramiê w obronnym ge¶cie. Brzêcz±ce cepigi owijaj± siê niegro¼nie o rozk³adaj±c± siê koñczynê nie czyni±c nieumar³emu ¿adnej krzywdy. $n uchyla siê b³yskawicznie przed rozpêdzonym bijakiem wracaj±cym w kierunku $s twarzy, oraz jednym szarpniêciem uwalnia sw± broñ.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Zaciskaj±c zêby z w¶ciek³o¶ci, warcz±c dziko z ob³±kañczym b³yskiem w zwê¿onych oczach biegniesz w d³ugich susach w kierunku przeciwnika z unosz±c wysoko $h. W kilka chwil dopadasz $Z i zadajesz morderczy cios. Bijak opada z zastraszaj±c± si³a i prêdko¶ci± trafiaj±c w skroñ o¿ywieñca. Mia¿d¿ona, po¿ó³k³a ko¶æ zapada siê z dono¶nym chrzêstem, uwalniaj±c ga³kê oczn± i rozbryzguj±cy siê wszêdzie mózg. Bijak sunie dalej mia¿d¿±c przegni³y policzek, nos i szczêkê nieumar³ego. Potworny impet walniêcia odrzuca $Z kilka metrów w ty³. Przeciwnik chwieje siê zamroczony przez chwilê. Nie trac±c ani chwili poprawiaj±c chwyt na dzier¿aku t³uczesz w klatkê piersiow± o¿ywieñca mia¿d¿±c j± w huku pêkaj±cych ko¶ci. Z przed¶miertnym skrzekiem na ustach nieumar³y wali siê z ³oskotem na pod³o¿e i nieruchomieje ca³kowicie.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n warcz±c w¶ciekle z b³yskiem ob³±kania w zwê¿onych oczach naciera na przeciwnika, $s uniesiony orê¿ tañczy w¶ciekle w powietrzu. $n w kilku d³ugich skokach dopada $Z i z dzik± si³± swych potê¿nych ramion t³ucze bijakiem w g³owê przeciwnika. Broñ z paskudnym chrzêstem opada na skroñ uwalniaj±c ga³kê oczn± i bryzgaj±cy na wszystko mózg nieumar³ego. Mia¿d¿y przy tym lew± po³owê gnij±cej twarzy o¿ywieñca. Impet potê¿nego uderzenia odrzuca przeciwnika na kilka metrów. Poprawiaj±c chwyt na dzier¿aku $n t³ucze z ca³ych si³ w klatkê piersiow± wroga mia¿d¿±c j± z dono¶nym chrupotem. $N z przed¶miertnym skrzekiem wali siê na pod³o¿e i nieruchomieje.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5$N kieruje siê w twoj± stronê niezdarnie gubi±c przy tym cuchn±ce zgnilizn± p³aty miêsa. ¦ciskaj±c mocno dier¿ak $f bierzesz szeroki zamach i wyprowadzasz potê¿ne walniêcie na korpus przeciwnika. Rozpêdzony bijak z chrzêstem mia¿d¿onych ko¶ci oraz mlaskiem odrywanego cia³a opada na klatkê piersiow± przeciwnika. Z rozwartych, zniekszta³conych ust o¿ywieñca wydobywa siê pe³en bólu skrzek i wylewa siê rzeka gêstej, cuchn±cej ¿ó³tawej juchy.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $N niezdarnie sunie pow³ócz±c zgni³ymi nogami w kierunku $z, $e zaciska mocno d³onie na dzier¿aku $f, bierze szeroki zamach i wyprowadza potê¿ne walniêcie na korpus przeciwnika. Rozpêdzony bijak w mgnieniu oka z paskudnym odg³osem mia¿d¿onych ko¶ci i dono¶nym mla¶niêciem rozrywanego miêsa oraz rwanych ¿y³ dosiêga klatki piersiowej. Z wykrzywionych, zniekszta³conych ust o¿ywieñca wydobywa siê pe³en bólu skrzek oraz bucha fontanna gêstej, cuchn±cej ¿ó³tawej juchy.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Nacierasz zdecydowanie na przeciwnika. Unosisz $h w szerokim zamachu i opuszczasz z zastraszaj±c± si³±, kieruj±c bijak na g³owê $Z. O¿ywieniec niezdarnie uchyla g³owê unikaj±c potê¿nego walniêcia, bijak jednak opada z chrzêstem na $S bark. Do twych uszu dociera st³umiony przez gnij±ce miêso, suchy trzask pêkaj±cych ko¶ci. Ramiê przeciwnika osuwa siê nienaturalnie kilkana¶cie centymetrów w dó³ zwisaj±c bezw³adnie. Ze skrzekiem bólu i w¶ciek³o¶ci $N rzuca siê na ciebie atakuj±c ze zdwojon± si³a.{x", ch, wield, victim, TO_CHAR );
						act( "{5Podziwiasz z zapartym tchem potê¿ne natarcie $z. Unosi $e swój orê¿ wysoko w górê i opuszcza go ze ¶wistem kieruj±c bijak na czaszkê $Z. O¿ywieniec jednak przekrzywia g³owê unikaj±c morderczego walniêcia, lecz $p opada z suchym trzaskiem mia¿d¿onych ko¶ci st³umionym nieco przez gnij±ce miêso na $S bark. Ramiê przeciwnika osuwa siê kilkana¶cie centymetrów w dó³ i zwisa bezw³adnie wzd³u¿ rozk³adaj±cego siê korpusu. Z gard³a $Z wydobywa siê dono¶ny skrzek bólu i w¶ciek³o¶ci. Rzuca siê w furii na $c i atakuje ze zdwojon± si³± i precyzj±.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "duch" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "banshee" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "cieñ" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "widmo" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "zmora" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "upiór" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Dostrzegaj±c przed sob± ledwo widoczn± sylwetkê niematerialnego cia³a $Z, zaczynasz wymachiwaæ szaleñczo swym potê¿nym $j. Ju¿ prawie dostrzegasz jak wielka kula przebija niematerialne cia³o, jak ledwo widoczna dusza rozrywana jest na strzêpy w potê¿nym blasku czerwonej po¶wiaty... Nagle jednak staje siê co¶ nieoczekiwanego! Twój cel momentalnie znika, a za plecami czujesz podmuch mro¼nego wiatru. Trac±c równowagê walisz siê na ziemiê, lecz przera¿ony mo¿liwymi skutkami tego b³êdu b³yskawicznie d¼wigasz siê na nogi i nieco ju¿ roztropniej kontynuujesz walkê.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz, jak $n dostrzegaj±c przed sob± ledwo widoczn± sylwetkê $Z, zaczyna wymachiwaæ szaleñczo $f. Po chwili, bêd±c u celu, stara siê wykonaæ potê¿ny atak, lecz w jednej chwili staje siê co¶ wprost niesamowitego! $N momentalnie znika i pojawia siê tu¿ za plecami $z, a $e z kolei trac±c równowagê wali siê na ziemie jak bela, jednak¿e szybko d¼wiga siê na nogi i kontynuuje zaciêt± walkê.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Wymachuj±c $j bacznym wzrokiem ¶ledzisz tor ruchu potê¿nej, nabijanej kolcami kuli, lawiruj±cej wokó³ twej g³owy. Ostatecznie, czuj±c przyp³yw niesamowitej energii bierzesz potê¿ny zamach i wyprowadzasz silny, skierowany w stronê $Z atak. Kula zostawia w niematerialnym ciele wielk±, promieniuj±c± czerwonawym strumieniem pulsuj±cej negatywnej enrgii wyrwê, a w twej g³owie rozbrzmiewa potêpieñczy wrzask bólu. Otwór w ciele zaczyna siê stopniowo powiêkszaæ, wydaj±c siê zjadaæ $C od ¶rodka. W koñcu nie zostaje ani ¶ladu, prócz odbijaj±cego siê jeszcze echem w twej g³owie, pe³nego bólu krzyku.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz, jak wielka kula, lawiruj±ca wokó³ g³owy $z nabiera prêdko¶ci i finalnie spada na niematerialne cia³o $Z, pozostawiaj±c w nim wielk±, promieniuj±c± czerwonawym strumieniem pulsuj±cej negatywnej energii wyrwê, a twoje cia³o przeszywa dziwne uczucie bólu. Otwór z kolei zaczyna siê stopniowo powiêkszaæ, wydaj±c siê zjadaæ $C od ¶rodka, a¿ ostatecznie nie zostaje ani ¶ladu.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Z dono¶nym krzykiem na ustach dobiegasz do $Z i potê¿nie wymachuj±c $j dokonujesz potê¿nego ciosu! Ciê¿ka, ¿elazna kula opadaj±c z wielk± prêdko¶ci± przebija siê przez niematerialne cia³o niczym przez mas³o. Nagle odczuwasz, jakby wewn±trz twej g³owy co¶ potê¿nie eksplodowa³o, jakby zagra³y w niej naraz tysi±ce dzwonów. Przera¿ony odbiegasz jak najdalej od krwawi±cego czerwonym blaskiem celu i nacierasz ponownie.{x", ch, wield, victim, TO_CHAR );
						act( "{5Z dono¶nym okrzykiem na ustach $n dobiega do $Z i potê¿nie wymachuj±c $j dokonuje potê¿nego ciosu! Ciê¿ka, ¿elazna kula opadaj±c z wielk± prêdko¶ci± przebija siê przez niematerialne cia³o niczym przez mas³o. Okoliczne tereny rozdziera nie tyle s³yszalny, co odczuwalny krzyk bólu, a zaniepokojony tym $n odbiega na bezpieczn± odleg³o¶æ i naciera ponownie.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5¦ciskasz obur±cz jeszcze mocniej $h i rozpoczynasz szaleñczy bieg. Niestety, w pewnym momencie okazuje siê, i¿ ¼le oceni³e¶ odleg³o¶æ i najprawdopodobniej zderzysz siê ze swym celem. Mimo to, wykonujesz potê¿ny zamach, a $h spada na wroga tworz±c wyrwê w $S ciele, z której z sykiem wylatuje strumieñ czerwonej, negatywnej energii.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n znajduje dogodny moment do wyprowadzenia potê¿nej szar¿y i rozpoczyna szaleñczy bieg w stronê ods³oniêtego przeciwnika. Niestety ¼le ocenia odleg³o¶æ i najprawdopodobniej lada moment, nie mog±æ wyhamowaæ, zderzy siê z $V. Mimo to wykonuje z zapa³em potê¿ny zamach, a jego $p spada na wroga tworz±c wyrwê w jego ciele, z której z sykiem wylatuje strumieñ czerwonej, negatywnej enrgii.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "drakolicz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "licz" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet wojownik" )||
						!str_cmp( race_table[ GET_RACE(victim) ].name, "szkielet" ))
				{
					if ( dam <= 0 )
					{
						act( "{5Wci±gasz powietrze po czym rozpoczynasz szaleñczy bieg w kierunku $Z. Bijak $f wiruje z³owieszczo, a ty wyprowadzasz potê¿ne uderzenie w kierunku nieumar³ego, jednak jakims cudem ten dojrza³ ciê i uchyli³ siê przed ciosem, a ty si³± rozpêdu zatrzyma³<&e¶/a¶/e¶> siê dopiero po chwili.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n wci±ga powietrze po czym rozpoczyna szaleñczy bieg w kierunku $Z. Bijak $s $f wiruje w powietrzu z³owieszczo gdy wyprowadza potê¿ny cios w kierunku o¿ywienca, jednak $N jakim¶ cudem zauwa¿y³ go i sprytnie siê przed nim uchyli³. {x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Ze w¶ciek³ym rykiem wyprowadzasz morderczy cios skierowany w po³yskuj±c± czaszkê przeciwnika. Ten jednak nag³ym zrywem odchyla siê lekko do ty³u. Rozpêdzony bijak $f ze ¶wistem opada na miednicê. Si³a ciosu jest tak wielka, ¿e rozpada siê ona w mgnieniu oka z g³uchym odg³osem na tysi±ce drobnych kawa³ków oddzielaj±c zmursza³e nogi $Z od korpusu. Chmura wiekowego, stêch³ego py³y zas³ania ci widoczno¶æ. Jak przez mg³ê spogl±dasz na opadaj±cy, wymachuj±cy w powietrzu rêkoma korpus przeciwnika. W jednej sekundzie unosisz broñ z morderczym b³yskiem w oku i opuszczasz j± raz za razem wgniataj±c o¿ywieñca w pod³o¿e. Ze $Z nie pozostaje nic oprócz porozrzucanych wszêdzie kostnych drobin i tañcz±cej w powietrzu chmury szarawego py³u. Ocieraj±c pot z czo³a warczysz w¶ciekle rozgl±daj±c siê po okolicy.{x", ch, wield, victim, TO_CHAR );
						act( "{5Do twych uszu dociera potworny ryk $Z, szar¿uje $e w¶ciekle na przeciwnika. Potê¿ne walniêcie z zastraszaj±c± prêdko¶ci± opada na ods³oniêt±, po³yskuj±c± czaszkê $Z, jednak o¿ywieniec jest zbyt czujny i odchyla siê b³yskawicznie do ty³u, lecz rozpêdzony bijak opada na zmursza³± miednicê nieumar³ego, która z g³uchym, pustym ³oskotem rozpada siê na tysi±ce drobin kostnych oddzielaj±c dolne koñczyny od korpusu szkieletu. Wzbita przy tym chmura wiekowego py³u zas³ania widoczno¶æ. Kiedy opada widzisz sylwetkê $z t³uk±c± bez opamiêtania w le¿±cy korpus przeciwnika. Kiedy milknie odg³os ostatniego uderzenia po $X zostaje jedynie walaj±ca siê wszêdzie kupka od³amków kostnych.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Chwytasz mocniej $h i z bojowym rykiem rzucasz siê na $C. W dwóch krokach ju¿ stoisz u celu i twoja niszczycielska broñ leci w kierunku przeciwnika, który chyba nie wie co go czeka. Potê¿ne uderzenie odrzuca $C w ty³, krusz±c ¿ebra i miednice o¿ywienca. Szybko odskakujesz na bezpieczn± odleg³o¶æ.{x", ch, wield, victim, TO_CHAR );
						act( "{5Widzisz jak $n mocniej chwyta $h, i z bojowym rykiem rzuca siê g³êbiej w wir walki. Potê¿ny bijak $s broni zbliza siê z olbrzymi± prêdkosci± do koscianego korpusu i uderza go z niszczycielsk± si³±. $N odlatuje w ty³, a drobne kawa³ki od³amanych kosci i py³u unosz± siê jeszcze przez chwilê. $n odskakuje na bezpieczn± odleg³o¶æ.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Z g³o¶nym okrzykiem rzucasz siê na $C, wznosz±c $h wysoko i bior±c zamach. Potê¿ne narzêdzie zniszczenia okreca siê kilka razy woko³ twojej g³owy po czym uderza w cel. S³ychaæ trzask pêkanych ko¶ci, zgrzyt uderzenia o tward± powieszchnie po czym zatrzymujesz siê kilka metrów za szkieletem.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n z g³o¶nym okrzykiem rzuca siê na $C, wznos±c wysoko $h i bior±c zamach. Kula na ³añcuchu wiruje przez chwile w powietrzu po czym prowadzona pewn± rêk± trafia cel prosto w korpus. S³ychac trzask pêkanych kosci lecz $n zatrzymuje siê dopiero kilka kroków za $V.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if (IS_SET(victim->form, FORM_CONSTRUCT))
				{
					if ( dam <= 0 )
					{
						act( "{5¶ciskaj±c obur±cz masywny orê¿, rozpoczynasz szaleñczy bieg. Z bojowym okrzykiem, rzucasz siê zapalczywie na $C, $E jednak, przejrzawszy na wskro¶ twe niecne zamiary, robi zwinny unik, wysy³aj±c ciê na ziemiê. Odruchowo, podnosisz siê b³yskawicznie i kontynuujesz nieco rozs±dniej walkê.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n, ¶ciskaj±c obur±cz $h, rozpoczyna szaleñczy bieg. Z bojowym okrzykiem, rzuca siê zapalczywie na $C, $E jednak robi zwinny unik, wysy³aj±c niedosz³ego zwyciêzcê na ziemiê. $n nie trac±c na czasie podnosi siê i kontynuuje ju¿ nieco rozs±dniej walkê.{x", ch, wield, victim, TO_NOTVICT );
						return TRUE;
					}
					else if ( victim->hit - dam < -11 )
					{
						act( "{5Spogl±daj±c nienawistnie na rysuj±c± siê nieopodal masywn± sylwetkê $Z, zaciskasz mocniej d³onie na trzonku $f i wyprowadzasz potê¿ny atak. Rozpêdzona do granic mo¿liwo¶ci ogromna kula spada z hukiem na korpus przeciwnika, przebijaj±c siê przez twardy, skorupiasty korpus. Wokó³ rozbrzmiewa dono¶ne echo wal±cej siê na ziemiê, potrzaskanej sylwetki $Z.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n spogl±daj±c nienawistnie na rysuj±c± siê nieopodal masywn± sylwetkê $Z, zaciska mocniej d³onie na trzonku $f i wyprowadza potê¿ny atak. Rozpêdzona do granic mo¿liwo¶ci ogromna kula spada z hukiem na korpus przeciwnika przebijaj±c siê przez twardy, skorupiasty korpus. Wszêdzie rozbrzmiewa g³uchy odg³os wal±cej siê na ziemiê istoty.{x", ch, wield, victim, TO_NOTVICT );
					}
					else if (number_range(1,2) == 2 )
					{
						act( "{5Koncentrujesz siê najbardziej jak tylko mo¿esz na $B i bez najmniejszego zastanowienia wyprowadzasz w jego stronê potê¿ny atak wymachuj±c szaleñczo $j. W koñcu bierzesz zamach, a ogromna, stalowa kula spada z impetem na korpus $Z, od³upuj±c przy okazji wielkie kawa³y budulca.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n bez najmniejszego zastanowienia wyprowadza w stronê $Z potê¿ny atak. Z bojowym zapa³em naciera na przeciwnika, wymachuj±c szaleñczo $j. W koñcu bierze zamach, a ogromna, stalowa kula spada z impetem na korpus $Z, odtrzaskuj±c parê poka¼nej wielko¶ci bry³, które spadaj±c na ziemiê, d¼wiêcznie postukuj±.{x", ch, wield, victim, TO_NOTVICT );
					}
					else
					{
						act( "{5Dzier¿±c obur±cz $p, rozpoczynasz szaleñczy bieg w stronê $Z i wymachujesz zapalczywie swym orê¿em, jakby¶ w ten sposób chcia³ odgoniæ jakie¶ natrêtne owady. Gdy dobiegasz do niczego nie spodziewaj±cego siê przeciwnika bierzesz potê¿ny zamach i zapalczywie ³upiesz w jego masywny bark. Kula spada nañ z wielkim hukiem, odrywaj±c przy okazji parê warstw dziwnego materia³u.{x", ch, wield, victim, TO_CHAR );
						act( "{5$n dzier¿±c obur±cz $h, rozpoczyna szaleñczy bieg w stronê $Z. Nabieraj±c na prêdko¶ci, wymachuje zapalczywie swym orê¿em, a gdy dobiega do $Z bierze potê¿ny zamach i zapalczywie ³upie w $S masywny bark. Kula spada nañ z wielkim hukiem, uszkadzaj±c powa¿nie czê¶æ ramienia, a unieruchomiona rêka zwisa pod bardzo dziwnym k±tem.{x", ch, wield, victim, TO_NOTVICT );
					}
					return TRUE;
				}
				else if ( dam <= 0 )
				{
					act( "{5Biegniesz ze prosto na $Z szaleñczo krêc±c w powietrzu $j, niestety potykasz siê i kolczasta kula na ³añcuchu opl±tuje siê wokó³ ciebie harataj±c twe cia³o.{x", ch, wield, victim, TO_CHAR );
					act( "{5$n biegnie prosto na ciebie szaleñczo krêc±c w powietrzu $j, na szczê¶cie ³amaga potyka siê i kolczasta kula na ³añcuchu opl±tuje siê wokó³ $s cia³a harataj±c je.{x", ch, wield, victim, TO_VICT );
					act( "{5$n biegnie ze prosto na $Z szaleñczo krêc±c w powietrzu $j, jednak ³amaga potyka siê i kolczasta kula na ³añcuchu opl±tuje siê wokó³ $s cia³a harataj±c je.{x", ch, wield, victim, TO_NOTVICT );
					ch->hit = ch->hit - ( dice( wield->value[ 1 ], wield->value[ 2 ]) + wield->value[ 6 ] );
					update_pos( ch );
					return TRUE;
				}
				else if ( victim->hit - dam < -11 )
				{
					int types = 1;

					if ( ch->class == CLASS_BARBARIAN && is_affected(ch, gsn_berserk ) )
						types = 2;

					switch ( number_range( 0, types ) )
					{
						case 0:
							if ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) )
							{
								act( "{5Spogl±dasz na $C i ju¿ wiesz, ¿e to $S koniec. Z furi± rzucasz siê na wroga, wal±c go kolczast± kul± swego $f prosto w skroñ. $S czaszka rozpada siê na kawa³ki, a twoj± twarz zalewa fala krwi!{x", ch, wield, victim, TO_CHAR );
								act( "{5$n widz±c, ¿e nie czujesz siê najlepiej, z furi± rzuca siê na ciebie, wal±c z ogromn± si³± kolczast± kul± swego $f prosto w tw± skroñ. Przez chwilkê czujesz potworny ból, a potem nie czujesz ju¿ nic.{x", ch, wield, victim, TO_VICT );
								act( "{5Dostrzegasz z³owieszczy b³ysk w oku $z. Widzisz jak z furi± rzuca siê $e na $C, wal±c kolczast± kul± swego $f prosto w $S skroñ. Czaszka $Z rozpada siê na kawa³ki, a twarz zabójcy zalewa fala krwi!{x", ch, wield, victim, TO_NOTVICT );
								make_blood( victim );
								break;
							}
						case 1:
							act( "{5Wrzeszcz±c g³o¶no rzucasz siê na $C krêc±c w¶ciekle kolczast± kul± swego $f celuj±c w $S bok. Twoje potê¿ne uderzenie ca³kowicie mia¿d¿y miednicê $Z, a temu zdarzeniu towarzyszy g³o¶ny pisk ofiary i dono¶ny gruchot pêkaj±cych ko¶ci. $N wykrzywia siê nienaturalnie i upada na ziemiê.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n wrzeszczy g³o¶no rzuca siê na ciebie krêc±c w¶ciekle kolczast± kul± swego $f. Cios by³ tak szybki, ¿e nawet go nie zauwa¿y³<&e¶/a¶/o¶>, przez chwilkê czujesz tylko dziwny strzyk w boku. Nagle przestajesz czuæ nogi. Ostatnimi si³y zauwa¿asz jak ¶wiat krêci siê dooko³a, pod³oga pojawia siê tu¿ nad tob±, a ty na ni± spadasz...{x", ch, wield, victim, TO_VICT );
							act( "{5$n wrzeszcz±c g³o¶no rzuca siê na siê na $C krêc±c w¶ciekle kolczast± kul± swego $f celuj±c w $S bok. $s potê¿ne uderzenie ca³kowicie mia¿d¿y miednicê $Z, a temu zdarzeniu towarzyszy g³o¶ny pisk ofiary i dono¶ny gruchot pêkaj±cych ko¶ci. $N wykrzywia siê nienaturalnie i upada na ziemiê.{x", ch, wield, victim, TO_NOTVICT );
							break;
						case 2:
							act( "{5Ogarnia ciê sza³ i w zaskakuj±cym tempie zbli¿asz siê do $Z. W ostatniej chwili bierzesz potê¿ny zamach i zadajesz $j potê¿ny cios prosto w $S pier¶. $N pó³przytomny stoi przez chwilê, po czym osuwa siê na ziemiê.{x", ch, wield, victim, TO_CHAR );
							act( "{5$c ogarnia sza³ i w zaskakuj±cym tempie biegnie w twoim kierunku. W ostatniej chwili bierze potê¿ny zamach i wali ciê $j w klatkê piersiow±. Osuwasz siê pó³przytomny z bólu na ziemiê po czym tracisz ¶wiadomo¶æ...{x", ch, wield, victim, TO_VICT );
							act( "{5$c ogarnia sza³ i w zaskakuj±cym tempie zbli¿a siê do $Z. W ostatniej chwili bierze potê¿ny zamach i wali $j w $S pier¶. Przez chwile panuje cisza, po czym $N pada na ziemiê i umiera.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				else
				{
					int types = 0;
					int types2 = 1;
					if ( ch->class == CLASS_BARBARIAN && is_affected(ch, gsn_berserk ) )
					{
						types = 2;
						types2 = 2;
					}
					switch ( number_range( types, types2 ) )
					{
						case 0:
							act( "{5Po wielkim rozpêdzie rzucasz siê na $C trzymaj±c wysoko nad g³ow± $h. Kolczasta kula twej broni z wielk± si³± spada na $S ³eb. $N na chwilê traci ¶wiadomo¶æ i wykonuje jakie¶ dziwne, nieskoordynowane ruchy.{x", ch, wield, victim, TO_CHAR );
							act( "{5$n po wielkim rozpêdzie rzuca siê na ciebie, trzymaj±c $h wysoko nad g³ow±. Wielka kolczasta kula spada z ogromn± si³± na tw± g³owê, przez chwilê nie wiesz gdzie jeste¶ i co siê z tob± dzieje, a ból wype³nia ca³e twe cia³o!{x", ch, wield, victim, TO_VICT );
							act( "{5Widzisz jak $n po wielkim rozpêdzie rzuca siê na $C, trzymaj±c $h wysoko nad g³ow±. Potê¿ne hukniêcie $z pozbawia $C ¶wiadomo¶ci, chwieje siê $E przez chwilê i wykonuje dziwne, nieskoordynowane ruchy.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 1:
							act( "{5Rozpêdzasz siê, trzymaj±c $h mocno w rêkach. Gdy jeste¶ ju¿ wystarczaj±co blisko $Z bierzesz szeroki zamach i grzmocisz bezlito¶nie jego szczêkê. Po chwili z zadowoleniem spogl±dasz jak $N spluwa krwi±.{x", ch, wield, victim, TO_CHAR );
							act( "{5Widzisz jak $n rozpêdza siê szaleñczo, trzymaj±c $h mocno w rêkach. Gdy jest juz blisko ciebie bierze szeroki zamach i wali ciê prosto w szczêkê!{x", ch, wield, victim, TO_VICT );
							act( "{5$n rozpêdza siê szaleñczo, trzymaj±c obur±cz $h. Bêd±c ju¿ blisko swojego celu bierze szeroki zamach i bezlito¶nie grzmoci szczêkê $Z, przez co $e soczy¶cie spluwa krwi±.{x", ch, wield, victim, TO_NOTVICT );
							break;

						case 2:
							act( "{5Wype³niaj±ca ciê furia eksploduje i lekko cofaj±c ciê bierzesz naprawdê olbrzymi zamach i z ca³ej si³y, z lekkiego doskoku, walisz kolczast± kul± swego $f prosto w pier¶ $Z gruchocz±c $S ¿ebra! $N g³o¶no wzdycha i jêcz±c wypuszcza powietrze z p³uc.{x", ch, wield, victim, TO_CHAR );
							act( "{5Nagle dostrzegasz b³ysk furii w oczach $z. Odsuwa siê $e lekko od ciebie i z lekkim doskokiem oraz rykiem na ustach bierze naprawdê olbrzymi zamach i wali ciê kolczast± kul± swego $f prosto w pier¶ gruchocz±c twe ¿ebra i przyprawiaj±c straszny ból!{x", ch, wield, victim, TO_VICT );
							act( "{5Nagle dostrzegasz b³ysk furii w oczach $z. Odsuwa siê $e lekko od $Z i bior±c naprawdê olbrzymi zamach doskakuje do swej ofiary i z rykiem na ustach mia¿dzy jej ¿ebra kolczast± kul± swego $f. $N g³o¶no wzdycha i jêcz±c wypuszcza powietrze z p³uc.{x", ch, wield, victim, TO_NOTVICT );
							break;
					}
					return TRUE;
				}
				break;
		}
	}
	else if ( dt == gsn_stun )
	{
		if ( immune )
		{
			act( "{5Twój cios l±duje na czole $Z, ale $E nawet nie zwraca na to uwagi.{x", ch, wield, victim, TO_CHAR );
			act( "{5Cios $z l±duje ci na czole, jednak nawet nie zwracasz na to uwagi.{x", ch, wield, victim, TO_VICT );
			act( "{5Cios $z l±duje na czole $Z, ale $E nawet nie zwraca na to uwagi.{x", ch, NULL, victim, TO_NOTVICT );
		}
		else if ( dam > 0 )
		{
			act( "{5Twój cios l±duje na czole $Z któr$R przez chwilê chwieje siê og³uszon$t.{x", ch, victim->sex == 2 ? "a" : victim->sex == 0 ? "e" : "y", victim, TO_CHAR );
			act( "{5Cios $z l±duje ci na czole. Przez chwilê nie widzisz niczego oprócz gwiazd.{x", ch, wield, victim, TO_VICT );
			act( "{5Cios $z l±duje na czole $Z, któr$R przez chwilê chwieje siê og³uszon$t.{x", ch, victim->sex == 2 ? "a" : victim->sex == 0 ? "e" : "y", victim, TO_NOTVICT );
		}
		else if ( dam <= 0 )
		{
			act( "{5Twój cios o w³os mija g³owê $Z.{x", ch, wield, victim, TO_CHAR );
			act( "{5Cios $z ze ¶wistem przelatuje ko³o twojej g³owy.{x", ch, wield, victim, TO_VICT );
			act( "{5Cios $z o w³os mija g³owê $Z.{x", ch, NULL, victim, TO_NOTVICT );
		}
		return TRUE;
	}
	else if ( dt == gsn_crush )
	{
		if ( immune )
		{
			act( "{5Twój cios jest nieskuteczny, $X nic siê nie sta³o.{x", ch, wield, victim, TO_CHAR );
			act( "{5Cios $z jest nieskuteczny, nic ci siê nie sta³o{x", ch, wield, victim, TO_VICT );
			act( "{5Potê¿ny cios $z jest nieskuteczny, $X nic siê nie sta³o.{x", ch, NULL, victim, TO_NOTVICT );
		}
		return TRUE;
	}
	if ( dt == gsn_tail )
	{
		if ( immune )
		{
			act( "{5Robisz szybki zamach ogonem w kierunku $Z i uderzasz $M najmocniej jak potrafisz, jednak $E nawet nie drgnie.{x", ch, NULL, victim, TO_CHAR );
			act( "{5$n robi szybki zamach swoim ogonem i uderza ciê nim najmocniej jak mo¿e, jednak nawet tego nie zauwa¿asz.{x", ch, NULL, victim, TO_VICT );
			act( "{5$n robi szybki zamach ogonem w kierunku $Z i uderza go najmocniej jak potrafi, jednak $E nawet tego nie zauwa¿a.{x", ch, NULL, victim, TO_NOTVICT );
			return TRUE;
		}
		else if ( dam <= 0 )
		{
			switch ( number_range( 0, 3 ) )
			{
				case 0:
					act( "{5Bierzesz zamach ogonem i próbujesz nim trafiæ $C, ale chybiasz $M z kretesem.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n wykonuje jakie¶ dziwne ruchy, jakby próbowa³ uderzyæ ciê ogonem, jednak chybia z kretesem.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n robi szybki zamach swoim ogonem, jednak chybia $C z kretesem.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 1:
					act( "{5Robisz zamach swoim ogonem, jednak $N wykonuje nag³y unik i twój cios chybia.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n robi zamach swoim ogonem, jednak szybki unik wystarcza, aby silny, acz niedok³adny cios omin±³ ciê o parê centymetrów.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n wykonuje zwinny zamach swoim ogonem, szybki unik jednak wystarcza, aby silny, acz niedok³adny cios chybi³ $Z.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 2:
					act( "{5Próbujesz uderzyæ ogonem $C, ale tracisz na sekundê równowagê i chybiasz haniebnie.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n próbuje ciê uderzyæ swoim umiê¶nionym ogonem, ale podczas zamachu traci na sekundê równowagê i chybia haniebnie.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n próbuje uderzyæ swoim ogromnym ogonem $C, ale podczas zamachu traci na sekundê równowagê i chybia haniebnie.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 3:
					act( "{5Próbuj±c uderzyæ swoim ogonem $C prawie $t obrót o pó³ osi.{x", ch, ch->sex == 2 ? "zrobi³a¶" : "zrobi³e¶", victim, TO_CHAR );
					act( "{5$n wykonuje jakie¶ podejrzane ruchy, bierze tak têgi zamach swoim ogonem, ¿e kiedy nie trafia obraca siê do ciebie ty³em.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n wykonuje jakie¶ podejrzane ruchy, bierze tak têgi zamach swoim ogonem, ¿e kiedy nie trafia obraca siê do $Z ty³em.{x", ch, NULL, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
		else if ( victim->hit - dam < -11 )
		{
			switch ( number_range( 0, 2 ) )
			{
				case 0:
					act( "{5Przymierzasz siê do zamachu swoim ogonem, wyrzucasz go gwa³townie do przodu, lekko siê obracaj±c, po czym uderzasz $C. Niesamowicie potê¿ny cios sprawia, ¿e $N pada na ziemiê, pluj±æ krwi± rzêzi chwilê po czym umiera w niewypowiedzianym bólu.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n przymierza siê przez chwilê do zamachu swoim ogonem, wyrzuca go gwa³townie do przodu i w powietrzu jakby $t zamiar, bowiem uderza ciê prosto w g³owê. Widzisz jak oczy zachodz± ci czarno-czerwon±, krwaw± mg³±. Powoli opadasz bez si³ na kolana, nastêpnie na plecy. Ostatkiem si³ próbujesz zaczerpn±c odrobinê powietrza, jednak cia³o ju¿ nie s³uga mó¿gu. Czujesz jak odp³ywasz... Umierasz..{x", ch, ch->sex == 2 ? "zmieni³a" : "zmieni³", victim, TO_VICT );
					act( "{5$n przymierza siê przez chwilê do zamachu swoim ogonem, wyrzuca go gwa³townie do przodu i w powietrzu jakby $t zamiar, bowiem uderza $C prosto miêdzy oczy. Potê¿ny cios sprawia, ¿e $N pada na kolana, a nastêpnie osuwa siê na ziemiê. Rzêzi jeszcze przez chwilê i umiera w niewypowiedzianym bólu.{x", ch, ch->sex == 2 ? "zmieni³a" : "zmieni³", victim, TO_NOTVICT );
					break;

				case 1:
					act( "{5Udeszasz ogonem $C tak mocno jak tylko potrafisz. $N pada natychmiast na ziemiê ca³kowicie oszo³omion$R. Przez chwile wydaje ci siê, ¿e dostrzegasz gasn±ce w $Q ¿ycie.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n uderza ciê mocno swoim ogonem, sprawiaj±c, ¿e przewracasz siê na ziemiê. Uderzenie wypchnê³o z twoich p³uc ca³e powietrze, a gdy próbujesz go troche zaczerpn±æ z twojego gard³a wydobywa siê jedynie g³uche rzê¿enie, oczy zachodz± ci mg³±, umierasz.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n uderza $C mocno swoim ogonem wk³adaj±c w to chyba ca³± si³ê i umiejêtno¶ci. $N upada na ziemiê zupe³nie oszo³omion$R, dostrzegasz tylko przez moment gasn±ce w $Q ¿ycie a ju¿ po sekundzie jest po wszystkim.{x" , ch, NULL, victim, TO_NOTVICT );
					break;

				case 2:
					act( "{5$N stara siê w ostatniej chwili wymin±æ twój cios, otrzymuje jednak potê¿ne uderzenie twoim ogonem nie bed±c na to w ogóle przygotowan$R. $N pada na ziemiê, wyje z bólu i nieruchomieje.{x", ch, NULL, victim, TO_CHAR );
					act( "{5Otrzymujesz silne uderzenie ogonem od $z prosto w kark. Czujesz jak pêka ci krêgos³up, a w ca³ym ciele tracisz czucie... umierasz natychmiast.{x", ch, NULL, victim, TO_VICT );
					act( "{5$N stara siê w ostatniej chwili omin±æ nadlatuj±cy cios, prawie mu siê udaje, ale po chwili jednak zostaje przez $c potê¿nie uderzony ogonem w kark. S³ychaæ tylko g³o¶ny chrupot pêkaj±cego krêgos³upa. $N wali siê bez czucia na ziemiê i umiera.{x", ch, NULL, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
		else
		{
			switch ( number_range( 0, 3 ) )
			{
				case 0:
					act( "{5Wyczekujesz cierpliwie na okazje po czym uderzasz $C z ca³ej si³y swoim ogonem.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n wyczekuje na odpowiedni± chwilê, kiedy jeste¶ $t i uderza ciê z ca³ej si³y swoim ogonem.{x", ch, victim->sex == 2 ? "ods³oniêta" : "ods³oniêty", victim, TO_VICT );
					act( "{5$n wyczekuje na odpowiedni moment, kiedy $N jest $t i uderza $M z ca³ej si³y swoim ogonem.{x", ch, victim->sex == 2 ? "ods³oniêta" : "ods³oniêty", victim, TO_NOTVICT );
					break;

				case 1:
					act( "{5Odchodzisz lekko od $Z i uderzasz $M swoim ogonem tak mocno jak tylko mo¿esz.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n cofa siê przezornie od ciebie nabieraj±c dystansu, po czym uderza ciê swoim ogonem najmocniej jak potrafi.{x", ch, NULL, victim, TO_VICT );
					act( "{5$n cofa siê przezornie od $Z nabieraj±c dystansu, po czymi uderza $M swoim ogonem najmocniej jak potrafi.{x", ch, NULL, victim, TO_NOTVICT );
					break;

				case 2:
					act( "{5Odczekujesz chwilê a¿ $N siê trochê ods³oni i kiedy to siê staje uderzasz $M swoim ogonem.{x", ch, NULL, victim, TO_CHAR );
					act( "{5$n przez chwile dziwnie siê zachowuje... Zastanawiaj±c siê nad tym, na sekunde ods³aniasz siê i natychmiast odczuwasz na sobie skutek uderzenia ogona.{x", ch, victim->sex == 2 ? "poczêstowana" : "poczêstowany", victim, TO_VICT );
					act( "{5$n przez chwilê dziwnie siê zachowuje obserwuj±c $Z, po czym wykorzystuj±c dogodny moment i kiedy $t siê ods³ania, najmocniej jak potrafi uderza swoim ogonem.{x", ch, victim->sex == 2 ? "tamta" : "tamten", victim, TO_NOTVICT );
					break;

				case 3:
					act( "{5Ooo, udaje ci siê wyprowadziæ niez³y cios, uderzasz ogonem $C z pó³obrotu.{x", ch, NULL, victim, TO_CHAR );
					act( "{5Niech to! $x udaje siê zrobiæ g³êboki zamach swoim ogonem, a nastêpnie uderza ciê nim. Niezbyt interesuje ciê jakim cudem to sie sta³o, wa¿nym jest wszechogarniaj±cy twoje cia³o ból spowodowany mocarnym uderzeniem ogona!{x", ch, NULL, victim, TO_VICT );
					act( "{5$x udaje siê zrobiæ gwa³towny, mocny i precyzyjny zamach swoim ogonem. Trafia $C prosto w czu³e miejsce!{x", ch, NULL, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
	}
	else if ( dt == gsn_trip )
	{
		if ( dam > 0 )
		{
			act( "{5Przewracasz $C, $N z g³o¶nym stêkniêciem l±duje na ziemiê!{x", ch, NULL, victim, TO_CHAR );
			act( "{5$n przewraca ciê, walisz siê na ziemiê!{x", ch, NULL, victim, TO_VICT );
			act( "{5$n przewraca $C, posy³aj±c $M na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
			victim->position = POS_SITTING;
		}
		else
		{
			act( "{5Probujesz przewróciæ $C, bezskutecznie!{x", ch, NULL, victim, TO_CHAR );
			act( "{5$n próbuje ciê przewróciæ, bezskutecznie!{x", ch, NULL, victim, TO_VICT );
			act( "{5$n próbuje przewróciæ $C, bezskutecznie.{x", ch, NULL, victim, TO_NOTVICT );
		}
		return TRUE;
	}
	else if ( dt == gsn_smite )
	{
		if ( victim->hit - dam < -11 )
		{
			act( "{5Zbierasz w sobie moce dobra i czuj±c jak rozpiera ciê niesamowita si³a zadajesz $X ¶miertelny cios.{x", ch, NULL, victim, TO_CHAR );
			act( "{5$n zbiera w sobie moce dobra i zadaje ci ¶miertelny cios!{x", ch, NULL, victim, TO_VICT );
			act( "{5$n zbiera w sobie moce dobra i zadaje $X ¶miertelny cios.{x", ch, NULL, victim, TO_NOTVICT );
		}
		else
		{
			if ( dam >= 75 )
			{
				act( "{5Szepcz±c s³owa krótkiej modlitwy prosisz bogów o wsparcie i natychmiast wyprowadzasz morderczy cios, który rozpruwa $C!{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n szepcze s³owa krótkiej modlitwy prosi bogów o wsparcie i natychmiast wyprowadza morderczy cios, który ciê rozpruwa!{x", ch, NULL, victim, TO_VICT );
				act( "{5$n szepcze s³owa krótkiej modlitwy prosi bogów o wsparcie i natychmiast wyprowadza morderczy cios, który rozpruwa $C!{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam >= 50 && dam < 75 )
			{
				act( "{5Skupiasz w sobie energiê ¶wiat³a po czym wyprowadzasz silny cios rani±c mocno $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n skupia w sobie energiê ¶wiat³a po czym wyprowadza silny cios mocno ciê rani±c.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n skupia w sobie energiê ¶wiat³a po czym wyprowadza silny cios rani±c mocno $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam >= 35 && dam < 50 )
			{
				act( "{5Zbierasz w sobie moce dobra i wyprowadzasz potê¿ny cios dotkliwie rani±c $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie moce dobra i wyprowadza potê¿ny cios dotkliwie ciê rani±c.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie moce dobra i wyprowadza potê¿ny cios dotkliwie rani±c $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam >= 18 && dam < 35 )
			{
				act( "{5Zbierasz w sobie moce dobra i wyprowadzasz potê¿ny cios rani±c $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie moce dobra i wyprowadza potê¿ny cios rani±c ciê.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie moce dobra i wyprowadza potê¿ny cios rani±c $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam > 0 )
			{
				act( "{5Zbierasz w sobie moce dobra i wyprowadzasz cios rani±c lekko $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie moce dobra i wyprowadza cios rani±c ciê lekko.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie moce dobra i wyprowadza cios rani±æ lekko $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else
			{
				act( "{5Zbierasz w sobie moce dobra i próbujesz wyprowadziæ cios w kierunku $Z, jednak potykasz siê i ledwo utrzymujesz równowagê.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie moce dobra i próbuje wyprowadziæ cios w twoim kierunku, jednak potyka siê i ledwo utrzymuje równowagê.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie moce dobra i próbuje wyprowadziæ cios w kierunku $Z, jednak potyka siê i ledwo utrzymuje równowagê.{x", ch, NULL, victim, TO_NOTVICT );
			}
		}
		return TRUE;
	}
	else if ( dt == gsn_smite_good )
	{
		if ( victim->hit - dam < -11 )
		{
			act( "{5Zbierasz w sobie moce z³a i czuj±c jak rozpiera ciê niesamowita si³a zadajesz $X ¶miertelny cios.{x", ch, NULL, victim, TO_CHAR );
			act( "{5$n zbiera w sobie moce z³a i zadaje ci ¶miertelny cios!{x", ch, NULL, victim, TO_VICT );
			act( "{5$n zbiera w sobie moce z³a i zadaje $X ¶miertelny cios.{x", ch, NULL, victim, TO_NOTVICT );
		}
		else
		{
			if ( dam >= 75 )
			{
				act( "{5Szepcz±c s³owa krótkiej modlitwy prosisz mrocznych bogów o wsparcie i natychmiast wyprowadzasz morderczy cios, który rozpruwa $C!{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n szepcze s³owa krótkiej modlitwy prosi mrocznych bogów o wsparcie i natychmiast wyprowadza morderczy cios, który ciê rozpruwa!{x", ch, NULL, victim, TO_VICT );
				act( "{5$n szepcze s³owa krótkiej modlitwy prosi mrocznych bogów o wsparcie i natychmiast wyprowadza morderczy cios, który rozpruwa $C!{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam >= 50 && dam < 75 )
			{
				act( "{5Skupiasz w sobie energiê ciemno¶ci po czym wyprowadzasz silny cios powa¿nie rani±c $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n skupia w sobie energiê ciemno¶ci po czym wyprowadza silny cios powa¿nie ciê rani±c.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n skupia w sobie energiê ciemno¶ci po czym wyprowadza silny cios powa¿nie rani±c $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam >= 35 && dam < 50 )
			{
				act( "{5Zbierasz w sobie mroczne moce i wyprowadzasz potê¿ny cios dotkliwie rani±c $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie mroczne moce i wyprowadza potê¿ny cios dotkliwie ciê rani±c.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie mroczne moce i wyprowadza potê¿ny cios dotkliwie rani±c $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam >= 18 && dam < 35 )
			{
				act( "{5Zbierasz w sobie moce z³a i wyprowadzasz potê¿ny cios rani±c $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie moce z³a i wyprowadza potê¿ny cios rani±c ciê.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie moce z³a i wyprowadza potê¿ny cios rani±c $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else if ( dam > 0 )
			{
				act( "{5Zbierasz w sobie mroczne moce i wyprowadzasz cios rani±c lekko $C.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie mroczne moce i wyprowadza cios rani±c ciê lekko.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie mroczne moce i wyprowadza cios rani±æ lekko $C.{x", ch, NULL, victim, TO_NOTVICT );
			}
			else
			{
				act( "{5Zbierasz w sobie mroczne moce i próbujesz wyprowadziæ cios w kierunku $Z, jednak potykasz siê i ledwo utrzymujesz równowagê.{x", ch, NULL, victim, TO_CHAR );
				act( "{5$n zbiera w sobie mroczne moce i próbuje wyprowadziæ cios w twoim kierunku, jednak potyka siê i ledwo utrzymuje równowagê.{x", ch, NULL, victim, TO_VICT );
				act( "{5$n zbiera w sobie mroczne moce i próbuje wyprowadziæ cios w kierunku $Z, jednak potyka siê i ledwo utrzymuje równowagê.{x", ch, NULL, victim, TO_NOTVICT );
			}
		}
		return TRUE;
	}
	return FALSE;
}

/* autoresc dla berserkerow */
void check_berserk_rescue( CHAR_DATA * ch )
{
    CHAR_DATA * tank = NULL, *tch, *victim;
    int skill;

    if ( !ch ||
            !is_affected( ch, gsn_berserk ) ||
            !ch->fighting ||
            !ch->fighting->fighting ||
            ch->fighting->fighting == ch )
    {
        return;
    }

    victim = ch->fighting;

    for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
    {
        if ( tch != ch && victim->fighting == tch )
        {
            tank = tch;
            break;
        }
    }
    if ( !tank )
    {
        return;
    }
    /**
     * dla PC minimum 15 - nawet, je¿eli nie zna skilla
     * dla NPC maximum 15
     */
    skill = IS_NPC( ch ) ? UMIN( 15, get_skill( ch, gsn_rescue ) ) : UMAX( 15, get_skill( ch, gsn_rescue ) );

    if ( number_percent( ) < skill )
    {
        act( "{5Odpychasz $C na bok, wy³±czaj±c $M z walki!{x", ch, NULL, tank, TO_CHAR );
        act( "{5$n odpycha ciê na bok, wy³±czaj±c ciê z walki!{x", ch, NULL, tank, TO_VICT );
        act( "{5$n odpycha $C na bok, wy³±czaj±c $M z walki!{x", ch, NULL, tank, TO_NOTVICT );
        check_improve( ch, NULL, gsn_rescue, TRUE, 18 );

        stop_fighting( tank, FALSE );
        stop_fighting( victim, FALSE );
        check_killer( ch, victim );
        set_fighting( ch, victim );
        set_fighting( victim, ch );
    }
    else
    {
        act( "{5Probujesz wepchaæ siê przed $C, ale ci siê nie udaje!{x", ch, NULL, tank, TO_CHAR );
        switch ( ch->sex )
        {
            case SEX_NEUTRAL:
            case SEX_MALE:
            default:
                act( "{5$n próbuje wepchaæ siê przed ciebie, ale mu siê to nie udaje!{x", ch, NULL, tank, TO_VICT );
                act( "{5$n próbuje wepchaæ siê przed $C, ale mu siê to nie udaje!{x", ch, NULL, tank, TO_NOTVICT );
                break;
            case SEX_FEMALE:
                act( "{5$n próbuje wepchaæ siê przed ciebie, ale jej siê to nie udaje!{x", ch, NULL, tank, TO_VICT );
                act( "{5$n próbuje wepchac siê przed $C, ale jej siê to nie udaje!{x", ch, NULL, tank, TO_NOTVICT );
                break;
        }
        check_improve( ch, NULL, gsn_rescue, FALSE, 18 );
    }
    return;
}

bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim, int ac, int th_roll, int thac0, bool critical )
{
	int skill, dex_mod = 0, size_mod;

	//nie rusza sie, nie widzi albo krytyk - nie ma dodgowania
	if ( !IS_AWAKE( victim ) ||
			!can_move( victim ) ||
			!can_see( victim, ch ) ||
			critical )
		return FALSE;

	if ( ( IS_NPC( victim ) && EXT_IS_SET( victim->act, ACT_MONK ) ) ||
			( !IS_NPC( victim ) && victim->class == CLASS_MONK ) )
	{
		int bonus_ac = 0;

		if ( ( skill = get_skill( victim, gsn_monk_dodge ) ) <= 0 )
			return FALSE;

		if ( number_percent() > URANGE( 10, skill, 95 ) )
		{
			check_improve( victim, ch, gsn_monk_dodge, FALSE, 60 );
			return FALSE;
		}

		bonus_ac = URANGE( 1, 1 + ch->level / 5, 6 );

		if ( th_roll < ( thac0 - ( ac - bonus_ac ) ) )
		{
			act( "{5Robisz b³yskawiczny unik i cios ciê nie trafia.{x", ch, NULL, victim, TO_VICT );
			act( "{5$N robi b³yskawiczny unik i twój atak nie trafia.{x", ch, NULL, victim, TO_CHAR );
			act( "{5$N robi b³yskawiczny unik i cios $z nie trafia.{x", ch, NULL, victim, TO_NOTVICT );
			check_improve( victim, ch, gsn_monk_dodge, TRUE, 60 );
			return TRUE;
		}

		check_improve( victim, ch, gsn_monk_dodge, FALSE, 70 );
		return FALSE;
	}


	if ( !IS_SET( victim->fight_data, FIGHT_DODGE ) )
		return FALSE;

	skill = get_skill( victim, gsn_dodge );

	switch ( get_curr_stat_deprecated( victim, STAT_DEX ) )
	{
		case 19: dex_mod = 2;	break;
		case 20: dex_mod = 3;	break;
		case 21: dex_mod = 4;	break;
		case 22: dex_mod = 5;	break;
		case 23: dex_mod = 6;	break;
		case 24: dex_mod = 7;	break;
		case 25: dex_mod = 7;	break;
		case 26: dex_mod = 8;	break;
		case 27: dex_mod = 8;	break;
		case 28: dex_mod = 9;	break;
		default: break;
	}

	switch ( victim->size )
	{
		case SIZE_TINY: size_mod = 130;	break;
		case SIZE_SMALL: size_mod = 100;	break;
		case SIZE_MEDIUM: size_mod = 85;	break;
		case SIZE_LARGE: size_mod = 65;	break;
		case SIZE_HUGE: size_mod = 60;	break;
		case SIZE_GIANT: size_mod = 50;	break;
		default: size_mod = 100;	break;
	}

	dex_mod = UMAX( 1, size_mod * dex_mod / 100 );

	dex_mod += skill / 16;

#ifdef INFO
	print_char( victim, "Dex_mod = %d\n\r", dex_mod );
	print_char( victim, "THAC0: %d\n\r", thac0 );
	print_char( victim, "AC: %d\n\r", ac );
	print_char( victim, "DICEROLL: %d\n\r", th_roll );
	print_char( victim, "THAC0-ac-dex_mod: %d\n\r", thac0 - ( ac - dex_mod ) );
#endif


	if ( th_roll < ( thac0 - ( ac - dex_mod ) ) )
	{
		act( "{5Robisz b³yskawiczny unik i cios $z ciê nie trafia.{x", ch, NULL, victim, TO_VICT );
		act( "{5$N robi b³yskawiczny unik i twój atak nie trafia.{x", ch, NULL, victim, TO_CHAR );
		act( "{5$N robi b³yskawiczny unik i cios $z nie trafia.{x", ch, NULL, victim, TO_NOTVICT );

		if( ch->size <= SIZE_LARGE && ch->size >= SIZE_SMALL && ch->position > POS_SITTING )
        {
                check_trick( victim, ch, SN_TRICK_SHOVE );
        }

		return TRUE;
	}

	return FALSE;
}

//naliczanie zgonow/zabojstw dla postaci i klanu
void update_death_statistic( CHAR_DATA *victim, CHAR_DATA *ch )
{
	if ( IS_NPC( victim ) && IS_NPC( ch ) )
		return;

	if ( IS_NPC( victim ) && !IS_NPC( ch ) )
	{
		ch->pcdata->death_statistics.mob_kills++;

		if ( ch->pcdata->clan )
			ch->pcdata->clan->mkills++;

		return;
	}
	else if ( !IS_NPC( victim ) )
	{
		if ( victim == ch )
		{
			victim->pcdata->death_statistics.suicides++;
			victim->pcdata->death_statistics.deaths++;
			return;
		}
		else if ( !IS_NPC( ch ) || ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) && ch->master && !IS_NPC( ch->master ) ) )
		{
			MURDER_LIST * tmp_death, *tmp_kill;
			char buf[ MAX_STRING_LENGTH ];
			bool charm = FALSE;
			CHAR_DATA * killer = ch;

			tmp_death = malloc( sizeof( *tmp_death ) );

			if ( IS_NPC( ch ) )
			{
				charm = TRUE;
				killer = ch->master;
			}

			if ( charm )
			{
				sprintf( buf, "%s [%5d] (%d) - charm gracza %s", ch->short_descr, ch->pIndexData->vnum, ch->level, killer->name );
				tmp_death->name = str_dup( buf );
				tmp_death->char_level = killer->level;
				tmp_death->victim_level = victim->level;
			}
			else
			{
				tmp_death->name = str_dup( killer->name4 );
				tmp_death->char_level = killer->level;
				tmp_death->victim_level = victim->level;
			}

			tmp_death->room = victim->in_room->vnum;
			tmp_death->time = current_time;
			tmp_death->next = victim->pcdata->death_statistics.pkdeath_list;
			victim->pcdata->death_statistics.pkdeath_list = tmp_death;

			tmp_kill = malloc( sizeof( *tmp_kill ) );
			if ( charm )
			{
				sprintf( buf, "gracza %s (%d) z wykorzystaniem charma %s [%5d]", victim->name, victim->level, ch->short_descr, ch->pIndexData->vnum );
				tmp_kill->name = str_dup( buf );
				tmp_kill->char_level = killer->level;
				tmp_kill->victim_level = ch->level;
			}
			else
			{
				tmp_kill->name = str_dup( victim->name4 );
				tmp_kill->char_level = killer->level;
				tmp_kill->victim_level = victim->level;
			}

			tmp_kill->time = tmp_death->time;
			tmp_kill->room = tmp_death->room;
			tmp_kill->next = killer->pcdata->death_statistics.pkills_list;
			killer->pcdata->death_statistics.pkills_list = tmp_kill;


			victim->pcdata->death_statistics.pkdeaths++;
			victim->pcdata->death_statistics.deaths++;
			killer->pcdata->death_statistics.player_kills++;

			if ( victim->level > 5 && killer->pcdata->clan )
				killer->pcdata->clan->pkills++;

			if ( victim->pcdata->clan )
				victim->pcdata->clan->pdeaths++;
			return;
		}
		else if ( IS_NPC( ch ) )
		{
			MURDER_LIST * tmp_death;
			char buf[ MAX_STRING_LENGTH ];
			tmp_death = malloc( sizeof( *tmp_death ) );
			sprintf( buf, "%s [%5d]", ch->short_descr, ch->pIndexData->vnum );
			tmp_death->name = str_dup( buf );
			tmp_death->char_level = ch->level;
			tmp_death->victim_level = victim->level;
			tmp_death->room = victim->in_room->vnum;
			tmp_death->time = current_time;
			tmp_death->next = victim->pcdata->death_statistics.pkdeath_list;
			victim->pcdata->death_statistics.pkdeath_list = tmp_death;

			victim->pcdata->death_statistics.deaths++;

			if ( victim->pcdata->clan )
				victim->pcdata->clan->mdeaths++;

			return;
		}

	}
}

void check_special_unarmed_hit( CHAR_DATA *ch, CHAR_DATA *victim, int base_dam )
{
	int dam;
	AFFECT_DATA bleeding_wound;

	if ( !IS_NPC( ch ) || ch->fighting != victim || base_dam <= 0 )
		return;

	if ( IS_SET( ch->attack_flags, WEAPON_POISON ) )
	{
		poison_to_char( victim, 1 );
	}

	/* wysysanie, nie zadaje obrazen bo juz zadalo wczesniej */
	if ( IS_SET( ch->attack_flags, WEAPON_VAMPIRIC ) && !is_undead( victim ) && !IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		if (dam>0)
		{
			act( "Dotkniêcie $Z wysysa ¿ycie z $z.", victim, NULL, ch, TO_ROOM );
			act( "Czujesz jak dotkniêcie $Z wysysa z ciebie energiê.", victim, NULL, ch, TO_CHAR );
			if (ch->hit + dam > 125 * ch->hit / 100)
			{
				act( "Energia wyssana z $Z ucieka bezpowrotnie.", ch, NULL, victim, TO_CHAR );
			}
			else
			{
				ch->hit = UMIN( ch->hit + dam, 125 * ch->hit / 100 );
			}
		}
	}

	if ( IS_SET( ch->attack_flags, WEAPON_FLAMING ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "$n parzy siê przy zetkniêciu z $V.", victim, NULL, ch, TO_ROOM );
		act( "Parzysz siê przy zetkniêciu z $V.", victim, NULL, ch, TO_CHAR );
		fire_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_FIRE, FALSE );
	}

	if ( IS_SET( ch->attack_flags, WEAPON_TOXIC ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "Pokrywaj±cy rêce $Z kwas wy¿era skórê $z.", victim, NULL, ch, TO_ROOM );
		act( "Pokrywaj±cy rêce $Z kwas wy¿era tw± skórê!", victim, NULL, ch, TO_CHAR );
		acid_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_ACID, FALSE );
	}

	if ( IS_SET( ch->attack_flags, WEAPON_FROST ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "Dotyk $Z zamra¿a $c.", victim, NULL, ch, TO_ROOM );
		act( "Zimny dotyk $Z zamienia ciê w bry³ê lodu.", victim, NULL, ch, TO_CHAR );
		cold_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_COLD, FALSE );
	}

	if ( IS_SET( ch->attack_flags, WEAPON_SHOCKING ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );

		if ( victim->sex == 2 )
			act( "B³yskawica z $Z trafia $c wywo³uj±c u niej niekontrolowane drgawki.", victim, NULL, ch, TO_ROOM );
		else
			act( "B³yskawica z $Z trafia $c wywo³uj±c u niego niekontrolowane drgawki.", victim, NULL, ch, TO_ROOM );

		act( "Dotkniêcie $Z pora¿a ciê seri± elektrowstrz±sów.", victim, NULL, ch, TO_CHAR );
		shock_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_LIGHTNING, FALSE );
	}

	if ( IS_SET( ch->attack_flags, WEAPON_RESONANT ) )
    {
        dam = number_range( base_dam / 4, base_dam / 2 );
        dam = UMAX( dam, 1 );
        if (IS_SET(victim->parts, PART_HEAD ) )
        {
            act( "Fala ultrad¼wiêków z d³oni $Z wdziera siê do g³owy $z.", victim, NULL, NULL, TO_ROOM );
            act( "Fala ultrad¼wiêków z d³oni $Z wdziera siê do twej g³owy wraz z pulsuj±cym bólem.", victim, NULL, NULL, TO_CHAR );
        }
        else
        {
            act( "Fala ultrad¼wiêków z d³oni $Z wdziera siê do cia³a $z.", victim, NULL, NULL, TO_ROOM );
            act( "Fala ultrad¼wiêków z d³oni $Z wdziera siê do twojego cia³a wraz z pulsuj±cym bólem.", victim, NULL, NULL, TO_CHAR );
        }
		sound_effect( victim, 1, dam, TARGET_CHAR );
		damage( ch, victim, dam, 0, DAM_SOUND, FALSE );
	}

	//nie-undeadom zada polowe tego
	if ( IS_SET( ch->attack_flags, WEAPON_SACRED ) )
	{
		dam = number_range( base_dam / 4, base_dam / 2 );
		dam = UMAX( dam, 1 );
		act( "Niewielki strumieñ ¶wiêtej mocy z d³oni $Z trafia $c.", victim, NULL, NULL, TO_ROOM );
		act( "Niewielki strumieñ ¶wiêtej mocy z d³oni $Z trafia ciê prosto w pier¶.", victim, NULL, NULL, TO_CHAR );
		damage( ch, victim, dam, 0, DAM_HOLY, FALSE );
		if( IS_NPC(victim) && victim->pIndexData->vnum == MOB_VNUM_AVATAR )
		{
			act( "{5W krótkim rozb³ysku ¶wiat³a $n zamienia siê w kupkê dymi±cego popio³u.{x", victim, NULL, NULL, TO_ROOM );
			raw_damage( ch, victim, 20 + get_max_hp(victim) );
		}
	}

//	void spell_great_dispel( int sn, int level, CHAR_DATA *ch, void *vo,int target )
//  void spell_dispel_magic( int sn, int level, CHAR_DATA *ch, void *vo, int target )
	if ( IS_SET( ch->attack_flags, WEAPON_DISPEL ) )
	{
		weapon_dispeller(ch, victim, base_dam);
	}

	if ( !is_affected(victim,gsn_bleeding_wound) && !is_undead(victim) && ( ( IS_SET( ch->attack_flags, WEAPON_SHARP ) ) || ( IS_SET( ch->attack_flags, WEAPON_VORPAL ) ) || ( IS_SET( ch->attack_flags, WEAPON_INJURIOUS ) ) ) && ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) ) )
	{
		//chance na zranienie
		if( IS_SET( ch->attack_flags, WEAPON_SHARP ) && !stat_throw( victim, STAT_CON ) && number_percent() <= 15 )
		{
			act( "Cios zadany przez $C pozostawia na ciele $z krwawi±c± ranê!", victim, NULL, ch, TO_ROOM );
			act( "Cios zadany przez $C pozostawia na twoim ciele krwawi±c± ranê!", victim, NULL, ch, TO_CHAR );
			bleeding_wound.where = TO_AFFECTS;
			bleeding_wound.type = gsn_bleeding_wound;
			bleeding_wound.level = 50;
			bleeding_wound.duration = number_range(1,3);
			bleeding_wound.rt_duration = 0;
			bleeding_wound.location = APPLY_NONE;
			bleeding_wound.modifier = 0;
			bleeding_wound.bitvector = &AFF_NONE;
			affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
		}
		else if( IS_SET( ch->attack_flags, WEAPON_VORPAL ) && !stat_throw( victim, STAT_CON ) && number_percent() <= 35 )
		{
			act( "Cios zadany przez $C pozostawia na ciele $z krwawi±c± ranê!", victim, NULL, ch, TO_ROOM );
			act( "Cios zadany przez $C pozostawia na twoim ciele krwawi±c± ranê!", victim, NULL, ch, TO_CHAR );
			bleeding_wound.where = TO_AFFECTS;
			bleeding_wound.type = gsn_bleeding_wound;
			bleeding_wound.level = 50;
			bleeding_wound.duration = number_range(1,5);
         bleeding_wound.rt_duration = 0;
			bleeding_wound.location = APPLY_NONE;
			bleeding_wound.modifier = 0;
			bleeding_wound.bitvector = &AFF_NONE;
			affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
		}
		else if( IS_SET( ch->attack_flags, WEAPON_INJURIOUS ) && number_percent() <= 25 )
		{
			act( "Cios zadany przez $C pozostawia na ciele $z krwawi±c± ranê!", victim, NULL, ch, TO_ROOM );
			act( "Cios zadany przez $C pozostawia na twoim ciele krwawi±c± ranê!", victim, NULL, ch, TO_CHAR );
			bleeding_wound.where = TO_AFFECTS;
			bleeding_wound.type = gsn_bleeding_wound;
			bleeding_wound.level = 50;
			bleeding_wound.duration = number_range(1,10);
			bleeding_wound.rt_duration = 0;
			bleeding_wound.location = APPLY_NONE;
			bleeding_wound.modifier = 0;
			bleeding_wound.bitvector = &AFF_NONE;
			affect_to_char(	victim,	&bleeding_wound, NULL, TRUE );
		}
	}

	return;
}

/* do_one_hit_monk*/
void one_hit_monk( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
	int victim_ac;
	int thac0;
	int dam, base_dam = 0;
	int diceroll;
	int dam_type = DAM_BASH;
	bool result = TRUE, immune = FALSE, critical = FALSE, prog_mod = FALSE;
	MONK_HITS * monk_hit = generate_hit( ch, victim );

	DEBUG_INFO( "one_hit:checks" );
	if ( victim == ch || ch == NULL || victim == NULL )
		return;

	if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
		return;

	DEBUG_INFO( "onehit:get_AC" );
	victim_ac = GET_AC( victim, AC_BASH ) / 10;

	DEBUG_INFO( "onehit:ac_modifiers" );
	if ( !can_see( ch, victim ) )
		victim_ac -= 4;

	if ( ch->alignment < -750 && IS_AFFECTED( victim, AFF_PROTECT_EVIL ) )
		victim_ac -= 2;

	if ( ch->alignment > 750 && IS_AFFECTED( victim, AFF_PROTECT_GOOD ) )
		victim_ac -= 2;

	if ( victim->position < POS_FIGHTING )
		victim_ac += 3;

	if ( victim->position < POS_RESTING )
		victim_ac += 6;

	if ( IS_SET( victim->fight_data, FIGHT_CASTING ) )
		victim_ac += 3;

	if ( victim->daze > 0 )
		victim_ac += 4;

	DEBUG_INFO( "onehit:compute_thac0" );
	thac0 = compute_tohit( ch, FALSE );
	thac0 -= monk_hit->hit_bonus;

	DEBUG_INFO( "onehit:trig_onehit" );
	to_hit_bonus = 0;
	to_dam_bonus = 0;
	fight_text_ch[ 0 ] = '\0';
	fight_text_victim[ 0 ] = '\0';
	fight_text_room[ 0 ] = '\0';

	/*onehit trigger for mobiles*/
	if ( IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_ONEHIT ) )
		prog_mod = mp_onehit_trigger( ch, victim );

	if ( !victim->in_room )
		return;

	if ( prog_mod && to_hit_bonus != 0 )
		thac0 -= to_hit_bonus;


	check_defensive_spells( ch, victim );

	if ( ch->position < POS_RESTING || !ch->in_room )
		return;

	while ( ( diceroll = number_bits( 5 ) ) >= 20 );

	/* standardowe rzeczy przeklejone z damage(...)*/

	DEBUG_INFO( "onehit:standard_checks" );
	if ( victim->position == POS_DEAD )
		return;

	if ( victim != ch )
	{
		DEBUG_INFO( "onehit:is_safe" );
		if ( is_safe( ch, victim ) )
			result = FALSE;

		DEBUG_INFO( "onehit:trig_kill" );
		if ( victim->position > POS_STUNNED )
		{
			if ( victim->fighting == NULL )
			{
				set_fighting( victim, ch );

				if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
					mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_KILL );
			}
		}

		DEBUG_INFO( "onehit:set_fighting" );
		if ( ch->position > POS_STUNNED )
		{
			if ( ch->fighting == NULL )
				set_fighting( ch, victim );
		}

		DEBUG_INFO( "onehit:charm_stop_follower" );
		if ( victim->master == ch && !accident )
		{
			if( IS_NPC( victim ) && IS_AFFECTED(victim,AFF_CHARM) )
			{
				act( "Wiê¼ miêdzy tob± a $V zostaje brutalnie przerwana!", ch, NULL, victim, TO_CHAR );
				raw_damage( ch, ch, number_range( ch->level/2, ch->level+ch->level/2 ) );
				if ( ch->position <= POS_STUNNED )
					return;
			}
			stop_follower( victim );
		}

		DEBUG_INFO( "onehit:strip_hide/invis" );
		if ( IS_AFFECTED( ch, AFF_HIDE ) )
			affect_strip( ch, gsn_hide );

		strip_invis( ch, TRUE, TRUE );

		DEBUG_INFO( "onehit:start_hating" );
		if ( IS_NPC( victim ) && !is_fearing( victim, ch ) && !accident && !is_hating( victim, ch ) )
		{
		DEBUG_INFO( "onehit:start_hating:start_hunting" );
			if ( !EXT_IS_SET( victim->act, ACT_SENTINEL ) )
            {
				start_hunting( victim, ch );
            }
		DEBUG_INFO( "onehit:start_hating" );
			start_hating( victim, ch );
		}
	}
	/* koniec standardowych rzeczy*/


	DEBUG_INFO( "onehit:miss" );
	/*czesc kodu przy nietrafieniu kogostam*/
	if ( ( diceroll == 0 || ( can_move( victim ) && diceroll != 19 && diceroll < thac0 - victim_ac ) ) )
	{
		generate_monk_miss( ch, victim );
		return;
	}
	/* koniec czesci kodu przy nie-trafieniu kogostam */

	DEBUG_INFO( "onehit:mob_hitting_without_weapon" );
	/* mobile wali z reki */
	dam = dice( monk_hit->damage[ 0 ], monk_hit->damage[ 1 ] ) + monk_hit->dam_bonus;
	base_dam = dam;

	/* critical mobilesa*/
	if ( diceroll == 19 )
	{
		dam += dice( monk_hit->damage[ 0 ], monk_hit->damage[ 1 ] ) + monk_hit->dam_bonus;
		critical = TRUE;
	}

	if ( prog_mod && to_dam_bonus != 0 )
		dam += to_dam_bonus;

	/* dodajemy reszte dodatkowych obrazen*/
	dam += GET_DAMROLL( ch, NULL );
	dam += weapon_skill_mod( ch, TRUE, FALSE );


#ifdef INFO
	print_char( ch, "DAM +DAMROLL: %d\n\r", dam );
#endif


	DEBUG_INFO( "onehit:bashed_mod" );
	/* jesli koles nie stoi */
	if ( victim->position < POS_FIGHTING )
	{
		/* nieprzytomy/spiacy obrazenia razy 2, reszta +33%*/
		if ( !IS_AWAKE( victim ) )
			dam *= 2;
		else
			dam += dam / 4;
#ifdef INFO
		print_char( ch, "DAM + POSITION_BONUS: %d\n\r", dam );
#endif

	}

	/* sparowany atak??*/
	if ( dt != gsn_backstab )
	{
		DEBUG_INFO( "onehit:parry" );
		if ( check_parry( ch, victim, dam, NULL ) )
			return;

		DEBUG_INFO( "onehit:dodge" );
		/* po dodge, jest chance ze cios pojdzie w kogos obok */
		if ( check_dodge( ch, victim, victim_ac , diceroll, thac0, critical ) )
		{
			CHAR_DATA * vch = NULL;

			if ( number_percent() < 15 )
			{
				for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
					if ( vch != ch && vch->fighting == victim )
						break;

				if ( vch )
				{
					accident = TRUE;
					one_hit_monk( ch, vch, dt );
					accident = FALSE;
					return;
				}
			}

			return;
		}
	}
	/* jesli trafilismy to obrazenia minimum 1*/
	if ( dam <= 0 )
		dam = 1;

	if ( dam > 50 && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && dt == TYPE_UNDEFINED )
	{
		bugf( "Koles %s cos podejrzanie mocno bije z reki!", ch->name );
		dam = 50;
	}

	DEBUG_INFO( "onehit:mirror_image" );
	if ( check_mirror_image( victim, ch ) )
	{
		print_char( ch, "Twój cios trafia, %s znika.\n\r", PERS( victim, ch ) );
		print_char( victim, "%s trafia jedno z twoich lustrzanych odbiæ.\n\r", PERS2( ch, victim ) );
		act( "$n trafia jedno z lustrzanych odbiæ $Z.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit:blink" );
	if ( check_blink( victim, ch, TRUE ) )
	{
		act( "Kiedy twój cios ju¿ ma trafiæ w $C, $S sylwetka chwilowo znika.", ch, NULL, victim, TO_CHAR );
		act( "Kiedy cios $z ma ju¿ w ciebie trafiæ znikasz na chwilkê z tego planu egzystencji.\n\r", ch, NULL, victim, TO_VICT );
		act( "Kiedy cios $z ma ju¿ trafiæ w $C, $S sylwetka chwilowo znika.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit:stone_skin" );
	if ( check_stone_skin( ch, victim, dam_type, &dam, NULL ) )
	{
		if ( dam == 0 )
		{
			print_char( ch, "Twój cios jest nieskuteczny, jakby¶ uderzy³%s w kamieñ.\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
			print_char( victim, "%s trafia ciê, jednak %s cios jest nieskuteczny.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : "jego", ch->sex == 2 ? "uderzy³a" : "uderzy³" );
			act( "$n trafia $C, jednak $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
			return;
		}
		else
		{
			print_char( ch, "Twój cios przebija siê przez kamienn± skórê pokrywajac± %s.\n\r", PERS4( victim, ch ) );
			print_char( victim, "Cios %s przebija siê przez twoja kamienn± skórê.\n\r", PERS2( ch, victim ) );
			act( "Cios $z trafia $C przebijajac siê przez kammiena skórê.", ch, NULL, victim, TO_NOTVICT );
		}
	}

	DEBUG_INFO( "onehit:magic_weapon" );
	if ( victim->resists[ RESIST_MAGIC_WEAPON ] > 0 )
	{
		//do uzupelnienia
	}

	if ( immune || dam == 0 )
	{
		print_char( ch, "Twój atak jest nieskuteczny, cios nic %s nie robi.\n\r", PERS3( victim, ch ) );
		print_char( victim, "Atak %s jest nieskuteczny, %s cios niczego ci nie robi.\n\r", PERS2( ch, victim ), ch->sex == 2 ? "jej" : "jego" );
		act( "$n trafia, jednak $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit:check_resist" );
	dam = check_resist( victim, dam_type, dam );

#ifdef INFO
	print_char( ch, "DAM -RESISTS: %d\n\r", dam );
#endif

	if ( immune || dam == 0 )
	{
		print_char( ch, "Twój atak jest nieskuteczny, cios nic %s nie robi.\n\r", PERS3( victim, ch ) );
		print_char( victim, "Atak %s jest nieskuteczny, %s cios niczego ci nie robi.\n\r", PERS2( ch, victim ), ch->sex == 2 ? "jej" : "jego" );
		act( "Atak $z nie robi wrazenia na $B, $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );

		return;
	}

	DEBUG_INFO( "onehit:increase_wounds" );
	/* spell zwiekszajacy obrazenia slash/pierce/bash */
	check_increase_wounds( victim, dam_type, &dam );

#ifdef INFO
	print_char( ch, "DAM +INCREASE_WOUNDS: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit:troll_power" );
	if ( IS_AFFECTED( victim, AFF_TROLL ) && victim->hit > 0 && victim->hit - dam < 0 )
	{
		dam = victim->hit + 1;
		create_event( EVENT_TROLL_POWER_REGEN, number_range( 3, 5 ) * PULSE_VIOLENCE, victim, NULL, 0 );
	}

	if ( is_affected(victim,gsn_demon_aura))
	{
        int dam_mod = 14;
		if( IS_GOOD(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura)/dam_mod;
        }
		else if ( !IS_EVIL(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura)/22;
        }
	}

	DEBUG_INFO( "onehit:damage_reduction" );
	if ( is_affected( victim, gsn_damage_reduction ) && dam > 0 )
	{
		AFFECT_DATA * pAff;

		if ( ( pAff = affect_find( victim->affected, gsn_damage_reduction ) ) != NULL )
		{
			if ( pAff->level <= 16 )
				dam -= 1;
			else if ( pAff->level <= 20 )
				dam -= 2;
			else if ( pAff->level <= 25 )
				dam -= 3;
			else if ( pAff->level <= 30 )
				dam -= 4;
			else
				dam -= 5;

			dam = UMAX( dam, 1 );
		}
#ifdef INFO
		print_char( ch, "DAM - DAMAGE_REDUCTION: %d\n\r", dam );
#endif

	}

#ifdef INFO
	print_char( ch, "FINAL DAMAGE: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit:dam_mesg" );
	/* walimy opis ciosu*/
	generate_monk_dam_message( ch, victim, monk_hit );

	DEBUG_INFO( "onehit:raw_damage" );
	raw_damage( ch, victim, dam );

	/* koniec ciekawostek dla umierajacych*/
	if ( ch->fighting != victim || !victim->in_room )
		return;

	DEBUG_INFO( "onehit:check_armor" );
	/* dla tych co przezyli, sprawdzamy armora */
	check_armor_spell( victim, dam );

	if ( result && IS_NPC( ch ) && ch->attack_flags != 0 )
		check_special_unarmed_hit( ch, victim, base_dam );

	if ( IS_NPC( ch ) && HAS_TRIGGER( ch, TRIG_HIT ) )
		mp_hit_trigger( ch, victim );

	tail_chain( );
	return;
}

void strip_fight_affects( CHAR_DATA * ch )
{
	DEBUG_INFO( "strip_fight_affects(1)" );
	if ( is_affected( ch, gsn_berserk ) )
	{
        bool check = FALSE;
        //Rysand: Analiza kodu: z featem berserk nie schodzi?!
		if ( ch->hit > -1 && !check )
        {
			affect_strip( ch, gsn_berserk );
        }
	}

	DEBUG_INFO( "strip_fight_affects(2)" );
	if ( is_affected( ch, gsn_wardance ) )
		affect_strip( ch, gsn_wardance );

	DEBUG_INFO( "strip_fight_affects(3)" );
	if ( is_affected( ch, gsn_damage_reduction ) )
		affect_strip( ch, gsn_damage_reduction );

	DEBUG_INFO( "strip_fight_affects(4)" );
	if ( is_affected( ch, gsn_chant ) )
		affect_strip( ch, gsn_chant );

	DEBUG_INFO( "strip_fight_affects(5)" );
	if ( is_affected( ch, gsn_prayer ) )
		affect_strip( ch, gsn_prayer );

	DEBUG_INFO( "strip_fight_affects(6)" );
	if ( is_affected( ch, gsn_flurry_of_blows ) )
		affect_strip( ch, gsn_flurry_of_blows );

	if ( is_affected( ch, gsn_weapon_wrench ) )
		affect_strip( ch, gsn_weapon_wrench );

	if ( is_affected( ch, gsn_glorious_impale ) )
		affect_strip( ch, gsn_glorious_impale );

	REMOVE_BIT( ch->fight_data, FIGHT_THOUSANDSLAYER );

	ch->fight_data = 0;
	ch->num_attacks = 0;
}

//fireshield, iceshield, blade barrier, confusion shell
void check_defensive_spells( CHAR_DATA * ch, CHAR_DATA * victim )
{
	if ( !ch || !victim || !ch->in_room || !victim->in_room )
		return;

	DEBUG_INFO( "check_def_spells:fireshield" );
	/* ognista tarcza: 2d6 obrazen ogniowych */
	if ( IS_AFFECTED( victim, AFF_FIRESHIELD ) )
	{
		if ( IS_NPC( ch ) && ( ch->pIndexData->vnum == 3 || IS_AFFECTED(ch, AFF_ILLUSION ) ) )
			;
		else
		{
			print_char( ch, "{4Jêzor p³omienia z ognistej tarczy otaczaj±cej %s {4pali ciê!\n\r{x", PERS4( victim, ch ) );
			print_char( victim, "{2Jêzor p³omienia z twojej ognistej tarczy pali %s{2!\n\r{x", PERS4( ch, victim ) );
			act( "{3Jêzor p³omienia z ognistej tarczy $z {3pali $C{3!{x", victim, NULL, ch, TO_NOTVICT );
			damage( victim, ch, dice( 2, 6 ), gsn_fireshield, DAM_FIRE, FALSE );
		}

		if ( ch->position < POS_RESTING || !ch->in_room )
			return;
	}

	DEBUG_INFO( "check_def_spells:iceshield" );
	/* lodowa tarcza: 2d6 obrazen od zimna */
	if ( IS_AFFECTED( victim, AFF_ICESHIELD ) && !IS_AFFECTED( victim, AFF_FIRESHIELD ) )
	{
		if ( IS_NPC( ch ) && ( ch->pIndexData->vnum == 3 || IS_AFFECTED(ch, AFF_ILLUSION ) ) )
			;
		else
		{
			print_char( ch, "{4Lodowa tarcza otaczaj±ca %s {4mrozi ciê!{x\n\r", PERS4( victim, ch ) );
			print_char( victim, "{2Twoja lodowa tarcza mrozi %s{2!{x\n\r", PERS4( ch, victim ) );
			act( "{3Lodowa tarcza $z {3mrozi $C{3!{x", victim, NULL, ch, TO_NOTVICT );

			damage( victim, ch, dice( 2, 6 ), gsn_iceshield, DAM_COLD, FALSE );
		}

		if ( ch->position < POS_RESTING || !ch->in_room )
			return;
	}

	/* bariera ostrzy: 2d8 obrazen od 'slash/pierce' + (level-10)/5 */
	if ( IS_AFFECTED( victim, AFF_BLADE_BARRIER ) )
	{
		AFFECT_DATA * pAff;
		int level = victim->level;

		if ( ( pAff = affect_find( victim->affected, gsn_blade_barrier ) ) != NULL )
			level = pAff->level;

		if ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 )
			;
		else
		{
			print_char( ch, "{4Bariera wiruj±cych ostrzy %s {4dotkliwie ciê rani!{x\n\r", PERS4( victim, ch ) );
			print_char( victim, "{2Twoja bariera wiruj±cych ostrzy dotkliwie rani %s{2!{x\n\r", PERS4( ch, victim ) );
			act( "{3Bariera wiruj±cych ostrzy $z {3dotkliwie rani $C{3!{x", victim, NULL, ch, TO_NOTVICT );

			damage( victim, ch, dice( 2, 8 ) + (level-10) / 5, gsn_blade_barrier, number_percent() < 50 ? DAM_PIERCE : DAM_SLASH, FALSE );
		}

		if ( ch->position < POS_RESTING || !ch->in_room )
			return;
	}


	/* bariera elementów: 2d8 obrazen od 'magic' + (level-10)/5 */
	DEBUG_INFO( "check_def_spells:shield_of_nature" );
	if ( IS_AFFECTED( victim, AFF_SHIELD_OF_NATURE ) )
	{
		AFFECT_DATA * pAff;
		int level = victim->level;

		if ( ( pAff = affect_find( victim->affected, gsn_shield_of_nature ) ) != NULL )
		{
			level = pAff->level;
		}

		if (!( IS_NPC( ch ) && ch->pIndexData->vnum == 3 ))
		{
			// DAM_FIRE
			// DAM_COLD
			// DAM_LIGHTNING
			// DAM_ACID
			// DAM_ENERGY
			// DAM_SOUND
			/*

			   Twoje ( ciecie itp zale¿y od broni) trafia w tarcze Moiza widzisz jak z tarczy kieruje siê w twoja stronê kula ognia która trafia ciê prosto w pier¶!

			 *B³yskawice

			 Twoje ( ciecie itp zale¿y od broni) trafia w tarcze Moiza widzisz jak z tarczy wydobywa siê strumieñ skumulowanej energii który kieruje siê w twoja stronê ów promieñ trafia prosto w ciebie. Ból jest nie do wytrzymania.

			 *Woda

			 Twoje ciecie trafia wprost w tarcze, z której w oka mgnieniu wylatuj± jêzyki wodne kieruj±ce siê w twoja stronê si³a by³a tak potê¿na, ¿e a¿ powali³a ciê na ziemie.
			 Od strony druida.



Druid:

			 *Ogieñ

			 Ciecie Moiza trafia w twoja tarcze która jakby od¿y³a a z niej wylatuje kula ognia która trafia Moiza prosto w pier¶!

			 *Elektryczno¶æ

			 Ciecie Moiza trafia w twoja tarcze po której przechodzi miliony promieni elektrycznych a tuz zaraz z tarczy wylatuje promieñ który trafia w Moiza widzisz jak Moiz trzêsie siê jak opêtany.

			 *Woda

			 Ciecie Moiza trafia wprost w tarcze z której b³yskawicznie wystrzelaj± jêzyki wodne zmierzaj±ce w stronê Moiza. Jêzyki wodne atakuj± Moiza z taka si³a, ¿e a¿ ten przewraca siê na ziemie.


			 *Ogieñ
			 Z otaczaj±cej ciê bariery ¿ywio³ów wylatuje kula ognia, która trafia Moiza prosto w pier¶!
			 Z otaczaj±cej Druida bariery ¿ywio³ów wylatuje kula ognia, która trafia ciê prosto w pier¶!

			 *Elektryczno¶æ (ew. +shock)
			 Z otaczaj±cej ciê bariery ¿ywio³ów wylatuje b³yskawica, która trafia prosto w Moiza.
			 Z otaczaj±cej Druida bariery ¿ywio³ów wylatuje b³yskawica, która trafia prosto w ciebie.

			 *Woda (ew. +bash)
			 Z otaczaj±cej ciê bariery ¿ywio³ów wystrzelaj± jêzyki wodne, po czym z ogromnym pêdem trafiaj± w Moiza.
			 Z otaczaj±cej Druida bariery ¿ywio³ów wystrzelaj± jêzyki wodne, po czym z ogromnym pêdem trafiaj± w ciebie.


			 */
			print_char( ch, "{4Bariera wiruj±cych ostrzy %s {4dotkliwie ciê rani!{x\n\r", PERS4( victim, ch ) );
			print_char( victim, "{2Twoja bariera wiruj±cych ostrzy dotkliwie rani %s{2!{x\n\r", PERS4( ch, victim ) );
			act( "{3Bariera wiruj±cych ostrzy $z {3dotkliwie rani $C{3!{x", victim, NULL, ch, TO_NOTVICT );

			damage( victim, ch, dice( 2, 8 ) + (level-10) / 5, gsn_blade_barrier, number_percent() < 50 ? DAM_PIERCE : DAM_SLASH, FALSE );
		}

		if ( ch->position < POS_RESTING || !ch->in_room )
		{
			return;
		}
	}

	DEBUG_INFO( "check_def_spells:demon_aura" );
	if ( is_affected( victim, gsn_demon_aura ) )
	{
		AFFECT_DATA * aura = affect_find( victim->affected, gsn_demon_aura );

		if ( ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 ) || !IS_GOOD(ch) || aura->level < 50 || IS_AFFECTED(ch, AFF_ILLUSION ) )
			;
		else
		{
			int dam;
			if ( IS_AFFECTED(ch, AFF_PROTECT_EVIL ) )
			{
				dam = dice( 1, aura->level/20 );
				if (dam>0)
				{
					act( "Aura mroku otaczaj±ca $C sprawia, ¿e zaczynasz czuæ siê ¼le, ale dziêki ochronnemu zaklêciu jej wp³yw zostaje lekko zniwelowany!", ch, NULL, victim, TO_CHAR );
					act( "Otaczaj±ca ciê aura mroku wysysa niewiele energii z $z, bowiem jej wp³yw zosta³ zniwelowany przez jakie¶ zaklêcie!", ch, NULL, victim, TO_VICT );
					act( "Aura mroku otaczaj±ca $C próbuje wysysaæ ¿ycie z $z, jednak co¶ sprawia, ¿e jej wp³yw zostaje zminimalizowany.", ch, NULL, victim, TO_NOTVICT );
					damage( victim, ch, dam, gsn_demon_aura, DAM_NEGATIVE, FALSE );
				}
			}
			else
			{
				dam = dice( 1, aura->level/10 );
				if (dam>0)
				{
					act( "Aura negatywnej energii otaczaj±ca $C wysysa z ciebie ¿ycie!", ch, NULL, victim, TO_CHAR );
					act( "Otaczaj±ca ciê aura negatywnej energii wysysa ¿ycie z $z.", ch, NULL, victim, TO_VICT );
					act( "Aura negatywnej energii otaczaj±ca $C wysysa ¿ycie z $z.", ch, NULL, victim, TO_NOTVICT );
					damage( victim, ch, dam, gsn_demon_aura, DAM_NEGATIVE, FALSE );
				}
			}
		}
		if ( ch->position < POS_RESTING || !ch->in_room )
		{
			return;
		}
	}

	AFFECT_DATA *damned, hold;
	OBJ_DATA *damned_armor, *weapon2;
	DEBUG_INFO( "check_def_spells:damned_armor" );
	if ( IS_GOOD(ch) && victim->class == CLASS_BLACK_KNIGHT && ( damned_armor = get_eq_char(victim,WEAR_BODY)) != NULL )
	{
		if ( ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 ) )
			;
		else if ( get_skill(victim,gsn_damn_armor) > number_range(35,70 ) )
		{
			for ( damned = damned_armor->affected; damned != NULL; damned = damned->next )
				if ( damned->type == gsn_damn_armor )
				{
                    int mod = 0;
					switch ( number_range( 1, 25 + mod ) )
					{
						case 1:
							act( "Z twojej zbroi ulatuje przeklêta dusza i zawodz±c okrêca siê wokó³ $Z.", victim, NULL, ch, TO_CHAR );
							act( "Ze zbroi $z ulatuje przeklêta dusza i zawodz±c okrêca siê wokó³ ciebie!", victim, NULL, ch, TO_VICT );
							act( "Ze zbroi $z ulatuje przeklêta dusza i zawodz±c okrêca siê wokó³ $Z.", victim, NULL, ch, TO_NOTVICT );

							if ( IS_AFFECTED(victim, AFF_PROTECT_EVIL ) && number_percent() > 50 )
							{
								act( "Chwile pó¼niej dusza ze straszliwym jêkiem znika w rozb³ysku jasnego ¶wiat³a.", NULL, NULL, NULL, TO_ALL );
								break;
							}

							if ( !IS_AFFECTED( ch, AFF_PARALYZE ) && !IS_AFFECTED( ch, AFF_DAZE ) && !IS_AFFECTED( ch, AFF_FREE_ACTION ) && 2*get_curr_stat_deprecated( ch, STAT_WIS ) < number_range( 1, get_skill(victim,gsn_damn_armor )))
							{
								hold.where     = TO_AFFECTS;
								hold.level     = victim->level;
								hold.location  = APPLY_NONE;
								hold.duration  = damned->level/6 + get_skill(ch,gsn_damn_armor)/24;
								hold.rt_duration = 0;
								hold.modifier	 = 0;

								if( damned->level > 15 )
								{
									send_to_char( "Zosta³<&e¶/a¶/o¶> sparali¿owan<&y/a/e>!\n\r", ch );
									act("$n zatrzymuje siê w miejscu.",ch,NULL,NULL,TO_ROOM);
									hold.type      = gsn_holdmonster;
									hold.bitvector = &AFF_PARALYZE;
								}
								else
								{
									switch ( ch->sex )
									{
										case 0:
											act( "$n rozgl±da siê dooko³a, lekko oszo³omione.", ch, NULL, NULL, TO_ROOM );
											send_to_char( "Czujesz siê lekko oszo³omione!\n\r", ch );
											break;
										case 2:
											act( "$n rozgl±da siê dooko³a, lekko oszo³omiona.", ch, NULL, NULL, TO_ROOM );
											send_to_char( "Czujesz siê lekko oszo³omiona!\n\r", ch );
											break;
										default:
											act( "$n rozgl±da siê dooko³a, lekko oszo³omiony.", ch, NULL, NULL, TO_ROOM );
											send_to_char( "Czujesz siê lekko oszo³omiony!\n\r", ch );
											break;
									}
									hold.type      = 304;
									hold.bitvector = &AFF_DAZE;
								}

								affect_to_char( ch, &hold, NULL, TRUE  );
							}
							break;
						case 2:
							act( "Z twojej zbroi ulatuje przeklêta dusza i zawodz±c okrêca siê wokó³ $Z.", victim, NULL, ch, TO_CHAR );
							act( "Ze zbroi $z ulatuje przeklêta dusza i zawodz±c okrêca siê wokó³ ciebie!", victim, NULL, ch, TO_VICT );
							act( "Ze zbroi $z ulatuje przeklêta dusza i zawodz±c okrêca siê wokó³ $Z.", victim, NULL, ch, TO_NOTVICT );

							if ( IS_AFFECTED(victim, AFF_PROTECT_EVIL ) && number_percent() > 50 )
							{
								act( "Chwile pó¼niej dusza ze straszliwym jêkiem znika w rozb³ysku jasnego ¶wiat³a.", NULL, NULL, NULL, TO_ALL );
								break;
							}

							if ( ( weapon2 = get_eq_char( ch, WEAR_SECOND ) ) == NULL )
								weapon2 = get_eq_char( ch, WEAR_WIELD );
							if( 2*get_curr_stat_deprecated( ch, STAT_WIS ) < number_range( 1, get_skill(victim,gsn_damn_armor )))
							{
								if ( weapon2 && !IS_OBJ_STAT( weapon2, ITEM_NOREMOVE ))
								{
									act( "Z twoich trzês±cych siê ze strachu d³oni wypada $h.",ch, weapon2, victim, TO_CHAR);
									act( "Z trzês±cych siê ze strachu d³oni $z wypada $h.",ch, weapon2, victim, TO_ROOM);
									if ( is_artefact( weapon2 ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
										artefact_from_char( weapon2, ch );
									obj_from_char( weapon2 );
									if ( IS_OBJ_STAT( weapon2, ITEM_NODROP ) || IS_OBJ_STAT( weapon2, ITEM_INVENTORY ) )
									{
										if ( is_artefact( weapon2 ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
											artefact_to_char( weapon2, ch );
										obj_to_char( weapon2, ch );
									}
									else
									{
										obj_to_room( weapon2, ch->in_room );
										if ( IS_NPC( ch ) && ch->wait == 0 && can_see_obj( ch, weapon2 ) )
											get_obj( ch, weapon2, NULL );
									}
									break;
								}
							}
							break;
						case 3:
							act( "Z twojej zbroi ulatuje przeklêta dusza i zawodz±c okrêca siê wokó³ $Z.", victim, NULL, ch, TO_CHAR );
							act( "Ze zbroi $z ulatuje przeklêta dusza i zawodz±c okrêca siê wokó³ ciebie!", victim, NULL, ch, TO_VICT );
							act( "Ze zbroi $z ulatuje przeklêta dusza i zawodz±c okrêca siê wokó³ $Z.", victim, NULL, ch, TO_NOTVICT );

							if ( IS_AFFECTED(victim, AFF_PROTECT_EVIL ) && number_percent() > 50 )
							{
								act( "Chwile pó¼niej dusza ze straszliwym jêkiem znika w rozb³ysku jasnego ¶wiat³a.", NULL, NULL, NULL, TO_ALL );
								break;
							}

							if (ch->position > POS_SITTING)
							{
								if( 2*get_curr_stat_deprecated( ch, STAT_WIS ) < number_range( 1, get_skill(victim,gsn_damn_armor )))
								{
									act( "Potykasz siê i padasz na ziemiê!",ch, NULL, victim, TO_CHAR);
									act( "$n potyka siê i upada na ziemiê.",ch, NULL, victim, TO_ROOM);
									if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED(ch, AFF_FLOAT))
									{
										affect_strip( ch, gsn_fly );
										affect_strip( ch, gsn_float );
									}
									ch->position = POS_SITTING;
									WAIT_STATE( ch, 2*PULSE_VIOLENCE );
								}
							}
							break;
						default:
							break;
					}
					break;
				}
		}
	}

	DEBUG_INFO( "check_def_spells:confusion_shell" );
	/* iluzje zlych stworkow - victim dostaje jakiestam feary i chance ze spieprzy */
	if ( IS_AFFECTED( victim, AFF_CONFUSION_SHELL ) && !IS_AFFECTED( ch, AFF_BRAVE_CLOAK ) )
	{
		AFFECT_DATA fear;
		OBJ_DATA * weapon;
		if ( IS_NPC( ch ) && ch->pIndexData->vnum == 3 )
			;
		else if (ch->resists[RESIST_FEAR] > 0 && number_percent() < ch->resists[RESIST_FEAR])
			;
		else if ( ch->level - number_range(-1,5) > victim->level )
			;
		else if	( IS_AFFECTED( ch, AFF_PARALYZE ) )
			;
		else if ( !saves_spell_new(ch, skill_table[gsn_confusion_shell].save_type, skill_table[gsn_confusion_shell].save_mod, victim, gsn_confusion_shell ) && get_curr_stat_deprecated(ch, STAT_WIS) < number_range(1,30))
		{
			if ( IS_AFFECTED( ch, AFF_BERSERK ) )
			{
				act( "Sza³ bitewny za bardzo opanowa³ $z, nie zwraca $e uwagi na otaczaj±ce ciê iluzje.\n\r", ch, NULL, victim, TO_VICT );
				return;
			}
			if( !IS_AFFECTED(ch, AFF_FEAR))
			{
				fear.where = TO_AFFECTS;
				fear.type = 172;//fear
				fear.level = victim->level;
				fear.duration = URANGE(1, victim->level/8, 3);
				fear.rt_duration = 0;
				fear.location = APPLY_NONE;
				fear.modifier = 0;
				fear.bitvector = &AFF_FEAR;
				affect_to_char( ch, &fear, NULL, TRUE  );
			}

			act( "Otaczaj±ce $C monstra rzucaj± siê na ciebie! Aaaaa! Biegasz w kó³ko w panice.",ch, NULL, victim, TO_CHAR);
			act( "$n panikuje widz±c otaczaj±ce ciê iluzje potworów.",ch, NULL, victim, TO_VICT);
			act( "Otaczaj±ce $C monstra rzucaj± siê na $c, $e krzyczy i biega w kó³ko w panice.",ch, NULL, victim, TO_NOTVICT);
			if( ch->fighting && number_range(1,ch->level) < victim->level/3 )
			{
				if(IS_NPC(ch)) start_fearing(ch,victim);
				switch(number_range(1,3))
				{
					case 1:
						interpret(ch,"flee");
						break;
					case 2:
						if ( ( weapon = get_eq_char( ch, WEAR_SECOND ) ) == NULL )
							weapon = get_eq_char( ch, WEAR_WIELD );
						if ( weapon && !IS_OBJ_STAT( weapon, ITEM_NOREMOVE ))
						{
							act( "Z twoich trzês±cych siê ze strachu d³oni wypada $h.",ch, weapon, victim, TO_CHAR);
							act( "Z trzês±cych siê ze strachu d³oni $z wypada $h.",ch, weapon, victim, TO_ROOM);
							if ( is_artefact( weapon ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
								artefact_from_char( weapon, ch );
							obj_from_char( weapon );
							if ( IS_OBJ_STAT( weapon, ITEM_NODROP ) || IS_OBJ_STAT( weapon, ITEM_INVENTORY ) )
							{
								if ( is_artefact( weapon ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
									artefact_to_char( weapon, ch );
								obj_to_char( weapon, ch );
							}
							else
							{
								obj_to_room( weapon, ch->in_room );
								if ( IS_NPC( ch ) && ch->wait == 0 && can_see_obj( ch, weapon ) )
									get_obj( ch, weapon, NULL );
							}
							break;
						}
					case 3:
						if (ch->position > POS_SITTING)
						{
							act( "Potykasz siê i padasz na ziemiê!",ch, NULL, victim, TO_CHAR);
							act( "$n potyka siê i upada na ziemiê.",ch, NULL, victim, TO_ROOM);
							if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED(ch, AFF_FLOAT))
							{
								affect_strip( ch, gsn_fly );
								affect_strip( ch, gsn_float );
							}
							ch->position = POS_SITTING;
							WAIT_STATE( ch, 2*PULSE_VIOLENCE );
						}
						break;
				}
			}
			DAZE_STATE( ch, number_range(1,3)*PULSE_VIOLENCE );
		}

		if ( ch->position < POS_RESTING || !ch->in_room )
			return;
	}
	return;
}

bool check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim, int ac, int th_roll, int thac0, bool critic, int * dam )
{
	int ac_bonus = 0;
	int dex_skill_mod = 0;		//bonus do skilla
	int dex_block_bonus = 0;	//bonus do blokowania wiekszej ilosci atakow
	int shield_type = -1;
	int max_dam = 0;
	int alternatywny_blok = 0;	//jezeli mimo shield blocka atakujacy zda test na thac0, ale cios bedzie bardzo slaby damowo, to i tak bedzie chance na zablokowanie
	OBJ_DATA *shield = NULL;
	int shield_skill = get_skill( victim, gsn_shield_block );
    /**
     * jak nie umie, to spadamy
     */
    if ( shield_skill < 1 )
    {
        return FALSE;
    }
    /**
     * jak nie ma tarczy to tez sio
     */
    if( ( shield = get_eq_char( victim, WEAR_SHIELD ) ) == NULL )
    {
        return FALSE;
    }
    /**
     * nie blokujemy gdy nie mozemy sie ruszac etc
     */
    if ( !IS_AWAKE( victim ) || !can_move( victim ) || !can_see( victim, ch ) || critic )
    {
        return FALSE;
    }
    if ( get_curr_stat_deprecated( ch, STAT_INT ) < 16 )
    {
        shield_skill /= 2;
    }

    /**
     * Drake: Dodanie shield blocka dla paladynów(80%), kleryków i druidów(50%).
     */
    switch ( victim->class )
    {
        case CLASS_PALADIN:
            shield_skill -= get_skill( victim, gsn_shield_block )/5;
            break;
        case CLASS_CLERIC:
        case CLASS_DRUID:

            shield_skill -= get_skill( victim, gsn_shield_block )/2;
            break;
    }
    shield_skill = UMAX(1, shield_skill);

    //jesli to nowy typ obiektu pobieramy typ tarczy z v[0], jesli nie to typ = -1
    if( shield->item_type == ITEM_SHIELD )
    {
        shield_type = shield->value[0];
    }

	switch( get_curr_stat_deprecated( victim, STAT_DEX ) )
	{
		case 16:
		case 17:
			dex_skill_mod = 5;
			dex_block_bonus = 2;
			break;
		case 18:
		case 19:
			dex_skill_mod = 8;
			dex_block_bonus = 3;
			break;
		case 20:
		case 21:
		case 22:
			dex_skill_mod = 10;
			dex_block_bonus = 5;
			break;
		case 23:
		case 24:
			dex_skill_mod = 15;
			dex_block_bonus = 7;
			break;
		case 25:
		case 26:
			dex_skill_mod = 20;
			dex_block_bonus = 10;
			break;
		case 27:
		case 28:
			dex_skill_mod = 25;
			dex_block_bonus = 15;
			break;
		default:break;
	}

	//za rozmiary - bardzo duzy nie zablokuje bardzo malego, maly nie zablokuje malego,
	//najlepiej jak blokuje sie kogos o 1 mniejszego, najgorzej jak kogos duzo wiekszego:p
	//ch to bijacy, victim to blokujacy sie
	if ( ch->size - victim->size == 1 )
	{
		dex_skill_mod -= 4;
		dex_block_bonus -= 3;
	}
	else if ( ch->size - victim->size == 2 )
	{
		dex_skill_mod -= 8;
		dex_block_bonus -= 8;
	}
	else if ( ch->size - victim->size == 3 )
	{
		dex_skill_mod -= 15;
		dex_block_bonus -= 15;
	}
	else if ( ch->size - victim->size >= 4 )
	{
		dex_skill_mod -= 30;
		dex_block_bonus -= 30;
	}

	if ( ch->size - victim->size == -1 )
	{
		dex_skill_mod += 10;
		dex_block_bonus += 3;
	}
	else if ( ch->size - victim->size == -2 )
	{
		dex_skill_mod += 5;
		dex_block_bonus += 6;
	}
	else if ( ch->size - victim->size == -3 )
	{
		dex_skill_mod -= 10;
		dex_block_bonus += 3;
	}
	else if ( ch->size - victim->size <= -4 )
	{
		dex_skill_mod -= 25;
		dex_block_bonus -= 3;
	}


	//nie wyszlo
	if( number_percent() > ( shield_skill + dex_skill_mod ) )
	{
		check_improve( victim, ch, gsn_shield_block, FALSE, 250 );
		return FALSE;
	}

	//test na dodatkowe ataki
	//bonus od tarczy zasadniczo odnosi sie tylko do jednego atakujacego, tego z ktorym walczymy
	//ataki od inny atakujacych maja szanse trafic w tarcze, ale juz nie tak czesto
	if( victim->fighting != ch	&&
			( shield_type >= 0		||
			  number_percent() > shield->value[3] + dex_block_bonus ) )
	{
		check_improve( victim, ch, gsn_shield_block, FALSE, 270 );
		return FALSE;
	}

	//nowy typ tarczy
	if( shield_type >= 0 )
		ac_bonus = URANGE( 1, ( shield->value[1] * ( 50 + shield_skill ))/100, 8 );
	else
		ac_bonus = 1 + shield_skill/30;

	if ( th_roll < ( thac0 - ( ac - ac_bonus ) ) )
	{
		if( shield_type >= 0 )
			max_dam = shield->value[2];
		else
			max_dam = 25;

		check_improve( victim, ch, gsn_shield_block, TRUE, 280);

		max_dam += str_app[get_curr_stat(victim,STAT_STR)].todam;


		if( max_dam < *dam )
		{
			if ( max_dam <= ( *dam * 2 ) && !IS_OBJ_STAT( shield, ITEM_UNDESTRUCTABLE )
					&& number_percent() > material_table[ shield->material ].hardness )
			{
				shield->condition -= UMIN( 5, *dam / UMAX( 1, max_dam ) );
			}

			*dam -= max_dam;

			if ( shield->condition <= 0 )
			{
				act( "{5Próbujesz zablokowaæ mia¿d¿±cy cios $z, ale $p rozpada siê na dwie czê¶ci. Czujesz dotkliwy ból w przedramieniu.{x", ch, shield, victim, TO_VICT );
				act( "{5$N próbuje zablokowaæ twoj mia¿d¿±cy cios, ale $S $p rozpada siê na dwie czê¶ci. Przez twarz $Z przebiega grymas bólu.{x", ch, shield, victim, TO_CHAR );
				act( "{5$N próbuje zablokowaæ mia¿d¿±cy cios $z, ale $S $p rozpada siê na dwie czê¶ci. Przez twarz $Z przebiega grymas bólu.{x", ch, shield, victim, TO_NOTVICT );

				/*artefact*/
				if ( is_artefact( shield ) ) extract_artefact( shield );

				extract_obj( shield );
				return FALSE;
			}
			else
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "{5Szybkim ruchem unosisz tarczê, cios $z by³ jednak zbyt potê¿ny.{x", ch, shield, victim, TO_VICT );
						act( "{5$N szybkim ruchem unosi tarczê, jednak twój cios by³ zbyt potê¿ny.{x", ch, shield, victim, TO_CHAR );
						act( "{5$N szybkim ruchem unosi tarczê, cios $z by³ jednak zbyt potê¿ny.{x", ch, shield, victim, TO_NOTVICT );
						break;
					case 2:
						act( "{5Os³aniasz siê $j, jednak potê¿ny cios $z odtr±ca tarczê na bok i trafia ciê!{x", ch, shield, victim, TO_VICT );
						act( "{5$N os³ania siê $j, jednak twój potê¿ny cios odtr±ca tarczê na bok i trafia $C!{x", ch, shield, victim, TO_CHAR );
						act( "{5$N os³ania siê $j, jednak potê¿ny cios $z odtr±ca tarczê na bok i trafia $C!{x", ch, shield, victim, TO_NOTVICT );
						break;
					case 3:
						act( "{5P³ynnym ruchem zas³aniasz siê $j, jednak uderzenie $z odtr±ca twoj± tarcze na bok i trafia ciê!{x", ch, shield, victim, TO_VICT );
						act( "{5P³ynnym ruchem $N zas³ania siê $j, jednak twój silny cios uderza w tarczê, odrzucaj±c j± na bok i trafia w $Z!{x", ch, shield, victim, TO_CHAR );
						act( "{5P³ynnym ruchem $N zas³ania siê $j, jednak silne uderzenie $z odtr±ca $s tarczê na bok i trafia $m!{x", ch, shield, victim, TO_NOTVICT );
						break;
				}
				return FALSE;
			}
		}
		else
		{
			switch ( number_range( 1, 4 ) )
			{
				case 1:
					act( "{5Szybkim ruchem unosisz tarczê w ostatniej chwili zbijaj±c cios $z.{x", ch, shield, victim, TO_VICT );
					act( "{5$N szybkim ruchem unosi tarczê w ostatniej chwili zbijaj±c twoj cios.{x", ch, shield, victim, TO_CHAR );
					act( "{5$N szybkim ruchem unosi tarczê w ostatniej chwili zbijaj±c cios $z.{x", ch, shield, victim, TO_NOTVICT );
					break;
				case 2:
					act( "{5Szybkim ruchem wystawiasz $h i parujesz uderzenie $z.{x", ch, shield, victim, TO_VICT );
					act( "{5$N szybkim, wyæwiczonym ruchem wystawia przed siebie $h i paruje twoje uderzenie.{x", ch, shield, victim, TO_CHAR );
					act( "{5$N szybkim, wyæwiczonym ruchem wystawia $h i paruje uderzenie $z.{x", ch, shield, victim, TO_NOTVICT );
					break;
				case 3:
					act( "{5Dok³adnym, wyæwiczonym ruchem wystawiasz $h przed siebie i cios $z ze¶lizguje siê nie czyni±c ci ¿adnej szkody.{x", ch, shield, victim, TO_VICT );
					act( "{5Dok³adnym, wyæwiczonym ruchem $N wystawia $h przed siebie i twój atak ze¶lizguje siê nie czyni±c $O ¿adnej szkody.{x", ch, shield, victim, TO_CHAR );
					act( "{5Dok³adnym, wyæwiczonym ruchem $N wystawia $h przed siebie i cios $z ze¶lizguje siê nie czyni±c $O ¿adnej szkody.{x", ch, shield, victim, TO_NOTVICT );
					break;
				case 4:
					act( "{5Zas³aniasz siê $j, tak ¿e cios $z nie czyni ci ¿adnej szkody.{x", ch, shield, victim, TO_VICT );
					act( "{5$N zas³ania siê $j, tak ¿e twój cios nie czyni $O ¿adnej szkody.{x", ch, shield, victim, TO_CHAR );
					act( "{5$N zas³ania siê $j, tak ¿e cios $z nie czyni $O ¿adnej szkody.{x", ch, shield, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
	}

	alternatywny_blok = number_range( UMIN( get_skill( victim, gsn_shield_block )/10, 10 ), UMAX( 11, (UMAX( get_curr_stat_deprecated( ch,STAT_STR ), get_curr_stat_deprecated( ch,STAT_DEX ) )-13)*2 ) );


	if ( alternatywny_blok > *dam )
	{
		if( shield_type >= 0 )
			max_dam = shield->value[2];
		else
			max_dam = 25;

		check_improve( victim, ch, gsn_shield_block, TRUE, 80);

		max_dam += str_app[get_curr_stat(victim,STAT_STR)].todam;


		if( max_dam < *dam )
		{
			if ( max_dam <= ( *dam * 2 ) && !IS_OBJ_STAT( shield, ITEM_UNDESTRUCTABLE )
					&& number_percent() > material_table[ shield->material ].hardness )
			{
				shield->condition -= UMIN( 5, *dam / UMAX( 1, max_dam ) );
			}

			*dam -= max_dam;

			if ( shield->condition <= 0 )
			{
				act( "{5Próbujesz zablokowaæ mia¿d¿±cy cios $z, ale $p rozpada siê na dwie czê¶ci. Czujesz dotkliwy ból w przedramieniu.{x", ch, shield, victim, TO_VICT );
				act( "{5$N próbuje zablokowaæ twoj mia¿d¿±cy cios, ale $S $p rozpada siê na dwie czê¶ci. Przez twarz $Z przebiega grymas bólu.{x", ch, shield, victim, TO_CHAR );
				act( "{5$N próbuje zablokowaæ mia¿d¿±cy cios $z, ale $S $p rozpada siê na dwie czê¶ci. Przez twarz $Z przebiega grymas bólu.{x", ch, shield, victim, TO_NOTVICT );

				/*artefact*/
				if ( is_artefact( shield ) ) extract_artefact( shield );

				extract_obj( shield );
				return FALSE;
			}
			else
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "{5Szybkim ruchem unosisz tarczê, cios $z by³ jednak zbyt potê¿ny.{x", ch, shield, victim, TO_VICT );
						act( "{5$N szybkim ruchem unosi tarczê, cios $z by³ jednak zbyt potê¿ny.{x", ch, shield, victim, TO_CHAR );
						act( "{5$N szybkim ruchem unosi tarczê, cios $z by³ jednak zbyt potê¿ny.{x", ch, shield, victim, TO_NOTVICT );
						break;
					case 2:
						act( "{5Os³aniasz siê $j, jednak potê¿ny cios $z odtr±ca tarczê na bok i trafia ciê!{x", ch, shield, victim, TO_VICT );
						act( "{5$N os³ania siê $j, jednak twój potê¿ny cios odtr±ca tarczê na bok i trafia $C!{x", ch, shield, victim, TO_CHAR );
						act( "{5$N os³ania siê $j, jednak potê¿ny cios $z odtr±ca tarczê na bok i trafia $C!{x", ch, shield, victim, TO_NOTVICT );
						break;
					case 3:
						act( "{5P³ynnym ruchem zas³aniasz siê $j, jednak uderzenie $z odtr±ca twoj± tarcze na bok i trafia ciê!{x", ch, shield, victim, TO_VICT );
						act( "{5P³ynnym ruchem $N zas³ania siê $j, jednak twój silny cios uderza w tarczê, odrzucaj±c j± na bok i trafia w $Z!{x", ch, shield, victim, TO_CHAR );
						act( "{5P³ynnym ruchem $N zas³ania siê $j, jednak silne uderzenie $z odtr±ca $s tarczê na bok i trafia $m!{x", ch, shield, victim, TO_NOTVICT );
						break;
				}
				return FALSE;
			}
		}
		else
		{
			switch ( number_range( 1, 4 ) )
			{
				case 1:
					act( "{5Szybkim ruchem unosisz tarczê w ostatniej chwili zbijaj±c cios $z.{x", ch, shield, victim, TO_VICT );
					act( "{5$N szybkim ruchem unosi tarczê w ostatniej chwili zbijaj±c twoj cios.{x", ch, shield, victim, TO_CHAR );
					act( "{5$N szybkim ruchem unosi tarczê w ostatniej chwili zbijaj±c cios $z.{x", ch, shield, victim, TO_NOTVICT );
					break;
				case 2:
					act( "{5Szybkim ruchem wystawiasz $h i parujesz uderzenie $z.{x", ch, shield, victim, TO_VICT );
					act( "{5$N szybkim, wyæwiczonym ruchem wystawia przed siebie $h i paruje twoje uderzenie.{x", ch, shield, victim, TO_CHAR );
					act( "{5$N szybkim, wyæwiczonym ruchem wystawia $h i paruje uderzenie $z.{x", ch, shield, victim, TO_NOTVICT );
					break;
				case 3:
					act( "{5Dok³adnym, wyæwiczonym ruchem wystawiasz $h przed siebie i cios $z ze¶lizguje siê nie czyni±c ci ¿adnej szkody.{x", ch, shield, victim, TO_VICT );
					act( "{5Dok³adnym, wyæwiczonym ruchem $N wystawia $h przed siebie i twój atak ze¶lizguje siê nie czyni±c $O ¿adnej szkody.{x", ch, shield, victim, TO_CHAR );
					act( "{5Dok³adnym, wyæwiczonym ruchem $N wystawia $h przed siebie i cios $z ze¶lizguje siê nie czyni±c $O ¿adnej szkody.{x", ch, shield, victim, TO_NOTVICT );
					break;
				case 4:
					act( "{5Zas³aniasz siê $j, tak ¿e cios $z nie czyni ci ¿adnej szkody.{x", ch, shield, victim, TO_VICT );
					act( "{5$N zas³ania siê $j, tak ¿e twój cios nie czyni $O ¿adnej szkody.{x", ch, shield, victim, TO_CHAR );
					act( "{5$N zas³ania siê $j, tak ¿e cios $z nie czyni $O ¿adnej szkody.{x", ch, shield, victim, TO_NOTVICT );
					break;
			}
			return TRUE;
		}
	}

	check_improve( victim, ch, gsn_shield_block, FALSE, 280);

	return FALSE;
}


void one_hit_shield( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
	OBJ_DATA * shield;
	OBJ_DATA * obj, *obj_next, *damned_armor;
	CHAR_DATA *tch;
	int victim_ac;
	int thac0_00, thac0_32, thac0;
	int dam;
#ifdef INFO
    int base_dam = 0;
#endif
	int dice_1, dice_2;
	int diceroll, critical_chance;
	int sn = -1;
	int dam_type;
	bool immune = FALSE, critical = FALSE, prog_mod = FALSE, check;
	AFFECT_DATA *scarfskin, *damned, *defense_curl;

	DEBUG_INFO( "one_hit_shield:checks" );
	if ( victim == ch || ch == NULL || victim == NULL )
		return;

	if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
		return;

	if ( check_blink( ch, victim, FALSE ) )
	{
		send_to_char( "Próbujesz wyprowadziæ cios, jednak przeskakujesz na chwilkê do innego planu egzystencji i przeciwnik znika ci z oczu.\n\r", ch );
		return;
	}

	shield = get_eq_char( ch, WEAR_SHIELD );

	if ( shield == NULL )
		return;

	if ( dt == TYPE_UNDEFINED )
	{
		dt = TYPE_HIT + 43;
	}

	//zawsze uderzenie
	dam_type = DAM_BASH;

	DEBUG_INFO( "onehit_shield:get_weapon_skill" );
	sn = gsn_shield_block;

	DEBUG_INFO( "onehit_shield:get_AC" );
	victim_ac = GET_AC( victim, AC_BASH ) / 10;

	DEBUG_INFO( "onehit_shield:ac_modifiers" );
	if ( !can_see( ch, victim ) )
		victim_ac -= 4;

    check = FALSE;
	if ( ch->alignment < -750 && IS_AFFECTED( victim, AFF_PROTECT_EVIL ) && !check )
    {
		victim_ac -= 2;
    }

	if ( !IS_EVIL(ch) && victim->class == CLASS_BLACK_KNIGHT && ( damned_armor = get_eq_char(victim,WEAR_BODY)) != NULL )
	{
		for ( damned = damned_armor->affected; damned != NULL; damned = damned->next )
            if ( damned->type == gsn_damn_armor )
            {
                check = FALSE;
                if( IS_GOOD(ch) && !check )
                {
                    victim_ac -= URANGE( 1, damned->level/10 +  get_skill(victim,gsn_damn_armor)/24, 6 );
                }
				else
                {
					victim_ac -= URANGE( 1, damned->level/12 +  get_skill(victim,gsn_damn_armor)/33, 4 );
                }
			}
	}

	if ( victim->position < POS_FIGHTING )
		victim_ac += 3;

	if ( victim->position < POS_RESTING )
		victim_ac += 6;

	if ( IS_SET( victim->fight_data, FIGHT_CASTING ) )
		victim_ac += 3;

	if ( IS_SET( victim->fight_data, FIGHT_VERTICAL_SLASH ) )
		victim_ac += 6;

	if ( victim->daze > 0 )
		victim_ac += 4;

	if ( IS_NPC( ch ) )
	{
		thac0_00 = 20;
		if ( EXT_IS_SET( ch->act, ACT_WARRIOR ) )
			thac0_32 = -2;
		if ( EXT_IS_SET( ch->act, ACT_MONK ) )
			thac0_32 = -2;
		else if ( EXT_IS_SET( ch->act, ACT_PALADIN ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_BLACK_KNIGHT ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_BARBARIAN ) )
			thac0_32 = 0;
		else if ( EXT_IS_SET( ch->act, ACT_THIEF ) )
			thac0_32 = 4;
		else if ( EXT_IS_SET( ch->act, ACT_BARD ) )
			thac0_32 = 4;
		else if ( EXT_IS_SET( ch->act, ACT_CLERIC ) )
			thac0_32 = 4;
		else if ( EXT_IS_SET( ch->act, ACT_MAGE ) )
			thac0_32 = 8;
		else if ( EXT_IS_SET( ch->act, ACT_SHAMAN ) )
			thac0_32 = 4;
		else
			thac0_32 = 2;
	}
	else
	{
		thac0_00 = class_table[ ch->class ].thac0_00;
		thac0_32 = class_table[ ch->class ].thac0_32;
	}

	thac0 = interpolate( ch->level, thac0_00, thac0_32 );

	thac0 -= GET_HITROLL( ch );

	thac0 -= UMIN( shield->value[ 4 ], 6 );

	if ( get_eq_char(ch,WEAR_WIELD))
    {
        thac0 += 2 + 6 * ( 100 - get_skill(ch,gsn_two_weapon_fighting) ) / 100;
    }

	if ( get_eq_char(ch,WEAR_WIELD) )
		check_improve( ch, victim, gsn_two_weapon_fighting, TRUE, 220 );

	if ( !IS_NPC( ch ) && ch->condition[ COND_SLEEPY ] == 0 )
		thac0 += 4;

	if ( IS_SET( ch->fight_data, FIGHT_DISARM ) )
		thac0 += 4;

	if ( IS_SET( ch->fight_data, FIGHT_DODGE ) )
		thac0 += 2;

	if ( IS_SET( ch->fight_data, FIGHT_PARRY ) )
		thac0 += 6;

	if ( ch->daze > 0 )
		thac0 += 6;

	//premie i kary za rozmiary

	//tu jak jest wiekszy - kara za bicie w malego
	if ( ch->size - victim->size == 1 )
		thac0 += 2;
	else if ( ch->size - victim->size == 2 )
		thac0 += 3;
	else if ( ch->size - victim->size == 3 )
		thac0 += 4;
	else if ( ch->size - victim->size >= 4 )
		thac0 += 5;

	//jak jest mniejszy - poczatkowo premia, ale jak jest za duzy to tez kara
	if ( ch->size - victim->size == -1 )
		thac0 -= 2;
	else if ( ch->size - victim->size == -2 )
		;
	else if ( ch->size - victim->size == -3 )
		thac0 += 2;
	else if ( ch->size - victim->size <= -4 )
		thac0 += 4;

	//goody w grupce z death knightem z demon aura maja minusy do trafien
	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	{
		if( IS_GOOD(ch) && is_affected(tch,gsn_demon_aura) && is_same_group(ch,tch))
			thac0 += URANGE( 1, get_skill(tch,gsn_demon_aura)/16, 6);
	}

	tch = NULL;

	DEBUG_INFO( "onehit:trig_onehit" );
	to_hit_bonus = 0;
	to_dam_bonus = 0;
	fight_text_ch[ 0 ] = '\0';
	fight_text_victim[ 0 ] = '\0';
	fight_text_room[ 0 ] = '\0';

	/*onehit trigger for mobiles*/
	if ( shield && HAS_OTRIGGER( shield, TRIG_ONEHIT ) )
		prog_mod = op_onehit_trigger( shield, ch );

	if ( !victim->in_room )
		return;

	if ( prog_mod && to_hit_bonus != 0 )
		thac0 -= to_hit_bonus;

	check_defensive_spells( ch, victim );

	if ( ch->position < POS_RESTING || !ch->in_room )
		return;

	while ( ( diceroll = number_bits( 5 ) ) >= 20 );

	/* standardowe rzeczy przeklejone z damage(...)*/

	DEBUG_INFO( "onehit:standard_checks" );
	if ( victim->position == POS_DEAD )
		return;

	if ( victim != ch )
	{
		DEBUG_INFO( "onehit:trig_kill" );
		if ( victim->position > POS_STUNNED )
		{
			if ( victim->fighting == NULL )
			{
				set_fighting( victim, ch );

				if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
					mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_KILL );
			}
		}

		DEBUG_INFO( "onehit:set_fighting" );
		if ( ch->position > POS_STUNNED )
		{
			if ( ch->fighting == NULL )
				set_fighting( ch, victim );
		}

		DEBUG_INFO( "onehit:charm_stop_follower" );
		if ( victim->master == ch && !accident )
		{
			if( IS_NPC( victim ) && IS_AFFECTED(victim,AFF_CHARM) )
			{
				act( "Wiê¼ miêdzy tob± a $V zostaje brutalnie przerwana!", ch, NULL, victim, TO_CHAR );
				raw_damage( ch, ch, number_range( ch->level/2, ch->level+ch->level/2 ) );
				if ( ch->position <= POS_STUNNED )
					return;
			}
			stop_follower( victim );
		}

		DEBUG_INFO( "onehit:strip_hide/invis" );
		if ( IS_AFFECTED( ch, AFF_HIDE ) )
			affect_strip( ch, gsn_hide );

		strip_invis( ch, TRUE, TRUE );

		DEBUG_INFO( "onehit:start_hating" );
		if ( IS_NPC( victim ) && !is_fearing( victim, ch ) && !accident &&
				!is_hating( victim, ch ) )
		{
			if ( !EXT_IS_SET( victim->act, ACT_SENTINEL ) )
				start_hunting( victim, ch );

			start_hating( victim, ch );
		}
	}
	/* koniec standardowych rzeczy*/


	DEBUG_INFO( "onehit:miss" );
	/*czesc kodu przy nietrafieniu kogostam*/
	if ( ( diceroll == 0 || ( can_move( victim ) && diceroll != 19 && diceroll < thac0 - victim_ac ) ) )
	{
		switch( number_range(1, 2 + ( get_eq_char( victim,WEAR_BODY ) ? 1 : 0 ) ) )
		{
			case 1:
				act( "Próbujesz uderzyæ $C $j, jednak chybiasz.", ch, shield, victim, TO_CHAR );
				act( "$n próbuje uderzyæ ciê $j, jednak chybia.", ch, shield, victim, TO_VICT );
				act( "$n próbuje uderzyæ $C $j, jednak chybia.", ch, shield, victim, TO_NOTVICT );
				break;
			case 2:
				act( "Bior±c szeroki zamach próbujesz trafiæ $C kantem $f, jednak $E uchyla siê.", ch, shield, victim, TO_CHAR );
				act( "Bior±c szeroki zamach $n próbuje trafiæ ciê kantem $f, jednak szybko uchylasz siê.", ch, shield, victim, TO_VICT );
				act( "Bior±c szeroki zamach $n próbuje trafiæ $C kantem $f, jednak $E uchyla siê.", ch, shield, victim, TO_NOTVICT );
				break;
			case 3:
				act( "P³ynnym ruchem rêki uderzasz tarcz± w pier¶ $Z, jednak przez $f cios nie odniós³ skutku.", ch, get_eq_char(victim,WEAR_BODY), victim, TO_CHAR );
				act( "P³ynnym ruchem rêki $n uderza tarcz± w tw± pier¶, jednak dziêki $f nic ci siê nie sta³o.", ch, get_eq_char(victim,WEAR_BODY), victim, TO_VICT );
				act( "P³ynnym ruchem rêki $n uderza tarcz± w pier¶ $Z, jednak dziêki $f cios nie odniós³ skutku.", ch, get_eq_char(victim,WEAR_BODY), victim, TO_NOTVICT );
				break;
		}
		if ( sn != -1 )
        {
			check_improve( ch, victim, sn, FALSE, 280 );
        }
		return;
	}
	/* koniec czesci kodu przy nie-trafieniu kogostam */

	DEBUG_INFO( "onehit_shield:hitting" );
	/* jesli konkretna umiejetnosc broni to sprawdzamy czy siê nauczyl czegos*/
	if ( sn != -1 )
    {
		check_improve( ch, victim, sn, TRUE, 250 );
    }

	switch( shield->value[0] )
	{
        default:
		case 0://puklerz
			dice_1 = 1;
			dice_2 = 4;
			break;
		case 1://lekka
			dice_1 = 2;
			dice_2 = 2;
			break;
		case 2://srednia
			dice_1 = 2;
			dice_2 = 3;
			break;
		case 3://ciezka
			dice_1 = 2;
			dice_2 = 4;
			break;
		case 4://pawez
			dice_1 = 2;
			dice_2 = 5;
			break;
	}

	/* jesli uzywa jakiegos narzedzia*/
	dam = dice( UMIN( dice_1, 8 ), UMIN( dice_2, 20 ) ) + UMIN( shield->value[ 5 ], 6 );

#ifdef INFO
	base_dam = dam;
	print_char( ch, "BASE_DAM:%d\n\r", base_dam );
#endif

	/* critical */

	critical_chance = 19;
	if ( is_affected(ch,gsn_wardance) && number_percent() < get_skill(ch,gsn_wardance) ) --critical_chance;
	if ( is_affected(ch,gsn_berserk) && number_percent() < get_skill(ch,gsn_berserk) ) --critical_chance;
	if ( IS_AFFECTED(ch,AFF_HALLUCINATIONS_POSITIVE) ) --critical_chance;
	if ( IS_AFFECTED(ch,AFF_EYES_OF_THE_TORTURER) ) critical_chance -= 3;

	if ( diceroll >= critical_chance )
	{
		dam += dice( UMIN( dice_1, 8 ), UMIN( dice_2, 20 ) ) + UMIN( shield->value[ 5 ], 6 );
		critical = TRUE;

#ifdef INFO
		print_char( ch, "CRITICAL: %d\n\r", dam );
#endif

	}

	/* modyfikator obrazen ze wzgledu na uszkodzenie broni */
	//niezbyt pikne
	if ( shield->condition >= 90 )
		;
	else if ( shield->condition >= 85 )
		dam = UMAX( ( ( 95 * dam ) / 100 ), 1 );
	else if ( shield->condition >= 75 )
		dam = UMAX( ( ( 90 * dam ) / 100 ), 1 );
	else if ( shield->condition >= 65 )
		dam = UMAX( ( ( 70 * dam ) / 100 ), 1 );
	else if ( shield->condition >= 45 )
		dam = UMAX( ( ( 65 * dam ) / 100 ), 1 );
	else
		dam = UMAX( dam / 2, 1 );

#ifdef INFO
	print_char( ch, "DAM -WEAPON_CONDITION: %d\n\r", dam );
#endif

	if ( prog_mod && to_dam_bonus != 0 )
		dam += to_dam_bonus;

	//bonus do walki bronia
	if ( IS_NPC( ch ) )
		dam += ch->weapon_damage_bonus;

	//dobzi slabiej bija jak sa w grupce z death knightem z demoniczna aura
	for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
	{
		if( IS_GOOD(ch) && is_affected(tch,gsn_demon_aura) && is_same_group(ch,tch))
			dam -= URANGE( 1, get_skill(tch,gsn_demon_aura)/24, 4);
	}

	/* dodajemy reszte dodatkowych obrazen */
	dam += GET_DAMROLL( ch, shield );

#ifdef INFO
	print_char( ch, "DAM +DAMROLL: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit_shield:bashed_mod" );
	/* jesli koles nie stoi */
	if ( victim->position < POS_FIGHTING )
	{
		/* nieprzytomy/spiacy obrazenia razy 2, reszta +25%*/
		if ( !IS_AWAKE( victim ) )
			dam *= 2;
		else
			dam += dam / 4;
#ifdef INFO
		print_char( ch, "DAM + POSITION_BONUS: %d\n\r", dam );
#endif

	}

	/* sparowany atak??*/
	if ( dt != gsn_backstab /*dt != gsn_backstab && dt != gsn_circle && dt != gsn_charge*/ )
	{
		DEBUG_INFO( "onehit_shield:parry" );
		if ( check_parry( ch, victim, dam, shield ) )
			return;

		DEBUG_INFO( "onehit_shield:dodge" );
		/* po dodge, jest szansa ze cios pojdzie w kogos obok */
		if ( check_dodge( ch, victim, victim_ac , diceroll, thac0, critical ) )
		{
			CHAR_DATA * vch = NULL;

			if ( number_percent() < 15 )
			{
				for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
					if ( vch != ch && vch->fighting == victim )
						break;

				if ( vch )
				{
					accident = TRUE;
					one_hit_shield( ch, vch, dt );
					accident = FALSE;
					return;
				}
			}

			return;
		}

		if ( check_shield_block( ch, victim, victim_ac , diceroll, thac0, critical, &dam  ) )
			return;
	}

	/* jesli trafilismy to obrazenia minimum 1*/
	if ( dam <= 0 )
		dam = 1;

	if ( dam > 70 && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) && dt == TYPE_UNDEFINED )
	{
		bugf( "Gracz: %s co¶ podejrzanie mocno bije z %s [%d]!", ch->name, shield->short_descr, shield->pIndexData->vnum );
	}

	DEBUG_INFO( "onehit_shield:blink" );
	if ( check_blink( victim, ch, TRUE ) )
	{
		act( "Kiedy twój cios ju¿ ma trafiæ w $C, $S sylwetka chwilowo znika.", ch, NULL, victim, TO_CHAR );
		act( "Kiedy cios $z ma ju¿ w ciebie trafiæ znikasz na chwilkê z tego planu egzystencji.", ch, NULL, victim, TO_VICT );
		act( "Kiedy cios $z ma ju¿ trafiæ w $C, $S sylwetka chwilowo znika.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit_shield:mirror_image" );
	if ( check_mirror_image( victim, ch ) )
	{
		print_char( ch, "Twój cios trafia, %s znika.\n\r", PERS( victim, ch ) );
		print_char( victim, "%s trafia jedno z twoich lustrzanych odbiæ.\n\r", PERS( ch, victim ) );
		act( "$n trafia jedno z lustrzanych odbiæ $Z.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit_shield:illusion" );
	if ( IS_AFFECTED( victim, AFF_ILLUSION ) )
	{
		print_char( ch, "Twój cios trafia, %s rozp³ywa siê w powietrzu.\n\r", PERS( victim, ch ) );
		print_char( victim, "%s trafia ciê, rozp³ywasz siê w powietrzu.\n\r", PERS( ch, victim ) );
		act( "$n trafia, $N rozp³ywa siê w powietrzu.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala iluzja opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemiê.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp³ywa siê w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez ¶ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp³ywa siê pozostawiaj±c tylko chmurê dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy siê przez chwilê, po czym rozpryskujê siê na kwa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p pêka z hukiem i rozpryskuje siê dooko³a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje siê na kawa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}

		extract_char( victim, TRUE );

		return;
	}

	DEBUG_INFO( "onehit_shield:soul" );
	if ( IS_AFFECTED( victim, AFF_SOUL ) )
	{
		print_char( ch, "Twój cios trafia, uwolniona dusza ulatuje do swojego wymiaru.\n\r", PERS( victim, ch ) );
		print_char( victim, "%s trafia ciê, ulatujesz do swojego wymiaru.\n\r", PERS( ch, victim ) );
		act( "$n trafia, dusza $Z ulatuje do swojego wymiaru.", ch, NULL, victim, TO_NOTVICT );

		/* itemy co je trzymala dusza opadaja na ziemie*/
		for ( obj = victim->carrying; obj != NULL; obj = obj_next )
		{
			OBJ_NEXT_CONTENT( obj, obj_next );

			if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
				continue;
			if ( obj->wear_loc != WEAR_NONE )
				unequip_char( victim, obj );

			obj_from_char( obj );
			act( "$p upada na ziemiê.", victim, obj, NULL, TO_ROOM );
			obj_to_room( obj, victim->in_room );

			if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p rozp³ywa siê w powietrzu.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p znika bez ¶ladu.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozp³ywa siê pozostawiaj±c tylko chmurê dymu.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
			else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
			{
				switch ( number_range( 1, 3 ) )
				{
					case 1:
						act( "$p toczy siê przez chwilê, po czym rozpryskujê siê na kwa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
					case 2:
						act( "$p pêka z hukiem i rozpryskuje siê dooko³a.", ch, obj, NULL, TO_ROOM );
						break;
					default:
						act( "$p rozpryskuje siê na kawa³eczki.", ch, obj, NULL, TO_ROOM );
						break;
				}
			}
		}
		extract_char( victim, TRUE );
		return;
	}

	DEBUG_INFO( "onehit_shield:steel_scarfskin" );
		if( is_affected( victim, gsn_steel_scarfskin ) &&
				dam <= number_range( 6, 14 ) )
		{
			scarfskin = affect_find( victim->affected, gsn_steel_scarfskin );

			if ( scarfskin->modifier > 0 )
			{
				scarfskin->modifier -= dam;
				dam = 0;

				print_char( ch, "%s odbija siê jakby¶ uderzy³%s w stal.\n\r", capitalize( shield->short_descr), ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
				print_char( victim, "%s trafia cie, jednak %s %s odskakuje jakby uderzy³%s w stal.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : ch->sex == 0 ? "tego" : "jego", shield->short_descr, ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
				switch ( victim->sex )
				{
					case 0:
						act( "$n trafia $C, jednak $s $p odskakuje jakby $N by³o ze stali.", ch, shield, victim, TO_NOTVICT );
						break;
					case 1:
						act( "$n trafia $C, jednak $s $p odskakuje jakby $N by³ ze stali.", ch, shield, victim, TO_NOTVICT );
						break;
					default :
						act( "$n trafia $C, jednak $s $p odskakuje jakby $N by³a ze stali.", ch, shield, victim, TO_NOTVICT );
						break;
				}
			}
		}

	DEBUG_INFO( "onehit_shield:stone_skin" );
	if ( check_stone_skin( ch, victim, dam_type, &dam, NULL ) )
	{
		if ( dam == 0 )
		{
			print_char( ch, "%s odbija siê jakby¶ uderzy³%s w kamieñ.\n\r", capitalize( shield->short_descr), ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
			print_char( victim, "%s trafia cie, jednak %s %s odskakuje jakby uderzy³%s w kamieñ.\n\r", PERS( ch, victim ), ch->sex == 2 ? "jej" : ch->sex == 0 ? "tego" : "jego", shield->short_descr, ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
			switch ( victim->sex )
			{
				case 0:
					act( "$n trafia $C, jednak $s $p odskakuje jakby $N by³o z kamienia.", ch, shield, victim, TO_NOTVICT );
					break;
				case 1:
					act( "$n trafia $C, jednak $s $p odskakuje jakby $N by³ z kamienia.", ch, shield, victim, TO_NOTVICT );
					break;
				default :
					act( "$n trafia $C, jednak $s $p odskakuje jakby $N by³a z kamienia.", ch, shield, victim, TO_NOTVICT );
					break;
			}
			return;
		}
		else
		{
			print_char( ch, "%s przebija siê przez kamienn± skorê pokrywaj±c± %s.\n\r", capitalize( shield->short_descr), PERS2( victim, ch ) );
			print_char( victim, "%s %s przebija siê przez twoj± kamienn± skórê.\n\r", capitalize( shield->short_descr), PERS2( ch, victim ) );
			act( "Tarcza $z trafia $C przebijaj±c siê przez kammien± skórê.", ch, NULL, victim, TO_NOTVICT );
		}
	}

	DEBUG_INFO( "onehit_shield:magic_weapon" );
	if ( victim->resists[ RESIST_MAGIC_WEAPON ] > 0 && check_magic_attack( ch, victim, shield ) < victim->resists[ RESIST_MAGIC_WEAPON ] )
	{
		if( check_magic_attack( ch, victim, shield ) == 0 && IS_AFFECTED(victim, AFF_RESIST_MAGIC_WEAPON))
		{
			;
		}
		else
		{
			immune = TRUE;
			dam = 0;
		}
	}

	if ( immune || dam == 0 )
	{
		print_char( ch, "Twój atak jest nieskuteczny, cios nic %s nie robi.\n\r", PERS3( victim, ch ) );
		print_char( victim, "Atak %s jest nieskuteczny, %s cios niczego ci nie robi.\n\r", PERS2( ch, victim ), ( ch->sex == 2 ) ? "jej" : "jego" );
		act( "$n trafia, jednak $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit_shield:check_resist" );
	dam = check_resist( victim, dam_type, dam );

#ifdef INFO
	print_char( ch, "DAM -RESISTS: %d\n\r", dam );
#endif

	if ( immune || dam == 0 )
	{
		print_char( ch, "Twój atak jest nieskuteczny, cios nic %s nie robi.\n\r", PERS3( victim, ch ) );
		print_char( victim, "Atak %s jest nieskuteczny, %s cios niczego ci nie robi.\n\r", PERS2( ch, victim ), ch->sex == 2 ? "jej" : "jego" );
		act( "Atak $z nie robi wrazenia na $B, $s cios jest nieskuteczny.", ch, NULL, victim, TO_NOTVICT );
		return;
	}

	DEBUG_INFO( "onehit_shield:increase_wounds" );
	/* spell zwiekszajacy obrazenia slash/pierce/bash */
	check_increase_wounds( victim, dam_type, &dam );

#ifdef INFO
	print_char( ch, "DAM +INCREASE_WOUNDS: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit_shield:troll_power" );
	if ( IS_AFFECTED( victim, AFF_TROLL ) && victim->hit > 0 && victim->hit - dam < 0 )
	{
		dam = victim->hit + 1;
		create_event( EVENT_TROLL_POWER_REGEN, number_range( 3, 5 ) * PULSE_VIOLENCE, victim, NULL, 0 );
	}

	DEBUG_INFO( "onehit_shield:damage_reduction" );


	if ( is_affected(victim,gsn_demon_aura))
	{
        int mod = 14;
		if( IS_GOOD(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura) / mod;
        }
		else if ( !IS_EVIL(ch))
        {
			dam -= get_skill(victim,gsn_demon_aura)/22;
        }
	}

	if ( is_affected(victim,gsn_defense_curl))
	{
		defense_curl = affect_find(victim->affected,gsn_defense_curl );
		defense_curl->modifier -= UMIN( defense_curl->level, dam );
		dam -= UMIN( defense_curl->level, dam );
		if( defense_curl->modifier <= 0 )
		{
			affect_remove( victim, defense_curl );
			act( "Otaczaj±ca ciê ochronna spirala zwija siê, a po chwili zanika.", victim,NULL,NULL,TO_CHAR);
		}
	}

	if ( is_affected( victim, gsn_damage_reduction ) && dam > 0 )
	{
		AFFECT_DATA * pAff;

		if ( ( pAff = affect_find( victim->affected, gsn_damage_reduction ) ) != NULL )
		{
			if ( pAff->level <= 16 )
				dam -= 1;
			else if ( pAff->level <= 20 )
				dam -= 2;
			else if ( pAff->level <= 25 )
				dam -= 3;
			else if ( pAff->level <= 30 )
				dam -= 4;
			else
				dam -= 5;

			dam = UMAX( dam, 1 );
		}
#ifdef INFO
		print_char( ch, "DAM - DAMAGE_REDUCTION: %d\n\r", dam );
#endif

	}

#ifdef INFO
	print_char( ch, "FINAL DAMAGE: %d\n\r", dam );
#endif

	DEBUG_INFO( "onehit_shield:dam_mesg" );
	/* walimy opis ciosu*/
	dam_message( ch, victim, dam, dt, immune );

	DEBUG_INFO( "onehit_shield:raw_damage" );
	raw_damage( ch, victim, dam );

	/* koniec ciekawostek dla umierajacych*/
	if ( ch->fighting != victim || !victim->in_room )
		return;

	DEBUG_INFO( "onehit_shield:check_armor" );
	/* dla tych co przezyli, sprawdzamy armora */
	check_armor_spell( victim, dam );

	if ( shield && HAS_OTRIGGER( shield, TRIG_HIT ) )
		op_hit_trigger( shield, ch );

	tail_chain( );
	return;
}


int get_backstabbed_dam( CHAR_DATA * ch, CHAR_DATA * victim, int old_dam )
{
    int dam = 0, multiplier, skill = get_skill( ch, gsn_backstab ), scinacz = 0, scinacz_mod = 0;
    OBJ_DATA * dagger = get_eq_char(ch, WEAR_WIELD );
    bool check;

    if( !dagger || dagger->item_type != ITEM_WEAPON || dagger->value[0] != WEAPON_DAGGER )
    {
        return old_dam;
    }

	multiplier = (30+skill)/6.5;
	if ( get_curr_stat_deprecated( ch, STAT_STR ) > 27 && stat_throw(ch,STAT_DEX) && number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) > 3 )) ++multiplier;
	if ( get_curr_stat_deprecated( ch, STAT_STR ) > 25 && stat_throw(ch,STAT_DEX) && number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) > 3 )) ++multiplier;
	if ( get_curr_stat_deprecated( ch, STAT_STR ) > 23 && stat_throw(ch,STAT_DEX) && number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) > 3 )) ++multiplier;
	if ( get_curr_stat_deprecated( ch, STAT_STR ) > 20 && stat_throw(ch,STAT_DEX) && number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) > 3 )) ++multiplier;
	if ( get_curr_stat_deprecated( ch, STAT_STR ) > 17 && stat_throw(ch,STAT_DEX) && number_range( 0, get_curr_stat_deprecated(ch,STAT_LUC) > 3 )) ++multiplier;

	//duze w przyblizeniu czlekoksztalne monstery
	if( !str_cmp( race_table[ GET_RACE(victim) ].name, "ogr" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "troll" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "umberkolos" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "yeti" ))
	{
		if ( ch->size == SIZE_SMALL || ch->size == SIZE_TINY )//tacy co nie siegna ogrowi do plecow
			multiplier /= 2;
	}//monstery o wielkim cielsku :P
	else if (!str_cmp( race_table[ GET_RACE(victim) ].name, "wywerna" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "hydra" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "mantykora" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "smok" ) ||
			!str_cmp( race_table[ GET_RACE(victim) ].name, "bazyliszek" ))
	{//za bardzo nie rozni sie bees w takie wielkie cus od zwyklego ciosu - 1/4 dama zwyklego beesa
		multiplier /= 4;
	}

	dam = old_dam * multiplier;

	if( dam > UMAX( 40, ( ch->level - 3 ) * 10 ) )
	{
		scinacz = number_range( (75*UMIN( (2*dam)/5, dam - UMAX( 40, (ch->level-3)*10 )))/100,UMIN( (2*dam)/5, dam - UMAX( 40, (ch->level-3)*10 )) ) ;

		switch( (number_range( 20, get_curr_stat_deprecated(ch,STAT_DEX) + get_curr_stat_deprecated(ch,STAT_INT) + get_curr_stat_deprecated(ch,STAT_LUC) )/4 ) )
		{
			default:
				break;
			case 7:
				scinacz = (scinacz*95)/100;
				break;
			case 8:
				scinacz = (scinacz*9)/10;
				break;
			case 9:
				scinacz = (scinacz*85)/100;
				break;
			case 10:
				scinacz = (scinacz*8)/10;
				break;
			case 11:
				scinacz = (scinacz*75)/100;
				break;
			case 12:
				scinacz = (scinacz*7)/10;
				break;
			case 13:
				scinacz = (scinacz*65)/100;
				break;
			case 14:
				scinacz = (scinacz*6)/10;
				break;
			case 15:
				scinacz = (scinacz*55)/100;
				break;
			case 16:
				scinacz = (scinacz*5)/10;
				break;
			case 17:
				scinacz = (scinacz*45)/100;
				break;
			case 18:
				scinacz = (scinacz*4)/10;
				break;
			case 19:
				scinacz = (scinacz*35)/100;
				break;
			case 20:
				scinacz = (scinacz*3)/10;
				break;
			case 21:
				scinacz = (scinacz*25)/100;
				break;
			case 22:
				scinacz = (scinacz*2)/10;
				break;
		}

        check = FALSE;
        if( (dam - scinacz > 300 && !check ) || dam - scinacz > 350 )
		{
			switch( (dam - scinacz - 300)/10 )
			{
				case 0:
				case 1:
					break;
				case 2:
					scinacz_mod = (scinacz*105)/100;
				case 3:
					scinacz_mod = (scinacz*110)/100;
				case 4:
					scinacz_mod = (scinacz*115)/100;
				case 5:
					scinacz_mod = (scinacz*120)/100;
				case 6:
					scinacz_mod = (scinacz*125)/100;
				case 7:
					scinacz_mod = (scinacz*130)/100;
				case 8:
					scinacz_mod = (scinacz*135)/100;
				case 9:
					scinacz_mod = (scinacz*140)/100;
				case 10:
					scinacz_mod = (scinacz*145)/100;
				case 11:
					scinacz_mod = (scinacz*150)/100;
				case 12:
					scinacz_mod = (scinacz*155)/100;
				case 13:
					scinacz_mod = (scinacz*160)/100;
				case 14:
					scinacz_mod = (scinacz*165)/100;
				case 15:
					scinacz_mod = (scinacz*170)/100;
				case 16:
					scinacz_mod = (scinacz*175)/100;
				case 17:
					scinacz_mod = (scinacz*180)/100;
				case 18:
					scinacz_mod = (scinacz*185)/100;
				case 19:
					scinacz_mod = (scinacz*190)/100;
				case 20:
					scinacz_mod = (scinacz*195)/100;
				default:
					scinacz_mod = scinacz * 2;
			}
			scinacz_mod -= scinacz;

			if( dice(2,get_curr_stat_deprecated(ch,STAT_STR)) >= dice(2,get_curr_stat_deprecated(victim,STAT_CON)))
			{
				switch( (scinacz_mod*100)/dam )
				{
					case 0:
					case 1:
						break;
					case 2:
						--scinacz_mod;
						break;
					case 3:
						scinacz_mod -= scinacz_mod/10;
						break;
					case 4:
						scinacz_mod -= scinacz_mod/9;
						break;
					case 5:
						scinacz_mod -= scinacz_mod/8;
						break;
					case 6:
						scinacz_mod -= scinacz_mod/7;
						break;
					case 7:
						scinacz_mod -= scinacz_mod/6;
						break;
					case 8:
						scinacz_mod -= scinacz_mod/5;
						break;
					case 9:
						scinacz_mod -= scinacz_mod/4;
						break;
					case 10:
						scinacz_mod -= scinacz_mod/3;
						break;
					default:
						scinacz_mod -= scinacz_mod/2;
						break;
				}
			}
			scinacz += scinacz_mod;
		}
		scinacz = UMIN( scinacz, number_range( (40*dam)/10, (6*dam)/10) );
	}

	dam -= scinacz;


    if ( IS_SET( dagger->value[ 4 ], WEAPON_HEARTSEEKER ) )
    {
        dam *= ( 100 + number_range( 1, 5 ) );
        dam /= 100;
    }

	return UMAX( old_dam*3, dam);
}

void remove_mirror( CHAR_DATA *ch, CHAR_DATA *victim, int how_many )
{
	AFFECT_DATA *mirror = NULL;

	if ( !IS_AFFECTED( victim, AFF_MIRROR_IMAGE ) )
    {
		return;
    }
	if ( ( mirror = affect_find( victim->affected, gsn_mirror_image ) ) == NULL )
    {
		return;
    }
	if ( mirror->modifier > 0 )
	{
		print_char( ch, "Jedno z lustrzanych odbiæ %s zniknê³o.\n\r", PERS2( victim, ch ) );
		send_to_char( "Jedno z twoich odbiæ zniknê³o.\n\r", victim );

		if ( is_affected(victim, gsn_mirrorfall) )
        {
			affect_strip(victim, gsn_mirrorfall);
        }
	}
	mirror->modifier -= UMAX( 1, how_many );
	if ( mirror->modifier <= 0 )
	{
		if ( mirror->type > 0 && skill_table[ mirror->type ].msg_off )
		{
			send_to_char( skill_table[ mirror->type ].msg_off, victim );
			send_to_char( "\n\r", victim );
		}
		affect_remove( victim, mirror );
	}
	return;
}

int	check_critical_hit( CHAR_DATA *ch, CHAR_DATA *victim, int *diceroll, int *dt, OBJ_DATA *wield, int *base_dam, int old_dam, int *twohander_fight_style, int *weapon_mastery, bool *critical )
{
    int chance = 0, multiplier = 1, new_dam;

    if ( *dt == gsn_charge )
    {
        new_dam = (*base_dam + old_dam)/2;//old dam tym sie tylko rozni od base, ze sa wliczone premie za sharp/vorpal
        new_dam *= 2;
        return new_dam;
    }

    //DRAKE: NIE ZMIENIAC DO KRWY NEDZY BEZ UZGODNIENIA, BO NOGI Z DUPY POWYRYWAM. ZE KURWA SWORD Z WIEKSZA SZANSA KRYTYKA MA SZANSE JAK AXE? KTO OCIPIAL?!
    if( wield->item_type == ITEM_WEAPON )
    {
        switch( wield->value[0] )
        {
            case WEAPON_EXOTIC:
                chance = 1;
                multiplier = 2;
                break;

            case WEAPON_FLAIL:
            case WEAPON_MACE: //Dwureczna bron tego typu ma szanse na mocniejsze krytyki
                chance = 1;
                if( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) )
                {
                    multiplier = number_range(2,3);
                }
                else
                {
                    multiplier = 2;
                }
                break;
            case WEAPON_POLEARM:
            case WEAPON_SPEAR:
            case WEAPON_AXE:
            case WEAPON_CLAWS:
                chance = 1;
                multiplier = 3;
                break;

            case WEAPON_STAFF:
            case WEAPON_SWORD:
            case WEAPON_DAGGER:
            case WEAPON_WHIP:
            case WEAPON_SHORTSWORD:
                chance = 2;
                multiplier = 2;
                break;
        }
    }

    if ( IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) )
    {
        if ( number_percent() < *twohander_fight_style ) ++chance;
    }
    else
    {
        if ( number_percent() < *weapon_mastery ) ++chance;
    }


    if ( is_affected(ch,gsn_wardance) && ( number_percent() < get_skill(ch,gsn_wardance)) )
    {
        ++chance;
    }

    if ( is_affected(ch,gsn_berserk) && ( number_percent() < get_skill(ch,gsn_berserk)) )
    {
        ++chance;
    }

    if ( IS_AFFECTED(ch,AFF_HALLUCINATIONS_POSITIVE) )
    {
        ++chance;
    }

    if ( IS_WEAPON_STAT( wield, WEAPON_VORPAL )  )
    {
        chance += number_range(0,1);
    }

    if ( IS_WEAPON_STAT( wield, WEAPON_KEEN ) )
    {
        ++chance;
    }

    if ( IS_SET( wield->value[ 4 ], WEAPON_HEARTSEEKER ) )
    {
        ++chance;
    }

    if ( IS_AFFECTED(ch, AFF_EYES_OF_THE_TORTURER) )
    {
        chance *= 2;
    }

    if ( IS_WEAPON_STAT( wield, WEAPON_THUNDERING ) )
    {
        ++multiplier;
    }

    if ( IS_AFFECTED(ch, AFF_SPIRITUAL_GUIDANCE) )
    {
        multiplier += number_range(0,1);
    }

    if( *diceroll >= (20 - chance) || IS_SET( ch->fight_data, FIGHT_THOUSANDSLAYER ) || IS_SET( ch->fight_data, FIGHT_CRITICAL_STRIKE ))
    {
        new_dam = *base_dam;
        new_dam *= multiplier;
        *critical = TRUE;
        if ( IS_SET( ch->fight_data, FIGHT_CRITICAL_STRIKE ) )
        {
            REMOVE_BIT( ch->fight_data, FIGHT_CRITICAL_STRIKE );
        }
    }
    else
    {
        new_dam = old_dam;
        *critical = FALSE;
    }
    return new_dam;
}

int one_hit_check_mastery( CHAR_DATA *ch, CHAR_DATA *victim, int gsn, int check_improve_level )
{
    if ( get_skill( ch, gsn ) )
    {
        check_improve( ch, victim, gsn, TRUE, check_improve_level );
        return get_skill( ch, gsn );
    }
    return 0;
}

