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
 * $Id: scan.c 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/scan.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"

char *const distance[4]=
{
    "tutaj.",
    "blisko na %s.",
    "niedaleko na %s.",
    "sporo na %s."
};

void scan_list   args( ( ROOM_INDEX_DATA *scan_room, CHAR_DATA *ch, sh_int depth, sh_int door ) );
void scan_char   args( ( CHAR_DATA *victim, CHAR_DATA *ch, sh_int depth, sh_int door ) );
bool check_blind args( ( CHAR_DATA *ch ) );

int scan_room( CHAR_DATA *ch, const ROOM_INDEX_DATA *room, char *buf, int distance )
{
    CHAR_DATA *target = room->people;
    int number_found = 0;
    AFFECT_DATA *dazzling;

    if ( EXT_IS_SET( room->room_flags, ROOM_MAGICDARK ) )
    {
        return -1;
    }
    while ( target )
    {
        if ( !IS_NPC( target ) && target->invis_level > get_trust( ch ) )
        {
            target = target->next_in_room;
            continue;
        }

        if ( target == ch && ( IS_NPC( ch ) || ( !IS_NPC( ch ) && !ch->pcdata->mind_in ) ) )
        {
            target = target->next_in_room;
            continue;
        }

        if ( can_see( ch, target ) )
        {
            strcat ( buf, "  " );
            if ( ( IS_NPC(target) || target->ss_data ) && !is_affected(target, gsn_fetch) )
            {
                strcat ( buf, "{y");
                strcat ( buf, capitalize(target->short_descr) );
                strcat ( buf, "{x{c");
            }
            else
            {
                strcat ( buf, "{Y");
                strcat ( buf, capitalize(target->name) );
                strcat ( buf, "{x{c");
            }

            if ( IS_AFFECTED( target, AFF_INVISIBLE ) ) strcat( buf, " (niewidzialny)" );
            else if( is_affected( ch, gsn_dazzling_flash) && affect_find( ch->affected, gsn_dazzling_flash)->level == 0 &&
                    is_affected( target, gsn_dazzling_flash) &&
                    !IS_AFFECTED( ch, AFF_PERFECT_SENSES) && ( !IS_AFFECTED(ch,AFF_DETECT_INVIS) || IS_AFFECTED(target,AFF_NONDETECTION)) )
            {
                for( dazzling = target->affected ; dazzling; dazzling = dazzling->next )
                {
                    if( dazzling->level == 1 && affect_find( ch->affected, gsn_dazzling_flash)->modifier == dazzling->modifier )
                        strcat( buf, " (niewidzialny)" );
                }
            }

            if ( IS_AFFECTED( target, AFF_HIDE ) )
            {
                strcat( buf, " (ukryty)" );
            }
            if (( EXT_IS_SET( target->act, ACT_AGGRESSIVE )) && IS_AFFECTED( ch, AFF_DETECT_AGGRESSIVE ) )
            {
                if( ch->level + 10 > target->level )
                {
                    strcat( buf, " {R(agresywny){x" );
                }
                else if ( dice(1,3) == 1 )
                {
                    strcat( buf, " {R(agresywny){x" );
                }
            }
            else if ( IS_AFFECTED( ch, AFF_DETECT_AGGRESSIVE ) && ( ch->level + 10 < target->level ) )
            {
                if ( dice(1,3) == 1 )
                {
                    strcat( buf, " {R(agresywny){x" );
                }
            }

            if (( is_undead(target) ) && IS_AFFECTED( ch, AFF_DETECT_UNDEAD ) )
            {
                if( ch->level + 10 > target->level )
                {
                    strcat( buf, " {D(nieumar³y){x" );
                }
                else if ( dice(1,3) == 1 )
                {
                    strcat( buf, " {D(nieumar³y){x" );
                }
            }
            else if ( IS_AFFECTED( ch, AFF_DETECT_UNDEAD ) && ( ch->level + 10 < target->level ) )
            {
                if ( dice(1,3) == 1 )
                {
                    strcat( buf, " {D(nieumar³y){x" );
                }
            }
            switch ( distance )
            {
                case 0:
                    strcat ( buf, " jest tutaj." );
                    break;
                case 1:
                    strcat ( buf, " zaraz obok." );
                    break;
                case 2:
                    strcat ( buf, " niedaleko." );
                    break;
                case 3:
                    strcat ( buf, " daleko." );
                    break;
                case 4:
                    strcat ( buf, " ca³kiem daleko." );
                    break;
                case 5:
                    strcat ( buf, " bardzo daleko." );
                    break;
                case 6:
                    strcat ( buf, " niesamowicie daleko." );
                    break;
                default:
                    strcat ( buf, " dziwnie daleko." );
                    break;
            }
            strcat ( buf, "\n\r" );
            number_found++;
        }
        target = target->next_in_room;
    }
    return number_found;
}

