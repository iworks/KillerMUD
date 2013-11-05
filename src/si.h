#ifndef _SI_H_
#define _SI_H_

/* kategorie inteligencji */
#define INT_GENIUS		5
#define INT_VERY_HIGH		4
#define INT_HIGH		3
#define INT_AVERAGE		2
#define INT_POOR		1
#define INT_IDIOT		0


#define MAX_SPELL_INFO			11
#define MAX_SPELLS				70
#define MAX_PRIOR_CHECK			5
#define MAX_ACTION				25
#define MAX_ACTION_NOT_FIGHT	14
#define MAX_CHECKS				60
#define MAX_SEARCH				32

/* rodzaje akcji */
#define HEAL_FRIEND			0
#define DISABLE_ENEMY_CASTER	1
#define FRAG_TANK			2
#define RESCUE_FRIEND		3
#define USE_SKILL			4
#define MAKE_SOME_MAGIC		5
#define DISABLE_TANK		6
#define POWERUP_SELF		7
#define BASH_CASTER			8
#define JUST_ASSIST			9
#define BASH_PARALIZED		10
#define CHARGE_PARALIZED	11
#define BERSERK				12
#define STAND				13
#define CHECK_STONE_SKIN	14
#define FRAG_SOMEONE		15
#define FRAG_CASTER			16
#define CHARGE				17
#define BACKSTAB			18
#define FLEE_AND_HUNT		19
#define WARDANCE			20
#define TARGET_MASTER		21
#define NASTY_HOLD			22
#define MEM_SPELLS			23
#define LAY_FRIEND			24
#define MAKE_LIGHT			25
#define HP_REGEN			26
#define SHOP_STAND			27
#define SUMMON_PLAYER		28
#define TELEPORT_TO_PLAYER	29
#define WAKE				30
#define TURN_UNDEAD			31
#define CALL_FRIEND			32

/* kategorie zaklec */
#define HEALS				0
#define FRAGS				1
#define DEF_ANTITANK_SELF	2
#define DEF_ANTITANK_OTHER	3
#define OFF_ANTITANK		4
#define OFF_ANTICASTER		5
#define DEF_ANTIMAGIC_SELF	6
#define DEF_ANTIMAGIC_OTHER	7
#define SAVE_LIFE			8
#define NASTY				9
#define CALLING				10

/* makra do selfcheckow */
#define Is_caster				0
#define	Can_cast				1
#define Got_any_frags			2
#define Knows_rescue			3
#define Knows_berserk			4
#define Knows_bash				5
#define Knows_charge			6
#define Can_charge				7
#define Knows_any_skills		8
#define Can_do_some_magic		9
#define Got_any_antitanks		10
#define Can_heal				11
#define Got_anticasters			12
#define Is_tank					13
#define Can_tank				14
#define Can_rescue				15
#define Can_berserk				16
#define Got_powerups_self		17
#define Not_fighting			18
#define Fighting				19
#define Is_down					20
#define Is_defending			21
#define Got_stone_skin			22
#define Standing				23
#define Knows_backstab			24
#define Can_backstab			25
#define Knows_wardance			26
#define Can_wardance			27
#define Wardancing				28
#define Disarmed				29
#define Can_target_master		30
#define Knows_damage_reduction  31
#define Can_do_damage_reduction 32
#define Got_any_holds			33
#define Need_memming			34
#define Is_resting				35
#define Is_boring				36
#define Can_lay					37
#define Is_dark_here			38
#define Is_wounded				39
#define Shopping				40
#define Should_lay				41
#define Can_summon				42
#define Should_summon			43
#define Can_teleport			44
#define Should_teleport			45
#define Default_pos_resting		46
#define Is_sleeping_normal		47
#define Default_pos_sleeping	48
#define Can_call_friend			49


struct char_desc
{
    CHAR_DATA	* ch;
    int		caster;
    int		tank;
    int		hp_percent;
    int		active;
    struct char_desc	*next;
};

/* globalna lista zaklec po filtracji */


typedef bool CHECK_FUN	args( (CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell) );
typedef void AFTER_SPELL args( (CHAR_DATA *ch, CHAR_DATA *victim) );
typedef bool PRIOR_FUNC  args( (struct char_desc *ch_desc, struct char_desc *desc) );
typedef bool ACTION_FUNC args( (CHAR_DATA *ch, struct char_desc *desc) );

