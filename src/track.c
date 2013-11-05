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
 * KILLER MUD is copyright 1999-2012 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: track.c 11460 2012-06-29 07:43:35Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/track.c $
 *
 */
#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "magic.h"
#include "telnet.h"
#include "lang.h"
#include "track.h"
#define closesocket close

char * get_target_by_name(CHAR_DATA *ch, CHAR_DATA *victim);
OBJ_DATA* has_key		args( ( CHAR_DATA *ch, int key ) );
CHAR_DATA *get_char_area	args(( CHAR_DATA *ch, char *argument )

);
extern unsigned int top_vnum_room;

/*
 * Structure types.
 */
typedef	struct	hash_link		HASH_LINK;
typedef	struct	hash_header		HASH_HEADER;
typedef	struct	hunting_data		HUNTING_DATA;
typedef	struct	room_q			ROOM_Q;
typedef	struct	nodes			NODES;

/*
 * Hunting parameters.
 * Make sure area_last has the last room vnum!
 */
#define IS_DIR			( get_room_index( q_head->room_nr )->exit[i] )
#define GO_OK( ch, ex )	( !IS_SET( ex->exit_info, EX_ISDOOR ) || \
                          ( \
                           IS_SET( ex->exit_info, EX_ISDOOR ) && \
                           !IS_SET( ex->exit_info, EX_LOCKED ) ) || \
                          ( \
                           IS_SET( ex->exit_info, EX_ISDOOR ) && \
                           IS_SET( ex->exit_info, EX_LOCKED ) && \
                           has_key( ch, ex->key ) ) )
#define GO_OK_SMARTER		1
#define WORLD_SIZE		top_vnum_room
#define	HASH_KEY( ht, key )	((((unsigned int)(key))*17)%(ht)->table_size)


struct hash_link
{
	int			key;
	HASH_LINK		*next;
	void		*data;
};

struct hash_header
{
	int			rec_size;
	int			table_size;
	int			klistsize;
	int			klistlen;
	int			*keylist;    /* this is really lame, AMAZINGLY lame */
	HASH_LINK	**buckets;
};

struct hunting_data
{
	char		*name;
	CHAR_DATA	**victim;
};

struct room_q
{
	int			room_nr;
	ROOM_Q		*next_q;
};

struct nodes
{
	int visited;
	int ancestor;
};

void init_hash_table( HASH_HEADER *ht, int rec_size, int table_size )
{
	ht->rec_size	= rec_size;
	ht->table_size	= table_size;
	ht->buckets		= (void *) calloc( sizeof( HASH_LINK ** ), table_size );
	ht->keylist		= (void *) malloc( sizeof( ht->keylist ) * ( ht->klistsize = 128 ) );
	ht->klistlen	= 0;
}

void init_world( ROOM_INDEX_DATA *room_db[] )
{
	/* zero out the world */
	memset( room_db, 0, sizeof( ROOM_INDEX_DATA * ) * WORLD_SIZE );
	return;
}

void destroy_hash_table( HASH_HEADER *ht, void ( *gman ) ( ) )
{
	HASH_LINK *scan;
	HASH_LINK *temp;
	int        i;

	for ( i = 0; i < ht->table_size; i++ )
	for ( scan = ht->buckets[i]; scan; )
	{
		temp = scan->next;
		( *gman ) ( scan->data );
		free( scan );
		scan = temp;
	}

	free( ht->buckets );
	free( ht->keylist );
	return;
}

void _hash_enter( HASH_HEADER *ht, int key, void *data )
{
	/* precondition: there is no entry for <key> yet */
	HASH_LINK *temp;
	int        i;

	temp	= (HASH_LINK *) malloc( sizeof( HASH_LINK ) );
	temp->key	= key;
	temp->next	= ht->buckets[HASH_KEY( ht, key )];
	temp->data	= data;
	ht->buckets[HASH_KEY( ht, key )] = temp;
	if ( ht->klistlen >= ht->klistsize )
	{
	   ht->keylist = (void *) realloc( ht->keylist, sizeof( *ht->keylist ) *
					   ( ht->klistsize *= 2 ) );
	}
	for ( i = ht->klistlen; i >= 0; i-- )
	   {
	      if ( i == 0 || (ht->keylist[i - 1] < key) )
		 {
		    ht->keylist[i] = key;
		    break;
		 }
	      ht->keylist[i] = ht->keylist[i - 1];
	   }
	ht->klistlen++;
	return;
}

