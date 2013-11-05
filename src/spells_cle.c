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
 * Zdziech Tomasz        (tzdziech@gmail.com            ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: spells_cle.c 11986 2013-01-23 13:13:07Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/spells_cle.c $
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
#include "tables.h"
/*
 *
 * CZARY KAP£AÑSKIE
 *
 */

extern char *	target_name;

bool check_dispel          args( ( int dis_level, CHAR_DATA *victim, int sn ) );
bool check_improve_strenth args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool verbose ) );
bool remove_obj            args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
bool saves_dispel          args( ( int dis_level, int spell_level, int duration ) );
void raw_kill              args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void set_fighting          args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

void heal_char( CHAR_DATA *ch, CHAR_DATA *victim, sh_int value, bool all )
{
	if ( ! ch || ! victim )
    {
		return;
    }
	sh_int heal_value;
	int _heal, luck = get_curr_stat( ch, STAT_LUC ), tmp, i;
	AFFECT_DATA *aff_bandage;

	int exp_gain = 0;

	if ( !all && ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) ) )
		return;

	if ( is_affected( ch, gsn_life_transfer ) )
	{
		send_to_char( "Na razie nie masz do¶æ si³ witalnych.\n\r", ch );
		return;
	}

	_heal = victim->hit;

	// modyfikator zale¿ny od szczê¶cia dodatni
	if ( number_range( 0, luck ) > 90 ) _heal = ( _heal * 11 ) / 10;
	// modyfikator zale¿ny od szczê¶cia ujemny
	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) < 3 ) _heal = ( _heal * 9 ) / 10;
	// modyfikator dla strasznego pecha
	if (( number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 ))
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( IS_NPC( ch ) )
    {
		value *= 4;
        value /= 3;
    }

	if ( victim->position != POS_FIGHTING && victim->fighting == NULL )
	{
		if ( is_affected ( victim, gsn_bandage ) )
		{
			aff_bandage = affect_find ( victim->affected, gsn_bandage );
			if ( aff_bandage->modifier > 0 )
			{
				value += ( value * aff_bandage->modifier ) / 100;
				send_to_char( "Czujesz lekkie szczypanie pod opatrunkiem, ktory najwyra¼niej zwiêkszy³ lecznicza moc czaru.\n\r", victim );
				send_to_char( "Czujesz, ¿e pob³ogos³awione banda¿e zwiêkszaj± lecznicza moc twojego czaru.\n\r", ch );
			}
		}
	}

	heal_value = UMIN( get_max_hp( victim ) + 11 - victim->hit, value );
	heal_value = 100 * heal_value / get_max_hp( victim );

	/* exp dla castera za leczenie. najpierw procentowo, potem dzielimy przez 100*/

	exp_gain = number_range( 90, 110 ) * IS_NPC( victim ) ? 50 : 130;
	exp_gain = UMIN( 300, (exp_gain * UMIN( value, (get_max_hp(victim) - victim->hit) ) ) / 100 );
	if ( ch != victim )
	   gain_exp( ch, exp_gain, TRUE );

	victim->hit = UMIN( victim->hit + value, get_max_hp( victim ) );
	tmp = value;

	for ( i = 0; i <= 2; i++ )
	{
		if ( tmp <= victim->counter[ i ] )
		{
			victim->counter[ i ] -= tmp;
			tmp = 0;
		}
		else
		{
			tmp -= victim->counter[ i ];
			victim->counter[ i ] = 0;
		}
	}

	_heal = victim->hit - _heal;

	if ( _heal < 1 )
		return;

	if ( heal_value > 75 )
	{
		send_to_char( "Twoje cia³o wype³nia lecznicze ciep³o, prawie wszystkie twoje rany goj± siê.\n\r", victim );
		act( "Prawie wszystkie rany $z goj± siê.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_value > 50 )
	{
		send_to_char( "Twoje cia³o wype³nia lecznicze ciep³o, wiekszo¶æ twoich ran goi siê.\n\r", victim );
		act( "Wiekszo¶æ ran $z goi siê.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_value > 25 )
	{
		send_to_char( "Twoje cia³o wype³nia lecznicze ciep³o, kilka twoich g³ebokich ran goi siê.\n\r", victim );
		act( "Kilka g³êbokich ran $z goi siê.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_value > 10 )
	{
		send_to_char( "Twoje cia³o wype³nia lecznicze ciep³o, kilka twoich ran goi siê.\n\r", victim );
		act( "Kilka ran $z goi siê.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_value > 5 )
	{
		send_to_char( "Czujesz jak przez twoje cia³o przep³ywa fala ciep³a, kilka zranieñ znika.\n\r", victim );
		act( "Kilka zranieñ $z znika.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( heal_value > 0 )
	{
		send_to_char( "Twoje cia³o wype³nia lecznicze ciep³o, kilka siniaków znika.\n\r", victim );
		act( "Niektóre z siniaków $z znikaj±.", victim, NULL, NULL, TO_ROOM );
	}
	update_pos( victim );
	return;
}

void heal_undead( CHAR_DATA *ch, CHAR_DATA *victim, sh_int value )
{
	sh_int heal_value;
	int _heal, luck = get_curr_stat( ch, STAT_LUC );

	if ( !is_undead( victim ) )
    {
		return;
    }

    if ( ch == victim )
    {
		send_to_char( "W³a¶ciwie to nic siê nie dzieje.\n\r", ch );
        return;
    }

	_heal = victim->hit;

	// modyfikator zale¿ny od szczê¶cia dodatni
	if ( number_range( 0, luck ) > 90 ) _heal = ( _heal * 105 ) / 100;
	// modyfikator zale¿ny od szczê¶cia ujemny
	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) < 3 ) _heal = ( _heal * 9 ) / 10;

	heal_value = UMIN( get_max_hp( victim ) + 11 - victim->hit, value );
	heal_value = 100 * heal_value / get_max_hp( victim );
	victim->hit = UMIN( victim->hit + value, get_max_hp( victim ) );
	_heal = victim->hit - _heal;

	if ( _heal < 1 )
    {
		return;
    }

    if ( heal_value > 75 )
    {
        act( "Negatywna energia wype³nia ca³± pow³okê $z wszczepiaj±c w ni± nowe ¿ycie.", victim, NULL, NULL, TO_ROOM );
        if ( ! IS_NPC( victim ) )
        {
            send_to_char( "Negatywna energia wype³nia ca³<&ego/±/e> ciebie i wszczepia nowe ¿ycie.\n\r", victim );
        }
    }
    else if ( heal_value > 50 )
    {
        act( "Wiêkszo¶æ ubytków $z regeneruje siê.", victim, NULL, NULL, TO_ROOM );
        if ( ! IS_NPC( victim ) )
        {
            send_to_char( "Wiêkszo¶æ twoich ubytków regeneruje siê.\n\r", victim );
        }
    }
    else if ( heal_value > 25 )
    {
        act( "Kilka powa¿niejszych ubytków $z regeneruje siê.", victim, NULL, NULL, TO_ROOM );
        if ( ! IS_NPC( victim ) )
        {
            send_to_char( "Kilka twoich powa¿niejszych ubytków regeneruje siê.\n\r", victim );
        }
    }
    else if ( heal_value > 10 )
    {
        act( "Kilka ubytków $z regeneruje siê.", victim, NULL, NULL, TO_ROOM );
        if ( ! IS_NPC( victim ) )
        {
            send_to_char( "Kilka twoich ubytków regeneruje siê.\n\r", victim );
        }
    }
    else if ( heal_value > 5 )
    {
        act( "Kilka mniejszych ubytków $z regeneruje siê.", victim, NULL, NULL, TO_ROOM );
        if ( ! IS_NPC( victim ) )
        {
            send_to_char( "Kilka twoich mniejszych ubytków regeneruje siê.\n\r", victim );
        }
    }
    else if ( heal_value > 0 )
    {
        act( "Jeden ma³y ubytek $z regeneruje siê.", victim, NULL, NULL, TO_ROOM );
        if ( ! IS_NPC( victim ) )
        {
            send_to_char( "Twój jeden ma³y ubytek regeneruje siê.\n\r", victim );
        }
    }
    update_pos( victim );
    return;
}

/* LECZENIA (I RANIENIA) W KOLEJNOSCI SILY */

/* leczenie lekkich ran (skladnia: cast 'cure light' [kto])*/
void spell_cure_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    bool check = FALSE;
    AFFECT_DATA *paf;
    int holy_heal_mod = 100;
    bool pool_mod = TRUE;

    if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
         if (paf->modifier > 24 && paf->modifier < 36)
         {
            holy_heal_mod = 50;
            pool_mod = FALSE;
            send_to_char( "Czujesz jak brak modlitwy os³abia twoj± moc.\n\r", ch );
         }

         if (paf->modifier >= 36)
         {
            send_to_char( "Inkantujesz zaklêcie lecz±ce, jednak nie mo¿esz znale¼æ nawet iskry mocy w wype³niaj±cej cie pustce.\n\r", ch );
            return;
         }

    }
    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL  && pool_mod == TRUE)
       {
           if(paf->modifier > 20 && number_percent() > URANGE(21,paf->modifier,40))
           {
             paf->modifier -= 1;
             holy_heal_mod += 30;

           }

           if(paf->modifier > 40 && number_percent() > URANGE(21,paf->modifier,40))
           {
             paf->modifier -= 1;
             holy_heal_mod += 30;

           }

           if(paf->modifier > 60 && number_percent() > URANGE(21,paf->modifier,40))
           {
             paf->modifier -= 1;
             holy_heal_mod += 30;

           }
           //minusy
           if (paf->modifier < -80)
           {
              if(number_percent() + ( paf->modifier / 2 ) > 0 )
              {
              send_to_char( "Próbujesz zebraæ w sobie si³ê lecz±c±, jednak czujesz, ¿e boska moc zosta³a ci zabrana.\n\r", ch );
              return;
              }else
              {
               send_to_char( "Próbujesz zebraæ w sobie si³ê lecz±c±, po chwili twoje serce wype³nia przera¿enie, z twoim zaklêciem sta³o siê co¶ okropnego.\n\r", ch );
               if ( spell_damage( ch, ( CHAR_DATA * ) vo, 6 + dice( 1, 5 ) + URANGE(1, level/2, 15) , sn, DAM_NEGATIVE, TRUE ) == TRUE )
                 {
		         send_to_char( "Czujesz siê gorzej.\n\r", victim );
                 if ( ch != victim )
			     act( "$n czuje siê gorzej.", victim, NULL, NULL, TO_ROOM );
			     return;
                 }

              }
          }
           if (paf->modifier < -60 )
           {
              if(number_percent() - paf->modifier/2 > 0)
              {
              holy_heal_mod -= 30;
              }else
              {
              send_to_char( "Próbujesz zebraæ w sobie si³ê lecz±c±, jednak czujesz, ¿e boska moc zosta³a ci zabrana.\n\r", ch );
              return;
              }
           }
           if (paf->modifier < -40) holy_heal_mod -= 30;
           if (paf->modifier < -20) holy_heal_mod -= 30;
       }
     if(holy_heal_mod > 100) send_to_char( "{gCzujesz jak si³a twojego zaklêcia ro¶nie.{x\n\r", ch);
     if(holy_heal_mod < 100) send_to_char( "{rCzujesz jak si³a twojego zaklêcia s³abnie.{x\n\r", ch);



	if ( IS_NPC( ch ) || ch->class == CLASS_CLERIC || check )
    {
		heal_char( ch, victim, 6 + dice( 1, 5 ) + URANGE(1, level/2, 15), FALSE );
    }
	else
    {
		heal_char( ch, victim, ((6 + dice( 1, 5 ) + URANGE(1, level/2, 15))*holy_heal_mod)/100, FALSE );
    }
}

int cause_damage_helper( int kind, int level )
{
    return ( 6 * kind + dice( kind, 5) + URANGE( 0, level, kind * 15 ) );
}

/* zadawanie lekkich ran (skladnia: cast 'cause light' [kto]) */
void spell_cause_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    int dam, luck = get_curr_stat( ch, STAT_LUC );
    if ( number_range( 0, luck ) < 3 )
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        return;
    }
    if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
    {
        return;
    }
    dam = luck_dam_mod( ch, cause_damage_helper( 1, level ) );
    if ( spell_damage( ch, ( CHAR_DATA * ) vo, dam , sn, DAM_NEGATIVE, TRUE ) == TRUE )
    {
        send_to_char( "Czujesz siê gorzej.\n\r", victim );
        if ( ch != victim )
        {
            act( "$n czuje siê gorzej.", victim, NULL, NULL, TO_ROOM );
        }
    }
    return;
}

/* leczenie ¶rednich ran (skladnia: cast 'cure moderate wounds' [kto]) */
void spell_cure_moderate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA *paf;
    int holy_heal_mod = 100;
    bool pool_mod = TRUE;
if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
         if (paf->modifier > 24 && paf->modifier < 36)
         {
            holy_heal_mod = 50;
            pool_mod = FALSE;
            send_to_char( "Czujesz jak brak modlitwy os³abia tw± moc.\n\r", ch );
         }

         if (paf->modifier > 36)
         {
            send_to_char( "Inkantujesz zaklêcie lecz±ce, jednak nie mo¿esz znale¼æ nawet iskry mocy w wype³niaj±cej cie pustce.\n\r", ch );
            return;
         }

    }
    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL  && pool_mod == TRUE)
       {
           if(paf->modifier > 20 && number_percent() > URANGE(10,paf->modifier/2,40))
           {
             paf->modifier -= 1;
             holy_heal_mod += 30;

           }

           if(paf->modifier > 40 && number_percent() > URANGE(10,paf->modifier/3,40))
           {
             paf->modifier -= 1;
             holy_heal_mod += 30;

           }

           if(paf->modifier > 60 && number_percent() > URANGE(10,paf->modifier/3,40))
           {
             paf->modifier -= 1;
             holy_heal_mod += 30;

           }
           //minusy
           if (paf->modifier < -80)
           {
              if(number_percent() + (paf->modifier/2) > 0)
              {
              send_to_char( "Próbujesz zebraæ w sobie si³ê lecz±c±, jednak czujesz, ¿e boska moc zosta³a ci zabrana.\n\r", ch );
              return;
              }else
              {
                send_to_char( "Próbujesz zebraæ w sobie si³ê lecz±c±, po chwili twoje serce wype³nia przera¿enie, z twoim zaklêciem sta³o siê co¶ okropnego.\n\r", ch );

              if ( spell_damage( ch, ( CHAR_DATA * ) vo, 6 + dice( 1, 5 ) + URANGE(1, level/2, 15) , sn, DAM_NEGATIVE, TRUE ) == TRUE )
              {
		         send_to_char( "Czujesz siê gorzej.\n\r", victim );
                 if ( ch != victim )
			     act( "$n czuje siê gorzej.", victim, NULL, NULL, TO_ROOM );
			     return;
              }

              }
           }
           if (paf->modifier < -60 )
           {
              if(number_percent() - paf->modifier/2 > 0)
              {
              holy_heal_mod -= 30;
              }else
              {
              send_to_char( "Próbujesz zebraæ w sobie si³ê lecz±c±, jednak czujesz, ¿e boska moc zosta³a ci zabrana.\n\r", ch );
              return;
              }
           }
           if (paf->modifier < -40) holy_heal_mod -= 30;
           if (paf->modifier < -20) holy_heal_mod -= 30;
       }
     if(holy_heal_mod > 100) send_to_char( "{gCzujesz jak si³a twojego zaklêcia ro¶nie.{x\n\r", ch);
     if(holy_heal_mod < 100) send_to_char( "{rCzujesz jak si³a twojego zaklêcia s³abnie.{x\n\r", ch);

	if ( IS_NPC( ch ) || ch->class == CLASS_CLERIC )
    {
		heal_char( ch, victim, 15 + dice( 3, 6 ) + URANGE(5, level, 30), FALSE );
    }
	else
    {
		heal_char( ch, victim, ((15 + dice( 3, 6 ) + URANGE(5, level, 30))*holy_heal_mod)/100, FALSE );
    }
}

void spell_cause_moderate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    int dam, luck = get_curr_stat( ch, STAT_LUC );
    if ( number_range( 0, luck ) < 5 )
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        return;
    }
    if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
    {
        return;
    }
    dam = luck_dam_mod( ch, cause_damage_helper( 2, level ) );
    if ( spell_damage( ch, ( CHAR_DATA * ) vo, dam , sn, DAM_NEGATIVE, TRUE ) == TRUE )
    {
        send_to_char( "Czujesz siê ¼le.\n\r", victim );
        if ( ch != victim )
        {
            act( "$n czuje siê ¼le.", victim, NULL, NULL, TO_ROOM );
        }
    }
    return;
}

/* leczenie powaznych ran (skladnia: cast 'cure serious' [kto]) */
void spell_cure_serious( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA *paf;
    int holy_heal_mod = 100;
    bool pool_mod = TRUE;
    if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
        if (paf->modifier > 24 && paf->modifier < 36)
        {
            holy_heal_mod = 50;
            pool_mod = FALSE;
            send_to_char( "Czujesz jak brak modlitwy os³abia tw± moc.\n\r", ch );
        }

        if (paf->modifier > 36)
        {
            send_to_char( "Inkantujesz zaklêcie lecz±ce, jednak nie mo¿esz znale¼æ nawet iskry mocy w wype³niaj±cej cie pustce.\n\r", ch );
            return;
        }

    }
    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL  && pool_mod == TRUE)
    {
        if(paf->modifier > 20 && number_percent() > URANGE(10,paf->modifier/2,40))
        {
            paf->modifier -= 2;
            holy_heal_mod += 30;

        }

        if(paf->modifier > 40 && number_percent() > URANGE(10,paf->modifier/3,40))
        {
            paf->modifier -= 1;
            holy_heal_mod += 30;

        }

        if(paf->modifier > 60 && number_percent() > URANGE(10,paf->modifier/3,40))
        {
            paf->modifier -= 1;
            holy_heal_mod += 30;

        }
        //minusy
        if (paf->modifier < -80)
        {
            if(number_percent() + (paf->modifier/2) > 0)
            {
                send_to_char( "Próbujesz zebraæ w sobie si³ê lecz±c±, jednak czujesz, ¿e boska moc zosta³a ci zabrana.\n\r", ch );
                return;
            }else
            {
                send_to_char( "Próbujesz zebraæ w sobie si³ê lecz±c±, po chwili twoje serce wype³nia przera¿enie, z twoim zaklêciem sta³o siê co¶ okropnego.\n\r", ch );

                if ( spell_damage( ch, ( CHAR_DATA * ) vo, 6 + dice( 1, 5 ) + URANGE(1, level/2, 15) , sn, DAM_NEGATIVE, TRUE ) == TRUE )
                {
                    send_to_char( "Czujesz siê gorzej.\n\r", victim );
                    if ( ch != victim )
                        act( "$n czuje siê gorzej.", victim, NULL, NULL, TO_ROOM );
                    return;
                }

            }
        }
        if (paf->modifier < -60 )
        {
            if(number_percent() - paf->modifier/2 > 0)
            {
                holy_heal_mod -= 30;
            }else
            {
                send_to_char( "Próbujesz zebraæ w sobie si³ê lecz±c±, jednak czujesz, ¿e boska moc zosta³a ci zabrana.\n\r", ch );
                return;
            }
        }
        if (paf->modifier < -40) holy_heal_mod -= 30;
        if (paf->modifier < -20) holy_heal_mod -= 30;
    }
    if(holy_heal_mod > 100) send_to_char( "{gCzujesz jak si³a twojego zaklêcia ro¶nie.{x\n\r", ch);
    if(holy_heal_mod < 100) send_to_char( "{rCzujesz jak si³a twojego zaklêcia s³abnie.{x\n\r", ch);

    if ( IS_NPC( ch ) || ch->class == CLASS_CLERIC )
    {
        heal_char( ch, victim, 20 + dice( 4, 6 ) + URANGE(10, (level*2)/3, 50), FALSE );
    }
    else
    {
        if(ch->class == CLASS_PALADIN)
        {
            heal_char( ch, victim,(( 20 + dice( 4, 6 ) + URANGE(10, (level*2)/3, 50))*holy_heal_mod)/100, FALSE );
            //Raszer zmiany w cs dla paladyna. Pal ma najwyzej 19 level magiczny.
        }else
        {
            //coby druid nie mial za dobrze
            heal_char( ch, victim, 20 + dice( 4, 6 ) + URANGE(10, (level*2)/3, 50), FALSE );

        }
    }
}

/* zadawanie powaznych ran (skladnia: cast 'cause seriuos' <kto>) */
void spell_cause_serious( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    int dam, luck = get_curr_stat( ch, STAT_LUC );
    if ( number_range( 0, luck ) < 6 )
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        return;
    }
    if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
    {
        return;
    }
    dam = luck_dam_mod( ch, cause_damage_helper( 3, level ) );
    if ( spell_damage( ch, ( CHAR_DATA * ) vo, dam , sn, DAM_NEGATIVE, TRUE ) == TRUE )
    {
        send_to_char( "Czujesz siê du¿o gorzej.\n\r", victim );
        if ( ch != victim )
        {
            act( "$n czuje siê du¿o gorzej.", victim, NULL, NULL, TO_ROOM );
        }
    }
    return;
}

/* leczenie krytycznych ran (skladnia: cast 'cure critical' [kto]) */
void spell_cure_critical( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;

	if ( IS_NPC( ch ) || ch->class == CLASS_CLERIC )
		heal_char( ch, victim, 50 + dice( 6, 6 ) + URANGE(10, level*2, 60), FALSE );
	else
		heal_char( ch, victim, 50 + dice( 6, 6 ) + URANGE(0, level*2, 50), FALSE );

	return;
}

/* zadawanie krytycznych ran (skladnia: cast 'cause critical' <kto>) */
void spell_cause_critical( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    int dam, luck = get_curr_stat( ch, STAT_LUC );
    if ( number_range( 0, luck ) < 7 )
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        return;
    }
    if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
    {
        return;
    }
    dam = luck_dam_mod( ch, cause_damage_helper( 4, level ) );
    if ( spell_damage( ch, ( CHAR_DATA * ) vo, dam , sn, DAM_NEGATIVE, TRUE ) == TRUE )
    {
        send_to_char( "Czujesz siê naprawdê ¼le.\n\r", victim );
        if ( ch != victim )
        {
            act( "$n czuje siê naprawdê ¼le.", victim, NULL, NULL, TO_ROOM );
        }
    }
    return;
}

/* Uzdrowienie (skladnia: cast heal [kto]) */
void spell_heal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;

	if ( !IS_NPC( ch ) )
	{
      	if( !spell_item_check( ch, sn , "komponent do heal" ) )
    {
		send_to_char( "Brakuje ci odpowiedniego katalizatora.\n\r", ch );
		return;
    }
	heal_char( ch, victim, 100 + dice( 10, 6 ) + URANGE(30, level*2, 80) , FALSE ); // 140-240
	}
	else
	heal_char( ch, victim, 150 + dice( 10, 20 ) , FALSE );


	if ( is_affected( victim, gsn_feeblemind ) )
		affect_strip( victim, gsn_feeblemind );
	return;
}

void spell_harm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, luck = get_curr_stat( ch, STAT_LUC );

	if ( number_range( 0, LUCK_BASE_MOD + luck/6 ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
		return;

   	if ( !IS_NPC( ch ) )
	{
   	if( !spell_item_check( ch, sn , "komponent do harm" ) )
    {
		send_to_char( "Brakuje ci odpowiedniego katalizatora.\n\r", ch );
		return;
    }
    else dam = 70 + dice( 10, 6 ) + URANGE(40, level*2, 80); //120-210
	}
	else dam = 150 + dice( 10, 20 );

	dam = luck_dam_mod( ch, dam );

	spell_damage( ch, victim, dam, sn, DAM_NEGATIVE , TRUE );

	return;
}

/* KONIEC LECZEN (RANIEN) */

/* blogoslawienstwo (skladnia: cast bless [kto])
 *
 * ogolnie ma byc:
 * na grupke/goscia:  na  bron  : HR +(1-2)
 * HR +(2-3)              zbroje: AC -(2-3)
 * VS -(1-2)              inne  : VS -(1-2)
 */
void spell_bless( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj;
	AFFECT_DATA af;
	int s_level, s_duration;
	int luck = get_curr_stat( ch, STAT_LUC );

	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;

		if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
		{
            switch(obj->gender)
            {
                case GENDER_MESKI:
    			    act( "$p jest ju¿ pob³ogos³awiony.", ch, obj, NULL, TO_CHAR );
                    break;
                case GENDER_ZENSKI:
    			    act( "$p jest ju¿ pob³ogos³awiona.", ch, obj, NULL, TO_CHAR );
                    break;
                case GENDER_NIJAKI:
    			    act( "$p jest ju¿ pob³ogos³awione.", ch, obj, NULL, TO_CHAR );
                    break;
                case GENDER_MESKOOSOBOWY:
    			    act( "$p s± ju¿ pob³ogos³awione.", ch, obj, NULL, TO_CHAR );
                    break;
                case GENDER_NONE:
                default:
        			send_to_char( "Ten przedmiot jest ju¿ pob³ogos³awiony.\n\r", ch );
                    break;
            }
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_EVIL ) )
		{
			AFFECT_DATA * paf;

			paf = affect_find( obj->affected, gsn_curse );
			if ( !saves_dispel( level, paf != NULL ? paf->level : 1, 0 ) )
			{
				if ( paf != NULL ) affect_remove_obj( obj, paf );
				act( "Zimna aura z³a wokó³ $f rozp³ywa siê.", ch, obj, NULL, TO_ALL );
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_EVIL );
				return;
			}
			else
			{
				act( "Nic nie poradzisz wobec mrocznej magii $f.", ch, obj, NULL, TO_CHAR );
				return;
			}
		}

		if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
		{
			send_to_char( "Nie uda³o ci siê wyb³agaæ b³ogos³awieñstwa.\n\r", ch );
			return;
		}

		s_level = level - 10;
		if ( number_range( 0, luck ) > 60 ) s_level += 5;
		if ( number_range( 0, luck ) > 90 ) s_level += 5;
		if ( number_range( 0, luck ) < 30 ) s_level -= 5;
		if ( number_range( 0, luck ) < 18 ) s_level -= 5;

		s_duration = 4 + level / 4;
		if ( number_range( 0, luck ) < 30 ) s_duration -= 1;
		if ( number_range( 0, luck ) < 18 ) s_duration -= 1;

		if ( obj->item_type == ITEM_WEAPON )
		{
			af.where = TO_OBJECT;
			af.type = sn;
			af.level = UMAX( 1, s_level );
			af.duration = UMAX( 1, s_duration ); af.rt_duration = 0;
			af.bitvector = &ITEM_BLESS;
			af.location = APPLY_HITROLL;
			af.modifier = 1 + ( level > 15 ? 1 : 0 );
			affect_to_obj( obj, &af );
		}
		else
		{
			act( "$p nie da siê pob³ogos³awiæ.", ch, obj, NULL, TO_CHAR );
			return;
		}

		act( "$p p³onie b³ogos³awion± aur±.", ch, obj, NULL, TO_ALL );
		return;
	}

	victim = ( CHAR_DATA * ) vo;

	if ( is_affected( victim, sn ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Ju¿ jeste¶ b³ogos³awion<&y/a/e>.\n\r", victim );
		}
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N ju¿ jest b³ogos³awione.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N ju¿ jest b³ogos³awiona.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "$N ju¿ jest b³ogos³awiony.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( victim->class == CLASS_BLACK_KNIGHT || is_undead(victim) )
	{
		send_to_char( "Zaklêcie nie zadzia³a³o.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê wyb³agaæ b³ogos³awieñstwa.\n\r", ch );
		return;
	}

	s_level = level - 10;
	if ( number_range( 0, luck ) > 90 ) s_level += 5;
	if ( number_range( 0, luck ) > 60 ) s_level += 5;
	if ( number_range( 0, luck ) < 30 ) s_level -= 5;
	if ( number_range( 0, luck ) < 18 ) s_level -= 5;

	s_duration = 4 + level / 7;
	if ( number_range( 0, luck ) > 90 ) s_duration += 1 + level / 6;
	if ( number_range( 0, luck ) > 60 ) s_duration += level / 6;
	if ( number_range( 0, luck ) < 30 ) s_duration -= 1;
	if ( number_range( 0, luck ) < 18 ) s_duration -= 1;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = UMAX( 1, s_level );
	af.duration = UMAX( 1, s_duration ); af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.modifier = 1 + ( level > 15 ? 1 : 0 );
	af.bitvector = &AFF_NONE;

	affect_to_char( victim, &af, NULL, TRUE );

	af.location = APPLY_SAVING_SPELL;
	af.modifier = -1 - ( level > 15 ? 1 : 0 );
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Na $c sp³ywa boskie b³ogos³awieñstwo.", victim, NULL, NULL, TO_ROOM );
	if ( victim == ch ) send_to_char( "Sam<&/a/o> siebie b³ogos³awisz.\n\r", victim );
	else act( "Za po¶rednictwem $z sp³ywa na ciebie boskie b³ogos³awieñstwo.", ch, NULL, victim, TO_VICT );

	return;
}

