/* The following code is based on ILAB OLC by Jason Dinkel */
/* Mobprogram code by Lordrom for Nevermore Mud */

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
#include "olc.h"
#include "recycle.h"
#include "interp.h"
#include "progs.h"

#define MPEDIT( fun )           bool fun(CHAR_DATA *ch, char*argument)


const struct olc_cmd_type mpedit_table[] =
    {
/*      {   command         function            },*/
        {   "commands",     show_commands       },
        {   "create",       mpedit_create       },
        {   "code",         mpedit_code         },
        {   "show",         mpedit_show         },
        {   "list",         mpedit_list         },
        {   "description",  mpedit_description  },
        {   "name",         mpedit_name         },
        {   "area",         mpedit_area         },
        {   "varcmd",       mpedit_varcmd       },
        {   "?",            show_help           },
        {   NULL,           0                   }
    };

void mpedit( CHAR_DATA *ch, char *argument )
{
    PROG_CODE * pMcode;
    char arg[ MAX_INPUT_LENGTH ];
    char command[ MAX_INPUT_LENGTH ];
    char log_buf[ MAX_STRING_LENGTH ];
    int cmd;
    AREA_DATA *ad;

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    EDIT_MPCODE( ch, pMcode );

    if ( pMcode )
    {
        ad = pMcode->area;

        if ( ad == NULL )  /* ? */
        {
            edit_done( ch );
            return ;
        }

        if ( !IS_BUILDER( ch, ad ) )
        {
            send_to_char( "MPEdit: Insufficient security to modify code.\n\r", ch );
            edit_done( ch );
            return ;
        }
    }

    if ( command[ 0 ] == '\0' )
    {
        mpedit_show( ch, argument );
        return ;
    }

    if ( !str_cmp( command, "done" ) )
    {
        edit_done( ch );
        return ;
    }

    for ( cmd = 0; mpedit_table[ cmd ].name != NULL; cmd++ )
    {
        if ( !str_prefix( command, mpedit_table[ cmd ].name ) )
        {
            if ( ( *mpedit_table[ cmd ].olc_fun ) ( ch, argument ) && pMcode )
            {
                if ( ( ad = pMcode->area ) != NULL )
                {
                    SET_BIT( ad->area_flags, AREA_CHANGED );
                }
                append_file_format_daily( ch, BUILD_LOG_FILE, "-> mpedit %s: %s", pMcode ? pMcode->name: "0", arg );
            }
            return ;
        }
    }

    interpret( ch, arg );

    return ;
}

void do_mpedit( CHAR_DATA *ch, char *argument )
{
    PROG_CODE * pMcode;
    AREA_DATA *ad;
    char command[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, command );

    if ( command[0] == '\0' )
    {
        send_to_char( "MPEdit: Skladnia : mpedit [name]\n\r", ch );
        send_to_char( "                   mpedit create [name]\n\r", ch );
    }

    if ( !str_cmp( command, "create" ) )
    {
        if ( argument[ 0 ] == '\0' )
        {
            send_to_char( "RPEdit: Sk³adnia: mpedit create [name]\n\r", ch );
            return ;
        }

        mpedit_create( ch, argument );
        return ;
    }

    if ( ( pMcode = get_mprog_index( command ) ) == NULL )
    {
        print_char( ch, "MPEdit : Mobprog o nazwie '%s' nie istnieje.\n\r", command );
        return ;
    }

    ad = pMcode->area;

    if ( ad && !IS_BUILDER( ch, ad ) )
    {
        send_to_char( "MPEdit : Niewystarczaj±ce uprawnienia.\n\r", ch );
        return ;
    }

    ch->desc->pEdit	= ( void * ) pMcode;
    ch->desc->editor	= ED_MPCODE;

    return ;
}

MPEDIT ( mpedit_create )
{
    PROG_CODE * pMcode;
    AREA_DATA *ad;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( IS_NULLSTR( arg ) )
    {
        send_to_char( "MPEdit: Sk³adnia: mpedit create [name]\n\r", ch );
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
        send_to_char( "MPEdit : Niewystarczaj±ce uprawnienia.\n\r", ch );
        return FALSE;
    }

    if ( get_mprog_index( arg ) )
    {
        print_char( ch, "MPEdit: MobProg o nazwie '%s' ju¿ istnieje.\n\r", arg );
        return FALSE;
    }

    pMcode          = new_pcode();
    pMcode->name    = str_dup( arg );
    pMcode->area    = ad;
    pMcode->next    = mprog_list;
    mprog_list      = pMcode;
    ch->desc->pEdit	= ( void * ) pMcode;
    ch->desc->editor	= ED_MPCODE;

    send_to_char( "MobProgram zosta³ stworzony.\n\r", ch );

    return TRUE;
}

