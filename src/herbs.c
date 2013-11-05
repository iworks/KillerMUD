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
 * KILLER MUD is copyright 2009-2011 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * ZMIENIA£E¦ CO¦? DOPISZ SIÊ!                                         *
 *                                                                     *
 * Jaromir Klisz         (jklisz@gmail.com                ) [Drake   ] *
 * Pietrzak Marcin       (marcin@iworks.pl                ) [Gurthg  ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl        ) [Agron   ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: herbs.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/herbs.c $
 *
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "herbs.h"
#include "recycle.h"
#include "interp.h"
#include "projects.h"
#include "tables.h"
#include "money.h"

HERB_DATA * herb_free;
HERB_DATA * herb_list;

HERB_DATA *new_herb()
{
	HERB_DATA * new_herb;

	if ( !herb_free )
	{
		CREATE( new_herb, HERB_DATA, 1 );
	}
	else
	{
		new_herb	= herb_free;
		herb_free	= herb_free->next;
	}

	new_herb->next	= NULL;
	new_herb->id	= &str_empty[ 0 ];
	new_herb->name	= &str_empty[ 0 ];
	new_herb->name2	= &str_empty[ 0 ];
	new_herb->name3	= &str_empty[ 0 ];
	new_herb->name4	= &str_empty[ 0 ];
	new_herb->name5	= &str_empty[ 0 ];
	new_herb->name6	= &str_empty[ 0 ];

	return new_herb;
}

void free_herb( HERB_DATA * herb )
{
	HERB_DATA * pHerb;
	if ( !herb )
		return ;

	free_string( herb->id );
	free_string( herb->name );
	free_string( herb->name2 );
	free_string( herb->name3 );
	free_string( herb->name4 );
	free_string( herb->name5 );
	free_string( herb->name6 );

	if ( herb_list == herb )
	{
		herb_list = herb_list->next;
	}
	else
	{
		for ( pHerb = herb_list; pHerb; pHerb = pHerb->next )
		{
			if ( pHerb->next == herb )
			{
				pHerb->next = herb->next;
				break;
			}
		}
	}

	herb->next	= herb_free;
	herb_free	= herb;
	return ;
}

/*
void do_herblist( CHAR_DATA *ch, char *argument )
{
    HERB_DATA *pHerb;
    BUFFER *buffer = NULL;
    char buf [MAX_STRING_LENGTH];
    int count = 0;

    if ( !herb_list )
    {
        send_to_char( "Na mudzie nie zdefiniowano ¿adnych zió³ek.\n\r", ch );
        return;
    }

    buffer = new_buf();

    for ( pHerb = herb_list; pHerb; pHerb = pHerb->next )
    {
        if ( !pHerb->herb )
        continue;

        count++;

        sprintf( buf, "%3d) %-30.30s {CSektor{x: %s    {CPrawd.{x: %d\n\r",
                      count,
                      pHerb->herb->short_descr,
                      sector_table[pHerb->herb->value[4]].name,
                      pHerb->herb->value[5] );
        add_buf( buffer, buf );
    }

    sprintf( buf, "\n\rZnaleziono zió³ek: %d.\n\r", count );
    add_buf( buffer, buf );

    page_to_char( buf_string( buffer ), ch );
    free_buf( buffer );

    return;
}

void do_herb( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    HERB_DATA *pHerb;
    int skill;
    char arg1 [MAX_INPUT_LENGTH];

    if ( IS_NPC( ch ) )
    return;

    argument = one_argument( argument, arg1 );

    if ( !str_cmp( arg1, "search" ) )
    {
        if ( !ch->in_room )
        return;

        if ( IS_SET( ch->in_room->room_flags, ROOM_NO_HERBS ) )
        {
            send_to_char( "Nie widaæ tutaj ¿adnych ro¶lin przypominaj±cych zio³a.\n\r", ch );
            return;
        }

        skill = UMAX( 4, get_skill( ch, gsn_herbs_knowledge ) );

        for ( pHerb = herb_list; pHerb; pHerb = pHerb->next )
        {
            if ( pHerb->herb->value[4] == ch->in_room->sector_type )
            break;
        }

        if ( !pHerb )
        {
            send_to_char( "Nie widaæ tutaj ¿adnych ro¶lin przypominaj±cych zio³a.\n\r", ch );
            return;
        }

        obj = NULL;
        if ( ch->in_room->herbs_count > 10 && number_percent() < skill )
        for ( pHerb = herb_list; pHerb; pHerb = pHerb->next )
        {
            if ( pHerb->herb->value[4] != ch->in_room->sector_type )
            continue;

            if ( number_percent() < pHerb->herb->value[5] )
            {
                obj = create_object( pHerb->herb, FALSE );
                act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafiasz na $h.",
                     ch, obj, NULL, TO_CHAR );
                act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafia na co¶.",
                     ch, obj, NULL, TO_ROOM );
                obj_to_char( obj, ch );
                break;
            }
        }

        if ( !obj )
        {
            act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ ale nic nie znajdujesz.",
                 ch, NULL, NULL, TO_CHAR );
            act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ ale nic nie znajduje.",
                 ch, NULL, NULL, TO_ROOM );
            return;
        }
        ch->in_room->herbs_count++;
    }
    else if ( !str_cmp( arg1, "lore" ) )
    {
        argument = one_argument( argument, arg1 );

        if ( ( skill = get_skill( ch, gsn_herbs_knowledge ) ) <= 0 )
        {
        	send_to_char("Nie znasz siê na tym.\n\r", ch );
        	return;
        }

        if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
        {
            send_to_char( "Nie masz czego¶ takiego przy sobie.\n\r", ch );
            return;
        }

        if ( obj->item_type != ITEM_HERB )
        {
            send_to_char( "Ten przedmiot nie jest zio³em.\n\r", ch );
            return;
        }

    }
    else
    {
        do_function( ch, &do_help, "herb" );
    }

    return;
}

*/
// SKILL---------------------------------
void gether_comunicate( CHAR_DATA *ch, OBJ_DATA *obj, bool type )
{
	if(type)
	{
	switch(herb_table[obj->value[ 0 ]].plant_part)
	{
		case PLANT_PART_NONE:
		{
			act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafiasz na $h.",
                     ch, obj, NULL, TO_CHAR );
		    act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafia na co¶.",
                     ch, obj, NULL, TO_ROOM );
			break;
		}
		case PLANT_PART_STALK:                     //lodyga
		{
			act( "Badasz najbli¿sz± okolicê, znajdujesz ro¶linê któr± oczyszczasz z ga³±zek, korzeni, li¶ci w twojej d³oni pozostaje $p.",
                     ch, obj, NULL, TO_CHAR );
		    act( "$n bada najbli¿sz± okolicê po chwili podnosi jak±¶ ³odygê i chowa j±.",
                     ch, obj, NULL, TO_ROOM );
			break;
		}
		case PLANT_PART_LEAF:                      //lisc
		{
			act( "Rozgl±dasz siê dooko³±, szybko odnajdujesz i zrywasz $h.",
                     ch, obj, NULL, TO_CHAR );
		    act( "$n bada najbli¿sz± okolicê po chwili zrywaj±c kilka li¶ci.",
                     ch, obj, NULL, TO_ROOM );
			break;
		}
		case PLANT_PART_FLOWER:                    //kwiat
		{
			act( "Badasz najbli¿sz± okolicê w oczy wpada ci $p który od razu zrywasz.",
                     ch, obj, NULL, TO_CHAR );
		    act( "$n bada najbli¿sz± okolicê po chwili zastanowienia zrywa jaki¶ kwiat.",
                     ch, obj, NULL, TO_ROOM );
			break;
		}
		case PLANT_PART_ROOT:                      //korzen
		{
			act( "Rozgl±dasz siê wokó³ i po chwili dostrzegasz interesuj±c± ciê ro¶line, przez chwilê kopiesz w ziemi i wyci±gasz $h.",
                     ch, obj, NULL, TO_CHAR );
		    act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafia na co¶.",
                     ch, obj, NULL, TO_ROOM );
			break;
		}
		case PLANT_PART_SEEDS:                     //nasiona
		{
			act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafiasz na $h.",
                     ch, obj, NULL, TO_CHAR );
		    act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafia na co¶.",
                     ch, obj, NULL, TO_ROOM );
			break;
		}
		case PLANT_PART_BERRYS:                    //jagody
		{
			act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafiasz na $h.",
                     ch, obj, NULL, TO_CHAR );
		    act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafia na co¶.",
                     ch, obj, NULL, TO_ROOM );
			break;
		}
		case PLANT_PART_PERENNIAL:                 //bylina
		{
			act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafiasz na $h.",
                     ch, obj, NULL, TO_CHAR );
		    act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafia na co¶.",
                     ch, obj, NULL, TO_ROOM );
			break;
		}
		case PLANT_PART_FRUITS:                    //owoce
		{
			act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafiasz na $h.",
                     ch, obj, NULL, TO_CHAR );
		    act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafia na co¶.",
                     ch, obj, NULL, TO_ROOM );
			break;
		}
		case PLANT_PART_BULB:                      //cebulka
		{
			act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafiasz na $h.",
                     ch, obj, NULL, TO_CHAR );
		    act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafia na co¶.",
                     ch, obj, NULL, TO_ROOM );
			break;
		}
		case PLANT_PART_BUSH:                      //krzek
		{
			act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafiasz na $h.",
                     ch, obj, NULL, TO_CHAR );
		    act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafia na co¶.",
                     ch, obj, NULL, TO_ROOM );
			break;
		}
		case PLANT_PART_CACTUS:                   //kaktus
		{
			act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafiasz na $h.",
                     ch, obj, NULL, TO_CHAR );
		    act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafia na co¶.",
                     ch, obj, NULL, TO_ROOM );
			break;
		}
		case PLANT_PART_FUNGUS:                   //grzyb
		{
			act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafiasz na $h.",
                     ch, obj, NULL, TO_CHAR );
		    act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafia na co¶.",
                     ch, obj, NULL, TO_ROOM );
			break;
		}
	}
	}
	else if(!type)
	{
	switch(herb_table[obj->value[ 0 ]].plant_part)
	{
		case PLANT_PART_NONE:
		case PLANT_PART_STALK:                     //lodyga
		case PLANT_PART_LEAF:                      //lisc
		case PLANT_PART_FLOWER:                    //kwiat
		case PLANT_PART_ROOT:                      //korzen
		case PLANT_PART_SEEDS:                     //nasiona
		case PLANT_PART_BERRYS:					//jagody
		case PLANT_PART_PERENNIAL:               //bylina
		case PLANT_PART_FRUITS:					//owoce
		case PLANT_PART_BULB:					//cebulka
		case PLANT_PART_BUSH:					//krzek
		case PLANT_PART_CACTUS:                  //kaktus
		case PLANT_PART_FUNGUS:					//grzyb
		break;
	}
	}
}


