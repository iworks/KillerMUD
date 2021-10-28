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
 * KILLER MUD is copyright 1999-2011 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Kulesza Kamil         (kamili@mud.pl                 ) [Kamili    ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (tzdziech@gmail.com            ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: comm.c 11439 2012-06-18 19:26:06Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/comm.c $
 *
 */
/*********************************************************************
 * This file contains all of the OS-dependent stuff:                 *
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.          *
 *                                                                   *
 * The data flow for input is:                                       *
 *  Game_loop ---> Read_from_descriptor ---> Read                    *
 *  Game_loop ---> Read_from_buffer                                  *
 *                                                                   *
 * The data flow for output is:                                      *
 *  Game_loop ---> Process_Output ---> Write_to_descriptor -> Write  *
 *                                                                   *
 * The OS-dependent functions are Read_from_descriptor and           *
 * Write_to_descriptor.                                              *
 * -- Furey  26 Jan 1993                                             *
 *********************************************************************/
#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <err.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include "merc.h"
#include "nanny.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"
#include "lang.h"
#include "name_check.h"
#include "clans.h"
#include "telnet.h"
#include "todelete.h" // delayed ch delete by Fuyara
#include "projects.h"
#include "money.h"

/**
 * exping.c
 */
int get_percent_exp args ( ( CHAR_DATA *ch ) );
/*
 * Malloc debugging stuff.
 */
#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern	int	malloc_debug	args( ( int  ) );
extern	int	malloc_verify	args( ( void ) );
#endif

const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str	[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const	char 	go_ahead_str	[] = { IAC, GA, '\0' };

/* mccp: compression negotiation strings */
#ifdef MCCP
const   char    eor_on_str      [] = { IAC, WILL, TELOPT_EOR, '\0' };
const   char    compress_on_str [] = { IAC, WILL, TELOPT_COMPRESS, '\0' };
const   char    compress2_on_str [] = { IAC, WILL, TELOPT_COMPRESS2, '\0' };
#endif

#ifdef ASYNCH_DNS
#ifdef CYGWIN
pthread_mutex_t lookup_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
#endif

#if	defined(linux)
//int	close		args( ( int fd ) );
//int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
//int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
//				fd_set *exceptfds, struct timeval *timeout ) );
//int	socket		args( ( int domain, int type, int protocol ) );
#endif

/*
 * Global variables.
 */
FILE *					fpReserve;				/* Reserved file handle		*/
bool					merc_down;				/* Shutdown			*/
bool					wizlock;				/* Game is wizlocked		*/
bool					newlock;				/* Game is newlocked		*/
time_t					str_boot_time;
time_t					current_time;			/* time of this pulse */
bool					MOBtrigger = TRUE;		/* act() switch				 */
bool					OBJtrigger = TRUE;		/* act() switch				 */
MISC_DATA				misc;					/* opcje copyovera, czas ¶wiata itp */
DESCRIPTOR_DATA *		descriptor_list;		/* All open descriptors		*/
INFO_DESCRIPTOR_DATA *	info_descriptor_list;	/* lista deskryptorów serwisu informacyjnego */
INFO_DESCRIPTOR_DATA *	id_next;				/* wska¼nik na kolejny deskryptor serwisu informacyjny */
fd_set					fSet;
int						port;					/* port muda */
int						port2;					/* port serwisu informacyjnego */
int						control;				/* deskryptor nas³uchuj±cy muda */
int						control2;				/* deskryptor nas³uchuj±cy serwisu informacyjnego */
int						in_thread = 0;


char *
names_alias						( CHAR_DATA *ch, int type, int val );
void
game_loop_unix					( int control );
int
init_socket						( int port );
void
init_descriptor					( int control );
bool
read_from_descriptor			( DESCRIPTOR_DATA *d );
bool
write_to_descriptor				( DESCRIPTOR_DATA *d, char *txt, int length );
bool
write_to_descriptor_2			( int desc, char *txt, int length );
void
init_info_socket				( int control );
bool
process_info_command			( INFO_DESCRIPTOR_DATA *id );
bool
read_from_info_descriptor		( INFO_DESCRIPTOR_DATA *id );
bool
write_to_info_descriptor		( INFO_DESCRIPTOR_DATA * id, char * txt );
void
close_info_socket				( INFO_DESCRIPTOR_DATA *idclose );
bool
check_parse_name				( char *name );
bool
check_host_broken_link			( DESCRIPTOR_DATA *d );
bool
check_reconnect					( DESCRIPTOR_DATA *d, char *name, bool fConn );
bool
check_playing					( DESCRIPTOR_DATA *d, char *name );
bool
check_creating					( DESCRIPTOR_DATA *d, char *name );
void
nanny							( DESCRIPTOR_DATA *d, char *argument );
bool
process_output					( DESCRIPTOR_DATA *d, bool fPrompt );
void
read_from_buffer				( DESCRIPTOR_DATA *d );
void
stop_idling						( CHAR_DATA *ch );
void
bust_a_prompt					( CHAR_DATA *ch );
void menu_show						( CHAR_DATA *ch );
void op_act_trigger					( char *argument, CHAR_DATA *ch, bool room_enabled );
void recycle_sockets					( void );
bool check_all_alpha					( char *str );

/**
 * local functions
 */
void roll_stats args( ( CHAR_DATA *ch ) );

int main( int argc, char **argv )
{
	struct timeval now_time;
	bool fCopyOver = FALSE;
	//rellik: do debugowania, ustawiam debuguj na poziom 0 czyli tylko b³êdy, nie wiem gdzie to daæ, jestem ¿a³osny
	debuguj = 0;
	debugged_project = 0; //debugowany jest nie wiadomo co
	//rellik: do debugowania, ustawiam gcaller
	strcpy( gcaller, "comm.c => main" );
	/*
	 * Memory debugging if needed.
	 */
#if defined(MALLOC_DEBUG)
	malloc_debug( 2 );
#endif
	/*
	 * Init time.
	 */
	gettimeofday( &now_time, NULL );
	current_time = ( time_t ) now_time.tv_sec;
	str_boot_time = ( time_t ) now_time.tv_sec;

	/*
	 * Reserve one channel for our use.
	 */
	if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
	{
		perror( NULL_FILE );
		exit( 1 );
	}

	/*
	 * Get the port number.
	 */
	port = PORT_PLAYER;
	if ( argc > 1 )
	{
		if ( !is_number( argv[ 1 ] ) )
		{
			fprintf( stderr, "Usage: %s [port #]\n", argv[ 0 ] );
			exit( 1 );
		}
		else if ( ( port = atoi( argv[ 1 ] ) ) <= 1024 )
		{
			fprintf( stderr, "Port number must be above 1024.\n" );
			exit( 1 );
		}

		/* Are we recovering from a copyover? */
		if ( argv[ 2 ] && argv[ 2 ][ 0 ] )
		{
			fCopyOver = TRUE;
			control = atoi( argv[ 3 ] );
			control2 = atoi( argv[ 4 ] );
		}
		else
			fCopyOver = FALSE;

	}
	port2 = port + 1;

	if ( !fCopyOver )
	{
		control = init_socket( port );
		control2 = init_socket( port2 );
	}

	boot_db();
	init_signals();
	sprintf( log_buf, "Killer is ready to rock on port: %d.", port );
	log_string( log_buf );

	if ( fCopyOver )
		copyover_recover();

	game_loop_unix( control );
	close ( control );
	close ( control2 );

	/*
	 * That's all, folks.
	 */
	log_string( "Normalne zakonczenie pracy serwera." );
	exit( 0 );
	return 0;
}

void game_loop_unix( int control )
{
	fd_set rFd;
	DESCRIPTOR_DATA *d;
	INFO_DESCRIPTOR_DATA *id;
	static struct timeval null_time;
	struct timeval last_time;

	signal( SIGPIPE, SIG_IGN );
	gettimeofday( &last_time, NULL );
	current_time = ( time_t ) last_time.tv_sec;

	FD_ZERO( &fSet );

	FD_SET( control, &fSet );
	FD_SET( control2, &fSet );

  	for ( d = descriptor_list; d; d = d->next )
		FD_SET(d->descriptor, &fSet);

	/* Main loop */
	while ( !merc_down )
	{
		int syscall_result;
#if defined(MALLOC_DEBUG)
		if ( malloc_verify( ) != 1 )
			abort( );
#endif
		/*
		 * Poll all active descriptors.
		 */
		memcpy( &rFd, &fSet, sizeof( fd_set ) );

		do { syscall_result = select( FD_SETSIZE, &rFd, NULL, NULL, &null_time ); }
		while ((syscall_result == -1) && (errno = EINTR));

		if (syscall_result < 0)
		{
			perror( "Game_loop: select: poll" );
			exit( 1 );
		}


		/*
		 * New connection?
		 */
		if ( FD_ISSET( control, &rFd ) )
		{
			DEBUG_INFO( "init_descriptor" );
			init_descriptor( control );
			DEBUG_INFO( NULL );
		}

		/*
		 * New connection2?
		 */
		if ( FD_ISSET( control2, &rFd ) )
		{
			DEBUG_INFO( "init_info_socket" );
			init_info_socket( control2 );
			DEBUG_INFO( NULL );
		}

		/*
		 * Process input.
		 */
		DEBUG_INFO( "process_input" );
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			d->fcommand	= FALSE;

			if ( FD_ISSET( d->descriptor, &rFd ) )
			{
				if ( d->character != NULL )
					d->character->timer = 0;

				if ( !read_from_descriptor( d ) )
				{
					if ( d->character != NULL && d->connected == CON_PLAYING )
						save_char_obj( d->character, FALSE, FALSE );

					d->outtop	= 0;
					close_socket( d, 0 );
					continue;
				}
			}

			DEBUG_INFO( NULL );

			/* jak ktos wpadnie do menu z waitem*/
			if ( d->character && d->connected != CON_PLAYING && d->character->wait > 0 )
				d->character->wait = 0;

			if ( d->character && d->character->wait > 0 )
				continue;

			read_from_buffer( d );

			if ( d->incomm[ 0 ] != '\0' )
			{
				d->fcommand	= TRUE;
				stop_idling( d->character );

				/* OLC */
				if ( d->showstr_point )
					show_string( d, d->incomm );
				else
					if ( d->pString )
						string_add( d->character, d->incomm );
					else
						switch ( d->connected )
						{
							case CON_PLAYING:
								if ( !run_olc_editor( d ) )
									substitute_alias( d, d->incomm );
								break;
							default:
								DEBUG_INFO( "nanny" );
#ifdef NEW_NANNY
								new_nanny( d, d->incomm );
#else
								nanny( d, d->incomm );
#endif
								DEBUG_INFO( NULL );
								break;
						}

				d->incomm[ 0 ] = '\0';
			}
		}

		for ( id = info_descriptor_list; id != NULL; id = id_next )
		{
			id_next	= id->next;

			if ( FD_ISSET( id->descriptor, &rFd ) )
			{
				if ( !read_from_info_descriptor( id ) )
				{
					close_info_socket( id );
					continue;
				}
			}
		}

		/*
		 * Autonomous game motion.
		 */
		DEBUG_INFO( "update_handler" );
		update_handler( );
		DEBUG_INFO( NULL );

		/*
		 * Output.
		 */
		DEBUG_INFO( "output" );
		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( d->connected < 0 ) continue;

			if ( d->fcommand || d->outtop > 0 )
			{
				if ( !process_output( d, TRUE ) )
				{
					if ( d->character != NULL && d->connected == CON_PLAYING )
						save_char_obj( d->character, FALSE, FALSE );

					d->outtop	= 0;
					close_socket( d, 0 );
				}
			}
		}

		for ( id = info_descriptor_list; id != NULL; id = id_next )
		{
			id_next = id->next;

			if ( process_info_command( id ) )
					close_info_socket( id );
		}
		DEBUG_INFO( NULL );

		/*
		 * Synchronize to a clock.
		 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
		 * Careful here of signed versus unsigned arithmetic.
		 */
		DEBUG_INFO( "time_stuff+stall" );
		{
			struct timeval now_time;
			long secDelta;
			long usecDelta;

			gettimeofday( &now_time, NULL );
			usecDelta	= ( ( int ) last_time.tv_usec ) - ( ( int ) now_time.tv_usec )
			            + 1000000 / PULSE_PER_SECOND;
			secDelta	= ( ( int ) last_time.tv_sec ) - ( ( int ) now_time.tv_sec );
			while ( usecDelta < 0 )
			{
				usecDelta += 1000000;
				secDelta -= 1;
			}

			while ( usecDelta >= 1000000 )
			{
				usecDelta -= 1000000;
				secDelta += 1;
			}

			if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
			{
			  struct timeval stall_time;

			  stall_time.tv_usec = usecDelta;
			  stall_time.tv_sec = secDelta;

			  syscall_result = select( 0, NULL, NULL, NULL, &stall_time );

                          if ((syscall_result < 0) && (errno != EINTR))
                            {
                              perror( "Game_loop: select: stall" );
                              exit( 1 );
                            }
			}

		}

		DEBUG_INFO( NULL );
		gettimeofday( &last_time, NULL );
		current_time = ( time_t ) last_time.tv_sec;

		recycle_sockets();
	}

	return ;
}

int init_socket( int port )
{
	static struct sockaddr_in sa_zero;
	struct sockaddr_in sa;
	int x = 1;
	int fd;

	if ( ( fd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		perror( "Init_socket: socket" );
		exit( 1 );
	}

	if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR,
	                 ( char * ) & x, sizeof( x ) ) < 0 )
	{
		perror( "Init_socket: SO_REUSEADDR" );
		close( fd );
		exit( 1 );
	}

#if defined(SO_DONTLINGER) && !defined(SYSV)

	{
		struct	linger	ld;

		ld.l_onoff = 1;
		ld.l_linger = 1000;

		if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
		                 ( char * ) & ld, sizeof( ld ) ) < 0 )
		{
			perror( "Init_socket: SO_DONTLINGER" );
			close( fd );
			exit( 1 );
		}
	}
#endif

	sa	= sa_zero;
	sa.sin_family = AF_INET;
	sa.sin_port	= htons( port );

	if ( bind( fd, ( struct sockaddr * ) & sa, sizeof( sa ) ) < 0 )
	{
		perror( "Init socket: bind" );
		close( fd );
		exit( 1 );
	}


	if ( listen( fd, 3 ) < 0 )
	{
		perror( "Init socket: listen" );
		close( fd );
		exit( 1 );
	}
	return fd;
}

#ifdef ASYNCH_DNS
typedef struct lookup_data             LOOKUP_DATA;

struct lookup_data
{
  DESCRIPTOR_DATA *		desc;
  struct sockaddr_in	addr;
};

/* does the lookup, changes the hostname, and dies */

void *lookup_address( void *arg )
{
	LOOKUP_DATA * lData = ( LOOKUP_DATA * ) arg;
	struct hostent *from = 0;
	int rc = 0;

	if ( lData->desc )
	{
#ifdef CYGWIN
		pthread_mutex_lock( &lookup_mutex );
		from = gethostbyaddr( (char *) &lData->addr.sin_addr, sizeof( lData->addr.sin_addr ), AF_INET );

		if ( from && from->h_name )
		{
			free_string( lData->desc->host );
			lData->desc->host = str_dup( from->h_name );
		}
		pthread_mutex_unlock( &lookup_mutex );

#else
		from = gethostbyaddr( (char *) &lData->addr.sin_addr, sizeof( lData->addr.sin_addr ), AF_INET );

		if ( from && from->h_name )
		{
			free_string( lData->desc->host );
			lData->desc->host = str_dup( from->h_name );
		}

#endif

		lData->desc->lookup_state = DNS_DONE;
	}

	DISPOSE( lData );

	--in_thread;

	pthread_exit( &rc );

#ifdef CYGWIN
	return NULL;
#endif
}
#endif

void init_descriptor( int control )
{
	char buf[ MAX_STRING_LENGTH ];
	DESCRIPTOR_DATA *dnew;
	struct sockaddr_in sock;
#ifdef ASYNCH_DNS
	pthread_t thread_lookup;
	pthread_attr_t attr;
	LOOKUP_DATA * lData;
#else
	struct hostent *from = 0;
#endif
	int argp = 1;
	int desc;
	unsigned int size;


#ifdef ASYNCH_DNS
	/* initialize threads */
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
#endif

	size = sizeof( sock );

	getsockname( control, ( struct sockaddr * ) & sock, &size );

	if ( ( desc = accept( control, ( struct sockaddr * ) & sock, &size ) ) < 0 )
	{
		perror( "New_descriptor: accept" );
		return ;
	}

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

	if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
	{
		perror( "New_descriptor: fcntl: FNDELAY" );
		return ;
	}

	ioctl( desc, FIONBIO, &argp );

	dnew = new_descriptor();
	dnew->descriptor = desc;

	size = sizeof( sock );

	if ( getpeername( desc, ( struct sockaddr * ) & sock, &size ) < 0 )
	{
		perror( "New_descriptor: getpeername" );
		dnew->host = str_dup( "(unknown)" );
		dnew->ip_addr = str_dup( "(unknown)" );
	}
	else
	{
		/*
		 * Would be nice to use inet_ntoa here but it takes a struct arg,
		 * which ain't very compatible between gcc and system libraries.
		 */
		int addr;
		int in_port;

		addr = ntohl( sock.sin_addr.s_addr );

		sprintf( buf, "%d.%d.%d.%d",
		         ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
		         ( addr >> 8 ) & 0xFF, ( addr	) & 0xFF
		       );

		in_port = 0;
		in_port |= ( sock.sin_port & 0xFF ) << 8;
		in_port |= ( sock.sin_port >> 8 ) & 0xFF;

		dnew->port = in_port;

		sprintf( log_buf, "Sock.sinaddr:  %s:%d", buf, in_port );
		log_string( log_buf );

#ifdef OFFLINE
		if ( str_cmp( buf, "127.0.0.1" ) )
		{
			log_string( "W builderze mozna laczyc sie tylko przez 127.0.0.1" );
			close( desc );
			free_descriptor( dnew );
			return ;
		}
#endif

		dnew->ip_addr = str_dup( buf );
		dnew->host    = str_dup( buf );

#ifdef ASYNCH_DNS
		if ( str_cmp( buf, "127.0.0.1" ) )
		{
			CREATE( lData, LOOKUP_DATA, 1 );

			lData->desc   =  dnew;
			memcpy( &lData->addr, &sock, sizeof( sock ) );

			++in_thread;
			pthread_create(&thread_lookup, &attr, lookup_address, (void*) lData);
		} else
			dnew->lookup_state = DNS_DONE;
#else
		if ( str_cmp( buf, "127.0.0.1" ) )
		{
			from = gethostbyaddr( (char *) &sock.sin_addr, sizeof( sock.sin_addr ), AF_INET);

			if ( from && from->h_name )
			{
				free_string( dnew->host );
				dnew->host = str_dup( from->h_name );
			}
			else
			{
			    sprintf( log_buf, "Cannot resolve revDNS for %s.", buf );
		        log_string( log_buf );
			}
			dnew->lookup_state = DNS_DONE;
		}
#endif
	}

	/*
	 * Swiftest: I added the following to ban sites.  I don't
	 * endorse banning of sites, but Copper has few descriptors now
	 * and some people from certain sites keep abusing access by
	 * using automated 'autodialers' and leaving connections hanging.
	 *
	 * Furey: added suffix check by request of Nickel of HiddenWorlds.
	 */
	if ( check_ban( dnew->host, BAN_ALL ) )
	{
		close( desc );
		free_descriptor( dnew );
		return ;
	}

	/* dodajemy do listy deskryptorów */
	dnew->next	= descriptor_list;
	descriptor_list	= dnew;

	/* dodajemy do zbioru deskryptorów do odczytywania przez select */
	FD_SET( dnew->descriptor, &fSet );

	/*
	 * Send the greeting.
	 */

#ifdef MCCP
	/* mccp: tell the client we support compression */
	write_to_buffer( dnew, eor_on_str, 0 );
	write_to_buffer( dnew, compress2_on_str, 0 );
#endif

	{
		extern char * help_greeting;

		if ( help_greeting[ 0 ] == '.' )
			write_to_buffer( dnew, help_greeting + 1, 0 );
		else
			write_to_buffer( dnew, help_greeting , 0 );
	}

	return ;
}

void close_socket( DESCRIPTOR_DATA *dclose, int options )
{
	CHAR_DATA * ch;
	char buf[ MAX_STRING_LENGTH ];

	buf[ 0 ] = '\0';

	FD_CLR( dclose->descriptor, &fSet );

	if ( dclose->outtop > 0 )
		process_output( dclose, FALSE );

	if ( dclose->snoop_by != NULL )
	{
		write_to_buffer( dclose->snoop_by, "Twoja ofiara opusci³a grê.\n\r", 0 );
	}

	{
		DESCRIPTOR_DATA *d;

		for ( d = descriptor_list; d != NULL; d = d->next )
		{
			if ( d->connected < 0 ) continue;

			if ( d->snoop_by == dclose )
				d->snoop_by = NULL;
		}
	}

	if ( dclose->remote_char != NULL )
	{
		free_char( dclose->remote_char );
		dclose->remote_char = NULL;
	}

	if ( dclose->new_char_data != NULL )
	{
		free_new_char_data( dclose->new_char_data );
		dclose->new_char_data = NULL;
	}

	if ( ( ch = dclose->character ) != NULL )
	{
		sprintf( log_buf, "[%d] Closing link to %s.", ch->in_room ? ch->in_room->vnum : 0, ch->name );
		log_string( log_buf );

		/* cut down on wiznet spam when rebooting */
		if ( !IS_NULLSTR( ch->name ) && is_allowed( ch->name ) != NAME_DISALLOWED && dclose->connected == CON_PLAYING && !merc_down )
		{
			act( "$n zastyga z dziwnym grymasem na twarzy.", ch, NULL, NULL, TO_ROOM );
			wiznet( "Net death has claimed $N.", ch, NULL, WIZ_LINKS, 0, 0 );
			free_string( ch->host );
			ch->host = str_dup( dclose->host );
			ch->desc = NULL;
		}
		else
		{
			free_char( dclose->original ? dclose->original:dclose->character );
		}
	}
	dclose->character = NULL;
	dclose->connected = CON_CLOSED;
	return ;
}

