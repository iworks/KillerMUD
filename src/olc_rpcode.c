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
#include "interp.h"


#define RPEDIT( fun )           bool fun(CHAR_DATA *ch, char*argument)

extern ROOM_INDEX_DATA *find_location( CHAR_DATA *, char * );

const struct olc_cmd_type rpedit_table[] =
    {
/*      {   command         function            },*/
        {   "commands",     show_commands       },
        {   "create",       rpedit_create       },
        {   "code",         rpedit_code         },
        {   "show",         rpedit_show         },
        {   "list",         rpedit_list         },
        {   "description",  rpedit_description  },
        {   "name",         rpedit_name         },
        {   "area",         rpedit_area         },
		{   "varcmd",       rpedit_varcmd       },
        {   "?",            show_help           },
        {   NULL,           0                   }
    };

void rpedit( CHAR_DATA *ch, char *argument )
{
    PROG_CODE * pRcode;
    char arg[ MAX_INPUT_LENGTH ];
    char command[ MAX_INPUT_LENGTH ];
    char log_buf[ MAX_STRING_LENGTH ];
    int cmd;
    AREA_DATA *ad;

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    EDIT_RPCODE( ch, pRcode );

    if ( pRcode )
    {
        ad = pRcode->area;

        if ( ad == NULL )
        {
            edit_done( ch );
            return ;
        }

        if ( !IS_BUILDER( ch, ad ) )
        {
            send_to_char( "RPEdit: Insufficient security to modify code.\n\r", ch );
            edit_done( ch );
            return ;
        }
    }

    if ( command[ 0 ] == '\0' )
    {
        rpedit_show( ch, argument );
        return ;
    }

    if ( !str_cmp( command, "done" ) )
    {
        edit_done( ch );
        return ;
    }

    for ( cmd = 0; rpedit_table[ cmd ].name != NULL; cmd++ )
    {
        if ( !str_prefix( command, rpedit_table[ cmd ].name ) )
        {
            if ( ( *rpedit_table[ cmd ].olc_fun ) ( ch, argument ) && pRcode )
            {
                if ( ( ad = pRcode->area ) != NULL )
                    SET_BIT( ad->area_flags, AREA_CHANGED );
                append_file_format_daily( ch, BUILD_LOG_FILE, "-> rpedit %s: %s", pRcode ? pRcode->name : "0", arg );
            }
            return ;
        }
    }

    interpret( ch, arg );

    return ;
}

void do_rpedit( CHAR_DATA *ch, char *argument )
{
    PROG_CODE * pRcode;
    AREA_DATA * ad;
    char command[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
        send_to_char( "RPEdit: Sk³adnia : rpedit [name]\n\r", ch );
        send_to_char( "                   rpedit create [name]\n\r", ch );
    }

    if ( !str_cmp( command, "create" ) )
    {
        if ( argument[ 0 ] == '\0' )
        {
            send_to_char( "RPEdit: Sk³adnia: rpedit create [name]\n\r", ch );
            return ;
        }

        rpedit_create( ch, argument );
        return ;
    }

    if ( ( pRcode = get_rprog_index( command ) ) == NULL )
    {
        print_char( ch, "RPEdit : Roomprog o nazwie '%s' nie istnieje.\n\r", command );
        return ;
    }

    ad = pRcode->area;

    if ( ad && !IS_BUILDER( ch, ad ) )
    {
        send_to_char( "RPEdit : Niewystarczaj±ce uprawnienia.\n\r", ch );
        return ;
    }

    ch->desc->pEdit	= ( void * ) pRcode;
    ch->desc->editor	= ED_RPCODE;

    return ;
}

RPEDIT ( rpedit_create )
{
    PROG_CODE * pRcode;
    AREA_DATA *ad;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( IS_NULLSTR( arg ) )
    {
        send_to_char( "RPEdit: Sk³adnia: rpedit create [name]\n\r", ch );
        return FALSE;
    }

    if ( strlen( arg ) > 15 )
    {
        send_to_char( "RPEdit: Nazwa nie mo¿e byæ d³u¿sza ni¿ 15 znaków.\n\r", ch );
        return FALSE;
    }

    ad = ch->in_room->area;

    if ( !ad )
        return FALSE;

    if ( !IS_BUILDER( ch, ad ) )
    {
        send_to_char( "RPEdit: Niewystarczaj±ce uprawnienia.\n\r", ch );
        return FALSE;
    }

    if ( get_rprog_index( arg ) )
    {
        print_char( ch, "RPEdit: RoomProg o nazwie '%s' ju¿ istnieje.\n\r", arg );
        return FALSE;
    }

    pRcode          = new_pcode();
    pRcode->name    = str_dup( arg );
    pRcode->area    = ad;
    pRcode->next    = rprog_list;
    rprog_list      = pRcode;
    ch->desc->pEdit	= ( void * ) pRcode;
    ch->desc->editor	= ED_RPCODE;

    send_to_char( "RoomProgram Code Created.\n\r", ch );

    return TRUE;
}

RPEDIT( rpedit_show )
{
    PROG_CODE * pRcode;
    char buf[ MAX_STRING_LENGTH ];

    EDIT_RPCODE( ch, pRcode );

    sprintf( buf,
             "{GName{x:       [%s] {GArea{x: [%5d] %s\n\r"
             "{GDescription{x:\n\r%s"
             "{GCode{x:\n\r%s\n\r",
             pRcode->name, pRcode->area->vnum, pRcode->area->name, pRcode->description, pRcode->code );
    send_to_char( buf, ch );

    return FALSE;
}

RPEDIT( rpedit_code )
{
    PROG_CODE * pRcode;
    EDIT_RPCODE( ch, pRcode );

    if ( argument[ 0 ] == '\0' )
    {
        string_append( ch, &pRcode->code );
        return TRUE;
    }

    send_to_char( "Syntax: code\n\r", ch );
    return FALSE;
}

