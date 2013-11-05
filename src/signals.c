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
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: signals.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/signals.c $
 *
 */
#include <sys/time.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"
#include "progs.h"

#ifdef CYGWIN
volatile sig_atomic_t fatal_error_in_process = 0;
void shutdown_wgrace();
void do_copyover (CHAR_DATA *ch, char * argument);
bool write_to_descriptor	    args( ( DESCRIPTOR_DATA *d, char *txt, int length ) );
bool write_to_descriptor_2	args( ( int desc, char *txt, int length ) );

/* Copyover note:
 * Whenever a program is executed from exec (as in the case of copyover)
 * all signals from the previous execution of the mud return to their
 * SIG_DEF (default) signal handler, since copyover starts the mud
 * process all over again, init_signals will get called and properly
 * reset the signal handler.
 */

/** Function: signal_handler
  * Descr   : Our signal handler, to shutdown gracefully (if possible)
  *         : upon reciept of a signal signifying we should shutdown.
  * Returns : void
  * Syntax  : (n/a)
  * Written : v1.0 11/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */

void signal_handler(int sig)
{
  /* Since our 'shutdown_wgrace' is not reentrant, and may (likely) access
   * non-volatile global variables, be damn sure it's not called twice.
   */
  if ( fatal_error_in_process )
     raise(sig);

  switch(sig)
  {
    case SIGBUS:
    case SIGTERM:
    case SIGABRT:
    case SIGSEGV:
    case SIGFPE:
    case SIGILL:
		fatal_error_in_process = 1;	     /* Yes, this IS a fatal error  */
		bugf("BUG: Signal handler in %s.",  /* Log signal to bug log file  */
			(sig == SIGBUS)  ? "SIGBUS"  :
			(sig == SIGTERM) ? "SIGTERM" :
			(sig == SIGABRT) ? "SIGABRT" :
			(sig == SIGSEGV) ? "SIGSEGV" :
			(sig == SIGFPE) ? "SIGFPE" : "SIGILL");

		bugf("Bugged function :%s.", last_command ? last_command : "unknown");

		shutdown_wgrace();		     /* Attempt a graceful shutdown  */
		raise(sig);			     /* set return status of process */
      break;
	}
}

/** Function: init_signals
  * Descr   : Initialize signals that we trap, setting up a handler for them.
  * Returns : void
  * Syntax  : void
  * Written : v1.0 11/98
  * Author  : Gary McNickle <gary@dharvest.com>
  * Note    : By default, signals sent to sighandler are blocked until the
  *         : handler returns, but other signals are not blocked. We need
  *         : to block any other signal we recieve that we normally trap,
  *         : until we are done trying to be graceful...
  */

void init_signals()
{
  struct sigaction sigact;
  sigset_t mask;

    /* NOTE: We inherit any current signal actions by default.
     * Dont install a signal handler for any ignored signals.
     */
    sigaction ( SIGBUS, NULL, &sigact );

    if ( sigact.sa_handler != SIG_IGN )
    {
      sigact.sa_handler = signal_handler;

      sigemptyset(&mask);
      sigaddset(&mask, SIGTERM);
      sigaddset(&mask, SIGABRT);
      sigaddset(&mask, SIGSEGV);
      sigaddset(&mask, SIGFPE);
      sigaddset(&mask, SIGILL);
      sigact.sa_mask = mask;
      sigaction ( SIGBUS, &sigact, NULL );
    } else
      log_string("Init: Signal SIGBUS ignored.");

    sigaction ( SIGTERM, NULL, &sigact );

    if ( sigact.sa_handler != SIG_IGN )
    {
      sigact.sa_handler = signal_handler;

      sigemptyset(&mask);
      sigaddset(&mask, SIGBUS);
      sigaddset(&mask, SIGABRT);
      sigaddset(&mask, SIGSEGV);
      sigaddset(&mask, SIGFPE);
      sigaddset(&mask, SIGILL);
      sigact.sa_mask = mask;
      sigaction ( SIGTERM, &sigact, NULL );
    } else
      log_string("Init: Signal SIGTERM ignored.");

    sigaction ( SIGABRT, NULL, &sigact );

    if ( sigact.sa_handler != SIG_IGN )
    {
      sigact.sa_handler = signal_handler;
      sigemptyset(&mask);
      sigaddset(&mask, SIGBUS);
      sigaddset(&mask, SIGTERM);
      sigaddset(&mask, SIGSEGV);
      sigaddset(&mask, SIGFPE);
      sigaddset(&mask, SIGILL);
      sigact.sa_mask = mask;
      sigaction ( SIGABRT, &sigact, NULL );
    } else
      log_string("Init: Signal SIGABRT ignored.");

    sigaction ( SIGSEGV, NULL, &sigact );

    if ( sigact.sa_handler != SIG_IGN )
    {
      sigact.sa_handler = signal_handler;
      sigemptyset(&mask);
      sigaddset(&mask, SIGBUS);
      sigaddset(&mask, SIGTERM);
      sigaddset(&mask, SIGABRT);
      sigaddset(&mask, SIGILL);
      sigaddset(&mask, SIGFPE);
      sigact.sa_mask = mask;
      sigaction ( SIGSEGV, &sigact, NULL );
    } else
      log_string("Init: Signal SIGSEGV ignored.");

    sigaction ( SIGFPE, NULL, &sigact );

    if ( sigact.sa_handler != SIG_IGN )
    {
      sigact.sa_handler = signal_handler;

      sigemptyset(&mask);
      sigaddset(&mask, SIGBUS);
      sigaddset(&mask, SIGTERM);
      sigaddset(&mask, SIGABRT);
      sigaddset(&mask, SIGSEGV);
      sigaddset(&mask, SIGILL);
      sigact.sa_mask = mask;
      sigaction ( SIGFPE, &sigact, NULL );
    } else
      log_string("Init: Signal SIGFPE ignored.");


    sigaction ( SIGILL, NULL, &sigact );

    if ( sigact.sa_handler != SIG_IGN )
    {
      sigact.sa_handler = signal_handler;

      sigemptyset(&mask);
      sigaddset(&mask, SIGBUS);
      sigaddset(&mask, SIGTERM);
      sigaddset(&mask, SIGABRT);
      sigaddset(&mask, SIGSEGV);
      sigaddset(&mask, SIGFPE);
      sigact.sa_mask = mask;
      sigaction ( SIGILL, &sigact, NULL );
    } else
      log_string("Init: Signal SIGILL ignored.");

    log_string("Init: Signals initialized.");

}


