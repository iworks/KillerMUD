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
 * KILLER MUD is copyright 2002-2012 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Skrzetnicki Krzysztof (tener@tenet.pl                ) [Tener     ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: artefact.c 11032 2012-02-25 11:25:45Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/artefact.c $
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <dirent.h>
#include "merc.h"
#include "db.h"
#include "tables.h"
#include "recycle.h"
#include "lang.h"
#include "olc.h"
#include "money.h"

void remove_name_from_artefact_list ( char *name );
void remove_char_from_artefact_list ( CHAR_DATA * ch );
int artefact_new_count( ARTEFACT_DATA *art );
void logoff_update ( char *name, time_t logoff );
void get_money_from_rent( CHAR_DATA* ch, OBJ_DATA* obj );

//////////NOWE FUNKCJE///////////

int is_artefact_vnum( int vnum )
{
    ARTEFACT_DATA * tmp = NULL;
    DEBUG_INFO( "is_artefact_vnum" );
    for ( tmp = artefact_system;tmp;tmp = tmp->next )
    {
        if ( tmp->avnum == vnum )
        {
            return 1;
        }
    }
    return 0;
}
int is_artefact( OBJ_DATA *obj )
{
    DEBUG_INFO( "is_artefact" );
    return is_artefact_vnum( obj->pIndexData->vnum );
}


//na przyklad jak cialko sie robi, to calkiem z listy wywala
//ownera ale zostaje na mudzie artef. count sie nie zmienia
void all_artefact_from_char( CHAR_DATA *ch )
{
	ARTEFACT_DATA * atmp = NULL;
	ARTEFACT_OWNER *otmp, *prev = NULL;
	OBJ_DATA *obj = NULL;

	for ( atmp = artefact_system;atmp;atmp = atmp->next )
	{
		otmp = atmp->first_owner;
		for ( ;otmp; )
		{
			if ( !strcmp( ch->name, otmp->owner ) )
			{
				if ( prev != NULL )
					prev->next = otmp->next;
				else
					atmp->first_owner = otmp->next;

				obj = create_object( get_obj_index( atmp->avnum ), FALSE );
				wiznet( "$N STRACIL(a) artefact : $p.", ch, obj, WIZ_ARTEFACT, 0, get_trust( ch ) );

				append_file_format_daily( ch, ARTEFACT_LOG_FILE, "-> STRACIL (zgon):%s vnum:%d count:%d max: %d (move)", obj->short_descr, atmp->avnum, atmp->count, atmp->max_count );

				extract_obj( obj );

				otmp->next = NULL;
				free_artefact_owner( otmp );

				//nastepny element
				if ( prev != NULL )
					otmp = prev->next;
				else
					otmp = atmp->first_owner;


			}
			else //to nie ten
			{
				prev = otmp;
				otmp = otmp->next;
			}
		}
	}

}

//heh zlicza ilosc po boot_db
//bo przez nieszczesne ladowanie pccorpsow moze sie ilosc zjebac...
void count_artefacts()
{
	ARTEFACT_DATA * atmp = NULL;
	ARTEFACT_OWNER *otmp = NULL;

    if ( !artefact_system )
    {
        log_string( "Artefact system: empty artefact list" );
        return ; //brak wpisow
    }

	append_file_format_daily( NULL, ARTEFACT_LOG_FILE, "PO REBOOCIE ZLICZANIE", NULL );

	for ( atmp = artefact_system;atmp;atmp = atmp->next )
	{
		if ( atmp->first_owner == NULL )
		{
			atmp->count = get_obj_index( atmp->avnum ) ->count;
			continue;
		}
		else
		{
			otmp = atmp->first_owner;
		}
		//to stwarza niebezpieczenstwo nie ruszac :)
		
		//while ( otmp )
		//{

		//	if ( otmp->status == 0 )   //zlicza tych co na rencie
		//		licznik++;
		//	otmp = otmp->next;
		//}
		//atmp->count = licznik;
		//licznik = 0;
		//a teraz ilosc
		//atmp->count += (get_obj_index( atmp->avnum )->count);

		append_file_format_daily( NULL, ARTEFACT_LOG_FILE, "%s vnum:%d count:%d max:%d", get_obj_index( atmp->avnum ) ->short_descr, atmp->avnum, atmp->count, atmp->max_count );
	}

	append_file_format_daily( NULL, ARTEFACT_LOG_FILE, "PO REBOOCIE KONIEC", NULL );

	return ;
}

void restore_char_main( OBJ_DATA *obj, CHAR_DATA *ch )
{
	ARTEFACT_DATA * art = NULL;
	ARTEFACT_OWNER *ow = NULL;


	if ( is_artefact( obj ) )
	{
		//leci po liscie
		for ( art = artefact_system;art;art = art->next )
		{
			if ( art->avnum == obj->pIndexData->vnum )
			{
				if ( IS_IMMORTAL( ch ) )   //jesli immo to dodaje counta
				{
					art->count++;
					break;
				}
				if ( art->first_owner == NULL )   //jesli pusta lista, dodaje na poczatek
				{
					artefact_to_char( obj, ch );
					art->count++;
					break;
				}
				for ( ow = art->first_owner;ow;ow = ow->next )
				{
					if ( !strcmp( ch->name, ow->owner ) )
					{
						if ( ow->status == 0 )   //jesli name jest taki sam i status=0 to zmienia na 1
						{
							ow->status = 1;
							break;
						}
					}
					if ( ow->next == NULL )   //jesli doszedl do konca  to zglasza bleda i dodaje na koniec
					{
						artefact_to_char( obj, ch );
						art->count++;
						break;
					}
				}
			}
		}
	}

}

void restore_char_half( OBJ_DATA *obj, CHAR_DATA *ch )
{
	OBJ_DATA * otmp = NULL, *obj_next = NULL;

	if ( obj->contains )
	{
		for ( otmp = obj->contains;otmp != NULL;otmp = obj_next )
		{
			obj_next = otmp->next_content;
			restore_char_half( otmp, ch );
		}
		restore_char_main( obj, ch );
	}
	else
		restore_char_main( obj, ch );

	//    return 0;
}

//sprawdza postac czy ma artafakty po wejsciu na muda
//lub zmienia status jesli ma
void restore_char_on_artefact_list( CHAR_DATA *ch )
{
	OBJ_DATA * obj, *obj_next = NULL;

	remove_name_from_artefact_list ( ch->name );

	for ( obj = ch->carrying;obj != NULL;obj = obj_next )
	{
		obj_next = obj->next_content;

		restore_char_half( obj, ch );
	}

	//dla hoarda
	for ( obj = ch->hoard;obj != NULL;obj = obj_next )
	{
		obj_next = obj->next_content;

		restore_char_half( obj, ch );
	}
	



	return ;

}

void quit_artefact_char( CHAR_DATA *ch )
{
	OBJ_DATA * obj = NULL, *obj_next = NULL;
	ARTEFACT_DATA *art = NULL;
	ARTEFACT_OWNER *ow = NULL;


	//to na immortali jak maja to niech nie psuja licznika
	if ( IS_IMMORTAL( ch ) )
	{
		for ( obj = ch->carrying;obj != NULL;obj = obj_next )
		{
			obj_next = obj->next;
			if ( is_artefact( obj ) ) extract_artefact( obj );
		}
		return ;
	}

	for ( art = artefact_system;art;art = art->next )
	{
		for ( ow = art->first_owner;ow;ow = ow->next )
		{
			if ( !strcmp( ow->owner, ch->name ) )
			{
				if ( ow->status == 1 )
				{
					ow->status = 0;
					//break;
				}
			}
		}
	}


}


//ktos podniosl artefakt i to go wrzuca na liste
void artefact_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
	char buf[ MAX_STRING_LENGTH ];
	ARTEFACT_DATA *atmp = NULL;
	ARTEFACT_OWNER *ow, *otmp = NULL;
	int licznik;

	for ( atmp = artefact_system;atmp;atmp = atmp->next )
	{

		if ( atmp->avnum == obj->pIndexData->vnum )
		{
			licznik = 0;
			for ( otmp = atmp->first_owner;otmp;otmp = otmp->next )
			{
				licznik++;
				if ( otmp->next == NULL ) break;
			}
			if ( licznik > atmp->max_count )   //jak by sie cos jeblo
			{
				sprintf( buf, "Artefact error posiadacze: %d, max_count: %d", licznik, atmp->max_count );
				log_string( buf );
			}

			ow = new_artefact_owner();

			if ( atmp->first_owner != NULL )
				otmp->next = ow;
			else
				atmp->first_owner = ow;

			ow->next = NULL;
			ow->owner = str_dup( ch->name );
			ow->status = 1;
			ow->last_logoff = current_time;

			wiznet( "$N {GDOSTAL(a){x artefakt : $p.", ch, obj, WIZ_ARTEFACT, 0, get_trust( ch ) );
			append_file_format_daily( ch, ARTEFACT_LOG_FILE, "-> DOSTAL :%s vnum:%d count:%d max: %d (move)", obj->short_descr, atmp->avnum, atmp->count, atmp->max_count );

			return ;
		}
	}
}

