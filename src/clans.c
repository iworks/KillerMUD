/************************************************************************
 *                                                                      *
 * KILLER MUD is copyright 1999-2011 Killer MUD Staff (alphabetical)    *
 *                                                                      *
 *    Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti  ] *
 *    Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor   ] *
 *    Pietrzak Marcin       (marcin.pietrzak@mud.pl        ) [Gurthg  ] *
 *    Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas  ] *
 *    Skrzetnicki Krzysztof (tener@tenet.pl                ) [Tener   ] *
 *    Trebicki Marek        (maro@killer.radom.net         ) [Maro    ] *
 *    Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron   ] *
 *                                                                      *
 ************************************************************************/
/*
 $Id: clans.c 10701 2011-12-02 16:03:39Z illi $
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
#include <dirent.h>
#include "merc.h"
#include "clans.h"
#include "recycle.h"

CLAN_DATA * clan_list = NULL;

long last_clan_id = 0;

/*
 * zwraca id klanu - aktualny czas
 */
long get_clan_id(void)
{
	int val;

	val = (current_time <= last_clan_id) ? last_clan_id + 1 : current_time;
	last_clan_id = val;
	return val;
}

/*
 * Tworzy nowego cz³onka klanu.
 */
CLAN_MEMBER * new_clan_member(void)
{
	CLAN_MEMBER * member;

	CREATE( member, CLAN_MEMBER, 1 );

	member->next = NULL;
	member->name = &str_empty[0];
	member->rank = CLAN_RANK_MEMBER;
	member->joined = current_time;

	return member;
}

/*
 * Zwalnia pamiêæ cz³onka klanu, nie wywala z listy
 */
void free_clan_member(CLAN_MEMBER * member)
{
	if (!member)
		return;

	free_string( member->name );

	DISPOSE( member );
}

/*
 * Zwalnia ca³± listê cz³onków klanu
 */
void free_members_list(CLAN_MEMBER ** list)
{
	CLAN_MEMBER *member, *member_next;

	for (member = *list; member; member = member_next)
	{
		member_next = member->next;
		free_clan_member(member);
	}

	*list = NULL;
	return;
}

/*
 * Zwraca wskaznik na clan_member o podanym imieniu
 */
CLAN_MEMBER * get_member(CLAN_DATA * clan, char * name)
{
	CLAN_MEMBER * member;

	if (!clan || !clan->members || IS_NULLSTR( name ))
		return NULL;

	for (member = clan->members; member; member = member->next)
	{
		if (FAST_STR_CMP( member->name, name ))
			return member;
	}
	return NULL;
}

/*
 * Dodaje gracza do klanu na dane stanowisko
 */
CLAN_MEMBER * add_to_clan(CLAN_DATA * clan, char * name, int rank)
{
	CLAN_MEMBER * member;

	if (IS_NULLSTR( name ) || !clan)
		return NULL;

	member = new_clan_member();

	member->name = str_dup(capitalize( name ));
	member->rank = rank;
	member->joined = current_time;

	member->next = clan->members;
	clan->members = member;

	return member;
}

/*
 * Wywala podany clan_member z klanu
 */
void remove_from_clan(CLAN_DATA * clan, CLAN_MEMBER * member)
{
	CHAR_DATA * vch;

	if (!clan || !clan->members || !member)
		return;

	if (member == clan->members)
	{
		clan->members = member->next;
	}
	else
	{
		CLAN_MEMBER * prev;

		for (prev = clan->members; prev; prev = prev->next)
		{
			if (prev->next == member)
			{
				prev->next = member->next;
				break;
			}
		}
	}

	if (member->rank == CLAN_RANK_OVERLORD && count_members(clan) > 0)
	{
		char * chief;
		CLAN_MEMBER * for_member, *best_member;
		int i;
		time_t best_time;

		chief = get_chieftain(clan);

		if (!IS_NULLSTR( chief ))
		{
			best_member = get_member(clan, chief);
		}
		else
		{
			best_time = current_time + 1;
			best_member = NULL;
			for (i = CLAN_RANK_CUSTOM; i < CLAN_MAX_RANKS
					&& clan->rank_names[i]; i++)
			{
				for (for_member = clan->members; for_member; for_member
						= for_member->next)
				{
					if (for_member->rank == i && for_member->joined < best_time)
					{
						best_member = for_member;
						best_time = for_member->joined;
					}

				}
				if (best_member)
					break;
			}

			if (!best_member)
			{
				for (for_member = clan->members; for_member; for_member
						= for_member->next)
				{
					if (for_member->rank == CLAN_RANK_MEMBER
							&& for_member->joined < best_time)
					{
						best_member = for_member;
						best_time = for_member->joined;
					}
				}
			}
		}

		if (best_member)
		{
			best_member->rank = CLAN_RANK_OVERLORD;
			for (vch = char_list; vch; vch = vch->next)
			{
				if (!IS_NPC( vch ) && vch->pcdata
						&& FAST_STR_CMP( vch->name, best_member->name ))
					vch->pcdata->clan_rank = CLAN_RANK_OVERLORD;
			}

		}
		else
		{
			clan->active = FALSE;
		}
	}
	else if (count_members(clan) == 0)
	{
		clan->active = FALSE;
	}

	free_clan_member(member);

	return;

}

void set_clan_rank(CLAN_DATA * clan, char * name, int rank)
{
	CLAN_MEMBER * member = get_member(clan, name);

	if (member)
		member->rank = rank;

	return;
}

void change_member_name(CLAN_DATA * clan, char * old_name, char * new_name)
{
	CLAN_MEMBER * member;

	if (!clan || IS_NULLSTR( old_name ) || IS_NULLSTR( new_name ))
		return;

	member = get_member(clan, old_name);

	if (!member)
		return;

	free_string( member->name );
	member->name = str_dup(capitalize( new_name ));

	return;
}

/*
 * Zwraca imiê szefa klanu
 */
char * get_overlord(CLAN_DATA * clan)
{
	CLAN_MEMBER * member;

	for (member = clan->members; member; member = member->next)
	{
		if (member->rank == CLAN_RANK_OVERLORD)
			return member->name;
	}
	return NULL;
}

/*
 * Zwraca imiê zastêpcy szefa klanu
 */
char * get_chieftain(CLAN_DATA * clan)
{
	CLAN_MEMBER * member;

	for (member = clan->members; member; member = member->next)
	{
		if (member->rank == CLAN_RANK_CHIEFTAIN)
			return member->name;
	}
	return NULL;
}

/*
 * Zwraca listê imion ludzi na podanym stanowisku
 */
