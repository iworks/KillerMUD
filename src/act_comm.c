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
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Brosig Michal         (brohacz@gmail.com             ) [Brohacz   ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (tzdziech@gmail.com            ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: act_comm.c 11427 2012-06-12 16:35:28Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/12.02/src/act_comm.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"
#include "lang.h"
#include "name_check.h"
#include "progs.h"
#include "shapeshifting.h"
#include "clans.h"
#include "friend_who.h"
#include "projects.h"
#include "money.h"

#define SPEAK_SAY      1
#define SPEAK_SAYTO    2
#define SPEAK_ASK      3
#define SPEAK_SHOUT    4
#define SPEAK_TELL     5
#define SPEAK_REPLY    6
#define SPEAK_YELL     7

/* RT code to delete yourself */
void menu_show( CHAR_DATA *ch );
void update_death_statistic( CHAR_DATA *victim, CHAR_DATA *ch );
void forget( CHAR_DATA *mob, CHAR_DATA *who, int reaction, bool All );
bool is_ignoring( CHAR_DATA *ch, char *argument );
int  find_door args( ( CHAR_DATA *ch, char *arg ) );
bool check_noremovable_affects args ( ( AFFECT_DATA *aff ) );

int can_speak( CHAR_DATA *ch, char *argument, int speak_type )
{
    int speaking;
    char arg[ MAX_INPUT_LENGTH ];
    speaking = ch->speaking;
    /* dajemy standardowy tekst jak go¶æ jest pod wp³ywem 'silence' */
    if ( IS_AFFECTED( ch, AFF_SILENCE ) )
    {
        send_to_char( AFF_SILENCE_TEXT, ch );
        return 0;
    }
    if ( !IS_NPC( ch ) && ( strlen ( argument ) > 300 ) && !IS_IMMORTAL ( ch ) )
    {
        act( "Spokojnie, nie mów tyle na raz!", ch, NULL, NULL, TO_CHAR );
        return 0;
    }
    if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ) )
    {
        send_to_char( "Pod wod±? Nie da rady.\n\r", ch );
        switch (ch->sex)
        {
        case SEX_NEUTRAL:
            act( "$n probuje co¶ powiedzieæ, chmura b±belków powietrza wydobywa sie temu z ust.", ch, NULL, NULL, TO_ROOM );
            break;
        case SEX_MALE:
            act( "$n probuje co¶ powiedzieæ, chmura b±belków powietrza wydobywa sie mu z ust.", ch, NULL, NULL, TO_ROOM );
            break;
        case SEX_FEMALE:
        default:
            act( "$n probuje co¶ powiedzieæ, chmura b±belków powietrza wydobywa sie jej z ust.", ch, NULL, NULL, TO_ROOM );
            break;
        }
        return 0;
    }
    if ( speaking < 0 || speaking > MAX_LANG )
    {
        ch->speaking = 0;
        send_to_char( "Chcia³<&/a/o>by¶ co¶ powiedzieæ, ale zapomnia³<&e/a/o>s¶ jêzyka.\n\r", ch );
        return 0;
    }
    one_argument( argument, arg );
    switch ( speak_type )
    {
    case SPEAK_SAY:
        if ( argument[ 0 ] == '\0' )
        {
            print_char( ch, "Co chcesz powiedzieæ?\n\r" );
            return 0;
        }
        break;
    case SPEAK_SAYTO:
        if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
        {
            print_char( ch, "Komu i co powiedzieæ?\n\r" );
            return 0;
        }
        break;
    case SPEAK_ASK:
        if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
        {
            print_char( ch, "Kogo i o co zapytaæ?\n\r" );
            return 0;
        }
        break;
    case SPEAK_SHOUT:
        if ( argument[ 0 ] == '\0' )
        {
            print_char( ch, "O czym chcia³<&/a/o>by¶ pokrzyczeæ?\n\r");
            return 0;
        }
        break;
    case SPEAK_TELL:
        if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
        {
            print_char( ch, "Komu i co powiedzieæ?\n\r" );
            return 0;
        }
        break;
    case SPEAK_REPLY:
        if ( argument[ 0 ] == '\0' )
        {
            print_char( ch, "Co chcesz odpowiedzieæ?\n\r" );
            return 0;
        }
        break;
    case SPEAK_YELL:
        if ( argument[ 0 ] == '\0' )
        {
            print_char( ch, "O czym chcesz powrzeszczeæ?\n\r" );
            return 0;
        }
        break;
    }
    return 1;
}
/* do_delete */
void do_delete( CHAR_DATA *ch, char *argument )
{
    CLAN_DATA * clan;
    char strsave[ MAX_INPUT_LENGTH ];
    char buf[ MAX_INPUT_LENGTH ];
    if ( IS_NPC( ch ) )
        return;
    /*artefact*/
    remove_char_from_artefact_list( ch );
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
    if ( ch->sex == 0 )
    {
        sprintf( buf, "%s skasowa³o siê.", ch->name );
        wiznet( "$N skasowa³o siê.", ch, NULL, 0, 0, 0 );
    }
    else if ( ch->sex == 1 )
    {
        sprintf( buf, "%s skasowa³ siê.", ch->name );
        wiznet( "$N skasowa³ siê.", ch, NULL, 0, 0, 0 );
    }
    else
    {
        sprintf( buf, "%s skasowa³a siê.", ch->name );
        wiznet( "$N skasowa³a siê.", ch, NULL, 0, 0, 0 );
    }
    log_string( buf );
    sprintf( buf, "%s", ch->name );
    stop_fighting( ch, TRUE );
    do_function( ch, &do_quit, "" );
    unlink( strsave );
    if ( ( clan = get_clan_by_member( buf ) ) != NULL )
    {
        remove_from_clan( clan, get_member( clan, buf ) );
        save_clans();
    }
    return;
}
/* afk command */
void do_afk ( CHAR_DATA *ch, char * argument )
{
    if ( IS_NPC( ch ) )
        return;
    if ( IS_SET( ch->comm, COMM_AFK ) )
    {
        send_to_char( "Tryb AFK wy³±czony. Wpisz {Rreplay{x, ¿eby sprawdziæ czy kto¶ co¶ chcia³.\n\r", ch );
        REMOVE_BIT( ch->comm, COMM_AFK );
        if ( IS_IMMORTAL( ch ) )
        {
            free_string( ch->pcdata->afk_text );
            ch->pcdata->afk_text = NULL;
        }
        return;
    }
    else
    {
        send_to_char( "Tryb AFK w³±czony.\n\r", ch );
        SET_BIT( ch->comm, COMM_AFK );
        if ( IS_IMMORTAL( ch ) )
        {
            if ( ch->pcdata->afk_text )
            {
                free_string( ch->pcdata->afk_text );
                ch->pcdata->afk_text = NULL;
            }
            if ( argument[ 0 ] != '\0' )
                ch->pcdata->afk_text = str_dup( argument );
        }
        return;
    }
    return;
}
void do_replay ( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC( ch ) )
        return;
    if ( buf_string( ch->pcdata->buffer ) [ 0 ] == '\0' )
    {
        send_to_char( "Nie ma dla ciebie ¿adnych informacji.\n\r", ch );
        return;
    }
    page_to_char( buf_string( ch->pcdata->buffer ), ch );
    clear_buf( ch->pcdata->buffer );
}
/* do_immtalk */
void do_immtalk( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA * d;
    if ( argument[ 0 ] == '\0' )
    {
        if ( IS_SET( ch->comm, COMM_NOWIZ ) )
        {
            send_to_char( "Kana³ nie¶miertelnych jest teraz {GW£¡CZONY{x.\n\r", ch );
            REMOVE_BIT( ch->comm, COMM_NOWIZ );
        }
        else
        {
            send_to_char( "Kana³ nie¶miertelnych jest teraz {RWY£¡CZONY{x.\n\r", ch );
            SET_BIT( ch->comm, COMM_NOWIZ );
        }
        return;
    }
    REMOVE_BIT( ch->comm, COMM_NOWIZ );
    act_new( "{i[{I$n{i]: $t{x", ch, argument, NULL, TO_CHAR, POS_DEAD );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( !d->character || d->connected < 0 ) continue;
        if ( d->connected == CON_PLAYING &&
                IS_IMMORTAL( d->character ) &&
                !IS_SET( d->character->comm, COMM_NOWIZ ) )
        {
            act_new( "{i[{I$n{i]: $t{x", ch, argument, d->character, TO_VICT, POS_DEAD );
        }
    }
    return;
}
/* do_say */
void do_say( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * vch;
    MEMDAT * mdat;
    char buf[ MAX_STRING_LENGTH ];
    int speaking;
    int speakswell;
    char *sbuf;
    int ri, response;
    bool question = FALSE, scream = FALSE;

    if ( ! can_speak( ch, argument, SPEAK_SAY ) )
    {
        return;
    }
    if ( !IS_NPC( ch ) && ch->level < 32 )
    {
        argument = rpg_control( argument );
    }
    argument = strip_dupspaces ( argument );
    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Co chcesz powiedzieæ?\n\r", ch );
        return;
    }
    /**
     * log it
     */
    if ( !IS_NPC( ch ) )
    {
        append_file_format_daily( ch, SPEECH_LOG_FILE, "%s", argument );
    }
    // ustawiamy warunki czy to krzyk, czy te¿ pytanie
    if ( !IS_NPC( ch ) )
    {
        question = argument[ strlen( argument ) - 1 ] == '?';
        scream   = argument[ strlen( argument ) - 1 ] == '!';
    }
    if ( question )
    {
        act( "{6Pytasz '{7$T{6'{x", ch, NULL, argument, TO_CHAR );
    }
    else if ( scream )
    {
        act( "{6Wykrzykujesz '{7$T{6'{x", ch, NULL, argument, TO_CHAR );
    }
    else
    {
        sprintf(buf,"{6%s '{7$T{6'{x", race_table[ GET_RACE( ch ) ].say_text_2);
        act( buf, ch, NULL, argument, TO_CHAR );
    }
    speaking = ch->speaking;
    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
        sbuf = argument;
        if ( vch == ch || IS_AFFECTED( vch, AFF_DEAFNESS ) || IS_AFFECTED( vch, AFF_FORCE_FIELD ) ) continue;
        speakswell = UMIN( knows_language( vch, ch->speaking, ch ), knows_language( ch, ch->speaking, vch ) );
        // zmniejszamy rozumno¶æ mowy dla DAZE!
        if ( IS_AFFECTED( ch, AFF_DAZE ) || IS_AFFECTED( vch, AFF_DAZE ) )
        {
            speakswell /= 2;
        }
        if ( speakswell < 95 )
        {
            sbuf = translate( speakswell, argument, lang_table[ speaking ].name );
        }
        if ( !is_ignoring( vch, ch->name ) )
        {
            if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
                    !IS_NPC( vch ) && !IS_IMMORTAL( vch ) &&
                    IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
            {
                if ( question )
                {
                    act( "{6Nie¶miertelny pyta '{7$t{6'{x", ch, sbuf, vch, TO_VICT );
                }
                else if ( scream )
                {
                    act( "{6Nie¶miertelny wykrzykuje '{7$t{6'{x", ch, sbuf, vch, TO_VICT );
                }
                else
                {
                    act( "{6Nie¶miertelny mówi '{7$t{6'{x", ch, sbuf, vch, TO_VICT );
                }
            }
            else
            {
                if ( !str_cmp ( race_table[ GET_RACE( ch ) ].name , "gnom" ) && str_cmp ( race_table[ GET_RACE( vch ) ].name , "gnom" ) )
                {
                    sbuf = translate_gnomish ( sbuf );
                }
                if ( question )
                {
                    sprintf(buf, "{6$n %s '{7$t{6'{x", race_table[ GET_RACE( ch ) ].ask_text);
                }
                else if ( scream )
                {
                    sprintf(buf, "{6$n wykrzykuje '{7$t{6'{x");
                }
                else
                {
                    sprintf(buf, "{6$n %s '{7$t{6'{x", race_table[ GET_RACE( ch ) ].say_text);
                }
                act( buf, ch, sbuf, vch, TO_VICT );
            }
        }
        mdat = get_mob_memdat( vch, ch, NULL, MEMDAT_RESPONSEDELAY );
        if ( !IS_NPC( ch ) && IS_NPC( vch ) && mdat && mdat->response )
        {
            response = 0;
            ri = 0;
            sbuf = mdat->response;
            for (;; )
            {
                ri++;
                sbuf = one_argument( sbuf, buf );
                if ( !str_cmp( argument, buf ) )
                {
                    response = ri;
                    break;
                }
                if ( sbuf[ 0 ] == '\0' )
                {
                    break;
                }
            }
            if ( response > 0 )
            {
                vch->prog_response = response;
                add_prog_env( mdat->prog_env );
                program_flow();
                free_mdat( mdat );
                vch->prog_response = 0;
            }
            continue;
        }
        if ( !IS_NPC( ch ) && IS_NPC( vch ) && HAS_TRIGGER( vch, TRIG_SPEECH ) )
        {
            mp_act_trigger( argument, vch, ch, NULL, NULL, &TRIG_SPEECH );
        }
    }
    if ( !IS_NPC( ch ) && HAS_RTRIGGER( ch->in_room, TRIG_SPEECH ) )
    {
        rp_act_trigger( argument, ch, &TRIG_SPEECH );
    }
    return;
}
/* do_sayto */
void do_sayto( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA * vch;
    CHAR_DATA * victim;
    MEMDAT * mdat;
    char buf[ MAX_STRING_LENGTH ];
    char name2[ MAX_STRING_LENGTH ];
    char name3[ MAX_STRING_LENGTH ];
    char name4[ MAX_STRING_LENGTH ];
    char arg[ MAX_INPUT_LENGTH ];
    int speaking;
    int speakswell;
    char *sbuf;
    int ri, response;
    bool question = FALSE, scream = FALSE;

    if ( ! can_speak( ch, argument, SPEAK_SAYTO ) )
    {
        return;
    }
    if ( !IS_NPC( ch ) && ch->level < 32 )
    {
        argument = rpg_control( argument );
    }

    argument = one_argument( argument, arg );
    // ustawiamy warunki czy to krzyk, czy te¿ pytanie
    if ( !IS_NPC( ch ) )
    {
        question = argument[ strlen( argument ) - 1 ] == '?';
        scream   = argument[ strlen( argument ) - 1 ] == '!';
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Nie ma tutaj tej osoby.\n\r", ch );
        return;
    }
    argument = strip_dupspaces ( argument );
    if ( argument[ 0 ] == '\0' )
    {
        if ( ch == victim )
        {
            for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
            {
                if ( !is_ignoring( vch, ch->name ) )
                {
                    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
                            !IS_NPC( vch ) && !IS_IMMORTAL( vch ) &&
                            IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
                    {
                        act( "{6Nie¶miertelny mamarocze co¶ pod nosem.{x", ch, NULL, vch, TO_VICT );
                    }
                    else
                    {
                        act( "{6$n mamrocze co¶ pod nosem.{x", ch, NULL, vch, TO_VICT );
                    }
                }
            }
        }
        send_to_char( "Co chcesz powiedzieæ?\n\r", ch );
        return;
    }
    if ( ch == victim )
    {
        sbuf = argument;
        sprintf( buf,"{6%s pod nosem '{7$t{6'{x", race_table[ GET_RACE ( ch ) ].say_text_2);
        act( buf, ch, sbuf, NULL, TO_CHAR );
        for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
        {
            if ( !is_ignoring( vch, ch->name ) )
            {
                if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
                        !IS_NPC( vch ) && !IS_IMMORTAL( vch ) &&
                        IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
                {
                    act( "{6Nie¶miertelny mówi pod nosem '{7$t{6'{x", ch, sbuf, vch, TO_VICT );
                }
                else
                {
                    if ( !str_cmp ( race_table[ GET_RACE( ch ) ].name , "gnom" ) && str_cmp ( race_table[ GET_RACE( vch ) ].name , "gnom" ) )
                    {
                        sbuf = translate_gnomish ( sbuf );
                    }
                    sprintf( buf,"{6$n %s pod nosem '{7$t{6'{x", race_table[ GET_RACE ( ch ) ].say_text);
                    act( buf, ch, sbuf, vch, TO_VICT );
                }
            }
        }
        return;
    }
    if ( is_ignoring( victim, ch->name ) )
    {
        return;
    }
    if ( IS_AFFECTED( victim, AFF_DEAFNESS ) || IS_AFFECTED( victim, AFF_FORCE_FIELD ) )
    {
        send_to_char( "Nie mo¿e ciê us³yszeæ.\n\r", ch );
        return;
    }
    if ( !( IS_IMMORTAL( ch ) && ch->level > LEVEL_IMMORTAL ) && !IS_AWAKE( victim ) )
    {
        act( "$E nie mo¿e ciê us³yszeæ.", ch, 0, victim, TO_CHAR );
        return;
    }
    if ( question )
    {
        act( "{6Pytasz $C '{7$t{6'{x", ch, argument, victim, TO_CHAR );
    }
    else if ( scream )
    {
        act( "{6Wykrzykujesz do $Z '{7$t{6'{x", ch, argument, victim, TO_CHAR );
    }
    else
    {
        sprintf( buf,"{6%s $X '{7$t{6'{x", race_table[ GET_RACE ( ch ) ].say_text_2);
        act( buf, ch, argument, victim, TO_CHAR );
    }
    /**
     * log it
     */
    if ( !IS_NPC( ch ) )
    {
        append_file_format_daily( ch, SPEECH_LOG_FILE, "sayto %s: %s", victim->name, argument );
    }
    speaking = ch->speaking;
    for ( vch = ch->in_room->people; vch; vch = vch->next_in_room )
    {
        sbuf = argument;
        if ( vch == ch || IS_AFFECTED( vch, AFF_DEAFNESS ) || IS_AFFECTED( vch, AFF_FORCE_FIELD ) ) continue;
        speakswell = UMIN( knows_language( vch, ch->speaking, ch ), knows_language( ch, ch->speaking, vch ) );
        if ( speakswell < 95 )
        {
            sbuf = translate( speakswell, argument, lang_table[ speaking ].name );
        }
        if ( !is_ignoring( vch, ch->name ) )
        {
            if ( can_see( vch, victim ) )
            {
                sprintf( name2, "%s '{7%s{6'{x", victim->name2, sbuf );
                sprintf( name3, "%s '{7%s{6'{x", victim->name3, sbuf );
                sprintf( name4, "%s '{7%s{6'{x", victim->name4, sbuf );
            }
            else
            {
                sprintf( name2, "kogo¶ '{7%s{6'{x", sbuf );
                sprintf( name3, "komu¶ '{7%s{6'{x", sbuf );
                sprintf( name4, "kogo¶ '{7%s{6'{x", sbuf );
            }
            if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
                    !IS_NPC( vch ) && !IS_IMMORTAL( vch ) &&
                    IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
            {
                if ( vch == victim )
                {
                    if ( question )
                    {
                        act( "{6Nie¶miertelny pyta siê ciebie '{7$t{6'{x", ch, sbuf, vch, TO_VICT );
                    }
                    else if ( scream )
                    {
                        act( "{6Nie¶miertelny wykrzykuje do ciebie '{7$t{6'{x", ch, sbuf, vch, TO_VICT );
                    }
                    else
                    {
                        act( "{6Nie¶miertelny mówi tobie '{7$t{6'{x", ch, sbuf, vch, TO_VICT );
                    }
                }
                else
                {
                    if ( question )
                    {
                        act( "{6Nie¶miertelny pyta $t", ch, name4, vch, TO_VICT );
                    }
                    else if ( scream )
                    {
                        act( "{6Nie¶miertelny wykrzykuje do $t", ch, name2, vch, TO_VICT );
                    }
                    else
                    {
                        act( "{6Nie¶miertelny mówi $t", ch, name3, vch, TO_VICT );
                    }
                }
            }
            else
            {
                if ( !str_cmp ( race_table[ GET_RACE( ch ) ].name , "gnom" ) && str_cmp ( race_table[ GET_RACE( vch ) ].name , "gnom" ) )
                {
                    sbuf = translate_gnomish ( sbuf );
                }
                if ( vch == victim )
                {
                    if ( question )
                    {
                        act( "{6$n pyta siê ciebie '{7$t{6'{x", ch, sbuf, vch, TO_VICT );
                    }
                    else if ( scream )
                    {
                        act( "{6$n wykrzykuje w twoim kierunku '{7$t{6'{x", ch, sbuf, vch, TO_VICT );
                    }
                    else
                    {
                        sprintf( buf,"{6$n %s tobie '{7$t{6'{x", race_table[ GET_RACE ( ch ) ].say_text);
                        act( buf, ch, sbuf, vch, TO_VICT );
                    }
                }
                else
                {
                    if ( question )
                    {
                        sprintf(buf, "{6$n %s $t{x", race_table[ GET_RACE( ch ) ].ask_text);
                        act( buf, ch, ( !str_cmp( race_table[ GET_RACE( ch ) ].ask_text , "pyta")) ? name4:name3, vch, TO_VICT );
                    }
                    else if ( scream )
                    {
                        sprintf(buf, "{6$n wykrzykuje w kierunku $t");
                        act( buf, ch, name2, vch, TO_VICT );
                    }
                    else
                    {
                        sprintf(buf, "{6$n %s $t{x", race_table[ GET_RACE( ch ) ].say_text);
                        act( buf, ch, name3, vch, TO_VICT );
                    }
                }
            }
        }
        mdat = get_mob_memdat( vch, ch, NULL, MEMDAT_RESPONSEDELAY );
        if ( !IS_NPC( ch ) && IS_NPC( vch ) && mdat && mdat->response )
        {
            response = 0;
            ri = 0;
            sbuf = mdat->response;
            for (;; )
            {
                ri++;
                sbuf = one_argument( sbuf, buf );
                if ( !str_cmp( argument, buf ) )
                {
                    response = ri;
                    break;
                }
                if ( sbuf[ 0 ] == '\0' )
                    break;
            }
            if ( response > 0 )
            {
                vch->prog_response = response;
                add_prog_env( mdat->prog_env );
                program_flow();
                free_mdat( mdat );
                vch->prog_response = 0;
            }
            continue;
        }
        if ( !IS_NPC( ch ) && IS_NPC( vch ) && HAS_TRIGGER( vch, TRIG_SPEECH ) )
        {
            mp_act_trigger( argument, vch, ch, NULL, NULL, &TRIG_SPEECH );
        }
    }
    if ( !IS_NPC( ch ) && HAS_RTRIGGER( ch->in_room, TRIG_SPEECH ) )
    {
        rp_act_trigger( argument, ch, &TRIG_SPEECH );
        return;
    }
    if ( !IS_NPC( ch ) && IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_TELL ) )
    {
        mp_act_trigger( argument, victim, ch, NULL, NULL, &TRIG_TELL );
    }
    return;
}
/* do_ask */
void do_ask( CHAR_DATA * ch, char * argument )
{
    CHAR_DATA * victim;
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    int speaking;
    int speakswell;
    char *sbuf;
    time_t czas;
    bool is_immortal = FALSE;
    bool introduced, tmp;

    if ( ! can_speak( ch, argument, SPEAK_ASK ) )
    {
        return;
    }
    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
        return;
    }

    if ( IS_SET( ch->comm, COMM_NOTELL ) )
    {
        send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
        return;
    }
    if ( IS_SET( ch->comm, COMM_QUIET ) )
    {
        send_to_char( "Wy³±cz tryb quiet.\n\r", ch );
        return;
    }
    if ( IS_SET( ch->comm, COMM_DEAF ) )
    {
        send_to_char( "Wy³±cz tryb deaf.\n\r", ch );
        return;
    }
    argument = one_argument( argument, arg );
    if ( ( victim = get_char_world( ch, arg ) ) == NULL ||
            ( IS_NPC( victim ) && victim->in_room != ch->in_room ) )
    {
        send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
        return;
    }
    if (!IS_NPC(ch) && IS_IMMORTAL( ch) )
    {
        is_immortal = TRUE;
    }
    if ( !is_immortal )
    {
        if ( !IS_NPC( ch )
                && !IS_NPC( victim )
                && victim->in_room != ch->in_room
                && ( !friend_exist( ch, victim->name, NULL, NULL, &czas, &introduced ) || !friend_exist( victim, ch->name, NULL, NULL, &czas, &tmp ) )
                && !is_same_clan( ch, victim )
                && victim->level > LEVEL_WHO_SHOW ) //do postaci do 14 lev mozna mowic na tell
        {
            if ( !introduced )
            {
                send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
                return;
            }
        }
    }
    if ( victim == ch )
    {
        send_to_char( "Pewnie sam sobie te¿ odpowiesz.\n\r", ch );
        return;
    }
    if ( is_ignoring( victim, ch->name ) )
        return;
    if ( IS_AFFECTED( victim, AFF_DEAFNESS ) || IS_AFFECTED( victim, AFF_FORCE_FIELD ) )
    {
        print_char( ch, "Twoja wiadomo¶æ nie dotar³a.\n\r", IS_NPC( victim ) ? victim->short_descr : victim->name );
        return;
    }
    speaking = ch->speaking;
    speakswell = UMIN( knows_language( victim, ch->speaking, ch ),
                       knows_language( ch, ch->speaking, victim ) );
    sbuf = argument;
    if ( !IS_NPC( ch ) && ch->level < 32 )
        argument = rpg_control( argument );
    argument = strip_dupspaces ( argument );
    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
        return;
    }
    if ( speakswell < 95 )
        sbuf = translate( speakswell, argument, lang_table[ speaking ].name );
    if ( victim->desc == NULL && !IS_NPC( victim ) )
    {
        act( "$N ma zerwane po³±czenie... spróbuj pó¼niej.", ch, NULL, victim, TO_CHAR );
        if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
                !IS_NPC( victim ) && !IS_IMMORTAL( victim ) &&
                IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
            sprintf( buf, "{qNie¶miertelny pyta siê ciebie '{Q%s{q'{x\n\r", sbuf );
        else
        {
            if ( !str_cmp ( race_table[ GET_RACE( ch ) ].name , "gnom" ) &&
                    str_cmp ( race_table[ GET_RACE( victim ) ].name , "gnom" ) )
                sbuf = translate_gnomish ( sbuf );
            sprintf( buf, "{q%s pyta siê ciebie '{Q%s{q'{x\n\r", PERS( ch, victim ), sbuf );
        }
        sprintf( buf, "%s", capitalize( buf ) );
        add_buf( victim->pcdata->buffer, buf );
        return;
    }
    if ( !( IS_IMMORTAL( ch ) && ch->level > LEVEL_IMMORTAL ) && !IS_AWAKE( victim ) )
    {
        act( "$E nie mo¿e ciê us³yszeæ.", ch, 0, victim, TO_CHAR );
        return;
    }
    if ( ( IS_SET( victim->comm, COMM_QUIET ) || IS_SET( victim->comm, COMM_DEAF ) ) && !IS_IMMORTAL( ch ) )
    {
        act( "$E nie mo¿e ciê us³yszeæ.", ch, 0, victim, TO_CHAR );
        return;
    }
    /**
     * log it
     */
    if ( !IS_NPC( ch ) )
    {
        append_file_format_daily( ch, SPEECH_LOG_FILE, "ask %s: %s", victim->name, argument );
    }
    if ( IS_SET( victim->comm, COMM_AFK ) )
    {
        if ( IS_NPC( victim ) )
        {
            act( "$E jest AFK i nie odbiera wiadomo¶ci.", ch, NULL, victim, TO_CHAR );
            return;
        }
        act( "$E jest AFK, ale twoja wiadomo¶æ dojdzie kiedy $E wróci.",
             ch, NULL, victim, TO_CHAR );
        if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
                !IS_NPC( victim ) && !IS_IMMORTAL( victim ) &&
                IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
            sprintf( buf, "{qNie¶miertelny pyta siê ciebie '{Q%s{q'{x\n\r", sbuf );
        else
        {
            if ( !str_cmp ( race_table[ GET_RACE( ch ) ].name , "gnom" ) &&
                    str_cmp ( race_table[ GET_RACE( victim ) ].name , "gnom" ) )
                sbuf = translate_gnomish ( sbuf );
            sprintf( buf, "{q%s pyta siê ciebie '{Q%s{q'{x\n\r", PERS( ch, victim ), sbuf );
        }
        sprintf( buf, "%s", capitalize( buf ) );
        add_buf( victim->pcdata->buffer, buf );
        return;
    }
    act( "{qPytasz $C '{Q$t{q'{x", ch, argument, victim, TO_CHAR );
    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
            !IS_NPC( victim ) && !IS_IMMORTAL( victim ) &&
            IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
        act_new( "{qNie¶miertelny pyta siê ciebie '{Q$t{q'{x", ch, sbuf, victim, TO_VICT, POS_DEAD );
    else
    {
        if ( !str_cmp ( race_table[ GET_RACE( ch ) ].name , "gnom" ) &&
                str_cmp ( race_table[ GET_RACE( victim ) ].name , "gnom" ) )
            sbuf = translate_gnomish ( sbuf );
        act( "{q$n pyta siê ciebie '{Q$t{q'{x", ch, sbuf, victim, TO_VICT );
    }
    if ( ( IS_NPC( ch ) || ( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) ) ) && ch->in_room == victim->in_room )
        act( "$n mówi co¶ do $Z.", ch, NULL, victim, TO_NOTVICT );
    victim->reply	= ch;
    if ( !IS_NPC( ch ) && IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_ASK ) )
        mp_act_trigger( argument, victim, ch, NULL, NULL, &TRIG_ASK );
    return;
}
/* do_shout */
void do_shout( CHAR_DATA * ch, char * argument )
{
    CHAR_DATA * victim;
    DESCRIPTOR_DATA *d;
    int speaking;
    AFFECT_DATA af, *aff;
    char *sbuf;
    int speakswell;

    if ( ch -> level < 32 )
    {
        return;
    }
    if ( ! can_speak( ch, argument, SPEAK_SHOUT ) )
    {
        return;
    }
    if ( IS_SET( ch->comm, COMM_NOSHOUT ) )
    {
        send_to_char( "Nie mo¿esz krzyczeæ.\n\r", ch );
        return;
    }
    aff = affect_find( ch->affected, gsn_yell );
    if ( aff && aff->duration > 1 && !IS_IMMORTAL( ch ) )
    {
        send_to_char( "Chcia³<&/a/o>by¶ co¶ krzykn±æ, ale straci³<&e/a/o>¶ g³os.\n\r", ch );
        act( "$n porusza ustami wydaj±c jakie¶ nieartyku³owane d¼wiêki.\n\r", ch, NULL, NULL, TO_ROOM );
        return;
    }
    REMOVE_BIT( ch->comm, COMM_SHOUTSOFF );
    WAIT_STATE( ch, 12 );
    if ( !IS_NPC( ch ) && ch->level < 32 )
        argument = rpg_control( argument );
    argument = strip_dupspaces ( argument );
    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Co chcesz krzykn±æ?\n\r", ch );
        return;
    }
    /**
     * log it
     */
    if ( !IS_NPC( ch ) )
    {
        append_file_format_daily( ch, SPEECH_LOG_FILE, "shout %s", argument );
    }
    act( "{tKrzyczysz '{T$T{t'{x", ch, NULL, argument, TO_CHAR );
    if ( !IS_IMMORTAL( ch ) )
    {
        if ( !aff )
        {
            af.where	= TO_AFFECTS;
            af.type	= gsn_yell;
            af.level	= 50; //by nie szlo dispelnac
            af.duration = 1;
            af.rt_duration = 0;
            af.location = APPLY_NONE;
            af.modifier = 0;
            af.bitvector = &AFF_NONE;
            af.visible	= FALSE;
            affect_to_char( ch, &af, NULL, FALSE );
        }
        else
            ++aff->duration;
    }
    speaking = ch->speaking;
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( !d->character || d->connected < 0 ) continue;
        sbuf = argument;
        victim = d->original ? d->original : d->character;
        if ( d->connected == CON_PLAYING &&
                d->character != ch &&
                !IS_SET( victim->comm, COMM_SHOUTSOFF ) &&
                !IS_SET( victim->comm, COMM_QUIET ) &&
                !IS_AFFECTED( victim, AFF_DEAFNESS ) &&
                !IS_AFFECTED( victim, AFF_FORCE_FIELD ) )
        {
            speakswell = UMIN( knows_language( d->character, ch->speaking, ch ),
                               knows_language( ch, ch->speaking, d->character ) );
            if ( speakswell < 95 )
                sbuf = translate( speakswell, argument, lang_table[ speaking ].name );
            if ( !is_ignoring( d->character, ch->name ) )
                act( "{t$n krzyczy '{T$t{t'{x", ch, sbuf, d->character, TO_VICT );
        }
    }
    aff = affect_find( ch->affected, gsn_yell );
    if ( aff == NULL )
        return;
    if ( aff->duration == 1 )
        send_to_char( "Nie krzycz tak, bo ci zaraz gard³o wysi±dzie.\n\r", ch );
    else if ( aff->duration == 2 )
    {
        send_to_char( "No to masz po gardle. Lepiej posied¼ chwilê cicho.\n\r", ch );
        aff->duration = 10;
        aff->rt_duration = 0; /* a co! hehe :) */
    }
    return;
}
/* do_tell */
void do_tell( CHAR_DATA * ch, char * argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    CHAR_DATA *victim;
    MEMDAT * mdat;
    int speaking;
    int speakswell;
    char *sbuf;
    int ri, response;
    bool scream = FALSE, ask = FALSE;
    time_t czas;
    bool is_immortal = FALSE;
    bool introduced, tmp;

    if ( ! can_speak( ch, argument, SPEAK_TELL ) )
    {
        return;
    }
    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
        return;
    }

    sbuf = argument;
    argument = one_argument( argument, arg );
    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Co chcesz powiedzieæ?\n\r", ch );
        return;
    }
    if (!IS_NPC(ch) && IS_IMMORTAL( ch) )
    {
        is_immortal = TRUE;
    }
    if ( !is_immortal )
    {
        if ( IS_SET( ch->comm, COMM_NOTELL ) || IS_SET( ch->comm, COMM_DEAF ) )
        {
            send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
            return;
        }
        if ( IS_SET( ch->comm, COMM_QUIET ) )
        {
            send_to_char( "Wy³±cz tryb quiet.\n\r", ch );
            return;
        }
        if ( IS_SET( ch->comm, COMM_DEAF ) )
        {
            send_to_char( "Wy³±cz tryb deaf.\n\r", ch );
            return;
        }
    }
    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( ( victim = get_char_world( ch, arg ) ) == NULL || ( IS_NPC( victim ) && victim->in_room != ch->in_room ) )
    {
        if (is_immortal)
        {
            send_to_char( "Nie ma takiej osoby.\n\r", ch );
        }
        else
        {
            send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
        }
        return;
    }
    if (   !IS_NPC( ch )
        && !IS_NPC( victim )
        && ( ch->level < LEVEL_NEWBIE && IS_SET( victim->comm, COMM_NONEWBIE ) ) )
    {
        send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
        return;
    }
    if ( ch == victim )
    {
        send_to_char( "Chcesz ze sob± porozmawiaæ? Nie¼le.\n\r", ch );
        return;
    }
    if ( !is_immortal )
    {
        if ( is_ignoring( victim, ch->name ) )
        {
            send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
            return;
        }
        if ( IS_AFFECTED( victim, AFF_DEAFNESS ) || IS_AFFECTED( victim, AFF_FORCE_FIELD ) )
        {
            send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
            return;
        }
        if ( !can_see( ch, victim ) )
        {
            send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
            return;
        }
    }
    /**
     * log it
     */
    if ( !IS_NPC( ch ) )
    {
        append_file_format_daily( ch, SPEECH_LOG_FILE, "tell %s: %s", victim->name, argument );
    }
    // obs³uga pytania
    if ( sbuf[ strlen( sbuf ) - 1 ] == '?' )
    {
        ask = TRUE;
    }
    //obs³uga wykrzyczenia
    if ( sbuf[ strlen( sbuf ) - 1 ] == '!' )
    {
        scream = TRUE;
    }
    sbuf = argument;
    speaking = ch->speaking;
    speakswell = UMIN( knows_language( victim, ch->speaking, ch ),
                       knows_language( ch, ch->speaking, victim ) );
    if ( speakswell < 95 )
    {
        sbuf = translate( speakswell, argument, lang_table[ speaking ].name );
    }
    argument = strip_dupspaces ( argument );
    if ( victim->desc == NULL && !IS_NPC( victim ) )
    {
        act( "$N ma zerwane po³±czenie...spróbuj pó¼niej.", ch, NULL, victim, TO_CHAR );
        if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
                !IS_NPC( victim ) && !IS_IMMORTAL( victim ) &&
                IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
        {
            if ( scream )
            {
                sprintf( buf, "{kNie¶miertelny krzyczy do ciebie '{K%s{k'{x\n\r", sbuf );
            }
            else if ( ask )
            {
                sprintf( buf, "{kNie¶miertelny pyta siê ciebie '{K%s{k'{x\n\r", sbuf );
            }
            else
            {
                sprintf( buf, "{kNie¶miertelny mówi ci '{K%s{k'{x\n\r", sbuf );
            }
        }
        else
        {
            if ( !str_cmp ( race_table[ GET_RACE( ch ) ].name , "gnom" ) &&
                    str_cmp ( race_table[ GET_RACE( victim ) ].name , "gnom" ) )
                sbuf = translate_gnomish ( sbuf );
            if (scream)
            {
                sprintf( buf, "{k%s wykrzykuje do ciebie '{K%s{k'{x\n\r", PERS( ch, victim ), sbuf );
            }
            else if (ask)
            {
                sprintf( buf, "{k%s pyta siê ciebie '{K%s{k'{x\n\r", PERS( ch, victim ), sbuf );
            }
            else
            {
                sprintf( buf, "{k%s mówi ci '{K%s{k'{x\n\r", PERS( ch, victim ), sbuf );
            }
        }
        sprintf( buf, "%s", capitalize( buf ) );
        add_buf( victim->pcdata->buffer, buf );
        return;
    }
    if ( !( IS_IMMORTAL( ch ) && ch->level > LEVEL_IMMORTAL ) && !IS_AWAKE( victim ) )
    {
        send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
        return;
    }
    if ( ( IS_SET( victim->comm, COMM_QUIET ) || IS_SET( victim->comm, COMM_DEAF ) )
            && !IS_IMMORTAL( ch ) )
    {
        send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
        return;
    }
    if ( IS_SET( victim->comm, COMM_AFK ) )
    {
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
            return;
        }
        send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
        if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
                !IS_NPC( victim ) && !IS_IMMORTAL( victim ) &&
                IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
        {
            if ( scream )
            {
                sprintf( buf, "{kNie¶miertelny krzyczy do ciebie '{K%s{k'{x\n\r", sbuf );
            }
            else if ( ask )
            {
                sprintf( buf, "{kNie¶miertelny pyta siê ciebie '{K%s{k'{x\n\r", sbuf );
            }
            else
            {
                sprintf( buf, "{kNie¶miertelny mówi ci '{K%s{k'{x\n\r", sbuf );
            }
        }
        else
        {
            if ( !str_cmp ( race_table[ GET_RACE( ch ) ].name , "gnom" ) &&
                    str_cmp ( race_table[ GET_RACE( victim ) ].name , "gnom" ) )
                sbuf = translate_gnomish ( sbuf );
            if ( scream )
            {
                sprintf( buf, "{k%s krzyczy do ciebie '{K%s{k'{x\n\r", PERS( ch, victim ), sbuf );
            }
            else if ( ask )
            {
                sprintf( buf, "{k%s pyta siê ciebie '{K%s{k'{x\n\r", PERS( ch, victim ), sbuf );
            }
            else
            {
                sprintf( buf, "{k%s mówi ci '{K%s{k'{x\n\r", PERS( ch, victim ), sbuf );
            }
        }
        sprintf( buf, "%s", capitalize( buf ) );
        add_buf( victim->pcdata->buffer, buf );
        return;
    }
    if ( scream )
    {
        act( "{kKrzyczysz $X '{K$t{k'{x", ch, argument, victim, TO_CHAR );
    }
    else if ( ask )
    {
        act( "{kpytasz $X '{K$t{k'{x", ch, argument, victim, TO_CHAR );
    }
    else
    {
        act( "{kMówisz $X '{K$t{k'{x", ch, argument, victim, TO_CHAR );
    }
    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
            !IS_NPC( victim ) && !IS_IMMORTAL( victim ) &&
            IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
    {
        if ( scream )
        {
            act_new( "{kNie¶miertelny krzyczy do ciebie '{K$t{k'{x", ch, sbuf, victim, TO_VICT, POS_DEAD );
        }
        else if ( ask )
        {
            act_new( "{kNie¶miertelny pyta siê ciebie '{K$t{k'{x", ch, sbuf, victim, TO_VICT, POS_DEAD );
        }
        else
        {
            act_new( "{kNie¶miertelny mówi ci '{K$t{k'{x", ch, sbuf, victim, TO_VICT, POS_DEAD );
        }
    }
    else
    {
        if ( !str_cmp ( race_table[ GET_RACE( ch ) ].name , "gnom" ) &&
                str_cmp ( race_table[ GET_RACE( victim ) ].name , "gnom" ) )
            sbuf = translate_gnomish ( sbuf );
        if ( scream )
        {
            act( "{k$n krzyczy do ciebie '{K$t{k'{x", ch, sbuf, victim, TO_VICT );
        }
        else if ( scream )
        {
            act( "{k$n pyta siê ciebie '{K$t{k'{x", ch, sbuf, victim, TO_VICT );
        }
        else
        {
            act( "{k$n mówi ci '{K$t{k'{x", ch, sbuf, victim, TO_VICT );
        }
    }
    if ( ( IS_NPC( ch ) || ( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) ) ) && ch->in_room == victim->in_room )
    {
        if ( scream )
        {
            act( "$n krzyczy co¶ do $Z.", ch, NULL, victim, TO_NOTVICT );
        }
        else if ( ask )
        {
            act( "$n pyta siê $Z.", ch, NULL, victim, TO_NOTVICT );
        }
        else
        {
            act( "$n mówi co¶ do $Z.", ch, NULL, victim, TO_NOTVICT );
        }
    }
    victim->reply	= ch;
    mdat = get_mob_memdat( victim, ch, NULL, MEMDAT_RESPONSEDELAY );
    if ( !IS_NPC( ch ) && IS_NPC( victim ) && mdat && mdat->response )
    {
        response = 0;
        ri = 0;
        sbuf = mdat->response;
        for (;; )
        {
            ri++;
            sbuf = one_argument( sbuf, buf );
            if ( !str_cmp( argument, buf ) )
            {
                response = ri;
                break;
            }
            if ( sbuf[ 0 ] == '\0' )
                break;
        }
        if ( response > 0 )
        {
            victim->prog_response = response;
            add_prog_env( mdat->prog_env );
            program_flow();
            free_mdat( mdat );
            victim->prog_response = 0;
        }
        return;
    }
    if ( !IS_NPC( ch ) && IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_TELL ) )
    {
        mp_act_trigger( argument, victim, ch, NULL, NULL, &TRIG_TELL );
    }
    return;
}
/* do_reply */
void do_reply( CHAR_DATA * ch, char * argument )
{
    CHAR_DATA * victim;
    MEMDAT * mdat;
    char buf[ MAX_STRING_LENGTH ];
    int speaking;
    int speakswell;
    char *sbuf;
    int ri, response;

    if ( ! can_speak( ch, argument, SPEAK_REPLY ) )
    {
        return;
    }
    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
        return;
    }
    if ( IS_SET( ch->comm, COMM_NOTELL ) )
    {
        send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
        return;
    }
    if ( ( victim = ch->reply ) == NULL || !can_see( ch, victim ) )
    {
        send_to_char( "Zastanawiasz siê komu mia³<&e/a/o>¶ odpowiedzieæ.\n\r", ch );
        return;
    }
    if ( IS_NPC( victim ) && victim->in_room != ch->in_room )
    {
        send_to_char( "Zastanawiasz siê komu mia³<&e/a/o>¶ odpowiedzieæ.\n\r", ch );
        return;
    }
    if ( is_ignoring( victim, ch->name ) )
        return;
    if ( IS_AFFECTED( victim, AFF_DEAFNESS ) || IS_AFFECTED( victim, AFF_FORCE_FIELD ) )
    {
        send_to_char( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch );
        return;
    }
    sbuf = argument;
    speaking = ch->speaking;
    speakswell = UMIN( knows_language( victim, ch->speaking, ch ),
                       knows_language( ch, ch->speaking, victim ) );
    if ( speakswell < 95 )
        sbuf = translate( speakswell, argument, lang_table[ speaking ].name );
    if ( !IS_NPC( ch ) && ch->level < 32 )
        argument = rpg_control( argument );
    argument = strip_dupspaces ( argument );
    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Co chcesz odpowiedzieæ?\n\r", ch );
        return;
    }
    if ( victim->desc == NULL && !IS_NPC( victim ) )
    {
        act( "$N ma zerwane po³±czenie...spróbuj pó¼niej.",
             ch, NULL, victim, TO_CHAR );
        if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
                !IS_NPC( victim ) && !IS_IMMORTAL( victim ) &&
                IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
            sprintf( buf, "{lNie¶miertelny mówi ci '{L%s{l'{x\n\r", sbuf );
        else
        {
            if ( !str_cmp ( race_table[ GET_RACE( ch ) ].name , "gnom" ) &&
                    str_cmp ( race_table[ GET_RACE( victim ) ].name , "gnom" ) )
                sbuf = translate_gnomish ( sbuf );
            sprintf( buf, "{l%s mówi ci '{L%s{l'{x\n\r", PERS( ch, victim ), sbuf );
        }
        sprintf( buf, "%s", capitalize( buf ) );
        add_buf( victim->pcdata->buffer, buf );
        return;
    }
    if ( !IS_IMMORTAL( ch ) && !IS_AWAKE( victim ) )
    {
        act( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch, 0, victim, TO_CHAR );
        return;
    }
    if ( ( IS_SET( victim->comm, COMM_QUIET ) || IS_SET( victim->comm, COMM_DEAF ) )
            && !IS_IMMORTAL( ch ) && !IS_IMMORTAL( victim ) )
    {
        act_new( "Twoja wiadomo¶æ nie dotar³a.\n\r", ch, 0, victim, TO_CHAR, POS_DEAD );
        return;
    }
    if ( !IS_IMMORTAL( victim ) && !IS_AWAKE( ch ) )
    {
        send_to_char( "W snach, czy co?\n\r", ch );
        return;
    }
    if ( IS_SET( victim->comm, COMM_AFK ) )
    {
        if ( IS_NPC( victim ) )
        {
            act_new( "Twoja wiadomo¶æ nie dotar³a.", ch, NULL, victim, TO_CHAR, POS_DEAD );
            return;
        }
        act_new( "$E jest AFK, ale twoja wiadomo¶æ dojdzie kiedy $E wróci.",
                 ch, NULL, victim, TO_CHAR, POS_DEAD );
        if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
                !IS_NPC( victim ) && !IS_IMMORTAL( victim ) &&
                IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
            sprintf( buf, "{lNie¶miertelny mówi ci '{L%s{l'{x\n\r", sbuf );
        else
        {
            if ( !str_cmp ( race_table[ GET_RACE( ch ) ].name , "gnom" ) &&
                    str_cmp ( race_table[ GET_RACE( victim ) ].name , "gnom" ) )
                sbuf = translate_gnomish ( sbuf );
            sprintf( buf, "{l%s mówi ci '{L%s{l'{x\n\r", PERS( ch, victim ), sbuf );
        }
        sprintf( buf, "%s", capitalize( buf ) );
        add_buf( victim->pcdata->buffer, buf );
        return;
    }
    /**
     * log it
     */
    if ( !IS_NPC( ch ) )
    {
        append_file_format_daily( ch, SPEECH_LOG_FILE, "reply %s: %s", victim->name, argument );
    }
    act( "{lMówisz $X '{L$t{l'{x", ch, argument, victim, TO_CHAR );
    if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
            !IS_NPC( victim ) && !IS_IMMORTAL( victim ) &&
            IS_SET( ch->wiz_config, WIZCFG_WIZSPEECH ) )
        act_new( "{lNie¶miertelny mówi ci '{L$t{l'{x", ch, sbuf, victim, TO_VICT, POS_DEAD );
    else
    {
        if ( !str_cmp ( race_table[ GET_RACE( ch ) ].name , "gnom" ) &&
                str_cmp ( race_table[ GET_RACE( victim ) ].name , "gnom" ) )
            sbuf = translate_gnomish ( sbuf );
        act( "{l$n mówi ci '{L$t{l'{x", ch, sbuf, victim, TO_VICT );
    }
    if ( ( IS_NPC( ch ) || ( !IS_NPC( ch ) && !IS_IMMORTAL( ch ) ) ) && ch->in_room == victim->in_room )
        act( "$n mówi co¶ do $Z.", ch, NULL, victim, TO_NOTVICT );
    victim->reply	= ch;
    mdat = get_mob_memdat( victim, ch, NULL, MEMDAT_RESPONSEDELAY );
    if ( !IS_NPC( ch ) && IS_NPC( victim ) && mdat && mdat->response )
    {
        response = 0;
        ri = 0;
        sbuf = mdat->response;
        for (;; )
        {
            ri++;
            sbuf = one_argument( sbuf, buf );
            if ( !str_cmp( argument, buf ) )
            {
                response = ri;
                break;
            }
            if ( sbuf[ 0 ] == '\0' )
                break;
        }
        if ( response > 0 )
        {
            victim->prog_response = response;
            add_prog_env( mdat->prog_env );
            program_flow();
            free_mdat( mdat );
            victim->prog_response = 0;
        }
        return;
    }
    if ( !IS_NPC( ch ) && IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_TELL ) )
        mp_act_trigger( argument, victim, ch, NULL, NULL, &TRIG_TELL );
    return;
}
/* do_yell */
void do_yell( CHAR_DATA * ch, char * argument )
{
    DESCRIPTOR_DATA * d;
    int speaking;
    int speakswell;
    char *sbuf;
    AFFECT_DATA af, *aff;

    if ( ! can_speak( ch, argument, SPEAK_YELL ) )
    {
        return;
    }
    if ( IS_SET( ch->comm, COMM_NOSHOUT ) && !IS_NPC( ch ) )
    {
        send_to_char( "Nie mo¿esz wrzeszczeæ.\n\r", ch );
        return;
    }
    if ( IS_SET( race_table[ GET_RACE( ch ) ].type , ANIMAL ) )
    {
        return;
    }
    if ( IS_SET( race_table[ GET_RACE( ch ) ].type , INSECT ) )
    {
        return;
    }
    aff = affect_find( ch->affected, gsn_yell );
    if ( aff && aff->duration > 2 && !IS_IMMORTAL( ch ) && !is_undead( ch ) )
    {
        send_to_char( "Za bardzo zdar³<&e/a/o>¶ gard³o krzycz±c. Posiedz chwilê cicho.\n\r", ch );
        return;
    }
    if ( !str_infix( "immo", argument ) || !str_infix( "niesmiertelny", argument ) || !str_infix( "bogowie", argument ) || !str_infix( "lord", argument ) )
    {
        print_char( ch, "W celu skontaktowania siê z nie¶miertelnymi prosimy korzystaæ z adresu e-mail:\n\r%s\n\r", MAIL_ADDRESS );
        return;
    }
    if ( !IS_NPC( ch ) && ch->level < 32 )
        argument = rpg_control( argument );
    argument = strip_dupspaces ( argument );
    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Co chcesz wrzasn±æ?\n\r", ch );
        return;
    }
    /**
     * log it
     */
    if ( !IS_NPC( ch ) )
    {
        append_file_format_daily( ch, SPEECH_LOG_FILE, "yell: %s", argument );
    }
    act( "{uWrzeszczysz '{U$t{u'{x", ch, argument, NULL, TO_CHAR );
    if ( !IS_IMMORTAL( ch ) && !is_undead( ch ) )
    {
        if ( !aff )
        {
            af.where	= TO_AFFECTS;
            af.type	= gsn_yell;
            af.level	= 50; //by nie szlo dispelnac
            af.duration = 1;
            af.rt_duration = 0;
            af.location = APPLY_NONE;
            af.modifier = 0;
            af.bitvector = &AFF_NONE;
            af.visible	= FALSE;
            affect_to_char( ch, &af, NULL, FALSE );
        }
        else
            ++aff->duration;
    }
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( !d->character || d->connected < 0 ) continue;
        sbuf = argument;
        if ( d->connected == CON_PLAYING
                && d->character != ch
                && d->character->in_room != NULL
                && SAME_AREA( d->character->in_room->area, ch->in_room->area )
                && SAME_AREA_PART( d->character, ch )
                && !IS_SET( d->character->comm, COMM_QUIET )
                && !IS_AFFECTED( d->character, AFF_DEAFNESS )
                && !IS_AFFECTED( d->character, AFF_FORCE_FIELD ) &&
                ( !IS_IMMORTAL( d->character ) ||
                  ( IS_IMMORTAL( d->character ) && !IS_SET( d->character->wiz_config, WIZCFG_SHOWYELLS ) ) ) )
        {
            speaking = ch->speaking;
            speakswell = UMIN( knows_language( d->character, ch->speaking, ch ),
                               knows_language( ch, ch->speaking, d->character ) );
            if ( speakswell < 95 )
                sbuf = translate( speakswell, argument, lang_table[ speaking ].name );
            if ( !is_ignoring( d->character, ch->name ) )
            {
                if ( !IS_NPC( ch ) && IS_IMMORTAL( ch ) &&
                        !IS_NPC( d->character ) && !IS_IMMORTAL( d->character ) )
                {
                    act( "{uS³yszysz w g³owie grzmi±cy g³os Nie¶miertelnego mowi±cy '{U$t{u'{x", ch, sbuf, d->character, TO_VICT );
                }
                else
                {
                    if ( !str_cmp ( race_table[ GET_RACE( ch ) ].name , "gnom" ) &&
                            str_cmp ( race_table[ GET_RACE( d->character ) ].name , "gnom" ) )
                        sbuf = translate_gnomish ( sbuf );
                    act( "{u$n wrzeszczy '{U$t{u'{x", ch, sbuf, d->character, TO_VICT );
                }
            }
        }
        if ( d->connected == CON_PLAYING &&
                d->character != ch &&
                d->character->in_room != NULL &&
                IS_IMMORTAL( d->character ) &&
                IS_SET( d->character->wiz_config, WIZCFG_SHOWYELLS ) )
        {
            if ( !is_ignoring( d->character, ch->name ) )
                print_char( d->character, "{U[%5d]{x {u%s wrzeszczy '{U%s{u'{x\n\r", ch->in_room ? ch->in_room->vnum : 0, IS_NPC( ch ) ? capitalize( ch->short_descr ) : ch->name, sbuf );
        }
    }
    return;
}
/* do_emote */
void do_emote( CHAR_DATA * ch, char * argument )
{
    if ( !IS_NPC( ch ) )
    {
        do_function( ch, &do_help, "emote" );
        return;
    }
    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Co chcesz pokazaæ?\n\r", ch );
        return;
    }
    MOBtrigger = FALSE;
    if ( IS_NPC( ch ) )
    {
        act( "$n $T", ch, NULL, argument, TO_ROOM );
        act( "$n $T", ch, NULL, argument, TO_CHAR );
    }
    else
    {
        act( "*$n $T", ch, NULL, argument, TO_ROOM );
        act( "*$n $T", ch, NULL, argument, TO_CHAR );
    }
    MOBtrigger = TRUE;
    return;
}
/* do_pmote */
void do_pmote( CHAR_DATA * ch, char * argument )
{
    CHAR_DATA * vch;
    char *letter, *name;
    char last[ MAX_INPUT_LENGTH ], temp[ MAX_STRING_LENGTH ];
    int matches = 0;
    if ( !IS_NPC( ch ) && IS_SET( ch->comm, COMM_NOEMOTE ) )
    {
        send_to_char( "Nie mo¿esz okazywaæ emocji.\n\r", ch );
        return;
    }
    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Jaka to ma byæ emocja?\n\r", ch );
        return;
    }
    act( "*$n $t", ch, argument, NULL, TO_CHAR );
    for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room )
    {
        if ( vch->desc == NULL || vch == ch )
            continue;
        if ( ( letter = strstr( argument, vch->name ) ) == NULL )
        {
            MOBtrigger = FALSE;
            act( "*$N $t", vch, argument, ch, TO_CHAR );
            MOBtrigger = TRUE;
            continue;
        }
        strcpy( temp, argument );
        temp[ strlen( argument ) - strlen( letter ) ] = '\0';
        last[ 0 ] = '\0';
        name = vch->name;
        for (; *letter != '\0'; letter++ )
        {
            if ( *letter == '\'' && matches == strlen( vch->name ) )
            {
                strcat( temp, "r" );
                continue;
            }
            if ( *letter == 's' && matches == strlen( vch->name ) )
            {
                matches = 0;
                continue;
            }
            if ( matches == strlen( vch->name ) )
            {
                matches = 0;
            }
            if ( *letter == *name )
            {
                matches++;
                name++;
                if ( matches == strlen( vch->name ) )
                {
                    strcat( temp, "you" );
                    last[ 0 ] = '\0';
                    name = vch->name;
                    continue;
                }
                strncat( last, letter, 1 );
                continue;
            }
            matches = 0;
            strcat( temp, last );
            strncat( temp, letter, 1 );
            last[ 0 ] = '\0';
            name = vch->name;
        }
        MOBtrigger = FALSE;
        act( "*$N $t", vch, temp, ch, TO_CHAR );
        MOBtrigger = TRUE;
    }
    return;
}
/*
 * All the posing stuff.
 */
