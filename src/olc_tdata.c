/*
	$Id: olc_tdata.c 10360 2011-06-30 08:07:28Z illi $
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


#define TEDIT( fun )           bool fun(CHAR_DATA *ch, char*argument)
extern ROOM_INDEX_DATA *find_location( CHAR_DATA *, char * );

const struct olc_cmd_type tedit_table[] =
{
	{	"commands",	show_commands	},
	{	"create",	tedit_create	},
	{	"show",		tedit_show	},
	{	"list",		tedit_list	},
	{	"?",		show_help	},
	{	"level",	tedit_level	},
	{	"active", 	tedit_active	},
	{	"type",		tedit_type	},
	{	"disarmed",	tedit_disarmed	},
	{	"failed",	tedit_failed	},
	{	"triggered",	tedit_triggered },
	{	"limit",	tedit_limit	},
	{	NULL,		0		}
};



void tedit( CHAR_DATA *ch, char *argument)
{
    TRAP_DATA *pTdata;
    char arg[MAX_INPUT_LENGTH];
    char command[MAX_INPUT_LENGTH];
    char log_buf[MAX_STRING_LENGTH];
    int cmd;
    AREA_DATA *ad;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument( argument, command);

    EDIT_TRAPDATA(ch, pTdata);

    if (pTdata)
    {
	ad = get_vnum_area( pTdata->vnum );

	if ( ad == NULL )
	{
		edit_done(ch);
		return;
	}

	if ( !IS_BUILDER(ch, ad) )
	{
		send_to_char("Tedit: Brak praw dostêpu.\n\r", ch);
		edit_done(ch);
		return;
	}
    }

    if (command[0] == '\0')
    {
        tedit_show(ch, argument);
        return;
    }

    if (!str_cmp(command, "done") )
    {
        edit_done(ch);
        return;
    }

    for (cmd = 0; tedit_table[cmd].name != NULL; cmd++)
    {
    	if (!str_prefix(command, tedit_table[cmd].name) )
    	{
    		if ((*tedit_table[cmd].olc_fun) (ch, argument) && pTdata)
    		{
    			if ((ad = get_vnum_area(pTdata->vnum)) != NULL)
    				SET_BIT(ad->area_flags, AREA_CHANGED);
                append_file_format_daily( ch, BUILD_LOG_FILE, "%s -> tedit %5d: %s", ch->name, pTdata ? pTdata->vnum : 0, arg );
        }
    		return;
    	}
    }

    interpret(ch, arg);
    return;
}


void do_tedit(CHAR_DATA *ch, char *argument)
{
    TRAP_DATA *pTdata;
    char command[MAX_INPUT_LENGTH];

    argument = one_argument(argument, command);

    if( is_number(command) )
    {
	ush_int vnum = atoi(command);

	AREA_DATA *ad;

	if ( (pTdata = get_trap_index(vnum)) == NULL )
	{
		send_to_char("TEdit: Nie istnieje pu³apka o takim vnumie.\n\r",ch);
		return;
	}

	ad = get_vnum_area(vnum);

	if ( ad == NULL )
	{
		send_to_char( "TEdit: Nie ma przypisanej krainy.\n\r", ch );
		return;
	}

	if ( !IS_BUILDER(ch, ad) )
	{
		send_to_char("TEdit: Masz niewystarczaj±ce security.\n\r", ch );
		return;
	}

	ch->desc->pEdit		= (void *)pTdata;
	ch->desc->editor	= ED_TRAPDATA;

	return;
    }

    if ( !str_cmp(command, "create") )
    {
	if (argument[0] == '\0')
	{
		send_to_char( "TEdit: Sk³adnia: tedit create [vnum]\n\r", ch );
		return;
	}

	tedit_create(ch, argument);
	return;
    }

    send_to_char( "Syntax :   tedit [vnum]\n\r", ch );
    send_to_char( "           tedit create [vnum]\n\r", ch );

    return;
}

TEDIT (tedit_create)
{
    TRAP_DATA *pTdata;
    int value = atoi(argument);
    AREA_DATA *ad;

    if (IS_NULLSTR(argument) || value < 1)
    {
	send_to_char( "TEdit: Sk³adnia: tedit create [vnum]\n\r", ch );
	return FALSE;
    }

    ad = get_vnum_area(value);

    if ( ad == NULL )
    {
    	send_to_char( "TEdit : VNUM nie przypisany ¿adnej krainie.\n\r", ch );
    	return FALSE;
    }

    if ( !IS_BUILDER(ch, ad) )
    {
        send_to_char("TEdit: Masz niewystarczaj±ce security.\n\r", ch);
        return FALSE;
    }

    if ( get_trap_index(value) )
    {
	send_to_char("TEdit: Pu³apka ju¿ istnieje.\n\r",ch);
	return FALSE;
    }

    pTdata			= new_trapdata();
    pTdata->vnum		= value;
    pTdata->next		= trap_list;
    trap_list			= pTdata;
    ch->desc->pEdit		= (void *)pTdata;
    ch->desc->editor		= ED_TRAPDATA;

    send_to_char("Nowa pu³apka zosta³a utworzona.\n\r",ch);

    return TRUE;
}

TEDIT(tedit_show)
{
    TRAP_DATA *trap;
    char buf[MAX_STRING_LENGTH];
    int i;

    EDIT_TRAPDATA(ch,trap);

    sprintf(buf,
           "Vnum:           [%d]\n\r"
           "Active:         [%s]\n\r"
	   "Level:          [%d]\n\r",
           trap->vnum, trap->active ? "YES" : "NO", trap->level);
    send_to_char(buf, ch);

    if( trap->type != 0 )
    {
	sprintf(buf, "Type:           [");

	for( i = 0; trap_type_table[i].name; i++ )
		if( IS_SET(trap->type, trap_type_table[i].flag))
		{
		    strcat(buf, trap_type_table[i].name);
		    strcat(buf, " ");
		}
	strcat(buf, "]\r\n");
	send_to_char(buf, ch);
    }
    else
	send_to_char("Type:           [ NONE ]\n\r", ch);

    sprintf(buf,"Level limit:    [%d]\n\r", get_limit( trap, LIMIT_LEVEL ));
    send_to_char(buf,ch);
    sprintf(buf,"Dex limit  :    [%d]\n\r", get_limit( trap, LIMIT_DEX ));
    send_to_char(buf,ch);
    sprintf(buf,"Skill limit:    [%d]\n\r", get_limit( trap, LIMIT_SKILL ));
    send_to_char(buf,ch);

    return FALSE;
}

TEDIT( tedit_list )
{
    int count = 1;
    TRAP_DATA *trap;
    char buf[MAX_STRING_LENGTH];
    BUFFER *buffer;
    bool fAll = !str_cmp(argument, "all");
    char blah;
    AREA_DATA *ad;

    buffer = new_buf();

    for (trap = trap_list; trap !=NULL; trap = trap->next)
	if ( fAll || ENTRE(ch->in_room->area->min_vnum, trap->vnum, ch->in_room->area->max_vnum) )
	{
		ad = get_vnum_area(trap->vnum);

		if ( ad == NULL )
			blah = '?';
		else if ( IS_BUILDER(ch, ad) )
			blah = '*';
		else
			blah = ' ';

		sprintf(buf, "[%3d] (%c) %5d\n\r", count, blah, trap->vnum );
		add_buf(buffer, buf);

		count++;
	}

    if ( count == 1 )
    {
	add_buf( buffer, "Nie znaleziono pu³apek.\n\r" );
    }

    page_to_char(buf_string(buffer), ch);
    free_buf(buffer);

    return FALSE;
}

TEDIT( tedit_level )
{
    TRAP_DATA *trap;

    EDIT_TRAPDATA(ch, trap);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
	send_to_char( "Syntax:  level [number]\n\r", ch );
	return FALSE;
    }

    trap->level = URANGE(0, atoi( argument ), 10);
    send_to_char( "Level set.\n\r", ch);
    return TRUE;
}

TEDIT( tedit_active )
{
    TRAP_DATA *trap;

    EDIT_TRAPDATA(ch, trap);

    if( trap->active )
    {
	trap->active = FALSE;
	send_to_char("Pu³apka wy³±czona.\n\r", ch);
    }
    else
    {
	trap->active = TRUE;
	send_to_char("Pu³apka w³±czona.\n\r", ch);
    }

    return TRUE;
}

TEDIT( tedit_type )
{
    TRAP_DATA *trap;
    int flag=0, i;

    EDIT_TRAPDATA(ch, trap);

    for( i = 0; trap_type_table[i].name; i++ )
	if( !str_prefix(argument, trap_type_table[i].name))
	{
	    flag = trap_type_table[i].flag;
	    break;
	}

    if( flag == 0 )
    {
	send_to_char("Nie ma takiej flagi.\n\rMo¿liwe flagi:\n\r", ch);

	for( i = 0; trap_type_table[i].name; i++ )
	    print_char(ch,"%s\n\r", trap_type_table[i].name);

	return FALSE;
    }

    trap->type ^= flag;
    print_char(ch,"Typ pu³apki prze³±czony.\n\r");
    return TRUE;
}

TEDIT(tedit_triggered)
{
    TRAP_DATA *trap;
    EDIT_TRAPDATA(ch, trap);

    if (argument[0] =='\0')
    {
       string_append(ch, &trap->triggered);
       return TRUE;
    }

    send_to_char("Syntax: triggered\n\r",ch);
    return FALSE;
}

TEDIT(tedit_disarmed)
{
    TRAP_DATA *trap;
    EDIT_TRAPDATA(ch, trap);

    if (argument[0] =='\0')
    {
       string_append(ch, &trap->disarmed);
       return TRUE;
    }

    send_to_char("Syntax: disarmed\n\r",ch);
    return FALSE;
}

TEDIT(tedit_failed)
{
    TRAP_DATA *trap;
    EDIT_TRAPDATA(ch, trap);

    if (argument[0] =='\0')
    {
       string_append(ch, &trap->failed_disarm);
       return TRUE;
    }

    send_to_char("Syntax: failed\n\r",ch);
    return FALSE;
}

TEDIT(tedit_limit)
{
    TRAP_DATA *trap;
    char arg[MAX_INPUT_LENGTH];

    EDIT_TRAPDATA(ch, trap);

    argument = one_argument(argument, arg);

    if (argument[0] =='\0' || !is_number(argument))
    {
	send_to_char("limit dex/skill/level [value]\n\r", ch);
	return FALSE;
    }

    if( !str_prefix(arg, "dex"))
    {
	set_trap_limit(trap, LIMIT_DEX, URANGE(4, atoi(argument), 27));
	send_to_char("Limit dex ustawiony.\n\r", ch);
	return TRUE;
    }
    else if( !str_prefix(arg, "skill"))
    {
	set_trap_limit(trap, LIMIT_SKILL, URANGE(2, atoi(argument), 99));
	send_to_char("Limit skill ustawiony.\n\r", ch);
	return TRUE;
    }
    else if( !str_prefix(arg, "level"))
    {
	set_trap_limit(trap, LIMIT_LEVEL, URANGE(2, atoi(argument), 29));
	send_to_char("Limit level ustawiony.\n\r", ch);
	return TRUE;
    }
    else
    {
	send_to_char("limit dex/skill/level [value]\n\r", ch);
	return FALSE;
    }
}
