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
 * KILLER MUD is copyright 2003-2012 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Mierzwa Michal        (nil.michal@googlewave.com     ) [Rellik    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: olc_bonus_set.c 10998 2012-02-19 18:00:57Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/olc_bonus_set.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "progs.h"
#include "olc.h"
#include "recycle.h"
#include "lang.h"

#define BSEDIT( fun )           bool fun( CHAR_DATA *ch, char *argument )

char *  string_linedel      args( ( char *, int ) );
char *  string_lineadd      args( ( char *, char *, int ) );
char *  numlineas           args( ( char * ) );

const struct olc_cmd_type bsedit_table[] =
{
	{	"commands",		show_commands		},
	{	"create",		bsedit_create		},
	{	"show",			bsedit_show			},
	{	"list",			bsedit_list			},
	{	"?",			show_help			},
	{	"addaffect",	bsedit_addaffect	},
	{	"delaffect",	bsedit_delaffect	},
	{	"description",	bsedit_description	},
	{	"wearprog",		bsedit_wearprog		},
	{	"removeprog",	bsedit_removeprog	},
	{	"addapply",		bsedit_addapply		},
	{	"addcomment",	bsedit_addcomment	},
	{	"delcomment",	bsedit_delcomment	},
	{	"name",			bsedit_name			},
	{	"object",		bsedit_object		},
	{	"ed",			bsedit_ed			},
	{	NULL,			0					}
};

void bsedit( CHAR_DATA *ch, char *argument )
{
	BONUS_INDEX_DATA * pBonus;
	char arg[ MAX_INPUT_LENGTH ];
	char command[ MAX_INPUT_LENGTH ];
	int cmd;
	AREA_DATA *ad;

	smash_tilde( argument );
	strcpy( arg, argument );
	argument = one_argument( argument, command );

	EDIT_BSDATA( ch, pBonus );

	if ( pBonus )
	{
		ad = get_vnum_area( pBonus->vnum );

		if ( ad == NULL )
		{
			edit_done( ch );
			return ;
		}

		if ( !IS_BUILDER( ch, ad ) )
		{
			send_to_char( "BSEdit: Masz niewystarczaj±ce security.\n\r", ch );
			edit_done( ch );
			return ;
		}
	}

	if ( command[ 0 ] == '\0' )
	{
		bsedit_show( ch, argument );
		return ;
	}

	if ( !str_cmp( command, "done" ) )
	{
		edit_done( ch );
		return ;
	}

	for ( cmd = 0; bsedit_table[ cmd ].name != NULL; cmd++ )
	{
		if ( !str_prefix( command, bsedit_table[ cmd ].name ) )
		{
			if ( ( *bsedit_table[ cmd ].olc_fun ) ( ch, argument ) && pBonus )
				if ( ( ad = get_vnum_area( pBonus->vnum ) ) != NULL )
					SET_BIT( ad->area_flags, AREA_CHANGED );
			return ;
		}
	}

	interpret( ch, arg );
	return ;
}


void do_bsedit( CHAR_DATA *ch, char *argument )
{
	BONUS_INDEX_DATA * pBonus;
	char command[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, command );

	if ( is_number( command ) )
	{
		ush_int vnum = atoi( command );

		AREA_DATA *ad;

		if ( ( pBonus = get_bonus_index( vnum ) ) == NULL )
		{
			send_to_char( "BSEdit: Nie istnieje taki bonus set.\n\r", ch );
			return ;
		}

		ad = get_vnum_area( vnum );

		if ( ad == NULL )
		{
			send_to_char( "BSEdit: Nie ma przypisanej krainy.\n\r", ch );
			return ;
		}

		if ( !IS_BUILDER( ch, ad ) )
		{
			send_to_char( "BSEdit: Masz niewystarczaj±ce security.\n\r", ch );
			return ;
		}

		ch->desc->pEdit	= ( void * ) pBonus;
		ch->desc->editor	= ED_BSDATA;

		return ;
	}

	if ( !str_cmp( command, "create" ) )
	{
		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "BSEdit: Sk³adnia: bsedit create [vnum]\n\r", ch );
			return ;
		}

		bsedit_create( ch, argument );
		return ;
	}

	send_to_char( "BSEdit: SK³adnia: bsedit [vnum]\n\r", ch );
	send_to_char( "                  bsedit create [vnum]\n\r", ch );

	return ;
}

