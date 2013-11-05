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
 * $Id: magic.c 11016 2012-02-23 12:22:02Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/magic.c $
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
#include <ctype.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "projects.h"
#include "money.h"

//funkcje z innych plikow
extern void affect_modify( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd );
/*
 * Local functions.
 */
void say_spell                      args( ( CHAR_DATA *ch, int sn ) );
bool check_improve_strenth          args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool verbose ) );
bool check_shaman_invoke            args( ( CHAR_DATA *ch, int sn ) );
void remove_memorized_spell         args( ( CHAR_DATA *ch, int sn ) );
int  helper_do_cast_wait_wand_bonus args( ( CHAR_DATA *ch, int sn ) );

/* imported functions */
void mp_onload_trigger	args( ( CHAR_DATA *mob ) );
bool    remove_obj      args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
void 	wear_obj	args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
bool 	check_pwait	args( (CHAR_DATA *ch,int sn) );
void 	setup_pwait	args( (CHAR_DATA *ch,char *spell,void *voo,int target, int sn) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool 	check_reflect_spell	args( (CHAR_DATA *victim, sh_int circle) );
bool 	check_globes	args( (CHAR_DATA *victim, sh_int circle) );
bool	check_blink	args( (CHAR_DATA *victim, CHAR_DATA *ch, bool unik) );
void	mem_update_count args( ( CHAR_DATA *ch ) );

CHAR_DATA *get_char_area( CHAR_DATA *ch, char *argument );
sh_int get_caster(CHAR_DATA *ch);

/*1,10,10,10,9, 9,9,7,7,7, 6,6,6,5,5, 5,4,4,4,2, 2,2   1,2,3*/
int saves_table[MAX_CLASS][5][31] =
{
 { /* trucizna, smiertelna magia */
   /* berlo, laski, rozdzka */
   /* polimorfia petryfikacja */
   /* zioniecie */
   /* czar */

  /* MAGE */
  {-1, 14,14,14,14,14, 13,13,13,13,13, 12,12,12,12,12, 11,11,11,11,11, 10,10,10,10,10, 9,9,9,9,9},
  {-1, 11,11,11,11,11, 10,10,10,10,10,  9, 9, 9, 9, 9,  8, 8, 8, 8, 8,  7, 7, 7, 7, 7, 6,6,6,6,6},
  {-1, 13,13,13,13,13, 12,12,12,12,12, 11,11,11,11,11, 10,10,10,10,10,  9, 9, 9, 9, 9, 8,8,8,8,8},
  {-1, 15,15,15,15,15, 14,14,14,14,14, 13,13,13,13,13, 12,12,12,12,12,  11,11,11,11,11, 10,10,10,10,10},
  {-1, 12,12,12,12,12, 11,11,11,11,11, 10,10,10,10,10,  9, 9, 9, 9, 9,  8, 8, 8, 8, 8,  7, 7, 7, 7, 7}
 },

 { /* CLERIC */
  {-1, 10,10,10,10,10,  9, 9, 9, 9, 9,  8, 8, 8, 8, 8,   7, 7, 6, 6, 6,  5, 5, 4, 4, 4,  3, 2, 2, 2, 2},
  {-1, 14,14,14,14,14, 13,13,13,13,13,  12,12,12,12,12,  11,11,10,10,10, 9, 9, 8, 8, 7,  7, 6, 6, 6, 6},
  {-1, 13,13,13,13,13, 12,12,12,12,12,  11,11,11,11,11,  10,10,10,10,10, 9, 9, 9, 8, 8,  8, 7, 7, 7, 5},
  {-1, 16,16,16,16,16, 15,15,15,14,14,  14,13,13,13,13,  12,12,12,12,12,11,11,11,10,10, 10, 9, 9, 9, 8},
  {-1, 15,15,15,15,15, 14,14,14,13,13,  13,12,12,12,12,  11,11,11,10,10,10,10,10, 9, 9,  9, 8, 8, 8, 7}
 },

 { /* THIEF */
  {-1, 13,13,13,13,13, 12,12,12,12,12,  11,11,11,11,11, 10,10,10,10,10,  9, 9, 9, 9, 9,  8, 8, 8, 8, 8},
  {-1, 14,14,14,14,14, 13,13,13,13,13,  12,12,12,12,12, 11,11,11,11,11, 10,10,10, 9, 9,  9, 8, 8, 7, 6},
  {-1, 12,12,12,12,12, 11,11,11,11,11,  10,10,10,10,10,  9, 9, 9, 9, 9,  8, 8, 8, 8, 7,  7, 7, 7, 7, 7},
  {-1, 16,16,16,16,16, 15,15,15,15,15,  14,14,14,14,14, 13,13,13,13,13, 12,12,12,12,12, 11,11,11,11,11},
  {-1, 15,15,15,15,15, 14,14,14,14,14,  13,13,13,13,13, 12,12,12,12,12, 11,11,11,11,11,  9, 9, 9, 9, 9}
 },

 {/* WARRIOR */
  {-1, 14,14,14,13,13, 12,12,12,11,11, 10,10,10, 9, 9,  8, 8, 8, 7, 7,   6, 6, 6, 5, 5,  4, 4, 4, 3, 3},
  {-1, 16,16,15,15,15, 14,14,13,12,12, 11,11,10,10, 9,  9, 9, 8, 8, 7,   7, 7, 6, 6, 5,  5, 5, 4, 4, 4},
  {-1, 15,15,15,14,14, 13,13,12,12,12, 11,11,10,10, 9,  9, 9, 8, 8, 7,   7, 6, 6, 6, 5,  5, 5, 4, 4, 4},
  {-1, 17,17,17,16,16, 15,15,14,14,13, 13,13,12,12,11, 11,10,10, 8, 8,   8, 7, 7, 6, 6,  5, 5, 4, 4, 4},
  {-1, 17,17,17,16,16, 15,15,15,14,14, 13,13,13,12,12, 11,11,11,10, 10,  9, 9, 9, 8, 8,  7, 7, 7, 6, 6}
 },

/* PALADIN */
{
	{-1, 14,14,14,13,13, 12,12,12,11,11, 10,10,10, 9, 9,  8, 8, 8, 7, 7,   6, 6, 6, 5, 5,  4, 4, 4, 3, 3},
	{-1, 16,16,15,15,15, 14,14,13,12,12, 11,11,10,10, 9,  9, 9, 8, 8, 7,   7, 7, 6, 6, 5,  5, 5, 4, 4, 4},
	{-1, 15,15,15,14,14, 13,13,12,12,12, 11,11,10,10, 9,  9, 9, 8, 8, 7,   7, 6, 6, 6, 5,  5, 5, 4, 4, 4},
	{-1, 17,17,17,16,16, 15,15,14,14,13, 13,13,12,12,11, 11,10,10, 8, 8,   8, 7, 7, 6, 6,  5, 5, 4, 4, 4},
	{-1, 17,17,17,16,16, 15,15,15,14,14, 13,13,13,12,12, 11,11,11,10, 10,  9, 9, 9, 8, 8,  7, 7, 7, 6, 6}
},

/* DRUID */
{
	{-1, 10,10,10,10,10,  9, 9, 9, 9, 9,  8, 8, 8, 8, 8,   7, 7, 6, 6, 6,  5, 5, 4, 4, 4,  3, 2, 2, 2, 2},
	{-1, 14,14,14,14,14, 13,13,13,13,13,  12,12,12,12,12,  11,11,10,10,10, 9, 9, 8, 8, 7,  7, 6, 6, 6, 6},
	{-1, 13,13,13,13,13, 12,12,12,12,12,  11,11,11,11,11,  10,10,10,10,10, 9, 9, 9, 8, 8,  8, 7, 7, 7, 5},
	{-1, 16,16,16,16,16, 15,15,15,14,14,  14,13,13,13,13,  12,12,12,12,12,11,11,11,10,10, 10, 9, 9, 9, 8},
	{-1, 15,15,15,15,15, 14,14,14,13,13,  13,12,12,12,12,  11,11,11,10,10,10,10,10, 9, 9,  9, 8, 8, 8, 7}
},

/* BARBARIAN */
{
	{-1, 10,10,10,10,10,  9, 9, 9, 9, 9,  8, 8, 8, 8, 8,   7, 7, 6, 6, 6,  5, 5, 4, 4, 4,  3, 2, 2, 2, 2},
	{-1, 16,16,15,15,15, 14,14,13,12,12, 11,11,10,10, 9,  9, 9, 8, 8, 7,   7, 7, 6, 6, 5,  5, 5, 4, 4, 4},
	{-1, 15,15,15,14,14, 13,13,12,12,12, 11,11,10,10, 9,  9, 9, 8, 8, 7,   7, 6, 6, 6, 5,  5, 5, 4, 4, 4},
	{-1, 17,17,17,16,16, 15,15,14,14,13, 13,13,12,12,11, 11,10,10, 8, 8,   8, 7, 7, 6, 6,  5, 5, 4, 4, 4},
	{-1, 18,18,18,17,17, 16,16,16,15,15, 14,14,14,13,13, 12,12,12,11,11,  10,10,10, 9, 9,  8, 8, 8, 7, 7}
},

/* MONK - uwaga, tylko kopia z wojownika*/
{
	{-1, 14,14,14,13,13, 12,12,12,11,11, 10,10,10, 9, 9,  8, 8, 8, 7, 7,   6, 6, 6, 5, 5,  4, 4, 4, 3, 3},
	{-1, 16,16,15,15,15, 14,14,13,12,12, 11,11,10,10, 9,  9, 9, 8, 8, 7,   7, 7, 6, 6, 5,  5, 5, 4, 4, 4},
	{-1, 15,15,15,14,14, 13,13,12,12,12, 11,11,10,10, 9,  9, 9, 8, 8, 7,   7, 6, 6, 6, 5,  5, 5, 4, 4, 4},
	{-1, 17,17,17,16,16, 15,15,14,14,13, 13,13,12,12,11, 11,10,10, 8, 8,   8, 7, 7, 6, 6,  5, 5, 4, 4, 4},
	{-1, 17,17,17,16,16, 15,15,15,14,14, 13,13,13,12,12, 11,11,11,10, 10,  9, 9, 9, 8, 8,  7, 7, 7, 6, 6}
},

{ /* BARD - tak samo jak thief */
  {-1, 13,13,13,13,13, 12,12,12,12,12,  11,11,11,11,11, 10,10,10,10,10,  9, 9, 9, 9, 9,  8, 8, 8, 8, 8},
  {-1, 14,14,14,14,14, 13,13,13,13,13,  12,12,12,12,12, 11,11,11,11,11, 10,10,10, 9, 9,  9, 8, 8, 7, 6},
  {-1, 12,12,12,12,12, 11,11,11,11,11,  10,10,10,10,10,  9, 9, 9, 9, 9,  8, 8, 8, 8, 7,  7, 7, 7, 7, 7},
  {-1, 16,16,16,16,16, 15,15,15,15,15,  14,14,14,14,14, 13,13,13,13,13, 12,12,12,12,12, 11,11,11,11,11},
  {-1, 15,15,15,15,15, 14,14,14,14,14,  13,13,13,13,13, 12,12,12,12,12, 11,11,11,11,11,  9, 9, 9, 9, 9}
},

/* BLACK KNIGHT - podobnie jak paladyn, tylko sejwy na death jak barb (lepsze) */
{
	{-1, 10,10,10,10,10,  9, 9, 9, 9, 9,  8, 8, 8, 8, 8,   7, 7, 6, 6, 6,  5, 5, 4, 4, 4,  3, 2, 2, 2, 2},
	{-1, 16,16,15,15,15, 14,14,13,12,12, 11,11,10,10, 9,  9, 9, 8, 8, 7,   7, 7, 6, 6, 5,  5, 5, 4, 4, 4},
	{-1, 15,15,15,14,14, 13,13,12,12,12, 11,11,10,10, 9,  9, 9, 8, 8, 7,   7, 6, 6, 6, 5,  5, 5, 4, 4, 4},
	{-1, 17,17,17,16,16, 15,15,14,14,13, 13,13,12,12,11, 11,10,10, 8, 8,   8, 7, 7, 6, 6,  5, 5, 4, 4, 4},
	{-1, 17,17,17,16,16, 15,15,15,14,14, 13,13,13,12,12, 11,11,11,10, 10,  9, 9, 9, 8, 8,  7, 7, 7, 6, 6}
},

/* SZAMAN - rzuty obronne jak barbarzyñca */
{
	{-1, 10,10,10,10,10,  9, 9, 9, 9, 9,  8, 8, 8, 8, 8,   7, 7, 6, 6, 6,  5, 5, 4, 4, 4,  3, 2, 2, 2, 2},
	{-1, 16,16,15,15,15, 14,14,13,12,12, 11,11,10,10, 9,  9, 9, 8, 8, 7,   7, 7, 6, 6, 5,  5, 5, 4, 4, 4},
	{-1, 15,15,15,14,14, 13,13,12,12,12, 11,11,10,10, 9,  9, 9, 8, 8, 7,   7, 6, 6, 6, 5,  5, 5, 4, 4, 4},
	{-1, 17,17,17,16,16, 15,15,14,14,13, 13,13,12,12,11, 11,10,10, 8, 8,   8, 7, 7, 6, 6,  5, 5, 4, 4, 4},
	{-1, 18,18,18,17,17, 16,16,16,15,15, 14,14,14,13,13, 12,12,12,11,11,  10,10,10, 9, 9,  8, 8, 8, 7, 7}
},


};



//rellik: components, sprawdza czy ch zna ju¿ wszystkie mo¿liwe components do sn
bool spell_items_know_all( CHAR_DATA *ch, int sn )
{
  int i;
  for( i=0 ; spell_items_table[i].key_number != 0; i++ )
  {
    //je¶li komponent odnosi sie do naszego sn i nie zna tego nasz ch
    if ( spell_items_table[i].spell_number == sn
      && !chk_bigflag( &ch->pcdata->spell_items_knowledge, spell_items_table[i].key_number ) )
      {
        //to znaczy ze nie zna wszystkich
        return FALSE;
      }
  }
  return TRUE;
}

//rellik: components, uczy ch losowego komponentu dot. sn
void spell_item_get_knowledge( CHAR_DATA *ch, int sn )
{
  int i, counter = 0;
  int slots[MAX_COMPO_GROUP_SIZE]; //lamerska pomocnicza tabelka;
	char wpis_do_loga [MAX_STRING_LENGTH];

  for( i=0 ; spell_items_table[i].key_number != 0; ++i )
  {
    //je¶li komponent odnosi sie do naszego sn i nie zna tego nasz ch
    if ( spell_items_table[i].spell_number == sn
      && !chk_bigflag( &ch->pcdata->spell_items_knowledge, spell_items_table[i].key_number ) )
      {
        //to znaczy ze mo¿e siê go nauczyæ
        ++counter;
        slots[counter] = i;
      }
  }

  if ( counter == 0 )
  {
    save_debug_info( "magic.c => spell_item_get_knowledge", NULL, "komponent do sn nie znaleziony lub zna wszystkie", DEBUG_PROJECT_SPELL_ITEMS, DEBUG_LEVEL_ALL, FALSE );
    return;
  }

  if ( counter >= MAX_COMPO_GROUP_SIZE )
  {
    save_debug_info( "magic.c => spell_item_get_knowledge", NULL, "mo¿e znaæ zbyt du¿o komponentów", DEBUG_PROJECT_SPELL_ITEMS, DEBUG_LEVEL_ALL, FALSE );
    return;
  }

  //no to losujemy który komponent
  i = slots[ number_range( 1, counter ) ];
  //i uczymy go gracza
  set_bigflag( &ch->pcdata->spell_items_knowledge, spell_items_table[i].key_number );
  print_char( ch, "Dowiadujesz siê, ¿e komponent %s mo¿e mieæ wp³yw na rzucanie czaru %s.\n\r", spell_items_table[i].official_spell_item_name, skill_table[sn].name);

	sprintf(wpis_do_loga,"%s dowiedzia³ siê o komponencie %s do czaru %s", ch->name, spell_items_table[i].official_spell_item_name, skill_table[sn].name );
	log_string( wpis_do_loga );


  if ( spell_items_table[i].group_nb ) print_char( ch, "Ten komponent wydaje siê byæ sk³adnikiem wiêkszej grupy." );
  act( "Widzisz nag³y b³ysk w oku $z.", ch, NULL, NULL, TO_ROOM );
  print_char( ch, "\n\r" );
}

/*  STUDY
 * komenda sluzy do uczenia siê zaklec z ksiazek
 */
void do_study( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *book;
    OBJ_DATA *obj;
    int chance, chance_mod, counter, rand = -1, rand2 = -1;
    int spells[5], spells_count;
    extern int port;
    char wpis_do_loga [MAX_STRING_LENGTH];
    sh_int sn;
    char arg[ MAX_INPUT_LENGTH ];

    /**
     * moby sie zaklec raczej nie ucza
     */
    if(IS_NPC(ch))
    {
        return;
    }
    /**
     * klasy nieczarujace tez nie bardzo
     */
    if( class_table[ch->class].caster < 0 )
    {
        send_to_char("Studia? A co to takiego?\n\r",ch);
        return;
    }
    argument = one_argument( argument, arg );
    /**
     * nie wiadomo co chcemy studiowac
     */
    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "A co chcesz studiowaæ?\n\r", ch );
        return;
    }
    if ( ( book = get_obj_carry( ch, argument, ch ) ) == NULL )
    {
        send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
        return;
    }
    /**
     * sprawdzenie typu przedmiotu, ciezko studiowac lyzke
     * ale mozna ja obejrzec.
     */
    if(!book || book->item_type != ITEM_SPELLBOOK)
    {
        do_function( ch, &do_examine, arg );
        return;
    }
    /**
     * jak nie pasuje profesja
     */
    if(book->value[0] != ch->class)
    {
        send_to_char("Nie jeste¶ w stanie nic zrozumieæ.\n\r",ch);
        return;
    }
    /* 0 daje +25 do szansy, 10 daje -25 do szansy */
    chance_mod = URANGE( SPELLBOOK_LEVEL_MIN, book->value[1], SPELLBOOK_LEVEL_MAX );
    chance_mod -= 5;
    chance_mod *= -5;

    /* wyzerowanie tablicy zaklec */
    for(counter = 0; counter < 5 ; counter++)
    {
        spells[counter] = 0;
    }
    /* wczytanie zaklec do tablicy zaklec */
    for(spells_count = 0, counter = 2; counter < 7; counter ++)
    {
        if( book->value[counter] > 0 && book->value[counter] < MAX_SKILL && skill_table[book->value[counter]].spell_fun != spell_null)
        {
            spells[spells_count] = book->value[counter];
            spells_count++;
        }
    }

    if (is_affected (ch, gsn_energy_drain))
    {
        print_char(ch,"Z uwaga przygl±dasz siê %s kartkuj±c kolejno strony.", book->name3);
        print_char(ch,"Nie udaje ci siê jednak znale¼æ niczego warto¶ciowego b±d¼ czego¶ nowego.");
        print_char(ch,"Po chwili %s rozb³yskuje jasnym ¶wiat³em i zamienia siê w kupkê popio³u.\n\r", book->short_descr);

        // ³adowanie popio³u
        obj = create_object( get_obj_index( OBJ_VNUM_ASH ), FALSE );
        obj->timer = UMAX(3,number_percent()/10);
        obj_to_room( obj, ch->in_room );

        if(book->wear_loc != WEAR_NONE)
        {
            unequip_char( ch, book );
        }
        /*artefact*/
        if( is_artefact(book) && !IS_NPC(ch) && !IS_IMMORTAL(ch) )
        {
            artefact_to_char( book, ch);
        }
        obj_from_char( book );
        if( is_artefact(book) )
        {
            extract_artefact(book);
        }
        extract_obj( book );
        WAIT_STATE(ch,12);
        return;
    }

    /* no dobra nie bylo zaklec, dowidzenia */
    if( spells_count == 0 )
    {
        print_char(ch,"Z uwaga przygl±dasz siê %s kartkuj±c kolejno strony.", book->name3);
        print_char(ch,"Nie udaje ci siê jednak znale¼æ niczego warto¶ciowego b±d¼ czego¶ nowego.");
        print_char(ch,"Po chwili %s rozb³yskuje jasnym ¶wiat³em i zamienia siê w kupkê popio³u.\n\r", book->short_descr);

        // ³adowanie popio³u
        obj = create_object( get_obj_index( OBJ_VNUM_ASH ), FALSE );
        obj->timer = UMAX(3,number_percent()/10);
        obj_to_room( obj, ch->in_room );

        sprintf(wpis_do_loga,"%s study %s[%d], no new spells.", ch->name, book->short_descr, book->pIndexData->vnum );
        log_string( wpis_do_loga );

        if(book->wear_loc != WEAR_NONE)
        {
            unequip_char( ch, book );
        }

        /*artefact*/
        if( is_artefact(book) && !IS_NPC(ch) && !IS_IMMORTAL(ch) )
        {
            artefact_to_char( book, ch);
        }

        obj_from_char( book );

        /*artefact*/
        if( is_artefact(book) )
        {
            extract_artefact(book);
        }
        extract_obj( book );

        WAIT_STATE(ch,12);
        return;
    }

    /* losowanie zaklêcia z tablicy */
    if(spells_count == 1)
    {
        sn = spells[0];
    }
    else
    {
        sn = spells[number_range(0,spells_count-1)];
    }

    /* bazowy chance : polowa learn z inta */
    chance = chance_to_learn_spell(ch, sn);

    /* minus od chance_mod*2*/
    chance = URANGE(5, chance + chance_mod, 95);

    /* roznica poziomow */
    if(skill_table[sn].skill_level[ch->class] > ch->level)
    {
        chance -= 2*(skill_table[sn].skill_level[ch->class] - ch->level);
    }
    if(ch->class == CLASS_MAG )
    {
        if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
        {
            ;
        }
        else
        {
            chance -= UMAX(1,chance/5);
        }
    }

    //mistrz poznania ma jeszcze premie
    if( ch->class == CLASS_MAG && ch->pcdata->mage_specialist == 3 )
    {
        chance += chance/4;
    }
    chance = UMAX(chance,1);
    /* zna juz? no to dowidzenia, to samo specjalisci */
    //rellik: components, czy zna wszystkie mo¿liwe components do zaklêcia?
    if( ( ch->pcdata->learned[sn] > 0 && spell_items_know_all( ch, sn ) ) || !can_learn_spell(ch, sn))
    {
        print_char(ch,"Z uwaga przyglad±sz siê %s kartkuj±c kolejno strony. ", book->name3);
        print_char(ch,"Nie udaje ci siê jednak znale¼æ niczego warto¶ciowego b±d¼ czego¶ nowego. ");
        print_char(ch,"Po chwili %s rozb³yskuje jasnym ¶wiat³em i zamienia siê w kupkê popio³u.\n\r", book->short_descr);

        // ³adowanie popio³u
        obj = create_object( get_obj_index( OBJ_VNUM_ASH ), FALSE );
        obj->timer = UMAX(3,number_percent()/10);
        obj_to_room( obj, ch->in_room );

        sprintf(wpis_do_loga,"%s study %s[%d], already known, or specialization conflict.", ch->name, book->short_descr, book->pIndexData->vnum );
        log_string( wpis_do_loga );

        if(book->wear_loc != WEAR_NONE)
        {
            unequip_char( ch, book );
        }
        /*artefact*/
        if( is_artefact(book) && !IS_NPC(ch) && !IS_IMMORTAL(ch) )
        {
            artefact_to_char( book, ch);
        }
        obj_from_char( book );
        /*artefact*/
        if( is_artefact(book) )
        {
            extract_artefact(book);
        }
        extract_obj( book );
        WAIT_STATE(ch,12);
        return;
    }

    //rellik: components, ok, umie ju¿ zaklêcie to dajmy mu siê komponentów nauczyæ
    if ( ch->pcdata->learned[sn] > 0 )
    {
        if ( ( rand = number_percent() ) < chance || ( rand2 = number_percent() ) < chance )
        {
            spell_item_get_knowledge( ch, sn ); //losuje wiedzê o komponencie dla gracza dot. sn
        }
        else
        {
            print_char(ch,"Przegl±dasz %s zafascynowan<&y/a/e> ogromem nowej wiedzy. ", book->name4);
            print_char(ch,"Probujesz co¶ zrozumieæ, jednak po chwili zdajesz sobie sprawe, ¿e ci siê to raczej nie uda. ");
            print_char(ch,"Docierasz do po³owy, gdy %s rozb³yskuje i zamienia siê w kupkê popio³u.\n\r", book->short_descr);

            // ³adowanie popio³u
            obj = create_object( get_obj_index( OBJ_VNUM_ASH ), FALSE );
            obj->timer = UMAX(3,number_percent()/10);
            obj_to_room( obj, ch->in_room );
        }
        if(book->wear_loc != WEAR_NONE) unequip_char( ch, book );
        /*artefact*/
        if( is_artefact(book) && !IS_NPC(ch) && !IS_IMMORTAL(ch) ) artefact_to_char( book, ch);
        obj_from_char( book );
        /*artefact*/
        if( is_artefact(book) ) extract_artefact(book);
        extract_obj( book );
        WAIT_STATE(ch,12);
        return;
    }

    /* za cienki ¿eby siê nauczyc? papa */
    if( skill_table[sn].skill_level[ch->class] > (ch->level + 5) || skill_table[sn].skill_level[ch->class] >= 32 )
    {
        print_char(ch,"Przegl±dasz %s zafascynowan<&y/a/e> ogromem nowej wiedzy. ", book->name4);
        print_char(ch,"Probujesz co¶ zrozumieæ, jednak po chwili zdajesz sobie sprawe, ¿e ci siê to raczej nie uda. ");
        print_char(ch,"Docierasz do po³owy, gdy %s rozb³yskuje i zamienia siê w kupkê popio³u.\n\r", book->short_descr);

        // ³adowanie popio³u
        obj = create_object( get_obj_index( OBJ_VNUM_ASH ), FALSE );
        obj->timer = UMAX(3,number_percent()/10);
        obj_to_room( obj, ch->in_room );

        sprintf(wpis_do_loga,"%s study %s[%d], level too low.", ch->name, book->short_descr, book->pIndexData->vnum );
        log_string( wpis_do_loga );

        if(book->wear_loc != WEAR_NONE)
            unequip_char( ch, book );

        /*artefact*/
        if( is_artefact(book) && !IS_NPC(ch) && !IS_IMMORTAL(ch) )
            artefact_to_char( book, ch);
        obj_from_char( book );
        /*artefact*/
        if( is_artefact(book) ) extract_artefact(book);
        extract_obj( book );
        WAIT_STATE(ch,12);
        return;
    }

    if ( ( rand = number_percent() ) < chance )
    {
        print_char(ch,"Przegl±dasz %s zafascynowan<&y/a/e> ilo¶ci± nowej wiedzy. ", book->name4);
        print_char(ch,"Starasz siê opanowaæ dr¿enie r±k przewracaj±c kolejne strony. ");
        print_char(ch,"Wszystko uk³ada siê w logiczn± ca³o¶æ, nie masz najmniejszych problemów z przyswojeniem nowych informacji. ");
        print_char(ch,"Kiedy docierasz do koñca, %s rozpada siê w twoich d³oniach i znika.\n\r", book->short_descr);

        switch(class_table[ch->class].caster)
        {
            case CLASS_CLERIC:
            case CLASS_PALADIN:
                print_char(ch,"Przepisujesz zaklêcie '%s' do swojego modlitewnika.\n\r", skill_table[sn].name);
                break;
            case CLASS_MAG:
            case CLASS_BARD:
                print_char(ch,"Przepisujesz zaklêcie '%s' do swojej ksiêgi czarów.\n\r", skill_table[sn].name);
                break;
            case CLASS_DRUID:
                print_char(ch,"Przepisujesz zaklêcie '%s' do swojej ksiêgi zaklêæ.\n\r", skill_table[sn].name);
                break;
            case CLASS_SHAMAN:
                print_char(ch,"Zapamiêtujesz wezwanie '%s'.\n\r", skill_table[sn].name);
                break;
        }

        sprintf(wpis_do_loga,"%s study %s[%d], learned '%s'.", ch->name, book->short_descr, book->pIndexData->vnum, skill_table[sn].name );
        log_string( wpis_do_loga );

        ch->pcdata->learned[sn]=1;

        //rellik: components, nauczy³ siê czaru to ma jeszcze ma³± szansê przy okazji komponent obryæ
        if ( number_percent() < chance && number_percent() < chance )
        {
            spell_item_get_knowledge( ch, sn ); //losuje wiedzê o komponencie dla gracza dot. sn
        }
    }
    else
    {
        print_char(ch,"Przegl±dasz %s zafascynowan<&y/a/e> ilo¶ci± nowej wiedzy. ", book->name4);
        print_char(ch,"Starasz siê opanowaæ dr¿enie r±k przewracaj±c kolejne strony. ");
        print_char(ch,"Wszystko uk³ada siê w logiczna ca³o¶æ, nie masz najmniejszych problemow z przyswojeniem nowych informacji. ");
        print_char(ch,"Nagle zdajesz sobie sprawê, ¿e co¶ jest jednak nie tak. ");
        print_char(ch,"Starasz sobie przypomnieæ informacje na pocz±tkowych stronach, gor±czkowo kartkujesz w poszukiwaniu cennej informacji. ");
        print_char(ch,"Niestety, zanim uda³o ci znale¼æ brakuj±ce informacje, %s rozpada siê na kawa³eczki i znika.\n\r", book->short_descr);

        sprintf(wpis_do_loga,"%s study %s[%d], failed.", ch->name, book->short_descr, book->pIndexData->vnum);
        log_string( wpis_do_loga );
    }

    if(book->wear_loc != WEAR_NONE) unequip_char( ch, book );

    /*artefact*/
    if( is_artefact(book) && !IS_NPC(ch) && !IS_IMMORTAL(ch) ) artefact_to_char( book, ch);
    obj_from_char( book );
    /*artefact*/
    if( is_artefact(book) ) extract_artefact(book);
    extract_obj( book );
    WAIT_STATE(ch,12);
    return;
}