/* Oczyszczenie pokarmu (skladnia: cast 'purify food' <co>)
 * odswieza plyny, jedzenie
 */
void spell_purify_food( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	int luck = get_curr_stat( ch, STAT_LUC );
	if ( obj->item_type != ITEM_DRINK_CON && obj->item_type != ITEM_FOOD )
	{
		send_to_char( "Tylko jedzenie i picie mo¿esz w ten sposób oczy¶ciæ.\n\r", ch );
		return;
	}
	if ( !obj->value[ 3 ] )
	{
		act( "$p nie jest zatrute.", ch, obj, NULL, TO_CHAR );
		return;
	}
	if ( number_range( 0, luck ) > 5 )
	{
		obj->value[ 3 ] = 0;
		act( "Usuwasz truciznê z $f.", ch, obj, NULL, TO_CHAR );
		act( "$n usuwa truciznê z $f.", ch, obj, NULL, TO_ROOM );
	}
	else
    {
		act( "Nie uda³o ci siê usun±æ trucizny z $f.", ch, obj, NULL, TO_CHAR );
    }
	return;
}

/*
 * S³up ognia (skladnia: cast flamestrike <kto>)
 */

void spell_flamestrike( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
//    OBJ_DATA *obj;
	int dam, luck = get_curr_stat( ch, STAT_LUC ), type = 0;

	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ kolumny p³omieni.\n\r", ch );
		return;
	}

	if ( !IS_OUTSIDE( ch ) || IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOWEATHER ) )
	{
		type = 0;
		dam = number_range( 15, 25 ) + dice( level, 4 );
	}
	else if( ch->level > 25 && number_percent() < 5*(get_curr_stat( ch, STAT_WIS)/6-20) )
	{
		type = 2;
		if( ch->class == CLASS_DRUID )
			dam = number_range( 15, 35 ) + dice( level, 5 ) + number_range( 10, (level+get_curr_stat( ch, STAT_WIS)/6+get_curr_stat(ch,STAT_LUC)/6-10)/2 );
		else
			dam = number_range( 15, 35 ) + dice( level, 5 ) + number_range( 4, (level+get_curr_stat( ch, STAT_WIS)/6+get_curr_stat(ch,STAT_LUC)/6-10)/2 );
	}
	else
	{
		type = 1;
		if( ch->class == CLASS_DRUID )
			dam = number_range( 15, 35 ) + dice( level, 4 ) + number_range( 5, (level+get_curr_stat( ch, STAT_WIS)/6)/3 );
		else
			dam = number_range( 15, 35 ) + dice( level, 4 ) + number_range( 0, (level+get_curr_stat( ch, STAT_WIS)/6)/3 );
	}

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( type != 2 && saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		{
			dam -= dam / 4;
			if( type != 1 )
			{
            	if ( ch == victim )
	                act( "Otaczasz siê kolumn± p³omieni.", ch, NULL, victim, TO_CHAR );
            	else
	                act( "Kolumna p³omieni obejmuje $C.", ch, NULL, victim, TO_CHAR );
				act( "Kolumna rycz±cych p³omieni obejmuje ciê.", ch, NULL, victim, TO_VICT );
				act( "Kolumna rycz±cych p³omieni obejmuje $C.", ch, NULL, victim, TO_NOTVICT );
			}
			else
			{
                act( "S³up ognia sp³ywa z nieba dos³ownie stopê od ciebie przypalaj±c bole¶nie tw± skórê.", ch, NULL, victim, TO_VICT );
				act( "S³up ognia sp³ywa z nieba dos³ownie stopê od $z przypalaj±c bole¶nie $s skórê.", victim, NULL, NULL, TO_ROOM );
			}
		}
		else
        {
        	if( type == 2 )
			{
                act( "S³up ognia sp³ywa z nieba prosto na ciebie, pokrywaj±c ciê <&ca³ego/ca³±/ca³e> p³omieniami.", ch, NULL, victim, TO_VICT );
				act( "S³up ognia sp³ywa z nieba prosto na $c, pokrywaj±c ca³e $s cia³o p³omieniami.", victim, NULL, NULL, TO_ROOM );
			}
        	else if( type == 1 )
			{
                act( "S³up ognia sp³ywa z nieba prosto na ciebie, pokrywaj±c ciê p³omieniami prawie w ca³o¶ci.", ch, NULL, victim, TO_VICT );
                if( victim->sex == SEX_FEMALE )
					act( "S³up ognia sp³ywa z nieba prosto na $c, pokrywaj±c j± prawie w ca³o¶ci p³omieniami.", victim, NULL, NULL, TO_ROOM );
				else
					act( "S³up ognia sp³ywa z nieba prosto na $c, pokrywaj±c go prawie w ca³o¶ci p³omieniami.", victim, NULL, NULL, TO_ROOM );
			}
        	else
        	{
            	if ( ch == victim )
	                act( "Otaczasz siê kolumn± potê¿nych p³omieni.", ch, NULL, victim, TO_CHAR );
            	else
	               	act( "Kolumna potê¿nych p³omieni otacza $C.", ch, NULL, victim, TO_CHAR );
				act( "Kolumna rycz±cych p³omieni otacza ciê.", ch, NULL, victim, TO_VICT );
				act( "Kolumna rycz±cych p³omieni otacza $C.", ch, NULL, victim, TO_NOTVICT );
			}
        }
	}
	else
    {
    	if( type == 2 )
		{
			act( "Olbrzymi s³up ognia sp³ywa z nieba prosto na ciebie, prawie toniesz w p³omieniach!", ch, NULL, victim, TO_VICT );
			act( "Olbrzymi s³up ognia sp³ywa z nieba prosto na $c, $s cia³o prawie tonie w p³omienich!", victim, NULL, NULL, TO_ROOM );
			DAZE_STATE( victim, URANGE( 0, 25 - get_curr_stat( victim, STAT_CON )/6, 12 ));
		}
		else if( type == 1 )
		{
			act( "S³up ognia sp³ywa z nieba prosto na ciebie, pokrywaj±c ciê <&ca³ego/ca³±/ca³e> p³omieniami.", ch, NULL, victim, TO_VICT );
			act( "S³up ognia sp³ywa z nieba prosto na $c, pokrywaj±c ca³e $s cia³o p³omieniami.", victim, NULL, NULL, TO_ROOM );
		}
		else
		{
        	if ( ch == victim )
	            act( "Otaczasz siê ca³kowicie kolumn± rycz±cych p³omieni!", ch, NULL, victim, TO_CHAR );
        	else
	       		act( "Kolumna rycz±cych p³omieni ca³kowicie otacza $C!", ch, NULL, victim, TO_CHAR );
			act( "Kolumna rycz±cych p³omieni spada na ciebie!", ch, NULL, victim, TO_VICT );
			act( "Kolumna rycz±cych p³omieni opada na $C.", ch, NULL, victim, TO_NOTVICT );
		}
    }
    if( !spell_item_check( ch, sn , "fire based spell" ) )
    {
      dam -= dam/5;
    }

	spell_damage( ch, victim, dam, sn, DAM_FIRE, FALSE );
	fire_effect( victim, level, dam, TARGET_CHAR );
	return;
}

/* Swiatlo (skladnia: cast 'light')
 *
 * no stworzy kule swiatla (nie mam teraz pomyslu na nic innego)
 * ew moze zrobic aureolke bedzie po klerycku *giggle*
 */

void spell_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj;
	int luck = get_curr_stat( ch, STAT_LUC );
	int timer;
	AFFECT_DATA *paf;


	if ( !IS_NPC( ch ) && number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "W ostatniej chwili rozpraszasz swoj± uwagê.\n\r", ch );
		return;
	}

	timer = 10 + level / 10;
	if ( number_range( 0, luck ) > 90 ) timer += 2;
	if ( number_range( 0, luck ) < 18 ) timer -= 3;


	if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
        if ( paf->modifier < 30 )
        {
             if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
                  {

                    if ( number_percent() - paf->modifier * 2 < 0 && number_percent() < 30 )
                    {
                       send_to_char( "Czujesz jak zaklêcie rezonuje z zebran± w tobie moc±.\n\r", ch );
	                   timer += 10;
	                   obj = create_object( get_obj_index( OBJ_VNUM_GOOD_BALL ), FALSE );
                       paf->modifier -= 10;
	                   if (paf->modifier < 1) paf->modifier = 1;
	                   obj_to_char( obj, ch );
                       obj->timer = UMAX( 1, timer );
                       act( "Na rêku $z pojawia siê mlecznobia³a kula ¶wiat³a.", ch, obj, NULL, TO_ROOM );
		               act( "Na twoim rêku pojawia siê mlecznobia³a kula ¶wiat³a.", ch, obj, NULL, TO_CHAR );
                       return;
                    }

                    if (number_percent() < (-paf->modifier))
                    {
                       send_to_char( "Czujesz jak wype³niaj±cy ciê mrok uniemo¿liwia ci zebranie ¶wiat³a.\n\r", ch );
	                   return;

                    }


                  }


        }else
        {
             send_to_char( "Brak modlitwy powoduje i¿ ¶wiat³o gromadz±ce siê na twojej d³oni wygasa.\n\r", ch );
	         return;

        }

    }
    obj = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ), FALSE );
	obj_to_char( obj, ch );
	obj->timer = UMAX( 1, timer );

	act( "Na rêku $z pojawia siê kula ¶wiat³a.", ch, obj, NULL, TO_ROOM );
	act( "Na twoim rêku pojawia siê kula ¶wiat³a.", ch, obj, NULL, TO_CHAR );
	return;
}

void spell_spirit_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA * obj;
    int luck = get_curr_stat( ch, STAT_LUC );
    int timer;

    if ( !IS_NPC( ch ) && number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
    {
        send_to_char( "W ostatniej chwili rozpraszasz swoj± uwagê.\n\r", ch );
        return;
    }

    timer = 10 + level / 10;
    if ( number_range( 0, luck ) > 90 )
    {
        timer += 2;
    }
    else if ( number_range( 0, luck ) < 18 )
    {
        timer -= 2;
    }

    if ( IS_EVIL( ch ) )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_EVIL_BALL ), FALSE );
        act( "Na rêku $z pojawia siê ciemnoczerwona, pulsuj±ca kula ¶wiat³a.", ch, obj, NULL, TO_ROOM );
        act( "Na twoim rêku pojawia siê ciemnoczerwona, pulsuj±ca kula ¶wiat³a.", ch, obj, NULL, TO_CHAR );
    }
    else if ( IS_GOOD( ch ) )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_GOOD_BALL ), FALSE );
        act( "Na rêku $z pojawia siê mlecznobia³a kula ¶wiat³a.", ch, obj, NULL, TO_ROOM );
        act( "Na twoim rêku pojawia siê mlecznobia³a kula ¶wiat³a.", ch, obj, NULL, TO_CHAR );
    }
    else if ( IS_NEUTRAL( ch ) )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_NEUTRAL_BALL ), FALSE );
        act( "Na rêku $z pojawia siê b³êkitna kula boskiego ¶wiat³a.", ch, obj, NULL, TO_ROOM );
        act( "Na twoim rêku pojawia siê b³êkitna kula boskiego ¶wiat³a.", ch, obj, NULL, TO_CHAR );
    }
    else

    {
        send_to_char( "W³a¶ciwie to nic siê nie dzieje.\n\r", ch );
        return;
    }
    obj_to_char( obj, ch );
    obj->timer = UMAX( 1, timer );
}

void spell_spirit_hammer( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   OBJ_DATA * obj = NULL;
   int value, skill = get_skill ( ch, gsn_mace );
   int luck = get_curr_stat( ch, STAT_LUC );

   if ( number_range( 0, LUCK_BASE_MOD + luck/6 ) < 1 )
      {
	 send_to_char( "Nie uda³o ci siê przywo³aæ widmowego m³ota.\n\r", ch );
	 return;
      }

   if ( get_hand_slots( ch, WEAR_WIELD ) >= 2 )
      {
	 send_to_char( "Masz ju¿ dwie zajête rêce. Zdejmij co¶.\n\r", ch );
	 return;
      }

   if ( get_eq_char( ch, WEAR_WIELD ) )
      {
	 send_to_char( "Nosisz ju¿ jak±¶ broñ. Zdejmij j±.\n\r", ch );
	 return;
      }

   obj = create_object( get_obj_index( OBJ_VNUM_SPIRITUAL_HAMMER ), FALSE );

   if ( !obj )
      {
	 send_to_char( "Puff ... i nic.\n\r", ch );
	 bug( "Brak obiektu w dla czaru spell_spiritual_weapon.", 0 );
	 return;
      }

   EXT_SET_BIT( obj->extra_flags, ITEM_GLOW );
   EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );

   value = 2 * level;
   /*
    * moby dluzej
    * PC - zalezy od szczescia
    */
   if ( IS_NPC ( ch ) )
      {
	 value *= 4;
      }
   else
      {
	 if ( number_range( 0, luck ) > 90 ) value *= 2;
	 if ( number_range( 0, luck ) < 30 ) value /= 2;
      }
   obj->timer = value;

   value = 2 * level;
   if ( number_range( 0, luck ) > 90 ) value = ( value * 9 ) / 10;
   if ( number_range( 0, luck ) < 30 ) value = ( value * 115 ) / 100;
   obj->rent_cost = value;

   value = level / 3;
   if ( number_range( 0, luck ) > 60 ) ++value;
   if ( number_range( 0, luck ) < 18 ) --value;

   /*
    * dodaje bonus za posiadane umiejetnosci w danej broni
    */
   if ( skill > 50 ) { value++; }
   if ( skill > 30 ) { value++; }
   if ( skill > 10 ) { value++; }

   value = UMAX( 0, value );

   /*
    * mobom limitujemy mozliwe do wyloswania bronie
    */
   if ( IS_NPC ( ch ) )
      {
	 value = UMIN ( 8, value );
      }

   switch ( value )
      {
      case 0:
	 obj->value[ 1 ] = 1;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 1;
	 obj->value[ 6 ] = 1;
	 break;
      case 1:
	 obj->value[ 1 ] = 1;
	 obj->value[ 2 ] = 5;
	 obj->value[ 5 ] = 1;
	 obj->value[ 6 ] = 1;
	 break;
      case 2:
	 obj->value[ 1 ] = 1;
	 obj->value[ 2 ] = 6;
	 obj->value[ 5 ] = 1;
	 obj->value[ 6 ] = 1;
	 break;
      case 3:
	 obj->value[ 1 ] = 1;
	 obj->value[ 2 ] = 5;
	 obj->value[ 5 ] = 2;
	 obj->value[ 6 ] = 1;
	 break;
      case 4:
	 obj->value[ 1 ] = 1;
	 obj->value[ 2 ] = 5;
	 obj->value[ 5 ] = 3;
	 obj->value[ 6 ] = 1;
	 break;
      case 5:
	 obj->value[ 1 ] = 1;
	 obj->value[ 2 ] = 5;
	 obj->value[ 5 ] = 4;
	 obj->value[ 6 ] = 1;
	 break;
      case 6:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 1;
	 obj->value[ 6 ] = 1;
	 break;
      case 7:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 2;
	 obj->value[ 6 ] = 1;
	 break;
      case 8:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 3;
	 obj->value[ 6 ] = 1;
	 break;
      case 9:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 2;
	 obj->value[ 6 ] = 2;
	 break;
      case 10:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 5;
	 obj->value[ 5 ] = 2;
	 obj->value[ 6 ] = 2;
	 break;
      case 11:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 3;
	 obj->value[ 6 ] = 3;
	 break;
      default:
	 obj->value[ 1 ] = 2;
	 obj->value[ 2 ] = 4;
	 obj->value[ 5 ] = 4;
	 obj->value[ 6 ] = 4;
	 break;
      }

   /**
    * dodatkowy bonus broni
    */

   if ( !IS_NPC ( ch ) )
      {

	 if ( value > 6 && number_percent() < 30 )
	    {
	       SPEC_DAMAGE *specdam = new_spec_damage();

	       specdam->type   = SD_TYPE_ALIGN;
	       // FIXME: dodaæ zale¿no¶ci od alignu klera
	       specdam->param1 = 0; // align ofiary
	       specdam->param2 = 1; // align atakujacego

	       specdam->target_type = SD_TARGET_DAM;
	       specdam->bonus  = URANGE ( 1, level/5, 7 );
	       specdam->chance = URANGE ( 10, 2*level, 70 );

	       specdam->next  = obj->spec_dam;
	       obj->spec_dam = specdam;
	    }

	 if ( value > 7 && number_percent() < 20 )
	    {
	       SPEC_DAMAGE *specdam = new_spec_damage();

	       specdam->type   = SD_TYPE_MAGIC;
	       specdam->param1 = DAM_HOLY;
	       specdam->param2 = 0;
	       specdam->bonus  = URANGE ( 50, 10*level, 200 );
	       specdam->chance = URANGE ( 1, level/10, 4 );
	       specdam->next  = obj->spec_dam;
	       obj->spec_dam = specdam;
	    }

	 if ( value > 9 && number_percent() < 10 )
	    {
	       SPEC_DAMAGE *specdam = new_spec_damage();

	       specdam->type   = SD_TYPE_ALIGN;
	       // FIXME: dodaæ zale¿no¶ci od alignu klera
	       specdam->param1 = 0; // align ofiary
	       specdam->param2 = 1; // align atakujacego

	       specdam->target_type = SD_TARGET_MAG;
	       specdam->bonus  = URANGE ( 1, level/10, 3 );
	       specdam->chance = URANGE ( 10, 10+2*level, 70 );

	       if ( IS_IMMORTAL( ch ) || ( value > 6 && number_percent() < 30 ) )
		  {
		     switch ( number_range( 1, 3 * LEVEL_HERO - level ) )
			{
			case 1:
			   SET_BIT( obj->value[ 4 ], WEAPON_FLAMING );
			   break;
			case 2:
			   SET_BIT( obj->value[ 4 ], WEAPON_FROST );
			   break;
			case 3:
			   SET_BIT( obj->value[ 4 ], WEAPON_SHOCKING );
			   break;
			case 4:
			   SET_BIT( obj->value[ 4 ], WEAPON_POISON );
			   break;
			case 5:
			   SET_BIT( obj->value[ 4 ], WEAPON_TOXIC );
			   break;
			case 6:
			   SET_BIT( obj->value[ 4 ], WEAPON_SACRED );
			   break;
			case 7:
			   SET_BIT( obj->value[ 4 ], WEAPON_RESONANT );
			   break;
			case 8:
			   add_new_affect( obj, 0, APPLY_SKILL, gsn_mace, number_range( 1, level/3 ), &AFF_NONE, 0, TRUE );
			   break;
			default:
			   break;
			}
		  }
	    }
      }

   /*
    * logowanie jaka bron zostala stworzona, ale tylko gracze
    */
   if ( !IS_NPC ( ch ) )
      {
	 sprintf
            (
             log_buf,
             "[%d] spell_spirit_hammer: %s (%d), skill: %d, chance: %d",
             ch->in_room ? ch->in_room->vnum : 0,
             IS_NPC( ch )? ch->short_descr : ch->name,
             ch->level,
             skill,
             value
	     );
	 log_string( log_buf );
      }

   obj_to_char( obj, ch );
   equip_char( ch, obj, WEAR_WIELD, TRUE );

   act( "W d³oni $z pojawia siê widmowy m³ot.", ch, NULL, NULL, TO_ROOM );
   send_to_char( "W twojej d³oni pojawia siê widmowy m³ot.\n\r", ch );

   // - Tener -
   // UWAGA: Nie nale¿y zwalniaæ zaalokowanego specdama, chyba ¿e siê go nie wykorzysta³o!
   //        Zakomentowujê zamiast usun±æ, by ostrze¿enie zosta³o dla potomno¶ci
   //
   // free_spec_damage( specdam );

   return;
}

OBJ_DATA * spiritual_armor_obj_helper( int vnum, int timer, int level, int pierce, int bash, int slash, int exotic )
{
    OBJ_DATA * obj = create_object( get_obj_index( vnum ), FALSE );
    obj->timer      = timer;
    obj->cost       = 0;
    obj->rent_cost  = 2 * level;
    obj->value[ 0 ] = pierce;
    obj->value[ 1 ] = bash;
    obj->value[ 2 ] = slash;
    obj->value[ 3 ] = exotic;
    obj->value[ 4 ] = 0;
    obj->value[ 5 ] = 0;
    obj->value[ 6 ] = 0;
    return obj;
}

