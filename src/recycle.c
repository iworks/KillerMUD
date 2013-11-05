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
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: recycle.c 11138 2012-03-21 21:22:26Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/12.02/src/recycle.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "nanny.h"
#include "recycle.h"
#include "lang.h"
#include "traps.h"
#include "tables.h"
#include "progs.h"

EVENT2_DATA *ev2_free;
EVENT2_DATA *new_event2( void )
{
	static EVENT2_DATA ev2_zero;
	EVENT2_DATA *ev2;

	if ( ev2_free == NULL )
		ev2 = malloc( sizeof( *ev2 ) );
	else
	{
		ev2 = ev2_free;
		ev2_free = ev2_free->next;
	}

	*ev2 = ev2_zero;
	VALIDATE( ev2 );
	ev2->to = NULL;
	return ev2;
}
void free_event2( EVENT2_DATA *ev2 )
{
	if ( !IS_VALID( ev2 ) )
		return ;

	INVALIDATE( ev2 );

	ev2->next = ev2_free;
	ev2_free = ev2;
}

/* stuff for recyling notes */
NOTE_DATA *note_free = NULL;
NOTE_DATA *new_note()
{
	NOTE_DATA * note;

	if ( note_free == NULL )
		CREATE( note, NOTE_DATA, 1 );
	else
	{
		note = note_free;
		note_free = note_free->next;
	}
	VALIDATE( note );
	return note;
}
void free_note( NOTE_DATA *note )
{
	if ( !IS_VALID( note ) )
		return ;

	free_string( note->text );
	free_string( note->subject );
	free_string( note->to_list );
	free_string( note->date );
	free_string( note->sender );
	INVALIDATE( note );

	note->next = note_free;
	note_free = note;
}

/*learn system*/
LEARN_LIST *learn_list_free = NULL;
LEARN_LIST *new_learn_list( void )
{
	static LEARN_LIST learn_list_zero;
	LEARN_LIST *learn;

	if ( learn_list_free == NULL )
		CREATE( learn, LEARN_LIST, 1 );
	else
	{
		learn = learn_list_free;
		learn_list_free = learn_list_free->next;
	}

	*learn = learn_list_zero;
	VALIDATE( learn );
	return learn;
}
void free_learn_list( LEARN_LIST *learn )
{
	if ( !IS_VALID( learn ) ) return ;

	INVALIDATE( learn );
	learn->next = learn_list_free;
	learn_list_free = learn;
}

LEARN_TRICK_LIST *learn_trick_list_free = NULL;
LEARN_TRICK_LIST *new_learn_trick_list( void )
{
	static LEARN_TRICK_LIST learn_list_zero;
	LEARN_TRICK_LIST *learn;

	if ( learn_trick_list_free == NULL )
		CREATE( learn, LEARN_TRICK_LIST, 1 );
	else
	{
		learn = learn_trick_list_free;
		learn_trick_list_free = learn_trick_list_free->next;
	}

	*learn = learn_list_zero;
	VALIDATE( learn );
	return learn;
}
void free_learn_trick_list( LEARN_TRICK_LIST *learn )
{
	if ( !IS_VALID( learn ) ) return ;

	INVALIDATE( learn );
	learn->next = learn_trick_list_free;
	learn_trick_list_free = learn;
}

LEARN_DATA *learn_free = NULL;
LEARN_DATA *new_learn_data( void )
{
	static LEARN_DATA learn_data_zero;
	LEARN_DATA *learn;

	if ( learn_free == NULL )
		CREATE( learn, LEARN_DATA, 1 );
	else
	{
		learn = learn_free;
		learn_free = learn_free->next;
	}

	*learn = learn_data_zero;
	VALIDATE( learn );
	return learn;
}
void free_learn_data( LEARN_DATA *learn )
{
	if ( !IS_VALID( learn ) ) return ;
	INVALIDATE( learn );
	learn->next = learn_free;
	learn_free = learn;
}

LEARN_TRICK_DATA *learn_trick_free = NULL;
LEARN_TRICK_DATA *new_learn_trick_data( void )
{
	static LEARN_TRICK_DATA learn_trick_data_zero;
	LEARN_TRICK_DATA *learn;

	if ( learn_free == NULL )
		CREATE( learn, LEARN_TRICK_DATA, 1 );
	else
	{
		learn = learn_trick_free;
		learn_trick_free = learn_trick_free->next;
	}

	*learn = learn_trick_data_zero;
	VALIDATE( learn );
	return learn;
}
void free_learn_trick_data( LEARN_TRICK_DATA *learn )
{
	if ( !IS_VALID( learn ) ) return ;
	INVALIDATE( learn );
	learn->next = learn_trick_free;
	learn_trick_free = learn;
}


/*artefact*/
ARTEFACT_DATA *artefact_free = NULL;
ARTEFACT_DATA *new_artefact_data( void )
{
	static ARTEFACT_DATA artefact_data_zero;
	ARTEFACT_DATA *artefact;

	if ( artefact_free == NULL )
		CREATE( artefact, ARTEFACT_DATA, 1 );
	else
	{
		artefact = artefact_free;
		artefact_free = artefact_free->next;
	}

	*artefact = artefact_data_zero;
	VALIDATE( artefact );
	return artefact;
}
void free_artefact_data( ARTEFACT_DATA *artefact )
{
	if ( !IS_VALID( artefact ) ) return ;
	INVALIDATE( artefact );
	artefact->next = artefact_free;
	artefact_free = artefact;
}

ARTEFACT_OWNER *owner_free = NULL;
ARTEFACT_OWNER *new_artefact_owner( void )
{
	static ARTEFACT_OWNER owner_data_zero;
	ARTEFACT_OWNER *owner;

	if ( owner_free == NULL )
		CREATE( owner, ARTEFACT_OWNER, 1 );
	else
	{
		owner = owner_free;
		owner_free = owner_free->next;
	}

	*owner = owner_data_zero;
	VALIDATE( owner );
	return owner;
}
void free_artefact_owner( ARTEFACT_OWNER *owner )
{
	if ( !IS_VALID( owner ) ) return ;
	INVALIDATE( owner );
	owner->next = owner_free;
	owner_free = owner;
}

