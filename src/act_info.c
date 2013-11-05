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
 * $Id: act_info.c 11469 2012-07-07 19:21:32Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/12.02/src/act_info.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lang.h"
#include "clans.h"
#include "progs.h"
#include "friend_who.h"
#include "todelete.h"	// delayed ch delete - by Fuyara
#include "projects.h"
#include "money.h"

sh_int get_caster( CHAR_DATA *ch );
/**
 * exping.c
 */
int get_percent_exp args ( ( CHAR_DATA *ch ) );
int get_hp_notch ( CHAR_DATA *ch, bool usePerfectSelf );
char *names_alias( CHAR_DATA * ch, int type, int val );


char *	const	where_name	[] =
{
	"<trzymane jako ¶wiat³o>        ",
	"<noszone na palcu lewej rêki>  ",
	"<noszone na palcu prawej rêki> ",
	"<noszone na szyi>              ",
	"<noszone na szyi>              ",
	"<noszone na torsie>            ",
	"<noszone na g³owie>            ",
	"<noszone na nogach>            ",
	"<noszone na stopach>           ",
	"<noszone na rêkach>            ",
	"<noszone na ramionach>         ",
	"<u¿ywane jako tarcza>          ",
	"<noszone wokó³ cia³a>          ",
	"<noszone jako pasek>           ",
	"<noszone na lewym nadgarstku>  ",
	"<noszone na prawym nadgarstku> ",
	"<pierwsza broñ>                ",
	"<trzymane w d³oni>             ",
	"<unosz±cy siê w pobli¿u>       ",
	"<druga broñ>                   ",
	"<trzymane w d³oniach>          ",
	"<przypiête do lewego ucha>     ",
	"<przypiête do prawego ucha>    "
};


/*
 * Local functions.
 */
char *  format_obj_to_char  args( ( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort ) );
void    show_list_to_char   args( ( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing ) );
void    show_char_to_char_0 args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    show_char_to_char_1 args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    show_char_to_char   args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool    check_blind         args( ( CHAR_DATA *ch ) );
void    create_rand_desc    args( ( ROOM_INDEX_DATA *pRoom, char *src_desc, char *dest ) );
int     get_cost            args( ( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy ) );
bool    mp_precommand_trigger   args( ( CHAR_DATA *mob, CHAR_DATA *victim, OBJ_DATA *obj, DO_FUN * fun, char *fun_name, char *argument ) );

#ifdef MCCP
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "telnet.h"
const char compress_on_str3 [] =
{
	IAC, WILL, TELOPT_COMPRESS, '\0'
};
const char compress2_on_str3 [] =
{
	IAC, WILL, TELOPT_COMPRESS2, '\0'
};
const char eor_on_str3 [] =
{
	IAC, WILL, TELOPT_EOR, '\0'
};
#endif

char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[ MAX_STRING_LENGTH ];
    AFFECT_DATA *damned;
    int i,count;

    buf[ 0 ] = '\0';

    if ( obj->pIndexData && IS_IMMORTAL( ch ) && IS_SET( ch->wiz_config, WIZCFG_SHOWVNUMS ) )
    {
        sprintf( buf, "{c[{C%5d{c]{x ", obj->pIndexData->vnum );
    }

    if ( ( fShort && ( obj->short_descr == NULL || obj->short_descr[ 0 ] == '\0' ) ) || ( obj->description == NULL || obj->description[ 0 ] == '\0' ) )
    {
        return buf;
    }
    /**
     * invis
     */
    if ( IS_OBJ_STAT( obj, ITEM_INVIS ) )
    {
        switch (obj->gender)
        {
            case GENDER_NIJAKI: // nijaki
                strcat( buf, "(niewidzialne) " );
                break;
            case GENDER_ZENSKI: // ¿eñski
                strcat( buf, "(niewidzialna) " );
                break;
            case GENDER_MESKOOSOBOWY: // mêskoosobowy
                strcat( buf, "(niewidzialni) " );
                break;
            case GENDER_ZENSKOOSOBOWY: // ¿eñskoosobowy
                strcat( buf, "(niewidzialne) " );
                break;
            case GENDER_NONE: // nieokre¶lony
            case GENDER_MESKI: // mêski
            default :
                strcat( buf, "(niewidzialny) " );
                break;
        }
    }
    /**
     * evil
     */
    if ( IS_AFFECTED( ch, AFF_DETECT_EVIL ) && IS_OBJ_STAT( obj, ITEM_EVIL ) )
    {
        strcat( buf, "{D({Rczerwona aura{D){x " );
    }
    /**
     * bless
     */
    if ( IS_AFFECTED( ch, AFF_DETECT_GOOD )	&& IS_OBJ_STAT( obj, ITEM_BLESS ) )
    {
        strcat( buf, "{D({Bb³êkitna aura{D){x " );
    }
    /**
     * magic
     */
    if ( IS_AFFECTED( ch, AFF_DETECT_MAGIC ) && IS_OBJ_STAT( obj, ITEM_MAGIC ) )
    {
        strcat( buf, "{D({Rm{Ga{Bg{Mi{Cc{Yz{Wn{R " );
        switch (obj->gender)
        {
            case  GENDER_NIJAKI: // nijaki
            case  GENDER_ZENSKOOSOBOWY: // ¿eñskoosobowy
                strcat( buf, "e" );
                break;
            case  GENDER_ZENSKI: // ¿eñski
                strcat( buf, "a" );
                break;
            case  GENDER_MESKOOSOBOWY: // mêskoosobowy
                strcat( buf, "i" );
                break;
            case  GENDER_NONE: // nieokre¶lony
            case  GENDER_MESKI: // mêski
            default :
                strcat( buf, "y" );
                break;
        }
        strcat( buf, "{D) " );
    }
    /**
     * glow
     */
    if ( IS_OBJ_STAT( obj, ITEM_GLOW ) )
    {
        strcat( buf, "{D({W¶wieci{D){x " );
    }
    if ( obj->item_type == ITEM_NOTEPAPER && obj->value[ 0 ] == 1 )
    {
        strcat( buf, "{D(zapisane){x " );
    }
    if ( IS_OBJ_STAT( obj, ITEM_TRANSMUTE ) )
    {
        strcat( buf, "{D({Rpulsuje {W¶wiat³em{D){x " );
    }
    if ( ( obj->item_type == ITEM_LIGHT ) && ( obj->value[4] != 0 ) )
    {
        strcat( buf, "{D({Wo¶wietla{D){x " );
    }
    for ( damned = obj->affected; damned != NULL; damned = damned->next )
    {
        if ( damned->type == gsn_damn_armor || damned->type == gsn_damn_weapon )
        {
            strcat( buf, "{D({Rkrwistoczerwony {rca³un{D){x " );
            break;
        }
    }
    if ( fShort )
    {
        if ( obj->short_descr != NULL )
        {
            strcat( buf, obj->short_descr );
        }
    }
    else if ( obj->description != NULL )
    {
        strcat( buf, obj->description );
    }
    if ( ( ch->class == CLASS_MAG || ch->pcdata->wiz_conf & W4 ) && obj->item_type == ITEM_SCROLL )
    {
        strcat( buf, " {D(zwój");
        if ( ch->level > obj->value[0] || ch->pcdata->wiz_conf & W4 )
        {
            // mag specjalista poznania lub ... stra¿nik
            if (ch->pcdata->mage_specialist == 3 || ch->pcdata->wiz_conf & W4 )
            {
                strcat( buf, ": ");
                count = 0;
                for (i=1;i<4;i++)
                {
                    if (obj->value[i] > 0)
                    {
                        if (count > 0)
                        {
                            strcat( buf, ", ");
                        }
                        strcat( buf, skill_table[obj->value[i]].name);
                        count++;
                    }
                }
            }
            else if (ch->pcdata->mage_specialist < 0) // mag ogólny
            {
                strcat( buf, ": ");
                count = 0;
                for (i=1;i<4;i++)
                {
                    if (obj->value[i] > 0)
                    {
                        count++;
                    }
                }
                switch (count)
                {
                    case 1:
                        strcat( buf, "jeden");
                        break;
                    case 2:
                        strcat( buf, "dwa");
                        break;
                    case 3:
                        strcat( buf, "trzy");
                        break;
                    case 4:
                        strcat( buf, "cztery");
                        break;
                    default:
                        strcat( buf, "pusty");
                        break;
                }
            }
        }
        strcat( buf, "){x " );
    }
    /**
     * reward_obj
     */
    if ( ch->pcdata && obj->pIndexData->vnum  ==  ch->pcdata->reward_obj )
    {
        if( ch->pcdata->hunt_time < current_time )
        {
            reset_hunt(ch);
        }
        else
        {
            strcat( buf, " {D(poszukiwany){x " );
        }
    }
    return buf;
}

/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
	char buf[ MAX_STRING_LENGTH ];
	BUFFER *output;
	char **prgpstrShow;
	int *prgnShow;
	char *pstrShow;
	OBJ_DATA *obj;
	int nShow;
	int iShow;
	int count;
	bool fCombine;

	if ( ch->desc == NULL )
		return ;

	/*
	 * Alloc space for output lines.
	 */
	output = new_buf();

	count = 0;

	for ( obj = list; obj != NULL; obj = obj->next_content )
		count++;

	CREATE( prgpstrShow, char *, count );
	CREATE( prgnShow, int, count );
	nShow	= 0;

	/*
	 * Format the list of objects.
	 */
	for ( obj = list; obj != NULL; obj = obj->next_content )
	{
		if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
		{
			pstrShow = format_obj_to_char( obj, ch, fShort );

			fCombine = FALSE;

			if ( IS_NPC( ch ) || IS_SET( ch->comm, COMM_COMBINE ) )
			{
				/*
				 * Look for duplicates, case sensitive.
				 * Matches tend to be near end so run loop backwords.
				 */
				for ( iShow = nShow - 1; iShow >= 0; iShow-- )
				{
					if ( !strcmp( prgpstrShow[ iShow ], pstrShow ) )
					{
						prgnShow[ iShow ] ++;
						fCombine = TRUE;
						break;
					}
				}
			}

			/*
			 * Couldn't combine, or didn't want to.
			 */
			if ( !fCombine )
			{
				prgpstrShow [ nShow ] = str_dup( pstrShow );
				prgnShow [ nShow ] = 1;
				nShow++;
			}
		}
	}

	/*
	 * Output the formatted list.
	 */
	for ( iShow = 0; iShow < nShow; iShow++ )
	{
		if ( prgpstrShow[ iShow ][ 0 ] == '\0' )
		{
			free_string( prgpstrShow[ iShow ] );
			continue;
		}

		if ( IS_NPC( ch ) || IS_SET( ch->comm, COMM_COMBINE ) )
		{
			if ( prgnShow[ iShow ] != 1 )
			{
				sprintf( buf, "(%2d) ", prgnShow[ iShow ] );
				add_buf( output, buf );
			}
			else
			{
				add_buf( output, "     " );
			}
		}
		add_buf( output, prgpstrShow[ iShow ] );
		add_buf( output, "\n\r" );
		free_string( prgpstrShow[ iShow ] );
	}

	if ( fShowNothing && nShow == 0 )
	{
		if ( IS_NPC( ch ) || IS_SET( ch->comm, COMM_COMBINE ) )
			send_to_char( "     ", ch );
		send_to_char( "Ogólnie nic.\n\r", ch );
	}
	page_to_char( buf_string( output ), ch );

	/*
	 * Clean up.
	 */
	free_buf( output );
	DISPOSE( prgpstrShow );
	DISPOSE( prgnShow );

	return ;
}

void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
	char buf[ MAX_STRING_LENGTH ], message[ MAX_STRING_LENGTH ];
	char mxp1[ MAX_STRING_LENGTH ], mxp2[ MAX_STRING_LENGTH ], mxp[ MAX_STRING_LENGTH ];
	AFFECT_DATA *paf, *dazzling;
	OBJ_DATA *obj;
	sh_int x, mirrors = 1;
	int counter;

	buf[ 0 ] = '\0';

	if ( IS_NPC( victim ) && victim->pIndexData && IS_IMMORTAL( ch ) && IS_SET( ch->wiz_config, WIZCFG_SHOWVNUMS ) )
	{
		sprintf( buf, "{g[{G%5d{g]{x ", victim->pIndexData->vnum );
	}

	if ( !IS_NPC( victim ) && IS_SET( victim->comm, COMM_AFK ) )
	{
		strcat( buf, "[AFK] "	);
	}
	if ( !IS_NPC( victim ) && victim->level <= LEVEL_NEWBIE )
	{
		switch (victim->sex)
		{
			case SEX_NEUTRAL:
				strcat( buf, "{G(pocz±tkuj±ce){x " );
				break;
			case SEX_FEMALE:
				strcat( buf, "{G(pocz±tkuj±ca){x " );
				break;
			case SEX_MALE:
			default:
				strcat( buf, "{G(pocz±tkuj±cy){x " );
				break;
		}
	}
	if ( IS_AFFECTED( victim, AFF_INVISIBLE ) )
	{
		switch (victim->sex)
		{
			case SEX_NEUTRAL:
				strcat( buf, "(niewidzialne) " );
				break;
			case SEX_FEMALE:
				strcat( buf, "(niewidzialna) " );
				break;
			case SEX_MALE:
			default:
				strcat( buf, "(niewidzialny) " );
				break;
		}


	}
	else if( is_affected( ch, gsn_dazzling_flash) && affect_find( ch->affected, gsn_dazzling_flash)->level == 0 &&
		is_affected( victim, gsn_dazzling_flash) &&
		!IS_AFFECTED( ch, AFF_PERFECT_SENSES) && ( !IS_AFFECTED(ch,AFF_DETECT_INVIS) || IS_AFFECTED(victim,AFF_NONDETECTION)) )
	{
		for( dazzling = victim->affected ; dazzling; dazzling = dazzling->next )
		{
			if( dazzling->level == 1 && affect_find( ch->affected, gsn_dazzling_flash)->modifier == dazzling->modifier )
			{
				switch (victim->sex)
				{
					case SEX_NEUTRAL:
						strcat( buf, "(niewidzialne) " );
						break;
					case SEX_FEMALE:
						strcat( buf, "(niewidzialna) " );
						break;
					case SEX_MALE:
					default:
						strcat( buf, "(niewidzialny) " );
						break;
				}
			}
		}
	}

	if ( victim->invis_level >= LEVEL_HERO )
	{
		strcat( buf, "(wizinvis) "	);
	}
	if ( IS_AFFECTED( victim, AFF_HIDE ) )
	{
		switch (victim->sex)
		{
			case SEX_NEUTRAL:
				strcat( buf, "(ukryte) " );
				break;
			case SEX_FEMALE:
				strcat( buf, "(ukryta) " );
				break;
			case SEX_MALE:
			default:
				strcat( buf, "(ukryty) " );
				break;
		}
	}
	if ( IS_AFFECTED( victim, AFF_PASS_DOOR ) )
	{
		switch (victim->sex)
		{
			case SEX_NEUTRAL:
				strcat( buf, "(bezcielesne) " );
				break;
			case SEX_FEMALE:
				strcat( buf, "(bezcielesna) " );
				break;
			case SEX_MALE:
			default:
				strcat( buf, "(bezcielesny) " );
				break;
		}
	}
	if ( is_affected( victim, gsn_demon_aura ) )
	{
		strcat( buf, "(demoniczna aura) " );
	}
	if ( IS_AFFECTED( victim, AFF_FAERIE_FIRE ) )
	{
		strcat( buf, "(ró¿owa aura) " );
	}
	if ( IS_EVIL( victim ) && IS_AFFECTED( ch, AFF_DETECT_EVIL ) )
	{
		strcat( buf, "(czerwona aura) " );
	}
	if ( IS_GOOD( victim ) && IS_AFFECTED( ch, AFF_DETECT_GOOD ) )
	{
		strcat( buf, "(z³ota aura) " );
	}
	if ( IS_AFFECTED( victim, AFF_SANCTUARY ) )
	{
		strcat( buf, "(bia³a aura) " );
	}
	if ( IS_AFFECTED( victim, AFF_INCREASE_WOUNDS ) )
	{
		strcat( buf, "(pulsuj±ca czerwona aura) " );
	}
	if ( IS_AFFECTED( victim, AFF_FORCE_FIELD ) )
	{
		strcat( buf, "(wewn±trz kopu³y mocy) " );
	}
	if ( EXT_IS_SET( victim->act, ACT_AGGRESSIVE ) && IS_AFFECTED( ch, AFF_DETECT_AGGRESSIVE ) )
	{
		if( ch->level + 10 > victim->level )
		{
			switch (victim->sex)
			{
				case SEX_NEUTRAL:
					strcat( buf, "(agresywne) " );
					break;
				case SEX_FEMALE:
					strcat( buf, "(agresywna) " );
					break;
				case SEX_MALE:
				default:
					strcat( buf, "(agresywny) " );
					break;
			}
		}
		else if ( dice(1,3) == 1 )
		{
			switch (victim->sex)
			{
				case SEX_NEUTRAL:
					strcat( buf, "(agresywne) " );
					break;
				case SEX_FEMALE:
					strcat( buf, "(agresywna) " );
					break;
				case SEX_MALE:
				default:
					strcat( buf, "(agresywny) " );
					break;
			}
		}
	}
	else if ( IS_AFFECTED( ch, AFF_DETECT_AGGRESSIVE ) && ( ch->level + 10 < victim->level ) )
	{
		if ( dice(1,3) == 1 )
		{
			switch (victim->sex)
			{
				case SEX_NEUTRAL:
					strcat( buf, "(agresywne) " );
					break;
				case SEX_FEMALE:
					strcat( buf, "(agresywna) " );
					break;
				case SEX_MALE:
				default:
					strcat( buf, "(agresywny) " );
					break;
			}
		}
	}

	if ( is_undead(victim) && IS_AFFECTED( ch, AFF_DETECT_UNDEAD ) )
	{
		if( ch->level + 10 > victim->level )
		{
			switch (victim->sex)
			{
				case SEX_NEUTRAL:
					strcat( buf, "(nieumar³e) " );
					break;
				case SEX_FEMALE:
					strcat( buf, "(nieumar³a) " );
					break;
				case SEX_MALE:
				default:
					strcat( buf, "(nieumar³y) " );
					break;
			}
		}
		else if ( dice(1,3) == 1 )
		{
			switch (victim->sex)
			{
				case SEX_NEUTRAL:
					strcat( buf, "(nieumar³e) " );
					break;
				case SEX_FEMALE:
					strcat( buf, "(nieumar³a) " );
					break;
				case SEX_MALE:
				default:
					strcat( buf, "(nieumar³y) " );
					break;
			}
		}
	}
	else if ( IS_AFFECTED( ch, AFF_DETECT_UNDEAD ) && ( ch->level + 10 < victim->level ) )
	{
		if ( dice(1,3) == 1 )
		{
			switch (victim->sex)
			{
				case SEX_NEUTRAL:
					strcat( buf, "(nieumar³e) " );
					break;
				case SEX_FEMALE:
					strcat( buf, "(nieumar³a) " );
					break;
				case SEX_MALE:
				default:
					strcat( buf, "(nieumar³y) " );
					break;
			}
		}
	}

	if( is_affected( victim, gsn_blink ) )
	{
		strcat( buf, "{x(migocze){x " );
	}

	if( ( victim->ss_data ) && ( IS_IMMORTAL( ch ) ))
	{
		strcat( buf, "{b (SHAPESHIFT: ");
		strcat( buf, victim->ss_backup->name );
		strcat( buf, ") {x");
	}

	if ( IS_AFFECTED( ch, AFF_SENSE_LIFE ) && !IS_SET(victim->form, FORM_CONSTRUCT) && !is_undead(victim) )
	{
		int ret = get_hp_notch(victim, TRUE);
		strcat( buf, "(" );
		if(IS_NPC(victim) && IS_SET(victim->form, FORM_CONSTRUCT))
		{
			strcat(buf, condition_info_construct[ret]);
		}
		else
		{
			strcat(buf, condition_info_all[ret]);
		}
		strcat( buf, ") " );
	}

	if ( IS_AFFECTED( ch, AFF_SENSE_FATIGUE ) && !IS_SET(victim->form, FORM_CONSTRUCT) && !is_undead(victim) )
	{
		strcat( buf, "(" );
		strcat(buf, names_alias(victim, 2/*MV*/, 0));
		strcat( buf, ") " );
	}

	if ( ( victim->ss_data && !victim->fighting ) || ( IS_NPC( victim ) && !victim->fighting && victim->position == victim->start_pos && victim->long_descr[ 0 ] != '\0' ) )
	{
		if ( room_is_dark(ch, ch->in_room ) && !IS_AFFECTED( ch, AFF_DARK_VISION ) && IS_AFFECTED( ch, AFF_INFRARED ) )
		{
			strcat( buf, "{R" );
			strcat( buf, strip_colour_codes( victim->long_descr ) );
			strcat( buf, "{x" );
		}
		else
			strcat( buf, victim->long_descr );


		/*
		 * ok zwyklym mobom stojacym w default_pos
		 * nie pojawia siê "lustrzane odbicie"
		 * poniewaz gracz nie wie czy to odbicie
		 * czy po prostu taka grupka
		 * jak kwasno to zmienie
		 */

		if ( IS_AFFECTED( victim, AFF_MIRROR_IMAGE ) )
		{
			strcat( buf, "\n\r" );
			for ( paf = victim->affected; paf != NULL; paf = paf->next )
			{
				if ( paf->type == gsn_mirror_image )
				{
					mirrors = paf->modifier;
					break;
				}
			}

			for ( x = 0; x < mirrors; x++ )
				send_to_char( buf, ch );

			return ;
		}
		else
		{
			if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && IS_SET( ch->wiznet, WIZ_MXP ) && IS_NPC( victim ) && ( victim->pIndexData->pShop ) != NULL )
			{
				sprintf( mxp1, "\e[1z<send '" );
				mxp2[ 0 ] = '\0';
				for ( counter = 1, obj = victim->carrying; obj; obj = obj->next_content )
				{
					if ( obj->wear_loc == WEAR_NONE
							&& can_see_obj( ch, obj )
							&& can_see_obj( victim, obj )
							&& get_cost( victim, obj, TRUE ) > 0 )
					{
						if ( counter == 1 )
							sprintf( mxp, "list|buy %d", counter );
						else
							sprintf( mxp, "|buy %d", counter );
						strcat( mxp1, mxp ) ;
						if ( counter == 1 )
							sprintf( mxp, "' 'kliknij tu aby zobaczyc przedmioty w sklepie|kup %s", obj->short_descr );
						else
							sprintf( mxp, "|kup %s", obj->short_descr );
						strcat( mxp2, mxp );
						counter++;
					}
				}
				strcat( mxp2, "'>" );
				strcat( mxp1, mxp2 );
				send_to_char( mxp1, ch );
				send_to_char( buf, ch );
				send_to_char( "</send>\n\r", ch );
				return ;
			}
			strcat( buf, "\n\r" );
			send_to_char( buf, ch );
		}

		return ;
	}

	if ( IS_AFFECTED( ch, AFF_INFRARED ) && !IS_AFFECTED( ch, AFF_DARK_VISION ) && room_is_dark(ch, ch->in_room ) )
		strcat( buf, "{R" );

	strcat( buf, capitalize( PERS( victim, ch ) ) );

	/*if (!IS_NPC(victim) && !IS_SET(ch->comm, COMM_BRIEF)
	  && victim->position == POS_STANDING && ch->on == NULL)
	  strcat(buf, victim->pcdata->title);*/

	if ( !IS_NPC( victim ) && !victim->ss_data && !IS_SET( ch->comm, COMM_BRIEF ) )
		strcat( buf, victim->pcdata->title );

	if ( victim->mount )
	{
		strcat( buf, " siedzi na grzbiecie " );
		strcat( buf, victim->mount->name2 );
		strcat( buf, ".{x\n\r" );
		send_to_char( buf, ch );
		return ;
	}

	switch ( victim->position )
	{
		case POS_DEAD:
            switch (victim->sex)
            {
                case SEX_NEUTRAL:
                    strcat( buf, " jest MARTWE!" );
                    break;
                case SEX_FEMALE:
                    strcat( buf, " jest MARTWA!" );
                    break;
                case SEX_MALE:
                default:
                    strcat( buf, " jest MARTWY!" );
                    break;
            }
			break;
		case POS_MORTAL:
            switch (victim->sex)
            {
                case SEX_NEUTRAL:
                    strcat( buf, " jest ¶miertelnie ranne." );
                    break;
                case SEX_FEMALE:
                    strcat( buf, " jest ¶miertelnie ranna." );
                    break;
                case SEX_MALE:
                default:
                    strcat( buf, " jest ¶miertelnie ranny." );
                    break;
            }
			break;
		case POS_INCAP:
            switch (victim->sex)
            {
                case SEX_NEUTRAL:
                    strcat( buf, " jest unieruchomione." );
                    break;
                case SEX_FEMALE:
                    strcat( buf, " jest unieruchomiona." );
                    break;
                case SEX_MALE:
                default:
                    strcat( buf, " jest unieruchomiony." );
                    break;
            }
			break;
		case POS_STUNNED:
            switch (victim->sex)
            {
                case SEX_NEUTRAL:
                    strcat( buf, " le¿y tutaj oszo³omione." );
                    break;
                case SEX_FEMALE:
                    strcat( buf, " le¿y tutaj oszo³omiona." );
                    break;
                case SEX_MALE:
                default:
                    strcat( buf, " le¿y tutaj oszo³omiony." );
                    break;
            }
			break;
		case POS_SLEEPING:
			if ( victim->on != NULL )
			{
				if ( IS_SET( victim->on->value[ 2 ], SLEEP_AT ) )
				{
					sprintf( message, " ¶pi przy %s.",
							victim->on->name6 );
					strcat( buf, message );
				}
				else if ( IS_SET( victim->on->value[ 2 ], SLEEP_ON ) )
				{
					sprintf( message, " ¶pi na %s.",
							victim->on->name6 );
					strcat( buf, message );
				}
				else
				{
					sprintf( message, " ¶pi w %s.",
							victim->on->name6 );
					strcat( buf, message );
				}
			}
			else
				strcat( buf, " ¶pi tutaj." );
			break;
		case POS_RESTING:
			if ( victim->on != NULL )
			{
				if ( IS_SET( victim->on->value[ 2 ], REST_AT ) )
				{
					sprintf( message, " odpoczywa przy %s.",
							victim->on->name6 );
					strcat( buf, message );
				}
				else if ( IS_SET( victim->on->value[ 2 ], REST_ON ) )
				{
					sprintf( message, " odpoczywa na %s.",
							victim->on->name6 );
					strcat( buf, message );
				}
				else
				{
					sprintf( message, " odpoczywa w %s.",
							victim->on->name6 );
					strcat( buf, message );
				}
			}
			else
				strcat( buf, " odpoczywa tutaj." );
			break;
		case POS_SITTING:
			if ( victim->on != NULL )
			{
				if ( IS_SET( victim->on->value[ 2 ], SIT_AT ) )
				{
					sprintf( message, " siedzi przy %s",
							victim->on->name6 );
					strcat( buf, message );
				}
				else if ( IS_SET( victim->on->value[ 2 ], SIT_ON ) )
				{
					sprintf( message, " siedzi na %s",
							victim->on->name6 );
					strcat( buf, message );
				}
				else if ( victim->on->item_type == ITEM_MUSICAL_INSTRUMENT )
				{
					sprintf( message, " siedzi przy %s",
							victim->on->name6 );
					strcat( buf, message );
				}
				else
				{
					sprintf( message, " siedzi w %s",
							victim->on->name6 );
					strcat( buf, message );
				}
			}
			else
				strcat( buf, " siedzi tutaj" );
			break;
		case POS_STANDING:
			if ( victim->on != NULL )
			{
				if ( IS_SET( victim->on->value[ 2 ], STAND_AT ) )
				{
					sprintf( message, " stoi przy %s",
							victim->on->name6 );
					strcat( buf, message );
				}
				else if ( IS_SET( victim->on->value[ 2 ], STAND_ON ) )
				{
					sprintf( message, " stoi na %s",
							victim->on->name6 );
					strcat( buf, message );
				}
				else
				{
					sprintf( message, " stoi w %s",
							victim->on->name6 );
					strcat( buf, message );
				}
			}
			else
            {
	            if ( IS_AFFECTED( victim, AFF_FLYING ) || IS_AFFECTED( victim, AFF_FLOAT ) )
                {
				    strcat( buf, " unosi siê tutaj" );
                }
                else
                {
                    strcat( buf, " stoi tutaj" );
                }
            }
			break;
		case POS_FIGHTING:
			strcat( buf, " jest tutaj" );
			break;
			/*
			   if (victim->fighting == NULL)
			   strcat(buf, "z powietrzem?");
			   else if (victim->fighting == ch)
			   strcat(buf, "z Toba!");
			   else if (victim->in_room == victim->fighting->in_room)
			   {
			   strcat(buf, "z ");
			   strcat(buf, victim->fighting->name5);
			   strcat(buf, ".");
			   }
			   else
			   strcat(buf, "z kims kto sobie poszedl?");
			   break;*/
	}

	if ( victim->position >= POS_SITTING )
	{
		if ( victim->fighting )
		{
			strcat( buf, ", walczy " );

			if ( victim->fighting == ch )
				strcat( buf, "z toba!" );
			else if ( victim->in_room == victim->fighting->in_room )
			{
				strcat( buf, "z " );
				if ( can_see(ch, victim->fighting) )
					strcat( buf, victim->fighting->name5 );
				else
					strcat( buf, "kim¶" );
				strcat( buf, "." );
			}
			else
				strcat( buf, "z kim¶ kto sobie poszed³?" );
		}
		else if ( victim->wait_char )
			strcat( buf, ", rzuca zaklêcie." );
		else
			strcat( buf, "." );

	}

	sprintf( buf, "%s", capitalize( buf ) );


	if ( IS_AFFECTED( victim, AFF_MIRROR_IMAGE ) )
	{
		for ( paf = victim->affected; paf != NULL; paf = paf->next )
		{
			if ( paf->type == gsn_mirror_image )
			{
				mirrors = paf->modifier;
				break;
			}
		}

		send_to_char( buf, ch );
		send_to_char( "\n\r", ch );
		strcat( buf, " (lustrzane odbicie){x\n\r" );

		for ( x = 0;x < mirrors;x++ )
			send_to_char( buf, ch );

		return ;
	}
	else
	{
		strcat( buf, "{x\n\r" );
		send_to_char( buf, ch );
	}

	return ;
}