BSEDIT ( bsedit_create )
{
	BONUS_INDEX_DATA * pBonus;
	int value = atoi( argument );
	AREA_DATA *ad;

	if ( IS_NULLSTR( argument ) || value < 1 )
	{
		send_to_char( "BSEdit: SK³adnia: bsedit create [vnum]\n\r", ch );
		return FALSE;
	}

	ad = get_vnum_area( value );

	if ( ad == NULL )
	{
		send_to_char( "BSEdit : VNUM nie przypisany ¿adnej krainie.\n\r", ch );
		return FALSE;
	}

	if ( !IS_BUILDER( ch, ad ) )
	{
		send_to_char( "BSEdit: Masz niewystarczaj±ce security.\n\r", ch );
		return FALSE;
	}

	if ( get_bonus_index( value ) )
	{
		send_to_char( "BSEdit: Bonus set o tym vnumie ju¿ istnieje.\n\r", ch );
		return FALSE;
	}

	pBonus	= new_bonus_index();
	pBonus->vnum	= value;
	pBonus->area = ad;
	pBonus->next	= bonus_list;
	bonus_list	= pBonus;
	ch->desc->pEdit = ( void * ) pBonus;
	ch->desc->editor = ED_BSDATA;

	send_to_char( "BSEdit: Nowy bonus set zosta³ utworzony.\n\r", ch );

	return TRUE;
}