void herb_to_char( CHAR_DATA *ch, HERB_IN_ROOM_DATA *herb)
{
    OBJ_DATA *obj;
    char text[ MAX_INPUT_LENGTH ];
    char herb_name[ MAX_INPUT_LENGTH ];
    int skill;

    obj = create_object( get_obj_index( 6 ), FALSE );
    if ( !obj )
    {
        return;
    }

    skill = get_skill( ch, gsn_herbs_knowledge );

    //Drake: Dorzucam sell_extracta bo zbyt du¿e dysproporcje by³y miêdzy skillami zarobkowymi. Jak kiedy¶ wejdzie alchemia to siê wywali (?).
    obj->wear_flags = ITEM_TAKE | ITEM_HOLD;
    EXT_SET_BIT( obj->extra_flags, ITEM_SELL_EXTRACT );
    obj->item_type = ITEM_HERB;
    /*
     * condition
     * im wyzsze wyszkolenie i trudnosc ziola tym wieksza szansa
     * na jakosc 95
     */
    obj->condition = UMIN( number_range( herb_table[herb->herb].difficult / 2 + skill / 2, 120 ), 95 );
    /**
     * pozostale 10 zalezy od szczescia,
     * uwaga moze wyjsc poza 100, dlatego potem normalizacja
     */
    if ( get_curr_stat( ch, STAT_LUC ) > number_range( MAX_STATS / 10, MAX_STATS ) )
    {
        obj->condition += number_range( 0, 10 );
    }
    obj->condition = UMIN( obj->condition, 100 );
    obj->value[ 0 ] = herb->herb; //numerek ziola w tabeli
    //modyfikator ceny w zaleznosci od trudnosci 1.05 - 3.5
    obj->cost = herb_table[herb->herb].cost * ( herb_table[herb->herb].difficult / ( 20 + herb_table[herb->herb].difficult / 5 ) +1 );
    obj->rent_cost = 1;
    obj->weight = number_range(1,2);

    //NA RAZIE IDA PRAWDZIWE NAZWY
    free_string( obj->short_descr ); obj->short_descr = str_dup( herb_table[herb->herb].name );

    //Drake: Doda³em dorzucanie 'zio³a i zio³a' do nazwy zbieranego zielska, w celu ³atwiejszego sprzedawania/pakowania.
    sprintf ( herb_name, "%s zio³o zio³a", herb_table[herb->herb].name );
    free_string( obj->name ); obj->name = str_dup( herb_name );

    free_string( obj->name2 ); obj->name2 = str_dup( herb_table[herb->herb].name2 );
    free_string( obj->name3 ); obj->name3 = str_dup( herb_table[herb->herb].name3 );
    free_string( obj->name4 ); obj->name4 = str_dup( herb_table[herb->herb].name4 );
    free_string( obj->name5 ); obj->name5 = str_dup( herb_table[herb->herb].name5 );
    free_string( obj->name6 ); obj->name6 = str_dup( herb_table[herb->herb].name6 );

    free_string( obj->description );
    sprintf( text, "Widzisz tu %s.", herb_table[herb->herb].name4  );
    obj->description = str_dup( text );

    free_string( obj->item_description );
    obj->item_description = str_dup( capitalize( herb_table[herb->herb].description ));

    gether_comunicate( ch, obj, TRUE);

    obj_to_char( obj, ch );

    return;
}
void do_herb( CHAR_DATA *ch, char *argument )
{
HERB_IN_ROOM_DATA *temp;
    sh_int skill
		, herb_diff //trudnosc ziola
		, skill_roll;
    char arg1 [MAX_INPUT_LENGTH];
	bool find = FALSE,notfinded = FALSE;

    if ( IS_NPC( ch ) )
    return;


    argument = one_argument( argument, arg1 );


        if ( !ch->in_room )
        return;

		if ( ( skill = get_skill( ch, gsn_herbs_knowledge ) ) <= 0 )
		{
			send_to_char( "Huh?\n\r", ch );
			return;
		}

		if(room_is_dark(ch, ch->in_room ))
		{
			send_to_char( "Ciemno¶æ widze, ciemno¶æ!!!.\n\r", ch );
			return;
		}

		if ( ch->mount )
		{
			send_to_char( "Twój wierzchowiec w milczeniu zaczyna wcinaæ jakie¶ zielsko.\n\r", ch );
			return;
		}

		//przeszukiwanie rooma kosztuje troche mova
		if ( ch->move <= 0 )
		{
			act( "Jeste¶ zbyt zmêczon<&y/a/e> by szukaæ zió³.", ch, NULL, NULL, TO_CHAR );
			return;
		}
		ch->move -= 1;//zjadamy move... Drake: -4 mv? Jeszcze powiedzmy przy bieganiu po lesie?
                    //                       Czyli co, szukanie zió³ bardziej mêczy od pracy w kopalni?! =.=
		if ( ch->move < 0 )
		{
			ch->move = 0;
		}

		//Brak zió³ w ROOMIE
		if( !(temp = ch->in_room->first_herb)){
			 act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ ale nic nie znajdujesz.",
                 ch, NULL, NULL, TO_CHAR );
            act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ ale nic nie znajduje.",
                 ch, NULL, NULL, TO_ROOM );

			return;
		}//koniec nie ma zió³


		//lecimy po li¶cie zió³
		// teraz mamy:
		skill = get_skill( ch, gsn_herbs_knowledge ); //skill gracz

		for(temp = ch->in_room->first_herb;temp;temp = temp->next)
	    {
				herb_diff = herb_table[temp->herb].difficult; //trudnosc znalezienia ziola
				if(temp->herb_status > 1) continue; //znaczy ze juz zerwane lub proba byla
				//jesli tutaj to sprawdzamy czy uda sie zebrac
				if(skill >= herb_diff)//proba zerwania
				{
					//skill_roll = number_range(1, 100);
					skill_roll = number_range(1, 100-herb_diff);
					find = TRUE;

					if ( FALSE ||  !str_cmp( ch->name, "Agron" ))
						{
      					print_char( ch, "\n\rRzut na skill:%d + trudnosc %d  >  skill = %d.\n\r",skill_roll,herb_diff,skill  );
						}
					if (skill_roll+herb_diff > skill ) //nie udany rzut na skill,  po prostu koles chujowy skill ma
					{


						temp->herb_status = 3;
						if(notfinded == FALSE){
							switch(number_range(1,2)){
							case 1:
								{
								act( "Znajdujesz i probujesz zebraæ znalezione zio³o, ale w ostatniej chwili rozpada siê ono na nieu¿yteczne kawa³eczki.",ch, NULL, NULL, TO_CHAR );
								act( "$n znajduje co¶ jednak w ostatniej chwili za³amuje rêce i rozsypuje dooko³a nieu¿yteczne szcz±tki.",ch, NULL, NULL, TO_ROOM );
								break;
								}
							case 2:
								{
								act( "Bez problemu odnajdujesz i szybkim ruchem starasz siê zebraæ zio³o, jednak ³apiesz w z³ym miejscu i rozrywasz je na kilka bezwarto¶ciowych czê¶ci.",ch, NULL, NULL, TO_CHAR );
								act( "$n bez problemu odnajduje i zrywa jak±¶ ro¶linê, ta jednak rozpada siê w jego rêkach.",ch, NULL, NULL, TO_ROOM );
								break;
								}

							}
							notfinded = TRUE;
						}
						WAIT_STATE( ch, 4 );
						continue;
					}
					else // zrywamy!
					{
						herb_to_char(ch,temp);
						temp->herb_status = 2;
						WAIT_STATE( ch, 4 );

					}

				}
				else continue;


		}

