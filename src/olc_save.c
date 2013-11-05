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
 *  File: olc_save.c                                                   *
 *                                                                     *
 *  Much time and thought has gone into this software and you are      *
 *  benefitting.  We hope that you share your changes too.  What goes  *
 *  around, comes around.                                              *
 *                                                                     *
 *  This code was freely distributed with the The Isles 1.1 source     *
 *  code, and has been used here for OLC - OLC would not be what it is *
 *  without all the previous coders who released their source code.    *
 *                                                                     *
 * This takes care of saving all the .are information.                 *
 *                                                                     *
 * Notes:                                                              *
 *                                                                     *
 * If a good syntax checker is used for setting vnum ranges of areas   *
 * then it would become possible to just cycle through vnums instead   *
 * of using the iHash stuff and checking that the room or reset or     *
 * mob etc is part of that area.                                       *
 *                                                                     *
 ***********************************************************************
 *                                                                     *
 * KILLER MUD is copyright 1999-2012 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Jaron Krzysztof       (chris.jaron@gmail.com           ) [Razor   ] *
 * Pietrzak Marcin       (marcin@iworks.pl                ) [Gurthg  ] *
 * Sawicki Tomasz        (furgas@killer-mud.net           ) [Furgas  ] *
 * Trebicki Marek        (maro@killer.radom.net           ) [Maro    ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl        ) [Agron   ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: olc_save.c 11237 2012-04-07 15:23:19Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/olc_save.c $
 *
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
#include "olc.h"
#include "progs.h"

#define DIF(a,b) (~((~a)|(b)))

void new_save_mobiles( FILE *fp, AREA_DATA *pArea );
void new_save_objects( FILE *fp, AREA_DATA *pArea );
void new_save_rooms( FILE *fp, AREA_DATA *pArea );
void save_roomprogs( FILE *fp, AREA_DATA *pArea );
void save_traps( FILE *fp, AREA_DATA *pArea );
void save_desc( FILE *fp, AREA_DATA *pArea );
void save_rand_desc( FILE *fp, AREA_DATA *pArea );
void save_bonus_set( FILE *fp, AREA_DATA *pArea );

/*
 *  Verbose writes reset data in plain english into the comments
 *  section of the resets.  It makes areas considerably larger but
 *  may aid in debugging.
 */

/* #define VERBOSE */

/*****************************************************************************
 Name:		fix_string
 Purpose:	Returns a string without \r and ~.
 ****************************************************************************/
char *fix_string( const char *str )
{
    static char strfix[MAX_STRING_LENGTH * 2];
    int i;
    int o;

    if ( str == NULL )
        return '\0';

    for ( o = i = 0; str[i+o] != '\0'; i++ )
    {
        if (str[i+o] == '\r' || str[i+o] == '~')
            o++;
        strfix[i] = str[i+o];
    }
    strfix[i] = '\0';
    return strfix;
}

/*****************************************************************************
 Name:		save_area_list
 Purpose:	Saves the listing of files to be loaded at startup.
 Called by:	do_asave(olc_save.c).
 ****************************************************************************/
void save_area_list()
{
    FILE *fp;
    AREA_DATA *pArea;
    extern HELP_AREA * had_list;
    HELP_AREA * ha;

    if ( ( fp = fopen( "area.lst", "w" ) ) == NULL )
    {
        bug( "Save_area_list: fopen", 0 );
        perror( "area.lst" );
    }
    else
    {
        /*
         * Add any help files that need to be loaded at
         * startup to this section.
         */
        fprintf( fp, "social.are\n" );    /* ROM OLC */

        for ( ha = had_list; ha; ha = ha->next )
        {
            if ( ha->area == NULL )
            {
                fprintf( fp, "%s\n", ha->filename );
            }
        }
        for( pArea = area_first; pArea; pArea = pArea->next )
        {
            if(IS_SET(pArea->area_flags, AREA_RANDOM))
            {
                continue;
            }
            fprintf( fp, "%s\n", pArea->file_name );
        }
        fprintf( fp, "$\n" );
        fclose( fp );
    }

    return;
}

/*
 * ROM OLC
 * Used in save_mobile and save_object below.  Writes
 * flags on the form fread_flag reads.
 *
 * buf[] must hold at least 32+1 characters.
 *
 * -- Hugin
 */
char *fwrite_flag( long flags, char buf[] )
{
    char offset;
    char *cp;

    buf[0] = '\0';

    if ( flags == 0 )
    {
	strcpy( buf, "0" );
	return buf;
    }

    /* 32 -- number of bits in a long */

    for ( offset = 0, cp = buf; offset < 32; offset++ )
	if ( flags & ( (long)1 << offset ) )
	{
	    if ( offset <= 'Z' - 'A' )
		*(cp++) = 'A' + offset;
	    else
		*(cp++) = 'a' + offset - ( 'Z' - 'A' + 1 );
	}

    *cp = '\0';

    return buf;
}

void save_mobprogs( FILE *fp, AREA_DATA *pArea )
{
    PROG_CODE *prog;
	char* min = "zz";
	char* actual = "00";

    fprintf( fp, "#MOBPROGSNEW\n" );
    /**
     * zapisz posortowane po nazwie
     */
	while ( str_cmp( min, "zzz" ) )
	{
        min = "zzz";
        for( prog = mprog_list; prog; prog = prog->next )
        {
            if ( strcmp( prog->name, min ) <= 0 && strcmp( prog->name, actual ) > 0 )
            {
                min = prog->name;
            }
        }
		actual = min;
        if ( strcmp( actual, "zzz" ) )
        {
            for( prog = mprog_list; prog; prog = prog->next )
            {
                if ( prog->area == pArea && !strcmp( prog->name, actual ) )
                {
                    fprintf( fp, "#%s\n", prog->name );
                    fprintf( fp, "%s~\n", fix_string( prog->description ) );
                    fprintf( fp, "%s~\n", fix_string( prog->code ) );
                }
            }
        }
    }
    fprintf( fp, "#0\n\n" );
    return;
}

void save_objprogs( FILE *fp, AREA_DATA *pArea )
{
    PROG_CODE *prog;
	char* min = "zz";
	char* actual = "00";

    fprintf( fp, "#OBJPROGSNEW\n" );
    /**
     * zapisz posortowane po nazwie
     */
	while ( str_cmp( min, "zzz" ) )
	{
        min = "zzz";
        for( prog = oprog_list; prog; prog = prog->next )
        {
            if ( strcmp( prog->name, min ) <= 0 && strcmp( prog->name, actual ) > 0 )
            {
                min = prog->name;
            }
        }
		actual = min;
        if ( strcmp( actual, "zzz" ) )
        {
            for( prog = oprog_list; prog; prog = prog->next )
            {
                if ( prog->area == pArea && !strcmp( prog->name, actual ) )
                {
                    fprintf( fp, "#%s\n", prog->name );
                    fprintf( fp, "%s~\n", fix_string( prog->description ) );
                    fprintf( fp, "%s~\n", fix_string( prog->code ) );
                }
            }
        }
    }
    fprintf( fp, "#0\n\n" );
    return;
}

void save_roomprogs( FILE *fp, AREA_DATA *pArea )
{
    PROG_CODE *prog;
	char* min = "zz";
	char* actual = "00";

    fprintf( fp, "#ROOMPROGSNEW\n" );
    /**
     * zapisz posortowane po nazwie
     */
	while ( str_cmp( min, "zzz" ) )
	{
        min = "zzz";
        for( prog = rprog_list; prog; prog = prog->next )
        {
            if ( strcmp( prog->name, min ) <= 0 && strcmp( prog->name, actual ) > 0 )
            {
                min = prog->name;
            }
        }
		actual = min;
        if ( strcmp( actual, "zzz" ) )
        {
            for( prog = rprog_list; prog; prog = prog->next )
            {
                if ( prog->area == pArea && !strcmp( prog->name, actual ) )
                {
                    fprintf( fp, "#%s\n", prog->name );
                    fprintf( fp, "%s~\n", fix_string( prog->description ) );
                    fprintf( fp, "%s~\n", fix_string( prog->code ) );
                }
            }
        }
    }
    fprintf( fp, "#0\n\n" );
    return;
}

/*****************************************************************************
 Name:		save_specials
 Purpose:	Save #SPECIALS section of area file.
 Called by:	save_area(olc_save.c).
 ****************************************************************************/
void save_specials( FILE *fp, AREA_DATA *pArea )
{
    int iHash;
    MOB_INDEX_DATA *pMobIndex;

    fprintf( fp, "#SPECIALS\n" );

    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next )
        {
            if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->spec_fun )
            {
#if defined( VERBOSE )
                fprintf( fp, "M %d %s Load to: %s\n", pMobIndex->vnum,
                                                      spec_name( pMobIndex->spec_fun ),
                                                      pMobIndex->short_descr );
#else
                fprintf( fp, "M %d %s\n", pMobIndex->vnum,
                              spec_name( pMobIndex->spec_fun ) );
#endif
            }
        }
    }

    fprintf( fp, "S\n\n\n\n" );
    return;
}



