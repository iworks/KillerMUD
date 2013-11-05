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
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Skrzetnicki Krzysztof (tener@tenet.pl                ) [Tener     ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: pipes.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/pipes.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "merc.h"
#include "tables.h"

/* 	C R E D I T S
Stworzone 15 IV 2005 by Loop
System fajek (pipe) z poziomu kodu.
Wazne rzeczy: ITEM_PIPE (fajka), ITEM_WEED (ziele fajkowe) i DO_PIPE (komenda-matka do obslugi fajek)

Dodano:	[Kainti] - informacja o spaleniu do desc i efekty palenia.

ToDo - jak wejda ziolka dolozyc ich mozliwosc palenia w fajkach, co zaowocuje jakimis specjalnymi efektami dla palacza.
*/

void do_pipe ( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA * pObj;
	OBJ_DATA * qObj;
	char *pipe_name;
	char pipe_name_2[MAX_INPUT_LENGTH];
	int amount, range, weed_amount, weed_type, dex, wis, con;
	AFFECT_DATA *old_smoke, *old_poison;
	AFFECT_DATA new_smoke, poison;
	int dur = 1, old_dur = 0, old_level = 0, level = 1;

	char buf[ MAX_INPUT_LENGTH ];

	char arg1[ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char arg3[ MAX_INPUT_LENGTH ];
	char arg4[ MAX_INPUT_LENGTH ];

	// mobom dziêkujemy bez komunikatu
	if ( IS_NPC( ch ) )
	{
		return ;
	}

    // w czasie walki dziêkujemy z komunikatem
    if ( ch->position == POS_FIGHTING || ch->fighting != NULL )
	{
		send_to_char( "Lepiej skup siê na walce.\n\r", ch );
		return;
    }

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );
	argument = one_argument( argument, arg4 );

	pObj = get_obj_carry( ch, arg2, ch );

	if ( arg1[ 0 ] == '\0' )
	{
		send_to_char( "Co takiego chcesz zrobiæ z fajk±?\n\r", ch );
		send_to_char( "{R[PORADA]:{y 'Je¿eli nie wiesz, co mo¿esz, wpisz {Rhelp pipe{x / {Cpomoc fajka{x.{Y'{x\n\r", ch );
		return;
	}

	if ( !str_prefix( arg1, "smoke" ) || !str_prefix( arg1, "pal" ))
	{
		if ( arg2[ 0 ] == '\0' )
		{
			send_to_char( "Paliæ co?\n\r", ch );
			return ;
		}

		// je¿eli obiekt nieistnieje w inventory, to mo¿e kto¶ co¶ takiego trzyma

		if ( pObj == NULL ) pObj = get_eq_char( ch, WEAR_HOLD );

		if ( pObj == NULL )
		{
			send_to_char( "Potrzebujesz fajki ¿eby paliæ ziele fajkowe.\n\r", ch );
			return ;
		}

		/*if ( ( pObj = get_obj_carry( ch, arg2, ch ) ) == NULL )
		{
			send_to_char( "Potrzebujesz fajki ¿eby paliæ ziele fajkowe.\n\r", ch );
			return ;
		}*/

		if ( pObj->item_type != ITEM_PIPE )
		{
			send_to_char( "Potrzebujesz fajki ¿eby paliæ ziele fajkowe.\n\r", ch );
			return ;
		}

		/* Gdy fajka jest pusta... */
		if ( pObj->value[ 1 ] <= 0 )
		{
			act( "W $j nie ma niczego, co móg³by¶ paliæ! Jest pusta.", ch, pObj, NULL, TO_CHAR );
			act( "$n spogl±da z dziwnym smutkiem na $h.", ch, pObj, NULL, TO_ROOM );
			return ;
		}

		/* Gdy fajka jest zgaszona*/
		if ( pObj->value[ 0 ] == 0 )
		{
			act( "Przecie¿ $p jest zgaszona!", ch, pObj, NULL, TO_CHAR );
			return ;
		}

		/* Flaga informujaca o tym, ze gracz sobie pociagnal dymka... */
		if ( !EXT_IS_SET( ch->act, PLR_SMOKED ) )
			EXT_SET_BIT( ch->act, PLR_SMOKED );

		if ( is_affected( ch, gsn_on_smoke ) )
		{
			old_smoke = affect_find( ch->affected, gsn_on_smoke );
			old_dur = old_smoke->duration;
			old_level = old_smoke->level;
			affect_strip( ch, gsn_on_smoke );
		}

		level = weed_table[ pObj->value[2] ].weed_affect[0];

		dur += old_dur;
		level += old_level;

		new_smoke.where = TO_AFFECTS;
		new_smoke.type = gsn_on_smoke;
		new_smoke.level = level;
		new_smoke.duration = UMIN( dur, number_range( 5, 7 ) );
		new_smoke.rt_duration = 0;
		new_smoke.location = APPLY_NONE;
		new_smoke.modifier = 0;
		new_smoke.bitvector = &AFF_NONE;

		if ( level > number_range( 5, 7 ) && weed_table[ pObj->value[2] ].weed_affect[0] && number_percent() < weed_table[ pObj->value[2] ].weed_affect[1] )
		{
			send_to_char( "\n\rZaczynasz czuæ siê jako¶ dziwnie. Przed oczyma zaczynaj± lataæ ci kolorowe plamki.\n\r", ch );
			act( "\n\r$n zaczyna wygl±dajaæ jako¶ dziwnie blado.", ch, pObj, NULL, TO_ROOM );
			if ( dice( 1, 2 ) == 1 )
				new_smoke.bitvector = &AFF_HALLUCINATIONS_NEGATIVE;
			else
				new_smoke.bitvector = &AFF_HALLUCINATIONS_POSITIVE;
		}
		new_smoke.visible = FALSE;
		affect_to_char( ch, &new_smoke, NULL, FALSE );

		if ( number_percent() < weed_table[ pObj->value[2] ].weed_affect[2] )
		{
			if( is_affected(ch,gsn_poison ))
			{
				old_poison = affect_find( ch->affected, gsn_poison );
				old_poison->duration += stat_throw(ch,STAT_CON) ? number_range(2,4) : number_range(4,8);
				if( old_poison->level < UMIN( weed_table[ pObj->value[2] ].weed_affect[0], 3) )
					old_poison->level = UMIN( weed_table[ pObj->value[2] ].weed_affect[0], 3);
				act( "$n jeszcze bardziej zielenieje na twarzy.", ch, pObj, NULL, TO_ROOM );
				send_to_char( "Czujesz siê jeszcze gorzej.\n\r", ch );
			}
			else
			{
				poison.where = TO_AFFECTS;
				poison.type = gsn_poison;
				poison.level = UMIN( weed_table[ pObj->value[2] ].weed_affect[0], 3);
				poison.duration = stat_throw(ch,STAT_CON) ? number_range(2,4) : number_range(4,8);
				poison.rt_duration = 0;
				poison.location = APPLY_NONE;
				poison.modifier = 0;
				poison.bitvector = &AFF_NONE;
				poison.visible = FALSE;
				affect_to_char( ch, &poison, NULL, FALSE );
				send_to_char( "Momentalnie zaczynasz czuæ siê jako¶ niedobrze. Bardzo niedobrze.\n\r", ch );
				act( "$n widocznie zielenieje na twarzy.", ch, pObj, NULL, TO_ROOM );
			}
		}

		/* Troche losowosci */
		range = number_range( 1, 9 );
		switch ( range )
		{
			case 1:
				act( "$n zaci±ga siê g³êboko $j.", ch, pObj, NULL, TO_ROOM );
				act( "Zaci±gasz siê g³êboko $j.", ch, pObj, NULL, TO_CHAR );
				break;
			case 2:
				act( "$n pyka sobie $h przez chwilê.", ch, pObj, NULL, TO_ROOM );
				act( "Pykasz sobie $h przez chwilê.", ch, pObj, NULL, TO_CHAR );
				break;
			case 3:
				act( "$n wci±ga gwa³townie dym z $f.", ch, pObj, NULL, TO_ROOM );
				act( "Wci±gasz gwa³townie dym z $f.", ch, pObj, NULL, TO_CHAR );
				break;
			case 4:
				act( "$n zaci±ga siê delikatnie $j.", ch, pObj, NULL, TO_ROOM );
				act( "Zaci±gasz siê delikatnie $j.", ch, pObj, NULL, TO_CHAR );
				break;
			case 5:
				act( "Widzisz jak $n zaci±ga siê lekko $j.", ch, pObj, NULL, TO_ROOM );
				act( "Zaci±gasz siê lekko $j.", ch, pObj, NULL, TO_CHAR );
				break;
			case 6:
				act( "Dostrzegasz, jak $n z wpraw± zaci±ga siê dymkiem z $f.", ch, pObj, NULL, TO_ROOM );
				act( "Z wielk± wpraw± zaci±gasz siê dymkiem z $f.", ch, pObj, NULL, TO_CHAR );
				break;
			case 7:
				act( "$n jakby od niechcenia pali przez chwilê swoj± $h.", ch, pObj, NULL, TO_ROOM );
				act( "Przez chwilkê machinalnie palisz sobie swoj± $h.", ch, pObj, NULL, TO_CHAR );
				break;
			case 8:
				act( "$n z u¶miechem kurzy sobie $h.", ch, pObj, NULL, TO_ROOM );
				act( "Kurzysz sobiê $h, u¶miechaj±c siê przy tym b³ogo.", ch, pObj, NULL, TO_CHAR );
				break;
			default:
				act( "$n pali sobie przez chwilkê $h.", ch, pObj, NULL, TO_ROOM );
				act( "Palisz sobie przez chwilkê $h.", ch, pObj, NULL, TO_CHAR );
				break;
		}

		amount = number_range( 1, 3 ); // moze sie palic szybciej, lub wolniej (o 1, 2 lub o 3)
		if ( pObj->value[ 0 ] == 1 )     // sprawdzamy czy fajeczka jest zapalona
		{
			pObj->value[ 1 ] -= amount; // wypalamy zawartosc fajeczki
			//pObj->short_descr = capitalize(pObj->short_descr );
			amount = number_range( 1, 7 );
			switch ( amount )
			{
				case 1:
					act( "Nad $j trzyman± przez $z unosi siê w±ska stru¿ka dymu.", ch, pObj, NULL, TO_ROOM );
					act( "Nad $j unosi siê w±ska stru¿ka dymu.", ch, pObj, NULL, TO_CHAR );
					break;
				case 2:
					act( "Nad $j $z unosz± siê ma³e, ciemne chmurki dymu.", ch, pObj, NULL, TO_ROOM );
					act( "Nad $j unosz± siê ma³e, ciemne chmurki dymu.", ch, pObj, NULL, TO_CHAR );
					break;
				case 3:
					act( "Widzisz jak w $f $z tl± siê ma³e, jasne iskierki.", ch, pObj, NULL, TO_ROOM );
					act( "Widzisz kilka ma³ych, jasnych iskierek tl±cych siê w $k.", ch, pObj, NULL, TO_CHAR );
					break;
				case 4:
					act( "Dostrzegasz, ¿e nad $f trzyman± przez $z unosi siê gêsta smuga dymu.", ch, pObj, NULL, TO_ROOM );
					act( "Dosrzegasz unosz±c± siê nad $f gêst± smugê dymu.", ch, pObj, NULL, TO_CHAR );
					break;
				case 5:
					act( "S³yszysz jak zawarto¶æ $f $z lekko syczy tl±c siê.", ch, pObj, NULL, TO_ROOM );
					act( "S³yszysz jak zawarto¶æ $f lekko syczy tl±c siê.", ch, pObj, NULL, TO_CHAR );
					break;
				case 6:
					act( "Czujesz delikatny aromat jakoby zio³owego dymu, bij±cy od $z i jego $f.", ch, pObj, NULL, TO_ROOM );
					act( "Czujesz bij±cy od $f lekki, jakby zio³owy aromat.", ch, pObj, NULL, TO_CHAR );
					break;
				default:
					act( "Zawarto¶æ $f nale¿±cej do $z tli siê lekko.", ch, pObj, NULL, TO_ROOM );
					act( "Zawarto¶æ $f tli siê lekko.", ch, pObj, NULL, TO_CHAR );
					break;
			}

			if ( pObj->value[ 1 ] <= 0 )
			{
				act( "Ostatnie, tl±ce siê iskierki gasn±, kiedy wypali³a siê ca³a zawarto¶æ $f.", ch, pObj, NULL, TO_CHAR );
				act( "Widzisz jak $p $z ga¶nie z lekkim sykiem.", ch, pObj, NULL, TO_ROOM );
				pObj->value[ 0 ] = 0;
				pObj->value[ 1 ] = 0;
				pObj->value [ 2 ] = 0;
			}
		}
		return ;
	}


	if ( !str_prefix( arg1, "exhale" ) || !str_prefix( arg1, "wydech" ))
	{
		// Potrzebne staty do testow na 'triki'...
		dex = get_curr_stat_deprecated(ch,STAT_DEX);
		con = get_curr_stat_deprecated(ch,STAT_CON);
		wis = get_curr_stat_deprecated(ch,STAT_WIS);

		/* Zeby "wydychac" flaga palenia musi byc nalozona na gracza - sprawdzanie */
		if ( !EXT_IS_SET( ch->act, PLR_SMOKED ) )
		{
			send_to_char( "Jak chcesz to zrobiæ? Najpierw musisz zaci±gn±æ siê dymem!\n\r", ch );
			return ;
		}

		/* Pomocy!
		if ( !str_prefix( arg2, "help" ) || !str_prefix( arg2, "pomoc" ))
		{
			act( "{R[PORADA]:{x\n\r", ch, NULL, NULL, TO_CHAR );
			act( "{YZanim bêdziesz <&móg³/mog³a/mog³o> wydychaæ dym musisz paliæ fajkê.{x", ch, NULL, NULL, TO_CHAR );
			act( "{YPo tym jak skoñczysz rozkoszowaæ siê bogatym smakiem ziela fajkowego...{x", ch, NULL, NULL, TO_CHAR );
			act( "{YMo¿esz wydychaæ tytoniowy dym na ró¿ne sposoby, ¿eby uatrakcyjniæ palenie.{x", ch, NULL, NULL, TO_CHAR );
			act( "{YSpróbuj tych oto typów wydechu, aby zadziwiæ znajomych:{x \n\r", ch, NULL, NULL, TO_CHAR );
			act( "Angielskie komendy: RINGS, LINES, SPHERE, WEB, PHOENIX, HORNS, NAME", ch, NULL, NULL, TO_CHAR );
			act( "Polskie komendy: KO£A, LINIE, KULA, SIEÆ, FENIKS, ROGI, IMIÊ\n\r", ch, NULL, NULL, TO_CHAR );
			act( "\n\r{YSk³adnia angielska: {Gpipe exhale <nazwa figury>{x.", ch, NULL, NULL, TO_CHAR );
			act( "{YSk³adnia polska: {Gfajka wydech <nazwa figury>{x.\n\r", ch, NULL, NULL, TO_CHAR );
			return ;
		}*/

		if ( arg2[ 0 ] == '\0' )
		{
			switch ( number_range ( 1, 5 ) )
			{
				case 1:
					act( "$n wydycha ustami ob³ok bladego, szarego dymu.", ch, NULL, NULL, TO_ROOM );
					act( "Wydychasz ustami ob³ok szarego, bladego dymu.", ch, NULL, NULL, TO_CHAR );
					EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
					return ;
				case 2:
					act( "$n wydycha nosem ob³ok bladego, szarego dymu.", ch, NULL, NULL, TO_ROOM );
					act( "Wydychasz nosem ob³ok szarego, bladego dymu.", ch, NULL, NULL, TO_CHAR );
					EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
					return ;
				case 3:
					act( "$n spokojnie wydycha ustami ob³ok bladego, szarego dymu.", ch, NULL, NULL, TO_ROOM );
					act( "Spokojnie wydychasz ustami ob³ok szarego, bladego dymu.", ch, NULL, NULL, TO_CHAR );
					EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
					return ;
				case 4:
					act( "$n spokojnie wydycha nosem ob³ok bladego, szarego dymu.", ch, NULL, NULL, TO_ROOM );
					act( "Spokojnie wydychasz nosem ob³ok szarego, bladego dymu.", ch, NULL, NULL, TO_CHAR );
					EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
					return ;
				default:
					act( "$n wydycha ob³ok bladego, szarego dymu.", ch, NULL, NULL, TO_ROOM );
					act( "Wydychasz ob³ok szarego, bladego dymu.", ch, NULL, NULL, TO_CHAR );
					EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
					return ;
			}
		}
		else if ( !str_prefix( arg2, "ko³a" ) || !str_prefix( arg2, "rings" ) )
		{
			if ( dice( 2, dex - 10 ) >= 4 && dice( 2, con - 6 ) > 6 )
			{
				act( "$n niespodziewanie wypuszcza z ust kilka du¿ych, zadziwiaj±co okr±g³ych kó³ z dymu, które uk³adaj± siê w ¶mieszny tunel zawieszony w powietrzu!", ch, NULL, NULL, TO_ROOM );
				act( "Wypuszczasz z ust kilka du¿ych, zadziwiaj±co okr±g³ych kó³ z dymu, które uk³adaj± siê w ¶mieszny tunel zawieszony w powietrzu!", ch, NULL, NULL, TO_CHAR );
				EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
				return ;
			}
			else
			{
				switch ( number_range ( 1, 4 ) )
				{
					case 1:
						act( "$n nagle robi dziwn± minê, po czym momentalnie dostaje ataku kaszlu.", ch, pObj, NULL, TO_ROOM );
						act( "Próbujesz zrobiæ sztuczkê, jednak w ostatniej chwili dym zacz±³ gry¼æ ciê w gar³o i <&dosta³e¶/dosta³a¶/dosta³o¶> ataku kaszlu!", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 2:
						act( "Widzisz jak $n koncentruje siê przez chwilkê, aby po chwili wypu¶ciæ z ust niekszta³tn± chmurê dymu.", ch, pObj, NULL, TO_ROOM );
						act( "Koncentrujesz siê przez chwilkê, chc±c wypu¶ciæ z ust chmurê dymu w kszta³cie ko³a, jednak nie udaje ci siê to.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 3:
						act( "$n nagle zielenieje na twarzy, a z jego nosa zaczynaj± wylatywaæ szare stru¿ki dymu.", ch, pObj, NULL, TO_ROOM );
						act( "W ostatniej chwili przygotowañ do zrobienia sztuczki krztusisz siê i bezwiednie wypuszczasz dym nosem.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					default:
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						act( "$n próbuje zrobiæ jak±¶ sztuczkê z wypuszczeniem z ust dymu, jednak udaje mu siê jedynie uformowaæ dym w bezkszta³tn± chmurê.", ch, pObj, NULL, TO_ROOM );
						act( "Starasz siê wypu¶ciæ z ust kilka kszta³tnych kó³, jednak nie udaje ci siê, a dym przybiera kszta³t nieforemnej chmury.", ch, pObj, NULL, TO_CHAR );
						break;
				}
			}
		}
		else if ( !str_prefix( arg2, "linie" ) || !str_prefix( arg2, "lines" ))
		{
			if ( dice( 2, dex - 10 ) > 5 && dice( 2, con - 8 ) >= 5 )
			{
				act( "$n wydycha d³ug± smugê dymu w kszta³cie piêknej, prostej linii, zdaj±c± siê pi±æ w kierunku nieba!", ch, NULL, NULL, TO_ROOM );
				act( "Wydychasz d³ug± smugê dymu w kszta³cie piêknej, prostej linii, zdaj±c± siê pi±æ w kierunku nieba!", ch, NULL, NULL, TO_CHAR );
				EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
				return ;
			}
			else
			{
				switch ( number_range ( 1, 5 ) )
				{
					case 1:
						act( "$n nagle robi dziwn± minê, po czym momentalnie dostaje ataku kaszlu.", ch, pObj, NULL, TO_ROOM );
						act( "Próbujesz zrobiæ sztuczkê, jednak w ostatniej chwili dym zacz±³ gry¼æ ciê w gar³o i <&dosta³e¶/dosta³a¶/dosta³o¶> ataku kaszlu!", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 2:
						act( "Widzisz jak $n koncentruje siê przez chwilkê, aby po chwili wypu¶ciæ z ust niekszta³tn± chmurê dymu.", ch, pObj, NULL, TO_ROOM );
						act( "Koncentrujesz siê przez chwilkê, chc±c wypu¶ciæ z ust chmurê dymu w kszta³cie linii, jednak nie udaje ci siê to.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 3:
						act( "$n nagle zielenieje na twarzy, a z jego nosa zaczynaj± wylatywaæ szare stru¿ki dymu.", ch, pObj, NULL, TO_ROOM );
						act( "W ostatniej chwili przygotowañ do zrobienia sztuczki krztusisz siê i bezwiednie wypuszczasz dym nosem.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 4:
						act( "$n z gracj± zaczyna wypuszczaæ z ust kilka chmurek dymu, które niespodziewanie szybko rozp³ywaj± siê w powietrzu.", ch, pObj, NULL, TO_ROOM );
						act( "W skupieniu wypuszczasz z ust kilka chmurek dymu, te jednak rozp³ywaj± siê szybko w powietrzu zanim zd±zy³y siê uformowaæ w jaki¶ kszta³t.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					default:
						act( "$n próbuje zrobiæ jak±¶ sztuczkê z wypuszczeniem z ust dymu, jednak udaje mu siê jedynie uformowaæ dym w bezkszta³tn± chmurê.", ch, pObj, NULL, TO_ROOM );
						act( "Starasz siê wypu¶ciæ z ust prost± liniê dymu, jednak nie udaje ci siê, a dym przybiera kszta³t nieforemnej chmury.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
				}
			}
		}
		else if ( !str_prefix( arg2, "kula" ) || !str_prefix( arg2, "sphere" ))
		{
			if ( dice( 2, dex - 5 ) >= 7 && dice( 3, con - 2 ) > 15 )
			{
				act( "$n wypuszcza z ust jedn±, niesamowicie okr±g³a, chmurê dymu w kszta³cie idealnej kuli!", ch, NULL, NULL, TO_ROOM );
				act( "Wypuszczasz z ust du¿±, niesamowicie okr±g³±, chmurê dymu w kszta³cie idealnej kuli!", ch, NULL, NULL, TO_CHAR );
				EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
				return ;
			}
			else
			{
				switch ( number_range ( 1, 5 ) )
				{
					case 1:
						act( "$n nagle robi dziwn± minê, po czym momentalnie dostaje ataku kaszlu.", ch, pObj, NULL, TO_ROOM );
						act( "Próbujesz zrobiæ sztuczkê, jednak w ostatniej chwili dym zacz±³ gry¼æ ciê w gar³o i <&dosta³e¶/dosta³a¶/dosta³o¶> ataku kaszlu!", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 2:
						act( "Widzisz jak $n koncentruje siê przez chwilkê, aby po chwili wypu¶ciæ z ust niekszta³tn± chmurê dymu.", ch, pObj, NULL, TO_ROOM );
						act( "Koncentrujesz siê przez chwilkê, chc±c wypu¶ciæ z ust chmurê dymu w kszta³cie kuli, jednak nie udaje ci siê to.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 3:
						act( "$n nagle zielenieje na twarzy, a z jego nosa zaczynaj± wylatywaæ szare stru¿ki dymu.", ch, pObj, NULL, TO_ROOM );
						act( "W ostatniej chwili przygotowañ do zrobienia sztuczki krztusisz siê i bezwiednie wypuszczasz dym nosem.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 4:
						act( "$n nagle zaczyna kaszlaæ dymem, a dym który ulatuje z jego ust tworzy dziwn± chmurkê.", ch, pObj, NULL, TO_ROOM );
						act( "Niespodziewanie co¶ ci nie wychodzi, krztusisz siê i kaszlaj±c panicznie wypuszczasz z ust resztki dymu.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					default:
						act( "$n próbuje zrobiæ jak±¶ sztuczkê z wypuszczeniem z ust dymu, jednak udaje mu siê jedynie uformowaæ dym w bezkszta³tn± chmurê.", ch, pObj, NULL, TO_ROOM );
						act( "Starasz siê wypu¶ciæ z ust kulê dymu, jednak nie udaje ci siê, a dym przybiera kszta³t nieforemnej chmury.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
				}
			}
		}
		else if ( !str_prefix( arg2, "sieæ" ) || !str_prefix( arg2, "web" ))
		{
			if ( dice( 2, dex - 5 ) >= 15 && dice( 2, con - 8 ) > 6 )
			{
				act( "$n wydycha kilkana¶cie cieniutkich, zwiewnych pasemek dymu, które niespodziewanie ³±cz± siê tworz±c delikatn±, prawie pajêcz±, sieæ w powietrzu!", ch, NULL, NULL, TO_ROOM );
				act( "Wydychasz kilkana¶cie cieniutkich, zwiewnych pasemek dymu, które niespodziewanie ³±cz± siê tworz±c delikatn±, prawie pajêcz±, sieæ w powietrzu!", ch, NULL, NULL, TO_CHAR );
				EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
				return ;
			}
			else
			{
				switch ( number_range ( 1, 5 ) )
				{
					case 1:
						act( "$n nagle robi dziwn± minê, po czym momentalnie dostaje ataku kaszlu.", ch, pObj, NULL, TO_ROOM );
						act( "Próbujesz zrobiæ sztuczkê, jednak w ostatniej chwili dym zacz±³ gry¼æ ciê w gar³o i <&dosta³e¶/dosta³a¶/dosta³o¶> ataku kaszlu!", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 2:
						act( "Widzisz jak $n koncentruje siê przez chwilkê, aby po chwili wypu¶ciæ z ust niekszta³tn± chmurê dymu.", ch, pObj, NULL, TO_ROOM );
						act( "Koncentrujesz siê przez chwilkê, chc±c wypu¶ciæ z ust chmurê dymu w kszta³cie kuli, jednak nie udaje ci siê to.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 3:
						act( "$n nagle zielenieje na twarzy, a z jego nosa zaczynaj± wylatywaæ szare stru¿ki dymu.", ch, pObj, NULL, TO_ROOM );
						act( "W ostatniej chwili przygotowañ do zrobienia sztuczki krztusisz siê i bezwiednie wypuszczasz dym nosem.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 4:
						act( "$n wydycha szybk± seriê w±skich dymków, które wydaj± siê pe³zaæ w powietrzu jak niepos³usze robaczki, szybko rozp³ywaj±ce siê w powietrzu.", ch, pObj, NULL, TO_ROOM );
						act( "Szybko wydychasz seriê w±skich dymków chc±c z nich uformowaæ pajêczynê, te jednak szybko rozp³ywaj± siê w powietrzu.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					default:
						act( "$n próbuje zrobiæ jak±¶ sztuczkê z wypuszczeniem z ust dymu, jednak udaje mu siê jedynie uformowaæ dym w bezkszta³tn± chmurê.", ch, pObj, NULL, TO_ROOM );
						act( "Starasz siê wypu¶ciæ z ust seriê dymków, jednak nie udaje ci siê, a sam dym przybiera kszta³t nieforemnej chmury.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
				}
			}
		}
		else if ( !str_prefix( arg2, "feniks" ) || !str_prefix( arg2, "phoenix" ) || !str_prefix( arg2, "fenix" ) )
		{
			if ( dice( 2, dex - 10 ) >= 8 && dice( 2, con - 10 ) >= 8 && dice( 2, wis - 10 ) >= 5 )
			{
				act( "$n wydycha serie ma³ych chmurek dymu, które na chwilê uk³adaj± siê w wizerunek feniksa, tylko po to, aby chwilê pó¼niej rozp³ynê³y siê, niczym proch.", ch, NULL, NULL, TO_ROOM );
				act( "Wydychasz serie ma³ych chmurek dymu, które na chwilê uk³adaj± siê w wizerunek feniksa, tylko po to, aby chwilê pó¼niej rozp³ynê³y siê, niczym proch.", ch, NULL, NULL, TO_CHAR );
				EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
				return ;
			}
			else
			{
				switch ( number_range ( 1, 7 ) )
				{
					case 1:
						act( "$n nagle robi dziwn± minê, po czym momentalnie dostaje ataku kaszlu.", ch, pObj, NULL, TO_ROOM );
						act( "Próbujesz zrobiæ sztuczkê, jednak w ostatniej chwili dym zacz±³ gry¼æ ciê w gar³o i <&dosta³e¶/dosta³a¶/dosta³o¶> ataku kaszlu!", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 2:
						act( "Widzisz jak $n koncentruje siê przez chwilkê, aby po chwili wypu¶ciæ z ust niekszta³tn± chmurê dymu.", ch, pObj, NULL, TO_ROOM );
						act( "Koncentrujesz siê przez chwilkê, chc±c wypu¶ciæ z ust chmurê dymu w kszta³cie linii, jednak nie udaje ci siê to.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 3:
						act( "$n nagle zielenieje na twarzy, a z jego nosa zaczynaj± wylatywaæ szare stru¿ki dymu.", ch, pObj, NULL, TO_ROOM );
						act( "W ostatniej chwili przygotowañ do zrobienia sztuczki krztusisz siê i bezwiednie wypuszczasz dym nosem.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 4:
						act( "$n w wielkim skupieniu wydycha kilkana¶cie dymków, które zaczynaj± uk³adaæ siê w powietrzu w co¶ przypominaj±cego hybrydê szczura z ludzkimi nogami... Czy to by³o zamierzone?", ch, pObj, NULL, TO_ROOM );
						act( "W wielkim skupieniu wydychasz kilkana¶cie dymków, które niespodziewanie zamiast feniksa uk³adaj± siê w co¶ przypominaj±cego po³±czenie szczura z ludzkimi nogami! Okropne.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 5:
						act( "$n, z grymasem na twarzy, wypuszcza z ust kilkana¶cie dymków, które uk³adaj± siê w powietrzu w jaki¶ dziwny ptakopodobny twór, wygl±daj±cy dosyæ ohydnie.", ch, pObj, NULL, TO_ROOM );
						act( "Czuj±c, ¿e co¶ pójdzie ¼le wypuszczasz z ust kilkana¶cie dymków, które uk³adaj± siê w powietrzu w jaki¶ bezkszta³tny, ptakopodobny twór.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 6:
						act( "$n, z wyrazem pewno¶ci siebie na twarzy, wydycha seriê ma³ych dymków, które nagle ³±cz± siê w powietrzu tworz±c pewien du¿y, oble¶ny kszta³t... Hmm, to ciekawe. K±tem oka widzisz, ¿e $n siê rumieni na widok swojego dzie³a.", ch, pObj, NULL, TO_ROOM );
						act( "Wypuszczasz z ust kilkana¶cie ma³ych dymków, które nagle ³±cz± siê w powietrzu, tworz±c naprawdê du¿y, oble¶ny kszta³t. Chyba siê zaraz spalisz ze wstydu!", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					default:
						act( "$n próbuje zrobiæ jak±¶ sztuczkê z wypuszczeniem z ust dymu, jednak udaje mu siê jedynie uformowaæ dym w bezkszta³tn± chmurê.", ch, pObj, NULL, TO_ROOM );
						act( "Starasz siê wypu¶ciæ z ust kilkana¶cie dymków, jednak nie udaje ci siê, a sam dym przybiera kszta³t nieforemnej chmury.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
				}
			}
		}
		else if ( !str_prefix( arg2, "rogi" ) || !str_prefix( arg2, "horns" ))
		{
			if ( dice( 2, dex - 10 ) >= 9 && dice( 2, con - 6 ) >= 10 )
			{
				act( "$n wydycha dwie grube smugi dymu, które uk³adaj± siê nad $m niczym para wielkich rogów!", ch, NULL, NULL, TO_ROOM );
				act( "Wydychasz dwie grube smugi dymu, które uk³adaj± sie nad tob± niczym para wielkich rogów!", ch, NULL, NULL, TO_CHAR );
				EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
				return ;
			}
			else
			{
				switch ( number_range ( 1, 4 ) )
				{
					case 1:
						act( "$n nagle robi dziwn± minê, po czym momentalnie dostaje ataku kaszlu.", ch, pObj, NULL, TO_ROOM );
						act( "Próbujesz zrobiæ sztuczkê, jednak w ostatniej chwili dym zacz±³ gry¼æ ciê w gar³o i <&dosta³e¶/dosta³a¶/dosta³o¶> ataku kaszlu!", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 2:
						act( "Widzisz jak $n koncentruje siê przez chwilkê, aby po chwili wypu¶ciæ z ust niekszta³tn± chmurê dymu.", ch, pObj, NULL, TO_ROOM );
						act( "Koncentrujesz siê przez chwilkê, chc±c wypu¶ciæ z ust chmurê dymu w kszta³cie linii, jednak nie udaje ci siê to.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 3:
						act( "$n nagle zielenieje na twarzy, a z jego nosa zaczynaj± wylatywaæ szare stru¿ki dymu.", ch, pObj, NULL, TO_ROOM );
						act( "W ostatniej chwili przygotowañ do zrobienia sztuczki krztusisz siê i bezwiednie wypuszczasz dym nosem.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					default:
						act( "$n próbuje zrobiæ jak±¶ sztuczkê z wypuszczeniem z ust dymu, jednak udaje mu siê jedynie uformowaæ dym w bezkszta³tn± chmurê.", ch, pObj, NULL, TO_ROOM );
						act( "Starasz siê wypu¶ciæ z ust prost± liniê dymu, jednak nie udaje ci siê, a dym przybiera kszta³t nieforemnej chmury.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
				}
			}
		}
		else if ( !str_prefix( arg2, "imiê" ) || !str_prefix( arg2, "name" ))
		{
			if ( dice( 2, dex - 10 ) >= 8 && dice( 2, con - 8 ) >= 9 && dice( 2, wis - 10 ) > 6 )
			{
				act( "$n wypuszcza z ust kilkana¶cie ma³ych dymków, które niespodziewanie tworz± w powietrzu napis '$n'! Niesamowite!", ch, NULL, NULL, TO_ROOM );
				act( "Wypuszczasz z ust kilkana¶cie ma³ych dymków, które niespodziewanie tworz± w powietrzu napis '$n'! ¦wietnie!", ch, NULL, NULL, TO_CHAR );
				EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
				return ;
			}
			else
			{
				switch ( number_range ( 1, 6 ) )
				{
					case 1:
						act( "$n nagle robi dziwn± minê, po czym momentalnie dostaje ataku kaszlu.", ch, pObj, NULL, TO_ROOM );
						act( "Próbujesz zrobiæ sztuczkê, jednak w ostatniej chwili dym zacz±³ gry¼æ ciê w gar³o i <&dosta³e¶/dosta³a¶/dosta³o¶> ataku kaszlu!", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 2:
						act( "Widzisz jak $n koncentruje siê przez chwilkê, aby po chwili wypu¶ciæ z ust niekszta³tn± chmurê dymu.", ch, pObj, NULL, TO_ROOM );
						act( "Koncentrujesz siê przez chwilkê, chc±c wypu¶ciæ z ust chmurê dymu w kszta³cie linii, jednak nie udaje ci siê to.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 3:
						act( "$n nagle zielenieje na twarzy, a z jego nosa zaczynaj± wylatywaæ szare stru¿ki dymu.", ch, pObj, NULL, TO_ROOM );
						act( "W ostatniej chwili przygotowañ do zrobienia sztuczki krztusisz siê i bezwiednie wypuszczasz dym nosem.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 4:
						act( "$n, z wyrazem pewno¶ci siebie na twarzy, wydycha seriê ma³ych dymków, które nagle ³±cz± siê w powietrzu tworz±c pewien du¿y, oble¶ny kszta³t... Hmm, to ciekawe. K±tem oka widzisz, ¿e $n siê rumieni na widok swojego dzie³a.", ch, pObj, NULL, TO_ROOM );
						act( "Wypuszczasz z ust kilkana¶cie ma³ych dymków, które nagle ³±cz± siê w powietrzu, tworz±c naprawdê du¿y, oble¶ny kszta³t. Chyba siê zaraz spalisz ze wstydu!", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					case 5:
						print_char( ch, "Wydychasz w wielkim skupieniu kilkana¶cie szarych dymków, które niespodziewanie tworz± w powietrzu bezsensowny napis '%s'!", gen_random_str( 4, 8 ) );
						sprintf( buf, "$n wydycha w wielkim skupieniu kilkana¶cie szarych dymków, które niespodziewanie tworz± w powietrzu bezsensowny napis '%s'!\n\r", gen_random_str( 4, 8 ) );
						act( buf, ch, NULL, NULL, TO_ROOM );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
					default:
						act( "$n próbuje zrobiæ jak±¶ sztuczkê z wypuszczeniem z ust dymu, jednak udaje mu siê jedynie uformowaæ dym w bezkszta³tn± chmurê.", ch, pObj, NULL, TO_ROOM );
						act( "Starasz siê wypu¶ciæ z ust kilka dziwnych chmurek dymu, jednak nie udaje ci siê, a dym przybiera kszta³t nieforemnej chmury.", ch, pObj, NULL, TO_CHAR );
						EXT_REMOVE_BIT( ch->act, PLR_SMOKED );
						break;
				}
			}
		}
		else
		{
			send_to_char( "Huh?! Nie znasz takiej sztuczki!\n\r", ch );
			return;
		}
	}


	// Nabijanie fajki
	if ( !str_prefix( arg1, "tamp" ) || !str_prefix( arg1, "nabij" ))
	{
		// Seria zabezpieczen...
		if ( pObj == NULL ) pObj = get_eq_char( ch, WEAR_HOLD );

		if ( pObj == NULL )
		{
			act( "Co chcesz nabiæ?", ch, pObj, NULL, TO_CHAR );
			return ;
		}

		if ( pObj->item_type != ITEM_PIPE )
		{
			send_to_char( "Mo¿esz nabijaæ jedynie fajki!\n\r", ch );
			return ;
		}

		if ( ( qObj = get_obj_carry( ch, arg3, ch ) ) == NULL )
		{
			act( "Czym chcesz nabiæ $h?", ch, pObj, NULL, TO_CHAR );
			return ;
		}

		if ( qObj->item_type != ITEM_WEED )
		{
			pipe_name = capitalize( pObj->name4 );
			sprintf( pipe_name_2, "%s mo¿esz nabiæ jedynie jakim¶ rodzajem ziela.", pipe_name );
			act( pipe_name_2, ch, pObj, NULL, TO_CHAR );
		}
		else if ( pObj->value[ 0 ] == 1 )
		{
			pipe_name = capitalize( pObj->short_descr );
			sprintf( pipe_name_2, "%s jest zapalona, nie mo¿esz jej teraz nabijaæ!", pipe_name );
			act( pipe_name_2, ch, pObj, NULL, TO_CHAR );
			return ;
		}
		else if ( pObj->value[ 1 ] > 0 )
		{
			pipe_name = capitalize( pObj->short_descr );
			sprintf( pipe_name_2, "%s jest ju¿ czym¶ nabita. Przed ponownym nabiciem opró¿nij j±.", pipe_name );
			act( pipe_name_2, ch, pObj, NULL, TO_CHAR );
			return ;
		}
		else
		{
			// Nabijanie fajki, przepisywanie wartosci ziela do fajeczki.
			weed_amount = qObj->value [ 0 ];
			weed_type = qObj->value [ 1 ];
			pObj->value [ 1 ] = weed_amount;
			pObj->value [ 2 ] = weed_type;
			print_char( ch, "Nabijasz %s %s.\n\r", pObj->name4, qObj->name5 );
			sprintf( buf, "$n nabija %s %s.\n\r", pObj->name4, qObj->name5 );
			act( buf, ch, NULL, NULL, TO_ROOM );
			extract_obj( qObj );
			return ;
		}


	}

	// zapalanie
	if ( !str_prefix( arg1, "light" ) || !str_prefix( arg1, "zapal" ))
	{
		pObj = get_obj_carry( ch, arg2, ch );

		if ( pObj == NULL ) pObj = get_eq_char( ch, WEAR_HOLD );

		if ( pObj == NULL )
		{
			act( "Co chcesz zapaliæ?", ch, pObj, NULL, TO_CHAR );
			return ;
		}

		if ( pObj->item_type != ITEM_PIPE )
		{
			send_to_char( "Mo¿esz zapalaæ jedynie fajki!\n\r", ch );
			return ;
		}
		else if ( pObj->value[ 0 ] == 1 )
		{
			pipe_name = capitalize( pObj->short_descr );
			sprintf( pipe_name_2, "%s jest ju¿ zapalona.", pipe_name );
			act( pipe_name_2, ch, pObj, NULL, TO_CHAR );
			return ;
		}
		else if ( pObj->value[ 2 ] == 4 )
		{
			act( "Zawarto¶æ $f jest mokra! Nie dasz rady tego zapaliæ.", ch, pObj, NULL, TO_CHAR );
			act( "Lepiej wyczy¶æ $h i nape³nij j± czym¶ suchym, najlepiej zielem fajkowym.", ch, pObj, NULL, TO_CHAR );
			return ;
		}
		else
		{
			pObj->value [ 0 ] = 1;
			act( "Zapalaj±c $h zaczynasz czuæ przyjemny, delikatny zapach palonego ziela.", ch, pObj, NULL, TO_CHAR );
			act( "$n zapala $h. Czujesz jak w powietrzu zaczyna unosiæ siê lekki zapach palonego ziela.", ch, pObj, NULL, TO_ROOM );
			return ;
		}
	}

	// gaszenie
	if ( !str_prefix( arg1, "extinguish" ) || !str_prefix( arg1, "zga¶" ))
	{

		/*if ( ( pObj = get_obj_carry( ch, arg2, ch ) ) == NULL )
		{
			act( "Co chcesz zgasiæ?", ch, pObj, NULL, TO_CHAR );
			return ;
		}*/

		if ( pObj == NULL ) pObj = get_eq_char( ch, WEAR_HOLD );

		if ( pObj == NULL )
		{
			act( "Co chcesz zgasiæ?", ch, pObj, NULL, TO_CHAR );
			return ;
		}

		if ( pObj->item_type != ITEM_PIPE )
		{
			send_to_char( "Mo¿esz gasiæ jedynie fajki!\n\r", ch );
			return ;
		}
		else if ( pObj->value[ 0 ] == 0 )
		{
			pipe_name = capitalize( pObj->short_descr );
			sprintf( pipe_name_2, "%s jest ju¿ zgaszona.", pipe_name );
			act( pipe_name_2, ch, pObj, NULL, TO_CHAR );
			return ;
		}
		else
		{
			pObj->value [ 0 ] = 0;
			act( "Gasz±c $h widzisz jak ostatnie iskierki ognia bledn± i znikaj±.", ch, pObj, NULL, TO_CHAR );
			act( "$n gasi $h.", ch, pObj, NULL, TO_ROOM );
			return ;
		}
	}

	// czyszczenie / opró¿nianie
	if ( !str_prefix( arg1, "clean" ) || !str_prefix( arg1, "wyczy¶æ" ))
	{

		/*if ( ( pObj = get_obj_carry( ch, arg2, ch ) ) == NULL )
		{
			act( "Co chcesz wyczy¶ciæ?", ch, pObj, NULL, TO_CHAR );
			return ;
		}*/

		if ( pObj == NULL ) pObj = get_eq_char( ch, WEAR_HOLD );

		if ( pObj == NULL )
		{
			act( "Co chcesz wyczy¶ciæ?", ch, pObj, NULL, TO_CHAR );
			return ;
		}

		if ( pObj->item_type != ITEM_PIPE )
		{
			send_to_char( "Mo¿esz czy¶ciæ jedynie fajki!\n\r", ch );
			return ;
		}
		else if ( pObj->value[ 0 ] == 1 )
		{
			act( "Mo¿e lepiej najpierw zga¶ $h?", ch, pObj, NULL, TO_CHAR );
			return ;
		}
		else if ( pObj->value[ 1 ] == 0 )
		{
			act( "Ogl±dasz z dum± $h, po czym pucujesz j± dok³adnie rêkawem. Teraz jest naprawdê czysta!", ch, pObj, NULL, TO_CHAR );
			act( "$n z dum± pucuje $h, u¶miechaj±c siê przy tym tajemniczo.", ch, pObj, NULL, TO_ROOM );
			return ;
		}
		else
		{
			pObj->value [ 1 ] = 0;
			pObj->value [ 2 ] = 0;
			act( "Wysypujesz zawarto¶æ $f. Teraz jest pusta.", ch, pObj, NULL, TO_CHAR );
			act( "$n wysypuje zawarto¶æ $f.", ch, pObj, NULL, TO_ROOM );
			return ;
		}
	}

	send_to_char( "Huh?\n\r", ch );
	return;
}


/* A tutaj moze w przyszlosci - zucie tytoniu! */

/*
void do_chew (CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *pObj;
    int amount;

    if ( argument[0] == '\0' )
    {
send_to_char( "{cChew what?\n\r", ch );
return;
    }
if ( ( pObj = get_obj_carry( ch, argument, ch ) ) == NULL )
{
send_to_char( "{cYou need some tobacco if you want to chew.\n\r",ch);
return;
}
if ( pObj->item_type != ITEM_CHEW_TOBACCO)
{
send_to_char( "{cYou need some tobacco if you want to chew.\n\r",ch);
return;
}
if ( pObj->value[1] <= 0 )
{
    send_to_char( "{cYou're all out of tobacco.\n\r", ch );
        extract_obj(pObj);
    return;
}
    act( "{c$n chews contentedly on some $p.",  ch, pObj, NULL, TO_ROOM );
    act( "{cYou chew contentedly on some $p.", ch, pObj, NULL, TO_CHAR );
SET_BIT(ch->act,TOBACCO_CHEWING);
*/
/* Mix up the numbers a bit */
/*
amount = number_range(1,2);
    if (pObj->value[0] > 0)
    {
        pObj->value[1] -= amount;
        capit2(pObj->short_descr);
        amount = number_range(1,6);
        switch ( amount )
        {
case 1:
    act( "{c$n grinds some $p between $s teeth.",  ch, pObj, NULL, TO_ROOM );
    act( "{cYou grind some $p between your teeth.",  ch, pObj, NULL, TO_CHAR );
    break;
    case 2:
    act( "{c$n grinds some $p between $s teeth.",  ch, pObj, NULL, TO_ROOM );
    act( "{cYou grind some $p between your teeth.",  ch, pObj, NULL, TO_CHAR );
    break;
    case 3:
    act( "{c$n grinds some $p between $s teeth.",  ch, pObj, NULL, TO_ROOM );
    act( "{cYou grind some $p between your teeth.",  ch, pObj, NULL, TO_CHAR );
    break;
    case 4:
    act( "{c$n grinds some $p between $s teeth.",  ch, pObj, NULL, TO_ROOM );
    act( "{cYou grind some $p between your teeth.",  ch, pObj, NULL, TO_CHAR );
break;
    case 5:
    act( "{c$n grinds some $p between $s teeth.",  ch, pObj, NULL, TO_ROOM );
    act( "{cYou grind some $p between your teeth.",  ch, pObj, NULL, TO_CHAR );
break;
default:
    act( "{c$n grinds some $p between $s teeth.",  ch, pObj, NULL, TO_ROOM );
    act( "{cYou grind some $p between your teeth.",  ch, pObj, NULL, TO_CHAR );
break;
}
}
    return;
}


void do_spit (CHAR_DATA *ch, char *argument)
{
char arg[MAX_INPUT_LENGTH];

if (!EXT_IS_SET(ch->act,TOBACCO_CHEWING))
{
send_to_char("{wBut you aren't chewing any tobacco.\n\r",ch);
return;
}

one_argument( argument, arg );

if ( arg == NULL)
{
act("{c$n spits out a juicy brown glob of tobacco juice.", ch, NULL, NULL, TO_ROOM );
act("{cYou spit out a juicy brown glob of tobacco juice.", ch, NULL, NULL, TO_CHAR );
EXT_REMOVE_BIT(ch->act,TOBACCO_CHEWING);
return;
}
if ( !str_prefix( arg, "stream" ))
{
act("{c$n spits a long, thin stream of tobacco juice.", ch, NULL, NULL, TO_ROOM );
act("{cYou spit a long, thin stream of tobacco juice.", ch, NULL, NULL, TO_CHAR );
EXT_REMOVE_BIT(ch->act,TOBACCO_CHEWING);
return;
}
if ( !str_prefix( arg, "spray" ))
{
act("{c$n spits a long spray of juicy brown tobacco juice.", ch, NULL, NULL, TO_ROOM );
act("{cYou spit a long spray of juicy brown tobacco juice.", ch, NULL, NULL, TO_CHAR );
EXT_REMOVE_BIT(ch->act,TOBACCO_CHEWING);
return;
}
if ( !str_prefix( arg, "ball" ))
{
act("{c$n hawks up a big ball of brown tobacco juice.", ch, NULL, NULL, TO_ROOM );
act("{cYou hawk up a big ball of brown tobacco juice.", ch, NULL, NULL, TO_CHAR );
EXT_REMOVE_BIT(ch->act,TOBACCO_CHEWING);
return;
}
}
*/
