
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
#include "si.h"

bool check_chant_pray_affect(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);

struct si_spell_data si_spell_info[MAX_SPELL_INFO][MAX_SPELLS] =
{
    /* leczenia  - w ekstra max ile moze wyleczyc tak na oko*/
    {
	{ "cure light", 	0, 10, 15, check_undead, NULL },
	{ "aid", 			0, 20, 15, check_undead, NULL },
	{ "cure moderate", 	0, 35, 50, check_undead, NULL },
	{ "cure serious", 	0, 55, 35, check_undead, NULL },
	{ "cure critical", 	0, 85, 95, check_undead, NULL },
	{ "heal", 			0, 95,320, check_undead, NULL },
	{ NULL, 			0,  0, 0, NULL }
    },

    /* frags */
    {
	{ "acid blast", 	0, 65, 0, check_resist_acid, NULL },
	{ "burning hands", 	0, 40, 0, check_resist_fire, NULL },
	{ "cause critical", 0, 90, 0, check_undead, NULL },
	{ "cause light", 	0, 35, 0, check_undead, NULL },
	{ "cause serious", 	0, 65, 0, check_undead, NULL },
	{ "chain lightning",0, 85, 0, check_resist_lightning, NULL },
	{ "chill touch", 	0, 35, 0, check_resist_cold, NULL },
	{ "colour spray", 	0, 35, 0, NULL, NULL },
	{ "dispel evil", 	0, 85, 0, check_dispel_evil, NULL },
	{ "dispel good", 	0, 85, 0, check_dispel_good, NULL },
	{ "energy drain", 	0, 95, 0, NULL, NULL },
	{ "fireball",		0, 51, 0, NULL, NULL },
	{ "flamestrike", 	0, 79, 0, check_resist_fire, NULL },
	{ "harm", 			0, 95, 0, check_undead, NULL },
	{ "lightning bolt", 0, 69, 0, check_resist_lightning, NULL },
	{ "magic missile", 	0, 45, 0, NULL, NULL },
	{ "shocking grasp", 0, 56, 0, check_resist_lightning, NULL },
	{ "force bolt", 	0, 80, 0, NULL, NULL },
	{ "elemental devastation", 	0, 99, 0, NULL, NULL },
	{ "spray of thorns",0, 28, 0, NULL, NULL },
	{ "acid hands",		0, 50, 0, check_resist_acid, NULL },
	{ "fire darts",		0, 45, 0, check_resist_fire, NULL },
	{ "frost hands",	0, 45, 0, check_resist_cold, NULL },
	{ "cold snap",		0, 50, 0, check_resist_cold, NULL },
	{ "flame arrow",	0, 75, 0, check_resist_fire, NULL },
	{ "cone of cold",	0, 90, 0, check_resist_cold, NULL },
	{ "finger of death", 	0, 99, 0, check_undead, NULL },
	{ "gas breath", 	0, 99, 0, NULL, NULL },
	{ "lightning breath",0,99, 0, check_resist_lightning, NULL },
	{ "reverse gravity", 	0, 99, 0, NULL, NULL },
	{ "power word kill",0,100, 0, check_word_kill, NULL },
	{ "vampiric touch", 0, 84, 0, check_undead, NULL },
	{ "alicorn lance",	0, 50, 0, NULL, NULL },//DRUID
	{ "sunscorch",		0, 75, 0, check_resist_fire, NULL },//DRUID
	{ "call lightning",	0, 95, 0, check_call_lightning, NULL },//DRUID
	{ "freezing rain",	0, 75, 0, check_freezing_rain, NULL }, //DRUID
	{ "smashing wave",	0, 75, 0, NULL,	NULL }, //DRUID
	{ "burst of fire",	0, 30, 0, check_resist_fire,	NULL }, //DRUID
	{ "burst of flame",	0, 20, 0, check_resist_fire,	NULL }, //DRUID
	{ "force missiles",	0, 70, 0, NULL,	NULL },
	{ "thunder bolt",	0, 98, 0, check_resist_lightning,	NULL },
	{ "horrid wilting",	0, 98, 0, check_undead,	NULL },
	{ "decay",			0, 80, 0, check_undead,	NULL },
	{ "orb of entropy",	0, 98, 0, check_undead,	NULL },
	{ "ice bolt",		0, 77, 0, check_resist_cold,	NULL },//DRUID
	{ "ghoul touch",	0, 65, 0, check_undead,	NULL },
	{ "hellfire",		0, 40, 0, check_resist_fire,	NULL },//DRUID
	{ "frost rift",		0, 35, 0, check_resist_cold,	NULL },//DRUID
	{ "summon lesser meteor",0, 93, 0, check_meteor, NULL },
	{ "summon greater meteor",0, 98, 0, check_meteor, NULL },
	{ "poison",	0, 79, 0, check_affect, NULL },
	{ "plague",	0, 79, 0, check_affect, NULL },
	{ "cause moderate", 0, 55, 0, check_undead, NULL },
	{ "produce fire",	0, 45, 0, NULL, NULL },
	{ "lava bolt",		0, 80, 0, check_lava_bolt, NULL },
	{ "wind charger",	0, 80, 0, NULL,	NULL }, //DRUID
	{ "wood master",	0, 98, 0, check_wood_master,	NULL }, //DRUID
	{ "stone master",	0, 98, 0, check_stone_master,	NULL }, //DRUID
	{ "acid arrow",		0, 55, 0, check_resist_acid,	NULL },
	{ "heat metal",		0, 78, 0, check_x_metal, NULL },
	{ "chill metal",	0, 78, 0, check_x_metal, NULL },
	{ "banshee`s howl",	0, 82, 0, NULL, NULL },
	{ "psionic blast",	0, 99, 0, check_psionic, NULL },
	{ "flame lace",		0, 55, 0, check_resist_fire,	NULL },
	{ "mind strike",     0, 30, 0, check_psionic, NULL },
	{ "mind blast",      0, 60, 0, check_psionic, NULL },
	/* na summony, w pewnym sensie to są fragi za 1e10 dmg ;-) */
        { NULL, 		0,  0, 0, NULL, NULL }
    },