struct si_spell_data
{
    char *	name;	//str lookup of spell, just for initializing
    sh_int	sn;	//sn - id of a spell
    int		rating;	//how 'good' is this spell
    int		extra;	//unused
    CHECK_FUN * check;	//func checked before casting this spell
    AFTER_SPELL * func;	//special func checked after casting this spell
};

#ifndef SI_FUNCS_C
extern struct si_spell_data	* memmed_spells[MAX_SPELLS];
#endif

/* kategori akcji */
struct si_priorytet
{
    char *	name;
    int		action;
    int 	min;
    int		max;
    int		variations;
    bool	group;
    int		int_check;
    bool    ch_check_cond[MAX_PRIOR_CHECK];   /* TRUE - jesli funkcja z ch_check ma byc spelniona, FALSE, jesli ma byc nie spelniona */
    int		ch_check[MAX_PRIOR_CHECK];
    bool    check_cond[MAX_PRIOR_CHECK];   /* TRUE - jesli funkcja z check ma byc spelniona, FALSE, jesli ma byc nie spelniona */
    PRIOR_FUNC * check[MAX_PRIOR_CHECK];
    ACTION_FUNC * fun;
};

/* optymalizacja */
struct self_check
{
    PRIOR_FUNC *	fun;
    char		value;
};

extern struct self_check self_checks_table[MAX_CHECKS];
extern struct si_spell_data si_spell_info[MAX_SPELL_INFO][MAX_SPELLS];
extern struct si_priorytet actions[MAX_ACTION];
extern struct si_priorytet actions_notfight[MAX_ACTION_NOT_FIGHT];

/* globalne funckje */
void    wield_weapon    args( (CHAR_DATA *ch, OBJ_DATA *obj, bool primary));
void forget(CHAR_DATA *mob, CHAR_DATA *who, int reaction,  bool All);
void remember(CHAR_DATA *mob, CHAR_DATA *who, int reaction);
sh_int get_caster(CHAR_DATA *ch);
int test_int(CHAR_DATA *ch);
void force_cast(CHAR_DATA *ch, CHAR_DATA *victim, sh_int sn);
int check_tank(CHAR_DATA *ch);
int need_help(CHAR_DATA *ch, CHAR_DATA *gch);
struct char_desc * create_desc(CHAR_DATA *ch, int caster, int tank, int hp_percent, int active);
struct char_desc * copy_desc(struct char_desc *element);
void add_desc_list(struct char_desc **list, struct char_desc *element);
void dispose_list(struct char_desc **list);
void si_update(CHAR_DATA *ch);
void optimize_self_checks( struct char_desc *ch_desc );