//gdy gracz wywala artefakt to zdejmujemy go raz z listy
//ktos pozbyl sie artefaktu, wywalam go z listy raz
//znaczy zostaje na mudzie ale nie ma go gracz
void artefact_from_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
	ARTEFACT_DATA * atmp = NULL;
	ARTEFACT_OWNER *otmp, *prev = NULL;

	for ( atmp = artefact_system;atmp;atmp = atmp->next )
	{
		if ( obj->pIndexData->vnum == atmp->avnum )
		{
			for ( otmp = atmp->first_owner;otmp;otmp = otmp->next )
			{
				if ( !strcmp( ch->name, otmp->owner ) )
				{
					if ( prev != NULL )
						prev->next = otmp->next;
					else
						atmp->first_owner = otmp->next;

					otmp->next = NULL;
					free_artefact_owner( otmp );


					wiznet( "$N {RSTRACIL(a){x artefakt : $p.", ch, obj, WIZ_ARTEFACT, 0, get_trust( ch ) );
					append_file_format_daily( ch, ARTEFACT_LOG_FILE, "-> STRACIL :%s vnum:%d count:%d max: %d (move)", obj->short_descr, atmp->avnum, atmp->count, atmp->max_count );
					return ;
				}
				else //to nie ten
					prev = otmp;
			}
		}
	}

}

