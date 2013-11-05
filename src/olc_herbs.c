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
#include "herbs.h"

#define HBEDIT( fun )		bool fun( CHAR_DATA *ch, char *argument )

char *  string_linedel		args( ( char *, int ) );
char *  string_lineadd		args( ( char *, char *, int ) );
char *  numlineas			args( ( char * ) );

const struct olc_cmd_type hbedit_table[] =
{
	{	"commands",		show_commands		},
	{	"create",		hbedit_create		},
	{	"show",			hbedit_show			},
	{	"list",			hbedit_list			},
	{	"?",			show_help			},
	{	"id",			hbedit_id			},
	{	"name",			hbedit_name			},
	{	"odmiana",		hbedit_odmiana		},
	{	NULL,			0					}
};

void hbedit( CHAR_DATA *ch, char *argument )
{
	HERB_DATA * pHerb;
	char arg[ MAX_INPUT_LENGTH ];
	char command[ MAX_INPUT_LENGTH ];
	int cmd;

	smash_tilde( argument );
	strcpy( arg, argument );
	argument = one_argument( argument, command );

	EDIT_HBDATA( ch, pHerb );

	if ( !pHerb )
		return;

	if ( command[ 0 ] == '\0' )
	{
		hbedit_show( ch, argument );
		return ;
	}

	if ( !str_cmp( command, "done" ) )
	{
		edit_done( ch );
		return ;
	}

	for ( cmd = 0; hbedit_table[ cmd ].name != NULL; cmd++ )
	{
		if ( !str_prefix( command, hbedit_table[ cmd ].name ) )
		{
			( *hbedit_table[ cmd ].olc_fun ) ( ch, argument );
			return ;
		}
	}

	interpret( ch, arg );
	return ;
}


void do_hbedit( CHAR_DATA *ch, char *argument )
{
	HERB_DATA * pHerb;
	char arg[ MAX_INPUT_LENGTH ];

	argument = one_argument( argument, arg );

	if ( !str_cmp( arg, "create" ) )
	{
		if ( argument[ 0 ] == '\0' )
		{
			send_to_char( "Syntax : hbedit create [vnum]\n\r", ch );
			return ;
		}

		hbedit_create( ch, argument );
		return ;
	}

	if ( !IS_NULLSTR( arg ) )
	{
		if ( ( pHerb = get_herb_index( arg ) ) == NULL )
		{
			send_to_char( "HBEdit: Nie istnieje takie zió³ko.\n\r", ch );
			return ;
		}

		ch->desc->pEdit	= ( void * ) pHerb;
		ch->desc->editor	= ED_HBDATA;

		return ;
	}

	send_to_char( "Syntax :   hbedit [id]\n\r", ch );
	send_to_char( "           hbedit create [id]\n\r", ch );

	return ;
}

HBEDIT( hbedit_create )
{
	HERB_DATA * pHerb;

	if ( IS_NULLSTR( argument ) )
	{
		send_to_char( "Syntax : bsedit create [id]\n\r", ch );
		return FALSE;
	}

	if ( get_herb_index( argument ) )
	{
		send_to_char( "HBEdit: Bonus set o tym identyfikatorze ju¿ istnieje.\n\r", ch );
		return FALSE;
	}

	pHerb = new_herb();
	pHerb->id = str_dup( argument );
	pHerb->next	= herb_list;
	herb_list = pHerb;
	ch->desc->pEdit = ( void * ) pHerb;
	ch->desc->editor = ED_HBDATA;

	send_to_char( "Nowe zió³ko zosta³o stworzone.\n\r", ch );

	return TRUE;
}