ROOM_INDEX_DATA *room_find( ROOM_INDEX_DATA *room_db[], int key )
{
	return ( ( key < WORLD_SIZE && key > -1 ) ? room_db[key] : 0 );
}

void *hash_find( HASH_HEADER *ht, int key )
{
	HASH_LINK *scan;

	scan = ht->buckets[HASH_KEY( ht, key )];

	while ( scan && scan->key != key )
	scan = scan->next;

	return scan ? scan->data : NULL;
}

int room_enter( ROOM_INDEX_DATA *rb[], int key, ROOM_INDEX_DATA *rm )
{
	ROOM_INDEX_DATA *temp;

	temp = room_find( rb, key );
	if ( temp )
	return ( 0 );

	rb[key] = rm;
	return ( 1 );
}

int hash_enter( HASH_HEADER *ht, int key, void *data )
{
	void *temp;

	temp = hash_find( ht, key );
	if ( temp )
	return 0;

	_hash_enter( ht, key, data );
	return 1;
}

ROOM_INDEX_DATA *room_find_or_create( ROOM_INDEX_DATA *rb[], int key )
{
	ROOM_INDEX_DATA *rv;

	rv = room_find( rb, key );
	if ( rv )
	return rv;

	rv = (ROOM_INDEX_DATA *) malloc( sizeof( ROOM_INDEX_DATA ) );
	rb[key] = rv;

	return rv;
}

void *hash_find_or_create( HASH_HEADER *ht, int key )
{
	void *rval;

	rval = hash_find( ht, key );
	if ( rval )
	return rval;

	rval = (void *) malloc( ht->rec_size );
	_hash_enter( ht, key, rval );

	return rval;
}

int room_remove( ROOM_INDEX_DATA *rb[], int key )
{
	ROOM_INDEX_DATA *tmp;

	tmp = room_find( rb, key );
	if ( tmp )
	{
	rb[key] = 0;
	free( tmp );
	}
	return ( 0 );
}

void *hash_remove( HASH_HEADER *ht, int key )
{
	HASH_LINK **scan;

	scan = ht->buckets + HASH_KEY( ht, key );

	while ( *scan && ( *scan )->key != key )
	scan = &( *scan )->next;

	if ( *scan )
	{
	HASH_LINK *temp;
	HASH_LINK *aux;
	int        i;

	temp = ( *scan )->data;
	aux = *scan;
	*scan = aux->next;
	free( aux );

	for ( i = 0; i < ht->klistlen; i++ )
		if ( ht->keylist[i] == key )
		break;

	if ( i < ht->klistlen )
	{
		memmove( ht->keylist + i, ht->keylist + i + 1, ( ht->klistlen - i )
		* sizeof( *ht->keylist ) );
		ht->klistlen--;
	}

	return temp;
	}

	return NULL;
}

void room_iterate( ROOM_INDEX_DATA *rb[], void ( *func ) ( ), void *cdata )
{
	register int i;

	for ( i = 0; i < WORLD_SIZE; i++ )
	{
	ROOM_INDEX_DATA *temp;

	temp = room_find( rb, i );
	if ( temp )
		( *func ) ( i, temp, cdata );
	}
}

void hash_iterate( HASH_HEADER *ht, void ( *func ) ( ), void *cdata )
{
	int i;

	for ( i = 0; i < ht->klistlen; i++ )
	{
	void          *temp;
	register int   key;

	key = ht->keylist[i];
	temp = hash_find( ht, key );
	( *func ) ( key, temp, cdata );
	if ( ht->keylist[i] != key )	/* They must have deleted this room */
		i--;		/* Hit this slot again. */
	}
}

int exit_ok( EXIT_DATA *pexit )
{
	ROOM_INDEX_DATA *to_room;

	if ( ( !pexit )
	|| !( to_room = pexit->u1.to_room ) )
	return 0;

	return 1;
}

void donothing( )
{
	return;
}

