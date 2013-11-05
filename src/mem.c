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
 *  File: mem.c                                                        *
 *                                                                     *
 *  Much time and thought has gone into this software and you are      *
 *  benefitting.  We hope that you share your changes too.  What goes  *
 *  around, comes around.                                              *
 *                                                                     *
 *  This code was freely distributed with the The Isles 1.1 source     *
 *  code, and has been used here for OLC - OLC would not be what it    *
 *  is without all the previous coders who released their source code. *
 *                                                                     *
 ***********************************************************************
 *                                                                     *
 *  KILLER MUD is copyright 1999-2010 Killer MUD Staff (alphabetical)  *
 *                                                                     *
 *   ZMIENIA£E¦ CO¦? DOPISZ SIÊ!                                       *
 *                                                                     *
 *   Mierzwa Michal        (nil.michal@googlewave.com     ) [Rellik  ] *
 *   Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg  ] *
 *   Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas  ] *
 *   Skrzetnicki Krzysztof (gtener@gmail.com              ) [Tener   ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: mem.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/mem.c $
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
#include "lang.h"
#include "recycle.h"

/*
 * Globals
 */
extern          int                     top_reset;
extern          int                     top_area;
extern          int                     top_exit;
extern          int                     top_ed;
extern          unsigned int                     top_room;

AREA_DATA		*	area_free = NULL;
EXIT_DATA		*	exit_free = NULL;
ROOM_INDEX_DATA		*	room_index_free = NULL;
OBJ_INDEX_DATA		*	obj_index_free = NULL;
SHOP_DATA		*	shop_free = NULL;
BANK_DATA		*	bank_free = NULL;
MOB_INDEX_DATA		*	mob_index_free = NULL;
RESET_DATA		*	reset_free = NULL;
HELP_DATA		*	help_last = NULL;
BONUS_INDEX_DATA	*	bonus_index_free = NULL;
SONG_DATA	*	song_index_free = NULL;

void	free_extra_descr	args( ( EXTRA_DESCR_DATA *pExtra ) );
void	free_affect		args( ( AFFECT_DATA *af ) );
void	free_prog              args ( (  PROG_LIST *mp ) );



RESET_DATA *new_reset_data( void )
{
    RESET_DATA *pReset;

    if ( !reset_free )
    {
        CREATE( pReset, RESET_DATA, 1 );
        top_reset++;
    }
    else
    {
        pReset          =   reset_free;
        reset_free      =   reset_free->next;
    }

    pReset->next        =   NULL;
    pReset->command     =   'X';
    pReset->arg1        =   0;
    pReset->arg2        =   0;
    pReset->arg3        =   0;
    pReset->arg4	=   0;

    return pReset;
}



void free_reset_data( RESET_DATA *pReset )
{
    pReset->next            = reset_free;
    reset_free              = pReset;
    return;
}



AREA_DATA *new_area( void )
{
    AREA_DATA *pArea;
    char buf[MAX_INPUT_LENGTH];

    if ( !area_free )
    {
        CREATE( pArea, AREA_DATA, 1 );
        top_area++;
    }
    else
    {
        pArea       =   area_free;
        area_free   =   area_free->next;
    }

    pArea->next             =   NULL;
    pArea->desc_data        =   NULL;
    pArea->name             =   str_dup( "New area" );
    pArea->area_flags       =   AREA_ADDED;
    pArea->security         =   1;
    pArea->builders         =   str_dup( "None" );
	pArea->locked           =   str_dup( "" );
	pArea->credits          =   str_dup( "" );
    pArea->min_vnum         =   0;
    pArea->max_vnum         =   0;
    pArea->region           =   0;
    pArea->age              =   0;
    pArea->resetage         =   20;
    pArea->nplayer          =   0;
    pArea->empty            =   TRUE;
    pArea->vnum             =   top_area-1;
    sprintf( buf, "area%d.are", pArea->vnum );
    pArea->file_name        =   str_dup( buf );

    return pArea;
}



void free_area( AREA_DATA *pArea )
{
    free_string( pArea->name );
    free_string( pArea->file_name );
    free_string( pArea->builders );
    free_string( pArea->credits );

    pArea->next         =   area_free->next;
    area_free           =   pArea;
    return;
}



EXIT_DATA *new_exit( void )
{
    EXIT_DATA *pExit;

    if ( !exit_free )
    {
        CREATE( pExit, EXIT_DATA, 1 );
        top_exit++;
    }
    else
    {
        pExit           =   exit_free;
        exit_free       =   exit_free->next;
    }

    pExit->u1.to_room   =   NULL;
    pExit->next         =   NULL;
    pExit->exit_info    =   0;
    pExit->key          =   0;
    pExit->keyword      =   &str_empty[0];
    pExit->description  =   &str_empty[0];
    pExit->nightdescription  =   &str_empty[0];
    pExit->vName        =   &str_empty[0];
    pExit->TextOut      =   &str_empty[0];
    pExit->TextIn       =   &str_empty[0];
    pExit->rs_flags     =   0;
    pExit->biernik      =   &str_empty[0];
    pExit->liczba_mnoga =	TRUE;

    return pExit;
}



void free_exit( EXIT_DATA *pExit )
{
    free_string( pExit->keyword );
    free_string( pExit->description );
    free_string( pExit->nightdescription );
    free_string( pExit->vName );
    free_string( pExit->TextOut );
    free_string( pExit->TextIn );

    pExit->next         =   exit_free;
    exit_free           =   pExit;
    return;
}


ROOM_INDEX_DATA *new_room_index( void )
{
    ROOM_INDEX_DATA *pRoom;
    int door;

    if ( !room_index_free )
    {
        CREATE( pRoom, ROOM_INDEX_DATA, 1 );
        top_room++;
    }
    else
    {
        pRoom           =   room_index_free;
        room_index_free =   room_index_free->next;
    }

    pRoom->next             =   NULL;
    pRoom->people           =   NULL;
    pRoom->contents         =   NULL;
    pRoom->extra_descr      =   NULL;
    pRoom->area             =   NULL;
	pRoom->minds            =   NULL;

    for ( door=0; door < MAX_DIR; door++ )
        pRoom->exit[door]   =   NULL;

    pRoom->name             =   &str_empty[0];
    pRoom->description      =   &str_empty[0];
    pRoom->nightdesc        =   &str_empty[0];
    pRoom->night_rand_desc  =   0;
    pRoom->day_rand_desc    =   0;
    pRoom->owner	    =	&str_empty[0];
    pRoom->vnum             =   0;
    ext_flags_clear( pRoom->room_flags );
    pRoom->light            =   0;
    pRoom->sector_type      =   8;
    pRoom->heal_rate	    =   100;
    pRoom->rent_rate	    =   100;
    pRoom->herbs_count	    =   0;
	pRoom->area_part	    =   0;
    ext_flags_clear( pRoom->prog_flags );
    pRoom->prog_delay	    =   0;
    pRoom->prog_target	    =   NULL;
    pRoom->echo_to.vnum     = 0;
    pRoom->has_memdat		= 0;
    return pRoom;
}



void free_room_index( ROOM_INDEX_DATA *pRoom )
{
    int door;
    EXTRA_DESCR_DATA *pExtra;
    RESET_DATA *pReset;

    free_string( pRoom->name );
    free_string( pRoom->description );
    free_string( pRoom->owner );

    for ( door = 0; door < MAX_DIR; door++ )
    {
        if ( pRoom->exit[door] )
            free_exit( pRoom->exit[door] );
    }

    for ( pExtra = pRoom->extra_descr; pExtra; pExtra = pExtra->next )
    {
        free_extra_descr( pExtra );
    }

    for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
    {
        free_reset_data( pReset );
    }

    pRoom->next     =   room_index_free;
    room_index_free =   pRoom;
    return;
}

extern AFFECT_DATA *affect_free;


SHOP_DATA *new_shop( void )
{
    SHOP_DATA *pShop;
    int buy;

    if ( !shop_free )
    {
        CREATE( pShop, SHOP_DATA, 1 );
        top_shop++;
    }
    else
    {
        pShop           =   shop_free;
        shop_free       =   shop_free->next;
    }

    pShop->next         =   NULL;
    pShop->keeper       =   0;

    for ( buy=0; buy<MAX_TRADE; buy++ )
        pShop->buy_type[buy]    =   0;

    pShop->profit_buy   =   100;
    pShop->profit_sell  =   100;
    pShop->open_hour    =   0;
    pShop->close_hour   =   23;
    pShop->shop_room    =   0;

    return pShop;
}



void free_shop( SHOP_DATA *pShop )
{
    pShop->next = shop_free;
    shop_free   = pShop;
    return;
}

BANK_DATA *new_bank( void )
{
    BANK_DATA *pBank;

    if ( !bank_free )
    {
        CREATE( pBank, BANK_DATA, 1 );
        top_bank++;
    }
    else
    {
        pBank           =   bank_free;
        bank_free       =   bank_free->next;
    }

    pBank->next         =   NULL;
    pBank->banker       =   0;
    pBank->provision   =   0;
    pBank->open_hour    =   0;
    pBank->close_hour   =   23;
    pBank->bank_room    =   0;

    return pBank;
}



void free_bank( BANK_DATA *pBank )
{
    pBank->next = bank_free;
    bank_free   = pBank;
    return;
}

OBJ_INDEX_DATA *new_obj_index( void )
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( !obj_index_free )
    {
        CREATE( pObj, OBJ_INDEX_DATA, 1 );
        top_obj_index++;
    }
    else
    {
        pObj            =   obj_index_free;
        obj_index_free  =   obj_index_free->next;
    }

    pObj->next          =   NULL;
    pObj->extra_descr   =   NULL;
    pObj->affected      =   NULL;
    pObj->area          =   NULL;
    pObj->spec_dam      =   NULL;
    pObj->name          =   str_dup( "no name" );

    pObj->name2	        =   str_dup( "null" );
    pObj->name3	        =   str_dup( "null" );
    pObj->name4	        =   str_dup( "null" );
    pObj->name5	        =   str_dup( "null" );
    pObj->name6	        =   str_dup( "null" );

    pObj->short_descr   =   str_dup( "(no short description)" );
    pObj->description   =   str_dup( "(no description)" );
    pObj->description   =   str_dup( "" );

    pObj->item_description   =   NULL;
    pObj->ident_description  =   NULL;
	pObj->hidden_description  =   NULL;
    pObj->comments           =   str_dup("");

    pObj->vnum          =   0;
    pObj->item_type     =   ITEM_TRASH;
    ext_flags_clear( pObj->extra_flags );
    pObj->wear_flags    =   0;
    ext_flags_clear( pObj->wear_flags2 );
    pObj->count         =   0;
    pObj->weight        =   1;
    pObj->cost          =   1;
    pObj->material      =   0;
    pObj->condition     =   75;
    pObj->rent_cost		=   0;
    pObj->bonus_set		=   0;
    pObj->trap			=   0;
    pObj->liczba_mnoga 	= 	FALSE;
    pObj->gender 		=	0;
    //rellik: komponenty, nowa definicja obiektu nie jest komponentem
    pObj->is_spell_item = FALSE;

    for ( value = 0; value < 7; value++ )
        pObj->value[value]  =   0;

    pObj->new_format    = TRUE;

    return pObj;
}