    /* def antitank self */
    {
	{ "spiritual armor", 	0, 85, 0, check_sp_armor, NULL },
	{ "spiritual hammer", 	0, 85, 0, check_sp_hammer, NULL },
	{ "flame blade", 	0, 90, 0, check_sp_hammer, NULL },
	{ "holy weapons", 	0, 95, 0, check_holy_wea, NULL },
	{ "giant strength", 	0, 25, 0, check_affect, NULL },
	{ "regenerate", 	0, 45, 0, check_affect, NULL },
	{ "haste", 		0, 15, 0, check_affect, NULL },
	{ "shield", 		0, 25, 0, check_affect, NULL },
	{ "blur",		0, 85, 0, check_affect, NULL },
	{ "bark skin", 		0, 75, 0, check_affect, NULL },
	{ "immolate", 		0, 75, 0, check_affect, NULL },
	{ "stone skin", 	0, 95, 0, check_affect, NULL },
	{ "mirror image", 	0, 90, 0, check_affect, NULL },
	{ "resist normal weapon",0, 94, 0, check_affect, NULL },
	{ "resist magic weapon",0, 94, 0, check_affect, NULL },
	{ "resist weapon", 0, 100, 0, check_affect, NULL },
	{ "mantle", 0, 95, 0, check_affect, NULL },
	{ "fireshield", 	0, 95, 0, check_fire_shield, NULL },
	{ "iceshield", 		0, 95, 0, check_ice_shield, NULL },
	{ "chant",		0, 75, 0, check_chant_pray_affect, NULL },
	{ "prayer",		0, 75, 0, check_chant_pray_affect, NULL },
	{ "ethereal armor", 	0, 95, 0, check_affect, NULL },
	{ "divine favor",	0, 85, 0, check_affect, NULL },
	{ "divine power",	0, 99, 0, check_affect, NULL },
	{ "blade barrier",	0, 99, 0, check_affect, NULL },
	{ "shield of nature",	0, 99, 0, check_affect, NULL },
	{ "wind shield",	0, 75, 0, check_affect, NULL },
	{ "spirit armor",	0, 65, 0, check_affect, NULL },
	{ "razorblade hands", 	0, 10, 0, check_razorblade, NULL },//by rzucal na koncu
#ifdef STEEL_SKIN_ON
//Nil: steel_skin
	{ "steel skin",0, 85, 0, check_affect, NULL },
#endif /* STEEL_SKIN_ON */
	{ "beast claws", 	0, 10, 0, check_razorblade, NULL },//by rzucal na koncu
	{ "deflect wounds",	0, 90, 0, check_affect, NULL },
	{ "blink", 			0, 70, 0, check_affect, NULL },
	{ "confusion shell",0, 70, 0, check_affect, NULL },
	{ "perfect self", 	0, 70, 0, check_affect, NULL },
	{ "stability", 		0, 74, 0, check_affect, NULL },
	{ "defense curl", 	0, 35, 0, check_affect, NULL },
	{ NULL, 		0,  0, 0, NULL, 	NULL }
    },

/* def antitank other */
    {
	{ "bless",			0, 15, 0, check_affect, NULL },
	{ "regenerate", 	0, 75, 0, check_affect, NULL },
	{ "haste", 			0, 75, 0, check_affect, NULL },
	{ "blur",			0, 85, 0, check_affect, NULL },
	{ "bark skin", 		0, 75, 0, check_affect, NULL },
	{ "chant",			0, 75, 0, check_chant_pray_affect, NULL },
	{ "prayer",			0, 75, 0, check_chant_pray_affect, NULL },
	{ "armor",			0, 20, 0, check_affect, NULL },
	{ "eyes of the torturer", 0, 90, 0, check_affect, NULL },
	{ "major haste",	0, 80, 0, check_affect, NULL },
	{ "strength"	,	0, 40, 0, check_affect, NULL },
	{ "giant strength"	,	0, 65, 0, check_affect, NULL },
	{ "luck",			0, 15, 0, check_affect, NULL },
	{ "champion strength"	,	0, 65, 0, check_affect, NULL },
	{ "unholy fury", 	0, 79, 0, check_unholy, NULL },
	{ NULL, 		0,  0, 0, NULL, NULL }
    },

/* antitank */
    {
	{ "blindness",				0, 65, 0, check_affect, NULL },
	{ "flare",				0, 65, 0, check_affect, NULL },
        { "slow", 				0, 45, 0, check_affect, NULL },
	{ "weaken", 				0, 65, 0, check_affect, NULL },
	{ "hold person", 			0, 85, 0, check_hold, NULL },
	{ "hold monster", 			0, 85, 0, check_hold, NULL },
	{ "hold animal", 			0, 85, 0, check_hold, NULL },
	{ "hold undead",			0, 85, 0, check_hold, NULL },
	{ "hold plant",				0, 85, 0, check_hold, NULL },
	{ "hold evil", 				0, 85, 0, check_hold, NULL },
	{ "charm person", 			0, 99, 0, check_affect, stop_fight_after_charm },
	{ "web",					0, 75, 0, check_affect, NULL },
	{ "curse",					0, 65, 0, check_affect, NULL },
	{ "increase wounds",		0, 75, 0, check_affect, NULL },
	{ "ray of enfeeblement",	0, 85, 0, check_affect, NULL },
	{ "power word blindness",	0, 80, 0, check_word_blind, NULL },
	{ "power word stun",		0, 80, 0, check_word_stun, NULL },
	{ "daze",					0, 80, 0, check_affect, NULL },
	{ "bane",					0, 50, 0, check_affect, NULL },
	{ "dispel magic",			0, 65, 0, check_si_dispel, NULL },
	{ "great dispel magic",		0,100, 0, check_si_dispel, NULL },
	{ "puppet master", 			0, 82, 0, check_si_puppet_master, stop_fight_after_charm },
	{ "repayment", 				0, 82, 0, check_si_puppet_master, NULL },
	{ "loop", 					0, 65, 0, check_affect, NULL },
	{ "maze", 					0, 65, 0, check_affect, NULL },
	{ "hallucinations", 		0, 65, 0, check_affect, NULL },
	{ "calm",			 		0, 65, 0, check_affect, NULL },
	{ "fear",			 		0, 65, 0, check_affect, NULL },
	{ "domination", 			0,100, 0, check_affect, stop_fight_after_charm },
	{ "exile",		 			0, 89, 0, check_dismiss, NULL },
	{ "pyrotechnics", 			0, 50, 0, check_affect, NULL },
	{ "entangle",	 			0, 66, 0, check_affect, NULL },
	{ NULL, 					0,  0, 0, check_affect, NULL }
    },

