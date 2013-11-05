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
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: load_fun.c 10989 2012-02-18 11:33:25Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/load_fun.c $
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include "merc.h"
#include "db.h"
#include "tables.h"
#include "recycle.h"
#include "lang.h"
#include "olc.h"
#include "herbs.h"
#include "progs.h"

bool fread_mobile( FILE *fp )
{
    MOB_INDEX_DATA * pMobIndex = NULL;
    char *buf;
    int i, num = 0;
    bool match = FALSE;
    bool vnum_found = FALSE;
    char letter;
    ush_int vnum = 0;
    int iHash;
    char *word;
    bool newmp = FALSE;

    if ( !area_last )
    {
        bug( "Fread_mobile: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    /* szukamy pierwszego '#'*/
    do
    {
        letter = fread_letter( fp );

        if ( feof( fp ) )
        {
            bug( "Fread_mobile: nie znaleziono '#' & EOF", 0 );
            return FALSE;
        }

    }
    while ( letter != '#' );


    /* wczytujemy mobilesa */
    while ( TRUE )
    {
        word = fread_word( fp );

        if ( num > 0 && !vnum_found )
            return FALSE;

        num++;
        match = FALSE;

        newmp = FALSE;
        if ( !str_cmp( word, "MobprogNew" ) )
            newmp = TRUE;
        switch ( UPPER( word[ 0 ] ) )
        {
            case 'A':
                if ( !str_cmp( word, "Act" ) )
                {
                	long old_act;
                    old_act = fread_flag( fp ) | 1;
                    pMobIndex->act[0] = old_act;
                    match = TRUE;
                    break;
                }
                if ( !str_cmp( word, "ActExt" ) )
                {
                	ext_flags_clear( pMobIndex->act );
                	fread_ext_flags( fp, pMobIndex->act );
                	EXT_SET_BIT( pMobIndex->act, ACT_IS_NPC );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "AffectedExt" ) )
                {
					fread_ext_flags( fp, pMobIndex->affected_by );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "AffectedExt" ) )
                {
					fread_to_eol( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Affected" ) )
                {
                    fread_number( fp );
                    fread_number( fp );
                    fread_number( fp );
                    fread_number( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Align" ) )
                {
                    pMobIndex->alignment = fread_number( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Attack" ) )
                {
                    pMobIndex->dam_type = attack_lookup( fread_word( fp ) );
                    match = TRUE;
                    break;
                }
                break;
            case 'C':
                if ( !str_cmp( word, "Corpse" ) )
                {
                    pMobIndex->corpse_vnum = fread_number( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Comments" ) )
                {
                    free_string( pMobIndex->comments );
                    pMobIndex->comments = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;

            case 'D':
                if ( !str_cmp( word, "Damage" ) )
                {
                    pMobIndex->damage[ DICE_NUMBER ] = fread_number( fp );
                    fread_letter( fp );
                    pMobIndex->damage[ DICE_TYPE ] = fread_number( fp );
                    fread_letter( fp );
                    pMobIndex->damage[ DICE_BONUS ] = fread_number( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Damflags" ) )
                {
                    pMobIndex->attack_flags = fread_number( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Dammagic" ) )
                {
                    pMobIndex->magical_damage = fread_number( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Descr" ) )
                {
                    free_string( pMobIndex->description );
                    buf = fread_string( fp );
                    pMobIndex->description = str_dup( capitalize( buf ) );
					free_string( buf );
                    match = TRUE;
                    break;
                }
                break;
            case 'E':
                if ( !str_cmp( word, "ExpMultiplier" ) )
                {
                    pMobIndex->exp_multiplier = fread_number( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'F':
                if ( !str_cmp( word, "Form" ) )
                {
                    pMobIndex->form = fread_flag( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'G':
                if ( !str_cmp( word, "Group" ) )
                {
                    pMobIndex->group = fread_number( fp );
                    match = TRUE;
                    break;
                }
                break;

            case 'H':
                if ( !str_cmp( word, "Hitroll" ) )
                {
                    pMobIndex->hitroll = fread_number( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "HP" ) )
                {
                    pMobIndex->hit[ DICE_NUMBER ] = fread_number( fp );
                    /* 'd'          */              fread_letter(  fp );
                    pMobIndex->hit[ DICE_TYPE ]   = fread_number( fp );
                    /* '+'          */              fread_letter(  fp );
                    pMobIndex->hit[ DICE_BONUS ]  = fread_number( fp );
                    match = TRUE;
                    break;
                }
		else if ( !str_cmp( word, "HealingFrom" ) )
                {
                    int type, value;

                    type = fread_number( fp );
                    value = fread_number( fp );

                    pMobIndex->healing_from[ type ] = value;
                    match = TRUE;
                    break;
                }
                break;
            case 'L':
                if ( !str_cmp( word, "Lang" ) )
                {
                    pMobIndex->languages = fread_flag( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Level" ) )
                {
                    pMobIndex->level = fread_number( fp );
                    pMobIndex->level = UMIN( pMobIndex->level, MAX_LEVEL );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Long" ) )
                {
                    buf = fread_string( fp );
                    if ( strlen( buf ) > 1 && buf[strlen( buf ) - 2] == '\n' )
                        buf[strlen( buf ) - 2] = '\0';

                    free_string( pMobIndex->long_descr );
                    pMobIndex->long_descr = str_dup( capitalize( buf ) );
					free_string( buf );
                    match = TRUE;
                    break;
                }
                break;
            case 'M':
                if ( !str_cmp( word, "Material" ) )
                {
                    pMobIndex->material = str_dup( fread_word( fp ) );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Mobend" ) )
                {
                    fread_to_eol( fp );

                    newmobs++;

                    ext_flags_copy( ext_flags_sum( pMobIndex->act, ext_flag_value( act_flags, race_table[ pMobIndex->race ].act ) ), pMobIndex->act );
                    ext_flags_copy( ext_flags_sum( pMobIndex->off_flags, ext_flag_value( off_flags, race_table[ pMobIndex->race ].off ) ), pMobIndex->off_flags );
                    pMobIndex->form      = pMobIndex->form | race_table[ pMobIndex->race ].form;
                    pMobIndex->parts     = pMobIndex->parts | race_table[ pMobIndex->race ].parts;

                    vnum = pMobIndex->vnum;
                    iHash = vnum % MAX_KEY_HASH;
                    pMobIndex->next = mob_index_hash[ iHash ];
                    mob_index_hash[ iHash ] = pMobIndex;
                    top_mob_index++;
                    top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob;
                    assign_area_vnum( vnum );
                    kill_table[ URANGE( 0, 0, MAX_LEVEL - 1 ) ].number++;

                    return TRUE;
                }
                else if ( !str_cmp( word, "Mobprog" ) || !str_cmp( word, "MobprogNew" ) )
                {
                    PROG_LIST * pMprog;
                    PROG_LIST * tMprog;
                    char *word;
                    BITVECT_DATA * trigger;

                    CREATE( pMprog, PROG_LIST, 1 );
                    word = fread_word( fp );

					trigger = ext_flag_lookup( word, prog_flags );

                    if ( trigger == &EXT_NONE )
                    {
                        bugf( "Fread_mobile MOBprogs: invalid trigger %s.", word );
                        exit( 1 );
                    }

                    if ( !check_trigger( 0, word ) )
                    {
                        bugf( "Fread_Mobiles : bad, baad trigger %s", word );
                        exit( 1 );
                    }

                    EXT_SET_BIT( pMobIndex->prog_flags, *trigger );
                    pMprog->trig_type       = trigger;
                    pMprog->name            = str_dup( fread_word( fp ) );
                    pMprog->valid_positions = mprog_trigger_default_pos( pMprog->trig_type );
                    if ( newmp )
                        pMprog->valid_positions = fread_flag( fp );
                    pMprog->trig_phrase = fread_string( fp );

                    if (pMobIndex->progs==NULL)
                    {
                        pMprog->next        = pMobIndex->progs;
                        pMobIndex->progs    = pMprog;
                    }
                    else
                    {
                        for ( tMprog = pMobIndex->progs; tMprog->next; tMprog = tMprog->next );
                       tMprog->next    = pMprog;
                    }

                    match = TRUE;
                    break;
                }
                break;
            case 'N':
                if ( !str_cmp( word, "Name" ) )
                {
                    free_string( pMobIndex->player_name );
                    pMobIndex->player_name = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'O':
                if ( !str_cmp( word, "Odmiana" ) )
                {
                    free_string( pMobIndex->name2 );
                    free_string( pMobIndex->name3 );
                    free_string( pMobIndex->name4 );
                    free_string( pMobIndex->name5 );
                    free_string( pMobIndex->name6 );
                    pMobIndex->name2 = fread_string( fp );
                    pMobIndex->name3 = fread_string( fp );
                    pMobIndex->name4 = fread_string( fp );
                    pMobIndex->name5 = fread_string( fp );
                    pMobIndex->name6 = fread_string( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Off" ) )
                {
                	long old_off_flags;

                    old_off_flags = fread_flag( fp );
                    pMobIndex->off_flags[0] = old_off_flags;
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "OffExt" ) )
                {
                	ext_flags_clear( pMobIndex->off_flags );
                	fread_ext_flags( fp, pMobIndex->off_flags );
                    match = TRUE;
                    break;
                }
                break;

            case 'P':
                if ( !str_cmp( word, "Parts" ) )
                {
                    pMobIndex->parts = fread_flag( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Pos_def" ) )
                {
                    pMobIndex->default_pos = position_lookup( fread_word( fp ) );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Pos_start" ) )
                {
                    pMobIndex->start_pos = position_lookup( fread_word( fp ) );
                    match = TRUE;
                    break;
                }
                break;

            case 'R':
                if ( !str_cmp( word, "Race" ) )
                {
					char * race;
					race = fread_string( fp );
                    pMobIndex->race = race_lookup( race );
					free_string( race );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Resist" ) )
                {
                    int type, value;

                    type = fread_number( fp );
                    value = fread_number( fp );

                    pMobIndex->resists[ type ] = value;
                    match = TRUE;
                    break;
                }
                break;

            case 'S':
                if ( !str_cmp( word, "Sex" ) )
                {
                    pMobIndex->sex = sex_lookup( fread_word( fp ) );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Short" ) )
                {
                    free_string( pMobIndex->short_descr );
                    pMobIndex->short_descr = fread_string( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Size" ) )
                {
                    CHECK_POS( pMobIndex->size, size_lookup( fread_word( fp ) ), "size" );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Skin" ) )
                {
                    pMobIndex->skin_multiplier = fread_number( fp );
                    match = TRUE;
                    break;
                }
				else if ( !str_cmp( word, "Speaks" ) )
                {
                    pMobIndex->speaking = fread_number( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Spells" ) )
                {
                    int i, x;
                    int value[ 4 ];

                    for ( i = 0;i < 4;i++ )
                        value[ i ] = fread_number( fp );

                    for ( i = 0;i < 16 && pMobIndex->spells[ i ] != 0;i++ );

                    for ( x = 0 ; i < 16 && x < 4 && value[ x ] != 0; x++, i++ )
                        pMobIndex->spells[ i ] = value[ x ];

                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Stats" ) )
                {
                    for ( i=0; i< MAX_STATS; i++ )
                    {
                        pMobIndex->stats[ i ] = fread_number( fp );
                    }
                    match = TRUE;
                    break;
                }
                break;
            case 'V':
                if ( !str_cmp( word, "Vnum" ) )
                {
                    ush_int vnum;

                    vnum = fread_number( fp );
                    if ( vnum == 0 )
                        return FALSE;

                    CREATE( pMobIndex, MOB_INDEX_DATA, 1 );

                    for ( i = 0;i < 16;i++ )
                        pMobIndex->spells[ i ] = 0;

                    pMobIndex->pShop = NULL;
                    pMobIndex->pBank = NULL;
                    pMobIndex->area = area_last;
                    pMobIndex->new_format = TRUE;
                    pMobIndex->spec_fun = NULL;
                    pMobIndex->progs = NULL;
                    pMobIndex->group = 0;
                    ext_flags_clear( pMobIndex->act );
                    pMobIndex->player_name = str_dup( "" );
                    pMobIndex->name2 = str_dup( "" );
                    pMobIndex->name3 = str_dup( "" );
                    pMobIndex->name4 = str_dup( "" );
                    pMobIndex->name5 = str_dup( "" );
                    pMobIndex->name6 = str_dup( "" );
                    pMobIndex->short_descr = str_dup( "" );
                    pMobIndex->long_descr = str_dup( "" );
                    pMobIndex->description = str_dup( "" );
                    pMobIndex->corpse_vnum = 0;
					pMobIndex->skin_multiplier=race_table[pMobIndex->race].skin_multi;
					/*pMobIndex->skin_multiplier = 0;*/

                    for ( i = 0; i < MAX_VECT_BANK; i++ )
                        pMobIndex->affected_by[ i ] = 0;

                    pMobIndex->alignment = 0;
                    pMobIndex->hitroll = 0;

                    for ( i = 0; i < 4; i++ )
                        pMobIndex->ac[ i ] = 0;

                    for ( i = 0; i < MAX_STATS; i++ )
                        pMobIndex->stats[ i ] = 13;

                    ext_flags_clear( pMobIndex->off_flags );

                    for ( i = 0; i < MAX_RESIST; i++ )
		       {
			  pMobIndex->resists[ i ] = 0;
			  pMobIndex->healing_from[ i ] = 0;
		       }

                    pMobIndex->form = 0;
                    pMobIndex->parts = 0;
                    pMobIndex->size = SIZE_MEDIUM;
                    pMobIndex->sex = 0;
                    ext_flags_clear( pMobIndex->prog_flags );
                    pMobIndex->languages = 0;
                    pMobIndex->speaking = 0;

                    for ( i = 0; i < 16 ;i++ )
                        pMobIndex->spells[ i ] = 0;

                    pMobIndex->vnum = vnum;
                    vnum_found = TRUE;
                    match = TRUE;
                    break;
                }
                break;
            case 'W':
                if ( !str_cmp( word, "Wealth" ) )
                {
                    pMobIndex->wealth = fread_number( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Weaponbonus" ) )
                {
                    pMobIndex->weapon_damage_bonus = fread_number( fp );
                    match = TRUE;
                    break;
                }
                break;

            case 'X':
                if ( !str_cmp( word, "XAC" ) )
                {
                    pMobIndex->ac[ AC_PIERCE ] = fread_number( fp ) * 10;
                    pMobIndex->ac[ AC_BASH ] = fread_number( fp ) * 10;
                    pMobIndex->ac[ AC_SLASH ] = fread_number( fp ) * 10;
                    pMobIndex->ac[ AC_EXOTIC ] = fread_number( fp ) * 10;
                    match = TRUE;
                    break;
                }
                break;
            default : break;
        }

        if ( !match )
        {
            bugf( "Fread_mobile : no match '%s'.", word );
            fread_to_eol( fp );
        }

    }

    return FALSE;
}


bool fread_object( FILE *fp )
{
    OBJ_INDEX_DATA * Obj = NULL;
    int i, num = 0;
    bool match = FALSE;
    bool vnum_found = FALSE;
    char letter;
    ush_int vnum = 0;
    int iHash;
    char *word;
    BOARD_DATA *brd = NULL;

    if ( !area_last )
    {
        bug( "Fread_object: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    /* szukamy pierwszego '#'*/
    do
    {
        letter = fread_letter( fp );

        if ( feof( fp ) )
        {
            bug( "Fread_object: nie znaleziono '#' & EOF", 0 );
            return FALSE;
        }

    }
    while ( letter != '#' );

    while ( TRUE )
    {
        word = fread_word( fp );

        if ( num > 0 && !vnum_found )
            return FALSE;

        num++;
        match = FALSE;

        switch ( UPPER( word[ 0 ] ) )
        {
            case 'A':
                if ( !str_cmp( word, "Aff" ) )
                {
                    AFFECT_DATA * paf;
                    int bank, vector;

                    CREATE( paf, AFFECT_DATA, 1 );

                    paf->where = fread_number( fp );
                    paf->type = -1;
                    paf->duration = -1;
                    paf->rt_duration = 0;
                    paf->location = fread_number( fp );
                    paf->modifier = fread_number( fp );
                    paf->level = fread_number( fp );
                    bank = fread_number( fp );
                    vector = fread_flag( fp );
                    paf->bitvector = vector_to_pointer( affect_flags, bank, vector );

                    paf->next = Obj->affected;
                    Obj->affected = paf;
                    top_affect++;

                    match = TRUE;
                    break;
                }
                break;
            case 'B':
                if ( !str_cmp( word, "Bonus" ) )
                {
                    Obj->bonus_set = fread_number( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'C':
                if ( !str_cmp( word, "Cond" ) )
                {
                    Obj->condition = fread_number( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Cost" ) )
                {
                    Obj->cost = fread_number( fp );
                    Obj->rent_cost = ( Obj->cost * RENT_COST_RATE ) / 100;
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Comments" ) )
                {
                    free_string( Obj->comments );
                    Obj->comments = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'D':
                if ( !str_cmp( word, "Descr" ) )
                {
                    free_string( Obj->description );
                    Obj->description = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'E':
                if ( !str_cmp( word, "Extra" ) )
                {
                    long old_extra_flags;
                    old_extra_flags = fread_flag( fp );
                    Obj->extra_flags[0] = old_extra_flags;
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Extra2" ) )
                {
                	long old_extra_flags2;

                    old_extra_flags2 = fread_flag( fp );
                    Obj->extra_flags[1] = old_extra_flags2;
                    match = TRUE;
                    break;
                }
                if ( !str_cmp( word, "ExtraExt" ) )
                {
                	ext_flags_clear( Obj->extra_flags );
                	fread_ext_flags( fp, Obj->extra_flags );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Extradesc" ) )
                {
                    EXTRA_DESCR_DATA * ed;

                    CREATE( ed, EXTRA_DESCR_DATA, 1 );
                    ed->keyword = fread_string( fp );
                    ed->description = fread_string( fp );
                    ed->next = Obj->extra_descr;
                    Obj->extra_descr = ed;
                    top_ed++;
                    match = TRUE;
                    break;
                }
                break;
            case 'I':
                if ( !str_cmp( word, "Itemdesc" ) )
                {
                    Obj->item_description = fread_string( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Identdesc" ) )
                {
                    Obj->ident_description = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
			case 'G':
                if ( !str_cmp( word, "Gender" ) )
                {
                    Obj->gender = fread_number( fp );
                    match = TRUE;
                    break;
                }
                break;
			case 'H':
                if ( !str_cmp( word, "Hiddendesc" ) )
                {
                    Obj->hidden_description = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
      case 'K':
      					//rellik: komponenty, ³adowanie komponentowo¶ci do definicji obiektu
          			if ( !str_cmp( word, "Komponent" ) )
          			{
          				Obj->is_spell_item = TRUE;
          				Obj->spell_item_counter = 1;
          				Obj->spell_item_timer = 525600;
          				Obj->spell_item_counter = fread_number( fp );
          				Obj->spell_item_timer = fread_number( fp );
          				match = TRUE;
          				break;
          			}
          			break;
			case 'L':
                if ( !str_cmp( word, "LiczbaMnoga" ) )
                {
                    Obj->liczba_mnoga = fread_number( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'M':
                if ( !str_cmp( word, "Material" ) )
                {
					char * material;
					material      = fread_string( fp );
                    Obj->material = material_lookup( material );
					free_string( material );
                    match = TRUE;
                    break;
                }
                break;
            case 'N':
                if ( !str_cmp( word, "Name" ) )
                {
                    free_string( Obj->name );
                    Obj->name = fread_string( fp );
                    match = TRUE;
                    break;
                }

                break;
            case 'O':
                if ( !str_cmp( word, "Objend" ) )
                {
                    fread_to_eol( fp );

                    /* inicjalizowanie tablicy */
                    if ( Obj->item_type == ITEM_BOARD )
                    {
                        CREATE( brd, BOARD_DATA, 1 );

                        brd->vnum = Obj->vnum;
                        brd->message = NULL;
                        brd->next = board_system;
                        board_system = brd;
                    }

                    newobjs++;
                    vnum = Obj->vnum;
                    iHash = vnum % MAX_KEY_HASH;
                    Obj->next = obj_index_hash[ iHash ];
                    obj_index_hash[ iHash ] = Obj;
                    top_obj_index++;
                    top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;
                    assign_area_vnum( vnum );
                    return TRUE;
                }
                else if ( !str_cmp( word, "Objprog" ) )
                {
                    PROG_LIST * pOprog;
                    char *word;
                    BITVECT_DATA * trigger;

                    CREATE( pOprog, PROG_LIST, 1 );
                    word = fread_word( fp );

					trigger = ext_flag_lookup( word, prog_flags );
                    if ( trigger == &EXT_NONE )
                    {
                        bug( "Fread_object: invalid trigger.", 0 );
                        exit( 1 );
                    }

                    EXT_SET_BIT( Obj->prog_flags, *trigger );
                    pOprog->trig_type = trigger;
                    pOprog->name = str_dup( fread_word( fp ) );
                    pOprog->trig_phrase = fread_string( fp );
                    pOprog->next = Obj->progs;
                    Obj->progs = pOprog;
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Odmiana" ) )
                {
                    free_string( Obj->name2 );
                    free_string( Obj->name3 );
                    free_string( Obj->name4 );
                    free_string( Obj->name5 );
                    free_string( Obj->name6 );
                    Obj->name2 = fread_string( fp );
                    Obj->name3 = fread_string( fp );
                    Obj->name4 = fread_string( fp );
                    Obj->name5 = fread_string( fp );
                    Obj->name6 = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'R':
                if ( !str_cmp( word, "Rent" ) )
                {
                    fread_number( fp );
                    match = TRUE;
                    break;
                }
                if ( !str_cmp( word, "Repair" ) )
                {
                    Obj->repair_limit     = fread_number( fp );
                    Obj->repair_counter   = fread_number( fp );
                    Obj->repair_penalty   = fread_number( fp );
                    Obj->repair_condition = fread_number( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'S':
                if ( !str_cmp( word, "Short" ) )
                {
                    free_string( Obj->short_descr );
                    Obj->short_descr = fread_string( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "SpecDam" ) )
                {
                    SPEC_DAMAGE * specdam;
                    specdam = new_spec_damage();

                    specdam->type = fread_number( fp );
                    specdam->chance = fread_number( fp );
                   	specdam->param1 = fread_number( fp );
                    if ( specdam->type == SD_TYPE_ACT )
                   	{
	                	specdam->ext_param[0] = specdam->param1;
       		        	specdam->param1 = SD_PARAM_ANY;
                   	}
                    specdam->param2 = fread_number( fp );
                    specdam->param3 = fread_number( fp );
                    specdam->bonus = fread_number( fp );

                    specdam->next = Obj->spec_dam;
                    Obj->spec_dam = specdam;
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "SpecDamExt" ) )
                {
                    SPEC_DAMAGE * specdam;
                    specdam = new_spec_damage();

                    specdam->type = fread_number( fp );
                    specdam->chance = fread_number( fp );
                    specdam->param1 = fread_number( fp );
                    specdam->param2 = fread_number( fp );
                    specdam->param3 = fread_number( fp );

                	ext_flags_clear( specdam->ext_param );
                	fread_ext_flags( fp, specdam->ext_param );

                    specdam->bonus = fread_number( fp );
                    // Tener: byc mo¿e obiekt nie zostal zapisany z tym ficzerem
		    if ( isdigit( next_nonspace_before_newline( fp ) ) )
			specdam->target_type = fread_number( fp );

                    specdam->next = Obj->spec_dam;
                    Obj->spec_dam = specdam;
                    match = TRUE;
                    break;
                }
                break;
            case 'T':
                if ( !str_cmp( word, "Trap" ) )
                {
                    Obj->trap = fread_number( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Type" ) )
                {
                    CHECK_POS( Obj->item_type, item_lookup( fread_word( fp ) ), "item_type" );
                    match = TRUE;
                    break;
                }
                break;
            case 'V':
                if ( !str_cmp( word, "Value" ) )
                {
                    for ( i = 0 ; i < 7 ; i++ )
                    {
                        Obj->value[ i ] = fread_number( fp );
                    }
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Vnum" ) )
                {
                    ush_int vnum;

                    vnum = fread_number( fp );

                    if ( vnum == 0 )
                        return FALSE;

                    CREATE( Obj, OBJ_INDEX_DATA, 1 );
                    Obj->vnum = vnum;

                    Obj->area = area_last;
                    Obj->new_format = TRUE;
                    Obj->reset_num = 0;
                    Obj->item_description = NULL;
                    Obj->ident_description = NULL;
					Obj->hidden_description = NULL;
                    Obj->comments = str_dup( "" );
                    Obj->progs = NULL;
                    Obj->name = str_dup( "" );
                    Obj->name2 = str_dup( "" );
                    Obj->name3 = str_dup( "" );
                    Obj->name4 = str_dup( "" );
                    Obj->name5 = str_dup( "" );
                    Obj->name6 = str_dup( "" );
                    Obj->short_descr = str_dup( "" );
                    Obj->description = str_dup( "" );
                    Obj->material = 0;
                    Obj->item_type = 0;
                    ext_flags_clear( Obj->extra_flags );
                    Obj->wear_flags = 0;
                    ext_flags_clear( Obj->wear_flags2 );
                    Obj->condition = 0;
                    Obj->weight = 0;
                    Obj->cost = 0;
                    Obj->liczba_mnoga = FALSE;
                    Obj->gender = 0;

                    for ( i = 0;i < 7;i++ )
                        Obj->value[ i ] = 0;

                    ext_flags_clear( Obj->prog_flags );
                    Obj->bonus_set = 0;
                    Obj->rent_cost = 0;

                    vnum_found = TRUE;
                    match = TRUE;
                    break;
                }
                break;
            case 'W':
                if ( !str_cmp( word, "Wear" ) )
                {
                    Obj->wear_flags = fread_flag( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Wear2" ) )
                {
                	long old_wear_flags2;

                    old_wear_flags2 = fread_flag( fp );
                    Obj->wear_flags2[0] = old_wear_flags2;
                    match = TRUE;
                    break;
                }
                if ( !str_cmp( word, "Wear2Ext" ) )
                {
                	ext_flags_clear( Obj->wear_flags2 );
                	fread_ext_flags( fp, Obj->wear_flags2 );
                    match = TRUE;
                    break;
                }
                if ( !str_cmp( word, "Weight" ) )
                {
                    Obj->weight = fread_number( fp );
                    match = TRUE;
                    break;
                }
                break;
            default : break;
        }

        if ( !match )
        {
            bugf( "Fread_object : no match '%s'.", word );
            fread_to_eol( fp );
            break;
        }

    }

    return FALSE;
}


bool fread_room( FILE *fp )
{
	ROOM_INDEX_DATA * Room = NULL;
	EXIT_DATA * last_exit = NULL;
	int last_door = 0;
	int num = 0;
	bool match = FALSE;
	bool vnum_found = FALSE;
	char letter;
	ush_int vnum = 0;
	int iHash;
	char *word;
	int i, x;

	if ( !area_last )
	{
		bug( "Fread_room: no #AREA seen yet.", 0 );
		exit( 1 );
	}

	/* szukamy pierwszego '#'*/
	do
	{
		letter = fread_letter( fp );

		if ( feof( fp ) )
		{
			bug( "Fread_mobile: nie znaleziono '#' & EOF", 0 );
			return FALSE;
		}

	}
	while ( letter != '#' );


	while ( TRUE )
	{
		word = fread_word( fp );

		if ( num > 0 && !vnum_found )
			return FALSE;

		num++;
		match = FALSE;

		switch ( UPPER( word[ 0 ] ) )
		{
		case 'A':
			if ( !str_cmp( word, "AreaPart" ) )
			{
				Room->area_part = fread_number( fp );
				match = TRUE;
				break;
			}
			break;
		case 'C':
			if ( !str_cmp( word, "Capacity") )
			{
				Room->rawmaterial_capacity = fread_number( fp );
				match = TRUE;
				break;
			}
			break;
		case 'D':
			if ( !str_cmp( word, "Descr" ) )
			{
				free_string( Room->description );
				Room->description = fread_string( fp );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "DayRDesc" ) )
			{
				Room->day_rand_desc = fread_number( fp );
				match = TRUE;
				break;
			}
			break;
		case 'E':
			if ( !str_cmp( word, "Exit" ) )
			{
				EXIT_DATA * pexit;
				char field[ MAX_STRING_LENGTH ];
				int door;
				char* tempstr = NULL;

				door = fread_number( fp );

				if ( door < 0 || door > 5 )
				{
					bug( "Fread_rooms: vnum %d has bad door number.", Room->vnum );
					exit( 1 );
				}

				pexit = new_exit();
				pexit->description = fread_string( fp );

				for ( ; ; )
				{
					tempstr = fread_string( fp );
					if ( !str_prefix( "NightExit", tempstr ) )
					{
						pexit->nightdescription = str_dup( one_argument( tempstr, field ) );
					}
					else if ( !str_prefix( "VName", tempstr ) )
					{
						pexit->vName = str_dup( one_argument( tempstr, field ) );
					}
					else if ( !str_prefix( "TextIn", tempstr ) )
					{
						pexit->TextIn = str_dup( one_argument( tempstr, field ) );
					}
					else if ( !str_prefix( "TextOut", tempstr ) )
					{
						pexit->TextOut = str_dup( one_argument( tempstr, field ) );
					}
					else
					{
						pexit->keyword = tempstr;
						break;
					}
					free_string( tempstr );
				}

				pexit->exit_info = fread_number( fp );
				pexit->rs_flags = pexit->exit_info;
				pexit->key = fread_number( fp );
				pexit->u1.vnum = fread_number( fp );
				pexit->orig_door = door;

				if ( last_exit )
					free_exit( pexit );
				else
				{
					Room->exit[ door ] = pexit;
					top_exit++;
				}
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "ExitExt" ) )
			{
				EXIT_DATA * pexit;
				int door;

				door = fread_number( fp );

				if ( door < 0 || door > 5 )
				{
					bug( "Fread_rooms: vnum %d has bad door number.", Room->vnum );
					exit( 1 );
				}

				pexit = new_exit();

				last_exit = pexit;
				last_door = door;
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "ExitExtDescription" ) )
			{
				if ( !last_exit )
				{
					bug( "Fread_rooms: ExitExtDescription without ExitExt.", Room->vnum );
					exit( 1 );
				}
				last_exit->description = fread_string( fp );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "ExitExtNightDescription" ) )
			{
				if ( !last_exit )
				{
					bug( "Fread_rooms: ExitExtNightDescription without ExitExt.", Room->vnum );
					exit( 1 );
				}
				last_exit->nightdescription = fread_string( fp );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "ExitExtVName" ) )
			{
				if ( !last_exit )
				{
					bug( "Fread_rooms: ExitExtVName without ExitExt.", Room->vnum );
					exit( 1 );
				}
				last_exit->vName = fread_string( fp );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "ExitExtTextIn" ) )
			{
				if ( !last_exit )
				{
					bug( "Fread_rooms: ExitExtTextIn without ExitExt.", Room->vnum );
					exit( 1 );
				}
				last_exit->TextIn = fread_string( fp );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "ExitExtTextOut" ) )
			{
				if ( !last_exit )
				{
					bug( "Fread_rooms: ExitExtTextOut without ExitExt.", Room->vnum );
					exit( 1 );
				}
				last_exit->TextOut = fread_string( fp );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "ExitExtLiczba" ) )
			{
				if ( !last_exit )
				{
					bug( "Fread_rooms: ExitExtLiczba without ExitExt.", Room->vnum );
					exit( 1 );
				}
				last_exit->liczba_mnoga = fread_number( fp );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "ExitExtBiernik" ) )
			{
				if ( !last_exit )
				{
					bug( "Fread_rooms: ExitExtBiernik without ExitExt.", Room->vnum );
					exit( 1 );
				}
				last_exit->biernik = fread_string( fp );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "ExitExtName" ) )
			{
				if ( !last_exit )
				{
					bug( "Fread_rooms: ExitExtName without ExitExt.", Room->vnum );
					exit( 1 );
				}
				last_exit->keyword = fread_string( fp );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "ExitExtInfo" ) )
			{
				if ( !last_exit )
				{
					bug( "Fread_rooms: ExitExtInfo without ExitExt.", Room->vnum );
					exit( 1 );
				}
				last_exit->exit_info = fread_number( fp );
				last_exit->rs_flags = last_exit->exit_info;
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "ExitExtKey" ) )
			{
				if ( !last_exit )
				{
					bug( "Fread_rooms: ExitExtKey without ExitExt.", Room->vnum );
					exit( 1 );
				}
				last_exit->key = fread_number( fp );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "ExitExtToVnum" ) )
			{
				if ( !last_exit )
				{
					bug( "Fread_rooms: ExitExtToVnum without ExitExt.", Room->vnum );
					exit( 1 );
				}
				last_exit->u1.vnum = fread_number( fp );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "ExitExtEnd" ) )
			{
				if ( !last_exit )
				{
					bug( "Fread_rooms: ExitExtEnd without ExitExt.", Room->vnum );
					exit( 1 );
				}
				last_exit->orig_door = last_door;
				Room->exit[ last_door ] = last_exit;
				last_door = 0;
				top_exit++;
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "Exittrap" ) )
			{
				int door;
				int trap;

				door = fread_number( fp );
				trap = fread_number( fp );

				if ( door < 0 || door > 5 )
				{
					bug( "Fread_rooms: vnum %d has bad door number.", Room->vnum );
					exit( 1 );
				}


				Room->exit[ door ] ->trap = trap;
				SET_BIT( Room->exit[ door ] ->rs_flags, EX_TRAP );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "Extra" ) )
			{
				EXTRA_DESCR_DATA * ed;

				CREATE( ed, EXTRA_DESCR_DATA, 1 );

				ed->keyword = fread_string( fp );
				ed->description = fread_string( fp );

				ed->next = Room->extra_descr;
				Room->extra_descr = ed;
				top_ed++;
				match = TRUE;
				break;
			} else if ( !str_cmp( word, "EchoTo" ) )
			{
				Room->echo_to.vnum = fread_number( fp );
				match = TRUE;
				break;
			}

			break;
		case 'F':
			if ( !str_cmp( word, "FlagsExt" ) )
			{
				fread_ext_flags( fp, Room->room_flags );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "Flags" ) )
			{
				Room->room_flags[0] = fread_flag( fp );
				match = TRUE;
				break;
			}
			break;
		case 'H':
			if ( !str_cmp( word, "Heal" ) )
			{
				Room->heal_rate = fread_number( fp );
				match = TRUE;
				break;
			}

		case 'N':
			if ( !str_cmp( word, "Name" ) )
			{
				free_string( Room->name );
				Room->name = fread_string( fp );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "Nightdescr" ) )
			{
				free_string( Room->nightdesc );
				Room->nightdesc = fread_string( fp );
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "NightRDesc" ) )
			{
				Room->night_rand_desc = fread_number( fp );
				match = TRUE;
				break;
			}


			break;
		case 'O':
			if ( !str_cmp( word, "Owner" ) )
			{
				free_string( Room->owner );
				Room->owner = fread_string( fp );
				match = TRUE;
				break;
			}
			break;
		case 'R':
			if ( !str_cmp( word, "Roomend" ) )
			{
				fread_to_eol( fp );

				vnum = Room->vnum;
				iHash = vnum % MAX_KEY_HASH;
				Room->next = room_index_hash[ iHash ];
				room_index_hash[ iHash ] = Room;
				top_room++;
				top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room;
				assign_area_vnum( vnum );
				return TRUE;
			}
			else if ( !str_cmp( word, "Roomprog" ) )
			{
				PROG_LIST * pOprog;
				char *word;
				BITVECT_DATA * trigger;

				CREATE( pOprog, PROG_LIST, 1 );
				word = fread_word( fp );

				trigger = ext_flag_lookup( word, prog_flags );
				if ( trigger == &EXT_NONE )
				{
					bug( "ROOMprogs: invalid trigger.", 0 );
					exit( 1 );
				}

				EXT_SET_BIT( Room->prog_flags, *trigger );

				enable_room_update( Room, trigger );

				pOprog->trig_type = trigger;
				pOprog->name = str_dup( fread_word( fp ) );;
				pOprog->trig_phrase = fread_string( fp );
				pOprog->next = Room->progs;
				Room->progs = pOprog;
				match = TRUE;
				break;
			}
			else if ( !str_cmp( word, "Rentrate" ) )
			{
				Room->rent_rate = fread_number( fp );
				match = TRUE;
				break;
			}
			//rellik: mining
			else if ( !str_cmp( word, "Resources" ) )
			{
				for( i = 0; rawmaterial_table[i].name; ++i )
				{
					x = fread_number( fp );
					if ( x == 30000 )
					{
						//ci±gnie siê ten u³omny b³±d ale ok 30000 = podejrzenie b³êdu w odczycie
						break;
					}
					Room->rawmaterial[i] = x;
				}
				match = TRUE;
				break;
			}
			break;
		case 'S':
			if ( !str_cmp( word, "Sector" ) )
			{
				Room->sector_type = fread_number( fp );
				//ziola      (herbs)
				Room->first_herb = start_herbs(Room->first_herb, Room->sector_type);
				Room->herb_update_timer = sector_table[Room->sector_type].herb_reset_time;
				match = TRUE;
				break;
			}
			break;
		case 'T':
			if ( !str_cmp( word, "Trap" ) )
			{
				Room->trap = fread_number( fp );
				EXT_SET_BIT( Room->room_flags, ROOM_TRAP );
				match = TRUE;
				break;
			}
			break;

		case 'V':
			if ( !str_cmp( word, "Vnum" ) )
			{
				ush_int vnum;
				int door;

				vnum = fread_number( fp );

				if ( vnum == 0 )
					return FALSE;

				CREATE( Room, ROOM_INDEX_DATA, 1 );

				//rellik: mining
				Room->rawmaterial_capacity = 0;
				for ( i = 0; rawmaterial_table[i].name; ++i ) Room->rawmaterial[i] = 0;

				for ( door = 0; door <= 5; door++ )
					Room->exit[ door ] = NULL;

				Room->owner = str_dup( "" );
				Room->name = str_dup( "" );
				Room->description = str_dup( "" );
				Room->nightdesc = str_dup( "" );
				Room->day_rand_desc = 0;
				Room->night_rand_desc = 0;

				Room->heal_rate = 100;
				Room->rent_rate = 100;
				Room->herbs_count = 0;
				Room->people = NULL;
				Room->contents = NULL;
				Room->extra_descr = NULL;
				ext_flags_clear( Room->prog_flags );
				Room->prog_delay = 0;
				Room->prog_target = NULL;
				Room->area = area_last;
				Room->light = 0;
				Room->pflag_list = NULL;
				Room->progs = NULL;
				Room->extra_descr = NULL;
				Room->reset_first = NULL;
				Room->reset_last = NULL;
				ext_flags_clear( Room->room_flags );
				Room->sector_type = 0;
				Room->has_memdat = 0;

				Room->vnum = vnum;

				vnum_found = TRUE;
				match = TRUE;
				break;
			}
			break;
		default : break;
		}

		if ( !match )
		{
			bugf( "Fread_room : no match '%s'. ", word );
			fread_to_eol( fp );
			break;
		}

	}

	return FALSE;
}


bool fread_trap( FILE * fp )
{
    TRAP_DATA tmp, *trap;
    char letter;
    char *word;
    int num = 0;
    bool vnum_found = FALSE;
    bool match = FALSE;

    do
    {
        letter = fread_letter( fp );

        if ( feof( fp ) )
        {
            bug( "Load_traps: nie znaleziono '#' & EOF", 0 );
            return FALSE;
        }

    }
    while ( letter != '#' );

    while ( TRUE )
    {
        word = fread_word( fp );

        if ( ( num > 1 && !vnum_found ) || num > 20 )
            return FALSE;

        num++;
        match = FALSE;

        switch ( UPPER( word[ 0 ] ) )
        {

            case 'A':
                if ( !str_cmp( word, "Active" ) )
                {
                    tmp.active = fread_number( fp );
                    match = TRUE;
                    break;
                }
                break;

                break;
            case 'D':
                if ( !str_cmp( word, "Disarmed" ) )
                {
                    tmp.disarmed = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'F':
                if ( !str_cmp( word, "Failed" ) )
                {
                    tmp.failed_disarm = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'L':
                if ( !str_cmp( word, "Level" ) )
                {
                    tmp.level = fread_number( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Limit" ) )
                {
                    tmp.limit = fread_number( fp );
                    match = TRUE;
                    break;
                }
                break;

            case 'T':
                if ( !str_cmp( word, "Trapend" ) )
                {
                    fread_to_eol( fp );

                    trap = new_trapdata();

                    trap->level = tmp.level;
                    trap->vnum = tmp.vnum;
                    trap->limit = tmp.limit;
                    trap->active = tmp.active;
                    trap->type = tmp.type;
                    trap->disarmed = tmp.disarmed;
                    trap->failed_disarm = tmp.failed_disarm;
                    trap->triggered = tmp.triggered;
                    trap->next = trap_list;
                    trap_list = trap;
                    top_trap_index++;
                    match = TRUE;
                    return TRUE;
                }
                else if ( !str_cmp( word, "Triggered" ) )
                {
                    tmp.triggered = fread_string( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "Type" ) )
                {
                    tmp.type = fread_number( fp );
                    match = TRUE;
                    break;
                }

                break;
            case 'V':
                if ( !str_cmp( word, "Vnum" ) )
                {
                    tmp.vnum = fread_number( fp );

                    if ( tmp.vnum == 0 )
                        return FALSE;

                    tmp.level = 0;
                    tmp.limit = 0;
                    tmp.type = 0;
                    tmp.disarmed = NULL;
                    tmp.failed_disarm = NULL;
                    tmp.triggered = NULL;

                    match = TRUE;
                    vnum_found = TRUE;
                    break;
                }

            default: break;
        }

        if ( !match )
        {
            bugf( "Fread_trap: no match '%s'.", word );
            fread_to_eol( fp );
            break;
        }
    }
    return FALSE;
}

bool fread_rand_desc( FILE * fp )
{
    RAND_DESC_DATA tmp, *rdesc;
    char letter;
    char *word;
    int num = 0;
    bool vnum_found = FALSE;
    bool match = FALSE;

    if ( !area_last )
    {
        bug( "Fread_rand_desc: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    do
    {
        letter = fread_letter( fp );

        if ( feof( fp ) )
        {
            bug( "Fread_rand_desc: nie znaleziono '#' & EOF", 0 );
            return FALSE;
        }

    }
    while ( letter != '#' );

    while ( TRUE )
    {
        word = fread_word( fp );

        if ( ( num > 1 && !vnum_found ) || num > 20 )
            return FALSE;

        num++;
        match = FALSE;

        switch ( UPPER( word[ 0 ] ) )
        {
            case 'D':
                if ( !str_cmp( word, "DescRand" ) )
                {
                    tmp.rand_desc = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'R':
                if ( !str_cmp( word, "RandDescEnd" ) )
                {
                    fread_to_eol( fp );

                    rdesc = new_rdescdata();

                    rdesc->vnum = tmp.vnum;
                    rdesc->rand_desc = tmp.rand_desc;
                    rdesc->next = rand_desc_list;
                    rdesc->area = tmp.area;
                    rand_desc_list = rdesc;
                    top_rdesc_index++;
                    match = TRUE;
                    return TRUE;
                }
                break;
            case 'V':
                if ( !str_cmp( word, "Vnum" ) )
                {
                    tmp.vnum = fread_number( fp );

                    if ( tmp.vnum == 0 )
                        return FALSE;

                    tmp.rand_desc = NULL;
                    tmp.area = area_last;

                    match = TRUE;
                    vnum_found = TRUE;
                    break;
                }

            default: break;
        }

        if ( !match )
        {
            bugf( "Fread_rand_desc: no match '%s'.", word );
            fread_to_eol( fp );
            break;
        }
    }
    return FALSE;
}

bool fread_desc( FILE * fp )
{
    DESC_DATA tmp, *desc;
    char letter;
    char *word;
    int num = 0;
    bool vnum_found = FALSE;
    bool match = FALSE;

    if ( !area_last )
    {
        bug( "Fread_desc: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    do
    {
        letter = fread_letter( fp );

        if ( feof( fp ) )
        {
            bug( "Fread_desc: nie znaleziono '#' & EOF", 0 );
            return FALSE;
        }

    }
    while ( letter != '#' );

    while ( TRUE )
    {
        word = fread_word( fp );

        if ( ( num > 1 && !vnum_found ) || num > 20 )
            return FALSE;

        num++;
        match = FALSE;

        switch ( UPPER( word[ 0 ] ) )
        {
            case 'D':
                if ( !str_cmp( word, "Desc" ) )
                {
                    tmp.description = fread_string( fp );
                    match = TRUE;
                    break;
                }
                else if ( !str_cmp( word, "DescEnd" ) )
                    return FALSE;
                break;

            case 'E':
                if ( !str_cmp( word, "End" ) )
                {
                    fread_to_eol( fp );

                    desc = new_descdata();

                    desc->description = tmp.description;
                    desc->name = tmp.name;
                    desc->group = tmp.group;
                    desc->next = area_last->desc_data;
                    area_last->desc_data = desc;
                    match = TRUE;
                    return TRUE;
                }
                break;
            case 'N':
                if ( !str_cmp( word, "Name" ) )
                {
                    tmp.name = fread_string( fp );

                    tmp.description = NULL;
                    tmp.group = 0;

                    match = TRUE;
                    vnum_found = TRUE;
                    break;
                }
                break;
            case 'G':
                if ( !str_cmp( word, "Group" ) )
                {
                    tmp.group = fread_number( fp );
                    match = TRUE;
                    break;
                }
                break;

            default: break;
        }

        if ( !match )
        {
            bugf( "Fread_desc: no match '%s'.", word );
            fread_to_eol( fp );
            break;
        }
    }
    return FALSE;
}

bool fread_bonus_set( FILE * fp )
{
    BONUS_INDEX_DATA *pBonus;
    char letter;
    char *word;
    int num = 0;
    bool vnum_found = FALSE;
    bool match = FALSE;

    if ( !area_last )
    {
        bug( "Fread_bonus_set: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    do
    {
        letter = fread_letter( fp );

        if ( feof( fp ) )
        {
            bug( "Fread_bonus_set: nie znaleziono '#' & EOF", 0 );
            return FALSE;
        }

    }
    while ( letter != '#' );

    while ( TRUE )
    {
        word = fread_word( fp );

        if ( ( num > 1 && !vnum_found ) || num > 50 )
            return FALSE;

        num++;
        match = FALSE;

        switch ( UPPER( word[ 0 ] ) )
        {
            case 'A':
                if ( !str_cmp( word, "Aff" ) )
                {
                    AFFECT_DATA * paf;
                    int bank, vector;

                    CREATE( paf, AFFECT_DATA, 1 );

                    paf->where            = fread_number( fp );
                    paf->type             = -1;
                    paf->duration         = -1;
                    paf->rt_duration			= 0;
                    paf->location         = fread_number( fp );
                    paf->modifier         = fread_number( fp );
                    paf->level            = fread_number( fp );
                    bank                  = fread_number( fp );
                    vector                = fread_flag( fp );
                    paf->bitvector        = vector_to_pointer( affect_flags, bank, vector );

                    paf->next       = pBonus->affects;
                    pBonus->affects = paf;
                    top_affect++;

                    match = TRUE;
                    break;
                }
                break;
            case 'B':
                if ( !str_cmp( word, "BonusEnd" ) )
                {
                    fread_to_eol( fp );

                    pBonus->next = bonus_list;
                    bonus_list   = pBonus;
                    match = TRUE;
                    return TRUE;
                }
                break;
            case 'C':
                if ( !str_cmp( word, "Comments" ) )
                {
                    free_string( pBonus->comments );
                    pBonus->comments = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'D':
                if ( !str_cmp( word, "Desc" ) )
                {
                    pBonus->description = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'E':
                if ( !str_cmp( word, "Extradesc" ) )
                {
                    EXTRA_DESCR_DATA * ed;

                    CREATE( ed, EXTRA_DESCR_DATA, 1 );
                    ed->keyword = fread_string( fp );
                    ed->description = fread_string( fp );
                    ed->next = pBonus->extra_descr;
                    pBonus->extra_descr = ed;
                    top_ed++;
                    match = TRUE;
                    break;
                }
                break;
            case 'N':
                if ( !str_cmp( word, "Name" ) )
                {
                    free_string( pBonus->name );
                    pBonus->name = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'R':
                if ( !str_cmp( word, "RemoveProg" ) )
                {
                    free_string( pBonus->remove_prog );
                    pBonus->remove_prog = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'W':
                if ( !str_cmp( word, "WearProg" ) )
                {
                    free_string( pBonus->wear_prog );
                    pBonus->wear_prog = fread_string( fp );
                    match = TRUE;
                    break;
                }
                break;
            case 'V':
                if ( !str_cmp( word, "Vnum" ) )
                {
                    ush_int vnum;

                    vnum = fread_number( fp );

                    if ( vnum == 0 )
                        return FALSE;

                    pBonus = new_bonus_index();

                    pBonus->vnum = vnum;
                    pBonus->area = area_last;

                    match = TRUE;
                    vnum_found = TRUE;
                    break;
                }
                break;

            default: break;
        }

        if ( !match )
        {
            bugf( "Fread_bonus_set: no match '%s'.", word );
            fread_to_eol( fp );
            continue;
        }
    }
    return FALSE;
}