void free_obj_index( OBJ_INDEX_DATA *pObj )
{
    EXTRA_DESCR_DATA *pExtra;
    AFFECT_DATA *pAf;

    free_string( pObj->name );
    free_string( pObj->name2 );
    free_string( pObj->name3 );
    free_string( pObj->name4 );
    free_string( pObj->name5 );
    free_string( pObj->name6 );
    free_string( pObj->short_descr );
    free_string( pObj->description );
    free_string( pObj->item_description );
    free_string( pObj->ident_description );
	free_string( pObj->hidden_description );
    free_string( pObj->comments );

    for ( pAf = pObj->affected; pAf; pAf = pAf->next )
    {
        free_affect( pAf );
    }

    for ( pExtra = pObj->extra_descr; pExtra; pExtra = pExtra->next )
    {
        free_extra_descr( pExtra );
    }

    pObj->next              = obj_index_free;
    obj_index_free          = pObj;
    free_prog( pObj->progs );
    return;
}

BONUS_INDEX_DATA *new_bonus_index( void )
{
    BONUS_INDEX_DATA *pBonus;

    if ( !bonus_index_free )
        CREATE( pBonus, BONUS_INDEX_DATA, 1 );
    else
    {
        pBonus            =   bonus_index_free;
        bonus_index_free  =   bonus_index_free->next;
    }

    pBonus->next        =   NULL;
    pBonus->affects     =   NULL;
    pBonus->area        =   NULL;
    ext_flags_clear( pBonus->prog_flags );
    pBonus->name        =   &str_empty[0];
    pBonus->description =   &str_empty[0];
    pBonus->comments    =   &str_empty[0];
    pBonus->wear_prog   =   &str_empty[0];
    pBonus->remove_prog =   &str_empty[0];
    pBonus->extra_descr =   NULL;

    pBonus->vnum          =   0;

    return pBonus;
}