ARTEFACT_LOADER *loader_free = NULL;
ARTEFACT_LOADER *new_artefact_loader( void )
{
	static ARTEFACT_LOADER loader_data_zero;
	ARTEFACT_LOADER *loader;

	if ( loader_free == NULL )
		CREATE( loader, ARTEFACT_LOADER, 1 );
	else
	{
		loader = loader_free;
		loader_free = loader_free->next;
	}

	*loader = loader_data_zero;
	VALIDATE( loader );
	return loader;
}
void free_artefact_loader( ARTEFACT_LOADER *loader )
{
	if ( !IS_VALID( loader ) ) return ;
	INVALIDATE( loader );
	loader->next = loader_free;
	loader_free = loader;
}

/**************/

/* stuff for recycling ban structures */
BAN_DATA *ban_free = NULL;
BAN_DATA *new_ban( void )
{
	static BAN_DATA ban_zero;
	BAN_DATA *ban;

	if ( ban_free == NULL )
		CREATE( ban, BAN_DATA, 1 );
	else
	{
		ban = ban_free;
		ban_free = ban_free->next;
	}

	*ban = ban_zero;
	VALIDATE( ban );
	ban->site = &str_empty[ 0 ];
	ban->person = &str_empty[ 0 ];
	ban->reason = &str_empty[ 0 ];
	return ban;
}
void free_ban( BAN_DATA *ban )
{
	if ( !IS_VALID( ban ) )
		return ;

	free_string( ban->site );
	free_string( ban->person );
	free_string( ban->reason );
	INVALIDATE( ban );

	ban->next = ban_free;
	ban_free = ban;
}

/*board system*/

/*BOARD_DATA *board_free;

BOARD_DATA *new_board(void)
{
    static BOARD_DATA board_zero;
    BOARD_DATA *board;

    if (board_free == NULL)
	board = alloc_perm(sizeof(*board));
    else
    {
	board = board_free;
	board_free = board_free->next;
    }

    *board = board_zero;
    VALIDATE(board);
    return board;
}

void free_board(BOARD_DATA *board)
{
    if (!IS_VALID(board)) return;

    INVALIDATE(board);

    board->next = board_free;
    board_free = board;
}*/

/*message system*/
MESSAGE_DATA *msg_free = NULL;
MESSAGE_DATA *new_msg( void )
{
	static MESSAGE_DATA msg_zero;
	MESSAGE_DATA *msg;

	if ( msg_free == NULL )
		CREATE( msg, MESSAGE_DATA, 1 );
	else
	{
		msg = msg_free;
		msg_free = msg_free->next;
	}

	*msg = msg_zero;
	VALIDATE( msg );

	msg->author = &str_empty[ 0 ];
	msg->title = &str_empty[ 0 ];
	msg->message = &str_empty[ 0 ];

	return msg;
}
void free_msg( MESSAGE_DATA *msg )
{
	if ( !IS_VALID( msg ) ) return ;

	free_string( msg->author );
	free_string( msg->title );
	free_string( msg->message );
	INVALIDATE( msg );

	msg->next = msg_free;
	msg_free = msg;
}


/****************/



/*******************/




/* stuff for recycling descriptors */
DESCRIPTOR_DATA *descriptor_free = NULL;
DESCRIPTOR_DATA *new_descriptor( void )
{
	static DESCRIPTOR_DATA d_zero;
	DESCRIPTOR_DATA *d;

	if ( descriptor_free == NULL )
		CREATE( d, DESCRIPTOR_DATA, 1 );
	else
	{
		d = descriptor_free;
		descriptor_free = descriptor_free->next;
	}

	*d = d_zero;
	VALIDATE( d );

#ifdef NEW_NANNY
	d->connected	= NANNY_FIRST_INPUT;
#else
	d->connected	= CON_GET_NAME;
#endif
	d->showstr_head	= NULL;
	d->showstr_point = NULL;
	d->outsize	= 2000;
	d->pEdit	= NULL;			// OLC
	d->pString	= NULL;			// OLC
	d->editor	= 0;			// OLC
	CREATE( d->outbuf, char, d->outsize );
	d->account = NULL;
	d->remote_char = NULL;
	d->new_char_data = NULL;
	d->next = NULL;
	d->spaces = &str_empty[ 0 ];
	d->tmp_passwd = &str_empty[ 0 ];
	d->other = &str_empty[ 0 ];
	d->ignore_input = FALSE;
	d->custom_map = &str_empty[ 0 ];
	d->lookup_state = DNS_LOOKUP;

	return d;
}
void free_descriptor( DESCRIPTOR_DATA *d )
{
	if ( !IS_VALID( d ) )
		return ;
	free_string( d->host );
    free_string( d->ip_addr );
	free_string( d->user );
	free_string( d->spaces );
	free_string( d->tmp_passwd );
	free_string( d->other );
	if ( d->remote_char )
	{
		free_char( d->remote_char );
		d->remote_char = NULL;
	}

	if ( d->new_char_data )
	{
		free_new_char_data( d->new_char_data);
		d->new_char_data = NULL;
	}

	DISPOSE( d->outbuf );
	if ( d->account )
	{
		free_string( d->account->email );
		free_string( d->account->passwd );
		free_string( d->account->characters );
		free_string( d->account->activation );
		DISPOSE( d->account );
	}

	INVALIDATE( d );
	d->next = descriptor_free;
	descriptor_free = d;
}



/* stuff for recycling extended descs */
EXTRA_DESCR_DATA *extra_descr_free = NULL;
EXTRA_DESCR_DATA *new_extra_descr( void )
{
	EXTRA_DESCR_DATA * ed;

	if ( extra_descr_free == NULL )
		CREATE( ed, EXTRA_DESCR_DATA, 1 );
	else
	{
		ed = extra_descr_free;
		extra_descr_free = extra_descr_free->next;
	}

	ed->keyword = &str_empty[ 0 ];
	ed->description = &str_empty[ 0 ];
	ed->next = NULL;
	VALIDATE( ed );
	return ed;
}
void free_extra_descr( EXTRA_DESCR_DATA *ed )
{
	if ( !IS_VALID( ed ) )
		return ;

	free_string( ed->keyword );
	free_string( ed->description );
	INVALIDATE( ed );

	ed->next = extra_descr_free;
	extra_descr_free = ed;
}


