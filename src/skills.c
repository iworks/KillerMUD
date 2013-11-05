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
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: skills.c 12023 2013-02-10 13:22:18Z raszer $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/12.02/src/skills.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "traps.h"
#include "tables.h"
#include "progs.h"
#include "lang.h"
#include "music.h"
#include "projects.h"
#include "money.h"
#include "mount.h"

//#define INFO

char *names_alias args( (CHAR_DATA *ch, int type, int val) );
sh_int get_caster args( (CHAR_DATA *ch) );
int  check_magic_attack args( (CHAR_DATA *ch, CHAR_DATA *vch, OBJ_DATA *weapon) );
void one_hit  args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool second ) );
int  parry_mod  args( (OBJ_DATA *weapon) );
void set_fighting args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int  find_door args( ( CHAR_DATA *ch, char *arg ) );
void raw_kill args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void heal_char args( ( CHAR_DATA *ch, CHAR_DATA *victim, sh_int value ) );
bool check_blind  args( ( CHAR_DATA *ch ) );
bool check_mirror_image args( (CHAR_DATA *victim, CHAR_DATA *ch) );
bool check_blink args( (CHAR_DATA *victim, CHAR_DATA *ch, bool unik) );
void mp_onload_trigger args( ( CHAR_DATA *mob ) );
void do_skin1(CHAR_DATA *ch, char *argument);
void check_defensive_spells args( (CHAR_DATA * ch, CHAR_DATA * victim ) );
bool check_increase_wounds args( (CHAR_DATA *victim, int dam_type, int *dam) );
void wait_obj_to_char args ( ( OBJ_DATA *obj, int delay, CHAR_DATA *ch ) ) ;
void message_when_experience_gain args( ( CHAR_DATA *ch, int experience ) );

struct learn_skill_chance_type
{
    int skill;
    int chance;
};

//ponizej struktura z szansami, jak mamy wykupione plusy, bez zmian w stosunku do oryginalu
struct learn_skill_chance_type learn_skill_chance[100] =
{
    {   0,	  3100	 },
    {   1,	  3100	 },
    {   2,	  3100	 },
    {   3,	  3100	 },
    {   4,	  2800	 },
    {   5,	  2790	 },
    {   6,	  2780	 },
    {   7,	  2780	 },
    {   8,	  2770	 },
    {   9,	  2760	 },
    {  10,	  2750	 },
    {  11,	  2740	 },
    {  12,	  2730	 },
    {  13,	  2720	 },
    {  14,	  2700	 },
    {  15,	  2600	 },
    {  16,	  2590	 },
    {  17,	  2580	 },
    {  18,	  2570	 },
    {  19,	  2560	 },
    {  20,	  2550	 },
    {  21,	  2540	 },
    {  22,	  2530	 },
    {  23,	  2520	 },
    {  24,	  2510	 },
    {  25,	  2400	 },
    {  26,	  2390	 },
    {  27,	  2380	 },
    {  28,	  2370	 },
    {  29,	  2360	 },
    {  30,	  2340	 },
    {  31,	  2330	 },
    {  32,	  2320	 },
    {  33,	  2310	 },
    {  34,	  2200	 },
    {  35,	  2190	 },
    {  36,	  2180	 },
    {  37,	  2170	 },
    {  38,	  2160	 },
    {  39,	  2150	 },
    {  40,	  2140	 },
    {  41,	  2130	 },
    {  42,	  1550	 },
    {  43,	  1500	 },
    {  44,	  1450	 },
    {  45,	  1400	 },
    {  46,	  1350	 },
    {  47,	  1300	 },
    {  48,	  1250	 },
    {  49,	  1200	 },
    {  50,	  1150	 },
    {  51,	  1100	 },
    {  52,	  1050	 },
    {  53,	  1010	 },
    {  54,	  990	  },
    {  55,	  970	  },
    {  56,	  950	  },
    {  57,	  930	  },
    {  58,	  910	  },
    {  59,	  870	  },
    {  60,	  790	  },
    {  61,	  760	  },
    {  62,	  730	  },
    {  63,	  700	  },
    {  64,	  670	  },
    {  65,	  650	  },
    {  66,	  620	  },
    {  67,	  590	  },
    {  68,	  560	  },
    {  69,	  540	  },
    {  70,	  510	  },
    {  71,	  490	  },
    {  72,	  460	  },
    {  73,	  440	  },
    {  74,	  420	  },
    {  75,	  390	  },
    {  76,	  370	  },
    {  77,	  350	  },
    {  78,	  330	  },
    {  79,	  310	  },
    {  80,	  290	  },
    {  81,	  270	  },
    {  82,	  260	  },
    {  83,	  240	  },
    {  84,	  220	  },
    {  85,	  210	  },
    {  86,	  200	  },
    {  87,	  180	  },
    {  88,	  170	  },
    {  89,	  160	  },
    {  90,	  150	  },
    {  91,	  140	  },
    {  92,	  130	  },
    {  93,	  120	  },
    {  94,	  120	  },
    {  95,	  110	  },
    {  96,	  110	  },
    {  97,	  100	  },
    {  98,	  100	  },
    {  99,	  100	  }
};

struct learn_skill_chance_type_two
{
    int skill;
    int chance;
};
//ponizej struktura z szansami, jak nie mamy wykupionych plusow
//na poczatku szansa 2x mniejsza niz
struct learn_skill_chance_type learn_skill_chance_two[100] =
{
    {   0,	  700	 }, 
    {   1,	  690	 },
    {   2,	  680	 },
    {   3,	  670	 },
    {   4,	  660	 },
    {   5,	  650	 },
    {   6,	  640	 },
    {   7,	  630	 },
    {   8,	  620	 },
    {   9,	  610	 },
    {  10,	  600	 }, //5x roznica
    {  11,	  450	 },
    {  12,	  435	 },
    {  13,	  430	 },
    {  14,	  420	 },
    {  15,	  410	 },
    {  16,	  400	 },
    {  17,	  380	 },
    {  18,	  360	 },
    {  19,	  340	 },
    {  20,	  320	 }, //8x roznica
    {  21,	  315	 },
    {  22,	  310	 },
    {  23,	  300	 },
    {  24,	  290	 },
    {  25,	  280	 },
    {  26,	  270	 },
    {  27,	  260	 },
    {  28,	  250    },
    {  29,	  240	 },
    {  30,	  230	 },
    {  31,	  225	 },
    {  32,	  220	 },
    {  33,	  215	 },
    {  34,	  210	 },
    {  35,	  205    },
    {  36,	  200	 },
    {  37,	  190	 },
    {  38,	  180    },
    {  39,	  170	 },
    {  40,	  160	 }, //15x roznica
    {  41,	  150	 },
    {  42,	  140	 },
    {  43,	  130	 },
    {  44,	  120    },
    {  45,	  110	 },
    {  46,	  100	 },
    {  47,	  90	 },
    {  48,	  80	 },
    {  49,	  70	 },
    {  50,	  60	 }, //20x roznica
    {  51,	  60	 },
    {  52,	  57	 },
    {  53,	  53	 },
    {  54,	  50	  },
    {  55,	  47	  },
    {  56,	  45	  },
    {  57,	  40	  },
    {  58,	  35	  },
    {  59,	  30	  },
    {  60,	  26	  }, //30x roznica
    {  61,	  24	  },
    {  62,	  22	  },
    {  63,	  20	  },
    {  64,	  19	  },
    {  65,	  18	  },
    {  66,	  17	  },
    {  67,	  16	  },
    {  68,	  15	  },
    {  69,	  14	  },
    {  70,	  13	  }, //40x roznica
    {  71,	  11	  },
    {  72,	  10   	  },
    {  73,	  9	      },
    {  74,	  8	      },
    {  75,	  8	      },
    {  76,	  7	      },
    {  77,	  7	      },
    {  78,	  6	      },
    {  79,	  6       }, 
    {  80,	  5	      }, //60x roznica
    {  81,	  5	      },
    {  82,	  4	      },
    {  83,	  4       },
    {  84,	  4	      },
    {  85,	  3       },
    {  86,	  3	      },
    {  87,	  3       },
    {  88,	  2	      },
    {  89,	  2 	  },
    {  90,	  2      }, //80x roznica
    {  91,	  2      },
    {  92,	  2       },
    {  93,	  2  	  },
    {  94,	  2  	  },
    {  95,	  2  	  },
    {  96,	  1 	  },//100x ponad
    {  97,	  1 	  },
    {  98,	  1 	  },
    {  99,	  1 	  } 
};

void add_wait_after_miss(CHAR_DATA *ch, int multi)
{
#ifdef ENABLE_WAIT_AFTER_MISS
	WAIT_STATE( ch, multi * PULSE_VIOLENCE );
#endif
}


bool form_check( CHAR_DATA *ch, CHAR_DATA *vch, OBJ_DATA* obj, sh_int sn )
{
    char buf[ MAX_STRING_LENGTH ];
    if ( sn == gsn_backstab )
    {
        /* skoro czyma broñ czyli raczej dwuno¿ny i ¶wiadomy */
        if ( !IS_NPC( vch ) || IS_SET( race_table[ GET_RACE( vch ) ].wear_flag, ITEM_WIELD ) )
            return TRUE;

        if ( IS_SET( vch->form, FORM_MIST ) || IS_SET( vch->form, FORM_INTANGIBLE ) )
        {
            act( "Nie za bardzo wiesz gdzie uderzyæ aby zraniæ $N.", ch, obj, vch, TO_CHAR );
            return FALSE;
        }
        if ( IS_SET( vch->form, FORM_CRUSTACEAN ) )
        {
            act( "Skorupa $Z jest zbyt twarda by próbowaæ tego manewru.", ch, obj, vch, TO_CHAR );
            return FALSE;
        }
        if ( IS_SET( vch->form, FORM_CONSTRUCT ) )
        {
            act( "Taki cios nie wyjdzie ci raczej w walce z $V.", ch, obj, vch, TO_CHAR );
            return FALSE;
        }
        if ( IS_SET( vch->form, FORM_FISH ) && vch->hit > 0 )
        {
            act( "Wykonujesz skomplikowane ruchy by d¼gn±æ $C, ale co¶ ci nie wychodzi.", ch, obj, vch, TO_CHAR );
            return FALSE;
        }
    }
    else if ( sn == gsn_stun )
    {
        if ( IS_SET( vch->form, FORM_MIST ) || IS_SET( vch->form, FORM_INTANGIBLE ) || IS_AFFECTED( vch, AFF_PASS_DOOR ) || IS_SET( vch->form, FORM_BLOB ))
        {
            act( "$n bior±c potê¿ny zamach rzuca siê na $C, ale przelatuje przez $A jak przez dym.", ch, NULL, vch, TO_NOTVICT );
            act( "Bierzesz potê¿ny zamach i rzucasz siê na $C, ale przelatujesz przez $A jak przez dym.", ch, NULL, vch, TO_CHAR );
            act( "$n bior±c potê¿ny zamach rzuca siê na ciebie, ale przelatuje przez ciebie jak przez dym.", ch, NULL, vch, TO_VICT );
            return FALSE;
        }

        if ( IS_SET( vch->form, FORM_BIRD ) && vch->hit > 0 )
        {
            act( "$n bior±c potê¿ny zamach rzuca siê na $C, ale $N szybko odfruwa na bok.", ch, NULL, vch, TO_NOTVICT );
            act( "Bierzesz potê¿ny zamach i rzucasz siê na $C, ale $N szybko odfruwa na bok.", ch, NULL, vch, TO_CHAR );
            act( "$n bior±c potê¿ny zamach rzuca siê na ciebie, ale szybko odfruwasz na bok.", ch, NULL, vch, TO_VICT );
            return FALSE;
        }

        if ( vch->size < SIZE_SMALL )
        {
            switch ( vch->sex )
            {
                case SEX_FEMALE:
                    sprintf( buf, "ma³a" );
                    break;
                case SEX_NEUTRAL:
                    sprintf( buf, "ma³e" );
                    break;
                case SEX_MALE:
                default:
                    sprintf( buf, "ma³y" );
                    break;
            }
            act( "$n bior±c potê¿ny zamach rzuca siê na $C, ale $N jest zbyt $t by w $A trafiæ.", ch, buf, vch, TO_NOTVICT );
            act( "Bierzesz potê¿ny zamach i rzucasz siê na $C, ale $N jest zbyt $t by w $A trafiæ.", ch, buf, vch, TO_CHAR );
            act( "$n bior±c potê¿ny zamach rzuca siê na ciebie, ale $N jest zbyt $t by w $A trafiæ.", ch, buf, vch, TO_VICT );
            return FALSE;
        }
    }
    else if ( sn == gsn_bash )
    {
        if ( IS_SET( vch->form, FORM_MIST ) || IS_SET( vch->form, FORM_INTANGIBLE ) )
        {
            act( "$n próbuje powaliæ $C uderzeniem ca³ego cia³a, ale przelatuje przez $A jak przez dym.", ch, NULL, vch, TO_NOTVICT );
            act( "Próbujesz powaliæ $C uderzeniem ca³ego cia³a, ale przelatujesz przez $A jak przez dym.", ch, NULL, vch, TO_CHAR );
            act( "$n próbuje powaliæ ciê uderzeniem ca³ego cia³a, ale przelatuje przez ciebie jak przez dym.", ch, NULL, vch, TO_VICT );
            return FALSE;
        }
        if ( IS_SET( vch->form, FORM_BIRD ) && vch->hit > 0 )
        {
            act( "$n próbuje powaliæ $C uderzeniem ca³ego cia³a, ale $N szybko odfruwa na bok.", ch, NULL, vch, TO_NOTVICT );
            act( "Próbujesz powaliæ $C uderzeniem ca³ego cia³a, ale $N szybko odfruwa na bok.", ch, NULL, vch, TO_CHAR );
            act( "$n próbuje powaliæ ciê uderzeniem ca³ego cia³a, ale szybko odfruwasz na bok.", ch, NULL, vch, TO_VICT );
            return FALSE;
        }
        if ( IS_SET( vch->form, FORM_SNAKE ) )
        {
            act( "Chcia³by¶ powaliæ $C? Ciekawy pomys³.", ch, NULL, vch, TO_CHAR );
            return FALSE;
        }
        if ( vch->size < SIZE_SMALL )
        {
            if ( vch->sex == 2 )
                sprintf( buf, "ma³a" );
            else if ( vch->sex == 1 )
                sprintf( buf, "ma³y" );
            else
                sprintf( buf, "ma³e" );
            act( "$n próbuje powaliæ $C uderzeniem ca³ego cia³a, ale $N jest zbyt $t by w $A trafiæ.", ch, buf, vch, TO_NOTVICT );
            act( "Próbujesz powaliæ $C uderzeniem ca³ego cia³a, ale $N jest zbyt $t by w $A trafiæ.", ch, buf, vch, TO_CHAR );
            act( "$n próbuje powaliæ ciê uderzeniem ca³ego cia³a, ale $N jest zbyt $t by w $A trafiæ.", ch, buf, vch, TO_VICT );
            return FALSE;
        }
        if ( vch->size > SIZE_HUGE )
        {
            if ( vch->sex == 2 )
                sprintf( buf, "du¿a" );
            else if ( vch->sex == 1 )
                sprintf( buf, "du¿y" );
            else
                sprintf( buf, "du¿e" );
            act( "$n próbuje powaliæ $C uderzeniem ca³ego cia³a, ale $N jest zbyt $t by cokolwiek da³o siê zdzia³aæ.", ch, buf, vch, TO_NOTVICT );
            act( "Próbujesz powaliæ $C uderzeniem ca³ego cia³a, ale $N jest zbyt $t by cokolwiek da³o siê zdzia³aæ.", ch, buf, vch, TO_CHAR );
            act( "$n próbuje powaliæ ciê uderzeniem ca³ego cia³a, ale $N jest zbyt $t by cokolwiek da³o siê zdzia³aæ.", ch, buf, vch, TO_VICT );
            return FALSE;
        }
    }
    else if ( sn == gsn_kick )
    {
        if ( IS_SET( vch->form, FORM_MIST ) || IS_SET( vch->form, FORM_INTANGIBLE ) )
        {
            act( "$n próbuje kopn±æ $C, ale przelatuje przez $A jak przez dym.", ch, NULL, vch, TO_NOTVICT );
            act( "Próbujesz kopn±æ $C, ale przelatujesz przez $A jak przez dym.", ch, NULL, vch, TO_CHAR );
            act( "$n próbuje kopn±æ ciê, ale przelatuje przez ciebie jak przez dym.", ch, NULL, vch, TO_VICT );
            return FALSE;
        }

        if ( IS_SET( vch->form, FORM_BIRD ) && vch->hit > 0 && vch->position > POS_SITTING )
        {
            act( "$n próbuje kopn±æ $C, ale $N szybko odfruwa na bok.", ch, NULL, vch, TO_NOTVICT );
            act( "Próbujesz kopn±æ $C, ale $N szybko odfruwa na bok.", ch, NULL, vch, TO_CHAR );
            act( "$n próbuje ciê kopn±æ, ale szybko odfruwasz na bok.", ch, NULL, vch, TO_VICT );
            return FALSE;
        }
    }
    else if ( sn == gsn_trip )
    {
        if ( !IS_SET( vch->form, FORM_BIPED) )
        {
            act( "Z $V raczej nie wyjdzie ci ta sztuczka.", ch, obj, vch, TO_CHAR );
            return FALSE;
        }

    }
    else if ( sn == gsn_charge )
    {
        if ( vch->size == SIZE_TINY )
        {
            send_to_char( "Nie uda ci siê zaszar¿owaæ w co¶ tak ma³ego.\n\r", ch );
            return FALSE;
        }
    }
    return TRUE;
}

int crowd_check( CHAR_DATA *ch, CHAR_DATA *victim )
{
    CHAR_DATA * tch;
    int crowd = 0;

    //jesli victim nie walczy to spoko, nie ma tloku
    if ( !victim->fighting )
        return 0;

    for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
        if ( tch->fighting == victim && tch != ch )
            crowd++;
    return crowd;
}


void showComponentsForSpell(CHAR_DATA *ch, int sn)
{
    int x;

    char buf[ MAX_STRING_LENGTH ];
    char bufik[ MAX_STRING_LENGTH ];


    int gr[MAX_COMPO_GROUP_SIZE][MAX_COMPO_GROUP_SIZE];
    int i, c = 0;
    for(x = 0;x < MAX_COMPO_GROUP_SIZE;++x){
        gr[x][0] = 0;
    }
    print_char(ch, "Komponenty, które znasz do zaklêcia %s:\n\r", skill_table[sn].name);
    for(x = 0;spell_items_table[x].key_number != 0;++x){
        if(spell_items_table[x].spell_number == sn && chk_bigflag(&ch->pcdata->spell_items_knowledge, spell_items_table[x].key_number)){
            if(spell_items_table[x].group_nb != 0){
                ++gr[spell_items_table[x].group_nb][0];
                gr[spell_items_table[x].group_nb][gr[spell_items_table[x].group_nb][0]] = x;
                continue;
            }else{
                ++c;
            }
            print_char(ch, "%d. %s\n\r", c, spell_items_table[x].official_spell_item_name);
        }

    }

    for(x = 0;x < 31;++x){
        if(gr[x][0] > 0){
            bufik[0] = '\0';
            ++c;
            sprintf(buf, "%s", spell_items_table[gr[x][1]].official_spell_item_name);
            strcat(bufik, buf);
            for(i = 2;i <= gr[x][0];++i){
                sprintf(buf, "; %s", spell_items_table[gr[x][i]].official_spell_item_name);
                strcat(bufik, buf);
            }
            print_char(ch, "%d. %s\n\r", c, bufik);
        }

    }

    if(c == 0){
        send_to_char("Komponenty? Jakie komponenty?", ch);
        send_to_char("\n\r", ch);
    }
}

void do_spells( CHAR_DATA *ch, char *argument )
{
    BUFFER * buffer;
    int sn, x;
    int col = 0;
    bool test = FALSE;
    char buf[ MAX_STRING_LENGTH ];
    char bufik[ MAX_STRING_LENGTH ];
    int count = 0;

    if ( get_caster( ch ) < 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->position == POS_SLEEPING )
    {
        send_to_char( "¦ni ci siê, ¿e w³a¶nie wymysli³<&e/a/o>¶ potê¿ne zaklêcie.\n\r", ch );
        return;
    }

    //rellik: komponenty, pokazywanie jakie zna komponenty do danego zaklêcia
    if ( argument[0] != '\0' )
    {
        sn = spell_only_lookup(argument);
        if(sn == -1 || !knows_spell_book(ch, sn)){
            send_to_char("Nie znasz takiego zaklêcia.\n\r", ch);
            return;
        }
        showComponentsForSpell(ch, sn);
        return;
    }

    buffer = new_buf();
    sprintf( buf, "\n\r{b==<>==<>==<>==<>==<>==<>==<>==<> {GKsiêga Zaklêæ {b<>==<>==<>==<>==<>==<>==<>==<>=={x" );
    add_buf( buffer, buf );

    for ( x = 0;x < 9;x++ )
    {
        test = TRUE;
        col = 0;

        for ( sn = 0; sn < MAX_SKILL; sn++ )
        {
            if ( skill_table[ sn ].name == NULL ||
                    skill_table[ sn ].spell_fun == spell_null )
                continue;


            if ( knows_spell_book( ch, sn ) && spell_circle( ch, sn ) == x + 1 )
            {
                if ( test )
                {
                    sprintf( buf, "\n\r\n\r{GKr±g %d: {x", ( x + 1 ) );
                    add_buf( buffer, buf );
                }

                test = FALSE;

                if ( col > 2 )
                {
                    add_buf( buffer, "\n\r        " );
                    col = 0;
                }

                col++;
                if ( sn == gsn_create_symbol )
                {
                    if ( is_affected( ch, gsn_create_symbol ) )
                        sprintf( bufik, "*%-23s ", skill_table[ sn ].name );
                    else
                        sprintf( bufik, "%-24s ", skill_table[ sn ].name );
                }
                else if ( sn == gsn_holy_weapons )
                {
                    if ( is_affected( ch, gsn_holy_weapons ) )
                        sprintf( bufik, "*%-23s ", skill_table[ sn ].name );
                    else
                        sprintf( bufik, "%-24s ", skill_table[ sn ].name );
                }
                else if ( sn == gsn_energy_strike )
                {
                    if ( is_affected( ch, gsn_energy_strike ) )
                        sprintf( bufik, "*%-23s ", skill_table[ sn ].name );
                    else
                        sprintf( bufik, "%-24s ", skill_table[ sn ].name );
                }
                else if ( sn == gsn_shillelagh )
                {
                    if ( is_affected( ch, gsn_shillelagh ) )
                        sprintf( bufik, "*%-23s ", skill_table[ sn ].name );
                    else
                        sprintf( bufik, "%-24s ", skill_table[ sn ].name );
                }else if (sn == 402)
                 {
                      if ( is_affected( ch, sn ) )
                        sprintf( bufik, "*%-23s ", skill_table[ sn ].name );
                    else
                        sprintf( bufik, "%-24s ", skill_table[ sn ].name ); 
                }
                else
                    sprintf( bufik, "%-24s ", skill_table[ sn ].name );

                if( !IS_NPC(ch) && EXT_IS_SET( ch->act, PLR_STYL1 ) )
                {
                    switch( skill_table[ sn ].colour_style1 )
                    {
                        case SPELL_COLOUR_HIYELLOW:
                            sprintf( buf, "{Y%s{x", bufik );
                            break;
                        case SPELL_COLOUR_HIRED:
                            sprintf( buf, "{R%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWYELLOW:
                            sprintf( buf, "{y%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWRED:
                            sprintf( buf, "{r%s{x", bufik );
                            break;
                        case SPELL_COLOUR_HICYAN:
                            sprintf( buf, "{C%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWCYAN:
                            sprintf( buf, "{c%s{x", bufik );
                            break;
                        case SPELL_COLOUR_HIGREEN:
                            sprintf( buf, "{G%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWGREEN:
                            sprintf( buf, "{g%s{x", bufik );
                            break;
                        case SPELL_COLOUR_HIWHITE:
                            sprintf( buf, "{W%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWWHITE:
                            sprintf( buf, "{w%s{x", bufik );
                            break;
                        case SPELL_COLOUR_HIBLUE:
                            sprintf( buf, "{B%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWBLUE:
                            sprintf( buf, "{b%s{x", bufik );
                            break;
                        case SPELL_COLOUR_HIMAGNETA:
                            sprintf( buf, "{M%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWMAGNETA:
                            sprintf( buf, "{m%s{x", bufik );
                            break;
                        case SPELL_COLOUR_DARK:
                            sprintf( buf, "{D%s{x", bufik );
                            break;
                    }
                }
                else if( !IS_NPC(ch) && EXT_IS_SET( ch->act, PLR_STYL2 ) )
                {
                    switch( skill_table[ sn ].colour_style2 )
                    {
                        case SPELL_COLOUR_HIYELLOW:
                            sprintf( buf, "{Y%s{x", bufik );
                            break;
                        case SPELL_COLOUR_HIRED:
                            sprintf( buf, "{R%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWYELLOW:
                            sprintf( buf, "{y%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWRED:
                            sprintf( buf, "{r%s{x", bufik );
                            break;
                        case SPELL_COLOUR_HICYAN:
                            sprintf( buf, "{C%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWCYAN:
                            sprintf( buf, "{c%s{x", bufik );
                            break;
                        case SPELL_COLOUR_HIGREEN:
                            sprintf( buf, "{G%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWGREEN:
                            sprintf( buf, "{g%s{x", bufik );
                            break;
                        case SPELL_COLOUR_HIWHITE:
                            sprintf( buf, "{W%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWWHITE:
                            sprintf( buf, "{w%s{x", bufik );
                            break;
                        case SPELL_COLOUR_HIBLUE:
                            sprintf( buf, "{B%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWBLUE:
                            sprintf( buf, "{b%s{x", bufik );
                            break;
                        case SPELL_COLOUR_HIMAGNETA:
                            sprintf( buf, "{M%s{x", bufik );
                            break;
                        case SPELL_COLOUR_LOWMAGNETA:
                            sprintf( buf, "{m%s{x", bufik );
                            break;
                        case SPELL_COLOUR_DARK:
                            sprintf( buf, "{D%s{x", bufik );
                            break;
                    }
                }
                else if( !IS_NPC(ch) && EXT_IS_SET( ch->act, PLR_STYL3 ) )
                {
                    if( IS_SET( skill_table[ sn ].school, Odrzucanie ) )
                        sprintf( buf, "{W%s{x", bufik );
                    else if( IS_SET( skill_table[ sn ].school, Nekromancja ) )
                        sprintf( buf, "{D%s{x", bufik );
                    else if( IS_SET( skill_table[ sn ].school, Przemiany ) )
                        sprintf( buf, "{G%s{x", bufik );
                    else if( IS_SET( skill_table[ sn ].school, Przywolanie ) )
                        sprintf( buf, "{Y%s{x", bufik );
                    else if( IS_SET( skill_table[ sn ].school, Poznanie ) )
                        sprintf( buf, "{B%s{x", bufik );
                    else if( IS_SET( skill_table[ sn ].school, Zauroczenie ) )
                        sprintf( buf, "{M%s{x", bufik );
                    else if( IS_SET( skill_table[ sn ].school, Iluzje ) )
                        sprintf( buf, "{C%s{x", bufik );
                    else if( IS_SET( skill_table[ sn ].school, Inwokacje ) )
                        sprintf( buf, "{R%s{x", bufik );
                    else
                        sprintf( buf, "%s", bufik );

                }
                else // if( !IS_NPC(ch) && EXT_IS_SET( ch->act, PLR_STYL4 ) )
                    sprintf( buf, "%s", bufik );

                add_buf( buffer, buf );
                count++;
            }
        }
    }

    add_buf( buffer, "\n\r" );
    sprintf( buf, "\n\r{b==< {GLiczba znanych ci zaklêæ: %d{x{b ==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x\n\r", count );
    add_buf( buffer, buf );
    sprintf( buf, "Aby sprawdziæ, jakie komponenty znasz do zaklêcia u¿yj 'spells <nazwa_zaklêcia>'.\n\r");
    add_buf( buffer, buf );
    page_to_char( buf_string( buffer ), ch );
    free_buf( buffer );
    return;
}

void do_skills( CHAR_DATA *ch, char *argument )
{
    BUFFER * buffer;
    char arg[ MAX_INPUT_LENGTH ];
    char skill_list[ LEVEL_HERO + 1 ][ MAX_STRING_LENGTH ];
    char skill_columns[ LEVEL_HERO + 1 ];
    int sn, level, min_lev = 1, max_lev = LEVEL_HERO;
    bool fAll = FALSE, found = FALSE;
    char buf[ MAX_STRING_LENGTH ];
    bool na = FALSE, check;

    if ( ch->position == POS_SLEEPING )
    {
        int max_skill = 0, sn, skill, max_skill_sn = -1;

        for ( sn = 0; sn < MAX_SKILL; sn++ )
        {
            if ( skill_table[ sn ].spell_fun != spell_null )
                continue;

            skill = get_skill( ch, sn );

            if ( skill > max_skill )
            {
                max_skill = skill;
                max_skill_sn = sn;
            }
        }
        if ( max_skill_sn == - 1 )
            send_to_char( "¦nisz o swoich umiejêtno¶ciach.\n\r", ch );
        else
        {
            if (get_skill(ch, max_skill_sn) > 90 )
            {
                print_char( ch, "¦ni ci siê, ¿e stajesz siê arcymistrz<&em/yni±/em> w umiejêtno¶ci '%s'.\n\r", skill_table[ max_skill_sn ].name );
            }
            else {
                print_char( ch, "¦ni ci siê, ¿e stajesz siê mistrz<&em/yni±/em> w umiejêtno¶ci '%s'.\n\r", skill_table[ max_skill_sn ].name );
            }
        }
        return;
    }


    if ( IS_NPC( ch ) )
    {
        int col = 0;

        for ( sn = 0; sn < MAX_SKILL; sn++ )
        {
            if ( skill_table[ sn ].name == NULL )
            {
                break;
            }

            if ( skill_table[ sn ].spell_fun != spell_null || get_skill( ch, sn ) <= 0 )
            {
                continue;
            }

            print_char( ch, "%-15s %-18s", skill_table[ sn ].name, names_alias( ch, 0, sn ) );

            if ( ++col == 2 )
            {
                print_char( ch, "\n\r" );
                col = 0;
            }
        }
        return;
    }

    if ( argument[ 0 ] != '\0' )
    {
        fAll = TRUE;

        if ( str_prefix( argument, "all" ) )
        {
            argument = one_argument( argument, arg );
            if ( !is_number( arg ) )
            {
                send_to_char( "Podawane argumenty musz± byæ liczbami.\n\r", ch );
                return;
            }
            max_lev = atoi( arg );

            if ( max_lev < 1 || max_lev > LEVEL_HERO )
            {
                sprintf( buf, "Poziomy musz± byæ z zakresu 1 do %d.\n\r", LEVEL_HERO - 1 );
                send_to_char( buf, ch );
                return;
            }

            if ( argument[ 0 ] != '\0' )
            {
                argument = one_argument( argument, arg );
                if ( !is_number( arg ) )
                {
                    send_to_char( "Podawane argumenty musz± byæ liczbami.\n\r", ch );
                    return;
                }
                min_lev = max_lev;
                max_lev = atoi( arg );

                if ( max_lev < 1 || max_lev > LEVEL_HERO )
                {
                    sprintf( buf,
                            "Poziomy musz± byæ z zakresu 1 do %d.\n\r", LEVEL_HERO - 1 );
                    send_to_char( buf, ch );
                    return;
                }

                if ( min_lev > max_lev )
                {
                    send_to_char( "Nie mo¿na podaæ je¿eli minimum jest wiêksze od maximum.\n\r", ch );
                    return;
                }
            }
        }
    }

    send_to_char("\n\r{b==<>==<>==<>==<>==<>==<>==<> {GZnane ci umiejêtno¶ci {b<>==<>==<>==<>==<>==<>==<>=={x",ch);
    /* initialize data */
    for ( level = 0; level < LEVEL_HERO + 1; level++ )
    {
        skill_columns[ level ] = 0;
        skill_list[ level ][ 0 ] = '\0';
    }

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
        if ( skill_table[ sn ].name == NULL )
            break;
        if ( ( skill_table[ sn ].affect_desc != NULL ) &&
                (!str_cmp( skill_table[ sn ].affect_desc, "shapeshift" ) == TRUE) )
            continue; // przemiany druida wywalamy ze 'skills'

        if ( ( level = skill_table[ sn ].skill_level[ ch->class ] ) < LEVEL_HERO + 1
                && ( fAll || level <= ch->level )
                && level >= min_lev && level <= max_lev
                && skill_table[ sn ].spell_fun == spell_null
                && ch->pcdata->learned[ sn ] > 0 )
        {
            found = TRUE;
            level = skill_table[ sn ].skill_level[ ch->class ];
            /*		if (ch->level < level)
                    sprintf(buf,"%-15s n/a	  ", skill_table[sn].name);
                    else*/

            na = FALSE;

            if ( sn == gsn_berserk )
            {
                if ( ch->counter[ 3 ] > 0 )
                    na = TRUE;
            }
            else if ( sn == gsn_demon_aura )
            {
                if ( ch->counter[ 3 ] > 0 )
                    na = TRUE;
            }
            else if ( sn == gsn_call_avatar )
            {
                if ( is_affected( ch, gsn_call_avatar ) )
                    na = TRUE;
            }
            else if ( sn == gsn_control_undead )
            {
                if ( is_affected( ch, gsn_control_undead ) )
                    na = TRUE;
            }
            else if ( sn == gsn_smite )
            {
                if ( ch->counter[ 4 ] > 0 )
                    na = TRUE;
            }
            else if ( sn == gsn_smite_good )
            {
                if ( ch->counter[ 4 ] > 0 )
                    na = TRUE;
            }
            else if ( sn == gsn_recuperate )
            {
                if ( is_affected( ch, gsn_recuperate ) && !IS_AFFECTED( ch, AFF_RECUPERATE ) )
                    na = TRUE;
            }
            else if ( sn == gsn_lay )
            {
                if ( is_affected( ch, gsn_lay ) )
                    na = TRUE;
            }
            else if ( sn == gsn_torment )
            {
                if ( is_affected( ch, gsn_torment ))
                    na = TRUE;
            }
            else if ( sn == gsn_garhal )
            {
                if ( is_affected(ch,gsn_garhal) )
                    na = TRUE;
            }
            else if ( sn == gsn_meditation )
            {
                if ( is_affected( ch, gsn_meditation ) && !IS_AFFECTED( ch, AFF_MEDITATION ) )
                    na = TRUE;
            }
            else if ( sn == gsn_healing_touch )
            {
                if ( is_affected( ch, gsn_healing_touch ))
                    na = TRUE;
            }
            else if ( sn == gsn_lore )
            {
                if ( is_affected( ch, gsn_lore ) )
                    na = TRUE;
            }
            else if ( sn == gsn_lore_intuition )
            {
                if ( is_affected( ch, gsn_lore_intuition ) )
                    na = TRUE;
            }
            else if ( sn == gsn_healing_hands )
            {
                if ( is_affected( ch, gsn_healing_hands ) )
                    na = TRUE;
            }
            else if ( sn == gsn_turn )
            {
                check = FALSE;
                AFFECT_DATA * paf;
                if ((paf = affect_find( ch->affected, gsn_turn ) ) != NULL &&
                        ((!check&&
                          paf->level > ch->level / 5 )||
                         (paf->level > ch->level / 3 )))
                    na = TRUE;
            }
            else if ( sn == gsn_persuasion )
            {
                if ( is_affected( ch, gsn_persuasion ) )
                    na = TRUE;
            }
            else if ( sn == gsn_shapeshift )
            {
                if ( is_affected( ch, gsn_shapeshift ) )
                    na = TRUE;
            }
            else if ( sn == gsn_holy_prayer )
            {
                if ( is_affected( ch, gsn_holy_prayer ) )
                    na = TRUE;
            }
	    sprintf( buf, "%s%-14s %1s%-17s",
                        na ? "*" : "",
                        skill_table[ sn ].name,
                        ( ch->pcdata->learning_rasz[ sn ] > 0 ) ? "+" : " ",
                        names_alias( ch, 0, sn ) );
            if ( skill_list[ level ][ 0 ] == '\0' )
                sprintf( skill_list[ level ], "\n\r\n\r{wPoziom %2d:{x %s", level, buf );
            else /* append */
            {
                if ( ++skill_columns[ level ] % 2 == 0 )
                    strcat( skill_list[ level ], "\n\r           " );
                strcat( skill_list[ level ], buf );
            }
        }
    }

    /* return results */

    if ( !found )
    {
        send_to_char( "\n\r\n\rNie znasz ¿adnych umiejêtno¶ci.\n\r", ch );
        send_to_char("\n\r{b==<>==<>==<>==<>==<>==<>==<>==<>==<>==<=>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x\n\r",ch);
        return;
    }

    buffer = new_buf();
    for ( level = 0; level < LEVEL_HERO + 1; level++ )
    {
        if ( skill_list[ level ][ 0 ] != '\0' )
        {
            add_buf( buffer, skill_list[ level ] );
        }
    }
    add_buf( buffer, "\n\r" );
    add_buf( buffer, "\n\r{b==<>==<>==<>==<>==<>==<>==<>==<>==<>==<=>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x\n\r");
    page_to_char( buf_string( buffer ), ch );
    free_buf( buffer );
}

/* checks for skill improvement */
//wersja Raszera
void check_improve( CHAR_DATA *ch, CHAR_DATA *victim, int sn, bool success, int multiplier )
{
    char buf[ MAX_STRING_LENGTH ];
    int base_chance, chance_final, stat_int, stat_wis, chance, lvl_mod, lvl_diff, chance_stat_mod = 0;
    sh_int learned, learning = 0;
    bool debug = FALSE;

    /**
     * not for mobs
     */
    if ( IS_NPC( ch ) )
    {
        return;
    }

    /* ch odlecial po uzyciu skilla a przed improve */
    if ( ch->in_room == NULL )
    {
        return;
    }

    if ( victim && !IS_NPC( victim ) )
    {
        return;
    }

    /**
     * wyssanym i smierciozercom dziekujemy
     */
    if ( is_affected (ch, gsn_energy_drain) || is_affected (ch, gsn_playerdeathlearnpenalty ))
    {
        return;
    }

    /**
     * czary nie rosna
     */
    if ( skill_table[ sn ].spell_fun != spell_null )
    {
        return;
    }

    /**
     * nie swoje i nieznane, tez nie rosna
     */
    if ( ch->level < skill_table[ sn ].skill_level[ ch->class ] || ch->pcdata->learned[ sn ] == 0 )
    {
        return;
    }

    // w safe, nie bêdziemy siê uczyæ!
    if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
        return;
    }

    learned  = ch->pcdata->learned[ sn ];
    /**
     * if nothing to learn, he now everything
     */
    if ( learned > 99 ) //dajmy jednak szanse na arcy, premia dla tych co wkladaja max wysilku w jedna postac
    {
        return;
    }

    debug    = IS_IMMORTAL( ch );
    learning = ch->pcdata->learning_rasz[ sn ];
    lvl_mod = 0;

    if ( victim )
    {
       lvl_diff = victim->level - ch->level;
       if(lvl_diff < -(ch->level/2)) lvl_mod -= 25;
       if(lvl_diff < -(ch->level/3)) lvl_mod -= 15;
       if(lvl_diff < -(ch->level/4)) lvl_mod -= 10;
       if(lvl_diff < -(ch->level/5)) lvl_mod -= 10;

       if(lvl_diff > ch->level/5) lvl_mod += 10;
       if(lvl_diff > ch->level/4) lvl_mod += 10;
       if(lvl_diff > ch->level/3) lvl_mod += 15;

       if ( EXT_IS_SET( victim->act, ACT_BOSS ))   lvl_mod += 15;

    }


    /* check to see if the character has a chance to learn */
    stat_int = get_curr_stat( ch, STAT_INT );
    stat_wis = get_curr_stat( ch, STAT_WIS );
    chance_stat_mod = (stat_int+stat_wis)/2;
    chance_stat_mod = chance_stat_mod - 100;
    if(learning > 0)
    {
       base_chance = 10 * learn_skill_chance[ learned ].chance;
    }else
    {
       base_chance = 10 * learn_skill_chance_two[ learned ].chance;
    }
     if(debug) print_char( ch, "Liczba plusów: %d , poziom wyuczenia: %d\n\r", learning,  learned);
     if(debug) print_char( ch, "base chance: %d , stat_mod: %d\n\r", base_chance,  chance_stat_mod);
     chance = base_chance;
     chance *= 15;//po przeskalowaniu multiplierow + pierwsza balansacja + druga balansacja
     chance += (chance_stat_mod*chance)/100;
    if(debug) print_char( ch, "chance statmod: %d\n\r", chance );
     chance += (((get_curr_stat( ch, STAT_LUC )-100)*chance)/100)/2;
    if(debug) print_char( ch, "chance luckmod: %d\n\r", chance );
     chance += (lvl_mod*chance)/100;
    if(debug) print_char( ch, "chance lvlmod: %d\n\r", chance );
     if(!success)
     {
        chance = (90*chance)/100;
     }
     chance /= (URANGE(1, multiplier, 20));
     if(debug)print_char( ch, "chance multipl: %d\n\r", chance );

     chance_final = UMAX( 1, chance );

     if ( debug )
     {
         print_char( ch, "chance: %d ,na 70000\n\r", chance_final);
     }
     if ( number_range( 1, 70000 ) < chance_final )
     {
          sprintf
                (
                 buf,
                 "Stajesz siê lepsz%s w umiejêtno¶ci '%s'.\n\r",
                 ch->sex == 2 ? "a" : "y",
                 skill_table[ sn ].name
                );
          send_to_char( buf, ch );
          ch->pcdata->learned[ sn ]++;
          if(learning > 0) ch->pcdata->learning_rasz[ sn ]--;
          if(debug) print_char( ch, "Ilo¶æ plusów: %d , poziom wyuczenia: %d\n\r", ch->pcdata->learning_rasz[ sn ],  ch->pcdata->learned[ sn ]);
     }
     return;
}

void do_lay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim;
    AFFECT_DATA af, *paf;
    int value, skill, pskill = 0, charisma_mod = get_curr_stat_deprecated( ch, STAT_CHA ) - 18, holy_prayer_mod = 0;
    bool cure_po = FALSE, shorter_wait = FALSE, extended_wait = FALSE, lay_failed = FALSE, bonus_conditions = TRUE;
    bool lay_weakened = FALSE;

    if ( ( skill = get_skill( ch, gsn_lay ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Kogo chcesz uleczyæ?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        return;
    }

    if ( is_affected(ch,gsn_lay) )
    {
        send_to_char( "Nie mo¿esz siê wystarczaj±co skoncentrowaæ.\n\r", ch );
        return;
    }

    if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_SAFE ) )
    {
        send_to_char( "Zaczynasz zbieraæ w sobie ¿yciodajn± energiê, ale kto¶ przechodz±c obok, potr±ci³ ciê.\n\r", ch );
        return;
    }

    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
    {
        holy_prayer_mod = paf->modifier/10;
    }

    af.where       = TO_AFFECTS;
    af.type        = gsn_lay;
    af.level       = 50;//by nie szlo dispelnac
    af.duration    = UMAX(8, 30 - ch->level / 10 - holy_prayer_mod);
    af.rt_duration = af.duration/2;
    af.location    = APPLY_NONE;
    af.modifier    = 0;
    af.bitvector   = &AFF_NONE;
    af.visible     = FALSE;

    WAIT_STATE( ch, skill_table[ gsn_lay ].beats );
    value = 10 + ch->level * number_range( 12, 20 ) + 5*charisma_mod;
    value = ( 34 + ( 2 * skill ) / 3 ) * value / 100;

    // blokada braku modlitwy
    if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
        if ( paf->modifier > 48 )
        {
            send_to_char( "Starasz siê zebraæ w sobie ¿yciodajn± energie i od razu zdajesz sobie sprawê, ¿e moc uzdrawiania zosta³a ci zabrana.", ch );
            act( "$n wysuwa przed siebie d³onie i przymyka oczy... by po chwili je otwiera i z przera¿eniem spogl±da do góry.", ch, NULL, NULL, TO_ROOM );
            return;
        }
        else if ( paf->modifier > 36 )
        {
            send_to_char( "Zbieraj±c w sobie ¿yciodajn± energie wyczuwasz, ze co¶ jest nie tak.", ch );
            value /= 2;
            bonus_conditions = FALSE;
        }
    }

    // bonus/minus od bycia dobrym palem, zmiany Rasz_pray
    //warunki poczatkowe dla dobrych bonusow
    if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) || victim->position == POS_FIGHTING || victim->fighting != NULL)
    {
        bonus_conditions = FALSE;
    }

    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
    {
        pskill = get_skill( ch, gsn_holy_prayer );
        if (paf->modifier > 30 && number_percent() < pskill/3 && bonus_conditions == TRUE)
        {
            value += value/4;
            paf->modifier -= 10;
            act("Twoje d³onie otacza ¶wietlista aura, czujesz jak twoja uzdrowicielska moc ro¶nie.", ch, NULL, victim, TO_CHAR);
            act("Widzisz jak $n staje siê bardziej pewny siebie.", ch, NULL, victim, TO_NOTVICT);
            act("Widzisz jak $n staje siê bardziej pewny siebie.",ch, NULL, victim, TO_VICT);

        }
        if (paf->modifier > 60 && bonus_conditions == TRUE)
        {
            cure_po = TRUE;
        }
        if (paf->modifier > 80 && number_percent() < pskill/5 && bonus_conditions == TRUE)
        {
            af.duration  /= 2;
            shorter_wait = TRUE;
            paf->modifier -= 20;
        }
        if (paf->modifier < -150) { af.duration *= 2; extended_wait = TRUE;}
        if (paf->modifier < -100) { af.duration += 5; extended_wait = TRUE;}
        if (paf->modifier < -75) lay_failed = TRUE;
        if (paf->modifier < -30 )
        {
            if(number_percent()< (-paf->modifier))
            {
                lay_failed = TRUE;
            }else
            {
                value/=2;
                lay_weakened = TRUE;
            }
        }

        if (paf->modifier < -5 && number_percent()< 3*(-paf->modifier))
        {
            value/=2;
            lay_weakened = TRUE;
        }

    }
    affect_to_char(	ch,	&af, NULL, FALSE );
    if ( lay_failed == TRUE)
    {
        if ( ch == victim )
        {
            act( "Przyk³adasz d³onie do swojego cia³a i zaczynasz siê koncentrowaæ, jednak uzdrowicielska moc zosta³a ci zabrana.", ch, NULL, NULL, TO_CHAR );
            act( "$n przyk³ada d³onie do swojego cia³a i zamyka oczy.", ch, NULL, NULL, TO_ROOM );
        }
        else
        {
            act( "Przyk³adasz d³onie do cia³a $Z i zaczynasz siê koncentrowaæ.", ch, NULL, victim, TO_CHAR );
            act( "$n przyk³ada d³onie do twojego ciala i zamyka oczy, jednak nic siê nie dzieje.", ch, NULL, victim, TO_VICT );
            act( "$n przyk³ada d³onie do cia³a $Z i zamyka oczy.", ch, NULL, victim, TO_NOTVICT );
        }
        return;
    }
    else
    {
        if ( lay_weakened == TRUE) send_to_char( "Zbieraj±c w sobie ¿yciodajn± energie wyczuwasz, ze co¶ jest nie tak.\n\r", ch );
    }

    //nie da siê undeadow
    if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) )
    {
        act("Przykladasz d³onie do cia³a $Z, ale nic siê nie dzieje.", ch, NULL, victim, TO_CHAR);
        act("$n przyk³ada d³onie do cia³a $Z i zamyka oczy.", ch, NULL, victim, TO_NOTVICT);
        act("$n przyk³ada d³onie do twojego cia³a ale nic siê nie dzieje.",ch, NULL, victim, TO_VICT);
        return;
    }

    //tutaj szansa na spierdolenie, jezeli cel walczy
    if ( victim->position == POS_FIGHTING || victim->fighting != NULL )
    {
        if ( number_range( -5, 20 ) > get_curr_stat_deprecated(ch,STAT_DEX ) && ch != victim )
        {
            act("Próbujesz przy³o¿yæ d³onie do cia³a $Z, jednak $e wykonuje nagle gwa³towny ruch i wiêkszo¶æ leczniczego ciep³a rop³ywa siê po okolicy.", ch, NULL, victim, TO_CHAR);
            act("$n próbuje przy³o¿yæ d³onie do twojego cia³a, jednak walka wymusza na tobie gwa³towny ruch i wiêkszo¶æ leczniczego ciep³a rop³ywa siê po okolicy.",ch, NULL, victim, TO_VICT);
            act("$n próbuje przy³o¿yæ d³onie do cia³a $Z, jednak $e wykonuje nagle gwa³towny ruch i wiêkszo¶æ leczniczego ciep³a rop³ywa siê po okolicy.",ch, NULL, victim, TO_NOTVICT);
            heal_char (ch, ch, value/3);
            return;
        }
    }

    if ( ch == victim )
    {
        act( "Przyk³adasz d³onie do swojego cia³a i zaczynasz siê koncentrowaæ. Po chwili zaczynasz odczuwaæ rozchodz±ce siê z twoich d³oni ciep³o.", ch, NULL, NULL, TO_CHAR );
        act( "$n przyk³ada d³onie do swojego cia³a i zamyka oczy.", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
        act( "Przyk³adasz d³onie do cia³a $Z i zaczynasz siê koncentrowaæ.", ch, NULL, victim, TO_CHAR );
        act( "$n przyk³ada d³onie do twojego ciala i zamyka oczy. Po chwili zaczynasz odczuwaæ rozchodz±ce siê z d³oni $z ciep³o.", ch, NULL, victim, TO_VICT );
        act( "$n przyk³ada d³onie do cia³a $Z i zamyka oczy.", ch, NULL, victim, TO_NOTVICT );
    }

    heal_char (ch, victim, UMAX(10, value));
    if (cure_po == TRUE)
    {
        if ( is_affected( victim, gsn_poison ) )
        {
            affect_strip( victim, gsn_poison );
            send_to_char( "Trucizna przestaje kr±¿yæ w twojej krwi.\n\r", victim );
            act( "$n wygl±da o wiele lepiej.", victim, NULL, NULL, TO_ROOM );
            paf->modifier -= 15;
        }
        if ( is_affected( victim, gsn_plague ) )
        {
            send_to_char( "Wrzody pokrywaj±ce twoje cia³o znikaj±.\n\r", victim );
            act( "Wrzody pokrywaj±ce cia³o $z znikaj±.", victim, NULL, NULL, TO_ROOM );
            affect_strip( victim, gsn_plague );
            paf->modifier -= 10;
        }

        if ( IS_AFFECTED( victim, AFF_BLIND ) )
        {
            affect_strip( victim, gsn_blindness );
            affect_strip( victim, gsn_wind_charger );
            affect_strip( victim, 215 ); //pwb
            affect_strip( victim, 51 ); //pyrotechnics
            affect_strip( victim, 240 ); //sunscorch
            EXT_REMOVE_BIT( victim->affected_by, AFF_BLIND );
            act( "$n odzyskuje wzrok.", victim, NULL, NULL, TO_ROOM );
            send_to_char( "Odzyskujesz wzrok.\n\r", victim );
            paf->modifier -= 15;
        }

    }
    if (extended_wait == TRUE) send_to_char( "Czujesz jak moc zupe³nie opuszcza twoje rêce.\n\r", ch );
    if (shorter_wait == TRUE) send_to_char( "Czujesz jak w twoich rêkach na nowo gromadzi siê energia.\n\r", ch );

    update_pos( victim );
    check_improve( ch, NULL, gsn_lay, TRUE, 7 );
    return;
}

void do_first_aid( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim;
    int healed_perc, healed_val, skill;
    AFFECT_DATA af, *affect;
    bool success = FALSE;
    int leamod;

    if ( ( skill = get_skill( ch, gsn_first_aid ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Kogo chcesz wspomóc?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        return;
    }

    if ( is_undead(victim) || IS_SET( victim->form, FORM_CONSTRUCT ) )
    {
        send_to_char( "To chyba nie jest najlepszy pomys³.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_SILENCE ) )
    {
        act( "K³adziesz d³onie na ciele $Z i be³koczesz co¶, co ma przypominaæ s³owa modlitwy.", ch, NULL, victim, TO_CHAR );
        act( "$n k³adzie d³onie na twoim ciele i be³kocze co¶ bez sensu.", ch, NULL, victim, TO_VICT );
        act( "$n k³adzie d³onie na ciele $Z i be³kocze co¶ bez sensu.", ch, NULL, victim, TO_NOTVICT );
        return;
    }

    if ( victim->position == POS_FIGHTING || victim->fighting != NULL )
    {
        if ( victim == ch )
            act( "Nie jestes w stanie pomóc sobie w czasie walki!", ch, NULL, victim, TO_CHAR );
        else
            act( "Nie jestes w stanie pomóc $X w czasie gdy $E walczy!", ch, NULL, victim, TO_CHAR );
        return;
    }

    healed_val   = victim->counter[ 0 ];
    if ( ch->level >= 20 )
        healed_val  += victim->counter[ 1 ];
    if ( ch->level >= 25 )
        healed_val  += victim->counter[ 2 ];

    if ( healed_val == 0 )
    {
        if ( ch == victim )
            print_char( ch, "Nie masz ¿adnych ran, które %s uleczyæ.\n\r", ch->sex == 2 ? "mog³aby¶" : ch->sex == 0 ? "mog³oby¶" : "móg³by¶" );
        else
            act( "$N nie ma ran, które mo¿esz uleczyæ.", ch, NULL, victim, TO_CHAR );

        return;
    }

    WAIT_STATE( ch, skill_table[ gsn_first_aid ].beats );

    healed_perc  = UMIN( skill / 2 + ch->level * 2, 100 );
    healed_val   = (healed_val * healed_perc) / 100;

    victim->counter[ 0 ] = 0;
    victim->counter[ 1 ] = 0;
    victim->counter[ 2 ] = 0;

    if ( number_percent() < skill )
    {
        if ( ch == victim )
        {
            act( "K³adziesz d³onie na swoich ostatnio odniesionych ranach i wymawiasz krótkie s³owa modlitwy.", ch, NULL, NULL, TO_CHAR );
            act( "$n k³adzie d³onie na swoich ostatnio odniesionych ranach i wymawia krótk± modlitwe.", ch, NULL, NULL, TO_ROOM );
        }
        else
        {
            act( "K³adziesz d³onie na ostatnio odniesionych ranach $Z i wymawiasz krótkie s³owa modlitwy.", ch, NULL, victim, TO_CHAR );
            act( "$n k³adzie d³onie na twoich ostatnio odniesionych ranach i wymawia krótk± modlitwe.", ch, NULL, victim, TO_VICT );
            act( "$n k³adzie d³onie na ostatnio odniesionych ranach $Z i wymawia krótk± modlitwe.", ch, NULL, victim, TO_NOTVICT );
        }

        if ( healed_perc == 100 )
        {
            send_to_char("Twoje cia³o wype³nia lecznicze ciep³o, a wszystkie ostatnio odniesione rany znikaj±.\n\r", victim );
            act("Wszystkie ostatnio odniesione rany $z bledn± i znikaj±.", victim, NULL, NULL, TO_ROOM );
        }
        else
        {
            send_to_char("Twoje cia³o wype³nia lecznicze ciep³o, cze¶æ ostatnio odniesionych ran goi siê.\n\r", victim );
            act("Czê¶æ ostatnio odniesionych ran $z goi siê.", victim, NULL, NULL, TO_ROOM );
        }

        victim->hit = UMIN( victim->hit + healed_val, get_max_hp(victim) );
        update_pos( victim );
        success = TRUE;

        if( is_affected( victim, gsn_bleeding_wound ) )
        {
            if ( skill_table[ gsn_bleeding_wound ].msg_off )
            {
                send_to_char( skill_table[ gsn_bleeding_wound ].msg_off, victim );
                send_to_char( "\n\r", victim );
            }
            affect_strip( victim, gsn_bleeding_wound );
        }
    }
    else
    {
        if ( ch == victim )
        {
            act( "K³adziesz d³onie na swoich ostatnio odniesionych ranach i wymawiasz krótkie s³owa modlitwy. Przez chwile czujesz tylko ból.", ch, NULL, NULL, TO_CHAR );
            act( "$n k³adzie d³onie na swoich ostatnio odniesionych ranach i wymawia krótk± modlitwe. Po chwili krzywi siê na moment z bólu.", ch, NULL, NULL, TO_ROOM );
        }
        else
        {
            act( "K³adziesz d³onie na ostatnio odniesionych ranach $Z i wymawiasz krótkie s³owa modlitwy. Niestety nie uda³o ci siê.", ch, NULL, victim, TO_CHAR );
            act( "$n k³adzie d³onie na twoich ostatnio odniesionych ranach i wymawia krótk± modlitwe. Przez chwile czujesz przejmuj±cy ból.", ch, NULL, victim, TO_VICT );
            act( "$n k³adzie d³onie na ostatnio odniesionych ranach $Z i wymawia krótk± modlitwe. Po chwili $N krzywi siê z bólu.", ch, NULL, victim, TO_NOTVICT );
        }

        victim->hit = UMAX( victim->hit - healed_val / 10, 0 );
        success = FALSE;

    }

    /* learn part */
    /*
       blokada learna tik po uzyciu
       potem im czesciej ktos uzywa (spam learna) tym gorzej siê uczy
     */

    af.where	 = TO_AFFECTS;
    af.type	  	 = gsn_first_aid;
    af.level	 = 50;
    af.duration  = 5;
    af.rt_duration = 0;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = &AFF_NONE;
    af.visible	 = FALSE;

    if ( is_affected( ch, gsn_first_aid ) )
    {
        affect = affect_find( ch->affected, gsn_first_aid );
        switch ( affect->duration )
        {
            case 0:
                leamod = 10;
                affect_strip(victim, gsn_first_aid);
                affect_to_char(	ch,	&af, NULL, FALSE );
                break;
            case 1:
                leamod = 15;
                affect_strip(victim, gsn_first_aid);
                affect_to_char(	ch,	&af, NULL, FALSE );
                break;
            case 2:
                leamod = 20;
                affect_strip(victim, gsn_first_aid);
                affect_to_char(	ch,	&af, NULL, FALSE );
                break;
            case 3:
                leamod = 25;
                affect_strip(victim, gsn_first_aid);
                affect_to_char(	ch,	&af, NULL, FALSE );
                break;
            case 4:
                leamod = 30;
                affect_strip(victim, gsn_first_aid);
                affect_to_char(	ch,	&af, NULL, FALSE );
                break;
            case 5:
            default:
                /* 2 tiki po uzyciu blokada learna */
                return;
                break;
        }
    }
    else
    {
        leamod = 10;
        affect_to_char(	ch,	&af, NULL, FALSE );
    }

    if ( success )
        check_improve( ch, NULL, gsn_first_aid, TRUE, leamod );
    else
        check_improve( ch, NULL, gsn_first_aid, FALSE, leamod / 3*4 );

    return;
}

void do_healing_touch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim;
    AFFECT_DATA af;
    int skill, sid;

    if ( ( skill = get_skill( ch, gsn_healing_touch ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Kogo chcesz uleczyæ?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        return;
    }

    if ( is_affected( ch, gsn_healing_touch ) )
    {
        if ( !IS_AFFECTED( ch, AFF_HEALING_TOUCH ) )
            send_to_char( "Nie masz jeszcze tyle si³ witalnych.\n\r", ch );
        else
            send_to_char( "Przecie¿ ju¿ to robisz.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Nie mo¿esz uleczyæ siebie swoj± energi± witaln±.\n\r", ch );
        return;
    }
    if (
            is_undead(victim) ||
            !(
                IS_SET( victim->form, FORM_ANIMAL ) ||
                IS_SET( victim->form, FORM_BIPED ) ||
                IS_SET( victim->form, FORM_BIRD ) ||
                IS_SET( victim->form, FORM_CENTAUR ) ||
                IS_SET( victim->form, FORM_DRAGON ) ||
                IS_SET( victim->form, FORM_MAMMAL ) ||
                IS_SET( victim->form, FORM_REPTILE ) ||
                IS_SET( victim->form, FORM_SNAKE ) ||
                IS_SET( victim->form, FORM_WARM )
             )
       )
    {
        send_to_char( "To chyba nie jest najlepszy pomys³.\n\r", ch );
        return;
    }

    if ( victim->position == POS_FIGHTING || victim->fighting != NULL )
    {
        act( "Nie jestes w stanie uleczyæ $Z w czasie gdy $E walczy!", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( victim->hit == get_max_hp(victim) )
    {
        act( "$N nie ma ¿adnych widocznych ran na ciele.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if ( IS_AFFECTED( victim, AFF_HEALING_TOUCH ))
    {
        act( "Lecznicza energia czyjego¶ dotyku ju¿ wype³nia cia³o $Z.", ch, NULL, victim, TO_CHAR);
        return;
    }

    af.where	= TO_AFFECTS;
    af.type		= gsn_healing_touch;
    af.level	= ch->level;
    af.duration = 3; af.rt_duration = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = &AFF_NONE;
    af.visible	 = FALSE;

    if ( number_percent() < skill )
    {
        sid = number_range( 1, 9999 );

        af.duration =  3;         af.rt_duration = 0;
        af.modifier = sid;	//caster
        af.bitvector = &AFF_HEALING_TOUCH;
        affect_to_char( ch, &af, NULL, FALSE );

        af.duration =  3;         af.rt_duration = 0;
        af.modifier = sid+10000;	//victim
        af.bitvector = &AFF_HEALING_TOUCH;
        affect_to_char( victim, &af, NULL, FALSE );

        act( "K³adziesz d³onie na ciele $Z i w skupieniu zaczynasz siê modliæ.", ch, NULL, victim, TO_CHAR );
        act( "$n k³adzie d³onie na twoim ciele i w skupieniu zaczyna siê modliæ.", ch, NULL, victim, TO_VICT );
        act( "$n k³adzie d³onie na ciele $Z i w skupieniu zaczyna siê modliæ.", ch, NULL, victim, TO_NOTVICT );

        // Tener: zabieram check_improve do update.c [20080513]
    }
    else
    {
        affect_to_char( ch, &af, NULL, FALSE );
        act( "K³adziesz d³onie na ciele $Z i w skupieniu zaczynasz siê modliæ, jednak po chwili dekoncentrujesz siê i przerywasz.", ch, NULL, victim, TO_CHAR );
        act( "$n k³adzie d³onie na twoim ciele i przymyka oczy. Po chwili je otwiera i z irytacj± patrzy na ciebie.", ch, NULL, victim, TO_VICT );
        act( "$n k³adzie d³onie na ciele $Z i w skupieniu zaczyna siê modliæ, po chwili jednak przerywa i patrzy siê w niebo.", ch, NULL, victim, TO_NOTVICT );
        check_improve( ch, NULL, gsn_healing_touch, FALSE, 10 );
    }

    WAIT_STATE( ch, skill_table[ gsn_healing_touch ].beats );
    return;
}

void abort_healing_touch( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA *aff_ch = 0, *aff_vch = 0;
    CHAR_DATA *vch = 0, *vch_next = 0;
    int sid = 0, breakside = 0;

    // breakside: 0 - target brake, 1 - caster break


    aff_ch  = affect_find( ch->affected, gsn_healing_touch );
    sid = aff_ch->modifier;

    if ( sid > 10000 )
    {
        // target makes action
        sid -= 10000;
        breakside   = 0;
    }
    else
    {
        // caster makes action
        sid += 10000;
        breakside   = 1;
    }

    for ( vch = ch->in_room->people;vch;vch = vch_next )
    {
        vch_next = vch->next_in_room;
        if ( vch != ch && IS_AFFECTED( vch, AFF_HEALING_TOUCH ))
        {
            aff_vch = affect_find( vch->affected, gsn_healing_touch );
            if ( aff_vch->modifier == sid )
                break;
            else
                aff_vch = NULL;
        }
    }

    if (( aff_ch == NULL ) && ( aff_vch == NULL ))
        return;
    else if (( aff_ch == NULL ) && (!breakside) )
    {
        EXT_REMOVE_BIT( vch->affected_by, AFF_HEALING_TOUCH );
        print_char( vch, "Przerywasz modlitwe.\n\r" );
        return;
    }
    else if (( aff_ch == NULL ) && breakside )
    {
        affect_strip( vch, gsn_healing_touch );
        print_char( vch, "Lecznicze ciep³o opuszcza twoje cia³o.\n\r" );
        return;
    }
    else if (( aff_vch == NULL ) && (!breakside) )
    {
        affect_strip( ch, gsn_healing_touch );
        print_char( ch, "Lecznicze ciep³o opuszcza twoje cia³o.\n\r" );
        return;
    }
    else if (( aff_vch == NULL ) && breakside )
    {
        EXT_REMOVE_BIT( ch->affected_by, AFF_HEALING_TOUCH );
        print_char( ch, "Przerywasz modlitwe.\n\r" );
        return;
    }

    if ( breakside )
    {
        act( "Przerywasz modlitwe i odsuwasz d³onie od cia³a $Z.", ch, NULL, vch, TO_CHAR );
        act( "$n przerywa modlitwe i odsuwa d³onie od twojego cia³a.", ch, NULL, vch, TO_VICT );
        act( "$n przerywa modlitwe i odsuwa d³onie od cia³± $Z.", ch, NULL, vch, TO_NOTVICT );
        EXT_REMOVE_BIT( ch->affected_by, AFF_HEALING_TOUCH );
        affect_strip( vch, gsn_healing_touch );
    }
    else
    {
        act( "Odsuwasz siê przerywaj±c $X modlitwe.", ch, NULL, vch, TO_CHAR );
        act( "$n odsuwa siê od ciebie przerywaj±c ci modlitwe.", ch, NULL, vch, TO_VICT );
        act( "$n odsuwa siê przerywajac $X modlitwe.", ch, NULL, vch, TO_NOTVICT );
        EXT_REMOVE_BIT( vch->affected_by, AFF_HEALING_TOUCH );
        affect_strip( ch, gsn_healing_touch );
    }

    return;
}

/* 'Garhal symbiosis' <- skill druidyczny na bazie pomys³u Tenera z Forum.
   - wrzuca krotka "regeneracje" hp
   - zwieksza pragnienie i glod uzywajacego skilla
   - raz na 15 tikow
   - jezeli ofiara jest zatruta wrzuca slow_poison
 */
void do_garhal( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim;
    AFFECT_DATA af;
    int luck, skill;

    luck = get_curr_stat_deprecated( ch, STAT_LUC );

    if ( ( skill = get_skill( ch, gsn_garhal ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Kto ma byæ celem symbiozy nasienia garhal?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, argument ) ) != ch && ch->level < 20 )
    {
        send_to_char ( "Twoje nasienie garhal jest jeszcze zbyt s³abe, ¿eby przekazywaæ swoj± moc poza twoje cia³o.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        return;
    }

    if ( is_affected(ch,gsn_garhal) )
    {
        send_to_char( "Musisz poczekaæ chwilê zanim nasienie garhal odnowi swoj± moc.\n\r", ch );
        return;
    }

    af.where	 = TO_AFFECTS;
    af.type	  	 = gsn_garhal;
    af.level	 = 50;//by nie szlo dispelnac
    af.duration  = 15;
    af.rt_duration = 0;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = &AFF_NONE;
    af.visible	 = FALSE;
    affect_to_char(	ch,	&af, NULL, FALSE );

    WAIT_STATE( ch, skill_table[ gsn_garhal ].beats );

    //naklada affect "regeneration" na cel
    af.where	 = TO_AFFECTS;
    af.type	  = 86;
    af.level	 = skill/10 + UMIN(ch->level/10, 5);
    af.duration  = ((number_range( 0, luck ) > 15 )? 1:0);
    af.rt_duration = 0;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = &AFF_REGENERATION;
    affect_to_char(	victim,	&af, NULL, TRUE	 );

    //zwiekszenie glodu i pragnienia wykonujacego "garhal symbiosis"
    if ( !IS_NPC( ch ) )
    {
        ch->condition[COND_HUNGER] = UMAX( 0, ch->condition[COND_HUNGER] -10);
        ch->condition[COND_THIRST] = UMAX( 0, ch->condition[COND_THIRST] -10);
    }

    //jezeli victim jest poisoniety to to cure'uje :)
    if ( is_affected( victim, gsn_poison ) )
    {
        if (victim == ch)
            act( "{GK³adziesz rêce na $Z. Twoje d³onie rozb³yskuj± na chwilê zielonkawym ¶wiat³em, przep³ywa przez nie magiczna energia. Wyczuwasz, ¿e garhal oczy¶ci³ p³yn±c± w krwi $Z truciznê.{x", ch, NULL, victim, TO_CHAR );
        act( "{G$n k³adzie na tobie swoje rece, które rozb³yskuj± na chwilê zielonkawym ¶wiat³em. Czujesz jak przez twoje cia³o przep³ywa fala delikatnej i koj±cej energi, która wype³nia ciê now± si³±! Z zaskoczeniem stwierdzasz tak¿e, ¿e wszelkie objawy zatrucia twojego organizmu minê³y.{x", ch, NULL, victim, TO_VICT );
        act( "{G$n k³adzie rêce na $Z, które rozb³yskuj± na chwilê zielonkawym ¶wiat³em.{x", ch, NULL, victim, TO_NOTVICT );
        affect_strip(victim, gsn_poison);
        return;
    }

    if ( ch == victim )
    {
        act( "{GZamykasz oczy i odprê¿asz siê. Po chwili zaczynasz czuæ delikatne ciep³o jakim emanuje garhal w twoim ciele.{x", ch, NULL, NULL, TO_CHAR );
        act( "{G$n zamyka na moment oczy po czym w skupieniu rozpoczyna co¶ na kszta³t niemej medytacji.{x", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
        act( "{GK³adziesz rêce na $Z. Twoje d³onie rozb³yskuj± na chwilê zielonkawym ¶wiat³em, przep³ywa przez nie magiczna energia.{x", ch, NULL, victim, TO_CHAR );
        act( "{G$n k³adzie na tobie swoje rece, które rozb³yskuj± na chwilê zielonkawym ¶wiat³em. Czujesz jak przez twoje cia³o przep³ywa fala delikatnej i koj±cej energi, która wype³nia ciê now± si³±!{x", ch, NULL, victim, TO_VICT );
        act( "{G$n k³adzie rêce na $Z, które rozb³yskuj± na chwilê zielonkawym ¶wiat³em.{x", ch, NULL, victim, TO_NOTVICT );
    }

    check_improve( ch, NULL, gsn_garhal, TRUE, 10 );
    return;
}

/* skraca czas zapamietywania dla klerykow */
void do_meditate( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int skill;

    if ( ( skill = get_skill( ch, gsn_meditation ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->position != POS_RESTING )
    {
        send_to_char( "Przyjmij bardziej komfortow± pozycjê do medytacji.\n\r", ch );
        return;
    }

    if ( is_affected( ch, gsn_meditation ) )
    {
        if ( !IS_AFFECTED( ch, AFF_MEDITATION ) )
            send_to_char( "Nie mo¿esz siê wystarczaj±co skoncentrowaæ.\n\r", ch );
        else
            send_to_char( "Przecie¿ ju¿ medytujesz.\n\r", ch );
        return;
    }

    if ( ch->count_memspell <= 0 || ch->memming == NULL )
    {
        send_to_char( "Nie masz ochoty na medytacjê.\n\r", ch );
        return;
    }

    // no jak kto¶ jest pod wp³ywem halucynacji to sobie nie pomedytuje
    if ( is_affected( ch, gsn_hallucinations ) )
    {
        send_to_char( "Medytujesz nieokre¶lone kszta³ty w nieokre¶lonym uniwersum.\n\r", ch );
        WAIT_STATE( ch, skill_table[ gsn_meditation ].beats );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = gsn_meditation;
    af.level = 0;
    af.duration =  3;     af.rt_duration = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = &AFF_NONE;

    if ( number_percent() < skill )
    {
        af.level = skill;
        af.duration = -1;         af.rt_duration = 0;
        af.bitvector = &AFF_MEDITATION;
        affect_to_char( ch, &af, NULL, TRUE );

        print_char( ch, "Pogr±¿asz siê w medytacji.\n\r" );
        act( "$n pogr±¿a siê w medytacji.", ch, NULL, NULL, TO_ROOM );

        if ( ch->memming != NULL )
            ch->memming->duration = UMAX( 1, ch->memming->duration - ch->level / 15 );

        // Tener: przesuwam check_improve do update.c [20080513]
    }
    else
    {
        affect_to_char( ch, &af, NULL, FALSE );
        print_char( ch, "Nie uda³o ci siê wystarczaj±co skoncentrowaæ.\n\r" );
        check_improve( ch, NULL, gsn_meditation, FALSE, 5 );
    }
    WAIT_STATE( ch, skill_table[ gsn_meditation ].beats );
    return;
}

/* przyspiesza regeneracje - dla barbiesow */
void do_recuperate( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int skill;

    if ( ( skill = get_skill( ch, gsn_recuperate ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->position != POS_RESTING )
    {
        send_to_char( "Przyjmij bardziej komfortow± pozycjê.\n\r", ch );
        return;
    }

    if ( is_affected( ch, gsn_recuperate ) )
    {
        if ( !IS_AFFECTED( ch, AFF_RECUPERATE ) )
        {
            send_to_char( "Nie mo¿esz siê wystarczaj±co skoncentrowaæ.\n\r", ch );
        }
        return;
    }

    if ( ch->hit > (100*ch->max_hit)/95 && ch->move > (100*ch->max_move)/95 )
    {
        send_to_char( "Nie czujesz potrzeby do takiego odpoczynku.\n\r", ch );
        return;
    }

    skill += URANGE( 0, ch->level, 20 );
    skill = URANGE( 20, skill, 95 );

    af.where = TO_AFFECTS;
    af.type = gsn_recuperate;
    af.level = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = &AFF_NONE;

    /**
     * check_improve - nieuzywamy, bo jest w update.c
     */
    if ( number_percent() < skill )
    {
        print_char( ch, "Zamykasz oczy i odprê¿asz siê.\n\r" );
        act( "$n zamyka oczy i odprê¿a siê.", ch, NULL, NULL, TO_ROOM );
        af.duration = -1;
        af.rt_duration = 0;
        af.bitvector = &AFF_RECUPERATE;
        affect_to_char( ch, &af, NULL, TRUE );
        WAIT_STATE( ch, skill_table[ gsn_recuperate ].beats );
    }
    else
    {
        print_char( ch, "Nie uda³o ci siê.\n\r" );
        af.duration = 2;
        af.rt_duration = 0;
        affect_to_char( ch, &af, NULL, FALSE );
        WAIT_STATE( ch, skill_table[ gsn_recuperate ].beats );
    }

    return;
}

/* reperowanie zbroi - dla barbiesow */
void do_mend( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *armor;
    int skill;
    AFFECT_DATA *affect, af;

    argument = one_argument( argument, arg );

    if ( ( skill = get_skill( ch, gsn_repair ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->fighting )
    {
        send_to_char( "Lepiej skup siê na walce!\n\r", ch );
        return;
    }

    if ( arg[ 0 ] == '\0' )
    {
        print_char( ch, "Co chcia³%s naprawiæ?\n\r", ch->sex == 2 ? "aby¶" : "by¶" );
        return;
    }

    if ( ( armor = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
        return;
    }

    if ( armor->item_type != ITEM_ARMOR )
    {
        print_char( ch, "To raczej nie wygl±da na zbroje.\n\r" );
        return;
    }

    if ( armor->condition > 95 )
    {
        print_char( ch, "%s wygl±da ca³kiem dobrze, nie ma co naprawiaæ.\n\r", capitalize( armor->short_descr ) );
        return;
    }

    for ( affect = armor->affected; affect != NULL; affect = affect->next )
    {
        if ( affect->type == gsn_repair )
        {
            if ( affect->duration < 0 )
                print_char( ch, "%s jest prawie zniszczony, nie ma sensu czegokolwiem naprawiaæ.\n\r", capitalize( armor->short_descr ) );
            else
                print_char( ch, "%s wygl±da ca³kiem dobrze, nie ma co naprawiaæ.\n\r", capitalize( armor->short_descr ) );
            return;
        }
    }

    skill = URANGE( 1, skill , 75 );

    if ( number_percent() < skill )
    {
        int repair;

        repair = URANGE( 5, ( ( 50 + skill / 2 ) * ( 100 - armor->condition ) ) / 100 , 95 );
        armor->condition += repair;

        af.where = TO_OBJECT;
        af.type = gsn_repair;
        af.level = 0;
        af.duration = 48;         af.rt_duration = 0;
        af.location = 0;
        af.modifier = 0;
        af.bitvector = &AFF_NONE;
        affect_to_obj( armor, &af );

        print_char( ch, "Naprawiasz %s.\n\r", armor->short_descr );
        act( "$n naprawia $h.", ch, armor, NULL, TO_ROOM );
        WAIT_STATE( ch, skill_table[ gsn_repair ].beats );
        check_improve( ch, NULL, gsn_repair, TRUE, 8 );
    }
    else
    {
        print_char( ch, "Naprawiasz %s, jednak teraz wygl±da jeszcze gorzej.\n\r", armor->short_descr );
        act( "$n naprawia $h.", ch, armor, NULL, TO_ROOM );
        armor->condition = UMAX( armor->condition - 3, 1 );

        /* zeby automaty nie staly*/
        if ( number_percent() < 15 )
        {
            af.where = TO_WEAPON;
            af.type = gsn_repair;
            af.level = 0;
            af.duration = -1;             af.rt_duration = 0;
            af.location = 0;
            af.modifier = 0;
            af.bitvector = &AFF_NONE;
            affect_to_obj( armor, &af );
            armor->cost = 0;
        }
        WAIT_STATE( ch, skill_table[ gsn_repair ].beats );
        check_improve( ch, NULL, gsn_repair, FALSE, 6 );
    }
    return;
}

/* reperacja broni - dla barbiesow */
void do_sharpen( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *weapon;
    int skill, weapon_skill, dex_mod, chance = 0, luck = get_curr_stat_deprecated( ch, STAT_LUC );
    AFFECT_DATA *weapon_affect, af;

    argument = one_argument( argument, arg );

    if ( ( skill = get_skill( ch, gsn_sharpen ) ) < 1 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->fighting )
    {
        send_to_char( "Lepiej skup siê na walce!\n\r", ch );
        return;
    }

    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Co chcia³<&/a/o>by¶ naostrzyæ?\n\r", ch );
        return;
    }

    if ( ( weapon = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
        return;
    }

    if ( weapon->item_type != ITEM_WEAPON )
    {
        print_char( ch, "Przecie¿ nie ma sensu ostrzyæ %s.\n\r", weapon->name2 );
        return;
    }

    switch ( weapon->value[ 0 ] )
    {
        case WEAPON_MACE:
        case WEAPON_FLAIL:
        case WEAPON_WHIP:
        case WEAPON_STAFF:
            print_char( ch, "Przecie¿ nie ma sensu ostrzyæ %s.\n\r", weapon->name2 );
            return;
        default :
            break;
    }

    /**
     * ITEM_NOREPAIR
     */
    if ( EXT_IS_SET( weapon->extra_flags, ITEM_NOREPAIR ) )
    {
        print_char( ch, "%s nie bardzo daje siê naostrzyæ.\n\r", capitalize( weapon->short_descr ) );
        return;
    }

    /**
     * repair limits
     */
    if ( weapon->repair_limit > 0 && weapon->repair_counter > weapon->repair_limit )
    {
        print_char( ch, "%s nie bardzo daje siê ju¿ naostrzyæ.\n\r", capitalize( weapon->short_descr ) );
        return ;
    }
    if ( weapon->condition > weapon->repair_condition )
    {
        print_char( ch, "%s nie bardzo daje siê bardziej naostrzyæ.\n\r", capitalize( weapon->short_descr ) );
        return ;
    }

    if ( weapon->condition > 95 )
    {
        print_char( ch, "%s wygl±da ca³kiem dobrze, nie ma co ostrzyæ.\n\r", capitalize( weapon->short_descr ) );
        return;
    }

    for ( weapon_affect = weapon->affected; weapon_affect != NULL; weapon_affect = weapon_affect->next )
    {
        if ( weapon_affect->type == gsn_sharpen )
        {
            print_char( ch, "Kto¶ niedawno ostrzy³ %s, nic wiêcej nie zdzia³asz.\n\r", weapon->name4 );
            return;
        }
    }

    weapon_skill = get_skill( ch, weapon_sn( weapon ) );
    dex_mod = get_curr_stat_deprecated( ch, STAT_DEX );

    chance = skill / 2;
    chance += dex_mod;
    chance += weapon_skill / 3;
    chance += URANGE( 1, number_range( 0, luck / 2 ), 10 );

    chance = URANGE( 1, chance , 95 );

    if ( number_percent() < chance )
    {
        int repair;
        repair = URANGE( 5, ( ( 25 + chance / 3 + weapon_skill / 3 ) * ( 100 - weapon->condition ) ) / 100 , 95 );
        weapon->condition = UMAX( weapon->repair_condition, weapon->condition + repair );
        if ( weapon->condition > 90 &&
                skill > 50 &&
                weapon_skill > 50 && number_percent() < 25 &&
                !IS_WEAPON_STAT( weapon, WEAPON_SHARP ) &&
                !IS_WEAPON_STAT( weapon, WEAPON_VORPAL ) )
        {
            af.where = TO_WEAPON;
            af.type = gsn_sharpen;
            af.level = 0;
            af.duration = 24;
            af.rt_duration = 0;
            af.location = WEAPON_SHARP;
            af.modifier = 0;
            af.bitvector = &AFF_NONE;
            affect_to_obj( weapon, &af );
        }


        print_char( ch, "Ostrzysz %s.\n\r", weapon->name4 );
        act( "$n ostrzy $h.", ch, weapon, NULL, TO_ROOM );
        WAIT_STATE( ch, skill_table[ gsn_sharpen ].beats );
        check_improve( ch, NULL, gsn_sharpen, TRUE,10 );
    }
    else if ( number_range( 0, luck + weapon_skill / 4 ) == 0 )
    {
        print_char( ch, "Ci±gniesz ose³k± po ostrzu i po chwili %s nie nadaje siê ju¿ do niczego.\n\r", weapon->short_descr );
        act( "$n ci±gnie ose³k± po ostrzu $f.", ch, weapon, NULL, TO_ROOM );
        weapon->condition = 1;
        /**
         * repair limits
         */
        weapon->repair_counter++;
    }
    else
    {
        print_char( ch, "Ostrzysz %s, jednak teraz wygl±da jeszcze gorzej.\n\r", weapon->name4 );
        act( "$n ostrzy $h.", ch, weapon, NULL, TO_ROOM );
        weapon->condition = UMAX( weapon->condition - 2, 1 );
        WAIT_STATE( ch, skill_table[ gsn_sharpen ].beats );
        check_improve( ch, NULL, gsn_sharpen, FALSE, 8 );
        /**
         * repair limits
         */
        weapon->repair_counter++;
    }

    /* zeby automaty nie staly*/
    if ( number_percent() < ( 30 - luck / 2 ) )
    {
        af.where = TO_WEAPON;
        af.type = gsn_sharpen;
        af.level = 0;
        af.duration = 3;
        af.rt_duration = 0;
        af.location = 0;
        af.modifier = 0;
        af.bitvector = &AFF_NONE;
        affect_to_obj( weapon, &af );
    }

    return;
}

void do_lore( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj;
    CHAR_DATA *mob;
    int skill, val, pass = 0, stat_bonus, dur = 0;
    AFFECT_DATA af;
    AFFECT_DATA	*paf;
    extern int port; // port na którym ³±czy siê mud

    argument = one_argument( argument, arg );

    if( ch->class == CLASS_BARBARIAN || ch->class == CLASS_SHAMAN )
    {
        skill = get_skill( ch, gsn_lore );
        if ( skill <= 0 )
        {
            send_to_char( "Huh?\n\r", ch );
            return;
        }

        if ( ch->fighting )
        {
            send_to_char( "Lepiej skup siê na walce!\n\r", ch );
            return;
        }

        if ( arg[ 0 ] == '\0' )
        {
            print_char( ch, "Co chcial%s zbadac?\n\r", ch->sex == 2 ? "abys" : "bys" );
            return;
        }

        if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
        {
            send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
            return;
        }

        if ( is_affected( ch, gsn_lore ) )
        {
            send_to_char( "Nie mo¿esz siê wystarczaj±co skoncentrowaæ.\n\r", ch );
            return;
        }

        send_to_char( "\n\r", ch );
        switch ( obj->item_type )
        {
            case ITEM_ARMOR:
                if ( obj->condition >= 100 )
                {
                    if ( obj->liczba_mnoga )
                        sprintf( buf, "%s s± w znakomitym stanie.\n\r", capitalize( obj->short_descr ) );
                    else
                        sprintf( buf, "%s jest w znakomitym stanie.\n\r", capitalize( obj->short_descr ) );
                }
                else if ( obj->condition > 75 )
                {
                    if ( obj->liczba_mnoga )
                        sprintf( buf, "%s wygl±daj± ca³kiem nie¼le.\n\r", capitalize( obj->short_descr ) );
                    else
                        sprintf( buf, "%s wygl±da ca³kiem nie¼le.\n\r", capitalize( obj->short_descr ) );
                }
                else if ( obj->condition > 55 )
                {
                    if ( obj->liczba_mnoga )
                        sprintf( buf, "%s nie wygl±daj± najgorzej.\n\r", capitalize( obj->short_descr ) );
                    else
                        sprintf( buf, "%s nie wygl±da najgorzej.\n\r", capitalize( obj->short_descr ) );
                }
                else if ( obj->condition > 35 )
                {
                    if ( obj->liczba_mnoga )
                        sprintf( buf, "%s prosz± siê o wizytê u p³atnerza.\n\r", capitalize( obj->short_descr ) );
                    else
                        sprintf( buf, "%s prosi siê o wizytê u p³atnerza.\n\r", capitalize( obj->short_descr ) );
                }
                else if ( obj->condition >= 1 )
                {
                    if ( obj->liczba_mnoga )
                        sprintf( buf, "%s prawie siê rozpadaj± na kawa³ki.\n\r", capitalize( obj->short_descr ) );
                    else
                        sprintf( buf, "%s prawie siê rozpada na kawa³ki.\n\r", capitalize( obj->short_descr ) );
                }
                send_to_char( buf, ch );

                print_char( ch, "Materia³: %s.\n\r", material_table[ obj->material ].name );

                if ( number_percent() < skill + 10 + (get_curr_stat_deprecated(ch,STAT_WIS)-7)*3 )
                {
                    print_char( ch, "Klasa pancerza: %d [kluj±ce], %d [obuchowe], %d [ciête].\n\r",
                            obj->value[ 0 ], obj->value[ 1 ], obj->value[ 2 ] );
                }
                else
                {
                    print_char( ch, "Klasa pancerza: %d [kluj±ce], %d [obuchowe], %d [ciête].\n\r",
                            number_range( 6, 9 ), number_range( 6, 9 ), number_range( 6, 9 ) );
                }
                break;

            case ITEM_WEAPON:

                send_to_char( "Bron jest typu: ", ch );
                switch ( obj->value[ 0 ] )
                {
                    case( WEAPON_SWORD ) : send_to_char( "miecz.\n\r", ch );	break;
                    case( WEAPON_DAGGER ) : send_to_char( "sztylet.\n\r", ch );	break;
                    case( WEAPON_SPEAR ) : send_to_char( "w³ócznia.\n\r", ch );	break;
                    case( WEAPON_MACE ) : send_to_char( "maczuga.\n\r", ch );	break;
                    case( WEAPON_AXE ) : send_to_char( "topór.\n\r", ch );	break;
                    case( WEAPON_FLAIL ) : send_to_char( "korbacz.\n\r", ch );	break;
                    case( WEAPON_WHIP ) : send_to_char( "bicz.\n\r", ch );	break;
                    case( WEAPON_POLEARM ) : send_to_char( "broñ drzewcowa.\n\r", ch );	break;
                    case( WEAPON_STAFF ) : send_to_char( "laska.\n\r", ch );	break;
                    case( WEAPON_SHORTSWORD ) : send_to_char( "krótki miecz.\n\r", ch );	break;
                    case( WEAPON_CLAWS ) : send_to_char("szpony.\n\r", ch ); break;
                    default	: send_to_char( "nieznany.\n\r", ch );	break;
                }

                if ( obj->condition >= 100 )
                    sprintf( buf, "%s jest w znakomitym stanie.\n\r", capitalize( obj->short_descr ) );
                else if ( obj->condition > 75 )
                    sprintf( buf, "%s wygl±da ca³kiem nie¼le.\n\r", capitalize( obj->short_descr ) );
                else if ( obj->condition > 55 )
                    sprintf( buf, "%s nie wygl±da najgorzej.\n\r", capitalize( obj->short_descr ) );
                else if ( obj->condition > 35 )
                    sprintf( buf, "%s prosi siê o wizytê u kowala.\n\r", capitalize( obj->short_descr ) );
                else if ( obj->condition >= 1 )
                    sprintf( buf, "%s prawie rozpada siê na kawa³ki.\n\r", capitalize( obj->short_descr ) );
                send_to_char( buf, ch );

                print_char( ch, "Materia³: %s.\n\r", material_table[ obj->material ].name );

                if ( number_percent() < skill + 30 + (get_curr_stat_deprecated(ch,STAT_WIS)-7)*3 )
                    print_char( ch, "Obra¿enia zadawane %dd%d.\n\r", obj->value[ 1 ], obj->value[ 2 ] );

                if ( number_percent() < skill + (get_curr_stat_deprecated(ch,STAT_WIS)-7)*3 )
                {
                    if ( ( val = check_magic_attack( ch, NULL, obj ) ) != 0 )
                        print_char( ch, "Broñ magiczna +%d.\n\r", val );
                }
                break;
            default:
                print_char( ch, "Nie znasz siê na tym.\n\r" );
                return;
                break;
        }

        af.where = TO_AFFECTS;
        af.type = gsn_lore;
        af.level = 50;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = &AFF_NONE;
        af.duration = 5;         af.rt_duration = 0;
        af.visible = FALSE;
        affect_to_char( ch, &af, NULL, FALSE );
        WAIT_STATE( ch, skill_table[ gsn_lore ].beats );
        check_improve( ch, NULL, gsn_lore, TRUE, 15 );

        return;
    }

    skill = get_skill( ch, gsn_lore_intuition );

    if ( skill <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->fighting )
    {
        send_to_char( "Lepiej skup siê na walce!\n\r", ch );
        return;
    }

    if ( is_affected( ch, gsn_lore ) )
    {
        send_to_char( "Nie mo¿esz siê wystarczaj±co skoncentrowaæ.\n\r", ch );
        return;
    }

    stat_bonus = (get_curr_stat_deprecated(ch,STAT_WIS)-10)*3;
    stat_bonus += get_curr_stat_deprecated(ch,STAT_INT)-10;
    stat_bonus += get_curr_stat_deprecated(ch,STAT_CHA)-12;

    if( skill < 20 )
    {
        dur = 5;
    }
    else if( skill < 35 )
    {
        dur = 4;
    }
    else if( skill < 50 )
    {
        dur = 3;
    }
    else if( skill < 65 )
    {
        dur = 2;
    }
    else if( skill < 80 )
    {
        dur = 1;
    }

    if( stat_throw(ch,STAT_WIS) && stat_throw(ch,STAT_CHA) )
    {
        dur = URANGE( 1, dur - 1, 4 );
    }

    if ( arg[ 0 ] == '\0' || !str_cmp( arg, "room" ) || !str_cmp( arg, "lokacja" ) )
    {
        WAIT_STATE( ch, 24 );
        check_improve( ch, NULL, gsn_lore_intuition, TRUE, 8 );

        if( skill < 80 )
        {
            af.where = TO_AFFECTS;
            af.type = gsn_lore_intuition;
            af.level = 0;
            af.location = APPLY_NONE;
            af.modifier = 0;
            af.bitvector = &AFF_NONE;
            af.duration = UMAX( 1, dur );
            af.rt_duration = 0;
            af.visible = FALSE;
            affect_to_char( ch, &af, NULL, FALSE );
        }
        send_to_char("lore rooma.\n\r", ch );
        return;
    }
    else if ( ( mob = get_char_room( ch, arg ) ) != NULL )
    {
        WAIT_STATE( ch, 24 );
        check_improve( ch, NULL, gsn_lore_intuition, TRUE, 8 );

        if( skill < 80 )
        {
            af.where = TO_AFFECTS;
            af.type = gsn_lore_intuition;
            af.level = 0;
            af.location = APPLY_NONE;
            af.modifier = 0;
            af.bitvector = &AFF_NONE;
            af.duration = UMAX( 1, dur );             af.rt_duration = 0;
            af.visible = FALSE;
            affect_to_char( ch, &af, NULL, FALSE );
        }

        act("lore moba $Z.", ch, NULL, mob, TO_CHAR );
        return;
    }
    else if ( ( obj = get_obj_carry( ch, arg, ch ) ) != NULL || ( obj = get_obj_list( ch, arg, ch->in_room->contents ) ) != NULL )
    {
        WAIT_STATE( ch, 24 );
        check_improve( ch, NULL, gsn_lore_intuition, TRUE, 3 );

        if( skill < 80 )
        {
            af.where = TO_AFFECTS;
            af.type = gsn_lore_intuition;
            af.level = 0;
            af.location = APPLY_NONE;
            af.modifier = 0;
            af.bitvector = &AFF_NONE;
            af.duration = UMAX( 1, dur );             af.rt_duration = 0;
            af.visible = FALSE;
            affect_to_char( ch, &af, NULL, FALSE );
        }

        if ( (skill*3)/2 + 20 > number_range(1,100))
        {
            if ( obj->weight > 5 )
            {
                print_char( ch, "Waga %s wynosi oko³o %.2f kg.", obj->name2, obj->weight / 22.05 );
                send_to_char( ".\n\r", ch );
            }
            else
            {
                print_char( ch, "%s prawie nic nie wa¿y.", capitalize( obj->short_descr ) );
                send_to_char( ".\n\r", ch );
            }
        }

        if ( skill*2 + 20 > number_range(1,100))
        {
            if ( obj->material > 0 )
                print_char( ch, "Przedmiot ten wykonano z materia³u '%s'.\n\r", material_table[ obj->material ].name );
            else
                send_to_char( ".\n\r", ch );
        }
        else
        {
            if ( obj->material > 0 )
                print_char( ch, "Przedmiot ten wykonano z materia³u '%s'.\n\r", material_table[ number_range(1,45) ].name );
            else
                send_to_char( ".\n\r", ch );
        }

        if ( obj->cost <         1 * RATTING_SILVER )
        {
            print_char( ch, "Za %s w sklepie nie dosta³%sby¶ nawet jednej, srebrnej monety.\n\r", obj->name4, ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : ""	);
        }
        else if ( obj->cost <   11 * RATTING_SILVER  )
        {
            print_char( ch, "Za %s w sklepie dosta³%sby¶ kilka monet.\n\r", obj->name4, ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
        }
        else if ( obj->cost	<   51 * RATTING_SILVER  )
        {
            print_char( ch, "Za %s w sklepie dosta³%sby¶ kilkadziesi±t monet.\n\r", obj->name4, ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
        }
        else if	( obj->cost <  101 * RATTING_SILVER  )
        {
            print_char( ch, "Za %s w sklepie dosta³%sby¶ zgrabn± kupkê monet.\n\r",	obj->name4,	ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
        }
        else if	( obj->cost <  501 * RATTING_SILVER  )
        {
            print_char( ch, "Za %s w sklepie dosta³%sby¶ spor± kupkê monet.\n\r", obj->name4, ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
        }
        else if	( obj->cost < 1001 * RATTING_SILVER  )
        {
            print_char( ch, "Za %s w sklepie dosta³%sby¶ bardzo du¿± kupkê monet.\n\r", obj->name4, ch->sex	== 2 ? "a" : ch->sex ==	0 ? "o"	: "" );
        }
        else
        {
            print_char( ch, "%s to niezwykle cenny przedmiot.\n\r", capitalize( obj->short_descr ));
        }

        if ( skill*2 > number_range(1,100))
        {
            if ( IS_OBJ_STAT( obj, ITEM_UNDESTRUCTABLE ) )
            {
                print_char( ch, "%s emanuje niezwyk³± magi±, ¿adna moc nie by³aby w stanie zniszczyc tego przedmiotu.\n\r", capitalize( obj->short_descr ) );
            }
        }
        if ( skill*2 > number_range(1,100))
        {
            if ( IS_OBJ_STAT( obj, ITEM_EVIL ) )
            {
                print_char( ch, "Od %s bije zimna aura z³a.\n\r", obj->name2 );
            }
        }
        if ( skill*2 > number_range(1,100))
        {
            if ( IS_OBJ_STAT( obj, ITEM_DARK ) )
            {
                print_char( ch, "Od %s bije mroczna aura.\n\r", obj->name2 );
            }
        }
        if ( skill*3 > number_range(1,100))
        {
            if ( IS_OBJ_STAT( obj, ITEM_MAGIC )	)
            {
                print_char( ch, "Dotykaj±c %s odczuwasz delikatn±, magiczn± aurê.\n\r", obj->name2 );
            }
        }
        if ( skill*3 > number_range(1,100))
        {
            if ( IS_OBJ_STAT( obj, ITEM_BURN_PROOF ) )
            {
                print_char( ch, "Przedmiot ten wydaje siê byæ odporny na jakiekolwiek oddzia³ywanie ognia.\n\r", obj->name2 );
            }
        }
        if ( skill*3 > number_range(1,100))
        {
            if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
            {
                print_char( ch, "%s emanuje lekk±, koj±c± aur±.\n\r", obj->name2 );
            }
        }

        switch ( obj->item_type )
        {
            case ITEM_SCROLL:
                if ( skill+30 < number_range(1,100))
                {
                    act( "Ogl±dasz $h z zaciekawieniem ale nie udaje ci siê niczego wyczytaæ.", ch, obj, NULL, TO_CHAR );
                    break;
                }
                pass = 1;
                break;

            case ITEM_POTION:
                if ( pass != 1 )
                {
                    if ( skill+30 < number_range(1,100))
                    {
                        act( "Ogl±dasz $h z zaciekawieniem ale nie udaje ci siê zidentyfikowaæ co to za p³yn.", ch, obj, NULL, TO_CHAR );
                        break;
                    }
                }
                pass = 1;
                break;

            case ITEM_PILL:
                if ( pass != 1 )
                {
                    if ( skill+30 < number_range(1,100))
                    {
                        act( "Ogl±dasz $h z zaciekawieniem ale nie udaje ci siê tego zidentyfikowaæ.", ch, obj, NULL, TO_CHAR );
                        break;
                    }
                }
                pass = 0;

                sprintf( buf, "Poziom %d czarów:", obj->value[ 0 ] );
                send_to_char( buf, ch );

                if ( obj->value[ 1 ] >= 0 && obj->value[ 1 ] < MAX_SKILL )
                {
                    send_to_char( "	'", ch );
                    send_to_char( skill_table[ obj->value[ 1 ] ].name, ch );
                    send_to_char( "'", ch );
                }

                if ( obj->value[ 2 ] >= 0 && obj->value[ 2 ] < MAX_SKILL )
                {
                    send_to_char( "	'", ch );
                    send_to_char( skill_table[ obj->value[ 2 ] ].name, ch );
                    send_to_char( "'", ch );
                }

                if ( obj->value[ 3 ] >= 0 && obj->value[ 3 ] < MAX_SKILL )
                {
                    send_to_char( "	'", ch );
                    send_to_char( skill_table[ obj->value[ 3 ] ].name, ch );
                    send_to_char( "'", ch );
                }

                if ( obj->value[ 4 ] >= 0 && obj->value[ 4 ] < MAX_SKILL )
                {
                    send_to_char( "	'", ch );
                    send_to_char( skill_table[ obj->value[ 4 ] ].name, ch );
                    send_to_char( "'", ch );
                }

                send_to_char( ".\n\r", ch );
                break;

            case ITEM_STAFF:
                if (skill+number_range(10,30) > number_range(1,100))
                {
                    sprintf( buf, "Ma jeszcze %d ³adunków poziomu %d.\n\r",
                            obj->value[ 2 ], obj->value[ 0 ] );
                    send_to_char( buf, ch );

                    if ( obj->value[ 3 ] >= 0 && obj->value[ 3 ] < MAX_SKILL )
                    {
                        send_to_char( "	'", ch );
                        send_to_char( skill_table[ obj->value[ 3 ] ].name, ch );
                        send_to_char( "'", ch );
                    }

                    send_to_char( ".\n\r", ch );
                }
                else
                {
                    sprintf( buf, "Ma jeszcze %d ³adunków poziomu %d.\n\r",
                            number_range(1,ch->hit/50), dice(2,ch->level) );
                    send_to_char( buf, ch );
                    send_to_char( "	'", ch );
                    send_to_char( skill_table[ number_range(1,MAX_SKILL) ].name, ch );
                    send_to_char( "'", ch );
                    send_to_char( ".\n\r", ch );
                }
                break;

            case ITEM_DRINK_CON:
                if(skill*2+25 > number_range(1,100))
                {
                    sprintf( buf, "W tym jest %s (%s).\n\r",
                            liq_table[ obj->value[ 2 ] ].liq_name,
                            liq_table[ obj->value[ 2 ] ].liq_color );
                    send_to_char( buf, ch );
                }
                else
                {
                    sprintf( buf, "W tym jest %s (%s).\n\r",
                            liq_table[ number_range(0,41) ].liq_name,
                            liq_table[ number_range(0,41) ].liq_color );
                    send_to_char( buf, ch );
                }
                break;

            case ITEM_CONTAINER:
                if(skill+25 > number_range(1,100))
                {
                    sprintf( buf, "Pojemno¶æ: %d#  Maksymalna no¶no¶æ: %d#  flagi: %s\n\r",
                            obj->value[ 0 ], obj->value[ 3 ], cont_bit_name( obj->value[ 1 ] )	);
                    send_to_char( buf, ch );
                    if ( obj->value[ 4 ] != 100	)
                    {
                        sprintf( buf, "Mno¿nik wagi: %d%%\n\r",
                                obj->value[ 4 ] );
                        send_to_char( buf, ch );
                    }
                }
                else
                {
                    sprintf( buf, "Pojemno¶æ: %d#  Maksymalna no¶no¶æ: %d#  flagi: %s\n\r",
                            number_range(50,250), number_range(50,250), cont_bit_name( obj->value[ 1 ] ));
                    send_to_char( buf, ch );
                    sprintf( buf, "Mno¿nik wagi: %d%%\n\r",
                            number_range(50,300));
                    send_to_char( buf, ch );
                }
                break;

            case ITEM_WEAPON:
                send_to_char( "Typ broni: ", ch );
                switch ( obj->value[ 0 ] )
                {
                    case( WEAPON_EXOTIC ) : send_to_char( "'nieznany'.\n\r", ch );	break;
                    case( WEAPON_SWORD ) : send_to_char( "'miecz'.\n\r", ch	);	break;
                    case( WEAPON_DAGGER ) :	send_to_char( "'sztylet'.\n\r", ch );	break;
                    case( WEAPON_SPEAR ) : send_to_char( "'w³ócznia'.\n\r", ch );	break;
                    case( WEAPON_MACE ) : send_to_char( "'maczuga'.\n\r", ch );	break;
                    case( WEAPON_AXE ) : send_to_char( "'topór'.\n\r", ch );	break;
                    case( WEAPON_FLAIL ) : send_to_char( "'cep'.\n\r", ch );	break;
                    case( WEAPON_WHIP ) : send_to_char( "'bat'.\n\r", ch );	break;
                    case( WEAPON_POLEARM ) : send_to_char( "'broñ drzewcowa'.\n\r", ch );	break;
                    case( WEAPON_STAFF ) : send_to_char( "'laska'.\n\r", ch	);	break;
                    case( WEAPON_SHORTSWORD	) : send_to_char( "'krótki miecz'.\n\r", ch );	break;
                    case( WEAPON_CLAWS ) : send_to_char( "'szpony'.\n\r", ch ); break;
                    default	: send_to_char( "nieznanego.\n\r", ch );	break;
                }

                if (skill+15 > number_range(1,100))
                {
                    sprintf( buf, "Premia do trafienia: %d.\n\r", obj->value[ 5 ] );
                    send_to_char( buf, ch );
                }
                else
                {
                    sprintf( buf, "Premia do trafienia: %d.\n\r", number_range(-10,10) );
                    send_to_char( buf, ch );
                }

                if (skill+15 > number_range(1,100))
                {
                    sprintf( buf, "Obra¿enia zadawane %dd%d + %d (¶rednio %d).\n\r",
                            obj->value[ 1 ], obj->value[ 2 ], obj->value[ 6 ],
                            ( obj->value[ 1 ] * obj->value[ 2 ] + obj->value[ 1 ] + 2 * obj->value[ 6 ] ) / 2 );
                    send_to_char( buf, ch );
                }
                else
                {
                    sprintf( buf, "Obra¿enia zadawane %dd%d + %d (¶rednio %d).\n\r",
                            number_range(1,7), number_range(1,10), number_range(0,7),( number_range(1,25) ));
                    send_to_char( buf, ch );
                }

                if(skill+15 > number_range(1,100))
                {
                    if ( obj->value[ 4 ] )
                    {
                        if ( IS_SET( obj->value[ 4 ], WEAPON_FLAMING ) )
                            print_char( ch, "%s okrywa ledwo widoczna, ognista aura.\n\r", capitalize( obj->name4 )	);

                        if ( IS_SET( obj->value[ 4 ], WEAPON_FROST ) )
                            print_char( ch, "%s okrywa ledwo widoczna, zamra¿aj±ca aura.\n\r", capitalize( obj->name4 ) );

                        if ( IS_SET( obj->value[ 4 ], WEAPON_SHOCKING )	)
                            print_char( ch, "W %s zgromadzono pote¿ny, elektryczny ³adunek.\n\r", obj->name5 );

                        if ( IS_SET( obj->value[ 4 ], WEAPON_SHARP ) )
                            print_char( ch, "Ostrze %s jest niezwykle ostre.\n\r", obj->name2 );

                        if ( IS_SET( obj->value[ 4 ], WEAPON_VORPAL ) )
                            print_char( ch,	"Ostrze %s jest niesamowicie ostre, z ³atwo¶ci¶ przeci%sby¶ t± broni± granitowa ska³ê.\n\r", obj->name2, ch->sex == 2 ? "e³a" : ch->sex == 0 ? "e³o" : "±³" );

                        if ( IS_SET( obj->value[ 4 ], WEAPON_POISON ) )
                            print_char( ch, "Na %s znajdujesz kilka ciemnych plam po jakiej¶ ¶mierdzacej cieczy, ta broñ jest zatruta.\n\r", obj->name5 );

                        if ( IS_SET( obj->value[ 4 ], WEAPON_VAMPIRIC ) )
                            print_char( ch, "%s okrywa ledwo widoczna, czarna aura, wysysaj±ca ¿ycie z otoczenia.\n\r", capitalize( obj->name4 )	);

                        if ( IS_SET( obj->value[ 4 ], WEAPON_DISPEL ) )
                            print_char( ch, "%s okrywa ledwo widoczna, rozpraszaj±ca magiê bia³a aura.\n\r", capitalize( obj->name4 )	);

                        if ( IS_SET( obj->value[ 4 ], WEAPON_TOXIC ) )
                            print_char( ch, "%s okrywa ledwo widoczna, ¿r±ca aura.\n\r", capitalize( obj->name4 ) );

                        if ( IS_SET( obj->value[ 4 ], WEAPON_SACRED ) )
                            print_char( ch, "%s okrywa potê¿na, ¶wiêta aura.\n\r", capitalize( obj->name4 ) );

                        if ( IS_SET( obj->value[ 4 ], WEAPON_RESONANT ) )
                            print_char( ch, "Z %s wydobywaj± siê lekkie wibracje.\n\r", obj->name2 );

                        if ( IS_SET( obj->value[ 4 ], WEAPON_KEEN ) )
                            print_char( ch, "Trzymaj±c w d³oniach %s zauwa¿asz, ¿e broñ ta jest doskonale wywa¿ona.\n\r", obj->name2 );

                        if ( IS_SET( obj->value[ 4 ], WEAPON_THUNDERING ) )
                            print_char( ch, "%s wibruje lekko od zawartej w ¶rodku mocy.\n\r", obj->name2 );

                    }
                }
                break;

            case ITEM_ARMOR:
                if (skill+15 > number_range(1,100))
                {
                    sprintf( buf,
                            "Klasa pancerza: %d k³uj±ce, %d obuchowe, %d ciête\n\r",
                            obj->value[ 0 ], obj->value[ 1 ], obj->value[ 2 ] );
                    send_to_char( buf, ch );
                }
                else
                {
                    sprintf( buf,
                            "Klasa pancerza: %d k³uj±ce, %d obuchowe, %d ciête\n\r",
                            number_range(-2,10), number_range(-2,10), number_range(-2,10));
                    send_to_char( buf, ch );
                }
                break;

            case ITEM_MUSICAL_INSTRUMENT:
                send_to_char( "Typ instrumentu: ", ch );
                switch ( obj->value[ 0 ] )
                {
                    case( INSTRUMENT_OCARINA ) : send_to_char( "'okaryna'.\n\r", ch );	break;
                    case( INSTRUMENT_FLUTE ) : send_to_char( "'flet'.\n\r", ch	);	break;
                    case( INSTRUMENT_LUTE ) :	send_to_char( "'lutnia'.\n\r", ch );	break;
                    case( INSTRUMENT_HARP ) : send_to_char( "'harfa'.\n\r", ch );	break;
                    case( INSTRUMENT_MANDOLIN ) : send_to_char( "'mandolina'.\n\r", ch );	break;
                    case( INSTRUMENT_PIANO ) : send_to_char( "'fortepian'.\n\r", ch );	break;
                    default	: send_to_char( "nieznanego.\n\r", ch );	break;
                }
                if ( skill+50 > number_range(1,100))
                {
                    sprintf( buf, "Premia do umiejêtno¶ci: %d.\n\r", obj->value[ 1 ] );
                    send_to_char( buf, ch );
                }
                if ( obj->value[ 4 ] == 100 ) send_to_char( "Instrument jest doskonale nastrojony.\n\r", ch );
                else if ( obj->value[ 4 ] > 80 ) send_to_char( "Instrument jest dobrze nastrojony.\n\r", ch );
                else if ( obj->value[ 4 ] > 60 ) send_to_char( "Instrument jest do¶æ dobrze nastrojony.\n\r", ch );
                else if ( obj->value[ 4 ] > 40 ) send_to_char( "Instrument jest kiepsko nastrojony.\n\r", ch );
                else if ( obj->value[ 4 ] > 20 ) send_to_char( "Instrument jest do¶æ mocno rozstrojony.\n\r", ch );
                else send_to_char( "Instrument prawie nie nadaje siê do u¿ytku.\n\r", ch );
                break;

            case ITEM_TURN:
                if ( skill+25 > number_range(1,100))
                {
                    sprintf( buf, "Premia do umiejêtno¶ci: %d.\n\r", obj->value[ 0 ] );
                    send_to_char( buf, ch );
                }
                if ( skill+25 > number_range(1,100))
                {
                    sprintf( buf, "Premia do obra¿eñ: %d.\n\r", obj->value[ 1 ] );
                    send_to_char( buf, ch );
                }
                break;

            case ITEM_LIGHT:
                if ( skill+25 > number_range(1,100))
                {
                    if ( obj->value[ 2 ] == -1 )
                    {
                        act( "Aura magii otaczaj±ca $h mowi ci, ¿e to ¼ród³o ¶wiat³a siê nigdy nie wypali.", ch, obj, NULL, TO_CHAR );
                    }
                    else if ( obj->value[ 2 ] > 24 )
                    {
                        print_char( ch, "%s po¶wieci jeszcze d³u¿ej ni¿ doba.\n\r", capitalize( obj->short_descr ) );
                    }
                    else if ( obj->value[ 2 ] > 18 )
                    {
                        print_char( ch, "%s po¶wieci jeszcze prawie ca³y dzieñ.\n\r", capitalize( obj->short_descr ) );
                    }
                    else if ( obj->value[ 2 ] > 12 )
                    {
                        print_char( ch, "%s po¶wieci jeszcze parena¶cie godzin.\n\r", capitalize( obj->short_descr ) );
                    }
                    else if ( obj->value[ 2 ] > 8 )
                    {
                        print_char( ch, "%s po¶wieci jeszcze prawie pó³ doby.\n\r", capitalize( obj->short_descr ) );
                    }
                    else if ( obj->value[ 2 ] > 5 )
                    {
                        print_char( ch, "%s po¶wieci jeszcze kilka godzin.\n\r", capitalize( obj->short_descr ) );
                    }
                    else if ( obj->value[ 2 ] > 2 )
                    {
                        print_char( ch, "%s po¶wieci jeszcze z trzy, cztery godziny.\n\r", capitalize( obj->short_descr ) );
                    }
                    else if ( obj->value[ 2 ] == 2 )
                    {
                        print_char( ch, "%s po¶wieci jeszcze mo¿e z dwie godziny.\n\r", capitalize( obj->short_descr ) );
                    }
                    else if ( obj->value[ 2 ] == 1 )
                    {
                        print_char( ch, "%s zaczyna przygasaæ.\n\r", capitalize( obj->short_descr ) );
                    }
                    else
                    {
                        print_char( ch, "%s nie daje ju¿ ¶wiat³a.\n\r", capitalize( obj->short_descr ) );
                    }
                }
                if ( skill+25 > number_range(1,100))
                {
                    if( obj->value[0] == 1 )
                        print_char( ch, "%s nie zga¶nie pod wod±.\n\r", capitalize( obj->short_descr ) );
                    else if ( obj->value[0] == 0 )
                        print_char( ch, "%s zga¶nie pod wod±.\n\r", capitalize( obj->short_descr ) );
                }
                break;
        }

        if (skill+35 > number_range(1,100))
        {
            if ( !obj->enchanted )
            {
                for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
                {
                    if ( paf->location != APPLY_NONE &&	paf->modifier != 0 && paf->where != TO_WEAPON )
                    {
                        if ( paf->location == APPLY_SKILL )
                            sprintf( buf, "Wp³ywa na umiejêtno¶æ '%s' o %d.\n\r",
                                    skill_table[ paf->modifier	].name, paf->level );
                        else if	( paf->location == APPLY_LANG )
                            sprintf( buf, "Wp³ywa na znajomo¶æ jêzyka '%s' o %d.\n\r",
                                    lang_table[ paf->modifier ].name, paf->level );
                        else if	( paf->location == APPLY_MEMMING )
                            sprintf( buf, "Wp³ywa na ilo¶æ mo¿liwych zakleæ do zapamietania z %d krêgu o %d.\n\r",
                                    paf->modifier, paf->level );
                        else if	( paf->location == APPLY_RESIST	)
                            sprintf( buf, "Wp³ywa na odporno¶æ na '%s' o %d.\n\r",
                                    resist_name( paf->modifier	), paf->level );
                        else
                            sprintf( buf, "Wp³ywa na %s o %d.\n\r",
                                    affect_loc_name( paf->location ), paf->modifier );

                        send_to_char( buf, ch );

                        if ( paf->bitvector && paf->bitvector != &AFF_NONE )
                        {
                            switch ( paf->where	)
                            {
                                case TO_AFFECTS:
                                    sprintf( buf, "Dodaje %s.\n",
                                            affect_bit_name( NULL, paf->bitvector ) );
                                    break;
                                case TO_OBJECT:
                                    sprintf( buf, "Daje flagê %s.\n",
                                            ext_bit_name( extra_flags, paf->bitvector ) );
                                    break;
                                case TO_WEAPON:
                                    sprintf( buf, "Daje flagê %s.\n",
                                            weapon_bit_name( paf->location	) );
                                    break;
                            }
                            send_to_char( buf, ch );
                        }
                    }
                }
            }

            for	( paf =	obj->affected; paf != NULL; paf = paf->next )
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
                                affect_loc_name( paf->location	), paf->modifier );

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
                                sprintf( buf, "Daje flagê %s.\n",
                                        ext_bit_name( extra_flags, paf->bitvector ) );
                                break;
                            case TO_WEAPON:
                                sprintf( buf, "Daje flagê %s.\n",
                                        weapon_bit_name( paf->location	) );
                                break;
                        }
                        send_to_char( buf, ch );
                    }
                }
            }
        }
    }
    else
    {
        print_char( ch, "Nie ma tu nikogo ani niczego takiego jak '%s'.\n\r", arg );
    }

    return;
}

void do_wardance( CHAR_DATA *ch, char *argument )
{
    int chance, mod, ac_bonus;
    AFFECT_DATA af;
    OBJ_DATA *weapon;
    bool has_feat_dancer_of_war;

    if ( ( chance = get_skill( ch, gsn_wardance ) ) == 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( !ch->fighting )
    {
        send_to_char( "Przecie¿ nie walczysz.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_BERSERK ) || is_affected( ch, gsn_wardance )
            || is_affected( ch, skill_lookup( "frenzy" ) ) )
    {
        send_to_char( "Skupiasz siê jeszcze bardziej na walce.\n\r", ch );
        return;
    }

    if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char( "Postaraj siê o jak±¶ broñ.\n\r", ch );
        return;
    }
    if (
            !IS_NPC( ch )
            && ch->condition[ COND_DRUNK ] > DRUNK_FULL
       )
    {
        chance /= 3;
    }

    has_feat_dancer_of_war = FALSE;

    if ( !IS_NPC( ch ) && is_affected(ch,gsn_on_smoke) && !has_feat_dancer_of_war )
    {
        if (IS_AFFECTED(ch,AFF_HALLUCINATIONS_POSITIVE)||IS_AFFECTED(ch,AFF_HALLUCINATIONS_NEGATIVE))
            chance /= 4;
        else
            chance /= 2;
    }

    if ( IS_AFFECTED( ch, AFF_CALM ) )
    {
        chance += 8;
    }

    if ( has_feat_dancer_of_war )
    {
        chance += 15;
    }

    chance = UMIN( 95, chance );

    if ( number_percent() < chance )
    {
        WAIT_STATE( ch, PULSE_VIOLENCE );

        mod = 2 + get_skill( ch, gsn_wardance )/33;
        ac_bonus = - get_skill( ch, gsn_wardance )/4;

        if( has_feat_dancer_of_war )
        {
            mod *=2;
            ac_bonus *= 2;
        }

        send_to_char( "{5Zaczynasz dok³adniej koncentrowaæ siê na walce, czujesz jak twój puls przy¶piesza a razem z nim ro¶nie precyzja i szybko¶æ zadawanych ciosów.{x\n\r", ch );
        act( "{5$n nagle bierze g³êboki wdech a $s ciosy staj± siê mniej chaotyczne, a bardziej zgrane.{x", ch, NULL, ch->fighting, TO_VICT );
        act( "{5$n nagle bierze g³êboki wdech, po czym zdaje siê wrêcz prowadzic jaki¶ ¶miertelny taniec, balansuj±c cia³em i wyprowadzaj±c ciosy.{x", ch, NULL, ch->fighting, TO_NOTVICT );
        check_improve( ch, NULL, gsn_wardance, TRUE, 15 );

        af.where	= TO_AFFECTS;
        af.type	= gsn_wardance;
        af.level	= ch->level;
        af.duration = 3;         af.rt_duration = 0;
        af.bitvector = &AFF_BERSERK;
        af.modifier	= 1 + ( get_skill(ch,gsn_wardance) > 50 ? 1 : 0 );
        af.location	= APPLY_DEX;
        af.visible = TRUE;
        affect_to_char( ch, &af, NULL, TRUE );

        af.bitvector = &AFF_FREE_ACTION;
        af.modifier	= mod/3;
        af.location	= APPLY_HITROLL;
        affect_to_char( ch, &af, NULL, TRUE );

        if( !has_feat_dancer_of_war )
        {
            af.bitvector = &AFF_NONE;
            af.modifier	= - ( get_skill(ch,gsn_wardance) > 50 ? 1 : 2 );
            af.location	= APPLY_DAMROLL;
            affect_to_char( ch, &af, NULL, TRUE );
        }

        af.bitvector = &AFF_NONE;
        af.modifier	= ac_bonus;
        af.location	= APPLY_AC;
        affect_to_char( ch, &af, NULL, TRUE );
    }
    else
    {
        WAIT_STATE( ch, PULSE_VIOLENCE );
        send_to_char( "{5Starasz siê skoncentrowaæ, jednak adrenalina sprawia, ¿e zamiast precyzyjnych i ¶miertelnie skutecznych ciosów wyprowadzasz tylko chaotyczne uderzenia.{x\n\r", ch );

        if ( ch->sex == 2 )
            act( "{5$n próbuje siê skoncentrowaæ, jednak po chwili daje za wygran±.{x", ch, NULL, NULL, TO_ROOM );
        else
            act( "{5$n próbuje siê skoncentrowac, jednak nie przynosi to ¿adnych efektów.{x", ch, NULL, NULL, TO_ROOM );

        check_improve( ch, NULL, gsn_wardance, FALSE, 20 );
    }

    return;
}

void do_dodge( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * vch = NULL;
    int skill, chance;

    if ( ( skill = get_skill( ch, gsn_dodge ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->fighting == NULL )
    {
        send_to_char( "Z nikim nie walczysz.\n\r", ch );
        return;
    }

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
        if ( vch->fighting == ch )
            break;

    if ( !vch )
    {
        send_to_char( "Nie jeste¶ nara¿on<&y/a/e> na ciosy, wiêc nie ma za bardzo czego unikaæ.\n\r", ch );
        return;
    }

    chance = (skill*3)/4;

    chance += 2 * URANGE( 0, get_curr_stat_deprecated( ch, STAT_DEX ) - 18, 10 );

    switch ( ch->size )
    {
        case SIZE_TINY: chance += 20;	break;
        case SIZE_SMALL: chance += 10;	break;
        default: break;
    }

    //rasowa premia dla niziolkow
    if ( !str_cmp( race_table[ GET_RACE(ch) ].name, "nizio³ek" ) )
        chance += 4;

    if ( number_percent() < chance )
    {
        print_char( ch, "{5Bêdziesz teraz wykonywa³%s uniki.{x\n\r", ch->sex == 2 ? "a" : ch->sex == 0 ? "o" : "" );
        SET_BIT( ch->fight_data, FIGHT_DODGE );
        check_improve( ch, NULL, gsn_dodge, TRUE, 60 );
    }
    else
    {
        send_to_char( "{5Nie uda³o ci siê.{x\n\r", ch );
        check_improve( ch, NULL, gsn_dodge, FALSE, 50 );
    }
    WAIT_STATE( ch, skill_table[ gsn_dodge ].beats );
    return;
}

/* stun - skill powodujacy oszolomienie u przeciwnika
 * czyli: 75% na nieudane rzucenie zaklecia
 * skille spadaja do 1/4 normalnej wartosci
 * trudniej przeciwnikowi trafic, natomiast jego znacznie latwiej sypac
 * wymaga broni obuchowej
 * klerycy beda mieli stuna, ale takiego na 1 runde
 * paladyni i wojki beda mieli ladnego 2-3 rundowego stuna
 */
void do_stun( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * vch;
    OBJ_DATA *wield, *helmet;
    int skill, chance;

    if ( ( skill = get_skill( ch, gsn_stun ) ) == 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ( wield = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char( "Postaraj siê o jak±¶ broñ obuchow±!\n\r", ch );
        return;
    }

    if ( wield->value[ 0 ] != WEAPON_MACE &&
            wield->value[ 0 ] != WEAPON_FLAIL &&
            wield->value[ 0 ] != WEAPON_STAFF )
    {
        send_to_char( "Lepiej znajd¼ jak±¶ broñ obuchow±!\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        if ( ch->fighting )
            vch = ch->fighting;
        else
        {
            send_to_char( "Kogo chcesz og³uszyæ?\n\r", ch );
            return;
        }
    }
    else if ( ( vch = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        add_wait_after_miss(ch, 2);
        return;
    }

    if ( ch == vch )
    {
        send_to_char( "{5Potê¿nie walisz siê w ³eb, z zaciekawieniem przygl±dasz siê orbituj±cym wokó³ ciebie gwiazdkom.{x\n\r", ch );
        return;
    }

    if ( is_safe( ch, vch ) )
    {
        send_to_char( "Nie tutaj.\n\r", ch );
        return;
    }

    if ( !IS_SET( vch->parts, PART_HEAD ) )
    {
        print_char( ch, "{5Nie bardzo wiesz jak mo¿na og³uszyæ %s.{x\n\r", PERS4( vch, ch ) );
        return;
    }

    if ( !form_check( ch, vch, wield, gsn_stun ) )
        return;

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == vch )
    {
        send_to_char( "No co ty...przecie¿ to twój przyjaciel.\n\r", ch );
        return;
    }

    if ( 3*ch->height < 2*vch->height && vch->position >= POS_FIGHTING )
    {
        act( "Nie dasz rady siêgn±æ do g³owy $Z.", ch, NULL, vch, TO_CHAR);
        return;
    }

    if( check_trick( ch, vch, SN_TRICK_THUNDERING_WHACK ) )
        return;

    chance = (66*skill)/100;

    if ( skill > 90 ) //dla mistrzow dodatkowa premia
        chance += number_range(5,10);

    //zaleznosc od sily
    chance += get_curr_stat_deprecated( ch, STAT_STR ) / 2;

    //niewielka zaleznosc od lucka ch i vch
    if ( ch_vs_victim_stat_throw( ch, vch, STAT_LUC ) )
        chance += 3;
    else
        chance -= 3;

    //niewielka zaleznosc od dexa ch i vch
    if ( ch_vs_victim_stat_throw( ch, vch, STAT_DEX ) )
        chance += 7;
    else
        chance -= 4;

    helmet = get_eq_char( vch, WEAR_HEAD );

    //im cel ma lepszy helm tym trudniej stunnac
    if ( helmet && helmet->item_type == ITEM_ARMOR )
    {
        chance -= 10;
        chance += helmet->value[1]*2; //ac na obrazenia obuchowe, od 0 do 10
    }

    chance = URANGE( 3, chance , 95 );

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( number_percent() < chance || !can_move( vch ) )
    {
        act( "{5$n bierze potê¿ny zamach i z krzykiem rzuca siê na $C próbuj±c $M og³uszyæ.{x", ch, wield, vch, TO_NOTVICT );
        act( "{5Bierzesz potê¿ny zamach i z krzykiem rzucasz siê na $C próbuj±c $M og³uszyæ.{x", ch, wield, vch, TO_CHAR );
        act( "{5$n bierze potê¿ny zamach i z krzykiem rzuca siê na ciebie próbuj±c ciê og³uszyæ!{x", ch, wield, vch, TO_VICT );

        check_improve( ch, vch, gsn_stun, TRUE, 50 );
        one_hit( ch, vch, gsn_stun, FALSE );
        return;
    }
    else
    {
        if ( ch->sex == 2 )
        {
            act( "{5$n bierze potê¿ny zamach i z krzykiem rzuca siê na $C próbuj±c $M og³uszyæ, jednak potyka siê i ledwo udaje jej siê zadaæ cios.{x", ch, wield, vch, TO_NOTVICT );
            act( "{5$n bierze potê¿ny zamach i z krzykiem rzuca siê na ciebie próbuj±c ciê og³uszyæ, jednak potyka siê i ledwo udaje jej siê zadaæ cios.{x", ch, wield, vch, TO_VICT );
        }
        else
        {
            act( "{5$n bierze potê¿ny zamach i z krzykiem rzuca siê na $C próbuj±c $M og³uszyæ, jednak potyka siê i ledwo udaje mu siê zadaæ cios.{x", ch, wield, vch, TO_NOTVICT );
            act( "{5$n bierze potê¿ny zamach i z krzykiem rzuca siê na ciebie próbuj±c ciê og³uszyæ, jednak potyka siê i ledwo udaje mu siê zadaæ cios.{x", ch, wield, vch, TO_VICT );
        }

        act( "{5Bierzesz potê¿ny zamach i z krzykiem rzucasz siê na $C próbuj±c $M og³uszyæ, jednak potykasz siê i ledwo udaje ci siê zadaæ cios.{x", ch, wield, vch, TO_CHAR );
        damage( ch, vch, 0, gsn_stun, DAM_BASH, TRUE );
        check_improve( ch, vch, gsn_stun, FALSE, 60 );
        return;
    }
    return;
}

/* skladnia: charge <kto>
   trza jeszcze jakos sensownie pomyslec nad szarza na kogos w
   drugim rzedzie (jak jest tanker)
 */
void do_charge( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * vch, *tch;
    OBJ_DATA *wield;
    int skill, chance;
    bool hth = FALSE, check;

    if ( ( skill = get_skill( ch, gsn_charge ) ) == 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->mount )
    {
        send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
        return;
    }

    if ( ( wield = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char( "Nie masz dobytej odpowiedniej broni do wykonania szar¿y.\n\r", ch );
        return;
    }

    if( is_affected( ch, gsn_glorious_impale ) )
    {
        send_to_char( "Raczej nie dasz rady tego zrobiæ.\n\r", ch );
        return;
    }

    if ( wield->value[ 0 ] == WEAPON_WHIP || wield->value[ 0 ] == WEAPON_DAGGER || wield->value[ 0 ] == WEAPON_SHORTSWORD || wield->value[0] == WEAPON_CLAWS || !IS_WEAPON_STAT( wield, WEAPON_TWO_HANDS ) )
    {
        send_to_char( "Nie masz dobytej odpowiedniej broni do wykonania szar¿y.\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        if ( ch->fighting )
            vch = ch->fighting;
        else
        {
            send_to_char( "Na kogo chcesz szar¿owac?\n\r", ch );
            return;
        }
        for ( tch = ch->in_room->people;tch;tch = tch->next_in_room )
        {
            if ( tch->fighting == ch && can_move( tch ) && !EXT_IS_SET( tch->off_flags, OFF_ZERO_ATTACK ) )
            {
                send_to_char( "Nie masz miejsca na szar¿ê, walcz!\n\r", ch );
                return;
            }
        }
    }
    else if ( ( vch = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        add_wait_after_miss(ch, 2);
        return;
    }

    if ( ch == vch )
    {
        send_to_char( "Jeszcze nikomu siê to nie uda³o.\n\r", ch );
        return;
    }

    if ( !form_check( ch, vch, NULL, gsn_charge ) )
    {
        return;
    }

    if ( is_safe( ch, vch ) )
    {
        return;
    }

    for ( tch = ch->in_room->people;tch;tch = tch->next_in_room )
    {
        if ( tch->fighting == ch && can_move( tch ) && !EXT_IS_SET( tch->off_flags, OFF_ZERO_ATTACK ) )
        {
            send_to_char( "Nie masz miejsca na szar¿ê, walcz!\n\r", ch );
            add_wait_after_miss(ch, 2);
            return;
        }
    }
    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == vch )
    {
        send_to_char( "No co ty... przecie¿ to Twój przyjaciel.\n\r", ch );
        return;
    }

    chance = skill;

    chance += get_curr_stat_deprecated( ch, STAT_DEX ) - 10;

    if( hth )//z wlasciwosci feata charga w walce mozna tylko w tanka, bo zajebiscie bugogenne bylo po polaczeniu z autoassistem feata prawdziwy berserker
    {
        if (( ch->fighting != NULL) && ( vch != ch->fighting ))
            vch = ch->fighting;
    }

    switch ( wield->value[ 0 ] )
    {
        case WEAPON_SWORD:
            chance += 5;
            break;
        case WEAPON_AXE:
            chance += 2;
            break;
        case WEAPON_POLEARM:
            chance -= 2;
            break;
        case WEAPON_SPEAR:
            chance -= 4;
            break;
        case WEAPON_MACE:
            chance += 2;
            break;
        case WEAPON_FLAIL:
            chance -= 6;
            break;
        default:
            break;
    }

    switch ( vch->size )
    {
        case SIZE_TINY:
        case SIZE_SMALL:
            chance -= 10;
            break;
        case SIZE_LARGE:
            chance += 5;
            break;
        case SIZE_HUGE:
            chance += 10;
            break;
        case SIZE_GIANT:
            chance += 15;
        default:
            break;
    }

    if ( IS_AFFECTED( ch, AFF_SLOW ) )
    {
        chance /= 4;
    }
    else if ( IS_AFFECTED( ch, AFF_HASTE ) )
    {
        chance += 10;
    }

    switch ( crowd_check( ch, vch ) )
    {
        case 0:
        case 1: break;
        case 2: chance = 80 * chance / 100;	break;
        case 3: chance = 65 * chance / 100;	break;
        case 4: chance = 45 * chance / 100;	break;
        case 5: chance = 30 * chance / 100;	break;
        default: chance = 20 * chance / 100;	break;
    }

    if( hth )//jezeli ma feata i charguje tankujac
    {
        chance /= 3;
        SET_BIT( ch->fight_data, FIGHT_HTH_CHARGE );
    }

    if( EXT_IS_SET( ch->in_room->room_flags, ROOM_SLIPPERY_FLOOR ) && number_percent() > chance/2 && !IS_AFFECTED( ch, AFF_STABILITY ) )
    {
        if( ch->sex == 2 )
        {
            act( "{5$n szar¿owa³a na $C szaleñczo wymachuj±c $j, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgnê³a siê pada z jêkiem na ziemiê.{x", ch, wield, vch, TO_NOTVICT );
            act( "{5$n szar¿owa³a na ciebie szaleñczo wymachuj±c $j, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgnê³a siê pada z jêkiem na ziemiê.{x", ch, wield, vch, TO_VICT );
        }
        else
        {
            act( "{5$n szar¿owa³ na $C szaleñczo wymachuj±c $j, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgn±³ siê pada z jêkiem na ziemiê.{x", ch, wield, vch, TO_NOTVICT );
            act( "{5$n szar¿owa³ na ciebie szaleñczo wymachuj±c $j, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgn±³ siê pada z jêkiem na ziemiê.{x", ch, wield, vch, TO_VICT );
        }
        act( "{5Szar¿ujesz na $C szaleñczo wymachuj±c $j, jednak przez rozlan± na pod³o¿u t³ust± i ¶lisk± substancjê twoja noga po¶lizgnê³a siê i padasz na ziemiê z jêkiem.{x", ch, wield, vch, TO_CHAR );
        damage( ch, vch, 0, gsn_charge, DAM_NONE, FALSE );
        ch->position = POS_SITTING;
        WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
        check_improve( ch, vch, gsn_charge, FALSE, 50 );
        if ( skill < 30 )
        {
            SET_BIT( ch->fight_data, FIGHT_CASTING );
        }
        return;
    }

    if ( number_percent() < chance || !can_move( vch ) )
    {
        act( "{5$n szar¿uje na $C szaleñczo wymachuj±c $j.{x", ch, wield, vch, TO_NOTVICT );
        act( "{5Szar¿ujesz na $C szaleñczo wymachuj±c $j.{x", ch, wield, vch, TO_CHAR );
        act( "{5$n szar¿uje na ciebie szaleñczo wymachuj±c $j!{x", ch, wield, vch, TO_VICT );
        check_improve( ch, vch, gsn_charge, TRUE, 60 );
        one_hit( ch, vch, gsn_charge, FALSE );
        WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
        /* zeby nastepna runda byla bez ataku!*/
        if ( skill < 30 )
        {
            SET_BIT( ch->fight_data, FIGHT_CASTING );
        }
        return;
    }
    else
    {
        switch ( wield->value[ 0 ] )
        {
            case WEAPON_AXE:
            case WEAPON_SWORD:
                act( "{5Szar¿ujesz na $C, jednak potykasz siê o w³asne nogi, ledwie udaje ci siê zadaæ cios.{x", ch, NULL, vch, TO_CHAR );
                act( "{5$n szar¿uje na ciebie, jednak potyka siê o w³asne nogi.{x", ch, NULL, vch, TO_VICT );
                act( "{5$n szar¿uje na $C, jednak potyka siê o w³asne nogi.{x", ch, NULL, vch, TO_NOTVICT );
                break;

            case WEAPON_POLEARM:
                if ( !IS_SET( vch->parts, PART_HEAD ) ) {
                    act( "{5Ruszasz szaleñczo na $C celuj±c w g³owê. Jednak twój cios chybia i $p jedynie skraca $S w³osy o kilka centymentrów.{x", ch, NULL, vch, TO_CHAR );
                    act( "{5$n rusza na ciebie szaleñczo celuj±c w g³owê, jednak uchylasz siê lekko i $S cios jedynie skraca twoje w³osy o kilka centymetrów.{x", ch, NULL, vch, TO_VICT );
                    act( "{5Widzisz jak $n rusza szaleñczo na $C celuj±c w $S g³owê. $N jednak uchyla siê lekko i cios jedynie skraca $S w³osy o kilka centymetrów.{x", ch, wield, vch, TO_NOTVICT );
                }
                else {
                    act( "{5Ruszasz szaleñczo na $C. Jednak twój cios chybia i $p, a broñ przelatuje o d³oñ od celu.{x", ch, NULL, vch, TO_CHAR );
                    act( "{5$n rusza na ciebie szaleñczo, uchylasz siê lekko i $S broñ przelatuje obok ciebie.{x", ch, NULL, vch, TO_VICT );
                    act( "{5Widzisz jak $n rusza szaleñczo na $C. $N jednak uchyla siê lekko i broñ $S przelatuje o d³oñ od celu.{x", ch, wield, vch, TO_NOTVICT );
                }
                break;

            case WEAPON_SPEAR:
                act( "{5Wk³adasz w ten cios ca³± swoj± si³ê, jednak chybiasz $C. K±tem oka dostrzegasz jak $N oddycha z ulg±.{x", ch, NULL, vch, TO_CHAR );
                if ( !IS_SET( vch->parts, PART_EAR ) ) {
                    act( "{5S³yszysz przera¼liwy ¶wist, to $p $z przebija powietrze tu¿ ko³o twojego ucha!{x", ch, wield, vch, TO_VICT );
                }
                else {
                    act( "{5S³yszysz przera¼liwy ¶wist, to $p $z przebija powietrze tu¿ ko³o ciebie!{x", ch, wield, vch, TO_VICT );
                }
                act( "{5$n wykonuje potê¿ne pchniêcie celuj±c w $Z, jednak chybia! Dostrzegasz jak $N oddycha z ulg±.{x", ch, NULL, vch, TO_NOTVICT );
                break;
            case WEAPON_FLAIL:
            case WEAPON_STAFF:
            case WEAPON_MACE:
                act( "{5Wk³adasz w ten cios ca³± swoj± si³ê, ale w ostatniej chwili twoja broñ lekko wysuwa ci siê z r±k i z ledwo udaje ci siê zadaæ cios.{x", ch, NULL, vch, TO_CHAR );
                act( "{5$n szar¿uje na ciebie trzymaj±c w rêkach $h, na szczê¶cie gdy jest ju¿ trzy kroki od ciebie broñ lekko wysuwa siê z $s r±k.{x", ch, wield, vch, TO_VICT );
                act( "{5$n szar¿uje na $C trzymaj±c w rêkach $h, ale w ostatniej chwili broñ lekko wysuwa siê z $s r±k.{x", ch, NULL, vch, TO_NOTVICT );
                break;
        }
        one_hit( ch, vch, TYPE_UNDEFINED, FALSE );
        WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
        check_improve( ch, vch, gsn_charge, FALSE, 50 );
        return;
    }

    return;
}

void do_assist( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim = NULL, *wch;
    char arg[ MAX_INPUT_LENGTH ];
  	save_debug_info("skills.c => do_assist", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );


    one_argument( argument, arg );

    if ( ch->fighting )
    {
        print_char( ch, "Jeste¶ teraz zbyt %s.\n\r", ch->sex == 2 ? "zajêta" : "zajêty" );
        return;
    }

    if ( arg[ 0 ] != '\0' )
    {
        if ( !( victim = get_char_room( ch, arg ) ) )
        {
            send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
            return;
        }

    }
    else
    {
        for ( wch = ch->in_room->people; wch != NULL; wch = wch->next_in_room )
            if ( wch != ch &&
                    wch->fighting &&
                    ( is_same_group( ch, wch ) ||
                      ( ch->master == wch && IS_AFFECTED( ch, AFF_CHARM ) ) ||
                      ( wch->master == ch && IS_AFFECTED( wch, AFF_CHARM ) ) ) )
            {
                victim = wch;
                break;
            }

    }

    if ( !victim )
    {
        send_to_char( "Niestety, nikogo takiego tu nie ma.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Hmm, ciekawostka.\n\r", ch );
        return;
    }

    if ( !victim->fighting )
    {
        print_char( ch, "Przecie¿ %s nie walczy.\n\r", victim->sex == 2 ? "ona" : "on" );
        return;
    }

    if ( !is_same_group( ch, victim ) &&
            !( ch->master == victim && IS_AFFECTED( ch, AFF_CHARM ) ) &&
            !( ch == victim->master && IS_AFFECTED( victim, AFF_CHARM ) ) )
    {
        send_to_char( "Nie wygl±da na to, aby kto¶ potrzebowa³ twojej pomocy.\n\r", ch );
        return;
    }

    if ( !can_see( ch, victim->fighting ) && !IS_NPC( ch ) )
    {
        send_to_char( "Nie wygl±da na to, aby kto¶ potrzebowa³ twojej pomocy.\n\r", ch );
        return;
    }


    if ( !IS_AFFECTED( ch, AFF_SILENCE ) )
    {
        print_char( ch, "Wykrzykujesz zawo³anie bitewne i wspierasz heroicznie %s w walce.\n\r", victim->name4 );
    }
    act( "$n rusza do walki, bohatersko ciê wspomagaj±c.", ch, NULL, victim, TO_VICT );
    act( "$n anga¿uje siê w walkê i wspomaga heroicznie $C.", ch, NULL, victim, TO_NOTVICT );
    ch->fighting = victim->fighting;
    return;
}

void do_berserk( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int chance, hp_bonus, mod, dur = 3, ac_pen;
    bool check;

    if ( !ch->fighting )
    {
        send_to_char( "Przecie¿ nie walczysz.\n\r", ch );
        return;
    }

    if ( ( chance = get_skill( ch, gsn_berserk ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( is_undead( ch ) )
    {
        send_to_char( "Jako istota nieumar³a nie jeste¶ w stanie wpa¶æ sza³ bojowy.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_BERSERK ) || is_affected( ch, gsn_berserk )
            || is_affected( ch, skill_lookup( "frenzy" ) ) )
    {
        send_to_char( "Wpadasz w lekki sza³.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_CALM ) )
    {
        send_to_char( "Nie masz ochoty na szaleñstwa.\n\r", ch );
        return;
    }

    if ( !IS_NPC( ch ) && ch->counter[ 3 ] > 0 )
    {
        send_to_char( "Zmêczenie daje znaæ o sobie, nie masz si³ na ¿adne szaleñstwa.\n\r", ch );
        return;
    }

    mod = 1 + get_skill( ch, gsn_berserk )/33;
    hp_bonus = number_range( 2 * ch->level, 4 * ch->level );
    ac_pen = get_skill(ch,gsn_berserk)/2;

    if ( IS_NPC( ch ) )
    {
        chance += chance / 3;
    }
    else
    {
        if ( ch->condition[ COND_DRUNK ] > DRUNK_FULL )
        {
            chance += 10;
        }
        else
        {
            chance /= 5;
        }
    }

    if ( !IS_NPC( ch ) && is_affected(ch,gsn_on_smoke) )
    {
        chance += 4;
        if (IS_AFFECTED(ch,AFF_HALLUCINATIONS_POSITIVE)||IS_AFFECTED(ch,AFF_HALLUCINATIONS_NEGATIVE))
        {
            chance += 6;
        }
    }

    chance = UMIN( 95, chance );

    if ( number_percent() < chance )
    {
        WAIT_STATE( ch, PULSE_VIOLENCE );

        if( number_range(1,2) == 1)
        {
            send_to_char( "{5Twój puls skacze raptownie i poch³ania ciê sza³!{x\n\r", ch );
            act( "{5$n zaczyna ciê¿ko oddychaæ, a w $s oczach pojawia siê dziki b³ysk.{x", ch, NULL, NULL, TO_ROOM );
        }
        else
        {
            send_to_char( "{5Wydajesz z siebie og³uszaj±cy, bojowy okrzyk i z furi± zaczynasz atakowaæ przeciwnika!{x\n\r", ch );
            act( "{5$n wydaje z siebie og³uszaj±cy, bojowy okrzyk i z furi± zaczyna atakowaæ $C.{x", ch, NULL, ch->fighting, TO_NOTVICT );
            act( "{5$n wydaje z siebie og³uszaj±cy, bojowy okrzyk i z furi± zaczyna ciê atakowaæ.{x", ch, NULL, ch->fighting, TO_VICT );
        }

        if ( ch->level < 11 )
        {
            ch->counter[ 3 ] = number_range( 6, 10 );
        }
        else if ( ch->level < 17 )
        {
            ch->counter[ 3 ] = number_range( 3, 8 );
        }
        else if ( ch->level < 25 )
        {
            ch->counter[ 3 ] = number_range( 2, 6 );
        }
        else {
            ch->counter[ 3 ] = number_range( 1, 4 );
        }

        check_improve( ch, NULL, gsn_berserk, TRUE, 17 );

        af.where	= TO_AFFECTS;
        af.type	= gsn_berserk;
        af.level	= ch->level;
        af.duration = dur;         af.rt_duration = 0;
        af.bitvector = &AFF_BERSERK;
        af.location	= APPLY_DAMROLL;
        af.modifier	= 2 + mod;
        af.visible = TRUE;
        affect_to_char( ch, &af, NULL, TRUE );

        af.location	= APPLY_AC;
        af.modifier	= ac_pen;
        affect_to_char( ch, &af, NULL, TRUE );

        af.location	= APPLY_INT;
        af.modifier	= -mod;
        affect_to_char( ch, &af, NULL, TRUE );

        af.location	= APPLY_WIS;
        af.modifier	= -mod;
        affect_to_char( ch, &af, NULL, TRUE );

        af.location	= APPLY_HIT;
        af.modifier	= hp_bonus;
        af.bitvector = &AFF_FREE_ACTION;
        affect_to_char( ch, &af, NULL, FALSE );
        ch->hit += hp_bonus;
    }
    else
    {
        WAIT_STATE( ch, PULSE_VIOLENCE );

        if( number_range(1,2) == 1 )
        {
            send_to_char( "{5Twój puls przyspiesza, ale nic siê nie dzieje.{x\n\r", ch );
        }
        else
        {
            send_to_char( "{5Czujesz narastaj±c± furiê, glo¶no warczysz przez zaci¶niête zêby.{x\n\r", ch );
        }

        switch ( ch->sex )
        {
            case SEX_NEUTRAL:
                act( "{5$n glo¶no warczy przez zaci¶niête zêby i jest coraz bardziej rozw¶cieczone.{x", ch, NULL, NULL, TO_ROOM );
                break;
            case SEX_FEMALE:
                act( "{5$n glo¶no warczy przez zaci¶niête zêby i jest coraz bardziej rozw¶cieczona.{x", ch, NULL, NULL, TO_ROOM );
                break;
            case SEX_MALE:
            default:
                act( "{5$n g³o¶no warczy przez zaci¶niête zêby i jest coraz bardziej rozw¶cieczony.{x", ch, NULL, NULL, TO_ROOM );
                break;
        }
        check_improve( ch, NULL, gsn_berserk, FALSE, 12 );
    }
}

//Zrekonstruowany troszke bash, Raszer
void do_bash( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    CHAR_DATA *mount;
    OBJ_DATA *shield;
    // *shield_v;
    int chance, chance_p, ogluszenie_ch = 0, ogluszenie_v = 0;
    bool beznogie, trafil, przewrocil, ogluszyl;

    one_argument( argument, arg );

    shield = get_eq_char(ch, WEAR_SHIELD);
    //shield_v = get_eq_char(victim, WEAR_SHIELD);
    //warunki poczatkowe
    if ( ( chance = get_skill( ch, gsn_bash ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->mount )
    {
        act( "Walisz $N w glowe, a $E zrzuca ciê z siebie.", ch, NULL, ch->mount, TO_CHAR );
        act( "$N zrzuca $z z siebie.", ch, NULL, ch->mount, TO_ROOM );
        start_fearing( ch->mount, ch );
        ch->mount->position = POS_RESTING;
        do_dismount_body( ch );
        ch->position = POS_RESTING;
        return;
    }

    if ( arg[ 0 ] == '\0' )
    {
        victim = ch->fighting;
        if ( victim == NULL )
        {
            send_to_char( "Rozgl±dasz siê w poszukiwaniu przeciwnika.\n\r", ch );
            return;
        }
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        add_wait_after_miss(ch, 2);
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Próbujesz wytrz±snac swój mózg, bezskutecznie.\n\r", ch );
        WAIT_STATE( ch, PULSE_VIOLENCE );
        return;
    }

    if ( is_safe( ch, victim ) )
    {
        send_to_char( "Rozpedzasz sie, ale w koncu rezygnujesz, bo tutaj jakos nie wypada.\n\r", ch );
        return;
    }

    if ( !form_check( ch, victim, NULL, gsn_bash ) )
    {
        WAIT_STATE( ch, PULSE_VIOLENCE );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
        act( "Przeciez $N jest twoim kumplem!", ch, NULL, victim, TO_CHAR );
        return;
    }

    /* wycinamy rasy bez nóg lub takie których nie da siê przewróciæ */
    if (
            !str_cmp( race_table[ GET_RACE(victim) ].name, "krokodyl" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "krokodyl olbrzymi" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "meduza" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "owad pe³zaj±cy" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "ro¶lina" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "rój" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "water flow" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "w±¿" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "¿yj±ca ¶ciana" )
       )
    {
        chance = 0;
        beznogie = TRUE;

    }
    else
    {
        beznogie = FALSE;
    }

    //jak spi
    if (  victim->position < POS_RESTING )
    {
        act( "Przelatujesz nad $V nie trafiaj±c.", ch, NULL, victim, TO_CHAR );
        damage( ch, victim, 0, gsn_bash, DAM_BASH, FALSE );
        WAIT_STATE( ch, PULSE_VIOLENCE );
        return;
    }

    //Bash wersja Raszera
    //bazowa szansa na trafienie w przeciwnika
    chance = (get_skill( ch, gsn_bash )*5)/6 + 2*(get_curr_stat_deprecated(ch,STAT_DEX)-get_curr_stat_deprecated(victim,STAT_DEX));
    chance = URANGE(10,chance,95);
    //print_char( ch, "Szansa bazowa trafienia: %d dex_twoj %d , dex przeciwnika %d \n\r", chance, get_curr_stat_deprecated(ch,STAT_DEX), get_curr_stat_deprecated(victim,STAT_DEX));
    //modyfikatory od tarczy
    if(shield != NULL)
    {
        switch( shield->value[0] )
        {
            case 0://puklerz
                chance-=5;
                break;
            case 1://lekka
                break;
            case 2://srednia
                chance+=7;
                break;
            case 3://ciezka
                break;
            case 4://pawez
                chance-=5;
                break;
        }
        chance += shield->value[4] * 2; //rellik: a co, niech to ma jakies znaczenie bo +- 3 to nie ma
        chance = URANGE(10,chance,95);
    }
    else
    {
        //moby bez tarczy maja inaczej wyliczane
        if ( IS_NPC( ch ) )
        {
            if ( IS_SET( race_table[ GET_RACE( ch ) ].type , ANIMAL ) ||
                  IS_SET( race_table[ GET_RACE( ch ) ].type , PLANT ) ||
                  IS_SET( ch->form, FORM_ANIMAL ) ||
                  IS_SET( ch->form, FORM_BIPED ) ||
                  IS_SET( ch->form, FORM_BIRD ) ||
                  IS_SET( ch->form, FORM_CENTAUR ) ||
                  IS_SET( ch->form, FORM_DRAGON ) ||
                  IS_SET( ch->form, FORM_MAMMAL ) ||
                  IS_SET( ch->form, FORM_REPTILE ) ||
                  IS_SET( ch->form, FORM_SNAKE ) ||
                  IS_SET( ch->form, FORM_WARM ) ||
                   (ch->size)-2>=(victim->size) )
            {
                //chance = chance * 3 ;
                //chance = chance / 4 ; //trzeba sprawdzic jakie szanse maj± boa od druida i ew. zbalansowaæ
            }else
            {
                chance = chance / 4;
            }
        }else
        {
            chance/=9;
        }
    }
    //print_char( ch, "Szansa trafienia po tarczy: %d \n\r", chance );

    //koniec modyfikator od tarczy dla trafienia

    //modyfikatory od wielkosci
    switch ( victim->size - ch->size )
    {

        case -2:
            chance -= 15;
            break;
        case -1:
            chance -= 10;
            break;
        case  1:
            chance += 10;
            break;
        case  2:
            chance += 15;
            break;
    }
    //skrajne roznice wielkosci
    if((ch->size)+3<=victim->size)
    {
        chance += 25;
    }

    if((ch->size)-3>=victim->size)
    {
        if(IS_NPC(ch))
        {
            chance-=10; //zeby smok mogl w miare bashnac
        }
        else
        {
            chance-=75; //zeby gracz nie mogl pajaka malego bashnac
        }
    }
    //print_char( ch, "Szansa trafienia po wielkosci: %d \n\r", chance );
    chance = URANGE(10,chance,95);
    //dodatkowe modyfikatory

    //haste
    if(IS_AFFECTED( ch, AFF_HASTE ))chance+=7;
    if(IS_AFFECTED( victim, AFF_HASTE ))chance-=7;

    //blur
    if( is_affected( victim, skill_lookup("blur") )) chance-=10;
    if( is_affected( ch, skill_lookup("blur") )) chance+=5;

    //web
    if ( IS_AFFECTED( victim, AFF_WEB ) )chance+=20;
    if ( IS_AFFECTED( ch, AFF_WEB ) )chance-=50;

    //blind
    if ( IS_AFFECTED( victim, AFF_BLIND ) )chance+=15;
    if ( IS_AFFECTED( ch, AFF_BLIND ) )chance-=15;

    //slow
    if ( IS_AFFECTED( victim, AFF_SLOW ) )chance+=7;
    if ( IS_AFFECTED( ch, AFF_SLOW ) )chance-=7;

    //wardance
    if ( is_affected( ch, gsn_wardance ) )chance+=7;
    if ( is_affected( victim, gsn_wardance ) )chance-=7;

    //shield

    if (!can_see( ch, victim )) chance/=2;
    //print_char( ch, "Szansa po modyfikatorach: %d \n \r", chance );
    if (!can_move(victim)) chance += 50;
    
    //slippery floor
    chance = URANGE(10,chance,95);
    //modyfikatory od pozycji
    if ( victim->position == POS_SITTING )
    {
        if(shield != NULL)
        {
            chance = chance * 25;
            chance = chance / 100;
        }else
        {
            chance=0;
        }
        //print_char( ch, "Szansa, przeciwnik siedzi: %d \n \r", chance );
    }

    if( victim->position == POS_RESTING)
    {
        if(shield != NULL)
        {
            chance/=2;
        }else
        {
            chance=0;
        };
        //print_char( ch, "Szansa, przeciwnik odpoczywa: %d \n \r", chance );
    }

    if( victim->position < POS_RESTING)
    {
        if(shield != NULL)
        {
            chance/=9;
        }else
        {
            chance=0;
        };
    }

    if(beznogie==FALSE)
    {

        if(number_percent()<=URANGE( 0, chance, 95))
        {
            trafil=TRUE;
        }
        else
        {
            trafil=FALSE;
        }

    }else
    {
        trafil=FALSE;
    }
    //koniwx liczenia trafienia

    //Raszer - czas na szanse na przewrócenie
    if(trafil==FALSE)
    {
        ogluszyl=FALSE;
        przewrocil=FALSE;
    }
    else
    {
        //liczenie przewrocenia
        //uzaleznienie szansy na przewrocenie od skill, str i conda ch oraz victima
        chance_p = 40 + (get_skill( ch, gsn_bash ) )/3+ (get_curr_stat_deprecated(ch,STAT_STR)-get_curr_stat_deprecated(victim,STAT_STR)+get_curr_stat_deprecated(ch,STAT_CON)-get_curr_stat_deprecated(victim,STAT_CON));
        //print_char( ch, "Bazowa szansa na przewrocenie %d\n\r", chance_p );

        //modyfikatory tarczy
        if(shield != NULL)
        {

            switch( shield->value[0] )
            {
                case 0://puklerz
                    chance_p-=5;
                    break;
                case 1://lekka
                    break;
                case 2://srednia
                    break;
                case 3://ciezka
                    chance_p+=5;
                    break;
                case 4://pawez
                    chance_p+=10;
                    break;
            }
            chance_p+=shield->value[5]*2;
        }
        else
        {
            if(IS_NPC(ch))
            {

            if ( IS_SET( race_table[ GET_RACE( ch ) ].type , ANIMAL ) ||
                  IS_SET( race_table[ GET_RACE( ch ) ].type , PLANT ) ||
                  IS_SET( ch->form, FORM_ANIMAL ) ||
                  IS_SET( ch->form, FORM_BIPED ) ||
                  IS_SET( ch->form, FORM_BIRD ) ||
                  IS_SET( ch->form, FORM_CENTAUR ) ||
                  IS_SET( ch->form, FORM_DRAGON ) ||
                  IS_SET( ch->form, FORM_MAMMAL ) ||
                  IS_SET( ch->form, FORM_REPTILE ) ||
                  IS_SET( ch->form, FORM_SNAKE ) ||
                  IS_SET( ch->form, FORM_WARM ) ||
                   (ch->size)-2>=(victim->size) )
                   {
                    //nic
                   }else
                   {
                   chance_p-=15;
                   }
            }else
            {
            chance_p-=15;
            }
        }
        //print_char( ch, "Przewrocenie po tarczy: %d\n\r", chance_p );
        //koniec modyfikatorow od tarczy na przewrocenie

        //modyfikatory od rozmiarow
        if((ch->size)+1==(victim->size)){chance_p-=15;};
        if((ch->size)+2==(victim->size)){chance_p-=20;};
        if((ch->size)-1==(victim->size)){chance_p+=15;};
        if((ch->size)-2==(victim->size)){chance_p+=20;};

        //modyfikator od formy przeciwnika
         if ( IS_SET( race_table[ GET_RACE( victim ) ].type , ANIMAL ) ||
                  IS_SET( race_table[ GET_RACE( victim ) ].type , PLANT ) ||
                  IS_SET( victim->form, FORM_ANIMAL ) ||
                  IS_SET( victim->form, FORM_BIPED ) ||
                  IS_SET( victim->form, FORM_BIRD ) ||
                  IS_SET( victim->form, FORM_CENTAUR ) ||
                  IS_SET( victim->form, FORM_DRAGON ) ||
                  IS_SET( victim->form, FORM_MAMMAL ) ||
                  IS_SET( victim->form, FORM_REPTILE ) ||
                  IS_SET( victim->form, FORM_SNAKE ) ||
                  IS_SET( victim->form, FORM_WARM ))
                   {
                   chance_p *= 3;
                   chance_p /= 4;
                   }

        //print_char( ch, "Przewrocenie po wielkosci: %d\n\r", chance_p );

        //modyfikatory do przewrocenia od affectow

        //berserk
        if( IS_AFFECTED( victim, AFF_BERSERK )) chance_p-=10;
        //przeciwnik ma tarcze
        //if(shield_v!=NULL && shield_v->value[0]>shield->value[0])chance_p-=10;


        //print_char( ch, "Przewrocenie po modyfikatorach: %d\n\r", chance_p );


        //liczenie szansy na przewalenie, jak nie przewalil to ogluszyl
        if(number_percent()<=URANGE( 5, chance_p, 95)){przewrocil=TRUE;ogluszyl=FALSE;}else{przewrocil=FALSE;ogluszyl=TRUE;};

        //skrajne roznice wielkosci
        if(trafil==TRUE)
        {
                   if((ch->size)+3<=victim->size){przewrocil=FALSE; ogluszyl=TRUE; ogluszenie_ch=number_range(1,2); ogluszenie_v=0;};
                   if((ch->size)-3>=victim->size){przewrocil=TRUE; ogluszyl=FALSE;};
        };
        //print_char( ch, "Ogluszenie, char: %d , wroga %d\n\r", ogluszenie_ch, ogluszenie_v );
        //liczenie dlugosci ogluszenia


        if(ogluszyl==TRUE)
        {
            if((ch->size)==(victim->size)){ogluszenie_ch=2;ogluszenie_v=2;};
            if((ch->size)+1==(victim->size)){ogluszenie_ch=2;ogluszenie_v=number_range(1,2);};
            if((ch->size)+2==(victim->size)){ogluszenie_ch=2;ogluszenie_v=1;};
            if((ch->size)-1==(victim->size)){ogluszenie_ch=2;ogluszenie_v=number_range(2,3);};
            if((ch->size)-2==(victim->size)){ogluszenie_ch=number_range(1,2);ogluszenie_v=number_range(2,3);};
            if((ch->size)+3<=victim->size){przewrocil=FALSE; ogluszyl=TRUE; ogluszenie_ch=number_range(1,2); ogluszenie_v=0;};
            if((ch->size)-3>=victim->size){przewrocil=TRUE; ogluszyl=FALSE; ogluszenie_v = 3;};

            //modyfikatory od czarow
            if ( IS_AFFECTED( victim, AFF_STONE_SKIN ) && ogluszenie_v>1) ogluszenie_v-=1;
            if (ogluszenie_v < 0) ogluszenie_v = 0;
        }
        //koniec liczenia szansy przewrucenia i dlugosci ogluszenia
    }

    //wykonanie
    if((!can_move(victim) && chance>0 && ogluszenie_v!=0)) przewrocil=TRUE;
    if(trafil==TRUE )
    {
        //trafil
        //sprawdzenie mirrora
        if ( check_mirror_image( victim, ch ) )
        {
            act( "{5Próbujesz powaliæ $C na ziemiê, ale $E znika a ty l±dujesz na ziemi.{x", ch, NULL, victim, TO_CHAR );
            act( "{5$n próbuje powaliæ na ziemiê jedno z twoich lustrzanych odbiæ i l±duje na ziemi.{x", ch, NULL, victim, TO_VICT );
            act( "{5$n próbuje powaliæ $C na ziemiê, ale trafia jedno z $S lustrzanych odbiæ i l±duje na ziemi.", ch, NULL, victim, TO_NOTVICT );
            damage( ch, victim, 0, gsn_bash, DAM_BASH, FALSE );
            check_improve( ch, victim, gsn_bash, FALSE, 60 );
            if ( ch->position > POS_SITTING )
            {
                ch->position = POS_SITTING;
            }
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            return;
        }
        //sprawdzenie blinka
        if ( check_blink( victim, ch, TRUE ) )
        {
            act( "{5Próbujesz powaliæ $C na ziemiê, ale $S sylwetka chwilowo znika a ty l±dujesz na ziemi.{x", ch, NULL, victim, TO_CHAR );
            act( "{5$n próbuje powaliæ ciê na ziemiê, ale ty znikasz na chwilê z tego planu egzystencji, a $E l±duje na ziemi.{x", ch, NULL, victim, TO_VICT );
            act( "{5$n próbuje powaliæ $C na ziemiê, ale $S sylwetka chwilowo znika, l±duje $e na ziemi.", ch, NULL, victim, TO_NOTVICT );
            damage( ch, victim, 0, gsn_bash, DAM_BASH, FALSE );
            check_improve( ch, victim, gsn_bash, FALSE, 60 );
            if ( ch->position > POS_SITTING )
                ch->position = POS_SITTING;
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            return;
        }
        //sprawdzenie stability
        if ( IS_AFFECTED(victim,AFF_STABILITY ) )
        {
            act( "{5$n uderza ciê napieraj±c ca³ym cia³em, jednak bez problemu utrzymujesz równowagê.{x", ch, NULL, victim, TO_VICT );
            act( "{5Uderzasz z rozpêdu w $C, jednak $E bez problemu utrzymuje równowagê.{x", ch, NULL, victim, TO_CHAR );
            act( "{5$n uderza $C napieraj±c ca³ym cia³em, jednak $E bez problemu utrzymuje równowagê.{x", ch, NULL, victim, TO_NOTVICT );
            damage( ch, victim, number_range( 2, 2 + 2 * ch->size + chance / 20 ), gsn_bash, DAM_BASH, FALSE );
            check_improve( ch, victim, gsn_bash, TRUE, 50 );
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            return;
        }
        //sprawdzenie mounta
        if ( !IS_NPC(victim) && victim -> mount )
        {
            mount = victim -> mount;
            if ( IS_AFFECTED(mount,AFF_STABILITY ) )
            {
                act( "{5$n uderza twojego wierzchowca napieraj±c ca³ym cia³em, jednak bez problemu utrzymujesz równowagê.{x", ch, NULL, victim, TO_VICT );
                act( "{5Uderzasz z rozpêdu w $C, jednak $E bez problemu utrzymuje równowagê.{x", ch, NULL, mount, TO_CHAR );
                act( "{5$n uderza $C napieraj±c ca³ym cia³em, jednak $E bez problemu utrzymuje równowagê.{x", ch, NULL, victim, TO_NOTVICT );
                check_improve( ch, victim, gsn_bash, TRUE, 80 );
                WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
                return;
            }
            //jezdziec nie ma stabilki
            if ( IS_AFFECTED( victim, AFF_FLYING ) || IS_AFFECTED( victim, AFF_FLOAT ))
            {
                if ( skill_table[ gsn_fly ].msg_off )
                {
                    send_to_char( skill_table[ gsn_fly ].msg_off, victim );
                    send_to_char( "\n\r", victim );
                }
                affect_strip( victim, gsn_fly );
                affect_strip( victim, gsn_float );
            }
            strip_invis( victim, TRUE, TRUE );
            if ( IS_AFFECTED( victim, AFF_HIDE ) )
            {
                affect_strip( victim, gsn_hide );
            }

            act( "{5$n powala twojego wierzchowca na ziemiê potê¿nym uderzeniem ca³ego cia³a! Spaaaadasz.{x", ch, NULL, victim, TO_VICT );
            act( "{5Uderzasz z rozpêdu w $C, posy³aj±c $M na ziemiê!{x", ch, NULL, mount, TO_CHAR );
            act( "{5$n powala wierzchowca $C na ziemiê uderzaj±c ca³ym cia³em.{x", ch, NULL, victim, TO_NOTVICT );
            check_improve( ch, victim, gsn_bash, TRUE, 60 );
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            WAIT_STATE( victim, 3 * PULSE_VIOLENCE );
            // zmianiamy pozycjê je¼d¼ca
            if ( victim->position >= POS_RESTING )
            {
                victim->position = POS_SITTING;
            }
            // zdejmujemy je¿d¼ca z wierzchowca, a tego ostatniego pozbawiamy ridera
            mount->mounting=NULL;
            victim->mount = NULL;
            // no i smaczek, zaatakowany wierzchowiec niech siê odgryza, a co!
            do_kill( mount, ch->name );
            return;


        }

        //trick
        if ( get_eq_char(ch, WEAR_SHIELD) && check_trick( ch, victim, SN_TRICK_FLABBERGAST ) )
        {
            return;
        }

        if(przewrocil==TRUE)
        {
            //no to przeciwnik gleba, skopiowane z starego basha
            if ( ( shield == NULL || number_range(1,3) == 1 || !can_move(victim) ) && victim->position > POS_SITTING )
            {
                act( "{5$n powala ciê na ziemiê potê¿nym uderzeniem ca³ego cia³a!{x", ch, NULL, victim, TO_VICT );
                act( "{5Uderzasz z rozpêdu w $C, posy³aj±c $M w powietrze!{x", ch, NULL, victim, TO_CHAR );
                act( "{5$n powala $C na ziemiê uderzaj±c ca³ym cia³em.{x", ch, NULL, victim, TO_NOTVICT );
            }
            else if ( victim->position > POS_SITTING &&	number_range(1,2) == 1 && ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) ))
            {
                act( "{5Wyczekujesz na odpowiedni moment, po czym rzucasz siê z $j w rêku i g³o¶nym okrzykiem na ustach, na $C, przenosz±c na $S cia³o ca³y swój ciê¿ar. Uderzenie to by³o na tyle silne, ¿e twój wróg leci troche do ty³u po czym z g³uchym jêkiem przewraca siê na ziemie i spluwa krwi±.{x", ch, shield, victim, TO_CHAR );
                act( "{5$n mierzy ciê bacznym spojrzeniem, po czym w chwili gdy siê tego najmniej spodziewasz rzuca siê na ciebie z $j w rêku i g³o¶nym okrzykiem na ustach. Sprawnie przenosi ca³y ciê¿ar cia³a na to jedno uderzenie powoduj±c, ¿e z g³uchym odg³osem uderzenia odlatujesz do ty³u wal±c siê na ziemiê i spluwasz siarczy¶cie krwi±.{x", ch, shield, victim, TO_VICT );
                act( "{5$n mierzy przez chwilê $C bacznym spojrzeniem, po czym sprawnie rzuca siê do przodu z $j w rêku i g³o¶nym okrzykiem na ustach. Ca³kowicie nieprzygotowana na to ofiara, ze zdziwiniem w oczach, obserwuje jak odbija siê od ziemi dziêki sile uderzenia, leci trochê do ty³u, po czym upada z g³o¶nym ³oskotem na ziemiê.{x", ch, shield, victim, TO_NOTVICT );
            }
            else
            {
                act( "{5W b³yskawicznym zrywie doskakujesz do $Z, napinasz wszystkie miê¶nie do granic mo¿liwo¶ci i t³uczesz sw± ofiarê $j. Trac±c równowagê $N chwieje siê chwilê w otêpieniu po czym wali na ziemiê z g³uchym jêkiem i ³oskotem.{x", ch, shield, victim, TO_CHAR );
                act( "{5$n w b³yskawicznym doskoku zbli¿a siê do ciebie, wyrzuca ramiê i t³ucze ciê z ca³ych si³ $j. Potrz±sasz g³ow± próbuj±c odzyskaæ równowagê, ale nie udaje ci siê, walisz bezw³adnie ca³ym cia³em o ziemiê.{x", ch, shield, victim, TO_VICT );
                act( "{5$n w nag³ym doskoku zbli¿a siê do $C. Napinaj±c miê¶nie do granic mo¿liwo¶ci t³ucze $e z ca³ych si³ $j $C. Zamroczony przeciwnik chwieje siê chwilê na szeroko rozstawionych nogach, po czym wali z ³oskotem na ziemiê.{x", ch, shield, victim, TO_NOTVICT );
            }
            if ( IS_AFFECTED( victim, AFF_FLYING ) || IS_AFFECTED( victim, AFF_FLOAT ))
            {
                if ( skill_table[ gsn_fly ].msg_off )
                {
                    send_to_char( skill_table[ gsn_fly ].msg_off, victim );
                    send_to_char( "\n\r", victim );
                }
                affect_strip( victim, gsn_fly );
                affect_strip( victim, gsn_float );
            }
            strip_invis( victim, TRUE, TRUE );
            if ( IS_AFFECTED( victim, AFF_HIDE ) )
            {
                affect_strip( victim, gsn_hide );
            }

            check_improve( ch, victim, gsn_bash, TRUE, 60 );
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            if ( victim->position != POS_STANDING )
            {
                victim->position = POS_STANDING; //zeby nie bylo -opis o powaleniu i wstanie z resta- Raszer
            }

            WAIT_STATE( victim, 3 * PULSE_VIOLENCE );//wait przed damage, zeby zapobiec ratowaniu przed wimpy na 100%
            damage( ch, victim, number_range( 2, 2 + 2 * ch->size + chance / 7 ), gsn_bash, DAM_BASH, FALSE );

            if ( victim->position >= POS_RESTING )
            {
                victim->position = POS_SITTING;
            }
            return;
        }

        if(ogluszyl==TRUE)
        {
            if(ogluszenie_v>0)
            {
                if ( ( shield == NULL || number_range(1,3) == 1 ) && victim->position > POS_SITTING )
                {
                    act( "{5$n uderza ciê z rozpêdu ca³ym swoim cia³em, z trudem utrzymujesz równowagê!{x", ch, NULL, victim, TO_VICT );
                    act( "{5Uderzasz z rozpêdu w $C, nie udaje ci siê jednak wybiæ $M z równowagi!{x", ch, NULL, victim, TO_CHAR );
                    act( "{5$n uderza $C ca³± mas± swojego cia³a, jednak $E utrzymuje równowagê.{x", ch, NULL, victim, TO_NOTVICT );
                }
                else
                {
                    if ( victim->position > POS_SITTING &&	number_range(1,2) == 1 && ( IS_SET( victim->form, FORM_WARM ) || IS_SET( victim->form, FORM_COLD_BLOOD ) ))
                    {
                        act( "{5Bacznie mierzysz $C swoim spojrzeniem, poprawiaj±c uchwyt na $j. Bierzesz ostatni wdech i rzucasz siê wroga, uderzeniem posy³aj±c go ty³u. Przeciwnik robi pare chwiejnych kroków jednak udaje mu siê utrzymaæ równowagê.{x", ch, shield, victim, TO_CHAR );
                        act( "{5$n mierzy ciê bacznym spojrzeniem, po czym b³yskawicznie rzuca siê na ciebie z $j w rêku i g³o¶nym okrzykiem na ustach. Uderzenie by³o naprawdê silne, og³uszony robisz pare chwiejnych kroków do ty³u, jednak utrzymujesz równowagê.{x", ch, shield, victim, TO_VICT );
                        act( "{5$n mierzy przez chwilê $C bacznym spojrzeniem, po czym sprawnie rzuca siê do przodu z $j w rêku i g³o¶nym okrzykiem na ustach. Wydawa³oby siê nieprzygotowana ofiara pod wp³ywem uderzenia robi parê chwiejnych kroków w ty³, jednak nie przewraca siê.{x", ch, shield, victim, TO_NOTVICT );
                    }
                    else
                    {
                        act( "{5W b³yskawicznym zrywie doskakujesz do $Z, napinasz wszystkie miê¶nie do granic mo¿liwo¶ci i t³uczesz sw± ofiarê $j. Zamroczona ofiara cofa siê, jednak zachowuje równowagê.{x", ch, shield, victim, TO_CHAR );
                        act( "{5$n w b³yskawicznym doskoku zbli¿a siê do ciebie, wyrzuca ramiê i t³ucze ciê z ca³ych si³ $j. Przez chwilê widzisz tylko czerñ i gwiazdki, jednak o dziwo udaje ci siê ustaæ.{x", ch, shield, victim, TO_VICT );
                        act( "{5$n w nag³ym doskoku zbli¿a siê do $C. Napinaj±c miê¶nie do granic mo¿liwo¶ci t³ucze $e z ca³ych si³ $j $C. Efekt nie by³ tym razem jednak najlepszy, nie uda³u mu siê przewróciæ $C{x", ch, shield, victim, TO_NOTVICT );
                    }
                }

                if ( IS_AFFECTED( victim, AFF_FLYING ) || IS_AFFECTED( victim, AFF_FLOAT ))
                {
                    if ( skill_table[ gsn_fly ].msg_off )
                    {
                        send_to_char( skill_table[ gsn_fly ].msg_off, victim );
                        send_to_char( "\n\r", victim );
                    }
                    affect_strip( victim, gsn_fly );
                    affect_strip( victim, gsn_float );
                }
                strip_invis( victim, TRUE, TRUE );
                if ( IS_AFFECTED( victim, AFF_HIDE ) )
                {
                    affect_strip( victim, gsn_hide );
                }

                check_improve( ch, victim, gsn_bash, TRUE, 60 );
                if ( ogluszenie_ch > 0 )
                {
                    WAIT_STATE( ch, ogluszenie_ch * PULSE_VIOLENCE );
                }
                if ( ogluszenie_v > 0 )
                {
                    WAIT_STATE( victim, ogluszenie_v * PULSE_VIOLENCE );
                }
                damage( ch, victim, number_range( 2, 2 + 2 * ch->size + chance / 7 ), gsn_bash, DAM_BASH, FALSE );

                return;

                //oguszenie bylo wieksze od zero
            }
            else
            {
                //bylo mniejsze
                strip_invis( victim, TRUE, TRUE );
                if ( IS_AFFECTED( victim, AFF_HIDE ) )
                {
                    affect_strip( victim, gsn_hide );
                }

                act( "{5Napinasz miê¶nie do granic mo¿liwo¶ci i rzucasz siê na $C. Uderzenie okazuje siê jednak zbyt s³abe jak na tak du¿± istotê, odbijasz siê od niej.{x", ch, shield, victim, TO_CHAR );
                act( "{5$n napina miê¶nie, po czym rzuca siê na ciebie z okrzykiem. Jest jednak zbyt ma³y by móc co¶ wskóraæ.{x", ch, shield, victim, TO_VICT );
                act( "{5$n napina miê¶nie do granic mo¿liwo¶ci i rzuca siê na $C uderzaj±c ca³ym cia³em, jednak z tak du¿ym przeciwnikiem nie udaje mu siê nic wskóraæ.{x", ch, shield, victim, TO_NOTVICT );
                if ( ogluszenie_ch > 0 )
                {
                    WAIT_STATE( ch, ogluszenie_ch * PULSE_VIOLENCE );
                }
                return;
            }//rozdzielenie na ogluszenie wieksze od 0 i nie
        }//koniec ogluszania
    }
    else
    {
        //nie trafil
        damage( ch, victim, 0, gsn_bash, DAM_BASH, FALSE );
        if ( shield == NULL || number_range(1,3) == 1 )
        {
            act( "{5Padasz bezw³adnie uderzaj±c twarz± o ziemiê!{x", ch, NULL, victim, TO_CHAR );
            if (IS_SET( race_table[ GET_RACE( ch ) ].type, PERSON ))
            {
                act( "{5$n pada bezw³adnie prosto na twarz.{x", ch, NULL, victim, TO_NOTVICT );
            }
            else
            {
                act( "{5$n pada bezw³adnie prosto na pysk.{x", ch, NULL, victim, TO_NOTVICT );
            }
            act( "{5Unikasz uderzenia $z, wytr±caj±c $m z równowagi i powalaj±c na ziemiê.{x", ch, NULL, victim, TO_VICT );
        }
        else if ( number_range(1,2) == 1 )
        {
            act( "{5Patrzysz w¶ciekle spod przymkniêtych powiek na przeciwnika trzymaj±c kurczowo $h, nacierasz na niego w furii ¶ciskaj±c mocno $h. Bierzesz potê¿ny rozpêd i z ca³ym impetem rzucasz siê w stronê $Z, $E jednak przewiduje twój zamiar i uchodzi z linii ataku. Zatrzymujesz swój szaleñczy bieg parê metrów za przeciwnikiem i tracisz równowagê. {/Walisz siê ca³ym ciê¿arem na ziemiê z bolesnym jêkiem.{x", ch, shield, victim, TO_CHAR );
            act( "{5$n mierzy ciê z nienawi¶ci± spod przymkniêtych powiek i kurczowo ³apie $h w rêkê. Teraz ju¿ wiesz co za chwilê siê stanie. Zgodnie z przewidywaniami naciera $e na ciebie w¶ciekle ca³ym ciê¿arem swojego cia³a z wyci±gaj±c przed siebie $h. Schodzisz w ostatniej chwili z toru ciosu i obracasz siê b³yskawicznie. {/Przeciwnik mija ciê ci±gniêty impetem ataku, traci równowagê i wali siê na ziemiê z bolesnym jêkiem.{x", ch, shield, victim, TO_VICT );
            act( "{5$n obserwuje spod przymkniêtych powiek z nienawi¶ci± sylwetkê $Z kurczowo trzymaj±c $h. W jednej chwili rzuca siê $e na przeciwnika w furii nacieraj±c na niego ca³ym ciê¿arem cia³a. $N orientuje siê w porê, do czego to wszystko zmierza i z gracj± uchyla siê w bok schodz±c z toru w¶ciek³ego ataku. $n mija cel i trac±c równowagê wali siê na ziemiê z bolesnym jêkiem.{x", ch, shield, victim, TO_NOTVICT );
        }
        else
        {
            act( "{5Mierzysz $C bacznym spojrzeniem i po chwili widzisz doskonale nadarzaj±c± siê okazjê. Rzucasz siê do przodu z g³o¶nym okrzykiem, na niczego nie spodziewaj±cego siê przeciwnika. Ten jednak przewidzia³ twój ruch i gdy masz ju¿ uderzyc ca³ym ciê¿arem cia³a na niego, odsuwa siê on zwinnie poza zasiêg ciosu. {/Nie mog±c z³apaæ równowagi, machaj±c rêkami jak idiot<&a/ka/a> walisz siê na ziemiê!{x", ch, shield, victim, TO_CHAR );
            act( "{5$n mierzy ciê bacznym spojrzeniem i po chwili usmiech pojawia siê na $s twarzy. Rzuca siê nagle do przodu z g³o¶nym okrzykiem wywo³uj±c twoje niema³e zdziwinie. Jednak jeste¶ na to przygotowan<&y/a/e> i sprawnie odsuwasz siê poza zasiêg ciosu.{x", ch, shield, victim, TO_VICT );
            act( "{5$n nie mog±c z³apaæ równowagi macha rêkami jak idiota i wali siê na ziemiê.{x", ch, shield, victim, TO_VICT );
            act( "{5$n spogl±da pobierznie na $C i rzuca siê do przodu z g³o¶nym okrzykiem, jednak cel, wydawa³o by siê nieprzygotowany, sprawnie odsuwa siê poza zasiêg ciosu, a $n, machaj±c g³upio rêkoma próbuj±c z³apaæ równowage, wali siê jak k³oda na ziemiê.{x", ch, shield, victim, TO_NOTVICT );
        }
        check_improve( ch, victim, gsn_bash, FALSE, 60 );
        if ( ch->position > POS_SITTING )
        {
            ch->position = POS_SITTING;
        }
        WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
        return;

    }//koniec nie trafiania
    check_killer( ch, victim );
    return;
}


void do_trip( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    int chance;

    one_argument( argument, arg );

    if ( ( chance = get_skill( ch, gsn_trip ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->mount )
    {
        send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
        return;
    }

    if ( arg[ 0 ] == '\0' )
    {
        victim = ch->fighting;

        if ( victim == NULL )
        {
            send_to_char( "Przecie¿ z nikim nie walczysz!\n\r", ch );
            return;
        }
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        add_wait_after_miss(ch, 1);
        return;
    }

    if ( is_safe( ch, victim ) )
        return;

    if ( IS_AFFECTED( victim, AFF_FLYING ) )
    {
        act( "Stopy $Z nie dotykaj± ziemi.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( !form_check( ch, victim, NULL, gsn_trip ) )
    {
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "{5Przewracasz siê wal±c g³ow± w ziemiê!{x\n\r", ch );
        WAIT_STATE( ch, PULSE_VIOLENCE );
        act( "{5$n przewraca siê o w³asne nogi!{x", ch, NULL, NULL, TO_ROOM );
        if ( victim->position > POS_SITTING )
        {
            victim->position = POS_SITTING;
        }
        return;
    }

    if ( victim->position < POS_FIGHTING )
    {
        act( "$N juz le¿y.", ch, NULL, victim, TO_CHAR );
        WAIT_STATE( ch, PULSE_VIOLENCE );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
        act( "$N jest twoim ukochanym mistrzem.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( check_mirror_image( victim, ch ) )
    {
        if ( !stat_throw( ch, STAT_DEX ) )
        {
            act( "{5Próbujesz wywróciæ $C na ziemiê, ale $E znika a ty nie napotykaj±c oporu tracisz równowagê.{x", ch, NULL, victim, TO_CHAR );
            act( "{5$n próbuje wywróciæ jedno z twoich lustrzanych odbiæ i traci równowagê.{x", ch, NULL, victim, TO_VICT );
            act( "{5$n próbuje wywróciæ $C na ziemiê, ale trafia jedno z $S lustrzanych odbiæ i l±duje na ziemi.", ch, NULL, victim, TO_NOTVICT );
            if ( ch->position > POS_SITTING )
            {
                ch->position = POS_SITTING;
            }
        }
        else
        {
            act( "{5Próbujesz wywróciæ $C na ziemiê, ale $E znika.{x", ch, NULL, victim, TO_CHAR );
            act( "{5$n próbuje wywróciæ jedno z twoich lustrzanych odbiæ.{x", ch, NULL, victim, TO_VICT );
            act( "{5$n próbuje wywróciæ $C na ziemiê, ale trafia jedno z $S lustrzanych odbiæ.", ch, NULL, victim, TO_NOTVICT );
        }
        check_improve( ch, victim, gsn_trip, FALSE, 60 );
        damage( ch, victim, 0, gsn_trip, DAM_BASH, FALSE );
        WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
        return;
    }

    if ( check_blink( victim, ch, TRUE ) )
    {
        if ( !stat_throw( ch, STAT_DEX ) )
        {
            act( "{5Próbujesz wywróciæ $C na ziemiê, ale $S sylwetka chwilowo znika a ty nie napotykaj±c oporu tracisz równowagê.{x", ch, NULL, victim, TO_CHAR );
            act( "{5$n próbuje wywróciæ ciê na ziemiê, ale ty znikasz na chwilê z tego planu egzystencji, a $E l±duje na ziemi.{x", ch, NULL, victim, TO_VICT );
            act( "{5$n próbuje wywróciæ $C na ziemiê, ale $S sylwetka chwilowo znika, l±duje $e na ziemi.", ch, NULL, victim, TO_NOTVICT );
            if ( ch->position > POS_SITTING )
            {
                ch->position = POS_SITTING;
            }
        }
        else
        {
            act( "{5Próbujesz wywróciæ $C na ziemiê, ale $S sylwetka chwilowo znika.{x", ch, NULL, victim, TO_CHAR );
            act( "{5$n próbuje wywróciæ ciê na ziemiê, ale ty znikasz na chwilê z tego planu egzystencji.{x", ch, NULL, victim, TO_VICT );
            act( "{5$n próbuje wywróciæ $C na ziemiê, ale $S sylwetka chwilowo znika.", ch, NULL, victim, TO_NOTVICT );
        }
        damage( ch, victim, 0, gsn_trip, DAM_BASH, FALSE );
        check_improve( ch, victim, gsn_trip, FALSE, 60 );
        WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
        return;
    }

    if ( is_affected( victim, gsn_float ) )
    {
        if( ch->size >= victim->size && ch->level + get_curr_stat_deprecated(ch,STAT_DEX) > victim->level + get_curr_stat_deprecated(victim,STAT_DEX ) )
        {
            act( "Szybkim ruchem ³apiesz $C za nogi i ¶ci±gasz na ziemiê.", ch, NULL, victim, TO_CHAR );
            act( "$n szybkim ruchem ³apie ciê za nogi i ¶ci±ga na ziemiê!", ch, NULL, victim, TO_VICT );
            act( "$n szybkim ruchem ³apie $C za nogi i ¶ci±ga na ziemiê.", ch, NULL, victim, TO_NOTVICT );
            affect_strip( victim, gsn_float );
        }
        else
        {
            act( "Stopy $Z nie dotykaj± ziemi.", ch, NULL, victim, TO_CHAR );
            return;
        }
    }

    switch ( ch->size - victim->size )
    {
        case  1:
        case -1:
            chance -= 5;
            break;
        case  2:
        case -2:
            chance -= 10;
            break;
        case  3:
        case -3:
            chance -= 40;
            break;
        case  5:
        case -5:
            chance -= 80;
            break;
    }

    /* dex + polowa do szansy */
    chance += (get_curr_stat_deprecated( ch, STAT_DEX ) - 15)*3 ;
    chance -= get_curr_stat_deprecated( victim, STAT_DEX ) * 3 / 2;

    /* speed */
    if ( EXT_IS_SET( ch->off_flags, OFF_FAST ) || IS_AFFECTED( ch, AFF_HASTE ) )
    {
        chance += 10;
    }

    if ( EXT_IS_SET( victim->off_flags, OFF_FAST ) || IS_AFFECTED( victim, AFF_HASTE ) )
    {
        chance -= 20;
    }

    /* level */
    chance += URANGE( -10, ch->level - victim->level, 10 );

    /* hold */

    if ( !can_move( victim ) )
    {
       chance = 100;
    }

    /* tylko na persony i animale */
    if ( !IS_SET( race_table[ GET_RACE( victim ) ].type, PERSON ) &&
            !IS_SET( race_table[ GET_RACE( victim ) ].type, ANIMAL ) &&
            str_cmp( race_table[ GET_RACE(victim) ].name, "ogr" ) &&
            str_cmp( race_table[ GET_RACE(victim) ].name, "szkolny potworek" ) &&
            str_cmp( race_table[ GET_RACE(victim) ].name, "troll" ) &&
            str_cmp( race_table[ GET_RACE(victim) ].name, "sobowtór" ) &&
            str_cmp( race_table[ GET_RACE(victim) ].name, "likantrop" ) &&
            str_cmp( race_table[ GET_RACE(victim) ].name, "syrena" ) &&
            str_cmp( race_table[ GET_RACE(victim) ].name, "tasloi" ) &&
            str_cmp( race_table[ GET_RACE(victim) ].name, "troglodyta" ) &&
            str_cmp( race_table[ GET_RACE(victim) ].name, "umberkolos" ) &&
            str_cmp( race_table[ GET_RACE(victim) ].name, "³akowilk" ) &&
            str_cmp( race_table[ GET_RACE(victim) ].name, "yuan-ti" ) &&
            str_cmp( race_table[ GET_RACE(victim) ].name, "kaszcz" ) &&
            str_cmp( race_table[ GET_RACE(victim) ].name, "yeti" ))
        chance = 0;

    /* now the attack */
    if ( number_percent() < chance )
    {
        check_improve( ch, victim, gsn_trip, TRUE, 60 );
        if ( IS_AFFECTED( victim, AFF_STABILITY ) )
        {
            act( "{5$n podstawia ci nogê, jednak bez problemu utrzymujesz równowagê.{x", ch, NULL, victim, TO_VICT );
            act( "{5Podstawiasz nogê $X, jednak $E bez problemu utrzymuje równowagê.{x", ch, NULL, victim, TO_CHAR );
            act( "{5$n podstawia $X nogê, jednak $E bez problemu utrzymuje równowagê.{x", ch, NULL, victim, TO_NOTVICT );
            damage( ch, victim, 0, 0, DAM_BASH, FALSE );
            return;
        }
        WAIT_STATE( victim, 2 * PULSE_VIOLENCE );
        damage( ch, victim, number_range( 2, 2 + 2 * victim->size ), gsn_trip, DAM_BASH, TRUE );
    }
    else
    {
        damage( ch, victim, 0, gsn_trip, DAM_BASH, TRUE );
        check_improve( ch, victim, gsn_trip, FALSE, number_range( 50, 60) );
    }
    WAIT_STATE( ch, skill_table[ gsn_trip ].beats );
    return;
}

/**
 * Nazwa: sap
 * Typ: umiejêtno¶æ ofensywna
 * Sk³adania: sap <ofiara>
 * Opis: Dobrze ukryty ³otrzyk potrafi niezauwa¿enie zakra¶æ siê za plecy
 * przeciwnika i za pomoc± szybkiego uderzenia pa³k± pozbawiæ go przytomno¶ci.
 *
 * Dzia³anie: Z³odziej zakrada siê za plecy ofiary i uderza pa³k± w ³eb.
 * Przeciwnik dostaje flagê paralyze na krótki czas. Z³odziej nie rozpoczyna
 * tym walki, wykonanie trwa 3 delaye po wykonaniu skilla ³otrzyk dostaje
 * d³ugiego waita. Umiejêtno¶æ ta wymaga affectu sneak i hide oraz
 * wieldniêtej broni typu mace
 *
 */
void do_sap( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	one_argument( argument, arg );

    int skill = get_skill( ch, gsn_sap );
    if ( skill <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->fighting != NULL )
    {
        send_to_char( "Nie teraz, walcz.\n\r", ch );
        add_wait_after_miss(ch, 2);
        return;
    }

    bool hidden = IS_AFFECTED( ch, AFF_HIDE );
	bool sneak = IS_AFFECTED( ch, AFF_SNEAK );


    CHAR_DATA *victim;
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
    	if ( hidden && number_percent() > 30 )
    	{
    		send_to_char( "Wyprowadzasz cios, ale orientujesz siê, ¿e przeciwnika nie ma.\n\r", ch );
    		affect_strip( ch, gsn_hide );
    		EXT_REMOVE_BIT( ch->affected_by, AFF_HIDE );
    	}
    	else if ( sneak && number_percent() > 30  )
    	{
    		send_to_char( "Twój zamach powoduje, ¿e zaczynasz ha³asowaæ.\n\r", ch );
    		affect_strip( ch, gsn_sneak );
    	}
    	else
    	{
    		send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
    	}
    	strip_invis( ch, TRUE, TRUE );
    	add_wait_after_miss(ch, 2);
    	return;
    }

    if(!hidden || !sneak)
    {
    	switch(number_range(0, 2))
    	{
    	case 0:
    		act( "{5Krêcisz siê i wymachujesz broni± tak, ¿e tracisz element zaskoczenia.{x", ch, NULL, victim, TO_CHAR );
    		act( "{5$n próbuje ciê og³uszyæ, jednak robi to tak nieudolnie, ¿e traci element zaskoczenia.{x", ch, NULL, victim, TO_VICT );
    		act( "{5$n próbuje og³uszyæ $E, jednak $n robi to tak nieudolnie, ¿e traci element zaskoczenia.{x", ch, NULL, victim, TO_NOTVICT );
    		break;
    	case 1:
    		act( "{5Twoje niezbyt skoordynowane ruchy ostrzegaj± przeciwnika.{x", ch, NULL, victim, TO_CHAR );
    		act( "{5$n próbuje ciê og³uszyæ, jednak za wcze¶nie wychodzi z ukrycia.{x", ch, NULL, victim, TO_VICT );
    		act( "{5$n próbuje og³uszyæ $E, jednak $n za wcze¶nie wychodzi z ukrycia.{x", ch, NULL, victim, TO_NOTVICT );
    		break;
    	case 2:
    		act( "{5Potykasz siê i ca³y impet uderzenia rozprasza siê.{x", ch, NULL, victim, TO_CHAR );
    		act( "{5$n próbuje ciê og³uszyæ, jednak potyka siê i ca³y impet uderzenia rozprasza siê.{x", ch, NULL, victim, TO_VICT );
    		act( "{5$n próbuje og³uszyæ $E, jednak $n potyka siê i ca³y impet uderzenia rozprasza siê.{x", ch, NULL, victim, TO_NOTVICT );
    		break;
    	}
    	damage( ch, victim, 0, gsn_sap, DAM_NONE, TRUE );
    	return;
    }

    if ( is_safe( ch, victim ) )
    {
    	send_to_char( "Machasz rêkoma, ale co¶ ci przeszkadza.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Machasz broni±, ale nic z tego.\n\r", ch );
        return;
    }

    OBJ_DATA *weapon = get_eq_char( ch, WEAR_WIELD );
    if ( weapon == NULL )
    {
        send_to_char( "Musisz mieæ jak±¶ broñ.\n\r", ch );
        return;
    }

    if ( weapon->value[ 0 ] != WEAPON_MACE )
    {
        send_to_char( "Przyda³aby siê jaka¶ broñ obuchowa.\n\r", ch );
        return;
    }

    if ( ch->mount )
    {
        send_to_char( "Mo¿e najpierw zsi±dziesz z wierzchowca?\n\r", ch );
        return;
    }

    if ( !(
    		str_cmp( race_table[ GET_RACE(victim) ].name, "stonoga" )
    		&& str_cmp( race_table[ GET_RACE(victim) ].name, "w±¿" )
    		&& str_cmp( race_table[ GET_RACE(victim) ].name, "water flow" )
    		&& str_cmp( race_table[ GET_RACE(victim) ].name, "delfin" )
    		&& str_cmp( race_table[ GET_RACE(victim) ].name, "¿ywio³" )
    		&& str_cmp( race_table[ GET_RACE(victim) ].name, "ryba" )
    		&& str_cmp( race_table[ GET_RACE(victim) ].name, "grzyb" )
    		&& str_cmp( race_table[ GET_RACE(victim) ].name, "ro¶lina" )
    		&& str_cmp( race_table[ GET_RACE(victim) ].name, "¿yj±ca ¶ciana" )
    		&& str_cmp( race_table[ GET_RACE(victim) ].name, "meduza" )
    		&& str_cmp( race_table[ GET_RACE(victim) ].name, "b³êdny ognik" )
    		&& str_cmp( race_table[ GET_RACE(victim) ].name, "rój owadów" )
    		&& str_cmp( race_table[ GET_RACE(victim) ].name, "owad pe³zaj±cy" )
    		&& str_cmp( race_table[ GET_RACE(victim) ].name, "owad lataj±cy" )
    ) )
    {
    	send_to_char( "Hmm, nie za bardzo wiesz jak siê do tego zabraæ.\n\r", ch );
    	return;
    }

    if ( victim->position <= POS_SLEEPING )
    {
        skill += 20;
    }
    else if ( victim->position == POS_RESTING || victim->position == POS_SITTING )
    {
        skill += 10;
    }

    switch ( 5 + ch->size - victim->size )
    {
        // ch->size - victim->size == 1 ||  ch->size - victim->size == -1
        case 4:
        case 6:
        skill -= 5;
            break;
        // ch->size - victim->size == 2 ||  ch->size - victim->size == -2
        case 3:
        case 7:
        skill -= 10;
            break;
        // ch->size - victim->size == 3 ||  ch->size - victim->size == -3
        case 2:
        case 8:
        skill -= 20;
            break;
        // ch->size - victim->size == 4 ||  ch->size - victim->size == -4
        case 1:
        case 9:
        skill -= 40;
            break;
        // ch->size - victim->size == 5 ||  ch->size - victim->size == -5
        case 0:
        case 10:
        skill -= 80;
            break;
    }

    OBJ_DATA *helmet = get_eq_char( ch, WEAR_HEAD );

    //im cel ma lepszy helm tym trudniej stunnac
    if ( helmet && helmet->item_type == ITEM_ARMOR )
    {
        skill -= 10;
        skill -= helmet->value[1]*2; //ac na obrazenia obuchowe, od 0 do 10
    }

    skill = URANGE( 3, skill, 100 );

	affect_strip( ch, gsn_hide );

    if( EXT_IS_SET( ch->in_room->room_flags, ROOM_SLIPPERY_FLOOR ) && number_percent() > skill/2 && !IS_AFFECTED( ch, AFF_STABILITY ) )
    {
        if( ch->sex == 2 )
        {
            act( "{5$n stara³a siê podkra¶æ za plecy $Z, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgnê³a siê i pada z jêkiem na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
            act( "{5$n stara³a siê podkra¶æ za twe plecy, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgnê³a siê i pada z jêkiem na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
        }
        else
        {
            act( "{5$n stara³ siê podkra¶æ za plecy $Z, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgn±³ siê i pada z jêkiem na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
            act( "{5$n stara³ siê podkra¶æ za twe plecy, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgn±³ siê i pada z jêkiem na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
        }
        act( "{5Starasz siê podkra¶æ za plecy $Z, jednak przez rozlan± na pod³o¿u t³ust± i ¶lisk± substancjê twoja noga po¶lizgnê³a siê i padasz na ziemiê z jêkiem.{x", ch, NULL, victim, TO_CHAR );
        damage( ch, victim, 0, gsn_sap, DAM_NONE, FALSE );
        ch->position = POS_SITTING;
        WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
        check_improve( ch, victim, gsn_sap, FALSE, 65 );
        return;
    }

    if(skill > number_range(0, 100))
    {
    	act( "{5Walisz z ca³ej si³y w g³owê $X.{x", ch, NULL, victim, TO_CHAR );
    	act( "{5$n wali ciê z ca³ej si³y w g³owê i og³usza.{x", ch, NULL, victim, TO_VICT );
		act( "{5$n wali z ca³ej si³y w g³owê $X.{x", ch, NULL, victim, TO_NOTVICT );
		check_improve( ch, victim, gsn_sap, TRUE, 50 );

		//udany bez damage
        //damage( ch, victim, number_range(2, 7), gsn_sap, DAM_BASH, TRUE );
		WAIT_STATE( ch,  number_range(1, 2) * PULSE_VIOLENCE); //grupa mo¿e pomóc ofierze
		WAIT_STATE( victim, number_range(2, 4) * PULSE_VIOLENCE ); //symulacja og³uszania z ró¿n± si³±
    }
    else
    {
		act( "{5Próbujesz og³uszyæ $X, ale nie udaje ci siê.{x", ch, NULL, victim, TO_CHAR );
		act( "{5$n próbuje ciê og³uszyæ, ale nie trafia.{x", ch, NULL, victim, TO_VICT );
		act( "{5$n próbuje og³uszyæ $E, jednak nie trafia.{x", ch, NULL, victim, TO_NOTVICT );
		check_improve( ch, victim, gsn_sap, FALSE, 55 );
		one_hit( ch, victim, gsn_sap, FALSE );
		WAIT_STATE( ch, skill_table[ gsn_sap ].beats );
    }

}
void do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int skill_backstab = get_skill( ch, gsn_backstab );
#ifdef ENHANCED_BACKSTAB
    int skill_dagger_mastery = 0, skill_dagger;
#endif
    bool check = FALSE, hidden = IS_AFFECTED( ch, AFF_HIDE );

    one_argument( argument, arg );

    if ( skill_backstab < 1 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Komu chcesz zadaæ cios w plecy?\n\r", ch );
        return;
    }

    if ( ch->fighting != NULL )
    {
        send_to_char( "Nie teraz, walcz.\n\r", ch );
        WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
        return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        if ( hidden && number_percent() > 50 )
        {
            send_to_char( "Twoje cia³o wyprowadza cios w nieistniej±cego przeciwnika.\n\r", ch );
            affect_strip( ch, gsn_hide );
            EXT_REMOVE_BIT( ch->affected_by, AFF_HIDE );
        }
        else if ( IS_AFFECTED( ch, AFF_SNEAK ) && number_percent() > 50  )
        {
            send_to_char( "Twój zamach powoduje, ¿e zaczynasz ha³asowaæ.\n\r", ch );
            affect_strip( ch, gsn_sneak );
        }
        else
        {
            send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        }
        // zdejmujemy invisa, z komunikatami
        strip_invis( ch, TRUE, TRUE );
        // dodajemy waita
        add_wait_after_miss(ch, 1);
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Masz trochê za krótkie r±czki.\n\r", ch );
        return;
    }

    if ( is_safe( ch, victim ) )
    {
        return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char( "Musisz mieæ jak±¶ broñ.\n\r", ch );
        return;
    }

    if ( obj->value[ 0 ] != WEAPON_DAGGER )
    {
        send_to_char( "Przyda³by siê jaki¶ sztylet.\n\r", ch );
        return;
    }

    if ( ch->mount )
    {
        send_to_char( "Mo¿e najpierw zsi±dziesz z wierzchowca?\n\r", ch );
        return;
    }

    if ( !form_check( ch, victim, obj, gsn_backstab ) )
    {
        return;
    }

    // tutaj proszê dodawaæ te rasy, na które ma NIE DZIALAC skill, dziêki
    // temu wy³±czonmy takie rasy, jak nap rój owadów, któremu to, sami przyznacie,
    // ciê¿ko jest wsadziæ sztylet miêdzy ³opatki ;-)

    if ( !(
                str_cmp( race_table[ GET_RACE(victim) ].name, "stonoga" )
                && str_cmp( race_table[ GET_RACE(victim) ].name, "w±¿" )
                && str_cmp( race_table[ GET_RACE(victim) ].name, "water flow" )
                && str_cmp( race_table[ GET_RACE(victim) ].name, "delfin" )
                && str_cmp( race_table[ GET_RACE(victim) ].name, "¿ywio³" )
                && str_cmp( race_table[ GET_RACE(victim) ].name, "ryba" )
                && str_cmp( race_table[ GET_RACE(victim) ].name, "grzyb" )
                && str_cmp( race_table[ GET_RACE(victim) ].name, "ro¶lina" )
                && str_cmp( race_table[ GET_RACE(victim) ].name, "¿yj±ca ¶ciana" )
                && str_cmp( race_table[ GET_RACE(victim) ].name, "meduza" )
                && str_cmp( race_table[ GET_RACE(victim) ].name, "b³êdny ognik" )
                && str_cmp( race_table[ GET_RACE(victim) ].name, "rój owadów" )
                && str_cmp( race_table[ GET_RACE(victim) ].name, "owad pe³zaj±cy" )
                && str_cmp( race_table[ GET_RACE(victim) ].name, "owad lataj±cy" )
          ) )
    {
        send_to_char( "Hmm, nie za bardzo wiesz jak siê do tego zabraæ.\n\r", ch );
        return;
    }

#ifdef ENHANCED_BACKSTAB
    skill_dagger = get_skill( ch, gsn_dagger);
    if ( skill_dagger > 0 )
    {
        skill_dagger_mastery = get_skill( ch, gsn_dagger_mastery);
    }
    //dla dobrze wyszkolonego dagger i dagger mastery wait spada max o 20 %
    int multi_wait = URANGE(800, 1000 - (skill_dagger_mastery * 7 + skill_dagger * 3), 1000);
    WAIT_STATE( ch, (skill_table[ gsn_backstab ].beats * multi_wait) / 1000);
#else
    WAIT_STATE( ch, skill_table[ gsn_backstab ].beats);
#endif


    if ( hidden )
    {
        affect_strip( ch, gsn_hide );
        EXT_REMOVE_BIT( ch->affected_by, AFF_HIDE );
        skill_backstab += UMAX( 5, get_curr_stat_deprecated( ch, STAT_DEX ) - 11 );
    }
    else if ( IS_AFFECTED( ch, AFF_SNEAK ) )
    {
        skill_backstab += UMAX( 2, get_curr_stat_deprecated( ch, STAT_DEX ) - 16 );
    }

    if ( ch_vs_victim_stat_throw( ch, victim, STAT_LUC ) )
    {
        skill_backstab += 4;
    }
    else
    {
        skill_backstab -= 4;
    }

    if ( victim->position <= POS_SLEEPING )
    {
        skill_backstab += 20;
    }
    else if ( victim->position == POS_RESTING || victim->position == POS_SITTING )
    {
        skill_backstab += 10;
    }

    switch ( 5 + ch->size - victim->size )
    {
        // ch->size - victim->size == 1 ||  ch->size - victim->size == -1
        case 4:
        case 6:
            skill_backstab -= 5;
            break;
            // ch->size - victim->size == 2 ||  ch->size - victim->size == -2
        case 3:
        case 7:
            skill_backstab -= 10;
            break;
            // ch->size - victim->size == 3 ||  ch->size - victim->size == -3
        case 2:
        case 8:
            skill_backstab -= 20;
            break;
            // ch->size - victim->size == 4 ||  ch->size - victim->size == -4
        case 1:
        case 9:
            skill_backstab -= 40;
            break;
            // ch->size - victim->size == 5 ||  ch->size - victim->size == -5
        case 0:
        case 10:
            skill_backstab -= 80;
            break;
    }

    if ( IS_SET( obj->value[ 4 ], WEAPON_HEARTSEEKER ) )
    {
        skill_backstab += 20;
    }

    skill_backstab = UMAX( 3, skill_backstab );

    if( EXT_IS_SET( ch->in_room->room_flags, ROOM_SLIPPERY_FLOOR ) && number_percent() > skill_backstab/2 && !IS_AFFECTED( ch, AFF_STABILITY ) )
    {
        if( ch->sex == 2 )
        {
            act( "{5$n stara³a siê podkra¶æ za plecy $Z, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgnê³a siê i pada z jêkiem na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
            act( "{5$n stara³a siê podkra¶æ za twe plecy, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgnê³a siê i pada z jêkiem na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
        }
        else
        {
            act( "{5$n stara³ siê podkra¶æ za plecy $Z, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgn±³ siê i pada z jêkiem na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
            act( "{5$n stara³ siê podkra¶æ za twe plecy, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgn±³ siê i pada z jêkiem na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
        }
        act( "{5Starasz siê podkra¶æ za plecy $Z, jednak przez rozlan± na pod³o¿u t³ust± i ¶lisk± substancjê twoja noga po¶lizgnê³a siê i padasz na ziemiê z jêkiem.{x", ch, NULL, victim, TO_CHAR );
        damage( ch, victim, 0, gsn_backstab, DAM_NONE, FALSE );
        ch->position = POS_SITTING;
        WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
        check_improve( ch, victim, gsn_backstab, FALSE, 60 );
        return;
    }

    /* tutaj mamy cos w stylu ataku zaskoczenia w przypadku uzycia beesa na undeady - udany
       to 2 ataki zwykle sztyletami/sztyletem wieldowanym, nieudany zero atakow*/
    if ( is_undead(victim) )
    {
        if ( !can_move( victim ) || number_percent( ) < skill_backstab - victim->level )
        {
            act( "{5Niezauwa¿on<&y/a/e> podkradasz siê za plecy $Z, widz±c jednak, ¿e masz do czynienia z nieposiadaj±cym czu³ych punktów nieumar³ym wykorzystujesz element zaskoczenia i wyprowadzasz kilka szybkich pchniêæ.{x", ch, obj, victim, TO_CHAR );
            act( "{5$n podkrada siê bezszelestnie za plecy $Z, ale widz±c, ¿e ma do czynienia z nieposiadaj±cym czu³ych punktów nieumar³ym wykorzystuje element zaskoczenia i wyprowadza kilka szybkich pchniêæ.{x", ch, obj, victim, TO_NOTVICT );
            check_improve( ch, victim, gsn_backstab, TRUE, 70 );
            multi_hit( ch, victim, TYPE_UNDEFINED );
            multi_hit( ch, victim, TYPE_UNDEFINED );
            return;
        }
        else
        {
            act( "{5Starasz siê podkra¶æ za plecy $Z, jednak nadzwyczajny instynkt tej nieumar³ej istoty pozwala jej w porê przejrzeæ twe zamiary i zrobiæ unik.{x", ch, obj, victim, TO_CHAR );
            act( "{5$n stara siê podkra¶æ za plecy $Z, jednak nadzwyczajny instynkt nieumar³ego pozwala mu w porê przejrzeæ zamiary $z i zrobiæ unik.{x", ch, obj, victim, TO_NOTVICT );
            damage( ch, victim, 0, 0, DAM_NONE, FALSE );
            check_improve( ch, victim, gsn_backstab, FALSE, 60 );
            return;
        }
    }

    /*tutaj proszê dodawaæ te rasy, które maj± SPECJALNE opisy w przypadku
      trafienia, a take te, ktore maja miec jakies modyfikatory do dama
      opisy prosze pisac w fight.c, w celu szybkiego znalezienia miejsca wpisywania
      wpisac w find sentencje "backstab opisy" :P*/

    if ( ( !IS_SET( race_table[ GET_RACE( victim ) ].type, PERSON ) &&
                !IS_SET( race_table[ GET_RACE( victim ) ].type, ANIMAL ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "ogr" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "szkolny potworek" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "troll" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "wywerna" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "sobowtór" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "hydra" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "likantrop" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "mantykora" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "syrena" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "tasloi" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "troglodyta" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "umberkolos" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "³akowilk" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "yuan-ti" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "smok" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "yeti" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "kaszcz" ) &&
                str_cmp( race_table[ GET_RACE(victim) ].name, "bazyliszek" ))||
            victim->size < SIZE_SMALL ||
            victim->size > SIZE_LARGE )
            {
                damage( ch, victim, 0, gsn_backstab, DAM_NONE, TRUE );
                return;
            }

    if ( can_move(victim))
    {
        if ( ( ( IS_NPC( victim ) && EXT_IS_SET( victim->act, ACT_BARBARIAN ) ) || ( victim->class == CLASS_BARBARIAN ) ) &&  number_percent() < URANGE( 10, 4*(victim->level - ch->level + 8), 90 ))
        {
            damage( ch, victim, 0, gsn_backstab, DAM_NONE, TRUE );
            check_improve( ch, victim, gsn_backstab, FALSE, 70 );
            return;
        }
    }
    if ( !can_move( victim ) || number_percent( ) < skill_backstab )
    {
        one_hit( ch, victim, gsn_backstab, FALSE );
        check_improve( ch, victim, gsn_backstab, TRUE, 75 );
        return;
    }
    damage( ch, victim, 0, gsn_backstab, DAM_NONE, TRUE );
    check_improve( ch, victim, gsn_backstab, FALSE, 55 );
    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
    return;
}

void do_circle( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int skill;

    one_argument( argument, arg );

    skill = get_skill( ch, gsn_circle );

    if ( skill <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if( !ch->fighting )
    {
        send_to_char( "Przecie¿ nie walczysz.\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        victim = ch->fighting;
    }
    else
    {
        victim = get_char_room( ch, argument );

        if ( !victim )
        {
            send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
            add_wait_after_miss(ch, 2);
            return;
        }

        if ( victim == ch )
        {
            send_to_char( "Niez³y pomys³.\n\r", ch );
            return;
        }

        if ( victim->fighting != ch )
        {
            act( "$n nie walczy z tob±!", victim, NULL, ch, TO_VICT );
            return;
        }
    }

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char( "Przyda³by siê jaki¶ sztylet.\n\r", ch );
        return;
    }

    if ( obj->value[ 0 ] != 2 )
    {
        send_to_char( "Przyda³by siê jaki¶ sztylet.\n\r", ch );
        return;
    }

    WAIT_STATE( ch, skill_table[ gsn_circle ].beats );

    if ( !can_move( victim ) || number_percent( ) < skill )
    {
        check_improve( ch, victim, gsn_circle, TRUE, 80 );
        one_hit( ch, victim, gsn_circle, FALSE );
        return;
    }

    check_improve( ch, victim, gsn_circle, FALSE, 60 );
    damage( ch, victim, 0, gsn_circle, DAM_NONE, TRUE );

    return;
}


void do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    CHAR_DATA *tch;
    sh_int skill;

    one_argument( argument, arg );

    if ( ( skill = get_skill( ch, gsn_rescue ) ) <= 0 )
    {
        send_to_char( "Nie wiesz za bardzo jak mo¿naby kogo¶ uratowaæ.\n\r", ch );
        return;
    }

    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Kogo uratowaæ?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Mo¿e lepiej uciekaj?\n\r", ch );
        return;
    }

    if ( !IS_NPC( ch ) && IS_NPC( victim ) )
    {
        send_to_char( "Nie potrzebuje twojej pomocy!\n\r", ch );
        return;
    }

    if ( ch->fighting == victim )
    {
        send_to_char( "Lepiej skup siê na walce.\n\r", ch );
        return;
    }

    if ( ch->mount )
    {
        send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
        return;
    }

    for ( tch = ch->in_room->people; tch ; tch = tch->next_in_room )
        if ( tch->fighting == victim )
            break;

    if ( !tch )
    {
        send_to_char( "Ta osoba nie potrzebuje na razie pomocy.\n\r", ch );
        return;
    }

    WAIT_STATE( ch, skill_table[ gsn_rescue ].beats );

    // moby umiej± rescue zawsze na minimum 15%
    if (IS_NPC( ch ) && skill < 15)
    {
        skill = 15;
    }

    if ( IS_NPC( ch ) )
        skill += skill / 2;

    if ( skill <= 0 )
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        return;
    }

    if ( number_percent( ) > skill )
    {
        act( "{5Próbujesz uratowaæ $C z opresji ale ci siê nie udaje!{x", ch, NULL, victim, TO_CHAR );

        if ( ch->sex == 1 || ch->sex == 0 )
        {
            act( "{5$n próbuje uratowaæ ciê z opresji, ale mu siê nie udaje!{x", ch, NULL, victim, TO_VICT );
            act( "{5$n próbuje uratowaæ $C z opresji, ale mu siê nie udaje!{x", ch, NULL, victim, TO_NOTVICT );
        }
        else
        {
            act( "{5$n próbuje uratowaæ ciê z opresji, ale jej siê nie udaje!{x", ch, NULL, victim, TO_VICT );
            act( "{5$n próbuje uratowaæ $C z opresji, ale jej siê nie udaje!{x", ch, NULL, victim, TO_NOTVICT );
        }
        check_improve( ch, NULL, gsn_rescue, FALSE, 15 );
        return;
    }

    act( "{5Ratujesz $C z opresji!{x", ch, NULL, victim, TO_CHAR );
    act( "{5$n ratuje ciê z opresji!{x", ch, NULL, victim, TO_VICT );
    act( "{5$n wybawia $C z opresji!{x", ch, NULL, victim, TO_NOTVICT );
    check_improve( ch, NULL, gsn_rescue, TRUE, 15 );

    stop_fighting( tch, FALSE );
    stop_fighting( victim, FALSE );

    check_killer( ch, tch );
    set_fighting( ch, tch );
    set_fighting( tch, ch );

    return;
}

void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim;
    OBJ_DATA *butki;
    int dam, skill = -1, v_sit_wait, remembered_hp;
    bool sit = FALSE, self_sit = FALSE, check;

    if ( ch->mount )
    {
        send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
        return;
    }

    if ( ( skill = get_skill( ch, gsn_kick ) ) < 1 )
    {
/*         if ( IS_SET (sector_table[ch->in_room->sector_type].flag, SECT_NOREST)) */
/*         { */
       send_to_char( "{5Majtasz nogami, ale nic siê nie dzieje.{x\n\r", ch );
       act( "{5$n zaczyna dziwnie majtaæ nogami.{x", ch, NULL, NULL, TO_ROOM );
/*         } */
	/* Tener: wykomentowujê, bo to g³upie, skoro nie umiej± kopaæ, to niech nie kopi± wcale [30052008] */
/*         else */
/*         { */
/*             if ( ch->position > POS_SITTING ) */
/*             { */
/*                 ch->position = POS_SITTING; */
/*             } */
/*             affect_strip( ch, gsn_fly ); */
/*             affect_strip( ch, gsn_float ); */
/*             send_to_char( "{5Tracisz równowagê i przewracasz siê.{x\n\r", ch ); */
/*             act( "{5$n po nieudanym kopniêciu traci rownowagê i przewraca siê.{x", ch, NULL, NULL, TO_ROOM ); */
/*         } */
/*         WAIT_STATE( ch,  6 ); */
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        victim = ch->fighting;

        if ( victim == NULL )
        {
            send_to_char( "Kogo chcesz kopn±æ?\n\r", ch );
            return;
        }
    }
    else if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        add_wait_after_miss(ch, 2);
        return;
    }

    if ( ch == victim )
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        return;
    }

    if ( !form_check( ch, victim, NULL, gsn_kick ) )
    {
        if ( number_percent() > skill )
        {
            affect_strip( ch, gsn_fly );
            affect_strip( ch, gsn_float );
            if ( ch->position > POS_SITTING )
            {
                ch->position = POS_SITTING;
            }
            send_to_char( "{5Tracisz rownowagê i przewracasz siê.{x\n\r", ch );
            act( "{5$n po nieudanym kopniêciu traci rownowagê i przewraca siê.{x", ch, NULL, NULL, TO_ROOM );
            WAIT_STATE( ch,  24 );
        }
        else
        {
            WAIT_STATE( ch,  12 );
        }
        return;
    }

    if ( is_safe( ch, victim ) )
    {
        act( "{5Jako¶ nie dajesz rady kopn±æ $Z.{x", ch, NULL, victim, TO_CHAR );
        return;
    }

    /* obrazenia:
     * baza = 2d4
     * + bonus ze skilla
     * plus damroll/2
     */

    if( check_trick( ch, victim, SN_TRICK_VERTICAL_KICK ) )
        return;

    /* wieksi kopia jak w mythie, na kawaleczki rozwalajac */
    switch ( ch->size )
    {
        case SIZE_TINY: dam = dice( 1, 6 );	break;
        case SIZE_SMALL: dam = dice( 2, 5 );	break;
        case SIZE_MEDIUM: dam = dice( 2, 6 );	break;
        case SIZE_LARGE: dam = 15 + dice( 3, 5 );	break;
        case SIZE_HUGE: dam = 20 + dice( 5, 6 );	break;
        case SIZE_GIANT: dam = 30 + dice( 6, 10 );	break;
        default: dam = dice( 2, 5 );	break;
    }

    //bonus od skilla
    dam += skill / 10;
    dam += GET_DAMROLL( ch, NULL )/2;

    //bez butko tylko 75% dama
    if ( ( butki = get_eq_char( ch, WEAR_FEET ) ) == NULL )
    {
        if ( !IS_NPC( ch ) )
            dam = ( dam * 75 ) / 100;
    }
    else
    {
        // ale z koleji metalowe butki dodaj± obra¿eñ
        if ( IS_SET( material_table[ butki->material ].flag, MAT_METAL ) )
        {
            dam += number_range( 1, 6 );
        }
    }

    WAIT_STATE( ch,  24 );

    if ( number_percent() < 10 - ( skill / 10 ) )
    {
        self_sit = TRUE;
    }
    v_sit_wait = number_range( 7, 25 );
    remembered_hp = victim->hit;

    if( EXT_IS_SET( ch->in_room->room_flags, ROOM_SLIPPERY_FLOOR ) && number_percent() > skill/2 && !IS_AFFECTED( ch, AFF_STABILITY ) )
    {
        if( ch->sex == 2 )
        {
            act( "{5$n stara³a siê kopn±æ $C, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgnê³a siê i pada z jêkiem na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
            act( "{5$n stara³a siê kopn±æ ciê, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgnê³a siê i pada z jêkiem na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
        }
        else
        {
            act( "{5$n stara³ siê kopn±æ $C, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgn±³ siê i pada z jêkiem na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
            act( "{5$n stara³ siê kopn±æ ciê, jednak przez rozlan± na pod³o¿u ¶lisk± i t³ust± substancjê po¶lizgn±³ siê i pada z jêkiem na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
        }
        act( "{5Starasz siê kopn±æ $C, jednak przez rozlan± na pod³o¿u t³ust± i ¶lisk± substancjê twoja noga po¶lizgnê³a siê i padasz na ziemiê z jêkiem.{x", ch, NULL, victim, TO_CHAR );
        damage( ch, victim, 0, gsn_kick, DAM_NONE, FALSE );
        affect_strip( ch, gsn_fly );
        affect_strip( ch, gsn_float );
        ch->position = POS_SITTING;
        WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
        check_improve( ch, victim, gsn_kick, FALSE, 50 );
        return;
    }

    if ( UMAX( skill, 10 ) > number_percent() || !can_move( victim ) )
    {
        check = FALSE;
        if (
                ch->size >= victim->size
                && victim->position > POS_SITTING
                && victim->hit > 0
                && (
                    IS_SET( race_table[ GET_RACE( victim ) ].type, ANIMAL )
                    || IS_SET( race_table[ GET_RACE( victim ) ].type, PERSON )
                    || !IS_NPC(victim)
                   )
           )
        {
            if ( ( number_percent() < 3 + ( skill / 7 ) ) || ( dam > 20 && number_percent() < ( dam - 20 ) ) )
            {
                sit = TRUE;
            }
        }
        damage( ch, victim, dam, gsn_kick, DAM_BASH, TRUE );
        check_defensive_spells(ch, victim);
        check_improve( ch, victim, gsn_kick, TRUE, 40 );
    }
    else
    {
        damage( ch, victim, 0, gsn_kick, DAM_BASH, TRUE );
        check_defensive_spells(ch, victim);
        check_improve( ch, victim, gsn_kick, FALSE, 50 );
    }

    if ( sit && victim->in_room != NULL && victim->hit > 0 && !IS_AFFECTED(victim,AFF_STABILITY) && victim->hit != remembered_hp )
    {
        WAIT_STATE( victim, v_sit_wait );
        if ( victim->position > POS_SITTING )
        {
            victim->position = POS_SITTING;
        }
        affect_strip( victim, gsn_fly );
        affect_strip( victim, gsn_float );
        act( "{5$N chwieje siê przez chwilê po czym z jêkniêciem przewraca siê na ziemiê.{x", ch, NULL, victim, TO_CHAR );
        act( "{5Celne kopniêcie $z wytr±ca ciê z równowagi, z jêkniêciem przewracasz siê na ziemiê.{x", ch, NULL, victim, TO_VICT );
        act( "{5$N chwieje siê przez chwilê po czym z jêkniêciem przewraca siê na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
    }

    if ( self_sit )
    {
        if ( ch->position > POS_SITTING )
        {
            ch->position = POS_SITTING;
        }
        affect_strip( ch, gsn_fly );
        affect_strip( ch, gsn_float );
        send_to_char( "{5Tracisz równowagê i przewracasz siê.{x\n\r", ch );
        act( "{5$n po kopniêciu traci równowagê i przewraca siê.{x", ch, NULL, NULL, TO_ROOM );
    }

    WAIT_STATE( ch, 24 );

    return;
}

void do_crush( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim;
    OBJ_DATA *body;
    OBJ_DATA *arms;
    OBJ_DATA *legs;
    OBJ_DATA *hands;
    OBJ_DATA *feet;
    OBJ_DATA *head;
    OBJ_DATA *bron;
    AFFECT_DATA af;
    int dam, dama, skill, sila, chance, ch_dex, v_dex, check_disarm, check_daze, obj_cr, bashed;

    if ( ch->size < SIZE_LARGE || !IS_NPC(ch) )
    {
        return;
    }

    if ( ( skill = get_skill( ch, gsn_crush ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        victim = ch->fighting;

        if ( victim == NULL )
        {
            send_to_char( "Komu chcesz uszkodziæ zbrojê?\n\r", ch );
            return;
        }
    }
    else if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        add_wait_after_miss(ch, 2);
        return;
    }

    if ( ch == victim )
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        return;
    }

    if ( is_safe( ch, victim ) )
    {
        send_to_char( "Nie dasz rady.\n\r", ch );
        return;
    }

    ch_dex = get_curr_stat_deprecated( ch, STAT_DEX );
    v_dex = get_curr_stat_deprecated( victim, STAT_DEX );

    /* sejw na dexa */
    if ( can_move(victim) && ch_dex*3 + number_range( 0, 75) < v_dex*3 + number_range( 0, 50))
    {
        act( "{5Rzucasz siê na $C, ale w ostatniej chwili twojemu celowi udaje siê odsun±æ i twój cios przecina powietrze.{x", ch, NULL, victim, TO_CHAR );
        act( "{5$n rzuca siê na ciebie, ale w ostatniej chwili udaje ci siê zrobiæ unik i $s cios przecina tylko powietrze.{x", ch, NULL, victim, TO_VICT );
        act( "{5$n rzuca siê na $C, jednak $E robi unik i cios przecina tylko powietrze.{x", ch, NULL, victim, TO_NOTVICT );
        damage( ch, victim, 0, gsn_crush, DAM_BASH, TRUE );
        WAIT_STATE( ch, skill_table[ gsn_crush ].beats );
        return;
    }

    sila = get_curr_stat_deprecated( ch, STAT_STR );
    dam = dice( sila, 2) * 2/3;
    body = get_eq_char( victim, WEAR_BODY );
    arms = get_eq_char( victim, WEAR_ARMS );
    legs = get_eq_char( victim, WEAR_LEGS );
    hands = get_eq_char( victim, WEAR_HANDS );
    feet = get_eq_char( victim, WEAR_FEET );
    head = get_eq_char( victim, WEAR_HEAD );
    bron = get_eq_char( victim, WEAR_WIELD );
    check_disarm = 0;
    check_daze = 0;
    bashed = 0;

    obj_cr = number_range(number_range(10,15), sila + number_range(5,10));

    if ( ch->level <= number_range( 20, 25 ) )
    {
        dam /= 2;
        obj_cr /= 3;
    }

    if ( bron == NULL )
    {
        bron = get_eq_char( victim, WEAR_SECOND );
    }

    if ( check_mirror_image( victim, ch ) )
    {
        act( "{5Twoje druzgocz±ce uderzenie przebija na wylot wizerunek $Z, a ten zaraz po tym nagle znika.{x", ch, NULL, victim, TO_CHAR );
        act( "{5Druzgocz±ce uderzenie $z przebija na wylot jedno z twoich lustrzanych odbic.{x", ch, NULL, victim, TO_VICT );
        act( "{5Druzgocz±ce uderzenie $z trafia w lustrzane odbicie $Z.{x", ch, NULL, victim, TO_NOTVICT );
        WAIT_STATE( ch, skill_table[ gsn_crush ].beats );
        damage( ch, victim, 0, gsn_crush, DAM_BASH, TRUE );
        return;
    }

    bool good_part = FALSE; // czy udalo nam sie wybrac dobra czesc ciala; nie zapetli sie, bo w przypadkach 1,2 i 3 trafia w bezwarunkowo w cialo
    int counter = 0;

    while( !good_part )
       {
	  counter++;
	  if ( counter > 1000 )
	     {
		bug("do_crush: a¿ 1000 uruchomieñ pêtli!", 0 );
		return;
	     }
	  chance = number_range( 1, 10);

	  switch (chance)
	     {
	     case 1:
	     case 2:
	     case 3:
		good_part = TRUE;
		if ( body == NULL )
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w go³y tors $Z.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twój go³y tors.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w go³y tors $Z.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*2;
		      break;
		   }
		if ( IS_OBJ_STAT(body, ITEM_UNDESTRUCTABLE))
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w tors $Z, jednak $S zbroja pozostaje nienaruszona.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twój tors, jednak twoja zbroja pozostaje nienaruszona.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w tors $Z, jednak $S zbroja pozostaje nienaruszona.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*2/3;
		      break;
		   }
		if ( body->material == 6 || body->material == 12 || body->material == 17 || body->material == 18 || body->material == 20 || body->material == 24 || body->material == 25 || body->material == 26 || body->material == 29 || body->material == 30 || body->material == 35 || body->material == 37 || body->material == 38 || body->material == 39 || body->material == 44 || body->material == 45)
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w s³abo chroniony tors $Z.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twój s³abo chroniony tors.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w s³abo chroniony tors $Z.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*3/2;
		      break;
		   }
		if ( body->material == 11 || body->material == 15 || body->material == 31 )
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w tors $Z przebijaj±c $S zbrojê.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twój tors przebijaj±c tw± zbrojê.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w tors $Z przebijaj±c $S zbrojê.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*4/3;
		      destroy_obj( body->carried_by, body );
		      break;
		   }
		body->condition -= obj_cr;
		act( "{5Twoje druzgocz±ce uderzenie trafia w tors $Z uszkadzaj±c $S zbrojê.{x", ch, NULL, victim, TO_CHAR );
		act( "{5Druzgocz±ce uderzenie $z trafia w twój tors uszkadzaj±c tw± zbrojê.{x", ch, NULL, victim, TO_VICT );
		act( "{5Druzgocz±ce uderzenie $z trafia w tors $Z uszkadzaj±c $S zbrojê.{x", ch, NULL, victim, TO_NOTVICT );
		dama = dam;
		if (body->condition < 1 )
		   destroy_obj( body->carried_by, body );
		break;
	     case 4:
		good_part = TRUE;
		af.where	 = TO_AFFECTS;
		af.type		 = 90;
		af.level	 = 20;
		af.duration  = 4;
		af.rt_duration = 0;
		af.location  = APPLY_DEX;
		af.modifier  = -3;
		af.bitvector = &AFF_SLOW;
		affect_to_char(	victim,	&af, NULL, TRUE	 );
		if ( feet == NULL )
		   {
		      // Obs³uga zwierzaków, bo przecie¿ maj± nogi, ale chyba lepiej napisaæ ³apa, ni¿ stopa?
		      if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , ANIMAL ) )
			 {
			    act( "{5Twoje druzgocz±ce uderzenie trafia w ³apy $Z.{x", ch, NULL, victim, TO_CHAR );
			    act( "{5Druzgocz±ce uderzenie $z trafia w twoje ³apy.{x", ch, NULL, victim, TO_VICT );
			    act( "{5Druzgocz±ce uderzenie $z trafia w ³apy $Z.{x", ch, NULL, victim, TO_NOTVICT );
			 }
		      else
			 {
			    act( "{5Twoje druzgocz±ce uderzenie trafia w go³e stopy $Z.{x", ch, NULL, victim, TO_CHAR );
			    act( "{5Druzgocz±ce uderzenie $z trafia w twoje go³e stopy.{x", ch, NULL, victim, TO_VICT );
			    act( "{5Druzgocz±ce uderzenie $z trafia w go³e stopy $Z.{x", ch, NULL, victim, TO_NOTVICT );
			 }
		      send_to_char("Ból stopy powoduje ¿e zaczynasz utykaæ.\n\r", victim );
		      act("$n zaczyna utykaæ, a przy ka¿dym ruchu grymas bólu pojawia siê na $s twarzy.",victim,NULL,NULL,TO_ROOM);
		      dama = dam*2;
		      break;
		   }
		if ( IS_OBJ_STAT(feet, ITEM_UNDESTRUCTABLE))
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w stopy $Z, jednak $S buty pozostaj± nienaruszone.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoje stopy, jednak twoje buty pozostaj± nienaruszone.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w stopy $Z, jednak $S buty pozostaj± nienaruszone.{x", ch, NULL, victim, TO_NOTVICT );
		      send_to_char("Ból stopy powoduje, ¿e zaczynasz utykaæ.\n\r", victim );
		      act("$n zaczyna utykaæ na praw± stopê, a przy ka¿dym ruchu grymas bólu pojawia siê na $s twarzy.",victim,NULL,NULL,TO_ROOM);
		      dama = dam*2/3;
		      break;
		   }
		if ( feet->material == 6 || feet->material == 12 || feet->material == 17 || feet->material == 18 || feet->material == 20 || feet->material == 24 || feet->material == 25 || feet->material == 26 || feet->material == 29 || feet->material == 30 || feet->material == 35 || feet->material == 37 || feet->material == 38 || feet->material == 39 || feet->material == 44 || feet->material == 45)
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w s³abo chronione stopy $Z.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoje s³abo chronione stopy.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w s³abo chronione stopy $Z.{x", ch, NULL, victim, TO_NOTVICT );
		      send_to_char("Ból stopy powoduje ¿e zaczynasz utykaæ.\n\r", victim );
		      act("$n zaczyna utykaæ na praw± stopê, a przy ka¿dym ruchu grymas bólu pojawia siê na $s twarzy.",victim,NULL,NULL,TO_ROOM);
		      dama = dam*3/2;
		      break;
		   }
		if ( feet->material == 11 || feet->material == 15 || feet->material == 31 )
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w stopy $Z przebijaj±c $S buty.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoje stopy przebijaj±c twe buty.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w stopy $Z przebijaj±c $S buty.{x", ch, NULL, victim, TO_NOTVICT );
		      send_to_char("Ból stopy powoduje ¿e zaczynasz utykaæ.\n\r", victim );
		      act("$n zaczyna utykaæ na praw± stopê, a przy ka¿dym ruchu grymas bólu pojawia siê na $s twarzy.",victim,NULL,NULL,TO_ROOM);
		      dama = dam*4/3;
		      destroy_obj( feet->carried_by, feet );
		      break;
		   }
		feet->condition -= obj_cr;
		act( "{5Twoje druzgocz±ce uderzenie trafia w stopy $Z uszkadzaj±c $S buty.{x", ch, NULL, victim, TO_CHAR );
		act( "{5Druzgocz±ce uderzenie $z trafia w twoje stopy uszkadzaj±c twe buty.{x", ch, NULL, victim, TO_VICT );
		act( "{5Druzgocz±ce uderzenie $z trafia w stopy $Z uszkadzaj±c $S buty.{x", ch, NULL, victim, TO_NOTVICT );
		send_to_char("Ból stopy powoduje ¿e zaczynasz utykaæ.\n\r", victim );
		act("$n zaczyna utykaæ na praw± stopê, a przy ka¿dym ruchu grymas bólu pojawia siê na $s twarzy.",victim,NULL,NULL,TO_ROOM);
		dama = dam;
		if (feet->condition < 1 )
		   destroy_obj( feet->carried_by, feet );
		break;
	     case 5:
		if ( !IS_SET( victim->parts, PART_HEAD ) ) {
		   continue;
		}
		good_part = TRUE;
		if ( head == NULL )
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w nieos³onion± g³owê $Z.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoj± nieos³onion± g³owê.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w nieos³onion± g³owê $Z.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*2;
		      check_daze = 1;
		      break;
		   }
		if ( head->item_type != ITEM_ARMOR )
		   {
		      check_daze = 1;
		   }
		if ( IS_OBJ_STAT(head, ITEM_UNDESTRUCTABLE))
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w g³owê $Z, jednak $S he³m pozostaje nienaruszony.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoj± g³owê, jednak twój he³m pozostaje nienaruszony.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w g³owê $Z, jednak $S he³m pozostaje nienaruszony.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*2/3;
		      if ( head->item_type == ITEM_ARMOR )
			 {
			    if ( head->value[1] + 1 > number_range(0,10))
			       check_daze = 1;
			 }
		      break;
		   }
		if ( head->material == 6 || head->material == 12 || head->material == 17 || head->material == 18 || head->material == 20 || head->material == 24 || head->material == 25 || head->material == 26 || head->material == 29 || head->material == 30 || head->material == 35 || head->material == 37 || head->material == 38 || head->material == 39 || head->material == 44 || head->material == 45)
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w s³abo chronion± g³owê $Z.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoj± s³abo chronion± g³owê.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w s³abo chronion± g³owê $Z.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*3/2;
		      if ( head->item_type == ITEM_ARMOR )
			 {
			    if ( head->value[1] + 1 > number_range(0,10))
			       check_daze = 1;
			 }
		      break;
		   }
		if ( head->material == 11 || head->material == 15 || head->material == 31 )
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w g³owê $Z przebijaj±c $S he³m.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoj± g³owê przebijaj±c twój he³m.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w g³owê $Z przebijaj±c $S he³m.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*4/3;
		      destroy_obj( head->carried_by, head );
		      check_daze = 1;
		      break;
		   }
		head->condition -= obj_cr;
		act( "{5Twoje druzgocz±ce uderzenie trafia w g³owê $Z uszkadzaj±c $S he³m.{x", ch, NULL, victim, TO_CHAR );
		act( "{5Druzgocz±ce uderzenie $z trafia w twoj± g³owê uszkadzaj±c twój he³m.{x", ch, NULL, victim, TO_VICT );
		act( "{5Druzgocz±ce uderzenie $z trafia w g³owê $Z uszkadzaj±c $S he³m.{x", ch, NULL, victim, TO_NOTVICT );
		if ( head->item_type == ITEM_ARMOR )
		   {
		      if ( head->value[1] + 1 > number_range(0,13))
			 check_daze = 1;
		   }
		dama = dam;
		if (head->condition < 1 )
		   {
		      destroy_obj( head->carried_by, head );
		      check_daze = 1;
		   }
		break;
	     case 6:
	     case 7:
		if ( !IS_SET( victim->parts, PART_ARMS ) ) {
		   continue;
		}
		good_part = TRUE;
		if ( arms == NULL )
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w go³e ramiona $Z.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoje go³e ramiona.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w go³e ramiona $Z.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*2;
		      break;
		   }
		if ( IS_OBJ_STAT(arms, ITEM_UNDESTRUCTABLE))
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w ramiê $Z, jednak $S pancerz pozostaje nienaruszony.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoje ramiê, jednak twój pancerz pozostaje nienaruszony.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w ramiê $Z, jednak $S pancerz pozostaje nienaruszony.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*2/3;
		      break;
		   }
		if ( arms->material == 6 || arms->material == 12 || arms->material == 17 || arms->material == 18 || arms->material == 20 || arms->material == 24 || arms->material == 25 || arms->material == 26 || arms->material == 29 || arms->material == 30 || arms->material == 35 || arms->material == 37 || arms->material == 38 || arms->material == 39 || arms->material == 44 || arms->material == 45)
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w s³abo chronione ramiona $Z.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoje s³abo chronione ramiona.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w s³abo chronione ramiona $Z.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*3/2;
		      break;
		   }
		if ( arms->material == 11 || arms->material == 15 || arms->material == 31 )
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w ramiê $Z przebijaj±c $S pancerz.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoje ramiê przebijaj±c twój pancerz.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w ramiê $Z przebijaj±c $S pancerz.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*4/3;
		      destroy_obj( arms->carried_by, arms );
		      break;
		   }
		arms->condition -= obj_cr;
		act( "{5Twoje druzgocz±ce uderzenie trafia w ramiona $Z uszkadzaj±c $S pancerz.{x", ch, NULL, victim, TO_CHAR );
		act( "{5Druzgocz±ce uderzenie $z trafia w twe ramiona uszkadzaj±c twój pancerz.{x", ch, NULL, victim, TO_VICT );
		act( "{5Druzgocz±ce uderzenie $z trafia w ramiona $Z uszkadzaj±c $S pancerz.{x", ch, NULL, victim, TO_NOTVICT );
		dama = dam;
		if (arms->condition < 1 )
		   destroy_obj( arms->carried_by, arms );
		break;
	     case 8:
	     case 9:
		if ( !IS_SET( victim->parts, PART_LEGS ) ) {
		   continue;
		}
		good_part = TRUE;
		if ( victim->position > POS_SITTING && !IS_AFFECTED(victim,AFF_STABILITY ) && number_range(0,30) < victim->level )
		   {
		      bashed = 1;
		      victim->position = POS_SITTING;
		      WAIT_STATE( victim, 2*PULSE_VIOLENCE );
		   }
		if ( legs == NULL )
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w go³e nogi $Z.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoje go³e nogi.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w go³e nogi $Z.{x", ch, NULL, victim, TO_NOTVICT );
		      if ( bashed )
			 {
			    act( "{5$N chwieje siê przez chwilê po czym z jêkniêciem przewraca siê na ziemiê.{x", ch, NULL, victim, TO_CHAR );
			    act( "{5Celne trafienie $z wytr±ca ciê z równowagi, z jêkniêciem przewracasz siê na ziemiê.{x", ch, NULL, victim, TO_VICT );
			    act( "{5$N chwieje siê przez chwilê po czym z jêkniêciem przewraca siê na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
			 }
		      dama = dam*2;
		      break;
		   }
		if ( IS_OBJ_STAT(legs, ITEM_UNDESTRUCTABLE))
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w nogê $Z, jednak $S pancerz pozostaje nienaruszony.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoj± nogê, jednak twój pancerz pozostaje nienaruszony.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w nogê $Z, jednak $S pancerz pozostaje nienaruszony.{x", ch, NULL, victim, TO_NOTVICT );
		      if ( bashed )
			 {
			    act( "{5$N chwieje siê przez chwilê po czym z jêkniêciem przewraca siê na ziemiê.{x", ch, NULL, victim, TO_CHAR );
			    act( "{5Celne trafienie $z wytr±ca ciê z równowagi, z jêkniêciem przewracasz siê na ziemiê.{x", ch, NULL, victim, TO_VICT );
			    act( "{5$N chwieje siê przez chwilê po czym z jêkniêciem przewraca siê na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
			 }
		      dama = dam*2/3;
		      break;
		   }
		if ( legs->material == 6 || legs->material == 12 || legs->material == 17 || legs->material == 18 || legs->material == 20 || legs->material == 24 || legs->material == 25 || legs->material == 26 || legs->material == 29 || legs->material == 30 || legs->material == 35 || legs->material == 37 || legs->material == 38 || legs->material == 39 || legs->material == 44 || legs->material == 45)
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w s³abo chronione nogi $Z.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoje s³abo chronione nogi.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w s³abo chronione nogi $Z.{x", ch, NULL, victim, TO_NOTVICT );
		      if ( bashed )
			 {
			    act( "{5$N chwieje siê przez chwilê po czym z jêkniêciem przewraca siê na ziemiê.{x", ch, NULL, victim, TO_CHAR );
			    act( "{5Celne trafienie $z wytr±ca ciê z równowagi, z jêkniêciem przewracasz siê na ziemiê.{x", ch, NULL, victim, TO_VICT );
			    act( "{5$N chwieje siê przez chwilê po czym z jêkniêciem przewraca siê na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
			 }
		      dama = dam*3/2;
		      break;
		   }
		if ( legs->material == 11 || legs->material == 15 || legs->material == 31 )
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w nogê $Z przebijaj±c $S pancerz.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoj± nogê przebijaj±c twój pancerz.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w nogê $Z przebijaj±c $S pancerz.{x", ch, NULL, victim, TO_NOTVICT );
		      if ( bashed )
			 {
			    act( "{5$N chwieje siê przez chwilê po czym z jêkniêciem przewraca siê na ziemiê.{x", ch, NULL, victim, TO_CHAR );
			    act( "{5Celne trafienie $z wytr±ca ciê z równowagi, z jêkniêciem przewracasz siê na ziemiê.{x", ch, NULL, victim, TO_VICT );
			    act( "{5$N chwieje siê przez chwilê po czym z jêkniêciem przewraca siê na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
			 }
		      dama = dam*4/3;
		      destroy_obj( legs->carried_by, legs );
		      break;
		   }
		legs->condition -= obj_cr;
		act( "{5Twoje druzgocz±ce uderzenie trafia w nogi $Z uszkadzaj±c $S pancerz.{x", ch, NULL, victim, TO_CHAR );
		act( "{5Druzgocz±ce uderzenie $z trafia w twe nogi uszkadzaj±c twój pancerz.{x", ch, NULL, victim, TO_VICT );
		act( "{5Druzgocz±ce uderzenie $z trafia w nogi $Z uszkadzaj±c $S pancerz.{x", ch, NULL, victim, TO_NOTVICT );
		if ( bashed )
		   {
		      act( "{5$N chwieje siê przez chwilê po czym z jêkniêciem przewraca siê na ziemiê.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Celne trafienie $z wytr±ca ciê z równowagi, z jêkniêciem przewracasz siê na ziemiê.{x", ch, NULL, victim, TO_VICT );
		      act( "{5$N chwieje siê przez chwilê po czym z jêkniêciem przewraca siê na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
		   }
		dama = dam;
		if (legs->condition < 1 )
		   destroy_obj( legs->carried_by, legs );
		break;
	     case 10:
		if ( !IS_SET( victim->parts, PART_HANDS ) ) {
		   continue;
		}
		good_part = TRUE;
		if ( bron != NULL )
		   {
		      if ( bron->value[4] != WEAPON_TWO_HANDS )
			 {
			    if ( sila*2 > number_range(0,100))
			       {
				  check_disarm = 1;
			       }
			 }
		      if ( bron->value[4] == WEAPON_TWO_HANDS )
			 {
			    if ( sila*3/2 > number_range(0,100))
			       {
				  check_disarm = 1;
			       }
			 }
		   }
		if ( hands == NULL )
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w go³e d³onie $Z.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoje go³e d³onie.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w go³e d³onie $Z.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*2;
		      break;
		   }
		if ( IS_OBJ_STAT(hands, ITEM_UNDESTRUCTABLE))
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w d³oñ $Z, jednak $S rêkawice pozostaj± nienaruszone.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoj± d³oñ, jednak twoje rêkawice pozostaj± nienaruszone.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w d³oñ $Z, jednak $S rêkawice pozostaj± nienaruszone.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*2/3;
		      break;
		   }
		if ( hands->material == 6 || hands->material == 12 || hands->material == 17 || hands->material == 18 || hands->material == 20 || hands->material == 24 || hands->material == 25 || hands->material == 26 || hands->material == 29 || hands->material == 30 || hands->material == 35 || hands->material == 37 || hands->material == 38 || hands->material == 39 || hands->material == 44 || hands->material == 45)
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w s³abo chronione d³onie $Z.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoje s³abo chronione d³onie.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w s³abo chronione d³onie $Z.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*3/2;
		      break;
		   }
		if ( hands->material == 11 || hands->material == 15 || hands->material == 31 )
		   {
		      act( "{5Twoje druzgocz±ce uderzenie trafia w d³onie $Z przebijaj±c $S rêkawice.{x", ch, NULL, victim, TO_CHAR );
		      act( "{5Druzgocz±ce uderzenie $z trafia w twoje d³onie przebijaj±c twoje rêkawice.{x", ch, NULL, victim, TO_VICT );
		      act( "{5Druzgocz±ce uderzenie $z trafia w d³onie $Z przebijaj±c $S rêkawice.{x", ch, NULL, victim, TO_NOTVICT );
		      dama = dam*4/3;
		      destroy_obj( hands->carried_by, hands );
		      break;
		   }
		hands->condition -= obj_cr;
		act( "{5Twoje druzgocz±ce uderzenie trafia w d³onie $Z uszkadzaj±c $S rêkawice.{x", ch, NULL, victim, TO_CHAR );
		act( "{5Druzgocz±ce uderzenie $z trafia w twe d³onie uszkadzaj±c twoje rêkawice.{x", ch, NULL, victim, TO_VICT );
		act( "{5Druzgocz±ce uderzenie $z trafia w d³onie $Z uszkadzaj±c $S rêkawice.{x", ch, NULL, victim, TO_NOTVICT );
		dama = dam;
		if (hands->condition < 1 )
		   destroy_obj( hands->carried_by, hands );
		break;
	     }
       } // koniec rolowania czesci ciala

    if (check_disarm == 1)
    {
        if ( !IS_OBJ_STAT( bron, ITEM_NOREMOVE ) )
        {
		/*artefact*/
		if ( is_artefact( bron ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
			artefact_from_char( bron, victim );
		obj_from_char( bron );
//Brohacz: zeby itemy z nodrop nie ladowaly na ziemi tylko w inv, przeklejone ze skilla disarm
		if ( IS_OBJ_STAT( bron, ITEM_NODROP ) || IS_OBJ_STAT( bron, ITEM_INVENTORY ) )
		{
			/*artefact*/
			if ( is_artefact( bron ) && !IS_NPC( victim ) && !IS_IMMORTAL( victim ) )
				artefact_to_char( bron, victim );
			obj_to_char( bron, victim );
		}
		else
		{
			obj_to_room( bron, victim->in_room );
			if ( IS_NPC( victim ) && victim->wait == 0 && can_see_obj( victim, bron ) )
				get_obj( victim, bron, NULL );
		}
       		act( "{5Twój cios wytr±ca $X broñ z r±k!{x", ch, NULL, victim, TO_CHAR );
       		act( "{5Broñ wypada ci z r±k!{x", ch, NULL, victim, TO_VICT );
        	act( "{5Cios $z wytr±ca $X broñ z r±k!{x", ch, NULL, victim, TO_NOTVICT );
        }
    }

    if (check_daze == 1)
    {
        af.where	 = TO_AFFECTS;
        af.type		 = 304;
        af.level	 = 20;
        af.location  = APPLY_NONE;
        af.duration  = number_range(1,3);
        af.rt_duration = 0;
        af.modifier  = 0;
        af.bitvector = &AFF_DAZE;
        affect_to_char(	victim,	&af, NULL, TRUE	 );
        act( "{5Twój cios l±duje na czole $Z któr$R przez chwilê chwieje siê og³uszon$t.{x", ch, victim->sex == 2 ? "a" : victim->sex == 0 ? "e" : "y", victim, TO_CHAR );
        act( "{5Cios $z l±duje ci na czole. Przez chwilê nie widzisz niczego oprócz gwiazd.{x", ch, NULL, victim, TO_VICT );
        act( "{5Cios $z l±duje na czole $Z, któr$R przez chwilê chwieje siê og³uszon$t.{x", ch, victim->sex == 2 ? "a" : victim->sex == 0 ? "e" : "y", victim, TO_NOTVICT );
    }

    if (victim->hit - dama < -11 )
    {
        act( "{5$N chwieje siê przez chwilê, po czym spogl±da ostatni raz na ciebie martwym wzrokiem i upada bezw³adnie na ziemiê.{x", ch, NULL, victim, TO_CHAR );
        act( "{5Tracisz czucie na ca³ym ciele, potworny ból ogarnia ciebie ca³<&ego/±/e>! Nagle co¶ uderza ciê z ogromn± si³± w plecy - w ostatnim przeb³ysku my¶li dochodzisz do wniosku, ¿e to ziemia.{x", ch, NULL, victim, TO_VICT );
        act( "{5$N chwieje siê przez chwilê po czym upada bezw³adnie na ziemiê.{x", ch, NULL, victim, TO_NOTVICT );
    }

    WAIT_STATE( ch, skill_table[ gsn_crush ].beats );
    damage( ch, victim, dama, gsn_crush, DAM_BASH, TRUE );
    return;
}

void do_tail( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * victim;
	CHAR_DATA *vch, *vch_next;
	AFFECT_DATA af;
	int dam, skill = 100, ch_dex, v_dex, vluck, chance;
	bool sit = FALSE, self_sit = FALSE;

	save_debug_info("skills.c => do_tail", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

	if ( ( skill = get_skill( ch, gsn_tail ) ) <= 0 )
	{
		send_to_char( "Huh?\n\r", ch );
		return;
	}

	if ( argument[ 0 ] == '\0' )
	{
		victim = ch->fighting;

		if ( victim == NULL )
		{
			send_to_char( "Kogo chcesz uderzyæ swoim ogonem?\n\r", ch );
			return;
		}
	}
	else if ( ( victim = get_char_room( ch, argument ) ) == NULL )
	{
		send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
		add_wait_after_miss(ch, 2);
		return;
	}

	if ( ch == victim )
	{
		send_to_char( "Nie uda³o ci siê uderzyæ siebie ogonem.\n\r", ch );
		return;
	}

	if ( is_safe( ch, victim ) )
		return;

	if ( IS_NPC( ch ) )
	{
		if ( !IS_SET( ch->parts, PART_TAIL ) )
		{
			//rellik, zakomentowuje, po co przesy³aæ komunikat do NPC??? [20080518]
			//send_to_char( "Nie mo¿esz tego zrobiæ nie maj±c ogona!\n\r", ch );
			return;
		}
	}

	ch_dex = get_curr_stat_deprecated( ch, STAT_DEX );

	chance = number_percent();

	if ( number_percent() < 10 - ( skill / 20 ) )
		self_sit = TRUE;

	switch ( ch->size )
	{
	case SIZE_TINY: dam = dice( 1, 3 );		break;
	case SIZE_SMALL: dam = dice( 1, 6 );		break;
	case SIZE_MEDIUM: dam = dice( 2, 6 );		break;
	case SIZE_LARGE: dam = 20 + dice( 3, 5 );	break;
	case SIZE_HUGE: dam = 25 + dice( 4, 5 );	break;
	case SIZE_GIANT: dam = 30 + dice( 5, 6 );	break;
	default: dam = dice( 3, 3 );			break;
	}

	//bonus od skilla
	dam += GET_DAMROLL( ch, NULL ) / 3;

	for ( vch = ch->in_room->people; vch; vch = vch_next )
	{
		vch_next = vch->next_in_room;

		if ( is_same_group( ch, vch ) )
		{
			continue;
		}
		//jesli mniej ni¿ 5% to leci po wszystkich w roomie, jesli wieksza to tylko victim
		if ( chance > 5 && vch != victim )
			continue;


		chance = number_percent();

		vluck = get_curr_stat_deprecated( vch, STAT_LUC );
		//rellik, czy tu nie powinno byæ raczej ( vch, STAT_DEX ) ? bo inaczej uderza w vch a sprawdza stat od victim [20080518]
		v_dex = get_curr_stat_deprecated( victim, STAT_DEX );

		//bonus luck
		if ( number_range( 0, vluck ) > 15 )
		{
			dam = ( dam * 8 ) / 10;
		}
		if ( number_range( 0, vluck ) < 10 )
		{
			dam = ( dam * 103 ) / 100;
		}

		WAIT_STATE( ch, skill_table[ gsn_tail ].beats );

		/* sejw na dexa */
		if ( ch_dex * 2 + number_range( 0, 50 ) < v_dex * 2 + number_range( 0, 35 ) )
		{
			act( "{5Robisz g³êboki zmach swoim ogonem w kierunku $Z, ale dos³ownie w ostatnim momencie twojemu celowi udaje siê unikn±æ ciosu.{x", ch, NULL, vch, TO_CHAR );
			act( "{5$n robi g³êboki zamach swoim ogonem w twoim kierunku, ale dos³ownie w ostatnim momencie udaje ci siê zrobiæ unik i cios przecina tylko powietrze.{x", ch, NULL, vch, TO_VICT );
			act( "{5$n robi g³êboki zamach swoim ogonem w kierunku $Z, jednak wystarcza szybki zwód i cios przecina tylko powietrze, nie czyni±c $X ¿adnej szkody.{x", ch, NULL, vch, TO_NOTVICT );
			WAIT_STATE( ch, skill_table[ gsn_tail ].beats );
			return;
		}

		if ( UMAX( skill, 10 ) > number_percent() || !can_move( vch ) )
		{
			/* przydalby siê warunek jesli ktos jest np:niematerialny*/
			if ( ch->size >= vch->size &&
					( IS_SET( race_table[ GET_RACE( vch ) ].type, ANIMAL ) ||
							IS_SET( race_table[ GET_RACE( vch ) ].type, PERSON ) ) &&
							vch->position >= POS_FIGHTING && vch->hit > 0 )
			{
				//jakis super fatal
				//wtedy szansa taka jaki damage, jesli powyzej 55
				if ( ( number_percent() < 5 + ( skill / 10 ) ) ||
						( dam > 55 && number_percent() < dam ) )
					sit = TRUE;
			}
			//najpierw damage
			WAIT_STATE( vch, PULSE_VIOLENCE );
			damage( ch, vch, dam, gsn_tail, DAM_BASH, TRUE );

			//jesli dead to koniec
			if ( !vch->in_room )
			{
				return;
			}

			//potem rozne efekty
			if ( ( ch->size == SIZE_HUGE || ch->size == SIZE_GIANT ) && chance > 15 && chance <= 30 )
			{

				af.where = TO_AFFECTS;
				af.type	= 304;
				af.level = 20;
				af.location = APPLY_NONE;
				af.duration = 4; 				af.rt_duration = 0;
				af.modifier = 0;
				af.bitvector = &AFF_DAZE;
				affect_to_char( vch, &af, NULL, TRUE	);

				//jaki¶ lepszy tekst (uderzenie + ogluszenie)
				act( "{5Potê¿ny cios $z sprawia, ¿e ból ogarnia ca³e twoje cia³o... Przed oczyma pojawiaj± ci siê czarne plamy. Czujesz, ¿e jeste¶ conajmniej lekko og³uszony!{x", ch, NULL, vch, TO_VICT );
			}
			else if ( ( ch->size == SIZE_HUGE || ch->size == SIZE_GIANT ) && chance > 5 && chance <= 15 )
			{
				//wywal z rooma
				ROOM_INDEX_DATA *location;
				int ex_dir[ MAX_DIR ];
				int dir;
				int counter = 0;
				int door = 0;

				/*najpierw czyscimy tablice wyjsc*/
				for ( dir = 0;dir < MAX_DIR;dir++ )
					ex_dir[ dir ] = -1;

				/*teraz lecimy po exitach i wpisujemy do ex_dir*/
				for ( dir = 0;dir < MAX_DIR;dir++ ) //rellik, czy tu nie powinno byæ dir <= MAX_DIR ? [20080521]
				{
					if ( vch->in_room->exit[ dir ] != NULL
							&& ( ( vch->mount && !IS_SET( vch->in_room->exit[ dir ]->exit_info, EX_NO_MOUNT ) ) || !vch->mount )
							&& ( vch->in_room->exit[ dir ]->u1.to_room != NULL )
							&& can_see_room( vch, vch->in_room->exit[ dir ]->u1.to_room )
							&& !IS_SET( vch->in_room->exit[dir]->exit_info, EX_CLOSED ) )
					{
						ex_dir[ counter++ ] = dir;
					} else {
						continue;
					}
				}

				/*  dobra, teraz w ex_dir powinny byc po kolei wszystkie
				 *  mozliwe exity w ktore moze isc gracz...
				 *  zostalo teraz tylko wylosowac liczbe z zakresu
				 *  0 do counter i isc w tym kierunku
				 */
				dir = door;
				door = ex_dir[ number_range( 0, counter - 1 ) ];

				if ( door == -1 )
					continue;

				location = vch->in_room->exit[ door ]->u1.to_room;

				if ( !location )
					continue;

				//jaki¶ tekst ze go uderza i wywala z lokacji
				char_from_room( vch );
				char_to_room( vch, location );
				act( "{5$N po otrzymaniu potê¿nego ciosu przewraca siê na ziemiê.{x", ch, NULL, vch, TO_CHAR );
				act( "{5Czujesz, ¿e tracisz grunt pod nogami a impet uderzenia sprawia, ¿e odlatujesz gdzie¶ w ty³...{x", ch, NULL, vch, TO_VICT );
				act( "{5Impet mocarnego ciosu sprawia, ¿e $N odlatuje gdzie¶ w ty³...{x", ch, NULL, vch, TO_NOTVICT );
				// oraz zresetowanie pozycji, bo przecie walczy³
				//rellik, jak dosta³ tak ogonem, ¿e odlecia³ to raczej nie ustoi na nogach, zmieniam [20080518]
				vch->position = POS_SITTING;
			}
			else if ( sit && vch->hit > 0 && !IS_AFFECTED(vch, AFF_STABILITY) )
			{
				if ( vch->position > POS_SITTING )
					vch->position = POS_SITTING;
				act( "{5$N po otrzymaniu potê¿nego ciosu przewraca siê na ziemiê.{x", ch, NULL, vch, TO_CHAR );
				act( "{5Potê¿ne uderzenie ogonem $z wywraca ciê na ziemiê.{x", ch, NULL, vch, TO_VICT );
				act( "{5$N po otrzymaniu potê¿nego ciosu ogonem wywraca siê na ziemiê.{x", ch, NULL, vch, TO_NOTVICT );
			}

			check_improve( ch, vch, gsn_tail, TRUE, 60 );
		}
		else
		{

			act( "{5Robisz g³êboki zmach swoim ogonem w kierunku $Z, ale dos³ownie w ostatnim momencie twojemu celowi udaje siê unikn±æ twojego ciosu.{x", ch, NULL, vch, TO_CHAR );
			act( "{5$n robi g³êboki zamach swoim ogonem w twoim kierunku, ale dos³ownie w ostatnim momencie udaje ci siê zrobiæ unik i cios przecina tylko powietrze.{x", ch, NULL, vch, TO_VICT );
			act( "{5$n robi g³êboki zamach swoim ogonem w kierunku $Z, jednak wystarcza szybki zwód i cios przecina tylko powietrze, nie czyni±c $X ¿adnej szkody.{x", ch, NULL, vch, TO_NOTVICT );
			damage( ch, vch, 0, gsn_tail, DAM_BASH, TRUE );
			WAIT_STATE( ch, skill_table[ gsn_tail ].beats );
			check_improve( ch, vch, gsn_tail, FALSE, 40 );
			return;
		}
	}

	if ( self_sit )
	{
		if ( ch->position > POS_SITTING )
			ch->position = POS_SITTING;
		send_to_char( "{5Podczas zamachu ogonem tracisz rownowagê i wywracasz siê.{x\n\r", ch );
		act( "{5$n podczas zamachu ogonem traci rownowagê i przewraca siê.{x", ch, NULL, NULL, TO_ROOM );
	}

}

//je¶li nie chcemy by z summonów sz³o wycinaæ to tutaj
bool is_summon_corpse( int vnum )
{
	if (   vnum == MOB_VNUM_WIEWIORKA
			|| vnum == MOB_VNUM_WOLF
			|| vnum == MOB_VNUM_BEAR
			|| vnum == MOB_VNUM_WIEWIORKA_M
			|| vnum == MOB_VNUM_KRUK
			|| vnum == MOB_VNUM_DZIK
			|| vnum == MOB_VNUM_TYGRYS_M
			|| vnum == MOB_VNUM_WILK_M
			|| vnum == MOB_VNUM_DUSICIEL_M
			|| vnum == MOB_VNUM_ORZEL_M
			|| vnum == MOB_VNUM_BORSUK
			|| vnum == MOB_VNUM_TIGER
			|| vnum == MOB_VNUM_BEAR_M
			|| vnum == MOB_VNUM_PAJAK_M
			|| vnum == MOB_VNUM_OSA
			|| vnum == MOB_VNUM_WAZKA
			|| vnum == MOB_VNUM_ZUK
			|| vnum == MOB_VNUM_GOBLIN
			|| vnum == MOB_VNUM_HOBGOBLIN
			|| vnum == MOB_VNUM_TASLOI
			|| vnum == MOB_VNUM_GRYF
			|| vnum == MOB_VNUM_MANTYKORA
			|| vnum == MOB_VNUM_HARPIA
			|| vnum == MOB_VNUM_SAHUAGIN
			|| vnum == MOB_VNUM_GITHYANKI
			|| vnum == MOB_VNUM_NAGA
			|| vnum == MOB_VNUM_DZINN
			|| vnum == MOB_VNUM_JEDNOROZEC
			|| vnum == MOB_VNUM_HYDRA
			|| vnum == 3014 //jednoro¿ec ale jako koñ
			|| vnum == 39675 //jednoro¿ec w siedzibie klanu
			) return TRUE;
	return FALSE;
}

int find_monster_body_part (  MOB_INDEX_DATA *mob, char *czesc, CHAR_DATA *ch )
{
    /* u¿ywane w do_carve, znajduje numer czê¶ci danej rasy w tabeli body_parts_table */
    int i;

    for ( i = 0; body_parts_table[i].name ; i++ )
    {
        if ( !str_cmp( body_parts_table[i].race , race_table[mob->race].name )
                && !str_prefix( czesc, body_parts_table[i].name ) )
        {
            if ( body_parts_table[i].owner_vnum == mob->vnum
                    || body_parts_table[i].owner_vnum == 0 ) return i;
        }

    }
    return -1;
}

void show_monster_body_part (  MOB_INDEX_DATA *mob, CHAR_DATA *ch )
{
    /* u¿ywane w do_carve, pokazuje czê¶ci danej rasy z tabeli body_parts_table */
    int i;

    print_char( ch, "Z cia³a %s mo¿na próbowaæ wyci±æ: ", mob->name2 );
    for ( i = 0; body_parts_table[i].name ; i++ )
    {
        if ( !str_cmp( body_parts_table[i].race , race_table[mob->race].name )
                && body_parts_table[i].show )
        {
            if ( body_parts_table[i].owner_vnum == mob->vnum
                    || body_parts_table[i].owner_vnum == 0 )
            {
            	print_char( ch, "%s; ", body_parts_table[i].short_desc );
            }
        }
    }
    print_char( ch, "\n\r" );
    return;
}

void carve_do_spec_fun( int cz, CHAR_DATA *ch, OBJ_DATA *corpse, OBJ_DATA *part, MOB_INDEX_DATA  *mob )
{
	AFFECT_DATA 		af;
	int skin_skill, skin_multi;


	skin_skill = 	get_skill( ch, gsn_skin );
    skin_multi =  mob->skin_multiplier;

    part->condition = 50 + (skin_skill/2);

    switch ( cz )
    {
      case 0: //Smocza ³uska
         part->cost =(skin_multi * (25 + number_range(-10, 20) + (skin_skill/4)))/3;
         break;

    	case 1: //kie³ smoka
    		//rellik: komponenty, w³±czamy tê czê¶æ jako komponent
    		part->is_spell_item = TRUE;
    		part->spell_item_counter = 5;
    		part->spell_item_timer = (48*60);
    		part->cost = (skin_multi * (40 + number_range(0, 25) + (skin_skill/2)))/3;
         break;

      case 2: //Smoczy Pazur
    		part->cost = (skin_multi * (38 + number_range(0, 25) + (skin_skill/2)))/3;
    		break;

        case 3: //flaki
            //mo¿na je¶æ
            part->item_type = ITEM_FOOD;
            part->value[0] = 12;
            part->value[1] = 40;
            part->value[3] = number_range(0,1);
            part->liczba_mnoga = TRUE;
            part->wear_flags |= ITEM_HOLD;
            part->cost = 0;
            break;
        case 4: //jajca
            //mozna za³o¿yæ na kark
            part->item_type = ITEM_TRASH;
            part->wear_flags |= ITEM_HOLD;
            part->wear_flags |= ITEM_WEAR_NECK;
            part->liczba_mnoga = TRUE;
            part->cost = 0;
            break;
        case 5: //ucho ogromnego niedzwiedzia
            //mozna za³o¿yæ na kark, daje bark skin ale na bardzo krótko
            add_new_affect( part, 0, APPLY_NONE, 0, 0, &AFF_BARK_SKIN, number_range(3, 6), TRUE );
            part->item_type = ITEM_SKIN;
            part->wear_flags |= ITEM_HOLD;
            part->wear_flags |= ITEM_WEAR_NECK;
            part->cost = (skin_multi * (10 + number_range(-5, 15) + (skin_skill/5)))/25;
            break;
        case 6: //ucho niedzwiedzia
            //mozna za³o¿yæ na kark
            part->item_type = ITEM_SKIN;
            part->wear_flags |= ITEM_HOLD;
            part->wear_flags |= ITEM_WEAR_NECK;
            part->cost = (skin_multi * (5 + number_range(-3, 12) + (skin_skill/5)))/25;
            break;

        case 7: //Ogon Lisa
            part->cost = (skin_multi * (15 + number_range(0, 15) + (skin_skill/3)))/10;
            break;

        case 8: //zajecza lapka
            //mozna wziac do reki, daje luck na dlugo
            add_new_affect( part, 0, APPLY_LUC, 1, 0, &AFF_NONE, 30, TRUE );
            part->item_type = ITEM_TRASH;
            part->wear_flags |= ITEM_HOLD;
            part->cost = (skin_multi * 12)/25;
            break;

        case 9: //serce kolosa
            //mo¿na je¶æ
            part->item_type = ITEM_FOOD;
            part->value[0] = 18;
            part->value[1] = 30;
            part->wear_flags |= ITEM_HOLD;
            part->cost = (skin_multi *18)/25;
            break;
        case 10: //piszczel ogra
            //bron typu mace
            part->item_type = ITEM_WEAPON;
            part->value[0] = WEAPON_MACE;
            part->value[1] = number_range(1,3); //liczba kostek
            part->value[2] = number_range(4,5); //liczba scian kostki
            part->value[3] = 13; //rodzaj zadawanych ran: uderzenie
            part->value[4] = WEAPON_TWO_HANDS; //wlasciwosci specjalne: twohands
            part->value[5] = ( number_percent() > 95 ) ? number_range(0,2) : number_range(-2,0); //bonus do trafienia
            part->value[6] = ( number_percent()  > 70 ) ? number_range(0,1) : number_range(-2,0); //bonus do obrazen
            part->wear_flags |= ITEM_WIELD;
            part->material = 21; //material: kosc
            part->weight = 250; //waga: 11,34 kg
            part->cost = part->value[1] * part->value[2] * 10 + ( part->value[5] + part->value[6] ) * 15;
            part->gender = SEX_FEMALE;
            part->liczba_mnoga = FALSE;
            part->cost = (skin_multi *15)/25;
            break;
        case 11: //rogi demona
            //umagiczniamy
            EXT_SET_BIT( part->extra_flags, ITEM_MAGIC );
    	    	part->cost = (skin_multi * (75 + number_range(10, 25) + (skin_skill/2)))/3;
            break;
        case 12: //szpony gryfa
            //umagiczniamy
            EXT_SET_BIT( part->extra_flags, ITEM_MAGIC );
    	   	part->cost = (skin_multi * (35 + number_range(0, 15) + (skin_skill/2)))/3;
            break;
        case 13: //skrzyd³o nietoperza
    				//rellik: komponenty, w³±czamy tê czê¶æ jako komponent
    				part->is_spell_item = TRUE;
    				part->spell_item_counter = 1;
    				part->spell_item_timer = ( HOURS_PER_YEAR );
               part->cost = (skin_multi * 18)/25;
    				break;
        case 14: //ogon szczura
    				//rellik: komponenty, w³±czamy tê czê¶æ jako komponent
    				part->is_spell_item = TRUE;
    				part->spell_item_counter = 1;
    				part->spell_item_timer = ( HOURS_PER_YEAR );
               part->cost = 1;
    				break;
        case 15: //róg jednoro¿ca
                    //przez nieuwagê mo¿e siê skaleczyæ i dostaje blinda w prezencie (pomys³ by Annor)
                    if ( number_range(1, 10) == 1 )
                    {
                        if ( IS_AFFECTED( ch, AFF_BLIND ) )	return;
                        af.where = TO_AFFECTS;
                        af.type = 5; //blindness
                        af.level = 31;
                        af.location = APPLY_NONE;
                        af.modifier = 0;
                        af.duration = UMAX( 1, 5 ); af.rt_duration = 0;
                        af.bitvector = &AFF_BLIND;
                        affect_to_char( ch, &af, NULL, TRUE );
                        switch ( ch->sex )
                        {
                            case 0:
                                send_to_char( "Zadrasn±³e¶ siê koñcówk± rogu, czujesz jak tracisz wzrok.\n\r", ch );
                                act( "$n zadransnê³o siê koñcówk± rogu i chyba straci³o wzrok.", ch, NULL, NULL, TO_ROOM );
                                break;
                            case 1:
                                send_to_char( "Zadrasn±³e¶ siê koñcówk± rogu, czujesz jak tracisz wzrok.\n\r", ch );
                                act( "$n zadransn±³ siê koñcówk± rogu i chyba straci³ wzrok.", ch, NULL, NULL, TO_ROOM );
                                break;
                            default :
                                send_to_char( "Zadrasnê³a¶ siê koñcówk± rogu, czujesz jak tracisz wzrok.\n\r", ch );
                                act( "$n zadransnê³a siê koñcówk± rogu i chyba straci³a wzrok.", ch, NULL, NULL, TO_ROOM );
                                break;
                        }
                    }
                    /**
                     * spell_item
                     */
                    part->is_spell_item = TRUE;
                    part->spell_item_counter = 1;
                    part->spell_item_timer = ( HOURS_PER_YEAR );
                    /**
                     * weapon
                     */
                    part->item_type = ITEM_WEAPON;
                    part->value[0] = WEAPON_EXOTIC;
                    part->value[1] = number_range(1,2); //liczba kostek
                    part->value[2] = number_range(1,3); //liczba scian kostki
                    part->value[3] = 19;//DAM_ENERGY;
                    part->value[4] = 0;
                    part->value[4] |= WEAPON_VORPAL;
                    part->value[4] |= WEAPON_SHARP;
                    part->value[5] = ( number_percent() > 95 ) ? number_range(0,2) : number_range(-2,0); //bonus do trafienia
                    part->value[6] = ( number_percent()  > 70 ) ? number_range(0,1) : number_range(-2,0); //bonus do obrazen
                    part->wear_flags |= ITEM_WIELD;
                    part->wear_flags |= ITEM_WIELDSECOND;
                    part->material = 21; //material: kosc
                    part->weight = 25;
    	              part->cost = (skin_multi * (125 + number_range(10, 25) + (skin_skill/2)))/3;
                    part->rent_cost = 20 + number_range( 0, 100 );
                    part->gender = SEX_MALE;
                    part->liczba_mnoga = FALSE;
                    break;
         case 16: //k³y wilka
               part->cost = (skin_multi * (20 + number_range(0, 15) + (skin_skill/4)))/5;
               break;

         case 17: //ogon wyverny
               part->cost = (skin_multi * (45 + number_range(0, 20) + (skin_skill/2)))/3;
               break;

         case 18: //pazury niedzwiedzia
               part->cost = (skin_multi * (25 + number_range(0, 15) + (skin_skill/4)))/5;
               break;

         case 19: //K³y tyranozaura!
    	         part->cost = (skin_multi * (150 + number_range(10, 25) + (skin_skill/2)))/3;
               break;

         case 20: //k³y likantropa
               part->cost = (skin_multi * (35 + number_range(0, 20) + (skin_skill/4)))/3;
               break;

			case 21: //sercebehemota
    				//rellik: komponenty, w³±czamy tê czê¶æ jako komponent
    				part->is_spell_item = TRUE;
    				part->spell_item_counter = 3;
    				part->spell_item_timer = ( HOURS_PER_YEAR );
    		      part->cost = (skin_multi * (75 + number_range(0, 25) + (skin_skill/2)))/3;
    				break;

			case 22: //sercesmoka
    				//rellik: komponenty, w³±czamy tê czê¶æ jako komponent
    				part->is_spell_item = TRUE;
    				part->spell_item_counter = 2;
    				part->spell_item_timer = ( HOURS_PER_YEAR );
               part->cost = (skin_multi * (100 + number_range(0, 50) + skin_skill))/3;
    			   break;

         case 23: //poro¿e
            part->cost = (skin_multi * (20 + number_range(0, 15) + (skin_skill/4)))/5;
            break;

         case 24: //Królicza ³apka
            part->wear_flags |= ITEM_HOLD;
            part->cost = (skin_multi * 14)/25;
            break;

         case 25: //trupi palec
            part->item_type = ITEM_TRASH;
            part->wear_flags |= ITEM_HOLD;
            part->is_spell_item = TRUE;
    			part->spell_item_counter = 1;
    			part->spell_item_timer = 6000;
    			part->material = 21;  //ko¶æ
    			EXT_SET_BIT( part->extra_flags, ITEM_EVIL );

         case 26: //szpon ghula
            part->item_type = ITEM_TRASH;
            part->wear_flags |= ITEM_HOLD;
            part->is_spell_item = TRUE;
    			part->spell_item_counter = 1;
    			part->spell_item_timer = 6000;
    			part->material = 21;  //ko¶æ
    			EXT_SET_BIT( part->extra_flags, ITEM_EVIL );

          case 27: //szpon ghasta
            part->item_type = ITEM_TRASH;
            part->wear_flags |= ITEM_HOLD;
            part->is_spell_item = TRUE;
    			part->spell_item_counter = 1;
    			part->spell_item_timer = 6000;
    			part->material = 21;  //ko¶æ
    			EXT_SET_BIT( part->extra_flags, ITEM_EVIL );

          case 28: //czaszka licza

            send_to_char( "W oczodo³ach czaszki na chwilê pojawia siê czerwony ogieñ, po czym ga¶nie.\n\r", ch );
            part->item_type = ITEM_TRASH;
            part->wear_flags |= ITEM_HOLD;
            part->is_spell_item = TRUE;

    			if (mob->level > 30)
    			part->spell_item_counter = number_range(1,2);
    			else
    			part->spell_item_counter = 1;
    			part->spell_item_timer = 6000;
    			part->material = 21;  //ko¶æ
    			EXT_SET_BIT( part->extra_flags, ITEM_EVIL );

          case 29: //K³y wampira
            part->cost = (skin_multi * (45 + number_range(0, 20) + (skin_skill/4)))/3;
            part->wear_flags |= ITEM_HOLD;
            part->is_spell_item = TRUE;
    			part->spell_item_counter = 1;
    			part->spell_item_timer = 6000;
    			part->material = 21;  //ko¶æ
    			EXT_SET_BIT( part->extra_flags, ITEM_EVIL );
    }
}

void do_carve_end(CHAR_DATA *ch, char *argument)
{
    /* funkcja pomocnicza do do_carve */
    OBJ_DATA *obj = NULL, *obj_next;
    MOB_INDEX_DATA *mob;
    char buf[MAX_INPUT_LENGTH];

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
        if ( obj->item_type == ITEM_CORPSE_NPC && obj->value[3] == (int) ch->id )
        {
            mob = get_mob_index(obj->value[0]);
            sprintf(buf, "%s%s%s", "Rozciête cia³o ", mob->name2, " le¿y tutaj powoli gnij±c.");
            free_string(obj->description);
            obj->description = str_dup(buf);
            obj->value[3] = 0;
            break;
        }
    }
}

struct pc_body_parts_type
{
	int race;
	char *part_name;
	char *part_desc;
	char *dop;
	char *cel;
	char *bier;
	char *narze;
	char *miejs;
};

const struct pc_body_parts_type pc_body_parts_table[] =
{
		{ 1, "ucho", "cz³owiecze ucho", "ucha", "uchu", "ucho", "uchem", "uchu" },
		{ 1, "oczy", "cz³owiecze oczy", "oczu", "oczom", "oczy", "oczami", "oczach" },
		{ 2, "ucho", "spiczaste elfie ucho", "ucha", "uchu", "ucho", "uchem", "uchu" },
		{ 3, "broda", "d³uga, poskrêcana krasnoludzka broda", "brody", "brodzie", "brodê", "brod±", "brodzie" },
		{ 0, "palec", "palec", "palca", "palcu", "palec", "palcem", "palcu" },
		{ 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL }
};

//rellik: carve, pokazuje co mo¿na wyci±æ z cia³a gracza
void show_pc_body_part( CHAR_DATA *ch, int race )
{
	int i;
	print_char( ch, "Z tego cia³a mo¿na wyci±æ: " );
	for ( i = 0; pc_body_parts_table[i].part_name; ++i )
	{
		if ( pc_body_parts_table[i].race == race || pc_body_parts_table[i].race == 0 )
		{
			print_char( ch, "%s", pc_body_parts_table[i].part_name );
			if ( pc_body_parts_table[i+1].part_name ) print_char( ch, ", " );
		}
	}
	print_char( ch, "\n\r" );
}

//rellik: carve, znajduje w tabeli czê¶æ do wyciêcia
int find_pc_body_part( int race, char *name )
{
	int i;
	for ( i = 0; pc_body_parts_table[i].part_name; ++i )
	{
		if ( pc_body_parts_table[i].race == race || pc_body_parts_table[i].race == 0 )
		{
			if ( !str_prefix( name, pc_body_parts_table[i].part_name ) )
			{
				return i;
			}
		}
	}
	return -1;
}

void carve_helper_not_enough_skill_message( CHAR_DATA *ch )
{
    switch ( ch->sex )
    {
        case SEX_MALE:
            send_to_char( "Nie jeste¶ wystarczaj±co bieg³y by to zrobiæ.\n\r", ch );
            break;
        case SEX_FEMALE:
            send_to_char( "Nie jeste¶ wystarczaj±co bieg³a by to zrobiæ.\n\r", ch );
            break;
        case SEX_NEUTRAL:
        default:
            send_to_char( "Nie jeste¶ wystarczaj±co bieg³e by to zrobiæ.\n\r", ch );
            break;
    }
}

void carve_pc_corpse( CHAR_DATA *ch, OBJ_DATA *corpse, char *arg1 )
{
	//ok w ->hidden_descr mamy name2, w v6 mamy rasê a v5 czy ju¿ wyciête
	int part;
	OBJ_DATA *trof;
	char buf[MAX_STRING_LENGTH];

	if ( !str_prefix( arg1, "examine" ) )
	{
		//pokazywanie co jest do wyciêcia na podstawie rasy
		show_pc_body_part( ch, corpse->value[6] );
		return;
	}

	print_char( ch, "Pochylasz siê z no¿em nad cia³em szczerz±c zêby we wrednym u¶miechu.\n\r" );
	act( "$n pochyla siê z no¿em nad cia³em szczerz±c zêby we wrednym u¶miechu.", ch, NULL, NULL, TO_ROOM );
	if ( get_skill( ch, gsn_skin ) < 30 )
	{
        carve_helper_not_enough_skill_message( ch );
		return;
	}
	if ( corpse->value[5] != 0 )
	{
		print_char( ch, "Zauwa¿asz, ¿e kto¶ ju¿ wyci±³ z tego cia³a trofeum.\n\r");
		return;
	}

	if ( ( part = find_pc_body_part( corpse->value[6], arg1 ) ) == -1 )
	{
		print_char( ch, "Niestety nie mo¿na z cia³a wyci±æ %s.\n\r", arg1 );
		return;
	}

	corpse->value[5] = 1;

	print_char( ch, "Krótkim szybkim ciêciem wycinasz %s.\n\r", pc_body_parts_table[part].part_desc );
	act( "$n krótkim szybkim ciêciem wycina $t.", ch, pc_body_parts_table[part].part_desc, NULL, TO_ROOM );

  if ( ( trof = create_object( get_obj_index( 6 ), FALSE ) ) == NULL ) //ustawiæ docelowo OBJ_VNUM_BODY_PART
  {
      print_char( ch, "Nic siê nie sta³o.\n\r" );
      DEBUG_INFO( "do_carve: Nie ma obiektu vnum 9 do za³adowania.\n\r" );
      return;
  }

  trof->weight = 1;
  trof->name 	  	= str_dup( pc_body_parts_table[part].part_name );
  trof->name2	  	= str_dup( pc_body_parts_table[part].dop );
  trof->name3 		= str_dup( pc_body_parts_table[part].cel );
  trof->name4 		= str_dup( pc_body_parts_table[part].bier );
  trof->name5 		= str_dup( pc_body_parts_table[part].narze );
  trof->name6 		= str_dup( pc_body_parts_table[part].miejs );
  trof->item_description = str_dup( pc_body_parts_table[part].part_desc );
  trof->description = str_dup( pc_body_parts_table[part].part_desc );
  sprintf( buf, "%s %s", pc_body_parts_table[part].part_name, corpse->hidden_description );
  trof->short_descr = str_dup( buf );
  trof->cost = 1;
  trof->item_type = ITEM_TRASH;
  trof->wear_flags |= ITEM_HOLD;
  trof->wear_flags |= ITEM_WEAR_NECK;
  trof->timer = 180;

  //powinien gdzie¶ to tam zwalniaæ ale dla pewno¶ci sprz±tam po sobie
  if ( corpse->hidden_description )  free( corpse->hidden_description );
  corpse->hidden_description = &str_empty[0];

  obj_to_char( trof, ch );
  WAIT_STATE( ch, 8 );

	return;
}


void do_carve( CHAR_DATA *ch, char *argument )
{
    /* wycinanie czê¶ci zw³ok */
    OBJ_DATA *weapon;
    OBJ_DATA *corpse;
    OBJ_DATA *part;
    MOB_INDEX_DATA *mob;
    char arg[ MAX_INPUT_LENGTH ];
    char arg1[ MAX_INPUT_LENGTH ];
    char buf[ MAX_INPUT_LENGTH ];
    char buf1[ MAX_INPUT_LENGTH ];
    int czesc, skin_skill, delay, time_left;
    int skn_state, skin_potentially, max_learn, penalty;
    argument = one_argument( argument, arg ); //czyli arg = z czego wycinac
    one_argument( argument, arg1 ); // a arg1 co wycinac

    if ( arg[0] == '\0' )
    {
        send_to_char( "Z czego chcesz wycinaæ?\n\r", ch );
        return;
    }
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Co by¶ chcia³ wyci±æ?\n\r", ch );
        return;
    }

    //spr czy ma odpowiednie warunki fizjologiczne nasz oprawca
    if ( IS_AFFECTED( ch, AFF_RAZORBLADED ) )
    {
        send_to_char( "Przemienione w ostrza d³onie uniemo¿liwiaj± ci wykonanie tej czynno¶ci.\n\r", ch );
        return;
    }
    if ( IS_AFFECTED( ch, AFF_BEAST_CLAWS ) )
    {
        send_to_char( "Przemienione w szpony d³onie uniemo¿liwiaj± ci wykonanie tej czynno¶ci.\n\r", ch );
        return;
    }
    //czy ma skill wyæwiczony
    if ( ( skin_skill = get_skill( ch, gsn_skin ) ) <= 0 )
    {
        send_to_char( "Nie za bardzo wiesz jak siê do tego zabraæ.\n\r", ch );
        return;
    }
    //czy mo¿e siedzi na koniu
    if ( ch->mount )
    {
        send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
        return;
    }
    //albo jest w wodzie
    if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_WATER ) )
    {
        send_to_char( "Nie dasz rady tutaj ¶ci±gaæ skóry!\n\r", ch );
        return;
    }

    //czy ma noz i wolna reke
    if ( get_hand_slots( ch, WEAR_WIELD ) > 0 )	/*znaczy ze ma cos jeszcze w ³apce oprócz wielda ( nie jest dla mnie jasne czym ró¿ni siê WEAR_SECOND od WEAR_HOLD */
    {
        send_to_char( "Do tego potrzebujesz jednej rêki wolnej i odpowiedniego narzêdzia.\n\r", ch );
        return;
    }
    //sprawdzamy je¶li to nie sztyletonó¿
    weapon = get_eq_char( ch, WEAR_WIELD );
    if ( ( !weapon ) || ( weapon->value[0] != WEAPON_DAGGER ) )
    {
        send_to_char("Przyda³by siê do tego jaki¶ nó¿ albo w ostateczno¶ci sztylet.\n\r", ch );
        return;
    }
    //sprawdzamy czy jest cia³o
    if ( ( corpse = get_obj_list( ch, arg, ch->in_room->contents ) ) == NULL )
    {
        send_to_char( "Nie widzisz tu nic takiego.\n\r", ch );
        return;
    }

    //sprawdzamy czy cia³o jest od pc
    //specjalna funkcja do wycinania z cia³ gracza bo trochê to inaczej wygl±da
    if ( corpse->item_type == ITEM_CORPSE_PC )
    {
    	carve_pc_corpse( ch, corpse, arg1 );
    	return;
    }

    //sprawdzamy czy jest to cia³o NPC
    if ( corpse->item_type != ITEM_CORPSE_NPC )
    {
        print_char( ch, "Z tego to ty chyba nie dasz rady nic wyci±æ.\n\r" );
        return;
    }
    skn_state = corpse->value[4];
    //sprawdzamy czy kto¶ w³a¶nie oprawia cia³o wtedy musi poczekaæ na swoj± kolej (1-w trakcie, 2-oskinowane)
    if ( skn_state == 1 )
    {
        print_char( ch, "Kto¶ w³a¶nie oprawia to cia³o, poczekaj na swoj¶ kolej.\n\r" );
        return;
    }
    //ustalamy do jakiego moba nale¿a³o cia³o
   	if ( corpse->value[0] <= 0 || ( mob = get_mob_index( corpse->value[0] ) ) == NULL )
    {
        print_char( ch, "Nic siê nie sta³o.\n\r" );
        DEBUG_INFO( "do_carve: Nie znalazlo moba ktorego cialo lezy." );
        return;
    }

    //sprawdzamy czy nie jest to cia³o summona
   	if ( is_summon_corpse( corpse->value[0] ) )
   	{
  		print_char( ch, "Nie mo¿na wycinaæ nic z przywo³ywanych stworzeñ.\n\r" );
   		return;
  	}

    //je¶li examine to pokazujemy co jest do wyciêcia
   	if ( !str_prefix( arg1, "examine" ))
   	{
 			show_monster_body_part( mob, ch );
   		return;
   	}

    //sprawdzamy czy jest taka czesc ogolnie
 		if ( ( czesc = find_monster_body_part ( mob, arg1, ch ) ) == -1 ) //funkcja przeszukuje tabelê monster_part_table i probuje znale¼æ part o nazwie arg1 pod warunkiem rasy .name
 		{
 			print_char( ch, "Nie mo¿esz wyci±æ %s z %s.\n\r", arg1, corpse->name2 );
      return;
 		}

        penalty = 10;
    //mag raczej nie szweda sie z nozem po lesie tylko studiouje w zaciszu laboratorium, kler to samo, paladyn ma swieta wojne, czarny rycarz woli swoje podziemia, zlodziej czeka w zaulku miedzy knajpa a smietnikiem gdzie sie goscie odlewaja, a druid kocha zwierz±tka i krzywdy im nie zrobi, te co zostaja moga wycinac
    //za to cooking i alchemy inne bed± mia³y tak ¿eby uzale¿niæ jednych od drugich
    penalty -= get_curr_stat( ch, STAT_DEX ) / 20; //zrêczno¶æ przydaje siê przy precyzyjnym operowaniu
    penalty -= get_curr_stat( ch, STAT_WIS ) / 30; //wiedza pomaga gdy wie sie jak co¶ przyci±æ ¿eby by³o dobrze
    penalty -= get_curr_stat( ch, STAT_STR ) / 40; // sila tez ma znaczenie

   switch ( ch->class )
    {
        case CLASS_BLACK_KNIGHT:
        case CLASS_PALADIN:
        case CLASS_WARRIOR:
            penalty += skin_skill/8;
            break;
        case CLASS_MONK:
        case CLASS_THIEF:
        case CLASS_MAG:
            penalty += skin_skill/6;
            break;
        case CLASS_CLERIC:
            penalty += skin_skill/4;
            break;
        case CLASS_BARD:
        case CLASS_DRUID:
            penalty += skin_skill/3;
            break;
        case CLASS_BARBARIAN:
            //trapper jest klasowa umiejetnoscia barbarzyncow
            break;
        default:
            break;
    }
    //czy wystarczaj±ce umiejêtn¶ci aby to zrobiæ
    if ( skin_skill < ( body_parts_table[czesc].difficult + penalty ) )
    {
        carve_helper_not_enough_skill_message( ch );
        return;
    }

    //oraz czy nie zostala wycieta ju¿
    if ( IS_SET( body_parts_table[czesc].czesc, corpse->wyciete ) )
    {
        print_char( ch, "%s jest ju¿ %s z %s.\n\r", capitalize_first( body_parts_table[czesc].name4 ), body_parts_table[czesc].gender == SEX_MALE ? "wyciêty" : body_parts_table[czesc].gender == SEX_FEMALE ? "wyciêta": "wyciête", corpse->name2 );
        return;
    }

    //sprawdzmy czy skora nie jest sciagnieta i nie koliduje to z czescia
    if ( ( body_parts_table[czesc].skn_destroyed ) && skn_state == 2 )
    {
        print_char( ch, "Niestety przy ¶ci±ganiu skóry zosta³a zniszczona równie¿ czê¶æ, któr± chcia³by¶ wyci±æ.\n\r" );
        return;
    }

    if ( ( part = create_object( get_obj_index( 6 ), FALSE ) ) == NULL ) //ustawiæ docelowo OBJ_VNUM_BODY_PART
    {
        print_char( ch, "Nic siê nie sta³o.\n\r" );
        DEBUG_INFO( "do_carve: Nie ma obiektu vnum 9 do za³adowania.\n\r" );
        return;
    }

    //wage ustawiam statycznie chyba nie ma sensu sie bawic bo nie da sie na podstawie corpsa
    part->weight = 2;
    //name
    free_string( part->name );
    part->name = strdup( body_parts_table[czesc].name );
    free_string( part->name2 );
    part->name2 = strdup( body_parts_table[czesc].name2 );
    free_string( part->name3 );
    part->name3 = strdup( body_parts_table[czesc].name3 );
    free_string( part->name4 );
    part->name4 = strdup( body_parts_table[czesc].name4 );
    free_string( part->name5 );
    part->name5 = strdup( body_parts_table[czesc].name5 );
    free_string( part->name6 );
    part->name6 = strdup( body_parts_table[czesc].name6 );

    //short Wyciêta ³uska ze smoka
    free_string( part->short_descr );
    if ( body_parts_table[czesc].short_desc )
    {
        part->short_descr =	strdup( body_parts_table[czesc].short_desc );
    }
    else
    {
        sprintf( buf, "%s %s%s{x %s %s", body_parts_table[czesc].gender == SEX_MALE ? "Wyciêty" : body_parts_table[czesc].gender == SEX_FEMALE ? "Wyciêta": "Wyciête", body_parts_table[czesc].color, part->name, ( part->name[0] == 'z' || part->name[0] == 's' ) ? "ze" : "z",race_table[mob->race].name2 );
        part->short_descr = strdup( buf );
    }

    //long £uska, która zosta³a wyciêta ze smoka
    free_string( part->description );
    if ( body_parts_table[czesc].long_desc )
    {
        part->description =	strdup( body_parts_table[czesc].long_desc );
    }
    else
    {
        sprintf( buf, "%s%s{x %s pewnie %s %s.", body_parts_table[czesc].color, capitalize_first( part->name ), body_parts_table[czesc].gender == SEX_MALE ? "wyciêty" : body_parts_table[czesc].gender == SEX_FEMALE ? "wyciêta": "wyciête", ( part->name[0] == 'z' || part->name[0] == 's' ) ? "ze" : "z", race_table[mob->race].name2 );
        part->description = strdup( buf );
    }

    //description "Czê¶æ cia³a, ktora wygl±da jak ³uska, wyciêta zapewne z jakiego¶ smoka"
    free_string( part->item_description );
    if ( body_parts_table[czesc].description )
    {
        part->description =	strdup( body_parts_table[czesc].description );
    }
    else
    {
        sprintf( buf, "Czê¶æ cia³a, która wygl±da jak %s%s{x, %s zapewne z %s %s.", body_parts_table[czesc].color, capitalize_first( part->name ),		body_parts_table[czesc].gender == SEX_MALE ? "wyciêty" : body_parts_table[czesc].gender == SEX_FEMALE ? "wyciêta": "wyciête", mob->sex == SEX_MALE ? "jakiego¶" : "jakiej¶", race_table[mob->race].name2 );
        part->item_description = strdup( buf );
    }

    //v0 przechowuje index tabeli body_parts_table z jakiego czesc pochodzi
    part->value[0] = czesc;
    //koszt czesci koszt bazowy +- max 20%
    part->cost = body_parts_table[czesc].cost + (2 * number_range(0,1) - 1 ) * body_parts_table[czesc].cost * number_range(0, 20)/100 ;
    //ustawienie w ciele bitu, czê¶ci wyciêtej
    SET_BIT(corpse->wyciete, body_parts_table[czesc].czesc);
    //ustawiamy flagê, ¿e kto¶ w³a¶nie oprawia cia³o
    corpse->value[4] = 1; //zapamiêtaæ jescze tutaj jaki value by³
    part->item_type = ITEM_SKIN; //powinno byc ITEM_MONSTER_PART ale nie jest zeby sklepikarz kupowal jak skore
    part->wear_flags = ITEM_TAKE;
    part->value[3] = 0; //jesli nie zero to 'wyglda dosc podejrzanie' czyli zepsuta
    if ( body_parts_table[czesc].has_funct ) carve_do_spec_fun( czesc, ch, corpse, part, mob );

    //ustalamy czas wycinania
    delay = URANGE(10,( 120 - skin_skill ) / 5 - ( get_skill( ch, gsn_dagger) / 25 ) + number_range(0,5), 30);
    if ( skin_skill == 100 )
    {
        delay /= 2;
    }

    //tutaj komunikat, ¿e zaczynamy wycinaæ
    switch ( skin_skill / 20 )
    {
        case 0:
            switch ( number_range(1, 3) )
            {
                case 1:
                    send_to_char( "My¶l o ciêciu tego cia³a napawa ciê obrzydzeniem, dlatego nie mo¿esz siê skupiæ.\n\r", ch );
                    act( "Na twarzy $z pojawia siê obrzydzenie, wyra¼nie nie mo¿e siê skupiæ.\n\r", ch, NULL, NULL, TO_ROOM );
                    break;

                case 2:
                    send_to_char( "Pochylasz siê na cia³em. Rêce ci dr¿±, po plecach przechodz± dreszcze. Nie wierzysz, ¿e uda ci siê wyci±æ choæby kawa³ek.\n\r", ch );
                    sprintf( buf, "%s pochyla siê nad cia³em. Rêce %s dr¿±, po plecach przechodz± dreszcze. Nie wierzysz, ¿e uda %s siê wyci±æ choæby kawa³ek.\n\r", capitalize_first( ch->name ), ch->sex == SEX_FEMALE ? "jej" : "mu" , ch->sex == SEX_FEMALE ? "jej" : "mu" );
                    act( buf , ch, NULL, NULL, TO_ROOM );
                    break;

                case 3:
                    send_to_char( "Nie masz pojêcia jak zabraæ siê za t± robotê, dlatego twoje pierwsze ciêcie jest tak nieudolne.\n\r", ch );
                    sprintf( buf, "%s nie ma pojêcia jak zabraæ siê za t± robotê, dlatego %s pirwsze ciêcie jest tak nieudolne.\n\r", capitalize_first( ch->name ), ch->sex == SEX_FEMALE ? "jej" : "jego" );
                    act( buf , ch, NULL, NULL, TO_ROOM );
                    break;
            }
            break;

        case 1:
            switch ( number_range(1, 2) )
            {
                case 1:
                    send_to_char( "Wydaje ci siê, ¿e wiesz jak zacz±æ, jednak lekkie dr¿enie rêki i brak do¶wiadczenia sprawiaj±, ¿e tniesz zupe³nie nie tam gdzie zamierza³e¶.\n\r", ch );
                    sprintf( buf, "Wydaje siê, ¿e %s wie jak zacz±æ, jednak lekkie dr¿enie rêki i brak do¶wiadczenia sprawiaj±, ¿e tnie zupe³nie nie tam gdzie trzeba.\n\r", capitalize_first( ch->name ) );
                    act( buf, ch, NULL, NULL, TO_ROOM );
                    break;

                case 2:
                    send_to_char( "Na sam± my¶l o wnêtrzno±ciach, które zaraz bêdziesz rozcina³ robi ci siê niedobrze.\n\r", ch );
                    sprintf( buf, "Na twarzy %s maluje siê grymas niezadowolenia na my¶l o czekaj±cej %s robocie.\n\r", capitalize_first( ch->name ), ch->sex == SEX_FEMALE ? "j±" : "go" );
                    act( buf, ch, NULL, NULL, TO_ROOM );
                    break;
            }
            break;

        case 2:
            switch ( number_range(1, 2) )
            {
                case 1:
                    send_to_char( "Rozcinasz skórê i twoim oczom ukazuj± siê wnêtrzno¶ci, na których widok mdli ciê.\n\r", ch );
                    sprintf( buf, "%s chyba zaraz zwymiotuje ale dzielnie zabiera siê do roboty.\n\r", capitalize_first( ch->name ) );
                    act( buf, ch, NULL, NULL, TO_ROOM );
                    break;

                case 2:
                    send_to_char( "Podchodzisz do cia³a dobrze wiedz±c co masz zrobiæ, jednak je¶li chodzi o wykonanie to jeszcze bardzo daleko ci do idea³u.\n\r", ch );
                    sprintf( buf, "%s podchodzi do cia³a, wydaje siê, ¿e wie co robi jednak dostrzegasz na %s twarzy cieñ zawahania.\n\r", capitalize_first( ch->name ), ch->sex == SEX_FEMALE ? "j±j" : "jego" );
                    act( buf, ch, NULL, NULL, TO_ROOM );
                    break;
            }
            break;

        case 3:
            switch ( number_range(1, 2) )
            {
                case 1:
                    send_to_char( "Ch³odno spogl±dasz na cia³o i nakre¶lasz na nim kilka linii pomocniczych, po czym przystêpujesz do dok³adnego wycinania.\n\r", ch );
                    sprintf( buf, "%s ch³odno spogl±da na cia³o i nakre¶la na nim kilka linii pomocniczych, po czym przystêpuje do dok³adnego wycinania.\n\r", capitalize_first( ch->name ) );
                    act( buf, ch, NULL, NULL, TO_ROOM );
                    break;

                case 2:
                    send_to_char( "Ostro¿nie przygl±dasz siê cia³u ze wszystkich stron, aby wybraæ najlepsze miejsce, do rozpoczêcia ciêæ.\n\r", ch );
                    sprintf( buf, "%s ostro¿nie przygl±da siê cia³u ze wszystkich stron, aby wybraæ najlepsze miejsce, do rozpoczêcia pierwszego ciêcia.\n\r", capitalize_first( ch->name ) );
                    act( buf, ch, NULL, NULL, TO_ROOM );
                    break;
            }
            break;

        case 4:
            switch ( number_range(1, 2) )
            {
                case 1:
                    sprintf( buf, "B³yskawicznie oceniasz cia³o i pewn%s siebie przystêpujesz do pierwszego ciêcia.\n\r", ch->sex == SEX_FEMALE ? "a" : "y" );
                    send_to_char( buf , ch );
                    sprintf( buf, "%s b³yskawicznie ocenia cia³o i pewn%s siebie przystêpuje do pierwszego ciêcia.\n\r", capitalize_first( ch->name ), ch->sex == SEX_FEMALE ? "a" : "y" );
                    act( buf, ch, NULL, NULL, TO_ROOM );
                    break;

                case 2:
                    send_to_char( "Jedno spojrzenie wystarcza ci, aby od razu przej¶æ do pierwszego ciêcia.\n\r", ch );
                    sprintf( buf, "Jedno spojrzenie wystarcza %s aby od razu przej¶æ do pierwszego ciêcia.\n\r", capitalize_first( ch->name3 ) );
                    act( buf, ch, NULL, NULL, TO_ROOM );
                    break;
            }
            break;

        default:
            send_to_char( "Pochylasz siê nad cia³em zastanawiaj±c siê gdzie wykonaæ pierwsze ciêcie.\n\r", ch );
            act( "$n pochyla siê nad cia³em zastanawia±c siê gdzie wykonaæ pierwsze ciêcie.\n\r", ch, NULL, NULL, TO_ROOM );
            break;
    }

    if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED( ch, AFF_FLOAT ))
    {
        if ( skill_table[ gsn_fly ].msg_off )
        {
            send_to_char( skill_table[ gsn_fly ].msg_off, ch );
            send_to_char( "\n\r", ch );
        }
        affect_strip( ch, gsn_fly );
        affect_strip( ch, gsn_float );
    }
    strip_invis( ch, TRUE, TRUE );
    if ( IS_AFFECTED( ch, AFF_HIDE ) )
    {
        affect_strip( ch, gsn_hide );
    }

    ch->position = POS_STANDING;

    //pêtelka w ktorej ³adujemy komunikaty które bêda pokazywaæ siê w czasie wycinania
    for ( time_left = delay; time_left > 1 ; time_left-- )
    {
        if ( number_percent() < 30 )
        {
            switch ( skin_skill / 20 )
            {
                case 0:
                    switch ( number_range(1,5) )
                    {
                        case 1:
                            sprintf( buf, "%s mimowolnie wylatuje ci z d³oni. Speszony szybko %s podnosisz.\n\r", weapon->name, weapon->gender == SEX_FEMALE ? "j±" : weapon->gender == SEX_MALE ? "go" : "je" );
                            sprintf( buf1, "%s mimowolnie upuszcza %s. Speszon%s szybko %s podnosi.\n\r", ch->name, weapon->name, ch->sex == SEX_FEMALE ? "a" : ch->sex == SEX_MALE ? "y" : "e", weapon->gender == SEX_FEMALE ? "j±" : weapon->gender == SEX_MALE ? "go" : "je" );
                            break;
                        case 2:
                            sprintf( buf, "Twoje nieumiejêtne ciêcie natrafia na têtnicê, z której wycieka krew. Niedobrze ci.\n\r" );
                            sprintf( buf1, "Nieumiejêtne ciêcie %s natrafia na têtnicê, z której wycieka krew. Niedobrze ci.\n\r", ch->name );
                            break;
                        case 3:
                            sprintf( buf, "Przerywasz ciêcie, zupe³nie nie wiedz±c co dalej nale¿y zrobiæ.\n\r" );
                            sprintf( buf1, "%s przerywa ciêcie, zupe³nie nie wiedz±c co dalej nale¿y zrobiæ.\n\r", ch->name );
                            break;
                        case 4:
                            sprintf( buf, "Mylisz siê i rozcinasz nie to co nale¿a³o.\n\r" );
                            sprintf( buf1, "%s mysli siê i rozcina nie to co nale¿a³o.\n\r", ch->name );
                            break;
                        case 5:
                            sprintf( buf, "Z rozpacz± patrzysz na dotychczasowe efekty swojej pracy. To jest do niczego!\n\r" );
                            sprintf( buf1, "%s z dezaprobat± patrzy na dotychczasowe efekty swojej pracy.\n\r", ch->name );
                            break;
                    }	break;
                case 1:
                    switch ( number_range(1,5) )
                    {
                        case 1:
                            sprintf( buf, "Próbujesz nieudolnie naci±æ kawa³ek cia³a, jednak tylko je niszczysz.\n\r" );
                            sprintf( buf1, "%s próbuje nieudolnie naci±æ kawa³ek cia³a, jednak tylko je niszczy.\n\r", ch->name );
                            break;
                        case 2:
                            sprintf( buf, "Przez nieuwagê przecinasz zupe³nie co¶ innego ni¿ nale¿a³o.\n\r" );
                            sprintf( buf1, "Przez nieuwagê %s przecina zupe³nie co¶ innego ni¿ nale¿a³o.\n\r", ch->name );
                            break;
                        case 3:
                            sprintf( buf, "Zbyt mocno ¶ciskasz %s i wykonujesz o wiele za g³êbokie ciêcie.\n\r", weapon->name );
                            sprintf( buf1, "%s zbyt mocno ¶ciska %s i wykonuje o wiele za g³êbokie ciêcie.\n\r", ch->name, weapon->name );
                            break;
                        case 4:
                            sprintf( buf, "Odciêcie tej cz±stki cia³a przychodzi ci z wielkimi trudno¶ciami.\n\r" );
                            sprintf( buf1, "Odciêcie tej cz±stki cia³a przychodzi %s z wielkimi trudno¶ciami.\n\r", ch->name3 );
                            break;
                        case 5:
                            sprintf( buf, "Denerwujesz siê, ¿e nigdy nie nauczysz siê porz±dnie wycinaæ czê¶ci z cia³.\n\r" );
                            sprintf( buf1, "%s denerwuje siê, ¿e nigdy siê nie nauczy porz±dnie wycinaæ czê¶ci z cia³.\n\r", ch->name );
                            break;
                    }	break;

                case 2:
                    switch ( number_range(1,5) )
                    {
                        case 1:
                            sprintf( buf, "Raz z gorszym, raz z lepszym efektem starasz siê równo prowadziæ ostrze.\n\r" );
                            sprintf( buf1, "%s usi³uje równo prowadziæ ostrze, z ró¿nym jednak efektem.\n\r", ch->name );
                            break;
                        case 2:
                            sprintf( buf, "Trochê trudno¶ci sprawia ci odciêcie kolejnego kawa³ka cia³a.\n\r" );
                            sprintf( buf1, "Odciêcie kolejnego kawa³ka cia³a sprawia %s trochê trudno¶ci.\n\r", ch->name3 );
                            break;
                        case 3:
                            sprintf( buf, "Zarówno precyzja jak i szybko¶æ twoich ciêæ pozostawij± wiele do ¿yczenia.\n\r" );
                            sprintf( buf1, "Zarówno precyzja jak i szybko¶æ ciêæ %s pozostawiaj± wiele do ¿yczenia.\n\r", ch->name2 );
                            break;
                        case 4:
                            sprintf( buf, "Rozcinasz fragment cia³a nie maj±c do koñca pewno¶ci, czy tak to nale¿a³o zrobiæ.\n\r" );
                            sprintf( buf1, "%s rozcina fragmet cia³a ale widaæ, ¿e nie ma pewno¶ci, czy tak to nale¿alo zrobiæ.\n\r", ch->name );
                            break;
                        case 5:
                            sprintf( buf, "Oceniasz, ¿e wycinany fragment nie wygl±da a¿ tak ¼le.\n\r" );
                            sprintf( buf1, "%s patrzy na wycinany fragmet jakby oceniaj±c go.\n\r", ch->name );
                            break;
                    }	break;

                case 3:
                    switch ( number_range(1,5) )
                    {
                        case 1:
                            sprintf( buf, "Bezb³êdnie nacinasz cia³o w odpowiednich miejscach, jednak jeszcze trochê brakuje ci do perfekcji.\n\r" );
                            sprintf( buf1, "%s bezb³êdnie nacina cia³o w odpowiednich miejscach, jednak jeszcze trochê brakuje %s do perfekcji.\n\r", ch->name, ch->sex == SEX_FEMALE ? "jej" : "mu" );
                            break;
                        case 2:
                            sprintf( buf, "Powoli i dok³adnie starasz siê naci±æ cia³o, jednak wysz³o nieco krzywo.\n\r" );
                            sprintf( buf1, "%s stara siê powoli i dok³adnie naci±æ cia³o lecz wysz³o to nieco krzywo.\n\r", ch->name );
                            break;
                        case 3:
                            sprintf( buf, "Krytycznie spogl±dasz na efekt swojej dotychczasowej pracy i zauwa¿asz kilka niedoci±gniêæ.\n\r" );
                            sprintf( buf1, "%s krytycznie spogl±da na efekt swojej dotychczasowej pracy, trudno powiedzieæ czy jest zadowoln%s.\n\r", ch->name, ch->sex == SEX_FEMALE ? "a" : ch->sex == SEX_MALE ? "y" : "e" );
                            break;
                        case 4:
                            sprintf( buf, "Trochê zbyt szybkie ciêcie nieco schodzi z wyznaczonej linii pomocniczej.\n\r" );
                            sprintf( buf1, "Widzisz jak trochê zbyt szybkie ciêcie %s schodzi nieco z wyznaczonej linii pomocniczej.\n\r", ch->name2 );
                            break;
                        case 5:
                            sprintf( buf, "Z niewielkimi trudno¶ciami oddzielasz kolejny kawa³ek cia³a.\n\r" );
                            sprintf( buf1, "%s z niewielkimi trudno¶ciami oddziela kolejny kawa³ek cia³a.\n\r", ch->name );
                            break;
                    }	break;

                case 4:
                    switch ( number_range(1,5) )
                    {
                        case 1:
                            sprintf( buf, "Szybko i nies³ychanie precyzyjnie wykonujesz kolejne ciêcia.\n\r" );
                            sprintf( buf1, "%s szybko i nies³ychanie precyzyjnie wykonuje kolejne ciêcia.\n\r", ch->name );
                            break;
                        case 2:
                            sprintf( buf, "Twoja rêka nawet nie drgnie podczas wykonywania kolejnych ciêæ.\n\r" );
                            sprintf( buf1, "Rêka %s nawet nie drgnie podczas wykonywania kolejnych ciêæ.\n\r", ch->name2 );
                            break;
                        case 3:
                            sprintf( buf, "Z chirurgiczn± precyzj± oddzielasz kolejny kawa³ek cia³a.\n\r" );
                            sprintf( buf1, "%s z chirurgiczn± precyzj± oddziela kolejny kawa³ek cia³a.\n\r", ch->name );
                            break;
                        case 4:
                            sprintf( buf, "Oszczêdnymi ruchami, metodycznie wycinasz czê¶æ z cia³a.\n\r" );
                            sprintf( buf1, "%s oszczêdnymi ruchami, metodycznie wycina czê¶æ z cia³a.\n\r", ch->name );
                            break;
                        case 5:
                            sprintf( buf, "Z ³atwo¶ci± wykonujesz kolejne ciêcia.\n\r" );
                            sprintf( buf1, "%s z ³atwo¶ci± wykonuje kolejne ciêcia.\n\r", ch->name );
                            break;
                    }	break;

                default:
                    sprintf( buf, "Cierpliwie wycinasz czê¶æ z cia³a.\n\r" );
                    sprintf( buf1, "%s %s", ch->name, "cierpliwie wycina co¶ z cia³a.\n\r" );
                    break;
            }
            wait_printf( ch, delay - time_left, buf );
            wait_act( delay - time_left, buf1, ch, NULL, NULL, TO_ROOM );
            continue;
        }
        //zaci±³ siê no¿em biedaczek
        if ( ( number_range( 1, 70 ) - skin_skill > 0 ) && ( number_percent() < 30 ) )
        {
            wait_printf( ch, delay - time_left, "Zacinasz siê przy ¶ci±ganiu skóry w palec.\n\r" );
            sprintf( buf, "%s %s", ch->name, "zacina siê przy ¶ci±ganiu skóry w palec.\n\r" );
            wait_act( delay - time_left, buf, ch, NULL, NULL, TO_ROOM );
            ch->hit = UMAX( ch->hit - number_range( 2, 3 ), 1 );
        }
    }
    //ustawiamy naszemu traperowi czas oczekiwania
    WAIT_STATE( ch, delay * 4 + 10);
    //zaw³aszczam v3 na to zeby powiazac corpse ze skinowaczem
    corpse->value[3] = (int) ch->id; //my¶lê, ¿e dopuszczalne
    //tutaj mówimy ¿e co¶ tam wyci±gn±³
    //w zale¿no¶ci od szansy mo¿e zepsuæ
    if ( number_percent() > body_parts_table[czesc].chance && !IS_IMMORTAL( ch ) )
    {
        switch ( skin_skill / 20 )
        {
            case 0:
                sprintf( buf, "Tak jak my¶la³%s, co¶ wyci%s ale nie bardzo wiadomo co to by³o.\n\r", ch->sex == SEX_FEMALE ? "a¶" : "e¶", ch->sex == SEX_FEMALE ? "ê³a¶" : "±³e¶" );
                sprintf( buf1, "Tak ci siê te¿ zdawa³o, ¿e ta sztuczka %s nie wyjdzie.\n\r", ch->name3 );
                break;
            case 1:
                sprintf( buf, "Co¶ tam prawie uda³o ci siê wyci±æ, prawie...\n\r" );
                sprintf( buf1, "%s prawie uda³o siê co¶ wyci±æ.\n\r", ch->name3 );
                break;
            case 2:
                sprintf( buf, "Jeste¶ coraz bieglejsz%s w tej sztuce, tym razem jednak siê nie uda³o.\n\r", ch->sex == SEX_FEMALE ? "a" : "y" );
                sprintf( buf1, "Widaæ, ¿e %s stara%s siê jak m%s, tym razem jednak na pró¿no.\n\r", ch->name, ch->sex == SEX_FEMALE ? "³a" : "³", ch->sex == SEX_FEMALE ? "g³a" : "óg³" );
                break;
            case 3:
                sprintf( buf, "Nie¼le, prawie ci siê uda³o.\n\r" );
                sprintf( buf1, "%s prawie uda³o siê co¶ wyci±æ.\n\r", ch->name3 );
                break;
            case 4:
                sprintf( buf, "Dobrze, nawet bardzo dobrze, tym razem jednak nie do¶æ dobrze.\n\r" );
                sprintf( buf1, "Pomimo ca³ego kunsztu i serca jakie w t± pracê w³o¿y%s %s, nie uda³o %s siê wyci±æ czê¶ci.\n\r", ch->sex == SEX_FEMALE ? "³a" : "³", ch->name, ch->sex == SEX_FEMALE ? "jej" : "mu" );
                break;
            case 5:
                sprintf( buf, "Có¿... nawet takiemu mistrzowi jak ty mo¿e czasem nie wyj¶æ.\n\r" );
                sprintf( buf1, "Widaæ, ¿e pomimo, i¿ %s niew±tpliwie zna siê na rzeczy, to jednak tym razem siê nie uda³o.\n\r", ch->name );
                break;
        }
        wait_printf( ch, delay, buf );
        wait_act( delay, buf1, ch, NULL, NULL, TO_ROOM );
    }
    else
    {
        sprintf( buf, "%s", "Powoli wyci±gasz wyciêt± czê¶æ.\n\r" );
        wait_printf( ch, delay, buf );
        sprintf( buf, "%s %s", ch->name, "powoli wyci±ga wyciêt± czê¶æ.\n\r" );
        wait_act( delay, buf, ch, NULL, NULL, TO_ROOM );
        wait_obj_to_char( part, delay, ch );
    }
    if ( body_parts_table[czesc].skn_destroyed )
    {
        corpse->value[4] = 2;
    }
    else
    {
        corpse->value[4] = skn_state;
    }
    //tutaj wywo³ujemy  na koniec funkcjê która zakañcza wycinanie
    wait_function( ch, delay, &do_carve_end, "" );
    //jeszcze sprawdzamy czy moze sie czegos nauczy
    switch ( ch->class )
    {
        case CLASS_THIEF:
        case CLASS_BARD:
        case CLASS_WARRIOR:
        case CLASS_BARBARIAN:
        case CLASS_PALADIN:
        case CLASS_BLACK_KNIGHT:
        case CLASS_DRUID:
        case CLASS_MONK:
        case CLASS_CLERIC:
        case CLASS_MAG:
        case CLASS_SHAMAN:
            max_learn = 100;
            break;
        default:
            max_learn = 100;
    }

    skin_potentially = ch->pcdata->learned[ gsn_skin ];

    if ( skin_potentially <= max_learn )
    {
        check_improve( ch, NULL, gsn_skin, TRUE, 9 );
    }
}

void do_skin( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_INPUT_LENGTH ];
    int skin_skill, skin_level,  /*chance = 0,*/ counter = 0, size = 3, skin_broken = 1, cost_multi = 100, mod1, mod2, skin_delay, ch_delay, max_learn, skin_potentially, skin_time_left, skin_time_waste, condition;
    char * size_name = "¶redniej wielko¶ci", *msg_tochar1, *msg_toroom1, *msg_tochar2, *msg_toroom2, *msg_tochar3, *msg_toroom3, *skin_msg_tochar, *skin_msg_toroom;
    OBJ_DATA *skin, *corpse, *knife, *skin_custom;
    MOB_INDEX_DATA *mob;
    AFFECT_DATA af;

    // flagi co jest ubrane (nie wiem, maro sobie wymy¶li³ na jaki¶ pseudobitach,
    // nie potrafiê tego ogarn±æ, Gurthg, 2004-04-17
    int use_shield = 0;

    if ( ( skin_skill = get_skill( ch, gsn_skin ) ) <= 0 )
    {
        send_to_char( "Nie wiesz za bardzo jak siê do tego zabraæ.\n\r", ch );
        return;
    }

    if ( ch->fighting )
    {
        send_to_char( "Lepiej skup siê na walce!\n\r", ch );
        return;
    }

    if ( ch->mount )
    {
        send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
        return;
    }

    // dla lokacji wodnych nie pozwalamy skinowaæ
    if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_WATER ) )
    {
        send_to_char( "Nie dasz rady tutaj ¶ci±gaæ skóry!\n\r", ch );
        return;
    }

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Co chcesz oskórowaæ?\n\r", ch );
        return;
    }

    if ( ( corpse = get_obj_list( ch, arg, ch->in_room->contents ) ) == NULL )
    {
        send_to_char( "Nie widzisz tu nic takiego.\n\r", ch );
        return;
    }

    if ( corpse->item_type == ITEM_CORPSE_PC )
    {
        print_char( ch, "Co¶ jednak powstrzymywa³oby ciê przed tym barbarzyñskim ruchem.\n\r" );
        return;
    }

    if ( corpse->item_type != ITEM_CORPSE_NPC )
    {
        print_char( ch, "Z tego to ty skóry chyba nie ¶ci±gniesz.\n\r" );
        return;
    }

    if ( corpse->value[ 4 ] == 1 )
    {
        print_char( ch, "Kto¶ ju¿ chyba siê ¶ci±ganiem skóry z tego cia³a zajmuje.\n\r" );
        return;
    }

    if ( corpse->value[ 4 ] == 2 )
    {
        print_char( ch, "Skóra z tego cia³a zosta³a ju¿ ¶ci±gniêta.\n\r" );
        return;
    }

    if ( corpse->item_type != ITEM_CORPSE_NPC ||
            corpse->value[ 0 ] <= 0 ||
            ( mob = get_mob_index( corpse->value[ 0 ] ) ) == NULL )
    {
        print_char( ch, "Nic siê nie sta³o.\n\r" );
        DEBUG_INFO( "do_skin: Co¶ siê wali ze znajdowaniem cia³a." );
        return;
    }

    if ( !IS_SET( mob->parts, PART_SKIN ) || mob->skin_multiplier < 1 )
    {
        print_char( ch, "Chyba nie uda ci siê z tego stworzenia ¶ci±gn±æ skóry.\n\r" );
        return;
    }

    if ( !get_obj_index( OBJ_VNUM_SKIN ) )
    {
        print_char( ch, "Nic specjalnego siê nie dzieje.\n\r" );
        DEBUG_INFO( "do_skin: Nie ma skóry do za³adowania.\n\r" );
        return;
    }

    if ( ( skin = create_object( get_obj_index( OBJ_VNUM_SKIN ), FALSE ) ) == NULL )
    {
        print_char( ch, "Nic siê nie sta³o.\n\r" );
        DEBUG_INFO( "do_skin: Nie ma skóry do za³adowania.\n\r" );
        return;
    }
    /* skoro ju¿ bêdziemy skinowaæ to ¶ci±gamy affecty */
    if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED( ch, AFF_FLOAT ))
    {
        if ( skill_table[ gsn_fly ].msg_off )
        {
            send_to_char( skill_table[ gsn_fly ].msg_off, ch );
            send_to_char( "\n\r", ch );
        }
        affect_strip( ch, gsn_fly );
        affect_strip( ch, gsn_float );
    }
    strip_invis( ch, TRUE, TRUE );
    if ( IS_AFFECTED( ch, AFF_HIDE ) )
    {
        affect_strip( ch, gsn_hide );
    }

    /*obliczanie jako¶ci (condition) skórki na podstawie wytrenowania skilla i narzêdzia jakie siê posiada i innych*/
    condition = 100;

    switch ( ch->class )
    {
        case CLASS_MAG:
            condition = 65;
            max_learn = 80;
            break;
        case CLASS_CLERIC:
            condition = 75;
            max_learn = 80;
            break;
        case CLASS_THIEF:
        case CLASS_BARD:
        case CLASS_BARBARIAN:
        case CLASS_WARRIOR:
        case CLASS_SHAMAN:
            max_learn = 100;
            break;
        case CLASS_PALADIN:
        case CLASS_BLACK_KNIGHT:
            condition = 85;
            max_learn = 85;
            break;
        case CLASS_DRUID:
            condition = 80;
            max_learn = 80;
            break;
        case CLASS_MONK:
            condition = 70;
            max_learn = 70;
            break;
        default:
            max_learn = 100;
    }

    if ( ( knife = get_eq_char( ch, WEAR_WIELD ) ) != NULL )
    {
        if ( knife->value[ 0 ] == WEAPON_DAGGER )
        {
            skin_skill += get_skill(ch,gsn_dagger)/10;
        }
        else if ( knife->value[ 0 ] == WEAPON_SHORTSWORD )
        {
            condition -= condition/5;
            skin_skill += get_skill(ch,gsn_shortsword)/10;
        }
    }

    if ( ( knife = get_eq_char( ch, WEAR_SECOND ) ) != NULL )
    {
        if ( knife->value[ 0 ] == WEAPON_DAGGER )
        {
            skin_skill += get_skill(ch,gsn_dagger)/10;
        }
        else if ( knife->value[ 0 ] == WEAPON_SHORTSWORD )
        {
            condition -= condition/5;
            skin_skill += get_skill(ch,gsn_shortsword)/10;
        }
    }
    
  
    skin_broken = 1;
    skin_level  = corpse->value[ 2 ];
    skin_delay  = URANGE( 1, skin_level / 10, 10);

    if ( skin_skill > 0 && skin_skill < 11 )
    {
        skin_broken = number_range( 1, 100 );
        msg_tochar1 = "Podchodzisz chwiejnie i niepewnie do cia³a mierz±c je z grubsza wzrokiem.\n\r";
        msg_toroom1 = "podchodzi chwiejnie i powoli do cia³a nie wiedz±c chyba za dobrze jak je oprawiæ.\n\r";
        msg_tochar2 = "Z lekkim wahaniem, przewracaj±c oczami podchodzisz do cia³a.\n\r";
        msg_toroom2 = "podchodzi do cia³a z lekkim wahaniem, przewracaj±c niepewnie oczami.\n\r";
        msg_tochar3 = "Niepewnie nacinasz skórê tu i ówdzie, nie wychodzi ci to jednak specjalnie.\n\r";
        msg_toroom3 = "niepewnie nacina skórê tu i ówdzie, ale specjalnie to nie wychodzi.\n\r";
        skin_msg_tochar = "Niezdarnie prowadzisz ostrze niszcz±c nieco skórê.\n\r";
        skin_msg_toroom = "niezdarnie prowadzi ostrze niszcz±c nieco skórê.\n\r";
    }
    else if ( skin_skill > 10 && skin_skill < 21 )
    {
        skin_broken = number_range( 1, 70 );
        msg_tochar1 = "Zbli¿asz siê powoli do cia³a rozgl±daj±c siê niepewnie na boki.\n\r";
        msg_toroom1 = "zbli¿a siê powoli do cia³a rozgl±daj±c siê niepewnie na boki.\n\r";
        msg_tochar2 = "Patrzysz przez chwilê na cia³o jakby nie wiedz±c dok³adnie co robiæ po czym podchodzisz do niego.\n\r";
        msg_toroom2 = "patrzy przez chwilê na cia³o jakby nie wiedz±c dok³adnie co robiæ po czym podchodzi do niego.\n\r";
        msg_tochar3 = "Z wyra¼nym brakiem wprawy tniesz skórê i zrywasz j±.\n\r";
        msg_toroom3 = "z wyra¼nym brakiem wprawy tnie skórê i zrywa j±.\n\r";
        skin_msg_tochar = "Ostrze ob¶lizguje ci siê przecinaj±c skórê nie tam gdzie trzeba.\n\r";
        skin_msg_toroom = "przecina skórê nie tam gdzie trzeba.\n\r";
    }
    else if ( skin_skill > 20 && skin_skill < 31 )
    {
        skin_broken = number_range( 1, 55 );
        msg_tochar1 = "Podchodzisz do cia³a koncentruj±c na nim wzrok.\n\r";
        msg_toroom1 = "podchodzi do cia³a koncentruj±c na nim wzrok.\n\r";
        msg_tochar2 = "Koncentrujesz wzrok na ciele po czym wolno do niego podchodzisz.\n\r";
        msg_toroom2 = "koncentruje wzrok na ciele po czym wolno do niego podchodzi.\n\r";
        msg_tochar3 = "Wolno oprawiasz cia³o, ale mimo tego do idea³u ci±gle daleko.\n\r";
        msg_toroom3 = "wolno oprawia cia³o, daleko chyba jednak temu do idea³u.\n\r";
        skin_msg_tochar = "Ostrze ze¶lizguje ci siê przecinaj±c skórê nie tam gdzie trzeba.\n\r";
        skin_msg_toroom = "przecina skórê nie tam gdzie trzeba.\n\r";
    }
    else if ( skin_skill > 30 && skin_skill < 41 )
    {
        skin_broken = number_range( 1, 40 );
        msg_tochar1 = "¦ciskasz rêkê na rêkoje¶ci ostrza i podchodzisz do cia³a.\n\r";
        msg_toroom1 = "¶ciska rêkê na rêkoje¶ci ostrza i podchodzi do cia³a.\n\r";
        msg_tochar2 = "Zaciskasz rêkê na trzonie ostrza i zbli¿asz siê do cia³a.\n\r";
        msg_toroom2 = "zaciska rêkê na trzonie ostrza i zbli¿a siê do cia³a.\n\r";
        msg_tochar3 = "Szybkimi ruchami próbujesz ¶ci±gn±æ skórê, nie do koñca dobrze siê to jednak udaje.\n\r";
        msg_toroom3 = "szybkimi ruchami próbuje ¶ci±gn±æ skórê, nie do koñca dobrze siê to jednak udaje.\n\r";
        skin_msg_tochar = "Chwila nieuwagi i przecinasz skórê od spodu.\n\r";
        skin_msg_toroom = "przebija skórê od spodu nie skoncentrowawszy siê wystarczaj±co.\n\r";
    }
    else if ( skin_skill > 40 && skin_skill < 51 )
    {
        skin_broken = number_range( 1, 25 );
        msg_tochar1 = "Mierzysz wzrokiem cia³o po czym spokojnym krokiem do niego podchodzisz.\n\r";
        msg_toroom1 = "mierzy wzrokiem cia³o po czym spokojnym krokiem do niego podchodzi.\n\r";
        msg_tochar2 = "Patrzysz uwa¿nie na cia³o podchodz±c do niego i wa¿±c jednocze¶nie ostrze w d³oni.\n\r";
        msg_toroom2 = "patrzy uwa¿nie na cia³o podchodz±c do niego i wa¿±c jednocze¶nie ostrze w d³oni.\n\r";
        msg_tochar3 = "Spokojnie nacinasz w odpowiednich miejscach skórê, nie wszêdzie jednak równie precyzyjnie siê to udaje. Nie wszêdzie te¿ jej ¶ci±ganie idzie równie dobrze.\n\r";
        msg_toroom3 = "spokojnie nacina w odpowiednich, jak masz wra¿enie, miejscach skórê, nie wszêdzie jednak równie precyzyjnie siê to udaje. Nie wszêdzie te¿ jej ¶ci±ganie idzie równie dobrze.\n\r";
        skin_msg_tochar = "Chwila nieuwagi i przecinasz skórê od spodu.\n\r";
        skin_msg_toroom = "przebija skórê od spodu nie skoncentrowawszy siê wystarczaj±co.\n\r";
    }
    else if ( skin_skill > 50 && skin_skill < 61 )
    {
        skin_broken = number_range( 1, 10 );
        msg_tochar1 = "Taksujesz wzrokiem cia³o oceniajaæ jego rozmiary po czym do niego podchodzisz.\n\r";
        msg_toroom1 = "taksuje wzrokiem cia³o oceniaj±c jego rozmiary po czym do niego podchodzi.\n\r";
        msg_tochar2 = "Oceniasz w kilku spojrzeniach wagê i rozmiar cia³a podchodz±c do niego.\n\r";
        msg_toroom2 = "ocenia w kilku spojrzeniach wagê i rozmiar cia³a podchodz±c do niego.\n\r";
        msg_tochar3 = "Spokojnie podcinasz skórê tam gdzie trzeba i wolno próbujesz j± ¶ci±gn±æ. Nie wszystko jednak posz³o znowu tak jak trzeba.\n\r";
        msg_toroom3 = "spokojnie podcina skórê tam gdzie trzeba i wolno próbuje j± ¶ci±gn±æ. Nie wszystko jednak chyba posz³o tak jak trzeba.\n\r";
        skin_msg_tochar = "Nie napinasz wystarczaj±co fragmentu skóry i przecinasz go.\n\r";
        skin_msg_toroom = "nie napina wystarczaj±co fragmentu skóry i przecina go.\n\r";
    }
    else if ( skin_skill > 60 && skin_skill < 71 )
    {
        cost_multi *= number_range( 100, 105 );
        cost_multi /= 100;
        msg_tochar1 = "W kilku szybkich, pewnych krokach znajdujesz siê przy ciele.\n\r";
        msg_toroom1 = "w kilku szybkich, pewnych krokach znaduje siê przy ciele.\n\r";
        msg_tochar2 = "Kilka kroków wystarcza ci na znalezienie siê przy ciele.\n\r";
        msg_toroom2 = "w kilku krokach znajduje siê ju¿ przy ciele.\n\r";
        msg_tochar3 = "Szybko zabierasz siê do oprawiania skóry. Posz³o do¶æ dobrze, ale niestety mog³o pój¶æ jeszcze lepiej.\n\r";
        msg_toroom3 = "szybko zabiera siê do oprawiania skóry. Posz³o chyba do¶æ dobrze. Niestety jednak mog³o pój¶æ jeszcze lepiej.\n\r";
        skin_msg_tochar = "Nie napinasz wystarczaj±co fragmentu skóry i przecinasz go.\n\r";
        skin_msg_toroom = "nie napina wystarczaj±co fragmentu skóry i przecina go.\n\r";
    }
    else if ( skin_skill > 70 && skin_skill < 81 )
    {
        cost_multi *= number_range( 105, 110 );
        cost_multi /= 100;
        msg_tochar1 = "Fachowym spojrzeniem mierzysz lez±ce przed tob± cia³o oceniaj±c warto¶æ skóry, któr± zaraz z niego ¶ci±gniesz.\n\r";
        msg_toroom1 = "fachowym spojrzeniem mierzy le¿±ce cia³o oceniaj±c chyba warto¶æ skóry jak± zaraz z niego uzyska.\n\r";
        msg_tochar2 = "Skupionym wzrokiem patrzysz na cia³o licz±c ile pieniêdzy wpadnie ci za skórê z niego.\n\r";
        msg_toroom2 = "skupionym spojrzeniem patrzy na cia³o licz±c pewnie ile pieniêdzy wpadnie za skórê z niego.\n\r";
        msg_tochar3 = "Wprawnymi ruchami ¶ci±gasz bardzo szybko i sprawnie skórê. Nie uda³o ci siê jednak ustrzec kilku naprawdê drobniuteñkich b³êdów.\n\r";
        msg_toroom3 = "wprawnymi ruchami ¶ci±ga bardzo szybko i sprawnie skórê. Od kilku jednak naprawdê drobniuteñkich b³êdów ustrzec siê chyba nie uda³o.\n\r";
        skin_msg_tochar = "Troszkê zbyt niecierpliwie nacinasz skórê i nieco j± niszczysz.\n\r";
        skin_msg_toroom = "troszkê zbyt niecierpliwie nacina skórê i nieco j± niszczy.\n\r";
    }
    else if ( skin_skill > 80 && skin_skill < 91 )
    {
        cost_multi *= number_range( 115, 120 );
        cost_multi /= 100;
        msg_tochar1 = "B³yskawicznie znajdujesz siê przy ciele, przerzucaj±c w d³oniach ostrze.\n\r";
        msg_toroom1 = "b³yskawicznie znajduje siê przy ciele przerzucaj±c w d³oniach ostrze.\n\r";
        msg_tochar2 = "P³ynnym ruchem przesuwasz siê w kierunku cia³a uk³adaj±c odpowiednio ostrze.\n\r";
        msg_toroom2 = "p³ynnym ruchem przesuwa siê w kierunku cia³a uk³adaj±c odpowiednio ostrze.\n\r";
        msg_tochar3 = "Szybko¶æ i sprawno¶æ zamkniêta w kilku wyuczonych przez lata ruchach. Malutki, malutki brak precyzji czasami...\n\r";
        msg_toroom3 = "robi to szybko i sprawnie w kilku ruchach. Idealna robota. Hmm, prawie?\n\r";
        skin_msg_tochar = "";
        skin_msg_toroom = "";
    }
    else
    {
        cost_multi *= number_range( 130, 150 );
        cost_multi /= 100;
        msg_tochar1 = "Bez ¶ladu zawahania, wyæwiczonym ruchem znajdujesz siê przy ciele od razu pochylaj±c siê nad nim z ostrzem.\n\r";
        msg_toroom1 = "bez najmniejszego bodaj zawahania wyæwiczonym ruchem znajduje siê przy ciele od razu siê przy nim pochylaj±c z ostrzem.\n\r";
        msg_tochar2 = "Ruchem wyæwiczonym od lat uk³adasz ostrze na d³oni i pochylasz siê nad cia³em.\n\r";
        msg_toroom2 = "ruchem wyæwiczonym chyba od wielu ju¿ lat, uk³ada na d³oni ostrze i p³ynnie pochyla siê nad cia³em.\n\r";
        msg_tochar3 = "Wystudiowanymi od niepamiêtnych czasów ruchami koñczysz to co potrafisz zrobiæ z zamkniêtymi oczami przez sen. Idealnie wykrojona skóra jest twoja.\n\r";
        msg_toroom3 = "ruchami wystudiowanymi od niepamiêtnych czasów wykonuje to co chyba potrafi zrobiæ z zamkniêtymi oczami i przez sen. Skóra wykrojona idealnie.\n\r";
        skin_msg_tochar = "";
        skin_msg_toroom = "";

        /*
           a to siê mo¿e przyda do testowania to zostawiê
           sprintf(buf, "%s %d.\n\r", "COST_MULTI TERAZ TO:", cost_multi);
           send_to_char(buf, ch);
         */
    }
    /* plynne liczenie condition skory w zaleznosci od skilla - Tener */
    condition *= UMIN( number_range(skin_skill+10,skin_skill+20) , 100 );
    condition /= 100;
    condition = UMIN(condition, 100);
    condition -= number_range(0,5);

    /*liczenie jakiej warto¶ci skórê uzyskamy, jakie¶ tam pseudo przybli¿enie rozk³adu normalnego w jednym momencie, miejmy nadziejê, ¿e dzia³a*/
    if ( skin_broken > 50 )
    {
        condition *= 0;
    }
    else if ( skin_broken > 1 )
    {
        condition *= 100 - skin_broken;
        condition /= 100;
    }


    /*a tu przelatujemy mantyk ze zliczaniem które r±czki i czym s± zajête*/

    knife = get_eq_char ( ch, WEAR_WIELD );

    if ( knife )
    {
        counter += 1;
    }

    if ( knife && ( ( knife->value[0] == WEAPON_DAGGER ) || ( knife->value[ 0 ] == WEAPON_SHORTSWORD ) ) )
    {
        counter += 2;
    }

    if ( ( knife = get_eq_char( ch, WEAR_HOLD ) ) != NULL )
    {
        counter += 4;
    }

    if ( ( knife = get_eq_char( ch, WEAR_SECOND ) ) != NULL )
    {
        counter += 8;
    }

    if ( ( ( knife = get_eq_char( ch, WEAR_SECOND ) ) != NULL ) &&
            ( ( knife->value[ 0 ] == WEAPON_DAGGER ) || ( knife->value[ 0 ] == WEAPON_SHORTSWORD ) ) )
    {
        counter += 16;
    }

    if ( ( knife = get_eq_char( ch, WEAR_LIGHT ) ) != NULL )
    {
        counter += 32;
    }

    if ( ( knife = get_eq_char( ch, WEAR_SHIELD ) ) != NULL )
    {
        counter += 64;
        use_shield = 1;
    }

    if ( counter == 0 )
    {
        send_to_char( "Hmm, go³ymi rêkami? Ambitny plan, ale chyba siê nie powiedzie.\n\r", ch );
        return;
    }

    if ( counter == 4 || counter == 32 || counter == 36 )
    {
        send_to_char( "Przyda³oby ci siê jednak do tego odpowiednie narzêdzie.\n\r", ch );
        return;
    }

    if ( counter == 1 || counter == 5 || counter == 9 || counter == 33 )
    {
        send_to_char( "W sumie mo¿naby spróbowaæ, ale to jakby rze¼biæ patykiem.\n\r", ch );
        return;
    }

    if (counter == 7 || counter == 11 || counter == 27 || counter == 35 || use_shield)
    {
        send_to_char( "¦ci±ganie skóry to nie siekanie buraków. Pozostaw jedn± rêkê woln±.\n\r", ch );
        return;
    }

    if ( ( ( knife = get_eq_char( ch, WEAR_WIELD ) ) != NULL ) && ( knife->value[ 4 ] & WEAPON_FLAMING ) )
    {
        send_to_char( "Tym narzêdziem to co najwy¿ej mo¿esz skórê spaliæ, a nie ¶ci±gn±æ.\n\r", ch );
        return;
    }

    if ( ( ( knife = get_eq_char( ch, WEAR_WIELD ) ) != NULL ) && ( knife->value[ 4 ] & WEAPON_FROST ) )
    {
        send_to_char( "Tym narzêdziem co najwy¿ej mo¿esz zamroziæ cia³o, a nie ¶ci±gn±æ skórê.\n\r", ch );
        return;
    }

    if ( ( ( knife = get_eq_char( ch, WEAR_WIELD ) ) != NULL ) && ( knife->value[ 4 ] & WEAPON_TOXIC ) )
    {
        send_to_char( "Po u¿yciu tego narzêdzia ze skóry nic nie zostanie.\n\r", ch );
        return;
    }

    if ( ( ( knife = get_eq_char( ch, WEAR_WIELD ) ) != NULL ) && ( knife->value[ 4 ] & WEAPON_VORPAL ) )
    {
        condition *= number_range( 120, 125 );
        condition /= 100;
    }
    else
        if ( ( ( knife = get_eq_char( ch, WEAR_WIELD ) ) != NULL ) && ( knife->value[ 4 ] & WEAPON_SHARP ) )
        {
            condition *= number_range( 115, 120 );
            condition /= 100;
        }
        
    skin_potentially = ch->pcdata->learned[ gsn_skin ];//Raszer - przenosze probe nauki tutaj, bo cos czuje, ze kod nie docieral do konca funkcji, ulatwiam 2x

    if ( skin_potentially <= max_learn )
    {
        check_improve( ch, NULL, gsn_skin, TRUE, 4 );
    }   
    /*tutaj robimy skórkê*/

    if ( corpse->weight < 150 )
    {
        size = 1;
        size_name = "jaka¶";
        cost_multi *= number_range( 70, 75 );
        cost_multi /= 100;
    }

    if ( corpse->weight >= 150 && corpse->weight < 450 )
    {
        size = 2;
        size_name = "niewielkich rozmiarów";
        cost_multi *= number_range( 70, 75 );
        cost_multi /= 100;
    }

    if ( corpse->weight >= 450 && corpse->weight < 1000 )
    {
        size = 3;
        size_name = "¶redniej wielko¶ci";
        cost_multi *= number_range( 85, 95 );
        cost_multi /= 100;
    }

    if ( corpse->weight >= 1000 && corpse->weight < 2000 )
    {
        size = 4;
        size_name = "du¿a";
        cost_multi *= number_range( 105, 115 );
        cost_multi /= 100;
    }

    if ( corpse->weight >= 2000 && corpse->weight < 3500 )
    {
        size = 5;
        size_name = "wielka";
        cost_multi *= number_range( 120, 135 );
        cost_multi /= 100;
    }

    if ( corpse->weight >= 3500 )
    {
        size = 6;
        size_name = "ogromna";
        cost_multi *= number_range( 135, 150 );
        cost_multi /= 100;
    }

    /*name*/
    sprintf( buf, skin->name, race_table[ mob->race ].name2 );
    free_string( skin->name );
    skin->name = str_dup( buf );

    /*short*/
    sprintf( buf, "%s%s%s", size_name, " skóra ", race_table[ mob->race ].name2 );
    free_string( skin->short_descr );
    skin->short_descr = str_dup( buf );

    /*long*/
    sprintf( buf, "%s%s%s%s", capitalize( size_name ), " skóra zdarta z ", race_table[ mob->race ].name2, " le¿y tutaj." );
    free_string( skin->description );
    skin->description = str_dup( buf );

    /*description*/
    sprintf( buf, "%s%s%s%s%s", "Jaka¶ ", size_name, " skóra. Nie³atwo na pierwszy rzut oka oceniæ z jakiego stworzenia zosta³a ona zdjêta, ale wydaje ci siê, ¿e by³o ono ", race_table[ mob->race ].name5, ", albo czym¶ w tym rodzaju. Nie mo¿na powiedzieæ ¿eby jako¶ specjalnie pachnia³a, ale sprzedaæ ja siê chyba da." );
    free_string( skin->item_description );
    skin->item_description = str_dup( buf );

    skin->weight = corpse->weight * number_range( 9, 12 ) / 100;

    mod1 = corpse->value[ 0 ];
    mod2 = corpse->value[ 2 ];

    if ( number_range( 1, 2 ) == 1 )
    {
        sprintf( buf, "%s", msg_tochar1 );
        send_to_char( buf, ch );
        sprintf( buf, "%s %s", ch->name, msg_toroom1 );
        act( buf, ch, NULL, NULL, TO_ROOM );
        corpse->value[ 4 ] = 1;
    }
    else
    {
        sprintf( buf, "%s", msg_tochar2 );
        send_to_char( buf, ch );
        sprintf( buf, "%s %s", ch->name, msg_toroom2 );
        act( buf, ch, NULL, NULL, TO_ROOM );
        corpse->value[ 4 ] = 1;
    }

    if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED( ch, AFF_FLOAT ))
    {
        if ( skill_table[ gsn_fly ].msg_off )
        {
            send_to_char( skill_table[ gsn_fly ].msg_off, ch );
            send_to_char( "\n\r", ch );
        }
        affect_strip( ch, gsn_fly );
        affect_strip( ch, gsn_float );
    }

   // ch->position = POS_SITTING;

    /*pierd obs³uguj±cy zranienie siê no¿em i komunikaty ¶ci±gania*/
    if ( skin_skill < 81 )
    {
        for ( skin_time_left = skin_delay;skin_time_left > 2 ; skin_time_left = skin_time_left - skin_time_waste )
        {
            skin_time_waste = number_range( 1, 2 );

            if ( number_range( 1, 5 ) == 3 )

            {
                sprintf( buf, "%s", skin_msg_tochar );
                wait_printf( ch, skin_delay - skin_time_left + skin_time_waste, buf );
                sprintf( buf, "%s %s", ch->name, skin_msg_toroom );
                wait_act( skin_delay - skin_time_left + skin_time_waste, buf, ch, NULL, NULL, TO_ROOM );
                condition -= number_range( 1, 2 );
                continue;

            }

            if ( number_range( 1, 50 ) - skin_skill > 0 )
            {
                if ( number_range( 1, 3 ) == 2 )
                {
                    wait_printf( ch, skin_delay - skin_time_left + skin_time_waste, "Zacinasz siê przy ¶ci±ganiu skóry w palec.\n\r" );
                    sprintf( buf, "%s %s", ch->name, "zacina siê przy ¶ci±ganiu skóry w palec.\n\r" );
                    wait_act( skin_delay - skin_time_left + skin_time_waste, buf, ch, NULL, NULL, TO_ROOM );
                    ch->hit = UMAX( ch->hit - number_range( 2, 3 ), 1 );
                }
            }
        }
    }

    if ( corpse->value[ 6 ] > 0 )
    {
        skin_custom = create_object( get_obj_index( corpse->value[ 6 ] ), FALSE );
        if ( skin_custom->cost > 0 )
        {
            if ( number_percent() > 50 )
            {
                skin_custom->cost += skin_custom->cost/( 8 + number_range( 0, 4 ) );
            }
            else
            {
                skin_custom->cost -= skin_custom->cost/( 8 + number_range( 0, 4 ) );
            }
        }
        else if ( corpse->value[ 6 ] > 0 )
        {
            skin_custom->cost = ( ( skin_level * cost_multi * 3 * mob->skin_multiplier * ( ( number_range( 6, 8 ) + number_range( 6, 8 ) ) ) ) ) / 100000;
        }
        skin = skin_custom;
    }
    else
    {

		skin->cost = skin_level;
        if ( !str_cmp( ch->name, "Raszer" ) ) print_char( ch, "Cost 1: %d\n\r", skin->cost );
        skin->cost *= cost_multi;
        skin->cost /= 100;
        if ( !str_cmp( ch->name, "Raszer" ) ) print_char( ch, "Cost 2: %d\n\r", skin->cost );
        skin->cost *= mob->skin_multiplier;
        skin->cost /= 100;
        if ( !str_cmp( ch->name, "Raszer" ) ) print_char( ch, "Cost 3: %d\n\r", skin->cost );
        skin->cost *= ( number_range( 6, 8 ) + number_range( 8, 10 ) )  * 12;
        if ( !str_cmp( ch->name, "Raszer" ) ) print_char( ch, "Cost 4: %d\n\r", skin->cost );

    }

    mod1 = corpse->value[ 0 ];
    mod2 = corpse->value[ 2 ];

    ch_delay = skin_delay * 4 + 10;
    WAIT_STATE( ch, ch_delay );

    af.where = TO_AFFECTS;
    af.type = gsn_skin;
    af.level = mod2;
    af.location = APPLY_NONE;
    af.modifier = mod1;
    af.bitvector = &AFF_NONE;
    af.duration = 1;     af.rt_duration = 0;
    af.visible = FALSE;
    affect_to_char( ch, &af, NULL, FALSE );

    sprintf( buf, "%s", msg_tochar3 );
    wait_printf( ch, skin_delay, buf );
    sprintf( buf, "%s %s", ch->name, msg_toroom3 );
    wait_act( skin_delay, buf, ch, NULL, NULL, TO_ROOM );
    corpse->value[ 4 ] = 2;
    wait_function( ch, skin_delay, &do_skin1, "" );

    if ( corpse->value[ 6 ] > 0 )
    {
        skin->condition = condition;
        wait_obj_to_char( skin, skin_delay, ch );
        sprintf
            (
             buf,
             "do_skin: custom: v: %d c: %d w: %d",
             corpse->value[ 6 ],
             skin->condition,
             skin->cost
            );
        log_string( buf );
    }
    else
    {
        skin->condition = condition;
        wait_obj_to_char( skin, skin_delay, ch );
    }

   

    return;
}

void do_skin1(CHAR_DATA *ch, char *argument)
{
    /*komenda pomocnicza do do_skin*/

    OBJ_DATA *obj = NULL, *obj_next;
    MOB_INDEX_DATA *mob;

    AFFECT_DATA *taff;
    int level, modifier;
    char buf[MAX_INPUT_LENGTH];


    if ( !is_affected( ch, gsn_skin ) )
    {
        sprintf(log_buf,"do_skin1: Komu¶ zosta³ gsn_skin albo kto¶ cffaniakuje.");
        log_string( log_buf );
        return;
    }


    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;

        /*ustalanie valuesów cia³a*/
        taff = affect_find( ch->affected, gsn_skin );
        level = taff->level;
        modifier = taff->modifier;

        /* wczesaæ tu skin_custom na tej zasadzie, ¿e v6 > 0 i po v6 za³adowaæ, pobieraj±c hidden_desc gdzie trzeba*/
        if ( obj->item_type == ITEM_CORPSE_NPC
                /* && obj->value[4] == 1 */
                && obj->value[0] == modifier
                && obj->value[2] == level )
        {
            mob = get_mob_index(modifier);

            if (obj->value[6] > 0)
            {

                free_string(obj->description);
                obj->description = str_dup(obj->hidden_description);
            }
            else
            {
                if ( obj->wyciete > 0 )
                {
                    sprintf(buf, "%s%s%s", "Zmasakrowane, pociete i odarte ze skóry cia³o ", race_table[ mob->race ].name2, " le¿y tutaj powoli gnij±c.");
                } else {
                    sprintf(buf, "%s%s%s", "Zmasakrowane, odarte ze skóry cia³o ", race_table[ mob->race ].name2, " le¿y tutaj powoli gnij±c.");
                }
                free_string(obj->description);
                obj->description = str_dup(buf);
            }

            affect_strip(ch, gsn_skin);
            break;
        }

    }

}


void do_parry( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * vch = NULL;
    OBJ_DATA *weapon_primary, *weapon_offhand = NULL;
    int parry_skill, chance = 0, weapon_skill_primary = 0, weapon_skill_offhand = 0;
    int weapon_primary_sn, weapon_offhand_sn, mod;

    if ( IS_AFFECTED(ch, AFF_PARALYZE))
    {
        send_to_char( "Nic nie mo¿esz zrobiæ.\n\r", ch );
        return;
    }

    if ( ( parry_skill = get_skill( ch, gsn_parry ) ) == 0 )
    {
        send_to_char( "Nie znasz siê na tym.\n\r", ch );
        return;
    }

    weapon_primary = get_eq_char( ch, WEAR_WIELD );
    weapon_offhand = get_eq_char( ch, WEAR_SECOND );

    if ( weapon_primary == NULL && weapon_offhand == NULL )
    {
        send_to_char( "Przyda³aby siê jaka¶ broñ.\n\r", ch );
        return;
    }

    if ( ch->fighting == NULL )
    {
        send_to_char( "Z nikim nie walczysz.\n\r", ch );
        return;
    }

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
        if ( vch->fighting == ch )
            break;
    }

    if ( !vch )
    {
        send_to_char( "Nie jeste¶ nara¿on<&y/a/e> na ciosy, wiêc nie ma za bardzo czego parowaæ.\n\r", ch );
        return;
    }

    chance = parry_skill / 2;

    if( is_affected( ch, gsn_wardance ))
        chance += get_skill(ch, gsn_wardance)/13;

    /* find weapon skills */
    if ( weapon_primary )
    {
        weapon_primary_sn = get_weapon_sn( ch, TRUE );
        weapon_skill_primary = get_weapon_skill( ch, weapon_primary_sn );

        /* bonus jesli sa 2 bronie */
        if ( weapon_offhand )
        {
            weapon_offhand_sn = get_weapon_sn( ch, FALSE );
            weapon_skill_offhand = get_weapon_skill( ch, weapon_offhand_sn );
            chance += ( parry_mod( weapon_primary ) + parry_mod( weapon_offhand ) ) / 2;
            mod = 5;
            chance += ( weapon_skill_primary + weapon_skill_offhand ) / mod;
        }
        else
        {
            chance += parry_mod( weapon_primary );
            mod = 4;
            chance += weapon_skill_primary / mod;
        }
    }
    else if ( weapon_offhand )
    {
        weapon_offhand_sn = get_weapon_sn( ch, FALSE );
        weapon_skill_offhand = get_weapon_skill( ch, weapon_offhand_sn );
        chance += parry_mod( weapon_offhand );
        mod = 5;
        chance += weapon_skill_offhand / mod;
    }

    chance += (get_curr_stat_deprecated( ch, STAT_DEX ) - 15);

    chance = URANGE( 10, chance, 95 );
#ifdef INFO
    print_char( ch, "Parry skill: %d\n\r", parry_skill );

    if ( weapon_primary )
    {
        print_char( ch, "Primary weapon skill: %d\n\r", weapon_skill_primary );
        print_char( ch, "Primary weapon parry mod: %d\n\r", parry_mod( weapon_primary ) );

    }

    if ( weapon_offhand )
    {
        print_char( ch, "Offhand weapon skill: %d\n\r", weapon_skill_offhand );
        print_char( ch, "Offhand weapon parry mod: %d\n\r", parry_mod( weapon_offhand ) );
    }

    print_char( ch, "Chance to parry: %d\n\r", chance );
#endif

    if ( number_percent() < chance )
    {
        print_char( ch, "{5Bêdziesz teraz parowa%s ataki.{x\n\r", ch->sex == 2 ? "³a" : "³" );
        SET_BIT( ch->fight_data, FIGHT_PARRY );
        check_improve( ch, ch->fighting, gsn_parry, TRUE, 90 );
    }
    else
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        check_improve( ch, ch->fighting, gsn_parry, FALSE, 80 );
    }
    WAIT_STATE( ch, skill_table[ gsn_parry ].beats );
    return;
}

void do_bandage( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim = NULL;
    OBJ_DATA *bandage = NULL;
    AFFECT_DATA af;
    int skill, hp_percent;

    if ( ( skill = get_skill( ch, gsn_bandage ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->fighting )
    {
        send_to_char( "Skup siê lepiej na walce.\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Czyje rany chcesz opatrzyæ?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        return;
    }

    if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) )
    {
        send_to_char( "To siê nie uda.\n\r", ch );
        return;
    }

    if ( victim->position == POS_FIGHTING || victim->fighting != NULL )
    {
        if ( ch == victim )
            act( "Nie jestes w stanie siê obanda¿owaæ w czasie walki!", ch, NULL, victim, TO_CHAR );
        else
            act( "Nie jestes w stanie obanda¿owaæ $Z w czasie gdy $E walczy!", ch, NULL, victim, TO_CHAR );

        return;
    }

    if ( IS_AFFECTED(victim, AFF_HEALING_SALVE))
    {
        send_to_char( "To siê nie uda.\n\r", ch );
        return;
    }

    if ( is_affected( victim, gsn_bandage ) && !is_affected(victim,gsn_bleeding_wound ) )
    {
        if ( ch == victim )
            send_to_char( "Twoje rany s± ju¿ opatrzone.\n\r", ch );
        else
            print_char( ch, "Rany %s s± ju¿ opatrzone.\n\r", victim->name2 );
        return;
    }

    if ( ( bandage = get_eq_char( ch, WEAR_HOLD ) ) == NULL ||
            bandage->item_type != ITEM_BANDAGE )
    {
        send_to_char( "Najpierw postaraj siê o jaki¶ banda¿ albo co¶ do opatrywania ran.\n\r", ch );
        return;
    }

    if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) )
    {
        print_char( ch, "Zabanda¿owanie %s raczej nic nie da, %s nie ma przecie¿ krwi.\n\r",
                victim->name2,
                victim->sex == 2 ? "ona" : victim->sex == 1 ? "on" : "ono" );
        return;
    }

    hp_percent = 100 * victim->hit / UMAX( 1, get_max_hp(victim) );

    if ( hp_percent > 95 && !is_affected(victim,gsn_bleeding_wound) )
    {
        print_char( ch, "Nie ma tu co opatrywaæ.\n\r" );
        return;
    }

    skill = skill * URANGE( 25, hp_percent, 95 ) / 100;

    if ( ch->sex == 2 )
    {
        skill += 5;
    }

    switch ( ch->class )
    {
        case CLASS_CLERIC:
            skill += 20;
            break;
        case CLASS_DRUID:
            skill += 15;
            break;
        case CLASS_WARRIOR:
        case CLASS_SHAMAN:
            skill += 10;
            break;
        case CLASS_BARBARIAN:
        case CLASS_PALADIN:
            skill += 5;
            break;
        case CLASS_MAG:
        case CLASS_BLACK_KNIGHT:
            skill = UMAX( skill - 5, 1 );
            break;
        default:
            break;
    }

    if ( number_percent() < skill || victim->hit < -5 )
    {
        af.where = TO_AFFECTS;
        af.type = gsn_bandage;

        if ( ch->class == CLASS_CLERIC )
        {
            af.level = bandage->value[ 1 ] + 2;
        }
        else if ( ch->class == CLASS_DRUID )
        {
            af.level = bandage->value[ 1 ] + 1;
        }
        else
        {
            af.level = bandage->value[ 1 ];
        }
        af.level *= 2;

        af.duration = 2 * UMAX( 6, get_skill( ch, gsn_bandage) / 8 );
        af.rt_duration = 0;
        af.location = APPLY_NONE;
        af.modifier = 0;

        if ( ch->class == CLASS_CLERIC)
        {
            af.modifier = URANGE( 10, ch->level - 5, 25 );
        }

        af.bitvector = &AFF_NONE;
        affect_to_char( victim, &af, "bandaze", TRUE );

        if ( victim->hit < 1 )
        {
            victim->hit = 1;
            update_pos( victim );
        }

        if ( ch == victim )
        {
            print_char( ch, "Ostro¿nie opatrujesz swoje rany, boli jak diabli.\n\r" );
            act( "$n opatruje swoje rany.", ch, NULL, NULL, TO_ROOM );
        }
        else
        {
            if ( victim->position > POS_SLEEPING )
            {
                act( "Ostro¿nie opatrujesz $O rany. $N krzywi siê z bólu, po chwili uspokaja siê i oddycha z ulg±.", ch, NULL, victim, TO_CHAR );
                act( "$n ostro¿nie opatruje ci rany. Boli jak diabli, po chwili jednak ból ustêpuje.", ch, NULL, victim, TO_VICT );
            }
            else if ( victim->position == POS_SLEEPING && !IS_AFFECTED(victim, AFF_SLEEP))
            {
                act( "Ostro¿nie opatrujesz $O rany. $N budzi siê gwa³townie krzywi±c siê z bólu, ale po chwili uspokaja siê i oddycha z ulg±.", ch, NULL, victim, TO_CHAR );
                victim->position = POS_SITTING;
                act( "Budzisz siê gwa³townie czuj±c jaki¶ ból. To $n opatruje ci rany. Po chwili ból ustêpuje i oddychasz z ulg±.", ch, NULL, victim, TO_VICT );
            }
            else
                act( "Ostro¿nie opatrujesz rany $Z.", ch, NULL, victim, TO_CHAR );

            act( "$n ostro¿nie opatruje rany $Z.", ch, NULL, victim, TO_NOTVICT );
        }

        if ( ch->class == CLASS_CLERIC )
        {
            print_char( ch, "Dotykasz lekko opatrunku i wymawiasz szeptem s³owa modlitwy. \n\r" );
            act( "$n po chwili dotyka lekko opatrunku i wymawia szeptem s³owa modlitwy.", ch, NULL, NULL, TO_ROOM );
        }

        if( is_affected( victim, gsn_bleeding_wound ) )
        {
            if ( skill_table[ gsn_bleeding_wound ].msg_off )
            {
                send_to_char( skill_table[ gsn_bleeding_wound ].msg_off, victim );
                send_to_char( "\n\r", victim );
            }
            affect_strip( victim, gsn_bleeding_wound );
        }

        WAIT_STATE( ch, skill_table[ gsn_bandage ].beats );
        check_improve( ch, NULL, gsn_bandage, TRUE, 7 );
    }
    else
    {
        if ( ch == victim )
        {
            print_char( ch, "Próbujesz opatrzyæ swoje rany, jednak nie udaje ci siê nic zdzia³aæ.\n\r" );
            act( "$n próbuje opatrzyæ swoje rany.", ch, NULL, NULL, TO_ROOM );
        }
        else
        {
            if ( victim->position > POS_SLEEPING )
            {
                act( "Próbujesz opatrzyæ rany $Z, ale zadajesz tylko wiêcej bólu.", ch, NULL, victim, TO_CHAR );
                act( "$n próbuje opatrzyæ ci rany, jednak z do¶æ mizernym skutkiem.", ch, NULL, victim, TO_VICT );
            }
            else if ( victim->position == POS_SLEEPING && !IS_AFFECTED(victim, AFF_SLEEP))
            {
                if (IS_NPC( victim ))
                {
                    act( "$N budzi siê z rykiem gdy nieumiejêtnie próbujesz opatrzyæ $S rany.", ch, NULL, victim, TO_CHAR );
                }
                else
                {
                    act( "$N budzi siê z krzykiem gdy nieumiejêtnie próbujesz opatrzyæ $S rany.", ch, NULL, victim, TO_CHAR );
                }
                victim->position = POS_SITTING;
                act( "Budzisz siê z krzykiem czuj±c okropny ból. To $n próbuje nieumiejêtnie opatrzyæ twoje rany.", ch, NULL, victim, TO_VICT );
            }
            else
                act( "Próbujesz opatrzyæ rany $Z, jednak nie udaje ci siê nic zdzia³aæ.", ch, NULL, victim, TO_CHAR );

            act( "$n próbuje opatrzyæ rany $Z, z do¶æ mizernym skutkiem.", ch, NULL, victim, TO_NOTVICT );
        }
        if ( number_percent() < 10 )
        {
            victim->hit = UMAX( victim->hit - 1, 1 );
        }
        WAIT_STATE( ch, skill_table[ gsn_bandage ].beats );
        check_improve( ch, NULL, gsn_bandage, FALSE, 10 );
    }

    if ( bandage->value[ 0 ] > 0 && --bandage->value[ 0 ] == 0 )
    {
        print_char( ch, "Zu¿y³%s juz ca³y %s.\n\r", ch->sex == 2 ? "a¶" : "e¶", bandage->name4 );

        /*artefact*/
        if ( is_artefact( bandage ) && !IS_NPC( ch ) && !IS_IMMORTAL( ch ) )
            artefact_to_char( bandage, ch );
        obj_from_char( bandage );
        /*artefact*/
        if ( is_artefact( bandage ) ) extract_artefact( bandage );
        extract_obj( bandage );
    }
    return;
}


extern int exp_table[];

void do_trap( CHAR_DATA *ch, char *argument )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char buf [ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj;
    AFFECT_DATA af, *afuj;
    EXIT_DATA *pexit;
    int door;
    int trap_level;
    TRAP_DATA *trap;
    int skill, exp, iWear;

    argument = one_argument( argument, arg1 );

    if ( arg1[ 0 ] == '\0' )
    {
        send_to_char( "Chcesz szukaæ czy rozbrajaæ pu³apki?\n\r", ch );
        return;
    }

    if (ch->class == CLASS_BARD )
    {
        for ( iWear = 0; iWear < MAX_WEAR - 1; iWear++ )
        {
            if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
                continue;

            if ( obj->item_type != ITEM_ARMOR )
                continue;

            if ( iWear != WEAR_HEAD && iWear != WEAR_BODY &&
                    iWear != WEAR_LEGS && iWear != WEAR_FEET &&
                    iWear != WEAR_HANDS && iWear != WEAR_ARMS )
                continue;

            if ( obj->value[4] != 0 && obj->value[4] != 1 && obj->value[4] != 6 )
            {
                send_to_char( "Twoja zbroja zbyt ciê uwiera.\n\r", ch );
                return;
            }
        }
    }

    if ( !str_prefix( arg1, "szukaj" ) || !str_prefix( arg1, "search" ) )
    {
        if ( ( skill = get_skill( ch, gsn_detect_traps ) ) <= 0 )
        {
            send_to_char( "Nie znasz siê na tym.\n\r", ch );
            return;
        }
        /*
           zdejmujê to przyciêcie skilla
Gurthg: 2003-09-25
skill = skill / 2;
         */

        switch ( GET_RACE( ch ) )
        {
            case 2: skill +=  5;	break; // elf
            case 4: skill += 10;	break; // gnome
            case 6: skill += 15;	break; // hobbit
            default: break;
        }

        switch ( get_curr_stat_deprecated( ch, STAT_DEX ) )
        {
            case  6: skill -= 50;	break;
            case  7: skill -= 40;	break;
            case  8: skill -= 30;	break;
            case  9: skill -= 25;	break;
            case 10: skill -= 20;	break;
            case 11: skill -= 15;	break;
            case 12:
            case 13: skill -= 10;	break;
            case 14:
            case 15: skill -= 5;	break;
            case 16:
            case 17:
            case 18:
            case 19: break;
            case 20:
            case 21: skill += 5;	break;
            case 22:
            case 23: skill += 10;	break;
            case 24:
            case 25: skill += 15;	break;
            case 26:
            case 27: skill += 20;	break;
            case 28: skill += 25;	break;
            default: break;
        }

        if (ch->class == CLASS_BARD )
            skill += skill/4;

        argument = one_argument( argument, arg2 );

        WAIT_STATE( ch, skill_table[ gsn_detect_traps ].beats );

        if ( arg2[ 0 ] == '\0' )
        {
            if ( ch->in_room->trap <= 0 ||
                    !EXT_IS_SET( ch->in_room->room_flags, ROOM_TRAP ) ||
                    ( trap = get_trap_index( ch->in_room->trap ) ) == NULL ||
                    !trap->active )
            {
                print_char( ch, "Ostro¿nie rozgl±dasz siê, jednak nie znajdujesz nic podejrzanego.\n\r" );
                act( "$n ostro¿nie rozgl±da siê.", ch, NULL, NULL, TO_ROOM );
                return;
            }

            trap_level = trap->level;

            skill = URANGE( 1, skill - ( trap_level * 5 ), 95 );

            if ( number_percent() < skill )
            {
                if( is_affected(ch,gsn_detect_traps ))
                    affect_strip(ch,gsn_detect_traps);
                print_char( ch, "Ostro¿nie rozgladasz siê, po chwili znajdujesz sprytnie zamaskowan± pu³apkê.\n\r" );
                check_improve( ch, NULL, gsn_detect_traps, TRUE, 10 );
                af.where = TO_AFFECTS;
                af.type = gsn_detect_traps;
                af.level = 0;
                af.duration = 24;                 af.rt_duration = 0;
                af.location = APPLY_NONE;
                af.modifier = trap->vnum;
                af.bitvector = &AFF_NONE;
                affect_to_char( ch, &af, NULL, FALSE );
                return;
            }
            else
            {
                print_char( ch, "Ostro¿nie rozgl±dasz siê, jednak nie znajdujesz nic podejrzanego.\n\r" );
                check_improve( ch, NULL, gsn_detect_traps, FALSE, 14 );
            }

            act( "$n ostro¿nie rozgl±da siê.", ch, NULL, NULL, TO_ROOM );
            return;
        }

        if ( ( obj = get_obj_here( ch, arg2 ) ) != NULL )
        {
            if ( obj->trap <= 0 ||
                    ( trap = get_trap_index( obj->trap ) ) == NULL ||
                    !trap->active )
            {
                print_char( ch, "Ostro¿nie ogl±dasz %s, jednak nie znajdujesz nic podejrzanego.\n\r", obj->name4);
                act( "$n ostro¿nie ogl±da $h.", ch, obj, NULL, TO_ROOM );
                return;
            }

            trap_level = trap->level;

            skill = URANGE( 1, skill - ( trap_level * 5 ), 95 );

            if ( number_percent() < skill )
            {
                print_char( ch, "Ostro¿nie ogl±dasz %s, po chwili znajdujesz sprytnie zamaskowan± pu³apkê.\n\r", obj->name4 );
                if( is_affected(ch,gsn_detect_traps ))
                    affect_strip(ch,gsn_detect_traps);
                check_improve( ch, NULL, gsn_detect_traps, TRUE, 10 );
                af.where = TO_AFFECTS;
                af.type = gsn_detect_traps;
                af.level = 0;
                af.duration = 24;                 af.rt_duration = 0;
                af.location = APPLY_NONE;
                af.modifier = trap->vnum;
                af.bitvector = &AFF_NONE;
                affect_to_char( ch, &af, NULL, FALSE );
            }
            else
            {
                print_char( ch, "Ostro¿nie ogl±dasz %s, jednak nie znajdujesz nic podejrzanego.\n\r", obj->name4 );
                check_improve( ch, NULL, gsn_detect_traps, FALSE, 14 );
            }

            act( "$n ostro¿nie ogl±da $h.", ch, obj, NULL, TO_ROOM );
            return;
        }

        if ( ( door = find_door( ch, arg2 ) ) >= 0 )
        {
            if ( ( pexit = ch->in_room->exit[ door ] ) == NULL	||
                    !IS_SET( pexit->exit_info, EX_ISDOOR ) )
            {
                send_to_char( "Gdzie dok³adnie chcesz szukaæ pu³apek?\n\r", ch );
                return;
            }

            if ( pexit->trap <= 0 ||
                    !IS_SET( pexit->exit_info, EX_TRAP ) ||
                    ( trap = get_trap_index( pexit->trap ) ) == NULL ||
                    !trap->active )
            {
                print_char( ch, "Nie znajdujesz nic podejrzanego.\n\r" );
                return;
            }

            trap_level = trap->level;

            skill = URANGE( 1, skill - ( trap_level * 5 ), 95 );

            if ( number_percent() < skill )
            {
                if( is_affected(ch,gsn_detect_traps ))
                    affect_strip(ch,gsn_detect_traps);
                print_char( ch, "Znajdujesz sprytnie zamaskowan± pu³apkê.\n\r" );
                check_improve( ch, NULL, gsn_detect_traps, TRUE, 10 );
                af.where = TO_AFFECTS;
                af.type = gsn_detect_traps;
                af.level = 0;
                af.duration = 24;                 af.rt_duration = 0;
                af.location = APPLY_NONE;
                af.modifier = trap->vnum;
                af.bitvector = &AFF_NONE;
                affect_to_char( ch, &af, NULL, FALSE );
            }
            else
            {
                print_char( ch, "Nie znajdujesz nic podejrzanego.\n\r" );
                check_improve( ch, NULL, gsn_detect_traps, FALSE, 14 );
            }

            act( "$n ostro¿nie ogl±da wyj¶cie na $t.", ch, dir_name[ door ], NULL, TO_ROOM );
            return;
        }

        send_to_char( "Gdzie dok³adnie chcesz szukaæ pu³apek?\n\r", ch );
        return;
    }
    else if ( !str_prefix( arg1, "rozbroj" ) || !str_prefix( arg1, "disarm" ) )
    {
        if ( ( skill = get_skill( ch, gsn_disarm_traps ) ) <= 0 )
        {
            send_to_char( "Nie wiesz jak siê do tego zabraæ.\n\r", ch );
            return;
        }

        /*
           zdejmujê to przyciêcie skilla
Gurthg: 2003-09-25
skill = skill / 2;
         */

        switch ( GET_RACE( ch ) )
        {
            case 2: skill +=  5;	break; // elf
            case 4: skill += 10;	break; // gnome
            case 6: skill += 15;	break; // hobbit
            default: break;
        }

        switch ( get_curr_stat_deprecated( ch, STAT_DEX ) )
        {
            case  6: skill -= 50;	break;
            case  7: skill -= 40;	break;
            case  8: skill -= 30;	break;
            case  9: skill -= 25;	break;
            case 10: skill -= 20;	break;
            case 11: skill -= 15;	break;
            case 12:
            case 13: skill -= 10;	break;
            case 14:
            case 15: skill -= 5;	break;
            case 16:
            case 17:
            case 18:
            case 19: break;
            case 20:
            case 21: skill += 5;	break;
            case 22:
            case 23: skill += 10;	break;
            case 24:
            case 25: skill += 15;	break;
            case 26:
            case 27: skill += 20;	break;
            case 28: skill += 25;	break;
            default: break;
        }
        if (ch->class == CLASS_BARD )
            skill -= skill/4;

        argument = one_argument( argument, arg2 );

        WAIT_STATE( ch, skill_table[ gsn_disarm_traps ].beats );

        if ( arg2[ 0 ] == '\0' )
        {
            if ( ch->in_room->trap <= 0 ||
                    !EXT_IS_SET( ch->in_room->room_flags, ROOM_TRAP ) ||
                    ( trap = get_trap_index( ch->in_room->trap ) ) == NULL ||
                    !trap->active )
            {
                print_char( ch, "Spróbuj mo¿e pierw zlokalizowaæ t± pu³apkê, któr± chcesz rozbrajaæ.\n\r" );
                act( "$n ostro¿nie rozgl±da siê.", ch, NULL, NULL, TO_ROOM );
                return;
            }

            afuj = affect_find(ch->affected, gsn_detect_traps );
            if ( !afuj || afuj->modifier != trap->vnum )
            {
                print_char( ch, "Spróbuj mo¿e pierw zlokalizowaæ t± pu³apkê, któr± chcesz rozbrajaæ.\n\r" );
                act( "$n ostro¿nie rozgl±da siê.", ch, NULL, NULL, TO_ROOM );
                return;
            }

            trap_level = trap->level;

            skill = URANGE( 1, skill - ( trap_level * 5 ), 95 );

            if ( !supermob )
                init_supermob();

            char_from_room ( supermob );
            char_to_room( supermob, ch->in_room );

            if ( number_percent() < skill && check_limits( trap, ch ) )
            {
                EXT_REMOVE_BIT( ch->in_room->room_flags, ROOM_TRAP );

                sprintf( buf, "trap%d", trap->vnum );
                create_rprog_env( buf, trap->disarmed, ch->in_room, ch, NULL, NULL, NULL, NULL );
                program_flow();
                release_supermob();

                check_improve( ch, NULL, gsn_disarm_traps, TRUE, 10 );

                if ( afuj ) affect_strip( ch, gsn_detect_traps );

                exp = ( 75 + number_range( 1, 50 ) ) * exp_table[ URANGE( 1, 2 * trap_level, 20 ) ] / 100;
                exp = gain_exp( ch, exp, TRUE );
                message_when_experience_gain( ch, exp );

                if ( !IS_NPC( ch ) )
                {
                    append_file_format_daily
                        (
                         ch,
                         EXP_LOG_FILE,
                         "-> %d (%d) - exp za rozbrojenie pu³apki [%d] w roomie",
                         ch->exp,
                         exp,
                         trap->vnum
                        );
                }
            }
            else
            {
                sprintf( buf, "trap%d", trap->vnum );
                create_rprog_env( buf, trap->failed_disarm, ch->in_room, ch, NULL, NULL, NULL, NULL );
                program_flow();
                release_supermob();

                check_improve( ch, NULL, gsn_disarm_traps, FALSE, 14 );
            }
            return;
        }

        if ( ( obj = get_obj_here( ch, arg2 ) ) != NULL )
        {
            if ( obj->trap <= 0 ||
                    ( trap = get_trap_index( obj->trap ) ) == NULL ||
                    !trap->active )
            {
                print_char( ch, "Spróbuj mo¿e pierw zlokalizowaæ t± pu³apkê, któr± chcesz rozbrajaæ.\n\r" );
                act( "$n ostro¿nie rozgl±da siê.", ch, NULL, NULL, TO_ROOM );
                return;
            }

            afuj = affect_find(ch->affected, gsn_detect_traps );
            if ( !afuj || afuj->modifier != trap->vnum )
            {
                print_char( ch, "Spróbuj mo¿e pierw zlokalizowaæ t± pu³apkê, któr± chcesz rozbrajaæ.\n\r" );
                act( "$n ostro¿nie rozgl±da siê.", ch, NULL, NULL, TO_ROOM );
                return;
            }

            trap_level = trap->level;

            skill = URANGE( 1, skill - ( trap_level * 5 ), 95 );

            if ( !supermob )
                init_supermob();

            char_from_room ( supermob );
            char_to_room( supermob, ch->in_room );


            if ( number_percent() < skill && check_limits( trap, ch ) )
            {
                obj->trap = 0;

                sprintf( buf, "trap%d", trap->vnum );
                create_rprog_env( buf, trap->disarmed, ch->in_room, ch, obj, NULL, NULL, NULL );
                program_flow();
                release_supermob();

                check_improve( ch, NULL, gsn_disarm_traps, TRUE, 10 );

                if ( afuj ) affect_strip( ch, gsn_detect_traps );

                exp = ( 75 + number_range( 1, 50 ) ) * exp_table[ URANGE( 1, 2 * trap_level, 20 ) ] / 100;
                exp = gain_exp( ch, exp, TRUE );
                message_when_experience_gain( ch, exp );

                if ( !IS_NPC( ch ) )
                {
                    append_file_format_daily
                        (
                         ch,
                         EXP_LOG_FILE,
                         "-> %d (%d) - exp za rozbrojenie pu³apki [%d] na obiekcie [%d]",
                         ch->exp,
                         exp,
                         trap->vnum,
                         obj->pIndexData->vnum
                        );
                }
                return;
            }
            else
            {
                sprintf( buf, "trap%d", trap->vnum );
                create_rprog_env( buf, trap->failed_disarm, ch->in_room, ch, obj, NULL, NULL, NULL );
                program_flow();
                release_supermob();

                check_improve( ch, NULL, gsn_disarm_traps, FALSE, 14 );

                return;
            }

            return;
        }

        if ( ( door = find_door( ch, arg2 ) ) >= 0 )
        {
            if ( ( pexit = ch->in_room->exit[ door ] ) == NULL	||
                    !IS_SET( pexit->exit_info, EX_ISDOOR ) )
            {
                send_to_char( "Co chcesz robroiæ?\n\r", ch );
                return;
            }

            if ( pexit->trap <= 0 ||
                    !IS_SET( pexit->exit_info, EX_TRAP ) ||
                    ( trap = get_trap_index( pexit->trap ) ) == NULL ||
                    !trap->active )
            {
                print_char( ch, "Spróbuj mo¿e pierw zlokalizowaæ t± pu³apkê, któr± chcesz rozbrajaæ.\n\r" );
                act( "$n ostro¿nie rozgl±da siê.", ch, NULL, NULL, TO_ROOM );
                return;
            }

            afuj = affect_find(ch->affected, gsn_detect_traps );
            if ( !afuj || afuj->modifier != trap->vnum )
            {
                print_char( ch, "Spróbuj mo¿e pierw zlokalizowaæ t± pu³apkê, któr± chcesz rozbrajaæ.\n\r" );
                act( "$n ostro¿nie rozgl±da siê.", ch, NULL, NULL, TO_ROOM );
                return;
            }

            trap_level = trap->level;

            skill = URANGE( 1, skill - ( trap_level * 5 ), 95 );

            if ( !supermob )
                init_supermob();

            char_from_room ( supermob );
            char_to_room( supermob, ch->in_room );

            if ( number_percent() < skill && check_limits( trap, ch ) )
            {
                REMOVE_BIT( pexit->exit_info, EX_TRAP );

                sprintf( buf, "trap%d", trap->vnum );
                create_rprog_env( buf, trap->disarmed, ch->in_room, ch, NULL, NULL, NULL, NULL );
                program_flow();
                release_supermob();

                if ( afuj ) affect_strip( ch, gsn_detect_traps );

                exp = ( 75 + number_range( 1, 50 ) ) * exp_table[ URANGE( 1, trap_level, 10 ) ] / 200;
                exp = gain_exp( ch, exp, TRUE );
                message_when_experience_gain( ch, exp );

                if ( !IS_NPC( ch ) )
                {
                    append_file_format_daily
                        (
                         ch,
                         EXP_LOG_FILE,
                         "-> %d (%d) - exp za rozbrojenie pu³apki [%d] na wyj¶ciu",
                         ch->exp,
                         exp,
                         trap->vnum
                        );
                }
                check_improve( ch, NULL, gsn_disarm_traps, TRUE, 10 );
                return;
            }
            else
            {

                sprintf( buf, "trap%d", trap->vnum );
                create_rprog_env( buf, trap->failed_disarm, ch->in_room, ch, NULL, NULL, NULL, NULL );
                program_flow();
                release_supermob();

                check_improve( ch, NULL, gsn_disarm_traps, FALSE, 14 );

                return;
            }
            return;
        }

        send_to_char( "Co chcesz dok³adnie rozbrajaæ?\n\r", ch );
        return;
    }
    else
    {
        send_to_char( "Chcesz szukaæ czy rozbrajaæ pu³apki?\n\r", ch );
        return;
    }
    return;
}

void do_target( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim, *vch;
    int skill, counter = 0;

    if ( ( skill = get_skill( ch, gsn_target ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( !ch->fighting )
    {
        send_to_char( "Przecie¿ nie walczysz!\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Kto ma byæ twoim nowym celem?\n\r", ch );
        return;
    }
    else if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        return;
    }

    if ( ch == victim )
    {
        send_to_char( "Hmm, nie jest to najlepszy pomys³.\n\r", ch );
        return;
    }

    if ( victim == ch->fighting )
    {
        print_char( ch, "Przecie¿ juz walczysz z %s!\n\r", ch->fighting->name5 );
        return;
    }

    if ( is_safe( ch, victim ) )
        return;

    WAIT_STATE( ch, PULSE_VIOLENCE );

    if ( victim->fighting == ch )
        skill += 30;

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
        if ( vch->fighting && vch->fighting == ch && vch != victim )
            counter++;

    skill -= counter * 10;

    skill += get_curr_stat_deprecated(ch,STAT_DEX) - 10;

    skill = URANGE( 1, skill, 95 );

    if ( number_percent() < skill )
    {
        act( "{5B³yskawicznie odskakujesz od $Z i zaczynasz atakowaæ $t.{x", ch, PERS4( victim, ch ), ch->fighting, TO_CHAR );
        act( "{5$n b³yskawicznie odskakuje od $t i z okrzykiem rzuca siê na CIEBIE.{x", ch, PERS2( ch->fighting, victim ), victim, TO_VICT );
        act( "{5$n b³yskawicznie odskakuje od ciebie i z okrzykiem rzuca siê na $t.{x", ch, PERS4( victim, ch->fighting ), ch->fighting, TO_VICT );

        for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
        {
            if ( vch != ch && vch != victim && vch != ch->fighting && IS_AWAKE( vch ) )
            {
                print_char( vch, "{5%s b³yskawicznie odskakuje od %s i z okrzykiem rzuca siê na %s.{x\n\r",
                        capitalize( PERS( ch, vch) ),
                        PERS2( ch->fighting, vch),
                        PERS4( victim, vch ) );
            }
        }

        stop_fighting( ch, FALSE );
        set_fighting( ch, victim );
        damage( ch, victim, 0, 0, DAM_NONE, FALSE );
        check_improve( ch, NULL, gsn_target, TRUE, 10 );
        return;
    }
    else
    {
        print_char( ch, "{5Próbujesz zmieniæ swój cel ataku, jednak nie udaje ci sie.{x\n\r" );
        act( "{5$n próbuje zmieniæ swój cel ataku, jednak bezskutecznie.{x", ch, NULL, NULL, TO_ROOM );
        check_improve( ch, NULL, gsn_target, FALSE, 15 );
        if ( !victim->fighting && IS_NPC( victim ) )
            multi_hit( victim, ch, TYPE_UNDEFINED );

        return;
    }
    return;
}

void do_smite( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * vch;
    OBJ_DATA *wield;
    OBJ_DATA * obj;
    AFFECT_DATA *paf;
    int skill, chance, holy_chance_mod, delay_mod, charisma_mod = (get_curr_stat_deprecated(ch, STAT_CHA) - 18)*2;
    bool smite_failed = FALSE;
    delay_mod = 0;
    holy_chance_mod = 0;
    if ( ch->class == CLASS_PALADIN || EXT_IS_SET(ch->act, ACT_PALADIN ))
    {
        if ( ( skill = get_skill( ch, gsn_smite ) ) == 0 )
        {
            send_to_char( "Huh?\n\r", ch );
            return;
        }

        if ( ch->mount )
        {
            send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
            return;
        }

        if ( ch->counter[ 4 ] > 0 )
        {
            send_to_char( "Nie mo¿esz siê wystarczaj±co skoncentrowaæ.\n\r", ch );
            return;
        }

        if ( ( wield = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
        {
            send_to_char( "Postaraj siê o jak±¶ broñ!\n\r", ch );
            return;
        }

        if ( argument[ 0 ] == '\0' )
        {
            if ( ch->fighting )
                vch = ch->fighting;
            else
            {
                send_to_char( "Kogo chcesz zaatakowaæ?\n\r", ch );
                return;
            }
        }
        else if ( ( vch = get_char_room( ch, argument ) ) == NULL )
        {
            send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
            add_wait_after_miss(ch, 2);
            return;
        }

        if ( ch == vch )
            return;

        if ( is_safe( ch, vch ) )
            return;
        //send_to_char( "Sprawdzanie ostatniej modlitwy. \n\r", ch );
        if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
        {
            ch->counter[ 4 ] = 3;
            if ( paf->modifier > 48 )
            {
                send_to_char( "Nie czujesz w sobie niezbêdnej boskiej mocy, aby tego dokonac!\n\r", ch );
                act( "$n koncentruje siê przez chwile i opuszcza bezsilnie rêce.", ch, NULL, NULL, TO_ROOM );
                WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
                return;
            }
            else if ( paf->modifier > 36 )
            {
                send_to_char( "Wykrzykujesz imiona swoich bogow i wyprowadzasz atak, jednak brak ci dosyæ ¶wiêtej mocy, aby mo¿na by³o nazwaæ ten atak potê¿nym. \n \r", ch );
                act( "$n wykrzykuje imiona swoich bogów i wyprowadza niezbyt mocny, ¿eby niepowiedzieæ zwyk³y atak w którym najwyra¼niej brak ¶wiêtej si³y.", ch, NULL, NULL, TO_ROOM );

                multi_hit( ch, vch, TYPE_UNDEFINED );
                WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
                return;
            }
        }
        //bonusy i minusy od bycia dobrym/zlym paladynem - Raszer, Rasz_holy, patrz tez w fight.c
        //send_to_char( "Sprawdzanie obecnosci puli.\n\r", ch );
        if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
        {
             holy_chance_mod = paf->modifier;
             //send_to_char( "Delay mody. \n\r", ch );
             //dobry pal
              if (paf->modifier > 90 && number_percent() >  paf->modifier/3)
               {
                 delay_mod--;
                 paf->modifier-= 3;
               };

             if (paf->modifier > 45 && number_percent() >  paf->modifier/2)
               {
                 delay_mod--;
                 paf->modifier-= 3;
               };

             if (paf->modifier > 25 && number_percent() >  (paf->modifier * 3)/2)
               {
                 delay_mod--;
                 paf->modifier-= 3;
               };


             //zly pal
         //   send_to_char( "Opoznienia. \n\r", ch );
            //opoznienia
            if (paf->modifier < -25 && number_percent() >  (paf->modifier*3)/2) delay_mod++;
            if (paf->modifier < -45 && number_percent() >  (paf->modifier*3)/2) delay_mod++;
            if (paf->modifier < -65 && number_percent() >  (paf->modifier*3)/2) delay_mod++;
            //szansa na nieudanie
          //  send_to_char( "Szansa na nieudanie. \n\r", ch );
            if (paf->modifier < -50 && number_percent() >  paf->modifier*2/3) smite_failed = TRUE;
            //szansa na disarm
          //  send_to_char( "Szansa na disarm. \n\r", ch );
            if (paf->modifier < - 70 && number_percent() >  paf->modifier*2/3)
            {
               	if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
	               	obj = get_eq_char( ch, WEAR_SECOND );

	                if ( obj == NULL || IS_OBJ_STAT( obj, ITEM_NOREMOVE ) || IS_OBJ_STAT( obj, ITEM_NODROP ) || IS_OBJ_STAT( obj, ITEM_INVENTORY ))
                       {
                      //   send_to_char( "Smite nieudany. \n\r", ch );
                         smite_failed = TRUE;
                       }else
                       {
                         obj_from_char( obj );
                         obj_to_room( obj, ch->in_room );
                        // send_to_char( "Disarm za bycie niegrzecznym. \n\r", ch );
                         act( "{5Koncentrujesz siê chwilê, zbieraj±c bosk± energiê, co¶ jednak dzieje siê nie tak. Przez chwilê odczuwasz wyra¼nie obecno¶æ swojego Boga i jego gniew. Twój umys³ ogarnia mg³a i na chwilê tracisz czucie w rêkach, upuszczaj±c swoj± broñ!{x",	ch, NULL, vch, TO_CHAR );
                    act( "{5$n koncentruje siê chwilê, jednak sta³o siê co¶ dziwnego, z przera¿eniem otwiera oczy, a broñ wypada mu z rêki.{x", ch, NULL, vch, TO_VICT );
                    act( "{5$n koncentruje siê chwilê, jednak sta³o siê co¶ dziwnego, z przera¿eniem otwiera oczy, a broñ wypada mu z rêki.{x", ch, NULL, vch, TO_NOTVICT );
                       WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
                       ch->counter[ 4 ] = 6;
                       return;
                       }


            }



        }


        chance = skill + charisma_mod + holy_chance_mod/3;
        chance = URANGE( 20, chance , 90 );

        if (( number_percent() < chance || !can_move( vch ) ) )
        {
            ch->counter[ 4 ] = 3 + delay_mod;
            if(IS_EVIL(vch))
            {
                if (delay_mod > 0) send_to_char( "{5Czujesz, jak wyczerpuje siê w tobie ¶wiêta moc.{x\n\r", ch );
                if (delay_mod < 0) send_to_char( "{5Czujesz jak zgromadzona ¶wiêta moc przywraca ci czê¶æ si³.{x\n\r", ch );
                //send_to_char( "Koniec smite, bedzie  onehit. /r /n", ch );
                check_improve( ch, vch, gsn_smite, TRUE, 10 );
                one_hit( ch, vch, gsn_smite, FALSE );
            }
            else
            {
                send_to_char( "{5Próbujesz siê skoncentrowaæ ale moce dobra lituj± siê nad twoj± ofiar±.{x\n\r", ch );
                if ( ch->counter[ 4 ] < 3) ch->counter[ 4 ] = 3; //nie bedziemy dawac plusow jak sie smituje nie-evili
                return;
            }
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
        }
        else
        {
            if (delay_mod > 1) delay_mod = 1;
            if (delay_mod == 1) delay_mod = 0;
            ch->counter[ 4 ] = 1 + delay_mod;
            if (delay_mod > 0) send_to_char( "{5Czujesz, jak wyczerpuje siê w tobie ¶wiêta moc.{x\n\r", ch );
            if (delay_mod < 0) send_to_char( "{5Czujesz jak zgromadzona ¶wiêta moc przywraca ci czê¶æ si³.{x\n\r", ch );

            switch ( number_range( 1, 3 ) )
            {
                case 1:
                    act( "{5Koncentrujesz siê chwilê i z potê¿n± si³± wyprowadzasz cios w kierunku $Z, $E w ostatniej chwili robi unik.{x",	ch, NULL, vch, TO_CHAR );
                    act( "{5$n koncentruje siê chwilê i z potê¿n± si³± wyprowadza cios w twoim kierunku, ale w ostatniej chwili robisz unik.{x", ch, NULL, vch, TO_VICT );
                    act( "{5$n koncentruje siê chwilê i z potê¿n± si³± wyprowadza cios w kierunku $Z, ale $N w ostatniej chwili robi unik.{x", ch, NULL, vch, TO_NOTVICT );
                    break;
                case 2:
                    act( "{5Na chwilê zatrzymujesz siê chc±c wyprowadziæ silny cios w kierunku $Z, jednak $E uprzedza twoje zamiary i wykonuje zwinny unik.{x", ch, NULL, vch, TO_CHAR );
                    if ( ch->sex == 2 )
                        act( "{5$n na chwilê zatrzymuje siê chc±c wyprowadziæ silny cios w twoim kierunku, jednak uprzedzasz jej zamiar i wykonujesz zwinny unik.{x", ch, NULL, vch, TO_VICT );
                    else
                        act( "{5$n na chwilê zatrzymuje siê chc±c wyprowadziæ silny cios w twoim kierunku, jednak uprzedzasz jego zamiar i wykonujesz zwinny unik.{x", ch, NULL, vch, TO_VICT );
                    if ( ch->sex == 2 )
                        act( "{5$n na chwilê zatrzymuje siê chc±c wyprowadziæ silny cios w kierunku $Z, jednak $N uprzedza jej zamiar i wykonuje zwinny unik.{x", ch, NULL, vch, TO_NOTVICT );
                    else
                        act( "{5$n na chwilê zatrzymuje siê chc±c wyprowadziæ silny cios w kierunku $Z, jednak $N uprzedza jego zamiar i wykonuje zwinny unik.{x", ch, NULL, vch, TO_NOTVICT );
                    break;
                case 3:
                    act( "{5Wykrzykuj±c imiona swoich bogów rzucasz siê na $C, jednak w ostatniej chwili potykasz siê i chybiasz haniebnie.{x", ch, NULL, vch, TO_CHAR );
                    act( "{5$n wykrzykuj±c imiona swoich bogów rzuca siê na ciebie, jednak w ostatniej chwili potyka siê i chybia haniebnie.{x", ch, NULL, vch, TO_VICT );
                    act( "{5$n wykrzykuj±c imiona swoich bogów rzuca siê na $C, jednak w ostatniej chwili potyka siê i chybia haniebnie.{x", ch, NULL, vch, TO_NOTVICT );
                    break;
            }
            //send_to_char( "Koniec smite, bedzie  multihit. /r /n", ch );
            multi_hit( ch, vch, TYPE_UNDEFINED );
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            check_improve( ch, vch, gsn_smite, FALSE, 14 );
        }
    }
    else
    {

        if ( ( skill = get_skill( ch, gsn_smite_good ) ) == 0 )
        {
            send_to_char( "Huh?\n\r", ch );
            return;
        }

        if ( ch->mount )
        {
            send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
            return;
        }

        if ( ch->counter[ 4 ] > 0 )
        {
            send_to_char( "Nie mo¿esz siê wystarczaj±co skoncentrowaæ.\n\r", ch );
            return;
        }

        if ( ( wield = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
        {
            send_to_char( "Postaraj siê o jak±¶ broñ!\n\r", ch );
            return;
        }

        if ( argument[ 0 ] == '\0' )
        {
            if ( ch->fighting )
                vch = ch->fighting;
            else
            {
                send_to_char( "Kogo chcesz zaatakowaæ?\n\r", ch );
                return;
            }
        }
        else if ( ( vch = get_char_room( ch, argument ) ) == NULL )
        {
            send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
            add_wait_after_miss(ch, 2);
            return;
        }

        if ( ch == vch )
            return;

        if ( is_safe( ch, vch ) )
            return;

        chance = skill + charisma_mod;

        chance = URANGE( 10, chance, 95 );

        if (( number_percent() < chance || !can_move( vch ) ) )
        {
            ch->counter[ 4 ] = 3;
            if(IS_GOOD(vch))
            {

                one_hit( ch, vch, gsn_smite_good, FALSE );
                check_improve( ch, vch, gsn_smite_good, TRUE, 10 );
            }
            else
            {
                send_to_char( "{5Próbujesz siê skoncentrowaæ, ale ¼li bogowie nie dziel± siê z tob± sw± moc±.{x\n\r", ch );
                return;
            }
            WAIT_STATE( ch, skill_table[ gsn_smite_good ].beats );
        }
        else
        {
            ch->counter[ 4 ] = 1;
            switch ( number_range( 1, 3 ) )
            {
                case 1:
                    act( "{5Koncentrujesz siê chwilê i z potê¿n± si³± wyprowadzasz cios w kierunku $Z, $E w ostatniej chwili robi unik.{x",	ch, NULL, vch, TO_CHAR );
                    act( "{5$n koncentruje siê chwilê i z potê¿n± si³± wyprowadza cios w twoim kierunku, ale w ostatniej chwili robisz unik.{x", ch, NULL, vch, TO_VICT );
                    act( "{5$n koncentruje siê chwilê i z potê¿n± si³± wyprowadza cios w kierunku $Z, ale $N w ostatniej chwili robi unik.{x", ch, NULL, vch, TO_NOTVICT );
                    break;
                case 2:
                    act( "{5Na chwilê zatrzymujesz siê chc±c wyprowadziæ silny cios w kierunku $Z, jednak $E uprzedza twoje zamiary i wykonuje zwinny unik.{x", ch, NULL, vch, TO_CHAR );
                    if ( ch->sex == 2 )
                        act( "{5$n na chwilê zatrzymuje siê chc±c wyprowadziæ silny cios w twoim kierunku, jednak uprzedzasz jej zamiar i wykonujesz zwinny unik.{x", ch, NULL, vch, TO_VICT );
                    else
                        act( "{5$n na chwilê zatrzymuje siê chc±c wyprowadziæ silny cios w twoim kierunku, jednak uprzedzasz jego zamiar i wykonujesz zwinny unik.{x", ch, NULL, vch, TO_VICT );
                    if ( ch->sex == 2 )
                        act( "{5$n na chwilê zatrzymuje siê chc±c wyprowadziæ silny cios w kierunku $Z, jednak $N uprzedza jej zamiar i wykonuje zwinny unik.{x", ch, NULL, vch, TO_NOTVICT );
                    else
                        act( "{5$n na chwilê zatrzymuje siê chc±c wyprowadziæ silny cios w kierunku $Z, jednak $N uprzedza jego zamiar i wykonuje zwinny unik.{x", ch, NULL, vch, TO_NOTVICT );
                    break;
                case 3:
                    act( "{5Wykrzykuj±c imiona swoich mrocznych bogów rzucasz siê na $C, jednak w ostatniej chwili potykasz siê i chybiasz haniebnie.{x", ch, NULL, vch, TO_CHAR );
                    act( "{5$n wykrzykuj±c imiona swoich mrocznych bogów rzuca siê na ciebie, jednak w ostatniej chwili potyka siê i chybia haniebnie.{x", ch, NULL, vch, TO_VICT );
                    act( "{5$n wykrzykuj±c imiona swoich mrocznych bogów rzuca siê na $C, jednak w ostatniej chwili potyka siê i chybia haniebnie.{x", ch, NULL, vch, TO_NOTVICT );
                    break;
            }

            multi_hit( ch, vch, TYPE_UNDEFINED );
            WAIT_STATE( ch, skill_table[ gsn_smite_good ].beats );
            check_improve( ch, vch, gsn_smite_good, FALSE, 14 );
        }
    }


    return;
}

void do_turn( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * vch, *vch_next;
    int skill, chance, level_dif, duration;
    bool failed = FALSE;
    AFFECT_DATA af, *paf;
    OBJ_DATA *item_turner = NULL, *obj;
    bool turned = FALSE, check;
    int luck = get_curr_stat( ch, STAT_LUC ), charisma_mod, dam, wait_state, pskill, holy_lev_mod = 0, holy_counter_mod = 0, holy_dam_mod = 100;
    int lev_for_kill = 18, lev_for_paralyze = 22, base_dam_mod = 40;

    if ( ( skill = get_skill( ch, gsn_turn ) ) == 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->mount )
    {
        send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
        return;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
        if ( obj->item_type == ITEM_TURN )
        {
            switch ( obj->wear_loc )
            {
                case WEAR_HOLD:
                case WEAR_NECK_1:
                case WEAR_NECK_2:
                    item_turner = obj;
                    break;
                default: break;
            }
            if ( item_turner )
                break;
        }
    }

    if ( !item_turner )
    {
        send_to_char( "Postaraj siê o jaki¶ symbol wiary.\n\r", ch );
        return;
    }

    if ( !IS_NPC(ch) && item_turner->wear_loc != WEAR_HOLD && get_hand_slots( ch, WEAR_NONE ) >= 2 )
    {
        send_to_char( "Musisz mieæ woln± rêkê aby to uczyniæ.\n\r", ch );
        return;
    }

    /* dajemy standardowy tekst jak go¶æ jest pod wp³ywem 'silence' */
    if ( IS_AFFECTED( ch, AFF_SILENCE ) )
    {
        send_to_char( AFF_SILENCE_TEXT, ch );
        return ;
    }

    skill = URANGE( 0, skill + item_turner->value[ 0 ], 110 );
    if ( IS_AFFECTED( ch, AFF_HIDE ) ) affect_strip( ch, gsn_hide );

    strip_invis( ch, TRUE, TRUE );
   if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
        if ( paf->modifier > 48 )
        {

            send_to_char( "Sciskasz d³oñ na symbolu, jednak nie czujesz w sobie nawet iskry ¶wiêtej mocy niezbêdnej do odpêdzania martwiaków!\n\r", ch );
            act( "$n sciska w d³oni symbol swej wiary koncentruj±c siê przez chwile, jednak nic siê nie dzieje.", ch, NULL, NULL, TO_ROOM );
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            return;
        }
        else if ( paf->modifier > 36 )
        {
            send_to_char( "Dotykaj±c symbolu wiary od razu wyczuwasz, jak ma³o w tobie ¶wiêtej mocy. \n\r", ch );
            skill /= 2;
        }
    }


    switch( ch->class)
    {
        case CLASS_CLERIC:
            charisma_mod = get_curr_stat( ch, STAT_CHA) - 66;
            break;
        case CLASS_PALADIN:
            charisma_mod = get_curr_stat( ch, STAT_CHA) - 108;
            break;
        default:
            charisma_mod = 0;
    }

    //chance = URANGE( 5, skill/2 + get_curr_stat_deprecated( ch, STAT_WIS ) + luck / 3, 90 );
    chance = URANGE( 5, skill/2 + (get_curr_stat( ch, STAT_WIS )-60)/6 + charisma_mod*2/6 + (luck-10)/6, 90 );

    if ( number_percent() > chance  )
    {
        failed = TRUE;
    }
    //Nowy wp³yw holy_prayera na turna Rasz_pray, Raszer
    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
      {
         pskill = get_skill( ch, gsn_holy_prayer);
         if (failed == FALSE)
       {

         if ( paf->modifier > 70 && number_percent() < pskill/3)
         {
              holy_counter_mod = -1;
              paf->modifier -= 5;

         }
         if ( paf->modifier > 50 && number_percent() < pskill/2)
         {
              holy_lev_mod += 3;
              paf->modifier -= 10;
         }

         if ( paf->modifier > 30 && number_percent() < pskill/2)
         {
              holy_dam_mod = 135;
              paf->modifier -= 10;
         }


         if ( paf->modifier < -30 && number_percent() < pskill)
         {
              holy_dam_mod = 50;
         }
          if ( paf->modifier < -90 && number_percent() < pskill)
         {
             holy_lev_mod = -4;

         }
       }
         //te minusy nawet przy nieudanym
          if ( paf->modifier < -30 )
         {
             holy_counter_mod = 2;
         }


          if ( paf->modifier < -60 && number_percent() < pskill/2)
         {
             if ( !IS_OBJ_STAT(item_turner, ITEM_UNDESTRUCTABLE))
             {
                  send_to_char( "Sciskasz d³oñ na symbolu, jednak po chwili twe serce wype³nia strach, a symbol rozsypuje siê w proch.\n\r", ch );
                  act( "$n sciska w d³oni symbol swej wiary koncentruj±c siê przez chwile, po chwili na jego twarzy pojawia siê grymas przera¿enia, a symbol rozsypuje siê w proch. \n\r", ch, NULL, NULL, TO_ROOM );
                  extract_obj( obj );
                  WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
                  return;
             }

         }

      }


    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) )
        ;
    else
    {
        if ( ( paf = affect_find( ch->affected, gsn_turn ) ) == NULL )
        {
            duration = 24;
            if ( number_range( 0, luck ) > 90 ) duration -= 1;
            if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) < 3 ) duration += 2;

            af.where = TO_AFFECTS;
            af.type = gsn_turn;
            af.level = 1;
            af.location = APPLY_NONE;
            af.duration = duration;             af.rt_duration = 0;
            af.modifier = 0;
            af.bitvector = &AFF_NONE;
            affect_to_char( ch, &af, NULL, FALSE );
        }
        else
        {
            paf->level += 1 + holy_counter_mod;
            check = FALSE;
            if ( ( !check && paf->level > ch->level / 5 ) || paf->level > ch->level / 3 )
            {
                send_to_char( "Nie czujesz siê na si³ach by to uczyniæ.\n\r", ch );
                return;
            }
        }
    }

    SET_BIT( ch->fight_data, FIGHT_CASTING );

    /**
     * add wait state
     */
    wait_state = skill_table[ gsn_turn ].beats;
    WAIT_STATE( ch, wait_state );

    if ( !IS_EVIL(ch) )
    {
        print_char( ch, "{5Szepcz±c s³owa modlitwy zaciskasz d³oñ na %s.{x\n\r", item_turner->name6 );
        act( "{5$n cicho szepcze modlitwê ¶ciskaj±c w rêku $h.{x", ch, item_turner, NULL, TO_ROOM );
    }
    else
    {
        print_char( ch, "{5G³o¶no wymawiasz mroczne s³owa, trzymaj±c w wyci±gnietej rece %s.{x\n\r", item_turner->name4 );
        act( "{5$n wymawia mroczno brzmi±ce s³owa, w wyciagnietej przed siebie rêce mocno ¶ciska $h.{x", ch, item_turner, NULL, TO_ROOM );
    }

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
        vch_next = vch->next_in_room;

        if ( !is_undead(vch) || is_safe( ch, vch ) )
            continue;

        if ( !turned )
        {
            if ( failed )
                check_improve( ch, NULL, gsn_turn, FALSE, number_range(10,20) );
            else
                check_improve( ch, NULL, gsn_turn, TRUE, 12 );
            turned = TRUE;
        }

        if ( failed || vch->level > ch->level + 4 + holy_lev_mod)
        {
            if ( !vch->fighting && can_move( vch ) )
            {
                vch->fighting = ch;
                act( "{5Rzucasz siê z krzykiem na $c.{x", ch, NULL, vch, TO_VICT );
                act( "{5$N rzuca siê z krzykiem na $c.{x", ch, NULL, vch, TO_NOTVICT );
                print_char( ch, "{5%s rzuca siê na ciebie z krzykiem.{x\n\r", capitalize( vch->short_descr ) );
            }
            continue;
        }
        else
        {
            level_dif = ch->level - vch->level + holy_lev_mod;
            //unicestwienie
            if ( vch->level < (lev_for_kill + holy_lev_mod) && number_percent() - charisma_mod/2/6 < URANGE( 0, 10 * UMAX( level_dif + 1, 0 ), 50 ) )
            {
                if ( ch->alignment > -750 )
                    act( "{5W krótkim rozb³ysku ¶wiat³a $n zamienia siê w kupkê dymi±cego popio³u.{x", vch, NULL, ch, TO_ROOM );
                else
                    act( "{5$n rzuca siê do ucieczki, po chwili rozb³yskuje ponuro i znika w k³êbach czarnego dymu.{x", vch, NULL, ch, TO_ROOM );

                //taka glupota, zeby nie dostal expa za zadanie obrazen
                vch->hit = 1;
                damage( ch, vch, 20, gsn_turn, DAM_NONE, FALSE );
                continue;
            }
            //unieruchomienie
            else if ( vch->level < (lev_for_paralyze + holy_lev_mod) && !IS_AFFECTED( vch, AFF_PARALYZE ) && number_percent() - charisma_mod/2/6 < 20 + URANGE( 0, 10 * UMAX( level_dif + 1, 0 ), 50 ) )
            {
                AFFECT_DATA af;
                af.where = TO_AFFECTS;
                af.type = gsn_holdperson;
                af.level = 40;
                af.location = APPLY_NONE;
                af.duration = number_range( 3, 7 );                 af.rt_duration = 0;
                af.modifier = 0;
                af.bitvector = &AFF_PARALYZE;
                affect_to_char( vch, &af, "turn undead - unieruchomienie", TRUE );

                if ( ch->alignment > -750 )
                    act( "{5Wokó³ $z tworzy siê bia³a aura ca³kowicie $m unieruchamiaj±c.{x", vch, NULL, NULL, TO_ROOM );
                else
                    act( "{5$n krzyczy przera¼liwie, po chwili ca³kowicie zamiera w przera¿eniu.{x", vch, NULL, NULL, TO_ROOM );
                continue;
            }
            else
            {
                dam = ( ( base_dam_mod + (3*skill)/4 + charisma_mod/6 ) * number_range( 4, 10 ) * ch->level / 100 ) + item_turner->value[ 1 ];
                dam = luck_dam_mod( ch, dam );
                dam *= holy_dam_mod/100;

                if( !IS_NPC( vch ) )
                {
                    if ( vch->class == CLASS_MAG )
                        dam /= 4;
                    else
                        dam /= 2;
                }

                damage( ch, vch, dam, gsn_turn, DAM_HOLY, FALSE );
                act( "{5$n okrywa siê zielon± mg³± wyj±c z bólu.{x", vch, NULL, NULL, TO_ROOM );
                act( "{5$n okrywa ciê zielon± mg³±, wyjesz z bólu!{x", ch, NULL, vch, TO_VICT );
                continue;
            }
        }
    }
    return;
}

//mighty blow - skill barbarzyncy
//-3 hitroll/ +3 damroll
void do_might( CHAR_DATA *ch, char *argument )
{
    int skill, chance;

    if ( ( skill = get_skill( ch, gsn_mighty_blow ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->position < POS_FIGHTING )
    {
        send_to_char( "Wstañ najpierw z ziemi.\n\r", ch );
        return;
    }

    if ( ch->fighting == NULL )
    {
        send_to_char( "Z nikim nie walczysz.\n\r", ch );
        return;
    }

    if ( !get_eq_char( ch, WEAR_WIELD ) )
    {
        send_to_char( "Postaraj siê o jak±¶ broñ!\n\r", ch );
        return;
    }

    chance = URANGE( 30, skill + get_curr_stat_deprecated(ch,STAT_STR) , 95 );

    if ( IS_NPC( ch ) )
        chance += chance / 2;
    else
    {
        if ( ch->condition[ COND_DRUNK ] > DRUNK_FULL )
            chance += 10;
    }

    if ( number_percent() < chance )
    {
        send_to_char( "Przygotowujesz siê do zadania mocnego ciosu.\n\r", ch );
        SET_BIT( ch->fight_data, FIGHT_MIGHTY_BLOW );
        check_improve( ch, NULL, gsn_mighty_blow, TRUE, 40 );
    }
    else
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        check_improve( ch, NULL, gsn_mighty_blow, FALSE, 30 );
    }

    WAIT_STATE( ch, skill_table[ gsn_mighty_blow ].beats );
    return;
}

void do_power( CHAR_DATA *ch, char *argument )
{
    int skill, chance;

    if ( ( skill = get_skill( ch, gsn_power_strike ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->position < POS_FIGHTING )
    {
        send_to_char( "Wstañ najpierw z ziemi.\n\r", ch );
        return;
    }

    if ( ch->fighting == NULL )
    {
        send_to_char( "Z nikim nie walczysz.\n\r", ch );
        return;
    }

    if ( !get_eq_char( ch, WEAR_WIELD ) )
    {
        send_to_char( "Postaraj siê o jak±¶ broñ!\n\r", ch );
        return;
    }

    chance = URANGE( 30, skill + get_curr_stat_deprecated(ch,STAT_STR) - 10, 99 );

    if ( IS_NPC( ch ) )
        chance += chance / 2;
    else
    {
        if ( ch->condition[ COND_DRUNK ] > DRUNK_FULL )
            chance += 10;
    }


    if ( number_percent() < chance )
    {
        send_to_char( "Przygotowujesz siê do zadania pote¿nego ciosu.\n\r", ch );
        SET_BIT( ch->fight_data, FIGHT_POWER_STRIKE );
        check_improve( ch, NULL, gsn_power_strike, TRUE, 50 );
    }
    else
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        check_improve( ch, NULL, gsn_power_strike, FALSE, 40 );
    }

    WAIT_STATE( ch, skill_table[ gsn_power_strike ].beats );
    return;
}

void do_critical( CHAR_DATA *ch, char *argument )
{
    int skill, chance;

    if ( ( skill = get_skill( ch, gsn_critical_strike ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->position < POS_FIGHTING )
    {
        send_to_char( "Wstañ najpierw z ziemi.\n\r", ch );
        return;
    }

    if ( ch->fighting == NULL )
    {
        send_to_char( "Z nikim nie walczysz.\n\r", ch );
        return;
    }

    if ( !get_eq_char( ch, WEAR_WIELD ) )
    {
        send_to_char( "Postaraj siê o jak±¶ broñ!\n\r", ch );
        return;
    }

    chance = URANGE( 20, skill + get_curr_stat_deprecated(ch,STAT_STR) - 18, 99 );

    if ( IS_NPC( ch ) )
        chance += chance / 2;
    else
    {
        if ( ch->condition[ COND_DRUNK ] > DRUNK_FULL )
            chance += 10;
    }

    if ( number_percent() < chance )
    {
        send_to_char( "Bêdziesz próbowa<&³/³a/³o> wyprowadziæ dewastuj±cy atak.\n\r", ch );
        SET_BIT( ch->fight_data, FIGHT_CRITICAL_STRIKE );
        check_improve( ch, NULL, gsn_critical_strike, TRUE, 60 );
    }
    else
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        check_improve( ch, NULL, gsn_critical_strike, FALSE, 50 );
    }

    WAIT_STATE( ch, skill_table[ gsn_critical_strike ].beats );
    return;
}

void do_damage( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * vch;
    int chance;

    if ( ( chance = get_skill( ch, gsn_damage_reduction ) ) == 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( !ch->fighting )
    {
        send_to_char( "Przecie¿ nie walczysz.\n\r", ch );
        return;
    }

    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
        if ( vch->fighting == ch )
            break;
    }

    if ( !vch )
    {
        send_to_char( "Nie jeste¶ nara¿on<&y/a/e> na ciosy, wiêc nie ma za bardzo czego redukowaæ.\n\r", ch );
        return;
    }

    if ( is_affected( ch, gsn_damage_reduction ) )
    {
        send_to_char( "Twoje odczucie bólu zosta³o ju¿ zredukowane do minimum.\n\r", ch );
        return;
    }

    if ( ch->hit >= 9 * get_max_hp(ch) / 10 )
    {
        send_to_char( "Nie jeste¶ wystarczaj±co rann<&y/a/e>, skup siê na walce.\n\r", ch );
        return;
    }

    chance += ch->level;

    if ( IS_NPC( ch ) )
    {
        chance += chance / 2;
    }
    else
    {
        if ( ch->condition[ COND_DRUNK ] > DRUNK_FULL )
            chance += 10;
    }

    chance = URANGE( 5, chance, 95 );

    if ( number_percent() < chance )
    {
        AFFECT_DATA af;

        send_to_char( "{5Bierzesz gleboki oddech i wydajesz z siebie dono¶ny, gard³owy okrzyk. Przestajesz odczuwaæ jakikolwiek ból.{x\n\r", ch );
        act( "{5$n bierze g³eboki oddech i wydaje z siebie dono¶ny gard³owy okrzyk.{x", ch, NULL, NULL, TO_ROOM );

        af.where	= TO_AFFECTS;
        af.type	= gsn_damage_reduction;
        af.level	= ch->level;
        af.duration = 3;         af.rt_duration = 0;
        af.bitvector = &AFF_NONE;
        af.location	= APPLY_NONE;
        af.modifier	= 0;
        af.visible = TRUE;
        affect_to_char( ch, &af, NULL, TRUE );
        check_improve( ch, NULL, gsn_damage_reduction, TRUE, 35 );
        WAIT_STATE( ch, PULSE_VIOLENCE );
    }
    else
    {
        send_to_char( "{5Starasz siê zapanowaæ nad bólem, z twojego gard³a wydobywa siê dono¶ne warczenie.{x\n\r", ch );
        act( "{5$n wydaje z siebie glo¶ny, gard³owy okrzyk.{x", ch, NULL, NULL, TO_ROOM );
        check_improve( ch, NULL, gsn_damage_reduction, FALSE, 50 );
        WAIT_STATE( ch, PULSE_VIOLENCE );
    }

    return;
}

void do_peek( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim;
    OBJ_DATA *obj;
    int skill, copper;
    bool found = FALSE, noticed = FALSE;

    if ( ( skill = get_skill( ch, gsn_peek ) ) == 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->fighting )
    {
        send_to_char( "Lepiej skup siê na walce.\n\r", ch );
        return;
    }

    if ( !check_blind( ch ) )
        return;

    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Czyj inwentarz chcesz podejrzeæ?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        add_wait_after_miss(ch, 2);
        return;
    }

    if ( victim == ch )
    {
        do_function( ch, do_inventory, "" );
        return;
    }

    if (ch->move < 0)
        ch->move = 0;

    if (ch->move == 0)
    {
        act( "Nie masz do¶æ si³y by uwa¿nie przyjrzeæ siê $X.", ch, NULL, victim, TO_CHAR );
        act( "$n próbuje przeszywa ciê wzrokiem.", ch, NULL, victim, TO_VICT );
        act( "$n próbuje przeszyæ wzrokiem $Z.", ch, NULL, victim, TO_NOTVICT );
        return;
    }

    if ( is_safe( ch, victim ) )
    {
        send_to_char( "Nic ciekawego nie widzisz.\n\r", ch );
        return;
    }

    if (ch->class == CLASS_BARD )
        skill -= skill/4;

    skill = URANGE( 15, 10 + skill, 95 );

    if ( skill < number_percent() )
    {
        check_improve( ch, NULL, gsn_peek, TRUE, 75 );
    }
    else
    {
        check_improve( ch, NULL, gsn_peek, FALSE, 65 );
    }

    WAIT_STATE( ch, PULSE_VIOLENCE );

    if ( number_percent() > skill )
    {
        act( "$n lustruje ciê bardzo uwa¿nie.", ch, NULL, victim, TO_VICT );
        act( "$n uwa¿nie przypatruje siê $X.", ch, NULL, victim, TO_NOTVICT );
        noticed = TRUE;
    }

    for ( obj = victim->carrying; obj; obj = obj->next_content )
    {
        if ( obj->wear_loc != WEAR_NONE || !can_see_obj( ch, obj ) )
        {
            continue;
        }
        if ( number_percent() < skill )
        {
            if ( !found )
            {
                found = TRUE;
                print_char( ch, "Patrzysz co %s nosi przy sobie:\n\r", PERS( victim, ch ) );
                ch->move -= 1;
            }
            print_char( ch, "%s\n\r", obj->short_descr );
        }
    }

    if ( !found )
    {
        print_char( ch, "Chyba %s nic nie nosi przy sobie.\n\r", PERS( victim, ch ) );
        ch->move -= 1;
    }

    if ( number_percent() < ( skill + 25 ))
    {
        copper = RATTING_SILVER * money_count_copper( victim );
        copper = UMAX ( 0, number_range( skill, 200 - skill ) * copper / 100 );

        if ( !copper )
        {
            print_char( ch, "Sakiewka %s wygl±da na pust±.\n\r", victim->name2 );
        }
        else if (copper < 10 * RATTING_SILVER )
        {
            print_char( ch, "%s ma w sakiewce gar¶æ monet.\n\r", capitalize( PERS( victim, ch ) ) );
        }
        else if (copper < 50 * RATTING_SILVER )
        {
            print_char( ch, "%s ma w sakiewce trochê monet.\n\r", capitalize( PERS( victim, ch ) ) );
        }
        else if (copper < 100 * RATTING_SILVER )
        {
            print_char( ch, "%s ma w sakiewce sporo monet.\n\r", capitalize( PERS( victim, ch ) ) );
        }
        else if (copper < 200 * RATTING_SILVER )
        {
            print_char( ch, "%s ma w sakiewce du¿o monet.\n\r", capitalize( PERS( victim, ch ) ) );
        }
        else if (copper < 500 * RATTING_SILVER )
        {
            print_char( ch, "%s ma w sakiewce bardzo du¿o monet.\n\r", capitalize( PERS( victim, ch ) ) );
        }
        else
        {
            print_char( ch, "%s ma w sakiewce nieprzeliczaln± ilo¶æ monet.\n\r", capitalize( PERS( victim, ch ) ) );
        }
    }

    if ( IS_NPC( victim ) && noticed )
    {
        int int_test;

        int_test = get_curr_stat_deprecated( victim, STAT_INT );
        int_test = 5 * URANGE( 0, int_test - 7, 20 );

        if ( number_percent() < int_test )
        {
            multi_hit( victim, ch, TYPE_UNDEFINED );
        }
    }

    if (ch->move < 0)
    {
        ch->move = 0;
    }

    return;
}

//dla undeadow i nie wiadomo kogo jeszcze
//pozeranie ciala z bonusem leczacym
void do_devour( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj;
    int heal_percent, heal_amount;
    AFFECT_DATA af;

    if ( !is_undead(ch) )
        return;

    if ( get_skill( ch, gsn_devour ) <= 0 )
    {
        print_char( ch, "No chyba ¿artujesz.\n\r" );
        return;
    }

    if ( is_affected( ch, gsn_devour ) )
    {
        print_char( ch, "Juz ci siê chyba nic wiêcej w brzuchu nie zmie¶ci.\n\r" );
        return;
    }

    one_argument( argument, arg );

    if ( ch->fighting )
    {
        send_to_char( "Lepiej skup siê na walce!\n\r", ch );
        return;
    }

    if ( arg[ 0 ] == '\0' )
    {
        print_char( ch, "Co by¶ %s?\n\r", ch->sex == 2 ? "zjad³a" : "zjad³" );
        return;
    }

    if ( ( obj = get_obj_list( ch, arg, ch->in_room->contents ) ) == NULL )
    {
        send_to_char( "Nie widzisz tu nic takiego.\n\r", ch );
        return;
    }

    if ( obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC )
    {
        print_char( ch, "Hmm, a to ma jakie¶ kalorie w ogóle?\n\r" );
        return;
    }

    if ( obj->item_type == ITEM_CORPSE_NPC &&
            ( obj->value[ 0 ] <= 0 ||
              !get_mob_index( obj->value[ 0 ] ) ) )
    {
        print_char( ch, "Wygl±da jako¶ podejrzanie, chyba odesz³a ci ochota ¿eby to po¿reæ.\n\r" );
        return;
    }

    if ( obj->item_type == ITEM_CORPSE_PC )
        obj->item_type = ITEM_CORPSE_NPC;

    obj->value[ 0 ] = 0;

    act( "Po¿erasz $h rozchlapuj±c wszêdzie krew i resztki wnêtrzno¶ci.", ch, obj, NULL, TO_CHAR );

    switch ( number_range( 1, 5 ) )
    {
        case 1:
            act( "$n rzuca siê na $h i rozszarpuje je w mgnieniu oka na krwawe och³apy, które po¿era.", ch, obj, NULL, TO_ROOM );
            break;
        case 2:
            act( "$n niczym wyg³odnia³a bestia tnie d³ugimi pazurami $h i zjada krwawi±ce kawa³ki.", ch, obj, NULL, TO_ROOM );
            break;
        case 3:
            act( "Z odraz± obserwujesz, jak $n zatapia swe d³ugie zêby w $h i po¿ywia siê nim.", ch, obj, NULL, TO_ROOM );
            break;
        case 4:
            act( "Zbiera ci siê na wymioty, gdy widzisz jak $n zjada $h, ogryzaj±c je szybko do ko¶ci.", ch, obj, NULL, TO_ROOM );
            break;
        case 5:
            act( "$n rozszarpuje le¿±ce tu $h na strzêpy, po czym zaczyna je niewiarygodnie szybko zjadaæ.", ch, obj, NULL, TO_ROOM );
            break;
        default:
            act( "$n rozszarpuje le¿±ce tu $h na strzêpy, po czym zaczyna je niewiarygodnie szybko zjadaæ.", ch, obj, NULL, TO_ROOM );
            break;
    }

    af.where = TO_AFFECTS;
    af.type = gsn_devour;
    af.level = 0;
    af.duration = number_range( 6, 10 );     af.rt_duration = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = &AFF_NONE;

    affect_to_char( ch, &af, NULL, TRUE );

    heal_percent = number_range( 20, 40 );
    heal_percent += ch->level / 2;
    heal_amount = heal_percent * get_max_hp(ch) / 100;
    heal_char( ch, ch, heal_amount );
    return;
}

void do_listen( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int skill;

    if ( ( skill = get_skill( ch, gsn_listen ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if( ch->fighting )
    {
        send_to_char("Lepiej skup siê na walce.\n\r", ch);
        return;
    }

    if ( is_affected (ch, gsn_listen) )
    {
        send_to_char( "Przecie¿ ju¿ ws³uchujesz siê w otoczenie.\n\r", ch );
        return;
    }

    act("$n w skupieniu nadstawia uszy i bacznie siê rozgl±da.",ch,NULL,NULL,TO_ROOM);
    if ( number_percent( ) < skill)
    {
        send_to_char( "Ws³uchujesz siê w otoczenie w poszukiwaniu ukrywaj±cych siê postaci.\n\r", ch );
        af.where	 = TO_AFFECTS;
        af.type	  = gsn_listen;
        af.level	 = ch->level;
        af.duration  = ch->level/2;
        af.rt_duration = 0;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = &AFF_DETECT_HIDDEN;
        affect_to_char( ch, &af, NULL, TRUE );
        check_improve(ch, NULL, gsn_listen, TRUE, 60 );
        WAIT_STATE(ch, 12);
    }
    else
    {
        send_to_char( "Starasz siê ws³uchaæ w otaczaj±ce ciê d¼wiêki ale nie uda³o ci siê dostatecznie skoncentrowaæ.\n\r", ch );
        check_improve(ch, NULL, gsn_listen, FALSE, 60 );
        WAIT_STATE(ch, 12);
        return;
    }

    return;
}

void do_remlisten( CHAR_DATA *ch, char *argument ) //pomocnicze do listen
{
    if( is_affected(ch,gsn_listen))
        affect_strip(ch,gsn_listen);
    return;
}

void do_persuasion( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim;
    AFFECT_DATA af;
    char vict_name[ MAX_INPUT_LENGTH ];
    char *comm2, comm[ MAX_INPUT_LENGTH ];
    bool ok = FALSE;
    int luck = get_curr_stat_deprecated( ch, STAT_LUC ), cha = get_curr_stat_deprecated( ch, STAT_CHA );
    int skill, vwis, bonus = 0, vint;

    if ( ( skill = get_skill( ch, gsn_persuasion ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    argument = one_argument( argument, vict_name );
    comm2 = argument;
    argument = one_argument( argument, comm );

    if ( comm[ 0 ] == '\0' || vict_name[ 0 ] == '\0' )
    {
        send_to_char( "Na kogo chcesz wp³yn±æ swoim urokiem osobistym?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, vict_name ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        add_wait_after_miss(ch, 2);
        return;
    }

    if ( is_affected( ch, gsn_persuasion ) )
    {
        act( "{5Nie mo¿esz siê wystarczaj±co skupiæ, by nawi±zaæ rozmowê z $V.{x", ch, NULL, victim, TO_CHAR );
        return;
    }

    vint = get_curr_stat_deprecated( victim, STAT_INT);
    if ( !IS_NPC(victim) || is_safe(ch,victim) || vint < 8 || !IS_SET(victim->parts, PART_BRAINS))
    {
        act( "{5$N nie zwraca na ciebie najmniejszej nawet uwagi.{x", ch, NULL, victim, TO_CHAR );
        act( "{5$n podchodzi do ciebie i zaczyna rozmowê, jednak ty nie zwracasz na to uwagi i wracasz do swych spraw.{x", ch, NULL, victim, TO_VICT );
        act( "{5$n próbuje przekonaæ $C do swych racji, ale nic nie osi±ga.{x", ch, NULL, victim, TO_NOTVICT );
        return;
    }

    if ( IS_AFFECTED(victim, AFF_PARALYZE))
    {
        act( "Nie jeste¶ w stanie wp³yn±æ na $C.", ch, NULL, victim, TO_CHAR );
        return;
    }

    vwis = get_curr_stat_deprecated( victim, STAT_WIS)-10;
    vwis += get_curr_stat_deprecated( victim, STAT_INT)-10;

    if ((ch->sex ==	1 && victim->sex == 2) || (ch->sex == 2	&& victim->sex != 2))
        bonus = 18;

    if ( (skill+(cha-10)*3+(luck/3)+bonus < number_range(1,100)+vwis+victim->level) || ( victim->resists[ RESIST_CHARM ] > 0 && number_percent() < victim->resists[ RESIST_CHARM ] ) )
    {
        act( "{5Bardzo stara³<&e¶/a¶/o¶> siê nak³oniæ $C do wype³nienia twojej woli, jednak wszelkie metody zawiod³y.{x", ch, NULL, victim, TO_CHAR );
        switch(ch->sex)
        {
            case 0:
                act( "{5$n próbowa³o sprytnie nak³oniæ ciê do wykonania $s woli, lecz uda³o ci siê w ostatniej chwili zorientowaæ. {x", ch, NULL, victim, TO_VICT );
                act( "{5$n próbowa³o sprytnie nak³oniæ $C do wykonania swej woli, jednak $N w ostatniej chwili orientuje siê co siê dzieje.{x", ch, NULL, victim, TO_NOTVICT );
                break;
            case 1:
                act( "{5$n próbowa³ sprytnie nak³oniæ ciê do wykonania $s woli, lecz uda³o ci siê w ostatniej chwili zorientowaæ. {x", ch, NULL, victim, TO_VICT );
                act( "{5$n próbowa³ sprytnie nak³oniæ $C do wykonania jego woli, jednak $N w ostatniej chwili orientuje siê co siê dzieje.{x", ch, NULL, victim, TO_NOTVICT );
                break;
            case 2:
                act( "{5$n próbowa³a sprytnie nak³oniæ ciê do wykonania $s woli, lecz uda³o ci siê w ostatniej chwili zorientowaæ. {x", ch, NULL, victim, TO_VICT );
                act( "{5$n próbowa³a sprytnie nak³oniæ $C do wykonania jej woli, jednak $N w ostatniej chwili orientuje siê co siê dzieje.{x", ch, NULL, victim, TO_NOTVICT );
                break;
        }

        if ( can_see( victim, ch ) && can_move( victim ) && stat_throw(victim,STAT_INT) )
            multi_hit( victim, ch, TYPE_UNDEFINED );

        if ( skill < 25)
            check_improve(ch, NULL, gsn_persuasion, FALSE, 12 );
        else
            check_improve(ch, NULL, gsn_persuasion, FALSE, 15 );

        af.where = TO_AFFECTS;
        af.type = gsn_persuasion;
        af.level = 0;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = &AFF_NONE;
        af.duration = URANGE(1, 4-(skill/25), 4);         af.rt_duration = 0;
        af.visible = FALSE;
        affect_to_char( ch, &af, NULL, FALSE );
        WAIT_STATE(ch, 12);
        return;
    }

    if ( !str_prefix( comm, "north" ) ||
            !str_prefix( comm, "east" ) ||
            !str_prefix( comm, "south" ) ||
            !str_prefix( comm, "west" ) ||
            !str_prefix( comm, "flee" ) ||
            !str_prefix( comm, "up" ) ||
            !str_prefix( comm, "down" ) ||
            !str_prefix( comm, "get" ) ||
            !str_prefix( comm, "take" ) ||
            !str_prefix( comm, "mount" ) ||
            !str_prefix( comm, "dismount" ) ||
            !str_prefix( comm, "sleep" ) ||
            !str_prefix( comm, "sit" ) ||
            !str_prefix( comm, "stand" ) ||
            !str_prefix( comm, "wake" ) ||
            !str_prefix( comm, "rest" ) ||
            !str_prefix( comm, "knock" ) ||
            !str_prefix( comm, "say" ) ||
            !str_prefix( comm, "yell" ) ||
            !str_prefix( comm, "speak" ) ||
            !str_prefix( comm, "sayto" ) ||
            !str_prefix( comm, "tell" ) ||
            !str_prefix( comm, "display" ) ||
            !str_prefix( comm, "scan" ) ||
            !str_prefix( comm, "hold" ) ||
            !str_prefix( comm, "wear" ) ||
            !str_prefix( comm, "wield" ) ||
            !str_prefix( comm, "uncloak" ) ||
            !str_prefix( comm, "disarm" ) ||
            !str_prefix( comm, "charge" ) ||
            !str_prefix( comm, "wardance" ) ||
            !str_prefix( comm, "damage" ) ||
            !str_prefix( comm, "surrender" ) ||
            !str_prefix( comm, "berserk" ) ||
            !str_prefix( comm, "dodge" ) ||
            !str_prefix( comm, "parry" ) ||
            !str_prefix( comm, "mighty" ) ||
            !str_prefix( comm, "power" ) ||
            !str_prefix( comm, "critical" ) ||
            !str_prefix( comm, "lay" ) ||
            !str_prefix( comm, "healing" ) ||
            !str_prefix( comm, "sharpen" ) ||
            !str_prefix( comm, "recuperate" ) ||
            !str_prefix( comm, "mend" ) ||
            !str_prefix( comm, "aura" ) ||
            !str_prefix( comm, "damn" ) ||
            !str_prefix( comm, "call" ) ||
            !str_prefix( comm, "aid" ) ||
            !str_prefix( comm, "touch" ) ||
            !str_prefix( comm, "pray" ) ||
            !str_prefix( comm, "sanct" ) ||
            !str_prefix( comm, "hide" ) ||
            !str_prefix( comm, "sneak" ) ||
            !str_prefix( comm, "visible" ) ||
            !str_prefix( comm, "listen" ) ||
            !str_prefix( comm, "meditation" ) ||
            !str_prefix( comm, "bandage" ) ||
            !str_prefix( comm, "cleave" ) )
            {
                ok = TRUE;
            }

    if ( ok )
    {
        act( "{5U¿ywaj±c swojego ca³ego uroku osobistego nak³aniasz $C do wype³nienia swej woli.{x", ch, NULL, victim, TO_CHAR );
        act( "{5Z rozmowy z $v wyci±gasz wiele wniosków, i od razu wprowadzasz je w ¿ycie.{x", ch, NULL, victim, TO_VICT );
        act( "{5$n rozpoczyna rozmowê z $V i u¶miechaj±c siê weso³o przekonuje $S do swej woli.{x", ch, NULL, victim, TO_NOTVICT );

        if( skill < 75 )
        {
            af.where = TO_AFFECTS;
            af.type = gsn_persuasion;
            af.level = 50;
            af.location = APPLY_NONE;
            af.modifier = 0;
            af.bitvector = &AFF_NONE;
            af.duration = URANGE(1, 3-(skill/25), 3);             af.rt_duration = 0;
            af.visible = FALSE;
            affect_to_char( ch, &af, NULL, FALSE );
        }

        /*		if ( EXT_IS_SET( ch->act, PLR_COMMANDSPL ) )
                {
                for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
                {
                if ( NOPOL( comm[ 0 ] ) == NOPOL( cmd_table[ cmd ].name_pl[ 0 ] )
                && !str_prefix( comm, cmd_table[ cmd ].name_pl ) )
                {
                sprintf( comm, cmd_table[ cmd ].name );
                interpret( victim, comm );
                check_improve(ch, NULL, gsn_persuasion, TRUE, 2 );
                WAIT_STATE(ch, 12);
                break;
                }
                }
                }
                else*/
        interpret( victim, comm2 );
        check_improve(ch, NULL, gsn_persuasion, TRUE, 12 );
        WAIT_STATE(ch, 12);
    }
    else
    {
        act( "{5Nie uda ci siê nak³oniæ $C do wykonania tak skomplikowanego polecenia.{x", ch, NULL, victim, TO_CHAR );
        return;
    }

    return;
}


/* strojenie instrumentow - dla barda */
void do_tune( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj;
    int skill, instrument_skill, dex_mod, chance = 0, luck = get_curr_stat_deprecated( ch, STAT_LUC );
    AFFECT_DATA *obj_affect, af;

    argument = one_argument( argument, arg );

    if ( ( skill = get_skill( ch, gsn_tune ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Co chcia³<&/a/o>by¶ nastroiæ?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        send_to_char( "Nie masz takiej rzeczy.\n\r", ch );
        return;
    }

    if ( obj->item_type != ITEM_MUSICAL_INSTRUMENT )
    {
        print_char( ch, "Przecie¿ %s nie jest instrumentem muzycznym.\n\r", obj->name );
        return;
    }

    if ( obj->value[2] > 95 )
    {
        send_to_char( "Ten instrument jest ju¿ przecie¿ nastrojony.\n\r", ch );
        return;
    }

    for ( obj_affect = obj->affected; obj_affect != NULL; obj_affect = obj_affect->next )
        if ( obj_affect->type == gsn_tune )
        {
            print_char( ch, "Kto¶ niedawno nastroi³ %s, nic wiecêj nie zdzia³asz.\n\r", obj->name4 );
            return;
        }

    instrument_skill = get_skill( ch, musical_instrument_sn( obj ) );
    dex_mod = get_curr_stat_deprecated( ch, STAT_DEX );

    chance = skill;
    chance += (dex_mod-10)*2;
    chance += instrument_skill / 3;
    chance += URANGE( 1, number_range( 0, luck / 2 ), 10 );

    chance = URANGE( 1, chance , 95 );

    if ( number_percent() < chance )
    {
        int repair;
        repair = URANGE( 5, ( ( 25 + chance / 3 + instrument_skill / 3 ) * ( 100 - obj->value[2] ) ) / 100 , 95 );
        obj->value[2] += repair;

        print_char( ch, "Stroisz %s.\n\r", obj->name4 );
        act( "$n stroi $h.", ch, obj, NULL, TO_ROOM );
        WAIT_STATE( ch, 12 );
        check_improve( ch, NULL, gsn_tune, TRUE, 60 );
        return;
    }
    else if ( number_range( 0, luck + instrument_skill / 4 ) == 0 )
    {
        print_char( ch, "Starasz siê nastroiæ %s, jednak mylisz siê kompletnie i robisz nie to co trzeba, %s nadaje siê ju¿ tylko do wyrzucenia.\n\r", obj->name4, obj->name );
        act( "$n ci±gnie ose³k± po ostrzu $f.", ch, obj, NULL, TO_ROOM );
        obj->value[2] = 1;
        WAIT_STATE( ch, 12 );
        check_improve( ch, NULL, gsn_tune, FALSE, 50 );
    }
    else
    {
        print_char( ch, "Starasz siê nastroiæ %s, jednak teraz %s wydaje z siebie jeszcze gorsze d¼wiêki.\n\r", obj->name4, obj->name );
        act( "$n stroi $h.", ch, obj, NULL, TO_ROOM );
        obj->value[2] = UMAX( obj->value[2] - 3, 1 );
        WAIT_STATE( ch, 12 );
        check_improve( ch, NULL, gsn_tune, FALSE, 85 );
    }

    /* zeby automaty nie staly*/
    if ( number_percent() < ( 30 - luck / 2 ) )
    {
        af.where = TO_OBJECT;
        af.type = gsn_tune;
        af.level = 0;
        af.duration = 3;         af.rt_duration = 0;
        af.location = 0;
        af.modifier = 0;
        af.bitvector = &AFF_NONE;
        affect_to_obj( obj, &af );
    }

    return;
}

void do_torment( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * victim;
    AFFECT_DATA af;
    int value, skill, charisma_mod = get_curr_stat_deprecated( ch, STAT_CHA ) - 18;

    if ( ( skill = get_skill( ch, gsn_torment ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        if ( ch->fighting )
        {
            victim = ch->fighting;
        }
        else
        {
            send_to_char( "Kogo chcesz zaatakowaæ?\n\r", ch );
            return;
        }
    }
    else if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        add_wait_after_miss(ch, 2);
        return;
    }

    if ( is_affected(ch,gsn_torment ))
    {
        send_to_char( "Nie mo¿esz siê wystarczaj±co skoncentrowaæ.\n\r", ch );
        return;
    }

    if ( ( victim == ch && !is_undead(ch) ) || is_safe( ch, victim ) )
    {
        send_to_char("Nie mo¿esz tego zrobiæ.\n\r", ch );
        WAIT_STATE( ch, skill_table[ gsn_torment ].beats / 2 );
        return;
    }

    //nie da siê konstruktow
    if ( IS_SET(victim->form, FORM_CONSTRUCT) )
    {
        act("To nie ma sensu, negatywna energia nie zadzia³a na $C.", ch, NULL, victim, TO_CHAR );
        WAIT_STATE( ch, 2 * skill_table[ gsn_torment ].beats );
        return;
    }

    af.where       = TO_AFFECTS;
    af.type        = gsn_torment;
    af.level       = 50; // avoid to dispel magic
    af.location    = APPLY_NONE;
    af.duration    = 24 - ch->level/10;
    af.rt_duration = af.duration/2;
    af.modifier    = 0;
    af.bitvector   = &AFF_NONE;
    af.visible     = FALSE;

    affect_to_char( ch, &af, NULL, FALSE );

    WAIT_STATE( ch, skill_table[ gsn_torment ].beats );
    value = 10 + ch->level * number_range( 6, 11 ) + 5*charisma_mod;
    value = ( 34 + (2 * skill) / 3 ) * value / 100;

    if ( !is_undead(victim) )
    {
       if( victim->hit - ( ( value * ( 100 - victim->resists[RESIST_NEGATIVE] ) )/100 ) > -11 ) // FIXME: uwzglêdniæ pvp i deflect wounds
        {
            if( IS_AFFECTED(victim, AFF_BARK_SKIN))
            {
                act( "Wskazujesz palcem na $C wymawiaj±c g³o¶no s³owa kl±twy. Wokó³ $S cia³a zbiera siê chmura bia³ego dymu, która po chwili przes³ania $S postaæ. S³yszysz tylko wycie z bólu i czujesz smród palonego drewna.", ch, NULL, victim, TO_CHAR );
                act( "$n wskazuje na ciebie palcem wymawiaj±c g³o¶no s³owa kl±twy. Wokó³ ciebie zbiera siê przera¿aj±ca chmura bia³ego dymu, który po chwili zas³ania ciê ca³kowicie. Nagle czujesz przera¼liwy parali¿uj±cy ból!", ch, NULL, victim, TO_VICT );
                act( "$n wskazuje palcem na $C wymawiaj±c g³o¶no s³owa kl±twy. Wokó³ $Z zbiera siê chmura bia³ego dymu, która po chwili przes³ania $S postaæ. Czujesz smród pal±cego siê drewna.", ch, NULL, victim, TO_NOTVICT );
            }
            else
            {
                act( "Wskazujesz palcem na $C wymawiaj±c g³o¶no s³owa kl±twy. Wokó³ $S cia³a zbiera siê chmura czerwonego dymu, która po chwili przes³ania $S postaæ. S³yszysz tylko wycie z bólu i czujesz smród nadpalonego cia³a.", ch, NULL, victim, TO_CHAR );
                act( "$n wskazuje na ciebie palcem wymawiaj±c g³o¶no s³owa kl±twy. Wokó³ ciebie zbiera siê przera¿aj±ca chmura czerwonego dymu, który po chwili zas³ania ciê ca³kowicie. Nagle czujesz przera¼liwy parali¿uj±cy ból!", ch, NULL, victim, TO_VICT );
                act( "$n wskazuje palcem na $C wymawiaj±c g³o¶no s³owa kl±twy. Wokó³ $Z zbiera siê chmura czerwonego dymu, która po chwili przes³ania $S postaæ. Czujesz smród pal±cego siê cia³a.", ch, NULL, victim, TO_NOTVICT );
            }
        }
        else
        {
            act( "Gdy mroczna chmura otacza $c, $e miotaj±c siê na wszystkie strony próbuje uciec - lecz na pró¿no! W zastraszaj±cym têpie $s si³y witalne poch³aniane s± przez ³apczyw± chmurê negatywnej energii tak, i¿ w koñcu z $z zostaje wysuszona mumia z wykrzywion± twarz±, na której go¶ci wyraz olbrzymiej udrêki.", victim, NULL, NULL, TO_ROOM );
            act( "Nagle otacza ciê chmura otacza ciê, a tobie, mimo ¿e miotasz siê na wszystkie strony, nie udaje siê jej opu¶ciæ! W zastraszaj±cym têpie twe si³y witalne poch³aniane s± przez ³apczyw± chmurê negatywnej energii, teraz czujesz ju¿ tylko ból...", victim, NULL, NULL, TO_CHAR );
        }

        if ( IS_GOOD(victim) && !IS_AFFECTED(victim, AFF_PARALYZE) && !IS_AFFECTED(victim, AFF_FREE_ACTION ) &&
                !saves_spell_new(victim, skill_table[gsn_torment].save_type, skill_table[gsn_torment].save_mod, ch, gsn_torment ) &&
                number_percent() < get_skill(ch,gsn_torment) + get_curr_stat_deprecated(ch,STAT_CHA) - 10 )
        {
            af.where       = TO_AFFECTS;
            af.type        = gsn_holdmonster;
            af.level       = ch->level;
            af.location    = APPLY_NONE;
            af.duration    = URANGE( 2, 1 + get_skill(ch, gsn_torment )/15, 5 );
            af.rt_duration = af.duration;
            af.modifier    = 0;
            af.bitvector   = &AFF_PARALYZE;
            affect_to_char( victim, &af, NULL, TRUE  );
        }

        if( IS_EVIL(victim) )
        {
            value /= 4;
        }
        else if ( !IS_GOOD(victim ))
        {
            value /= 2;
        }

        if ( victim->position == POS_DEAD )
        {
            return;
        }
        spell_damage( ch, victim, value, gsn_torment, DAM_NEGATIVE, FALSE );
        SET_BIT( ch->fight_data, FIGHT_TORMENT_NOFLEE );
    }
    else
    {
        if( victim != ch )
        {
            act( "Wskazujesz palcem na $C wymawiaj±c g³o¶no s³owa kl±twy. Wokó³ $S cia³a zbiera siê chmura czerwonego dymu, która po chwili zostaje do niego wessana.", ch, NULL, victim, TO_CHAR );
            act( "$n wskazuje palcem na $C wymawiaj±c g³o¶no s³owa kl±twy. Wokó³ $Z zbiera siê chmura czerwonego dymu, która po chwili zostaje do niego wessana.", ch, NULL, victim, TO_NOTVICT );
        }
        else
        {
            act( "Koncetrujesz siê przez chwilê, aby uwolniæ negatywn± energiê. Dooko³a ciebie wzbiera siê chmura czerwonego dymu, któr± po chwili wsysasz. Czujesz przez moment przeszywaj±ce dreszcze towarzysz±cy regenerowani siê ran.", ch, NULL, NULL, TO_CHAR );
            act( "Nagle dooko³a $z wybucha chmura czerwonego dymu. Z przera¿eniem i zarazem fascynacj± przygl±dasz siê jak wch³ania j± jednocze¶nie regeneruj±c swe rany.", ch, NULL, NULL, TO_ROOM );
        }
        heal_undead( ch, victim, value/2 );
    }
    check_improve( ch, victim, gsn_torment, TRUE, 5 );
    return;
}

void do_overwhelming( CHAR_DATA *ch, char *argument )
{
    int skill;
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *weapon;

    one_argument( argument, arg );

    if ( ( skill = get_skill( ch, gsn_overwhelming_strike ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->mount )
    {
        send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
        return;
    }

    if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char( "Postaraj siê o jak±¶ broñ!\n\r", ch );
        return;
    }

    if ( ch->fighting != NULL )
    {
        victim = ch->fighting;
    }
    else if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Rozgl±dasz siê w poszukiwaniu przeciwnika.\n\r", ch );
        return;
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        add_wait_after_miss(ch, 2);
        return;
    }

    if( ch == victim || is_safe( ch, victim ) )
    {
        send_to_char( "To siê nie uda.\n\r", ch );
        return;
    }

    if ( !IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS ) ||
            weapon->value[0] == WEAPON_DAGGER ||
            weapon->value[0] == WEAPON_WHIP ||
            weapon->value[0] == WEAPON_SHORTSWORD ||
            weapon->value[0] == WEAPON_CLAWS )
    {
        send_to_char( "Aby wykonaæ taki atak potrzebujesz broni dwurêcznej.\n\r", ch );
        return;
    }

    WAIT_STATE( ch, skill_table[ gsn_overwhelming_strike ].beats );

    skill = get_curr_stat_deprecated(ch,STAT_STR) - 10 + skill;
    skill = URANGE( 2, skill, 95 );

    if ( number_percent() < skill )
    {
        send_to_char( "Bêdziesz teraz próbowa<&³/³a/³o> wyprowadziæ przyt³aczaj±ce trafienie krytyczne.\n\r", ch );
        SET_BIT( ch->fight_data, FIGHT_OVERWHELMING_STRIKE );
        check_improve( ch, victim, gsn_overwhelming_strike, TRUE, 55 );
    }
    else
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        check_improve( ch, victim, gsn_overwhelming_strike, FALSE, number_range(50,60) );
    }

    if ( ch->fighting == NULL )
        one_hit( ch, victim, TYPE_UNDEFINED, FALSE );

    return;
}

void do_cleave( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *weapon;
    CHAR_DATA * tch;
    int skill;
    int crowd = 0;

    if ( ( skill = get_skill( ch, gsn_cleave ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->fighting == NULL )
    {
        send_to_char( "Z nikim nie walczysz.\n\r", ch );
        return;
    }

    if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char( "Postaraj siê o jak±¶ broñ!\n\r", ch );
        return;
    }

    if ( !IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS ) ||
            weapon->value[0] == WEAPON_DAGGER ||
            weapon->value[0] == WEAPON_WHIP ||
            weapon->value[0] == WEAPON_MACE ||
            weapon->value[0] == WEAPON_STAFF ||
            weapon->value[0] == WEAPON_SPEAR ||
            weapon->value[0] == WEAPON_FLAIL ||
            weapon->value[0] == WEAPON_SHORTSWORD ||
            weapon->value[0] == WEAPON_CLAWS )
    {
        send_to_char( "Aby wykonaæ taki atak potrzebujesz tn±cej broni dwurêcznej.\n\r", ch );
        return;
    }

    for ( tch = ch->in_room->people; tch; tch = tch->next_in_room )
        if ( tch->fighting == ch || ch->fighting == tch )
            crowd++;

    if ( crowd <= 1 && !IS_AFFECTED( ch->fighting, AFF_MIRROR_IMAGE ) && !is_affected(ch->fighting, gsn_mirror_image) )
    {
        send_to_char( "Przecie¿ walczysz z tylko jednym przeciwnikiem.\n\r", ch );
        return;
    }

    WAIT_STATE( ch, skill_table[ gsn_cleave ].beats );

    skill += get_curr_stat_deprecated(ch,STAT_STR);
    skill += get_curr_stat_deprecated(ch,STAT_DEX);
    skill -= 20;
    skill = URANGE( 5, skill, 95 );

    if ( number_percent() < skill )
    {
        send_to_char( "Skupiasz siê próbuj±c wyprowadziæ zamaszysty cios we wszystkich swoich przeciwników.\n\r", ch );
        SET_BIT( ch->fight_data, FIGHT_CLEAVE );
        check_improve( ch, NULL, gsn_cleave, TRUE, 40 );
    }
    else
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        check_improve( ch, NULL, gsn_cleave, FALSE, 40 );
    }
    return;
}

void do_demon_aura( CHAR_DATA *ch, char *argument )
{
    int chance;
    AFFECT_DATA af;

    if ( ( chance = get_skill( ch, gsn_demon_aura ) ) == 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( is_affected( ch, gsn_demon_aura ) )
    {
        send_to_char( "Jeste¶ ju¿ otoczon<&y/a/e> demoniczn± aur±.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_CALM ) )
    {
        send_to_char( "Nie mo¿esz siê wystarczaj±co skoncentrowaæ.\n\r", ch );
        return;
    }

    if ( !IS_NPC( ch ) && ch->counter[ 3 ] > 0 )
    {
        send_to_char( "Nie mo¿esz siê wystarczaj±co skoncentrowaæ, poczekaj a¿ twe demoniczne moce siê zregeneruj±.\n\r", ch );
        return;
    }

    WAIT_STATE( ch, skill_table[ gsn_demon_aura ].beats );

    chance += get_curr_stat_deprecated(ch,STAT_CHA) - 10;

    chance = URANGE(5,chance,95);
    if ( number_percent() < chance )
    {
        if ( ch->level < 5 )
        {
            ch->counter[ 3 ] = 20;
        }
        else if ( ch->level < 11 )
        {
            ch->counter[ 3 ] = 15;
        }
        else if ( ch->level < 15 )
        {
            ch->counter[ 3 ] = 12;
        }
        else if ( ch->level < 20 )
        {
            ch->counter[ 3 ] = 10;
        }
        else if ( ch->level < 23 )
        {
            ch->counter[ 3 ] = 8;
        }
        else if ( ch->level < 26 )
        {
            ch->counter[ 3 ] = 6;
        }
        else if ( ch->level < 30 )
        {
            ch->counter[ 3 ] = 5;
        }
        else
        {
            ch->counter[ 3 ] = 4;
        }

        switch( ch->sex )
        {
            case SEX_NEUTRAL:
                send_to_char( "{5Odmawiasz modlitwê do swojego Mrocznego Bóstwa, która otacza ciê sw± demoniczn± aur±.{x\n\r", ch );
                act( "$n odmawia modlitwê do swej Mrocznego Bóstwa, które otacza je demoniczn± aur±.", ch, NULL,NULL,TO_ROOM );
                break;
            case SEX_FEMALE:
                send_to_char( "{5Odmawiasz modlitwê do swej Mrocznej Pani, która otacza ciê sw± demoniczn± aur±.{x\n\r", ch );
                act( "$n odmawia modlitwê do swej Mrocznej Pani, która otacza j± demoniczn± aur±.", ch, NULL,NULL,TO_ROOM );
                break;
            case SEX_MALE:
            default:
                send_to_char( "{5Odmawiasz modlitwê do swego Mrocznego Pana, który otacza ciê sw± demoniczn± aur±.{x\n\r", ch );
                act( "$n odmawia modlitwê do swego Mrocznego Pana, który otacza go sw± demoniczn± aur±.", ch, NULL,NULL,TO_ROOM );
                break;
        }

        check_improve( ch, NULL, gsn_demon_aura, TRUE, 15 );

        af.where		= TO_AFFECTS;
        af.type			= gsn_demon_aura;
        af.level		= chance;
        af.duration		= 3;
        af.rt_duration = 0;
        af.bitvector	= &AFF_RESIST_NEGATIVE;
        af.modifier		= RESIST_NEGATIVE;
        af.location		= APPLY_RESIST;
        af.visible 		= TRUE;
        af.bitvector 	= &AFF_RESIST_NEGATIVE;
        affect_to_char( ch, &af, NULL, TRUE );

        af.location		= APPLY_DAMROLL;
        af.modifier		= URANGE( 0, get_skill(ch,gsn_demon_aura)/45, 2 );
        af.visible	 	= FALSE;
        af.bitvector 	= &AFF_DARK_VISION;
        affect_to_char( ch, &af, NULL, FALSE );

        af.location		= APPLY_HITROLL;
        af.modifier		= URANGE( 0, get_skill(ch,gsn_demon_aura)/45, 2 );
        af.bitvector 	= &AFF_DETECT_GOOD;
        affect_to_char( ch, &af, NULL, FALSE );
    }
    else
    {
        if( ch->sex == 2 )
        {
            send_to_char( "{5Odmawiasz modlitwê do swej mrocznej pani, ta jednak nie wys³uchuje twych pró¶b.{x\n\r", ch );
            act( "$n odmawia modlitwê do swej Mrocznej Pani, ta jednak nie wys³uchuje jej pró¶b.", ch, NULL,NULL,TO_ROOM );
        }
        else
        {
            send_to_char( "{5Odmawiasz modlitwê do swego mrocznego pana, ten jednak nie wys³uchuje twych pró¶b.{x\n\r", ch );
            act( "$n odmawia modlitwê do swego Mrocznego Pana, ten jednak nie wys³uchuje jego pró¶b.", ch, NULL,NULL,TO_ROOM );
        }
        ch->counter[ 3 ] = 1;
        check_improve( ch, NULL, gsn_demon_aura, FALSE, 15 );
    }

    return;
}

void do_vertical_slash( CHAR_DATA *ch, char *argument )
{
    int skill;
    CHAR_DATA *victim;
    char arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *weapon;

    one_argument( argument, arg );

    if ( ( skill = get_skill( ch, gsn_vertical_slash ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ( weapon = get_eq_char( ch, WEAR_WIELD ) ) == NULL )
    {
        send_to_char( "Postaraj siê o jak±¶ broñ!\n\r", ch );
        return;
    }

    if ( ch->mount )
    {
        send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
        return;
    }

    if ( ch->fighting != NULL )
    {
        victim = ch->fighting;
    }
    else if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Rozgl±dasz siê w poszukiwaniu przeciwnika.\n\r", ch );
        return;
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        add_wait_after_miss(ch, 2);
        return;
    }

    if( ch == victim || is_safe( ch, victim ) )
    {
        send_to_char( "To siê nie uda.\n\r", ch );
        return;
    }

    if ( !IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS ) ||
            weapon->value[0] == WEAPON_DAGGER ||
            weapon->value[0] == WEAPON_WHIP ||
            weapon->value[0] == WEAPON_MACE ||
            weapon->value[0] == WEAPON_STAFF ||
            weapon->value[0] == WEAPON_SPEAR ||
            weapon->value[0] == WEAPON_FLAIL ||
            weapon->value[0] == WEAPON_SHORTSWORD ||
            weapon->value[0] == WEAPON_CLAWS )
    {
        send_to_char( "Aby wykonaæ taki atak potrzebujesz tn±cej broni dwurêcznej.\n\r", ch );
        return;
    }

    WAIT_STATE( ch, skill_table[ gsn_vertical_slash ].beats );

    send_to_char( "Robisz gwa³towny krok w ty³, jednocze¶nie unosz±c swoj± broñ wysoko nad g³owê, przygotowuj±c siê do wyprowadzenia ni± silnego ciecia z góry do do³u.\n\r", ch );

    SET_BIT( ch->fight_data, FIGHT_VERTICAL_SLASH );

    if ( ch->fighting == NULL )
        multi_hit( ch, victim, TYPE_UNDEFINED );

    //wszystkie rzuty czy siê uda i na pluski sa w fight.c
    return;
}

void do_damn( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    char corpse[ MAX_INPUT_LENGTH ];
    char buf[ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj, *weapon_offhand, *dead_body;
    MOB_INDEX_DATA *cadaver;
    int skill = 0, chance;
    AFFECT_DATA damnation, *affected;
    // sterowanie
    bool target_weapon   = FALSE;
    bool target_armor    = FALSE;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, corpse );

    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Co chcia³<&by¶/aby¶/a³oby¶> przekl±æ?\n\r", ch );
        return;
    }

    // sprawdzam poziom odpowiedniej umiejêtno¶ci
    if( !str_prefix(arg,"weapon") || !str_prefix(arg,"broñ"))
    {
        skill = get_skill( ch, gsn_damn_weapon );
        target_weapon = TRUE;
    }
    else if(!str_prefix(arg,"armor") || !str_prefix(arg,"pancerz"))
    {
        skill = get_skill( ch, gsn_damn_armor );
        target_armor = TRUE;
    }

    // jak nie ma umiejêtno¶ci, to siê wynosimy
    if( skill == 0 )
    {
        if ( target_weapon )
        {
            send_to_char( "D³ugo i namiêtnie przeklinasz broñ, ale nic siê nie dzieje.\n\r", ch );
            act( "$n d³ugo i namiêtnie przeklina broñ, ale nic siê nie dzieje.", ch, NULL, NULL, TO_ROOM );
        }
        else if ( target_armor )
        {
            send_to_char( "D³ugo i namietnie przeklinasz pancerz, ale nic siê nie dzieje.\n\r", ch );
            act( "$n d³ugo i namiêtnie przeklina pancerz, ale nic siê nie dzieje.", ch, NULL, NULL, TO_ROOM );
        }
        WAIT_STATE( ch, UMAX( 1, skill_table[ gsn_damn_weapon ].beats / 2 ) );
        return;
    }

    if ( !target_weapon && !target_armor )
    {
        send_to_char( "Chcia³<&by¶/aby¶/a³oby¶> przekl±æ broñ czy pancerz?\n\r", ch );
        return;
    }

    // jak nie ma cia³a, lub jest nieodpowiednie, to te¿ siê wynosimy
    if ( corpse[ 0 ] == '\0' ||
            (( dead_body = get_obj_list( ch, corpse, ch->in_room->contents ) ) == NULL ) ||
            (( dead_body->item_type != ITEM_CORPSE_NPC ) && ( dead_body->item_type != ITEM_CORPSE_PC ) ) ||
            (( cadaver = get_mob_index( dead_body->value[ 0 ] ) ) == NULL ) ||
            ( IS_EVIL( cadaver ))||
            !IS_SET( race_table[ cadaver->race ].type, PERSON) ||
            dead_body->timer < 4 )
    {
        send_to_char("Aby przekl±æ przedmiot potrzebujesz ¶wie¿ych zw³ok dobrej lub neutralnej osoby.\n\r", ch );
        return;
    }

    if( target_weapon )
    {
        bool has_primary_weapon = TRUE; // domyslnie tak dla pierwszej broni
        bool has_offhand_weapon = TRUE; // domyslnie tak dla drugiej broni

        if ((( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL ) ||  obj->item_type != ITEM_WEAPON )
        {
            has_primary_weapon = FALSE;
        }
        if ((( weapon_offhand = get_eq_char( ch, WEAR_SECOND ) ) == NULL ) ||  weapon_offhand->item_type != ITEM_WEAPON )
        {
            has_offhand_weapon = FALSE;
        }

        if ( !has_primary_weapon && !has_offhand_weapon )
        {
            send_to_char( "Nie trzymasz teraz w d³oniach ¿adnej broni.\n\r", ch );
            return;
        }

        if (
                ( has_primary_weapon && IS_OBJ_STAT( obj, ITEM_MAGIC ) )
                ||
                ( has_offhand_weapon && IS_OBJ_STAT( weapon_offhand, ITEM_MAGIC ) )
           )
        {
            send_to_char( "Nie dasz rady przekln±æ swojej broni.\n\r", ch );
            return;
        }

        if ( has_primary_weapon && has_offhand_weapon )
        {
            WAIT_STATE( ch, ( 3 * skill_table[ gsn_damn_weapon ].beats ) / 2 );
        }
        else
        {
            WAIT_STATE( ch, skill_table[ gsn_damn_weapon ].beats );
        }

        if ( has_primary_weapon )
        {
            for ( affected = obj->affected; affected != NULL; affected = affected->next )
            {
                if ( affected->type == gsn_damn_weapon )
                {
                    send_to_char( "Twoja pierwsza broñ jest ju¿ przeklêta.\n\r", ch );
                    return;
                }
            }
            if ( IS_OBJ_STAT( obj, ITEM_BLESS ) || IS_OBJ_STAT( obj, ITEM_ANTI_EVIL ) )
            {
                send_to_char( "Nie uda³o ci siê przekl±æ pierwszej broni.\n\r", ch );
                return;
            }
            //sama próba ustawia na stale takie extra
            EXT_SET_BIT( obj->extra_flags, ITEM_EVIL );
            EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );
        }

        if ( has_offhand_weapon )
        {
            for ( affected = weapon_offhand->affected; affected != NULL; affected = affected->next )
            {
                if ( affected->type == gsn_damn_weapon )
                {
                    send_to_char( "Twoja druga broñ jest ju¿ przeklêta.\n\r", ch );
                    return;
                }
            }
            if ( IS_OBJ_STAT( weapon_offhand, ITEM_BLESS ) || IS_OBJ_STAT( weapon_offhand, ITEM_ANTI_EVIL ) )
            {
                send_to_char( "Nie uda³o ci siê przekl±æ drugiej broni.\n\r", ch );
                return;
            }
            //sama próba ustawia na stale takie extra
            EXT_SET_BIT( weapon_offhand->extra_flags, ITEM_EVIL );
            EXT_SET_BIT( weapon_offhand->extra_flags, ITEM_ANTI_GOOD );
        }


        dead_body->value[0] = 0;

        chance = skill + ch->level/2 + get_curr_stat_deprecated(ch,STAT_CHA) - 10;

        // podwojny damn zmniejsza szanse
        if ( has_primary_weapon && has_offhand_weapon )
        {
            chance *= 2;
            chance /= 3;
        }

        chance = URANGE( 5, chance, 95 );

        if ( number_percent() < chance )
        {
            damnation.type = gsn_damn_weapon;
            damnation.level = cadaver->level;
            damnation.duration = URANGE(3, skill/7 + get_curr_stat_deprecated(ch,STAT_CHA) - 10, 15);
            damnation.rt_duration = 0;
            damnation.location = APPLY_NONE;
            damnation.modifier = 0;

            if ( has_primary_weapon )
            {
                if( !IS_WEAPON_STAT( obj, WEAPON_VAMPIRIC ) )
                {
                    damnation.where = TO_WEAPON;
                    damnation.location = WEAPON_VAMPIRIC;
                    damnation.bitvector = &AFF_NONE;
                }
                else
                {
                    damnation.where = TO_OBJECT;
                    damnation.bitvector = &ITEM_DARK;
                }
                affect_to_obj( obj, &damnation );
            }

            if ( has_offhand_weapon )
            {
                if( !IS_WEAPON_STAT( weapon_offhand, WEAPON_VAMPIRIC ) )
                {
                    damnation.where = TO_WEAPON;
                    damnation.location = WEAPON_VAMPIRIC;
                    damnation.bitvector = &AFF_NONE;
                }
                else
                {
                    damnation.where = TO_OBJECT;
                    damnation.bitvector = &ITEM_DARK;
                }
                affect_to_obj( weapon_offhand, &damnation );
            }

            damnation.location = APPLY_NONE;

            if ( has_primary_weapon )
            {
                if( !IS_OBJ_STAT( obj, ITEM_MAGIC ) )
                {
                    damnation.where = TO_OBJECT;
                    damnation.bitvector = &ITEM_MAGIC;
                    affect_to_obj( obj, &damnation );
                }

                if( !IS_OBJ_STAT( obj, ITEM_UNDESTRUCTABLE ) )
                {
                    damnation.where = TO_OBJECT;
                    damnation.bitvector = &ITEM_UNDESTRUCTABLE;
                    affect_to_obj( obj, &damnation );
                }

                if( !IS_OBJ_STAT( obj, ITEM_NODROP ) )
                {
                    damnation.where = TO_OBJECT;
                    damnation.bitvector = &ITEM_NODROP;
                    affect_to_obj( obj, &damnation );
                }

                if( !IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
                {
                    damnation.where = TO_OBJECT;
                    damnation.bitvector = &ITEM_NOREMOVE;
                    affect_to_obj( obj, &damnation );
                }

                if( !IS_OBJ_STAT( obj, ITEM_NOUNCURSE ) )
                {
                    damnation.where = TO_OBJECT;
                    damnation.bitvector = &ITEM_NOUNCURSE;
                    affect_to_obj( obj, &damnation );
                }

                print_char( ch, "Wyci±gasz duszê %s z cia³a i nasycasz ni± %s.\n\r", cadaver->name2, obj->name4 );
                act( "$n przeklina $h.", ch, obj, NULL, TO_ROOM );
            }

            if ( has_offhand_weapon )
            {
                if( !IS_OBJ_STAT( weapon_offhand, ITEM_MAGIC ) )
                {
                    damnation.where = TO_OBJECT;
                    damnation.bitvector = &ITEM_MAGIC;
                    affect_to_obj( weapon_offhand, &damnation );
                }

                if( !IS_OBJ_STAT( weapon_offhand, ITEM_UNDESTRUCTABLE ) )
                {
                    damnation.where = TO_OBJECT;
                    damnation.bitvector = &ITEM_UNDESTRUCTABLE;
                    affect_to_obj( weapon_offhand, &damnation );
                }

                if( !IS_OBJ_STAT( weapon_offhand, ITEM_NODROP ) )
                {
                    damnation.where = TO_OBJECT;
                    damnation.bitvector = &ITEM_NODROP;
                    affect_to_obj( weapon_offhand, &damnation );
                }

                if( !IS_OBJ_STAT( weapon_offhand, ITEM_NOREMOVE ) )
                {
                    damnation.where = TO_OBJECT;
                    damnation.bitvector = &ITEM_NOREMOVE;
                    affect_to_obj( weapon_offhand, &damnation );
                }

                if( !IS_OBJ_STAT( weapon_offhand, ITEM_NOUNCURSE ) )
                {
                    damnation.where = TO_OBJECT;
                    damnation.bitvector = &ITEM_NOUNCURSE;
                    affect_to_obj( weapon_offhand, &damnation );
                }

                print_char( ch, "Wyci±gasz duszê %s z cia³a i nasycasz ni± %s.\n\r", cadaver->name2, weapon_offhand->name4 );
                act( "$n przeklina $h.", ch, weapon_offhand, NULL, TO_ROOM );
            }

            check_improve( ch, NULL, gsn_damn_weapon, TRUE, 7 );
            sprintf(buf, "%s%s%s", "Zbeszczeszczone zw³oki ", race_table[ cadaver->race ].name2, " le¿± tutaj powoli gnij±c.");
            free_string(dead_body->description);
            dead_body->description = str_dup(buf);
            return;
        }
        else
        {
            act( "W ostatniej chwili dusza wyrywa siê z twojej sieci i nie udaje ci siê przekl±æ $f.", ch, obj, NULL, TO_CHAR );
            act( "$n próbuje przekl±æ $h, lecz $s kl±twy nie zosta³y wys³uchane.", ch, obj, NULL, TO_ROOM );
            check_improve( ch, NULL, gsn_damn_weapon, FALSE, 7 );

            sprintf(buf, "%s%s%s", "Zbeszczeszczone zw³oki ", race_table[ cadaver->race ].name2, " le¿± tutaj powoli gnij±c.");
            free_string(dead_body->description);
            dead_body->description = str_dup(buf);
        }
    }
    else if( target_armor )
    {
        if ((( obj = get_eq_char( ch, WEAR_BODY ) ) == NULL ) ||  obj->item_type != ITEM_ARMOR )
        {
            send_to_char( "Nie masz teraz na sobie ¿adnej zbroi.\n\r", ch );
            return;
        }

        if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
        {
            send_to_char( "Nie dasz rady przekln±æ tej zbroi.\n\r", ch );
            return;
        }

        for ( affected = obj->affected; affected != NULL; affected = affected->next )
            if ( affected->type == gsn_damn_armor )
            {
                send_to_char( "Ta zbroja jest ju¿ przeklêta.\n\r", ch );
                return;
            }

        WAIT_STATE( ch, skill_table[ gsn_damn_armor ].beats );

        if( IS_OBJ_STAT( obj, ITEM_BLESS ) ||
                IS_OBJ_STAT( obj, ITEM_ANTI_EVIL ))
        {
            send_to_char( "Nie uda³o ci siê przekl±æ tej zbroi.\n\r", ch );
            return;
        }

        //sama próba ustawia na stale takie extra
        EXT_SET_BIT( obj->extra_flags, ITEM_EVIL );
        EXT_SET_BIT( obj->extra_flags, ITEM_ANTI_GOOD );

        dead_body->value[0] = 0;

        chance = skill + ch->level/2 + get_curr_stat_deprecated(ch,STAT_CHA) - 10;

        chance = URANGE( 5, chance-10 , 95 );

        if ( number_percent() < chance )
        {
            damnation.where = TO_OBJECT;
            damnation.type = gsn_damn_armor;
            damnation.level = cadaver->level;
            damnation.duration = URANGE(3, skill/7 + get_curr_stat_deprecated(ch,STAT_CHA) - 10, 15);             damnation.rt_duration = 0;
            damnation.location = APPLY_NONE;
            damnation.modifier = 0;
            damnation.bitvector = &ITEM_DARK;
            affect_to_obj( obj, &damnation );

            if( !IS_OBJ_STAT( obj, ITEM_MAGIC ) )
            {
                damnation.where = TO_OBJECT;
                damnation.bitvector = &ITEM_MAGIC;
                affect_to_obj( obj, &damnation );
            }

            if( !IS_OBJ_STAT( obj, ITEM_UNDESTRUCTABLE ) )
            {
                damnation.where = TO_OBJECT;
                damnation.bitvector = &ITEM_UNDESTRUCTABLE;
                affect_to_obj( obj, &damnation );
            }

            if( !IS_OBJ_STAT( obj, ITEM_NODROP ) )
            {
                damnation.where = TO_OBJECT;
                damnation.bitvector = &ITEM_NODROP;
                affect_to_obj( obj, &damnation );
            }

            if( !IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
            {
                damnation.where = TO_OBJECT;
                damnation.bitvector = &ITEM_NOREMOVE;
                affect_to_obj( obj, &damnation );
            }

            if( !IS_OBJ_STAT( obj, ITEM_NOUNCURSE ) )
            {
                damnation.where = TO_OBJECT;
                damnation.bitvector = &ITEM_NOUNCURSE;
                affect_to_obj( obj, &damnation );
            }

            print_char( ch, "Wyci±gasz duszê %s z cia³a i nasycasz ni± %s.\n\r", cadaver->name2, obj->name4 );
            act( "$n przeklina $h.", ch, obj, NULL, TO_ROOM );
            check_improve( ch, NULL, gsn_damn_armor, TRUE, 7 );

            sprintf(buf, "%s%s%s", "Zbeszczeszczone zw³oki ", race_table[ cadaver->race ].name2, " le¿± tutaj powoli gnij±c.");
            free_string(dead_body->description);
            dead_body->description = str_dup(buf);
            return;
        }
        else
        {
            act( "W ostatniej chwili dusza wyrywa siê z twojej sieci i nie udaje ci siê przekl±æ $f.", ch, obj, NULL, TO_CHAR );
            act( "$n próbuje przekl±æ $h, lecz $s kl±twy nie zosta³y wys³uchane.", ch, obj, NULL, TO_ROOM );
            check_improve( ch, NULL, gsn_damn_armor, FALSE, 7 );

            sprintf(buf, "%s%s%s", "Zbeszczeszczone zw³oki ", race_table[ cadaver->race ].name2, " le¿± tutaj powoli gnij±c.");
            free_string(dead_body->description);
            dead_body->description = str_dup(buf);
        }
        return;
    }

    return;
}

void do_call_avatar( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *avatar;
    AFFECT_DATA af;
    char name[ MAX_INPUT_LENGTH ];
    char short_d[ MAX_INPUT_LENGTH ];
    char long_d[ MAX_INPUT_LENGTH ];
    int avatar_level, avatar_hit, skill;

    if ( ( skill = get_skill( ch, gsn_call_avatar ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    // jak kto¶ nie chce byæ followany
    if ( EXT_IS_SET( ch->act, PLR_NOFOLLOW ) )
    {
        send_to_char( "Przecie¿ nie chcesz ¿eby kto¶ za tob± chodzi³.\n\r", ch );
        return ;
    }

    // no jak stoii w private, to przecie gdzie to summ±æ?
    if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_PRIVATE ) )
    {
        send_to_char( "To siê tutaj nie uda, jest za ma³o miejsca.\n\r", ch );
        return ;
    }

    // jak kto¶ ma mniej ni¿ 15 charyzmy, to niech spada
    if ( get_curr_stat_deprecated( ch, STAT_CHA ) < 15 )
    {
        print_char( ch, "Nie dasz rady kontrolowaæ awatara sfery negatywnej.\n\r" );
        return ;
    }

    if ( is_affected( ch, gsn_call_avatar ) )
    {
        send_to_char( "Nie dasz rady tak czêsto przywo³ywaæ awatara sfery negatywnej.\n\r", ch );
        return;
    }
    WAIT_STATE( ch, skill_table[ gsn_call_avatar ].beats );

    avatar_level = URANGE( 1, skill / 5, 20 );
    if ( get_curr_stat_deprecated(ch,STAT_CHA) > number_range( 12, 27 ) ) ++avatar_level;
    if ( get_curr_stat_deprecated(ch,STAT_CHA) < number_range( 4, 28 ) ) --avatar_level;
    avatar_level = UMAX( 1, avatar_level );

    //avatar ma hapki jak gracz, ktory na 1 poziomie ma 35-40 hp, i na poziom
    //dostaje 15-20 hp. I premie(b±d¼ nie-premie:P) od feata i charyzmy
    avatar_hit = 20 + avatar_level * 14 + dice( avatar_level, 6 ) ;
    if ( get_curr_stat_deprecated(ch,STAT_CHA) > number_range( 12, 27 ) ) avatar_hit += avatar_hit/15;
    if ( get_curr_stat_deprecated(ch,STAT_CHA) < number_range( 4, 28 ) ) avatar_hit -= avatar_hit/10;
    avatar_hit = UMAX( 1, avatar_hit );

    avatar = create_mobile( get_mob_index( MOB_VNUM_AVATAR ) );
    char_to_room( avatar, ch->in_room );
    EXT_SET_BIT( avatar->act, ACT_NO_EXP );
    money_reset_character_money( avatar );
    avatar->level = avatar_level;
    avatar->hit = avatar_hit ;
    avatar->max_hit = avatar->hit;
    avatar->hit = get_max_hp(avatar);
    avatar->hitroll = URANGE( 0, skill/24, 4 );
    avatar->damage[ DICE_NUMBER ] = URANGE( 2, 2 + skill/40, 4);
    avatar->damage[ DICE_TYPE ] = 5;
    avatar->damage[ DICE_BONUS ] = URANGE( 1, skill/19, 5 );

    avatar->armor[ AC_PIERCE ] = URANGE( -19, 81 - skill , 80 );
    avatar->armor[ AC_BASH ] = URANGE( -19, 81 - skill , 80 );
    avatar->armor[ AC_SLASH ] = URANGE( -19, 81 - skill , 80 );
    avatar->armor[ AC_EXOTIC ] = URANGE( -19, 81 - skill , 80 );

    if( avatar->level > 20 )//feat mroczny pakt praktycznie tylko pozwala
    {
        avatar->damroll += URANGE( 2, (get_curr_stat_deprecated( ch, STAT_CHA)-18)/2, 5 );
    }

    switch( avatar->level )
    {
        case 1:
        case 2:
        case 3:
        case 4:
            sprintf( name, "pomniejszy cieñ" );
            sprintf( short_d, "pomniejszy cieñ" );
            sprintf( long_d, "Dziwna ma³a plama ciemno¶ci unosi siê nad ziemi±." );
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            sprintf( name, "cieñ" );
            sprintf( short_d, "cieñ" );
            sprintf( long_d, "Dziwna plama ciemno¶ci unosi siê nad ziemi±." );
            break;
        case 9:
        case 10:
        case 11:
        case 12:
            sprintf( name, "wiêkszy cieñ" );
            sprintf( short_d, "wiêkszy cieñ" );
            sprintf( long_d, "Dziwna wielka plama ciemno¶ci unosi siê nad ziemi±." );
            break;
        case 13:
        case 14:
        case 15:
        case 16:
            sprintf( name, "cienisty lord" );
            sprintf( short_d, "cienisty lord" );
            sprintf( long_d, "Widzisz majacz±cy tutaj ogromny cieñ, od którego bije niesamowity ch³ód." );
            break;
        case 17:
        case 18:
        case 19:
        case 20:
            sprintf( name, "w³adca cieni" );
            sprintf( short_d, "w³adca cieni" );
            sprintf( long_d, "Cieñ, który tutaj widzisz pokrywa ca³± okolicê, rozsiewaj±c dooko³a siebie uczucia strachu i grozy." );
            break;
        default:
            sprintf( name, "awatar ¶mierci cieñ" );
            sprintf( short_d, "awatar ¶mierci" );
            sprintf( long_d, "Budz±cy grozê cieñ o kszta³ce zakapturzonego cz³owieka z kos± sunie lekko nad ziemi±." );
            break;
    }

    free_string( avatar->name );
    avatar->name = str_dup( name );
    free_string( avatar->short_descr );
    avatar->short_descr = str_dup( short_d );
    free_string( avatar->long_descr );
    avatar->long_descr = str_dup( long_d );

    if ( skill > 66 )
        EXT_SET_BIT( avatar->off_flags, OFF_TWO_ATTACK );
    else if( skill > 33 )
        EXT_SET_BIT( avatar->off_flags, OFF_ONE_HALF_ATTACK );

    if( !add_charm( ch, avatar, TRUE ) )
    {
        extract_char( avatar, TRUE );
        return;
    }

    act( "Tu¿ przed tob± z nico¶ci formuje siê $N.", ch, NULL, avatar, TO_CHAR );
    act( "Tu¿ przed $v z nico¶ci formuje siê $N.", ch, NULL, avatar, TO_NOTVICT );

    if ( HAS_TRIGGER( avatar, TRIG_ONLOAD ) )
    {
        mp_onload_trigger( avatar );
    }

    af.where = TO_AFFECTS;
    af.type = gsn_domination;
    af.level = ch->level;
    af.duration = -1;     af.rt_duration = 0;
    af.location = 0;
    af.modifier = 0;
    af.bitvector = &AFF_CHARM;
    affect_to_char( avatar, &af, NULL, TRUE );

    af.where = TO_AFFECTS;
    af.type = gsn_call_avatar;
    af.level = 50;
    af.duration = 24;     af.rt_duration = 0;
    af.location = 0;
    af.modifier = 0;
    af.visible = FALSE;
    af.bitvector = &AFF_NONE;
    affect_to_char( ch, &af, NULL, FALSE );

    add_follower( avatar, ch, TRUE );

    //240 to tick, czyli zawsze na 10 tickow summon
    create_event( EVENT_EXTRACT_CHAR, 2400, avatar, NULL, 0 );

    check_improve( ch, NULL, gsn_call_avatar, TRUE, 5 );
    return ;
}

void do_hustle( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim, *old_tank, *enemy;
    sh_int skill;

    one_argument( argument, arg );

    if ( ( skill = get_skill( ch, gsn_hustle ) ) <= 0 )
    {
        send_to_char( "Huh ?\n\r", ch );
        return;
    }

    if ( ch->fighting == NULL )
    {
        send_to_char( "Przecie¿ nie walczysz.\n\r", ch );
        return;
    }

    //
    old_tank = ch->fighting->fighting;
    enemy = ch->fighting;
    if ( !enemy || !old_tank )
    {
    	send_to_char( "To nie ma sensu.\n\r", ch );
        return;
    }

    if ( arg[ 0 ] == '\0' )
    {
        act( "Kogo wepchn±æ pod ciosy $Z?", ch, NULL, enemy, TO_CHAR );
        return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        return;
    }

    if ( IS_SET( ch->fight_data, FIGHT_TORMENT_NOFLEE ) )
    {
        print_char( ch, "Nie jeste¶ w stanie wepchn±æ nikogo przed twojego przeciwnika. Teraz %s przys³ania ci ca³y ¶wiat.\n\r", ch->fighting ? ( IS_NPC(ch->fighting) ? ch->fighting->short_descr : ch->fighting->name ) : "przeciwnik" );
        return;
    }

    if ( victim == ch )
    {
        if ( old_tank == ch )
            send_to_char( "To by nic nie zmieni³o.\n\r", ch );
        else
            send_to_char( "Nie potrafisz tego zrobiæ.\n\r", ch );
        return;
    }

    if ( old_tank == victim )
    {
        send_to_char( "To by nic nie zmieni³o.\n\r", ch );
        return;
    }

    if ( victim->fighting != enemy )
    {
        act("$N nie walczy z tym samym wrogiem co ty.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if ( victim->position <= POS_SITTING )
    {
        act("$N nie stoi przecie¿ na nogach.", ch, NULL, victim,TO_CHAR );
        return;
    }

    WAIT_STATE( ch, skill_table[ gsn_hustle ].beats );

    skill += URANGE( 0, get_curr_stat_deprecated( ch, STAT_DEX ) - 14, 10 );

    //zaleznosc od wagi przekopiowana z basha
    if ( victim->weight > 0 && ch->weight > 0 )
    {
        if ( ch->weight > victim->weight )
        {
            skill += 5 * ch->weight / victim->weight;
        }
        else
        {
            if ( ch->weight < victim->weight / 2 )
                skill = UMIN( skill, 5 );
            else
                skill = skill * ch->weight / victim->weight;
        }
    }

    if ( EXT_IS_SET( ch->off_flags, OFF_FAST ) || IS_AFFECTED( ch, AFF_HASTE ) )
        skill += 10;

    if ( number_percent( ) > skill )
    {
        act( "{5Próbujesz wepchn±æ $C tu¿ przed waszego przeciwnika, jednak nie udaje ci siê.{x", ch, NULL, victim, TO_CHAR );

        if ( ch->sex == 1 || ch->sex == 0 )
        {
            act( "{5$n próbuje wepchn±æ ciê tu¿ przed waszego przeciwnika, ale mu siê nie udaje.{x", ch, NULL, victim, TO_VICT );
            act( "{5$n próbuje wepchn±æ $C tu¿ przed ich przeciwnika, ale mu siê nie udaje.{x", ch, NULL, victim, TO_NOTVICT );
        }
        else
        {
            act( "{5$n próbuje wepchn±æ ciê tu¿ przed waszego przeciwnika, ale jej siê nie udaje.{x", ch, NULL, victim, TO_VICT );
            act( "{5$n próbuje wepchn±æ $C tu¿ przed ich przeciwnika, ale jej siê nie udaje.{x", ch, NULL, victim, TO_NOTVICT );
        }
        check_improve( ch, NULL, gsn_hustle, FALSE, 15 );
        return;
    }

    act( "{5Wpychasz $C tu¿ przed swojego przeciwnika.{x", ch, NULL, victim, TO_CHAR );
    act( "{5$n wpycha ciê tu¿ przed twojego przeciwnika.{x", ch, NULL, victim, TO_VICT );
    act( "{5$n wpycha $C tu¿ przed swojego przeciwnika.{x", ch, NULL, victim, TO_NOTVICT );
    check_improve( ch, NULL, gsn_hustle, TRUE, 15 );

    stop_fighting( old_tank, FALSE );
    stop_fighting( enemy, FALSE );
    stop_fighting( victim, FALSE );
    set_fighting( victim, enemy );
    set_fighting( enemy, victim );
    set_fighting( old_tank, enemy );

    return;
}

void do_control_undead( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim, *controled_1 = NULL, *controled_2 = NULL;
    CHARM_DATA * new_charm, *charm;
    int skill, charmies = 0;
    AFFECT_DATA af;
    OBJ_DATA *item_turner = NULL, *obj, *spellbook;

    if ( IS_NPC(ch))
        return;

    if ( ( skill = get_skill( ch, gsn_control_undead ) ) == 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->mount )
    {
        send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
        return;
    }

    // jak kto¶ nie chce byæ followany
    if ( EXT_IS_SET( ch->act, PLR_NOFOLLOW ) )
    {
        send_to_char( "Przecie¿ nie chcesz ¿eby kto¶ za tob± chodzi³.\n\r", ch );
        return ;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
        if ( obj->item_type == ITEM_TURN )
        {
            switch ( obj->wear_loc )
            {
                case WEAR_HOLD:
                case WEAR_NECK_1:
                case WEAR_NECK_2:
                    item_turner = obj;
                    break;
                default: break;
            }
            if ( item_turner )
                break;
        }
    }

    if ( !item_turner )
    {
        send_to_char( "Postaraj siê o jaki¶ symbol wiary.\n\r", ch );
        return;
    }

    if ( item_turner->wear_loc != WEAR_HOLD && get_hand_slots( ch, WEAR_NONE ) >= 2 )
    {
        send_to_char( "Musisz mieæ woln± rêkê aby to uczyniæ.\n\r", ch );
        return;
    }

    /* dajemy standardowy tekst jak go¶æ jest pod wp³ywem 'silence' */
    if ( IS_AFFECTED( ch, AFF_SILENCE ) )
    {
        send_to_char( AFF_SILENCE_TEXT, ch );
        return ;
    }

    if ( is_affected(ch,gsn_control_undead))
    {
        send_to_char("Nie masz na to si³y, odpocznij kilka godzin.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_HIDE ) ) affect_strip( ch, gsn_hide );

    strip_invis( ch, TRUE, TRUE );
    skill += item_turner->value[ 0 ];
    skill += (get_curr_stat_deprecated( ch, STAT_CHA ) - 16)*2;
    af.where = TO_AFFECTS;
    af.type = gsn_control_undead;
    af.level = 50;
    af.location = APPLY_NONE;
    af.duration = URANGE( 8, (150 - skill)/6, 25 );     af.rt_duration = 0;
    af.modifier = 0;
    af.bitvector = &AFF_NONE;
    affect_to_char( ch, &af, NULL, FALSE );

    SET_BIT( ch->fight_data, FIGHT_CASTING );

    print_char( ch, "{5G³o¶no wymawiasz mroczne s³owa, trzymaj±c w wyci±gnietej rece %s.{x\n\r", item_turner->name4 );
    act( "{5$n wymawia mroczno brzmi±ce s³owa, w wyciagnietej przed siebie rêce mocno ¶ciska $h.{x", ch, item_turner, NULL, TO_ROOM );

    WAIT_STATE( ch, skill_table[ gsn_control_undead ].beats );

    for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
    {
        if ( !IS_NPC( victim ) )
            continue;

        if ( !is_undead(victim) || !can_see( ch, victim ) || is_safe( ch, victim ) || !can_see( victim, ch ) )
            continue;

        if ( victim->master != NULL && !IS_NPC( victim->master ) )
            continue;

        for ( charm = ch->pcdata->charm_list; charm; charm = charm->next )
            charmies++;

        skill += ( 8 - victim->level )*2;

        if ( controled_2 != NULL ||
                number_percent() > skill ||
                victim->level+number_range(3,9) > ch->level ||
                victim->level >= 24 ||
                (( get_curr_stat_deprecated( ch, STAT_CHA ) < 22 && charmies > 1 ) || charmies > 2 ) )
        {
            if ( !victim->fighting && can_move( victim ) )
            {
                act( "{5$N rzuca siê z krzykiem na $c.{x", ch, NULL, victim, TO_ROOM );
                print_char( ch, "{5%s rzuca siê na ciebie z krzykiem.{x\n\r", capitalize( victim->short_descr ) );
                check_improve( ch, NULL, gsn_control_undead, FALSE, 5 );
                set_fighting( victim, ch );
                set_fighting( ch, victim );
            }
            continue;
        }

        for( spellbook = victim->carrying; spellbook != NULL; spellbook = spellbook->next_content )
        {
            if( !spellbook || spellbook->item_type != ITEM_SPELLBOOK )
                continue;

            act( "$p rozp³ywa siê w dym.", ch, spellbook, NULL, TO_ALL );
            extract_obj( spellbook );
        }

        if ( controled_1 == NULL )
            controled_1 = victim;
        else
            controled_2 = victim;

        ++charmies;
    }

    if ( controled_1 != NULL )
    {
        controled_1->group = 0;
        controled_1->master = NULL;
        new_charm = ( CHARM_DATA * ) malloc( sizeof( *new_charm ) );
        new_charm->type = 0;
        new_charm->victim = controled_1;
        new_charm->next = ch->pcdata->charm_list;
        ch->pcdata->charm_list = new_charm;

        af.where = TO_AFFECTS;
        af.type = gsn_control_undead;
        af.level = ch->level;
        af.duration = 5 + URANGE( 1, skill/5, 20);         af.rt_duration = 0;
        af.location = 0;
        af.modifier = 0;
        af.bitvector = &AFF_CHARM;
        affect_to_char( controled_1, &af, NULL, TRUE );

        add_follower( controled_1, ch, TRUE );

        act( "Przejmujesz kontrolê nad $V.", ch, NULL, controled_1, TO_CHAR );
        check_improve( ch, NULL, gsn_control_undead, TRUE, 4 );

        if ( controled_1->resists[ RESIST_MAGIC_WEAPON ] )
            controled_1->resists[ RESIST_MAGIC_WEAPON ] = 0;
    }

    if ( controled_2 != NULL )
    {
        controled_2->group = 0;
        controled_2->master = NULL;
        new_charm = ( CHARM_DATA * ) malloc( sizeof( *new_charm ) );
        new_charm->type = 0;
        new_charm->victim = controled_2;
        new_charm->next = ch->pcdata->charm_list;
        ch->pcdata->charm_list = new_charm;

        af.where = TO_AFFECTS;
        af.type = gsn_control_undead;
        af.level = ch->level;
        af.duration = 5 + URANGE( 1, skill/5, 20);         af.rt_duration = 0;
        af.location = 0;
        af.modifier = 0;
        af.bitvector = &AFF_CHARM;
        affect_to_char( controled_2, &af, NULL, TRUE );

        add_follower( controled_2, ch, TRUE );

        act( "Przejmujesz kontrolê nad $V.", ch, NULL, controled_2, TO_CHAR );
        check_improve( ch, NULL, gsn_control_undead, TRUE, 5 );

        if ( controled_2->resists[ RESIST_MAGIC_WEAPON ] )
            controled_2->resists[ RESIST_MAGIC_WEAPON ] = 0;
    }

    return;
}

/*
   No siê ta komenda nazywa throw - czyli rzuæ.
   Na razie obs³uguje tylko rzut moneta...
   Ale jest w skills.c bo kiedys pewnie dojdzie skill
   dla kogos "throw" i siê podepnie ogolnie o wiele wiecej
   fajnych rzeczy. Wiec niech to tu zostanie :)
   - Loop
 */

void do_throw( CHAR_DATA *ch, char *argument )
{
    char arg1[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg1 );

    if ( arg1[ 0 ] == '\0' )
    {
        send_to_char( "Czym chcesz rzuciæ?\n\r", ch );
        return;
    }
    else if ( !str_prefix( arg1, "monet±" ) || !str_prefix( arg1, "monetê" ) || !str_prefix( arg1, "coin" ))
    {
        if ( ch->copper < 1 )
        {
            send_to_char( "Nie masz ¿adnej miedzianej monety, któr± móg³by¶ rzuciæ.\n\r", ch );
            act( "$n robi g³upi± mine na widok swojej sakiewki.", ch, NULL, NULL, TO_ROOM );
            return;
        }
        else
        {
            if ( number_percent() < 5 )
            {
                send_to_char( "Podrzucasz bardzo wysoko monetê, a ta dziwnie wiruj±c spada na ziemiê!\n\r", ch );
                act( "$n niezrêcznie podrzuca monetê, która ko¶lawo wiruj±c spada na ziemiê!", ch, NULL, NULL, TO_ROOM );
                ch->copper -= 1;
                obj_to_room( create_money( 1, 0, 0, 0 ), ch->in_room );
                return;
            }
            else if ( number_percent() > 50 )
            {
                send_to_char( "Podrzucasz wysoko monetê, ta wiruj±c opada na twoj± otwart± d³oñ.\n\r", ch );
                act( "$n wysoko podrzuca monetê, która wiruj±c opada na $s d³oñ.", ch, NULL, NULL, TO_ROOM );
                send_to_char( "Pokazujesz wszystkim, ¿e moneta opad³a na awers.\n\r", ch );
                act( "$n pokazuje wszystkim awers monety.", ch, NULL, NULL, TO_ROOM );
                return;
            }
            else
            {
                send_to_char( "Podrzucasz wysoko monetê, ta wiruj±c opada na twoj± otwart± d³oñ.\n\r", ch );
                act( "$n wysoko podrzuca monetê, która wiruj±c opada na $s d³oñ.", ch, NULL, NULL, TO_ROOM );
                send_to_char( "Pokazujesz wszystkim, ¿e moneta opad³a na rewers.\n\r", ch );
                act( "$n pokazuje wszystkim rewers monety.", ch, NULL, NULL, TO_ROOM );
                return;
            }
        }
    }
    else
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }
}

void do_pre_holy_prayer( CHAR_DATA *ch, char *argument )
{
    if ( get_skill( ch, gsn_holy_prayer ) == 0 )
    {
        switch ( number_range(0,2) )
        {
            case 0:
                send_to_char( "Usilnie modlisz siê do swojego boga, ale chyba aktualnie ciê nie s³ucha.\r", ch );
                act( "$n mamrocze co¶ niewyra¼nie pod nosem.", ch, NULL, NULL, TO_ROOM );
                break;
            case 1:
                send_to_char( "Wzywasz gestami, swoich bogów, ale nic siê nie dzieje.\r", ch );
                act( "$n rzuca siê niespokojnie.", ch, NULL, NULL, TO_ROOM );
                break;
            case 2:
                send_to_char( "Starasz siê modliæ, ale nie bardzo wiesz jak siê do tego zabraæ.\r", ch );
                act( "$n wykonuje dziwne, zupe³nie nie zrozumia³e gesty...", ch, NULL, NULL, TO_ROOM );
                break;
        }
        return;
    }

    if ( ch->mount )
    {
        send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
        return;
    }

    if ( is_affected( ch, gsn_holy_prayer ) )
    {
        send_to_char( "Jeszcze nie nadszed³ czas na kolejn± modlitwe.\n\r", ch );
        return;
    }

    if ( ch->fighting )
    {
        send_to_char( "Lepiej skup siê na walce.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED( ch, AFF_FLOAT ))
    {
        if ( skill_table[ gsn_fly ].msg_off )
        {
            send_to_char( skill_table[ gsn_fly ].msg_off, ch );
            send_to_char( "\n\r", ch );
        }
        affect_strip( ch, gsn_fly );
        affect_strip( ch, gsn_float );
    }
    ch->position = POS_SITTING;

    wait_printf(ch,0,"Bierzesz g³êboki wdech i starasz siê wyciszyæ.\n\r");
    wait_printf(ch,3,"Klêkasz i pochylaj±c pokornie g³owê zaczynasz siê modliæ.\n\r");
    wait_printf(ch,9,"Przez chwilê wydaje ci siê, ¿e ca³y ¶wiat zalewa bia³e ¶wiat³o, czujesz jakby przypatrwa³a siê tobie jaka¶ pote¿na si³a.\n\r");

    wait_act( 0, "$n bierze g³êboki wdech i przystaje na chwilê w milczeniu.", ch, NULL, NULL, TO_ROOM );
    wait_act( 3, "$n klêka i pochylaj±c g³owê zaczyna siê cicho modliæ.", ch, NULL, NULL, TO_ROOM );
    wait_act( 9, "$n wymawia szeptem kolejne s³owa modlitwy z natchnionym wyrazem twarzy.", ch, NULL, NULL, TO_ROOM );

    wait_function(ch,12, &do_holy_prayer, "" );
    WAIT_STATE( ch, 54 );
}

void do_holy_prayer( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af, *paf;
    int skill, deeds, throw;

    skill = get_skill( ch, gsn_holy_prayer );

    if ( !is_affected( ch, gsn_prayer_last ) )
    {
        af.where	= TO_AFFECTS;
        af.type		= gsn_prayer_last;
        af.level	= 50;
        af.duration = -1;         af.rt_duration = 0;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = &AFF_NONE;
        af.visible	 = FALSE;
        affect_to_char( ch, &af, NULL, FALSE );

        af.type		= gsn_holy_prayer;
        af.duration = 8;         af.rt_duration = 0;
        affect_to_char( ch, &af, NULL, FALSE );

        if ( !is_affected( ch, gsn_deeds_pool ) )
        {
            af.type		= gsn_deeds_pool;
            af.duration = -1;             af.rt_duration = 0;
            affect_to_char( ch, &af, NULL, FALSE );
        }

        send_to_char( "Koñcz±c modlitwe czujesz, ¿e od tej chwili twoje czyny bed± obserwowane przez boga.\n\r", ch );
        act( "$n wstaje z kolan i niepewnie siê u¶miecha.", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_STANDING;

        check_improve( ch, NULL, gsn_holy_prayer, TRUE, 5 );
        return;
    }

    af.where	= TO_AFFECTS;
    af.type		= gsn_holy_prayer;
    af.level	= 50;
    af.duration = 12;     af.rt_duration = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = &AFF_NONE;
    af.visible	 = FALSE;
    affect_to_char( ch, &af, NULL, FALSE );

    if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) == NULL )
    {
        send_to_char( "Sta³o siê co¶ bardzo, bardzo z³ego.\n\r", ch );
        return;
    }
    paf->modifier = 0;

    // no to sprawdzamy jak paladyn byl grzeczny

    if ( ( paf = affect_find( ch->affected, gsn_deeds_pool )) == NULL )
    {
        send_to_char( "Sta³o siê co¶ bardzo, bardzo z³ego.\n\r", ch );
        return;
    }
    deeds = paf->modifier;
    paf->modifier = 0;

    if ( !is_affected( ch, gsn_holy_pool ) )
    {
        af.where	= TO_AFFECTS;
        af.type		= gsn_holy_pool;
        af.level	= 50;
        af.duration = -1;         af.rt_duration = 0;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = &AFF_NONE;
        af.visible	 = FALSE;

        affect_to_char( ch, &af, NULL, FALSE );
    }
    paf = affect_find( ch->affected, gsn_holy_pool );

    if ( deeds < 0 )
    {
        // he was very bad, bad person ;]
        paf->modifier += deeds;
        paf->modifier = UMAX( paf->modifier, -(skill) * 2 );

        ch->position = POS_STANDING;
        switch ( number_range(0,4) )
        {
            case 0:
                send_to_char( "Wraz z koñcem modlitwy czujesz, jak ch³ód wype³nia twe cia³o.\n\r", ch );\
                    act( "$n koñcz±c modlitwe trzêsiê siê przez chwilê jakby zawia³ bardzo mro¼ny wiatr poczym wstaje z niezadowolon± min±.", ch, NULL, NULL, TO_ROOM );
                break;
            case 1:
                send_to_char( "Wraz z koñcem modlitwy czujesz, jak mróz mrozi twe kosci, a moc odp³ywa od ciebie.\n\r", ch );
                act( "$n koñcz±c modlitwe podrywa siê raptownie z kolan na równe nogi i rozgl±da siê z niepokojem.", ch, NULL, NULL, TO_ROOM );
                break;
            case 2:
                send_to_char( "Wraz z koñcem modlitwy czujesz, ¿e ziemia dr¿y przez chwilê... próbujesz siê podnie¶æ ale nag³y zawrót g³owy sprowadza ciê na ziemiê.\n\r", ch );
                act( "$n koñczy modlitwe i wstaje z kolan by po chwili zatoczyæ siê, i z przera¿eniem w oczach potykaj±c siê upa¶æ twarza do ziemi.", ch, NULL, NULL, TO_ROOM );
                ch->position = POS_SITTING;
                break;
            case 3:
                send_to_char( "Wraz z koñcem modlitwy czujesz ogromny wstyd, ³zy staj± ci w oczach a krzyki twych ofiar dudni± ci w g³owie.\n\r", ch );
                act( "$n koñcz±c modlitwe pochyla g³owê, wstaje i z ³zami w oczach unika ludzkich spojrzeñ. Wygl±da bardzo marnie.", ch, NULL, NULL, TO_ROOM );
                break;
            case 4:
                send_to_char( "Wraz z koñcem modlitwy czujesz ból w sercu i tak przeogromny wstyd, i¿ zaczynasz cicho chlipaæ.\n\r", ch );
                act( "$n koñcz±c modlitwe chwyta siê nagle za pier¶ i pochylaj±c ku ziemi g³owê zaczyna cichutko szlochaæ... co za niezwyk³y widok.", ch, NULL, NULL, TO_ROOM );
                break;
        }
        return;
    }
    else
    {
        // good pal got a holy pool
        deeds += skill / 5;

        throw = number_percent();

        if ( throw < skill )
        {
            // udany konwertuje %skilla deedsow na holy
            paf->modifier += UMAX( skill / 5, ( skill * deeds ) / 100 );
            switch ( number_range(0,4) )
            {
                case 0:
                    send_to_char( "Wraz z koñcem modlitwy czujesz w sobie przyp³yw enegii, która zdaje siê rozgrzewaæ ciê wewnetrznie.\n\r", ch );\
                        act( "$n koñczy modlitwe i kiwaj±c g³ow± z u¶miechem wstaje z klêczek.", ch, NULL, NULL, TO_ROOM );
                    break;
                case 1:
                    send_to_char( "Wraz z koñcem modlitwy czujesz rozlewaj±ce siê po twoim ciele ciep³o.\n\r", ch );\
                        act( "$n koñcz±c modlitwe bierze g³eboki oddech po czym wstaje z u¶miechem.", ch, NULL, NULL, TO_ROOM );
                    break;
                case 2:
                    send_to_char( "Wraz z koñcem modlitwy czujesz olbrzymi± falê mocy która wype³nia twoje cia³o.\n\r", ch );\
                        act( "$n koñczy modlitwe i wstaj±c zaciska d³onie jakby zbieraj±æ niewidzialna energie.", ch, NULL, NULL, TO_ROOM );
                    break;
                case 3:
                    send_to_char( "Wraz z koñcem modlitwy czujesz jak energia rozpiera twe cia³o, a potêga dobra pulsuje w twym wnêtrzu.\n\r", ch );\
                        act( "$n koñcz±c modlitwe bierze g³eboki oddech po czym wstaje z u¶miechem.", ch, NULL, NULL, TO_ROOM );
                    break;
                case 4:
                    send_to_char( "Wraz z koñcem modlitwy czujesz siê jakby ca³e twe cia³o p³one³o ¶wiêtym ogniem.\n\r", ch );\
                        act( "$n koñcz±c modlitwe i u¶miechaj±c siê rado¶nie wstaje z ziemi.", ch, NULL, NULL, TO_ROOM );
                    break;
            }
            ch->position = POS_STANDING;
            check_improve( ch, NULL, gsn_holy_prayer, TRUE, 5 );
        }
        else if ( throw < skill * 2 )
        {
            paf->modifier += UMAX(skill / 8, ( skill * deeds ) / 150 );
            send_to_char( "Mylisz siê kilka razy w czasie modlitwy, lecz mimo tego czujesz przyp³yw si³.\n\r", ch );
            act( "$n konczy modlitwe i z niepewnym u¶miechem wstaje z kolan.", ch, NULL, NULL, TO_ROOM );
            ch->position = POS_STANDING;
            check_improve( ch, NULL, gsn_holy_prayer, TRUE, 5 );
        }
        else
        {
            paf->modifier += UMAX( skill / 10, ( skill * deeds ) / 200 );
            send_to_char( "Mylisz i pl±czesz s³owa modlitwy. Pomimo tego i tak czujesz siê lepiej.\n\r", ch );
            act( "$n konczy modlitwe i wstaje z kolan mrucz±c co¶ do siebie.", ch, NULL, NULL, TO_ROOM );
            ch->position = POS_STANDING;
            check_improve( ch, NULL, gsn_holy_prayer, FALSE, 5 );
        }

        paf->modifier = UMIN( paf->modifier, skill );
    }

    return;
}

void do_sanct( CHAR_DATA *ch, char *argument )
{
    return;
}

bool mine_check(CHAR_DATA *ch, int sk){
	ROOM_INDEX_DATA *room;
	if ( IS_NPC( ch ) )
		return FALSE;

	if ( !ch->in_room )
		return FALSE;

	room = ch->in_room;

	if ( IS_AFFECTED( ch, AFF_BLIND ) && !IS_AFFECTED(ch,AFF_PERFECT_SENSES) )
	{
		print_char( ch, "Jak? Na o¶lep?\n\r" );
		return FALSE;
	}

	if ( room_is_dark(ch, room ) && !IS_AFFECTED(ch, AFF_DARK_VISION) )
	{
		print_char( ch, "Po ciemku to mo¿esz przeprowadziæ co najwy¿ej wydobycie butelki z plecaka.\n\r" );
		return FALSE;
	}

	if ( sk <= 0 )
	{
		print_char( ch, "Musisz siê tego najpierw gdzie¶ nauczyæ.\n\r" );
		return FALSE;
	}

    if ( ch->mount )
    {
        send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
        return FALSE;
    }

    if ( room->sector_type != 14 //podziemia
        && room->sector_type != 17 //podziemia naturalne
        && room->sector_type != 15 //jaskinia
        && room->sector_type != 47 //kopalnia
        && room->sector_type != 45 //podziemna droga
        && room->sector_type != 13 //wysokie góry
        )
    {
        print_char( ch, "Na tym terenie chyba nie ma sensu próbowaæ czegokolwiek wydobywaæ.\n\r" );
        return FALSE;
    }

    return TRUE;
}
//struct rawmaterial_type
//{
//	char *name;
//	int bit;
//	int skill;
//	int vnum;
//	int vnum_big;
//};
void mine_prospect_quant(BUFFER* buf, ROOM_INDEX_DATA* room, int i, int sk){

  if (rawmaterial_table[i].skill > sk)
		return;

	char str[MAX_STRING_LENGTH];
	char* quant;

	if (sk > 70)
	{
		if (room->rawmaterial[i] > 80)
			quant = str_dup("naprawdê du¿o!");
		else if (room->rawmaterial[i] > 60)
			quant = str_dup("ca³kiem du¿o.");
		else if (room->rawmaterial[i] > 40)
			quant = str_dup("chyba warto kopaæ.");
		else if (room->rawmaterial[i] > 20)
			quant = str_dup("trochê tego jest.");
		else if (room->rawmaterial[i] > 0)
			quant = str_dup("chyba jest na wyczerpaniu.");
		else
			quant = str_dup("Brak.");
	}
	else if (sk > 50)
	{
		if (room->rawmaterial[i] > 75)
			quant = str_dup("w miarê sporo");
		else if (room->rawmaterial[i] > 50)
			quant = str_dup("wiêcej ni¿ trochê");
		else if (room->rawmaterial[i] > 25)
			quant = str_dup("niewiele");
		else if (room->rawmaterial[i] > 0)
			quant = str_dup("resztki");
		else
			quant = str_dup("Brak.");
	}
	else if (sk > 30)
	{
		if (room->rawmaterial[i] > 60)
			quant = str_dup("ca³kiem sporo");
		else if (room->rawmaterial[i] > 30)
			quant = str_dup("powinno wystarczyæ");
		else if (room->rawmaterial[i] > 0)
			quant = str_dup(" - resztki");
		else
			quant = str_dup("Brak.");
	}
	else if (sk > 10)
	{
		if (room->rawmaterial[i] > 50)
			quant = str_dup("wiêcej ni¿ mniej");
		else if (room->rawmaterial[i] > 0)
			quant = str_dup("mniej ni¿ wiêcej");
		else
			quant = str_dup("Brak.");
	}
	else
	{
		if (room->rawmaterial[i] > 0)
			quant = str_dup("co¶ takiego tu jest");
		else
			quant = str_dup("Brak.");
	}

	sprintf(str, "%s - %s\n\r", rawmaterial_table[i].name, quant);
	add_buf(buf, str);

}

void mine_prospect_res(CHAR_DATA *ch, ROOM_INDEX_DATA* room, int sk)
{

	BUFFER* buf;
	buf = new_buf();
	int i;

	add_buf( buf, "\n\r{b==<>==<>==<>==<>==<>==  Minera³y, które mo¿na tu wydobywaæ <>==<>==<>==<>==<>=={x\n\r");
	for ( i = 0; i < MAX_RAWMATERIAL ; i++)
	{
		mine_prospect_quant(buf, room, i, sk);
	}

	add_buf( buf, "{b==<>==<>==<>==<>==<>==<>==<>==<>==<>==<=>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x\n\r");
	page_to_char( buf_string( buf ), ch );
	free_buf( buf );
}

/**
 * Polecenie: mine prospect
 * Dzia³anie:
 * Sprawdza czy z³o¿e posiada jeszcze dostêpne materia³y (capacity) i
 * okre¶la rodzaj materia³ów (resources[]).
 *
 * W zale¿no¶ci od poziomu wyszkolenia mining informacje staj± siê coraz bardziej szczegó³owe.
 *
 * Kopacz widzi tylko takie zasoby, które jest w stanie wydobyæ (zale¿y od poziomu skilla).
 */
void mine_prospect( CHAR_DATA *ch )
{
	if ( !ch->in_room )
		return;

	ROOM_INDEX_DATA* room = ch->in_room;

	int sk = get_skill( ch, skill_lookup( "mining" ) );

	if(!mine_check(ch, sk))
		return;

	if(room->rawmaterial_capacity == 0){
		print_char(ch, "Nie ma tu czego kopaæ.\n\r");
		return;
	}

	//procentowa zawarto¶æ minera³ów w z³o¿u
	int proc_cap = (room->rawmaterial_capacity_now * 100 ) / room->rawmaterial_capacity;

	print_char(ch, "{b==<>==<>==<>==<>==<>==<>==  Ilo¶æ minera³ów do wydobycia  ==<>==<>==<>==<>==<>=={x\n\r");

	if(room->rawmaterial_capacity_now <= 0){
		print_char(ch, "W tym ¿³o¿u nie ma ju¿ wiêcej zasobów.\n\r");
		return;
	}
	//jak ma nie byc numerków, to mo¿na zakomentowaæ
	if(sk > 80){
		print_char(ch, "Badasz otoczenie i stwierdzasz, ¿e zosta³o jeszcze oko³o %d%% zasobów do wykopania", proc_cap);
	}
	else if(sk > 50){
		if(proc_cap < 20)
			print_char(ch, "Wykopano st±d prawie wszystko.");
		else if(proc_cap < 40)
			print_char(ch, "Co¶ tam jeszcze zosta³o - ale mniej tego ni¿ wiêcej.");
		else if(proc_cap < 60)
			print_char(ch, "Wydaje siê, ¿e jest co kopaæ.");
		else if(proc_cap < 80)
			print_char(ch, "Hmm. Ca³kiem sporo do wykopania.");
		else
			print_char(ch, "Ha! To dziewicza ¿y³a!");
	}
	else if(sk > 20){
		if(proc_cap < 30)
			print_char(ch, "Niewiele ju¿ tu zosta³o.");
		else if(proc_cap < 60)
			print_char(ch, "Co¶ siê tu powinno znale¿æ.");
		else
			print_char(ch, "Hmmm. Ca³kiem sporo do wykopania.");
	}
	else if(sk > 0){
		if(proc_cap < 50)
			print_char(ch, "To z³o¿e jest na tyle popularne, ¿e mo¿esz mieæ problemy z kopaniem.");
		else
			print_char(ch, "Z tego co widaæ, zagl±da³o tu niewielu górników.");
	}

	mine_prospect_res(ch, room, sk);
}

/**
 * Generowanie bry³ek w losowych rozmiarach
 */
OBJ_DATA* generate_brylka(int qu, int i, ROOM_INDEX_DATA *room){
	OBJ_DATA *brylka;
	if ( number_percent() < ( qu  * 4 ) )
	{
		brylka = create_object( get_obj_index( rawmaterial_table[i].vnum_big ), FALSE );
		room->rawmaterial_capacity_now -= 2;
	} else {
		brylka = create_object( get_obj_index( rawmaterial_table[i].vnum ), FALSE );
		--room->rawmaterial_capacity_now;
	}
	return brylka;
}


/**
 * mine - kopanie minera³ów
 * W zale¿no¶ci od typu sektora na którym siê kopie (podziemia, góry)
 * jako¶ci trzymanego kilofa (tool, kilof)
 * skilla (mining)
 * ustawionych parametrow (capacity, resorces)
 * mo¿na wykopaæ przedmioty okre¶lone w rawmaterial_table
 * (wêgiel, mied¼, srebro, z³oto, mitril, adamantyt, gemy)
 *
 * TODO:
 * - przerobiæ tak, aby ilo¶æ komunikatów "pozytywnych" by³a równa ilo¶ci bry³ek
 * - bry³ki ró¿nej wielko¶ci (opisy + zró¿nicowanie cen/mas)
 */
void do_mine( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA *room;
	OBJ_DATA *tool, *brylka;
	int sk, qu, i, j;
	char buf[MAX_STRING_LENGTH];
	int points, ile_wypadnie = 1, ile_poz_komunikatow = 0;
	int time;
	//CHAR_DATA *mob;
	bool other;
	bool vEventAggresiv = FALSE;
	int C_BASE_TIME = 20;
	OBJ_INDEX_DATA *obj_index;
	//MOB_INDEX_DATA *mob_index;
	char command [ MAX_STRING_LENGTH ];

	argument = one_argument( argument, command );
	if(!str_prefix(command, "prospect")){
		mine_prospect(ch);
		return;
	}

	if(command[0] != '\0')
	{
		send_to_char( "Co?\n\r", ch );
		return;
	};

	const int mes1 = 9;
	char *messages1[] =
	{
			"Z mozo³em uderzasz kilofem w ¶cianê.",
			"Niecierpliwie uderzasz kilofem.",
			"Uderzasz i znowu trafiasz nie tam gdzie trzeba.",
			"Walisz i walisz w tê ¶cianê i niewiele z tego po¿ytku.",
			"Bierzesz zamach i z ca³ych si³ uderzasz w ¶cianê jednak kilof odskakuje pozostawiaj±c ma³± ryskê.",
			"Czujesz coraz wiêkszy ból ramion z ka¿dym uderzeniem.",
			"Kolejne uderzenie, rêce ju¿ omdlewaj±.",
			"My¶lisz, ¿e to jednak nie robota dla ciebie.",
			"Przy kolejnym zamachu my¶lisz, ¿e czas zmieniæ zajêcie.",
			"Próbujesz wykuæ co¶ ze ¶ciany ale sypi± ci siê pod stopy jedynie ma³e okruchy i kamienny py³."
	};

	const int mes2 = 9;
	char *messages2[] =
	{
			"Z wpraw± wykuwasz kawa³ki ze ¶ciany.",
			"Z cierpliwo¶æi± uderzasz w obrane miejsce.",
			"Szybko i precyzyjnie! Tak nale¿y to robiæ.",
			"Z ca³ej si³y walisz w ¶cianê, a¿ odpryski ska³ ¶migaj± ci ko³o g³owy.",
			"Bierzesz zamach i z ca³ych si³ uderzasz w ¶cianê od³upuj±c wielk± bry³ê.",
			"Delikatnie, aby nie naruszyæ ca³o¶ci, odkuwasz ze ¶ciany ma³y fragment.",
			"Uderzasz raz po raz w ¶cianê wykuwaj±c ma³e kawa³ki surowca.",
			"Pracowicie r±biesz ska³ê wywo³uj±c co chwila kaskady iskier.",
			"Pod wp³ywem kolejnych uderzeñ ska³a poddaje siê i kruszy na drobne bry³ki.",
			"Z ka¿dym nowym uderzeniem odpada z hukiem ze ¶ciany fragment kamienia."
	};

	const int mes3 = 3;
	char *messages3[] =
	{
			"Ostatnim potê¿nym uderzeniem koñczysz pracê.",
			"Z kolejnym uderzeniem my¶lisz sobie, ¿e wystarczy.",
			"Mocno ju¿ wyczerpany koñczysz ostatnim uderzeniem.",
			"Zmêczony kuciem dostrzegasz na skale pêkniêcie, bierzesz wiêc potê¿ny zamach i z hukiem od³upujesz kamienn± p³ytê, która upada roztrzaskuj±c siê."
	};

	const int mes_newbie = 3;
	char *messages_newbie[] =
	{
			"Walisz w ¶cianê, ale s³ysz±c dobiegaj±ce zza niej odg³osy zmieniasz kierunek kopania.",
			"Jaki¶ niepokoj±ce odg³osy zza ¶ciany budz± twoj± czujno¶æ.",
			"Echa walki dobiegaj± zza ¶ciany i ostrzegaj± o gro¿±cym niebezpieczeñstwie.",
			"Przyk³adasz ucho do ¶ciany i wiesz z ca³± pewno¶ci±, ¿ê za ni± czeka co¶ niebezpiecznego."
	};

	if ( !ch->in_room )
		return;

	room = ch->in_room;

	sk = get_skill( ch, skill_lookup( "mining" ) );

	if(!mine_check(ch, sk))
		return;

	//TODO mine_before_trigger

	if ( ch->move < ( ch->max_move / 8 ) )
	{
		print_char( ch, "Zmêczenie uniemo¿liwia ci rozpoczêcie wydobycia.\n\r");
		return;
	}

	//wcze¶niej == dzia³a³o, bo lecia³a zazwyczaj jedna bry³ka
	//po podkrêceniu na 2-3 nale¿a³o poprawiæ na <=
	if (room->rawmaterial_capacity_now <= 0){
		print_char( ch, "To z³o¿e wygl±da na wyczerpane.\n\r");
		return;
	}
	//dopuszczamy u¿ycie broni "kilof" jako narzêdzie ale z najgorsz± jako¶ci±
	tool = get_eq_char( ch, WEAR_WIELD );
	if ( tool
			&& tool->item_type == ITEM_WEAPON
			&& ( tool->value[0] == WEAPON_MACE || tool->value[0] == WEAPON_AXE )
			&& is_name( "kilof", tool->name ) )
	{
		qu = 0;
	}
	else
	{
		//sprawdzanie narzêdzia
		if ( get_hand_slots( ch, WEAR_HOLD ) > 0 )	/*znaczy ze ma cos jeszcze w ³apce */
		{
			send_to_char( "Do tego potrzebujesz jednej rêki wolnej i odpowiedniego narzêdzia.\n\r", ch );
			return;
		}

		//sprawdzamy je¶li to nie kilof
		tool = get_eq_char( ch, WEAR_HOLD );
		if ( !tool || tool->item_type != ITEM_TOOL || tool->value[2] != TOOL_PICKAXE )
		{
			send_to_char("Przyda³by siê do tego jaki¶ kilof.\n\r", ch );
			return;
		}

		qu = tool->value[0];
	}

	if ( tool->condition <= 0 )
	{
		destroy_obj( ch, tool );
		return;
	}

	//dla kilofów o wysokiej jako¶ci jest wiêksza szansa na wypadniêcie bry³ki
	if ( number_percent() < ( qu * 4 ) ) ile_wypadnie += 1;
	if ( number_percent() < ( qu * 2 ) ) ile_wypadnie += number_range(1, 2);

	//Minig trwa od 3 do 20 okresów
	//Przy wyszkoleniu na ¶rednio prawie za ka¿dym razem powinno co¶ wypa¶æ (z ni¿szej pó³ki)
	//Dalsze szkolenie ma na celu podnoszenie jako¶ci wydobywanych przedmiotów oraz dostêp do gemów

	//Czas bazowy - 20 okresów, zmniejszany o (równowa¿nie):
	//2009-06-24 - zmiana czasu na 15, ale za to mniej bry³ek.
	// - wspó³czynnik zale¿ny od jako¶ci kilofa: qu (0-9)
	// - stopieñ wytrenowania skilla: sk (0-100)
	time = 15;
	time -= number_range( 1, qu ); //qu = 0 -> -1, qu = 9 -> -9
	time -= (sk + 5) / 13; //sk=0 -> -0, sk=100 -> -8
	time = URANGE( 4, time, 20 ); //poczatkujacy = 20, arcymistrz = 4

	for ( i = time - 2; i > 1; i-=2 )
	{
		//losowanie komunikatów
		if ( number_percent() > sk )
		{
			wait_printf( ch, time - i, "%s\n\r", messages1[number_range(0, mes1)] );
		} else {
			wait_printf( ch, time - i, "%s\n\r", messages2[number_range(0, mes2)] );
			ile_poz_komunikatow++;
		}
		wait_act( time - i, "$n z mozo³em uderza kilofem w ¶cianê.", ch, NULL, NULL, TO_ROOM );
	}

	print_char( ch, "Zamierzasz siê do pierwszego uderzenia.\n\r" );

	if ( !IS_OBJ_STAT( tool, ITEM_UNDESTRUCTABLE ) && number_percent() > material_table[ tool->material ].hardness )
		tool->condition -= number_range( 1, 2 );
	else
		tool->condition -= number_range( 0, 1 );

	for ( i = (MAX_RAWMATERIAL - 1); i >= 0 ; --i )
	{
		other = FALSE;
		//je¶li nie ma ju¿ kamieni, to wyj¶cie
		if ( room->rawmaterial[i] <= 0 )
			continue;

		//jesli nie ma kamieni o skilu wystarczaj±cym do wykopania to wyj¶cie
		if ( rawmaterial_table[i].skill > sk )
			continue;

		//szansa na "wydarzenia spokojne"
		if ( number_range( 1, 100 ) == 1 )
		{
			buf[0] = '\0';
			switch (number_range(1, 3))
			{
			case 1:
				//Podobizna gracza, ustawiona jako pomnik
				//object musi byc typu gem
				//OBJ_VNUM_MINE_IMAGE==6

				obj_index = get_obj_index( OBJ_VNUM_MINE_IMAGE );
				if(!obj_index)
					break;
				brylka = create_object(obj_index , FALSE );
				if ( !brylka )
					break;
				brylka->item_type = ITEM_TRASH;
				brylka->name = str_dup("podobizna pomnik");
				brylka->name2 = str_dup("podobizny");
				brylka->name3 = str_dup("podobi¼nie");
				brylka->name4 = str_dup("podobiznê");
				brylka->name5 = str_dup("podobizn±");
				brylka->name6 = str_dup("podobi¼nie");
				sprintf( buf, "Podobizna, chyba jaki¶ %s.", pc_race_table[ ch->race ].name );
				brylka->description = str_dup(buf);
				brylka->short_descr = str_dup(buf);
				sprintf( buf, "Stoi tutaj wykuta w skale podobizna %s.", ch->name2 );
				brylka->item_description = str_dup(buf);
				brylka->timer = 30;
				wait_obj_to_room( brylka, time, room );
				wait_printf( ch, time, "Z zaskoczeniem patrzysz na efekt swojej pracy - w³asn± podobiznê.\n\r");
				wait_act( time, "$n z zaskoczeniem spogl±da na efekt swojej pracy - w³asn± podobiznê.", ch, NULL, NULL, TO_ROOM );
				other = TRUE;
				break;
			case 2:
				//Naszyjnik
				//OBJ_VNUM_MINE_SUPERB_PIECE==43677
				obj_index = get_obj_index( OBJ_VNUM_MINE_SUPERB_PIECE );
				if(!obj_index)
					break;
				brylka = create_object(obj_index , FALSE );
				if ( !brylka )
					break;
				wait_obj_to_room( brylka, time, room);
				wait_printf( ch, time, "Patrzysz na to co wyku³e¶. A to sk±d siê tutaj wziê³o?\n\r");
				wait_act( time, "$n patrz±c na efekt swojej pracy, zastanawia siê sk±d siê to tutaj mog³o wzi±æ.", ch, NULL, NULL, TO_ROOM );
				other = TRUE;
				break;
			case 3:
				//Robaczek
				//MOB_VNUM_ZUK = 3003
				wait_mob_to_room_by_vnum(MOB_VNUM_ZUK, time, room);
				wait_printf( ch, time, "Uderzasz w ¶cianê i z powsta³ego pêkniêcia co¶ wype³za.\n\r");
				wait_act( time, "$n uderza w ¶cianê i patrzy jak co¶ wype³za z powsta³ego pêkniêcia.", ch, NULL, NULL, TO_ROOM );
				other = TRUE;
				break;
			default:
				break;
			}
			break;
		}


		//losowanie szansy na wypadniêcie kamyczków
		//i - numer kolejny rodzaju materia³u
		//2009-04-15 Rysand: warunki by³y zbyt restrykcyjne - zmieni³em tak, ¿eby czê¶ciej wylatywa³y bry³ki
		//wszystko ustawiane przez builderów, poprzednia wersja wprowadza³a sztuczne ograniczenia
		//room->rawmaterial[i]) = resources - prawdopodobieñstwo wypadniêcia
		//
		if(number_percent() < room->rawmaterial[i])
		{
			for ( j = 1; j <= ile_wypadnie; ++j )
			{
				if ( rawmaterial_table[i].vnum > 0 )
				{
					brylka = generate_brylka(qu, i, room);
					wait_obj_to_room( brylka, time, room );
				} else {
					switch( rawmaterial_table[i].bit )
					{
					case RAW_GEMS:
					brylka = create_object( get_obj_index( 6 ), FALSE );
					brylka->wear_flags = ITEM_TAKE | ITEM_HOLD;
					brylka->item_type = ITEM_GEM;
					if ( number_range( 1, 50 ) == 1 )
					{
							points = 6;
					} else {
							points = number_range( 1, 5 );
					}

					generate_gem( brylka, &points );
					brylka->cost *= RATTING_SILVER;//Raszer, bez tego ceny wydobytych kamieni sa 60x mniejsze od normalnie dropionych
					wait_obj_to_room( brylka, time, room );
					--room->rawmaterial_capacity_now;
						break;
					}
				}
					--ile_wypadnie;
			}
			if ( ile_wypadnie <= 0 ) break;
		}
    }

    /**
     * ladowanie potworkow ze skal
     */
    if(!other && ( number_range( 1, 500 ) == 1 ))
    {
        //Rysand:
        //czasem zdarza siê, ¿e dokopiemy siê nie tam gdzie trzeba.
        //Pomys³ z Drizda i wizyty w kopalni
        vEventAggresiv = TRUE;
        if( ch->level > 29 && number_range( 0, 50 ) < 1 )
        {
            //MOB_VNUM_MINE_DRAKOLICZ 4001
            //naprawdê ma³a szansa, dla wysokich leveli - niech maj± uciechê
            wait_mob_to_room_by_vnum(MOB_VNUM_MINE_DRAKOLICZ, time, room);
            //³adowanie na pocz±tku mining
            wait_printf( ch, time, "Uderzasz w ¶cianê i z powsta³ego pêkniêcia co¶ wype³za!\n\r");
            wait_act( time, "$n uderza w ¶cianê i patrzy jak co¶ wype³za z powsta³ego pêkniêcia!", ch, NULL, NULL, TO_ROOM );
        }
        else if(ch->level > 25 && number_range( 0, 15) < 1 )
        {
            //MOB_VNUM_MINE_LARGE_WORM 4002
            //hakowa poczwara
            //3079
            wait_mob_to_room_by_vnum(MOB_VNUM_MINE_LARGE_WORM, time, room);
            wait_printf( ch, time, "Uderzasz w ¶cianê i z powsta³ego pêkniêcia co¶ wychodzi!\n\r");
            wait_act( time, "$n uderza w ¶cianê i patrzy jak co¶ wype³za z powsta³ego pêkniêcia!", ch, NULL, NULL, TO_ROOM );
        }
        else if( ch->level > 20 && number_range( 0, 10 ) < 1 )
        {
            //MOB_VNUM_MINE_GOBLIN 4003
            //3080
            //1-3 agresywne gobliny, tekst o wielkich skarbach i ich obronie
            CHAR_DATA * goblin;
            goblin = wait_mob_to_room_by_vnum(MOB_VNUM_MINE_GOBLIN, time, room);
            /**
             * 66% na dodatkowego goblina
             */
            if ( number_percent() < 67 )
            {
                goblin = wait_mob_to_room_by_vnum(MOB_VNUM_MINE_GOBLIN, time, room);
            }
            /**
             * oraz jeszcze jeden z szansa 33%
             */
            if ( number_percent() < 34 )
            {
                goblin = wait_mob_to_room_by_vnum(MOB_VNUM_MINE_GOBLIN, time, room);
            }
            wait_printf( ch, time, "Uderzasz w ¶cianê i z powsta³ego pêkniêcia kto¶ wyskakuje!\n\r");
            wait_act( time, "$n wykrzykuje co¶ o naruszaniu terytorium!", goblin, NULL, NULL, TO_ROOM );
        }
        else if( ch->level > 15  && number_range( 0, 8 ) < 1 )
        {
            ////MOB_VNUM_MINE_GOBLIN 4003
            //1 goblin, tekst o zak³ócaniu spokoju
            CHAR_DATA *goblin = wait_mob_to_room_by_vnum( MOB_VNUM_MINE_SMALL_WORM, time, room);
            wait_printf( ch, time, "Uderzasz w ¶cianê i z powsta³ego pêkniêcia kto¶ wyskakuje!\n\r");
            wait_act( time, "$n wykrzykuje co¶ o naruszaniu terytorium!", goblin, NULL, NULL, TO_ROOM );
        }
        else if(ch->level > 10 && number_range( 0, 6 ) < 1 )
        {
            //MOB_VNUM_MINE_SMALL_WORM 4004
            //agresywny robak, na 2-3 uderzenia lekko rani
            //3081
            wait_mob_to_room_by_vnum(MOB_VNUM_MINE_SMALL_WORM, time, room);
            wait_printf( ch, time, "Uderzasz w ¶cianê i z powsta³ego pêkniêcia co¶ wype³za!\n\r");
        }
        else
        {
            //nic nie ³adujemy, niech sie mlodzie¿ szkoli, o ile cokolwiek s³yszy
            if ( !IS_AFFECTED( ch, AFF_DEAFNESS ) || IS_AFFECTED(ch,AFF_PERFECT_SENSES) )
            {
                wait_printf( ch, time, "%s\n\r", messages_newbie[number_range(0, mes_newbie)] );
            }
            vEventAggresiv = FALSE;
        }
    }

	if ( !other && !vEventAggresiv)
	{
		wait_printf( ch, time, "%s\n\r", messages3[number_range(0, mes3)] );
		wait_act( time, "$n koñcz±cym uderzeniem od³upuje kawa³ ze ¶ciany.", ch, NULL, NULL, TO_ROOM );
	}

	WAIT_STATE( ch, time * 4 + 10);

	//zmniejszanie mv przy kopaniu
	//nie powinno byæ za du¿o, ale tak, ¿eby nie da³o siê kopac bez przerwy
	ch->move -=
		URANGE(
				3,
				//czê¶æ sta³a, "wyrównanie" szans ró¿nych ras
				(ch->max_move / 8)
				//Postacie maj±ce lepsz± kondycjê mog± wiêcej
				- URANGE(1,  get_curr_stat_deprecated(ch,STAT_CON) / 4,  5)
				//element losowy, antytrigger
				+ number_range(2, 4)
				,12
		);

	//Z za³o¿enia to bêdzie skill do otriggerowania
	//Plusy nie powinny za czêsto lecieæ
	//Oryginalnie jest 160, mo¿na zmniejszyæ do C_BASE_TIME * 2, ale mniej to ju¿ przesada
	//szansa przy 40 okresach by³a 4
	check_improve( ch, NULL, skill_lookup( "mining" ), TRUE, 160/C_BASE_TIME ); //czas kopania: 40 -> 4, 20 -> 8


	//TODO mine_after_trigger
	return;
}

//Drake: Breathy jako skille, ze spora pomoca Kaintiego. OLBRZYMIA pomoc±.
void do_fire_breath( CHAR_DATA *ch, char *argument)
{
   CHAR_DATA * victim;
	CHAR_DATA *tmp_vict;
	bool found;
	int dam, skill = 100, size_modifier, tmp_dam;

	save_debug_info("skills.c => do_fire_breath", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

	if ( ( skill = get_skill( ch, gsn_fire_breath ) ) <= 0 )
	{
		send_to_char( "Huh?\n\r", ch );
		return;
	}

	if ( argument[ 0 ] == '\0' )
	{
		victim = ch->fighting;

		if ( victim == NULL )
		{
			send_to_char( "Kogo chcesz poraziæ p³omiennym oddechem?\n\r", ch );
			return;
		}
	}
	else if ( ( victim = get_char_room( ch, argument ) ) == NULL )
	{
		send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
		return;
	}

	if ( ch == victim )
	{
		send_to_char( "Nie dasz rady tego zrobi&#263;.\n\r", ch );
		return;
	}

	if ( is_safe( ch, victim ) )
		return;


   //Obrazenia od oddechu i szansa na objecie wiekszej ilosci wrogow wrogow.
	switch ( ch->size )
	{
	case SIZE_TINY: dam = dice( 2, 6 ); size_modifier = 0; break;
	case SIZE_SMALL: dam = dice( 3, 6 ); size_modifier = 0; 	break;
	case SIZE_MEDIUM: dam = dice( 4, 6 ); size_modifier = 1;  break;
	case SIZE_LARGE: dam = 20 + dice( 5, 6 ); size_modifier = 1; break;
	case SIZE_HUGE: dam = 30 + dice( 6, 6 ); size_modifier = 2; break;
	case SIZE_GIANT: dam = 50 + dice( 10, 6 ); size_modifier = 3; break;
	default: dam = dice( 3, 3 ); size_modifier = 0; break;
	}

	//bonus od bycia bossem
			if ( EXT_IS_SET( ch->act, ACT_BOSS ) )
		{
      dam *= 2;
      size_modifier += 1;
		}

   //Bonus od levela
     dam = (dam*(100+number_range(0,UMAX(1,ch->level-20)*2)))/100;

   //Obrazenia w glowny cel
	act( "{5Ziejesz ogniem w kierunku $Z, twój cel próbuje w panicznie zej¶æ z toru ognistej chmury jednak nie udaje mu mu siê to!{x", ch, NULL, victim, TO_CHAR );
	act( "{5$n zieje ogniem w twoim kierunku, próbujesz unikn±æ ognistej fali jednak na pró¿no, ogieñ ogarnia twoje cia³o!{x", ch, NULL, victim, TO_VICT );
	act( "{5$n zieje ogniem, $Z próbuje unikn±æ ognistej fali jednak na pró¼no i znika w ognistym podmuchu!{x", ch, NULL, victim, TO_NOTVICT );
   damage( ch, victim, dam, gsn_fire_breath, DAM_FIRE, FALSE );
   WAIT_STATE( ch, skill_table[ gsn_fire_breath ].beats );

	found = FALSE;

		for ( tmp_vict = ch->in_room->people;
            tmp_vict != NULL && size_modifier > 0;
            tmp_vict = tmp_vict->next_in_room )
		{
			if ( tmp_vict != ch && tmp_vict != victim )
				{
				found = TRUE;
	    		}
    			else continue;
                if ( is_same_group( ch, tmp_vict ) )
		              	continue;

                if (number_range(0, size_modifier*16)+ch->level-tmp_vict->level < number_range (0, UMAX(1, (get_curr_stat_deprecated( tmp_vict, STAT_DEX )-14)*2) )
                 + number_range (0, UMAX(1, (get_curr_stat_deprecated( tmp_vict, STAT_INT )-20)*2) ) + number_range (0, UMAX(1, (get_curr_stat_deprecated( tmp_vict, STAT_LUC )-12))))
		          {
		            act( "{5Ognista chmura mija $Z o centymetry.{x", ch, NULL, tmp_vict, TO_CHAR );
		            act( "{5Dos³ownie w ostatnim momencie udaje ci siê zrobiæ unik i ognista fala lekko ciê osmala.{x", ch, NULL, tmp_vict, TO_VICT );
		            act( "{5Ognista fala mija $X nie czyni±c ¿adnej szkody.{x", ch, NULL, tmp_vict, TO_NOTVICT );
		            damage( ch, tmp_vict, 0, gsn_fire_breath, DAM_FIRE, FALSE );
		          }
     		     else
		          {
		            act( "{5Ognista chmura ogarnia $Z.{x", ch, NULL, tmp_vict, TO_CHAR );
		            act( "{5Nie udaje ci siê zej¶æ z drogi nadlatuj±cego p³omienia.{x", ch, NULL, tmp_vict, TO_VICT );
		            act( "{5$Z próbuje unikn±æ ognistej fali jednak na pró¼no i znika w ognistym podmuchu!{x", ch, NULL, tmp_vict, TO_NOTVICT );
				      tmp_dam = (dam/5)*size_modifier;

				if ( number_range( 0, UMAX( 1, get_curr_stat_deprecated( tmp_vict, STAT_DEX) - 14 ) ) + number_range( 0, UMAX( 1, get_curr_stat_deprecated( tmp_vict, STAT_LUC ) - 12 )/3 ) > 6 )
				{
					tmp_dam -= tmp_dam/4;
		         act( "{5W ostatniej chwili udaje ci siê lekko odskoczyæ, unikaj±c centrum gor±cej fali.{x", ch, NULL, tmp_vict, TO_VICT );
				}

            if ( saves_spell_new( victim, skill_table[ gsn_fire_breath ].save_type, skill_table[ gsn_fire_breath ].save_mod, ch, gsn_fire_breath ) )
               tmp_dam -= tmp_dam / 4;


			         damage( ch, tmp_vict, tmp_dam, gsn_fire_breath, DAM_FIRE, FALSE );
		          }


	         size_modifier -= 1;
		}

   return;
 }
//Zmieniona kopia Fire Breatha
 void do_frost_breath( CHAR_DATA *ch, char *argument)
{
   CHAR_DATA * victim;
	CHAR_DATA *tmp_vict;
	bool found;
	int dam, skill = 100, size_modifier, tmp_dam;

	save_debug_info("skills.c => do_frost_breath", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

	if ( ( skill = get_skill( ch, gsn_frost_breath ) ) <= 0 )
	{
		send_to_char( "Huh?\n\r", ch );
		return;
	}

	if ( argument[ 0 ] == '\0' )
	{
		victim = ch->fighting;

		if ( victim == NULL )
		{
			send_to_char( "Kogo chcesz poraziæ lodowym oddechem?\n\r", ch );
			return;
		}
	}
	else if ( ( victim = get_char_room( ch, argument ) ) == NULL )
	{
		send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
		return;
	}

	if ( ch == victim )
	{
		send_to_char( "Nie dasz rady tego zrobi&#263;.\n\r", ch );
		return;
	}

	if ( is_safe( ch, victim ) )
		return;


   //Obrazenia od oddechu i szansa na objecie wiekszej ilosci wrogow wrogow.
	switch ( ch->size )
	{
	case SIZE_TINY: dam = dice( 2, 6 ); size_modifier = 0; break;
	case SIZE_SMALL: dam = dice( 3, 6 ); size_modifier = 0; 	break;
	case SIZE_MEDIUM: dam = dice( 4, 6 ); size_modifier = 1;  break;
	case SIZE_LARGE: dam = 20 + dice( 5, 6 ); size_modifier = 1; break;
	case SIZE_HUGE: dam = 30 + dice( 6, 6 ); size_modifier = 2; break;
	case SIZE_GIANT: dam = 50 + dice( 10, 6 ); size_modifier = 3; break;
	default: dam = dice( 3, 3 ); size_modifier = 0; break;
	}

	//bonus od bycia bossem
			if ( EXT_IS_SET( ch->act, ACT_BOSS ) )
		{
      dam *= 2;
      size_modifier += 1;
		}

   //Bonus od levela
     dam = (dam*(100+number_range(0,UMAX(1,ch->level-20)*2)))/100;

   //Obrazenia w glowny cel
	act( "{5Ziejesz lodem w kierunku $Z, twój cel próbuje w panicznie zej¶æ z toru nadchodz±cej zamieci jednak nie udaje mu mu siê to!{x", ch, NULL, victim, TO_CHAR );
	act( "{5$n zieje lodem w twoim kierunku, próbujesz unikn±æ nadchodz±cej zamieci jednak na pró¿no, lód ogarnia twoje cia³o!{x", ch, NULL, victim, TO_VICT );
	act( "{5$n zieje lodem, $Z próbuje unikn±æ nadchodz±cej zamieci jednak na pró¼no i znika w lodowym podmuchu!{x", ch, NULL, victim, TO_NOTVICT );
   damage( ch, victim, dam, gsn_frost_breath, DAM_COLD, FALSE );
   WAIT_STATE( ch, skill_table[ gsn_frost_breath ].beats );

	found = FALSE;

		for ( tmp_vict = ch->in_room->people;
            tmp_vict != NULL && size_modifier > 0;
            tmp_vict = tmp_vict->next_in_room )
		{
			if ( tmp_vict != ch && tmp_vict != victim )
				{
				found = TRUE;
	    		}
    			else continue;
                if ( is_same_group( ch, tmp_vict ) )
		              	continue;

                if (number_range(0, size_modifier*16)+ch->level-tmp_vict->level < number_range (0, UMAX(1, (get_curr_stat_deprecated( tmp_vict, STAT_DEX )-14)*2) )
                 + number_range (0, UMAX(1, (get_curr_stat_deprecated( tmp_vict, STAT_INT )-20)*2) ) + number_range (0, UMAX(1, (get_curr_stat_deprecated( tmp_vict, STAT_LUC )-12))))
		          {
		            act( "{5Lodowa chmura mija $Z o centymetry.{x", ch, NULL, tmp_vict, TO_CHAR );
		            act( "{5Dos³ownie w ostatnim momencie udaje ci siê zrobiæ unik i lodowa zamieæ przelatuje obok ciebie.{x", ch, NULL, tmp_vict, TO_VICT );
		            act( "{5Lodowa zamieæ mija $X nie czyni±c ¿adnej szkody.{x", ch, NULL, tmp_vict, TO_NOTVICT );
		            damage( ch, tmp_vict, 0, gsn_frost_breath, DAM_COLD, FALSE );
		          }
     		     else
		          {
		            act( "{5Lodowa zamieæ ogarnia $Z.{x", ch, NULL, tmp_vict, TO_CHAR );
		            act( "{5Nie udaje ci siê zej¶æ z drogi nadlatuj±cego lodu.{x", ch, NULL, tmp_vict, TO_VICT );
		            act( "{5$Z próbuje unikn±æ nadchodz±cej zamieci jednak na pró¼no i znika w lodowym podmuchu!{x", ch, NULL, tmp_vict, TO_NOTVICT );
				      tmp_dam = (dam/5)*size_modifier;

				if ( number_range( 0, UMAX( 1, get_curr_stat_deprecated( tmp_vict, STAT_DEX) - 14 ) ) + number_range( 0, UMAX( 1, get_curr_stat_deprecated( tmp_vict, STAT_LUC ) - 12 )/3 ) > 6 )
				{
					tmp_dam -= tmp_dam/4;
		         act( "{5W ostatniej chwili udaje ci siê lekko odskoczyæ, unikaj±c centrum lodowej nawa³nicy.{x", ch, NULL, tmp_vict, TO_VICT );
				}

            if ( saves_spell_new( victim, skill_table[ gsn_frost_breath ].save_type, skill_table[ gsn_frost_breath ].save_mod, ch, gsn_frost_breath ) )
               tmp_dam -= tmp_dam / 4;


			         damage( ch, tmp_vict, tmp_dam, gsn_frost_breath, DAM_COLD, FALSE );
		          }


	         size_modifier -= 1;
		}

   return;
 }

 void do_acid_breath( CHAR_DATA *ch, char *argument)
{
   CHAR_DATA * victim;
	CHAR_DATA *tmp_vict;
	bool found;
	int dam, skill = 100, size_modifier, tmp_dam;

	save_debug_info("skills.c => do_acid_breath", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

	if ( ( skill = get_skill( ch, gsn_acid_breath ) ) <= 0 )
	{
		send_to_char( "Huh?\n\r", ch );
		return;
	}

	if ( argument[ 0 ] == '\0' )
	{
		victim = ch->fighting;

		if ( victim == NULL )
		{
			send_to_char( "Kogo chcesz poraziæ kwasowym oddechem?\n\r", ch );
			return;
		}
	}
	else if ( ( victim = get_char_room( ch, argument ) ) == NULL )
	{
		send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
		return;
	}

	if ( ch == victim )
	{
		send_to_char( "Nie dasz rady tego zrobi&#263;.\n\r", ch );
		return;
	}

	if ( is_safe( ch, victim ) )
		return;


   //Obrazenia od oddechu i szansa na objecie wiekszej ilosci wrogow wrogow.
	switch ( ch->size )
	{
	case SIZE_TINY: dam = dice( 2, 6 ); size_modifier = 0; break;
	case SIZE_SMALL: dam = dice( 3, 6 ); size_modifier = 0; 	break;
	case SIZE_MEDIUM: dam = dice( 4, 6 ); size_modifier = 1;  break;
	case SIZE_LARGE: dam = 20 + dice( 5, 6 ); size_modifier = 1; break;
	case SIZE_HUGE: dam = 30 + dice( 6, 6 ); size_modifier = 2; break;
	case SIZE_GIANT: dam = 50 + dice( 10, 6 ); size_modifier = 3; break;
	default: dam = dice( 3, 3 ); size_modifier = 0; break;
	}

	//bonus od bycia bossem
			if ( EXT_IS_SET( ch->act, ACT_BOSS ) )
		{
      dam *= 2;
      size_modifier += 1;
		}

   //Bonus od levela
     dam = (dam*(100+number_range(0,UMAX(1,ch->level-20)*2)))/100;

   //Obrazenia w glowny cel
	act( "{5Ziejesz kwasem w kierunku $Z, twój cel próbuje w panicznie zej¶æ z toru kwasowej chmury, jednak nie udaje mu mu siê to!{x", ch, NULL, victim, TO_CHAR );
	act( "{5$n zieje kwasem w twoim kierunku, próbujesz unikn±æ niszczycielskiej fali, jednak na pró¿no, kwas wypala twoje cia³o!{x", ch, NULL, victim, TO_VICT );
	act( "{5$n zieje kwasem, $N próbuje unikn±æ niszczycielskiej fali, jednak na pró¿no i znika w zielonkawej chmurze!{x", ch, NULL, victim, TO_NOTVICT );
   damage( ch, victim, dam, gsn_acid_breath, DAM_ACID, FALSE );
   WAIT_STATE( ch, skill_table[ gsn_acid_breath ].beats );

	found = FALSE;

		for ( tmp_vict = ch->in_room->people;
            tmp_vict != NULL && size_modifier > 0;
            tmp_vict = tmp_vict->next_in_room )
		{
			if ( tmp_vict != ch && tmp_vict != victim )
				{
				found = TRUE;
	    		}
    			else continue;
                if ( is_same_group( ch, tmp_vict ) )
		              	continue;

                if (number_range(0, size_modifier*16)+ch->level-tmp_vict->level < number_range (0, UMAX(1, (get_curr_stat_deprecated( tmp_vict, STAT_DEX )-14)*2) )
                 + number_range (0, UMAX(1, (get_curr_stat_deprecated( tmp_vict, STAT_INT )-20)*2) ) + number_range (0, UMAX(1, (get_curr_stat_deprecated( tmp_vict, STAT_LUC )-12))))
		          {
		            act( "{5Chmura kwasu mija $Z o centymetry.{x", ch, NULL, tmp_vict, TO_CHAR );
		            act( "{5Dos³ownie w ostatnim momencie udaje ci siê zrobiæ unik i wydostaæ siê poza zasiêg kwasowej chmury.{x", ch, NULL, tmp_vict, TO_VICT );
		            act( "{5Niszczycielska fala mija $X nie czyni±c ¿adnej szkody.{x", ch, NULL, tmp_vict, TO_NOTVICT );
		            damage( ch, tmp_vict, 0, gsn_acid_breath, DAM_ACID, FALSE );
		          }
     		     else
		          {
		            act( "{5Chmura kwasu ogarnia $Z.{x", ch, NULL, tmp_vict, TO_CHAR );
		            act( "{5Nie udaje ci siê zej¶æ z drogi nadlatuj±cego kwasu.{x", ch, NULL, tmp_vict, TO_VICT );
		            act( "{5$Z próbuje unikn±æ niszczycielskiej fali jednak na pró¼no i znika w chmurze kwasu!{x", ch, NULL, tmp_vict, TO_NOTVICT );
				      tmp_dam = (dam/5)*size_modifier;

				if ( number_range( 0, UMAX( 1, get_curr_stat_deprecated( tmp_vict, STAT_DEX) - 14 ) ) + number_range( 0, UMAX( 1, get_curr_stat_deprecated( tmp_vict, STAT_LUC ) - 12 )/3 ) > 6 )
				{
					tmp_dam -= tmp_dam/4;
		         act( "{5W ostatniej chwili udaje ci siê lekko odskoczyæ, unikaj±c centrum niszczycielskiej fali.{x", ch, NULL, tmp_vict, TO_VICT );
				}

            if ( saves_spell_new( victim, skill_table[ gsn_acid_breath ].save_type, skill_table[ gsn_acid_breath ].save_mod, ch, gsn_acid_breath ) )
               tmp_dam -= tmp_dam / 4;


			         damage( ch, tmp_vict, tmp_dam, gsn_acid_breath, DAM_ACID, FALSE );
		          }


	         size_modifier -= 1;
		}

   return;
 }

void do_lightning_breath( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA * victim;
    CHAR_DATA *tmp_vict;
    bool found;
    int dam, skill = 100, size_modifier, tmp_dam;

    save_debug_info("skills.c => do_lightning_breath", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

    if ( ( skill = get_skill( ch, gsn_lightning_breath ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' )
    {
        victim = ch->fighting;

        if ( victim == NULL )
        {
            send_to_char( "Kogo chcesz poraziæ elektrycznym oddechem?\n\r", ch );
            return;
        }
    }
    else if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        return;
    }

    if ( ch == victim )
    {
        send_to_char( "Nie dasz rady tego zrobi&#263;.\n\r", ch );
        return;
    }

    if ( is_safe( ch, victim ) )
        return;


    //Obrazenia od oddechu i szansa na objecie wiekszej ilosci wrogow wrogow.
    switch ( ch->size )
    {
        case SIZE_TINY: dam = dice( 2, 6 ); size_modifier = 0; break;
        case SIZE_SMALL: dam = dice( 3, 6 ); size_modifier = 0; 	break;
        case SIZE_MEDIUM: dam = dice( 4, 6 ); size_modifier = 1;  break;
        case SIZE_LARGE: dam = 20 + dice( 5, 6 ); size_modifier = 1; break;
        case SIZE_HUGE: dam = 30 + dice( 6, 6 ); size_modifier = 2; break;
        case SIZE_GIANT: dam = 50 + dice( 10, 6 ); size_modifier = 3; break;
        default: dam = dice( 3, 3 ); size_modifier = 0; break;
    }

    //bonus od bycia bossem
    if ( EXT_IS_SET( ch->act, ACT_BOSS ) )
    {
        dam *= 2;
        size_modifier += 1;
    }

    //Bonus od levela
    dam = (dam*(100+number_range(0,UMAX(1,ch->level-20)*2)))/100;

    //Obrazenia w glowny cel
    act( "{5Ziejesz piorunami w kierunku $Z, twój cel próbuje w panicznie zej¶æ z toru nadlatuj±cych pocisków jednak nie udaje mu mu siê to!{x", ch, NULL, victim, TO_CHAR );
    act( "{5$n zieje piorunami w twoim kierunku, próbujesz unikn±æ nadlatuj±cych pocisków jednak na pró¿no, potê¿na energia trafia w twoje cia³o!{x", ch, NULL, victim, TO_VICT );
    act( "{5$n zieje piorunami, $Z próbuje unikn±æ ognistej fali jednak na pró¼no i znika w ognistym podmuchu!{x", ch, NULL, victim, TO_NOTVICT );
    damage( ch, victim, dam, gsn_lightning_breath, DAM_LIGHTNING, FALSE );
    WAIT_STATE( ch, skill_table[ gsn_lightning_breath ].beats );

    found = FALSE;

    for ( tmp_vict = ch->in_room->people;
            tmp_vict != NULL && size_modifier > 0;
            tmp_vict = tmp_vict->next_in_room )
    {
        if ( tmp_vict != ch && tmp_vict != victim )
        {
            found = TRUE;
        }
        else continue;
        if ( is_same_group( ch, tmp_vict ) )
            continue;

        if (number_range(0, size_modifier*16)+ch->level-tmp_vict->level < number_range (0, UMAX(1, (get_curr_stat_deprecated( tmp_vict, STAT_DEX )-14)*2) )
                + number_range (0, UMAX(1, (get_curr_stat_deprecated( tmp_vict, STAT_INT )-20)*2) ) + number_range (0, UMAX(1, (get_curr_stat_deprecated( tmp_vict, STAT_LUC )-12))))
        {
            act( "{5Elektryczna nawa³nica mija $Z o centymetry.{x", ch, NULL, tmp_vict, TO_CHAR );
            act( "{5Dos³ownie w ostatnim momencie udaje ci siê zrobiæ unik i elektryczna nawa³nica.{x", ch, NULL, tmp_vict, TO_VICT );
            act( "{5Elektryczna nawa³nica mija $X nie czyni±c ¿adnej szkody.{x", ch, NULL, tmp_vict, TO_NOTVICT );
            damage( ch, tmp_vict, 0, gsn_lightning_breath, DAM_LIGHTNING, FALSE );
        }
        else
        {
            act( "{5Elektryczna nawa³nica ogarnia $Z.{x", ch, NULL, tmp_vict, TO_CHAR );
            act( "{5Nie udaje ci siê zej¶æ z drogi nadlatuj±cych piorunów.{x", ch, NULL, tmp_vict, TO_VICT );
            act( "{5$Z próbuje unikn±æ nadlatuj±cych piorunów jednak na pró¼no i znika w¶ród elektrycznych wy³adowañ!{x", ch, NULL, tmp_vict, TO_NOTVICT );
            tmp_dam = (dam/5)*size_modifier;

            if ( number_range( 0, UMAX( 1, get_curr_stat_deprecated( tmp_vict, STAT_DEX) - 14 ) ) + number_range( 0, UMAX( 1, get_curr_stat_deprecated( tmp_vict, STAT_LUC ) - 12 )/3 ) > 6 )
            {
                tmp_dam -= tmp_dam/4;
                act( "{5W ostatniej chwili udaje ci siê lekko odskoczyæ, unikaj±c centrum nawa³nicy.{x", ch, NULL, tmp_vict, TO_VICT );
            }

            if ( saves_spell_new( victim, skill_table[ gsn_lightning_breath ].save_type, skill_table[ gsn_lightning_breath ].save_mod, ch, gsn_lightning_breath ) )
                tmp_dam -= tmp_dam / 4;


            damage( ch, tmp_vict, tmp_dam, gsn_lightning_breath, DAM_LIGHTNING, FALSE );
        }


        size_modifier -= 1;
    }
}

void do_slam( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    CHAR_DATA *mount;
    int chance, chance_p;
    bool no_legs, slam_hit, slam_bash = FALSE, slam_selfbash;

    one_argument( argument, arg );

    if ( ( chance = get_skill( ch, gsn_slam ) ) <= 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }

    if ( ch->mount )
    {
        print_char( ch, "Jak chcesz to zrobiæ? Przecie¿ dosiadasz %s!\n\r", ch->mount->name2 );
        return;
    }

    if ( arg[ 0 ] == '\0' )
    {
        victim = ch->fighting;

        if ( victim == NULL )
        {
            send_to_char( "Rozgl±dasz siê w poszukiwaniu przeciwnika.\n\r", ch );
            return;
        }
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        WAIT_STATE( ch, PULSE_VIOLENCE );
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Obracasz siê w ko³o, próbuj±c siê uderzyæ.\n\r", ch );
        WAIT_STATE( ch, PULSE_VIOLENCE );
        return;
    }

    if ( is_safe( ch, victim ) )
    {
        return;
    }

    if ( !form_check( ch, victim, NULL, gsn_slam ) )
    {
        WAIT_STATE( ch, PULSE_VIOLENCE );
        return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
        act( "Przeciez $N jest twoim kumplem!", ch, NULL, victim, TO_CHAR );
        return;
    }

    /* Rasy bez nog i odporne na basha, sa tez odporne na slama */
    if (
            !str_cmp( race_table[ GET_RACE(victim) ].name, "krokodyl" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "krokodyl olbrzymi" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "meduza" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "owad pe³zaj±cy" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "ro¶lina" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "rój" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "water flow" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "w±¿" ) ||
            !str_cmp( race_table[ GET_RACE(victim) ].name, "¿yj±ca ¶ciana" )
       )
    {
        chance = 0;
        no_legs = TRUE;

    }else
    {
        no_legs = FALSE;
    }

    //Bazowa szansa na trafienie w przeciwnika
    chance = get_skill( ch, gsn_slam ) + 2*(get_curr_stat_deprecated(ch,STAT_DEX)-get_curr_stat_deprecated(victim,STAT_DEX));
    chance = URANGE(10,chance,95);
    //Bazowa szansa na przewrocenie przeciwnika
    chance_p = (get_skill( ch, gsn_slam ) )/4+ (get_curr_stat_deprecated(ch,STAT_STR)-get_curr_stat_deprecated(victim,STAT_STR)+get_curr_stat_deprecated(ch,STAT_CON)-get_curr_stat_deprecated(victim,STAT_CON));
    chance_p = URANGE(5,chance,35);
    print_char( ch, "Szansa bazowa trafienia: %d dex_twoj %d , dex przeciwnika %d \n\r", chance, get_curr_stat_deprecated(ch,STAT_DEX), get_curr_stat_deprecated(victim,STAT_DEX));
    print_char( ch, "Bazowa szansa na przewrocenie %d\n\r", chance_p );


    //modyfikatory od wielkosci
    switch ( victim->size - ch->size )
    {

        case -2:
            chance -= 25;
            chance_p = chance_p/3;
            break;
        case -1:
            chance -= 15;
            chance_p = chance_p/2;
            break;
        case  1:
            chance += 5;
            chance_p += 3;
            break;
        case  2:
            chance += 10;
            chance_p += 6;
            break;
    }
    //skrajne roznice wielkosci
    if((ch->size)+3<=victim->size)
    {chance+=25;
        chance_p+=9;
    }

    if((ch->size)-3>=victim->size)
    {
        if(IS_NPC(ch))
        {
            chance-=15; //zeby smok mogl w miare slamnac
            chance_p +=15;
        }else
        {
            chance-=75; //zeby gracz nie mogl slamnac malego bashnac
            chance_p +=15;
        };
    }
    print_char( ch, "Szansa trafienia po wielkosci: %d \n\r", chance );
    print_char( ch, "Szansa przewrocenia po wielkosci: %d \n\r", chance_p );
    chance = URANGE(10,chance,95);
    chance_p = URANGE(5, chance, 50);
    //dodatkowe modyfikatory

    //haste
    if(IS_AFFECTED( ch, AFF_HASTE ))chance+=7;
    if(IS_AFFECTED( victim, AFF_HASTE ))chance-=7;

    //blur
    if( is_affected( victim, skill_lookup("blur") )) chance-=10;
    if( is_affected( ch, skill_lookup("blur") )) chance+=5;

    //web
    if ( IS_AFFECTED( victim, AFF_WEB ) )chance+=20;
    if ( IS_AFFECTED( ch, AFF_WEB ) )chance-=25;

    //blind
    if ( IS_AFFECTED( victim, AFF_BLIND ) )chance+=15;
    if ( IS_AFFECTED( ch, AFF_BLIND ) )chance-=15;

    //slow
    if ( IS_AFFECTED( victim, AFF_SLOW ) )chance+=7;
    if ( IS_AFFECTED( ch, AFF_SLOW ) )chance-=7;

    //shield
    if (!can_see( ch, victim )) chance/=2;
    print_char( ch, "Szansa na trafienie po modyfikatorach: %d \n \r", chance );

    //slippery floor
    chance = URANGE(10,chance,95);
    //modyfikatory od pozycji
    if ( victim->position == POS_SITTING )
    {
        chance += chance/4;
        chance_p = 0;
        print_char( ch, "Szansa trafienia, przeciwnik siedzi: %d \n \r", chance );
    }

    if( victim->position == POS_RESTING)
    {
        chance += chance/4;
        chance_p = 0;
        print_char( ch, "Szansa trafienia, przeciwnik odpoczywa: %d \n \r", chance );
    }

    if( victim->position < POS_RESTING)
    {
        chance += chance/4;
        chance_p = 0;
    }

    if(no_legs==FALSE)
    {

        if(number_percent()<=URANGE( 5, chance, 95))
        {
            slam_hit=TRUE;
        }
        else
        {
            slam_hit=FALSE;
        }

    }else
    {
        slam_hit=FALSE;
    }
    //koniec liczenia trafienia

    //Obliczanie obrazen i przewrocenia
    if(slam_hit==FALSE)
    {
        slam_bash=FALSE;
    }
    else
    {
        //modyfikatory do przewrocenia od affectow

        //jesli mamy berka to latwiej lecimy na glebe i wrog tez berserk
        if( IS_AFFECTED( ch, AFF_BERSERK )) chance_p+=5;

        //liczenie szansy na przewalenie, jak nie przewalil jest szansa, ze sam barb sie przewali ;d

        if(number_percent()<=URANGE( 5, chance_p, 50))
        {
            slam_bash=TRUE;
        }
        else if(number_percent()<=(URANGE( 5, (100-chance_p - get_curr_stat_deprecated(ch, STAT_DEX)), 75 - get_curr_stat_deprecated(ch, STAT_DEX))))
        {
            slam_selfbash=TRUE;
        }
        else // jak nie dzieje siÄ™ nic ciekawego, to nie robimy teÅ¼ basha. zapobiega to uÅ¼yciu tej zmiennej jako niezainicjalizowanej w kodzie poniÅ¼ej.
        {
            slam_bash=FALSE;
        }
    }

    //wykonanie
    if((!can_move(victim) && chance>0)) slam_bash=TRUE;
    if(slam_hit==TRUE || (!can_move(victim) && chance>0))
    {
        //trafil
        //sprawdzenie mirrora
        if ( check_mirror_image( victim, ch ) )
        {
            act( "{5Próbujesz uderzyæ w $C ca³ym ciê¿arem swego cia³a, ale $E znika niespodziewanie.{x", ch, NULL, victim, TO_CHAR );
            act( "{5$n próbuje uderzyæ ciê ca³ym ciêzarem swojego cia³a, jednak trafia jedno z twoich lustrzanych odbiæ.{x", ch, NULL, victim, TO_VICT );
            act( "{5$n próbuje uderzyæ $C ca³ym ciê¿arem swego cia³a, ale trafia jedno z $S lustrzanych odbiæ.", ch, NULL, victim, TO_NOTVICT );
            damage( ch, victim, 0, gsn_slam, DAM_BASH, FALSE );
            check_improve( ch, victim, gsn_slam, FALSE, 30 );
            if ( number_percent() < ((100-chance_p)-get_curr_stat_deprecated(ch, STAT_DEX)) && ch->position > POS_SITTING )
            {
                ch->position = POS_SITTING;
                act( "{5Tracisz równowagê i l±dujesz na ziemi!{x", ch, NULL, victim, TO_CHAR );
            }
            if ( number_percent() > 30 ) WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            return;
        }
        //sprawdzenie blinka
        if ( check_blink( victim, ch, TRUE ) )
        {
            act( "{5Próbujesz uderzyæ w $C ca³ym ciê¿arem swojego cia³a, ale $S sylwetka chwilowo znika.{x", ch, NULL, victim, TO_CHAR );
            act( "{5$n próbuje uderzyæ ciê ca³ym ciê¿arem swojego cia³a, ale ty znikasz na chwilê z tego planu egzystencji.{x", ch, NULL, victim, TO_VICT );
            act( "{5$n próbuje uderzyæ $C ca³ym ciê¿arem swojego cia³a, ale $S sylwetka chwilowo znika.", ch, NULL, victim, TO_NOTVICT );
            damage( ch, victim, 0, gsn_slam, DAM_BASH, FALSE );
            check_improve( ch, victim, gsn_slam, FALSE, 35 );
            if ( number_percent() < (100-chance_p-get_curr_stat_deprecated(ch, STAT_DEX)) && ch->position > POS_SITTING )
            {
                ch->position = POS_SITTING;
                act( "{5Tracisz równowagê i l±dujesz na ziemi!{x", ch, NULL, victim, TO_CHAR );
            }
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            return;
        }
        //sprawdzenie stability
        if ( IS_AFFECTED(victim,AFF_STABILITY ) )
        {
            act( "{5$n uderza ciê napieraj±c ca³ym cia³em, jednak bez problemu utrzymujesz równowagê.{x", ch, NULL, victim, TO_VICT );
            act( "{5Uderzasz $C ca³ym ciê¿arem swojego cia³a, jednak $E bez problemu utrzymuje równowagê.{x", ch, NULL, victim, TO_CHAR );
            act( "{5$n uderza $C napieraj±c ca³ym cia³em, jednak $E bez problemu utrzymuje równowagê.{x", ch, NULL, victim, TO_NOTVICT );
            damage( ch, victim, (number_range(3,get_skill( ch, gsn_slam )/5)+(get_curr_stat_deprecated(ch, STAT_STR)-14)+(get_curr_stat_deprecated(ch, STAT_CON)-14)), gsn_slam, DAM_BASH, FALSE );
            check_improve( ch, victim, gsn_slam, TRUE, 30 );
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            return;
        }
        //sprawdzenie mounta
        if ( !IS_NPC(victim) && victim -> mount )
        {
            mount = victim -> mount;
            if ( IS_AFFECTED(mount,AFF_STABILITY ) )
            {
                act( "{5$n uderza twojego wierzchowca napieraj±c ca³ym cia³em, jednak bez problemu utrzymujesz równowagê.{x", ch, NULL, victim, TO_VICT );
                act( "{5Uderzasz w $C ca³ym ciê¿arem swojego cia³a, jednak $E bez problemu utrzymuje równowagê.{x", ch, NULL, mount, TO_CHAR );
                act( "{5$n uderza $C napieraj±c ca³ym cia³em, jednak $E bez problemu utrzymuje równowagê.{x", ch, NULL, victim, TO_NOTVICT );
                check_improve( ch, victim, gsn_slam, TRUE, 40 );
                WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
                return;
            }
            //jezdziec nie ma stabilki
            if ( IS_AFFECTED( victim, AFF_FLYING ) || IS_AFFECTED( victim, AFF_FLOAT ))
            {
                if ( skill_table[ gsn_fly ].msg_off )
                {
                    send_to_char( skill_table[ gsn_fly ].msg_off, victim );
                    send_to_char( "\n\r", victim );
                }
                affect_strip( victim, gsn_fly );
                affect_strip( victim, gsn_float );
            }
            strip_invis( victim, TRUE, TRUE );
            if ( IS_AFFECTED( victim, AFF_HIDE ) )
            {
                affect_strip( victim, gsn_hide );
            }

            act( "{5$n powala twojego wierzchowca na ziemiê potê¿nym uderzeniem ca³ego cia³a! Spaaaadasz.{x", ch, NULL, victim, TO_VICT );
            act( "{5Uderzasz z rozpêdu w $C, posy³aj±c $M na ziemiê!{x", ch, NULL, mount, TO_CHAR );
            act( "{5$n powala wierzchowca $C na ziemiê uderzaj±c ca³ym cia³em.{x", ch, NULL, victim, TO_NOTVICT );
            check_improve( ch, victim, gsn_slam, TRUE, 30 );
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            WAIT_STATE( victim, 3 * PULSE_VIOLENCE );
            // zmianiamy pozycjê je¼d¼ca
            if ( victim->position >= POS_RESTING )
            {
                victim->position = POS_SITTING;
            }
            // zdejmujemy je¿d¼ca z wierzchowca, a tego ostatniego pozbawiamy ridera
            mount->mounting=NULL;
            victim->mount = NULL;
            // no i smaczek, zaatakowany wierzchowiec niech siê odgryza, a co!
            do_kill( mount, ch->name );
            return;


        }

        if(slam_bash==TRUE)
        {
            if ( victim->position > POS_SITTING )
            {
                act( "{5$n powala ciê na ziemiê potê¿nym uderzeniem ca³ego cia³a!{x", ch, NULL, victim, TO_VICT );
                act( "{5Uderzasz z rozpêdu w $C, posy³aj±c $M na ziemiê!{x", ch, NULL, victim, TO_CHAR );
                act( "{5$n powala $C na ziemiê uderzaj±c ca³ym cia³em.{x", ch, NULL, victim, TO_NOTVICT );
                damage( ch, victim, (number_range(5,get_skill( ch, gsn_slam )/4)+(get_curr_stat_deprecated(ch, STAT_STR)-14)+(get_curr_stat_deprecated(ch, STAT_CON)-14)), gsn_slam, DAM_BASH, FALSE );
            }
            else
            {
                act( "{5$n rzuca siê i uderza ciê pote¿nie ³okciem w brzuch!{x", ch, NULL, victim, TO_VICT );
                act( "{5Rzucasz siê na $C, uderzaj±c swoj± ofiarê wyci±gniêtym ³okciem i ca³ym ciê¿arem swojego cia³a!{x", ch, NULL, victim, TO_CHAR );
                act( "{5$n rzuca siê na $C wyprowadzaj±c potêzne uderzenie ³okciem.{x", ch, NULL, victim, TO_NOTVICT );
                if ( ch->position >= POS_RESTING )
                {
                    ch->position = POS_SITTING;
                }
                damage( ch, victim, (number_range(10,get_skill( ch, gsn_slam )/2)+(get_curr_stat_deprecated(ch, STAT_STR)-14)+(get_curr_stat_deprecated(ch, STAT_CON)-14)), gsn_slam, DAM_BASH, FALSE );
            }
            if ( IS_AFFECTED( victim, AFF_FLYING ) || IS_AFFECTED( victim, AFF_FLOAT ))
            {
                if ( skill_table[ gsn_fly ].msg_off )
                {
                    send_to_char( skill_table[ gsn_fly ].msg_off, victim );
                    send_to_char( "\n\r", victim );
                }
                affect_strip( victim, gsn_fly );
                affect_strip( victim, gsn_float );
            }
            strip_invis( victim, TRUE, TRUE );
            if ( IS_AFFECTED( victim, AFF_HIDE ) )
            {
                affect_strip( victim, gsn_hide );
            }

            check_improve( ch, victim, gsn_slam, TRUE, 30 );
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            if ( victim->position != POS_STANDING )
            {
                victim->position = POS_STANDING; //zeby nie bylo -opis o powaleniu i wstanie z resta- Raszer
            }


            WAIT_STATE( victim, 3 * PULSE_VIOLENCE );
            if ( victim->position >= POS_RESTING )
            {
                victim->position = POS_SITTING;
            }
            return;

        }

        if(slam_selfbash==TRUE)
        {
            act( "{5$n uderza w ciebie, po czym traci równowagê i l±dujê na ziemi!{x", ch, NULL, victim, TO_VICT );
            act( "{5Uderzasz z rozpêdu w $C, jednak nagle tracisz rownowagê i l±dujesz na ziemi!{x", ch, NULL, victim, TO_CHAR );
            act( "{5$n uderza w $C, jednak nagle traci równowagê i l±duje na ziemi!{x", ch, NULL, victim, TO_NOTVICT );

            if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED( ch, AFF_FLOAT ))
            {
                if ( skill_table[ gsn_fly ].msg_off )
                {
                    send_to_char( skill_table[ gsn_fly ].msg_off, ch );
                    send_to_char( "\n\r", ch );
                }
                affect_strip( ch, gsn_fly );
                affect_strip( ch, gsn_float );
            }
            strip_invis( ch, TRUE, TRUE );

            check_improve( ch, victim, gsn_slam, TRUE, 30 );
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );
            if ( victim->position != POS_STANDING )
            {
                victim->position = POS_STANDING; //zeby nie bylo -opis o powaleniu i wstanie z resta- Raszer
            }
            damage( ch, victim, (number_range(3,get_skill( ch, gsn_slam )/5)+(get_curr_stat_deprecated(ch, STAT_STR)-14)+(get_curr_stat_deprecated(ch, STAT_CON)-14)), gsn_slam, DAM_BASH, FALSE );
            WAIT_STATE( ch, 3 * PULSE_VIOLENCE );
            if ( ch->position >= POS_RESTING )
            {
                ch->position = POS_SITTING;
            }
            return;
        }
        else
        {
            //Trafil, ale nie przewrocil.
            if(slam_hit==TRUE)
            {
                act( "{5$n uderza w ciebie ca³ym cia³em potê¿nie!{x", ch, NULL, victim, TO_VICT );
                act( "{5Uderzasz w $C ca³ym ciê¿arem swojego cia³a!{x", ch, NULL, victim, TO_CHAR );
                act( "{5$n uderza w $C ca³ym ciê¿arem swojego cia³a!{x", ch, NULL, victim, TO_NOTVICT );
                damage( ch, victim, (number_range(5,get_skill( ch, gsn_slam )/4)+(get_curr_stat_deprecated(ch, STAT_STR)-14)+(get_curr_stat_deprecated(ch, STAT_CON)-14)), gsn_slam, DAM_BASH, FALSE );
                check_improve( ch, victim, gsn_slam, FALSE, 30 );
                WAIT_STATE( ch, 3 * PULSE_VIOLENCE );
                return;
            }
            else
                act( "{5$n próbuje ciê uderzyæ ca³ym ciê¿arem swego cia³a, jednak odsuwasz siê w czas.{x", ch, NULL, victim, TO_VICT );
            act( "{5Próbuje uderzyæ w $C ca³ym ciê¿arem swojego cia³a, jednak mija siê z celem!{x", ch, NULL, victim, TO_CHAR );
            act( "{5$n próbuje uderzyæ w $C ca³ym ciê¿arem swojego cia³a, jednak mija siê z celem!{x", ch, NULL, victim, TO_NOTVICT );
            damage( ch, victim, 0, gsn_slam, DAM_BASH, FALSE );
            check_improve( ch, victim, gsn_slam, FALSE, 30 );
            WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

        }//koniec nie trafiania
        check_killer( ch, victim );
        return;
    }
}