void recycle_sockets( void )
{
	DESCRIPTOR_DATA * d, *dnext;

	for( d = descriptor_list; d; d = dnext )
	{
		dnext = d->next;

		if ( d->connected != CON_CLOSED || d->lookup_state == DNS_LOOKUP )
			continue;

		if ( d == descriptor_list )
		{
			descriptor_list = descriptor_list->next;
		}
		else
		{
			DESCRIPTOR_DATA *prev;

			for ( prev = descriptor_list; prev && prev->next != d; prev = prev->next )
				;
			if ( prev != NULL )
				prev->next = d->next;
			else
				bug( "Close_socket: descriptor not found.", 0 );
		}


		#ifdef MCCP
			compressEnd( d );
		#endif

		close( d->descriptor );
		free_descriptor( d );
	}
}

bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
	unsigned int iStart;

	/* Hold horses if pending command already. */
	if ( d->incomm[ 0 ] != '\0' )
		return TRUE;

	/* Check for overflow. */
	iStart = strlen( d->inbuf );
	if ( iStart >= sizeof( d->inbuf ) - 10 )
	{
		sprintf( log_buf, "%s input overflow!", d->host );
		log_string( log_buf );
		write_to_descriptor( d,
		                     "\n\r*** Eee wyluzuj !!! ***\n\r", 0 );
		return FALSE;
	}

	/* Snarf input. */

	for ( ; ; )
	{
		int nRead;

		nRead = read( d->descriptor, d->inbuf + iStart,
		              sizeof( d->inbuf ) - 10 - iStart );
		if ( nRead > 0 )
		{
			iStart += nRead;
			if ( d->inbuf[ iStart - 1 ] == '\n' || d->inbuf[ iStart - 1 ] == '\r' )
				break;
		}
		else if ( nRead == 0 )
		{
			log_string( "EOF encountered on read (lost link)." );
			return FALSE;
		}
		else if ( errno == EWOULDBLOCK )
			break;
		else
		{
			perror( "Read_from_descriptor" );
			return FALSE;
		}
	}

	d->inbuf[ iStart ] = '\0';
	return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
	int i, j, k;
#ifdef MCCP
	int iac = 0;
#endif

	/*
	 * Hold horses if pending command already.
	 */
	if ( d->incomm[ 0 ] != '\0' )
		return ;

	/*
	 * Look for at least one new line.
	 */
	for ( i = 0; d->inbuf[ i ] != '\n' && d->inbuf[ i ] != '\r'; i++ )
	{
		if ( d->inbuf[ i ] == '\0' )
			return ;
	}

	/*
	 * Canonical input processing.
	 */
	for ( i = 0, k = 0; d->inbuf[ i ] != '\n' && d->inbuf[ i ] != '\r'; i++ )
	{
		if ( k >= MAX_INPUT_LENGTH - 20 )
		{
			write_to_descriptor( d, "Zbyt d³uga komenda.\n\r", 0 );

			/* skip the rest of the line */
			for ( ; d->inbuf[ i ] != '\0'; i++ )
			{
				if ( d->inbuf[ i ] == '\n' || d->inbuf[ i ] == '\r' )
					break;
			}
			d->inbuf[ i ] = '\n';
			d->inbuf[ i + 1 ] = '\0';
			break;
		}

		d->inbuf[ i ] = ISOIZE( d->inbuf[ i ] );

#ifdef MCCP
		if ( d->inbuf[ i ] == ( signed char ) IAC )
			iac = 1;
		else if ( iac == 1 && d->inbuf[ i ] == ( signed char ) DO )
			iac = 2;
		else if ( iac == 1 && d->inbuf[ i ] == ( signed char ) DONT )
			iac = 3;
		else if ( iac == 2 || iac == 3 )
		{
			if ( d->inbuf[ i ] == ( signed char ) TELOPT_COMPRESS )
			{
				if ( iac == 2 )
					compressStart( d, TELOPT_COMPRESS );
				else if ( iac == 3 )
					compressEnd( d );
			}
			else if ( d->inbuf[ i ] == ( signed char ) TELOPT_COMPRESS2 )
			{
				if ( iac == 2 )
					compressStart( d, TELOPT_COMPRESS2 );
				else if ( iac == 3 )
				{
					write_to_buffer( d, compress_on_str, 0 );
					compressEnd( d );
				}
			}
			iac = 0;
		}
		else
#endif
			if ( d->inbuf[ i ] == '\b' && k > 0 )
				--k;
			else if ( isascii( NOPOL( d->inbuf[ i ] ) ) && isprint( NOPOL( d->inbuf[ i ] ) ) )
				d->incomm[ k++ ] = d->inbuf[ i ];
	}

	/*
	 * Finish off the line.
	 */
	if ( k == 0 )
		d->incomm[ k++ ] = ' ';
	d->incomm[ k ] = '\0';

	/*
	 * Deal with bozos with #repeat 1000 ...
	 */

	if ( k > 1 || d->incomm[ 0 ] == '!' )
	{
		if ( d->incomm[ 0 ] != '!' && strcmp( d->incomm, d->inlast ) )
		{
			d->repeat = 0;
		}
		else
		{
			if ( ++d->repeat >= 25 && d->character
			     && d->connected == CON_PLAYING )
			{
				sprintf( log_buf, "%s input spamming!", d->host );
				log_string( log_buf );
				wiznet( "Spam spam spam $N spam spam spam spam spam!",
				        d->character, NULL, WIZ_SPAM, 0, get_trust( d->character ) );
				if ( d->incomm[ 0 ] == '!' )
					wiznet( d->inlast, d->character, NULL, WIZ_SPAM, 0,
					        get_trust( d->character ) );
				else
					wiznet( d->incomm, d->character, NULL, WIZ_SPAM, 0,
					        get_trust( d->character ) );

				d->repeat = 0;
				/*
						write_to_descriptor( d->descriptor,
							"\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
						strcpy( d->incomm, "quit" );
				*/
			}
		}
	}


	/*
	 * Do '!' substitution.
	 */
	if ( d->incomm[ 0 ] == '!' )
		strcpy( d->incomm, d->inlast );
	else
		strcpy( d->inlast, d->incomm );

	/*
	 * Shift the input buffer.
	 */
	while ( d->inbuf[ i ] == '\n' || d->inbuf[ i ] == '\r' )
		i++;
	for ( j = 0; ( d->inbuf[ j ] = d->inbuf[ i + j ] ) != '\0'; j++ )
		;
	return ;
}



/*
 * Low level output function.
 */
bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
	extern bool merc_down;
	/*
	 * Bust a prompt.
	 */
	if ( !merc_down )
	{
		if ( d->showstr_point )
			write_to_buffer( d, "[Naci¶nij Enter aby kontynuowaæ]\n\r", 0 );
		else if ( fPrompt && d->pString && ( d->connected == CON_PLAYING || d->connected == CON_MENU ) )
			write_to_buffer( d, "> ", 2 );
		else if ( fPrompt && d->connected == CON_PLAYING )
		{
			CHAR_DATA * ch;

			ch = d->character;

			ch = d->original ? d->original : d->character;

			if ( ch && !get_spirit( ch ) )
			{
				if ( !IS_SET( ch->comm, COMM_COMPACT ) )
					write_to_buffer( d, "\n\r", 2 );

				if ( IS_SET( ch->comm, COMM_PROMPT ) )
					bust_a_prompt( d->character );

				if ( IS_SET( ch->comm, COMM_TELNET_GA ) )
					write_to_buffer( d, go_ahead_str, 0 );
			}
			else
				write_to_buffer( d, "\n\r", 2 );
		}
	}

	/*
	 * Short-circuit if nothing to write.
	 */
	if ( d->outtop == 0 )
		return TRUE;

	/*
	 * Snoop-o-rama.
	 */
	if ( d->snoop_by != NULL )
	{
		if ( d->character != NULL )
			write_to_buffer( d->snoop_by, d->character->name, 0 );
		write_to_buffer( d->snoop_by, "> ", 2 );
		write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
	}

	/*
	 * OS-dependent output.
	 */
	if ( !write_to_descriptor( d, d->outbuf, d->outtop ) )
	{
		d->outtop = 0;
		return FALSE;
	}
	else
	{
		d->outtop = 0;
		return TRUE;
	}
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt( CHAR_DATA *ch )
{
	char buf[ MAX_STRING_LENGTH ];
	char buf2[ MAX_STRING_LENGTH ];
	char exit[ MAX_STRING_LENGTH ];
	const char *str;
	const char *i;
	char *point;
	char *pbuff;
	char buffer[ MAX_STRING_LENGTH * 2 ];
	char doors[ MAX_INPUT_LENGTH ];
	EXIT_DATA *pexit;
	bool found, check;
	const char *dir_name[] = {"N", "E", "S", "W", "U", "D" };
	int door, percent_exp, j;
	/*do kolorkow na cyferkach*/
	sh_int col_hp, col_mv;
	char tmp[] = "{g%d{c";
	bool dirs = FALSE;

    if ( IS_NPC( ch ) )
    {
        return;
    }

	col_hp = get_max_hp( ch ) != 0 ? 100 * ch->hit / get_max_hp( ch ) : 0;
	col_mv = ch->max_move != 0 ? 100 * ch->move / ch->max_move : 0;

	//combat prompt
	if ( ch->fighting && ch->fighting->in_room )
	{
		CHAR_DATA * tank, *enemy;

		//col_hp=100*ch->hit/ch->max_hit;
		//col_mv=100*ch->move/ch->max_move;

		buf[ 0 ] = '\0';

		strcat( buf, "{c<" );

		if ( col_hp >= 70 )
			tmp[ 1 ] = 'g';
		else if ( col_hp < 70 && col_hp >= 40 )
			tmp[ 1 ] = 'f';
		else if ( col_hp < 40 )
			tmp[ 1 ] = 'r';

		sprintf( buf2, tmp, ch->hit );

		strcat( buf, buf2 );
		strcat( buf, "hp " );

		if ( col_mv >= 75 )
			tmp[ 1 ] = 'g';
		else if ( col_mv < 75 && col_mv >= 25 )
			tmp[ 1 ] = 'f';
		else if ( col_mv < 25 )
			tmp[ 1 ] = 'r';

		// Raszer: zmiana mv dla nieumarlych, bylo sprintf( buf2, tmp, is_undead( ch ) ? ch->max_move : ch->move );
		sprintf( buf2, tmp, ch->move );
        strcat( buf, buf2 );

		strcat( buf, "mv> " );

		tank = ch->fighting->fighting;

		if ( !tank )
			;
		else
        {
            strcat( buf, " [{wTank " );

            if ( can_see( ch, tank ) )
            {
                sprintf( buf2, "%-1.15s:", PERS( tank, ch ) );
                strcat( buf, buf2 );
                strcat( buf, " " );
                strcat( buf, names_alias( tank, 1, 0 ) );
                if ( EXT_IS_SET( ch->act, PLR_SHOWPOS ) )
                {
                    strcat( buf, "{w, " );
                    if ( EXT_IS_SET( ch->act, PLR_SHOWPOSS ) )
                    {
                        switch( tank->position )
                        {
                            case POS_FIGHTING:
                            case POS_STANDING:
                                strcat( buf, "{gS" ); //stoi walczy
                                break;
                            case POS_SITTING:
                            case POS_RESTING:
                                strcat( buf, "{yNK" ); //lezy
                                break;
                            case POS_STUNNED:
                                strcat( buf, "{sL" ); //ogluszony
                                break;
                            case POS_INCAP:
                            case POS_MORTAL:
                                strcat( buf, "{rU" ); //umiera
                                break;
                            default:
                                strcat( buf, " " ); //nieokreslona
                                break;
                        }

                    }
                    else
                    {
                        switch( tank->position )
                        {
                            case POS_FIGHTING:
                            case POS_STANDING:
                                strcat( buf, "{gstoi" ); //stoi walczy
                                break;
                            case POS_SITTING:
                            case POS_RESTING:
                                strcat( buf, "{yna kolanach" ); //lezy
                                break;
                            case POS_STUNNED:
                                strcat( buf, "{sle¿y" ); //ogluszony
                                break;
                            case POS_INCAP:
                            case POS_MORTAL:
                                strcat( buf, "{rumiera" ); //umiera
                                break;
                            default:
                                strcat( buf, " " ); //nieokreslona
                                break;
                        }
                    }
                }
                strcat( buf, "{c]{x " );
            }
            else
            {
                strcat( buf, "Kto¶:{c]{x " );
            }
        }

		enemy = ch->fighting;

		if ( enemy && can_see( ch, enemy ) )
		{
			strcat( buf, "{c[{w" );
			sprintf( buf2, "%-1.15s", PERS( enemy, ch ) );
			strcat( buf, buf2 );
			strcat( buf, ": " );
			strcat( buf, names_alias( enemy, 1, 0 ) );
			if ( EXT_IS_SET( ch->act, PLR_SHOWPOS ) )
			{
			strcat( buf, "{w, " );
			if ( EXT_IS_SET( ch->act, PLR_SHOWPOSS ) )
				{
                     switch( enemy->position )
				{
					case 7:
					case 8:
						strcat( buf, "{gS" ); //stoi walczy
						break;
					case 6:
					case 5:
						strcat( buf, "{yNK" ); //lezy
						break;
					case 3:
						strcat( buf, "{sL" ); //ogluszony
						break;
					case 2:
					case 1:
						strcat( buf, "{rU" ); //umiera
						break;
					default:
						strcat( buf, " " ); //nieokreslona
						break;
				}
                }
                else
                {
                switch( enemy->position )
				{
					case 7:
					case 8:
						strcat( buf, "{gstoi" ); //stoi walczy
						break;
					case 6:
					case 5:
						strcat( buf, "{yna kolanach" ); //lezy
						break;
					case 3:
						strcat( buf, "{sle¿y" ); //ogluszony
						break;
					case 2:
					case 1:
						strcat( buf, "{rumiera" ); //umiera
						break;
					default:
						strcat( buf, " " ); //nieokreslona
						break;
				}
				}
                }

			    strcat( buf, "{c]{x " );
		}

		send_to_char( buf, ch );
		send_to_char( "\n\r", ch );

		//Combat group prompt
		if ( EXT_IS_SET( ch->act, PLR_COMBAT_PROMPT ) )
		{
			CHAR_DATA * vch, *group[ 5 ], *tch, *group_wounded[ 5 ];
			int counter = 0, x, wounded = 0, shown = 0;
			bool found = FALSE, stop;

			//najpierw ci co dostaja becki
			for ( vch = ch->in_room->people; vch ; vch = vch->next_in_room )
				if ( vch->fighting && vch != ch && is_same_group( ch, vch ) && vch->fighting->fighting == vch )
				{
					if ( counter >= 5 )
						break;

					group[ counter++ ] = vch;
				}


			//posortowanie group
			for ( ; ; )
			{
				stop = TRUE;
				for ( x = 0; x < counter - 1; x++ )
					if ( 100 * group[ x ] ->hit / get_max_hp( group[ x ] ) > 100 * group[ x + 1 ] ->hit / get_max_hp( group[ x + 1 ] ) )
					{
						stop = FALSE;
						tch = group[ x + 1 ];
						group[ x + 1 ] = group[ x ];
						group[ x ] = tch;
					}
				if ( stop )
					break;
			}

			//a potem ci co maja malo hp i nie sa bici
			for ( vch = ch->in_room->people; vch ; vch = vch->next_in_room )
				if ( vch != ch && is_same_group( ch, vch ) && vch->hit < get_max_hp( vch ) )
				{
					if ( wounded >= 5 )
                    {
						break;
                    }
					stop = FALSE;

					//jesli juz sa w group[] to wychodzimy
					for ( j = 0; j < counter; j++ )
						if ( group[ j ] == vch )
						{
							stop = TRUE;
							break;
						}
					if ( stop )
                    {
						continue;
                    }
					group_wounded[ wounded++ ] = vch;
				}

			//posortowanie wounded
			for ( ; ; )
			{
				stop = TRUE;
				for ( x = 0; x < wounded - 1; x++ )
					if ( 100 * group_wounded[ x ] ->hit / get_max_hp( group_wounded[ x ] ) > 100 * group_wounded[ x + 1 ] ->hit / get_max_hp( group_wounded[ x + 1 ] ) )
					{
						stop = FALSE;
						tch = group_wounded[ x + 1 ];
						group_wounded[ x + 1 ] = group_wounded[ x ];
						group_wounded[ x ] = tch;
					}
				if ( stop )
                {
					break;
                }
			}

			//wypisywanie pod promptem groupkowiczow
			buf[ 0 ] = '\0';

			//wypisujemy walczacych
            for ( j = 0; j < counter; j++ )
            {
                if ( wounded > 0 && 100 * group[ j ] ->hit / get_max_hp( group[ j ] ) > 50 * group_wounded[ 0 ] ->hit / get_max_hp( group_wounded[ 0 ] ) )
                    break;

                if ( shown >= 4 )
                    break;

                if ( can_see( ch, group[ j ] ) )
                {
                    if ( !found )
                    {
                        found = TRUE;
                        strcat( buf, "[" );
                    }

                    sprintf( buf2, "{w<%-1.3s: (", group[ j ] ->name );
                    strcat( buf, buf2 );
                    strcat( buf, names_alias( group[ j ], 4, 0 ) );
                    strcat( buf, "){w>" );
                    if ( EXT_IS_SET( ch->act, PLR_SHOWPOS ) )
                    {

                        switch( group[ j ]->position )
                        {
                            case POS_FIGHTING:
                            case POS_STANDING:
                                strcat( buf, "{g:s " ); //stoi walczy
                                break;
                            case POS_SITTING:
                            case POS_RESTING:
                                strcat( buf, "{yn:k " ); //lezy
                                break;
                            case POS_STUNNED:
                                strcat( buf, "{s:l " ); //ogluszony
                                break;
                            case POS_INCAP:
                            case POS_MORTAL:
                                strcat( buf, "{r:u " ); //umiera
                                break;
                            default:
                                strcat( buf, " " ); //nieokreslona
                                break;
                        }
                    }
                    shown++;
                }
            }

			//wypisujemy tych ktorych nikt nie bije
            for ( j = 0; j < wounded; j++ )
            {
                if ( shown >= 4 )
                    break;

                if ( can_see( ch, group_wounded[ j ] ) )
                {
                    if ( !found )
                    {
                        found = TRUE;
                        strcat( buf, "[" );
                    }

                    sprintf( buf2, "{w%-1.3s: (", group_wounded[ j ] ->name );
                    strcat( buf, buf2 );
                    strcat( buf, names_alias( group_wounded[ j ], 4, 0 ) );
                    strcat( buf, ")" );
                    if ( EXT_IS_SET( ch->act, PLR_SHOWPOS ) )
                    {

                        switch( group_wounded[ j ]->position )
                        {
                            case POS_FIGHTING:
                            case POS_STANDING:
                                strcat( buf, "{g:s " ); //stoi walczy
                                break;
                            case POS_SITTING:
                            case POS_RESTING:
                                strcat( buf, "{yn:k " ); //lezy
                                break;
                            case POS_STUNNED:
                                strcat( buf, "{s:l " ); //ogluszony
                                break;
                            case POS_INCAP:
                            case POS_MORTAL:
                                strcat( buf, "{r:u " ); //umiera
                                break;
                            default:
                                strcat( buf, " " ); //nieokreslona
                                break;
                        }
                    }
                    shown++;
                }
            }

			if ( found )
			{
				strcat( buf, "{w]{x\n\r " );
				send_to_char( buf, ch );
			}
		}
		return ;
	}


	point = buf;
	str = ch->prompt;

	if ( str == NULL || str[ 0 ] == '\0' )
	{
		sprintf( buf, "{c<%dhp %dmv>{x %s", ch->hit, ch->move, ch->prefix );
		//bylo ch->hit, is_undead( ch ) ? ch->max_move : ch->move, ch->prefix );
		send_to_char( buf, ch );
		return ;
	}

	if ( IS_SET( ch->comm, COMM_AFK ) )
	{
		send_to_char( "{f<AFK>{x ", ch );
		return ;
	}

	while ( *str != '\0' )
	{
		if ( *str != '%' )
		{
			*point++ = *str++;
			continue;
		}
		++str;
		switch ( *str )
		{
			default :
				i = " "; break;
			case 'e':
				found = FALSE;
				doors[ 0 ] = '\0';
				if ( ch->position != POS_SLEEPING || IS_IMMORTAL( ch ) )
				{
					if ( IS_NPC( ch ) || IS_IMMORTAL( ch ) || !IS_AFFECTED( ch, AFF_BLIND ) )
					{
						if ( !room_is_dark( ch, ch->in_room ) || IS_AFFECTED( ch, AFF_DARK_VISION ) || IS_IMMORTAL( ch ) )
						{
							for ( door = 0; door < 6; door++ )
							{
								if ( ( pexit = ch->in_room->exit[ door ] ) == NULL )
									continue;
								if ( pexit->u1.to_room == NULL )
									continue;
								if ( !can_see_room( ch, pexit->u1.to_room ) )
									continue;

								if ( IS_SET( pexit->exit_info, EX_SECRET ) && !IS_IMMORTAL( ch ) )
									continue;

								if ( IS_SET( pexit->exit_info, EX_HIDDEN ) && !IS_AFFECTED( ch, AFF_DETECT_HIDDEN ) )
									continue;

								if ( !found )
									strcat( doors, "[ " );

								if ( pexit->vName && pexit->vName[ 0 ] != '\0' )
									sprintf( exit, "%s", pexit->vName );
								else
								{
									if ( IS_AFFECTED( ch, AFF_MAZE ) )
									{
										sprintf( exit, "%s", dir_name[ number_range( 0, 5 ) ] );
									}
									else
									{
										sprintf( exit, "%s", dir_name[ door ] );
									}
								}

								if ( IS_SET( pexit->exit_info, EX_HIDDEN ) || IS_SET( pexit->exit_info, EX_SECRET ) )
								{
									strcat( doors, "-" );
									strcat( doors, exit );
									strcat( doors, "-" );
								}
								else if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
								{
									strcat( doors, "(" );
									strcat( doors, exit );
									strcat( doors, ")" );
								}
								else
								{
									strcat( doors, exit );
								}
								strcat( doors, " " );
								found = TRUE;
							}
						}
					}
				}

				if ( !found )
				{
					strcat( doors, "[ brak ]" );
				}
				else
				{
					strcat( doors, "]" );
				}

				dirs = TRUE;
				sprintf( buf2, "%s", doors );
				i = buf2;
				break;

			case 'c' :
				sprintf( buf2, "%s", "\n\r" );
				i = buf2; break;

			case 'h' :
				if ( col_hp > 70 )
					tmp[ 1 ] = 'g';
				else if ( col_hp <= 70 && col_hp > 40 )
					tmp[ 1 ] = 'f';
				else if ( col_hp <= 40 )
					tmp[ 1 ] = 'r';

				sprintf( buf2, tmp, ch->hit );
				i = buf2; break;

			case 'H' :
				sprintf( buf2, "%d", get_max_hp( ch ) );
				i = buf2; break;

			case 'v' :

				if ( col_mv > 75 )
					tmp[ 1 ] = 'g';
				else if ( col_mv <= 75 && col_mv > 25 )
					tmp[ 1 ] = 'f';
				else if ( col_mv <= 25 )
					tmp[ 1 ] = 'r';
					sprintf( buf2, tmp, ch->move );

				// bylo sprintf( buf2, tmp, is_undead( ch ) ? ch->max_move : ch->move );
				i = buf2; break;

			case 'V' :
				sprintf( buf2, "%d", ch->max_move );
				i = buf2;
                break;
            case 'x' :
                if ( IS_IMMORTAL( ch ) )
                {
                    break;
                }
                percent_exp = 10 - get_percent_exp( ch );
                sprintf( buf2, "{G" );
                for ( j = 0; j < 10; j++ )
                {
                    strcat( buf2, ( j < percent_exp )? "*" : "{c." );
                }
                i = buf2;
                break;
            case 'X' :
                if
                    (
                     IS_SET( ch->pcdata->wiz_conf, W5 ) ||
                     IS_SET( ch->pcdata->wiz_conf, W6 )
                    )
                {
                    if ( ch->level != LEVEL_HERO)
                    {
                        if ( ch->level > LEVEL_HERO)
                        {
                            sprintf( buf2, "0" );
                            i = buf2;
                            break;
                        }
                        sprintf
                            (
                             buf2, "{R%ld{c/{G%ld{c",
                             UMAX( 0, exp_per_level( ch, ch->level ) - ch->exp ),
                             ch->exp
                            );
                        i = buf2;
                    }
                }
                break;
			case 's' :
				if ( ch->position == POS_RESTING || ch->position == POS_SITTING || ch->position == POS_STANDING || IS_IMMORTAL( ch ) )
				{
					sprintf( buf2, "%ldc %lds %ldg %ldm", ch->copper, ch->silver, ch->gold, ch->mithril );
					i = buf2;
				}
				else i = "-";
				break;
			case 'S' :
				if ( ch->position == POS_RESTING || ch->position == POS_SITTING || ch->position == POS_STANDING || IS_IMMORTAL( ch ) )
				{
					sprintf
                        (
                         buf2,
                         "%ldc %lds %ldg %ldm",
                         money_copper_from_copper ( ch->bank ),
                         money_silver_from_copper ( ch->bank ),
                         money_gold_from_copper   ( ch->bank ),
                         money_mithril_from_copper( ch->bank )
                        );
					i = buf2;
				}
				else i = "-";
				break;
			case 'b' :
				if ( ch->position == POS_RESTING || ch->position == POS_SITTING || ch->position == POS_STANDING || IS_IMMORTAL( ch ) )
				{
					sprintf( buf2, "%ld", ch->bank );
					i = buf2;
				}
				else i = "-";
				break;
			case 'r' :
				if ( ch->in_room != NULL )
					sprintf( buf2, "%s",
					         ( ( !IS_NPC( ch ) && EXT_IS_SET( ch->act, PLR_HOLYLIGHT ) ) ||
					           ( !IS_AFFECTED( ch, AFF_BLIND ) && !room_is_dark( ch, ch->in_room ) ) )
					         ? ch->in_room->name : "ciemno¶æ" );
				else
					sprintf( buf2, " " );
				i = buf2; break;
			case 'R' :
				if ( IS_IMMORTAL( ch ) && ch->in_room != NULL )
					sprintf( buf2, "%d", ch->in_room->vnum );
				else
					sprintf( buf2, " " );
				i = buf2; break;
			case 'z' :
				if ( IS_IMMORTAL( ch ) && ch->in_room != NULL )
					sprintf( buf2, "%s", ch->in_room->area->name );
				else
					sprintf( buf2, " " );
				i = buf2; break;
			case '%' :
				sprintf( buf2, "%%" );
				i = buf2; break;
			case 'o' :
				sprintf( buf2, "%s", olc_ed_name( ch ) );
				i = buf2; break;
			case 'O' :
				sprintf( buf2, "%s", olc_ed_vnum( ch ) );
				i = buf2; break;
			case 't' :
				sprintf( buf2, "%d", time_info.hour );
				i = buf2; break;
			case 'T' :
				sprintf( buf2, "%s", hour_name[ time_info.hour ] );
				i = buf2; break;
		}
		++str;
		while ( ( *point = *i ) != '\0' )
			++point, ++i;
	}
	/* tu memtick */
	if ( !IS_NPC( ch ) && ch->count_memspell > 0 )
	{
		ch->count_memspell = UMAX( 0, ch->count_memspell );

		if ( ch->count_memspell > 0 )
		{
			sprintf( buf2, " [%d mem] ", ch->count_memspell );
			i = buf2;
			while ( ( *point = *i ) != '\0' )
				++point, ++i;
		}
	}

	/* a tu automatic exits*/
	check = TRUE;

	if ( dirs ) check = FALSE;
	if ( !IS_NPC( ch ) && EXT_IS_SET( ch->act, PLR_AUTOEXIT ) ) check = FALSE;
	if ( !IS_NPC( ch ) && !EXT_IS_SET( ch->act, PLR_AUTOEXIT )
	     && !IS_IMMORTAL( ch ) && room_is_dark( ch, ch->in_room ) &&
	     !IS_AFFECTED( ch, AFF_DARK_VISION ) ) check = FALSE;
	if ( IS_NPC( ch ) ) check = TRUE;

	if ( check )
	{
		found = FALSE;
		doors[ 0 ] = '\0';

		for ( door = 0; door < 6; door++ )
		{
			if ( ( pexit = ch->in_room->exit[ door ] ) == NULL )
				continue;
			if ( pexit->u1.to_room == NULL )
				continue;
			/* nie mozna przerobic funkcji can_see_room, bo to uniemozliwiloby chodzenie po ciemku */
			if ( IS_AFFECTED( ch, AFF_BLIND ) )
				break;
			if ( !can_see_room( ch, pexit->u1.to_room ) )
				continue;

			if ( IS_SET( pexit->exit_info, EX_SECRET ) && !IS_IMMORTAL( ch ) )
				continue;

			if ( IS_SET( pexit->exit_info, EX_HIDDEN ) && !IS_AFFECTED( ch, AFF_DETECT_HIDDEN ) )
				continue;

			if ( !found )
				strcat( doors, "[ " );

			if ( pexit->vName && pexit->vName[ 0 ] != '\0' )
				sprintf( exit, "%s", pexit->vName );
			else
			{
				if ( IS_AFFECTED( ch, AFF_MAZE ) )
				{
					sprintf( exit, "%s", dir_name[ number_range( 0, 5 ) ] );
				}
				else
				{
					sprintf( exit, "%s", dir_name[ door ] );
				}
			}

			if ( IS_SET( pexit->exit_info, EX_HIDDEN ) || IS_SET( pexit->exit_info, EX_SECRET ) )
			{
				strcat( doors, "-" );
				strcat( doors, exit );
				strcat( doors, "-" );
			}
			else if ( IS_SET( pexit->exit_info, EX_CLOSED ) )
			{
				strcat( doors, "(" );
				strcat( doors, exit );
				strcat( doors, ")" );
			}
			else
			{
				strcat( doors, exit );
			}

			strcat( doors, " " );
			found = TRUE;
		}

		if ( !found )
			strcat( doors, " {c[ brak ]{x " );
		else
			strcat( doors, "]{x " );

		dirs = TRUE;
		sprintf( buf2, "%s", doors );
		i = buf2;

		while ( ( *point = *i ) != '\0' )
			++point, ++i;
	}
	/*******************/

	*point	= '\0';
	pbuff	= buffer;
	colourconv( pbuff, buf, ch );
	send_to_char( "{p", ch );
	write_to_buffer( ch->desc, buffer, 0 );
	send_to_char( "{x", ch );

	if ( ch->prefix[ 0 ] != '\0' )
		write_to_buffer( ch->desc, ch->prefix, 0 );
	return ;
}