    /* anticaster */
    {
	{ "silence",				0, 95, 0, check_affect, NULL },
	{ "sleep", 					0, 85, 0, check_affect, NULL },
	{ "hold person", 			0, 85, 0, check_hold, NULL },
	{ "hold monster", 			0, 85, 0, check_hold, NULL },
	{ "hold animal", 			0, 85, 0, check_hold, NULL },
	{ "hold undead",			0, 85, 0, check_hold, NULL },
	{ "hold plant",				0, 85, 0, check_hold, NULL },
	{ "hold evil", 				0, 85, 0, check_hold, NULL },
	{ "charm person", 			0, 99, 0, check_affect, stop_fight_after_charm },
	{ "deafness", 				0, 65, 0, check_affect, NULL },
	{ "curse",					0, 35, 0, check_affect, NULL },
	{ "power word stun",		0,80, 0, check_word_stun, NULL },
	{ "feeblemind",				0,100,0,  check_affect, NULL },
	{ "daze",					0, 80, 0, check_affect, NULL },
	{ "confusion",				0, 75, 0, check_affect, NULL },
	{ "dispel magic",			0, 65, 0, check_si_dispel, NULL },
	{ "great dispel magic",		0,100, 0, check_si_dispel, NULL },
	{ "antimagic manacles",		0, 95, 0, check_affect, NULL },
	{ "loop", 					0, 80, 0, check_affect, NULL },
	{ "maze", 					0, 65, 0, check_affect, NULL },
	{ "hallucinations", 		0, 65, 0, check_affect, NULL },
	{ "fear",			 		0, 65, 0, check_affect, NULL },
	{ "domination", 			0,100, 0, check_affect, stop_fight_after_charm },
	{ "exile",		 			0, 89, 0, check_dismiss, NULL },
	{ "pyrotechnics", 			0, 50, 0, check_affect, NULL },
	{ NULL, 					0,  0, 0, check_affect, NULL }
    },

    /* def antimagic self */
    {
	{ "shield", 		0, 25, 0, check_affect, NULL },
	{ "resist elements",	0, 93, 0, check_affect, NULL },
	{ "resist fire",	0, 83, 0, check_affect, NULL },
	{ "resist cold",	0, 83, 0, check_affect, NULL },
	{ "resist acid",	0, 83, 0, check_affect, NULL },
	{ "resist poison",	0, 83, 0, check_affect, NULL },
	{ "energy shield",	0, 80, 0, check_affect, NULL },
	{ "mirror image", 	0, 90, 0, check_affect, NULL },
	{ "regenerate", 	0, 45, 0, check_affect, NULL },
	{ "reflect spell I",	0, 75, 0, check_affect, NULL },
	{ "reflect spell II",	0, 85, 0, check_affect, NULL },
	{ "reflect spell III",	0, 99, 0, check_affect, NULL },
	{ "globe of invulnerability",	0, 95, 0, check_affect, NULL },
	{ "minor globe of invulnerability",	0, 90, 0, check_affect, NULL },
	{ "major globe of invulnerability",	0, 99, 0, check_affect, NULL },
	{ "endure cold", 0, 30, 0, check_affect, NULL }, //druid
	{ "endure acid", 0, 30, 0, check_affect, NULL }, //druid
	{ "endure lightning", 0, 30, 0, check_affect, NULL }, //druid
	{ "endure fire", 0, 30, 0, check_affect, NULL }, //druid
	{ "absolute magic protection", 0, 100, 0, check_affect, NULL },
	{ "blink", 				0, 70, 0, check_affect, NULL },
	{ "divine shield", 		0, 40, 0, check_affect, NULL },
	{ "perfect senses", 	0, 40, 0, check_affect, NULL },
	{ "free action",		0, 50, 0, check_affect, NULL },
	{ "fortitude",			0, 40, 0, check_affect, NULL },
	{ NULL, 		0,  0, 0, check_affect, NULL }
    },