//sprawdza contenery czy w nich nie siedzi artefakt
//+ dodaje lub zdejmuje z listy
void container_artefact_check( OBJ_DATA *obj, CHAR_DATA *ch, int status )
{
	//status: 	0 - ktos podnosi container, wrzucamy go na liste
	//		1 - ktos wywala container, wywalamy go z listy

	OBJ_DATA * tobj = NULL, *obj_next = NULL;

	for ( tobj = obj->contains; tobj != NULL; tobj = obj_next )
	{
		obj_next = tobj->next_content;
                container_artefact_check( tobj, ch, status ); // musi byc rekurencyjne
		if ( is_artefact( tobj ) )
		{
			if ( status == 0 ) artefact_to_char( tobj, ch );
			else if ( status == 1 ) artefact_from_char( tobj, ch );
		}
	}

	return ;
}


//++count, jesli za duzo to zwraca 0
void create_artefact( int vnum )
{
	ARTEFACT_DATA * ar = NULL;

	for ( ar = artefact_system;ar;ar = ar->next )
	{
		if ( ar->avnum == vnum )
		{
			if ( ar->count >= ar->max_count )
                        {
                                char buf[ MSL ];
				sprintf( buf, "create_artefact: za duzo artow: %d, max_count: %d", ar->count, ar->max_count );
				log_string( buf );
                        }

                	ar->count++;
			append_file_format_daily(NULL, ARTEFACT_LOG_FILE,"-> STWORZONO vnum:%d count:%d max:%d",vnum,ar->count,ar->max_count);
			return;
		}
	}

	return ; //nie artefakt
}


void extract_artefact( OBJ_DATA *obj )
{
	ARTEFACT_DATA * ar = NULL;

	//jesli ma cos w sobie if(obj->contains)
	// extract_artefact_container(obj);

	for ( ar = artefact_system;ar;ar = ar->next )
	{
		if ( ar->avnum == obj->pIndexData->vnum )
		{
			if ( ar->count > 0 )
			{
				ar->count--;
				append_file_format_daily(NULL, ARTEFACT_LOG_FILE, "-> ZNISZCZONO %s vnum:%d count:%d max:%d",obj->short_descr,obj->pIndexData->vnum,ar->count,ar->max_count);
				return ;
			}
		}
	}

}

/*if( is_artefact(obj) ) extract_artefact(obj);*/

void extract_artefact_container( OBJ_DATA *obj )
{
	OBJ_DATA * tobj = NULL, *obj_next = NULL;

	for ( tobj = obj->contains; tobj != NULL /* o tutaj */ ; tobj = obj_next )
	{
		obj_next = tobj->next_content;
		if ( is_artefact( tobj ) )
			extract_artefact( tobj );
                // ta funkcja musi byæ rekurencyjna

                // nie ma potrzeby sprawdzenia czy
                // tobj->contains != NULL,
                // jest to ju¿ sprawdzane w ciele
                // tej w³a¶nie pêtli for (patrz wy¿ej)

                //extract_artefact_container( tobj );
	}

	return ;
}

