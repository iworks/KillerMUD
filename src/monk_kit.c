/*
 * $Id: monk_kit.c 10351 2011-06-12 11:38:53Z raszer $
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
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "traps.h"
#include "tables.h"
#include "progs.h"
#include "monk_kit.h"

char	*names_alias	args( (CHAR_DATA *ch, int type, int val) );
sh_int	get_caster	args( (CHAR_DATA *ch) );
void	one_hit 	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt, bool second ) );
int	parry_mod 	args( (OBJ_DATA *weapon) );
void	set_fighting	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int	find_door	args( ( CHAR_DATA *ch, char *arg ) );
void	raw_kill	args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void	heal_char	args( ( CHAR_DATA *ch, CHAR_DATA *victim, sh_int value, bool all ) );


MONK_HITS *monk_hits_list = NULL;


void free_monk_hit( MONK_HITS * hit )
{
    if( !hit )
	return;

    free_string( hit->hit_name );
    free_string( hit->monk_text );
    free_string( hit->victim_text );
    free_string( hit->room_text );
    free( hit );
    return;
}

MONK_HITS * new_monk_hit()
{
    MONK_HITS * hit = NULL;

    hit = (MONK_HITS *)malloc(sizeof(*hit));

    hit->hit_name	= "\0";
    hit->monk_text	= "\0";
    hit->victim_text	= "\0";
    hit->room_text	= "\0";

    hit->min_skill_req	= 0;
    hit->min_wisdom_req	= 0;
    hit->rating		= 0;
    hit->damage[0]	= 0;
    hit->damage[1]	= 0;
    hit->dam_bonus	= 0;
    hit->hit_bonus	= 0;
    hit->victim_type	= 0;
    hit->victim_parts	= 0;
    hit->special_flag	= 0;

    return hit;
}

void load_monk_hits()
{
    MONK_HITS *hit = NULL;
    FILE *fp;
    char *word;
    bool match;

    if ( ( fp = fopen( MONK_HIT_FILE, "r" ) ) == NULL )
        return;

    for ( ; ; )
    {
	if ( feof(fp) )
        {
            fclose( fp );
    	    return;
        }

    	word = fread_word(fp);
	match = FALSE;

	 switch( word[0] )
	 {
	    case 'A':
	    case 'B':
	    case 'D':
	    	    if( !str_cmp( word, "Damage"))
		    {
		    	if( !hit )
			{
			    fread_to_eol(fp);
			    break;
			}

			hit->damage[0] = fread_number( fp );
			hit->damage[1] = fread_number( fp );
			match = TRUE;
			break;
		    }
		    else if( !str_cmp( word, "Dam_bonus"))
		    {
		    	if( !hit )
			{
			    fread_to_eol(fp);
			    break;
			}

			hit->dam_bonus = fread_number( fp );
			match = TRUE;
			break;
		    }
		    break;
	    case 'H':
	    	    if( !str_cmp( word, "Hit_bonus"))
		    {
		    	if( !hit )
			{
			    fread_to_eol(fp);
			    break;
			}

			hit->hit_bonus = fread_number( fp );
			match = TRUE;
			break;
		    }
		    else if( !str_cmp( word, "Hit_end"))
		    {
		    	if( !hit )
			{
			    fread_to_eol(fp);
			    break;
			}

			hit->next = monk_hits_list;
			monk_hits_list = hit;
			hit = NULL;
		    	match = TRUE;
			fread_to_eol(fp);
			break;
		    }
		    break;

	    case 'M':
	    	    if( !str_cmp( word, "Min_skill"))
		    {
		    	if( !hit )
			{
			    fread_to_eol(fp);
			    break;
			}

			hit->min_skill_req = fread_number( fp );
			match = TRUE;
			break;
		    }
		    else if( !str_cmp( word, "Min_wisdom"))
		    {
		    	if( !hit )
			{
			    fread_to_eol(fp);
			    break;
			}

			hit->min_wisdom_req = fread_number( fp );
			match = TRUE;
			break;
		    }
		    break;

		    break;

	    case 'N':
		    if( !str_cmp( word, "Name"))
		    {
			if( hit )
			    free_monk_hit( hit );

			hit = new_monk_hit();
			hit->hit_name = fread_string( fp );
			match = TRUE;
			break;
		    }
		    break;
	    case 'R':
		    if( !str_cmp( word, "Rating"))
		    {
		    	if( !hit )
			{
			    fread_to_eol(fp);
			    break;
			}

			hit->rating = fread_number( fp );
			match = TRUE;
			break;
		    }
		    break;
    	    case 'S':
	    	    if( !str_cmp( word, "Special_flag"))
		    {
		    	if( !hit )
			{
			    fread_to_eol(fp);
			    break;
			}

			hit->special_flag = fread_flag( fp );
			match = TRUE;
			break;
		    }

	    case 'T':
		    if( !str_cmp( word, "Text_monk"))
		    {
			if( !hit )
			{
			    fread_to_eol(fp);
			    break;
			}

			hit->monk_text = fread_string( fp );
			match = TRUE;
			break;
		    }
		    else if( !str_cmp( word, "Text_victim"))
		    {
			if( !hit )
			{
			    fread_to_eol(fp);
			    break;
			}

			hit->victim_text = fread_string( fp );
			match = TRUE;
			break;
		    }
		    else if( !str_cmp( word, "Text_room"))
		    {
			if( !hit )
			{
			    fread_to_eol(fp);
			    break;
			}

			hit->room_text = fread_string( fp );
			match = TRUE;
			break;
		    }
		    break;
	    case 'V':
	    	    if( !str_cmp( word, "Victim_type"))
		    {
		    	if( !hit )
			{
			    fread_to_eol(fp);
			    break;
			}

			hit->victim_type = fread_flag( fp );
			match = TRUE;
			break;
		    }
		    else if( !str_cmp( word, "Victim_parts"))
		    {
		    	if( !hit )
			{
			    fread_to_eol(fp);
			    break;
			}

			hit->victim_parts = fread_flag( fp );
			match = TRUE;
			break;
		    }

		    break;

	    default:
		    fread_to_eol(fp);
		    bugf("Error while loading monk hits, word '%s' not matched", word);
		    break;
	 }

	 if( !match )
	    bugf("Error while loading monk hits, word '%s' not matched", word);

    }
    return;
}

//flurry of blows - +1 atak, -2 do trafienia do wszystkich ciosow
void do_flurry( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA aff;
    int skill = get_skill( ch, gsn_flurry_of_blows );

    if ( skill == 0 )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }
    if( !ch->fighting )
    {
	send_to_char("Przeciez z nikim nie walczysz.\n\r", ch);
	return;
    }

    if( is_affected( ch, gsn_flurry_of_blows ) )
    {
	affect_strip( ch, gsn_flurry_of_blows );
	send_to_char("Skupiasz sie bardziej na celnosci swoich ciosow.\n\r", ch);
	return;
    }

    aff.where	= TO_AFFECTS;
    aff.type	= gsn_flurry_of_blows;
    aff.level	= ch->level;
    aff.duration	= -1;
    aff.rt_duration = 0;
    aff.bitvector 	= &AFF_NONE;
    aff.location	= APPLY_NONE;
    aff.modifier	= 0;
    aff.visible      = TRUE;
    affect_to_char(ch,&aff, NULL, TRUE);
    send_to_char("Skupiasz sie bardziej na szybkosci zadawanych ciosow.\n\r", ch);
    WAIT_STATE(ch,PULSE_VIOLENCE);
    return;
}

//nastepny cios ma szanse wywrocic na runde przeciwnika
void do_knockdown( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA aff;

    if( !ch->fighting )
    {
	send_to_char("Przeciez z nikim nie walczysz.\n\r", ch);
	return;
    }

    aff.where		= TO_AFFECTS;
    aff.type		= gsn_knockdown;
    aff.level		= ch->level;
    aff.duration	= -1;
    aff.rt_duration = 0;
    aff.bitvector 	= &AFF_NONE;
    aff.location	= APPLY_NONE;
    aff.modifier	= 0;
    aff.visible      	= FALSE;
    affect_to_char(ch,&aff, NULL, FALSE);
    WAIT_STATE(ch, 2*PULSE_VIOLENCE);
    return;
}

//odwrotny do flurry of blows - +2..4 do trafienia, -1 atak
void do_concentration( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA aff;

    if( !ch->fighting )
    {
	send_to_char("Przeciez z nikim nie walczysz.\n\r", ch);
	return;
    }

    if( is_affected( ch, gsn_concentration ) )
    {
	affect_strip( ch, gsn_concentration );
	send_to_char("Przestajesz sie koncentrowac na celnosci swoich ciosow.\n\r", ch);
	return;
    }

    aff.where	= TO_AFFECTS;
    aff.type	= gsn_concentration;
    aff.level	= ch->level;
    aff.duration	= -1;
    aff.rt_duration = 0;
    aff.bitvector 	= &AFF_NONE;
    aff.location	= APPLY_NONE;
    aff.modifier	= 0;
    aff.visible      = TRUE;
    affect_to_char(ch,&aff, NULL, TRUE);

    send_to_char("Skupiasz sie bardziej na celnosci zadawanych ciosow.\n\r", ch);
    WAIT_STATE(ch,PULSE_VIOLENCE);
    return;
    return;
}

//oglusza na 2-4 rundy, limit uzyc
void do_stunning( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA aff;

    if( !ch->fighting )
    {
	send_to_char("Przeciez z nikim nie walczysz.\n\r", ch);
	return;
    }

    aff.where		= TO_AFFECTS;
    aff.type		= gsn_stunning_fist;
    aff.level		= ch->level;
    aff.duration	= -1;
    aff.rt_duration = 0;
    aff.bitvector 	= &AFF_NONE;
    aff.location	= APPLY_NONE;
    aff.modifier	= 0;
    aff.visible      	= FALSE;
    affect_to_char(ch,&aff, NULL, FALSE);
    WAIT_STATE(ch, 2*PULSE_VIOLENCE);
    return;
}

//'memowanie' specjalnych umiejetnosci
void do_meditation( CHAR_DATA *ch, char *argument )
{
    return;
}

//leczenie dlonmi, medytowane, leczy do 100 dzies tak
void do_healing( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int value, skill;
    AFFECT_DATA af;

    if( (skill = get_skill(ch, gsn_healing_hands)) <= 0 )
    {
	send_to_char("Huh?\n\r", ch );
	return;
    }

    if (argument[0] == '\0')
    {
	send_to_char("Kogo chcesz uleczyc?\n\r",ch);
	return;
    }

    if ((victim = get_char_room(ch,argument)) == NULL)
    {
	send_to_char("Nie ma tu nikogo takiego.\n\r",ch);
	return;
    }

    if( is_affected( ch, gsn_healing_hands ) )
    {
        send_to_char("Nie mozesz siê wystarczaj±co skoncentrowaæ.\n\r",ch);
	return;
    }

    af.where     = TO_AFFECTS;

    af.type      = gsn_healing_hands;

    af.level     = 0;

    af.duration  = URANGE( 3, 5 - ch->level/10, 5);
    af.rt_duration = 0;

    af.location  = APPLY_NONE;

    af.modifier  = 0;
    af.bitvector = &AFF_NONE;


    affect_to_char( ch, &af, NULL, FALSE );


    WAIT_STATE( ch, skill_table[gsn_healing_hands].beats );

    value = (number_range(75, 95)*4*ch->level)/100;
    value = URANGE( 20, value,  110 );
    value = (50 + skill/2)*value/100;

    if(ch==victim)
    {
	act("Przykladasz dlonie do swojego ciala i zaczynasz sie koncentrowac. Po chwili zaczynasz odczuwac rozchodzace sie z twoich dloni cieplo.", ch, NULL, NULL, TO_CHAR);
	act("$n przyklada dlonie do swojego ciala i zamyka oczy.", ch, NULL, NULL, TO_ROOM);
    }
    else
    {
	act("Przykladasz dlonie do ciala $Z i zaczynasz sie koncentrowac.", ch, NULL, victim, TO_CHAR);
	act("$n przyklada dlonie do twojego ciala i zamyka oczy. Po chwili zaczynasz odczuwac rozchodzace sie z dloni $z cieplo.", ch, NULL, victim, TO_VICT);
	act("$n przykladasz dlonie do ciala $Z i zamyka oczy.", ch, NULL, victim, TO_NOTVICT);
    }

    heal_char( ch, victim, value, FALSE );
    update_pos( victim );


    check_improve(ch, NULL, gsn_healing_hands, TRUE,20);
    check_improve(ch, NULL, gsn_healing_hands, TRUE,40);
    return;
}

//na kilka tickow, medytowane
void do_kistrike( CHAR_DATA *ch, char *argument )
{
    return;
}

//parry monka, tyle ciosow moze sparowac ile sam ma
void do_deflect( CHAR_DATA *ch, char *argument )
{
    return;
}

void generate_monk_miss( CHAR_DATA * ch, CHAR_DATA *victim )
{
    return;
}

void generate_monk_dam_message(CHAR_DATA *ch, CHAR_DATA *victim, MONK_HITS * hit)
{
    char buf[256];

    if( !hit )
	return;

    sprintf( buf, "{2%s{x", hit->monk_text );
    act( buf, ch, NULL, victim, TO_CHAR );

    sprintf( buf, "{4%s{x", hit->victim_text );
    act( buf, ch, NULL, victim, TO_VICT );

    sprintf( buf, "{3%s{x", hit->room_text );
    act( buf, ch, NULL, victim, TO_NOTVICT );
}

MONK_HITS * generate_hit( CHAR_DATA *ch, CHAR_DATA *victim )
{
    MONK_HITS * hits_table[32], *ptr;
    int num = 0;

    for( ptr = monk_hits_list; ptr ; ptr = ptr->next, num++ )
	hits_table[num] = ptr;

    return hits_table[number_range(0, num-1)];
}

int max_monk_hits( CHAR_DATA *ch )
{
    //baza
    int hits = 1;
    int rush_skill = get_skill( ch, gsn_rush );
    int flurry_skill = get_skill( ch, gsn_flurry_of_blows );


    if( rush_skill > 0 )
    {
	if( (25 + 3*rush_skill/4) > number_percent() )
	{
	    check_improve(ch, NULL, gsn_rush, TRUE, 23 );
	    hits++;
	}
	else
	{
	    check_improve(ch, NULL, gsn_rush, FALSE, 20 );
	}
    }

    if( flurry_skill > 0 && is_affected( ch, gsn_flurry_of_blows) )
    {
	if( (50 + flurry_skill/2 ) > number_percent() )
	{
	    hits++;
	    check_improve(ch, NULL, gsn_flurry_of_blows, TRUE, 23 );
	}
	else
	{
	    check_improve(ch, NULL, gsn_flurry_of_blows, FALSE, 20 );
	}
    }
    return hits;
}
