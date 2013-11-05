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

#define OPEDIT( fun )           bool fun(CHAR_DATA *ch, char*argument)

const struct olc_cmd_type opedit_table[] =
    {
/*      {   command         function            },*/
        {   "commands",     show_commands       },
        {   "create",       opedit_create       },
        {   "code",         opedit_code         },
        {   "show",         opedit_show         },
        {   "list",         opedit_list         },
        {   "description",  opedit_description  },
        {   "name",         opedit_name         },
        {   "area",         opedit_area         },
		{   "varcmd",       opedit_varcmd       },
        {   "?",            show_help           },
        {   NULL,           0                   }
    };

void opedit( CHAR_DATA *ch, char *argument )
{
    PROG_CODE * pOcode;
    char arg[ MAX_INPUT_LENGTH ];
    char command[ MAX_INPUT_LENGTH ];
    char log_buf[ MAX_STRING_LENGTH ];
    int cmd;
    AREA_DATA *ad;

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    EDIT_OPCODE( ch, pOcode );

    if ( pOcode )
    {
        ad = pOcode->area;

        if ( ad == NULL )  /* ? */
        {
            edit_done( ch );
            return ;
        }

        if ( !IS_BUILDER( ch, ad ) )
        {
            send_to_char( "OPEdit: Insufficient security to modify code.\n\r", ch );
            edit_done( ch );
            return ;
        }
    }

    if ( command[ 0 ] == '\0' )
    {
        opedit_show( ch, argument );
        return ;
    }

    if ( !str_cmp( command, "done" ) )
    {
        edit_done( ch );
        return ;
    }

    for ( cmd = 0; opedit_table[ cmd ].name != NULL; cmd++ )
    {
        if ( !str_prefix( command, opedit_table[ cmd ].name ) )
        {
            if ( ( *opedit_table[ cmd ].olc_fun ) ( ch, argument ) && pOcode )
            {
                if ( ( ad = pOcode->area ) != NULL )
                    SET_BIT( ad->area_flags, AREA_CHANGED );
                append_file_format_daily( ch, BUILD_LOG_FILE, "-> opedit %s: %s", pOcode ? pOcode->name : "0", arg );
            }
            return ;
        }
    }

    interpret( ch, arg );

    return ;
}

void do_opedit( CHAR_DATA *ch, char *argument )
{
    PROG_CODE * pOcode;
    AREA_DATA *ad;
    char command[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
        send_to_char( "OPEdit: Sk³adnia:  opedit [name]\n\r", ch );
        send_to_char( "                   opedit create [name]\n\r", ch );
    }

    if ( !str_cmp( command, "create" ) )
    {
        if ( argument[ 0 ] == '\0' )
        {
            send_to_char( "OPEdit: Sk³adnia: opedit create [name]\n\r", ch );
            return ;
        }

        opedit_create( ch, argument );
        return ;
    }

    if ( ( pOcode = get_oprog_index( command ) ) == NULL )
    {
        print_char( ch, "OPEdit : Objprog o nazwie '%s' nie istnieje.\n\r", command );
        return ;
    }

    ad = pOcode->area;

    if ( ad && !IS_BUILDER( ch, ad ) )
    {
        send_to_char( "OPEdit: Niewystarczaj±ce uprawnienia.\n\r", ch );
        return ;
    }

    ch->desc->pEdit	= ( void * ) pOcode;
    ch->desc->editor	= ED_OPCODE;

    return;
}

OPEDIT ( opedit_create )
{
    PROG_CODE * pOcode;
    AREA_DATA *ad;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( IS_NULLSTR( arg ) )
    {
        send_to_char( "OPEdit: Sk³adnia: opedit create [name]\n\r", ch );
        return FALSE;
    }

    if ( strlen( arg ) > 15 )
    {
        send_to_char( "Nazwa nie mo¿e byæ d³u¿sza ni¿ 15 znaków.\n\r", ch );
        return FALSE;
    }

    ad = ch->in_room->area;

    if ( !ad )
        return FALSE;

    if ( !IS_BUILDER( ch, ad ) )
    {
        send_to_char( "OPEdit: Niewystarczaj±ce uprawnienia.\n\r", ch );
        return FALSE;
    }

    if ( get_oprog_index( arg ) )
    {
        print_char( ch, "OPEdit: ObjProg o nazwie '%s' ju¿ istnieje.\n\r", arg );
        return FALSE;
    }

    pOcode          = new_pcode();
    pOcode->name    = str_dup( arg );
    pOcode->area    = ad;
    pOcode->next    = oprog_list;
    oprog_list      = pOcode;
    ch->desc->pEdit	= ( void * ) pOcode;
    ch->desc->editor	= ED_OPCODE;

    send_to_char( "ObjProgram zosta³ stworzony.\n\r", ch );

    return TRUE;
}

OPEDIT( opedit_show )
{
    PROG_CODE * pOcode;
    char buf[ MAX_STRING_LENGTH ];

    EDIT_OPCODE( ch, pOcode );

    sprintf( buf,
             "{GName{x:       [%s] {GArea{x: [%5d] %s\n\r"
             "{GDescription{x:\n\r%s"
             "{GCode{x:\n\r%s\n\r",
             pOcode->name, pOcode->area->vnum, pOcode->area->name, pOcode->description, pOcode->code );
    send_to_char( buf, ch );

    return FALSE;
}

