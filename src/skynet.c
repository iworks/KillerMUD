#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "magic.h"
#include "interp.h"


void set_mem( CHAR_DATA *ch, sh_int sn, sh_int count );
void mcast( CHAR_DATA *ch, CHAR_DATA *victim, sh_int sn, bool self );
sh_int get_spell_type( CHAR_DATA *ch, int type );
int get_mem_slots( CHAR_DATA *ch, int circle, bool free );
sh_int get_caster( CHAR_DATA *ch );
sh_int spells_si( CHAR_DATA *ch );
void forget( CHAR_DATA *mob, CHAR_DATA *who, int reaction, bool All );
void remember( CHAR_DATA *mob, CHAR_DATA *who, int reaction );
void mem_update_count ( CHAR_DATA *ch );

/*
 * wypelnia sloty zaklec dla ladowanych mobow
 */
void fill_slots( CHAR_DATA *mob )
{
	int sn = -1, free_spells = 0;
	int circle, caster = -1, x;
	bool offensive = FALSE, cleric_mem = FALSE;
	int spells_tab[ 10 ][ 2 ];
	sh_int cure_tab[ 4 ][ 2 ];

	if ( !IS_NPC( mob ) )
		return ;

	caster = get_caster( mob );

	/* imbuowany mobik, czyli taki koles co moze w jakis sposob
	 * rzucic raz jakies zaklecie
	 */
	if ( caster < 0 )
	{
		for ( x = 0;x < 16 && mob->pIndexData->spells[ x ] != 0 ; x++ )
			set_mem( mob, mob->pIndexData->spells[ x ], 1 );

		return ;
	}

	/* dodatkowa tablica [kregi][spelle ilosc/spelle wykorzystane]
	 * efektywniej bedzie zapelnial slociki, hmm inteligetniej?
	 */
	for ( x = 0;x < 10;x++ )
	{
		spells_tab[ x ][ 0 ] = 0;
		spells_tab[ x ][ 1 ] = 0;
	}

	/*
	 * podobna sprawa dla leczen, osobna kategoria bo to dosc istotne
	 */
	for ( x = 0;x < 4;x++ )
	{
		cure_tab[ x ][ 0 ] = 0;
		cure_tab[ x ][ 1 ] = 0;
	}


	/*
	 * wstawka do poprawnej ilosci zapamietywanych spelli typu leczacego
	 */
	if ( caster == 1 && ( number_percent() < 33 ) )
	{
		cleric_mem = TRUE;

		for ( x = 0;x < 16 && mob->pIndexData->spells[ x ] != 0 ; x++ )
		{
			sn = mob->pIndexData->spells[ x ];

			if ( skill_table[ sn ].skill_level[ caster ] > mob->level )
				continue;

			if ( skill_table[ sn ].name[ 0 ] == 'c' && skill_table[ sn ].name[ 1 ] == 'u' )
			{
				if ( !str_prefix( "cure li", skill_table[ sn ].name ) )
				{
					cure_tab[ 0 ][ 0 ] = get_mem_slots( mob, spell_circle( mob, sn ), TRUE );
					cure_tab[ 0 ][ 1 ] = sn;
					continue;
				}
				else if ( !str_prefix( "cure ser", skill_table[ sn ].name ) )
				{
					cure_tab[ 1 ][ 0 ] = get_mem_slots( mob, spell_circle( mob, sn ), TRUE );
					cure_tab[ 1 ][ 1 ] = sn;
					continue;
				}
				else if ( !str_prefix( "cure cri", skill_table[ sn ].name ) )
				{
					cure_tab[ 2 ][ 0 ] = get_mem_slots( mob, spell_circle( mob, sn ), TRUE );
					cure_tab[ 2 ][ 1 ] = sn;
					continue;
				}
			}
			else if ( skill_table[ sn ].name[ 0 ] == 'h' && skill_table[ sn ].name[ 1 ] == 'e' &&
			          skill_table[ sn ].name[ 2 ] == 'e' && skill_table[ sn ].name[ 3 ] == 'l' )
			{
				cure_tab[ 3 ][ 0 ] = get_mem_slots( mob, spell_circle( mob, sn ), TRUE );
				cure_tab[ 3 ][ 1 ] = sn;
				continue;
			}
			else
				continue;
		}

		/* od 3 w dol, lepsze maja wiekszy priorytet*/
		for ( x = 3; x >= 0; x-- )
		{
			int tmp = 0;

			if ( cure_tab[ x ][ 0 ] > 0 && cure_tab[ x ][ 1 ] > 0 )
			{
				set_mem( mob, cure_tab[ x ][ 1 ], cure_tab[ x ][ 0 ] );
				cure_tab[ x ][ 1 ] = 0;
				tmp++;

				if ( tmp >= 2 )
					break;
			}
		}
	}


	/* inicjacja dodatkowej tablicy */
	for ( x = 0;x < 16 && mob->pIndexData->spells[ x ] != 0 ; x++ )
	{
		sn = mob->pIndexData->spells[ x ];

		if ( skill_table[ sn ].skill_level[ caster ] > mob->level )
			continue;

		if ( cleric_mem &&
		     ( cure_tab[ 0 ][ 1 ] == sn || cure_tab[ 1 ][ 1 ] == sn ||
		       cure_tab[ 2 ][ 1 ] == sn || cure_tab[ 3 ][ 1 ] == sn ) )
			continue;

		circle = spell_circle( mob, sn );
		if (1 <= circle && circle <= 10)
			spells_tab[ circle - 1 ][ 0 ] ++;
	}


	for ( x = 0;x < 16 && mob->pIndexData->spells[ x ] != 0 ; x++ )
	{
		sn = mob->pIndexData->spells[ x ];

		if ( skill_table[ sn ].skill_level[ caster ] > mob->level )
			continue;

		if ( cleric_mem &&
		     ( cure_tab[ 0 ][ 1 ] == sn || cure_tab[ 1 ][ 1 ] == sn ||
		       cure_tab[ 2 ][ 1 ] == sn || cure_tab[ 3 ][ 1 ] == sn ) )
			continue;

		circle = spell_circle( mob, sn );
		free_spells = get_mem_slots( mob, circle, TRUE );


		if ( free_spells <= 0 )
			continue;

		if ( skill_table[ sn ].target == TAR_CHAR_OFFENSIVE ||
		     skill_table[ sn ].target == TAR_OBJ_CHAR_OFF )
			offensive = TRUE;
		else
			offensive = FALSE;



		if ( spells_tab[ circle - 1 ][ 0 ] == 1 || spells_tab[ circle - 1 ][ 1 ] == spells_tab[ circle - 1 ][ 0 ] - 1 )
			set_mem( mob, sn, free_spells );
		else
		{
			int val = 0;
			spells_tab[ circle - 1 ][ 1 ] ++;

			if ( offensive )
				val = number_range( 2, 4 );
			else
				val = number_range( 1, 2 );

			if ( val > free_spells )
				val = free_spells;

			set_mem( mob, sn, val );
		}
	}

	return ;
}