//to calkowita obsluga ladowania artefactow
//sprawdzanie czy mozna + ladowanie + ew. zakladanie
void load_artefact( ROOM_INDEX_DATA *room, OBJ_DATA *obj, CHAR_DATA *ch )
{
	char buf[ MAX_INPUT_LENGTH ];
	ARTEFACT_DATA *atmp = NULL;
	ARTEFACT_LOADER *ltmp = NULL;
	OBJ_DATA *loaded_obj;

	int type;
	int vnum = 0;


	if ( room != NULL )
	{
		vnum = room->vnum;type = 0;
	}
	else if ( obj != NULL )
	{
		vnum = obj->pIndexData->vnum;type = 1;
	}
	else if ( ch != NULL )
	{
		vnum = ch->pIndexData->vnum;type = 2;
	}
	else return ;


	//gdzie
	for ( atmp = artefact_system;atmp;atmp = atmp->next )
	{
		for ( ltmp = atmp->loader;ltmp;ltmp = ltmp->next )
		{
			//jesli nie zgadza sie typ (np: obj=obj) i vnumy to wychodzimy
			if (!( ltmp->type == type && ltmp->vnum == vnum ))
			{
                                continue;
                        }

                        //jesli room to sprawdzanie czy juz tam nie ma
			if (( type == 0 ) &&
                            ( count_obj_list( get_obj_index( atmp->avnum ), room->contents ) > 0 ) )
			{
				sprintf( buf, "NIE LADUJE do room a (juz jest): %d do %d", atmp->avnum, room->vnum );
				wiznet( buf, NULL, NULL, WIZ_ARTEFACTLOAD, 0, 39 );
				continue;
			}

                        //aktualizacja ilosci
			atmp->count = artefact_new_count( atmp );


			//tera prawd. jesli mniejsze to nastepny loader
			if ( number_range( 1, ltmp->probup ) > ltmp->probdown )
			{
				continue;
			}

                        //juz max artefaktu, nastepny loader
			if ( atmp->count >= atmp->max_count ) continue;


			//ladujemy bo wsie warunki spelnione
			loaded_obj = create_object ( get_obj_index( atmp->avnum ), FALSE );
			create_artefact( loaded_obj->pIndexData->vnum );

			//dokad ladujemy
			if ( type == 0 )   //do rooma
			{
				sprintf( buf, "Artefakt [%d] za³adowany do rooma [%d].", loaded_obj->pIndexData->vnum, room->vnum );
				obj_to_room( loaded_obj, room );
				wiznet( buf, NULL, NULL, WIZ_ARTEFACTLOAD, 0, 39 );
			}
			else if ( type == 1 )   //do srodka objectu
			{
				sprintf( buf, "Artefakt [%d] za³adowany do wnêtrza obiektu [%d],", loaded_obj->pIndexData->vnum, obj->pIndexData->vnum );
				obj_to_obj( loaded_obj, obj );
				wiznet( buf, NULL, NULL, WIZ_ARTEFACTLOAD, 0, 39 );
					//okresla room
				if ( obj->in_room )
					sprintf( buf, "	w roomie [%d]", obj->in_room->vnum );
				if ( obj->carried_by )
					sprintf( buf, "	na mobie [%d] w roomie [%d]", obj->carried_by->pIndexData->vnum, obj->carried_by->in_room->vnum );
				wiznet( buf, NULL, NULL, WIZ_ARTEFACTLOAD, 0, 39 );
				}
			else if ( type == 2 )   //mobowi
			{
				sprintf( buf, "Artefakt [%d] za³adowany mobowi [%d] roomie [%d].", loaded_obj->pIndexData->vnum, ch->pIndexData->vnum, ch->in_room->vnum );
				obj_to_char( loaded_obj, ch );
				wiznet( buf, NULL, NULL, WIZ_ARTEFACTLOAD, 0, 39 );
					//+zakladanie jesli moze (nie sprzedawca)
				if ( ch->pIndexData->pShop == NULL && loaded_obj->wear_loc == WEAR_NONE && can_see_obj( ch, loaded_obj ) )
				{
					if ( loaded_obj->item_type == ITEM_WEAPON )
					{
						if ( !get_eq_char( ch, WEAR_WIELD ) )
							wield_weapon( ch, loaded_obj, TRUE );
					}
					else
						wear_obj( ch, loaded_obj, FALSE );
				}

			} //end dokad
//		} //end typ=typ, vnum=vnum
	} //end po loaderach
} //end po artefaktach
}

//trzy ponizsze to sprawdzanie czy room, obj, mob
//jest na liscie tych do(na) ktorych sie cos laduje

int is_artefact_load_room( ROOM_INDEX_DATA *room )
{
	ARTEFACT_DATA * atmp = NULL;
	ARTEFACT_LOADER *ltmp = NULL;

	for ( atmp = artefact_system; atmp; atmp = atmp->next )
	{
		for ( ltmp = atmp->loader; ltmp; ltmp = ltmp->next )
		{
			if ( (ltmp->type == room->vnum) == ltmp->vnum ) return 1;
		}
	}

	return 0;
}

int is_artefact_load_obj( OBJ_INDEX_DATA *pObj )
{
	ARTEFACT_DATA * atmp = NULL;
	ARTEFACT_LOADER *ltmp = NULL;

	for ( atmp = artefact_system; atmp; atmp = atmp->next )
	{
		for ( ltmp = atmp->loader; ltmp; ltmp = ltmp->next )
		{
			if ( pObj->vnum == ltmp->vnum ) return 1;
		}
	}

	return 0;
}

int is_artefact_load_mob( CHAR_DATA *ch )
{
	ARTEFACT_DATA * atmp = NULL;
	ARTEFACT_LOADER *ltmp = NULL;

	for ( atmp = artefact_system; atmp; atmp = atmp->next )
	{
		for ( ltmp = atmp->loader; ltmp; ltmp = ltmp->next )
		{
			if ( ch->pIndexData->vnum == ltmp->vnum ) return 1;
		}
	}

	return 0;
}