RPEDIT( rpedit_area )
{
    PROG_CODE * pRcode;
    AREA_DATA * pArea;
    EDIT_RPCODE( ch, pRcode );

    if ( argument[0] != '\0' && is_number( argument ) )
    {
        if ( ( pArea = get_area_data( atoi( argument ) ) ) == NULL )
        {
            send_to_char( "Nie ma krainy o takim numerze.\n\r", ch );
            return FALSE;
        }

        SET_BIT( pRcode->area->area_flags, AREA_CHANGED );
        pRcode->area = pArea;
        SET_BIT( pRcode->area->area_flags, AREA_CHANGED );
        send_to_char( "Przydzia³ proga do krainy zmieniony.\n\r", ch );
        return TRUE;
    }

    send_to_char( "RPEdit: Sk³adnia: area <area number>\n\r", ch );
    return FALSE;
}

RPEDIT( rpedit_description )
{
    PROG_CODE * pRcode;
    EDIT_RPCODE( ch, pRcode );

    if ( argument[0] == '\0' )
    {
        string_append( ch, &pRcode->description );
        return TRUE;
    }

    send_to_char( "RPEdit: Sk³adnia: description\n\r", ch );
    return FALSE;
}

RPEDIT( rpedit_name )
{
    ROOM_INDEX_DATA * room;
    PROG_LIST *rpl;
    PROG_CODE * pRcode;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int hash;
    EDIT_RPCODE( ch, pRcode );

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "RPEdit: Sk³adnia: name <wyraz>\n\r"
                              "Nazwa nie mo¿e byæ d³u¿sza ni¿ 15 znaków.\n\r", ch );
        return FALSE;
    }

    if ( strlen( arg ) > 15 )
    {
        send_to_char( "RPEdit: Nazwa nie mo¿e byæ d³u¿sza ni¿ 15 znaków.\n\r", ch );
        return FALSE;
    }

    if ( get_rprog_index( arg ) )
    {
        print_char( ch, "RPEdit: RoomProg o nazwie '%s' ju¿ istnieje.\n\r", arg );
        return FALSE;
    }

    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
        for ( room = room_index_hash[ hash ]; room; room = room->next )
            for ( rpl = room->progs; rpl; rpl = rpl->next )
                if ( FAST_STR_CMP( rpl->name, pRcode->name ) )
                {
                    rpl->name = arg;
                }

    free_string( pRcode->name );
    pRcode->name = str_dup( arg );

    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
        for ( room = room_index_hash[ hash ]; room; room = room->next )
            for ( rpl = room->progs; rpl; rpl = rpl->next )
                if ( FAST_STR_CMP( rpl->name, arg ) )
                {
                    sprintf( buf, "Uaktualnianie lokacji %d.\n\r", room->vnum );
                    send_to_char( buf, ch );
                    rpl->name = pRcode->name;
                    SET_BIT( room->area->area_flags, AREA_CHANGED );
               }

    return TRUE;
}

RPEDIT( rpedit_list )
{
    do_rplist( ch, argument );
    return FALSE;
}

RPEDIT( rpedit_varcmd )
{
	int cmd, arg_count;
	char args[ MAX_STRING_LENGTH ];

	send_to_char( "Lista komend operuj±cych na zmiennych:\n\r", ch );

	for ( cmd = 0; var_cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
	{
		args[0] = '\0';
		for ( arg_count = 0; var_cmd_table[ cmd ].args_type[ arg_count ] != -1; arg_count++ )
		{
			if ( arg_count > 0 )
				strcat( args, " " );
			strcat( args, var_type_to_str[ var_cmd_table[ cmd ].args_type[ arg_count ] ] );
		}

		print_char( ch, "{G%-8s{x {C%s{x {M%s{x - %s\n\r",
					var_type_to_str[ var_cmd_table[ cmd ].ret_type ],
					var_cmd_table[ cmd ].name,
					args,
					var_cmd_table[ cmd ].syntax_desc );
	}

	return FALSE;
}

void do_rpstat( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_STRING_LENGTH ];
    PROG_LIST *rprg;
    ROOM_INDEX_DATA *room = NULL;
    int i;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
        room = ch->in_room;
    else
    {
        if ( ( room = find_location( ch, arg ) ) == NULL )
        {
            send_to_char( "No such location.\n\r", ch );
            return ;
        }
    }
    sprintf( arg, "Room #%-6d [%s]\n\r", room->vnum, room->name );
    send_to_char( arg, ch );

    sprintf( arg, "Delay   %-6d \n\rTarget  %s\n\r",
             room->prog_delay, room->prog_target ? room->prog_target->name : "None" );
    send_to_char( arg, ch );

    if ( !room->prog_flags )
    {
        send_to_char( "[No programs set]\n\r", ch );
        return ;
    }

    for ( i = 0, rprg = room->progs; rprg != NULL;
          rprg = rprg->next )

    {
        sprintf( arg, "[%2d] Trigger [%-8s] Program [%-15s] Phrase [%s]\n\r",
                 ++i,
                 ext_bit_name( prog_flags, rprg->trig_type ),
                 rprg->name,
                 rprg->trig_phrase );
        send_to_char( arg, ch );
    }

    return ;

}


void do_rpdump( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_INPUT_LENGTH ];
    PROG_CODE *rprg;
    AREA_DATA *pArea;
    one_argument( argument, buf );
    if ( ( rprg = get_rprog_index( buf ) ) == NULL )
    {
        send_to_char( "RPEdit: Nie ma takiego ROOMproga.\n\r", ch );
        return ;
    }
    pArea = rprg->area;
    page_to_char( rprg->code, ch );
}