char * get_list_by_rank(CLAN_DATA * clan, int rank, char * colour)
{
	static char buf[MAX_STRING_LENGTH];
	char separator[MAX_STRING_LENGTH];
	CLAN_MEMBER * member;
	bool first = TRUE;

	buf[0] = '\0';

	if (!IS_NULLSTR( colour ))
	{
		sprintf(separator, "{x, %s", colour);
		strcat(buf, colour);
	}

	for (member = clan->members; member; member = member->next)
	{
		if (member->rank == rank)
		{
			if (!first && IS_NULLSTR( colour ))
				strcat(buf, ", ");

			if (!first && !IS_NULLSTR( colour ))
				strcat(buf, separator);

			strcat(buf, member->name);
			first = FALSE;
		}
	}

	if (!IS_NULLSTR( colour ))
	{
		strcat(buf, "{x");
	}

	return buf;
}

/*
 * Zwraca liczbê imion ludzi na podanym stanowisku
 */
int get_count_by_rank(CLAN_DATA * clan, int rank)
{
	CLAN_MEMBER * member;
	int count = 0;

	for (member = clan->members; member; member = member->next)
	{
		if (member->rank == rank)
			count++;
	}
	return count;
}

/*
 * Tworzy nowy klan, nie do³±cza do listy
 */
CLAN_DATA * new_clan(void)
{
	CLAN_DATA * clan;
	int i;

	CREATE( clan, CLAN_DATA, 1 );

	clan->id = get_clan_id();
	clan->name = &str_empty[0];
	clan->motto = &str_empty[0];
	clan->description = &str_empty[0];
	clan->who_name = &str_empty[0];
	clan->pkills = 0;
	clan->pdeaths = 0;
	clan->mkills = 0;
	clan->mdeaths = 0;
	clan->members = NULL;
	clan->active = TRUE;
	clan->last_entered = 0;

	for (i = 0; i < CLAN_MAX_RANKS; i++)
		clan->rank_names[i] = NULL;

	clan->rank_names[CLAN_RANK_OVERLORD] = str_dup("przywódca klanu");
	clan->rank_names[CLAN_RANK_CHIEFTAIN] = str_dup("zastêpca przywódcy");
	clan->rank_names[CLAN_RANK_MEMBER] = str_dup("cz³onek");
	clan->ranks = 3;
	return clan;
}

/*
 * Zwalnia pamiêc dla klanu, nie wywala z listy
 */
void free_clan(CLAN_DATA *clan)
{
	int i;

	if (!clan)
		return;

	free_string( clan->name );
	free_string( clan->motto );
	free_string( clan->description );
	free_string( clan->who_name );
	for (i = 0; i < CLAN_MAX_RANKS; i++)
		free_string( clan->rank_names[i] );

	free_members_list(&clan->members);
	DISPOSE( clan );
	return;
}

/*
 * wywala klan z listy i zwalnia pamiêæ
 */
void clan_from_list(CLAN_DATA *clan)
{
	CHAR_DATA *vch;

	if (!clan)
		return;

	if (clan == clan_list)
	{
		clan_list = clan->next;
	}
	else
	{
		CLAN_DATA * prev;

		for (prev = clan_list; prev; prev = prev->next)
		{
			if (prev->next == clan)
			{
				prev->next = clan->next;
				break;
			}
		}
	}

	for (vch = char_list; vch; vch = vch->next)
	{
		if (!IS_NPC( vch ) && vch->pcdata && vch->pcdata->clan == clan)
		{
			vch->pcdata->clan = NULL;
			vch->pcdata->clan_rank = -1;
		}
	}

	free_clan(clan);
}

/*
 * Odszukuje klan z podanym cz³onkiem
 */
CLAN_DATA * get_clan_by_member(char * name)
{
	CLAN_DATA * clan;

	if (IS_NULLSTR( name ))
		return NULL;

	for (clan = clan_list; clan; clan = clan->next)
	{
		if (get_member(clan, name))
			return clan;
	}
	return NULL;
}

CLAN_DATA * get_clan_by_name(char *clan_name)
{
	CLAN_DATA * clan;
	int clan_nr, counter;

	if (IS_NULLSTR( clan_name ))
		return NULL;

	if (is_number(clan_name))
	{
		clan_nr = atoi(clan_name);

		if (clan_nr <= 0)
			return NULL;

		for (clan = clan_list, counter = 1; clan; clan = clan->next)
		{
			if (counter == clan_nr)
				return clan;

			counter++;
		}

		return NULL;
	}

	for (clan = clan_list; clan; clan = clan->next)
	{
		if (FAST_STR_CMP( clan->name, clan_name ))
			return clan;
	}

	return NULL;
}

CLAN_DATA * get_clan_by_id(long id)
{
	CLAN_DATA * clan;

	for (clan = clan_list; clan; clan = clan->next)
		if (clan->id == id)
			return clan;

	return NULL;
}

int count_members(CLAN_DATA * clan)
{
	CLAN_MEMBER * member;
	int count = 0;

	for (member = clan->members; member; member = member->next, count++)
		;

	return count;
}

void clean_ranks(CLAN_DATA * clan)
{
	int i;

	for (i = CLAN_RANK_CUSTOM; i < CLAN_MAX_RANKS; i++)
	{
		free_string( clan->rank_names[ i ] );
		clan->rank_names[i] = NULL;
	}

	clan->ranks = 3;

	return;
}

void update_member_ranks(CLAN_DATA * clan)
{
	CLAN_MEMBER * member;

	for (member = clan->members; member; member = member->next)
	{
		if (member->rank >= clan->ranks - 1)
			member->rank = CLAN_RANK_MEMBER;
	}
}

void save_clans(void)
{
	CLAN_DATA * clan;
	CLAN_MEMBER * member;
	CLAN_STANCE * stance;
	FILE *fp;
	int i;

	if (!clan_list)
		return;

	fclose(fpReserve);

	if ((fp = fopen(CLAN_FILE, "w")) == NULL)
		return;

	for (clan = clan_list; clan; clan = clan->next)
	{
		fprintf(fp, "Name %s~\n", clan->name);
		fprintf(fp, "Id %ld\n", clan->id);
		fprintf(fp, "Motto %s~\n", clan->motto);
		fprintf(fp, "Descr %s~\n", clan->description);
		fprintf(fp, "Who %s~\n", clan->who_name);
		fprintf(fp, "Pkills %d\n", clan->pkills);
		fprintf(fp, "Pdeaths %d\n", clan->pdeaths);
		fprintf(fp, "Mkills %d\n", clan->mkills);
		fprintf(fp, "Mdeaths %d\n", clan->mdeaths);
		fprintf(fp, "Active %d\n", clan->active);
		fprintf(fp, "LastEntered %ld\n", clan->last_entered);

		for (i = 0; i < CLAN_MAX_RANKS; i++)
		{
			if (IS_NULLSTR( clan->rank_names[i] ))
				continue;

			fprintf(fp, "Rank %s~\n", clan->rank_names[i]);
		}

		for (member = clan->members; member; member = member->next)
		{
			if (IS_NULLSTR( member->name ))
				continue;

			fprintf(fp, "Member %s %d %ld\n", member->name, member->rank,
					member->joined);
		}

		for (stance = clan->stances; stance; stance = stance->next)
		{
			if (stance->clan_id == 0)
				continue;

			fprintf(fp, "Aliance %d %d\n", stance->clan_id, stance->clan_state);
		}

		fprintf(fp, "Clanend\n");
	}

	fclose(fp);
	fpReserve = fopen(NULL_FILE, "r");
	return;
}