//poprawia liste na reboocie lub zamknieciu muda...
void reboot_artefact_list( void )
{
	ARTEFACT_DATA * atmp = NULL;
	ARTEFACT_OWNER *otmp = NULL;
	int licznik = 0;

	if ( !artefact_system ) return ; //brak wpisow
	for ( atmp = artefact_system;atmp;atmp = atmp->next )
	{

		if ( atmp->first_owner == NULL )
		{
			atmp->count = 0;
			continue;
		}
		else
		{
			otmp = atmp->first_owner;
		}
		//to stwarza niebezpieczenstwo nie ruszac :)
		while ( otmp )
		{

			otmp->status = 0;
			licznik++;
			otmp = otmp->next;
		}
		atmp->count = licznik;
		licznik = 0;
	}

	return ;

}


int artefact_new_count( ARTEFACT_DATA *art )
{
	//char buf[MAX_INPUT_LENGTH];
	OBJ_DATA * obj = NULL;
	ARTEFACT_OWNER *otmp = NULL;
	int licznik = 0;

	char *name = NULL;

	//wpierw to co w grze, jesli nosi je PC to nie liczymy

	name = get_obj_index( art->avnum ) ->name;

	for ( obj = object_list; obj != NULL; obj = obj->next )
	{
		if ( !is_name( name, obj->name ) )   //nie ten
			continue;

		//ten
		if ( obj->carried_by == NULL )   /*nie noszony*/
		{
			licznik++;
			continue;
		}
		else if ( obj->carried_by != NULL && IS_NPC( obj->carried_by ) )   /*przez NPC'a*/
		{
			licznik++;
			continue;
		}
		else continue; //nosi PC nie liczymy
	}

	//teraz te co na liscie czyli pc

	otmp = art->first_owner;

	while ( otmp )
	{
		licznik++;
		otmp = otmp->next;
	}

	return licznik;

}
/*******************************************************************************/
//Tera leci gowno do sprawdzania czy ktos nie przetrzymuje artefaktow
//dluzej niz xx

//totalne wywalenie imie gracza z listy -> na razie sluzy przy restarcie muda, jak nie mamy CHAR_DATA
//wszystkie obj leca w pizdu wiec artefakty tez
//NIE UZYWAC GDY PRZEDMIOTY ZOSTAJA NA MUDZIE

void remove_char_from_artefact_list ( CHAR_DATA * ch )
{
  remove_name_from_artefact_list ( ch->name );
}

void remove_name_from_artefact_list ( char *name )
{
	ARTEFACT_DATA * atmp;
	ARTEFACT_OWNER *otmp, *prev = NULL, *otmp_new = NULL;

	for ( atmp = artefact_system;atmp;atmp = atmp->next )
	{
		for ( otmp = atmp->first_owner;otmp;otmp = otmp_new )
		{
			if ( !strcmp( name, otmp->owner ) )
			{
				if ( prev != NULL )
				{
					prev->next = otmp->next;
					otmp_new = otmp->next;
				}
				else
				{
					atmp->first_owner = otmp->next;
					otmp_new = otmp->next;
				}

				otmp->next = NULL;
				free_artefact_owner( otmp );

				if ( atmp->count > 0 ) atmp->count--; //jeden ubywa
			}
			else //to nie ten
			{
				prev = otmp;
				otmp_new = otmp->next;
			}
		}
		prev = NULL;
	}
}

//ta fukcja szuka arta do wywalenia i od razu go wywala z listy
OBJ_DATA * przeszukanie( OBJ_DATA *list, sh_int vnum, OBJ_DATA *container )
{
	OBJ_DATA * found = NULL;
	OBJ_DATA *obj, *prev_obj = NULL;

	for ( obj = list; obj; obj = obj->next_content )
	{
		if ( obj->contains )
			found = przeszukanie( obj->contains, vnum, obj );

		if ( found )
			return found;

		if ( obj->pIndexData->vnum == vnum )
		{
		  if ( container == NULL && prev_obj == NULL )
		    {
		      bugf("Small probability bug encountered. See comments on how to deal with it.");
		      /* 
			 Mieli¶my pecha i trafi³a nam siê sytuacja taka, ¿e usuwany artefakt jest pierwszym elementem w ekwipunku gracza. W tym momencie prev_obj jest równy NULL i container tak¿e.
			 Mo¿na sprawiæ, by ten kod nigdy siê nie wychrznia³: wystarczy ¿e container nie bêdzie wska¼nikiem na pojemnik, ale bardziej ogólnie, wska¼nikiem na zmienn± w pojemniku przechowuj±c± zawarto¶æ. To znaczy, jedno z dwóch:
			 ch->carrying # dla postaci
			 cointainer->contains # dla pojemników
			 
			 Taka zmiana jest jednak czasoch³onna i podatna na dalsze b³êdy. Zamiast tego zwracamy NULL.
			 
		       */
		      return NULL;
		    }
			if ( container != NULL && prev_obj == NULL )
			{
				container->contains = obj->next_content;
			}
			else
				prev_obj->next_content = obj->next_content;

			return obj;
		}
		prev_obj = obj;
	}

	return NULL;
}