OPEDIT( opedit_code )
{
    PROG_CODE * pOcode;
    EDIT_OPCODE( ch, pOcode );

    if ( argument[ 0 ] == '\0' )
    {
        string_append( ch, &pOcode->code );
        return TRUE;
    }

    send_to_char( "Syntax: code\n\r", ch );
    return FALSE;
}

OPEDIT( opedit_area )
{
    PROG_CODE * pOcode;
    AREA_DATA * pArea;
    EDIT_OPCODE( ch, pOcode );

    if ( argument[0] != '\0' && is_number( argument ) )
    {
        if ( ( pArea = get_area_data( atoi( argument ) ) ) == NULL )
        {
            send_to_char( "Nie ma krainy o takim numerze.\n\r", ch );
            return FALSE;
        }

        SET_BIT( pOcode->area->area_flags, AREA_CHANGED );
        pOcode->area = pArea;
        SET_BIT( pOcode->area->area_flags, AREA_CHANGED );
        send_to_char( "Przydzia³ proga do krainy zmieniony.\n\r", ch );
        return TRUE;
    }

    send_to_char( "Sk³adnia: area <area number>\n\r", ch );
    return FALSE;
}

OPEDIT( opedit_description )
{
    PROG_CODE * pOcode;
    EDIT_OPCODE( ch, pOcode );

    if ( argument[0] == '\0' )
    {
        string_append( ch, &pOcode->description );
        return TRUE;
    }

    send_to_char( "Syntax: description\n\r", ch );
    return FALSE;
}

OPEDIT( opedit_name )
{
    OBJ_INDEX_DATA * obj;
    PROG_LIST *opl;
    PROG_CODE * pOcode;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int hash;
    EDIT_OPCODE( ch, pOcode );

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "OPEdit: Sk³adnia: name <wyraz>\n\r"
                              "Nazwa nie mo¿e byæ d³u¿sza ni¿ 15 znaków.\n\r", ch );
        return FALSE;
    }

    if ( strlen( arg ) > 15 )
    {
        send_to_char( "Nazwa nie mo¿e byæ d³u¿sza ni¿ 15 znaków.\n\r", ch );
        return FALSE;
    }

    if ( get_oprog_index( arg ) )
    {
        print_char( ch, "ObjProg o nazwie '%s' ju¿ istnieje.\n\r", arg );
        return FALSE;
    }

    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
        for ( obj = obj_index_hash[ hash ]; obj; obj = obj->next )
            for ( opl = obj->progs; opl; opl = opl->next )
                if ( FAST_STR_CMP( opl->name, pOcode->name ) )
                {
                    opl->name = arg;
                }

    free_string( pOcode->name );
    pOcode->name = str_dup( arg );

    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
        for ( obj = obj_index_hash[ hash ]; obj; obj = obj->next )
            for ( opl = obj->progs; opl; opl = opl->next )
                if ( FAST_STR_CMP( opl->name, arg ) )
                {
                    sprintf( buf, "Uaktualnianie obiektu %d.\n\r", obj->vnum );
                    send_to_char( buf, ch );
                    opl->name = pOcode->name;
                    SET_BIT( obj->area->area_flags, AREA_CHANGED );
               }

    return TRUE;
}

OPEDIT( opedit_list )
{
    do_oplist( ch, argument );
    return FALSE;
}

OPEDIT( opedit_varcmd )
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

void do_opstat( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_STRING_LENGTH ];
    PROG_LIST *oprg;
    OBJ_DATA *obj;
    int i;

    one_argument( argument, arg );

    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Opstat what?\n\r", ch );
        return ;
    }

    if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
    {
        send_to_char( "No such thing.\n\r", ch );
        return ;
    }

    sprintf( arg, "Object #%-6d [%s]\n\r", obj->pIndexData->vnum, obj->short_descr );
    send_to_char( arg, ch );

    sprintf( arg, "Delay   %-6d [%s]\n\r",
             obj->prog_delay,
             obj->prog_target == NULL
             ? "No target" : obj->prog_target->name );
    send_to_char( arg, ch );

    if ( !obj->pIndexData->prog_flags )
    {
        send_to_char( "[No programs set]\n\r", ch );
        return ;
    }

    for ( i = 0, oprg = obj->pIndexData->progs; oprg != NULL;
          oprg = oprg->next )

    {
        sprintf( arg, "[%2d] Trigger [%-8s] Program [%-15s] Phrase [%s]\n\r",
                 ++i,
                 ext_bit_name( prog_flags, oprg->trig_type ),
                 oprg->name,
                 oprg->trig_phrase );
        send_to_char( arg, ch );
    }

    return ;

}


void do_opdump( CHAR_DATA *ch, char *argument )
{
    char buf[ MAX_INPUT_LENGTH ];
    PROG_CODE *oprg;
    AREA_DATA *pArea;
    one_argument( argument, buf );
    if ( ( oprg = get_oprog_index( buf ) ) == NULL )
    {
        send_to_char( "OPEdit: Nie ma takiego OBJproga.\n\r", ch );
        return ;
    }
    pArea = oprg->area;
    page_to_char( oprg->code, ch );
}

