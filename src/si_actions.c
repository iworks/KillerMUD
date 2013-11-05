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

//memowanie
//restowanie jesli ranny
//rzucenie sobie invisa badz komus z grupki
//leczenie rannych
//lay rannych
//summon przeciwnikow
//huntowanie przeciwnikow
struct si_priorytet actions_notfight[MAX_ACTION_NOT_FIGHT] =
{//priorytet 100 ma miec tylko 'wake' (by sie nie jebalo podczas wlaczenia u spiacego)
    {
	"heal friend",
	HEAL_FRIEND,
	45,
	90,
	10,
	TRUE,
	INT_POOR,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Can_heal, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ is_wounded, NULL, NULL, NULL, NULL },
	do_some_healing
    },
    {
	"mem spells",
	MEM_SPELLS,
	20,
	85,
	0,
	TRUE,
	INT_IDIOT,
	{ TRUE, TRUE, FALSE, TRUE, TRUE },
	{ Need_memming, Standing, Shopping, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	just_rest
    },
    {
	"stand",
	STAND,
	99,
	99,
	0,
	TRUE,
	INT_IDIOT,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_resting, Is_boring, Default_pos_resting, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	just_stand
    },
    {
	"powerup self",
	POWERUP_SELF,
	65,
	95,
	0,
	TRUE,
	INT_AVERAGE,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Got_powerups_self, -1 , -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	powerup_caster_not_fight
    },
    {
	"make some magic",
	MAKE_SOME_MAGIC,
	30,
	75,
	25,
	TRUE,
	INT_AVERAGE,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Can_do_some_magic, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ NULL, NULL, NULL, NULL, NULL },
	make_some_magic_not_fight
    },
    {
	"lay friend",
	LAY_FRIEND,
	85,
	95,
	15,
	TRUE,
	INT_IDIOT,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Can_lay, -1, -1, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ should_lay, NULL, NULL, NULL, NULL },
	lay_friend
    },
    {
	"make light",
	MAKE_LIGHT,
	95,
	95,
	0,
	TRUE,
	INT_IDIOT,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Is_dark_here, -1 , -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	make_light
    },
    {
	"hp regen",
	HP_REGEN,
	4,
	4,
	0,
	TRUE,
	INT_IDIOT,
	{ TRUE, TRUE, FALSE, TRUE, TRUE },
	{ Is_wounded, Standing, Shopping, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	hp_regen
    },
    {
	"shop stand",
	SHOP_STAND,
	99,
	99,
	0,
	TRUE,
	INT_IDIOT,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_resting, Shopping, -1, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	just_stand
    },
    {
	"summon player",
	SUMMON_PLAYER,
	60,
	90,
	0,
	FALSE,
	INT_GENIUS,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Can_summon, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ should_summon, NULL, NULL, NULL, NULL },
	summon_player
    },
    {
	"teleport to player",
	TELEPORT_TO_PLAYER,
	60,
	90,
	0,
	FALSE,
	INT_GENIUS,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Can_teleport, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ should_teleport, NULL, NULL, NULL, NULL },
	teleport_to_player
    },
    {
	"wake",
	WAKE,
	100,
	100,
	0,
	FALSE,
	INT_IDIOT,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_sleeping_normal, Default_pos_sleeping, -1, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	just_wake
    },
    {
	"call friend",
	CALL_FRIEND,
	40,
	70,
	0,
	FALSE,
	INT_AVERAGE,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Can_call_friend, Can_cast, -1, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ should_call_friend, NULL, NULL, NULL, NULL },
	call_friend
    },
};