/*
 * Lookup a skill by name.
 */
int skill_lookup( const char *name )
{
    int sn;

    for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	if ( skill_table[sn].name == NULL )
	    break;
	if ( NOPOL(name[0]) == NOPOL(skill_table[sn].name[0])
	&&   !str_prefix( name, skill_table[sn].name ) )
	    return sn;
	}

    return -1;
}

int spell_only_lookup( const char *name )
{
    int sn;

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
        if ( skill_table[sn].name == NULL )
            break;
        if ( NOPOL( name[0] ) == NOPOL( skill_table[sn].name[0] )
        &&   !str_prefix( name, skill_table[sn].name )
        &&   skill_table[sn].spell_fun != spell_null )
        return sn;
    }

    return -1;
}

int skill_only_lookup( const char *name )
{
    int sn;

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
        if ( skill_table[sn].name == NULL )
            break;
        if ( NOPOL( name[0] ) == NOPOL( skill_table[sn].name[0] )
        &&   !str_prefix( name, skill_table[sn].name )
        &&   skill_table[sn].spell_fun == spell_null )
        return sn;
    }

    return -1;
}

bool is_skill_lookup( const char *name )
{
	int sn;
	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
		if ( skill_table[sn].name == NULL )
			break;
		if ( NOPOL(name[0]) == NOPOL(skill_table[sn].name[0]))
			if( !str_cmp( name, skill_table[sn].name ) )
				if (skill_table[sn].spell_fun==spell_null)
					return TRUE;
	}
	return FALSE;
}

int find_skill( CHAR_DATA *ch, const char *name, bool must_know )
{
	/* finds a spell the character can cast if possible */
	int sn, found = -1;

	if ( IS_NPC( ch ) )
		return skill_lookup( name );

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
		if ( skill_table[sn].name == NULL )
			break;

		if ( NOPOL( name[0] ) == NOPOL( skill_table[sn].name[0] )
		&&  !str_prefix( name,skill_table[sn].name ) )
		{
			if ( found == -1 )
				found = sn;

		if ( !must_know && ch->level >= skill_table[sn].skill_level[ch->class] )
			return sn;

	    if ( must_know && ch->level >= skill_table[sn].skill_level[ch->class] && ch->pcdata->learned[sn] > 0 )
		    return sn;
		}
	}
	return found;
}

int find_spell_new( CHAR_DATA *ch, char *name, bool must_know )
{
    int sn, found = -1;
    char arg[2][MAX_INPUT_LENGTH], *pnt, *tmp;


    arg[0][0]='\0';
    arg[1][0]='\0';

    pnt=name;
    pnt=one_argument(pnt, arg[0]);
    pnt=one_argument(pnt, arg[1]);

    if(arg[0][0]=='\0')
	return found;

    for ( sn = 0; sn < MAX_SKILL; sn++ )
	{

	if (skill_table[sn].name == NULL)
	    continue;

	if (skill_table[sn].spell_fun == spell_null)
	    continue;

	if (NOPOL(arg[0][0]) != NOPOL(skill_table[sn].name[0]))
	    continue;

	if(str_prefix(arg[0],skill_table[sn].name))
	    continue;


	  if(arg[1][0] != '\0')
	  {
	   tmp=skill_table[sn].name;

	   while(!isspace( *tmp) && *tmp!='\0')
	       tmp++;

	   if(*tmp=='\0')
	       continue;

	   tmp++;

	   if(*tmp=='\0' || str_prefix(arg[1],tmp))
	       continue;

		if(pnt[0] != '\0')
	        {
	         while(!isspace( *tmp) && *tmp!='\0')
	         tmp++;

		   if(*tmp=='\0')
		       continue;

		   tmp++;

		   if(*tmp=='\0' || str_prefix(pnt,tmp))
		       continue;
	        }
	  }


	    if ( found == -1)
		found = sn;

		if ( IS_NPC( ch ) )
			return sn;

		if ( !must_know && ch->level >= skill_table[sn].skill_level[ch->class] )
			return sn;

	    if ( must_know && ch->level >= skill_table[sn].skill_level[ch->class] && ch->pcdata->learned[sn] > 0 )
		    return sn;
	}
    return found;
}

/*
 * Lookup a skill by slot number.
 * Used for object loading.
 */

const char *spell_translate( char *source )
{
	static char buf  [MAX_STRING_LENGTH];
	char *pName;
	int iSyl;
	int length;

	struct syl_type
	{
		char *	old;
		char *	new;
	};

	static const struct syl_type syl_table[] =
	{
			{ " ",		" "		},
			{ "ar",		"abra"		},
			{ "au",		"kada"		},
			{ "bless",	"fido"		},
			{ "blind",	"nose"		},
			{ "bur",	"mosa"		},
			{ "cu",		"judi"		},
			{ "de",		"oculo"		},
			{ "en",		"unso"		},
			{ "light",	"dies"		},
			{ "lo",		"hi"		},
			{ "mor",	"zak"		},
			{ "move",	"sido"		},
			{ "ness",	"lacri"		},
			{ "ning",	"illa"		},
			{ "per",	"duda"		},
			{ "ra",		"gru"		},
			{ "fresh",	"ima"		},
			{ "re",		"candus"	},
			{ "son",	"sabru"		},
			{ "tect",	"infra"		},
			{ "tri",	"cula"		},
			{ "ven",	"nofo"		},
			{ "a", "a" }, { "b", "b" }, { "c", "q" }, { "d", "e" },
			{ "e", "z" }, { "f", "y" }, { "g", "o" }, { "h", "p" },
			{ "i", "u" }, { "j", "y" }, { "k", "t" }, { "l", "r" },
			{ "m", "w" }, { "n", "i" }, { "o", "a" }, { "p", "s" },
			{ "q", "d" }, { "r", "f" }, { "s", "g" }, { "t", "h" },
			{ "u", "j" }, { "v", "z" }, { "w", "x" }, { "x", "n" },
			{ "y", "l" }, { "z", "k" },
			{ "", "" }
	};

	buf[0]	= '\0';
	for ( pName = source; *pName != '\0'; pName += length )
	{
		for ( iSyl = 0; (length = strlen(syl_table[iSyl].old)) != 0; iSyl++ )
		{
			if ( !str_prefix( syl_table[iSyl].old, pName ) )
			{
				strcat( buf, syl_table[iSyl].new );
				break;
			}
		}

		if ( length == 0 )
			length = 1;
	}
	return buf;
}

/*
 * Utter mystical words for an sn.
 */
void say_spell( CHAR_DATA *ch, int sn )
{
	char buf  [MAX_STRING_LENGTH];
	char buf2 [MAX_STRING_LENGTH];
	CHAR_DATA *rch;

	sprintf( buf2, "$n wymawia s³owa, '%s'.", spell_translate( skill_table[sn].name ) );
	sprintf( buf,  "$n wymawia s³owa, '%s'.", skill_table[sn].name );
	print_char(ch, "Wymawiasz s³owa, '%s'.\n\r", skill_table[sn].name );

	for ( rch = ch->in_room->people; rch; rch = rch->next_in_room )
	{
		if ( rch != ch )
		{
			if ( IS_AFFECTED( rch, AFF_DEAFNESS) )
			{
				act("$n wykonuje jakie¶ dziwne gesty i porusza wargami.", ch, NULL, rch, TO_VICT );
			}
			else
			{
				act((!IS_NPC(rch) && knows_spell(rch, sn)) ? buf : buf2, ch, NULL, rch, TO_VICT );
			}
		}
	}

	return;
}