/* stuff for recycling affects */
AFFECT_DATA *affect_free = NULL;
MSPELL_DATA *mspell_free = NULL;
MEMSET_DATA *memset_free = NULL;

MSPELL_DATA *new_mspell( void )
{
	static MSPELL_DATA mspell_zero;
	MSPELL_DATA *mspell;

	if ( mspell_free == NULL )
		CREATE( mspell, MSPELL_DATA, 1 );
	else
	{
		mspell = mspell_free;
		mspell_free = mspell_free->next;
	}

	*mspell = mspell_zero;


	VALIDATE( mspell );
	mspell->next = NULL;
	mspell->spell = 0;
	mspell->circle = 0;
	mspell->done = FALSE;
	mspell->duration = 0;
	return mspell;
}
void free_mspell( MSPELL_DATA *mspell )
{
	if ( !IS_VALID( mspell ) ) return ;

	INVALIDATE( mspell );
	mspell->next = mspell_free;
	mspell_free = mspell;
}

MEMSET_DATA *new_memset( void )
{
	static MEMSET_DATA memset_zero;
	MEMSET_DATA *memset;
	int i;

	if ( memset_free == NULL )
		CREATE( memset, MEMSET_DATA, 1 );
	else
	{
		memset = memset_free;
		memset_free = memset_free->next;
	}

	*memset = memset_zero;

	VALIDATE( memset );
	memset->next     = NULL;
	memset->set_name = &str_empty[0];
	
	for ( i = 0; i < MAX_SKILL; i++)
	   memset->set_spells[ i ] = 0;
	
	return memset;
}
void free_memset( MEMSET_DATA *memset )
{
	if ( !IS_VALID( memset ) ) return ;
	INVALIDATE( memset );
	
	free_string( memset->set_name );
	
	memset->next = memset_free;
	memset_free = memset;
}

AFFECT_DATA *new_affect( void )
{
	static AFFECT_DATA af_zero;
	AFFECT_DATA *af = NULL;

	if ( affect_free == NULL )
		CREATE( af, AFFECT_DATA, 1 );
	else
	{
		af = affect_free;
		affect_free = affect_free->next;
	}

	*af = af_zero;


	VALIDATE( af );
	af->where = 0;
	af->type = 0;
	af->level = 0;
	af->duration = 0;
	af->rt_duration = 0;
	af->location = 0;
	af->modifier = 0;
	af->bitvector = &AFF_NONE;
	af->info = NULL;
	af->visible = FALSE;
	af->next = NULL;
	return af;
}
void free_affect( AFFECT_DATA *af )
{
   if ( !((af != NULL) && (af->valid) ))
      return;

/* 	if ( !IS_VALID( af ) ) */
/* 		return ; */

	INVALIDATE( af );

	if ( af->info )
		free_string( af->info );

	af->next = affect_free;
	affect_free = af;
}

/* stuff for recycling objects */
OBJ_DATA *obj_free = NULL;

OBJ_DATA *new_obj( void )
{
	static OBJ_DATA obj_zero;
	OBJ_DATA *obj;

	if ( obj_free == NULL )
		CREATE( obj, OBJ_DATA, 1 );
	else
	{
		obj = obj_free;
		obj_free = obj_free->next;
	}
	*obj = obj_zero;
	VALIDATE( obj );

	obj->item_description = NULL;
	obj->ident_description = NULL;
	obj->hidden_description = NULL;
	obj->pflag_list	= NULL;
	obj->on_ground	= 0;
	obj->has_memdat = 0;
	obj->next = NULL;
	//shapeshifting
	obj->shapeshift = FALSE;
	obj->liczba_mnoga = FALSE;
	obj->gender = 0;
	//rellik: komponenty, nowy obiekt nie jest komponentem
	obj->is_spell_item = FALSE;
	obj->vnum_hoard     = 0;
	obj->hoarded_by     = NULL;

	return obj;
}

void free_obj( OBJ_DATA *obj )
{
	AFFECT_DATA * paf, *paf_next;
	EXTRA_DESCR_DATA *ed, *ed_next;
	PFLAG_DATA *pflag, *pflag_next;
	MEMDAT * mdat;
	PROG_ENV * penv;

	if ( !IS_VALID( obj ) )
		return ;

	for ( paf = obj->affected; paf != NULL; paf = paf_next )
	{
		paf_next = paf->next;
		free_affect( paf );
	}
	obj->affected = NULL;

	for ( pflag = obj->pflag_list;pflag != NULL;pflag = pflag_next )
	{
		pflag_next = pflag->next;
		free_pflag( pflag );
	}

	obj->pflag_list = NULL;

	for ( ed = obj->extra_descr; ed != NULL; ed = ed_next )
	{
		ed_next = ed->next;
		free_extra_descr( ed );
	}
	obj->extra_descr = NULL;

	//wywalanie odwo³añ do tego obiektu w memdatach
	DEBUG_INFO( "free_obj:mdat" );
	for ( mdat = memdat_list; mdat; mdat = mdat->next )
	{
		PROG_VAR * pvar, * pvar_next;

		if ( !mdat->prog_env )
			continue;

		if ( mdat->prog_env->obj == obj ) mdat->prog_env->obj = NULL;
		if ( mdat->prog_env->arg1 == obj ) mdat->prog_env->arg1 = NULL;
		if ( mdat->prog_env->arg2 == obj ) mdat->prog_env->arg2 = NULL;

		for ( pvar = mdat->prog_env->vars; pvar; pvar = pvar_next )
		{
			pvar_next = pvar->next;

			if ( pvar->type == PROG_VAR_OBJ && get_obj_var( pvar ) == obj )
				var_set_value( pvar, NULL, pvar->type );
		}
	}

	//wywalanie odwo³añ do tego obiektu w dzia³aj±cych progach
	DEBUG_INFO( "free_obj:progs" );
	for ( penv = running_prog_env; penv; penv = penv->next )
	{
		PROG_VAR * pvar, * pvar_next;

		if ( penv->obj == obj ) penv->obj = NULL;
		if ( penv->arg1 == obj ) penv->arg1 = NULL;
		if ( penv->arg2 == obj ) penv->arg2 = NULL;

		for ( pvar = penv->vars; pvar; pvar = pvar_next )
		{
			pvar_next = pvar->next;

			if ( pvar->type == PROG_VAR_OBJ && get_obj_var( pvar) == obj )
				var_set_value( pvar, NULL, pvar->type );
		}
	}

	DEBUG_INFO( "free_obj:free" );
	free_string( obj->name );
	free_string( obj->description );
	free_string( obj->short_descr );
	free_string( obj->owner );

	if ( obj->item_description )
		free_string( obj->item_description );
	if ( obj->ident_description )
		free_string( obj->ident_description );
	if ( obj->hidden_description )
		free_string( obj->hidden_description );

	obj->item_description = NULL;
	obj->ident_description = NULL;
	obj->hidden_description = NULL;
	obj->hoarded_by     = NULL;
	INVALIDATE( obj );

	obj->next = obj_free;
	obj_free = obj;
}