void load_clans(void)
{
	CLAN_DATA * clan = NULL;
	FILE *fp;
	char *word;
	bool match;
	int ranks;

	if ((fp = fopen(CLAN_FILE, "r")) == NULL)
		return;

	for (;;)
	{
		if (feof(fp))
		{
			fclose(fp);
			return;
		}

		word = fread_word(fp);

		match = FALSE;

		switch (word[0])
		{
		case 'A':
			if (!str_cmp(word, "Active"))
			{
				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				clan->active = fread_number(fp);

				match = TRUE;
				break;
			}
			if (!str_cmp(word, "Aliance"))
			{
				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				CLAN_STANCE* stance;

				CREATE(stance, CLAN_STANCE, 1);

				stance->clan_id = fread_number(fp);
				stance->clan_state = fread_number(fp);

				if (clan->stances == NULL)
					clan->stances = stance;
				else
				{
					CLAN_STANCE* stance_next;
					stance_next = clan->stances;
					stance->next = stance_next;
					clan->stances = stance;
				}

				match = TRUE;
				break;
			}
			break;

		case 'C':
			if (!str_cmp(word, "Clanend"))
			{
				CLAN_DATA * clan_last;

				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				if (clan->last_entered != 0 && clan->last_entered
						+ CLAN_INACTIVE_PERIOD < current_time)
					clan->active = FALSE;

				clan->ranks = ranks;
				clan->next = NULL;

				if (!clan_list)
					clan_list = clan;
				else
				{
					for (clan_last = clan_list; clan_last->next; clan_last
							= clan_last->next)
						;

					clan_last->next = clan;
				}

				clan = NULL;
				fread_to_eol(fp);
				match = TRUE;
				break;
			}
			break;

		case 'D':
			if (!str_cmp(word, "Descr"))
			{
				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				clan->description = fread_string(fp);
				match = TRUE;
				break;
			}
			break;

		case 'I':
			if (!str_cmp(word, "Id"))
			{
				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				clan->id = fread_long_number(fp);
				match = TRUE;
			}
			break;

		case 'L':
			if (!str_cmp(word, "LastEntered"))
			{
				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				clan->last_entered = fread_number(fp);
				match = TRUE;
				break;
			}
			break;

		case 'M':
			if (!str_cmp(word, "Mdeaths"))
			{
				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				clan->mdeaths = fread_number(fp);
				match = TRUE;
				break;
			}
			else if (!str_cmp(word, "Member"))
			{
				CLAN_MEMBER * member;
				char * name;
				int rank;
				time_t joined;

				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				name = fread_word(fp);
				rank = fread_number(fp);
				joined = fread_long_number(fp);

				member = add_to_clan(clan, name, rank);
				if (member)
					member->joined = joined;

				match = TRUE;
				break;
			}
			else if (!str_cmp(word, "Mkills"))
			{
				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				clan->mkills = fread_number(fp);
				match = TRUE;
				break;
			}
			else if (!str_cmp(word, "Motto"))
			{
				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				clan->motto = fread_string(fp);
				match = TRUE;
				break;
			}
			break;

		case 'N':
			if (!str_cmp(word, "Name"))
			{
				if (clan)
					free_clan(clan);

				clan = new_clan();
				clan->name = fread_string(fp);
				ranks = 0;
				match = TRUE;
				break;
			}
			break;

		case 'P':
			if (!str_cmp(word, "Pdeaths"))
			{
				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				clan->pdeaths = fread_number(fp);
				match = TRUE;
				break;
			}
			else if (!str_cmp(word, "Pkills"))
			{
				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				clan->pkills = fread_number(fp);
				match = TRUE;
				break;
			}

		case 'R':
			if (!str_cmp(word, "Rank"))
			{
				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				if (ranks >= CLAN_MAX_RANKS)
				{
					fread_to_eol(fp);
					break;
				}

				free_string( clan->rank_names[ranks] );
				clan->rank_names[ranks] = fread_string(fp);
				ranks++;
				match = TRUE;
				break;
			}
			break;

		case 'W':
			if (!str_cmp(word, "Who"))
			{
				int i;

				if (!clan)
				{
					fread_to_eol(fp);
					break;
				}

				clan->who_name = fread_string(fp);

				for (i = 0; clan->who_name[i] != '\0'; i++)
					clan->who_name[i] = UPPER( clan->who_name[ i ] );

				match = TRUE;
				break;
			}
			break;

		default:
			fread_to_eol(fp);
			bugf("Error while loading clans, word '%s' not matched", word);
			break;
		}

		if (!match)
			bugf("Error while loading clans, word '%s' not matched", word);
	}
	fclose(fp);
}

bool is_clan(CHAR_DATA *ch)
{
	if (!IS_NPC( ch ) && ch->pcdata->clan)
		return TRUE;

	return FALSE;
}

bool is_same_clan(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (IS_NPC( ch ) || IS_NPC( victim ))
		return FALSE;

	if (ch->pcdata->clan == victim->pcdata->clan && ch->pcdata->clan)
		return TRUE;

	if(clans_in_aliance(ch->pcdata->clan, victim->pcdata->clan))
		return TRUE;

	return FALSE;
}