bool saves_spell_new( CHAR_DATA *ch, int type, sh_int mod, CHAR_DATA *subject, int sn )
{
    int dice;
    int save_value, class, level, save_mod = 0;
    AFFECT_DATA *aura, *mind_fortess, *mind_master;
    CHAR_DATA *mind;
    OBJ_DATA *wand;
    bool wand_first_value = FALSE;

    if ( type < 0 || type > 4 )
        return FALSE;

    /* sprawdzanie odpornosci */
    if ( type != SAVE_DEATH && ch->resists[ RESIST_MAGIC ] > 0 && number_percent() < ch->resists[ RESIST_MAGIC ] )
    {
    	if( subject->level < 30 || !IS_NPC(subject) )
        	return TRUE;//na czary bossow trzeba sie testowac dwa razy
        else if ( number_percent() < ch->resists[ RESIST_MAGIC ] )
        	return TRUE;
	}

	if ( is_affected(ch,gsn_mind_fortess) )
	{
		mind_fortess = affect_find( ch->affected, gsn_mind_fortess );
		if ( mind_fortess->modifier == 1 )
		{
			for ( mind = ch->in_room->people;mind;mind = mind->next_in_room )
			{
				if ( !is_affected( mind, gsn_mind_fortess ) )
					continue;

				mind_master = affect_find( mind->affected, gsn_mind_fortess );
				if ( mind_master->modifier == 2 )
				{
					if ( saves_spell_new( mind, type, mod, subject, sn ) )
					{
						act( "Czujesz, ¿e opiekuj±cy siê tob± umys³ $Z uratowa³ ciê od czego¶.", ch, NULL, mind, TO_CHAR );
						return TRUE;
					}
				}
			}
		}
	}

    while ( ( dice = number_bits( 5 ) ) >= 20 );

    switch ( dice )
    {
        case 0 : return FALSE;
        case 19: return TRUE;
        default: break;
    }

    class = get_class( ch, 3 );

    level = UMIN( 30, ch->level );

    save_value = saves_table[ class ][ type ][ level ];
    save_value -= mod;
    save_value -= ch->saving_throw[ type ];

    if( is_affected(ch, gsn_demon_aura ))
    {
    	aura = affect_find( ch->affected, gsn_demon_aura );
    	save_value -= aura->level/23;
    }

    if ( subject )
    {
	    if ( ch->level > subject->level )
	    	save_value -= URANGE( 1, (ch->level - subject->level)/3, 7 );
	    else if ( ch->level < subject->level )
	    	save_value += URANGE( 1, (subject->level - ch->level)/3, 7 );

		//i wandy
		wand = get_eq_char( subject,WEAR_HOLD );
		if( !IS_NPC(subject) && wand != NULL && wand->item_type == ITEM_WAND )
		{
			if( wand->value[0] == WAND_SAVE_PIERCE )
			{
				wand_first_value = TRUE;
				if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
				{
					if( IS_GOOD( subject ) )
						save_mod += wand->value[1];
					else
						save_mod -= wand->value[1];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
				{
					if( IS_NEUTRAL( subject ) )
						save_mod += wand->value[1];
					else
						save_mod -= wand->value[1];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
				{
					if( IS_EVIL( subject ) )
						save_mod += wand->value[1];
					else
						save_mod -= wand->value[1];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
				{
					if( subject->class == CLASS_MAG )
						save_mod += wand->value[1];
					else
						save_mod -= wand->value[1];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
				{
					if( subject->class == CLASS_CLERIC )
						save_mod += wand->value[1];
					else
						save_mod -= wand->value[1];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
				{
					if( subject->class == CLASS_DRUID )
						save_mod += wand->value[1];
					else
						save_mod -= wand->value[1];
				}
				else
					save_mod += wand->value[1];
			}
			else if( wand->value[2] == WAND_SAVE_PIERCE )
			{
				if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
				{
					if( IS_GOOD( subject ) )
						save_mod += wand->value[3];
					else
						save_mod -= wand->value[3];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
				{
					if( IS_NEUTRAL( subject ) )
						save_mod += wand->value[3];
					else
						save_mod -= wand->value[3];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
				{
					if( IS_EVIL( subject ) )
						save_mod += wand->value[3];
					else
						save_mod -= wand->value[3];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
				{
					if( subject->class == CLASS_MAG )
						save_mod += wand->value[3];
					else
						save_mod -= wand->value[3];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
				{
					if( subject->class == CLASS_CLERIC )
						save_mod += wand->value[3];
					else
						save_mod -= wand->value[3];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
				{
					if( subject->class == CLASS_DRUID )
						save_mod += wand->value[3];
					else
						save_mod -= wand->value[3];
				}
				else
					save_mod += wand->value[3];
			}
			else if( wand->value[4] == WAND_SAVE_PIERCE )
			{
				if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
				{
					if( IS_GOOD( subject ) )
						save_mod += wand->value[5];
					else
						save_mod -= wand->value[5];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
				{
					if( IS_NEUTRAL( subject ) )
						save_mod += wand->value[5];
					else
						save_mod -= wand->value[5];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
				{
					if( IS_EVIL( subject ) )
						save_mod += wand->value[5];
					else
						save_mod -= wand->value[5];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
				{
					if( subject->class == CLASS_MAG )
						save_mod += wand->value[5];
					else
						save_mod -= wand->value[5];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
				{
					if( subject->class == CLASS_CLERIC )
						save_mod += wand->value[5];
					else
						save_mod -= wand->value[5];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
				{
					if( subject->class == CLASS_DRUID )
						save_mod += wand->value[5];
					else
						save_mod -= wand->value[5];
				}
				else
					save_mod += wand->value[5];
			}

			if ( save_mod != 0 && !IS_NPC(subject) )
			{
				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
				 {
				 	if ( !wand_first_value )
				 		save_mod = 0;
				 }

				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
				 {
				 	if ( !wand_first_value )
				 		save_mod = 0;
				 }

				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
				 {
				 	if ( !wand_first_value )
				 		save_mod = 0;
				 }

				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
				 {
				 	if ( !wand_first_value )
				 		save_mod = 0;
				 }

				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
				 {
				 	if ( !wand_first_value )
				 		save_mod = 0;
				 }

				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
				 {
				 	if ( !wand_first_value )
				 		save_mod = 0;
				 }

				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
				 {
				 	if ( !wand_first_value )
				 		save_mod = 0;
				 }

				if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
				 {
					if ( !wand_first_value )
				 		save_mod = 0;
				 }

				if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && !IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
				{
				 	if ( wand_first_value )
				 		save_mod = 0;
				}

				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && !IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
				 {
				 	if ( wand_first_value )
				 		save_mod = 0;
				 }

				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && !IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
				 {
				 	if ( wand_first_value )
				 		save_mod = 0;
				 }

				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && !IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
				 {
				 	if ( wand_first_value )
				 		save_mod = 0;
				 }

				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && !IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
				 {
				 	if ( wand_first_value )
				 		save_mod = 0;
				 }

				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && !IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
				 {
				 	if ( wand_first_value )
				 		save_mod = 0;
				 }

				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && !IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
				 {
				 	if ( wand_first_value )
				 		save_mod = 0;
				 }

				 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && !IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
				 {
				 	if ( wand_first_value )
				 		save_mod = 0;
				 }
			}
		}

		save_value -= save_mod;

	}
    return ( dice > save_value );
}

/*
 * Compute a saving throw.
 * Negative apply's make saving throw better.
 */
bool saves_spell( int level, CHAR_DATA *victim, int dam_type )
{
    int save;

    save = 50 + ( victim->level - level) * 5 - victim->saving_throw[0] * 2;
    if (IS_AFFECTED(victim,AFF_BERSERK))
	save += victim->level/2;

    switch(check_immune(victim,dam_type))
	{
	case IS_IMMUNE:		return TRUE;
	case IS_RESISTANT:	save += 2;	break;
	case IS_VULNERABLE:	save -= 2;	break;
	}

/*    if (!IS_NPC(victim) && class_table[victim->class].fMana)
	save = 9 * save / 10;*/
    save = URANGE( 5, save, 75 );
    return number_percent( ) < save;
}

/* RT save for dispels */

bool saves_dispel( int dis_level, int spell_level, int duration)
{
    int save;

/*    if (duration == -1)
      spell_level += 5;  */

      /* very hard to dispel permanent effects */

    save = 50 + (spell_level - dis_level) * 5;
    save = URANGE( 5, save, 95 );
    return number_percent( ) < save;
}

/* co-routine for dispel magic and cancellation */

bool check_dispel( int dis_level, CHAR_DATA *victim, int sn)
{
    AFFECT_DATA *af;

    if (is_affected(victim, sn))
	{
        for ( af = victim->affected; af != NULL; af = af->next )
        {
            if ( af->type == sn )
            {
                if (!saves_dispel(dis_level,af->level,af->duration))
                {
                    affect_strip(victim,sn);
        	    if ( skill_table[sn].msg_off )
        	    {
            		send_to_char( skill_table[sn].msg_off, victim );
            		send_to_char( "\n\r", victim );
        	    }
		    return TRUE;
		}
		else
		    af->level--;
            }
        }
	}
    return FALSE;
}


/*
 * The kludgy global is for spells who want more stuff from command line.
 */
char *target_name;

void do_cast( CHAR_DATA *ch, char *argument )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim, *sleeping_char;
    OBJ_DATA *obj, *obj_wear = NULL, *wand;
    void *vo;
    int sn, target, iWear;
    int level;
    int position;
    int victim_affects = 0;
    AFFECT_DATA *pAffect;
    //AFFECT_DATA *pHoly;
    int victim_hit = 0, wait = 0;
    bool cast_ok = FALSE, wand_first_value = FALSE, check;
    AFFECT_DATA *kajdany, *ment_bariera;
    int bonus_from_feat, wis_or_int = 0, bonus_from_sector = 0, bonus_from_wand = 0;
    //int pskill = 0;
    //float sskill = 100;

    level = ch->level;
    level -= after_death( ch );

    /* dajemy standardowy tekst jak go¶æ jest pod wp³ywem 'silence' */
    if ( IS_AFFECTED( ch, AFF_SILENCE ) )
    {
        send_to_char( AFF_SILENCE_TEXT, ch );
        return;
    }

    /* ROOM_NOMAGIC */
    if (EXT_IS_SET( ch->in_room->room_flags, ROOM_NOMAGIC ))
    {
        send_to_char( "Nic siê nie udaje, tylko krêci ci siê trochê w g³owie.\n\r", ch );
        return;
    }

    if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ) )
    {
        send_to_char( "Pod wod±? Nie da rady.\n\r", ch );
        switch ( ch->sex )
        {
            case SEX_NEUTRAL:
                act( "$n probuje rzuciæ zaklecie, chmura b±belków powietrza wydobywa siê temu z ust.", ch, NULL, NULL, TO_ROOM );
                break;
            case SEX_MALE:
                act( "$n probuje rzuciæ zaklecie, chmura b±belków powietrza wydobywa siê mu z ust.", ch, NULL, NULL, TO_ROOM );
                break;
            case SEX_FEMALE:
            default:
                act( "$n probuje rzuciæ zaklecie, chmura b±belków powietrza wydobywa siê jej z ust.", ch, NULL, NULL, TO_ROOM );
                break;
        }
        return;
    }

    if ( get_curr_stat( ch, STAT_INT ) < 30 || get_curr_stat( ch, STAT_WIS ) < 30 )
    {
        print_char( ch, "Uhhmm, jak to siê robi³o?!?\n\r" );
        return;
    }

    if ( ch->class == CLASS_DRUID )
    {
        for ( iWear = 0; iWear < MAX_WEAR - 1; iWear++ )
        {
            if ( ( obj_wear = get_eq_char( ch, iWear ) ) == NULL )
                continue;

            if ( ( ( iWear == WEAR_HEAD ) || ( iWear == WEAR_BODY ) ||
                   ( iWear == WEAR_LEGS ) || ( iWear == WEAR_FEET ) ||
                   ( iWear == WEAR_HANDS ) || ( iWear == WEAR_ARMS ) ||
                   ( iWear == WEAR_SHIELD ) ) &&
                 material_table[ obj_wear->material ].flag == MAT_METAL &&
                 ( obj_wear->item_type == ITEM_ARMOR || obj_wear->item_type == ITEM_SHIELD ) )
            {
                send_to_char( "Metalowe czê¶ci zbroi przeszkadzaj± ci w rzucaniu czarów.\n\r", ch );
                return;
            }
        }
    }

    if ( ch->class == CLASS_BARD )
    {
        for ( iWear = 0; iWear < MAX_WEAR - 1; iWear++ )
        {
            if ( ( obj_wear = get_eq_char( ch, iWear ) ) == NULL )
                continue;

            if ( ( ( iWear == WEAR_HEAD ) || ( iWear == WEAR_BODY ) ||
                   ( iWear == WEAR_LEGS ) || ( iWear == WEAR_FEET ) ||
                   ( iWear == WEAR_HANDS ) || ( iWear == WEAR_ARMS ) ||
                   ( iWear == WEAR_SHIELD ) ) && ( obj_wear->item_type == ITEM_ARMOR ||
                   obj_wear->item_type == ITEM_SHIELD ) )
            {
                if( obj_wear->value[0] < 9 ||
                    obj_wear->value[1] < 9 ||
                    obj_wear->value[2] < 9 ||
                    obj_wear->value[3] < 9 )
                {
                    send_to_char( "Twoja zbroja zbytnio ciê uwiera, nie jeste¶ w stanie wykonaæ tak skomplikowanych gestów.\n\r", ch );
                    return;
                }
            }

        }
    }

    target_name = one_argument( argument, arg1 );
    one_argument( target_name, arg2 );

    if ( arg1[ 0 ] == '\0' )
    {
        send_to_char( "Rzuciæ czar, gdzie, jak?\n\r", ch );
        return;
    }

    /* sprawdzenie czy jest takie, czy na pewno zaklecie,
       czy mo¿e znac, i czy zna*/

    if ( ( sn = find_spell_new( ch, arg1, TRUE ) ) < 1
         || skill_table[ sn ].spell_fun == spell_null
         || !knows_spell( ch, sn ) )
    {
        send_to_char( "Nie znasz tego zaklêcia.\n\r", ch );
        return;
    }

    /* czy ma zamemowane */
    if ( IS_NPC( ch ) || ( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) ) )
    {
        if ( !SPELL_MEMMED( ch, sn ) )
        {
            send_to_char( "Nie masz tego zaklêcia zapamiêtanego.\n\r", ch );
            return;
        }
    }

    /* czy jest w odpowiedniej pozycji */
    if ( ch->position == POS_STANDING && ch->fighting )
    {
        position = POS_FIGHTING;
    }
    else
    {
        position = ch->position;
    }

    if ( position < skill_table[ sn ].minimum_position )
    {
        send_to_char( "Nie mo¿esz siê wystarczaj±co skoncentrowaæ.\n\r", ch );
        return;
    }

	if ( is_affected( ch, gsn_mental_barrier ))
    {
    	ment_bariera = affect_find( ch->affected, gsn_mental_barrier );
    	if( sn == ment_bariera->modifier )
		{
        	send_to_char( "Nie mo¿esz sobie przypomnieæ, jak siê rzuca³o to zaklêcie.\n\r", ch );
        	return;
    	}
    }

    /* tu od razu waicik, by pozniej nie zapomniec */
    wait = skill_table[ sn ].beats;

    /*
     * Locate targets.
     */
    victim	= NULL;
    obj	= NULL;
    vo	= NULL;
    target	= TARGET_NONE;

    switch ( skill_table[ sn ].target )
    {
        default:
            bugf( "Do_cast: bad target for spell '%s'[sn=%d].", skill_table[ sn ].name, sn );
            return;

        case TAR_IGNORE:
            if ( arg2[ 0 ] != '\0' )
                vo = ( void * ) str_dup( target_name );
            break;

        case TAR_CHAR_OFFENSIVE:
            if ( arg2[ 0 ] == '\0' )
            {
                if ( ( victim = ch->fighting ) == NULL )
                {
                    /* wymawiamy slowka */
                    say_spell( ch, sn );
                    send_to_char( "Na kogo chcesz rzuciæ ten czar?\n\r", ch );
                    remove_memorized_spell ( ch, sn );
                    WAIT_STATE( ch, wait );
                    return;
                }
            }
            else
            {
                if ( ( victim = get_char_room( ch, target_name ) ) == NULL )
                {
                    /* wymawiamy slowka */
                    say_spell( ch, sn );
                    send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
                    remove_memorized_spell ( ch, sn );
                    WAIT_STATE( ch, wait );
                    return;
                }
            }

            if ( is_safe( ch, victim ) && victim != ch )
            {
                /* wymawiamy slowka */
                say_spell( ch, sn );
                send_to_char( "Na ten cel nie da rady.\n\r", ch );
                remove_memorized_spell ( ch, sn );
                WAIT_STATE( ch, wait );
                return;
            }

            if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
            {
                /* wymawiamy slowka */
                say_spell( ch, sn );
                send_to_char( "Przecie¿ to twój przyjaciel!\n\r", ch );
                remove_memorized_spell ( ch, sn );
                WAIT_STATE( ch, wait );
                return;
            }

            // zdejmujemy invisa, z komunikatami
            strip_invis( ch, TRUE, TRUE );

            vo = ( void * ) victim;
            target = TARGET_CHAR;
            break;

        case TAR_CHAR_DEFENSIVE:
            if ( arg2[ 0 ] == '\0' )
            {
                victim = ch;
            }
            else
            {
                if ( ( victim = get_char_room( ch, target_name ) ) == NULL )
                {
                    /* wymawiamy slowka */
                    say_spell( ch, sn );
                    send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
                    remove_memorized_spell ( ch, sn );
                    WAIT_STATE( ch, wait );
                    return;
                }
            }

            if( IS_AFFECTED( victim, AFF_FORCE_FIELD ))
            {
                /* wymawiamy slowka */
                say_spell( ch, sn );
                act( "$N znajduje siê wewn±trz kopu³y mocy, ¿adnemu twemu zaklêciu nie uda siê przedrzeæ.", ch, NULL, victim, TO_CHAR);
                remove_memorized_spell ( ch, sn );
                WAIT_STATE( ch, wait );
                return;
            }

            vo = ( void * ) victim;
            target = TARGET_CHAR;
            break;

        case TAR_CHAR_SELF:
            if ( arg2[ 0 ] != '\0' && get_char_room( ch, arg2 ) != ch )
            {
                /* wymawiamy slowka */
                say_spell( ch, sn );
                send_to_char( "Ten czar mo¿esz rzuciæ tylko na siebie.\n\r", ch );
                remove_memorized_spell ( ch, sn );
                WAIT_STATE( ch, wait );
                return;
            }

            vo = ( void * ) ch;
            target = TARGET_CHAR;
            break;

        case TAR_OBJ_INV:
            if ( arg2[ 0 ] == '\0' )
            {
                send_to_char( "Na co ma byæ rzucony ten czar?\n\r", ch );
                return;
            }

            if ( ( obj = get_obj_carry( ch, target_name, ch ) ) == NULL )
            {
                /* wymawiamy slowka */
                say_spell( ch, sn );
                send_to_char( "Nie masz tego przy sobie.\n\r", ch );
                remove_memorized_spell ( ch, sn );
                WAIT_STATE( ch, wait );
                return;
            }

            vo = ( void * ) obj;
            target = TARGET_OBJ;
            break;

        case TAR_OBJ_ROOM:
            if ( arg2[ 0 ] == '\0' )
            {
                send_to_char( "Na co ma byæ rzucony ten czar?\n\r", ch );
                return;
            }

            if ( ( obj = get_obj_list( ch, target_name, ch->in_room->contents ) ) == NULL )
            {
                /* wymawiamy slowka */
                say_spell( ch, sn );
                send_to_char( "Nie widzisz tu nic takiego.\n\r", ch );
                remove_memorized_spell ( ch, sn );
                WAIT_STATE( ch, wait );
                return;
            }

            vo = ( void * ) obj;
            target = TARGET_OBJ;
            break;

        case TAR_OBJ_CHAR_OFF:
            if ( arg2[ 0 ] == '\0' )
            {
                if ( ( victim = ch->fighting ) == NULL )
                {
                    send_to_char( "Rzuciæ ten czar na kogo czy co?\n\r", ch );
                    return;
                }

                target = TARGET_CHAR;
            }
            else if ( ( victim = get_char_room( ch, target_name ) ) != NULL )
            {
                target = TARGET_CHAR;
            }

            if ( target == TARGET_CHAR )  /* check the sanity of the attack */
            {
                if ( is_safe_spell( ch, victim, FALSE ) && victim != ch )
                {
                    /* wymawiamy slowka */
                    say_spell( ch, sn );
                    send_to_char( "Nie mo¿esz tego zrobiæ.\n\r", ch );
                    remove_memorized_spell ( ch, sn );
                    WAIT_STATE( ch, wait );
                    return;
                }

                if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
                {
                    /* wymawiamy slowka */
                    say_spell( ch, sn );
                    send_to_char( "Przecie¿ to twój przyjaciel.\n\r", ch );
                    remove_memorized_spell ( ch, sn );
                    WAIT_STATE( ch, wait );
                    return;
                }

                /*	    if (!IS_NPC(ch))
                        check_killer(ch,victim);*/

                // zdejmujemy invisa, z komunikatami
                strip_invis( ch, TRUE, TRUE );

                vo = ( void * ) victim;
            }
            else if ( ( obj = get_obj_here( ch, target_name ) ) != NULL )
            {
                vo = ( void * ) obj;
                target = TARGET_OBJ;
            }
            else
            {
                /* wymawiamy slowka */
                say_spell( ch, sn );
                send_to_char( "Nie widzisz tego tutaj.\n\r", ch );
                remove_memorized_spell ( ch, sn );
                WAIT_STATE( ch, wait );
                return;
            }
            break;

        case TAR_OBJ_CHAR_DEF:
            if ( arg2[ 0 ] == '\0' )
            {
                vo = ( void * ) ch;
                target = TARGET_CHAR;
            }
            else if ( ( victim = get_char_room( ch, target_name ) ) != NULL )
            {
                if( IS_AFFECTED( victim, AFF_FORCE_FIELD ))
                {
                    /* wymawiamy slowka */
                    say_spell( ch, sn );
                    act( "$N znajduje siê wewn±trz kopu³y mocy, ¿adnemu twemu zaklêciu nie uda siê przedrzeæ.", ch, NULL, victim, TO_CHAR);
                    remove_memorized_spell ( ch, sn );
                    WAIT_STATE( ch, wait );
                    return;
                }
                vo = ( void * ) victim;
                target = TARGET_CHAR;
            }
            else if ( ( obj = get_obj_carry( ch, target_name, ch ) ) != NULL )
            {
                vo = ( void * ) obj;
                target = TARGET_OBJ;
            }
            else
            {
                /* wymawiamy slowka */
                say_spell( ch, sn );
                send_to_char( "Nie widzisz tego tutaj.\n\r", ch );
                remove_memorized_spell ( ch, sn );
                WAIT_STATE( ch, wait );
                return;
            }
            break;

        case TAR_OBJ_IGNORE:
            if ( arg2[ 0 ] != '\0' )
            {
                if ( ( obj = get_obj_carry( ch, target_name, ch ) ) == NULL )
                {
                    /* wymawiamy slowka */
                    say_spell( ch, sn );
                    send_to_char( "Nie widzisz tu nic takiego.\n\r", ch );
                    remove_memorized_spell ( ch, sn );
                    WAIT_STATE( ch, wait );
                    return;
                }
                vo = ( void * ) obj;
                target = TARGET_OBJ;
            }
            break;
    }

    /* wymawiamy slowka */
    say_spell( ch, sn );

    //tutaj budzenie jak sie ma cautious sleep
	DEBUG_INFO( "do_cast:cautious_sleep");
	for( sleeping_char = ch->in_room->people; sleeping_char; sleeping_char = sleeping_char->next_in_room )
	{
		if( sleeping_char->position != POS_SLEEPING || !IS_AFFECTED( sleeping_char, AFF_CAUTIOUS_SLEEP ) )
			continue;

		if ( is_same_group( ch, sleeping_char ) )
			continue;

		if( IS_AFFECTED( sleeping_char, AFF_SLEEP ) )
		{
			affect_strip( sleeping_char, gsn_sleep );
			affect_strip( sleeping_char, gsn_healing_sleep );
			EXT_REMOVE_BIT( sleeping_char->affected_by, AFF_SLEEP );
		}

		send_to_char( "Nagle jaka¶ silna magia wyrywa ciê ze snu!\n\r", sleeping_char );
		do_stand( sleeping_char, "" );
	}

    //i rozdzki
    DEBUG_INFO( "do_cast:wands");
    wait += helper_do_cast_wait_wand_bonus( ch, sn );

    /* rzucanie czarow z przemienionymi dlonmi trwa dluzej */
    if (  IS_AFFECTED( ch, AFF_RAZORBLADED ) || IS_AFFECTED( ch, AFF_BEAST_CLAWS ))
    {
        wait = ( 3 * wait ) / 2;
    }

   //Szaman potrafi szybciej rzucac czary.
    if ( ch->class == CLASS_SHAMAN )
    {
        int invoke = get_skill(ch, gsn_invoke_spirit);
        //Maxymalnie o 33% przy skillu invoke na arcymistrz.
        wait = UMIN (wait/3, ((wait*100)-invoke/3)/100);
    }

    WAIT_STATE( ch, wait );

    /* nomagic? a mo¿e gluchota.. no to kasujemu mu spella i koniec */
    if ( ( IS_AFFECTED( ch, AFF_DEAFNESS ) && number_percent() < 50 ) ||
         EXT_IS_SET( ch->in_room->room_flags, ROOM_NOMAGIC ) ||
         ( ch->daze > 0 && number_percent() < 75 ) ||
         ( !IS_NPC( ch ) && ch->condition[ COND_SLEEPY ] == 0 && number_percent() < 75 ) )
    {
        remove_memorized_spell ( ch, sn );

        if ( ch->fighting )
            SET_BIT( ch->fight_data, FIGHT_CASTING );

        send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia.\n\r", ch );
        return;
    }

    if ( is_affected( ch, gsn_antimagic_manacles ))
    {
        kajdany = affect_find( ch->affected, gsn_antimagic_manacles );
        if( number_percent() < kajdany->level*2 )
        {
            remove_memorized_spell ( ch, sn );

            if ( ch->fighting )
                SET_BIT( ch->fight_data, FIGHT_CASTING );

            send_to_char( "Kajdany na twych d³oniach wsysaj± moc zaklêcia.\n\r", ch );
            return;
        }
    }

    switch( get_caster(ch))
    {
		case 0:
		case 8:
			wis_or_int = get_curr_stat_deprecated( ch, STAT_INT );
			break;
		case 1:
		case 4:
		case 5:
		case 10:
			wis_or_int = get_curr_stat_deprecated( ch, STAT_WIS );
			break;
		default:
			wis_or_int = 0;
			break;
    }

    if ( number_percent() <= wis_app[wis_or_int].fail )
    {
        remove_memorized_spell ( ch, sn );
        if ( ch->fighting )
            SET_BIT( ch->fight_data, FIGHT_CASTING );

        send_to_char( "Tracisz koncentracjê i nie udaje ci siê rzuciæ zaklêcia.\n\r", ch );
        return;
    }

    if ( ch != victim && check_blink( ch, victim, FALSE ) )
    {
        send_to_char( "Próbujesz rzuciæ zaklêcie, jednak przeskakujesz na chwilkê do innego planu egzystencji i cel znika ci z oczu.\n\r", ch );
        remove_memorized_spell ( ch, sn );
        if ( ch->fighting )
        {
            SET_BIT( ch->fight_data, FIGHT_CASTING );
        }
        return;
    }

    //confusion
    pAffect = affect_find( ch->affected, gsn_confusion );
    if ( pAffect && pAffect->duration > 0 && number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) < number_range( 0, pAffect->modifier ) )
    {
        if ( ch->fighting && ( skill_table[ sn ].target == TAR_CHAR_OFFENSIVE
                               || ( skill_table[ sn ].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR ) ) )
        {
            pAffect->duration = UMAX( 0, pAffect->duration - 1 ); pAffect->rt_duration = 0;
            send_to_char( "Czujesz siê trochê zdezorientowan<&y/a/e> i zapominasz dokoñczyæ rzucanie czaru!\n\r", ch );
            act( "$n przez chwilê wygl±da na $t i zapomina dokoñczyæ rzucanie czaru!",
                 ch, ch->sex == 2 ? "zdezorientowan±" : "zdezorientowanego", victim, TO_NOTVICT );
            return;
        }
    }

    /* prewait? */
    if ( check_pwait( ch, sn ) )
    {
        setup_pwait( ch, skill_table[ sn ].name, vo, target, sn );
        remove_memorized_spell ( ch, sn );
        return;
    }

    /* reflect spell stuff */
        if ( target == TARGET_CHAR )
        {
            int circle = 0;
            CHAR_DATA *victim;

            victim = ( CHAR_DATA * ) vo;

            if ( victim != ch )
            {
                circle = spell_circle( ch, sn );
				//dispel magic i great dispel magic olewaja reflecty i globy
                if ( check_globes( victim, circle ) && sn != 392 && sn != 40 )
                {
                    act( "Twoje zaklêcie znika przy zetkniêciu ze sfer± otaczaj±c± $C.", ch, NULL, victim, TO_CHAR );
                    act( "Zaklêcie $z znika przy zetkniêciu z otaczaj±c± ciê sfer±.\n\r", ch, NULL, victim, TO_VICT );
                    act( "Zaklêcie $z znika przy zetkniêciu z otaczaj±c± $C sfer±.", ch, NULL, victim, TO_NOTVICT );

                    if ( ch->fighting )
                    {
                        SET_BIT( ch->fight_data, FIGHT_CASTING );
                    }

                    remove_memorized_spell ( ch, sn );

                    if ( ( skill_table[ sn ].target == TAR_CHAR_OFFENSIVE
                           || ( skill_table[ sn ].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR ) )
                         && victim != ch
                         && victim->master != ch )
                    {
                        CHAR_DATA * vch;
                        CHAR_DATA *vch_next;

                        for ( vch = ch->in_room->people; vch; vch = vch_next )
                        {
                            vch_next = vch->next_in_room;

                            if ( victim == vch && victim->fighting == NULL && can_see( victim, ch ) )
                            {
                            	if ( victim->position == POS_SITTING || victim->position == POS_RESTING )
								{
									victim->position = POS_STANDING;
									act( "Wstajesz.", victim, NULL, NULL, TO_CHAR );
									act( "$n wstaje.", victim, NULL, NULL, TO_ROOM );
								}
                                multi_hit( victim, ch, TYPE_UNDEFINED );
                                break;
                            }
                        }
                    }
                    return;
                }//dispel magic i great dispel magic olewaja reflecty i globy
                else if ( check_reflect_spell( victim, circle ) && sn != 392 && sn != 40 )
                {
                    vo = ( void * ) ch;
                    print_char( ch, "Twoje zaklêcie odbija siê od ochronnej tarczy %s.\n\r", victim->name2 );
                    print_char( victim, "Zaklêcie %s odbija siê od ochronnej tarczy.\n\r", ch->name2 );
                    act( "Zaklêcie $z odbija siê od ochronnej tarczy $Z.", ch, NULL, victim, TO_NOTVICT );
                    if ( !IS_AFFECTED( victim, AFF_REFLECT_SPELL ) )
                    {
                        send_to_char( skill_table[ gsn_reflect_spell ].msg_off, victim );
                        send_to_char( "\n\r", victim );
                    }
                }
            }
        }
        /* koniec reflect spell */

        if ( victim )
        {
            victim_hit = victim->hit;
            for ( pAffect = victim->affected; pAffect; pAffect = pAffect->next )
            {
                victim_affects++;
            }
        }

		bonus_from_feat = 0;

		//druidzi - bonusy w sektorach naturalnych, penalty w pozostalych
		int sector_type = ch->in_room->sector_type; // coby mniej referencji u¿ywaæ

		if ( !IS_NPC( ch ) && ch->class == CLASS_DRUID )
		{
			if (IS_SET( sector_table[ sector_type ].flag, SECT_DRUID ))
				bonus_from_sector = 2;
			else
				bonus_from_sector = -2;
		}
		/* ma³a szansa na wzrost levelu spella dla druidow w sektorach naturalnych */
		if ( ( ch->class == CLASS_DRUID ) && IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_DRUID ))
		{
			if ( number_range(0,1000) < ( level + (level / 31)*10) )
			{
				send_to_char( "{gCzujesz {Wnag³y {Yprzyp³yw {Gmocy{x.\n\r", ch );
				bonus_from_sector += 4; // zeby wogole byl zauwazalny
			}
		}
		else
		{	/* i odwrotna sprawa, obni¿enie levelu spella */
			if ( number_range(0, (10 * level) + (100*(level / 31)) ) == 0 && (ch->class == CLASS_DRUID) )
			{
				send_to_char( "Czujesz, ¿e ten czar bêdzie o wiele s³abszy.\n\r", ch );
				bonus_from_sector -= 4;
			}

		}

		//rozdzki
		wand_first_value = FALSE;
		wand = get_eq_char( ch, WEAR_HOLD );
		if( !IS_NPC(ch) && wand != NULL && wand->item_type == ITEM_WAND )
		{
			if( wand->value[0] == WAND_LEV_VALUE )
			{
				wand_first_value = TRUE;
				if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
				{
					if( IS_GOOD( ch ) )
						bonus_from_wand += wand->value[1];
					else
						bonus_from_wand -= wand->value[1];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
				{
					if( IS_NEUTRAL( ch ) )
						bonus_from_wand += wand->value[1];
					else
						bonus_from_wand -= wand->value[1];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
				{
					if( IS_EVIL( ch ) )
						bonus_from_wand += wand->value[1];
					else
						bonus_from_wand -= wand->value[1];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
				{
					if( ch->class == CLASS_MAG )
						bonus_from_wand += wand->value[1];
					else
						bonus_from_wand -= wand->value[1];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
				{
					if( ch->class == CLASS_CLERIC )
						bonus_from_wand += wand->value[1];
					else
						bonus_from_wand -= wand->value[1];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
				{
					if( ch->class == CLASS_DRUID )
						bonus_from_wand += wand->value[1];
					else
						bonus_from_wand -= wand->value[1];
				}
				else
					bonus_from_wand += wand->value[1];
			}
			else if( wand->value[2] == WAND_LEV_VALUE )
			{
				if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
				{
					if( IS_GOOD( ch ) )
						bonus_from_wand += wand->value[3];
					else
						bonus_from_wand -= wand->value[3];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
				{
					if( IS_NEUTRAL( ch ) )
						bonus_from_wand += wand->value[3];
					else
						bonus_from_wand -= wand->value[3];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
				{
					if( IS_EVIL( ch ) )
						bonus_from_wand += wand->value[3];
					else
						bonus_from_wand -= wand->value[3];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
				{
					if( ch->class == CLASS_MAG )
						bonus_from_wand += wand->value[3];
					else
						bonus_from_wand -= wand->value[3];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
				{
					if( ch->class == CLASS_CLERIC )
						bonus_from_wand += wand->value[3];
					else
						bonus_from_wand -= wand->value[3];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
				{
					if( ch->class == CLASS_DRUID )
						bonus_from_wand += wand->value[3];
					else
						bonus_from_wand -= wand->value[3];
				}
				else
					bonus_from_wand += wand->value[3];
			}
			else if( wand->value[4] == WAND_LEV_VALUE )
			{
				if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
				{
					if( IS_GOOD( ch ) )
						bonus_from_wand += wand->value[5];
					else
						bonus_from_wand -= wand->value[5];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
				{
					if( IS_NEUTRAL( ch ) )
						bonus_from_wand += wand->value[5];
					else
						bonus_from_wand -= wand->value[5];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
				{
					if( IS_EVIL( ch ) )
						bonus_from_wand += wand->value[5];
					else
						bonus_from_wand -= wand->value[5];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
				{
					if( ch->class == CLASS_MAG )
						bonus_from_wand += wand->value[5];
					else
						bonus_from_wand -= wand->value[5];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
				{
					if( ch->class == CLASS_CLERIC )
						bonus_from_wand += wand->value[5];
					else
						bonus_from_wand -= wand->value[5];
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
				{
					if( ch->class == CLASS_DRUID )
						bonus_from_wand += wand->value[5];
					else
						bonus_from_wand -= wand->value[5];
				}
				else
					bonus_from_wand += wand->value[5];
			}
		}

		if ( bonus_from_wand != 0 && !IS_NPC(ch) )
		{
			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && !IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && !IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && !IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && !IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && !IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && !IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && !IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && !IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }
		}

		bonus_from_feat += bonus_from_wand;

		wand_first_value = FALSE;
		bonus_from_wand = 0;
		wand = get_eq_char( ch, WEAR_HOLD );
		if( !IS_NPC(ch) && wand != NULL && wand->item_type == ITEM_WAND )
		{
			if( wand->value[0] == WAND_LEV_PERCENT )
			{
				wand_first_value = TRUE;
				if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
				{
					if( IS_GOOD( ch ) )
						bonus_from_wand += (level*wand->value[1])/100;
					else
						bonus_from_wand -= (level*wand->value[1])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
				{
					if( IS_NEUTRAL( ch ) )
						bonus_from_wand += (level*wand->value[1])/100;
					else
						bonus_from_wand -= (level*wand->value[1])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
				{
					if( IS_EVIL( ch ) )
						bonus_from_wand += (level*wand->value[1])/100;
					else
						bonus_from_wand -= (level*wand->value[1])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
				{
					if( ch->class == CLASS_MAG )
						bonus_from_wand += (level*wand->value[1])/100;
					else
						bonus_from_wand -= (level*wand->value[1])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
				{
					if( ch->class == CLASS_CLERIC )
						bonus_from_wand += (level*wand->value[1])/100;
					else
						bonus_from_wand -= (level*wand->value[1])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
				{
					if( ch->class == CLASS_DRUID )
						bonus_from_wand += (level*wand->value[1])/100;
					else
						bonus_from_wand -= (level*wand->value[1])/100;
				}
				else
					bonus_from_wand += (level*wand->value[1])/100;
			}
			else if( wand->value[2] == WAND_LEV_PERCENT )
			{
				if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
				{
					if( IS_GOOD( ch ) )
						bonus_from_wand += (level*wand->value[3])/100;
					else
						bonus_from_wand -= (level*wand->value[3])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
				{
					if( IS_NEUTRAL( ch ) )
						bonus_from_wand += (level*wand->value[3])/100;
					else
						bonus_from_wand -= (level*wand->value[3])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
				{
					if( IS_EVIL( ch ) )
						bonus_from_wand += (level*wand->value[3])/100;
					else
						bonus_from_wand -= (level*wand->value[3])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
				{
					if( ch->class == CLASS_MAG )
						bonus_from_wand += (level*wand->value[3])/100;
					else
						bonus_from_wand -= (level*wand->value[3])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
				{
					if( ch->class == CLASS_CLERIC )
						bonus_from_wand += (level*wand->value[3])/100;
					else
						bonus_from_wand -= (level*wand->value[3])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
				{
					if( ch->class == CLASS_DRUID )
						bonus_from_wand += (level*wand->value[3])/100;
					else
						bonus_from_wand -= (level*wand->value[3])/100;
				}
				else
					bonus_from_wand += (level*wand->value[3])/100;
			}
			else if( wand->value[4] == WAND_LEV_PERCENT )
			{
				if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
				{
					if( IS_GOOD( ch ) )
						bonus_from_wand += (level*wand->value[5])/100;
					else
						bonus_from_wand -= (level*wand->value[5])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
				{
					if( IS_NEUTRAL( ch ) )
						bonus_from_wand += (level*wand->value[5])/100;
					else
						bonus_from_wand -= (level*wand->value[5])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
				{
					if( IS_EVIL( ch ) )
						bonus_from_wand += (level*wand->value[5])/100;
					else
						bonus_from_wand -= (level*wand->value[5])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
				{
					if( ch->class == CLASS_MAG )
						bonus_from_wand += (level*wand->value[5])/100;
					else
						bonus_from_wand -= (level*wand->value[5])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
				{
					if( ch->class == CLASS_CLERIC )
						bonus_from_wand += (level*wand->value[5])/100;
					else
						bonus_from_wand -= (level*wand->value[5])/100;
				}
				else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
				{
					if( ch->class == CLASS_DRUID )
						bonus_from_wand += (level*wand->value[5])/100;
					else
						bonus_from_wand -= (level*wand->value[5])/100;
				}
				else
					bonus_from_wand += (level*wand->value[5])/100;
			}
		}

		if ( bonus_from_wand != 0 && !IS_NPC(ch) )
		{
			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
			 {
			 	if ( !wand_first_value )
			 		bonus_from_wand = 0;
			 }

			if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && !IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && !IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && !IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && !IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && !IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && !IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && !IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }

			 if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && !IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
			 {
			 	if ( wand_first_value )
			 		bonus_from_wand = 0;
			 }
		}

		bonus_from_feat += bonus_from_wand;
/* juz nieaktualne, czary traktowane sa indywidualnie, Raszer, holy_pray
        // paladyn ma szanse na podbicie (obnizenie) lv czarku kosztem holy
        if ( ( pHoly = affect_find( ch->affected, gsn_holy_pool )) != NULL )
        {
             pskill = get_skill( ch, gsn_holy_prayer );
             sskill = get_skill( ch, gsn_sanctification );

             if ( number_percent() < pskill / 5 )
             {
                 if ( pHoly->modifier > 0 )
                 {
                     pHoly->modifier--;
                     sskill = 100 + UMAX( sskill, 10 );
                     send_to_char( "Czujesz jak twój czar staje siê pote¿niejszy dziêki przepe³niaj±cej cie mocy ¶wiat³a.\n\r", ch );
                     act( "Widzisz jak dooko³a $z rozb³yska na chwilê z³ota po¶wiata.", ch, NULL, NULL, TO_ROOM );
                 }
                 else if  ( pHoly->modifier < 0 )
                 {
                     sskill = 100 - UMAX( sskill, 10 );
                     send_to_char( "Czujesz jak twoj czar s³abnie.\n\r", ch );
                     act( "Widzisz $z otacza na chwilê zgni³ozielona po¶wiata.", ch, NULL, NULL, TO_ROOM );
                 }
                 else
                     sskill = 100;
             }
             else
                 sskill = 100;
        }
*/
        check = FALSE;
        /* specjalisci maja +2 do levela zaklêcia*/
        if ( !IS_NPC( ch ) &&
             ch->pcdata->mage_specialist >= 0 &&
             IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
            ( *skill_table[ sn ].spell_fun ) ( sn, level + 2 + bonus_from_feat, ch, vo, target );
        //paladyni
        else if ( get_caster( ch ) == 4 )
            ( *skill_table[ sn ].spell_fun ) ( sn, UMAX( 1, ( check ? level : level - 12 ) + bonus_from_feat ), ch, vo, target );
        //bardzi maja -5 do leva zaklecia
        else if ( get_caster( ch ) == 8 )
            ( *skill_table[ sn ].spell_fun ) ( sn, UMAX( 1, level - 5 + bonus_from_feat), ch, vo, target );
        else
            ( *skill_table[ sn ].spell_fun ) ( sn, level + bonus_from_feat + bonus_from_sector, ch, vo, target );

    /* na wypadek jesli by siê rozwalil o reflect spell*/
    if ( !ch || !ch->in_room )
    {
        return;
    }

    if ( ch->fighting )
    {
        SET_BIT( ch->fight_data, FIGHT_CASTING );
    }

    remove_memorized_spell ( ch, sn );

    if ( victim )
    {
        cast_ok = FALSE;
        if ( victim_hit != victim->hit )
            cast_ok = TRUE;
        else
        {
            for ( pAffect = victim->affected; pAffect; pAffect = pAffect->next )
                victim_affects--;
            if ( victim_affects < 0 )
                cast_ok = TRUE;
        }
    }

    /* przywalenie zlosliwemu kolesiowi za czarek */
    if ( ( skill_table[ sn ].target == TAR_CHAR_OFFENSIVE
           || ( skill_table[ sn ].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR ) )
         && victim != ch
         && victim->master != ch
         && victim->in_room == ch->in_room )
    {
        if ( !EXT_IS_SET( victim->act, ACT_SENTINEL ) )
            start_hunting( victim, ch );
        else
            start_hating( victim, ch );

        if ( cast_ok && victim->fighting == NULL && IS_AFFECTED( victim, AFF_PARALYZE )
             && sn != gsn_holdperson && sn != gsn_holdmonster && sn != gsn_holdanimal && sn != gsn_holdplant
             && sn != gsn_holdevil && sn != gsn_ghoul_touch )
        {
            affect_strip( victim, gsn_holdperson );
            affect_strip( victim, gsn_holdmonster );
            affect_strip( victim, gsn_holdanimal );
            affect_strip( victim, gsn_holdplant );
            affect_strip( victim, gsn_holdevil );
            affect_strip( victim, gsn_ghoul_touch );
        }

		if ( victim->fighting == NULL && can_see( victim, ch ) )
        {
            if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
                mp_percent_trigger( victim, ch, NULL, NULL, &TRIG_KILL );

			if ( victim->position == POS_SITTING || victim->position == POS_RESTING )
			{
				victim->position = POS_STANDING;
				act( "Wstajesz.", victim, NULL, NULL, TO_CHAR );
				act( "$n wstaje.", victim, NULL, NULL, TO_ROOM );
			}
            multi_hit( victim, ch, TYPE_UNDEFINED );
        }
		else if ( can_see( ch, victim ) && can_see( victim, ch ) )
			set_fighting( ch, victim );
    }

    return;
}

/*
* Cast spells at targets using a magical object.
*/
void obj_cast_spell( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj )
{
	void * vo;
	int target = TARGET_NONE;

	if ( sn <= 0 )
		return;

	if ( sn >= MAX_SKILL || skill_table[ sn ].spell_fun == 0 )
	{
		bug( "Obj_cast_spell: bad sn %d.", sn );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_NOMAGIC ) )
		return;

	switch ( skill_table[ sn ].target )
	{
		default:
			bug( "Obj_cast_spell: bad target for sn %d.", sn );
			return;

		case TAR_IGNORE:
			vo = NULL;
			break;

		case TAR_CHAR_OFFENSIVE:
			if ( victim == NULL )
				victim = ch->fighting;
			if ( victim == NULL )
			{
				send_to_char( "Nie mo¿esz tego zrobiæ.\n\r", ch );
				return;
			}
			if ( is_safe( ch, victim ) && ch != victim )
			{
				send_to_char( "Co¶ siê tu nie zgadza...\n\r", ch );
				return;
			}
			vo = ( void * ) victim;
			target = TARGET_CHAR;
			break;

		case TAR_CHAR_DEFENSIVE:
		case TAR_CHAR_SELF:
			if ( victim == NULL )
				victim = ch;
			vo = ( void * ) victim;
			target = TARGET_CHAR;
			break;

		case TAR_OBJ_INV:
			if ( obj == NULL )
			{
				send_to_char( "Nie mo¿esz tego zrobiæ.\n\r", ch );
				return;
			}
			vo = ( void * ) obj;
			target = TARGET_OBJ;
			break;

		case TAR_OBJ_CHAR_OFF:
			if ( victim == NULL && obj == NULL )
			{
				if ( ch->fighting != NULL )
					victim = ch->fighting;
				else
				{
					send_to_char( "Nie mo¿esz tego zrobiæ.\n\r", ch );
					return;
				}
			}

			if ( victim != NULL )
			{
				if ( is_safe_spell( ch, victim, FALSE ) && ch != victim )
				{
					send_to_char( "Co¶ jest nie tak...\n\r", ch );
					return;
				}

				vo = ( void * ) victim;
				target = TARGET_CHAR;
			}
			else
			{
				vo = ( void * ) obj;
				target = TARGET_OBJ;
			}
			break;


		case TAR_OBJ_CHAR_DEF:
			if ( victim == NULL && obj == NULL )
			{
				vo = ( void * ) ch;
				target = TARGET_CHAR;
			}
			else if ( victim != NULL )
			{
				vo = ( void * ) victim;
				target = TARGET_CHAR;
			}
			else
			{
				vo = ( void * ) obj;
				target = TARGET_OBJ;
			}

			break;
	}

	target_name = "";
	( *skill_table[ sn ].spell_fun ) ( sn, level, ch, vo, target );

	if ( ( skill_table[ sn ].target == TAR_CHAR_OFFENSIVE
	       || ( skill_table[ sn ].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR ) )
	     && victim != ch
	     && victim->master != ch )
	{
		CHAR_DATA * vch;
		CHAR_DATA *vch_next;

		for ( vch = ch->in_room->people; vch; vch = vch_next )
		{
			vch_next = vch->next_in_room;
			if ( victim == vch && victim->fighting == NULL && can_see( victim, ch ) )
			{
				check_killer( victim, ch );
				multi_hit( victim, ch, TYPE_UNDEFINED );
				break;
			}
		}
	}

	return;
}

void spell_cancellation( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	bool found = FALSE;

	level += 2;

	if ( ( !IS_NPC( ch ) && IS_NPC( victim ) &&
	       !( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim ) ) ||
	     ( IS_NPC( ch ) && !IS_NPC( victim ) ) )
	{
		send_to_char( "Nie uda³o ci siê, mo¿e rozproszenie magii pomo¿e.\n\r", ch );
		return;
	}

	/* unlike dispel magic, the victim gets NO save */

	/* begin running through the spells */

	if ( check_dispel( level, victim, skill_lookup( "armor" ) ) )
		found = TRUE;

	if ( check_dispel( level, victim, skill_lookup( "bless" ) ) )
		found = TRUE;

	if ( check_dispel( level, victim, skill_lookup( "blindness" ) ) )
	{
		found = TRUE;
		switch ( victim->sex )
		{
			case 0:
				act( "$n ju¿ nie jest ¶lepe.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n ju¿ nie jest ¶lepy.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n ju¿ nie jest ¶lepa.", victim, NULL, NULL, TO_ROOM );
				break;
		}
	}

	if ( check_dispel( level, victim, skill_lookup( "calm" ) ) )
	{
		found = TRUE;
		switch ( victim->sex )
		{
			case 0:
				act( "$n ju¿ nie wygl±da na tak spokojne...", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n ju¿ nie wygl±da na tak spokojnego...", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n ju¿ nie wygl±da na tak spokojn±...", victim, NULL, NULL, TO_ROOM );
				break;
		}
	}

	if ( check_dispel( level, victim, skill_lookup( "change sex" ) ) )
	{
		found = TRUE;
		act( "$n wygl±da ju¿ normalniej.", victim, NULL, NULL, TO_ROOM );
	}

	if ( check_dispel( level, victim, skill_lookup( "charm person" ) ) )
	{
		found = TRUE;
		act( "$n odzyskuje w³asn± wolê.", victim, NULL, NULL, TO_ROOM );
	}

	if ( check_dispel( level, victim, skill_lookup( "chill touch" ) ) )
	{
		found = TRUE;
		act( "$x jest chyba cieplej.", victim, NULL, NULL, TO_ROOM );
	}

	if ( check_dispel( level, victim, skill_lookup( "curse" ) ) )
		found = TRUE;

	if ( check_dispel( level, victim, skill_lookup( "detect evil" ) ) )
		found = TRUE;

	if ( check_dispel( level, victim, skill_lookup( "detect good" ) ) )
		found = TRUE;

	if ( check_dispel( level, victim, skill_lookup( "detect hidden" ) ) )
		found = TRUE;

	if ( check_dispel( level, victim, skill_lookup( "detect invis" ) ) )
		found = TRUE;

	if ( check_dispel( level, victim, skill_lookup( "detect magic" ) ) )
		found = TRUE;

	if ( check_dispel( level, victim, skill_lookup( "faerie fire" ) ) )
	{
		act( "P³on±ca otoczka wokó³ $z znika.", victim, NULL, NULL, TO_ROOM );
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "fly" ) ) )
	{
		act( "$n opada na ziemie!", victim, NULL, NULL, TO_ROOM );
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "float" ) ) )
	{
		act( "$n opada na ziemie!", victim, NULL, NULL, TO_ROOM );
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "frenzy" ) ) )
	{
		act( "Dziko¶æ $z zanika.", victim, NULL, NULL, TO_ROOM );;
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "giant strength" ) ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n nie wygl±da ju¿ na tak silne.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n nie wygl±da ju¿ na tak silnego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n nie wygl±da ju¿ na tak siln±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "haste" ) ) )
	{
		act( "$n zaczyna poruszaæ siê wolniej.", victim, NULL, NULL, TO_ROOM );
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "infravision" ) ) )
		found = TRUE;

	if ( check_dispel( level, victim, skill_lookup( "invis" ) ) )
	{
		act( "$n powoli pojawia siê z nico¶ci.", victim, NULL, NULL, TO_ROOM );
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "mass invis" ) ) )
	{
		act( "$n powoli pojawia siê z nico¶ci.", victim, NULL, NULL, TO_ROOM );
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "pass door" ) ) )
		found = TRUE;

	if ( check_dispel( level, victim, skill_lookup( "protection evil" ) ) )
		found = TRUE;

	if ( check_dispel( level, victim, skill_lookup( "protection good" ) ) )
		found = TRUE;

	if ( check_dispel( level, victim, skill_lookup( "sanctuary" ) ) )
	{
		act( "Bia³a aura znika z cia³a $z.", victim, NULL, NULL, TO_ROOM );
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "shield" ) ) )
	{
		act( "Magiczna tarcza otaczaj±ca $c znika.", victim, NULL, NULL, TO_ROOM );
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "sleep" ) ) )
		found = TRUE;

	if ( check_dispel( level, victim, skill_lookup( "slow" ) ) )
	{
		act( "$n przestaje poruszaæ siê tak wolno.", victim, NULL, NULL, TO_ROOM );
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "stone skin" ) ) )
	{
		act( "Skóra $z odzyskuje swój pierwotny wygl±d.", victim, NULL, NULL, TO_ROOM );
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "weaken" ) ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n wygl±da na silniejsze.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n wygl±da na silniejszego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n wygl±da na silniejsz±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "luck" ) ) )
	{
		act( "Otaczaj±ca $z zielono-z³ota otoczka szczê¶cia znika.", victim, NULL, NULL, TO_ROOM );
		found = TRUE;
	}

	if ( check_dispel( level, victim, skill_lookup( "bull strength" ) ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n nie wygl±da ju¿ na tak silne.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n nie wygl±da ju¿ na tak silnego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n nie wygl±da ju¿ na tak siln±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
		found = TRUE;
	}

		if ( check_dispel( level, victim, skill_lookup( "cat grace" ) ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n nie wygl±da ju¿ na tak zrêcznie.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n nie wygl±da ju¿ na tak zrêcznego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n nie wygl±da ju¿ na tak zrêczn±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
		found = TRUE;
	}

		if ( check_dispel( level, victim, skill_lookup( "owl wisdom" ) ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n nie wygl±da ju¿ na tak m±drze.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n nie wygl±da ju¿ na tak m±drego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n nie wygl±da ju¿ na tak m±dr±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
		found = TRUE;
	}

		if ( check_dispel( level, victim, skill_lookup( "fox cunning" ) ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n nie wygl±da ju¿ na tak inteligentnie.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n nie wygl±da ju¿ na tak inteligentnego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n nie wygl±da ju¿ na tak inteligentn±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
		found = TRUE;
	}

		if ( check_dispel( level, victim, skill_lookup( "bear endurance" ) ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n nie wygl±da ju¿ na tak wytrzymale.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n nie wygl±da ju¿ na tak wytrzyma³ego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n nie wygl±da ju¿ na tak wytrzyma³±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
		found = TRUE;
	}

		if ( check_dispel( level, victim, skill_lookup( "eagle splendor" ) ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n nie wygl±da ju¿ na tak charyzmatycznie.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n nie wygl±da ju¿ na tak charyzmatycznego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n nie wygl±da ju¿ na tak charyzmatyczn±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
		found = TRUE;
	}

		if ( check_dispel( level, victim, skill_lookup( "nimbleness" ) ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n nie wygl±da ju¿ na tak zrêcznie.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n nie wygl±da ju¿ na tak zrêcznego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n nie wygl±da ju¿ na tak zrêczn±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
		found = TRUE;
	}

			if ( check_dispel( level, victim, skill_lookup( "draconic wisdom" ) ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n nie wygl±da ju¿ na tak m±drze.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n nie wygl±da ju¿ na tak m±drego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n nie wygl±da ju¿ na tak m±dr±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
		found = TRUE;
	}

		if ( check_dispel( level, victim, skill_lookup( "insight" ) ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n nie wygl±da ju¿ na tak inteligentnie.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n nie wygl±da ju¿ na tak inteligentnego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n nie wygl±da ju¿ na tak inteligentn±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
		found = TRUE;
	}

			if ( check_dispel( level, victim, skill_lookup( "inspiring presence" ) ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n nie wygl±da ju¿ na tak charyzmatycznie.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n nie wygl±da ju¿ na tak charyzmatycznego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n nie wygl±da ju¿ na tak charyzmatyczn±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
		found = TRUE;
	}

		if ( check_dispel( level, victim, skill_lookup( "behemot toughness" ) ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n nie wygl±da ju¿ na tak wytrzymale.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n nie wygl±da ju¿ na tak wytrzyma³ego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n nie wygl±da ju¿ na tak wytrzyma³±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
		found = TRUE;
	}

	if ( found )
		send_to_char( "Ok.\n\r", ch );
	else
		send_to_char( "Nie uda³o ci sie.\n\r", ch );
}

void spell_create_food( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * food;
	AFFECT_DATA *paf;
	int number;
	bool goodbarries = FALSE, poisoned = FALSE;

	number = URANGE( 1, number_range( level / 4, level / 2 ), 14 );

	if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
        if ( paf->modifier < 30 )
        {
             if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
                  {


                    if (number_percent()  < paf->modifier * 2 && number_percent() < 30)
                    {
                       send_to_char( "Czujesz jak twoje zaklêcie staje siê potê¿niejsze.\n\r", ch );
	                   number += 5;
                       paf->modifier -= 1;
                       if (paf->modifier < 1) paf->modifier = 1;

                    }

                    if (paf->modifier > 30 && number_percent() < 30)
                    {
                      send_to_char( "Czujesz jak wype³niaj±ca ciê moc wp³ywa na zaklêcie.\n\r", ch );
                      goodbarries = TRUE;
                      paf->modifier -= 5;
                      if (paf->modifier < 1) paf->modifier = 1;

                    }

                    if (number_percent() < (-paf->modifier))
                    {
                      send_to_char( "Czujesz jak twe nieczyste sumienie zmienia zaklêcie.\n\r", ch );
                      poisoned = TRUE;

                    }



                  }


        }

    }



    for ( ; number > 0; number-- )
    {
        if ( goodbarries == TRUE)
        {
            food = create_object( get_obj_index( OBJ_VNUM_GOODBARRY ), FALSE );
            food->value[ 0 ] = 6;
            food->value[ 1 ] = 0;
            obj_to_char( food, ch );
        }else
        {
            food = create_object( get_obj_index( OBJ_VNUM_MUSHROOM ), FALSE );
            food->value[ 0 ] = 12;
            food->value[ 1 ] = 48 + level / 2;
            if (poisoned == TRUE) food->value[ 3 ] = 1;
            obj_to_char( food, ch );
        }
    }
    act( "Na twoich d³oniach pojawia siê jedzenie.", ch, NULL, NULL, TO_CHAR );
    return;
}


/* RT added to cure plague */
void spell_cure_disease( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;

	if ( !is_affected( victim, gsn_plague ) )
	{
		if ( victim == ch )
		{
			switch ( victim->sex )
			{
				case 0:
					send_to_char( "Nie jeste¶ chore.\n\r", ch );
					break;
				case 1:
					send_to_char( "Nie jeste¶ chory.\n\r", ch );
					break;
				default:
					send_to_char( "Nie jeste¶ chora.\n\r", ch );
					break;
			}
		}
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie wygl±da na chore.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N nie wygl±da na chorego.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "$N nie wygl±da na chor±.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( check_dispel( level, victim, gsn_plague ) )
	{
		send_to_char( "Choroba ciê opuszcza.\n\r", victim );
		switch ( victim->sex )
		{
			case 0:
				act( "$n wygl±da na zdrowsze.", victim, NULL, NULL, TO_ROOM );
				break;
			case 1:
				act( "$n wygl±da na zdrowszego.", victim, NULL, NULL, TO_ROOM );
				break;
			default:
				act( "$n wygl±da na zdrowsz±.", victim, NULL, NULL, TO_ROOM );
				break;
		}
	}
	else
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
    }
}


int check_poison(CHAR_DATA *ch, CHAR_DATA * victim, AFFECT_DATA *paf)
{
	if ( !paf )
	{
		if ( victim == ch )
		{
			switch ( victim->sex )
			{
				case 0:
					send_to_char( "Nie jeste¶ zatrute.\n\r", ch );
					break;
				case 1:
					send_to_char( "Nie jeste¶ zatruty.\n\r", ch );
					break;
				default:
					send_to_char( "Nie jeste¶ zatruta.\n\r", ch );
					break;
			}
		}
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N chyba nie jest zatrute.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N chyba nie jest zatruty.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "$N chyba nie jest zatruta.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return 0;
	}
	return 1;
}

int chance_cure_poison(CHAR_DATA *ch, AFFECT_DATA *paf, int spell_level)
{
	int chance = 0;
	switch(spell_level)
	{
	case 0:
		chance += URANGE(0, number_range( 10, (get_curr_stat_deprecated( ch, STAT_LUC ) *50) /(MAX_STAT_VALUE/9) ), 50);
		chance += URANGE(10, ch->level*4 ,40);
		chance = URANGE(0, chance - ((paf->level * 30) / MAX_POISON), 100);
		break;
	case 1:
		chance += URANGE(0, number_range( 30, (get_curr_stat_deprecated( ch, STAT_LUC ) *70) /(MAX_STAT_VALUE/9) ), 70);
		chance += URANGE(10, ch->level*2 ,40);
		chance = URANGE(0, chance - ((paf->level * 20) / MAX_POISON), 100);
		break;
	case 2:
		chance += URANGE(0, number_range( 30, (get_curr_stat_deprecated( ch, STAT_LUC ) *80) /(MAX_STAT_VALUE/9) ), 80);
		chance += URANGE(20, ch->level*2 ,60);
		chance = URANGE(0, chance - ((paf->level * 10) / MAX_POISON), 100);
		break;
	}
	return chance;
}

int get_flags_cure_poison(int spell_level)
{
	switch(spell_level)
	{
	case 0: //lesser - najs³absze trucizny
		return POISON_SLOWERED | POISON_WEAK | POISON_NORMAL;
		break;
	case 1: //normal - wszystkie, oprócz paralizinig, strongest i death
		return POISON_SLOWERED | POISON_WEAK | POISON_NORMAL | POISON_STRONG
		| POISON_WEAKEN | POISON_SLEEP
		| POISON_BLIND;
		break;
	case 2: //wszystko
		return POISON_SLOWERED | POISON_WEAK | POISON_NORMAL | POISON_STRONG | POISON_STRONGEST
		| POISON_WEAKEN | POISON_SLEEP
		| POISON_BLIND | POISON_PARALIZING | POISON_DEATH ;
		break;
	}
	return 0;
}
void core_cure_poison(int chance, CHAR_DATA *ch, CHAR_DATA *victim, AFFECT_DATA *paf, int flags)
{
	if(number_range(0, 30) > chance)
	{
		send_to_char( "Trucizna wzi±¿ p³ynie w twoich ¿y³ach.\n\r", victim );
		act( "$n jest ci±gle zatruty.", victim, NULL, NULL, TO_ROOM );
		return;
	}

	//100% udane
	affect_strip(victim, gsn_poison);
	AFFECT_DATA *pafp = affect_find(victim->affected, gsn_poison);
	if(!pafp)
	{
		send_to_char( "Lecznicze ciep³o wype³nia twoje cia³o.\n\r", victim );
		act( "$n wygl±da o wiele lepiej.", victim, NULL, NULL, TO_ROOM );
	}
	else
	{
		send_to_char( "Lecznicze ciep³o wype³nia twoje cia³o, ale niestety nie ca³a trucizna zosta³a zneutralizowana.\n\r", victim );
		act( "$n wygl±da o wiele lepiej, jednak ci±gle niezbyt dobrze.", victim, NULL, NULL, TO_ROOM );
	}
}

void spell_lesser_cure_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA *paf = affect_find(victim->affected, gsn_poison);
	if(!check_poison(ch, victim, paf))
		return;

	int chance = chance_cure_poison(ch, paf, 0);
	int flags = get_flags_cure_poison(0);
	core_cure_poison(chance, ch, victim, paf, flags);
}

void spell_greater_cure_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA *paf = affect_find(victim->affected, gsn_poison);
	if(!check_poison(ch, victim, paf))
		return;

	int chance = chance_cure_poison(ch, paf, 2);
	int flags = get_flags_cure_poison(2);
	core_cure_poison(chance, ch, victim, paf, flags);
}

void spell_cure_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	//http://code.killer.mud.pl/p/killer-mud/issues/3/
	//Zmiana "cure poison" - wprowadzenie zale¿no¶ci od lucka, levelu i elementu losowego - im wy¿sza trucizna, tym trudniej wyleczyæ.
	//TODO Wprowadzenie skracania czasu dzia³ania i zmniejszania mocy trutki (niepe³ne wyleczenie).

	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA *paf = affect_find(victim->affected, gsn_poison);
	if(!check_poison(ch, victim, paf))
		return;

	int chance = chance_cure_poison(ch, paf, 1);
	int flags = get_flags_cure_poison(2); //do momentu rozmieszczenia ksi±g 2, potem 1
	core_cure_poison(chance, ch, victim, paf, flags);
}

void spell_detect_hidden( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dur;

	if ( IS_AFFECTED( victim, AFF_DETECT_HIDDEN ) )
	{
		if ( victim == ch )
		{
			switch ( victim->sex )
			{
				case 0:
					send_to_char( "Jeste¶ ju¿ wystarczaj±co czujne. \n\r", ch );
					break;
				case 1:
					send_to_char( "Jeste¶ ju¿ wystarczaj±co czujny. \n\r", ch );
					break;
				default:
					send_to_char( "Jeste¶ ju¿ wystarczaj±co czujna. \n\r", ch );
					break;
			}
		}
		else
			act( "$N potrafi ju¿ wyczuwaæ ukryte formy ¿ycia.", ch, NULL, victim, TO_CHAR );
		return;
	}

	switch ( victim->class )
	{
		case CLASS_MAG:
			if ( !IS_NPC( ch ) )
			{
				if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
				{
					dur = level / 2;
				}
				else
				{
					dur = level / 4;
				}
			}
			else
			{
				dur = level / 4;
			}
			break;
		case CLASS_CLERIC:
		case CLASS_BARD:
			dur = level / 4;
			break;

		default: dur = URANGE( 3, level / 4, 5 );break;
	}


	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 3, 1 + dur );
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_DETECT_HIDDEN;
	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Twoja czujno¶æ wzrasta.\n\r", victim );
	if ( ch != victim )
		send_to_char( "Ok.\n\r", ch );
	return;
}

void spell_detect_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int dur;

	if ( IS_AFFECTED( victim, AFF_DETECT_INVIS ) )
	{
		if ( victim == ch )
			send_to_char( "Potrafisz przecie¿ dostrzegaæ to, co niewidzialne.\n\r", ch );
		else
			act( "$N potrafi dostrzegaæ to, co niewidzialne.", ch, NULL, victim, TO_CHAR );
		return;
	}

	switch ( victim->class )
	{
		case CLASS_MAG:
			if ( !IS_NPC( ch ) )
			{
				if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
				{
					dur = level / 2;
				}
				else
				{
					dur = level / 4;
				}
			}
			else
			{
				dur = level / 4;
			}
			break;
		case CLASS_CLERIC:
		case CLASS_BARD:
			dur = level / 4;
			break;

		default: dur = URANGE( 3, level / 4, 5 );break;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 3, 1 + dur );
	af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_DETECT_INVIS;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Twoje oczy wibruj±.\n\r", victim );

	if ( ch != victim )
		send_to_char( "Ok.\n\r", ch );
	return;
}

/* modified for enhanced use */
void spell_dispel_magic( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA * aff;
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	AFFECT_DATA	* spells_found[ 12 ];
	int spells_found_count = 0, i, gsn;

	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;

		if ( IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
		{
			send_to_char( "Ten przedmiot jest odporny na magiê.\n\r", ch );
			return;
		}

		for ( aff = obj->affected; aff != NULL; aff = aff->next )
		{
			if ( aff->type <= 0 || aff->type >= MAX_SKILL )
				continue;
			if ( skill_table[ aff->type ].spell_fun == spell_null )
				continue;
			if ( aff->level > 40 )
				continue;

			affect_remove_obj( obj, aff );
			act( "Rozpraszasz zaklêcie wp³ywaj±ce na $h.", ch, obj, NULL, TO_CHAR );
			return;
		}

		//jak przedmiot niezniszczalny to moze zdjac tylko czesc affectow
		if ( IS_OBJ_STAT( obj, ITEM_UNDESTRUCTABLE ) )
		{
			if ( IS_OBJ_STAT( obj, ITEM_DARK ) )
			{
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_DARK );
				act( "Mroczna aura otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
				return;
			}

			if ( IS_OBJ_STAT( obj, ITEM_EVIL ) )
			{
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_EVIL );
				act( "Zimna aura z³a otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
				return;
			}

			if ( IS_OBJ_STAT( obj, ITEM_INVIS ) )
			{
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_INVIS );
				act( "Aura niewidzialno¶ci otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
				return;
			}

			if ( IS_OBJ_STAT( obj, ITEM_UNDEAD_INVIS ) )
			{
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_UNDEAD_INVIS );
				act( "Aura niewidzialno¶ci dla nieumar³ych otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
				return;
			}

			if ( IS_OBJ_STAT( obj, ITEM_ANIMAL_INVIS ) )
			{
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_ANIMAL_INVIS );
				act( "Aura niewidzialno¶ci dla zwierz±t otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
				return;
			}
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_DARK ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_DARK );
			act( "Mroczna aura otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_EVIL ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_EVIL );
			act( "Z³a aura otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_INVIS ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_INVIS );
			act( "Aura niewidzialno¶ci otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_UNDEAD_INVIS ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_UNDEAD_INVIS );
			act( "Aura niewidzialno¶ci dla nieumar³ych otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_ANIMAL_INVIS ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_ANIMAL_INVIS );
			act( "Aura niewidzialno¶ci dla zwierz±t otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_GLOW ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_GLOW );
			act( "$p ga¶nie.", ch, obj, NULL, TO_ALL );
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_BURN_PROOF ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_BURN_PROOF );
			act( "Odporno¶c na ogien $f zostaje rozproszona.", ch, obj, NULL, TO_ALL );
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
		{
			EXT_REMOVE_BIT( obj->extra_flags, ITEM_BLESS );
			act( "Aura b³ogos³awieñstwa otaczaj±ca $h ga¶nie.", ch, obj, NULL, TO_ALL );
			return;
		}

		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	victim = ( CHAR_DATA * ) vo;

	for ( i = 0; i < 12; i++ )
		spells_found[ i ] = 0;

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
		send_to_char( "Nic siê nie sta³o.\n\r", ch );
		return;
	}

	i = number_range( 0, spells_found_count - 1 );

	if ( level + number_range( 1, 20 ) <= 12 + spells_found[ i ] ->level )
	{
		send_to_char( "Nic siê nie sta³o.\n\r", ch );
		return;
	}

	gsn = spells_found[ i ] ->type;
	affect_strip( victim, gsn );

	if ( skill_table[ gsn ].msg_off )
	{
		send_to_char( skill_table[ gsn ].msg_off, victim );
		send_to_char( "\n\r", victim );
	}

	/* victim opada na ziemiê, je¿li pod wp³ywem czaru dispel magic straci
	 * si³ê na noszenie ca³ego swojego stuffu.
	 */
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

void spell_fireproof( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	OBJ_DATA *obj = ( OBJ_DATA * ) vo;

	if ( IS_OBJ_STAT( obj, ITEM_BURN_PROOF ) )
	{
		act( "$p ju¿ jest chroniony przed ogniem.", ch, obj, NULL, TO_CHAR );
		return;
	}

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = number_fuzzy( level / 4 );
	af.rt_duration = 0;
	af.location = APPLY_NONE;

	if ( !IS_NPC( ch ) && ch->class == CLASS_MAG && ch->pcdata->mage_specialist == 1 )
		af.modifier = 95;
	else
		af.modifier = 70;

	af.bitvector = &ITEM_BURN_PROOF;
	affect_to_obj( obj, &af );
	act( "Twoje czary zaczynaj± chroniæ $h przed sp³oniêciem.", ch, obj, NULL, TO_CHAR );
	act( "$p jest otoczony przez ochronn± aurê.", ch, obj, NULL, TO_ROOM );
	return;
}

void spell_faerie_fire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	if ( IS_AFFECTED( victim, AFF_FAERIE_FIRE ) )
		return;
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level	= level;
	af.duration = level;
	af.rt_duration = 0;
	af.location = APPLY_AC;
	af.modifier = 2 * level;
	af.bitvector = &AFF_FAERIE_FIRE;
	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Otaczasz siê ró¿ow± po¶wiat±.\n\r", victim );
	act( "$n otacza siê ró¿ow± po¶wiat±.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_null( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	send_to_char( "Niestety, to nie jest ¿aden czar!\n\r", ch );
	return;
}


/* RT plague spell, very nasty */
void spell_plague( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		act( "Na $C to chyba nie zadzia³a.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, gsn_plague ) )
	{
		switch ( victim->sex )
		{
			case 0:
				send_to_char( "Czujesz siê przez moment chore, ale to znika.\n\r", victim );
				break;
			case 1:
				send_to_char( "Czujesz siê przez moment chory, ale to znika.\n\r", victim );
				break;
			default:
				send_to_char( "Czujesz siê przez moment chora, ale to znika.\n\r", victim );
				break;
		}
		act( "$n nagle zaczyna wygl±daæ bardzo ¼le, jednak po chwili objawy choroby znikaj±.", victim, NULL, NULL, TO_ROOM );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level	= level * 3 / 4;
	af.duration = level;
	af.rt_duration = 0;
	af.location = APPLY_STR;
	af.modifier = -5;
	af.bitvector = &AFF_PLAGUE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Jêczysz z bólu gdy zaraza rozsadza twoje cia³o.\n\r", victim );
	switch ( victim->sex )
	{
		case 0:
			act( "$n jêczy z bólu gdy zaraza rozrywa tego cia³o.", victim, NULL, NULL, TO_ROOM );
			break;
		case 1:
			act( "$n jêczy z bólu gdy zaraza rozrywa jego cia³o.", victim, NULL, NULL, TO_ROOM );
			break;
		default:
			act( "$n jêczy z bólu gdy zaraza rozrywa jej cia³o.", victim, NULL, NULL, TO_ROOM );
			break;
	}
}

void spell_ray_of_truth ( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, align;

	if ( IS_EVIL( ch ) )
	{
		victim = ch;
		send_to_char( "Jaka¶ energia rozsadza twoje cia³o!\n\r", ch );
	}

	if ( victim != ch )
	{
		act( "$n wyci±ga d³oñ z której wylatuj± promienie energi!",
		     ch, NULL, NULL, TO_ROOM );
		send_to_char(
		    "Wyciagasz d³oñ z której wylatuj± promienie energi!\n\r",
		    ch );
	}

	if ( IS_GOOD( victim ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$n pozostaje niewzruszone.", victim, NULL, victim, TO_ROOM );
				break;
			case 1:
				act( "$n pozostaje niewzruszony.", victim, NULL, victim, TO_ROOM );
				break;
			default:
				act( "$n pozostaje niewzruszona.", victim, NULL, victim, TO_ROOM );
				break;
		}
		send_to_char( "Energia nie robi ci ¿adnej krzywdy.\n\r", victim );
		return;
	}

	dam = dice( level, 10 );
	if ( saves_spell( level, victim, DAM_HOLY ) )
		dam /= 2;

	align = victim->alignment;
	align -= 350;

	if ( align < -1000 )
		align = -1000 + ( align + 1000 ) / 3;

	dam = ( dam * align * align ) / 1000000;

	dam = luck_dam_mod( ch, dam );

	spell_damage( ch, victim, dam, sn, DAM_HOLY , TRUE );
	spell_blindness( gsn_blindness, 3 * level / 4, ch, ( void * ) victim, TARGET_CHAR );

	return;
}

void spell_remove_curse( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj;
	bool found = FALSE;

	/* do object cases first */
	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;

		if ( IS_OBJ_STAT( obj, ITEM_NODROP ) || IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
		{
			if ( !IS_OBJ_STAT( obj, ITEM_NOUNCURSE ) )
				/*	    &&  !saves_dispel(level + 2,obj->level,0))*/
			{
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_NODROP );
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_NOREMOVE );
				act( "$p rozb³yskuje b³êkitnym ¶wiat³em.", ch, obj, NULL, TO_ALL );
				return;
			}

			act( "Kl±twa na³o¿ona na $h jest dla ciebie zbyt potê¿na.", ch, obj, NULL, TO_CHAR );
			return;
		}
		act( "$p wydaje siê nie byæ pod wp³ywem jakiejkolwiek kl±twy.", ch, obj, NULL, TO_CHAR );
		return;
	}

	/* characters */
	victim = ( CHAR_DATA * ) vo;

	if ( check_dispel( level, victim, gsn_curse ) )
	{
		send_to_char( "Czujesz siê lepiej.\n\r", victim );
		act( "$n wygl±da lepiej.", victim, NULL, NULL, TO_ROOM );
	}

	for ( obj = victim->carrying; ( obj != NULL && !found ); obj = obj->next_content )
	{
		if ( ( IS_OBJ_STAT( obj, ITEM_NODROP ) || IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
		     && !IS_OBJ_STAT( obj, ITEM_NOUNCURSE ) )
		{   /* attempt to remove curse */
			if ( !saves_dispel( level, /*obj->level*/1, 0 ) )
			{
				found = TRUE;
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_NODROP );
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_NOREMOVE );
				act( "$p p³onie na niebiesko.", victim, obj, NULL, TO_CHAR );
				act( "$p $z p³onie na niebiesko.", victim, obj, NULL, TO_ROOM );
			}
		}
	}
}

/*
 * NPC spells.
 */

bool check_reflect_spell( CHAR_DATA *victim, sh_int circle )
{
	AFFECT_DATA * paf, *reflect = NULL;

	if ( !IS_AFFECTED( victim, AFF_REFLECT_SPELL ) )
		return FALSE;

	for ( paf = victim->affected; paf != NULL; paf = paf->next )
		if ( paf->bitvector == &AFF_REFLECT_SPELL )
		{
			reflect = paf;
			break;
		}

	if ( !reflect )
		return FALSE;

	/* ok to jesli jest to na pewno odbije
	 * jesli modifier < 0 to jest na stale
	 * w przeciwnym razie zmniejszamy modifier o circle
	 */

	circle = UMAX( 0, circle );

	/* na stale */
	if ( reflect->modifier < 0 )
		return TRUE;

	reflect->modifier -= circle;

	if ( reflect->modifier <= 0 )
		affect_remove( victim, reflect );
	return TRUE;
}

bool check_globes( CHAR_DATA *victim, sh_int circle )
{
	if ( IS_AFFECTED( victim, AFF_ABSOLUTE_MAGIC_PROTECTION ) && circle < 10 ) return TRUE;
	if ( IS_AFFECTED( victim, AFF_MAJOR_GLOBE ) && circle < 6 ) return TRUE;
	if ( IS_AFFECTED( victim, AFF_GLOBE ) && circle < 5 ) return TRUE;
	if ( IS_AFFECTED( victim, AFF_MINOR_GLOBE ) && circle < 4 ) return TRUE;
	return FALSE;
}

void summon_malfunction( CHAR_DATA *ch, int sn )
{
	CHAR_DATA * stworek;
	OBJ_DATA *obj;
    int vnum = 0;

    if ( IS_NPC( ch ) )
    {
        return;
    }

	//pierw szansa ze zamiast zlego moba zrobi jakis obiekt
	if ( number_range( 1, 3 ) == 1 && sn != gsn_made_undead && !( ch->class == CLASS_MAG && ch->pcdata->mage_specialist == 7 ) )
	{
		obj = create_object( get_obj_index( OBJ_VNUM_GLAZ ), FALSE );
		obj->timer = 10;
		obj_to_room( obj, ch->in_room );

		if ( ch->class == CLASS_MAG && ch->pcdata->mage_specialist == 1 )
		{
			act( "Nagle mylisz siê, kszta³towana przez ciebie materia wymyka siê z pod twej kontroli, rozb³yskuje ca³ym widmem kolorów i przy d¼wiêkach wy³adowañ elektrycznych przekszta³ca siê najpospolitszy na ¶wiecie g³az.", ch, NULL, NULL, TO_CHAR );
			act( "Nagle kszta³towana przez $c materia rozb³yska ca³ym widmem kolorów i przy d¼wiêkach wy³adowañ elektrycznych przekszta³ca siê najpospolitszy na ¶wiecie g³az.", ch, NULL, NULL, TO_ROOM );
		}
		else
		{
			act( "Niefortunnie mylisz kilka s³ów w zaklêciu.", ch, NULL, NULL, TO_CHAR );
			act( "$p pojawia siê.", ch, obj, NULL, TO_ALL );
		}
		return;
	}

	//dla druida
	if ( ch->class == CLASS_DRUID )
	{
		switch ( dice( 1, 4 ) )
		{
			case 1:
				vnum = MOB_VNUM_AURUMVORAX; //bestiariusz add str 9
				break;
			case 2:
				vnum = MOB_VNUM_TYRANOZAUR; //str 34
				break;
			case 3:
				vnum = MOB_VNUM_FENIX; //str 54
				break;
			case 4:
				vnum = MOB_VNUM_GARGANTUA; //str 57
				break;
		}
	} //dla nekrosa i animate dead by kler
	else if ( sn == gsn_made_undead || ( ch->class == CLASS_MAG && ch->pcdata->mage_specialist == 7 ) )
	{
		switch ( dice( 1, 4 ) )
		{
			case 1:
				vnum = MOB_VNUM_DRAKOLICZ; //str 37
				break;
			case 2:
				vnum = MOB_VNUM_DEMILICZ; //str 160
				break;
			case 3:
				vnum = MOB_VNUM_UPIORNY_RYCERZ; //str 327
				break;
			case 4:
				vnum = MOB_VNUM_WAMPIR; //str 329
				break;

		}
	} //dla przemian
	else if ( ch->class == CLASS_MAG && ch->pcdata->mage_specialist == 1 )
	{
		switch ( dice( 1, 4 ) )
		{
			case 1:
				vnum = MOB_VNUM_GREATER_GOLEM; //str 97
				break;
			case 2:
				vnum = MOB_VNUM_GARGULEC; //str 99
				break;
			case 3:
				vnum = MOB_VNUM_SKRIAXIT; //str 373
				break;
			case 4:
				vnum = MOB_VNUM_JUGGERNAUT; //str 100
				break;
		}
	}
	else
	{ //dla reszty (glownie summonera)
		switch ( dice( 1, 4 ) )
		{
			case 1:
				vnum = MOB_VNUM_ARGOS; //bestiariusz add str 8
				break;
			case 2:
				vnum = MOB_VNUM_ZIELONY_ABISHAI; //str 10
				break;
			case 3:
				vnum = MOB_VNUM_LAMMASU; //str 156
				break;
			case 4:
				vnum = MOB_VNUM_BALOR; //str 314
				break;
		}
	}

    if ( vnum == 0 )
    {
        log_string( "summon_malfunction: wrong vnum!" );
        return;
    }

	ch->counter[ 4 ] = 24;

	stworek = create_mobile( get_mob_index( vnum ) );

	char_to_room( stworek, ch->in_room );

	EXT_SET_BIT( stworek->act, ACT_NO_EXP );
	EXT_SET_BIT( stworek->act, ACT_AGGRESSIVE );

    money_reset_character_money( stworek );
	stworek->hit = get_max_hp( stworek );

	if ( ch->class == CLASS_MAG && ch->pcdata->mage_specialist == 1 )
	{
		act( "Nagle mylisz siê, kszta³towana przez ciebie materia wymyka siê z pod twej kontroli, rozb³yskuje ca³ym widmem kolorów i przy d¼wiêkach wy³adowañ elektrycznych zaczyna formowaæ naprawdê przera¿aj±ce kszta³ty!", ch, NULL, NULL, TO_CHAR );
		act( "Nagle kszta³towana przez $c materia rozb³yska ca³ym widmem kolorów i przy d¼wiêkach wy³adowañ elektrycznych zaczyna formowaæ naprawdê przera¿aj±ce kszta³ty!", ch, NULL, NULL, TO_ROOM );
	}
	else if ( sn == gsn_made_undead || ( ch->class == CLASS_MAG && ch->pcdata->mage_specialist == 7 ) )
	{
		EXT_SET_BIT( stworek->act, ACT_RAISED );
		act( "Nagle mylisz siê, kontrolowana przez ciebie negatywna energia wybucha i rozsadza o¿ywiane przez ciebie cia³o, które zaczyna formowaæ naprawdê przera¿aj±ce kszta³ty!", ch, NULL, NULL, TO_CHAR );
		act( "Nagle kontrolowana przez $c negatywna energia wybucha i rozsadza o¿ywiane w³a¶nie cia³o, które zaczyna formowaæ naprawdê przera¿aj±ce kszta³ty!", ch, NULL, NULL, TO_ROOM );
	}
	else
	{
		act( "Niefortunnie mylisz kilka s³ów w zaklêciu.", ch, NULL, NULL, TO_CHAR );
		act( "$N pojawia siê.", ch, NULL, stworek, TO_ALL );
	}

	if ( HAS_TRIGGER( stworek, TRIG_ONLOAD ) )
		mp_onload_trigger( stworek );

	//na 10 tickow
	create_event( EVENT_EXTRACT_CHAR, 2400, stworek, NULL, 0 );
	return;
}

int	luck_dam_mod( CHAR_DATA *ch, int dam )
{
	int damage_modifier = number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) );
    /**
     * decrease for bad luck
     */
    if ( damage_modifier < 12 )
    {
        dam *= ( 78 + ( 2 * damage_modifier ) );
        dam /= 100;
        /**
         * extr bonus for very lucky one
         */
        if ( damage_modifier < 3 && number_percent() == 50 )
        {
            dam /= 2;
        }
    }
    /**
     * increse for good luck
     */
    else if ( damage_modifier > 14 )
    {
        dam *= ( 72 + ( 2 * damage_modifier ) );
        dam /= 100;
        /**
         * extr bonus for very lucky one
         */
        if ( damage_modifier > 25 && number_percent() == 50 )
        {
            dam *= 2;
        }
    }
    return dam;
}
/**
 * check_shaman_invoke
 *
 * funkcja sprawdzajaca czy uda³o siê naszemu szamanowi przywo³aæ duchy
 *
 */

bool check_shaman_invoke ( CHAR_DATA *ch, int sn )
{

	/*Sprawdzamy czy trzyma totem w drugiej ³apie, paskud */
   //OBJ_DATA *totem = get_eq_char( ch, WEAR_HOLD );

   /*Nasza umiejêtno¶æ 'invoke spirit' definiuje jak silne s± nasze czary i
     jak czêsto uda nam siê ich u¿yæ */
   int spirit_chance = get_skill(ch, gsn_invoke_spirit);

   if (get_skill( ch, gsn_invoke_spirit ) <= 0 )
   {
		return FALSE;
	}

   //print_char( ch, "Spirit chance bazowy: %d\n\r", spirit_chance );

	/*Je¶li nie jest NPC, je¶li trzyma co¶ w drugiej ³apie i to co¶ jest totemem, to sprawdzamy typ totemu i porównujemy ze szko³±
	if( !IS_NPC(ch) && totem != NULL && totem->item_type == ITEM_TOTEM )
		{
         if ( totem->value[0] == 1 && IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ))
         {
         spirit_chance += totem->value[1]*10; //Moc totemu wacha siê w granicach od 1 do 9.
         print_char( ch, "To jest totem rysia i szko³a nekromancji.\n\r");
         }
         else if ( totem->value[0] == 2 && IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ))
         {
         spirit_chance += totem->value[1]*10;
         print_char( ch, "To jest totem wilka i szko³a zauroczeñ.\n\r");
         }
         else if ( totem->value[0] == 4 && IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ))
         {
         spirit_chance += totem->value[1]*10;
         print_char( ch, "To jest totem niedzwiedzia i szko³a przemian.\n\r");
         }
         else if ( totem->value[0] == 8 && IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ))
         {
         spirit_chance += totem->value[1]*10;
         print_char( ch, "To jest totem rosomaka i szko³a inwokacji.\n\r");
         }
         else if ( totem->value[0] == 16 && IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ))
         {
         spirit_chance += totem->value[1]*10;
         print_char( ch, "To jest totem or³a i szko³a przemian.\n\r");
         }
		}

   print_char( ch, "Typ totemu: %d\n\r", totem->value[0] );
   print_char( ch, "Spirit chance z totemami: %d\n\r", spirit_chance );

   */

   /*Po pijanemu (w koñcu szaman wywodzi siê z rodzaju barbarzyñców) jako¶ lepiej nam to wychodzi */
   if ( ch->condition[ COND_DRUNK ] > DRUNK_FULL ) spirit_chance += 5;

   /*Je¶li jest pod dzia³aniem czaru 'Subdue Spirits' to ³atwiej udaje mu siê nawi±zywaæ kontakt */
   if ( is_affected( ch, 516 )) spirit_chance += 10;

   /*Mobom siê prawie zawsze udaje*/
   if ( IS_NPC(ch)) spirit_chance = 100;

   if (number_percent()<=URANGE(10, (spirit_chance+((get_curr_stat_deprecated(ch, STAT_WIS)-14)*2)), 95))
     {
        check_improve( ch, NULL, gsn_invoke_spirit, TRUE, 60 );
        return TRUE;
     }
   else
     {
     check_improve( ch, NULL, gsn_invoke_spirit, FALSE, 50 );
     return FALSE;
     }
}


/**
 * check_improve_strenth
 *
 * funkcja sprawdza czy *ch znajduje sie pod wplywem czarow
 * zwiekszajacych sile
 *
 */
bool check_improve_strenth ( CHAR_DATA *ch, CHAR_DATA *victim, bool verbose )
{
    if (
            is_affected( victim,  58 ) || /* giant strenght */
            is_affected( victim, 167 ) || /* strenght       */
            is_affected( victim, 288 ) || /* champion strenght  */
            is_affected( victim, 481 )    /* bull strenght  */
       )
    {
        if ( verbose )
        {
            if ( victim == ch )
            {
                send_to_char( "Jeste¶ ju¿ tak siln<&y/a/e> jak tylko potrafisz!\n\r", ch );
            }
            else
            {
                switch ( victim->sex )
                {
                    case SEX_NEUTRAL:
                        act( "$N nie mo¿e byæ ju¿ silniejsze.", ch, NULL, victim, TO_CHAR );
                        break;
                    case SEX_FEMALE:
                        act( "$N nie mo¿e byæ ju¿ silniejsza.", ch, NULL, victim, TO_CHAR );
                        break;
                    case SEX_MALE:
                    default:
                        act( "$N nie mo¿e byæ ju¿ silniejszy.", ch, NULL, victim, TO_CHAR );
                        break;
                }
            }
        }
        return TRUE;
    }
    return FALSE;
}

//rellik: components, sprawdza czy zaklêcie potrzebuje komponentu
bool spell_need_item( int sn, char *name )
{
  int i;
  for( i=0 ; spell_items_table[i].key_number != 0; i++ )
  {
    //je¶li komponent odnosi sie do naszego sn
    if ( spell_items_table[i].spell_number == sn )
    {
      //je¶li podana nazwa to musi te¿ j± spe³niaæ
    	if ( name )
    	{
    		if ( !str_cmp( spell_items_table[i].spell_name, name ) ) return TRUE;
    	} else {
    		return TRUE;
    	}
    }
  }
  return FALSE;
}

//rellik: components, podaje slot komponentu po kluczu, zwraca -1 je¶li nie znaleziono
int spell_item_get_slot_by_key( int key )
{
	int i;
	for ( i = 0; spell_items_table[i].key_number != 0; ++i )
	{
		if ( spell_items_table[i].key_number == key ) return i;
	}
	return -1;
}

//rellik: components, podaje slot komponentu po nazwie obiektu, zwraca -1 je¶li nie znaleziono
int spell_item_get_slot_by_item_name( OBJ_DATA *obj )
{
    DEBUG_INFO ( "magic.c:spell_item_get_slot_by_item_name" );

    int i;
    for ( i = 0 ; spell_items_table[i].key_number != 0; ++i )
        if ( is_name( spell_items_table[i].spell_item_name, obj->name ) )
            return i;

    return -1;
}

//rellik: components, sprawdza czy ch wie o przydatno¶ci slotu do czaru
bool spell_item_know_key( CHAR_DATA *ch, int key )
{
  return chk_bigflag( &ch->pcdata->spell_items_knowledge, key );
}

bool spell_item_special_requirements( OBJ_DATA *komp, int key )
{
	int slot;
	CHAR_DATA *ch = NULL;
	ROOM_INDEX_DATA *room = NULL;

	if ( !komp )
	{
		save_debug_info( "magic.c=>spell_item_special_requirements", NULL, "komponent jest NULL", DEBUG_PROJECT_SPELL_ITEMS, DEBUG_LEVEL_ERROR, FALSE );
		return FALSE;
	}

	if ( komp->carried_by )
	{
		ch = komp->carried_by;
		room = ch->in_room;
	} else {
		save_debug_info( "magic.c=>spell_item_special_requirements", NULL, "niby komponentu nikt nie trzyma", DEBUG_PROJECT_SPELL_ITEMS, DEBUG_LEVEL_ERROR, FALSE );
		return FALSE;
	}

	slot = spell_item_get_slot_by_key( key );

	switch( key ) //tutaj specjalne warunki jakie musi spe³niaæ komponent
	{
	case 24: //kamienny pier¶cieñ, pomys³odawca ¿yczy³ sobie aby komponent by³ ubrany
		if ( komp->wear_loc != WEAR_NONE ) return TRUE;
		break;
	}
	return FALSE;
}

//rellik: components, czy ma komponent i czy go zna, je¶li tak to zwracany jest wska¼nik na niego lub na grupê (funkcja zakrêcona i nieelegancka ale skuteczna)
bool spell_item_has( CHAR_DATA *ch, int sn, char *sname, OBJ_DATA *items_group[31], int indeksy[MAX_COMPO_GROUP_SIZE] )
{
  OBJ_DATA *komponent;
  int i;
  const char *call = "magic.c => spell_item_has";
  int valid[ MAX_COMPO_GROUP_SIZE ];
  OBJ_DATA *found_gr[MAX_COMPO_GROUP_SIZE][MAX_COMPO_GROUP_SIZE]; //znalezione components 30 grup po 30 komponentow w grupie
  int indeks_gr[MAX_COMPO_GROUP_SIZE][MAX_COMPO_GROUP_SIZE]; //indeks slotu odpowiadaj±cy obiektowi
  OBJ_DATA *found[MAX_TOTAL_COMPO]; //znalezione components nie nale¿±ce do grup
  int indeks[MAX_TOTAL_COMPO]; //indeks slotu odpowiadaj±cy obiektowi found
  int count = 1;
  int gr_count = 0;
  int use;

  //print_char( ch, "dobra sprawdzamy czy ma komponent\n\r" );

  save_debug_info( call, NULL, NULL, DEBUG_PROJECT_SPELL_ITEMS, DEBUG_LEVEL_ALL, TRUE );

  for( i = 0; i < MAX_COMPO_GROUP_SIZE; ++i )
  {
    //wartosc <> 0 ¶wiadczy o aktualno¶ci grupy i jednocze¶nie stanowi indeks na kolejny element w grupie
    valid[i] = 1;
  }

  //lecimy przez wszystkie sloty komponentowo-czarowe
  for( i=0 ; spell_items_table[i].key_number != 0; ++i )
  {
    //print_char( ch, "Sprawdzam slot nr %d\n\r", i );

    //czy komponent odnosi siê do naszego czaru i w dodatku je¶li jest nazwa to siê zgadza
    if ( spell_items_table[i].spell_number == sn )
    {
    	//print_char( ch, "Komponent odnosi siê do naszego czaru.\n\r" );

      //czy podano nazwê do sprawdzenia?
      if ( sname && str_cmp( spell_items_table[i].spell_name, sname ) )
      {
      	//print_char( ch, "Podano nazwê do sprawdzenia i siê nie zgadza!!!\n\r" );

        //jak widaæ podano nazwê i sie ona nie zgadza wiêc to nie to czego szukamy;
        continue;
      }
      //czy wie o jego przydatno¶ci
      if ( !spell_item_know_key( ch, spell_items_table[i].key_number ) )
      {
      	//print_char( ch, "Kurwa nie wiem ¿e mam komponent do tego czaru.\n\r" );

        //a wiêc lamka nie wie, sprawd¼my czy nale¿y do grupy aby j± zdelegalizowaæ
        if ( spell_items_table[i].group_nb != 0 )
        {
            //nale¿y, a go nie mamy, wiêc trzeba zdelegalizowaæ t± grupê
            valid[spell_items_table[i].group_nb] = 0;
        }
        continue;
      }
      //czy ma komponent w inv, lecimy przez wszystko co ma
     	for ( komponent = ch->carrying; komponent; )
      {
     		//print_char( ch, "Sprawdzam w inv: %s\n\r", komponent->name );

        //czy nazwa siê zgadza
        if ( is_name( spell_items_table[i].spell_item_name, komponent->name ) )
        {
        	//print_char( ch, "Fajnie nazwa kompnentu zgadza siê ze wzorcem z tabeli.\n\r" );

          //czy aby jest zdatny do spo¿ycia
          if ( komponent->is_spell_item && komponent->spell_item_counter != 0 )
          {
          	//print_char( ch, "OKI, komponent jest zdatny do spozycia\n\r" );
            //jeszcze sprawd¼my wymagania specjalne
            if ( !spell_items_table[i].special_requirements  )
            {
            	//to wychodzimy z nim z pêtli
              break;
            } else if ( spell_item_special_requirements( komponent, spell_items_table[i].key_number ) )
            {
            	//spe³nia to te¿ wychodzimy z nim z pêtli
            	break;
            }
          }
        }
        komponent = komponent->next_content;
      }
      //je¶li nie znale¼li¶my komponenentu to sprawd¼my czy nale¿a³ do grupy aby j± zdelegalizowaæ
      if ( !komponent )
      {
      	//print_char( ch, "Niby nie znale¼li¶my komponentu w inv - kicha\n\r" );

        if ( spell_items_table[i].group_nb != 0 )
        {
            //nale¿y, a go nie mamy, wiêc trzeba zdelegalizowaæ t± grupê
            valid[spell_items_table[i].group_nb] = 0;
        }
        //i lecimy nastêpny slot z tabeli w pêtli
        continue;
      }

      //dobra, ma komponent i nie zawacha siê go u¿yæ
      //sprawd¼my czy slot nale¿y do grupy
      //print_char( ch, "Tutaj wiadomo ¿e mam komponent - coool!\n\r" );

      if ( spell_items_table[i].group_nb != 0 )
      {
      	//print_char( ch, "Komponent nale¿y do grupy.\n\r" );

        //je¶li grupa nie jest valid to i tak mo¿emy go olaæ
        if ( valid[ spell_items_table[i].group_nb ] == 0 )
        {
          //nie jest valid - lecimy dalej
          continue;
        }
        //wygl±da na to, ¿e grupa jest valid
        //nale¿y, to dopisujemy go jako kolejny komponent grupy
        found_gr[spell_items_table[i].group_nb][valid[spell_items_table[i].group_nb]] = komponent;
        indeks_gr[spell_items_table[i].group_nb][valid[spell_items_table[i].group_nb]] = i;
        //zwiêkszymy te¿ licznik
        ++valid[spell_items_table[i].group_nb];

        //sprawd¼my przekroczenie zakresu
        if ( valid[ spell_items_table[i].group_nb ] >= MAX_COMPO_GROUP_SIZE )
        {
          save_debug_info( call, NULL, "przekroczona ilo¶æ komponentów na grupê", DEBUG_PROJECT_SPELL_ITEMS, DEBUG_LEVEL_ERROR, FALSE );
          return FALSE;
        }
        //i mo¿emy lecieæ do nastêpnego slotu
        continue;
      }
      //nie nale¿y do grupy to dopisujemy go do listy singli
      //print_char( ch, "Zapisujemy komponent i indeks w tabeli slotów do tabeli pomocniczej, indeks jest %d.\n\r", i );

      found[count] = komponent;
      indeks[count] = i;
      ++count;
    }
  }
  //tutaj powinni¶my mieæ kompletne grupy i wszystkie single
  //singli jest ( count - 1 ), policzmy grupy
  for( i = 1; i < MAX_COMPO_GROUP_SIZE; ++i )
  {
    if ( valid[i] != 0 && valid[i] != 1 ) ++gr_count;
  }
  //print_char( ch, "Policzonych grup jest %d\n\r", gr_count );

  //sprawd¼my czy cokolwiek ma
  if ( count - 1 + gr_count == 0 )
  {
  	//print_char( ch, "Co¶ wysz³o ¿e nic nie ma jako komponent.\n\r" );

    //sorry gienia ¶wiat siê zmienia
    return FALSE;
  }
  //dobra, teraz wylosujemy czego u¿yjemy do czaru
  use = number_range( 1, count -1 + gr_count );
  //czy wylosowa³ siê single?
  if ( use < count )
  {
  	//print_char( ch, "Wylosowa³ siê single, %d.\n\r", use );

    items_group[1] = found[use];
    items_group[2] = NULL;
    indeksy[1] = indeks[use];
  } else {
    //wylosowa³a siê grupa ( use - ( count - 1 ) ) mówi nam która z valid grup ale nie znamy jej indeksu...
    //sprawd¼my
    gr_count = 0;
    for( i = 1; i < 31; ++i )
    {
      if ( valid[i] != 0 && valid[i] != 1 ) ++gr_count;
      if ( gr_count == ( use - ( count - 1 ) ) )
      {
        //znalezlismy nasz indeks
        break;
      }
    }
    //przechowajmy sobie nasz indeks w gr_count
    gr_count = i;
    if ( i == MAX_COMPO_GROUP_SIZE )
    {
      //uuuu.... cosik mocno nie teges
      save_debug_info( call, NULL, "przekroczony indeks wylosowanej grupy", DEBUG_PROJECT_SPELL_ITEMS, DEBUG_LEVEL_ERROR, FALSE );
      return FALSE;
    }

    for( i = 1; i < valid[gr_count]; ++i )
    {
      //wype³niamy naszymi komponenentami
      items_group[i] = found_gr[gr_count][i];
      indeksy[i] = indeks_gr[gr_count][i];
    }
    items_group[valid[gr_count]] = NULL;
  }
  //print_char( ch, "Znaczy ¿e spell_item_has = true\n\r" );
  return TRUE;
}

//rellik: components, czy ma wiedzê o u¿yteczno¶ci przedmiotu do zaklêcia
bool know_magic_of_item( CHAR_DATA *ch, OBJ_DATA *obj )
{
    DEBUG_INFO ( "magic.c:know_magic_of_item:start" );
    int i;

    for( i=0 ; spell_items_table[i].key_number != 0; i++ )
    {
        //je¶li nazwa komponentu pasuje do nazwy obiektu
        if ( is_name( spell_items_table[i].spell_item_name, obj->name ) )
            if ( chk_bigflag( &ch->pcdata->spell_items_knowledge, spell_items_table[i].key_number ) )
                return TRUE;
    }
    return FALSE;
}

bool spell_item_destroy( bool timer, OBJ_DATA *komp, int key )
{
	OBJ_DATA *ash;
	char buf[MAX_STRING_LENGTH];
	int slot;
	CHAR_DATA *ch = NULL;
	ROOM_INDEX_DATA *room;
	AFFECT_DATA *lpaf_next, *lpaf = NULL, *paf = NULL;

	DEBUG_INFO ( "magic.c:spell_item_destroy:start" );

	if ( !komp ) return FALSE;

	if ( komp->carried_by )
	{
		ch = komp->carried_by;
		room = ch->in_room;
	} else {
		room = komp->in_room;
	}

	slot = spell_item_get_slot_by_key( key );

	switch( key ) //je¶li chcesz aby twój koponent znika³ inaczej to wstaw go tutaj jako case nr_key_z_tabeli_spell_items_table
	{
        case 19: //podmiana li¶cia na zwiêdniêty je¶li skoñczy³ siê timer
        case 27:
            if ( timer )
            {
                if ( komp )
                {
                    ash = create_object( get_obj_index( 1675 ), FALSE );
                    if ( ch )
                    {
                        if ( komp->wear_loc != WEAR_NONE ) unequip_char( ch, komp );
                        obj_from_char( komp );
                        //tworzenie zwiêdniêtego wielkiego li¶cia
                        obj_to_char( ash, ch );
                    } else {
                        obj_from_room( komp );
                        obj_to_room( ash, room );
                    }
                    extract_obj( komp );
                }
            } else {
                break;
            }
            return TRUE;
            break;
        case 22: //mirror image, lusterko siê t³ucze
            if ( ch )
            {
                print_char( ch, "%s t³ucze siê na milion drobnych kawa³ków.\n\r", spell_items_table[slot].official_spell_item_name );
                if ( number_range( 1, 5 ) == 1 )
                {
                    buf[0] = '\0';
                    sprintf( buf, "Widzisz jak %s t³ucze siê na milion drobnych kawa³ków.", spell_items_table[slot].official_spell_item_name );
                    act( buf, komp->carried_by, NULL, NULL, TO_ROOM );
                }
            }
            break;
        case 23: //raise zombie, palec znika bo jest dodawany do ciala a jak po timerze to gnije
            if ( ch && timer )
            {
                print_char( ch, "{sTrupi palec{x gnije do reszty." );
            }
            break;
        case 24: //orb of entropy, pier¶cieñ ma traciæ afekty, jak to sobie laszlo ubzdura³
            if (( !timer ) && ( ch ) && ( komp->pIndexData ))
            {
                for ( paf = komp->pIndexData->affected; paf != NULL; paf = paf->next )
                {
                    if ( paf->location == APPLY_SPELL_AFFECT )
                    {
                        for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
                        {
                            lpaf_next = lpaf->next;
                            if ( ( lpaf->type == paf->type ) &&
                               ( lpaf->level == paf->level ) &&
                               ( lpaf->location == APPLY_SPELL_AFFECT ) )
                            {
                                affect_remove( ch, lpaf );
                                lpaf_next = NULL;
                            }
                        }
                    }
                    else
                    {
                      affect_modify( ch, paf, FALSE );
                      affect_check( ch, paf->where, paf->bitvector );
                    }
                }
                komp->enchanted = TRUE;
            }
            return TRUE;
        case 25: //Kawa³ki kory do liveoaka.
        case 26:
            if ( timer )
            {
                if ( komp )
                {
                    ash = create_object( get_obj_index( 42653 ), FALSE );
                    if ( ch )
                    {
                        if ( komp->wear_loc != WEAR_NONE ) unequip_char( ch, komp );
                        obj_from_char( komp );
                        //tworzenie spróchania³ych kawa³ków kory
                        obj_to_char( ash, ch );
                    } else {
                        obj_from_room( komp );
                        obj_to_room( ash, room );
                    }
                    extract_obj( komp );
                }
            } else {
                break;
            }
            return TRUE;
            break;
        case 28: //circle i ring of vanion, ¿o³±d¼ wielkiego dêbu znika po u¿yciu
        case 29:
            break;
        case 42: //animate dead - czaszka licza, po u¿yciu komponent jest niszczony.
        case 44: //animate dead - bandarze mumii, po u¿yciu komponent jest niszczony.
            break;
        case 45: //animate dead - upiorny py³, po u¿yciu lub up³yniêciu timera komponent jest niszczony.
            if ( ch && timer )
            {
                print_char( ch, "Upiorny py³ rozwiewa sie." );
            }
            break;
        case 46: //komponent do heal - fiolka z ³z± Isoryka, po u¿yciu znika.
            if ( ch )
            {
                print_char( ch, "%s znika nagle w jasnym rozb³ysku ¶wiat³a.\n\r", spell_items_table[slot].official_spell_item_name );
            }
            break;
        case 47: //Komponent do harma - czaszka licza, po u¿yciu komponent jest niszczony.
           break;

        default:
            if ( ch )
            {
                if ( number_range(1,2) == 1 )
                {
                    damage( ch, ch, dice(1,5), TYPE_UNDEFINED, DAM_FIRE, FALSE );
                    print_char( ch, "%s zaczyna {Rp³on±æ{x i sycz±c spala siê w wysokiej temperaturze.\n\r", spell_items_table[slot].official_spell_item_name );
                    if ( number_range( 1, 5 ) == 1 )
                    {
                        buf[0] = '\0';
                        sprintf( buf, "Widzisz jak %s zaczyna {Rp³on±æ{x i sycz±c spala siê w wysokiej temperaturze.", spell_items_table[slot].official_spell_item_name );
                        act( buf, ch, NULL, NULL, TO_ROOM );
                    }
                    ash = create_object( get_obj_index( OBJ_VNUM_ASH ), FALSE );
                    ash->timer = UMAX(3,number_percent()/10);
                    obj_to_char( ash, ch );
                } else {
                    print_char( ch, "%s {Brozb³yska{x i imploduje znikaj±c w bli¿ej nieokre¶lonym punkcie przestrzeni.\n\r", spell_items_table[slot].official_spell_item_name );
                    if ( number_range( 1, 5 ) == 1 )
                    {
                        buf[0] = '\0';
                        sprintf( buf, "Widzisz jak %s {Brozb³yska{x i imploduje znikaj±c w bli¿ej nieokre¶lonym punkcie przestrzeni.", spell_items_table[slot].official_spell_item_name );
                        act( buf, ch, NULL, NULL, TO_ROOM );
                    }
                }
            }
            break;
	}

	if ( komp )
	{
		if ( komp->carried_by )
		{
			if ( komp->wear_loc != WEAR_NONE ) unequip_char( komp->carried_by, komp );
			obj_from_char( komp );
		}
		extract_obj( komp );
	}
	return TRUE;
}

//rellik: components, funkcja obs³ugi komponentu przy rzucaniu czaru
bool spell_item_check( CHAR_DATA *ch, int sn, char *name )
{
    OBJ_DATA *components[MAX_COMPONENTS]; //grupa komponentów
    int ind[MAX_COMPONENTS]; //grupa indeksów
    int i;
    for( i = 0; i < MAX_COMPONENTS; ++i ) //Rellik powraca z za¶wiatów by debugowaæ kod! xD i prze³amuje wszystkie zabezpieczenia svn'a! xD
    {
        components[i] = NULL;
        active_spell_items.components[i] = NULL;
        active_spell_items.ind[i] = 0;
    }

    //npc nie potrzebuj± komponentów, mo¿na rozwa¿yæ potrzebê komponentu ale nie wiedzy o nim wiêc trzeba funkcjê sprawdzaj±c± tylko czy ma napisaæ
    if ( IS_NPC( ch ) ) return TRUE;

    //print_char( ch, "sprawd¼my czy potrzebuje\n\r" );
    if ( spell_need_item( sn, name ) )
    {
        //print_char( ch, "potrzebuje, a jak¿e\n\r" );
        //2. Sprawdzamy czy po kolei czy mamy wiedzê o komponencie i czy go mamy
        //w tabeli components bêdziemy mieli adresy obiektów a w ind ich indeksy w tabeli spell_items_table
        if ( spell_item_has( ch, sn, name, components, ind ) )
        {
            //ok, ma niezbêdne components
            //print_char( ch, "ok, ma niezbêdne components\n\r" );
            for( i = 1; i < MAX_COMPONENTS; ++i )
            {
                //czy koniec listy
                if ( !components[i] )
                {
                    break;
                }

                //dodatkowo za³adowuje do tabelki u¿ytych komponentów, tak na przysz³o¶æ
                active_spell_items.components[i] = components[i];
                active_spell_items.ind[i]= ind[i];

                //wysy³amy komunikat o u¿yciu
                print_char( ch, "%s\n\r", spell_items_table[ind[i]].action_description );
                if ( number_range(1,100) == 1 )
                {
                    act( spell_items_table[ind[i]].action_description, ch, NULL, NULL, TO_ROOM );
                }

                //zmniejszamy licznik
                if ( components[i]->spell_item_counter > 0 ) --components[i]->spell_item_counter;

                //czy siê wyczerpa³
                if ( components[i]->spell_item_counter == 0 )
                {
                    //to w takim razie komponentem ju¿ nie jest
                    components[i]->is_spell_item = FALSE;
                    //mo¿e jeszcze wybuchn±æ ma?
                    if ( spell_items_table[ind[i]].item_blows )
                    {
                        spell_item_destroy( FALSE, components[i], spell_items_table[ind[i]].key_number );
                    }
                }
            }
            //u¿y³ komponent(u/ów)
            //print_char( ch, "u¿y³ komponentu/ów\n\r" );
            return TRUE;
        }
        //nie ma komponentu
        send_to_char("Zaklêcie nie mo¿e uwolniæ ca³ej swojej energii, czujesz, ¿e czego¶ jeszcze brakuje.\n\r", ch );
        return FALSE;
    }
    //nie wymaga komponentu wiêc ok
    return TRUE;
}

void remove_memorized_spell( CHAR_DATA *ch, int sn )
{
    /**
     * sprawdzanie komu obciac z pamieci czar.
     * oszczedzamy immortali bo oni i tak nie musza pamietac.
     */
    if (
            IS_NPC( ch ) ||
            (
             !IS_NPC( ch ) && !IS_IMMORTAL( ch )
            )
       )
    {
        MSPELL_DATA * mspell;
        mspell = get_mspell_by_sn( ch, sn, TRUE );
        if ( mspell )
        {
            mspell->done = FALSE;
            mem_update_count( ch );
        }
    }
}

int mind_based_spell_bonus( CHAR_DATA *ch, int bonus )
{
    int damage = 0;
    if( IS_AFFECTED( ch, AFF_CONFUSION ) )
    {
        damage += bonus;
    }
    if( IS_AFFECTED( ch, AFF_DAZE ) )
    {
        damage += bonus;
    }
    if( IS_AFFECTED( ch, AFF_FEAR ) )
    {
        damage += bonus;
    }
    if( IS_AFFECTED( ch, AFF_HALLUCINATIONS_NEGATIVE ) )
    {
        damage += bonus;
    }
    if( IS_AFFECTED( ch, AFF_HALLUCINATIONS_POSITIVE ) )
    {
        damage += bonus;
    }
    if( IS_AFFECTED( ch, AFF_CHARM ) )
    {
        damage += bonus;
    }
    return damage;
}
int helper_do_cast_wait_wand_bonus ( CHAR_DATA *ch, int sn )
{
    int wait_bonus = 0;
    OBJ_DATA *wand;
    bool wand_first_value;

    wand = get_eq_char( ch, WEAR_HOLD );
    if( !IS_NPC(ch) && wand != NULL && wand->item_type == ITEM_WAND )
    {
        if( wand->value[0] == WAND_POSTWAIT )
        {
            wand_first_value = TRUE;
            if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
            {
                if( IS_GOOD( ch ) )
                    wait_bonus += wand->value[1];
                else
                    wait_bonus -= wand->value[1];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
            {
                if( IS_NEUTRAL( ch ) )
                    wait_bonus += wand->value[1];
                else
                    wait_bonus -= wand->value[1];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
            {
                if( IS_EVIL( ch ) )
                    wait_bonus += wand->value[1];
                else
                    wait_bonus -= wand->value[1];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
            {
                if( ch->class == CLASS_MAG )
                    wait_bonus += wand->value[1];
                else
                    wait_bonus -= wand->value[1];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
            {
                if( ch->class == CLASS_CLERIC )
                    wait_bonus += wand->value[1];
                else
                    wait_bonus -= wand->value[1];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
            {
                if( ch->class == CLASS_DRUID )
                    wait_bonus += wand->value[1];
                else
                    wait_bonus -= wand->value[1];
            }
            else
                wait_bonus += wand->value[1];
        }
        else if( wand->value[2] == WAND_POSTWAIT )
        {
            if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
            {
                if( IS_GOOD( ch ) )
                    wait_bonus += wand->value[3];
                else
                    wait_bonus -= wand->value[3];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
            {
                if( IS_NEUTRAL( ch ) )
                    wait_bonus += wand->value[3];
                else
                    wait_bonus -= wand->value[3];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
            {
                if( IS_EVIL( ch ) )
                    wait_bonus += wand->value[3];
                else
                    wait_bonus -= wand->value[3];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
            {
                if( ch->class == CLASS_MAG )
                    wait_bonus += wand->value[3];
                else
                    wait_bonus -= wand->value[3];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
            {
                if( ch->class == CLASS_CLERIC )
                    wait_bonus += wand->value[3];
                else
                    wait_bonus -= wand->value[3];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
            {
                if( ch->class == CLASS_DRUID )
                    wait_bonus += wand->value[3];
                else
                    wait_bonus -= wand->value[3];
            }
            else
                wait_bonus += wand->value[3];
        }
        else if( wand->value[4] == WAND_POSTWAIT )
        {
            if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_GOOD ) )
            {
                if( IS_GOOD( ch ) )
                    wait_bonus += wand->value[5];
                else
                    wait_bonus -= wand->value[5];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_NEUTRAL ) )
            {
                if( IS_NEUTRAL( ch ) )
                    wait_bonus += wand->value[5];
                else
                    wait_bonus -= wand->value[5];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_ALIGN_EVIL ) )
            {
                if( IS_EVIL( ch ) )
                    wait_bonus += wand->value[5];
                else
                    wait_bonus -= wand->value[5];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_MAG ) )
            {
                if( ch->class == CLASS_MAG )
                    wait_bonus += wand->value[5];
                else
                    wait_bonus -= wand->value[5];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_CLERIC ) )
            {
                if( ch->class == CLASS_CLERIC )
                    wait_bonus += wand->value[5];
                else
                    wait_bonus -= wand->value[5];
            }
            else if( IS_SET( wand->value[6], WAND_EXTRA_CLASS_DRUID ) )
            {
                if( ch->class == CLASS_DRUID )
                    wait_bonus += wand->value[5];
                else
                    wait_bonus -= wand->value[5];
            }
            else
                wait_bonus += wand->value[5];
        }
    }

    if ( wait_bonus != 0 && !IS_NPC(ch) )
    {
        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
        {
            if ( !wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
        {
            if ( !wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
        {
            if ( !wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
        {
            if ( !wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
        {
            if ( !wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
        {
            if ( !wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
        {
            if ( !wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
        {
            if ( !wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_0 ) && !IS_SET( skill_table[ sn ].school, school_table[ 0 ].flag ) )
        {
            if ( wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_1 ) && !IS_SET( skill_table[ sn ].school, school_table[ 1 ].flag ) )
        {
            if ( wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_2 ) && !IS_SET( skill_table[ sn ].school, school_table[ 2 ].flag ) )
        {
            if ( wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_3 ) && !IS_SET( skill_table[ sn ].school, school_table[ 3 ].flag ) )
        {
            if ( wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_4 ) && !IS_SET( skill_table[ sn ].school, school_table[ 4 ].flag ) )
        {
            if ( wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_5 ) && !IS_SET( skill_table[ sn ].school, school_table[ 5 ].flag ) )
        {
            if ( wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_6 ) && !IS_SET( skill_table[ sn ].school, school_table[ 6 ].flag ) )
        {
            if ( wand_first_value )
                wait_bonus = 0;
        }

        if( IS_SET(wand->value[6], WAND_EXTRA_SCHOOL_7 ) && !IS_SET( skill_table[ sn ].school, school_table[ 7 ].flag ) )
        {
            if ( wand_first_value )
                wait_bonus = 0;
        }
    }
    return wait_bonus;
}

bool helper_make_portal( CHAR_DATA *ch, bool is_portal, int sn )
{
    OBJ_DATA *stone = get_eq_char( ch, WEAR_HOLD ), *portal;
    ROOM_INDEX_DATA *to_room, *from_room;
    int timer;
    /**
     * uciekaj jezeli nic nie trzyma oraz to nie jest kamien 
     */
    if ( stone == NULL || stone->item_type != ITEM_PORTAL )
    {
        send_to_char( "Nie masz potrzebnego przedmiotu do rzucenia tego czaru.\n\r", ch );
        return;
    }
    /**
     * eksploduj pusty kamien
     */
    if ( stone->value[0] < 1 )
    {
        if( stone->liczba_mnoga )
        {
            act( "$p p³on± i znikaj±!", ch, stone, NULL, TO_CHAR );
            act( "W rêkach $z p³on± $p.", ch, stone, NULL, TO_ROOM );
        }
        else
        {
            act( "$p p³onie i znika!", ch, stone, NULL, TO_CHAR );
            act( "W rêkach $z p³onie $p.", ch, stone, NULL, TO_ROOM );
        }
        spell_damage( ch, ch, dice( 3, 10 ), sn, DAM_FIRE, TRUE );
        /*artefact*/
        if ( is_artefact( stone ) )
        {
            extract_artefact( stone );
        }
        extract_obj( stone );
        return;
    }
    stone->value[0]--;
    from_room = ch->in_room;
    to_room   = get_room_index( stone->value[3] );

    act( "Wzywasz magiczne moce $f.", ch, stone, NULL, TO_CHAR );
    act( "$n wzywa magiczne moce $f.", ch, stone, NULL, TO_ROOM );

    if ( to_room == from_room || to_room == NULL )
    {
        act( "Nic sie jednak nie dzieje", ch, stone, NULL, TO_CHAR );
        return;
    }
    timer = 2 + ch->level / 5 + number_range( 0, 3 );
    /**
     * portal one
     */
    portal             = create_object( get_obj_index( OBJ_VNUM_PORTAL ), FALSE );
    portal->timer      = timer;
    portal->value[ 1 ] = stone->value[ 1];
    portal->value[ 2 ] = stone->value[ 2];
    portal->value[ 3 ] = to_room->vnum;
    obj_to_room( portal, from_room );
    act( "$p wyrasta spod ziemi.", ch, portal, NULL, TO_ROOM );
    act( "$p wyrasta przed tob±.", ch, portal, NULL, TO_CHAR );
    if ( is_portal )
    {
        return;
    }
    /**
     * portal two
     */
    portal             = create_object( get_obj_index( OBJ_VNUM_PORTAL ), FALSE );
    portal->timer      = timer / 2;
    portal->value[ 1 ] = stone->value[ 1];
    portal->value[ 2 ] = stone->value[ 2];
    portal->value[ 3 ] = from_room->vnum;
    obj_to_room( portal, to_room );
    if ( to_room->people != NULL )
    {
        act( "$p wyrasta spod ziemi.", to_room->people, portal, NULL, TO_ROOM );
        act( "$p wyrasta spod ziemi.", to_room->people, portal, NULL, TO_CHAR );
    }
}
/**
 * modyfikuj obrazenia wzgledem typu lokacji
 */
int damage_modifier_by_spell_type( int dam, int spell_type, CHAR_DATA *ch )
{
    switch( spell_type )
    {
        case SECT_WATER:
        case SECT_UNDERWATER:
            if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_THIRST ) )
            {
                dam *=  9;
                dam /= 10;
                dam--;
            }
            else if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ) )
            {
                dam *= 105;
                dam /= 100;
                dam++;
            }
            else if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_WATER ) )
            {
                dam *= 11;
                dam /= 10;
                dam++;
            }
            break;
    }
    return UMAX( 0, dam );
}
/**
 * modyfikuj obrazenia wzgledem fazy ksiezyca
 */
int damage_modifier_by_moon_phase( int dam )
{
    switch ( time_info.moon_phase )
    {
        case MOON_NEW_MOON:
            dam *= 90;
            dam--;
            break;
        case MOON_WAXING_CRESCENT:
        case MOON_WANING_CRESCENT:
            dam *= 95;
            dam--;
            break;
        case MOON_WAXING_GIBBOUS:
        case MOON_WANING_GIBBOUS:
            dam *= 105;
            dam++;
            break;
        case MOON_FULL_MOON:
            dam *= 110;
            dam++;
            break;
        default:
            dam *= 100;
            break;
    }
    dam /= 100;
    return UMAX( 0, dam );
}

/**
 * modyfikuj czas trwania czarow w zaleznosci od typu lokacji
 */
int duration_modifier_by_spell_type( int duration, int spell_type, CHAR_DATA *ch )
{
    int duration_orginal = duration;
    switch( spell_type )
    {
        case SECT_AIR:
            if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_AIR ) )
            {
                duration *= 125;
            }
            break;
        case SECT_COLD:
            if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_HOT ) )
            {
                duration *= 90;
            }
            else if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_COLD ) )
            {
                duration *= 120;
            }
            break;
        case SECT_HOT:
            if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_COLD ) )
            {
                duration *= 90;
            }
            else if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_HOT ) )
            {
                duration *= 120;
            }
            break;
        default:
            duration *=100;
            break;
    }
    if ( duration != duration_orginal )
    {
        duration /= 100;
    }
    return duration;
}