/* stuff for recyling characters */
CHAR_DATA *char_free = NULL;

void init_new_char_data( CHAR_DATA *ch )
{
        int i;
        if ( ch == NULL )
        {
                bug("init_new_char_data: ch == NULL", 1);
                return;
        }

	ch->next = NULL;

	ch->wait_char	= NULL;
	ch->memming		= NULL;
	ch->fight_data	= 0;
	ch->num_attacks	= 0;
	ch->lines = 30;

	for ( i = 0; i < 9;i++ )
		ch->mem_mods[ i ] = 0;

	for ( i = 0;i < 5;i++ )
		ch->saving_throw[ i ] = 0;

	for ( i = 0;i < MAX_COUNTER;i++ )
		ch->counter[ i ] = 0;

	for ( i = 0;i < MAX_RESIST;i++ )
		ch->resists[ i ] = 0;

	for ( i = 0;i < MAX_RESIST;i++ )
		ch->healing_from[ i ] = 0;

	for ( i = 0; i < MAX_SKILL;i++)
	   ch->memspell_cache[ i ] = 0;

	ch->memspell = NULL;
	ch->memset = NULL;
	ch->count_memspell = 0;
	ch->hunting = NULL;
	ch->mount = NULL;
	ch->mounting = NULL;
	ch->pflag_list = NULL;
	ch->bonus = NULL;
	ch->hoard = NULL;

	ch->name = &str_empty[ 0 ];
	/*xxx*/
	ch->name2 = &str_empty[ 0 ];
	ch->name3 = &str_empty[ 0 ];
	ch->name4 = &str_empty[ 0 ];
	ch->name5 = &str_empty[ 0 ];
	ch->name6 = &str_empty[ 0 ];

	ch->short_descr = &str_empty[ 0 ];
	ch->long_descr = &str_empty[ 0 ];
	ch->description = &str_empty[ 0 ];
	ch->prompt = &str_empty[ 0 ];
	ch->prefix	= &str_empty[ 0 ];
	ch->material	= &str_empty[ 0 ];
	ch->logon = current_time;
	ch->lines = PAGELEN;
	ch->speaking	= 0;

	for ( i = 0; i < 4; i++ )
		ch->armor[ i ] = 100;

	ch->position = POS_STANDING;
	ch->hit = 20;
	ch->max_hit = 20;
	ch->move = 100;
	ch->max_move = 100;

	ch->leader = NULL;
	ch->master = NULL;

	ch->precommand_pending = FALSE;
	ch->precommand_fun = NULL;
	ch->precommand_arg = NULL;
	ch->affected = NULL;
	ch->carrying = NULL;
	ch->walking = 0;
	ch->real_race = 0;
	ch->host = &str_empty[ 0 ];
	ch->reset_vnum = 0;
	ch->has_memdat = 0;
	ch->wiz_config = WIZCFG_SEXCONV | WIZCFG_COLOURCONV;
	ch->ss_data = NULL;
	ch->group = 0;
}

CHAR_DATA *new_char ( void )
{
	static CHAR_DATA ch_zero;
	CHAR_DATA *ch;

	if ( char_free == NULL )
		CREATE( ch, CHAR_DATA, 1 );
	else
	{
		ch = char_free;
		char_free = char_free->next;
	}

	*ch = ch_zero;
	VALIDATE( ch );

        init_new_char_data( ch );

        return ch;
}

void dispose_memory( CHAR_DATA *mob );