/*
                obj = create_object( pHerb->herb, FALSE );
                act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafiasz na $h.",
                     ch, obj, NULL, TO_CHAR );
                act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ i po chwili natrafia na co¶.",
                     ch, obj, NULL, TO_ROOM );
                obj_to_char( obj, ch );
  */
		WAIT_STATE( ch, 24 );
       if (!find)//czyli na koniec jesli przelecialo liste i nic nie zostalo znalezione
       {
            act( "Badasz najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ ale nic nie znajdujesz.",
                 ch, NULL, NULL, TO_CHAR );
            act( "$n bada najbli¿sz± okolicê w poszukiwaniu jakich¶ zió³ ale nic nie znajduje.",
                 ch, NULL, NULL, TO_ROOM );

			return;
		}
	//cos znalazl ale nie zebral
	if(notfinded) check_improve( ch, NULL, gsn_herbs_knowledge, FALSE, 12 );
    //tu skill jesli sie udalo, wieksza szansa na nauczenie
    else check_improve( ch, NULL, gsn_herbs_knowledge, TRUE, 10 );

    return;
}

/**
 * zeby drogie i bardzo drogie ziola nie wysypywaly sie jak z rekawa
 */
int generate_herb_chance_to_load( int chance_to_load, int cost, int difficult )
{
    cost -= 5 * difficult;

    if ( cost > 9 * RATTING_GOLD )
    {
        return ( number_percent() > 50 )? 1:0;
    }
    if ( cost > 8 * RATTING_GOLD )
    {
        return UMIN( number_range( 0,  2 ), chance_to_load );
    }
    if ( cost > 7 * RATTING_GOLD )
    {
        return UMIN( number_range( 0,  4 ), chance_to_load );
    }
    if ( cost > 6 * RATTING_GOLD )
    {
        return UMIN( number_range( 0,  8 ), chance_to_load );
    }
    if ( cost > 5 * RATTING_GOLD )
    {
        return UMIN( number_range( 0, 16 ), chance_to_load );
    }
    if ( cost > 4 * RATTING_GOLD )
    {
        return UMIN( number_range( 0, 24 ), chance_to_load );
    }
    if ( cost > 3 * RATTING_GOLD )
    {
        return UMIN( number_range( 0, 32 ), chance_to_load );
    }
    if ( cost > 2 * RATTING_GOLD )
    {
        return UMIN( number_range( 0, 48 ), chance_to_load );
    }
    if ( cost > 1 * RATTING_GOLD )
    {
        return UMIN( number_range( 0, 64 ), chance_to_load );
    }
    return chance_to_load;
}

