/*********************************************************************
 *                                                                   *
 * KILLER MUD is copyright 1999-2011 Killer MUD Staff (alphabetical) *
 *                                                                   *
 *   ZMIENIA£E¦ CO¦? DOPISZ SIÊ!                                     *
 *                                                                   *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti  ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor   ] *
 * Koper Tadeusz         (garloop@killer-mud.net        ) [Garloop ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg  ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas  ] *
 * Skrzetnicki Krzysztof (gtener@gmail.com              ) [Tener   ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro    ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron   ] *
 *                                                                   *
 *********************************************************************/
/* $Id: prewait.c 10701 2011-12-02 16:03:39Z illi $ */
#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "interp.h"
#include "tables.h"
#include "progs.h"

void show_comms	args( ( CHAR_DATA *ch, PFLAG_DATA *lst ) );
bool check_reflect_spell	args( ( CHAR_DATA *victim, sh_int circle ) );
sh_int get_caster( CHAR_DATA *ch );
int flag_value args( ( const struct flag_type *flag_table, char *argument ) );
char *flag_string args( ( const struct flag_type *flag_table, int bits ) );

/*
  Komenda do ustawiania i sprawdzania prewaitow dla spelli/skill
 */

void do_prewait( CHAR_DATA *ch, char *argument )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char arg3[ MAX_INPUT_LENGTH ];
    char buf[ MAX_STRING_LENGTH ];
    PRE_WAIT *tmp = NULL, *new_el = NULL;
    PFLAG_DATA *list = NULL, *newlist = NULL, *tmpls = NULL, *prev_tmp = NULL;
    BUFFER *buffer = NULL;
    int sn;
    int time;
    int target;

    if ( ch->pcdata->security < 8 )
    {
        send_to_char( "Acces denied.\n\r", ch );
        return ;
    }

    if ( argument[ 0 ] == '\0' )
    {
        if ( pre_waits == NULL )
        {
            send_to_char( "Nie zdefiniowano ¿adnych prewaitów.\n\r", ch );
            return ;
        }

        buffer = new_buf();

        for ( tmp = pre_waits; tmp ; tmp = tmp->next )
        {
            sprintf( buf, "Zaklêcie:%-20s Prewait:[%-2d] (%s)\n\r",
                     tmp->spell_name, tmp->duration,
                     tmp->list ? "komentarze" : "brak komentarzy" );
            add_buf( buffer, buf );
        }

        page_to_char( buf_string( buffer ), ch );
        free_buf( buffer );
        return ;
    }
    else if ( !str_cmp( argument, "save" ) )
    {
        FILE * fp;

        if ( pre_waits == NULL )
            return ;

        fclose( fpReserve );

        if ( ( fp = fopen( PREWAIT_FILE, "w" ) ) == NULL )
            return ;

        for ( tmp = pre_waits;tmp;tmp = tmp->next )
        {
            fprintf( fp, "s %d %s~\n", tmp->duration, tmp->spell_name );

            for ( list = tmp->list;list;list = list->next )
                fprintf( fp, "k %d %d %s~\n", list->target, list->duration, list->id );
        }

        fclose( fp );
        fpReserve = fopen( NULL_FILE, "r" );
        send_to_char( "Lista zosta³a zapisana.\n\r", ch );
        return ;
    }

    argument = one_argument( argument, arg1 );

    if ( ( sn = skill_lookup( arg1 ) ) == -1 || is_skill_lookup( arg1 ) )
    {
        send_to_char( "Nie ma takiego zaklêcia.\n\r", ch );
        return ;
    }

    if ( !str_cmp( argument, "del" ) )
    {
        if ( pre_waits == NULL )
        {
            send_to_char( "Nie ma czego usuwaæ.\n\r", ch );
            return ;
        }

        if ( pre_waits->sn == sn )
        {
            tmp = pre_waits;
            pre_waits = pre_waits->next;

            if ( tmp->list )
            {
                tmpls = tmp->list;
                tmp->list = NULL;

                for ( ;tmpls;tmpls = prev_tmp )
                {
                    prev_tmp = tmpls->next;
                    free_pflag( tmpls );
                    tmpls = NULL;
                }
            }

            free_prewait( tmp );
            tmp = NULL;
            send_to_char( "Element zosta³ usuniêty.\n\r", ch );
            return ;
        }
        else
        {
            if ( !pre_waits->next )
            {
                send_to_char( "Nie ma takiego elementu.\n\r", ch );
                return ;
            }

            for ( tmp = pre_waits;tmp->next->sn != sn;tmp = tmp->next );

            new_el = tmp;
            tmp = tmp->next;
            new_el->next = tmp->next;

            if ( tmp->list )
            {
                tmpls = tmp->list;
                tmp->list = NULL;

                for ( ;tmpls;tmpls = prev_tmp )
                {
                    prev_tmp = tmpls->next;
                    free_pflag( tmpls );
                    tmpls = NULL;
                }
            }

            free_prewait( tmp );
            tmp = NULL;
            send_to_char( "Element zosta³ usuniêty.\n\r", ch );
            return ;

        }
        return ;
    }

    if ( argument[ 0 ] == '\0' )
    {
        for ( tmp = pre_waits;tmp;tmp = tmp->next )
        {
            if ( sn == tmp->sn )
            {
                if ( tmp->list == NULL )
                {
                    sprintf( buf, "Zaklêcie:%-20s Prewait:[%-2d]\n\rBrak komentarzy\n\r", tmp->spell_name, tmp->duration );
                    send_to_char( buf, ch );
                    return ;
                }

                buffer = new_buf();
                sprintf( buf, "Zaklêcie:%-20s Prewait:[%-2d]\n\rKomentarze:\n\r", tmp->spell_name, tmp->duration );
                add_buf( buffer, buf );

                for ( list = tmp->list;list;list = list->next )
                {
                    sprintf( buf, "[%-2d] Cel: %-10s\n\r%s\n\r",
                             list->duration,
                             flag_string( prewait_target, list->target ),
                             list->id );
                    add_buf( buffer, buf );
                }

                page_to_char( buf_string( buffer ), ch );
                free_buf( buffer );
                return ;
            }
        }

        send_to_char( "Nie zdefiniowano prewait.\n\r", ch );
        return ;
    }
    else
    {

        argument = one_argument( argument, arg2 );
        argument = one_argument( argument, arg3 );

        if ( !is_number( arg2 ) )
        {
            send_to_char( "Drugi argument powinien byæ liczb±.\n\r", ch );
            return ;
        }

        time = atoi( arg2 );

        if ( time < 0 )
        {
            send_to_char( "Drugi argument powinien byæ wiêkszy od zera.\n\r", ch );
            return ;
        }

        for ( tmp = pre_waits;tmp;tmp = tmp->next )
            if ( tmp->sn == sn )
            {
                if ( time >= tmp->duration )
                {
                    send_to_char( "Drugi argument jest za du¿y.\n\r", ch );
                    return ;
                }

                target = flag_value( prewait_target, arg3 );
                switch ( skill_table[ sn ].target )
                {
                    case TAR_IGNORE:
                        if ( target == NO_FLAG )
                            target = TARGET_NONE;
                        else
                            target = -1;
                        break;
                    case TAR_CHAR_OFFENSIVE:
                    case TAR_CHAR_DEFENSIVE:
                        if ( target == NO_FLAG || target == TARGET_OBJ )
                            target = -1;
                        break;
                    case TAR_CHAR_SELF:
                        if ( target == NO_FLAG )
                            target = TARGET_SELF;
                        else
                            target = -1;
                        break;
                    case TAR_OBJ_INV:
                    case TAR_OBJ_ROOM:
                        if ( target == NO_FLAG )
                            target = TARGET_OBJ;
                        else
                            target = -1;
                        break;
                    case TAR_OBJ_CHAR_DEF:
                    case TAR_OBJ_CHAR_OFF:
                        if ( target == NO_FLAG )
                            target = -1;
                        break;
                    default: target = TARGET_NONE; break;
                }

                if ( target == -1 )
                {
                    send_to_char( "Poda³e¶ z³y cel, którego bêdzie dotyczy³ prewait.\n\r", ch );
                    switch ( skill_table[ sn ].target )
                    {
                        case TAR_IGNORE:
                            send_to_char( "Nie musisz podawaæ celu przy tym czarze.\n\r", ch );
                            break;
                        case TAR_CHAR_OFFENSIVE:
                        case TAR_CHAR_DEFENSIVE:
                            send_to_char( "Dostêpne cele dla tego czaru: self other.\n\r", ch );
                            break;
                        case TAR_CHAR_SELF:
                            send_to_char( "Dostêpne cele dla tego czaru: self.\n\r", ch );
                            break;
                        case TAR_OBJ_INV:
                        case TAR_OBJ_ROOM:
                            send_to_char( "Dostêpne cele dla tego czaru: object.\n\r", ch );
                            break;
                        case TAR_OBJ_CHAR_DEF:
                        case TAR_OBJ_CHAR_OFF:
                            send_to_char( "Dostêpne cele dla tego czaru: self other object.\n\r", ch );
                            break;
                    }
                    return ;
                }

                for ( list = tmp->list;list;list = list->next )
                {
                    if ( list->duration == time && list->target == target )
                    {
                        send_to_char( "Edycja komentarza.\n\r", ch );

                        ch->desc->pEdit = NULL;
                        ch->desc->editor = ED_PREWAIT;
                        string_append( ch, &list->id );
                        return ;
                    }
                }

                newlist = new_pflag();
                send_to_char( "Wprowadzanie nowego komentarza.\n\r", ch );
                ch->desc->pEdit = NULL;
                ch->desc->editor = ED_PREWAIT;
                string_append( ch, &newlist->id );
                newlist->duration = time;
                newlist->target = target;
                newlist->next = NULL;

                if ( tmp->list == NULL )
                {
                    tmp->list = newlist;
                }
                else
                {
                    for ( list = tmp->list;list->next;list = list->next );

                    list->next = newlist;
                }
                return ;
            }

        new_el = new_prewait();
        new_el->spell_name = str_dup( skill_table[ sn ].name );
        new_el->duration = time;
        new_el->sn = sn;
        new_el->next = NULL;

        if ( pre_waits == NULL )
        {
            pre_waits = new_el;
        }
        else
        {
            for ( tmp = pre_waits;tmp->next;tmp = tmp->next );
            tmp->next = new_el;
        }

        send_to_char( "Wprowadzono nowy prewait.\n\r", ch );
        return ;
    }
    return ;
}