void free_char ( CHAR_DATA *ch )
{
	OBJ_DATA * obj;
	OBJ_DATA *obj_next;
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;
	PFLAG_DATA *pflag, *pflag_next;
	MSPELL_DATA *mspell, *mspell_next;
	MEMSET_DATA *memset, *memset_next;
	BONUS_DATA *bonus, *bonus_next;
	EVENT_DATA *event = NULL;
	MEMDAT * mdat = NULL;
	PROG_ENV * penv = NULL;

	if ( !IS_VALID( ch ) )
		return ;

	if ( IS_NPC( ch ) )
		mobile_count--;

	DEBUG_INFO( "free_char:char_from_room" );
	if ( ch->in_room != NULL )
		char_from_room( ch );


	DEBUG_INFO( "[free_char]:removing_from_list(again)" );
	if ( ch == char_list )
	{
		char_list = ch->next;
	}
	else
	{
		CHAR_DATA *prev;

		for ( prev = char_list; prev != NULL; prev = prev->next )
		{
			if ( prev->next == ch )
			{
				prev->next = ch->next;
				break;
			}
		}
	}

	DEBUG_INFO( "free_char:dispose_memory" );
	if ( IS_NPC( ch ) && ch->memory )
		dispose_memory( ch );

	ch->memory = NULL;

	DEBUG_INFO( "free_char:extract_obj" );
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
		OBJ_NEXT_CONTENT( obj, obj_next );
		extract_obj( obj );
	}

	ch->carrying = NULL;

	DEBUG_INFO( "free_char:remove_bonus" );
	for ( bonus = ch->bonus; bonus ; bonus = bonus_next )
	{
		bonus_next = bonus->next;
		bonus_remove( ch, bonus );
	}

	ch->bonus = NULL;

	DEBUG_INFO( "free_char:remove_affects" );
	for ( paf = ch->affected; paf ; paf = paf_next )
	{
		paf_next = paf->next;
		affect_remove( ch, paf );
	}

	ch->affected = NULL;

	DEBUG_INFO( "free_char:pflag_remove" );
	for ( pflag = ch->pflag_list; pflag ; pflag = pflag_next )
	{
		pflag_next = pflag->next;
		free_pflag( pflag );
	}

	ch->pflag_list = NULL;

	DEBUG_INFO( "free_char:memspell" );
	for ( mspell = ch->memspell;mspell != NULL;mspell = mspell_next )
	{
		mspell_next = mspell->next;
		free_mspell( mspell );
	}

	ch->memspell = NULL;
	ch->hoard    = NULL;

	DEBUG_INFO( "free_char:memset" );
	for ( memset = ch->memset;memset != NULL;memset = memset_next )
	{
		memset_next = memset->next;
		free_memset( memset );
	}

	ch->memset = NULL;	

	//wywalanie odwo³añ do tej postaci w dzia³aj±cych eventach
	DEBUG_INFO( "free_char:events" );
	for ( event = event_first; event; event = event->next )
	{
		if ( event->deleted )
			continue;

		if ( ( event->type == EVENT_EXTRACT_CHAR || event->type == EVENT_EXTRACT_SOUL )
		     && ( CHAR_DATA * ) event->arg1 == ch )
			event->deleted = TRUE;
	}

	//wywalanie odwo³añ do tej postaci w memdatach
	DEBUG_INFO( "free_obj:mdat" );
	for ( mdat = memdat_list; mdat; mdat = mdat->next )
	{
		PROG_VAR * pvar, * pvar_next;

		if ( !mdat->prog_env )
			continue;

		if ( mdat->prog_env->ch == ch ) mdat->prog_env->ch = NULL;
		if ( mdat->prog_env->vch == ch ) mdat->prog_env->vch = NULL;
		if ( mdat->prog_env->rch == ch ) mdat->prog_env->rch = NULL;
		if ( mdat->prog_env->arg1 == ch ) mdat->prog_env->arg1 = NULL;
		if ( mdat->prog_env->arg2 == ch ) mdat->prog_env->arg2 = NULL;

		for ( pvar = mdat->prog_env->vars; pvar; pvar = pvar_next )
		{
			pvar_next = pvar->next;

			if ( pvar->type == PROG_VAR_CHAR && get_char_var( pvar ) == ch )
				var_set_value( pvar, NULL, pvar->type );
		}
	}

	//wywalanie odwo³añ do tej postaci w dzia³aj±cych progach
	DEBUG_INFO( "free_char:progs" );
	for ( penv = running_prog_env; penv; penv = penv->next )
	{
		PROG_VAR * pvar, * pvar_next;

		if ( penv->ch == ch ) penv->ch = NULL;
		if ( penv->vch == ch ) penv->vch = NULL;
		if ( penv->rch == ch ) penv->rch = NULL;
		if ( penv->arg1 == ch ) penv->arg1 = NULL;
		if ( penv->arg2 == ch ) penv->arg2 = NULL;

		for ( pvar = penv->vars; pvar; pvar = pvar_next )
		{
			pvar_next = pvar->next;

			if ( pvar->type == PROG_VAR_CHAR && get_char_var( pvar ) == ch )
				var_set_value( pvar, NULL, pvar->type );
		}
	}

	DEBUG_INFO( "free_char:free_string" );
	free_string( ch->name );
	free_string( ch->name2 );
	free_string( ch->name3 );
	free_string( ch->name4 );
	free_string( ch->name5 );
	free_string( ch->name6 );

	free_string( ch->short_descr );
	free_string( ch->long_descr );
	free_string( ch->description );
	free_string( ch->prompt );
	free_string( ch->prefix );
	free_string( ch->material );
	free_string( ch->precommand_arg );
	free_note ( ch->pnote );
	free_pcdata( ch->pcdata );

	ch->leader = NULL;
	ch->master = NULL;
	ch->group = 0;

	ch->next = char_free;
	char_free = ch;
	INVALIDATE( ch );
	return ;
}

PC_DATA *pcdata_free = NULL;
PC_DATA *new_pcdata( void )
{
	int i;

	static PC_DATA pcdata_zero;
	PC_DATA *pcdata;

	if ( pcdata_free == NULL )
		CREATE( pcdata, PC_DATA, 1 );
	else
	{
		pcdata = pcdata_free;
		pcdata_free = pcdata_free->next;
	}

	*pcdata = pcdata_zero;
	pcdata->next = NULL;

	for ( i = 0; i < MAX_ALIAS; i++ )
	{
		pcdata->alias[ i ] = NULL;
		pcdata->alias_sub[ i ] = NULL;
	}

	for ( i = 0; i < MAX_SKILL; i++ )
	{
		pcdata->learning[ i ] = 0;
		pcdata->learn_mod[ i ] = 0;
	}

	for ( i = 0; i < MAX_LANG; i++ )
	{
		pcdata->language[ i ] = 0;
		pcdata->lang_mod[ i ] = 0;
	}

	for ( i = 0; i < TROPHY_SIZE; i++ )
	{
		pcdata->trophy[ i ][ 0 ] = 0;
		pcdata->trophy[ i ][ 1 ] = 0;
	}

	for ( i = 0; i < MAX_HOST; i++ )
		pcdata->hosts[ i ] = NULL;

	pcdata->points = 0;
	pcdata->oxygen = 0;
	pcdata->spells_failed = NULL;
	pcdata->charm_list = NULL;
	pcdata->mage_specialist = -1;
	pcdata->account_email = &str_empty[ 0 ];

	pcdata->death_statistics.deaths = 0;
	pcdata->death_statistics.pkdeaths = 0;
	pcdata->death_statistics.suicides = 0;
	pcdata->death_statistics.mob_kills = 0;
	pcdata->death_statistics.player_kills = 0;
	pcdata->death_statistics.pkdeath_list = NULL;
	pcdata->death_statistics.pkills_list = NULL;

	pcdata->buffer = new_buf();
	pcdata->rolls_count=0;
	pcdata->new_rolls_count=0;
	pcdata->mind_in = NULL;
//Brohacz: bounty: przeniesienie na globalna liste
//	pcdata->bounty = 0;

	for ( i = 0; i < MAX_LEVEL; i++ )
    {
		pcdata->perm_hit_per_level[ i ] = 0;
    }

	VALIDATE( pcdata );
	return pcdata;
}