int find_path( int in_room_vnum, int out_room_vnum, CHAR_DATA *ch,
		int depth, int in_zone )
{
	ROOM_INDEX_DATA *herep;
	ROOM_INDEX_DATA *startp;
	EXIT_DATA       *exitp;
	ROOM_Q          *tmp_q;
	ROOM_Q          *q_head;
	ROOM_Q          *q_tail;
	HASH_HEADER      x_room;
	bool             thru_doors;
	long             i;
	int              tmp_room;
	int              count = 0;

	if ( depth < 0 )
	{
	thru_doors = TRUE;
	depth = -depth;
	}
	else
	{
	thru_doors = FALSE;
	}

	startp = get_room_index( in_room_vnum );

	init_hash_table( &x_room, sizeof( int ), 2048 );
	hash_enter( &x_room, in_room_vnum, (void *) - 1 );

	/* initialize queue */
	q_head = (ROOM_Q *) malloc( sizeof( ROOM_Q ) );
	q_tail = q_head;
	q_tail->room_nr = in_room_vnum;
	q_tail->next_q = 0;

	while ( q_head )
	{
	herep = get_room_index( q_head->room_nr );
	/* for each room test all directions */
	if ( ( SAME_AREA( herep->area, startp->area ) && herep->area_part == startp->area_part ) || !in_zone )
	{
		/*
		* only look in this zone...
		* saves cpu time and makes world safer for players
		*/
		for ( i = 0; i < MAX_DIR; i++ )
		{
			exitp = herep->exit[i];
			if ( exit_ok( exitp ) && ( thru_doors ? GO_OK_SMARTER : GO_OK( ch, exitp ) ) )
			{
				/* next room */
				tmp_room = herep->exit[i]->u1.to_room->vnum;
				if ( tmp_room != out_room_vnum )
				{
				/*
				* shall we add room to queue ?
				* count determines total breadth and depth
				*/
				if ( !hash_find( &x_room, tmp_room )
					&& ( count < depth ) )
					/* && !IS_SET( RM_FLAGS(tmp_room), DEATH ) ) */
				{
					count++;
					/* mark room as visted and put on queue */

					tmp_q = (ROOM_Q *) malloc( sizeof( ROOM_Q ) );
					tmp_q->room_nr = tmp_room;
					tmp_q->next_q = 0;
					q_tail->next_q = tmp_q;
					q_tail = tmp_q;

					/* ancestor for first layer is the direction */
					hash_enter( &x_room, tmp_room,
					( (long) hash_find( &x_room, q_head->room_nr ) == -1 )
					? (void *) ( i + 1 )
					: hash_find( &x_room, q_head->room_nr ) );
				}
				}
				else
				{
					/* have reached our goal so free queue */
					tmp_room = q_head->room_nr;
					for ( ; q_head; q_head = tmp_q )
					{
						tmp_q = q_head->next_q;
						free( q_head );
					}
					/* return direction if first layer */
					if ( (long) hash_find( &x_room, tmp_room ) == -1 )
					{
						if ( x_room.buckets )
						{
						/* junk left over from a previous track */
						destroy_hash_table( &x_room, donothing );
						}
						return ( i );
					}
					else
					{
						/* else return the ancestor */
						long i;

						i = (long) hash_find( &x_room, tmp_room );
						if ( x_room.buckets )
						{
						/* junk left over from a previous track */
						destroy_hash_table( &x_room, donothing );
						}
						return ( -1 + i );
					}
				}
			}
		}
	}

	/* free queue head and point to next entry */
	tmp_q = q_head->next_q;
	free( q_head );
	q_head = tmp_q;
	}

	/* couldn't find path */
	if ( x_room.buckets )
	{
	/* junk left over from a previous track */
	destroy_hash_table( &x_room, donothing );
	}
	return -1;
}



/* ciut przerobione */
void found_prey( CHAR_DATA *ch, CHAR_DATA *victim )
{
	OBJ_DATA *weapon;
	char * target;

	if ( !victim )
	{
	bug( "Found_prey: null victim", 0 );
	return;
	}

	if ( !victim->in_room )
	{
		bug( "Found_prey: null victim->in_room", 0 );
		return;
	}

	if ( !can_see( ch, victim ) )
	return;

	if ( is_safe( ch, victim, TRUE ) )
	return;

	stop_hunting( ch );

	if( (weapon = get_eq_char(ch, WEAR_WIELD)) != NULL )
	{
		if( (target = get_target_by_name(ch,victim)) == NULL)
		target = victim->name;

	if(weapon->value[0] == WEAPON_DAGGER && get_skill(ch, gsn_backstab) > 0)
	{
		do_function(ch, &do_backstab, target );
	}
		else if ( get_skill(ch, gsn_charge) > 0 && IS_WEAPON_STAT( weapon, WEAPON_TWO_HANDS ) && weapon->value[0] != WEAPON_WHIP && weapon->value[0] != WEAPON_SHORTSWORD && weapon->value[0] != WEAPON_DAGGER )
	{
		do_function(ch, &do_charge, target );
	}
	else
		multi_hit( ch, victim, TYPE_UNDEFINED );
	return;
	}

	multi_hit( ch, victim, TYPE_UNDEFINED );
	return;
}

