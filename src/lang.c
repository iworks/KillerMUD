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
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: lang.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/lang.c $
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
#include <ctype.h>
#include <stdarg.h>
#include "merc.h"
#include "lang.h"
#include "recycle.h"

#define TONGUE_FILE "../system/lang.txt"

LANG_DATA *		first_lang;
char *names_alias(CHAR_DATA *ch, int type, int val);

struct lang_type lang_table[MAX_LANG] =
{
 { "wspolny", 		LANG_COMMON  	},
 { "elfi", 			LANG_ELVEN	},
 { "krasnoludzki", 	LANG_DWARVEN	},
 { "pixie", 		LANG_PIXIE	},
 { "ogre", 			LANG_OGRE	},
 /* 5 */
 { "orcish", 		LANG_ORCISH	},
 { "trollish", 		LANG_TROLLISH	},
 { "rodent", 		LANG_RODENT	},
 { "insectoid", 	LANG_INSECTOID	},
 { "mammal", 		LANG_MAMMAL	},
 /* 10 */
 { "reptile", 		LANG_REPTILE	},
 { "dragon", 		LANG_DRAGON	},
 { "spiritual", 	LANG_SPIRITUAL	},
 { "magical", 		LANG_MAGICAL	},
 { "goblin", 		LANG_GOBLIN	},
 /* 15 */
 { "god", 			LANG_GOD	},
 { "ancient", 		LANG_ANCIENT	},
 { "halfling", 		LANG_HALFLING	},
 { "gith", 			LANG_GITH	},
 { "telepatic", 	LANG_TELEPATIC	},
 { "unknown", 		LANG_UNKNOWN	}
};


void print_char(CHAR_DATA *ch, char *fmt, ...)
{
    char buf[MAX_STRING_LENGTH*2];
    va_list args;

    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    send_to_char(capitalize(buf), ch);
}

LANG_DATA *get_lang(const char *name)
{
    LANG_DATA *lng;

    for (lng = first_lang; lng; lng = lng->next)
	if (!str_cmp(lng->name, name))
	    return lng;
    return NULL;
}

int get_langflag( char *flag )
{
	int x;

	for ( x = 0; lang_table[x].bit != LANG_UNKNOWN; x++ )
		if ( !str_cmp( flag, lang_table[x].name ) )
			return lang_table[x].bit;

	return LANG_UNKNOWN;
}

int get_langnum( char *flag )
{
        int x;

        for ( x = 0; lang_table[x].bit != LANG_UNKNOWN; x++ )
            if ( !str_prefix( flag, lang_table[x].name ) )
                return x;
        return -1;
}

int knows_language( CHAR_DATA *ch, int language, CHAR_DATA *cch )
{
	int value, mod=0;

	if ( !IS_NPC( ch ) )
	{
		// uwaga, wy³±czamy jêzyki: god, telepatic, unknown
		if(
			IS_AFFECTED(ch, AFF_COMPREHEND_LANGUAGES )
			&& lang_table[language].bit != LANG_GOD
			&& lang_table[language].bit != LANG_TELEPATIC
			&& lang_table[language].bit != LANG_UNKNOWN
		)
		{
			value = 100;
		}
		else
		{
			value=ch->pcdata->language[language];
			mod=ch->pcdata->lang_mod[language];
		}

		if( get_curr_stat_deprecated(ch, STAT_INT) < 4 )
		{
			mod = 0;
			value = value/4;
		}

		return URANGE(0, value+mod, 100);
	}
	else
		return (ch->pIndexData->languages & lang_table[language].bit ? 100 : 0);
}

void fread_cnv(FILE * fp, LCNV_DATA **first_cnv)
{
    LCNV_DATA *cnv;
    char letter;

    for (;;)
    {
	letter = fread_letter(fp);

	if (letter == '~' || letter == EOF)
	    break;

	ungetc(letter, fp);

	CREATE( cnv, LCNV_DATA, 1 );

	cnv->old = str_dup(fread_word(fp));
	cnv->olen = strlen(cnv->old);
	cnv->new = str_dup(fread_word(fp));
	cnv->nlen = strlen(cnv->new);
	cnv->next = NULL;
	fread_to_eol(fp);

	if( *first_cnv==NULL )
	{

	 *first_cnv=cnv;

	}
	else
	{

	 cnv->next=*first_cnv;
	 *first_cnv=cnv;

	}
    }
}

void load_tongues()
{
    FILE *fp;
    LANG_DATA *lng;
    char *word;
    char letter;

    if (!(fp=fopen(TONGUE_FILE, "r")))
    {
	perror("Load_tongues");
	return;
    }

    for (;;)
    {
	letter = fread_letter(fp);

	if (letter == EOF)
	    return;

	else if (letter == '*')
	{
	    fread_to_eol(fp);
	    continue;
	}

	else if (letter != '#')
	{
	    bug("Letter '%c' not #.", letter);
	    exit(0);
	}

	word = fread_word(fp);

	if (!str_cmp(word, "end"))
	    return;

	fread_to_eol(fp);

	CREATE( lng, LANG_DATA, 1 );

	lng->name = str_dup(word);
	lng->next = NULL;

	fread_cnv(fp, &lng->first_precnv);
	lng->alphabet = fread_string(fp);

	fread_cnv(fp, &lng->first_cnv);
	fread_to_eol(fp);

	if(!first_lang)
	 first_lang=lng;
	else
	{
	 lng->next = first_lang;
	 first_lang = lng;
	}
    }
    return;
}