void do_bug( CHAR_DATA * ch, char * argument )
{
    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Jaki b³±d chcesz zg³osiæ?\n\r", ch );
    }
    else
    {
        append_file( ch, BUG_FILE, argument );
        send_to_char( "{RB³±d zanotowany{x - {Gdziêkujemy{x.\n\r", ch );
    }
    send_to_char( "Pamiêtaj:\n\r"
                  "Do zg³aszania b³êdów w opisach s³u¿y komenda TYPO.\n\r"
                  "Do zg³aszania pozosta³ych b³êdów s³u¿y komenda BUG.\n\r"
                  "Do zg³aszania pomys³ów s³u¿y komenda IDEA.\n\r", ch );
    return;
}
void do_typ( CHAR_DATA * ch, char * argument )
{
    send_to_char( "Je¿li chcesz zg³osiæ typo, napisz ca³y wyraz \"{Gtypo{x\".\n\r", ch );
    return;
}
void do_typo( CHAR_DATA * ch, char * argument )
{
    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Jaki typo chcesz zg³osiæ?\n\r", ch );
    }
    else
    {
        append_file( ch, TYPO_FILE, argument );
        send_to_char( "{RLiterówka zanotowana{x - {Gdziêkujemy{x.\n\r", ch );
    }
    send_to_char( "Pamiêtaj:\n\r"
                  "Do zg³aszania b³êdów w opisach s³u¿y komenda TYPO.\n\r"
                  "Do zg³aszania pozosta³ych b³êdów s³u¿y komenda BUG.\n\r"
                  "Do zg³aszania pomys³ów s³u¿y komenda IDEA.\n\r", ch );
    return;
}
void do_ide( CHAR_DATA * ch, char * argument )
{
    send_to_char( "Wpisz pe³n± komendê IDEA je¶li chcesz zg³osiæ pomys³.\n\r", ch );
    return;
}
void do_idea( CHAR_DATA * ch, char * argument )
{
    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Jaki pomys³ chcesz zg³osiæ?\n\r", ch );
    }
    else
    {
        append_file( ch, IDEA_FILE, argument );
        send_to_char( "{RPomys³ zanotowany{x - {Gdziêkujemy{x.\n\r", ch );
    }
    send_to_char( "Pamiêtaj:\n\r"
                  "Do zg³aszania b³êdów w opisach s³u¿y komenda TYPO.\n\r"
                  "Do zg³aszania pozosta³ych b³êdów s³u¿y komenda BUG.\n\r"
                  "Do zg³aszania pomys³ów s³u¿y komenda IDEA.\n\r", ch );
    return;
}