void do_cstat(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA * clan;
	int counter = 1;
	int i;

	if (IS_NPC( ch ))
		return;

	if (argument[0] == '\0' || !str_cmp(argument, "all"))
	{
		send_to_char("Lista klanow:\n\r", ch);

		for (clan = clan_list; clan; clan = clan->next, counter++)
			print_char(ch, "%d) %-20.20s: %-5d members\n\r", counter,
					capitalize( clan->name ), count_members(clan));

		return;
	}
	else if ((clan = get_clan_by_name(argument)) != NULL)
	{
		print_char(ch, "Nazwa klanu:    %s\n\r", clan->name);
		print_char(ch, "Skrót klanu:    %s\n\r", clan->who_name);
		print_char(ch, "Motto klanu:    %s\n\r", clan->motto);
		print_char(ch, "Opis klanu: \n\r%s\n\r\n\r", clan->description);

		print_char(ch, "1. %s: %s\n\r", clan->rank_names[CLAN_RANK_OVERLORD],
				get_overlord(clan));
		print_char(ch, "2. %s: %s\n\r", clan->rank_names[CLAN_RANK_CHIEFTAIN],
				get_chieftain(clan));
		for (i = CLAN_RANK_CUSTOM; i < CLAN_MAX_RANKS && clan->rank_names[i]; i++)
			print_char(ch, "%d. %s ({C%d{x): %s\n\r", i, clan->rank_names[i],
					get_count_by_rank(clan, i), get_list_by_rank(clan, i, NULL));
		print_char(ch, "%d. %s ({C%d{x): %s\n\r\n\r", clan->ranks,
				clan->rank_names[CLAN_RANK_MEMBER], get_count_by_rank(clan,
						CLAN_RANK_MEMBER), get_list_by_rank(clan,
						CLAN_RANK_MEMBER, NULL));

		print_char(ch, "PKills:         %d\n\r", clan->pkills);
		print_char(ch, "PDeaths:        %d\n\r", clan->pdeaths);
		print_char(ch, "MKills:         %d\n\r", clan->mkills);
		print_char(ch, "Mdeaths:        %d\n\r", clan->mdeaths);

		CLAN_STANCE * stance;
		CLAN_DATA * state_clan;
		for (stance = clan->stances; stance; stance = stance->next)
		{
			state_clan = get_clan_by_id(stance->clan_id);
			if (state_clan == NULL)
			{
				continue;
			}
			switch (stance->clan_state)
			{
			default:
				break;
				//        		case CLANSTATE_WAR:
			case 1:
				print_char(ch, "%s WOJNA\n\r", state_clan->name);
				break;
			case 2:
				//        		case CLANSTATE_NEUTRAL:
				print_char(ch, "%s NEUTRALNY\n\r", state_clan->name);
				break;
			case 3:
				//        		case CLANSTATE_PEACE:
				print_char(ch, "%s POKÓJ\n\r", state_clan->name);
				break;
			case 4:
				//        		case CLANSTATE_ALIANCE:
				print_char(ch, "%s SOJUSZ\n\r", state_clan->name);
				break;
			}
		}
		return;
	}

	send_to_char("cstat - komenda s³u¿y do sprawdzania klanów\n\r", ch);
	send_to_char("cstat, cstat all - pokazuje listê wszystkich klanów.\n\r", ch);
	send_to_char("cstat nazwa_klanu - pokazuje dane konkretnego klanu.\n\r", ch);
	return;
}