void free_pcdata( PC_DATA *pcdata )
{
	int alias, host;
	MURDER_LIST *tmp, *next;
	CHARM_DATA *charm, *charm_next;

	if ( !IS_VALID( pcdata ) )
		return ;

	free_string( pcdata->pwd );
	free_string( pcdata->tmp_pwd );
	free_string( pcdata->bamfin );
	free_string( pcdata->bamfout );
	free_string( pcdata->title );
	free_string( pcdata->ignore );
	free_string( pcdata->last_host );
	free_string( pcdata->name_deny_txt );
	free_buf( pcdata->buffer );
	free_string( pcdata->afk_text );
	free_string( pcdata->account_email );
	free_string( pcdata->new_title );

	for ( alias = 0; alias < MAX_ALIAS; alias++ )
	{
		free_string( pcdata->alias[ alias ] );
		free_string( pcdata->alias_sub[ alias ] );
	}

	for ( host = 0; host < MAX_HOST; host++ )
		free_string( pcdata->hosts[ host ] );
	pcdata->hosts_count = 0;

	INVALIDATE( pcdata );
	pcdata->next = pcdata_free;
	pcdata_free = pcdata;

	for ( tmp = pcdata->death_statistics.pkdeath_list; tmp; tmp = next )
	{
		next = tmp->next;
		free( tmp );
	}

	for ( tmp = pcdata->death_statistics.pkills_list; tmp; tmp = next )
	{
		next = tmp->next;
		free( tmp );
	}

	for ( charm = pcdata->charm_list; charm; charm = charm_next )
	{
		charm_next = charm->next;
		free( charm );
	}
	return ;
}

/* mob repair data */
REPAIR_DATA *repair_free = NULL;
REPAIR_DATA * new_repair()
{
    REPAIR_DATA *repair;

    if ( !repair_free )
    {
        CREATE( repair, REPAIR_DATA, 1 );
        top_shop++;
    }
    else
    {
        repair            =   repair_free;
        repair_free       =   repair_free->next;
    }

    repair->item_type 		= 0;
    repair->max_cost 		= 0;
    repair->repair_cost_mod	= 100;
    repair->min_condition	= 1;
    repair->next		    = NULL;
    repair->repair_room     = 0;

    return repair;
}
void free_repair( REPAIR_DATA * pRepair )
{
    pRepair->next = repair_free;
    repair_free   = pRepair;
    return;
}

/* stuff for setting ids */
long	last_pc_id;
long	last_mob_id;

long get_pc_id( void )
{
	int val;

	val = ( current_time <= last_pc_id ) ? last_pc_id + 1 : current_time;
	last_pc_id = val;
	return val;
}

long get_mob_id( void )
{
	last_mob_id++;
	return last_mob_id;
}

MEM_DATA *mem_data_free = NULL;
MEM_DATA *new_mem_data( void )
{
	MEM_DATA * memory;

	if ( mem_data_free == NULL )
		CREATE( memory, MEM_DATA, 1 );
	else
	{
		memory = mem_data_free;
		mem_data_free = mem_data_free->next;
	}

	memory->next = NULL;
	memory->who = NULL;
	memory->reaction = 0;
	VALIDATE( memory );

	return memory;
}

void free_mem_data( MEM_DATA *memory )
{
	if ( !IS_VALID( memory ) )
		return ;

	memory->next = mem_data_free;
	mem_data_free = memory;
	INVALIDATE( memory );
}

/* procedures and constants needed for buffering */

BUFFER *buf_free = NULL;

/* buffer sizes */
const int buf_size[ MAX_BUF_LIST ] =
    {
        16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768, 65536
    };

/* local procedure for finding the next acceptable size */
/* -1 indicates out-of-boundary error */
int get_size ( int val )
{
	int i;

	for ( i = 0; i < MAX_BUF_LIST; i++ )
		if ( buf_size[ i ] >= val )
		{
			return buf_size[ i ];
		}

	return -1;
}

BUFFER *new_buf()
{
	BUFFER * buffer;

	if ( buf_free == NULL )
		CREATE( buffer, BUFFER, 1 );
	else
	{
		buffer = buf_free;
		buf_free = buf_free->next;
	}

	buffer->next	= NULL;
	buffer->state	= BUFFER_SAFE;
	buffer->size	= get_size( BASE_BUF );

	CREATE( buffer->string, char, buffer->size );
	buffer->string[ 0 ] = '\0';
	VALIDATE( buffer );

	return buffer;
}

BUFFER *new_buf_size( int size )
{
	BUFFER * buffer;

	if ( buf_free == NULL )
		CREATE( buffer, BUFFER, 1 );
	else
	{
		buffer = buf_free;
		buf_free = buf_free->next;
	}

	buffer->next = NULL;
	buffer->state = BUFFER_SAFE;
	buffer->size = get_size( size );
	if ( buffer->size == -1 )
	{
		bug( "new_buf: buffer size %d too large.", size );
		exit( 1 );
	}
	CREATE( buffer->string, char, buffer->size );
	buffer->string[ 0 ] = '\0';
	VALIDATE( buffer );

	return buffer;
}


