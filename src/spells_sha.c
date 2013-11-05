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
 * KILLER MUD is copyright 1999-2010 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Jaromir Klisz         (jklisz@gmail.com              ) [Drake     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: spells_sha.c 11255 2012-04-15 15:25:44Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/spells_sha.c $
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
#include "lang.h"
#include "music.h"
#include "tables.h"

/**
 *
 * Czary szamana
 *
 */

extern char * target_name;
CHAR_DATA *get_char_area( CHAR_DATA *ch, char *argument );

bool  saves_dispel	args( ( int dis_level, int spell_level, int duration ) );
bool  check_dispel	args( ( int dis_level, CHAR_DATA *victim, int sn ) );
bool  check_shaman_invoke args( ( CHAR_DATA *ch ) );
void  set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	raw_kill	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
bool	check_blink	args( (CHAR_DATA *victim, CHAR_DATA *ch, bool unik) );
bool	remove_obj	args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );

/** Trzy zasady w tworzeniu czarów szamana:
 *
 *  1) Moc czarów zale¿y tylko od umiejêno¶ci 'Invoke Spirit', 3% wyszkolenia umiejêtno¶ci powinno dawaæ bonusy mniej
 *     wiêcej równe 1 poziomowi gracza.
 *  Przykad:
 *            int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
 *
 *  2) Szansa na udanie siê czaru wywo³ywana jest funkcj± check_shaman_invoke (I nie jest zale¿na od szczê¶cia).
 *     Wywo³ywanie funkcji check_shaman_invoke znajduje siê w pliku magic.c
 *  Przyk³ad:
 *           if ( check_shaman_invoke ( ch ) == TRUE )
 *           {
 *           <Tutaj piszemy kod czaru>
 *           }
 *            else
 *           {
 *	        	  send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
 *		        return;
 *	          }
 *
 *  3) Nigdy nie ³am zasady pierwszej i drugiej.
 *
 *  Drake dnia pañskiego 1 czerwca 2009 roku.
 */