BSEDIT( bsedit_show )
{
	BONUS_INDEX_DATA * pBonus;
	OBJ_INDEX_DATA * pObj;
	AFFECT_DATA * paf;
	char buf[ MAX_STRING_LENGTH ];
	char buf1[ MAX_STRING_LENGTH ];
	int cnt, vnum;

	EDIT_BSDATA( ch, pBonus );

	buf1[0] = '\0';

	if ( pBonus->extra_descr )
	{
		EXTRA_DESCR_DATA *ed;

		sprintf( buf1, "{GDesc Kwds{x:      [" );
		for ( ed = pBonus->extra_descr; ed; ed = ed->next )
		{
			strcat( buf1, ed->keyword );
			if ( ed->next )
				strcat( buf1, " " );
		}
		strcat( buf1, "]\n\r" );
	}

	sprintf( buf,
	         "{GName{x:           [%s]\n\r"
	         "{GVnum{x:           [%d]\n\r"
	         "{GArea{x:           [%5d] %s\n\r"
	         "%s"
	         "{GDescription{x:\n\r%s\n\r",
	         IS_NULLSTR( pBonus->name ) ? "none" : pBonus->name, pBonus->vnum, pBonus->area->vnum, pBonus->area->name, buf1,
	         pBonus->description );
	send_to_char( buf, ch );

	if ( pBonus->affects )
	{
		for ( cnt = 0, paf = pBonus->affects; paf; paf = paf->next )
		{
			if ( cnt == 0 )
			{
				send_to_char( "{CAffects{x:\n\r", ch );
				send_to_char( "Number Modifier Affects\n\r", ch );
				send_to_char( "------ -------- -------\n\r", ch );
			}

			switch ( paf->location )
			{
				case APPLY_SKILL:
					sprintf( buf, "[%4d] skill %s o %d ", cnt,
					         skill_table[ paf->modifier ].name, paf->level );
					break;
				case APPLY_LANG:
					sprintf( buf, "[%4d] language %s o %d ", cnt,
					         lang_table[ paf->modifier ].name, paf->level );
					break;
				case APPLY_MEMMING:
					sprintf( buf, "[%4d] memming krag %d o %d ", cnt,
					         paf->modifier, paf->level );
					break;
				case APPLY_RESIST:
					sprintf( buf, "[%4d] odpornosc na %s o %d ", cnt,
					         resist_name( paf->modifier ), paf->level );
					break;
				default :
					sprintf( buf, "[%4d] %-8d %s ", cnt,
					         paf->modifier,
					         flag_string( apply_flags, paf->location ) );
					break;
			}

			if ( paf->bitvector && paf->bitvector != &AFF_NONE )
			{
				strcat( buf, "[" );
				strcat( buf, affect_bit_name( NULL, paf->bitvector ) );
				strcat( buf, "]\n\r" );
			}
			else
				strcat( buf, "\n\r" );

			send_to_char( buf, ch );
			cnt++;
		}
		send_to_char( "\n\r", ch );
	}

	sprintf( buf, "{CWear prog{x: %s\n\r", !IS_NULLSTR( pBonus->wear_prog ) ? "jest" : "nie ma" );
	send_to_char( buf, ch );

	sprintf( buf, "{CRemove prog{x: %s\n\r", !IS_NULLSTR( pBonus->remove_prog ) ? "jest" : "nie ma" );
	send_to_char( buf, ch );

	send_to_char( "\n\r{CPrzedmioty nale¿±ce do tego kompletu{x:", ch );
	cnt = 0;
	for ( vnum = pBonus->area->min_vnum; vnum <= pBonus->area->max_vnum; vnum++ )
	{
		pObj = get_obj_index( vnum );
		if ( pObj && pObj->bonus_set == pBonus->vnum )
		{
			if ( cnt == 0 )
            {
				send_to_char( "\n\r", ch );
            }
			sprintf( buf, "[%5d] %s (%s)\n\r", pObj->vnum, IS_NULLSTR( pObj->short_descr ) ? "no short desc" : pObj->short_descr, flag_string( wear_flags, pObj->wear_flags ) );
			send_to_char( buf, ch );
			cnt++;
		}
	}

	if ( cnt == 0 )
		send_to_char( " brak.\n\r", ch );
	else
		print_char( ch, "Razem: %d\n\r", cnt );

	if ( !IS_NULLSTR( pBonus->comments ) )
	{
		send_to_char( "\n\r{CKomentarze{x:\n\r", ch );
		send_to_char( numlineas( pBonus->comments ), ch );
	}

	return FALSE;
}

BSEDIT( bsedit_list )
{
	int count = 1;
	BONUS_INDEX_DATA *pBonus, *bonus;
	char buf[ MAX_STRING_LENGTH ];
	BUFFER *buffer;
	bool fAll = !str_cmp( argument, "all" );
	char blah;
	AREA_DATA *ad;

	EDIT_BSDATA( ch, pBonus );
	buffer = new_buf();

	for ( bonus = bonus_list; bonus; bonus = bonus->next )
	{
		if ( fAll || bonus->area == pBonus->area )
		{
			ad = bonus->area;

			if ( ad == NULL )
				blah = '?';
			else if ( IS_BUILDER( ch, ad ) )
				blah = '*';
			else
				blah = ' ';

			sprintf( buf, "[%5d] (%c) %s\n\r", bonus->vnum, blah, IS_NULLSTR( bonus->name ) ? "no name" : bonus->name );
			add_buf( buffer, buf );

			count++;
		}
	}

	if ( count == 1 )
	{
		add_buf( buffer, "Nie znaleziono bonus setów.\n\r" );
	}

	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );

	return FALSE;
}