//-------------------------------------
sh_int generate_herb( sh_int percent, sh_int sector_type )
{
    sh_int /*herb = 0,*/ i;
    int counter = 0, chosen;
    int chance_to_load;

    for(i=1;;i++)//zliczanie wszystkich ktore sie mieszcza w percent
    {
        if (herb_table[i].plant_part == 0) break;  //do konca doszlismy

        chance_to_load = generate_herb_chance_to_load( herb_table[i].sectors[sector_type], herb_table[i].cost, herb_table[i].difficult );

        if (chance_to_load < 1) continue; //znaczy ze prawd. = 0 w tym sektorze
        if (chance_to_load < percent) continue; //jesli wczesniejsze prawdopodobienstwo mniejsze

        counter += herb_table[i].sectors[sector_type];
    }
    chosen = number_range( 0, counter );
    if ( chosen > counter )
    {
        return 0;
    }
    counter = 0;
    for(i=1;;i++)
    {
        if (herb_table[i].plant_part == 0) break;  //do konca doszlismy

        chance_to_load = generate_herb_chance_to_load( herb_table[i].sectors[sector_type], herb_table[i].cost, herb_table[i].difficult );

        if (chance_to_load < 1) continue; //znaczy ze prawd. = 0 w tym sektorze
        if (chance_to_load < percent) continue; //jesli wczesniejsze prawdopodobienstwo mniejsze

        counter += herb_table[i].sectors[sector_type];
        if ( chosen <= counter )
        {
            return i;
        }
    }
    return 0;
}


