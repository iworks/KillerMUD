#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "todelete.h"

#define TODELETE_DELAY 	(7 * 86400)			// days * seconds per one day
#define TODELETE_FILE 	"../system/todelete.txt"		// todelete data file

extern const char echo_on_str [];

extern void do_function args( ( CHAR_DATA *ch, DO_FUN *do_fun, char *argument ) );
extern void do_delete	args( ( CHAR_DATA *ch, char *argument ) );

TODELETE_DATA *todelete_list;				// todelete list
TODELETE_DATA *todelete_free = NULL;			// todelete free objects list


TODELETE_DATA *new_todelete( void )			// create new todelete object
{
    static TODELETE_DATA todelete_zero;			// declare the static todelete_zero variable
    TODELETE_DATA *todelete;				// declare the todelete variable

    if ( todelete_free == NULL )			// if the free list is empty
    {
        CREATE( todelete, TODELETE_DATA, 1 );		// create the object
    }
    else						// otherwise
    {
	todelete      = todelete_free;			// take the object from the free list
	todelete_free = todelete_free->next;		// remove the object from the free list
    }

    *todelete 	   	= todelete_zero;		// ???
    VALIDATE( todelete );				// validate the object
    todelete->name 	= &str_empty[ 0 ];		// set the empty string variable
    todelete->timestamp = 0;				// set the timestamp to zero
    todelete->next   	= NULL;				// set the next in list object to NULL

    return todelete;					// return the object
}

void free_todelete( TODELETE_DATA *todelete )		// free the todelete object
{
    if ( !IS_VALID( todelete ) )			// if the object is already invalid
    {
	return;						// exit the function
    }

    free_string( todelete->name );			// free the string variable
    INVALIDATE( todelete );				// invalidate the object

    todelete->next = todelete_free;			// add the object to the beginning of the free list
    todelete_free  = todelete;				// set the beginning of the free list to the object

    return;						// exit the function
}

TODELETE_DATA *find_todelete( char *name )		// find the ch on todelete list
{
    TODELETE_DATA *todelete = NULL;			// decalre the object variable

    for ( todelete = todelete_list; todelete; todelete = todelete->next ) // parse the todelete list
    {
	if ( !strcmp( name, todelete->name ) )		// found a match
	{
	    break;					// exit the search loop
	}
    }

    return todelete; 					// return the object
}

void save_todelete_list( )				// save the todolist to a file
{
    TODELETE_DATA *todelete = NULL;			// create an instance of the todelete object
    FILE	  *fp;					// define the file variable

    if ( !( fp = fopen( TODELETE_FILE, "w" ) ) )	// open the todelete file for writing
    {
        bug( "save_todelete_list: fopen failed to open file for writing", 0 );	// report an error
	return;						// exit the function
    }

    for ( todelete = todelete_list; todelete; todelete = todelete->next )	// parse the todelete list
    {
	fprintf( fp, "%s %ld\n", todelete->name, todelete->timestamp );		// save each todelete item
    }

    fprintf( fp, "End\n" );				// write the end of the file marker
    fclose( fp );					// close the file

    return;						// exit the function
}

void add_todelete( char *name, time_t timestamp )	// add a todelete object to the list
{
    TODELETE_DATA *todelete = find_todelete( name );	// declare object variable and try to find it on the list

    if ( todelete )					// check if the todelete is on the list
    {
	char buf[MSL];					// declare a char array

	sprintf( buf, "%s already on the todelete list.", name ); // create a bug message
	bug( buf, 0 ); 					// report a bug
	return;
    }

    todelete 		= new_todelete( ); 		// create object instance
    todelete->name 	= str_dup( name );		// set the name
    todelete->timestamp = timestamp;			// set the timestamp
    todelete->next 	= todelete_list;		// append the list to to object
    todelete_list 	= todelete;			// make object the beginning of the list

    return;						// exit the function
}

void remove_todelete( char *name )			// removes a todelete object from the list
{
    TODELETE_DATA *todelete      = NULL;		// create an instance of an object
    TODELETE_DATA *prev_todelete = NULL;		// create an instance of an object (used to unlink objects from the list)

    for ( todelete = todelete_list; todelete; todelete = todelete->next ) // parse the todelete list
    {
	if ( !strcmp( name, todelete->name ) )		// found a match
	{
	    if ( prev_todelete )			// if the object is in the middle
	    {
		prev_todelete->next = todelete->next; 	// unlink the object from the list
	    }
	    else					// otherwise
	    {
		todelete_list = todelete->next;		// unlink the object from the beginning of the list
	    }

	    free_todelete( todelete );			// recycle the object
	    break;					// exit the search loop
	}

	prev_todelete = todelete;			// set the previous item on the list
    }

    return;						// exit the function
}