MPEDIT( mpedit_show )
{
    PROG_CODE * pMcode;
    char buf[ MAX_STRING_LENGTH ];

    EDIT_MPCODE( ch, pMcode );

    sprintf( buf,
             "{GName{x:       [%s] {GArea{x: [%5d] %s\n\r"
             "{GDescription{x:\n\r%s"
             "{GCode{x:\n\r%s\n\r",
             pMcode->name, pMcode->area->vnum, pMcode->area->name, pMcode->description, pMcode->code );
    send_to_char( buf, ch );

    return FALSE;
}

MPEDIT( mpedit_code )
{
    PROG_CODE * pMcode;
    EDIT_MPCODE( ch, pMcode );

    if ( argument[ 0 ] == '\0' )
    {
        string_append( ch, &pMcode->code );
        return TRUE;
    }

    send_to_char( "RPEdit: Sk³adnia: code\n\r", ch );
    return FALSE;
}

MPEDIT( mpedit_description )
{
    PROG_CODE * pMcode;
    EDIT_MPCODE( ch, pMcode );

    if ( argument[0] == '\0' )
    {
        string_append( ch, &pMcode->description );
        return TRUE;
    }

    send_to_char( "RPEdit: Sk³adnia: description\n\r", ch );
    return FALSE;
}

MPEDIT( mpedit_area )
{
    PROG_CODE * pMcode;
    AREA_DATA * pArea;
    EDIT_MPCODE( ch, pMcode );

    if ( argument[0] != '\0' && is_number( argument ) )
    {
        if ( ( pArea = get_area_data( atoi( argument ) ) ) == NULL )
        {
            send_to_char( "Nie ma krainy o takim numerze.\n\r", ch );
            return FALSE;
        }

        SET_BIT( pMcode->area->area_flags, AREA_CHANGED );
        pMcode->area = pArea;
        SET_BIT( pMcode->area->area_flags, AREA_CHANGED );
        send_to_char( "Przydzia³ proga do krainy zmieniony.\n\r", ch );
        return TRUE;
    }

    send_to_char( "RPEdit: Sk³adnia: area <area number>\n\r", ch );
    return FALSE;
}

MPEDIT( mpedit_name )
{
    MOB_INDEX_DATA * mob;
    PROG_LIST *mpl;
    PROG_CODE * pMcode;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int hash;
    EDIT_MPCODE( ch, pMcode );

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

    if ( get_mprog_index( arg ) )
    {
        print_char( ch, "MPEdit: MobProg o nazwie '%s' ju¿ istnieje.\n\r", arg );
        return FALSE;
    }

    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
        for ( mob = mob_index_hash[ hash ]; mob; mob = mob->next )
            for ( mpl = mob->progs; mpl; mpl = mpl->next )
                if ( FAST_STR_CMP( mpl->name, pMcode->name ) )
                {
                    mpl->name = arg;
                }

    free_string( pMcode->name );
    pMcode->name = str_dup( arg );
    send_to_char( "Nazwa zmieniona.\n\r", ch );

    for ( hash = 0; hash < MAX_KEY_HASH; hash++ )
        for ( mob = mob_index_hash[ hash ]; mob; mob = mob->next )
            for ( mpl = mob->progs; mpl; mpl = mpl->next )
                if ( FAST_STR_CMP( mpl->name, arg ) )
                {
                    sprintf( buf, "Uaktualnianie moba %d.\n\r", mob->vnum );
                    send_to_char( buf, ch );
                    mpl->name = pMcode->name;
                    SET_BIT( mob->area->area_flags, AREA_CHANGED );
               }

    return TRUE;
}

MPEDIT( mpedit_list )
{
    do_mplist( ch, argument );
    return FALSE;
}

MPEDIT( mpedit_varcmd )
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