/* HUNT HATE FEAR */

bool is_hunting( CHAR_DATA *ch, CHAR_DATA *victim )
{
	if ( !ch->hunting || ch->hunting != victim )
		return FALSE;

	return TRUE;
}

bool is_hating( CHAR_DATA *ch, CHAR_DATA *victim )
{
	MEM_DATA * tmp;

	if ( !ch || !ch->memory || !victim )
		return FALSE;


	for ( tmp = ch->memory;tmp;tmp = tmp->next )
		if ( tmp->who == victim && tmp->reaction == MEM_HATE )
			return TRUE;

	return FALSE;
}

bool is_fearing( CHAR_DATA *ch, CHAR_DATA *victim )
{
	MEM_DATA * tmp;

	if ( !ch || !ch->memory || !victim )
		return FALSE;


	for ( tmp = ch->memory;tmp;tmp = tmp->next )
		if ( tmp->who == victim && tmp->reaction == MEM_FEAR )
			return TRUE;

	return FALSE;
}

void stop_hunting( CHAR_DATA *ch )
{
	if ( ch->hunting )
		ch->hunting = NULL;
	return ;
}

void stop_hating( CHAR_DATA *ch, CHAR_DATA *victim , bool All )
{
	if ( !ch || !ch->memory )
		return ;

	forget( ch, victim, MEM_HATE, FALSE );
	return ;
}

void stop_fearing( CHAR_DATA *ch, CHAR_DATA *victim, bool All )
{
	if ( !ch || !ch->memory )
		return ;

	forget( ch, victim, MEM_FEAR, All );
	return ;
}

void start_hunting( CHAR_DATA *ch, CHAR_DATA *victim )
{
	if ( !ch || !victim || !IS_NPC( ch ) )
		return ;

	if ( !EXT_IS_SET( ch->act, ACT_MEMORY ) || ch == victim )
		return ;

	if ( ch->hunting )
		stop_hunting( ch );

	ch->hunting = victim;
	return ;
}

void start_hating( CHAR_DATA *ch, CHAR_DATA *victim )
{
	if ( !ch || !victim || !IS_NPC( ch ) || IS_NPC( victim ) )
		return ;

	if ( !EXT_IS_SET( ch->act, ACT_MEMORY ) || ch == victim )
		return ;

	if ( IS_AFFECTED( ch, AFF_FEAR ) )
		return ;

	remember
	( ch, victim, MEM_HATE );
	return ;
}

void start_fearing( CHAR_DATA *ch, CHAR_DATA *victim )
{
	if ( !ch || !victim || !IS_NPC( ch ) || IS_NPC( victim ) )
		return ;

	if ( !EXT_IS_SET( ch->act, ACT_MEMORY ) )
		return ;

	forget( ch, victim, MEM_HATE, FALSE );

	remember( ch, victim, MEM_FEAR );
	return ;
}