void do_track( CHAR_DATA *ch, char *argument )
{
	char       buf [ MAX_STRING_LENGTH ];
	char       arg [ MAX_STRING_LENGTH ];
	CHAR_DATA *victim;
	int        direction, skill;
	bool       fArea=TRUE;

	skill=get_skill(ch, gsn_track);

	if (IS_NPC(ch))
	{
		return;
	}

	if ( ch->mount )
	{
		send_to_char( "Najpierw zsi±d¼ ze swojego wierzchowca.\n\r", ch );
		return;
	}

	if ( !IS_IMMORTAL( ch ) && IS_AFFECTED( ch, AFF_BLIND ) )
	{
		act("$n klêka i bezradnie potrz±sa g³ow±.",ch,NULL,NULL,TO_ROOM);
		send_to_char("Klekasz, ale przecie¿ niczego nie widzisz.\n\r",ch);
		return;
	}
	if ( room_is_dark( ch, ch->in_room ) &&  !(IS_AFFECTED( ch, AFF_DARK_VISION ) || IS_IMMORTAL( ch ) ))
	{
		send_to_char("Klekasz, ale jest tu tak ciemno, ¿e niczego nie widzisz.\n\r",ch);
		return;
	}

	// wycinamy niektóre ze sektorów
	switch ( ch->in_room->sector_type )
	{
		case  6:  //woda p³yw
		case  7:  //woda niep
		case 19:  //rzeka
		case 20:  //jezioro
		case 21:  //morze
		case 22:  //ocean
		case 23:  //lawa
		case 24:  //ruchome piaski
		case 25:  //gor±ce ¼ród³o
		case 34:  //pod wod±
		case 37:  //ciemna woda
		case 39:  //podziemne jezioro
			send_to_char( "Tutaj chyba nie da siê szukaæ ¶ladów.\n\r", ch );
			return;
	}

	// komunikaty dla tych, którzy nie umiej± szukaæ ¶ladów
	if ( skill == 0 )
	{
		switch (number_range(1,7))
		{
			case 1:
				act("$n klêka i nie wie co robiæ dalej.",ch,NULL,NULL,TO_ROOM);
				send_to_char("Klêkasz i nie za bardzo wiesz co robiæ dalej.\n\r",ch);
				break;
			case 2:
				act("$n klêka i niczego nie widzi.",ch,NULL,NULL,TO_ROOM);
				send_to_char("Klêkasz i niczego nie widzisz.\n\r",ch);
				break;
			case 3:
				act("$n mocno tupie, klêka i przygl±da siê ¶ladom.",ch,NULL,NULL,TO_ROOM);
				send_to_char("Mocno tupiesz, klêkasz i przygl±dasz siê ¶ladom.\n\r",ch);
				break;
			case 4:
				act("$n klêka i ¶ledzi tropy, ale nie ma o tym zielonego pojêcia.",ch,NULL,NULL,TO_ROOM);
				send_to_char("Klêkasz i ¶ledzisz tropy, choæ nie masz o tym zielonego pojêcia.\n\r",ch);
				break;
			case 5:
				act("$n klêka i robi nieszczê¶liw± minê.",ch,NULL,NULL,TO_ROOM);
				send_to_char("Klêkasz i robisz nieszczê¶liw± minê.\n\r",ch);
				break;
			case 6:
				act("$n rozgl±da siê dooko³a.",ch,NULL,NULL,TO_ROOM);
				send_to_char("Rozgl±dasz siê dooko³a, ale to nie pomaga.\n\r",ch);
				break;
			case 7:
				act("$n zastanawia siê nad tym, ¿e tropienie to niez³a umiejêtno¶æ.",ch,NULL,NULL,TO_ROOM);
				send_to_char("Zastanawiasz siê nad tym, ¿e tropienie to niez³a umiejêtno¶æ.\n\r",ch);
				break;
			default :
				send_to_char( "Nie masz pojêcia jak to siê robi.\n\r", ch );
				act("$n klêka i ¶ledzi tropy, ale nie ma o tym zielonego pojêcia.",ch,NULL,NULL,TO_ROOM);
				break;
		}
		return;
	}

	// bonus/kara od dobrej lokacji s± wiêksze ni¿ od z³ej
	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_CONSECRATE ))
	{
		if (IS_EVIL( ch ) ) skill -= number_range(1,5);
		if (IS_GOOD( ch ) ) skill += number_range(1,5);
	}

	if ( EXT_IS_SET( ch->in_room->room_flags, ROOM_DESECRATE ))
	{
		if (IS_EVIL( ch ) ) skill += number_range(0,4);
		if (IS_GOOD( ch ) ) skill -= number_range(0,4);
	}

	// sprawdzamy czy skill nie zrobi³ siê nam mniejszy ni¿ 1
	if (skill < 1)
	{
		skill = 1;
	}

	// no to zaczynamy w³a¶ciwe trackowanie :D
	one_argument( argument, arg );