    /* def antimagic other */
    {
	{ "resist elements",0, 93, 0, check_affect, NULL },
	{ "resist fire",	0, 83, 0, check_affect, NULL },
	{ "resist cold",	0, 83, 0, check_affect, NULL },
	{ "resist acid",	0, 83, 0, check_affect, NULL },
	{ "resist poison",	0, 83, 0, check_affect, NULL },
	{ "energy shield",	0, 80, 0, check_affect, NULL },
	{ "regenerate", 	0, 45, 0, check_affect, NULL },
	{ "perfect senses", 0, 40, 0, check_affect, NULL },
	{ "free action",	0, 50, 0, check_affect, NULL },
	{ "fortitude",		0, 40, 0, check_affect, NULL },
	{ NULL, 			0,  0, 0, NULL, NULL }
    },

    /* savelife */
    {
	{ "invisibility",	0, 75, 0, check_affect, NULL },
	{ "mass invis",		0, 85, 0, check_affect, NULL },
	{ "cure critical",	0, 65, 0, NULL, NULL },
	{ "heal",			0, 85, 0, NULL, NULL },
	{ NULL, 			0,  0, 0, NULL, NULL }
    },

    /* nasty */
    {
    { "summon",				0, 92, 0, NULL, NULL },
	{ "dimension door",		0, 92, 0, NULL,	NULL },
	{ NULL, 				0,  0, 0, NULL, NULL }
    },

    /* calling */
    {
    { "summon animal",			0, 40, 0, NULL,	NULL },
    { "summon insect",			0, 30, 0, NULL, NULL },
    { "summon greenskin",		0, 40, 0, NULL, NULL },
    { "summon flying creature",	0, 55, 0, NULL, NULL },
    { "summon strong creature",	0, 70, 0, NULL, NULL },
    { "summon ancient creatrue",0, 85, 0, NULL, NULL },
    { "nature ally i", 0, 10, 0, NULL, NULL },
    { "nature ally ii", 0, 20, 0, NULL, NULL },
    { "nature ally iii", 0, 30, 0, NULL, NULL },
    { "nature ally iv", 0, 70, 0, NULL, NULL },
	{ NULL, 				0,  0, 0, NULL, NULL }
	}
};

bool check_dismiss_part(CHAR_DATA *victim, int RACE )
{
   if ( ! IS_SET( race_table[ victim->race ].type, RACE ) ) return TRUE;
   if ( ! EXT_IS_SET( victim->act, ACT_NO_EXP ) ) return TRUE;
   if ( ! IS_NPC( victim ) ) return TRUE;

   return FALSE; // czaruje
}

bool check_dism_anim(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
   return check_dismiss_part( victim, ANIMAL );
}

bool check_dism_plant(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
   return check_dismiss_part( victim, PLANT );
}

bool check_dism_person(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
   return check_dismiss_part( victim, PERSON );
}

bool check_dism_monster(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
   return check_dismiss_part( victim, MONSTER );
}

bool check_dism_undead(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
   return check_dismiss_part( victim, UNDEAD );
}

bool check_dism_insect(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
   return check_dismiss_part( victim, INSECT );
}



/* specjalne sprawdzanie affectow z zaklec*/
bool check_affect(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
sh_int sn;

    if(type < 0 || type > MAX_SPELL_INFO)
	return FALSE;

    sn = si_spell_info[type][spell].sn;


    switch(type)
    {
	case DEF_ANTITANK_SELF:
	case DEF_ANTIMAGIC_SELF:
				return is_affected(ch, sn);
	case DEF_ANTITANK_OTHER:
	case DEF_ANTIMAGIC_OTHER:
	case OFF_ANTITANK:
	case OFF_ANTICASTER:
	case FRAGS://dotyczy poisona
				return is_affected(victim, sn);
	default: return FALSE;
    }

return FALSE;
}


/* specjalne sprawdzanie affectow z zaklec*/
bool check_chant_pray_affect(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
sh_int sn;

    if( !ch->fighting )
	return TRUE;

    if(type < 0 || type > MAX_SPELL_INFO)
	return TRUE;

    sn = si_spell_info[type][spell].sn;


    switch(type)
    {
	case DEF_ANTITANK_SELF:
	case DEF_ANTIMAGIC_SELF:
				return is_affected(ch, sn);
	case DEF_ANTITANK_OTHER:
	case DEF_ANTIMAGIC_OTHER:
	case OFF_ANTITANK:
	case OFF_ANTICASTER:
				return is_affected(victim, sn);
	default: return FALSE;
    }

return FALSE;
}