void spell_spiritual_armor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA * obj = NULL;
    int timer, pierce, bash, slash, exotic;
    extern int wear_to_itemwear[ MAX_WEAR ];
    int luck = get_curr_stat( ch, STAT_LUC );

    timer = 5 + level / 2;
    if ( number_range( 0, luck ) > 90 )
    {
        timer *= 2;
    }
    if ( number_range( 0, luck ) < 30 )
    {
        timer /= 2;
    }

    pierce = 4 - ( level / 7.5 );
    if ( number_range( 0, luck ) > 90 ) pierce -= 1;
    if ( number_range( 0, luck ) < 30 ) pierce += 2;

    slash = 4 - ( level / 7.5 );
    if ( number_range( 0, luck ) > 90 ) slash -= 1;
    if ( number_range( 0, luck ) < 30 ) slash += 2;

    bash = 4 - ( level / 7.5 );
    if ( number_range( 0, luck ) > 90 ) bash -= 1;
    if ( number_range( 0, luck ) < 30 ) bash += 2;

    exotic = 5 - ( level / 10 );
    if ( number_range( 0, luck ) > 90 ) exotic -= 1;
    if ( number_range( 0, luck ) < 30 ) exotic += 2;

    pierce = URANGE( -15, pierce, 10 );
    slash  = URANGE( -15, slash,  10 );
    bash   = URANGE( -15, bash,   10 );
    exotic = URANGE( -15, exotic, 10 );

    if ( !get_eq_char( ch, WEAR_HEAD ) && IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, wear_to_itemwear[ WEAR_HEAD ] ) )
    {
        obj = spiritual_armor_obj_helper( OBJ_VNUM_SPIRITUAL_ARMOR_HEAD, timer, level, pierce, bash, slash, exotic );
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_HEAD, TRUE );
        send_to_char( "Na twojej g³owie pojawia siê widmowy he³m.\n\r", ch );
        act( "Na g³owie $z pojawia siê widmowy he³m.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !get_eq_char( ch, WEAR_BODY ) && IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, wear_to_itemwear[ WEAR_BODY ] ) )
    {
        obj = spiritual_armor_obj_helper( OBJ_VNUM_SPIRITUAL_ARMOR_BODY, timer, level, pierce, bash, slash, exotic );
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_BODY, TRUE );
        send_to_char( "Twoje cia³o okrywa widmowa zbroja.\n\r", ch );
        act( "Cia³o $z okrywa widmowa zbroja.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !get_eq_char( ch, WEAR_LEGS ) && IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, wear_to_itemwear[ WEAR_LEGS ] ) )
    {
        obj = spiritual_armor_obj_helper( OBJ_VNUM_SPIRITUAL_ARMOR_LEGS, timer, level, pierce, bash, slash, exotic );
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_LEGS, TRUE );
        send_to_char( "Twoje nogi chroni± teraz widmowe spodnie.\n\r", ch );
        act( "Na nogach $z tworz± siê widmowe spodnie.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !get_eq_char( ch, WEAR_ARMS ) && IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, wear_to_itemwear[ WEAR_ARMS ] ) )
    {
        obj = spiritual_armor_obj_helper( OBJ_VNUM_SPIRITUAL_ARMOR_ARMS, timer, level, pierce, bash, slash, exotic );
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_ARMS, TRUE );
        send_to_char( "Widmowe naramienniki chroni± twoje ramiona.\n\r", ch );
        act( "Na ramionach $z pojawiaja siê widmowe naramienniki.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !get_eq_char( ch, WEAR_HANDS ) && IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, wear_to_itemwear[ WEAR_HANDS ] ) )
    {
        obj = spiritual_armor_obj_helper( OBJ_VNUM_SPIRITUAL_ARMOR_HANDS, timer, level, pierce, bash, slash, exotic );
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_HANDS, TRUE );
        send_to_char( "Na twoich d³oniach pojawiaj± siê widmowe rêkawice.\n\r", ch );
        act( "Na d³oniach $z pojawiaj± siê widmowe rêkawice.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !get_eq_char( ch, WEAR_FEET ) && IS_SET( race_table[ GET_RACE( ch ) ].wear_flag, wear_to_itemwear[ WEAR_FEET ] ) )
    {
        obj = spiritual_armor_obj_helper( OBJ_VNUM_SPIRITUAL_ARMOR_FEET, timer, level, pierce, bash, slash, exotic );
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_FEET, TRUE );
        send_to_char( "Czujesz, ¿e stopy chronia ci widmowe buty.\n\r", ch );
        act( "Widzisz jak widmowe buty pojawiaj± siê na nogach $z.", ch, NULL, NULL, TO_ROOM );
    }

    if ( !obj )
    {
        send_to_char( "W³a¶ciwie to nic siê nie dzieje.\n\r", ch );
    }
    return;
}

/* Leczenie slepoty (skladnia: cast 'cure blindness' <kto>)
 *
 * no to jest standartowy cure blindness
 *
 */
void spell_cure_blindness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;

	if ( !IS_AFFECTED( victim, AFF_BLIND ) )
	{
		if ( victim == ch )
			send_to_char( "Nie jeste¶ o¶lepion<&y/a/e>.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie wygl±da na ¶lepe.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N nie wygl±da na ¶lep±.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "$N nie wygl±da na ¶lepego.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	affect_strip( victim, gsn_blindness );
	affect_strip( victim, gsn_wind_charger );
	affect_strip( victim, gsn_power_word_blindness );
	affect_strip( victim, gsn_pyrotechnics );
	affect_strip( victim, gsn_sunscorch );
	EXT_REMOVE_BIT( victim->affected_by, AFF_BLIND );

	act( "$n odzyskuje wzrok.", victim, NULL, NULL, TO_ROOM );
	send_to_char( "Odzyskujesz wzrok.\n\r", victim );

	return;
}

/* TU IDA WSZYSTKIE HOLDY
 * rozroznianie czy animal/monster/... po wpisie ->type w race_type
 */

/* Unieruchomienie osoby (skladnia: cast 'hold person' <kto>)
 */
void spell_hold_person( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat( ch, STAT_LUC );
	int vluck = get_curr_stat( victim, STAT_LUC );

	/*sprawdzanie bez powiadomienia bledu*/
	if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , PERSON )
	     || IS_AFFECTED( victim, AFF_FREE_ACTION ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PARALYZE ) ||
	     saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( number_range( 0, vluck - luck ) > 30 )
	{
		send_to_char( "Twoja ofiara mia³a naprawdê du¿o szczê¶cia i nie uda³o ci siê.\n\r", ch );
		return;
	}

    // stripujemy invisa je¿eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }

	duration = 1 + ( level / 10 );

	if ( number_range( 0, luck ) > 90 ) ++duration;
	else if ( number_range( 0, vluck ) < 18 ) ++duration;

	if ( number_range( 0, luck ) < 30 ) --duration;
	if ( number_range( 0, vluck ) > 90 ) --duration;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			if ( level > 24 )
				++duration;
			if ( number_range( 1, 2 ) == 1 )
				++duration;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier	= 0;
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
/* Unieruchomienie z³ego (skladnia: cast 'hold evil' <kto>)
 */
void spell_hold_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af, *paf;
	int duration;
	int luck = get_curr_stat( ch, STAT_LUC );
	int vluck = get_curr_stat( victim, STAT_LUC );
	int holy_dur_mod = 0;
	bool hold_self = FALSE, bonus_conditions = TRUE;

	/*sprawdzanie z powiadomieniem o b³êdzie*/
	if ( !IS_EVIL( victim ) )
	{
		send_to_char( "Ten czar jest skuteczny tylko przeciw Z³u.\n\r", ch );
		return;
	}


	/*sprawdzanie bez powiadomienia bledu*/
	if ( IS_AFFECTED( victim, AFF_FREE_ACTION ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PARALYZE ) ||
	     saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck/6 + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( number_range( 0, vluck - luck ) > 30 )
	{
		send_to_char( "Twoja ofiara mia³a naprawdê du¿o szczê¶cia i nie uda³o ci siê.\n\r", ch );
		return;
	}

    // stripujemy invisa je¿eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }

    if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
        if ( paf->modifier > 36 )
        {
            //niemodlacy sie nie maja pozytywnych bonusow
            bonus_conditions = FALSE;
        }

    }
    //efekty pozytywne przy holy prayerze
    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL)
    {
      if( bonus_conditions == TRUE)
      {
         if(paf->modifier > 20 && number_percent() < 30)
         {
            holy_dur_mod += 2;
            paf->modifier -= 3;
         }

         if(paf->modifier > 50 && number_percent() < 30)
         {
            holy_dur_mod += 2;
            paf->modifier -= 4;
         }

         if(paf->modifier > 70 && number_percent() < 30)
         {
            holy_dur_mod += 2;
            paf->modifier -= 5;
         }
      }
        if(paf->modifier < -70)
         {
            hold_self = TRUE;
         }
        if(paf->modifier < -50 && paf->modifier > -70)
         {
            holy_dur_mod -=1;
         }
        if(paf->modifier < -30 && paf->modifier > -70)
         {
            holy_dur_mod -=1;
         }

    }


	duration = 1 + ( level / 10 );
    duration += holy_dur_mod;
	if ( number_range( 0, luck ) > 90 ) ++duration;
	else if ( number_range( 0, vluck ) < 18 ) ++duration;

	if ( number_range( 0, luck ) < 30 ) --duration;
	if ( number_range( 0, vluck ) > 90 ) --duration;

    duration = URANGE(1, duration, 10);

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier	= 0;
	af.bitvector = &AFF_PARALYZE;
	if(hold_self == TRUE)
	{
       affect_to_char( ch, &af, NULL, TRUE );

       	//kasowanie prewaitów
        	if ( ch->wait_char )
         	{
          		free_pwait_char( ch->wait_char );
            	ch->wait_char = NULL;
            	ch->wait = 2;
          	}
            send_to_char( "Wymawiaj±c s³owa zaklêcia czujesz jak mrok rozlewa siê po twoim ciele, a czar obraca sw± moc przeciw tobie!\n\r", victim );
            act( "Widzisz jak $C otacza przez chwile mroczna aura.", ch, NULL, NULL, TO_ROOM );
           	switch ( ch->sex )
           	{
           		case 0:
               	send_to_char( "Zosta³o¶ unieruchomione!\n\r", ch );
		        break;
		        case 2:
			    send_to_char( "Zosta³a¶ unieruchomiona!\n\r", ch );
			    break;
		        default:
                send_to_char( "Zosta³e¶ unieruchomiony!\n\r", ch );
			    break;
            }
            act( "$n zatrzymuje siê w miejscu.", ch, NULL, NULL, TO_ROOM );
            return;


    }
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
    if(holy_dur_mod > 0)
    {
      send_to_char( "Czujesz jak moc twojego zaklêcia ro¶nie!\n\r", ch );
      act( "Widzisz jak $c otacza przez chwilê jasna aura.", ch, NULL, NULL, TO_ROOM );
    }
    if(holy_dur_mod < 0)
    {
      send_to_char( "Czujesz jak moc twojego zaklêcia s³abnie!\n\r", ch );
      act( "Widzisz jak $c otacza przez chwilê ciemna aura.", ch, NULL, NULL, TO_ROOM );
    }
	act( "$n zatrzymuje siê w miejscu.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_hold_monster( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int duration;
    int luck  = 0;
    int vluck = 0;

    /*sprawdzanie bez powiadomienia bledu*/
    if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , MONSTER ) || IS_AFFECTED( victim, AFF_FREE_ACTION ) )
    {
        send_to_char( "Nic siê nie dzieje.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED( victim, AFF_PARALYZE ) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
    {
        send_to_char( "Nic siê nie dzieje.\n\r", ch );
        return;
    }

    luck  = get_curr_stat( ch, STAT_LUC );
    if ( number_range( 0, luck + LUCK_BASE_MOD ) < 5 )
    {
        send_to_char( "Nie uda³o ci siê.\n\r", ch );
        return;
    }

    vluck = get_curr_stat( victim, STAT_LUC );
    if ( vluck - 30 > luck && number_range( 0, vluck - luck ) > 30 )
    {
        send_to_char( "Twoja ofiara mia³a naprawdê du¿o szczê¶cia i nie uda³o ci siê.\n\r", ch );
        return;
    }
    // stripujemy invisa je¿eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }
    duration = 1 + ( level / 10 );
    if ( number_range( 0, luck ) > 90 )
    {
        ++duration;
    }
    else if ( number_range( 0, vluck ) < 18 )
    {
        ++duration;
    }
    if ( number_range( 0, luck ) < 30 )
    {
        --duration;
    }
    if ( number_range( 0, vluck ) > 90 )
    {
        --duration;
    }
    /* Bonus dla specjalisty */
    if ( !IS_NPC( ch ) )
    {
        if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
        {
            if ( level > 24 )
            {
                ++duration;
            }
            if ( number_range( 1, 2 ) == 1 )
            {
                ++duration;
            }
        }
    }

    af.bitvector   = &AFF_PARALYZE;
    af.duration    = UMAX( 1, duration );
    af.level       = level;
    af.location    = APPLY_NONE;
    af.modifier    = 0;
    af.rt_duration = 0;
    af.type        = sn;
    af.where       = TO_AFFECTS;

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
        case SEX_NEUTRAL:
            send_to_char( "Zosta³o¶ unieruchomione!\n\r", victim );
            break;
        case SEX_FEMALE:
            send_to_char( "Zosta³a¶ unieruchomiona!\n\r", victim );
            break;
        case SEX_MALE:
        default:
            send_to_char( "Zosta³e¶ unieruchomiony!\n\r", victim );
            break;
    }

    act( "$n zatrzymuje siê w miejscu.", victim, NULL, NULL, TO_ROOM );
    return;
}

void spell_hold_undead( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	/*sprawdzanie bez powiadomienia bledu*/
	if ( IS_AFFECTED( victim, AFF_FREE_ACTION ) || !is_undead( victim ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PARALYZE ) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( number_range( 0, vluck - luck ) > 5 )
	{
		send_to_char( "Twoja ofiara mia³a naprawdê du¿o szczê¶cia i nie uda³o ci siê.\n\r", ch );
		return;
	}

    // stripujemy invisa je¿eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }

	duration = 1 + ( level / 10 );

	if ( number_range( 0, luck ) > 15 ) ++duration;
	else if ( number_range( 0, vluck ) < 3 ) ++duration;

	if ( number_range( 0, luck ) < 5 ) --duration;
	if ( number_range( 0, vluck ) > 15 ) --duration;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			if ( level > 24 )
				++duration;
			if ( number_range( 1, 2 ) == 1 )
				++duration;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier	= 0;
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

void spell_hold_animal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	/*sprawdzanie bez powiadomienia bledu*/
	if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , ANIMAL )
	     || IS_AFFECTED( victim, AFF_FREE_ACTION ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PARALYZE ) ||
	     saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( number_range( 0, vluck - luck ) > 5 )
	{
		send_to_char( "Twoja ofiara mia³a naprawdê du¿o szczê¶cia i nie uda³o ci siê.\n\r", ch );
		return;
	}

    // stripujemy invisa je¿eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }

	duration = 1 + ( level / 10 );

	if ( number_range( 0, luck ) > 15 ) ++duration;
	else if ( number_range( 0, vluck ) < 3 ) ++duration;

	if ( number_range( 0, luck ) < 5 ) --duration;
	if ( number_range( 0, vluck ) > 15 ) --duration;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			if ( level > 24 )
				++duration;
			if ( number_range( 1, 2 ) == 1 )
				++duration;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier	= 0;
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

void spell_hold_plant( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	/*sprawdzanie bez powiadomienia bledu*/
	if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , PLANT )
	     || IS_AFFECTED( victim, AFF_FREE_ACTION ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PARALYZE ) ||
	     saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		send_to_char( "Nic siê nie dzieje.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

    // stripujemy invisa je¿eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }

	duration = 1 + ( level / 10 );

	if ( number_range( 0, luck ) > 15 ) ++duration;
	else if ( number_range( 0, vluck ) < 3 ) ++duration;

	if ( number_range( 0, luck ) < 5 ) --duration;
	if ( number_range( 0, vluck ) > 15 ) --duration;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			if ( level > 24 )
				++duration;
			if ( number_range( 1, 2 ) == 1 )
				++duration;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier	= 0;
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

/* metamorfoza laski w bron (skladnia: cast 'transmute staff')

 *
 * czarek daje lasce affecta do HR i DR reszte trzeba dac w fight.c
 * sprawdza czy ma bron, albo laske, ktora ma affecta, jesli tak, to wali jak
 * z broni z obrazeniami standartowego miecza (ale magicznymi!)
 * jak ju¿ robisz walke, to zostawiam to Tobie *grin*
 */
void spell_transmute_staff( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj;
	AFFECT_DATA af;
	int duration, mod;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int wis_mod = URANGE( -2, ( get_curr_stat_deprecated( ch, STAT_WIS ) / 2 ) - 9, 3 );

	if ( target != TARGET_OBJ )
	{
		send_to_char( "Umiesz przemieniaæ tylko laski w broñ.\n\r", ch );
		return;
	}

	obj = ( OBJ_DATA * ) vo;

	if ( obj->item_type != ITEM_WEAPON || obj->value[ 0 ] != WEAPON_STAFF )
	{
		send_to_char( "Umiesz przemieniaæ tylko laski w broñ.\n\r", ch );
		return;
	}

	if ( IS_OBJ_STAT( obj, ITEM_TRANSMUTE ) )
	{
		act( "$p jest ju¿ przemieniona.", ch, obj, NULL, TO_CHAR );
		return;
	}

	if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) )
	{
		act( "Ta broñ jest magiczna.", ch, obj, NULL, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	duration = ( ( level / 2 ) * ( 100 + luck ) ) / 100;
	duration += wis_mod;
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 ) duration /= 2;

	mod = URANGE( 1, level / 10, 3 );
	if ( number_range( 0, luck ) > 10 ) ++mod;
	if ( number_range( -2, wis_mod ) > 0 ) ++mod;
	if ( dice( 2, luck ) < 9 ) --mod;
	if ( dice( UMAX( 1, wis_mod ), 10 ) < 5 ) --mod;

	af.where = TO_OBJECT;
	af.type = sn;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.bitvector = &ITEM_TRANSMUTE;
	af.location = APPLY_HITROLL;
	af.modifier = mod;
	affect_to_obj( obj, &af );
	af.location = APPLY_DAMROLL;
	af.modifier = mod;
	if ( level > 14 )
		af.bitvector = &ITEM_MAGIC;
	affect_to_obj( obj, &af );

	act( "$p p³onie jasnym ¶wiat³em.", ch, obj, NULL, TO_ALL );
	return;
}

/* Rozkaz (skladnia: cast command <kto> <co_ma_zrobic>)
 *
 * no wiec ja to bym widzial dokladnie tak, jak mob force <co>, ale dziala na
 * flee, rest, sleep, stand... bo mialo byc na 1 wyraz. tylko trza sprzwdzac
 * bo beda przekrety np. c command <kto> quit i bierzemy eq
 * jest zrobione tak, ze bardziej inteligentnych do niczego nie zmusimy
 */

void spell_command( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * vch;
	char vict_name[ MAX_INPUT_LENGTH ];
	char command[ MAX_INPUT_LENGTH ];
	char *tar = ( char * ) vo;
	bool ok = FALSE;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = 0;
	int cmd;

	if ( !tar || tar[ 0 ] == '\0' )
	{
		send_to_char( "Komu chcesz co¶ rozkazaæ?\n\r", ch );
		return;
	}

	tar = one_argument( tar, vict_name );

	vch = get_char_room( ch, vict_name );

	if ( !vch )
	{
		send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
		return;
	}

	if ( !tar || !tar[ 0 ] )
	{
		send_to_char( "Co chcesz rozkazaæ?\n\r", ch );
		return;
	}

	if ( ch == vch )
	{
		send_to_char( "Nie uwa¿asz, ¿e prze³amywanie w³asnej woli to lekki bezsens?\n\r", ch );
		return;
	}

	if ( ch->mount && vch->mounting == ch )
	{
		send_to_char( "Najpierw zsi±d¼ z wierzchowca.\n\r", ch );
		return;
	}

	/* no to trzeba jakis wzorek wymyslec, coby siê nie zawsze wyszlo
	 * trza by to jakos od inta uzaleznic i troche losowosci dodac
	 */
	if ( is_safe( ch, vch, TRUE ) )
	{
		send_to_char( "Nic siê nie sta³o.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( vch, AFF_MINOR_GLOBE ) || IS_AFFECTED( vch, AFF_GLOBE ) || IS_AFFECTED( vch, AFF_MAJOR_GLOBE ) )
	{
		act( "Twoje zaklêcie znika przy zetkniêciu ze sfer± otaczaj±c± $C.", ch, NULL, vch, TO_CHAR );
		act( "Zaklêcie $z znika przy zetkniêciu z otaczaj±c± ciê sfer±.\n\r", ch, NULL, vch, TO_VICT );
		act( "Zaklêcie $z znika przy zetkniêciu z otaczaj±c± $C sfer±.", ch, NULL, vch, TO_NOTVICT );
		return;
	}

	if ( IS_AFFECTED( vch, AFF_REFLECT_SPELL ) )
	{
		print_char( ch, "Twoje zaklêcie odbija siê od ochronnej tarczy %s.\n\r", vch->name2 );
		print_char( vch, "Zaklêcie %s odbija siê od ochronnej tarczy.\n\r", ch->name2 );
		act( "Zaklêcie $z odbija siê od ochronnej tarczy $Z.", ch, NULL, vch, TO_NOTVICT );
		return;
	}

	if ( IS_AFFECTED( vch, AFF_PARALYZE ) )
	{
		act( "$N nie jest w stanie wykonaæ twojego polecenia.", ch, NULL, vch, TO_CHAR );
		return;
	}

	if ( saves_spell_new( vch, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		if ( IS_NPC( vch ) )
		{
			act( "$N nie zwraca na uwagi na to co robisz.", ch, NULL, vch, TO_CHAR );
		}
		else
		{
			act( "$N ¶mieje siê z ciebie.", ch, NULL, vch, TO_CHAR );
			act( "$n próbowa³ wydaæ ci rozkaz, jednak ty siê z tego ¶miejesz.", ch, NULL, vch, TO_VICT );
		}
		if ( vch->fighting == NULL && can_see( vch, ch ) && can_move( vch ) )
		{
			if ( vch->position == POS_SITTING || vch->position == POS_RESTING )
			{
				vch->position = POS_STANDING;
				act( "Wstajesz.", vch, NULL, NULL, TO_CHAR );
				act( "$n wstaje.", vch, NULL, NULL, TO_ROOM );
			}

			if ( IS_NPC( vch ) && HAS_TRIGGER( vch, TRIG_KILL ) )
				mp_percent_trigger( vch, ch, NULL, NULL, &TRIG_KILL );

			multi_hit( vch, ch, TYPE_UNDEFINED );
		}
		return;
	}

	if ( vch->resists[ RESIST_MENTAL ] > 0 && number_percent() < vch->resists[ RESIST_MENTAL ] )
		return;

	vluck = get_curr_stat_deprecated( vch, STAT_LUC );

	if ( number_range( 0, luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}
	if ( number_range( 0, vluck - luck ) > 5 )
	{
		send_to_char( "Twoja ofiara mia³a naprawdê du¿o szczê¶cia i nie uda³o ci siê.\n\r", ch );
		return;
	}

	/* zawsze mozna cos dopisac */
	if ( EXT_IS_SET( ch->act, PLR_COMMANDSPL ) )
	{
		if ( !str_prefix( tar, "uciekaj" ) ||
		     !str_prefix( tar, "spij" ) ||
		     !str_prefix( tar, "usiadz" ) ||
		     !str_prefix( tar, "wstan" ) ||
		     !str_prefix( tar, "odpocznij" ) ||
		     !str_prefix( tar, "gora" ) ||
		     !str_prefix( tar, "dol" ) ||
		     !str_prefix( tar, "polnoc" ) ||
		     !str_prefix( tar, "wschod" ) ||
		     !str_prefix( tar, "poludnie" ) ||
		     !str_prefix( tar, "zachod" ) ||
		     !str_prefix( tar, "kopnij" ) ||
		     !str_prefix( tar, "powal" ) )
			ok = TRUE;
	}
	else
	{
		if ( !str_prefix( tar, "flee" ) ||
		     !str_prefix( tar, "sleep" ) ||
		     !str_prefix( tar, "sit" ) ||
		     !str_prefix( tar, "stand" ) ||
		     !str_prefix( tar, "rest" ) ||
		     !str_prefix( tar, "up" ) ||
		     !str_prefix( tar, "down" ) ||
		     !str_prefix( tar, "north" ) ||
		     !str_prefix( tar, "east" ) ||
		     !str_prefix( tar, "south" ) ||
		     !str_prefix( tar, "west" ) ||
		     !str_prefix( tar, "kick" ) ||
		     !str_prefix( tar, "bash" ) )
			ok = TRUE;
	}

    if ( ok )
    {
        switch ( vch->sex )
        {
            case 0 :
                act( "Uleg³o¶ silnej woli $z", ch, NULL, vch, TO_VICT );
                break;
            case 2 :
                act( "Uleg³a¶ silnej woli $z", ch, NULL, vch, TO_VICT );
                break;
            default :
                act( "Uleg³e¶ silnej woli $z", ch, NULL, vch, TO_VICT );
                break;
        }
        switch ( ch->sex )
        {
            case 0 :
                act( "Prze³ama³e¶ woln± wolê $Z.", ch, NULL, vch, TO_CHAR );
                break;
            case 2 :
                act( "Prze³ama³a¶ woln± wolê $Z.", ch, NULL, vch, TO_CHAR );
                break;
            default :
                act( "Prze³ama³e¶ woln± wolê $Z.", ch, NULL, vch, TO_CHAR );
                break;
        }

        if ( EXT_IS_SET( ch->act, PLR_COMMANDSPL ) )
        {
            for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
            {
                if ( NOPOL( tar[ 0 ] ) == NOPOL( cmd_table[ cmd ].name_pl[ 0 ] )
                        && !str_prefix( tar, cmd_table[ cmd ].name_pl ) )
                {
                    sprintf( command, "%s", cmd_table[ cmd ].name );
                    interpret( vch, command );
                    break;
                }
            }
        }
        else
        {
            interpret( vch, tar );
        }
    }
    else
    {
        send_to_char( "To jest zbyt skomplikowane.\n\r", ch );
        return;
    }

    return;
}

/* Regeneracja konczyc (skladnia: cast regenerate <kto> <co>)
 *
 * w sumie ja to widze tak, ze mozna regenerowac w zaleznosci od parametru
 * nie wiem, co ma odpadac, a co nie, wiec zostawiam puste
 *
 */
void spell_regenerate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), dur, lev;

	if ( IS_AFFECTED( victim, AFF_REGENERATION ) )
	{
		if ( ch == victim )
			send_to_char( "Przecie¿ ju¿ jeste¶ pod dzia³eniem tego czaru.\n\r", victim );
		else
			act( "$N jest ju¿ pod dzia³aniem tego czaru.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	dur = 2;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_INT ) ) > 15 )
		++dur;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 )
		++dur;
	if ( number_range( 0, level ) > 20 )
		++dur;

	lev = 2 + level / 7.5;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = lev;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_REGENERATION;

	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Odczuwasz przyjemne mrowienie na ca³ym ciele.\n\r", victim );

	if ( ch == victim )
		return;

	act( "Widzisz jak $N dr¿y na ca³ym ciele.", ch, NULL, victim, TO_CHAR );
	return;
}

/* Niewidzialny dla martiwakow (skladnia: cast 'undead invis' [co])
 */
void spell_undead_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration = 2 + level / 4;
	if ( number_range( 0, luck ) > 10 ) duration += 2;
	if ( number_range( 0, luck ) < 3 ) duration -= 2;

	/* object invi sibility - niech bedzie i dla rzeczy */
	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;

		if ( IS_OBJ_STAT( obj, ITEM_UNDEAD_INVIS ) )
		{
			act( "Nieumarli nie widz± ju¿ $f.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( number_range( 0, luck ) < 1 )
		{
			act( "Nie uda³o ci siê ukryæ $f przed nieumar³ymi.", ch, obj, NULL, TO_CHAR );
			return;
		}

		af.where = TO_OBJECT;
		af.type = sn;
		af.level = level;
		af.duration = duration; af.rt_duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &ITEM_UNDEAD_INVIS;
		affect_to_obj( obj, &af );

		act( "$p znika przed wzrokiem nieumar³ych.", ch, obj, NULL, TO_ALL );
		return;
	}

	/* character invisibility */
	victim = ( CHAR_DATA * ) vo;

	if ( IS_AFFECTED( victim, AFF_UNDEAD_INVIS ) )
	{
		if ( ch == victim )
			send_to_char( "Nieumarli ju¿ ciebie nie widz±.\n\r", ch );
		else
			act( "Nieumarli nie widz± ju¿ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck ) < 1 )
	{
		if ( ch == victim )
			send_to_char( "Nie uda³o ci siê ukryæ przed nieumar³ymi.\n\r", ch );
		else
			act( "Nie uda³o ci siê ukryæ $Z przed nieumar³ymi.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( ch != victim )
	{
		luck = get_curr_stat_deprecated( victim, STAT_LUC );
		if ( number_range( 0, luck ) > 10 ) duration += 2;
		if ( number_range( 0, luck ) < 3 ) duration -= 2;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_UNDEAD_INVIS;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			act( "$n staje siê niewidzialne dla nieumar³ych.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Stajesz siê niewidzialne dla martwiaków.\n\r", victim );
			break;
		case 2:
			act( "$n staje siê niewidzialna dla nieumar³ych.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Stajesz siê niewidzialna dla martwiaków.\n\r", victim );
			break;
		default:
			act( "$n staje siê niewidzialny dla nieumar³ych.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Stajesz siê niewidzialny dla martwiaków.\n\r", victim );
			break;
	}

	return;
}

/* Niewidzialny dla zwierzat (skladnia: cast 'animal invis' [co])
 */

void spell_animal_invis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration = 2 + level / 4;
	if ( number_range( 0, luck ) > 10 ) duration += 2;
	if ( number_range( 0, luck ) < 5 ) duration -= 2;

	/* object invi sibility - niech bedzie i dla rzeczy */
	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;

		if ( IS_OBJ_STAT( obj, ITEM_ANIMAL_INVIS ) )
		{
			act( "Zwierzêta nie widz± ju¿ $f.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( number_range( 0, luck ) < 1 )
		{
			act( "Nie uda³o ci siê ukryæ $f przed zwierzêtami.", ch, obj, NULL, TO_CHAR );
			return;
		}

		af.where	= TO_OBJECT;
		af.type	= sn;
		af.level	= level;
		af.duration = duration; af.rt_duration = 0;
		af.location	= APPLY_NONE;
		af.modifier	= 0;
		af.bitvector	= &ITEM_ANIMAL_INVIS;
		affect_to_obj( obj, &af );

		act( "$p znika przed wzrokiem zwierz±t.", ch, obj, NULL, TO_ALL );
		return;
	}

	/* character invisibility */
	victim = ( CHAR_DATA * ) vo;

	if ( IS_AFFECTED( victim, AFF_ANIMAL_INVIS ) )
	{
		act( "Zwierzêta nie widz± ju¿ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck ) < 1 )
	{
		if ( ch != victim )
			act( "Nie uda³o ci siê ukryæ $Z przed wzrokiem zwierz±t.", ch, NULL, victim, TO_CHAR );
		else
			act( "Nie uda³o ci siê ukryæ przed wzrokiem zwierz±t.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( ch != victim )
	{
		luck = get_curr_stat_deprecated( victim, STAT_LUC );
		if ( number_range( 0, luck ) > 10 ) duration += 2;
		if ( number_range( 0, luck ) < 5 ) duration -= 2;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_ANIMAL_INVIS;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			act( "$n staje siê niewidzialne dla zwierz±t.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Stajesz siê niewidzialne dla zwierz±t.\n\r", victim );
			break;
		case 1:
			act( "$n staje siê niewidzialny dla zwierz±t.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Stajesz siê niewidzialny dla zwierz±t.\n\r", victim );
			break;
		default:
			act( "$n staje siê niewidzialna dla zwierz±t.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Stajesz siê niewidzialna dla zwierz±t.\n\r", victim );
			break;
	}

	return;
}

/* Ochrona przed zlem (skladnia: cast 'protection evil' [kto])
 *
 * tutaj nie bedzie nic dawac, bo ma dzialac tylko w walce ze zlymi
 * wiec nie widze sensu dawac teraz do affectow hr+2,ac-2 lepiej to
 * zrobic w czasie walki, sprawdzic alignment atakujacego i wtedy ew
 * doliczac ac/hr. troche bedzie tych zmian w fight.c
 */
void spell_protection_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_PROTECT_EVIL ) )
	{
		if ( victim == ch )
			send_to_char( "Jeste¶ ju¿ chronion<&y/a/e> przed z³em.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N jest ju¿ chronione przed z³em.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N jest ju¿ chroniona przed z³em.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N jest ju¿ chroniony przed z³em.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PROTECT_GOOD ) )
	{
		if ( victim == ch )
			send_to_char( "Jeste¶ ju¿ chronion<&y/a/e> przed dobrem.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N jest ju¿ chronione przed dobrem.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N jest ju¿ chroniona przed dobrem.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N jest ju¿ chroniony przed dobrem.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}

		return;
	}

	if ( IS_EVIL( victim ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo¿esz powiazaæ siê z dobrem!\n\r", ch );
		else
			act( "$N nie mo¿e wi±zaæ siê z dobrem!", ch, NULL, victim, TO_CHAR );

		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch != victim )
			act( "Nie uda³o ci siê ochroniæ $Z przed z³em.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda³o ci siê ochroniæ siebie przed z³em.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 20 + level / 3; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_PROTECT_EVIL;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz siê powi±zane z dobrem.\n\r", victim );
			break;
		case 2:
			send_to_char( "Czujesz siê powi±zana z dobrem.\n\r", victim );
			break;
		default :
			send_to_char( "Czujesz siê powi±zany z dobrem.\n\r", victim );
			break;
	}
	if ( ch != victim )
		switch ( victim->sex )
		{
			case 0:
				act( "$N jest chronione przed z³em.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N jest chroniona przed z³em.", ch, NULL, victim, TO_CHAR );
				break;
			default :
				act( "$N jest chroniony przed z³em.", ch, NULL, victim, TO_CHAR );
				break;
		}
	return;
}

/* Ochrona przed dobrem (skladnia: cast 'protection good' [kto])
 */
void spell_protection_good( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_PROTECT_EVIL ) )
	{
		if ( victim == ch )
			send_to_char( "Jeste¶ ju¿ chronion<&y/a/e> przed z³em.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N jest ju¿ chronione przed z³em.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N jest ju¿ chroniona przed z³em.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N jest ju¿ chroniony przed z³em.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( IS_AFFECTED( victim, AFF_PROTECT_GOOD ) )
	{
		if ( victim == ch )
			send_to_char( "Jeste¶ ju¿ chronion<&y/a/e> przed dobrem.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N jest ju¿ chronione przed dobrem.", ch, NULL, victim, TO_CHAR );
					break;
				case 2:
					act( "$N jest ju¿ chroniona przed dobrem.", ch, NULL, victim, TO_CHAR );
					break;
				default :
					act( "$N jest ju¿ chroniony przed dobrem.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	if ( IS_GOOD( victim ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo¿esz powi±zaæ siê ze zlem!\n\r", ch );
		else
			act( "$N nie mo¿e wi±zaæ siê ze z³em!", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( ch != victim )
			act( "Nie uda³o ci siê ochroniæ $Z przed dobrem.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda³o ci siê ochroniæ siebie przed dobrem.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 20 + level / 3; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_PROTECT_GOOD;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz siê powi±zane ze z³em.\n\r", victim );
			break;
		case 2:
			send_to_char( "Czujesz siê powi±zana ze z³em.\n\r", victim );
			break;
		default :
			send_to_char( "Czujesz siê powi±zany ze z³em.\n\r", victim );
			break;
	}
	if ( ch != victim )
		switch ( victim->sex )
		{
			case 0:
				act( "$N jest teraz chronione przed dobrem.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N jest teraz chroniona przed dobrem.", ch, NULL, victim, TO_CHAR );
				break;
			default :
				act( "$N jest teraz chroniony przed dobrem.", ch, NULL, victim, TO_CHAR );
				break;
		}
	return;
}

void spell_resist_fire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), mod, dur;

	if ( IS_AFFECTED( victim, AFF_RESIST_FIRE ) )
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
        {
            send_to_char( "Nie uda³o ci siê zwiêkszyæ swojej odporno¶ci na ogieñ.\n\r", ch );
        }
        else
        {
            act( "Nie uda³o ci siê zwiêkszyæ odporno¶ci $Z na ogieñ.", ch, NULL, victim, TO_CHAR );
        }
        return;
	}



	mod = 30+level/2;
	dur = 4 + level / 6;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur += 1 + level / 10;
			mod += level/2;
		}
	}

	    if( !spell_item_check( ch, sn , "fire based spell" ) )
    {
        send_to_char( "Brakuje ci agatu ognistego by zaklêcie osi±gne³o pe³nie mocy.\n\r", ch );
        mod = mod - 10;
    }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = mod;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_FIRE;
	af.bitvector = &AFF_RESIST_FIRE;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz, ¿e teraz jeste¶ bardziej odporne na ogieñ.\n\r", victim );
			act( "$n jest teraz bardziej odporne na ogieñ.", victim, NULL, NULL, TO_ROOM );
			break;
		case 2:
			send_to_char( "Czujesz, ¿e teraz jeste¶ bardziej odporna na ogieñ.\n\r", victim );
			act( "$n jest teraz bardziej odporna na ogieñ.", victim, NULL, NULL, TO_ROOM );
			break;
		default :
			send_to_char( "Czujesz, ¿e teraz jeste¶ bardziej odporny na ogieñ.\n\r", victim );
			act( "$n jest teraz bardziej odporny na ogieñ.", victim, NULL, NULL, TO_ROOM );
			break;
	}

	return;
}

/* Odpornosc na zimno (skladnia: cast 'resist cold' [kto])
 */

void spell_resist_cold( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), mod, dur;

	if ( IS_AFFECTED( victim, AFF_RESIST_COLD ) )
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
        {
			send_to_char( "Nie uda³o ci siê zwiêkszyæ swojej odporno¶ci na zimno.\n\r", ch );
        }
		else
        {
			act( "Nie uda³o ci siê zwiêkszyæ odporno¶ci $Z na zimno.", ch, NULL, victim, TO_CHAR );
        }
		return;
	}

	mod = 30+level/2;
	dur = 4 + level / 6;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur += 1 + level / 10;
			mod += level/2;
		}
	}

	    if( !spell_item_check( ch, sn , "cold based spell" ) )
    {
        send_to_char( "Brakuje ci srebrnego proszku by zaklêcie osi±gne³o pe³nie mocy.\n\r", ch );
        mod = mod - 10;
    }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = mod;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_COLD;
	af.bitvector = &AFF_RESIST_COLD;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz, ¿e teraz jeste¶ bardziej odporne na zimno.\n\r", victim );
			act( "$n jest teraz bardziej odporne na zimno.", victim, NULL, NULL, TO_ROOM );
			break;
		case 2:
			send_to_char( "Czujesz, ¿e teraz jeste¶ bardziej odporna na zimno.\n\r", victim );
			act( "$n jest teraz bardziej odporna na zimno.", victim, NULL, NULL, TO_ROOM );
			break;
		default :
			send_to_char( "Czujesz, ¿e teraz jeste¶ bardziej odporny na zimno.\n\r", victim );
			act( "$n jest teraz bardziej odporny na zimno.", victim, NULL, NULL, TO_ROOM );
			break;
	}

	return;
}

void spell_resist_lightning( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), mod, dur;

	if ( IS_AFFECTED( victim, AFF_RESIST_LIGHTNING ) )
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
		{
			send_to_char( "Nie uda³o ci siê zwiêkszyæ swojej odporno¶ci na elektryczno¶æ.\n\r", ch );
		}
		else
		{
			act( "Nie uda³o ci siê zwiêkszyæ odporno¶ci $Z na elektryczno¶æ.", ch, NULL, victim, TO_CHAR );
		}
		return;
	}

	mod = 30+level/2;
	dur = 4 + level / 6;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur += 1 + level / 10;
			mod += level/2;
		}
	}

	    if( !spell_item_check( ch, sn , "lighting based spell" ) )
    {
		send_to_char( "Brakuje ci kawaleczka bursztynu by zaklêcie osi±gne³o pe³nie mocy.\n\r", ch );
		mod = mod - 10;
    }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = mod;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_ELECTRICITY;
	af.bitvector = &AFF_RESIST_LIGHTNING;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz, ¿e teraz jeste¶ bardziej odporne na elektryczno¶æ.\n\r", victim );
			act( "$n jest teraz bardziej odporne na elektryczno¶æ.", victim, NULL, NULL, TO_ROOM );
			break;
		case 2:
			send_to_char( "Czujesz, ¿e teraz jeste¶ bardziej odporna na elektryczno¶æ.\n\r", victim );
			act( "$n jest teraz bardziej odporna na elektryczno¶æ.", victim, NULL, NULL, TO_ROOM );
			break;
		default :
			send_to_char( "Czujesz, ¿e teraz jeste¶ bardziej odporny na elektryczno¶æ.\n\r", victim );
			act( "$n jest teraz bardziej odporny na elektryczno¶æ.", victim, NULL, NULL, TO_ROOM );
			break;
	}

	return;
}