/* Tener: znikanie artów po pewnym czasie.
   S³owa kluczowe: rt_art, gsn_art_destroy */

/* Funkcja do wysylania komunikatow o stanie naladowania artefaktu
   Sprawdza czy trzeba wyslac, wysyla i aktualizuje timer
 */
void show_rt_art_destroy_info( OBJ_DATA * obj )
{
  CHAR_DATA * ch;
  AFFECT_DATA * aff;

  /* jezeli nie obiekt nie ma informacji o tym kto go nosi nie wyswietlany jest komunikat.
     UWAGA: celowym, nie do konca oczywistym nastepstwem jest to, ze arty w pojemnikach
     takze nie beda miec wyswietlanych komunikatow o updacie czasu.

     timer jest aktualizowany, ale o mniejsz± warto¶æ ni¿ w przypadku wy¶wietlenia komunikatu.
  */

  aff = affect_find( obj->affected, gsn_art_destroy );
  if (! aff )
    return;

  if ( (current_time / 60) < aff->modifier )
    return;

  aff->modifier = (current_time / 60) + RT_ART_SHOW_TIME_ONE; // na razie mala wartosc

  ch = obj->carried_by;
  if (! ch )
    return;

  /* ch istnieje, wiec wyswietlimy mu komunikat. zwiekszamy 'aff->modifier' by kolejny komunikat byl sporo pozniej */

  aff->modifier += RT_ART_SHOW_TIME_TWO; // a tu przychrzaniamy znacznie wieksza wartosc, bo byl (bedzie) komunikat

  /* wrzucamy komunikaty */
  {
    int life_percent = UMAX( 0, (aff->rt_duration * 100) / MAX_ARTEFACT_RT_DURATION );
    char buf[ MSL ];

    if ( life_percent >= 97 )
      {
	char * comm;
	switch( obj->gender )
	  {
	  case GENDER_MESKI: // stó³
	  case GENDER_ZENSKI: // noga
	  case GENDER_NIJAKI: // pismo
	  case GENDER_NONE: // ??
	  default: // ??
	    comm = "rozb³yska" ;
	    break;
	  case GENDER_MESKOOSOBOWY: // narty
	  case GENDER_ZENSKOOSOBOWY: // slepia, lzy
	    comm = "rozb³yskaj±";
	    break;
	  }
	sprintf(buf, "$p %s na chwilê bardzo jasnym ¶wiat³em, które po chwili znika zupe³nie.", comm );
	act( buf, ch, obj, NULL, TO_CHAR );
      }
    else if ( life_percent < 5 )
      {
	char * comm1;
	char * comm2;
	char * comm3;

	switch( obj->gender )
	  {
	  case GENDER_MESKI: // stó³
	    comm1 = "staje"; comm2 = "pó³przezroczysty"; comm3 = "jego"; break;
	  case GENDER_ZENSKI: // noga
	    comm1 = "staje"; comm2 = "po³przezroczysta"; comm3 = "jej"; break;
	  case GENDER_NIJAKI: // pismo
	    comm1 = "staje"; comm2 = "pó³przezroczyste"; comm3 = "jego"; break;
	  case GENDER_NONE: // ??
	  default: // ??
	    comm1 = "staje"; comm2 = "pó³przezroczyste"; comm3 = "tego";
	    append_file_format_daily
	    (
	    		ch,
	    		ARTEFACT_LOG_FILE,
	    		"-> _: Obiekt %s, kod odmiany %d - nieprawid³owo ustawiona odmiana!",
	    		obj->name,
	    		obj->gender
	    );
	    break;
	  case GENDER_MESKOOSOBOWY: // narty
	  case GENDER_ZENSKOOSOBOWY: // slepia, lzy
	    comm1 = "staj±"; comm2 = "pó³przezroczyste"; comm3 = "ich"; break;
	    break;
	  }
	sprintf(buf, "Przez moment $p %s siê %s, a %s kontury zamazane i nieostre.", comm1, comm2, comm3 );
	act( buf, ch, obj, NULL, TO_CHAR );
      }
  }
}

/* test na obecnosc w obiekcie affectu ktoremu uplynal juz czas dzialania
   'obj' - obiekt do niszczenia
   'time_elapsed' - ile czasu nalezy "odj±æ" od rt_duration. wykorzystywany przy zabieraniu arta postaci offline */
bool check_rt_art_destroy( OBJ_DATA * obj, int time_elapsed )
{
   AFFECT_DATA *paf;

   for( paf = obj->affected; paf; paf = paf->next )
      {
	if( (paf->type == gsn_art_destroy) && (paf->rt_duration >= 0) && (paf->rt_duration <= time_elapsed) )
	    {
	       return TRUE;
	    }
      }

   return FALSE;
}