/* specjalne sprawdzanie affectow z zaklec*/
bool check_hold(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
sh_int sn;

    if( type < 0 || type > MAX_SPELL_INFO )
	return FALSE;

    sn = si_spell_info[type][spell].sn;

    //zeby nie wrzucac holda na wardancerow i berserkierow
    if( IS_AFFECTED( victim, AFF_FREE_ACTION ))
	return TRUE;

    if( check_affect( ch, victim, type, spell) )
	return TRUE;

	if( sn == gsn_holdevil )
	return !IS_EVIL(victim);
    else if( sn == gsn_holdperson )
	return !IS_SET(race_table[GET_RACE(victim)].type, PERSON);
    else if ( sn == gsn_holdmonster )
	return !IS_SET(race_table[GET_RACE(victim)].type, MONSTER);
    else if ( sn == gsn_holdplant )
	return !IS_SET(race_table[GET_RACE(victim)].type, PLANT);
    else if ( sn == gsn_holdanimal )
	return !IS_SET(race_table[GET_RACE(victim)].type, ANIMAL);
    else
	return !is_undead(victim);

return FALSE;
}

extern EVENT_DATA * event_first;

bool check_call_lightning(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    sh_int sn;
    EVENT_DATA *event = NULL;

    if( type < 0 || type > MAX_SPELL_INFO )
	return TRUE;

    sn = si_spell_info[type][spell].sn;

    if ( !IS_OUTSIDE(ch) ||
    IS_SET(sector_table[ch->in_room->sector_type].flag, SECT_NOWEATHER))
	return TRUE;

    for( event = event_first; event; event = event->next )
	if( event->type == EVENT_CALL_LIGHTNING &&
	    (CHAR_DATA *)event->arg1 == ch )
		return TRUE;


return FALSE;
}


bool check_lava_bolt(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    sh_int sn;
    EVENT_DATA *event = NULL;

    if( type < 0 || type > MAX_SPELL_INFO )
	return TRUE;

    sn = si_spell_info[type][spell].sn;

    for( event = event_first; event; event = event->next )
	if( event->type == EVENT_LAVA &&
	    (CHAR_DATA *)event->arg1 == ch )
		return TRUE;


return FALSE;
}

bool check_meteor(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    sh_int sn;
    EVENT_DATA *event = NULL;

    if( type < 0 || type > MAX_SPELL_INFO )
	return TRUE;

    sn = si_spell_info[type][spell].sn;

    if ( !IS_OUTSIDE(ch) ||
    IS_SET(sector_table[ch->in_room->sector_type].flag, SECT_NOWEATHER))
	return TRUE;

    for( event = event_first; event; event = event->next )
	if( (event->type == EVENT_SUMMON_LESSER_METEOR && (CHAR_DATA *)event->arg1 == ch ) ||
	(event->type == EVENT_SUMMON_GREATER_METEOR && (CHAR_DATA *)event->arg1 == ch ))
		return TRUE;


return FALSE;
}

/* cure&cause check */
bool check_undead(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    if ( !is_undead(victim))
		return FALSE;

    return TRUE;
}

//unholy fury
bool check_unholy(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	//TRUE to znaczy ze spela nie rzuci
    if ( is_affected(victim, gsn_unholy_fury) )
		return TRUE;

    if ( !is_undead(victim) || ch == victim )
    	return TRUE;

    return FALSE;
}

/* dispel good check */
bool check_dispel_good(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    if(ch->alignment < -750 && victim->alignment > 750)
	return FALSE;

    return TRUE;
}


bool check_dispel_evil(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    if(ch->alignment > 750 && victim->alignment < -750)
	return FALSE;

    return TRUE;
}

bool check_sp_armor(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    /* tylko dla personow */
    if(!IS_SET(race_table[GET_RACE(ch)].type, PERSON))
	return TRUE;

    if( !get_eq_char(ch,WEAR_HEAD) ||
        !get_eq_char(ch,WEAR_BODY) ||
	!get_eq_char(ch,WEAR_LEGS) ||
	!get_eq_char(ch,WEAR_ARMS) ||
	!get_eq_char(ch,WEAR_HANDS) ||
	!get_eq_char(ch,WEAR_FEET))
	    return FALSE;

    return TRUE;
}

bool check_sp_hammer(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    if(get_eq_char(ch,WEAR_WIELD))
	return TRUE;

    return FALSE;
}

bool check_razorblade(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	if(get_eq_char(ch,WEAR_WIELD))
		return TRUE;

	if(get_eq_char(ch,WEAR_HOLD))
		return TRUE;

	if(get_eq_char(ch,WEAR_SECOND))
		return TRUE;

	if(get_eq_char(ch,WEAR_HANDS))
		return TRUE;

	if(get_eq_char(ch,WEAR_WRIST_R))
		return TRUE;

	if(get_eq_char(ch,WEAR_WRIST_L))
		return TRUE;

	if(get_eq_char(ch,WEAR_LIGHT))
		return TRUE;

	if(get_eq_char(ch,WEAR_SHIELD))
		return TRUE;

	if(get_eq_char(ch,WEAR_INSTRUMENT))
		return TRUE;


    return FALSE;
}