int door_rs_flags(int flags)
{

  if(IS_SET(flags, EX_LOCKED))
  {
    if(IS_SET(flags, EX_SECRET))
	return 8;
    if(IS_SET(flags, EX_HIDDEN))
	return 5;
    return 2;
  }

  if(IS_SET(flags, EX_CLOSED))
  {
    if(IS_SET(flags, EX_SECRET))
	return 7;
    if(IS_SET(flags, EX_HIDDEN))
	return 4;
    return 1;
  }

  if(IS_SET(flags, EX_SECRET))
      return 6;

  if(IS_SET(flags, EX_HIDDEN))
      return 3;

  return 0;
}

char *door_rs_names(int nr)
{
    switch(nr)
    {
        case 0: return "open";
        case 1: return "closed";
        case 2: return "closed and locked";
        case 3: return "open hidden";
        case 4: return "closed hidden";
        case 5: return "locked hidden";
        case 6: return "open secret";
        case 7: return "closed secret";
        case 8: return "locked secret";
    }
    return "none";
}

/*****************************************************************************
 Name:		save_resets
 Purpose:	Saves the #RESETS section of an area file.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
void save_resets( FILE *fp, AREA_DATA *pArea )
{
    RESET_DATA *pReset;
    MOB_INDEX_DATA *pLastMob = NULL;
    OBJ_INDEX_DATA *pLastObj;
    ROOM_INDEX_DATA *pRoom;
    char buf[MAX_STRING_LENGTH];
    int iHash;

    fprintf( fp, "#RESETS\n" );

    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next )
        {
            if ( pRoom->area == pArea )
	    {
    for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
    {
	switch ( pReset->command )
	{
	default:
	    bug( "Save_resets: bad command %c.", pReset->command );
	    break;

#if defined( VERBOSE )
	case 'M':
            pLastMob = get_mob_index( pReset->arg1 );
	    fprintf( fp, "M 0 %d %d %d %d Load %s\n",
	        pReset->arg1,
                pReset->arg2,
                pReset->arg3,
		pReset->arg4,
                pLastMob->short_descr );
            break;

	case 'O':
            pLastObj = get_obj_index( pReset->arg1 );
            pRoom = get_room_index( pReset->arg3 );
	    fprintf( fp, "O 0 %d 0 %d %s loaded to %s\n",
	        pReset->arg1,
                pReset->arg3,
                capitalize(pLastObj->short_descr),
                pRoom->name );
            break;

	case 'P':
            pLastObj = get_obj_index( pReset->arg1 );
	    fprintf( fp, "P 0 %d %d %d %d %s put inside %s\n",
	        pReset->arg1,
	        pReset->arg2,
                pReset->arg3,
                pReset->arg4,
                capitalize(get_obj_index( pReset->arg1 )->short_descr),
                pLastObj->short_descr );
            break;

	case 'G':
	    fprintf( fp, "G 0 %d 0 %s is given to %s\n",
	        pReset->arg1,
	        capitalize(get_obj_index( pReset->arg1 )->short_descr),
                pLastMob ? pLastMob->short_descr : "!NO_MOB!" );
            if ( !pLastMob )
            {
                sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
                bug( buf, 0 );
            }
            break;

	case 'E':
	    fprintf( fp, "E 0 %d 0 %d %s is loaded %s of %s\n",
	        pReset->arg1,
                pReset->arg3,
                capitalize(get_obj_index( pReset->arg1 )->short_descr),
                flag_string( wear_loc_strings, pReset->arg3 ),
                pLastMob ? pLastMob->short_descr : "!NO_MOB!" );
            if ( !pLastMob )
            {
                sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
                bug( buf, 0 );
            }
            break;

	case 'D':
            break;

	case 'R':
            pRoom = get_room_index( pReset->arg1 );
	    fprintf( fp, "R 0 %d %d Randomize %s\n",
	        pReset->arg1,
                pReset->arg2,
                pRoom->name );
            break;
            }
#endif
#if !defined( VERBOSE )
	case 'M':
            pLastMob = get_mob_index( pReset->arg1 );
	    fprintf( fp, "M %d %d %d %d %d\n",
	        pReset->arg0,
	        pReset->arg1,
                pReset->arg2,
                pReset->arg3,
                pReset->arg4 );
            break;

	case 'O':
            pLastObj = get_obj_index( pReset->arg1 );
            pRoom = get_room_index( pReset->arg3 );
	    fprintf( fp, "O %d %d 0 %d\n",
	        pReset->arg0,
	        pReset->arg1,
                pReset->arg3 );
            break;

	case 'P':
            pLastObj = get_obj_index( pReset->arg1 );
	    fprintf( fp, "P %d %d %d %d %d\n",
	        pReset->arg0,
	        pReset->arg1,
	        pReset->arg2,
                pReset->arg3,
                pReset->arg4 );
            break;

	case 'G':
	    fprintf( fp, "G %d %d 0\n", pReset->arg0, pReset->arg1 );
            if ( !pLastMob )
            {
                sprintf( buf,
                    "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
                bug( buf, 0 );
            }
            break;

	case 'E':
	    fprintf( fp, "E %d %d 0 %d\n",
		 pReset->arg0,
	        pReset->arg1,
                pReset->arg3 );
            if ( !pLastMob )
            {
                sprintf( buf,
                    "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
                bug( buf, 0 );
            }
            break;

	case 'D':
            break;

	case 'R':
            pRoom = get_room_index( pReset->arg1 );
	    fprintf( fp, "R 0 %d %d\n",
	        pReset->arg1,
                pReset->arg2 );
            break;
            }
#endif
        }
	    }	/* End if correct area */
	}	/* End for pRoom */
    }	/* End for iHash */
    fprintf( fp, "S\n\n\n\n" );
    return;
}