void do_setclan(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA * clan;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	if (IS_NPC( ch ))
		return;

	argument = one_argument(argument, arg1);

	if (arg1[0] == '\0')
	{
		send_to_char("setclan <clan> name	<string>\n\r", ch);
		send_to_char("setclan <clan> motto	<string>\n\r", ch);
		send_to_char("setclan <clan> who	<string>\n\r", ch);
		send_to_char("setclan <clan> description\n\r", ch);
		send_to_char("setclan <clan> overlord	<string>\n\r", ch);
		//        send_to_char( "setclan <clan> chieftain	<string>\n\r", ch ); - niezaimplementowane
		send_to_char("setclan <clan> pkills	<value>\n\r", ch);
		send_to_char("setclan <clan> pdeaths	<value>\n\r", ch);
		send_to_char("setclan <clan> mkills	<value>\n\r", ch);
		send_to_char("setclan <clan> mdeaths	<value>\n\r", ch);
		send_to_char("setclan <clan> rank overlord  <string>\n\r", ch);
		send_to_char("setclan <clan> rank chieftain <string>\n\r", ch);
		send_to_char("setclan <clan> rank member    <string>\n\r", ch);
		send_to_char("setclan <clan> ranks <ranga1> <ranga2> <ranga3> ...\n\r",
				ch);
		send_to_char("setclan <clan> ranks delete \n\r", ch);
		send_to_char("setclan <clan> active\n\r", ch);
		send_to_char("setclan <clan> inactive\n\r", ch);
		send_to_char("setclan create <name>\n\r", ch);
		return;
	}

	if (!str_cmp(arg1, "create"))
	{
		CLAN_DATA * clan_last;

		if (argument[0] == '\0')
		{
			do_setclan(ch, "");
			return;
		}

		clan = new_clan();
		clan->name = str_dup(argument);

		if (!clan_list)
			clan_list = clan;
		else
		{
			for (clan_last = clan_list; clan_last->next; clan_last
					= clan_last->next)
				;

			clan_last->next = clan;
		}
		send_to_char("Nowy klan stworzony.\n\r", ch);
		save_clans();
		return;
	}

	argument = one_argument(argument, arg2);

	if ((clan = get_clan_by_name(arg1)) == NULL)
	{
		print_char(ch, "Klan '%s' nie istnieje.\n\r", arg1);
		return;
	}

	if (!str_prefix(arg2, "remove"))
	{
		clan_from_list(clan);
		send_to_char("Klan usuniêty.\n\r", ch);
		save_clans();
		return;
	}
	else if (!str_prefix(arg2, "active"))
	{
		clan->active = TRUE;
		send_to_char("Klan ustawiony jako aktywny.\n\r", ch);
		save_clans();
		return;
	}
	else if (!str_prefix(arg2, "inactive"))
	{
		clan->active = FALSE;
		send_to_char("Klan ustawiony jako nieaktywny.\n\r", ch);
		save_clans();
		return;
	}
	else if (!str_prefix(arg2, "description"))
	{
		string_append(ch, &clan->description);
		return;
	}
	else if (!str_prefix(arg2, "name"))
	{
		free_string( clan->name );
		clan->name = str_dup(argument);
		send_to_char("Done.\n\r", ch);
		save_clans();
		return;
	}
	else if (!str_prefix(arg2, "motto"))
	{
		free_string( clan->motto );
		clan->motto = str_dup(argument);
		send_to_char("Done.\n\r", ch);
		save_clans();
		return;
	}
	else if (!str_prefix(arg2, "who"))
	{
		free_string( clan->who_name );
		clan->who_name = str_dup(argument);
		send_to_char("Done.\n\r", ch);
		save_clans();
		return;
	}

	else if (!str_prefix(arg2, "overlord"))
	{
		CLAN_DATA * vclan;
		CHAR_DATA * victim, *overlord;
		CLAN_MEMBER * member;
		char * buf;

		victim = get_char_world(ch, argument);

		if (victim && IS_NPC( victim ))
		{
			send_to_char("Mob nie mo¿e byæ szefem klanu.\n\r", ch);
			return;
		}

		vclan = get_clan_by_member(argument);
		if (vclan && vclan != clan)
		{
			send_to_char("Ta osoba znajduje siê ju¿ w innym klanie.\n\r", ch);
			return;
		}

		buf = get_overlord(clan);

		if (!IS_NULLSTR( buf ))
		{
			set_clan_rank(clan, buf, CLAN_RANK_MEMBER);

			overlord = get_char_world(ch, buf);

			if (overlord)
				overlord->pcdata->clan_rank = CLAN_RANK_MEMBER;
		}

		member = get_member(clan, argument);

		if (member)
			set_clan_rank(clan, argument, CLAN_RANK_OVERLORD);
		else
			add_to_clan(clan, argument, CLAN_RANK_OVERLORD);

		if (victim)
		{
			victim->pcdata->clan = clan;
			victim->pcdata->clan_rank = CLAN_RANK_OVERLORD;
		}

		send_to_char("Done.\n\r", ch);
		save_clans();
		return;
	}
	else if (!str_prefix(arg2, "pkills"))
	{
		clan->pkills = atoi(argument);
		send_to_char("Done.\n\r", ch);
		save_clans();
		return;
	}
	else if (!str_prefix(arg2, "pdeaths"))
	{
		clan->pdeaths = atoi(argument);
		send_to_char("Done.\n\r", ch);
		save_clans();
		return;
	}
	else if (!str_prefix(arg2, "mkills"))
	{
		clan->mkills = atoi(argument);
		send_to_char("Done.\n\r", ch);
		save_clans();
		return;
	}
	else if (!str_prefix(arg2, "mdeaths"))
	{
		clan->mdeaths = atoi(argument);
		send_to_char("Done.\n\r", ch);
		save_clans();
		return;
	}
	else if (!str_prefix(arg2, "mdeaths"))
	{

		clan->mdeaths = atoi(argument);
		send_to_char("Done.\n\r", ch);
		save_clans();
		return;
	}
	else if (!str_cmp(arg2, "rank"))
	{
		argument = one_argument(argument, arg1);

		if (!str_prefix(arg1, "overlord"))
		{
			free_string( clan->rank_names[CLAN_RANK_OVERLORD] );
			clan->rank_names[CLAN_RANK_OVERLORD] = str_dup(argument);
			send_to_char("Ustawiono nazwê wodza.\n\r", ch);
			save_clans();
			return;
		}
		else if (!str_prefix(arg1, "chieftain"))
		{
			free_string( clan->rank_names[CLAN_RANK_CHIEFTAIN] );
			clan->rank_names[CLAN_RANK_CHIEFTAIN] = str_dup(argument);
			send_to_char("Ustawiono nazwê zastêpcy wodza.\n\r", ch);
			save_clans();
			return;
		}
		else if (!str_prefix(arg1, "member"))
		{
			free_string( clan->rank_names[CLAN_RANK_MEMBER] );
			clan->rank_names[CLAN_RANK_MEMBER] = str_dup(argument);
			send_to_char("Ustawiono nazwê zwyk³ego cz³onka.\n\r", ch);
			save_clans();
			return;
		}
		else
		{
			do_setclan(ch, "");
			return;
		}
	}
	else if (!str_cmp(arg2, "ranks"))
	{
		if (!str_cmp(argument, "delete"))
		{
			clean_ranks(clan);
			update_member_ranks(clan);
			send_to_char("Dodatkowe rangi usuniête.\n\r", ch);
			save_clans();
			return;
		}

		if (IS_NULLSTR( argument ))
		{
			send_to_char(
					"Podaj listê rang w postaci \"Ranga 1\" \"Ranga 2\" \"Ranga 3\" ...\n\r",
					ch);
			send_to_char(
					"W li¶cie nie uwzglêdniaj wodza, zastêpcy oraz zwyk³ego cz³onka.\n\r",
					ch);
			send_to_char(
					"Aby usun±æ wszystkie dodatkowe rangi wpisz 'ranks delete'.\n\r",
					ch);
			return;
		}

		clean_ranks(clan);
		argument = case_one_argument(argument, arg1);

		while (!IS_NULLSTR( arg1 ))
		{
			if (clan->ranks >= CLAN_MAX_RANKS)
			{
				print_char(ch, "W klanie mo¿e byæ maksymalnie %d rang.\n\r",
						CLAN_MAX_RANKS);
				break;
			}
			free_string( clan->rank_names[clan->ranks] );
			clan->rank_names[clan->ranks] = str_dup(arg1);
			argument = case_one_argument(argument, arg1);
			clan->ranks++;
		}
		update_member_ranks(clan);
		send_to_char("Dodatkowe rangi ustawione.\n\r", ch);
		save_clans();
		return;
	}
	else
	{
		do_setclan(ch, "");
		return;
	}
	return;
}