void set_mem( CHAR_DATA *ch, sh_int sn, sh_int count )
{
	MSPELL_DATA * mspell, *tmp;
	sh_int circle, caster = -1;
	int i;

	if ( !IS_NPC( ch ) )
		return ;

	if ( sn < 0 ||
	     sn > MAX_SKILL ||
	     skill_table[ sn ].spell_fun == spell_null )
		return ;

	caster = get_caster( ch );

	if ( caster == 0 )
	{
		if ( ch->level < skill_table[ sn ].skill_level[ 0 ] )
			return ;

		circle = ( skill_table[ sn ].skill_level[ 0 ] / 3 + 1 );
	}
	else if ( caster == 1 )
	{
		if ( ch->level < skill_table[ sn ].skill_level[ 1 ] )
			return ;

		circle = ( skill_table[ sn ].skill_level[ 1 ] / 4 + 1 );
	}
	else if ( caster == 5 )
	{
		if ( ch->level < skill_table[ sn ].skill_level[ 5 ] )
			return ;

		circle = ( skill_table[ sn ].skill_level[ 5 ] / 4 + 1 );
	}
	else if ( caster == 4 )
	{
		if ( ch->level < 13 || ch->level < skill_table[ sn ].skill_level[ 4 ] )
			return ;

		circle = ( ( UMIN( 0, skill_table[ sn ].skill_level[ 4 ] - 13 ) ) / 4 + 1 );
	}

	else
	{
		/* imbuowany */
		circle = 1;
	}

	if ( sn == skill_lookup( "spiritual arm" ) )
		count = 1;


	for ( i = 0; i < count; i++ )
	{
		mspell = new_mspell();
		mspell->spell = sn;
		mspell->circle = circle;
		mspell->done = TRUE;
		mspell->next = NULL;

		if ( !ch->memspell )
		{
			ch->memspell = mspell;
		}
		else
		{
			for ( tmp = ch->memspell;tmp->next;tmp = tmp->next ) ;
				tmp->next = mspell;
		}
	}

	mem_update_count( ch );

	return ;
}

void remember( CHAR_DATA *mob, CHAR_DATA *who, int reaction )
{
	MEM_DATA * new_el, *tmp;

	if ( reaction < 0 || reaction > 2 || !mob || !who || mob == who )
		return ;

	if ( !mob->memory )
	{
		new_el = new_mem_data();
		new_el->who = who;
		new_el->reaction = reaction;
		new_el->next = NULL;
		mob->memory = new_el;
		return ;
	}
	else
	{
		/* jesli juz to pamieta*/
		for ( tmp = mob->memory;tmp;tmp = tmp->next )
			if ( tmp->who == who && tmp->reaction == reaction )
				return ;

		new_el = new_mem_data();
		new_el->who = who;
		new_el->reaction = reaction;
		new_el->next = NULL;
		new_el->next = mob->memory;
		mob->memory = new_el;
	}
	return ;
}

void forget( CHAR_DATA *mob, CHAR_DATA *who, int reaction, bool All )
{
	MEM_DATA * tmp, *tmp_next, *new_list = NULL;

	if ( !mob || !mob->memory )
		return ;

	if ( All )
	{
		for ( tmp = mob->memory;tmp;tmp = tmp_next )
		{
			tmp_next = tmp->next;
			free_mem_data( tmp );
			tmp = NULL;
		}

		mob->memory = NULL;
		return ;
	}

	if ( !who || reaction < 0 || reaction > 1 )
		return ;

	for ( tmp = mob->memory; tmp ; tmp = tmp_next )
	{
		tmp_next = tmp->next;

		if ( tmp->who == who && tmp->reaction == reaction )
		{
			free_mem_data( tmp );
			tmp = NULL;
		}
		else
		{
			tmp->next = new_list;
			new_list = tmp;
		}
	}

	mob->memory = new_list;

	/*    if(mob->memory->who==who)
	    {
		dump=mob->memory;
		mob->memory=mob->memory->next;
		free_mem_data(dump);
		dump=NULL;
	    }
	    else
	    {
		dump=mob->memory;
		tmp=dump;

		for(;tmp;tmp=tmp->next)
		{
		    if(tmp->who==who)
		    {
			dump->next=tmp->next;
			free_mem_data(tmp);
			tmp=NULL;
			return;
	            }

		    dump=tmp;
		}
	    }*/

	return ;
}

void dispose_memory( CHAR_DATA *mob )
{
	MEM_DATA * tmp, *tmp_next;

	if ( !mob || !mob->memory )
		return ;

	for ( tmp = mob->memory;tmp;tmp = tmp_next )
	{
		tmp_next = tmp->next;
		free_mem_data( tmp );
	}

	mob->memory = NULL;

	return ;
}