/* wylicza efektywny rent rate dla gracza */
int get_rent_rate( CHAR_DATA * ch )
{
    ROOM_INDEX_DATA *pRoomIndex;
    int rent_rate = 100;

    if ( ch->in_room && EXT_IS_SET( ch->in_room->room_flags, ROOM_INN ) )
      rent_rate = ch->in_room->rent_rate;
    else
    {
        if ( !IS_IMMORTAL( ch ) && ch->pcdata->last_rent > 0 )
        {
            if ( ( pRoomIndex = get_room_index( ch->pcdata->last_rent ) ) )
                rent_rate = pRoomIndex->rent_rate;
        }
        else
        {
            if ( ( pRoomIndex = get_room_index( 599 ) ) )
                rent_rate = pRoomIndex->rent_rate;
        }
    }

    return rent_rate;

}

/*rekurencyjna funkcja zliczajaca oplate przy rent*/
int get_rent_cost( CHAR_DATA * ch, OBJ_DATA * object, bool show_item )
{
    OBJ_DATA * obj, *obj_next = NULL;
    char blah[ MAX_INPUT_LENGTH ];
    int cost = 0, obj_cost = 0;
    int rent_rate = get_rent_rate( ch );

    for ( obj = object; obj != NULL; obj = obj_next )
    {
        OBJ_NEXT_CONTENT( obj, obj_next );
        if
            (
             IS_OBJ_STAT( obj, ITEM_NO_RENT )
             || obj->item_type == ITEM_KEY
             || obj->item_type == ITEM_CORPSE_NPC
             || ( obj->item_type == ITEM_MAP && !obj->value[ 0 ] )
            )
        {
            if ( show_item )
            {
                sprintf( blah, "[%-*s] : pozb±d¼ siê tego je¶li chcesz wynaj±æ pokój.\n\r", 30+count_colors(obj->short_descr,MAX_INPUT_LENGTH), obj->short_descr );
                send_to_char( blah, ch );
            }
            obj_cost = 0;
        }
        else
        {
            obj_cost = obj->rent_cost;
            if ( obj->wear_loc == WEAR_NONE )
            {
                obj_cost *= 3;
                obj_cost /= 2;
            }
            if ( IS_OBJ_STAT( obj, ITEM_HIGH_RENT ) )
            {
                obj_cost *= 2;
            }
            obj_cost = UMAX( 1, obj_cost * rent_rate / 100 );
            if ( ( IS_OBJ_STAT( obj, ITEM_NEWBIE_FREE_RENT ) && ch->level <= LEVEL_NEWBIE ) || IS_OBJ_STAT( obj, ITEM_FREE_RENT ) )
            {
                obj_cost = 0;
            }
            if ( show_item )
            {
                if ( obj_cost > 0 )
                {
                    sprintf( blah, "[%-*s] : %s\n\r", 30+count_colors(obj->short_descr,MAX_INPUT_LENGTH), obj->short_descr, money_string( obj_cost, TRUE ) );
                }
                else
                {
                    sprintf( blah, "[%-*s] : karczmarz przechowa ci to bez op³aty.\n\r", 30+count_colors(obj->short_descr,MAX_INPUT_LENGTH), obj->short_descr );
                }
                send_to_char( blah, ch );
            }
        }
        cost += obj_cost;
        if ( obj->item_type == ITEM_CONTAINER )
        {
            cost += get_rent_cost( ch, obj->contains, show_item );
        }
    }
    return cost;
}