HERB_IN_ROOM_DATA *start_herbs(HERB_IN_ROOM_DATA *first_herb_data, sh_int sector_type)
{
 HERB_IN_ROOM_DATA *herb=NULL, *temp=NULL;
 sh_int herb_count = 0,herb_count_prev = 0, counter, percent = 0, herb_temp = 0;


 herb_count_prev = number_range( 0,sector_table[sector_type].max_herb);
 herb_count = number_range(0,herb_count_prev);

 if(!herb_count) //czyli w tej chwili nie bedzie ziolka w roomie
     return NULL;
 else //czyli beda jakies ziolka
 {
     for(counter = 1;counter <= herb_count;counter++)
     {
		 percent = number_range( 1, 100 );
		 if( (herb_temp = generate_herb( percent, sector_type )) == 0 ) continue;//zadne sie nie wylosowalo, idziemy dalje
		 herb = new_herb_room_data();
		 herb->herb = herb_temp;
 		 herb->herb_status = 1;
 		 herb->next = NULL;

  		 if(first_herb_data == NULL)
	   	     first_herb_data = herb;
		 else
   		     temp->next = herb;

 		 temp = herb;
  	 }
     return first_herb_data;
 }
}

void update_herbs_room(ROOM_INDEX_DATA *room, const char *caller)
{
	HERB_IN_ROOM_DATA *temp, *prev=NULL, *herb = NULL, *zast=NULL;
	sh_int licznik = 0, wsp = 0, dorolka = 0;
	sh_int local_test = 0, random_reset_time = 0, plus_minus = 0;
	//rellik, komentujê bo funkcja odpalana w pêtli [20080428]
	//const char *call = "herbs.c => update_herbs_room";
	//save_debug_info( call, caller, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

	if(sector_table[room->sector_type].max_herb==0) return;   //czyli nie wystepuja ziola
	room->herb_update_timer--;
	if ( room->herb_update_timer<=0 )
	{
		room->herb_update_timer = sector_table[room->sector_type].herb_reset_time;
		random_reset_time = number_range ( 0, sector_table[room->sector_type].herb_reset_time/10);
		plus_minus = number_range(0,1);
		if(plus_minus == 0) room->herb_update_timer -= random_reset_time;
		else room->herb_update_timer += random_reset_time;

		// 2 czesci:
		//   	 1-update ziol istniejacych, czy odrastaja czy znikaja
		//       2-uzupelnianie pustych miejsc do maksa jesli wyjdzie szansa

		//czesc 1

		for(temp=room->first_herb;temp;temp=zast)
		{
			//prev = temp;
			// 1 - nie ruszone ,2 - zerwane, 3 - przeszukane(w tym resecie juz sie nie zerwie), na razie takie opcje rozpatrujemy tylko
			// pozniej mozna to rozwinac, na przyklad zaleznie od skila zrywajacego itd...
			if(temp->herb_status == 1)//nie ruszone
			{
				prev = temp;
				zast = temp->next;
				local_test=1;
				continue; //na razie lecimy dalej, mozna zaimplementowac wiedniecie itd...
			}
			else if (temp->herb_status == 2)//zerwane
			{

				//hmm powiedzmy ze rzut na trudnosc ziola, jesli wyjdzie to ziolo odrasta
				if ( number_percent() < herb_table[temp->herb].difficult )
				{
					temp->herb_status = 1;
					prev = temp;
					zast = temp->next;
					local_test=2;
				}
				//usuniecie
				else
				{
					if(prev != NULL)
					{
						prev->next = temp->next;
						zast = temp->next;
						local_test=3;
					}
					else
					{
						room->first_herb = temp->next;
						zast = temp->next;
						local_test=4;
					}

					temp->next = NULL;
					free_herb_room_data(temp);
					//temp=zast;

				}
			}
			else if(temp->herb_status == 3)//byla proba zerwania
			{
				temp->herb_status = 1;
				prev = temp;
				zast = temp->next;
				continue;
			}

		}

		//czesc 2
		for(temp=room->first_herb;temp;temp=temp->next) licznik++;  //zliczanie ilosci
		if (licznik >= sector_table[room->sector_type].max_herb)
		{
			return;   //nie mozna nic dodac
		}	else {

			dorolka = sector_table[room->sector_type].max_herb - licznik;
			for(;dorolka>0;dorolka--)
			{   //tu czy powinno cos urosnac zalezne od max_herb i herb_reset_time
				wsp = 2*sector_table[room->sector_type].max_herb + (100-sector_table[room->sector_type].herb_reset_time);
				if (wsp < number_range ( 1 , 100 )) continue;  //nie wyuszedl rzut
				else //ok dolosowujemy
				{

					if ( (wsp = generate_herb(number_range(1,100),room->sector_type))==0 ) continue;
					herb = new_herb_room_data();
					herb->herb = wsp;
					herb->herb_status = 1;
					herb->next = NULL;
					for(temp=room->first_herb;temp;temp=temp->next) if(temp->next == NULL) break;
					if (room->first_herb==NULL)
						room->first_herb = herb;
					else
						temp->next = herb;
				}
			}
		}

	}

	return;
}

void update_herbs(const char *caller)
{
AREA_DATA *pArea;
ROOM_INDEX_DATA *pRoom;
int vnum;
const char *call = "herbs.c => update_herbs";
	save_debug_info( call, caller, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
	{
		for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    	{
            if ( ( pRoom = get_room_index(vnum) ) )
				update_herbs_room(pRoom, call);
		}
	}
    return;
}