HBEDIT( hbedit_show )
{
	HERB_DATA * pHerb;
	char buf[ MAX_STRING_LENGTH ];

	EDIT_HBDATA( ch, pHerb );

	sprintf( buf,
	         "{GId{x:           [%s]\n\r"
	         "{GName{x:         [%s]\n\r"
	         "{GDope³niacz{x:   [%s] ({Gczego nie ma?{x)\n\r"
	         "{GCelownik{x:     [%s] ({Gczemu siê przygl±dam?{x)\n\r"
	         "{GBiernik{x:      [%s] ({Gco widzê?{x)\n\r"
	         "{GNarzêdnik{x:    [%s] ({Gczym siê pos³ugujê?{x)\n\r"
	         "{GMiejscownik{x:  [%s] ({Go czym mówiê?{x)\n\r",
			 IS_NULLSTR( pHerb->id ) ? "none" : pHerb->id,
	         IS_NULLSTR( pHerb->name ) ? "none" : pHerb->name,
	         IS_NULLSTR( pHerb->name2 ) ? "none" : pHerb->name2,
	         IS_NULLSTR( pHerb->name3 ) ? "none" : pHerb->name3,
	         IS_NULLSTR( pHerb->name4 ) ? "none" : pHerb->name4,
	         IS_NULLSTR( pHerb->name5 ) ? "none" : pHerb->name5,
	         IS_NULLSTR( pHerb->name6 ) ? "none" : pHerb->name6 );
	send_to_char( buf, ch );

	return FALSE;
}

HBEDIT( hbedit_list )
{
	int count = 1;
	HERB_DATA * herb;
	char buf[ MAX_STRING_LENGTH ];
	BUFFER *buffer;

	buffer = new_buf();

	for ( herb = herb_list; herb; herb = herb->next )
	{
			sprintf( buf, "[%3d] %s - %s\n\r", count, herb->id, IS_NULLSTR( herb->name ) ? "no name" : herb->name );
			add_buf( buffer, buf );

			count++;
	}

	if ( count == 1 )
	{
		add_buf( buffer, "Nie znaleziono zió³ek.\n\r" );
	}

	page_to_char( buf_string( buffer ), ch );
	free_buf( buffer );

	return FALSE;
}

HBEDIT( hbedit_id )
{
	HERB_DATA * pHerb;

	EDIT_HBDATA( ch, pHerb );

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Syntax:  id [string]\n\r", ch );
		return FALSE;
	}

	if ( get_herb_index( argument ) )
	{
		send_to_char( "Istnieje ju¿ zio³o o takim identyfikatorze.\n\r", ch );
		return FALSE;
	}

	free_string( pHerb->id );
	pHerb->id = str_dup( argument );

	send_to_char( "Id set.\n\r", ch );
	return TRUE;
}

HBEDIT( hbedit_name )
{
	HERB_DATA * pHerb;

	EDIT_HBDATA( ch, pHerb );

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Syntax:  name [string]\n\r", ch );
		return FALSE;
	}

	free_string( pHerb->name );
	pHerb->name = str_dup( argument );

	send_to_char( "Name set.\n\r", ch );
	return TRUE;
}

HBEDIT( hbedit_odmiana )
{
	HERB_DATA * pHerb;
	char arg1[ MAX_INPUT_LENGTH ];

	EDIT_HBDATA( ch, pHerb );

	for( ; ; )
	{
		if ( argument[ 0 ] == '\0' ) break;

		argument = one_argument( argument, arg1 );

		if ( argument[ 0 ] == '\0' ) break;

		if ( !str_prefix( arg1, "dopelniacz" ) )
		{
			free_string( pHerb->name2 );
			pHerb->name2 = str_dup( argument );
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}

		if ( !str_prefix( arg1, "celownik" ) )
		{
			free_string( pHerb->name3 );
			pHerb->name3 = str_dup( argument );
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}

		if ( !str_prefix( arg1, "biernik" ) )
		{
			free_string( pHerb->name4 );
			pHerb->name4 = str_dup( argument );
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}

		if ( !str_prefix( arg1, "narzednik" ) )
		{
			free_string( pHerb->name5 );
			pHerb->name5 = str_dup( argument );
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}

		if ( !str_prefix( arg1, "miejscownik" ) )
		{
			free_string( pHerb->name6 );
			pHerb->name6 = str_dup( argument );
			send_to_char( "Odmiana ustawiona.\n\r", ch );
			return TRUE;
		}
		send_to_char( "B³êdny przypadek.\n\r", ch );
		return TRUE;
	}

	send_to_char( "Sk³adnia: odmiana <przypadek> <string>\n\r" , ch );
	return FALSE;
}