int get_hoard_cost( CHAR_DATA * ch, OBJ_DATA * object, bool show_item )
{
    OBJ_DATA * obj;
    char blah[ MAX_INPUT_LENGTH ];
    int cost = 0, obj_cost = 0;
    int rent_rate = get_rent_rate( ch );
    if ( object )
    {
        if ( show_item )
        {
            sprintf( blah, "\n\rOp³ata za przechowywane rzeczy:\n\r" );
            send_to_char( blah, ch );
        }
    }
    else
    {
        return 0;
    }
    for ( obj = object; obj != NULL; obj = obj->next_content )
    {
        if ( obj->rent_cost > 0 )
        {
            obj_cost = ( 3 * obj->rent_cost * HOARD_RENT_RATE / 200 );
        }
        else
        {
             obj_cost = UMAX( 3 * obj->cost * HOARD_RENT_RATE / 800, 1 );
        }
        if ( IS_OBJ_STAT( obj, ITEM_HIGH_RENT ) )
        {
            obj_cost *= 2;
        }
        obj_cost = UMAX( 1, obj_cost * rent_rate / 100 );
        if ( ( IS_OBJ_STAT( obj, ITEM_NEWBIE_FREE_RENT ) && ch->level <= LEVEL_NEWBIE ) || IS_OBJ_STAT( obj, ITEM_FREE_RENT ) )
        {
            obj_cost = 0;
            if ( show_item )
            {
                sprintf( blah, "[%-*s] : mo¿esz przechowaæ to bez op³aty.\n\r", 30+count_colors(obj->short_descr,MAX_INPUT_LENGTH), obj->short_descr );
                send_to_char( blah, ch );
            }
        }
        else
        {
            if ( show_item )
            {
                sprintf( blah, "[%-*s] : %s\n\r", 30+count_colors(obj->short_descr,MAX_INPUT_LENGTH), obj->short_descr, money_string( obj_cost, TRUE ) );
                send_to_char( blah, ch );
            }
        }
        cost += obj_cost;
    }
    return cost;
}

/* ta sama rekurencja sprawdzajaca czy moze*/
bool check_rent( CHAR_DATA * ch, OBJ_DATA * object, bool show )
{
    OBJ_DATA * obj, *obj_next = NULL;
    bool test = TRUE;
    for ( obj = object; obj != NULL; obj = obj_next )
    {
        OBJ_NEXT_CONTENT( obj, obj_next );
        if ( obj->item_type == ITEM_KEY
                || ( obj->item_type == ITEM_MAP && !obj->value[ 0 ] )
                || ( obj->item_type == ITEM_CORPSE_PC )
                || ( obj->item_type == ITEM_CORPSE_NPC ) )
        {
            if ( show )
                send_to_char( "Wyrzuæ gdzie¶ niepotrzebne rzeczy.\n\r", ch );
            test = FALSE;
            return test;
        }
        if ( IS_OBJ_STAT( obj, ITEM_NO_RENT ) )
        {
            if ( show )
                send_to_char( "Wyrzuæ niepotrzebne rzeczy.\n\r", ch );
            test = FALSE;
            return test;
        }
        if ( obj->item_type == ITEM_CONTAINER )
            test &= check_rent( ch, obj->contains, show );
    }
    return test;
}
/* wyrzuca na ziemie wszystkie no_renty - do autoquita */
void drop_objects( CHAR_DATA * ch, OBJ_DATA * object, bool auto_rent )
{
    OBJ_DATA * obj, *obj_next = NULL;
    for ( obj = object; obj != NULL; obj = obj_next )
    {
        OBJ_NEXT_CONTENT( obj, obj_next );
        if ( obj->item_type == ITEM_CONTAINER && ( auto_rent || IS_OBJ_STAT( obj, ITEM_NODROP ) ) )
            drop_objects( ch, obj->contains, auto_rent );
        if ( !auto_rent && IS_OBJ_STAT( obj, ITEM_NODROP ) )
            continue;
        if ( !auto_rent
                || obj->item_type == ITEM_KEY
                || ( obj->item_type == ITEM_MAP && !obj->value[ 0 ] )
                || ( obj->item_type == ITEM_CORPSE_PC )
                || ( obj->item_type == ITEM_CORPSE_NPC )
                || IS_OBJ_STAT( obj, ITEM_NO_RENT ) )
        {
            if ( obj->wear_loc != WEAR_NONE )
                unequip_char( ch, obj );
            /*artefact*/
            if ( !IS_NPC( ch ) && is_artefact( obj ) )
                artefact_from_char( obj, ch );
            if ( obj->in_obj )
                obj_from_obj( obj );
            obj_from_char( obj );
            act( "$p upada na ziemie.", ch, obj, NULL, TO_ROOM );
            obj_to_room( obj, ch->in_room );
            if ( IS_OBJ_STAT( obj, ITEM_MELT_DROP ) )
            {
                switch ( number_range( 1, 3 ) )
                {
                case 1:
                    act( "$p rozp³ywa siê w powietrzu.", ch, obj, NULL, TO_ROOM );
                    break;
                case 2:
                    act( "$p znika bez ¶ladu.", ch, obj, NULL, TO_ROOM );
                    break;
                default:
                    act( "$p rozp³ywa siê pozostawiaj±c tylko chmurê dymu.", ch, obj, NULL, TO_ROOM );
                    break;
                }
                /*artefact*/
                if ( is_artefact( obj ) ) extract_artefact( obj );
                if ( obj->contains ) extract_artefact_container( obj );
                extract_obj( obj );
            }
            else if ( obj->item_type == ITEM_POTION && ( obj->material == 11 || obj->material == 15 ) )
            {
                switch ( number_range( 1, 3 ) )
                {
                case 1:
                    act( "$p toczy siê przez chwilê, po czym rozpryskujê siê na kwa³eczki.", ch, obj, NULL, TO_ROOM );
                    break;
                case 2:
                    act( "$p pêka z hukiem i rozpryskuje siê dooko³a.", ch, obj, NULL, TO_ROOM );
                    break;
                default:
                    act( "$p rozpryskuje siê na kawa³eczki.", ch, obj, NULL, TO_ROOM );
                    break;
                }
                /*artefact*/
                if ( is_artefact( obj ) ) extract_artefact( obj );
                if ( obj->contains ) extract_artefact_container( obj );
                extract_obj( obj );
            }
        }
    }
}
/* do_offer */
void do_offer( CHAR_DATA * ch, char * argument )
{
    int cost = 0;
    int days, hours, copper;
    char blah[ MAX_INPUT_LENGTH ];

    if ( IS_NPC( ch ) )
    {
        return;
    }

    if ( ch->position == POS_SLEEPING )
    {
        send_to_char( "¦ni ci siê du¿a gospoda z wygodnym pos³aniem.\n\r", ch );
        return;
    }

    if ( ch->position <= POS_INCAP )
    {
        send_to_char ( "Bogowie wysluchaja cie za darmo.\n\r", ch );
        return;
    }

    if ( !EXT_IS_SET( ch->in_room->room_flags, ROOM_INN ) )
    {
        send_to_char( "Lepiej udaj siê w tym celu do gospody.\n\r", ch );
        return;
    }

    cost = get_rent_cost( ch, ch->carrying, TRUE );
    cost += get_hoard_cost( ch, ch->hoard, TRUE );

    if ( cost > 0 )
    {
        copper = money_count_copper_all ( ch );
        days = copper / cost;
        hours = 24 * ( copper % cost ) / cost;
        sprintf( blah, "\n\rRazem: %s na jeden dzieñ.\n\r", money_string( cost, TRUE ) );
        send_to_char( blah, ch );
        if ( days >= 1 && hours > 0 )
        {
            print_char
                (
                 ch,
                 "Pieniêdzy starczy ci na %d %s i %d %s.\n\r",
                 days,
                 days == 1 ? "dzieñ" : "dni",
                 hours,
                 hour_to_str( hours )
                );
        }
        else if ( days >= 1 && hours == 0 )
        {
            print_char( ch, "Pieniêdzy starczy ci na %d %s.\n\r",
                        days,
                        days == 1 ? "dzieñ" : "dni" );
        }
        else if ( days == 0 && hours >= 1 )
        {
            print_char( ch, "Pieniêdzy starczy ci tylko na %d %s.\n\r",
                        hours,
                        hour_to_str( hours ) );
        }
        else
        {
            print_char( ch, "Pieniêdzy nie starczy ci nawet na godzinê.\n\r" );
        }
    }
    else
    {
        send_to_char( "Nie masz nic warto¶ciowego.\n\r", ch );
        return;
    }
    return;
}
/* do_rent */
void do_rent( CHAR_DATA * ch, char * argument )
{
    CHAR_DATA * wch;
    int cost = 0;
    int days, hours, copper;
    char buf[ MAX_INPUT_LENGTH ];

    /**
     * thx to NPC
     */
    if ( IS_NPC( ch ) )
    {
        return;
    }

    /**
     * hey dude, be ready
     */
    if ( ch->position == POS_SLEEPING )
    {
        send_to_char( "¦ni ci siê du¿a gospoda z wygodnym pos³aniem.\n\r", ch );
        return;
    }
    if ( ch->position <= POS_INCAP )
    {
        send_to_char( "Widzisz swiatlo na koncu tunelu...\n\r", ch );
        return;
    }
    if ( ch->fighting )
    {
        send_to_char( "Nie poddawaj siê, walcz!\n\r", ch );
        return;
    }
    if ( !EXT_IS_SET( ch->in_room->room_flags, ROOM_INN ) )
    {
        send_to_char( "Lepiej udaj sie w tym celu do gospody.\n\r", ch );
        return;
    }
    if ( ch->position != POS_STANDING )
    {
        send_to_char( "Jak chcesz wynaj±æ pokój? Mo¿e najpierw wstañ?\n\r", ch );
        return;
    }
    if ( ch->mount )
    {
        send_to_char( "Dla swojego wierzchowca te¿ chcesz wynaj±æ pokój?\n\r", ch );
        return;
    }
    if ( ch->level < 2 )
    {
        send_to_char( "Wychodzenie teraz nie jest najlepszym pomys³em, zrób chocia¿ drugi poziom.\n\r", ch );
        return;
    }
    /**
     * do only "set my rent here"
     */
    if ( argument[ 0 ] != '\0' )
    {
        if ( !str_cmp( argument, "here" ) || !str_cmp( argument, "tutaj" ))
        {
            ch->pcdata->last_rent = ch->in_room->vnum;
            switch ( ch->sex )
            {
            case SEX_MALE:
                send_to_char( "Zameldowa³e¶ siê i nastêpny raz bêdziesz startowa³ z tego miejsca.\n\r", ch );
                break;
            case SEX_FEMALE:
                send_to_char( "Zameldowa³a¶ siê i nastêpny raz bêdziesz startowa³a z tego miejsca.\n\r", ch );
                break;
            case SEX_NEUTRAL:
                send_to_char( "Zameldowa³o¶ siê i nastêpny raz bêdziesz startowa³o z tego miejsca.\n\r", ch );
                break;
            }
            sprintf( log_buf, "[%d] %s set new rent (%s).",
                     ch->in_room->vnum,
                     ch->name,
                     ch->in_room->name
                   );
            log_string( log_buf );
            return;
        }
    }
    /**
     * do_rent
     */
    if ( IS_IMMORTAL( ch ) )
    {
        do_function( ch , &do_quit2menu, "auto" );
        return;
    }
    if ( !check_rent( ch, ch->carrying, TRUE ) )
    {
        return;
    }
    cost = get_rent_cost( ch, ch->carrying, TRUE );
    cost += get_hoard_cost( ch, ch->hoard, TRUE );

    if ( cost > 0 )    /*czyli co¶ tam nosi*/
    {
        copper = money_count_copper_all ( ch );
        days = copper / cost;
        hours = 24 * ( copper % cost ) / cost;
        sprintf( buf, "\n\r           Razem: %s na dzieñ.\n\r", money_string( cost, TRUE ) );
        send_to_char( buf, ch );
        if ( days >= 1 && hours > 0 )
            print_char( ch, "Pieniêdzy starczy ci na %d %s i %d %s.\n\r",
                        days,
                        days == 1 ? "dzieñ" : "dni",
                        hours,
                        hour_to_str( hours ) );
        else if ( days >= 1 && hours == 0 )
            print_char( ch, "Pieniêdzy starczy ci na %d %s.\n\r",
                        days,
                        days == 1 ? "dzieñ" : "dni" );
        else if ( days == 0 && hours >= 1 )
            print_char( ch, "Pieniêdzy starczy ci tylko na %d %s.\n\r",
                        hours,
                        hour_to_str( hours ) );
        else
            print_char( ch, "Pieniêdzy nie starczy ci nawet na godzinê.\n\r" );
    }
    if ( money_count_copper_all( ch ) < cost )
    {
        send_to_char( "Nie staæ ciê na pokój.\n\r", ch );
        return;
    }
    /**
     * stripujemy invisa
     */
    strip_invis( ch, TRUE, TRUE );
    /*nie pobiera oplaty... pieniazki traci sie dopiero przy logowaniu
      po obliczeniu oplata*ilosc_dni*/
    send_to_char( "Có¿, wszystko siê kiedy¶ koñczy.\n\r", ch );
    act( "$n wynajmuje pokój.", ch, NULL, NULL, TO_ROOM );
    sprintf( log_buf, "[%d] %s has rent (%s).",
             ch->in_room->vnum,
             ch->name,
             ch->in_room->name
           );
    log_string( log_buf );
    wiznet( "$N przenosi siê do ¶wiata rzeczywistego.", ch, NULL, WIZ_LOGINS, 0, get_trust( ch ) );
    die_follower( ch, TRUE );
    ch->pcdata->last_rent = ch->in_room->vnum;
    save_char_obj( ch, FALSE, FALSE );
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
        if ( wch->reply == ch )
        {
            wch->reply = NULL;
        }
        if ( wch->prog_target == ch )
        {
            wch->prog_target = NULL;
        }
        if ( wch->master == ch )
        {
            wch->master = NULL;
        }
        if ( wch->leader == ch )
        {
            wch->leader = NULL;
        }
    }
    /* won z rooma */
    if ( ch->in_room != NULL )
    {
        char_from_room( ch );
    }
    /* wywalamy z char_list */
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
        if ( prev == NULL )
        {
            bug( "Extract_char: char not found.", 0 );
            return;
        }
    }
    if ( ch->desc == NULL )
    {
        do_quit( ch, "auto" );
        return;
    }
    ch->desc->connected = CON_MENU;
    menu_show( ch );
    return;
}
/* do_qui */
void do_qui( CHAR_DATA * ch, char * argument )
{
    send_to_char( "Je¶li chcesz wyj¶æ z muda wpisz 'quit'.\n\r", ch );
    return;
}
/* do_quit */
void do_quit( CHAR_DATA * ch, char * argument )
{
    if ( ch->position <= POS_STUNNED )
    {
        send_to_char ( "Tylko ¶mieræ jest ostatecznym rozwi±zaniem.\n\r", ch );
        return;
    }
    /* shapeshifting */
    if ( ch->ss_data )
    {
        SHAPESHIFT_DATA* ss;
        ss = do_reform_silent ( ch );
        free_shape_data( ss );
    }
    send_to_char( "No có¿, wszystko siê kiedy¶ koñczy.\n\r", ch );
    sprintf( log_buf, "%s has quit.", ch->name );
    log_string( log_buf );
    if ( !IS_IMMORTAL( ch ) )
    {
        quit_artefact_char( ch );
    }
    /* hmm moze byæ bug z czyszczeniem gratow przy quit*/
    DEBUG_INFO( "extract_char" );
    extract_char( ch, TRUE );
    return;
}