BSEDIT( bsedit_addaffect )
{
	int value, sn = -1;
	BONUS_INDEX_DATA *pBonus;
	AFFECT_DATA *pAf;
	char loc[ MAX_STRING_LENGTH ];
	char mod[ MAX_STRING_LENGTH ];
	char arg_last[ MAX_STRING_LENGTH ];

	EDIT_BSDATA( ch, pBonus );

	argument = one_argument( argument, loc );
	argument = one_argument( argument, mod );

	if ( loc[ 0 ] == '\0' || mod[ 0 ] == '\0' )
	{
		send_to_char( "Syntax: addaffect [location] [#xmod]\n\r", ch );
		return FALSE;
	}

	if ( ( value = flag_value( apply_flags, loc ) ) == NO_FLAG )   /* Hugin */
	{
		send_to_char( "Valid affects are:\n\r", ch );
		show_help( ch, "apply" );
		return FALSE;
	}

	if ( value == APPLY_SKILL )
	{
		sn = skill_lookup( mod );
		one_argument( argument, arg_last );

		if ( sn < 0 || arg_last[ 0 ] == '\0' || !is_number( arg_last ) || skill_table[ sn ].spell_fun != spell_null )
		{
			send_to_char( "BSEdit: SK³adnia: addaffect skill [nazwa skilla] [wartosc]\n\r", ch );
			return FALSE;
		}
	}

	else if ( value == APPLY_LANG )
	{
		sn = get_langnum( mod );
		one_argument( argument, arg_last );

		if ( sn < 0 || sn > MAX_LANG || arg_last[ 0 ] == '\0' || !is_number( arg_last ) )
		{
			send_to_char( "BSEdit: SK³adnia: addaffect lang [nazwa jezyka] [mod]\n\r", ch );
			return FALSE;
		}
	}
	else if ( value == APPLY_RESIST )
	{
		sn = resist_number( mod );
		one_argument( argument, arg_last );

		if ( sn < 0 || sn > MAX_RESIST || arg_last[ 0 ] == '\0' || !is_number( arg_last ) )
		{
			send_to_char( "BSEdit: SK³adnia: addaffect resist [odpornosc] [wartosc%]\n\r", ch );
			return FALSE;
		}
	}

	else if ( value == APPLY_MEMMING )
	{
		one_argument( argument, arg_last );

		if ( !is_number( mod ) || arg_last[ 0 ] == '\0' || !is_number( arg_last ) )
		{
			send_to_char( "BSEdit: SK³adnia: addaffect spellmem [krag] [wartosc]\n\r", ch );
			return FALSE;
		}

		sn = atoi( mod );

		if ( sn < 1 || sn > 11 )
		{
			send_to_char( "BSEdit: SK³adnia: addaffect spellmem [krag] [wartosc]\n\r", ch );
			return FALSE;
		}

	}

	else if ( !is_number( mod ) )
	{
		send_to_char( "BSEdit: SK³adnia: addaffect [location] [#xmod]\n\r", ch );
		return FALSE;
	}

	pAf = new_affect();
	pAf->location = value;

	if ( sn < 0 )
	{
		pAf->modifier = atoi( mod );
		pAf->level = 0;
	}
	else
	{
		pAf->modifier = sn;
		pAf->level = atoi( arg_last );
	}
	pAf->where	= TO_OBJECT;
	pAf->type = -1;
	pAf->duration = -1;
	pAf->rt_duration = 0;
	pAf->bitvector = &AFF_NONE;
	pAf->next = pBonus->affects;
	pBonus->affects = pAf;

	send_to_char( "Affect dodany.\n\r", ch );
	return TRUE;
}