void free_buf( BUFFER *buffer )
{
	if ( !IS_VALID( buffer ) )
		return ;

	DISPOSE( buffer->string );
	buffer->string = NULL;
	buffer->size = 0;
	buffer->state = BUFFER_FREED;
	INVALIDATE( buffer );

	buffer->next = buf_free;
	buf_free = buffer;
}


bool add_buf( BUFFER *buffer, char *string )
{
	int len;
	char *oldstr;
	int oldsize;

	oldstr = buffer->string;
	oldsize = buffer->size;

	if ( buffer->state == BUFFER_OVERFLOW )  /* don't waste time on bad strings! */
		return FALSE;

	len = strlen( buffer->string ) + strlen( string ) + 1;

	while ( len >= buffer->size )  /* increase the buffer size */
	{
		buffer->size = get_size( buffer->size + 1 );

		{
			if ( buffer->size == -1 )  /* overflow */
			{
				buffer->size = oldsize;
				buffer->state = BUFFER_OVERFLOW;
				bug( "buffer overflow past size %d", buffer->size );
				return FALSE;
			}
		}
	}

	if ( buffer->size != oldsize )
	{
		CREATE( buffer->string, char, buffer->size );

		strcpy( buffer->string, oldstr );
		DISPOSE( oldstr );
	}

	strcat( buffer->string, string );
	return TRUE;
}



bool add_bufx( BUFFER *buffer, char *string )
{
	int len;
	char *oldstr;
	char *tmp = NULL;
	int oldsize;

	oldstr = buffer->string;
	oldsize = buffer->size;

	if ( buffer->state == BUFFER_OVERFLOW )  /* don't waste time on bad strings! */
		return FALSE;

	len = strlen( buffer->string ) + strlen( string ) + 1;

	while ( len >= buffer->size )  /* increase the buffer size */
	{
		buffer->size = get_size( buffer->size + 1 );
		{
			if ( buffer->size == -1 )  /* overflow */
			{
				buffer->size = oldsize;
				buffer->state = BUFFER_OVERFLOW;
				bug( "buffer overflow past size %d", buffer->size );
				return FALSE;
			}
		}
	}

	CREATE( tmp, char, buffer->size );
	strcpy( tmp, string );
	strcat( tmp, buffer->string );
	DISPOSE( oldstr );
	buffer->string = tmp;
	return TRUE;
}



void clear_buf( BUFFER *buffer )
{
	buffer->string[ 0 ] = '\0';
	buffer->state = BUFFER_SAFE;
}


char *buf_string( BUFFER *buffer )
{
	return buffer->string;
}

/* stuff for recycling mobprograms */
PROG_LIST *prog_free = NULL;

PROG_LIST *new_prog( void )
{
	static PROG_LIST mp_zero;
	PROG_LIST *mp;

	if ( prog_free == NULL )
		CREATE( mp, PROG_LIST, 1 );
	else
	{
		mp = prog_free;
		prog_free = prog_free->next;
	}

	*mp = mp_zero;
	mp->name = str_dup( "" );
	mp->trig_type = 0;
	mp->code = str_dup( "" );
	mp->valid_positions = FP_ACTIVE;
	mp->next = NULL;
	VALIDATE( mp );
	return mp;
}


void free_prog( PROG_LIST *mp )
{
	if ( !IS_VALID( mp ) ) return ;

	INVALIDATE( mp );
	mp->next = prog_free;
	prog_free = mp;
}


HELP_AREA * had_free = NULL;

HELP_AREA * new_had ( void )
{
	HELP_AREA * had;
	static	HELP_AREA zHad;

	if ( had_free )
	{
		had	= had_free;
		had_free	= had_free->next;
	}
	else
		CREATE( had, HELP_AREA, 1 );

	*had = zHad;

	return had;
}

HELP_DATA * help_free = NULL;

HELP_DATA * new_help ( void )
{
	HELP_DATA * help;

	if ( help_free )
	{
		help	= help_free;
		help_free	= help_free->next;
	}
	else
		CREATE( help, HELP_DATA, 1 );

	return help;
}

void free_help( HELP_DATA *help )
{
	free_string( help->keyword );
	free_string( help->text );
	help->next = help_free;
	help_free = help;
}

/*Brohacz: bounty*/
BOUNTY_DATA *bounty_free = NULL;
BOUNTY_DATA *new_bounty( void )
{
	static BOUNTY_DATA bounty_data_zero;
	BOUNTY_DATA *bounty;

	if ( bounty_free == NULL )
		CREATE( bounty, BOUNTY_DATA, 1 );
	else
	{
		bounty = bounty_free;
		bounty_free = bounty_free->next;
	}

	*bounty = bounty_data_zero;
	bounty->next = NULL;
	VALIDATE( bounty );
	return bounty;
}

void free_bounty( BOUNTY_DATA *bounty )
{
	if ( !IS_VALID( bounty ) ) return ;
	INVALIDATE( bounty );
	free_string( bounty->name );
	bounty->next = bounty_free;
	bounty_free = bounty;
}

/*pflagi*/
PFLAG_DATA *pflag_free = NULL;
PFLAG_DATA *new_pflag( void )
{
	static PFLAG_DATA pflag_data_zero;
	PFLAG_DATA *pflag;

	if ( pflag_free == NULL )
		CREATE( pflag, PFLAG_DATA, 1 );
	else
	{
		pflag = pflag_free;
		pflag_free = pflag_free->next;
	}

	*pflag = pflag_data_zero;
	pflag->next = NULL;
	VALIDATE( pflag );
	return pflag;
}

void free_pflag( PFLAG_DATA *pflag )
{
	if ( !IS_VALID( pflag ) ) return ;
	INVALIDATE( pflag );
	free_string( pflag->id );
	pflag->next = pflag_free;
	pflag_free = pflag;
}


/*pre wait sysem*/

PRE_WAIT *prewait_free = NULL;
PRE_WAIT *new_prewait( void )
{
	static PRE_WAIT pwait_zero;
	PRE_WAIT *pwait;

	if ( prewait_free == NULL )
		CREATE( pwait, PRE_WAIT, 1 );
	else
	{
		pwait = prewait_free;
		prewait_free = prewait_free->next;
	}

	*pwait = pwait_zero;
	pwait->next = NULL;
	VALIDATE( pwait );
	return pwait;
}