void do_quit2menu( CHAR_DATA * ch, char * argument )
{
    CHAR_DATA *wch;
    AFFECT_DATA *aff, *aff_next;
    int exp_loss;
    if ( IS_NPC( ch ) )
        return;
    if ( !IS_IMMORTAL( ch ) && argument[ 0 ] == '\0' )
    {
        send_to_char( "W naszym ¶wiecie aby nie straciæ przedmiotów miêdzy kolejnymi sesjami grania\n\r"
                      "nale¿y odszukaæ jak±¶ gospodê i wynaj±c pokój. S³u¿y do tego komenda {Grent{x.\n\r"
                      "Je¶li wyjdziesz z gry komend± {Rquit{x stracisz wszystkie przedmioty.\n\r\n\r"
                      "Je¶li jeste¶ pew<&ien/na/ne>, ¿e chcesz u¿yæ tej komendy wpisz: {Rquit yes{x\n\r", ch );
        return;
    }
    if ( ch->fighting )
    {
        send_to_char( "Nie poddawaj siê, walcz!\n\r", ch );
        return;
    }
    if ( ch->mount )
    {
        ch->mount->mounting = NULL;
        ch->mount = NULL;
    }
    if ( ch->position <= POS_STUNNED )
    {
        send_to_char( "Jeszcze nie umar³<&e/a/o>¶.\n\r", ch );
        return;
    }
    /* shapeshifting */
    if ( ch->ss_data )
    {
        SHAPESHIFT_DATA* ss;
        ss = do_reform_silent ( ch );
        free_shape_data( ss );
    }
    /* quit2log */
    sprintf( log_buf, "%s quit to menu.", ch->name );
    log_string( log_buf );
    /*	Opisy do quit yes by Tanon z forum*/
    if ( !IS_IMMORTAL( ch ) && number_range( 1, 3 ) != 1 )
    {
        switch ( ch->class )
        {
        case CLASS_MAG:
            switch ( ch->pcdata->mage_specialist )
            {
            case - 1:  //ogolny
                act( "$n wytwarza niewielk± kulê bia³ego ¶wiat³a, któr± nastêpnie kieruje w stronê swojej twarzy. S³ychaæ ciche skwierczenie. Po chwili $n upada na ziemiê i szybko koñczy swój ¿ywot w konwulsjach, a $s cia³o znika.", ch, NULL, NULL, TO_ROOM );
                break;
            case 0:  //odrzucanie
                act( "$n z rozpacz± wymawia s³owa zaklêcia. Po chwili $s cia³o robi siê ca³e czerwony i upada na ziemiê. Z $s ust cieknie krew. Cia³o dematerializuje siê.", ch, NULL, NULL, TO_ROOM );
                break;
            case 1:  //przemiany
                act( "$n wykrzykuje zaklêcie. Po chwili $s d³oñ zmienia siê w zakrzywione ostrze, które wbija sobie w brzuch i poci±ga w bok wypruwaj±c wnetrzno¶ci. Upada z cichym jêkiem na ziemiê. Po chwili $s pociête cia³o znika.", ch, NULL, NULL, TO_ROOM );
                break;
            case 2:  //przywolania
                act( "$n unosi rêce do góry i wo³a s³owa zaklêci± odchylaj±c siê do ty³u. Po chwili z nieba spada ostrze centralnie przebijaj±c $s szyjê. $n charczy przez chwilê i umiera, a $s cia³o znika w rozb³ysku ¶wiat³a.", ch, NULL, NULL, TO_ROOM );
                break;
            case 3:  //poznanie
                act( "$n przyk³ada rêce do swojego czo³a szepcz±c modlitwê. Po chwili odchyla d³oñ, za któr± ci±gnie siê srebrna niematerialna niæ. $n upada na ziemiê a niæ wzbija siê powoli do nieba. Cia³o dematerializuje siê.", ch, NULL, NULL, TO_ROOM );
                break;
            case 4:  //zauroczenia
                act( "$n wykonuje szybkie koliste ruchy nad g³ow± wymawiaj±c zaklêcie. Nagle pó³materialna lina pojawia siê z nik±d i oplata siê wokó³ $s szyi. Po chwili $n upada na ziemiê, a $s cia³o znika.", ch, NULL, NULL, TO_ROOM );
                break;
            case 5:  //iluzje
                act( "$n bardzo szybko pociera rêce szepczaæ zaklêciê. Po chwili z $s rêki wyrasta pó³materialny sztylet, który szybkim ruchem wbija sobie w g³owê! Strumieñ krwi tryska z drugiej strony, a $n pada na ziemiê. Zmasakrowane cia³o znika po chwili.", ch, NULL, NULL, TO_ROOM );
                break;
            case 6:  //inwokacje
                act( "$n kilkoma ruchami r±k wytwarza niewielk± ¶wiec±c± kulkê, któr± po³yka bez zastanowienia. Po chwili $s g³owa wybucha. Pokrwawiony korpus upada na ziemie i znika.", ch, NULL, NULL, TO_ROOM );
                break;
            case 7:  //nekromancja
                act( "$n wymawia na g³os jak±¶ mroczn± sentencjê. Po chwili z jêkiem upada na ziemie, a $s cia³o b³yskawicznie zaczyna siê rozk³adaæ. Po kilku chwilach pozostaje sam szkielet, a po jeszcze kilku tylko kupka prochu.", ch, NULL, NULL, TO_ROOM );
                break;
            default:
                act( "$n dematerializuje siê powoli.", ch, NULL, NULL, TO_ROOM );
                break;
            }
            break;
        case CLASS_CLERIC:
            act( "$n wznosi rêce do góry b³agaj±c swoich bogów o szybk± ¶mieræ. Po chwili upada na ziemiê. Widzisz jak $s dusza oddziela siê od cia³a i wzlatuje wysoko do góry.", ch, NULL, NULL, TO_ROOM );
            break;
        case CLASS_THIEF:
            act( "$n rozgl±da siê dooko³a i wyciaga rêkê do góry krzycz±c co¶. Po chwili w $s rêku zauwa¿asz sztylet, którym bez zbêdnych ceregieli podrzyna sobie gard³o. $n pada na ziemiê, a $s cia³o po chwili znika.", ch, NULL, NULL, TO_ROOM );
            break;
        case CLASS_WARRIOR:
        case CLASS_BLACK_KNIGHT:
            act( "$n wyci±ga rêkê do góry krzycz±c g³o¶no. W $s rêku pojawia siê pó³materialny krótki miecz, który po chwili wbija sobie w podbrzusze i szarpie w bok. $n zgina siê w pó³, a z rany wy³a¿± mu wnêtrzno¶ci. Po chwili pada na ziemiê, a $s cia³o dematerializuje sie powoli.", ch, NULL, NULL, TO_ROOM );
            break;
        case CLASS_PALADIN:
            act( "$n wznosi do góry z³o¿one rêce b³agaj±c swojego boga o ¶mieræ. Po chwili zaczyna na $s cia³o padaæ coraz to ja¶niejszy snop ¶wiat³a z nieba. Gdy ¶wiat³o blednie $z ju¿ nie ma.", ch, NULL, NULL, TO_ROOM );
            break;
        case CLASS_DRUID:
            act( "$n wykonuje dziwne gesty we wszystkie strony mamrocz±c przy tym zaklêcie. W pewnym momencie ze wszystkich stron zlatuj± siê ogromne szerszenie, które bezlito¶nie k±saj± $c. Po chwili pada $e na ziemiê, a szerszenie odlatuj± tak szybko jak tu przelacia³y, a $s spuchniête cia³o dematerializuje siê powoli i znika.", ch, NULL, NULL, TO_ROOM );
            break;
        case CLASS_BARBARIAN:
            act( "$n warcz±c g³o¶no ³apie siê za g³owê w dwóch miejscach. Po chwili szybkim i zdecydowanym ruchem skrêca sobie kark, zwala siê na ziemiê a $s cia³o powoli dematerializuje siê i znika.", ch, NULL, NULL, TO_ROOM );
            break;
        case CLASS_SHAMAN:
            act( "$n warcz±c g³o¶no wyci±ga piê¶ci ku niebu. Po chwili jakby z nik±d pojawiaj± siê g³odne duchy, zwala na ziemiê $s, po chwili cia³o powoli dematerializuje siê i znika.", ch, NULL, NULL, TO_ROOM );
            break;
        default:
            act( "$n dematerializuje siê powoli.", ch, NULL, NULL, TO_ROOM );
            break;
        }
    }
    else
    {
        if ( IS_GOOD( ch ) )
            act( "$n wznosi rêce do góry prosz±c o ¶mieræ. Nagle mdleje i upada na ziemiê, a z nad cia³a ku niebu wznosi siê dusza $z. Po chwili cia³o równie¿ znika.", ch, NULL, NULL, TO_ROOM );
        else if ( IS_EVIL( ch ) )
            act( "$n krzyczy jakie¶ mroczne s³owa w nieznanym ci jêzyku. Nagle zaczyna wrzeszczeæ z bólu, pada na kolana i staje w ogniu. Po kilku chwilach spala siê doszczêtnie.", ch, NULL, NULL, TO_ROOM );
        else
            act( "$n odchyla sie do ty³u rozwieraj±c rêce na boki i krzyczy prosz±c o ¶mieræ. Po chwili w $c, nie wiadomo sk±d, uderza potê¿ny piorun pozostawiaj±c tylko kupkê popio³u.", ch, NULL, NULL, TO_ROOM );
    }
    clear_mem( ch ); //czyszczonko mema
    //zerujemy affecty
    for ( aff = ch->affected; aff; aff = aff_next )
    {
        aff_next = aff->next;
        if ( check_noremovable_affects( aff ) )
        {
            continue;
        }
        affect_remove( ch, aff );
    }
    /* zdejmujemy graty */
    if ( !IS_IMMORTAL( ch ) )
    {
        ch->hit = 1;
        drop_objects( ch, ch->carrying, FALSE );
        int copper = money_count_copper( ch );
        if ( copper > 0 )
        {
            obj_to_room( create_money( ch->copper, ch->silver, ch->gold, ch->mithril ), ch->in_room );
            append_file_format_daily ( ch, MONEY_LOG_FILE, "-> S: %d - quit yes", copper );
            if ( copper > 1 ) act( "Kupka monet upada na ziemiê.", ch, NULL, NULL, TO_ROOM );
            else act( "Jedna miedziana moneta upada na ziemiê.", ch, NULL, NULL, TO_ROOM );
            money_reset_character_money ( ch );
        }
    }
    //a tu utrata expa, level traci tylko jezeli ma gsn_playerdeath, ale quit yes
    //nie naklada tego gsn.
    update_death_statistic( ch, ch );
    exp_loss = ( number_range( 3, 6 ) * ( exp_per_level( ch, ch->level ) - exp_per_level( ch, ch->level - 1 ) ) ) / 100;
    if ( ch->level < LEVEL_HERO)
    {
        if ( is_affected( ch, gsn_playerdeath ) )
        {
            ch->exp -= exp_loss;
            /**
             * delevel tylko co jakis czas, tak zeby nie bylo
             * to masowo wykorzystywane do zmniejszania sobie
             * poziomu, bo nie po to jest
             */
            if ( !is_affected( ch, gsn_playerquityes ) && exp_per_level( ch, ch->level - 1 ) > ch->exp )
            {
                /**
                 * delevel
                 */
                delevel( ch, FALSE );
                /**
                 * dodaj affect gsn_playerquityes
                 */
                AFFECT_DATA af;
                af.where       = TO_AFFECTS;
                af.type        = gsn_playerquityes;
                af.level       = 0;
                af.duration    = 60;
                af.rt_duration = 4 * af.duration;
                af.bitvector   = &AFF_NONE;
                af.location    = APPLY_NONE;
                af.modifier    = 0;
                affect_to_char( ch, &af, "player use quit yes", FALSE );
            }
        }
        else
        {
            ch->exp = UMAX( exp_per_level( ch, ch->level - 1 ) + 1, ch->exp - exp_loss );
        }
    }
    /* kasujemy follow */
    die_follower( ch, TRUE );
    /* return ze switcha */
    if ( ch->desc != NULL && ch->desc->original != NULL )
    {
        do_function( ch, &do_return, "" );
        ch->desc = NULL;
    }
    /* kasujemy reply i prog_targ */
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
        if ( wch->reply == ch )
            wch->reply = NULL;
        if ( ch->prog_target == wch )
            wch->prog_target = NULL;
    }
    /* save vnumow dla immo*/
    if ( IS_IMMORTAL( ch ) )
        ch->pcdata->last_rent = ch->in_room->vnum;
    save_char_obj( ch, FALSE, FALSE );
    /* won z rooma */
    if ( ch->in_room != NULL )
        char_from_room( ch );
    /* wywalamy z char_list */
    if ( ch == char_list )
        char_list = ch->next;
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
        if ( prev == NULL )
        {
            bug( "Extract_char: char not found.", 0 );
            return;
        }
    }
    if ( ch->desc == NULL )
    {
        do_quit( ch, "auto" );
        return;
    }
    ch->desc->connected = CON_MENU;
    menu_show( ch );
    return;
}
/* extract_dead_player */
void extract_dead_player( CHAR_DATA * ch )
{
    CHAR_DATA * wch;
    if ( ch != NULL )
    {
        if ( IS_NPC( ch ) )
        {
            return;
        }
        /*act("$n opuscil gre.", ch, NULL, NULL, TO_ROOM);*/
        DEBUG_INFO( "extract_dead_player:1" );
        die_follower( ch, TRUE );
        DEBUG_INFO( "extract_dead_player:2" );
        if ( ch->desc != NULL && ch->desc->original != NULL )
        {
            do_function( ch, &do_return, "" );
            ch->desc = NULL;
        }
        DEBUG_INFO( "extract_dead_player:3" );
        for ( wch = char_list; wch != NULL; wch = wch->next )
        {
            if ( wch->hunting == ch )
            {
                wch->hunting = NULL;
            }
            if ( wch->reply == ch )
            {
                wch->reply = NULL;
            }
            if ( ch->prog_target == wch )
            {
                wch->prog_target = NULL;
            }
            forget( wch, ch, 0, TRUE );
        }
        DEBUG_INFO( "extract_dead_player:4" );
        save_char_obj( ch, FALSE, FALSE );
        DEBUG_INFO( "extract_dead_player:5" );
        if ( !ch->desc )
        {
            do_function( ch, &do_quit, "" );
            return;
        }
        DEBUG_INFO( "extract_dead_player:6" );
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
            if ( prev == NULL )
            {
                bug( "Extract_char: char not found.", 0 );
                return;
            }
        }
        DEBUG_INFO( "extract_dead_player:7" );
        if ( ch->in_room != NULL )
        {
            char_from_room( ch );
        }
        DEBUG_INFO( "extract_dead_player:8" );
        ch->desc->connected = CON_MENU;
        DEBUG_INFO( "extract_dead_player:9" );
        menu_show( ch );
    }
    return;
}
/* do_save */
void do_save( CHAR_DATA * ch, char * argument )
{
    CHAR_DATA * victim;
    char arg[ MAX_INPUT_LENGTH ];
    if ( IS_NPC( ch ) )
        return;
    if ( ch->level < 2 )
    {
        send_to_char( "Nie mozna zapisaæ postaci poni¿ej 2 poziomu.\n\r", ch );
        return;
    }
    argument = one_argument( argument, arg );
    if ( arg[ 0 ] != '\0' && IS_IMMORTAL( ch ) )
    {
        if ( !str_cmp( arg, "help" ) )
        {
            send_to_char( "Sk³adnia:\n\r"
                          "save        - zapisuje postaæ która u¿y³a komendy\n\r"
                          "save <imie> - zapisuje gracza o podanym imieniu bêd±cego w grze\n\r"
                          "save all    - zapisuje wszystkich graczy bêd±cych w grze\n\r", ch );
            return;
        }
        if ( !str_cmp( arg, "all" ) )
        {
            DESCRIPTOR_DATA * d;
            for ( d = descriptor_list; d != NULL; d = d->next )
            {
                if ( !d->character || d->connected != CON_PLAYING ) continue;
                victim = d->original ? d->original : d->character;
                if ( victim != NULL )
                    save_char_obj( victim, FALSE, FALSE );
            }
            send_to_char( "Wszyscy gracze zapisani.\n\r", ch );
            save_clans();
            return;
        }
        if ( ( victim = get_char_world( ch, arg ) ) == NULL )
        {
            send_to_char( "Nie ma takiej osoby.\n\r", ch );
            send_to_char( "Sk³adnia:\n\r"
                          "save        - zapisuje postaæ która u¿y³a komendy\n\r"
                          "save <imie> - zapisuje gracza o podanym imieniu bêd±cego w grze\n\r"
                          "save all    - zapisuje wszystkich graczy bêd±cych w grze\n\r", ch );
            return;
        }
        if ( IS_NPC( victim ) )
        {
            send_to_char( "Mo¿na zapisywaæ tylko graczy.\n\r", ch );
            return;
        }
        if ( victim->level < 2 )
        {
            send_to_char( "Nie mozna zapisaæ postaci poni¿ej 2 poziomu.\n\r", ch );
            return;
        }
        save_char_obj( victim, FALSE, FALSE );
        send_to_char( "Podana postaæ zapisana.\n\r", ch );
        return;
    }
    //if ( !IS_IMMORTAL( ch ) )
    //    WAIT_STATE( ch, PULSE_VIOLENCE );
    //save_char_obj( ch, FALSE, FALSE );
    send_to_char( "Zapisywanie...\n\r", ch );
    return;
}
/* do_follow */
void do_follow( CHAR_DATA * ch, char * argument )
{
    /* RT changed to allow unlimited following and follow the NOFOLLOW rules */
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
        send_to_char( "Za kim chcesz chodziæ?\n\r", ch );
        return;
    }
    if ( ch->position == POS_RESTING )
    {
        send_to_char( "Mo¿e jednak najpierw wstañ.\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Nie ma tej osoby tutaj.\n\r", ch );
        return;
    }
    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master != NULL )
    {
        act( "Przecie¿ wolisz chodziæ za $V!", ch, NULL, ch->master, TO_CHAR );
        return;
    }
    if ( victim == ch )
    {
        if ( ch->master == NULL )
        {
            send_to_char( "No taaa, niez³y pomys³.\n\r", ch );
            return;
        }
        stop_follower( ch );
        return;
    }
    if ( !IS_NPC( ch ) )
    {
        if ( IS_NPC( victim ) )
        {
            if (
                (
                    IS_AFFECTED( victim, AFF_SANCTUARY ) ||		// za mobami z sanctuary nie wolno chodziæ
                    victim->pIndexData->pShop != NULL ||		// za sprzedawcami, te¿ nie
                    EXT_IS_SET( victim->act, ACT_PRACTICE )		// no i za nauczycielami te¿ nie, prawda?
                ) && !IS_IMMORTAL( ch )							// no chyba¿e jeste¶ nie¶miertelnym to olewamy regu³y
            )
            {
                switch (victim->sex)
                {
                case SEX_MALE:
                    act( "$N nie chce ¿eby kto¶ za nim chodzi³.", ch, NULL, victim, TO_CHAR );
                    break;
                case SEX_FEMALE:
                    act( "$N nie chce ¿eby kto¶ za ni± chodzi³.", ch, NULL, victim, TO_CHAR );
                    break;
                case SEX_NEUTRAL:
                    act( "$N nie chce ¿eby kto¶ za tym chodzi³.", ch, NULL, victim, TO_CHAR );
                    break;
                }
                return;
            }
        }
        if ( ( EXT_IS_SET( victim->act, ACT_NOFOLLOW ) || EXT_IS_SET( victim->act, PLR_NOFOLLOW ) ) && !IS_IMMORTAL( ch )
           )
        {
            if ( ch->sex == 2 )
            {
                act( "$N nie chce ¿eby kto¶ za ni± chodzi³.", ch, NULL, victim, TO_CHAR );
            }
            else
            {
                act( "$N nie chce ¿eby kto¶ za nim chodzi³.", ch, NULL, victim, TO_CHAR );
            }
            return;
        }
    }
    if ( victim->master == ch )
    {
        act( "Przecie¿ $N chodzi za tob±.\n\r", ch, NULL, victim, TO_CHAR );
        return;
    }
    if ( !IS_NPC( ch ) )
        EXT_REMOVE_BIT( ch->act, PLR_NOFOLLOW );
    if ( ch->master != NULL )
    {
        save_debug_info("act_comm.c => do_follow", NULL, "PRZED is_same_group", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE);
        if ( is_same_group( ch, ch->master ) && is_same_group( ch, victim ) )
        {
            stop_follower( ch );
            add_follower( ch, victim, TRUE );
            send_to_char( "Nadal jeste¶ cz³onkiem grupy.\n\r", ch );
            ch->leader = victim->leader ? victim->leader : victim;
            return;
        }
        save_debug_info("act_comm.c => do_follow", NULL, "za is_same_group", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE);
        stop_follower( ch );
    }
    add_follower( ch, victim, TRUE );
    return;
}
bool check_loop_followers( CHAR_DATA * ch, CHAR_DATA * master )
{
    bool loop_detected = FALSE;
    if ( !master || !ch )
        return FALSE;
    if ( ch == master )
        loop_detected = TRUE;
    if ( master->master )
        loop_detected = check_loop_followers( ch, master->master );
    if ( master->mounting )
        loop_detected = check_loop_followers( ch, master->mounting );
    return loop_detected;
}
/* add_follower */
void add_follower( CHAR_DATA * ch, CHAR_DATA * master, bool strip_weapon_bonus )
{
    if ( ch->master != NULL )
    {
        bug( "Add_follower: non-null master.", 0 );
        return;
    }
    /* sprawdzanie czy ktos za kims nie chodzi */
    if ( check_loop_followers( ch, master ) )
    {
        print_char( ch, "Chodzenie za sob± w kó³eczku nie jest dobrym pomys³em.\n\r" );
        return;
    }
    ch->master = master;
    ch->leader = master;
    if ( strip_weapon_bonus && IS_AFFECTED( ch, AFF_CHARM ) )
    {
        /*
         * 2008-01-22 Gurthg
         * stripujemy bonus do broni :D
         * oraz umagicznienie ataku
         */
        if ( !IS_NPC( master ) && IS_NPC ( ch ) )
        {
            if ( ch->magical_damage > 1 )
            {
                ch->magical_damage = 1;
            }
            ch->weapon_damage_bonus = 0;
        }
    }

    if ( !IS_AFFECTED( ch, AFF_CHARM ) )
    {
        ch->leader = NULL;
    }

    if ( can_see( master, ch ) )
        ch->sex == 2
        ? act( "$n bêdzie teraz chodzi³a za tob±.", ch, NULL, master, TO_VICT )
        : act( "$n bêdzie teraz chodzi³ za tob±.", ch, NULL, master, TO_VICT );
    act( "Bêdziesz teraz chodziæ za $V.", ch, NULL, master, TO_CHAR );
    return;
}
/* stop_follower */
void stop_follower( CHAR_DATA * ch )
{
    if ( ch->master == NULL )
    {
        bug( "Stop_follower: null master.", 0 );
        return;
    }
    if ( ch->in_room && ch->master->in_room && can_see( ch->master, ch ) )
    {
        act( "$n przestaje chodziæ za tob±.", ch, NULL, ch->master, TO_VICT );
        act( "Przestajesz chodziæ za $V.", ch, NULL, ch->master, TO_CHAR );
    }
    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
        AFFECT_DATA * paf, *paf_next;
        for ( paf = ch->affected; paf != NULL; paf = paf_next )
        {
            paf_next = paf->next;
            if ( paf->bitvector == &AFF_CHARM )
            {
                affect_remove( ch, paf );
            }
        }
    }
    ch->master = NULL;
    ch->leader = NULL;
    return;
}
/* die_follower */
void die_follower( CHAR_DATA * ch, bool charm )
{
	//    ROOM_INDEX_DATA * reset_room;
	CHAR_DATA * fch;
	CHARM_DATA *charmed, *charmed_next;
	AFFECT_DATA *paf_next, *paf;
	if ( ch->master != NULL ) stop_follower( ch );
	else return;
	ch->leader = NULL;
	//usuwanie szkieletow i innych cudakow
	if ( charm && !IS_NPC( ch ) && ch->pcdata->charm_list )
	{
		for ( charmed = ch->pcdata->charm_list; charmed; charmed = charmed_next )
		{
			charmed_next = charmed->next;
			DEBUG_INFO( "die_follower:charmies_is_fighting" );
			if ( charmed->victim->fighting )
			{
				DEBUG_INFO( "die_follower:stop_fight" );
				stop_fighting( charmed->victim, TRUE );
				DEBUG_INFO( "die_follower:null_master/leader_self" );
				charmed->victim->leader = NULL;
				charmed->victim->master = NULL;
				DEBUG_INFO( "die_follower:generating_text" );
				if ( charmed->victim->in_room )
				{
					if ( EXT_IS_SET( charmed->victim->act, ACT_NO_EXP ) )
					{
						act( "$n znika.", charmed->victim, NULL, NULL, TO_ROOM );
						DEBUG_INFO( "die_follower:extracting_charm:pre" );
						extract_char( charmed->victim, TRUE );
						DEBUG_INFO( "die_follower:extracting_charm:post" );
					}
					else
					{
						act( "$n ucieka w po¶piechu.", charmed->victim, NULL, NULL, TO_ROOM );
						if ( charmed->victim->reset_vnum > 0 )
						{
							stop_fighting( charmed->victim, TRUE );
							for ( paf = charmed->victim->affected; paf != NULL; paf = paf_next )
							{
								paf_next = paf->next;
								if ( paf->bitvector == &AFF_CHARM )
									affect_remove( charmed->victim, paf );
								if ( paf->bitvector == &AFF_LOYALTY )
									affect_remove( charmed->victim, paf );
							}
							/* wywalam tego walka, bo nie chce dzialac
                               if ( IS_NPC( charmed->victim ) && charmed->victim->in_room &&
                               ( reset_room = get_room_index( charmed->victim->reset_vnum ) ) != NULL &&
                               reset_room->area != charmed->victim->in_room->area )
                               {
                               charmed->victim->walking = -charmed->victim->reset_vnum;
                               }
                               else
                               {*/
							act( "$n wyrusza w dalek± podró¿.", charmed->victim, NULL, NULL, TO_ROOM );
							DEBUG_INFO( "die_follower:extracting_charm:pre" );
							extract_char( charmed->victim, TRUE );
							DEBUG_INFO( "die_follower:extracting_charm:post" );
							//                			}
						}
						else
						{
							DEBUG_INFO( "die_follower:extracting_charm:pre" );
							extract_char( charmed->victim, TRUE );
							DEBUG_INFO( "die_follower:extracting_charm:post" );
						}
					}
				}
			}
			else
			{
				charmed->victim->leader = NULL;
				charmed->victim->master = NULL;
				if ( charmed->victim->reset_vnum == 0 || EXT_IS_SET( charmed->victim->act, ACT_NO_EXP ) )
				{
					act( "$n znika.", charmed->victim, NULL, NULL, TO_ROOM );
					DEBUG_INFO( "die_follower:extracting_charm:pre" );
					extract_char( charmed->victim, TRUE );
					DEBUG_INFO( "die_follower:extracting_charm:post" );
				}
				else
				{
					for ( paf = charmed->victim->affected; paf != NULL; paf = paf_next )
					{
						paf_next = paf->next;
						if ( paf->bitvector == &AFF_CHARM )
							affect_remove( charmed->victim, paf );
						if ( paf->bitvector == &AFF_LOYALTY )
							affect_remove( charmed->victim, paf );
					}
					/*   wywalam tego walka, bo nie chce dzialac
                         if ( IS_NPC( charmed->victim ) && charmed->victim->reset_vnum > 0 && charmed->victim->in_room &&
                         ( reset_room = get_room_index( charmed->victim->reset_vnum ) ) != NULL &&
                         reset_room->area != charmed->victim->in_room->area )
                         {
                         charmed->victim->walking = -charmed->victim->reset_vnum;
                         }
                         else
                         {*/
					act( "$n wyrusza w dalek± podró¿.", charmed->victim, NULL, NULL, TO_ROOM );
					DEBUG_INFO( "die_follower:extracting_charm:pre" );
					extract_char( charmed->victim, TRUE );
					DEBUG_INFO( "die_follower:extracting_charm:post" );
					//                	}
				}
			}
			free( charmed );
		}
		ch->pcdata->charm_list = NULL;
	}
	for ( fch = char_list; fch != NULL; fch = fch->next )
	{
		if ( fch->master == ch )
			stop_follower( fch );
		if ( fch->leader == ch )
			fch->leader = NULL;
	}
	return;
}
/* do_order */
void do_order( CHAR_DATA * ch, char * argument )
{
    char buf[ MAX_STRING_LENGTH ];
    char arg[ MAX_INPUT_LENGTH ], arg2[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    int ile = 0;
    bool check = FALSE;
    /* dajemy standardowy tekst jak go¶æ jest pod wp³ywem 'silence' */
    if ( IS_AFFECTED( ch, AFF_SILENCE ) )
    {
        send_to_char( AFF_SILENCE_TEXT, ch );
        return;
    }
    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );
    if ( !str_cmp( arg2, "delete" )
            || !str_cmp( arg2, "mob" )
            || !str_cmp( arg2, "em")
            || !str_cmp( arg2, "emo")
            || !str_cmp( arg2, "emot")
            || !str_cmp( arg2, "emote")
            || !str_cmp( arg2, ",")
       )
    {
        return;
    }
    if (IS_NPC( ch ))
    {
        return;
    }
    if ( IS_AFFECTED( ch, AFF_SILENCE ) )
    {
        send_to_char( AFF_SILENCE_TEXT, ch );
        return;
    }
    if ( IS_SET( ch->fight_data, FIGHT_TORMENT_NOFLEE ) )
    {
        print_char( ch, "Nie jeste¶ w stanie skupiæ siê na wydawaniu poleceñ swoim s³ugom. Teraz %s przys³ania ci ca³y ¶wiat.\n\r", ch->fighting ? ( IS_NPC(ch->fighting) ? ch->fighting->short_descr : ch->fighting->name ) : "przeciwnik" );
        return;
    }
    if ( arg[ 0 ] == '\0' || argument[ 0 ] == '\0' )
    {
        send_to_char( "Komu lub czemu i co chcesz rozkazaæ?\n\r", ch );
        return;
    }
    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
        send_to_char( "Marzysz sobie jak by³oby fajnie komu¶ porozkazywaæ.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg, "all" ) && check )
    {
        if ( class_table[ch->class].caster < 0 &&
                (
                 !str_cmp( arg2, "czaruj" ) ||
                 !str_cmp( arg2, "czaru"  ) ||
                 !str_cmp( arg2, "czar"   ) ||
                 !str_cmp( arg2, "cza"    ) ||
                 !str_cmp( arg2, "cz"     ) ||
                 !str_cmp( arg2, "cast"   ) ||
                 !str_cmp( arg2, "cas"    ) ||
                 !str_cmp( arg2, "ca"     ) ||
                 !str_cmp( arg2, "c"      )
                )
           )
        {
            send_to_char("Nie bardzo wiesz jak co¶ takiego rozkaæ.\n\r",ch);
            return;
        }
        for ( victim = ch->in_room->people; victim; victim = victim->next_in_room )
        {
            if ( victim == ch )
                continue;
            if ( IS_NPC(victim) && victim->position < POS_RESTING)
            {
                continue;
            }
            if ( !IS_NPC(victim) && is_same_group( ch, victim ) )
            {
                if ( ch->leader && victim->leader != ch )
                {
                    act( "Nie jeste¶ przywódc± grupy.", ch, NULL, victim, TO_CHAR );
                    continue;
                }
                sprintf( buf, "$n rozkazuje ci '%s'.", argument );
                act( buf, ch, NULL, victim, TO_VICT );
                act( "$n wydaje rozkaz $X.", ch, NULL, victim, TO_NOTVICT );
                act( "Wydajesz $X rozkaz.", ch, NULL, victim, TO_CHAR );
                continue;
            }
            save_debug_info("act_comm.c => do_order", NULL, "za is_same_group", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE);
            if ( !IS_AFFECTED( victim, AFF_CHARM ) || victim->master != ch || ( IS_IMMORTAL( victim ) && victim->trust >= ch->trust ) )
            {
                act( "$N nie zwraca uwagi na twoje rozkazy.", ch, NULL, victim, TO_CHAR );
                continue;
            }
            if ( victim->wait > 0 )
            {
                print_char( ch, "%s nie ma teraz czasu aby wykonaæ twój rozkaz.\n\r", IS_NPC(victim) ? capitalize(victim->short_descr) : capitalize(victim->name) );
                continue;
            }
            if ( IS_AFFECTED( victim, AFF_PARALYZE ) || IS_AFFECTED( victim, AFF_DAZE ) )
            {
                print_char( ch, "%s nie reaguje na twoje rozkazy.\n\r", capitalize( !IS_NPC( victim ) ? victim->name : victim->short_descr ) );
                continue;
            }
            if ( !is_affected( victim, gsn_domination ) )
            {
                if ( IS_AFFECTED( ch, AFF_HIDE ) )
                    affect_strip( ch, gsn_hide );
                strip_invis( ch, TRUE, FALSE );
            }
            if ( IS_AFFECTED( victim, AFF_CHARM ) && victim->master == ch )
            {
                sprintf( buf, "$n rozkazuje ci '%s'.", argument );
                act( buf, ch, NULL, victim, TO_VICT );
                act( "$n wydaje rozkaz $X.", ch, NULL, victim, TO_NOTVICT );
                act( "Wydajesz $X rozkaz.", ch, NULL, victim, TO_CHAR );
                interpret( victim, argument );
                ++ile;
            }
            continue;
        }
        if ( ile )
            WAIT_STATE( ch, PULSE_VIOLENCE );
        return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Nikogo takiego tu nie ma.\n\r", ch );
        return;
    }
    if ( victim == ch )
    {
        send_to_char( "Tak jest!!! Ju¿ siê robi!\n\r", ch );
        return;
    }
    if ( class_table[ch->class].caster < 0 &&
            (
             !str_cmp( arg2, "czaruj" ) ||
             !str_cmp( arg2, "czaru"  ) ||
             !str_cmp( arg2, "czar"   ) ||
             !str_cmp( arg2, "cza"    ) ||
             !str_cmp( arg2, "cz"     ) ||
             !str_cmp( arg2, "cast"   ) ||
             !str_cmp( arg2, "cas"    ) ||
             !str_cmp( arg2, "ca"     ) ||
             !str_cmp( arg2, "c"      )
            )
       )
    {
        send_to_char("Nie bardzo wiesz jak co¶ takiego rozkaæ.\n\r",ch);
        return;
    }
    if ( IS_NPC(victim) && victim->position < POS_RESTING)
    {
        send_to_char( "Nie zwraca uwagi na twoje rozkazy!\n\r", ch );
        return;
    }
    if ( !IS_NPC(victim) && is_same_group( ch, victim ) )
    {
        if ( ch->leader && victim->leader != ch )
        {
            send_to_char( "Nie jeste¶ przywódc± tej grupy.\n\r", ch );
            return;
        }
        sprintf( buf, "$n rozkazuje ci '%s'.", argument );
        act( buf, ch, NULL, victim, TO_VICT );
        act( "$n wydaje rozkaz $X.", ch, NULL, victim, TO_NOTVICT );
        act( "Wydajesz $X rozkaz.", ch, NULL, victim, TO_CHAR );
        return;
    }
    save_debug_info("act_comm.c => do_order", NULL, "za is_same_group (1)", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE);
    if ( !IS_AFFECTED( victim, AFF_CHARM ) || victim->master != ch || ( IS_IMMORTAL( victim ) && victim->trust >= ch->trust ) )
    {
        send_to_char( "Nie zwraca uwagi na twoje rozkazy!\n\r", ch );
        return;
    }
    if ( victim->wait > 0 )
    {
        print_char(	ch, "%s nie ma teraz czasu aby wykonaæ twój rozkaz!\n\r", IS_NPC(victim) ? capitalize(victim->short_descr) : capitalize(victim->name) );
        return;
    }
    if ( IS_AFFECTED( victim, AFF_PARALYZE ) || IS_AFFECTED( victim, AFF_DAZE ) )
    {
        print_char( ch, "%s nie reaguje na twoje rozkazy.\n\r",	capitalize( !IS_NPC( victim ) ? victim->name : victim->short_descr ) );
        return;
    }
    //coby mistrzowie przywolan nie mieli za dobrze w pk.
    if ( !is_affected( victim, gsn_domination ) )
    {
        if ( IS_AFFECTED( ch, AFF_HIDE ) )
            affect_strip( ch, gsn_hide );
        strip_invis( ch, TRUE, FALSE );
    }
    sprintf( buf, "$n rozkazuje ci '%s'.", argument );
    act( buf, victim, NULL, victim, TO_VICT );
    act( "$n wydaje rozkaz $X.", ch, NULL, victim, TO_NOTVICT );
    act( "Wydajesz $X rozkaz.", ch, NULL, victim, TO_CHAR );
    interpret( victim, argument );
    /**
     * add wait
     */
    WAIT_STATE( ch, PULSE_VIOLENCE );
    return;
}
char *names_alias( CHAR_DATA * ch, int type, int val );
/* do_group */
void do_group( CHAR_DATA * ch, char * argument )
{
    char buf[ MAX_STRING_LENGTH ];
    BUFFER *active;
    BUFFER *players;
    BUFFER *charmed;
    bool was_charmed = FALSE;
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    int count = 0;
    if ( IS_NPC(ch))
    {
        return;
    }
    one_argument( argument, arg );
    if ( ch->position == POS_SLEEPING )
    {
        switch ( number_range( 1, 7 ) )
        {
        case 1:
            send_to_char( "¦nisz o silnej i sprawnej grupie.\n\r", ch );
            break;
        case 2:
            send_to_char( "¦nisz o tym, jak pokonujesz grupê goblinów.\n\r", ch );
            break;
        case 3:
            send_to_char( "¦nisz o potê¿nej wyprawie z grup± towarzyszy.\n\r", ch );
            break;
        case 4:
            send_to_char( "¦nisz o grupie przyjació³.\n\r", ch );
            break;
        case 5:
            send_to_char( "¦nisz o jakie¶ grupie.\n\r", ch );
            break;
        case 6:
            send_to_char( "¦nisz koszmar o ostatniej grupie.\n\r", ch );
            break;
        default:
            send_to_char( "¦nisz o zwyciêstwie swojej grupy nad potê¿nym smokiem.\n\r", ch );
            break;
        }
        return;
    }
    if ( arg[ 0 ] == '\0' )
    {
        CHAR_DATA * gch;
        char state[ 10 ];
        state[ 0 ] = '\0';
        for ( gch = char_list; gch != NULL; gch = gch->next )
            if ( is_same_group( gch, ch ) )
                count++;
        save_debug_info("act_comm.c => do_group", NULL, "za is_same_group", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE);

        // sprawdzamy wielko¶æ grupy oraz to czy lista charmów jest pusta
        if ( count < 2 && ch->pcdata->charm_list == NULL)
        {
            send_to_char( "Aktualnie nie nale¿ysz do ¿adnej grupy.\n\r", ch );
            return;
        }
        players = new_buf();
        charmed = new_buf();
        send_to_char( "{b>==< {GImiê {b>==<>==< {GZdrowie {b>==<>==< {GZmêczenie {b>==<>==< {GStan {b>=< {GMem {b>\n\r\n\r", ch );
        for ( gch = char_list; gch != NULL; gch = gch->next )
        {
            if ( is_same_group( gch, ch ) )
            {
                if ( IS_NPC( gch ) )
                {
                    active = charmed;
                    was_charmed = TRUE;
                }
                else
                    active = players;
                switch ( gch->position )
                {
                case POS_SLEEPING:
                    sprintf( state, "{y¶pi   ");
                    break;
                case POS_RESTING:
                    sprintf( state, "{yodpocz");
                    if ( can_mem_now( gch ) && gch->memspell && gch->memming )
                    {
                        if ( IS_AFFECTED( gch, AFF_MEDITATION ) )
                            sprintf( state, "{ymedit " );
                        else
                            sprintf( state, "{ymem   " );
                    }
                    if ( IS_AFFECTED( gch, AFF_RECUPERATE ) )
                        sprintf( state, "{yrecup ");
                    break;
                case POS_SITTING:
                    sprintf( state, "{ysiedzi");
                    break;
                case POS_FIGHTING:
                    sprintf( state, "{ywalczy");
                    break;
                default:
                    sprintf( state, "{ystoi  ");
                    if ( IS_AFFECTED( gch, AFF_FLOAT ) )
                        sprintf( state, "{ylewit ");
                    if ( IS_AFFECTED( gch, AFF_FLYING ) )
                        sprintf( state, "{ylata  ");
                    break;
                }
                if ( IS_NPC( gch ) )
                {
                    sprintf( buf,
                             "{m%-32.32s {c[ %-18s {c] [ %-6s {c] [%2d ]",
                             capitalize( PERS( gch, ch ) ),
                             names_alias( gch, 1, 0 ),
                             state,
                             gch->count_memspell );
                }
                else
                {
                    sprintf( buf,
                             "%s%-12s {c[ %-18s {c]  [ %-18s {c] [ %-6s {c] [%2d ]",
                             (!gch->leader && count > 1) ? "{r" : "{c",
                             capitalize( PERS( gch, ch ) ),
                             names_alias( gch, 1, 0 ),
                             names_alias( gch, 2, 0 ),
                             state,
                             gch->count_memspell > 0 ? gch->count_memspell : 0 );
                }
                strcat( buf, "{x\n\r" );
                add_buf( active, buf );
            }
        }
        page_to_char( buf_string( players ), ch );
        free_buf( players );
        // poddani
        if ( was_charmed )
        {
            send_to_char( "\n\r{b>===< {GPoddany {b>===<>==<>===<>==<>===< {GZdrowie {b>==<>==< {GStan {b>=< {GMem {b>\n\r\n\r", ch );
            page_to_char( buf_string( charmed ), ch );
        }
        free_buf( charmed );
        send_to_char( "\n\r{b>==<>==<>==<>==<>==<>==<>==<>==<>==<>===<>==<>==<>==<>==<>==<>==<>==<{x\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Nie ma tu nikogo takiego.\n\r", ch );
        return;
    }
    if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) )
    {
        send_to_char( "Przecie¿ chodzisz za kim¶ innym!\n\r", ch );
        return;
    }
    if ( victim->master != ch && ch != victim )
    {
        act_new( "$N nie chodzi za tob±.", ch, NULL, victim, TO_CHAR, POS_SLEEPING );
        return;
    }
    if ( IS_AFFECTED( victim, AFF_CHARM ) )
    {
        send_to_char( "Nie mo¿esz usun±æ magicznych poddanych z grupy.\n\r", ch );
        return;
    }
    if ( victim == ch )
    {
        send_to_char( "Od razu ci ra¼niej.\n\r", ch );
        return;
    }
    if ( IS_NPC( victim ) )
    {
        print_char( ch, "%s nie chce byæ w twojej grupie.\n\r", victim->short_descr );
        return;
    }
    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
        act_new( "Sk±d ten pomys³?\n\r", ch, NULL, victim, TO_VICT, POS_SLEEPING );
        return;
    }
    if ( is_same_group( victim, ch ) && ch != victim )
    {
        victim->leader = NULL;
        act_new( "$n usuwa $C ze swojej grupy.", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
        act_new( "$n usuwa ciê ze swojej grupy.", ch, NULL, victim, TO_VICT, POS_SLEEPING );
        act_new( "Usuwasz $C ze swojej grupy.", ch, NULL, victim, TO_CHAR, POS_SLEEPING );
        return;
    }
    victim->leader = ch;
    act_new( "$N przy³±cza siê do grupy $z.", ch, NULL, victim, TO_NOTVICT, POS_RESTING );
    act_new( "Przy³±czasz siê do grupy $z.", ch, NULL, victim, TO_VICT, POS_SLEEPING );
    act_new( "$N przy³±cza siê do twojej grupy.", ch, NULL, victim, TO_CHAR, POS_SLEEPING );
    return;
}
/*
 * 'Split' originally by Gnort, God of Chaos.
 */