bool check_holy_wea(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	if(get_eq_char(ch,WEAR_WIELD))
		return TRUE;

	if(get_eq_char(ch,WEAR_HOLD))
		return TRUE;

	if(get_eq_char(ch,WEAR_SECOND))
		return TRUE;

	if(get_eq_char(ch,WEAR_LIGHT))
		return TRUE;

	if(get_eq_char(ch,WEAR_SHIELD))
		return TRUE;

	if(get_eq_char(ch,WEAR_INSTRUMENT))
		return TRUE;

    return FALSE;
}

bool check_word_stun(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    if( 100*victim->hit/UMAX(get_max_hp(victim), 1) > 39 )
	return TRUE;

    return FALSE;
}

bool check_word_blind(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    if( IS_AFFECTED(victim, AFF_BLIND) ||  100*victim->hit/UMAX(get_max_hp(victim), 1) > 39 )
	return TRUE;

    return FALSE;
}

bool check_word_kill(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    if( (!IS_NPC(victim) && IS_IMMORTAL(victim)) || 100*victim->hit/UMAX(get_max_hp(victim), 1) > 24 )
	return TRUE;

    return FALSE;
}


void stop_fight_after_charm(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if(IS_AFFECTED(victim, AFF_CHARM))
    {
	victim->fighting = NULL;
	victim->position = POS_STANDING;
	update_pos( victim );

	if(ch->fighting == victim)
	{
	    ch->fighting = NULL;
	    ch->position = POS_STANDING;
	    update_pos( ch );
	    forget(ch, victim, MEM_HATE, FALSE);
	}
    }
    return;
}

/***********************/
/***********************/

void si_setup_spell_data()
{
 int sn, type, x;

    for(sn=1; sn < MAX_SKILL; sn++)
    {
	if (skill_table[sn].name == NULL)
	    break;

	if (skill_table[sn].spell_fun == spell_null)
	    continue;

	for(type=0; type < MAX_SPELL_INFO; type++)
	    for(x=0; si_spell_info[type][x].name; x++)
		if(skill_table[sn].name[0] == si_spell_info[type][x].name[0] &&
		    !str_prefix(skill_table[sn].name, si_spell_info[type][x].name))
			si_spell_info[type][x].sn=sn;
    }

    return;
}


bool check_fire_shield(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    if( is_affected(ch, gsn_iceshield) )
	return TRUE;

    return is_affected(ch, gsn_fireshield);
}

bool check_ice_shield(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    if( is_affected(ch, gsn_fireshield) )
	return TRUE;

    return is_affected(ch, gsn_iceshield);
}

bool check_freezing_rain(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
    sh_int sn;

	if( type < 0 || type > MAX_SPELL_INFO )
		return TRUE;

    sn = si_spell_info[type][spell].sn;

    if ( !IS_OUTSIDE(ch) || IS_SET(sector_table[ch->in_room->sector_type].flag, SECT_NOWEATHER))
		return TRUE;

	return FALSE;
}

bool check_si_reflect_spell( CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell )
{
    AFFECT_DATA * paf, *reflect = NULL;
    sh_int sn = si_spell_info[type][spell].sn;
    sh_int circle = spell_circle( ch, sn );

    if ( !IS_AFFECTED( victim, AFF_REFLECT_SPELL ) )
        return FALSE;

    for ( paf = victim->affected; paf != NULL; paf = paf->next )
        if ( paf->bitvector == &AFF_REFLECT_SPELL )
        {
            reflect = paf;
            break;
        }

    if ( !reflect )
        return FALSE;

    /* ok to jesli jest to na pewno odbije
    * jesli modifier < 0 to jest na stale
    * w przeciwnym razie zmniejszamy modifier o circle
    */

    circle = UMAX( 0, circle );

    /* na stale */
    if ( reflect->modifier < circle )
    {
        send_to_char( skill_table[ gsn_reflect_spell ].msg_off, victim );
        send_to_char( "\n\r", victim );
        affect_remove( victim, reflect );
        return FALSE;
    }
    else
    {
        reflect->modifier -= circle;

        if ( reflect->modifier <= 0 )
        {
            send_to_char( skill_table[ gsn_reflect_spell ].msg_off, victim );
            send_to_char( "\n\r", victim );
            affect_remove( victim, reflect );
        }
    }
    return TRUE;
}

