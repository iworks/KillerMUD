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
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: event.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/event.c $
 *
 */
#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"

long int money_count_copper_all  args ( ( CHAR_DATA *ch ) );
void money_reset_character_money args ( ( CHAR_DATA *ch ) );

EVENT_DATA * event_first = NULL;
EVENT_DATA * event_free = NULL;


EVENT_DATA * create_event( int type, int time, void * arg1, void * arg2, int var )
{
	EVENT_DATA * event;

	if ( event_free )
	{
		event       = event_free;
		event_free  = event->next;
		event->next = NULL;
	}
	else
	{
		CREATE( event, EVENT_DATA, 1 );
		top_event++;
	}

	memset( event, 0, sizeof( EVENT_DATA ) );

	event->type    = type;
	event->time    = time;
	event->arg1    = arg1;
	event->arg2    = arg2;
	event->var     = var;
	event->deleted = FALSE;
	event->next    = event_first;
	event_first    = event;

	return event;
}

void remove_event( int type, void * arg1, void * arg2, int var )
{
	EVENT_DATA *event = NULL;
	EVENT_DATA *prev = NULL;
	EVENT_DATA *next = NULL;

	for ( event = event_first; event; event = next )
	{
		next = event->next;

		if ( event->type == type && event->arg1 == arg1 && event->arg2 == arg2 && event->var == var )
		{
			if ( event_first == event )
				event_first = event->next;
			if ( prev )
				prev->next = event->next;
			event->next = event_free;
			event_free = event;
			continue;
		}
		else
			prev = event;
	}

	return;
}

void event_light_nova( EVENT_DATA * ev )
{
  int add_power = 0;
  LIGHT_NOVA_DATA * data = (LIGHT_NOVA_DATA *) ev->arg1;
  CHAR_DATA * caster = data->caster;

  /* magic ball need be present and to match */
  OBJ_DATA * ball = get_eq_char(caster, WEAR_FLOAT);
  if (ball != data->ball)
    {
      ev->deleted = TRUE;
      return;
    }

  int break_spell = FALSE;
  /* room must match... or not. */
  if (!data->component && !data->specialist)
    {
      if (data->cast_room != caster->in_room)
        {
          act( "Magiczna kula nagle zaczyna bardzo puchnąć. Po chwili staje się niemal przeźroczysta i wydawszy z siebie ciche puknięcie - znika zupełnie.", caster, NULL, NULL, TO_ALL );
          break_spell = TRUE;
        }
    }

  /* darkness spell breaks the nova */
  if ( EXT_IS_SET( caster->in_room->room_flags, ROOM_MAGICDARK ) )
    {
      act( "Ciemność wypełniająca pomieszczenie błyskawicznie wysysa całą moc z kuli magicznego światła sprawiając, że znika ona bez śladu.", caster, NULL, NULL, TO_ALL );
      break_spell = TRUE;
    }


  if (break_spell)
    {
      // remove the magic ball
      extract_obj( ball );
      ev->deleted = TRUE;
      return;
    }
  
  /* suck the light from room */  
  /* room_is_dark */
  if (!room_is_dark(NULL, caster->in_room))
    {
      add_power += 3;
    }
  
  add_power += caster->in_room->light; // bonusowe punkty za każde źrodło światła
  data->power += add_power;

  if (add_power > 10)
    {
      data->countdown--;
    }

  data->countdown--;

  /* boom? */
  if (data->countdown <= 0)
    {
      // damage everyone except for the caster
      act( "Kula światła wybucha raniąc wszystkich dookoła magicznym światłem!", caster, NULL, NULL, TO_ALL );

//        act( "{5Rozpylasz w kierunku $Z drobne cząsteczki wszystkich żywiołów!{x", ch, NULL, victim, TO_CHAR );
//        act( "{5$n rozpyla w twoim kierunku drobne cząsteczki wszystkich żywiołów!{x", ch, NULL, victim, TO_VICT );
//        act( "{5$n rozpyla drobne cząsteczki wszystkich żywiołów w kierunku $Z!{x", ch, NULL, victim, TO_NOTVICT );

      int base_damage = dice( data->power, 2 + data->specialist + data->component + data->spell_level_bonus);
      base_damage = luck_dam_mod( caster, base_damage );
      
      CHAR_DATA * victim;
      for(victim = caster->in_room->people; victim; victim = victim->next_in_room)
        {
          

          int dam = base_damage;
          if ( is_same_group( caster, victim ) )
            dam /= 3;

          // caster mniej obrywa
          if ( victim == caster )
            dam /= 2;

          // niewielki bonus przeciwko undeadom
          if ( is_undead( victim ) )
            dam = number_range( dam * 105, dam * 120 ) / 100;

          if ( saves_spell_new( victim, skill_table[ data->sn ].save_type, skill_table[ data->sn ].save_mod, caster, data->sn ) )
            {
              dam /= 2;
            }
          else if ( number_range(1, 4) == 1 && !IS_AFFECTED( victim,AFF_PERFECT_SENSES ) && !is_same_group( caster, victim ) )
            {
              AFFECT_DATA af;
              af.where = TO_AFFECTS;
              af.type = data->sn;
              af.level = data->level;
              af.duration = dice(1,4);
              af.rt_duration = 0;
              af.location = APPLY_NONE;
              af.modifier = 0;
              af.bitvector = &AFF_BLIND;
              affect_to_char( victim, &af, NULL, TRUE );
              
              send_to_char( "Zostałeś oślepiony!\n\r", victim );
              act( "$n wygląda na oślepion$t.", victim, victim->sex == SEX_FEMALE ? "ą" : "ego", NULL, TO_ROOM );
            }
          
          spell_damage( caster, victim, dam, data->sn, DAM_LIGHT , FALSE );
        }

      // remove the magic ball
      extract_obj( ball );
      ev->deleted = TRUE;
      return;
    }
  else
    {
      if (add_power)
        {
          act( "Okoliczne światła przygasają gdy magiczna kula pochłania ich świaŧło.", caster, NULL, NULL, TO_ALL );
        }
      ev->time = (number_range(10,25) * PULSE_VIOLENCE) / 10;
    }
}