void spell_resist_acid( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), mod, dur;

	if ( IS_AFFECTED( victim, AFF_RESIST_ACID ) )
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
        {
			send_to_char( "Nie uda³o ci siê zwiêkszyæ swojej odporno¶ci na kwas.\n\r", ch );
        }
		else
        {
			act( "Nie uda³o ci siê zwiêkszyæ odporno¶ci $Z na kwas.", ch, NULL, victim, TO_CHAR );
        }
		return;
	}

	mod = 30+level/2;
	dur = 4 + level / 6;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			dur += 1 + level / 10;
			mod += level/2;
		}
	}

	    if( !spell_item_check( ch, sn , "acid based spell" ) )
    {
		send_to_char( "Brakuje ci kwasu by zaklêcie osi±gne³o pe³nie mocy.\n\r", ch );
		mod = mod - 10;
    }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = mod;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_ACID;
	af.bitvector = &AFF_RESIST_ACID;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz, ¿e teraz jeste¶ bardziej odporne na kwas.\n\r", victim );
			act( "$n jest teraz bardziej odporne na kwas.", victim, NULL, NULL, TO_ROOM );
			break;
		case 2:
			send_to_char( "Czujesz, ¿e teraz jeste¶ bardziej odporna na kwas.\n\r", victim );
			act( "$n jest teraz bardziej odporna na kwas.", victim, NULL, NULL, TO_ROOM );
			break;
		default :
			send_to_char( "Czujesz, ¿e teraz jeste¶ bardziej odporny na kwas.\n\r", victim );
			act( "$n jest teraz bardziej odporny na kwas.", victim, NULL, NULL, TO_ROOM );
			break;
	}

	return;
}

void spell_resist_magic( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_RESIST_FIRE ) ||
	     IS_AFFECTED( victim, AFF_RESIST_COLD ) ||
	     IS_AFFECTED( victim, AFF_RESIST_LIGHTNING ) ||
	     IS_AFFECTED( victim, AFF_RESIST_MAGIC ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo¿esz byæ bardziej chronion<&y/a/e>.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case SEX_NEUTRAL:
					act( "$N nie mo¿e byæ bardziej chronione.", ch, NULL, victim, TO_CHAR );
					break;
				case SEX_FEMALE:
					act( "$N nie mo¿e byæ bardziej chroniona.", ch, NULL, victim, TO_CHAR );
					break;
				case SEX_MALE:
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
        {
			send_to_char( "Nie uda³o ci siê zwiêkszyæ swojej odporno¶ci na magiê.\n\r", ch );
        }
		else
        {
			act( "Nie uda³o ci siê zwiêkszyæ odporno¶ci $Z na magiê.", ch, NULL, victim, TO_CHAR );
        }
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 3; af.rt_duration = 0;
	af.location = APPLY_SAVING_SPELL;
	af.modifier = -5;
	af.bitvector = &AFF_RESIST_MAGIC;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			act( "$n jest teraz lepiej chronione przed magi±.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Czujesz, ¿e teraz jeste¶ lepiej chronione przed magi±.\n\r", victim );
			break;
		case 2:
			act( "$n jest teraz lepiej chroniona przed magi±.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Czujesz, ¿e teraz jeste¶ lepiej chroniona przed magi±.\n\r", victim );
			break;
		default :
			act( "$n jest teraz lepiej chroniony przed magi±.", victim, NULL, NULL, TO_ROOM );
			send_to_char( "Czujesz, ¿e teraz jeste¶ lepiej chroniony przed magi±.\n\r", victim );
			break;
	}

	return;
}

void spell_brave_cloak( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA *vch;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( is_affected( victim, gsn_fear ) )
	{
		if ( check_dispel( level, victim, gsn_fear ) && number_range( 0, luck ) > 0 )
		{
			send_to_char( "Twój strach mija.\n\r", victim );
			act( "$n ju¿ siê nikogo nie boi.", victim, NULL, NULL, TO_ROOM );
		}
		else
		{
			act( "Nie uda³o ci siê uwolniæ $Z od strachu.", ch, NULL, victim, TO_CHAR );
			return;
		}
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if( ch == victim )
			act( "Nie uda³o ci siê otoczyæ siebie p³aszczem odwagi.", ch, NULL, victim, TO_CHAR );
		else
			act( "Nie uda³o ci siê otoczyæ $Z p³aszczem odwagi.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 4 + level / ( ( number_range( 0, luck ) > 10 ) ? 6 : 7.5 );
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 2;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = URANGE( 20, level * 3, 90 );
	af.location = APPLY_RESIST;
	af.modifier = RESIST_FEAR;
	af.bitvector = &AFF_BRAVE_CLOAK;

	if ( victim != ch )
	{
		luck = get_curr_stat_deprecated( victim, STAT_LUC );
		if ( number_range( 0, luck ) > 15 ) duration += 1;
		if ( number_range( 0, luck ) < 5 ) duration -= 2;
		af.duration = UMAX( 1, duration ); af.rt_duration = 0;

		affect_to_char( victim, &af, NULL, TRUE );

		act( "Otaczasz siê p³aszczem odwagi.", ch, NULL, victim, TO_VICT );
		act( "$n otacza siê p³aszczem odwagi.", victim, NULL, victim, TO_ROOM );
	}
	else
	{
		for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
			if ( !is_affected( vch, sn ) && is_same_group( vch, ch ) )
			{
				luck = get_curr_stat_deprecated( vch, STAT_LUC );
				if ( number_range( 0, luck ) > 15 ) duration += 1;
				if ( number_range( 0, luck ) < 5 ) duration -= 2;
				af.duration = UMAX( 1, duration ); af.rt_duration = 0;

				affect_to_char( vch, &af, NULL, TRUE );
				act( "Otaczasz siê p³aszczem odwagi.", vch, NULL, NULL, TO_CHAR );
				act( "$n otacza siê p³aszczem odwagi.", vch, NULL, vch, TO_ROOM );
			}
	}
	return;
}

void spell_remove_fear( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( !IS_AFFECTED( victim, AFF_FEAR ) )
	{
		if ( victim == ch ) send_to_char( "Przecie¿ niczego siê nie boisz.\n\r", ch );
		else
			if ( victim->sex == 0 )
			{
				act( "$N nie jest przera¿one.", ch, NULL, victim, TO_CHAR );
			}
			else if ( victim->sex == 1 )
			{
				act( "$N nie jest przera¿ony.", ch, NULL, victim, TO_CHAR );
			}
			else
			{
				act( "$N nie jest przera¿ona.", ch, NULL, victim, TO_CHAR );
			}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch ) send_to_char( "Nie uda³o ci siê przgnaæ strachu.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$n nadal jest przera¿one.", victim, NULL, NULL, TO_ROOM );
					break;
				case 1:
					act( "$n nadal jest przera¿ony.", victim, NULL, NULL, TO_ROOM );
					break;
				default:
					act( "$n nadal jest przera¿ona.", victim, NULL, NULL, TO_ROOM );
					break;
			}
		}
		return;
	}

	affect_strip( victim, gsn_fear );
	EXT_REMOVE_BIT( victim->affected_by, AFF_FEAR );

	send_to_char( "Twój strach mija.\n\r", victim );
	if ( victim->sex == 0 )
	{
		act( "$n ju¿ nie jest przera¿one.", victim, NULL, NULL, TO_ROOM );
	}
	else if ( victim->sex == 1 )
	{
		act( "$n ju¿ nie jest przera¿ony.", victim, NULL, NULL, TO_ROOM );
	}
	else
	{
		act( "$n ju¿ nie jest przera¿ona.", victim, NULL, NULL, TO_ROOM );
	}

	return;
}

/* Sanktuarium (skladnia: cast sanctuary [kto])
 * a moze by sancta zrobic jak w baldurze? nikt cie nie moze zaatakowac,
 * az sam nie podejmiesz jakiejs ofensywnej akcji, albo minie czas czaru?
 * na razie zostawiam samego affecta.
 */
void spell_sanctuary( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat( ch, STAT_LUC );
	int duration;

	if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
	{
		if ( victim == ch )
		{
			send_to_char( "Jeste¶ ju¿ otoczon<&y/a/e> bia³± aur±.\n\r", victim );
		}
		else
		{
			if ( victim->sex == 0 )
			{
				act( "$N jest ju¿ otoczony bia³± aur±.", ch, NULL, victim, TO_CHAR );
			}
			else if ( victim->sex == 1 )
			{
				act( "$N jest ju¿ otoczony bia³± aur±.", ch, NULL, victim, TO_CHAR );
			}
			else
			{
				act( "$N jest ju¿ otoczony bia³± aur±.", ch, NULL, victim, TO_CHAR );
			}
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch ) send_to_char( "Nie uda³o ci siê otoczyæ bia³± aur±.\n\r", ch );
		else act( "Nie uda³o ci siê otoczyæ $Z bia³± aur±.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 3 + level/10 + UMAX( 0, level - LEVEL_HERO );
	if ( number_range( 0, OLD_28_VALUE ) > luck ) duration -= 1;
	if ( number_range( 0, luck ) < 30 ) duration -= 1;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 2, duration );
    af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_SANCTUARY;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "$n otacza siê bia³± aur±.", victim, NULL, NULL, TO_ROOM );
	if ( victim == ch ) send_to_char( "Otacza ciê bia³a aura.\n\r", victim );
	else act( "$n otacza ciê bia³± aur±.", ch, NULL, victim, TO_VICT );

	return;
}

void spell_silence( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );
	int duration;

	if ( IS_AFFECTED( victim, AFF_SILENCE ) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) || IS_AFFECTED( victim, AFF_PERFECT_SENSES ) )
		return;

	if ( number_range( 0, vluck - luck ) > 5 )
	{
		send_to_char( "Twoja ofiara mia³a naprawdê du¿o szczê¶cia i nie uda³o ci siê.\n\r", ch );
		return;
	}

	duration = 4 + level / 7.5;
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 1;

	if ( number_range( 0, vluck ) > 15 ) duration -= 1;
	if ( number_range( 0, vluck ) < 5 ) duration += 1;

	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			if ( level > 24 )
				++duration;
			++duration;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_SILENCE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Zosta³<&e¶/a¶/e¶> uciszon<&y/a/e>!\n\r", victim );

	switch ( victim->sex )
	{
		case 0:
			act( "$n staje siê nagle bardzo ciche.", victim, NULL, NULL, TO_ROOM );
			break;
		case 1:
			act( "$n staje siê nagle bardzo cichy.", victim, NULL, NULL, TO_ROOM );
			break;
		default:
			act( "$n staje siê nagle bardzo cicha.", victim, NULL, NULL, TO_ROOM );
			break;
	}
	return;
}

/* Pomoc (skladnia: cast aid <kto>)
 *
 * No wiec niech bedzie bless + cure light
 * z tym, ze kazde nastepne rzucenie daje tylko leczenie bez blessa
 */

void spell_aid( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration;

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim == ch ) send_to_char( "Nie uda³o ci siê sprowadziæ boskiej mocy.\n\r", ch );
		else act( "Nie uda³o ci siê sprowadziæ boskiej mocy na $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 3 + level / ( 10 + ( ( number_range( 0, luck ) > 10 ) ? 3 : 6 ) );
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 2;

	if ( !is_affected( victim, sn ) )
	{
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = UMAX( 1, duration ); af.rt_duration = 0;
		af.location = APPLY_HITROLL;
		af.modifier = 1 + ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) > 15 ) ) ? 1 : 0;
		af.bitvector = &AFF_NONE;
		affect_to_char( victim, &af, NULL, TRUE );
	}

	act( "Na $c sp³ywa boska moc.", victim, NULL, NULL, TO_ROOM );
	if ( victim == ch ) send_to_char( "Sam<&/a/o> powodujesz, ¿e sp³ywa na ciebie boska moc.\n\r", victim );
	else act( "Za po¶rednictwem $z sp³ywa na ciebie boska moc.", ch, NULL, victim, TO_VICT );
	heal_char( ch, victim, 8 + dice( 1, 4 ), FALSE );

	if ( is_affected( victim, gsn_bleeding_wound ) )
	{
		if ( skill_table[ gsn_bleeding_wound ].msg_off )
		{
			send_to_char( skill_table[ gsn_bleeding_wound ].msg_off, victim );
			send_to_char( "\n\r", victim );
		}
		affect_strip( victim, gsn_bleeding_wound );
	}

	return;
}

void spell_chant( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	CHAR_DATA *vch;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration, mod;

	if ( !ch->fighting || ch->fighting->in_room != ch->in_room )
	{
		send_to_char( "Ju¿ mia³<&e/a/o>¶ zacz±æ ¶piewaæ psalm, kiedy zda³<&e/a/o>¶ sprawê, ¿e nikt tu nie walczy.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Co¶ zak³óci³o twoje skupienie i nie dokoñczy³<&e/a/o>¶ ¶piewu.\n\r", ch );
		return;
	}

	duration = 2 + level / ( 10 + ( ( number_range( 0, luck ) > 10 ) ? 0 : 3 ) );
	if ( number_range( 0, luck ) > 15 ) duration += 1;
    if ( number_range( 0, luck ) < 5 ) duration -= 1;

	if ( ch->class == CLASS_CLERIC )
    {
		mod = URANGE( 2, 2 * level / 15 + ( number_range( 0, luck ) > 12 ? 1 : 0 ), 5 );
    }
    else
    {
        mod = URANGE( 1, ch->level / 10 + ( number_range( 0, luck ) > 12 ? 1 : 0 ), 3 );
    }
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_DAMROLL;
	af.modifier = mod;
	af.bitvector = &AFF_CHANT;
	/* dla grupki ch */

	for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
	{
		if ( !is_same_group( vch, ch ) || IS_AFFECTED( vch, AFF_CHANT )
		     || IS_AFFECTED( vch, AFF_PRAYER ) || !vch->fighting )
			continue;

		affect_to_char( vch, &af, "+Zyska³<&e¶/a¶/o¶> przychylno¶æ bogów", TRUE );
	}

	af.modifier = -mod;

	/* dla grupki victim */
	victim = ch->fighting;

	for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
	{
		if ( !is_same_group( vch, victim ) || IS_AFFECTED( vch, AFF_CHANT )
		     || IS_AFFECTED( vch, AFF_PRAYER ) || !vch->fighting )
			continue;

		affect_to_char( vch, &af, "-Bogowie nie sprzyjaj± tobie", TRUE );
	}

	act( "$n zaczyna nuciæ psalm.", ch, NULL, NULL, TO_ROOM );
	act( "Zaczynasz nuciæ psalm.", ch, NULL, NULL, TO_CHAR );
	return;
}

/* Stworzenie wody (skladnia: cast 'create water')
 * napelnia jeden obiekt aktualnie */

void spell_create_water( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
    AFFECT_DATA *paf;

	if ( obj->item_type != ITEM_DRINK_CON )
	{
		print_char( ch, "%s nie mo¿na nape³niæ wod±.\n\r", capitalize( obj->name2 ) );
		return;
	}

	if ( obj->value[ 0 ] == obj->value[ 1 ] )
	{
		print_char( ch, "W %s nie zmie¶ci siê ju¿ nic wiêcej.\n\r", obj->name6 );
		return;
	}

	if ( obj->value[ 2 ] != LIQ_WATER && obj->value[ 1 ] > 0 )
	{
		print_char( ch, "W %s jest jakas inna ciecz.\n\r", obj->name6 );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		print_char( ch, "Nie uda³o ci siê wype³niæ %s wod±.\n\r", obj->name2 );
		return;
	}
	if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
        if ( paf->modifier < 30 )
        {
             if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
                  {

                    if (number_percent()  < paf->modifier)
                    {
                       obj->value[ 2 ] = LIQ_HEALING_WATER;
	                   obj->value[ 1 ] = obj->value[ 0 ];
	                   send_to_char( "Czujesz jak zebran± wodê zaczyna przenikaæ wype³niaj±ca ciê ¶wiêta moc.\n\r", ch );
	                   act( "$n nape³nia $h uzdrawiaj±c± wod±.", ch, obj, NULL, TO_ROOM );
	                   print_char( ch, "%s nape³nia siê uzdrawiaj±c± wod±.\n\r", capitalize( obj->short_descr ) );
	                   paf->modifier -= 1;
	                   if (paf->modifier < 1) paf->modifier = 1;
                       return;
                    }

                    if (number_percent() < (-paf->modifier))
                    {
                       obj->value[ 2 ] = LIQ_WATER;
	                   obj->value[ 1 ] = obj->value[ 0 ];
	                   obj->value[ 3 ] = 1;
	                   send_to_char( "Czujesz jak z zaklêciem dzieje siê co¶ z³ego.\n\r", ch );
	                   act( "$n nape³nia $h  wod±.", ch, obj, NULL, TO_ROOM );
	                   print_char( ch, "%s nape³nia siê wod±.\n\r", capitalize( obj->short_descr ) );
	                   return;

                    }


                  }


        }

    }

	act( "$n nape³nia $h wod±.", ch, obj, NULL, TO_ROOM );
	print_char( ch, "%s nape³nia siê wod±.\n\r", capitalize( obj->short_descr ) );

	obj->value[ 2 ] = LIQ_WATER;
	obj->value[ 1 ] = obj->value[ 0 ];
	return;
}

/*
 * Tener:
 * create healing water : dok³adna kopia create water, za wyj±tkiem innych komunikatów oraz rodzaju p³ynu.
 * Nie mam si³y abstrahowaæ tego czarku
 */

void spell_create_healing_water( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( obj->item_type != ITEM_DRINK_CON )
	{
		print_char( ch, "%s nie mo¿na nape³niæ uzdrawiaj±c± wod±.\n\r", capitalize( obj->name2 ) );
		return;
	}

	if ( obj->value[ 0 ] == obj->value[ 1 ] )
	{
		print_char( ch, "W %s nie zmie¶ci siê ju¿ nic wiêcej.\n\r", obj->name6 );
		return;
	}

	if ( obj->value[ 2 ] != LIQ_HEALING_WATER && obj->value[ 1 ] > 0 )
	{
		print_char( ch, "W %s jest jakas inna ciecz.\n\r", obj->name6 );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		print_char( ch, "Nie uda³o ci siê wype³niæ %s uzdrawiaj±c± wod±.\n\r", obj->name2 );
		return;
	}

	act( "$n nape³nia $h uzdrawiaj±c± wod±.", ch, obj, NULL, TO_ROOM );
	print_char( ch, "%s nape³nia siê uzdrawiaj±c± wod±.\n\r", capitalize( obj->short_descr ) );

	obj->value[ 2 ] = LIQ_HEALING_WATER;
	obj->value[ 1 ] = obj->value[ 0 ];
	return;
}

/* Dobre jagody (skladnia: cast goodbarry)
 *
 * robi iles tam jagod, ktore z proga daja hp
 */

void spell_goodbarry( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj;
	int i, ile;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_NOREST ) || ch->in_room->sector_type == 25 )
	{
		send_to_char( "Nie uda³o ci siê sprowadziæ jagód.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê sprowadziæ jagód.\n\r", ch );
		return;
	}

	ile = number_range( level / 4, level / 2 );

	if ( !stat_throw( ch, STAT_WIS ) && !stat_throw( ch, STAT_LUC ) ) ile /= 2;
	if ( stat_throw( ch, STAT_WIS ) && stat_throw( ch, STAT_LUC ) ) ++ile;

	ile = URANGE( 1, ile, 9 );

	for ( i = 0; i < ile; i++ )
	{
		obj = create_object( get_obj_index( OBJ_VNUM_GOODBARRY ), FALSE );
		obj->value[ 0 ] = 6;
		obj->value[ 1 ] = 0;
		obj_to_room( obj, ch->in_room );
	}

	if ( ile == 1 ) act( "Na ziemi pojawia siê jagoda.", ch, NULL, NULL, TO_ALL );
	else act( "Na ziemi pojawia siê kilka jagód.", ch, NULL, NULL, TO_ALL );

	return;
}

/* Chodzenie po wodzie (skladnia: cast waterwalk [kto])
 */
void spell_waterwalk( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration;

	if ( IS_AFFECTED( victim, AFF_WATERWALK ) )
	{
		if ( victim == ch )
			send_to_char( "Ju¿ potrafisz chodziæ po wodzie.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					send_to_char( "Ono ju¿ to potrafi.\n\r", ch );
					break;
				case 1:
					send_to_char( "On ju¿ to potrafi.\n\r", ch );
					break;
				default :
					send_to_char( "Ona ju¿ to potrafi.\n\r", ch );
					break;
			}
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	duration = 3 + ch->level / ( 10 + ( ( number_range( 0, luck ) > 10 ) ? 0 : 5 ) );

	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 2;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier = 0;

	af.bitvector = &AFF_WATERWALK;
	affect_to_char( victim, &af, NULL, TRUE );

	act( "Rzucony przez $z czar powoduje, ¿e od teraz mo¿esz chodziæ po wodzie.", ch, NULL, victim, TO_VICT );
	if ( victim == ch )
		send_to_char( "Teraz potrafisz chodzic po wodzie!\n\r", victim );
	else
		act( "Uda³o ci siê rzuciæ zaklêcie na $Z.", ch, NULL, victim, TO_CHAR );

	return;
}

/**
 * Poznanie charakteru (skladnia: cast 'know alignment' <kto>)
 */
void spell_know_alignment( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	char buf[ MAX_STRING_LENGTH ];
	char *color;

	if ( victim == ch )
	{
		send_to_char( "Nie dowiesz siê niczego nowego o sobie.\n\r", victim );
		return;
	}

	act( "$n patrzy siê na ciebie przenikliwie.", ch, NULL, victim, TO_VICT );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		act( "Nie uda³o ci siê poznaæ charakteru $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->alignment > 900 ) color = "{Gjasnozielon±";
	else if ( victim->alignment > 700 ) color = "{gzielon±";
	else if ( victim->alignment > 500 ) color = "{Gciemnozielon±";
	else if ( victim->alignment > 300 ) color = "{bciemnoniebiesk±";
	else if ( victim->alignment > -100 ) color = "{Cjasnoniebiesk±";
	else if ( victim->alignment > -300 ) color = "{bciemnoniebiesk±";
	else if ( victim->alignment > -500 ) color = "{rciemnoczerwon±";
	else if ( victim->alignment > -700 ) color = "{Rczerwon±";
	else if ( victim->alignment > -900 ) color = "{Rjasnoczerwon±";
	else color = "{R^fmigoczac± czerwon±";

	switch ( victim->sex )
	{
		case 0:
			sprintf( buf, "$N otoczone jest %s{x aur±.", color );
			break;
		case 1:
			sprintf( buf, "$N otoczony jest %s aur±.", color );
			break;
		default:
			sprintf( buf, "$N otoczona jest %s aur±.", color );
			break;
	}

	act( buf, ch, NULL, victim, TO_CHAR );

	return;
}

/* Oddychanie woda (skladnia: cast 'water breathing' [kto])
 */

void spell_water_breathing( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int duration;

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim != ch )
			act( "Nie uda³o ci siê rzuciæ zaklêcia oddychania pod wod± na $Z.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda³o ci siê rzuciæ zaklêcia oddychania pod wod±.\n\r", ch );
		return;
	}

	duration = 2 + level / ( 9 + ( ( number_range( 0, luck ) > 10 ) ? 0 : 3 ) );
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration -= 2;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_WATERBREATH;
	affect_to_char( victim, &af, NULL, TRUE );

	if ( victim != ch )
		act( "Pod wp³ywem twojego zaklêcia $Z mo¿e oddychaæ pod wod±.", ch, NULL, victim, TO_CHAR );

	if ( !IS_NPC( victim ) && victim->pcdata->oxygen > 0 &&
	     IS_SET( sector_table[ victim->in_room->sector_type ].flag, SECT_UNDERWATER ) )
	{
		victim->pcdata->oxygen = 0;
		send_to_char( "Ze zdziwieniem stwierdzasz ¿e mo¿esz oddychaæ pod wod±.\n\r", ch );
		act( "$n z niedowierzaniem próbuje oddychaæ wod±.", victim, NULL, NULL, TO_ROOM );
	}
	else
		send_to_char( "Mo¿esz teraz oddychaæ pod wod±.\n\r", victim );

	return;
}

/* Przepedzenie dobra (skladnia: cast 'dispel good' <kto>)
 *
 * hmm no z opisu by wynikalo, ze ma albo odsylac w kosmos(slay)
 * albo mocno walic i zmuszac do ucieczki
 * nie wiem za bardzo jak to mam zrobic. Na razie bedzie walic w zaleznosci
 * od alignmentu ofiary i rzucajacego. pozniej ew siê cosik wymysli
 */
 /*Drake: Wywalanie slayowania z dispela, dodanie - przy duzych roznicach poziomów
 kler wali poprostu wiecej (+1d10 za kazdy 1 poziom ponad 8 roznicy) obrazen.*/
void spell_dispel_good( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int dam, vluck, modyfikator;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( victim == ch )
	{
		send_to_char( "Sam<&/a/o> siebie chcesz odpêdziæ?\n\r", ch );
		return;
	}

	if ( IS_GOOD( ch ) )
	{
		send_to_char( "Tylko kto¶ powi±zany ze Z³em mo¿e przepêdzaæ Dobro!\n\r", ch );
		return;
	}

	if ( !IS_GOOD( victim ) )
	{
		send_to_char( "Ten czar jest skuteczny tylko przeciw Dobru.\n\r", ch );
		return;
	}

	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, luck + vluck ) == 0 )
	{
		act( "Nie uda³o ci siê przepêdziæ Dobra w $B.", ch, NULL, victim, TO_CHAR );
		return;
	}

	//Dopalka przy duzej roznicy poziomow do obrazen.
	modyfikator = level - (victim->level) - 8;
	if (modyfikator < 0) modyfikator = 0;

	/*damage zalezny od alignmentu
	 * 1000-(-1000) /16 = 125
	 * 350-(-350)   /16 = 44
	 * chyba wystarczy nie?
	 * dodatkowo modyfikatory od szczê¶cia
	 */

	dam = ( victim->alignment - ch->alignment ) / 16 + dice( modyfikator, 10) + dice (2, 12);

	if ( number_range( 0, luck ) > 15 ) dam += 10;
	if ( number_range( 0, luck ) < 5 ) dam -= 10;
	if ( number_range( 0, vluck ) > 15 ) dam -= 10;
	if ( number_range( 0, vluck ) < 5 ) dam += 10;

	dam = luck_dam_mod( ch, dam );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam/4;
	 	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
			dam -= dam/4;
	}

	if ( IS_NEUTRAL( ch ) )
	   dam = (dam*3)/4;

	spell_damage( ch, vo, dam, sn, DAM_NONE, TRUE );
	return;
}