//Czar osobisty który umo¿liwia szamanowi widzenie w ciemno¶ciach.
void spell_spirit_of_vision( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int duration, af_level = spirit_power / 2;

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

   duration = 2 + spirit_power / 4;

	if ( IS_AFFECTED( victim, AFF_DARK_VISION ) )
	{
		send_to_char( "Przecie¿ widzisz w ciemno¶ci.\n\r", ch );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = duration; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_DARK_VISION;
	     affect_to_char( victim, &af, NULL, TRUE );

	     act( "Oczy $z zaczynaj± lekko ¶wieciæ zielonym blaskiem.", ch, NULL, NULL, TO_ROOM );
		  send_to_char( "Twoje oczy zaczynaj± lekko ¶wieciæ zielonym blaskiem.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
	     return;
      }
}

/*Czar umozliwia ³atwiejsz± kontrolê nad duchami, jest jednym z tych które szaman ca³y czas powinien mieæ na sobie.
  Chyba, ¿e jest na tyle potê¿ny, by nie korzystaæ z opieki przodków.*/
void spell_subdue_spirits( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int duration, af_level = spirit_power / 2;

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

   duration = 2 + spirit_power / 4;

	if ( IS_AFFECTED( victim, AFF_SUBDUE_SPIRITS ) )
	{
   	send_to_char( "Posiadasz ju¿ przychylno¶æ duchów.\n\r", ch );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = duration; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_SUBDUE_SPIRITS;
	     affect_to_char( victim, &af, NULL, TRUE );

	     act( "Dooko³a $z zaczynaj± unosiæ siê widmowe postacie.", ch, NULL, NULL, TO_ROOM );
		  send_to_char( "Przywo³ujesz swoje duchy opiekuñcze, których widmowe sylwetki zaczynaj± unosiæ siê niezauwa¿alnie dooko³a ciebie.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda³o ci siê zyskaæ przychylno¶ci duchów.\n\r", ch );
	     return;
      }
}
/*Czar powoduje, ¿e rzucaj±cy zaklêcie nie zostawia ¶ladów, dodatkowo zmniejsza kary do mv za chodzenie
  po górach, stepach, jaskiniach, lodowcu i zaspach. */
void spell_spirit_walk( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int duration, af_level = spirit_power / 2;

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

   duration = 1 + spirit_power / 5;

	if ( IS_AFFECTED( victim, AFF_SPIRIT_WALK ) )
	{
   	send_to_char( "Nie mo¿esz bardziej nie zostawiaæ ¶ladów.\n\r", ch );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = duration; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_SPIRIT_WALK;
	     affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Niczym duch, przestajesz zostawiaæ na ziemi jakiekolwiek ¶lady.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
	     return;
      }
}

//Czar przyspieszaj±cy zabli¼nianie siê ran. Nie kumuluje siê z efektem bandarzy.
void spell_healing_salve( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int  hp_percent, duration;

	    if ( ch->fighting )
    {
        send_to_char( "Skup siê lepiej na walce.\n\r", ch );
        return;
    }

    if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) )
    {
        send_to_char( "To siê nie uda.\n\r", ch );
        return;
    }

    if ( is_affected( victim, gsn_bandage ) && !is_affected(victim,gsn_bleeding_wound ) )
    {
        if ( ch == victim )
            send_to_char( "Twoje rany s± opatrzone, nie dasz rady rady bardziej przy¶pieszyæ ich leczenia.\n\r", ch );
        else
            print_char( ch, "Rany %s s± ju¿ opatrzone, nie dasz rady bardziej przy¶pieszyæ ich leczenia.\n\r", victim->name2 );
        return;
    }

    hp_percent = 100 * victim->hit / UMAX( 1, get_max_hp(victim) );

    if ( hp_percent > 95 && !is_affected(victim,gsn_bleeding_wound) )
    {
        print_char( ch, "Nie dasz rady bardziej przy¶pieszyæ gojenia ran.\n\r" );
        return;
    }

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

     duration = 3 + URANGE(0, spirit_power/5, 5);

	if ( IS_AFFECTED(ch, AFF_ANCESTORS_FAVOR ) )
	{
     duration = 3 + URANGE(0, spirit_power/5, 7);
	}

   if ( check_shaman_invoke ( ch ) == TRUE )
   {
        af.where = TO_AFFECTS;
        af.type = sn;
        af.level = spirit_power/3;
        af.duration = duration;
        af.rt_duration = 0;
        af.location = APPLY_NONE;
	     af.bitvector = &AFF_HEALING_SALVE;
	     affect_to_char( victim, &af, NULL, TRUE );

        if (ch == victim)
        {
            print_char( ch, "Twoje rany zaczynaj± piec, czujesz ¿e bêd± goiæ siê szybciej.\n\r" );
        }
        else
        {
            if ( victim->position > POS_SLEEPING )
            {
                act( "Przyspieszasz gojenie ran $C.", ch, NULL, victim, TO_CHAR );
                act( "Twoje rany zaczynaj± piec, czujesz, ¿e bêd± goiæ siê szybciej.", ch, NULL, victim, TO_VICT );
            }
            else if ( victim->position == POS_SLEEPING)
            {
                act( "Przyspieszasz gojenie ran $C.", ch, NULL, victim, TO_CHAR );
            }
            else
                act( "Przyspieszasz gojenie ran $C.", ch, NULL, victim, TO_CHAR );
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
   }
   else
      {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
	     return;
      }
}

/*Pieczêæ s³abo¶ci - kl±twa os³abiaj±ca si³ê uderzeñ wroga. Szaman poch³ania czê¶æ mocy wroga.
  Wróg mo¿e posiadaæ na sobie tylko jedn± kl±twe. */
