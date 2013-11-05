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
#include "randdesc.h"
#include "tables.h"
#include "progs.h"

#define RDESC( fun )           DESC_DATA* fun(ROOM_INDEX_DATA *pRoom, char *argument)
#define RDBOOL( fun )          bool fun(ROOM_INDEX_DATA *pRoom, char *argument)

const struct randdesc_cmd_type rdesc_table[] =
{
	{	"grouprand",    rdesc_grouprand	},
	{	"show",		    rdesc_show	    },
	{	NULL,		0		}
};

const struct rdbool_cmd_type rdbool_table[] =
{
	{	"random",	    rdbool_random	},
	{	"sector",	    rdbool_sector	},
	{	"day",	        rdbool_day  	},
	{	"month",	    rdbool_month	},
	{	"dayweek",	    rdbool_dayweek	},
	{	"hour",	        rdbool_hour 	},
	{	"room",	        rdbool_room 	},
	{	NULL,		0		}
};

void command_dispatcher(ROOM_INDEX_DATA *pRoom, char *command, char *output)
{
    DESC_DATA *pDesc;
    char *args, *d;
    char control[MAX_STRING_LENGTH];
    int cmd;
    bool check = TRUE;
    bool found = FALSE;

    args = command;


    for( ; ; )
    {
        args = one_argument( args, control );

        if ( control[0] == '\0' )
            break;

        if ( control[0] != '#' )
            continue;

        if ( !str_cmp( control, "#newline" ) )
        {
            *output++ = '\n';
            *output++ = '\r';
            check = TRUE;
        } else if ( !str_cmp( control, "#pass" ) )
        {
            d = args;
            while( *d ) *output++ = *d++;
            *output++ = '\n';
            *output++ = '\r';
            break;
        } else if ( !str_cmp( control, "#else" ) )
        {
            check = !check;
        }
        else
        {
            found = FALSE;
            if (check)
            {
                pDesc = NULL;
                for (cmd = 0; rdesc_table[cmd].name != NULL; cmd++)
                {
                	if (!str_prefix(control+1, rdesc_table[cmd].name) )
                	{
               		    pDesc = ((*rdesc_table[cmd].function) (pRoom, args));
               		    found = TRUE;
                		break;
                	}
                }

                if ( pDesc && pDesc->description && pDesc->description[0] != '\0')
                {
                    d = pDesc->description;
                    while( *d ) *output++ = *d++;

                    do
                    {
                        output--;
                    } while ( *output == '\r' || *output == '\n');
                    output++;
                    *output++ = ' ';
                }
            }

            if ( !found )
            {
                for (cmd = 0; rdbool_table[cmd].name != NULL; cmd++)
                {
                	if (!str_prefix(control+1, rdbool_table[cmd].name) )
                	{
               		    check = check && ((*rdbool_table[cmd].function) (pRoom, args));
                		break;
                	}
                }
            }
        }
   }
    *output = '\0';
    return;
}

void create_rand_desc( ROOM_INDEX_DATA *pRoom, char *src_desc, char *dest)
{
    char command[MAX_STRING_LENGTH];
    char comm_out[MAX_STRING_LENGTH];
    char *c, *out;
    char *original;

    command[0] = '\0';
    original = dest;

    while( *src_desc )
    {
        if ( *src_desc == '#' )
        {
            c = command;

            while( *src_desc )
            {
                if ( *src_desc == '%' )
                {
                    *c++ = *src_desc++;
                    break;
                }

                if ( *src_desc == '\n' )
                {
                    src_desc++;
                    if ( *src_desc != '\r' )
                        src_desc--;
                    break;
                }

                *c++ = *src_desc++;
            }

            *c = '\0';

            command_dispatcher( pRoom, command, comm_out );
            out = comm_out;
            while( *out ) *dest++ = *out++;
        }

        *dest++ = *src_desc++;
    }

    *dest++ = '\n';
    *dest++ = '\r';
    *dest = '\0';

    return;
}

RDESC( rdesc_grouprand )
{
    DESC_DATA *list;
    char arg[MAX_STRING_LENGTH];
    int group, count, rand;

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number(arg) )
        return NULL;

    group = atoi(arg);
    count = 0;

    for( list = pRoom->area->desc_data; list; list = list->next )
    if ( list->group == group )
        count++;

    if ( count == 0 )
        return NULL;

    rand = number_range(1, count);
    count = 0;

    for( list = pRoom->area->desc_data; list; list = list->next )
    {
        if ( list->group != group )
            continue;

        count++;

        if ( count >= rand )
            break;
    }

    return list;
}

RDESC( rdesc_show )
{
    char arg[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
        return NULL;

    return get_desc_index( pRoom->area, arg );
}

RDBOOL( rdbool_random )
{
    char arg1[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' || !is_number(arg1) )
        return FALSE;

    return number_percent() < atoi( arg1 );
}

RDBOOL( rdbool_sector )
{
    char arg1[MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
        return FALSE;

    return !str_cmp( sector_table[pRoom->sector_type].name, arg1 );
}

RDBOOL( rdbool_day )
{
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    int oper;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg2) )
        return FALSE;

	if ( (oper = keyword_lookup( fn_evals, arg1 )) < 0 )
	    return FALSE;

    return num_eval( time_info.day+1, oper, atoi(arg2) );
}

RDBOOL( rdbool_month )
{
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    int oper;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg2) )
        return FALSE;

	if ( (oper = keyword_lookup( fn_evals, arg1 )) < 0 )
	    return FALSE;

    return num_eval( time_info.month+1, oper, atoi(arg2) );
}

RDBOOL( rdbool_dayweek )
{
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    int oper;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg2) )
        return FALSE;

	if ( (oper = keyword_lookup( fn_evals, arg1 )) < 0 )
	    return FALSE;

    return num_eval( ( time_info.day + 1 ) % 7 + 1, oper, atoi(arg2) );

}

RDBOOL( rdbool_hour )
{
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    int oper;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg2) )
        return FALSE;

	if ( (oper = keyword_lookup( fn_evals, arg1 )) < 0 )
	    return FALSE;

    return num_eval( time_info.hour, oper, atoi(arg2) );
}

RDBOOL( rdbool_room )
{
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    int oper;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg2) )
        return FALSE;

	if ( (oper = keyword_lookup( fn_evals, arg1 )) < 0 )
	    return FALSE;

    return num_eval( pRoom->vnum, oper, atoi(arg2) );
}