/* do_split */
void do_split( CHAR_DATA * ch, char * argument )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    long int amount = 0;
    int nomination = NOMINATION_NONE;
    long int money = 0;

    argument = one_argument( argument, arg1 ); //suma do podzialu
    one_argument( argument, arg2 ); // nominal

    if ( arg1[ 0 ] == '\0' )
    {
        send_to_char( "Jak± sum± chcesz siê podzieliæ z grup±?\n\r", ch );
        return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
        long int copper = money_count_copper( ch );
        money_split( ch, ch->copper,  NOMINATION_COPPER,  TRUE, TRUE, FALSE );
        money_split( ch, ch->silver,  NOMINATION_SILVER,  TRUE, TRUE, FALSE );
        money_split( ch, ch->gold,    NOMINATION_GOLD,    TRUE, TRUE, FALSE );
        money_split( ch, ch->mithril, NOMINATION_MITHRIL, TRUE, TRUE, FALSE );
        /**
         * jezeli wielkosc kasy sie nie zmienila, to znaczy, ze nic
         * nie zostalo podzielone, wiec trzeba ch poinformowac o tym
         * fakcie, co by sie nie dziwil ... ze nic
         */
        if ( copper == money_count_copper ( ch ) )
        {
            send_to_char( "Chcesz podzielic wszystko, czyli co?\n\r", ch );
        }
        return;
    }

    if ( arg2[ 0 ] == '\0' )
    {
        send_to_char( "Jakim nominalem chcesz siê podzieliæ z grup±?\n\r", ch );
        return;
    }

    nomination = money_nomination_find( arg2 );

    if ( nomination == NOMINATION_NONE )
    {
        send_to_char( "Nie ma takiego nominalu?\n\r", ch );
        return;
    }

    switch ( nomination )
    {
        case NOMINATION_COPPER:
            money = ch->copper;
            break;
        case NOMINATION_SILVER:
            money = ch->silver;
            break;
        case NOMINATION_GOLD:
            money = ch->gold;
            break;
        case NOMINATION_MITHRIL:
            money = ch->mithril;
            break;
    }

    amount = atoi( arg1 );

    if ( amount < 0 )
    {
        send_to_char( "Ehh nie spodoba im siê to.\n\r", ch );
        act( "$n robi co¶ z monetami, ale do koñca nie wiadomo co.", ch, NULL, NULL, TO_ROOM );
        return;
    }
    if ( amount == 0 )
    {
        send_to_char( "Robisz sztuczki z monetami, ale nikt nie zwraca na ciebie uwagi.\n\r", ch );
        act( "$n kombinuje co¶ z sakiewk±, ale niczego nie wyjmuje.", ch, NULL, NULL, TO_ROOM );
        return;
    }
    else if ( amount == 1 )
    {
        send_to_char( "Ogl±dasz monetê, ale nie bardzo wiesz jak j± podzieliæ.\n\r", ch );
        act( "$n pokazuje jedn± monetê, podrzuca j± i chowa spowrotem do sakiewki.", ch, NULL, NULL, TO_ROOM );
        return;
    }

    if ( amount > money )
    {
        switch ( nomination )
        {
            case NOMINATION_COPPER:
                send_to_char( "Nie masz tylu miedziakow.\n\r", ch );
                break;
            case NOMINATION_SILVER:
                send_to_char( "Nie masz tyle srebra.\n\r", ch );
                break;
            case NOMINATION_GOLD:
                send_to_char( "Nie masz tyle zlota.\n\r", ch );
                break;
            case NOMINATION_MITHRIL:
                send_to_char( "Nie masz tyle mithrilu.\n\r", ch );
                break;
            default:
                send_to_char( "Nie masz tylu monet.\n\r", ch );
                break;
        }
        act( "$n robi g³upi± mine na widok swojej sakiewki.", ch, NULL, NULL, TO_ROOM );
        return;
    }
    money_split( ch, amount, nomination, TRUE, FALSE, FALSE );
    return;
}
/* do_gtell */
void do_gtell( CHAR_DATA * ch, char * argument )
{
    CHAR_DATA * gch;
    CHARM_DATA *charm;
    int speaking;
    int speakswell;
    int counter = 0;
    char *sbuf;
    if ( IS_NPC(ch) )
        return;
    /* dajemy standardowy tekst jak go¶æ jest pod wp³ywem 'silence' */
    if ( IS_AFFECTED( ch, AFF_SILENCE ) )
    {
        send_to_char( AFF_SILENCE_TEXT, ch );
        return;
    }
    if ( !IS_NPC( ch ) && ch->level < 32 )
        argument = rpg_control( argument );
    argument = strip_dupspaces ( argument );
    if ( IS_SET( sector_table[ ch->in_room->sector_type ].flag, SECT_UNDERWATER ) )
    {
        send_to_char( "Pod wod±? Nie da rady.\n\r", ch );
        switch (ch->sex)
        {
        case 0:
        case 1:
            act( "$n próbuje co¶ powiedzieæ, chmura b±belków powietrza wydobywa siê mu z ust.", ch, NULL, NULL, TO_ROOM );
            break;
        case 2:
        default:
            act( "$n probuje co¶ powiedzieæ, chmura b±belków powietrza wydobywa siê jej z ust.", ch, NULL, NULL, TO_ROOM );
            break;
        }
        return;
    }
    if ( argument[ 0 ] == '\0' )
    {
        send_to_char( "Co chcesz powiedzieæ grupie?\n\r", ch );
        return;
    }
    if ( IS_SET( ch->comm, COMM_NOTELL ) )
    {
        send_to_char( "Twoja wiadomo¶æ nie dotar³a!\n\r", ch );
        return;
    }
    speaking = ch->speaking;
    if ( speaking < 0 || speaking > MAX_LANG )
    {
        ch->speaking = 0;
        send_to_char( "Chcia³<&/a/o>by¶ co¶ powiedzieæ, ale zapomnia³<&e/a/o>¶ jêzyka.\n\r", ch );
        return;
    }
    sbuf = argument;
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
        if ( !is_same_group( gch, ch ) )
            continue;
        counter++;
        if ( gch == ch || IS_AFFECTED( gch, AFF_DEAFNESS ) || IS_AFFECTED( gch, AFF_FORCE_FIELD ) )
            continue;
        speakswell = UMIN( knows_language( gch, ch->speaking, ch ),
                           knows_language( ch, ch->speaking, gch ) );
        if ( speakswell < 95 )
            sbuf = translate( speakswell, argument, lang_table[ speaking ].name );
        act_new( "{n$n mówi grupie '{N$t{n'{x",
                 ch, sbuf, gch, TO_VICT, POS_RESTING );
    }
    for ( charm = ch->pcdata->charm_list; charm; charm = charm->next )
    {
        counter++;
        if ( IS_AFFECTED( charm->victim, AFF_DEAFNESS ) || IS_AFFECTED( charm->victim, AFF_FORCE_FIELD ) )
            continue;
        speakswell = UMIN( knows_language( charm->victim, ch->speaking, ch ),
                           knows_language( ch, ch->speaking, charm->victim ) );
        if ( speakswell < 95 )
            sbuf = translate( speakswell, argument, lang_table[ speaking ].name );
        act_new( "{n$n mówi grupie '{N$t{n'{x",
                 ch, sbuf, charm->victim, TO_VICT, POS_RESTING );
    }
    if ( counter == 1 )
    {
        send_to_char( "Nie masz grupy.\n\r", ch );
    }
    else
    {
        send_to_char( "{nMówisz do grupy '{N", ch );
        send_to_char( argument, ch );
        send_to_char( "{n'{x\n\r", ch );
        /**
         * log it
         */
        if ( !IS_NPC( ch ) )
        {
            append_file_format_daily( ch, SPEECH_LOG_FILE, "gtell: %s", argument );
        }
    }
    return;
}