void load_todelete_list( )				// read the todelete list from file
{
    char *name;						// define name pointer variable
    int   timestamp;					// define timestamp variable
    FILE *fp;						// define file pointer variable

    todelete_list = NULL;				// set the todelete list to NULL

    if ( !( fp = fopen( TODELETE_FILE, "r" ) ) )	// check if file exists and if so open it
    {
	save_todelete_list( );				// if not create it
	return;						// and exit the function
    }

    for ( ; ; )						// parse the file
    {
	name = fread_word( fp );			// read the name

	if ( !strcmp( name, "End" ) )			// if no further todelete names
	{
	    break;					// exit the parse loop
	}
	
	if ( feof( fp ) )                               // perhaps we've reached EOF without finding "End"
	{
	    bug("load_todelete_list: EOF reached without finding \"End\"", 0);
	    break;
	}

	timestamp = fread_number( fp );			// read the timestamp
	add_todelete( name, timestamp );		// add new todelete item to the list
    }

    fclose( fp );					// close the file

    return;						// exit the function
}

int time_diff( time_t time1, time_t time2, char format )// get time format
{
    long diff    = difftime( time1, time2 );		// set the difference
    long days    = diff / 86400;			// set the days
    int  hours   = ( diff - ( days * 86400 ) ) / 3600;	// set the hours
    int  minutes = ( diff - ( days * 86400 ) - ( hours * 3600 ) ) / 60;			// set the minutes
    int  seconds = ( diff - ( days * 86400 ) - ( hours * 3600 ) - ( minutes * 60 ) ); 	// set the seconds

    switch ( format )					// check which value to return
    {
	case 's': return seconds; break;		// exit the function with seconds
	case 'm': return minutes; break;		// exit the function with minutes
	case 'h': return hours;   break;		// exit the function with hours
	case 'd': return days;    break;		// exit the function with days
	case ' ': return diff;    break;		// exit the function with the difference
	default:  return -1;      break;		// exit the function with default value
    }

    return -1;						// exit the function with default value
}

char *todelete_time( char *name, char format )		// get the ch todelete time string
{
    TODELETE_DATA *todelete = find_todelete( name );	// declare and set the todelete variable
    long	   days     = time_diff( todelete->timestamp, current_time, 'd' );	// declare and set the days variable
    int		   hours    = time_diff( todelete->timestamp, current_time, 'h' );	// declare and set the hours variable
    int		   minutes  = time_diff( todelete->timestamp, current_time, 'm' );	// declare and set the minutes variable
    int		   seconds  = time_diff( todelete->timestamp, current_time, 's' );	// declare and set the seconds variable
    char 	   buf[MSL];				// declare a char array

    switch ( format )					// get format
    {
	default:
	case 's': sprintf( buf, "%ldd %dh %dm %ds", days, hours, minutes, seconds ); break;	// prepare the time string
	case 'm': sprintf( buf, "%ldd %dh %dm",     days, hours, minutes 	  ); break;	// prepare the time string
	case 'h': sprintf( buf, "%ldd %dh",         days, hours 		  ); break;	// prepare the time string
	case 'd': sprintf( buf, "%ldd",             days 			  ); break;	// prepare the time string
    }

    return str_dup( buf );				// return the time string
}

void delete_todelete( )					// delete chs from the todelete list
{
    TODELETE_DATA *todelete;				// declare the todelete variable

    for ( todelete = todelete_list; todelete; todelete = todelete->next )	// parse the todelete list
    {
	if ( time_diff( todelete->timestamp, current_time, ' ' ) < 0 )		// if the time is up
	{
	    CHAR_DATA *dch;				// declare the ch variable

	    for ( dch = char_list; dch; dch = dch->next )	// parse the currently playing ch list
	    {
		if ( !strcmp( dch->name, todelete->name ) )	// if found the ch
		{
		    break;				// exit the find ch loop
		}
	    }

	    if ( !dch )					// if no ch found
	    {
		if ( !( dch = load_char_remote( todelete->name ) ) )	// if can't load the ch from file
		{
		    char buf[MSL];			// declare a char array

		    sprintf( buf, "delete_todelete: can't find player: %s", todelete->name );	// prepare an error message
		    bug( buf, 0 );			// send the error message to the bug channel
		    remove_todelete( todelete->name );	// fix the todelete list

		    return;				// exit the function
		}
	    }

	    do_function( dch, &do_delete, "" );		// delete the ch
	}
    }

    return;						// exit the function
}