void free_prewait( PRE_WAIT *pwait )
{
	if ( !IS_VALID( pwait ) ) return ;
	INVALIDATE( pwait );
	free_string( pwait->spell_name );
	pwait->next = prewait_free;
	prewait_free = pwait;
}


/********************/
/********************/
PWAIT_CHAR *pwait_char_free = NULL;
PWAIT_CHAR *new_pwait_char( void )
{
	static PWAIT_CHAR pwait_char_zero;
	PWAIT_CHAR *pw_char;

	if ( pwait_char_free == NULL )
		CREATE( pw_char, PWAIT_CHAR, 1 );
	else
	{
		pw_char = pwait_char_free;
		pwait_char_free = pwait_char_free->next;
	}

	*pw_char = pwait_char_zero;
	pw_char->next = NULL;
	VALIDATE( pw_char );
	return pw_char;
}

void free_pwait_char( PWAIT_CHAR *pwait )
{
	if ( !IS_VALID( pwait ) ) return ;
	INVALIDATE( pwait );
	free_string( pwait->spell_name );
	pwait->next = pwait_char_free;
	pwait_char_free = pwait;
}

SPELL_FAILED *spell_failed_free = NULL;
SPELL_FAILED *new_spell_failed( void )
{
	static SPELL_FAILED sp_failed_zero;
	SPELL_FAILED *sp_failed;

	if ( spell_failed_free == NULL )
		CREATE( sp_failed, SPELL_FAILED, 1 );
	else
	{
		sp_failed = spell_failed_free;
		spell_failed_free = spell_failed_free->next;
	}

	*sp_failed = sp_failed_zero;
	sp_failed->next = NULL;
	VALIDATE( sp_failed );
	return sp_failed;
}

void free_spell_failed( SPELL_FAILED *sp_failed )
{
	if ( !IS_VALID( sp_failed ) )
		return ;

	INVALIDATE( sp_failed );
	sp_failed->next = spell_failed_free;
	spell_failed_free = sp_failed;
}


INFO_DESCRIPTOR_DATA *info_descriptor_free = NULL;

INFO_DESCRIPTOR_DATA *new_info_descriptor( void )
{
	static INFO_DESCRIPTOR_DATA id_zero;
	INFO_DESCRIPTOR_DATA *id;

	if ( info_descriptor_free == NULL )
		CREATE( id, INFO_DESCRIPTOR_DATA, 1 );
	else
	{
		id = info_descriptor_free;
		info_descriptor_free = info_descriptor_free->next;
	}

	*id = id_zero;

	id->descriptor = -1;
	id->input[ 0 ] = '\0';
	id->next = NULL;

	return id;
}

void free_info_descriptor( INFO_DESCRIPTOR_DATA *id )
{
	id->next = info_descriptor_free;
	info_descriptor_free = id;
}

/*herb*/
HERB_IN_ROOM_DATA *herb_room_free = NULL;
HERB_IN_ROOM_DATA *new_herb_room_data( void )
{
	static HERB_IN_ROOM_DATA herb_data_zero;
	HERB_IN_ROOM_DATA *herb;

	if ( herb_room_free == NULL )
		CREATE( herb, HERB_IN_ROOM_DATA, 1 );
	else
	{
		herb = herb_room_free;
		herb_room_free = herb_room_free->next;
	}

	*herb = herb_data_zero;
	VALIDATE( herb );
	return herb;
}

void free_herb_room_data( HERB_IN_ROOM_DATA *herb )
{
	if ( !IS_VALID( herb ) ) return ;
	INVALIDATE( herb );
	herb->next = herb_room_free;
	herb_room_free = herb;
}

NEW_CHAR_DATA *new_new_char_data( void )
{
	static NEW_CHAR_DATA new_char_data_zero;
	NEW_CHAR_DATA *new_char_data;

	CREATE( new_char_data, NEW_CHAR_DATA, 1 );

	*new_char_data = new_char_data_zero;
	new_char_data->mage_specialist = -1;
	new_char_data->name = &str_empty[ 0 ];
	new_char_data->name2 = &str_empty[ 0 ];
	new_char_data->name3 = &str_empty[ 0 ];
	new_char_data->name4 = &str_empty[ 0 ];
	new_char_data->name5 = &str_empty[ 0 ];
	new_char_data->name6 = &str_empty[ 0 ];
	new_char_data->password = &str_empty[ 0 ];
	new_char_data->description = &str_empty[ 0 ];

	return new_char_data;
}

void free_new_char_data( NEW_CHAR_DATA *new_char_data )
{
	free_string( new_char_data->name );
	free_string( new_char_data->name2 );
	free_string( new_char_data->name3 );
	free_string( new_char_data->name4 );
	free_string( new_char_data->name5 );
	free_string( new_char_data->name6 );
	free_string( new_char_data->password );
	free_string( new_char_data->description );
	DISPOSE(new_char_data);
}




//no dobra, przenios³em tutaj bo nie wiedzia³em gdzie :)
//rellik, dodanie do affectów pola które zlicza gdy postaæ niezalogowana

void add_new_affect_rt( OBJ_DATA *obj, int type, int loc, int mod, int level, BITVECT_DATA * bitv, int dur, int rtdur, bool last )
{
	AFFECT_DATA * pAf;

	if ( dur == 0 && rtdur == 0 )
		return;

	pAf	= new_affect();
	pAf->where	= TO_AFFECTS;
	pAf->type	= type;
	pAf->location = loc;
	pAf->modifier = mod;
	pAf->level	= level;
	pAf->bitvector = bitv;

	pAf->duration = dur;
	pAf->rt_duration = rtdur;
	obj->enchanted = TRUE;
	pAf->next = obj->affected;
	obj->affected = pAf;

	if ( !last )
		pAf = NULL;

	return;
}

void add_new_affect( OBJ_DATA *obj, int type, int loc, int mod, int level, BITVECT_DATA * bitv, int dur, bool last )
{
   add_new_affect_rt(obj,type,loc,mod,level,bitv,dur,0,last);
}