void do_speak( CHAR_DATA *ch, char *argument )
{
	int langs;
	char arg[MAX_INPUT_LENGTH];

    /* dajemy standardowy tekst jak go¶æ jest pod wp³ywem 'silence' */
    if (IS_AFFECTED(ch, AFF_SILENCE))
    {
		send_to_char(AFF_SILENCE_TEXT, ch);
		return;
    }

	if(argument[0]=='\0')
	{
         print_char(ch, "Mowisz jezykiem :%s\n\r", lang_table[ch->speaking%32].name);
	       return;
	}

	argument = one_argument(argument, arg );



	for ( langs = 0; lang_table[langs].bit != LANG_UNKNOWN; langs++ )
		if ( !str_prefix( arg, lang_table[langs].name ) )
			if ( knows_language( ch, langs, ch ) )
			{
				ch->speaking = langs;
				print_char( ch, "Mowisz teraz jezykiem: %s.\n\r", lang_table[langs].name );
				return;
			}

	send_to_char( "Nie znasz tego jezyka.\n\r", ch );
	return;
}

char *translate(int percent, const char *in, const char *name)
{
    LCNV_DATA *cnv;
    static char buf[2 * MAX_INPUT_LENGTH ];
    char buf2[ 2 * MAX_INPUT_LENGTH ];
    const char *pbuf;
    char *pbuf2 = buf2;
    LANG_DATA *lng;

    if ( percent > 99 )
	return (char *) in;

    if ( !(lng=get_lang(name)) )
	if ( !(lng = get_lang("default")) )
	    return (char *) in;

    for (pbuf = in; *pbuf;)
    {
	for (cnv = lng->first_precnv; cnv; cnv = cnv->next)
	{
	    if (!str_prefix(cnv->old, pbuf))
	    {
		if ( (percent > 0) &&  ( ((number_percent() + number_percent()) /2) < percent ))
		{
		    strncpy(pbuf2, pbuf, cnv->olen);
		    pbuf2[cnv->olen] = '\0';
		    pbuf2 += cnv->olen;
		}
		else
		{
		    strcpy(pbuf2, cnv->new);
		    pbuf2 += cnv->nlen;
		}
		pbuf += cnv->olen;
		break;
	    }
	}
	if (!cnv)
	{
	    if (isalpha(*pbuf) && (!percent || ((number_percent() + number_percent())/2) > percent) )
	    {
		*pbuf2 = lng->alphabet[LOWER(*pbuf) - 'a'];
		if ( isupper(*pbuf) )
		    *pbuf2 = UPPER(*pbuf2);
	    }
	    else
		*pbuf2 = *pbuf;
	    pbuf++;
	    pbuf2++;
	}
    }
    *pbuf2 = '\0';
    for (pbuf = buf2, pbuf2 = buf; *pbuf;)
    {
	for (cnv = lng->first_cnv; cnv; cnv = cnv->next)
	    if (!str_prefix(cnv->old, pbuf))
	    {
		strcpy(pbuf2, cnv->new);
		pbuf += cnv->olen;
		pbuf2 += cnv->nlen;
		break;
	    }
	if (!cnv)
	    *(pbuf2++) = *(pbuf++);
    }
    *pbuf2 = '\0';
    return buf;
}

char *translate_gnomish( char *string )
{
    static char buf[ MAX_STRING_LENGTH ];
    char *point;

    buf[0] = '\0';
    point = buf;

    while ( *string )
    {
        if ( isspace( *string ) )
        {
            if ( !isspace( *( string + 1 ) ) )
                *point++ = UPPER (*++string);
            string++;
        }
        else
            *point++ = *string++;
    }

    *point = '\0';

    buf[0] =  UPPER (buf[0]);
    return buf;
}

void do_languages( CHAR_DATA *ch, char *argument )
{
	int langs,col=0;

	print_char(ch,"{b\n\r==<>==<>==<>==<>==<>===< {GZnasz nastêpuj±ce jêzyki {b>===<>==<>==<>==<>==<>==<>=={x\n\r");

	for ( langs = 0; lang_table[langs].bit != LANG_UNKNOWN; langs++ )
			if ( knows_language( ch, langs, ch ) )
			{
                if(++col%2 == 1)
                    send_to_char("\n\r",ch);

                print_char(ch,"{G%-15s{x %-20s",lang_table[langs].name,
                names_alias(ch,3,langs));
			}

	send_to_char("{b\n\r\n\r==<>==<>==<>==<>==<>==<>==<>==<>==<>==<=>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x",ch);
	send_to_char("\n\r",ch);
	return;
}

