/*
	$Id: olc_descdata.c 7666 2009-07-03 12:04:22Z illi $
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
#include "traps.h"


#define RDEDIT( fun )           bool fun(CHAR_DATA *ch, char*argument)
extern ROOM_INDEX_DATA *find_location( CHAR_DATA *, char * );

const struct olc_cmd_type rdedit_table[] =
{
	{	"commands",	    show_commands		},
	{	"create",	    rdedit_create		},
	{	"show",		    rdedit_show	    	},
	{	"list",		    rdedit_list	    	},
	{	"?",		    show_help	    	},
	{	"randdesc",	    rdedit_randdesc		},
	{	"adddesc",	    rdedit_adddesc		},
	{	"deldesc",	    rdedit_deldesc		},
	{	"eddesc",	    rdedit_eddesc		},
	{	"showdesc",	    rdedit_showdesc		},
	{	"listdesc",	    rdedit_listdesc		},
	{	"chgroup",	    rdedit_chgroup		},
	{	"clonedesc",	rdedit_clonedesc	},
	{	NULL,		0		}
};


void rdedit( CHAR_DATA *ch, char *argument)
{
    RAND_DESC_DATA *pDdata;
    char arg[MAX_INPUT_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;
    AREA_DATA *ad;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument( argument, command);

    EDIT_RDESCDATA(ch, pDdata);

    if (pDdata)
    {
	ad = get_vnum_area( pDdata->vnum );

	if ( ad == NULL )
	{
		edit_done(ch);
		return;
	}

	if ( !IS_BUILDER(ch, ad) )
	{
		send_to_char("DescEdit: Brak praw dostêpu.\n\r", ch);
		edit_done(ch);
		return;
	}
    }

    if (command[0] == '\0')
    {
        rdedit_show(ch, argument);
        return;
    }

    if (!str_cmp(command, "done") )
    {
        edit_done(ch);
        return;
    }

    for (cmd = 0; rdedit_table[cmd].name != NULL; cmd++)
    {
	if (!str_prefix(command, rdedit_table[cmd].name) )
	{
		if ((*rdedit_table[cmd].olc_fun) (ch, argument) && pDdata)
			if ((ad = get_vnum_area(pDdata->vnum)) != NULL)
				SET_BIT(ad->area_flags, AREA_CHANGED);
		return;
	}
    }

    interpret(ch, arg);
    return;
}


void do_rdedit(CHAR_DATA *ch, char *argument)
{
    RAND_DESC_DATA *pDdata;
    char command[MAX_INPUT_LENGTH];

    argument = one_argument(argument, command);

    if( is_number(command) )
    {
	ush_int vnum = atoi(command);

	AREA_DATA *ad;

	if ( (pDdata = get_rdesc_index(vnum)) == NULL )
	{
		send_to_char("RDEdit: Nie istnieje taki opis.\n\r",ch);
		return;
	}

	ad = get_vnum_area(vnum);

	if ( ad == NULL )
	{
		send_to_char( "RDEdit: Nie ma przypisanej krainy.\n\r", ch );
		return;
	}

	if ( !IS_BUILDER(ch, ad) )
	{
		send_to_char("RDEdit: Masz niewystarczaj±ce security.\n\r", ch );
		return;
	}

	ch->desc->pEdit		= (void *)pDdata;
	ch->desc->editor	= ED_RDESCDATA;

	return;
    }

    if ( !str_cmp(command, "create") )
    {
	if (argument[0] == '\0')
	{
		send_to_char( "RDEdit: Sk³adnia: rdedit create [vnum]\n\r", ch );
		return;
	}

	rdedit_create(ch, argument);
	return;
    }

    send_to_char( "RDEdit: Sk³adnia:   rdedit [vnum]\n\r", ch );
    send_to_char( "                    rdedit create [vnum]\n\r", ch );

    return;
}

RDEDIT (rdedit_create)
{
    RAND_DESC_DATA *pDdata;
    int value = atoi(argument);
    AREA_DATA *ad;

    if (IS_NULLSTR(argument) || value < 1)
    {
    	send_to_char( "RDEdit: Sk³adnia: rdedit create [vnum]\n\r", ch );
    	return FALSE;
    }

    ad = get_vnum_area(value);

    if ( ad == NULL )
    {
    	send_to_char( "RDEdit: VNUM nie przypisany ¿adnej krainie.\n\r", ch );
    	return FALSE;
    }

    if ( !IS_BUILDER(ch, ad) )
    {
        send_to_char("RDEdit: Masz niewystarczaj±ce security.\n\r", ch);
        return FALSE;
    }

    if ( get_rdesc_index(value) )
    {
    	send_to_char("RDEdit: Opis o tym vnumie ju¿ istnieje.\n\r",ch);
    	return FALSE;
    }

    pDdata			    = new_rdescdata();
    pDdata->vnum		= value;
    pDdata->area        = ad;
    pDdata->next		= rand_desc_list;
    rand_desc_list		= pDdata;
    ch->desc->pEdit		= (void *)pDdata;
    ch->desc->editor	= ED_RDESCDATA;

    send_to_char("Nowy opis zosta³ utworzony.\n\r",ch);

    return TRUE;
}

RDEDIT(rdedit_show)
{
    RAND_DESC_DATA *rdesc;
    char buf[MAX_STRING_LENGTH];

    EDIT_RDESCDATA(ch,rdesc);

    sprintf(buf,
           "{GVnum{x:           [%d]\n\r"
           "{GArea{x:           [%5d] %s\n\r"
	       "{GRand Desc{x:\n\r%s\n\r",
           rdesc->vnum, rdesc->area->vnum, rdesc->area->name,
           rdesc->rand_desc );
    send_to_char(buf, ch);

    return FALSE;
}

RDEDIT( rdedit_list )
{
    int count = 1;
    RAND_DESC_DATA *rdesc;
    char buf[MAX_STRING_LENGTH];
    BUFFER *buffer;
    bool fAll = !str_cmp(argument, "all");
    char blah;
    AREA_DATA *ad;

    buffer = new_buf();

    for (rdesc = rand_desc_list; rdesc; rdesc = rdesc->next)
	if ( fAll || rdesc->area == ch->in_room->area )
	{
		ad = rdesc->area;

		if ( ad == NULL )
			blah = '?';
		else if ( IS_BUILDER(ch, ad) )
			blah = '*';
		else
			blah = ' ';

		sprintf(buf, "[%3d] (%c) %5d\n\r", count, blah, rdesc->vnum );
		add_buf(buffer, buf);

		count++;
	}

    if ( count == 1 )
    {
	add_buf( buffer, "Nie znaleziono opisów.\n\r" );
    }

    page_to_char(buf_string(buffer), ch);
    free_buf(buffer);

    return FALSE;
}

RDEDIT(rdedit_randdesc)
{
    RAND_DESC_DATA *rdesc;
    EDIT_RDESCDATA(ch, rdesc);

    if (argument[0] =='\0')
    {
       string_append(ch, &rdesc->rand_desc);
       return TRUE;
    }

    send_to_char("Syntax: randdesc\n\r",ch);
    return FALSE;
}

RDEDIT(rdedit_adddesc)
{
    RAND_DESC_DATA *rdesc;
    DESC_DATA *desc;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    EDIT_RDESCDATA(ch, rdesc);

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] != '\0' )
    {
        if ( get_desc_index( rdesc->area, arg1 ) )
        {
            send_to_char( "Istnieje ju¿ opis o takiej nazwie w tej krainie.\n\r", ch );
            return FALSE;
        }

        desc = new_descdata();
        desc->name = str_dup(arg1);
        desc->group = 0;
        if ( arg2[0] != '\0' && is_number(arg2) )
            desc->group = atoi(arg2);

        desc->next = rdesc->area->desc_data;
        rdesc->area->desc_data = desc;

    	print_char( ch, "Tworzê opis '%s' w grupie %d.\n\r", desc->name, desc->group );
        string_append( ch, &desc->description );
        return TRUE;
    }

    send_to_char("RDEdit: Sk³adnia: addesc <name> [group]\n\r", ch );
    return FALSE;
}

RDEDIT(rdedit_deldesc)
{
    RAND_DESC_DATA *rdesc;
    DESC_DATA *desc, *prev;
    char arg1[MAX_INPUT_LENGTH];

    EDIT_RDESCDATA(ch, rdesc);

    argument = one_argument( argument, arg1 );

    if ( arg1[0] != '\0' )
    {
        if ( ( desc = get_desc_index( rdesc->area, arg1 ) ) == NULL )
        {
            send_to_char( "RDEdit: W tej krainie nie ma opisu o takiej nazwie.\n\r", ch );
            return FALSE;
        }

    	if ( desc == rdesc->area->desc_data )
    	{
        	    rdesc->area->desc_data = desc->next;
    	}
    	else
    	{
    	    for ( prev = rdesc->area->desc_data; prev != NULL; prev = prev->next )
    	    {
        		if ( prev->next == desc )
        		{
        		    prev->next = desc->next;
        		    break;
        		}
    	    }
    	}

    	free_descdata( desc );

    	send_to_char( "Opis usuniety.\n\r", ch );
        return TRUE;
    }

    send_to_char("Syntax: eddesc <name>\n\r", ch );
    return FALSE;
}

RDEDIT(rdedit_eddesc)
{
    RAND_DESC_DATA *rdesc;
    DESC_DATA *desc;
    char arg1[MAX_INPUT_LENGTH];

    EDIT_RDESCDATA(ch, rdesc);

    argument = one_argument( argument, arg1 );

    if ( arg1[0] != '\0' )
    {
        if ( ( desc = get_desc_index( rdesc->area, arg1 ) ) == NULL )
        {
            send_to_char( "RDEdit: W tej krainie nie ma opisu o takiej nazwie.\n\r", ch );
            return FALSE;
        }

        string_append( ch, &desc->description );
        return TRUE;
    }

    send_to_char("RDEdit: Sk³adnia: eddesc <name>\n\r", ch );
    return FALSE;
}

RDEDIT(rdedit_showdesc)
{
    RAND_DESC_DATA *rdesc;
    DESC_DATA *desc;
    char arg1[MAX_INPUT_LENGTH];

    EDIT_RDESCDATA(ch, rdesc);

    argument = one_argument( argument, arg1 );

    if ( arg1[0] != '\0' )
    {
        if ( ( desc = get_desc_index( rdesc->area, arg1 ) ) == NULL )
        {
            send_to_char( "RDEdit: W tej krainie nie ma opisu o takiej nazwie.\n\r", ch );
            return FALSE;
        }

        print_char( ch, "{CNazwa{x:  %s\n\r"
                        "{CGrupa{x:  [%d]\n\r"
                        "{COpis{x: \n\r%s\n\r",
                        desc->name, desc->group, desc->description );
       return FALSE;
    }

    send_to_char("Syntax: showdesc <name>\n\r", ch );
    return FALSE;
}

RDEDIT(rdedit_listdesc)
{
    RAND_DESC_DATA *rdesc;
    DESC_DATA *desc;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int group, count;
    BUFFER		*buffer;

    EDIT_RDESCDATA(ch, rdesc);

    one_argument( argument, arg );

    if ( arg[0] == '\0' || ( arg[0] != '\0' && is_number(arg) ) )
    {
        if ( arg[0] != '\0' )
            group = atoi(arg);

        buffer = new_buf();

        count = 1;
        for ( desc = rdesc->area->desc_data; desc; desc = desc->next )
        if ( arg[0] == '\0' || ( arg[0] != '\0' && desc->group == group ) )
        {
            if ( count == 1 )
            {
                sprintf( buf, "Nr  Group   Name\n\r" );
                add_buf( buffer, buf );
                sprintf( buf, "--- ------- --------\n\r" );
                add_buf( buffer, buf );
            }
            sprintf( buf, "%3d %7d %s\n\r", count, desc->group, desc->name );
            add_buf( buffer, buf );
            count++;
        }

        page_to_char( buf_string(buffer), ch );
        free_buf(buffer);
        return FALSE;
    }

    send_to_char("Syntax: showdesc <name>\n\r", ch );
    return FALSE;
}

RDEDIT(rdedit_chgroup)
{
    RAND_DESC_DATA *rdesc;
    DESC_DATA *desc;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    EDIT_RDESCDATA(ch, rdesc);

    argument = one_argument( argument, arg1 );
               one_argument( argument, arg2 );

    if ( arg1[0] != '\0' && arg2[0] != '\0' && is_number(arg2) )
    {
        if ( ( desc = get_desc_index( rdesc->area, arg1 ) ) == NULL )
        {
            send_to_char( "RDEdit: W tej krainie nie ma opisu o takiej nazwie.\n\r", ch );
            return FALSE;
        }


        desc->group = atoi(arg2);
        print_char( ch, "Grupa opisu '%s' zmieniona na %d.\n\r", desc->name, desc->group );
        return TRUE;
    }

    send_to_char("Syntax: chgroup <name> <number>\n\r", ch );
    return FALSE;
}

RDEDIT( rdedit_clonedesc )
{
    RAND_DESC_DATA *rdesc;
    DESC_DATA *desc, *s_desc;
    AREA_DATA *cArea;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    EDIT_RDESCDATA(ch, rdesc);

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] != '\0' )
    {
    	if ( !is_number( arg1 ) ||  ( cArea = get_area_data( atoi( arg1 ) ) ) == NULL )
    	{
            send_to_char( "RDEdit: Nie istnieje kraina o takim numerze.\n\r", ch );
            return FALSE;
    	}

		if ( !str_cmp( arg2, "all" ) )
		{
			int count = 0;
		    for( s_desc = cArea->desc_data; s_desc; s_desc = s_desc->next )
		    {
		        desc = new_descdata();
		        desc->name = str_dup( s_desc->name );
		        desc->group = s_desc->group;
		        desc->description = str_dup( s_desc->description );

		        desc->next = rdesc->area->desc_data;
		        rdesc->area->desc_data = desc;

				count++;
		    }

			print_char( ch, "RDEdit: Przekopiowano opisów: %d\n\r", count );
	        return TRUE;
		}
		else
		{
	        if ( ( s_desc = get_desc_index( cArea, arg2 ) ) == NULL )
	        {
	            send_to_char( "RDEdit: Nie ma opisu o takiej nazwie w podanej krainie.\n\r", ch );
	            return FALSE;
	        }

	        desc = new_descdata();
	        desc->name = str_dup( s_desc->name );
	        desc->group = s_desc->group;
	        desc->description = str_dup( s_desc->description );

	        desc->next = rdesc->area->desc_data;
	        rdesc->area->desc_data = desc;

	    	print_char( ch, "RDEdit: Kopiujê opis '%s' w grupie %d.\n\r", desc->name, desc->group );
	        return TRUE;
	    }
    }

    send_to_char("RDEdit: Sk³adnia: clonedesc <area number> <desc name>|all\n\r", ch );
    return FALSE;
}