/** Function: shutdown_wgrace
  * Descr   : Upon receipt of a fatal signal, attempt a graceful shutdown.
  * Returns : void
  * Syntax  : void
  * Written : v1.0 11/98
  * Author  : Gary McNickle <gary@dharvest.com>
  */
void shutdown_wgrace()
{
    char buf[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d ; d = d->next )
	if( d->character && IS_IMMORTAL(d->character) && d->connected == CON_PLAYING )
	{
	    sprintf(buf,"\n\r ****************************************************\n\r");
	    write_to_descriptor (d, buf, 0);
	    sprintf(buf," Zabugowana komenda: %s\n\r", last_command ? last_command : "nie wiadomo");
	    write_to_descriptor (d, buf, 0);
	    sprintf(buf," ****************************************************\n\r");
	    write_to_descriptor (d, buf, 0);
	}

    if(!supermob)
	init_supermob();

    free_string(supermob->name);
    supermob->name=str_dup("*** SYSTEM ***");
    do_copyover (supermob,"now");
    return;
}
#else
#ifndef SI_KERNEL
#define SI_KERNEL 1
#endif

volatile sig_atomic_t fatal_error_in_process = 0;
void shutdown_wgrace( char * info);
void do_copyover (CHAR_DATA *ch, char * argument);
bool write_to_descriptor	    args( ( DESCRIPTOR_DATA *d, char *txt, int length ) );
bool write_to_descriptor_2	args( ( int desc, char *txt, int length ) );
extern char *strsignal (int __sig) __THROW;