void do_clantell( CHAR_DATA * ch, char * argument )
{
    CHAR_DATA * gch;
    int speaking;
    int speakswell;
    char *sbuf;

    if ( IS_NPC(ch) )
    {
        return;
    }

    if ( ! can_speak( ch, argument, SPEAK_TELL ) )
    {
        return;
    }
    if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
    {
        return;
    }

    if ( !is_clan( ch ) )
    {
        send_to_char( "Nie jeste¶ przecie¿ w ¿adnym klanie.\n\r", ch );
        return;
    }

    if ( argument[ 0 ] == '\0' ) {
        send_to_char( "Co chcesz powiedzieæ klanowi?\n\r", ch );
        return;
    }
    if ( IS_SET( ch->comm, COMM_NOTELL ) ) {
        send_to_char( "Twoja wiadomo¶æ nie dotar³a!\n\r", ch );
        return;
    }

    /* wywalamy buzki itp */
    if ( !IS_NPC( ch ) && ch->level < 32 )
        argument = rpg_control( argument );
    argument = strip_dupspaces ( argument );

    speaking = ch->speaking;
    if ( speaking < 0 || speaking > MAX_LANG )
    {
        ch->speaking = 0;
        send_to_char( "Chcia³<&/a/o>by¶ co¶ powiedzieæ, ale zapomnia³<&e/a/o>¶ jêzyka.\n\r", ch );
        return;
    }

    sbuf = argument;
    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
        if ( !is_same_clan( gch, ch ) )
            continue;

        if ( gch == ch || IS_AFFECTED( gch, AFF_DEAFNESS ) )
            continue;

        speakswell = UMIN( knows_language( gch, ch->speaking, ch ),
                           knows_language( ch, ch->speaking, gch ) );
        if ( speakswell < 95 )
            sbuf = translate( speakswell, argument, lang_table[ speaking ].name );

        /*
        if ( ( ch->pcdata->clan->who_name ) && ( ch->pcdata->clan->who_name[0] != '\0' ) )
            print_char( gch, "{I[{C%s{I] %s: {i%s{x\n\r", ch->pcdata->clan->who_name, ch->name, argument );
        else
            print_char( gch, "{I[CLAN] %s: {i%s{x\n\r", ch->name, argument );
        */
        act_new( "{i$n mówi klanowi '{N$t{n'{x", ch, sbuf, gch, TO_VICT, POS_RESTING );
    }

    send_to_char( "{iMówisz do klanu '{N", ch );
    send_to_char( argument, ch );
    send_to_char( "{n'{x\n\r", ch );
    /**
     * log it
     */
    if ( !IS_NPC( ch ) )
    {
        append_file_format_daily( ch, SPEECH_LOG_FILE, "ctell: %s", argument );
    }
    return;
}

/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 * zmienione na zabugowany kodzik
 */