#ifdef ENABLE_NEW_TRACK
	if(MAX_TRACK_LEN > 0)
	{
		track_new(ch, skill, argument);
		return;
	}
#endif

	if ( arg[0] == '\0' )
	{
		act("$n klêka i zaczyna z uwag± przygl±daæ siê ¶ladom.",ch,NULL,NULL,TO_ROOM);
		act("Klêkasz i z uwag± przygl±dasz siê ¶ladom.",ch,NULL,NULL,TO_CHAR);
		return;
	}

	if ( fArea )
	{
		victim = get_char_area( ch, arg );
	}
	else
	{
		victim = get_char_world( ch, arg );
	}

	if ( !victim )
	{
		act("$n klêka i zaczyna z uwag± przygl±daæ siê ¶ladom.",ch,NULL,NULL,TO_ROOM);
		act("Klêkasz, przygl±dasz siê ¶ladom, ale nie znajdujesz ¿adnego tropu.",ch,NULL,NULL,TO_CHAR);
		WAIT_STATE( ch, skill_table[gsn_track].beats );
		return;
	}

	if(victim==ch)
	{
		act("Zastanawiasz siê przez chwilê czy to ma sens, no có¿...",ch,NULL,NULL,TO_CHAR);
		return;
	}

	if ( ch->in_room == victim->in_room )
	{
		act( "Klêkasz i badasz trop, po chwili zdajesz sobie sprawê, ¿e $N stoi ko³o ciebie.", ch, NULL, victim, TO_CHAR );
		act("$n klêka i zaczyna z uwag± przygl±daæ siê ¶ladom.",ch,NULL,NULL,TO_ROOM);
		return;
	}

	if ( skill < number_range( 20, 40 ) )
	{
		act("$n klêka i zaczyna z uwag± przygl±daæ siê ¶ladom.",ch,NULL,NULL,TO_ROOM);
		act("Klêkasz, przygl±dasz siê ¶ladom, ale nie znajdujesz ¿adnego tropu.",ch,NULL,NULL,TO_CHAR);
		WAIT_STATE( ch, skill_table[gsn_track].beats );
		check_improve(ch, NULL, gsn_track, FALSE, 12);
		return;
	}

	if ( IS_AFFECTED ( victim, AFF_SPIRIT_WALK ) )
	{
		act("$n klêka i zaczyna z uwag± przygl±daæ siê ¶ladom.",ch,NULL,NULL,TO_ROOM);
		act("Klêkasz, przygl±dasz siê ¶ladom, ale nie znajdujesz ¿adnego tropu.",ch,NULL,NULL,TO_CHAR);
		WAIT_STATE( ch, skill_table[gsn_track].beats );
		return;
	}
	else
	{
		act("$n klêka i zaczyna z uwag± przygl±daæ siê ¶ladom.",ch,NULL,NULL,TO_ROOM);
		act("Klêkasz i przygladasz siê ¶ladom, chyba widzisz jaki¶ trop.",ch,NULL,NULL,TO_CHAR);
	}

	WAIT_STATE( ch, skill_table[gsn_track].beats );


	direction = find_path( ch->in_room->vnum, victim->in_room->vnum, ch, -40000, fArea );

	if ( direction == -1 )
	{
		act( "Niestety, nie mo¿esz nic wiêcej odczytaæ ze ¶ladów.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( direction < 0 || direction >= MAX_DIR )
	{
		send_to_char( "Hmm... co¶ jest nie tak.\n\r", ch );
		return;
	}

/*
 * Give a random direction if the player misses the die roll.
 */
	if ( number_percent() > skill )
	{
		do
		{
			direction = number_door( );
		}
		while (   !( ch->in_room->exit[direction] ) || !( ch->in_room->exit[direction]->u1.to_room ) );
	}

	check_improve(ch, NULL, gsn_track, TRUE, 9);

/*
 * Display the results of the search.
 */


	sprintf( buf, "Znajdujesz ¶lady $Z prowadz±ce na %s.", direction == 4 ? "górê" : dir_name[direction] );
	act( buf, ch, NULL, victim, TO_CHAR );
	return;
}

void hunt_victim( CHAR_DATA *ch )
{
	CHAR_DATA *tmp;
	int        dir,chance;
	bool       found;

    if ( !ch || !ch->hunting )
    {
        return;
    }
    /*
     * Make sure the victim still exists.
     */
    for ( found = FALSE, tmp = char_list; tmp && !found; tmp = tmp->next )
    {
        if ( ch->hunting == tmp )
        {
            found = TRUE;
        }
    }

    if ( !can_move( ch) || !found || !can_see( ch, ch->hunting ) )
    {
        stop_hunting( ch );
        return;
    }


    if ( ch->in_room == ch->hunting->in_room )
    {
        if ( ch->fighting )
            return;

        found_prey( ch, ch->hunting );
        return;
    }

    if ( !IS_NPC ( ch ) )
    {
        WAIT_STATE( ch, skill_table[gsn_track].beats );
    }
	dir = find_path( ch->in_room->vnum, ch->hunting->in_room->vnum, ch, -40000, TRUE );

	if ( dir < 0 || dir >= MAX_DIR )
	{
	stop_hunting( ch );
	return;
	}

	/*
	* Give a random direction if the mob misses the die roll.
    *
    * 20% for PC
    *  2% for NPC
	*/
    chance = IS_NPC ( ch ) ? 2 : 20;
    if ( number_percent( ) < chance + 1 )
    {
        do
        {
            dir = number_door( );
        }
        while (   !( ch->in_room->exit[dir] )
                || !( ch->in_room->exit[dir]->u1.to_room ) );
    }


	if ( IS_SET(ch->in_room->exit[dir]->exit_info, EX_SECRET) &&
		!IS_SET(ch->in_room->exit[dir]->exit_info, EX_HIDDEN) &&
        IS_AFFECTED(ch, AFF_DETECT_HIDDEN) )
    {
        //	do_secret(ch, (char *)dir_name[dir]);
        return;
    }

	if ( IS_SET( ch->in_room->exit[dir]->exit_info, EX_CLOSED ) )
	{
	do_open( ch, (char *) dir_name[dir] );
	return;
	}

	if(	IS_SET(sector_table[ch->in_room->exit[dir]->u1.to_room->sector_type].flag, SECT_UNDERWATER)
	||	IS_SET(ch->in_room->exit[dir]->exit_info, EX_NO_MOB)
	||	EXT_IS_SET(ch->in_room->exit[dir]->u1.to_room->room_flags, ROOM_NO_MOB)
	||  ( EXT_IS_SET(ch->act, ACT_STAY_AREA)
		&&   ch->in_room->exit[dir]->u1.to_room->area != ch->in_room->area )
	||	( EXT_IS_SET(ch->act, ACT_STAY_SECTOR)
		&&  ch->in_room->exit[dir]->u1.to_room->sector_type != ch->in_room->sector_type )
	||	( EXT_IS_SET(ch->act, ACT_OUTDOORS)
	&&   EXT_IS_SET(ch->in_room->exit[dir]->u1.to_room->room_flags,ROOM_INDOORS))
	||	( EXT_IS_SET(ch->act, ACT_INDOORS)
	&&   EXT_IS_SET(ch->in_room->exit[dir]->u1.to_room->room_flags,ROOM_INDOORS)))
		return;

	move_char( ch, dir, FALSE, NULL );

	if ( !ch->hunting )
	{
	if ( !ch->in_room )
	{
		char buf [ MAX_STRING_LENGTH ];

		sprintf( buf, "Hunt_victim: no ch->in_room!  Mob #%d, name: %s.  Placing mob in limbo (char_to_room).",
			ch->pIndexData->vnum, ch->name );
		bug( buf, 0 );
		char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
		return;
	}
	return;
	}

	if ( ch->in_room == ch->hunting->in_room )
	found_prey( ch, ch->hunting );

	return;
}

void char_walk( CHAR_DATA *ch )
{
	int        dir = -1;
	bool close = FALSE;
	bool lock = FALSE;
	ROOM_INDEX_DATA *prev_room;

	if ( !ch || ch->walking == 0 )
	return;

	if ( ch->in_room->vnum == ch->walking || ch->in_room->vnum == -1 * ch->walking )
	{
		ch->walking = 0;
		return;
	}

	if ( ch->walking > 0 )
	dir = find_path( ch->in_room->vnum, ch->walking, ch, 40000, TRUE );
	else if ( ch->walking < 0 )
	dir = find_path( ch->in_room->vnum, -1 * ch->walking, ch, 40000, FALSE );

	if ( dir < 0 || dir >= MAX_DIR )
	{
		ch->walking = 0;
		return;
	}

	if ( IS_SET( ch->in_room->exit[dir]->exit_info, EX_LOCKED ) )
	{
		do_unlock( ch, (char *) eng_dir_name[dir] );
		lock = TRUE;
	}

	if ( IS_SET( ch->in_room->exit[dir]->exit_info, EX_CLOSED ) )
	{
		do_open( ch, (char *) eng_dir_name[dir] );
		close = TRUE;
	}

	if ( IS_SET( sector_table[ch->in_room->exit[dir]->u1.to_room->sector_type].flag, SECT_UNDERWATER ) )
		return;

	prev_room = ch->in_room;
	move_char( ch, dir, FALSE, NULL );

	if ( ch->in_room->vnum == ch->walking || ch->in_room->vnum == -1 * ch->walking )
	ch->walking = 0;

	if ( ch->in_room == prev_room )
	return;

	if ( close )
	do_close( ch, (char *) eng_dir_name[rev_dir[dir]] );

	if ( lock )
	do_lock( ch, (char *) eng_dir_name[rev_dir[dir]] );

	return;
}

//http://code.killer.mud.pl/p/killer-mud/issues/5/
void track_new(CHAR_DATA *ch, int skill, char *argument)
{
	char       buf [ MAX_STRING_LENGTH ];

	char *messages_czas1[] =
	{
			"¶wie¿e",
			"¶wie¿e",
			"lekko zatarte",
			"zatarte",
			"zatarte",
			"zatarte",
			"ledwo rozpoznawalne",
			"ledwo rozpoznawalne"
	};

	char *messages_czas2[] =
	{
			"¶wie¿e",
			"¶wie¿e",
			"lekko zatarte",
			"zatarte",
			"zatarte",
			"mocno zatarte",
			"ledwo rozpoznawalne",
			"ledwo rozpoznawalne"
	};

	char *messages_czas3[] =
	{
			"bardzo ¶wie¿e",
			"ca³kiem ¶wie¿e",
			"bardzo lekko zatarte",
			"lekko zatarte",
			"zatarte",
			"mocno zatarte",
			"ledwo rozpoznawalne",
			"prawie nierozpoznawalne"
	};

	TRACK_DATA* td = ch->in_room->track_data;
	if(!td)
	{
		act("$n klêka i zaczyna z uwag± przygl±daæ siê ¶ladom.",ch,NULL,NULL,TO_ROOM);
		act("Klêkasz, przygl±dasz siê ¶ladom, ale nie znajdujesz ¿adnych tropów.",ch,NULL,NULL,TO_CHAR);
		WAIT_STATE( ch, skill_table[gsn_track].beats );
		return;
	}

	//cleanup_track_data(ch->in_room);

	int track_cnt =
		URANGE(
				1,
				(
						(number_range(1, MAX_TRACK_LEN) * skill)/100
						+
						(number_range(1, MAX_TRACK_LEN / 6) * get_curr_stat(ch,STAT_INT)/6)/OLD_28_VALUE/6
				)
				,
				MAX_TRACK_LEN
		);
	int i = 0;
	int succ = 0;

//	if(debug ==1) bugf("track_cnt %d", track_cnt);

	while(td)
	{
		i++;
		if(i > track_cnt)
			break;

		if ((number_range(1, 100) > skill/20) && is_name( argument, td->ch->ss_data ? td->ch->short_descr : td->ch->name ) )
		{
			int roznica = current_time - td->czas;
//			if(debug ==1) bugf("roznica %d", roznica);
			//skill = 1 - 36 sekund, 75 - 3600s - im lepszy skill, tym wiêcej ¶ladów (czas)
			if(roznica / 96 <= skill)
			{
				int czas_message = URANGE(0, roznica / 400, 7) ;

				char* vdir_name = td->kierunek == -1 ? "do nik±d" : (td->inout == 0 ? dir_name2[td->kierunek] : dir_name_dir[td->kierunek]);
				char* vinout = td->inout == 0 ? ", prowadz±ce tu," : ", wychodz±ce st±d,";
				//skill 0 do 95
				//skill/27 0 do 3.52
				//kolejno: 0 27 53 81 inf
				switch(skill/27)
				{
					case 0:
						sprintf( buf, "Znajdujesz jakie¶ ¶lady $Z.");
						break;
					case 1:
						sprintf( buf, "Znajdujesz %s ¶lady $Z.", messages_czas1[czas_message]);
						break;
					case 2:
						sprintf( buf, "Znajdujesz %s ¶lady $Z.", messages_czas2[czas_message]);
						break;
					case 3:
						sprintf( buf, "Znajdujesz %s%s ¶lady $Z prowadz±ce %s.", messages_czas3[czas_message], vinout, vdir_name );
						break;
					default:
						sprintf( buf, "Znajdujesz %s%s ¶lady $Z prowadz±ce %s.", messages_czas3[czas_message], vinout, vdir_name );
						break;
				}
				succ=1;
				act( buf, ch, NULL, td->ch, TO_CHAR );
			}
		}
		td = td->previous;
	}
	if(succ == 0)
	{
		act("$n klêka i zaczyna z uwag± przygl±daæ siê ¶ladom.",ch,NULL,NULL,TO_ROOM);
		act("Klêkasz, przygl±dasz siê ¶ladom, ale nie znajdujesz ¿adnych tropów.",ch,NULL,NULL,TO_CHAR);
		check_improve(ch, NULL, gsn_track, FALSE, 12);
		WAIT_STATE( ch, skill_table[gsn_track].beats );
		return;
	}
	check_improve(ch, NULL, gsn_track, TRUE, 9);
	WAIT_STATE( ch, skill_table[gsn_track].beats );
}

int get_door_index(
		ROOM_INDEX_DATA *from_room,
		ROOM_INDEX_DATA *to_room
)
{
	if (!from_room)
		return -1;

	if (!to_room)
		return -1;

	int vExit = 0;
    for(vExit = 0; vExit < MAX_EXIT_COUNT; vExit++)
	{
		if(!to_room->exit[vExit])
			continue;

		if(!to_room->exit[vExit]->u1.to_room)
			continue;

		if(to_room->exit[vExit]->u1.to_room == from_room)
		{
			return vExit;
		}
	}
    return -1;
}



void save_track_data(CHAR_DATA *ch, ROOM_INDEX_DATA *from_room,
		ROOM_INDEX_DATA *to_room, sh_int inout)
{
	//je¶li wy³±czone, to nie zapisujemy
	if(MAX_TRACK_LEN == 0)
		return;

	if (!ch)
		return;

	if (is_affected(ch, gsn_float) || is_affected(ch, gsn_fly))
		return;

	if (!from_room)
		return;

	if (!to_room)
		return;

	TRACK_DATA *td_last = (TRACK_DATA *) malloc(sizeof(*td_last));
    int vJak = 0;
    if(ch->mounting)
        vJak = 1;

    td_last->ch = ch;
    td_last->czas = current_time;
    td_last->jak = vJak;
    td_last->kierunek = get_door_index(from_room, to_room);
    td_last->previous = to_room->track_data;
    td_last->inout = inout;
    to_room->track_data = td_last;

	cleanup_track_data(to_room);
}

void cleanup_track_data(ROOM_INDEX_DATA *room)
{
	if(!room)
		return;

	if(!room->track_data)
		return;

	int cnt = 0;

	TRACK_DATA *dummy = room->track_data;
	//okre¶lenie d³ugo¶ci kolejki
	while (dummy->previous)
	{
		dummy = dummy->previous;
		cnt++;
	}

	//je¶li wiêcej ni¿ MAX_TRACK_LEN, to usuwanie pierwszego
	if (cnt > MAX_TRACK_LEN)
	{
		//usuwamy tyle, o ile jest za du¿o - je¶li nic siê nie sypnie,
		//to powinno byæ zawsze 1
		int over = cnt - MAX_TRACK_LEN;
		int i = 0;
		for (i = 0; i < over; i++)
		{
			dummy = room->track_data;
			if (dummy)
			{
				room->track_data = dummy->previous;
				free(dummy);
			}
			else
			{
				//je¶li lista pusta, to przerwanie pêtli...
				break;
			}
		}
	}
}
