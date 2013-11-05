/*********************************************************************
 *   ROM 2.4 is copyright 1993-1998 Russ Taylor                      *
 *        Russ Taylor (rtaylor@hypercube.org)                        *
 *        Gabrielle Taylor (gtaylor@hypercube.org)                   *
 *        Brian Moore (zump@rom.org)                                 *
 *   By using this code, you have agreed to follow the terms of the  *
 *   ROM license, in the file Rom24/doc/rom.license                  *
 *                                                                   *
 * KILLER MUD is copyright 1999-2006 Killer MUD Staff (alphabetical) *
 *                                                                   *
 *********************************************************************
 *                                                                   *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti  ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor   ] *
 * Koper Tadeusz         (garloop@killer-mud.net        ) [Garloop ] *
 * Pietrzak Marcin       (marcin.pietrzak@mud.pl        ) [Gurthg  ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas  ] *
 * Skrzetnicki Krzysztof (tener@tenet.pl                ) [Tener   ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro    ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron   ] *
 *                                                                   *
 *********************************************************************/
/* $Id: names_alias.c 10701 2011-12-02 16:03:39Z illi $ */
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
#include "tables.h"
#include "lang.h"

#define SK 0
#define HP 1
#define MV 2
#define LG 3
#define GP 4
#define HU 5
#define TH 6



char *skillstr[10]={
	"¿enuj±co",
	"bardzo s³abo",
	"s³abo",
	"kiepsko",
	"poni¿ej ¶redniej",
	"¶rednio",
	"nie¼le",
	"dobrze",
	"bardzo dobrze",
	"mistrzowsko"
};

char *asteriskstr[8]=
{
    "{G********{x",
    "{G******* {x",
    "{g******  {x",
    "{f*****   {x",
    "{f****    {x",
    "{r***     {x",
    "{r**      {x",
    "{r*       {x"
};

char *movstrmale[5]={
	"{Gwypoczêty{x",
	"{glekko zmêczony{x",
	"{fzmêczony{x",
	"{rbardzo zmêczony{x",
	"{rzamêczony{x"
};
char *movstrfemale[5]={
	"{Gwypoczêta{x",
	"{glekko zmêczona{x",
	"{fzmêczona{x",
	"{rbardzo zmêczona{x",
	"{rzamêczona{x"
};

char *hungerstrmale[5]={
	"{Gnajedzony{x",
	"{gtrochê g³odny{x",
	"{fg³odny{x",
	"{rpotwornie g³odny{x",
	"{rwyg³odzony{x"
};
char *hungerstrfemale[5]={
	"{Gnajedzona{x",
	"{gtrochê g³odna{x",
	"{fg³odna{x",
	"{rpotwornie g³odna{x",
	"{rwyg³odzona{x"
};


char *thirststrmale[5]={
	"{Gnapojony{x",
	"{glekko spragniony{x",
	"{fspragniony{x",
	"{rpotwornie spragniony{x",
	"{rwysuszony{x"
};
char *thirststrfemale[5]={
	"{Gnapojona{x",
	"{gtrochê spragniona{x",
	"{fspragniona{x",
	"{rpotwornie spragniona{x",
	"{rwysuszona{x"
};

char *names_alias(CHAR_DATA *ch, int type, int val)
{
	int percent, ret=0;
	sh_int learned;

	switch(type)
	{
		case SK:
			if( val < 0 )
				return "null";

			learned=get_skill(ch,val);

			if ( learned == 100 )
				return "arcymistrzowsko";
			else
				return skillstr[((learned-1)/10)];

			break;

		case HP:
			ret = get_hp_notch(ch, TRUE);
			if(IS_NPC(ch) && IS_SET(ch->form, FORM_CONSTRUCT))
			{
				return (char *)condition_info_construct[ret];
			}
			else
			{
				return (char *)condition_info_all[ret];
			}

		case GP:
			return asteriskstr[get_hp_notch(ch, FALSE)];

		case MV:
			if (ch->max_move > 0)
				percent = (ch->move * 100) / ch->max_move;
			else
				percent = -1;

			if      (percent > 99) ret=0;
			else if (percent > 74) ret=1;
			else if (percent > 49) ret=2;
			else if (percent > 24) ret=3;
			else                   ret=4;


			return ch->sex == 2 ? movstrfemale[ret] : movstrmale[ret];

		case LG:

			if( IS_NPC(ch) ||  val < 0 )
			{
				return "null";
			}

			return skillstr[((knows_language( ch, val, ch ) -1)/10)];
		case HU:
			if (ch->condition[ COND_HUNGER ]  > 0)
				percent = ch->condition[ COND_HUNGER ] * 100 / EAT_FULL;
			else
				percent = -1;

			if      (percent > 99) ret=0;
			else if (percent > 74) ret=1;
			else if (percent > 49) ret=2;
			else if (percent > 24) ret=3;
			else                   ret=4;


			return ch->sex == 2 ? hungerstrfemale[ret] : hungerstrmale[ret];
		case TH:
			if (ch->condition[ COND_THIRST ]  > 0)
				percent = ch->condition[ COND_THIRST ] * 100 / DRINK_FULL;
			else
				percent = -1;

			if      (percent > 99) ret=0;
			else if (percent > 74) ret=1;
			else if (percent > 49) ret=2;
			else if (percent > 24) ret=3;
			else                   ret=4;


			return ch->sex == 2 ? thirststrfemale[ret] : thirststrmale[ret];

		default: return "unknow";
	}
}