void do_scan( CHAR_DATA *ch, char *argument )
{
    EXIT_DATA       *pexit;
    ROOM_INDEX_DATA *room;
    ROOM_INDEX_DATA *in_room;
    DESCRIPTOR_DATA * d;
    extern char *const dir_name[];
    char buf[ MAX_STRING_LENGTH ];
    char exitname[ MAX_STRING_LENGTH ];
    int lookdoor, door, iter;
    int distance, max_dist;
    int mob_count, mob_count_new;
    bool darkness = FALSE;
    const char *dir_long_name[] = { "pó³noc", "wschód", "po³udnie", "zachód", "góra", "dó³" };
    lookdoor = -1;
    door = -1;

    if ( !IS_NPC( ch ) && ch->pcdata->mind_in )
    {
        in_room = ch->pcdata->mind_in;
    }
    else
    {
        if ( !check_blind( ch ) )
        {
            return ;
        }
        in_room = ch->in_room;
    }

    if ( argument[0] != '\0' )
    {
        if ( IS_AFFECTED( ch, AFF_MAZE ) )
        {
            send_to_char("Nie bardzo wiesz w która stronê popatrzeæ.\n\r", ch);
            return;
        }

        //wieza obserwacyjna
        if( !str_prefix( argument, "around" ) )
        {
            if ( !EXT_IS_SET( ch->in_room->room_flags, ROOM_WATCH_TOWER ) )
            {
                send_to_char( "Musisz byæ na szczycie wie¿y obserwacyjnej.\n\r", ch );
                return;
            }

            if ( ch->position != POS_STANDING )
            {
                send_to_char( "Najpierw wstañ.\n\r", ch );
                return;
            }

            send_to_char( "Ze szczytu wie¿y ogl±dasz uwa¿nie ca³± okolicê.\n\r", ch );
            act( "Stoj±c na szczycie wie¿y $n uwa¿nie omiata wzrokiem ca³± okolicê.", ch, NULL, NULL, TO_ROOM );

            for ( d = descriptor_list; d != NULL; d = d->next )
            {
                if ( !d->character || d->connected < 0 )
                {
                    continue;
                }
                if( d->connected != CON_PLAYING || d->character == ch || d->character->in_room == NULL )
                {
                    continue;
                }
                if ( !SAME_AREA( d->character->in_room->area, ch->in_room->area ) || !SAME_AREA_PART( d->character, ch ) )
                {
                    continue;
                }
                if ( !can_see( ch, d->character ) )
                {
                    continue;
                }
                if ( EXT_IS_SET( d->character->in_room->room_flags, ROOM_INDOORS ) || IS_SET( sector_table[ d->character->in_room->sector_type ].flag, SECT_NOWEATHER ) )
                {
                    continue;
                }
                if ( ch->in_room == d->character->in_room )
                {
                    continue;
                }
                print_char( ch, "{x%s - Widzisz tam {C%s{x.\n\r", d->character->in_room->name, d->character->name4 );
            }
            return;
        }
        door = get_door( ch, argument );
        if ( door >= 0 && check_vname( ch, door, FALSE ) )
        {
            send_to_char("W ktorym kierunku chcesz siê rozejrzeæ?\n\r", ch);
            return;
        }
        if ( door < 0 )
        {
            door = -1;
            for ( iter = 0; iter <= 5; iter++ )
            {
                if ( ( pexit = in_room->exit[iter] ) != NULL &&   !str_prefix( argument, pexit->vName ) &&  !( (IS_SET(pexit->exit_info, EX_SECRET) || IS_SET(pexit->exit_info, EX_HIDDEN))&& !IS_AFFECTED(ch,AFF_DETECT_HIDDEN) ) )
                {
                    door = iter;
                    break;
                }
            }
        }
        if ( door < 0 || in_room->exit[door] == NULL )
        {
            send_to_char("W ktorym kierunku chcesz siê rozejrzeæ?\n\r", ch);
            return;
        }
        lookdoor = door;
        door = -1;
    }
    if ( lookdoor < 0 )
    {
        if ( IS_NPC( ch ) || !ch->pcdata->mind_in )
        {
            act("$n rozgl±da siê dooko³a.", ch, NULL, NULL, TO_ROOM);
        }
        send_to_char("Rozgl±daj±c siê dooko³a widzisz:\n\r", ch);
        sprintf( buf, "{CTutaj{x\n\r" );
        mob_count = 0;
        mob_count_new = scan_room (ch, in_room, buf, 0);
        if ( mob_count_new < 0 )
        {
            send_to_char( "{CTutaj{x{c - nieprzenikniona ciemno¶æ.{x\n\r", ch );
            darkness = TRUE;
        }
        else
        {
            mob_count += mob_count_new;
        }
        strcat(buf, "{x");
        if ( mob_count > 0 )
        {
            send_to_char ( buf, ch );
        }
        else
        {
            if ( mob_count == 0 && !darkness)
            {
                send_to_char( "{CTutaj{x{c - nikogo nie ma.{x\n\r", ch );
            }
        }
    }
    darkness = FALSE;

    if ( !IS_NPC( ch ) && !EXT_IS_SET( ch->act, PLR_HOLYLIGHT ) )
    {
        if ( room_is_dark(ch, in_room) && !IS_AFFECTED(ch, AFF_INFRARED) )
        {
            send_to_char("Jest tutaj zbyt ciemno aby powiedzieæ co¶ o okolicy.\n\r", ch);
            return;
        }
    }
    for ( door = 0; door < MAX_DIR; door++ )
    {
        room = in_room;
        if ( lookdoor >= 0 && door != lookdoor )
        {
            continue;
        }
        if ( ( pexit = room->exit[door] ) == NULL )
        {
            continue;
        }
        if ( lookdoor >= 0 || !( ( IS_SET(pexit->exit_info, EX_SECRET) && !IS_SET(pexit->exit_info, EX_HIDDEN) ) || ( IS_SET(pexit->exit_info, EX_HIDDEN) && !IS_AFFECTED(ch,AFF_DETECT_HIDDEN) ) ) )
        {
            if ( pexit )
            {
                if ( pexit->vName && pexit->vName[0] != '\0' )
                {
                    sprintf( exitname, "%s", pexit->vName );
                }
                else
                {
                    if ( IS_AFFECTED( ch, AFF_MAZE ) )
                    {
                        sprintf ( exitname, "%s", capitalize( dir_long_name[ number_range( 0, 5 ) ] ) );
                    }
                    else
                    {
                        sprintf( exitname, "%s", capitalize( dir_name[door] ) );
                    }
                }
            }
            else
            {
                exitname[0] = '\0';
            }
            if ( !room_is_dark( ch, in_room ) )
            {
                if ( IS_SET( pexit->exit_info, EX_HIDDEN ) )
                {
                    sprintf( buf, "{C%s{x{c - kto¶ stara³ siê ukryæ to przej¶cie.{x\n\r", exitname );
                    send_to_char( buf, ch );
                    continue;
                }
                else if ( IS_SET( pexit->exit_info, EX_CLOSED ) && !IS_AFFECTED(ch, AFF_PIERCING_SIGHT) )
                {
                    if ( pexit->biernik != NULL && pexit->biernik[ 0 ] != '\0' && pexit->biernik[ 0 ] != ' ' )
                    {
                        if( !pexit->liczba_mnoga )
                        {
                            sprintf( buf, "{C%s{x{c - nic nie widzisz przez zamkniêt± %s.{x\n\r", exitname, pexit->biernik );
                        }
                        else
                        {
                            sprintf( buf, "{C%s{x{c - nic nie widzisz przez zamkniête %s.{x\n\r", exitname, pexit->biernik );
                        }
                        send_to_char( buf, ch );
                    }
                    else
                    {
                        sprintf( buf, "{C%s{x{c - nic nie widzisz przez zamkniête drzwi.{x\n\r", exitname );
                        send_to_char( buf, ch );
                    }
                    continue;
                }
                else if ( IS_SET( pexit->exit_info, EX_WALL_OF_MIST ) && !IS_AFFECTED(ch, AFF_PIERCING_SIGHT) )
                {
                    sprintf( buf, "{C%s{x{c - ¶ciana mg³y zas³ania ci widoczno¶æ.{x\n\r", exitname );
                    send_to_char( buf, ch );
                    continue;
                }
            }

            buf[0] = '\0';
            mob_count = 0;
            max_dist = (lookdoor < 0)? 2:4;

            /**
             * imorovmen range for elfs
             * http://forum.mud.pl/viewtopic.php?t=5457
             */
            if(!str_cmp( race_table[ GET_RACE(ch) ].name, "elf" ))
            {
                max_dist += number_range(0, 2);
            }
            for ( distance = 1 ; distance < max_dist; distance++ )
            {
                pexit = room->exit[door];
                if ( !pexit || !pexit->u1.to_room || ( IS_SET( pexit->exit_info, EX_CLOSED ) && !IS_AFFECTED(ch, AFF_PIERCING_SIGHT) ) || ( IS_SET( pexit->exit_info, EX_WALL_OF_MIST ) && !IS_AFFECTED(ch, AFF_PIERCING_SIGHT) ))
                {
                    break;
                }
                darkness = FALSE;
                mob_count_new = scan_room ( ch, pexit->u1.to_room, buf, distance);
                if (  mob_count_new < 0)
                {
                    printf_to_char( ch, "{C%s{x{c - panuje tam nieprzenikniona ciemno¶æ.{x\n\r", exitname );
                    darkness = TRUE;
                }
                else
                {
                    mob_count += mob_count_new;
                }

                room = pexit->u1.to_room;
            }
            strcat(buf, "{x");

            if ( mob_count > 0 && !darkness )
            {
                if ( room_is_dark( ch, in_room ) )
                {
                    printf_to_char( ch, "{C%s{x{c - widaæ tam chyba kogo¶.{x\n\r", exitname );
                }
                else
                {
                    print_char( ch, "{C%s{x\n\r", exitname );
                    send_to_char ( buf, ch );
                }
            }
            else if ( !room_is_dark( ch, in_room ) && !darkness )
            {
                if ( mob_count == 0 && in_room->exit[door])
                {
                    printf_to_char( ch, "{C%s{x{c - nikogo tam nie widaæ.{x\n\r", exitname );
                }
            }
            darkness = FALSE;
        }
    }

    if ( room_is_dark( ch, in_room ) )
    {
        send_to_char("\n\rJest tutaj zbyt ciemno aby powiedzieæ co¶ dok³adniej o okolicy.\n\r", ch);
        return;
    }
    return;
}