void spell_seal_of_weakness( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int spirit_power = get_skill(ch, gsn_invoke_spirit)/3;

	if ( ch == victim )
		{
			   send_to_char( "Nie dasz rady tego zrobiæ.\n\r", ch );
			   return;
		}

	if ( is_affected( victim, gsn_seal_of_weakness ) ||
	     IS_AFFECTED( victim, AFF_SEAL_OF_ATROCITY ) || //AFF_ jest wazny
	     is_affected( victim, gsn_seal_of_despair ) ||
	     is_affected( victim, gsn_seal_of_doom ) )
	{
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

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		act( "Nie uda³o ci siê przekln±æ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

	duration = 5 + spirit_power / 6;

   if ( check_shaman_invoke ( ch ) == TRUE )
   {
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = spirit_power/3;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_DAMROLL;
	af.modifier = -(spirit_power/6);
	af.bitvector = &AFF_NONE;
	affect_to_char( victim, &af, "Zosta³<&e¶/a¶/o¶> os³abion<&y/a/e>", TRUE );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = spirit_power/3;
	af.duration =  2; af.rt_duration = 0;
	af.location  = APPLY_DAMROLL;
	af.modifier  = 1;
	affect_to_char( ch, &af, NULL, FALSE  );

		act( "$n celuje w $C wyci±gniêt± rêk±.", ch, NULL, victim, TO_NOTVICT );
		act( "Czujesz jak si³a odp³ywa z twych r±k.", ch, NULL, victim, TO_VICT );
		act( "Celujesz w $C wyci±gniêt± rêk± i poch³aniasz jego si³ê.", ch, NULL, victim, TO_CHAR );
	return;
  }
  else
  {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
		return;
  }
}

//Pierwszy z darów przodków - wiedza przodków zwiêksza si³e wszystkich innych szamañskich czarów.
void spell_ancestors_wisdom( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int  af_level = spirit_power / 2;

	if ( is_affected( victim, gsn_ancestors_wisdom ) ||
	     is_affected( victim, gsn_ancestors_protection ) ||
	     is_affected( victim, gsn_ancestors_favor ) ||
	     is_affected( victim, gsn_ancestors_vision )  ||
	     is_affected( victim, gsn_ancestors_fury ) )
	{
   	act( "Przywo³a³e¶ ju¿ b³ogos³awieñstwo przodków.", ch, NULL, victim, TO_CHAR );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = 8; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_ANCESTORS_WISDOM;
	     affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Przywo³ujesz duchy przodków, które skupiaj± siê dooko³a ciebie szeptaj±c ci do ucha zapomniane sekrety.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów przodków.\n\r", ch );
	     return;
      }
}

//Drugi z darow przodkow - protekcja pozwala zwiekszyc odpornosci szamana na wiele negatywnych efektow.
void spell_ancestors_protection( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	if ( is_affected( victim, gsn_ancestors_wisdom ) ||
	     is_affected( victim, gsn_ancestors_protection ) ||
	     is_affected( victim, gsn_ancestors_favor ) ||
	     is_affected( victim, gsn_ancestors_vision )  ||
	     is_affected( victim, gsn_ancestors_fury ) )
	{
   	act( "Przywo³a³e¶ ju¿ b³ogos³awieñstwo przodków.", ch, NULL, victim, TO_CHAR );
		return;
	}


      if ( check_shaman_invoke ( ch ) == TRUE )
      {
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = get_skill(ch, gsn_invoke_spirit)/9;
	af.duration = 8;
	af.rt_duration = 0;
	af.location = APPLY_RESIST;

		af.modifier = RESIST_PIERCE;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_SLASH;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_BASH;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_FIRE;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_ELECTRICITY;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_COLD;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_ACID;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_POISON;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_NEGATIVE;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_MENTAL;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_HOLY;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

      af.modifier    = RESIST_SOUND;
		af.bitvector = &AFF_ANCESTORS_PROTECTION;
		affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Przywo³ujesz duchy przodków, które skupiaj± siê dooko³a ciebie chroni±c ciê przed zagro¿eniem.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów przodków.\n\r", ch );
	     return;
      }
}

//Trzeci z darów przodków - zwiêksza d³ugo¶æ czarów leczniczych rzucanych przez nasz± postaæ.
void spell_ancestors_favor( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int  af_level = spirit_power / 2;

	if ( is_affected( victim, gsn_ancestors_wisdom ) ||
	     is_affected( victim, gsn_ancestors_protection ) ||
	     is_affected( victim, gsn_ancestors_favor ) ||
	     is_affected( victim, gsn_ancestors_vision )  ||
	     is_affected( victim, gsn_ancestors_fury ) )
	{
   	act( "Przywo³a³e¶ ju¿ b³ogos³awieñstwo przodków.", ch, NULL, victim, TO_CHAR );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = 8; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_ANCESTORS_FAVOR;
	     affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Przywo³ujesz duchy przodków i wrêcz czujesz ca³ym sob± przep³ywaj±c± dooko³a ciebie ¿yciodajn± energiê.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów przodków.\n\r", ch );
	     return;
      }
}

//Czwarty z darów przodków - wizja przodków pozwala nam zobaczyæ to, co do tej pory by³o ukryte przed wzrokiem.
void spell_ancestors_vision( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int  af_level = spirit_power / 2;

	if ( is_affected( victim, gsn_ancestors_wisdom ) ||
	     is_affected( victim, gsn_ancestors_protection ) ||
	     is_affected( victim, gsn_ancestors_favor ) ||
	     is_affected( victim, gsn_ancestors_vision )  ||
	     is_affected( victim, gsn_ancestors_fury ) )
	{
   	act( "Przywo³a³e¶ ju¿ b³ogos³awieñstwo przodków.", ch, NULL, victim, TO_CHAR );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {

   //Stripujemy detecta jak ma, nie mówi±c o tym - nie chcemy by siê dublowa³.
   if ( IS_AFFECTED( victim, AFF_DETECT_INVIS ) )
   affect_strip( victim, gsn_detect_invis );
   //J/W tyle ¿e detect hidden.
	if ( IS_AFFECTED( victim, AFF_DETECT_HIDDEN ) )
   affect_strip( victim, gsn_detect_hidden );

   af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 8;
	af.rt_duration = 0;
	af.modifier = 0;
	af.location = APPLY_NONE;
	af.bitvector = &AFF_DETECT_INVIS;
	affect_to_char( victim, &af, NULL, FALSE );

	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = level;
	af.duration = 8;
	af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_DETECT_HIDDEN;
	affect_to_char( victim, &af, NULL, FALSE );

   af.where = TO_AFFECTS;
	af.type = sn;
	af.level = af_level;
	af.duration = 8; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = &AFF_ANCESTORS_VISION;
	affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Przywo³ujesz duchy przodków i otwierasz ponownie oczy na ¶wiat, na rzeczy które by³y dotychczas ukryte i niewidzialne.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów przodków.\n\r", ch );
	     return;
      }
}

/*Pi±ty z darów przodków - furia przodków. Daje szanse na dodatkowy atak (zale¿n± od skilla invoke - 20-40% -
  ale blokuje uciekanie z walki na ca³y czas trwania (8 ticków).*/
void spell_ancestors_fury( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int  af_level = spirit_power / 2;

	if ( is_affected( victim, gsn_ancestors_wisdom ) ||
	     is_affected( victim, gsn_ancestors_protection ) ||
	     is_affected( victim, gsn_ancestors_favor ) ||
	     is_affected( victim, gsn_ancestors_vision )  ||
	     is_affected( victim, gsn_ancestors_fury ) )
	{
   	act( "Przywo³a³e¶ ju¿ b³ogos³awieñstwo przodków.", ch, NULL, victim, TO_CHAR );
		return;
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = 8; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_ANCESTORS_FURY;
	     affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Przywo³ujesz duchy przodków, czujesz narastaj±cy w twych ¿y³ach gniew.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów przodków.\n\r", ch );
	     return;
      }
}
//Spirit shackle - hold na niematerialne widma i przybyszów z innych sfer.
void spell_spirit_shackle( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration, spirit_power = get_skill(ch, gsn_invoke_spirit)/3;;


	/*sprawdzanie bez powiadomienia bledu*/
	if ( !IS_SET( race_table[ GET_RACE( victim ) ].type , OUTSIDER )
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

    // stripujemy invisa je¿eli target jest graczem
    if (!IS_NPC(victim))
    {
        strip_invis(ch, TRUE, TRUE);
    }


	duration = 1 + ( spirit_power / 10 );

 if ( check_shaman_invoke ( ch ) == TRUE )
    {
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
   else
   {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów przodków.\n\r", ch );
	     return;
   }
}

//Dismiss outsider - odsy³anie istot z innych planów i widm: dla maga, druida i szamana.
void spell_dismiss_outsider( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	CHAR_DATA * vmaster;
	int chance, hp_bonus, spirit_power = get_skill(ch, gsn_invoke_spirit)/3;;;

	DEBUG_INFO( "do_cast:spell_dismiss_outsider:1");

	if ( victim == NULL )
	{
		send_to_char( "Kogo chcesz odes³aæ?", ch);
		return;
	}
	else if ( victim == ch )
	{
		send_to_char( "No co ty, sam<&/a/o> siebie chcesz odes³aæ?", ch);
		return;
	}

	DEBUG_INFO( "do_cast:spell_dismiss_outsider:2");

	vmaster = victim->master;

	if ( vmaster == NULL )
		vmaster = victim;

	if ( !IS_SET( race_table[ victim->race ].type, OUTSIDER ) )
	{
		act( "$N nie jest pozasferowcem.", ch, NULL, victim, TO_CHAR );
		return;
	}
	if ( !EXT_IS_SET( victim->act, ACT_NO_EXP ) || !IS_NPC( victim ) )
	{
		switch ( victim->sex )
		{
			case 0:
				act( "$N nie zosta³o przywo³ane.", ch, NULL, victim, TO_CHAR );
				break;
			case 1:
				act( "$N nie zosta³ przywo³any.", ch, NULL, victim, TO_CHAR );
				break;
			case 2:
				act( "$N nie zosta³a przywo³ana.", ch, NULL, victim, TO_CHAR );
				break;
		}
		return;
	}

	if ( vmaster == ch )
	{
		chance = URANGE(80,80+number_range(1,15),95);
	}
	else
	{
		chance = 55;
		chance += ( get_curr_stat_deprecated( ch, STAT_LUC ) - 10 ) / 2;
		chance -= ( get_curr_stat_deprecated( victim, STAT_LUC ) - 10 ) / 2;
		if ( ch->class == CLASS_SHAMAN )
		{ chance += spirit_power * 4; }
		else
		{ chance += level * 4; }
		chance -= victim->level / 2;
		chance -= vmaster->level * 3;
		chance -= get_curr_stat_deprecated( vmaster, STAT_WIS ) - 10;
		chance += get_curr_stat_deprecated( ch, STAT_WIS ) - 10;
		hp_bonus = 50 - ( 100 * victim->hit / UMAX( get_max_hp(victim), 1 ) );
		chance += hp_bonus / 2;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance -= 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_GOOD( vmaster ) && IS_EVIL( ch ) ) chance += 20;
		if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ) && IS_EVIL( vmaster ) && IS_GOOD( ch ) ) chance -= 20;
		if ( !IS_NPC( ch ) )
		{
			if ( ch->pcdata->mage_specialist >= 0 && IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
			{
				chance += chance/5;
			}
		}
		chance = URANGE( 5, chance, 75 );
	}

	if( ch->class == CLASS_DRUID )
	{
		act( "Koncentrujesz siê wzywaj±c moce natury by ciê wspomog³y, zamykasz oczy na chwilê po czym otwieraj±c je kierujesz moc w kierunku $Z.", ch, NULL, victim, TO_CHAR );
		act( "$n koncentruje siê przez chwilê po czym wymawia s³owa w dziwnym dialekcie. Zamyka na chwilê oczy po czym otwieraj±c je wypuszcza ze swych r±k promienie zielonej energii, które opl±tuj± $C.", ch, NULL, victim, TO_ROOM );

		if ( number_percent() < chance )
		{
			act( "$n znika w rozb³ysku ¶wiat³a!", victim, NULL, NULL, TO_ALL );
			extract_char( victim, TRUE );
		}
		else
			act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );
		return;
	}
   else if ( ch->class == CLASS_SHAMAN && check_shaman_invoke ( ch ) == TRUE )
   {
		act( "Koncentrujesz siê próbuj±c odes³aæ niepokornego ducha z powrotem tam gdzie jego miejsce, wskazujesz na $Z.", ch, NULL, victim, TO_CHAR );
		act( "$n koncentruje siê przez chwilê po czym wskazuje na $C.", ch, NULL, victim, TO_ROOM );

		if ( number_percent() < chance )
		{
			act( "$n znika w rozb³ysku ¶wiat³a!", victim, NULL, NULL, TO_ALL );
			extract_char( victim, TRUE );
		}
		else
			act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );
		return;
	}
	else
	{
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów przodków.\n\r", ch );
	     return;
	}

	act( "Koncentruj±c siê przywo³ujesz wielkie, staro¿ytne moce, które kr±¿± wokó³ twej ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_CHAR );
	act( "$n przywo³uje wielkie, staro¿ytne moce, które kr±¿± wokó³ $s ofiary przykuwaj±c j± do ziemi.", ch, NULL, victim, TO_ROOM );

	DEBUG_INFO( "do_cast:spell_dismiss_outsider:4");

	if ( number_percent() < chance )
	{
		act( "W nag³ym rozb³ysku ¶wiat³a $n znika.", victim, NULL, NULL, TO_ALL );
		extract_char( victim, TRUE );
	}
	else
	{
		act( "Sylwetka $z migocze przez chwilê, jednak nic siê nie dzieje.", victim, NULL, NULL, TO_ALL );
	}

	return;
}
// Spirit of Life - czar zwiêkszajacy regeneracjê celu
void spell_spirit_of_life( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int duration;

   if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) )
   {
        send_to_char( "To siê nie uda.\n\r", ch );
        return;
   }

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

     duration = number_range(1, 2);

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_FAVOR ) )
	{
     duration = number_range(2, 4);
	}

   if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = spirit_power/2;
	     af.duration = duration; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_SPIRIT_OF_LIFE;
	     affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Czujesz lecznic± energie rozlewaj±c± siê po twoim ciele.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
	     return;
      }
}