/*****************************************************************************
 Name:		save_shops
 Purpose:	Saves the #SHOPS section of an area file.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
void save_shops( FILE *fp, AREA_DATA *pArea )
{
    SHOP_DATA *pShopIndex;
    MOB_INDEX_DATA *pMobIndex;
    int iTrade;
    int iHash;

    fprintf( fp, "#SHOPS\n" );

    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next )
        {
            if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->pShop )
            {
                pShopIndex = pMobIndex->pShop;

                fprintf( fp, "%d ", pShopIndex->keeper );
                for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
                {
                    if ( pShopIndex->buy_type[iTrade] != 0 )
                    {
                       fprintf( fp, "%d ", pShopIndex->buy_type[iTrade] );
                    }
                    else
                       fprintf( fp, "0 ");
                }
                fprintf( fp, "%d %d ", pShopIndex->profit_buy, pShopIndex->profit_sell );
                fprintf( fp, "%d %d ", pShopIndex->open_hour, pShopIndex->close_hour );
                fprintf( fp, "* %d\n", pShopIndex->shop_room );
            }
        }
    }

    fprintf( fp, "0\n\n\n\n" );
    return;
}

/*****************************************************************************
 Name:		save_repairs
 Purpose:	Saves the #REPAIRS section of an area file.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
 
void save_repairs( FILE *fp, AREA_DATA *pArea )
{
    REPAIR_DATA *pRepair;
    MOB_INDEX_DATA *pMobIndex;
    int iHash;

    fprintf( fp, "#REPAIRS\n" );

    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next )
        {
            if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->pRepair )
            {
                for( pRepair = pMobIndex->pRepair; pRepair; pRepair = pRepair->next )
                    fprintf( fp, "%d %d %d %d %d %d %d * %d\n",
                        pMobIndex->vnum,
                        pRepair->item_type,
                        pRepair->max_cost,
                        pRepair->repair_cost_mod,
                        pRepair->min_condition,
                        pRepair->repair_open_hour,
                        pRepair->repair_close_hour,
                        pRepair->repair_room );
            }
        }
    }

    fprintf( fp, "0\n\n\n\n" );
    return;
}


/*****************************************************************************
 Name:		save_banks
 Purpose:	Saves the #BANKS section of an area file.
 Called by:	save_area(olc_save.c)
 ****************************************************************************/
void save_banks( FILE *fp, AREA_DATA *pArea )
{
    BANK_DATA *pBankIndex;
    MOB_INDEX_DATA *pMobIndex;
    int iHash;

    fprintf( fp, "#BANKS\n" );

    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next )
        {
            if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->pBank )
            {
                pBankIndex = pMobIndex->pBank;

                fprintf( fp, "%d %d %d %d * %d\n",
                pBankIndex->banker,
                pBankIndex->provision,
                pBankIndex->open_hour,
                pBankIndex->close_hour,
                pBankIndex->bank_room );
            }
        }
    }

    fprintf( fp, "0\n\n\n\n" );
    return;
}

void save_helps( FILE *fp, HELP_AREA *ha )
{
	HELP_DATA *help = ha->first;

	fprintf( fp, "#HELPS\n" );

	for ( ; help; help = help->next_area )
	{
		fprintf( fp, "%d %s~\n", help->level, help->keyword );
		fprintf( fp, "%s~\n\n", fix_string( help->text ) );
	}

	fprintf( fp, "-1 $~\n\n" );

	ha->changed = FALSE;

	return;
}

void save_other_helps( CHAR_DATA *ch )
{
	extern HELP_AREA * had_list;
	HELP_AREA *ha;
	FILE *fp;

	for ( ha = had_list; ha; ha = ha->next )
		if ( ha->changed == TRUE )
		{
			fp = fopen( ha->filename, "w" );

			if ( !fp )
			{
				perror( ha->filename );
				return;
			}

			save_helps( fp, ha );

			if (ch)
				printf_to_char( ch, "%s\n\r", ha->filename );

			fprintf( fp, "#$\n" );
			fclose( fp );
		}

	return;
}

/*****************************************************************************
 Name:		save_area
 Purpose:	Save an area, note that this format is new.
 Called by:	do_asave(olc_save.c).
 ****************************************************************************/