bool is_same_group( CHAR_DATA * ach, CHAR_DATA * bch )
{
    CHAR_DATA * a, *b;
    int ia, ib;
    char parametry[ MAX_STRING_LENGTH ];
    DEBUG_INFO( "is_same_group:1" );
    sprintf( parametry, "charA: %s charB: %s", ach ? ach->name : "NULL", bch ? bch->name : "NULL" );
    DEBUG_INFO( "is_same_group:2" );
    save_debug_info( "act_comm.c => is_same_group", NULL, parametry, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE );
    DEBUG_INFO( "is_same_group:3" );
    if (!ach || !bch)
    {
        return FALSE;
    }
    DEBUG_INFO( "is_same_group:4" );
    // sprzedawcy i teacherzy nigdy nie s± w grupie
    if ( IS_NPC( ach ) && ( ach->pIndexData->pShop || EXT_IS_SET( ach->act, ACT_PRACTICE ) ) ) return FALSE;
    if ( IS_NPC( bch ) && ( bch->pIndexData->pShop || EXT_IS_SET( bch->act, ACT_PRACTICE ) ) ) return FALSE;
    DEBUG_INFO( "is_same_group:5" );
    for ( ia = 0, a = ach; a; a = a->leader, ia++ )
    {
        DEBUG_INFO( "is_same_group:6" );
        for ( ib = 0, b = bch; b; b = b->leader, ib++ )
        {
            DEBUG_INFO( "is_same_group:7" );
            if ( ia > 16 || ib > 16 )
            {
                DEBUG_INFO( "is_same_group:8" );
                bugf( "is_same_group siê zapêtlil %s %s.", a->name, b->name );
                return FALSE;
            }
            if ( a == b )
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}
/*
 * ColoUr setting and unsetting, way cool, Ant Oct 94
 *        revised to include config colour, Ant Feb 95
 */
/* do_colour */
void do_colour( CHAR_DATA * ch, char * argument )
{
    char arg[ MAX_STRING_LENGTH ];
    if ( IS_NPC( ch ) )
    {
        send_to_char_bw( "WY£¡CZASZ kolory, wszystko wygl±da gorzej, ale... twoja wola.\n\r", ch );
        return;
    }
    argument = one_argument( argument, arg );
    if ( !*arg )
    {
        if ( !EXT_IS_SET( ch->act, PLR_COLOUR ) )
        {
            EXT_SET_BIT( ch->act, PLR_COLOUR );
            send_to_char( "W£¡CZASZ kolory, od razu lepiej!\n\r"
                          "Sk³adnia, na przysz³o¶æ:\n\r   colour {c<{xpole{c> <{xcolour{c>{x\n\r"
                          "   colour {c<{xpole{c>{x {cbeep{x|{cnobeep{x\n\r"
                          "Wpisz help {ccolour{x i {ccolour2{x aby poznaæ szczegó³y.\n\r", ch );
        }
        else
        {
            send_to_char_bw( "WY£¡CZASZ kolory, *wzdychasz*.\n\r", ch );
            EXT_REMOVE_BIT( ch->act, PLR_COLOUR );
        }
        return;
    }
    if ( !str_cmp( arg, "default" ) )
    {
        default_colour( ch );
        send_to_char_bw( "Ustawienia kolorów wracaj± do warto¶ci domy¶lnych.\n\r", ch );
        return;
    }
    if ( !str_cmp( arg, "all" ) )
    {
        all_colour( ch, argument );
        return;
    }
    /*
     * Yes, I know this is ugly and unnessessary repetition, but its old
     * and I can't justify the time to make it pretty. -Lope
     */
    if ( !str_cmp( arg, "text" ) )
    {
        ALTER_COLOUR( text )
    }
    else if ( !str_cmp( arg, "auction" ) )
    {
        ALTER_COLOUR( auction )
    }
    else if ( !str_cmp( arg, "auction_text" ) )
    {
        ALTER_COLOUR( auction_text )
    }
    else if ( !str_cmp( arg, "gossip" ) )
    {
        ALTER_COLOUR( gossip )
    }
    else if ( !str_cmp( arg, "gossip_text" ) )
    {
        ALTER_COLOUR( gossip_text )
    }
    else if ( !str_cmp( arg, "music" ) )
    {
        ALTER_COLOUR( music )
    }
    else if ( !str_cmp( arg, "music_text" ) )
    {
        ALTER_COLOUR( music_text )
    }
    else if ( !str_cmp( arg, "question" ) )
    {
        ALTER_COLOUR( question )
    }
    else if ( !str_cmp( arg, "question_text" ) )
    {
        ALTER_COLOUR( question_text )
    }
    else if ( !str_cmp( arg, "answer" ) )
    {
        ALTER_COLOUR( answer )
    }
    else if ( !str_cmp( arg, "answer_text" ) )
    {
        ALTER_COLOUR( answer_text )
    }
    else if ( !str_cmp( arg, "quote" ) )
    {
        ALTER_COLOUR( quote )
    }
    else if ( !str_cmp( arg, "quote_text" ) )
    {
        ALTER_COLOUR( quote_text )
    }
    else if ( !str_cmp( arg, "immtalk_text" ) )
    {
        ALTER_COLOUR( immtalk_text )
    }
    else if ( !str_cmp( arg, "immtalk_type" ) )
    {
        ALTER_COLOUR( immtalk_type )
    }
    else if ( !str_cmp( arg, "info" ) )
    {
        ALTER_COLOUR( info )
    }
    else if ( !str_cmp( arg, "say" ) )
    {
        ALTER_COLOUR( say )
    }
    else if ( !str_cmp( arg, "say_text" ) )
    {
        ALTER_COLOUR( say_text )
    }
    else if ( !str_cmp( arg, "tell" ) )
    {
        ALTER_COLOUR( tell )
    }
    else if ( !str_cmp( arg, "tell_text" ) )
    {
        ALTER_COLOUR( tell_text )
    }
    else if ( !str_cmp( arg, "shout" ) )
    {
        ALTER_COLOUR( shout )
    }
    else if ( !str_cmp( arg, "shout_text" ) )
    {
        ALTER_COLOUR( shout_text )
    }
    else if ( !str_cmp( arg, "yell" ) )
    {
        ALTER_COLOUR( yell )
    }
    else if ( !str_cmp( arg, "yell_text" ) )
    {
        ALTER_COLOUR( yell_text )
    }
    else if ( !str_cmp( arg, "reply" ) )
    {
        ALTER_COLOUR( reply )
    }
    else if ( !str_cmp( arg, "reply_text" ) )
    {
        ALTER_COLOUR( reply_text )
    }
    else if ( !str_cmp( arg, "gtell_text" ) )
    {
        ALTER_COLOUR( gtell_text )
    }
    else if ( !str_cmp( arg, "gtell_type" ) )
    {
        ALTER_COLOUR( gtell_type )
    }
    else if ( !str_cmp( arg, "wiznet" ) )
    {
        ALTER_COLOUR( wiznet )
    }
    else if ( !str_cmp( arg, "room_title" ) )
    {
        ALTER_COLOUR( room_title )
    }
    else if ( !str_cmp( arg, "room_text" ) )
    {
        ALTER_COLOUR( room_text )
    }
    else if ( !str_cmp( arg, "room_exits" ) )
    {
        ALTER_COLOUR( room_exits )
    }
    else if ( !str_cmp( arg, "room_things" ) )
    {
        ALTER_COLOUR( room_things )
    }
    else if ( !str_cmp( arg, "prompt" ) )
    {
        ALTER_COLOUR( prompt )
    }
    else if ( !str_cmp( arg, "fight_death" ) )
    {
        ALTER_COLOUR( fight_death )
    }
    else if ( !str_cmp( arg, "fight_yhit" ) )
    {
        ALTER_COLOUR( fight_yhit )
    }
    else if ( !str_cmp( arg, "fight_ohit" ) )
    {
        ALTER_COLOUR( fight_ohit )
    }
    else if ( !str_cmp( arg, "fight_thit" ) )
    {
        ALTER_COLOUR( fight_thit )
    }
    else if ( !str_cmp( arg, "fight_skill" ) )
    {
        ALTER_COLOUR( fight_skill )
    }
    else if ( !str_cmp( arg, "fight_trick" ) )
    {
        ALTER_COLOUR( fight_trick )
    }
    else if ( !str_cmp( arg, "fight_spell" ) )
    {
        ALTER_COLOUR( fight_spell )
    }
    else
    {
        send_to_char_bw( "Nierozpoznany parametr kolorów, nie ustawiam.\n\r", ch );
        return;
    }
    send_to_char_bw( "Nowy parametr kolorów ustawiony.\n\r", ch );
    return;
}
bool is_ignoring( CHAR_DATA * ch, char * argument )
{
    char name[ MAX_INPUT_LENGTH ];
    char arg[ MAX_INPUT_LENGTH ];
    char *ign;
    one_argument( argument, arg );
    if ( IS_NPC( ch ) )
        return FALSE;
    if ( arg[ 0 ] == '\0' )
        return FALSE;
    if ( !ch->pcdata || !ch->pcdata->ignore || ch->pcdata->ignore[ 0 ] == '\0' )
        return FALSE;
    ign = ch->pcdata->ignore;
    for (;; )
    {
        ign = one_argument( ign, name );
        if ( name[ 0 ] == '\0' )
            break;
        if ( !str_cmp( arg, name ) )
            return TRUE;
    }
    return FALSE;
}
void do_ignore( CHAR_DATA * ch, char * argument )
{
    CHAR_DATA * victim;
    char arg[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    char buf1[ MAX_STRING_LENGTH ];
    char buf2[ MAX_STRING_LENGTH ];
    char *iter;
    int col = 0;
    bool found = FALSE;
    if ( IS_NPC( ch ) )
        return;
    one_argument( argument, arg );
    if ( arg[ 0 ] == '\0' )
    {
        if ( ch->pcdata->ignore[ 0 ] == '\0' )
        {
            send_to_char( "Nie ignorujesz ¿adnej osoby.\n\r", ch );
            return;
        }
        buf1[ 0 ] = '\0';
        send_to_char( "Lista osób ignorowanych:\n\r", ch );
        iter = ch->pcdata->ignore;
        for (;; )
        {
            iter = one_argument( iter, arg );
            if ( arg[ 0 ] == '\0' )
                break;
            sprintf( buf, "%-19.18s", capitalize( arg ) );
            strcat( buf1, buf );
            if ( ++col % 4 == 0 )
                strcat( buf1, "\n\r" );
        }
        if ( col % 4 != 0 )
            strcat( buf1, "\n\r" );
        send_to_char( buf1, ch );
        return;
    }
    if ( ch->pcdata->ignore && ch->pcdata->ignore[ 0 ] != '\0' )
    {
        sprintf( buf, "%s", ch->pcdata->ignore );
        iter = buf;
        buf2[ 0 ] = '\0';
        for (;; )
        {
            iter = one_argument( iter, arg2 );
            if ( arg2[ 0 ] == '\0' )
                break;
            if ( str_cmp( arg, arg2 ) )
            {
                sprintf( buf1, "%s ", arg2 );
                strcat( buf2, buf1 );
            }
            else
                if ( !str_cmp( arg, arg2 ) )
                    found = TRUE;
        }
        if ( found )
        {
            free_string( ch->pcdata->ignore );
            ch->pcdata->ignore = str_dup( buf2 );
            print_char( ch, "Osoba '%s' usuniêta z listy osób ignorowanych.\n\r", capitalize( arg ) );
            return;
        }
    }
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        /* 2007-05-23: Gurthg
         * nie ma sensu sprawdzaæ plus kwestia zmian we who
        	send_to_char( "Nie ma takiej osoby w grze.\n\r", ch );
        */
        send_to_char( "Nie ma tutaj takiej osoby.\n\r", ch );
        return;
    }
    /* wykluczamy samego siebie */
    if ( victim == ch )
    {
        send_to_char( "Chcesz ignorowaæ sa<&m/ma/mo> siebie?\n\r", ch );
        return;
    }
    DEBUG_INFO( "get_char_world" );
    if ( IS_NPC( victim ) )
    {
        /* 2007-05-23: Gurthg
         * nie ma sensu sprawdzaæ plus kwestia zmian we who
        	send_to_char( "Mo¿esz ignorowaæ tylko graczy.\n\r", ch );
        */
        send_to_char( "Nie ma tutaj takiej osoby.\n\r", ch );
        return;
    }
    DEBUG_INFO( "is_npc_victim" );
    if ( is_ignoring( ch , victim->name ) )
    {
        send_to_char( "Ju¿ ignorujesz t± osobê. Je¶li chcesz usun±æ j± z listy wpisz pe³ne imiê.\n\r", ch );
        return;
    }
    DEBUG_INFO( "is_ignoring" );
    if ( IS_IMMORTAL( victim ) )
    {
        send_to_char( "Bóstwa trzeba s³uchaæ, a nie ignorowaæ.\n\r", ch );
        return;
    }
    DEBUG_INFO( "is_immortal" );
    if ( !ch->pcdata->ignore || ch->pcdata->ignore[ 0 ] == '\0' )
    {
        free_string( ch->pcdata->ignore );
        ch->pcdata->ignore = str_dup( victim->name );
    }
    else
    {
        sprintf( buf, "%s ", ch->pcdata->ignore );
        free_string( ch->pcdata->ignore );
        strcat( buf, victim->name );
        ch->pcdata->ignore = str_dup( buf );
    }
    print_char( ch, "'{R%s{x' - dodano do listy osób ignorowanych.\n\r", victim->name );
    return;
}

//Brohacz: bounty: usuwa gracza z bounty list przy odbieraniu nagrody
bool rem_bounty( BOUNTY_DATA **list, char *argument )
{
	BOUNTY_DATA * tmp, *org_list, *tmp_prev;
	bool result = FALSE;

	org_list = *list;
	tmp = org_list;

	if ( !tmp )
		return FALSE;

	if ( !str_cmp( tmp->name, argument ) )
	{
		org_list = org_list->next;
		free_bounty( tmp );
		tmp = NULL;
		*list = org_list;
		result = TRUE;
	}
	else
	{

		tmp_prev = tmp;

		for ( ;tmp;tmp = tmp->next )
		{
			if ( !str_cmp( tmp->name, argument ) )
			{
				tmp_prev->next = tmp->next;
				tmp->next = NULL;
				free_bounty( tmp );
				tmp = NULL;
				result = TRUE;
				break;
			}

			tmp_prev = tmp;
		}
	}

	if ( *list == misc.bounty_list )
		save_misc_data();

	return result;
}

//Brohacz: bounty: dodaje gracza do bounty list
BOUNTY_DATA * add_bounty( BOUNTY_DATA **list, char *name, int value )
{
	BOUNTY_DATA *tmp, *it, *old_it;
	int i;

	if ( !name || name[ 0 ] == '\0' )
		return NULL;

	if ( strlen( name ) > 32 )
		return NULL;

	name[ 0 ] = UPPER( name[ 0 ] );
	for ( i = 1; name[ i ] != '\0'; i++ )
		name[ i ] = LOWER( name[ i ] );

	tmp = search_bounty( name );
	if ( tmp ) //jezeli gosc jest juz na liscie, to sumujemy nagrody, wywalamy go i dodajemy ponownie, zeby zachowac sortowanie
	{
		value += tmp->value;
		rem_bounty( &misc.bounty_list, name );
		tmp = add_bounty( &misc.bounty_list, name, value );
		return tmp;
	}
	else
	{
		it = *list;
		old_it = NULL;
		while ( it && it->value > value ) {
			old_it = it;
			it = it->next;
		}

		tmp = new_bounty();
		tmp->name = str_dup( name );
		tmp->value = value;
		tmp->next = it;

		if ( !old_it )
			(*list) = tmp;
		else
			old_it->next = tmp;
	}
	return tmp;
}

//Brohacz: bounty: sprawdza, czy gracz jest na bounty list
BOUNTY_DATA * search_bounty( char *name )
{
	BOUNTY_DATA * tmp;
	BOUNTY_DATA * list = misc.bounty_list;

	if ( !name || name[ 0 ] == '\0' )
		return NULL;

	if ( strlen( name ) > 32 )
		return NULL;

	if ( list )
	{
		for ( tmp = list; tmp; tmp = tmp->next )
		{
			if ( !str_cmp( tmp->name, name ) )
				return tmp;
		}
	}

	return NULL;
}

//orginalny pomysl by FRiTZ <fritz1@swbell.net>
//bardzo zmodyfikowany snippet z ftp.game.org
void do_bounty( CHAR_DATA *ch, char *argument )
{
    char arg1 [ MAX_INPUT_LENGTH ];
    char arg2 [ MAX_INPUT_LENGTH ];
    BOUNTY_DATA *wch;
    int bounty_price;
    OBJ_DATA *head;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( !EXT_IS_SET( ch->in_room->room_flags, ROOM_BOUNTY_OFFICE ) )
    {
        send_to_char( "Nie mo¿esz tego tutaj zrobiæ, id¼ do biura ³owców nagród.\n\r", ch );
        return;
    }
    if ( !str_prefix( arg1, "list" ) )
    {
        if ( !misc.bounty_list )
        {
            send_to_char( "W tym momencie nie ma na ¶wiecie nikogo, za którego g³owê by³aby wyznaczona nagroda.\n\r", ch );
        }
        else
        {
            print_char( ch, "Aktualnie nagrody wyznaczone s± za:\n\r" );
            for ( wch = misc.bounty_list; wch != NULL; wch = wch->next )
            {
                print_char( ch, "%-9s - %d srebrnych monet\n\r", wch->name, wch->value );
            }
        }
        return;
    }
    if ( arg1[ 0 ] == '\0' )
    {
        for ( head = ch->carrying; head; head = head->next_content )
        {
            if ( head->item_type == ITEM_TROPHY && head->value[ 0 ] > 0 )
            {
                print_char( ch, "Otrzymujesz %s za przyniesienie %s.\n\r", money_string( head->value[ 0 ], FALSE ), head->name2 );
                act( "$n odbiera nagrodê.", ch, NULL, NULL, TO_ROOM );
                money_gain( ch, head->value[ 0 ] );
                extract_obj( head );
                return;
            }
        }
        send_to_char( "Za zabicie kogo chcesz wyznaczyæ nagrodê?\n\r", ch );
        return;
    }
    if ( is_allowed( arg1 ) != NAME_ALLOWED && is_allowed( arg1 ) != NAME_NEW )
    {
        send_to_char( "Nikt o takim imieniu nie przemierza ¶wiata.\n\r", ch );
        return;
    }
    if ( arg2[ 0 ] == '\0' || !is_number( arg2 ) )
    {
        send_to_char( "Jak wysok± nagrodê chcesz na³o¿yæ za tê g³owê?\n\r", ch );
        return;
    }

    bounty_price = atoi( arg2 );
    if ( bounty_price < 500 * RATTING_SILVER )
    {
        send_to_char( "Minimalna warto¶æ stawki to 500 srebrnych monet.\n\r", ch );
        return;
    }
    if ( money_count_copper ( ch ) < bounty_price * RATTING_SILVER )
    {
        send_to_char( "Masz przy sobie za malo pieniêdzy.\n\r", ch );
        return;
    }
    money_reduce( ch, bounty_price );

    wch = add_bounty( &misc.bounty_list, arg1, bounty_price );
    save_misc_data();
    if ( bounty_price != wch->value )
    {
        print_char( ch, "Do³o¿y³<&e¶/a¶/o¶> %s do nagrody za zabicie swojego wroga o imieniu %s.\n\r", money_string( bounty_price, FALSE ), arg1 );
    }
    else
    {
        print_char( ch, "Na³o¿y³<&e¶/a¶/o¶> %s nagrody za zabicie swojego wroga o imieniu %s.\n\r", money_string( bounty_price, FALSE ), arg1 );
    }
    return;
}

// Pomysl by Taka z Ghost MUD 3
void do_knock(CHAR_DATA *ch, char *argument)
{
    long door;
    char arg[MAX_INPUT_LENGTH];
    one_argument(argument,arg);
    if (arg[0] == '\0')
    {
        send_to_char("W co chcesz zapukaæ?\n\r",ch);
        return;
    }
    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
        ROOM_INDEX_DATA *to_room;
        EXIT_DATA *pexit;
        EXIT_DATA *pexit_rev;
        pexit = ch->in_room->exit[door];
        if ( !IS_SET( pexit->exit_info, EX_ISDOOR ) )
        {
            send_to_char("W co chcesz zapukaæ?\n\r",ch);
            return;
        }
        act( "$n kilka razy uderza mocno w $d.", ch, NULL, pexit->biernik, TO_ROOM );
        act( "Uderzasz kilka razy mocno w $d.", ch, NULL, pexit->biernik, TO_CHAR );
        /* Info dla tych po drugiej stronie  */
        if (   ( to_room   = pexit->u1.to_room            ) != NULL
                && ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
                && pexit_rev->u1.to_room == ch->in_room )
        {
            CHAR_DATA *rch;
            for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
                act( "S³yszysz g³o¶nie pukanie do $d.", rch, NULL, pexit_rev->keyword, TO_CHAR );
        }
        rp_knock_trigger( ch, door );
    }
    else
    {
        send_to_char("W co chcesz zapukaæ?\n\r",ch);
        act( "$n dziwnie macha rêkami.", ch, NULL, NULL, TO_ROOM );
    }
}

void do_randtest(CHAR_DATA *ch, char *argument)
{
    int i;
    int max = atoi(argument);
    char buf[MSL];
    if ( max > 1000 )
        max = 1000;
    if ( max == 0 )
        max = 10;
    buf[0] = '\0';
    for (i=0;i<max;i++)
    {
        sprintf(buf,"%d ", number_percent() );
        send_to_char(buf,ch);
    }
    send_to_char(buf,ch);
    return;
}

//gracze do poziomu NEWBIE_LEVEL mog± u¿ywaæ kana³u newbie, pytanie pojawia siê wszystkim w grze, którzy maj± ten kana³ w³±czony.
void do_newbie( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA * d;
    if ( argument[ 0 ] == '\0' )
    {
        return;
    }

    if ( !IS_SET( ch->comm, COMM_NEWBIE ) && !IS_IMMORTAL( ch ) )
    {
      send_to_char( "Nie mo¿esz u¿ywaæ kana³u NEWBIE.\n\r", ch );
      return;
    }

    act_new( "{iNewbie [{I$n{i]: $t{x", ch, argument, NULL, TO_CHAR, POS_DEAD );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( !d->character || d->connected < 0 ) continue;
        if ( d->connected == CON_PLAYING &&
                !IS_SET( d->character->comm, COMM_NONEWBIE ) )
        {
            act_new( "{iNewbie [{I$n{i]: $t{x", ch, argument, d->character, TO_VICT, POS_DEAD );
        }
    }
    return;
}

//dodawanie statow przez graczy
void do_addstat( CHAR_DATA *ch, char *argument )
{
	char arg[ MAX_STRING_LENGTH ];

	if ( argument[ 0 ] == '\0' )
	{
		send_to_char( "Komenda ta s³u¿y do modyfikowania w³asnych statysyk.\n\r", ch );
		send_to_char( "Sposob spozycia:\n\r", ch );
		send_to_char( "addstat <argument>.\n\r", ch );
		send_to_char( "gdzie argumenty to: show - pokazuje ile punktów masz do dyspozycji .\n\r", ch );
		send_to_char( "str - dodaje jeden punkt do si³y, int - inteligencji, wis - wiedzy, dex - zreczno¶ci, con - kondycji, cha - charyzmy, luc - szczê¶cia.\n\r", ch );
		return;
	}

	argument = one_argument( argument, arg );

	if ( !str_cmp( arg, "show" ) )
	{
		print_char( ch, "Masz do rozdysponowania %d punktów statystyk.\n\r", ch->statpointsleft);
		return;
	}

	if (ch->statpointsleft <= 0){ send_to_char( "Brak dostêpnych punktów statystyk do rozdysponowania.\n\r", ch );return;}

	if ( !str_cmp( arg, "str" ) )
	{
		ch->perm_stat[0] += 1;
		send_to_char( "Doda³e¶ jeden punkt do si³y.\n\r", ch );
		ch->statpointsleft -=1;
		ch->statpointsspent +=1;
		return;
	}
	if ( !str_cmp( arg, "int" ) )
	{
		ch->perm_stat[1] += 1;
		send_to_char( "Doda³e¶ jeden punkt do inteligencji.\n\r", ch );
		ch->statpointsleft -=1;
		ch->statpointsspent +=1;
		return;
	}
	if ( !str_cmp( arg, "wis" ) )
	{
		ch->perm_stat[2] += 1;
		send_to_char( "Doda³e¶ jeden punkt do wiedzy.\n\r", ch );
		ch->statpointsleft -=1;
		ch->statpointsspent +=1;
		return;
	}
	if ( !str_cmp( arg, "dex" ) )
	{
		ch->perm_stat[3] += 1;
		send_to_char( "Doda³e¶ jeden punkt do zrêczno¶ci.\n\r", ch );
		ch->statpointsleft -=1;
		ch->statpointsspent +=1;
		return;
	}
	if ( !str_cmp( arg, "con" ) )
	{
		ch->perm_stat[4] += 1;
		send_to_char( "Doda³e¶ jeden punkt do kondycji.\n\r", ch );
		ch->statpointsleft -=1;
		ch->statpointsspent +=1;
		return;
	}
	if ( !str_cmp( arg, "cha" ) )
	{
		ch->perm_stat[5] += 1;
		send_to_char( "Doda³e¶ jeden punkt do charyzmy.\n\r", ch );
		ch->statpointsleft -=1;
		ch->statpointsspent +=1;
		return;
	}
	if ( !str_cmp( arg, "luc" ) )
	{
		ch->perm_stat[6] += 1;
		send_to_char( "Doda³e¶ jeden punkt do szczê¶cia.\n\r", ch );
		ch->statpointsleft -=1;
		ch->statpointsspent +=1;
		return;
	}

	send_to_char( "Huh?.\n\r", ch );
	return;

	}