/*
 * Append onto an output buffer.
 */
void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length )
{
	int i;
	char *ptr;
	static char buf [ MAX_STRING_LENGTH ];

	if ( !d )
	{
		bug( "Write_to_buffer: NULL descriptor", 0 );
		return ;
	}

	if ( !d->outbuf )
		return ;

	/*
	 * Find length in case caller didn't.
	 */
	if ( length <= 0 )
		length = strlen( txt );

	strcpy( buf, txt );

	for ( i = 0; i <= length; i++ )
	{
		if ( ( ptr = strchr( pol_iso, txt[ i ] ) ) )
		{
			if ( d->character )
			{
				if ( EXT_IS_SET( d->character->act, PLR_POL_WIN ) )
					buf[ i ] = pol_win[ ptr - pol_iso ];
				else if ( EXT_IS_SET( d->character->act, PLR_POL_NOPOL ) )
					buf[ i ] = pol_nopl[ ptr - pol_iso ];
				else if ( !EXT_IS_SET( d->character->act, PLR_POL_ISO ) )
					buf[ i ] = pol_nopl[ ptr - pol_iso ];
			}
			else
				buf[ i ] = pol_nopl[ ptr - pol_iso ];
		}
	}


	/*
	 * Initial \n\r if needed.
	 */
	if ( d->outtop == 0 && !d->fcommand )
	{
		d->outbuf[ 0 ] = '\n';
		d->outbuf[ 1 ] = '\r';
		d->outtop	= 2;
	}

	/*
	 * Expand the buffer as needed.
	 */
	while ( d->outtop + length >= d->outsize )
	{
		if ( d->outsize > 32000 )
		{
			/* empty buffer */
			d->outtop = 0;
			bugf( "Buffer overflow: %ld. Closing (%s).", d->outsize, d->character ? d->character->name : "?" );
			close_socket( d, 0 );
			return ;
		}
		d->outsize *= 2;
		RECREATE( d->outbuf, char, d->outsize );
	}

	/*
	 * Copy.
	 */
	strncpy( d->outbuf + d->outtop, buf, length );
	d->outtop += length;
	d->outbuf[ d->outtop ] = '\0';
	return ;
}

/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
#ifdef MCCP
bool write_to_descriptor( DESCRIPTOR_DATA *d, char *txt, int length )
{
	int iStart = 0;
	int nWrite = 0;
	int nBlock;
	int len;
	int desc;

	desc = d->descriptor;

	if ( length <= 0 )
		length = strlen( txt );

	if ( d && d->out_compress )
	{
		d->out_compress->next_in = ( unsigned char * ) txt;
		d->out_compress->avail_in = length;

		while ( d->out_compress->avail_in )
		{
			d->out_compress->avail_out = COMPRESS_BUF_SIZE - ( d->out_compress->next_out - d->out_compress_buf );

			if ( d->out_compress->avail_out )
			{
				int status = deflate( d->out_compress, Z_SYNC_FLUSH );

				if ( status != Z_OK )
					return FALSE;
			}

			len = d->out_compress->next_out - d->out_compress_buf;
			if ( len > 0 )
			{
				for ( iStart = 0; iStart < len; iStart += nWrite )
				{
					nBlock = UMIN ( len - iStart, 4096 );
					if ( ( nWrite = write( d->descriptor, d->out_compress_buf + iStart, nBlock ) ) < 0 )
					{
						perror( "Write_to_descriptor: compressed" );
						return FALSE;
					}

					if ( !nWrite )
						break;
				}

				if ( !iStart )
					break;

				if ( iStart < len )
					memmove( d->out_compress_buf, d->out_compress_buf + iStart, len - iStart );

				d->out_compress->next_out = d->out_compress_buf + len - iStart;
			}
		}
		return TRUE;
	}

	for ( iStart = 0; iStart < length; iStart += nWrite )
	{
		nBlock = UMIN ( length - iStart, 4096 );
		if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
		{
			perror( "Write_to_descriptor" );
			return FALSE;
		}
	}

	return TRUE;
}
#else
bool write_to_descriptor( DESCRIPTOR_DATA *d, char *txt, int length )
{
	int iStart;
	int nWrite;
	int nBlock;
	int desc;

	desc = d->descriptor;

	if ( length <= 0 )
		length = strlen( txt );

	for ( iStart = 0; iStart < length; iStart += nWrite )
	{
		nBlock = UMIN( length - iStart, 4096 );
		if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
		{
			perror( "Write_to_descriptor" ); return FALSE;
		}
	}

	return TRUE;
}
#endif