void do_claninfo(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA * clan;
	char buf[MAX_STRING_LENGTH];
	BUFFER *buffer;
	char * tmp;

	if (IS_NPC( ch ))
		return;

	if (argument[0] == '\0')
	{
		if (ch->pcdata->clan)
			clan = ch->pcdata->clan;
		else
		{
			send_to_char(
					"Informacje na temat jakiego klanu chcesz zobaczyæ (lista - clans)?\n\r",
					ch);
			return;
		}
	}
	else if ((clan = get_clan_by_name(argument)) == NULL)
	{
		print_char(ch, "Klan '%s' nie istnieje.\n\r", argument);
		return;
	}

	buffer = new_buf();

	sprintf(buf, "%s\n\r", capitalize( clan->name ));
	add_buf(buffer, buf);

	sprintf(buf, "Motto: \"%s\"\n\r", capitalize( clan->motto ));
	add_buf(buffer, buf);

	add_buf(
			buffer,
			"-----------------------------------------------------------------------------\n\r");

	sprintf(buf, "%s\n\r", clan->description);
	add_buf(buffer, buf);

	if (!clan->active)
	{
		add_buf(buffer, "\n\r{RKlan jest nieaktywny.{x\n\r");
		page_to_char(buf_string(buffer), ch);
		free_buf(buffer);
		return;
	}

	sprintf(
			buf,
			"Cz³onków tego klanu mo¿na rozpoznaæ po napisie {C%s{x na li¶cie graczy.\n\r\n\r",
			clan->who_name);
	add_buf(buffer, buf);

	tmp = get_overlord(clan);
	sprintf(buf, "%s: %s\n\r",
			capitalize( clan->rank_names[CLAN_RANK_OVERLORD] ),
			IS_NULLSTR( tmp ) ? "brak" : capitalize( tmp ));
	add_buf(buffer, buf);

	tmp = get_chieftain(clan);
	sprintf(buf, "%s: %s\n\r",
			capitalize( clan->rank_names[CLAN_RANK_CHIEFTAIN] ),
			IS_NULLSTR( tmp ) ? "brak" : capitalize( tmp ));
	add_buf(buffer, buf);

	if (ch->pcdata->clan == clan)
	{
		int i;

		add_buf(buffer, "\n\r");

		for (i = CLAN_RANK_CUSTOM; i < CLAN_MAX_RANKS && clan->rank_names[i]; i++)
		{
			sprintf(buf, "W randze %s ({C%d{x): %s\n\r", clan->rank_names[i],
					get_count_by_rank(clan, i), get_list_by_rank(clan, i, "{C"));
			add_buf(buffer, buf);
		}

		sprintf(buf, "W randze %s ({C%d{x): %s\n\r",
				clan->rank_names[CLAN_RANK_MEMBER], get_count_by_rank(clan,
						CLAN_RANK_MEMBER), get_list_by_rank(clan,
						CLAN_RANK_MEMBER, "{C"));
		add_buf(buffer, buf);
	}

	sprintf(buf, "Liczba wszystkich cz³onków:  %3d\n\r\n\r",
			count_members(clan));
	add_buf(buffer, buf);

	sprintf(buf, "Zabitych graczy:       %3d\n\r", clan->pkills);
	add_buf(buffer, buf);

	sprintf(buf, "¦mierci z r±k graczy:  %3d\n\r", clan->pdeaths);
	add_buf(buffer, buf);

	sprintf(buf, "Zabitych NPC:          %3d\n\r", clan->mkills);
	add_buf(buffer, buf);

	sprintf(buf, "¦mierci z r±k NPC:     %3d\n\r", clan->mdeaths);
	add_buf(buffer, buf);

	CLAN_STANCE * stance;
	CLAN_DATA * state_clan;
	for (stance = clan->stances; stance; stance = stance->next)
	{
		state_clan = get_clan_by_id(stance->clan_id);
		if (state_clan == NULL)
		{
			continue;
		}
		switch (stance->clan_state)
		{
		default:
			break;
			//        		case CLANSTATE_WAR: //Rysand - a u mnie siê to nie kompiluje...
		case 1:
			sprintf(buf, "%s WOJNA\n\r", state_clan->name);
			add_buf(buffer, buf);
			break;
		case 2:
			//        		case CLANSTATE_NEUTRAL:
			sprintf(buf, "%s NEUTRALNY\n\r", state_clan->name);
			add_buf(buffer, buf);
			break;
		case 3:
			//        		case CLANSTATE_PEACE:
			sprintf(buf, "%s POKÓJ\n\r", state_clan->name);
			add_buf(buffer, buf);
			break;
		case 4:
			//        		case CLANSTATE_ALIANCE:
			sprintf(buf, "%s SOJUSZ\n\r", state_clan->name);
			add_buf(buffer, buf);
			break;
		}
	}

	page_to_char(buf_string(buffer), ch);
	free_buf(buffer);
	return;
}

void do_clans(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA * clan;
	char buf[MAX_STRING_LENGTH];
	int found, counter = 1;
	BUFFER *buffer;
	char * tmp;

	if (IS_NPC( ch ))
		return;

	buffer = new_buf();

	for (clan = clan_list, found = 0; clan; clan = clan->next, counter++)
	{
		if (clan->active)
		{
			tmp = get_overlord(clan);
			sprintf(buf, "%d) %-45.45s  %-16.16s\n\r", counter, clan->name,
					IS_NULLSTR( tmp ) ? "brak" : capitalize( tmp ));
		}
		else
		{
			sprintf(buf, "%d) %-45.45s  %s\n\r", counter, clan->name,
					"{Rklan nieaktywny{x");
		}
		add_buf(buffer, buf);
		found++;
	}

	if (found == 0)
		send_to_char("Aktualnie nie ma ¿adnego klanu.\n\r", ch);
	else
	{
		send_to_char(
				"Klan                                              Przywódca\n\r",
				ch);
		sprintf(buf, "Liczba klanów: %d.\n\r", found);
		add_buf(buffer, buf);
		add_buf(buffer, "Szczegó³owe informacje - {Cclaninfo 1{x itd.\n\r");
		page_to_char(buf_string(buffer), ch);
	}

	free_buf(buffer);
	return;
}

//komenda do przyjmowania nowych czlonkow
void do_initiate(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA * victim;
	CLAN_DATA *clan;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC( ch ))
		return;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Kogo chcesz przyj±æ do klanu?\n\r", ch);
		return;
	}

	if ((victim = get_char_room(ch, arg)) == NULL)
	{
		send_to_char("Nie ma tu nikogo takiego.\n\r", ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("Kogo chcesz przyj±æ do klanu?\n\r", ch);
		return;
	}

	if (!is_clan(ch) || (ch->pcdata->clan_rank != CLAN_RANK_OVERLORD
			&& ch->pcdata->clan_rank != CLAN_RANK_CHIEFTAIN))
	{
		send_to_char("Nie masz wystarczaj±cej w³adzy ¿eby to zrobiæ.\n\r", ch);
		return;
	}

	clan = ch->pcdata->clan;

	if (is_clan(victim) || IS_NPC( victim ))
	{
		print_char(ch,
				"%s nie mo¿e zostaæ przyjêt<&y/a/e> do twojego klanu.\n\r",
				capitalize( victim->name ));
		print_char(victim, "Nie mo¿esz wst±pic w szeregi klanu '%s'.\n\r",
				capitalize( clan->name ));
		return;
	}

	add_to_clan(clan, victim->name, CLAN_RANK_MEMBER);
	victim->pcdata->clan = clan;
	victim->pcdata->clan_rank = CLAN_RANK_MEMBER;

	sprintf(buf, "Log %s: przyjmuje %s to %s", ch->name, victim->name2,
			clan->name);
	log_string(buf);

	if (victim->sex == 2)
	{
		act( "$n zostaje cz³onkini± klanu '$t'!", victim, clan->name, NULL, TO_ROOM );
		act( "Zosta³a¶ cz³onkiem klanu '$t'!", ch, clan->name, victim, TO_VICT );
	}
	else
	{
		act( "$n zostaje cz³onkiem klanu '$t'!", victim, clan->name, NULL, TO_ROOM );
		act( "Zosta³e¶ cz³onkiem klanu '$t'!", ch, clan->name, victim, TO_VICT );
	}
	save_char_obj(victim, FALSE, FALSE);
	save_clans();
	return;
}