bool check_si_dispel(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	//TRUE to znaczy ze dispela nie rzuci
	int count = 0;

    if( type < 0 || type > MAX_SPELL_INFO )
		return FALSE;

	//po pierwsze i najwazniejsze, coby ludziom mob nie zdejmowal holdow, blindow itd
	if( IS_AFFECTED(victim, AFF_BLIND )) return TRUE;
	if( IS_AFFECTED(victim, AFF_PARALYZE )) return TRUE;
	if( IS_AFFECTED(victim, AFF_POISON )) return TRUE;
	if( IS_AFFECTED(victim, AFF_PLAGUE )) return TRUE;
	if( IS_AFFECTED(victim, AFF_CALM )) return TRUE;
	if( IS_AFFECTED(victim, AFF_DAZE )) return TRUE;
	if( IS_AFFECTED(victim, AFF_SLOW )) return TRUE;
	if( IS_AFFECTED(victim, AFF_CONFUSION )) return TRUE;
	if( IS_AFFECTED(victim, AFF_DEAFNESS )) return TRUE;
	if( IS_AFFECTED(victim, AFF_FEAR )) return TRUE;
	if( IS_AFFECTED(victim, AFF_INCREASE_WOUNDS )) return TRUE;
	if( IS_AFFECTED(victim, AFF_SILENCE )) return TRUE;
	if( IS_AFFECTED(victim, AFF_LOOP )) return TRUE;
	if( is_affected(victim, gsn_feeblemind )) return TRUE;
	if( is_affected(victim, gsn_mental_barrier )) return TRUE;
	if( is_affected(victim, gsn_antimagic_manacles )) return TRUE;

	//sprawdza kilka powerniejszych affectow, ktore wystarczy, ze bedzie jeden idzie dispel
	if( is_affected(victim, 204 )) return FALSE;//minor globe
	if( is_affected(victim, 205 )) return FALSE;//globe
	if( is_affected(victim, 366 )) return FALSE;//major globe
	if( is_affected(victim, 176 )) return FALSE;//r n w
	if( is_affected(victim, 177 )) return FALSE;//r m w
	if( is_affected(victim, 364 )) return FALSE;//r w
	if( is_affected(victim, 58 )) return FALSE;//giant strenght
	if( is_affected(victim, 487 )) return FALSE;// nimbleness
	if( is_affected(victim, 488 )) return FALSE;// draconic wisdom
	if( is_affected(victim, 489 )) return FALSE;// insight
	if( is_affected(victim, 490 )) return FALSE;// behemot toughness
	if( is_affected(victim, 178 )) return FALSE;//resist elements
	if( is_affected(victim, 246 )) return FALSE;//storm shell
	if( is_affected(victim, 60 )) return FALSE;//haste
	if( is_affected(victim, 161 )) return FALSE;//free action
	if( is_affected(victim, 306 )) return FALSE;//eyes of the torturer
	if( is_affected(victim, 393 )) return FALSE;//deflect wounds
	if( is_affected(victim, 528 )) return FALSE;//breath of life
	if( is_affected(victim, gsn_regenerate )) return FALSE;
	if( is_affected(victim, gsn_mirror_image )) return FALSE;
	if( is_affected(victim, gsn_stone_skin )) return FALSE;
	if( is_affected(victim, gsn_blade_barrier )) return FALSE;
	if( is_affected(victim, gsn_shield_of_nature )) return FALSE;
	if( is_affected(victim, gsn_divine_power )) return FALSE;
	if( is_affected(victim, gsn_blink )) return FALSE;
	if( is_affected(victim, gsn_unholy_fury )) return FALSE;

	//i teraz spelle co trzeba miec conajmniej dwa by rzucil
	if( is_affected(victim, gsn_invis )) ++count;
	if( is_affected(victim, gsn_fly )) ++count;
	if( is_affected(victim, gsn_float )) ++count;
	if( is_affected(victim, gsn_undead_invis )) ++count;
	if( is_affected(victim, gsn_resist_fire )) ++count;
	if( is_affected(victim, gsn_resist_cold )) ++count;
	if( is_affected(victim, gsn_resist_lightning )) ++count;
	if( is_affected(victim, 175 )) ++count;//res aci
	if( is_affected(victim, gsn_energy_shield )) ++count;
	if( is_affected(victim, gsn_resist_magic )) ++count;
	if( is_affected(victim, gsn_brave_cloak )) ++count;
	if( is_affected(victim, gsn_bark_skin )) ++count;
#ifdef STEEL_SKIN_ON
//Nil: steel_skin
	if( is_affected(victim, gsn_steel_skin )) ++count;
#endif /* STEEL_SKIN_ON */
	if( is_affected(victim, gsn_iceshield )) ++count;
	if( is_affected(victim, gsn_fireshield )) ++count;
	if( is_affected(victim, gsn_armor )) ++count;
	if( is_affected(victim, gsn_immolate )) ++count;
	if( is_affected(victim, gsn_resist_summon )) ++count;
	if( is_affected(victim, gsn_confusion_shell )) ++count;
	if( is_affected(victim, gsn_summon_distortion )) ++count;
	if( is_affected(victim, gsn_mind_fortess )) ++count;
	if( is_affected(victim, gsn_defense_curl )) ++count;
	if( is_affected(victim, 329 )) ++count;//razorblade
	if( is_affected(victim, 331 )) ++count;//beast claws
	if( is_affected(victim, 136 )) ++count;//stability
	if( is_affected(victim, 441 )) ++count;//perfect senses
	if( is_affected(victim, 444 )) ++count;//fortitude
	if( is_affected(victim, 481 )) ++count; // bull strenght
	if( is_affected(victim, 482 )) ++count; // cat grace
	if( is_affected(victim, 483 )) ++count; // owl wisdom
	if( is_affected(victim, 484 )) ++count; // fox cunning
	if( is_affected(victim, 485 )) ++count; // bear endurance
	if( is_affected(victim, 486 )) ++count; // eagle splendor
	if( is_affected(victim, 199 )) ++count;//ethereal arm
	if( is_affected(victim, 527 )) ++count;// spirit of life
	if( is_affected(victim, 285 )) ++count;// spirit armor
	if( is_affected(victim, 529 )) ++count;// spiritual guidance
	if( is_affected(victim, 245 )) ++count;//darkvis
	if( is_affected(victim, 79 )) ++count;//prot evi
	if( is_affected(victim, 80 )) ++count;//prot goo

	if( count > 1 ) return FALSE;

return TRUE;
}