char * translate_error_code( siginfo_t *info )
{
char * signal = NULL, *signal_code_info = NULL;
void * addr = NULL;

    signal = strsignal(info->si_signo);

    switch( info->si_signo )
    {
	case SIGTERM:
		switch( info->si_code )
		{
		    case SI_USER: signal_code_info = "kill, sigsend or raise";break;
		    case SI_KERNEL: signal_code_info = "kernel";break;
		    case SI_QUEUE: signal_code_info = "sigqueue";break;
		    case SI_TIMER: signal_code_info = "timer expire";
		    case SI_MESGQ: signal_code_info = "mesg state changed";break;
		    case SI_ASYNCIO: signal_code_info = "AIO completed";break;
		    case SI_SIGIO: signal_code_info = "queued SIGIO";break;
		}
		break;
        case SIGABRT:
		switch( info->si_code )
		{
		    case SI_USER: signal_code_info = "kill, sigsend or raise";break;
		    case SI_KERNEL: signal_code_info = "kernel";break;
		    case SI_QUEUE: signal_code_info = "sigqueue";break;
		    case SI_TIMER: signal_code_info = "timer expire";
		    case SI_MESGQ: signal_code_info = "mesg state changed";break;
		    case SI_ASYNCIO: signal_code_info = "AIO completed";break;
		    case SI_SIGIO: signal_code_info = "queued SIGIO";break;
		}
		break;

        case SIGILL:
		addr = info->si_addr;

		switch( info->si_code )
		{
		    case ILL_ILLOPC: signal_code_info = "illegal opcode";break;
		    case ILL_ILLOPN: signal_code_info = "illegal operand";break;
		    case ILL_ILLADR: signal_code_info = "illegal addresing mode";break;
		    case ILL_ILLTRP: signal_code_info = "illegal trap";break;
		    case ILL_PRVOPC: signal_code_info = "privileged opcode";break;
		    case ILL_PRVREG: signal_code_info = "privileged operand";break;
		    case ILL_COPROC: signal_code_info = "coprocessor error";break;
		    case ILL_BADSTK: signal_code_info = "internal stack error";break;
		}
		break;

	case SIGBUS:
		addr = info->si_addr;

		switch( info->si_code )
		{
		    case BUS_ADRALN: signal_code_info = "invalid address alignment";break;
		    case BUS_ADRERR: signal_code_info = "non-existant physical address";break;
		    case BUS_OBJERR: signal_code_info = "object specific hardware error";break;
		}
		break;

	case SIGFPE:
		addr = info->si_addr;

		switch( info->si_code )
		{
		    case FPE_INTDIV: signal_code_info = "integer divide by zero";break;
		    case FPE_INTOVF: signal_code_info = "integer overflow";break;
		    case FPE_FLTDIV: signal_code_info = "floating point divide by zero";break;
		    case FPE_FLTOVF: signal_code_info = "floating divide by zero";break;
		    case FPE_FLTUND: signal_code_info = "floating point underflow";break;
		    case FPE_FLTRES: signal_code_info = "floating point inexact result";break;
		    case FPE_FLTINV: signal_code_info = "floating point invalid operation";break;
		    case FPE_FLTSUB: signal_code_info = "subscript out of range";break;
		}
		break;

	case SIGSEGV:
		addr = info->si_addr;

		switch( info->si_errno )
		{
		    case SEGV_MAPERR: signal_code_info = "address not mapped to object";break;
		    case SEGV_ACCERR: signal_code_info = "invalid permission for mapped object";break;
		}
		break;

	case SIGPOLL:
		switch( info->si_code )
		{
		    case POLL_IN: signal_code_info = "data input available";break;
		    case POLL_OUT: signal_code_info = "output buffers available";break;
		    case POLL_MSG: signal_code_info = "input message available";break;
		    case POLL_ERR: signal_code_info = "i/o error";break;
		    case POLL_PRI: signal_code_info = "high priority input available";break;
		    case POLL_HUP: signal_code_info = "device disconnect";break;
		}
		break;

    }

    bugf("Signal: %s", signal ? signal : "UNKNOWN?");

    if( signal_code_info )
	bugf( "Info: %s", signal_code_info );

    if( addr != NULL )
	bugf("Addres of fault: %x", addr );

    if( last_command )
			bugf("Bugged function :%s, previous: %s.", last_command, previous_command?previous_command:"BRAK_POPRZEDNIEJ");
    return signal_code_info;
}

void signal_handler(int sig, siginfo_t * info, void * ptr)
{
	if ( fatal_error_in_process )
		raise( sig );

    fatal_error_in_process = 1;

    shutdown_wgrace( translate_error_code( info ) );
}

