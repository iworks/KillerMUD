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
 * random_spellbooks.c and associated patches copyright 2009 by Gurthg *
 *                                                                     *
 * In order to use any part of this ROM Merc Diku code you must comply *
 * the original Diku license in 'license.doc' as well the Merc license *
 * in 'license.txt' and also the ROM license in 'rom.license', each to *
 * be found in doc/. Using the reward.c code without conforming to the *
 * requirements of each of these documents is violation of any and all *
 * applicable copyright laws. In particular, you may not remove any of *
 * these copyright notices or claim other's work as your own.          *
 *                                                                     *
 * Much time and thought has gone into this software you are using.    *
 * We hope that you share your improvements, too.                      *
 * "What goes around, comes around."                                   *
 *                                                                     *
 ***********************************************************************
 *                                                                     *
 * KILLER MUD is copyright 2009-2012 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * ZMIENIA£E¦ CO¦? DOPISZ SIÊ!                                         *
 *                                                                     *
 * Pietrzak Marcin       (marcin@iworks.pl                ) [Gurthg  ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: random_spellbooks.c 10990 2012-02-18 12:11:01Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/random_spellbooks.c $
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
#include "money.h"

OBJ_DATA * generate_random_spellbook( CHAR_DATA *ch, int counter )
{
    char text[ MAX_INPUT_LENGTH ];
    char *name, *name2, *name3, *name4, *name5, *name6;
    char *adjective, *subjective;
    int value_number = 2, level, class, points, points_top, points_used = 0, spell_counter = 0, adjective_kind;
    static OBJ_DATA *spellbook;
    OBJ_DATA * return_spellbook;
    if ( !spellbook )
    {
        spellbook = create_object( get_obj_index( OBJ_VNUM_RANDOM_ITEM ), FALSE );
    }
    /*
     * dziêkujemy
     *  - istotom przywo³anym
     *  - graczom
     *  - animalom
     */
    if ( EXT_IS_SET( ch->act, ACT_NO_EXP ) || !IS_NPC( ch ) || IS_SET( race_table[ GET_RACE( ch ) ].type , ANIMAL ) )
    {
        return NULL;
    }
    /*
     * losowanie klasy dla której zostanie stworzona ksiêga
     * 1/9 - paladyn
     * 2/9 - kleryk, druid
     * 4/9 - mag
     * 0/9 - bard (brak w case)
     */
    switch ( number_range( 0, 9 ) )
    {
        case 0:
            class = CLASS_PALADIN;
            break;
        case 1:
        case 2:
            class = CLASS_CLERIC;
            break;
        case 3:
        case 4:
            class = CLASS_DRUID;
            break;
        default:
            class = CLASS_MAG;
            break;
    }
    /*
     * szansa ³adowania siê, minimum 1 prom
     * maskymalnie to level moba jako promile,
     * czyli mob 40 poziom daje 4% szansy
     */
    int chance = UMAX( 1, ch->level );
    /*
     * zwiekszenie szansy ³adowania siê ksiêgi dla mobów posiadaj±cych
     * rêce
     */
    if ( IS_SET( ch->parts, PART_HANDS ) )
    {
        chance += 10;
    }
    /*
     * podwajamy szanse ladowania dla bosów
     */
    if ( EXT_IS_SET( ch->act, ACT_BOSS ) )
    {
        chance *= 2;
    }
    /*
     * zwiekszamy szansê ³adowania siê ksiêgi je¿eli
     * mob jest tej samej klasy co rodzaj ksiegi
     */
    if (
            ( class == CLASS_BARD    && EXT_IS_SET( ch->act, ACT_BARD    ) ) ||
            ( class == CLASS_CLERIC  && EXT_IS_SET( ch->act, ACT_CLERIC  ) ) ||
            ( class == CLASS_DRUID   && EXT_IS_SET( ch->act, ACT_DRUID   ) ) ||
            ( class == CLASS_MAG     && EXT_IS_SET( ch->act, ACT_MAGE    ) ) ||
            ( class == CLASS_PALADIN && EXT_IS_SET( ch->act, ACT_PALADIN ) )
       )
    {
        chance += 20;
    }
    /*
     * maksymalna szansa dla moba 40 poziom, boss, posiadaj±cy rêce
     * caster w danym rodzaju to 12%, bo
     * chance = ( 40 + 10 ) * 2 + 20 = 120
     */
    if ( number_range( 0, 1000 ) > chance )
    {
        return NULL;
    }
    /**
     * dla kolejnych wywolan w petli zmniejszamy szanse na wystapienie
     * ksiegi:
     * 1/2
     * 1/3
     * 1/4
     * etc. etc.
     */
    if ( counter > 1 && number_range( 1, counter ) != 1 )
    {
        return NULL;
    }
    /*
     * wyliczenie poziom trudnosci ksiegi
     */
    level = number_range( 0, UMAX( 1, ch->level/4 ) );
    /*
     * dla mobow o wyzszym poziomie, dodatkowa szasa na podniesienie
     * poziomu ksieg, a co za tym idzie rowniez poziomu czarow
     */
    if ( ch->level > number_range( 20, 39 ) )
    {
        level += number_range( 0, UMAX( 1, ch->level/10 ) );
    }
    /*
     * wyliczenie liczby punktów od poziomu trudno¶ci
     */
    points = ( 1 + level ) * ch->level;
    if ( EXT_IS_SET( ch->act, ACT_BOSS ) )
    {
        points *= 2;
    }
    points_top = points;
    /*
     * zwiekszam poziom ksiêgi oraz liczbê punktów je¿eli
     * mobik jest casterm tego rodzaju co losowana ksiêga
     */
    if (
            ( class == CLASS_BARD    && EXT_IS_SET( ch->act, ACT_BARD    ) ) ||
            ( class == CLASS_CLERIC  && EXT_IS_SET( ch->act, ACT_CLERIC  ) ) ||
            ( class == CLASS_DRUID   && EXT_IS_SET( ch->act, ACT_DRUID   ) ) ||
            ( class == CLASS_MAG     && EXT_IS_SET( ch->act, ACT_MAGE    ) ) ||
            ( class == CLASS_PALADIN && EXT_IS_SET( ch->act, ACT_PALADIN ) )
       )
    {
        level++;
        points *= 3;
        points /= 2;
    }
    /*
     * wyzerowanie czarów
     */
    spellbook->value[ 2 ] =
        spellbook->value[ 3 ] =
        spellbook->value[ 4 ] =
        spellbook->value[ 5 ] =
        spellbook->value[ 6 ] = 0;
    /*
     * petla losujaca czary do ksiegi, zabezpieczeniem jest ograniczenie
     * losowania do 50-ciu iteracji, jak sie nie uda to ksiega bedzie
     * pusta
     */
    while ( points > 0 && value_number < 7 )
    {
        /*
         * losujemy sobie sn , czyli to jaka wlascieiw umiejetnoscia
         * bedziemy sie w danej iteracji zajmowac
         */
        int sn = number_range( 1, MAX_SKILL );
        /*
         * wylaczamy niektore czary, z roznych przyczyn
         * selekcji dokonal Mistrz Drake
         */
        if (
                sn ==  18 || // continual light
                sn ==  44 || // energy drain
                sn ==  55 || // spiritual hammer
                sn ==  82 || // recharge
                sn ==  97 || // word of recall
                sn ==  98 || // finger of death
                sn ==  99 || // elemental devastation
                sn == 100 || // frost breath
                sn == 101 || // gas breath
                sn == 102 || // lightning breath
                sn == 133 || // summon animal
                sn == 167 || // strength
                sn == 192 || // bladethirst
                sn == 229 || // feeblemind
                sn == 396 || // psionic blast
                sn == 398 || // wood master
                sn == 411 || // singing ruff
                sn == 453 || // shield of nature
                sn == 397 || // astral journey
                (
                 class == CLASS_PALADIN &&
                 (
                  sn ==  85 || // sanctuary
                  sn == 231    // divine power
                 )
                )
                )
                {
                    continue;
                }
        /*
         * wylacz to co jest umiejka nie czarem poprzez sprawdzenie
         * czy wywolywana funkcja jest "spell_null", czyli skill
         */
        if ( skill_table[ sn ].spell_fun != spell_null )
        {
            /*
             * skill level - czyli poziom czaru dla danej profesji
             */
            int sl = skill_table[ sn ].skill_level[ class ];
            /*
             * sl - czyli skill level dla danej umiejetnosci i danej klasy
             * miesci sie w widelkach 1-31
             */
            if ( sl > 0 && sl < 32 )
            {
                /*
                 * dla paladyna obnizamy wymagania, bo koles ma pierwszy krag
                 * na 12-tym poziomie, ale odjecia 12-tu powodowalo, ze ksiegi
                 * pala byly za latwe
                 */
                if ( class == CLASS_PALADIN )
                {
                    sl -= 8;
                }
                /*
                 * wyliczenie kosztu jednostkowego czaru, jak widac, dla
                 * 9 kregu czaro maga (level 25) koszt jednostkowy czaru to
                 * okolo 208 punktow
                 */
                int cost = UMAX( 1, sl * sl / 3 );
                /*
                 * zabezpieczenie antyzawieszeniowe, bo w warunkach
                 * bardzo granicznych mozna nie wyjsc z petli do usranej
                 * smierci
                 */
                if ( cost - 1 > points && points_used == 0 )
                {
                    return NULL;
                }
                /*
                 * if sprawdza czy:
                 *
                 * czy jest wystarczajaca liczba punktow (points) na to, zeby
                 * obsluzyc koszt umieszczenia danego czaru
                 *
                 * oraz ... szansa
                 */
                if ( points > cost && number_percent() > 50 )
                {
                    spellbook->value[ value_number ] = sn;
                    value_number++;
                    points -= cost;
                    points_used += cost;
                    spell_counter++;
                }
            }
            /*
             * je¿eli juz wylosowali¶my choæ jeden czar i podbijemy
             * value_number to mozemy ominaæ dany value[3-6] w celu
             * zostawienia go w spokoju i nie przydzielania czaru
             *
             * dodatkowo zmniejszamy pule dostêpnych punktów o 25%
             */
            else if ( value_number != 2 && number_percent() < 5 )
            {
                value_number++;
                points *= 3;
                points /= 4;
            }
        }
    }
    /*
     * je¿eli nie zosta³ zapisany ¿aden czar, to nie dajemy ksiegi
     */
    if ( spell_counter == 0 )
    {
        return NULL;
    }
    /*
     * ustawianie klasy i poziomu trudno¶ci
     */
    spellbook->value[ 0 ] = class;
    /*
     * normalizacja poziom trudnosci ksiegi
     */
    spellbook->value[ 1 ] = URANGE( SPELLBOOK_LEVEL_MIN, level, SPELLBOOK_LEVEL_MAX );
    /*
     * pozosta³e standardowe w³asno¶ci dla ksiêgi
     */
    spellbook->timer = number_percent() * number_range( 1, 20 ) * level;
    spellbook->condition = dice(25,4);
    spellbook->weight = 5 + number_range( level, 3 * level) ;
    spellbook->wear_flags = ITEM_TAKE | ITEM_HOLD;
    spellbook->item_type = ITEM_SPELLBOOK;
    spellbook->liczba_mnoga = 0; // pojedyñcza
    /*
     * material
     */
    switch ( number_range( 0, 20 ) )
    {
        case 0:
            spellbook->material =  6;//skóra
            break;
        case 1:
            spellbook->material = 38;//pergamin
            break;
        case 2:
            spellbook->material = 44;//mocna skóra
            break;
        case 3:
            spellbook->material = 45;//eteryczny
            break;
        default:
            spellbook->material = 20;//papier
            break;
    }
    /*
     * policzmy sobie koszt ksiegi
     */
    spellbook->cost = number_range( 1, level )  * 1000;
    spellbook->cost -= number_range( 1, spellbook->cost / number_range( 10, 20 ) );
    spellbook->cost = UMAX( 100, spellbook->cost );
    /*
     * zwiekszam cene o wartosci punktowe
     */
    spellbook->cost += points_top;
    /*
     * normalizujemy cene wzgledem srebra
     */
    spellbook->cost *= RATTING_COPPER;
    /*
     * ustawiam koszt rentu na pi±t± czê¶æ ceny, tak
     * ¿eby op³aca³o siê je troszkê rentowaæ
     */
    spellbook->rent_cost = 2 * RENT_COST( spellbook->cost );
    /*
     * losujemy gender, potem nazwe
     */
    switch ( number_range( 1, 3 ) )
    {
        case GENDER_NIJAKI:
            spellbook->gender = GENDER_NIJAKI;
            switch ( number_range ( 0, 1 ) )
            {
                case 0:
                    name  = "dzie³o";
                    name2 = "dzie³u";
                    name3 = "dzie³a";
                    name4 = "dzie³o";
                    name5 = "dzie³em";
                    name6 = "dzie³e";
                    break;
                case 1:
                    name  = "cymelium";
                    name2 = "cymelium";
                    name3 = "cymelium";
                    name4 = "cymelium";
                    name5 = "cymelium";
                    name6 = "cymelium";
                    break;
            }
            break;
        case GENDER_MESKI:
            spellbook->gender = GENDER_MESKI;
            switch ( number_range ( 0, 6 ) )
            {
                case 0:
                    name  = "tom";
                    name2 = "tomu";
                    name3 = "tomowi";
                    name4 = "tom";
                    name5 = "tomem";
                    name6 = "tomie";
                    break;
                case 1:
                    name  = "inkunabu³";
                    name2 = "inkunabu³u";
                    name3 = "inkunabu³owi";
                    name4 = "inkunabu³";
                    name5 = "inkunabu³em";
                    name6 = "inkunabule";
                    break;
                case 2:
                    name  = "wolumen";
                    name2 = "wolumenu";
                    name3 = "wolumenowi";
                    name4 = "wolumen";
                    name5 = "wolumenem";
                    name6 = "wolumenie";
                    break;
                case 3:
                    name  = "wolumin";
                    name2 = "woluminu";
                    name3 = "woluminowi";
                    name4 = "wolumin";
                    name5 = "woluminem";
                    name6 = "woluminie";
                    break;
                case 4:
                    name  = "folia³";
                    name2 = "folia³u";
                    name3 = "folia³owi";
                    name4 = "folia³";
                    name5 = "folia³em";
                    name6 = "foliale";
                    break;
                case 5:
                    name  = "foliant";
                    name2 = "foliantu";
                    name3 = "foliantowi";
                    name4 = "foliant";
                    name5 = "foliantem";
                    name6 = "foliancie";
                    break;
                case 6:
                    name  = "rêkopis";
                    name2 = "rêkopisu";
                    name3 = "rêkopisowi";
                    name4 = "rêkopis";
                    name5 = "rêkopisem";
                    name6 = "rêkopisie";
                    break;
            }
            break;
        case GENDER_ZENSKI:
            spellbook->gender = GENDER_ZENSKI;
            switch ( number_range ( 0, 1 ) )
            {
                case 0:
                    name  = "ksiêga";
                    name2 = "ksiêgi";
                    name3 = "ksiêdze";
                    name4 = "ksiêgê";
                    name5 = "ksiêg±";
                    name6 = "ksiêdze";
                    break;
                case 1:
                    name  = "ksi±¿ka";
                    name2 = "ksi±¿ki";
                    name3 = "ksi±¿ce";
                    name4 = "ksi±¿kê";
                    name5 = "ksi±¿k±";
                    name6 = "ksi±¿ce";
                    break;
            }
            break;
    }
    /*
     * deklinacja przymiotnika
     */
    static char* declination[4][2][7] =
    {
        { // fake, ¿eby nie trzeba by³o inkrementowaæ rodzaju (spellbook->gender)
            { "", "", "", "", "", "", "" },
            { "", "", "", "", "", "", "" }
        },
        { // nijaki
            // masywne cymelium (masywn)
            {  "", "e",  "ego",  "emu",  "e", "ym", "ym" },
            // wielkie cymelium (wielk)
            { "", "ie", "iego", "iemu", "ie", "im", "im" }
        },
        { // mêski
            // masywny tom (masywn)
            { "", "y",  "ego", "emu",  "y", "ym", "ym" },
            // wielki tom (wielk)
            { "", "i", "iego", "iemu", "i", "im", "im" }
        },
        { // ¿enski
            // masywna ksiêga (masywn)
            { "", "a",  "ej",  "ej", "a", "±",  "ej" },
            // wielka ksiega (wielk)
            { "", "a", "iej", "iej", "±", "±", "iej" }
        }
    };
    /*
     * losowanie rodzaju przymiotnika
     *
     * 0 - przymiotniki koñcz±ce siê na "y"
     * 1 - przymiotniki koñcz±ce siê na "i"
     *
     */
    adjective_kind = number_range ( 0, 1 );
    /*
     * lozowanie przymiotnika
     */
    if ( points_top > number_range( 100, 200 ) )
    {
        switch ( adjective_kind )
        {
            case 0:
                switch ( number_range( 0, 20 ) )
                {
                    case 0:
                        adjective = "du¿";
                        break;
                    case 1:
                        adjective = "masywn";
                        break;
                    case 2:
                        adjective = "kolosaln";
                        break;
                    case 3:
                        adjective = "ogromn";
                        break;
                    case 4:
                        adjective = "potê¿n";
                        break;
                    case 5:
                        adjective = "spor";
                        break;
                    case 6:
                        adjective = "wielgachn";
                        break;
                    case 7:
                        adjective = "znaczn";
                        break;
                    case 8:
                        adjective = "gigantyczn";
                        break;
                    case 9:
                        adjective = "gargantuiczn";
                        break;
                    case 10:
                        adjective = "tward";
                        break;
                    case 11:
                        adjective = "poka¼n";
                        break;
                    case 12:
                        adjective = "monumentaln";
                        break;
                    case 13:
                        adjective = "now";
                        break;
                    case 14:
                        adjective = "nieu¿ywan";
                        break;
                    case 15:
                        adjective = "idealn";
                        break;
                    case 16:
                        adjective = "okut";
                        break;
                    case 17:
                        adjective = "zdobion";
                        break;
                    case 18:
                        adjective = "wzmocnion";
                        break;
                    case 19:
                        adjective = "b³yszcz±c";
                        break;
                    case 20:
                        adjective = "l¶ni±c";
                        break;
                }
                break;
            case 1:
                switch ( number_range( 0, 3 ) )
                {
                    case 0:
                        adjective = "wielk";
                        break;
                    case 1:
                        adjective = "szerok";
                        break;
                    case 2:
                        adjective = "têg";
                        break;
                    case 3:
                        adjective = "nowiusieñk";
                        break;
                }
        }
    }
    else
    {
        spellbook->cost /= 2;
        switch ( adjective_kind )
        {
            case 0:
                switch ( number_range( 0, 22 ) )
                {
                    case 0:
                        adjective = "ma³";
                        spellbook->weight /= 3;
                        break;
                    case 1:
                        adjective = "delikatn";
                        break;
                    case 2:
                        adjective = "filigranow";
                        spellbook->weight /= 3;
                        break;
                    case 3:
                        adjective = "niedu¿";
                        spellbook->weight /= 3;
                        break;
                    case 4:
                        adjective = "ma³";
                        spellbook->weight /= 3;
                        break;
                    case 5:
                        adjective = "drobn";
                        spellbook->weight /= 3;
                        break;
                    case 6:
                        adjective = "niepoka¼n";
                        spellbook->weight /= 3;
                        break;
                    case 7:
                        adjective = "kieszonkow";
                        spellbook->weight /= 3;
                        break;
                    case 8:
                        adjective = "star";
                        break;
                    case 9:
                        adjective = "u¿ywan";
                        break;
                    case 10:
                        adjective = "zniszczon";
                        break;
                    case 11:
                        adjective = "sfatygowan";
                        break;
                    case 12:
                        adjective = "zdezelowan";
                        break;
                    case 13:
                        adjective = "z³achmanion";
                        break;
                    case 14:
                        adjective = "skomkan";
                        break;
                    case 15:
                        adjective = "wys³u¿on";
                        break;
                    case 16:
                        adjective = "podniszczon";
                        break;
                    case 17:
                        adjective = "zeszmacon";
                        break;
                    case 18:
                        adjective = "wytart";
                        break;
                    case 19:
                        adjective = "zmaltretowan";
                        break;
                    case 20:
                        adjective = "sparcia³";
                        break;
                    case 21:
                        adjective = "za¶niedzia³";
                        break;
                    case 22:
                        adjective = "przetart";
                        break;
                }
                break;
            case 1:
                spellbook->weight /= 2;
                switch ( number_range( 0, 8 ) )
                {
                    case 0:
                        adjective = "lekk";
                        break;
                    case 1:
                        adjective = "niewielk";
                        break;
                    case 2:
                        adjective = "malutk";
                        break;
                    case 3:
                        adjective = "w±sk";
                        break;
                    case 4:
                        adjective = "lilipuc";
                        break;
                    case 5:
                        adjective = "tyc";
                        break;
                    case 6:
                        adjective = "cienk";
                        break;
                    case 7:
                        adjective = "miêkk";
                        break;
                    case 8:
                        adjective = "p³ask";
                        break;
                }
                break;
        }
    }
    /*
     * ustawianie nazw
     *
     * mozliwe parametry do u¿ycia
     *
     * class         - dla jakiej profesji
     * level         - poziom trudno¶ci ksiegi czarów (wy¿szy, trudniejsze)
     * spell_counter - liczba czarów
     * points_used   - liczba u¿ytych punktów (im wiêcej tym lepsze czary)
     */
    switch ( class )
    {
        case CLASS_MAG:
            switch ( number_range( 0, 17 ) )
            {
                case 0:
                    subjective = "magii";
                    break;
                case 1:
                    subjective = "czarów";
                    break;
                case 2:
                    subjective = "dymu";
                    break;
                case 3:
                    spellbook->weight /= 2;
                    subjective = "kurzu";
                    break;
                case 4:
                    subjective = "sztuczek";
                    break;
                case 5:
                    subjective = "trików";
                    break;
                case 6:
                    subjective = "gestów";
                    break;
                case 7:
                    subjective = "mocy";
                    break;
                case 8:
                    subjective = "woli";
                    break;
                case 9:
                    subjective = "inteligencji";
                    break;
                case 10:
                    subjective = "wiedzy";
                    break;
                case 11:
                    spellbook->weight *= 2;
                    subjective = "potêgi";
                    break;
                case 12:
                    subjective = "z pentagramem na ok³adce";
                    EXT_SET_BIT( spellbook->extra_flags, ITEM_EVIL );
                    break;
                case 13:
                    subjective = "z rubinami";
                    spellbook->cost += number_range( 1000, 1500 );
                    break;
                case 14:
                    subjective = "z szafirami";
                    spellbook->cost += number_range( 1000, 1500 );
                    break;
                case 15:
                    subjective = "z topazami";
                    spellbook->cost += number_range( 1000, 1500 );
                    break;
                case 16:
                    subjective = "z granatami";
                    spellbook->cost += number_range( 1000, 1500 );
                    break;
                case 17:
                    subjective = "z czerwon± wst±¿eczk±";
                    break;
            }
            break;
        case CLASS_CLERIC:
            switch ( number_range( 0, 11 ) )
            {
                case 0:
                    subjective = "b³ogos³awieñstwa";
                    break;
                case 1:
                    subjective = "modlitwy";
                    break;
                case 2:
                    subjective = "Zapomnianego Boga";
                    break;
                case 3:
                    subjective = "wiary";
                    break;
                case 4:
                    subjective = "przekleñstwa";
                    break;
                case 5:
                    subjective = "zaufania";
                    break;
                case 6:
                    subjective = "medytacji";
                    break;
                case 7:
                    subjective = "skupienia";
                    break;
                case 8:
                    subjective = "sakramentu";
                    break;
                case 9:
                    subjective = "przebaczenia";
                    break;
                case 10:
                    subjective = "uzdrowienia";
                    break;
                case 11:
                    subjective = "odpuszczenia";
                    break;
            }
            break;
        case CLASS_PALADIN:
            spellbook->weight += number_range( 5, 20 );
            switch ( number_range( 0, 10 ) )
            {
                case 0:
                    subjective = "miecza";
                    break;
                case 1:
                    subjective = "tarczy";
                    break;
                case 2:
                    subjective = "buzdyganu";
                    break;
                case 3:
                    subjective = "w³óczni";
                    break;
                case 4:
                    subjective = "si³y";
                    break;
                case 5:
                    subjective = "Portena";
                    break;
                case 6:
                    subjective = "walki";
                    break;
                case 7:
                    subjective = "parady";
                    break;
                case 8:
                    subjective = "satysfakcji";
                    break;
                case 9:
                    subjective = "triumfu";
                    break;
                case 10:
                    subjective = "zwyciêstwa";
                    break;
            }
            break;
        case CLASS_DRUID:
            switch ( number_range( 0, 12 ) )
            {
                case 0:
                    subjective = "lasu";
                    break;
                case 1:
                    subjective = "puszczy";
                    break;
                case 2:
                    subjective = "ro¶lin";
                    break;
                case 3:
                    subjective = "kwiatów";
                    break;
                case 4:
                    subjective = "zwierz±t";
                    break;
                case 5:
                    spellbook->weight /= 3;
                    subjective = "owadów";
                    break;
                case 6:
                    subjective = "oceanów";
                    break;
                case 7:
                    subjective = "rzek";
                    break;
                case 8:
                    spellbook->weight *= 2;
                    subjective = "górskich szczytów";
                    break;
                case 9:
                    subjective = "piasku";
                    break;
                case 10:
                    subjective = "bagna";
                    spellbook->cost /= 2;
                    break;
                case 11:
                    subjective = "piargów";
                    break;
                case 12:
                    subjective = "wiatru";
                    break;
            }
            break;
        case CLASS_BARD: // wy³±czone w switch
            switch ( number_range( 0, 5 ) )
            {
                case 0:
                    subjective = "muzyki";
                    break;
                case 1:
                    subjective = "dzwiêków";
                    break;
                case 2:
                    subjective = "ciszy";
                    break;
                case 3:
                    subjective = "rytmów";
                    break;
                case 4:
                    subjective = "dysonansu";
                    break;
                case 5:
                    subjective = "harmonii";
                    break;
            }
            break;
        default:
            return NULL;
    }
    /*
     * losowanie w³asno¶ci specjalnych
     *
     * na sta³e ustawiam ITEM_MAGIC oraz ITEM_NOLOCATE
     */
    EXT_SET_BIT( spellbook->extra_flags, ITEM_MAGIC );
    EXT_SET_BIT( spellbook->extra_flags, ITEM_NOLOCATE );
    if ( level < 3 && number_percent() < 50 )
    {
        EXT_SET_BIT( spellbook->extra_flags, ITEM_NEWBIE_FREE_RENT );
    }
    switch ( number_percent() )
    {
        case 1:
            switch ( number_range( 0, 1 ) )
            {
                case 0:
                    EXT_SET_BIT( spellbook->extra_flags, ITEM_GLOW );
                    break;
                case 1:
                    EXT_SET_BIT( spellbook->extra_flags, ITEM_DARK );
                    break;
            }
            break;
        case 2:
            EXT_SET_BIT( spellbook->extra_flags, ITEM_BURN_PROOF );
            spellbook->weight *= 2;
            break;
        case 3:
            EXT_SET_BIT( spellbook->extra_flags, ITEM_UNDESTRUCTABLE );
            spellbook->weight *= 2;
            break;
        case 4:
            EXT_SET_BIT( spellbook->extra_flags, ITEM_INVIS );
            spellbook->weight /= 2;
            break;
        case 5:
            EXT_SET_BIT( spellbook->extra_flags, ITEM_NODROP );
            break;
        case 6:
            EXT_SET_BIT( spellbook->extra_flags, ITEM_NOREMOVE );
            break;
        case 7:
            EXT_SET_BIT( spellbook->extra_flags, ITEM_UNDEAD_INVIS );
            break;
        case 8:
            EXT_SET_BIT( spellbook->extra_flags, ITEM_ANIMAL_INVIS );
            break;
        case 9:
            EXT_SET_BIT( spellbook->extra_flags, ITEM_NO_RENT );
            break;
        case 10:
            EXT_SET_BIT( spellbook->extra_flags, ITEM_HIGH_RENT );
            break;
        case 11:
            EXT_SET_BIT( spellbook->extra_flags, ITEM_FREE_RENT );
            break;
        default:
            break;
    }
    /*
     * przypisz nazwê
     */
    free_string( spellbook->short_descr );
    free_string( spellbook->name );
    free_string( spellbook->description );

    sprintf ( text, "%s%s %s %s", adjective, declination[ spellbook->gender][ adjective_kind ][1], name, subjective );
    spellbook->name = str_dup( text );
    spellbook->short_descr = str_dup( text );

    sprintf( text, "%s le¿y tutaj.", capitalize( text ) );
    spellbook->description = str_dup( text );

    free_string( spellbook->name2 );
    sprintf ( text, "%s%s %s %s", adjective, declination[ spellbook->gender][ adjective_kind ][2], name2, subjective );
    spellbook->name2 = str_dup( text);

    free_string( spellbook->name3 );
    sprintf ( text, "%s%s %s %s", adjective, declination[ spellbook->gender][ adjective_kind ][3], name3, subjective );
    spellbook->name3 = str_dup( text);

    free_string( spellbook->name4 );
    sprintf ( text, "%s%s %s %s", adjective, declination[ spellbook->gender][ adjective_kind ][4], name4, subjective );
    spellbook->name4 = str_dup( text);

    free_string( spellbook->name5 );
    sprintf ( text, "%s%s %s %s", adjective, declination[ spellbook->gender][ adjective_kind ][5], name5, subjective );
    spellbook->name5 = str_dup( text);

    free_string( spellbook->name6 );
    sprintf ( text, "%s%s %s %s", adjective, declination[ spellbook->gender][ adjective_kind ][6], name6, subjective );
    spellbook->name6 = str_dup( text);

    /*
     * debug
     */
    sprintf
        (
         log_buf,
         "[LOAD SPELLBOOK] %s [ %d / %d / %d / %d ] %s (%d), %s (%d), %s (%d), %s (%d), %s (%d) [mob: %s (%d) ] %s",
         class_table[class].name,
         level,
         points_top,
         points,
         counter,
         skill_table[ spellbook->value[2] ].name,
         skill_table[ spellbook->value[2] ].skill_level[ class ],
         skill_table[ spellbook->value[3] ].name,
         skill_table[ spellbook->value[3] ].skill_level[ class ],
         skill_table[ spellbook->value[4] ].name,
         skill_table[ spellbook->value[4] ].skill_level[ class ],
         skill_table[ spellbook->value[5] ].name,
         skill_table[ spellbook->value[5] ].skill_level[ class ],
         skill_table[ spellbook->value[6] ].name,
         skill_table[ spellbook->value[6] ].skill_level[ class ],
         ch->short_descr,
         ch->level,
         spellbook->name
        );
    wiznet( log_buf, NULL, NULL, WIZ_ARTEFACT, 0, 39 );
    log_string( log_buf );
    /**
     * * sprawdzenie wagi
     */
    spellbook->weight = UMAX( number_range( 1, 3 ), spellbook->weight );
    /*
     * a tutaj myk aby nam nie ucieka³a pamiêæ
     */
    return_spellbook = spellbook;
    spellbook = NULL;
    /*
     * oddaj mi wredny mobie moj± ksi±¿eczkê
     */
    return return_spellbook;
}