PRE_WAIT * get_prewait( int sn )
{
    PRE_WAIT * tmp = NULL;

    if ( sn < 0 )
        return NULL;

    for ( tmp = pre_waits;tmp;tmp = tmp->next )
        if ( tmp->sn == sn )
            return ( tmp );

    return NULL;
}

bool check_pwait( CHAR_DATA *ch, int sn )
{
    PRE_WAIT * tmp = NULL;

    if ( sn < 0 )
        return FALSE;

    for ( tmp = pre_waits;tmp;tmp = tmp->next )
        if ( tmp->sn == sn )
        {
            WAIT_STATE( ch, tmp->duration );
            return TRUE;
        }

    return FALSE;
}


/* nie ma zadnych testow poprawnosci argumentow...*/
void setup_pwait( CHAR_DATA *ch, char *spell, void *voo, int target, int sn )
{
    PWAIT_CHAR * tmp;

    tmp = new_pwait_char();
    tmp->spell_name = str_dup( spell );
    tmp->vo = voo;
    tmp->target = target;
    tmp->sn = sn;
    ch->wait_char = tmp;

    return ;
}

void pwait_cast_update( CHAR_DATA *ch )
{
    PRE_WAIT * tmp = NULL;
    PFLAG_DATA *lst = NULL;
    CHAR_DATA *victim = NULL;
    OBJ_DATA *obj = NULL;
    void *tar = NULL;
    int sn, target, counter = 0, timer, tg, bonus_from_feat;

    timer = ch->wait;

    if ( ch->wait_char == NULL )
        return ;

    tmp = get_prewait( ch->wait_char->sn );
    sn = ch->wait_char->sn;
    target = ch->wait_char->target;
    tar = ch->wait_char->vo;

    tg = target;
    if ( ch == ( CHAR_DATA * ) tar )
        tg = TARGET_SELF;

    if ( target == TARGET_CHAR &&
         ( victim = ( CHAR_DATA * ) tar ) != NULL &&
         ( !IS_VALID( victim ) ||
         ( victim->position == POS_DEAD || !victim->in_room ) ) )
    {
        free_pwait_char( ch->wait_char );
        ch->wait_char = NULL;
        return ;
    }



    if ( timer > 0 )
    {

        if ( !tmp->list )
            return ;

        counter = ch->wait;

        for ( lst = tmp->list;lst;lst = lst->next )
            if ( lst->duration == counter && lst->target == tg )
            {
                show_comms( ch, lst );
                break;
            }
        return ;
    }
    else
    {
        free_pwait_char( ch->wait_char );
        ch->wait_char = NULL;

        if ( target == TARGET_CHAR &&
             ( victim = ( CHAR_DATA * ) tar ) != NULL &&
             ( !IS_VALID( victim ) ||
             !get_char_room( ch, victim->name ) ) )
        {
            if ( skill_table[ sn ].target == TAR_CHAR_OFFENSIVE ||
                 skill_table[ sn ].target == TAR_OBJ_CHAR_OFF )
            {
                if ( ch->sex == 0 )
                {
                    send_to_char( "Twoja ofiara zd±¿y³a czmychn±æ zanim skoñczy³o¶ rzucaæ zaklêcie.\n\r", ch );
                    send_to_char( "Nagle, odnosisz wra¿enie, ¿e co¶ jest nie tak..\n\r", ch );
                    send_to_char( "Na kogo to mia³o¶ rzuciæ to zaklêcie? Najwyrazniej twój cel gdzie¶ sobie poszed³.\n\r", ch );
                }
                else if ( ch->sex == 1 )
                {
                    send_to_char( "Twoja ofiara zd±¿y³a czmychn±æ zanim skoñczy³e¶ rzucaæ zaklêcie.\n\r", ch );
                    send_to_char( "Nagle, odnosisz wra¿enie, ¿e co¶ jest nie tak..\n\r", ch );
                    send_to_char( "Na kogo to mia³e¶ rzuciæ to zaklêcie? Najwyrazniej twój cel gdzie¶ sobie poszed³.\n\r", ch );
                }
                else
                {
                    send_to_char( "Twoja ofiara zd±¿y³a czmychn±æ zanim skoñczy³a¶ rzucaæ zaklêcie.\n\r", ch );
                    send_to_char( "Nagle, odnosisz wra¿enie, ¿e co¶ jest nie tak..\n\r", ch );
                    send_to_char( "Na kogo to mia³a¶ rzuciæ to zaklêcie? Najwyrazniej twój cel gdzie¶ sobie poszed³.\n\r", ch );
                }
                return ;
            }
            return ;
        }
        else if ( target == TARGET_OBJ )
        {
            if ( ( obj = ( OBJ_DATA * ) tar ) != NULL &&
                 ( !IS_VALID( obj ) ||
                 !get_obj_here( ch, obj->name ) ) )
            {
                send_to_char( "Nie widzisz tutaj tego.\n\r", ch );
                return ;
            }
        }


        /* reflect spell stuff */
        if ( target == TARGET_CHAR )
        {
            int circle = 0;
            CHAR_DATA *victim;

            victim = ( CHAR_DATA * ) tar;

            if ( IS_VALID( victim ) && victim != ch )
            {
                circle = spell_circle( ch, sn );

                if ( check_reflect_spell( victim, circle ) )
                {
                    tar = ( void * ) ch;
                    print_char( ch, "Twoje zaklêcie odbija siê od ochronnej tarczy %s.\n\r", victim->name2 );
                    print_char( victim, "Zaklêcie %s odbija siê od ochronnej tarczy.\n\r", ch->name2 );
                    act( "Zaklêcie $z odbija siê od ochronnej tarczy $Z.", ch, NULL, victim, TO_NOTVICT );
                    if ( !IS_AFFECTED( victim, AFF_REFLECT_SPELL ) )
                    {
                        send_to_char( skill_table[ gsn_reflect_spell ].msg_off, victim );
                        send_to_char( "\n\r", victim );
                    }
                }
            }
        }
        bonus_from_feat = 0;

        /* specjalisci maja +2 do levela zaklecia*/
        if ( !IS_NPC( ch ) &&
             ch->pcdata->mage_specialist >= 0 &&
             IS_SET( skill_table[ sn ].school, school_table[ ch->pcdata->mage_specialist ].flag ) )
            ( *skill_table[ sn ].spell_fun ) ( sn, ch->level + 2 + bonus_from_feat, ch, tar, target );
        //paladyni
        else if ( get_caster( ch ) == 4 )
        {
            ( *skill_table[ sn ].spell_fun ) ( sn, UMAX( 1, ( ch->level - 12 ) + bonus_from_feat ), ch, tar, target );
        }
        else
            ( *skill_table[ sn ].spell_fun ) ( sn, ch->level + bonus_from_feat, ch, tar, target );

        WAIT_STATE( ch, skill_table[ sn ].beats );


        if ( ( skill_table[ sn ].target == TAR_CHAR_OFFENSIVE
               || ( skill_table[ sn ].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR ) )
             && ( victim = ( CHAR_DATA * ) tar ) != ch
             && victim->master != ch )
        {
            CHAR_DATA * vch;
            CHAR_DATA *vch_next;

            for ( vch = ch->in_room->people; vch; vch = vch_next )
            {
                vch_next = vch->next_in_room;

                if ( victim == vch && victim->fighting == NULL && can_see( victim, ch ) )
                {
                    multi_hit( victim, ch, TYPE_UNDEFINED );
                    break;
                }
            }
        }
    }

    return ;
}

void show_comms( CHAR_DATA *ch, PFLAG_DATA *lst )
{
    sh_int sn;
    char buf[ MAX_INPUT_LENGTH ];

    if ( !lst->id || !ch->wait_char ) return ;

    sn = ch->wait_char->sn;


    sprintf( buf, "prewait %d", sn );
    pset_supermob( ch );

	if ( ch->wait_char->target == TARGET_CHAR || ch->wait_char->target == TARGET_OBJ )
		create_mprog_env( buf, lst->id, supermob, ch, NULL, ch->wait_char->vo, NULL, NULL );
	else
		create_mprog_env( buf, lst->id, supermob, ch, NULL, NULL, NULL, NULL );

    program_flow();
    release_supermob();
    return ;
}