/* Przepedzenie zla (skladnia: cast 'dispel evil' <kto>)
 *
 * to samo co dispel_good
 */
void spell_dispel_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA *paf;
	int dam, vluck, modyfikator, holy_dam_mod = 100;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	bool holy_bonus = TRUE;

	if ( victim == ch )
	{
		send_to_char( "Sam<&/a/o> siebie chcesz odpêdziæ?\n\r", ch );
		return;
	}

	if ( IS_EVIL( ch ) )
	{
		send_to_char( "Tylko kto¶ si³± Dobra mo¿e przepedzaæ Z³o!\n\r", ch );
		return;
	}
	if ( !IS_EVIL( victim ) )
	{
		send_to_char( "Ten czar jest skuteczny tylko przeciw Z³u.\n\r", ch );
		return;
	}

   //blokada braku modlitwy
    if ( ( paf = affect_find( ch->affected, gsn_prayer_last )) != NULL )
    {
        if ( paf->modifier > 48 )
        {
            send_to_char( "Starasz siê zebraæ w sobie moce dobra, jednak twój Bóg ci nie sprzyja. \n\r", ch );
            act( "$n stara siê skoncentrowaæ, jednak co¶ mu najwyra¼niej przeszkodzi³o, poniewa¿ otwiera oczy z zdziwieniem.", ch, NULL, NULL, TO_ROOM );
            holy_bonus = FALSE;
            return;
        }
        else if ( paf->modifier > 36 )
        {
            send_to_char( "Zbieraj±c w sobie moce dobra wyczuwasz, ze co¶ jest nie tak. \n\r", ch );
            holy_dam_mod = 50;
            holy_bonus = FALSE;
        }
    }
    //bonusy od bycia dobrym, zlym paladynem
    if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL && holy_bonus == TRUE)
    {
         if (paf->modifier > 30 && number_percent() < paf->modifier/2)
         {
             holy_dam_mod += 20;
             paf->modifier -= 3;
         }

          if (paf->modifier > 60 && number_percent() < paf->modifier/3)
         {
             holy_dam_mod += 20;
             paf->modifier -= 4;
         }

          if (paf->modifier < - 40 )
         {
             holy_dam_mod -= 20;

         }

          if (paf->modifier < - 60 )
         {
             holy_dam_mod -= 30;

         }

          if (paf->modifier < - 80 && paf->modifier > -100 )
         {
          send_to_char( "Rzucaj±c zaklêcie czujesz nag³y ucisk w gardle, w oczach ci ciemnieje, a ca³a zgromadzona moc rozchodzi siê w nico¶æ. \n\r", ch );
          act( "$n stara siê skoncentrowaæ, nagle zaczyna sapaæ, jakby kto¶ go dusi³. Po chwili odzyskuje trze¼wo¶æ umys³u.", ch, NULL, NULL, TO_ROOM );
          return;
         }

           if (paf->modifier <= - 100 )
         {
          send_to_char( "Inkantuj±c zaklêcie zaczynasz czuæ ból w sercu, a twój umys³ wype³nia strach. \n\r", ch );
          act( "$n wypowiada zaklêcie, a w jego oczach pojawia siê przera¿enie.", ch, NULL, NULL, TO_ROOM );
          dam = (-paf->modifier/2);
          spell_damage( ch, ch, dam, sn, DAM_NONE, TRUE );
          return;
         }

    }

	vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, luck + vluck ) == 0 )
	{
		act( "Nie uda³o ci siê przepêdziæ Z³a w $B.", ch, NULL, victim, TO_CHAR );
		return;
	}

		//Dopalka przy duzej roznicy poziomow do obrazen.
	modyfikator = level - victim->level - 8;
	if (modyfikator < 0) modyfikator = 0;

	/*
	 *
	 * Wyliczanie damage zale¿nego od alignmentu
	 * 1000-(-1000) /16 = 125
	 * 350-(-350)   /16 = 44
	 * chyba wystarczy nie?
	 *
	 */
  dam = ( ch->alignment - victim->alignment ) / 16 + dice( modyfikator, 10) + dice (2, 12);//Raszer, poprawa dmg, ch->align - victim->align zamiast odwrotnie
	/*
	 * losowanie szaans na zmniejszenie lub zwiêkszenie dam
	 */
	if ( number_range( 0, luck ) > 15 ) dam += 10;
	if ( number_range( 0, luck ) < 5 ) dam -= 10;
	if ( number_range( 0, vluck ) > 15 ) dam -= 10;
	if ( number_range( 0, vluck ) < 5 ) dam += 10;

	dam = luck_dam_mod( ch, dam );
	/*
	 * Damage po modyfikacjach szans:
	 *
	 * maksymalny - 156 + 1-10 za kazdy poziom roznicy powyzej 9.
	 * minimalny  - 27
	 *
	 */

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		dam -= dam / 4;
		if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
			dam -= dam/4;
	}

	 if ( ch->class == CLASS_PALADIN && !IS_NPC(ch) )
	 dam -= dam / number_range( 4, 5 );

	if ( IS_NEUTRAL( ch ) )
	   dam = (dam*3)/4;
    if (holy_dam_mod > 100) send_to_char( "Czujesz jak zaklêcie zaczyna rezonowaæ z zgromadzon± w tobie ¶wiêt± moc±. \n\r", ch );
    if (holy_dam_mod < 100) send_to_char( "Czujesz jak zebrana w tobie ciemno¶æ os³abia moc zaklêcia. \n\r", ch );
    dam *= holy_dam_mod/100;
	spell_damage( ch, vo, dam, sn, DAM_NONE, TRUE );
	return;
}

/* Wykrycie dobra (skladnia: cast 'detect good' [kto])
 *
 * tutaj bedzie tylko affect, reszta w patrzeniu i ruszaniu sie
 */

void spell_detect_good( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration, luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_DETECT_GOOD ) )
	{
		if ( victim == ch ) send_to_char( "Przecie¿ ju¿ potrafisz wykryæ dobro.\n\r", ch );
		else act( "$N ju¿ potrafi wykryæ dobro.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim != ch )
		{
			switch ( victim->sex )
			{
				case 0:
					act( "Nie uda³o ci siê sprawiæ, by $N potrafi³o wykrywaæ dobro.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "Nie uda³o ci siê sprawiæ, by $N potrafi³ wykrywaæ dobro.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "Nie uda³o ci siê sprawiæ, by $N potrafi³a wykrywaæ dobro.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		else
			send_to_char( "Nie uda³o ci siê sprawiæ, ¿eby¶ wykrywa³<&/a/o> dobro.\n\r", ch );
		return;
	}

	duration = level;
	if ( number_range( 0, luck ) > 15 ) duration += number_range( 2, level / 5 );
	if ( number_range( 0, luck ) < 5 ) duration /= 2;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_DETECT_GOOD;
	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Wyra¼niej dostrzegasz bia³± magiê.\n\r", victim );
	if ( ch != victim )
		act( "Pod wp³ywem twojego zaklêcia $Z dotrzega przejawy bia³ej magii.", ch, NULL, victim, TO_CHAR );
	return;
}

/* Wykrycie zla (skladnia: cast 'detect evil' [kto])
 *
 * tutaj bedzie tylko affect, reszta w patrzeniu i ruszaniu sie
 */

void spell_detect_evil( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration, luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_DETECT_EVIL ) )
	{
		if ( victim == ch ) send_to_char( "Przecie¿ ju¿ potrafisz wykryc zlo.\n\r", ch );
		else act( "$N ju¿ potrafi wykryc zlo.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim != ch )
		{
			switch ( victim->sex )
			{
				case 0:
					act( "Nie uda³o ci siê sprawiæ, by $N potrafi³o wykrywaæ z³o.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "Nie uda³o ci siê sprawiæ, by $N potrafi³ wykrywaæ z³o.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "Nie uda³o ci siê sprawiæ, by $N potrafi³a wykrywaæ z³o.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		else
			send_to_char( "Nie uda³o ci siê sprawiæ, ¿eby¶ wykrywa³<&/a/o> z³o.\n\r", ch );
		return;
	}

	duration = level;
	if ( number_range( 0, luck ) > 15 ) duration += number_range( 2, level / 5 );
	if ( number_range( 0, luck ) < 5 ) duration /= 2;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_DETECT_EVIL;
	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Wyrazniej dostrzegasz czarna magie.\n\r", victim );
	if ( ch != victim )
		act( "Pod wp³ywem twojego zaklêcia $Z dotrzega przejawy czarnej magii.", ch, NULL, victim, TO_CHAR );
	return;
}

/* Plomienne ostrze (skladnia: cast 'flame blade')
 *
 * Wlasciwie to bedzie to samo, co spirit_hammer, tez bron
 * tylko obrazenia beda od ognia
 */

void spell_flame_blade( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA * obj = NULL;
    int duration, luck = get_curr_stat_deprecated( ch, STAT_LUC );

    if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
    {
        send_to_char( "Nie uda³o ci siê przywo³aæ p³omiennego ostrza.\n\r", ch );
        return;
    }

    if ( get_hand_slots( ch, WEAR_WIELD ) >= 2 )
    {
        send_to_char( "Masz ju¿ dwie zajête rêce. Zdejmij co¶.\n\r", ch );
        return;
    }

    if ( !get_eq_char( ch, WEAR_WIELD ) )
    {
        obj = create_object( get_obj_index( OBJ_VNUM_FLAMEBLADE ), FALSE );

        if ( !obj )
        {
            bug( "Brak obiektu: OBJ_VNUM_FLAMEBLADE.", 0 );
            return;
        }

        duration = level;
        if ( number_range( 0, luck ) > 10 )
        {
            duration *= 2;
        }
        obj->timer = duration;
        obj->rent_cost = 2 * level;

        obj->value[ 1 ] = 2 + ( ( number_range( 0, luck ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, level ) > 17 ) ? 1 : 0 );
        obj->value[ 2 ] = 3 + ( ( number_range( 0, luck ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, level ) > 17 ) ? 1 : 0 );
        //Brohacz: przeglad broni z czarow
        //		obj->value[ 5 ] = URANGE( 1, level / 6, 5 );
        //		obj->value[ 6 ] = URANGE( 1, level / 6, 5 );
        obj->value[ 5 ] = 0;
        obj->value[ 6 ] = 0;

        if ( number_range( 1, 2*level ) > 10 )
            obj->value[ 5 ] += 1;

        if ( number_range( 1, 2*level ) > 15 )
            obj->value[ 5 ] += 1;

        if ( number_range( 1, 2*level ) > 10 )
            obj->value[ 6 ] += 1;

        if ( number_range( 1, 2*level ) > 15 )
            obj->value[ 6 ] += 1;

        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_WIELD, TRUE );
        act( "Z rêki $z wyrasta p³omienne ostrze.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "Z twej rêki wyrasta p³omienne ostrze.\n\r", ch );

        if ( number_range( 0, 620 ) < level )  // koles ma farta i przywolal lepsze ostrze
        {
            obj->value[ 1 ] += 1;
            obj->value[ 2 ] += 1;
            obj->value[ 5 ] += 1;
            obj->value[ 6 ] += 1;

            send_to_char( "P³omienne ostrze rozb³yska przez chwilê bardzo jasnym p³omieniem, który po chwili przygasa.\n\r", ch );
        }

        return;
    }
    send_to_char( "Przecie¿ trzymasz ju¿ co¶ w rêce.\n\r", ch );
    return;
}

/* Ochrona przed energia (skladnia: cast 'energy shield' [kto])
 *
 * resist na negatywna i pozytywna energie
 *
 */
void spell_energy_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ), dur, mod;

	if ( IS_AFFECTED( victim, AFF_ENERGY_SHIELD ) )
	{
		if ( ch == victim )
			send_to_char( "Nie mo¿esz byæ ju¿ bardziej chronion<&y/a/e>.\n\r", ch );
		else
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie mo¿e byæ ju¿ bardziej chronione.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N nie mo¿e byæ ju¿ bardziej chroniony.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "$N nie mo¿e byæ ju¿ bardziej chroniona.", ch, NULL, victim, TO_CHAR );
					break;
			}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim != ch )
			act( "Nie uda³o ci siê otoczyæ $Z polem pozytywnej energii.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda³o ci siê otoczyæ polem pozytywnej energii.\n\r", ch );
		return;
	}

	mod = 30 + level/2 + ( ( luck > 15 ) ? 5 : 0 );
	dur = 3 + level / 6 + ( ( luck > 15 ) ? 1 : 0 );
	/* Bonus dla specjalisty */
	if ( !IS_NPC( ch ) )
	{
		if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
		{
			mod += level / 2;
			dur += level / 6;
		}
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = mod;
	af.duration = dur; af.rt_duration = 0;
	af.location = APPLY_RESIST;
	af.modifier = RESIST_NEGATIVE;
	af.bitvector = &AFF_ENERGY_SHIELD;
	affect_to_char( victim, &af, NULL, TRUE );

	af.modifier = RESIST_HOLY;
	affect_to_char( victim, &af, NULL, TRUE );

	if ( ch == victim )
	{
		act( "$n otacza siê polem pozytywnej energii.", ch, NULL, victim, TO_ROOM );
		send_to_char( "Czujesz, ¿e chroni ciê pole pozytywnej energii.\n\r", victim );
	}
	else
	{
		act( "$n otacza $C polem pozytywnej energii.", ch, NULL, victim, TO_NOTVICT );
		act( "$n otacza ciê polem pozytywnej energii.", ch, NULL, victim, TO_VICT );
		act( "Otaczasz $C polem pozytywnej energii.", ch, NULL, victim, TO_CHAR );
	}
	return;
}

void spell_prayer( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	CHAR_DATA *vch;
	AFFECT_DATA af;
	int duration, luck = get_curr_stat_deprecated( ch, STAT_LUC ), mod;

	if ( !ch->fighting || ch->fighting->in_room != ch->in_room )
	{
		send_to_char( "Nie bierzesz udzia³u w walce.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "W ostatniej chwili rozpraszasz swoj± uwagê.\n\r", ch );
		return;
	}

	duration = 2 + level / 10;

	if ( ch->class == CLASS_CLERIC )
    {
		mod = URANGE( 2, 2 * level / 15 + ( number_range( 0, luck ) > 12 ? 1 : 0 ), 5 );
    }
	else
    {
		mod = URANGE( 1, ch->level / 10 + ( number_range( 0, luck ) > 12 ? 1 : 0 ), 3 );
    }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.bitvector = &AFF_PRAYER;

	/* dla grupki ch (+1) */
	for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
	{
		if ( !is_same_group( vch, ch ) || IS_AFFECTED( vch, AFF_PRAYER ) || IS_AFFECTED( vch, AFF_CHANT ) || !vch->fighting )
			continue;

		luck = get_curr_stat_deprecated( vch, STAT_LUC );

		af.modifier = mod;

		affect_to_char( vch, &af, "+Jeste¶ pod wp³ywem b³ogos³awieñstwa celno¶ci", TRUE );
	}

	/* dla grupki victim (-1) */
	victim = ch->fighting;

	for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
	{
		if ( !is_same_group( vch, victim ) || IS_AFFECTED( vch, AFF_PRAYER ) || IS_AFFECTED( vch, AFF_CHANT ) || !vch->fighting )
			continue;

		luck = get_curr_stat_deprecated( vch, STAT_LUC );

		af.modifier = - mod;

		affect_to_char( vch, &af, "-Jeste¶ pod wp³ywem przekleñstwa celno¶ci", TRUE );
	}

	act( "$n zaczyna nuciæ modlitwê.", ch, NULL, NULL, TO_ROOM );
	act( "Zaczynasz nuciæ modlitwê.", ch, NULL, NULL, TO_CHAR );
	return;
}

/* Fajerwerki (skladnia: cast pyrotechnics)
 *
 * wlasciwie, to daje blinda na 3 ticki wszystkim poza grupa kaplana
 * i innymi kaplanami
 */

void spell_pyrotechnics( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * vch;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê wywo³aæ wybuchu.\n\r", ch );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dice( 1, 3 ) + ch->level / 10 + ( number_range( 0, luck ) > 10 ) ? 1 : 0 ; af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.bitvector = &AFF_BLIND;

	act( "Przera¼liwie jasna kula unosi siê w powietrze i eksploduje {Wbia³ym{x ¶wiat³em!", ch, NULL, NULL, TO_ALL );

	for ( vch = ch->in_room->people;vch;vch = vch->next_in_room )
	{
		if ( IS_AFFECTED( vch, AFF_BLIND ) || is_safe( ch, vch, TRUE ) || is_same_group( vch, ch ) || IS_AFFECTED( vch, AFF_PERFECT_SENSES ) || is_undead(vch) )
			continue;

		if ( dice( 2, level ) < vch->level || saves_spell_new( vch, skill_table[sn].save_type, skill_table[sn].save_mod, ch, sn ) )
		{
			if ( vch->fighting == NULL && can_see( vch, ch ) && can_move( vch ) )
			{
				if ( IS_NPC( vch ) && HAS_TRIGGER( vch, TRIG_KILL ) )
					mp_percent_trigger( vch, ch, NULL, NULL, &TRIG_KILL );

				multi_hit( vch, ch, TYPE_UNDEFINED );
			}
			continue;
		}

		af.modifier = -4 + ( number_range( 0, get_curr_stat_deprecated( vch, STAT_LUC ) ) > 15 ) ? 1 : 0;
		affect_to_char( vch, &af, NULL, TRUE );

		act( "O¶lepiasz $C!", ch, NULL, vch, TO_CHAR );
		send_to_char( "Zosta³<&e/a/o>¶ o¶lepion<&y/a/e>!\n\r", vch );
		act( "Jaskrawe ¶wiat³o o¶lepia $C!", ch, NULL, vch, TO_NOTVICT );
	}

	return;
}

/* Zdjecie paralizu (skladnia: cast 'remove paralysis' <kto>)
 *
 * zdejmuje holdy, hmm z truciznami nie wiem jak bo ich jeszcze nie ma
 *
 */
void spell_remove_paralysis( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA *af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( !IS_AFFECTED( victim, AFF_PARALYZE ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N nie jest sparali¿owane.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N nie jest sparali¿owany.", ch, NULL, victim, TO_CHAR );
				break;
			default:
				act( "$N nie jest sparali¿owana.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		act( "Nie uda³o ci siê usun±æ parali¿u $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	for ( af = victim->affected; af != NULL; af = af->next )
	{
		if ( af->bitvector == &AFF_PARALYZE )
			affect_strip( victim, af->type );
	}

    act( "$n odzyskuje kontrolê nad swoim cia³em.", victim, NULL, NULL, TO_ROOM );
    send_to_char( "Odzyskujesz kontrolê nad swoim cia³em.\n\r", victim );

	return;
}

void spell_blindness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( IS_AFFECTED( victim, AFF_BLIND ) || IS_AFFECTED( victim, AFF_PERFECT_SENSES ) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		return;

	if ( number_range( 0, luck + vluck ) == 0 )
	{
		act( "Nie uda³o ci siê o¶lepiæ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 4;
	if ( number_range( 0, luck ) > 15 ) ++duration;
	if ( number_range( 0, luck ) < 5 ) --duration;

	if ( number_range( 0, vluck ) > 15 ) --duration;
	if ( number_range( 0, vluck ) < 5 ) ++duration;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.bitvector = &AFF_BLIND;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Zosta³o¶ o¶lepione.\n\r", victim );
			act( "$n wygl±da na o¶lepione.", victim, NULL, NULL, TO_ROOM );
			break;
		case 1:
			send_to_char( "Zosta³e¶ o¶lepiony.\n\r", victim );
			act( "$n wygl±da na o¶lepionego.", victim, NULL, NULL, TO_ROOM );
			break;
		default :
			send_to_char( "Zosta³a¶ o¶lepiona.\n\r", victim );
			act( "$n wygl±da na o¶lepion±.", victim, NULL, NULL, TO_ROOM );
			break;
	}
	return;
}

void spell_slow_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA *af, *af_next;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int vluck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( !is_affected( victim, gsn_poison ) )
	{
		if ( victim == ch )
			send_to_char( "Nie jeste¶ zatrut<&y/a/e>.\n\r", ch );
		else
		{
			switch ( victim->sex )
			{
				case 0:
					act( "$N nie jest chyba zatrute.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N nie jest chyba zatruty.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "$N nie jest chyba zatruta.", ch, NULL, victim, TO_CHAR );
					break;
			}
		}
		return;
	}

	for ( af = victim->affected; af; af = af_next )
	{
		af_next = af->next;

		if ( af->type != gsn_poison )
			continue;

		if ( af->bitvector == &AFF_POISON )
		{
			if ( af->level == 0 )
			{
				if ( victim == ch )
					send_to_char( "Kr±¿±ca w twojej krwi trucizna zosta³a ju¿ wcze¶niej spowolniona.\n\r", ch );
				else
					act( "Kr±¿±ca w krwi $Z trucizna zosta³a ju¿ spowolniona.", ch, NULL, victim, TO_CHAR );
				return;
			}
		}
	}

	if ( ch == victim )
	{
		if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
		{
			send_to_char( "Nie uda³o ci siê zwolniæ dzia³ania trucizny kr±¿±cej w twojej krwi.\n\r", ch );
			return;
		}
		else
		{
			if ( number_range( 0, luck + vluck + LUCK_BASE_MOD / 2 ) == 0 )
			{
				act( "Nie uda³o ci siê zwolniæ dzia³ania trucizny kr±¿±cej w krwi $Z.", ch, NULL, victim, TO_CHAR );
				return;
			}
		}

		for ( af = victim->affected; af; af = af_next )
		{
			af_next = af->next;

			if ( af->type != gsn_poison )
				continue;

			if ( af->bitvector == &AFF_POISON )
			{
				send_to_char( "Trucizna kr±¿±ca w twojej krwi zosta³a spowolniona.\n\r", victim );
				if ( ch != victim )
					act( "Trucizna kr±¿±cej w krwi $Z zosta³a spowolniona.", ch, NULL, victim, TO_CHAR );
				af->level = 0;
			}
			else
				affect_remove( victim, af );
		}
	}
	return;
}

void spell_lesser_restoration( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA *paf_next, *paf;
	sh_int gsn_to_strip = 0, roll = 0;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	roll = number_range( 1, 4 );

	for ( paf = victim->affected; paf != NULL; paf = paf_next )
	{
		paf_next = paf->next;

		if ( paf->type <= 0 || paf->type >= MAX_SKILL )
			continue;

		if ( skill_table[ paf->type ].spell_fun == spell_null )
			continue;

		gsn_to_strip = 0;

		switch ( paf->location )
		{
			case APPLY_STR:
			case APPLY_DEX:
			case APPLY_INT:
			case APPLY_WIS:
			case APPLY_CON:
			case APPLY_CHA:
			case APPLY_LUC:
			case APPLY_SEX:
			case APPLY_CLASS:
			case APPLY_HEIGHT:
			case APPLY_WEIGHT:
			case APPLY_HIT:
				if ( paf->modifier >= 0 )
					continue;

				gsn_to_strip = paf->type;
				break;
			default: continue;
		}

		/* tylko spelle */
		if ( gsn_to_strip == 0 )
			continue;

		roll--;
		affect_strip( victim, gsn_to_strip );

		if ( skill_table[ gsn_to_strip ].msg_off )
		{
			send_to_char( skill_table[ gsn_to_strip ].msg_off, victim );
			send_to_char( "\n\r", victim );
		}

		if ( roll == 0 )
			break;
	}

	act( "Na $c sp³ywa oczyszczaj±ca si³a.", victim, NULL, NULL, TO_ROOM );
	if ( victim == ch ) send_to_char( "Czujesz, jak ogarnia ciê oczyszczaj±ca si³a.\n\r", victim );
	else
	{
		act( "Dziêki tobie na $C sp³ywa oczyszczaj±ca si³a.", ch, NULL, victim, TO_CHAR );
		act( "Za po¶rednictwem $z sp³ywa na ciebie oczyszczaj±ca si³a.", ch, NULL, victim, TO_VICT );
	}

	if ( !IS_NPC( ch ) )
		ch->condition[ COND_SLEEPY ] = 0;

	return;
}

void spell_restoration( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA *paf_next, *paf;
	sh_int gsn_to_strip = 0;
	bool exit = FALSE;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		if ( victim != ch )
			act( "Nie uda³o ci siê przywo³aæ oczyszczaj±ca si³y na $Z.", ch, NULL, victim, TO_CHAR );
		else
			send_to_char( "Nie uda³o ci siê przywo³aæ potê¿nej, oczyszczaj±cej si³y.\n\r", ch );
		return;
	}

	while ( TRUE )
	{
		for ( exit = TRUE, paf = victim->affected; paf != NULL; paf = paf_next )
		{
			paf_next = paf->next;

			if ( paf->type <= 0 || paf->type >= MAX_SKILL )
				continue;

			if ( skill_table[ paf->type ].spell_fun == spell_null )
				continue;

			gsn_to_strip = 0;

			switch ( paf->location )
			{
				case APPLY_STR:
				case APPLY_DEX:
				case APPLY_INT:
				case APPLY_WIS:
				case APPLY_CON:
				case APPLY_CHA:
				case APPLY_LUC:
				case APPLY_SEX:
				case APPLY_CLASS:
				case APPLY_HEIGHT:
				case APPLY_WEIGHT:
				case APPLY_HIT:
				case APPLY_EXP:
					if ( paf->modifier >= 0 )
						continue;
					gsn_to_strip = paf->type;
					break;
				default:
					continue;
			}

			/* tylko spelle */
			if ( gsn_to_strip == 0 )
				continue;

			if ( skill_table[ gsn_to_strip ].msg_off &&
			     skill_table[ gsn_to_strip ].msg_off != '\0' )
			{
				send_to_char( skill_table[ gsn_to_strip ].msg_off, victim );
				send_to_char( "\n\r", victim );
			}

			affect_strip( victim, gsn_to_strip );
			exit = FALSE;
			break;
		}

		if ( exit )
			break;
	}

	act( "Na $c sp³ywa potê¿na, oczyszczaj±ca si³a.", victim, NULL, NULL, TO_ROOM );
	if ( victim == ch ) send_to_char( "Czujesz, jak ogarnia ciê potê¿na, oczyszczaj±ca si³a.\n\r", victim );
	else
	{
		act( "Dziêki tobie na $C sp³ywa potê¿na, oczyszczaj±ca si³a.", ch, NULL, victim, TO_CHAR );
		act( "Za po¶rednictwem $z sp³ywa na ciebie potê¿na, oczyszczaj±ca si³a.", ch, NULL, victim, TO_VICT );
	}

	if ( !IS_NPC( ch ) )
		ch->condition[ COND_SLEEPY ] = 0;

	return;
}

void spell_divine_favor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af, *paf,*paff;
	int duration, mod = level / 3, holy_dur_mod = 0, holy_dam_mod = 0;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( is_affected( victim, sn ) )
	{
		send_to_char( "Przywo³ane wcze¶niej bóstwa s± ci nadal przychylne.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê zdobyæ przychylno¶ci bóstwa.\n\r", ch );
		return;
	}
    if ( ( paff = affect_find( ch->affected, gsn_prayer_last )) != NULL)
    {
         if( paff->modifier < 24)
         {
             //bonusy tylko gdy modlimy sie regularnie

             if( (paf = affect_find( ch->affected, gsn_holy_pool)) != NULL)
             {
                 if (paf->modifier > 25 && number_percent() < URANGE(15, paf->modifier/2, 40))
                 {
                     holy_dur_mod += 2;
                     paf->modifier -= 1;
                 }

                 if (paf->modifier > 50 && number_percent() < 30)
                 {
                     holy_dam_mod += 2;
                     paf->modifier -= 1;
                 }

                 if (paf->modifier > 70 && number_percent() < 30)
                 {
                     holy_dam_mod += 2;
                     paf->modifier -= 2;
                 }
             }
         }
          if( (paf = affect_find( ch->affected, gsn_holy_pool)) != NULL)
          {
              if (-(paf->modifier) > 20 ) holy_dam_mod -= 2;
              if (-(paf->modifier) > 45 ) holy_dam_mod -= 2;
              if (-(paf->modifier) > 65 && -(paf->modifier) <= 85 )
              {
                send_to_char( "Próbujesz zdobyæ przychylno¶æ swego boga, jednak odwraca siê on od twojej pro¶by.\n\r", ch );
		        return;
              }
              if (-(paf->modifier) > 85 ) mod = -3;
          }


    }

	duration = 3 + level / 5 + holy_dur_mod;
    mod += holy_dam_mod;
	if ( number_range( 0, luck ) > 15 ) ++duration;
	if ( number_range( 0, luck ) < 3 ) --duration;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_CHA ) ) > 15 ) ++duration;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_CHA ) ) < 3 ) --duration;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ++duration;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) < 3 ) --duration;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	duration = 3 + level / 5 + holy_dur_mod;
	if ( number_range( 0, luck ) > 15 ) ++duration;
	if ( number_range( 0, luck ) < 3 ) --duration;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_CHA ) ) > 15 ) ++duration;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_CHA ) ) < 3 ) --duration;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ++duration;
	if ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) < 3 ) --duration;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_DAMROLL;
	af.modifier = mod;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	if(mod < 0)
	{
           send_to_char( "Co¶ dzieje siê nie tak, z otaczaj±c± ciê aur±.\n\r", victim );
           act( "$n otacza siê iskrz±c± po¶wiat±, która emanuje dziwnym ch³odem.", victim, NULL, NULL, TO_ROOM );
           return;
    }else
    {
         if(holy_dam_mod > 0 || holy_dur_mod > 0) send_to_char( "Czujesz jak moc zaklêcia ro¶nie.\n\r", victim );
         if(holy_dam_mod < 0 ) send_to_char( "Czujesz jak zgromadzona w tobie ciemno¶æ os³abia dzia³anie zaklêcia.\n\r", victim );

    }

	send_to_char( "Nagle otacza ciê iskrz±ca siê, bia³a po¶wiata.\n\r", victim );
	act( "$n otacza siê iskrz±c±, bia³± po¶wiat±.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_divine_power( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int mod_str, mod_hp, duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( is_affected( victim, sn ) )
	{
		send_to_char( "Ju¿ jeste¶ obdarzon<&y/a/e> bosk± moc±.\n\r", ch );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê zdobyæ boskiej mocy.\n\r", ch );
		return;
	}

	mod_str = UMAX( 0, 24 - get_curr_stat_deprecated( victim, STAT_STR ) );
	mod_hp = number_range( 30, 45 ) * get_max_hp( victim ) / 100;

	if ( mod_str > 0 && number_range( 0, luck + LUCK_BASE_MOD ) > 2 )
	{
		duration = 5 + level / 3;
		if ( number_range( 0, luck ) > 15 ) duration += 1;
		if ( number_range( 0, luck ) < 5 ) duration -= 1;

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = UMAX( 1, duration ); af.rt_duration = 0;
		af.location = APPLY_STR;
		af.modifier = mod_str;
		af.bitvector = &AFF_NONE;
		affect_to_char( victim, &af, NULL, TRUE );
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) > 2 )
	{
		duration = 5 + level / 3;
		if ( number_range( 0, luck ) > 15 ) duration += 1;
		if ( number_range( 0, luck ) < 5 ) duration -= 1;

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = UMAX( 1, duration ); af.rt_duration = 0;
		af.location = APPLY_HIT;
		af.modifier = mod_hp;
		af.bitvector = &AFF_NONE;
		affect_to_char( victim, &af, NULL, TRUE );
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) > 2 )
		victim->hit += mod_hp;

	if ( number_range( 0, luck + LUCK_BASE_MOD ) > 2 )
	{
		duration = 5 + level / 3;
		if ( number_range( 0, luck ) > 15 ) duration += 1;
		if ( number_range( 0, luck ) < 5 ) duration -= 1;

		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = level;
		af.duration = UMAX( 1, duration ); af.rt_duration = 0;
		af.location = APPLY_HITROLL;
		af.modifier = level / 3;
		af.bitvector = &AFF_NONE;
		affect_to_char( victim, &af, NULL, TRUE );
	}

	if ( is_affected( victim, sn ) )
	{
		send_to_char( "Czujesz jak zaczyna rozpieraæ ciê potê¿na energia, przez chwile wydaje ci siê, ¿e twoje cia³o eksploduje.\n\r", victim );
		act( "$n zaczyna promieniowaæ bia³ym ¶wiat³em.", victim, NULL, NULL, TO_ROOM );
	}
	else
		send_to_char( "Nie uda³o ci zdobyæ boskiej mocy.\n\r", ch );

	return;
}