void do_demote(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA * victim;
	CLAN_DATA * clan;
	CLAN_MEMBER * member;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC( ch ))
		return;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Kogo chcesz zdegradowaæ?\n\r", ch);
		return;
	}

	if (!is_clan(ch) || (ch->pcdata->clan_rank != CLAN_RANK_OVERLORD
			&& ch->pcdata->clan_rank != CLAN_RANK_CHIEFTAIN))
	{
		send_to_char("Nie mo¿esz tego zrobiæ.\n\r", ch);
		return;
	}

	clan = ch->pcdata->clan;

	victim = get_char_room(ch, arg);

	if ((member = get_member(clan, arg)) == NULL)
	{
		send_to_char("Nie ma nikogo takiego w twoim klanie.\n\r", ch);
		return;
	}

	if (is_name(arg, ch->ss_data ? ch->short_descr : ch->name))
	{
		send_to_char("Chcesz zrezygnowaæ z przywództwa?\n\r", ch);
		return;
	}

	if (member->rank == CLAN_RANK_OVERLORD)
	{
		send_to_char("Nie mo¿esz tego zrobiæ.\n\r", ch);
		return;
	}

	switch (member->rank)
	{
	default:
		if (member->rank < CLAN_MAX_RANKS - 1 && clan->rank_names[member->rank
				+ 1])
			set_clan_rank(clan, arg, member->rank + 1);
		else
			set_clan_rank(clan, arg, CLAN_RANK_MEMBER);
		break;
	case CLAN_RANK_MEMBER:
		send_to_char("Ni¿ej juz siê nie da.\n\r", ch);
		return;
		break;
	case CLAN_RANK_CHIEFTAIN:
		if (clan->rank_names[CLAN_RANK_CUSTOM])
			set_clan_rank(clan, arg, CLAN_RANK_CUSTOM);
		else
			set_clan_rank(clan, arg, CLAN_RANK_MEMBER);
		break;
	}

	member = get_member(clan, arg);

	sprintf(buf, "Log %s: degraduje %s [klan %s]", ch->name, arg, clan->name);
	log_string(buf);

	if (victim)
	{
		victim->pcdata->clan_rank = member->rank;

		sprintf(
				buf,
				"Przywódca klanu '%s', %s przemawia:\n\r\n\r"
					"'Degraduje ciê, %s,  do stopnia '%s'!!!'\n\r"
					"Byæ mo¿e w przysz³o¶ci udowodnisz swoimi czynami, ¿e staæ ciê na wiêcej!",
				clan->name, ch->name, victim->name,
				clan->rank_names[victim->pcdata->clan_rank]);

		send_to_char(buf, victim);
	}

	print_char(ch, "%s od tej pory bêdzie w stopniu '%s'.\n\r",
			capitalize( arg ), clan->rank_names[member->rank]);

	save_clans();
	return;
}

void do_promote(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA * victim;
	CLAN_DATA * clan;
	CLAN_MEMBER * member;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC( ch ))
		return;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Kogo chcesz awansowaæ?\n\r", ch);
		return;
	}

	if (!is_clan(ch) || (ch->pcdata->clan_rank != CLAN_RANK_OVERLORD
			&& ch->pcdata->clan_rank != CLAN_RANK_CHIEFTAIN))
	{
		send_to_char("Nie mo¿esz tego zrobiæ.\n\r", ch);
		return;
	}

	clan = ch->pcdata->clan;

	victim = get_char_room(ch, arg);

	if ((member = get_member(clan, arg)) == NULL)
	{
		send_to_char("Nie ma nikogo takiego w twoim klanie.\n\r", ch);
		return;
	}

	if (is_name(arg, ch->ss_data ? ch->short_descr : ch->name))
	{
		send_to_char("Chcesz zrezygnowaæ z przywództwa?\n\r", ch);
		return;
	}

	switch (member->rank)
	{
	default:
		set_clan_rank(clan, arg, member->rank - 1);
		break;

	case CLAN_RANK_MEMBER:
		if (clan->ranks > CLAN_RANK_CUSTOM)
		{
			set_clan_rank(clan, arg, clan->ranks - 1);
			break;
		}

	case CLAN_RANK_CUSTOM:
		if (get_chieftain(clan))
		{
			print_char(ch, "Mo¿e byæ tylko jeden %s.\n\r",
					clan->rank_names[CLAN_RANK_CHIEFTAIN]);
			return;
		}
		else if (count_members(clan) < 5)
		{
			print_char(
					ch,
					"Klan musi mieæ co najmniej 5 cz³onkow, zanim bêdziesz móg³ awansowaæ kogo¶\n\r"
						"do rangi %s.\n\r",
					clan->rank_names[CLAN_RANK_CHIEFTAIN]);
			return;
		}

		set_clan_rank(clan, arg, CLAN_RANK_CHIEFTAIN);
		break;

	case CLAN_RANK_CHIEFTAIN:
	case CLAN_RANK_OVERLORD:
		send_to_char("Nie mo¿na ju¿ dalej awansowaæ.\n\r", ch);
		return;
		break;
	}

	member = get_member(clan, arg);

	sprintf(buf, "Log %s: promoting %s to %s", ch->name, arg,
			ch->pcdata->clan->name);
	log_string(buf);

	if (victim)
	{
		victim->pcdata->clan_rank = member->rank;

		sprintf(buf, "Przywdca klanu '%s, %s przemawia:\n\r\n\r"
			"'Awansuje ciê, %s, do stopnia '%s'!!!'\n\r"
			"Przyjmij t± promocje jako nagrodê za swe zas³ugi dla klanu.\n\r"
			"%s\n\r", clan->name, ch->name, victim->name,
				clan->rank_names[victim->pcdata->clan_rank], clan->motto);

		send_to_char(buf, victim);
	}

	print_char(ch, "%s od tej pory bêdzie w stopniu '%s'.\n\r",
			capitalize( arg ), clan->rank_names[member->rank]);

	save_clans();
	return;
}

void do_exil(CHAR_DATA *ch, char *argument)
{
	if (IS_NPC( ch ))
		return;
	send_to_char(
			"Je¶li chcesz kogo¶ usun±æ z klanu, musisz wpisaæ komendê w ca³o¶ci.\n\r",
			ch);
	return;
}