void show_char_extras( CHAR_DATA *victim, CHAR_DATA *ch )
{
	EXTRA_DESCR_DATA * ed;
	OBJ_DATA *obj, *obj2, *cover;
	BONUS_DATA * bonus;
	bool shown = FALSE;
	bool repeat = FALSE;
	extern int wear_to_itemwear[ MAX_WEAR ];
	time_t time;
	char fr_info[MAX_STRING_LENGTH];
	bool introduced;

	if ( !IS_NPC( victim ) && victim->level <= LEVEL_NEWBIE )
    {
		act( "{RWidaæ, ¿e $n dopiero zaczyna swoj± przygodê w tym ¶wiecie.\n\r{x", victim, NULL, ch, TO_VICT );
    }

    if ( !IS_NPC( victim ) && victim->level < 11 )
    {
	    char buf[ MAX_STRING_LENGTH ];
        sprintf( buf, "{G%s{x\n\r", level_info_common[ victim->level - 1 ] );
		send_to_char( buf, ch );
    }

	cover = get_eq_char( victim, WEAR_ABOUT );
	if ( cover && !IS_OBJ_STAT( cover, ITEM_COVER ) ) cover = NULL;

	for ( obj = victim->carrying; obj != NULL; obj = obj->next_content )
	{
		repeat = FALSE;
		if ( !can_see_obj( ch, obj ) || obj->wear_loc == WEAR_NONE )
			continue;

		for ( obj2 = victim->carrying; obj2 != obj && obj2 != NULL; obj2 = obj2->next_content )
		{
			if ( obj2->pIndexData->vnum == obj->pIndexData->vnum )
			{
				repeat = TRUE;
				break;
			}
		}

		if ( repeat ) continue;

		if ( !cover || !IS_SET( cover->wear_flags, wear_to_itemwear[ obj->wear_loc ] ) )
		{
			for ( ed = obj->extra_descr; ed; ed = ed->next )
			{
				if ( ed->keyword[ 0 ] == '_' )
				{
					if ( !shown && victim->description[ 0 ] != '\0' )
					{
						send_to_char( victim->description, ch );
						send_to_char( "\n\r", ch );
					}

					if ( ch == victim )
						act( ed->description, ch, NULL, NULL, TO_CHAR );
					else
						act( ed->description, victim, NULL, ch, TO_VICT );
					shown = TRUE;
				}
			}

			for ( ed = obj->pIndexData->extra_descr; ed; ed = ed->next )
			{
				if ( ed->keyword[ 0 ] == '_' )
				{
					if ( !shown && victim->description[ 0 ] != '\0' )
					{
						send_to_char( victim->description, ch );
						send_to_char( "\n\r", ch );
					}

					if ( ch == victim )
						act( ed->description, ch, NULL, NULL, TO_CHAR );
					else
						act( ed->description, victim, NULL, ch, TO_VICT );
					shown = TRUE;
				}
			}
		}
	}

	for ( bonus = victim->bonus; bonus != NULL; bonus = bonus->next )
	{
		for ( ed = bonus->index_data->extra_descr; ed; ed = ed->next )
		{
			if ( ed->keyword[ 0 ] == '_' )
			{
				if ( !shown && victim->description[ 0 ] != '\0' )
				{
					send_to_char( victim->description, ch );
					send_to_char( "\n\r", ch );
				}

				if ( ch == victim )
					act( ed->description, ch, NULL, NULL, TO_CHAR );
				else
					act( ed->description, victim, NULL, ch, TO_VICT );
				shown = TRUE;
			}
		}
	}

	if ( !shown )
	{
		if ( victim->description[ 0 ] != '\0' )
			send_to_char( victim->description, ch );
		else
			act( "Nie widzisz nic dziwnego w $B.", ch, NULL, victim, TO_CHAR );
	}

	if ( !IS_NPC( victim ) && victim->condition[ COND_DRUNK ] > DRUNK_FULL )
	{
		if ( victim->class == CLASS_BARBARIAN || victim->class == CLASS_SHAMAN || victim->class == CLASS_WARRIOR|| victim->class == CLASS_BLACK_KNIGHT )
		{
			if ( victim->sex == SEX_FEMALE )
			{
				act( "Widzisz, ¿e $N jest jak zwykle pijana, jednak chyba ju¿ siê przyzwyczai³a do tego, poniewa¿ chodzi w miarê prosto.", ch, NULL, victim, TO_CHAR );
			}
			else
			{
				act( "Widzisz, ¿e $N jest jak zwykle pijany, jednak chyba ju¿ siê przyzwyczai³ do tego, poniewa¿ chodzi w miarê prosto.", ch, NULL, victim, TO_CHAR );
			}
		}
		else
		{
			if ( victim->sex == SEX_FEMALE )
			{
				act( "$N musia³a za du¿o wypiæ, bo ledwo stoi na nogach i czêsto myl± jej siê drogi.", ch, NULL, victim, TO_CHAR );
			}
			else
			{
				act( "$N musia³ za du¿o wypiæ, bo ledwo stoi na nogach i czêsto myl± mu siê drogi.", ch, NULL, victim, TO_CHAR );
			}
		}
	}

	if( is_affected(victim, gsn_on_smoke ))
	{
		switch( victim->class )
		{
			case CLASS_MAG:
				if(IS_AFFECTED(victim,AFF_HALLUCINATIONS_POSITIVE)||IS_AFFECTED(victim,AFF_HALLUCINATIONS_NEGATIVE))
					act( "Widzisz, ¿e $N ma oczy wielko¶ci ig³y kapelusznika Azazelo. U¶miecha siê on do wszystkich z byle powodu. Kto¶ tu chyba musia³ spaliæ du¿± ilo¶æ ziela...", ch, NULL, victim, TO_CHAR );
				else
					act( "Widzisz, ze oczy $Z s± bardzo ma³e, a na $S twarzy ci±gle widnieje mi³y u¶miech. Kto¶ tu chyba musia³ siê lekko spaliæ...", ch, NULL, victim, TO_CHAR );
				break;
			case CLASS_WARRIOR:
			case CLASS_BARBARIAN:
			case CLASS_BLACK_KNIGHT:
			case CLASS_SHAMAN:
				if(IS_AFFECTED(victim,AFF_HALLUCINATIONS_POSITIVE)||IS_AFFECTED(victim,AFF_HALLUCINATIONS_NEGATIVE))
					act( "Z wielkiego, lecz nie ten samego co zawsze, u¶miechu na twarzy $Z, mo¿esz odczytaæ, ¿e dzisiaj zosta³a spalona du¿a ilo¶æ ziela fajkowego.", ch, NULL, victim, TO_CHAR );
				else
					act( "Inny ni¿ zwykle u¶miech na twarzy $Z wskazuje na to, ¿e spali³ trochê ziela fajkowego.", ch, NULL, victim, TO_CHAR );
				break;
			case CLASS_CLERIC:
			case CLASS_PALADIN:
			case CLASS_MONK:
				if(IS_AFFECTED(victim,AFF_HALLUCINATIONS_POSITIVE)||IS_AFFECTED(victim,AFF_HALLUCINATIONS_NEGATIVE))
					act( "$N ¶mieje siê przez ca³y czas. Jest pod du¿ym wp³ywem narkotyku, jakim jest ziele. Ciekawe, co my¶li o nim bóg, któremu s³u¿y?", ch, NULL, victim, TO_CHAR );
				else
					act( "Przez $Z spalone zosta³o trochê ziela fajkowego, poniewa¿ $S oczy s± bardzo ma³e, a na twarzy widnieje du¿y u¶miech. Chyba jednak nie czuje siê z tym najlepiej.", ch, NULL, victim, TO_CHAR );
				break;
			case CLASS_DRUID:
				if(IS_AFFECTED(victim,AFF_HALLUCINATIONS_POSITIVE)||IS_AFFECTED(victim,AFF_HALLUCINATIONS_NEGATIVE))
					if( victim->sex == 2 )
						act( "$N jest dzisiaj niezwykle roze¶miana, a jej oczy s± bardzo malutkie. Spali³a siê zapewne du¿± porcj± ziela... ciekawe, czy by³o ono w³asnej produkcji?", ch, NULL, victim, TO_CHAR );
					else
						act( "$N jest dzisiaj niezwykle roze¶miany, a jego oczy s± bardzo malutkie. Spali³ siê zapewne du¿± porcj± ziela... ciekawe, czy by³o ono w³asnej produkcji?", ch, NULL, victim, TO_CHAR );
				else
					act( "Widzisz, ze oczy $Z s± bardzo ma³e, a na $S twarzy ci±gle widnieje mi³y u¶miech. Kto¶ tu chyba musia³ siê lekko spaliæ...", ch, NULL, victim, TO_CHAR );
				break;
			case CLASS_BARD:
			case CLASS_THIEF:
				if(IS_AFFECTED(victim,AFF_HALLUCINATIONS_POSITIVE)||IS_AFFECTED(victim,AFF_HALLUCINATIONS_NEGATIVE))
					if( victim->sex == 2 )
						act( "$N zachowuje siê teraz ca³kiem inaczej ni¿ zwykle. Jest ca³a roze¶miana. Pewnie spali³a przed chwil± du¿± ilo¶æ ziela fajkowego.", ch, NULL, victim, TO_CHAR );
					else
						act( "$N zachowuje siê teraz ca³kiem inaczej ni¿ zwykle. Jest ca³y roze¶miany. Pewnie spali³ przed chwil± du¿± ilo¶æ ziela fajkowego.", ch, NULL, victim, TO_CHAR );
				else
					if( victim->sex == 2 )
						act( "Mniejsze oczy $Z wskazuj± na to, ¿e spali³a ona trochê ziela. To by t³umaczy³o jej lekko dziwne zachowanie.", ch, NULL, victim, TO_CHAR );
					else
						act( "Mniejsze oczy $Z wskazuj± na to, ¿e spali³ on trochê ziela. To by t³umaczy³o jego lekko dziwne zachowanie.", ch, NULL, victim, TO_CHAR );
				break;
		}
	}

	if ( friend_exist( ch, victim->name, NULL, fr_info, &time, &introduced ) )
	{
		print_char( ch, "{w%s{x\n\r", fr_info );
	}

	if( IS_AFFECTED(victim, AFF_STONE_SKIN ))
	{
		act( "Skóra $Z wygl±da, jakby by³a z kamienia.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_HASTE ))
	{
		act( "$N porusza siê niewiarygodnie szybko.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_ETHEREAL_ARMOR ))
	{
		act( "$C otacza eteryczny pancerz.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_SHIELD ))
	{
		act( "$C otacza magiczna tarcza.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_ARMOR ))
	{
		act( "$C otacza magiczny pancerz.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, 93 ))//blur
	{
		act( "Próbujesz skoncentrowaæ wzrok na $B, lecz $S sylwetka rozmywa ci siê w oczach.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_IMMOLATE))
	{
		act( "Wokó³ $Z roztacza siê niewielka, jarz±ca siê po¶wiata.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_BARK_SKIN))
	{
		act( "Skóra $Z wygl±da, jakby by³a z drewna.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, gsn_steel_scarfskin))
	{
		act( "Naskórek $Z wygl±da, jakby by³ ze stali.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_FIRESHIELD))
	{
		act( "$C otacza ledwo widoczna aura z p³omieni.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_ICESHIELD))
	{
		act( "$C otacza ledwo widoczna aura z lodu.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_BLADE_BARRIER))
	{
		act( "Dooko³a $Z wiruje gro¼nie kilka ostrych kawa³ków ska³y.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_SHIELD_OF_NATURE))
	{
		act( "Dooko³a $Z wiruj± niewielkie, b³yszcz±ce punkty ¶wiat³a.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, 246 ))//storm shell
	{
		act( "Wokó³ $Z widaæ pulsuj±c± skorupê energii.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_REFLECT_SPELL))
	{
		act( "Pulsuj±ca, magiczna tarcza otacza ca³kowicie $C.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_PARALYZE))
	{
		act( "$N stoi tutaj w bezruchu, wpatruj±c siê w jeden punkt krajobrazu.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_MINOR_GLOBE))
	{
		act( "$C otacza pó³przezroczysta, magiczna sfera.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_GLOBE))
	{
		act( "$C otacza pulsuj±ca, pó³przezroczysta magiczna sfera.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_MAJOR_GLOBE))
	{
		act( "$C otacza potê¿na, pulsuj±ca pó³przezroczysta magiczna sfera.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, 230 ))//divine favor
	{
		act( "Rêce $Z otoczone s± aur± ¶wiêtej energii.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, 231 ))//divine power
	{
		act( "$C otacza ¶wietlista boska aura.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, 276 ))//divine shield
	{
		act( "$C otacza boska tarcza.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, 144 ))//energy shield
	{
		act( "$C otacza tarcza z pozytywnej energii.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_POISON))
	{
		act( "$N wygl±da bardzo ¼le.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_PLAGUE))
	{
		act( "$N wygl±da bardzo ¼le, ca³e $S cia³o pokryte jest wrzodami.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, 168 ))//web
	{
		act( "$C oplata magiczna pajêczyna.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, 156 ))//entangle
	{
		act( "Ruchy $Z krêpuj± wyrastaj±ce z ziemi korzenie ro¶lin.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_INCREASE_WOUNDS))
	{
		act( "$C otacza pulsuj±ca, czerwona aura.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, gsn_antimagic_manacles ))
	{
		act( "D³onie $Z oplataj± pó³materialne kajdany.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_DEFLECT_WOUNDS))
	{
		act( "$C otacza potê¿na, zó³tawa kopu³a.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_EYES_OF_THE_TORTURER))
	{
		act( "Oczy $Z pob³yskuj± na czerwono.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_LOYALTY) && victim->master == ch )
	{
		act( "Lojalno¶æ $Z wzglêdem ciebie nie ma granic!", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, gsn_blink ))
	{
		act( "Sylwetka $Z lekko migocze.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, 312 ))//unholy fury
	{
		act( "$C ogarnia przeklêta furia.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, gsn_magic_hands ))
	{
		act( "D³onie $Z otacza magiczna aura.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, gsn_draining_hands ))
	{
		act( "D³onie $Z otacza wysysaj±ca ¿ycie aura.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, gsn_demon_aura ))
	{
		act( "$C otacza czerwona demoniczna aura negatywnej energii.", ch, NULL, victim, TO_CHAR );
	}
	if( EXT_IS_SET(victim->act, ACT_RAISED) )
	{
		act( "$N wygl±da na ¶wie¿o o¿ywionego nieumar³ego.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, gsn_magic_fang ))
	{
		act( "Z pysku $Z toczy siê piana.", ch, NULL, victim, TO_CHAR );
	}
	if( is_affected(victim, gsn_animal_rage ))
	{
		act( "$N zachowuje siê bardzo agresywnie.", ch, NULL, victim, TO_CHAR );
	}
	if ( is_affected ( victim, gsn_bleeding_wound ) )
	{
		act( "$N krwawi z niezasklepionych ran.", ch, NULL, victim, TO_CHAR );
	}
	if ( is_affected ( victim, gsn_bleed ) )
	{
		act( "$N jest okaleczon$R.", ch, NULL, victim, TO_CHAR );
	}
	if ( is_affected ( victim, gsn_bleed ) )
	{
		act( "$N ma g³êbok± ranê w udzie.", ch, NULL, victim, TO_CHAR );
	}
	if ( is_affected ( victim, gsn_bandage ) )
	{
		act( "$N ma obanda¿owane rany.", ch, NULL, victim, TO_CHAR );
	}
	if ( is_affected ( victim, gsn_defense_curl ) )
	{
		act( "$C otacza ochronna spirala.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_FLOAT) )
	{
		act( "$N lewituje tu¿ nad ziemi±.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_FLYING) )
	{
		act( "$N unosi siê wysoko nad ziemi±.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_MEDITATION) )
	{
		act( "$N jest pogr±¿on$R w medytacji.", ch, NULL, victim, TO_CHAR );
	}
	if( IS_AFFECTED(victim, AFF_SUBDUE_SPIRITS) || IS_AFFECTED(victim, AFF_SPIRITUAL_GUIDANCE) )
	{
		act( "Dooko³a $Z migaj± pó³prze¼roczyste sylwetki.", ch, NULL, victim, TO_CHAR );
	}

}


void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
	char buf[ MAX_STRING_LENGTH ],buf2[ MAX_STRING_LENGTH ];
	OBJ_DATA *obj, *cover, *gloves;
	int iWear,i,percent;
	bool found;
	extern int wear_to_itemwear[ MAX_WEAR ];

	if ( can_see( victim, ch ) && ( IS_NPC( ch ) || !ch->pcdata->mind_in ) )
	{
		if ( ch == victim )
			act( "$n patrzy na siebie.", ch, NULL, NULL, TO_ROOM );
		else
		{
			if( IS_AFFECTED( victim, AFF_HALLUCINATIONS_POSITIVE ))
			{
				switch ( number_range( 1, 6 ) )
				{
					case 1:
						act( "$n patrzy na ciebie z pogard±, chyba ciê nienawidzi...", ch, NULL, victim, TO_VICT );
						break;
					case 2:
						act( "$n patrzy na ciebie jak na kawa³ek miêsa.", ch, NULL, victim, TO_VICT );
						break;
					case 3:
						act( "We wzroku $z dostrzegasz bezgraniczn± pogardê.", ch, NULL, victim, TO_VICT );
						break;
					case 4:
						act( "Zimne, taksuj±ce spojrzenie $z, przyprawia ciê o dreszcze.", ch, NULL, victim, TO_VICT );
						break;
					case 5:
						act( "$n patrzy przez ciebie, tak ¿e czujesz siê niczym.", ch, NULL, victim, TO_VICT );
						break;
					default :
						act( "$n patrzy na ciebie, oceniaj±c jak ciê zabiæ.", ch, NULL, victim, TO_VICT );
						break;
				}
			}
			else if ( IS_AFFECTED( victim, AFF_HALLUCINATIONS_NEGATIVE ))
			{
				switch ( number_range( 1, 6 ) )
				{
					case 1:
						act( "$n robi zeza! Ale ¶mieszne!", ch, NULL, victim, TO_VICT );
						break;
					case 2:
						act( "$n wykrzywia g³owê i wydaje dzwiêk spuszczanej ciêciwy.", ch, NULL, victim, TO_VICT );
						break;
					case 3:
						act( "$n k³adzie siê na ziemi i patrzy na twoje nogi.", ch, NULL, victim, TO_VICT );
						break;
					case 4:
						act( "$n siada przed tob± po czym szybko wstaje, patrz±c ci prosto w oczy.", ch, NULL, victim, TO_VICT );
						break;
					case 5:
						act( "Usmiech $z sp³ywa na ziemiê, potem do³±czaj± siê do niego oczy.", ch, NULL, victim, TO_VICT );
						break;
					default :
						act( "Twarz $z rozp³ywa siê w rózow± mgie³kê.", ch, NULL, victim, TO_VICT );
						break;
				}
			}
			else
			{
				act( "$n patrzy na ciebie.", ch, NULL, victim, TO_VICT );
			}
			act( "$n patrzy na $C.", ch, NULL, victim, TO_NOTVICT );
		}
	}

	show_char_extras( victim, ch );

	percent = GET_PERCENT_HP(victim);

	strcpy( buf, PERS( victim, ch ) );

	if ( !IS_NPC( victim ) )
	{
		sprintf( buf2, ", %s,", race_table[ GET_RACE( victim ) ].name );
		strcat( buf, buf2 );
	}

	if(IS_NPC(victim) && IS_SET(victim->form, FORM_CONSTRUCT))
	{
		if (percent >= 100)
        {
            strcat( buf, " wygl±da na {Gnienaruszon" );
			switch (victim->sex)
			{
                case SEX_NEUTRAL:
                   strcat( buf, "e" );
                   break;
                case SEX_MALE:
                   strcat( buf, "ego" );
                   break;
                case SEX_FEMALE:
                   strcat( buf, "±" );
                   break;
            }
        }
		else if ((percent >= 85) || is_affected( victim, gsn_perfect_self)) strcat( buf, " ma {Gkilka zadrapañ" );
		else if (percent >= 70)	strcat( buf, " ma {glekkie uszkodzenia" );
		else if (percent >= 55)	strcat( buf, " ma {f¶rednie uszkodzenia" );
		else if (percent >= 40)	strcat( buf, " ma {fciê¿kie uszkodzenia" );
		else if (percent >= 25)	strcat( buf, " ma {rogromne uszkodzenia" );
        else if (percent >= 0)	strcat( buf, " {rledwo stoi" );
        else
        {
            strcat( buf, " {rjest unieruchomion" );
            switch (victim->sex)
            {
                case SEX_NEUTRAL:
                    strcat( buf, "e");
                    break;
                case SEX_MALE:
                    strcat( buf, "y");
                    break;
                case SEX_FEMALE:
                    strcat( buf, "a");
                    break;
            }
        }
    }
    else
    {
        if (percent >= 100) strcat( buf, " jest w {Gdoskona³ej kondycji" );
        else if ((percent >= 85) || is_affected( victim, gsn_perfect_self)) strcat( buf, " ma {Gkilka zadrapañ" );
        else if (percent >= 70)	strcat( buf, " ma {glekkie rany" );
        else if (percent >= 55)	strcat( buf, " ma {f¶rednie rany" );
        else if (percent >= 40)	strcat( buf, " ma {fciê¿kie rany" );
        else if (percent >= 25)	strcat( buf, " ma {rogromne rany" );
        else if (percent >= 0)	strcat( buf, " {rledwo stoi" );
        else			strcat( buf, " {rumiera" );
    }

    strcat( buf, "{x.\n\r" );

    sprintf( buf, "%s", capitalize( buf ) );
    send_to_char( buf, ch );

    /*
     * get cover from char
     */
    cover = get_eq_char( victim, WEAR_ABOUT );
    if ( cover && !IS_OBJ_STAT( cover, ITEM_COVER ) ) cover = NULL;

    /*
     * get gloves from char
     */
    gloves = get_eq_char( victim, WEAR_HANDS );
    if ( gloves && gloves->item_type != ITEM_ARMOR ) gloves = NULL;

    found = FALSE;
    for ( i = 0; wear_order[i] >= 0; i++ )
    {
        iWear = wear_order[i];
        if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
                && can_see_obj( ch, obj ) )
        {
            if (
                    cover
                    && !IS_IMMORTAL( ch )
                    && !IS_AFFECTED( ch, AFF_PIERCING_SIGHT )
                    && IS_SET( cover->wear_flags, wear_to_itemwear[ iWear ] )
               )
            {
                continue;
            }
            /*
             * obs³uga pier¶cieni jest zale¿na od tego, czy posiadamy rêkawice
             * oraz czy te rêkawice zas³aniaj± pier¶cienie
             */
            if (
                    ( iWear == WEAR_FINGER_L || iWear == WEAR_FINGER_R )
                    && gloves
                    && !IS_IMMORTAL( ch )
                    && !IS_AFFECTED( ch, AFF_PIERCING_SIGHT )
               )
            {
                if ( gloves->value[5] == GLOVES_UNDER ) //czy rekawice zakrywaj± pier¶cienie
                {
                    continue;
                }
            }

            if ( !found )
            {
                send_to_char( "\n\r", ch );
                act( "$N u¿ywa:", ch, NULL, victim, TO_CHAR );
                found = TRUE;
            }

            if ( iWear == WEAR_SHIELD )
            {
                send_to_char( "\n\r", ch );
            }

            if ( iWear == WEAR_WIELD && ( IS_WEAPON_STAT( obj, WEAPON_TWO_HANDS ) ))
            {
                send_to_char( "<trzymane dwurêcznie>          ", ch );
            }
            else
            {
                send_to_char( where_name[ iWear ], ch );
            }

            send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
            /*
             * obs³uga rzeczy noszonych pod p³aszczem
             */
            if (
                    cover
                    && ( IS_IMMORTAL( ch ) || IS_AFFECTED( ch, AFF_PIERCING_SIGHT ) )
                    && IS_SET( cover->wear_flags, wear_to_itemwear[ iWear ] )
               )
            {
                printf_to_char( ch, " {D(pod %s){x", cover->name5 );
            }
            /*
             * obs³uga pier¶cieni jest zale¿na od tego, czy posiadamy rêkawice
             * oraz czy te rêkawice zas³aniaj± pier¶cienie
             */
            if ( ( iWear == WEAR_FINGER_L || iWear == WEAR_FINGER_R ) && gloves )
            {
                if ( gloves->value[5] == GLOVES_UNDER )
                {
                    printf_to_char( ch, " {D(pod %s){x", gloves->name5 );
                }
                else
                {
                    printf_to_char( ch, " {D(na %s){x", gloves->name6 );
                }
            }
            send_to_char( "\n\r", ch );
        }
    }

    if(EXT_IS_SET(victim->act, ACT_MOUNTABLE))
    {
    	printf_to_char( ch, "%s jest %s, %s i %s\n\r", capitalize_first(victim->short_descr), names_alias(victim, 2/*MV*/, 0), names_alias(victim, 5/*HU*/, 0), names_alias(victim, 6/*TH*/, 0) );
    	if(IS_IMMORTAL( ch ))
    	{
    		printf_to_char( ch, "%s ma %i/%i mv\n\r", victim->short_descr, victim->move, victim->max_move );
    		printf_to_char( ch, "%s najedzony %i/%i\n\r", victim->short_descr, victim->condition[ COND_HUNGER ], EAT_FULL );
    		printf_to_char( ch, "%s napojony: %i/%i\n\r", victim->short_descr, victim->condition[ COND_THIRST ], DRINK_FULL );
    	}
    }

    if ( victim != ch
            && ( IS_IMMORTAL( ch ) || (IS_AFFECTED( ch, AFF_PIERCING_SIGHT ) && ch->level > number_range(27, 30) )))
    {
        send_to_char( "\n\rPatrzysz do inwentarza:\n\r", ch );
        show_list_to_char( victim->carrying, ch, TRUE, TRUE );
    }
    return ;
}

void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
	CHAR_DATA * rch;

	for ( rch = list; rch != NULL; rch = rch->next_in_room )
	{
		if ( rch == ch && ( IS_NPC( ch ) || !ch->pcdata->mind_in ) )
			continue;

		if ( get_trust( ch ) < rch->invis_level )
			continue;

		if ( can_see( ch, rch ) )
		{
			show_char_to_char_0( rch, ch );
		}
	}

	return ;
}

bool check_blind( CHAR_DATA *ch )
{

	if ( !IS_NPC( ch ) && EXT_IS_SET( ch->act, PLR_HOLYLIGHT ) )
		return TRUE;

	if ( IS_AFFECTED( ch, AFF_BLIND ) && !IS_AFFECTED( ch,AFF_PERFECT_SENSES) )
	{
		send_to_char( "Nie widzisz nawet czubka nosa!\n\r", ch );
		return FALSE;
	}

	return TRUE;
}


/* RT does socials */
void do_socials( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	SOCIAL_DATA * social;
	int col;

	if( IS_NPC(ch)) return;//mobom dziêkujemy

	col = 0;

	for ( social = social_list; social; social = social->next )
	{
		sprintf( buf, "%-12s", social->name );
		send_to_char( buf, ch );
		if ( ++col % 6 == 0 )
			send_to_char( "\n\r", ch );
	}

	if ( col % 6 != 0 )
		send_to_char( "\n\r", ch );
	return ;
}



/* RT Commands to replace news, motd, imotd, etc from ROM */

void do_motd( CHAR_DATA *ch, char *argument )
{
	do_function( ch, &do_help, "motd" );
}

void do_imotd( CHAR_DATA *ch, char *argument )
{
	do_function( ch, &do_help, "imotd" );
}

void do_rules( CHAR_DATA *ch, char *argument )
{
	do_function( ch, &do_help, "rules" );
}

void do_story( CHAR_DATA *ch, char *argument )
{
	do_function( ch, &do_help, "story" );
}


/* RT this following section holds all the auto commands from ROM, as well as
   replacements for config */

void show_config( CHAR_DATA *ch )
{
	char buffer[ MAX_INPUT_LENGTH ];
	BUFFER *buf = NULL;

	if ( IS_NPC( ch ) )
		return ;

	buf = new_buf();

	add_buf( buf, "\n\r{WAktualna konfiguracja:{x\n\n\r" );

	sprintf( buffer, "autoexit       [%s]\n\r",
			EXT_IS_SET( ch->act, PLR_AUTOEXIT ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "automem        [%s]\n\r",
			EXT_IS_SET( ch->act, PLR_AUTOMEM ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "autogold       [%s]\n\r",
			EXT_IS_SET( ch->act, PLR_AUTOGOLD ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "autosplit      [%s]\n\r",
			EXT_IS_SET( ch->act, PLR_AUTOSPLIT ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "polish         [%s]\n\r",
			EXT_IS_SET( ch->act, PLR_COMMANDSPL ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "acceptsurrender[%s]\n\r",
			EXT_IS_SET( ch->act, PLR_ACCEPTSURR ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );


	sprintf( buffer, "compact mode   [%s]\n\r",
			IS_SET( ch->comm, COMM_COMPACT ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "hints          [%s]\n\r",
			IS_SET( ch->comm, COMM_HINT ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "combine items  [%s]\n\r",
			IS_SET( ch->comm, COMM_COMBINE ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "deaf           [%s]\n\r",
			IS_SET( ch->comm, COMM_DEAF ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "quiet          [%s]\n\r",
			IS_SET( ch->comm, COMM_QUIET ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );

	sprintf( buffer, "group prompt   [%s]\n\r",
			EXT_IS_SET( ch->act, PLR_COMBAT_PROMPT ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );
//Raszer: config showposition
	sprintf( buffer, "show position  [%s]\n\r",
			EXT_IS_SET( ch->act, PLR_SHOWPOS ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );
#ifdef ENABLE_SHOW_LFG
	sprintf( buffer, "lfg            [%s]\n\r",
			IS_SET( ch->comm, COMM_LFG ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );
#endif
#ifdef ENABLE_SHOW_ON_WHO
	sprintf( buffer, "showwho        [%s]\n\r",
			IS_SET( ch->comm, COMM_SHOW_ON_WHO ) ? "{gV{x" : "{r-{x" );
	add_buf( buf, buffer );
#endif
	sprintf( buffer, "prompt         %s\n\r",
			IS_SET( ch->comm, COMM_PROMPT ) ? ch->prompt : "{r-{x" );
	add_buf( buf, buffer );

	add_buf( buf, "\n\r" );


	if ( EXT_IS_SET( ch->act, PLR_NOFOLLOW ) )
		add_buf( buf, "Nie chcesz aby kto¶ za tob± chodzi³.\n\r" );
	else
		add_buf( buf, "Pozwalasz chodziæ za sob±.\n\r" );

	if ( EXT_IS_SET( ch->act, PLR_REVBOARD ) )
		add_buf( buf, "Widzisz wiadomo¶ci na tablicach w odwrotnej kolejno¶ci.\n\r" );
	else
		add_buf( buf, "Widzisz wiadomo¶ci na tablicach w normalnej kolejno¶ci.\n\r" );

	sprintf( buffer, "WIMPY ustawione na %d hp\n\r", ch->wimpy );

	if ( ch->lines != PAGELEN )
	{
		if ( ch->lines )
		{
			sprintf( buffer, "Buforowanie przewijania ustawione na %d linii.\n\r", ch->lines + 2 );
			add_buf( buf, buffer );
		}
		else
			add_buf( buf, "Buforowanie przewijania wy³±czone.\n\r" );
	}

	if ( IS_SET( ch->comm, COMM_NOSHOUT ) )
		add_buf( buf, "Nie mo¿esz u¿ywac komendy shout.\n\r" );

	if ( IS_SET( ch->comm, COMM_NOTELL ) )
		add_buf( buf, "Nie mo¿esz u¿ywac komendy tell.\n\r" );

	if ( IS_SET( ch->comm, COMM_NOEMOTE ) )
		add_buf( buf, "Nie mo¿esz u¿ywac komendy emote.\n\r" );
  if ( IS_SET( ch->comm, COMM_NONEWBIE ) )
  {
    add_buf( buf, "Nie widzisz pytañ kana³u newbie.\n\r" );
  } else {
    add_buf( buf, "Widzisz pytania kana³u newbie.\n\r" );
  }

	if ( EXT_IS_SET( ch->act, PLR_POL_ISO ) )
		add_buf( buf, "Standard kodowania polskich liter: ISO-8859-2.\n\r" );
	else if ( EXT_IS_SET( ch->act, PLR_POL_WIN ) )
		add_buf( buf, "Standard kodowania polskich liter: CP-1250 (Windows).\n\r" );
	else
		add_buf( buf, "Standard kodowania polskich liter: polskawy.\n\r" );

	if ( EXT_IS_SET( ch->act, PLR_STYL1 ) )
		add_buf( buf, "Standard kolorowania czarow: ¿ywio³y.\n\r" );
	else if ( EXT_IS_SET( ch->act, PLR_STYL2 ) )
		add_buf( buf, "Standard kolorowania czarow: charakter.\n\r" );
	else if ( EXT_IS_SET( ch->act, PLR_STYL3 ) )
		add_buf( buf, "Standard kolorowania czarow: szko³a magii.\n\r" );
	else
		add_buf( buf, "Standard kolorowania czarow: brak.\n\r" );

	sprintf( buffer, "WIMPY ustawione na %d hp\n\r", ch->wimpy );
	add_buf( buf, buffer );

#ifdef MCCP
	if ( ch->desc->out_compress )
		sprintf( buffer, "Kompresja danych w³±czona (ver %d). Aktualny wspó³czynnik kompresji: %.2f.\n\r",
				ch->desc->compressing == TELOPT_COMPRESS ? 1 : 2,
				ch->desc->out_compress->total_in == 0 ? 0.0 : ( double ) ch->desc->out_compress->total_out / ch->desc->out_compress->total_in );
	else
#endif
		sprintf( buffer, "Kompresja danych wy³±czona.\n\r" );
	add_buf( buf, buffer );

	page_to_char( buf_string( buf ), ch );
	free_buf( buf );
	return ;
}

void do_config( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	char buf[ MAX_INPUT_LENGTH ];
	sh_int wimpy = 0;

	if ( IS_NPC( ch ) )
		return ;

	if ( argument[ 0 ] == '\0' )
	{
		show_config( ch );
		return ;
	}

	argument = one_argument( argument, arg );

    //Raszer, show position, mozliwe show pelne i skrocone, SHOWPOSS odpowiada za skrotowe
	if ( !str_prefix( arg, "showposition" ) )
	{
		if ( EXT_IS_SET( ch->act, PLR_SHOWPOS ) )
		{
            if ( EXT_IS_SET( ch->act, PLR_SHOWPOSS ) )
            {
			send_to_char( "Pozycja przeciwnika nie bêdzie wy¶wietlana.\n\r", ch );
			EXT_REMOVE_BIT( ch->act, PLR_SHOWPOS );
			EXT_REMOVE_BIT( ch->act, PLR_SHOWPOSS );
            }
            else
            {
            send_to_char( "Pozycja przeciwnika podczas walki bêdzie wy¶wietlana (wersja skrócona).\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_SHOWPOSS );
            }

		}
		else
		{
			send_to_char( "Pozycja przeciwnika podczas walki bêdzie wy¶wietlana (wersja pe³na).\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_SHOWPOS );
		}
		return ;
	}
    //i koniec showpos

	if ( !str_prefix( arg, "autoexit" ) )
	{
		if ( EXT_IS_SET( ch->act, PLR_AUTOEXIT ) )
		{
			send_to_char( "Wyj¶cia nie bed± wy¶wietlane.\n\r", ch );
			EXT_REMOVE_BIT( ch->act, PLR_AUTOEXIT );
		}
		else
		{
			send_to_char( "Wyj¶cia bed± wy¶wietlane.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_AUTOEXIT );
		}
		return ;
	}

	else if ( !str_prefix( arg, "autogold" ) )
	{
		if ( EXT_IS_SET( ch->act, PLR_AUTOGOLD ) )
		{
			send_to_char( "Nie bêdziesz wyci±gaæ automatycznie pieniêdzy z cia³.\n\r", ch );
			EXT_REMOVE_BIT( ch->act, PLR_AUTOGOLD );
		}
		else
		{
			send_to_char( "Bêdziesz wyci±gaæ automatycznie pieni±dze z cia³.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_AUTOGOLD );
		}
		return ;
	}

	else if ( !str_prefix( arg, "autosplit" ) )
	{
		if ( EXT_IS_SET( ch->act, PLR_AUTOSPLIT ) )
		{
			send_to_char( "Nie bêdziesz automatycznie dzieli<&³/³a/³o> pieniêdzy wyci±gniêtych z cia³.\n\r", ch );
			EXT_REMOVE_BIT( ch->act, PLR_AUTOSPLIT );
		}
		else
		{
			send_to_char( "Bêdziesz automatycznie dzieli<&³/³a/³o> pieni±dze wyci±gniête z cia³.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_AUTOSPLIT );
		}
		return ;
	}

	else if ( !str_prefix( arg, "polish" ) )
	{
		if ( EXT_IS_SET( ch->act, PLR_COMMANDSPL ) )
		{
			send_to_char( "Komendy w jêzyku angielskim.\n\r", ch );
			EXT_REMOVE_BIT( ch->act, PLR_COMMANDSPL );
		}
		else
		{
			send_to_char( "Komendy w jêzyku polskim.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_COMMANDSPL );
		}
		return ;
	}
	else if ( !str_prefix( arg, "acceptsurrender" ) )
	{
		if ( EXT_IS_SET( ch->act, PLR_ACCEPTSURR ) )
		{
			send_to_char( "Nie bêdziesz mieæ lito¶ci dla s³abszych.\n\r", ch );
			EXT_REMOVE_BIT( ch->act, PLR_ACCEPTSURR );
		}
		else
		{
			send_to_char( "Bêdziesz mi³osiern<&y/a/e> dla s³abszych.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_ACCEPTSURR );
		}
		return ;
	}
	else if ( !str_prefix( arg, "automem" ) )
	{
		if ( EXT_IS_SET( ch->act, PLR_AUTOMEM ) )
		{
			send_to_char( "Automem wy³±czony.\n\r", ch );
			EXT_REMOVE_BIT( ch->act, PLR_AUTOMEM );
		}
		else
		{
			send_to_char( "Automem w³±czony.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_AUTOMEM );
		}
		return ;
	}
	else if ( !str_prefix( arg, "groupprompt" ) )
	{
		if ( EXT_IS_SET( ch->act, PLR_COMBAT_PROMPT ) )
		{
			send_to_char( "Prompt grupy wy³±czony.\n\r", ch );
			EXT_REMOVE_BIT( ch->act, PLR_COMBAT_PROMPT );
		}
		else
		{
			send_to_char( "Prompt grupy w³±czony.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_COMBAT_PROMPT );
		}
		return ;
	}

	else if (!str_prefix(arg, "hints" ))
	{
		if (IS_SET(ch->comm, COMM_HINT))
		{
			send_to_char("Kana³ z poradami nie bêdzie wy¶wietlany.\n\r", ch);
			REMOVE_BIT(ch->comm, COMM_HINT);
		}
		else
		{
			send_to_char("Kana³ z poradami bêdzie teraz wy¶wietlany.\n\r", ch);
			SET_BIT(ch->comm, COMM_HINT);

		}
		return;
	}

	else if ( !str_prefix( arg, "prompt" ) )
	{
		if ( IS_SET( ch->comm, COMM_PROMPT ) )
		{
			send_to_char( "Prompt nie bêdzie wy¶wiatlany.\n\r", ch );
			REMOVE_BIT( ch->comm, COMM_PROMPT );
		}
		else
		{
			send_to_char( "Prompt bêdzie wy¶wietlny.\n\r", ch );
			SET_BIT( ch->comm, COMM_PROMPT );
		}
		return ;
	}

	else if ( !str_prefix( arg, "brief" ) )
	{
		if ( IS_SET( ch->comm, COMM_BRIEF ) )
		{
			send_to_char( "Bêda do ciebie docieraæ pe³ne opisy *clap*.\n\r", ch );
			REMOVE_BIT( ch->comm, COMM_BRIEF );
		}
		else
		{
			send_to_char( "Bêda do ciebie docieraæ opisy skrócone, *bonk*.\n\r", ch );
			SET_BIT( ch->comm, COMM_BRIEF );
		}
		return ;
	}

	else if ( !str_prefix( arg, "compact" ) )
	{
		if ( IS_SET( ch->comm, COMM_COMPACT ) )
		{
			send_to_char( "Tekst nie bêdzie ci siê wy¶wietla³ w trybie compact.\n\r", ch );
			REMOVE_BIT( ch->comm, COMM_COMPACT );
		}
		else
		{
			send_to_char( "Tekst bêdzie ci siê wy¶wietla³ w trybie compact.\n\r", ch );
			SET_BIT( ch->comm, COMM_COMPACT );
		}
		return ;
	}
	else if ( !str_prefix( arg, "combine" ) )
	{
		if ( IS_SET( ch->comm, COMM_COMBINE ) )
		{
			send_to_char( "Inwentarz wy¶wietla siê teraz w normalnym trybie.\n\r", ch );
			REMOVE_BIT( ch->comm, COMM_COMBINE );
		}
		else
		{
			send_to_char( "Inwentarz wy¶wietla siê teraz w trybie skroconym.\n\r", ch );
			SET_BIT( ch->comm, COMM_COMBINE );
		}
		return ;
	}
	else if ( !str_prefix( arg, "nofollow" ) )
	{
		if ( EXT_IS_SET( ch->act, PLR_NOFOLLOW ) )
		{
			send_to_char( "Pozwalasz na pod±¿anie za sob±.\n\r", ch );
			EXT_REMOVE_BIT( ch->act, PLR_NOFOLLOW );
		}
		else
		{
			CHARM_DATA *charm;
			int charmies = 0;
			if ( IS_AFFECTED( ch, AFF_CHARM ) )
			{
				send_to_char( "Wcale nie chcesz tego zrobic.\n\r", ch );
				return ;
			}
			// zliczanie ilo¶ci followersów
			// jak go¶æ ma folowersów to niestety nie mo¿e byæ nofollow
			for ( charm = ch->pcdata->charm_list; charm; charm = charm->next )
			{
				charmies++;
			}
			if (charmies>0)
			{
				send_to_char( "No co ty? przecie¿ nie opu¶cisz swoich wyczarowanych przyjació³?\n\r", ch );
				return;
			}
			send_to_char( "Nie pozwalasz na pod±¿anie za soba.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_NOFOLLOW );
			die_follower( ch, TRUE );
		}
		return ;
	}

	else if ( !str_prefix( arg, "revboard" ) )
	{
		if ( EXT_IS_SET( ch->act, PLR_REVBOARD ) )
		{
			send_to_char( "Od teraz widzisz wiadomo¶ci na tablicach w normalnej kolejno¶ci.\n\r", ch );
			EXT_REMOVE_BIT( ch->act, PLR_REVBOARD );
		}
		else
		{
			send_to_char( "Od teraz widzisz wiadomo¶ci na tablicach w odwrotnej kolejno¶ci.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_REVBOARD );
		}
		return ;
	}

	else if ( !str_prefix( arg, "deaf" ) )
	{
		if ( IS_SET( ch->comm, COMM_DEAF ) )
		{
			send_to_char( "Tryb 'deaf' wy³±czony.\n\r", ch );
			REMOVE_BIT( ch->comm, COMM_DEAF );
		}
		else
		{
			send_to_char( "Tryb 'deaf' w³±czony.\n\r", ch );
			SET_BIT( ch->comm, COMM_DEAF );
		}
		return ;
	}
#ifdef ENABLE_SHOW_LFG
	else if ( !str_prefix( arg, "lfg" ) )
	{
		if ( IS_SET( ch->comm, COMM_LFG ) )
		{
			send_to_char( "Tryb 'looking for group' wy³±czony.\n\r", ch );
			REMOVE_BIT( ch->comm, COMM_LFG );
		}
		else
		{
			send_to_char( "Tryb 'looking for group' w³±czony.\n\r", ch );
			SET_BIT( ch->comm, COMM_LFG );
		}
		return ;
	}
#endif
#ifdef ENABLE_SHOW_ON_WHO
	else if ( !str_prefix( arg, "showwho" ) )
		{
			if ( IS_SET( ch->comm, COMM_SHOW_ON_WHO ) )
			{
				send_to_char( "Tryb 'widoczny na who' wy³±czony.\n\r", ch );
				REMOVE_BIT( ch->comm, COMM_SHOW_ON_WHO );
			}
			else
			{
				send_to_char( "Tryb 'widoczny na who' w³±czony.\n\r", ch );
				SET_BIT( ch->comm, COMM_SHOW_ON_WHO );
			}
			return ;
		}
#endif

	else if ( !str_prefix( arg, "newbie" ) )
	{
	  if ( IS_SET( ch->comm, COMM_NONEWBIE ) )
	  {
	    send_to_char( "Bêdziesz widzia³ widzia³ kana³ 'newbie'\n\r", ch );
	    REMOVE_BIT( ch->comm, COMM_NONEWBIE );
	  } else {
	    send_to_char( "Nie bêdziesz widzia³ kana³u 'newbie'\n\r", ch );
	    SET_BIT( ch->comm, COMM_NONEWBIE );
	  }
	  return;
	}

	else if ( !str_prefix( arg, "quiet" ) )
	{
		if ( IS_SET( ch->comm, COMM_QUIET ) )
		{
			send_to_char( "Tryb 'quiet' wy³±czony.\n\r", ch );
			REMOVE_BIT( ch->comm, COMM_QUIET );
		}
		else
		{
			send_to_char( "Tryb 'quiet' w³±czony.\n\r", ch );
			SET_BIT( ch->comm, COMM_QUIET );
		}
		return ;
	}

	else if ( !str_prefix( arg, "wimpy" ) )
	{
		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Brawo! nie bêdziesz uciekaæ z walki.\n\r", ch );
			ch->wimpy = 0;
			return ;
		}

		if ( !is_number( argument ) )
		{
			send_to_char( "Podano b³êdn± warto¶æ, nie bêdziesz uciekaæ z walki.\n\r", ch );
			ch->wimpy = 0;
			return ;
		}

		wimpy = atoi( argument );

		if ( wimpy > get_max_hp(ch) )
		{
			send_to_char( "Tchórzu!!! To wiêcej ni¿ masz.\n\r", ch );
			return ;
		}

		ch->wimpy = wimpy;

		if ( wimpy == 0 )
		{
			send_to_char( "Bêdziesz walczy³<&/a/o> do koñca ka¿dej walki.\n\r", ch );
		}
		else if ( wimpy == get_max_hp(ch) )
		{
			send_to_char( "Bêdziesz teraz uciekaæ szybciej ni¿ zacznie siê walka.\n\r", ch );
		}
		else if ( wimpy > get_max_hp(ch) * 3 / 4 )
		{
			send_to_char( "Bêdziesz teraz uciekaæ zaraz po rozpoczêciu walki.\n\r", ch );
		}
		else if ( wimpy > get_max_hp(ch) / 2 )
		{
			send_to_char( "Bêdziesz uciekaæ gdzie¶ w po³owie walki.\n\r", ch );
		}
		else if ( wimpy > get_max_hp(ch) / 20 )
		{
			send_to_char( "Bêdziesz uciekaæ z walki.\n\r", ch );
		}
		else
		{
			send_to_char( "Prawie nie bêdziesz uciekaæ.\n\r", ch );
		}


		return ;
	}
	else if ( !str_prefix( arg, "scroll" ) )
	{
		int scroll;

		if ( argument[ 0 ] == '\0' )
		{
			sprintf( buf, "Buforowanie strony ustawione na %d linii.\n\r", ch->lines + 2 );
			send_to_char( buf, ch );
			return ;
		}

		if ( !is_number( argument ) )
		{
			send_to_char( argument, ch );
			send_to_char( "B³êdna warto¶æ, podaj liczbê.\n\r", ch );
			return ;
		}

		scroll = atoi( argument );

		if ( scroll < 10 || scroll > 60 )
		{
			send_to_char( "Podaj warto¶æ z zakresu 10-60.\n\r", ch );
			return ;
		}

		ch->lines = scroll - 2;
		send_to_char( "Buforowanie strony ustawione.\n\r", ch );
		return ;
	}
	else if ( !str_prefix( arg, "codepage" ) )
	{

		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Skladnia:\n\r"
					"config codepage iso   - polskie litery w standardzie ISO-8859-2\n\r"
					"config codepage win   - polskie litery w standardzie CP-1250 (Windows)\n\r"
					"config codepage nopol - literki polskawe\n\r", ch );
			return ;
		}

		EXT_REMOVE_BIT( ch->act, PLR_POL_WIN );
		EXT_REMOVE_BIT( ch->act, PLR_POL_ISO );
		EXT_REMOVE_BIT( ch->act, PLR_POL_NOPOL );

		if ( !str_cmp( argument, "iso" ) )
		{
			send_to_char( "Kodowanie polskich liter ustawione na ISO-8859-2.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_POL_ISO );
		}
		else if ( !str_cmp( argument, "win" ) )
		{
			send_to_char( "Kodowanie polskich liter ustawione na CP-1250 (Windows).\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_POL_WIN );
		}
		else
		{
			send_to_char( "Kodowanie polskich liter ustawione na polskawy.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_POL_NOPOL );
		}

		send_to_char( "Test: ±æê³ñó¶¼¿  ¡ÆÊ£ÑÓ¦¬¯\n\r", ch );
		return ;
	}
	else if ( !str_prefix( arg, "spellcolour" ) )
	{

		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Skladnia:\n\r"
					"config spellcolour styl1   - Czary bêd± kolorowane wed³ug powi±zañ z ¿ywio³ami itp.\n\r"
					"config spellcolour styl2   - Czary bêd± kolorowane wed³ug ich charakteru (ofensywne, defensywne itd.)\n\r"
					"config spellcolour styl3   - Czary bêd± kolorowane wed³ug szkó³ magii.\n\r"
					"config spellcolour none    - Czary nie bêd± kolorowane.\n\r", ch );
			return ;
		}

		EXT_REMOVE_BIT( ch->act, PLR_STYL1 );
		EXT_REMOVE_BIT( ch->act, PLR_STYL2 );
		EXT_REMOVE_BIT( ch->act, PLR_STYL3 );
		EXT_REMOVE_BIT( ch->act, PLR_STYL4 );

		if ( !str_cmp( argument, "styl1" ) )
		{
			send_to_char( "Czary bêd± kolorowane wed³ug powi±zañ z ¿ywio³ami itp.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_STYL1 );
		}
		else if ( !str_cmp( argument, "styl2" ) )
		{
			send_to_char( "Czary bêd± kolorowane wed³ug ich charakteru (ofensywne, defensywne itd.)\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_STYL2 );
			send_to_char( "Legenda:\n\r", ch );
			send_to_char( "{RCzary zadaj±ce obra¿enia.{x\n\r", ch );
			send_to_char( "{rCzary przeszkadzaj±ce przeciwnikowi.{x\n\r", ch );
			send_to_char( "{BCzary chroni±ce przed obra¿eniami natury fizycznej.{x\n\r", ch );
			send_to_char( "{cCzary chroni±ce przed obra¿eniami natury magicznej.{x\n\r", ch );
			send_to_char( "{CCzary wspomagaj±ce postaæ w walce.{x\n\r", ch );
			send_to_char( "{MCzary tworz±ce b±d¼ manipuluj±ce przedmiotami.{x\n\r", ch );
			send_to_char( "{YCzary przywo³uj±ce lub pozwalaj±ce przej±æ nad kim¶ kontrolê.{x\n\r", ch );
			send_to_char( "{GCzary uzdrawiaj±ce.{x\n\r", ch );
			send_to_char( "{wCzary u³atwiaj±ce grê, ale nie wp³ywaj±ce bezpo¶rednio na walkê.{x\n\r", ch );
			send_to_char( "{mCzary rzucane na lokacjê.{x\n\r", ch );
			send_to_char( "{yCzary pozosta³e.{x\n\r", ch );
		}
		else if ( !str_cmp( argument, "styl3" ) )
		{
			send_to_char( "Czary bêd± kolorowane wed³ug szkó³ magii.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_STYL3 );
		}
		else
		{
			send_to_char( "Czary nie bêd± kolorowane.\n\r", ch );
			EXT_SET_BIT( ch->act, PLR_STYL4 );
		}

		return ;
	}

#ifdef MCCP
	else if ( !str_prefix( arg, "compress" ) )
	{
		if ( !ch->desc )
		{
			send_to_char( "B³±d. Nie ma deskryptora!\n\r", ch );
			return ;
		}

		if ( !ch->desc->out_compress )
		{
			write_to_buffer( ch->desc, compress2_on_str3, 0 );

			send_to_char( "Negocjacja kompresji danych zainicjowana. Sprawd¼ wynik za chwilê komend± config.\n\r", ch );
		}
		else
		{
			if ( !compressEnd( ch->desc ) )
			{
				send_to_char( "Nie uda³o siê.\n\r", ch );
				return ;
			}

			send_to_char( "Kompresja danych wy³±czona.\n\r", ch );
		}
	}
#endif
	else
	{
		send_to_char( "Nie ma takiej opcji.\n\r", ch );
		do_function( ch, &do_help, "config" );
		return ;
	}

	return ;
}

void do_show( CHAR_DATA *ch, char *argument )
{
	if ( IS_NPC( ch ) )
		return;

	if ( IS_SET( ch->comm, COMM_SHOW_AFFECTS ) )
	{
		send_to_char( "Affekty ju¿ nie bêd± widoczne w twoim 'score'.\n\r", ch );
		REMOVE_BIT( ch->comm, COMM_SHOW_AFFECTS );
	}
	else
	{
		send_to_char( "Affekty od teraz bêd± widoczne w twoim 'score'.\n\r", ch );
		SET_BIT( ch->comm, COMM_SHOW_AFFECTS );
	}
}

void do_prompt( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];
    if ( IS_NPC( ch ) )
    {
        return;
    }
    /**
     * wlacz/wylacz
     */
    if ( argument[ 0 ] == '\0' )
    {
        if ( IS_SET( ch->comm, COMM_PROMPT ) )
        {
            send_to_char("Prompt nie bêdzie wy¶wietlany.\n\r", ch );
            REMOVE_BIT( ch->comm, COMM_PROMPT );
        }
        else
        {
            send_to_char( "Od teraz prompt bêdzie wy¶wietlany.\n\r", ch );
            SET_BIT( ch->comm, COMM_PROMPT );
        }
        return ;
    }
    /**
     * ustaw prompt
     */
    if ( !strcmp( argument, "all" ) )
    {
        strcpy( buf, STANDARD_PROMPT );
    }
    else
    {
        if ( strlen( argument ) > 50 )
        {
            argument[ 50 ] = '\0';
        }
        strcpy( buf, argument );
        smash_tilde( buf );
        if ( str_suffix( "%c", buf ) )
        {
            strcat( buf, " " );
        }
    }
    free_string( ch->prompt );
    ch->prompt = str_dup( buf );
    sprintf( buf, "Prompt ustawiony na: %s\n\r", ch->prompt );
    send_to_char( buf, ch );
    return ;
}

/* ASCI mapka */

int mapuj(MAP_DATA *lokacja, CHAR_DATA *ch)
{
	int i, kod;
	CHAR_DATA *ludzie;
	//rellik: do debugowania
	const char *call = "act_info.c => mapuj";
	char *caller;
	caller = strdup( gcaller );
	strcpy( gcaller, call );
	save_debug_info( call, caller, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );
	//koniec do debugowania
	/* zakladam ze w lokacji nie bedzie wiecej niz 100 mobow */
	if ( lokacja->room->people != NULL
		&& ch->class == CLASS_MAG && ch->pcdata->mage_specialist == 3 )
	{
		if ( ch->level > number_range( 20, 27 ) )
		{
			ludzie = lokacja->room->people;
			for( i = 1; ludzie; i++)
			{
				if ( !IS_NPC( ludzie ) && i < 100 && ch->level > 30 )
				{
					i+=100;
				}
				ludzie = ludzie->next_in_room;
			}
			return (i-1);
		}
		if ( ch->level > number_range( 14, 25 ) ) return -2;
	}
	if ( room_is_dark(ch, lokacja->room)) kod = 100; else kod = 0;
	switch ( lokacja->room->sector_type )
	{

		case  0: // wewn±trz
		case 14: // podziemia
		case 15: // jaskinia
		case 17: // podziemia naturalne
		case 41: // arena
			return (kod + 201);
			break;

		case  1: // miasto
		case 36: // plac
			return (kod + 202);
			break;

		case  3: // las
		case 11: // puszcza
		case 16: // tundra
			return (kod + 203);
			break;

		case 38: // park
			return (kod + 214);
			break;

		case 45: // podziemna droga
			return (kod + 215);
			break;

		case 46: // pustynna droga
			return (kod + 216);
			break;

		case 47: // kopalnia
		case 48: // ruiny
			return (kod + 217);
			break;

		case  8: // nieu¿ytki
			return (kod + 218);
			break;

		case 12: // bagno
			return (kod + 204);
			break;

		case  4: // wzgórza
		case  5: // góry
		case 13: // wysokie góry
		case 35: // górska ¶cie¿ka
		case 43: // stroma ¶cie¿ka
			return (kod + 205);
			break;

		case 18: // droga
		case 28: // ¶cie¿ka
			return (kod + 206);
			break;

		case 23: // lawa
			return (kod + 207);
			break;

		case 24: // ruchome piaski
		case 30: // eden
			return (kod + 208);
			break;

		case 26: // lodowiec
		case 27: // arktyczny l±d
			return (kod + 209);
			break;


		case 10: // pustynia
		case 29: // pla¿a
		case 40: // wydmy
			return (kod + 210);
			break;

		case  2: // pole
		case 31: // sawanna
		case 32: // trawa
		case 33: // step
		case 42: // ³±ka
			return (kod + 211);
			break;

		case 44: // b³otna ¶cie¿ka
			return (kod + 212);
			break;

		case  6: // woda p³ywalna
		case  7: // woda niep³ywalna
		case  9: // powietrze
		case 19: // rzeka
		case 20: // jezioro
		case 21: // morze
		case 22: // ocean
		case 25: // gor±ce ¼ród³a
		case 34: // podwod±
		case 37: // ciemna woda
		case 39: // podziemne jezioro
			return (kod + 213);
			break;

		default:
			return 300; //nie uwzgledniony sector type
			break;
	}
}

MAP_DATA *lista_dodaj(MAP_DATA *wsk, MAP_DATA *element )
{
//rellik: do debugowania
	char *caller;
	const char *call = "act_info.c => *lista_dodaj";
	caller = strdup( gcaller );
	strcpy( gcaller, call );
	save_debug_info( call, caller, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );
//koniec do debugowania
	MAP_DATA *nowy = (MAP_DATA*) malloc ( sizeof(MAP_DATA) );
	if ( nowy != NULL )
	{
		nowy->next = NULL;
		nowy->poz_x = element->poz_x;
		nowy->poz_y = element->poz_y;
		nowy->poz_z = element->poz_z;
		nowy->room = element->room;
		wsk->next = nowy;
	} else {
		bug("Blad alokacji pamieci", 0);
	}
	return nowy;
}

//TODO: (rellik#) mo¿e trochê pozmieniaæ znaczki, mo¿e robiæ znaczek na kolorowym tle?
void show_mapka ( CHAR_DATA *ch, int mapka[16][16], sh_int r)
{
	sh_int i,j, kod;
	char wiadomosc[130];
	char buf[3];
	sh_int d3[4]={3,3,2,1};
	sh_int d5[6]={5,5,4,4,3,1};
	bool ciemno;

	for( i=-r; i < r + 1; i++)
	{
		strcpy(wiadomosc, "\0");
		for( j = 0; j <= ( ( (r == 5)? -d5[abs(i)] : -d3[abs(i)] ) + r ); j++) strcat(wiadomosc, " ");
 		for( j = ( (r == 5)? -d5[abs(i)] : -d3[abs(i)] );	j <= ( (r==5)? d5[abs(i)] : d3[abs(i)] );	j++)
		{
			buf[0]=' ';
			ciemno = FALSE;
			kod = mapka[j+7][i+7];
			if ( kod > 300 )
			{
				kod -= 100;
				ciemno = TRUE;
				strcat( wiadomosc, "{b" ); //granatowy bo zakladam ze ciemno
			}
			if ( kod == 201 )
			{
				buf[0]='*';
				if ( !ciemno ) strcat( wiadomosc, "{w" ); //bialy
			}
			if ( kod == 202 )
			{
				buf[0]='+';
				if ( !ciemno ) strcat( wiadomosc, "{w" ); //bialy
			}
			if ( kod == 203 )
			{
				buf[0]=',';
				if ( !ciemno ) strcat( wiadomosc, "{g" ); //zielony
			}
			if ( kod == 204 )
			{
				buf[0]=';';
				if ( !ciemno ) strcat( wiadomosc, "{c" ); //seledynowy
			}
			if ( kod == 205 )
			{
				buf[0]='^';
				if ( !ciemno ) strcat( wiadomosc, "{y" ); //brazowy
			}
			if ( kod == 206 )
			{
				buf[0]='+';
				if ( !ciemno ) strcat( wiadomosc, "{w" ); //bialy
			}
			if ( kod == 207 )
			{
				buf[0]=':';
				if ( !ciemno ) strcat( wiadomosc, "{r" ); //czerwony
			}
			if ( kod == 208 )
			{
				buf[0]=',';
				if ( !ciemno ) strcat( wiadomosc, "{Y" ); //zolty
			}
			if ( kod == 209 )
			{
				buf[0]='x';
				if ( !ciemno ) strcat( wiadomosc, "{W" ); //bialy
			}
			if ( kod == 210 )
			{
				buf[0]='.';
				if ( !ciemno ) strcat( wiadomosc, "{Y" ); //zolty
			}
			if ( kod == 211 )
			{
				buf[0]='.';
				if ( !ciemno ) strcat( wiadomosc, "{g" ); //zielony
			}
			if ( kod == 212 )
			{
				buf[0]='+';
				if ( !ciemno ) strcat( wiadomosc, "{c" ); //seledynowy
			}
			if ( kod == 213 )
			{
				buf[0]='.';
				if ( !ciemno ) strcat( wiadomosc, "^i{B" ); else strcat( wiadomosc, "^i{b" );  //niebieski lub granatowy
			}
			if ( kod == 300 )
			{
				buf[0]='?';
				strcat( wiadomosc, "{R" ); //czerwony
			}
			if ( kod == 214 )
			{
				buf[0]='+';
				if ( !ciemno ) strcat( wiadomosc, "{g" ); //zielony
			}
			if ( kod == 215 )
			{
				buf[0]='+';
				if ( !ciemno ) strcat( wiadomosc, "{c" ); //seledynowy
			}
			if ( kod == 216 )
			{
				buf[0]='+';
				if ( !ciemno ) strcat( wiadomosc, "{y" ); //zolty
			}
			if ( kod == 217 )
			{
				buf[0]='=';
				if ( !ciemno ) strcat( wiadomosc, "{c" ); //seledynowy
			}
			if ( kod == 218 )
			{
				buf[0]='.';
				if ( !ciemno ) strcat( wiadomosc, "{m" ); //fioletowy
			}
			if ( kod < 200 && kod > 0 )
			{
				if ( kod > 100 )
				{
					strcat( wiadomosc, "{r" ); //jest PC
					kod -= 100;
				} else {
					strcat( wiadomosc, "{g" ); //same NPC
				}
				if ( kod > 9 )
				{
					strcat( wiadomosc, "^f" ) ; //migotanie znaczy jest wiecej niz 9;
					kod = 9;
				}
				sprintf( buf, "%d", kod );
			}
			if ( kod == -2 ) buf[0]='@';
			if ( (i == 0) && (j == 0) )
			{
				buf[0]='o';
			}
			buf[1]='\0';
			strcat( wiadomosc, buf );
			strcat( wiadomosc, "{x" );
		}
		strcat( wiadomosc, "\0" );
		send_to_char( wiadomosc, ch );
		send_to_char( "\n\r", ch );
	}
}

void do_map(CHAR_DATA *ch, char *argument)
{
	bool dalej;
	int mapka[16][16];
	sh_int roz = 5;
	sh_int i, j, znacznik;
	MAP_DATA curr;
	MAP_DATA *tail, *tmp;
	MAP_DATA *head = (MAP_DATA*) malloc ( sizeof(MAP_DATA) );
	EXIT_DATA *wyj;
	char *caller;
	const char *call = "act_info.c => do_map";
	caller = strdup( gcaller );
	strcpy( gcaller, call );
	save_debug_info( call, caller, "wywo³ana mapka", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );

	if ( roz > 3 && roz != 5 ) return;
	if( IS_NPC(ch)) return;//mobom dziêkujemy

	if ( head == NULL )
	{
		save_debug_info( call, caller, "Blad alokacji pamieci", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ERROR, FALSE );
		return;
	}

	if ( ch->position < POS_SLEEPING )
	{
		send_to_char( "Gwiazdy, wszêdzie gwiazdy, to chyba nie ta mapa...\n\r", ch );
		return;
	}
	if ( ch->position == POS_SLEEPING )
	{
		send_to_char( "Patrzysz na mape, nagle perspektywa oddala siê, a mapa zwija siê w kule, widzisz jak ludzie spadaja w otch³añ, brrr, lepiej siê obud¼!\n\r", ch );
		return;
	}
	if ( !check_blind( ch ) )
	{
        send_to_char( "Ciemno¶æ, wszêdzie ciemno¶æ...", ch );
		return;
	}
    sprintf( log_buf, "[%d] %s use a map command.",
            ch->in_room ? ch->in_room->vnum : 0,
            ch->name
           );
    log_string( log_buf );

	head->next = NULL;
	head->poz_x = 0;
	head->poz_y = 0;
	head->poz_z = 0;
	head->room = ch->in_room;
	curr.next = NULL;
	curr.poz_x = 0;
	curr.poz_y = 0;
	curr.poz_z = 0;
	curr.room = head->room;
	tail = head;
	dalej = TRUE;
	znacznik = number_range( 0, 32000 );
	head->room->touched = znacznik;
	for( i = 0; i < 15; i++ )
	{
		for( j = 0; j < 15; j++ )
		{
			mapka[i][j] = -1;
		}
	}
	while( dalej )
	{
		mapka[head->poz_x+7][head->poz_y+7] = mapuj(head, ch);
		strcpy( gcaller, call ); //rellik: do debugowania, przywrocenie globalnej zmiennej przechowywujacej nazwe funkcji wywolujacej aktualna funkcje
		for( i=0; i < 6; i++ )
		{
			if ( head->room->exit[i] != NULL )
			{
				wyj = head->room->exit[i];
				if( ( wyj->u1.to_room != NULL )
					&& !IS_SET( wyj->exit_info, EX_SECRET )
					&& ( !IS_SET( wyj->exit_info, EX_HIDDEN ) || IS_AFFECTED( ch, AFF_DETECT_HIDDEN ) )
					&& !IS_SET( wyj->exit_info, EX_CLOSED )
					&& ( head->room->exit[i]->u1.to_room->touched != znacznik ) )
				{
					curr.poz_x = head->poz_x;
					curr.poz_y = head->poz_y;
					curr.poz_z = head->poz_z;
					switch( i )
					{
					case DIR_NORTH:
						{
							curr.poz_y--;
						} break;
					case DIR_EAST:
						{
							curr.poz_x++;
						} break;
					case DIR_SOUTH:
						{
							curr.poz_y++;
						} break;
					case DIR_WEST:
						{
							curr.poz_x--;
						} break;
					case DIR_UP:
						{
							curr.poz_z++;
						} break;
					case DIR_DOWN:
						{
							curr.poz_z--;
						} break;
					}
					curr.room = head->room->exit[i]->u1.to_room;

					if ( ( curr.poz_x >= -roz )
						&& ( curr.poz_x <= roz )
						&& ( curr.poz_y <= roz )
						&& ( curr.poz_y >= -roz )
						&& ( curr.poz_z == 0 ) )
					{
						tail = lista_dodaj( tail, &curr );
						strcpy(gcaller, call); //rellik: do debugowania, przywrócenie zmiennej gcaller przechowywuj±cej nazwê funkcji która wywo³uje aktualn± funkcjê
						if ( tail == NULL )
						{
							bug("Blad alokacji pamieci",0);
						}
					}
					head->room->exit[i]->u1.to_room->touched = znacznik;
				}
			}
		}

		tmp=head;
		head = head->next;
		free(tmp);
		if (head == NULL)
		{
			dalej = FALSE;
		}
	}
	show_mapka( ch, mapka, roz );
	strcpy( gcaller, "NIEZDEFINIOWANY" );
}

void do_look( CHAR_DATA *ch, char *argument )
{
	ROOM_INDEX_DATA * in_room;
	char buf [ MAX_STRING_LENGTH ];
	char *buf1;
	char arg1 [ MAX_INPUT_LENGTH ];
	char arg2 [ MAX_INPUT_LENGTH ];
	char arg3 [ MAX_INPUT_LENGTH ];
	char fullarg [ MAX_INPUT_LENGTH ];
	EXIT_DATA *pexit;
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	char *pdesc;
	int door, iter;
	int number, count;

	if ( ch->desc == NULL )
		return ;

	if ( IS_NPC( ch ) || !ch->pcdata->mind_in )
	{
		if ( ch->position < POS_SLEEPING )
		{
			send_to_char( "Nie widzisz niczego poza gwiazdami!\n\r", ch );
			return ;
		}

		if ( ch->position == POS_SLEEPING )
		{
			if ( ch->sex == 0 )
			{
				send_to_char( "¦nisz jaki¶ koszmar.\n\r", ch );
			}
			else if ( ch->sex == 1 )
			{
				send_to_char( "Go³e kobiety uwijaj± siê dooko³a ciebie, alkohol i pla¿a... szkoda, ¿e to tylko sen!\n\r", ch );
			}
			else
			{
				send_to_char( "Futra, bi¿uteria , doskona³a s³u¿ba... có¿ za doskona³y sen.\n\r", ch );
			}
		}

		if ( !check_blind( ch ) )
		{
			send_to_char( "Ciemno¶æ! Widzisz ciemno¶æ!\n\r", ch );
			return ;
		}
	}

	if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
		in_room = ch->pcdata->mind_in;
	else
		in_room = ch->in_room;

	sprintf( fullarg, "%s", argument );
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	number = number_argument( arg1, arg3 );
	count = 0;

	if ( arg1[ 0 ] == '\0' || !str_cmp( arg1, "auto" ) )
	{
		/* 'look' or 'look auto' */

		if ( !IS_NPC( ch )
				&& !EXT_IS_SET( ch->act, PLR_HOLYLIGHT )
				&& !IS_AFFECTED( ch, AFF_DARK_VISION )
				&& room_is_dark(ch, ch->in_room ) )
		{
			if ( EXT_IS_SET( in_room->room_flags, ROOM_MAGICDARK ) )
			{
				send_to_char( "Jest tu nienaturalnie ciemno, nie jeste¶ w stanie niczego dostrzec.\n\r", ch );
			}
			else
			{
				send_to_char( "Nie jeste¶ w stanie niczego dostrzec w tych ciemno¶ciach.\n\r", ch );
			}
			show_char_to_char( in_room->people, ch );
			return ;
		}

		sprintf( buf, "{s%s{x", in_room->name );
		send_to_char( buf, ch );

		if ( ( IS_IMMORTAL( ch ) && IS_BUILDER( ch, in_room->area ) ) )
        {
			sprintf( buf, " {r[{R%5d{r] {{%s}\n\r", in_room->vnum, sector_table[in_room->sector_type].name );
        }
		else
			sprintf( buf, "\n\r" );

		send_to_char( buf, ch );

		if ( !IS_NPC( ch ) && EXT_IS_SET( ch->act, PLR_AUTOEXIT ) )
		{
			if ( IS_AFFECTED(ch, AFF_DARK_VISION ))
				do_function( ch, &do_exits, "auto" );
			else if ( !room_is_dark(ch, ch->in_room ) || IS_IMMORTAL( ch ) )
				do_function( ch, &do_exits, "auto" );
		}


		if ( arg1[ 0 ] == '\0'
				|| ( !IS_NPC( ch ) && !IS_SET( ch->comm, COMM_BRIEF ) ) )
		{
			if ( EXT_IS_SET( in_room->room_flags, ROOM_RANDDESC ) )
			{
				if ( in_room->nightdesc[ 0 ] == '\0' ||
						( time_info.hour < HOUR_NIGHT_BEGIN && time_info.hour >= HOUR_DAY_BEGIN ) )
				{
					create_rand_desc( in_room, in_room->description, buf );
					buf1 = str_dup( buf );
					sprintf( buf, "{S%s{x\n\r", format_string( buf1 ) );
				}
				else
				{
					create_rand_desc( in_room, in_room->nightdesc, buf );
					buf1 = str_dup( buf );
					sprintf( buf, "{S%s{x\n\r", format_string( buf1 ) );
				}
			}
			else
			{
				if ( in_room->nightdesc[ 0 ] == '\0' ||
						( time_info.hour < HOUR_NIGHT_BEGIN && time_info.hour >= HOUR_DAY_BEGIN ) )
					sprintf( buf, "{S%s{x\n\r", in_room->description );
				else
					sprintf( buf, "{S%s{x\n\r", in_room->nightdesc );
			}

			send_to_char( buf, ch );
		}

		show_list_to_char( in_room->contents, ch, FALSE, FALSE );
		show_char_to_char( in_room->people, ch );
		return ;
	}

	if ( !str_cmp( arg1, "do" ) || !str_cmp( arg1, "in" ) || !str_cmp( arg1, "on" ) )
	{
		/* 'look in' */
		if ( arg2[ 0 ] == '\0' )
		{
			send_to_char( "A na co chcesz popatrzeæ?\n\r", ch );
			return ;
		}

		if ( ( obj = get_obj_here( ch, arg2 ) ) == NULL )
		{
			send_to_char( "Niczego takiego tutaj nie widzisz.\n\r", ch );
			return ;
		}

		switch ( obj->item_type )
		{
			default:
				send_to_char( "To nie jest pojemnik.\n\r", ch );
				break;

			case ITEM_ENVENOMER:
			case ITEM_DRINK_CON:
				if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
				{
					send_to_char( "Nie mo¿esz tego sprawdziæ w takim stanie.\n\r", ch );
					break;
				}

				if ( obj->value[ 1 ] == 0 )
				{
					send_to_char( "To jest puste.\n\r", ch );
					break;
				};

				if ( obj->value[ 1 ] == -1 )
				{
					print_char( ch, "Przygl±dasz siê uwa¿nie i wach±sz zawarto¶æ. To %s.\n\rWydaje siê byæ pe³ne po brzegi.\n\r", liq_table[ obj->value[ 2 ] ].liq_name );
					break;
				};

				sprintf( buf, "Przygl±dasz siê uwa¿nie i wach±sz zawarto¶æ. To %s.\n\r%s.\n\r",
						liq_table[ obj->value[ 2 ] ].liq_name,
						obj->value[ 1 ] < obj->value[ 0 ] / 6
						? "Chlupocze tam jeszcze tego troche na dnie." :
						obj->value[ 1 ] < 2 * obj->value[ 0 ] / 6
						? "Sporo mniej tego ni¿ po³owa." :
						obj->value[ 1 ] < 3 * obj->value[ 0 ] / 6
						? "Mniej tego ni¿ po³owa." :
						obj->value[ 1 ] < 4 * obj->value[ 0 ] / 6
						? "Wiêcej tego ni¿ po³owa." :
						obj->value[ 1 ] < 5 * obj->value[ 0 ] / 6
						? "Du¿o wiêcej tego ni¿ po³owa." : "Wiele ju¿ chyba tam tego nie wejdzie."
				       );


				send_to_char( buf, ch );
				break;


			case ITEM_BOARD:
				send_to_char( obj->description, ch );
				send_to_char( "\n\r", ch );
				show_contents( ch, obj );

				break;


			case ITEM_CONTAINER:
				if ( IS_SET( obj->value[ 1 ], CONT_CLOSED ) )
				{
					send_to_char( "Zamkniête.\n\r", ch );
					if ( !IS_AFFECTED( ch, AFF_PIERCING_SIGHT ) )
						break;
				}
			case ITEM_CORPSE_NPC:
			case ITEM_CORPSE_PC:
				if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
				{
					send_to_char( "Nie mo¿esz tego sprawdziæ w takim stanie.\n\r", ch );
					break;
				}

				if ( !obj->carried_by )
					act( "$p (le¿y na ziemi) zawiera:", ch, obj, NULL, TO_CHAR );
				else
					act( "$p (nosisz przy sobie) zawiera:", ch, obj, NULL, TO_CHAR );

				show_list_to_char( obj->contains, ch, TRUE, TRUE );
				break;
		}
		return ;
	}

	if (!str_cmp (arg1, "moon") || !str_cmp (arg1, "ksiê¿yc"))
	{
		do_function(ch, &do_moon_phase, "");
		return;
	}

	if ( ( victim = get_char_room( ch, arg1 ) ) != NULL )
	{
		show_char_to_char_1( victim, ch );
		return ;
	}

	for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
	{

		if ( can_see_obj( ch, obj ) )
		{/* player can see object */
			pdesc = get_extra_descr( arg3, obj->extra_descr );
			if ( pdesc != NULL )
			{
				if ( ++count == number )
				{
					send_to_char( pdesc, ch );
					return ;
				}
				else continue;
			}

			pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );

			if ( pdesc != NULL )
			{
				if ( ++count == number )
				{
					send_to_char( pdesc, ch );
					return ;
				}
				else continue;
			}

			if ( is_name( arg3, obj->name ) )
				if ( ++count == number )
				{
					if ( obj->item_description && obj->item_description[ 0 ] != '\0' )
					{
						if ( obj->item_type == ITEM_NOTEPAPER && obj->value[ 0 ] == 1 )
						{
							send_to_char( "Kto¶ tutaj napisa³:\n\r", ch );
						}
						send_to_char( obj->item_description, ch );
					}
					else
						send_to_char( obj->description, ch );

					send_to_char( "\n\r", ch );

					if ( obj->item_type == ITEM_BOARD )
						show_contents( ch, obj );

					return ;
				}
		}
	}

	for ( obj = in_room->contents; obj != NULL; obj = obj->next_content )
	{
		if ( can_see_obj( ch, obj ) )
		{
			pdesc = get_extra_descr( arg3, obj->extra_descr );
			if ( pdesc != NULL )
				if ( ++count == number )
				{
					send_to_char( pdesc, ch );
					return ;
				}

			pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
			if ( pdesc != NULL )
				if ( ++count == number )
				{
					send_to_char( pdesc, ch );
					return ;
				}

			if ( is_name( arg3, obj->name ) )
				if ( ++count == number )
				{
					if ( obj->item_description && obj->item_description[ 0 ] != '\0' )
						send_to_char( obj->item_description, ch );
					else
						send_to_char( obj->description, ch );
					send_to_char( "\n\r", ch );

					if ( obj->item_type == ITEM_BOARD )
						show_contents( ch, obj );

					return ;
				}
		}
	}

	pdesc = get_extra_descr( arg3, in_room->extra_descr );
	if ( pdesc != NULL )
	{
		if ( ++count == number )
		{
			send_to_char( pdesc, ch );
			return ;
		}
	}

	if ( count > 0 && count != number )
	{
		if ( count == 1 )
		{
			sprintf( buf, "Widzisz tu tylko jedn± tak± rzecz jak %s.\n\r", arg3 );
		}
		else
		{
			sprintf( buf, "Widzisz tu %d takich rzeczy.\n\r", count );
		}
		send_to_char( buf, ch );
		return ;
	}

	door = get_door( ch, fullarg );

	if ( door >= 0 && check_vname( ch, door, FALSE ) )
	{
		send_to_char( "W jakim kierunku chcesz spojrzeæ?\n\r", ch );
		return ;
	}

	if ( door < 0 )
	{
		door = -1;
		for ( iter = 0; iter <= 5; iter++ )
		{
			if ( ( pexit = in_room->exit[ iter ] ) != NULL
					&& pexit->vName != NULL
					&& !str_prefix( fullarg, pexit->vName )
					&& !( IS_SET( pexit->exit_info, EX_HIDDEN ) && !IS_AFFECTED( ch, AFF_DETECT_HIDDEN ) ) )
			{
				door = iter;
				break;
			}
		}
	}

	if ( door < 0 )
	{
		send_to_char( "W jakim kierunku chcesz spojrzeæ?\n\r", ch );
		return ;
	}

	/* 'look direction' */
	if ( ( pexit = in_room->exit[ door ] ) == NULL )
	{
		send_to_char( "Nic specjalnego tam nie widaæ.\n\r", ch );
		return ;
	}

	if ( ( IS_SET( pexit->exit_info, EX_HIDDEN ) && !IS_AFFECTED( ch, AFF_DETECT_HIDDEN ) ) )
	{
		send_to_char( "Nic specjalnego tam nie widaæ.\n\r", ch );
		return ;
	}

	if ( pexit->nightdescription == NULL || pexit->nightdescription [ 0 ] == '\0' ||
			( time_info.hour < HOUR_NIGHT_BEGIN && time_info.hour >= HOUR_DAY_BEGIN ) )
	{
		if ( pexit->description != NULL && pexit->description[ 0 ] != '\0' )
			send_to_char( pexit->description, ch );
		else
			send_to_char( "Nic ciekawego.\n\r", ch );
	}
	else
		send_to_char( pexit->nightdescription, ch );

	if ( pexit->keyword != NULL
			&& pexit->keyword[ 0 ] != '\0'
			&& pexit->keyword[ 0 ] != ' ' )
	{
		if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
		{
			if( !pexit->liczba_mnoga )
				act( "$d jest zamkniêta.", ch, NULL, pexit->keyword, TO_CHAR );
			else
				act( "$d s± zamkniête.", ch, NULL, pexit->keyword, TO_CHAR );
		}
		else if ( IS_SET( pexit->exit_info, EX_ISDOOR ) )
		{
			if( !pexit->liczba_mnoga )
				act( "$d jest otwarta.", ch, NULL, pexit->keyword, TO_CHAR );
			else
				act( "$d s± otwarte.", ch, NULL, pexit->keyword, TO_CHAR );
		}
		if ( IS_SET( pexit->exit_info, EX_HIDDEN ) )
		{
			if( !pexit->liczba_mnoga )
				act( "$d jest ukryta.", ch, NULL, pexit->keyword, TO_CHAR );
			else
				act( "$d s± ukryte.", ch, NULL, pexit->keyword, TO_CHAR );
		}
	}

	return ;
}

void do_examine( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	char *sbuf;
	AFFECT_DATA *poison = NULL;
	OBJ_DATA *obj;
	int condition, mod, low, high, speakswell;

	// je¿li ch jest mobem, to olewamy ca³e to sprawdzanie, bo i poco? na
	// zewn±tz i tak nie leci komunikat, ¿e kto¶ co¶ ogl±da,
	if ( IS_NPC( ch ) ) return ;

	one_argument( argument, arg );

	buf[ 0 ] = '\0';
	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Co sprawdziæ?\n\r", ch );
		return ;
	}

	obj = get_obj_here( ch, arg );

	if ( !obj )
		do_function( ch, &do_look, arg );

	if ( obj )
	{
		if ( obj->item_type == ITEM_NOTEPAPER && obj->value[ 0 ] == 1 )
		{
			if ( ch->level > LEVEL_HERO )
				send_to_char( "Kto¶ tutaj napisa³:\n\r", ch );
			else
				send_to_char( "tu imiê tutaj napisa³:\n\r", ch );
			speakswell = UMIN( obj->value[1], knows_language( ch, ch->speaking, NULL ) );
			if ( speakswell < 95 )
			{
				sbuf = translate( speakswell, obj->item_description, lang_table[ obj->value[2] ].name );
				send_to_char( sbuf, ch );
			}
			else
				send_to_char( obj->item_description, ch );
			send_to_char( "\n\r", ch );
		}
		else
		{
			if ( obj->item_description && obj->item_description[ 0 ] != '\0' )
				send_to_char( obj->item_description, ch );
			else
				send_to_char( obj->description, ch );
			send_to_char( "\n\r", ch );
		}

		if ( obj->item_type == ITEM_BOARD )
		{
			show_contents( ch, obj );
		}

		// pobieram ni¿sz i wy¿sz± cechê,

		low  = UMIN(get_curr_stat_deprecated(ch, STAT_INT), get_curr_stat_deprecated(ch, STAT_WIS));
		high = UMAX(get_curr_stat_deprecated(ch, STAT_INT), get_curr_stat_deprecated(ch, STAT_WIS));

		// wynik dzia³ania algorytmu le¿y w katalogu ../docs/ w pliku do_examine.txt
		// generator tegeo pliku równie¿ tam le¿y, pod nazw± do_examine.txt, który
		// mo¿ne po skopiowaniu dowolnie modyfikowaæ, a wstawiaj±c pod zmienn± $fx
		// inn± funkcjê dostaniemy inny rozk³ad, wiêc ... czêstujcie siê
		//
		// Taaa, tylko po co to condition, skoro mo¿na oceniaæ wg samej kondycji
		// przedmiotu?
		//
		// Ano poto, ¿eby taka ocena nabra³a pozorów losowo¶ci, tak ¿eby patrz±c
		// na przedmiot dwa razy inaczej go oceniæ. Nale¿y tylko pamiêtaæ
		// o zmiejszaniszaniu b³êdu w ocenie dla danego rodzaju przedmiotu oraz
		// konkretnej klasy, bo przecie wojownik lepiej ni¿ kletyk ocenia pancerz,
		// a ubrania, to chyba najlepiej oceni bard, nie?
		//
		// 2004-07-22, Gurthg
		mod = ( ( 40  - high / 2 - low ) * low ) / high;

		if ( number_range(0, 1) == 0) mod *= -1;


		switch ( obj->item_type )
		{
			default:
				break;

			case ITEM_WEAPON:

				switch ( ch->class )
				{
					default: break;
					case CLASS_MAG:
					case CLASS_CLERIC:
					case CLASS_DRUID:
					case CLASS_MONK:
						 break;
					case CLASS_THIEF:
						 mod /= 2;
						 break;
					case CLASS_WARRIOR:
					case CLASS_BLACK_KNIGHT:
					case CLASS_PALADIN:
					case CLASS_BARBARIAN:
						 mod /= 4;
						 break;
					case CLASS_BARD:
					case CLASS_SHAMAN:
						 mod /= 3;
						 break;
				}

				condition = obj->condition + mod;

				if ( condition >= 95 )
				{
					sprintf( buf, "%s jest w znakomitym stanie.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 85 )
				{
					if ( obj->material == 2 || obj->material == 3 || obj->material == 4 || obj->material == 13 || obj->material == 32 ) //metalowe
					{
						sprintf( buf, "%s ma parê szczerb i kilka plamek rdzy.\n\r", capitalize_first( obj->short_descr ) );
					}
					else if ( obj->material == 1 ) //drewno
					{
						sprintf( buf, "%s ma parê rys i pêkniêæ.\n\r", capitalize_first( obj->short_descr ) );
					}
					else if ( obj->material == 21 ) //ko¶æ
					{
						sprintf( buf, "%s ma parê rys i odprysków.\n\r", capitalize_first( obj->short_descr ) );
					}
					else
					{
						sprintf( buf, "%s ma parê rys.\n\r", capitalize_first( obj->short_descr ) );
					}
				}
				else if ( condition > 70 )
				{
					if ( obj->material == 2 || obj->material == 3 || obj->material == 4 || obj->material == 13 || obj->material == 32 )
					{
						sprintf( buf, "%s wygl±da ca³kiem nie¼le.\n\r", capitalize_first( obj->short_descr ) );
					}
					else if ( obj->material == 1 ) //drewno
					{
						sprintf( buf, "%s ma parê g³êbszych rys i kilka do¶æ g³êbokich pêkniêæ.\n\r", capitalize_first( obj->short_descr ) );
					}
					else if ( obj->material == 21 ) //ko¶æ
					{
						sprintf( buf, "%s ma parê g³êbszych rys i do¶æ spore odpryski.\n\r", capitalize_first( obj->short_descr ) );
					}
					else
					{
						sprintf( buf, "%s ma parê g³êbokich rys.\n\r", capitalize_first( obj->short_descr ) );
					}
				}
				else if ( condition > 55 )
				{
					if ( obj->material == 2 || obj->material == 3 || obj->material == 4 || obj->material == 13 || obj->material == 32 )
					{
						sprintf( buf, "%s nie wygl±da najgorzej.\n\r", capitalize_first( obj->short_descr ) );
					}
					else if ( obj->material == 1 ) //drewno
					{
						sprintf( buf, "%s ma g³êbokie pêkniêcia.\n\r", capitalize_first( obj->short_descr ) );
					}
					else if ( obj->material == 21 ) //ko¶æ
					{
						sprintf( buf, "%s ma parê du¿ych ubytków i do¶æ du¿e pêkniêcia.\n\r", capitalize_first( obj->short_descr ) );
					}
					else
					{
						sprintf( buf, "%s ma parê rys i pêkniêæ.\n\r", capitalize_first( obj->short_descr ) );
					}
				}
				else if ( condition > 40 )
				{
					if ( obj->material == 2 || obj->material == 3 || obj->material == 4 || obj->material == 13 || obj->material == 32 )
					{
						sprintf( buf, "%s wygl±da do¶æ kiepsko.\n\r", capitalize_first( obj->short_descr ) );
					}
					else if ( obj->material == 1 ) //drewno
					{
						sprintf( buf, "%s ma g³êbokie i szerokie pêkniêcia i parê zbutwia³ych kawa³ków.\n\r", capitalize_first( obj->short_descr ) );
					}
					else if ( obj->material == 21 ) //ko¶æ
					{
						sprintf( buf, "%s ma g³êbokie pêkniêcia i zniszczon± powierzchniê.\n\r", capitalize_first( obj->short_descr ) );
					}
					else
					{
						sprintf( buf, "%s ma parê g³êbokich pêkniêæ.\n\r", capitalize_first( obj->short_descr ) );
					}
				}
				else if ( condition > 25 )
				{
					if ( obj->material == 2 || obj->material == 3 || obj->material == 4 || obj->material == 13 || obj->material == 32 )
					{
						sprintf( buf, "%s prosi siê o wizytê u kowala.\n\r", capitalize_first( obj->short_descr ) );
					}
					else if ( obj->material == 1 ) //drewno
					{
						sprintf( buf, "%s trzyma siê na ostatnich w³óknach drewna.\n\r", capitalize_first( obj->short_descr ) );
					}
					else if ( obj->material == 21 ) //ko¶æ
					{
						if (obj->liczba_mnoga)
						{
							sprintf( buf, "%s s± sitowate i pewnie zaraz zaczn± siê rozpadaæ.\n\r", capitalize_first( obj->short_descr ) );
						}
						else
						{
							switch (obj -> gender)
							{
								case 2:
									sprintf( buf, "%s jest sitowaty i pewnie zaraz zacznie siê rozpadaæ.\n\r", capitalize_first( obj->short_descr ) );
									break;
								case 3:
									sprintf( buf, "%s jest sitowata i pewnie zaraz zacznie siê rozpadaæ.\n\r", capitalize_first( obj->short_descr ) );
									break;
								default :
									sprintf( buf, "%s jest sitowate i pewnie zaraz siê zacznie rozpadaæ.\n\r", capitalize_first( obj->short_descr ) );
									break;
							}
						}
					}
					else
					{
						sprintf( buf, "%s prosi siê o naprawê.\n\r", capitalize_first( obj->short_descr ) );
					}
				}
				else if ( condition > 10 )
				{
					if( obj->liczba_mnoga )
					{
						sprintf( buf, "%s za jaki¶ czas rozpadn± siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
					}
					else
					{
						sprintf( buf, "%s za jaki¶ czas rozpadnie siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
					}
				}
				else
				{
					if( obj->liczba_mnoga )
					{
						sprintf( buf, "%s zaraz rozpadn± siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
					}
					else
					{
						sprintf( buf, "%s zaraz rozpadnie siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
					}
				}

				send_to_char( buf, ch );

				for ( poison = obj->affected; poison; poison = poison->next )
				{
					if ( poison->type == gsn_poison )
					{
						switch ( poison->duration )
						{
							case 1:
							case 2:
								print_char( ch, "Trucizna pokrywaj±ca %s prawie ju¿ wysch³a.\n\r", obj->name4 );
								break;
							case 3:
								print_char( ch, "Trucizna pokrywaj±ca %s zaczyna powoli wysychaæ.\n\r", obj->name4 );
								break;
							case 4:
								print_char( ch, "%s pokrywa cienka warstwa trucizny.\n\r", capitalize_first( obj->short_descr ) );
								break;
							case 5:
								print_char( ch, "%s pokrywa warstwa trucizny.\n\r", capitalize_first( obj->short_descr ) );
								break;
							default:
								print_char( ch, "%s pokrywa gruba warstwa trucizny.\n\r", capitalize_first( obj->short_descr ) );
								break;
						}
						break;
					}
				}
				break;

			case ITEM_SHIELD:
			case ITEM_ARMOR:
				switch ( ch->class )
				{
					default: break;
					case CLASS_MAG:
					case CLASS_CLERIC:
					case CLASS_DRUID:
					case CLASS_MONK:
						 break;
					case CLASS_THIEF:
						 mod /= 2;
						 break;
					case CLASS_WARRIOR:
					case CLASS_BLACK_KNIGHT:
					case CLASS_PALADIN:
					case CLASS_BARBARIAN:
						 mod /= 4;
						 break;
					case CLASS_BARD:
					case CLASS_SHAMAN:
						 mod /= 3;
						 break;
				}

				condition = obj->condition + mod;

				if ( condition >= 95 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s s± w znakomitym stanie.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s jest w znakomitym stanie.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 85 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s maj± parê rys i wgnieceñ.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s ma parê rys i wgnieceñ.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 70 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s wygl±daj± ca³kiem nie¼le.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s wygl±da ca³kiem nie¼le.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 55 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s nie wygl±daj± najgorzej.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s nie wygl±da najgorzej.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 40 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s wygl±daj± do¶æ kiepsko.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s wygl±da do¶æ kiepsko.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 25 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s prosz± siê o wizytê u p³atnerza.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s prosi siê o wizytê u p³atnerza.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 10 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s za jaki¶ czas rozpadn± siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s za jaki¶ czas rozpadnie siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
				}
				else
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s zaraz rozpadn± siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s zaraz rozpadnie siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
				}

				send_to_char( buf, ch );
				break;

			case ITEM_CLOTHING:
			case ITEM_MUSICAL_INSTRUMENT:

				switch ( ch->class )
				{
					default: break;
					case CLASS_MAG:
					case CLASS_CLERIC:
					case CLASS_THIEF:
					case CLASS_DRUID:
						 mod /= 2;
						 break;
					case CLASS_WARRIOR:
					case CLASS_BLACK_KNIGHT:
					case CLASS_PALADIN:
					case CLASS_BARBARIAN:
					case CLASS_SHAMAN:
					case CLASS_MONK:
						 break;
					case CLASS_BARD:
						 mod /= 3;
						 // tak wiem ¿e bêdzie siê dzieliæ dwa razy dla intrumentów, czyli pomy³ka
						 // barda dotycz±ca tego jak zniszczone s± instrumenty bêdzie zmiejszona
						 // sumarycznie o 6 czyli mod /= 6;
						 if ( obj->item_type == ITEM_MUSICAL_INSTRUMENT )
						 {
							 mod /= 2;
						 }
						 break;
				}

				condition = obj->condition + mod;

				if ( condition >= 100 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s s± w znakomitym stanie.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s jest w znakomitym stanie.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 75 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s wygl±daj± ca³kiem nie¼le.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s wygl±da ca³kiem nie¼le.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 55 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s nie wygl±daj± najgorzej.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s nie wygl±da najgorzej.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 35 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s prosz± siê o naprawê.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s prosi siê o naprawê.\n\r", capitalize_first( obj->short_descr ) );
				}
				else
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s prawie rozpadaj± siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s prawie rozpada siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
				}

				send_to_char( buf, ch );
				break;

			case ITEM_MONEY:

				if ( obj->value[ 0 ] == 1 )
				{
					sprintf( buf, "Tylko jeden, samotny srebrniak.\n\r" );
				}
				else if ( obj->value[ 0 ] > 1 && obj->value[ 0 ] < 5 )
				{
					sprintf( buf, "%d srebrne monety w kupce.\n\r", obj->value[ 0 ] );
				}
				else
				{
					sprintf( buf, "%d srebrnych monet w kupce.\n\r", obj->value[ 0 ] );
				}

				send_to_char( buf, ch );
				break;

			case ITEM_LIGHT:
				if ( obj->value[ 2 ] > 16 || obj->value[ 2 ] == -1 )
				{
					print_char( ch, "%s po¶wieci jeszcze bardzo d³ugo.\n\r", capitalize( obj->short_descr ) );
				}
				else if ( obj->value[ 2 ] > 8 )
				{
					print_char( ch, "%s po¶wieci jeszcze wystarczaj±co d³ugo.\n\r", capitalize( obj->short_descr ) );
				}
				else if ( obj->value[ 2 ] > 4 )
				{
					print_char( ch, "%s po¶wieci jeszcze jaki¶ czas.\n\r", capitalize( obj->short_descr ) );
				}
				else if ( obj->value[ 2 ] > 2 )
				{
					print_char( ch, "%s po¶wieci jeszcze troszkê.\n\r", capitalize( obj->short_descr ) );
				}
				else if ( obj->value[ 2 ] == 2 )
				{
					print_char( ch, "%s po¶wieci jeszcze mo¿e z godzinê.\n\r", capitalize( obj->short_descr ) );
				}
				else if ( obj->value[ 2 ] == 1 )
				{
					print_char( ch, "%s zaczyna powoli przygasaæ.\n\r", capitalize( obj->short_descr ) );
				}
				else
				{
					print_char( ch, "%s nie daje ju¿ ¶wiat³a.\n\r", capitalize( obj->short_descr ) );
				}
                /**
                 * pokazuj tylko dla obiektów, które mo¿na wzi±¶æ do ³apki
                 * bo dla reszty nie ma sensu tego pokazywaæ
                 */
                if ( IS_SET( obj->wear_flags, ITEM_TAKE ) )
                {
                    if ( obj->value[1] == 0 )
                    {
                        print_char( ch, "%s nie daje siê zamocowaæ.\n\r", capitalize( obj->short_descr ) );
                    }
                    else
                    {
                        if ( IS_SET(obj->value[1], LIGHT_LIE ) )
                        {
                            print_char( ch, "%s mo¿e byæ po³o¿ona na ziemi.\n\r", capitalize( obj->short_descr ) );
                        }
                        if ( IS_SET(obj->value[1], LIGHT_HANG ) )
                        {
                            print_char( ch, "%s mo¿e byæ zawieszona.\n\r", capitalize( obj->short_descr ) );
                        }
                        if ( IS_SET(obj->value[1], LIGHT_STICK ) )
                        {
                            print_char( ch, "%s mo¿e byæ wbijana w ziemiê.\n\r", capitalize( obj->short_descr ) );
                        }
                    }
                }
				if ( obj->value[3] == VUL_RAIN || obj->value[3] == VUL_WIND )
					print_char( ch, "%s zdaje siê byæ wra¿liwa na pogodê.\n\r", capitalize( obj->short_descr ) );

				break;

			case ITEM_FOOD:
				if ( obj->value[ 3 ] != 0 )
				{
					if ( obj->liczba_mnoga )
						print_char( ch, "%s wygladaj± do¶æ podejrzanie.\n\r", capitalize( obj->short_descr ) );
					else
						print_char( ch, "%s wyglada do¶æ podejrzanie.\n\r", capitalize( obj->short_descr ) );
				}
				else
				{
					print_char( ch, "Nie widzisz w %s nic podejrzanego.\n\r", obj->name6 );
				}

				break;

			case ITEM_GEM:

				switch ( ch->class )
				{
					default: break;
					case CLASS_MAG:
					case CLASS_CLERIC:
					case CLASS_THIEF:
						 mod /=2;
						 break;
						 break;
					case CLASS_WARRIOR:
					case CLASS_PALADIN:
					case CLASS_BLACK_KNIGHT:
					case CLASS_BARBARIAN:
					case CLASS_MONK:
					case CLASS_SHAMAN:
						 break;
					case CLASS_BARD:
						 mod /= 4;
						 break;
					case CLASS_DRUID:
						 mod /= 3;
						 break;
				}

				condition = obj->condition + mod;

				if ( condition >= 98 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s s± w znakomitym stanie.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s jest w znakomitym stanie.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 80 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s maj± parê rys.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s ma parê rys.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 70 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s maj± parê rys i pêkniêæ.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s ma parê rys i pêkniêæ.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 60 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s wygl±daj± ca³kiem nie¼le.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s wygl±da ca³kiem nie¼le.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 50 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s nie wygl±daj± najgorzej.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s nie wygl±da najgorzej.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 40 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s wygl±daj± do¶æ kiepsko.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s wygl±da do¶æ kiepsko.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 30 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s maj± mnóstwo szczerb i braków.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s ma mnóstwo szczerb i braków.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 20 )
				{
					if( obj->liczba_mnoga )
						sprintf( buf, "%s za jaki¶ czas rozpadn± siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s za jaki¶ czas rozpadnie siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
				}
				else if ( condition > 10 )
				{
					if ( obj->liczba_mnoga )
						sprintf( buf, "%s rozsypuj± ci siê w rêkach.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s rozsypuje ci siê w rêkach.\n\r", capitalize_first( obj->short_descr ) );
				}
				else
				{
					if ( obj->liczba_mnoga )
						sprintf( buf, "%s zaraz rozpadn± siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
					else
						sprintf( buf, "%s zaraz rozpadnie siê na kawa³ki.\n\r", capitalize_first( obj->short_descr ) );
				}

				send_to_char( buf, ch );
				break;

			case ITEM_DRINK_CON:
			case ITEM_ENVENOMER:
			case ITEM_CONTAINER:
			case ITEM_CORPSE_NPC:
			case ITEM_CORPSE_PC:
				sprintf( buf, "do %s", argument );
				do_function( ch, &do_look, buf );
				break;
			case ITEM_PIPE:
				if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
				{
					send_to_char( "Nie mo¿esz tego sprawdziæ w takim stanie.\n\r", ch );
					break;
				}

				if ( obj->value[ 0 ] == 0 )
					print_char( ch, "%s jest {Dzgaszona{x.\n\r", capitalize( obj->short_descr ) );
				else
					print_char( ch, "%s jest {Rza{Ypa{Rlo{Yna{x.\n\r", capitalize( obj->short_descr ) );

				if ( obj->value[ 1 ] == 0 )
					print_char( ch, "Wygl±da na to, ¿e %s jest pusta.\n\r", obj->short_descr );
				else if ( obj->value[ 1 ] > 0 )
					sprintf( buf, "Widzisz, ¿e zawarto¶æ %s ma kolor %s.\n\r", obj->name2, weed_table[obj->value[2]].weed_color );
				//send_to_char( "To nie jest puste.\n\r", ch );

				send_to_char( buf, ch );
				break;
			case ITEM_BANDAGE:
				if ( obj->value[ 0 ] > 9 )
				{
					print_char( ch, "%s starczy jeszcze na bardzo d³ugo.\n\r", capitalize( obj->name2 ) );
				}
				else if ( obj->value[ 0 ] > 7 )
				{
					print_char( ch, "%s starczy jeszcze na d³ugo.\n\r", capitalize( obj->name2 ) );
				}
				else if ( obj->value[ 0 ] > 5 )
				{
					print_char( ch, "%s starczy jeszcze na do¶æ d³ugo.\n\r", capitalize( obj->name2 ) );
				}
				else if ( obj->value[ 0 ] > 3 )
				{
					print_char( ch, "%s starczy jeszcze na trochê.\n\r", capitalize( obj->name2 ) );
				}
				else if ( obj->value[ 0 ] > 1 )
				{
					print_char( ch, "%s ju¿ siê koñczy.\n\r", capitalize( obj->short_descr ) );
				}
				else if ( obj->value[ 0 ] > 0 )
				{
					print_char( ch, "%s zosta³o ju¿ tylko na jeden raz.\n\r", capitalize( obj->name2 ) );
				}
				break;
			case ITEM_NOTEPAPER:
				if ( obj->value[ 0 ] == 0 )
				{
					send_to_char( "\n\rZauwa¿asz, ¿e nie jest tu nic zapisane.\n\r", ch );
				}
				break;
			case ITEM_TOOL:
				switch ( obj->value[2] )
				{
				case TOOL_ANVIL:
					print_char( ch, "Typ narzêdzia to kowad³o.\n\r" );
					break;
				case TOOL_HAMMER:
					print_char( ch, "Typ narzêdzia to m³ot.\n\r" );
					break;
				case TOOL_PICKAXE:
					print_char( ch, "Typ narzêdzia to kilof.\n\r" );
					break;
				default:
					print_char( ch, "Jest to narzêdzie nieokre¶lonego typu.\n\r" );
					break;
				}
				switch( obj->value[0] )
				{
				case 0:
					print_char( ch, "Narzêdzie jest pod³ej jako¶ci.\n\r" );
					break;
				case 1:
					print_char( ch, "To narzêdzie jest do¶æ toporne.\n\r" );
					break;
				case 2:
					print_char( ch, "To narzêdzie nie wygl±da na najwygodniejsze.\n\r" );
					break;
				case 3:
					print_char( ch, "To narzêdzie wygl±da na takie do wszystkiego i niczego.\n\r" );
					break;
				case 4:
					print_char( ch, "To narzêdzie wygl±da jakby czego¶ mu brakowa³o.\n\r" );
					break;
				case 5:
					print_char( ch, "To narzêdzie wygl±da jak twór szalonego wynalazcy.\n\r" );
					break;
				case 6:
					print_char( ch, "To narzêdzie wyl±da na do¶æ pomocne w pracy.\n\r" );
					break;
				case 7:
					print_char( ch, "To narzêdzie jest bez w±tpienia u¿yteczne.\n\r" );
					break;
				case 8:
					print_char( ch, "Wygl±da, ¿e to narzêdzie dobrze spe³ni funkcjê do której go stworzono.\n\r" );
					break;
				case 9:
				case 10:
					print_char( ch, "To najwy¿szej jako¶ci narzêdzie.\n\r" );
					break;
				}
				if ( obj->condition > 90 )
				{
					print_char( ch, "Narzêdzie jest w doskona³ej kondycji.\n\r" );
				} else if ( obj->condition > 80 ) {
					print_char( ch, "Narzêdzie jest w dobrej kondycji.\n\r" );
				} else if ( obj->condition > 70 ) {
					print_char( ch, "Narzêdzie jest w ¶redniej kondycji.\n\r" );
				} else if ( obj->condition > 60 ) {
					print_char( ch, "Na narzêdziu widaæ ¶lady zu¿ycia.\n\r" );
				} else if ( obj->condition > 50 ) {
					print_char( ch, "Narzêdzie jest mocno zu¿yte.\n\r" );
				} else if ( obj->condition > 40 ) {
					print_char( ch, "Temu narzêdziu przyda³aby siê ju¿ naprawa.\n\r" );
				} else if ( obj->condition > 30 ) {
					print_char( ch, "To narzêdzie prosi siê o naprawê.\n\r" );
				} else if ( obj->condition > 20 ) {
					print_char( ch, "Jedyne do czego to narzêdzie siê nadaje to do naprawy.\n\r" );
				} else if ( obj->condition > 10 ) {
					print_char( ch, "To narzêdzie jest tak zu¿yte, ¿e zaraz siê rozpadnie.\n\r" );
				} else {
					print_char( ch, "Nie wiadomo jakim cudem to siê jeszcze trzyma kupy.\n\r" );
				}
				break;
		}

		if ( affect_find( obj->affected, gsn_chill_metal ) )
		{
			print_char( ch, "%s pokrywa gruba warstwa szronu.\n\r", capitalize( obj->short_descr ) );
		}

		if ( affect_find( obj->affected, gsn_heat_metal ) )
		{
			if ( obj->liczba_mnoga )
				print_char( ch, "%s ¶wiec± g³êbok± czerwieni± od wysokiej temperatury.\n\r", capitalize( obj->short_descr ) );
			else
				print_char( ch, "%s ¶wieci g³êbok± czerwieni± od wysokiej temperatury.\n\r", capitalize( obj->short_descr ) );
		}
	}
	return ;
}



/*
 * Thanks to Zrin for auto-exit part.
 */

void do_exits( CHAR_DATA *ch, char *argument )
{
	extern char * const dir_name[];
	ROOM_INDEX_DATA * in_room;
	char buf[ MAX_STRING_LENGTH ];
	char exit[ MAX_STRING_LENGTH ];
	EXIT_DATA *pexit;
	bool found;
	bool fAuto;
	int door;
	const char *dir_long_name[] =  { "pó³noc", "wschód", "po³udnie", "zachód", "góra", "dó³" };

	if( IS_NPC(ch)) return;//mobom dziêkujemy

	fAuto = !str_cmp( argument, "auto" );

	if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
		in_room = ch->pcdata->mind_in;
	else
	{
		if ( !check_blind( ch ) )
			return ;

		in_room = ch->in_room;
	}

	if ( fAuto )
    {
		sprintf( buf, "{c[Wyj¶cia:" );
    }
	else if ( IS_IMMORTAL( ch ) )
    {
		sprintf( buf, "Widoczne wyj¶cia z lokacji %d:\n\r", in_room->vnum );
    }
	else
    {
		sprintf( buf, "Widoczne wyj¶cia:\n\r" );
    }

	found = FALSE;
	for ( door = 0; door <= 5; door++ )
	{
		if ( ( pexit = in_room->exit[ door ] ) != NULL
				&& pexit->u1.to_room != NULL
				&& can_see_room( ch, pexit->u1.to_room )
				&& (
					(
					 (!( IS_SET( pexit->exit_info, EX_SECRET ) && !IS_SET( pexit->exit_info, EX_HIDDEN ) )
					  && !( IS_SET( pexit->exit_info, EX_HIDDEN ) && !IS_AFFECTED( ch, AFF_DETECT_HIDDEN ) ) )
					 || IS_IMMORTAL(ch)
					)
					|| EXT_IS_SET( ch->act, PLR_HOLYLIGHT )
				   )
		   )
		{
            if ( pexit->vName && pexit->vName[ 0 ] != '\0' )
            {
                sprintf( exit, "%s", pexit->vName );
            }
            else
            {
                if ( IS_AFFECTED( ch, AFF_MAZE ) && !IS_IMMORTAL( ch ) )
                {
                    sprintf ( exit, "%s", dir_long_name[ number_range( 0, 5 ) ] );
                }
                else
                {
                    sprintf( exit, "%s", dir_name[ door ] );
                }
            }

			found = TRUE;
			if ( fAuto )
			{
				strcat( buf, " " );
				// w³±czamy kolorki dla sekretów i przej¶æ ukrytych
				if (IS_SET( pexit->exit_info, EX_SECRET ))
				{
					strcat( buf, "{G" );
				}
				else if (IS_SET( pexit->exit_info, EX_HIDDEN ))
				{
					strcat( buf, "{B" );
				}
				//
				if ( !IS_SET( pexit->exit_info, EX_CLOSED ) )
				{
					strcat( buf, exit );
					strcat( buf, "{c" );
				}
				else if ( !IS_SET( pexit->exit_info, EX_HIDDEN ) )
				{
					strcat( buf, "(" );
					strcat( buf, exit );
					strcat( buf, ")" );
				}
				else
				{
					strcat( buf, "<" );
					strcat( buf, exit );
					strcat( buf, ">" );
				}
                // importalom z odpowiedni± opcj± pokazujemy vnumy
                if (IS_IMMORTAL( ch ))
                {
                    if ( pexit->vName && pexit->vName[ 0 ] != '\0' )
                    {
                        sprintf( buf + strlen( buf ), " {m{{%s}", dir_name[ door ] );
                    }
                    if(IS_SET( ch->wiz_config, WIZCFG_SHOWVNUMS ))
                    {
                        sprintf( buf + strlen( buf ), " {D{{%d}", pexit->u1.to_room->vnum );
                    }
                }
				// na wszelki wypadek wracamy do koloru wyj¶æ
				strcat( buf, "{c" );
			}
			else
			{
				sprintf( buf + strlen( buf ), "%-5s - %s",
						capitalize( exit ),
						IS_SET( pexit->exit_info, EX_CLOSED ) && !IS_AFFECTED( ch, AFF_PIERCING_SIGHT )
						? "Niczego nie widzisz przez zamkniête drzwi"
						: ( room_is_dark(ch, pexit->u1.to_room ) && !EXT_IS_SET( ch->act, PLR_HOLYLIGHT )
							? "Za ciemno, ¿eby mo¿na by³o co¶ powiedzieæ"
							: IS_SET( pexit->exit_info, EX_WALL_OF_MIST ) && !IS_AFFECTED( ch, AFF_PIERCING_SIGHT )
							? "Chmura mg³y zas³ania ci widoczno¶æ"
							: pexit->u1.to_room->name ) );
				if ( IS_IMMORTAL( ch ) )
				{
					sprintf( buf + strlen( buf ), " (room %d)\n\r", pexit->u1.to_room->vnum );
				}
				else
				{
					sprintf( buf + strlen( buf ), "\n\r" );
				}
			}
		}
	}

	if ( !found )
    {
		strcat( buf, fAuto ? " brak" : "Brak.\n\r" );
    }

	if ( fAuto )
    {
		strcat( buf, "]{x\n\r" );
    }

	send_to_char( buf, ch );
	return ;
}

void do_score( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	char center[ MAX_STRING_LENGTH ];
	BUFFER *buffer;

	if( IS_NPC(ch)) return;//mobom dziêkujemy

	if ( ch->position == POS_SLEEPING )
	{
		send_to_char( "¦ni ci siê, ¿e siê nad sob± zastanawiasz.\n\r", ch );
		return ;
	}

	buffer = new_buf();

	/* start */
	add_buf( buffer, "\n\r{b==<>==<>==<>==<>==<>==<>==<>==< {GKarta  Postaci {b>==<>==<>==<>==<>==<>==<>==<>=={x\n\r\n\r" );

	/* nazwa, clasa, rasa i p³eæ */
	if ( IS_NPC( ch ) )
	{
		sprintf( buf, "{gZw± ciê {G%s{g, %s %s.",
				ch->short_descr, "NPC",
				ch->sex == 0 ? "bez p³ci" : ch->sex == 1 ? "mê¿czyzna" : "kobieta" );
		center_text( buf, center, 79 );
		center[ MAX_INPUT_LENGTH - 1 ] = '\0';
		add_buf( buffer, center );
	}
	else
	{
		sprintf( buf, "{gZw± ciê {G%s{g, %s, %s %s.{x",
				ch->name,
				get_class_name(ch),
				race_table[ GET_RACE( ch ) ].name,
				ch->sex == 0 ? "bez p³ci" : ch->sex == 1 ? "mê¿czyzna" : "kobieta" );
		center_text( buf, center, 79 );
		center[ MAX_INPUT_LENGTH - 1 ] = '\0';
		add_buf( buffer, center );

		/* UNREGISTERED */
		if ( !IS_NPC( ch ) && EXT_IS_SET( ch->act, PLR_UNREGISTERED ) )
		{
			sprintf( buf, "{RTwoje imiê nie zosta³o jeszcze zaakceptowane.{x" );
			center_text( buf, center, 79 );
			center[ MAX_INPUT_LENGTH - 1 ] = '\0';
			add_buf( buffer, center );
		}
	}

	/* klan i ranga w klanie*/
	if ( !IS_NPC( ch ) && ch->pcdata->clan )
	{
		sprintf( buf, "{gNale¿ysz do klanu {G%s{g, masz stopieñ {G%s{g.{x",
				capitalize( ch->pcdata->clan->name ), ch->pcdata->clan->rank_names[ ch->pcdata->clan_rank ] );
		center_text( buf, center, 79 );
		center[ MAX_INPUT_LENGTH - 1 ] = '\0';
		add_buf( buffer, center );
	}

	/* wiek i poziom */
    if ( ch->level < 11 )
    {
        sprintf( buf, "{g¦wiat nazywa ciê prosto: {G%s{x", level_info_common[ ch->level - 1 ] );
    }
    else if ( ch->level < LEVEL_HERO )
    {
        bool add_level = TRUE;
        switch ( ch->class )
        {
            case CLASS_MAG:
                sprintf( buf, "{g¦wiat zwie takich jak ty: {G%s{x", level_info_mag[ ch->level - 11 ] );
                break;
            case CLASS_CLERIC:
                sprintf( buf, "{g¦wiat zwie takich jak ty: {G%s{x", level_info_cleric[ ch->level - 11 ] );
                break;
            case CLASS_THIEF:
                sprintf( buf, "{g¦wiat zwie takich jak ty: {G%s{x", level_info_thief[ ch->level - 11 ] );
                break;
            case CLASS_WARRIOR:
                sprintf( buf, "{g¦wiat zwie takich jak ty: {G%s{x", level_info_warrior[ ch->level - 11 ] );
                break;
            case CLASS_PALADIN:
                sprintf( buf, "{g¦wiat zwie takich jak ty: {G%s{x", level_info_paladin[ ch->level - 11 ] );
                break;
            case CLASS_DRUID:
                sprintf( buf, "{g¦wiat zwie takich jak ty: {G%s{x", level_info_druid[ ch->level - 11 ] );
                break;
            case CLASS_BARBARIAN:
                sprintf( buf, "{g¦wiat zwie takich jak ty: {G%s{x", level_info_barbarian[ ch->level - 11 ] );
                break;
            case CLASS_MONK:
                sprintf( buf, "{g¦wiat zwie takich jak ty: {G%s{x", level_info_monk[ ch->level - 11 ] );
                break;
            case CLASS_BARD:
                sprintf( buf, "{g¦wiat zwie takich jak ty: {G%s{x", level_info_bard[ ch->level - 11 ] );
                break;
            case CLASS_BLACK_KNIGHT:
                sprintf( buf, "{g¦wiat zwie takich jak ty: {G%s{x", level_info_black_knight[ ch->level - 11 ] );
                break;
            case CLASS_SHAMAN:
                sprintf( buf, "{g¦wiat zwie takich jak ty: {G%s{x", level_info_shaman[ ch->level - 11 ] );
                break;
            default:
                log_string( "cos nie tak w do_score, w switchu od info o klasie" );
                add_level = FALSE;
        }
        if ( add_level ) {
            center_text( buf, center, 79 );
            center[ MAX_INPUT_LENGTH - 1 ] = '\0';
            add_buf( buffer, center );
        }
    }
    /**
     * wiek i/lub poziom
     */
    if ( ch->level > LEVEL_HERO )
    {
        sprintf
            (
             buf,
             "{gMasz {G%d{g lat i osi±gn%s¶ {G%d{g poziom.{x",
             get_age( ch ),
             ch->sex == 0 ? "ê³o" : ch->sex == 2 ? "ê³a" : "±³e",
             ch->level
            );
    }
    else
    {
        sprintf
            (
             buf,
             "{gMasz {G%d{g lat.{x",
             get_age( ch )
            );
    }
	center_text( buf, center, 79 );
	center[ MAX_INPUT_LENGTH - 1 ] = '\0';
	add_buf( buffer, center );

	/* wzrost i waga, ale dopirto powy¿ej 1 poziomu */
	if ( ch->level > 1 )
	{
		sprintf( buf, "{gMasz {G%d{g cm wzrostu i wa¿ysz {G%d.%d{g kg.{x", ch->height, ch->weight/10, ch->weight%10 );
		center_text( buf, center, 79 );
		center[ MAX_INPUT_LENGTH - 1 ] = '\0';
		add_buf( buffer, center );
	}

	/* punkty ¿ycia i punkty ruchu                Hp: 30 z 30 <>==<> Mv: 100 ze 100 */
	add_buf( buffer, "\n\r" );
	//Raszer mv_undead
    sprintf( buf, "{gPunkty ¿ycia: {G%d{g z {G%d{b <>==<> {gPunkty ruchu: {G%d{g z {G%d{g.",
			ch->hit, get_max_hp(ch), ch->move, ch->max_move );
			//przedostatnie bylo uzaleznione od is_undead(ch) ? ch->max_move : ch -> move
	center_text( buf, center, 79 );
	center[ MAX_INPUT_LENGTH - 1 ] = '\0';
	add_buf( buffer, center );

    // czy czego¶ dosiada
    if ( ch->mount )
    {
        sprintf( buf, "{gAktualnie dosiadasz {G%s{g.", ch->mount->name4 );
		center_text( buf, center, 79 );
		center[ MAX_INPUT_LENGTH - 1 ] = '\0';
		add_buf( buffer, center );
    }

	/* kasa przy sobie */
    if ( ch->copper > 0 || ch->silver > 0 || ch->gold > 0 || ch->mithril > 0 )
    {
        add_buf( buffer, "\n\r" );
        sprintf
            (
             buf,
             "Noszone monety: %ldc %lds %ldg %ldm",
             ch->copper,
             ch->silver,
             ch->gold,
             ch->mithril
            );
        center_text( buf, center, 79 );
        center[ MAX_INPUT_LENGTH - 1 ] = '\0';
        add_buf( buffer, center );
    }

	/* kasa w banku */
    if ( ch->bank > 0 )
    {
        sprintf
            (
             buf,
             "Pieniadze w banku: %ldc %lds %ldg %ldm",
             money_copper_from_copper ( ch->bank ),
             money_silver_from_copper ( ch->bank ),
             money_gold_from_copper   ( ch->bank ),
             money_mithril_from_copper( ch->bank )
            );
        center_text( buf, center, 79 );
        center[ MAX_INPUT_LENGTH - 1 ] = '\0';
        add_buf( buffer, center );
	}

    /**
     * wyswietlanie informacji o doswiadczeniu
     */
    if ( ch->level < LEVEL_HERO )
    {
        add_buf( buffer, "\n\r" );
        switch ( get_percent_exp( ch ) )
        {
            case 10:
            case 9:
                sprintf( buf, "{wDo zdobycia nastepnego poziomu jeszcze bardzo dluga droga przed toba.{x");
                break;
            case 8:
                sprintf( buf, "{wJestes nadal daleko od nastepnego poziomu.{x");
                break;
            case 7:
                sprintf( buf, "{bDrobne kroki do nastepnego poziomu wydaja sie byc dobre na poczatek.{x");
                break;
            case 6:
                sprintf( buf, "{bZblizasz sie do polowy drogi do nastepnego poziomu.{x");
                break;
            case 5:
                sprintf( buf, "{mJestes mniej wiecej w polowie drogi do nastepnego poziomu.{x");
                break;
            case 4:
                sprintf( buf, "{mJestes blizej nastepnego poziomu niz dalej.{x");
                break;
            case 3:
                sprintf( buf, "{rNiezbyt duzo przed toba, jesli myslisz o nastepnym poziomie.{x");
                break;
            case 2:
                sprintf( buf, "{rDo nastepnego poziomu masz naprawde niedaleko.{x");
                break;
            case 1:
            case 0:
                sprintf( buf, "{RW kazdej chwili mozesz zdobyc poziom.{x");
                break;
        }
        center_text( buf, center, 79 );
        center[ MAX_INPUT_LENGTH - 1 ] = '\0';
        add_buf( buffer, center );
    }

	/* glory points */
	//Brohacz: zakomentowanie glory points, poki co nieuzywane
	/*
	if (ch->glory < -15)
		sprintf( buf, "{gZosta³%s¶ splamion%s ogromn± hañb±.{x", ch->sex == 2 ? "a" : "e" , ch->sex == 2 ? "a" : "y" );
	else if (ch->glory < -10 && ch->glory >= -15)
	        sprintf( buf, "{gJeste¶ pogardzany przez wiêkszo¶æ osób.{x" );
	else if (ch->glory < -3 && ch->glory >= -10)
		sprintf( buf, "{gNiektórzy nieszanuj± twojej osoby.{x" );
	else if (ch->glory <= 3  && ch->glory >= -3)
		sprintf( buf, "{gJeste¶ przeciêtnym zajadaczem chleba.{x" );
	else if (ch->glory > 3 && ch->glory <= 10)
		sprintf( buf, "{gPowoli zdobywasz uznanie w oczach innych.{x" );
	else if (ch->glory > 10  && ch->glory <= 15)
		sprintf( buf, "{gJeste¶ osob±, która jest szanowana w spo³eczeñstwie.{x" );
	else if (ch->glory > 15)
		sprintf( buf, "{gJeste¶ darzon%s olbrzymim respektem.{x", ch->sex == 2 ? "a" : "y" );

	center_text( buf, center, 79 );
	center[ MAX_INPUT_LENGTH - 1 ] = '\0';
	add_buf( buffer, center );
	*/
	/* koniec dla gracza */
	add_buf( buffer, "\n\r{b==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x\n\r" );
	/* pcz±tek dla immo */
	if ( ch->level > LEVEL_HERO )
	{
		add_buf( buffer, "\n\r{b==<>==<>==<>==<>==<>==<>==< {RDane Nie¶miertelnego{b =<>==<>==<>==<>==<>==<>==<>=={x\n\r\n\r" );
		/* ilo¶æ rzeczy, razem z ich wag± */
		if ( ch->carry_number > 0 )
		{
            sprintf
                (
                 buf,
                 "{gNosisz {G%d{g/%d rzeczy w sumie wa¿±cych {G%d{g/%d funtów.{x",
                 ch->carry_number,
                 can_carry_n( ch ),
                 get_carry_weight( ch ) / 10,
                 can_carry_w( ch ) / 10
                );
			center_text( buf, center, 79 );
			center[ MAX_INPUT_LENGTH - 1 ] = '\0';
			add_buf( buffer, center );
		}
		/* pancerz */
		sprintf( buf, "{gAC: pierce[ {G%-4d{g] bash[ {G%-4d{g] slash[ {G%-4d{g] magic[ {G%-4d{g]{x",
				GET_AC( ch, AC_PIERCE ),
				GET_AC( ch, AC_BASH ),
				GET_AC( ch, AC_SLASH ),
				GET_AC( ch, AC_EXOTIC )
		       );
		center_text( buf, center, 79 );
		center[ MAX_INPUT_LENGTH - 1 ] = '\0';
		add_buf( buffer, center );
		/* thaco */
		sprintf( buf, "{gTHAC0: [ {G%-3d{g] TOHIT: [ {G%-2d{g] TODAM: [ {G%-2d{g]{x",
				compute_tohit( ch, TRUE ),
				GET_HITROLL( ch ),
				GET_DAMROLL( ch, NULL )
		       );
		center_text( buf, center, 79 );
		center[ MAX_INPUT_LENGTH - 1 ] = '\0';
		add_buf( buffer, center );
		/* STATS */
		sprintf( buf, "{gStr: {G%-2d{g  Int: {G%-2d{g  Wis: {G%-2d{g  Dex: {G%-2d{g  Con: {G%-2d{x  {gCha: {G%-2d{g  Luc: {G%-2d{x",
				get_curr_stat( ch, STAT_STR ),
				get_curr_stat( ch, STAT_INT ),
				get_curr_stat( ch, STAT_WIS ),
				get_curr_stat( ch, STAT_DEX ),
				get_curr_stat( ch, STAT_CON ),
				get_curr_stat( ch, STAT_CHA ),
				get_curr_stat( ch, STAT_LUC )
		       );
		center_text( buf, center, 79 );
		center[ MAX_INPUT_LENGTH - 1 ] = '\0';
		add_buf( buffer, center );
		/* HOLYLIGHT */
		if ( EXT_IS_SET( ch->act, PLR_HOLYLIGHT ) )
		{
			sprintf( buf, "{gHOLYLIGHT jest {Gw³±czone{g.{x" );
			center_text( buf, center, 79 );
			center[ MAX_INPUT_LENGTH - 1 ] = '\0';
			add_buf( buffer, center );
		}
		/* incognito */
		if ( ch->incog_level )
		{
			sprintf( buf, "{gMasz w³±czone incognito do poziomu {G%d{g.{x", ch->incog_level );
			center_text( buf, center, 79 );
			center[ MAX_INPUT_LENGTH - 1 ] = '\0';
			add_buf( buffer, center );
		}
		/* invis */
		if ( ch->invis_level )
		{
			sprintf( buf, "{gMasz w³±czonego wizinvisa do poziomu: {G%d{g.{x", ch->invis_level );
			center_text( buf, center, 79 );
			center[ MAX_INPUT_LENGTH - 1 ] = '\0';
			add_buf( buffer, center );
		}
		add_buf( buffer, "\n\r{b==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>==<>=={x\n\r" );
	}

	if ( buffer )
	{
		page_to_char( buf_string( buffer ), ch );
		free_buf( buffer );
	}
}


void do_affects( CHAR_DATA *ch, char *argument )
{

	AFFECT_DATA * paf, *paf_last = NULL;
	char buf[ MAX_STRING_LENGTH ];
	char buf_noinfo_bad[ MAX_STRING_LENGTH ];
	char buf_info_bad[ MAX_STRING_LENGTH ];
	char buf_noinfo_ok[ MAX_STRING_LENGTH ];
	char buf_info_ok[ MAX_STRING_LENGTH ];
	char buf_reserved[ MAX_STRING_LENGTH ];
	int mirrors = 0;

	if( IS_NPC(ch)) return;//mobom dziêkujemy

	buf[ 0 ] = buf_noinfo_bad[ 0 ] = buf_info_bad[ 0 ] = buf_noinfo_ok[ 0 ] = buf_info_ok[ 0 ] = buf_reserved[ 0 ] = '\0';

	if ( ch->position == POS_SLEEPING )
	{
		send_to_char( "¦ni ci siê, ¿e jeste¶ pod wp³ywem potê¿nych czarów.\n\r", ch );
		return ;
	}

	if ( ch->affected == NULL )
	{
		send_to_char( "Wydaje siê, ¿e jest z tob± wszystko w porz±dku.\n\r", ch );
		return ;
	}

	for ( paf = ch->affected; paf != NULL; paf = paf->next )
	{
		if ( !paf->visible )
			continue;

		if ( !paf_last ||
				paf_last->type != paf->type ||
				( paf_last->info && paf->info && str_cmp( paf->info, paf_last->info ) ) )
		{

			if ( paf->type == gsn_mirror_image )
				mirrors = paf->modifier;

			if ( paf->type == gsn_malnutrition || paf->type == gsn_dehydration || paf->type == gsn_bleeding_wound || paf->type == gsn_bleed || paf->type == gsn_thigh_jab )
			{
				sprintf( buf_reserved + strlen( buf_reserved ), "%s\n\r",//tu bez kropki, bo sa wykrzykniki
						capitalize_first( skill_table[ paf->type ].affect_desc ) );
			}
			else if ( paf->type && skill_table[ paf->type ].affect_desc )
			{
				if ( skill_table[ paf->type ].target == TAR_CHAR_OFFENSIVE
						|| skill_table[ paf->type ].target == TAR_OBJ_CHAR_OFF )
				{
					if ( IS_NPC( ch ) || ( !IS_NPC( ch ) && class_table[ ch->class ].caster >= 0 ) )
						sprintf( buf_info_bad + strlen( buf_info_bad ), "{R%s{x - %s.\n\r",
								skill_table[ paf->type ].name,
								skill_table[ paf->type ].affect_desc );
					else
						sprintf( buf_info_bad + strlen( buf_info_bad ), "{R%s.{x\n\r",
								capitalize_first( skill_table[ paf->type ].affect_desc ) );
				}
				else
				{
					if ( IS_NPC( ch ) || ( !IS_NPC( ch ) && class_table[ ch->class ].caster >= 0 ) )
					{
						if ( paf->type == gsn_mirror_image )
							sprintf( buf_info_ok + strlen( buf_info_ok ), "{G%s ({x{Y%d{x{G){x - %s.\n\r",
									skill_table[ paf->type ].name,
									mirrors,
									capitalize_first( skill_table[ paf->type ].affect_desc ) );
						else if ( paf->type == gsn_mental_barrier )
							sprintf( buf_info_ok + strlen( buf_info_ok ), "{G%s{x - %s '%s'.\n\r",
									skill_table[ paf->type ].name,
									capitalize_first( skill_table[ paf->type ].affect_desc ),
									skill_table[ paf->modifier ].name );
						else
							sprintf( buf_info_ok + strlen( buf_info_ok ), "{G%s{x - %s.\n\r",
									skill_table[ paf->type ].name,
									capitalize_first( skill_table[ paf->type ].affect_desc ) );
					}
					else
						sprintf( buf_info_ok + strlen( buf_info_ok ), "{G%s.{x\n\r",
								capitalize_first( skill_table[ paf->type ].affect_desc ) );
				}
			}
			else if ( paf->type != 0 && !paf->info )
			{
				if ( skill_table[ paf->type ].target == TAR_CHAR_OFFENSIVE
						|| skill_table[ paf->type ].target == TAR_OBJ_CHAR_OFF )
				{
					sprintf( buf_noinfo_bad + strlen( buf_noinfo_bad ), "{R%s{x\n\r",
							skill_table[ paf->type ].name );
				}
				else
				{
					sprintf( buf_noinfo_ok + strlen( buf_noinfo_ok ), "{G%s{x\n\r",
							skill_table[ paf->type ].name );
				}
			}
			else if ( paf->type != 0 && paf->info ) //czar jest w tabeli const.c, ale nie ma affect_desc, a ma info
			{
				//je¶li na poczatku info jest -, efekt z³y, + efekt dobry
				if ( paf->info[0] == '-' )
				{
					if ( IS_NPC( ch ) || ( !IS_NPC( ch ) && class_table[ ch->class ].caster >= 0 ) )
						sprintf( buf_info_bad + strlen( buf_info_bad ), "{R%s{x - %s.\n\r",
								skill_table[ paf->type ].name,
								paf->info + 1 );
					else
						sprintf( buf_info_bad + strlen( buf_info_bad ), "{R%s.{x\n\r",
								capitalize_first( paf->info + 1 ) );
				}
				else if ( paf->info[0] == '+' )
				{
					if ( IS_NPC( ch ) || ( !IS_NPC( ch ) && class_table[ ch->class ].caster >= 0 ) )
					{
						sprintf( buf_info_ok + strlen( buf_info_ok ), "{G%s{x - %s.\n\r",
								skill_table[ paf->type ].name,
								capitalize_first( paf->info + 1 ) );
					}
					else
						sprintf( buf_info_ok + strlen( buf_info_ok ), "{G%s.{x\n\r",
								capitalize_first( paf->info + 1 ) );
				}
			}
			else if ( paf->type == 0 && paf->info )
			{
				sprintf( buf_reserved + strlen( buf_reserved ), "%s.\n\r",
						capitalize_first( paf->info ) );
			}
			//sprintf(buf, "%-15s", paf->info ? paf->info : skill_table[paf->type].name);
		}
		else
			continue;

		/*if (IS_IMMORTAL(ch))
		  {
		  switch(paf->location)
		  {
		  case 26:
		  sprintf(buf, ": modyfikuje umiejêtno¶æ %s o %d ",
		  skill_table[paf->modifier].name, paf->level);
		  break;
		  case 27:
		  sprintf(buf, ": modyfikuje ilo¶æ czarów na kr±g %d o %d ",
		  paf->modifier, paf->level);
		  break;
		  case 28:
		  sprintf(buf, ": modyfikuje jêzyk %s o %d ",
		  lang_table[paf->modifier].name, paf->level);
		  break;
		  default:sprintf(buf,": modyfikuje %s o %d ",
		  affect_loc_name(paf->location),paf->modifier);
		  break;
		  }
		  send_to_char(buf, ch);

		  if (paf->duration == -1)
		  sprintf(buf, "na sta³e.\n\r");
		  else
		  sprintf(buf, "na %d %s.\n\r", paf->duration, hour_to_str( paf->duration ) );

		  send_to_char(buf, ch);
		  }
		  else
		//send_to_char("\n\r", ch);*/
		paf_last = paf;

	}

	if ( buf_noinfo_bad[ 0 ] != '\0' || buf_info_bad[ 0 ] != '\0'
			|| buf_noinfo_ok[ 0 ] != '\0' || buf_info_ok[ 0 ] != '\0' )
		send_to_char( "Oto co siê z tob± dzieje:\n\r", ch );
	else if ( buf_reserved[ 0 ] != '\0' )
	{
		send_to_char( "Oto co siê z tob± dzieje:\n\r", ch );
		send_to_char( buf_reserved, ch );
		return ;
	}
	else
	{
		send_to_char( "Wydaje siê, ¿e jest z tob± wszystko w porz±dku.\n\r", ch );
		return ;
	}
	send_to_char( buf_noinfo_bad, ch );
	send_to_char( buf_info_bad, ch );
	send_to_char( buf_noinfo_ok, ch );
	send_to_char( buf_info_ok, ch );
	if ( buf_reserved[ 0 ] != '\0' )
		send_to_char( "\n\rDodatkowo:\n\r", ch );
	send_to_char( buf_reserved, ch );
	return ;
}



char *	const	day_name	[] =
{
	"£owów",
	"Podzia³u",
	"Zbiorów",
	"Wymiany",
	"Mod³ów",
	"Narad",
	"Przyjació³"
};

char *	const	month_name	[] =
{
	"Najd³u¿szej Nocy",
	"Samotnego ¦nie¿nego Wilka",
	"Lodowych Gigantów",
	"Pradawnych Si³",
	"Wielkich Zmagañ",
	"Wiosennego Brzasku",
	"Matki Natury",
	"Pró¿nych £owów",
	"Smoka",
	"S³oñca",
	"Letniego ¯aru",
	"Walki",
	"Poleg³ych",
	"Szarych Pól",
	"D³ugich Cieni",
	"Odwiecznego Mroku",
	"Wielkiego Z³a"
};


char * const hour_name [] =
{
	"pó³noc",
	"godzina pierwsza",
	"godzina druga",
	"godzina trzecia",
	"godzina czwarta",
	"godzina pi±ta",
	"godzina szósta",
	"godzina siódma",
	"godzina ósma",
	"godzina dziewi±ta",
	"godzina dziesi±ta",
	"godzina jedenasta",
	"po³udnie",
	"godzina trzynasta",
	"godzina czternasta",
	"godzina piêtnasta",
	"godzina szesnasta",
	"godzina siedemnasta",
	"godzina osiemnasta",
	"godzina dziewiêtnasta",
	"godzina dwudziesta",
	"godzina dwudziesta pierwsza",
	"godzina dwudziesta druga",
	"godzina dwudziesta trzecia",
};

void do_time( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];
    int day;

    if( IS_NPC(ch)) return;//mobom dziêkujemy

    day = time_info.day + 1;

    sprintf( buf,
            "Jest %s.\n\rDzieñ %s, %d miesi±ca %s.\n\rMamy rok %d Ery Niestrawno¶ci.\n\r",
            hour_name[ time_info.hour ],
            day_name[ day % 7 ],
            day,
            month_name[ time_info.month ],
            time_info.year );
    send_to_char( buf, ch );
    return ;
}

void do_evcheck(CHAR_DATA *ch, char *argument)
{
	wait_printf(ch,1,"No to testniemy te ca³e opó¼nione komendy...\n\r");
	wait_printf(ch,6,"To siê powinno pojawiæ 6 sekund pó¼niej...\n\r");
	wait_function(ch,9,&do_who,"");
	return;
}

void do_mudinfo( CHAR_DATA *ch, char *argument )
{
    extern time_t str_boot_time;
    char buf[ MAX_STRING_LENGTH ];
    char date[ MAX_STRING_LENGTH ];
    char year[ 10 ];
    char hour[ 20 ];
    struct tm *btime;

    if( IS_NPC(ch)) return;//mobom dziêkujemy

    btime = localtime( &str_boot_time );
    strftime( year, 10, "%Y", btime );
    strftime( hour, 20, "%H:%M:%S", btime );
    sprintf( date, "%s %s, %d %s %s o godzinie %s",
            btime->tm_wday == 2 ? "we" : "w",
            real_day_names[ btime->tm_wday ][ 1 ],
            btime->tm_mday,
            real_month_names[ btime->tm_mon ][ 1 ],
            year,
            hour );

    sprintf( buf, "MUD wystartowa³ %s.\n\r", date );
    send_to_char( buf, ch );

    btime = localtime( &current_time );
    strftime( year, 10, "%Y", btime );
    strftime( hour, 20, "%H:%M:%S", btime );
    sprintf( date, "%s, %d %s %s, godzina %s",
            real_day_names[ btime->tm_wday ][ 0 ],
            btime->tm_mday,
            real_month_names[ btime->tm_mon ][ 1 ],
            year,
            hour );

    sprintf( buf, "Czas systemowy to %s.\n\r", date );
    send_to_char( buf, ch );


    if ( misc.copyover_delayed > 0 )
        btime = localtime( &misc.copyover_delayed );
    else if ( misc.copyover_scheduled > 0 )
        btime = localtime( &misc.copyover_scheduled );

    strftime( year, 10, "%Y", btime );
    strftime( hour, 20, "%H:%M:%S", btime );
    sprintf( date, "%s %s, %d %s %s o godzinie %s",
            btime->tm_wday == 2 ? "we" : "w",
            real_day_names[ btime->tm_wday ][ 1 ],
            btime->tm_mday,
            real_month_names[ btime->tm_mon ][ 1 ],
            year,
            hour );

    if ( misc.copyover_delayed > 0 || misc.copyover_scheduled > 0 )
    {
        sprintf( buf, "Prze³adowanie ¶wiata %s.\n\r", date );
        send_to_char( buf, ch );
    }

    print_char( ch, "\n\rStrona internetowa naszego muda: %s\n\r"
            "Blog muda: %s\n\r"
            "Forum dyskusyjne muda: %s\n\r"
            "Zapraszamy!\n\r", WWW_ADDRESS, WWW_BLOG, FORUM_ADDRESS );
    return ;
}

void do_weather( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];
	ROOM_INDEX_DATA * in_room;

	if( IS_NPC(ch)) return;//mobom dziêkujemy

	static char * const sky_look[ 4 ] =
	{
		"bezchmurne",
		"pochmurne",
		"deszczowe",
		"roz¶wietlane przez pioruny"
	};

	if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
		in_room = ch->pcdata->mind_in;
	else
	{
		if ( !check_blind( ch ) )
			return ;

		in_room = ch->in_room;
	}

	if ( !IS_OUTSIDE( ch )
			|| IS_SET( sector_table[ in_room->sector_type ].flag, SECT_NOWEATHER ) )
	{
		send_to_char( "Nie widaæ nieba.\n\r", ch );
		return ;
	}

	sprintf( buf, "Niebo jest %s i %s.\n\r",
			sky_look[ weather_info[ in_room->sector_type ].sky ],
			weather_info[ in_room->sector_type ].change >= 0
			? "wieje lekko ciep³y, po³udniowy wiatr"
			: "wieje mocno zimny, pó³nocny wiatr"
	       );
	send_to_char( buf, ch );
	return ;
}

void do_help( CHAR_DATA *ch, char *argument )
{
	HELP_DATA * pHelp;
	BUFFER *output;
	bool found = FALSE;
	char argall[ MAX_INPUT_LENGTH ], argone[ MAX_INPUT_LENGTH ], nohelp[MAX_STRING_LENGTH];
	int level;
	int cmd;
    int counter = 0;

	if( IS_NPC(ch)) return;//mobom dziêkujemy

	output = new_buf();

	if ( argument[ 0 ] == '\0' )
	{
		if ( !IS_NPC( ch ) && EXT_IS_SET( ch->act, PLR_COMMANDSPL ) )
			argument = "podsumowanie";
		else
			argument = "summary";
	}
	else
	{
		strcpy(nohelp, argument);
	}

	/* this parts handles help a b so that it returns help 'a b' */
	argall[ 0 ] = '\0';
	while ( argument[ 0 ] != '\0' )
	{
		argument = one_argument( argument, argone );

		if ( !IS_NPC( ch ) && EXT_IS_SET( ch->act, PLR_COMMANDSPL ) )
		{
			for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
			{
				if ( argone[ 0 ] == cmd_table[ cmd ].name_pl[ 0 ]
						&& !str_prefix( argone, cmd_table[ cmd ].name_pl ) )
				{
					sprintf( argone, "%s", cmd_table[ cmd ].name );
					break;
				}
			}

		}

		if ( argall[ 0 ] != '\0' )
			strcat( argall, " " );
		strcat( argall, argone );
	}

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
        if ( counter > 15 )
        {
            add_buf( output, "\n\r\n\r{Rwyswietlono tylko 15 pierwszych pasujacych tematow pomocy{x\n\r" );
            add_buf( output, "{Rprosze wybrac dokladniejsza fraze{x\n\r\n\r" );
            break;
        }
        level = ( pHelp->level < 0 ) ? -1 * pHelp->level - 1 : pHelp->level;

        if ( level > get_trust( ch ) )
            continue;

        if ( is_name( argall, pHelp->keyword ) )
        {
            counter++;
            /* add seperator if found */
            if ( found )
            {
                add_buf( output, "\n\r============================================================\n\r\n\r" );
            }
            if ( pHelp->level >= 0 && str_cmp( argall, "imotd" ) )
            {
                add_buf( output, pHelp->keyword );
                add_buf( output, "\n\r" );
            }

            /*
             * Strip leading '.' to allow initial blanks.
             */
            if ( pHelp->text[ 0 ] == '.' )
            {
                add_buf( output, pHelp->text + 1 );
            }
            else
            {
                add_buf( output, pHelp->text );
            }
            found = TRUE;
            /* small hack :) */

            if ( ch->desc != NULL && ch->desc->connected != CON_PLAYING )
            {
                break;
            }
        }
    }

	if ( !found )
	{
		append_file( ch, HELP_FILE, nohelp );
		send_to_char( "Brak pomocy na ten temat.\n\r", ch );
	}
	else
	{
		page_to_char( buf_string( output ), ch );
	}
	free_buf( output );
}


char* wizstr( CHAR_DATA *ch )
{
	if ( !ch || !ch->pcdata )
		return "";

	if ( ch->pcdata->wiz_conf & W6 )
		return ch->sex == 2 ? "Hrabina" : "Lord";
	else if ( ch->pcdata->wiz_conf & W5 )
		return ch->sex == 2 ? "Wyrocznia" : "Sêdzia";
	else if ( ch->pcdata->wiz_conf & W4 )
		return ch->sex == 2 ? "Regentka" : "Regent";
	else if ( ch->pcdata->wiz_conf & W3 )
		return ch->sex == 2 ? "Stra¿niczka" : "Stra¿nik";
	else if ( ch->pcdata->wiz_conf & W2 )
		return ch->sex == 2 ? "Kreatorka" : "Twórca";
	else if ( ch->pcdata->wiz_conf & W1 )
		return ch->sex == 2 ? "Nowa" : "Nowy";
	else
		return ch->sex == 2 ? "Bohaterka" : "Bohater";
}

void do_who( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_STRING_LENGTH ];
    char buf2[ MAX_STRING_LENGTH ];
    char title[ MAX_STRING_LENGTH ];
    BUFFER *output;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *wch;
    OBJ_DATA *weapon;
    AFFECT_DATA *noob_killer;
    int count = 0, prior, wiz_conf, wiz_count = 0;
    int invis = 0;
    int clan = 0;
    bool immfound = FALSE;
    bool pfound = FALSE;
    bool show_special_info = FALSE, show_stats = FALSE, show_affects = FALSE;
    char* min = "zz";
    char* actual = "00";
    time_t czas;
        bool tmp;
    char fr_info[MAX_STRING_LENGTH];
#ifdef WHO_HIDE
    bool introduced;
#endif

    if( IS_NPC(ch)) return;//mobom dziêkujemy

    /* start delayed ch delete - by Fuyara */
    if ( IS_IMMORTAL( ch ) )
    {
        if ( !str_prefix( argument, "todelete" ) )
        {
            show_todelete( ch );
            return;
        }
    }
    /* end delayed ch delete */

    if ( !IS_IMMORTAL(ch) && ch->position == POS_SLEEPING )
    {
        switch ( number_range( 1, 5 ) )
        {
            case 1:
                send_to_char( "¦nisz o wszystkich których znasz.\n\r", ch );
                break;
            case 2:
                send_to_char( "¦nisz o wszystkich których nie znasz.\n\r", ch );
                break;
            case 3:
                send_to_char( "¦ni ci siê koszmar o tym ¿e jeste¶ sam<&/a/o> na ¦wiecie.\n\r", ch );
                break;
            case 4:
                send_to_char( "¦nisz o wszystkich, ale nikogo nie widzisz.\n\r", ch );
                break;
            case 5:
                send_to_char( "¦ni ci siê wiedza o wszystkich.\n\r", ch );
                break;
        }
        return ;
    }

    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && ( ch->pcdata->wiz_conf & W4 ) && argument[ 0 ] != '\0' && !str_prefix( argument, "info" ) )
    {
        show_special_info = TRUE;
    }

    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && ( ch->pcdata->wiz_conf & W6 ) && argument[ 0 ] != '\0' && !str_prefix( argument, "stats" ) )
    {
        show_stats = TRUE;
    }

    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && ( ch->pcdata->wiz_conf & W6 ) && argument[ 0 ] != '\0' && !str_prefix( argument, "affects" ) )
    {
        show_affects = TRUE;
    }

    output = new_buf();

    /*najpierw immosi*/

    for ( prior = 0; prior <= 6; prior++ )
    {
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA * wch;
            if ( !d->character || d->connected != CON_PLAYING ) continue;

            wch = ( d->original != NULL ) ? d->original : d->character;

            if ( !IS_IMMORTAL( wch ) )
                continue;

            if ( !can_see( ch, wch ) )
                continue;

            switch ( prior )
            {
                case 0: wiz_conf = W1 + W2 + W3 + W4 + W5 + W6;break;
                case 1: wiz_conf = W1 + W2 + W3 + W4 + W5;break;
                case 2: wiz_conf = W1 + W2 + W3 + W4;break;
                case 3: wiz_conf = W1 + W2 + W3;break;
                case 4: wiz_conf = W1 + W2;break;
                case 5: wiz_conf = W1;break;
                default : wiz_conf = 0; break;
            }

            if ( wch->pcdata->wiz_conf != wiz_conf )
                continue;

            /* shapeshifting */
            if ( wch->ss_data != NULL )
                continue;


            if ( !immfound )
            {
                immfound = TRUE;
                sprintf( buf, "{cNie¶miertelni\n\r-------\n\r{x" );
                add_buf( output, buf );
            }

            if ( !d->original && wch->desc->editor > 0 && wch->desc->editor < 10 && wch->desc->editor != 8 )
            {
                sprintf( buf, "{c[ %-11.11s {RB{c ] %-1.20s %-1.35s{x", wizstr( wch ), wch->name, IS_NPC( wch ) ? "" : wch->pcdata->title );
                strcat( buf, " (buduje)" );
            }
            else if ( !d->original && wch->desc->editor == 8 )
            {
                sprintf( buf, "{c[ %-11.11s {GP{x ] %-1.20s %-1.35s{x", wizstr( wch ), wch->name, IS_NPC( wch ) ? "" : wch->pcdata->title );
                strcat( buf, " (pisze)" );
            }
            else
            {
                sprintf( buf, "{c[ %-11.11s   ] %-1.20s %-1.35s{x", wizstr( wch ), wch->name, IS_NPC( wch ) ? "" : wch->pcdata->title );
            }

            if ( IS_SET( wch->comm, COMM_AFK ) )
            {
                if ( wch->pcdata->afk_text )
                {
                    add_buf( output, buf );
                    sprintf( buf, " (afk:%-1.20s)", wch->pcdata->afk_text );
                    add_buf( output, buf );
                    buf[ 0 ] = '\0';
                }
                else
                    strcat( buf, " (afk)" );
            }

            strcat( buf, "{x\n\r" );
            add_buf( output, buf );
            wiz_count++;
        }
    }

    /* zwykli gracze */

    while ( str_cmp( min, "zzz" ) )
    {
        min = "zzz";
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( !d->character || d->connected != CON_PLAYING ) continue;

            wch = ( d->original != NULL ) ? d->original : d->character;

            /**
             * wylacz immortali
             */
            if ( IS_IMMORTAL( wch ) )
            {
                continue;
            }

            /* shapeshifting */
            if ( wch->ss_data != NULL )
                continue;

            if ( strcmp( wch->name, min ) <= 0 && strcmp( wch->name, actual ) > 0 )
            {
                min = wch->name;
            }
        }
        actual = min;

        if ( str_cmp( actual, "zzz" ) )
        {
            for ( d = descriptor_list; d != NULL; d = d->next )
            {
                if ( !d->character || d->connected != CON_PLAYING ) continue;

                wch = ( d->original != NULL ) ? d->original : d->character;

                /**
                 * wylacz immortali
                 */
                if ( IS_IMMORTAL( wch ) )
                {
                    continue;
                }

                /* shapeshifting */
                if ( wch->ss_data != NULL ) continue;
                if ( str_cmp( wch->name, actual ) ) continue;
                /* zliczanue inwisów, ciemniaków i inne hide'y */
                if ( !can_see( ch, wch ) )
                {
                    invis++;
                    continue;
                }
                fr_info[0] = '\0';
                /* zliczanie klanowiczów, w przeciwnym wypadku limitacja */
                if (ch->pcdata->clan && !IS_NPC(wch) && wch->pcdata->clan && ch->pcdata->clan->who_name == wch->pcdata->clan->who_name) {
                    clan++;
                }
                /**
                 * wlacza sie dla ukrywania na who
                 */
#ifdef WHO_HIDE
                else if
                    (
                     !IS_IMMORTAL( ch )
                     && ch != wch
                     && ch->in_room != wch->in_room
                     && ((ch->level > LEVEL_WHO_SHOW) || (wch->level > LEVEL_WHO_SHOW))
#ifdef ENABLE_SHOW_ON_WHO
                     && (!IS_SET( wch->comm, COMM_SHOW_ON_WHO ))
#endif
                     //Rysand - do czasu poprawienia b³êdu wywalam ten ficzer
                     && !clans_in_aliance(ch->pcdata->clan, wch->pcdata->clan) //widoczno¶æ dla klanów bêd±cych w sojuszu
                    )
                    {
                    introduced = FALSE;
                    if ( !friend_exist( ch, wch->name, NULL, fr_info, &czas, &introduced )
                            || !friend_exist( wch, ch->name, NULL, NULL, &czas, &tmp ) )
                    {
                        invis++;
                        continue;
                    } else if ( !introduced )
                    {
                        invis++;
                        continue;
                    }
                }
#endif

                //current_time - czas ) / 360 => czas znajomosci w godzinach
                // TIME_TO_FORGET_FRIEND * 24 => czas do zapomnienia w godzinach bo TIME_TO... jest w dniach
                if ( friend_exist( ch, wch->name, NULL, fr_info, &czas, &tmp ) )
                    if ( ( ( current_time - czas ) / 3600 ) > ( TIME_TO_FORGET_FRIEND * 24 ) )
                    {
                        friend_usun( ch, wch->name );
                    }

                if ( !pfound )
                {
                    sprintf( buf, "\n\r{gGracze\n\r------\n\r{x" );
                    add_buf( output, buf );
                    pfound = TRUE;
                }

                if( IS_IMMORTAL( ch ) && is_affected(wch,gsn_noob_killer) && ( ( noob_killer = affect_find(wch->affected,gsn_noob_killer) ) != NULL ) && noob_killer->modifier >= 3 )
                {
                    if ( !IS_NULLSTR( wch->pcdata->new_title ) && ( IS_IMMORTAL( ch ) || wch == ch ) )
                        sprintf( title, "%s {x({RNowy przydomek{x: %s), %d noobkilli", wch->pcdata->title, wch->pcdata->new_title, noob_killer->modifier );
                    else
                        sprintf( title, "%s, %d noobkilli", wch->pcdata->title, noob_killer->modifier );
                }
                else
                {
                    if ( !IS_NULLSTR( wch->pcdata->new_title ) && ( IS_IMMORTAL( ch ) || wch == ch ) )
                        sprintf( title, "%s {x({RNowy przydomek{x: %s)", wch->pcdata->title, wch->pcdata->new_title );
                    else
                        sprintf( title, "%s", wch->pcdata->title );
                }
#ifdef ENABLE_SHOW_LFG
                if(IS_SET( wch->comm, COMM_LFG ))
                    sprintf(fr_info, " {RLFG{x");
#endif
                if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) && EXT_IS_SET( wch->act, PLR_UNREGISTERED ) )
                    sprintf( buf, "{g%-13s {C%-14s{x {g%s{x {R(UNREGISTERED){x  {w%s{x\n\r",
                            wch->name,
                            ( IS_NPC( wch ) || !wch->pcdata->clan ) ? ( ( wch->level <= LEVEL_NEWBIE ) ? ((wch->sex == 2) ? "{G(pocz±tkuj±ca){x" : ((wch->sex == 0)? "{G(pocz±tkuj±ce){x":"{G(pocz±tkuj±cy){x")) : "" ) : wch->pcdata->clan->who_name,
                            IS_NPC( wch ) ? "" : title, fr_info );
                else if ( wch->level <= LEVEL_NEWBIE )
                    sprintf( buf, "{g%-13s {C%-14s{x {g%s{x    {w%s{x\n\r",
                            wch->name,
                            ( IS_NPC( wch ) || !wch->pcdata->clan ) ?
                            ((wch->sex == 2) ? "{G(pocz±tkuj±ca){x" : ((wch->sex == 0)? "{G(pocz±tkuj±ce){x":"{G(pocz±tkuj±cy){x")):wch->pcdata->clan->who_name,
                            IS_NPC( wch ) ? "" : title, fr_info );
                else
                    sprintf( buf, "{g%-13s {C%-14s{x {g%s{x   {w%s{x\n\r",
                            wch->name,
                            ( IS_NPC( wch ) || !wch->pcdata->clan ) ? "" : wch->pcdata->clan->who_name,
                            IS_NPC( wch ) ? "" : title, fr_info );

                add_buf( output, buf );

                if ( show_special_info )
                {
                    //regentowi na who info pokazuje tylko klase, lew i ilosc fitow
                    if ( !IS_NPC(ch) && IS_IMMORTAL(ch) && IS_SET( ch->pcdata->wiz_conf, W4 ) && !IS_SET( ch->pcdata->wiz_conf, W5 ) )
                    {
                        sprintf( buf, "Class:%s Lev:%-2d",
                                class_table[ wch->class ].who_name, wch->level );
                        add_buf( output, buf );
                    }
                    else
                    {
                        weapon = get_eq_char( wch, WEAR_WIELD );
                        sprintf( buf, "Class:%s Lev:%-2d HP:%-3d/%-3d AC:%d:%d:%d:%d THAC0:%d Dam:%d-%d",
                                class_table[ wch->class ].who_name,
                                wch->level, wch->hit, get_max_hp(wch),
                                GET_AC( wch, AC_PIERCE ) / 10,
                                GET_AC( wch, AC_BASH ) / 10,
                                GET_AC( wch, AC_SLASH ) / 10,
                                GET_AC( wch, AC_EXOTIC ) / 10,
                                compute_tohit( wch, TRUE ),
                                weapon ? ( weapon->value[ 1 ] + weapon->value[ 6 ] + GET_DAMROLL( wch, weapon ) + GET_SKILL_DAMROLL( wch, weapon ) ) : 1 + GET_DAMROLL( wch, NULL ),
                                weapon ? ( weapon->value[ 1 ] * weapon->value[ 2 ] + weapon->value[ 6 ] + GET_DAMROLL( wch, weapon ) + GET_SKILL_DAMROLL( wch, weapon ) ) : 4 + GET_DAMROLL( wch, NULL ) );
                        add_buf( output, buf );
                        if ( ( weapon = get_eq_char( wch, WEAR_SECOND ) ) )
                        {
                            sprintf( buf, "(THAC0:%d Dam:%d-%d)",
                                    compute_tohit( wch, FALSE ),
                                    weapon->value[ 1 ] + weapon->value[ 6 ] + GET_DAMROLL( wch, weapon ) + GET_SKILL_DAMROLL( wch, weapon ),
                                    weapon->value[ 1 ] * weapon->value[ 2 ] + weapon->value[ 6 ] + GET_DAMROLL( wch, weapon ) + GET_SKILL_DAMROLL( wch, weapon ) );
                            add_buf( output, buf );
                        }
                    }
                    add_buf( output, "\n\r" );
                }

                if (show_stats)
                {
                    sprintf
                        (
                         buf,
                         "%s (%d) str:%d int:%d wis:%d dex:%d con:%d cha:%d luc:%d\n\r",
                         class_table[ wch->class ].who_name,
                         wch->level,
                         get_curr_stat( wch, STAT_STR ),
                         get_curr_stat( wch, STAT_INT ),
                         get_curr_stat( wch, STAT_WIS ),
                         get_curr_stat( wch, STAT_DEX ),
                         get_curr_stat( wch, STAT_CON ),
                         get_curr_stat( wch, STAT_CHA ),
                         get_curr_stat( wch, STAT_LUC )
                        );
                    add_buf( output, buf );
                }

                if ( show_affects )
                {
                    AFFECT_DATA *paf;
                    for ( paf = wch->affected; paf != NULL; paf = paf->next )
                    {
                        sprintf( buf,
                                "Spell: '%s' modifies %s by %d for %d mud hours and %d realtime with bits %s, level %d.\n\r",
                                skill_table[ ( int ) paf->type ].name,
                                affect_loc_name( paf->location ),
                                paf->modifier,
                                paf->duration,
                                paf->rt_duration,
                                affect_bit_name( NULL, paf->bitvector ),
                                paf->level
                               );
                        send_to_char( buf, ch );
                    }
                }

                count++;
            }
        }
    }

    sprintf( buf2, "\n\r{cAktualna liczba graczy\n\rWidocznych:      {C%2d{c\n\rNiewidocznych:   {C%2d{c\n\rRazem:           {C%2d{c{x\n\r", count, invis, count + invis );
    add_buf( output, buf2 );
    if (clan)
    {
        sprintf( buf2, "{cKlanowiczów:     {C%2d{x\n\r", clan );
        add_buf( output, buf2 );
    }
    if (IS_IMMORTAL(ch))
    {
        sprintf( buf2, "{cNie¶miertelnych: {C%2d{x\n\r", wiz_count );
        add_buf( output, buf2 );
    }
    page_to_char( buf_string( output ), ch );
    free_buf( output );
    return ;
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
	if( IS_NPC(ch)) return;//mobom dziêkujemy

	if ( ch->ss_data != NULL )
	{
		send_to_char( "Jeste¶ przemieniony w inne cia³o! Nie masz inwentarza!\n\r", ch);
		return;
	}

	if ( ch->position == POS_SLEEPING )
	{
		send_to_char( "¦nisz o przedmiotach które chcia³<&/a/o>by¶ posiadaæ.\n\r", ch );
		return ;
	}
	send_to_char( "Nosisz przy sobie:\n\r", ch );
	show_list_to_char( ch->carrying, ch, TRUE, TRUE );
	return ;
}

void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj, *cover, *gloves;
    int iWear;
    int i;
    bool found,header;
    extern int wear_to_itemwear[ MAX_WEAR ];
    int count_all   = 0;
    int count_invis = 0;

    if ( ch->ss_data != NULL )
    {
        send_to_char( "Jeste¶ przemieniony w inne cia³o! Nie masz ekwipunku!\n\r", ch);
        return;
    }

    if ( ch->position == POS_SLEEPING )
    {
        switch ( number_range( 1, 6 ) )
        {
            case 1:
                send_to_char( "¦ni± ci siê przepiêkne szaty.\n\r", ch );
                break;
            case 2:
                send_to_char( "¦nisz o he³mie z wielka kit±.\n\r", ch );
                break;
            case 3:
                send_to_char( "¦nisz sobie o tym jak piêkne masz ubrania.\n\r", ch );
                break;
            case 4:
                send_to_char( "¦nisz o potê¿nej broni, któr± trzymasz w rêku.\n\r", ch );
                break;
            case 5:
                send_to_char( "¦nisz o bogato zdobionym pasie, który masz na sobie.\n\r", ch );
                break;
            default :
                send_to_char( "¦nisz o wszystkim co mo¿esz ubraæ.\n\r", ch );
                break;
        }
        return ;
    }

    /*
     * get cover from char
     */
    cover = get_eq_char( ch, WEAR_ABOUT );
    if ( cover && !IS_OBJ_STAT( cover, ITEM_COVER ) )
    {
        cover = NULL;
    }
    /*
     * get gloves from char
     */
    gloves = get_eq_char( ch, WEAR_HANDS );
    if ( gloves && gloves->item_type != ITEM_ARMOR )
    {
        gloves = NULL;
    }
    found = FALSE;
    header = TRUE;

    for ( i = 0; wear_order[i] >= 0; i++ )
    {
        if (header)
        {
            send_to_char( "U¿ywasz:\n\r", ch );
            header = FALSE;
        }
        iWear = wear_order[i];
        if ( iWear == WEAR_SHIELD )
        {
            send_to_char( "\n\r", ch );
        }
        if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
        {
            continue;
        }
        /**
         * zlicz graty
         */
        count_all++;
        found = TRUE;
        /**
         * nie widzi, nie pokazujemy
         */
        if ( !can_see_obj( ch, obj ) )
        {
            count_invis++;
            continue;
        }
        /**
         * wyjatek dla dwurêcznie
         */
        if ( iWear == WEAR_WIELD && ( IS_WEAPON_STAT( obj, WEAPON_TWO_HANDS ) ))
        {
            send_to_char( "<trzymane dwurêcznie>          ", ch );
        }
        else
        {
            send_to_char( where_name[ iWear ], ch );
        }
        send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
        /*
         * obs³uga rzeczy noszonych pod p³aszczem
         */
        if (
                cover
                && can_see_obj( ch, cover )
                && IS_SET( cover->wear_flags, wear_to_itemwear[ iWear ] )
           )
        {
            printf_to_char( ch, " {D(pod %s){x", cover->name5 );
        }
        /*
         * obs³uga pier¶cieni jest zale¿na od tego, czy posiadamy rêkawice
         * oraz czy te rêkawice zas³aniaj± pier¶cienie
         */
        if (
                ( iWear == WEAR_FINGER_L || iWear == WEAR_FINGER_R )
                && gloves
                && can_see_obj( ch, gloves )
           )
        {
            if ( gloves->value[5] == GLOVES_UNDER )
            {
                printf_to_char( ch, " {D(pod %s){x", gloves->name5 );
            }
            else
            {
                printf_to_char( ch, " {D(na %s){x", gloves->name6 );
            }
        }
        send_to_char( "\n\r", ch );
    }
    if ( count_all == 0 )
    {
        send_to_char( "{RNiczego nie u¿ywasz.{x\n\r", ch );
    }
    else if ( count_all == count_invis )
    {
        send_to_char( "{RNiczego z tego co u¿ywasz nie widzisz, ale chyba pamiêtasz?{x\n\r", ch );
    }
    else if ( count_invis != 0 )
    {
        send_to_char( "\n\r{RNie widzisz wszystkich za³o¿onych na siebie przedmiotów.{x\n\r", ch );
    }
    return ;
}

void do_credits( CHAR_DATA *ch, char *argument )
{
	do_function( ch, &do_help, "diku" );
	return ;
}

void do_consider( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_INPUT_LENGTH ];
	AFFECT_DATA af;
	CHAR_DATA *victim;
	char *msg;
	int diff, skill, wisdom = get_curr_stat( ch, STAT_WIS );
	int vstr, vcon, vdex, vcha;

	one_argument( argument, arg );

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Kogo to chcesz oceniæ?\n\r", ch );
		return ;
	}

	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie ma tutaj kogo¶ takiego.\n\r", ch );
		return ;
	}

	if ( is_safe( ch, victim ) )
	{
		send_to_char( "Nawet o tym nie my¶l.\n\r", ch );
		return ;
	}

	if ( ch == victim )
	{
		send_to_char( "Tak, ale w³a¶ciwie co chcesz u siebie z czym porównaæ?\n\r", ch );
		return ;
	}
	if ( ( skill = get_skill( ch, gsn_lore_intuition ) ) <= 0 || is_affected( ch, gsn_lore_intuition ))
	{
		if ( !IS_NPC( victim ) && !victim->ss_data )
		{
			send_to_char( "Prawdopodobnie skoñczy siê to twoim zgonem, ale co tam...\n\r", ch );
			return ;
		}
		diff = victim->level - ch->level;
		if ( diff <= -20 ) msg = "Naprawdê szkoda twojego czasu, czy nie wystarczy po prostu spojrzeæ?";
		else if ( diff <= -10 ) msg = "Ka¿demu przyda siê ma³a rozgrzewka.";
		else if ( diff <= -5 ) msg = "$N to nie wyzwanie dla ciebie.";
		else if ( diff <= -3 ) msg = "$N wyglada na ³atwy cel.";
		else if ( diff <= 2 ) msg = "$N wygl±da na godnego ciebie przeciwnika.";
		else if ( diff <= 3 )
		{

			msg = "Chyba bêdziesz potrzebowa³o przyjació³.";
			switch (ch->sex)
			{
				case 0:
					msg = "Chyba bêdziesz potrzebowa³o przyjació³.";
					break;
				case 1:
					msg = "Chyba bêdziesz potrzebowa³ przyjació³.";
					break;
				default:
					msg = "Chyba bêdziesz potrzebowa³a przyjació³.";
					break;
			}
		}
		else if ( diff <= 5 )
		{
			switch (ch->sex)
			{
				case 0:
					msg = "Bêdziesz potrzebowa³o przyjació³ i dobrego sprzetu.";
					break;
				case 1:
					msg = "Bêdziesz potrzebowa³ przyjació³ i dobrego sprzetu.";
					break;
				default:
					msg = "Bêdziesz potrzebowa³a przyjació³ i dobrego sprzetu.";
					break;
			}
		}
		else if ( diff <= 6 )
		{
			switch (ch->sex)
			{
				case 0:
					msg = "Bêdziesz potrzebowa³o przyjació³, dobrego sprzetu i drogi ucieczki.";
					break;
				case 1:
					msg = "Bêdziesz potrzebowa³ przyjació³, dobrego sprzetu i drogi ucieczki.";
					break;
				default:
					msg = "Bêdziesz potrzebowa³a przyjació³, dobrego sprzetu i drogi ucieczki.";
					break;
			}
		}
		else msg = "Prawdopodobnie skoñczy siê twoim zgonem, ale co tam...";
		act( msg, ch, NULL, victim, TO_CHAR );
		WAIT_STATE(ch, 12);
		return ;
	}
	else
	{
		if ( victim->level > 26 ) msg = "$N to wszechpotê¿na istota.";
		else if ( victim->level > 19 ) msg = "$N wygl±da na do¶wiadczon± osobê.";
		else if ( victim->level > 13 ) msg = "$N z pewno¶ci± nie wygl±da na s³abeusza.";
		else if ( victim->level > 7 ) msg = "$N ró¿ni siê trochê pod wzglêdem do¶wiadczenia od zwyk³ych mieszkañców miast.";
		else if ( victim->level > 3 ) msg = "$N jeszcze ma³o wie o ¶wiecie.";
		else if ( victim->level > 0 ) msg = "$N ma do¶wiadczenie dziecka.";
		else msg = "Nie udaje ci siê nic wywnioskowaæ z wygl±du $Z.";
		act( msg, ch, NULL, victim, TO_CHAR );

		//if ( skill + (wisdom-10)*3 > number_range (0,200))
		//    print_char( ch, "%s ma oko³o %d punktów ¿ycia.\n\r", victim->short_descr, UMAX( 1, number_range( (victim->hit - (100-skill)), (victim->hit + (100-skill)))));

		if ( skill + wisdom/4 > number_range (1,100))
		{
			if ( ( vstr = get_curr_stat_deprecated( victim, STAT_STR )) > 25 ) msg = "$N wygl±da jak ogromna góra miê¶ni.";
			else if ( vstr > 22 ) msg = "Si³a miê¶ni $Z budzi twój podziw.";
			else if ( vstr > 17 ) msg = "$N wyró¿nia siê sw± si³± spo¶ród mot³ochu.";
			else if ( vstr > 14 ) msg = "Miê¶nie $Z wygl±daj± do¶æ przeciêtnie.";
			else if ( vstr > 9 ) msg = "Miê¶nie $Z z pewno¶ci± nie budz± podziwu.";
			else if ( vstr > 5 ) msg = "Miê¶nie $Z wzbudzaj± w tobie ¶miech.";
			else if ( vstr > 2 ) msg = "$N ma si³ê niemowlêcia.";
			else msg = "Nie jeste¶ w stanie okre¶liæ si³y $Z.";
			act( msg, ch, NULL, victim, TO_CHAR );
		}

		if ( skill +  wisdom/4 > number_range (1,100))
		{
			if ( ( vcon = get_curr_stat_deprecated( victim, STAT_CON )) > 25 ) msg = "Skóra $Z jest twarda jak stal.";
			else if ( vcon > 22 ) msg = "Budowa cia³a $Z budzi twój podziw.";
			else if ( vcon > 17 ) msg = "$N wyró¿nia siê sw± budow± cia³a spo¶ród mot³ochu.";
			else if ( vcon > 14 ) msg = "Cia³o $Z wygl±da do¶æ przeciêtnie.";
			else if ( vcon > 9 ) msg = "Budowa cia³a $Z z pewno¶ci± nie budzi podziwu.";
			else if ( vcon > 5 ) msg = "Budowa cia³a $Z wzbudza w tobie ¶miech.";
			else if ( vcon > 2 ) msg = "$N ma wytrzyma³o¶æ niemowlêcia.";
			else msg = "Nie jeste¶ w stanie okre¶liæ wytrzyma³o¶ci $Z.";
			act( msg, ch, NULL, victim, TO_CHAR );
		}

		if ( skill +  wisdom/4 > number_range (1,100))
		{
			if ( ( vdex = get_curr_stat_deprecated( victim, STAT_DEX )) > 25 ) msg = "$N porusza siê tak szybko, ¿e ledwo jeste¶ w stanie to dostrzec.";
			else if ( vdex > 22 ) msg = "Zwinno¶æ $Z budzi twój podziw.";
			else if ( vdex > 17 ) msg = "$N wyró¿nia siê sw± zrêczno¶ci± spo¶ród mot³ochu.";
			else if ( vdex > 14 ) msg = "$N nie wyró¿nia siê sw± zrêczno¶ci±.";
			else if ( vdex > 9 ) msg = "$N porusza siê do¶æ niezdarnie.";
			else if ( vdex > 5 ) msg = "$N co chwilê siê o co¶ potyka.";
			else if ( vdex > 2 ) msg = "$N ma zrêczno¶æ niemowlêcia.";
			else msg = "Nie jeste¶ w stanie okre¶liæ zrêczno¶ci $Z.";
			act( msg, ch, NULL, victim, TO_CHAR );
		}

		if ( skill +  wisdom/4 > number_range (1,100))
		{
			if ( ( vcha = get_curr_stat_deprecated( victim, STAT_CHA )) > 25 ) msg = "$N wygl±da po prostu BOSKO!";
			else if ( vcha > 22 ) msg = "Uroda $Z budzi twój podziw.";
			else if ( vcha > 17 ) msg = "$N zwraca tw± uwagê nadzwyczajn± urod±.";
			else if ( vcha > 14 ) msg = "$N nie wyró¿nia siê specjalnie sw± urod±.";
			else if ( vcha > 9 ) msg = "Odrzuca ciê na sam widok twarzy $Z.";
			else if ( vcha > 5 ) msg = "$N wygl±da ohydnie.";
			else if ( vcha > 2 ) msg = "$N jest potwornie brzydki.";
			else msg = "Nie jeste¶ w stanie okre¶liæ charyzmy $Z.";
			act( msg, ch, NULL, victim, TO_CHAR );
		}

		if (victim->weight > 12000) msg = "$N wa¿y znacznie wiêcej ni¿ tonê.";
		else if (victim->weight > 8000 ) msg = "$N wa¿y oko³o tony.";
		else if (victim->weight > 5500 ) msg = "$N wa¿y wiêcej ni¿ pó³ tony.";
		else if (victim->weight > 4000 ) msg = "$N wa¿y oko³o pó³ tony.";
		else if (victim->weight > 2000 ) msg = "$N wa¿y oko³o trzystu kilogramów.";
		else if (victim->weight > 1200 ) msg = "Waga $Z znacznie przekracza wagê zwyk³ego cz³owieka.";
		else if (victim->weight > 900 ) msg = "Waga $Z przekracza wagê zwyk³ego cz³owieka.";
		else if (victim->weight > 650 ) msg = "$N wa¿y tyle, ile powinien wa¿yæ zwyk³y, zdrowy cz³owiek.";
		else if (victim->weight > 400 ) msg = "$N wa¿y tyle, ile powinien wa¿yæ zwyk³y, zdrowy nizio³ek.";
		else if (victim->weight > 150 ) msg = "$N wa¿y tyle, ile powinien wa¿yæ zwyk³y, zdrowy kotek.";
		else msg = "Wagê $Z mo¿na porównaæ do wagi niemowlêcia.";
		act( msg, ch, NULL, victim, TO_CHAR );

		if (victim->height > 1200) msg = "Wysoko¶æ $Z powinno siê liczyæ w kilometrach.";
		else if (victim->height > 800 ) msg = "$N ma wzrost jak piêciu ustawionych na sobie zdrowych mê¿czyzn.";
		else if (victim->height > 550 ) msg = "G³owa $Z znajduje siê ze cztery metry nad tob±.";
		else if (victim->height > 400 ) msg = "Wzrost $Z przekracza twój bardziej ni¿ podwójnie.";
		else if (victim->height > 250 ) msg = "Wzrost $Z znacznie przekracza wzrost zwyk³ego cz³owieka.";
		else if (victim->height > 185 ) msg = "Wzrost $Z przekracza wzrost zwyk³ego cz³owieka.";
		else if (victim->height > 160 ) msg = "$N mierzy tyle, ile powinien mierzyæ zwyk³y, zdrowy cz³owiek.";
		else if (victim->height > 145 ) msg = "Wzost $Z przekracza wzrost zwyk³ego nizio³ka.";
		else if (victim->height > 110 ) msg = "$N mierzy tyle, ile powinien mierzyæ zwyk³y, zdrowy nizio³ek.";
		else if (victim->height > 80 ) msg = "$N mierzy tyle, ile powinino mierzyæ zdrowe, sze¶cioletnie ludzkie dziecko.";
		else if (victim->height > 40 ) msg = "$N ma ko³o pó³ metra.";
		else msg = "Wzrost $Z mo¿na porównaæ do wzrostu niemowlêcia.";
		act( msg, ch, NULL, victim, TO_CHAR );

		print_char( ch, "%s to %s.\n\r", PERS( victim, ch ), race_table[ victim->race ].name );

		/*if ( skill + (wisdom-10)*3 > number_range (1,100))
		  {
		  if ( victim->alignment < 0 ) msg = "Zachowanie $Z wskazuje na to, i¿ ¶cie¿k± $S ¿ycia jest z³o.";
		  else if ( victim->alignment > 0 ) msg = "Zachowanie $Z wskazuje na to, i¿ ¶cie¿k± $S ¿ycia jest dobro.";
		  else msg = "Zachowanie $Z wskazuje na to, i¿ ¶cie¿k± $S ¿ycia jest neutralno¶æ";
		  act( msg, ch, NULL, victim, TO_CHAR );
		  }*/

		if ( skill +  wisdom/4 > number_range (1,100))
		{
			if ( get_caster( victim ) > 0 )
			{
				act( "$Z otacza ledwo widoczna aura magii.", ch, NULL, victim, TO_CHAR );
				return;
			}
		}

		af.where = TO_AFFECTS;
		af.type = gsn_lore_intuition;
		af.level = 0;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = &AFF_NONE;
		af.duration = URANGE(1, 5-(skill/15), 5); af.rt_duration = 0;
		af.visible = FALSE;
		affect_to_char( ch, &af, NULL, FALSE );
		WAIT_STATE( ch, 12 );
		check_improve( ch, NULL, gsn_lore_intuition, TRUE, 14 );
		return;
	}
}

void set_title( CHAR_DATA *ch, char *title )
{
	char buf[ MAX_STRING_LENGTH ];
	char buf2[ MAX_STRING_LENGTH ];
	char *p;
	int i;

	if ( IS_NPC( ch ) )
	{
		bug( "Set_title: NPC.", 0 );
		return ;
	}

	if ( title[ 0 ] != '.' && title[ 0 ] != ',' && title[ 0 ] != '!' && title[ 0 ] != '?' )
	{
		buf[ 0 ] = ' ';
		strcpy( buf + 1, title );
	}
	else
	{
		strcpy( buf, title );
	}

	p = buf;
	i = 0;

	while ( *p )
	{
		if ( *p == '{' && *( p + 1 ) == '/' )
		{
			buf2[ i ] = ' ';
			p++;
		}
		else
			buf2[ i ] = *p;
		i++;
		p++;
	}

	buf2[ i ] = '\0';

	free_string( ch->pcdata->title );
	ch->pcdata->title = str_dup( buf2 );
	save_char_obj( ch, FALSE, FALSE );
	return ;
}

void set_new_title( CHAR_DATA *ch, char *title )
{
	char buf[ MAX_STRING_LENGTH ];
	char buf2[ MAX_STRING_LENGTH ];
	char *p;
	int i;

	if ( IS_NPC( ch ) )
	{
		bug( "Set_new_title: NPC.", 0 );
		return ;
	}

	strcpy( buf, title );
	p = buf;
	i = 0;

	while ( *p )
	{
		if ( *p == '{' && *( p + 1 ) == '/' )
		{
			buf2[ i ] = ' ';
			p++;
		}
		else
			buf2[ i ] = *p;
		i++;
		p++;
	}

	buf2[ i ] = '\0';

	free_string( ch->pcdata->new_title );
	ch->pcdata->new_title = str_dup( buf2 );
	save_char_obj( ch, FALSE, FALSE );
	return ;
}

void do_title( CHAR_DATA *ch, char *argument )
{
	char arg [ MAX_INPUT_LENGTH ];
	char arg2[ MAX_INPUT_LENGTH ];
	char buf[ MAX_STRING_LENGTH ];
	CHAR_DATA *victim;

	if( IS_NPC(ch)) return;//mobom dziêkujemy

	argument = one_argument( argument, arg );

	if ( IS_NPC( ch ) )
		return ;

	if ( !IS_IMMORTAL( ch ) )
	{
		send_to_char( "Przydomek mo¿na ustawiæ od 25 poziomu korzystaj±c z g³ównego menu gry.\n\r", ch );
		return ;
	}

	if ( ch->level < 25 )
	{
		send_to_char( "Ale o co chodzi?\n\r", ch );
		return ;
	}

	if ( !str_cmp( arg, "allow" ) )
	{
		argument = one_argument( argument, arg2 );

		if ( ( victim = get_char_world( ch, arg2 ) ) == NULL || IS_NPC( victim ) )
		{
			send_to_char( "Nie ma takiego gracza.\n\r", ch );
			return;
		}

		if ( IS_NULLSTR( victim->pcdata->new_title ) )
		{
			print_char( ch, "Gracz %s nie ma nowego przydomka do akceptacji.\n\r", victim->name );
			return;
		}

		set_title( victim, victim->pcdata->new_title);
		free_string( victim->pcdata->new_title );
		send_to_char( "Title przyjête.\n\r", ch );
		send_to_char( "{GPrzydomek zosta³ zaakceptowany.{x\n\r", victim );
		return;
	}

	if ( !str_cmp( arg, "reset" ) )
	{
		argument = one_argument( argument, arg2 );

		if ( ( victim = get_char_world( ch, arg2 ) ) == NULL || IS_NPC( victim ) )
		{
			send_to_char( "Nie ma takiego gracza.\n\r", ch );
			return;
		}

		sprintf( buf, "%s %s", victim->sex == SEX_FEMALE ? "kobieta" :"mê¿czyzna", race_table[GET_RACE(victim)].name );
		set_title( victim, buf );
		free_string( victim->pcdata->new_title );
		save_char_obj( ch, FALSE, FALSE );

		send_to_char( "{GTitle zresetowane.{x\n\r", ch );
		send_to_char( "{RTwój przydomek zosta³ przywrócony do standardowego.{x\n\r", victim );
		return;
	}


	if ( !str_cmp( arg, "deny" ) )
	{
		argument = one_argument( argument, arg2 );

		if ( ( victim = get_char_world( ch, arg2 ) ) == NULL || IS_NPC( victim ) )
		{
			send_to_char( "Nie ma takiego gracza.\n\r", ch );
			return;
		}

		if ( IS_NULLSTR( victim->pcdata->new_title ) )
		{
			print_char( ch, "Gracz %s nie ma nowego przydomka do akceptacji.\n\r", victim->name );
			return;
		}

		free_string( victim->pcdata->new_title );
		if ( IS_NULLSTR( argument ) )
		{
			send_to_char( "Title odrzucone. Nie podano powodu.\n\r", ch );
			send_to_char( "{RPrzydomek nie zosta³ zaakceptowany.{x\n\r", victim );
		}
		else
		{
			print_char( ch, "Title odrzucone. Powód: %s\n\r", argument );
			print_char( victim, "{RPrzydomek nie zosta³ zaakceptowany.\n\rPowód: %s.\n\r", argument );
		}
		return;
	}

	if ( arg[ 0 ] == '\0' )
	{
		send_to_char( "Jaki przydomek chcia³<&/a/o>by¶ sobie ustawiæ?\n\r", ch );
		return ;
	}

	if ( strlen( arg ) - count_colors( arg, 0 ) > 45 )
	{
		send_to_char( "Przydomek za d³ugi, nie mo¿e mieæ wiêcej ni¿ 45 znaków.\n\r", ch );
		return ;
	}

	smash_tilde( arg );
	set_title( ch, arg);
	send_to_char( "Przydomek zosta³ zmieniony.\n\r", ch );
}



void do_description( CHAR_DATA *ch, char *argument )
{
	char buf[ MAX_STRING_LENGTH ];

	if( IS_NPC(ch)) return;//mobom dziêkujemy

	send_to_char( "Opis postaci mo¿na ustawiæ z poziomu g³ównego menu gry.\n\r", ch );
	return ;

	if ( argument[ 0 ] != '\0' )
	{
		buf[ 0 ] = '\0';
		smash_tilde( argument );

		if ( argument[ 0 ] == '-' )
		{
			int len;
			bool found = FALSE;

			if ( ch->description == NULL || ch->description[ 0 ] == '\0' )
			{
				send_to_char( "Nie ma ju¿ linii do usuniêcia.\n\r", ch );
				return ;
			}

			strcpy( buf, ch->description );

			for ( len = strlen( buf ); len > 0; len-- )
			{
				if ( buf[ len ] == '\r' )
				{
					if ( !found )   /* back it up */
					{
						if ( len > 0 )
							len--;
						found = TRUE;
					}
					else /* found the second one */
					{
						buf[ len + 1 ] = '\0';
						free_string( ch->description );
						ch->description = str_dup( buf );
						send_to_char( "Twój opis to:\n\r", ch );
						send_to_char( ch->description ? ch->description :
								"(None).\n\r", ch );
						return ;
					}
				}
			}
			buf[ 0 ] = '\0';
			free_string( ch->description );
			ch->description = str_dup( buf );
			send_to_char( "Opis wyczyszczony.\n\r", ch );
			return ;
		}
		if ( argument[ 0 ] == '+' )
		{
			if ( ch->description != NULL )
				strcat( buf, ch->description );
			argument++;
			while ( isspace( *argument ) )
				argument++;
		}

		if ( strlen( buf ) >= 1024 )
		{
			send_to_char( "Opis niestety jest za d³ugi.\n\r", ch );
			return ;
		}

		strcat( buf, argument );
		strcat( buf, "\n\r" );
		free_string( ch->description );
		ch->description = str_dup( buf );
	}

	send_to_char( "Twój opis to:\n\r", ch );
	send_to_char( ch->description ? ch->description : "(None).\n\r", ch );
	return ;
}

#define GET_LEARN_PAYMENT( learned, payment ) ( ((learned+1)*(learned+1))*payment/100 )

//do_learn lekko zmodyfikowany przez Raszera
void do_learn( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA * mob = NULL;
	char buf[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	char* argall;
	int sn = 0;
	LEARN_DATA *ld = NULL, *ldmob = NULL;
	LEARN_LIST *ls = NULL;
	bool oksn = FALSE, learn = FALSE, proper_teacher = FALSE;
	sh_int chance = 100, min = 0, max = 100, payment = 0, pay_from = 0;
	int tmp, chance_mod, weaponskill_max = 0;
    int max_plus = 7;
    int wis = 0;
    int intel = 0;
    
	if( IS_NPC(ch)) return;//mobom dziêkujemy

	argall = argument;
	one_argument( argument, arg );

	save_debug_info( "do_learn", NULL, "przed wykonaniem funkcji", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_INFO, TRUE );

	/* dajemy standardowy tekst jak go¶æ jest pod wp³ywem 'silence' */
	if ( IS_AFFECTED( ch, AFF_SILENCE ) )
	{
		send_to_char( AFF_SILENCE_TEXT, ch );
		return ;
	}

	if ( arg[ 0 ] != '\0' && ( !str_cmp( arg, "all" ) ) )
	{
		send_to_char( "A co to takiego ten 'all'?\n\r", ch );
		return ;
	}
    
	
    if ( arg[ 0 ] != '\0' && ( sn = find_skill( ch, argument, FALSE ) ) < 0 )
	{
		for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
			if ( IS_NPC( mob ) && EXT_IS_SET( mob->act, ACT_PRACTICE ) && is_name( arg, mob->name ) )
				break;

		if ( mob == NULL )
		{
			send_to_char( "Nie widzisz tutaj takiego nauczyciela.\n\r", ch );
			return ;
		}

		argument = one_argument( argument, arg );
	}

	if ( mob == NULL || sn >= 0 )
	{
		for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
			if ( IS_NPC( mob ) && EXT_IS_SET( mob->act, ACT_PRACTICE ) )
				break;

		/* oo nie ma zadnego nauczyciela */
		if ( mob == NULL )
		{
			send_to_char( "Nie ma tutaj nikogo, kto móg³by ciê czego¶ nauczyæ.\n\r", ch );
			return ;
		}
	}


	for ( ld = learn_system ; ld ; ld = ld->next )
		if ( ld->vnum == mob->pIndexData->vnum )
		{
			ldmob = ld;
			break;
		}

	if ( !ldmob )
	{
		send_to_char( "Nie widzisz tutaj nikogo, kto móg³by ciê czego¶ nauczyæ.\n\r", ch );
		return ;
	}

	switch ( ch->class )
	{
		case CLASS_MAG:
			if ( EXT_IS_SET( mob->act, ACT_MAGE ) )
				proper_teacher = TRUE;
			break;
		case CLASS_CLERIC:
			if ( EXT_IS_SET( mob->act, ACT_CLERIC ) )
				proper_teacher = TRUE;
			break;

		case CLASS_THIEF:
			if ( EXT_IS_SET( mob->act, ACT_THIEF ) )
				proper_teacher = TRUE;
			break;

		case CLASS_WARRIOR:
			if ( EXT_IS_SET( mob->act, ACT_WARRIOR ) )
				proper_teacher = TRUE;
			break;

		case CLASS_PALADIN:
			if ( EXT_IS_SET( mob->act, ACT_PALADIN ) )
				proper_teacher = TRUE;
			break;

		case CLASS_DRUID:
			if ( EXT_IS_SET( mob->act, ACT_DRUID ) )
				proper_teacher = TRUE;
			break;

		case CLASS_BARBARIAN:
			if ( EXT_IS_SET( mob->act, ACT_BARBARIAN ) )
				proper_teacher = TRUE;
			break;

		case CLASS_MONK:
			if ( EXT_IS_SET( mob->act, ACT_MONK ) )
				proper_teacher = TRUE;
			break;

		case CLASS_BARD:
			if ( EXT_IS_SET( mob->act, ACT_BARD ) )
				proper_teacher = TRUE;
			break;

		case CLASS_BLACK_KNIGHT:
			if ( EXT_IS_SET( mob->act, ACT_BLACK_KNIGHT ) )
				proper_teacher = TRUE;
			break;

		case CLASS_SHAMAN:
			if ( EXT_IS_SET( mob->act, ACT_SHAMAN ) )
				proper_teacher = TRUE;
			break;

		default:
			send_to_char( "Nie widzisz tutaj nikogo, kto móg³by ciê czego¶ nauczyæ.\n\r", ch );
			return ;
	}

	if ( ! proper_teacher )
	{
		act( "{k$N mówi ci '{KNiestety, nie znam siê na umiejêtno¶ciach twojej profesji.{k'{x", ch, NULL, mob, TO_CHAR );
		return ;
	}

	if (is_affected (ch, gsn_energy_drain))
	{
		act( "{k$N mówi ci '{KWidzê, ¿e nie potrafisz siê teraz skupiæ na nauce.{k'{x", ch, NULL, mob, TO_CHAR );
		return;

	}

	if ( ch->precommand_fun && !ch->precommand_pending )
	{
		send_to_char( "Robisz teraz co¶ innego.\n\r", ch );
		return ;
	}

	if ( get_mob_memdat( mob, NULL, NULL, MEMDAT_ANY ) && HAS_TRIGGER( mob, TRIG_PRECOMMAND ) )
	{
		switch (mob->sex)
		{
			case 0:
				act( "{k$n mówi ci '{KJestem teraz zajête. Proszê chwilkê poczekaæ.{k'{x", mob, NULL, ch, TO_VICT );
				break;
			case 1:
				act( "{k$n mówi ci '{KJestem teraz zajêty. Proszê chwilkê poczekaæ.{k'{x", mob, NULL, ch, TO_VICT );
				break;
			default :
				act( "{k$n mówi ci '{KJestem teraz zajêta. Proszê chwilkê poczekaæ.{k'{x", mob, NULL, ch, TO_VICT );
				break;
		}
		return;
	}


	if ( !ch->precommand_pending && HAS_TRIGGER( mob, TRIG_PRECOMMAND ) )
	{
		if ( mp_precommand_trigger( mob, ch, NULL, &do_learn, "learn", argall ) )
			return ;
	}

	if ( argument[ 0 ] == '\0' )
	{
		int col = 0;

		for ( ls = ldmob->list ; ls ; ls = ls->next )
		{
			if ( skill_table[ ls->sn ].name == NULL )
				break;

			if ( ch->level < skill_table[ ls->sn ].skill_level[ ch->class ] )
				continue;

			oksn = FALSE;

			/* skills */
			if ( skill_table[ ls->sn ].spell_fun == spell_null )
			{//wstawka systemu uczenia siê masterow
				if (ls->sn == gsn_spear_mastery ||
						ls->sn == gsn_staff_mastery ||
						ls->sn == gsn_polearm_mastery ||
						ls->sn == gsn_shortsword_mastery ||
						ls->sn == gsn_whip_mastery ||
						ls->sn == gsn_axe_mastery ||
						ls->sn == gsn_sword_mastery ||
						ls->sn == gsn_flail_mastery ||
						ls->sn == gsn_mace_mastery ||
						ls->sn == gsn_dagger_mastery  )
				{
					if( get_skill( ch, ls->sn ) > 0 )
					{
						oksn = TRUE;
					}
					else
					{
						if ( get_skill(ch,gsn_spear_mastery) > 0 ) continue;
						if ( get_skill(ch,gsn_staff_mastery) > 0 ) continue;
						if ( get_skill(ch,gsn_polearm_mastery ) > 0 ) continue;
						if ( get_skill(ch,gsn_shortsword_mastery) > 0 ) continue;
						if ( get_skill(ch,gsn_whip_mastery) > 0 ) continue;
						if ( get_skill(ch,gsn_axe_mastery) > 0 ) continue;
						if ( get_skill(ch,gsn_sword_mastery) > 0 ) continue;
						if ( get_skill(ch,gsn_flail_mastery) > 0 ) continue;
						if ( get_skill(ch,gsn_mace_mastery) > 0 ) continue;
						if ( get_skill(ch,gsn_dagger_mastery) > 0 ) continue;
						weaponskill_max = get_skill(ch,gsn_staff);
						weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_sword));
						weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_shortsword));
						weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_whip));
						weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_polearm));
						weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_axe));
						weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_spear));
						weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_flail));
						weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_mace));
						weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_dagger));
						if ( weaponskill_max <= 50 ) continue;
						if ( weaponskill_max == get_skill(ch,gsn_spear		) && ls->sn == gsn_spear_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_staff		) && ls->sn == gsn_staff_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_polearm	) && ls->sn == gsn_polearm_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_shortsword	) && ls->sn == gsn_shortsword_mastery 		) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_whip		) && ls->sn == gsn_whip_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_axe		) && ls->sn == gsn_axe_mastery				) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_sword		) && ls->sn == gsn_sword_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_flail		) && ls->sn == gsn_flail_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_mace		) && ls->sn == gsn_mace_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_dagger		) && ls->sn == gsn_dagger_mastery 			) oksn = TRUE;
					}
				}
				else
				{
					if ( ls->sn == gsn_undead_resemblance && ch->class == CLASS_MAG && ch->pcdata->mage_specialist != 7 )
						oksn = FALSE;
					else
						oksn = TRUE;
				}
			}
			/* spells */
			else
			{
				if ( ch->pcdata->learned[ ls->sn ] == 0
						&& can_learn_here( ch, mob, ls->sn )
						&& can_learn_spell( ch, ls->sn ) )
					oksn = TRUE;
			}

			if ( !oksn )
				continue;

			if ( !learn )
			{
				printf_to_char( ch, "%s mo¿e nauczyæ nastêpuj±cych rzeczy:\n\r", capitalize( mob->short_descr ) );
				learn = TRUE;
			}

			if ( ls->payment > 0 && (ch->pcdata->learned[ ls->sn ]+ch->pcdata->learning_rasz[ ls->sn ]) >= ls->pay_from )
            {
				if ( (ch->pcdata->learned[ ls->sn ] + ch->pcdata->learning_rasz[ ls->sn ]) < ls->min || (ch->pcdata->learned[ ls->sn ]+ch->pcdata->learning_rasz[ ls->sn ]) > ls->max )
                {
					sprintf( buf, "%-37s{x{x ", skill_table[ ls->sn ].name );
                }
				else
                {
                    sprintf
                        (
                         buf,
                         "%s%-25s{x {W%s{x ",
                         skill_table[ ls->sn ].spell_fun == spell_null ? "{G" : "{B",
                         skill_table[ ls->sn ].name,
                         money_string ( GET_LEARN_PAYMENT( (ch->pcdata->learned[ ls->sn ]+ch->pcdata->learning_rasz[ ls->sn ]), ls->payment * RATTING_SILVER ), TRUE ) //Raszer, zeby nie kupowali hurtowo za ta sama cene plusa
                        );
                }
            }
            else
            {
                if ( ch->pcdata->learned[ ls->sn ] < ls->min || ch->pcdata->learned[ ls->sn ] > ls->max )
                {
                    sprintf( buf, "%-37s{x{x ", skill_table[ ls->sn ].name );
                }
                else
                {
                    sprintf( buf, "%s%-25s{x             ",
                            skill_table[ ls->sn ].spell_fun == spell_null ? "{G" : "{B",
                            skill_table[ ls->sn ].name );
                }
            }

            send_to_char( buf, ch );
            //if ( ++col % 2 == 0 )
            send_to_char( "\n\r", ch );
		}

		if ( col % 2 != 0 )
			send_to_char( "\n\r", ch );

		if ( !learn )
			act( "{k$N mówi ci '{KNiczego siê tu nie nauczysz.{k'{x", ch, NULL, mob, TO_CHAR );
		return ;
	}
	else /*czyli co¶ se wpisal*/
	{
		if ( ( sn = find_skill( ch, argument, FALSE ) ) < 0 )
		{
			act( "{k$N mówi ci '{KCzego chcesz siê nauczyæ?{k'{x", ch, NULL, mob, TO_CHAR );
			return ;
		}

		for ( ls = ldmob->list ; ls ; ls = ls->next )
        {
			if ( sn == ls->sn )
			{
				oksn = TRUE;
				min = ls->min;
				max = ls->max;
				chance = ls->chance;
				payment = ls->payment * RATTING_SILVER;
				pay_from = ls->pay_from;
				break;
			}
        }
        //raszer, testing
    if(IS_IMMORTAL( ch )) print_char( ch, "Ilo¶æ plusów: %d , poziom wyuczenia: %d\n\r", ch->pcdata->learning_rasz[ sn ],  ch->pcdata->learned[ sn ]);
		if ( oksn && skill_table[ ls->sn ].spell_fun == spell_null )
		{
			//wstawka systemu uczenia siê masterow
			if ( ls->sn == gsn_spear_mastery ||
					ls->sn == gsn_staff_mastery ||
					ls->sn == gsn_polearm_mastery ||
					ls->sn == gsn_shortsword_mastery ||
					ls->sn == gsn_whip_mastery ||
					ls->sn == gsn_axe_mastery ||
					ls->sn == gsn_sword_mastery ||
					ls->sn == gsn_flail_mastery ||
					ls->sn == gsn_mace_mastery ||
					ls->sn == gsn_dagger_mastery )
			{
				if ( get_skill( ch, ls->sn ) <= 0 )
				{
					oksn = FALSE;
					weaponskill_max = get_skill(ch,gsn_staff);
					weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_sword));
					weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_shortsword));
					weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_whip));
					weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_polearm));
					weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_axe));
					weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_spear));
					weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_flail));
					weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_mace));
					weaponskill_max = UMAX( weaponskill_max, get_skill(ch,gsn_dagger));
					if ( weaponskill_max > 50 )
					{
						if ( weaponskill_max == get_skill(ch,gsn_spear		) && ls->sn == gsn_spear_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_staff		) && ls->sn == gsn_staff_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_polearm	) && ls->sn == gsn_polearm_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_shortsword	) && ls->sn == gsn_shortsword_mastery 		) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_whip		) && ls->sn == gsn_whip_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_axe		) && ls->sn == gsn_axe_mastery				) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_sword		) && ls->sn == gsn_sword_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_flail		) && ls->sn == gsn_flail_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_mace		) && ls->sn == gsn_mace_mastery 			) oksn = TRUE;
						if ( weaponskill_max == get_skill(ch,gsn_dagger		) && ls->sn == gsn_dagger_mastery 			) oksn = TRUE;
						if ( get_skill(ch,gsn_spear_mastery) > 0 ) oksn = FALSE;
						if ( get_skill(ch,gsn_staff_mastery) > 0 ) oksn = FALSE;
						if ( get_skill(ch,gsn_polearm_mastery ) > 0 ) oksn = FALSE;
						if ( get_skill(ch,gsn_shortsword_mastery) > 0 ) oksn = FALSE;
						if ( get_skill(ch,gsn_whip_mastery) > 0 ) oksn = FALSE;
						if ( get_skill(ch,gsn_axe_mastery) > 0 ) oksn = FALSE;
						if ( get_skill(ch,gsn_sword_mastery) > 0 ) oksn = FALSE;
						if ( get_skill(ch,gsn_flail_mastery) > 0 ) oksn = FALSE;
						if ( get_skill(ch,gsn_mace_mastery) > 0 ) oksn = FALSE;
						if ( get_skill(ch,gsn_dagger_mastery) > 0 ) oksn = FALSE;
					}
				}
			}
		}

		if ( oksn && skill_table[ ls->sn ].spell_fun == spell_null && ls->sn == gsn_undead_resemblance && ch->class == CLASS_MAG && ch->pcdata->mage_specialist != 7 )
        {
			oksn = FALSE;
        }

		if ( !oksn )
		{
			act( "{k$N mówi ci '{KNiestety, nie potrafiê tego uczyæ.{k'{x", ch, NULL, mob, TO_CHAR );
			return ;
		}

		if ( ch->level < skill_table[ sn ].skill_level[ ch->class ] )
		{
			act( "{k$N mówi ci '{KTo jest jeszcze zbyt skomplikowane dla ciebie.{k'{x", ch, NULL, mob, TO_CHAR );
			return ;
		}
		if ( skill_table[ sn ].affect_desc && !str_cmp( skill_table[ sn ].affect_desc, "shapeshift" ) == TRUE)
		{
			if ( ch->pcdata->learned[ sn ] >= 1 )
			{
				act( "{k$N mówi ci '{KJu¿ znasz t± przemianê.{k'{x", ch, NULL, mob, TO_CHAR );
				return ;
			}
			ch->pcdata->learned[ sn ] = 1;
			act( "{B$N uczy ciê nowej przemiany '$t'.{x", ch, skill_table[ sn ].name, mob, TO_CHAR );
			act( "{B$N uczy $c nowej przemiany '$t'.{x", ch, skill_table[ sn ].name, mob, TO_ROOM );

			return ;
		}

		save_debug_info( "do_learn", NULL, "po te¶cie na przemianê", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_INFO, TRUE );

		if ( skill_table[ ls->sn ].spell_fun != spell_null )
		{
			if ( ch->pcdata->learned[ sn ] > 0 )
			{
				act( "{k$N mówi ci '{KJu¿ znasz to zaklêcie.{k'{x", ch, NULL, mob, TO_CHAR );
				return ;
			}

			if ( !can_learn_spell( ch, sn ) )
			{
				act( "{k$N mówi ci '{KWygl±da na to, ¿e tego zaklêcia nie mo¿esz siê nauczyæ.{k'{x", ch, NULL, mob, TO_CHAR );
				return ;
			}

			if ( !can_learn_here( ch, mob, sn ) )
			{
				act( "{k$N mówi ci '{KSpróbuj pó¼niej, jak zdobêdziesz wiêcej do¶wiadczenia.{k'{x", ch, NULL, mob, TO_CHAR );
				return ;
			}
		}

		if ( skill_table[ ls->sn ].spell_fun == spell_null )
		{
			/* siê uczymy od poczatku */
			if ( ch->pcdata->learned[ sn ] == 0 && ls->min == 0 )
			{
				ch->pcdata->learned[ sn ] = 2;
				ch->pcdata->learning[ sn ] = 123;//w nowym systemie stare plusy ustawiamy na 123 zeby moc rozpoznacz ktorym systemem jedziemy, Raszer
				ch->pcdata->learning_rasz[ sn ] = 0;
				ch->pcdata->learning_old[ sn ] = 0;// kopia starych plusow, a ze jedziem nowym systemem, to ich nie ma
				if ( ch->pcdata->learning_rasz[ sn ] < 0 || ch->pcdata->learning[ sn ] < 0)
					log_string( "do_learn(1): bug z learning" );

				act( "{G$N uczy ciê umiejêtno¶ci '$t'.{x", ch, skill_table[ sn ].name, mob, TO_CHAR );
				act( "$N uczy $c umiejêtno¶ci '$t'.", ch, skill_table[ sn ].name, mob, TO_ROOM );
				return ;
			}
           //raszer, zmiany zwiazane z systemem uczenia sie skilli
			/* cwiczymy dalej umiejetnosc */
			if ( ch->pcdata->learned[ sn ] > 0
					&& (ch->pcdata->learned[ sn ] + ch->pcdata->learning_rasz[ sn ]) >= min
					&& (ch->pcdata->learned[ sn ] + ch->pcdata->learning_rasz[ sn ]) <= max )
			{
                 //Raszer, okreslamy maks ilosc plusow jaka naraz moze miec postac (dla kazdego skilla)
                 wis = get_curr_stat(ch,STAT_WIS);
                 intel = get_curr_stat(ch,STAT_INT);
                 max_plus = 6;
                 if((wis+intel)/2 > 40) max_plus++;
                 if((wis+intel)/2 > 60) max_plus++;
                 if((wis+intel)/2 > 80) max_plus++;
                 if((wis+intel)/2 > 100) max_plus++;
                 if((wis+intel)/2 > 120) max_plus++;
                 if((wis+intel)/2 > 140) max_plus++;
                 
				if ( ch->pcdata->learning_rasz[ sn ] < max_plus )
				{
					if ( payment > 0 && (ch->pcdata->learned[ sn ]+ch->pcdata->learning_rasz[ sn ]) >= pay_from )
					{
						tmp = GET_LEARN_PAYMENT( (ch->pcdata->learned[ sn ])+ch->pcdata->learning_rasz[ sn ], payment );
                        long int copper = money_count_copper( ch );
						if ( copper >= tmp )
						{
							append_file_format_daily( ch, MONEY_LOG_FILE,
									"-> S: %d %d (%d), B: %d %d (%d) - zap³aci³ za nauke skilla u ticzera [%5d]",
									copper, copper - tmp, -tmp,
									ch->bank, ch->bank, 0,
									mob->pIndexData->vnum );
                            money_reduce ( ch, tmp );
							//ch->pcdata->learned[ sn ] ++; teraz dostajemy plusy
							ch->pcdata->learning_rasz[ sn ] ++;
							if ( ch->pcdata->learning_rasz[ sn ] < 0 )
                            {
								log_string( "do_learn(2): bug z learning" );
                            }
							sprintf( buf, "{G$N daje ci parê wskazówek do umiejêtno¶ci '$t' w zamian za {R%s{G.{x", money_string ( tmp, FALSE ) );
							act( buf, ch, skill_table[ sn ].name, mob, TO_CHAR );
							act( "$N uczt $c nowych tajników umiejêtno¶ci '$t' w zamian za gar¶æ monet.", ch, skill_table[ sn ].name, mob, TO_ROOM );
						}
						else
						{
							sprintf( buf, "{k$N mówi ci '{KPrzykro mi, ale za naukê umiejêtno¶ci '{Y$t{G' musisz zap³aciæ {R%s{G, a nie masz tyle przy sobie.{k'{x", money_string ( tmp, FALSE ) );
							act( buf, ch, skill_table[ sn ].name, mob, TO_CHAR );
						}
						return ;
					}
					else
					{
						if ( payment > 0 )
						{
							tmp = UMIN( (max_plus - ch->pcdata->learning_rasz[ sn ]), pay_from - (ch->pcdata->learned[ sn ] + ch->pcdata->learning_rasz[ sn ]));
							//ch->pcdata->learned[ sn ] += tmp;
							if(tmp > 0)
							{
							       ch->pcdata->learning_rasz[ sn ] += tmp;
                                 if ( ch->pcdata->learning_rasz[ sn ] < 0 )
								    log_string( "do_learn(3): bug z learning" );
								
                                 act( "{G$N uczy ciê nowych tajników umiejêtno¶ci '$t'.{x", ch, skill_table[ sn ].name, mob, TO_CHAR );
							     act( "$N uczy $c nowych tajników umiejêtno¶ci '$t'.", ch, skill_table[ sn ].name, mob, TO_ROOM );
                            }
							if ( (ch->pcdata->learned[ sn ] + ch->pcdata->learning_rasz[ sn ]) == pay_from )
                            {
								act( "{k$N mówi ci '{KZa dalsze moje nauki umiejêtno¶ci '$t' bêdzie pobierana op³ata.{k'{x", ch, skill_table[ sn ].name, mob, TO_CHAR );
                            }
                            if ( ch->pcdata->learning_rasz[ sn ] > max_plus)
                            {
                                 act( "{k$N mówi ci '{KId¼ najpierw wykorzystaj posiadan± wiedzê w praktyce.{k'{x", ch, skill_table[ sn ].name, mob, TO_CHAR );
                            
                            }
						}
						else
						{
							//tmp = UMIN( (10 - ch->pcdata->learning_rasz[ sn ]), max - (ch->pcdata->learned[ sn ] + 1 + ch->pcdata->learning_rasz[ sn ]));
							//ch->pcdata->learned[ sn ] += tmp;
							if ( ch->pcdata->learning_rasz[ sn ] > max_plus)
                            {
                                 act( "{k$N mówi ci '{KId¼ najpierw wykorzystaj posiadan± wiedzê w praktyce.{k'{x", ch, skill_table[ sn ].name, mob, TO_CHAR );
                            
                            }else
                            {
							ch->pcdata->learning_rasz[ sn ] += 1;
							if ( ch->pcdata->learning_rasz[ sn ] < 0 )
                            {
								log_string( "do_learn(4): bug z learning" );
                            }
							act( "{G$N uczy ciê paru wskazówek do umiejêtno¶ci '$t'.{x", ch, skill_table[ sn ].name, mob, TO_CHAR );
							act( "$N uczy $c  paru wskazówek do umiejêtno¶ci '$t'.", ch, skill_table[ sn ].name, mob, TO_ROOM );
							if ( (ch->pcdata->learned[ sn ]+ch->pcdata->learning_rasz[ sn ]) > max )
                            {
								act( "{k$N mówi ci '{KNiestety, to wszystko, czego mogê nauczyæ ciê o umiejêtno¶ci '$t'.{k'{x", ch, skill_table[ sn ].name, mob, TO_CHAR );
                            }
                            }
						}
					}
				}
				else
				{
					act( "{k$N mówi ci '{KId¼ najpierw wykorzystaj posiadan± wiedzê w praktyce.{k'{x", ch, skill_table[ sn ].name, mob, TO_CHAR );
					return ;
				}
			}
			else if ( (ch->pcdata->learned[ sn ]+ch->pcdata->learning_rasz[ sn ]) < min )
				act( "{k$N mówi ci '{KPoducz siê umiejêtno¶ci '$t' u kogo¶ innego i wróc wtedy do mnie, a nauczê ciê o wiele wiêcej.{k'{x", ch, skill_table[ sn ].name, mob, TO_CHAR );
			else if ( (ch->pcdata->learned[ sn ]+ch->pcdata->learning_rasz[ sn ]) > max )
				act( "{k$N mówi ci '{KJe¶li chodzi o umiejêtno¶æ '$t' to umiesz ju¿ wiêcej ode mnie.{k'{x", ch, skill_table[ sn ].name, mob, TO_CHAR );
		}
		else
		{
			if ( ch->pcdata->learned[ sn ] == 0 )
			{
				if ( chance < 0 )
					chance_mod = ( 25 * chance ) / 100;
				else
					chance_mod = ( 15 * chance ) / 100;

				if ( number_percent() < ( chance_to_learn_spell( ch, sn ) + URANGE( -25, chance_mod, 15 ) ) )
				{
					ch->pcdata->learned[ sn ] = 1;
					act( "{B$N uczy ciê zaklêcia '$t'.{x", ch, skill_table[ sn ].name, mob, TO_CHAR );
					act( "{B$N uczy $c zaklêcia '$t'.{x", ch, skill_table[ sn ].name, mob, TO_ROOM );
					return ;
				}
				else
				{
					act( "{RNie uda³o ci siê nauczyæ zaklêcia $t.{x", ch, skill_table[ sn ].name, NULL, TO_CHAR );
					add_spells_failed( ch, mob, sn );
					return ;
				}
			}
			else
			{
				act( "{k$N mówi ci '{KJu¿ znasz to zaklêcie.{k'{x", ch, skill_table[ sn ].name, mob, TO_CHAR );
				return ;
			}
		}
	}
	return ;
}


char* is_name_reserved( char *name )
{
	char strsave[ MAX_INPUT_LENGTH ];
	char pwd[ MAX_INPUT_LENGTH ];
	FILE *fp;

	fclose( fpReserve );
	sprintf( strsave, "%s%s", RESERVATION_DIR, capitalize( name ) );
	if ( ( fp = fopen( strsave, "r" ) ) != NULL )
	{
		sprintf( pwd, "%s", fread_string( fp ) );
		fclose( fp );
		fpReserve = fopen( NULL_FILE, "r" );
		if ( pwd[ 0 ] != '\0' )
			return str_dup( pwd );
		else
			return NULL;
	}
	else
	{
		fpReserve = fopen( NULL_FILE, "r" );
		return NULL;
	}
}

void do_reservation( CHAR_DATA *ch, char *argument )
{
	char strsave[ MAX_INPUT_LENGTH ];
	bool was_reserved;
	FILE *fp;

	if( IS_NPC(ch)) return;//mobom dziêkujemy

	if ( ( EXT_IS_SET( ch->act, PLR_UNREGISTERED ) && !IS_IMMORTAL( ch ) )
			|| ch->level < 2 )
	{
		send_to_char( "Musisz mieæ co najmniej drugi poziom do¶wiadczenia i zaakceptowane imiê.\n\r", ch );
		return ;
	}

	was_reserved = FALSE;
	fclose( fpReserve );
	sprintf( strsave, "%s%s", RESERVATION_DIR, capitalize( ch->name ) );
	if ( ( fp = fopen( strsave, "r" ) ) != NULL )
	{
		was_reserved = TRUE;
		fclose( fp );
		unlink( strsave );
	}
	else
    {
		was_reserved = FALSE;
    }

	if ( argument[ 0 ] == '\0' || !str_cmp( argument, "yes" ) )
	{
		if ( ( fp = fopen( strsave, "w" ) ) != NULL )
		{
			fprintf( fp, "%s~\n\r", ch->pcdata->pwd );

			if ( was_reserved )
				send_to_char( "Ponownie rezerwujesz swoj± postaæ.\n\r", ch );
			else
				send_to_char( "Rezerwujesz swoj± postaæ.\n\r", ch );
			fclose( fp );
		}
		else
			send_to_char( "Podczas rezerwacji wyst±pi³ b³±d. Spróbuj pó¼niej.\n\r", ch );
	}
	else if ( !str_cmp( argument, "no" ) )
	{
		sprintf( strsave, "%s%s", RESERVATION_DIR, capitalize( ch->name ) );
		unlink( strsave );
		if ( was_reserved )
			send_to_char( "Rezygnujesz z rezerwacji swojej postaci.\n\r", ch );
		else
			send_to_char( "Nie rezerwowa³e¶ swojej postaci wcze¶niej.\n\r", ch );
	}
	fpReserve = fopen( NULL_FILE, "r" );
}

void do_condition( CHAR_DATA *ch, char *argument )
{
	AFFECT_DATA *paf;
	int prct;

	if( IS_NPC(ch)) return;//mobom dziêkujemy

	send_to_char( "Informacje o twoim stanie:\n\r", ch );

	if ( get_carry_weight( ch ) > ( 3 * can_carry_w( ch ) / 4 ) )
		send_to_char( "{RNoszone przez ciebie przedmioty sporo wa¿±, utrudnia ci to troche podró¿owanie.{x\n\r", ch );

	switch ( ch->position )
	{
		case POS_DEAD:
			send_to_char( "{CNIE ¯YJESZ!{x\n\r", ch );
			break;

		case POS_MORTAL:
			send_to_char( "{RJeste¶ ¶miertelnie rann<&y/a/e>.{x\n\r", ch );
			break;

		case POS_INCAP:
			send_to_char( "{RJeste¶ unieruchomion<&y/a/e>.{x\n\r", ch );
			break;

		case POS_STUNNED:
			send_to_char( "{RJeste¶ nieprzytomn<&y/a/e>.{x\n\r", ch );
			break;

		case POS_SLEEPING:
			if ( ch->on != NULL )
			{
				if ( IS_SET( ch->on->value[ 2 ], SLEEP_AT ) )
					print_char( ch, "{C¦pisz przy %s.{x\n\r", ch->on->name6 );
				else if ( IS_SET( ch->on->value[ 2 ], SLEEP_ON ) )
					print_char( ch, "{C¦pisz na %s.{x\n\r", ch->on->name6 );
				else
					print_char( ch, "{C¦pisz w %s.{x\n\r", ch->on->name6 );
			}
			else
				send_to_char( "{C¦pisz sobie.{x\n\r", ch );
			break;

		case POS_RESTING:
			if ( ch->on != NULL )
			{
				if ( IS_SET( ch->on->value[ 2 ], REST_AT ) )
					print_char( ch, "{COdpoczywasz przy %s.{x\n\r", ch->on->name6 );
				else if ( IS_SET( ch->on->value[ 2 ], REST_ON ) )
					print_char( ch, "{COdpoczywasz na %s.{x\n\r", ch->on->name6 );
				else
					print_char( ch, "{COdpoczywasz w %s.{x\n\r", ch->on->name6 );
			}
			else
				send_to_char( "{COdpoczywasz sobie.{x\n\r", ch );
			if ( ch->memming )
				send_to_char( "{CJeste¶ w trakcie zapamiêtywania czarów.{x\n\r", ch );
			break;

		case POS_SITTING:
			if ( ch->on != NULL )
			{
				if ( IS_SET( ch->on->value[ 2 ], SIT_AT ) )
					print_char( ch, "{CSiedzisz przy %s.{x\n\r", ch->on->name6 );
				else if ( IS_SET( ch->on->value[ 2 ], SIT_ON ) )
					print_char( ch, "{CSiedzisz na %s.{x\n\r", ch->on->name6 );
				else if ( ch->on->item_type == ITEM_MUSICAL_INSTRUMENT )
					print_char( ch, "{CSiedzisz przy %s.{x\n\r", ch->on->name6 );
				else
					print_char( ch, "{CSiedzisz w %s.{x\n\r", ch->on->name6 );
			}
			else
				send_to_char( "{CSiedzisz sobie.{x\n\r", ch );
			break;

		case POS_STANDING:
			if ( !ch->mount )
			{
				if ( ch->on != NULL )
				{
					if ( IS_SET( ch->on->value[ 2 ], STAND_AT ) )
						print_char( ch, "{CStoisz przy %s.{x\n\r", ch->on->name6 );
					else if ( IS_SET( ch->on->value[ 2 ], STAND_ON ) )
						print_char( ch, "{CStoisz na %s.{x\n\r", ch->on->name6 );
					else
						print_char( ch, "{CStoisz w %s.{x\n\r", ch->on->name6 );
                }
                else
                {
                    if ( IS_AFFECTED( ch, AFF_FLYING ) || IS_AFFECTED( ch, AFF_FLOAT ) )
                    {
                        send_to_char( "{CUnosisz siê bez ruchu.{x\n\r", ch );
                    }
                    else
                    {
                        send_to_char( "{CStoisz sobie.{x\n\r", ch );
                    }
                }
			}
			else
			{
				print_char( ch, "{CDosiadasz %s.{x\n\r", PERS4( ch->mount, ch ) );
			}
			break;

		case POS_FIGHTING:
			if ( ch->fighting )
				send_to_char( "{RWalczysz.{c\n\r", ch );
			else
				print_char( ch, "{RWalczysz z %s.{c\n\r", PERS5( ch->fighting, ch ) );
			break;
	}

	if ( !IS_NPC( ch ) && ch->condition[ COND_DRUNK ] > DRUNK_FULL )
		send_to_char( "{RJeste¶ pijan<&y/a/e>.{x\n\r", ch );

	if ( !IS_NPC( ch ) && ch->condition[ COND_THIRST ] == 0 )
		send_to_char( "{RChce ci siê piæ.{x\n\r", ch );

	if ( !IS_NPC( ch ) && ch->condition[ COND_HUNGER ] == 0 )
		send_to_char( "{RJeste¶ g³odn<&y/a/e>.{x\n\r", ch );

	if ( !IS_NPC( ch ) && ch->condition[ COND_SLEEPY ] == 0 )
		send_to_char( "{RJeste¶ zmêczon<&y/a/e> i senn<&y/a/e>.{x\n\r", ch );

	if ( !IS_NPC( ch ) && is_affected( ch, gsn_on_smoke) )
	{
		if ( IS_AFFECTED( ch, AFF_HALLUCINATIONS_POSITIVE) || IS_AFFECTED( ch, AFF_HALLUCINATIONS_NEGATIVE))
			send_to_char( "{gJeste¶ spalon<&y/a/e>.{x\n\r", ch );
		else
			send_to_char( "{gPopalasz sobie ziele.{x\n\r", ch );
	}

	if ( ( paf = affect_find( ch->affected, gsn_holy_pool )) != NULL )
	{
		// procent wypelnienia puli zamieniony na ilo¶æ punktów w puli, Rasz_pray
		prct =  paf->modifier;

		if ( prct > 80 )
			send_to_char( "{GJeste¶ przepe³nion<&y/a/e> ¶wiêt± moc±.{x\n\r", ch );
		else if ( prct > 65 )
			send_to_char( "{GJeste¶ wype³nion<&y/a/e> ¶wiêt± moc±.{x\n\r", ch );
		else if ( prct > 40 )
			send_to_char( "{GCzujesz w sobie fale ¶wiêtej mocy.{x\n\r", ch );
		else if ( prct > 20 )
			send_to_char( "{GCzujesz w sobie ¶wiêt± moc.{x\n\r", ch );
		else if ( prct > 0 )
			send_to_char( "{GCzujesz w sobie iskrê ¶wietej mocy.{x\n\r", ch );
		else if ( prct == 0 )
			send_to_char( "{CNie czujesz w sobie ¶wiêtej mocy.{x\n\r", ch );
		else if ( prct > -25 )
			send_to_char( "{RTwój bog ci nie sprzyja.{x\n\r", ch );
		else if ( prct > -50 )
			send_to_char( "{RCzujesz niechêc swojego boga.{x\n\r", ch );
		else if ( prct > -75 )
			send_to_char( "{RCzujesz z³osc swojego boga.{x\n\r", ch );
		else if ( prct > -100 )
			send_to_char( "{RCzujesz w¶ciek³o¶æ swojego boga.{x\n\r", ch );
		else if ( prct <= -100 )
			send_to_char( "{RJeste¶ przeklêt<&y/a/e> przez swojego boga.{x\n\r", ch );
	}

	if ( ( paf = affect_find( ch->affected, gsn_deeds_pool )) != NULL )
	{
		// procent zape³nienia deed poola przez ucznki zamieniony na ilo¶æ deedow, Rasz_pray
		prct = paf->modifier;

		if ( prct == 30 )
			send_to_char( "{GTwoje dobre uczynki sprawiaj±, ¿e czujesz siê nieskazitelnie czyst<&y/a/e>.{x\n\r", ch );
		else if ( prct > 25 )
			send_to_char( "{GTwoje dobre uczynki sprawiaj±, ¿e czujesz siê wy¶mienicie.{x\n\r", ch );
		else if ( prct > 20 )
			send_to_char( "{GTwoje dobre uczynki sprawiaj±, ¿e czujesz siê wspaniale.{x\n\r", ch );
		else if ( prct > 10 )
			send_to_char( "{GTwoje dobre uczynki sprawiaj±, ¿e czujesz siê bardzo dobrze.{x\n\r", ch );
		else if ( prct > 0 )
			send_to_char( "{GTwoje dobre uczynki sprawiaj±, ¿e czujesz siê dobrze.{x\n\r", ch );
		else if ( prct == 0 )
			send_to_char( "{CCzujesz siê normalnie.{x\n\r", ch );
		else if ( prct > -15 )
			send_to_char( "{RTwoje z³e uczynki sprawiaj±, ¿e czujesz siê nieswojo.{x\n\r", ch );
		else if ( prct > -30 )
			send_to_char( "{RTwoje z³e uczynki sprawiaj±, ¿e czujesz siê podle.{x\n\r", ch );
		else if ( prct > -45 )
			send_to_char( "{RTwoje z³e uczynki sprawiaj±, ¿e czujesz siê nieczyst<&y/a/e>.{x\n\r", ch );
		else if ( prct > -60 )
			send_to_char( "{RTwoje z³e uczynki sprawiaj±, ¿e czujesz siê zbrukan<&y/a/e>.{x\n\r", ch );
		else if ( prct == -60 )
			send_to_char( "{RTwoje z³e uczynki sprawiaj±, ¿e czujesz siê niegodn<&y/a/e> bycia paladynem.{x\n\r", ch );
	}

	return ;
}

void do_moon_phase (CHAR_DATA *ch, char *argument)
{
	ROOM_INDEX_DATA *in_room;

	if( IS_NPC(ch)) return;//mobom dziêkujemy

	if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
		in_room = ch->pcdata->mind_in;
	else
		in_room = ch->in_room;

	if ( !IS_OUTSIDE( ch )
			|| IS_SET( sector_table[ in_room->sector_type ].flag, SECT_NOWEATHER ) )
	{
		send_to_char( "Nie widaæ nieba.\n\r", ch );
		return ;
	}

    if (
            !IS_NPC( ch )
            && !EXT_IS_SET( ch->act, PLR_HOLYLIGHT )
            && !IS_AFFECTED( ch, AFF_DARK_VISION )
            && room_is_dark(ch, ch->in_room )
            && EXT_IS_SET( in_room->room_flags, ROOM_MAGICDARK ) )
    {
        send_to_char( "Jest tu nienaturalnie ciemno, nie jeste¶ w stanie niczego dostrzec.\n\r", ch );
        return;
    }

	if ( time_info.hour < HOUR_NIGHT_BEGIN && time_info.hour >= HOUR_DAY_BEGIN )
	{
		if ( weather_info[ in_room->sector_type ].sky >= 1 )
			send_to_char( "Spogl±dasz na ksiê¿yc i w³asnie do ciebie dotar³o, ¿e jest dzieñ.\n\r", ch );
		else
			send_to_char( "Spogl±dasz na ksiê¿yc i widzisz piêkne, z³ote s³oñce...\n\r", ch );

		return ;
	}

	if (( weather_info[ in_room->sector_type ].sky >= 1 ) && (number_range(1,3) > 1))
	{
		send_to_char( "Spogl±dasz z nadziej± na ksiê¿yc, lecz niestety w³a¶nie zas³aniaj± go chmury.\n\r", ch );
		return;
	}

	// and finally he looked upon the moon...

	if ( time_info.moon_phase == MOON_NEW_MOON )
	{
		send_to_char("{xKsiê¿yc obecnie znajduje siê w Nowiu.\n\r\n\r", ch);
		send_to_char("{x         {y_..._{x           \n\r", ch);
		send_to_char("{x       {y.{D:::::::{y.{x         \n\r", ch);
		send_to_char("{x      {y:{D:::::::::{y:{x        \n\r", ch);
		send_to_char("{x      {y:{D:::::::::{y:{x        \n\r", ch);
		send_to_char("{x      {y`{D:::::::::{y'{x        \n\r", ch);
		send_to_char("{x        {y`':::'{y'{x          \n\r\n\r", ch);
	}
	else if ( time_info.moon_phase == MOON_WAXING_CRESCENT )
	{
		send_to_char("{xKsiê¿yc znajduje siê w fazie M³odego Ksiê¿yca.\n\r\n\r", ch);
		send_to_char("{x        {D_{Y..._{x\n\r", ch);
		send_to_char("{x      {D.:::{Y:. `.{x\n\r", ch);
		send_to_char("{x     {D:::::::{Y.  :{x \n\r", ch);
		send_to_char("{x     {D:::::::{Y:  :{x\n\r", ch);
		send_to_char("{x     {D`::::::{Y' .'{x\n\r", ch);
		send_to_char("{x       {D`':{Y.:-'{x\n\r\n\r", ch);
	}
	else if ( time_info.moon_phase == MOON_FIRST_QUARTER )
	{
		send_to_char("{xKsiê¿yc obecnie znajduje siê w Pierwszej Kwadrze.\n\r\n\r", ch);
		send_to_char("{x        {D_.{Y.._{x\n\r", ch);
		send_to_char("{x      {D.:::{Y:  `.{x\n\r", ch);
		send_to_char("{x     {D:::::{Y:    :{x\n\r", ch);
		send_to_char("{x     {D:::::{Y:    :{x\n\r", ch);
		send_to_char("{x     {D`::::{Y:   .'{x\n\r", ch);
		send_to_char("{x       {D`':{Y:.-'{x\n\r\n\r", ch);
	}
	else if ( time_info.moon_phase == MOON_WAXING_GIBBOUS )
	{
		send_to_char("{xKsiê¿yc znajduje siê w fazie Ksiê¿yca Garbatego.\n\r\n\r", ch);
		send_to_char("{x        {D_.{Y.._{x\n\r", ch);
		send_to_char("{x      {D.:{Y:'   `.{x\n\r", ch);
		send_to_char("{x     {D::{Y:       :{x\n\r", ch);
		send_to_char("{x     {D::{Y:       :{x\n\r", ch);
		send_to_char("{x     {D`:{Y:.     .'{x\n\r", ch);
		send_to_char("{x       {D`'{Y:..-'{x\n\r\n\r", ch);
	}
	else if ( time_info.moon_phase == MOON_FULL_MOON )
	{
		send_to_char("{xKsiê¿yc jest w Pe³ni.\n\r\n\r", ch);
		send_to_char("         {Y_..._{x           \n\r", ch);
		send_to_char("       {Y.:::::::.{x         \n\r", ch);
		send_to_char("      {Y:::::::::::{x        \n\r", ch);
		send_to_char("      {Y:::::::::::{x        \n\r", ch);
		send_to_char("      {Y`:::::::::'{x        \n\r", ch);
		send_to_char("        {Y`':::''{x          \n\r\n\r", ch);

	}
	else if ( time_info.moon_phase == MOON_WANING_GIBBOUS )
	{
		send_to_char("{xKsiê¿yc znajduje siê w fazie Ksiê¿yca Garbatego.\n\r\n\r", ch);
		send_to_char("{x        {Y_...{D_{x\n\r", ch);
		send_to_char("{x      {Y.'    `{D::.{x\n\r", ch);
		send_to_char("{x     {Y:       :{D:::{x\n\r", ch);
		send_to_char("{x     {Y:       :{D:::{x\n\r", ch);
		send_to_char("{x     {Y`.      .{D::'{x\n\r", ch);
		send_to_char("{x       {Y`-...'{D'{x\n\r\n\r", ch);
	}
	else if ( time_info.moon_phase == MOON_LAST_QUARTER )
	{
		send_to_char("{xKsiê¿yc obecnie znajduje siê w Trzeciej Kwadrze.\n\r\n\r", ch);
		send_to_char("{x        {Y_..{D._{x\n\r", ch);
		send_to_char("{x      {Y.'  :{D:::.{x\n\r", ch);
		send_to_char("{x     {Y:    :{D:::::{x\n\r", ch);
		send_to_char("{x     {Y:    :{D:::::{x\n\r", ch);
		send_to_char("{x     {Y`.   :{D::::'{x\n\r", ch);
		send_to_char("{x       {Y`-.:{D:''{x\n\r\n\r", ch);
	}
	else if ( time_info.moon_phase == MOON_WANING_CRESCENT )
	{
		send_to_char("{xKsiê¿yc znajduje siê w fazie Starego Ksiê¿yca.\n\r\n\r", ch);
		send_to_char("{x        {Y_..{D._{x\n\r", ch);
		send_to_char("{x      {Y.' .{D::::.{x\n\r", ch);
		send_to_char("{x     {Y:  :{D:::::::{x\n\r", ch);
		send_to_char("{x     {Y:  :{D:::::::{x\n\r", ch);
		send_to_char("{x     {Y`. ':{D:::::'{x\n\r", ch);
		send_to_char("{x       {Y`-.:{D::'{x\n\r\n\r", ch);
	}
	return;
}


void do_introduce( CHAR_DATA* ch, char* argument )
{
	char arg1 [ MAX_INPUT_LENGTH ];
	char arg2 [ MAX_INPUT_LENGTH ];
	char* argall;
	int sn = 0, chance = 0, payment = 0;
	bool oksn = FALSE, learn = FALSE, found = FALSE;
	CHAR_DATA *mob;
	LEARN_TRICK_DATA *ld = NULL, *ldmob = NULL;
	LEARN_TRICK_LIST *ls = NULL;

	if( IS_NPC(ch)) return;//mobom dziêkujemy

	argall = argument;

	/* dajemy standardowy tekst jak go¶æ jest pod wp³ywem 'silence' */
	if ( IS_AFFECTED( ch, AFF_SILENCE ) )
	{
		send_to_char( AFF_SILENCE_TEXT, ch );
		return ;
	}

	for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
		if ( IS_NPC( mob ) && EXT_IS_SET( mob->act, ACT_PRACTICE ) )
			break;

	if ( mob == NULL )
	{
		send_to_char( "Nie ma tutaj nikogo, kto móg³by ciê czego¶ nauczyæ.\n\r", ch );
		return ;
	}

	for ( ld = learn_trick_system ; ld ; ld = ld->next )
		if ( ld->vnum == mob->pIndexData->vnum )
		{
			ldmob = ld;
			break;
		}

	if ( !ldmob )
	{
		send_to_char( "Nie widzisz tutaj nikogo, kto móg³by ciê czego¶ nauczyæ.\n\r", ch );
		return ;
	}

	if ( ch->precommand_fun && !ch->precommand_pending )
	{
		send_to_char( "Robisz teraz co¶ innego.\n\r", ch );
		return ;
	}

	if ( !ch->precommand_pending && HAS_TRIGGER( mob, TRIG_PRECOMMAND ) )
	{
		if ( mp_precommand_trigger( mob, ch, NULL, &do_introduce, "introduce", argall ) )
			return ;
	}

	if (is_affected (ch, gsn_energy_drain))
	{
		act( "{k$N mówi ci '{KWidzê, ¿e nie potrafisz siê teraz skupiæ na nauce.{k'{x", ch, NULL, mob, TO_CHAR );
		return;

	}

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[ 0 ] == '\0' )
	{
		//act( "{k$N mówi ci '{KCo chcesz poznaæ?{k'{x", ch, NULL, mob, TO_CHAR );
		for ( ls = ldmob->list ; ls ; ls = ls->next )
		{
			if ( trick_table[ ls->sn ].name == NULL )
				break;

			if ( trick_table[ ls->sn ].alternate_skill1 == -1 )
			{
				if ( trick_table[ ls->sn ].skill_percent > get_skill( ch, trick_table[ ls->sn ].which_skill ) )
					continue;
			}
			else
			{
				if ( trick_table[ ls->sn ].skill_percent > get_skill( ch, trick_table[ ls->sn ].which_skill ) && trick_table[ ls->sn ].skill_percent > get_skill( ch, trick_table[ ls->sn ].alternate_skill1 ) )
					continue;
			}

			if ( (trick_table[ ls->sn ].which_skill2 != -1) && (trick_table[ ls->sn ].skill_percent > get_skill( ch, trick_table[ ls->sn ].which_skill2 )) )
				continue;

			if ( ch->pcdata->tricks[ls->sn] )
				continue;

			if ( !learn )
			{
				print_char( ch, "%s mo¿e nauczyæ nastêpuj±cych trików:\n\r", capitalize( mob->short_descr ) );
				learn = TRUE;
			}

			print_char( ch, "{G%s - {R%d{G srebrnych monet.{x\n\r", trick_table[ ls->sn ].name, ls->payment );

			found = TRUE;
		}

		if( !found )
        {
			act( "{k$N mówi ci '{KNiczego ciê nie nauczê.{k'{x", ch, NULL, mob, TO_CHAR );
        }
		return;
	}

	for ( sn = 0; sn < MAX_TRICKS; sn++ )
	{
		if( !str_prefix( arg1, trick_table[ sn ].name ) )
		{
			for ( ls = ldmob->list ; ls ; ls = ls->next )
				if ( sn == ls->sn )
				{
					oksn = TRUE;
					chance = ls->chance;
					payment = RATTING_SILVER * ls->payment;
					break;
				}

			if( !str_cmp( trick_table[ sn ].name, "reserved" ) || !oksn )
			{
				print_char( ch, "{k%s {kmówi ci:'{KTego ciê tu nie nauczê.{k'\n\r", mob->short_descr );
				return;
			}

			if( ch->pcdata->tricks[sn] == 1 )
			{
				print_char( ch, "{k%s {kmówi ci:'{KPrzecie¿ juz znasz '%s'.{k'\n\r", mob->short_descr, trick_table[ sn ].name );
				return;
			}

			if ( trick_table[ ls->sn ].alternate_skill1 == -1 )
			{
				if ( get_skill( ch, trick_table[ ls->sn ].which_skill ) < trick_table[ ls->sn ].skill_percent )
				{
					print_char( ch, "{k%s {kmówi ci:'{KNie masz wystarczajaco wytrenowanej umiejêtno¶ci '%s' by poznaæ '%s'.{k'\n\r", mob->short_descr, skill_table[ trick_table[ ls->sn ].which_skill ].name, trick_table[ ls->sn ].name );
					return;
				}
			}
			else
			{
				if ( get_skill( ch, trick_table[ ls->sn ].which_skill ) < trick_table[ ls->sn ].skill_percent && get_skill( ch, trick_table[ ls->sn ].alternate_skill1 ) < trick_table[ ls->sn ].skill_percent )
				{
					print_char( ch, "{k%s {kmówi ci:'{KNie masz wystarczajaco wytrenowanej umiejêtno¶ci '%s' b±d¼ '%s' by poznaæ '%s'.{k'\n\r", mob->short_descr, skill_table[ trick_table[ ls->sn ].which_skill ].name, skill_table[ trick_table[ ls->sn ].alternate_skill1 ].name, trick_table[ ls->sn ].name );
					return;
				}
			}


			if ( (trick_table[ ls->sn ].which_skill2 != -1) && ( get_skill( ch, trick_table[ ls->sn ].which_skill2 ) < trick_table[ ls->sn ].skill_percent ) )
			{
				print_char( ch, "{k%s {kmówi ci:'{KNie masz wystarczajaco wytrenowanej umiejêtno¶ci '%s' by poznaæ '%s'.{k'\n\r", mob->short_descr, skill_table[ trick_table[ ls->sn ].which_skill2 ].name, trick_table[ ls->sn ].name );
				return;
			}

            long int copper = money_count_copper( ch );
			if ( copper < payment )
			{
				act( "{k$N mówi ci '{KNie masz wystarczaj±cej ilo¶ci srebra.{k'{x", ch, NULL, mob, TO_CHAR );
				return;
			}

			append_file_format_daily( ch, MONEY_LOG_FILE,
					"-> S: %d %d (%d), B: %d %d (%d) - zap³aci³ za nauke triku u ticzera [%5d]",
					copper, copper - payment, -payment,
					ch->bank, ch->bank, 0,
					mob->pIndexData->vnum );

            money_reduce ( ch, payment );

			chance += int_app[ get_curr_stat_deprecated(ch, STAT_INT ) ].learn_chance;

			if ( number_percent() > chance )
			{
				print_char( ch, "Wydajesz %d srebrnych monet na naukê, ale nie udaje ci siê poznaæ %s\n\r", payment, trick_table[ ls->sn ].name );
				return;
			}

			ch->pcdata->tricks[sn] = 1;
			print_char( ch, "Poznajesz %s p³ac±æ %d srebrnych monet.\n\r", trick_table[ sn ].name, payment );
			return;
		}
	}

	act( "{k$N mówi ci '{KNie ma takiego triku.{k'{x", ch, NULL, mob, TO_CHAR );

	return;
}

int get_hp_notch ( CHAR_DATA *ch, bool usePerfectSelf )
{
	int percent = GET_PERCENT_HP(ch);

	if (usePerfectSelf && is_affected( ch, gsn_perfect_self))
	{
		return 0;
	}

	if      (percent > 99) return 0;
	else if (percent > 84) return 1;
	else if (percent > 69) return 2;
	else if (percent > 54) return 3;
	else if (percent > 39) return 4;
	else if (percent > 24) return 5;
	else if (percent > -1) return 6;
	return 7;
}

/*
  Tener

  questlogi - czyli informacje na temat zadañ, co¶ w rodzaju dzienniczka z zadañ w grach fabularnych

  Komendy dla gracza:

  questlog [current]
  questlog all
  questlog finished

  Wypisuje kolejno wpisy na temat zadañ aktualnie wykonywanych, wszystkich albo ukoñczonych

  TODO: informowanie o dacie dodania wpisu

*/

int show_questlogs_to_char( CHAR_DATA* ch, int state )
{
   QL_DATA* tmp;
   int counter = 0;

   for( tmp = ch->pcdata->ql_data; tmp != NULL; tmp = tmp->next )
      {
	 if( tmp->state == state )
	    {
	       send_to_char( "\n\r", ch );
	       send_to_char( tmp->text, ch );
	       send_to_char( tmp->state ? "{gZadanie {Gwykonane{g.{x\n\r" : "{gZadanie {Yw toku{g.{x\n\r", ch );
	       counter++;
	    }
      }
   return counter;
}

void do_questlog( CHAR_DATA *ch, char* argument )
{
   return; // Wy³±czam, bo jeszcze nie jest dokoñczone
   char komenda[ MAX_STRING_LENGTH ];

   argument = one_argument( argument, komenda );

   //mobom dziêkujemy
   if( IS_NPC(ch))
      return;

   if( ch->position == POS_SLEEPING ) // dobranoc!
      {
	 send_to_char( "¦nisz o epickich zadaniach.\n\r", ch );
	 return;
      }

   if( ! ch->pcdata->ql_data )
      {
	 send_to_char( "Nie masz w dzienniku zadañ ¿adnych wpisów.\n\r", ch);
	 return;
      }

   if ( komenda[ 0 ] == '\0' || !str_prefix( komenda, "current" ) )
      {
	 if ( ! show_questlogs_to_char( ch, QL_STATE_CURRENT ) )
	    send_to_char( "Nie masz w dzienniku zadañ ¿adnych wpisów na temat aktualnie wykonywanych zadañ.\n\r", ch );
      }
   else if ( !str_prefix( komenda, "finished" ) )
      {
	 if ( ! show_questlogs_to_char( ch, QL_STATE_FINISHED ) )
	    send_to_char( "Nie masz w dzienniku zadañ ¿adnych wpisów na temat zakoñczonych zadañ.\n\r", ch );
      }
   else if ( !str_prefix( komenda, "all" ) )
      {
	 if ( !( show_questlogs_to_char( ch, QL_STATE_CURRENT ) +
	         show_questlogs_to_char( ch, QL_STATE_FINISHED ) ) )
	    {
	       bug("do_questlog: ch->pcdata->ql_data nie jest NULL, ale mimo to nie wy¶wietlili¶my ¿adnego wpisu. Czy¿by kto¶ zapomnia³ dodaæ obs³ugi nowego stanu questloga?",0);
	    }
      }
   else
      {
	 send_to_char("Które wpisy z dziennika zadañ chcesz obejrzeæ?\n\r", ch );
      }

}

/**
 * sprwdzanie trophy komus dla W4+
 * lub sobie dla gracza
 */
void do_trophy( CHAR_DATA *ch, char *argument )
{
    int i, count = 0;
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    bool found = FALSE;
    MOB_INDEX_DATA *mob;
    char buf[ MAX_STRING_LENGTH ];
    double trophy;

    if (
            ( ch->pcdata->wiz_conf & W6 ) ||
            ( ch->pcdata->wiz_conf & W5 ) ||
            ( ch->pcdata->wiz_conf & W4 )
       )
    {
        one_argument( argument, arg );
        if ( arg[ 0 ] == '\0' )
        {
            send_to_char( "Czyje trophy chcesz sprawdziæ?\n\r", ch );
            return;
        }

        if ( ( victim = get_char_world( ch, arg ) ) == NULL )
        {
            send_to_char( "Nikogo takiego nie znaleziono.\n\r", ch );
            return;
        }

        if ( IS_NPC( victim ) )
        {
            send_to_char( "Nikogo takiego nie znaleziono.\n\r", ch );
            return;
        }

        send_to_char( "\n\r", ch );

        for ( i = 0; i < TROPHY_SIZE; i++ )
        {
            if ( !victim->pcdata->trophy[ i ][ 0 ] )
                break;

            if ( victim->pcdata->trophy[ i ][ 0 ] <= 0 || !get_mob_index( victim->pcdata->trophy[ i ][ 0 ] ) )
                break;

            if ( !found )
            {
                found = TRUE;
                print_char( ch, "\n\r%s ma nastepujace trofea:\n\r", victim->name );
            }

            trophy = victim->pcdata->trophy[ i ][ 1 ];
            trophy /= TROPHY_BASE;

            mob = get_mob_index( victim->pcdata->trophy[ i ][ 0 ] );
            sprintf
                (
                 buf,
                 "%6.2f %3d %s (%d)\n\r",
                 trophy,
                 victim->pcdata->trophy[ i ][ 1 ],
                 mob->short_descr,
                 mob->vnum
                );

            send_to_char( buf, ch );
        }

        if ( !found )
        {
            print_char( ch, "\n\r%s nic jeszcze nie zaklepa³.\n\r", victim->name );
        }
    }
    else
    {
        for ( i = 0; i < TROPHY_SIZE; i++ )
        {
            if ( !ch->pcdata->trophy[ i ][ 0 ] )
            {
                break;
            }
            if ( ch->pcdata->trophy[ i ][ 0 ] <= 0 || !get_mob_index( ch->pcdata->trophy[ i ][ 0 ] ) )
            {
                break;
            }
            if ( !found )
            {
                found = TRUE;
                print_char( ch, "\n\rOstatnie twoje trofea:\n\r" );
            }
            mob = get_mob_index( ch->pcdata->trophy[ i ][ 0 ] );
            print_char ( ch, "%s - ", mob->short_descr );
            /**
             * switch z grubsza odpowiada temu co jest w exping.c w trophy_eval
             */
            count = ch->pcdata->trophy[ i ][ 1 ] / TROPHY_BASE;
            switch ( count )
            {
                case  0:
                case  1:
                case  2:
                case  3:
                case  4:
                case  5:
                case  6:
                case  7:
                case  8:
                    send_to_char( "{bnieco{x", ch );
                    break;
                case  9:
                case 10:
                case 11:
                    send_to_char( "{Bma³o{x", ch );
                    break;
                case 12:
                case 13:
                case 14:
                case 15:
                    send_to_char( "{Cdu¿o{x", ch );
                    break;
                case 16:
                case 17:
                case 18:
                    send_to_char( "{gbardzo du¿o{x", ch );
                    break;
                case 19:
                case 20:
                case 21:
                    send_to_char( "{Gco niemiara{x", ch );
                    break;
                case 22:
                case 23:
                    send_to_char( "{Ykrocie{x", ch );
                    break;
                default:
                    send_to_char( "{Rod cholery{x", ch );
                    break;
            }
            /**
             * pokazywanie graczom ktory to boss jest ... zle
             *
             * if ( EXT_IS_SET( mob->act, ACT_BOSS ) )
             * {
             *     print_char( ch, "{D*{x" );
             * }
             */
            print_char( ch, "\n\r" );
        }
        if ( !found )
        {
            print_char( ch, "W³a¶ciwe to nie wiesz co do tej pory pad³o z twojej rêki, g³upio co?\n\r" );
        }
    }
    return;
}