void spell_bane( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( is_affected( victim, sn ) )
	{
		if ( ch == victim )
			send_to_char( "Ju¿ jeste¶ przeklêt<&y/a/e>.\n\r", ch );
		else
			switch ( victim->sex )
			{
				case 0:
					act( "$N jest ju¿ przeklête.", ch, NULL, victim, TO_CHAR );
					break;
				case 1:
					act( "$N jest ju¿ przeklêty.", ch, NULL, victim, TO_CHAR );
					break;
				default:
					act( "$N jest ju¿ przeklêta.", ch, NULL, victim, TO_CHAR );
					break;
			}
		return;
	}

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) && number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		act( "Nie uda³o ci siê przekln±æ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	duration = 2 + level / 6;
	if ( number_range( 0, luck ) > 15 ) duration += 1;
	if ( number_range( 0, luck ) < 5 ) duration /= 2;

	if ( ch != victim )
		luck = get_curr_stat_deprecated( victim, STAT_LUC );

	if ( number_range( 0, luck ) > 15 ) duration--;
	if ( number_range( 0, luck ) < 5 ) duration++;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, duration ); af.rt_duration = 0;
	af.location = APPLY_HITROLL;
	af.modifier = -1;
	af.bitvector = &AFF_NONE;

	affect_to_char( victim, &af, NULL, TRUE );

	af.location  = APPLY_SAVING_SPELL;
	af.modifier  = 1;
	affect_to_char( victim, &af, NULL, TRUE  );

	if ( ch != victim )
	{
		act( "$n okrywa $C aur± przekleñstwa.", ch, NULL, victim, TO_NOTVICT );
		act( "Okrywasz siê aur± przekleñstwa.", ch, NULL, victim, TO_VICT );
		act( "Okrywasz $C aur± przekleñstwa.", ch, NULL, victim, TO_CHAR );
	}
	else
	{
		act( "$n okrywa siê aur± przekleñstwa.", ch, NULL, NULL, TO_NOTVICT );
		act( "Okrywasz siê aur± przekleñstwa.", ch, NULL, NULL, TO_CHAR );
	}

	return;
}

//metalowe graty nagrzewaja siê i zadaja dam
//npc same zdejmuja graty, gracze recznie musza
//dam zalezny od czesci pancerza, to dalej, co runde walki spradzane
//nagrzanie moze zniszczyc metale slabo odporne na cieplo
//tutaj tylko nakladamy affect na obiekt[y]
void spell_heat_metal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj, *objects[ 10 ];
	int items_heated = 0, items, i;
	AFFECT_DATA aff, *pAff, *af;

	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;

		if ( !IS_SET( material_table[ obj->material ].flag, MAT_METAL ) ||
		     IS_OBJ_STAT( obj, ITEM_NOMAGIC ) ||
		     ( IS_OBJ_STAT( obj, ITEM_BLESS ) && number_percent() < 75 ) ||
		     ( IS_OBJ_STAT( obj, ITEM_MAGIC ) && number_percent() < 25 ) ||
		     ( obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_WEAPON ) )
			return;

		if ( ( pAff = affect_find( obj->affected, gsn_heat_metal ) ) )
		{
			act( "Ten przedmiot jest ju¿ wystarczajaco rozgrzany.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( ( pAff = affect_find( obj->affected, gsn_chill_metal ) ) )
		{
			act( "Szron pokrywajacy $p znika.", ch, obj, NULL, TO_CHAR );
			affect_remove_obj( obj, pAff );
			return;
		}

		aff.where	= TO_OBJECT;
		aff.type	= sn;
		aff.level	= level;
		aff.duration = 2 + level / 7.5; aff.rt_duration = 0;
		aff.location	= APPLY_NONE;
		aff.modifier	= 0;
		aff.bitvector	= &AFF_NONE;

		affect_to_obj( obj, &aff );

		act( "$p rozgrzewaj± siê do czerwono¶ci.", ch, obj, NULL, TO_ALL );
		return;
	}

	victim = ( CHAR_DATA * ) vo;

	//rzut obronny - ale ofiara tylko
	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		return;

	for ( i = 0; i < 10; i++ )
		objects[ i ] = NULL;

	items_heated = number_range( 1, 1 + level / 10 );

	for ( items = 0, obj = victim->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE )
			continue;

		switch ( obj->wear_loc )
		{
			case WEAR_FEET:
			case WEAR_LEGS:
			case WEAR_BODY:
			case WEAR_HEAD:
			case WEAR_SHIELD:
			case WEAR_ARMS:
			case WEAR_HANDS:
			case WEAR_WIELD:
			case WEAR_SECOND: break;
			default: continue;
		}

		if ( !IS_SET( material_table[ obj->material ].flag, MAT_METAL ) )
			continue;

		if ( IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
			continue;

		if ( IS_OBJ_STAT( obj, ITEM_BLESS ) && number_percent() < 75 )
			continue;

		if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) && number_percent() < 25 )
			continue;

		if ( obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_WEAPON )
			continue;

		objects[ items++ ] = obj;

		if ( items >= 10 )
			break;
	}

	if ( items == 0 )
		return;

	//affect nakladany
	aff.where	= TO_OBJECT;
	aff.type	= sn;
	aff.level	= level;
	aff.duration = 2 + level / 7.5; aff.rt_duration = 0;
	aff.location	= APPLY_NONE;
	aff.modifier	= 0;
	aff.bitvector	= &AFF_NONE;

	for ( i = 0; i < items_heated; i++ )
	{
		obj = objects[ number_range( 0, items - 1 ) ];

		if ( affect_find( obj->affected, sn ) )
			continue;

		affect_to_obj( obj, &aff );

		switch ( obj->wear_loc )
		{
			case WEAR_FEET:
			case WEAR_LEGS:
			case WEAR_HANDS:
			case WEAR_ARMS:
				act( "$p rozgrzewaj± siê do czerwono¶ci.", ch, obj, NULL, TO_ALL );
				break;
			default:
				act( "$p rozgrzewa siê do czerwono¶ci.", ch, obj, NULL, TO_ALL );
				break;
		}

		if ( IS_NPC( victim ) )
		{
			if ( !IS_AFFECTED( victim, AFF_PARALYZE ) && victim->position > POS_SLEEPING )
			{
				if ( !IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
				{
					if ( obj->item_type == ITEM_ARMOR )
						act( "$n jêczy i w wielkim po¶piechu zdejmuje $h!", victim, obj, NULL, TO_ROOM );
					else if ( obj->item_type == ITEM_WEAPON )
						act( "$p wypada $x z d³oni parz±c j±.", victim, obj, NULL, TO_ROOM );
					unequip_char( victim, obj );
				}
			}
			else if ( number_percent() > 85 )
			{
				for ( af = victim->affected; af != NULL; af = af->next )
				{
					if ( af->bitvector == &AFF_PARALYZE )
					{
						affect_strip( victim, af->type );
						if ( !IS_NPC( victim ) )
						{
							send_to_char( skill_table[ sn ].msg_off, victim );
							send_to_char( "\n\r", victim );
						}
					}
				}
				act( "Z powodu bólu, $n odzyskuje kontrolê nad swoim cia³em.", victim, NULL, NULL, TO_ROOM );
			}
		}
		else if ( victim->position > POS_SLEEPING )
		{
			act( "$p pali twoj± skórê!", victim, obj, NULL, TO_CHAR );
			if ( number_percent() > 0 && IS_AFFECTED( victim, AFF_PARALYZE ) )
			{
				act( "Z powodu bólu, $n odzyskuje kontrolê nad swoim cia³em.", victim, NULL, NULL, TO_ROOM );
				send_to_char( "Z powodu bólu, odzyskujesz kontrolê nad swoim cia³em.\n\r", ch );
				EXT_REMOVE_BIT( victim->affected_by, AFF_PARALYZE );
			}
		}

		if ( items == 1 )
			break;
	}
}

void spell_chill_metal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj, *objects[ 10 ];
	int items_heated = 0, items, i;
	AFFECT_DATA aff, *pAff;

	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;

		if ( !IS_SET( material_table[ obj->material ].flag, MAT_METAL ) ||
		     IS_OBJ_STAT( obj, ITEM_NOMAGIC ) ||
		     ( IS_OBJ_STAT( obj, ITEM_BLESS ) && number_percent() < 75 ) ||
		     ( IS_OBJ_STAT( obj, ITEM_MAGIC ) && number_percent() < 25 ) ||
		     ( obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_WEAPON ) )
			return;

		if ( ( pAff = affect_find( obj->affected, gsn_chill_metal ) ) )
		{
			act( "Ten przedmiot jest ju¿ lodowato zimny.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( ( pAff = affect_find( obj->affected, gsn_heat_metal ) ) )
		{
			act( "$p przestaje ¶wiecic g³êbok± czerwienia i wraca do normalnej temperatury.", ch, obj, NULL, TO_CHAR );
			affect_remove_obj( obj, pAff );
			return;
		}

		aff.where	= TO_OBJECT;
		aff.type	= sn;
		aff.level	= level;
		aff.duration = 2 + level / 7.5; aff.rt_duration = 0;
		aff.location	= APPLY_NONE;
		aff.modifier	= 0;
		aff.bitvector	= &AFF_NONE;

		affect_to_obj( obj, &aff );

		act( "$p pokrywa siê grub± warstwa szronu.", ch, obj, NULL, TO_ALL );
		return;
	}

	victim = ( CHAR_DATA * ) vo;

	//rzut obronny - ale ofiara tylko
	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		return;

	for ( i = 0; i < 10; i++ )
		objects[ i ] = NULL;

	items_heated = number_range( 1, 1 + level / 10 );

	for ( items = 0, obj = victim->carrying; obj != NULL; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE )
			continue;

		switch ( obj->wear_loc )
		{
			case WEAR_FEET:
			case WEAR_LEGS:
			case WEAR_BODY:
			case WEAR_HEAD:
			case WEAR_SHIELD:
			case WEAR_ARMS:
			case WEAR_HANDS:
			case WEAR_WIELD:
			case WEAR_SECOND: break;
			default: continue;
		}

		if ( !IS_SET( material_table[ obj->material ].flag, MAT_METAL ) )
			continue;

		if ( IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
			continue;

		if ( IS_OBJ_STAT( obj, ITEM_MAGIC ) && number_percent() < 25 )
			continue;

		if ( obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_WEAPON )
			continue;

		objects[ items++ ] = obj;

		if ( items >= 10 )
			break;
	}

	if ( items == 0 )
		return;

	//affect nakladany
	aff.where	= TO_OBJECT;
	aff.type	= sn;
	aff.level	= level;
	aff.duration = 2 + level / 7.5; aff.rt_duration = 0;
	aff.location	= APPLY_NONE;
	aff.modifier	= 0;
	aff.bitvector	= &AFF_NONE;

	for ( i = 0; i < items_heated; i++ )
	{
		obj = objects[ number_range( 0, items - 1 ) ];

		if ( affect_find( obj->affected, sn ) )
			continue;

		affect_to_obj( obj, &aff );

		switch ( obj->wear_loc )
		{
			case WEAR_FEET:
			case WEAR_LEGS:
			case WEAR_HANDS:
			case WEAR_ARMS:
				act( "$p pokrywaj± siê grub± warstw± szronu.", ch, obj, NULL, TO_ALL );
				break;
			default:
				act( "$p pokrywa siê grub± warstw± szronu.", ch, obj, NULL, TO_ALL );
				break;
		}

		if ( IS_NPC( victim ) )
		{
			if ( obj->item_type == ITEM_ARMOR )
				act( "$n jêczy i w wielkim po¶piechu zdejmuje $h!", victim, obj, NULL, TO_ROOM );
			else if ( obj->item_type == ITEM_WEAPON )
				act( "$p wypada $x z d³oni.", victim, obj, NULL, TO_ROOM );

			if ( !IS_OBJ_STAT( obj, ITEM_NOREMOVE ) )
				unequip_char( victim, obj );
		}
		else
			act( "$p zamra¿a twoj± skórê!", victim, obj, NULL, TO_CHAR );

		if ( items == 1 )
			break;
	}
}

void spell_mass_healing( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * gch;
	int heal_value, members = 0, refresh_value;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê uzdrowiæ swych przyjació³.\n\r", ch );
		return;
	}

	heal_value = number_range( 70, 90 ) + level;
	if ( number_range( 0, luck ) < 3 ) heal_value /= 2;
	if ( number_range( 0, luck ) > 15 ) heal_value *= ( number_range( 100, 110 ) ) / 100;

	refresh_value = number_range( 25, 40 ) + level / 2;
	if ( number_range( 0, luck ) < 3 ) refresh_value /= 2;

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( is_same_group( ch, gch ) )
		{
			members++;
			if ( ( ch->class == CLASS_CLERIC ) && ( gch->hit == get_max_hp( gch ) ) )
				members--;
		}
	}

	if ( members > 8 )
	{
		heal_value /= 3;
		refresh_value /= 3;
	}
	else if ( members > 6 )
	{
		heal_value /= 2;
		refresh_value /= 2;
	}
	else if ( members > 4 )
	{
		heal_value -= heal_value / 4;
		refresh_value -= refresh_value / 4;
	}

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
		if ( is_same_group( ch, gch ) )
		{
			heal_char( ch, gch, heal_value, FALSE );
			send_to_char( "Czujesz pote¿n± energie wype³niaj±c± twoje cia³o.\n\r", gch );

			if ( gch->move != gch->max_move )
			{
				gch->move = UMIN( gch->move + refresh_value, gch->max_move );

				if ( gch->max_move == gch->move )
					send_to_char( "Zmêczenie ca³kowicie ustêpuje.\n\r", gch );
				else
					send_to_char( "Czujesz jak zmêczenie powoli ustêpuje.\n\r", gch );
			}
		}
}