void free_bonus_index( BONUS_INDEX_DATA *pBonus )
{
    AFFECT_DATA *pAf;

    free_string( pBonus->name );
    free_string( pBonus->description );
    free_string( pBonus->comments );
    free_string( pBonus->wear_prog );
    free_string( pBonus->remove_prog );

    for ( pAf = pBonus->affects; pAf; pAf = pAf->next )
    {
        free_affect( pAf );
    }

    pBonus->next     = bonus_index_free;
    bonus_index_free = pBonus;

    return;
}

SONG_DATA *new_song_index( void )
{
    SONG_DATA *pSong;

    if ( !song_index_free )
        CREATE( pSong, SONG_DATA, 1 );
    else
    {
        pSong             = song_index_free;
        song_index_free   = song_index_free->next;
    }

    pSong->next        = NULL;
    pSong->perform     = NULL;
    pSong->name        = &str_empty[0];
    pSong->description = &str_empty[0];
    pSong->comments    = &str_empty[0];
    pSong->type        = 0;
    pSong->slots       = 0;
    pSong->target      = 0;

    return pSong;
}

void free_song_index( SONG_DATA *pSong )
{
    PERFORM_DATA *pPerf, *pPerf_next;

    free_string( pSong->name );
    free_string( pSong->description );
    free_string( pSong->comments );

    for ( pPerf = pSong->perform; pPerf; pPerf = pPerf_next )
    {
        pPerf_next = pPerf->next;
        free_string( pPerf->perform_prog[0] );
        free_string( pPerf->perform_prog[1] );
        free_string( pPerf->perform_prog[2] );
        free_string( pPerf->perform_prog[3] );
        DISPOSE( pPerf );
    }

    pSong->perform   = NULL;

    pSong->next      = song_index_free;
    song_index_free = pSong;

    return;
}