/* usun arty ktorym uplynela zywotnosc z postaci o imieniu name */
void remove_artefact_art_destroy( char *name )
{
    CHAR_DATA * ch;
    ch = load_char_remote( name );
    /* je¿eli 'ch' nie istnieje */
    if ( !ch )
    {
        char buf[MSL];
        sprintf(buf, "%s: brak pliku postaci %s", __func__, name );
        log_string( buf );
        return;
    }
    save_char_obj( ch, FALSE, TRUE );
    free_char( ch );
}

/* Koniec znikania artów po czasie */

/* zabierz od 'ch' kasê za przechowywanie 'obj' od ostatniego renta */

void get_money_from_rent( CHAR_DATA* ch, OBJ_DATA* obj )
{
  int cost, period_h, period_d, period_m;
  int copper = money_count_copper( ch );
  int bank   = ch->bank;

  //odjac kase za przedmiot*dni
  cost = obj->rent_cost;
  if ( cost < 0 )
  {
      cost = 0;
  }

  period_d = ( current_time - ch->pcdata->last_logoff ) / ( 24 * 60 * 60 );
  period_h = ( current_time - ch->pcdata->last_logoff ) / ( 60 * 60 );
  period_m = ( current_time - ch->pcdata->last_logoff ) / 60;

  if ( period_d >= 1 ) cost = UMAX( 0, cost * period_d );
  else if ( period_h >= 1 ) cost = UMAX( 0, ( cost * period_h ) / 24 );
  else if ( period_m >= 1 ) cost = UMAX( 0, ( cost * period_m ) / ( 24 * 60 ) );
  else cost = 1;

  cost = UMAX( 1, ( cost * get_rent_rate( ch ) / 100 ) );

  if ( copper + ch->bank < cost )
  {
      money_reset_character_money( ch );
      ch->bank = 0;
  }
  else
  {
      if ( copper < cost )
      {
          ch->bank += copper;
          money_reset_character_money ( ch );
          ch->bank -= cost;
      }
      else
      {
          money_reduce( ch, cost );
      }
  }

  if ( cost )
  {
      append_file_format_daily
          (
           ch,
           MONEY_LOG_FILE,
           "-> S: %d %d (%d), B: %d %d (%d) - oplata za rent przetrzymanego artefaktu (lub takiego ktoremu skonczyla sie zywotnosc) (przy jego zabieraniu)",
           copper,
           money_count_copper( ch ),
           money_count_copper( ch ) - copper,
           bank,
           ch->bank,
           ch->bank - bank
          );
  }
  return;
}

void remove_artefact_remote( char *name, ush_int vnum )
{
	CHAR_DATA * ch;
	OBJ_DATA *obj;
	char buf[ MAX_STRING_LENGTH ];
	char buf1[ MAX_STRING_LENGTH ];

	sprintf( buf1, "imie: %s vnum: %d", name, vnum );
	log_string( buf1 );

	//sprawdzalismy juz czy istnieje plik wiec:
	ch = load_char_remote( name );

    if ( ch == NULL )
    {
        bugf( "load_char_remote: no match '%s'.", name );
        remove_name_from_artefact_list( ch->name );
        return;
    }

	if ( !ch->carrying )
	{
		free_char( ch  );
		return;
	}

	//dobra to szuka i od razu wywala z listy
	if ( ch->carrying->pIndexData->vnum == vnum )
	{
		obj = ch->carrying;
		ch->carrying = ch->carrying->next_content;
	}
	else if ( ( obj = przeszukanie( ch->carrying, vnum, NULL ) ) == NULL )
	{
		remove_name_from_artefact_list( ch->name );
		restore_char_on_artefact_list( ch );
		return ;

	}

	sprintf( buf, "ZABRANO PRZETRZYMYWANY ARTEFAKT: Posiadacz:%s Avnum:%s,%d", ch->name, obj->name, obj->pIndexData->vnum );
	log_string( buf );

	append_file_format_daily(NULL, ARTEFACT_LOG_FILE, "%s -> PRZETRZYMANY :%s vnum:%d (DEL)",ch->name, obj->short_descr, obj->pIndexData->vnum);

	get_money_from_rent( ch, obj );

	//sprawdzic czy kase zapisuje na nowo
	//daty nie zmieniac przypadkiem !!!
	//logowac to

	//tu koncowe rzeczy
	obj->carried_by	= NULL;
	obj->next_content	= NULL;
	//to sprawdzic czy w workach tez liczone
	ch->carry_number	-= get_obj_number( obj );
	ch->carry_weight	-= get_obj_weight( obj );


	save_char_obj( ch, FALSE, TRUE );  //z TRUE nei zapisuje nowej daty
	//to cos zle dziala, dodaje zamiast odejmowac ;)
	remove_name_from_artefact_list( ch->name );
	restore_char_on_artefact_list( ch );

	free_char( ch );


	return ;
}


typedef struct artefact_storer ARTEFACT_STORER;
struct artefact_storer
{
	char *name;
	ush_int vnum;
	ARTEFACT_STORER *next;
	bool valid;
};