void save_area( AREA_DATA *pArea )
{
    FILE *fp;

    fclose( fpReserve );
    if ( !( fp = fopen( pArea->file_name, "w" ) ) )
    {
        bug( "Open_area: fopen", 0 );
        perror( pArea->file_name );
    }

    fprintf( fp, "#AREADATA\n" );
    fprintf( fp, "Name %s~\n",       pArea->name );
    fprintf( fp, "Builders %s~\n",   fix_string( pArea->builders ) );
    fprintf( fp, "VNUMs %d %d\n",    pArea->min_vnum, pArea->max_vnum );
    if ( !IS_NULLSTR( pArea->credits ) )
    {
		fprintf( fp, "Credits %s~\n",    pArea->credits );
    }
	fprintf( fp, "Locked %s~\n",     pArea->locked );
    fprintf( fp, "Security %d\n",    pArea->security );
    fprintf( fp, "ResetAge %d\n",    pArea->resetage );
    fprintf( fp, "Region %d\n",      pArea->region );
    fprintf( fp, "End\n\n\n\n" );

    new_save_mobiles( fp, pArea );
    new_save_objects( fp, pArea );
    new_save_rooms( fp, pArea );
    save_shops( fp, pArea );
    save_banks( fp, pArea );
    save_specials( fp, pArea );
    save_resets( fp, pArea );
    save_mobprogs( fp, pArea );
    save_objprogs( fp, pArea );
    save_roomprogs( fp, pArea );
    save_traps( fp, pArea );
    save_rand_desc( fp, pArea );
    save_desc( fp, pArea );
    save_repairs( fp, pArea );
    save_bonus_set( fp, pArea );

    if ( pArea->helps && pArea->helps->first )
	save_helps( fp, pArea->helps );

    fprintf( fp, "#$\n" );

    fclose( fp );


    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/*****************************************************************************
 Name:		do_asave
 Purpose:	Entry point for saving area data.
 Called by:	interpreter(interp.c)
 ****************************************************************************/
void do_asave( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    AREA_DATA *pArea;
    FILE *fp;
    int value, sec;

    fp = NULL;

    if ( !ch )       /* Do an autosave */
    {
        sec = 9;
    }
    else if ( !IS_NPC(ch) )
    {
    	sec = ch->pcdata->security;
    }
    else
    {
    	sec = 0;
    }

    smash_tilde( argument );
    strcpy( arg1, argument );

    if ( arg1[0] == '\0' )
    {
        if ( ch )
        {
            send_to_char( "Syntax:\n\r", ch );
            send_to_char( "  asave <vnum>   - zapisuje kraine o podanym vnumie\n\r",	ch );
            send_to_char( "  asave list     - zapisuje plik area.lst\n\r",	ch );
            send_to_char( "  asave area     - zapisuje aktualnie edytowan± krainê\n\r",	ch );
            send_to_char( "  asave changed  - zapisuje wszystkie zmiany w krainach\n\r",	ch );
            send_to_char( "  asave world    - saves the world! (db dump)\n\r",	ch );
            send_to_char( "\n\r", ch );
        }
        return;
    }

    /* Snarf the value (which need not be numeric). */
    value = atoi( arg1 );
    if ( !( pArea = get_area_data( value ) ) && is_number( arg1 ) )
    {
        if (ch)
        {
            send_to_char( "asave: Taka kraina nie istnieje.\n\r", ch );
        }
        return;
    }

    /* Save area of given vnum. */
    /* ------------------------ */
    if ( is_number( arg1 ) )
    {
        if ( ch && !IS_BUILDER( ch, pArea ) )
        {
            send_to_char( "Masz niewystarczaj±ce security, ¿eby zapisaæ t± krainê.\n\r", ch );
            return;
        }

        if ( !IS_NULLSTR ( pArea->locked ) )
        {
            if ( ch )
            {
                print_char( ch, "Kraina jest aktualnie zablokowana, nie mo¿esz jej zapisaæ. Powód:\n\r%s\n\r", pArea->locked );
            }
            return;
        }

        if ( IS_SET ( pArea->area_flags, AREA_RANDOM ) )
        {
            return;
        }

        save_area_list();
        save_area( pArea );

        return;
    }

    /* Save the world, only authorized areas. */
    /* -------------------------------------- */
    if ( !str_cmp( "world", arg1 ) )
    {
        save_area_list();
        for( pArea = area_first; pArea; pArea = pArea->next )
        {
            /* Builder must be assigned this area. */
            if ( ch && !IS_BUILDER( ch, pArea ) )
            {
                continue;
            }
            if ( !IS_NULLSTR( pArea->locked ) )
            {
                continue;
            }
            if(IS_SET(pArea->area_flags, AREA_RANDOM))
            {
                continue;
            }
            save_area( pArea );
            REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
        }
        save_other_helps( NULL );
        if ( ch )
        {
            send_to_char( "Zapisal<&e/a/>s caly swiat. Szczesliw<&y/a/e>?\n\r", ch );
        }
        return;
    }

    /* Save changed areas, only authorized areas. */
    /* ------------------------------------------ */
    if ( !str_cmp( "changed", arg1 ) )
    {
        char buf[MAX_INPUT_LENGTH];

        save_area_list();

        if ( ch )
        {
            send_to_char( "Saved zones:\n\r", ch );
        }
        else
        {
            log_string( "Saved zones:" );
        }

        sprintf( buf, "None.\n\r" );

        for( pArea = area_first; pArea; pArea = pArea->next )
        {
            /* Builder must be assigned this area. */
            if ( ch && !IS_BUILDER( ch, pArea ) )
            {
                continue;
            }

            if(IS_SET(pArea->area_flags, AREA_RANDOM))
            {
                continue;
            }

            /* Save changed areas. */
            if ( IS_SET(pArea->area_flags, AREA_CHANGED) )
            {

                if ( !IS_NULLSTR( pArea->locked ) )
                    sprintf( buf, "%24s - {Rzapis krainy zablokowany - %s{x", pArea->name, pArea->locked );
                else
                {
                    save_area( pArea );
                    sprintf( buf, "%24s - '%s'", pArea->name, pArea->file_name );
                    REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
                }
                if ( ch )
                {
                    send_to_char( buf, ch );
                    send_to_char( "\n\r", ch );
                }
                else
                {
                    log_string( buf );
                }
            }
        }

        save_other_helps( NULL );

        if ( !str_cmp( buf, "None.\n\r" ) )
        {
            if ( ch )
            {
                send_to_char( buf, ch );
            }
            else
            {
                log_string( "None." );
            }
        }

        return;
    }

    /* Save the area.lst file. */
    /* ----------------------- */
    if ( !str_cmp( arg1, "list" ) )
    {
        save_area_list();
        return;
    }

    /* Save area being edited, if authorized. */
    /* -------------------------------------- */
    if ( !str_cmp( arg1, "area" ) )
    {
        if ( !ch || !ch->desc )
        {
            return;
        }

        /* Is character currently editing. */
        if ( ch->desc->editor == ED_NONE )
        {
            send_to_char( "Nie edytujesz ¿adnej krainy.\n\r", ch );
            return;
        }

        /* Find the area to save. */
        switch (ch->desc->editor)
        {
            case ED_AREA:
                pArea = (AREA_DATA *)ch->desc->pEdit;
                break;
            case ED_ROOM:
                pArea = ch->in_room->area;
                break;
            case ED_OBJECT:
                pArea = ( (OBJ_INDEX_DATA *)ch->desc->pEdit )->area;
                break;
            case ED_MOBILE:
                pArea = ( (MOB_INDEX_DATA *)ch->desc->pEdit )->area;
                break;
            case ED_HELP:
                send_to_char( "Zapisywanie plików z pomoc±.", ch );
                save_other_helps( ch );
                return;
            default:
                pArea = ch->in_room->area;
                break;
        }

	if ( !IS_BUILDER( ch, pArea ) )
	{
	    send_to_char( "Masz niewystarczaj±ce security, ¿eby zapisaæ t± krainê.\n\r", ch );
	    return;
	}

	if ( !IS_NULLSTR( pArea->locked ) )
	{
    	print_char( ch, "Kraina jest aktualnie zablokowana, nie mo¿esz jej zapisaæ. Powód:\n\r%s\n\r", pArea->locked );
	    return;
	}

	if(IS_SET(pArea->area_flags, AREA_RANDOM))
	    return;

	save_area_list();
	save_area( pArea );
	REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
	send_to_char( "Area saved.\n\r", ch );
	return;
    }

    /* Show correct syntax. */
    /* -------------------- */
    if (ch)
	do_asave( ch, "" );

    return;
}

void new_save_mobile( FILE *fp, MOB_INDEX_DATA *pMobIndex )
{
    sh_int race = pMobIndex->race;
    PROG_LIST *pMprog;
    char buf[ MAX_STRING_LENGTH ];
    int i;

    fprintf( fp, "#Vnum %d\n", pMobIndex->vnum );
    fprintf( fp, "Name %s~\n", pMobIndex->player_name );
    fprintf( fp, "Odmiana %s~%s~%s~%s~%s~\n", pMobIndex->name2, pMobIndex->name3,
             pMobIndex->name4, pMobIndex->name5, pMobIndex->name6 );
    fprintf( fp, "Short %s~\n", pMobIndex->short_descr );
    fprintf( fp, "Long %s~\n", fix_string( pMobIndex->long_descr ) );
    fprintf( fp, "Descr %s~\n", fix_string( pMobIndex->description ) );
    fprintf( fp, "Race %s~\n", race_table[ race ].name );
    fprintf( fp, "ActExt %s\n", print_ext_flags( pMobIndex->act ) );

    fprintf( fp, "Affected %ld %ld %ld %ld\n", pMobIndex->affected_by[ 0 ],
             pMobIndex->affected_by[ 1 ], pMobIndex->affected_by[ 2 ],
             pMobIndex->affected_by[ 3 ] );
	fprintf( fp, "AffectedExt %s\n", print_ext_flags( pMobIndex->affected_by ) );

    fprintf( fp, "Align %d\nGroup %d\n", pMobIndex->alignment , pMobIndex->group );
    fprintf( fp, "Lang %s\nSpeaks %d\n", fwrite_flag( pMobIndex->languages, buf ), pMobIndex->speaking );
    fprintf( fp, "Level %d\n", pMobIndex->level );
    fprintf( fp, "Hitroll %d\n", pMobIndex->hitroll );
    fprintf( fp, "HP %dd%d+%d\n", pMobIndex->hit[ DICE_NUMBER ],
             pMobIndex->hit[ DICE_TYPE ],
             pMobIndex->hit[ DICE_BONUS ] );
    fprintf( fp, "Damage %dd%d+%d\n", pMobIndex->damage[ DICE_NUMBER ],
             pMobIndex->damage[ DICE_TYPE ],
             pMobIndex->damage[ DICE_BONUS ] );
    fprintf( fp, "Dammagic %d\n", pMobIndex->magical_damage );
    fprintf( fp, "Weaponbonus %d\n", pMobIndex->weapon_damage_bonus );
    fprintf( fp, "Damflags %d\n", pMobIndex->attack_flags );
    fprintf( fp, "Attack %s\n", attack_table[ pMobIndex->dam_type ].name );


    fprintf( fp, "XAC %d %d %d %d\n",
             pMobIndex->ac[ AC_PIERCE ] / 10,
             pMobIndex->ac[ AC_BASH ] / 10,
             pMobIndex->ac[ AC_SLASH ] / 10,
             pMobIndex->ac[ AC_EXOTIC ] / 10 );

    fprintf( fp, "Stats %d %d %d %d %d %d %d\n",
             pMobIndex->stats[ 0 ],
             pMobIndex->stats[ 1 ],
             pMobIndex->stats[ 2 ],
             pMobIndex->stats[ 3 ],
             pMobIndex->stats[ 4 ],
             pMobIndex->stats[ 5 ],
             pMobIndex->stats[ 6 ] );

/*	fprintf( fp, "NewStats %d %d %d %d %d %d %d\n",
             pMobIndex->stats[ 0 ]*6,
             pMobIndex->stats[ 1 ]*6,
             pMobIndex->stats[ 2 ]*6,
             pMobIndex->stats[ 3 ]*6,
             pMobIndex->stats[ 4 ]*6,
             pMobIndex->stats[ 5 ]*6,
             pMobIndex->stats[ 6 ]*6 );*/


    fprintf( fp, "Off %s\n", fwrite_flag( pMobIndex->off_flags[0], buf ) );
    fprintf( fp, "OffExt %s\n", print_ext_flags( pMobIndex->off_flags ) );

    fprintf( fp, "Pos_start %s\nPos_def %s\nSex %s\nWealth %ld\n",
             position_table[ pMobIndex->start_pos ].short_name,
             position_table[ pMobIndex->default_pos ].short_name,
             sex_table[ pMobIndex->sex ].name,
             pMobIndex->wealth );
    fprintf( fp, "Form %s\n", fwrite_flag( pMobIndex->form, buf ) );
    fprintf( fp, "Parts %s\n", fwrite_flag( pMobIndex->parts, buf ) );

    fprintf( fp, "Size %s\n", size_table[ pMobIndex->size ].name );
    fprintf( fp, "Material %s\n", IS_NULLSTR( pMobIndex->material ) ? pMobIndex->material : "unknown" );
    if ( !IS_NULLSTR( pMobIndex->comments ) )
        fprintf( fp, "Comments %s~\n", pMobIndex->comments );

    for ( pMprog = pMobIndex->progs; pMprog; pMprog = pMprog->next )
    {
        fprintf( fp, "MobprogNew %s %s %s %s~\n",
                 ext_bit_name( prog_flags, pMprog->trig_type ),
                 pMprog->name,
                 fwrite_flag( pMprog->valid_positions, buf ),
                 pMprog->trig_phrase );
    }

    if ( pMobIndex->spells[ 0 ] != 0 )
    {
        fprintf( fp, "Spells %d %d %d %d\n", pMobIndex->spells[ 0 ], pMobIndex->spells[ 1 ],
                 pMobIndex->spells[ 2 ], pMobIndex->spells[ 3 ] );
    }

    if ( pMobIndex->spells[ 4 ] != 0 )
    {
        fprintf( fp, "Spells %d %d %d %d\n", pMobIndex->spells[ 4 ], pMobIndex->spells[ 5 ],
                 pMobIndex->spells[ 6 ], pMobIndex->spells[ 7 ] );
    }

    if ( pMobIndex->spells[ 8 ] != 0 )
    {
        fprintf( fp, "Spells %d %d %d %d\n", pMobIndex->spells[ 8 ], pMobIndex->spells[ 9 ],
                 pMobIndex->spells[ 10 ], pMobIndex->spells[ 11 ] );
    }

    if ( pMobIndex->spells[ 12 ] != 0 )
    {
        fprintf( fp, "Spells %d %d %d %d\n", pMobIndex->spells[ 12 ], pMobIndex->spells[ 13 ],
                 pMobIndex->spells[ 14 ], pMobIndex->spells[ 15 ] );
    }


    if ( pMobIndex->corpse_vnum != 0 )
        fprintf( fp, "Corpse %d\n", pMobIndex->corpse_vnum );

	if ( pMobIndex->skin_multiplier != 0 )
        fprintf( fp, "Skin %d\n", pMobIndex->skin_multiplier );

	if ( pMobIndex->exp_multiplier != 0 )
        fprintf( fp, "ExpMultiplier %d\n", pMobIndex->exp_multiplier );
	else
		fprintf( fp, "ExpMultiplier 100\n" );

    for ( i = 0; i < MAX_RESIST; i++ )
    {
        if ( pMobIndex->resists[ i ] != 0 )
            fprintf( fp, "Resist %d %d\n", i, pMobIndex->resists[ i ] );
    }

    for ( i = 0; i < MAX_RESIST; i++ )
    {
        if ( pMobIndex->healing_from[ i ] != 0 )
            fprintf( fp, "HealingFrom %d %d\n", i, pMobIndex->healing_from[ i ] );
    }

    fprintf( fp, "Mobend\n" );

    return ;
}

void new_save_object( FILE *fp, OBJ_INDEX_DATA *pObjIndex )
{
    SPEC_DAMAGE *specdam;
    AFFECT_DATA *pAf;
    EXTRA_DESCR_DATA *pEd;
    PROG_LIST *pOprog;
    char buf[MAX_STRING_LENGTH];

    fprintf( fp, "#Vnum %d\n",        pObjIndex->vnum );
    fprintf( fp, "Name %s~\n",        pObjIndex->name );
    fprintf( fp, "Odmiana %s~%s~%s~%s~%s~\n", pObjIndex->name2, pObjIndex->name3, pObjIndex->name4, pObjIndex->name5, pObjIndex->name6 );
    fprintf( fp, "Short %s~\n",       pObjIndex->short_descr );
    fprintf( fp, "Descr %s~\n",       fix_string( pObjIndex->description ) );
    fprintf( fp, "Material %s~\n",    material_table[pObjIndex->material].name );
    fprintf( fp, "Type %s\n",         item_name(pObjIndex->item_type ) );
    fprintf( fp, "ExtraExt %s\n",     print_ext_flags( pObjIndex->extra_flags ) );
    fprintf( fp, "Wear %s\n",         fwrite_flag( pObjIndex->wear_flags,  buf ) );
    fprintf( fp, "Wear2Ext %s\n",     print_ext_flags( pObjIndex->wear_flags2 ) );
    fprintf( fp, "Bonus %d\n",        pObjIndex->bonus_set );
    fprintf( fp, "LiczbaMnoga %d\n",  pObjIndex->liczba_mnoga );
    fprintf( fp, "Gender %d\n",       pObjIndex->gender );
    //rellik: komponenty, zapisywanie w definicji obiektu
    if ( pObjIndex->is_spell_item )
    {
        fprintf( fp, "Komponent %d %d\n", pObjIndex->spell_item_counter, pObjIndex->spell_item_timer );
    }

    fprintf
        (
         fp,
         "Value %d %d %d %d %d %d %d\n",
         pObjIndex->value[0],
         pObjIndex->value[1],
         pObjIndex->value[2],
         pObjIndex->value[3],
         pObjIndex->value[4],
         pObjIndex->value[5],
         pObjIndex->value[6]
        );
    /**
     * repair limits
     */
    fprintf
        (
         fp,
         "Repair %d %d %d %d\n",
         pObjIndex->repair_limit,
         pObjIndex->repair_counter,
         pObjIndex->repair_penalty,
         pObjIndex->repair_condition
        );

    fprintf( fp, "Length %d\n",       pObjIndex->length );
    fprintf( fp, "Weight %d\n",       pObjIndex->weight );
    fprintf( fp, "Cost %d\n",         pObjIndex->cost );
    fprintf( fp, "Cond %d\n",         pObjIndex->condition);

    for( pAf = pObjIndex->affected; pAf; pAf = pAf->next )
    {
        fprintf( fp, "Aff %d %d %d %d %d %s\n",
                     pAf->where,
                     pAf->location, pAf->modifier,
                     pAf->level, pAf->bitvector ? (*pAf->bitvector).bank : 0,
                     pAf->bitvector ? fwrite_flag( (*pAf->bitvector).vector, buf ) : "0" );
    }

    for( pEd = pObjIndex->extra_descr; pEd; pEd = pEd->next )
    {
        fprintf( fp, "Extradesc %s~\n%s~\n", pEd->keyword, fix_string( pEd->description ) );
    }

    for ( pOprog = pObjIndex->progs; pOprog; pOprog = pOprog->next )
    {
        fprintf(fp, "Objprog %s %s %s~\n",
                     ext_bit_name( prog_flags, pOprog->trig_type ),
                     pOprog->name,
                     pOprog->trig_phrase );
    }

    if ( pObjIndex->item_description )
    fprintf(fp, "Itemdesc %s~\n", fix_string(pObjIndex->item_description) );

    if ( pObjIndex->ident_description )
    fprintf( fp, "Identdesc %s~\n", fix_string(pObjIndex->ident_description ));

    if ( pObjIndex->hidden_description )
        fprintf( fp, "Hiddendesc %s~\n", fix_string( pObjIndex->hidden_description ) );

    if ( pObjIndex->comments )
        fprintf( fp, "Comments %s~\n", fix_string( pObjIndex->comments ) );

    if ( pObjIndex->item_type == ITEM_WEAPON && pObjIndex->spec_dam )
    {
        for ( specdam = pObjIndex->spec_dam; specdam; specdam = specdam->next )
        {
            fprintf( fp, "SpecDamExt %d %d %d %d %d %s %d %d\n",
                          specdam->type, specdam->chance, specdam->param1,
                          specdam->param2, specdam->param3, print_ext_flags( specdam->ext_param ), specdam->bonus,
                          specdam->target_type );
        }
    }

    if ( pObjIndex->trap > 0 )
    {
        fprintf( fp, "Trap %d\n", pObjIndex->trap );
    }

    fprintf( fp, "Objend\n" );
    return;
}



void new_save_objects( FILE *fp, AREA_DATA *pArea )
{
    int i;
    OBJ_INDEX_DATA *pObj;

    fprintf( fp, "#OBJECTS\n" );

    for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
    {
        if ( (pObj = get_obj_index( i )) )
        {
            new_save_object( fp, pObj );
        }
    }

    fprintf( fp, "#Vnum 0\n\n\n\n" );
    return;
}

void new_save_mobiles( FILE *fp, AREA_DATA *pArea )
{
    int i;
    MOB_INDEX_DATA *pMob;

    fprintf( fp, "#MOBILES\n" );

    for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
    {
	if ( (pMob = get_mob_index( i )) )
	    new_save_mobile( fp, pMob );
    }

    fprintf( fp, "#Vnum 0\n\n\n\n" );
    return;
}

void new_save_rooms( FILE *fp, AREA_DATA *pArea )
{
	ROOM_INDEX_DATA *pRoomIndex;
	EXTRA_DESCR_DATA *pEd;
	EXIT_DATA *pExit;
	PROG_LIST *prog;
	int iHash;
	int door;
	int i;

	fprintf( fp, "#ROOMS\n" );
	for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
		for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex = pRoomIndex->next )
		{
			if ( pRoomIndex->area == pArea )
			{
				fprintf( fp, "#Vnum %d\n",		pRoomIndex->vnum );
				fprintf( fp, "Name %s~\n",		pRoomIndex->name );

				if ( pRoomIndex->day_rand_desc <= 0 )
				{
					fprintf( fp, "Descr %s~\n",		fix_string( pRoomIndex->description ) );
				}

				if ( pRoomIndex->night_rand_desc <= 0 )
				{
					if ( pRoomIndex->nightdesc[0] != '\0' )
						fprintf( fp, "Nightdescr %s~\n", fix_string( pRoomIndex->nightdesc ) );
				}

				fprintf( fp, "FlagsExt %s\n", print_ext_flags( pRoomIndex->room_flags ) );
				fprintf( fp, "Sector %d\n",		pRoomIndex->sector_type );

				for ( pEd = pRoomIndex->extra_descr; pEd;
					pEd = pEd->next )
				{
					fprintf( fp, "Extra %s~\n%s~\n", pEd->keyword,
						fix_string( pEd->description ) );
				}

				for( door = 0; door < MAX_DIR; door++ )	/* I hate this! */
				{
					if ( ( pExit = pRoomIndex->exit[door] )
						&& pExit->u1.to_room )
					{

						/* HACK : TO PREVENT EX_LOCKED etc without EX_ISDOOR
						to stop booting the mud */
						if ( IS_SET(pExit->rs_flags, EX_CLOSED)
							||   IS_SET(pExit->rs_flags, EX_LOCKED)
							||   IS_SET(pExit->rs_flags, EX_PICKPROOF)
							||   IS_SET(pExit->rs_flags, EX_NOPASS)
							||   IS_SET(pExit->rs_flags, EX_EASY)
							||   IS_SET(pExit->rs_flags, EX_HARD)
							||   IS_SET(pExit->rs_flags, EX_INFURIATING)
							||   IS_SET(pExit->rs_flags, EX_NOCLOSE)
							||   IS_SET(pExit->rs_flags, EX_NOLOCK) )
							SET_BIT(pExit->rs_flags, EX_ISDOOR);

						//nowe wyjscia
						fprintf( fp, "ExitExt %d\n", pExit->orig_door );
						fprintf( fp, "ExitExtDescription %s~\n", fix_string( pExit->description ) );

						if ( !IS_NULLSTR( pExit->nightdescription ) )
							fprintf( fp, "ExitExtNightDescription %s~\n", fix_string( pExit->nightdescription ) );
						if ( !IS_NULLSTR( pExit->vName ) )
							fprintf( fp, "ExitExtVName %s~\n",pExit->vName );
						if ( !IS_NULLSTR( pExit->TextOut ) )
							fprintf( fp, "ExitExtTextOut %s~\n",pExit->TextOut );
						if ( !IS_NULLSTR( pExit->TextIn ) )
							fprintf( fp, "ExitExtTextIn %s~\n",pExit->TextIn );
						if ( !IS_NULLSTR( pExit->biernik ) )
							fprintf( fp, "ExitExtBiernik %s~\n",pExit->biernik );
						if ( !IS_NULLSTR( pExit->keyword ) )
							fprintf( fp, "ExitExtName %s~\n",pExit->keyword );

						fprintf( fp, "ExitExtLiczba %d\n", pExit->liczba_mnoga );
						fprintf( fp, "ExitExtInfo %d\n", pExit->rs_flags );
						fprintf( fp, "ExitExtKey %d\n", pExit->key );
						fprintf( fp, "ExitExtToVnum %d\n", pExit->u1.to_room->vnum );
						fprintf( fp, "ExitExtEnd\n" );

						//stare wyjscia
						fprintf( fp, "Exit %d\n%s~\n",
							pExit->orig_door,
							fix_string( pExit->description ) );

						if ( pExit->nightdescription && pExit->nightdescription[0] != '\0' )
							fprintf( fp, "NightExit %s~\n", fix_string( pExit->nightdescription ) );
						if ( pExit->vName && pExit->vName[0] != '\0' ) fprintf( fp, "VName %s~\n",pExit->vName );
						if ( pExit->TextOut && pExit->TextOut[0] != '\0' ) fprintf( fp, "TextOut %s~\n",pExit->TextOut );
						if ( pExit->TextIn && pExit->TextIn[0] != '\0' ) fprintf( fp, "TextIn %s~\n",pExit->TextIn );

						fprintf( fp, "%s~\n%d %d %d\n",
							pExit->keyword,
							pExit->rs_flags,
							pExit->key,
							pExit->u1.to_room->vnum);

						if( pExit->trap > 0 )
							fprintf( fp, "Exittrap %d %d\n",
							pExit->orig_door,
							pExit->trap);

					}
				}

				if (pRoomIndex->heal_rate != 100)
					fprintf ( fp, "Heal %d\n", pRoomIndex->heal_rate);

				if ( pRoomIndex->echo_to.room )
					fprintf ( fp, "EchoTo %d\n", pRoomIndex->echo_to.room->vnum );

				if (pRoomIndex->rent_rate != 100)
					fprintf ( fp, "Rentrate %d\n", pRoomIndex->rent_rate);

				if (pRoomIndex->day_rand_desc > 0)
					fprintf ( fp, "DayRDesc %d\n", pRoomIndex->day_rand_desc);

				if (pRoomIndex->night_rand_desc > 0)
					fprintf ( fp, "NightRDesc %d\n", pRoomIndex->night_rand_desc);

				if (!IS_NULLSTR(pRoomIndex->owner))
					fprintf ( fp, "Owner %s~\n" , pRoomIndex->owner );

				if( pRoomIndex->trap > 0)
					fprintf ( fp, "Trap %d\n" , pRoomIndex->trap );

				if( pRoomIndex->area_part != 0)
					fprintf ( fp, "AreaPart %d\n" , pRoomIndex->area_part );

				for (prog = pRoomIndex->progs; prog; prog = prog->next)
				{
					fprintf(fp, "Roomprog %s %s %s~\n",
						ext_bit_name( prog_flags, prog->trig_type ),
						prog->name,
						prog->trig_phrase);
				}
				//rellik: mining;
				fprintf( fp, "Resources " );
				for ( i = 0; rawmaterial_table[i].name; ++i )
				{
					fprintf( fp, "%d ", pRoomIndex->rawmaterial[i] );
				}
				fprintf( fp, "\n" );
				fprintf( fp, "Capacity %d\n", pRoomIndex->rawmaterial_capacity );

				fprintf( fp, "Roomend\n" );
			}
		}
	}
	fprintf( fp, "#Vnum 0\n\n\n\n" );
	return;
}