void spell_luck( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	if ( is_affected( victim, gsn_luck ) )
	{
		if ( victim == ch )
			send_to_char( "Nie mo¿esz mieæ wiêcej szczê¶cia!\n\r", ch );
		else
			act( "$N nie mo¿e mieæ wiêcej szczê¶cia.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_affected( victim, gsn_misfortune ) )
	{
		act( "Otaczaj±ca $x {yzgni³o{x-{Gzielona{x aura nieszczê¶cia zanika.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Otaczaj±ca ciê {yzgni³o{x-{Gzielona{x aura nieszczê¶cia zanika.\n\r", victim );
		affect_strip( victim, gsn_misfortune );
		return;
	}

	af.where = TO_AFFECTS;
	af.type = gsn_luck;
	af.level = level;
	if ( ch->class == CLASS_PALADIN )
	{
		af.duration = level / 6; af.rt_duration = 0;
		af.modifier = URANGE( 1, level / 10, 3 );
	}
	else
	{
		af.duration = 1 + level / 4; af.rt_duration = 0;
		af.modifier = UMAX( 1, level / 10 );
	}
	af.location = APPLY_LUC;
	af.bitvector = &AFF_NONE;

	affect_to_char( victim, &af, NULL, TRUE );
	send_to_char( "Czujesz, ¿e szczê¶cie zaczyna ci sprzyjaæ!\n\r", victim );
	act( "Dooko³a $z rozb³yska {Gzielono{x-{Ys³oneczna{x otoczka szczê¶cia!", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_create_symbol( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = NULL;
	int value;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	AFFECT_DATA af;

	if ( is_affected( ch, gsn_create_symbol ) )
	{
		send_to_char( "Dzisiaj bogowie nie wys³uchaj± twoich modlitw.\n\r", ch );
		return;
	}

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ magicznego symbolu.\n\r", ch );
		return;
	}

	if ( ch->alignment > 250 )
	{
		obj = create_object( get_obj_index( OBJ_VNUM_CREATE_SYMBOL_GOOD ), FALSE );
	}
	else if ( ch->alignment < 250 && ch->alignment > -250 )
	{
		obj = create_object( get_obj_index( OBJ_VNUM_CREATE_SYMBOL_NEUTRAL ), FALSE );
	}
	else if ( ch->alignment < -250 )
	{
		obj = create_object( get_obj_index( OBJ_VNUM_CREATE_SYMBOL_EVIL ), FALSE );
	}

	if ( !obj )
	{
		send_to_char( "W³a¶ciwie to nic siê nie dzieje.\n\r", ch );
		bug( "Brak obiektu: OBJ_VNUM_CREATE_SYMBOL_*.", 0 );
		return;
	}

	value = dice( 2, 4 ) + level / 7.5;
	if ( number_range( 0, luck ) > 15 ) value += 2;
	if ( number_range( 0, luck ) < 5 ) value -= 3;
	obj->timer = URANGE( 3, value, 6 );

	value = 2 * level;
	if ( number_range( 0, luck ) > 15 ) value = ( value * 9 ) / 10;
	if ( number_range( 0, luck ) < 5 ) value = ( value * 115 ) / 100;
	obj->rent_cost = value;

	value = level / 7.5;
	if ( number_range( 0, luck ) > 15 ) ++value;
	if ( number_range( 0, luck ) < 5 ) --value;

	EXT_SET_BIT( obj->extra_flags, ITEM_GLOW );
	EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );

	switch ( UMAX( 0, value ) )
	{
		case 0:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			break;
		case 1:
			obj->value[ 1 ] = 3;
			obj->value[ 2 ] = 5;
			break;
		case 2:
			obj->value[ 1 ] = 5;
			obj->value[ 2 ] = 7;
			break;
		case 3:
			obj->value[ 1 ] = 7;
			obj->value[ 2 ] = 10;
			break;
		default:
			obj->value[ 1 ] = 10;
			obj->value[ 2 ] = 15;
			break;
	}

	obj_to_char( obj, ch );

	af.where = TO_AFFECTS;
	af.type = gsn_create_symbol;
	af.level = 0;
	af.duration = 48; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	affect_to_char( ch, &af, NULL, FALSE );

	send_to_char( "W twoim inwentarzu pojawia siê symbol wiary.\n\r", ch );
	return;
}

/*
 * Zwiêksza timer, zanim co¶ siê stanie trucizn±
 */
void spell_slow_rot( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * vobj;
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int count = 0;
	int food = 0;

	if ( level < 15 && ( obj->item_type != ITEM_FOOD || obj->carried_by != ch ) )
	{
		send_to_char( "Czar mo¿esz rzuciæ na pojedyncz± sztukê jedzenia znajduj±c± siê w twoim inwentarzu.\n\r", ch );
		return;
	}

	if ( level >= 15 && ( ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_CONTAINER ) || obj->carried_by != ch ) )
	{
		send_to_char( "Czar mo¿esz rzuciæ na pojedyncz± sztukê jedzenia lub pojemnik zawieraj±cy jedzenie.\n\r", ch );
		return;
	}

	//pojedynczy food
	if ( obj->item_type == ITEM_FOOD )
	{
		if ( obj->value[ 3 ] )
		{
			act( "$p ¶mierdzi zgnilizn±, niczego nie da siê zrobiæ.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( obj->value[ 1 ] == 0 )
		{
			act( "Przecie¿ $p nie mo¿e zgniæ.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( obj->value[ 1 ] > 7 * level )
		{
			act( "Nie dasz rady bardziej spowolniæ gnicia $f.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( number_range( 0, luck ) > 1 )
		{
			obj->value[ 1 ] = 7 + 7 * level;
			act( "Spowalniasz procesy gnilne zachodz±ce w $k.", ch, obj, NULL, TO_CHAR );
			act( "$n spowalnia procesy gnilne zachodz±ce w $k.", ch, obj, NULL, TO_ROOM );
		}
		else
			act( "Nie uda³o ci siê spowolniæ procesów gnilnych zachodz±cych w $k.", ch, obj, NULL, TO_CHAR );

		if ( number_range( 0, luck ) < 2 )
			obj->value[ 1 ] = UMAX( 1, obj->value[ 1 ] - 10 );

		return;
	}

	for ( vobj = obj->contains; vobj; vobj = vobj->next_content )
	{
		if ( vobj->item_type != ITEM_FOOD )
			continue;

		food++;

		if ( vobj->value[ 3 ] || vobj->value[ 1 ] == 0 ||
		     vobj->value[ 1 ] > 7 * level || number_range( 0, luck ) <= 3 )
			continue;

		count++;

		vobj->value[ 1 ] = 7 + 7 * level;

		if ( number_range( 0, luck ) < 4 )
			vobj->value[ 1 ] = UMAX( 1, vobj->value[ 1 ] - 10 );
	}

	if ( food == 0 )
		act( "W $k nie ma ¿adnego jedzenia.", ch, obj, NULL, TO_CHAR );
	else
	{
		if ( count == 0 )
			act( "Nie uda³o ci siê spowolniæ procesów gnilnych zachodz±cych w jedzeniu znajduj±cym siê w $k.", ch, obj, NULL, TO_CHAR );
		else
		{
			act( "Spowalniasz procesy gnilne zachodz±ce w jedzeniu znajduj±cym siê w $k.", ch, obj, NULL, TO_CHAR );
			act( "$n spowalnia procesy gnilne zachodz±ce w jedzeniu znajduj±cym siê w $k.", ch, obj, NULL, TO_ROOM );
		}
	}
	return;
}

/*
 * zmiania si³ê jednej wybranej osoby z grupy, kosztem pozosta³ych
 */
void spell_champions_strength( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	CHAR_DATA *vch;
	int count = 0, count2 = 0, count3 = 0;

	if ( victim == ch )
	{
		send_to_char( "Nie dasz rady rzuciæ tego zaklêcia na siebie.\n\r", ch );
		return;
	}

	if ( !is_same_group( ch, victim ) )
	{
		send_to_char( "To siê nie uda, musisz wybraæ kogo¶ ze swojej grupy.\n\r", ch );
		return;
	}

	if ( is_affected( ch, sn ) )
	{
		send_to_char( "Przecie¿ dzielisz ju¿ z kim¶ swoj± si³ê.\n\r", ch );
		return;
	}

	if ( is_affected( victim, sn ) )
	{
		act( "Z $V ju¿ siê kto¶ podzieli³ swoj± si³±.", ch, NULL, victim, TO_CHAR );
		return;
	}

    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
        if (
                IS_NPC( vch ) ||
                !is_same_group( vch, ch ) ||
                is_affected( vch, sn ) ||
                check_improve_strenth ( ch, vch, FALSE )
           )
        {
            continue;
        }

        if ( vch != victim && !IS_NPC( vch ) )
        {
            count++;
        }
    }

	count = UMIN( count, level / 6 );

	count2 = count3 = 1;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level	= level;
	af.duration = 10 + level / 6; af.rt_duration = 0;
	af.location = APPLY_STR;
	af.modifier = -1;
	af.bitvector = &AFF_NONE;
	affect_to_char( ch, &af, NULL, TRUE );

	switch ( ch -> sex )
	{
		case 0:
			send_to_char( "Czujesz siê nieco s³absze.\n\r", ch );
			act( "$n wygl±da na nieco s³absze.", ch, NULL, NULL, TO_ROOM );
			break;
		case 1:
			send_to_char( "Czujesz siê nieco s³abszy.\n\r", ch );
			act( "$n wygl±da na nieco s³abszego.", ch, NULL, NULL, TO_ROOM );
			break;
		default:
			send_to_char( "Czujesz siê nieco s³absza.\n\r", ch );
			act( "$n wygl±da na nieco s³absz±.", ch, NULL, NULL, TO_ROOM );
			break;
	}

	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
	{
		if ( IS_NPC( vch ) )
			continue;

		if ( !is_same_group( vch, ch ) )
			continue;

		if ( is_affected( vch, sn ) )
			continue;

		if ( vch != victim && count2 < count )
		{
			af.where = TO_AFFECTS;
			af.type = sn;
			af.level	= level;
			af.duration = 10 + level / 6; af.rt_duration = 0;
			af.location = APPLY_STR;
			af.modifier = -1;
			af.bitvector = &AFF_NONE;
			affect_to_char( vch, &af, NULL, TRUE );

			switch ( vch -> sex )
			{
				case 0:
					send_to_char( "Czujesz siê nieco s³absze.\n\r", vch );
					act( "$n wygl±da na nieco s³absze.", vch, NULL, NULL, TO_ROOM );
					break;
				case 1:
					send_to_char( "Czujesz siê nieco s³abszy.\n\r", vch );
					act( "$n wygl±da na nieco s³abszego.", vch, NULL, NULL, TO_ROOM );
					break;
				default:
					send_to_char( "Czujesz siê nieco s³absza.\n\r", vch );
					act( "$n wygl±da na nieco s³absz±.", vch, NULL, NULL, TO_ROOM );
					break;
			}
			count3++;
		}
		count2++;
	}

	if ( !( count3 > 0 ) )
		return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level	= level;
	af.duration = 10 + level / 6; af.rt_duration = 0;
	af.location = APPLY_STR;
	af.modifier = ( count3 * 4 ) / 3;
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, NULL, TRUE );

	send_to_char( "Czujesz siê zbiera siê w tobie si³a ca³ej grupy.\n\r", victim );

	switch ( count3 )
	{
		case 1:
			act( "$n puszy nieco swoje miê¶nie.", victim, NULL, NULL, TO_ROOM );
			break;
		case 2:
			act( "$n napina swoje miê¶nie.", victim, NULL, NULL, TO_ROOM );
			break;
		case 3:
			act( "$n mocno napina swoje miê¶nie.", victim, NULL, NULL, TO_ROOM );
			break;
		case 4:
			act( "$n potê¿nie napina swoje miê¶nie.", victim, NULL, NULL, TO_ROOM );
			break;
		case 5:
			act( "$n niewyobra¿alnie napina swoje miê¶nie.", victim, NULL, NULL, TO_ROOM );
			break;
		default:
			act( "$n pokazuje swoje miê¶nie.", victim, NULL, NULL, TO_ROOM );
			break;
	}

	return;
}

/* RT calm spell stops all fighting in the room */

void spell_calm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * vch;
	int mlevel = 0;
	int count = 0;
	int high_level = 0;
	int chance;
	AFFECT_DATA af;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( ch->position != POS_FIGHTING && !ch->fighting )
	{
		send_to_char( "Przecie¿ z nikim nie walczysz.\n\r", ch );
		return;
	}

	/* get sum of all mobile levels in the room */
	for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
	{
		if ( vch != ch && vch->position == POS_FIGHTING )
		{
			count++;
			if ( IS_NPC( vch ) )
				mlevel += vch->level;
			else
				mlevel += vch->level / 2;
			high_level = UMAX( high_level, vch->level );
		}
	}

	/* compute chance of stopping combat */
	chance = UMAX( 1, 4 * level - high_level - 2 * count );

	if ( number_range( 0, luck ) > 15 ) chance = ( 11 * chance ) / 10;
	if ( number_range( 0, luck ) < 5 ) chance = chance / 2;

	chance = UMAX( chance, mlevel + 1 );

	if ( IS_IMMORTAL( ch ) )     /* always works */
		mlevel = 0;

	if ( number_range( 0, chance ) >= mlevel )      /* hard to stop large fights */
	{
		for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
		{
			if ( !vch->fighting && vch->position != POS_FIGHTING )
				continue;

			if ( is_same_group( vch, ch ) )
			{
				if ( vch->fighting || vch->position == POS_FIGHTING )
					stop_fighting( vch, FALSE );
				continue;
			}

			if ( is_undead( vch ) )
			{
				act( "$N nic sobie nie robi z twoich wysi³ków.", ch, NULL, vch, TO_CHAR );
				continue;
			}

			if ( IS_AFFECTED( vch, AFF_CALM ) )
			{
				send_to_char( "Tym razem to siê chyba nie uda.\n\r", ch );
				continue;
			}

			if ( IS_AFFECTED( vch, AFF_BERSERK ) || is_affected( vch, skill_lookup( "frenzy" ) ) )
			{
				print_char( ch, "Nie dasz rady uspokoiæ %s.\n\r", vch->name2 );
				continue;
			}

			if ( vch->fighting || vch->position == POS_FIGHTING )
			{
				if ( !IS_NPC( vch ) )
					send_to_char( "Fala spokoju przep³ywa przez twoje cia³o.\n\r", vch );

				act( "Przez $c przep³ywa fala spokoju.", vch, NULL, NULL, TO_ROOM );

				af.where = TO_AFFECTS;

				af.type = sn;
				af.level = level;
				af.duration = level / 4; af.rt_duration = 0;
				af.bitvector = &AFF_CALM;
				af.modifier = -2;

				if ( !IS_NPC( vch ) )
					af.modifier = -5;

				af.location = APPLY_HITROLL;
				affect_to_char( vch, &af, NULL, TRUE );

				af.location = APPLY_DAMROLL;
				affect_to_char( vch, &af, NULL, TRUE );

				stop_hating( vch, vch->fighting, FALSE );
				stop_hunting( vch );

				vch->fighting = NULL;
				vch->position = POS_STANDING;

			}
		}
		stop_fighting( ch, FALSE );
	}
	else
		send_to_char( "Nie uda³o ci siê nikogo uspokoiæ.\n\r", ch );
}

void spell_curse( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj;
	AFFECT_DATA af;

	/* deal with the object case first */
	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;
		if ( IS_OBJ_STAT( obj, ITEM_EVIL ) )
		{
			act( "$p jest ju¿ wype³niony z³em.", ch, obj, NULL, TO_CHAR );
			return;
		}

		if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
		{
			AFFECT_DATA * paf;

			paf = affect_find( obj->affected, gsn_bless );
			if ( !saves_dispel( level, paf != NULL ? paf->level :   /* obj->level*/1, 0 ) )
			{
				if ( paf != NULL )
					affect_remove_obj( obj, paf );
				act( "$p ¶wieci czerwon± aur±.", ch, obj, NULL, TO_ALL );
				EXT_REMOVE_BIT( obj->extra_flags, ITEM_BLESS );
				return;
			}
			else
			{
				act( "¦wieta aura $f jest zbyt silna ¿eby j± pokonaæ.", ch, obj, NULL, TO_CHAR );
				return;
			}
		}

		af.where = TO_OBJECT;
		af.type = sn;
		af.level = level;
		af.duration = -1; af.rt_duration = 0; /* a co :)*/
		/*2 * level;*/
		af.location = APPLY_SAVING_SPELL;
		af.modifier = + 1;
		af.bitvector = &ITEM_EVIL;
		affect_to_obj( obj, &af );

		act( "$p ¶wieci przeklêt± aur±.", ch, obj, NULL, TO_ALL );
		return;
	}

	/* character curses */
	victim = ( CHAR_DATA * ) vo;

	if ( IS_AFFECTED( victim, AFF_CURSE ) || saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
		return;

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = -1; af.rt_duration = 0;
	/*    2*level;*/
	af.location = APPLY_HITROLL;
	af.modifier = -1 * ( level / 7.5 );
	af.bitvector = &AFF_CURSE;
	affect_to_char( victim, &af, NULL, TRUE );

	af.location = APPLY_SAVING_SPELL;
	af.modifier = level / 7.5;
	affect_to_char( victim, &af, NULL, TRUE );

	switch ( victim->sex )
	{
		case 0:
			send_to_char( "Czujesz siê nieczyste.\n\r", victim );
			break;
		case 1:
			send_to_char( "Czujesz siê nieczysty.\n\r", victim );
			break;
		default:
			send_to_char( "Czujesz siê nieczysta.\n\r", victim );
			break;
	}
	if ( ch != victim )
		act( "$N wygl±da do¶æ nieprzyjemnie.", ch, NULL, victim, TO_CHAR );
	return;
}

void spell_detect_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = ( OBJ_DATA * ) vo;
	if ( obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD )
	{
		if ( obj->value[ 3 ] != 0 )
			send_to_char( "Czujesz tu zapach trucizny.\n\r", ch );
		else
			send_to_char( "To wygl±da normalnie.\n\r", ch );
	}
	else if ( obj->item_type == ITEM_WEAPON )
	{
		if ( IS_SET( obj->value[ 4 ], WEAPON_POISON ) )
			print_char( ch, "Na %s znajdujesz ciemne plamy i wiesz, ¿e ta broñ jest zatruta.\n\r", obj->name5 );
		else
			send_to_char( "To nie wygl±da na zatrute.\n\r", ch );
	}
	else
		send_to_char( "To nie wygl±da na zatrute.\n\r", ch );
	return;
}

/* RT recall spell is back */
void spell_word_of_recall( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	ROOM_INDEX_DATA *location;
	unsigned short int last_rent;

	if ( IS_NPC( victim ) )
		return;

	if ( ( last_rent = victim->pcdata->last_rent ) <= 0 )
		return;

	if ( ( location = get_room_index( last_rent ) ) == NULL )
		return;

	if ( EXT_IS_SET( victim->in_room->room_flags, ROOM_NO_RECALL ) || EXT_IS_SET( victim->in_room->room_flags, ROOM_NOMAGIC ) )
		return;

	if ( victim->fighting != NULL )
		stop_fighting( victim, TRUE );

	act( "$n znika.", victim, NULL, NULL, TO_ROOM );
	char_from_room( victim );
	char_to_room( victim, location );
	act( "$n pojawia siê tu.", victim, NULL, NULL, TO_ROOM );
	do_function( victim, &do_look, "auto" );

	return;
}

/* Uzdrowienie (skladnia: cast heal [kto])
 */

void spell_life_transfer( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int luck, dam, heal;

	if ( victim == ch )
	{
		send_to_char( "Nie dasz rady rzuciæ tego zaklêcia na siebie.\n\r", ch );
		return;
	}

	if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		act( "$n nie jest dobrym celem do przeniesienia swojego ¿ycia.", ch, victim, NULL, TO_CHAR );
		return;
	}

	if ( victim->hit == get_max_hp( victim ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N wygl±da na ca³kiem zdrowe.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N wygl±da na ca³kiem zdrowego.", ch, NULL, victim, TO_CHAR );
				break;
			default:
				act( "$N wygl±da na ca³kiem zdrow±.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	luck = get_curr_stat_deprecated( ch, STAT_LUC );
	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê.\n\r", ch );
		return;
	}

	if ( is_affected( ch, gsn_life_transfer ) )
	{
		send_to_char( "Na razie nie masz do¶æ si³ witalnych.\n\r", ch );
		return;
	}

	// taki wzór daje
	// 40% ran casterowi na 20 levelu
	// 26% ran casterowi na 32 levelu
	// 20% ran casterowi na 40 levelu
	heal = get_max_hp( victim ) - victim->hit;
	dam = 8 * heal / level;

	if ( dam > ch->hit )
	{
		heal = ch->hit * level / 8;
		heal_char( ch, victim, heal, FALSE );
	}
	else
	{
		victim->hit = get_max_hp( victim );
		send_to_char( "Twoje cia³o wype³nia lecznicze ciep³o, wszystkie twoje rany goj± siê.\n\r", victim );
		act( "Wszystkie rany $z goj± siê.", victim, NULL, NULL, TO_ROOM );
	}

	if ( is_affected( victim, gsn_feeblemind ) )
		affect_strip( victim, gsn_feeblemind );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level	= 50;
	af.duration = 6; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	affect_to_char( ch, &af, NULL, TRUE );

	act( "Czujesz jak twoje ¿ycie przep³ywa do $Z.", ch, NULL, victim, TO_CHAR );
	spell_damage( ch, ch, dam, sn, DAM_HARM, FALSE );

	return;
}

void spell_mass_bless( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	CHAR_DATA *gch;
	int duration = 20, luck = get_curr_stat_deprecated( ch, STAT_LUC ), count = 0;
/*
	if ( ch->position == POS_FIGHTING || ch->fighting )
	{
		send_to_char( "Nie mo¿esz siê skoncentrowaæ.\n\r", ch );
		return;
	}
*/
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
		if ( !is_same_group( gch, ch ) || is_affected( gch, gsn_bless ) )
			continue;

		if ( gch->class == CLASS_BLACK_KNIGHT )
			continue;

		af.where = TO_AFFECTS;
		af.type = gsn_bless;
		af.level = level;
		af.duration = 2 + level / 7.5; af.rt_duration = 0;
		af.location = APPLY_HITROLL;
		af.modifier = 1 + ( level > 15 ? 1 : 0 );
		af.bitvector = &AFF_NONE;

		affect_to_char( gch, &af, NULL, TRUE );

		af.location = APPLY_SAVING_SPELL;
		af.modifier = - 1 - ( level > 15 ? 1 : 0 );
		affect_to_char( gch, &af, NULL, TRUE );

		act( "Na $c sp³ywa boskie b³ogos³awieñstwo.", gch, NULL, NULL, TO_ROOM );
		if ( gch == ch ) send_to_char( "Sam<&/a/o> siebie b³ogos³awisz.\n\r", ch );
		else act( "Za po¶rednictwem $z sp³ywa na ciebie boskie b³ogos³awieñstwo.", ch, NULL, gch, TO_VICT );

		count++;
	}

	if ( count > 0 )
		send_to_char( "Uda³o ci siê sprowadziæ b³ogos³awieñstwo.\n\r", ch );
	else
		send_to_char( "Uda³o ci siê sprowadziæ b³ogos³awieñstwo, ale nikt go nie potrzebowa³.\n\r", ch );

	return;
}

void spell_mass_luck( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	AFFECT_DATA af;
	CHAR_DATA *gch;
	int count = 0;
/*
	if ( ch->position == POS_FIGHTING || ch->fighting )
	{
		send_to_char( "Nie mo¿esz siê skoncentrowaæ.\n\r", ch );
		return;
	}
*/
	af.where = TO_AFFECTS;
	af.type = gsn_luck;
	af.level = level;
	af.duration = 1 + level / 5; af.rt_duration = 0;
	af.modifier = level / 10;
	af.location = APPLY_LUC;
	af.bitvector = &AFF_NONE;

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( !is_same_group( gch, ch ) || is_affected( gch, gsn_luck ) )
			continue;

		count++;

		if ( is_affected( gch, gsn_misfortune ) )
		{
			act( "Otaczaj±ca $x {yzgni³o{x-{Gzielona{x aura nieszczê¶cia zanika.", gch, NULL, NULL, TO_ROOM );
			send_to_char( "Otaczaj±ca ciê {yzgni³o{x-{Gzielona{x aura nieszczê¶cia zanika.\n\r", gch );
			affect_strip( gch, gsn_misfortune );
			continue;
		}

		affect_to_char( gch, &af, NULL, TRUE );
		send_to_char( "Czujesz, ¿e szczê¶cie zaczyna ci sprzyjaæ!\n\r", gch );
		act( "Dooko³a $z rozb³yska {Gzielono-{Ys³oneczna{x otoczka szczê¶cia!", gch, NULL, NULL, TO_ROOM );
	}

    if ( count == 0 )
    {
        send_to_char( "Nie uda³o ci siê zwiêkszyæ niczyjego szczê¶cia.\n\r", ch );
    }
	return;
}

void spell_consecrate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	if ( IS_EVIL( ch ) )
	{
		send_to_char( "Nie jeste¶ w stanie po¶wiêciæ tego miejsca.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) )
	{
		send_to_char( "To miejsce jest ju¿ po¶wiêcone.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) )
	{
		remove_event( EVENT_DESECRATE_END, ch->in_room, NULL, 0 );
		EXT_REMOVE_BIT( ch->in_room->room_flags, ROOM_DESECRATE );
		send_to_char( "Wypêdzasz si³y z³a z tego miejsca.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_MAGICDARK ) )
	{
		EXT_REMOVE_BIT( ch->in_room->room_flags, ROOM_MAGICDARK );
		if ( ch->in_room->people )
			act( "Mrok opuszcza to miejsce.", ch->in_room->people, NULL, NULL, TO_ALL );
		return;
	}

	EXT_SET_BIT( ch->in_room->room_flags, ROOM_CONSECRATE );
	act( "Czujesz, ¿e temu miejscu zaczynaj± sprzyjaæ si³y dobra.", ch, NULL, NULL, TO_ALL );
	create_event( EVENT_CONSECRATE_END, PULSE_TICK * ( 2 + level / 6 ), ch->in_room, NULL, 0 );
	return;
}

void spell_desecrate( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	if ( IS_GOOD( ch ) )
	{
		send_to_char( "Nie jeste¶ w stanie zbeszcze¶ciæ tego miejsca.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) )
	{
		send_to_char( "To miejsce jest ju¿ zbeszczeszone.\n\r", ch );
		return;
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) )
	{
		remove_event( EVENT_CONSECRATE_END, ch->in_room, NULL, 0 );
		EXT_REMOVE_BIT( ch->in_room->room_flags, ROOM_CONSECRATE );
		send_to_char( "Wypêdzasz si³y dobra z tego miejsca.\n\r", ch );
		return;
	}

	EXT_SET_BIT( ch->in_room->room_flags, ROOM_DESECRATE );
	act( "Czujesz, ¿e temu miejscu zaczynaj± sprzyjaæ si³y z³a.", ch, NULL, NULL, TO_ALL );
	create_event( EVENT_DESECRATE_END, PULSE_TICK * ( 2 + level / 6 ), ch->in_room, NULL, 0 );
	return;
}

void spell_poison( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim;
	OBJ_DATA *obj;
	AFFECT_DATA af;
	int poison_level;

	if ( target == TARGET_OBJ )
	{
		obj = ( OBJ_DATA * ) vo;
		if ( obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON )
		{
			if ( IS_OBJ_STAT( obj, ITEM_BLESS ) )
			{
				act( "Twoje zaklêcie nie zdo³a³o zatruæ $f.", ch, obj, NULL, TO_CHAR );
				return;
			}
			obj->value[ 3 ] = 1;
			act( "$p pokrywa siê truj±cym nalotem.", ch, obj, NULL, TO_ALL );
			return;
		}

		if ( obj->item_type == ITEM_WEAPON )
		{
			if ( IS_WEAPON_STAT( obj, WEAPON_FLAMING )
			     || IS_WEAPON_STAT( obj, WEAPON_FROST )
			     || IS_WEAPON_STAT( obj, WEAPON_VAMPIRIC )
			     || IS_WEAPON_STAT( obj, WEAPON_VORPAL )
			     || IS_WEAPON_STAT( obj, WEAPON_SHOCKING )
			     || IS_WEAPON_STAT( obj, WEAPON_TOXIC )
			     || IS_WEAPON_STAT( obj, WEAPON_RESONANT )
			     || IS_WEAPON_STAT( obj, WEAPON_SACRED )
			     || IS_OBJ_STAT( obj, ITEM_BLESS )
			     || IS_OBJ_STAT( obj, ITEM_MAGIC ) )
			{
				act( "Nie uda³o ci siê zatruæ $f.", ch, obj, NULL, TO_CHAR );
				return;
			}

			if ( IS_WEAPON_STAT( obj, WEAPON_POISON ) )
			{
				act( "To ju¿ jest zatrute.", ch, obj, NULL, TO_CHAR );
				return;
			}
			//Raszer sciecie poisona wrzucanego na bron, bylo af.level = 1 + level/10
			poison_level = level / 10;
			if((poison_level > 2) && (number_range(0, 100) <50)) { poison_level = 2;}
			af.where	= TO_WEAPON;
			af.type	= sn;
			af.level	= poison_level;
			af.duration = level / 7.5; af.rt_duration = 0;
			af.location	= WEAPON_POISON;
			af.modifier	= 0;
			af.bitvector = &AFF_NONE;
			affect_to_obj( obj, &af );

			act( "$p pokrywa siê ¶mierciono¶nym nalotem.", ch, obj, NULL, TO_ALL );
			return;
		}

		act( "Nie mo¿esz zatruæ $f.", ch, obj, NULL, TO_CHAR );
		return;
	}

	victim = ( CHAR_DATA * ) vo;

	if ( is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ) )
	{
		act( "Nie dasz rady zatruæ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	strip_invis( ch, TRUE, TRUE );

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		act( "$n chwilowo lekko zielenieje.", victim, NULL, NULL, TO_ROOM );
		send_to_char( "Czujesz siê bardzo ¼le, na szczê¶cie to szybko mija.\n\r", victim );
		return;
	}

	poison_to_char( victim, 1 + level / 10 );
	spell_damage( ch, victim, 0, gsn_poison, DAM_NONE , FALSE );
	send_to_char( "Czujesz siê bardzo ¼le.\n\r", victim );
	act( "$n wygl±da bardzo ¼le.", victim, NULL, NULL, TO_ROOM );
	return;
}

void spell_change_liquid( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj;
	char buf [ MAX_STRING_LENGTH ];

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
			// zamieniamy wodê w wino
			if ( obj->value[ 2 ] == 0 )
			{
				act( "Zmieniasz wodê w $k w wino.", ch, obj, NULL, TO_CHAR );
				act( "$n zmienia wodê w $k w wino.", ch, obj, NULL, TO_ROOM );
				obj->value[ 2 ] = 2;
				return;
			}
			// zamieniamy wino w wodê
			if ( obj->value[ 2 ] == 2 || obj->value[ 2 ] == 30 || obj->value[ 2 ] == 33 )
			{
				act( "Zmieniasz wino w $k w wodê.", ch, obj, NULL, TO_CHAR );
				act( "$n zmienia wino w $k w wodê.", ch, obj, NULL, TO_ROOM );
				obj->value[ 2 ] = 0;
				return;
			}

			sprintf( buf, "Nie wiesz w co zmieniæ %s.\n\r", liq_table[ obj->value[ 2 ] ].liq_name4 );
			send_to_char( buf, ch );
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

void spell_ray_of_light( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	if ( IS_EVIL( ch ) )
	{
		send_to_char( "Nie dasz rady przywo³aæ ¦wiat³o¶ci.\n\r", ch );
		return;
	}
	if ( !EXT_IS_SET( ch->in_room->room_flags, ROOM_MAGICDARK ) )
	{
		send_to_char( "Nie wyczywasz tutaj magicznej ciemno¶ci.\n\r", ch );
		return;
	}
	EXT_REMOVE_BIT( ch->in_room->room_flags, ROOM_MAGICDARK );
	if ( ch->in_room->people )
		act( "Mrok opuszcza to miejsce.", ch->in_room->people, NULL, NULL, TO_ALL );

	return;
}

void spell_holy_weapons( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA * obj = NULL;
	OBJ_DATA *obj2 = NULL;
	AFFECT_DATA af, *paf;
	int value;
	int weap_type, weap_type2;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );
	int cha = get_curr_stat_deprecated( ch, STAT_CHA );
	int cursed_weapons = 20387;

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ ¶wiêtych broni.\n\r", ch );
		return;
	}

	if ( is_affected ( ch, gsn_holy_weapons ) )
	{
		send_to_char( "Niedawno prosi³<&e¶/a¶/o¶> ju¿ o pomoc swego boga, lepiej nie nadu¿ywaæ jego mi³osierdzia.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_WIELD ) || get_eq_char( ch, WEAR_SECOND ) )
	{
		send_to_char( "Nosisz ju¿ jak±¶ broñ. Zdejmij j±.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_LIGHT ) || get_eq_char( ch, WEAR_HOLD ) || get_eq_char( ch, WEAR_SHIELD ) || get_eq_char( ch, WEAR_INSTRUMENT ) )
	{
		send_to_char( "Twoje rêce s± zajête, od³ó¿ wszystkie zbêdne przedmioty.\n\r", ch );
		return;
	}

	//bardzo zly paladyn
	if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
	{
         if( paf->modifier <= -50 && paf->modifier > -100)
         {
                af.where = TO_AFFECTS;
             	af.type = sn;
              	af.level = level;
               	af.duration = UMAX( 1, 30 - ( ( ch->level - 24 ) * 3 ) ); af.rt_duration = 0;
               	af.location = APPLY_NONE;
               	af.modifier = 0;
              	af.bitvector = &AFF_NONE;
               	af.visible = FALSE;
             	affect_to_char( ch, &af, NULL, FALSE );
                send_to_char( "Twoje nieczyste sumienie sprawia, i¿ Porten odwraca siê od twojej pro¶by.\n\r", ch );
		        return;

         }
         if( paf->modifier <= -100)
         {
             weap_type = cursed_weapons;
             obj = create_object( get_obj_index( weap_type ), FALSE );
	         obj2 = create_object( get_obj_index( weap_type ), FALSE );
	         if ( !obj )
		     return;
	         if ( !obj2 )
		     return;
		     obj->timer = 10;
	         obj2->timer = 10;
             obj_to_char( obj, ch );
	         equip_char( ch, obj, WEAR_WIELD, TRUE );
          	 obj_to_char( obj2, ch );
	         equip_char( ch, obj2, WEAR_SECOND, TRUE );
	         af.where = TO_AFFECTS;
             af.type = sn;
             af.level = level;
	         af.duration = UMAX( 1, 30 - ( ( ch->level - 24 ) * 3 ) ); af.rt_duration = 0;
	         af.location = APPLY_NONE;
	         af.modifier = 0;
	         af.bitvector = &AFF_NONE;
	         af.visible = FALSE;
	         affect_to_char( ch, &af, NULL, FALSE );

	         act( "Gdy $n koñczy modlitwê powietrze przeszywa niepokoj±ca aura, a w jego d³oniach pojawiaj± siê dwie przeklête bronie.", ch, NULL, NULL, TO_ROOM );
	         send_to_char( "Gdy koñczysz modlitwê przestrzeñ dooko³a ciebie przeszywa niepokoj±ca aura, a twoich d³oniach pojawiaj± siê otoczone krwist± aur± baty.\n\r", ch );
	         return;



         }

    }

  if ( get_skill(ch,gsn_mace_mastery) > 0 )
		weap_type = OBJ_VNUM_HOLY_HAMMER;
  else if ( get_skill(ch,gsn_sword_mastery) > 0 )
		weap_type = OBJ_VNUM_HOLY_SWORD;
	else if ( get_skill(ch,gsn_flail_mastery) > 0 )
		weap_type = OBJ_VNUM_HOLY_FLAIL;
	else
	{
		if ( get_skill( ch, gsn_mace ) >= get_skill( ch, gsn_flail )
			&& get_skill( ch, gsn_mace ) >= get_skill( ch, gsn_sword ) )
				weap_type = OBJ_VNUM_HOLY_HAMMER;
		else if ( get_skill( ch, gsn_sword ) >= get_skill( ch, gsn_flail ) )
				weap_type = OBJ_VNUM_HOLY_SWORD;
		else
				weap_type = OBJ_VNUM_HOLY_FLAIL;
	}

  weap_type2 = weap_type;

  value = number_range( 0, luck + 3*cha );

  switch ( value )
  {
  	case 0:
  	case 1:
  	case 2:
  	case 3:
  	case 4:
  	case 5:
		weap_type = ( number_range(0,2) != 0 ? ( number_range(0,1) == 0 ? OBJ_VNUM_HOLY_HAMMER : OBJ_VNUM_HOLY_FLAIL ) : OBJ_VNUM_HOLY_SWORD );
		weap_type2 = ( number_range(0,2) != 0 ? ( number_range(0,1) == 0 ? OBJ_VNUM_HOLY_HAMMER : OBJ_VNUM_HOLY_FLAIL ) : OBJ_VNUM_HOLY_SWORD );
		break;
  	default:
  		break;
  }

	obj = create_object( get_obj_index( weap_type ), FALSE );
	obj2 = create_object( get_obj_index( weap_type2 ), FALSE );

	if ( !obj )
		return;
	if ( !obj2 )
		return;

	value = dice( 2, 4 ) + level / 7.5;
	if ( number_range( 0, luck ) > 15 ) value += 1;
	if ( number_range( 0, luck ) < 5 ) value -= 1;
	obj->timer = UMAX( 3, value );
	obj2->timer = UMAX( 3, value );

	value = 2 * level;
	if ( number_range( 0, luck ) > 15 ) value = ( value * 9 ) / 10;
	if ( number_range( 0, luck ) < 5 ) value = ( value * 115 ) / 100;
	obj->rent_cost = value;
	obj2->rent_cost = value;

	value = level / 7.5;
	 //bonus dla dobrego paladyna
  if (paf != NULL)
  {
          if( paf->modifier > 50 && number_percent() > paf->modifier/2)
          {
              value++;
              paf->modifier -= 15;
              send_to_char( "Czujesz jak zgromadzona w tobie ¶wiêta moc zaczyna pulsowaæ.\n\r", ch );

          }

          if( paf->modifier > 70 && number_percent() > paf->modifier/4)
          {
              value++;
              paf->modifier -= 15;
              send_to_char( "Czujesz jak otacza ciê ³aska twego boga.\n\r", ch );

          }

  }
	if ( number_range( 0, luck ) > 15 ) ++value;
	if ( number_range( 0, luck ) < 5 ) --value;

	EXT_SET_BIT( obj->extra_flags, ITEM_GLOW );
	EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );
	EXT_SET_BIT( obj2->extra_flags, ITEM_GLOW );
	EXT_SET_BIT( obj2->extra_flags, ITEM_MAGIC );

	switch ( UMAX( 0, value ) )
	{
		case 0:
			obj->value[ 1 ] = 1;
			obj->value[ 2 ] = 5;
			obj->value[ 5 ] = 1;
			obj->value[ 6 ] = 1;
			obj2->value[ 1 ] = 1;
			obj2->value[ 2 ] = 5;
			obj2->value[ 5 ] = 1;
			obj2->value[ 6 ] = 1;
			break;
		case 1:
			obj->value[ 1 ] = 1;
			obj->value[ 2 ] = 6;
			obj->value[ 5 ] = 2;
			obj->value[ 6 ] = 2;
			obj2->value[ 1 ] = 1;
			obj2->value[ 2 ] = 6;
			obj2->value[ 5 ] = 2;
			obj2->value[ 6 ] = 2;
			break;
		case 2:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 3;
			obj->value[ 5 ] = 3;
			obj->value[ 6 ] = 3;
			obj2->value[ 1 ] = 2;
			obj2->value[ 2 ] = 3;
			obj2->value[ 5 ] = 3;
			obj2->value[ 6 ] = 3;
			break;
		case 3:
			obj->value[1] = 2;
			obj->value[2] = 4;
			obj->value[5] = 4;
			obj->value[6] = 4;
			obj2->value[1] = 2;
			obj2->value[2] = 4;
			obj2->value[5] = 4;
			obj2->value[6] = 4;
			break;
		default:
			obj->value[1] = 2;
			obj->value[2] = 5;
			obj->value[5] = 4;
			obj->value[6] = 4;
			obj2->value[1] = 2;
			obj2->value[2] = 5;
			obj2->value[5] = 4;
			obj2->value[6] = 4;
			break;
	}

	obj_to_char( obj, ch );
	equip_char( ch, obj, WEAR_WIELD, TRUE );
	obj_to_char( obj2, ch );
	equip_char( ch, obj2, WEAR_SECOND, TRUE );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = UMAX( 1, 30 - ( ( ch->level - 24 ) * 3 ) ); af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_NONE;
	af.visible = FALSE;
	affect_to_char( ch, &af, NULL, FALSE );

	act( "Gdy $n koñczy modlitwê wraz z szaro-niebieskim ¶wiat³em w $s d³oniach pojawiaj± siê dwie ¶wiête bronie.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "Gdy koñczysz modlitwê z delikatnym rozb³yskiem ¶wiat³a w twoich d³oniach pojawiaj± siê dwie ¶wiête bronie.\n\r", ch );
	return;
}

/* strach. cast fear <kto>
   jak nie ma brave_cloak'a i nie wyjdzie rzut obronny, to gosc
   dostaje affecta. nie moze atakowac(bo siê boi :) i ucieka z walki
*/
void spell_fear( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    CHAR_DATA * victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int luck, vluck;

    if ( victim == ch )
    {
        send_to_char( "Ojej, jak tu strasznie!\n\r", ch );
        return;
    }

    /**
     * w przypadku bycia pod wplywem brave cloak, dajemy szanse na jego zdjecie
     */
    if ( is_affected(victim, gsn_brave_cloak ) )
    {
        if ( number_percent() > 50 && check_dispel( level, victim, gsn_brave_cloak ) )
        {
            send_to_char( "Chroni±cy ciê p³aszcz odwagi znika.\n\r", ch );
            act( "Chroni±cy $z p³aszcz odwagi znika.", victim, NULL, NULL, TO_ROOM );
            affect_strip( victim, gsn_brave_cloak );
            if ( IS_AFFECTED( victim, AFF_BRAVE_CLOAK ) )
            {
                EXT_REMOVE_BIT( victim->affected_by, AFF_BRAVE_CLOAK );
            }
        }
    }

    if ( is_affected(victim, gsn_brave_cloak ) || IS_AFFECTED( victim, AFF_BRAVE_CLOAK ) || is_undead( victim ) || IS_SET( victim->form, FORM_CONSTRUCT ))
    {
        switch ( victim->sex )
        {
            case SEX_MALE:
                act( "$N jest odporny na strach.\n\r", ch, NULL, victim, TO_CHAR );
                break;
            case SEX_FEMALE:
                act( "$N jest odporna na strach.\n\r", ch, NULL, victim, TO_CHAR );
                break;
            case SEX_NEUTRAL:
            default:
                act( "$N jest odporne na strach.\n\r", ch, NULL, victim, TO_CHAR );
                break;
        }
        return;
    }

    if ( IS_AFFECTED( victim, AFF_FEAR ) )
    {
        switch ( victim->sex )
        {
            case SEX_MALE:
                act( "$N jest ju¿ wystarcz±jaco przera¿ony.\n\r", ch, NULL, victim, TO_CHAR );
                break;
            case SEX_FEMALE:
                act( "$N jest ju¿ wystarcz±jaco przera¿ona.\n\r", ch, NULL, victim, TO_CHAR );
                break;
            case SEX_NEUTRAL:
            default:
                act( "$N jest ju¿ wystarcz±jaco przera¿one.\n\r", ch, NULL, victim, TO_CHAR );
                break;
        }
        return;
    }

    /* luck */

    luck = get_curr_stat_deprecated( ch, STAT_LUC );
    vluck = get_curr_stat_deprecated( victim, STAT_LUC );

    if ( number_range( 0, luck + vluck ) == 0 )
    {
        act( "Nie uda³o ci siê przestraszyæ $Z.", ch, NULL, victim, TO_CHAR );
        return;
    }

    /* odpornosc? */
    if ( victim->resists[ RESIST_FEAR ] > 0 && number_percent() < victim->resists[ RESIST_FEAR ] )
    {
        return;
    }

    /* nie ma straszenia starszych :)*/
    if ( victim->level > level + UMAX( 1, level / 10 ) )
    {
        return;
    }

    if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
    {
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 2; af.rt_duration = 0;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = &AFF_FEAR;
    affect_to_char( victim, &af, NULL, TRUE );

    send_to_char( "Zaczynasz odczuwaæ paniczny strach.\n\r", victim );
    act( "$n panikuje ze strachu.", victim, NULL, NULL, TO_ROOM );

    if ( victim->fighting )
    {
        interpret( victim, "flee" );
        interpret( victim, "flee" );
        interpret( victim, "flee" );
    }

    if ( IS_NPC( victim ) )
    {
        start_fearing( victim, ch );
    }

    return;
}

void spell_healing_ring( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * gch;
	int heal_value, members = 0, refresh_value;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê uzdrowiæ przyjació³.\n\r", ch );
		return;
	}

	heal_value = number_range( 40, 50 ) + level / 2;
	if ( number_range( 0, luck ) < 3 ) heal_value /= 2;
	if ( number_range( 0, luck ) > 15 ) heal_value *= ( number_range( 100, 110 ) ) / 100;

	//refreshowac bedzie tylko healing ring rzucony przez druida
	refresh_value = number_range( 10, 25 ) + level / 3;
	if ( number_range( 0, luck ) < 3 ) refresh_value /= 2;

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
	{
		if ( is_same_group( ch, gch ) )
		{
			members++;

			if ( ( ch->class == CLASS_CLERIC ) && ( gch->hit == get_max_hp( gch ) ) )
				members--;
		}
	}

	if ( members > 8 )
	{
		heal_value /= 3;
		refresh_value /= 3;
	}
	else if ( members > 6 )
	{
		heal_value /= 2;
		refresh_value /= 2;
	}
	else if ( members > 4 )
	{
		heal_value -= heal_value / 4;
		refresh_value -= refresh_value / 4;
	}

	for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
		if ( is_same_group( ch, gch ) )
		{
			heal_char( ch, gch, heal_value, FALSE );
			send_to_char( "Czujesz przyjemne mrowienie skóry.\n\r", gch );

			if ( ch->class == CLASS_DRUID )
			{
				if ( gch->move != gch->max_move )
				{
					gch->move = UMIN( gch->move + refresh_value, gch->max_move );

					if ( gch->max_move == gch->move )
						send_to_char( "Zmêczenie ca³kowicie ustêpuje.\n\r", gch );
					else
						send_to_char( "Czujesz jak zmêczenie powoli ustêpuje.\n\r", gch );
				}
			}
		}
}

void spell_divine_shield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af, *paf, *paff;
	bool ok = FALSE;
	int holy_dur_mod = 0, holy_lev_mod = 0;

	if ( IS_EVIL( victim ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N nie jest godne opieki bogów.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N nie jest godny opieki bogów.", ch, NULL, victim, TO_CHAR );
				break;
			default :
				act( "$N nie jest godna opieki bogów.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	if ( IS_AFFECTED( victim, AFF_RESIST_ACID ) ||
	     IS_AFFECTED( victim, AFF_RESIST_COLD ) ||
	     IS_AFFECTED( victim, AFF_RESIST_FIRE ) ||
	     IS_AFFECTED( victim, AFF_RESIST_LIGHTNING ) ||
	     IS_AFFECTED( victim, AFF_RESIST_FEAR ) ||
	     IS_AFFECTED( victim, AFF_RESIST_NEGATIVE ) )
	{
		send_to_char( "Nie uda³o ci siê wezwaæ tarczy bogów.\n\r", ch );
		return;
	}

	   if ( ( paff = affect_find( ch->affected, gsn_prayer_last )) != NULL)
    {
         if( paff->modifier < 24)
         {
             //bonusy tylko gdy modlimy sie regularnie
             if( (paf = affect_find( ch->affected, gsn_holy_pool)) != NULL)
             {
                 if (paf->modifier > 25 && number_percent() < URANGE(15, paf->modifier/2, 30))
                 {
                     holy_dur_mod += 2;
                     paf->modifier -= 2;
                 }

                 if (paf->modifier > 50 && number_percent() < 30)
                 {
                     holy_lev_mod += 5;
                     paf->modifier -= 2;
                 }

                 if (paf->modifier > 70 && number_percent() < 30)
                 {
                     holy_lev_mod += 10;
                     paf->modifier -= 5;
                 }
             }
         }
          if( (paf = affect_find( ch->affected, gsn_holy_pool)) != NULL)
          {
              if (-(paf->modifier) > 30 ) holy_lev_mod -= 5;
              if (-(paf->modifier) > 75 )
              {
                send_to_char( "Nie jeste¶ godny by prosiæ bogów o pomoc.\n\r", ch );
		        return;
              }

          }


    }

	af.where = TO_AFFECTS;
	af.type = sn;
	af.location = APPLY_RESIST;

	if ( !IS_AFFECTED( victim, AFF_RESIST_ACID ) && ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 3 ) )
	{
		af.level = 10 + holy_lev_mod + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ? 2 : 0 );
		af.duration = holy_dur_mod + URANGE( 1, level / 6, 4 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ); af.rt_duration = 0;
		af.modifier = RESIST_ACID;
		af.bitvector = &AFF_RESIST_ACID;
		affect_to_char( victim, &af, NULL, TRUE );
		ok = TRUE;
	}

	if ( !IS_AFFECTED( victim, AFF_RESIST_COLD ) && ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 3 ) )
	{
		af.level = 10 + holy_lev_mod + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ? 2 : 0 );
		af.duration = holy_dur_mod + URANGE( 1, level / 6, 4 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ); af.rt_duration = 0;
		af.modifier = RESIST_COLD;
		af.bitvector = &AFF_RESIST_COLD;
		affect_to_char( victim, &af, NULL, TRUE );
		ok = TRUE;
	}

	if ( !IS_AFFECTED( victim, AFF_RESIST_FIRE ) && ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 3 ) )
	{
		af.level = 10 + holy_lev_mod + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ? 2 : 0 );
		af.duration = holy_dur_mod + URANGE( 1, level / 6, 4 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ); af.rt_duration = 0;
		af.modifier = RESIST_FIRE;
		af.bitvector = &AFF_RESIST_FIRE;
		affect_to_char( victim, &af, NULL, TRUE );
		ok = TRUE;
	}

	if ( !IS_AFFECTED( victim, AFF_RESIST_LIGHTNING ) && ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 3 ) )
	{
		af.level = 10 + holy_lev_mod + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ? 2 : 0 );
		af.duration = holy_dur_mod + URANGE( 1, level / 6, 4 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ); af.rt_duration = 0;
		af.modifier = RESIST_ELECTRICITY;
		af.bitvector = &AFF_RESIST_LIGHTNING;
		affect_to_char( victim, &af, NULL, TRUE );
		ok = TRUE;
	}

	if ( !IS_AFFECTED( victim, AFF_RESIST_FEAR ) && ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 3 ) )
	{
		af.level = 10 + holy_lev_mod + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ? 2 : 0 );
		af.duration = holy_dur_mod + URANGE( 1, level / 6, 4 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ); af.rt_duration = 0;
		af.modifier = RESIST_FEAR;
		af.bitvector = &AFF_RESIST_FEAR;
		affect_to_char( victim, &af, NULL, TRUE );
		ok = TRUE;
	}

	if ( !IS_AFFECTED( victim, AFF_RESIST_NEGATIVE ) && ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 2 ) )
	{
		af.level = 20 + level + holy_lev_mod + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_WIS ) ) > 15 ) ? 2 : 0 );
		af.duration = holy_dur_mod + URANGE( 2, level / 6, 5 ) + ( ( number_range( 0, get_curr_stat_deprecated( ch, STAT_LUC ) ) > 10 ) ? 1 : 0 ); af.rt_duration = 0;
		af.modifier = RESIST_NEGATIVE;
		af.bitvector = &AFF_RESIST_NEGATIVE;
		affect_to_char( victim, &af, NULL, TRUE );
		ok = TRUE;
	}

	if ( ok )
	{
        if (holy_lev_mod > 0 || holy_dur_mod > 0) send_to_char( "Czujesz jak zgromadzona w tobie ¶wiêta moc uwalnia siê, wzmacniaj±c bosk± tarczê.\n\r", ch );
        if (holy_lev_mod < 0 ) send_to_char( "Czujesz jak czê¶æ zgromadzonej energi ucieka ci bezpowrotnie, a boska tarcza staje siê mniej stabilna.\n\r", ch );

        if ( ch == victim )
		{
			act( "$n otacza siê ochronn±, bosk± tarcz±.", ch, NULL, victim, TO_ROOM );
			send_to_char( "Otaczasz siê ochronn±, bosk± tarcz±.\n\r", victim );
		}
		else
		{
			act( "$n otacza $C ochronn±, bosk± tarcz±.", ch, NULL, victim, TO_NOTVICT );
			act( "$n otacza ciê ochronn±, bosk± tarcz±.", ch, NULL, victim, TO_VICT );
			act( "Otaczasz ochronn±, bosk± tacz± $C.", ch, NULL, victim, TO_CHAR );
		}
	}
	else
	{
		send_to_char( "Nie uda³o ci siê, a energia zaklêcia zosta³a rozproszona.\n\r", ch );
	}
	return;
}