/* tablica akcji podczas walki*/
struct si_priorytet actions[MAX_ACTION] =
{
    /* leczenie kumpli */
    {
	"heal friend",
	HEAL_FRIEND,
	15,
	75,
	25,
	TRUE,
	INT_POOR,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Can_heal, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ got_problems, can_be_healed, NULL, NULL, NULL },
	do_some_healing
    },

    /* utrudnianie zycia casterom */
    {
	"disable caster",
	DISABLE_ENEMY_CASTER,
	35,
	85,
	10,
	FALSE,
	INT_HIGH,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Got_anticasters, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ is_enemy_caster, is_target_active,  NULL, NULL, NULL },
	disable_caster
    },
    /* laduje jakiegos fraga */
    {
	"frag tank",
	FRAG_TANK,
	35,
	85,
	0,
	FALSE,
	INT_POOR,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Got_any_frags, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ is_enemy_tank, NULL, NULL, NULL, NULL },
	frag_tank
    },
    /* wybawia z opresji przyjaciol */
    {
	"rescue friend",
	RESCUE_FRIEND,
	75,
	95,
	0,
	TRUE,
	INT_POOR,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Can_tank, Knows_rescue, -1, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ can_rescue, got_problems, NULL, NULL, NULL },
	rescue_friend
    },
    /* uzywa skilli jakie tam sobie ma
       w checku na ofiary ma self bo tylko bedzie uzywal na
       ch->fighting a nie cos tam sobie wybieral  */
    {
	"use skill",
	USE_SKILL,
	45,
	75,
	10,
	FALSE,
	INT_IDIOT,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Standing, Knows_any_skills, Fighting, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ NULL, NULL, NULL, NULL, NULL },
	use_skills
    },
    /* robi jakies magiczne sztuczki  dla przyjaciol */
    {
	"make some magic",
	MAKE_SOME_MAGIC,
	30,
	75,
	25,
	TRUE,
	INT_AVERAGE,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Can_do_some_magic, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ need_a_help, NULL, NULL, NULL, NULL },
	make_some_magic
    },
    /* psuje zycie walczacym */
    {
	"disable tank",
	DISABLE_TANK,
	25,
	85,
	15,
	FALSE,
	INT_VERY_HIGH,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Got_any_antitanks, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ is_enemy_tank, is_target_active, NULL, NULL, NULL },
	disable_tank
    },
    /* rzuca se jakies spelliki jesli kiepsko mu idzie */
    {
	"powerup self",
	POWERUP_SELF,
	65,
	95,
	0,
	TRUE,
	INT_AVERAGE,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Got_powerups_self, -1 , -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, need_a_help, NULL, NULL, NULL },
	powerup_caster
    },
    /* wali na glebe casterow */
    {
	"bash caster",
	BASH_CASTER,
	40,
	90,
	10,
	FALSE,
	INT_AVERAGE,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Standing, Knows_bash, -1, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ is_enemy_caster, standing, NULL, NULL, NULL },
	bash_caster
    },
    /* po prostu sie do walki przylacza */
    {
	"just assist",
	JUST_ASSIST,
	75,
	75,
	0,
	TRUE,
	INT_IDIOT,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Standing, Not_fighting, -1, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	just_assist

    },
    /* wali na glebe sparalizowanych */
    {
	"bash paralized",
	BASH_PARALIZED,
	85,
	100,
	0,
	FALSE,
	INT_HIGH,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Standing, Knows_bash, -1, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ cant_move, NULL, NULL, NULL, NULL },
	bash_caster
    },
    /* laduje charga spraralizowanym */
    {
	"charge paralized",
	BASH_PARALIZED,
	90,
	100,
	0,
	FALSE,
	INT_HIGH,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Standing, Knows_charge, Can_charge, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ cant_move, NULL, NULL, NULL, NULL },
	charge_paralized
    },

    /* berserkuje */
    {
	"berserk",
	BERSERK,
	90,
	100,
	0,
	TRUE,
	INT_IDIOT,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Standing, Knows_berserk, Fighting, Can_berserk, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	berserk
    },

    /* wstaje jesli cos go przewrocilo */
    {
	"stand",
	STAND,
	100,
	100,
	0,
	TRUE,
	INT_IDIOT,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_down, -1, -1, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	just_stand
    },

    /* rzuca stone skina i basta */
    {
	"check stone skin",
	CHECK_STONE_SKIN,
	100,
	100,
	0,
	TRUE,
	INT_HIGH,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Got_stone_skin, Is_defending, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	cast_stone_skin
    },

    {
	"frag someone",
	FRAG_SOMEONE,
	35,
	85,
	0,
	FALSE,
	INT_POOR,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Got_any_frags, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ NULL, NULL, NULL, NULL, NULL },
	frag_someone
    },

    {
	"frag caster",
	FRAG_CASTER,
	35,
	85,
	0,
	FALSE,
	INT_AVERAGE,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Got_any_frags, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ is_enemy_caster, NULL, NULL, NULL, NULL },
	frag_caster
    },

    /* przywala z charge, na wejsciu albo podczas walki */
    {
	"charge",
	CHARGE,
	90,
	99,
	0,
	FALSE,
	INT_POOR,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Standing, Knows_charge, Can_charge, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ NULL, NULL, NULL, NULL, NULL },
	charge
    },

    {
	"backstab madafaka",
	BACKSTAB,
	100,
	100,
	0,
	FALSE,
	INT_POOR,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Standing, Not_fighting, Knows_backstab, Can_backstab, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ NULL, NULL, NULL, NULL, NULL },
	backstab
    },
    {
	"flee and hunt",
	FLEE_AND_HUNT,
	75,
	100,
	0,
	TRUE,
	INT_HIGH,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Standing, Fighting, Knows_backstab, Can_backstab, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	flee_and_hunt
    },
    /* dla wojkow wardance */
    {
	"wardance",
	WARDANCE,
	90,
	100,
	0,
	TRUE,
	INT_IDIOT,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Standing, Knows_wardance, Fighting, Can_wardance, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	wardance
    },
    {
	"changing target to master",
	TARGET_MASTER,
	60,
	100,
	0,
	TRUE,
	INT_AVERAGE,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Standing, Fighting, Can_target_master, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	target_master
    },
    /* dla barbi damage_reduction */
    {
	"damage reduction",
	WARDANCE,
	90,
	100,
	0,
	TRUE,
	INT_IDIOT,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Standing, Knows_damage_reduction, Fighting, Can_do_damage_reduction, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ self, NULL, NULL, NULL, NULL },
	damage_reduction
    },
    {
	"nasty hold person",
	NASTY_HOLD,
	90,
	100,
	0,
	FALSE,
	INT_HIGH,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Is_caster, Can_cast, Got_any_holds, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ nasty_holdable, NULL, NULL, NULL, NULL },
	nasty_hold
    },
    {
	"turn undead",
	TURN_UNDEAD,
	90,
	95,
	0,
	FALSE,
	INT_AVERAGE,
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ Can_cast, -1, -1, -1, -1 },
	{ TRUE, TRUE, TRUE, TRUE, TRUE },
	{ should_turn, NULL, NULL, NULL, NULL },
	turn_undead
    }
};