BSEDIT( bsedit_delaffect )
{
	BONUS_INDEX_DATA * pBonus;
	AFFECT_DATA *pAf;
	AFFECT_DATA *pAf_next;
	char affect[ MAX_STRING_LENGTH ];
	int value;
	int cnt = 0;

	EDIT_BSDATA( ch, pBonus );

	one_argument( argument, affect );

	if ( !is_number( affect ) || affect[ 0 ] == '\0' )
	{
		send_to_char( "BSEdit: SK³adnia:  delaffect [#xaffect]\n\r", ch );
		return FALSE;
	}

	value = atoi( affect );

	if ( value < 0 )
	{
		send_to_char( "BSEdit: Only non-negative affect-numbers allowed.\n\r", ch );
		return FALSE;
	}

	if ( !( pAf = pBonus->affects ) )
	{
		send_to_char( "BSEdit:  Non-existant affect.\n\r", ch );
		return FALSE;
	}

	if ( value == 0 )
	{
		pAf = pBonus->affects;
		pBonus->affects = pAf->next;
		free_affect( pAf );
	}
	else
	{
		while ( ( pAf_next = pAf->next ) && ( ++cnt < value ) )
			pAf = pAf_next;

		if ( pAf_next )
		{
			pAf->next = pAf_next->next;
			free_affect( pAf_next );
		}
		else
		{
			send_to_char( "No such affect.\n\r", ch );
			return FALSE;
		}
	}

	send_to_char( "Affect removed.\n\r", ch );
	return TRUE;
}

BSEDIT( bsedit_addapply )
{
	int value, typ;
	BONUS_INDEX_DATA *pBonus;
	AFFECT_DATA *pAf;
	char loc[ MAX_STRING_LENGTH ];
	char mod[ MAX_STRING_LENGTH ];
	char type[ MAX_STRING_LENGTH ];
	char bvector[ MAX_STRING_LENGTH ];
	BITVECT_DATA * vector;

	EDIT_BSDATA( ch, pBonus );

	argument = one_argument( argument, type );
	argument = one_argument( argument, loc );
	argument = one_argument( argument, mod );
	one_argument( argument, bvector );

	if ( type[ 0 ] == '\0' || ( typ = flag_value( apply_types, type ) ) == NO_FLAG )
	{
		send_to_char( "Invalid apply type. Valid apply types are:\n\r", ch );
		show_help( ch, "apptype" );
		return FALSE;
	}

	if ( loc[ 0 ] == '\0' || ( value = flag_value( apply_flags, loc ) ) == NO_FLAG )
	{
		send_to_char( "Valid applys are:\n\r", ch );
		show_help( ch, "apply" );
		return FALSE;
	}

	if ( bvector[ 0 ] == '\0' )
	{
		send_to_char( "Invalid bitvector type.\n\r", ch );
		send_to_char( "Valid bitvector types are:\n\r", ch );
		show_help( ch, bitvector_type[ typ ].help );
		return FALSE;
	}

	vector = ext_flag_lookup( bvector, affect_flags );

	if ( vector == &EXT_NONE )
	{
		send_to_char( "Invalid bitvector type.\n\r", ch );
		send_to_char( "Valid bitvector types are:\n\r", ch );
		show_help( ch, bitvector_type[ typ ].help );
		return FALSE;
	}


	if ( mod[ 0 ] == '\0' || !is_number( mod ) )
	{
		send_to_char( "BSEdit: Sk³adnia:  addapply [type] [location] [#xmod] [bitvector]\n\r", ch );
		return FALSE;
	}

	pAf = new_affect();
	pAf->location = value;
	pAf->modifier = atoi( mod );
	pAf->where	= apply_types[ typ ].bit;
	pAf->type	= -1;
	pAf->duration = -1;
	pAf->rt_duration = 0;
	pAf->bitvector = vector;
	pAf->level = 0;
	pAf->next = pBonus->affects;
	pBonus->affects = pAf;

	send_to_char( "Apply dodany.\n\r", ch );
	return TRUE;
}

BSEDIT( bsedit_description )
{
	BONUS_INDEX_DATA * pBonus;

	EDIT_BSDATA( ch, pBonus );

	if ( argument[ 0 ] == '\0' )
	{
		string_append( ch, &pBonus->description );
		return TRUE;
	}

	send_to_char( "BSEdit: Sk³adnia: desc    - krótki opis przeznaczenia bonus setu\n\r", ch );
	return TRUE;
}