void do_exile(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA * victim;
	CLAN_DATA * clan;
	CLAN_MEMBER * member;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char * v_name;

	if (IS_NPC( ch ))
		return;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Kogo chcesz usun±æ z klanu?\n\r", ch);
		return;
	}

	if (!is_clan(ch) || (ch->pcdata->clan_rank != CLAN_RANK_OVERLORD
			&& ch->pcdata->clan_rank != CLAN_RANK_CHIEFTAIN))
	{
		send_to_char("Nie mo¿esz tego zrobiæ.\n\r", ch);
		return;
	}

	clan = ch->pcdata->clan;

	victim = get_char_room(ch, arg);

	if (victim && victim == ch)
	{
		send_to_char("Chcesz opu¶ciæ swój klan? U¿yj komendy 'leave'.\n\r", ch);
		return;
	}

	if (victim)
		v_name = victim->name;
	else
		v_name = arg;

	if ((member = get_member(clan, v_name)) == NULL)
	{
		send_to_char("Nie ma nikogo takiego w klanie.\n\r", ch);
		return;
	}

	if (member->rank == CLAN_RANK_OVERLORD)
	{
		send_to_char("Nie mo¿esz tego zrobiæ.\n\r", ch);
		return;
	}

	sprintf(buf, "Log %s: exiling %s from %s", member->name, arg, clan->name);
	log_string(buf);

	sprintf(buf, "%s", member->name);

	remove_from_clan(clan, member);

	if (victim)
	{
		victim->pcdata->clan_rank = 0;
		victim->pcdata->clan = NULL;
		sprintf(
				buf,
				"Przywódca klanu '%s', %s przemawia:\n\r"
					"'Wyrzucam ciê, %s,  z naszego klanu!!!'\n\r"
					"Udowodni³e¶ wszystkim, i¿ przyjêcie Ciebie w nasze szeregi by³o b³êdem.\n\r",
				clan->name, ch->name, victim->name);
		send_to_char(buf, victim);
		save_char_obj(victim, FALSE, FALSE);
	}
	print_char(ch, "%s opuszcza szeregi klanu!\n\r", capitalize( buf ));
	save_clans();
	return;
}

void do_leav(CHAR_DATA *ch, char *argument)
{
	if (IS_NPC( ch ))
		return;
	send_to_char(
			"Je¶li chcesz odej¶æ z klanu, musisz wpisaæ ca³a komendê.\n\r", ch);
	return;
}

void do_leave(CHAR_DATA *ch, char *argument)
{
	CLAN_DATA * clan;

	if (IS_NPC( ch ))
		return;

	if (!is_clan(ch))
	{
		send_to_char("Nie jeste¶ w ¿adnym klanie.\n\r", ch);
		return;
	}

	clan = ch->pcdata->clan;

	if (argument[0] == '\0' || str_cmp(argument, "yes"))
	{
		send_to_char("Aby opu¶ciæ klan wpisz: 'leave yes'.\n\r", ch);
		return;
	}

	remove_from_clan(clan, get_member(clan, ch->name));
	ch->pcdata->clan_rank = 0;
	ch->pcdata->clan = NULL;
	print_char(ch,
			"Opuszczasz klan %s, rad¼ sobie teraz <&sam/sama/samo>.\n\r",
			clan->name);
	save_char_obj(ch, FALSE, FALSE);
	save_clans();
	return;
}

void do_clanstate(CHAR_DATA* ch, char* argument)
{
	CLAN_DATA * clanA; //klan który chce do³±czyæ do przymierza
	CLAN_DATA * clanB_ch; //klan zmieniaj±cego przymierze
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	if (IS_NPC( ch ))
		return;

	if (!is_clan(ch) || (ch->pcdata->clan_rank != CLAN_RANK_OVERLORD
			&& ch->pcdata->clan_rank != CLAN_RANK_CHIEFTAIN))
	{
		send_to_char("Nie mo¿esz tego zrobiæ.\n\r", ch);
		return;
	}

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if ((arg1[0] == '\0') || (arg2[0] == '\0'))
	{
		send_to_char("clanstate <clan> war\n\r", ch);
		send_to_char("clanstate <clan> neutral\n\r", ch);
		send_to_char("clanstate <clan> peace\n\r", ch);
		send_to_char("clanstate <clan> aliance\n\r", ch);
		return;
	}

	clanB_ch = ch->pcdata->clan;
	if ((clanA = get_clan_by_name(arg1)) == NULL)
	{
		print_char(ch, "Klan '%s' nie istnieje.\n\r", arg1);
		return;
	}

	if (clanA->id == clanB_ch->id)
	{
		print_char(ch, "To nie ma sensu.\n\r", arg1);
		return;
	}

	int state;
	if (!str_cmp(arg2, "war"))
	{
		state = CLANSTATE_WAR;
	}
	else if (!str_cmp(arg2, "neutral"))
	{
		state = CLANSTATE_NEUTRAL;
	}
	else if (!str_cmp(arg2, "peace"))
	{
		state = CLANSTATE_PEACE;
	}
	else if (!str_cmp(arg2, "aliance"))
	{
		state = CLANSTATE_ALIANCE;
	}
	else
	{
		send_to_char("Czy jeste¶ pewien, czego chcesz?\n\r", ch);
		return;
	}

	if (state == 0)
		return;

	CLAN_STANCE* stance;
	for (stance = clanB_ch->stances; stance; stance = stance->next)
	{
		if (stance->clan_id == clanA->id)
		{
			stance->clan_state = state;
			return;
		}
	}

	//else

	CREATE(stance, CLAN_STANCE, 1);

	stance->clan_id = clanA->id;
	stance->clan_state = state;

	if (clanB_ch->stances == NULL)
		clanB_ch->stances = stance;
	else
	{
		CLAN_STANCE* stance_next;
		stance_next = clanB_ch->stances;
		stance->next = stance_next;
		clanB_ch->stances = stance;
	}
}

//w sojuszu - A->B i B->A musz± mieæ ustawiony sojusz
bool clans_in_aliance(CLAN_DATA* clan, CLAN_DATA* clan_ch)
{
	if ((clan == NULL) || (clan_ch == NULL))
	{
		return FALSE;
	}

	CLAN_STANCE* stance;

	//sprawdzanie relacji A->B
	for (stance = clan->stances; stance; stance = stance->next)
	{
		if (stance->clan_id == clan_ch->id)
		{
			if (stance->clan_state != CLANSTATE_ALIANCE)
			{ //nie ma sensu dalej sprawdzaæ
				return FALSE;
				break;
			}
		}
	}
	//sprawdzanie relacji B->A
	for (stance = clan_ch->stances; stance; stance = stance->next)
	{
		if (stance->clan_id == clan->id)
		{
			if (stance->clan_state == CLANSTATE_ALIANCE)
				return TRUE;
			return FALSE;
		}
	}

	return FALSE;
}