void init_signals()
{
  struct sigaction sigact;
  sigset_t mask;

    sigaction ( SIGBUS, NULL, &sigact );

    if ( sigact.sa_handler != SIG_IGN )
    {
      sigact.sa_sigaction = signal_handler;

      sigemptyset(&mask);
      sigaddset(&mask, SIGTERM);
      sigaddset(&mask, SIGABRT);
      sigaddset(&mask, SIGSEGV);
      sigaddset(&mask, SIGFPE);
      sigaddset(&mask, SIGILL);
      sigaddset(&mask, SIGPOLL);
      sigact.sa_mask = mask;

      sigact.sa_flags = SA_SIGINFO|SA_NOMASK;
      sigaction ( SIGBUS, &sigact, NULL );
    } else
      log_string("Init: Signal SIGBUS ignored.");

    sigaction ( SIGTERM, NULL, &sigact );

    if ( sigact.sa_handler != SIG_IGN )
    {
      sigact.sa_sigaction = signal_handler;

      sigemptyset(&mask);
      sigaddset(&mask, SIGBUS);
      sigaddset(&mask, SIGABRT);
      sigaddset(&mask, SIGSEGV);
      sigaddset(&mask, SIGFPE);
      sigaddset(&mask, SIGILL);
      sigaddset(&mask, SIGPOLL);
      sigact.sa_mask = mask;

      sigact.sa_flags = SA_SIGINFO|SA_NOMASK;
      sigaction ( SIGTERM, &sigact, NULL );
    } else
      log_string("Init: Signal SIGTERM ignored.");

    sigaction ( SIGABRT, NULL, &sigact );

    if ( sigact.sa_handler != SIG_IGN )
    {
      sigact.sa_sigaction = signal_handler;

      sigemptyset(&mask);
      sigaddset(&mask, SIGBUS);
      sigaddset(&mask, SIGTERM);
      sigaddset(&mask, SIGSEGV);
      sigaddset(&mask, SIGFPE);
      sigaddset(&mask, SIGILL);
      sigaddset(&mask, SIGPOLL);
      sigact.sa_mask = mask;

      sigact.sa_flags = SA_SIGINFO|SA_NOMASK;
      sigaction ( SIGABRT, &sigact, NULL );
    } else
      log_string("Init: Signal SIGABRT ignored.");

    sigaction ( SIGSEGV, NULL, &sigact );

    if ( sigact.sa_handler != SIG_IGN )
    {
      sigact.sa_sigaction = signal_handler;

      sigemptyset(&mask);
      sigaddset(&mask, SIGBUS);
      sigaddset(&mask, SIGTERM);
      sigaddset(&mask, SIGABRT);
      sigaddset(&mask, SIGILL);
      sigaddset(&mask, SIGFPE);
      sigaddset(&mask, SIGPOLL);

      sigact.sa_flags = SA_SIGINFO|SA_NOMASK;
      sigaction ( SIGSEGV, &sigact, NULL );
    } else
      log_string("Init: Signal SIGSEGV ignored.");

    sigaction ( SIGFPE, NULL, &sigact );

    if ( sigact.sa_handler != SIG_IGN )
    {
      sigact.sa_sigaction = signal_handler;

      sigemptyset(&mask);
      sigaddset(&mask, SIGBUS);
      sigaddset(&mask, SIGTERM);
      sigaddset(&mask, SIGABRT);
      sigaddset(&mask, SIGSEGV);
      sigaddset(&mask, SIGILL);
      sigaddset(&mask, SIGPOLL);
      sigact.sa_mask = mask;

      sigact.sa_flags = SA_SIGINFO|SA_NOMASK;
      sigaction ( SIGFPE, &sigact, NULL );
    } else
      log_string("Init: Signal SIGFPE ignored.");


    sigaction ( SIGILL, NULL, &sigact );

    if ( sigact.sa_handler != SIG_IGN )
    {
      sigact.sa_sigaction = signal_handler;

      sigemptyset(&mask);
      sigaddset(&mask, SIGBUS);
      sigaddset(&mask, SIGTERM);
      sigaddset(&mask, SIGABRT);
      sigaddset(&mask, SIGSEGV);
      sigaddset(&mask, SIGFPE);
      sigaddset(&mask, SIGPOLL);
      sigact.sa_mask = mask;

      sigact.sa_flags = SA_SIGINFO|SA_NOMASK;
      sigaction ( SIGILL, &sigact, NULL );
    } else
      log_string("Init: Signal SIGILL ignored.");


    sigaction ( SIGPOLL, NULL, &sigact );

    if ( sigact.sa_handler != SIG_IGN )
    {
      sigact.sa_sigaction = signal_handler;

      sigemptyset(&mask);
      sigaddset(&mask, SIGBUS);
      sigaddset(&mask, SIGTERM);
      sigaddset(&mask, SIGABRT);
      sigaddset(&mask, SIGSEGV);
      sigaddset(&mask, SIGFPE);
      sigaddset(&mask, SIGILL);
      sigact.sa_mask = mask;

      sigact.sa_flags = SA_SIGINFO|SA_NOMASK;
      sigaction ( SIGPOLL, &sigact, NULL );
    } else
      log_string("Init: Signal SIGPOLL ignored.");


    log_string("Init: Signals initialized.");

}

void shutdown_wgrace( char * info)
{
    char buf[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d ; d = d->next )
	if( d->character && IS_IMMORTAL(d->character) && d->connected == CON_PLAYING )
	{
	    sprintf(buf,"\n\r ****************************************************\n\r");
	    write_to_descriptor (d, buf, 0);
			sprintf(buf,"Zabugowana komenda: %s, poprzednia: %s\n\r", last_command ? last_command : "nie wiadomo", previous_command ? previous_command: "nie ustawiono" );
	    write_to_descriptor (d, buf, 0);

	    if( info )
	    {
		sprintf(buf," **  Signal: %s \n\r", info);
		write_to_descriptor (d, buf, 0);
	    }
	    sprintf(buf," ****************************************************\n\r");
	    write_to_descriptor (d, buf, 0);
	    save_char_obj( d->character, FALSE, FALSE );
	}

    if(!supermob)
	init_supermob();

    free_string(supermob->name);
    supermob->name=str_dup("*** SYSTEM ***");
    do_copyover (supermob,"now");
    return;
}
#endif
