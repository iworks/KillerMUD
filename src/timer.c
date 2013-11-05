	#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>

#include "merc.h"
#include "recycle.h"

char *nsprintf( char *fr, char *fmt, ... )
{
    char buf[ 2 * MSL ];
    va_list args;

    va_start( args, fmt );
    vsprintf( buf, fmt, args );
    va_end( args );

    free_string( fr );
    return str_dup( buf );
}

void printf_to_world( char *fmt, ... )
{
    char buf[ 2 * MSL ];
    va_list args;
    DESCRIPTOR_DATA *d;

    va_start( args, fmt );
    vsprintf( buf, fmt, args );
    va_end( args );

    for ( d = descriptor_list;d;d = d->next )
    {
        if ( !d->character  || d->connected < 0 ) continue;
        send_to_char( buf, d->character );
    }
    return ;
}

void printf_to_room( ROOM_INDEX_DATA *rm, char *fmt, ... )
{
    char buf[ 2 * MSL ];
    va_list args;
    CHAR_DATA *ch;

    va_start( args, fmt );
    vsprintf( buf, fmt, args );
    va_end( args );
    for ( ch = rm->people;ch;ch = ch->next_in_room )
        send_to_char( buf, ch );
    return ;
}

EVENT2_DATA *create_event2( int delay, char *act )
{
    EVENT2_DATA * ev2;

    if ( !str_cmp( act, "print" ) )
    {
        ev2 = new_event2();
        ev2->action = ACTION_PRINT;
    }
    else if ( !str_cmp( act, "wait" ) )
    {
        ev2 = new_event2();
        ev2->action = ACTION_WAIT;
    }
    else if ( !str_cmp( act, "act" ) )
    {
        ev2 = new_event2();
        ev2->action = ACTION_ACT;
    }
    else if ( !str_cmp( act, "function" ) )
    {
        ev2 = new_event2();
        ev2->action = ACTION_FUNCTION;
    }
	else if ( !str_cmp( act, "obj_to_room" ) )
    {
        ev2 = new_event2();
        ev2->action = ACTION_OBJ_TO_ROOM;
    }
	else if ( !str_cmp( act, "obj_to_char" ) )
    {
        ev2 = new_event2();
        ev2->action = ACTION_OBJ_TO_CHAR;
    }
	else if ( !str_cmp( act, "char_to_room" ) )
    {
        ev2 = new_event2();
        ev2->action = ACTION_CHAR_TO_ROOM;
    }
    else
    { /* Bad Action */
        return NULL;
    }
    ev2->delay = delay;
    ev2->next = events2;
    events2 = ev2;
    return ev2;
}

void wait_act( int delay, char *msg, void *a1, void *a2, void *a3, int type )
{
    EVENT2_DATA * ev2 = create_event2( delay, "act" );
    ev2->args[ 0 ] = str_dup( msg );
    ev2->argv[ 0 ] = a1;
    ev2->argv[ 1 ] = a2;
    ev2->argv[ 2 ] = a3;
    ev2->argi[ 0 ] = type;
    return ;
}

void wait_printf( CHAR_DATA *ch, int delay, char * fmt, ... )
{
    char buf[ 2 * MSL ];
    va_list args;
    EVENT2_DATA *ev2 = create_event2( delay, "print" );

    va_start( args, fmt );
    vsprintf( buf, fmt, args );
    va_end( args );

    ev2->args[ 0 ] = str_dup( buf );
    ev2->argv[ 0 ] = ch;
    return ;
}

/* after the delay has been met..it will freeze
 * the targeted victim for the set amount of 'dur'ation
 * ex:  wait_wait(ch,2,5); would:
 * cause player who triggered this call to be frozen
 * for 5 seconds after 2 seconds of function call
 */
void wait_wait( CHAR_DATA *ch, int delay, int dur )
{
    EVENT2_DATA * ev2 = create_event2( delay, "wait" );

    ev2->argv[ 0 ] = ch;
    ev2->argi[ 0 ] = dur;
}

void wait_function ( CHAR_DATA *ch, int delay, DO_FUN *do_fun, char *argument )
{
    EVENT2_DATA * ev2 = create_event2( delay, "function" );

    ev2->argv[ 0 ] = ch;
    ev2->do_fun = do_fun;
    ev2->args[ 0 ] = str_dup( argument );
    return ;
}

/*Opó¿nione funkcje specjalne*/

void wait_obj_to_room ( OBJ_DATA *obj, int delay, ROOM_INDEX_DATA *room )
{
   EVENT2_DATA * ev2 = create_event2( delay, "obj_to_room" );

   ev2->argv[ 0 ] = obj;
   ev2->argv[ 1 ] = room;
   return ;
}

void wait_obj_to_char ( OBJ_DATA *obj, int delay, CHAR_DATA *ch )
{
   EVENT2_DATA * ev2 = create_event2( delay, "obj_to_char" );

   ev2->argv[ 0 ] = obj;
   ev2->argv[ 1 ] = ch;
   return ;
}

void wait_mob_to_room ( CHAR_DATA *obj, int delay, ROOM_INDEX_DATA *room )
{
   EVENT2_DATA * ev2 = create_event2( delay, "char_to_room" );

   ev2->argv[ 0 ] = obj;
   ev2->argv[ 1 ] = room;
   return ;
}

CHAR_DATA *wait_mob_to_room_by_vnum(int vnum, int delay, ROOM_INDEX_DATA *room)
{
    MOB_INDEX_DATA *mob_index = get_mob_index( vnum );
    if(!mob_index)
        return NULL;
    CHAR_DATA *mob = create_mobile( mob_index );
    if ( !mob )
        return NULL;
    wait_mob_to_room(mob, delay, room);
    return mob;
}