void event_update( )
{
	ROOM_INDEX_DATA * room;
	CHAR_DATA *ch, *tch, *victim_table[ 5 ], *victim, *vch_next;
	EVENT_DATA *event = NULL;
	EVENT_DATA *prev = NULL;
	EVENT_DATA *next = NULL;
	OBJ_DATA *obj, *obj2, *obj_next;
	EXIT_DATA *pexit;
	int i, dam, fruit_count, lightning_count, lava_count, magic_level, blade_count, shots, sn;

	for ( event = event_first; event; event = next )
	{
		next = event->next;
		if ( event->deleted )
		{
			if ( event_first == event )
				event_first = event->next;
			if ( prev )
				prev->next = event->next;
			event->next = event_free;
			event_free = event;
			continue;
		}
		else
			prev = event;
		event->time -= 1;

		if ( event->time <= 0 )
		{
			switch ( event->type )
			{
                                case EVENT_LIGHT_NOVA:
                                  event_light_nova( event );
                                  if (event->deleted)
                                    {
                                      free((LIGHT_NOVA_DATA *) event->arg1);
                                    }
                                  break;

				case EVENT_TREE_FRUITS:
					event->deleted = TRUE;
					//					ch = event->arg2;
					obj = NULL;
					//					fruit_count = UMAX( 1, number_range(4, 3));
					fruit_count = UMAX( 1, number_range(event->var-5,event->var+5));
					if ( (obj = event->arg1) == NULL )
						break;

					if ( obj->pIndexData->vnum != OBJ_VNUM_TREE1 &&
							obj->pIndexData->vnum != OBJ_VNUM_TREE2 &&
							obj->pIndexData->vnum != OBJ_VNUM_TREE3 )
						break;

					switch ( obj->pIndexData->vnum )
					{
						case OBJ_VNUM_TREE1:
							for ( ; fruit_count > 0; fruit_count-- )
							{
								obj2 = create_object( get_obj_index( OBJ_VNUM_TREE1_FRUIT ), FALSE);
								obj_to_obj( obj2, obj );
							}
							break;
						case OBJ_VNUM_TREE2:
							for ( ; fruit_count > 0; fruit_count-- )
							{
								obj2 = create_object( get_obj_index( OBJ_VNUM_TREE2_FRUIT ), FALSE);
								obj_to_obj( obj2, obj );
							}
							break;
						case OBJ_VNUM_TREE3:
							for ( ; fruit_count > 0; fruit_count-- )
							{
								obj2 = create_object( get_obj_index( OBJ_VNUM_TREE3_FRUIT ), FALSE);
								obj_to_obj( obj2, obj );
							}
							break;
					}
					for ( victim = obj->in_room->people; victim; victim = vch_next )
					{
						vch_next = victim->next_in_room;
						act( "Na $k wyrasta więcej owoców.", victim, obj, NULL, TO_CHAR );
					}
					break;

				case EVENT_TREE_EXTRACT:
					event->deleted = TRUE;
					obj = NULL;

					if ( (obj = event->arg1) == NULL )
						break;

					if ( obj->pIndexData->vnum != OBJ_VNUM_TREE1 &&
							obj->pIndexData->vnum != OBJ_VNUM_TREE2 &&
							obj->pIndexData->vnum != OBJ_VNUM_TREE3 )
						break;

					for ( victim = obj->in_room->people; victim; victim = vch_next )
					{
						vch_next = victim->next_in_room;
						act( "$p powoli więdnie, po czym rozsypuje się w proch.", victim, obj, NULL, TO_CHAR );
					}

					DEBUG_INFO( "extract_events:pre" );
					extract_obj( obj );
					DEBUG_INFO( "extract_events:post" );
					break;

				case EVENT_EXTRACT_CHAR:
					event->deleted = TRUE;
					ch = NULL;

					if ( !( ch = ( CHAR_DATA * ) event->arg1 ) )
						break;

					stop_fighting( ch, TRUE );

					if( IS_NPC(ch) && ( ch->pIndexData->vnum == MOB_VNUM_GOLEM ||
								ch->pIndexData->vnum == MOB_VNUM_LIVEOAK ||
								ch->pIndexData->vnum == MOB_VNUM_BEAR ||
								ch->pIndexData->vnum == MOB_VNUM_WOLF ||
								ch->pIndexData->vnum == MOB_VNUM_FETCH ||
								ch->pIndexData->vnum == MOB_VNUM_SNAKE ) )
					{
						for ( obj = ch->carrying; obj != NULL; obj = obj_next )
						{
							OBJ_NEXT_CONTENT( obj, obj_next );

							if ( IS_OBJ_STAT( obj, ITEM_NODROP ) )
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
										act( "$p rozpływa się w powietrzu.", ch, obj, NULL, TO_ROOM );
										break;
									case 2:
										act( "$p znika bez śladu.", ch, obj, NULL, TO_ROOM );
										break;
									default:
										act( "$p rozpływa się pozostawiając tylko chmurę dymu.", ch, obj, NULL, TO_ROOM );
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
										act( "$p toczy się przez chwilę, po czym rozpryskuję się na kwałeczki.", ch, obj, NULL, TO_ROOM );
										break;
									case 2:
										act( "$p pęka z hukiem i rozpryskuje się dookoła.", ch, obj, NULL, TO_ROOM );
										break;
									default:
										act( "$p rozpryskuje się na kawałeczki.", ch, obj, NULL, TO_ROOM );
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
							append_file_format_daily( ch, MONEY_LOG_FILE,
									"-> S: %d %d (%d), B: %d %d (%d) - quit yes",
									copper, 0, -copper,
									ch->bank,
									ch->bank,
                                    0
                                    );
							if ( money_count_copper_all( ch ) > 1) act( "Kupka monet upada na ziemię.", ch, NULL, NULL, TO_ROOM );
							else act( "Jedna moneta upada na ziemię.", ch, NULL, NULL, TO_ROOM );
                            money_reset_character_money( ch );
						}
					}

					if ( ch->in_room )
                    {
						act( "$n znika.", ch, NULL, NULL, TO_ROOM );
                    }

					DEBUG_INFO( "extract_events:pre" );
					extract_char( ch, TRUE );
					DEBUG_INFO( "extract_events:post" );
					break;

				case EVENT_EXTRACT_SOUL:
					event->deleted = TRUE;
					ch = NULL;

					if ( !( ch = ( CHAR_DATA * ) event->arg1 ) )
						break;

					stop_fighting( ch, TRUE );

					if ( ch->in_room )
						act( "$n zrywa się z uwięzi i ulatuje do swojego wymiaru.", ch, NULL, NULL, TO_ROOM );

					DEBUG_INFO( "extract_events:pre" );
					extract_char( ch, TRUE );
					DEBUG_INFO( "extract_events:post" );
					break;

				case EVENT_CALL_LIGHTNING:
					/* licznik blyskawic i poziom magii */
					lightning_count = (--event->var) % 100;
					magic_level = event->var / 100;
					if ( lightning_count <= 0 )
						event->deleted = TRUE;

					ch = NULL;
					room = NULL;

					if ( !( ch = ( CHAR_DATA * ) event->arg1 ) )
					{
						event->deleted = TRUE;
						break;
					}

					if ( !( room = ( ROOM_INDEX_DATA * ) event->arg2 ) )
					{
						event->deleted = TRUE;
						break;
					}

					if ( !ch || !ch->in_room )
					{
						event->deleted = TRUE;
						break;
					}

					if ( ch->in_room != room )
					{
						event->time = number_range( 2, 3 ) * PULSE_VIOLENCE;
						break;
					}

					for ( i = 0; i < 5; i++ )
						victim_table[ i ] = NULL;

					i = 0;
					for ( tch = ch->in_room->people; tch && i < 5; tch = tch->next_in_room )
					{
						if ( tch->fighting && is_same_group( tch->fighting, ch ) )
							victim_table[ i++ ] = tch;
					}

					if ( i == 0 )
					{
						event->deleted = TRUE;
						break;
					}
					//Raszer -poprzedni dmg
					//dam = number_range(25,35) + number_range(magic_level/2, magic_level) + dice( URANGE( 1, ch->level, 31 ), 5 );

					dam = number_range(magic_level/2, magic_level) + number_range(magic_level/2, magic_level) + dice( URANGE( 1, (ch->level)/2,LEVEL_HERO ), 5 ); //Raszer - sciecie dmg zadawanego przez call
                    dam = URANGE(35,dam,100);
					dam = luck_dam_mod( ch, dam );

					victim = victim_table[ number_range( 0, i - 1 ) ];

					if ( saves_spell_new( victim, skill_table[gsn_call_lightning].save_type, skill_table[gsn_call_lightning].save_mod, ch, gsn_call_lightning ) )
						dam /= 2;

					if ( weather_info[ victim->in_room->sector_type ].sky >= SKY_RAINING )
						dam += dam/3;

					if ( weather_info[ victim->in_room->sector_type ].sky >= SKY_LIGHTNING )
						dam += dam/10;

					spell_damage( ch, victim, dam, gsn_call_lightning, DAM_LIGHTNING , TRUE );
					shock_effect( victim, magic_level, dam, TARGET_CHAR );
					event->time = number_range( 2, 3 ) * PULSE_VIOLENCE;
					break;


				case EVENT_SUMMON_LESSER_METEOR:
					if ( --event->var <= 0 )
						event->deleted = TRUE;

					ch = NULL;
					room = NULL;

					if ( !( ch = ( CHAR_DATA * ) event->arg1 ) )
					{
						event->deleted = TRUE;
						break;
					}

					if ( !( room = ( ROOM_INDEX_DATA * ) event->arg2 ) )
					{
						event->deleted = TRUE;
						break;
					}

					if ( !ch || !ch->in_room )
					{
						event->deleted = TRUE;
						break;
					}

					if ( ch->in_room != room )
					{
						event->time = number_range( 2, 3 ) * PULSE_VIOLENCE;
						break;
					}

					for ( i = 0; i < 5; i++ )
						victim_table[ i ] = NULL;

					i = 0;
					for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
						if ( tch->fighting && is_same_group( tch->fighting, ch ) )
						{
							victim_table[ i++ ] = tch;
						}

					if ( i == 0 )
					{
						event->deleted = TRUE;
						break;
					}

					dam = 50 + dice( URANGE( 1, ch->level*3/4, 30 ), 5 );

					dam = luck_dam_mod( ch, dam );

					victim = victim_table[ number_range( 0, i - 1 ) ];

					spell_damage( ch, victim, dam, gsn_summon_lesser_meteor, DAM_BASH , TRUE );
					victim->position = POS_SITTING;
					WAIT_STATE( victim, 3 * PULSE_VIOLENCE );
					event->time = number_range( 2, 3 ) * PULSE_VIOLENCE;
					break;


				case EVENT_SUMMON_GREATER_METEOR:
					if ( --event->var <= 0 )
						event->deleted = TRUE;

					ch = NULL;
					room = NULL;

					if ( !( ch = ( CHAR_DATA * ) event->arg1 ) )
					{
						event->deleted = TRUE;
						break;
					}

					if ( !( room = ( ROOM_INDEX_DATA * ) event->arg2 ) )
					{
						event->deleted = TRUE;
						break;
					}

					if ( !ch || !ch->in_room )
					{
						event->deleted = TRUE;
						break;
					}

					if ( ch->in_room != room )
					{
						event->time = number_range( 2, 3 ) * PULSE_VIOLENCE;
						break;
					}

					for ( i = 0; i < 5; i++ )
						victim_table[ i ] = NULL;

					i = 0;
					for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
						if ( tch->fighting && is_same_group( tch->fighting, ch ) )
						{
							victim_table[ i++ ] = tch;
						}

					if ( i == 0 )
					{
						event->deleted = TRUE;
						break;
					}

					dam = number_range(45, 65) + dice( ch->level, 6 );

					dam = luck_dam_mod( ch, dam );

					victim = victim_table[ number_range( 0, i - 1 ) ];

					spell_damage( ch, victim, dam, gsn_summon_greater_meteor, DAM_BASH , TRUE );
					victim->position = POS_SITTING;
					WAIT_STATE( victim, 3 * PULSE_VIOLENCE );
					event->time = number_range( 2, 3 ) * PULSE_VIOLENCE;
					break;


				case EVENT_TROLL_POWER_REGEN:
					event->deleted = TRUE;
					ch = NULL;

					if ( !( ch = ( CHAR_DATA * ) event->arg1 ) )
					{
						event->deleted = TRUE;
						break;
					}

					if ( ch->hit <= 0 )
						ch->hit = number_range( 20, 45 ) * get_max_hp(ch) / 100;

					break;

				case EVENT_DARKNESS_END:
					//					bugf ("Event darkness end [room: %d].", room->vnum);
					log_string( "Event darkness end " );
					event->deleted = TRUE;
					room = NULL; /* hmm a bez tego nie bedzie dzialac? */

					if ( !( room = ( ROOM_INDEX_DATA * ) event->arg1 ) )
						break;

					if ( EXT_IS_SET( room->room_flags, ROOM_MAGICDARK ) )
					{
						EXT_REMOVE_BIT( room->room_flags, ROOM_MAGICDARK );
						if ( room->people )
							act( "Mrok opuszcza to miejsce.", room->people, NULL, NULL, TO_ALL );
					}
					break;

				case EVENT_SLIPPERY_FLOOR_END:
					log_string( "Event slippery floor end " );
					event->deleted = TRUE;
					room = NULL;

					if ( !( room = ( ROOM_INDEX_DATA * ) event->arg1 ) )
						break;

					if ( EXT_IS_SET( room->room_flags, ROOM_SLIPPERY_FLOOR ) )
					{
						EXT_REMOVE_BIT( room->room_flags, ROOM_SLIPPERY_FLOOR );
						if ( room->people )
							act( "Śliska i tłusta substancja pokrywająca podłoże wsiąka w nie.", room->people, NULL, NULL, TO_ALL );
					}
					break;

				case EVENT_WALL_OF_MIST_END:
					log_string( "Event wall of mist end " );
					event->deleted = TRUE;
					room = NULL;

					if ( !( room = ( ROOM_INDEX_DATA * ) event->arg1 ) )
						break;

					pexit = room->exit[ event->var ];

					if ( IS_SET( pexit->exit_info, EX_WALL_OF_MIST ) )
					{
						REMOVE_BIT( pexit->exit_info, EX_WALL_OF_MIST );
						if ( room->people )
							act( "Chmura mgły rozwiewa się.", room->people, NULL, NULL, TO_ALL );
					}
					break;

				case EVENT_LAVA:
					lava_count = (--event->var) % 100;
					magic_level = event->var / 100;
					if ( lava_count <= 0 )
						event->deleted = TRUE;
					ch = NULL;
					room = NULL;

					if ( !( ch = ( CHAR_DATA * ) event->arg1 ) )
					{
						event->deleted = TRUE;
						break;
					}

					if ( !( room = ( ROOM_INDEX_DATA * ) event->arg2 ) )
					{
						event->deleted = TRUE;
						break;
					}

					if ( !ch || !ch->in_room )
					{
						event->deleted = TRUE;
						break;
					}

					if ( ch->in_room != room )
					{
						event->time = number_range( 2, 3 ) * PULSE_VIOLENCE;
						break;
					}

					for ( i = 0; i < 5; i++ )
						victim_table[ i ] = NULL;

					i = 0;
					for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
						if ( tch->fighting && is_same_group( tch->fighting, ch ) )
						{
							victim_table[ i++ ] = tch;
						}

					if ( i == 0 )
					{
						event->deleted = TRUE;
						break;
					}

					dam = number_range(magic_level,magic_level+10) + dice(URANGE(1,magic_level/10,3)+(magic_level/31),10) + 5;
					dam = URANGE(25,dam,70);//Raszer, lekkie przyciecie maks dmg lava_bolt

					dam = luck_dam_mod( ch, dam );


					victim = victim_table[ number_range( 0, i - 1 ) ];

					if ( saves_spell_new( victim, skill_table[gsn_lava_bolt].save_type, skill_table[gsn_lava_bolt].save_mod, ch, gsn_lava_bolt ) )
						dam /= 2;

					spell_damage( ch, victim, dam, gsn_lava_bolt, DAM_FIRE , TRUE );
					//2008-11-20, Brohacz: lava bolt nie palila gratow. Swoja droga - fajna opcja TARGET_ROOM :)
					fire_effect( victim, ch->level, 3*dam/4, TARGET_CHAR );
					fire_effect( victim, ch->level, dam/4, TARGET_ROOM );
					event->time = number_range( 2, 3 ) * PULSE_VIOLENCE;
					break;

				case EVENT_RUFF:
					event->deleted = FALSE;

					int healing_rate = event->var;
					OBJ_DATA *obj_next;
					ROOM_INDEX_DATA *room = event->arg2;

					if ( ( obj = event->arg1 ) == NULL )
						break;

					for ( obj = room->contents; obj != NULL; obj = obj_next )
					{
						if ( obj->pIndexData->vnum == OBJ_VNUM_RUFF1 )
						{
							extract_obj( obj );
							obj = create_object( get_obj_index( OBJ_VNUM_RUFF2 ), FALSE );
							obj_to_room( obj, room );
							for ( victim = room->people; victim; victim = vch_next )
							{
								vch_next = victim->next_in_room;
								act( "Z ziemi wyrasta krzak_2.", victim, obj, NULL, TO_CHAR );
								event->time = number_range( 1, 1 ) * PULSE_VIOLENCE;
							}
							break;
						}

						if ( obj->pIndexData->vnum == OBJ_VNUM_RUFF2 )
						{
							if ( number_range( 1, 20 ) == 1 )
							{
								extract_obj( obj );
								obj = create_object( get_obj_index( OBJ_VNUM_RUFF31 ), FALSE );
								obj_to_room( obj, room );
								for ( victim = room->people; victim; victim = vch_next )
								{
									vch_next = victim->next_in_room;
									act( "Z ziemi wyrasta krzak_31.", victim, obj, NULL, TO_CHAR );
									event->time = number_range( 1, 1 ) * PULSE_VIOLENCE;
								}
							}

							else
							{
								extract_obj( obj );
								obj = create_object( get_obj_index( OBJ_VNUM_RUFF3 ), FALSE );
								obj_to_room( obj, room );
								for ( victim = room->people; victim; victim = vch_next )
								{
									vch_next = victim->next_in_room;
									act( "Z ziemi wyrasta krzak_3.", victim, obj, NULL, TO_CHAR );
									event->time = number_range( 1, 1 ) * PULSE_VIOLENCE;
								}
							}
							break;
						}

						if ( obj->pIndexData->vnum == OBJ_VNUM_RUFF3 )
						{
							extract_obj( obj );
							for ( victim = room->people; victim; victim = vch_next )
							{
								vch_next = victim->next_in_room;
								act( "Kasujemy krzak 3.", victim, obj, NULL, TO_CHAR );
								room->heal_rate = healing_rate;
							}
							break;
						}

						if ( obj->pIndexData->vnum == OBJ_VNUM_RUFF31 )
						{
							extract_obj( obj );
							for ( victim = room->people; victim; victim = vch_next )
							{
								vch_next = victim->next_in_room;
								act( "Kasujemy krzak 31.", victim, obj, NULL, TO_CHAR );
							}
							break;
						}

						if ( obj->pIndexData->vnum == OBJ_VNUM_RUFF4 )
						{
							extract_obj( obj );
							obj = create_object( get_obj_index( OBJ_VNUM_RUFF5 ), FALSE );
							obj_to_room( obj, room );
							for ( victim = room->people; victim; victim = vch_next )
							{
								vch_next = victim->next_in_room;
								act( "Z ziemi wyrasta krzak_5.", victim, obj, NULL, TO_CHAR );
								event->time = number_range( 1, 1 ) * PULSE_VIOLENCE;
							}
							break;
						}

						if ( obj->pIndexData->vnum == OBJ_VNUM_RUFF5 )
						{
							if ( number_range( 1, 4 ) == 1 )
							{
								extract_obj( obj );
								obj = create_object( get_obj_index( OBJ_VNUM_RUFF61 ), FALSE );
								obj_to_room( obj, room );
								for ( victim = room->people; victim; victim = vch_next )
								{
									vch_next = victim->next_in_room;
									act( "Z ziemi wyrasta krzak_61.", victim, obj, NULL, TO_CHAR );
									event->time = number_range( 1, 1 ) * PULSE_VIOLENCE;
								}
							}

							else
							{
								extract_obj( obj );
								obj = create_object( get_obj_index( OBJ_VNUM_RUFF6 ), FALSE );
								obj_to_room( obj, room );
								for ( victim = room->people; victim; victim = vch_next )
								{
									vch_next = victim->next_in_room;
									act( "Z ziemi wyrasta krzak_6.", victim, obj, NULL, TO_CHAR );
									event->time = number_range( 1, 1 ) * PULSE_VIOLENCE;
								}
							}
							break;
						}

						if ( obj->pIndexData->vnum == OBJ_VNUM_RUFF6 )
						{
							extract_obj( obj );
							for ( victim = room->people; victim; victim = vch_next )
							{
								vch_next = victim->next_in_room;
								act( "Kasujemy krzak_6.", victim, obj, NULL, TO_CHAR );
							}
							break;
						}

						if ( obj->pIndexData->vnum == OBJ_VNUM_RUFF61 )
						{
							extract_obj( obj );
							for ( victim = room->people; victim; victim = vch_next )
							{
								vch_next = victim->next_in_room;
								act( "Kasujemy krzak_61.", victim, obj, NULL, TO_CHAR );
							}
							break;
						}

						else
						{
							event->deleted = TRUE;
							break;
						}

					}
					break;

				case EVENT_CONSECRATE_END:
					log_string( "Event consecrate end." );
					event->deleted = TRUE;
					room = NULL; /* hmm a bez tego nie bedzie dzialac? */
					if ( !( room = ( ROOM_INDEX_DATA * ) event->arg1 ) )
						break;
					if ( EXT_IS_SET( room->room_flags, ROOM_CONSECRATE) )
					{
						EXT_REMOVE_BIT( room->room_flags, ROOM_CONSECRATE );
						if ( room->people )
							act( "Siły dobra przestają zwracać szczególną uwagę na to miejsce.", room->people, NULL, NULL, TO_ALL );
					}
					break;

				case EVENT_DESECRATE_END:
					log_string( "Event desecrate end." );
					event->deleted = TRUE;
					room = NULL; /* hmm a bez tego nie bedzie dzialac? */
					if ( !( room = ( ROOM_INDEX_DATA * ) event->arg1 ) )
						break;
					if ( EXT_IS_SET( room->room_flags, ROOM_DESECRATE ) )
					{
						EXT_REMOVE_BIT( room->room_flags, ROOM_DESECRATE );
						if ( room->people )
							act( "Siły zła przestają zwracać szczególną uwagę na to miejsce.", room->people, NULL, NULL, TO_ALL );
					}
					break;

					case EVENT_BLADE:
					blade_count = (--event->var) % 100;
					shots = (--event->var) % 100;
					int chance;
					if ( blade_count <= 0 )
					event->deleted = TRUE;
					ch = NULL;
					room = NULL;

					if ( !( ch = ( CHAR_DATA * ) event->arg1 ) )
					{
						event->deleted = TRUE;
						break;
					}

					if ( !( room = ( ROOM_INDEX_DATA * ) event->arg2 ) )
					{
						event->deleted = TRUE;
						break;
					}

					if ( !ch || !ch->in_room )
					{
						event->deleted = TRUE;
						break;
					}

					if ( ch->in_room != room )
					{
						event->time = number_range( 2, 3  ) * PULSE_VIOLENCE;
						break;
					}

					for ( i = 0; i < 5; i++ )
						victim_table[ i ] = NULL;

					i = 0;
					for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
						if ( tch->fighting && is_same_group( tch->fighting, ch ) )
						{
							victim_table[ i++ ] = tch;
						}

					if ( i == 0 )
					{
						event->deleted = TRUE;
						break;
					}



					victim = victim_table[ number_range( 0, i - 1 ) ];


                    sn = skill_lookup( "rain of blades" );
                    for ( ; shots > 0; shots-- )
					{
							if ( !victim->in_room || ch->in_room != victim->in_room )
							{
						event->deleted = TRUE;
						break;
							}
							if (shots < 0)
							{
						event->deleted = TRUE;
						break;
							}
							victim = victim_table[ number_range( 0, i - 1 ) ];
							chance = (ch->level /3+ dice(1,12)) + (GET_AC( victim, AC_PIERCE ) / 10);
							if (chance <5)
							{
							dam = 0;
							spell_damage( ch, victim, dam, sn, DAM_PIERCE , FALSE );
							act( "Magiczne ostrze uderzając w $z rozpryskuje się na kawałeczki.", victim, NULL, NULL, TO_ROOM );
							send_to_char( "Przed tobą formuje się magiczne ostrze które uderza w ciebie, nie robiąc ci żadnej krzywdy.\n\r", victim );
							}
							else if (chance < 9)
							{
							dam = dice( 1, 3 ) + 1;
							dam = luck_dam_mod( ch, dam );
							spell_damage( ch, victim, dam, sn, DAM_PIERCE , FALSE );
							act( "Z powietrza formuję się niewielkie ostrze z energi i uderza w $z.", victim, NULL, NULL, TO_ROOM );
							send_to_char( "Przed tobą formuje się niewielkie ostrze z energi i uderza w ciebie, muskając.\n\r", victim );
							}
							else if (chance < 15)
							{
							dam = dice( 2, 3 ) + 1;
							dam = luck_dam_mod( ch, dam );
							spell_damage( ch, victim, dam, sn, DAM_PIERCE , FALSE );
							act( "Z powietrza formuję się niewielkie magiczne ostrze i uderza prosto w $z.", victim, NULL, NULL, TO_ROOM );
							send_to_char( "Z powietrza formuje się niewielkie magiczne ostrze i uderza w ciebie, raniąc lekko.\n\r", victim );
							}
							else if (chance < 20)
							{
							dam = dice( 3, 3 ) + 2;
							dam = luck_dam_mod( ch, dam );
							spell_damage( ch, victim, dam, sn, DAM_PIERCE , FALSE );
							act( "Z powietrza formuję się magiczne ostrze i uderza prosto w $z.", victim, NULL, NULL, TO_ROOM );
							send_to_char( "Z powietrza formuje się magiczne ostrze i uderza w ciebie, raniąc boleśnie!\n\r", victim );
							}
							else if (chance > 19)
							{
							dam = dice( 3, 4 ) + 2;
							dam = luck_dam_mod( ch, dam );
							spell_damage( ch, victim, dam, sn, DAM_PIERCE , FALSE );
							act( "Z powietrza formuję się spore magiczne ostrze i uderza prosto w $z.", victim, NULL, NULL, TO_ROOM );
							send_to_char( "Z powietrza formuje się spore magiczne ostrze i uderza prosto w ciebie, raniąc!\n\r", victim );
							}

					}




					event->time = number_range( 2, 3 ) * PULSE_VIOLENCE;
					break;
			}
		}
	}

	return;
}