void save_desc( FILE *fp, AREA_DATA *pArea )
{
    DESC_DATA *desc;

    fprintf(fp, "#DESC\n");

    for( desc = pArea->desc_data; desc; desc = desc->next )
    {
	    fprintf(fp, "#Name %s~\n",	    desc->name);
	    fprintf(fp, "Desc %s~\n",	    desc->description);
	    fprintf(fp, "Group %d\n",		desc->group);
	    fprintf(fp, "End\n");
    }

    fprintf(fp,"#DescEnd\n\n");
    return;
}

void save_rand_desc( FILE *fp, AREA_DATA *pArea )
{
    RAND_DESC_DATA *rdesc;

    fprintf(fp, "#RANDDESC\n");

    for( rdesc = rand_desc_list; rdesc; rdesc = rdesc->next )
    {
    	if ( rdesc->area == pArea )
    	{
    	    fprintf(fp, "#Vnum %d\n",		rdesc->vnum );
    	    fprintf(fp, "DescRand %s~\n",	rdesc->rand_desc );
    	    fprintf(fp, "RandDescEnd\n");
    	}
    }

    fprintf(fp,"#Vnum 0\n\n");
    return;
}

void save_bonus_set( FILE *fp, AREA_DATA *pArea )
{
    BONUS_INDEX_DATA * pBonus;
    EXTRA_DESCR_DATA * pEd;
    AFFECT_DATA * pAf;
    char buf [MAX_STRING_LENGTH];

    fprintf(fp, "#BONUS_SET\n");

    for( pBonus = bonus_list; pBonus; pBonus = pBonus->next )
    {
    	if ( pBonus->area == pArea )
    	{
    	    fprintf(fp, "#Vnum %d\n",		pBonus->vnum );

            if ( !IS_NULLSTR( pBonus->name ) )
                fprintf( fp, "Name %s~\n", pBonus->name );

            if ( !IS_NULLSTR( pBonus->description ) )
    	        fprintf(fp, "Desc %s~\n",	    pBonus->description );

            if ( !IS_NULLSTR( pBonus->comments ) )
                fprintf( fp, "Comments %s~\n", pBonus->comments );

            for( pAf = pBonus->affects; pAf; pAf = pAf->next )
            {
                fprintf( fp, "Aff %d %d %d %d %d %s\n",
                             pAf->where,
                             pAf->location, pAf->modifier,
                             pAf->level, pAf->bitvector ? (*pAf->bitvector).bank : 0,
                             pAf->bitvector ? fwrite_flag( (*pAf->bitvector).vector, buf ) : "0" );
            }

            if ( !IS_NULLSTR( pBonus->wear_prog ) )
                fprintf( fp, "WearProg %s~\n", pBonus->wear_prog );

            if ( !IS_NULLSTR( pBonus->remove_prog ) )
                fprintf( fp, "RemoveProg %s~\n", pBonus->remove_prog );

			for( pEd = pBonus->extra_descr; pEd; pEd = pEd->next )
				fprintf( fp, "Extradesc %s~\n%s~\n", pEd->keyword, fix_string( pEd->description ) );

    	    fprintf(fp, "BonusEnd\n");
    	}
    }

    fprintf(fp,"#Vnum 0\n\n");
    return;
}