// Breath of life - czar znacznie zwiêkszaj±cy regeneracje celu, oprócz tego ma szansê usun±æ efekt zatrucia lub choroby.
void spell_breath_of_life( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int duration, af_level = spirit_power/2;

	if ( is_undead(victim) || IS_SET(victim->form, FORM_CONSTRUCT) )
	{
		send_to_char( "To siê nie uda.\n\r", ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
		spirit_power += 5;
	}

	duration = number_range(1, 2);

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_FAVOR ) )
	{
		duration = number_range(2, 4);
	}

	if ( check_shaman_invoke ( ch ) == TRUE )
	{
		af.where = TO_AFFECTS;
		af.type = sn;
		af.level = af_level;
		af.duration = duration; af.rt_duration = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &AFF_BREATH_OF_LIFE;
		affect_to_char( victim, &af, NULL, TRUE );

		send_to_char( "Czujesz potê¿n± i relaksuj±c± moc wype³niaj±c± ca³e twoje cia³o.\n\r", ch );

		if ( is_affected( victim, gsn_poison ) && number_percent() > (100 - spirit_power) )
		{
			affect_strip( victim, gsn_poison );
			send_to_char( "Trucizna przestaje kr±¿yæ w twojej krwi.\n\r", victim );
			act( "$n wygl±da o wiele lepiej.", victim, NULL, NULL, TO_ROOM );
		}

		if ( is_affected( victim, gsn_plague ) && number_percent() > (100 - spirit_power) )
		{
			affect_strip( victim, gsn_plague );
			send_to_char( "Objawy choroby cofaj± siê.\n\r", victim );
			act( "$n wygl±da o wiele lepiej.", victim, NULL, NULL, TO_ROOM );
		}


	}
	else
	{
		send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
		return;
	}
}