MOB_INDEX_DATA *new_mob_index( void )
{
    MOB_INDEX_DATA *pMob;
    int i;

    if ( !mob_index_free )
    {
        CREATE( pMob, MOB_INDEX_DATA, 1 );
        top_mob_index++;
    }
    else
    {
        pMob            =   mob_index_free;
        mob_index_free  =   mob_index_free->next;
    }

    pMob->next          =   NULL;
    pMob->spec_fun      =   NULL;
    pMob->pShop         =   NULL;
    pMob->pRepair	=   NULL;
    pMob->area          =   NULL;
    pMob->player_name   =   str_dup( "no name" );

    pMob->name2	        =   str_dup( "null" );
    pMob->name3	        =   str_dup( "null" );
    pMob->name4	        =   str_dup( "null" );
    pMob->name5	        =   str_dup( "null" );
    pMob->name6	        =   str_dup( "null" );

    pMob->short_descr   =   str_dup( "(no short description)" );
    pMob->long_descr    =   str_dup( "(no long description)" );
    pMob->description   =   &str_empty[0];
    pMob->vnum          =   0;
    pMob->count         =   0;
    pMob->killed        =   0;
    pMob->sex           =   0;
    pMob->level         =   0;
    ext_flags_clear( pMob->act );
    EXT_SET_BIT( pMob->act, ACT_IS_NPC );
    pMob->languages	=   LANG_COMMON;
    pMob->speaking	=   0;
    pMob->corpse_vnum	=   0;
	pMob->skin_multiplier	=   0;

    for(i=0;i < 16 ;i++)
	pMob->spells[i]	=   0;

    for(i=0;i < MAX_RESIST ;i++)
       {	
	  pMob->resists[i] =  0;
	  pMob->healing_from[i] = 0;
       }

    for(i=0;i<MAX_VECT_BANK;i++)
	pMob->affected_by[i]   =   0;

    pMob->alignment     =   0;
    pMob->hitroll	=   0;
    pMob->race          =   race_lookup( "human" );
    pMob->form          =   0;
    pMob->parts         =   0;
    pMob->material      =   str_dup("unknown");
    ext_flags_clear( pMob->off_flags );
    pMob->size          =   SIZE_MEDIUM;
    pMob->ac[AC_PIERCE]	=   100;
    pMob->ac[AC_BASH]	=   100;
    pMob->ac[AC_SLASH]	=   100;
    pMob->ac[AC_EXOTIC]	=   100;
    pMob->stats[0]	=13;
    pMob->stats[1]	=13;
    pMob->stats[2]	=13;
    pMob->stats[3]	=13;
    pMob->stats[4]	=13;
    pMob->stats[5]	=13;
    pMob->stats[6]	=13;
    pMob->hit[DICE_NUMBER]	=   0;
    pMob->hit[DICE_TYPE]	=   0;
    pMob->hit[DICE_BONUS]	=   0;
    pMob->damage[DICE_NUMBER]	=   0;
    pMob->damage[DICE_TYPE]	=   0;
    pMob->damage[DICE_NUMBER]	=   0;
    pMob->start_pos             =   POS_STANDING;
    pMob->default_pos           =   POS_STANDING;
    
    pMob->wealth                =   0;
    
    pMob->comments              =   str_dup("");

    pMob->new_format            = TRUE;

    return pMob;
}