BSEDIT( bsedit_wearprog )
{
	BONUS_INDEX_DATA * pBonus;

	EDIT_BSDATA( ch, pBonus );

	if ( argument[ 0 ] == '\0' )
	{
		string_append( ch, &pBonus->wear_prog );
		return TRUE;
	}

	send_to_char( "BSEdit: Sk³adnia: wearprog    - próg odpalany przy za³o¿eniu ca³ego kompletu\n\r", ch );
	return TRUE;
}

BSEDIT( bsedit_removeprog )
{
	BONUS_INDEX_DATA * pBonus;

	EDIT_BSDATA( ch, pBonus );

	if ( argument[ 0 ] == '\0' )
	{
		string_append( ch, &pBonus->remove_prog );
		return TRUE;
	}

	send_to_char( "BSEdit: Sk³adnia: removeprog    - próg odpalany przy zdjêciu jednej rzeczy z kompletu\n\r", ch );
	return TRUE;
}

BSEDIT( bsedit_addcomment )
{
	char buf1[ MAX_STRING_LENGTH ];
	char buf2[ MAX_STRING_LENGTH ];
	char time[ MAX_STRING_LENGTH ];
	BONUS_INDEX_DATA *pBonus;

	EDIT_BSDATA( ch, pBonus );

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Nie poda<&³e¶/³a¶/³e¶> komentarza.\n\r", ch );
		return FALSE;
	}

	buf1[ 0 ] = '\0';

	strcpy( time, ( char * ) ctime( &current_time ) );
	time[ strlen( time ) - 1 ] = '\0';

	if ( !IS_NULLSTR( pBonus->comments ) )
		strcpy( buf1, pBonus->comments );
	smash_tilde( argument );
	sprintf( buf2, " - {R%s{x ({C%s{x)", capitalize( ch->name ), time );
	strcat( buf1, argument );
	strcat( buf1, buf2 );
	strcat( buf1, "\n\r" );

	free_string( pBonus->comments );
	pBonus->comments = str_dup( buf1 );

	send_to_char( "Komentarz dodany.\n\r", ch );

	return TRUE;
}

BSEDIT( bsedit_delcomment )
{
	BONUS_INDEX_DATA * pBonus;

	EDIT_BSDATA( ch, pBonus );

	if ( argument[ 0 ] == '\0' || !is_number( argument ) )
	{
		send_to_char( "Podaj numer komentarza do wykasowania.\n\r", ch );
		return FALSE;
	}

	if ( !IS_NULLSTR( pBonus->comments ) )
		pBonus->comments = string_linedel( pBonus->comments, atoi( argument ) );

	send_to_char( "Komentarz usuniêty.\n\r", ch );

	return TRUE;
}

BSEDIT( bsedit_name )
{
	BONUS_INDEX_DATA * pBonus;

	EDIT_BSDATA( ch, pBonus );

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "BSEdit: Sk³adnia:  name [string]\n\r", ch );
		return FALSE;
	}

	free_string( pBonus->name );
	pBonus->name = str_dup( argument );

	send_to_char( "Name set.\n\r", ch );
	return TRUE;
}

BSEDIT( bsedit_object )
{
	BONUS_INDEX_DATA * pBonus;
	OBJ_INDEX_DATA *pObj;

	EDIT_BSDATA( ch, pBonus );

	if ( argument[ 0 ] == '\0' || !is_number( argument ) || atoi( argument ) <= 0 )
	{
		send_to_char( "BSEdit: Sk³adnia:  object [vnum]\n\r", ch );
		return FALSE;
	}

	pObj = get_obj_index( atoi( argument ) );

	if ( !pObj )
	{
		send_to_char( "BSEdit: Nie istnieje przedmiot o takim vnumie.\n\r", ch );
		return FALSE;
	}

	if ( pObj->bonus_set > 0 && pObj->bonus_set != pBonus->vnum && get_bonus_index( pObj->bonus_set ) != NULL )
	{
		print_char( ch, "BSEdit: Obiekt jest ju¿ czê¶ci± kompletu o vnumie %d.\n\r", pObj->bonus_set );
		return FALSE;
	}

	if ( pObj->bonus_set == pBonus->vnum )
	{
		pObj->bonus_set = 0;
		send_to_char( "Obiekt usuniêty z kompletu.\n\r", ch );
	}
	else
	{
		pObj->bonus_set = pBonus->vnum;
		send_to_char( "Obiekt dodany do kompletu.\n\r", ch );
	}

	return TRUE;
}