void show_todelete( CHAR_DATA *ch )			// show a list of chs to be deleted
{
    TODELETE_DATA *todelete;				// declara the todelete variable
    char	   buf[MSL];				// declare a char array

    send_to_char( "Todelete list: \n\r", ch );		// send list header

    for ( todelete = todelete_list; todelete; todelete = todelete->next )	// parse the todelete list
    {
        sprintf( buf, "%s: [%s] %s", todelete->name, todelete_time( todelete->name, 's' ), ctime( &todelete->timestamp ) );	// prepare the todelete message
        send_to_char( buf, ch );			// send the todelete message
    }

    send_to_char( "End.\n\r", ch );			// send list footer

    return;						// exit the function
}

void process_todelete( DESCRIPTOR_DATA *d, CHAR_DATA *ch ) 	// process the todelete request
{
    if ( ch->pcdata->todelete != 0 )	 		// if on todelete list
    {
	ch->pcdata->todelete = 0;			// clear the pcdata->todelete value
	save_char_obj( ch, FALSE, FALSE );		// save the change
	remove_todelete( ch->name ); 			// remove ch from todelete list
	save_todelete_list( );				// save the change
	write_to_buffer( d, echo_on_str, 0 );		// prepare message
	write_to_buffer( d, "\n\r      Przerwano kasowanie postaci.\n\r", 0 ); 	// send message
    }
    else				 		// otherwise
    {
	char buf[MSL];					// declare char array variable

	ch->pcdata->todelete = ( TODELETE_DELAY ) + current_time;	// set pcdata->todelete
	save_char_obj( ch, FALSE, FALSE );		// save the change
	add_todelete( ch->name, ch->pcdata->todelete ); // add ch to todelete list
	save_todelete_list( );				// save the change
	write_to_buffer( d, echo_on_str, 0 );		// send empty string
	sprintf( buf, "\n\r      Postaæ zostanie skasowana za %s.\n\r", todelete_time( ch->name, 's' ) );	// prepare message
	write_to_buffer( d, buf, 0 );			// send message
    }

    return;						// exit the function
}

void todelete_menu_mod( CHAR_DATA *ch )			// display menu option
{
    if ( ch->pcdata->todelete != 0 )			// if is todelete
    {
        char buf[MSL];					// declare char array

        sprintf( buf, "      *   3)  Skasowanie za: %-11s *\n\r", todelete_time( ch->name, 'h' ) );	// prepare message
	send_to_char( buf, ch );			// send message
    }
    else						// otherwise
    {
        send_to_char( "      *   3)  Skasuj postaæ              *\n\r", ch ); 	// send alternative message
    }

    return;						// exit the function
}

void check_todelete( CHAR_DATA *ch )			// check the todelete list against pcdata->todelete timestamp
{
    TODELETE_DATA *todelete = find_todelete( ch->name );// declare the todelete variable and try to find the object on the todelete list
    char	   buf[MSL];				// declara a char array

    buf[0] = '\0';					// zero out the first char in the array

    if ( ch->pcdata->todelete != 0 )			// if ch has a todelete timestamp
    {
	if ( todelete )					// and if ch is on the todelete list
	{
	    if ( ch->pcdata->todelete != todelete->timestamp ) 	// and if the timestamps differ
	    {
		sprintf( buf, "check_todelete: %s's todelete timestamp (%ld) differs from the one on the todelete list (%ld) - resetting list entry", ch->name, ch->pcdata->todelete, todelete->timestamp ); 	// prepare error message
		remove_todelete( ch->name );		// remove ch from the todelete list
		add_todelete( ch->name, ch->pcdata->todelete );	// add ch to the todelete list again
	    }
	}
	else						// but ch is not on the todelete list
	{
	    sprintf( buf, "check_todelete: %s has a todelete timestamp but is not on the todelete list - adding to the list", ch->name ); 	// prepare error message
	    add_todelete( ch->name, ch->pcdata->todelete );	// add ch to the todelete list
	}
    }
    else						// otherwise if ch has no todelete timestamp
    {
	if ( todelete )					// but is on the todelete list
	{
	    sprintf( buf, "check_todelete: %s has no todelete timestamp but is on the todelete list - removing from the list", ch->name ); 	// prepare error message
	    remove_todelete( ch->name );		// remove ch from the todelete list
	}
    }

    if ( buf[0] != '\0' )				// if error message not empty
    {
	bug( buf, 0 );					// send it to the bug channel
	save_todelete_list( );				// save the changes
    }

    return;						// exit the function
}