void free_mob_index( MOB_INDEX_DATA *pMob )
{
    free_string( pMob->player_name );
    free_string( pMob->name2 );
    free_string( pMob->name3 );
    free_string( pMob->name4 );
    free_string( pMob->name5 );
    free_string( pMob->name6 );
    free_string( pMob->short_descr );
    free_string( pMob->long_descr );
    free_string( pMob->description );
    free_prog( pMob->progs );

    free_shop( pMob->pShop );
    free_repair( pMob->pRepair );
    free_bank( pMob->pBank );
    pMob->pShop   = NULL;
    pMob->pBank   = NULL;
    pMob->pRepair = NULL;

    pMob->next              = mob_index_free;
    mob_index_free          = pMob;
    return;
}

PROG_CODE              *       pcode_free = NULL;


PROG_CODE *new_pcode(void)
{
     PROG_CODE *NewCode;

     if (!pcode_free)
     {
         CREATE( NewCode, PROG_CODE, 1);
     }
     else
     {
         NewCode     = pcode_free;
         pcode_free = pcode_free->next;
     }

     NewCode->name           = str_dup("");
     NewCode->description    = str_dup("");
     NewCode->code           = str_dup("");
     NewCode->next           = NULL;

     return NewCode;
}

TRAP_DATA              *       trap_data_free = NULL;