bool check_wood_master(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	//true nie czaruje, false czaruje

	if( victim->in_room->sector_type != 3 &&//las
		victim->in_room->sector_type != 11 &&//puszcza
		victim->in_room->sector_type != 38 )//park
	{
		return TRUE;
	}

return FALSE;
}

bool check_stone_master(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	//true nie czaruje, false czaruje

	if( victim->in_room->sector_type != 4 &&//wzgorza
		victim->in_room->sector_type != 5 &&//gory
		victim->in_room->sector_type != 13 &&//wys gory
		victim->in_room->sector_type != 15 &&//jaskinia
		victim->in_room->sector_type != 17 &&//podziemia naturalne
		victim->in_room->sector_type != 35 )//gorska sciezka
	{
		return TRUE;
	}

return FALSE;
}

bool check_si_puppet_master(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	//true nie czaruje, false czaruje
	if( !IS_NPC( victim ) ) return TRUE;

	if ( !IS_AFFECTED( victim, AFF_CHARM ) || victim->master == NULL ) return TRUE;

	if ( ch->level < LEVEL_HERO && IS_AFFECTED( victim, AFF_LOYALTY ))return TRUE;

return FALSE;
}

bool check_x_metal(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	OBJ_DATA *obj;
	//true nie czaruje, false czaruje

	for( obj = victim->carrying; obj != NULL; obj = obj->next_content )
	{
		switch( obj->wear_loc )
		{
			case WEAR_FEET:
			case WEAR_LEGS:
			case WEAR_BODY:
			case WEAR_HEAD:
			case WEAR_SHIELD:
			case WEAR_ARMS:
			case WEAR_HANDS:
			case WEAR_WIELD:
			case WEAR_SECOND:	break;
			default:	continue;
		}

		if (obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_WEAPON )
			continue;

		if (IS_OBJ_STAT( obj, ITEM_NOMAGIC ) )
			continue;

		if ( IS_SET(material_table[obj->material].flag, MAT_METAL) )
			return FALSE;
	}

return TRUE;
}

bool check_dismiss(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	CHAR_DATA *victim_next;
	int found = 0;
	//true nie czaruje, false czaruje

	for ( victim = ch->in_room->people;victim;victim = victim_next )
	{
		victim_next = victim->next_in_room;

		if ( EXT_IS_SET( victim->act, ACT_NO_EXP ) )
			++found;
	}

	if ( found == 0 ) return TRUE;

return FALSE;
}

bool check_psionic(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	//true nie czaruje, false czaruje
	if( !IS_SET( victim->parts, PART_BRAINS ) )
		return TRUE;

	if( victim->resists[RESIST_MENTAL] == 100 )
		return TRUE;

return FALSE;
}

bool check_resist_cold(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	//true nie czaruje, false czaruje
	if( victim->resists[RESIST_COLD] >= number_range( 45, 55 ) )
	{
		if( victim->resists[RESIST_FIRE] >= number_range( 45, 55 ) &&//tutaj na wypadek resist elements
			victim->resists[RESIST_ELECTRICITY] >= number_range( 45, 55 ) &&
			victim->resists[RESIST_ACID] >= number_range( 45, 55 ) )
			return FALSE;

		return TRUE;
	}

return FALSE;
}

bool check_resist_fire(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	//true nie czaruje, false czaruje
	if( victim->resists[RESIST_FIRE] >= number_range( 45, 55 ) )
	{
		if( victim->resists[RESIST_COLD] >= number_range( 45, 55 ) &&//tutaj na wypadek resist elements
			victim->resists[RESIST_ELECTRICITY] >= number_range( 45, 55 ) &&
			victim->resists[RESIST_ACID] >= number_range( 45, 55 ) )
			return FALSE;

		return TRUE;
	}

return FALSE;
}

bool check_resist_lightning(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	//true nie czaruje, false czaruje
	if( victim->resists[RESIST_ELECTRICITY] >= number_range( 45, 55 ) )
	{
		if( victim->resists[RESIST_FIRE] >= number_range( 45, 55 ) &&//tutaj na wypadek resist elements
			victim->resists[RESIST_COLD] >= number_range( 45, 55 ) &&
			victim->resists[RESIST_ACID] >= number_range( 45, 55 ) )
			return FALSE;

		return TRUE;
	}

return FALSE;
}

bool check_resist_acid(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell)
{
	//true nie czaruje, false czaruje
	if( victim->resists[RESIST_ACID] >= number_range( 45, 55 ) )
	{
		if( victim->resists[RESIST_FIRE] >= number_range( 45, 55 ) &&//tutaj na wypadek resist elements
			victim->resists[RESIST_ELECTRICITY] >= number_range( 45, 55 ) &&
			victim->resists[RESIST_COLD] >= number_range( 45, 55 ) )
			return FALSE;

		return TRUE;
	}

return FALSE;
}