BSEDIT( bsedit_ed )
{
	BONUS_INDEX_DATA *pBonus;
	EXTRA_DESCR_DATA *ed;
	char command[MAX_INPUT_LENGTH];
	char keyword[MAX_INPUT_LENGTH];

	EDIT_BSDATA( ch, pBonus );

	argument = one_argument( argument, command );
	one_argument( argument, keyword );

	if ( command[0] == '\0' )
	{
		send_to_char( "Sk³adnia: ed add [keyword]\n\r", ch );
		send_to_char( "         ed delete [keyword]\n\r", ch );
		send_to_char( "         ed edit [keyword]\n\r", ch );
		send_to_char( "         ed format [keyword]\n\r", ch );
		return FALSE;
	}

	if ( !str_cmp( command, "add" ) )
	{
		if ( keyword[0] == '\0' )
		{
			send_to_char( "Sk³adnia: ed add [keyword]\n\r", ch );
			return FALSE;
		}

		ed                  =   new_extra_descr();
		ed->keyword         =   str_dup( keyword );
		ed->next            =   pBonus->extra_descr;
		pBonus->extra_descr   =   ed;

		string_append( ch, &ed->description );

		return TRUE;
	}

	if ( !str_cmp( command, "edit" ) )
	{
		if ( keyword[0] == '\0' )
		{
			send_to_char( "Sk³adnia: ed edit [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pBonus->extra_descr; ed; ed = ed->next )
		{
			if ( is_name( keyword, ed->keyword ) )
				break;
		}

		if ( !ed )
		{
			send_to_char( "OEdit:  Extra description nie istnieje.\n\r", ch );
			return FALSE;
		}

		string_append( ch, &ed->description );

		return TRUE;
	}

	if ( !str_cmp( command, "delete" ) )
	{
		EXTRA_DESCR_DATA *ped = NULL;

		if ( keyword[0] == '\0' )
		{
			send_to_char( "Sk³adnia: ed delete [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pBonus->extra_descr; ed; ed = ed->next )
		{
			if ( is_name( keyword, ed->keyword ) )
				break;
			ped = ed;
		}

		if ( !ed )
		{
			send_to_char( "OEdit:  Extra description nie istnieje.\n\r", ch );
			return FALSE;
		}

		if ( !ped )
			pBonus->extra_descr = ed->next;
		else
			ped->next = ed->next;

		free_extra_descr( ed );

		send_to_char( "Extra description deleted.\n\r", ch );
		return TRUE;
	}


	if ( !str_cmp( command, "format" ) )
	{
		EXTRA_DESCR_DATA *ped = NULL;

		if ( keyword[0] == '\0' )
		{
			send_to_char( "Sk³adnia: ed format [keyword]\n\r", ch );
			return FALSE;
		}

		for ( ed = pBonus->extra_descr; ed; ed = ed->next )
		{
			if ( is_name( keyword, ed->keyword ) )
				break;
			ped = ed;
		}

		if ( !ed )
		{
			send_to_char( "OEdit:  Extra description nie istnieje.\n\r", ch );
			return FALSE;
		}

		ed->description = format_string( ed->description );

		send_to_char( "Extra description formatted.\n\r", ch );
		return TRUE;
	}

	bsedit_ed( ch, "" );
	return FALSE;
}