/* funkcje do zaklec */
bool check_affect(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_call_lightning(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_dispel_evil(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_dispel_good(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_fire_shield(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_freezing_rain(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_hold(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_ice_shield(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_sp_armor(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_sp_hammer(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_word_blind(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_word_kill(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_word_stun(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_meteor(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_razorblade(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_holy_wea(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_undead(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_unholy(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_si_dispel(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
void stop_fight_after_charm(CHAR_DATA *ch, CHAR_DATA *victim);
bool check_lava_bolt(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_wood_master(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_stone_master(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_si_puppet_master(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_x_metal(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_dismiss(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_psionic(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_resist_cold(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_resist_fire(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_resist_lightning(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_resist_acid(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_dismiss_part(CHAR_DATA *victim, int RACE );
bool check_dism_anim(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_dism_plant(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_dism_person(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_dism_monster(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_dism_undead(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);
bool check_dism_insect(CHAR_DATA *ch, CHAR_DATA *victim, int type, int spell);


/* funkcje do si */
int check_tank(CHAR_DATA *ch);
int need_help(CHAR_DATA *ch, CHAR_DATA *gch);
struct char_desc * create_desc(CHAR_DATA *ch, int caster, int tank, int hp_percent, int active);
void add_desc_list(struct char_desc **list, struct char_desc *element);
void dispose_list(struct char_desc **list);
struct char_desc * check_target(CHAR_DATA *vch);
struct char_desc * check_group(CHAR_DATA *ch);
//struct char_desc * check_enemy(CHAR_DATA *ch);
struct char_desc * check_enemy(CHAR_DATA *ch, struct char_desc *group_list);
char * get_target_by_name(CHAR_DATA *ch, CHAR_DATA *victim);

/* checki do akcji */
bool hp_check(struct char_desc *ch_desc, struct char_desc *desc);
bool is_caster(struct char_desc *ch_desc, struct char_desc *desc);
bool can_cast(struct char_desc *ch_desc, struct char_desc *desc);
bool got_any_frags(struct char_desc *ch_desc, struct char_desc *desc);
bool got_problems(struct char_desc *ch_desc, struct char_desc *desc);
bool is_wounded(struct char_desc *ch_desc, struct char_desc *desc);
bool need_a_help(struct char_desc *ch_desc, struct char_desc *desc);
bool knows_rescue(struct char_desc *ch_desc, struct char_desc *desc);
bool knows_berserk(struct char_desc *ch_desc, struct char_desc *desc);
bool knows_bash(struct char_desc *ch_desc, struct char_desc *desc);
bool knows_charge(struct char_desc *ch_desc, struct char_desc *desc);
bool can_charge(struct char_desc *ch_desc, struct char_desc *desc);
bool knows_any_skills(struct char_desc *ch_desc, struct char_desc *desc);
bool can_do_some_magic(struct char_desc *ch_desc, struct char_desc *desc);
bool got_any_antitanks(struct char_desc *ch_desc, struct char_desc *desc);
bool can_heal(struct char_desc *ch_desc, struct char_desc *desc);
bool is_enemy_caster(struct char_desc *ch_desc, struct char_desc *desc);
bool got_anticasters(struct char_desc *ch_desc, struct char_desc *desc);
bool is_tank(struct char_desc *ch_desc, struct char_desc *desc);
bool can_tank(struct char_desc *ch_desc, struct char_desc *desc);
bool is_enemy_tank(struct char_desc *ch_desc, struct char_desc *desc);
bool can_rescue(struct char_desc *ch_desc, struct char_desc *desc);
bool can_berserk(struct char_desc *ch_desc, struct char_desc *desc);
bool is_target_active(struct char_desc *ch_desc, struct char_desc *desc);
bool got_powerups_self(struct char_desc *ch_desc, struct char_desc *desc);
bool self(struct char_desc *ch_desc, struct char_desc *desc);
bool notself(struct char_desc *ch_desc, struct char_desc *desc);
bool standing(struct char_desc *ch_desc, struct char_desc *desc);
bool cant_move(struct char_desc *ch_desc, struct char_desc *desc);
bool not_fighting(struct char_desc *ch_desc, struct char_desc *desc);
bool fighting(struct char_desc *ch_desc, struct char_desc *desc);
bool is_down(struct char_desc *ch_desc, struct char_desc *desc);
bool is_resting(struct char_desc *ch_desc, struct char_desc *desc);
bool is_defending(struct char_desc *ch_desc, struct char_desc *desc);
bool got_stone_skin(struct char_desc *ch_desc, struct char_desc *desc);
bool can_backstab(struct char_desc *ch_desc, struct char_desc *desc);
bool knows_backstab(struct char_desc *ch_desc, struct char_desc *desc);
bool knows_wardance(struct char_desc *ch_desc, struct char_desc *desc);
bool can_wardance(struct char_desc *ch_desc, struct char_desc *desc);
bool player(struct char_desc *ch_desc, struct char_desc *desc);
bool wardancing(struct char_desc *ch_desc, struct char_desc *desc);
bool disarmed(struct char_desc *ch_desc, struct char_desc *desc);
bool can_target_master(struct char_desc *ch_desc, struct char_desc *desc);
bool can_be_healed(struct char_desc *ch_desc, struct char_desc *desc);
bool can_do_damage_reduction(struct char_desc *ch_desc, struct char_desc *desc);
bool knows_damage_reduction(struct char_desc *ch_desc, struct char_desc *desc);
bool got_any_holds(struct char_desc *ch_desc, struct char_desc *desc);
bool nasty_holdable(struct char_desc *ch_desc, struct char_desc *desc);
bool need_memming(struct char_desc *ch_desc, struct char_desc *desc);
bool is_boring(struct char_desc *ch_desc, struct char_desc *desc);
bool can_lay(struct char_desc *ch_desc, struct char_desc *desc);
bool is_dark_here(struct char_desc *ch_desc, struct char_desc *desc);
bool shopping(struct char_desc *ch_desc, struct char_desc *desc);
bool should_lay(struct char_desc *ch_desc, struct char_desc *desc);
bool can_summon(struct char_desc *ch_desc, struct char_desc *desc);
bool should_summon(struct char_desc *ch_desc, struct char_desc *desc);
bool should_teleport(struct char_desc *ch_desc, struct char_desc *desc);
bool can_teleport(struct char_desc *ch_desc, struct char_desc *desc);
bool default_pos_resting(struct char_desc *ch_desc, struct char_desc *desc);
bool is_sleeping_normal(struct char_desc *ch_desc, struct char_desc *desc);
bool default_pos_sleeping(struct char_desc *ch_desc, struct char_desc *desc);
bool should_turn(struct char_desc *ch_desc, struct char_desc *desc);
bool can_call_friend(struct char_desc *ch_desc, struct char_desc *desc);
bool should_call_friend(struct char_desc *ch_desc, struct char_desc *desc);

/* funkcje wykonujace akcje */
bool bash_caster(CHAR_DATA *ch, struct char_desc *desc);
bool charge_paralized(CHAR_DATA *ch, struct char_desc *desc);
bool just_assist(CHAR_DATA *ch, struct char_desc *desc);
bool just_stand(CHAR_DATA *ch, struct char_desc *desc);
bool rescue_friend(CHAR_DATA *ch, struct char_desc *desc);
bool berserk(CHAR_DATA *ch, struct char_desc *desc);
bool do_some_healing(CHAR_DATA *ch, struct char_desc *desc);
bool disable_caster(CHAR_DATA *ch, struct char_desc *desc);
bool frag_tank(CHAR_DATA *ch, struct char_desc *desc);
bool disable_tank(CHAR_DATA *ch, struct char_desc *desc);
bool powerup_caster(CHAR_DATA *ch, struct char_desc *desc);
bool cast_stone_skin(CHAR_DATA *ch, struct char_desc *desc);
bool frag_caster(CHAR_DATA *ch, struct char_desc *desc);
bool frag_someone(CHAR_DATA *ch, struct char_desc *desc);
bool use_skills(CHAR_DATA *ch, struct char_desc *desc);
bool charge(CHAR_DATA *ch, struct char_desc *desc);
bool backstab(CHAR_DATA *ch, struct char_desc *desc);
bool flee_and_hunt(CHAR_DATA *ch, struct char_desc *desc);
bool make_some_magic(CHAR_DATA *ch, struct char_desc *desc);
bool make_some_magic_not_fight(CHAR_DATA *ch, struct char_desc *desc);
bool wardance(CHAR_DATA *ch, struct char_desc *desc);
bool reequip(CHAR_DATA *ch, struct char_desc *desc);
bool target_master(CHAR_DATA *ch, struct char_desc *desc);
bool damage_reduction(CHAR_DATA *ch, struct char_desc *desc);
bool nasty_hold(CHAR_DATA *ch, struct char_desc *desc);
bool just_rest(CHAR_DATA *ch, struct char_desc *desc);
bool hp_regen(CHAR_DATA *ch, struct char_desc *desc);
bool lay_friend(CHAR_DATA *ch, struct char_desc *desc);
bool make_light(CHAR_DATA *ch, struct char_desc *desc);
bool powerup_caster_not_fight(CHAR_DATA *ch, struct char_desc *desc);
bool summon_player(CHAR_DATA *ch, struct char_desc *desc);
bool teleport_to_player(CHAR_DATA *ch, struct char_desc *desc);
bool just_wake(CHAR_DATA *ch, struct char_desc *desc);
bool turn_undead(CHAR_DATA *ch, struct char_desc *desc);
bool call_friend(CHAR_DATA *ch, struct char_desc *desc);
#endif