#ifndef NEW_NANNY
/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
	DESCRIPTOR_DATA * d_old;
	CLAN_DATA *clan;
	char buf[ MAX_STRING_LENGTH ];
	char arg[ MAX_INPUT_LENGTH ];
	CHAR_DATA *ch, *rch;
	char *pwdnew;
	int iClass, race, i;
	bool fOld;
	char* reserv_pwd;

	while ( isspace( *argument ) )
		argument++;

	if ( d->remote_char )
		ch = d->remote_char;
	else
		ch = d->character;

	if ( d->connected != CON_GET_NAME && !ch )
	{
		close_socket( d, 0 );
		return ;
	}

	switch ( d->connected )
	{

		default:
			bug( "Nanny: bad d->connected %d.", d->connected );
			close_socket( d, 0 );
			return ;

		case CON_NDENY_GET_NAME:
		case CON_GET_NAME:
			DEBUG_INFO( "nanny:CON_GET_NAME" );

			if ( argument[ 0 ] == '\0' )
			{
				close_socket( d, 0 );
				return ;
			}

			if ( (int) argument[0] == 39 ) //Kamili: It should resolve a "putty problem".
				argument++;

			//scinanie duzych liter
			for ( i = 0; argument[ i ] != '\0'; i++ )
				argument[ i ] = LOWER( argument[ i ] );

			//pierwsza wielka
			argument[ 0 ] = UPPER( argument[ 0 ] );

			if ( !check_parse_name( argument ) )
			{
				write_to_buffer( d, "Nielegalne imie, sprobuj jeszcze raz.\n\rImie: ", 0 );
				return ;
			}


			if ( d->connected == CON_GET_NAME )
			{
				if ( ( fOld = player_exists( argument ) ) == TRUE )
				{
					d->remote_char = load_char_remote( argument );
					ch = d->remote_char;
				}
				else
				{
					if ( strlen( argument ) < 4 )
					{
						write_to_buffer( d, "Imie zbyt krotkie, sprobuj jeszcze raz.\n\rImie: ", 0 );
						return ;
					}
					if ( check_creating( d, argument ) )
                    {
						return ;
                    }
					load_char_obj( d, argument, FALSE );
					ch = d->character;
				}

				if ( is_allowed( argument ) == NAME_DISALLOWED )
				{
					if ( fOld )
					{
						if ( !IS_IMMORTAL( ch ) )
						{
							write_to_buffer( d, "To imiê nie zosta³o zaakceptowane. Po podaniu aktualnego has³a otrzymasz\n\r", 0 );
							write_to_buffer( d, "mo¿liwo¶æ wyboru nowego imienia dla tej postaci oraz odmiany tego imienia.\n\r", 0 );
							if ( !IS_NULLSTR( ch->pcdata->name_deny_txt ) )
							{
								write_to_buffer( d, "Komentarz dotycz±cy niezaakceptowania imienia:\n\r", 0 );
								sprintf( buf, "'%s'\n\r", ch->pcdata->name_deny_txt );
								write_to_buffer( d, buf, 0 );
							}
							write_to_buffer( d, "Podaj aktualne has³o: ", 0 );
							d->connected = CON_NDENY_OLD_PASS;
							d->pass_fail = 0;
						}
					}
					else
						write_to_buffer( d, "Nielegalne imie, sprobuj jeszcze raz.\n\rImie: ", 0 );
					return ;
				}

				if ( EXT_IS_SET( ch->act, PLR_DENY ) )
				{
					sprintf( log_buf, "Denying access to %s@%s.", argument, d->host );
					log_string( log_buf );
					write_to_buffer( d, "Niestety masz zakaz wjazdu.\n\r", 0 );
					close_socket( d, 0 );
					return ;
				}
				if ( check_ban( d->host, BAN_PERMIT ) && !EXT_IS_SET( ch->act, PLR_PERMIT ) )
				{
					write_to_buffer( d, "No ³adnie, Twój adres jest zbanowany.\n\r", 0 );
					close_socket( d, 0 );
					return ;
				}
				if ( !IS_IMMORTAL( ch ) && check_host_broken_link( d ) )
				{
					write_to_buffer( d, "Dostêp zabroniony do czasu wyj¶cia z gry postaci z zerwanym po³±czeniem z tego adresu.\n\r", 0 );
					close_socket( d, 0 );
					return ;
				}
				if ( check_reconnect( d, argument, FALSE ) )
				{
					fOld = TRUE;
				}
				else
				{
					if ( wizlock && !IS_IMMORTAL( ch ) )
					{
						write_to_buffer( d, "Chwilowo mud jest zamkniêty dla graczy, zapraszamy za chwilê.\n\r", 0 );
						close_socket( d, 0 );
						return ;
					}
				}
			}
			else
			{
				fOld = player_exists( argument );
			}

			if ( fOld )
			{
				if ( d->connected == CON_NDENY_GET_NAME )
				{
					write_to_buffer( d, "Postac o takim imieniu juz istnieje.\n\rPodaj nowe imie: ", 0 );
					return ;
				}

				/* Old player */
				write_to_buffer( d, "Haslo: ", 0 );
				write_to_buffer( d, echo_off_str, 0 );
				d->connected = CON_GET_OLD_PASSWORD;
				d->pass_fail = 0;
				return ;
			}
			else
			{
				if ( d->connected == CON_NDENY_GET_NAME )
				{
					if ( is_allowed( argument ) == NAME_DISALLOWED )
					{
						write_to_buffer( d, "Nielegalne imie, sprobuj jeszcze raz.\n\rPodaj nowe imie: ", 0 );
						return ;
					}
				}

				/* New player */
				if ( newlock )
				{
					write_to_buffer( d, "Chwilowo nie mo¿na robiæ nowych postaci.\n\r", 0 );
					close_socket( d, 0 );
					return ;
				}

				if ( check_ban( d->host, BAN_NEWBIES ) )
				{
					write_to_buffer( d,
					                 "Masz zakaz robienia nowych postaci.\n\r", 0 );
					close_socket( d, 0 );
					return ;
				}

				free_string( ch->old_name );
				ch->old_name = str_dup( ch->name );
				free_string( ch->name );
				ch->name = str_dup( argument );

				// stuff do rezerwacji imion
				if ( ( reserv_pwd = is_name_reserved( argument ) ) != NULL )
				{
					write_to_buffer( d, "To imiê zosta³o przez kogo¶ zarezerwowane. Aby udowodniæ, ¿e to by³o imiê\n\r", 0 );
					write_to_buffer( d, "Twojej postaci wpisz has³o, które postaæ mia³a w momencie rezerwacji.\n\r", 0 );
					write_to_buffer( d, "Poprawne has³o zostanie jednocze¶nie has³em nowej postaci. Has³o mo¿na\n\r", 0 );
					write_to_buffer( d, "potem zmieniæ z poziomu menu wej¶cia do gry.\n\r", 0 );
					write_to_buffer( d, "Has³o: ", 0 );
					write_to_buffer( d, echo_off_str, 0 );
					if ( d->connected == CON_GET_NAME )
						d->connected = CON_GET_RESERV_PASSWORD;
					else
						d->connected = CON_NDENY_RSV_PASS;
					d->pass_fail = 0;
					free_string( ch->pcdata->pwd );
					ch->pcdata->pwd = reserv_pwd;
					return ;
				}

				write_to_buffer( d, "UWAGA!\n\r", 0 );
				write_to_buffer( d, "W naszym ¶wiecie istnieje szereg zasad dotycz±cych dopuszczalnych imion.\n\r", 0 );
				write_to_buffer( d, "Zabronione s± imiona które nie daja siê sensownie odmieniæ przez przypadki,\n\r", 0 );
				write_to_buffer( d, "s± bezsensownym zlepkiem liter, nie pasuj± do klimatu fantasy lub s±\n\r", 0 );
				write_to_buffer( d, "imionami bohaterow literackich. Ogólnie prosimy kierowaæ siê trzema zasadami:\n\r", 0 );
				write_to_buffer( d, "imiê powinno byæ ORYGINALNE, a co za tym idzie, nie powinno kojarzyæ siê\n\r", 0 );
				write_to_buffer( d, "z niczym istniej±cym w ¶wiecie rzeczywistym oraz powinno zachowaæ styl FANTASY.\n\r", 0 );
				write_to_buffer( d, "Kryteria oceny pozostaj± w naszej gestii. Imiona nie pasuj±ce do naszego ¶wiata\n\r", 0 );
				write_to_buffer( d, "zostan± odrzucone, wiêc prosimy postaraæ siê ju¿ na etapie tworzenia postaci.\n\r", 0 );
				write_to_buffer( d, "O imionach (i nie tylko) mo¿na poczytaæ na stronie muda:\n\r", 0 );
				write_to_buffer( d, "http://www.killer-mud.net/\n\r\n\r", 0 );

				sprintf( buf, "Czy na pewno chcesz mieæ takie imie: %s (T/N)? ", argument );

				write_to_buffer( d, buf, 0 );
				if ( d->connected == CON_GET_NAME )
					d->connected = CON_CONFIRM_NEW_NAME;
				else
					d->connected = CON_NDENY_CONFIRM_NAME;
				return ;
			}
			break;

		case CON_NDENY_OLD_PASS:
		case CON_GET_OLD_PASSWORD:
			DEBUG_INFO( "nanny:CON_GET_OLD_PASSWORD" );
#if defined(unix)
			/*	write_to_buffer( d, "\n\r", 2 );*/
#endif

			if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
			{
				if ( d->pass_fail > 1 )
				{
					write_to_buffer( d, "Zle haslo.", 0 );
					close_socket( d, 0 );
					return ;
				}
				else
				{
					write_to_buffer( d, "\n\rSprobuj jeszcze raz :", 0 );
					if ( d->connected == CON_GET_OLD_PASSWORD )
						d->connected = CON_GET_OLD_PASSWORD;
					else
						d->connected = CON_NDENY_OLD_PASS;
					d->pass_fail++;
					return ;
				}
			}

			write_to_buffer( d, echo_on_str, 0 );

			if ( d->connected == CON_NDENY_OLD_PASS )
			{
				if ( check_creating( d, ch->name ) )
					return ;

				write_to_buffer( d, "\n\rPodaj nowe imie:", 0 );
				d->connected = CON_NDENY_GET_NAME;
				return ;
			}

			if ( check_playing( d, ch->name ) )
				return ;

			if ( check_reconnect( d, ch->name, TRUE ) )
				return ;

			if ( d->remote_char )
			{
				load_char_obj( d, ch->name, FALSE );
				free_char( d->remote_char );
				d->remote_char = NULL;
				ch = d->character;
			}

			sprintf( log_buf, "%s %s@%s has connected.", ch->name, d->user, d->host );
			log_string( log_buf );
			wiznet( log_buf, NULL, NULL, WIZ_SITES, 0, get_trust( ch ) );

			/*artefact*/
			if ( !IS_IMMORTAL ( ch ) ) restore_char_on_artefact_list( ch );

			do_function( ch, &do_help, "motd" );

			if ( !IS_NULLSTR( ch->pcdata->last_host ) )
			{
				write_to_buffer( d, "\n\r* Poprzednie logowanie: ", 0 );
				write_to_buffer( d, ch->pcdata->last_host, 0 );
				write_to_buffer( d, "\n\r", 0 );
			}

			check_todelete( ch ); // delayed ch delete - by Fuyara
			add_host( ch, d->host, TRUE );
			free_string( ch->pcdata->last_host );
			ch->pcdata->last_host = str_dup( d->host );

			d->connected = CON_MENU;
			menu_show( d->character );
			break;

		case CON_NDENY_RSV_PASS:
		case CON_GET_RESERV_PASSWORD:
			DEBUG_INFO( "nanny:CON_GET_RESERV_PASSWORD" );

			if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
			{
				if ( d->pass_fail > 1 )
				{
					write_to_buffer( d, "Zle haslo.", 0 );
					close_socket( d, 0 );
					return ;
				}
				else
				{
					write_to_buffer( d, "\n\rSprobuj jeszcze raz :", 0 );
					if ( d->connected == CON_GET_RESERV_PASSWORD )
						d->connected = CON_GET_RESERV_PASSWORD;
					else
						d->connected = CON_NDENY_RSV_PASS;
					d->pass_fail++;
					return ;
				}
			}

			write_to_buffer( d, echo_on_str, 0 );
			write_to_buffer( d, "\n\rTeraz musisz podac odmiane swojego imienia. Potrzebne jest to do poprawnej\n\r", 0 );
			write_to_buffer( d, "interakcji Twojej postaci z otoczeniem, dlatego prosimy o szczegolna uwage\n\r", 0 );
			write_to_buffer( d, "przy jej wpisywaniu.\n\r", 0 );
			write_to_buffer( d, "Wpisz odmiane swojego imienia:\n\r", 0 );

			write_to_buffer( d, "Mianownik (kto, co): ", 0 );
			write_to_buffer( d, ch->name, 0 );
			write_to_buffer( d, "\n\r", 0 );
			write_to_buffer( d, "Dopelniacz (kogo, czego): ", 0 );
			if ( d->connected == CON_GET_RESERV_PASSWORD )
				d->connected = CON_GET_ODMIANA;
			else
			{
				free_string( ch->name2 );
				free_string( ch->name3 );
				free_string( ch->name4 );
				free_string( ch->name5 );
				free_string( ch->name6 );

				ch->name2 = str_dup( "null" );
				ch->name3 = str_dup( "null" );
				ch->name4 = str_dup( "null" );
				ch->name5 = str_dup( "null" );
				ch->name6 = str_dup( "null" );
				d->connected = CON_NDENY_GET_ODMIANA;
			}
			break;

			/* RT code for breaking link */

		case CON_BREAK_CONNECT:
			DEBUG_INFO( "nanny:CON_BREAK_CONNECT" );
			switch ( *argument )
			{
			case 't' : case 'T':
					for ( d_old = descriptor_list; d_old != NULL; d_old = d_old->next )
					{
						if ( d_old == d || d_old->character == NULL )
							continue;

						if ( str_cmp( ch->name, d_old->original ?
						              d_old->original->name : d_old->character->name ) )
							continue;

						close_socket( d_old, CS_NORECURSE );
					}

					if ( check_reconnect( d, ch->name, TRUE ) )
						return ;

					write_to_buffer( d, "Powtorne polaczenie nieudane.\n\rImie: ", 0 );

					if ( d->character != NULL )
					{
						free_char( d->character );
						d->character = NULL;
					}
					if ( d->remote_char != NULL )
					{
						free_char( d->remote_char );
						d->remote_char = NULL;
					}
					d->connected = CON_GET_NAME;
					break;

			case 'n' : case 'N':
					write_to_buffer( d, "Imie: ", 0 );
					if ( d->remote_char != NULL )
					{
						free_char( d->remote_char );
						d->remote_char = NULL;
					}
					if ( d->character != NULL )
					{
						free_char( d->character );
						d->character = NULL;
					}
					d->connected = CON_GET_NAME;
					break;

				default:
					write_to_buffer( d, "Wpisz T lub N? ", 0 );
					break;
			}
			break;

		case CON_NDENY_CONFIRM_NAME:
		case CON_CONFIRM_NEW_NAME:
			DEBUG_INFO( "nanny:CON_CONFIRM_NEW_NAME" );
			if ( check_playing( d, ch->name ) )
				return ;
			if ( d->connected == CON_CONFIRM_NEW_NAME )
			{
				switch ( *argument )
				{
				case 't': case 'T':
						sprintf( buf, "Nowa postac.\n\rPodaj haslo dla %s: %s",
						         ch->name, echo_off_str );
						write_to_buffer( d, buf, 0 );
						d->connected = CON_GET_NEW_PASSWORD;
						break;

				case 'n': case 'N':
						write_to_buffer( d, "Ok, to jakie ma byc? ", 0 );
						free_char( d->character );
						d->character = NULL;
						d->connected = CON_GET_NAME;
						break;

					default:
						write_to_buffer( d, "Wpisz T lub N? ", 0 );
						break;
				}
			}
			else
			{
				switch ( *argument )
				{
				case 't': case 'T':
						free_string( ch->name2 );
						free_string( ch->name3 );
						free_string( ch->name4 );
						free_string( ch->name5 );
						free_string( ch->name6 );

						ch->name2 = str_dup( "null" );
						ch->name3 = str_dup( "null" );
						ch->name4 = str_dup( "null" );
						ch->name5 = str_dup( "null" );
						ch->name6 = str_dup( "null" );

						write_to_buffer( d, echo_on_str, 0 );
						write_to_buffer( d, "\n\rTeraz musisz podac odmiane swojego imienia. Potrzebne jest to do poprawnej\n\r", 0 );
						write_to_buffer( d, "interakcji Twojej postaci z otoczeniem, dlatego prosimy o szczegolna uwage\n\r", 0 );
						write_to_buffer( d, "przy jej wpisywaniu.\n\r", 0 );
						write_to_buffer( d, "Wpisz odmiane swojego imienia:\n\r", 0 );

						write_to_buffer( d, "Mianownik (kto, co): ", 0 );
						write_to_buffer( d, ch->name, 0 );
						write_to_buffer( d, "\n\r", 0 );
						write_to_buffer( d, "Dopelniacz (kogo, czego): ", 0 );
						d->connected = CON_NDENY_GET_ODMIANA;
						break;

				case 'n': case 'N':
						write_to_buffer( d, "Ok, to jakie ma byc? ", 0 );
						d->connected = CON_NDENY_GET_NAME;
						break;

					default:
						write_to_buffer( d, "Wpisz T lub N? ", 0 );
						break;
				}
			}
			break;

		case CON_GET_NEW_PASSWORD:
			DEBUG_INFO( "nanny:CON_GET_NEW_PASSWORD" );
#if defined(unix)
			write_to_buffer( d, "\n\r", 2 );
#endif

			pwdnew = crypt( argument, ch->name );

			if ( !check_password( d, "", pwdnew ) )
			{
				write_to_buffer( d, "Haslo: ", 0 );
				return ;
			}

			free_string( ch->pcdata->pwd );
			ch->pcdata->pwd	= str_dup( pwdnew );
			write_to_buffer( d, "Potwierdz haslo: ", 0 );
			d->connected = CON_CONFIRM_NEW_PASSWORD;
			break;

		case CON_CONFIRM_NEW_PASSWORD:
			DEBUG_INFO( "nanny:CON_CONFIRM_NEW_PASSWORD" );
#if defined(unix)
			write_to_buffer( d, "\n\r", 2 );
#endif

			if ( strcmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
			{
				write_to_buffer( d, "Potwierdzenie nieudane.\n\rWpisz jeszcze raz haslo: ", 0 );
				d->connected = CON_GET_NEW_PASSWORD;
				return ;
			}

			write_to_buffer( d, echo_on_str, 0 );
			write_to_buffer( d, "\n\rTeraz musisz podac odmiane swojego imienia. Potrzebne jest to do poprawnej\n\r", 0 );
			write_to_buffer( d, "interakcji Twojej postaci z otoczeniem, dlatego prosimy o szczegolna uwage\n\r", 0 );
			write_to_buffer( d, "przy jej wpisywaniu.\n\r", 0 );
			write_to_buffer( d, "Wpisz odmiane swojego imienia:\n\r", 0 );

			write_to_buffer( d, "Mianownik (kto, co): ", 0 );
			write_to_buffer( d, ch->name, 0 );
			write_to_buffer( d, "\n\r", 0 );
			write_to_buffer( d, "Dopelniacz (kogo, czego): ", 0 );
			d->connected = CON_GET_ODMIANA;
			break;

		case CON_NDENY_GET_ODMIANA:
		case CON_GET_ODMIANA:
			DEBUG_INFO( "nanny:CON_GET_ODMIANA" );
			if ( !str_cmp( ch->name2, "null" ) )
			{
				one_argument( argument, arg );
				smash_tilde( arg );

				if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( ch->name[ 0 ] ) || !check_all_alpha ( arg ) )
				{
					write_to_buffer( d, "Dopelniacz (kogo, czego): ", 0 );
					break;
				}
				arg[ 0 ] = UPPER( arg[ 0 ] );
				ch->name2 = str_dup( arg );
				write_to_buffer( d, "Celownik (komu, czemu): ", 0 );
				break;
			}

			if ( !str_cmp( ch->name3, "null" ) )
			{
				one_argument( argument, arg );
				smash_tilde( arg );

				if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( ch->name[ 0 ] ) || !check_all_alpha ( arg ) )
				{
					write_to_buffer( d, "Celownik (komu, czemu): ", 0 );
					break;
				}

				arg[ 0 ] = UPPER( arg[ 0 ] );
				ch->name3 = str_dup( arg );
				write_to_buffer( d, "Biernik (kogo, co): ", 0 );
				break;
			}

			if ( !str_cmp( ch->name4, "null" ) )
			{
				one_argument( argument, arg );
				smash_tilde( arg );

				if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( ch->name[ 0 ] ) || !check_all_alpha ( arg ) )
				{
					write_to_buffer( d, "Biernik (kogo, co): ", 0 );
					break;
				}

				arg[ 0 ] = UPPER( arg[ 0 ] );
				ch->name4 = str_dup( arg );
				write_to_buffer( d, "Narzednik (z kim, z czym): ", 0 );
				break;
			}

			if ( !str_cmp( ch->name5, "null" ) )
			{
				one_argument( argument, arg );
				smash_tilde( arg );

				if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( ch->name[ 0 ] ) || !check_all_alpha ( arg ) )
				{
					write_to_buffer( d, "Narzednik (z kim, z czym): ", 0 );
					break;
				}

				arg[ 0 ] = UPPER( arg[ 0 ] );
				ch->name5 = str_dup( arg );
				write_to_buffer( d, "Miejscownik (o kim, o czym): ", 0 );
				break;
			}

			if ( !str_cmp( ch->name6, "null" ) )
			{
				one_argument( argument, arg );
				smash_tilde( arg );

				if ( arg[ 0 ] == '\0' || NOPOL( arg[ 0 ] ) != NOPOL( ch->name[ 0 ] ) || !check_all_alpha ( arg ) )
				{
					write_to_buffer( d, "Miejscownik (o kim, o czym): ", 0 );
					break;
				}

				arg[ 0 ] = UPPER( arg[ 0 ] );
				ch->name6 = str_dup( arg );
				write_to_buffer( d, "\n\rOdmiana:\n\r", 0 );
				write_to_buffer( d, "Mianownik:    ", 0 );
				write_to_buffer( d, ch->name, 0 );
				write_to_buffer( d, "\n\r", 0 );
				write_to_buffer( d, "Dopelniacz:   ", 0 );
				write_to_buffer( d, ch->name2, 0 );
				write_to_buffer( d, "\n\r", 0 );
				write_to_buffer( d, "Celownik:     ", 0 );
				write_to_buffer( d, ch->name3, 0 );
				write_to_buffer( d, "\n\r", 0 );
				write_to_buffer( d, "Biernik:      ", 0 );
				write_to_buffer( d, ch->name4, 0 );
				write_to_buffer( d, "\n\r", 0 );
				write_to_buffer( d, "Narzednik:    ", 0 );
				write_to_buffer( d, ch->name5, 0 );
				write_to_buffer( d, "\n\r", 0 );
				write_to_buffer( d, "Miejscownik:  ", 0 );
				write_to_buffer( d, ch->name6, 0 );
				write_to_buffer( d, "\n\rPoprawna odmiana?  [t/n] (s-pokaz odmiane)", 0 );
				if ( d->connected == CON_GET_ODMIANA )
					d->connected = CON_ACCEPT_ODMIANA;
				else
					d->connected = CON_NDENY_ACCEPT_ODMIANA;
				break;
			}

		case CON_NDENY_ACCEPT_ODMIANA:
		case CON_ACCEPT_ODMIANA:
			DEBUG_INFO( "nanny:CON_ACCEPT_ODMIANA" );
			switch ( *argument )
			{
			case 't': case 'T':
					{
						if ( d->connected == CON_NDENY_ACCEPT_ODMIANA )
						{
							rch = ch;
							load_char_obj( d, rch->old_name, FALSE );
							ch = d->character;
							free_string( ch->name );
							free_string( ch->name2 );
							free_string( ch->name3 );
							free_string( ch->name4 );
							free_string( ch->name5 );
							free_string( ch->name6 );
							free_string( ch->pcdata->pwd );
							free_string( ch->pcdata->name_deny_txt );

							ch->name = str_dup( rch->name );
							ch->name2 = str_dup( rch->name2 );
							ch->name3 = str_dup( rch->name3 );
							ch->name4 = str_dup( rch->name4 );
							ch->name5 = str_dup( rch->name5 );
							ch->name6 = str_dup( rch->name6 );
							ch->pcdata->pwd = str_dup( rch->pcdata->pwd );
							ch->pcdata->name_deny_txt = str_dup( "" );

							sprintf( buf, "%s%s", PLAYER_DIR, capitalize( rch->old_name ) );
							unlink( buf );
							free_char( rch );
							d->remote_char = NULL;

							if ( ch->level >= 2 && is_allowed( ch->name ) == NAME_NOTEXIST )
							{
								add_name( ch->name, NAME_NEW );
								EXT_SET_BIT( ch->act, PLR_UNREGISTERED );
							}

							save_char_obj( ch, FALSE, FALSE );
							do_function( ch, &do_help, "motd" );
							d->connected = CON_MENU;
							menu_show( d->character );

							return ;
						}

						write_to_buffer( d, "\n\r", 0 );
						write_to_buffer( d, echo_on_str, 0 );
						write_to_buffer( d, "Teraz stoi przed Toba zadanie wybrania rasy swojej postaci. Jest to\n\r", 0 );
						write_to_buffer( d, "bardzo wazny punkt tworzenia postaci, gdyz to jaka rase wybierzesz bedzie\n\r", 0 );
						write_to_buffer( d, "wplywac na wiele roznych cech Twojej postaci. Pamietaj takze, iz nie kazda\n\r", 0 );
						write_to_buffer( d, "rasa ma dostep do wszystkich profesji.\n\r", 0 );

						write_to_buffer( d, "Dostepne rasy:\n\r", 0 );

						for ( race = 1; race_table[ race ].name != NULL; race++ )
						{
							if ( !race_table[ race ].pc_race )
								break;

							sprintf( buf, "%d) %-15.15s [ ", race, race_table[ race ].name );

							for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
							{
								if ( iClass == CLASS_MONK || iClass == CLASS_BARD )
									continue;
								if ( IS_SET( pc_race_table[ race ].class_flag, ( 1 << iClass ) ) )
								{
									strcat( buf, class_table[ iClass ].name );
									strcat( buf, " " );
								}
							}

							strcat( buf, "]\n\r" );
							write_to_buffer( d, buf, 0 );
						}
						write_to_buffer( d, "\n\r", 0 );
						write_to_buffer( d, "Jaka rase wybierasz? ", 0 );
						d->connected = CON_GET_NEW_RACE;
						break;
					}

			case 'n': case 'N':
					{
						free_string( ch->name2 );
						free_string( ch->name3 );
						free_string( ch->name4 );
						free_string( ch->name5 );
						free_string( ch->name6 );
						ch->name2 = str_dup( "null" );
						ch->name3 = str_dup( "null" );
						ch->name4 = str_dup( "null" );
						ch->name5 = str_dup( "null" );
						ch->name6 = str_dup( "null" );

						write_to_buffer( d, "\n\rWpisz odmiane swojego imienia:\n\r", 0 );
						write_to_buffer( d, "Mianownik (kto, co): ", 0 );
						write_to_buffer( d, ch->name, 0 );
						write_to_buffer( d, "\n\r", 0 );
						write_to_buffer( d, "Dopelniacz (kogo, czego): ", 0 );
						if ( d->connected == CON_ACCEPT_ODMIANA )
							d->connected = CON_GET_ODMIANA;
						else
							d->connected = CON_NDENY_GET_ODMIANA;
						break;
					}

			case 's': case 'S':
					{
						write_to_buffer( d, "\n\rOdmiana: ", 0 );
						write_to_buffer( d, "\n\rMianownik:   ", 0 );
						write_to_buffer( d, ch->name, 0 );
						write_to_buffer( d, "\n\r", 0 );
						write_to_buffer( d, "Dopelniacz:  ", 0 );
						write_to_buffer( d, ch->name2, 0 );
						write_to_buffer( d, "\n\r", 0 );
						write_to_buffer( d, "Celownik:    ", 0 );
						write_to_buffer( d, ch->name3, 0 );
						write_to_buffer( d, "\n\r", 0 );
						write_to_buffer( d, "Biernik:     ", 0 );
						write_to_buffer( d, ch->name4, 0 );
						write_to_buffer( d, "\n\r", 0 );
						write_to_buffer( d, "Narzednik:   ", 0 );
						write_to_buffer( d, ch->name5, 0 );
						write_to_buffer( d, "\n\r", 0 );
						write_to_buffer( d, "Miejscownik: ", 0 );
						write_to_buffer( d, ch->name6, 0 );
						write_to_buffer( d, "\n\r", 0 );
						write_to_buffer( d, "\n\rPoprawna odmiana?  [t/n] (s-pokaz odmiane)", 0 );
						break;
					}
				default:
					write_to_buffer( d, "\n\rWpisz 't','n' lub 's'.", 0 );
					return ;
			}
			break;


		case CON_GET_NEW_RACE:
			DEBUG_INFO( "nanny:CON_GET_NEW_RACE" );
			one_argument( argument, arg );

			if ( !strcmp( arg, "help" ) )
			{
				argument = one_argument( argument, arg );

				if ( argument[ 0 ] == '\0' )
					do_function( ch, &do_help, "race help" );
				else
					do_function( ch, &do_help, argument );

				write_to_buffer( d, "Jaka rase wybierasz? ", 0 );

				break;
			}

			if ( is_number( arg ) )
			{
				race = atoi( arg );

				if ( race <= 0 || race > 12 || !race_table[ race ].pc_race )
					race = 0;
			}
			else
				race = race_lookup( argument );


			if ( race == 0 || !race_table[ race ].pc_race )
			{
				write_to_buffer( d, "Rasa nierozpoznana.\n\r", 0 );
				write_to_buffer( d, "Dostepne rasy:\n\r", 0 );

				for ( race = 1; race_table[ race ].name != NULL; race++ )
				{
					if ( !race_table[ race ].pc_race )
						break;

					sprintf( buf, "%d) %-15.15s [ ", race, race_table[ race ].name );

					for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
					{
						if ( iClass == CLASS_MONK || iClass == CLASS_BARD )
							continue;
						if ( IS_SET( pc_race_table[ race ].class_flag, ( 1 << iClass ) ) )
						{
							strcat( buf, class_table[ iClass ].name );
							strcat( buf, " " );
						}
					}

					strcat( buf, "]\n\r" );
					write_to_buffer( d, buf, 0 );
				}

				write_to_buffer( d, "\n\r", 0 );
				write_to_buffer( d, "Jaka rase wybierasz?", 0 );
				break;
			}

			sprintf( buf, "Wybrano rase: %s.\n\r", race_table[ race ].name );
			write_to_buffer( d, buf, 0 );
			SET_RACE( ch, race );

			/* znane w 90 + bonus jezyki */
			for ( i = 0; i < MAX_LANG; i++ )
			{
				if ( pc_race_table[ GET_REAL_RACE( ch ) ].well_known_lang & lang_table[ i ].bit )
				{
					ch->pcdata->language[ i ] = number_range( 80, 100 );
				}

				if ( pc_race_table[ GET_REAL_RACE( ch ) ].known_lang & lang_table[ i ].bit )
					ch->pcdata->language[ i ] = number_range( 45, 75 );
			}
			ch->speaking = 0;

			ext_flags_copy( ext_flags_sum( ch->affected_by, ext_flag_value( affect_flags, race_table[ race ].aff ) ), ch->affected_by );
			ch->form	= race_table[ race ].form;
			ch->parts	= race_table[ race ].parts;


			ch->size = pc_race_table[ race ].size;

			write_to_buffer( d, "\n\rKolejnym punktem tworzenia postaci jest wybor plci. Musisz pamietac o\n\r", 0 );
			write_to_buffer( d, "tym, ze plec postaci nie musi byc taka sama jak Twoja plec. W tego typu\n\r", 0 );
			write_to_buffer( d, "grach chodzi przede wszystkim o odgrywanie postaci. Cokolwiek wybierzesz,\n\r", 0 );
			write_to_buffer( d, "w grze musisz sie zachowywac tak, jak robilaby to grana przez Ciebie postac.\n\r", 0 );
			write_to_buffer( d, "Jaka plec wybierasz (M/K)? ", 0 );

			d->connected = CON_GET_NEW_SEX;
			break;


		case CON_GET_NEW_SEX:
			DEBUG_INFO( "nanny:CON_GET_NEW_SEX" );
			switch ( argument[ 0 ] )
			{
			case 'm': case 'M': ch->sex = SEX_MALE;
					ch->pcdata->true_sex = SEX_MALE;
					break;
			case 'k': case 'K': ch->sex = SEX_FEMALE;
					ch->pcdata->true_sex = SEX_FEMALE;
					break;
				default:
					write_to_buffer( d, "To nie jest plec.\n\rJaka plec? ", 0 );
					return ;
			}

			write_to_buffer( d, "\n\rProfesja to najwazniejsza cecha Twojej postaci. Wplywa na to jakich\n\r", 0 );
			write_to_buffer( d, "umiejetnosci bedzie mogla sie nauczyc oraz jaka role bedzie odgrywac w grze.\n\r", 0 );
			write_to_buffer( d, "Dla czesci profesji jedyna droga przezycia jest poslugiwanie sie magia. Inne\n\r", 0 );
			write_to_buffer( d, "znakomicie radza sobie calkowicie bez niej. Sa i takie, ktore swoja fizyczna\n\r", 0 );
			write_to_buffer( d, "sile wspomagaja odrobina magicznej mocy.\n\r", 0 );

			write_to_buffer( d, "Dostepne profesje :\n\r", 0 );

			for ( race = 1, iClass = 0; iClass < MAX_CLASS; iClass++ )
			{
				if ( iClass == CLASS_MONK || iClass == CLASS_BARD )
					continue;

				if ( class_ok( ch, iClass ) )
				{
					sprintf( buf, "%d) %s\n\r", race++, class_table[ iClass ].name );
					write_to_buffer( d, buf, 0 );
				}
			}

			write_to_buffer( d, "Wybierz profesje :", 0 );

			d->connected = CON_GET_NEW_CLASS;
			break;

		case CON_GET_NEW_CLASS:
			DEBUG_INFO( "nanny:CON_GET_NEW_CLASS" );

			if ( is_number( argument ) )
			{
				int new_class = atoi( argument );

				if ( new_class < 0 || new_class > MAX_CLASS )
				{
					iClass = -1;
				}
				else
				{
					for ( i = 1, iClass = 0; iClass < MAX_CLASS; iClass++ )
					{
						if ( iClass == CLASS_MONK || iClass == CLASS_BARD )
							continue;

						if ( class_ok( ch, iClass ) && i++ == new_class )
							break;
					}
					if ( ( i - 1 ) != new_class )
						iClass = -1;
				}
			}
			else
			{
				iClass = class_lookup( argument );

				if ( iClass == CLASS_MONK || iClass == CLASS_BARD || iClass == CLASS_SHAMAN )
				{
					iClass = -1;
				}

				if ( iClass != -1 && !class_ok( ch, iClass ) )
				{
					sprintf( buf, "Profesja '%s' jest niedostepna dla rasy '%s'.\n\r",
					         class_table[ iClass ].name, race_table[ GET_RACE( ch ) ].name );
					write_to_buffer( d, buf, 0 );
					iClass = -1;
				}
			}

			if ( iClass == CLASS_MONK || iClass == CLASS_BARD )
				iClass = -1;

			if ( iClass == -1 )
			{
				write_to_buffer( d, "\n\rDostepne profesje :\n\r", 0 );

				for ( race = 1, iClass = 0; iClass < MAX_CLASS; iClass++ )
				{
					if ( iClass == CLASS_MONK || iClass == CLASS_BARD || iClass == CLASS_SHAMAN )
						continue;

					if ( class_ok( ch, iClass ) )
					{
						sprintf( buf, "%d) %s\n\r", race++, class_table[ iClass ].name );
						write_to_buffer( d, buf, 0 );
					}
				}

				return ;
			}


			sprintf( buf, "Wybrales profesje: %s\n\r", class_table[ iClass ].name );
			write_to_buffer( d, buf, 0 );
			ch->class = iClass;

			/* paladyni nie wybieraja charakteru*/
			if ( ch->class == CLASS_PALADIN )
			{
				ch->alignment = 1000;
				ch->pcdata->learned[ gsn_lay ] = 10;
				d->connected = CON_FINISH_CREATE;
				break;
			}
			if ( ch->class == CLASS_BLACK_KNIGHT )
			{
				ch->alignment = -1000;
				ch->pcdata->learned[ gsn_torment ] = 10;
				d->connected = CON_FINISH_CREATE;
				break;
			}
			/* druidzi zawsze neutralni */
			else if ( ch->class == CLASS_DRUID )
			{
				ch->alignment = 0;
				d->connected = CON_FINISH_CREATE;
				break;
			}
			/* magowie moze chca byc specjalistami */
			else if ( ch->class == CLASS_MAG )
			{
				int school, x;
				bool specialist = FALSE;

				/* sprawdzamy czy w ogole moze byc specjalista */
				for ( school = 0 ; school < MAX_SCHOOL ; school++ )
				{
					if ( can_be_specialist( ch, school ) )
					{
						specialist = TRUE;
						break;
					}
				}

				/* jesli moze byc specjalista */
				if ( specialist )
				{
					write_to_buffer( d, "\n\rMozesz wybrac specjalizacje w jednej z ponizszych szkol magii, lub nacisnij [ENTER].\n\r", 0 );

					for ( x = 1, school = 0 ; school < MAX_SCHOOL ; school++ )
					{
						if ( can_be_specialist( ch, school ) )
						{
							sprintf( buf, "%d) %-20s [ %-13s]\n\r",
							         x++,
							         school_table[ school ].specialist_name,
							         school_table[ school ].name );

							write_to_buffer( d, buf, 0 );
						}
					}

					d->connected = CON_GET_MAGE_SPECIALIST;
					break;
				}
			}



			sprintf( log_buf, "%s@%s nowy gracz.", ch->name, d->host );
			log_string( log_buf );
			wiznet( "Jakis nowy koles!  $N ma na imie.", ch, NULL, WIZ_NEWBIE, 0, 0 );
			wiznet( log_buf, NULL, NULL, WIZ_SITES, 0, get_trust( ch ) );

			write_to_buffer( d, "\n\r", 2 );
			write_to_buffer( d, "Ostatnim etapem tworzenia postaci jest wybor jej charakteru. Mozesz\n\r", 0 );
			write_to_buffer( d, "zadeklarowac czy chcesz byc z natury postacia dobra i zwalczac pleniace sie\n\r", 0 );
			write_to_buffer( d, "po swiecie zlo, albo postacia lubujaca sie w niecnosci i nienawidzaca wszelkich\n\r", 0 );
			write_to_buffer( d, "przejawow dobrej woli. Jesli nie mozesz sie zdecydowac co do charakteru postaci\n\r", 0 );
			write_to_buffer( d, "masz mozliwosc wybrania moralnej neutralnosci.\n\r", 0 );
			if ( ch->class != CLASS_BARD )
			{
				write_to_buffer( d, "Mozesz byc dobry(d), neutralny(n) albo zly(z).\n\r", 0 );
				write_to_buffer( d, "Co wybierasz? (D/N/Z)? ", 0 );
			}
			else
			{
				write_to_buffer( d, "Jako Bard mozesz byc dobry(d) albo neutralny(n).\n\r", 0 );
				write_to_buffer( d, "Co wybierasz? (D/N)? ", 0 );
			}

			d->connected = CON_GET_ALIGNMENT;
			break;

		case CON_GET_MAGE_SPECIALIST:
			DEBUG_INFO( "nanny:CON_GET_MAGE_SPECIALIST" );

			/* jesli cos wpisal, sprawdzamy czy to jakas specializacja */
			if ( argument[ 0 ] != '\0' )
			{
				int x, school, arg, specialist = 0;

				/* jesli wpisal nazwe slownie */
				if ( !is_number( argument ) )
				{
					for ( school = 0; school < MAX_SCHOOL; school++ )
					{
						if ( !str_prefix( argument, school_table[ school ].name ) )
						{
							if ( can_be_specialist( ch, school ) )
								specialist = school_table[ school ].flag;
							break;
						}
					}
				}
				/* a moze wpisal cyferkie */
				else
				{
					arg = atoi( argument );

					if ( arg < 1 || arg > MAX_SCHOOL )
					{
						write_to_buffer( d, "Nie ma takiej specjalizacji!\n\r", 0 );
						break;
					}

					for ( x = 1, school = 0; school < MAX_SCHOOL; school++ )
					{
						if ( can_be_specialist( ch, school ) )
						{
							if ( x == arg )
							{
								specialist = school_table[ school ].flag;
								break;
							}
							x++;
						}
					}
				}

				if ( specialist != 0 )
				{
					ch->pcdata->mage_specialist = school;
					sprintf( buf, "Wybrales specjalizacje: %s\n\r", school_table[ school ].name );
					write_to_buffer( d, buf, 0 );

					/* nekromanci zli */
					if ( school_table[ ch->pcdata->mage_specialist ].flag == Nekromancja )
					{
						ch->alignment = -1000;
						d->connected = CON_FINISH_CREATE;
						break;
					}

				}
				else
				{
					write_to_buffer( d, "Nie ma takiej specjalizacji!\n\r", 0 );
					write_to_buffer( d, "Mozesz wybrac specjalizacje w jednej z ponizszych szkol magii, lub nacisnij [ENTER].\n\r", 0 );

					for ( x = 1, school = 0 ; school < MAX_SCHOOL ; school++ )
					{
						if ( can_be_specialist( ch, school ) )
						{
							sprintf( buf, "%d) %-20s [ %-13s]\n\r",
							         x++,
							         school_table[ school ].specialist_name,
							         school_table[ school ].name );

							write_to_buffer( d, buf, 0 );
						}
					}
					break;
				}
			}


			sprintf( log_buf, "%s@%s nowy gracz.", ch->name, d->host );
			log_string( log_buf );
			wiznet( "Jakis nowy koles!  $N ma na imie.", ch, NULL, WIZ_NEWBIE, 0, 0 );
			wiznet( log_buf, NULL, NULL, WIZ_SITES, 0, get_trust( ch ) );

			write_to_buffer( d, "\n\r", 2 );
			write_to_buffer( d, "Ostatnim etapem tworzenia postaci jest wybor jej charakteru. Mozesz\n\r", 0 );
			write_to_buffer( d, "zadeklarowac czy chcesz byc z natury postacia dobra i zwalczac pleniace sie\n\r", 0 );
			write_to_buffer( d, "po swiecie zlo, albo postacia lubujaca sie w niecnosci i nienawidzaca wszelkich\n\r", 0 );
			write_to_buffer( d, "przejawow dobrej woli. Jesli nie mozesz sie zdecydowac co do charakteru postaci\n\r", 0 );
			write_to_buffer( d, "masz mozliwosc wybrania moralnej neutralnosci.\n\r", 0 );

			write_to_buffer( d, "Mozesz byc dobry(d), neutralny(n) albo zly(z).\n\r", 0 );
			write_to_buffer( d, "Co wybierasz? (D/N/Z)? ", 0 );

			d->connected = CON_GET_ALIGNMENT;
			break;


		case CON_GET_ALIGNMENT:
			DEBUG_INFO( "nanny:CON_GET_ALIGNMENT" );
			switch ( argument[ 0 ] )
			{
			case 'd' : case 'D' : ch->alignment = 1000; break;
			case 'n' : case 'N' : ch->alignment = 0;	break;
			case 'z' : case 'Z' :
					if ( ch->class != CLASS_BARD )
					{
						ch->alignment = -1000; break;
					}
					else
					{
						write_to_buffer( d, "Charakter postaci nierozpoznany.\n\r", 0 );
						write_to_buffer( d, "Jaki charakter wybierasz (D/N)? ", 0 );
						return ;
					}
				default:
					if ( ch->class != CLASS_BARD )
					{
						write_to_buffer( d, "Charakter postaci nierozpoznany.\n\r", 0 );
						write_to_buffer( d, "Jaki charakter wybierasz (D/N/Z)? ", 0 );
						return ;
					}
					else
					{
						write_to_buffer( d, "Charakter postaci nierozpoznany.\n\r", 0 );
						write_to_buffer( d, "Jaki charakter wybierasz (D/N)? ", 0 );
						return ;
					}
			}


		case CON_FINISH_CREATE:
			DEBUG_INFO( "nanny:CON_FINISH_CREATE" );
            /**
             * roll stats
             */
            roll_stats( ch );

            /**
             * set base skills
             */
            set_base_skills ( ch ) ;

			/*zmiany zmiany zmiany*/
			ch->pcdata->perm_hit = ch->max_hit;
			ch->pcdata->perm_move = ch->max_move;
			ch->pcdata->perm_hit_per_level[ 0 ] = ch->pcdata->perm_hit;
			ch->wimpy = ch->max_hit / 2;
			ch->age = number_range ( pc_race_table[ch->real_race].age[ 0 ], pc_race_table[ch->real_race].age[ 1 ]);
			ch->glory = 0;

			//nie ma wpisu o imieniu, flaga
			if ( is_allowed( ch->name ) == NAME_NOTEXIST ||
			     is_allowed( ch->name ) == NAME_NEW )
			{
				EXT_SET_BIT( ch->act, PLR_UNREGISTERED );
				save_file( NAME_NEW );
			}

			do_function( ch, &do_help, "motd" );
			d->connected = CON_MENU;
			menu_show( d->character );
			break;

		case CON_MENU:
			DEBUG_INFO( "nanny:CON_MENU" );
			switch ( argument[ 0 ] )
			{
				case '0':
					ch->pcdata->last_rent = 0;
					DEBUG_INFO( "do_quit" );
					do_function( ch, &do_quit, "" );
					DEBUG_INFO( NULL );
					return ;

				case '1':

					write_to_buffer( d, "\n\r\n\r", 0 );

					if ( is_allowed( ch->name ) == NAME_ALLOWED && EXT_IS_SET( ch->act, PLR_UNREGISTERED ) )
					{
						EXT_REMOVE_BIT( ch->act, PLR_UNREGISTERED );
						write_to_buffer( d , "Twoje imie zostalo zaakceptowane.\n\r", 0 );
					}

					if ( ch->level > LEVEL_HERO) //Brohacz: zmieniam auto akceptacje imienia, by byla mozliwa jedynie w przyadku advance
					{
						if ( is_allowed( ch->name ) == NAME_NOTEXIST ||
						     is_allowed( ch->name ) == NAME_NEW )
						{
							add_name( ch->name, NAME_ALLOWED );
							save_file( NAME_ALLOWED );
						}
					}

					ch->next	= char_list;
					char_list	= ch;
					reset_char( ch );
					d->connected	= CON_PLAYING;


					if ( ch->level == 0 )
					{
						ch->level	= 1;
						ch->pcdata->last_rent = 0;
						ch->exp	= 0;
						ch->hit	= get_max_hp( ch );
						ch->move	= ch->max_move;
						sprintf( buf, "%s %s", ch->sex == SEX_FEMALE ? "kobieta" : "mê¿czyzna", pc_race_table[ GET_REAL_RACE( ch ) ].name );
						set_title( ch, buf );
						char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
						send_to_char( "\n\r", ch );
					}
					else
					{
						if ( ch->pcdata->last_rent > 0 )
                        {
							char_to_room( ch, get_room_index( ch->pcdata->last_rent ) );
                        }
						else
                        {
                            int room_inn_vnum = ROOM_VNUM_TEMPLE;
                            switch ( number_range( 0, 4 ) )
                            {
                                case 0:
                                    room_inn_vnum = 109;
                                    break;
                                case 1:
                                    room_inn_vnum = 239;
                                    break;
                                case 2:
                                    room_inn_vnum = 514;
                                    break;
                                case 3:
                                    room_inn_vnum = 6018;
                                    break;
                                case 4:
                                    room_inn_vnum = 6136;
                                    break;
                                default:
                                    room_inn_vnum = ROOM_VNUM_TEMPLE;
                                    break;
                            }
                            char_to_room( ch, get_room_index( room_inn_vnum ) );
                        }
					}

					if ( !IS_IMMORTAL( ch ) )
                    {
						act( "$n p³aci za pokój.", ch, NULL, NULL, TO_ROOM );
                    }

					// je¿eli ch ¶pi, to ustawiamy, ¿eby przesta³ spaæ
					if ( ch->position == POS_SLEEPING )
					{
						ch->position = POS_STANDING;
					}

					// je¿eli ch nie ¶pi, to niech zrobi sobie look
					if ( ch->position > POS_SLEEPING )
					{
						do_function( ch, &do_look, "auto" );
					}

					//Brohacz: logowanie wejscia do gry z menu
					sprintf( log_buf, "%s wchodzi do gry.", ch->name );
					log_string( log_buf );

					if ( IS_IMMORTAL( ch ) )
						wiznet( "$N wchodzi do gry.", ch, NULL, WIZ_LOGINS, 0, get_trust( ch ) );
					else
					{
                        sprintf( buf, "%s wchodzi do gry. (Class:%s Lev:%-2d)",ch->name, class_table[ ch->class ].who_name, ch->level );
                        wiznet( buf, NULL, NULL, WIZ_LOGINS, 0, get_trust(ch));
					}

					break;

				case '2':
					write_to_buffer( d, "\n\r      Podaj swoje stare has³o:", 0 );
					write_to_buffer( d, echo_off_str, 0 );
					d->connected = CON_MENU_NEWPASSWD;
					break;

				case '3':
					write_to_buffer( d, "\n\r      **************************************************************\n\r", 0 );
					write_to_buffer( d, "      *** Pamietaj! Kasowanie postaci to czynno¶æ nieodwracalna. ***\n\r", 0 );
					write_to_buffer( d, "      **************************************************************\n\r", 0 );
					write_to_buffer( d, "      Podaj swoje has³o:", 0 );
					write_to_buffer( d, echo_off_str, 0 );
					d->connected = CON_MENU_DELETE;
					break;

				case '4':
					write_to_buffer( d, "\n\r      Opcja wy³±czona:\n\r", 0 );
					d->connected = CON_MENU;
					break;

				case '5':
					if ( ch->level < 25 )
					{
						menu_show( d->character );
						break;
					}
					write_to_buffer( d, "\n\r      Wprowad¼ nowy przydomek (maksymalnie 45 znaków) albo ustaw pusty, ¿eby zresetowaæ na domy¶lny.", 0 );
					write_to_buffer( d, "\n\r      Szczegó³owe informacje: http://www.killer.mud.pl/1311/przydomki", 0 );
					write_to_buffer( d, "\n\r      ", 0 );
					d->connected = CON_MENU_CHANGE_TITLE;
					break;

				default :
					menu_show( d->character );
					break;
			}
			break;

		case CON_MENU_NEWPASSWD:
			DEBUG_INFO( "nanny:CON_MENU_NEWPASSWD" );

			if ( argument[ 0 ] == '\0' )
			{
				write_to_buffer( d, echo_on_str, 0 );
				write_to_buffer( d, "\n\r      Nie podano has³a.\n\r", 0 );
				d->connected = CON_MENU;
				menu_show( d->character );
				break;
			}

			if ( str_cmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
			{
				write_to_buffer( d, echo_on_str, 0 );
				write_to_buffer( d, "\n\r      B³êdne has³o.\n\r", 0 );
				d->connected = CON_MENU;
				menu_show( d->character );
				break;
			}

			write_to_buffer( d, "\n\r      Podaj nowe has³o:", 0 );
			d->connected = CON_MENU_NEWPASSWD2;
			break;

		case CON_MENU_NEWPASSWD2:
			DEBUG_INFO( "nanny:CON_MENU_NEWPASSWD2" );

			pwdnew = crypt( argument, ch->name );

			if ( !check_password( d, "      ", pwdnew ) )
			{
				write_to_buffer( d, "\n\r      Podaj nowe has³o:", 0 );
				break ;
			}
			else
			{
				free_string( ch->pcdata->tmp_pwd );
				ch->pcdata->tmp_pwd	= str_dup( pwdnew );

				write_to_buffer( d, echo_off_str, 0 );
				write_to_buffer( d, "      Potwierd¼ nowe has³o: ", 0 );
				d->connected = CON_MENU_CONFIRM_NEWPASSWD;
				break;
			}
			break;

		case CON_MENU_CONFIRM_NEWPASSWD:
			DEBUG_INFO( "nanny:CON_MENU_CONFIRM_NEWPASSWD" );

			if ( strcmp( crypt( argument, ch->pcdata->tmp_pwd ), ch->pcdata->tmp_pwd ) )
			{
				write_to_buffer( d, "      Potwierdzenie nieudane.\n\r", 0 );
				d->connected = CON_MENU;
				menu_show( d->character );
				free_string( ch->pcdata->tmp_pwd );
				break;
			}

			write_to_buffer( d, echo_on_str, 0 );
			write_to_buffer( d, "\n\r      Nowe has³o zosta³o zatwierdzone.\n\r", 0 );

			sprintf( log_buf, "%s@%s changed password.\n\r", ch->name, d->host );
			log_string( log_buf );
			free_string( ch->pcdata->pwd );
			ch->pcdata->pwd = ch->pcdata->tmp_pwd;
			ch->pcdata->tmp_pwd = NULL;
			d->connected = CON_MENU;
			save_char_obj( ch, FALSE, FALSE );
			menu_show( d->character );
			break;

		case CON_MENU_DELETE:
			DEBUG_INFO( "nanny:CON_MENU_DELETE" );
			if ( argument[ 0 ] == '\0' )
			{
				write_to_buffer( d, echo_on_str, 0 );
				write_to_buffer( d, "\n\r      Nie podano has³a.\n\r", 0 );
				d->connected = CON_MENU;
				menu_show( d->character );
				break;
			}
			else if ( !str_cmp( crypt( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
			{
				//do_function( ch, &do_delete, "" );
			    /* start delayed ch delete - by Fuyara */
			    if ( ch->level < 6 )
			    {
				do_function( ch, &do_delete, "" );
			    }
			    else
			    {
				process_todelete( d, ch );
				d->connected = CON_MENU;
				menu_show( d->character );
			    }
			    /* end delayed ch delete */

			    return ;
			}
			else
			{
				write_to_buffer( d, echo_on_str, 0 );
				write_to_buffer( d, "\n\r      Podano b³êdne has³o.\n\r", 0 );
				d->connected = CON_MENU;
				menu_show( d->character );
				break;
			}
			break;

		case CON_MENU_CHANGE_TITLE:
			DEBUG_INFO( "nanny:CON_MENU_CHANGE_TITLE" );

			if ( strlen( argument ) - count_colors( argument, 0 ) == 0 )
			{
				sprintf( buf, "%s %s", ch->sex == SEX_FEMALE ? "kobieta" :"mê¿czyzna", race_table[GET_RACE(ch)].name );
				set_title( ch, buf );
				free_string( ch->pcdata->new_title );
				write_to_buffer( d, "\n\r      Przydomek zosta³ zresetowany.\n\r", 0 );
				save_char_obj( ch, FALSE, FALSE );
				d->connected = CON_MENU;
				menu_show( d->character );

			}


			if ( strlen( argument ) - count_colors( argument, 0 ) > 45 )
			{
				write_to_buffer( d, "\n\r      Przydomek za d³ugi, nie mo¿e mieæ wiêcej ni¿ 45 znaków.\n\r", 0 );
				d->connected = CON_MENU;
				menu_show( d->character );
				break;
			}

			for ( clan = clan_list; clan; clan = clan->next )
			{
				if ( IS_NULLSTR( clan->who_name ) ) continue;

				if ( strstr( argument, clan->who_name ) )
				{
					write_to_buffer( d, "\n\r      Przydomek zawiera nazwê jednego z klanów.\n\r", 0 );
					d->connected = CON_MENU;
					menu_show( d->character );
					break;
				}
			}

			if ( !str_cmp( ch->pcdata->title, argument ) )
			{
				write_to_buffer( d, "\n\r      Nowy przydomek nie ró¿ni siê niczym od starego.\n\r", 0 );
				d->connected = CON_MENU;
				menu_show( d->character );
				break;
			}

			if ( d->connected == CON_MENU_CHANGE_TITLE )
			{
				smash_tilde( argument );
				if ( IS_IMMORTAL( ch ) ) set_title( ch, argument );
				else
				{
					set_new_title( ch, argument );
					write_to_buffer( d, "\n\r      Przydomek zosta³ przypisany, czeka na weryfikacjê Nie¶miertelnych.\n\r", 0 );
				}
				save_char_obj( ch, FALSE, FALSE );
				d->connected = CON_MENU;
				menu_show( d->character );
			}
			break;
	}

	return ;
}
#endif

void stop_idling( CHAR_DATA *ch )
{
	if ( ch == NULL
	     || ch->desc == NULL
	     || ch->desc->connected != CON_PLAYING
	     || ch->was_in_room == NULL
	     || ch->in_room != get_room_index( ROOM_VNUM_LIMBO ) 
	     || get_spirit( ch ) )
		return ;

	ch->timer = 0;
	char_from_room( ch );
	char_to_room( ch, ch->was_in_room );
	ch->was_in_room	= NULL;
	act( "$n powraca z niebytu.", ch, NULL, NULL, TO_ROOM );
	return ;
}

/*
 * Write to one char.
 */
void send_to_char_bw( const char *txt, CHAR_DATA *ch )
{
	if ( txt != NULL && ch->desc != NULL )
		write_to_buffer( ch->desc, txt, strlen( txt ) );
	return ;
}


int colour_back( char type, char *string )
{
	char code[ 20 ];
	char *p = '\0';

	switch ( type )
	{
		default:
			strcpy( code, CLEAR );
			break;
		case 'b':
			strcpy( code, C_BACK_BLUE );
			break;
		case 'd':
			strcpy( code, C_BACK_BLACK );
			break;
		case 'c':
			strcpy( code, C_BACK_CYAN );
			break;
		case 'g':
			strcpy( code, C_BACK_GREEN );
			break;
		case 'm':
			strcpy( code, C_BACK_MAGENTA );
			break;
		case 'r':
			strcpy( code, C_BACK_RED );
			break;
		case 'w':
			strcpy( code, C_BACK_WHITE );
			break;
		case 'y':
			strcpy( code, C_BACK_YELLOW );
			break;
		case 'B':
			strcpy( code, C_BACK_L_BLUE );
			break;
		case 'C':
			strcpy( code, C_BACK_L_CYAN );
			break;
		case 'G':
			strcpy( code, C_BACK_L_GREEN );
			break;
		case 'M':
			strcpy( code, C_BACK_L_MAGENTA );
			break;
		case 'R':
			strcpy( code, C_BACK_L_RED );
			break;
		case 'W':
			strcpy( code, C_BACK_L_WHITE );
			break;
		case 'Y':
			strcpy( code, C_BACK_L_YELLOW );
			break;
		case 'D':
			strcpy( code, C_BACK_L_BLACK );
			break;
		case 'u':
			strcpy( code, C_UNDERLINE );
			break;
		case 'i':
			strcpy( code, C_INVERSE );
			break;
		case 'f':
			strcpy( code, C_FLASHING );
			break;
		case 'l':
			strcpy( code, C_BOLD );
			break;
		case '^':
			sprintf( code, "%c", '^' );
			break;
	}

	p = code;

	while ( *p != '\0' )
	{
		*string = *p++;
		*++string = '\0';
	}

	return ( strlen( code ) );
}


/*
 * Write to one char, new colour version, by Lope.
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
	const char * point;
	char *point2;
	char buf[ MAX_STRING_LENGTH * 4 ];
	int skip = 0;

	buf[ 0 ] = '\0';
	point2 = buf;
	if ( txt && ch->desc )
	{
		if ( EXT_IS_SET( ch->act, PLR_COLOUR ) )
		{
			for ( point = txt ; *point ; point++ )
			{
				if ( *point == '{'
				     && ( IS_NPC( ch )
				          || !IS_IMMORTAL( ch )
				          || IS_SET( ch->wiz_config, WIZCFG_COLOURCONV ) ) )
				{
					point++;
					if ( *point == '\0' )
						break;

					skip = colour( *point, ch, point2 );

					while ( skip-- > 0 )
						++point2;

					continue;
				}

				if ( ( IS_NPC( ch ) || ( !IS_NPC( ch ) && ( IS_SET( ch->wiz_config, WIZCFG_SEXCONV ) || !IS_IMMORTAL( ch ) ) ) ) && *point == '<' )
				{
					point++;

					if ( *point != '&' )
					{
						*point2 = '<';
						*++point2 = '\0';
						point--;
						continue;
					}

					point++;

					if ( *point == '\0' )
						break;

					if ( *point == '>' )
						continue;

					skip = sexconv( ch, point, point2 );

					while ( skip-- > 0 )
						++point2;

					while ( *point != '>' && *point != '\0' )
						++point;

					if ( *point == '\0' )
						break;

					continue;
				}

				if ( *point == '^'
				     && ( IS_NPC( ch )
				          || !IS_IMMORTAL( ch )
				          || IS_SET( ch->wiz_config, WIZCFG_COLOURCONV ) ) )
				{
					point++;

					if ( *point == '\0' )
						break;

					skip = colour_back( *point, point2 );

					while ( skip-- > 0 )
						++point2;

					continue;
				}

				*point2 = *point;
				*++point2 = '\0';
			}

			*point2 = '\0';
			strcat( buf, "\e[0m" );
			write_to_buffer( ch->desc, buf, point2 - buf );
		}
		else
		{
			for ( point = txt ; *point ; point++ )
			{
				if ( *point == '{'
				     && ( IS_NPC( ch )
				          || !IS_IMMORTAL( ch )
				          || IS_SET( ch->wiz_config, WIZCFG_COLOURCONV ) ) )
				{
					point++;

					if ( *point == '\0' )
						break;

					continue;
				}

				if ( ( IS_NPC( ch ) || ( !IS_NPC( ch ) && ( IS_SET( ch->wiz_config, WIZCFG_SEXCONV ) || !IS_IMMORTAL( ch ) ) ) ) && *point == '<' )
				{
					point++;

					if ( *point != '&' )
					{
						*point2 = '<';
						*++point2 = '\0';
						point--;
						continue;
					}

					point++;

					if ( *point == '\0' )
						break;

					if ( *point == '>' )
						continue;

					skip = sexconv( ch, point, point2 );

					while ( skip-- > 0 )
						++point2;

					while ( *point != '>' && *point != '\0' )
						++point;

					if ( *point == '\0' )
						break;

					continue;
				}

				if ( *point == '^'
				     && ( IS_NPC( ch )
				          || !IS_IMMORTAL( ch )
				          || IS_SET( ch->wiz_config, WIZCFG_COLOURCONV ) ) )
				{
					point++;

					if ( *point == '\0' )
						break;

					continue;
				}

				*point2 = *point;
				*++point2 = '\0';
			}

			*point2 = '\0';
			write_to_buffer( ch->desc, buf, point2 - buf );
		}
	}
	return ;
}

void send_to_desc( DESCRIPTOR_DATA *d, const char *txt, bool colour )
{
	const char * point;
	char *point2;
	char buf[ MAX_STRING_LENGTH * 4 ];
	int skip = 0;

	buf[ 0 ] = '\0';
	point2 = buf;
	if ( !txt || !d )
		return ;

	if ( colour )
	{
		for ( point = txt ; *point ; point++ )
		{
			if ( *point == '{' )
			{
				point++;
				if ( *point == '\0' )
					break;

				skip = simple_colour( *point, point2 );

				while ( skip-- > 0 )
					++point2;

				continue;
			}

			if ( *point == '^' )
			{
				point++;

				if ( *point == '\0' )
					break;

				skip = colour_back( *point, point2 );

				while ( skip-- > 0 )
					++point2;

				continue;
			}

			*point2 = *point;
			*++point2 = '\0';
		}

		*point2 = '\0';
		strcat( buf, "\e[0m" );
		write_to_buffer( d, buf, point2 - buf );
	}
	else
	{
		for ( point = txt ; *point ; point++ )
		{
			if ( *point == '{' )
			{
				point++;

				if ( *point == '\0' )
					break;

				continue;
			}

			if ( *point == '^' )
			{
				point++;

				if ( *point == '\0' )
					break;

				continue;
			}

			*point2 = *point;
			*++point2 = '\0';
		}

		*point2 = '\0';
		write_to_buffer( d, buf, point2 - buf );
	}
	return ;
}

void print_desc( DESCRIPTOR_DATA *d, bool colour, char *fmt, ... )
{
	char buf[ MAX_STRING_LENGTH * 2 ];
	va_list args;

	va_start( args, fmt );
	vsprintf( buf, fmt, args );
	va_end( args );

	send_to_desc( d, buf, colour );
}

/*
 * Send a page to one char.
 */
void page_to_char_bw( const char *txt, CHAR_DATA *ch )
{
	if ( txt == NULL || ch->desc == NULL )
		return ;

	if ( ch->lines == 0 )
	{
		send_to_char_bw( txt, ch );
		return ;
	}

	CREATE( ch->desc->showstr_head, char, strlen( txt ) + 1 );
	strcpy( ch->desc->showstr_head, txt );
	ch->desc->showstr_point = ch->desc->showstr_head;
	show_string( ch->desc, "" );
}


/*
 * Page to one char, new colour version, by Lope.
 */
void page_to_char( const char *txt, CHAR_DATA *ch )
{
	const char * point;
	char *point2;
	char buf[ MAX_STRING_LENGTH * 7 ];
	int skip = 0;


	buf[ 0 ] = '\0';
	point2 = buf;
	if ( txt && ch->desc )
	{
		if ( EXT_IS_SET( ch->act, PLR_COLOUR ) )
		{
			for ( point = txt ; *point ; point++ )
			{
				if ( *point == '{'
				     && ( IS_NPC( ch )
				          || !IS_IMMORTAL( ch )
				          || IS_SET( ch->wiz_config, WIZCFG_COLOURCONV ) ) )
				{
					point++;

					if ( *point == '\0' )
						break;

					skip = colour( *point, ch, point2 );

					while ( skip-- > 0 )
						++point2;

					continue;
				}

				if ( ( IS_NPC( ch ) || ( !IS_NPC( ch ) && ( IS_SET( ch->wiz_config, WIZCFG_SEXCONV ) || !IS_IMMORTAL( ch ) ) ) ) && *point == '<' )
				{
					point++;

					if ( *point != '&' )
					{
						*point2 = '<';
						*++point2 = '\0';
						point--;
						continue;
					}

					point++;

					if ( *point == '\0' )
						break;

					if ( *point == '>' )
						continue;

					skip = sexconv( ch, point, point2 );

					while ( skip-- > 0 )
						++point2;

					while ( *point != '>' && *point != '\0' )
						++point;

					if ( *point == '\0' )
						break;

					continue;
				}

				if ( *point == '^'
				     && ( IS_NPC( ch )
				          || !IS_IMMORTAL( ch )
				          || IS_SET( ch->wiz_config, WIZCFG_COLOURCONV ) ) )
				{
					point++;

					if ( *point == '\0' )
						break;

					skip = colour_back( *point, point2 );

					while ( skip-- > 0 )
						++point2;

					continue;
				}

				*point2 = *point;
				*++point2 = '\0';
			}

			*point2 = '\0';
			CREATE( ch->desc->showstr_head, char, strlen( buf ) + 1 );
			strcpy( ch->desc->showstr_head, buf );
			ch->desc->showstr_point = ch->desc->showstr_head;
			show_string( ch->desc, "" );
		}
		else
		{
			for ( point = txt ; *point ; point++ )
			{
				if ( *point == '{' )
				{
					point++;

					if ( *point == '\0' )
						break;

					continue;
				}

				if ( *point == '^' )
				{
					point++;

					if ( *point == '\0' )
						break;

					continue;
				}

				*point2 = *point;
				*++point2 = '\0';
			}

			*point2 = '\0';
			CREATE( ch->desc->showstr_head, char, strlen( buf ) + 1 );
			strcpy( ch->desc->showstr_head, buf );
			ch->desc->showstr_point = ch->desc->showstr_head;
			show_string( ch->desc, "" );
		}
	}
	return ;
}


/* string pager */
void show_string( struct descriptor_data *d, char *input )
{
	char buffer[ 4*MAX_STRING_LENGTH ];
	char buf[ MAX_INPUT_LENGTH ];
	register char *scan, *chk;
	int lines = 0, toggle = 1;
	int show_lines;

	one_argument( input, buf );
	if ( buf[ 0 ] != '\0' )
	{
		if ( d->showstr_head )
		{
			DISPOSE( d->showstr_head );
			d->showstr_head = 0;
		}
		d->showstr_point = 0;
		return ;
	}

	if ( d->character )
		show_lines = d->character->lines;
	else
		show_lines = 0;

	for ( scan = buffer; ; scan++, d->showstr_point++ )
	{
		if ( ( ( *scan = *d->showstr_point ) == '\n' || *scan == '\r' )
		     && ( toggle = -toggle ) < 0 )
			lines++;

		else if ( !*scan || ( show_lines > 0 && lines >= show_lines ) )
		{
			*scan = '\0';
			write_to_buffer( d, buffer, strlen( buffer ) );
			for ( chk = d->showstr_point; isspace( *chk ); chk++ );
			{
				if ( !*chk )
				{
					if ( d->showstr_head )
					{
						DISPOSE( d->showstr_head );
						d->showstr_head = 0;
					}
					d->showstr_point = 0;
				}
			}
			return ;
		}
	}
	return ;
}


/* quick sex fixer */
void fix_sex( CHAR_DATA *ch )
{
	if ( ch->sex < 0 || ch->sex > 2 )
		ch->sex = IS_NPC( ch ) ? 0 : ch->pcdata->true_sex;
}

char * get_zaimek( CHAR_DATA *ch, CHAR_DATA *vch, char format )
{
	static char * const he_she [] =
	    { "to", "on", "ona"
	    };
	static char * const him_her [] =
	    { "je", "go", "j±"
	    };
	static char * const his_her [] =
	    { "tego", "jego", "jej"
	    };
	static char * const niego_nia [] =
	    { "nie", "niego", "ni±",
	    };
	static char * const mu_jej [] =
	    { "mu", "mu", "jej"
	    };
	static char * const nim_niej [] =
	    { "tym", "nim", "niej"
	    };
	static char * const e_a_y [] =
	    { "e", "y", "a"
	    };
	static char * const lo_la_l [] =
	    { "³o", "³", "³a"
	    };
	static char * const nim_nia [] =
	    { "tym", "nim", "ni±"
	    };
	static char buf[ MAX_STRING_LENGTH ];

	switch ( format )
	{
		case 'e': strcpy( buf, he_she [ URANGE( 0, ch ->sex, 2 ) ] );	break;
		case 'E': strcpy( buf, he_she [ URANGE( 0, vch ->sex, 2 ) ] );	break;
		case 'm': strcpy( buf, him_her [ URANGE( 0, ch ->sex, 2 ) ] );	break;
		case 'M': strcpy( buf, him_her [ URANGE( 0, vch ->sex, 2 ) ] );	break;
		case 's': strcpy( buf, his_her [ URANGE( 0, ch ->sex, 2 ) ] );	break;
		case 'S': strcpy( buf, his_her [ URANGE( 0, vch ->sex, 2 ) ] );	break;
		case 'a': strcpy( buf, niego_nia[ URANGE( 0, ch ->sex, 2 ) ] );	break;
		case 'A': strcpy( buf, niego_nia[ URANGE( 0, vch ->sex, 2 ) ] );	break;
		case 'o': strcpy( buf, mu_jej[ URANGE( 0, ch ->sex, 2 ) ] );	break;
		case 'O': strcpy( buf, mu_jej[ URANGE( 0, vch ->sex, 2 ) ] );	break;
		case 'q': strcpy( buf, nim_niej[ URANGE( 0, ch ->sex, 2 ) ] );	break;
		case 'Q': strcpy( buf, nim_niej[ URANGE( 0, vch ->sex, 2 ) ] );	break;
		case 'r': strcpy( buf, e_a_y[ URANGE( 0, ch ->sex, 2 ) ] );	break;
		case 'R': strcpy( buf, e_a_y[ URANGE( 0, vch ->sex, 2 ) ] );	break;
		case 'u': strcpy( buf, lo_la_l[ URANGE( 0, ch ->sex, 2 ) ] );	break;
		case 'U': strcpy( buf, lo_la_l[ URANGE( 0, vch ->sex, 2 ) ] );	break;
		case 'y': strcpy( buf, nim_nia[ URANGE( 0, ch ->sex, 2 ) ] );	break;
		case 'Y': strcpy( buf, nim_nia[ URANGE( 0, vch ->sex, 2 ) ] );	break;
		default:
			buf[ 0 ] = '\0';
			break;
	}

	return buf;
}

void act_new( const char *format, CHAR_DATA *ch, void *arg1,
              void *arg2, int type, int min_pos )
{
	CHAR_DATA * to;
	CHAR_DATA *vch = ( CHAR_DATA * ) arg2;
	CHAR_DATA * next_in_room = NULL;
	OBJ_DATA *obj1 = ( OBJ_DATA * ) arg1;
	OBJ_DATA *obj2 = ( OBJ_DATA * ) arg2;
	char *ret_buf = ( char * ) arg1;
	const char *str;
	char *i = NULL;
	char color = '\0';
	char *point;
	char *pbuff;
	char buffer[ MAX_STRING_LENGTH * 2 ];
	char buf[ MAX_STRING_LENGTH * 2 ];
	char fname[ MAX_INPUT_LENGTH ];
	bool	fColour = FALSE;
	bool	roomenabled = TRUE;
	int minds = 0;
	SPIRIT_DATA * duch = spirits;
	/*
	 * Discard null and zero-length messages.
	 */

	save_debug_info( "comm.c => act_new", NULL, NULL, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE  );

	if ( !format || !*format )
		return ;

	/* discard null rooms and chars */
	if ( !ch || !ch->in_room )
		return ;

	to = ch->in_room->people;


	if ( type == TO_VICT )
	{
		if ( !vch )
		{
			bug( "Act: null vch with TO_VICT.", 0 );
			return ;
		}

		if ( !vch->in_room )
			return ;

		to = vch->in_room->people;
	}

	for ( ; to ; to = next_in_room )
	{
		if ( minds > 0 )
		{
			next_in_room = to->next_mind_in_room;
			minds = 2;
			min_pos = 0;
		}
		else
			next_in_room = to->next_in_room;

		if ( minds == 0 && !next_in_room && to->in_room && to->in_room->minds )
		{
			next_in_room = to->in_room->minds;
			minds = 1;
		}

		if( duch && !next_in_room )
		{
			OBJ_DATA * in_obj;
			int room;
       	 	        for ( in_obj = duch->corpse; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
		                       ;
       	         	if ( in_obj && in_obj->carried_by != NULL  && in_obj->carried_by->in_room != NULL )
				room = in_obj->carried_by->in_room->vnum;
			else if ( in_obj && in_obj->in_room )
				room = in_obj->in_room->vnum;	

			if( ch->in_room->vnum == room ) 
	                        next_in_room = duch->ch;
                        duch = duch->next;
		}

		if ( ( !IS_NPC( to ) && !to->desc )
		     || ( IS_NPC( to ) && !HAS_TRIGGER( to, TRIG_ACT ) && !to->desc )
		     || to->position < min_pos )
			continue;

		if ( !IS_NPC( to ) && to->pcdata->mind_in && minds < 2 )
			continue;
		if ( !IS_NPC( to ) && to->pcdata->mind_in && ( type == TO_CHAR || type == TO_VICT ) )
			continue;
		if ( get_spirit( to ) && ( type == TO_CHAR || type == TO_VICT ) )
			continue;
		if ( ( type == TO_CHAR ) && to != ch )
			continue;
		if ( type == TO_VICT && ( to != vch || to == ch ) )
			continue;
		if ( type == TO_ROOM && to == ch )
			continue;
		if ( type == TO_NOTVICT && ( to == ch || to == vch ) )
			continue;

		point = buf;
		str	= format;

		while ( *str != '\0' )
		{
			if ( *str == '{' && *( str + 1 ) != '\0' && *( str + 1 ) != '{' )
				color = *( str + 1 );

			if ( *str != '$' )
			{
				*point++ = *str++;
				continue;
			}

			fColour = TRUE;
			++str;
			i = " <@@@> ";
			if ( !arg2 && *str >= 'A' && *str <= 'Z' )
			{
				bug( "Act: missing arg2 for code '%c'.", *str );
				i = " <@@@> ";
			}
			else
			{
				switch ( *str )
				{
					default:
						i = get_zaimek( ch, vch, *str );
						if ( IS_NULLSTR( i ) )
						{
							bug( "Act: bad code '%c'.", *str );
							i = " <@@@> ";
						}
						break;
					case 't': i = ( char * ) arg1;	break;
					case 'T': i = ( char * ) arg2;	break;
					case 'n': i = PERS( ch, to );	break;
					case 'N': i = PERS( vch, to );	break;

						/*start odm char*/
					case 'z': //dopelniacz
						if ( can_see( to, ch ) )
							i = ch->name2;
						else
							i = "kogo¶";
						break;
					case 'Z':
						if ( can_see( to, vch ) )
							i = vch->name2;
						else
							i = "kogo¶";
						break;

					case 'x': //celownik
						if ( can_see( to, ch ) )
							i = ch->name3;
						else
							i = "komu¶";
						break;

					case 'X':
						if ( can_see( to, vch ) )
							i = vch->name3;
						else
							i = "komu¶";
						break;

					case 'c': //biernik
						if ( can_see( to, ch ) )
							i = ch->name4;
						else
							i = "kogo¶";
						break;

					case 'C':
						if ( can_see( to, vch ) )
							i = vch->name4;
						else
							i = "kogo¶";
						break;

					case 'v': //narzednik
						if ( can_see( to, ch ) )
							i = ch->name5;
						else
							i = "kim¶";
						break;

					case 'V':
						if ( can_see( to, vch ) )
							i = vch->name5;
						else
							i = "kim¶";
						break;

					case 'b': //miejscownik
						if ( can_see( to, ch ) )
							i = ch->name6;
						else
							i = "kim¶";
						break;

					case 'B':
						if ( can_see( to, vch ) )
							i = vch->name6;
						else
							i = "kim¶";
						break;
						/*end odm char*/


						/*odm obiektu*/
					case 'f':
						if ( can_see_obj( to, obj1 ) )
							i = obj1->name2;
						else
							i = "czego¶";
						break;

					case 'F':
						if ( can_see_obj( to, obj2 ) )
							i = obj2->name2;
						else
							i = "czego¶";
						break;

					case 'g':
						if ( can_see_obj( to, obj1 ) )
							i = obj1->name3;
						else
							i = "czemu¶";
						break;

					case 'G':
						if ( can_see_obj( to, obj2 ) )
							i = obj2->name3;
						else
							i = "czemu¶";
						break;

					case 'h':
						if ( can_see_obj( to, obj1 ) )
							i = obj1->name4;
						else
							i = "co¶";
						break;

					case 'H':
						if ( can_see_obj( to, obj2 ) )
							i = obj2->name4;
						else
							i = "co¶";
						break;

					case 'j':
						if ( can_see_obj( to, obj1 ) )
							i = obj1->name5;
						else
							i = "czym¶";
						break;

					case 'J':
						if ( can_see_obj( to, obj2 ) )
							i = obj2->name5;
						else
							i = "czym¶";
						break;

					case 'k':
						if ( can_see_obj( to, obj1 ) )
							i = obj1->name6;
						else
							i = "czym¶";
						break;

					case 'K':
						if ( can_see_obj( to, obj2 ) )
							i = obj2->name6;
						else
							i = "czym¶";
						break;


						/*end odm obiektu*/

					case 'p':
						i = can_see_obj( to, obj1 )
						    ? obj1->short_descr
						    : "co¶";
						break;

					case 'P':
						i = can_see_obj( to, obj2 )
						    ? obj2->short_descr
						    : "co¶";
						break;

					case 'd':
						if ( arg2 == NULL || ( ( char * ) arg2 ) [ 0 ] == '\0' )
						{
							i = "drzwi";
						}
						else
						{
							one_argument( ( char * ) arg2, fname );
							i = fname;
						}
						break;
				}
			}

			++str;
			while ( ( *point = *i ) != '\0' )
				++point, ++i;

			if ( color != '\0' )
			{
				*point++ = '{';
				*point++ = color;
			}
		}

		*point++ = '\n';
		*point++ = '\r';
		*point = '\0';

		sprintf( buf, "%s", capitalize( buf ) );

		if ( type == TO_BUF )
		{
			strcpy( ret_buf, buf );
			return ;
		}

		if ( to->desc != NULL )
		{
			pbuff	= buffer;
			colourconv( pbuff, buf, to );
			write_to_buffer( to->desc, buffer, 0 );
		}
		else if ( MOBtrigger )
			mp_act_trigger( buf, to, ch, arg1, arg2, &TRIG_ACT );

		if ( OBJtrigger && to->desc )
		{
			op_act_trigger( buf, to, roomenabled );
			roomenabled = FALSE;
		}

	}

	if ( !IS_NPC( ch ) && HAS_RTRIGGER( ch->in_room, TRIG_ACT ) && type == TO_ROOM )
		rp_act_trigger( buf, ch, &TRIG_ACT );

	//obsluga echo_to w roomie
	if ( ch->in_room && ch->in_room->echo_to.room && ( type == TO_ROOM || type == TO_NOTVICT ) )
	{
		ROOM_INDEX_DATA * prev_room;

		prev_room = ch->in_room;
		ch->in_room = ch->in_room->echo_to.room;
		act_new( format, ch, arg1, arg2, type, min_pos );
		ch->in_room = prev_room;
	}

	return ;
}

int sexconv( CHAR_DATA *ch, const char *str_src, char *str_dest )
{
	char	txtout[ MAX_INPUT_LENGTH ];
	char	*p = '\0';
	const char	*p2 = '\0';
	int	sex = ch->sex;
	int	actsex = -1;
	int	count = 0;

	if ( IS_NPC( ch ) )
		return ( 0 );

	p = txtout;
	*p = '\0';

	for ( p2 = str_src; *p2 ; p2++ )
	{
		if ( *p2 == '\0' )
		{
			*p = '\0';
			break;
		}

		if ( *p2 == '/' || *p2 == '>' )
		{
			count++;
			switch ( count )
			{
				case 1: actsex = 1; break;
				case 2: actsex = 2; break;
				case 3: actsex = 0; break;
				default: actsex = 0; break;
			}

			if ( actsex == sex || *p2 == '>' )
				break;
			else
			{
				p = txtout;
				*p = '\0';
			}
			++p2;
		}

		*p = *p2;
		*++p = '\0';
	}

	p = txtout;
	while ( *p != '\0' )
	{
		*str_dest = *p++;
		*++str_dest = '\0';
	}

	return ( strlen( txtout ) );
}

int colour( char type, CHAR_DATA *ch, char *string )
{
	PC_DATA	* col;
	char	code[ 20 ];
	char	*p = '\0';

	if ( IS_NPC( ch ) )
		return ( 0 );

	col = ch->pcdata;

	switch ( type )
	{
		default:
			strcpy( code, CLEAR );
			break;
		case 'x':
			strcpy( code, CLEAR );
			break;
		case 'p':
			if ( col->prompt[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c", col->prompt[ 0 ], col->prompt[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->prompt[ 0 ], col->prompt[ 1 ] );
			break;
		case 's':
			if ( col->room_title[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->room_title[ 0 ], col->room_title[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->room_title[ 0 ], col->room_title[ 1 ] );
			break;
		case 'S':
			if ( col->room_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->room_text[ 0 ], col->room_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->room_text[ 0 ], col->room_text[ 1 ] );
			break;
		case 'd':
			if ( col->gossip[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->gossip[ 0 ], col->gossip[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->gossip[ 0 ], col->gossip[ 1 ] );
			break;
		case '9':
			if ( col->gossip_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->gossip_text[ 0 ], col->gossip_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->gossip_text[ 0 ], col->gossip_text[ 1 ] );
			break;
		case 'Z':
			if ( col->wiznet[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->wiznet[ 0 ], col->wiznet[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->wiznet[ 0 ], col->wiznet[ 1 ] );
			break;
		case 'o':
			if ( col->room_exits[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->room_exits[ 0 ], col->room_exits[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->room_exits[ 0 ], col->room_exits[ 1 ] );
			break;
		case 'O':
			if ( col->room_things[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->room_things[ 0 ], col->room_things[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->room_things[ 0 ], col->room_things[ 1 ] );
			break;
		case 'i':
			if ( col->immtalk_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->immtalk_text[ 0 ], col->immtalk_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm",
				         col->immtalk_text[ 0 ], col->immtalk_text[ 1 ] );
			break;
		case 'I':
			if ( col->immtalk_type[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->immtalk_type[ 0 ], col->immtalk_type[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm",
				         col->immtalk_type[ 0 ], col->immtalk_type[ 1 ] );
			break;
		case '2':
			if ( col->fight_yhit[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->fight_yhit[ 0 ], col->fight_yhit[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->fight_yhit[ 0 ], col->fight_yhit[ 1 ] );
			break;
		case '3':
			if ( col->fight_ohit[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->fight_ohit[ 0 ], col->fight_ohit[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->fight_ohit[ 0 ], col->fight_ohit[ 1 ] );
			break;
		case '4':
			if ( col->fight_thit[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->fight_thit[ 0 ], col->fight_thit[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->fight_thit[ 0 ], col->fight_thit[ 1 ] );
			break;
		case '5':
			if ( col->fight_skill[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->fight_skill[ 0 ], col->fight_skill[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->fight_skill[ 0 ], col->fight_skill[ 1 ] );
			break;
		case 'v':
			if ( col->fight_trick[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
					 col->fight_trick[ 0 ], col->fight_trick[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->fight_trick[ 0 ], col->fight_trick[ 1 ] );
			break;
		case 'V':
			if ( col->fight_spell[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
					 col->fight_spell[ 0 ], col->fight_spell[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->fight_spell[ 0 ], col->fight_spell[ 1 ] );
			break;
		case '1':
			if ( col->fight_death[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->fight_death[ 0 ], col->fight_death[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->fight_death[ 0 ], col->fight_death[ 1 ] );
			break;
		case '6':
			if ( col->say[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c", col->say[ 0 ], col->say[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->say[ 0 ], col->say[ 1 ] );
			break;
		case '7':
			if ( col->say_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->say_text[ 0 ], col->say_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->say_text[ 0 ], col->say_text[ 1 ] );
			break;
		case 'k':
			if ( col->tell[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c", col->tell[ 0 ], col->tell[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->tell[ 0 ], col->tell[ 1 ] );
			break;
		case 'K':
			if ( col->tell_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->tell_text[ 0 ], col->tell_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->tell_text[ 0 ], col->tell_text[ 1 ] );
			break;
		case 'l':
			if ( col->reply[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c", col->reply[ 0 ], col->reply[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->reply[ 0 ], col->reply[ 1 ] );
			break;
		case 'L':
			if ( col->reply_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->reply_text[ 0 ], col->reply_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->reply_text[ 0 ], col->reply_text[ 1 ] );
			break;
		case 'N':
			if ( col->gtell_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->gtell_text[ 0 ], col->gtell_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->gtell_text[ 0 ], col->gtell_text[ 1 ] );
			break;
		case 'n':
			if ( col->gtell_type[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->gtell_type[ 0 ], col->gtell_type[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->gtell_type[ 0 ], col->gtell_type[ 1 ] );
			break;
		case 'a':
			if ( col->auction[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->auction[ 0 ], col->auction[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->auction[ 0 ], col->auction[ 1 ] );
			break;
		case 'A':
			if ( col->auction_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->auction_text[ 0 ], col->auction_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->auction_text[ 0 ], col->auction_text[ 1 ] );
			break;
		case 'q':
			if ( col->question[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->question[ 0 ], col->question[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->question[ 0 ], col->question[ 1 ] );
			break;
		case 'Q':
			if ( col->question_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->question_text[ 0 ], col->question_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm",
				         col->question_text[ 0 ], col->question_text[ 1 ] );
			break;
		case 'f':
			if ( col->answer[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->answer[ 0 ], col->answer[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->answer[ 0 ], col->answer[ 1 ] );
			break;
		case 'F':
			if ( col->answer_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->answer_text[ 0 ], col->answer_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->answer_text[ 0 ], col->answer_text[ 1 ] );
			break;
		case 'e':
			if ( col->music[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->music[ 0 ], col->music[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->music[ 0 ], col->music[ 1 ] );
			break;
		case 'E':
			if ( col->music_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->music_text[ 0 ], col->music_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->music_text[ 0 ], col->music_text[ 1 ] );
			break;
		case 'h':
			if ( col->quote[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->quote[ 0 ], col->quote[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->quote[ 0 ], col->quote[ 1 ] );
			break;
		case 'H':
			if ( col->quote_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c",
				         col->quote_text[ 0 ], col->quote_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->quote_text[ 0 ], col->quote_text[ 1 ] );
			break;
		case 'j':
			if ( col->info[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c", col->info[ 0 ], col->info[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->info[ 0 ], col->info[ 1 ] );
			break;
		case 't':
			if ( col->shout[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c", col->shout[ 0 ], col->shout[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->shout[ 0 ], col->shout[ 1 ] );
			break;
		case 'T':
			if ( col->shout_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c", col->shout_text[ 0 ], col->shout_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->shout_text[ 0 ], col->shout_text[ 1 ] );
			break;
		case 'u':
			if ( col->yell[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c", col->yell[ 0 ], col->yell[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->yell[ 0 ], col->yell[ 1 ] );
			break;
		case 'U':
			if ( col->yell_text[ 2 ] )
				sprintf( code, "\e[%d;3%dm%c", col->yell_text[ 0 ], col->yell_text[ 1 ], '\a' );
			else
				sprintf( code, "\e[%d;3%dm", col->yell_text[ 0 ], col->yell_text[ 1 ] );
			break;
		case 'b':
			strcpy( code, C_BLUE );
			break;
		case 'c':
			strcpy( code, C_CYAN );
			break;
		case 'g':
			strcpy( code, C_GREEN );
			break;
		case 'm':
			strcpy( code, C_MAGENTA );
			break;
		case 'r':
			strcpy( code, C_RED );
			break;
		case 'w':
			strcpy( code, C_WHITE );
			break;
		case 'y':
			strcpy( code, C_YELLOW );
			break;
		case 'B':
			strcpy( code, C_B_BLUE );
			break;
		case 'C':
			strcpy( code, C_B_CYAN );
			break;
		case 'G':
			strcpy( code, C_B_GREEN );
			break;
		case 'M':
			strcpy( code, C_B_MAGENTA );
			break;
		case 'R':
			strcpy( code, C_B_RED );
			break;
		case 'W':
			strcpy( code, C_B_WHITE );
			break;
		case 'Y':
			strcpy( code, C_B_YELLOW );
			break;
		case 'D':
			strcpy( code, C_D_GREY );
			break;
		case '*':
			sprintf( code, "%c", '\a' );
			break;
		case '/':
			strcpy( code, "\n\r" );
			break;
		case '-':
			sprintf( code, "%c", '~' );
			break;
		case '{':
			sprintf( code, "%c", '{' );
			break;
	}

	p = code;
	while ( *p != '\0' )
	{
		*string = *p++;
		*++string = '\0';
	}

	return ( strlen( code ) );
}

int simple_colour( char type, char *string )
{
	char	code[ 20 ];
	char	*p = '\0';

	switch ( type )
	{
		default:
			strcpy( code, CLEAR );
			break;
		case 'x':
			strcpy( code, CLEAR );
			break;
		case 'b':
			strcpy( code, C_BLUE );
			break;
		case 'c':
			strcpy( code, C_CYAN );
			break;
		case 'g':
			strcpy( code, C_GREEN );
			break;
		case 'm':
			strcpy( code, C_MAGENTA );
			break;
		case 'r':
			strcpy( code, C_RED );
			break;
		case 'w':
			strcpy( code, C_WHITE );
			break;
		case 'y':
			strcpy( code, C_YELLOW );
			break;
		case 'B':
			strcpy( code, C_B_BLUE );
			break;
		case 'C':
			strcpy( code, C_B_CYAN );
			break;
		case 'G':
			strcpy( code, C_B_GREEN );
			break;
		case 'M':
			strcpy( code, C_B_MAGENTA );
			break;
		case 'R':
			strcpy( code, C_B_RED );
			break;
		case 'W':
			strcpy( code, C_B_WHITE );
			break;
		case 'Y':
			strcpy( code, C_B_YELLOW );
			break;
		case 'D':
			strcpy( code, C_D_GREY );
			break;
		case '*':
			sprintf( code, "%c", '\a' );
			break;
		case '/':
			strcpy( code, "\n\r" );
			break;
		case '-':
			sprintf( code, "%c", '~' );
			break;
		case '{':
			sprintf( code, "%c", '{' );
			break;
	}

	p = code;
	while ( *p != '\0' )
	{
		*string = *p++;
		*++string = '\0';
	}

	return ( strlen( code ) );
}

void colourconv( char *buffer, const char *txt, CHAR_DATA *ch )
{
	const char * point;
	int skip = 0;

	if ( ch->desc && txt )
	{
		if ( EXT_IS_SET( ch->act, PLR_COLOUR ) )
		{
			for ( point = txt ; *point ; point++ )
			{
				if ( *point == '{'
				     && ( IS_NPC( ch )
				          || !IS_IMMORTAL( ch )
				          || IS_SET( ch->wiz_config, WIZCFG_COLOURCONV ) ) )
				{
					point++;

					if ( *point == '\0' )
						break;

					skip = colour( *point, ch, buffer );

					while ( skip-- > 0 )
						++buffer;

					continue;
				}

				if ( ( IS_NPC( ch ) || ( !IS_NPC( ch ) && ( IS_SET( ch->wiz_config, WIZCFG_SEXCONV ) || !IS_IMMORTAL( ch ) ) ) ) && *point == '<' )
				{
					point++;

					if ( *point != '&' )
					{
						*buffer = '<';
						*++buffer = '\0';
						point--;
						continue;
					}

					point++;

					if ( *point == '\0' )
						break;

					if ( *point == '>' )
						continue;

					skip = sexconv( ch, point, buffer );

					while ( skip-- > 0 )
						++buffer;

					while ( *point != '>' && *point != '\0' )
						++point;

					if ( *point == '\0' )
						break;

					continue;
				}

				if ( *point == '^'
				     && ( IS_NPC( ch )
				          || !IS_IMMORTAL( ch )
				          || IS_SET( ch->wiz_config, WIZCFG_COLOURCONV ) ) )
				{
					point++;

					if ( *point == '\0' )
						break;

					skip = colour_back( *point, buffer );

					while ( skip-- > 0 )
						++buffer;

					continue;
				}

				*buffer = *point;
				*++buffer = '\0';
			}
			*buffer = '\0';
		}
		else
		{
			for ( point = txt ; *point ; point++ )
			{
				if ( *point == '{'
				     && ( IS_NPC( ch )
				          || !IS_IMMORTAL( ch )
				          || IS_SET( ch->wiz_config, WIZCFG_COLOURCONV ) ) )
				{
					point++;

					if ( *point == '\0' )
						break;

					continue;
				}

				if ( ( IS_NPC( ch ) || ( !IS_NPC( ch ) && ( IS_SET( ch->wiz_config, WIZCFG_SEXCONV ) || !IS_IMMORTAL( ch ) ) ) ) && *point == '<' )
				{
					point++;

					if ( *point != '&' )
					{
						*buffer = '<';
						*++buffer = '\0';
						point--;
						continue;
					}

					point++;

					if ( *point == '\0' )
						break;

					if ( *point == '>' )
						continue;

					skip = sexconv( ch, point, buffer );

					while ( skip-- > 0 )
						++buffer;

					while ( *point != '>' && *point != '\0' )
						++point;

					if ( *point == '\0' )
						break;

					continue;
				}

				if ( *point == '^'
				     && ( IS_NPC( ch )
				          || !IS_IMMORTAL( ch )
				          || IS_SET( ch->wiz_config, WIZCFG_COLOURCONV ) ) )
				{

					point++;

					if ( *point == '\0' )
						break;

					continue;
				}

				*buffer = *point;
				*++buffer = '\0';
			}
			*buffer = '\0';
		}
	}
	return ;
}



/* source: EOD, by John Booth <?> */

void printf_to_char ( CHAR_DATA *ch, char *fmt, ... )
{
	char buf [ MAX_STRING_LENGTH ];
	va_list args;
	va_start ( args, fmt );
	vsprintf ( buf, fmt, args );
	va_end ( args );

	send_to_char ( buf, ch );
}

void bugf ( char * fmt, ... )
{
	char buf [ 2 * MSL ];
	va_list args;
	va_start ( args, fmt );
	vsprintf ( buf, fmt, args );
	va_end ( args );

	bug ( buf, 0 );
}



void logformat ( char * fmt, ... )
{
	char buf [ 2 * MSL ];
	va_list args;
	va_start ( args, fmt );
	vsprintf ( buf, fmt, args );
	va_end ( args );

	log_string ( buf );
}

#ifdef NEW_NANNY
bool new_class_ok( NEW_CHAR_DATA * new_ch, int class )
{
	int class_ok_tab[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };

	if ( pc_race_table[ new_ch->race ].class_flag & class_ok_tab[ class ] )
		return TRUE;

	return FALSE;
}
#endif

bool class_ok( CHAR_DATA *ch , int class )
{
	int class_ok_tab[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };

	if ( pc_race_table[ GET_REAL_RACE( ch ) ].class_flag & class_ok_tab[ class ] )
		return TRUE;

	return FALSE;
}

void menu_show( CHAR_DATA *ch )
{
    if ( ch != NULL )
    {
        send_to_char( "\n\r\n\r\e[1;31m      ##  ##  ##  ##    ##    ##### #####\n\r", ch );
        send_to_char( "      ## ##   ##  ##    ##    ##    ##  ##\n\r", ch );
        send_to_char( "      ####    ##  ##    ##    ###   #####\n\r", ch );
        send_to_char( "      ## ##   ##  ##    ##    ##    ##  ##\n\r", ch );
        send_to_char( "      ##  ##  ##  ##### ##### ##### ##  ##\n\r\n\r", ch );
        send_to_char( "\e[1;36m      ************************************\n\r", ch );
        send_to_char( "      *                                  *\n\r", ch );
        send_to_char( "      *   1)  Wejd¼ do gry               *\n\r", ch );
        send_to_char( "      *   2)  Zmieñ has³o                *\n\r", ch );
        //send_to_char( "      *   3)  Skasuj postaæ              *\n\r", ch );
        todelete_menu_mod( ch ); // delayed ch delete - by Fuyara
        send_to_char( "      *   4)  Zmieñ opis postaci         *\n\r", ch );
        if ( ch->level >= 25 )
        {
            send_to_char( "      *   5)  Zmieñ przydomek postaci    *\n\r", ch );
        }
        send_to_char( "      *   0)  Wyjd¼ z gry                *\n\r", ch );
        send_to_char( "      *                                  *\n\r", ch );
        send_to_char( "      ************************************\n\r\n\r", ch );
        send_to_char( "      -> Twój wybór: \e[0m", ch );
    }
}

void init_info_socket( int control )
{
	INFO_DESCRIPTOR_DATA * id;
	struct sockaddr_in sock;
	int desc;
	unsigned int size;

	size = sizeof( sock );

	getsockname( control, ( struct sockaddr * ) & sock, &size );

	if ( ( desc = accept( control, ( struct sockaddr * ) & sock, &size ) ) < 0 )
	{
		perror( "New_descriptor: accept" );
		return ;
	}

	if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
	{
		perror( "New_descriptor: fcntl: FNDELAY" );
		return ;
	}

	FD_SET( desc, &fSet );

	id = new_info_descriptor();
	id->descriptor = desc;
	id->next = info_descriptor_list;
	info_descriptor_list = id;

	return ;
}

bool read_from_info_descriptor( INFO_DESCRIPTOR_DATA *id )
{
	int iStart;


	/* Check for overflow. */
	iStart = strlen( id->input );

	for ( ; ; )
	{
		int nRead;

		nRead = read( id->descriptor, id->input + iStart,
		              sizeof( id->input ) - 10 - iStart );
		if ( nRead > 0 )
		{
			iStart += nRead;
			if ( id->input[ iStart - 1 ] == '\n' || id->input[ iStart - 1 ] == '\r' )
				break;
		}
		else if ( nRead == 0 )
		{
			log_string( "EOF encountered on read (lost link)." );
			return FALSE;
		}
		else if ( errno == EWOULDBLOCK )
			break;
		else
		{
			perror( "Read_from_descriptor" );
			return FALSE;
		}
	}

	id->input[ iStart ] = '\0';
	return TRUE;
}

bool write_to_info_descriptor( INFO_DESCRIPTOR_DATA * id, char * txt )
{
	int iStart = 0;
	int nWrite = 0;
	int nBlock;
	int length;
	int desc;

	desc = id->descriptor;
	length = strlen( txt );

	for ( iStart = 0; iStart < length; iStart += nWrite )
	{
		nBlock = UMIN ( length - iStart, 4096 );
		if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
		{
			perror( "Write_to_info_descriptor" );
			return FALSE;
		}
	}
	return TRUE;
}

void strip_enter( char * string )
{
	char * point = string;

	while ( *point )
	{
		if ( *point == '\n' || *point == '\r' )
		{
			*point = '\0';
			return ;
		}
		point++;
	}
}

bool process_info_command( INFO_DESCRIPTOR_DATA *id )
{
	DESCRIPTOR_DATA * d;
	CHAR_DATA *victim;
	CHAR_DATA *wch;
	EXTRA_DESCR_DATA *ed;
	OBJ_DATA *obj;
	HELP_DATA *pHelp;
	char buf[ 2 * MAX_STRING_LENGTH ];
	char command[ MAX_INPUT_LENGTH ];
	char *line;
	int count = 0;
	bool found = FALSE;
	bool com_found = FALSE;

	buf[ 0 ] = '\0';

	if ( id->input[ 0 ] == '\0' )
		return FALSE;


	line = id->input;
	strip_enter( line );
	line = one_argument( line, command );

	switch ( NOPOL( command[ 0 ] ) )
	{
		case 'C':
			if ( !str_cmp( "commands", command ) )
			{
				int cmd;
				char* min = "zz";
				char* actual = "aa";

				com_found = TRUE;

				//sortowanie
				while ( str_cmp( min, "zzz" ) )
				{
					min = "zzz";
					for ( cmd = 0; cmd_table[ cmd ].name[ 0 ] != '\0'; cmd++ )
					{
						if ( cmd_table[ cmd ].level < LEVEL_HERO
						     && !IS_SET( cmd_table[ cmd ].options, OPT_HIDEHELP ) )
						{
							if ( strcmp( cmd_table[ cmd ].name, min ) <= 0
							     && strcmp( cmd_table[ cmd ].name, actual ) > 0 )
								min = cmd_table[ cmd ].name;
						}
					}

					actual = min;

					if ( strcmp( actual, "zzz" ) )
					{
						strcat( buf, actual );
						strcat( buf, "\n\r" );
					}
				}
			}
			else if ( !str_cmp( "countplayers", command ) )
			{
				com_found = TRUE;
				for ( d = descriptor_list; d != NULL; d = d->next )
				{
					wch = ( d->original != NULL ) ? d->original : d->character;

					if ( !wch || d->connected != CON_PLAYING )
						continue;
					if ( IS_IMMORTAL( wch ) && ( wch->invis_level > 0 || wch->incog_level > 0 ) )
						continue;
					count++;
				}

				sprintf( buf, "%d\n\r", count );
			}
			break;
		case 'L':
			if ( !str_cmp( "listplayers", command ) )
			{
				com_found = TRUE;
				for ( d = descriptor_list; d != NULL; d = d->next )
				{
					wch = ( d->original != NULL ) ? d->original : d->character;
					if ( !wch || d->connected != CON_PLAYING || IS_AFFECTED( wch, AFF_INVISIBLE ) || IS_AFFECTED( wch, AFF_HIDE ) )
						continue;
					if ( IS_IMMORTAL( wch ) && ( wch->invis_level > 0 || wch->incog_level > 0 ) )
						continue;

					strcat( buf, wch->name );
					strcat( buf, " " );
					if ( wch->pcdata )
						strcat( buf, wch->pcdata->title );
					strcat( buf, "\n\r" );
				}
			}
			break;
		case 'S':
			if ( !str_cmp( "showplayer", command ) )
			{
				com_found = TRUE;
				one_argument( line, command );

				for ( d = descriptor_list; d != NULL; d = d->next )
				{
					wch = ( d->original != NULL ) ? d->original : d->character;

					if ( !wch || d->connected != CON_PLAYING || IS_AFFECTED( wch, AFF_INVISIBLE ) || IS_AFFECTED( wch, AFF_HIDE ) )
						continue;
					if ( IS_IMMORTAL( wch ) && ( wch->invis_level > 0 || wch->incog_level > 0 ) )
						continue;
					if ( str_cmp( wch->name, command ) )
						continue;

					victim = d->character;

					for ( obj = victim->carrying; obj != NULL; obj = obj->next_content )
					{
						if ( obj->wear_loc == WEAR_NONE )
							continue;

						for ( ed = obj->extra_descr; ed; ed = ed->next )
							if ( ed->keyword[ 0 ] == '_' )
							{
								if ( !found && victim->description[ 0 ] != '\0' )
								{
									strcat( buf, victim->description );
									strcat( buf, "\n\r" );
								}

								act( ed->description, victim, buf + strlen( buf ), victim, TO_BUF );
								found = TRUE;
							}

						for ( ed = obj->pIndexData->extra_descr; ed; ed = ed->next )
							if ( ed->keyword[ 0 ] == '_' )
							{
								if ( !found && victim->description[ 0 ] != '\0' )
								{
									strcat( buf, victim->description );
									strcat( buf, "\n\r" );
								}

								act( ed->description, victim, buf + strlen( buf ), victim, TO_BUF );
								found = TRUE;
							}
					}

					if ( !found )
					{
						if ( victim->description[ 0 ] != '\0' )
							strcat( buf, victim->description );
						else
							act( "Nie widzisz nic dziwnego w $B.", victim, buf + strlen( buf ), victim, TO_BUF );
					}
				}
			}
			break;
		case 'H':
			if ( !str_cmp( "helplist", command ) )
			{
				com_found = TRUE;

				for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
				{
					if ( pHelp->level <= 2 )
						strcat( buf, pHelp->keyword );
					if ( pHelp->next )
						strcat( buf, " " );
				}
			}
			else if ( !str_cmp( "help", command ) )
			{
				com_found = TRUE;

				if ( line[ 0 ] == '\0' )
				{
					sprintf( command, "summary" );
					line = command;
				}

				for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
				{
					if ( pHelp->level >= LEVEL_HERO )
						continue;

					if ( is_name( line, pHelp->keyword ) )
					{
						/* add seperator if found */

						if ( pHelp->text[ 0 ] == '.' )
							strcat( buf, pHelp->text + 1 );
						else
							strcat( buf, pHelp->text );
						found = TRUE;

						break;
					}
				}

				if ( !found )
					strcat( buf, "Brak pomocy na ten temat.\n\r" );
			}
			break;
	}

	if ( !com_found )
		strcat( buf, "Komand not rekognajzd." );

	write_to_info_descriptor( id, buf );
	return TRUE;
}

void close_info_socket( INFO_DESCRIPTOR_DATA *idclose )
{
	if ( id_next == idclose )
		id_next = id_next->next;

	if ( idclose == info_descriptor_list )
	{
		info_descriptor_list = info_descriptor_list->next;
	}
	else
	{
		INFO_DESCRIPTOR_DATA *id;

		for ( id = info_descriptor_list; id && id->next != idclose; id = id->next )
			;
		if ( id != NULL )
			id->next = idclose->next;
	}

	FD_CLR( idclose->descriptor, &fSet );
	close( idclose->descriptor );
	free_info_descriptor( idclose );
}

void roll_stats (CHAR_DATA *ch)
{
    int stat[MAX_STATS], x, hp_mod, v_mod, min, max;
    char buf[ MAX_STRING_LENGTH ];

    /**
     * ustaw standardowe wartosci
     */
    for (x = 0; x < MAX_STATS; x++)
    {
        stat[x] = number_range(60, 70);
    }

    /**
     * +28 punktow
     */
    switch (GET_RACE( ch ) )
    {
        case RACE_PC_HUMAN: /* czlowiek */
            stat[STAT_STR] += number_range(2, 4);
            stat[STAT_INT] += number_range(2, 4);
            stat[STAT_WIS] += number_range(2, 4);
            stat[STAT_DEX] += number_range(2, 4);
            stat[STAT_CON] += number_range(2, 4);
            stat[STAT_CHA] += number_range(2, 4);
            stat[STAT_LUC] += number_range(2, 4);
            break;
        case RACE_PC_ELF: /* elf */
            stat[STAT_INT] += number_range( 4,  8);
            stat[STAT_WIS] += number_range( 8, 16);
            stat[STAT_DEX] += number_range( 2,  4);
            break;
        case RACE_PC_DWARF: /* krasnolud */
            stat[STAT_STR] += number_range(  5, 10);
            stat[STAT_WIS] += number_range(  2,  4);
            stat[STAT_CON] += number_range(  7, 14);
            break;
        case RACE_PC_GNOME: /* gnom */
            stat[STAT_INT] += number_range(  6, 12);
            stat[STAT_WIS] += number_range(  4,  8);
            stat[STAT_DEX] += number_range(  4,  8);
            break;
        case RACE_PC_HALFELF: /* polelf */
            stat[STAT_INT] += number_range(  4,  8);
            stat[STAT_WIS] += number_range(  6, 12);
            stat[STAT_DEX] += number_range(  2,  4);
            stat[STAT_CON] += number_range(  2,  4);
            break;
        case RACE_PC_HALFLING: /* niziolek */
            stat[STAT_DEX] += number_range( 10, 20);
            stat[STAT_LUC] += number_range(  4,  8);
            break;
        case RACE_PC_HALFORC: /* polork */
            stat[STAT_STR] += number_range(  6, 12);
            stat[STAT_CON] += number_range(  8, 16);
            break;
    }

    /**
     * modyfikator od plci
     */
    switch (ch->sex)
    {
        case SEX_FEMALE:
            stat[STAT_INT] += number_range(0, 4);
            stat[STAT_DEX] += number_range(0, 4);
            stat[STAT_WIS] += number_range(0, 4);
            break;
        case SEX_MALE:
            stat[STAT_STR] += number_range(0, 4);
            stat[STAT_CON] += number_range(0, 4);
            stat[STAT_CHA] += number_range(0, 4);
            break;
    }

    /* waga/wzrost */
    switch (ch->sex)
    {
        case SEX_FEMALE:
            min = pc_race_table[GET_REAL_RACE(ch)].weight[2];
            max = pc_race_table[GET_REAL_RACE(ch)].weight[3];
            ch->weight = (number_range (min, max) + number_range (min, max))/2;
            min = pc_race_table[GET_REAL_RACE(ch)].height[2];
            max = pc_race_table[GET_REAL_RACE(ch)].height[3];
            ch->height = (number_range (min, max) + number_range (min, max))/2;
            break;
        case SEX_MALE:
        default:
            min = pc_race_table[GET_REAL_RACE(ch)].weight[0];
            max = pc_race_table[GET_REAL_RACE(ch)].weight[1];
            ch->weight = (number_range (min, max) + number_range (min, max))/2;
            min = pc_race_table[GET_REAL_RACE(ch)].height[0];
            max = pc_race_table[GET_REAL_RACE(ch)].height[1];
            ch->height = (number_range (min, max) + number_range (min, max))/2;
            break;
    }

    /* barbarzyncy ciezsi */
    if ( ch->class == CLASS_BARBARIAN )
    {
        switch (ch->sex)
        {
            case SEX_FEMALE:
                ch->weight += number_range(10, 20);
                ch->height += number_range(2, 5);
                break;
            case SEX_MALE:
                ch->weight += number_range(30, 60);
                ch->height += number_range(5, 10);
                break;
        }
    }

    /**
     * bonus od profesji +40
     */
    switch (ch->class)
    {
        case CLASS_MAG:
            stat[STAT_INT] += number_range( 10, 20);
            stat[STAT_WIS] += number_range(  5, 10);
            stat[STAT_CHA] += number_range(  5, 10);
            break;
        case CLASS_CLERIC:
            stat[STAT_INT] += number_range(  5, 10);
            stat[STAT_WIS] += number_range( 10, 20);
            stat[STAT_CHA] += number_range(  5, 10);
            break;
        case CLASS_DRUID:
            stat[STAT_INT] += number_range(  5, 10);
            stat[STAT_WIS] += number_range( 10, 20);
            stat[STAT_LUC] += number_range(  5, 10);
            break;
        case CLASS_THIEF:
            stat[STAT_DEX] += number_range( 10, 20);
            stat[STAT_CHA] += number_range(  5, 10);
            stat[STAT_LUC] += number_range(  5, 10);
            break;
        case CLASS_WARRIOR:
            stat[STAT_STR] += number_range(  5, 10);
            stat[STAT_DEX] += number_range(  5, 10);
            stat[STAT_CON] += number_range( 10, 20);
            break;
        case CLASS_PALADIN:
            stat[STAT_STR] += number_range(  5, 10);
            stat[STAT_DEX] += number_range(  5, 10);
            stat[STAT_CON] += number_range(  5, 10);
            stat[STAT_CHA] += number_range(  5, 10);
            break;
        case CLASS_BARBARIAN:
            stat[STAT_STR] += number_range( 10, 20);
            stat[STAT_CON] += number_range( 10, 20);
            break;
        case CLASS_BLACK_KNIGHT:
            stat[STAT_STR] += number_range(  5, 10);
            stat[STAT_DEX] += number_range(  5, 10);
            stat[STAT_CON] += number_range(  5, 10);
            stat[STAT_CHA] += number_range(  5, 10);
            break;
        case CLASS_SHAMAN:
        case CLASS_MONK:
        case CLASS_BARD:
            break;
    }

    for (x = 0; x < MAX_STATS; x++)
    {
        ch->perm_stat[x] = stat[x];
    }

    sprintf
        (
         buf,
         "[ROLL_STAT][BASE] Postaæ: %s bonus_race: %s Klasa: %s str: %d int: %d wis: %d dex: %d con: %d cha: %d luc: %d",
         ch-> name,
         race_table[ GET_RACE( ch ) ].name,
         class_table[ch->class].name,
         stat[STAT_STR],
         stat[STAT_INT],
         stat[STAT_WIS],
         stat[STAT_DEX],
         stat[STAT_CON],
         stat[STAT_CHA],
         stat[STAT_LUC]
        );
    log_string( buf );
    wiznet( buf, NULL, NULL, WIZ_SITES, 0, get_trust( ch ) );

    hp_mod = 0;
    v_mod  = 0;

    switch ( GET_RACE( ch ) )
    {
        case RACE_PC_HUMAN:
            v_mod += 10;
            break;
        case RACE_PC_ELF:
            v_mod += 16;
            break;
        case RACE_PC_DWARF:
            v_mod -= 18;
            break;
        case RACE_PC_GNOME:
            v_mod += 11;
            break;
        case RACE_PC_HALFELF:
            v_mod += 13;
            break;
        case RACE_PC_HALFLING:
            v_mod += 12;
            break;
        case RACE_PC_HALFORC:
            v_mod -= 12;
            break;
    }

    switch ( ch->class )
    {
        case CLASS_MAG:
            hp_mod += 13;
            v_mod  -= 30;
            break;
        case CLASS_DRUID:
            hp_mod += 15;
            v_mod  += 5;
            break;
        case CLASS_CLERIC:
            hp_mod += 16;
            v_mod  -= 10;
            break;
        case CLASS_THIEF:
        case CLASS_BARD:
            hp_mod += 18;
            v_mod  += 10;
            break;
        case CLASS_WARRIOR:
        case CLASS_PALADIN:
        case CLASS_BLACK_KNIGHT:
            hp_mod += 27;
            break;
        case CLASS_BARBARIAN:
            hp_mod += 30;
            v_mod  += 5;
            break;

        case CLASS_MONK:
            hp_mod += 23;
            v_mod  += 8;
            break;

        case CLASS_SHAMAN:
            hp_mod += 19;
            v_mod  += 5;
            break;
    }

    ch->max_hit += hp_mod;
    ch->max_move += v_mod;
}