void spell_lore_undead( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	int luck = get_curr_stat_deprecated( ch, STAT_LUC ),mod = 5 + number_range(1,5);
	char buf[ MAX_STRING_LENGTH ];

	if (IS_NPC(ch)) return;

	if ( !IS_NPC(victim) )
	{
		act( "$N nie wygl±da na martwiaka.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( !is_undead(victim) )
	{
		act( "$N nie wygl±da na martwiaka.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( victim->fighting )
	{
		act( "$N jest w trakcie walki, wiêc nie za bardzo dasz radê przy³o¿yæ rêkê.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "W ostatniej chwili rozpraszasz swoj± uwagê.\n\r", ch );
		return;
	}

	if (ch->class == CLASS_PALADIN)
	{
		// paladyni z racji nienawi¶ci do martwiaków maj± mniejsze penalty
		mod /= 2;
	}

	if (victim->level - UMAX(3,mod) > ch->level)
	{
		act( "Nie dasz rady poznaæ w³a¶ciwo¶ci $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	sprintf( buf, "{r%s{x\n\r", victim->name );
	send_to_char( buf, ch );

	sprintf( buf, "Rasa: %s.\n\r",race_table[ GET_RACE( victim ) ].name);
	send_to_char( buf, ch );

	switch (victim->sex)
	{
		case SEX_MALE:
			send_to_char( "P³eæ: mê¿czyzna.\n\r", ch );
			break;
		case SEX_FEMALE:
			send_to_char( "P³eæ: kobieta.\n\r", ch );
			break;
		default:
			break;
	}

	sprintf( buf, "Wzrost: %d cm.\n\rWaga: %d.%d kg.\n\r", victim->height, victim->weight / 10, victim->weight % 10 );
	send_to_char( buf, ch );

	send_to_char("Wielko¶æ: ", ch);
	switch (victim->size)
	{
		case 0:
			send_to_char("niewielka", ch);
			break;
		case 1:
			send_to_char("ma³a", ch);
			break;
		case 2:
			send_to_char("¶rednia", ch);
			break;
		case 3:
			send_to_char("wiêksza", ch);
			break;
		case 4:
			send_to_char("ogromna", ch);
			break;
		case 5:
			send_to_char("gigantyczna", ch);
			break;
	}
	send_to_char( ".\n\r", ch );

	sprintf( buf, "Punkty ¿ycia: %d.\n\r", victim->hit);
	send_to_char( buf, ch );

	sprintf( buf,"Poziom: %d.\n\r",victim->level);
	send_to_char( buf, ch );

	if (ch->class == CLASS_CLERIC || ch->class == CLASS_PALADIN)
	{
		sprintf( buf,"Align: %d.\n\r", 	victim->alignment);
		send_to_char( buf, ch );
	}

	if ( ch->class == CLASS_PALADIN )
	{
		sprintf( buf,"Modyfikator trafienia: %d.\n\rModyfikator obra¿eñ: %d.\n\r",GET_HITROLL( victim ), GET_DAMROLL( victim, NULL ));
		send_to_char( buf, ch);
	}

	if ( IS_NPC( victim ) && victim->pIndexData->new_format )
	{
		sprintf( buf, "Obra¿enia: %dd%d  typu:  %s.\n\r",victim->damage[ DICE_NUMBER ], victim->damage[ DICE_TYPE ],attack_table[ victim->dam_type ].noun );
		send_to_char( buf, ch );
	}

	return;
}

void spell_sense_life( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
	int dur;

	if ( IS_AFFECTED( ch, AFF_SENSE_LIFE ) )
	{
		send_to_char( "Potrafisz ju¿ przecie¿ wyczuwaæ ¿ycie.\n\r", ch );
		return;
	}

	dur = level / 2;
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = dur; af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_SENSE_LIFE;
	affect_to_char( ch, &af, NULL, TRUE );

	send_to_char( "Przymykasz na chwilê oczy, a kiedy je otwierasz, ¶wiat zdaje siê eksplodowaæ tysi±cem kolorów.\n\r", ch );
	act( "$n przymyka na chwilê oczy, a kiedy je otwiera, widzisz, ¿e sta³y siê jasnoz³ote." , ch, NULL, NULL, TO_ROOM );
	return;
}

void spell_spiritual_weapon( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	OBJ_DATA *obj = NULL;

	int value;
	int weap_type = OBJ_VNUM_SPIRITUAL_HAMMER; /* default: mloteczek */
	int luck = get_curr_stat_deprecated( ch, STAT_LUC );

    int skill_max_value = 0, skill_value = 0;

	if ( number_range( 0, LUCK_BASE_MOD + luck ) < 1 )
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ widmowej broñi.\n\r", ch );
		return;
	}

	if ( get_hand_slots( ch, WEAR_WIELD ) >= 2 )
	{
		send_to_char( "Masz ju¿ dwie zajête rêce. Zdejmij co¶.\n\r", ch );
		return;
	}

	if ( get_eq_char( ch, WEAR_WIELD ) )
	{
		send_to_char( "Nosisz ju¿ jak±¶ broñ. Zdejmij j±.\n\r", ch );
		return;
    }

    /*
     * wybieranie najlepszej broni
     */

    /* mace */
    skill_value = get_skill ( ch, gsn_mace );
    if ( skill_value > skill_max_value )
    {
        skill_max_value = skill_value;
        weap_type = OBJ_VNUM_SPIRITUAL_HAMMER;
    }

    /* flail */
    skill_value = get_skill ( ch, gsn_flail );
    if ( skill_value > skill_max_value )
    {
        skill_max_value = skill_value;
        weap_type = OBJ_VNUM_SPIRITUAL_FLAIL;
    }

    /* staff */
    skill_value = get_skill ( ch, gsn_staff );
    if ( skill_value > skill_max_value )
    {
        skill_max_value = skill_value;
        weap_type = OBJ_VNUM_SPIRITUAL_STAFF;
    }

    /* short sword */
    skill_value = get_skill ( ch, gsn_shortsword );
    if ( skill_value > skill_max_value )
    {
        skill_max_value = skill_value;
        weap_type = OBJ_VNUM_SPIRITUAL_SHORTSWORD;
    }
    obj = create_object( get_obj_index( weap_type ), FALSE );
    if ( !obj )
    {
        send_to_char( "Puff ... i nic.\n\r", ch );
        bug( "Brak obiektu w dla czaru spell_spiritual_weapon.", 0 );
        return;
    }

	value = 2 + dice( 2, 4 ) + level / 5;

	if ( number_range( 0, luck ) > 15 ) value += 2;
	if ( number_range( 0, luck ) < 5 )  value /= 3;
	obj->timer = value;

	value = 2 * level;
	if ( number_range( 0, luck ) > 15 ) value = ( value * 9 ) / 10;
	if ( number_range( 0, luck ) < 5 ) value = ( value * 115 ) / 100;
	obj->rent_cost = value;

	value = level / 5;
	if ( number_range( 0, luck ) > 13 ) ++value;
	if ( number_range( 0, luck ) < 3 ) --value;

    /*
     * dodaje bonus za posiadane umiejetnosci w danej broni
     */
    if ( skill_max_value > 50 ) { value++; }
    if ( skill_max_value > 30 ) { value++; }
    if ( skill_max_value > 10 ) { value++; }

	EXT_SET_BIT( obj->extra_flags, ITEM_GLOW );
	EXT_SET_BIT( obj->extra_flags, ITEM_MAGIC );

	switch ( UMAX( 0, value ) )
	{
		case 0:
			obj->value[ 1 ] = 1;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 1;
			obj->value[ 6 ] = 1;
			break;
		case 1:
			obj->value[ 1 ] = 1;
			obj->value[ 2 ] = 5;
			obj->value[ 5 ] = 2;
			obj->value[ 6 ] = 2;
			break;
		case 2:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 1;
			obj->value[ 6 ] = 1;
			break;
		case 3:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 2;
			obj->value[ 6 ] = 1;
			break;
		case 4:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 3;
			obj->value[ 6 ] = 1;
			break;
		case 5:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 4;
			obj->value[ 6 ] = 1;
			break;
		case 6:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 1;
			obj->value[ 6 ] = 2;
			break;
		case 7:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 1;
			obj->value[ 6 ] = 3;
			break;
		case 8:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 2;
			obj->value[ 6 ] = 2;
			break;
		case 9:
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 2;
			obj->value[ 6 ] = 3;
            break;
		default:
            /*
             * zaladuje sie tylko jezeli:
             * - szczescie +1
             * - poziom > 29
             * - poziom broni > 50%
             */
			obj->value[ 1 ] = 2;
			obj->value[ 2 ] = 4;
			obj->value[ 5 ] = 3;
			obj->value[ 6 ] = 3;
			break;
    }

    /*
     * logowanie jaka bron zostala stworzona
     */
    sprintf
        (
         log_buf,
         "[%d] spell_spiritual_weapon: %s (%d), skill: %d, weapon: %d (%d)",
         ch->in_room ? ch->in_room->vnum : 0,
         IS_NPC( ch )? ch->short_descr : ch->name,
         ch->level,
         skill_max_value,
         obj->value[0],
         value
        );
    log_string( log_buf );

	obj_to_char( obj, ch );
	equip_char( ch, obj, WEAR_WIELD, TRUE );

	act( "W d³oni $z pojawia siê widmowa broñ.", ch, NULL, NULL, TO_ROOM );
	send_to_char( "W twojej d³oni pojawia siê widmowa broñ.\n\r", ch );
	return;
}

void spell_holy_bolt( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	OBJ_DATA *obj;
	int dam, luck = get_curr_stat_deprecated( ch, STAT_LUC );

	if ( number_range( 0, luck + LUCK_BASE_MOD ) == 0 )
	{
		send_to_char( "Nie uda³o ci siê uderzenia boskiej mocy.\n\r", ch );
		return;
    }

    if ( !is_undead( victim ) )
    {
        send_to_char( "¦wiêta boska moc tego czaru dzia³a tylko na nieumar³ych.\n\r", ch );
        return;
    }
	dam = number_range( 20, 30 ) + dice( level, 3 );
	dam = luck_dam_mod( ch, dam );

	if ( IS_GOOD( ch ) )
	{
		        act( "Promieñ bia³ego ¶wiat³a trafia prosto w $C.", ch, NULL, victim, TO_CHAR );
				act( "Promieñ bia³ego ¶wiat³a trafia ciê prosto w pier¶.", ch, NULL, victim, TO_VICT );
				act( "Promieñ bia³ego ¶wiat³a trafia posto w $C.", ch, NULL, victim, TO_NOTVICT );
	}
	if ( IS_EVIL( ch ) )
	{
		        act( "Promieñ czerwonego ¶wiat³a trafia prosto w $C.", ch, NULL, victim, TO_CHAR );
				act( "Promieñ czerwonego ¶wiat³a trafia ciê prosto w pier¶.", ch, NULL, victim, TO_VICT );
				act( "Promieñ czerwonego ¶wiat³a trafia posto w $C.", ch, NULL, victim, TO_NOTVICT );
	}
	if ( IS_NEUTRAL( ch) )
	{
		       act( "Promieñ niebieskiego ¶wiat³a trafia prosto w $C.", ch, NULL, victim, TO_CHAR );
				act( "Promieñ niebieskiego ¶wiat³a trafia ciê prosto w pier¶.", ch, NULL, victim, TO_VICT );
				act( "Promieñ niebieskiego ¶wiat³a trafia posto w $C.", ch, NULL, victim, TO_NOTVICT );
	}
	 if( victim->hit - (dam*(100-victim->resists[RESIST_HOLY]))/100 < -10 )
    {
    	act( "W jasnym rozb³ysku ¶wiat³a po $b zostaje tylko kupka popio³u.", victim, NULL, NULL, TO_ROOM );
    	act( "Czujesz okropny ból gdy, boska moc wypala ciê wypieraj±c negatywn± energiê z ka¿dego kawa³ka twojego nieumar³ego cia³a...", victim, NULL, NULL, TO_CHAR );
     	obj = create_object( get_obj_index( OBJ_VNUM_ASH ), FALSE );
	obj->timer = UMAX(3,number_percent()/10);
	obj_to_room( obj, ch->in_room );
     }

	spell_damage( ch, victim, dam, sn, DAM_HOLY, FALSE );
	return;
}

void spell_resurrection( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA * obj = ( OBJ_DATA * ) vo, *item, *item_next;
    CHAR_DATA * victim = NULL;
    AFFECT_DATA af;
    char buf[ MAX_INPUT_LENGTH ];

    if ( obj->item_type != ITEM_CORPSE_PC )
    {
            print_char( ch, "Nic siê nie sta³o.\n\r" );
            return;
    }

    SPIRIT_DATA * duch;
    for ( duch = spirits; duch != NULL; duch = duch->next )
    {
        if ( duch->corpse == obj )
        {
            victim = duch->ch;
        }
    }

    if ( victim == NULL )
    {
            print_char( ch, "Nie jeste¶ ju¿ w stanie wskrzesiæ tych zw³ok.\n\r" );
            return;
    }

    if ( victim->pcdata->corpse )
    {
        victim->pcdata->corpse = NULL;
    }

    del_spirit( victim );
    char_to_room( victim, ch->in_room );

    for ( item = obj->contains; item != NULL; item = item_next )
    {
            OBJ_NEXT_CONTENT( item, item_next );
            obj_from_obj( item );
            obj_to_char( item, victim ); 
    }    

    extract_obj( obj );

    act( "Wskrzeszasz $C.", ch, NULL, victim, TO_CHAR );
    act( "$N wskrzesza $C.", ch, NULL, victim, TO_NOTVICT );
    switch ( victim->sex )
    {
        case 0:
            act( "Jeste¶ wskrzeszone!", ch, NULL, victim, TO_VICT );
            break;
        case 2:
            act( "Zosta³as wskrzeszona!", ch, NULL, victim, TO_VICT );
            break;
        default:
            act( "Zosta³es wskrzeszony!", ch, NULL, victim, TO_VICT );
            break;
    }

    return;
}