void spell_spiritual_guidance( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;

	int  spirit_power = get_skill(ch, gsn_invoke_spirit)/3;
	int duration, af_level = spirit_power / 2;

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

   duration = 1 + spirit_power / 5;

	if ( IS_AFFECTED( victim, AFF_SPIRITUAL_GUIDANCE ) )
	{
	  if ( victim == ch )
	 {
	   send_to_char( "$N posiada juz opiekê duchów.\n\r", ch);
		return;
	 }
	 else
	 {
	   send_to_char( "Posiadasz ju¿ na sobie opiekê duchów.\n\r", ch );
		return;
	 }
	}

      if ( check_shaman_invoke ( ch ) == TRUE )
      {
        af.where = TO_AFFECTS;
	     af.type = sn;
	     af.level = af_level;
	     af.duration = duration; af.rt_duration = 0;
	     af.location = APPLY_NONE;
	     af.modifier = 0;
	     af.bitvector = &AFF_SPIRITUAL_GUIDANCE;
	     affect_to_char( victim, &af, NULL, TRUE );

		  send_to_char( "Czujesz, ¿e duchy zaczynaj± ci sprzyjaæ prowadz±c pewniej tw± d³oñ w walce.\n\r", victim );
		  send_to_char( "Sprowadzasz opiekê duchów.\n\r", ch );
      }
      else
      {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
	     return;
      }
}