TRAP_DATA *new_trapdata(void)
{
    TRAP_DATA *new_trap;

    if ( !trap_data_free )
    {
        CREATE( new_trap, TRAP_DATA, 1 );
    }
    else
    {
        new_trap   = trap_data_free;
	trap_data_free = trap_data_free->next;
    }

    new_trap->vnum	= 0;
    new_trap->triggered	= &str_empty[0];
    new_trap->disarmed	= &str_empty[0];
    new_trap->failed_disarm	= &str_empty[0];
    new_trap->next	= NULL;

     return new_trap;
}

DESC_DATA              *       desc_data_free = NULL;

DESC_DATA *new_descdata(void)
{
    DESC_DATA *new_desc;

    if ( !desc_data_free )
    {
        CREATE( new_desc, DESC_DATA, 1 );
    }
    else
    {
        new_desc   = desc_data_free;
	    desc_data_free = desc_data_free->next;
    }

    new_desc->name = str_dup("");
    new_desc->description = str_dup("");
    new_desc->group       = 0;
    new_desc->next	= NULL;

    return new_desc;
}

RAND_DESC_DATA              *       rdesc_data_free = NULL;

RAND_DESC_DATA *new_rdescdata(void)
{
    RAND_DESC_DATA *new_rdesc;

    if ( !rdesc_data_free )
    {
        CREATE( new_rdesc, RAND_DESC_DATA, 1 );
    }
    else
    {
        new_rdesc   = rdesc_data_free;
	    rdesc_data_free = rdesc_data_free->next;
    }

    new_rdesc->vnum	= 0;
    new_rdesc->area = NULL;
    new_rdesc->rand_desc = str_dup("");
    new_rdesc->next	= NULL;

    return new_rdesc;
}



void free_pcode(PROG_CODE *pMcode)
{
    free_string(pMcode->code);
    pMcode->next = pcode_free;
    pcode_free  = pMcode;
    return;
}

void free_trapdata( TRAP_DATA *trap )
{
    free_string(trap->triggered);
    free_string(trap->disarmed);
    free_string(trap->failed_disarm);

    trap->next = trap_data_free;
    trap_data_free  = trap;
    return;
}

void free_descdata( DESC_DATA *desc )
{
    free_string(desc->description);
    free_string(desc->name);

    desc->next = desc_data_free;
    desc_data_free  = desc;
    return;
}

void free_rdescdata( RAND_DESC_DATA *rdesc )
{
    free_string(rdesc->rand_desc);

    rdesc->next = rdesc_data_free;
    rdesc_data_free  = rdesc;
    return;
}

SPEC_DAMAGE *       spec_damage_free = NULL;


SPEC_DAMAGE *new_spec_damage(void)
{
    SPEC_DAMAGE *new_specdam;

    if ( !spec_damage_free )
    {
        CREATE( new_specdam, SPEC_DAMAGE, 1 );
    }
    else
    {
        new_specdam       = spec_damage_free;
	spec_damage_free  = spec_damage_free->next;
    }
    new_specdam->type        = SD_TYPE_NONE;
    new_specdam->chance      = 100;
    new_specdam->param1      = SD_PARAM_ANY;
    new_specdam->param2      = SD_PARAM_ANY;
    new_specdam->param3      = SD_PARAM_ANY;
    ext_flags_clear( new_specdam->ext_param );
    new_specdam->bonus       = 0;
    new_specdam->next        = NULL;
    new_specdam->target_type = SD_TARGET_DAM;


    return new_specdam;
}

void free_spec_damage( SPEC_DAMAGE *specdam )
{
    specdam->next = spec_damage_free;
    spec_damage_free = specdam;
    return;
}
