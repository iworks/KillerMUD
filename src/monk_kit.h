#ifndef _MONK_H_
#define _MONK_H_

#define MONK_HIT_FILE       "../system/monk_hits.txt"

typedef struct monk_hits MONK_HITS;

struct monk_hits
{
    char * 	hit_name;
    char *	monk_text;
    char *	victim_text;
    char *	room_text;
    int 	min_skill_req;
    int 	min_wisdom_req;
    int 	rating;
    int		damage[2];
    int 	dam_bonus;
    int		hit_bonus;
    int		victim_type;
    int		victim_parts;
    int		special_flag;
    MONK_HITS * next;
};

MONK_HITS * generate_hit( CHAR_DATA *ch, CHAR_DATA *victim );
void generate_monk_miss( CHAR_DATA * ch, CHAR_DATA *victim );
void generate_monk_dam_message(CHAR_DATA *ch, CHAR_DATA *victim, MONK_HITS * hit);
int  max_monk_hits( CHAR_DATA *ch );
#endif