ARTEFACT_STORER *storer_free = NULL;
ARTEFACT_STORER *new_storer_data( void )
{
	static ARTEFACT_STORER storer_data_zero;
	ARTEFACT_STORER *storer;

	if ( storer_free == NULL )
		CREATE( storer, ARTEFACT_STORER, 1 );
	else
	{
		storer = storer_free;
		storer_free = storer_free->next;
	}

	*storer = storer_data_zero;
	VALIDATE( storer );
	return storer;
}

void free_storer_data( ARTEFACT_STORER *storer )
{
	if ( !IS_VALID( storer ) ) return ;
	INVALIDATE( storer );
	storer->next = storer_free;
	storer_free = storer;
}


//leci po liscie artow i sprawdza wszystkie pliki graczy z llisty
void scan_players_for_artefacts()
{
    struct dirent * pDirent = NULL;
    DIR *pDir = NULL;
    ARTEFACT_DATA *atmp = NULL;
    ARTEFACT_OWNER *otmp = NULL;
    ARTEFACT_STORER *storer = NULL, *temp_storer = NULL, *first_storer = NULL;
    int czas;
    pDir = opendir( "../player/" );

    if ( !pDir )
    {
        bugf( "Cannot open players directory. Skipping check." );
        return; // can't continue.
    }

    //jedziemy po plikach
    while ( TRUE )
    {
        pDirent = readdir( pDir );
        if ( !pDirent )
        {
            break;
        }
        for ( atmp = artefact_system; atmp; atmp = atmp->next )
        {
            for ( otmp = atmp->first_owner;otmp;otmp = otmp->next )
            {
                {
                    CHAR_DATA * ch;
                    ch = load_char_remote( otmp->owner );
                    if ( !ch )
                    {
                        remove_name_from_artefact_list( otmp->owner );
                        continue;
                    }
                    otmp->last_logoff = ch->pcdata->last_logoff;
                    free_char( ch );
                }
                remove_artefact_art_destroy( otmp->owner );
                //i tu tera sprawdzanie przetrzymywania
                czas = ( current_time - ( otmp->last_logoff ) ) / ( 24 * 60 * 60 );
                if ( atmp->max_day < czas )   //przetrzymane, zabieramy kolesiowi
                {
                    storer = new_storer_data();
                    storer->name = otmp->owner;
                    storer->vnum = atmp->avnum;
                    storer->next = NULL;
                    if ( first_storer == NULL )
                    {
                        first_storer = storer;
                    }
                    else
                    {
                        temp_storer->next = storer;
                    }
                    temp_storer = storer;
                }
            } /*alast=atmp;*/
        } //fory
        break;
    }
    closedir( pDir );
    //dobra to teraz wywalanie,modlmy sie zeby dzialalo
    for ( temp_storer = first_storer;temp_storer;temp_storer = temp_storer->next )
    {
        remove_artefact_remote( temp_storer->name, temp_storer->vnum );
    }
    return ;
}

//********************KONIEC PRZYTRZYMUJACYCH*******************//
//to jest do_owhere() tylko zmodyfikowany na potrzeby do_astat()
void awhere( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_INPUT_LENGTH ];
	BUFFER *buffer = NULL;
	OBJ_DATA *obj;
	OBJ_DATA *in_obj;
	bool found;
	int vnum = -1;

	found = FALSE;

	buffer = new_buf();

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Find what?\n\r", ch );
		return ;
	}

	if ( is_number( argument ) )
		vnum = atoi( argument );

	for ( obj = object_list; obj != NULL; obj = obj->next )
	{
		if ( vnum > 0 )
		{
			if ( !can_see_obj( ch, obj ) || obj->pIndexData->vnum != vnum )
				continue;
		}
		else
		{
			if ( !can_see_obj( ch, obj ) || !is_name( argument, obj->name ) )
				continue;
		}

		found = TRUE;

		for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj );

		if ( in_obj->carried_by != NULL && can_see( ch, in_obj->carried_by )
		     && in_obj->carried_by->in_room != NULL )
			sprintf( buf, " %s jest noszone przez %s [Room %d]\n\r",
			         obj->short_descr, PERS( in_obj->carried_by, ch ),
			         in_obj->carried_by->in_room->vnum );
		else if ( in_obj->in_room != NULL && can_see_room( ch, in_obj->in_room ) )
			sprintf( buf, " %s jest w %s [Room %d]\n\r",
			         obj->short_descr, in_obj->in_room->name,
			         in_obj->in_room->vnum );
		else
			sprintf( buf, " %s jest gdzies\n\r", obj->short_descr );

		sprintf( buf, "%s", capitalize( buf ) );
		add_buf( buffer, buf );

	}

	if ( !found )
		send_to_char( "	Niczego takiego nie ma aktualnie w grze.\n\r", ch );
	else
		page_to_char( buf_string( buffer ), ch );

	free_buf( buffer );
}

