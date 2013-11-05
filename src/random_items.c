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
 * Zdziech Tomasz        (tzdziech@gmail.com            ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: update.c 8611 2010-02-23 21:24:03Z void
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/12.02/src/random_items.c $
 *
 ***********************************************************************
 *
 * Random_items napisany przez:
 *		Krzysztofa Jaronia (Razor)
 *		(randomize, generate, weapon, armor_head, armor_feet, armor_hands, ring)
 *
 *		Tomasza Zdziecha (Agron)
 *		(poprawki&dopiski, amulet, gem, cape, add_new_affect)
 *
 * Garloop - poprawki 2003-05-03
 * Garloop - poprawki 2003-05-23 - 12 dodatkowych item_sword
 * Garloop - poprawki 2003-12-17 - 13 dodatkowych item_whip, opisy/random_amulety
 * Kainti  - poprawki 2004-03-01 - Wieksze zroznicowanie obrazen flailow i spearek, 5 nowych item_flail
 * Agron   - 19-04-2004 - Shield zmienione na nowy typ
 * Kainti  - 23-04-2004 - Troszeczke nowych sword, short sword, mace.
 * Garloop - 24-04-2004 - Z 16 nowych gem-ów i dosyæ duzo desc do istniej±cych.
 * Kainti  - 09-07-2004 - generate_earring, generate_armor_legs
 * Kainti  - 21-07-2004 - generate_armor_arms
 * Kainti  - 22-07-2004 - generate_bracelet
 * Kainti  - 06-08-2004 - 26 nowych opisow capow
 * Gurthg  - 2004-11-11 - 12 nowych gemów klasy 7 - tworzonych na
 *                        podstawie nicków, zmiana condition przedmiotów
 *                        na takie od 1 do 99, oraz wy³±czenie condition
 *                        gemów, znaczy ich condition jest wyliczana
 *                        oddzielnie i im wy¿sza klasa kamienia tym mniej
 *                        uszkodzonyon bêdzie
 * Kainti  - 2005-01-21 - Przyciecie wszystkich jednorecznych randomow do max sredniej 9
 * Kainti  - 2005-01-27 - Rozbudowane losowanie nazw jednorecznych swordow ( dokladnie 1166 roznych nazw )
 * Kainti  - 2005-01-28 - Rozbudowane losowanie nazw short swordow ( dokladnie 230 roznych nazw )
 * Gurthg  - 2005-02-20 - Powa¿ne ¶ciêcie randowowych przedmiotów, wyciêcie applayów, TO_STAT
 * Gurthg  - 2005-02-25 - Refaktoring.
 * Kainti  - 2006-05-16 - Uzaleznienie szansy na staly affect i dlugosci czasu trwania czasowki od pointsow (poziomu randoma)
 *						  W tym szansa na naprawde dluga czasowke
 * Gurthg  - 2009-04-09 - Dodalem randomowe ksiegi czarow, zostaly jednoczesnie
 *                        podlaczone do make_corpse, dzieki czemu z odpowiednia
 *                        szansa sa ladowane na kazdym zabitym mobie zostalo
 *                        wydzielone do wlasnego pliku random_spellbooks.c
 *
 * Nie dajamy ¿adnych pozytywnych bojowych applayów, bo to bezsensu, od
 * tego s± magowie i klerycy... :D wyj±tkiem s± randomowe przedmioty
 * rzadkie.
 *
 * LIMITY!!!
 * HP         max 10
 * MOVE       max 10
 * SKILL      max 10
 * LANG       max 20
 * RESIST     max 10
 * RESIST DAM max  5
 * MEMING ... ? nie wiem jak tu, bo trza powi±zaæ kr±g i ilo¶æ!!!
 *
 * Niech randowmy maj± sens, bo zalew ¶wiata przez czary z przedmiotów, to
 * BARDZO z³a droga.
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
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "money.h"

/* OK male info randomizowane graty sa wzorowane na systemie gratow diablo
 * mud pozwala na calkowite przerobiebie jakiegos grata, obiekt jesli
 * sie rozni od wzorca pIndexdata jest zapisywany w pliku gracza
 * nie wiem i nie sprawdzalem jak jest z progami, choc mam w planach
 * zrobienie kilku wzorcowych progow i podczepiac je pod generowane rzeczy
 * wracajac do tematu, generowanie jest dosc rozbudowane ze wzgledu na kod
 * praktycznie nie da sie tego jakos zautomatyzowac, wiec samo generowanie
 * to jeden wielki -s-w-i-t-c-h-(-)- , nie probowalem tablicowac mozliwosci generowanych
 * gratow bo jest zbyt wiele zaleznosci i taka tablica bylaby ogromna...
 * na razie sa funkcje generujace bronie, he³my, pierscionki, tarcze, zbroje,
 * rekawice, buty i amulety. kiedys beda wszystkie typy :)
 * jesli ktos chce pozmieniac parametry musi sie troche nameczyc zeby przesledzic
 * gdzie co jest, kod jest w miare hehe czytelny :)
 * to w jaki sposob bedzie sie ustawiac czy mob ma ladowac takie graty
 * bedzie zalezec od flagi act(?) albo w zaladowaniu w resetach rooma specjalnego
 * grata. grat bedzie randomizowany w momencie smierci moba i ladowany do
 * jego corpsa. nie przewiduje zakladania tych gratow na moby...flagi moga
 * nie pasowac etc. dodatkowo bedzie mozliwosc ladowania mobowi grata random
 * z progow
 *
 *
 *
 *
 * zmieniam system wyceniania rzeczy na wspolczynnikowy
 * kazdy sprzet bedzie mial swoja poczatkowa cene i w zaleznosci co mu tam jeszcze
 * dolosuje to bedzie dodawany wspolczynnik np:
 * cena tarczy=10, losuje drewno(czyli slaba) wsp+=0,(szkoada ze nie ma ulamkow)
 * +10ac wsp+=10, +do skilla bash wsp+=20
 * no i teraz 10(cena)*(10+20)=300
 * jakos pokombinuje coby bylo ladnie
*/
#define MAGIC_ITEMS	100
/*
 * zdefiniowanie kosztów za poszczególne dodatki
 */
#define COST_AFF_FLYING                800
#define COST_AFF_REGENERATION         2000
#define COST_AFF_BRAVE_CLOAK          1800
#define COST_AFF_COMPREHEND_LANGUAGES  600
#define COST_AFF_DARK_VISION           550
#define COST_AFF_DETECT_ALIGMENT       400
#define COST_AFF_DETECT_HIDDEN        1000
#define COST_AFF_DETECT_INVIS         1500
#define COST_AFF_DETECT_MAGIC          600
#define COST_AFF_DETECT_UNDEAD         800
#define COST_AFF_EYES_OF_THE_TORTURER 1000
#define COST_AFF_ENERGY_SHIELD        2000
#define COST_AFF_FLOAT                 450
#define COST_AFF_FREE_ACTION          1500
#define COST_AFF_HASTE                2000
#define COST_AFF_INFRARED              250
#define COST_AFF_INVISIBLE            3000
#define COST_AFF_PROTECT_ALIGMENT      900
#define COST_AFF_WATERBREATH           250
#define COST_AFF_WATERWALK             250

#define COST_APPLY_AC                   55
#define COST_APPLY_DAMROLL              30
#define COST_APPLY_HITROLL              40
#define COST_APPLY_MEMMING             600
#define COST_APPLY_SKILL                50
#define COST_APPLY_STAT                800
//dla resist bash/slash/pierce/magic uzywac tego costa *2
#define COST_APPLY_RESIST               33
//dla saving_rod i saving_petri tego costa /2
#define COST_APPLY_SAVING              450
#define COST_APPLY_LANG                 20

#define COST_ITEM_MAGIC                 50
#define COST_ITEM_BLESS                 80
#define COST_ITEM_GLOW                  40

#define COST_WEAPON_FLAMING            750
#define COST_WEAPON_FROST              500
#define COST_WEAPON_POISON             250
#define COST_WEAPON_SHARP              150
#define COST_WEAPON_SHOCKING           750
#define COST_WEAPON_VAMPIRIC          2050
#define COST_WEAPON_VORPAL             950
#define COST_WEAPON_KEEN               400
#define COST_WEAPON_THUNDERING         250

#define NUMBER_PERCENT_FOR_STAT         10

void randomize_values( OBJ_DATA *obj, int *points );
void generate_weapon( OBJ_DATA *obj, int* points );
void generate_armor_body( OBJ_DATA *obj, int* points );
void generate_armor_head( OBJ_DATA *obj, int* points );
void generate_armor_hands( OBJ_DATA *obj, int* points );
void generate_armor_feets( OBJ_DATA *obj, int* points );
void generate_armor_shield( OBJ_DATA *obj, int* points );
void generate_ring( OBJ_DATA *obj, int* points, bool perm );
void generate_amulet( OBJ_DATA *obj, int* points );
void generate_gem( OBJ_DATA *obj, int* points );
void generate_cape( OBJ_DATA *obj, int* points );
void generate_earring( OBJ_DATA *obj, int* points );
void generate_armor_legs( OBJ_DATA *obj, int* points );
void generate_armor_arms( OBJ_DATA *obj, int* points );
void generate_bracelet( OBJ_DATA *obj, int* points );

// void add_new_affect( OBJ_DATA *objekt, int type, int loc, int mod, int level, BITVECT_DATA * bitv, int dur, bool last );

BITVECT_DATA * get_bitvector ();
int get_cost_of_bitvector(BITVECT_DATA * bitv);
int	generate_aff_duration( int *points, int *koszt );

/*
 * procedurka wpisujaca do extra_flags i wear_flags2
 * uwaga: wear_flags2 nie jest zerowane poniewaz przy tworzeniu
 * przedmiotow wpisywane jest tam ktora profesja nie moze uzywac...
 */

void generate_extra( OBJ_DATA *obj )
{
    int chance;
    ext_flags_clear( obj->extra_flags );
    ext_flags_clear( obj->wear_flags2 );

    switch ( number_range( 1, 12 ) )
    {
        case 1:
            EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
            break;
        case 2:
            EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
            break;
        case 3:
        case 4:
            EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_NEUTRAL );
            break;
        default :
            break;
    }

    /* rasy */
    do
    {
        switch ( number_range( 1, 10 ) )
        {
            case 1:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOGNOM );
                break;
            case 2:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOHALFELF );
                break;
            case 3:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NODWARF );
                break;
            case 4:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOHALFORC );
                break;
            case 5:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOHALFLING );
                break;
            case 6:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOELF );
                break;
            case 7:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOHUMAN );
                break;
            default :
                break;
        }
    }
    while ( number_percent() < 35 );


    /* profesji */
    do
    {
        switch ( number_range( 0, 10 ) )
        {
            case 0:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOMAG );
                break;
            case 1:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOCLE );
                break;
            case 2:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOTHI );
                break;
            case 3:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOWAR );
                break;
            case 4:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOPAL );
                break;
            case 5:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NODRUID );
                break;
            case 6:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOBARBARIAN );
                break;
            case 7:
                break;
            case 8:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOBARD );
                break;
            case 9:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOBLACKKNIGHT );
                break;
            case 10:
                EXT_SET_BIT( obj->wear_flags2, ITEM_NOSHAMAN );
                break;
            default :
                break;
        }
    }
    while ( number_percent() < 35 );


    do
    {
        chance = number_percent();
        if (chance < 3) // 2%
        {
            EXT_SET_BIT( obj->extra_flags, ITEM_NODROP );
        }
        else if ( chance < 4 ) // 3%
        {
            EXT_SET_BIT( obj->extra_flags, (number_percent() > 49)? ITEM_NOREMOVE:ITEM_NOREPAIR );
        }
        else if ( chance < 6 ) // 5 %
        {
            EXT_SET_BIT( obj->extra_flags, ITEM_BURN_PROOF );
            obj->cost += 30;
        }
        else if ( chance < 11 ) // 10%
        {
            EXT_SET_BIT( obj->extra_flags, ITEM_BLESS );
            obj->cost += COST_ITEM_BLESS;
        }
        else if ( chance < 21 ) // 20%
        {
            EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );
            obj->cost += COST_ITEM_MAGIC;
        }
    }
    while ( number_percent() < 25 );

    return;
}

/* funcja jak ponizej ale sterowalna*/
OBJ_DATA* rand_item( int type, int points )
{
	OBJ_DATA * object;
	int wear_flags, item_type;

	if ( points < 0 || type < 0 || type > 13 )
    {
		return NULL;
    }

	object = create_object( get_obj_index( OBJ_VNUM_RANDOM_ITEM ), FALSE );

	if ( !object )
    {
		return NULL;
    }

    /**
     * repair limits
     */
    object->repair_limit     = 10 + points + number_range( -9, 9 );
    object->repair_counter   = number_range( 0, object->repair_limit );
    object->repair_penalty   = number_range( 1, URANGE( 1, 10 - points, 10 ) );
    object->repair_condition = 70 + number_range( 0, 30 );


    /**
     * kazdy random ma 25% szansy na bycie nienaprawialnym
     */
    if ( number_percent() < 25 )
    {
        EXT_SET_BIT( object->extra_flags, ITEM_NOREPAIR );
    }

	switch ( type )
	{
		case 0:
			wear_flags = ITEM_TAKE | ITEM_WEAR_HEAD;
			item_type = ITEM_ARMOR;
			break;
		case 1:
			wear_flags = ITEM_TAKE | ITEM_WEAR_BODY;
			item_type = ITEM_ARMOR;
			break;
		case 2:
			wear_flags = ITEM_TAKE | ITEM_WEAR_HANDS;
			item_type = ITEM_ARMOR;
			break;
		case 3:
			wear_flags = ITEM_TAKE | ITEM_WEAR_FEET;
			item_type = ITEM_ARMOR;
			break;
		case 4:
			wear_flags = ITEM_TAKE | ITEM_WIELD;
			item_type = ITEM_WEAPON;
			break;
		case 5:
			wear_flags = ITEM_TAKE | ITEM_WEAR_SHIELD;
			item_type = ITEM_SHIELD;
			break;
		case 6:
			wear_flags = ITEM_TAKE | ITEM_WEAR_ABOUT;
			item_type = ITEM_CLOTHING;
			break;
		case 7:
			wear_flags = ITEM_TAKE | ITEM_WEAR_FINGER;
			item_type = ITEM_JEWELRY;
			break;
		case 8:
			wear_flags = ITEM_TAKE | ITEM_WEAR_NECK;
			item_type = ITEM_JEWELRY;
			break;
		case 9:
			wear_flags = ITEM_TAKE | ITEM_HOLD;
			item_type = ITEM_GEM;
			break;
		case 10:
			wear_flags = ITEM_TAKE | ITEM_WEAR_EAR;
			item_type = ITEM_JEWELRY;;
			break;
		case 11:
			wear_flags = ITEM_TAKE | ITEM_WEAR_LEGS;
			item_type = ITEM_ARMOR;;
			break;
		case 12:
			wear_flags = ITEM_TAKE | ITEM_WEAR_ARMS;
			item_type = ITEM_ARMOR;;
			break;
		case 13:
			wear_flags = ITEM_TAKE | ITEM_WEAR_WRIST;
			item_type = ITEM_JEWELRY;;
			break;
		default:
			return NULL;
	}

    points = UMIN( points, 7 );
	object->wear_flags = wear_flags;
	object->item_type = item_type;

	if ( item_type == ITEM_GEM )
    {
		generate_gem( object, &points );
// tak co by mia³y jak±¶ inn± condition ni¿ 100, choæ dla gemów bêdzie to
// du¿o wy¿sza warto¶æ ni¿ dla pozosta³ych przedmiotów
		object->condition = number_range( 1 + points*10, 100 );
	}
	else
    {
		switch ( wear_flags )
		{
			case ITEM_TAKE | ITEM_WEAR_HEAD:
				generate_armor_head( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_BODY:
				generate_armor_body( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_ABOUT:
				generate_cape( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_HANDS:
				generate_armor_hands( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_FEET:
				generate_armor_feets( object, &points );
				break;

			case ITEM_TAKE | ITEM_WIELD:
				generate_weapon( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_SHIELD:
				generate_armor_shield( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_FINGER:
				generate_ring( object, &points, FALSE );
				break;

			case ITEM_TAKE | ITEM_WEAR_NECK:
				generate_amulet( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_EAR:
				generate_earring( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_LEGS:
				generate_armor_legs( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_ARMS:
				generate_armor_arms( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_WRIST:
				generate_bracelet( object, &points );
				break;

			default : return NULL;
		}
/* tak co by mia³y jak±¶ inn± condition ni¿ 100 */
		object->condition = number_range( 1, 99 );
	}

	/* hmm zeby czasem graty nie mialy ujemnej wagi;) */
	object->weight = UMAX( object->weight, 1 );

    /* normalizujemy cene */
    object->cost *= RATTING_SILVER;
    /* oraz koszt renta */
    object->rent_cost = 2 * RENT_COST( object->cost );

	return object;
}

/* funkcja zwracajaca obiekt z randomizowanymi statystykami
 * losuje jedynie jakiego typu ma byc obiekt
 * losuje równie¿ jeko trwa³o¶æ
 */
OBJ_DATA* randomize_item( int points )
{
	OBJ_DATA * object;
	int wer = 26, wear_flags, item_type;

	if ( points < 0 )
		return NULL;

	/* pier¶cienie, amulety, bransoletki i kolczyki od 6 punktow */
	if ( points < 6 )
		wer = 22;

	object = create_object( get_obj_index( OBJ_VNUM_RANDOM_ITEM ), FALSE );

	if ( !object )
		return NULL;

	switch ( number_range( 0, wer ) )
	{
		case 0: wear_flags = ITEM_TAKE | ITEM_HOLD;item_type = ITEM_GEM;break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5: wear_flags = ITEM_TAKE | ITEM_WEAR_BODY;item_type = ITEM_ARMOR;break;
		case 6:
		case 7:
		case 8: wear_flags = ITEM_TAKE | ITEM_WEAR_SHIELD;item_type = ITEM_SHIELD;break;
		case 9:
		case 10: wear_flags = ITEM_TAKE | ITEM_WIELD;item_type = ITEM_WEAPON;break;
		case 11:
		case 12: wear_flags = ITEM_TAKE | ITEM_WEAR_HEAD;item_type = ITEM_ARMOR;break;
		case 13:
		case 14: wear_flags = ITEM_TAKE | ITEM_WEAR_HANDS;item_type = ITEM_ARMOR;break;
		case 15:
		case 16: wear_flags = ITEM_TAKE | ITEM_WEAR_FEET;item_type = ITEM_ARMOR;break;
		case 17:
		case 18: wear_flags = ITEM_TAKE | ITEM_WEAR_LEGS;item_type = ITEM_ARMOR;break;
		case 19:
		case 20: wear_flags = ITEM_TAKE | ITEM_WEAR_ARMS;item_type = ITEM_ARMOR;break;
		case 21:
		case 22: wear_flags = ITEM_TAKE | ITEM_WEAR_ABOUT;item_type = ITEM_CLOTHING;break;
		case 23: wear_flags = ITEM_TAKE | ITEM_WEAR_NECK;item_type = ITEM_JEWELRY;break;
		case 24: wear_flags = ITEM_TAKE | ITEM_WEAR_FINGER;item_type = ITEM_JEWELRY;break;
		case 25: wear_flags = ITEM_TAKE | ITEM_WEAR_EAR;item_type = ITEM_JEWELRY;break;
		case 26: wear_flags = ITEM_TAKE | ITEM_WEAR_WRIST;item_type = ITEM_JEWELRY;break;
		default: wear_flags = ITEM_TAKE;item_type = ITEM_GEM;break;
	}

	points = URANGE( 0, points, 7 );

	object->wear_flags = wear_flags;
	object->item_type = item_type;

	if ( item_type == ITEM_GEM )
		generate_gem( object, &points );
	else switch ( wear_flags )
		{
			case ITEM_TAKE | ITEM_WEAR_HEAD	:
				generate_armor_head( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_BODY :
				generate_armor_body( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_ABOUT :
				generate_cape( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_HANDS :
				generate_armor_hands( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_FEET	:
				generate_armor_feets( object, &points );
				break;

			case ITEM_TAKE | ITEM_WIELD	:
				generate_weapon( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_SHIELD :
				generate_armor_shield( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_FINGER :
				generate_ring( object, &points, FALSE );
				break;

			case ITEM_TAKE | ITEM_WEAR_NECK :
				generate_amulet( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_EAR :
				generate_earring( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_LEGS :
				generate_armor_legs( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_ARMS :
				generate_armor_arms( object, &points );
				break;

			case ITEM_TAKE | ITEM_WEAR_WRIST :
				generate_bracelet( object, &points );
				break;

			default: return NULL;
		}

	/* hmm zeby czasem graty nie mialy ujemnej wagi;) */
	object->weight = UMAX( object->weight, 1 );

	/* tak co by mia³y jak±¶ inn± condition ni¿ 100 */
	object->condition = number_range( 25, 90 );

    /* normalizujemy cene */
    object->cost *= RATTING_SILVER;

	return object;
}

/* generujaca bronie
 * dzialanie points - decyduje jak dobry ma byc generowany grat
 * points w zakresie:
 * 0 - slabizna, podstawowe typy broni
 * 1 - szansa na stopien lepszy typ broni
 * 2 - to samo co 1
 * 3 - bron bez bajerow, losowana z puli wszystkich podtypow broni
 * 4 - 6, dodawane bajery na valuesach (poison, v[5]++ etc)
 * > 6 - dodatkowo losowany JEDEN tylko affect (jakis glupi)
 */
void generate_weapon( OBJ_DATA *obj, int* points )
{
    /* dla jakiej cholery jest 'txt' i 'text'?! */
    char text[ MAX_INPUT_LENGTH ];
    char txt[ MAX_INPUT_LENGTH ];

    char *name = "n";
    char *sh_desc = "n";
    char *sh_des = "n";

    char
        *name2 = "n",
        *name3 = "n",
        *name4 = "n",
        *name5 = "n",
        *name6 = "n";

    /* to raczej do wywalenia */
    char
        *name_des = "n",
        *name2_des = "n",
        *name3_des = "n",
        *name4_des = "n",
        *name5_des = "n",
        *name6_des = "n";

    /* to raczej bêdzie do wywalenia */
    char *sh_fir = "n";
    char
        *name_fir = "n",
        *name2_fir = "n",
        *name3_fir = "n",
        *name4_fir = "n",
        *name5_fir = "n",
        *name6_fir = "n";

    bool go = TRUE, enchanted = FALSE, second = FALSE, third = FALSE;
    int weapon_type = -1, val, wer = 4;
    int type = 0, loc = 0, mod = 0, level = 0, dur = 0;
    BITVECT_DATA * bitv = &AFF_NONE;
    bool one_hand;

    /* najpierw jaki typ broni
     * 25% - miecze( zwykle, 2h i short )
     * 15% - topory
     * 10% - sztylety
     * 10% - wlocznie
     * 10% - flaile
     * 10% - mace
     *  5% - baty
     * 10% - drzewcowe
     *  5% - laski
     */
    switch ( number_range( 1, 20 ) )
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            weapon_type = WEAPON_SWORD;
            break;
        case 6:
        case 7:
        case 8:
            weapon_type = WEAPON_AXE;
            break;
        case 9:
        case 10:
            weapon_type = WEAPON_DAGGER;
            break;
        case 11:
        case 12:
            weapon_type = WEAPON_SPEAR;
            break;
        case 13:
        case 14:
            weapon_type = WEAPON_FLAIL;
            break;
        case 15:
        case 16:
            weapon_type = WEAPON_MACE;
            break;
        case 17:
            weapon_type = WEAPON_WHIP;
            break;
        case 18:
        case 19:
            weapon_type = WEAPON_POLEARM;
            break;
        case 20:
        default:
            weapon_type = WEAPON_STAFF;
            break;
    }

    obj->value[ 0 ] = weapon_type;
    generate_extra( obj );

    if ( *points < 3 )
    {
        wer = ( *points + 1 );
    }
    if ( *points > 3 )
    {
        second = TRUE;
    }
    if ( *points > 6 )
    {
        third = TRUE;
    }

    switch ( obj->value[ 0 ] )
    {

        case WEAPON_SWORD:

            obj->material = 2;

            obj->value[ 1 ] = 1;
            obj->value[ 2 ] = 2;
            switch ( number_range( 1, 6 ) )
            {
                case 1:
                case 2:
                case 3:
                case 4:
                    obj->value[ 3 ] = 3; // damtype "ciêcie"
                    break;
                case 5:
                    obj->value[ 3 ] = 42; // damtype "zamaszyste ciêcie"
                    break;
                case 6:
                    obj->value[ 3 ] = 41; // damtype "precyzyjne ciêcie"
                    break;
            }
            obj->value[ 5 ] = 0;
            obj->value[ 6 ] = 0;

            switch(number_range(1,3))
            {
                // 			  int waga[3][2];
                // 			  waga[0][0] = 90;
                // 			  waga[0][1] = 220;

                case 1://miecze jednoreczne zwykle
                    obj->weight = number_range( 90, 220 );

                    /* kostki obra¿eñ */
                    {
                        int kostki[8][2] = { { 2,3 }, { 1,5 }, { 2,4 },
                            { 1,7 }, { 2,5 }, { 3,3 },
                            { 3,4 }, { 2,6 }};
                        int tmp = number_range( 0, (wer*2)-1 );

                        obj->value[ 1 ] = kostki[tmp][0];
                        obj->value[ 2 ] = kostki[tmp][1];
                    }

                    /* + do hita i dam */
                    if( wer > 3 && obj->value[ 1 ] * obj->value[ 2 ] < 11 )
                    {
                        obj->value[ 5 ] = number_range( 0, 2 );
                        obj->value[ 6 ] = number_range( 0, 2 );
                    }

                    //no to ta, pierw bedzie losowanie rodzaju opisu z 'meski' badz 'zenski', meski czesciej
                    //a pozniej losowanie pierwszej czesci opisu, a pozniej drugiej.
                    if( number_range( 1, 3 ) != 1 )
                    {
                        switch( number_range(1,53))
                        {
                            case 1:
                                name_fir = "d³ugi";
                                sh_fir = "d³ugi";
                                name2_fir = "d³ugiego";
                                name3_fir = "d³ugiemu";
                                name4_fir = "d³ugi";
                                name5_fir = "d³ugim";
                                name6_fir = "d³ugim";
                                break;
                            case 2:
                                name_fir = "ostry";
                                sh_fir = "ostry";
                                name2_fir = "ostrego";
                                name3_fir = "ostremu";
                                name4_fir = "ostry";
                                name5_fir = "ostrym";
                                name6_fir = "ostrym";
                                break;
                            case 3:
                                name_fir = "b³yszcz±cy";
                                sh_fir = "b³yszcz±cy";
                                name2_fir = "b³yszcz±cego";
                                name3_fir = "b³yszcz±cym";
                                name4_fir = "b³yszcz±cy";
                                name5_fir = "b³yszcz±cym";
                                name6_fir = "b³yszcz±cym";
                                break;
                            case 4:
                                name_fir = "lekki";
                                sh_fir = "lekki";
                                name2_fir = "lekkiego";
                                name3_fir = "lekkiemu";
                                name4_fir = "lekki";
                                name5_fir = "lekkim";
                                name6_fir = "lekkim";
                                obj->weight /= 2;
                                break;
                            case 5:
                                name_fir = "ciê¿ki";
                                sh_fir = "ciê¿ki";
                                name2_fir = "ciêzkiego";
                                name3_fir = "cie¿kiemu";
                                name4_fir = "ciê¿ki";
                                name5_fir = "ciê¿kim";
                                name6_fir = "ciê¿kim";
                                obj->weight *= 2;
                                break;
                            case 6:
                                name_fir = "poszczerbiony";
                                sh_fir = "poszczerbiony";
                                name2_fir = "poszczerbionego";
                                name3_fir = "poszczerbionemu";
                                name4_fir = "poszczerbiony";
                                name5_fir = "poszczerbionym";
                                name6_fir = "poszczerbionym";
                                --obj->value[2];
                                break;
                            case 7:
                                name_fir = "l¶ni±cy";
                                sh_fir = "l¶ni±cy";
                                name2_fir = "l¶ni±cego";
                                name3_fir = "l¶ni±cemu";
                                name4_fir = "l¶ni±cy";
                                name5_fir = "l¶ni±cym";
                                name6_fir = "l¶ni±cym";
                                break;
                            case 8:
                                name_fir = "szeroki";
                                sh_fir = "szeroki";
                                name2_fir = "szerokiego";
                                name3_fir = "szerokiemu";
                                name4_fir = "szeroki";
                                name5_fir = "szerokim";
                                name6_fir = "szerokim";
                                break;
                            case 9:
                                name_fir = "w±ski";
                                sh_fir = "w±ski";
                                name2_fir = "w±skiego";
                                name3_fir = "w±skiemi";
                                name4_fir = "w±ski";
                                name5_fir = "w±skim";
                                name6_fir = "w±skim";
                                break;
                            case 10:
                                name_fir = "czarny";
                                sh_fir = "czarny";
                                name2_fir = "czarnego";
                                name3_fir = "czarnemu";
                                name4_fir = "czarny";
                                name5_fir = "czarnym";
                                name6_fir = "czarnym";
                                break;
                            case 11:
                                name_fir = "smuk³y";
                                sh_fir = "smuk³y";
                                name2_fir = "smuk³ego";
                                name3_fir = "smuk³emu";
                                name4_fir = "smuk³y";
                                name5_fir = "smuk³ym";
                                name6_fir = "smuk³ym";
                                break;
                            case 12:
                                name_fir = "kunsztowny";
                                sh_fir = "kunsztowny";
                                name2_fir = "kunsztownego";
                                name3_fir = "kunsztownemu";
                                name4_fir = "kunsztowny";
                                name5_fir = "kunsztownym";
                                name6_fir = "kunsztownym";
                                break;
                            case 13:
                                name_fir = "gnomi";
                                sh_fir = "gnomi";
                                name2_fir = "gnomiego";
                                name3_fir = "gnomiemu";
                                name4_fir = "gnomi";
                                name5_fir = "gnomim";
                                name6_fir = "gnomim";
                                break;
                            case 14:
                                name_fir = "elfi";
                                sh_fir = "elfi";
                                name2_fir = "elfiego";
                                name3_fir = "elfiemu";
                                name4_fir = "elfi";
                                name5_fir = "elfim";
                                name6_fir = "elfim";
                                obj->material = 3;
                                break;
                            case 15:
                                name_fir = "krasnoludzki";
                                sh_fir = "krasnoludzki";
                                name2_fir = "krasnoludzkiego";
                                name3_fir = "krasnoludzkiemu";
                                name4_fir = "krasnoludzki";
                                name5_fir = "krasnoludzkim";
                                name6_fir = "krasnoludzkim";
                                break;
                            case 16:
                                name_fir = "grawerowany";
                                sh_fir = "grawerowany";
                                name2_fir = "grawerowanego";
                                name3_fir = "grawerowanemu";
                                name4_fir = "grawerowany";
                                name5_fir = "grawerowanym";
                                name6_fir = "grawerowanym";
                                break;
                            case 17:
                                name_fir = "obosieczny";
                                sh_fir = "obosieczny";
                                name2_fir = "obosiecznego";
                                name3_fir = "obosiecznemu";
                                name4_fir = "obosieczny";
                                name5_fir = "obosiecznym";
                                name6_fir = "obosiecznym";
                                break;
                            case 18:
                                name_fir = "prosty";
                                sh_fir = "prosty";
                                name2_fir = "prostego";
                                name3_fir = "prostemu";
                                name4_fir = "prosty";
                                name5_fir = "prostym";
                                name6_fir = "prostym";
                                break;
                            case 19:
                                name_fir = "powykrzywiany";
                                sh_fir = "powykrzywiany";
                                name2_fir = "powykrzywianego";
                                name3_fir = "powykrzywianemu";
                                name4_fir = "powykrzywiany";
                                name5_fir = "powykrzywianym";
                                name6_fir = "powykrzywianym";
                                --obj->value[2];
                                break;
                            case 20:
                                name_fir = "zdobiony";
                                sh_fir = "zdobiony";
                                name2_fir = "zdobionego";
                                name3_fir = "zdobionemu";
                                name4_fir = "zdobiony";
                                name5_fir = "zdobionym";
                                name6_fir = "zdobionym";
                                break;
                            case 21:
                                name_fir = "wyszczerbiony";
                                sh_fir = "wyszczerbiony";
                                name2_fir = "wyszczerbionego";
                                name3_fir = "wyszczerbionemu";
                                name4_fir = "wyszczerbiony";
                                name5_fir = "wyszczerbionym";
                                name6_fir = "wyszczerbionym";
                                break;
                            case 22:
                                name_fir = "zêbaty";
                                sh_fir = "zêbaty";
                                name2_fir = "zêbatego";
                                name3_fir = "zêbatemu";
                                name4_fir = "zêbaty";
                                name5_fir = "zêbatym";
                                name6_fir = "zêbatym";
                                break;
                            case 23:
                                name_fir = "porêczny";
                                sh_fir = "porêczny";
                                name2_fir = "porêcznego";
                                name3_fir = "porêcznemu";
                                name4_fir = "porêczny";
                                name5_fir = "porêcznym";
                                name6_fir = "porêcznym";
                                break;
                            case 24:
                                name_fir = "po³yskuj±cy";
                                sh_fir = "po³yskuj±cy";
                                name2_fir = "po³yskuj±cego";
                                name3_fir = "po³yskuj±cemu";
                                name4_fir = "po³yskuj±cy";
                                name5_fir = "po³yskuj±cym";
                                name6_fir = "po³yskuj±cym";
                                break;
                            case 25:
                                name_fir = "opalizuj±cy";
                                sh_fir = "opalizuj±cy";
                                name2_fir = "opalizuj±cego";
                                name3_fir = "opalizuj±cemu";
                                name4_fir = "opalizuj±cy";
                                name5_fir = "opalizuj±cym";
                                name6_fir = "opalizuj±cym";
                                break;
                            case 26:
                                name_fir = "wywa¿ony";
                                sh_fir = "wywa¿ony";
                                name2_fir = "wywa¿onego";
                                name3_fir = "wywa¿onemu";
                                name4_fir = "wywa¿ony";
                                name5_fir = "wywa¿onym";
                                name6_fir = "wywa¿onym";
                                break;
                            case 27:
                                name_fir = "szary";
                                sh_fir = "szary";
                                name2_fir = "szarego";
                                name3_fir = "szaremu";
                                name4_fir = "szary";
                                name5_fir = "szarym";
                                name6_fir = "szarym";
                                break;
                            case 28:
                                name_fir = "stary";
                                sh_fir = "stary";
                                name2_fir = "starego";
                                name3_fir = "staremu";
                                name4_fir = "stary";
                                name5_fir = "starym";
                                name6_fir = "starym";
                                --obj->value[ 2 ];
                                break;
                            case 29:
                                name_fir = "podniszczony";
                                sh_fir = "podniszczony";
                                name2_fir = "podniszczonego";
                                name3_fir = "podniszczonemu";
                                name4_fir = "podniszczony";
                                name5_fir = "podniszczonym";
                                name6_fir = "podniszczonym";
                                --obj->value[ 2 ];
                                break;
                            case 30:
                                name_fir = "zadbany";
                                sh_fir = "zadbany";
                                name2_fir = "zadbanego";
                                name3_fir = "zadbanemu";
                                name4_fir = "zadbany";
                                name5_fir = "zadbanym";
                                name6_fir = "zadbanym";
                                break;
                            case 31:
                                name_fir = "zaostrzony";
                                sh_fir = "zaostrzony";
                                name2_fir = "zaostrzonego";
                                name3_fir = "zaostrzonemu";
                                name4_fir = "zaostrzony";
                                name5_fir = "zaostrzonym";
                                name6_fir = "zaostrzonym";
                                break;
                            case 32:
                                name_fir = "z±bkowany";
                                sh_fir = "z±bkowany";
                                name2_fir = "z±bkowanego";
                                name3_fir = "z±bkowanemu";
                                name4_fir = "z±bkowany";
                                name5_fir = "z±bkowanym";
                                name6_fir = "z±bkowanym";
                                break;
                            case 33:
                                name_fir = "antyczny";
                                sh_fir = "antyczny";
                                name2_fir = "antycznego";
                                name3_fir = "antycznemu";
                                name4_fir = "antyczny";
                                name5_fir = "antycznym";
                                name6_fir = "antycznym";
                                break;
                            case 34:
                                name_fir = "pod³u¿ny";
                                sh_fir = "pod³u¿ny";
                                name2_fir = "pod³u¿nego";
                                name3_fir = "pod³u¿nemu";
                                name4_fir = "pod³u¿ny";
                                name5_fir = "pod³u¿nym";
                                name6_fir = "pod³u¿nym";
                                break;
                            case 35:
                                name_fir = "stalowy";
                                sh_fir = "stalowy";
                                name2_fir = "stalowego";
                                name3_fir = "stalowemu";
                                name4_fir = "stalowy";
                                name5_fir = "stalowym";
                                name6_fir = "stalowym";
                                break;
                            case 36:
                                name_fir = "miedziany";
                                sh_fir = "miedziany";
                                name2_fir = "miedzianego";
                                name3_fir = "miedzianemu";
                                name4_fir = "miedziany";
                                name5_fir = "miedzianym";
                                name6_fir = "miedzianym";
                                obj->material = 5;
                                break;
                            case 37:
                                name_fir = "adamantytowy";
                                sh_fir = "adamantytowy";
                                name2_fir = "adamantytowego";
                                name3_fir = "adamantytowemu";
                                name4_fir = "adamantytowy";
                                name5_fir = "adamantytowym";
                                name6_fir = "adamantytowym";
                                obj->material = 7;
                                break;
                            case 38:
                                name_fir = "poz³acany";
                                sh_fir = "poz³acany";
                                name2_fir = "poz³acanego";
                                name3_fir = "poz³acanemu";
                                name4_fir = "poz³acany";
                                name5_fir = "poz³acanym";
                                name6_fir = "poz³acanym";
                                break;
                            case 39:
                                name_fir = "posrebrzany";
                                sh_fir = "posrebrzany";
                                name2_fir = "posrebrzanego";
                                name3_fir = "posrebrzanemu";
                                name4_fir = "posrebrzany";
                                name5_fir = "posrebrzanym";
                                name6_fir = "posrebrzanym";
                                break;
                            case 40:
                                name_fir = "mithrilowy";
                                sh_fir = "mithrilowy";
                                name2_fir = "mithrilowego";
                                name3_fir = "mithrilowemu";
                                name4_fir = "mithrilowy";
                                name5_fir = "mithrilowym";
                                name6_fir = "mithrilowym";
                                obj->material = 8;
                                break;
                            case 41:
                                name_fir = "z³oty";
                                sh_fir = "z³oty";
                                name2_fir = "z³otego";
                                name3_fir = "z³otemu";
                                name4_fir = "z³oty";
                                name5_fir = "z³otym";
                                name6_fir = "z³otym";
                                obj->material = 9;
                                break;
                            case 42:
                                name_fir = "srebrny";
                                sh_fir = "srebrny";
                                name2_fir = "srebrnego";
                                name3_fir = "srebrnemu";
                                name4_fir = "srebrny";
                                name5_fir = "srebrnym";
                                name6_fir = "srebrnym";
                                obj->material = 10;
                                break;
                            case 43:
                                name_fir = "¿elazny";
                                sh_fir = "¿elazny";
                                name2_fir = "¿elaznego";
                                name3_fir = "¿elaznemu";
                                name4_fir = "¿elazny";
                                name5_fir = "¿elaznym";
                                name6_fir = "¿elaznym";
                                obj->material = 13;
                                break;
                            case 44:
                                name_fir = "br±zowy";
                                sh_fir = "br±zowy";
                                name2_fir = "br±zowego";
                                name3_fir = "br±zowemu";
                                name4_fir = "br±zowy";
                                name5_fir = "br±zowym";
                                name6_fir = "br±zowym";
                                obj->material = 14;
                                break;
                            case 45:
                                name_fir = "damasceñski";
                                sh_fir = "damasceñski";
                                name2_fir = "damasceñskiego";
                                name3_fir = "damasceñskiemu";
                                name4_fir = "damasceñski";
                                name5_fir = "damasceñskim";
                                name6_fir = "damasceñskim";
                                obj->material = 32;
                                break;
                            case 46:
                                name_fir = "illitowy";
                                sh_fir = "illitowy";
                                name2_fir = "illitowego";
                                name3_fir = "illitowemu";
                                name4_fir = "illitowy";
                                name5_fir = "illitowym";
                                name6_fir = "illitowym";
                                obj->material = 33;
                                break;
                            case 47:
                                name_fir = "platynowy";
                                sh_fir = "platynowy";
                                name2_fir = "platynowego";
                                name3_fir = "platynowemu";
                                name4_fir = "platynowy";
                                name5_fir = "platynowym";
                                name6_fir = "platynowym";
                                obj->material = 34;
                                break;
                            case 48:
                                name_fir = "diamentowy";
                                sh_fir = "diamentowy";
                                name2_fir = "diamentowego";
                                name3_fir = "diamentowemu";
                                name4_fir = "diamentowy";
                                name5_fir = "diamentowym";
                                name6_fir = "diamentowym";
                                obj->material = 36;
                                break;
                            case 49:
                                name_fir = "chitynowy";
                                sh_fir = "chitynowy";
                                name2_fir = "chitynowego";
                                name3_fir = "chitynowemu";
                                name4_fir = "chitynowy";
                                name5_fir = "chitynowym";
                                name6_fir = "chitynowym";
                                obj->material = 40;
                                break;
                            case 50:
                                name_fir = "smoczy";
                                sh_fir = "smoczy";
                                name2_fir = "smoczego";
                                name3_fir = "smoczemu";
                                name4_fir = "smoczy";
                                name5_fir = "smoczym";
                                name6_fir = "smoczym";
                                obj->material = 42;
                                break;
                            case 51:
                                name_fir = "mosiê¿ny";
                                sh_fir = "mosiê¿ny";
                                name2_fir = "mosiê¿nego";
                                name3_fir = "mosiê¿nemu";
                                name4_fir = "mosiê¿ny";
                                name5_fir = "mosiê¿nym";
                                name6_fir = "mosiê¿nym";
                                obj->material = 43;
                                break;
                            case 52:
                                name_fir = "eteryczny";
                                sh_fir = "eteryczny";
                                name2_fir = "eterycznego";
                                name3_fir = "eterycznemu";
                                name4_fir = "eteryczny";
                                name5_fir = "eterycznym";
                                name6_fir = "eterycznym";
                                obj->material = 45;
                                obj->weight = 1;
                                break;
                            case 53:
                                name_fir = "têpy";
                                sh_fir = "têpy";
                                name2_fir = "têpego";
                                name3_fir = "têpemu";
                                name4_fir = "têpy";
                                name5_fir = "têpym";
                                name6_fir = "têpym";
                                --obj->value[ 2 ];
                                break;
                        }
                        switch( number_range(1,15))
                        {
                            case 1:
                                name_des = " miecz";
                                sh_des = " miecz";
                                name2_des = " miecza";
                                name3_des = " mieczowi";
                                name4_des = " miecz";
                                name5_des = " mieczem";
                                name6_des = " mieczu";
                                break;
                            case 2:
                                name_des = " pa³asz";
                                sh_des = " pa³asz";
                                name2_des = " pa³asza";
                                name3_des = " pa³aszowi";
                                name4_des = " pa³asz";
                                name5_des = " pa³aszem";
                                name6_des = " pa³aszu";
                                break;
                            case 3:
                                name_des = " kord";
                                sh_des = " kord";
                                name2_des = " kordu";
                                name3_des = " kordowi";
                                name4_des = " kord";
                                name5_des = " kordem";
                                name6_des = " kordzie";
                                break;
                            case 4:
                                name_des = " sejmitar";
                                sh_des = " sejmitar";
                                name2_des = " sejmitara";
                                name3_des = " sejmitarowi";
                                name4_des = " sejmitar";
                                name5_des = " sejmitarem";
                                name6_des = " sejmitarze";
                                break;
                            case 5:
                                name_des = " bastard bêkart";
                                sh_des = " bastard";
                                name2_des = " bastarda";
                                name3_des = " bastardowi";
                                name4_des = " bastard";
                                name5_des = " bastardem";
                                name6_des = " bastardzie";
                                break;
                            case 6:
                                name_des = " lancknecht";
                                sh_des = " lancknecht";
                                name2_des = " lancknechta";
                                name3_des = " lancknechtowi";
                                name4_des = " lancknecht";
                                name5_des = " lancknechtem";
                                name6_des = " lancknechcie";
                                break;
                            case 7:
                                name_des = " rapier";
                                sh_des = " rapier";
                                name2_des = " rapiera";
                                name3_des = " rapierowi";
                                name4_des = " rapier";
                                name5_des = " rapierem";
                                name6_des = " rapierze";
                                break;
                            case 8:
                                name_des = " falchion";
                                sh_des = " falchion";
                                name2_des = " falchiona";
                                name3_des = " falchionowi";
                                name4_des = " falchion";
                                name5_des = " falchionem";
                                name6_des = " falchionie";
                                break;
                            case 9:
                                name_des = " scimitar";
                                sh_des = " scimitar";
                                name2_des = " scimitara";
                                name3_des = " scimitarowi";
                                name4_des = " scimitar";
                                name5_des = " scimitarem";
                                name6_des = " scimitarze";
                                break;
                            case 10:
                                name_des = " brzeszczot";
                                sh_des = " brzeszczot";
                                name2_des = " brzeszczotu";
                                name3_des = " brzeszczotem";
                                name4_des = " brzeszczot";
                                name5_des = " brzeszczotem";
                                name6_des = " brzeszczocie";
                                break;
                            case 11:
                                name_des = " bu³at";
                                sh_des = " bu³at";
                                name2_des = " bu³atu";
                                name3_des = " bu³atem";
                                name4_des = " bu³at";
                                name5_des = " bu³atem";
                                name6_des = " bu³acie";
                                break;
                            case 12:
                                name_des = " tachi";
                                sh_des = " tachi";
                                name2_des = " tachi";
                                name3_des = " tachi";
                                name4_des = " tachi";
                                name5_des = " tachi";
                                name6_des = " tachi";
                                break;
                            case 13:
                                name_des = " tasak";
                                sh_des = " tasak";
                                name2_des = " tasaka";
                                name3_des = " tasakiem";
                                name4_des = " tasak";
                                name5_des = " tasakiem";
                                name6_des = " tasaku";
                                break;
                            case 14:
                                name_des = " miecz pó³torarêczny";
                                sh_des = " miecz pó³torarêczny";
                                name2_des = " miecza pó³torarêcznego";
                                name3_des = " mieczowi pó³torarêcznemu";
                                name4_des = " miecz pó³torareczny";
                                name5_des = " mieczem pó³torarêcznym";
                                name6_des = " mieczu pó³torarêcznym";
                                break;
                            case 15:
                                name_des = " szamszir";
                                sh_des = " szamszir";
                                name2_des = " szamszira";
                                name3_des = " szamszirem";
                                name4_des = " szamszir";
                                name5_des = " szamszirem";
                                name6_des = " szamszirze";
                                break;
                        }
                    }
                    else
                    {
                        switch( number_range(1,53))
                        {
                            case 1:
                                name_fir = "d³uga";
                                sh_fir = "d³uga";
                                name2_fir = "d³ugiej";
                                name3_fir = "d³ugiej";
                                name4_fir = "d³ug±";
                                name5_fir = "d³ug±";
                                name6_fir = "d³ugiej";
                                break;
                            case 2:
                                name_fir = "ostra";
                                sh_fir = "ostra";
                                name2_fir = "ostrej";
                                name3_fir = "ostrej";
                                name4_fir = "ostr±";
                                name5_fir = "ostr±";
                                name6_fir = "ostrej";
                                break;
                            case 3:
                                name_fir = "b³yszcz±ca";
                                sh_fir = "b³yszcz±ca";
                                name2_fir = "b³yszcz±cej";
                                name3_fir = "b³yszcz±cej";
                                name4_fir = "b³yszcz±c±";
                                name5_fir = "b³yszcz±c±";
                                name6_fir = "b³yszcz±cej";
                                break;
                            case 4:
                                name_fir = "lekka";
                                sh_fir = "lekka";
                                name2_fir = "lekkiej";
                                name3_fir = "lekkiej";
                                name4_fir = "lekk±";
                                name5_fir = "lekk±";
                                name6_fir = "lekkiej";
                                obj->weight /= 2;
                                break;
                            case 5:
                                name_fir = "ciê¿ka";
                                sh_fir = "ciê¿ka";
                                name2_fir = "ciêzkiej";
                                name3_fir = "cie¿kiej";
                                name4_fir = "ciê¿k±";
                                name5_fir = "ciê¿k±";
                                name6_fir = "ciê¿kiej";
                                obj->weight *= 2;
                                break;
                            case 6:
                                name_fir = "poszczerbiona";
                                sh_fir = "poszczerbiona";
                                name2_fir = "poszczerbionej";
                                name3_fir = "poszczerbionej";
                                name4_fir = "poszczerbion±";
                                name5_fir = "poszczerbion±";
                                name6_fir = "poszczerbionej";
                                --obj->value[2];
                                break;
                            case 7:
                                name_fir = "l¶ni±ca";
                                sh_fir = "l¶ni±ca";
                                name2_fir = "l¶ni±cej";
                                name3_fir = "l¶ni±cej";
                                name4_fir = "l¶ni±c±";
                                name5_fir = "l¶ni±c±";
                                name6_fir = "l¶ni±cej";
                                break;
                            case 8:
                                name_fir = "szeroka";
                                sh_fir = "szeroka";
                                name2_fir = "szerokiej";
                                name3_fir = "szerokiej";
                                name4_fir = "szerok±";
                                name5_fir = "szerok±";
                                name6_fir = "szerokiej";
                                break;
                            case 9:
                                name_fir = "w±ska";
                                sh_fir = "w±ska";
                                name2_fir = "w±skiej";
                                name3_fir = "w±skiej";
                                name4_fir = "w±sk±";
                                name5_fir = "w±sk±";
                                name6_fir = "w±skiej";
                                break;
                            case 10:
                                name_fir = "czarna";
                                sh_fir = "czarna";
                                name2_fir = "czarnej";
                                name3_fir = "czarnej";
                                name4_fir = "czarn±";
                                name5_fir = "czarn±";
                                name6_fir = "czarnej";
                                break;
                            case 11:
                                name_fir = "smuk³a";
                                sh_fir = "smuk³a";
                                name2_fir = "smuk³ej";
                                name3_fir = "smuk³ej";
                                name4_fir = "smuk³±";
                                name5_fir = "smuk³±";
                                name6_fir = "smuk³ej";
                                break;
                            case 12:
                                name_fir = "kunsztowna";
                                sh_fir = "kunsztowna";
                                name2_fir = "kunsztownej";
                                name3_fir = "kunsztownej";
                                name4_fir = "kunsztown±";
                                name5_fir = "kunsztown±";
                                name6_fir = "kunsztownej";
                                break;
                            case 13:
                                name_fir = "gnomia";
                                sh_fir = "gnomia";
                                name2_fir = "gnomiej";
                                name3_fir = "gnomiej";
                                name4_fir = "gnomi±";
                                name5_fir = "gnomi±";
                                name6_fir = "gnomiej";
                                break;
                            case 14:
                                name_fir = "elfia";
                                sh_fir = "elfia";
                                name2_fir = "elfiej";
                                name3_fir = "elfiej";
                                name4_fir = "elfi±";
                                name5_fir = "elfi±";
                                name6_fir = "elfiej";
                                obj->material = 3;
                                break;
                            case 15:
                                name_fir = "krasnoludzka";
                                sh_fir = "krasnoludzka";
                                name2_fir = "krasnoludzkiej";
                                name3_fir = "krasnoludzkiej";
                                name4_fir = "krasnoludzk±";
                                name5_fir = "krasnoludzk±";
                                name6_fir = "krasnoludzkiej";
                                break;
                            case 16:
                                name_fir = "grawerowana";
                                sh_fir = "grawerowana";
                                name2_fir = "grawerowanej";
                                name3_fir = "grawerowanej";
                                name4_fir = "grawerowan±";
                                name5_fir = "grawerowan±";
                                name6_fir = "grawerowanej";
                                break;
                            case 17:
                                name_fir = "obosieczna";
                                sh_fir = "obosieczna";
                                name2_fir = "obosiecznej";
                                name3_fir = "obosiecznej";
                                name4_fir = "obosieczn±";
                                name5_fir = "obosieczn±";
                                name6_fir = "obosiecznej";
                                break;
                            case 18:
                                name_fir = "prosta";
                                sh_fir = "prosta";
                                name2_fir = "prostej";
                                name3_fir = "prostej";
                                name4_fir = "prost±";
                                name5_fir = "prost±";
                                name6_fir = "prostej";
                                break;
                            case 19:
                                name_fir = "powykrzywiana";
                                sh_fir = "powykrzywiana";
                                name2_fir = "powykrzywianej";
                                name3_fir = "powykrzywianej";
                                name4_fir = "powykrzywian±";
                                name5_fir = "powykrzywian±";
                                name6_fir = "powykrzywianej";
                                --obj->value[2];
                                break;
                            case 20:
                                name_fir = "zdobiona";
                                sh_fir = "zdobiona";
                                name2_fir = "zdobionej";
                                name3_fir = "zdobionej";
                                name4_fir = "zdobion±";
                                name5_fir = "zdobion±";
                                name6_fir = "zdobionej";
                                break;
                            case 21:
                                name_fir = "wyszczerbiona";
                                sh_fir = "wyszczerbiona";
                                name2_fir = "wyszczerbionej";
                                name3_fir = "wyszczerbionej";
                                name4_fir = "wyszczerbion±";
                                name5_fir = "wyszczerbion±";
                                name6_fir = "wyszczerbionej";
                                break;
                            case 22:
                                name_fir = "zêbata";
                                sh_fir = "zêbata";
                                name2_fir = "zêbatej";
                                name3_fir = "zêbatej";
                                name4_fir = "zêbat±";
                                name5_fir = "zêbat±";
                                name6_fir = "zêbatej";
                                break;
                            case 23:
                                name_fir = "porêczna";
                                sh_fir = "porêczna";
                                name2_fir = "porêcznej";
                                name3_fir = "porêcznej";
                                name4_fir = "porêczn±";
                                name5_fir = "porêczn±";
                                name6_fir = "porêcznej";
                                break;
                            case 24:
                                name_fir = "po³yskuj±ca";
                                sh_fir = "po³yskuj±ca";
                                name2_fir = "po³yskuj±cej";
                                name3_fir = "po³yskuj±cej";
                                name4_fir = "po³yskuj±c±";
                                name5_fir = "po³yskuj±c±";
                                name6_fir = "po³yskuj±cej";
                                break;
                            case 25:
                                name_fir = "opalizuj±ca";
                                sh_fir = "opalizuj±ca";
                                name2_fir = "opalizuj±cej";
                                name3_fir = "opalizuj±cej";
                                name4_fir = "opalizuj±c±";
                                name5_fir = "opalizuj±c±";
                                name6_fir = "opalizuj±cej";
                                break;
                            case 26:
                                name_fir = "wywa¿ona";
                                sh_fir = "wywa¿ona";
                                name2_fir = "wywa¿onej";
                                name3_fir = "wywa¿onej";
                                name4_fir = "wywa¿on±";
                                name5_fir = "wywa¿on±";
                                name6_fir = "wywa¿onej";
                                break;
                            case 27:
                                name_fir = "szara";
                                sh_fir = "szara";
                                name2_fir = "szarej";
                                name3_fir = "szarej";
                                name4_fir = "szar±";
                                name5_fir = "szar±";
                                name6_fir = "szarej";
                                break;
                            case 28:
                                name_fir = "stara";
                                sh_fir = "stara";
                                name2_fir = "starej";
                                name3_fir = "starej";
                                name4_fir = "star±";
                                name5_fir = "star±";
                                name6_fir = "starej";
                                --obj->value[ 2 ];
                                break;
                            case 29:
                                name_fir = "podniszczona";
                                sh_fir = "podniszczona";
                                name2_fir = "podniszczonej";
                                name3_fir = "podniszczonej";
                                name4_fir = "podniszczon±";
                                name5_fir = "podniszczon±";
                                name6_fir = "podniszczonej";
                                --obj->value[ 2 ];
                                break;
                            case 30:
                                name_fir = "zadbana";
                                sh_fir = "zadbana";
                                name2_fir = "zadbanej";
                                name3_fir = "zadbanej";
                                name4_fir = "zadban±";
                                name5_fir = "zadban±";
                                name6_fir = "zadbanej";
                                break;
                            case 31:
                                name_fir = "zaostrzona";
                                sh_fir = "zaostrzona";
                                name2_fir = "zaostrzonej";
                                name3_fir = "zaostrzonej";
                                name4_fir = "zaostrzon±";
                                name5_fir = "zaostrzon±";
                                name6_fir = "zaostrzonej";
                                break;
                            case 32:
                                name_fir = "z±bkowana";
                                sh_fir = "z±bkowana";
                                name2_fir = "z±bkowanej";
                                name3_fir = "z±bkowanej";
                                name4_fir = "z±bkowan±";
                                name5_fir = "z±bkowan±";
                                name6_fir = "z±bkowanej";
                                break;
                            case 33:
                                name_fir = "antyczna";
                                sh_fir = "antyczna";
                                name2_fir = "antycznej";
                                name3_fir = "antycznej";
                                name4_fir = "antyczn±";
                                name5_fir = "antyczn±";
                                name6_fir = "antycznej";
                                break;
                            case 34:
                                name_fir = "pod³u¿na";
                                sh_fir = "pod³u¿na";
                                name2_fir = "pod³u¿nej";
                                name3_fir = "pod³u¿nej";
                                name4_fir = "pod³u¿n±";
                                name5_fir = "pod³u¿n±";
                                name6_fir = "pod³u¿nej";
                                break;
                            case 35:
                                name_fir = "stalowa";
                                sh_fir = "stalowa";
                                name2_fir = "stalowej";
                                name3_fir = "stalowej";
                                name4_fir = "stalow±";
                                name5_fir = "stalow±";
                                name6_fir = "stalowej";
                                break;
                            case 36:
                                name_fir = "miedziana";
                                sh_fir = "miedziana";
                                name2_fir = "miedzianej";
                                name3_fir = "miedzianej";
                                name4_fir = "miedzian±";
                                name5_fir = "miedzian±";
                                name6_fir = "miedzianej";
                                obj->material = 5;
                                break;
                            case 37:
                                name_fir = "adamantytowa";
                                sh_fir = "adamantytowa";
                                name2_fir = "adamantytowej";
                                name3_fir = "adamantytowej";
                                name4_fir = "adamantytow±";
                                name5_fir = "adamantytow±";
                                name6_fir = "adamantytowej";
                                obj->material = 7;
                                break;
                            case 38:
                                name_fir = "poz³acana";
                                sh_fir = "poz³acana";
                                name2_fir = "poz³acanej";
                                name3_fir = "poz³acanej";
                                name4_fir = "poz³acan±";
                                name5_fir = "poz³acan±";
                                name6_fir = "poz³acanej";
                                break;
                            case 39:
                                name_fir = "posrebrzana";
                                sh_fir = "posrebrzana";
                                name2_fir = "posrebrzanej";
                                name3_fir = "posrebrzanej";
                                name4_fir = "posrebrzan±";
                                name5_fir = "posrebrzan±";
                                name6_fir = "posrebrzanej";
                                break;
                            case 40:
                                name_fir = "mithrilowa";
                                sh_fir = "mithrilowa";
                                name2_fir = "mithrilowej";
                                name3_fir = "mithrilowej";
                                name4_fir = "mithrilow±";
                                name5_fir = "mithrilow±";
                                name6_fir = "mithrilowej";
                                obj->material = 8;
                                break;
                            case 41:
                                name_fir = "z³ota";
                                sh_fir = "z³ota";
                                name2_fir = "z³otej";
                                name3_fir = "z³otej";
                                name4_fir = "z³ot±";
                                name5_fir = "z³ot±";
                                name6_fir = "z³otej";
                                obj->material = 9;
                                break;
                            case 42:
                                name_fir = "srebrna";
                                sh_fir = "srebrna";
                                name2_fir = "srebrnej";
                                name3_fir = "srebrnej";
                                name4_fir = "srebrn±";
                                name5_fir = "srebrn±";
                                name6_fir = "srebrnej";
                                obj->material = 10;
                                break;
                            case 43:
                                name_fir = "¿elazna";
                                sh_fir = "¿elazna";
                                name2_fir = "¿elaznej";
                                name3_fir = "¿elaznej";
                                name4_fir = "¿elazn±";
                                name5_fir = "¿elazn±";
                                name6_fir = "¿elaznej";
                                obj->material = 13;
                                break;
                            case 44:
                                name_fir = "br±zowa";
                                sh_fir = "br±zowa";
                                name2_fir = "br±zowej";
                                name3_fir = "br±zowej";
                                name4_fir = "br±zow±";
                                name5_fir = "br±zow±";
                                name6_fir = "br±zowej";
                                obj->material = 14;
                                break;
                            case 45:
                                name_fir = "damasceñska";
                                sh_fir = "damasceñska";
                                name2_fir = "damasceñskiej";
                                name3_fir = "damasceñskiej";
                                name4_fir = "damasceñsk±";
                                name5_fir = "damasceñsk±";
                                name6_fir = "damasceñskiej";
                                obj->material = 32;
                                break;
                            case 46:
                                name_fir = "illitowa";
                                sh_fir = "illitowa";
                                name2_fir = "illitowej";
                                name3_fir = "illitowej";
                                name4_fir = "illitow±";
                                name5_fir = "illitow±";
                                name6_fir = "illitowej";
                                obj->material = 33;
                                break;
                            case 47:
                                name_fir = "platynowa";
                                sh_fir = "platynowa";
                                name2_fir = "platynowej";
                                name3_fir = "platynowej";
                                name4_fir = "platynow±";
                                name5_fir = "platynow±";
                                name6_fir = "platynowej";
                                obj->material = 34;
                                break;
                            case 48:
                                name_fir = "diamentowa";
                                sh_fir = "diamentowa";
                                name2_fir = "diamentowej";
                                name3_fir = "diamentowej";
                                name4_fir = "diamentow±";
                                name5_fir = "diamentow±";
                                name6_fir = "diamentowej";
                                obj->material = 36;
                                break;
                            case 49:
                                name_fir = "chitynowa";
                                sh_fir = "chitynowa";
                                name2_fir = "chitynowej";
                                name3_fir = "chitynowej";
                                name4_fir = "chitynow±";
                                name5_fir = "chitynow±";
                                name6_fir = "chitynowej";
                                obj->material = 40;
                                break;
                            case 50:
                                name_fir = "smocza";
                                sh_fir = "smocza";
                                name2_fir = "smoczej";
                                name3_fir = "smoczej";
                                name4_fir = "smocz±";
                                name5_fir = "smocz±";
                                name6_fir = "smoczej";
                                obj->material = 42;
                                break;
                            case 51:
                                name_fir = "mosiê¿na";
                                sh_fir = "mosiê¿na";
                                name2_fir = "mosiê¿nej";
                                name3_fir = "mosiê¿nej";
                                name4_fir = "mosiê¿n±";
                                name5_fir = "mosiê¿n±";
                                name6_fir = "mosiê¿nej";
                                obj->material = 43;
                                break;
                            case 52:
                                name_fir = "eteryczna";
                                sh_fir = "eteryczna";
                                name2_fir = "eterycznej";
                                name3_fir = "eterycznej";
                                name4_fir = "eteryczn±";
                                name5_fir = "eteryczn±";
                                name6_fir = "eterycznej";
                                obj->material = 45;
                                obj->weight = 1;
                                break;
                            case 53:
                                name_fir = "têpa";
                                sh_fir = "têpa";
                                name2_fir = "têpej";
                                name3_fir = "têpej";
                                name4_fir = "têp±";
                                name5_fir = "têp±";
                                name6_fir = "têpej";
                                --obj->value[ 2 ];
                                break;
                        }
                        switch( number_range(1,7))
                        {
                            case 1:
                                name_des = " szabla";
                                sh_des = " szabla";
                                name2_des = " szabli";
                                name3_des = " szabli";
                                name4_des = " szablê";
                                name5_des = " szabl±";
                                name6_des = " szabli";
                                break;
                            case 2:
                                name_des = " karabela";
                                sh_des = " karabela";
                                name2_des = " karabeli";
                                name3_des = " karabeli";
                                name4_des = " karabelê";
                                name5_des = " karabel±";
                                name6_des = " karabeli";
                                break;
                            case 3:
                                name_des = " katana";
                                sh_des = " katana";
                                name2_des = " katany";
                                name3_des = " katanie";
                                name4_des = " katanê";
                                name5_des = " katan±";
                                name6_des = " katanie";
                                break;
                            case 4:
                                name_des = " saberra";
                                sh_des = " saberra";
                                name2_des = " saberry";
                                name3_des = " saberze";
                                name4_des = " saberrê";
                                name5_des = " saberr±";
                                name6_des = " saberze";
                                break;
                            case 5:
                                name_des = " szpada";
                                sh_des = " szpada";
                                name2_des = " szpady";
                                name3_des = " szpadzie";
                                name4_des = " szpadê";
                                name5_des = " szpad±";
                                name6_des = " szpadzie";
                                break;
                            case 6:
                                name_des = " spatha";
                                sh_des = " spatha";
                                name2_des = " spathy";
                                name3_des = " spathie";
                                name4_des = " spathê";
                                name5_des = " spath±";
                                name6_des = " spathie";
                                break;
                            case 7:
                                name_des = " maczeta";
                                sh_des = " maczeta";
                                name2_des = " maczety";
                                name3_des = " maczecie";
                                name4_des = " maczetê";
                                name5_des = " maczet±";
                                name6_des = " maczecie";
                                break;
                        }
                    }
                    break;
                case 2://shorty
                    obj->weight = number_range( 50, 150 );
                    obj->value[ 0 ] = WEAPON_SHORTSWORD;

                    /* kostki obra¿eñ */
                    {
                        int kostki[8][2] = { { 1,4 }, { 2,3 }, { 1,5 },
                            { 2,4 }, { 1,7 }, { 2,5 },
                            { 3,3 }, { 3,4 }};
                        int tmp = number_range( 0, (wer*2)-1 );

                        obj->value[ 1 ] = kostki[tmp][0];
                        obj->value[ 2 ] = kostki[tmp][1];
                    }

                    if( wer > 3 && obj->value[ 1 ] * obj->value[ 2 ] < 11 )
                    {
                        obj->value[ 5 ] = number_range( 0, 2 );
                        obj->value[ 6 ] = number_range( 0, 1 );
                    }

                    switch( number_range(1,2))
                    {
                        case 1:
                            switch( number_range(1,43))
                            {
                                case 1:
                                    name_fir = "b³yszcz±ce";
                                    sh_fir = "b³yszcz±ce";
                                    name2_fir = "b³yszcz±cego";
                                    name3_fir = "b³yszcz±cemu";
                                    name4_fir = "b³yszcz±ce";
                                    name5_fir = "b³yszcz±cym";
                                    name6_fir = "b³yszcz±cym";
                                    break;
                                case 2:
                                    name_fir = "poszczerbione";
                                    sh_fir = "poszczerbione";
                                    name2_fir = "poszczerbionego";
                                    name3_fir = "poszczerbionemu";
                                    name4_fir = "poszczerbione";
                                    name5_fir = "poszczerbionym";
                                    name6_fir = "poszczerbionym";
                                    --obj->value[2];
                                    break;
                                case 3:
                                    name_fir = "l¶ni±ce";
                                    sh_fir = "l¶ni±ce";
                                    name2_fir = "l¶ni±cego";
                                    name3_fir = "l¶ni±cym";
                                    name4_fir = "l¶ni±ce";
                                    name5_fir = "l¶ni±cym";
                                    name6_fir = "l¶ni±cym";
                                    break;
                                case 4:
                                    name_fir = "ko¶ciane";
                                    sh_fir = "ko¶ciane";
                                    name2_fir = "ko¶cianego";
                                    name3_fir = "ko¶cianemu";
                                    name4_fir = "ko¶ciane";
                                    name5_fir = "ko¶cianym";
                                    name6_fir = "ko¶cianym";
                                    obj->material = 21;
                                    break;
                                case 5:
                                    name_fir = "czarne";
                                    sh_fir = "czarne";
                                    name2_fir = "czarnego";
                                    name3_fir = "czarnemu";
                                    name4_fir = "czarne";
                                    name5_fir = "czarnym";
                                    name6_fir = "czarnym";
                                    break;
                                case 6:
                                    name_fir = "smuk³e";
                                    sh_fir = "smuk³e";
                                    name2_fir = "smuk³ego";
                                    name3_fir = "smuk³emu";
                                    name4_fir = "smuk³e";
                                    name5_fir = "smuk³ym";
                                    name6_fir = "smuk³ym";
                                    break;
                                case 7:
                                    name_fir = "kunsztowne";
                                    sh_fir = "kunsztowne";
                                    name2_fir = "kunsztownego";
                                    name3_fir = "kunsztownemu";
                                    name4_fir = "kunsztowne";
                                    name5_fir = "kunsztownym";
                                    name6_fir = "kunsztownym";
                                    break;
                                case 8:
                                    name_fir = "gnomie";
                                    sh_fir = "gnomie";
                                    name2_fir = "gnomiego";
                                    name3_fir = "gnomiemu";
                                    name4_fir = "gnomie";
                                    name5_fir = "gnomim";
                                    name6_fir = "gnomim";
                                    break;
                                case 9:
                                    name_fir = "elfie";
                                    sh_fir = "elfie";
                                    name2_fir = "elfiego";
                                    name3_fir = "elfiemu";
                                    name4_fir = "elfie";
                                    name5_fir = "elfim";
                                    name6_fir = "elfim";
                                    obj->material = 3;
                                    break;
                                case 10:
                                    name_fir = "krasnoludzkie";
                                    sh_fir = "krasnoludzkie";
                                    name2_fir = "krasnoludzkiego";
                                    name3_fir = "krasnoludzkiemu";
                                    name4_fir = "krasnoludzkie";
                                    name5_fir = "krasnoludzkim";
                                    name6_fir = "krasnoludzkim";
                                    break;
                                case 11:
                                    name_fir = "grawerowane";
                                    sh_fir = "grawerowane";
                                    name2_fir = "grawerowanego";
                                    name3_fir = "grawerowanemu";
                                    name4_fir = "grawerowane";
                                    name5_fir = "grawerowanym";
                                    name6_fir = "grawerowanym";
                                    break;
                                case 12:
                                    name_fir = "powykrzywiane";
                                    sh_fir = "powykrzywiane";
                                    name2_fir = "powykrzywianego";
                                    name3_fir = "powykrzywianemu";
                                    name4_fir = "powykrzywiane";
                                    name5_fir = "powykrzywianym";
                                    name6_fir = "powykrzywianym";
                                    --obj->value[2];
                                    break;
                                case 13:
                                    name_fir = "zdobione";
                                    sh_fir = "zdobione";
                                    name2_fir = "zdobionego";
                                    name3_fir = "zdobionemu";
                                    name4_fir = "zdobione";
                                    name5_fir = "zdobionym";
                                    name6_fir = "zdobionym";
                                    break;
                                case 14:
                                    name_fir = "wyszczerbione";
                                    sh_fir = "wyszczerbione";
                                    name2_fir = "wyszczerbionego";
                                    name3_fir = "wyszczerbionemu";
                                    name4_fir = "wyszczerbione";
                                    name5_fir = "wyszczerbionym";
                                    name6_fir = "wyszczerbionym";
                                    break;
                                case 15:
                                    name_fir = "zêbate";
                                    sh_fir = "zêbate";
                                    name2_fir = "zêbatego";
                                    name3_fir = "zêbatemu";
                                    name4_fir = "zêbate";
                                    name5_fir = "zêbatym";
                                    name6_fir = "zêbatym";
                                    break;
                                case 16:
                                    name_fir = "porêczne";
                                    sh_fir = "porêczne";
                                    name2_fir = "porêcznego";
                                    name3_fir = "porêcznemu";
                                    name4_fir = "porêczne";
                                    name5_fir = "porêcznym";
                                    name6_fir = "porêcznym";
                                    break;
                                case 17:
                                    name_fir = "po³yskuj±ce";
                                    sh_fir = "po³yskuj±ce";
                                    name2_fir = "po³yskuj±cego";
                                    name3_fir = "po³yskuj±cemu";
                                    name4_fir = "po³yskuj±ce";
                                    name5_fir = "po³yskuj±cym";
                                    name6_fir = "po³yskuj±cym";
                                    break;
                                case 18:
                                    name_fir = "opalizuj±ce";
                                    sh_fir = "opalizuj±ce";
                                    name2_fir = "opalizuj±cego";
                                    name3_fir = "opalizuj±cemu";
                                    name4_fir = "opalizuj±ce";
                                    name5_fir = "opalizuj±cym";
                                    name6_fir = "opalizuj±cym";
                                    break;
                                case 19:
                                    name_fir = "wywa¿one";
                                    sh_fir = "wywa¿one";
                                    name2_fir = "wywa¿onego";
                                    name3_fir = "wywa¿onemu";
                                    name4_fir = "wywa¿one";
                                    name5_fir = "wywa¿onym";
                                    name6_fir = "wywa¿onym";
                                    break;
                                case 20:
                                    name_fir = "szare";
                                    sh_fir = "szare";
                                    name2_fir = "szarego";
                                    name3_fir = "szaremu";
                                    name4_fir = "szare";
                                    name5_fir = "szarym";
                                    name6_fir = "szarym";
                                    break;
                                case 21:
                                    name_fir = "podniszczone";
                                    sh_fir = "podniszczone";
                                    name2_fir = "podniszczonego";
                                    name3_fir = "podniszczonemu";
                                    name4_fir = "podniszczone";
                                    name5_fir = "podniszczonym";
                                    name6_fir = "podniszczonym";
                                    --obj->value[ 2 ];
                                    break;
                                case 22:
                                    name_fir = "zadbane";
                                    sh_fir = "zadbane";
                                    name2_fir = "zadbanego";
                                    name3_fir = "zadbanemu";
                                    name4_fir = "zadbane";
                                    name5_fir = "zadbanym";
                                    name6_fir = "zadbanym";
                                    break;
                                case 23:
                                    name_fir = "z±bkowane";
                                    sh_fir = "z±bkowane";
                                    name2_fir = "z±bkowanego";
                                    name3_fir = "z±bkowanemu";
                                    name4_fir = "z±bkowane";
                                    name5_fir = "z±bkowanym";
                                    name6_fir = "z±bkowanym";
                                    break;
                                case 24:
                                    name_fir = "antyczne";
                                    sh_fir = "antyczne";
                                    name2_fir = "antycznego";
                                    name3_fir = "antycznemu";
                                    name4_fir = "antyczne";
                                    name5_fir = "antycznym";
                                    name6_fir = "antycznym";
                                    break;
                                case 25:
                                    name_fir = "pod³u¿ne";
                                    sh_fir = "pod³u¿ne";
                                    name2_fir = "pod³u¿nego";
                                    name3_fir = "pod³u¿nemu";
                                    name4_fir = "pod³u¿ne";
                                    name5_fir = "pod³u¿nym";
                                    name6_fir = "pod³u¿nym";
                                    break;
                                case 26:
                                    name_fir = "stalowe";
                                    sh_fir = "stalowe";
                                    name2_fir = "stalowego";
                                    name3_fir = "stalowemu";
                                    name4_fir = "stalowe";
                                    name5_fir = "stalowym";
                                    name6_fir = "stalowym";
                                    break;
                                case 27:
                                    name_fir = "miedziane";
                                    sh_fir = "miedziane";
                                    name2_fir = "miedzianego";
                                    name3_fir = "miedzianemu";
                                    name4_fir = "miedziane";
                                    name5_fir = "miedzianym";
                                    name6_fir = "miedzianym";
                                    obj->material = 5;
                                    break;
                                case 28:
                                    name_fir = "adamantytowe";
                                    sh_fir = "adamantytowe";
                                    name2_fir = "adamantytowego";
                                    name3_fir = "adamantytowemu";
                                    name4_fir = "adamantytowe";
                                    name5_fir = "adamantytowym";
                                    name6_fir = "adamantytowym";
                                    obj->material = 7;
                                    break;
                                case 29:
                                    name_fir = "poz³acane";
                                    sh_fir = "poz³acane";
                                    name2_fir = "poz³acanego";
                                    name3_fir = "poz³acanemu";
                                    name4_fir = "poz³acane";
                                    name5_fir = "poz³acanym";
                                    name6_fir = "poz³acanym";
                                    break;
                                case 30:
                                    name_fir = "posrebrzane";
                                    sh_fir = "posrebrzane";
                                    name2_fir = "posrebrzanego";
                                    name3_fir = "posrebrzanemu";
                                    name4_fir = "posrebrzane";
                                    name5_fir = "posrebrzanym";
                                    name6_fir = "posrebrzanym";
                                    break;
                                case 31:
                                    name_fir = "mithrilowe";
                                    sh_fir = "mithrilowe";
                                    name2_fir = "mithrilowego";
                                    name3_fir = "mithrilowemu";
                                    name4_fir = "mithrilowe";
                                    name5_fir = "mithrilowym";
                                    name6_fir = "mithrilowym";
                                    obj->material = 8;
                                    break;
                                case 32:
                                    name_fir = "z³ote";
                                    sh_fir = "z³ote";
                                    name2_fir = "z³otego";
                                    name3_fir = "z³otemu";
                                    name4_fir = "z³ote";
                                    name5_fir = "z³otym";
                                    name6_fir = "z³otym";
                                    obj->material = 9;
                                    break;
                                case 33:
                                    name_fir = "srebrne";
                                    sh_fir = "srebrne";
                                    name2_fir = "srebrnego";
                                    name3_fir = "srebrnemu";
                                    name4_fir = "srebrne";
                                    name5_fir = "srebrnym";
                                    name6_fir = "srebrnym";
                                    obj->material = 10;
                                    break;
                                case 34:
                                    name_fir = "¿elazne";
                                    sh_fir = "¿elazne";
                                    name2_fir = "¿elaznego";
                                    name3_fir = "¿elaznemu";
                                    name4_fir = "¿elazne";
                                    name5_fir = "¿elaznym";
                                    name6_fir = "¿elaznym";
                                    obj->material = 13;
                                    break;
                                case 35:
                                    name_fir = "br±zowe";
                                    sh_fir = "br±zowe";
                                    name2_fir = "br±zowego";
                                    name3_fir = "br±zowemu";
                                    name4_fir = "br±zowe";
                                    name5_fir = "br±zowym";
                                    name6_fir = "br±zowym";
                                    obj->material = 14;
                                    break;
                                case 36:
                                    name_fir = "damasceñskie";
                                    sh_fir = "damasceñskie";
                                    name2_fir = "damasceñskiego";
                                    name3_fir = "damasceñskiemu";
                                    name4_fir = "damasceñskie";
                                    name5_fir = "damasceñskim";
                                    name6_fir = "damasceñskim";
                                    obj->material = 32;
                                    break;
                                case 37:
                                    name_fir = "illitowe";
                                    sh_fir = "illitowe";
                                    name2_fir = "illitowego";
                                    name3_fir = "illitowemu";
                                    name4_fir = "illitowe";
                                    name5_fir = "illitowym";
                                    name6_fir = "illitowym";
                                    obj->material = 33;
                                    break;
                                case 38:
                                    name_fir = "platynowe";
                                    sh_fir = "platynowe";
                                    name2_fir = "platynowego";
                                    name3_fir = "platynowemu";
                                    name4_fir = "platynowe";
                                    name5_fir = "platynowym";
                                    name6_fir = "platynowym";
                                    obj->material = 34;
                                    break;
                                case 39:
                                    name_fir = "diamentowe";
                                    sh_fir = "diamentowe";
                                    name2_fir = "diamentowego";
                                    name3_fir = "diamentowemu";
                                    name4_fir = "diamentowe";
                                    name5_fir = "diamentowym";
                                    name6_fir = "diamentowym";
                                    obj->material = 36;
                                    break;
                                case 40:
                                    name_fir = "chitynowe";
                                    sh_fir = "chitynowe";
                                    name2_fir = "chitynowego";
                                    name3_fir = "chitynowemu";
                                    name4_fir = "chitynowe";
                                    name5_fir = "chitynowym";
                                    name6_fir = "chitynowym";
                                    obj->material = 40;
                                    break;
                                case 41:
                                    name_fir = "smocze";
                                    sh_fir = "smocze";
                                    name2_fir = "smoczego";
                                    name3_fir = "smoczemu";
                                    name4_fir = "smocze";
                                    name5_fir = "smoczym";
                                    name6_fir = "smoczym";
                                    obj->material = 42;
                                    break;
                                case 42:
                                    name_fir = "mosiê¿ne";
                                    sh_fir = "mosiê¿ne";
                                    name2_fir = "mosiê¿nego";
                                    name3_fir = "mosiê¿nemu";
                                    name4_fir = "mosiê¿ne";
                                    name5_fir = "mosiê¿nym";
                                    name6_fir = "mosiê¿nym";
                                    obj->material = 43;
                                    break;
                                case 43:
                                    name_fir = "eteryczne";
                                    sh_fir = "eteryczne";
                                    name2_fir = "eterycznego";
                                    name3_fir = "eterycznemu";
                                    name4_fir = "eteryczne";
                                    name5_fir = "eterycznym";
                                    name6_fir = "eterycznym";
                                    obj->material = 45;
                                    obj->weight = 1;
                                    break;
                            }

                            if( number_range(1,4) == 1 )
                            {
                                name_des = " wakizashi";
                                sh_des = " wakizashi";
                                name2_des = " wakizashi";
                                name3_des = " wakizashi";
                                name4_des = " wakizashi";
                                name5_des = " wakizashi";
                                name6_des = " wakizashi";
                            }
                            else
                            {
                                sprintf( txt, "krótkie %s ostrze", name_fir );
                                name_des = str_dup( txt );
                                sprintf( txt, "krótkie %s ostrze", sh_fir );
                                sh_des = str_dup( txt );
                                sprintf( txt, "krótkiego %s ostrza", name2_fir );
                                name2_des = str_dup( txt );
                                sprintf( txt, "krótkiemu %s ostrzu", name3_fir );
                                name3_des = str_dup( txt );
                                sprintf( txt, "krótkie %s ostrze", name4_fir );
                                name4_des = str_dup( txt );
                                sprintf( txt, "krótkiem %s ostrzem", name5_fir );
                                name5_des = str_dup( txt );
                                sprintf( txt, "krótkim %s ostrzu", name6_fir );
                                name6_des = str_dup( txt );
                                name_fir = "";
                                sh_fir = "";
                                name2_fir = "";
                                name3_fir = "";
                                name4_fir = "";
                                name5_fir = "";
                                name6_fir = "";
                            }
                            break;
                        case 2:
                            switch( number_range(1,48))
                            {
                                case 1:
                                    name_fir = "ostry";
                                    sh_fir = "ostry";
                                    name2_fir = "ostrego";
                                    name3_fir = "ostremu";
                                    name4_fir = "ostry";
                                    name5_fir = "ostrym";
                                    name6_fir = "ostrym";
                                    break;
                                case 2:
                                    name_fir = "b³yszcz±cy";
                                    sh_fir = "b³yszcz±cy";
                                    name2_fir = "b³yszcz±cego";
                                    name3_fir = "b³yszcz±cym";
                                    name4_fir = "b³yszcz±cy";
                                    name5_fir = "b³yszcz±cym";
                                    name6_fir = "b³yszcz±cym";
                                    break;
                                case 3:
                                    name_fir = "poszczerbiony";
                                    sh_fir = "poszczerbiony";
                                    name2_fir = "poszczerbionego";
                                    name3_fir = "poszczerbionemu";
                                    name4_fir = "poszczerbiony";
                                    name5_fir = "poszczerbionym";
                                    name6_fir = "poszczerbionym";
                                    --obj->value[2];
                                    break;
                                case 4:
                                    name_fir = "l¶ni±cy";
                                    sh_fir = "l¶ni±cy";
                                    name2_fir = "l¶ni±cego";
                                    name3_fir = "l¶ni±cemu";
                                    name4_fir = "l¶ni±cy";
                                    name5_fir = "l¶ni±cym";
                                    name6_fir = "l¶ni±cym";
                                    break;
                                case 5:
                                    name_fir = "czarny";
                                    sh_fir = "czarny";
                                    name2_fir = "czarnego";
                                    name3_fir = "czarnemu";
                                    name4_fir = "czarny";
                                    name5_fir = "czarnym";
                                    name6_fir = "czarnym";
                                    break;
                                case 6:
                                    name_fir = "smuk³y";
                                    sh_fir = "smuk³y";
                                    name2_fir = "smuk³ego";
                                    name3_fir = "smuk³emu";
                                    name4_fir = "smuk³y";
                                    name5_fir = "smuk³ym";
                                    name6_fir = "smuk³ym";
                                    break;
                                case 7:
                                    name_fir = "kunsztowny";
                                    sh_fir = "kunsztowny";
                                    name2_fir = "kunsztownego";
                                    name3_fir = "kunsztownemu";
                                    name4_fir = "kunsztowny";
                                    name5_fir = "kunsztownym";
                                    name6_fir = "kunsztownym";
                                    break;
                                case 8:
                                    name_fir = "gnomi";
                                    sh_fir = "gnomi";
                                    name2_fir = "gnomiego";
                                    name3_fir = "gnomiemu";
                                    name4_fir = "gnomi";
                                    name5_fir = "gnomim";
                                    name6_fir = "gnomim";
                                    break;
                                case 9:
                                    name_fir = "elfi";
                                    sh_fir = "elfi";
                                    name2_fir = "elfiego";
                                    name3_fir = "elfiemu";
                                    name4_fir = "elfi";
                                    name5_fir = "elfim";
                                    name6_fir = "elfim";
                                    obj->material = 3;
                                    break;
                                case 10:
                                    name_fir = "krasnoludzki";
                                    sh_fir = "krasnoludzki";
                                    name2_fir = "krasnoludzkiego";
                                    name3_fir = "krasnoludzkiemu";
                                    name4_fir = "krasnoludzki";
                                    name5_fir = "krasnoludzkim";
                                    name6_fir = "krasnoludzkim";
                                    break;
                                case 11:
                                    name_fir = "grawerowany";
                                    sh_fir = "grawerowany";
                                    name2_fir = "grawerowanego";
                                    name3_fir = "grawerowanemu";
                                    name4_fir = "grawerowany";
                                    name5_fir = "grawerowanym";
                                    name6_fir = "grawerowanym";
                                    break;
                                case 12:
                                    name_fir = "obosieczny";
                                    sh_fir = "obosieczny";
                                    name2_fir = "obosiecznego";
                                    name3_fir = "obosiecznemu";
                                    name4_fir = "obosieczny";
                                    name5_fir = "obosiecznym";
                                    name6_fir = "obosiecznym";
                                    break;
                                case 13:
                                    name_fir = "prosty";
                                    sh_fir = "prosty";
                                    name2_fir = "prostego";
                                    name3_fir = "prostemu";
                                    name4_fir = "prosty";
                                    name5_fir = "prostym";
                                    name6_fir = "prostym";
                                    break;
                                case 14:
                                    name_fir = "powykrzywiany";
                                    sh_fir = "powykrzywiany";
                                    name2_fir = "powykrzywianego";
                                    name3_fir = "powykrzywianemu";
                                    name4_fir = "powykrzywiany";
                                    name5_fir = "powykrzywianym";
                                    name6_fir = "powykrzywianym";
                                    --obj->value[2];
                                    break;
                                case 15:
                                    name_fir = "zdobiony";
                                    sh_fir = "zdobiony";
                                    name2_fir = "zdobionego";
                                    name3_fir = "zdobionemu";
                                    name4_fir = "zdobiony";
                                    name5_fir = "zdobionym";
                                    name6_fir = "zdobionym";
                                    break;
                                case 16:
                                    name_fir = "wyszczerbiony";
                                    sh_fir = "wyszczerbiony";
                                    name2_fir = "wyszczerbionego";
                                    name3_fir = "wyszczerbionemu";
                                    name4_fir = "wyszczerbiony";
                                    name5_fir = "wyszczerbionym";
                                    name6_fir = "wyszczerbionym";
                                    break;
                                case 17:
                                    name_fir = "zêbaty";
                                    sh_fir = "zêbaty";
                                    name2_fir = "zêbatego";
                                    name3_fir = "zêbatemu";
                                    name4_fir = "zêbaty";
                                    name5_fir = "zêbatym";
                                    name6_fir = "zêbatym";
                                    break;
                                case 18:
                                    name_fir = "porêczny";
                                    sh_fir = "porêczny";
                                    name2_fir = "porêcznego";
                                    name3_fir = "porêcznemu";
                                    name4_fir = "porêczny";
                                    name5_fir = "porêcznym";
                                    name6_fir = "porêcznym";
                                    break;
                                case 19:
                                    name_fir = "po³yskuj±cy";
                                    sh_fir = "po³yskuj±cy";
                                    name2_fir = "po³yskuj±cego";
                                    name3_fir = "po³yskuj±cemu";
                                    name4_fir = "po³yskuj±cy";
                                    name5_fir = "po³yskuj±cym";
                                    name6_fir = "po³yskuj±cym";
                                    break;
                                case 20:
                                    name_fir = "opalizuj±cy";
                                    sh_fir = "opalizuj±cy";
                                    name2_fir = "opalizuj±cego";
                                    name3_fir = "opalizuj±cemu";
                                    name4_fir = "opalizuj±cy";
                                    name5_fir = "opalizuj±cym";
                                    name6_fir = "opalizuj±cym";
                                    break;
                                case 21:
                                    name_fir = "wywa¿ony";
                                    sh_fir = "wywa¿ony";
                                    name2_fir = "wywa¿onego";
                                    name3_fir = "wywa¿onemu";
                                    name4_fir = "wywa¿ony";
                                    name5_fir = "wywa¿onym";
                                    name6_fir = "wywa¿onym";
                                    break;
                                case 22:
                                    name_fir = "szary";
                                    sh_fir = "szary";
                                    name2_fir = "szarego";
                                    name3_fir = "szaremu";
                                    name4_fir = "szary";
                                    name5_fir = "szarym";
                                    name6_fir = "szarym";
                                    break;
                                case 23:
                                    name_fir = "stary";
                                    sh_fir = "stary";
                                    name2_fir = "starego";
                                    name3_fir = "staremu";
                                    name4_fir = "stary";
                                    name5_fir = "starym";
                                    name6_fir = "starym";
                                    --obj->value[ 2 ];
                                    break;
                                case 24:
                                    name_fir = "podniszczony";
                                    sh_fir = "podniszczony";
                                    name2_fir = "podniszczonego";
                                    name3_fir = "podniszczonemu";
                                    name4_fir = "podniszczony";
                                    name5_fir = "podniszczonym";
                                    name6_fir = "podniszczonym";
                                    --obj->value[ 2 ];
                                    break;
                                case 25:
                                    name_fir = "zadbany";
                                    sh_fir = "zadbany";
                                    name2_fir = "zadbanego";
                                    name3_fir = "zadbanemu";
                                    name4_fir = "zadbany";
                                    name5_fir = "zadbanym";
                                    name6_fir = "zadbanym";
                                    break;
                                case 26:
                                    name_fir = "zaostrzony";
                                    sh_fir = "zaostrzony";
                                    name2_fir = "zaostrzonego";
                                    name3_fir = "zaostrzonemu";
                                    name4_fir = "zaostrzony";
                                    name5_fir = "zaostrzonym";
                                    name6_fir = "zaostrzonym";
                                    break;
                                case 27:
                                    name_fir = "z±bkowany";
                                    sh_fir = "z±bkowany";
                                    name2_fir = "z±bkowanego";
                                    name3_fir = "z±bkowanemu";
                                    name4_fir = "z±bkowany";
                                    name5_fir = "z±bkowanym";
                                    name6_fir = "z±bkowanym";
                                    break;
                                case 28:
                                    name_fir = "antyczny";
                                    sh_fir = "antyczny";
                                    name2_fir = "antycznego";
                                    name3_fir = "antycznemu";
                                    name4_fir = "antyczny";
                                    name5_fir = "antycznym";
                                    name6_fir = "antycznym";
                                    break;
                                case 29:
                                    name_fir = "pod³u¿ny";
                                    sh_fir = "pod³u¿ny";
                                    name2_fir = "pod³u¿nego";
                                    name3_fir = "pod³u¿nemu";
                                    name4_fir = "pod³u¿ny";
                                    name5_fir = "pod³u¿nym";
                                    name6_fir = "pod³u¿nym";
                                    break;
                                case 30:
                                    name_fir = "stalowy";
                                    sh_fir = "stalowy";
                                    name2_fir = "stalowego";
                                    name3_fir = "stalowemu";
                                    name4_fir = "stalowy";
                                    name5_fir = "stalowym";
                                    name6_fir = "stalowym";
                                    break;
                                case 31:
                                    name_fir = "miedziany";
                                    sh_fir = "miedziany";
                                    name2_fir = "miedzianego";
                                    name3_fir = "miedzianemu";
                                    name4_fir = "miedziany";
                                    name5_fir = "miedzianym";
                                    name6_fir = "miedzianym";
                                    obj->material = 5;
                                    break;
                                case 32:
                                    name_fir = "adamantytowy";
                                    sh_fir = "adamantytowy";
                                    name2_fir = "adamantytowego";
                                    name3_fir = "adamantytowemu";
                                    name4_fir = "adamantytowy";
                                    name5_fir = "adamantytowym";
                                    name6_fir = "adamantytowym";
                                    obj->material = 7;
                                    break;
                                case 33:
                                    name_fir = "poz³acany";
                                    sh_fir = "poz³acany";
                                    name2_fir = "poz³acanego";
                                    name3_fir = "poz³acanemu";
                                    name4_fir = "poz³acany";
                                    name5_fir = "poz³acanym";
                                    name6_fir = "poz³acanym";
                                    break;
                                case 34:
                                    name_fir = "posrebrzany";
                                    sh_fir = "posrebrzany";
                                    name2_fir = "posrebrzanego";
                                    name3_fir = "posrebrzanemu";
                                    name4_fir = "posrebrzany";
                                    name5_fir = "posrebrzanym";
                                    name6_fir = "posrebrzanym";
                                    break;
                                case 35:
                                    name_fir = "mithrilowy";
                                    sh_fir = "mithrilowy";
                                    name2_fir = "mithrilowego";
                                    name3_fir = "mithrilowemu";
                                    name4_fir = "mithrilowy";
                                    name5_fir = "mithrilowym";
                                    name6_fir = "mithrilowym";
                                    obj->material = 8;
                                    break;
                                case 36:
                                    name_fir = "z³oty";
                                    sh_fir = "z³oty";
                                    name2_fir = "z³otego";
                                    name3_fir = "z³otemu";
                                    name4_fir = "z³oty";
                                    name5_fir = "z³otym";
                                    name6_fir = "z³otym";
                                    obj->material = 9;
                                    break;
                                case 37:
                                    name_fir = "srebrny";
                                    sh_fir = "srebrny";
                                    name2_fir = "srebrnego";
                                    name3_fir = "srebrnemu";
                                    name4_fir = "srebrny";
                                    name5_fir = "srebrnym";
                                    name6_fir = "srebrnym";
                                    obj->material = 10;
                                    break;
                                case 38:
                                    name_fir = "¿elazny";
                                    sh_fir = "¿elazny";
                                    name2_fir = "¿elaznego";
                                    name3_fir = "¿elaznemu";
                                    name4_fir = "¿elazny";
                                    name5_fir = "¿elaznym";
                                    name6_fir = "¿elaznym";
                                    obj->material = 13;
                                    break;
                                case 39:
                                    name_fir = "br±zowy";
                                    sh_fir = "br±zowy";
                                    name2_fir = "br±zowego";
                                    name3_fir = "br±zowemu";
                                    name4_fir = "br±zowy";
                                    name5_fir = "br±zowym";
                                    name6_fir = "br±zowym";
                                    obj->material = 14;
                                    break;
                                case 40:
                                    name_fir = "damasceñski";
                                    sh_fir = "damasceñski";
                                    name2_fir = "damasceñskiego";
                                    name3_fir = "damasceñskiemu";
                                    name4_fir = "damasceñski";
                                    name5_fir = "damasceñskim";
                                    name6_fir = "damasceñskim";
                                    obj->material = 32;
                                    break;
                                case 41:
                                    name_fir = "illitowy";
                                    sh_fir = "illitowy";
                                    name2_fir = "illitowego";
                                    name3_fir = "illitowemu";
                                    name4_fir = "illitowy";
                                    name5_fir = "illitowym";
                                    name6_fir = "illitowym";
                                    obj->material = 33;
                                    break;
                                case 42:
                                    name_fir = "platynowy";
                                    sh_fir = "platynowy";
                                    name2_fir = "platynowego";
                                    name3_fir = "platynowemu";
                                    name4_fir = "platynowy";
                                    name5_fir = "platynowym";
                                    name6_fir = "platynowym";
                                    obj->material = 34;
                                    break;
                                case 43:
                                    name_fir = "diamentowy";
                                    sh_fir = "diamentowy";
                                    name2_fir = "diamentowego";
                                    name3_fir = "diamentowemu";
                                    name4_fir = "diamentowy";
                                    name5_fir = "diamentowym";
                                    name6_fir = "diamentowym";
                                    obj->material = 36;
                                    break;
                                case 44:
                                    name_fir = "chitynowy";
                                    sh_fir = "chitynowy";
                                    name2_fir = "chitynowego";
                                    name3_fir = "chitynowemu";
                                    name4_fir = "chitynowy";
                                    name5_fir = "chitynowym";
                                    name6_fir = "chitynowym";
                                    obj->material = 40;
                                    break;
                                case 45:
                                    name_fir = "smoczy";
                                    sh_fir = "smoczy";
                                    name2_fir = "smoczego";
                                    name3_fir = "smoczemu";
                                    name4_fir = "smoczy";
                                    name5_fir = "smoczym";
                                    name6_fir = "smoczym";
                                    obj->material = 42;
                                    break;
                                case 46:
                                    name_fir = "mosiê¿ny";
                                    sh_fir = "mosiê¿ny";
                                    name2_fir = "mosiê¿nego";
                                    name3_fir = "mosiê¿nemu";
                                    name4_fir = "mosiê¿ny";
                                    name5_fir = "mosiê¿nym";
                                    name6_fir = "mosiê¿nym";
                                    obj->material = 43;
                                    break;
                                case 47:
                                    name_fir = "eteryczny";
                                    sh_fir = "eteryczny";
                                    name2_fir = "eterycznego";
                                    name3_fir = "eterycznemu";
                                    name4_fir = "eteryczny";
                                    name5_fir = "eterycznym";
                                    name6_fir = "eterycznym";
                                    obj->material = 45;
                                    obj->weight = 1;
                                    break;
                                case 48:
                                    name_fir = "têpy";
                                    sh_fir = "têpy";
                                    name2_fir = "têpego";
                                    name3_fir = "têpemu";
                                    name4_fir = "têpy";
                                    name5_fir = "têpym";
                                    name6_fir = "têpym";
                                    --obj->value[ 2 ];
                                    break;
                            }
                            switch( number_range(1,3))
                            {
                                case 1:
                                    name_des = " gladius";
                                    sh_des = " gladius";
                                    name2_des = " gladiusa";
                                    name3_des = " gladiusowi";
                                    name4_des = " gladius";
                                    name5_des = " gladiusem";
                                    name6_des = " gladiusie";
                                    break;
                                case 2:
                                    name_des = " krótki miecz";
                                    sh_des = " krótki miecz";
                                    name2_des = " krótkiego miecza";
                                    name3_des = " krótkiemu mieczowi";
                                    name4_des = " krótki miecz";
                                    name5_des = " krótkim mieczem";
                                    name6_des = " krótkim mieczu";
                                    break;
                                case 3:
                                    name_des = " ma³y miecz";
                                    sh_des = " ma³y miecz";
                                    name2_des = " ma³ego miecza";
                                    name3_des = " ma³emu mieczowi";
                                    name4_des = " ma³y miecz";
                                    name5_des = " ma³ym mieczem";
                                    name6_des = " ma³ym mieczu";
                                    break;
                            }
                    }
                    break;
                case 3://2h
                    obj->weight = number_range( 150, 350 );
                    SET_BIT( obj->value[4], WEAPON_TWO_HANDS );

                    /* kostki obra¿eñ */
                    {
                        int kostki[8][2] = { { 1,7 }, { 2,5 }, { 1,8 },
                            { 2,6 }, { 3,4 }, { 2,7 },
                            { 4,3 }, { 3,5 }};
                        int tmp = number_range( 0, (wer*2)-1 );

                        obj->value[ 1 ] = kostki[tmp][0];
                        obj->value[ 2 ] = kostki[tmp][1];
                    }

                    if( wer > 3 )
                    {
                        obj->value[ 5 ] = number_range( 0, 2 );
                        obj->value[ 6 ] = number_range( 0, 2 );
                    }

                    switch( number_range(1,56))
                    {
                        case 1:
                            name_fir = "d³ugi";
                            sh_fir = "d³ugi";
                            name2_fir = "d³ugiego";
                            name3_fir = "d³ugiemu";
                            name4_fir = "d³ugi";
                            name5_fir = "d³ugim";
                            name6_fir = "d³ugim";
                            break;
                        case 2:
                            name_fir = "ostry";
                            sh_fir = "ostry";
                            name2_fir = "ostrego";
                            name3_fir = "ostremu";
                            name4_fir = "ostry";
                            name5_fir = "ostrym";
                            name6_fir = "ostrym";
                            break;
                        case 3:
                            name_fir = "b³yszcz±cy";
                            sh_fir = "b³yszcz±cy";
                            name2_fir = "b³yszcz±cego";
                            name3_fir = "b³yszcz±cym";
                            name4_fir = "b³yszcz±cy";
                            name5_fir = "b³yszcz±cym";
                            name6_fir = "b³yszcz±cym";
                            break;
                        case 4:
                            name_fir = "lekki";
                            sh_fir = "lekki";
                            name2_fir = "lekkiego";
                            name3_fir = "lekkiemu";
                            name4_fir = "lekki";
                            name5_fir = "lekkim";
                            name6_fir = "lekkim";
                            obj->weight /= 2;
                            break;
                        case 5:
                            name_fir = "ciê¿ki";
                            sh_fir = "ciê¿ki";
                            name2_fir = "ciêzkiego";
                            name3_fir = "cie¿kiemu";
                            name4_fir = "ciê¿ki";
                            name5_fir = "ciê¿kim";
                            name6_fir = "ciê¿kim";
                            obj->weight *= 2;
                            break;
                        case 6:
                            name_fir = "poszczerbiony";
                            sh_fir = "poszczerbiony";
                            name2_fir = "poszczerbionego";
                            name3_fir = "poszczerbionemu";
                            name4_fir = "poszczerbiony";
                            name5_fir = "poszczerbionym";
                            name6_fir = "poszczerbionym";
                            --obj->value[2];
                            break;
                        case 7:
                            name_fir = "l¶ni±cy";
                            sh_fir = "l¶ni±cy";
                            name2_fir = "l¶ni±cego";
                            name3_fir = "l¶ni±cemu";
                            name4_fir = "l¶ni±cy";
                            name5_fir = "l¶ni±cym";
                            name6_fir = "l¶ni±cym";
                            break;
                        case 8:
                            name_fir = "szeroki";
                            sh_fir = "szeroki";
                            name2_fir = "szerokiego";
                            name3_fir = "szerokiemu";
                            name4_fir = "szeroki";
                            name5_fir = "szerokim";
                            name6_fir = "szerokim";
                            break;
                        case 9:
                            name_fir = "w±ski";
                            sh_fir = "w±ski";
                            name2_fir = "w±skiego";
                            name3_fir = "w±skiemi";
                            name4_fir = "w±ski";
                            name5_fir = "w±skim";
                            name6_fir = "w±skim";
                            break;
                        case 10:
                            name_fir = "czarny";
                            sh_fir = "czarny";
                            name2_fir = "czarnego";
                            name3_fir = "czarnemu";
                            name4_fir = "czarny";
                            name5_fir = "czarnym";
                            name6_fir = "czarnym";
                            break;
                        case 11:
                            name_fir = "smuk³y";
                            sh_fir = "smuk³y";
                            name2_fir = "smuk³ego";
                            name3_fir = "smuk³emu";
                            name4_fir = "smuk³y";
                            name5_fir = "smuk³ym";
                            name6_fir = "smuk³ym";
                            break;
                        case 12:
                            name_fir = "kunsztowny";
                            sh_fir = "kunsztowny";
                            name2_fir = "kunsztownego";
                            name3_fir = "kunsztownemu";
                            name4_fir = "kunsztowny";
                            name5_fir = "kunsztownym";
                            name6_fir = "kunsztownym";
                            break;
                        case 13:
                            name_fir = "gnomi";
                            sh_fir = "gnomi";
                            name2_fir = "gnomiego";
                            name3_fir = "gnomiemu";
                            name4_fir = "gnomi";
                            name5_fir = "gnomim";
                            name6_fir = "gnomim";
                            break;
                        case 14:
                            name_fir = "elfi";
                            sh_fir = "elfi";
                            name2_fir = "elfiego";
                            name3_fir = "elfiemu";
                            name4_fir = "elfi";
                            name5_fir = "elfim";
                            name6_fir = "elfim";
                            obj->material = 3;
                            break;
                        case 15:
                            name_fir = "krasnoludzki";
                            sh_fir = "krasnoludzki";
                            name2_fir = "krasnoludzkiego";
                            name3_fir = "krasnoludzkiemu";
                            name4_fir = "krasnoludzki";
                            name5_fir = "krasnoludzkim";
                            name6_fir = "krasnoludzkim";
                            break;
                        case 16:
                            name_fir = "grawerowany";
                            sh_fir = "grawerowany";
                            name2_fir = "grawerowanego";
                            name3_fir = "grawerowanemu";
                            name4_fir = "grawerowany";
                            name5_fir = "grawerowanym";
                            name6_fir = "grawerowanym";
                            break;
                        case 17:
                            name_fir = "obosieczny";
                            sh_fir = "obosieczny";
                            name2_fir = "obosiecznego";
                            name3_fir = "obosiecznemu";
                            name4_fir = "obosieczny";
                            name5_fir = "obosiecznym";
                            name6_fir = "obosiecznym";
                            break;
                        case 18:
                            name_fir = "prosty";
                            sh_fir = "prosty";
                            name2_fir = "prostego";
                            name3_fir = "prostemu";
                            name4_fir = "prosty";
                            name5_fir = "prostym";
                            name6_fir = "prostym";
                            break;
                        case 19:
                            name_fir = "powykrzywiany";
                            sh_fir = "powykrzywiany";
                            name2_fir = "powykrzywianego";
                            name3_fir = "powykrzywianemu";
                            name4_fir = "powykrzywiany";
                            name5_fir = "powykrzywianym";
                            name6_fir = "powykrzywianym";
                            --obj->value[2];
                            break;
                        case 20:
                            name_fir = "zdobiony";
                            sh_fir = "zdobiony";
                            name2_fir = "zdobionego";
                            name3_fir = "zdobionemu";
                            name4_fir = "zdobiony";
                            name5_fir = "zdobionym";
                            name6_fir = "zdobionym";
                            break;
                        case 21:
                            name_fir = "wyszczerbiony";
                            sh_fir = "wyszczerbiony";
                            name2_fir = "wyszczerbionego";
                            name3_fir = "wyszczerbionemu";
                            name4_fir = "wyszczerbiony";
                            name5_fir = "wyszczerbionym";
                            name6_fir = "wyszczerbionym";
                            break;
                        case 22:
                            name_fir = "zêbaty";
                            sh_fir = "zêbaty";
                            name2_fir = "zêbatego";
                            name3_fir = "zêbatemu";
                            name4_fir = "zêbaty";
                            name5_fir = "zêbatym";
                            name6_fir = "zêbatym";
                            break;
                        case 23:
                            name_fir = "porêczny";
                            sh_fir = "porêczny";
                            name2_fir = "porêcznego";
                            name3_fir = "porêcznemu";
                            name4_fir = "porêczny";
                            name5_fir = "porêcznym";
                            name6_fir = "porêcznym";
                            break;
                        case 24:
                            name_fir = "po³yskuj±cy";
                            sh_fir = "po³yskuj±cy";
                            name2_fir = "po³yskuj±cego";
                            name3_fir = "po³yskuj±cemu";
                            name4_fir = "po³yskuj±cy";
                            name5_fir = "po³yskuj±cym";
                            name6_fir = "po³yskuj±cym";
                            break;
                        case 25:
                            name_fir = "opalizuj±cy";
                            sh_fir = "opalizuj±cy";
                            name2_fir = "opalizuj±cego";
                            name3_fir = "opalizuj±cemu";
                            name4_fir = "opalizuj±cy";
                            name5_fir = "opalizuj±cym";
                            name6_fir = "opalizuj±cym";
                            break;
                        case 26:
                            name_fir = "wywa¿ony";
                            sh_fir = "wywa¿ony";
                            name2_fir = "wywa¿onego";
                            name3_fir = "wywa¿onemu";
                            name4_fir = "wywa¿ony";
                            name5_fir = "wywa¿onym";
                            name6_fir = "wywa¿onym";
                            break;
                        case 27:
                            name_fir = "szary";
                            sh_fir = "szary";
                            name2_fir = "szarego";
                            name3_fir = "szaremu";
                            name4_fir = "szary";
                            name5_fir = "szarym";
                            name6_fir = "szarym";
                            break;
                        case 28:
                            name_fir = "stary";
                            sh_fir = "stary";
                            name2_fir = "starego";
                            name3_fir = "staremu";
                            name4_fir = "stary";
                            name5_fir = "starym";
                            name6_fir = "starym";
                            --obj->value[ 2 ];
                            break;
                        case 29:
                            name_fir = "podniszczony";
                            sh_fir = "podniszczony";
                            name2_fir = "podniszczonego";
                            name3_fir = "podniszczonemu";
                            name4_fir = "podniszczony";
                            name5_fir = "podniszczonym";
                            name6_fir = "podniszczonym";
                            --obj->value[ 2 ];
                            break;
                        case 30:
                            name_fir = "zadbany";
                            sh_fir = "zadbany";
                            name2_fir = "zadbanego";
                            name3_fir = "zadbanemu";
                            name4_fir = "zadbany";
                            name5_fir = "zadbanym";
                            name6_fir = "zadbanym";
                            break;
                        case 31:
                            name_fir = "zaostrzony";
                            sh_fir = "zaostrzony";
                            name2_fir = "zaostrzonego";
                            name3_fir = "zaostrzonemu";
                            name4_fir = "zaostrzony";
                            name5_fir = "zaostrzonym";
                            name6_fir = "zaostrzonym";
                            break;
                        case 32:
                            name_fir = "z±bkowany";
                            sh_fir = "z±bkowany";
                            name2_fir = "z±bkowanego";
                            name3_fir = "z±bkowanemu";
                            name4_fir = "z±bkowany";
                            name5_fir = "z±bkowanym";
                            name6_fir = "z±bkowanym";
                            break;
                        case 33:
                            name_fir = "antyczny";
                            sh_fir = "antyczny";
                            name2_fir = "antycznego";
                            name3_fir = "antycznemu";
                            name4_fir = "antyczny";
                            name5_fir = "antycznym";
                            name6_fir = "antycznym";
                            break;
                        case 34:
                            name_fir = "pod³u¿ny";
                            sh_fir = "pod³u¿ny";
                            name2_fir = "pod³u¿nego";
                            name3_fir = "pod³u¿nemu";
                            name4_fir = "pod³u¿ny";
                            name5_fir = "pod³u¿nym";
                            name6_fir = "pod³u¿nym";
                            break;
                        case 35:
                            name_fir = "stalowy";
                            sh_fir = "stalowy";
                            name2_fir = "stalowego";
                            name3_fir = "stalowemu";
                            name4_fir = "stalowy";
                            name5_fir = "stalowym";
                            name6_fir = "stalowym";
                            break;
                        case 36:
                            name_fir = "miedziany";
                            sh_fir = "miedziany";
                            name2_fir = "miedzianego";
                            name3_fir = "miedzianemu";
                            name4_fir = "miedziany";
                            name5_fir = "miedzianym";
                            name6_fir = "miedzianym";
                            obj->material = 5;
                            break;
                        case 37:
                            name_fir = "adamantytowy";
                            sh_fir = "adamantytowy";
                            name2_fir = "adamantytowego";
                            name3_fir = "adamantytowemu";
                            name4_fir = "adamantytowy";
                            name5_fir = "adamantytowym";
                            name6_fir = "adamantytowym";
                            obj->material = 7;
                            break;
                        case 38:
                            name_fir = "poz³acany";
                            sh_fir = "poz³acany";
                            name2_fir = "poz³acanego";
                            name3_fir = "poz³acanemu";
                            name4_fir = "poz³acany";
                            name5_fir = "poz³acanym";
                            name6_fir = "poz³acanym";
                            break;
                        case 39:
                            name_fir = "posrebrzany";
                            sh_fir = "posrebrzany";
                            name2_fir = "posrebrzanego";
                            name3_fir = "posrebrzanemu";
                            name4_fir = "posrebrzany";
                            name5_fir = "posrebrzanym";
                            name6_fir = "posrebrzanym";
                            break;
                        case 40:
                            name_fir = "mithrilowy";
                            sh_fir = "mithrilowy";
                            name2_fir = "mithrilowego";
                            name3_fir = "mithrilowemu";
                            name4_fir = "mithrilowy";
                            name5_fir = "mithrilowym";
                            name6_fir = "mithrilowym";
                            obj->material = 8;
                            break;
                        case 41:
                            name_fir = "z³oty";
                            sh_fir = "z³oty";
                            name2_fir = "z³otego";
                            name3_fir = "z³otemu";
                            name4_fir = "z³oty";
                            name5_fir = "z³otym";
                            name6_fir = "z³otym";
                            obj->material = 9;
                            break;
                        case 42:
                            name_fir = "srebrny";
                            sh_fir = "srebrny";
                            name2_fir = "srebrnego";
                            name3_fir = "srebrnemu";
                            name4_fir = "srebrny";
                            name5_fir = "srebrnym";
                            name6_fir = "srebrnym";
                            obj->material = 10;
                            break;
                        case 43:
                            name_fir = "¿elazny";
                            sh_fir = "¿elazny";
                            name2_fir = "¿elaznego";
                            name3_fir = "¿elaznemu";
                            name4_fir = "¿elazny";
                            name5_fir = "¿elaznym";
                            name6_fir = "¿elaznym";
                            obj->material = 13;
                            break;
                        case 44:
                            name_fir = "br±zowy";
                            sh_fir = "br±zowy";
                            name2_fir = "br±zowego";
                            name3_fir = "br±zowemu";
                            name4_fir = "br±zowy";
                            name5_fir = "br±zowym";
                            name6_fir = "br±zowym";
                            obj->material = 14;
                            break;
                        case 45:
                            name_fir = "damasceñski";
                            sh_fir = "damasceñski";
                            name2_fir = "damasceñskiego";
                            name3_fir = "damasceñskiemu";
                            name4_fir = "damasceñski";
                            name5_fir = "damasceñskim";
                            name6_fir = "damasceñskim";
                            obj->material = 32;
                            break;
                        case 46:
                            name_fir = "illitowy";
                            sh_fir = "illitowy";
                            name2_fir = "illitowego";
                            name3_fir = "illitowemu";
                            name4_fir = "illitowy";
                            name5_fir = "illitowym";
                            name6_fir = "illitowym";
                            obj->material = 33;
                            break;
                        case 47:
                            name_fir = "platynowy";
                            sh_fir = "platynowy";
                            name2_fir = "platynowego";
                            name3_fir = "platynowemu";
                            name4_fir = "platynowy";
                            name5_fir = "platynowym";
                            name6_fir = "platynowym";
                            obj->material = 34;
                            break;
                        case 48:
                            name_fir = "diamentowy";
                            sh_fir = "diamentowy";
                            name2_fir = "diamentowego";
                            name3_fir = "diamentowemu";
                            name4_fir = "diamentowy";
                            name5_fir = "diamentowym";
                            name6_fir = "diamentowym";
                            obj->material = 36;
                            break;
                        case 49:
                            name_fir = "chitynowy";
                            sh_fir = "chitynowy";
                            name2_fir = "chitynowego";
                            name3_fir = "chitynowemu";
                            name4_fir = "chitynowy";
                            name5_fir = "chitynowym";
                            name6_fir = "chitynowym";
                            obj->material = 40;
                            break;
                        case 50:
                            name_fir = "smoczy";
                            sh_fir = "smoczy";
                            name2_fir = "smoczego";
                            name3_fir = "smoczemu";
                            name4_fir = "smoczy";
                            name5_fir = "smoczym";
                            name6_fir = "smoczym";
                            obj->material = 42;
                            break;
                        case 51:
                            name_fir = "mosiê¿ny";
                            sh_fir = "mosiê¿ny";
                            name2_fir = "mosiê¿nego";
                            name3_fir = "mosiê¿nemu";
                            name4_fir = "mosiê¿ny";
                            name5_fir = "mosiê¿nym";
                            name6_fir = "mosiê¿nym";
                            obj->material = 43;
                            break;
                        case 52:
                            name_fir = "eteryczny";
                            sh_fir = "eteryczny";
                            name2_fir = "eterycznego";
                            name3_fir = "eterycznemu";
                            name4_fir = "eteryczny";
                            name5_fir = "eterycznym";
                            name6_fir = "eterycznym";
                            obj->material = 45;
                            obj->weight = 1;
                            break;
                        case 53:
                            name_fir = "têpy";
                            sh_fir = "têpy";
                            name2_fir = "têpego";
                            name3_fir = "têpemu";
                            name4_fir = "têpy";
                            name5_fir = "têpym";
                            name6_fir = "têpym";
                            --obj->value[ 2 ];
                            break;
                        case 54:
                            name_fir = "wielki";
                            sh_fir = "wielki";
                            name2_fir = "wielkiego";
                            name3_fir = "wielkiemu";
                            name4_fir = "wielki";
                            name5_fir = "wielkim";
                            name6_fir = "wielkim";
                            break;
                        case 55:
                            name_fir = "masywny";
                            sh_fir = "masywny";
                            name2_fir = "masywnego";
                            name3_fir = "masywnemu";
                            name4_fir = "masywny";
                            name5_fir = "masywnym";
                            name6_fir = "masywnym";
                            break;
                        case 56:
                            name_fir = "potê¿ny";
                            sh_fir = "potê¿ny";
                            name2_fir = "potê¿nego";
                            name3_fir = "potê¿nemu";
                            name4_fir = "potê¿ny";
                            name5_fir = "potê¿nym";
                            name6_fir = "potê¿nym";
                            break;
                    }

                    switch( number_range(1,6))
                    {
                        case 1:
                            name_des = " miecz dwurêczny";
                            sh_des = " miecz dwurêczny";
                            name2_des = " miecza dwurêcznego";
                            name3_des = " mieczowi dwurêcznemui";
                            name4_des = " miecz dwurêczny";
                            name5_des = " mieczem dwurêcznym";
                            name6_des = " mieczu dwurêcznym";
                            break;
                        case 2:
                            name_des = " espadon";
                            sh_des = " espadon";
                            name2_des = " espadona";
                            name3_des = " espadonowi";
                            name4_des = " espadon";
                            name5_des = " espadonem";
                            name6_des = " espadonie";
                            break;
                        case 3:
                            name_des = " flamberg";
                            sh_des = " flamberg";
                            name2_des = " flamberga";
                            name3_des = " flambergowi";
                            name4_des = " flamberg";
                            name5_des = " flambergiem";
                            name6_des = " flambergu";
                            break;
                        case 4:
                            name_des = " hispadon";
                            sh_des = " hispadon";
                            name2_des = " hispadona";
                            name3_des = " hispadonowi";
                            name4_des = " hispadon";
                            name5_des = " hispadonem";
                            name6_des = " hispadonie";
                            break;
                        case 5:
                            name_des = " koncerz";
                            sh_des = " koncerz";
                            name2_des = " koncerza";
                            name3_des = " koncerzowi";
                            name4_des = " koncerz";
                            name5_des = " koncerzem";
                            name6_des = " koncerzu";
                            break;
                        case 6:
                            name_des = " claymore";
                            sh_des = " claymore";
                            name2_des = " claymora";
                            name3_des = " claymorowi";
                            name4_des = " claymore";
                            name5_des = " claymorem";
                            name6_des = " claymorze";
                            break;
                    }
                    break;
            }

            sprintf( txt, "%s%s", name_fir, name_des );
            name = str_dup( txt);
            sprintf( txt, "%s%s", sh_fir, sh_des );
            sh_desc = str_dup( txt);
            sprintf( txt, "%s%s", name2_fir, name2_des );
            name2 = str_dup( txt);
            sprintf( txt, "%s%s", name3_fir, name3_des );
            name3 = str_dup( txt);
            sprintf( txt, "%s%s", name4_fir, name4_des );
            name4 = str_dup( txt);
            sprintf( txt, "%s%s", name5_fir, name5_des );
            name5 = str_dup( txt);
            sprintf( txt, "%s%s", name6_fir, name6_des );
            name6 =str_dup(  txt);

            break;

        case WEAPON_DAGGER:

            obj->material = 2;

            obj->value[ 1 ] = 1;
            obj->value[ 2 ] = 2;
            obj->value[ 3 ] = 2; // dam_type "pchniêcie"
            obj->value[ 5 ] = 0;
            obj->value[ 6 ] = 0;

            obj->weight = 25;

            switch ( number_range( 1, 21 ) )
            {
                case 1:
                    name = "sztylet";
                    sh_desc = "sztylet";
                    name2 = "sztyletu";
                    name3 = "sztyletowi";
                    name4 = "sztylet";
                    name5 = "sztyletem";
                    name6 = "sztylecie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 4;
                            break;
                        case 2:
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 4:
                            obj->value[ 2 ] = 7;
                            break;
                    }
                    obj->weight = 40;
                    break;

                case 2:
                    name = "nó¿";
                    sh_desc = "nó¿";
                    name2 = "no¿a";
                    name3 = "no¿owi";
                    name4 = "nó¿";
                    name5 = "no¿em";
                    name6 = "no¿u";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 4:
                            obj->value[ 2 ] = 6;
                            break;
                    }

                    break;

                case 3:
                    name = "d³ugi sztylet";
                    sh_desc = "d³ugi sztylet";
                    name2 = "dlugiego sztyletu";
                    name3 = "dlugiemu sztyletowi";
                    name4 = "d³ugi sztylet";
                    name5 = "dlugim sztyletem";
                    name6 = "dlugim sztylecie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 2 ] = 7;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                    }

                    obj->weight = 50;
                    break;

                case 4:
                    name = "krótki sztylet";
                    sh_desc = "krótki sztylet";
                    name2 = "krótkiego sztyletu";
                    name3 = "krótkiemu sztyletowi";
                    name4 = "krótki sztylet";
                    name5 = "krótkim sztyletem";
                    name6 = "krótkim sztylecie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 2 ] = 5;
                            break;
                        case 4:
                            obj->value[ 2 ] = 7;
                            break;
                    }

                    obj->weight = 30;
                    break;

                case 5:
                    name = "ostry nó¿";
                    sh_desc = "ostry nó¿";
                    name2 = "ostrego no¿a";
                    name3 = "ostremu no¿owi";
                    name4 = "ostry nó¿";
                    name5 = "ostrym no¿em";
                    name6 = "ostrym no¿u";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 6 ] = 1;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                    }

                    break;

                case 6:
                    name = "ostry sztylet";
                    sh_desc = "ostry sztylet";
                    name2 = "ostrego sztyletu";
                    name3 = "ostremu sztyletowi";
                    name4 = "ostry sztylet";
                    name5 = "ostrym sztyletem";
                    name6 = "ostrym sztylecie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 2 ] = 5;
                            obj->value[ 5 ] = 1;
                            obj->value[ 6 ] = 1;
                            break;

                        case 4:
                            obj->value[ 2 ] = 6;
                            obj->value[ 6 ] = 1;
                            break;
                    }
                    break;

                case 7:
                    name = "zakrzywiony sztylet";
                    sh_desc = "zakrzywiony sztylet";
                    name2 = "zakrzywionego sztyletu";
                    name3 = "zakrzywionemu sztyletowi";
                    name4 = "zakrzywiony sztylet";
                    name5 = "zakrzywionym sztyletem";
                    name6 = "zakrzywionym sztylecie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 2 ] = 4;
                            obj->value[ 6 ] = 1;
                            break;
                        case 3:
                            obj->value[ 2 ] = 5;
                            obj->value[ 5 ] = 1;
                            break;
                        case 4:
                            obj->value[ 2 ] = 6;
                            obj->value[ 6 ] = 1;
                            break;
                    }
                    break;

                case 8:
                    name = "mizerykordia";
                    sh_desc = "mizerykordia";
                    name2 = "mizerykordi";
                    name3 = "mizerykordi";
                    name4 = "mizerykordie";
                    name5 = "mizerykordi±";
                    name6 = "mizerykordi";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 2;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 2;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 2;
                            obj->value[ 5 ] = 1;
                            break;
                        case 4:
                            obj->value[ 1 ] = 4;
                            obj->value[ 2 ] = 2;
                            obj->value[ 5 ] = 1;
                            obj->value[ 6 ] = 1;
                            break;
                    }

                    obj->value[ 3 ] = 1;
                    obj->weight = 15;
                    break;

                case 9:
                    name = "cienki sztylet";
                    sh_desc = "cienki sztylet";
                    name2 = "cienkiego sztyletu";
                    name3 = "cienkiemu sztyletowi";
                    name4 = "cienki sztylet";
                    name5 = "cienkim sztyletem";
                    name6 = "cienkim sztylecie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 2 ] = 5;
                            obj->value[ 5 ] = 1;
                            obj->value[ 6 ] = 1;
                            break;
                        case 4:
                            obj->value[ 2 ] = 6;
                            obj->value[ 6 ] = 1;
                            break;
                    }
                    break;

                case 10:
                    name = "lekki sztylet";
                    sh_desc = "lekki sztylet";
                    name2 = "lekkiego sztyletu";
                    name3 = "lekkiemu sztyletowi";
                    name4 = "lekki sztylet";
                    name5 = "lekkim sztyletem";
                    name6 = "lekkim sztylecie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 2 ] = 5;
                            obj->value[ 5 ] = 1;
                            obj->value[ 6 ] = 1;
                            break;
                        case 4:
                            obj->value[ 2 ] = 6;
                            obj->value[ 6 ] = 1;
                            break;
                    }

                    obj->weight = 15;
                    break;

                case 11:
                    name = "ostry pugina³";
                    sh_desc = "ostry pugina³";
                    name2 = "ostrego pugina³u";
                    name3 = "ostremu pugina³owi";
                    name4 = "ostry pugina³";
                    name5 = "ostrym pugina³em";
                    name6 = "ostrym puginale";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 6 ] = 1;
                            break;
                    }

                    obj->weight = 40;
                    break;

                case 12:
                    name = "stalowy tulich";
                    sh_desc = "stalowy tulich";
                    name2 = "stalowego tulicha";
                    name3 = "stalowemu tulichowi";
                    name4 = "stalowy tulich";
                    name5 = "stalowym tulichem";
                    name6 = "stalowym tulichu";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 2;
                            obj->value[ 6 ] = 1;
                            break;
                        case 3:
                            obj->value[ 2 ] = 8;
                            obj->value[ 5 ] = 1;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 6 ] = 1;
                            break;
                    }

                    obj->weight = 35;
                    break;

                case 13:
                    name = "d³ugi baselard";
                    sh_desc = "d³ugi baselard";
                    name2 = "dlugiego baselarda";
                    name3 = "dlugiemu baselardowi";
                    name4 = "d³ugi baselard";
                    name5 = "dlugim baselardem";
                    name6 = "dlugim baselardzie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 2 ] = 7;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 5 ] = 1;
                            break;
                    }

                    obj->weight = 50;
                    break;

                case 14:
                    name = "krótki baselard";
                    sh_desc = "krótki baselard";
                    name2 = "krótkiego baselarda";
                    name3 = "krótkiemu baselardowi";
                    name4 = "krótki baselard";
                    name5 = "krótkim baselardem";
                    name6 = "krótkim baselardzie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 2 ] = 5;
                            break;
                        case 4:
                            obj->value[ 2 ] = 6;
                            break;
                    }

                    obj->weight = 30;
                    break;

                case 15:
                    name = "ostry tulich";
                    sh_desc = "ostry tulich";
                    name2 = "ostrego tulicha";
                    name3 = "ostremu tulichowi";
                    name4 = "ostry tulich";
                    name5 = "ostrym tulichem";
                    name6 = "ostrym tulichu";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 6 ] = 1;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                    }
                    break;

                case 16:
                    name = "ostry pugina³";
                    sh_desc = "ostry pugina³";
                    name2 = "ostrego pugina³u";
                    name3 = "ostremu pugina³owi";
                    name4 = "ostry pugina³";
                    name5 = "ostrym pugina³em";
                    name6 = "ostrym puginale";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 2 ] = 5;
                            obj->value[ 5 ] = 1;
                            obj->value[ 6 ] = 1;
                            break;
                        case 4:
                            obj->value[ 2 ] = 6;
                            obj->value[ 6 ] = 1;
                            break;
                    }
                    break;

                case 17:
                    name = "zakrzywione wakizashi";
                    sh_desc = "zakrzywione wakizashi";
                    name2 = "zakrzywionego wakizashi";
                    name3 = "zakrzywionemu wakizashi";
                    name4 = "zakrzywione wakizashi";
                    name5 = "zakrzywionym wakizashi";
                    name6 = "zakrzywionym wakizashi";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 2 ] = 4;
                            obj->value[ 6 ] = 1;
                            break;
                        case 3:
                            obj->value[ 2 ] = 5;
                            obj->value[ 5 ] = 1;
                            break;
                        case 4:
                            obj->value[ 2 ] = 6;
                            obj->value[ 6 ] = 1;
                            break;
                    }
                    obj->value[ 3 ] = 2;
                    break;

                case 18:
                    name = "d³uga mizerykordia";
                    sh_desc = "d³uga mizerykordia";
                    name2 = "d³ugiej mizerykordi";
                    name3 = "d³ugiej mizerykordi";
                    name4 = "d³ug± mizerykordie";
                    name5 = "d³ug± mizerykordi±";
                    name6 = "d³ugiej mizerykordi";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            obj->value[ 5 ] = 1;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 5 ] = 1;
                            obj->value[ 6 ] = 1;
                            break;
                    }
                    obj->value[ 3 ] = 1;
                    obj->weight = 15;
                    break;

                case 19:
                    name = "cieñka cinquedea";
                    sh_desc = "cieñka cinquedea";
                    name2 = "cieñkiej cinquedei";
                    name3 = "cieñkiej cinquedei";
                    name4 = "cieñka cinquedea";
                    name5 = "cieñka cinquede±";
                    name6 = "cieñkiej cinquedei";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 5 ] = 1;
                            obj->value[ 6 ] = 1;
                            break;
                        case 4:
                            obj->value[ 1 ] = 4;
                            obj->value[ 2 ] = 2;
                            obj->value[ 6 ] = 1;
                            break;
                    }

                    obj->value[ 3 ] = 2;
                    break;

                case 20:
                    name = "lekka cinquedea";
                    sh_desc = "lekka cinquedea";
                    name2 = "lekkiej cinquedei";
                    name3 = "lekkiej cinquedei";
                    name4 = "lekka cinquedea";
                    name5 = "lekk± cinquede±";
                    name6 = "lekkiej cinquedei";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 2 ] = 5;
                            obj->value[ 5 ] = 1;
                            obj->value[ 6 ] = 1;
                            break;
                        case 4:
                            obj->value[ 2 ] = 6;
                            obj->value[ 6 ] = 1;
                            break;
                    }
                    obj->value[ 3 ] = 2;
                    obj->weight = 15;
                    break;

                case 21:
                    name = "falisty sztylet";
                    sh_desc = "falisty sztylet";
                    name2 = "falistego sztyletu";
                    name3 = "falistemu sztyletowi";
                    name4 = "falisty sztylet";
                    name5 = "falistym sztyletem";
                    name6 = "falistym sztylecie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 6;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    obj->weight = 35;
                    obj->value[ 5 ] = number_range( 0, 1 );
                    obj->value[ 6 ] = number_range( 0, 1 );
                    break;
            }

            break;

            /* WLOCZNIE */
        case WEAPON_SPEAR:
            obj->material = 2;
            obj->value[ 1 ] = 1;
            obj->value[ 2 ] = 2;
            obj->value[ 5 ] = 0;
            obj->value[ 6 ] = 0;
            obj->value[ 3 ] = 2;
            obj->weight = 80;

            switch ( number_range( 1, 8 ) )
            {
                case 1:
                    name = "w³ócznia";
                    sh_desc = "w³ócznia";
                    name2 = "w³óczni";
                    name3 = "w³óczni";
                    name4 = "w³ócznie";
                    name5 = "w³ócznia";
                    name6 = "w³ócznia";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 10;
                            obj->value[ 6 ] = 1;
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            obj->weight = 170;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            obj->weight = 180;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 7;
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            obj->weight = 200;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 8;
                            obj->value[ 5 ] = -1;
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            obj->weight = 370;
                            break;
                    }

                    break;
                case 2:
                    name = "oszczep";
                    sh_desc = "oszczep";
                    name2 = "oszczepu";
                    name3 = "oszczepowi";
                    name4 = "oszczep";
                    name5 = "oszczepem";
                    name6 = "oszczepie";

                    switch ( number_range( 1, UMAX( 1, wer - 1 ) ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 8;
                            obj->value[ 6 ] = 1;
                            break;
                        case 2:
                            obj->value[ 2 ] = 9;
                            obj->value[ 6 ] = 2;
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 5;
                            obj->value[ 5 ] = -1;
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            break;
                    }
                    break;
                case 3:
                    name = "d³uga w³ócznia";
                    sh_desc = "d³uga w³ócznia";
                    name2 = "d³ugiej w³óczni";
                    name3 = "d³ugiej w³óczni";
                    name4 = "d³ug± w³ócznie";
                    name5 = "d³ug± w³ócznia";
                    name6 = "d³uga w³ócznia";

                    switch ( number_range( 1, UMAX( 1, wer - 1 ) ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 10;
                            obj->value[ 6 ] = 2;
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            break;
                        case 2:
                            obj->value[ 2 ] = 9;
                            obj->value[ 6 ] = 3;
                            obj->value[ 5 ] = -1;
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            break;
                        case 3:
                            obj->value[ 2 ] = 12;
                            obj->value[ 5 ] = -1;
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            break;
                    }
                    break;
                case 4:
                    name = "d³ugi oszczep";
                    sh_desc = "d³ugi oszczep";
                    name2 = "d³ugiego oszczepu";
                    name3 = "d³ugiemu oszczepowi";
                    name4 = "d³ugi oszczep";
                    name5 = "d³ugim oszczepem";
                    name6 = "d³ugim oszczepie";

                    switch ( number_range( 1, UMAX( 1, wer - 1 ) ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 6 ] = 2;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 5;
                            obj->value[ 5 ] = -2;
                            obj->value[ 6 ] = number_range( 0, 1 );
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            break;
                    }
                    break;
                case 5:
                    name = "wid³y bojowe";
                    sh_desc = "wid³y bojowe";
                    name2 = "wide³ bojowych";
                    name3 = "wid³om bojowym";
                    name4 = "wid³y bojowe";
                    name5 = "wid³ami bojowymi";
                    name6 = "wid³ach bojowych";

                    obj->value[ 1 ] = number_range( 1, UMAX( 1, wer - 1 ) );
                    obj->value[ 2 ] = number_range( 3, wer + 2 );
                    obj->value[ 3 ] = 34;
                    obj->value[ 5 ] = -1;
                    obj->value[ 6 ] = 2;
                    obj->weight = number_range( 160, 360 );
                    SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                    break;
                case 6:
                    name = "zardzewia³y trójz±b";
                    sh_desc = "zardzewia³y trójz±b";
                    name2 = "zardzewia³ego trójzêbu";
                    name3 = "zardzewia³emu trójzêbowi";
                    name4 = "zardzewia³y trójz±b";
                    name5 = "zardzewia³ym trójzêbem";
                    name6 = "zardzewia³ym trójzêbie";

                    switch ( number_range( 1, UMAX( 1, wer - 1 ) ) )
                    {
                        case 1:
                            obj->value[ 2 ] = number_range( 6, 10 );
                            obj->value[ 6 ] = number_range( 0, 1 );
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = number_range( 2, 3 );
                            obj->value[ 2 ] = number_range( 4, 5 );
                            obj->value[ 5 ] = number_range( -2, 0 );
                            obj->value[ 6 ] = number_range( 0, 2 );
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            break;
                    }
                    obj->weight = number_range( 160, 370 );
                    break;
                case 7:
                    name = "stalowy harpun";
                    sh_desc = "stalowy harpun";
                    name2 = "stalowego harpuna";
                    name3 = "stalowemu harpunowi";
                    name4 = "stalowy harpun";
                    name5 = "stalowym harpunem";
                    name6 = "stalowym harpunie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = number_range( 5, 8 );
                            obj->value[ 6 ] = number_range( 0, 1 );
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            obj->value[ 6 ] = 1;
                            break;
                        case 3:
                            obj->value[ 1 ] = number_range( 2, 3 );
                            obj->value[ 2 ] = number_range( 2, 4 );
                            obj->value[ 5 ] = number_range( -2, 0 );
                            obj->value[ 6 ] = number_range( 0, 2 );
                            break;
                        case 4:
                            obj->value[ 1 ] = number_range( 2, 3 );
                            obj->value[ 2 ] = number_range( 2, 4 );
                            obj->value[ 6 ] = number_range( 0, 2 );
                            break;
                    }
                    obj->weight = number_range( 80, 180 );
                    obj->value[ 3 ] = 34;
                    break;
                case 8:
                    name = "ciê¿ka pika";
                    sh_desc = "ciê¿ka pika";
                    name2 = "ciê¿kiej piki";
                    name3 = "ciê¿kiej pice";
                    name4 = "ciê¿k± pikê";
                    name5 = "ciê¿k± pik±";
                    name6 = "ciê¿kiej pice";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = number_range( 7, 13 );
                            obj->value[ 5 ] = number_range( -1, 0 );
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 7;
                            obj->value[ 6 ] = 1;
                            break;
                        case 3:
                            obj->value[ 1 ] = number_range( 2, 3 );
                            obj->value[ 2 ] = number_range( 4, 6 );
                            obj->value[ 5 ] = number_range( -2, 0 );
                            obj->value[ 6 ] = number_range( 0, 2 );
                            break;
                        case 4:
                            obj->value[ 1 ] = number_range( 2, 3 );
                            obj->value[ 2 ] = number_range( 4, 6 );
                            obj->value[ 6 ] = number_range( 0, 2 );
                            break;
                    }
                    SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                    obj->weight = number_range( 300, 500 );
                    obj->value[ 3 ] = 15;
                    break;
            }
            break;

            /**
             * MACE
             */
        case WEAPON_MACE:

            obj->material = 1;

            obj->value[ 1 ] = 1;
            obj->value[ 2 ] = 2;
            obj->value[ 5 ] = 0;
            obj->value[ 6 ] = 0;

            switch ( number_range( 1, 14 ) )
            {
                case 1:
                    name = "maczuga";
                    sh_desc = "maczuga";
                    name2 = "maczugi";
                    name3 = "maczudze";
                    name4 = "maczugê";
                    name5 = "maczug±";
                    name6 = "maczudze";

                    obj->value[ 1 ] = 2;
                    obj->value[ 2 ] = 4;
                    obj->value[ 6 ] = 1;
                    obj->value[ 3 ] = 37;
                    obj->weight = 90;
                    break;
                case 2:
                    name = "m³ot bojowy";
                    sh_desc = "m³ot bojowy";
                    name2 = "m³ota bojowego";
                    name3 = "m³otowi bojowemu";
                    name4 = "m³ot bojowy";
                    name5 = "m³otem bojowym";
                    name6 = "m³ocie bojowym";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }

                    obj->value[ 3 ] = 8;
                    obj->weight = 160;
                    obj->material = 2;
                    break;

                case 3:
                    name = "buzdygan";
                    sh_desc = "buzdygan";
                    name2 = "buzdygana";
                    name3 = "buzdyganowi";
                    name4 = "buzdygan";
                    name5 = "buzdyganem";
                    name6 = "buzdyganie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 3;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 3;
                            obj->value[ 5 ] = 1;
                            break;
                    }

                    obj->value[ 3 ] = 8;
                    obj->weight = 160;
                    obj->material = 2;
                    break;
                case 4:
                    name = "ber³o";
                    sh_desc = "ber³o";
                    name2 = "ber³a";
                    name3 = "ber³u";
                    name4 = "ber³o";
                    name5 = "ber³em";
                    name6 = "berle";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            obj->value[ 6 ] = -1;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            obj->value[ 5 ] = -1;
                            break;
                    }

                    obj->value[ 3 ] = 8;
                    obj->weight = 110;
                    obj->material = 2;
                    break;
                case 5:
                    name = "wekiera";
                    sh_desc = "wekiera";
                    name2 = "wekiery";
                    name3 = "wekierze";
                    name4 = "wekierê";
                    name5 = "wekier±";
                    name6 = "wekierze";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }

                    obj->value[ 3 ] = 8;
                    obj->weight = 140;
                    obj->material = 2;
                    break;
                case 6:
                    name = "drewniana laga";
                    sh_desc = "drewniana laga";
                    name2 = "drewnianej lagi";
                    name3 = "drewnianej ladze";
                    name4 = "drewnian± lagê";
                    name5 = "drewnian± lag±";
                    name6 = "drewnianej ladze";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            obj->value[ 6 ] = 2;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }

                    obj->value[ 3 ] = 8;
                    obj->weight = 160;
                    obj->material = 1;
                    break;

                case 7:
                    name = "nadziak";
                    sh_desc = "nadziak";
                    name2 = "nadziaka";
                    name3 = "nadziakowi";
                    name4 = "nadziaka";
                    name5 = "nadziakiem";
                    name6 = "nadziaku";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            obj->value[ 6 ] = 1;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 5 ] = 1;
                            obj->value[ 6 ] = 1;
                            break;
                    }

                    obj->value[ 3 ] = 8;
                    obj->weight = 160;
                    obj->material = 2;
                    if ( number_range( 1, 3 ) == 1 ) SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                    break;

                case 8:
                    name = "obuch";
                    sh_desc = "obuch";
                    name2 = "obucha";
                    name3 = "obuchowi";
                    name4 = "obuch";
                    name5 = "obuchem";
                    name6 = "obuchu";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            obj->value[ 5 ] = -1;
                            obj->value[ 6 ] = -1;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            obj->value[ 5 ] = -1;
                            obj->value[ 6 ] = -1;
                            break;
                    }

                    obj->value[ 3 ] = 8;
                    obj->weight = 190;
                    obj->material = 2;
                    break;

                case 9:
                    name = "ogromna pa³a";
                    sh_desc = "ogromna pa³a";
                    name2 = "ogromnej pa³y";
                    name3 = "ogromnej pale";
                    name4 = "ogromn± pa³ê";
                    name5 = "ogromn± pa³±";
                    name6 = "ogromnej pale";
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            obj->value[ 5 ] = -1;
                            obj->value[ 6 ] = 2;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            obj->value[ 5 ] = -1;
                            obj->value[ 6 ] = -1;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 8;
                            obj->value[ 5 ] = -1;
                            obj->value[ 6 ] = -1;
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            break;
                    }

                    obj->value[ 3 ] = 37;
                    obj->weight = 120;
                    break;

                case 10:
                    name = "bu³awa";
                    sh_desc = "bu³awa";
                    name2 = "bu³awy";
                    name3 = "bu³awie";
                    name4 = "bu³awê";
                    name5 = "bu³aw±";
                    name6 = "bu³awie";

                    obj->material = 2;

                    obj->value[ 1 ] = 2;
                    obj->value[ 2 ] = 4;
                    obj->value[ 6 ] = 1;
                    obj->value[ 3 ] = 37;
                    obj->weight = 60;
                    break;

                case 11:
                    name = "morgensztern";
                    sh_desc = "morgensztern";
                    name2 = "morgenszterna";
                    name3 = "morgenszternowi";
                    name4 = "morgensztern";
                    name5 = "morgenszternem";
                    name6 = "morgenszternie";

                    obj->value[ 5 ] = -1;
                    obj->value[ 6 ] = 0;
                    obj->value[ 3 ] = 8;
                    SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                    obj->weight = 150;
                    obj->material = 2; //stal

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->value[ 3 ] = 8;
                    break;

                case 12:
                    name = "dwurêczny oksydowany m³ot";
                    sh_desc = "dwurêczny oksydowany m³ot";
                    name2 = "dwurêcznego oksydowanego m³ota";
                    name3 = "dwurêcznemu oksydowanemu m³otu";
                    name4 = "dwurêczny oksydowany m³ot";
                    name5 = "dwurêcznym oksydowanym m³otem";
                    name6 = "dwurêcznym oksydowanym m³ocie";
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 10;
                            obj->value[ 6 ] = 2;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 7;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 8;
                            obj->value[ 5 ] = -1;
                            break;
                        case 4:
                            obj->value[ 1 ] = 5;
                            obj->value[ 2 ] = 3;
                            obj->value[ 5 ] = -1;
                            break;
                    }
                    obj->value[ 3 ] = 8;
                    obj->weight = 380;
                    obj->material = 32;
                    SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                    break;
                case 13:
                    name = "monstrualny m³ot dwurêczny";
                    sh_desc = "monstrualny m³ot dwurêczny";
                    name2 = "monstrualnego m³ota dwurêcznego";
                    name3 = "monstrualnemu m³otu dwurêcznemu";
                    name4 = "monstrualny m³ot dwurêczny";
                    name5 = "monstrualnym m³otem dwurêcznym";
                    name6 = "monstrualnym m³ocie dwurêcznym";
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 6 ] = 2;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 7;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    obj->value[ 5 ] = number_range( 0, -2 );
                    obj->value[ 3 ] = 8;
                    obj->weight = 360;
                    SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                    break;
                case 14:
                    name = "ciê¿ki kamienny m³ot";
                    sh_desc = "ciê¿ki kamienny m³ot";
                    name2 = "ciê¿kiego kamiennego m³ota";
                    name3 = "ciê¿kiemu kamiennemu m³otu";
                    name4 = "ciê¿ki kamienny m³ot";
                    name5 = "ciê¿kim kamiennym m³otem";
                    name6 = "ciê¿kim kamiennym m³ocie";
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            obj->value[ 6 ] = 1;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->value[ 5 ] = number_range( -1, 0 );
                    obj->value[ 6 ] = number_range( 0, 1 );
                    obj->value[ 3 ] = 8;
                    obj->weight = 380;
                    obj->material = 16;
                    break;

            }
            break;



            /* TOPORY */
        case WEAPON_AXE:

            obj->material = 2;
            obj->value[ 3 ] = 25; // damtype "ciêcie"
            obj->value[ 1 ] = 2;
            obj->value[ 2 ] = 2;
            obj->value[ 5 ] = 0;
            obj->value[ 6 ] = 0;

            switch ( number_range( 1, 19 ) )
            {
                case 1:
                    name = "topór";
                    sh_desc = "topór";
                    name2 = "topora";
                    name3 = "toporowi";
                    name4 = "topór";
                    name5 = "toporem";
                    name6 = "toporze";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                    }
                    obj->value[ 3 ] = 25;
                    obj->weight = 110;
                    break;
                case 2:
                    name = "siekiera";
                    sh_desc = "siekiera";
                    name2 = "siekiery";
                    name3 = "siekierze";
                    name4 = "siekierê";
                    name5 = "siekier±";
                    name6 = "siekierze";

                    obj->value[ 1 ] = 2;
                    obj->value[ 2 ] = number_range(3,4);
                    obj->value[ 3 ] = 25;
                    obj->value[ 5 ] = -1;
                    obj->weight = 110;
                    break;
                case 3:
                    name = "topór bojowy";
                    sh_desc = "topór bojowy";
                    name2 = "topora bojowego";
                    name3 = "toporowi bojowemu";
                    name4 = "topór bojowy";
                    name5 = "toporem bojowym";
                    name6 = "toporze bojowym";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            obj->value[ 6 ] = 1;
                            break;
                    }

                    obj->value[ 3 ] = 3;
                    if ( number_range( 1, 2 ) == 2 ) SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                    obj->weight = 190;
                    break;
                case 4:
                    name = "dwurêczny topór";
                    sh_desc = "dwurêczny topór";
                    name2 = "dwurêcznego topora";
                    name3 = "dwurêcznemu toporowi";
                    name4 = "dwurêczny topór";
                    name5 = "dwurêcznym toporem";
                    name6 = "dwurêcznym toporze";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 6;
                            break;
                        case 2:
                            obj->value[ 2 ] = 7;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 5;
                            break;
                        case 4:
                            obj->value[ 1 ] = 4;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->value[ 5 ] = -1;
                    obj->value[ 6 ] = 1;
                    obj->value[ 3 ] = 3;
                    SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                    obj->weight = 320;
                    break;

                case 5:
                    name = "ciê¿ki dwurêczny topór";
                    sh_desc = "ciê¿ki dwurêczny topór";
                    name2 = "ciê¿kiego dwurêcznego topora";
                    name3 = "ciê¿kiemu dwurêcznemu toporowi";
                    name4 = "ciê¿ki dwurêczny topór";
                    name5 = "ciê¿kim dwurêcznym toporem";
                    name6 = "ciê¿kim dwurêcznym toporze";
                    obj->value[ 1 ] = number_range( 1, 3 );
                    obj->value[ 2 ] = UMIN( 6, 2 + number_range( 1, wer ) );
                    obj->value[ 5 ] = -2;
                    obj->value[ 6 ] = 2;
                    SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                    obj->weight = number_range( 250, 425 );
                    break;

                case 6:
                    name = "czarny topór";
                    sh_desc = "czarny topór";
                    name2 = "czarnego topora";
                    name3 = "czarnemu toporowi";
                    name4 = "czarny topór";
                    name5 = "czarnym toporem";
                    name6 = "czarnym toporze";
                    obj->weight = number_range( 110, 210 );
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 4:
                            obj->value[ 2 ] = 8;
                            if ( number_percent() > 50 )
                                SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                            break;
                    }
                    if ( number_percent() > 50 )
                        obj->value[ 6 ] = 1;
                    if ( number_percent() > 50 )
                        obj->value[ 5 ] = -1;
                    break;

                case 7:
                    name = "krasnoludzki topór";
                    sh_desc = "krasnoludzki topór";
                    name2 = "krasnoludzkiego topora";
                    name3 = "krasnoludzkiemu toporowi";
                    name4 = "krasnoludzki topór";
                    name5 = "krasnoludzkim toporem";
                    name6 = "krasnoludzkim toporze";
                    obj->value[ 1 ] = 1;
                    obj->weight = number_range( 110, 210 );
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 6;
                            break;
                        case 2:
                            obj->value[ 2 ] = 7;
                            break;
                        case 3:
                            obj->value[ 2 ] = 8;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    if ( number_percent() > 50 )
                    {
                        SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                        obj->value[ 5 ] = 1;
                        obj->value[ 6 ] = 1;
                    }
                    else
                        obj->value[ 5 ] = -1;
                    break;

                case 8:
                    name = "pó³kolisty topór";
                    sh_desc = "pó³kolisty topór";
                    name2 = "pólkolistego topora";
                    name3 = "pó³kolistemu toporowi";
                    name4 = "pó³kolisty topór";
                    name5 = "pó³kolistym toporem";
                    name6 = "pó³kolistym toporze";
                    obj->weight = number_range( 110, 210 );
                    obj->value[ 2 ] = UMIN( 6, 2 + number_range( 1, wer ) );
                    if ( number_percent() > 50 )
                    {
                        SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                        obj->value[ 5 ] = 1;
                    }
                    else if ( number_percent() > 50 )
                        obj->value[ 6 ] = -1;
                    break;

                case 9:
                    name = "porêczny topór";
                    sh_desc = "porêczny topór";
                    name2 = "porêcznego topora";
                    name3 = "porêcznemu toporowi";
                    name4 = "porêczny topór";
                    name5 = "porêcznym toporem";
                    name6 = "porêcznym toporze";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                        case 2:
                            obj->value[ 2 ] = 3;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    if ( number_percent() > 50 )
                    {
                        SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                        obj->value[ 6 ] = 1;
                    }
                    obj->value[ 5 ] = 1;
                    obj->value[ 3 ] = 41;
                    obj->weight = number_range( 80, 160 );
                    break;

                case 10:
                    name = "stalowy topór";
                    sh_desc = "stalowy topór";
                    name2 = "stalowego topora";
                    name3 = "stalowemu toporowi";
                    name4 = "stalowy topór";
                    name5 = "stalowym toporem";
                    name6 = "stalowym toporze";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 2 ] = 5;
                            break;
                        case 4:
                            obj->value[ 2 ] = 6;
                            break;
                    }
                    if ( number_percent() > 50 )
                    {
                        SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                        obj->value[ 5 ] = 1;
                    }
                    obj->weight = number_range( 110, 210 );
                    break;

                case 11:
                    name = "ogromny dwurêczny topór";
                    sh_desc = "ogromny dwurêczny topór";
                    name2 = "ogromego dwurêcznego topora";
                    name3 = "ogromnemu dwurêcznemu toporowi";
                    name4 = "ogromny dwurêczny topór";
                    name5 = "ogromnym dwurêcznym toporem";
                    name6 = "ogromnym dwurêcznym toporze";
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 7;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 8;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    obj->value[ 5 ] = number_range(-2,-1);
                    obj->value[ 6 ] = number_range(1,2);
                    SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                    obj->weight = 300 + number_range( 0, 200 );
                    break;
                case 12:
                    name = "masywny dwurêczny topór";
                    sh_desc = "masywny dwurêczny topór";
                    name2 = "masywnego dwurêcznego topora";
                    name3 = "masywnemu dwurêcznemu toporowi";
                    name4 = "masywny dwurêczny topór";
                    name5 = "masywnym dwurêcznym toporem";
                    name6 = "masywnym dwurêcznym toporze";
                    obj->value[ 6 ] = 1;
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 7;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    obj->value[ 5 ] = -1;
                    SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                    obj->weight = 180 + number_range( 0, 180 );
                    break;
                case 13:
                    name = "siekierka";
                    sh_desc = "siekierka";
                    name2 = "siekierki";
                    name3 = "siekierce";
                    name4 = "siekierkê";
                    name5 = "siekierk±";
                    name6 = "siekierce";

                    obj->value[ 1 ] = number_range( 1, 2 );
                    obj->value[ 3 ] = 25;
                    obj->value[ 2 ] = UMIN( 5, 1 + number_range( 1, wer ) );
                    obj->weight = number_range( 80, 160 );
                    break;
                case 14:
                    name = "prosta siekiera";
                    sh_desc = "prosta siekiera";
                    name2 = "prostej siekiery";
                    name3 = "prostej siekierze";
                    name4 = "prost± siekierê";
                    name5 = "prost± siekier±";
                    name6 = "prostej siekierze";

                    obj->value[ 3 ] = 25;
                    obj->value[ 1 ] = 1;
                    obj->value[ 2 ] = UMIN( 9, 1 + 2 * number_range( 1, wer ) );
                    obj->weight = number_range( 80, 160 );
                    break;

                case 15:

                    if ( number_percent() > 50 )
                    {
                        name = "toporna siekiera";
                        sh_desc = "toporna siekiera";
                        name2 = "topornej siekiery";
                        name3 = "topornej siekierze";
                        name4 = "toporn± siekierê";
                        name5 = "toporn± siekier±";
                        name6 = "topornej siekierze";
                    }
                    else
                    {
                        name = "siekiera bojowa";
                        sh_desc = "siekiera bojowa";
                        name2 = "siekiery bojowej";
                        name3 = "siekierze bojowej";
                        name4 = "siekierê bojow±";
                        name5 = "siekier± bojow±";
                        name6 = "siekierze bojowej";
                    }
                    obj->value[ 1 ] = number_range( 1, 2 );
                    if ( number_percent() > 50 )
                    {
                        SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                        obj->value[ 6 ] = number_range( 1, 2 );
                    }
                    obj->value[ 3 ] = 25;
                    obj->value[ 2 ] = UMIN( 6, 2 + number_range( 1, wer ) );
                    obj->weight = number_range( 110, 210 );
                    break;

                case 16:
                    name = "posrebrzany pó³kolisty topór";
                    sh_desc = "posrebrzany pó³kolisty topór";
                    name2 = "posrebrzanego pó³kolistego topora";
                    name3 = "posrebrzanemu pó³kolistemu toporowi";
                    name4 = "posrebrzany pó³kolisty topór";
                    name5 = "posrebrzanym pó³kolistym toporem";
                    name6 = "posrebrzanym pó³kolistym toporze";
                    obj->weight = number_range( 110, 210 );
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    if ( number_percent() > 50 )
                    {
                        SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                        obj->value[ 5 ] = 1;
                        obj->value[ 6 ] = 1;
                    }
                    break;
                case 17:
                    name = "czarny runiczny topór";
                    sh_desc = "czarny runiczny topór";
                    name2 = "czarnego runicznego topora";
                    name3 = "czarnemu runicznemu toporowi";
                    name4 = "czarny runiczny topór";
                    name5 = "czarnym runicznym toporem";
                    name6 = "czarnym runicznym toporze";
                    EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );
                    obj->weight = number_range( 110, 210 );
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 7;
                            break;
                        case 2:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 8;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    if ( number_percent() > 50 )
                    {
                        SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                        obj->value[ 5 ] = number_range( 1, 2 );
                        obj->value[ 6 ] = number_range( 1, 2 );
                    }
                    else
                    {
                        obj->value[ 5 ] = 1;
                        obj->value[ 6 ] = 1;
                    }
                    break;
                case 18:
                    name = "wielki topór orków";
                    sh_desc = "wielki topór orków";
                    name2 = "wielkiego topora orków";
                    name3 = "wielkiemu toporowi orków";
                    name4 = "wielki topór orków";
                    name5 = "wielkim toporem orków";
                    name6 = "wielkim toporze orków";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 7;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    obj->value[ 6 ] = number_range(1,2);
                    obj->value[ 3 ] = 25;
                    SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                    EXT_SET_BIT( obj->wear_flags2, ITEM_ONLYHALFORC );
                    obj->weight = number_range( 320, 400 );
                    break;
                case 19:
                    name = "masywny labrys";
                    sh_desc = "masywny labrys";
                    name2 = "masywnego labrysa";
                    name3 = "masywnemu labrysowi";
                    name4 = "masywny labrys";
                    name5 = "masywnym labrysem";
                    name6 = "masywnym labrysie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 10;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 7;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 8;
                            break;
                    }
                    obj->value[ 5 ] = number_range( -2, 1 );
                    obj->value[ 6 ] = number_range( 0, 3 );
                    obj->value[ 3 ] = 25;
                    obj->material = 16;
                    SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                    obj->weight = number_range( 300, 500 );
                    break;
            }
            break;
            /**
             * FLAILE - czyli cepy
             */
        case WEAPON_FLAIL:

            obj->material = 2;

            obj->value[ 1 ] = 1;
            obj->value[ 2 ] = 2;
            obj->value[ 5 ] = 0;
            obj->value[ 6 ] = 0;

            switch ( number_range( 1, 10 ) )
            {
                case 1:
                    name = "korbacz";
                    sh_desc = "korbacz";
                    name2 = "korbacza";
                    name3 = "korbaczowi";
                    name4 = "korbacz";
                    name5 = "korbaczem";
                    name6 = "korbaczu";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 6;
                            break;
                        case 2:
                            obj->value[ 2 ] = 7;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->value[ 3 ] = 8;
                    if ( number_range( 1, 3 ) != 1 ) SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                    obj->weight = 130;
                    break;

                case 2:
                    name = "cep bojowy";
                    sh_desc = "cep bojowy";
                    name2 = "cepa bojowego";
                    name3 = "cepowi bojowemu";
                    name4 = "cep bojowy";
                    name5 = "cepem bojowym";
                    name6 = "cepie bojowym";

                    obj->value[ 6 ] = 1;
                    obj->value[ 3 ] = 8;
                    if ( number_range( 1, 3 ) != 1 ) SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                    obj->weight = 90;

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            obj->value[ 5 ] = number_range( -1, 0 );
                            break;
                        case 4:
                            obj->value[ 2 ] = 10;
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            break;
                    }
                    break;

                case 3:
                    name = "ciê¿ki cep";
                    sh_desc = "ciê¿ki cep";
                    name2 = "ciê¿kiego cepa";
                    name3 = "ciê¿kiemu cepowi";
                    name4 = "ciê¿ki cep";
                    name5 = "ciê¿kim cepem";
                    name6 = "ciê¿kim cepie";

                    obj->value[ 5 ] = -1;
                    obj->value[ 6 ] = 1;
                    obj->value[ 3 ] = 37;
                    if ( number_range( 1, 4 ) != 1 )
                        SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                    else
                        SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );

                    obj->weight = number_range( 325, 420 );

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 8;
                            obj->value[ 6 ] = 1;
                            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                            break;
                    }
                    break;

                case 4:
                    name = "¿elazny korbacz";
                    sh_desc = "¿elazny korbacz";
                    name2 = "¿elaznego korbacza";
                    name3 = "¿elaznemu korbaczowi";
                    name4 = "¿elazny korbacz";
                    name5 = "¿elaznym korbaczem";
                    name6 = "¿elaznym korbaczu";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 7;
                            break;
                        case 2:
                            obj->value[ 2 ] = 8;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->value[ 3 ] = 37;
                    if ( number_range( 1, 3 ) == 1 )
                        SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                    else if ( number_range( 1, 3 ) == 1 )
                        SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );

                    obj->weight = 110;
                    obj->material = 13;
                    break;

                case 5:
                    name = "mithrilowy cep";
                    sh_desc = "mithrilowy cep";
                    name2 = "mithrilowego cepa";
                    name3 = "mithrilowemu cepowi";
                    name4 = "mithrilowy cep";
                    name5 = "mithrilowym cepem";
                    name6 = "mithrilowym cepie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            obj->value[ 6 ] = 1;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 6 ] = 1;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            obj->value[ 5 ] = -1;
                            break;
                    }
                    obj->value[ 3 ] = 37;
                    if ( number_range( 1, 3 ) == 1 )
                        SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                    else if ( number_range( 1, 3 ) == 1 )
                        SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );

                    obj->weight = 75;
                    obj->material = 8;
                    obj->cost += 150;
                    break;

                case 6:
                    name = "mocarny cep";
                    sh_desc = "mocarny cep";
                    name2 = "mocarnego cepa";
                    name3 = "mocarnemu cepowi";
                    name4 = "mocarny cep";
                    name5 = "mocarnym cepem";
                    name6 = "mocarnym cepie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            obj->value[ 6 ] = 1;
                            break;
                        case 2:
                            obj->value[ 1 ] = 4;
                            obj->value[ 2 ] = 3;
                            break;
                        case 3:
                            obj->value[ 1 ] = 4;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    obj->value[ 3 ] = 37;
                    SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
                    obj->weight = 180;
                    obj->material = number_range( 2, 5 );
                    break;

                case 7:
                    name = "æwiekowany korbacz";
                    sh_desc = "æwiekowany korbacz";
                    name2 = "æwiekowanego korbacza";
                    name3 = "æwiekowanemu korbaczowi";
                    name4 = "æwiekowany korbacz";
                    name5 = "æwiekowanym korbaczem";
                    name6 = "æwiekowanym korbaczu";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            obj->value[ 6 ] = 1;
                            break;
                        case 2:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 3;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                    }
                    obj->value[ 3 ] = 37;
                    if ( number_range( 1, 3 ) == 1 )
                        SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                    else if ( number_range( 1, 3 ) == 1 )
                        SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );

                    obj->weight = 180;
                    obj->material = number_range( 2, 5 );
                    break;

                case 8:
                    name = "porêczny ki¶cieñ";
                    sh_desc = "porêczny ki¶cieñ";
                    name2 = "porêcznego ki¶cienia";
                    name3 = "porêcznemu ki¶cieniowi";
                    name4 = "porêczny ki¶cieñ";
                    name5 = "porêcznym ki¶cieniem";
                    name6 = "porêcznym ki¶cieniu";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            obj->value[ 6 ] = 1;
                            break;
                        case 2:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 3;
                            obj->value[ 6 ] = 1;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 6 ] = 1;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                    }
                    obj->value[ 3 ] = 4;
                    if ( number_range( 1, 4 ) == 1 )
                        SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );

                    obj->value[ 5 ] += 2;
                    obj->weight = 100;
                    obj->material = number_range( 2, 5 );
                    break;

                case 9:
                    name = "gladiatorski ki¶cieñ";
                    sh_desc = "gladiatorski ki¶cieñ";
                    name2 = "gladiatorskiego ki¶cienia";
                    name3 = "gladiatorskiemu ki¶cieniowi";
                    name4 = "gladiatorski ki¶cieñ";
                    name5 = "gladiatorskim ki¶cieniem";
                    name6 = "gladiatorskim ki¶cieniu";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            obj->value[ 6 ] = 2;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 6 ] = 1;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            obj->value[ 6 ] = 2;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->value[ 3 ] = 4;
                    if ( number_range( 1, 4 ) == 1 )
                        SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );

                    obj->weight = number_range( 170, 220 );
                    obj->material = 32;
                    obj->cost += 80;
                    break;
                case 10:
                    name = "korbacz z ko¶ci";
                    sh_desc = "korbacz z ko¶ci";
                    name2 = "korbacza z ko¶ci";
                    name3 = "korbaczu z ko¶ci";
                    name4 = "korbacz z ko¶ci";
                    name5 = "korbaczem z ko¶ci";
                    name6 = "korbaczu z ko¶ci";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 2;
                            break;
                        case 2:
                            obj->value[ 2 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    obj->value[ 5 ] += number_range( 0, 3 );
                    obj->value[ 6 ] += number_range( 0, 3 );
                    obj->value[ 3 ] = 8;
                    obj->material = 21;
                    if ( number_range( 1, 3 ) != 1 ) SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                    obj->weight = number_range( 130, 190 );
                    break;
            }
            break;

            /* BATY (WEAPON_WHIP) */
        case WEAPON_WHIP:
            obj->value[ 3 ] = 4; // dam_type "smagniêcie"
            obj->material = 44; // material "mocna skóra"

            switch ( number_range( 1, 13 ) )
            {
                case 1:
                    name = "d³ugi nahaj";
                    sh_desc = "d³ugi nahaj";
                    name2 = "dlugiego nahaja";
                    name3 = "dlugiemu nahajowi";
                    name4 = "d³ugi nahaj";
                    name5 = "dlugim nahajem";
                    name6 = "dlugim nahaju";

                    /* 4 typy obrazen */
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 3;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->weight = 25;
                    obj->value[ 5 ] = number_range( -1, 1 );
                    obj->value[ 6 ] = number_range( -1, 1 );
                    break;

                case 2:
                    name = "bat";
                    sh_desc = "bat";
                    name2 = "bata";
                    name3 = "batu";
                    name4 = "bat";
                    name5 = "batem";
                    name6 = "bacie";

                    /* 4 typy obrazen */
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 2;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 3;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->weight = 20;
                    obj->value[ 5 ] = number_range( -1, 1 );
                    obj->value[ 6 ] = number_range( -1, 1 );
                    break;

                case 3:
                    name = "krótki bicz";
                    sh_desc = "krótki bicz";
                    name2 = "krótkiego bicza";
                    name3 = "krótkiemu biczowi";
                    name4 = "krótki bicz";
                    name5 = "krótkim biczem";
                    name6 = "krótkim biczu";

                    /* 4 typy obrazen */
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    obj->weight = 15;
                    obj->value[ 5 ] = number_range( -1, 0 );
                    obj->value[ 6 ] = number_range( -1, 1 );
                    break;

                case 4:
                    name = "kañczug";
                    sh_desc = "kañczug";
                    name2 = "kañczuga";
                    name3 = "kañczugowi";
                    name4 = "kañczug";
                    name5 = "kañczugiem";
                    name6 = "kañczugu";

                    /* 4 typy obrazen */
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                    }
                    obj->weight = 25;
                    obj->value[ 5 ] = number_range( 0, 2 );
                    obj->value[ 6 ] = number_range( -1, 1 );
                    break;

                case 5:
                    name = "krótki bat";
                    sh_desc = "krótki bat";
                    name2 = "krótkiego bata";
                    name3 = "krótkiemu batu";
                    name4 = "krótki bat";
                    name5 = "krótkim batem";
                    name6 = "krótkim bacie";

                    /* 4 typy obrazen */
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 7;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    obj->weight = 18;
                    obj->value[ 5 ] = number_range( -1, 2 );
                    obj->value[ 6 ] = number_range( -1, 1 );
                    break;

                case 6:
                    name = "d³ugi bicz";
                    sh_desc = "d³ugi bicz";
                    name2 = "d³ugiego bicza";
                    name3 = "d³ugiemu biczowi";
                    name4 = "d³ugi bicz";
                    name5 = "d³ugim biczem";
                    name6 = "d³ugim biczu";

                    /* 4 typy obrazen */
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->weight = 30;
                    obj->value[ 5 ] = number_range( 0, 2 );
                    obj->value[ 6 ] = number_range( 0, 2 );
                    break;

                case 7:
                    name = "d³ugi bat";
                    sh_desc = "d³ugi bat";
                    name2 = "d³ugiego bata";
                    name3 = "d³ugiemu batu";
                    name4 = "d³ugi bat";
                    name5 = "d³ugim batem";
                    name6 = "d³ugim bacie";

                    /* 4 typy obrazen */
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 3:
                            obj->value[ 1 ] = 4;
                            obj->value[ 2 ] = 3;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                    }
                    obj->weight = 30;
                    break;

                case 8:
                    name = "krótki batog";
                    sh_desc = "krótki batog";
                    name2 = "krótkiego batoga";
                    name3 = "krótkiemu batogowi";
                    name4 = "krótki batog";
                    name5 = "krótkim batogiem";
                    name6 = "krótkim batogu";

                    /* 4 typy obrazen */
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    obj->weight = 20;
                    obj->value[ 5 ] = number_range( -1, 0 );
                    obj->value[ 6 ] = number_range( -1, 0 );
                    break;

                case 9:
                    name = "krótki nahaj";
                    sh_desc = "krótki nahaj";
                    name2 = "krótkiego nahaja";
                    name3 = "krótkiemu nahajowi";
                    name4 = "krótki nahaj";
                    name5 = "krótkim nahajem";
                    name6 = "krótkim nahaju";

                    /* 4 typy obrazen */
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 3;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->weight = 10;
                    obj->value[ 5 ] = number_range( -1, 0 );
                    obj->value[ 6 ] = number_range( -1, 1 );
                    break;

                case 10:
                    name = "skórzany bicz";
                    sh_desc = "skórzany bicz";
                    name2 = "skórzanego bicza";
                    name3 = "skórzanemu biczowi";
                    name4 = "skórzany bicz";
                    name5 = "skórzanym biczem";
                    name6 = "skórzanym biczu";

                    /* 4 typy obrazen */
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 3;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->weight = 25;
                    break;

                case 11:
                    name = "mocny bat";
                    sh_desc = "mocny bat";
                    name2 = "mocnego bata";
                    name3 = "mocnemu batu";
                    name4 = "mocny bat";
                    name5 = "mocnym batem";
                    name6 = "mocnym bacie";

                    /* 4 typy obrazen */
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->weight = 35;
                    obj->value[ 5 ] = number_range( 0, 1 );
                    obj->value[ 6 ] = number_range( -1, 1 );
                    break;

                case 12:
                    name = "kunsztowny bicz";
                    sh_desc = "kunsztowny bicz";
                    name2 = "kunsztownego bicza";
                    name3 = "kunsztownemu biczowi";
                    name4 = "kunsztowny bicz";
                    name5 = "kunsztownym biczem";
                    name6 = "kunsztownym biczu";

                    /* 4 typy obrazen */
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->weight = 25;
                    obj->value[ 5 ] = number_range( -1, 0 );
                    obj->value[ 6 ] = number_range( -1, 0 );
                    break;

                case 13:
                    name = "prosty bicz";
                    sh_desc = "prosty bicz";
                    name2 = "prostego bicza";
                    name3 = "prostemu biczowi";
                    name4 = "prosty bicz";
                    name5 = "prostym biczem";
                    name6 = "prostym biczu";

                    /* 4 typy obrazen */
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 2;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 3;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 3;
                            break;
                    }
                    obj->weight = 25;
                    obj->value[ 5 ] = number_range( -1, 1 );
                    obj->value[ 6 ] = number_range( -1, 1 );
                    break;
            }
            break;

            /**
             * bronie drzewcowe
             * POLEARMY
             *
             * WEAPON_POLEARM
             */

        case WEAPON_POLEARM:

            obj->material = 2;
            obj->value[ 1 ] = 2;
            obj->value[ 2 ] = 2;
            obj->value[ 3 ] = 2; // damtype "pchniecie"
            obj->value[ 5 ] = -1;
            obj->value[ 6 ] = 2;

            obj->weight = 150 + number_range( 0, 50 );
            /**
             * WEAPON_POLEARM ma byc zawsze dwureczny
             */
            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );

            switch ( number_range( 1, 12 ) )
            {
                case 1:
                    name = "halabarda";
                    sh_desc = "halabarda";
                    name2 = "halabardy";
                    name3 = "halabardzie";
                    name4 = "halabarde";
                    name5 = "halabarda";
                    name6 = "halabardzie";
                    obj->value[ 2 ] = number_range(6,8);
                    obj->value[ 3 ] = 42;
                    obj->value[ 5 ] = number_range(-3,0);
                    obj->value[ 6 ] = 2;
                    obj->weight = 210;
                    break;
                case 2:
                    name = "berdysz";
                    sh_desc = "berdysz";
                    name2 = "berdysza";
                    name3 = "berdyszowi";
                    name4 = "berdysz";
                    name5 = "berdyszem";
                    name6 = "berdyszu";
                    if ( number_range( 1, 4 ) == 1 ) ++obj->value[ 1 ];
                    obj->value[ 2 ] = 7;
                    obj->value[ 3 ] = 25;
                    obj->value[ 5 ] = number_range(-3,1);
                    obj->value[ 6 ] = number_range(0,2);
                    obj->weight = 200;
                    break;
                case 3:
                    name = "lekki berdysz";
                    sh_desc = "lekki berdysz";
                    name2 = "lekkiego berdysza";
                    name3 = "lekkiemu berdyszowi";
                    name4 = "lekki berdysz";
                    name5 = "lekkim berdyszem";
                    name6 = "lekkim berdyszu";
                    obj->value[ 2 ] = number_range( 3, 5 );
                    obj->value[ 6 ] = number_range( 0, 1 );
                    if ( number_range( 1, 3 ) != 1 )
                    {
                        obj->value[ 5 ] = number_range( 0, 1 );
                    }
                    else
                    {
                        obj->value[ 2 ] = number_range( 3, 6 );
                        obj->value[ 5 ] = number_range( 0, 1 );
                    }
                    obj->value[ 3 ] = 25;
                    obj->weight = 180;
                    break;
                case 4:
                    name = "kosa bojowa";
                    sh_desc = "kosa bojowa";
                    name2 = "kosy bojowej";
                    name3 = "kosie bojowej";
                    name4 = "kosê bojow±";
                    name5 = "kosa bojowa";
                    name6 = "kosie bojowej";
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 10;
                            break;
                        case 2:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 12;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 7;
                            break;
                    }
                    obj->value[ 3 ] = 21;
                    obj->value[ 5 ] = number_range( -2, 0 );
                    obj->value[ 6 ] = 2;
                    obj->weight = 200;
                    break;

                case 5:
                    name = "partyzana";
                    sh_desc = "partyzana";
                    name2 = "partyzany";
                    name3 = "partyzanie";
                    name4 = "partyzanê";
                    name5 = "partyzan±";
                    name6 = "partyzanie";
                    switch ( number_range( 1, 5 ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 8;
                            break;
                        case 2:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 9;
                            break;
                        case 3:
                            obj->value[ 2 ] = 6;
                            break;
                        case 4:
                            obj->value[ 2 ] = 7;
                            break;
                        case 5:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    obj->weight = 190 + number_range( 0, 50 );
                    break;

                case 6:
                    name = "glewia";
                    sh_desc = "glewia";
                    name2 = "glewi";
                    name3 = "glewi";
                    name4 = "glewiê";
                    name5 = "glewi±";
                    name6 = "glewi";
                    obj->weight = 170 + number_range( 0, 40 );
                    obj->value[ 1 ] = number_range( 1, 3 );
                    obj->value[ 2 ] = UMIN( 6, 2 + number_range( 1, wer ) );
                    break;

                case 7:
                    name = "gizarma";
                    sh_desc = "gizarma";
                    name2 = "gizarmy";
                    name3 = "gizarmie";
                    name4 = "gizarmê";
                    name5 = "gizarm±";
                    name6 = "gizarmie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 6;
                            break;
                        case 2:
                        case 3:
                        case 4:
                            obj->value[ 2 ] = 2 + number_range( 1, 6 );
                            break;
                    }
                    if ( number_range( 1, 4 ) == 1 ) ++obj->value[ 1 ];
                    break;

                case 8:
                    name = "naginata";
                    sh_desc = "naginata";
                    name2 = "naginaty";
                    name3 = "naginacie";
                    name4 = "naginatê";
                    name5 = "naginat±";
                    name6 = "naginacie";
                    obj->value[ 3 ] = 2;
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 7;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 8;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    ++obj->value[ 5 ];
                    break;

                case 9:
                    name = "rohatyna";
                    sh_desc = "rohatyna";
                    name2 = "rohatyny";
                    name3 = "rohatynie";
                    name4 = "rohatynê";
                    name5 = "rohatyn±";
                    name6 = "rohatynie";

                    obj->value[ 2 ] = 4;
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    obj->weight = number_range( 70, 250 );
                    obj->value[ 3 ] = 34;
                    break;

                case 10:
                    name = "wyszczerbiona kosa";
                    sh_desc = "wyszczerbiona kosa";
                    name2 = "wyszczerbionej kosy";
                    name3 = "wyszczerbionej kosie";
                    name4 = "wyszczerbion± kosê";
                    name5 = "wyszczerbion± kos±";
                    name6 = "wyszczerbionej kosie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 7;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                    }

                    obj->value[ 3 ] = 21;
                    obj->value[ 5 ] = number_range( -2, 0 );
                    obj->value[ 6 ] = number_range( -1, 1 );;
                    obj->weight = 200;
                    break;

                case 11:
                    name = "lekka kosa";
                    sh_desc = "lekka kosa";
                    name2 = "lekkiej kosy";
                    name3 = "lekkiej kosie";
                    name4 = "lekk± kosê";
                    name5 = "lekk± kos±";
                    name6 = "lekkiej kosie";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 4;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                    }
                    obj->value[ 3 ] = 21;
                    obj->value[ 5 ] = number_range( 0, 1 );
                    obj->value[ 6 ] = number_range( 0, 1 );;
                    obj->weight = 180;
                    break;

                case 12:
                    name = "cep bojowy";
                    sh_desc = "cep bojowy";
                    name2 = "cepa bojowego";
                    name3 = "cepie bojowemu";
                    name4 = "cep bojowy";
                    name5 = "cepem bojowym";
                    name6 = "cepie bojowym";
                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 11;
                            break;
                        case 2:
                            obj->value[ 1 ] = 1;
                            obj->value[ 2 ] = 13;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 7;
                            break;
                        case 4:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 8;
                            break;
                    }
                    obj->value[ 3 ] = 9; // mwalniecie
                    obj->value[ 5 ] = number_range( -4, -2 );
                    obj->value[ 6 ] = number_range( 1, 4 );
                    obj->weight = 250;
                    break;
            }
            break;

            /*
             * kostury (laski)
             * STAFF
             *
             * WEAPON_STAFF
             */

        case WEAPON_STAFF:
            obj->material = 1;
            SET_BIT( obj->value[ 4 ], WEAPON_TWO_HANDS );
            obj->value[ 1 ] = 1;
            obj->value[ 2 ] = 2;
            obj->value[ 5 ] = number_range( 0, 1 );
            obj->value[ 6 ] = number_range( 0, 1 );

            switch ( number_range( 1, 5 ) )
            {
                case 1:
                    name = "kij";
                    sh_desc = "kij";
                    name2 = "kija";
                    name3 = "kijowi";
                    name4 = "kij";
                    name5 = "kijem";
                    name6 = "kiju";

                    obj->value[ 2 ] = 5;
                    obj->value[ 3 ] = 8;
                    obj->weight = 20;
                    break;
                case 2:
                    name = "drewniana laska";
                    sh_desc = "drewniana laska";
                    name2 = "drewnianej laski";
                    name3 = "drewnianej lasce";
                    name4 = "drewnian± laskê";
                    name5 = "drewnian± lask±";
                    name6 = "drewnianej lasce";

                    obj->value[ 2 ] = 4;
                    obj->value[ 3 ] = 8;
                    obj->value[ 6 ] = 1;
                    obj->weight = 15;
                    break;

                case 3:
                    name = "kostur";
                    sh_desc = "kostur";
                    name2 = "kostura";
                    name3 = "kosturowi";
                    name4 = "kostur";
                    name5 = "kosturem";
                    name6 = "kosturze";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 2 ] = 4;
                            break;
                        case 2:
                            obj->value[ 2 ] = 5;
                            break;
                        case 3:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 4:
                            obj->value[ 2 ] = 7;
                            break;
                    }
                    obj->value[ 3 ] = 8;
                    obj->weight = 90;
                    break;

                case 4:
                    name = "stalowy kostur";
                    sh_desc = "stalowy kostur";
                    name2 = "stalowego kostura";
                    name3 = "stalowemu kosturowi";
                    name4 = "stalowy kostur";
                    name5 = "stalowym kosturem";
                    name6 = "stalowym kosturze";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 2 ] = 7;
                            obj->value[ 5 ] = 1;
                            break;
                        case 4:
                            obj->value[ 2 ] = 8;
                            obj->value[ 6 ] = 1;
                            break;
                    }
                    obj->value[ 3 ] = 8;
                    obj->weight = 90;
                    obj->material = 2;
                    break;
                case 5:
                    name = "okuty kostur";
                    sh_desc = "okuty kostur";
                    name2 = "okutego kostura";
                    name3 = "okutemu kosturowi";
                    name4 = "okuty kostur";
                    name5 = "okutym kosturem";
                    name6 = "okutym kosturze";

                    switch ( number_range( 1, wer ) )
                    {
                        case 1:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 5;
                            break;
                        case 2:
                            obj->value[ 1 ] = 2;
                            obj->value[ 2 ] = 6;
                            break;
                        case 3:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 4;
                            break;
                        case 4:
                            obj->value[ 1 ] = 3;
                            obj->value[ 2 ] = 5;
                            break;
                    }
                    obj->value[ 3 ] = 8;
                    obj->weight = 90;
                    obj->material = 4;
                    obj->value[ 5 ] += number_range( 0, 1 );
                    obj->value[ 6 ] += number_range( 0, 1 );
                    break;
            }
            break;
    }


    /* Ustalam cenê na podstawie kostek obra¿eñ + bonusów */
    /*	   obj->cost+=(obj->value[2]^obj->value[1])/2 + 5*(obj->value[5]+obj->value[6]);*/

    obj->cost += ( obj->value[ 2 ] ^ obj->value[ 1 ] ) * 10;
    obj->cost += obj->value[ 5 ] * COST_APPLY_HITROLL;
    obj->cost += obj->value[ 6 ] * COST_APPLY_DAMROLL;

    free_string( obj->short_descr ); obj->short_descr = str_dup( sh_desc );
    free_string( obj->name ); obj->name = str_dup( name );
    free_string( obj->name2 ); obj->name2 = str_dup( name2 );
    free_string( obj->name3 ); obj->name3 = str_dup( name3 );
    free_string( obj->name4 ); obj->name4 = str_dup( name4 );
    free_string( obj->name5 ); obj->name5 = str_dup( name5 );
    free_string( obj->name6 ); obj->name6 = str_dup( name6 );


    free_string( obj->description );
    sprintf( text, "%s przyci±ga twój wzrok.", capitalize( sh_desc ) );
    obj->description = str_dup( text );

    if ( !second )
        return;

    if ( number_percent() > MAGIC_ITEMS )
        return;

    *points -= 3;

    /* teraz dodatkowe bajery w petli */
    go = TRUE;

    while ( go )
    {
        enchanted = FALSE;

        switch ( number_percent() )
        {
            case 1:
                SET_BIT( obj->value[ 4 ], WEAPON_UNBALANCED );
                break;
            case 2:
                if ( weapon_type == WEAPON_DAGGER )
                {
                    SET_BIT( obj->value[ 4 ], WEAPON_HEARTSEEKER );
                }
                break;
                /* bonus do obrazen*/
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
                obj->value[ 6 ] ++;
                obj->cost += COST_APPLY_DAMROLL;
                enchanted = TRUE;
                break;
                /* bonus do trafienia */
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
                obj->value[ 5 ] ++;
                obj->cost += COST_APPLY_HITROLL;;
                enchanted = TRUE;
                break;
                /* lzejsza */
            case 21:
            case 22:
            case 23:
            case 24:
            case 25:
                obj->weight = UMAX( 1, obj->weight - 10 );
                obj->cost += 50;
                enchanted = TRUE;
                break;
            case 26:
                if ( *points < 2 )
                    break;

                if ( IS_SET( obj->value[ 4 ], WEAPON_FROST ) )
                    break;

                SET_BIT( obj->value[ 4 ], WEAPON_FLAMING );
                obj->cost += COST_WEAPON_FLAMING;
                enchanted = TRUE;
                go = FALSE;
                break;

                /* frost */
            case 27:
                if ( *points < 2 )
                    break;

                if ( IS_SET( obj->value[ 4 ], WEAPON_FLAMING ) )
                    break;

                SET_BIT( obj->value[ 4 ], WEAPON_FROST );
                obj->cost += COST_WEAPON_FROST;
                enchanted = TRUE;
                go = FALSE;
                break;
                /* primary */
            case 28:
            case 29:
            case 30:
            case 31:
            case 32:
            case 33:
            case 34:
                SET_BIT( obj->value[ 4 ], WEAPON_PRIMARY );
                enchanted = TRUE;
                break;

                /* +1 type of dice*/
            case 35:
            case 36:
            case 37:
                obj->value[ 2 ] ++;
                obj->cost += 120;
                enchanted = TRUE;
                go = FALSE;
                break;

                /* +1 number of dice*/
            case 38:
            case 39:
            case 40:
                if ( *points < 1 )
                    break;
                obj->value[ 1 ] ++;
                obj->cost += 320;
                enchanted = TRUE;
                go = FALSE;
                break;
                /* vampiric */
            case 41:
                if ( *points < 2 )
                    break;
                SET_BIT( obj->value[ 4 ], WEAPON_VAMPIRIC );
                obj->cost += COST_WEAPON_VAMPIRIC;
                enchanted = TRUE;
                go = FALSE;
                break;
                /* sharp */
            case 42:
            case 43:
            case 44:
            case 45:
            case 46:
                if ( weapon_type == WEAPON_MACE
                        || weapon_type == WEAPON_FLAIL
                        || weapon_type == WEAPON_STAFF
                        || weapon_type == WEAPON_WHIP )
                    break;

                if ( IS_SET( obj->value[ 4 ], WEAPON_SHARP ) )
                    break;

                SET_BIT( obj->value[ 4 ], WEAPON_SHARP );
                obj->cost += COST_WEAPON_SHARP;
                enchanted = TRUE;
                break;

                /* minus do trafienia */
            case 47:
            case 48:
            case 49:
            case 50:
            case 51:
            case 52:
                obj->value[ 5 ] --;
                obj->cost -= COST_APPLY_HITROLL;
                if ( obj->cost < 1 )
                    obj->cost = 1;

                enchanted = TRUE;
                break;
                /* vorpal */
            case 53:
                if ( *points < 2 )
                    break;
                if ( weapon_type == WEAPON_MACE
                        || weapon_type == WEAPON_FLAIL
                        || weapon_type == WEAPON_STAFF
                        || weapon_type == WEAPON_WHIP )
                    break;

                if ( IS_SET( obj->value[ 4 ], WEAPON_SHARP ) )
                    break;

                SET_BIT( obj->value[ 4 ], WEAPON_VORPAL );
                obj->cost += COST_WEAPON_VORPAL;
                enchanted = TRUE;
                go = FALSE;
                break;

                /* shocking */
            case 54:
                if ( *points < 2 )
                    break;
                SET_BIT( obj->value[ 4 ], WEAPON_SHOCKING );
                obj->cost += COST_WEAPON_SHOCKING;
                enchanted = TRUE;
                go = FALSE;
                break;

                /* poison */
            case 55:
            case 56:
            case 57:
            case 58:
                SET_BIT( obj->value[ 4 ], WEAPON_POISON );
                obj->cost += COST_WEAPON_POISON;
                enchanted = TRUE;
                break;
                /* minus do obrazen */
            case 59:
            case 60:
            case 61:
            case 62:
            case 63:
            case 64:
            case 65:
            case 66:
                obj->value[ 6 ] --;
                obj->cost -= COST_APPLY_DAMROLL;
                if ( obj->cost < 1 )
                    obj->cost = 1;
                enchanted = TRUE;
                break;
                /* ciezsza */
            case 67:
            case 68:
            case 69:
            case 70:
            case 71:
                obj->weight += 5;
                obj->cost -= 100;
                if ( obj->cost < 1 )
                    obj->cost = 1;
                enchanted = TRUE;
                break;
            case 72:
            case 73:
            case 74:
            case 75:
            case 76:
            case 77:
            case 78:
                EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );
                obj->value[ 5 ] = 1;
                obj->value[ 6 ] = 1;
                obj->cost += COST_APPLY_HITROLL;
                obj->cost += COST_APPLY_DAMROLL;
                obj->cost += 100;
                enchanted = TRUE;
                go = FALSE;
                break;
            case 79:
            case 80:
            case 81:
            case 82:
                EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );
                obj->cost += 2 * COST_APPLY_HITROLL;
                obj->cost += 2 * COST_APPLY_DAMROLL;
                obj->cost += 250;
                enchanted = TRUE;
                go = FALSE;
                break;
            case 83:
                if ( *points < 2 )
                    break;
                SET_BIT( obj->value[ 4 ], WEAPON_TOXIC );
                obj->cost += COST_WEAPON_FLAMING;
                enchanted = TRUE;
                go = FALSE;
                break;
            case 84:
                if ( *points < 2 )
                    break;
                SET_BIT( obj->value[ 4 ], WEAPON_SACRED );
                obj->cost += COST_WEAPON_FLAMING;
                enchanted = TRUE;
                go = FALSE;
                break;
            case 85:
                if ( *points < 2 )
                    break;
                SET_BIT( obj->value[ 4 ], WEAPON_RESONANT );
                obj->cost += COST_WEAPON_FLAMING;
                enchanted = TRUE;
                go = FALSE;
                break;
            case 86:
                if ( *points < 2 )
                {
                    break;
                }
                if ( weapon_type == WEAPON_MACE || weapon_type == WEAPON_FLAIL || weapon_type == WEAPON_STAFF || weapon_type == WEAPON_WHIP )
                {
                    break;
                }
                SET_BIT( obj->value[ 4 ], WEAPON_INJURIOUS );
                obj->cost += COST_WEAPON_SHARP;
                enchanted = TRUE;
                go = FALSE;
                break;

                /* keen */
            case 87:
                if ( *points < 2 )
                    break;

                if ( IS_SET( obj->value[ 4 ], WEAPON_THUNDERING ) )
                    break;

                SET_BIT( obj->value[ 4 ], WEAPON_KEEN );
                obj->cost += COST_WEAPON_KEEN;
                enchanted = TRUE;
                go = FALSE;
                break;

                /* thundering */
            case 88:
                if ( *points < 2 )
                    break;

                if ( IS_SET( obj->value[ 4 ], WEAPON_KEEN ) )
                    break;

                SET_BIT( obj->value[ 4 ], WEAPON_THUNDERING );
                obj->cost += COST_WEAPON_THUNDERING;
                enchanted = TRUE;
                go = FALSE;
                break;

            default: break;
        }

        if ( enchanted ) * points -= 1;
        if ( enchanted && number_percent() > 50 ) go = FALSE;
        if ( *points <= 0 ) go = FALSE;
    }
    /* koniec petli losujace bajery na valuesach */

    //teraz scinanie jak sie wylosuje za dobre
    if( IS_WEAPON_STAT( obj, WEAPON_TWO_HANDS ) )
    {
        one_hand = FALSE;
    }
    else
    {
        one_hand = TRUE;
    }

tu_scinam:

    if ( (!one_hand && ( ( ( obj->value[ 1 ] * obj->value[ 2 ] + obj->value[ 1 ] + 2 * obj->value[ 6 ] ) / 2 ) > 18 )) ||
            (one_hand && ( ( ( obj->value[ 1 ] * obj->value[ 2 ] + obj->value[ 1 ] + 2 * obj->value[ 6 ] ) / 2 ) > 11 )) )
    {
        if( dice(1,2) == 1 )
        {
            --obj->value[ 2 ];
        }
        else
        {
            --obj->value[ 6 ];
        }
    }

    if ( (!one_hand && ( ( ( obj->value[ 1 ] * obj->value[ 2 ] + obj->value[ 1 ] + 2 * obj->value[ 6 ] ) / 2 ) > 18 )) ||
            (one_hand && ( ( ( obj->value[ 1 ] * obj->value[ 2 ] + obj->value[ 1 ] + 2 * obj->value[ 6 ] ) / 2 ) > 11 )) )
        goto tu_scinam;

    if ( !third ) return;

    obj->enchanted = TRUE;
    enchanted = FALSE;

    switch ( number_range( 1, 100 ) )
    {
        /* +stat */
        case 1:
            val = number_range( 1, 3 );
            loc = APPLY_STR;
            switch ( number_range ( 1, 7 ) )
            {
                default:
                case 1:
                    loc = APPLY_STR;
                    break;
                case 2:
                    loc = APPLY_DEX;
                    break;
                case 3:
                    loc = APPLY_INT;
                    break;
                case 4:
                    loc = APPLY_WIS;
                    break;
                case 5:
                    loc = APPLY_CON;
                    break;
                case 6:
                    loc = APPLY_CHA;
                    break;
                case 7:
                    loc = APPLY_LUC;
                    break;
            }
            mod = val;
            level = 0;
            obj->cost += COST_APPLY_STAT;
            enchanted = TRUE;
            break;

            /* skill do broni */
        case 2:
            val = number_range( 1, 10 );
            loc = APPLY_SKILL;
            mod = skill_lookup( weapon_class[ weapon_type ].name );
            level = val;
            obj->cost += val * COST_APPLY_SKILL;
            enchanted = TRUE;
            break;
            /* AC */
        case 3:
            val = number_range( -20, 5 );
            loc = APPLY_AC;
            mod = val;
            level	= 0;
            obj->cost -= val*COST_APPLY_AC;
            enchanted = TRUE;
            break;
            /* HP */
        case 4:
            val = number_range( 1, 10 );
            loc = APPLY_HIT;
            mod = val;
            level	= 0;
            obj->cost += 50 * val;
            enchanted = TRUE;
            break;

            /* HITROLL */
        case 5:
            val = 0;
            if (number_range(1,2) < 2 )
            {
                val = -1;
            }
            else if (number_range(1,20) < 2 )
            {
                val = 1;
            }
            else if (number_range(1,20) < 2 )
            {
                val = 2;
            }
            if (val)
            {
                loc        = APPLY_HITROLL;
                mod        = val;
                level	   = 0;
                obj->cost += val * 350;
                enchanted  = TRUE;
            }
            break;

            /* DAMTROLL */
        case 6:
            val = 0;
            if (number_range(1,2) < 2 )
            {
                val = -1;
            }
            else if (number_range(1,20) < 2 )
            {
                val = 1;
            }
            else if (number_range(1,20) < 2 )
            {
                val = 2;
            }
            if (val)
            {
                loc = APPLY_DAMROLL;
                mod = val;
                level	= 0;
                obj->cost += val * 450;
                enchanted = TRUE;
            }
            break;
        default:
            break;
    }
    /**
     * zabezpiecznie, zeby polowa najbardziej wypasnych broni
     * nie dala sie reperowac
     */
    if ( obj->value[ 1 ] * obj->value[ 2 ] > 10 && number_percent() > 50 )
    {
        EXT_SET_BIT( obj->extra_flags, ITEM_NOREPAIR );
    }
    /**
     * diamentwe ostrza zawsze vorpal
     */
    if
        (
         obj->material == 36
         && ( obj->value[ 0 ] == WEAPON_SWORD || obj->value[ 0 ] == WEAPON_DAGGER || obj->value[ 0 ] == WEAPON_AXE || obj->value[ 0 ] == WEAPON_SHORTSWORD )
         && !IS_SET( obj->value[4], WEAPON_VORPAL )
        )
        {
            SET_BIT( obj->value[ 4 ], WEAPON_VORPAL );
            obj->cost += COST_WEAPON_VORPAL;
        }
    /**
     * czy zaczarowana
     */
    if ( enchanted )
    {
        add_new_affect( obj, type, loc, mod, level, bitv, dur, TRUE );
    }
    return;
}

void set_min_ac( OBJ_DATA *armor )
{
	int i;

	for ( i = 0; i < 4; i++ )
		armor->value[ i ] = armor_table[ armor->value[ 4 ] ].min_ac[ i ];
}
/* Procedura generujaca zbroje
 * na razie tylko sposrod zbioru 6 typow
 * mozna latwo dodac nastepne
 * na razie nie laduje materialu...
 */
void generate_armor_body( OBJ_DATA *obj, int* points )
{
	char text[ MAX_INPUT_LENGTH ], *name = "(null)", *sh_desc = "(null)";
	char *name2 = "null", *name3 = "null", *name4 = "null", *name5 = "null", *name6 = "null";
	bool go = TRUE, enchanted = FALSE, second = FALSE, third = FALSE;
	int val, wer = 6;
	int type = 0, loc = 0, mod = 0, level = 0, dur = -1;
	BITVECT_DATA * bitv = &AFF_NONE;


	if ( *points < 3 )
	{
		switch ( *points )
		{
			case 0: wer = 2;break;
			case 1: wer = 3;break;
			case 2: wer = 4;break;
			case 3: wer = 6;break;
		}
	}

	if ( *points > 3 ) second = TRUE;
	if ( *points > 6 ) third = TRUE;

	generate_extra( obj );

	switch ( number_range( 1, wer ) )
	{
		case 1:
			name = "skórzana zbroja";
			sh_desc = "skórzana zbroja";
			name2 = "skórzanej zbroi";
			name3 = "skórzanej zbroi";
			name4 = "skórzana zbroje";
			name5 = "skórzana zbroja";
			name6 = "skórzanej zbroi";

			obj->value[ 4 ] = 1;
			set_min_ac( obj );
			obj->cost = 7;
			obj->weight = 120;
			obj->material = 6;
			break;
		case 2:
			name = "æwiekowa zbroja";
			sh_desc = "æwiekowa zbroja";
			name2 = "æwiekowej zbroi";
			name3 = "æwiekowej zbroi";
			name4 = "æwiekowa zbroje";
			name5 = "æwiekowa zbroja";
			name6 = "æwiekowej zbroi";

			obj->value[ 4 ] = 2;
			set_min_ac( obj );
			obj->cost = 12;
			obj->weight = 130;
			obj->material = 6;
			break;
		case 3:
			name = "gruba skórzana zbroja";
			sh_desc = "gruba skórzana zbroja";
			name2 = "grubej skórzanej zbroi";
			name3 = "grubej skórzanej zbroi";
			name4 = "gruba skórzana zbroje";
			name5 = "gruba skórzana zbroja";
			name6 = "grubej skórzanej zbroi";

			obj->value[ 4 ] = 3;
			set_min_ac( obj );
			obj->cost = 18;
			obj->weight = 140;
			obj->material = 6;
			break;
		case 4:
			name = "kolczuga";
			sh_desc = "kolczuga";
			name2 = "kolczugi";
			name3 = "kolczudze";
			name4 = "kolczuge";
			name5 = "kolczuga";
			name6 = "kolczudze";

			obj->value[ 4 ] = 8;
			set_min_ac( obj );
			obj->cost = 25;
			obj->weight = 150;
			obj->material = 4;
			break;
		case 5:
			name = "paskowa zbroja";
			sh_desc = "paskowa zbroja";
			name2 = "paskowej zbroi";
			name3 = "paskowej zbroi";
			name4 = "paskowa zbroje";
			name5 = "paskowa zbroja";
			name6 = "paskowej zbroi";

			obj->value[ 4 ] = 9;
			set_min_ac( obj );
			obj->cost = 35;
			obj->weight = 130;
			obj->material = 4;

			break;
		case 6:
			name = "zbroja p³ytowa";
			sh_desc = "p³ytowa zbroja";
			name2 = "p³ytowej zbroi";
			name3 = "p³ytowej zbroi";
			name4 = "p³ytowa zbroje";
			name5 = "p³ytowa zbroja";
			name6 = "p³ytowej zbroi";

			obj->value[ 4 ] = 14;
			set_min_ac( obj );
			obj->cost = 120;
			obj->weight = 250;
			obj->material = 4;
			break;
			//mozna jeszcze: luskowa, p³ytowa kolcza, spizowa, skórzana æwiekowana
		default : break;
	}

	free_string( obj->short_descr ); obj->short_descr = str_dup( sh_desc );
	free_string( obj->name ); obj->name = str_dup( name );
	free_string( obj->name2 ); obj->name2 = str_dup( name2 );
	free_string( obj->name3 ); obj->name3 = str_dup( name3 );
	free_string( obj->name4 ); obj->name4 = str_dup( name4 );
	free_string( obj->name5 ); obj->name5 = str_dup( name5 );
	free_string( obj->name6 ); obj->name6 = str_dup( name6 );

	free_string( obj->description );
	sprintf( text, "%s le¿y tutaj.", capitalize( sh_desc ) );
	obj->description = str_dup( text );

	if ( !second ) return;

	if ( number_percent() > MAGIC_ITEMS ) return;

	*points -= 3;

	/* teraz dodatkowe bajery w petli */
	while ( go )
	{
		enchanted = FALSE;

		switch ( number_range( 1, 12 ) )
		{
				/* bonus do AC_PIERCE*/
			case 1:
				obj->value[ 0 ] --;
				obj->cost += 25 * obj->cost / 100;
				enchanted = TRUE;
				break;
				/* bonus do AC_BASH*/
			case 2:
				obj->value[ 1 ] --;
				obj->cost += 25 * obj->cost / 100;
				enchanted = TRUE;
				break;
				/* bonus do AC_SLASH*/
			case 3:
				obj->value[ 2 ] --;
				obj->cost += 25 * obj->cost / 100;
				enchanted = TRUE;
				break;
				/* bonus do AC_EXOTIC*/
			case 4:
				obj->value[ 3 ] --;
				obj->cost += 25 * obj->cost / 100;
				enchanted = TRUE;
				break;
				/* lzejsza */
			case 5:
				obj->weight -= 10;
				obj->cost += 10;
				enchanted = TRUE;
				break;
				/* ciezsza */
			case 6:
				obj->weight += 10;
				enchanted = TRUE;
				break;
			case 7:
				obj->value[ 0 ] ++;
				enchanted = TRUE;
				break;
			case 8:
				obj->value[ 1 ] ++;
				enchanted = TRUE;
				break;
			case 9:
				obj->value[ 2 ] ++;
				enchanted = TRUE;
				break;
			case 10:
				obj->value[ 3 ] ++;
				enchanted = TRUE;
				break;

			default: break;
		}
		if ( enchanted )
			* points -= 1;

		if ( enchanted && number_percent() > 50 )
			go = FALSE;

		if ( *points <= 0 )
			go = FALSE;
	}
	/* koniec petli losujace bajery ba valuesach */

	if ( !third )
		return;

	/* tylko jeden affect */
	obj->enchanted = TRUE;
	enchanted = FALSE;

	switch ( number_range( 1, 6 ) )
	{
/* WY£¡CZAM: Gurthg - 2005-02-20
		case 1:
			val = 1;
			if (number_range(1,10) == 1 ) ++val;
			loc = APPLY_CON;
			mod = val;
			level	= 0;
			obj->cost += val * COST_APPLY_STAT;
			enchanted = TRUE;
			break;
*/
		case 2:
			val = number_range( 5, 25 );
			loc = APPLY_AC;
			mod = -val;
			level	= 0;
			obj->cost += val * COST_APPLY_AC;
			enchanted = TRUE;
			break;
		case 3:
			val = number_range( 1, 5 );
			loc = APPLY_HIT;
			mod = val;
			level	= 0;
			obj->cost += val * COST_APPLY_SKILL;
			enchanted = TRUE;
			break;
		case 4:
			val = number_range( 1, 10 );
			loc = APPLY_RESIST;
			mod = number_range(1,3);
			level = val;
			obj->cost = 2*val*COST_APPLY_RESIST;
			enchanted = TRUE;
			break;

		default: break;
	}

	if ( enchanted )
		add_new_affect( obj, type, loc, mod, level, bitv, dur, TRUE );

	return;
}



void generate_armor_head( OBJ_DATA *obj, int* points )
{
	int val, wer = 6;
	char text[ MAX_INPUT_LENGTH ], *name = "(null)", *sh_desc = "(null)";
	char *name2 = "(null)", *name3 = "(null)", *name4 = "(null)", *name5 = "(null)", *name6 = "(null)";
	bool go = TRUE, enchanted = FALSE, second = FALSE, third = FALSE;
	int type = 0, loc = 0, mod = 0, level = 0, dur = -1;
	BITVECT_DATA * bitv = &AFF_NONE;


	if ( *points < 3 )
		switch ( *points )
		{
			case 0: wer = 2;break;
			case 1: wer = 3;break;
			case 2: wer = 4;break;
			case 3: wer = 5;break;
		}

	if ( *points > 3 )
		second = TRUE;

	if ( *points > 6 )
		third = TRUE;

	generate_extra( obj );

	switch ( number_range( 1, UMIN( wer, 5 ) ) )
	{
		case 1:
			name = "he³m skórzany";
			sh_desc = "skórzany he³m";
			name2 = "skórzanego he³mu";
			name3 = "skórzanemu he³mowi";
			name4 = "skórzany he³m";
			name5 = "skórzanym he³mem";
			name6 = "skórzanym he³mie";

			obj->value[ 4 ] = 1;
			set_min_ac( obj );
			obj->material = 6;
			obj->cost = 1;
			obj->weight = 10;
			break;
		case 2:
			name = "lekki he³m";
			sh_desc = "lekki he³m";
			name2 = "lekkiego he³mu";
			name3 = "lekkiemu he³mowi";
			name4 = "lekki he³m";
			name5 = "lekkim he³mem";
			name6 = "lekkim he³mie";

			obj->value[ 4 ] = 3;
			set_min_ac( obj );
			obj->cost = 4;
			obj->material = 6;
			obj->weight = 20;
			break;
		case 3:
			name = "he³m ciê¿ki";
			sh_desc = "ciê¿ki he³m";
			name2 = "ciê¿kiego he³mu";
			name3 = "ciê¿kiemu he³mowi";
			name4 = "ciê¿ki he³m";
			name5 = "ciê¿kim he³mem";
			name6 = "ciê¿kim he³mie";

			obj->value[ 4 ] = 8;
			set_min_ac( obj );
			obj->material = 2;
			obj->cost = 10;
			obj->weight = 25;
			break;
		case 4:
			name = "he³m przy³bica";
			sh_desc = "he³m z przy³bic±";
			name2 = "he³mu z przy³bic±";
			name3 = "he³mowi z przy³bic±";
			name4 = "he³m z przy³bic±";
			name5 = "he³mem z przy³bic±";
			name6 = "he³mie z przy³bic±";
			obj->value[ 4 ] = 12;
			set_min_ac( obj );
			obj->material = 2;
			obj->cost = 15;
			obj->weight = 30;
			break;
		case 5:
			name = "he³m pe³ny";
			sh_desc = "pe³ny he³m";
			name2 = "pe³nego he³mu";
			name3 = "pe³nemu he³mowi";
			name4 = "pe³ny he³m";
			name5 = "pe³nym he³mem";
			name6 = "pe³nym he³mie";

			obj->value[ 4 ] = 14;
			set_min_ac( obj );
			obj->material = 2;
			obj->cost = 27;
			obj->weight = 35;
			break;

		default : break;
	}

	free_string( obj->short_descr );
	obj->short_descr = str_dup( sh_desc );

	free_string( obj->name );
	obj->name = str_dup( name );

	free_string( obj->name2 );
	obj->name2 = str_dup( name2 );

	free_string( obj->name3 );
	obj->name3 = str_dup( name3 );

	free_string( obj->name4 );
	obj->name4 = str_dup( name4 );

	free_string( obj->name5 );
	obj->name5 = str_dup( name5 );

	free_string( obj->name6 );
	obj->name6 = str_dup( name6 );


	free_string( obj->description );
	sprintf( text, "%s le¿y tutaj.", capitalize( sh_desc ) );
	obj->description = str_dup( text );

	if ( !second )
		return;
	if ( number_percent() > MAGIC_ITEMS )
		return;

	*points -= 3;

	/* teraz dodatkowe bajery w petli */
	while ( go )
	{
		switch ( number_range( 1, 50 ) )
		{
				/* bonus do AC_PIERCE*/
			case 1:
				obj->value[ 0 ] --;
				obj->cost += 25 * obj->cost / 100;
				break;
				/* bonus do AC_BASH*/
			case 2:
				obj->value[ 1 ] --;
				obj->cost += 25 * obj->cost / 100;
				break;
				/* bonus do AC_SLASH*/
			case 3:
				obj->value[ 2 ] --;
				obj->cost += 25 * obj->cost / 100;
				break;
				/* bonus do AC_EXOTIC*/
			case 4:
				obj->value[ 3 ] --;
				obj->cost += 25 * obj->cost / 100;
				break;
				/* lzejsza */
			case 5:
				obj->weight -= 10;
				obj->cost += 5;
				break;
				/* ciezsza */
			case 6:
				obj->weight += 10;
				break;
			case 7:
				obj->value[ 0 ] ++;
				obj->cost += 10;
				break;
			case 8:
				obj->value[ 1 ] ++;
				obj->cost += 18;
				break;
			case 9:
				obj->value[ 2 ] ++;
				obj->cost += 6;
				break;
			case 10:
				obj->value[ 3 ] ++;
				obj->cost += 13;
				break;

			default: break;
		}

		if ( enchanted )
			* points -= 1;

		if ( number_percent() > 50 )
			go = FALSE;

		if ( *points <= 0 )
			go = FALSE;
	}
	/* koniec petli losujace bajery ba valuesach */

	obj->enchanted = TRUE;
	enchanted = FALSE;

	if ( !third )
		return;

	switch ( number_range( 1, 50 ) )
	{
/* WY£¡CZAM: Gurthg - 2005-02-20
		case 1:
			val = number_range( 1, 2 );
			if ( number_range(1,10) == 1 ) ++val;
			loc = APPLY_WIS;
			mod = val;
			level	= 0;
			obj->cost += val * COST_APPLY_STAT;
			enchanted = TRUE;
			break;
*/
		case 2:
			val = number_range( 2, 6 );

			loc = APPLY_AC;
			mod = -val * 5;
			level	= 0;
			obj->cost += ( val * 5 * COST_APPLY_AC );
			enchanted = TRUE;
			break;
/* WY£¡CZAM: Gurthg - 2005-02-20
		case 3:
			val = number_range( 1, 2 );
			if ( number_range(1,10) == 1 ) ++val;
			loc = APPLY_INT;
			mod = val;
			level	= 0;
			obj->cost += val * COST_APPLY_STAT;
			enchanted = TRUE;
			break;
*/
		case 4:
			val = number_range( 4, -2 );
			loc = APPLY_SAVING_SPELL;
			mod = -val;
			level = 0;
			obj->cost += val * COST_APPLY_SAVING;
			enchanted = TRUE;
			break;
		case 5:
			loc = APPLY_MEMMING;
			mod = number_range(1,3);
			level = number_range(1,3);
			obj->cost += COST_APPLY_MEMMING;
			enchanted = TRUE;
			break;

		default: break;
	}

	if ( enchanted )
		add_new_affect( obj, type, loc, mod, level, bitv, dur, TRUE );

	return;
}


void generate_armor_hands( OBJ_DATA *obj, int* points )
{
	int val, wer = 5;
	char text[ MAX_INPUT_LENGTH ], *name = "(null)", *sh_desc = "(null)";
	char *name2 = "null", *name3 = "null", *name4 = "null", *name5 = "null", *name6 = "null";
	bool go = TRUE, enchanted = FALSE, second = FALSE, third = FALSE;
	int type = 0, loc = 0, mod = 0, level = 0, dur = -1;
	BITVECT_DATA *bitv = &AFF_NONE;


	if ( *points < 3 )
		switch ( *points )
		{
			case 0: wer = 2;break;
			case 1: wer = 3;break;
			case 2: wer = 4;break;
			case 3: wer = 5;break;
		}

	if ( *points > 3 )
		second = TRUE;

	if ( *points > 6 )
		third = TRUE;

	generate_extra( obj );
	obj->liczba_mnoga = TRUE;

	switch ( number_range( 1, wer ) )
	{
		case 1:
			name = "skórzane rekawice";
			sh_desc = "skórzane rekawice";
			name2 = "skórzanych rekawic";
			name3 = "skórzanym rekawicom";
			name4 = "skórzane rekawice";
			name5 = "skórzanymi rekawicami";
			name6 = "skórzanych rekawicach";

			obj->value[ 4 ] = 1;
			set_min_ac( obj );
			obj->cost = 1;
			obj->weight = 5;
			obj->material = 6;
			break;
		case 2:
			name = "grube skórzane rekawice";
			sh_desc = "grube skórzane rekawice";
			name2 = "grubych skórzanych rekawic";
			name3 = "grubym skórzanym rekawicom";
			name4 = "grube skórzane rekawice";
			name5 = "grubymi skórzanymi rekawicami";
			name6 = "grubych skórzanych rekawicach";

			obj->value[ 4 ] = 1;
			set_min_ac( obj );
			obj->cost = 3;
			obj->weight = 10;
			obj->material = 6;
			break;
		case 3:
			name = "metalowe rekawice";
			sh_desc = "metalowe rekawice";
			name2 = "metalowych rekawic";
			name3 = "metalowym rekawicom";
			name4 = "metalowe rekawice";
			name5 = "metalowymi rekawicami";
			name6 = "metalowych rekawicach";

			obj->value[ 4 ] = 9;
			set_min_ac( obj );
			obj->material = 2;
			obj->cost = 10;
			obj->weight = 15;
			break;
		case 4:
			name = "ciê¿kie metalowe rêkawice";
			sh_desc = "ciê¿kie metalowe rêkawice";
			name2 = "ciê¿kich metalowych rêkawic";
			name3 = "ciê¿kim metalowym rêkawicom";
			name4 = "ciê¿kie metalowe rêkawice";
			name5 = "ciê¿kimi metalowymi rêkawicami";
			name6 = "ciê¿kich metalowych rêkawicach";

			obj->value[ 4 ] = 13;
			set_min_ac( obj );
			obj->cost = 15;
			obj->weight = 22;
			obj->material = 2;
			break;
		case 5:
			name = "pancerne rekawice";
			sh_desc = "pancerne rekawice";
			name2 = "pancernych rekawic";
			name3 = "pancernym rekawicom";
			name4 = "pancerne rekawice";
			name5 = "pancernymi rekawicami";
			name6 = "pancernych rekawicach";


			obj->value[ 4 ] = 14;
			set_min_ac( obj );
			obj->cost = 27;
			obj->material = 2;
			obj->weight = 35;
			break;
		default : break;
	}



	free_string( obj->short_descr );
	obj->short_descr = str_dup( sh_desc );

	free_string( obj->name );
	obj->name = str_dup( name );

	free_string( obj->name2 );
	obj->name2 = str_dup( name2 );

	free_string( obj->name3 );
	obj->name3 = str_dup( name3 );

	free_string( obj->name4 );
	obj->name4 = str_dup( name4 );

	free_string( obj->name5 );
	obj->name5 = str_dup( name5 );

	free_string( obj->name6 );
	obj->name6 = str_dup( name6 );


	free_string( obj->description );
	sprintf( text, "%s le¿± tutaj.", capitalize( sh_desc ) );
	obj->description = str_dup( text );

	generate_extra( obj );

	if ( !second ) return;

	if ( number_percent() > MAGIC_ITEMS ) return;
	*points -= 3;

	/* teraz dodatkowe bajery w petli */
	while ( go )
	{
		enchanted = FALSE;
		switch ( number_range( 1, 12 ) )
		{
				/* bonus do AC_PIERCE*/
			case 1:
				obj->value[ 0 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* bonus do AC_BASH*/
			case 2:
				obj->value[ 1 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* bonus do AC_SLASH*/
			case 3:
				obj->value[ 2 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* bonus do AC_EXOTIC*/
			case 4:
				obj->value[ 3 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* lzejsza */
			case 5:
				obj->weight -= 10;
				obj->cost += 5;
				enchanted = TRUE;
				break;
				/* ciezsza */
			case 6:
				obj->weight += 10;
				obj->cost += 5;
				enchanted = TRUE;
				break;
			case 7:
				obj->value[ 0 ] ++;
				obj->cost += 10;
				enchanted = TRUE;
				break;
			case 8:
				obj->value[ 1 ] ++;
				obj->cost += 18;
				enchanted = TRUE;
				break;
			case 9:
				obj->value[ 2 ] ++;
				obj->cost += 6;
				enchanted = TRUE;
				break;
			case 10:
				obj->value[ 3 ] ++;
				obj->cost += 13;
				enchanted = TRUE;
				break;

			default: break;
		}

		if ( enchanted ) * points -= 1;
		if ( enchanted && number_percent() > 50 ) go = FALSE;
		if ( *points <= 0 ) go = FALSE;
	}
	/* koniec petli losujace bajery na valuesach */

	if ( !third ) return;

	obj->enchanted = TRUE;
	enchanted = FALSE;


	switch ( number_range( 1, 50 ) )
	{
/* WY£¡CZAM: Gurthg - 2005-02-20
		case 1:
			val = 1;
			if ( number_range(1,10) == 1 ) ++val;
			loc = APPLY_STR;
			mod = val;
			level	= 0;
			obj->cost += val * COST_APPLY_STAT;
			enchanted = TRUE;
			break;
*/
		case 2:
			switch( number_range(1,11))
			{
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
					val = -1;
					break;
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
					val = 1;
					break;
				case 11:
					val = 2;
					break;
			}
			loc = APPLY_DAMROLL;
			mod = val;
			level	= 0;
			obj->cost += val * (COST_APPLY_STAT/2);
			enchanted = TRUE;
			break;
/* WY£¡CZAM: Gurthg - 2005-02-20
		case 3:
			val = 1;
			if ( number_range(1,10) == 1 ) ++val;
			loc = APPLY_DEX;
			mod = val;
			level	= 0;
			obj->cost += val * COST_APPLY_STAT;
			enchanted = TRUE;
			break;
*/
		case 4:
			val = number_range( 1, 3 );
			loc = APPLY_HITROLL;
			mod = val;
			level	= 0;
			obj->cost += val * (COST_APPLY_STAT/2);
			enchanted = TRUE;
			break;
		case 5:
			loc = APPLY_SKILL;
			switch ( number_range( 1, 22 ) )
			{
				case 1: mod = gsn_axe;break;
				case 2: mod = gsn_dagger;break;
				case 3: mod = gsn_flail;break;
				case 4: mod = gsn_mace;break;
				case 5: mod = gsn_polearm;break;
				case 6: mod = gsn_shortsword;break;
				case 7: mod = gsn_spear;break;
				case 8: mod = gsn_staff;break;
				case 9: mod = gsn_sword;break;
				case 10: mod = gsn_whip;break;
				case 11: mod = gsn_two_weapon_fighting;break;
				case 12: mod = gsn_twohander_fighting;break;
				case 13: mod = gsn_flail_mastery;break;
				case 14: mod = gsn_dagger_mastery;break;
				case 15: mod = gsn_axe_mastery;break;
				case 16: mod = gsn_polearm_mastery;break;
				case 17: mod = gsn_shortsword_mastery;break;
				case 18: mod = gsn_spear_mastery;break;
				case 19: mod = gsn_staff_mastery;break;
				case 20: mod = gsn_sword_mastery;break;
				case 21: mod = gsn_whip_mastery;break;
				case 22: mod = gsn_mace_mastery;break;
			}
			level = number_range( 1, 10 );
			obj->cost += COST_APPLY_SKILL * level;
			enchanted = TRUE;
			break;
		default: break;
	}

	if ( enchanted )
		add_new_affect( obj, type, loc, mod, level, bitv, dur, TRUE );

	return;
}



void generate_armor_feets( OBJ_DATA *obj, int* points )
{
	int val, wer = 5;
	char text[ MAX_INPUT_LENGTH ], *name = "(null)", *sh_desc = "(null)";
	char *name2 = "null", *name3 = "null", *name4 = "null", *name5 = "null", *name6 = "null", *item_description = "";
	bool go = TRUE, enchanted = FALSE, second = FALSE, third = FALSE;
	int type = 0, loc = 0, mod = 0, level = 0, dur = -1;
	BITVECT_DATA * bitv = &AFF_NONE;

	if ( *points <= 3 )
		switch ( *points )
		{
			case 0: wer = 3;break;
			case 1: wer = 4;break;
			case 2: wer = 5;break;
			case 3: wer = 6;break;
		}

	if ( *points > 3 ) second = TRUE;
	if ( *points > 6 ) third = TRUE;
	ext_flags_clear( obj->wear_flags2 );

	switch ( number_range( 1, wer ) )
	{
		case 1:
			name = "lekkie skórzane buty";
			sh_desc = "lekkie skórzane buty";
			name2 = "lekkich skórzanych butow";
			name3 = "lekkim skórzanym butom";
			name4 = "lekkie skórzane buty";
			name5 = "lekkimi skórzanymi butami";
			name6 = "lekkich skórzanych butach";
			item_description = "Lekkie skórzane buty, takie jakich wiele.";

			obj->value[ 4 ] = 1;
			set_min_ac( obj );
			obj->cost = 1;
			obj->weight = 3;
			obj->material = 6;
			break;
		case 2:
        default:
			name = "skórzane buty";
			sh_desc = "skórzane buty";
			name2 = "skórzanych butow";
			name3 = "skórzanym butom";
			name4 = "skórzane buty";
			name5 = "skórzanymi butami";
			name6 = "skórzanych butach";
			item_description = "Skórzane buty, takie jakich wiele.";

			obj->value[ 4 ] = 1;
			set_min_ac( obj );
			obj->cost = 1;
			obj->weight = 5;
			obj->material = 6;
			break;
		case 3:
			name = "ciê¿kie skórzane buty";
			sh_desc = "ciê¿kie skórzane buty";
			name2 = "skórzanych butow";
			name3 = "skórzanym butom";
			name4 = "skórzane buty";
			name5 = "skórzanymi butami";
			name6 = "skórzanych butach";
			item_description = "Gruba podeszwa i solidna skóra tworz±ca te buty, pozwala s±dziæ, ¿e to dobre buty.";

			obj->value[ 4 ] = 11;
			set_min_ac( obj );
			obj->material = 6;
			obj->cost = 3;
			obj->weight = 10;
			break;
		case 4:
			name = "drewniaki";
			sh_desc = "drewniaki";
			name2 = "drewniakow";
			name3 = "drewniakom";
			name4 = "drewniaki";
			name5 = "drewniakami";
			name6 = "drewniakach";
			item_description = "Wyrze¼bione z jednego kawa³ka drewna, wygl±daj± na niewygodne.";

			obj->item_type = ITEM_CLOTHING;
			obj->value[ 4 ] = 8;
			set_min_ac( obj );
			obj->cost = 10;
			obj->weight = 15;
			obj->material = 1;
			break;
		case 5:
			name = "metalowe buty";
			sh_desc = "metalowe buty";
			name2 = "metalowych butow";
			name3 = "metalowym butom";
			name4 = "metalowe buty";
			name5 = "metalowymi butami";
			name6 = "metalowych butach";
			item_description = "Dok³adna snycersko-szewska robota. Skórzane buty z metalowymi wzomcnieami.";

			obj->value[ 4 ] = 13;
			set_min_ac( obj );
			obj->cost = 12;
			obj->weight = 20;
			obj->material = 13;
			break;
		case 6:
			name = "pancerne buty";
			sh_desc = "pancerne buty";
			name2 = "pancernych butow";
			name3 = "pancernym butom";
			name4 = "pancerne buty";
			name5 = "pancernymi butami";
			name6 = "pancernych butach";
            item_description = "Wykonane z paru kawa³ków metalu, pancerne buty zapewniaj± ca³kiem niez³± ochronê.";
            // w³a¶ciwo¶ci
            obj->value[ 4 ] = 14;
            set_min_ac( obj );
            obj->cost = 19;
            obj->weight = 35;
            obj->material = 2;
            break;
    }
    // cechy wspólne, liczba i rodzaj
    obj->liczba_mnoga = TRUE;
    obj->gender = GENDER_MESKOOSOBOWY;

	free_string( obj->short_descr );
	obj->short_descr = str_dup( sh_desc );

	free_string( obj->name );
	obj->name = str_dup( name );

	free_string( obj->name2 );
	obj->name2 = str_dup( name2 );

	free_string( obj->name3 );
	obj->name3 = str_dup( name3 );

	free_string( obj->name4 );
	obj->name4 = str_dup( name4 );

	free_string( obj->name5 );
	obj->name5 = str_dup( name5 );

	free_string( obj->name6 );
	obj->name6 = str_dup( name6 );

	free_string( obj->item_description );
	obj->item_description = str_dup( item_description );


	free_string( obj->description );
	sprintf( text, "%s le¿± tutaj.", capitalize( sh_desc ) );
	obj->description = str_dup( text );

	if ( !second ) return;

	if ( number_percent() > MAGIC_ITEMS ) return;
	*points -= 3;

	/* teraz dodatkowe bajery w petli */
	go = TRUE;

	while ( go )
	{
		enchanted = FALSE;
		switch ( number_range( 1, 12 ) )
		{
				/* bonus do AC_PIERCE*/
			case 1:
				obj->value[ 0 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* bonus do AC_BASH*/
			case 2:
				obj->value[ 1 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* bonus do AC_SLASH*/
			case 3:
				obj->value[ 2 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* bonus do AC_EXOTIC*/
			case 4:
				obj->value[ 3 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* lzejsza */
			case 5:
				obj->weight -= 10;
				obj->cost += 5;
				enchanted = TRUE;
				break;
				/* ciezsza */
			case 6:
				obj->weight += 10;
				obj->cost += 5;
				enchanted = TRUE;
				break;
			case 7:
				obj->value[ 0 ] ++;
				obj->cost += 10;
				enchanted = TRUE;
				break;
			case 8:
				obj->value[ 1 ] ++;
				obj->cost += 18;
				enchanted = TRUE;
				break;
			case 9:
				obj->value[ 2 ] ++;
				obj->cost += 6;
				enchanted = TRUE;
				break;
			case 10:
				obj->value[ 3 ] ++;
				obj->cost += 13;
				enchanted = TRUE;
				break;

			default: break;
		}

		if ( enchanted ) * points -= 1;
		if ( enchanted && number_percent() > 50 ) go = FALSE;
		if ( *points <= 0 ) go = FALSE;
	}
	/* koniec petli losujace bajery ba valuesach */
	if ( !third ) return;

	obj->enchanted = TRUE;
	enchanted = FALSE;


	switch ( number_range( 1, 50 ) )
	{
		case 1:
			val = number_range( 1, 10 );
			loc = APPLY_MOVE;
			mod = val;
			level	= 0;
			obj->cost += val * COST_APPLY_SKILL;
			enchanted = TRUE;
			break;
		case 2:
			val = number_range( 1, 15 );
			loc = APPLY_SKILL;
			mod = gsn_kick;
			level	= val;
			obj->cost += val * COST_APPLY_SKILL;
			enchanted = TRUE;
			break;
		case 3:
			val = number_range( 1, 15 );
			loc = APPLY_SKILL;
			mod = gsn_sneak;
			level	= val;
			obj->cost += val * COST_APPLY_SKILL;
			enchanted = TRUE;
			break;
/* WY£¡CZAM: Gurthg - 2005-02-20
		case 4:
			val = 1;
			if ( number_range(1,10) == 1 ) ++val;
			loc = APPLY_DEX;
			mod = val;
			level	= 0;
			obj->cost += val * COST_APPLY_STAT;
			enchanted = TRUE;
			break;
*/
		default: break;
	}

	if ( enchanted )
		add_new_affect( obj, type, loc, mod, level, bitv, dur, TRUE );

	return;
}

/* OK tu nie ma zbyt wiele miejsca do popisu skoro tarcza daje
 * tylko +10 do AC, istotne moze byc z jakiego materialu bedzie
 * najczesciej beda sie rozpieprzac tarcze wiec przyda sie miec twarda
 * dodatkowy bonus to plus do skilla basha (5-30)
 * no i jakies affecty do protekcji...
Agron: po poprawieniu na nowy typ tarcz daja takze do shield blocka
i ogolnie dostosowane.
 */
void generate_armor_shield( OBJ_DATA *obj, int* points )
{
	int bash = 0, ac_bonus = 0, wer = 4, block = 0, gender = GENDER_NONE, elem = 0, size_mod=1, bonus;
	char text[ MAX_INPUT_LENGTH ], *name = "(null)", *material = "(null)";
	char *name2 = "(null)", *name3 = "(null)", *name4 = "(null)", *name5 = "(null)", *name6 = "(null)";
	char *mat2 = "(null)", *mat3 = "(null)", *mat4 = "(null)", *mat5 = "(null)", *mat6 = "(null)";
	bool second = FALSE, third = FALSE, set_mat = FALSE, end = FALSE;
	AFFECT_DATA *pAf;


	if ( *points < 3 )
		switch ( *points )
		{
			case 0: wer = 1;break; // drewno
			case 1: wer = 1;break; // drewno
			case 2: wer = 2;break; // mied¼
			case 3: wer = 3;break; // z³oto
		}
	else
		wer = 4;

	if ( *points > 3 ) second = TRUE;
	if ( *points > 6 ) third = TRUE;

	generate_extra( obj );

	obj->value[ 0 ] = 0;
	obj->value[ 1 ] = 0;
	obj->value[ 2 ] = 0;
	obj->value[ 3 ] = 0;

	EXT_SET_BIT( obj->wear_flags2, ITEM_NOMAG );

	switch ( number_range( 1, wer ) )
	{
		case 1:   //puklerze
			name = "puklerz";
			name2 = "puklerza";
			name3 = "puklerzowi";
			name4 = "puklerz";
			name5 = "puklerzem";
			name6 = "puklerzu";

			obj->cost = 2;
			obj->weight = 5;
			obj->material = 1;
			//set_mat=TRUE;
			gender = GENDER_NIJAKI;
			size_mod = 4;

			obj->value[ 0 ] = 0;
			obj->value[ 1 ] = number_range( 0, 2 );
			obj->value[ 2 ] = number_range( 0, 18 );
			obj->value[ 3 ] = 0; //chance to block other attacker

			break;

		case 2:  //tarcze lekkie i srednie
			name = "tarcza";
			name2 = "tarczy";
			name3 = "tarczy";
			name4 = "tarczê";
			name5 = "tarcz±";
			name6 = "tarczy";

			obj->cost = 10;
			obj->weight = 50;
			EXT_SET_BIT( obj->wear_flags2, ITEM_NOTHI );

            gender = GENDER_MESKI;
            size_mod = 3;
			// val to skill bash
			if ( second )
				if ( number_percent() < 25 )
					bash = number_range( 5, 10 );

			if ( third )
			{
				switch ( number_range( 1, 20 ) )
				{
					case 18: ac_bonus = 1;break;
					case 19: ac_bonus = 2;break;
					case 20: ac_bonus = 3;break;
					default: break;
				}
				if ( number_percent() < 10 )
				{
					block = number_range( 1, 5 );
				}
			}

			obj->value[ 0 ] = number_range( 1, 2 );
			if ( obj->value[ 0 ] == 1 )
			{
				obj->value[ 1 ] = number_range( 0, 3 );
				obj->value[ 2 ] = number_range( 4, 25 );
				obj->value[ 3 ] = number_range( 0, 5 );
			}
			else
			{
				obj->value[ 1 ] = number_range( 1, 4 );
				obj->value[ 2 ] = number_range( 6, 30 );
				obj->value[ 3 ] = number_range( 0, 8 );
			}

			break;

		case 3:  //tarcze ciezkie
			name = "tarcza";
			name2 = "tarczy";
			name3 = "tarczy";
			name4 = "tarczê";
			name5 = "tarcz±";
			name6 = "tarczy";

			obj->cost = 40;
			obj->weight = 160;
			EXT_SET_BIT( obj->wear_flags2, ITEM_NOTHI );

            gender = GENDER_MESKI;
            size_mod = 2;

			if ( second )
			{
				if ( number_percent() < 33 )
					bash = number_range( 10, 20 );
				if ( number_percent() < 20 )
					block = number_range( 1, 10 );
			}

			if ( third )
			{
				switch ( number_range( 1, 20 ) )
				{
					case 18: ac_bonus = 1;break;
					case 19: ac_bonus = 2;break;
					case 20: ac_bonus = 3;break;
					default: break;
				}
			}

			obj->value[ 0 ] = 3;
			obj->value[ 1 ] = number_range( 1, 5 );
			obj->value[ 2 ] = number_range( 8, 35 );
			obj->value[ 3 ] = number_range( 0, 10 );

			break;

		case 4:  //paweze
			name = "pawê¿";
			name2 = "pawê¿y";
			name3 = "pawê¿y";
			name4 = "pawê¿";
			name5 = "pawê¿±";
			name6 = "pawê¿y";

			gender = GENDER_ZENSKI;
			obj->cost = 40;
			obj->weight = number_range( 160, 200 );
			EXT_SET_BIT( obj->wear_flags2, ITEM_NOTHI );

			if ( second )
			{
				if ( number_percent() < 33 )
					bash = number_range( 5, 20 );
				if ( number_percent() < 20 )
					block = number_range( 1, 15 );
			}

			if ( third )
			{
				switch ( number_range( 1, 20 ) )
				{
					case 18: ac_bonus = 1;break;
					case 19: ac_bonus = 2;break;
					case 20: ac_bonus = 3;break;
					default: break;
				}
			}
			obj->value[ 0 ] = 4;
			obj->value[ 1 ] = number_range( 2, 6 );
			obj->value[ 2 ] = number_range( 10, 45 );
			obj->value[ 3 ] = number_range( 0, 15 );

			break;

		default : break;
	}

    obj->cost += obj->value[ 0 ] * 30; //shield type
    obj->cost += obj->value[ 1 ] * 20; //+AC
    obj->cost += obj->value[ 2 ] * 5; //damage blocked
    obj->cost += obj->value[ 2 ] * 10; //chance to block other attacker

	/* wybor materialu i nie tylko*/
	if ( !set_mat )
		switch ( number_range( 1, 12 ) )
		{
			case 1:
				if ( gender == 1 )
				{
					material = "miedziany";
					mat2 = "miedzianego";
					mat3 = "miedzianemu";
					mat4 = "miedziany";
					mat5 = "miedzianym";
					mat6 = "miedzianym";
				}
				else
				{
					material = "miedziana";
					mat2 = "miedzianej";
					mat3 = "miedzianej";
					mat4 = "miedzian±";
					mat5 = "miedzian±";
					mat6 = "miedzianej";
				}

				obj->material = 5;
				obj->weight += 100;
				break;

			case 2:
				if ( gender == 1 )
				{
					material = "br±zowy";
					mat2 = "br±zowego";
					mat3 = "br±zowemu";
					mat4 = "br±zowy";
					mat5 = "br±zowym";
					mat6 = "br±zowym";
				}
				else
				{
					material = "br±zowa";
					mat2 = "br±zowej";
					mat3 = "br±zowej";
					mat4 = "br±zow±";
					mat5 = "br±zow±";
					mat6 = "br±zowej";
				}

				obj->weight += 300;
				obj->material = 14;
				break;

			case 3:
				if ( gender == 1 )
				{
					material = "¿elazny";
					mat2 = "¿elaznego";
					mat3 = "¿elaznemu";
					mat4 = "¿elazny";
					mat5 = "¿elaznym";
					mat6 = "¿elaznym";
				}
				else
				{
					material = "¿elazna";
					mat2 = "¿elaznej";
					mat3 = "¿elaznej";
					mat4 = "¿elazn±";
					mat5 = "¿elazn±";
					mat6 = "¿elaznej";
				}
				obj->weight += 250;
				obj->material = 13;
				break;

			case 4:
				if ( gender == 1 )
				{
					material = "srebrny";
					mat2 = "srebrnego";
					mat3 = "srebrnemu";
					mat4 = "srebrny";
					mat5 = "srebrnym";
					mat6 = "srebrnym";
				}
				else
				{
					material = "srebrna";
					mat2 = "srebrnej";
					mat3 = "srebrnej";
					mat4 = "srebrn±";
					mat5 = "srebrn±";
					mat6 = "srebrnej";
				}
				obj->material = 10;
				obj->cost += 200/size_mod;
				obj->weight += 200;
				break;

			case 5:
				if ( gender == 1 )
				{
					material = "z³oty";
					mat2 = "z³otego";
					mat3 = "z³otemu";
					mat4 = "z³oty";
					mat5 = "z³otym";
					mat6 = "z³otym";
				}
				else
				{
					material = "z³ota";
					mat2 = "z³otej";
					mat3 = "z³otej";
					mat4 = "z³ot±";
					mat5 = "z³ot±";
					mat6 = "z³otej";
				}

				obj->material = 9;
				obj->cost += 350/size_mod;
				obj->weight += 300;
				break;


			case 6:
				if ( gender == 1 )
				{
					material = "stalowy";
					mat2 = "stalowego";
					mat3 = "stalowemu";
					mat4 = "stalowy";
					mat5 = "stalowym";
					mat6 = "stalowym";
				}
				else
				{
					material = "stalowa";
					mat2 = "stalowej";
					mat3 = "stalowej";
					mat4 = "stalow±";
					mat5 = "stalow±";
					mat6 = "stalowej";
				}

				obj->weight += 200;
				obj->material = 2;
				break;

			case 7:
				if ( gender == 1 )
				{
					material = "b³yszcz±cy";
					mat2 = "b³yszcz±cego";
					mat3 = "b³yszcz±cemu";
					mat4 = "b³yszcz±cy";
					mat5 = "b³yszcz±cym";
					mat6 = "b³yszcz±cym";
				}
				else
				{
					material = "b³yszcz±ca";
					mat2 = "b³yszcz±cej";
					mat3 = "b³yszcz±cej";
					mat4 = "blyszcz±c±";
					mat5 = "b³yszcz±c±";
					mat6 = "b³yszcz±cej";
				}

				obj->weight += 200;
				obj->material = 4;
				obj->cost += 100/size_mod;
				break;

			case 8:
				if ( gender == 1 )
				{
					material = "elfi";
					mat2 = "elfiego";
					mat3 = "elfiemu";
					mat4 = "elfi";
					mat5 = "elfim";
					mat6 = "elfim";
				}
				else
				{
					material = "elfia";
					mat2 = "elfiej";
					mat3 = "elfiej";
					mat4 = "elfi±";
					mat5 = "elfi±";
					mat6 = "elfiej";
				}

				obj->material = 3;
				obj->weight += 10;
				obj->cost += 200/size_mod;
				break;

			case 9:
				if ( gender == 1 )
				{
					material = "trójk±tny herbowy";
					mat2 = "trójk±tnym herbowym";
					mat3 = "trójk±tnemu herbowemu";
					mat4 = "trójk±tny herbowy";
					mat5 = "trójk±tnym herbowym";
					mat6 = "trójk±tnym herbowym";
				}
				else
				{
					material = "trójk±tna herbowa";
					mat2 = "trójk±tnej herbowej";
					mat3 = "trójk±tnej herbowej";
					mat4 = "trójk±tn± herbow±";
					mat5 = "trójk±tn± herbow±";
					mat6 = "trójk±tnej herbowej";
				}

				obj->material = number_range( 1, 5 );
				obj->cost += 150/size_mod;
				obj->weight += 50;
				break;

			case 10:
				if ( gender == 1 )
				{
					material = "mithrilowy";
					mat2 = "mithrilowego";
					mat3 = "mithrilowemu";
					mat4 = "mithrilowy";
					mat5 = "mithrilowym";
					mat6 = "mithrilowym";
				}
				else
				{
					material = "mithrilowa";
					mat2 = "mithrilowej";
					mat3 = "mithrilowej";
					mat4 = "mithrilow±";
					mat5 = "mithrilow±";
					mat6 = "mithrilowej";
				}

				obj->material = 8;
				obj->cost += 750/size_mod;
				obj->weight += 50;
				break;

			case 11:
				if ( gender == 1 )
				{
					material = "³uskowy";
					mat2 = "³uskowego";
					mat3 = "³uskowemu";
					mat4 = "³uskowy";
					mat5 = "³uskowym";
					mat6 = "³uskowym";
				}
				else
				{
					material = "³uskowa";
					mat2 = "³uskowej";
					mat3 = "³uskowej";
					mat4 = "³uskow±";
					mat5 = "³uskow±";
					mat6 = "³uskowej";
				}

				obj->material = 41;
				obj->cost += 150/size_mod;
				obj->weight += 50;
				break;
			case 12:
				if ( gender == 1 )
				{
					material = "okuty";
					mat2 = "okutego";
					mat3 = "okutemu";
					mat4 = "okuty";
					mat5 = "okutym";
					mat6 = "okutym";
				}
				else
				{
					material = "okuta";
					mat2 = "okutej";
					mat3 = "okutej";
					mat4 = "okut±";
					mat5 = "okut±";
					mat6 = "okutej";
				}

				obj->material = 41;
				obj->cost += 150/size_mod;
				obj->weight += 50;
				break;
		}

	//no i tutaj bonusy do dama albo hita przy jebnieciu tarcza
	bonus = number_range( -500, 500 );
	if ( bonus == 500 )
		obj->value[4] = 5;
	else if ( bonus < 500 && bonus >= 470 )
		obj->value[4] = 4;
	else if ( bonus < 470 && bonus >= 420 )
		obj->value[4] = 3;
	else if ( bonus < 420 && bonus >= 300 )
		obj->value[4] = 2;
	else if ( bonus < 300 && bonus >= 100 )
		obj->value[4] = 1;
	else if ( bonus < 100 && bonus >= -100 )
		obj->value[4] = 0;
	else if ( bonus < -100 && bonus >= -300 )
		obj->value[4] = -1;
	else if ( bonus < -300 && bonus >= -420 )
		obj->value[4] = -2;
	else if ( bonus < -420 && bonus >= -470 )
		obj->value[4] = -3;
	else if ( bonus < -470 && bonus >= 499 )
		obj->value[4] = -4;
	else if ( bonus == -500 )
		obj->value[4] = -5;

	bonus = number_range( -500, 500 );
	if ( bonus == 500 )
		obj->value[5] = 5;
	else if ( bonus < 500 && bonus >= 470 )
		obj->value[5] = 4;
	else if ( bonus < 470 && bonus >= 420 )
		obj->value[5] = 3;
	else if ( bonus < 420 && bonus >= 300 )
		obj->value[5] = 2;
	else if ( bonus < 300 && bonus >= 100 )
		obj->value[5] = 1;
	else if ( bonus < 100 && bonus >= -100 )
		obj->value[5] = 0;
	else if ( bonus < -100 && bonus >= -300 )
		obj->value[5] = -1;
	else if ( bonus < -300 && bonus >= -420 )
		obj->value[5] = -2;
	else if ( bonus < -420 && bonus >= -470 )
		obj->value[5] = -3;
	else if ( bonus < -470 && bonus >= 499 )
		obj->value[5] = -4;
	else if ( bonus == -500 )
		obj->value[5] = -5;

	sprintf( text, "%s %s", material, name );

	free_string( obj->name );
	obj->name = str_dup( text );

	free_string( obj->short_descr );
	obj->short_descr = str_dup( text );

	free_string( obj->name2 );
	sprintf( text, "%s %s", mat2, name2 );
	obj->name2 = str_dup( text );

	free_string( obj->name3 );
	sprintf( text, "%s %s", mat3, name3 );
	obj->name3 = str_dup( text );

	free_string( obj->name4 );
	sprintf( text, "%s %s", mat4, name4 );
	obj->name4 = str_dup( text );

	free_string( obj->name5 );
	sprintf( text, "%s %s", mat5, name5 );
	obj->name5 = str_dup( text );

	free_string( obj->name6 );
	sprintf( text, "%s %s", mat6, name6 );
	obj->name6 = str_dup( text );


	sprintf( text, "%s %s le¿y tutaj.", capitalize( material ), name );
	sprintf( text, "%s", capitalize( text ) );
	free_string( obj->description );
	obj->description = str_dup( text );

	/* ok tu walimy skilla */

	if ( bash > 0 )
	{
		if ( ac_bonus > 0 || block > 0 )
			end = FALSE;
		else
			end = TRUE;

		add_new_affect( obj, gsn_bash, APPLY_SKILL, gsn_bash, bash, &AFF_NONE, -1, end );
		obj->enchanted = TRUE;
		obj->cost += ( bash * 75 + 12 * number_range( 1, 20 ) );
	}

	//shield_block
	if ( block > 0 )
	{
		if ( ac_bonus > 0 )
			end = FALSE;
		else
			end = TRUE;

		add_new_affect( obj, gsn_shield_block, APPLY_SKILL, gsn_shield_block, block, &AFF_NONE, -1, end );
		obj->enchanted = TRUE;
		obj->cost += ( block * 50 + 11 * number_range( 1, 20 ) );
	}


	/* a tu ac bonus */
	if ( ac_bonus > 0 )
	{
		add_new_affect( obj, 0, APPLY_AC, -ac_bonus * 10, 0, &AFF_NONE, -1, TRUE );
		obj->enchanted = TRUE;
		obj->cost += ( ac_bonus * 300 + 20 * number_range( 1, 20 ) );

	}

	if (number_range( 1, 50 ) < 2 )
	{
		elem = number_range( 1, 10 );
		pAf = new_affect();
		pAf->location = APPLY_RESIST;
		pAf->where = TO_AFFECTS;
		pAf->type = -1;
		pAf->duration = -1;
		pAf->rt_duration = 0;
		pAf->level = elem;
		pAf->next = obj->affected;

		switch ( number_range( 1, 11 ) )
		{
			case 1:
				pAf->modifier = RESIST_FIRE;
				break;
			case 2:
				pAf->modifier = RESIST_COLD;
				break;
			case 3:
				pAf->modifier = RESIST_ACID;
				break;
			case 4:
				pAf->modifier = RESIST_ELECTRICITY;
				break;
			case 5:
				pAf->modifier = RESIST_NEGATIVE;
				break;
			case 6:
				pAf->modifier = RESIST_SUMMON;
				break;
			case 7:
				pAf->modifier = RESIST_MENTAL;
				break;
			case 8:
				pAf->modifier = RESIST_POISON;
				break;
			case 9:
				pAf->modifier = RESIST_FEAR;
				break;
			case 10:
				pAf->modifier = RESIST_CHARM;
				break;
			case 11:
				pAf->modifier = RESIST_HOLY;
				break;
			default:
				break;
		}
		obj->affected = pAf;
		obj->cost += elem * COST_APPLY_RESIST;
		//rellik, zmniejszam ostateczny koszt randomowych tarcz [20080510]
		//rellik, jako¶ nie widaæ tych ni¿szych cen, dam drastyczniej [20080630]
		obj->cost -= ( obj->cost / 2 );
	}

	return;
}

void generate_ring( OBJ_DATA *obj, int* points, bool perm )
{
    char text[ MAX_INPUT_LENGTH ], *name = "(null)", *sh_desc = "(null)";
    char *name2 = "(null)", *name3 = "(null)", *name4 = "(null)", *name5 = "(null)", *name6 = "(null)", *item_description = "";
    char *subjective, *color;
    int loc = 0, mod = 0, level = 0, type = 0, multiple_effect = 1;
    BITVECT_DATA *bitv = &AFF_NONE;

    generate_extra( obj );

    obj->value[ 0 ] = 0;
    obj->value[ 1 ] = 0;
    obj->value[ 2 ] = 0;
    obj->value[ 3 ] = 0;
    obj->cost = 750;
    obj->weight = 1;

    /* wybor materialu i nazwy*/
    switch ( number_range( 1, 20 ) )
    {
        case 1:
        case 2:
            name = "stalowy pier¶cieñ";
            sh_desc = "stalowy pier¶cieñ";
            name2 = "stalowego pier¶cienia";
            name3 = "stalowemu pier¶cieniowi";
            name4 = "stalowy pier¶cieñ";
            name5 = "stalowym pier¶cieniem";
            name6 = "stalowym pier¶cieniu";
            obj->cost /= 2;
            obj->material = 2;
            item_description = "Stalowy kr±¿ek b³yszczy moc±.";
            break;

        case 3:
        case 4:
            name = "miedziany pier¶cieñ";
            sh_desc = "miedziany pier¶cieñ";
            name2 = "miedzianego pier¶cienia";
            name3 = "miedzianemu pier¶cieniowi";
            name4 = "miedziany pier¶cieñ";
            name5 = "miedzianym pier¶cieniem";
            name6 = "miedzianym pier¶cieniu";
            obj->material = 5;
            item_description = "Miedziany kr±¿ek b³yszczy moc±.";
            break;

        case 5:
            name = "z³ota obr±czka";
            sh_desc = "z³ota obr±czka";
            name2 = "z³otej obr±czki";
            name3 = "z³otej obr±czce";
            name4 = "z³ot± obr±czkê";
            name5 = "z³ot± obr±czk±";
            name6 = "z³otej obr±czce";
            obj->material = 9;
            obj->cost += 250;
            item_description = "Wykonana ze z³ota obr±czka b³yszczy moc±.";
            break;

        case 6:
            name = "srebrny pier¶cieñ";
            sh_desc = "srebrny pier¶cieñ";
            name2 = "srebrnego pier¶cienia";
            name3 = "srebrnemu pier¶cieniowi";
            name4 = "srebrny pier¶cieñ";
            name5 = "srebrnym pier¶cieniem";
            name6 = "srebrnym pier¶cieniu";
            obj->material = 10;
            obj->cost += 100;
            item_description = "Srebrny kr±¿ek b³yszczy moc±.";
            break;

        case 7:
        case 8:
            name = "ko¶ciany pier¶cieñ";
            sh_desc = "ko¶ciany pier¶cieñ";
            name2 = "ko¶cianego pier¶cienia";
            name3 = "ko¶cianemu pier¶cieniowi";
            name4 = "ko¶ciany pier¶cieñ";
            name5 = "ko¶cianym pier¶cieniem";
            name6 = "ko¶cianym pier¶cieniu";
            obj->material = 21;
            obj->cost += 50;
            item_description = "Ko¶ciany pier¶cieñ delikatnie pulsuje moc±.";
            break;

        case 9:
            name = "adamantowy pier¶cieñ";
            sh_desc = "adamantowy pier¶cieñ";
            name2 = "adamantowego pier¶cienia";
            name3 = "adamantowemu pier¶cieniowi";
            name4 = "adamantowy pier¶cieñ";
            name5 = "adamantowym pier¶cieniem";
            name6 = "adamantowym pier¶cieniu";
            obj->material = 7;
            obj->cost += 50;
            item_description = "Adamantowy kr±¿ek b³yszczy moc±.";
            break;

        case 10:
            name = "kryszta³owy pier¶cieñ";
            sh_desc = "kryszta³owy pier¶cieñ";
            name2 = "kryszta³owego pier¶cienia";
            name3 = "kryszta³owemu pier¶cieniowi";
            name4 = "kryszta³owy pier¶cieñ";
            name5 = "kryszta³owym pier¶cieniem";
            name6 = "kryszta³owym pier¶cieniu";
            obj->material = 11;
            obj->cost += 20;
            item_description = "Widzisz tysi±ce refleksów w fasetkach kryszta³owego pier¶cienia.";
            break;

        case 11:
        case 12:
            name = "¿elazny pier¶cieñ";
            sh_desc = "¿elazny pier¶cieñ";
            name2 = "¿elaznego pier¶cienia";
            name3 = "¿elaznemu pier¶cieniowi";
            name4 = "¿elazny pier¶cieñ";
            name5 = "¿elaznym pier¶cieniem";
            name6 = "¿elaznym pier¶cieniu";
            obj->cost /= 2;
            obj->material = 13;
            item_description = "¯elazny pier¶cieñ wygl±da do¶æ topornie.";
            break;

        case 13:
            name = "platynowy pier¶cieñ";
            sh_desc = "platynowy pier¶cieñ";
            name2 = "platynowego pier¶cienia";
            name3 = "platynowemu pier¶cieniowi";
            name4 = "platynowy pier¶cieñ";
            name5 = "platynowym pier¶cieniem";
            name6 = "platynowym pier¶cieniu";
            obj->material = 34;
            obj->cost *= 2;
            item_description = "Delikatnie rze¼biony, platynowy pier¶cieñ.";
            break;

        case 14:
        case 15:
            name = "o³owiany pier¶cieñ";
            sh_desc = "o³owiany pier¶cieñ";
            name2 = "o³owianego pier¶cienia";
            name3 = "o³owianemu pier¶cieniowi";
            name4 = "o³owiany pier¶cieñ";
            name5 = "o³owianym pier¶cieniem";
            name6 = "o³owianym pier¶cieniu";
            obj->material = 28;
            obj->weight += 4;
            item_description = "Do¶æ ciê¿ki o³owiany pier¶cieñ.";
            break;

        case 16:
        case 17:
            name = "drewniany pier¶cieñ";
            sh_desc = "drewniany pier¶cieñ";
            name2 = "drewnianego pier¶cienia";
            name3 = "drewnianemu pier¶cieniowi";
            name4 = "drewniany pier¶cieñ";
            name5 = "drewnianym pier¶cieniem";
            name6 = "drewnianym pier¶cieniu";
            obj->material = 1; //drewno
            obj->cost /= 3;
            obj->weight /= 2;
            item_description = "Na pier¶cieniu widaæ fantazyjne s³oje.";
            break;

        case 18:
            name = "marmurowy pier¶cieñ";
            sh_desc = "marmurowy pier¶cieñ";
            name2 = "marmurowego pier¶cienia";
            name3 = "marmurowemu pier¶cieniowi";
            name4 = "marmurowy pier¶cieñ";
            name5 = "marmurowym pier¶cieniem";
            name6 = "marmurowym pier¶cieniu";
            obj->material = 22; //marmur
            obj->weight += 2;
            item_description = "Marmurowy kr±¿ek b³yszczy moc±.";
            break;

        case 19:
            name = "porcelanowy pier¶cieñ";
            sh_desc = "porcelanowy pier¶cieñ";
            name2 = "porcelanowego pier¶cienia";
            name3 = "porcelanowemu pier¶cieniowi";
            name4 = "porcelanowy pier¶cieñ";
            name5 = "porcelanowym pier¶cieniem";
            name6 = "porcelanowym pier¶cieniu";
            obj->material = 31; //porcelana
            item_description = "Delikatny, porcelanowy pier¶cieñ z misternym malunkiem.";
            break;

        case 20:
            name = "illittowy pier¶cieñ";
            sh_desc = "illittowy pier¶cieñ";
            name2 = "illittowego pier¶cienia";
            name3 = "illittowemu pier¶cieniowi";
            name4 = "illittowy pier¶cieñ";
            name5 = "illittowym pier¶cieniem";
            name6 = "illittowym pier¶cieniu";
            obj->material = 33;
            obj->cost += 60;
            item_description = "Illittowy kr±¿ek b³yszczy moc±.";
            break;

    }

    /**
     * APPLY
     */
    switch ( number_range( 0, 6 ) )
    {
        case 0:
            switch(number_range(0,6))
            {
                case 0:
                    loc = APPLY_STR;
                    break;
                case 1:
                    loc = APPLY_DEX;
                    break;
                case 2:
                    loc = APPLY_INT;
                    break;
                case 3:
                    loc = APPLY_WIS;
                    break;
                case 4:
                    loc = APPLY_CON;
                    break;
                case 5:
                    loc = APPLY_CHA;
                    break;
                case 6:
                    loc = APPLY_LUC;
                    break;
            }
            mod = number_range( 1, 3 );
            obj->cost += number_range( 900, 1200 ) * mod;
            break;

        case 1:
            loc = APPLY_HIT;
            mod = number_range( 1, 10 );
            obj->cost += number_range( 75, 120 ) * mod;
            break;

        case 2:
            loc = APPLY_AC;
            mod = -10 * number_range( 1, 3 );
            obj->cost -= number_range( 250, 300 ) * mod;
            break;

        case 3:
            if ( number_percent() > 95 )
            {
                mod = 2;
            }
            else if ( number_percent() > 80 )
            {
                mod = 1;
            }
            else
            {
                mod = -1;
            }
            if (number_percent()> 50)
            {
                loc = APPLY_DAMROLL;
                obj->cost += number_range( 500, 750 ) * mod;
            }
            else
            {
                loc = APPLY_HITROLL;
                obj->cost += number_range( 400, 650 ) * mod;
            }
            break;

        case 4:
            loc = APPLY_MOVE;
            mod = number_range( 1, 30 );
            obj->cost += COST_APPLY_SKILL * UMIN( 10, mod );
            break;

        case 5:
            loc = APPLY_MEMMING;
            mod = number_range( 1, 7 );
            switch ( mod )
            {
                case 7: level = number_range( 1, 5 );break;
                case 6: level = number_range( 1, 4 );break;
                case 5: level = number_range( 1, 3 );break;
                case 4: level = number_range( 1, 3 );break;
                case 3: level = number_range( 1, 3 );break;
                case 2: level = number_range( 1, 2 );break;
                case 1: level = number_range( 1, 2 );break;
            }
            obj->cost += number_range( 40, 60 ) * ( mod * 10 + level );
            break;

            /**
             * weapon
             */
        case 6:
            loc = APPLY_SKILL;
            switch ( number_range( 0, 11 ) )
            {
                case  0: mod = gsn_axe;break;
                case  1: mod = gsn_claws;break;
                case  2: mod = gsn_dagger;break;
                case  3: mod = gsn_flail;break;
                case  4: mod = gsn_mace;break;
                case  5: mod = gsn_polearm;break;
                case  6: mod = gsn_polearm;break;
                case  7: mod = gsn_shortsword;break;
                case  8: mod = gsn_spear;break;
                case  9: mod = gsn_staff;break;
                case 10: mod = gsn_sword;break;
                case 11: mod = gsn_whip;break;
            }
            level = number_range( 1, 10 );
            obj->cost += COST_APPLY_SKILL * level;
            break;

            /**
             * SKILLZ
             */
        case 7:
            loc = APPLY_SKILL;
            level = number_range( 1, 10 );
            switch ( number_range( 0, 20 ) )
            {
                case  0: mod = gsn_axe_mastery;break;
                case  1: mod = gsn_backstab; break;
                case  2: mod = gsn_bash; break;
                case  3: mod = gsn_dagger_mastery;break;
                case  4: mod = gsn_disarm; break;
                case  5: mod = gsn_dodge; break;
                case  6: mod = gsn_flail_mastery;break;
                case  7: mod = gsn_hide;level+=number_range(0,10);break;
                case  8: mod = gsn_kick;break;
                case  9: mod = gsn_mace_mastery;break;
                case 10: mod = gsn_parry; break;
                case 11: mod = gsn_pick_lock; break;
                case 12: mod = gsn_polearm_mastery;break;
                case 13: mod = gsn_shortsword_mastery;break;
                case 14: mod = gsn_sneak;level+=number_range(0,10);break;
                case 15: mod = gsn_spear_mastery;break;
                case 16: mod = gsn_staff_mastery;break;
                case 17: mod = gsn_steal;level+=number_range(0,10);break;
                case 18: mod = gsn_sword_mastery;break;
                case 19: mod = gsn_trip; break;
                case 20: mod = gsn_whip_mastery;break;
            }
            obj->cost += COST_APPLY_SKILL * level;
            break;
    }

    if ( loc == APPLY_HIT && number_percent() < 11 )
    {
        multiple_effect = number_range(5, 10);
        switch (number_range(1, 2))
        {
            case 1:
                subjective = " ¿ycia{x";
                break;
            case 2:
                subjective = " krwi{x";
                break;
        }
        color = "{R";
    }
    else if ( loc == APPLY_MOVE && number_percent() < 11 )
    {
        multiple_effect = number_range(2, 10);
        switch (number_range(1, 2))
        {
            case 1:
        subjective = " lekko¶ci{x";
                break;
            case 2:
                subjective = " wigoru{x";
                break;
        }
        color = "{g";
    }
    else
    {
        multiple_effect = 1;
        subjective = "";
        color = "";
    }
    mod *= multiple_effect;

    /**
     * wytwarzenie nazw przedmiotu
     */

    free_string( obj->short_descr );
    sprintf ( text, "%s%s%s", color, sh_desc, subjective );
    obj->short_descr = str_dup( text );

    free_string( obj->name );
    sprintf ( text, "%s%s%s", color, name, subjective );
    obj->name = str_dup( text );

    free_string( obj->name2 );
    sprintf ( text, "%s%s%s", color, name2, subjective );
    obj->name2 = str_dup( text );

    free_string( obj->name3 );
    sprintf ( text, "%s%s%s", color, name3, subjective );
    obj->name3 = str_dup( text );

    free_string( obj->name4 );
    sprintf ( text, "%s%s%s", color, name4, subjective );
    obj->name4 = str_dup( text );

    free_string( obj->name5 );
    sprintf ( text, "%s%s%s", color, name5, subjective );
    obj->name5 = str_dup( text );

    free_string( obj->name6 );
    sprintf ( text, "%s%s%s", color, name6, subjective );
    obj->name6 = str_dup( text );

    free_string( obj->item_description );
    obj->item_description = str_dup( item_description );

    free_string( obj->description );
    sprintf( text, "%s pob³yskuje ukryt± moc±.", capitalize( sh_desc ) );
    obj->description = str_dup( text );

    bitv = get_bitvector();
    obj->cost += get_cost_of_bitvector(bitv);

    obj->enchanted = TRUE;

    add_new_affect( obj, type, loc, mod, level, bitv, generate_aff_duration( points, &obj->cost ), TRUE );

    if (obj->cost < 1)
    {
        obj->cost = number_range(1, 10);;
    }

    return;
}
//Bede amulety generowane losowo, lub z malym prawdopodobienstwem jakies niezle
void generate_amulet( OBJ_DATA *obj, int* points )
{
	/* tu nie wiem co */

	char txt[ MAX_INPUT_LENGTH ];
	char text[ MAX_INPUT_LENGTH ], *name = "(null)", *sh_desc = "(null)";
	char *name2 = "(null)", *name3 = "(null)", *name4 = "(null)", *name5 = "(null)", *name6 = "(null)";
	char *name_des = "n", *sh_des = "n", *name2_des = "n", *name3_des = "n", *name4_des = "n", *name5_des = "n", *name6_des = "n";
	bool end = FALSE, bylo = FALSE, losowy = FALSE;
	int los_tab[ 5 ], koszt = 0;
	int type = 0, loc = 0, mod = 0, level = 0, c = 0, mat = 0, number = 0, anumber = 0, a = 0, los = 0, z, val;
	BITVECT_DATA * bitv = &AFF_NONE;

	for ( a = 0;a < 6;a++ ) los_tab[ a ] = 0;

	a = 0;

	generate_extra( obj );

	obj->value[ 0 ] = 0;
	obj->value[ 1 ] = 0;
	obj->value[ 2 ] = 0;
	obj->value[ 3 ] = 0;
	obj->cost = 20;
	obj->weight = 1;

/*
								 | str | con | dex | int | vis | cha | luc | extra
	 1: amulet czarnej rêki      |  -1 |     |  +2 |     |     |     |     | +10 gsn_dagger
	 2: amulet czarnej d³oni     |  +1 |     |  -2 |     |     |     |     |
	 3: wê¿owy amulet si³y       |  +1 |     |     |     |     |     |     | sneak
	 4: wê¿owy amulet            |     |     |  -2 |     |     |     |     | slow
	 5: fascinum szybko¶ci       |     |     |     |     |     |     |  -2 | haste
	 6: talizman os³ony          |     |     |  +1 |     |     |     |     | -20 AC
	 7: wisior ataku lwa         |  +1 |     |     |     |     |     |     | +10 gsn_disarm
	 8: amulet przeczucia        |     |     |     |     |  +1 |     |     | det: evil, good
	 9: amulet wyczucia          |     |     |     |     |  -1 |     |     |
	10: krasnoludzki amulet si³y |  +1 |  +1 |  -1 |     |     |     |     | +5 gsn_axe
	11: krasnoludzki amulet      |  +2 |  +2 |  -1 |  -1 |  -1 |  -1 |  -1 | -5 gsn_axe
	12: fetysz ¿ywio³ów          |     |     |     |     |     |     |     | spell_mem losowanie
	13: amulet ¿ywio³ów          |     |     |     |  -1 |  -1 |     |     |
	14: oko boga                 |     |     |     |     |     |     |     | detect_invis
	15: oko bo¿ka                |     |     |     |     |     |     |     | blind
	16: kamieñ ol¶nienia         |     |     |     |  +1 |     |     |     | comprehend languages
	17: kamieñ l¶nienia          |     |     |     |  -1 |  -1 |     |     |
	18: amulet z dwoma mieczami  |  +1 |     |     |     |     |     |     | +10 gsn_sword
*/

	//pierwszy -s-w-i-t-c-h-(-)- -> nazwy i normy potrzebne do stworzenia
	switch ( c = number_range( 1, 10000 ) )
	{ //no powiedzmy mozna zmienic
			//wpierw rzadkie, prawdobodobnie ze stalymi affektami :)
		case 1:
			name = "amulet czarnej rêki";
			sh_desc = "amulet czarnej rêki";
			name2 = "amuletu czarnej rêki";
			name3 = "amuletowi czarnej rêki";
			name4 = "amulet czarnej rêki";
			name5 = "amuletem czarnej rêki";
			name6 = "amulecie czarnej rêki";
			break;
		case 2:
			name = "amulet czarnej d³oni";
			sh_desc = "amulet czarnej d³oni";
			name2 = "amuletu czarnej d³oni";
			name3 = "amuletowi czarnej d³oni";
			name4 = "amulet czarnej d³oni";
			name5 = "amuletem czarnej d³oni";
			name6 = "amulecie czarnej d³oni";
			break;
		case 3:
			name = "wê¿owy amulet si³y";
			sh_desc = "wê¿owy amulet si³y";
			name2 = "wê¿owego amuletu si³y";
			name3 = "wê¿owemu amuletowi si³y";
			name4 = "wê¿owy amulet si³y";
			name5 = "wê¿owym amuletem si³y";
			name6 = "wê¿owym amulecie si³y";
			break;
		case 4:
			name = "wê¿owy amulet";
			sh_desc = "wê¿owy amulet";
			name2 = "wê¿owego amuletu";
			name3 = "wê¿owemu amuletowi";
			name4 = "wê¿owy amulet";
			name5 = "wê¿owym amuletem";
			name6 = "wê¿owym amulecie";
			break;
		case 5:
			name = "fascinum szybk¶oci";
			sh_desc = "fascinum szybko¶ci";
			name2 = "fascinum szybko¶ci";
			name3 = "fascinum szybko¶ci";
			name4 = "fascinum szybko¶ci";
			name5 = "fascinum szybko¶ci";
			name6 = "fascinum szybko¶ci";
			break;
		case 6:
			name = "talizman os³ony";
			sh_desc = "talizman os³ony";
			name2 = "talizmanu os³ony";
			name3 = "talizmanie os³ony";
			name4 = "talizman os³ony";
			name5 = "talizmanem os³ony";
			name6 = "talizmanie os³ony";
			break;
		case 7:
			name = "wisior ataku lwa";
			sh_desc = "wisior ataku lwa";
			name2 = "wisiora ataku lwa";
			name3 = "wisiorowi ataku lwa";
			name4 = "wisior ataku lwa";
			name5 = "wisiorem ataku lwa";
			name6 = "wisiorze ataku lwa";
			break;
		case 8:
			name = "amulet przeczucia";
			sh_desc = "amulet przeczucia";
			name2 = "amuletu przeczucia";
			name3 = "amuletowi przeczucia";
			name4 = "amulet przeczucia";
			name5 = "amuletem przeczucia";
			name6 = "amulecie przeczucia";
			break;
		case 9:
			name = "amulet wyczucia";
			sh_desc = "amulet wyczucia";
			name2 = "amuletu wyczucia";
			name3 = "amuletowi wyczucia";
			name4 = "amulet wyczucia";
			name5 = "amuletem wyczucia";
			name6 = "amulecie wyczucia";
			break;
		case 10:
			name = "krasnoludzki amulet si³y";
			sh_desc = "krasnoludzki amulet si³y";
			name2 = "krasnoludzkiego amuletu si³y";
			name3 = "krasnoludzkiemu amuletowi si³y";
			name4 = "krasnoludzki amulet si³y";
			name5 = "krasnoludzkim amuletem si³y";
			name6 = "krasnoludzki amulecie si³y";
			break;
		case 11:
			name = "krasnoludzki amulet";
			sh_desc = "krasnoludzki amulet";
			name2 = "krasnoludzkiego amuletu";
			name3 = "krasnoludzkiemu amuletowi";
			name4 = "krasnoludzki amulet";
			name5 = "krasnoludzkim amuletem";
			name6 = "krasnoludzki amulecie";
			break;
		case 12: //fetysz ¿ywio³ów
			name = "fetysz ¿ywio³ów";
			sh_desc = "fetysz ¿ywio³ów";
			name2 = "fetysza ¿ywio³ów";
			name3 = "fetyszowi ¿ywio³ów";
			name4 = "fetysz ¿ywio³ów";
			name5 = "fetyszem ¿ywio³ów";
			name6 = "fetyszu ¿ywio³ów";
			break;
		case 13: //amulet ¿ywio³ów
			name = "amulet ¿ywio³ów";
			sh_desc = "amulet ¿ywio³ów";
			name2 = "amuleta ¿ywio³ów";
			name3 = "amuletowi ¿ywio³ów";
			name4 = "amulet ¿ywio³ów";
			name5 = "amuletem ¿ywio³ów";
			name6 = "amuletu ¿ywio³ów";
			break;
		case 14: //oko boga
			name = "oko boga";
			sh_desc = "oko boga";
			name2 = "oku boga";
			name3 = "oku boga";
			name4 = "oko boga";
			name5 = "okiem boga";
			name6 = "oku boga";
			break;
		case 15: //oko bo¿ka
			name = "oko bo¿ka";
			sh_desc = "oko bo¿ka";
			name2 = "oku bo¿ka";
			name3 = "oku bo¿ka";
			name4 = "oko bo¿ka";
			name5 = "okiem bo¿ka";
			name6 = "oku bo¿ka";
			break;
		case 16: //kamieñ olnienia
			name = "kamieñ ol¶nienia";
			sh_desc = "kamieñ ol¶nienia";
			name2 = "kamieniowi ol¶nienia";
			name3 = "kamienia ol¶nienia";
			name4 = "kamieñ ol¶nienia";
			name5 = "kamieniem ol¶nienia";
			name6 = "kamieniu ol¶nienia";
			break;
		case 17: //kamieñ l¶nienia
			name = "kamieñ l¶nienia";
			sh_desc = "kamieñ l¶nienia";
			name2 = "kamieniowi l¶nienia";
			name3 = "kamienia l¶nienia";
			name4 = "kamieñ l¶nienia";
			name5 = "kamieniem l¶nienia";
			name6 = "kamieniu l¶nienia";
			break;
		case 18: //amulet z dwoma mieczami
			name = "amulet dwa miecze";
			sh_desc = "amulet z dwoma mieczami";
			name2 = "amuletu z dwoma mieczami";
			name3 = "amuletowi z dwoma mieczami";
			name4 = "amulet z dwoma mieczami";
			name5 = "amuletem z dwoma mieczami";
			name6 = "amulecie z dwoma mieczami";
			break;

		default: //czyli kompletnie losowy amulet
			losowy = TRUE;
			switch ( number_range( 1, 7 ) )
			{
				case 1:
					name = "amulet";
					sh_desc = "amulet";
					name2 = "amuletu";
					name3 = "amuletowi";
					name4 = "amulet";
					name5 = "amuletem";
					name6 = "amulecie";
					break;

				case 2:
					name = "medalion";
					sh_desc = "medalion";
					name2 = "medalionu";
					name3 = "medalionowi";
					name4 = "medalion";
					name5 = "medalionem";
					name6 = "medalionie";
					break;
				case 3:
					name = "wisior";
					sh_desc = "wisior";
					name2 = "wisiora";
					name3 = "wisiorowi";
					name4 = "wisior";
					name5 = "wisiorem";
					name6 = "wisiorze";
					break;
				case 4:
					name = "naszyjnik";
					sh_desc = "naszyjnik";
					name2 = "naszyjnika";
					name3 = "naszyjnikowi";
					name4 = "naszyjnik";
					name5 = "naszyjnikiem";
					name6 = "naszyjniku";
					break;
				case 5:
					name = "talizman";
					sh_desc = "talizman";
					name2 = "talizmanu";
					name3 = "talizmanowi";
					name4 = "talizman";
					name5 = "talizmanem";
					name6 = "talizmanie";
					break;
				case 6:
					name = "symbol";
					sh_desc = "symbol";
					name2 = "symbolu";
					name3 = "symbolowi";
					name4 = "symbol";
					name5 = "symbolem";
					name6 = "symbolunie";
					break;
				case 7:
					name = "wisiorek";
					sh_desc = "wisiorek";
					name2 = "wisiorka";
					name3 = "wisiorkowi";
					name4 = "wisiorek";
					name5 = "wisiorkiem";
					name6 = "wisiorku";
					break;
			}
			//rodzaj materialu
			while ( mat < 1 ) mat = number_range( 1, 7 ) - number_range( 0, 3 );

			switch ( mat )
			{
				case 1: //za +0
					switch ( number_range( 1, 4 ) )
					{
						case 1: //miedz
							obj->material = 5;
							obj->cost += 40;
							sh_des = "miedziany";
							name_des = "miedziany";
							name2_des = "miedzianego";
							name3_des = "miedzianemu";
							name4_des = "miedziany";
							name5_des = "miedzianym";
							name6_des = "miedzianym";
							break;
						case 2: //kosc
							obj->material = 21;
							obj->cost += 10;
							sh_des = "ko¶ciany";
							name_des = "ko¶ciany";
							name2_des = "ko¶cianego";
							name3_des = "ko¶cianemu";
							name4_des = "ko¶ciany";
							name5_des = "ko¶cianym";
							name6_des = "ko¶cianym";
							break;
						case 3: //braz
							obj->material = 14;
							obj->cost += 30;
							sh_des = "br±zowy";
							name_des = "br±zowy";
							name2_des = "br±zowego";
							name3_des = "br±zowemu";
							name4_des = "br±zowy";
							name5_des = "br±zowym";
							name6_des = "br±zowym";
							break;
						case 4: //kamien
							obj->material = 16;
							obj->cost += 20;
							sh_des = "kamienny";
							name_des = "kamienny";
							name2_des = "kamiennego";
							name3_des = "kamiennemu";
							name4_des = "kamienny";
							name5_des = "kamiennym";
							name6_des = "kamiennym";
							break;
					}
					break;
				case 2:
					switch ( number_range( 1, 2 ) )
					{
						case 1: //zelazo
							obj->material = 13;
							obj->cost += 80;
							sh_des = "¿elazny";
							name_des = "¿elazny";
							name2_des = "¿elaznego";
							name3_des = "¿elaznemu";
							name4_des = "¿elazny";
							name5_des = "¿elaznym";
							name6_des = "¿elaznym";
							break;
						case 2: //stal
							obj->material = 2;
							obj->cost += 100;
							sh_des = "stalowy";
							name_des = "stalowy";
							name2_des = "stalowego";
							name3_des = "stalowemu";
							name4_des = "stalowy";
							name5_des = "stalowym";
							name6_des = "stalowym";
							break;
					}
					break;
				case 3: //stal doskonala
					obj->cost += 120;
					obj->material = 4;
					sh_des = "stalowy";
					name_des = "stalowy";
					name2_des = "stalowego";
					name3_des = "stalowemu";
					name4_des = "stalowy";
					name5_des = "stalowym";
					name6_des = "stalowym";
					break;
				case 4:
					switch ( number_range( 1, 3 ) )
					{
						case 1: //elfia stal
							obj->material = 2;
							obj->cost += 130;
							sh_des = "stalowy";
							name_des = "stalowy";
							name2_des = "stalowego";
							name3_des = "stalowemu";
							name4_des = "stalowy";
							name5_des = "stalowym";
							name6_des = "stalowym";
							break;
						case 2: //srebro
							obj->material = 10;
							obj->cost += 140;
							sh_des = "srebrny";
							name_des = "srebrny";
							name2_des = "srebrnego";
							name3_des = "srebrnemu";
							name4_des = "srebrny";
							name5_des = "srebrnym";
							name6_des = "srebrnym";
							break;
						case 3: //szklo
							obj->material = 15;
							obj->cost += 150;
							sh_des = "szklany";
							name_des = "szklany";
							name2_des = "szklanego";
							name3_des = "szklanemu";
							name4_des = "szklany";
							name5_des = "szklanym";
							name6_des = "szklanym";
							break;
					}
					break;
				case 5: //zloto
					obj->cost += 250;
					obj->material = 9;
					sh_des = "z³oty";
					name_des = "z³otemu";
					name2_des = "z³otego";
					name3_des = "z³otemu";
					name4_des = "z³oty";
					name5_des = "z³otym";
					name6_des = "z³otym";
					break;
				case 6: //adamantyt
					obj->cost += 300;
					obj->material = 7;
					sh_des = "adamantytowy";
					name_des = "adamantytowy";
					name2_des = "adamantytowego";
					name3_des = "adamantytowemu";
					name4_des = "adamantytowy";
					name5_des = "adamantytowym";
					name6_des = "adamantytowym";
					break;
				case 7:
					obj->cost += 400;
					switch ( number_range( 1, 3 ) )
					{
						case 1: //mithril
							obj->material = 8;
							sh_des = "mithrilowy";
							name_des = "mithrilowy";
							name2_des = "mithrilowego";
							name3_des = "mithrilowemu";
							name4_des = "mithrilowy";
							name5_des = "mithrilowym";
							name6_des = "mithrilowym";
							break;
						case 2: //krysztal
							obj->material = 11;
							sh_des = "kryszta³owy";
							name_des = "kryszta³owy";
							name2_des = "kryszta³owego";
							name3_des = "kryszta³owemu";
							name4_des = "kryszta³owy";
							name5_des = "kryszta³owym";
							name6_des = "kryszta³owym";
							break;
						case 3: //platyna
							obj->material = 34;
							sh_des = "platynowy";
							name_des = "platynowy";
							name2_des = "platynowego";
							name3_des = "platynowemu";
							name4_des = "platynowy";
							name5_des = "platynowym";
							name6_des = "platynowym";
							break;
					}
			} //koniec rodzaju materialu
			//koniec amuletu losowego
			break;


	} //koniec losowania amuletow


	if ( losowy )
	{
		free_string( obj->short_descr );
		sprintf( txt, "%s %s", sh_des, sh_desc );
		obj->short_descr = str_dup( txt );

		free_string( obj->name );
		sprintf( txt, "%s %s", name_des, name );
		obj->name = str_dup( txt );

		free_string( obj->name2 );
		sprintf( txt, "%s %s", name2_des, name2 );
		obj->name2 = str_dup( txt );

		free_string( obj->name3 );
		sprintf( txt, "%s %s", name3_des, name3 );
		obj->name3 = str_dup( txt );

		free_string( obj->name4 );
		sprintf( txt, "%s %s", name4_des, name4 );
		obj->name4 = str_dup( txt );

		free_string( obj->name5 );
		sprintf( txt, "%s %s", name5_des, name5 );
		obj->name5 = str_dup( txt );

		free_string( obj->name6 );
		sprintf( txt, "%s %s", name6_des, name6 );
		obj->name6 = str_dup( txt );
	}
	else
	{
		free_string( obj->short_descr ); obj->short_descr = str_dup( sh_desc );
		free_string( obj->name ); obj->name = str_dup( name );
		free_string( obj->name2 ); obj->name2 = str_dup( name2 );
		free_string( obj->name3 ); obj->name3 = str_dup( name3 );
		free_string( obj->name4 ); obj->name4 = str_dup( name4 );
		free_string( obj->name5 ); obj->name5 = str_dup( name5 );
		free_string( obj->name6 ); obj->name6 = str_dup( name6 );
	}

	free_string( obj->description );
	sprintf( text, "%s po³yskuje magicznym blaskiem.", capitalize( sh_desc ) );
	obj->description = str_dup( text );

	//drugi -s-w-i-t-c-h-(-)- -> affect i apply'ie
	switch ( c )
	{
		case 1: //amulet czarnej rêki
			add_new_affect( obj, 0, APPLY_DEX, 2, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_STR, -1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_SKILL, gsn_dagger, 10, &AFF_NONE, -1, TRUE );
			obj->material = 7;
			obj->cost += 10 * COST_APPLY_SKILL + 2 * COST_APPLY_STAT;
			break;
		case 2: //amulet czarnej d³oni
			add_new_affect( obj, 0, APPLY_DEX, -2, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_STR, 1, 0, &AFF_NONE, -1, TRUE );
			obj->material = 7;
			obj->cost += COST_APPLY_STAT;
			break;
		case 3: //wê¿owy amulet si³y
			add_new_affect( obj, 0, APPLY_STR, 1, 0, &AFF_SNEAK, -1, TRUE );
			obj->cost += COST_APPLY_STAT + 1500;
			obj->material = 7;
			break;
		case 4: //wê¿owy amulet
			val = -2; //obj->cost += val * COST_APPLY_STAT;
			add_new_affect( obj, 0, APPLY_DEX, val, 0, &AFF_SLOW, -1, TRUE );
			obj->material = 7;
			break;
		case 5: //fascinum szybkoci
			val = -2;obj->cost += COST_AFF_HASTE;
			add_new_affect( obj, 0, APPLY_LUC, val, 0, &AFF_HASTE, -1, TRUE );
			obj->material = 9;
			break;
		case 6: //talizman os³ony
			add_new_affect( obj, 0, APPLY_AC, -20, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_DEX, 1, 0, &AFF_NONE, -1, TRUE );
			obj->material = 5;
			obj->cost += 20 * COST_APPLY_AC + COST_APPLY_STAT;
			break;
		case 7: //wisiorze ataku lwa
			add_new_affect( obj, 0, APPLY_STR, 1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_SKILL, gsn_disarm, 10, &AFF_NONE, -1, TRUE );
			obj->material = 4;
			obj->cost += COST_APPLY_STAT + 10 * COST_APPLY_SKILL;
			break;
		case 8: //przeczycia
			add_new_affect( obj, 0, APPLY_WIS, 1, 0, &AFF_DETECT_EVIL, -1, FALSE );
			add_new_affect( obj, 0, APPLY_NONE, 0, 0, &AFF_DETECT_GOOD, -1, TRUE );
			obj->material = 4;
			obj->cost += 2 * COST_AFF_DETECT_ALIGMENT + COST_APPLY_STAT;
			break;
		case 9: //wyczucia
			add_new_affect( obj, 0, APPLY_WIS, -1, 0, &AFF_NONE, -1, TRUE );
			obj->material = 13;
			obj->cost += COST_APPLY_STAT;
			break;
		case 10: //krasnoludzki amulet si³y
			add_new_affect( obj, 0, APPLY_STR, 1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_CON, 1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_DEX, -1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_SKILL, gsn_axe, 5, &AFF_NONE, -1, TRUE );
			obj->material = 13;
			obj->cost += 2 * COST_APPLY_STAT + 5 * COST_APPLY_SKILL;
			break;
		case 11: //krasnoludzki amulet
			add_new_affect( obj, 0, APPLY_STR, 2, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_CON, 2, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_DEX, -1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_WIS, -1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_INT, -1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_LUC, -1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_CHA, -1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_SKILL, gsn_axe, -5, &AFF_NONE, -1, TRUE );
			obj->material = 13;
			obj->cost += 4 * COST_APPLY_STAT;
			break;
		case 12: //fetysz ¿ywio³ów
			add_new_affect( obj, 0, APPLY_MEMMING, number_range(1,9), number_range(1,3), &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_MEMMING, number_range(1,7), number_range(1,5), &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_MEMMING, number_range(1,4), number_range(1,7), &AFF_NONE, -1, TRUE );
			obj->material = 11;
			obj->cost += 3 * COST_APPLY_MEMMING;
			break;
		case 13: //amulet ¿ywio³ów
			add_new_affect( obj, 0, APPLY_WIS, -1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_INT, -1, 0, &AFF_NONE, -1, TRUE );
			obj->material = 11;
			obj->cost += 2 * COST_APPLY_STAT;
			break;
		case 14: //oko boga
			add_new_affect( obj, 0, APPLY_NONE, 0, 0, &AFF_DETECT_INVIS, -1, TRUE );
			obj->material = 4;
			obj->cost += COST_AFF_DETECT_INVIS;
			break;
		case 15: //oko bo¿ka
			add_new_affect( obj, 0, APPLY_NONE, 0, 0, &AFF_BLIND, -1, TRUE );
			obj->material = 4;
			obj->cost += COST_AFF_DETECT_INVIS;
			break;
		case 16: //kamieñ ol¶nienia
			add_new_affect( obj, 0, APPLY_INT, 1, 0, &AFF_COMPREHEND_LANGUAGES, -1, TRUE );
			obj->material = 4;
			obj->cost += COST_AFF_COMPREHEND_LANGUAGES + COST_APPLY_STAT;
			break;
		case 17: //kamieñ l¶nienia
			add_new_affect( obj, 0, APPLY_WIS, -1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_INT, -1, 0, &AFF_NONE, -1, TRUE );
			obj->material = 4;
			obj->cost += 2 * COST_APPLY_STAT;
			break;
		case 18: //amulet z dwoma mieczami
			add_new_affect( obj, 0, APPLY_STR, 1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_SKILL, gsn_sword, 10, &AFF_NONE, -1, TRUE );
			obj->material = 13;
			obj->cost += COST_APPLY_STAT + 10 * COST_APPLY_SKILL;
			break;

		default: //losowy
			/*
			losowanie ile dodatkowych affectow moze byc jakis system moze sie da im
			wiecej affectow tym trudniej napisac tez sprawdzanie czy taki affekt juz
			jest -> na razie beda sie dublowac (znaczy moga), zobacze czy sie beda
			sumowac czy zamazywac (sumowac nia powinny, ale jak zamazuja to tym
			lepiej) (sumuja sie) powiedzmy ze na poczatek moga byc od 1 do 4 affectow
			*/
			number = number_percent();
			//anumber=ilosc affectow
			if ( number < 8 ) anumber = 3;
			else if ( number < 30 ) anumber = 2;
			else if ( number < 100 ) anumber = 1;

			//anumber=5;
			end = FALSE;
			for ( a = 1;a <= anumber;a++ )
			{ //petla robiaca anumber affektow
				type = 0, loc = 0, mod = 0, level = 0, bitv = &AFF_NONE, bylo = FALSE;
				//no to jakie moga byc te affekty:
				//str,int,wis,dex,con 5
				//mov,hp,spell_mem 4
				//ac,dam,hit,skille, 4
				//zwyk³e affecty 1
				//cos jeszcze?
				if ( a == anumber )
				{
					end = TRUE;
					//affecty
					// 'identify' nie wywala tego (przynajmniej dla treasure)
					//nie wiem dlaczego, ale flagi dodaje
					/*bedzie mogl byc jeden czarek na obj, ze bedzie jakies 60%
					*/
					bitv = get_bitvector();
					koszt += get_cost_of_bitvector(bitv);
				}
				los = number_range( 5, 14 );
				switch ( los )
				{
/* WY£¡CZAM: Gurthg - 2005-02-20
					case 1: //str
						loc = APPLY_STR;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							obj->cost += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 2: //int
						loc = APPLY_INT;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							obj->cost += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 3: //wis
						loc = APPLY_WIS;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							obj->cost += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 4: //dex
						loc = APPLY_DEX;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							obj->cost += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 5: //con
						loc = APPLY_CON;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							obj->cost += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 6: //cha
						loc = APPLY_CHA;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							obj->cost += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 7: //luc
						loc = APPLY_LUC;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							obj->cost += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
*/
					case 8: //mov
						loc = APPLY_MOVE;
						mod = number_range( 1, 10 );
						koszt += number_range( 100, 200 ) * mod;
						break;
					case 9: //hp
						loc = APPLY_HIT;
						mod = number_range( 1, 10 );
						koszt += COST_APPLY_SKILL * mod;
						break;
					case 10: //spell_mem
						loc = APPLY_MEMMING;
						mod = number_range( 1, 7 );
						switch ( mod )
						{ //czeba poprawic to na pierdzionkach tez
							case 1: level = number_range( 1, 7 );break;
							case 2: level = number_range( 1, 6 );break;
							case 3: level = number_range( 1, 5 );break;
							case 4: level = number_range( 1, 4 );break;
							case 5: level = number_range( 1, 3 );break;
							case 6: level = number_range( 1, 2 );break;
							case 7: level = 1;break;
						}
						koszt += number_range( 150, 180 ) * ( mod * 10 + level );
						break;
					case 11: //ac
						loc = APPLY_AC;
						val = number_range( 1, 10 );
						mod = -val;
						koszt += val * COST_APPLY_AC;
						break;
					case 12: //dam
						loc = APPLY_DAMROLL;
						switch( number_range(1,11))
						{
							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
								mod = -1;
								break;
							case 6:
							case 7:
							case 8:
							case 9:
							case 10:
								mod = 1;
								break;
							case 11:
								mod = 2;
								break;
						}
						koszt += COST_APPLY_DAMROLL * mod;
						break;
					case 13: //hit
						loc = APPLY_HITROLL;
						switch( number_range(1,11))
						{
							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
								mod = -1;
								break;
							case 6:
							case 7:
							case 8:
							case 9:
							case 10:
								mod = 1;
								break;
							case 11:
								mod = 2;
								break;
						}
						koszt += COST_APPLY_HITROLL * mod;
						break;
					case 14: //skills
						loc = APPLY_SKILL;
						level = number_range( 1, 10 );
						koszt += level * COST_APPLY_SKILL;
						switch ( number_range( 1, 40 ) )
						{
							case 1: mod = gsn_axe;break;
							case 2: mod = gsn_flail;break;
							case 3: mod = gsn_dagger;break;
							case 4: mod = gsn_mace;break;
							case 5: mod = gsn_polearm;break;
							case 6: mod = gsn_staff;break;
							case 7: mod = gsn_shield_block;break;
							case 8: mod = gsn_spear;break;
							case 9: mod = gsn_sword;break;
							case 10: mod = gsn_whip;break;
							case 11: mod = gsn_backstab;break;
							case 12: mod = gsn_bash;break;
							case 13: mod = gsn_berserk;break;
							case 14: mod = gsn_disarm;break;
							case 15: mod = gsn_dodge;break;
							case 16: mod = gsn_envenom;break;
							case 17: mod = gsn_kick;break;
							case 18: mod = gsn_parry;break;
							case 19: mod = gsn_rescue;break;
							case 20: mod = gsn_trip;break;
							case 21: mod = gsn_flail_mastery;break;
							case 22: mod = gsn_wardance;break;
							case 23: mod = gsn_mace_mastery;break;
							case 24: mod = gsn_riding;break;
							case 25: mod = gsn_hide;break;
							case 26: mod = gsn_lore;break;
							case 27: mod = gsn_meditation;break;
							case 28: mod = gsn_peek;break;
							case 29: mod = gsn_pick_lock;break;
							case 30: mod = gsn_sneak;break;
							case 31: mod = gsn_steal;break;
							case 32: mod = gsn_charge;break;
							case 33: mod = gsn_recuperate;break;
							case 34: mod = gsn_lay;break;
							case 35: mod = gsn_damage_reduction;break;
							case 36: mod = gsn_critical_strike;break;
							case 37: mod = gsn_two_weapon_fighting;break;
							case 38: mod = gsn_dagger_mastery;break;
							case 39: mod = gsn_twohander_fighting;break;
							case 40: mod = gsn_turn;break;
							default: break;
						}

						break;

					default:
						break;
				}
				//moznaby tez dodac jakies zle rzeczy? np sex change, albo ujemne rzeczy
				//mozna to od razu zrobic np:
				//			-mod	 ?

				//problem jest taki ze po spadnieciu affectow czasowych ich cena pozostaje
				//bez zmian, trzeba to bardziej uzaleznic od czasu dzialania, a mniej od
				//rodzaju affektow...
				//trzeba pomyslec nad jakimis wspolczynnikami czy jakos tak...
				//toto nizej dodaje cokolwiek do przedmiotu (sprawdzajac czy bylo)

				for ( z = 1;z <= anumber;z++ ) if ( los_tab[ z - 1 ] == los ) bylo = TRUE;
				if ( bylo ) a++;
				else
				{
					los_tab[ a - 1 ] = los;
					add_new_affect( obj, type, loc, mod, level, bitv, generate_aff_duration( points, &koszt ), end );
					obj->cost += koszt;
				}
			} //koniec for'a affectow
	}
	return;
}

void generate_gem( OBJ_DATA *obj, int* points )
{
    char text[ MAX_INPUT_LENGTH ], *name = "(null)", *sh_desc = "(null)";
    char *name2 = "(null)", *name3 = "(null)", *name4 = "(null)", *name5 = "(null)", *name6 = "(null)", *item_description = "";
    int item_level = *points, power = 1, i = 0, max = 0;
    int choose = 0;

    ext_flags_clear( obj->wear_flags2 );

    obj->weight = 1;
    obj->material = 16;

    /**
     * 20% szansy na wylosowanie superkamienia;-), czyli kamienia, którego
     * nazwa pochodzi od imienia Nie¶miertelnego lub zas³u¿onego gracza.
     * Dodawajcie w kategorii 7 (siedem) jakie¶ dodatkowe kamienie, ale takie
     * które bêd± istnieæ tylko na naszym ¶wiecie, zreszt± polecam ju¿
     * istniej±ce kamienie;-)
     *
     * 2004-11-10: Gurthg
     *
     */
    if ( item_level > 6 )
    {
        if (number_percent() > 20)
        {
            item_level = 6;
        }
        else
        {
            item_level = 7;
        }
    }
    else
    {
        item_level = UMAX( 1, item_level );
    }

    /**
     * wyliczanie ceny
     * bazowa cena = MOD1 ^ ( item_level - 1 ) * BASE
     * dla:
     * BASE = 20
     * MOD1 =  3
     *
     * otrzymamy nastêpuj±ce ceny bazowe:
     *
     * 1 -    20
     * 2 -    60
     * 3 -   180
     * 4 -   540
     * 5 -  1620
     * 6 -  4860
     * 7 - 14580
     *
     */
    max = item_level-1;
    for ( i=0; i < max; i++)
    {
        power *= 3;
    }
    obj->cost = power * 50 / 3;

    //zmiana ceny +-(0-50)%
    if ( number_range( 1, 2 ) == 1 ) obj->cost -= number_range( 1, obj->cost/2 );
    else obj->cost += number_range( 1, obj->cost/2 );

    switch ( item_level )
    {
        // kamienie dekoracyjne
        case 1:
            obj->value[ 0 ] = 5 + number_range( 0, 20 );
            switch ( number_range( 1, 18 ) )
            {
                case 1:
                    name = "agat mchowy";
                    sh_desc = "agat mchowy";
                    name2 = "agatu mchowego";
                    name3 = "agatowi mchowemu";
                    name4 = "agat mchowy";
                    name5 = "agatem mchowym";
                    name6 = "agacie mchowym";
                    switch ( number_range( 1, 2 ) )
                    {
                        case 1:
                            item_description = "¯ó³toszary kamieñ z mchopodobnymi, zielonymi centkami.";
                            break;
                        default:
                            item_description = "Ró¿owy kamieñ z mchopodobnymi, zielonymi centkami.";
                            break;
                    }
                    break;
                case 2:
                    name = "agat paskowany";
                    sh_desc = "agat paskowany";
                    name2 = "agatu paskowanego";
                    name3 = "agatowi paskowanemu";
                    name4 = "agat paskowany";
                    name5 = "agatem paskowanym";
                    name6 = "agacie paskowanym";
                    switch ( number_range( 1, 3 ) )
                    {
                        case 1:
                            item_description = "Br±zowy kamieñ w bia³o-niebieskie poprzeczne pr±¿ki.";
                            break;
                        case 2:
                            item_description = "Niebieski kamieñ w bia³o-niebieskie poprzeczne pr±¿ki.";
                            break;
                        default:
                            item_description = "Czerwony kamieñ w bia³o-niebieskie poprzeczne pr±¿ki.";
                            break;
                    }
                    break;
                case 3:
                    name = "agat oczko";
                    sh_desc = "agat z oczkiem";
                    name2 = "agatu z oczkiem";
                    name3 = "agatowi z oczkiem";
                    name4 = "agat z oczkiem";
                    name5 = "agatem z oczkiem";
                    name6 = "agacie z oczkiem";
                    switch ( number_range( 1, 4 ) )
                    {
                        case 1:
                            item_description = "Szary kamieñ w zielone kó³eczka.";
                            break;
                        case 2:
                            item_description = "Bia³y kamieñ w zielone kó³eczka.";
                            break;
                        case 3:
                            item_description = "Br±zowy kamieñ w zielone kó³eczka.";
                            break;
                        default:
                            item_description = "Niebieski kamieñ w zielone kó³eczka.";
                            break;
                    }
                    break;
                case 4:
                    name = "agat tygrysie oczko";
                    sh_desc = "agat tygrysie oczko";
                    name2 = "agatu tygrysiego oczka";
                    name3 = "agatowi tygrysemu oczku";
                    name4 = "agat tygrysie oczko";
                    name5 = "agatem tygrysim oczkiem";
                    name6 = "agacie tygrysim oczku";
                    item_description = "¯ó³tobr±zowy kamieñ z jednym paskiem przez ¶rodek.";
                    break;
                case 5:
                    name = "azuryt";
                    sh_desc = "azuryt";
                    name2 = "azurytu";
                    name3 = "azurytowi";
                    name4 = "azuryt";
                    name5 = "azurytem";
                    name6 = "azurycie";
                    item_description = "Nieprzezroczysty kamieñ z ciemnoniebieskimi centakami.";
                    break;
                case 6:
                    name = "hematyt";
                    sh_desc = "hematyt";
                    name2 = "hematytu";
                    name3 = "hematytowi";
                    name4 = "hematyt";
                    name5 = "hematytem";
                    name6 = "hematycie";
                    item_description = "Ciemny, szaroczarny kamieñ.";
                    break;
                case 7:
                    name = "lazuryt";
                    sh_desc = "lazuryt";
                    name2 = "lazurytu";
                    name3 = "lazurytowi";
                    name4 = "lazuryt";
                    name5 = "lazurytem";
                    name6 = "lazurycie";
                    switch ( number_range( 1, 2 ) )
                    {
                        case 1:
                            item_description = "Ciemnoniebieski kamieñ z jasnymi centkami.";
                            break;
                        default:
                            item_description = "Jasnoniebieski kamieñ z jasnymi centkami.";
                            break;
                    }
                    break;
                case 8:
                    name = "malachit";
                    sh_desc = "malachit";
                    name2 = "malachitu";
                    name3 = "malachitowi";
                    name4 = "malachit";
                    name5 = "malachitem";
                    name6 = "malachicie";
                    switch ( number_range( 1, 2 ) )
                    {
                        case 1:
                            item_description = "Pr±¿kowany, jasnozielony kamyk";
                            break;
                        default:
                            item_description = "Pr±¿kowany, ciemnozielony kamyk";
                            break;
                    }
                    break;
                case 9:
                    name = "niebieski kwarc";
                    sh_desc = "niebieski kwarc";
                    name2 = "niebieskiego kwarcu";
                    name3 = "niebieskiemu kwarcowi";
                    name4 = "niebieski kwarc";
                    name5 = "niebieskim kwarcem";
                    name6 = "niebieskim kwarcu";
                    switch ( number_range( 1, 3 ) )
                    {
                        case 1:
                            item_description = "Prawie ca³kowicie prze¼roczysty kamieñ.";
                            break;
                        case 2:
                            item_description = "Prze¼roczysty, lecz lekko zabarwiony na niebiesko kamieñ.";
                            break;
                        default:
                            item_description = "Bladoniebieski, prze¼roczysty kamieñ.";
                            break;
                    }
                    break;
                case 10:
                    name = "rodochrozyt";
                    sh_desc = "rodochrozyt";
                    name2 = "rodochrozytu";
                    name3 = "rodochrozytowi";
                    name4 = "rodochrozyt";
                    name5 = "rodochrozytem";
                    name6 = "rodochrozycie";
                    item_description = "Jasnoró¿owy niczym nie wyró¿niaj±cy siê kamyczek.";
                    break;
                case 11:
                    name = "obsydian";
                    sh_desc = "obsydian";
                    name2 = "obsydianu";
                    name3 = "obsydianowi";
                    name4 = "obsydian";
                    name5 = "obsydianem";
                    name6 = "obsydianie";
                    switch ( number_range( 1, 3 ) )
                    {
                        case 1:
                            item_description = "Brunatny, silnie po³yskuj±cy kamieñ.";
                            break;
                        case 2:
                            item_description = "Czarny, silnie po³yskuj±cy kamieñ.";
                            break;
                        default:
                            item_description = "Zielonkawy, silnie po³yskuj±cy kamieñ.";
                            break;
                    }
                    break;
                case 12:
                    name = "nefryt";
                    sh_desc = "nefryt";
                    name2 = "nefrytu";
                    name3 = "nefrytowi";
                    name4 = "nefryt";
                    name5 = "nefrytem";
                    name6 = "nefrycie";
                    switch ( number_range( 1, 3 ) )
                    {
                        case 1:
                            item_description = "Ciemnozielony, silnie po³yskuj±cy kamieñ.";
                            break;
                        case 2:
                            item_description = "Delikatnie zielony, silnie po³yskuj±cy kamieñ.";
                            break;
                        default:
                            item_description = "Zielonkawy, silnie po³yskuj±cy kamieñ.";
                            break;
                    }
                    break;
                case 13:
                    name = "lwie oczko";
                    sh_desc = "lwie oczko";
                    name2 = "lwiego oczka";
                    name3 = "lwiemu oczku";
                    name4 = "lwie oczko";
                    name5 = "lwim oczkiem";
                    name6 = "lwim oczkiem";
                    item_description = "Lwie oczko jest odmian± labradorytu. Labradoryty s± kamieniami w kolorach od jasno do ciemnoszarego, które maj± ¿y³ki w ró¿nych kolorach. Te '¿y³ki' s± zazwyczaj b³êkitne, ale mog± byæ we wszystkich kolorach. Labradoryt z zielonymi ¿y³kami jest zwany lwim oczkiem. ";
                    break;
                case 14:
                    name = "kamieñ s³oneczny";
                    sh_desc = "kamieñ s³oneczny";
                    name2 = "kamienia s³onecznego";
                    name3 = "kamieniowi s³onecznemu";
                    name4 = "kamieñ s³oneczny";
                    name5 = "kamieniem s³onecznym";
                    name6 = "kamieniu s³onecznym";
                    item_description = "S³oneczny kamieñ nale¿y do skaleni i jest spokrewniony z Ksiê¿ycowym kamieniem. Jego fachowa nazwa to oligoklas. Mo¿e byæ bezbarwny lub lekko zielonkawy i jest bardzo twardy, jednak bardziej powszechna jest jego mniej twarda odmiana (kaboszon). Klejnoty tej odmiany maj± jasnoczerwone lub pomarañczowe skazy (malutkie kryszta³ki), zawieszone w bezbarwnym kamieniu, co nadaje im przyjemny z³otawy lub czerwono-br±zowy kolor";
                    break;
                case 15:
                    name = "andaryt";
                    sh_desc = "andaryt";
                    name2 = "andarytu";
                    name3 = "andarytowi";
                    name4 = "andaryt";
                    name5 = "andarytem";
                    name6 = "andarycie";
                    switch ( number_range( 1, 3 ) )
                    {
                        case 1:
                            item_description = "Andradyt jest twardym i wytrzyma³ym kamieniem szlachetnym. Z niego powstaj± przejrzyste klejnoty, zwane Andarami, które po odpowiednim oszlifowaniu maj± zabarwienie zielonkawo- lub br±zowawoczerwone. ";
                            break;
                        case 2:
                            item_description = "Wygl±daj±cy na pospolity, zielonkawy, silnie po³yskuj±cy kamieñ.";
                            break;
                        default:
                            item_description = "Delikatnie zielonkawy, silnie po³yskuj±cy kamieñ.";
                            break;
                    }
                    break;
                case 16:
                    name = "agat ognisty agatognisty";
                    sh_desc = "agat ognisty";
                    name2 = "agatu ognistego";
                    name3 = "agatowi ognistemu";
                    name4 = "agat ognisty";
                    name5 = "agatem ognistym";
                    name6 = "agacie ognistym";
                    item_description = "Agat ognisty to nazwa nadawana chalcedonowi, który zawiera têczowe ¿y³ki. Je¿eli zostanie odpowiednio oszlifowany, ukazuj± siê bogate czerwienie, br±zy, zielenie i z³ocisto¶ci. Najszlachetniejsze okazy s± lekko przezroczyste.";
                    break;
                case 17:
                    name = "kamieñ iol";
                    sh_desc = "kamieñ iol";
                    name2 = "kamienia iol";
                    name3 = "kamieniowi iol";
                    name4 = "kamieñ iol";
                    name5 = "kamieniem iol";
                    name6 = "kamieniu iol";
                    item_description = "Iol to skrót od Iolitu. Popularnie nazywany jest 'fioletowym kamieniem' mimo ¿e najczêstszy odcieñ, to niebieski. Iole najczê¶ciej szlifuje siê, aby podkre¶liæ zmianê koloru, widoczn± przy patrzeniu z ró¿nych stron. Ma³e oszlifowane Iole mog± byæ bez skazy, ale wiêksze okazy najczê¶ciej zawieraj± zanieczyszczenia, na przyk³ad kryszta³ami hemitytu, które nadaj± im z³otawy blask, podobny do blasku s³onecznego kamienia. ";
                    break;
                default:
                    name = "turkus";
                    sh_desc = "turkus";
                    name2 = "turkusu";
                    name3 = "turkusowi";
                    name4 = "turkus";
                    name5 = "turkusem";
                    name6 = "turkusie";
                    item_description = "Turkus jest nieprzejrzystym b³êkitnym kamieniem z ciemniejszymi ¿y³kami. Wielu je¼d¼ców umieszcza od³amek kamienia w uzdach, co ma im przynosiæ szczê¶cie. ";
                    break;
            }
            break;
            // pó³szlachetne
        case 2:
            choose = number_range( 0, 15 );
            obj->value[ 0 ] = 100 + choose;
            switch ( choose )
            {
                case 1:
                    name = "chalcedon";
                    sh_desc = "chalcedon";
                    name2 = "chalcedonu";
                    name3 = "chalcedonowi";
                    name4 = "chalcedon";
                    name5 = "chalcedonem";
                    name6 = "chalcedonie";
                    item_description = "Chalcedon to per³owo bia³y nieprzejrzysty kamieñ zwykle lekko zabarwiony na bladoniebiesko lub bladozielono ze z³ocistymi plamkami. Spokrewniony z ognistym i czarnym opalem jest od nich trochê czê¶ciej spotykany.";
                    break;
                case 2:
                    name = "chryzopraz";
                    sh_desc = "chryzopraz";
                    name2 = "chryzoprazu";
                    name3 = "chryzoprazowi";
                    name4 = "chryzopraz";
                    name5 = "chryzoprazem";
                    name6 = "chryzoprazie";
                    item_description = "Chryzoprazy charakteryzuj± siê najczê¶ciej piêkn± zielon± barw± i zachowuj± czê¶ciow± przezroczysto¶æ. Na Tamerze ich g³ówne z³o¿a znajduj± siê w Górach Nied¼wiedzich oraz na Wy¿ynie Deira. Wiele ras wierzy, ¿e kamienie te lecz± choroby zwi±zane z seksualn± stron± ¿ywota, poprzez otwarcie czakramu seksu i wzmocnienie sobowtóra erotycznego - dzieje siê tak jednak tylko, gdy jest u¿ywany przez jednego w³a¶ciciela, dotkniêcie przez kogo innego, choæby przez najbli¿sz± osobê, blokuje to dzia³anie na d³ugi okres czasu. Jest to kamieñ b³ogos³awiony przez Sorennê, co sk³ania jej wyznawców do dalekich wêdrówek w poszukiwaniu materia³u do wykonania przeró¿nych ozdób, maj±cych ponoæ magiczn± moc.";
                    break;
                case 3:
                    name = "cyrkon";
                    sh_desc = "cyrkon";
                    name2 = "cyrkonu";
                    name3 = "cyrkonowi";
                    name4 = "cyrkon";
                    name5 = "cyrkonem";
                    name6 = "cyrkonie";
                    item_description = "Cyrkon jest br±zowawym kryszta³em, wystêpuj±cym w ska³ach wulkanicznych. Zazwyczaj nabiera jasnob³êkitny odcieñ podczas podgrzewania i szlifowania (zazwyczaj w fasety).";
                    break;
                case 4:
                    name = "cytryn";
                    sh_desc = "cytryn";
                    name2 = "cytrynu";
                    name3 = "cytrynowi";
                    name4 = "cytryn";
                    name5 = "cytrynem";
                    name6 = "cytronie";
                    item_description = "Spogl±dasz z ciekawo¶ci± na kamieñ zwany cytrynem. Przypomina ci siê wówczas pewna legenda. Legenda o legionie ¶mia³ków i jednej wdowie, którzy wybrali siê z drewnianymi mieczami na zielonego smoka. W dawnych czasach, gdy w Arras by³y jeszcze dziewice, miastu uprzykrza³ siê nieustannie, straszliwy zielony smok. Mimo i¿ smok wielga¶ny by³ i ogólnie potê¿ny, w mieszkañcach budzi³ raczej odrazê ni¿ strach. Czê¶ciej bowiem zdarza³o siê, ¿e komu¶ siê umar³o na skutek zrzuconych z du¿ej wysoko¶ci ekskrementów, ani¿eli zjedzenia czy spalenia przez owego smoka straszliwego. Zarówno mieszkañcy miasta jak i wioski próbowali siê gada pozbyæ. A to przez zmniejszenie ilo¶ci dziewic, a to przez napakowanie owcy ró¿nymi truciznami, od których ¶mierdzia³y gównem a¿ po Fortece, przez co smok pó¼niej - najwyra¼niej ura¿ony spali³ cha³upe albo dwie, a owce wyrzuca³ pod miejskim Ratuszem. Burmistrz miasta nie móg³szy znie¶æ ju¿ d³u¿ej takiej sytuacji, wybra³ siê po poradê do starej wied¼my, która wzi±wszy w gar¶æ trochê w³osów ³ysego ogra wywró¿y³a by zebraæ 100 odwa¿nych ¶mia³ków i jedn± kobietê, po czym kampaniê na pochybel smoka wys³aæ. Zerba³o siê tego 100 mê¿nych oraz jedna niewiasta i wyruszyli w drogê. Droga by³a d³uga, a pie¶ni o zwyciêstwie nad smokiem ¶piewano jeszcze przed dotarciem w góry. Jednak co¶ posz³o nie tak. Po³owa orszaku rozweselono, widocznie zbyt mocnym gard³ogrzmotem z dziewk± zaczê³a siê integrowaæ, na skutek czego pad³a pó¼niej ofiar± chorób wenerycznych. Skrybi którzy ca³e zaj¶cie opisywali drug± po³owê orszaku znale¼li po drodze przez ¶nie¿ne góry do Easterial, gdzie owi ludzie udali siê zapewne po zapasy, a zapomniwszy o przestrogach dotycz±cych tamtejszego wina. Smok umar³ du¿o pó¼niej. Ze staro¶ci. Ale có¿ to jest cytryn w takim razie? A taki ma³y, ¿ó³ty kamyk.";
                    break;
                case 5:
                    name = "dymny kwarc";
                    sh_desc = "dymny kwarc";
                    name2 = "dymnego kwarcu";
                    name3 = "dymnemu kwarcowi";
                    name4 = "dymny kwarc";
                    name5 = "dymnym kwarcem";
                    name6 = "dymnym kwarcu";
                    item_description = "";
                    break;
                case 6:
                    name = "górski kryszta³";
                    sh_desc = "górski kryszta³";
                    name2 = "górskiego kryszta³u";
                    name3 = "górskiemu kryszta³owi";
                    name4 = "górski kryszta³";
                    name5 = "górskim kryszta³em";
                    name6 = "górskim krysztale";
                    item_description = "Kryszta³ górski czêsto nazywany jest równie¿ kamiennym lodem. Najbardziej znana jest bezbarwna odmiana, ale równie¿ spotykane s± takie, o barwie bia³ej czy szarej oraz w odcieniach ¿ó³tego, pomarañczowego, ró¿owego, br±zowo-purpurowego, przez fiolet, zieleñ, czerñ, a¿ do rzadko spotykanej odmiany o zabarwieniu niebieskim. Pod wzglêdem w³asno¶ci magicznych mo¿na go uznaæ za kamieñ uniwersalny, bez którego wprost nie sposób siê obej¶æ w zagadnieniach ezoterycznych, czy zwi±zanych medycyn± naturaln±. Dlatego niektórzy przypisuj± jego w³asno¶ci Isorykowi, ale Kamieñ ten, chyba jako jedyny, z powodzeniem mo¿na stosowaæ pracuj±c z ka¿d± z dziedzin magii, poniewa¿ dzia³a na wszystkich poziomach energetycznych. Jest wrêcz uwa¿any za klucz do otwierania centrów magicznych. Dlatego, pomimo, ¿e wystêpuje w niemal wszystkich pasmach górskich Tamery, jest uwa¿any za cenny kamieñ pó³szlachetny i handel nim kwitnie, w szczególno¶ci na traktach pomiêdzy Arras, Carrallak i Smoczymi Górami.";
                    break;
                case 7:
                    name = "gwiezdny ró¿owy kwarc";
                    sh_desc = "gwiezdny ró¿owy kwarc";
                    name2 = "gwiezdnego ró¿owego kwarcu";
                    name3 = "gwiezdnemu ró¿owemu kwarcowi";
                    name4 = "gwiezdny ró¿owy kwarc";
                    name5 = "gwiezdnym ró¿owym kwarcem";
                    name6 = "gwiezdnym ró¿owym kwarcu";
                    item_description = "";
                    break;
                case 8:
                    name = "jaspis";
                    sh_desc = "jaspis";
                    name2 = "jaspisu";
                    name3 = "jaspisowi";
                    name4 = "jaspis";
                    name5 = "jaspisem";
                    name6 = "jaspisie";
                    item_description = "Jaspis to nieprzejrzysty kwarc o kolorze czerwonym, br±zowym lub czarnym. Bardzo rzadkie okazy s± niebieskie lub maj± niebieskie pr±¿ki na tle w innym kolorze. Skruszony jaspis wykorzystuje siê przy wyrobie mikstur i magicznych przedmiotów, chroni±cych przed trucizn±. ";
                    break;
                case 9:
                    name = "kamien ksiezycowy";
                    sh_desc = "kamien ksiezycowy";
                    name2 = "kamienia ksiezycowego";
                    name3 = "kamieniowi ksiezycowemu";
                    name4 = "kamien ksiezycowy";
                    name5 = "kamieniem ksiezycowym";
                    name6 = "kamieniu ksiezycowym";
                    item_description = "Kamieñ ksiê¿ycowy to nieprzejrzysty bia³y skaleñ, polerowany w celu uzyskania b³êkitnawego poblasku. Legendy mówi±, ¿e widok tego klejnotu zmusza likantropa do przybrania zwierzêcej postaci. Nie wiadomo czy to jest prawda, albowiem nikt, kto by³ dostatecznie blisko, by siê o tym przekonaæ nie prze¿y³, aby móc o tym opowiedzieæ. Jednak u¿ywa siê tych kamieni przy rzucaniu czarów, które dzia³aj± na tê w³a¶nie kl±twê. ";
                    break;
                case 10:
                    name = "karnelian";
                    sh_desc = "karnelian";
                    name2 = "karnelianu";
                    name3 = "karnelianowi";
                    name4 = "karnelian";
                    name5 = "karnelianem";
                    name6 = "karnelianie";
                    item_description = "";
                    break;
                case 11:
                    name = "krwawnik";
                    sh_desc = "krwawnik";
                    name2 = "krwawnika";
                    name3 = "krwawnikowi";
                    name4 = "krwawnik";
                    name5 = "krwawnikiem";
                    name6 = "krwawniku";
                    item_description = "Krwawnik to ciemna zielonkawo szara odmiana kwarcu, w którym wystêpuj± zanieczyszczenia w postaci czerwonego kryszta³u.";
                    break;
                case 12:
                    name = "onyks";
                    sh_desc = "onyks";
                    name2 = "onyksu";
                    name3 = "onyksowi";
                    name4 = "onyks";
                    name5 = "onyksem";
                    name6 = "onyksie";
                    item_description = "";
                    break;
                case 13:
                    name = "kordun";
                    sh_desc = "kordun";
                    name2 = "korduna";
                    name3 = "kordunowi";
                    name4 = "kordun";
                    name5 = "kordunem";
                    name6 = "kordunie";
                    item_description = "";
                    break;
                case 14:
                    name = "kamieñ z nieba";
                    sh_desc = "kamieñ z nieba";
                    name2 = "kamienia z nieba";
                    name3 = "kamieniowi z nieba";
                    name4 = "kamieñ z nieba";
                    name5 = "kamieniem z nieba";
                    name6 = "kamieniu z nieba";
                    item_description = "Kamieñ z nieba to popularna nazwa nadawana przezroczystym lub lekko zabarwionym od³amkom szk³a niebiañskiego (meteorycznego) pochodzenia.";
                    break;
                case 15:
                    name = "wodna gwiazda";
                    sh_desc = "wodna gwiazda";
                    name2 = "wodnej gwiazdy";
                    name3 = "wodnej gwie¿dzie";
                    name4 = "wodn± gwiazdê";
                    name5 = "wodn± gwiazd±";
                    name6 = "wodnej gwie¿dzie";
                    item_description = "Rzadko spotykany achroit zwany jest tak¿e bezbarwnym turmalinem. Ten kamieñ jest pe³en skaz i pêkniêæ, wiêc tylko niewielka jego czê¶æ nadaje siê do obróbki.";
                    break;
                default:
                    name = "sardonyks";
                    sh_desc = "sardonyks";
                    name2 = "sardonyksu";
                    name3 = "sardonyksowi";
                    name4 = "sardonyks";
                    name5 = "sardonyksem";
                    name6 = "sardonyksie";
                    item_description = "";
                    break;
            }
            break;
            // fantazyjne
        case 3:
            choose = number_range( 0, 10 );
            obj->value[ 0 ] = 200 + choose;
            switch ( choose )
            {
                case 1:
                    name = "aleksandryt";
                    sh_desc = "aleksandryt";
                    name2 = "aleksandrytu";
                    name3 = "aleksandrytowi";
                    name4 = "aleksandryt";
                    name5 = "aleksandrytem";
                    name6 = "aleksandrycie";
                    item_description = "";
                    break;
                case 2:
                    name = "ametyst";
                    sh_desc = "ametyst";
                    name2 = "ametystu";
                    name3 = "ametystowi";
                    name4 = "ametyst";
                    name5 = "ametystem";
                    name6 = "ametyscie";
                    item_description = "";
                    break;
                case 3:
                    name = "bursztyn";
                    sh_desc = "bursztyn";
                    name2 = "bursztynu";
                    name3 = "bursztynowi";
                    name4 = "bursztyn";
                    name5 = "bursztynem";
                    name6 = "bursztynie";
                    item_description = "";
                    obj->material = 50;
                    break;
                case 4:
                    name = "chryzoberyl";
                    sh_desc = "chryzoberyl";
                    name2 = "chryzoberylu";
                    name3 = "chryzoberylowi";
                    name4 = "chryzoberyl";
                    name5 = "chryzoberylem";
                    name6 = "chryzoberylu";
                    item_description = "Chryzoberyl jest twardym, przezroczystym, zielonym kamieniem, zazwyczaj oszlifowanym. Mówi siê, ¿e ³±czy go wiê¼ z wymiarami zewnêtrznymi, st±d te¿ wykorzystuje siê go w przypadkach opêtania przez demony lub martwiaki. ";
                    break;
                case 5:
                    name = "gagat";
                    sh_desc = "gagat";
                    name2 = "gagatu";
                    name3 = "gagatowi";
                    name4 = "gagat";
                    name5 = "gagatem";
                    name6 = "gagacie";
                    item_description = "";
                    break;
                case 6:
                    name = "jadeit";
                    sh_desc = "jadeit";
                    name2 = "jadeitu";
                    name3 = "jadeitowi";
                    name4 = "jadeit";
                    name5 = "jadeitem";
                    name6 = "jadeicie";
                    item_description = "";
                    break;
                case 7:
                    name = "koral";
                    sh_desc = "koral";
                    name2 = "koralu";
                    name3 = "koralowi";
                    name4 = "koral";
                    name5 = "koralem";
                    name6 = "koralu";
                    item_description = "";
                    break;
                case 8:
                    name = "granat";
                    sh_desc = "granat";
                    name2 = "granatu";
                    name3 = "granatowi";
                    name4 = "granat";
                    name5 = "granatem";
                    name6 = "granacie";
                    item_description = "Granaty to ogólna grupa kryszta³ów w kolorach od ciemnoczerwonego do fioletowego. Najczê¶ciej szlifuje siê te rzadkie izometryczne kamienie tak, aby mia³y od 12 do 24 ¶cianek, a nawet, du¿o rzadziej, 36. Kr±¿± pog³oski, ¿e istnieje 48 ¶cienny granat, choæ nikt nie by³ w stanie go odnale¼æ.";
                    break;
                case 9:
                    name = "pirop";
                    sh_desc = "pirop";
                    name2 = "piropu";
                    name3 = "piropowi";
                    name4 = "pirop";
                    name5 = "piropem";
                    name6 = "piropie";
                    item_description = "";
                    break;
                case 10:
                    name = "koral rogowy";
                    sh_desc = "koral rogowy";
                    name2 = "korala rogowego";
                    name3 = "koralowi rogowemu";
                    name4 = "koral rogowy";
                    name5 = "koralem rogowym";
                    name6 = "koralu rogowym";
                    item_description = "Koral rogowy nazywany równie¿ nocnym, jest czarnym koralem podobnym do anielskiej skóry, ale o jednolitym kolorze. Bardzo trudno siê go obrabia, a w bi¿uterii u¿ywa siê go w postaci ga³±zek lub jako kaboszon.";
                    break;
                default:
                    name = "turmalin";
                    sh_desc = "turmalin";
                    name2 = "turmalinu";
                    name3 = "turmalinowi";
                    name4 = "turmalin";
                    name5 = "turmalinem";
                    name6 = "turmalinie";
                    item_description = "";
                    break;
            }
            break;
            // kamienie szlachetne
        case 4:
            choose = number_range( 0, 7 );
            obj->value[ 0 ] = 300 + choose;
            switch ( choose )
            {
                case 1:
                    name = "akwamaryn";
                    sh_desc = "akwamaryn";
                    name2 = "akwamarynu";
                    name3 = "akwamarynowi";
                    name4 = "akwamaryn";
                    name5 = "akwamarynem";
                    name6 = "akwamarynie";
                    item_description = "Akwamaryn jest twardym, przezroczystym typem berylu o b³êkitno-zielonym odcieniu.";
                    break;
                case 2:
                    name = "oliwin";
                    sh_desc = "oliwin";
                    name2 = "oliwinu";
                    name3 = "oliwinowi";
                    name4 = "oliwin";
                    name5 = "oliwinem";
                    name6 = "oliwinie";
                    item_description = "Oliwiny s± w jubilerstwie stosowane jako kamienie ozdobne, najczê¶ciej o zabarwieniu zielonym lub z³ocistozielonkawym, przypominaj±cym kolorystyk± pewne gatunki oliw, sk±d zreszt± pochodzi ich nazwa. Odno¶nie wystêpowania, s± one reliktami protolitowymi wystêpuj±cymi w serpentynitach, a wiêc ich z³o¿a mo¿na spotkaæ przede wszystkim w okolicach Ognistego Oka, Góry Popielnej oraz prastarego wulkanu, którego zniszczone przez erozjê resztki nazywane s± obecnie Brunatnym Garbem. Powszechna jest wiara we w³asno¶ci odtruwaj±ce oliwinów, dlatego, niemal równie szeroko panuj±ca, jest wiara w fakt, ¿e oliwiny s± jednym z ulubionych kamieni Isoryka. Miejsca wystêpowania niezbyt zgadzaj± z obszarami najintensywniejszego kultu, ale kto by tam zwraca³ uwagi na takie drobiazgi?";
                    break;
                case 3:
                    name = "per³a";
                    sh_desc = "per³a";
                    name2 = "per³y";
                    name3 = "perle";
                    name4 = "per³ê";
                    name5 = "per³±";
                    name6 = "perle";
                    item_description = "Per³a sk³ada siê z warstw aragonitu, powoli nak³adanych na ziarnko piasku lub inny podobny obiekt, który dostanie siê do ¶rodka muszli ostrygi lub innych miêczaków. Powsta³y w ten sposób klejnot ma g³êboki po³ysk. Wiêkszo¶æ pere³ jest bia³a, jednak wystêpuj± równie¿ du¿o rzadsze odmiany czarne i têczowe.";
                    break;
                case 4:
                    name = "spinel";
                    sh_desc = "spinel";
                    name2 = "spinel";
                    name3 = "spinelowi";
                    name4 = "spinel";
                    name5 = "spinelem";
                    name6 = "spinelu";
                    item_description = "Spinele tworz± ostrokrawêdziowe kryszta³y o charakterystycznym blasku, przypominaj±cym ¶wiat³o ognia - od tych w³asno¶ci pochodzi ich nazwa. Czysty spinel tworzy bezbarwne kryszta³y, jednak stosunkowo du¿y udzia³ maj± równie¿ formy domieszkowane, o zabarwieniu czarnym, czarnoszarym, czerwonym o rubinowym odcieniu, niebieskawym w odcieniu szmaragdowym itd. Najczê¶ciej maj± barwê czarn±, szaroczarn± lub brunatnoczarn±. Odmiany szlachetne, pó³szlachetne i pozosta³e spotykane s± rzadziej, przy posiadaj± znacznie atrakcyjniejsz± gamê barw ni¿ pospolite spinele rudne. Spinele o niewielkim zabarwieniu oraz te bezbarwne nazywane s± spinelami w³a¶ciwymi, natomiast pozosta³e odmiany maj± w³asne powszechnie u¿ywane nazwy, jak cejlonit, pleonast, rubicel, czy hercynit oraz wiele innych. Wszystkie odmiany spineli tworz± siê w wysokich temperaturach i dlatego wystêpuj± w okolicach aktywnych obecnie lub w czasach prehistorycznych wulkanów Tamery. Ich pewne odmiany upodobali sobie bogowie. Dla przyk³adu wystêpuj±ce na zboczach Góry Popielnej rubiny spinelowe oraz balas rubiny s± pociech± dla oka Istovirtha - kr±¿± plotki, ¿e jeden z wykonanych z takich rubinów diademów obdarzy³ on tak± moc±, ¿e wt³aczanej przezeñ m±dro¶ci niejedna g³owa nie wytrzyma³a. Natomiast pleonasty, które znale¼æ mo¿na jedynie w nurcie rzeki Trin'Ehr, wymywaj±cej je prawdopodobnie ze starszych, wygas³ych w prehistorycznych czasach wulkanów, upodobali sobie kap³ani Vastaina i chyba wiedz± dlaczego - mroczne legendy o ludach zniewolonych Kosturem Tyranii, którego pleonast by³ elementem zasadniczym, do dzi¶ s± u¿ywane do opanowania zapêdów do w³adzy prezentowanych przez kolejne pokolenia podrostków.";
                    break;
                case 5:
                    name = "shandon";
                    sh_desc = "shandon";
                    name2 = "shandonu";
                    name3 = "shandonowi";
                    name4 = "shandon";
                    name5 = "shandonem";
                    name6 = "shandonie";
                    item_description = "Smuk³y, bezbarwny kryszta³ o piêknie oszlifowanych ¶ciankach, u¿ywanego czêsto do przyozdabiania welonów i szat.";
                    break;
                case 6:
                    name = "szafir gwia¼dzisty";
                    sh_desc = "szafir gwia¼dzisty";
                    name2 = "szafiru gwia¼dzistego";
                    name3 = "szfairowi gwia¼dzistemu";
                    name4 = "szafir gwia¼dzisty";
                    name5 = "szafirem gwia¼dzistym";
                    name6 = "szafirze gwia¼dzistym";
                    item_description = "Gwiezdny szafir to rzadka odmiana szafiru, bardziej przejrzysta, z bia³± gwiazd± widoczn± w ¶rodku. U¿ywany jest do ozdabiania przedmiotów, chroni±cych przed magi±.";
                    break;
                case 7:
                    name = "tchzar";
                    sh_desc = "tchzar";
                    name2 = "tchzaru";
                    name3 = "tchzarowi";
                    name4 = "tchzar";
                    name5 = "tchzarem";
                    name6 = "tchzarze";
                    item_description = "Tchzar jest powszechn± nazw±, nadawan± aragonitowi s³omiano¿ó³tego kamienia szlachetnego maj±cego postaæ wyd³u¿onego kryszta³u o kszta³cie graniastos³upa. Jest delikatny i kruchy; potrzeba niez³ego jubilera, by go oszlifowaæ. ";
                    break;
                default:
                    name = "topaz";
                    sh_desc = "topaz";
                    name2 = "topazu";
                    name3 = "topazowi";
                    name4 = "topaz";
                    name5 = "topazem";
                    name6 = "topazie";
                    item_description = "";
                    break;
            }
            break;
            // drogie kamienie
        case 5:
            choose = number_range( 0, 9 );
            obj->value[ 0 ] = 400 + choose;
            switch ( choose )
            {
                case 1:
                    name = "czarny opal";
                    sh_desc = "czarny opal";
                    name2 = "czarnego opalu";
                    name3 = "czarnemu opalowi";
                    name4 = "czarny opal";
                    name5 = "czarnym opalem";
                    name6 = "czarnym opalu";
                    item_description = "Czarny opal ma zielonkawy kolor z czarnymi plamkami i z³otymi cêtkami. Najczê¶ciej wystêpuje w starych gor±cych ¼ród³ach i jest sprzedawany jako kaboszon. Popularne w Carrallak powiedzenie 'Czarny jak opal' jest u¿ywane, jako forma pochwa³y dla mi³ych z³odziejaszków i tym podobnych.";
                    break;
                case 2:
                    name = "ognisty opal";
                    sh_desc = "ognisty opal";
                    name2 = "ognistego opalu";
                    name3 = "ognistemu opalowi";
                    name4 = "ognisty opal";
                    name5 = "ognistym opalem";
                    name6 = "ognistym opalu";
                    item_description = "";
                    break;
                case 3:
                    name = "opal";
                    sh_desc = "opal";
                    name2 = "opalu";
                    name3 = "opalowi";
                    name4 = "opal";
                    name5 = "opalem";
                    name6 = "opalu";
                    item_description = "";
                    break;
                case 4:
                    name = "szafir";
                    sh_desc = "szafir";
                    name2 = "szafira";
                    name3 = "szafirowi";
                    name4 = "szafir";
                    name5 = "szafirze";
                    name6 = "szafirem";
                    item_description = "";
                    break;
                case 5:
                    name = "wschodni ametyst";
                    sh_desc = "wschodni ametyst";
                    name2 = "wschodniego ametystu";
                    name3 = "wschodniemu ametystowi";
                    name4 = "wschodni ametyst";
                    name5 = "wschodnim ametystem";
                    name6 = "wschodnim ametyscie";
                    item_description = "";
                    break;
                case 6:
                    name = "dipsoid gwia¼dzisty";
                    sh_desc = "dipsoid gwia¼dzisty";
                    name2 = "dipsoidu gwia¼dzistego";
                    name3 = "dipsoidowi gwia¼dzistemu";
                    name4 = "dipsoid gwia¼dzisty";
                    name5 = "dipsoidem gwia¼dzistym";
                    name6 = "dispoidzie gwia¼dzistym";
                    item_description = "Diopsyd Gwia¼dzisty jest najbardziej poszukiwan± odmian± twardego, wytrzyma³ego kamienia szlachetnego, który rzadko przybiera atrakcyjne kolory. Jest zbyt ciemny, by uznaæ go za piêkny, jednak¿e górskie i rzeczne kryszta³y w jasnozielonym odcieniu czyni± go atrakcyjnym. Kiedy znaleziony kamieñ jest ciemnozielony mo¿na go oszlifowaæ w taki sposób, aby powsta³a z niego cztero lub sze¶cioramienna gwiazda.";
                    break;
                case 7:
                    name = "sfin";
                    sh_desc = "sfin";
                    name2 = "sfinu";
                    name3 = "sfinowi";
                    name4 = "sfin";
                    name5 = "sfinem";
                    name6 = "sfinie";
                    item_description = "Sfin jest delikatnym i kruchym klejnotem (czêsto obrabianym przez niedo¶wiadczonych jubilerów) o kolorach od ¿ó³tego do zielonego; najcenniejsze s± szmaragdowozielone.";
                    break;
                case 8:
                    name = "królewska ³za";
                    sh_desc = "królewska ³za";
                    name2 = "królewskiej ³zy";
                    name3 = "królewsk±ej ³zie";
                    name4 = "królewsk± ³zê";
                    name5 = "królewsk± ³z±";
                    name6 = "królewskiej ³zie";
                    item_description = "Królewskie ³zy s± bardzo rzadkie. S± one przezroczyste, w kszta³cie ³zy, o g³adkiej powierzchni i jak do tej pory nie uda³o siê znale¼æ sposobu na ich zniszczenie. Podobno s± to skamienia³e ³zy dawno zmar³ego króla-nekromanty. W ka¿dym klejnocie mo¿na zobaczyæ to, co kocha³ p³acz±cy król. Ich prawdziwa natura jest nieznana, ale s± one bardzo cenione przez mêdrców. ";
                    break;
                case 9:
                    name = "wodny opal";
                    sh_desc = "wodny opal";
                    name2 = "wodnego opalu";
                    name3 = "wodnemu opalowi";
                    name4 = "wodny opal";
                    name5 = "wodnym opalem";
                    name6 = "wodnym opalu";
                    item_description = "Wodny opal jest bezbarwnym, przezroczystym opalem. Jest rzadki i bardzo cenny, poniewa¿ u¿ywany jest do dalekowidzenia. ";
                    break;
                default:
                    name = "wschodni topaz";
                    sh_desc = "wschodni topaz";
                    name2 = "wschodniego topazu";
                    name3 = "wschodniemu topazowi";
                    name4 = "wschodni topaz";
                    name5 = "wschodnim topazem";
                    name6 = "wschodnim topazie";
                    item_description = "";
                    break;
            }
            break;
            // klejnoty
        case 6:
            choose = number_range( 0, 8 );
            obj->value[ 0 ] = 500 + choose;
            switch ( choose )
            {
                case 1:
                    name = "czarny szafir";
                    sh_desc = "czarny szafir";
                    name2 = "czarnego szafiru";
                    name3 = "czarnemu szafirowi";
                    name4 = "czarny szafir";
                    name5 = "czarnym szafirem";
                    name6 = "czarnym szafirze";
                    item_description = "";
                    break;
                case 2:
                    name = "diament";
                    sh_desc = "diament";
                    name2 = "diamentu";
                    name3 = "diamentowi";
                    name4 = "diament";
                    name5 = "diamentem";
                    name6 = "diamencie";
                    item_description = "Diament jest twardym, przezroczystym klejnotem; mo¿e byæ bia³o niebieski, b³êkitny, ¿ó³ty lub ró¿owy. Jest najtwardszym ze wszystkich klejnotów i jednym z najcenniejszych. Wiêkszo¶æ diamentów pochodzi z pó³nocnych gór, gdzie wydobywane s± przez podziemne rasy, w ten sposób dostaj±c siê na powierzchniê. ";
                    break;
                case 3:
                    name = "gwiezdny rubin";
                    sh_desc = "gwiezdny rubin";
                    name2 = "gwiezdnego rubinu";
                    name3 = "gwiezdnemu rubinowi";
                    name4 = "gwiezdny rubin";
                    name5 = "gwiezdnym rubinem";
                    name6 = "gwiezdnym rubinie";
                    item_description = "";
                    break;
                case 4:
                    name = "gwiezdny szafir";
                    sh_desc = "gwiezdny szafir";
                    name2 = "gwiezdnego szafiru";
                    name3 = "gwiezdnemu szafirowi";
                    name4 = "gwiezdny szafir";
                    name5 = "gwiezdnym szafirem";
                    name6 = "gwiezdnym szafirze";
                    item_description = "";
                    break;
                case 5:
                    name = "hiacynt";
                    sh_desc = "hiacynt";
                    name2 = "hiacyntu";
                    name3 = "hiacyntowi";
                    name4 = "hiacynt";
                    name5 = "hiacyntem";
                    name6 = "hiacyncie";
                    item_description = "";
                    break;
                case 6:
                    name = "rubin";
                    sh_desc = "rubin";
                    name2 = "rubinu";
                    name3 = "rubinowi";
                    name4 = "rubin";
                    name5 = "rubinem";
                    name6 = "rubinie";
                    item_description = "";
                    break;
                case 7:
                    name = "szmaragd";
                    sh_desc = "szmaragd";
                    name2 = "szmaragdu";
                    name3 = "szmaragdowi";
                    name4 = "szmaragd";
                    name5 = "szmaragdem";
                    name6 = "szmaragdzie";
                    item_description = "Szmaragd jest l¶ni±cym zielonym berylem, oszlifowanym wed³ug ustalonego wzoru. Jest on czêsto kojarzony ze zdrowiem i bywa u¿ywany do ozdabiania przedmiotów ze zdrowiem zwi±zanych. ";
                    break;
                case 8:
                    name = "kamieñ ³otrzyków";
                    sh_desc = "kamieñ ³otrzyków";
                    name2 = "kamienia ³otrzyków";
                    name3 = "kamieniowi ³otrzyków";
                    name4 = "kamieñ ³otrzyków";
                    name5 = "kamieniem ³otrzyków";
                    name6 = "kamieniu ³otrzyków";
                    item_description = "Cudowny, wielokolorowy kamieñ, b³yszcz±cy siê wszystkimi kolorami têczy. W ¶wietle dnia jego kolory wydaj± siê stale zmieniaæ, co jest wyj±tkowo piêknym widokiem.";
                    break;
                default:
                    name = "wschodni szmaragd";
                    sh_desc = "wschodni szmaragd";
                    name2 = "wschodniego szmaragdu";
                    name3 = "wschodniemu szmaragdowi";
                    name4 = "wschodni szmaragd";
                    name5 = "wschodnim szmaragdem";
                    name6 = "wschodnim szmaragdzie";
                    item_description = "";
                    break;
            }
            break;
            // super klejnoty
        case 7:
            choose = number_range( 0, 15 );
            obj->value[ 0 ] = 600 + choose;
            switch ( choose )
            {
                case 1:
                    name = "agronit";
                    sh_desc = "agronit";
                    name2 = "agronitu";
                    name3 = "agronitowi";
                    name4 = "agronit";
                    name5 = "agronitem";
                    name6 = "agronicie";
                    item_description = "Agronit to rzadki kamieñ o naprawdê wyj±tkowej budowie. Jest twardy, choæ sprawia wra¿enie miêkkiego. Przepuszcza ¶wiat³o, choæ jakby trochê inne.";
                    break;
                case 2:
                    name = "furgasyd";
                    sh_desc = "furgasyd";
                    name2 = "furgasydu";
                    name3 = "furgasydowi";
                    name4 = "furgasyd";
                    name5 = "furgasydem";
                    name6 = "furgasydzie";
                    item_description = "Furgasyt to bardzo nietypowy klejnot o wielkim znaczeniu w czarach dotycz±cych osnowy ¶wiata i w³asno¶ci materii.";
                    break;
                case 3:
                    name = "maronid";
                    sh_desc = "maronid";
                    name2 = "maronidu";
                    name3 = "maronidowi";
                    name4 = "maronid";
                    name5 = "maronidem";
                    name6 = "maronidzie";
                    item_description = "Maronid to do¶æ dziwny kamieñ w ¶rodku którego widaæ ca³y ¶wiat. Niewa¿ne jak ma³y jest ten kamieñ i tak patrz±c do jego wnêtrza mo¿na dostrzec wielkie rzeczy.";
                    break;
                case 4:
                    name = "deggialryt";
                    sh_desc = "deggialryt";
                    name2 = "deggialrytu";
                    name3 = "deggialrytowi";
                    name4 = "deggialryt";
                    name5 = "deggialrytem";
                    name6 = "deggialrycie";
                    item_description = "Mroczny, czarny kamieñ o z³owieszczej, przera¿aj±cej aurze. Trzymaj±c go w d³oni do¶wiadczasz wra¿enia odp³ywania twoich si³ zyciowych. Czujesz pote¿n±, pulsuj±c± w tym kamieniu moc.";
                    break;
                case 5:
                    name = "razoryl";
                    sh_desc = "razoryl";
                    name2 = "razorylu";
                    name3 = "razorylowi";
                    name4 = "razoryl";
                    name5 = "razorylem";
                    name6 = "razorylu";
                    item_description = "Najstarszy ze znanych kamieni o prawie nieograniczonej mocy, choæ niestety wiedza o tym jak go wykorzystaæ zagine³a gdzie¶ w pomrokach czasów.";
                    break;
                case 6:
                    name = "azradunin";
                    sh_desc = "azradunin";
                    name2 = "azraduninu";
                    name3 = "azraduninowi";
                    name4 = "azradunin";
                    name5 = "azraduninem";
                    name6 = "azraduninie";
                    item_description = "Kamieñ ¶mia³ków i b³aznów, czasem zaskakuj±cy rozb³yskami wewnêtrznej energi. Niezwykle cenny.";
                    break;
                case 7:
                    name = "feerialyd";
                    sh_desc = "feerialyd";
                    name2 = "feerialydu";
                    name3 = "feerialydowi";
                    name4 = "feerialyd";
                    name5 = "feerialydem";
                    name6 = "feerialydzie";
                    item_description = "Feerialyd jest l¶ni±cym zielonym kamieniem który pozwala na zniewelowanie trudów ciê¿kiej podró¿y.";
                    break;
                case 8:
                    name = "garloopel";
                    sh_desc = "garloopel";
                    name2 = "garloopela";
                    name3 = "garloopelowi";
                    name4 = "garloopel";
                    name5 = "garloopelem";
                    name6 = "garloopelu";
                    item_description = "Ten dziwny, metaliczny, po³yskuj±cy, czarny kamieñ w kszta³cie ³zy wygl±da bardzo dziwnie i fascynuj±co, jest w nim co¶ niezwyk³ego.";
                    break;
                case 9:
                    name = "tanonir";
                    sh_desc = "tanonir";
                    name2 = "tanonira";
                    name3 = "tanonirowi";
                    name4 = "tanonir";
                    name5 = "tanonirem";
                    name6 = "tanonirze";
                    item_description = "Gdy patrzysz na niemal czarnej barwy kawa³ek po³yskuj±cego w ¶wietle lekko fioletem kamienia nie widzisz nic specjalnego. Dopiero gdy patrzysz przez niego na ¶wiat³o dostrzegasz dlaczego jest to tak cenny klejnot. Otó¿ ¶wiat³o za³amane wewn±trz klejnotu b³±kaj±c siê gdzie¶ miêdzy jego wewnêtrznymi ¶ciankami sprawia wra¿enie jakby co¶ w nim siedzia³o. Zielonkawe do z³udzenia przypominaj±ce cz³owieka postacie pojawiaj± siê i znikaj± przy poruszaniu klejnotem. Jest to oczywi¶cie tylko z³udzenie, które zaowocowa³o powstaniem wielu mitów na temat tego minera³u. Klejnot posiada jeszcze jedn± ciekaw± cechê, otó¿ gdy skryje siê go przed ¶wiat³em, emanuje ledwo zauwa¿alnym ¶wiat³em, które mo¿e mieæ barwê od zieleni, po krwist± czerwieñ. Pradawni w³adcy wydawali ca³e maj±tki aby zaopatrzyæ siê w jeden z tych niezwykle rzadkich klejnotów wierz±c, ¿e pomog± one przeprawiæ ich na drug± stronê i dadz± im w³adzê w za¶wiatach nad umar³ymi. Pewna legenda mówi, i¿ z³a bogini Sa'Fuhr da³a swoim kap³anom te klejnoty, aby zbierali oni w nich dusze mê¿nych wojowników, które w przysz³o¶ci mia³yby walczyæ dla niej siej±c strach i zniszczenie. Mimo to jubilerzy ceni± sobie ten klejnot nie ze wzglêdu na legendy, ale z powodu, ¿e odpowiednio oszlifowane potrafi± ¶wieciæ nawet i w s³abiej o¶wietlonym pomieszczeniu, a zamo¿ni panowie lubi± siê wyró¿niaæ.";
                    break;
                case 10:
                    name = "drakolid";
                    sh_desc = "drakolid";
                    name2 = "drakolida";
                    name3 = "drakolidzie";
                    name4 = "drakolid";
                    name5 = "drakolidem";
                    name6 = "drakolidzie";
                    switch ( number_range ( 0, 1 ) )
                    {
                        case 1:
                            item_description = "Drakolidy tworz± siê w wyniku stopienia i pó¼niejszej krystalizacji ska³, z których utworzony jest Smoczy Grzbiet, dzieje siê tak, gdy wydobywaj±cy siê z paszczy rozjuszonej bestii p³omienny ¿ar, zamiast strawiæ nêkaj±c± stwora grupê ¶mia³ków na popió³ trafi w tworz±ce grañ minera³y z odpowiedni± si³±. Zjawisko to nader rzadkie, a i wyprawa po nie jak¿e trudna, ze wzglêdu na ostre warunki klimatyczne oraz naje¿ona niebezpieczeñstwami, bêd±cymi skutkiem licznych agresywnych reliktów minionej przesz³o¶ci, o pradawnej, niemal niewyobra¿alnej mocy, których trudno unikn±æ na tej trasie. Dlatego drakolidy s± jednymi z najcenniejszych klejnotów Tamery. Ze wzglêdu na potêgê strzeg±cych je smoków, ¿aden z bogów nie zaryzykowa³ uznania drakolidów za sferê swoich wp³ywów.";
                            break;
                        default:
                            item_description = "Na pierwszy rzut oka jest to niewielka kula wielko¶ci oka, lecz gdy przetrze siê j± od razu widaæ jej przepiêkne wnêtrze. W samym centrum znajduje siê idealnie czarna kulka otoczona przez mieni±c± siê tysi±cem barw, nieco wiêksz± pow³okê. Gdy wpatrujesz siê coraz bardziej przenika ciê dziwne uczucie, jakoby owy klejnot patrzy³ na ciebie. Odrzucasz t± nieprzyjemn± my¶l i powracasz bowiem do tego czym jest ten skarb. Drakolid bowiem jest to trójwarstwowy, niezwykle rzadki i otoczony licznymi legendami kamieñ szlachetny. Jego pow³oka jest czystym wypolerowanym diamentem, g³êbiej za¶ znajduje siê warstwa potocznie nazywana przez jubilerów têczówk±. Przypuszcza siê ¿e têczówka zbudowana jest z innych cennych, pokruszonych klejnotów zamkniêtych pow³ok± z diamentu. Wewn±trz têczówki znajduje siê samo serce mistycznego skarbu. Przypuszcza siê, i¿ jest ono zbudowane z czarnego topazu lub innego rzadkiego minera³u, ale nikt nie by³ na tyle g³upi aby obrabiaæ, a co wiêcej badaæ te kamienie - to¿ by³by g³upcem niszcz±c tak rzadki klejnot. Gdy obracasz drakolid w ¶wietle wydaje ci siê jakby jego wnêtrze p³onê³o, a ¶wiat³o rozbija³o siê na setki tysiêcy barw gasn±c w samym centrum. Jedna z legend mówi, i¿ klejnoty te powstaj± z ocz±t nienarodzonych smoków, które wykradzione przez chciwe rasy koñcz± w piachu gdzie¶ na krañcu ¶wiata. Za¶ zrozpaczone smoki zbieraj± pozosta³o¶ci po swych dzieciach. Ta legenda wziê³a siê z tego ¿e drakolidy mo¿na znale¼æ najczê¶ciej w smoczych jamach. Wed³ug licznych teorii jubilerów i znanych gemmologów drakolidy powsta³y w rzeczywisto¶æi w bardzo ekstremalnych temperaturach, które powstaj± na wskutek smoczych zioniêæ. W ten sposób liczne klejnoty zawarte w ska³ach smoczych jam stapiaj± siê ze sob±. Kszta³t za¶ t³umacz± nieustaj±c± erozj±, która szlifuje kamienie do kulistego kszta³tu. Oczywi¶cie zdarzaj± siê nieidealne drakolidy, których kszta³t odbiega od kulistego ale s± one oczywi¶cie du¿o mniej warte.";
                            break;
                    }
                    break;
                case 11:
                    name = "kaintid";
                    sh_desc = "kaintid";
                    name2 = "kaintidu";
                    name3 = "kaintidzie";
                    name4 = "kaintid";
                    name5 = "kaintidem";
                    name6 = "kaintidzie";
                    item_description = "Kaintidy s± niezwykle rzadkimi reliktami protolitowymi wystêpuj±cymi w osnowie serpentynitowej, bêd±cej skutkiem regionalnego epi metamorfizmu niskiego stopnia. Udaje siê je czasem znale¼æ w jednym ze z³o¿y oliwinów, w którym, u ¼róde³ prawego dop³ywu Trin'Ehru wyp³ywaj±cych ze zboczy Ognistego Oka, serpentynity utworzy³y siê w wyniku metasomatozy hydrotermalnej. Trudno znale¼æ znawcê, który z czystym sumieniem i pewno¶ci± powie, do którego z bogów mo¿na te klejnoty przyporz±dkowaæ. Niektórzy twierdz±, ¿e upodoba³ sobie je Vastain, gdy¿ po³udniowo-wschodnie zbocza Czarnych Gór s± miejscem wystêpowania ognisk jego kultu. Inni twierdz±, ¿e opisywanymi cennymi klejnotami szczególnie zainteresowany jest Lanseril, gdy¿ do ich powstania potrzebna jest kojarzona z ogniem wysoka temperatura. Jeszcze inni twierdz±, ¿e tak rzadki klejnot mo¿e nale¿eæ jedynie do Mea'Eridu i wobec odmiennej opinii s± gotowi nawet zabiæ lub oddaæ ¿ycie, nie bacz±c na fakt, ¿e Stworzyciel Wszystkiego ma swoje dzie³o g³êboko w powa¿aniu, ironiczne rzecz ujmuj±c.";
                    break;
                case 12:
                    name = "harendid";
                    sh_desc = "harendid bli¼niaczy";
                    name2 = "harenditu";
                    name3 = "harenditowi";
                    name4 = "harendit";
                    name5 = "harenditem";
                    name6 = "harendicie";
                    item_description = "Unikatowy kamieñ sk³adaj±cy siê w³a¶ciwie z dwóch czê¶ci: harendilitu i harendalitu.";
                    break;
                case 13:
                    name = "s³abe oko nila";
                    sh_desc = "s³abe oko nila";
                    name2 = "s³abe oko nila";
                    name3 = "s³abemu oku nila";
                    name4 = "s³abe oko nila";
                    name5 = "s³abym okiem nila";
                    name6 = "s³abym oku nila";
                    item_description = "Zadziwiaj±cy kamieñ o przedziwnych w³asno¶ciach. Niby miêkki, ale jakby sprobowaæ go ¶cisn±æ to nic siê nie dzieje.";
                    break;
                case 14:
                    name = "karlamid";
                    sh_desc = "karlamid b³yszcz±cy";
                    name2 = "karlamitu";
                    name3 = "karlamitowi";
                    name4 = "karlamit";
                    name5 = "karlamitem";
                    name6 = "karlamicie";
                    item_description = "Przera¿aj±co b³yszcz±cy kamieñ o porowatej strukturze, która nie powinna b³yszczeæ, a b³yszczy. Dziwne do¶æ.";
                    break;
                case 15:
                    name = "risanel";
                    sh_desc = "risanel";
                    name2 = "risanela";
                    name3 = "risanelowi";
                    name4 = "risanel";
                    name5 = "risanelem";
                    name6 = "risanelu";
                    item_description = "Kamieñ wygl±da jak kañciasty od³upek czego¶ powrzechnego, ale to b³êdna i niedok³adna ocena.";
                    break;
                default:
                    name = "gurthgyr";
                    sh_desc = "gurthgyr";
                    name2 = "gurthgyra";
                    name3 = "gurthgyrowi";
                    name4 = "gurthgyr";
                    name5 = "gurthgyrem";
                    name6 = "gurthgyrze";
                    item_description = "Dziwny ten kamieñ, pogr±¿ony we w³asnym bycie-niebycie, dobry do czego¶ bardzo nieokre¶lonego.";
                    break;
            }
            obj->is_spell_item = TRUE;
            obj->spell_item_counter = 4;
            obj->spell_item_timer = ( 60 * 500 ); //500 godzin rzeczywistego czasu
            break;
        default:
            obj->cost = 1;
            break;
    }

    //tworzenie przedmiotu
    free_string( obj->short_descr );
    obj->short_descr = str_dup( sh_desc );

    free_string( obj->name );
    obj->name = str_dup( name );

    free_string( obj->name2 );
    obj->name2 = str_dup( name2 );

    free_string( obj->name3 );
    obj->name3 = str_dup( name3 );

    free_string( obj->name4 );
    obj->name4 = str_dup( name4 );

    free_string( obj->name5 );
    obj->name5 = str_dup( name5 );

    free_string( obj->name6 );
    obj->name6 = str_dup( name6 );

    free_string( obj->item_description );
    obj->item_description = str_dup( item_description );

    free_string( obj->description );
    sprintf( text, "%s le¿y tutaj.", capitalize( sh_desc ) );
    obj->description = str_dup( text );

    //rellik: komponenty, ustawianie go jako komponent je¶li wcze¶niej nie by³o inaczej ustawione
    if ( !obj->is_spell_item )
    {
        obj->is_spell_item = TRUE;
        obj->spell_item_counter = 10;
        obj->spell_item_timer = ( 60 * 500 ); //500 godzin rzeczywistego czasu
    }

    return;
}

void generate_cape( OBJ_DATA *obj, int* points )
{
	BITVECT_DATA * bitv = &AFF_NONE;
	char text[ MAX_INPUT_LENGTH ], *name = "(null)", *sh_desc = "(null)";
	char *name2 = "null", *name3 = "null", *name4 = "null", *name5 = "null", *name6 = "null";
	bool second = FALSE, third = FALSE;
	int type = 0, loc = 0, level = 0, mod = 0, kasa = 1;

	if ( *points > 3 || *points > number_range(0,6) ) second = TRUE;
	if ( *points > 6 || *points > number_range(3,10) ) third = TRUE;

	obj->cost = 10;
	obj->weight = 10;
	obj->material = 44;

	generate_extra( obj );

	switch ( number_range( 1, 39 ) )
	{
		case 1:
			name = "skórzana kapa";
			sh_desc = "skórzana kapa";
			name2 = "skórzanej kapy";
			name3 = "skórzanej kapie";
			name4 = "skórzan± kapê";
			name5 = "skórzan± kap±";
			name6 = "skórzanej kapie";
			break;
		case 2:
			name = "zwyk³a szata";
			sh_desc = "zwyk³a szata";
			name2 = "zwyk³ej szaty";
			name3 = "zwyk³ej szacie";
			name4 = "zwyk³± szatê";
			name5 = "zwyk³± szat±";
			name6 = "zwyk³ej szacie";
			break;
		case 3:
			name = "gruby p³aszcz";
			sh_desc = "gruby p³aszcz";
			name2 = "grubego p³aszcza";
			name3 = "grubemu p³aszczowi";
			name4 = "gruby p³aszcz";
			name5 = "grubym p³aszczem";
			name6 = "grubym p³aszczu";
			break;
		case 4:
			name = "delikatna peleryna";
			sh_desc = "delikatna peleryna";
			name2 = "delikatnej peleryny";
			name3 = "delikatnej pelerynie";
			name4 = "delikatn± pelerynê";
			name5 = "delikatn± peleryn±";
			name6 = "delikatnej pelerynie";
			break;
		case 5:
			name = "d³uga, poplamiona krwi± szata";
			sh_desc = "d³uga, poplamiona krwi± szata";
			name2 = "d³ugiej, poplamionej krwi± szaty";
			name3 = "d³ugiej, poplamionej krwi± szacie";
			name4 = "d³ug±, poplamion± krwi± szatê";
			name5 = "d³ug±, poplamion± krwi± szat±";
			name6 = "d³ugiej, poplamionej krwi± szacie";
			EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
			break;
		case 6:
			name = "obszerny p³aszcz z d³ugim kapturem";
			sh_desc = "obszerny p³aszcz z d³ugim kapturem";
			name2 = "obszernego p³aszcza z d³ugim kapturem";
			name3 = "obszernemu p³aszczowi z d³ugim kapturem";
			name4 = "obszerny p³aszcz z d³ugim kapturem";
			name5 = "obszernym p³aszczem z d³ugim kapturem";
			name6 = "obszernym p³aszczu z d³ugim kapturem";
			break;
		case 7:
			name = "bia³y habit obszyty z³ot± nitk±";
			sh_desc = "bia³y habit obszyty z³ot± nitk±";
			name2 = "bia³ego habitu obszytego z³ot± nitk±";
			name3 = "bia³emu habitowi obszytemu z³ot± nitk±";
			name4 = "bia³y habit obszyty z³ot± nitk±";
			name5 = "bia³ym habitem obszytym z³ot± nitk±";
			name6 = "bia³ym habicie obszytym z³ot± nitk±";
			EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_EVIL );
			EXT_SET_BIT( obj->wear_flags2, ITEM_NOTHI );
			EXT_SET_BIT( obj->wear_flags2, ITEM_NOBARBARIAN );
			EXT_SET_BIT( obj->wear_flags2, ITEM_NOSHAMAN );
			EXT_SET_BIT( obj->wear_flags2, ITEM_NOWAR );
			EXT_SET_BIT( obj->wear_flags2, ITEM_NOBARD );
			break;
		case 8:
			name = "d³uga czarna szata";
			sh_desc = "d³uga czarna szata";
			name2 = "d³ugiej czarnej szaty";
			name3 = "d³ugiej czarnej szacie";
			name4 = "d³ug± czarn± szatê";
			name5 = "d³ug± czarn± szat±";
			name6 = "d³ugiej czarnej szacie";
			break;
		case 9:
			name = "emanuj±ca z³± aur± rytualna szata";
			sh_desc = "emanuj±ca z³± aur± rytualna szata";
			name2 = "emanuj±cej z³± aur± rytualnej szaty";
			name3 = "emanuj±cej z³± aur± rytualnej szacie";
			name4 = "emanuj±c± z³± aur± rytualn± szatê";
			name5 = "emanuj±c± z³± aur± rytualn± szat±";
			name6 = "emanuj±cej z³± aur± rytualnej szacie";
			EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
			break;
		case 10:
			name = "czarna opoñcza zabójcy";
			sh_desc = "czarna opoñcza zabójcy";
			name2 = "czarnej opoñczy zabójcy";
			name3 = "czarnej opoñczy zabójcy";
			name4 = "czarn± opoñczê zabójcy";
			name5 = "czarn± opoñcz± zabójcy";
			name6 = "czarnej opoñczy zabójcy";
			EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
			EXT_SET_BIT( obj->wear_flags2, ITEM_ONLYTHI );
			break;
		case 11:
			name = "szata mrocznego maga";
			sh_desc = "szata mrocznego maga";
			name2 = "szaty mrocznego maga";
			name3 = "szacie mrocznego maga";
			name4 = "szatê mrocznego maga";
			name5 = "szat± mrocznego maga";
			name6 = "szacie mrocznego maga";
			EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
			EXT_SET_BIT( obj->wear_flags2, ITEM_ONLYMAG );
			break;
		case 12:
			name = "szata kap³ana chaosu";
			sh_desc = "szata kap³ana chaosu";
			name2 = "szaty kap³ana chaosu";
			name3 = "szacie kap³ana chaosu";
			name4 = "szatê kap³ana chaosu";
			name5 = "szat± kap³ana chaosu";
			name6 = "szacie kap³ana chaosu";
			EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
			EXT_SET_BIT( obj->wear_flags2, ITEM_ONLYCLE );
			break;
		case 13:
			name = "cienista opoñcza";
			sh_desc = "cienista opoñcza";
			name2 = "cienistej opoñczy";
			name3 = "cienistej opoñczy";
			name4 = "cienist± opoñczê";
			name5 = "cienist± opoñcz±";
			name6 = "cienistej opoñczy";
			break;
		case 14:
			name = "br±zowy p³aszcz podró¿ny";
			sh_desc = "br±zowy p³aszcz podró¿ny";
			name2 = "br±zowego p³aszcza podró¿nego";
			name3 = "br±zowemu p³aszczowi podró¿nemu";
			name4 = "br±zowy p³aszcz podró¿ny";
			name5 = "br±zowym p³aszczem podró¿nym";
			name6 = "br±zowym p³aszczu podró¿nym";
			break;
		case 15:
			name = "szary p³aszcz nieprzemakalny";
			sh_desc = "szary p³aszcz nieprzemakalny";
			name2 = "szarego p³aszcza nieprzemakalnego";
			name3 = "szaremu p³aszczowi nieprzemakalnemu";
			name4 = "szary p³aszcz nieprzemakalny";
			name5 = "szarym p³aszczem nieprzemakalnym";
			name6 = "szarym p³aszczu nieprzemakalnym";
			break;
		case 16:
			name = "ciep³a we³niana opoñcza";
			sh_desc = "ciep³a we³niana opoñcza";
			name2 = "ciep³ej we³nianej opoñczy";
			name3 = "ciep³ej we³nianej opoñczy";
			name4 = "ciep³± we³nian± opoñczê";
			name5 = "ciep³± we³nian± opoñcz±";
			name6 = "ciep³ej we³nianej opoñczya";
			obj->material = 26;
			break;
		case 17:
			name = "d³uga czarna peleryna";
			sh_desc = "d³uga czarna peleryna";
			name2 = "d³ugiej czarnej peleryny";
			name3 = "d³ugiej czarnej pelerynie";
			name4 = "d³ug± czarn± pelerynê";
			name5 = "d³ug± czarn± peleryn±";
			name6 = "d³ugiej czarnej pelerynie";
			break;
		case 18:
			name = "czerwony p³aszcz ozdobny";
			sh_desc = "czerwony p³aszcz ozdobny";
			name2 = "czerwonego p³aszcza ozdobnego";
			name3 = "czerwonemu p³aszczowi ozdobnemu";
			name4 = "czerwony p³aszcz ozdobny";
			name5 = "czerwonym p³aszczem ozdobnym";
			name6 = "czerwonym p³aszczu ozdobnym";
			break;
		case 19:
			name = "zielony p³aszcz maskuj±cy";
			sh_desc = "zielony p³aszcz maskuj±cy";
			name2 = "zielonego p³aszcza maskuj±cego";
			name3 = "zielonemu p³aszczowi maskuj±cemu";
			name4 = "zielony p³aszcz maskuj±cy";
			name5 = "zielonym p³aszczem maskuj±cym";
			name6 = "zielonym p³aszczu maskuj±cym";
			break;
		case 20:
			name = "krótka bia³a peleryna";
			sh_desc = "krótka bia³a peleryna";
			name2 = "krótkiej bia³ej peleryny";
			name3 = "krótkiej bia³ej pelerynie";
			name4 = "krótk± bia³± pelerynê";
			name5 = "krótk± bia³± peleryn±";
			name6 = "krótkiej bia³ej pelerynie";
			break;
		case 21:
			name = "d³uga purpurowa szata";
			sh_desc = "d³uga purpurowa szata";
			name2 = "d³ugiej purpurowej szaty";
			name3 = "d³ugiej purpurowej szacie";
			name4 = "d³ug± purpurow± szatê";
			name5 = "d³ug± purpurow± szat±";
			name6 = "d³ugiej purpurowej szacie";
			break;
		case 22:
			name = "szata z dziwnego materia³u";
			sh_desc = "szata z dziwnego materia³u";
			name2 = "szaty z dziwnego materia³u";
			name3 = "szacie z dziwnego materia³u";
			name4 = "szatê z dziwnego materia³u";
			name5 = "szat± z dziwnego materia³u";
			name6 = "szacie z dziwnego materia³u";
			obj->material = 40;
			break;
		case 23:
			name = "ciemny p³aszcz z kapturem";
			sh_desc = "ciemny p³aszcz z kapturem";
			name2 = "ciemnego p³aszcza z kapturem";
			name3 = "ciemnemu p³aszczowi z kapturem";
			name4 = "ciemny p³aszcz z kapturem";
			name5 = "ciemnym p³aszczem z kapturem";
			name6 = "ciemnym p³aszczu z kapturem";
			break;
		case 24:
			name = "niebieski herbowy p³aszcz";
			sh_desc = "niebieski herbowy p³aszcz";
			name2 = "niebieskiego herbowego p³aszcza";
			name3 = "niebieskiemu herbowemu p³aszczowi";
			name4 = "niebieski herbowy p³aszcz";
			name5 = "niebieskim herbowym p³aszczem";
			name6 = "niebieskim herbowym p³aszczu";
			break;
		case 25:
			name = "d³uga pow³óczysta szata";
			sh_desc = "d³uga pow³óczysta szata";
			name2 = "d³ugiej pow³óczystej szaty";
			name3 = "d³ugiej pow³óczystej szacie";
			name4 = "d³ug± pow³óczyst± szatê";
			name5 = "d³ug± pow³óczyst± szat±";
			name6 = "d³ugiej pow³óczystej szacie";
			break;
		case 26:
			name = "bia³y postrzêpiony ca³un";
			sh_desc = "bia³y postrzêpiony ca³un";
			name2 = "bia³ego postrzêpionego ca³unu";
			name3 = "bia³emu postrzêpionemu ca³unowi";
			name4 = "bia³y postrzêpiony ca³un";
			name5 = "bia³ym postrzêpionym ca³unem";
			name6 = "bia³ym postrzêpionym ca³unie";
			break;
		case 27:
			name = "starodawna postrzepiona szata";
			sh_desc = "starodawna postrzepiona szata";
			name2 = "starodawnej postrzepionej szaty";
			name3 = "starodawnej postrzepionej szacie";
			name4 = "starodawn± postrzepion± szatê";
			name5 = "starodawn± postrzepion± szat±";
			name6 = "starodawnej postrzepionej szacie";
			break;
		case 28:
			name = "stary dziurawy p³aszcz";
			sh_desc = "stary dziurawy p³aszcz";
			name2 = "starego dziurawego p³aszcza";
			name3 = "staremu dziurawemu p³aszczowi";
			name4 = "stary dziurawy p³aszcz";
			name5 = "starym dziurawym p³aszczem";
			name6 = "starym dziurawym p³aszczu";
			break;
		case 29:
			name = "solidna br±zowa kapota";
			sh_desc = "solidna br±zowa kapota";
			name2 = "solidnej br±zowej kapoty";
			name3 = "solidnej br±zowej kapocie";
			name4 = "solidn± br±zow± kapotê";
			name5 = "solidn± br±zow± kapot±";
			name6 = "solidnej br±zowej kapocie";
			break;
		case 30:
			name = "krótka ró¿owa pelerynka";
			sh_desc = "krótka ró¿owa pelerynka";
			name2 = "krótkiej ró¿owej pelerynki";
			name3 = "krótkiej ró¿owej pelerynce";
			name4 = "krótk± ró¿ow± pelerynkê";
			name5 = "krótk± ró¿ow± pelerynk±";
			name6 = "krótkiej ró¿owej pelerynki";
			break;
		case 31:
			name = "d³ugi we³niany szal";
			sh_desc = "d³ugi we³niany szal";
			name2 = "d³ugiego we³nianego szala";
			name3 = "d³ugiemu we³nianemu szalowi";
			name4 = "d³ugi we³niany szal";
			name5 = "d³ugim we³nianym szalem";
			name6 = "d³ugim we³nianym szalu";
			obj->material = 26;
			break;
		case 32:
			name = "szara opoñcza";
			sh_desc = "szara opoñcza";
			name2 = "szarej opoñczy";
			name3 = "szarej opoñczy";
			name4 = "szar± opoñczê";
			name5 = "szar± opoñcz±";
			name6 = "szarej opoñczy";
			break;
		case 33:
			name = "po³yskuj±ca zielonkawo szata";
			sh_desc = "po³yskuj±ca zielonkawo szata";
			name2 = "po³yskuj±cej zielonkawo szaty";
			name3 = "po³yskuj±cej zielonkawo szacie";
			name4 = "po³yskuj±c± zielonkawo szatê";
			name5 = "po³yskuj±c± zielonkawo szat±";
			name6 = "po³yskuj±cej zielonkawo szaty";
			break;
		case 34:
			name = "szary p³aszcz z kapturem";
			sh_desc = "szary p³aszcz z kapturem";
			name2 = "szarego p³aszcza z kapturem";
			name3 = "szaremu p³aszczowi z kapturem";
			name4 = "szary p³aszcz z kapturem";
			name5 = "szarym p³aszczem z kapturem";
			name6 = "szarym p³aszczu z kapturem";
			break;
		case 35:
			name = "d³ugi postrzêpiony p³aszcz";
			sh_desc = "d³ugi postrzêpiony p³aszcz";
			name2 = "d³ugiego postrzêpionego p³aszcza";
			name3 = "d³ugiemu postrzêpionemu p³aszczowi";
			name4 = "d³ugi postrzêpiony p³aszcz";
			name5 = "d³ugim postrzêpionym p³aszczem";
			name6 = "d³ugim postrzêpionym p³aszczu";
			break;
		case 36:
			name = "p³aszcz z li¶ci";
			sh_desc = "p³aszcz z li¶ci";
			name2 = "p³aszcza z li¶ci";
			name3 = "p³aszczowi z li¶ci";
			name4 = "p³aszcz z li¶ci";
			name5 = "p³aszczem z li¶ci";
			name6 = "p³aszczu z li¶ci";
			obj->material = 39;
			break;
		case 37:
			name = "szaro-br±zowy p³aszcz";
			sh_desc = "szaro-br±zowy p³aszcz";
			name2 = "szaro-br±zowego p³aszcza";
			name3 = "szaro-br±zowemu p³aszczowi";
			name4 = "szaro-br±zowy p³aszcz";
			name5 = "szaro-br±zowym p³aszczem";
			name6 = "szaro-br±zowym p³aszczu";
			break;
		case 38:
			name = "niedbale wykonany p³aszcz";
			sh_desc = "niedbale wykonany p³aszcz";
			name2 = "niedbale wykonanego p³aszcza";
			name3 = "niedbale wykonanemu p³aszczowi";
			name4 = "niedbale wykonany p³aszcz";
			name5 = "niedbale wykonanym p³aszczem";
			name6 = "niedbale wykonanym p³aszczu";
			break;
		case 39:
			name = "staro¿ytna Drakowa szata";
			sh_desc = "staro¿ytna Drakowa szata";
			name2 = "staro¿ytnej Drakowej szaty";
			name3 = "staro¿ytnej Drakowej szacie";
			name4 = "staro¿ytn± Drakow± szatê";
			name5 = "staro¿ytn± Drakow± szat±";
			name6 = "staro¿ytnej Drakowej szacie";
			second = TRUE;
			third = TRUE;
			obj->material = 45;
			obj->cost += 150;
			break;
	}

	free_string( obj->short_descr );
	obj->short_descr = str_dup( sh_desc );

	free_string( obj->name );
	obj->name = str_dup( name );

	free_string( obj->name2 );
	obj->name2 = str_dup( name2 );

	free_string( obj->name3 );
	obj->name3 = str_dup( name3 );

	free_string( obj->name4 );
	obj->name4 = str_dup( name4 );

	free_string( obj->name5 );
	obj->name5 = str_dup( name5 );

	free_string( obj->name6 );
	obj->name6 = str_dup( name6 );

	free_string( obj->description );
	sprintf( text, "%s po³yskuje magicznym blaskiem.", capitalize( sh_desc ) );
	obj->description = str_dup( text );

	//druga kategoria
	if ( !second ) return;

	switch ( number_range( 1, 5 ) )
	{
		case 1: //mov
			loc = APPLY_MOVE;
			mod = number_range( 1, 10 );
			kasa += COST_APPLY_SKILL * mod;
			break;
		case 2: //hp
			loc = APPLY_HIT;
			mod = number_range( 1, 10 );
			kasa += COST_APPLY_SKILL * mod;
			break;
		case 3:
			loc = APPLY_AC;
			mod = number_range(-25,10);
			kasa -= mod * COST_APPLY_AC;
			break;
		case 4:
			loc = APPLY_MEMMING;
			mod = number_range( 1, 10 );
			switch ( mod )
			{
				case 1: level = number_range( 0, 5 );break;
				case 2: level = number_range( 0, 4 );break;
				case 3: level = number_range( 0, 3 );break;
				case 4: level = number_range( 0, 2 );break;
				default: level = 1;
			}
			level = UMAX(1, level);
			kasa += number_range( 140, 160 ) * ( mod * level );
			break;
		case 5:
			loc = APPLY_SKILL;
			if (number_range(1,3) != 1 ) mod = gsn_hide;
			else mod = gsn_sneak;
			level = number_range(1, 10);
			kasa += level * COST_APPLY_SKILL;
			break;

	}

	//trzecia kategoria
	if ( !third )
	{
		add_new_affect( obj, type, loc, mod, level, bitv, generate_aff_duration( points, &kasa ), TRUE );
		obj->cost += kasa;
		return;
	}
	add_new_affect( obj, type, loc, mod, level, bitv, generate_aff_duration( points, &kasa ), FALSE );

	kasa = 1;

	switch ( number_range( 1, 2 ) )
	{
/* WY£¡CZAM: Gurthg - 2005-02-20
		case 4: //int
			loc = APPLY_INT;
			if ( number_percent() > 10 ) mod = 1;
			else mod = 2;
			kasa += COST_APPLY_STAT * mod;
			break;
		case 3: //wis
			loc = APPLY_WIS;
			if ( number_percent() > 10 ) mod = 1;
			else mod = 2;
			kasa += COST_APPLY_STAT * mod;
			break;
*/
		case 2: //resist
			loc = APPLY_RESIST;
			mod = number_range( 0,16);
			if ( mod == RESIST_MAGIC_FIRE || mod == RESIST_MAGIC_WEAPON )
				--mod;
			level = number_range(1,50);
			kasa += COST_APPLY_RESIST * level;
			if ( mod == RESIST_PIERCE || mod == RESIST_BASH || mod == RESIST_SLASH || mod == RESIST_MAGIC )
				level /= 2;
			if ( mod == RESIST_ALL )
				level /= 4;
			break;
		case 1: //save
			loc = number_range(20,24);
			mod = number_range(-4,2);
			if ( loc == APPLY_SAVING_ROD || loc == APPLY_SAVING_PETRI )
				kasa -= COST_APPLY_SAVING/2 * mod;
			else
				kasa -= COST_APPLY_SAVING * mod;
			break;
	}

	//tu dac vectorka
	bitv = get_bitvector();
	kasa += get_cost_of_bitvector(bitv);

	add_new_affect( obj, type, loc, mod, level, bitv, generate_aff_duration( points, &kasa ) , TRUE );

	obj->cost += kasa;

	if ( obj->cost < 1 )
		obj->cost = 1;

	return;
}

/*
void add_new_affect( OBJ_DATA *obj, int type, int loc, int mod, int level, BITVECT_DATA * bitv, int dur, bool last )
{
	AFFECT_DATA * pAf;

	if ( dur == 0 )
		return;

	pAf	= new_affect();
	pAf->where	= TO_AFFECTS;
	pAf->type	= type;
	pAf->location = loc;
	pAf->modifier = mod;
	pAf->level	= level;
	pAf->bitvector = bitv;

	pAf->duration = dur;
	pAf->rt_duration = 0;
	obj->enchanted = TRUE;
	pAf->next = obj->affected;
	obj->affected = pAf;

	if ( !last )
		pAf = NULL;

	return;
}
*/

void generate_earring( OBJ_DATA *obj, int* points )
{
	char txt[ MAX_INPUT_LENGTH ];
	char text[ MAX_INPUT_LENGTH ], *name = "(null)", *sh_desc = "(null)";
	char *name2 = "(null)", *name3 = "(null)", *name4 = "(null)", *name5 = "(null)", *name6 = "(null)";
	char *name_des = "n", *sh_des = "n", *name2_des = "n", *name3_des = "n", *name4_des = "n", *name5_des = "n", *name6_des = "n";
	bool losowy = FALSE, szyk_przestawny = FALSE;
	int elem, wer, ilosc_affectow, kasa = 1;
	int type = 0, loc = 0, mod = 0, level = 0, c = 0, val;
	BITVECT_DATA * bitv = &AFF_NONE;

	/* sprawdzenie czy *points ma sensown± warto¶æ */
	if ( (*points < 1) || (*points > 7) )
	  {
	    bugf("generate_earring: Z³a warto¶æ argumentu points: %d", *points);
	  }

	switch ( *points )
	{
		case 1: wer = 1000;break;
		case 2: wer = 500;break;
		case 3: wer = 300;break;
		case 4: wer = 200;break;
		case 5: wer = 150;break;
		case 6: wer = 100;break;
		case 7: wer = 0;break;
	}

	generate_extra( obj );

	obj->value[ 0 ] = 0;
	obj->value[ 1 ] = 0;
	obj->value[ 2 ] = 0;
	obj->value[ 3 ] = 0;
	obj->cost = 45;
	obj->weight = 5;
    // domy¶lna liczba pojedyñcza
    obj->liczba_mnoga = FALSE;
    // domy¶lny rodzaj mêski
    obj->gender = GENDER_MESKI;

	/*
								   | str | con | dex | int | vis | cha | luc | extra
	 1: kolczyk wszystkich bogów   |     |     |     | -1  | +2  |     |     | +10 gsn_meditation, +1 mem na 5 lub 6 krag
	 2: kolczyk zapomnianej bogini |     |     |     | +1  | -2  |     | +1  | infrared, + 1 mem na 5, 6, 7 lub 8 krag
	 3: klips wolno¶ci             |     |     |  +1 |     |     |     |     | free_action
	 4: klips swobody              |     |     |  -2 |     |     |     |     | float
	 5: ¶lepia czerwonego smoka    |     |     |     |     |     | -2  |     | eyes_of_the_torturer
	 6: ³zy bia³ej smoczycy        |     |     |     |     |     | +2  |     | detect_aggressive, save vs spell -4, save vs breath -4
	*/
	//pierwszy -s-w-i-t-c-h-(-)- -> nazwy i normy potrzebne do stworzenia
	switch ( c = number_range( 1, 800 ) )
	{ //no powiedzmy mozna zmienic
			//wpierw rzadkie, ze stalymi affektami
		case 1:
			name = "kolczyk wszystkich bogów";
			sh_desc = "kolczyk wszystkich bogów";
			name2 = "kolczyka wszystkich bogów";
			name3 = "kolczykowi wszystkich bogów";
			name4 = "kolczyk wszystkich bogów";
			name5 = "kolczykiem wszystkich bogów";
			name6 = "kolczyku wszystkich bogów";
			break;
		case 2:
			name = "kolczyk zapomnianej bogini";
			sh_desc = "kolczyk zapomnianej bogini";
			name2 = "kolczyka zapomnianej bogini";
			name3 = "kolczykowi zapomnianej bogini";
			name4 = "kolczyk zapomnianej bogini";
			name5 = "kolczykiem zapomnianej bogini";
			name6 = "kolczyku zapomnianej bogini";
			break;
		case 3:
			name = "klips wolno¶ci";
			sh_desc = "klips wolno¶ci";
			name2 = "klipsa wolno¶æi";
			name3 = "klipsowi wolno¶ci";
			name4 = "klips wolno¶ci";
			name5 = "klipsem wolno¶ci";
			name6 = "klipsie wolno¶ci";
			break;
		case 4:
			name = "klips swobody";
			sh_desc = "klips swobody";
			name2 = "klipsa swobody";
			name3 = "klipsowi swobody";
			name4 = "klips swobody";
			name5 = "klipsem swobody";
			name6 = "klipsie swobody";
			break;
		case 5:
			name = "¶lepia czerwonego smoka";
			sh_desc = "¶lepia czerwonego smoka";
			name2 = "¶lepi czerwonego smoka";
			name3 = "¶lepiom czerwonego smoka";
			name4 = "¶lepia czerwonego smoka";
			name5 = "¶lepiami czerwonego smoka";
            name6 = "¶lepiach czerwonego smoka";
            obj->liczba_mnoga = TRUE;
            obj->gender = GENDER_ZENSKOOSOBOWY;
			break;
		case 6:
			name = "³zy bia³ej smoczycy";
			sh_desc = "³zy bia³ej smoczycy";
			name2 = "³ez bia³ej smoczycy";
			name3 = "³zom bia³ej smoczycy";
			name4 = "³zy bia³ej smoczycy";
			name5 = "³zami bia³ej smoczycy";
            name6 = "³zach bia³ej smoczycy";
            obj->liczba_mnoga = TRUE;
            obj->gender = GENDER_ZENSKOOSOBOWY;
			break;

		default:    //czyli kompletnie losowy kolczyk
			losowy = TRUE;
			switch ( number_range( 1, 4 ) )
			{
				case 1:
					name = "klips";
					sh_desc = "klips";
					name2 = "klipsa";
					name3 = "klipsowi";
					name4 = "klips";
					name5 = "klipsem";
					name6 = "klipsie";
					break;

				default:
					name = "kolczyk";
					sh_desc = "kolczyk";
					name2 = "kolczyka";
					name3 = "kolczykowi";
					name4 = "kolczyk";
					name5 = "kolczykiem";
					name6 = "kolczyku";
					break;
			}

			if ( number_range( 1, 3 ) != 1 )
			{
				szyk_przestawny = FALSE;

				switch ( number_range( 1, 38 ) )
				{
					case 1:
						obj->material = 5;
						obj->cost += 40;
						sh_des = "miedziany";
						name_des = "miedziany";
						name2_des = "miedzianego";
						name3_des = "miedzianemu";
						name4_des = "miedziany";
						name5_des = "miedzianym";
						name6_des = "miedzianym";
						break;
					case 2:
						obj->material = 21;
						obj->cost += 10;
						sh_des = "ko¶ciany";
						name_des = "ko¶ciany";
						name2_des = "ko¶cianego";
						name3_des = "ko¶cianemu";
						name4_des = "ko¶ciany";
						name5_des = "ko¶cianym";
						name6_des = "ko¶cianym";
						break;
					case 3:
						obj->material = 14;
						obj->cost += 30;
						sh_des = "br±zowy";
						name_des = "br±zowy";
						name2_des = "br±zowego";
						name3_des = "br±zowemu";
						name4_des = "br±zowy";
						name5_des = "br±zowym";
						name6_des = "br±zowym";
						break;
					case 4:    //kamien
						obj->material = 16;
						obj->cost += 20;
						sh_des = "kamienny";
						name_des = "kamienny";
						name2_des = "kamiennego";
						name3_des = "kamiennemu";
						name4_des = "kamienny";
						name5_des = "kamiennym";
						name6_des = "kamiennym";
						break;
					case 5:
						obj->material = 13;
						obj->cost += 80;
						sh_des = "¿elazny";
						name_des = "¿elazny";
						name2_des = "¿elaznego";
						name3_des = "¿elaznemu";
						name4_des = "¿elazny";
						name5_des = "¿elaznym";
						name6_des = "¿elaznym";
						break;
					case 6:
						obj->material = 2;
						obj->cost += 100;
						sh_des = "stalowy";
						name_des = "stalowy";
						name2_des = "stalowego";
						name3_des = "stalowemu";
						name4_des = "stalowy";
						name5_des = "stalowym";
						name6_des = "stalowym";
						break;
					case 7:
						obj->material = 9;
						obj->cost += 100;
						sh_des = "finezyjny";
						name_des = "finezyjny";
						name2_des = "finezyjnego";
						name3_des = "finezyjnemu";
						name4_des = "finezyjny";
						name5_des = "finezyjnym";
						name6_des = "finezyjnym";
						break;
					case 8:
						obj->material = 9;
						obj->cost += 100;
						sh_des = "figlarny";
						name_des = "figlarny";
						name2_des = "figlarnego";
						name3_des = "figlarnemu";
						name4_des = "figlarny";
						name5_des = "figlarnym";
						name6_des = "figlarnym";
						break;
					case 9:
						obj->cost += 120;
						obj->material = 4;
						sh_des = "doskona³y";
						name_des = "doskona³y";
						name2_des = "doskona³ego";
						name3_des = "doskona³emu";
						name4_des = "doskona³y";
						name5_des = "doskona³ym";
						name6_des = "doskona³ym";
						break;
					case 10:
						obj->cost += 750;
						obj->material = 36;
						sh_des = "diamentowy";
						name_des = "diamentowy";
						name2_des = "diamentowego";
						name3_des = "diamentowemu";
						name4_des = "diamentowy";
						name5_des = "diamentowym";
						name6_des = "diamentowym";
						break;
					case 11:
						obj->cost += 200;
						obj->material = 43;
						sh_des = "mosiê¿ny";
						name_des = "mosiê¿ny";
						name2_des = "mosiê¿nego";
						name3_des = "mosiê¿nemu";
						name4_des = "mosiê¿ny";
						name5_des = "mosiê¿nym";
						name6_des = "mosiê¿nym";
						break;
					case 12:
						obj->material = 3;
						obj->cost += 130;
						sh_des = "elfi";
						name_des = "elfi";
						name2_des = "elfiego";
						name3_des = "elfiemu";
						name4_des = "elfi";
						name5_des = "elfim";
						name6_des = "elfim";
						break;
					case 13:
						obj->material = 10;
						obj->cost += 140;
						sh_des = "srebrny";
						name_des = "srebrny";
						name2_des = "srebrnego";
						name3_des = "srebrnemu";
						name4_des = "srebrny";
						name5_des = "srebrnym";
						name6_des = "srebrnym";
						break;
					case 14:
						obj->material = 15;
						obj->cost += 150;
						sh_des = "szklany";
						name_des = "szklany";
						name2_des = "szklanego";
						name3_des = "szklanemu";
						name4_des = "szklany";
						name5_des = "szklanym";
						name6_des = "szklanym";
						break;
					case 15:
						obj->cost += 250;
						obj->material = 9;
						sh_des = "z³oty";
						name_des = "z³oty";
						name2_des = "z³otego";
						name3_des = "z³otemu";
						name4_des = "z³oty";
						name5_des = "z³otym";
						name6_des = "z³otym";
						break;
					case 16:
						obj->cost += 250;
						obj->material = 4;
						sh_des = "poz³acany";
						name_des = "poz³acany";
						name2_des = "poz³acanego";
						name3_des = "poz³acanemu";
						name4_des = "poz³acany";
						name5_des = "poz³acanym";
						name6_des = "poz³acanym";
						break;
					case 17:
						obj->cost += 250;
						obj->material = 4;
						sh_des = "posrebrzany";
						name_des = "posrebrzany";
						name2_des = "posrebrzanego";
						name3_des = "posrebrzanemu";
						name4_des = "posrebrzany";
						name5_des = "posrebrzanym";
						name6_des = "posrebrzanym";
						break;
					case 18:
						obj->cost += 300;
						obj->material = 7;
						sh_des = "adamantytowy";
						name_des = "adamantytowy";
						name2_des = "adamantytowego";
						name3_des = "adamantytowemu";
						name4_des = "adamantytowy";
						name5_des = "adamantytowym";
						name6_des = "adamantytowym";
						break;
					case 19:
						obj->material = 10;
						obj->cost += 140;
						sh_des = "gnomi";
						name_des = "gnomi";
						name2_des = "gnomiego";
						name3_des = "gnomiemu";
						name4_des = "gnomi";
						name5_des = "gnomim";
						name6_des = "gnomim";
						break;
					case 20:
						obj->material = 8;
						obj->cost += 400;
						sh_des = "mithrilowy";
						name_des = "mithrilowy";
						name2_des = "mithrilowego";
						name3_des = "mithrilowemu";
						name4_des = "mithrilowy";
						name5_des = "mithrilowym";
						name6_des = "mithrilowym";
						break;
					case 21:
						obj->material = 11;
						obj->cost += 400;
						sh_des = "kryszta³owy";
						name_des = "kryszta³owy";
						name2_des = "kryszta³owego";
						name3_des = "kryszta³owemu";
						name4_des = "kryszta³owy";
						name5_des = "kryszta³owym";
						name6_des = "kryszta³owym";
						break;
					case 22:
						obj->cost += 400;
						obj->material = 34;
						sh_des = "platynowy";
						name_des = "platynowy";
						name2_des = "platynowego";
						name3_des = "platynowemu";
						name4_des = "platynowy";
						name5_des = "platynowym";
						name6_des = "platynowym";
						break;
					case 23:
						obj->cost += 400;
						obj->material = 34;
						sh_des = "ozdobny";
						name_des = "ozdobny";
						name2_des = "ozdobnego";
						name3_des = "ozdobnemu";
						name4_des = "ozdobny";
						name5_des = "ozdobnym";
						name6_des = "ozdobnym";
						break;
					case 24:
						obj->cost += 100;
						obj->material = 10;
						sh_des = "elegancki kr±g³y";
						name_des = "elegancki kr±g³y";
						name2_des = "eleganckiego kr±g³ego";
						name3_des = "eleganckiemu kr±g³emu";
						name4_des = "elegancki kr±g³y";
						name5_des = "eleganckim kr±g³ym";
						name6_des = "eleganckim kr±g³ym";
						break;
					case 25:
						obj->material = 10;
						sh_des = "ma³y";
						name_des = "ma³y";
						name2_des = "ma³ego";
						name3_des = "ma³emu";
						name4_des = "ma³y";
						name5_des = "ma³ym";
						name6_des = "ma³ym";
						break;
					case 26:
						obj->material = 10;
						sh_des = "okr±g³y";
						name_des = "okr±g³y";
						name2_des = "okr±g³ego";
						name3_des = "okr±g³emu";
						name4_des = "okr±g³y";
						name5_des = "okr±g³ym";
						name6_des = "okr±g³ym";
						break;
					case 27:
						obj->material = 10;
						sh_des = "pod³u¿ny";
						name_des = "pod³u¿ny";
						name2_des = "pod³u¿nego";
						name3_des = "pod³u¿nemu";
						name4_des = "pod³u¿ny";
						name5_des = "pod³u¿nym";
						name6_des = "pod³u¿nym";
						break;
					case 28:
						obj->material = 10;
						sh_des = "zwisaj±cy";
						name_des = "zwisaj±cy";
						name2_des = "zwisaj±cego";
						name3_des = "zwisaj±cemu";
						name4_des = "zwisaj±cy";
						name5_des = "zwisaj±cym";
						name6_des = "zwisaj±cym";
						break;
					case 29:
						obj->material = 10;
						sh_des = "owalny";
						name_des = "owalny";
						name2_des = "owalnego";
						name3_des = "owalnemu";
						name4_des = "owalny";
						name5_des = "owalnym";
						name6_des = "owalnym";
						break;
					case 30:
						obj->material = 3;
						sh_des = "matowy";
						name_des = "matowy";
						name2_des = "matowego";
						name3_des = "matowemu";
						name4_des = "matowy";
						name5_des = "matowym";
						name6_des = "matowym";
						break;
					case 31:
						obj->material = 10;
						sh_des = "b³yszcz±cy";
						name_des = "b³yszcz±cy";
						name2_des = "b³yszcz±cego";
						name3_des = "b³yszcz±cemu";
						name4_des = "b³yszcz±cy";
						name5_des = "b³yszcz±cym";
						name6_des = "b³yszcz±cym";
						break;
					case 32:
						obj->material = 10;
						sh_des = "tajemniczy b³yszcz±cy";
						name_des = "tajemniczy b³yszcz±cy";
						name2_des = "tajemniczego b³yszcz±cego";
						name3_des = "tajemniczemu b³yszcz±cemu";
						name4_des = "tajemniczy b³yszcz±cy";
						name5_des = "tajemniczym b³yszcz±cym";
						name6_des = "tajemniczym b³yszcz±cym";
						break;
					case 33:
						obj->material = 10;
						sh_des = "zakrwawiony ma³y";
						name_des = "zakrwawiony ma³y";
						name2_des = "zakrwawionego ma³ego";
						name3_des = "zakrwawionemu ma³emu";
						name4_des = "zakrwawiony ma³y";
						name5_des = "zakrwawionym ma³ym";
						name6_des = "zakrwawionym ma³ym";
					case 34:
						obj->material = 10;
						sh_des = "stalowy z³owrogi";
						name_des = "stalowy z³owrogi";
						name2_des = "stalowego z³owrogiego";
						name3_des = "stalowemu z³owrogiemu";
						name4_des = "stalowy z³owrogi";
						name5_des = "stalowym z³owrogim";
						name6_des = "stalowym z³owrogim";
					case 35:
						obj->material = 10;
						sh_des = "z³owieszczy czarny";
						name_des = "z³owieszczy czarny";
						name2_des = "z³owieszczego czarnego";
						name3_des = "z³owieszczemu czarnemu";
						name4_des = "z³owieszczy czarny";
						name5_des = "z³owieszczym czarnym";
						name6_des = "z³owieszczym czarnym";
					case 36:
						obj->material = 10;
						sh_des = "fiku¶ny";
						name_des = "fiku¶ny";
						name2_des = "fiku¶nego";
						name3_des = "fiku¶nemu";
						name4_des = "fiku¶ny";
						name5_des = "fiku¶nym";
						name6_des = "fiku¶nym";
						break;
					case 37:
						obj->material = 10;
						sh_des = "delikatny";
						name_des = "delikatny";
						name2_des = "delikatnego";
						name3_des = "delikatnemu";
						name4_des = "delikatny";
						name5_des = "delikatnym";
						name6_des = "delikatnym";
						break;
					case 38:
						obj->material = 10;
						sh_des = "grawerowany";
						name_des = "grawerowany";
						name2_des = "grawerowanego";
						name3_des = "grawerowanemu";
						name4_des = "grawerowany";
						name5_des = "grawerowanym";
						name6_des = "grawerowanym";
						break;
				}
			}
			else
			{
				szyk_przestawny = TRUE;
				obj->material = 10;
				switch ( number_range( 1, 13 ) )
				{
					case 1:
						sh_des = "z krwawnikiem";
						name_des = "z krwawnikiem";
						name2_des = "z krwawnikiem";
						name3_des = "z krwawnikiem";
						name4_des = "z krwawnikiem";
						name5_des = "z krwawnikiem";
						name6_des = "z krwawnikiem";
						break;
					case 2:
						sh_des = "z onyksem";
						name_des = "z onyksem";
						name2_des = "z onyksem";
						name3_des = "z onyksem";
						name4_des = "z onyksem";
						name5_des = "z onyksem";
						name6_des = "z onyksem";
						break;
					case 3:
						sh_des = "z sardonyksem";
						name_des = "z sardonyksem";
						name2_des = "z sardonyksem";
						name3_des = "z sardonyksem";
						name4_des = "z sardonyksem";
						name5_des = "z sardonyksem";
						name6_des = "z sardonyksem";
						break;
					case 4:
						sh_des = "z karnelianem";
						name_des = "z karnelianem";
						name2_des = "z karnelianem";
						name3_des = "z karnelianem";
						name4_des = "z karnelianem";
						name5_des = "z karnelianem";
						name6_des = "z karnelianem";
						break;
					case 5:
						sh_des = "z jaspisem";
						name_des = "z jaspisem";
						name2_des = "z jaspisem";
						name3_des = "z jaspisem";
						name4_des = "z jaspisem";
						name5_des = "z jaspisem";
						name6_des = "z jaspisem";
						break;
					case 6:
						sh_des = "z chalcedonem";
						name_des = "z chalcedonem";
						name2_des = "z chalcedonem";
						name3_des = "z chalcedonem";
						name4_des = "z chalcedonem";
						name5_des = "z chalcedonem";
						name6_des = "z chalcedonem";
						break;
					case 7:
						sh_des = "z chryzopazem";
						name_des = "z chryzopazem";
						name2_des = "z chryzopazem";
						name3_des = "z chryzopazem";
						name4_des = "z chryzopazem";
						name5_des = "z chryzopazem";
						name6_des = "z chryzopazem";
						break;
					case 8:
						sh_des = "z ametystem";
						name_des = "z ametystem";
						name2_des = "z ametystem";
						name3_des = "z ametystem";
						name4_des = "z ametystem";
						name5_des = "z ametystem";
						name6_des = "z ametystem";
						break;
					case 9:
						sh_des = "z koralem";
						name_des = "z koralem";
						name2_des = "z koralem";
						name3_des = "z koralem";
						name4_des = "z koralem";
						name5_des = "z koralem";
						name6_des = "z koralem";
						obj->cost += 150;
						break;
					case 10:
						sh_des = "z turmalinem";
						name_des = "z turmalinem";
						name2_des = "z turmalinem";
						name3_des = "z turmalinem";
						name4_des = "z turmalinem";
						name5_des = "z turmalinem";
						name6_des = "z turmalinem";
						break;
					case 11:
						sh_des = "z malachitem";
						name_des = "z malachitem";
						name2_des = "z malachitem";
						name3_des = "z malachitem";
						name4_des = "z malachitem";
						name5_des = "z malachitem";
						name6_des = "z malachitem";
						break;
					case 12:
						sh_des = "z awenturynem";
						name_des = "z awenturynem";
						name2_des = "z awenturynem";
						name3_des = "z awenturynem";
						name4_des = "z awenturynem";
						name5_des = "z awenturynem";
						name6_des = "z awenturynem";
						break;
					case 13:
						sh_des = "z koralikami";
						name_des = "z koralikami";
						name2_des = "z koralikami";
						name3_des = "z koralikami";
						name4_des = "z koralikami";
						name5_des = "z koralikami";
						name6_des = "z koralikami";
						break;
				}
			}
	}

	if ( losowy )
	{
		if ( !szyk_przestawny )    //tutaj np "szklany kolczyk, po else np "kolczyk z krwawnikiem"
		{
			free_string( obj->short_descr );
			sprintf( txt, "%s %s", sh_des, sh_desc );
			obj->short_descr = str_dup( txt );

			free_string( obj->name );
			sprintf( txt, "%s %s", name_des, name );
			obj->name = str_dup( txt );

			free_string( obj->name2 );
			sprintf( txt, "%s %s", name2_des, name2 );
			obj->name2 = str_dup( txt );

			free_string( obj->name3 );
			sprintf( txt, "%s %s", name3_des, name3 );
			obj->name3 = str_dup( txt );

			free_string( obj->name4 );
			sprintf( txt, "%s %s", name4_des, name4 );
			obj->name4 = str_dup( txt );

			free_string( obj->name5 );
			sprintf( txt, "%s %s", name5_des, name5 );
			obj->name5 = str_dup( txt );

			free_string( obj->name6 );
			sprintf( txt, "%s %s", name6_des, name6 );
			obj->name6 = str_dup( txt );
		}
		else
		{
			free_string( obj->short_descr );
			sprintf( txt, "%s %s", sh_desc, sh_des );
			obj->short_descr = str_dup( txt );

			free_string( obj->name );
			sprintf( txt, "%s %s", name, name_des );
			obj->name = str_dup( txt );

			free_string( obj->name2 );
			sprintf( txt, "%s %s", name2, name2_des );
			obj->name2 = str_dup( txt );

			free_string( obj->name3 );
			sprintf( txt, "%s %s", name3, name3_des );
			obj->name3 = str_dup( txt );

			free_string( obj->name4 );
			sprintf( txt, "%s %s", name4, name4_des );
			obj->name4 = str_dup( txt );

			free_string( obj->name5 );
			sprintf( txt, "%s %s", name5, name5_des );
			obj->name5 = str_dup( txt );

			free_string( obj->name6 );
			sprintf( txt, "%s %s", name6, name6_des );
			obj->name6 = str_dup( txt );
		}
	}
	else
	{
		free_string( obj->short_descr ); obj->short_descr = str_dup( sh_desc );
		free_string( obj->name ); obj->name = str_dup( name );
		free_string( obj->name2 ); obj->name2 = str_dup( name2 );
		free_string( obj->name3 ); obj->name3 = str_dup( name3 );
		free_string( obj->name4 ); obj->name4 = str_dup( name4 );
		free_string( obj->name5 ); obj->name5 = str_dup( name5 );
		free_string( obj->name6 ); obj->name6 = str_dup( name6 );
	}

	free_string( obj->description );
	sprintf( text, "%s po³yskuje magicznym blaskiem.", capitalize( obj->short_descr ) );
	obj->description = str_dup( text );

    if ( c < 7 )
    {
        switch( number_range( 1, 8 ) )
        {
            case 1:
                obj->material =  8; //mithril
                break;
            case 2:
                obj->material =  9; //zloto
                break;
            case 3:
                obj->material = 10; //srebro
                break;
            case 4:
                obj->material = 33; //illit
                break;
            case 5:
                obj->material = 34; //platyna
                break;
            default:
                obj->material =  7; //adamantyt
                break;
        }
    }

	switch ( c )
	{
		case 1:    //kolczyk wszystkich bogow
			add_new_affect( obj, 0, APPLY_INT, -number_range(3,9), 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_WIS, number_range(1,12), 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_MEMMING, number_range( 5, 6 ), 1, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_SKILL, gsn_meditation, number_range(1,20), &AFF_NONE, -1, TRUE );
			obj->cost += 15 * COST_APPLY_SKILL + COST_APPLY_STAT + COST_APPLY_MEMMING;
			break;
		case 2:    //kolczyk zapomnianej bogini
			add_new_affect( obj, 0, APPLY_MEMMING, number_range( 5, 8 ), 1, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_WIS, -number_range(3,9), 0, &AFF_INFRARED, -1, FALSE );
			add_new_affect( obj, 0, APPLY_INT, number_range(1,9), 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_LUC, number_range(1,9), 0, &AFF_NONE, -1, TRUE );
			obj->cost += COST_APPLY_STAT + COST_AFF_INFRARED + COST_APPLY_MEMMING;
			break;
		case 3:    //klips wolno¶ci
			add_new_affect( obj, 0, APPLY_DEX, number_range(1,12), 0, &AFF_FREE_ACTION, -1, TRUE );
			obj->cost += COST_APPLY_STAT + 1500;
			break;
		case 4:    //klips swobody
			add_new_affect( obj, 0, APPLY_DEX, -number_range(3,9), 0, &AFF_FLOAT, -1, TRUE );
			obj->cost += 1000;
			break;
		case 5:    //slepia czerwonego smoka
			add_new_affect( obj, 0, APPLY_CHA, -number_range(3,9), 0, &AFF_EYES_OF_THE_TORTURER, -1, TRUE );
			obj->cost += COST_AFF_EYES_OF_THE_TORTURER;
			break;
		case 6:    //³zy bialej smoczycy
			add_new_affect( obj, 0, APPLY_CHA, number_range(1,12), 0, &AFF_DETECT_AGGRESSIVE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_CON, -number_range(3,9), 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_SAVING_SPELL, -number_range(2,5), 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_SAVING_BREATH, -number_range(2,5), 0, &AFF_NONE, -1, TRUE );
			obj->cost += 4 * COST_APPLY_SAVING + COST_APPLY_STAT;
			break;

		default:

			if ( number_range( 1, 200 + wer ) < 25 ) ilosc_affectow = 3;
			else if ( number_range( 1, 150 + wer ) < 70 ) ilosc_affectow = 2;
			else ilosc_affectow = 1;

			for (; ilosc_affectow > 0; --ilosc_affectow )
			{
				bitv = get_bitvector();
				kasa += get_cost_of_bitvector(bitv);

				switch ( number_range( 1, 22 ) )
				{
					case 1:    //str
						loc = APPLY_STR;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = number_range( 1, 3 );
							kasa += mod * COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 2:    //int
						loc = APPLY_INT;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = number_range( 1, 3 );
							kasa += mod * COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 3:    //wis
						loc = APPLY_WIS;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = number_range( 1, 3 );
							kasa += mod * COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 4:    //dex
						loc = APPLY_DEX;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = number_range( 1, 3 );
							kasa += mod * COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 5:    //con
						loc = APPLY_CON;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = number_range( 1, 3 );
							kasa += mod * COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 6:    //cha
						loc = APPLY_CHA;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = number_range( 1, 3 );
							kasa += mod * COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 7:    //luc
						loc = APPLY_LUC;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = number_range( 1, 3 );
							kasa += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 8:    //mov
						loc = APPLY_MOVE;
						mod = number_range( 1, 10 );
						kasa += COST_APPLY_SKILL * mod;
						break;
					case 9:    //hp
						loc = APPLY_HIT;
						mod = number_range( 1, 10 );
						kasa += COST_APPLY_SKILL * mod;
						break;
					case 10:    //spell_mem
						loc = APPLY_MEMMING;
						mod = number_range( 1, 9 );
						switch ( mod )
						{ //czeba poprawic to na pierdzionkach tez
							case 1: level = number_range( 0, 6 );break;
							case 2: level = number_range( 0, 6 );break;
							case 3: level = number_range( 0, 5 );break;
							case 4: level = number_range( 0, 3 );break;
							case 5: level = number_range( 0, 2 );break;
							case 6: level = number_range( 0, 2 );break;
							default: level = 1;break;
						}
						level = UMAX(1, level);
						kasa += number_range( 150, 180 ) * ( 2 * mod * level );
						break;
					case 11:    //ac
						loc = APPLY_AC;
						val = number_range( -10, 20 );
						mod = -val;
						kasa += val * COST_APPLY_AC;
						break;
					case 12:    //dam
						loc = APPLY_DAMROLL;
						switch( number_range(1,11))
						{
							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
								mod = -1;
								break;
							case 6:
							case 7:
							case 8:
							case 9:
							case 10:
								mod = 1;
								break;
							case 11:
								mod = 2;
								break;
						}
						kasa += COST_APPLY_DAMROLL * mod;
						break;
					case 13:    //hit
						loc = APPLY_HITROLL;
						switch( number_range(1,11))
						{
							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
								mod = -1;
								break;
							case 6:
							case 7:
							case 8:
							case 9:
							case 10:
								mod = 1;
								break;
							case 11:
								mod = 2;
								break;
						}
						kasa += COST_APPLY_HITROLL * mod;
						break;
					case 14:
					case 15:
						loc = APPLY_SKILL;
						level = number_range( 1, 15 );
						switch ( number_range( 1, 40 ) )
						{
							case 1: mod = gsn_axe;break;
							case 2: mod = gsn_flail;break;
							case 3: mod = gsn_dagger;break;
							case 4: mod = gsn_mace;break;
							case 5: mod = gsn_polearm;break;
							case 6: mod = gsn_staff;break;
							case 7: mod = gsn_shield_block;break;
							case 8: mod = gsn_spear;break;
							case 9: mod = gsn_sword;break;
							case 10: mod = gsn_whip;break;
							case 11: mod = gsn_backstab;break;
							case 12: mod = gsn_bash;break;
							case 13: mod = gsn_berserk;break;
							case 14: mod = gsn_disarm;break;
							case 15: mod = gsn_dodge;break;
							case 16: mod = gsn_envenom;break;
							case 17: mod = gsn_kick;break;
							case 18: mod = gsn_parry;break;
							case 19: mod = gsn_rescue;break;
							case 20: mod = gsn_trip;break;
							case 21: mod = gsn_flail_mastery;break;
							case 22: mod = gsn_wardance;break;
							case 23: mod = gsn_mace_mastery;break;
							case 24: mod = gsn_riding;break;
							case 25: mod = gsn_hide;break;
							case 26: mod = gsn_lore;break;
							case 27: mod = gsn_meditation;break;
							case 28: mod = gsn_peek;break;
							case 29: mod = gsn_pick_lock;break;
							case 30: mod = gsn_sneak;break;
							case 31: mod = gsn_steal;break;
							case 32: mod = gsn_charge;break;
							case 33: mod = gsn_recuperate;break;
							case 34: mod = gsn_lay;break;
							case 35: mod = gsn_damage_reduction;break;
							case 36: mod = gsn_critical_strike;break;
							case 37: mod = gsn_two_weapon_fighting;break;
							case 38: mod = gsn_dagger_mastery;break;
							case 39: mod = gsn_twohander_fighting;break;
							case 40: mod = gsn_turn;break;
							default: break;
						}
						break;
					case 16:
						elem = number_range( 1, 45 );
						kasa += elem * COST_APPLY_LANG;
						loc = APPLY_LANG;
						mod = number_range( 0, 19 );
						level = elem;
						break;
					case 17:
						elem = number_range( -5, 3 );
						kasa += UMAX( 0, -elem * COST_APPLY_SAVING );
						mod = elem;
						switch ( number_range( 1, 5 ) )
						{
							case 1:
								loc = APPLY_SAVING_SPELL;
								break;
							case 2:
								loc = APPLY_SAVING_ROD;
								break;
							case 3:
								loc = APPLY_SAVING_PETRI;
								break;
							case 4:
								loc = APPLY_SAVING_BREATH;
								break;
							case 5:
								loc = APPLY_SAVING_DEATH;
								break;
						}
						break;
					default:
						elem = number_range( 1, 45 );
						kasa += elem * COST_APPLY_RESIST;
						loc = APPLY_RESIST;
						level = elem;
						switch ( number_range( 1, 15 ) )
						{
							case 1:
								mod = RESIST_FIRE;
								break;
							case 2:
								mod = RESIST_COLD;
								break;
							case 3:
								mod = RESIST_ACID;
								break;
							case 4:
								mod = RESIST_ELECTRICITY;
								break;
							case 5:
								mod = RESIST_NEGATIVE;
								break;
							case 6:
								mod = RESIST_SUMMON;
								break;
							case 7:
								mod = RESIST_MENTAL;
								break;
							case 8:
								mod = RESIST_POISON;
								break;
							case 9:
								mod = RESIST_FEAR;
								break;
							case 10:
								mod = RESIST_CHARM;
								break;
							case 11:
								mod = RESIST_MAGIC;
								level = elem/2;
								break;
							case 12:
								mod = RESIST_BASH;
								level = elem/2;
								break;
							case 13:
								mod = RESIST_SLASH;
								level = elem/2;
								break;
							case 14:
								mod = RESIST_PIERCE;
								level = elem/2;
								break;
							case 15:
								mod = RESIST_HOLY;
								break;
							default:
								break;
						}
						break;
				}

				add_new_affect( obj, type, loc, mod, level, bitv, generate_aff_duration( points, &kasa ), TRUE );
				obj->cost += kasa;
				kasa = 1;
			}
			break;
	}
	obj->cost = UMAX( 1, obj->cost );
	return;
}


void generate_armor_legs( OBJ_DATA *obj, int* points )
{
	int wer;
	char text[ MAX_INPUT_LENGTH ], *name = "(null)", *sh_desc = "(null)";
	char *name2 = "null", *name3 = "null", *name4 = "null", *name5 = "null", *name6 = "null";
	bool go = TRUE, enchanted = FALSE, second = FALSE, third = FALSE, single = TRUE;
	char *nam = "null", *sh_de = "null", *nam2 = "null", *nam3 = "null", *nam4 = "null", *nam5 = "null", *nam6 = "null";
	char bufik[ MAX_STRING_LENGTH ];
	;

	switch ( *points )
	{
		case 0: wer = 3;break;
		case 1: wer = 4;break;
		case 2: wer = 6;break;
		case 3: wer = 8;break;
		case 4: wer = 10;break;
		case 5: wer = 12;break;
		case 6: wer = 15;break;
		case 7: wer = 16;break;
		default: wer = 10;break;
	}

	generate_extra( obj );
	obj->liczba_mnoga = TRUE;

	if ( *points > 3 )
		second = TRUE;

	if ( *points > 6 )
		third = TRUE;

	switch ( number_range( 1, 3 ) )
	{
		case 1:
			nam = "nabioderniki";
			sh_de = "nabioderniki";
			nam2 = "nabioderników";
			nam3 = "nabiodernikom";
			nam4 = "nabioderniki";
			nam5 = "nabiodernikami";
			nam6 = "nabiodernikach";
			break;
		case 2:
			nam = "nagolenniki";
			sh_de = "nagolenniki";
			nam2 = "nagolenników";
			nam3 = "nagolennikom";
			nam4 = "nagolenniki";
			nam5 = "nagolennikami";
			nam6 = "nagolennikach";
			break;
		case 3:
			nam = "nakolanniki";
			sh_de = "nakolanniki";
			nam2 = "nakolanników";
			nam3 = "nakolannikom";
			nam4 = "nakolanniki";
			nam5 = "nakolannikami";
			nam6 = "nakolannikach";
			break;
	}

	/*no to tutaj zrobie po jednym przedstawicielu kazdego typu pancerza,
	by roznorodnosc byla :]*/
	switch ( number_range( 0, wer ) )
	{
		case 0:
			name = "pikowane spodnie";
			sh_desc = "pikowane spodnie";
			name2 = "pikowanych spodni";
			name3 = "pikowanym spodniom";
			name4 = "pikowane spodnie";
			name5 = "pikowanymi spodniami";
			name6 = "pikowanych spodniach";

			obj->value[ 4 ] = 0;
			set_min_ac( obj );
			obj->cost = 15;
			obj->weight = 10;
			obj->material = 6;
			break;
		case 1:
			name = "skórzane spodnie";
			sh_desc = "skórzane spodnie";
			name2 = "skórzanych spodni";
			name3 = "skórzanym spodniom";
			name4 = "skórzane spodnie";
			name5 = "skórzanymi spodniami";
			name6 = "skórzanych spodniach";

			obj->value[ 4 ] = 1;
			set_min_ac( obj );
			obj->cost = 20;
			obj->weight = 15;
			obj->material = 6;
			break;
		case 2:
			name = "æwiekowane skórzane spodnie";
			sh_desc = "æwiekowane skórzane spodnie";
			name2 = "æwiekowanych skórzanych spodni";
			name3 = "æwiekowanym skórzanym spodniom";
			name4 = "æwiekowane skórzane spodnie";
			name5 = "æwiekowanymi skórzanymi spodniami";
			name6 = "æwiekowanych skórzanych spodniach";

			obj->value[ 4 ] = 2;
			set_min_ac( obj );
			obj->cost = 40;
			obj->weight = 25;
			obj->material = 6;
			break;
		case 3:
			name = "naszywane spodnie";
			sh_desc = "naszywane spodnie";
			name2 = "naszywanych spodni";
			name3 = "naszywanym spodniom";
			name4 = "naszywane spodnie";
			name5 = "naszywanymi spodniami";
			name6 = "naszywanych spodniach";

			obj->value[ 4 ] = 3;
			set_min_ac( obj );
			obj->material = 6;
			obj->cost = 50;
			obj->weight = 30;
			break;
		case 4:
			name = "³uskowe %s";
			sh_desc = "³uskowe %s";
			name2 = "³uskowych %s";
			name3 = "³uskowym %s";
			name4 = "³uskowe %s";
			name5 = "³uskowymi %s";
			name6 = "³uskowych %s";
			single = FALSE;
			obj->value[ 4 ] = 4;
			set_min_ac( obj );
			obj->cost = 60;
			obj->weight = 50;
			obj->material = 2;
			break;
		case 5:
			name = "przeszywane spodnie";
			sh_desc = "przeszywane spodnie";
			name2 = "przeszywanych spodni";
			name3 = "przeszywanym spodniom";
			name4 = "przeszywane spodnie";
			name5 = "przeszywanymi spodniami";
			name6 = "przeszywanych spodniach";

			obj->value[ 4 ] = 5;
			set_min_ac( obj );
			obj->cost = 65;
			obj->weight = 30;
			obj->material = 6;
			break;
		case 6:
			name = "spodnie ze skór zwierzêcych";
			sh_desc = "spodnie ze skór zwierzêcych";
			name2 = "spodni ze skór zwierzêcych";
			name3 = "spodniom ze skór zwierzêcych";
			name4 = "spodnie ze skór zwierzêcych";
			name5 = "spodniami ze skór zwierzêcych";
			name6 = "spodniach ze skór zwierzêcych";

			obj->value[ 4 ] = 6;
			set_min_ac( obj );
			obj->cost = 75;
			obj->weight = 30;
			obj->material = 44;
			break;
		case 7:
			name = "lekkie kolcze %s";
			sh_desc = "lekkie kolcze %s";
			name2 = "lekkich kolczych %s";
			name3 = "lekkim kolczym %s";
			name4 = "lekkie kolcze %s";
			name5 = "lekkimi kolczymi %s";
			name6 = "lekkich kolczych %s";
			single = FALSE;
			obj->value[ 4 ] = 7;
			set_min_ac( obj );
			obj->cost = 85;
			obj->weight = 50;
			obj->material = 2;
			break;
		case 8:
			name = "ciê¿kie kolcze %s";
			sh_desc = "ciê¿kie kolcze %s";
			name2 = "ciê¿kich kolczych %s";
			name3 = "ciê¿kim kolczym %s";
			name4 = "ciê¿kie kolcze %s";
			name5 = "ciê¿kimi kolczymi %s";
			name6 = "ciê¿kich kolczych %s";
			single = FALSE;
			obj->value[ 4 ] = 8;
			set_min_ac( obj );
			obj->cost = 95;
			obj->weight = 60;
			obj->material = 2;
			break;
		case 9:
			name = "paskowe spodnie";
			sh_desc = "paskowe spodnie";
			name2 = "paskowych spodni";
			name3 = "paskowym spodniom";
			name4 = "paskowe spodnie";
			name5 = "paskowymi spodniami";
			name6 = "paskowych spodniach";

			obj->value[ 4 ] = 9;
			set_min_ac( obj );
			obj->cost = 100;
			obj->weight = 60;
			obj->material = 2;
			break;
		case 10:
			name = "%s z br±zu";
			sh_desc = "%s z br±zu";
			name2 = "%s z br±zu";
			name3 = "%s z br±zu";
			name4 = "%s z br±zu";
			name5 = "%s z br±zu";
			name6 = "%s z br±zu";
			single = FALSE;
			obj->value[ 4 ] = 10;
			set_min_ac( obj );
			obj->cost = 110;
			obj->weight = 60;
			obj->material = 14;
			break;
		case 11:
			name = "utwardzane skórzane spodnie";
			sh_desc = "utwardzane skórzane spodnie";
			name2 = "utwardzanych skórzanych spodni";
			name3 = "utwardzanym skórzanym spodniom";
			name4 = "utwardzane skórzane spodnie";
			name5 = "utwardzanymi skórzanymi spodniami";
			name6 = "utwardzanych skórzanych spodniach";

			obj->value[ 4 ] = 11;
			set_min_ac( obj );
			obj->cost = 115;
			obj->weight = 35;
			obj->material = 44;
			break;
		case 12:
			switch ( dice( 1, 2 ) )
			{
				case 1:
					name = "pó³p³ytowe %s";
					sh_desc = "pó³p³ytowe %s";
					name2 = "pó³p³ytowych %s";
					name3 = "pó³p³ytowym %s";
					name4 = "pó³p³ytowe %s";
					name5 = "pó³p³ytowymi %s";
					name6 = "pó³p³ytowych %s";
					break;
				case 2:
					name = "metalowe %s";
					sh_desc = "metalowe %s";
					name2 = "metalowych %s";
					name3 = "metalowym %s";
					name4 = "metalowe %s";
					name5 = "metalowymi %s";
					name6 = "metalowych %s";
					break;
			}
			single = FALSE;
			obj->value[ 4 ] = 12;
			set_min_ac( obj );
			obj->cost = 120;
			obj->weight = 75;
			obj->material = 2;
			break;
		case 13:
			switch ( dice( 1, 2 ) )
			{
				case 1:
					name = "wzmacniane pó³p³ytowe %s";
					sh_desc = "wzmacniane pó³p³ytowe %s";
					name2 = "wzmacnianych pó³p³ytowych %s";
					name3 = "wzmacnianym pó³p³ytowym %s";
					name4 = "wzmacniane pó³p³ytowe %s";
					name5 = "wzmacnianymi pó³p³ytowymi %s";
					name6 = "wzmacnianych pó³p³ytowych %s";
					break;
				case 2:
					name = "wzmacniane metalowe %s";
					sh_desc = "wzmacniane metalowe %s";
					name2 = "wzmacnianych metalowych %s";
					name3 = "wzmacnianym metalowym %s";
					name4 = "wzmacniane metalowe %s";
					name5 = "wzmacnianymi metalowymi %s";
					name6 = "wzmacnianych metalowych %s";
					break;
			}
			single = FALSE;
			obj->value[ 4 ] = 13;
			set_min_ac( obj );
			obj->cost = 125;
			obj->weight = 75;
			obj->material = 2;
			break;
		case 14:
			name = "p³ytowe %s";
			sh_desc = "p³ytowe %s";
			name2 = "p³ytowych %s";
			name3 = "p³ytowym %s";
			name4 = "p³ytowe %s";
			name5 = "p³ytowymi %s";
			name6 = "p³ytowych %s";
			single = FALSE;
			obj->value[ 4 ] = 14;
			set_min_ac( obj );
			obj->cost = 130;
			obj->weight = 75;
			obj->material = 4;
			break;
		case 15:
			name = "wzmacniane p³ytowe %s";
			sh_desc = "wzmacniane p³ytowe %s";
			name2 = "wzmacnianych p³ytowych %s";
			name3 = "wzmacnianym p³ytowym %s";
			name4 = "wzmacniane p³ytowe %s";
			name5 = "wzmacnianymi p³ytowymi %s";
			name6 = "wzmacnianych p³ytowych %s";
			single = FALSE;
			obj->value[ 4 ] = 15;
			set_min_ac( obj );
			obj->cost = 135;
			obj->weight = 75;
			obj->material = 4;
			break;
		case 16:
			name = "pe³ne p³ytowe %s";
			sh_desc = "pe³ne p³ytowe %s";
			name2 = "pe³nych p³ytowych %s";
			name3 = "pe³nym p³ytowym %s";
			name4 = "pe³ne p³ytowe %s";
			name5 = "pe³nymi p³ytowymi %s";
			name6 = "pe³nych p³ytowych %s";
			single = FALSE;
			obj->value[ 4 ] = 15;
			set_min_ac( obj );
			obj->cost = 135;
			obj->weight = 75;
			obj->material = 4;
			break;
		default : break;
	}

	if ( !single )
	{
		sprintf( bufik, name, nam );
		name = str_dup( bufik );

		sprintf( bufik, sh_desc, sh_de );
		sh_desc = str_dup( bufik );

		sprintf( bufik, name2, nam2 );
		name2 = str_dup( bufik );

		sprintf( bufik, name3, nam3 );
		name3 = str_dup( bufik );

		sprintf( bufik, name4, nam4 );
		name4 = str_dup( bufik );

		sprintf( bufik, name5, nam5 );
		name5 = str_dup( bufik );

		sprintf( bufik, name6, nam6 );
		name6 = str_dup( bufik );
	}

	free_string( obj->short_descr );
	obj->short_descr = str_dup( sh_desc );

	free_string( obj->name );
	obj->name = str_dup( name );

	free_string( obj->name2 );
	obj->name2 = str_dup( name2 );

	free_string( obj->name3 );
	obj->name3 = str_dup( name3 );

	free_string( obj->name4 );
	obj->name4 = str_dup( name4 );

	free_string( obj->name5 );
	obj->name5 = str_dup( name5 );

	free_string( obj->name6 );
	obj->name6 = str_dup( name6 );

	free_string( obj->description );
	sprintf( text, "%s przyci±gaj± twój wzrok.", capitalize( sh_desc ) );
	obj->description = str_dup( text );

	if ( !second ) return;

	if ( number_percent() > MAGIC_ITEMS ) return;
	*points -= 3;

	/* teraz dodatkowe bajery w petli */
	while ( go )
	{
		enchanted = FALSE;
		switch ( number_range( 1, 12 ) )
		{
				/* bonus do AC_PIERCE*/
			case 1:
				obj->value[ 0 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* bonus do AC_BASH*/
			case 2:
				obj->value[ 1 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* bonus do AC_SLASH*/
			case 3:
				obj->value[ 2 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* bonus do AC_EXOTIC*/
			case 4:
				obj->value[ 3 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* lzejsza */
			case 5:
				obj->weight -= 10;
				obj->cost += 5;
				enchanted = TRUE;
				break;
				/* ciezsza */
			case 6:
				obj->weight += 10;
				obj->cost += 5;
				enchanted = TRUE;
				break;
			case 7:
				obj->value[ 0 ] ++;
				obj->cost += 10;
				enchanted = TRUE;
				break;
			case 8:
				obj->value[ 1 ] ++;
				obj->cost += 18;
				enchanted = TRUE;
				break;
			case 9:
				obj->value[ 2 ] ++;
				obj->cost += 6;
				enchanted = TRUE;
				break;
			case 10:
				obj->value[ 3 ] ++;
				obj->cost += 13;
				enchanted = TRUE;
				break;

			default: break;
		}

		if ( enchanted ) * points -= 1;
		if ( enchanted && number_percent() > 60 ) go = FALSE;
		if ( *points <= 0 ) go = FALSE;
	}
	/* koniec petli losujace bajery na valuesach */
	if ( !third ) return;

	obj->enchanted = TRUE;

	return;
}

void generate_armor_arms( OBJ_DATA *obj, int* points )
{
	int wer;
	char text[ MAX_INPUT_LENGTH ], *name = "(null)", *sh_desc = "(null)";
	char *name2 = "null", *name3 = "null", *name4 = "null", *name5 = "null", *name6 = "null";
	bool go = TRUE, enchanted = FALSE, second = FALSE, third = FALSE;
	char *nam = "null", *sh_de = "null", *nam2 = "null", *nam3 = "null", *nam4 = "null", *nam5 = "null", *nam6 = "null";
	char bufik[ MAX_STRING_LENGTH ];
	;

	switch ( *points )
	{
		case 0: wer = 3;break;
		case 1: wer = 4;break;
		case 2: wer = 6;break;
		case 3: wer = 8;break;
		case 4: wer = 10;break;
		case 5: wer = 12;break;
		case 6: wer = 15;break;
		case 7: wer = 16;break;
		default: wer = 10;break;
	}

	if ( *points > 3 )
		second = TRUE;

	if ( *points > 6 )
		third = TRUE;

	generate_extra( obj );
	obj->liczba_mnoga = TRUE;

	switch ( number_range( 1, 4 ) )
	{
		case 1:
			nam = "naramienniki";
			sh_de = "naramienniki";
			nam2 = "naramienników";
			nam3 = "naramiennikom";
			nam4 = "naramienniki";
			nam5 = "naramiennikami";
			nam6 = "naramiennikach";
			break;
		case 2:
			nam = "rêkawy";
			sh_de = "rêkawy";
			nam2 = "rêkawów";
			nam3 = "rêkawom";
			nam4 = "rêkawy";
			nam5 = "rêkawami";
			nam6 = "rêkawach";
			break;
		case 3:
			nam = "narêczaki";
			sh_de = "narêczaki";
			nam2 = "narêczaków";
			nam3 = "narêczakom";
			nam4 = "narêczaki";
			nam5 = "narêczakami";
			nam6 = "narêczakach";
			break;
		case 4:
			nam = "narêkawniki";
			sh_de = "narêkawniki";
			nam2 = "narêkawników";
			nam3 = "narêkawnikom";
			nam4 = "narêkawniki";
			nam5 = "narêkawnikami";
			nam6 = "narêkawnikach";
			break;
	}

	/*no to tutaj zrobie po jednym przedstawicielu kazdego typu pancerza,
	by roznorodnosc byla :]*/
	switch ( number_range( 0, wer ) )
	{
		case 0:
			name = "pikowane %s";
			sh_desc = "pikowane %s";
			name2 = "pikowanych %s";
			name3 = "pikowanym %s";
			name4 = "pikowane %s";
			name5 = "pikowanymi %s";
			name6 = "pikowanych %s";

			obj->value[ 4 ] = 0;
			set_min_ac( obj );
			obj->cost = 15;
			obj->weight = 10;
			obj->material = 6;
			break;
		case 1:
			name = "skórzane %s";
			sh_desc = "skórzane %s";
			name2 = "skórzanych %s";
			name3 = "skórzanym %s";
			name4 = "skórzane %s";
			name5 = "skórzanymi %s";
			name6 = "skórzanych %s";

			obj->value[ 4 ] = 1;
			set_min_ac( obj );
			obj->cost = 20;
			obj->weight = 15;
			obj->material = 6;
			break;
		case 2:
			name = "æwiekowane skórzane %s";
			sh_desc = "æwiekowane skórzane %s";
			name2 = "æwiekowanych skórzanych %s";
			name3 = "æwiekowanym skórzanym %s";
			name4 = "æwiekowane skórzane %s";
			name5 = "æwiekowanymi skórzanymi %s";
			name6 = "æwiekowanych skórzanych %s";

			obj->value[ 4 ] = 2;
			set_min_ac( obj );
			obj->cost = 40;
			obj->weight = 25;
			obj->material = 6;
			break;
		case 3:
			name = "naszywane %s";
			sh_desc = "naszywane %s";
			name2 = "naszywanych %s";
			name3 = "naszywanym %s";
			name4 = "naszywane %s";
			name5 = "naszywanymi %s";
			name6 = "naszywanych %s";

			obj->value[ 4 ] = 3;
			set_min_ac( obj );
			obj->material = 6;
			obj->cost = 50;
			obj->weight = 30;
			break;
		case 4:
			name = "³uskowe %s";
			sh_desc = "³uskowe %s";
			name2 = "³uskowych %s";
			name3 = "³uskowym %s";
			name4 = "³uskowe %s";
			name5 = "³uskowymi %s";
			name6 = "³uskowych %s";

			obj->value[ 4 ] = 4;
			set_min_ac( obj );
			obj->cost = 60;
			obj->weight = 50;
			obj->material = 2;
			break;
		case 5:
			name = "przeszywane %s";
			sh_desc = "przeszywane %s";
			name2 = "przeszywanych %s";
			name3 = "przeszywanym %s";
			name4 = "przeszywane %s";
			name5 = "przeszywanymi %s";
			name6 = "przeszywanych %s";

			obj->value[ 4 ] = 5;
			set_min_ac( obj );
			obj->cost = 65;
			obj->weight = 30;
			obj->material = 6;
			break;
		case 6:
			name = "%s ze skór zwierzêcych";
			sh_desc = "%s ze skór zwierzêcych";
			name2 = "%s ze skór zwierzêcych";
			name3 = "%s ze skór zwierzêcych";
			name4 = "%s ze skór zwierzêcych";
			name5 = "%s ze skór zwierzêcych";
			name6 = "%s ze skór zwierzêcych";

			obj->value[ 4 ] = 6;
			set_min_ac( obj );
			obj->cost = 75;
			obj->weight = 30;
			obj->material = 44;
			break;
		case 7:
			name = "lekkie kolcze %s";
			sh_desc = "lekkie kolcze %s";
			name2 = "lekkich kolczych %s";
			name3 = "lekkim kolczym %s";
			name4 = "lekkie kolcze %s";
			name5 = "lekkimi kolczymi %s";
			name6 = "lekkich kolczych %s";

			obj->value[ 4 ] = 7;
			set_min_ac( obj );
			obj->cost = 85;
			obj->weight = 50;
			obj->material = 2;
			break;
		case 8:
			name = "ciê¿kie kolcze %s";
			sh_desc = "ciê¿kie kolcze %s";
			name2 = "ciê¿kich kolczych %s";
			name3 = "ciê¿kim kolczym %s";
			name4 = "ciê¿kie kolcze %s";
			name5 = "ciê¿kimi kolczymi %s";
			name6 = "ciê¿kich kolczych %s";

			obj->value[ 4 ] = 8;
			set_min_ac( obj );
			obj->cost = 95;
			obj->weight = 60;
			obj->material = 2;
			break;
		case 9:
			name = "paskowe %s";
			sh_desc = "paskowe %s";
			name2 = "paskowych %s";
			name3 = "paskowym %s";
			name4 = "paskowe %s";
			name5 = "paskowymi %s";
			name6 = "paskowych %s";

			obj->value[ 4 ] = 9;
			set_min_ac( obj );
			obj->cost = 100;
			obj->weight = 60;
			obj->material = 2;
			break;
		case 10:
			name = "%s z br±zu";
			sh_desc = "%s z br±zu";
			name2 = "%s z br±zu";
			name3 = "%s z br±zu";
			name4 = "%s z br±zu";
			name5 = "%s z br±zu";
			name6 = "%s z br±zu";

			obj->value[ 4 ] = 10;
			set_min_ac( obj );
			obj->cost = 110;
			obj->weight = 60;
			obj->material = 14;
			break;
		case 11:
			name = "utwardzane skórzane %s";
			sh_desc = "utwardzane skórzane %s";
			name2 = "utwardzanych skórzanych %s";
			name3 = "utwardzanym skórzanym %s";
			name4 = "utwardzane skórzane %s";
			name5 = "utwardzanymi skórzanymi %s";
			name6 = "utwardzanych skórzanych %s";

			obj->value[ 4 ] = 11;
			set_min_ac( obj );
			obj->cost = 115;
			obj->weight = 35;
			obj->material = 44;
			break;
		case 12:
			switch ( dice( 1, 2 ) )
			{
				case 1:
					name = "pó³p³ytowe %s";
					sh_desc = "pó³p³ytowe %s";
					name2 = "pó³p³ytowych %s";
					name3 = "pó³p³ytowym %s";
					name4 = "pó³p³ytowe %s";
					name5 = "pó³p³ytowymi %s";
					name6 = "pó³p³ytowych %s";
					break;
				case 2:
					name = "metalowe %s";
					sh_desc = "metalowe %s";
					name2 = "metalowych %s";
					name3 = "metalowym %s";
					name4 = "metalowe %s";
					name5 = "metalowymi %s";
					name6 = "metalowych %s";
					break;
			}

			obj->value[ 4 ] = 12;
			set_min_ac( obj );
			obj->cost = 120;
			obj->weight = 75;
			obj->material = 2;
			break;
		case 13:
			switch ( dice( 1, 2 ) )
			{
				case 1:
					name = "wzmacniane pó³p³ytowe %s";
					sh_desc = "wzmacniane pó³p³ytowe %s";
					name2 = "wzmacnianych pó³p³ytowych %s";
					name3 = "wzmacnianym pó³p³ytowym %s";
					name4 = "wzmacniane pó³p³ytowe %s";
					name5 = "wzmacnianymi pó³p³ytowymi %s";
					name6 = "wzmacnianych pó³p³ytowych %s";
					break;
				case 2:
					name = "wzmacniane metalowe %s";
					sh_desc = "wzmacniane metalowe %s";
					name2 = "wzmacnianych metalowych %s";
					name3 = "wzmacnianym metalowym %s";
					name4 = "wzmacniane metalowe %s";
					name5 = "wzmacnianymi metalowymi %s";
					name6 = "wzmacnianych metalowych %s";
					break;
			}

			obj->value[ 4 ] = 13;
			set_min_ac( obj );
			obj->cost = 125;
			obj->weight = 75;
			obj->material = 2;
			break;
		case 14:
			name = "p³ytowe %s";
			sh_desc = "p³ytowe %s";
			name2 = "p³ytowych %s";
			name3 = "p³ytowym %s";
			name4 = "p³ytowe %s";
			name5 = "p³ytowymi %s";
			name6 = "p³ytowych %s";

			obj->value[ 4 ] = 14;
			set_min_ac( obj );
			obj->cost = 130;
			obj->weight = 75;
			obj->material = 4;
			break;
		case 15:
			name = "wzmacniane p³ytowe %s";
			sh_desc = "wzmacniane p³ytowe %s";
			name2 = "wzmacnianych p³ytowych %s";
			name3 = "wzmacnianym p³ytowym %s";
			name4 = "wzmacniane p³ytowe %s";
			name5 = "wzmacnianymi p³ytowymi %s";
			name6 = "wzmacnianych p³ytowych %s";

			obj->value[ 4 ] = 15;
			set_min_ac( obj );
			obj->cost = 135;
			obj->weight = 75;
			obj->material = 4;
			break;
		case 16:
			name = "pe³ne p³ytowe %s";
			sh_desc = "pe³ne p³ytowe %s";
			name2 = "pe³nych p³ytowych %s";
			name3 = "pe³nym p³ytowym %s";
			name4 = "pe³ne p³ytowe %s";
			name5 = "pe³nymi p³ytowymi %s";
			name6 = "pe³nych p³ytowych %s";

			obj->value[ 4 ] = 15;
			set_min_ac( obj );
			obj->cost = 135;
			obj->weight = 75;
			obj->material = 4;
			break;
		default : break;
	}

	sprintf( bufik, name, nam );
	name = str_dup( bufik );

	sprintf( bufik, sh_desc, sh_de );
	sh_desc = str_dup( bufik );

	sprintf( bufik, name2, nam2 );
	name2 = str_dup( bufik );

	sprintf( bufik, name3, nam3 );
	name3 = str_dup( bufik );

	sprintf( bufik, name4, nam4 );
	name4 = str_dup( bufik );

	sprintf( bufik, name5, nam5 );
	name5 = str_dup( bufik );

	sprintf( bufik, name6, nam6 );
	name6 = str_dup( bufik );


	free_string( obj->short_descr );
	obj->short_descr = str_dup( sh_desc );

	free_string( obj->name );
	obj->name = str_dup( name );

	free_string( obj->name2 );
	obj->name2 = str_dup( name2 );

	free_string( obj->name3 );
	obj->name3 = str_dup( name3 );

	free_string( obj->name4 );
	obj->name4 = str_dup( name4 );

	free_string( obj->name5 );
	obj->name5 = str_dup( name5 );

	free_string( obj->name6 );
	obj->name6 = str_dup( name6 );

	free_string( obj->description );
	sprintf( text, "%s przyci±gaj± twój wzrok.", capitalize( sh_desc ) );
	obj->description = str_dup( text );

	if ( !second ) return;

	if ( number_percent() > MAGIC_ITEMS ) return;
	*points -= 3;

	/* teraz dodatkowe bajery w petli */
	while ( go )
	{
		enchanted = FALSE;
		switch ( number_range( 1, 12 ) )
		{
				/* bonus do AC_PIERCE*/
			case 1:
				obj->value[ 0 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* bonus do AC_BASH*/
			case 2:
				obj->value[ 1 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* bonus do AC_SLASH*/
			case 3:
				obj->value[ 2 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* bonus do AC_EXOTIC*/
			case 4:
				obj->value[ 3 ] --;
				obj->cost += obj->cost / 4;
				enchanted = TRUE;
				break;
				/* lzejsza */
			case 5:
				obj->weight -= 10;
				obj->cost += 5;
				enchanted = TRUE;
				break;
				/* ciezsza */
			case 6:
				obj->weight += 10;
				obj->cost += 5;
				enchanted = TRUE;
				break;
			case 7:
				obj->value[ 0 ] ++;
				obj->cost += 10;
				enchanted = TRUE;
				break;
			case 8:
				obj->value[ 1 ] ++;
				obj->cost += 18;
				enchanted = TRUE;
				break;
			case 9:
				obj->value[ 2 ] ++;
				obj->cost += 6;
				enchanted = TRUE;
				break;
			case 10:
				obj->value[ 3 ] ++;
				obj->cost += 13;
				enchanted = TRUE;
				break;

			default: break;
		}

		if ( enchanted ) * points -= 1;
		if ( enchanted && number_percent() > 60 ) go = FALSE;
		if ( *points <= 0 ) go = FALSE;
	}
	if ( !third ) return;

	obj->enchanted = TRUE;

	return;
}

void generate_bracelet( OBJ_DATA *obj, int* points )
{
	char txt[ MAX_INPUT_LENGTH ];
	char text[ MAX_INPUT_LENGTH ], *name = "(null)", *sh_desc = "(null)";
	char *name2 = "(null)", *name3 = "(null)", *name4 = "(null)", *name5 = "(null)", *name6 = "(null)";
	char *name_des = "n", *sh_des = "n", *name2_des = "n", *name3_des = "n", *name4_des = "n", *name5_des = "n", *name6_des = "n";
	bool losowy = FALSE, szyk_przestawny = FALSE;
	int elem, wer, ilosc_affectow, kasa = 1;
	int type = 0, loc = 0, mod = 0, level = 0, c = 0, val;
	BITVECT_DATA * bitv = &AFF_NONE;

	/* sprawdzenie czy *points ma sensown± warto¶æ */
	if ( (*points < 1) || (*points > 7) )
	  {
	    bugf("generate_bracelet: Z³a warto¶æ argumentu points: %d", *points);
	  }

	switch ( *points )
	{
		case 1: wer = 1000;break;
		case 2: wer = 500;break;
		case 3: wer = 300;break;
		case 4: wer = 200;break;
		case 5: wer = 150;break;
		case 6: wer = 100;break;
		case 7: wer = 0;break;
	}

	generate_extra( obj );

	obj->value[ 0 ] = 0;
	obj->value[ 1 ] = 0;
	obj->value[ 2 ] = 0;
	obj->value[ 3 ] = 0;
	obj->cost = 45;
	obj->weight = 5;

/*
                                  | str | con | dex | int | wis | cha | luc | extra
 1: bransoletka kamiennego golem  |     |     | -2  |     |     |     |     | 10% resist slash, 10% resist pierce
 2: bransoletka miêsnego golema   |     |     | +2  |     |     |     |     | 10% resist bash
 3: bransoleta sfery              |     |     |     |     |     |     |     | +2 save_vs_spell(to jest gorzej), minor_globe
 4: bransoleta z sierpem i m³otem | +1  |     |     | -1  | -1  |     |     | +15% mace
 5: skóra psiego demona           |     |     | +2  |     |     |     |     | +30% track, +20mv
 6: skóra ghula                   |     |     |     |     |     | -2  | -2  | -5 save_vs_death, 40% resist_negative_energy, 40% resist_poison, 20% resist_cold
*/
	switch ( c = number_range( 1, 800 ) )
	{
		case 1:
			name = "bransoletka kamiennego golema";
			sh_desc = "bransoletka kamiennego golema";
			name2 = "bransoletki kamiennego golema";
			name3 = "bransoletce kamiennego golema";
			name4 = "bransoletkê kamiennego golema";
			name5 = "bransoletk± kamiennego golema";
			name6 = "bransoletce kamiennego golema";
			break;
		case 2:
			name = "bransoletka miêsnego golema";
			sh_desc = "bransoletka miêsnego golema";
			name2 = "bransoletki miêsnego golema";
			name3 = "bransoletce miêsnego golema";
			name4 = "bransoletkê miêsnego golema";
			name5 = "bransoletk± miêsnego golema";
			name6 = "bransoletce miêsnego golema";
			break;
		case 3:
			name = "bransoleta sfery";
			sh_desc = "bransoleta sfery";
			name2 = "bransolety sfery";
			name3 = "bransolecie sfery";
			name4 = "bransoletê sfery";
			name5 = "bransolet± sfery";
			name6 = "bransolecie sfery";
			break;
		case 4:
			name = "bransoleta z sierpem i m³otem";
			sh_desc = "bransoleta z sierpem i m³otem";
			name2 = "bransolety z sierpem i m³otem";
			name3 = "bransolecie z sierpem i m³otem";
			name4 = "bransoletê z sierpem i m³otem";
			name5 = "bransolet± z sierpem i m³otem";
			name6 = "bransolecie z sierpem i m³otem";
			break;
		case 5:
			name = "skóra psiego demona";
			sh_desc = "skóra psiego demona";
			name2 = "skóry psiego demona";
			name3 = "skórze psiego demona";
			name4 = "skórê psiego demona";
			name5 = "skór± psiego demona";
			name6 = "skórze psiego demona";
			break;
		case 6:
			name = "skóra ghula";
			sh_desc = "skóra ghula";
			name2 = "skóry ghula";
			name3 = "skórze ghula";
			name4 = "skórê ghula";
			name5 = "skór± ghula";
			name6 = "skórze ghula";
			break;

		default:    //czyli kompletnie losowy kolczyk
			losowy = TRUE;
			switch ( number_range( 1, 4 ) )
			{
				case 1:
					name = "bransoletka";
					sh_desc = "bransoletka";
					name2 = "bransoletki";
					name3 = "bransoletce";
					name4 = "bransoletkê";
					name5 = "bransoletk±";
					name6 = "bransoletce";
					break;

				default:
					name = "bransoleta";
					sh_desc = "bransoleta";
					name2 = "bransolety";
					name3 = "bransolecie";
					name4 = "bransoletê";
					name5 = "bransolet±";
					name6 = "bransolecie";
					break;
			}

			if ( number_range( 1, 3 ) != 1 )
			{
				szyk_przestawny = FALSE;

				switch ( number_range( 1, 30 ) )
				{
					case 1:
						obj->material = 5;
						obj->cost += 40;
						sh_des = "miedziana";
						name_des = "miedziana";
						name2_des = "miedzianej";
						name3_des = "miedzianej";
						name4_des = "miedzian±";
						name5_des = "miedzian±";
						name6_des = "miedzianej";
						break;
					case 2:
						obj->material = 21;
						obj->cost += 10;
						sh_des = "ko¶ciana";
						name_des = "ko¶ciana";
						name2_des = "ko¶cianej";
						name3_des = "ko¶cianej";
						name4_des = "ko¶cian±";
						name5_des = "ko¶cian±";
						name6_des = "ko¶cianej";
						break;
					case 3:
						obj->material = 14;
						obj->cost += 30;
						sh_des = "br±zowa";
						name_des = "br±zowa";
						name2_des = "br±zowej";
						name3_des = "br±zowej";
						name4_des = "br±zow±";
						name5_des = "br±zow±";
						name6_des = "br±zowej";
						break;
					case 4:
						obj->material = 16;
						obj->cost += 20;
						sh_des = "kamienna";
						name_des = "kamienna";
						name2_des = "kamiennej";
						name3_des = "kamiennej";
						name4_des = "kamienn±";
						name5_des = "kamienn±";
						name6_des = "kamiennej";
						break;
					case 5:
						obj->material = 13;
						obj->cost += 80;
						sh_des = "¿elazna";
						name_des = "¿elazna";
						name2_des = "¿elaznej";
						name3_des = "¿elaznej";
						name4_des = "¿elazn±";
						name5_des = "¿elazn±";
						name6_des = "¿elaznej";
						break;
					case 6:
						obj->material = 2;
						obj->cost += 100;
						sh_des = "stalowa";
						name_des = "stalowa";
						name2_des = "stalowej";
						name3_des = "stalowej";
						name4_des = "stalow±";
						name5_des = "stalow±";
						name6_des = "stalowej";
						break;
					case 7:
						obj->material = 9;
						obj->cost += 100;
						sh_des = "finezyjna";
						name_des = "finezyjna";
						name2_des = "finezyjnej";
						name3_des = "finezyjnej";
						name4_des = "finezyjn±";
						name5_des = "finezyjn±";
						name6_des = "finezyjnej";
						break;
					case 8:
						obj->material = 9;
						obj->cost += 100;
						sh_des = "figlarna";
						name_des = "figlarna";
						name2_des = "figlarnej";
						name3_des = "figlarnej";
						name4_des = "figlarn±";
						name5_des = "figlarn±";
						name6_des = "figlarnej";
						break;
					case 9:
						obj->cost += 120;
						obj->material = 4;
						sh_des = "doskona³a";
						name_des = "doskona³a";
						name2_des = "doskona³ej";
						name3_des = "doskona³ej";
						name4_des = "doskona³±";
						name5_des = "doskona³±";
						name6_des = "doskona³ej";
						break;
					case 10:
						obj->cost += 750;
						obj->material = 36;
						sh_des = "diamentowa";
						name_des = "diamentowa";
						name2_des = "diamentowej";
						name3_des = "diamentowej";
						name4_des = "diamentow±";
						name5_des = "diamentow±";
						name6_des = "diamentowej";
						break;
					case 11:
						obj->cost += 200;
						obj->material = 43;
						sh_des = "mosiê¿na";
						name_des = "mosiê¿na";
						name2_des = "mosiê¿nej";
						name3_des = "mosiê¿nej";
						name4_des = "mosiê¿n±";
						name5_des = "mosiê¿n±";
						name6_des = "mosiê¿nej";
						break;
					case 12:
						obj->material = 3;
						obj->cost += 130;
						sh_des = "elfia";
						name_des = "elfia";
						name2_des = "elfiej";
						name3_des = "elfiej";
						name4_des = "elfi±";
						name5_des = "elfi±";
						name6_des = "elfiej";
						break;
					case 13:
						obj->material = 10;
						obj->cost += 140;
						sh_des = "srebrna";
						name_des = "srebrna";
						name2_des = "srebrnej";
						name3_des = "srebrnej";
						name4_des = "srebrn±";
						name5_des = "srebrn±";
						name6_des = "srebrnej";
						break;
					case 14:
						obj->material = 15;
						obj->cost += 150;
						sh_des = "szklana";
						name_des = "szklana";
						name2_des = "szklanej";
						name3_des = "szklanej";
						name4_des = "szklan±";
						name5_des = "szklan±";
						name6_des = "szklanej";
						break;
					case 15:
						obj->cost += 250;
						obj->material = 9;
						sh_des = "z³ota";
						name_des = "z³ota";
						name2_des = "z³otej";
						name3_des = "z³otej";
						name4_des = "z³ot±";
						name5_des = "z³ot±";
						name6_des = "z³otej";
						break;
					case 16:
						obj->cost += 250;
						obj->material = number_range( 2, 4 );
						sh_des = "poz³acana";
						name_des = "poz³acana";
						name2_des = "poz³acanej";
						name3_des = "poz³acanej";
						name4_des = "poz³acan±";
						name5_des = "poz³acan±";
						name6_des = "poz³acanej";
						break;
					case 17:
						obj->cost += 250;
						obj->material = number_range( 2, 4 );;
						sh_des = "posrebrzana";
						name_des = "posrebrzana";
						name2_des = "posrebrzanej";
						name3_des = "posrebrzanej";
						name4_des = "posrebrzan±";
						name5_des = "posrebrzan±";
						name6_des = "posrebrzanej";
						break;
					case 18:
						obj->cost += 300;
						obj->material = 7;
						sh_des = "adamantytowa";
						name_des = "adamantytowa";
						name2_des = "adamantytowej";
						name3_des = "adamantytowej";
						name4_des = "adamantytow±";
						name5_des = "adamantytow±";
						name6_des = "adamantytowej";
						break;
					case 19:
						obj->material = 10;
						obj->cost += 140;
						sh_des = "gnomia";
						name_des = "gnomia";
						name2_des = "gnomiej";
						name3_des = "gnomiej";
						name4_des = "gnomi±";
						name5_des = "gnomi±";
						name6_des = "gnomiej";
						break;
					case 20:
						obj->material = 8;
						obj->cost += 400;
						sh_des = "mithrilowa";
						name_des = "mithrilowa";
						name2_des = "mithrilowej";
						name3_des = "mithrilowej";
						name4_des = "mithrilow±";
						name5_des = "mithrilow±";
						name6_des = "mithrilowej";
						break;
					case 21:
						obj->material = 11;
						obj->cost += 400;
						sh_des = "kryszta³owa";
						name_des = "kryszta³owa";
						name2_des = "kryszta³owej";
						name3_des = "kryszta³owej";
						name4_des = "kryszta³ow±";
						name5_des = "kryszta³ow±";
						name6_des = "kryszta³owej";
						break;
					case 22:
						obj->cost += 400;
						obj->material = 34;
						sh_des = "platynowa";
						name_des = "platynowa";
						name2_des = "platynowej";
						name3_des = "platynowej";
						name4_des = "platynow±";
						name5_des = "platynow±";
						name6_des = "platynowej";
						break;
					case 23:
						obj->material = 10;
						obj->cost += 100;
						sh_des = "grawerowana";
						name_des = "grawerowana";
						name2_des = "grawerowanej";
						name3_des = "grawerowanej";
						name4_des = "grawerowan±";
						name5_des = "grawerowan±";
						name6_des = "grawerowanej";
						break;
					case 24:
						obj->material = 10;
						obj->cost += 100;
						sh_des = "b³yszcz±ca";
						name_des = "b³yszcz±ca";
						name2_des = "b³yszcz±cej";
						name3_des = "b³yszcz±cej";
						name4_des = "b³yszcz±c±";
						name5_des = "b³yszcz±c±";
						name6_des = "b³yszcz±cej";
						break;
					case 25:
						obj->material = 10;
						obj->cost += 100;
						sh_des = "mistrzowsko wykonana";
						name_des = "mistrzowsko wykonana";
						name2_des = "mistrzowsko wykonanej";
						name3_des = "mistrzowsko wykonanej";
						name4_des = "mistrzowsko wykonan±";
						name5_des = "mistrzowsko wykonan±";
						name6_des = "mistrzowsko wykonanej";
						break;
					case 26:
						obj->material = 10;
						obj->cost += 100;
						sh_des = "fiku¶na";
						name_des = "fiku¶na";
						name2_des = "fiku¶nej";
						name3_des = "fiku¶nej";
						name4_des = "fiku¶n±";
						name5_des = "fiku¶n±";
						name6_des = "fiku¶nej";
						break;
					case 27:
						obj->material = 10;
						obj->cost += 100;
						sh_des = "powyginana";
						name_des = "powyginana,";
						name2_des = "powyginanej";
						name3_des = "powyginanej";
						name4_des = "powyginan±";
						name5_des = "powyginan±";
						name6_des = "powyginanej";
						break;
					case 28:
						obj->material = 10;
						obj->cost += 100;
						sh_des = "popêkana";
						name_des = "popêkana";
						name2_des = "popêkanej";
						name3_des = "popêkanej";
						name4_des = "popêkan±";
						name5_des = "popêkan±";
						name6_des = "popêkanej";
						break;
					case 29:
						obj->material = 10;
						obj->cost += 100;
						sh_des = "trójk±tna";
						name_des = "trójk±tna";
						name2_des = "trójk±tnej";
						name3_des = "trójk±tnej";
						name4_des = "trójk±tn±";
						name5_des = "trójk±tn±";
						name6_des = "trójk±tnej";
						break;
					case 30:
						obj->material = 10;
						obj->cost += 100;
						sh_des = "pó³kolista";
						name_des = "pó³kolista";
						name2_des = "pó³kolistej";
						name3_des = "pó³kolistej";
						name4_des = "pó³kolist±";
						name5_des = "pó³kolist±";
						name6_des = "pó³kolistej";
						break;
				}
			}
			else
			{
				szyk_przestawny = TRUE;
				obj->material = 10;
				switch ( number_range( 1, 17 ) )
				{
					case 1:
						sh_des = "z krwawnikiem";
						name_des = "z krwawnikiem";
						name2_des = "z krwawnikiem";
						name3_des = "z krwawnikiem";
						name4_des = "z krwawnikiem";
						name5_des = "z krwawnikiem";
						name6_des = "z krwawnikiem";
						break;
					case 2:
						sh_des = "z onyksem";
						name_des = "z onyksem";
						name2_des = "z onyksem";
						name3_des = "z onyksem";
						name4_des = "z onyksem";
						name5_des = "z onyksem";
						name6_des = "z onyksem";
						break;
					case 3:
						sh_des = "z sardonyksem";
						name_des = "z sardonyksem";
						name2_des = "z sardonyksem";
						name3_des = "z sardonyksem";
						name4_des = "z sardonyksem";
						name5_des = "z sardonyksem";
						name6_des = "z sardonyksem";
						break;
					case 4:
						sh_des = "z karnelianem";
						name_des = "z karnelianem";
						name2_des = "z karnelianem";
						name3_des = "z karnelianem";
						name4_des = "z karnelianem";
						name5_des = "z karnelianem";
						name6_des = "z karnelianem";
						break;
					case 5:
						sh_des = "z jaspisem";
						name_des = "z jaspisem";
						name2_des = "z jaspisem";
						name3_des = "z jaspisem";
						name4_des = "z jaspisem";
						name5_des = "z jaspisem";
						name6_des = "z jaspisem";
						break;
					case 6:
						sh_des = "z spinelem";
						name_des = "z spinelem";
						name2_des = "z spinelem";
						name3_des = "z spinelem";
						name4_des = "z spinelem";
						name5_des = "z spinelem";
						name6_des = "z spinelem";
						obj->cost += 300;
						break;
					case 7:
						sh_des = "z szafirem";
						name_des = "z szafirem";
						name2_des = "z szafirem";
						name3_des = "z szafirem";
						name4_des = "z szafirem";
						name5_des = "z szafirem";
						name6_des = "z szafirem";
						obj->cost += 999;
						break;
					case 8:
						sh_des = "z ametystem";
						name_des = "z ametystem";
						name2_des = "z ametystem";
						name3_des = "z ametystem";
						name4_des = "z ametystem";
						name5_des = "z ametystem";
						name6_des = "z ametystem";
						break;
					case 9:
						sh_des = "z koralem";
						name_des = "z koralem";
						name2_des = "z koralem";
						name3_des = "z koralem";
						name4_des = "z koralem";
						name5_des = "z koralem";
						name6_des = "z koralem";
						obj->cost += 100;
						break;
					case 10:
						sh_des = "z turmalinem";
						name_des = "z turmalinem";
						name2_des = "z turmalinem";
						name3_des = "z turmalinem";
						name4_des = "z turmalinem";
						name5_des = "z turmalinem";
						name6_des = "z turmalinem";
						break;
					case 11:
						sh_des = "z malachitem";
						name_des = "z malachitem";
						name2_des = "z malachitem";
						name3_des = "z malachitem";
						name4_des = "z malachitem";
						name5_des = "z malachitem";
						name6_des = "z malachitem";
						break;
					case 12:
						sh_des = "z piór";
						name_des = "z piór";
						name2_des = "z piór";
						name3_des = "z piór";
						name4_des = "z piór";
						name5_des = "z piór";
						name6_des = "z piór";
						break;
					case 13:
						sh_des = "z awenturynem";
						name_des = "z awenturynem";
						name2_des = "z awenturynem";
						name3_des = "z awenturynem";
						name4_des = "z awenturynem";
						name5_des = "z awenturynem";
						name6_des = "z awenturynem";
						break;
					case 14:
						sh_des = "z ludzkich w³osów";
						name_des = "z ludzkich w³osów";
						name2_des = "z ludzkich w³osów";
						name3_des = "z ludzkich w³osów";
						name4_des = "z ludzkich w³osów";
						name5_des = "z ludzkich w³osów";
						name6_des = "z ludzkich w³osów";
						break;
					case 15:
						sh_des = "z ludzkich w³osów";
						name_des = "z ludzkich w³osów";
						name2_des = "z ludzkich w³osów";
						name3_des = "z ludzkich w³osów";
						name4_des = "z ludzkich w³osów";
						name5_des = "z ludzkich w³osów";
						name6_des = "z ludzkich w³osów";
						break;
					case 16:
						sh_des = "w kszta³cie smoczej g³owy";
						name_des = "w kszta³cie smoczej g³owy";
						name2_des = "w kszta³cie smoczej g³owy";
						name3_des = "w kszta³cie smoczej g³owy";
						name4_des = "w kszta³cie smoczej g³owy";
						name5_des = "w kszta³cie smoczej g³owy";
						name6_des = "w kszta³cie smoczej g³owy";
						EXT_SET_BIT( obj->wear_flags2, ITEM_NOPAL );
						break;
					case 17:
						sh_des = "w kszta³cie trupiej czaszki";
						name_des = "w kszta³cie trupiej czaszki";
						name2_des = "w kszta³cie trupiej czaszki";
						name3_des = "w kszta³cie trupiej czaszki";
						name4_des = "w kszta³cie trupiej czaszki";
						name5_des = "w kszta³cie trupiej czaszki";
						name6_des = "w kszta³cie trupiej czaszki";
						EXT_SET_BIT( obj->wear_flags2, ITEM_NOPAL );
						break;
				}
			}
	}

	if ( losowy )
	{
		if ( !szyk_przestawny )    //tutaj np "szklana bransoletka, po else np "bransoletka z krwawnikiem"
		{
			free_string( obj->short_descr );
			sprintf( txt, "%s %s", sh_des, sh_desc );
			obj->short_descr = str_dup( txt );

			free_string( obj->name );
			sprintf( txt, "%s %s", name_des, name );
			obj->name = str_dup( txt );

			free_string( obj->name2 );
			sprintf( txt, "%s %s", name2_des, name2 );
			obj->name2 = str_dup( txt );

			free_string( obj->name3 );
			sprintf( txt, "%s %s", name3_des, name3 );
			obj->name3 = str_dup( txt );

			free_string( obj->name4 );
			sprintf( txt, "%s %s", name4_des, name4 );
			obj->name4 = str_dup( txt );

			free_string( obj->name5 );
			sprintf( txt, "%s %s", name5_des, name5 );
			obj->name5 = str_dup( txt );

			free_string( obj->name6 );
			sprintf( txt, "%s %s", name6_des, name6 );
			obj->name6 = str_dup( txt );
		}
		else
		{
			free_string( obj->short_descr );
			sprintf( txt, "%s %s", sh_desc, sh_des );
			obj->short_descr = str_dup( txt );

			free_string( obj->name );
			sprintf( txt, "%s %s", name, name_des );
			obj->name = str_dup( txt );

			free_string( obj->name2 );
			sprintf( txt, "%s %s", name2, name2_des );
			obj->name2 = str_dup( txt );

			free_string( obj->name3 );
			sprintf( txt, "%s %s", name3, name3_des );
			obj->name3 = str_dup( txt );

			free_string( obj->name4 );
			sprintf( txt, "%s %s", name4, name4_des );
			obj->name4 = str_dup( txt );

			free_string( obj->name5 );
			sprintf( txt, "%s %s", name5, name5_des );
			obj->name5 = str_dup( txt );

			free_string( obj->name6 );
			sprintf( txt, "%s %s", name6, name6_des );
			obj->name6 = str_dup( txt );
		}
	}
	else
	{
		free_string( obj->short_descr ); obj->short_descr = str_dup( sh_desc );
		free_string( obj->name ); obj->name = str_dup( name );
		free_string( obj->name2 ); obj->name2 = str_dup( name2 );
		free_string( obj->name3 ); obj->name3 = str_dup( name3 );
		free_string( obj->name4 ); obj->name4 = str_dup( name4 );
		free_string( obj->name5 ); obj->name5 = str_dup( name5 );
		free_string( obj->name6 ); obj->name6 = str_dup( name6 );
	}

	free_string( obj->description );
	sprintf( text, "%s po³yskuje magicznym blaskiem.", capitalize( obj->short_descr ) );
	obj->description = str_dup( text );

	switch ( c )
	{
		case 1:    //bransoletka kamiennego golema
			add_new_affect( obj, 0, APPLY_DEX, -2, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_RESIST, RESIST_SLASH, 10, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_RESIST, RESIST_PIERCE, 10, &AFF_NONE, -1, TRUE );
			obj->material = 16; //kamien
			obj->cost += 40 * COST_APPLY_RESIST;//slash i pierce czyli *2 koszt
			break;
		case 2:    //bransoletka miesnego golema
			add_new_affect( obj, 0, APPLY_DEX, 2, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_RESIST, RESIST_BASH, 10, &AFF_NONE, -1, TRUE );
			obj->material = 7; //adamantyt
			obj->cost += 2 * COST_APPLY_STAT + 20 * COST_APPLY_RESIST;//bash czyli *2 koszt
			break;
		case 3:    //brasnoleta sfery
			add_new_affect( obj, 0, APPLY_SAVING_SPELL, 2, 0, &AFF_MINOR_GLOBE, -1, TRUE );
			obj->cost += 2500;
			obj->material = 7;
			break;
		case 4:    //bransoleta z sierpem i mlotem
			add_new_affect( obj, 0, APPLY_STR, 2, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_WIS, -1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_INT, -1, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_SKILL, gsn_mace, 10, &AFF_NONE, -1, TRUE );
			obj->cost += 2 * COST_APPLY_STAT + 10 * COST_APPLY_SKILL;
			obj->material = 7;
			break;
		case 5:    //skóra psiego demona
			add_new_affect( obj, 0, APPLY_MOVE, 20, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_SKILL, gsn_track, 20, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_DEX, 1, 0, &AFF_NONE, -1, TRUE );
			obj->cost += 1 * COST_APPLY_STAT + 20 * COST_APPLY_SKILL + 20 * COST_APPLY_SKILL;
			obj->material = 8;
			break;
		case 6:    //skóra ghula
			add_new_affect( obj, 0, APPLY_SAVING_DEATH, -5, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_RESIST, RESIST_NEGATIVE, 40, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_RESIST, RESIST_POISON, 40, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_RESIST, RESIST_COLD, 20, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_CHA, -2, 0, &AFF_NONE, -1, FALSE );
			add_new_affect( obj, 0, APPLY_LUC, -2, 0, &AFF_NONE, -1, TRUE );
			obj->cost += 5 * COST_APPLY_SAVING + 50 * COST_APPLY_RESIST;
			obj->material = 8;
			break;

		default:

			if ( number_range( 1, 200 + wer ) < 25 ) ilosc_affectow = 3;
			else if ( number_range( 1, 100 + wer ) < 70 ) ilosc_affectow = 2;
			else ilosc_affectow = 1;

			for (; ilosc_affectow > 0; --ilosc_affectow )
			{
				bitv = get_bitvector();
				kasa += get_cost_of_bitvector(bitv);

				switch ( number_range( 7, 22 ) )
				{
/* WY£¡CZAM: Gurthg - 2005-02-20
					case 1:    //str
						loc = APPLY_STR;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							kasa += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 2:    //int
						loc = APPLY_INT;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							kasa += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 3:    //wis
						loc = APPLY_WIS;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							kasa += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 4:    //dex
						loc = APPLY_DEX;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							kasa += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 5:    //con
						loc = APPLY_CON;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							kasa += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 6:    //cha
						loc = APPLY_CHA;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							kasa += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
					case 7:    //luc
						loc = APPLY_LUC;
						if ( number_percent() > NUMBER_PERCENT_FOR_STAT )
						{
							mod = 1;
							kasa += COST_APPLY_STAT;
						}
						else mod = -1;
						break;
*/
					case 8:    //mov
						loc = APPLY_MOVE;
						mod = number_range( 1, 10 );
						kasa += COST_APPLY_SKILL * mod;
						break;
					case 9:    //hp
						loc = APPLY_HIT;
						mod = number_range( 1, 10 );
						kasa += COST_APPLY_SKILL * mod;
						break;
					case 10:    //spell_mem
						loc = APPLY_MEMMING;
						mod = number_range( 1, 9 );
						switch ( mod )
						{ //czeba poprawic to na pierdzionkach tez
							case 1: level = number_range( 0, 4 );break;
							case 2: level = number_range( 0, 3 );break;
							case 3: level = number_range( 0, 3 );break;
							case 4: level = number_range( 0, 2 );break;
							case 5: level = number_range( 0, 1 );break;
							case 6: level = number_range( 0, 1 );break;
							default: level = 1;break;
						}
						level = UMAX(1, level);
						kasa += number_range( 150, 180 ) * ( 2 * mod * level );
						break;
					case 11:    //ac
						loc = APPLY_AC;
						val = number_range( -10, 20 );
						mod = -val;
						kasa += val * COST_APPLY_AC;
						break;
					case 12:    //dam
						loc = APPLY_DAMROLL;
						switch( number_range(1,20))
						{
							case 1:
								mod = -2;
								break;
							case 2:
							case 3:
							case 4:
							case 5:
							case 6:
							case 7:
							case 8:
							case 9:
							case 10:
								mod = -1;
								break;
							case 11:
							case 12:
							case 13:
							case 14:
							case 15:
							case 16:
							case 17:
							case 18:
							case 19:
								mod = 1;
								break;
							case 20:
								mod = 2;
								break;
						}
						kasa += COST_APPLY_DAMROLL * mod;
						break;
					case 13:    //hit
						loc = APPLY_HITROLL;
						switch( number_range(1,20))
						{
							case 1:
								mod = -2;
								break;
							case 2:
							case 3:
							case 4:
							case 5:
							case 6:
							case 7:
							case 8:
							case 9:
							case 10:
								mod = -1;
								break;
							case 11:
							case 12:
							case 13:
							case 14:
							case 15:
							case 16:
							case 17:
							case 18:
							case 19:
								mod = 1;
								break;
							case 20:
								mod = 2;
								break;
						}
						kasa += COST_APPLY_HITROLL * mod;
						break;
					case 14:
					case 15:
						loc = APPLY_SKILL;
						level = number_range( 1, 10 );
						switch ( number_range( 1, 59 ) )
						{
							// ppodstawowe skille do u¿ywania broni czê¶ciej
							case  1:
							case  2: mod = gsn_axe;break;
							case  3:
							case  4: mod = gsn_flail;break;
							case  5:
							case  6: mod = gsn_dagger;break;
							case  7:
							case  8: mod = gsn_mace;break;
							case  9:
							case 10: mod = gsn_polearm;break;
							case 11:
							case 12: mod = gsn_staff;break;
							case 13:
							case 14: mod = gsn_spear;break;
							case 15:
							case 16: mod = gsn_sword;break;
							case 17:
							case 18: mod = gsn_whip;break;

							case 19: mod = gsn_shield_block;break;
							case 20: mod = gsn_track;break;
							case 21: mod = gsn_backstab;break;
							case 22: mod = gsn_bash;break;
							case 23: mod = gsn_berserk;break;
							case 24: mod = gsn_disarm;break;
							case 25: mod = gsn_dodge;break;
							case 26: mod = gsn_envenom;break;
							case 27: mod = gsn_kick;break;
							case 28: mod = gsn_parry;break;
							case 29: mod = gsn_rescue;break;
							case 30: mod = gsn_trip;break;
							case 31: mod = gsn_flail_mastery;break;
							case 32: mod = gsn_wardance;break;
							case 33: mod = gsn_mace_mastery;break;
							case 34: mod = gsn_riding;break;
							case 35: mod = gsn_hide;break;
							case 36: mod = gsn_lore;break;
							case 37: mod = gsn_meditation;break;
							case 38: mod = gsn_peek;break;
							case 39: mod = gsn_pick_lock;break;
							case 40: mod = gsn_sneak;break;
							case 41: mod = gsn_steal;break;
							case 42: mod = gsn_charge;break;
							case 43: mod = gsn_recuperate;break;
							case 44: mod = gsn_lay;break;
							case 45: mod = gsn_damage_reduction;break;
							case 46: mod = gsn_critical_strike;break;
							case 47: mod = gsn_two_weapon_fighting;break;
							case 48: mod = gsn_dagger_mastery;break;
							case 49: mod = gsn_twohander_fighting;break;
							case 50: mod = gsn_turn;break;
							case 52: mod = gsn_sword_mastery;break;
							case 53: mod = gsn_axe_mastery;break;
							case 54: mod = gsn_spear_mastery;break;
							case 55: mod = gsn_staff_mastery;break;
							case 56: mod = gsn_whip_mastery;break;
							case 57: mod = gsn_polearm_mastery;break;
							case 58: mod = gsn_skin;break;
							case 59: mod = gsn_bandage;break;
							default: break;
						}
						break;
					case 16:
						elem = number_range( 1, 45 );
						kasa += elem * COST_APPLY_LANG;
						loc = APPLY_LANG;
						mod = number_range( 0, 19 );
						level = elem;
						break;
					case 17:
						elem = number_range( -5, 3 );
						kasa += UMAX( 0, -elem * COST_APPLY_SAVING );
						mod = elem;
						switch ( number_range( 1, 5 ) )
						{
							case 1:
								loc = APPLY_SAVING_SPELL;
								break;
							case 2:
								loc = APPLY_SAVING_ROD;
								break;
							case 3:
								loc = APPLY_SAVING_PETRI;
								break;
							case 4:
								loc = APPLY_SAVING_BREATH;
								break;
							case 5:
								loc = APPLY_SAVING_DEATH;
								break;
						}
						break;
					default:
						elem = number_range( 1, 10 );
						kasa += elem * COST_APPLY_RESIST;
						loc = APPLY_RESIST;
						level = elem;
						switch ( number_range( 1, 15 ) )
						{
							case 1:
								mod = RESIST_FIRE;
								break;
							case 2:
								mod = RESIST_COLD;
								break;
							case 3:
								mod = RESIST_ACID;
								break;
							case 4:
								mod = RESIST_ELECTRICITY;
								break;
							case 5:
								mod = RESIST_NEGATIVE;
								break;
							case 6:
								mod = RESIST_SUMMON;
								break;
							case 7:
								mod = RESIST_MENTAL;
								break;
							case 8:
								mod = RESIST_POISON;
								break;
							case 9:
								mod = RESIST_FEAR;
								break;
							case 10:
								mod = RESIST_CHARM;
								break;
							case 11:
								mod = RESIST_MAGIC;
								level = elem/2;
								break;
							case 12:
								mod = RESIST_BASH;
								level = elem/2;
								break;
							case 13:
								mod = RESIST_SLASH;
								level = elem/2;
								break;
							case 14:
								mod = RESIST_PIERCE;
								level = elem/2;
								break;
							case 15:
								mod = RESIST_HOLY;
								break;
							default:break;
						}
						break;
				}

				add_new_affect( obj, type, loc, mod, level, bitv, generate_aff_duration(points, &kasa), TRUE );
				obj->cost += kasa;
				kasa = 1;
			}
			break;
	}
	obj->cost = UMAX( 1, obj->cost );
	return;
}

BITVECT_DATA * get_bitvector ()
{
	BITVECT_DATA * bitv = &AFF_NONE;

// wypa¶ne afekty pozytywne
	if ( number_range( 1, 10000 ) == 1 )
	{
		switch ( number_range( 1, 18 ) )
		{
			case  1: bitv = &AFF_BRAVE_CLOAK;break;
			case  2: bitv = &AFF_DARK_VISION;break;
			case  3: bitv = &AFF_ENERGY_SHIELD;break;
			case  4: bitv = &AFF_EYES_OF_THE_TORTURER;break;
			case  5: bitv = &AFF_FIRESHIELD;break;
			case  6: bitv = &AFF_FLYING;break;
			case  7: bitv = &AFF_FREE_ACTION;break;
			case  8: bitv = &AFF_GLOBE;break;
			case  9: bitv = &AFF_HASTE;break;
			case 10: bitv = &AFF_HIDE;break;
			case 11: bitv = &AFF_ICESHIELD;break;
			case 12: bitv = &AFF_INVISIBLE;break;
			case 13: bitv = &AFF_MAJOR_GLOBE;break;
			case 14: bitv = &AFF_MINOR_GLOBE;break;
			case 15: bitv = &AFF_REGENERATION;break;
			case 16: bitv = &AFF_SNEAK;break;
			case 17: bitv = &AFF_WATERBREATH;break;
			case 18: bitv = &AFF_WATERWALK;break;
		}
	}
// moocne afekty pozytywne
	if ( number_range( 1, 1000 ) == 1 )
	{
		switch ( number_range( 1, 5 ) )
		{
			case  1: bitv = &AFF_PROTECT_GOOD;break;
			case  2: bitv = &AFF_PROTECT_EVIL;break;
			case  3: bitv = &AFF_FLOAT;break;
			case  4: bitv = &AFF_UNDEAD_INVIS;break;;
			case  5: bitv = &AFF_ANIMAL_INVIS;break;
		}
	}
// s³abe afekty pozytywne oraz afekty negatywne
	else
	{
		switch ( number_range( 1, 200 ) )
		{
// negatywne
			case  1: bitv = &AFF_BLIND;break;
//			case  2: bitv = &AFF_COMPREHEND_LANGUAGES;break;
			case  3: bitv = &AFF_CONFUSION;break;
//			case  4: bitv = &AFF_CONFUSION_SHELL;break;
			case  5: bitv = &AFF_CURSE;break;
			case  6: bitv = &AFF_ENTANGLE;break;
			case  7: bitv = &AFF_PLAGUE;break;
			case  8: bitv = &AFF_POISON;break;
			case  9: bitv = &AFF_SILENCE;break;
			case 10: bitv = &AFF_SLOW;break;
			case 11: bitv = &AFF_WEAKEN;break;
// pozytywne
			case 12: bitv = &AFF_DETECT_AGGRESSIVE;break;
			case 13: bitv = &AFF_DETECT_EVIL;break;
			case 14: bitv = &AFF_DETECT_GOOD;break;
			case 15: bitv = &AFF_DETECT_HIDDEN;break;
			case 16: bitv = &AFF_DETECT_INVIS;break;
			case 17: bitv = &AFF_DETECT_MAGIC;break;
			case 18: bitv = &AFF_DETECT_UNDEAD;break;
			case 19: bitv = &AFF_INFRARED;break;
		}
	}
	return bitv;
}

int get_cost_of_bitvector ( BITVECT_DATA * bitv )
{
	if      ( EXT_IS_EQUAL( *bitv, AFF_ANIMAL_INVIS))         return COST_AFF_INVISIBLE;
	else if ( EXT_IS_EQUAL( *bitv, AFF_BRAVE_CLOAK))          return COST_AFF_BRAVE_CLOAK;
	else if ( EXT_IS_EQUAL( *bitv, AFF_COMPREHEND_LANGUAGES)) return COST_AFF_COMPREHEND_LANGUAGES;
	else if ( EXT_IS_EQUAL( *bitv, AFF_DARK_VISION))          return COST_AFF_DARK_VISION;
	else if ( EXT_IS_EQUAL( *bitv, AFF_DETECT_EVIL))          return COST_AFF_DETECT_ALIGMENT;
	else if ( EXT_IS_EQUAL( *bitv, AFF_DETECT_GOOD))          return COST_AFF_DETECT_ALIGMENT;
	else if ( EXT_IS_EQUAL( *bitv, AFF_DETECT_HIDDEN))        return COST_AFF_DETECT_ALIGMENT;
	else if ( EXT_IS_EQUAL( *bitv, AFF_DETECT_INVIS))         return COST_AFF_DETECT_INVIS;
	else if ( EXT_IS_EQUAL( *bitv, AFF_DETECT_MAGIC))         return COST_AFF_DETECT_ALIGMENT;
	else if ( EXT_IS_EQUAL( *bitv, AFF_DETECT_UNDEAD))        return COST_AFF_DETECT_UNDEAD;
	else if ( EXT_IS_EQUAL( *bitv, AFF_ENERGY_SHIELD))        return COST_AFF_ENERGY_SHIELD;
	else if ( EXT_IS_EQUAL( *bitv, AFF_EYES_OF_THE_TORTURER)) return COST_AFF_EYES_OF_THE_TORTURER;
	else if ( EXT_IS_EQUAL( *bitv, AFF_FLOAT))                return COST_AFF_FLOAT;
	else if ( EXT_IS_EQUAL( *bitv, AFF_FLYING))               return COST_AFF_FLYING;
	else if ( EXT_IS_EQUAL( *bitv, AFF_FREE_ACTION))          return COST_AFF_FREE_ACTION;
	else if ( EXT_IS_EQUAL( *bitv, AFF_HASTE))                return COST_AFF_HASTE;
	else if ( EXT_IS_EQUAL( *bitv, AFF_INFRARED))             return COST_AFF_INFRARED;
	else if ( EXT_IS_EQUAL( *bitv, AFF_INVISIBLE))            return COST_AFF_INVISIBLE;
	else if ( EXT_IS_EQUAL( *bitv, AFF_PROTECT_EVIL))         return COST_AFF_PROTECT_ALIGMENT;
	else if ( EXT_IS_EQUAL( *bitv, AFF_PROTECT_GOOD))         return COST_AFF_PROTECT_ALIGMENT;
	else if ( EXT_IS_EQUAL( *bitv, AFF_REGENERATION))         return COST_AFF_REGENERATION;
	else if ( EXT_IS_EQUAL( *bitv, AFF_UNDEAD_INVIS))         return COST_AFF_INVISIBLE;
	else if ( EXT_IS_EQUAL( *bitv, AFF_WATERBREATH))          return COST_AFF_WATERBREATH;
	else if ( EXT_IS_EQUAL( *bitv, AFF_WATERWALK))            return COST_AFF_WATERWALK;

	else return 0;
}

int generate_aff_duration( int *points, int *kasa )
{
    int dur, chance;

    /* sprawdzenie czy *points ma sensown± warto¶æ */
    if ( (*points < 1) || (*points > 7) )
    {
        bugf("generate_aff_duration: Z³a warto¶æ argumentu points: %d", *points);
        return 24; // zwracamy minimaln± sensown± warto¶æ
    }

    //pierw osobno rzucana szansa na stalke w procentach
    switch ( *points )
    {
        case 1: chance = 1;break;
        case 2: chance = 1;break;
        case 3: chance = 2;break;
        case 4: chance = 2;break;
        case 5: chance = 3;break;
        case 6: chance = 3;break;
        case 7: chance = 4;break;
    }

    /**
     * szansa na staly affekt
     */
    if ( number_percent() < chance )
    {
        return -1;
    }

    //pozniej korzystajac z podwojonych szans z pierwszego ifa szansa na dosc dluga czasowke
    if ( number_percent() < chance )
    {
        chance = 25600;
    }
    else
    {
        switch ( *points )
        {
            case 7:  chance = 12800; break;
            case 6:  chance =  6400; break;
            case 5:  chance =  3200; break;
            case 4:  chance =  1600; break;
            case 3:  chance =   800; break;
            case 2:  chance =   400; break;
            case 1:  chance =   200; break;
            default: chance =   100; break;
        }
    }
    dur = number_range( 1, chance );

    if ( dur < 100 )
        *kasa /= number_range( 8, 12 );
    else if ( dur < 200 )
        *kasa /= number_range( 6, 8 );
    else if ( dur < 400 )
        *kasa /= number_range( 4, 6 );
    else if ( dur < 600 )
        *kasa /= number_range( 3, 4 );
    else if ( dur < 800 )
        *kasa /= 3;
    else if ( dur < 1200 )
        *kasa /= 2;
    else if ( dur < 1600 )
        *kasa /= 1.5;
    else
        *kasa /= 1.2;

    return dur;
}