/*Pieczêæ okrucieñstwa - obcina skille wroga o 1/3. Narazie wy³±czam bo buguje muda.
  Wróg mo¿e posiadaæ na sobie tylko jedn± kl±twe.*/
void spell_seal_of_atrocity( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	AFFECT_DATA *aff_count;
	int duration, count = 0;
	int spirit_power = get_skill(ch, gsn_invoke_spirit)/3;

	if ( ch == victim )
		{
			   send_to_char( "Nie dasz rady tego zrobiæ.\n\r", ch );
			   return;
		}

	if ( is_affected( victim, gsn_seal_of_weakness ) || 
	     IS_AFFECTED( victim, AFF_SEAL_OF_ATROCITY ) || //AFF_ jest wazny
	     is_affected( victim, gsn_seal_of_despair ) ||
	     is_affected( victim, gsn_seal_of_doom ) ) 
	{
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

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		act( "Nie uda³o ci siê przekln±æ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 3;
	}

	duration = 5 + spirit_power / 6;

   if ( check_shaman_invoke ( ch ) == TRUE )
   {
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = spirit_power/3;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = spirit_power;
	af.bitvector = &AFF_SEAL_OF_ATROCITY;
	affect_to_char( victim, &af, "Twoje ruchy s± bardziej niezdarne niz zwykle", TRUE );

	for( aff_count = ch->affected ; aff_count; aff_count = aff_count->next )
	{
	    if( aff_count->type == skill_lookup( "seal of atrocity" ) && aff_count->location == APPLY_HITROLL )
                count++;
	}

	if( count < 3 )
	{
	    af.where = TO_AFFECTS;
	    af.type = sn;
	    af.level = spirit_power/3;
	    af.duration =  2; af.rt_duration = 0;
	    af.location  = APPLY_HITROLL;
	    af.modifier  = 1;
	    af.bitvector = &AFF_NONE;
	    affect_to_char( ch, &af, NULL, FALSE  );

	    act( "$n celuje w $C wyci±gniêt± rêk±.", ch, NULL, victim, TO_NOTVICT );
	    act( "Czujesz jak pewno¶æ odp³ywa z twych r±k.", ch, NULL, victim, TO_VICT );
	    act( "Celujesz w $C wyci±gniêt± rêk± i poch³aniasz je<&go/j/go> pewno¶æ w pos³ugiwania siê umiejêtno¶ciami.", ch, NULL, victim, TO_CHAR );
	}
	else
	{
	    act( "$n celuje w $C wyci±gniêt± rêk±.", ch, NULL, victim, TO_NOTVICT );
	    act( "Czujesz jak pewno¶æ odp³ywa z twych r±k.", ch, NULL, victim, TO_VICT );
	    act( "Celujesz w $C wyci±gniêt± rêk± i ograniczajac je<&go/j/go> pewno¶æ w pos³ugiwania siê umiejêtno¶ciami.", ch, NULL, victim, TO_CHAR );
	}

	return;
  }
  else
  {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
		return;
  }
}

//Pieczêæ desperacji - blokuje mo¿liwo¶æ zadawania krytycznych ciosów wrogowi. Wy³±czam bo buguje muda.
// Wróg mo¿e posiadaæ na sobie tylko jedn± kl±twe.
void spell_seal_of_despair( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int spirit_power = get_skill(ch, gsn_invoke_spirit)/3;

	if ( ch == victim )
		{
			   send_to_char( "Nie dasz rady tego zrobiæ.\n\r", ch );
			   return;
		}

	if ( is_affected( victim, gsn_seal_of_weakness ) ||
	     IS_AFFECTED( victim, AFF_SEAL_OF_ATROCITY ) || //AFF_ jest wazny
	     is_affected( victim, gsn_seal_of_despair ) ||
	     is_affected( victim, gsn_seal_of_doom ) )
	{
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

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		act( "Nie uda³o ci siê przekln±æ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 3;
	}

	duration = 5 + spirit_power / 6;

   if ( check_shaman_invoke ( ch ) == TRUE )
   {
	af.where = TO_AFFECTS;
	af.type = sn;
	af.level = spirit_power/3;
	af.duration = duration; af.rt_duration = 0;
	af.location = APPLY_NONE;
	af.modifier = -(spirit_power/6);
	af.bitvector = &AFF_SEAL_OF_DESPAIR;
	affect_to_char( victim, &af, "Przestajesz widzieæ ods³oniête miejsa na ciele przeciwników", TRUE );

		act( "$n celuje w $C wyci±gniêt± rêk±.", ch, NULL, victim, TO_NOTVICT );
		act( "Czujesz jak sylwetka twego wroga rozmywa ci siê w oczach.", ch, NULL, victim, TO_VICT );
		act( "Celujesz w $C wyci±gniêt± rêk± i ukrywasz wszystkie s³abe punkty przed je<&go/j/go> wzrokiem.", ch, NULL, victim, TO_CHAR );
	return;
  }
  else
  {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
		return;
  }
}

/*Pieczêæ zag³ady - pozbawia wroga losowego zmys³u.
  Wróg mo¿e posiadaæ na sobie tylko jedn± kl±twe. */
void spell_seal_of_doom( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
	CHAR_DATA * victim = ( CHAR_DATA * ) vo;
	AFFECT_DATA af;
	int duration;
	int spirit_power = get_skill(ch, gsn_invoke_spirit)/3;

	if ( ch == victim )
		{
			   send_to_char( "Nie dasz rady tego zrobiæ.\n\r", ch );
			   return;
		}

	if ( is_affected( victim, gsn_seal_of_weakness ) ||
	     IS_AFFECTED( victim, AFF_SEAL_OF_ATROCITY ) || //AFF_ jest wazny
	     is_affected( victim, gsn_seal_of_despair ) ||
	     is_affected( victim, gsn_seal_of_doom ) )
	{
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

	if ( saves_spell_new( victim, skill_table[ sn ].save_type, skill_table[ sn ].save_mod, ch, sn ) )
	{
		act( "Nie uda³o ci siê przekln±æ $Z.", ch, NULL, victim, TO_CHAR );
		return;
	}


	if ( IS_AFFECTED( ch, AFF_ANCESTORS_WISDOM ) )
	{
     spirit_power += 5;
	}

	duration = 3 + spirit_power / 6;

   if ( check_shaman_invoke ( ch ) == TRUE )
   {

	switch ( number_range (1,3))//Losowanie który zmys³
	{
    case 1://o¶lepienie
            if ( IS_AFFECTED( victim, AFF_BLIND ) || IS_AFFECTED(victim,AFF_PERFECT_SENSES )  )
            {
               send_to_char( "Duchy odpowiedzia³y na twoje wezwanie, ale nic siê nie dzieje.\n\r", ch );
               return;
            }
            else
            {
             af.where = TO_AFFECTS;
             af.type = sn;
             af.level = spirit_power/3;
             af.location = APPLY_NONE;
             af.modifier = 0;
	          af.duration = duration;
	          af.rt_duration = 0;
	          af.bitvector = &AFF_BLIND;
	          affect_to_char( victim, &af, "Zosta³e¶ o¶lepiony", TRUE );

             act( "$n celuje w $C wyci±gniêt± rêk±.", ch, NULL, victim, TO_NOTVICT );
             act( "Czujesz jak ¶wiat³o opuszcza twe oczy.", ch, NULL, victim, TO_VICT );
             act( "Celujesz w $C wyci±gniêt± rêk± i pozbawiasz <&go/j±/je> wzroku.", ch, NULL, victim, TO_CHAR );
             return;
             break;
            }
      case 2://Uciszenie
            if ( IS_AFFECTED( victim, AFF_SILENCE ) || IS_AFFECTED(victim,AFF_PERFECT_SENSES )  )
            {
               send_to_char( "Duchy odpowiedzia³y na twoje wezwanie, ale nic siê nie dzieje.\n\r", ch );
               return;
            }
           else
            {
             af.where = TO_AFFECTS;
             af.type = sn;
             af.level = spirit_power/3;
             af.location = APPLY_NONE;
             af.modifier = 0;
	          af.duration = duration;
	          af.rt_duration = 0;
	          af.bitvector = &AFF_SILENCE;
	          affect_to_char( victim, &af, "Zosta³e¶ uciszony", TRUE );

             act( "$n celuje w $C wyci±gniêt± rêk±.", ch, NULL, victim, TO_NOTVICT );
             act( "Czujesz ¿e ¿adne s³owo nie przeci¶nie ci siê przez gard³o.", ch, NULL, victim, TO_VICT );
             act( "Celujesz w $C wyci±gniêt± rêk± i pozbawiasz <&go/j±/je> mowy.", ch, NULL, victim, TO_CHAR );
             return;
             break;
            }
      case 3://Og³uszenie
            if ( IS_AFFECTED( victim, AFF_DEAFNESS ) || IS_AFFECTED(victim,AFF_PERFECT_SENSES )  )
            {
               send_to_char( "Duchy odpowiedzia³y na twoje wezwanie, ale nic siê nie dzieje.\n\r", ch );
               return;
            }
           else
            {
             af.where = TO_AFFECTS;
             af.type = sn;
             af.level = spirit_power/3;
             af.location = APPLY_NONE;
             af.modifier = 0;
	          af.duration = duration;
	          af.rt_duration = 0;
	          af.bitvector = &AFF_DEAFNESS;
	          affect_to_char( victim, &af, "Zosta³e¶ og³uszony", TRUE );

             act( "$n celuje w $C wyci±gniêt± rêk±.", ch, NULL, victim, TO_NOTVICT );
             act( "Twoje uszy przestaj± s³yszeæ cokolwiek.", ch, NULL, victim, TO_VICT );
             act( "Celujesz w $C wyci±gniêt± rêk± i pozbawiasz <&go/j±/je> s³uchu.", ch, NULL, victim, TO_CHAR );
             return;
             break;
            }
       default: break;
     }
   }
  else
  {
        send_to_char( "Nie uda³o ci siê przywo³aæ duchów.\n\r", ch );
		return;
  }
}

