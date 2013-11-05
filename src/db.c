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
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: db.c 11356 2012-05-16 20:44:02Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/12.02/src/db.c $
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include "merc.h"
#include "db.h"
#include "recycle.h"
#include "music.h"
#include "tables.h"
#include "olc.h"
#include "music.h"
#include "herbs.h"
#include "todelete.h" // delayed ch delete - by Fuyara
#include "projects.h"
#include "money.h"

#include "charset_utils.h"
#include <locale.h>

//mt19937ar.c
void init_random(long x, long y);
unsigned long genrand_int32(void);
void init_by_array(unsigned long init_key[], int key_length);

#if !defined(macintosh)
extern	int	_filbuf		args( (FILE *) );
#endif

#if (RANDOM_GEN == 1) || (RANDOM_GEN == 2)
#if !defined(linux)
//long random();
#endif
//void srandom(unsigned int);
int getpid();
time_t time(time_t *tloc);
#endif


char * last_command;
//rellik: do debugowania, dodaje te¿ pamiêtanie poprzednieg debug_info
char * previous_command;

//rellik: do debugowania, nie wiem czy tu moze byc deklaracja funkcji (definicja w act_wiz)
void save_debug_info(const char *actual, const char *caller, char *txt,	int nr_proj, int debuglevel, bool include_info);

/* externals for counting purposes */
extern	OBJ_DATA	*obj_free;
extern	CHAR_DATA	*char_free;
extern  DESCRIPTOR_DATA *descriptor_free;
extern	PC_DATA		*pcdata_free;
extern  AFFECT_DATA	*affect_free;
extern  MSPELL_DATA	*mspell_free;
extern	int port;

/*
 * Globals.
 */
HELP_DATA *		help_first;
HELP_DATA *		help_last;

HELP_AREA *		had_list;

SHOP_DATA *		shop_first;
SHOP_DATA *		shop_last;

BANK_DATA *		bank_first;
BANK_DATA *		bank_last;

NOTE_DATA *		note_free;

PROG_CODE *		mprog_list;
PROG_CODE *		oprog_list;
PROG_CODE *		rprog_list;
TRAP_DATA * 	trap_list;
RAND_DESC_DATA * 	rand_desc_list;

char			bug_buf		[2*MAX_INPUT_LENGTH];
CHAR_DATA *		char_list = NULL;


ARTEFACT_DATA *         artefact_system;
LEARN_DATA *		learn_system;
LEARN_TRICK_DATA *		learn_trick_system;
BOARD_DATA *		board_system;
SPELL_MSG *		spellmsg_list;
PRE_WAIT *		pre_waits;

char *			help_greeting;
char			log_buf		[2*MAX_INPUT_LENGTH];
KILL_DATA		kill_table	[MAX_LEVEL];
NOTE_DATA *		note_list;
OBJ_DATA *		object_list = NULL;
TIME_INFO_DATA		time_info;
ROOM_UPDATE_LIST 	room_update_list;
BONUS_INDEX_DATA *		bonus_list = NULL;
SONG_DATA *		song_list = NULL;
SOCIAL_DATA *	social_list = NULL;

sh_int			gsn_backstab;
sh_int			gsn_circle;
sh_int			gsn_dodge;
sh_int			gsn_envenom;
sh_int			gsn_hide;
sh_int			gsn_peek;
sh_int			gsn_pick_lock;
sh_int			gsn_sneak;
sh_int			gsn_steal;
sh_int			gsn_disarm;
sh_int			gsn_enhanced_damage;
sh_int			gsn_kick;
sh_int			gsn_crush;
sh_int			gsn_tail;
sh_int			gsn_charge;
sh_int			gsn_stun;
sh_int			gsn_shoot;
sh_int			gsn_lay;
sh_int			gsn_garhal;
sh_int			gsn_parry;
sh_int			gsn_shield_block;
sh_int			gsn_rescue;
sh_int			gsn_second_attack;
sh_int			gsn_third_attack;
sh_int			gsn_blindness;
sh_int			gsn_call_lightning;
sh_int			gsn_summon_lesser_meteor;
sh_int			gsn_summon_greater_meteor;
sh_int			gsn_charm_person;
sh_int			gsn_charm_monster;
sh_int			gsn_curse;
sh_int			gsn_energy_drain;
sh_int			gsn_invis;
sh_int			gsn_mass_invis;
sh_int			gsn_poison;
sh_int			gsn_feeblemind;
sh_int			gsn_plague;
sh_int			gsn_sleep;
sh_int			gsn_sanctuary;
sh_int			gsn_fly;
sh_int			gsn_float;
sh_int  		gsn_two_hands_fighting;
sh_int  		gsn_axe;
sh_int  		gsn_dagger;
sh_int  		gsn_flail;
sh_int  		gsn_mace;
sh_int  		gsn_polearm;
sh_int  		gsn_sharpen;
sh_int  		gsn_recuperate;
sh_int  		gsn_repair;
sh_int			gsn_bladethirst;
sh_int			gsn_healing_sleep;
sh_int			gsn_energize;
sh_int  		gsn_shortsword;
sh_int  		gsn_staff;
sh_int  		gsn_spear;
sh_int  		gsn_sword;
sh_int  		gsn_whip;
sh_int  		gsn_bash;
sh_int  		gsn_berserk;
sh_int  		gsn_divine_power;
sh_int			gsn_wardance;
sh_int 			gsn_damage_reduction;
sh_int  		gsn_slam;
/*sh_int  		gsn_dirt;
  sh_int  		gsn_hand_to_hand;*/
sh_int  		gsn_trip;
sh_int      gsn_fire_breath;
sh_int      gsn_frost_breath;
sh_int      gsn_acid_breath;
sh_int      gsn_lightning_breath;
sh_int      gsn_sap;
sh_int      gsn_claws;

//style walki
sh_int  		gsn_two_weapon_fighting;
sh_int  		gsn_twohander_fighting;

//mastery
sh_int			gsn_dagger_mastery;
sh_int			gsn_mace_mastery;
sh_int			gsn_flail_mastery;
sh_int			gsn_sword_mastery;
sh_int			gsn_axe_mastery;
sh_int			gsn_spear_mastery;
sh_int			gsn_staff_mastery;
sh_int			gsn_shortsword_mastery;
sh_int			gsn_whip_mastery;
sh_int			gsn_polearm_mastery;

//inne umiejetnosci
sh_int  		gsn_fast_healing;
sh_int  		gsn_riding;
sh_int  		gsn_lore;
sh_int  		gsn_meditation;
sh_int  		gsn_bandage;
sh_int  		gsn_detect_traps;
sh_int  		gsn_disarm_traps;
sh_int  		gsn_target;
sh_int  		gsn_smite;
sh_int  		gsn_turn;
sh_int			gsn_skin;
/*sh_int  		gsn_scrolls;
  sh_int  		gsn_staves;
  sh_int  		gsn_wands;*/
sh_int  		gsn_recall;
sh_int			gsn_track;
sh_int			gsn_holdperson;
sh_int			gsn_holdmonster;
sh_int			gsn_holdplant;
sh_int			gsn_holdanimal;
sh_int			gsn_confusion;
sh_int			gsn_regenerate;
sh_int			gsn_animal_invis;
sh_int			gsn_undead_invis;
sh_int			gsn_resist_fire;
sh_int			gsn_resist_cold;
sh_int			gsn_resist_lightning;
sh_int			gsn_resist_magic;
sh_int			gsn_resist_summon;
sh_int			gsn_fear;
sh_int			gsn_brave_cloak;
sh_int			gsn_silence;
sh_int			gsn_bark_skin;
sh_int			gsn_aid;
sh_int			gsn_chant;
sh_int			gsn_prayer;
sh_int			gsn_waterwalk;
sh_int			gsn_entangle;
sh_int			gsn_web;
sh_int			gsn_water_breathing;
sh_int			gsn_energy_shield;
sh_int			gsn_stone_skin;
sh_int			gsn_mirror_image;
sh_int			gsn_fireshield;
sh_int			gsn_iceshield;
sh_int			gsn_reflect_spell;
sh_int			gsn_increase_wounds;
sh_int			gsn_armor;
sh_int			gsn_summon_distortion;
sh_int			gsn_draining_hands;
sh_int			gsn_magic_hands;
sh_int			gsn_unholy_fury;
sh_int			gsn_illusion;
sh_int			gsn_soul;
sh_int			gsn_razorbladed;
sh_int			gsn_energy_strike;
sh_int			gsn_ghoul_touch;
sh_int			gsn_hallucinations;
sh_int          gsn_domination;
sh_int			gsn_perfect_self;
sh_int			gsn_confusion_shell;
sh_int			gsn_maze;
sh_int			gsn_antimagic_manacles;
sh_int			gsn_mental_barrier;
sh_int			gsn_astral_journey;
sh_int			gsn_steel_scarfskin;
sh_int			gsn_acid_arrow;
sh_int			gsn_blink;
sh_int			gsn_fetch;
sh_int			gsn_mighty_blow;
sh_int			gsn_power_strike;
sh_int			gsn_critical_strike;
sh_int			gsn_heat_metal;
sh_int			gsn_chill_metal;
sh_int			gsn_holdevil;
sh_int			gsn_devour;
sh_int			gsn_made_undead;
sh_int			gsn_inspire;
sh_int			gsn_hardiness;
sh_int			gsn_defense_curl;
sh_int			gsn_mind_fortess;
sh_int			gsn_shadow_swarm;
sh_int			gsn_dazzling_flash;

sh_int			gsn_playerquityes;
sh_int			gsn_playerdeath;
sh_int			gsn_playerdeathpenalty;
sh_int			gsn_playerdeathlearnpenalty;
sh_int			gsn_noob_killer;
sh_int			gsn_mirrorfall;
sh_int			gsn_yell;
sh_int			gsn_dehydration;
sh_int			gsn_malnutrition;
sh_int			gsn_on_smoke;
sh_int			gsn_bleeding_wound;
sh_int			gsn_bleed;
sh_int			gsn_thigh_jab;
sh_int			gsn_weapon_wrench;
sh_int			gsn_glorious_impale;

//monk gsn
sh_int			gsn_concentration;
sh_int			gsn_flurry_of_blows;
sh_int			gsn_healing_hands;
sh_int			gsn_knockdown;
sh_int			gsn_monk_dodge;
sh_int			gsn_rush;
sh_int			gsn_stunning_fist;
sh_int			gsn_unarmed_strike;

//druid gsn
sh_int			gsn_alicorn_lance;
sh_int			gsn_animal_rage;
sh_int			gsn_beast_claws;
sh_int			gsn_blade_barrier;
sh_int			gsn_herbs_knowledge;
sh_int			gsn_immolate;
sh_int			gsn_lava_bolt;
sh_int			gsn_luck;
sh_int  		gsn_magic_fang;
sh_int			gsn_nature_curse;
sh_int			gsn_shapeshift;
sh_int			gsn_shield_of_nature;
sh_int			gsn_shillelagh;
sh_int			gsn_wind_charger;
sh_int			gsn_fury_of_the_wild;
sh_int			gsn_create_spring;

//paladyn
sh_int          gsn_holy_weapons;
sh_int          gsn_deeds_pool;
sh_int          gsn_holy_pool;
sh_int          gsn_holy_prayer;
sh_int          gsn_prayer_last;
sh_int          gsn_sanctification;
sh_int          gsn_sacred_group;

//kleryk
sh_int			gsn_create_symbol;
sh_int			gsn_life_transfer;
sh_int			gsn_first_aid;
sh_int			gsn_healing_touch;

//bard
sh_int			gsn_listen;
sh_int			gsn_persuasion;
sh_int			gsn_ocarina;
sh_int			gsn_flute;
sh_int          gsn_lute;
sh_int          gsn_harp;
sh_int          gsn_mandolin;
sh_int          gsn_piano;
sh_int          gsn_music;
sh_int          gsn_trade;
sh_int          gsn_lore_intuition;
sh_int          gsn_tune;
sh_int          gsn_sing;

//black knight
sh_int			gsn_torment;
sh_int			gsn_cleave;
sh_int			gsn_overwhelming_strike;
sh_int			gsn_demon_aura;
sh_int			gsn_vertical_slash;
sh_int			gsn_damn_weapon;
sh_int			gsn_damn_armor;
sh_int			gsn_smite_good;
sh_int			gsn_call_avatar;
sh_int			gsn_hustle;
sh_int			gsn_control_undead;
sh_int			gsn_undead_resemblance;

//przemiany
sh_int			gsn_basic_shapeshift;

//evoker
sh_int			gsn_burn;
sh_int			gsn_rain_of_blades;

//Szaman
sh_int			gsn_invoke_spirit;

//niszczenie artów
sh_int                  gsn_art_destroy;

//rozne takie
sh_int			gsn_none;

/*
 * Locals.
 */
MOB_INDEX_DATA *	mob_index_hash		[MAX_KEY_HASH];
OBJ_INDEX_DATA *	obj_index_hash		[MAX_KEY_HASH];
ROOM_INDEX_DATA *	room_index_hash		[MAX_KEY_HASH];

AREA_DATA *		area_first;
AREA_DATA *		area_last;
AREA_DATA *		current_area;

extern char			str_empty	[1];

int 			top_event;
int			top_affect;
int			top_area;
int			top_ed;
int			top_exit;
int			top_help;
unsigned int		top_mob_index;
unsigned int		top_obj_index;
int			top_reset;
unsigned int		top_room;
int			top_shop;
int			top_bank;
unsigned int            top_vnum_room;		/* OLC */
unsigned int            top_vnum_mob;		/* OLC */
unsigned int            top_vnum_obj;		/* OLC */
unsigned int		top_mprog_index;	/* OLC */
unsigned int		top_oprog_index;	/* OLC */
unsigned int		top_rprog_index;	/* OLC */
unsigned int		top_trap_index;		/* OLC */
unsigned int		top_rdesc_index;		/* OLC */
int 			mobile_count = 0;
int			newmobs = 0;
int			newobjs = 0;
bool                    str_cmp_debug = TRUE;

/*
 * Memory management.
 * Increase MAX_STRING if you have too.
 * Tune the others only if you understand what you're doing.
 */
#define			MAX_PERM_BLOCK	131072
extern long		MAX_STRING;

long nAllocString;
long sAllocString;
long	nOverFlowString;
long	sOverFlowString;
bool	Full;
int nAllocPerm;
int sAllocPerm;
char str_empty[1];



/*
 * Semi-locals.
 */
bool			fBootDb;
FILE *			fpArea;
char			strArea[MAX_INPUT_LENGTH];


/*
 * Local booting procedures.
 */
void    init_mm         args( ( void ) );
void    new_load_area   args( ( FILE *fp ) );   /* OLC */
void	load_helps	    args( ( FILE *fp, char *fname ) );
void	load_resets	    args( ( FILE *fp ) );
void	load_shops	    args( ( FILE *fp ) );
void    load_repairs    args( ( FILE *fp ) );
void	load_banks	    args( ( FILE *fp ) );
void 	load_socials	args( ( FILE *fp ) );
void	load_specials	args( ( FILE *fp ) );
void	load_notes	    args( ( void ) );
void	load_bans	    args( ( void ) );
void    save_bans       args( ( void ) );
void	load_mobprogs	args( ( FILE *fp, bool new ) );
void	load_objprogs	args( ( FILE *fp, bool new ) );
void	load_roomprogs	args( ( FILE *fp, bool new ) );

void	fix_exits	    args( ( void ) );
void	fix_echoto	    args( ( void ) );
void    fix_mobprogs	args( ( void ) );
void    fix_objprogs	args( ( void ) );
void    fix_roomprogs	args( ( void ) );

void	reset_area	args( ( AREA_DATA * pArea ) );
void    load_learn_system args( (void) );
void    load_learn_trick_system args( (void) );
void 	load_artefact_list args ( (void) );
void	initialize_name_table();
void    load_prewait_system args( (void) );
void 	load_boards	args( (void)   );
void 	fill_slots	args((CHAR_DATA *mob));
void 	rp_reset_trigger	args( (ROOM_INDEX_DATA *pRoom   ) );
void 	mp_onload_trigger	args( ( CHAR_DATA *mob ) );
void 	op_onload_trigger	args( ( OBJ_DATA *obj ) );
void 	load_tongues	args(());
void 	si_setup_spell_data();
void 	load_spell_msg	args( () );
bool	fread_mobile( FILE *fp );
bool	fread_room( FILE *fp );
bool	fread_object( FILE *fp );
bool	fread_trap( FILE * fp );
bool    fread_desc( FILE * fp );
bool    fread_rand_desc( FILE * fp );
bool    fread_bonus_set( FILE * fp );
void	load_clans();
void	load_monk_hits();
void    scan_players_for_artefacts();
void    load_misc_data();
void    save_misc_data();
void    create_rand_desc    args( ( ROOM_INDEX_DATA *pRoom, char *src_desc, char *dest) );
void    set_str_cmp_debug( bool enabled );

/*
 * Big mama top level function.
 */
void boot_db( )
{

    fBootDb		= TRUE;

    /*
     * Reset locale
     */

    setlocale(LC_ALL, "C");

    /*
     * Initialize charset utils
     */
    init_iconv();


    /*
     * Init random number generator.
     */
    init_mm();

    /*
     * Assign gsn's for skills which have them.
     */
    {
        int sn;
        for ( sn = 0; sn < MAX_SKILL; sn++ )
        {
            if ( skill_table[sn].pgsn != NULL )
            {
                *skill_table[sn].pgsn = sn;
            }
        }
    }
    /*
     * Read in all the area files.
     */
    {
        FILE *fpList;

        sprintf(log_buf,"Loading all area files");
        log_string( log_buf );
        set_str_cmp_debug( FALSE );

        if ( ( fpList = fopen( AREA_LIST, "r" ) ) == NULL )
        {
            perror( AREA_LIST );
            exit( 1 );
        }

        for ( ; ; )
        {

            strcpy( strArea, fread_word( fpList ) );

            if ( strArea[0] == '$' )
                break;

            if ( strArea[0] == '-' )
            {
                fpArea = stdin;
            }
            else
            {

                if ( ( fpArea = fopen( strArea, "r" ) ) == NULL )
                {
                    perror( strArea );
                    exit( 1 );
                }
            }
            current_area = NULL;

            for ( ; ; )
            {
                char *word;

                if ( fread_letter( fpArea ) != '#' )
                {
                    bug( "Boot_db: # not found.", 0 );
                    exit( 1 );
                }

                word = fread_word( fpArea );

                if ( word[0] == '$'               )    break;
                else if ( !str_cmp( word, "AREADATA" ) ) new_load_area(fpArea);
                else if ( !str_cmp( word, "HELPS"    ) ) load_helps   (fpArea, strArea);
                else if ( !str_cmp( word, "MOBILES"  ) ) for(;fread_mobile(fpArea););
                else if ( !str_cmp( word, "MOBPROGS" ) ) load_mobprogs( fpArea, FALSE );
                else if ( !str_cmp( word, "OBJPROGS" ) ) load_objprogs( fpArea, FALSE );
                else if ( !str_cmp( word, "ROOMPROGS") ) load_roomprogs( fpArea, FALSE );
                else if ( !str_cmp( word, "MOBPROGSNEW" ) ) load_mobprogs( fpArea, TRUE );
                else if ( !str_cmp( word, "OBJPROGSNEW" ) ) load_objprogs( fpArea, TRUE );
                else if ( !str_cmp( word, "ROOMPROGSNEW") ) load_roomprogs( fpArea, TRUE );
                else if ( !str_cmp( word, "OBJECTS"  ) ) for(;fread_object(fpArea););
                else if ( !str_cmp( word, "RESETS"   ) ) load_resets  (fpArea);
                else if ( !str_cmp( word, "ROOMS"    ) ) for(;fread_room(fpArea););
                else if ( !str_cmp( word, "SHOPS"    ) ) load_shops   (fpArea);
                else if ( !str_cmp( word, "BANKS"    ) ) load_banks   (fpArea);
                else if ( !str_cmp( word, "SOCIALS"  ) ) load_socials (fpArea);
                else if ( !str_cmp( word, "SPECIALS" ) ) load_specials(fpArea);
                else if ( !str_cmp( word, "REPAIRS"  ) ) load_repairs(fpArea);
                else if ( !str_cmp( word, "TRAPS") )	 for(;fread_trap(fpArea););
                else if ( !str_cmp( word, "DESC") )	 for(;fread_desc(fpArea););
                else if ( !str_cmp( word, "RANDDESC") )	 for(;fread_rand_desc(fpArea););
                else if ( !str_cmp( word, "BONUS_SET") ) for(;fread_bonus_set(fpArea););
                else
                {
                    bug( "Boot_db: bad section name.", 0 );
                    exit( 1 );
                }
            }

            if ( fpArea != stdin )
                fclose( fpArea );
            fpArea = NULL;
        }
        fclose( fpList );

        set_str_cmp_debug( TRUE );
    }
    /*
     * Fix up exits.
     * Declare db booting over.
     * Reset all areas once.
     * Load up the songs, notes and ban files.
     */



    {
        sprintf(log_buf,"Loading misc data.");
        log_string( log_buf );
        load_misc_data();

        sprintf(log_buf,"Loading artefact system.");
        log_string( log_buf );
        load_artefact_list();

        sprintf( log_buf, "Fixing exits." );
        log_string( log_buf );
        fix_exits( );

        fix_echoto( );

        sprintf( log_buf, "Fixing mobprograms." );
        log_string( log_buf );
        fix_mobprogs( );
        fix_objprogs( );
        fix_roomprogs( );

        fBootDb	= FALSE;

        sprintf( log_buf, "Updating area." );
        log_string( log_buf );
        area_update( );

        sprintf( log_buf, "Loading notes." );
        log_string( log_buf );
        load_notes( );

        sprintf( log_buf, "Loading bans." );
        log_string( log_buf );
        load_bans();

        sprintf( log_buf, "Saving bans." );
        log_string( log_buf );
        save_bans();

        sprintf( log_buf, "Loading songs." );
        log_string( log_buf );
        load_songs();

        sprintf( log_buf, "Loading todelete list." );
        log_string( log_buf );
        load_todelete_list( ); /* delayed ch delete - by Fuyara */

        sprintf(log_buf,"Loading learn system.");
        log_string( log_buf );
        load_learn_system();
        load_learn_trick_system();

        sprintf(log_buf,"Loading prewaits.");
        log_string( log_buf );
        load_prewait_system();

        sprintf(log_buf,"Loading boards.");
        log_string( log_buf );
        load_boards();

        sprintf(log_buf,"Setting weather.");
        log_string( log_buf );
        init_weather();

        sprintf(log_buf,"Loading languages.");
        log_string( log_buf );
        load_tongues();

        sprintf(log_buf,"Loading spell msg.");
        log_string( log_buf );
        load_spell_msg();

        sprintf(log_buf,"Loading spells ai data.");
        log_string( log_buf );
        si_setup_spell_data();

        sprintf(log_buf,"Loading name allow/disallow data.");
        log_string( log_buf );
        initialize_name_table();

        sprintf(log_buf,"Loading clans.");
        log_string( log_buf );
        load_clans();

        sprintf(log_buf,"Loading monk hits.");
        log_string( log_buf );
        load_monk_hits();

        sprintf(log_buf,"Artefact system: scaning players for artefacts.");
        log_string( log_buf );
        scan_players_for_artefacts();

        sprintf(log_buf,"Loading PC corpses.");
        log_string( log_buf );
        load_pccorpses();

        if ( misc.copyover_period > 0 )
        {
            misc.copyover_scheduled = current_time + misc.copyover_period;
        }

        sprintf(log_buf,"Zliczanie artefaktow: posiadacze+na mudzie");
        log_string( log_buf );
        count_artefacts();

        append_file_format_daily( NULL, MONEY_LOG_FILE, "SYSTEM: starting mud" );
        append_file_format_daily( NULL, SPEECH_LOG_FILE, "SYSTEM: starting mud" );

        /*
         * Set time and weather and moon.
         */
        {
            long lhour, lday, lmonth;

            lhour           = misc.world_time;
            time_info.hour	= lhour  % HOURS_PER_DAY;
            lday		    = lhour  / HOURS_PER_DAY;
            time_info.day	= lday   % DAYS_PER_MONTH;
            lmonth		    = lday   / DAYS_PER_MONTH;
            time_info.month	= lmonth % MONTHS_PER_YEAR;
            time_info.year	= lmonth / MONTHS_PER_YEAR;
            time_info.moon_phase           = (lhour / MOON_PHASE_DURATION) % MOON_PHASES;
            time_info.moon_phase_passed    = lhour % MOON_PHASE_DURATION;
        }

    }

    return;
}

/*
 * OLC
 * Use these macros to load any new area formats that you choose to
 * support on your MUD.  See the new_load_area format below for
 * a short example.
 */
#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                \
    if ( !str_cmp( word, literal ) )    \
{                                   \
    field  = value;                 \
    fMatch = TRUE;                  \
    break;                          \
}

#define SKEY( string, field )                       \
    if ( !str_cmp( word, string ) )     \
{                                   \
    free_string( field );           \
    field = fread_string( fp );     \
    fMatch = TRUE;                  \
    break;                          \
}



/* OLC
 * Snarf an 'area' header line.   Check this format.  MUCH better.  Add fields
 * too.
 *
 * #AREAFILE
 * Name   { All } Locke    Newbie School~
 * Repop  A teacher pops in the room and says, 'Repop coming!'~
 * Recall 3001
 * End
 */
void new_load_area( FILE *fp )
{
    AREA_DATA *pArea;
    char      *word;
    bool      fMatch;
    char buf[ MAX_STRING_LENGTH ];

    CREATE( pArea, AREA_DATA, 1 );
    pArea->age          = 15;
    pArea->resetage     = 15;
    pArea->nplayer      = 0;
    pArea->file_name    = str_dup( strArea );
    pArea->vnum         = top_area;
    pArea->name         = str_dup( "New Area" );
    pArea->builders     = str_dup( "" );
    pArea->locked     	= str_dup( "" );
    pArea->credits    	= str_dup( "" );
    pArea->security     = 9;
    pArea->min_vnum     = 0;
    pArea->max_vnum     = 0;
    pArea->area_flags   = 0;
    pArea->region       = 0;

    sprintf( buf, "load area: %s", pArea->file_name );
    log_string( buf );

    for ( ; ; )
    {
        word   = feof( fp ) ? "End" : fread_word( fp );
        fMatch = FALSE;

        switch ( UPPER(word[0]) )
        {
            case 'L':
                SKEY( "Locked", pArea->locked );
                break;

            case 'N':
                SKEY( "Name", pArea->name );
                break;

            case 'R':
                KEY( "ResetAge", pArea->resetage, fread_number( fp ) );
                KEY( "Region", pArea->region, fread_number( fp ) );
                break;

            case 'S':
                KEY( "Security", pArea->security, fread_number( fp ) );
                break;

            case 'V':
                if ( !str_cmp( word, "VNUMs" ) )
                {
                    pArea->min_vnum = fread_number( fp );
                    pArea->max_vnum = fread_number( fp );
                }
                break;

            case 'E':
                if ( !str_cmp( word, "End" ) )
                {
                    fMatch = TRUE;
                    pArea->age = pArea->resetage;
                    if ( area_first == NULL )
                        area_first = pArea;
                    if ( area_last  != NULL )
                        area_last->next = pArea;
                    area_last	= pArea;
                    pArea->next	= NULL;
                    current_area	= pArea;
                    top_area++;
                    return;
                }
                break;

            case 'B':
                SKEY( "Builders", pArea->builders );
                break;

            case 'C':
                SKEY( "Credits", pArea->credits );
                break;
        }
    }
}

/*
 * Sets vnum range for area using OLC protection features.
 */
void assign_area_vnum( ush_int vnum )
{
    if ( area_last->min_vnum == 0 || area_last->max_vnum == 0 )
        area_last->min_vnum = area_last->max_vnum = vnum;
    if ( vnum != URANGE( area_last->min_vnum, vnum, area_last->max_vnum ) )
    {
        if ( vnum < area_last->min_vnum )
            area_last->min_vnum = vnum;
        else
            area_last->max_vnum = vnum;
    }
    return;
}

/*
 * Snarf a help section.
 */
void load_helps( FILE *fp, char *fname )
{
    HELP_DATA *pHelp;
    int level;
    char *keyword;

    for ( ; ; )
    {
        HELP_AREA * had;

        level		= fread_number( fp );
        keyword		= fread_string( fp );

        if ( keyword[0] == '$' )
        {
            free_string( keyword );
            break;
        }

        if ( !had_list )
        {
            had			= new_had ();
            had->filename		= str_dup( fname );
            had->area		= current_area;
            if ( current_area )
                current_area->helps	= had;
            had_list		= had;
        }
        else
            if ( str_cmp( fname, had_list->filename ) )
            {
                had			= new_had ();
                had->filename		= str_dup( fname );
                had->area		= current_area;
                if ( current_area )
                    current_area->helps	= had;
                had->next		= had_list;
                had_list		= had;
            }
            else
                had			= had_list;

        pHelp		= new_help( );
        pHelp->level	= level;
        pHelp->keyword	= keyword;
        pHelp->text	= fread_string( fp );

        if ( !str_cmp( pHelp->keyword, "greeting" ) )
            help_greeting = pHelp->text;

        if ( help_first == NULL )
            help_first = pHelp;
        if ( help_last  != NULL )
            help_last->next = pHelp;

        help_last		= pHelp;
        pHelp->next		= NULL;

        if ( !had->first )
            had->first	= pHelp;
        if ( !had->last )
            had->last	= pHelp;

        had->last->next_area	= pHelp;
        had->last		= pHelp;
        pHelp->next_area	= NULL;

        top_help++;
    }

    return;
}

/*
 * Adds a reset to a room.  OLC
 * Similar to add_reset in olc.c
 */
void new_reset( ROOM_INDEX_DATA *pR, RESET_DATA *pReset )
{
    RESET_DATA *pr;

    if ( !pR )
        return;

    pr = pR->reset_last;

    if ( !pr )
    {
        pR->reset_first = pReset;
        pR->reset_last  = pReset;
    }
    else
    {
        pR->reset_last->next = pReset;
        pR->reset_last       = pReset;
        pR->reset_last->next = NULL;
    }

    /*    top_reset++; no estamos asignando memoria!!!! */

    return;
}

/*
 * Snarf a reset section.
 */
void load_resets( FILE *fp )
{
    RESET_DATA *pReset;
    int rVnum = -1;

    if ( !area_last )
    {
        bug( "Load_resets: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
        char letter;

        if ( ( letter = fread_letter( fp ) ) == 'S' )
            break;

        if ( letter == '*' || letter == 'D')
        {
            fread_to_eol( fp );
            continue;
        }

        pReset		= new_reset_data();
        pReset->command	= letter;

        pReset->arg0	= fread_number( fp );

        if( (pReset->arg0 == 0) || (pReset->arg0 == 1) )
            pReset->arg0=100;

        pReset->arg1	= fread_number( fp );
        pReset->arg2	= fread_number( fp );
        pReset->arg3	= (letter == 'G' || letter == 'R')
            ? 0 : fread_number( fp );
        pReset->arg4	= (letter == 'P' || letter == 'M')
            ? fread_number(fp) : 0;
        fread_to_eol( fp );

        switch( pReset->command )
        {
            case 'M':
            case 'O':
                rVnum = pReset->arg3;
                break;

            case 'P':
            case 'G':
            case 'E':
                break;

            case 'R':
                rVnum = pReset->arg1;
                break;
        }

        if ( rVnum == -1 )
        {
            bugf( "load_resets : rVnum == -1" );
            exit(1);
        }

        if ( pReset->command != 'D' )
            new_reset( get_room_index(rVnum), pReset );
        else
            free_reset_data( pReset );
    }

    return;
}

/*
 * Snarf a shop section.
 */
void load_shops( FILE *fp )
{
    SHOP_DATA *pShop;
    char c;
    int keeper;

    for ( ; ; )
    {
        MOB_INDEX_DATA *pMobIndex;
        int iTrade;

        keeper		= fread_number( fp );
        if ( keeper == 0 )
            break;

        CREATE( pShop, SHOP_DATA, 1 );
        pShop->keeper		= keeper;

        for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
            pShop->buy_type[iTrade]	= fread_number( fp );
        pShop->profit_buy	= fread_number( fp );
        pShop->profit_sell	= fread_number( fp );
        pShop->open_hour	= fread_number( fp );
        pShop->close_hour	= fread_number( fp );
        c = getc( fp );
        if ( c == '*' )
            pShop->shop_room = fread_number( fp );
        else ungetc( c, fp );
        fread_to_eol( fp );
        pMobIndex		= get_mob_index( pShop->keeper );
        pMobIndex->pShop	= pShop;

        if ( shop_first == NULL )
            shop_first = pShop;
        if ( shop_last  != NULL )
            shop_last->next = pShop;

        shop_last	= pShop;
        pShop->next	= NULL;
        top_shop++;
    }

    return;
}

/*
 * Snarf a repair section.
 */
void load_repairs( FILE *fp )
{
    REPAIR_DATA *repair, *prep;
    MOB_INDEX_DATA *pMobIndex;
    ush_int vnum;
    char c;

    for ( ; ; )
    {
        vnum = fread_number( fp );

        if ( vnum == 0 )
            break;

        pMobIndex		= get_mob_index( vnum );
        repair 			= new_repair();

        repair->item_type	      = fread_number( fp );
        repair->max_cost          = fread_number( fp );
        repair->repair_cost_mod   = fread_number( fp );
        repair->min_condition	  = fread_number( fp );
        repair->repair_open_hour  = fread_number( fp );
        repair->repair_close_hour = fread_number( fp );
        c = getc( fp );
        if ( c == '*' )
            repair->repair_room   = fread_number( fp );
        else
            ungetc( c, fp );

        repair->next		= NULL;

        if( !pMobIndex->pRepair )
            pMobIndex->pRepair = repair;
        else
        {
            for( prep = pMobIndex->pRepair; prep->next; prep = prep->next );
            prep->next = repair;
        }
    }

    return;
}

/*
 * Snarf a bank section.
 */
void load_banks( FILE *fp )
{
    BANK_DATA *pBank;
    char c;
    int banker;

    for ( ; ; )
    {
        MOB_INDEX_DATA *pMobIndex;
        banker		= fread_number( fp );

        if ( banker == 0 )
            break;

        CREATE( pBank, BANK_DATA, 1 );
        pBank->banker		= banker;

        pBank->provision	= fread_number( fp );
        pBank->open_hour	= fread_number( fp );
        pBank->close_hour	= fread_number( fp );
        c = getc( fp );
        if ( c == '*' )
            pBank->bank_room = fread_number( fp );
        else ungetc( c, fp );
        fread_to_eol( fp );

        pMobIndex		= get_mob_index( pBank->banker );
        pMobIndex->pBank	= pBank;

        if ( bank_first == NULL )
            bank_first = pBank;
        if ( bank_last  != NULL )
            bank_last->next = pBank;

        bank_last	= pBank;
        pBank->next	= NULL;
        top_bank++;
    }

    return;
}

/*
 * Snarf spec proc declarations.
 */
void load_specials( FILE *fp )
{
    for ( ; ; )
    {
        MOB_INDEX_DATA *pMobIndex;
        char letter;

        switch ( letter = fread_letter( fp ) )
        {
            default:
                bug( "Load_specials: letter '%c' not *MS.", letter );
                exit( 1 );

            case 'S':
                return;

            case '*':
                break;

            case 'M':
                pMobIndex		= get_mob_index	( fread_number ( fp ) );
                pMobIndex->spec_fun	= spec_lookup	( fread_word   ( fp ) );
                if ( pMobIndex->spec_fun == 0 )
                {
                    bugf( "Load_specials: mobile vnum %d: NULL spec_funk.", pMobIndex->vnum );
                    //exit( 1 );
                    break;
                }
                break;
        }

        fread_to_eol( fp );
    }
}


/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad reverse exits.
 */
#define NO_BUG_FIXEXIT 1

void fix_exits( void )
{
    extern const sh_int rev_dir [];
    char buf[ MAX_STRING_LENGTH ];
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;
    RESET_DATA *pReset;
    ROOM_INDEX_DATA *iLastRoom, *iLastObj;
    int iHash;
    int door;
    /* dodatek*/
    int last_mob = FALSE;

    bool debugThis = FALSE;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        if ( debugThis )
        {
            char buf[MSL];
            sprintf(buf, "fix_exits, iHash: %d", iHash);
            log_string(buf);
        }
        for ( pRoomIndex = room_index_hash[ iHash ];pRoomIndex != NULL;pRoomIndex = pRoomIndex->next )
        {
            bool fexit;

            iLastRoom = iLastObj = NULL;

            if ( debugThis )
            {
                char buf[MSL];
                sprintf(buf, "fix_exits, pRoomIndex->vnum: %d", pRoomIndex->vnum );
                log_string(buf);
            }

            /* OLC : nuevo chequeo de resets */
            for ( pReset = pRoomIndex->reset_first; pReset; pReset = pReset->next )
            {

                if ( debugThis )
                {
                    char buf[MSL];
                    sprintf(buf, "fix_exits, reset: %c : %d %d %d %d %d ", pReset->command, pReset->arg0, pReset->arg1, pReset->arg2, pReset->arg3, pReset->arg4 );
                    log_string(buf);
                }
                switch ( pReset->command )
                {
                    default:
                        bugf( "fix_exits : cuarto %d con reset cmd %c", pRoomIndex->vnum, pReset->command );
                        exit( 1 );
                        break;

                    case 'M':
                        get_mob_index( pReset->arg1 );
                        iLastRoom = get_room_index( pReset->arg3 );
                        last_mob = TRUE;
                        break;

                    case 'O':
                        get_obj_index( pReset->arg1 );
                        iLastObj = get_room_index( pReset->arg3 );
                        break;

                    case 'P':
                        get_obj_index( pReset->arg1 );
                        if ( iLastObj == NULL )
                        {
                            bugf( "fix_exits : reset en cuarto %d con iLastObj NULL", pRoomIndex->vnum );
                            exit( 1 );
                        }
                        break;

                    case 'G':
                    case 'E':
                        get_obj_index( pReset->arg1 );
                        if ( iLastRoom == NULL )
                        {

                            bugf( "fix_exits : reset en cuarto %d con iLastRoom NULL, area: %s", pRoomIndex->vnum, pRoomIndex->area->file_name );
                            bugf( "przeczytaj komentarz w kodzie to sie rozjasni o co biega.");
                            /* otoz tak. jest bug: jezeli mob ma reset nr. 0, a dalej jest item jako reset nr 1., to po skasowaniu resetu 0. zostaje reset 1. wtedy przy ladowaniu go mud nie wie komu ma go zaladowac i wywala ten bug. fix: usunac felerny reset, ktory to mozna dowiedziec sie ustawiajac debugThis na TRUE */
                            exit( 1 );
                        }
                        iLastObj = iLastRoom;

                        break;

                    case 'D':
                        bugf( "?" );
                        break;

                    case 'R':
                        get_room_index( pReset->arg1 );
                        if ( pReset->arg2 > MAX_DIR )
                        {
                            bugf( "fix_exits : reset en cuarto %d con arg2 %d >= MAX_DIR, area: %s",
                                    pRoomIndex->vnum, pReset->arg2, pRoomIndex->area->file_name );
                            exit( 1 );
                        }
                        break;
                } /* switch */
            } /* for */

            fexit = FALSE;
            for ( door = 0; door <= 5; door++ )
            {
                if ( ( pexit = pRoomIndex->exit[ door ] ) != NULL )
                {
                    if ( pexit->u1.vnum <= 0
                            || get_room_index( pexit->u1.vnum ) == NULL )
                        pexit->u1.to_room = NULL;
                    else
                    {
                        fexit = TRUE;
                        pexit->u1.to_room = get_room_index( pexit->u1.vnum );
                    }
                }
            }
            if ( !fexit )
                EXT_SET_BIT( pRoomIndex->room_flags, ROOM_NO_MOB );
        }
    }
    if ( NO_BUG_FIXEXIT ) return ;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for ( pRoomIndex = room_index_hash[ iHash ];
                pRoomIndex != NULL;
                pRoomIndex = pRoomIndex->next )
        {
            for ( door = 0; door <= 5; door++ )
            {
                if ( ( pexit = pRoomIndex->exit[ door ] ) != NULL
                        && ( to_room = pexit->u1.to_room ) != NULL
                        && ( pexit_rev = to_room->exit[ rev_dir[ door ] ] ) != NULL
                        && pexit_rev->u1.to_room != pRoomIndex
                        && ( pRoomIndex->vnum < 1200 || pRoomIndex->vnum > 1299 ) )
                {
                    sprintf( buf, "Fix_exits: %d:%d -> %d:%d -> %d.",
                            pRoomIndex->vnum, door,
                            to_room->vnum, rev_dir[ door ],
                            ( pexit_rev->u1.to_room == NULL )
                            ? 0 : pexit_rev->u1.to_room->vnum );
                    bug( buf, 0 );
                }
            }
        }
    }

    return ;
}

void fix_echoto( void )
{
    ROOM_INDEX_DATA *pRoomIndex;
    ROOM_INDEX_DATA *toRoom;
    int iHash;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for ( pRoomIndex = room_index_hash[iHash]; pRoomIndex != NULL ;pRoomIndex = pRoomIndex->next )
        {
            if ( pRoomIndex->echo_to.vnum <= 0 || ( toRoom = get_room_index( pRoomIndex->echo_to.vnum ) ) == NULL )
            {
                pRoomIndex->echo_to.room = NULL;
            }
            else
            {
                pRoomIndex->echo_to.room = toRoom;
            }
        }
    }

    return;
}

/*
 * Load mobprogs section
 */
void load_mobprogs( FILE *fp, bool new )
{
    PROG_CODE *pMprog;

    if ( area_last == NULL )
    {
        bug( "Load_mobprogs: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
        char * name;
        char letter;

        letter		  = fread_letter( fp );
        if ( letter != '#' )
        {
            bug( "Load_mobprogs: # not found.", 0 );
            exit( 1 );
        }

        name		 = fread_word( fp );
        if ( !str_cmp( name, "0" ) )
            break;

        fBootDb = FALSE;
        if ( get_mprog_index( name ) != NULL )
        {
            bugf( "Load_mobprogs: vnum %s duplicated.", name );
            exit( 1 );
        }
        fBootDb = TRUE;

        CREATE( pMprog, PROG_CODE, 1 );
        pMprog->name        = str_dup( name );
        if ( new )
            pMprog->description = fread_string( fp );
        else
            pMprog->description = str_dup( "" );
        pMprog->code        = fread_string( fp );
        pMprog->area        = current_area;
        if ( mprog_list == NULL )
            mprog_list = pMprog;
        else
        {
            pMprog->next = mprog_list;
            mprog_list 	= pMprog;
        }
        top_mprog_index++;
    }
    return;
}

/*
 *  Translate mobprog vnums pointers to real code
 */
void fix_mobprogs( void )
{
    MOB_INDEX_DATA *pMobIndex;
    PROG_LIST        *list;
    PROG_CODE        *prog;
    int iHash;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for ( pMobIndex   = mob_index_hash[iHash];
                pMobIndex   != NULL;
                pMobIndex   = pMobIndex->next )
        {
            for( list = pMobIndex->progs; list != NULL; list = list->next )
            {
                if ( ( prog = get_mprog_index( list->name ) ) != NULL )
                {
                    list->code = prog->code;
                    free_string( list->name );
                    list->name = prog->name;
                }
                else
                {
                    bugf( "Fix_mobprogs: code name %s not found.", list->name );
                    exit( 1 );
                }
            }
        }
    }
}

void load_objprogs( FILE *fp, bool new )
{
    PROG_CODE *pOprog;

    if ( area_last == NULL )
    {
        bug( "Load_objprogs: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
        char * name;
        char letter;

        letter		  = fread_letter( fp );
        if ( letter != '#' )
        {
            bugf( "Load_objprogs: # not found.", 0 );
            exit( 1 );
        }

        name		 = fread_word( fp );
        if ( !str_cmp( name, "0" ) )    break;

        fBootDb = FALSE;
        if ( get_oprog_index( name ) != NULL )
        {
            bugf( "Load_objprogs: name %s duplicated.", name );
            exit( 1 );
        }
        fBootDb = TRUE;

        CREATE( pOprog, PROG_CODE, 1 );
        pOprog->name        = str_dup( name );
        if ( new )
            pOprog->description = fread_string( fp );
        else
            pOprog->description = str_dup( "" );
        pOprog->code        = fread_string( fp );
        pOprog->area        = current_area;
        if ( oprog_list == NULL )
            oprog_list = pOprog;
        else
        {
            pOprog->next = oprog_list;
            oprog_list 	= pOprog;
        }
        top_oprog_index++;
    }
    return;
}

void load_roomprogs( FILE *fp, bool new )
{
    PROG_CODE *pRprog;

    if ( area_last == NULL )
    {
        bug( "Load_roomprogs: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
        char * name;
        char letter;

        letter		  = fread_letter( fp );
        if ( letter != '#' )
        {
            bug( "Load_roomprogs: # not found.", 0 );
            exit( 1 );
        }

        name		 = fread_word( fp );
        if ( !str_cmp( name, "0" ) )    break;

        fBootDb = FALSE;
        if ( get_rprog_index( name ) != NULL )
        {
            bugf( "Load_roomprogs: name %s duplicated.", name );
            exit( 1 );
        }
        fBootDb = TRUE;

        CREATE( pRprog, PROG_CODE, 1 );
        pRprog->name        = str_dup( name );
        if ( new )
            pRprog->description = fread_string( fp );
        else
            pRprog->description = str_dup( "" );
        pRprog->code        = fread_string( fp );
        pRprog->area        = current_area;
        if ( rprog_list == NULL )
            rprog_list = pRprog;
        else
        {
            pRprog->next = rprog_list;
            rprog_list 	= pRprog;
        }
        top_rprog_index++;
    }
    return;
}


void fix_objprogs( void )
{
    OBJ_INDEX_DATA *pObjIndex;
    PROG_LIST        *list;
    PROG_CODE        *prog;
    int iHash;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for ( pObjIndex   = obj_index_hash[iHash];
                pObjIndex   != NULL;
                pObjIndex   = pObjIndex->next )
        {
            for( list = pObjIndex->progs; list != NULL; list = list->next )
            {
                if ( ( prog = get_oprog_index( list->name ) ) != NULL )
                {
                    list->code = prog->code;
                    free_string( list->name );
                    list->name = prog->name;
                }
                else
                {
                    bugf( "Fix_objprogs: code name %s not found.", list->name );
                    exit( 1 );
                }
            }
        }
    }
}

void fix_roomprogs( void )
{
    ROOM_INDEX_DATA *room;
    PROG_LIST        *list;
    PROG_CODE        *prog;
    int iHash;

    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for ( room   = room_index_hash[iHash];
                room  != NULL;
                room   = room->next )
        {
            for( list = room->progs; list != NULL; list = list->next )
            {
                if ( ( prog = get_rprog_index( list->name ) ) != NULL )
                {
                    list->code = prog->code;
                    free_string( list->name );
                    list->name = prog->name;
                }
                else
                {
                    bugf( "Fix_roomprogs: code name %s not found.", list->name );
                    exit( 1 );
                }
            }
        }
    }
}

/*
 * Repopulate areas periodically.
 */
void area_update( void )
{
    AREA_DATA *pArea;
    char buf[MAX_STRING_LENGTH];
    int newage;

    for ( pArea = area_first; pArea != NULL; pArea = pArea->next )
    {
        if ( ++pArea->age < pArea->resetage )
            continue;

        reset_area( pArea );
        sprintf(buf,"%s wlasnie zostala zresetowana.",pArea->name);
        wiznet(buf,NULL,NULL,WIZ_RESETS,0,0);

        newage = number_range( 0, 3 );

        pArea->age = pArea->resetage <= newage ? 0 : newage;

        if (pArea->nplayer == 0)
            pArea->empty = TRUE;
    }
    return;
}

/* OLC
 * Reset one room.  Called by reset_area and olc.
 */
/* dodalem fragmencik do losowania szansy zaladowania moba/obj/eq*/

void reset_room( ROOM_INDEX_DATA *pRoom )
{
    char buf[ MAX_STRING_LENGTH ];
    RESET_DATA *pReset;
    CHAR_DATA *pMob;
    CHAR_DATA	*mob;
    OBJ_DATA *pObj;
    CHAR_DATA *LastMob = NULL;
    OBJ_DATA *LastObj = NULL;
    int iExit;
    int level = 0;
    bool last;
    bool last_mob = FALSE;

    if ( !pRoom )
        return ;


    pMob = NULL;
    last = FALSE;

    /* exity ?!? co ty robia? */
    for ( iExit = 0; iExit < MAX_DIR; iExit++ )
    {
        EXIT_DATA *pExit;

        if ( ( pExit = pRoom->exit[ iExit ] ) )
        {
            pExit->exit_info = pExit->rs_flags;

            if ( ( pExit->u1.to_room != NULL )
                    && ( ( pExit = pExit->u1.to_room->exit[ rev_dir[ iExit ] ] ) ) )
            {
                pExit->exit_info = pExit->rs_flags;
            }
        }
    }

    if ( pRoom->trap > 0 )
        EXT_SET_BIT( pRoom->room_flags, ROOM_TRAP );
    /* trig room reset idzie najpierw */
    rp_reset_trigger( pRoom );

    if ( pRoom->day_rand_desc > 0 )
    {
        RAND_DESC_DATA * rdesc;
        rdesc = get_rdesc_index( pRoom->day_rand_desc );

        if ( rdesc )
        {
            free_string( pRoom->description );
            create_rand_desc( pRoom, rdesc->rand_desc, buf );
            pRoom->description = str_dup( buf );
            pRoom->description = format_string( pRoom->description );
        }
    }

    if ( pRoom->night_rand_desc > 0 )
    {
        RAND_DESC_DATA * rdesc;
        rdesc = get_rdesc_index( pRoom->night_rand_desc );

        if ( rdesc )
        {
            free_string( pRoom->nightdesc );
            create_rand_desc( pRoom, rdesc->rand_desc, buf );
            pRoom->nightdesc = str_dup( buf );
            pRoom->nightdesc = format_string( pRoom->nightdesc );
        }
    }

    //rellik: mining, uzupe³nianie zasobno¶ci surowców
    pRoom->rawmaterial_capacity_now = pRoom->rawmaterial_capacity;

    if ( pRoom->herbs_count > 0 )
        pRoom->herbs_count--;

    for ( pReset = pRoom->reset_first; pReset != NULL; pReset = pReset->next )
    {
        MOB_INDEX_DATA * pMobIndex;
        OBJ_INDEX_DATA *pObjIndex;
        OBJ_INDEX_DATA *pObjToIndex;
        ROOM_INDEX_DATA *pRoomIndex;
        int count, limit = 0;


        switch ( pReset->command )
        {
            default:
                bug( "Reset_room: bad command %c.", pReset->command );
                break;

            case 'M':
                if ( number_percent() <= pReset->arg0 )
                {

                    if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
                    {
                        bug( "Reset_room: 'M': bad vnum %d.", pReset->arg1 );
                        continue;
                    }

                    if ( ( pRoomIndex = get_room_index( pReset->arg3 ) ) == NULL )
                    {
                        bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
                        continue;
                    }

                    if ( pMobIndex->count >= pReset->arg2 )
                        break;

                    count = 0;

                    for ( mob = pRoomIndex->people; mob != NULL; mob = mob->next_in_room )
                        if ( mob->pIndexData == pMobIndex )
                        {
                            count++;
                            if ( count >= pReset->arg4 )
                            {
                                last = FALSE;
                                break;
                            }
                        }

                    if ( count >= pReset->arg4 )
                        break;

                    pMob = create_mobile( pMobIndex );
                    char_to_room( pMob, pRoom );
                    pMob->reset_vnum = pRoom->vnum;

                    /*artefact*/
                    if ( is_artefact_load_mob( pMob ) == 1 ) load_artefact( NULL, NULL, pMob );

                    /* to idzie onload dla mobow */
                    if ( HAS_TRIGGER( pMob, TRIG_ONLOAD ) )
                        mp_onload_trigger( pMob );

                    LastMob = pMob;
                    level = URANGE( 0, pMob->level - 2, LEVEL_HERO - 1 ); /* -1 ROM */
                    last = TRUE;
                    last_mob = TRUE;
                }
                else
                    last_mob = FALSE;

                break;

            case 'O':
                if ( number_percent() <= pReset->arg0 )
                {

                    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
                    {
                        bugf( "Reset_room: 'Object' : bad obj vnum (%d), no such object", pReset->arg1 );
                        continue;
                    }

                    if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
                    {
                        bug( "Reset_room: 'Object' : room vnum %d not found.", pReset->arg3 );
                        continue;
                    }

                    /*            if ( pRoom->area->nplayer > 0
                                  || count_obj_list( pObjIndex, pRoom->contents ) > 0 )*/

                    if ( count_obj_list( pObjIndex, pRoom->contents ) > 0 )
                    {
                        for ( pObj = pRoom->contents; pObj != NULL; pObj = pObj->next_content )
                        {
                            if ( pObj->pIndexData == pObjIndex )
                                break;
                        }

                        if ( is_artefact_load_obj( pObjIndex ) == 1 && pObjIndex->item_type == ITEM_CONTAINER )
                            load_artefact( NULL, pObj, NULL );
                        last = FALSE;
                        break;
                    }

                    pObj = create_object( pObjIndex, FALSE );
                    if ( is_artefact( pObj ) == 1 )
                    {
                        extract_obj( pObj );
                        break;
                    }

                    obj_to_room( pObj, pRoom );
                    /*artefact*/
                    if ( is_artefact_load_obj( pObjIndex ) == 1 && pObjIndex->item_type == ITEM_CONTAINER )
                        load_artefact( NULL, pObj, NULL );

                    if ( HAS_OTRIGGER( pObj, TRIG_ONLOAD ) )
                        op_onload_trigger( pObj );

                    last = TRUE;
                }
                break;

            case 'P':
                if ( number_percent() <= pReset->arg0 )
                {
                    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
                    {
                        bug( "Reset_room: 'P': bad vnum %d, obj not found.", pReset->arg1 );
                        continue;
                    }


                    if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
                    {
                        bug( "Reset_room: 'P': bad vnum %d, obj not found.", pReset->arg3 );
                        continue;
                    }

                    limit = pReset->arg2;

                    if ( ( LastObj = get_obj_type( pRoom, pObjToIndex ) ) == NULL
                            || ( count = count_obj_list( pObjIndex, LastObj->contains ) ) > pReset->arg4 )
                    {
                        last = FALSE;
                        break;
                    }

                    while ( count < pReset->arg4 )
                    {
                        pObj = create_object( pObjIndex, FALSE );
                        /*artefact*/
                        if ( is_artefact( pObj ) == 1 )
                        {
                            extract_obj( pObj );
                            break;
                        }

                        obj_to_obj( pObj, LastObj );
                        /*artefact*/
                        if ( is_artefact_load_obj( pObjIndex ) == 1 && pObj->item_type == ITEM_CONTAINER )
                            load_artefact( NULL, pObj, NULL );

                        if ( HAS_OTRIGGER( pObj, TRIG_ONLOAD ) )
                            op_onload_trigger( pObj );

                        count++;

                        if ( pObjIndex->count >= limit )
                            break;
                    }


                    /* fix object lock state! */
                    LastObj->value[ 1 ] = LastObj->pIndexData->value[ 1 ];
                    last = TRUE;
                }
                break;

            case 'G':
            case 'E':
                if ( last_mob && ( number_percent() <= pReset->arg0 ) )
                {

                    if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
                    {
                        bug( "Reset_room: 'E' or 'G': bad vnum %d, obj not found.", pReset->arg1 );
                        continue;
                    }

                    if ( !last )
                        break;

                    if ( !LastMob )
                    {
                        bug( "Reset_room: 'E' or 'G': null mob for vnum %d.", pReset->arg1 );
                        last = FALSE;
                        break;
                    }

                    if ( LastMob->pIndexData->pShop )    /* Shop-keeper? */
                    {
                        pObj = create_object( pObjIndex, FALSE );
                        /*artefact*/
                        if ( is_artefact( pObj ) == 1 )
                        {
                            extract_obj( pObj );
                            break;
                        }
                        EXT_SET_BIT( pObj->extra_flags, ITEM_INVENTORY );
                    }
                    else
                    {
                        int limit;

                        if ( pReset->arg2 == 0 )
                            limit = 999;
                        else
                            limit = pReset->arg2;


                        if ( pObjIndex->count < limit )
                        {
                            pObj = create_object( pObjIndex, FALSE );
                            /*artefact*/
                            if ( is_artefact( pObj ) == 1 )
                            {
                                extract_obj( pObj );
                                break;
                            }
                        }
                        else
                            break;
                    }

                    obj_to_char( pObj, LastMob );
                    /*artefact*/
                    if ( is_artefact_load_obj( pObjIndex ) == 1 && pObj->item_type == ITEM_CONTAINER )
                        load_artefact( NULL, pObj, NULL );

                    if ( pReset->command == 'E' )
                        equip_char( LastMob, pObj, pReset->arg3, TRUE );

                    if ( !LastMob->pIndexData->pShop && HAS_OTRIGGER( pObj, TRIG_ONLOAD ) )
                        op_onload_trigger( pObj );

                    last = TRUE;
                }
                break;

            case 'D':
                break;

            case 'R':
                if ( !( pRoomIndex = get_room_index( pReset->arg1 ) ) )
                {
                    bug( "Reset_room: 'R': bad vnum %d, room not found.", pReset->arg1 );
                    continue;
                }

                {
                    EXIT_DATA *pExit;
                    int d0;
                    int d1;

                    for ( d0 = 0; d0 < pReset->arg2 - 1; d0++ )
                    {
                        d1 = number_range( d0, pReset->arg2 - 1 );
                        pExit = pRoomIndex->exit[ d0 ];
                        pRoomIndex->exit[ d0 ] = pRoomIndex->exit[ d1 ];
                        pRoomIndex->exit[ d1 ] = pExit;
                    }
                }
                break;
        }
    }

    return ;
}

/* OLC
 * Reset one area.
 */
void reset_area( AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *pRoom;
    int  vnum;

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
        if ( ( pRoom = get_room_index(vnum) ) )
        {
            /*artefact*/
            if(is_artefact_load_room(pRoom) == 1) load_artefact(pRoom,NULL,NULL);
            reset_room(pRoom);
        }
    }

    return;
}

/*
 * Create an instance of a mobile.
 */
// Tener 06-09-2007:
//                      UWAGA
// Na podstawie funkcji create_mobile napisa³em
// na potrzeby shapeshifta funkcjê make_shape_data.
// W razie zmian w create_mobile nale¿y obejrzeæ kod
// tamtej funkcji i wprowadziæ niezbêdne zmiany,
// by obie funkcje by³y kompatybilne i tworzy³y analogiczne
// struktury.

CHAR_DATA *create_mobile( MOB_INDEX_DATA *pMobIndex )
{
    CHAR_DATA *mob;
    int i;
    AFFECT_DATA af;

    if ( pMobIndex == NULL )
    {
        bug( "Create_mobile: NULL pMobIndex.", 0 );
        //exit( 1 );
        return NULL;
    }

    mobile_count++;

    mob = new_char();

    mob->pIndexData	= pMobIndex;

    mob->name                = str_dup( pMobIndex->player_name );    /* OLC */
    mob->short_descr         = str_dup( pMobIndex->short_descr );    /* OLC */
    mob->long_descr          = str_dup( pMobIndex->long_descr );     /* OLC */
    mob->description         = str_dup( pMobIndex->description );    /* OLC */
    mob->id                  = get_mob_id();
    mob->spec_fun            = pMobIndex->spec_fun;
    mob->attack_flags        = pMobIndex->attack_flags;
    mob->magical_damage      = pMobIndex->magical_damage;
    mob->weapon_damage_bonus = pMobIndex->weapon_damage_bonus;
    mob->prompt              = NULL;
    mob->prog_target         = NULL;


    mob->name2 = str_dup( pMobIndex->name2 );
    mob->name3 = str_dup( pMobIndex->name3 );
    mob->name4 = str_dup( pMobIndex->name4 );
    mob->name5 = str_dup( pMobIndex->name5 );
    mob->name6 = str_dup( pMobIndex->name6 );

    if (pMobIndex->wealth == 0)
    {
        money_reset_character_money( mob );
    }
    else
    {
        long int wealth;
        wealth = pMobIndex->wealth;
        wealth *= number_range( 75, 125 );
        wealth /= 100;
        money_gain( mob, wealth );
    }

    if (pMobIndex->new_format)
    {
        mob->group		= pMobIndex->group;
        ext_flags_copy( pMobIndex->act, mob->act );
        mob->comm		= COMM_NOSHOUT;

        for(i=0;i<MAX_VECT_BANK;i++)
            mob->affected_by[i]	= pMobIndex->affected_by[i];

        mob->alignment		= pMobIndex->alignment;
        mob->level		= pMobIndex->level;
        mob->hitroll		= pMobIndex->hitroll;
        mob->damroll		= 0;
        mob->max_hit		= dice(pMobIndex->hit[DICE_NUMBER],
                pMobIndex->hit[DICE_TYPE])
            + pMobIndex->hit[DICE_BONUS];
        mob->hit		= mob->max_hit;
        mob->damage[DICE_NUMBER]= pMobIndex->damage[DICE_NUMBER];
        mob->damage[DICE_TYPE]	= pMobIndex->damage[DICE_TYPE];
        mob->damage[DICE_BONUS] = pMobIndex->damage[DICE_BONUS];
        mob->dam_type		= pMobIndex->dam_type;

        if (mob->dam_type == 0)
            switch(number_range(1,3))
            {
                case (1): mob->dam_type = 3;        break;  /* slash */
                case (2): mob->dam_type = 7;        break;  /* pound */
                case (3): mob->dam_type = 11;       break;  /* pierce */
            }

        for (i = 0; i < 4; i++)
            mob->armor[i]	= pMobIndex->ac[i];

        for (i = 0; i < MAX_RESIST; i++)
        {
            mob->resists[i] = pMobIndex->resists[i];
            mob->healing_from[i] = pMobIndex->healing_from[i];
        }


        for (i = 0; i < MAX_STATS; i ++)
        {
            mob->perm_stat[i]    = pMobIndex->stats[i];
            mob->new_mod_stat[i] = 0;
        }

        ext_flags_copy( pMobIndex->off_flags, mob->off_flags );
        mob->start_pos		= pMobIndex->start_pos;
        mob->default_pos	= pMobIndex->default_pos;
        mob->sex		= pMobIndex->sex;
        mob->speaking 		= pMobIndex->speaking;

        if (mob->sex == 3) /* random sex */
            mob->sex = number_range(1,2);

        mob->race		= pMobIndex->race;
        mob->form		= pMobIndex->form;
        mob->parts		= pMobIndex->parts;
        mob->size		= pMobIndex->size;
        mob->material		= str_dup(pMobIndex->material);

        mob->precommand_pending    = FALSE;
        mob->precommand_fun        = NULL;
        mob->precommand_arg        = NULL;


        /* let's get some spell action */
        if (IS_AFFECTED(mob,AFF_SANCTUARY))
        {
            af.where	 = TO_AFFECTS;
            af.type      = skill_lookup("sanctuary");
            af.level     = mob->level;
            af.duration  = -1;
            af.rt_duration = 0;
            af.location  = APPLY_NONE;
            af.modifier  = 0;
            af.bitvector = &AFF_SANCTUARY;
            affect_to_char( mob, &af, NULL, TRUE );
        }

        if (IS_AFFECTED(mob,AFF_FIRESHIELD))
        {
            af.where	 = TO_AFFECTS;
            af.type      = skill_lookup("fireshield");
            af.level     = 50;
            af.duration  = -1;
            af.rt_duration = 0;
            af.location  = APPLY_RESIST;
            af.modifier  = RESIST_FIRE;
            af.bitvector = &AFF_FIRESHIELD;
            affect_to_char( mob, &af, NULL, TRUE );
        }

        if (IS_AFFECTED(mob,AFF_ICESHIELD) &&
                !IS_AFFECTED(mob,AFF_FIRESHIELD))
        {
            af.where	 = TO_AFFECTS;
            af.type      = skill_lookup("iceshield");
            af.level     = 50;
            af.duration  = -1;
            af.rt_duration = 0;
            af.location  = APPLY_RESIST;
            af.modifier  = RESIST_COLD;
            af.bitvector = &AFF_ICESHIELD;
            affect_to_char( mob, &af, NULL, TRUE );
        }

        if (IS_AFFECTED(mob,AFF_REFLECT_SPELL))
        {
            af.where	 = TO_AFFECTS;
            af.type      = gsn_reflect_spell;
            af.level     = 0;
            af.duration  = -1;
            af.rt_duration = 0;
            af.location  = APPLY_NONE;
            af.modifier  = -1;
            af.bitvector = &AFF_REFLECT_SPELL;
            affect_to_char( mob, &af, NULL, TRUE );
        }

        if (IS_AFFECTED(mob,AFF_STONE_SKIN))
        {
            af.where	 = TO_AFFECTS;
            af.type      = gsn_stone_skin;
            af.level     = mob->level;
            af.duration  = -1;
            af.rt_duration = 0;
            af.location  = APPLY_NONE;
            af.modifier  = 1073741824;
            af.bitvector = &AFF_STONE_SKIN;
            affect_to_char( mob, &af, NULL, TRUE );
        }

        if (IS_AFFECTED(mob,AFF_HASTE))
        {
            af.where = TO_AFFECTS;
            af.type = skill_lookup("haste");
            af.level = mob->level;
            af.duration = -1;
            af.rt_duration = 0;
            af.location = APPLY_DEX;
            af.modifier = 6 * ( 1 + (mob->level > 17) + (mob->level > 24) + (mob->level > 31) );
            af.bitvector = &AFF_HASTE;
            affect_to_char(mob, &af, NULL, TRUE);
        }

        if (IS_AFFECTED(mob,AFF_PROTECT_EVIL))
        {
            af.where	 = TO_AFFECTS;
            af.type	 = skill_lookup("protection evil");
            af.level	 = mob->level;
            af.duration	 = -1;
            af.rt_duration = 0;
            af.location	 = APPLY_SAVING_SPELL;
            af.modifier	 = -1;
            af.bitvector = &AFF_PROTECT_EVIL;
            affect_to_char(mob,&af, NULL, TRUE);
        }

        if (IS_AFFECTED(mob,AFF_PROTECT_GOOD))
        {
            af.where	 = TO_AFFECTS;
            af.type      = skill_lookup("protection good");
            af.level     = mob->level;
            af.duration  = -1;
            af.rt_duration = 0;
            af.location  = APPLY_SAVING_SPELL;
            af.modifier  = -1;
            af.bitvector = &AFF_PROTECT_GOOD;
            affect_to_char(mob,&af, NULL, TRUE);
        }
    }
    else /* read in old format and convert */
    {
        ext_flags_copy( pMobIndex->act, mob->act );
        for(i=0;i<MAX_VECT_BANK;i++)
        {
            mob->affected_by[i] = pMobIndex->affected_by[i];
        }

        mob->alignment = pMobIndex->alignment;
        mob->level     = pMobIndex->level;
        mob->hitroll   = pMobIndex->hitroll;
        mob->damroll   = 0;
        mob->max_hit   = mob->level * 8 + number_range ( mob->level * mob->level/4, mob->level * mob->level);
        mob->max_hit  *= .9;

        switch(number_range(1,3))
        {
            case (1):
                mob->dam_type = 3;
                break;  /* slash */
            case (2):
                mob->dam_type = 7;
                break;  /* pound */
            case (3):
                mob->dam_type = 11;
                break;  /* pierce */
        }
        for (i = 0; i < 3; i++)
        {
            mob->armor[i] = interpolate(mob->level,100,-100);
        }

        mob->armor[3]    = interpolate(mob->level,100,0);
        mob->race        = pMobIndex->race;
        ext_flags_copy( pMobIndex->off_flags, mob->off_flags );
        mob->start_pos   = pMobIndex->start_pos;
        mob->default_pos = pMobIndex->default_pos;
        mob->sex         = pMobIndex->sex;
        mob->form        = pMobIndex->form;
        mob->parts       = pMobIndex->parts;
        mob->size        = SIZE_MEDIUM;
        mob->material    = "";

        for (i = 0; i < MAX_STATS; i ++)
        {
            mob->perm_stat[i] = number_range(50,70) + 2 * mob->level;
        }
    }

    mob->hit		= get_max_hp(mob);

    mob->position = mob->start_pos;

    if( mob->perm_stat[STAT_INT] > number_range(25, 50) )
    {
        EXT_SET_BIT(mob->act, ACT_MEMORY);
    }

    /* link the mob to the world list */
    mob->next		= char_list;
    char_list		= mob;
    fill_slots(mob);

    switch(mob->size)
    {
        case SIZE_TINY:
            mob->weight = number_range(1, 300 );
            mob->height = number_range(1, 40 );
            break;
        case SIZE_SMALL:
            mob->weight = number_range( 250, 450 );
            mob->height = number_range(40, 100 );
            break;
        default:
        case SIZE_MEDIUM:
            mob->weight = number_range( 650, 1200 );
            mob->height = number_range(110, 210 );
            break;
        case SIZE_LARGE:
            mob->weight = number_range( 850, 2200 );
            mob->height = number_range(210, 260 );
            break;
        case SIZE_HUGE:
            mob->weight = number_range( 2100, 3700 );
            mob->height = number_range( 260, 400 );
            break;
        case SIZE_GIANT:
            mob->weight = number_range( 3500, 6000 );
            mob->height = number_range(400, 700 );
            break;
    }

    //Rysand: MV pocz±tkowy mountów, pocz±tkowa kondycja mountów
    if(	EXT_IS_SET(mob->act, ACT_MOUNTABLE))
    {

        mob->condition[COND_HUNGER] = EAT_FULL;
        mob->condition[COND_THIRST] = DRINK_FULL;

        //mounty maj± odrêbne liczenie mv - na podstawie:
        //wspó³czynnika prezesa
        //kupy szczê¶cia (co reset bêdzie inaczej - wiem!)
        //kondycji
        //rozmiaru
        //widzimisiê kodera


        //do wyboru kombinacje - powinny mniej wiêcej dawaæ tyle samo, je¶li builderzy
        //bêda przesadzaæ, to mo¿na daæ opcjê 2, je¶li nie to 1
        mob->max_move = 50 + number_range(0, 10) + get_curr_stat_deprecated(mob, STAT_CON)*5 + mob->size * 4;
        //mob->max_move = 40 + number_range(10, 40) + get_curr_stat_deprecated(mob, STAT_CON) + mob->size * 2;


        //arbitralnie przyjête przeliczniki - mo¿na balansowaæ wedle uznania
        if(mob->race == race_lookup("koñ"))
            mob->max_move += +20;
        else if(mob->race == race_lookup("pegaz"))
            mob->max_move += +50;
        else if(mob->race == race_lookup("krowa"))
            mob->max_move += +7;
        else if(mob->race == race_lookup("wilk"))
            mob->max_move += -5;
        else if(mob->race == race_lookup("s³oñ"))
            mob->max_move += +10;
        else if(mob->race == race_lookup("jednoro¿ec"))
            mob->max_move += +30;
        else if(mob->race == race_lookup("dzik"))
            mob->max_move += -5;
        else if(mob->race == race_lookup("tygrys"))
            mob->max_move += -10;
        else if(mob->race == race_lookup("jeleñ"))
            mob->max_move += +10;
        else if(mob->race == race_lookup("gryf"))
            mob->max_move += +40;
        else if(mob->race == race_lookup("¶winia"))
            mob->max_move += -20;
        else if(mob->race == race_lookup("piekielny ogar"))
            mob->max_move += +10;
        else if(mob->race == race_lookup("mantykora"))
            mob->max_move += +30;
        else if(mob->race == race_lookup("feniks"))
            mob->max_move += +40;
        else if(mob->race == race_lookup("satyr"))
            mob->max_move += +20;
        else
            mob->max_move += -10;
    }
    else
    {
        mob->max_move = 100;
    }

    //po za³adowaniu mob ma byæ wypoczêty
    mob->move = mob->max_move;

    pMobIndex->count++;
    return mob;
}

/* duplicate a mobile exactly -- except inventory */
void clone_mobile(CHAR_DATA *parent, CHAR_DATA *clone)
{
    int i;
    AFFECT_DATA *paf;
    char name[ MAX_INPUT_LENGTH ];

    //wywalam ta blokade dla !IS_NPC, potrzebne dla spella fetch
    if ( parent == NULL || clone == NULL /*|| !IS_NPC(parent)*/)
    {
        return;
    }

    /* start fixing values */
    sprintf( name, "clone_of_%s %s", parent->name, parent->name );
    clone->name 	= str_dup( name );

    clone->name2 	= str_dup(parent->name2);
    clone->name3 	= str_dup(parent->name3);
    clone->name4	= str_dup(parent->name4);
    clone->name5 	= str_dup(parent->name5);
    clone->name6 	= str_dup(parent->name6);

    clone->version     = parent->version;
    clone->short_descr = str_dup(parent->short_descr);
    clone->long_descr  = str_dup(parent->long_descr);
    clone->description = str_dup(parent->description);
    clone->group       = parent->group;
    clone->sex         = parent->sex;
    clone->class       = parent->class;
    clone->race        = parent->race;
    clone->level       = parent->level;
    clone->trust       = 0;
    clone->timer       = parent->timer;
    clone->wait        = parent->wait;
    clone->hit         = parent->hit;
    clone->max_hit     = parent->max_hit;
    clone->move        = parent->move;
    clone->max_move    = parent->max_move;
    clone->copper      = parent->copper;
    clone->silver       = parent->silver;
    clone->gold        = parent->gold;
    clone->mithril     = parent->mithril;
    clone->exp         = parent->exp;
    ext_flags_copy( parent->act, clone->act );
    clone->comm        = parent->comm;
    clone->invis_level = parent->invis_level;

    for(i=0;i<MAX_VECT_BANK;i++)
    {
        clone->affected_by[i] = parent->affected_by[i];
    }

    clone->position = parent->position;

    for(i=0;i<5;i++)
    {
        clone->saving_throw[i]= parent->saving_throw[i];
    }

    clone->alignment    = parent->alignment;
    clone->hitroll      = parent->hitroll;
    clone->damroll      = parent->damroll;
    clone->wimpy        = parent->wimpy;
    clone->form         = parent->form;
    clone->parts        = parent->parts;
    clone->size         = parent->size;
    clone->material     = str_dup(parent->material);
    ext_flags_copy( parent->off_flags, clone->off_flags );
    clone->dam_type     = parent->dam_type;
    clone->start_pos    = parent->start_pos;
    clone->default_pos  = parent->default_pos;
    clone->spec_fun     = parent->spec_fun;
    clone->attack_flags = parent->attack_flags;

    for (i = 0; i < 4; i++)
    {
        clone->armor[i] = parent->armor[i];
    }

    for (i = 0; i < MAX_STATS; i++)
    {
        clone->perm_stat[i]    = parent->perm_stat[i];
        clone->new_mod_stat[i] = parent->new_mod_stat[i];
    }

    for (i = 0; i < 3; i++)
    {
        clone->damage[i] = parent->damage[i];
    }

    /* now add the affects */
    for (paf = parent->affected; paf != NULL; paf = paf->next)
    {
        affect_to_char(clone,paf, paf->info, paf->visible );
    }
}

/*
 * Create an instance of an object.
 */
OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex, bool remote )
{
    AFFECT_DATA *paf;
    OBJ_DATA *obj;
    int i;

    if ( pObjIndex == NULL )
    {
        bug( "Create_object: NULL pObjIndex.", 0 );
        return NULL;
    }

    /*artefact*/
    //    if( create_artefact(pObjIndex->vnum) == 0 );
    /*tu powinen byc jakis zakaz*/

    obj = new_obj();

    obj->pIndexData  = pObjIndex;
    obj->in_room     = NULL;
    obj->enchanted   = FALSE;

    obj->wear_loc    = -1;
    obj->prewear_loc = -1;

    obj->name        = str_dup( pObjIndex->name );           /* OLC */
    obj->name2       = str_dup(pObjIndex->name2);
    obj->name3       = str_dup(pObjIndex->name3);
    obj->name4       = str_dup(pObjIndex->name4);
    obj->name5       = str_dup(pObjIndex->name5);
    obj->name6       = str_dup(pObjIndex->name6);
    obj->short_descr = str_dup( pObjIndex->short_descr );    /* OLC */
    obj->description = str_dup( pObjIndex->description );    /* OLC */

    if(pObjIndex->item_description)
    {
        obj->item_description = str_dup( pObjIndex->item_description );
    }
    if(pObjIndex->ident_description)
    {
        obj->ident_description = str_dup( pObjIndex->ident_description );
    }
    if(pObjIndex->hidden_description)
    {
        obj->hidden_description = str_dup( pObjIndex->hidden_description );
    }

    obj->material     = pObjIndex->material;
    obj->condition    = pObjIndex->condition;
    obj->item_type    = pObjIndex->item_type;
    ext_flags_copy( pObjIndex->extra_flags, obj->extra_flags );
    obj->wear_flags   = pObjIndex->wear_flags;
    ext_flags_copy( pObjIndex->wear_flags2, obj->wear_flags2 );
    obj->trap         = pObjIndex->trap;
    obj->value[0]     = pObjIndex->value[0];
    obj->value[1]     = pObjIndex->value[1];
    obj->value[2]     = pObjIndex->value[2];
    obj->value[3]     = pObjIndex->value[3];
    obj->value[4]     = pObjIndex->value[4];
    obj->value[5]     = pObjIndex->value[5];
    obj->value[6]     = pObjIndex->value[6];
    obj->weight       = pObjIndex->weight;
    obj->liczba_mnoga = pObjIndex->liczba_mnoga;
    obj->gender       = pObjIndex->gender;

    obj->cost         = pObjIndex->cost;

    //rellik: komponenty, dane z definicji do instancji
    obj->is_spell_item      = pObjIndex->is_spell_item;
    obj->spell_item_counter = pObjIndex->spell_item_counter;
    obj->spell_item_timer   = pObjIndex->spell_item_timer;

    obj->spec_dam = copy_specdam( pObjIndex->spec_dam );

    /**
     * Repair limits
     */
    obj->repair_limit     = pObjIndex->repair_limit;
    obj->repair_counter   = pObjIndex->repair_counter;
    obj->repair_penalty   = pObjIndex->repair_penalty;
    obj->repair_condition = pObjIndex->repair_condition;

    if ( obj->repair_limit == 0 && obj->repair_counter == 0 && obj->repair_penalty == 0 && obj->repair_condition == 0 )
    {
        obj->repair_penalty = 1;
        obj->repair_condition = 100;
    }

    /*
     * Mess with object properties.
     */
    switch ( obj->item_type )
    {
        default:
            bug( "Create_object: vnum %d bad type.", pObjIndex->vnum );
            break;

        case ITEM_LIGHT:
            if (obj->value[2] == 999)
            {
                obj->value[2] = -1;
            }
            break;

        case ITEM_FURNITURE:
        case ITEM_BOARD :
        case ITEM_TRASH:
        case ITEM_CONTAINER:
        case ITEM_DRINK_CON:
        case ITEM_KEY:
        case ITEM_FOOD:
        case ITEM_BOAT:
        case ITEM_CORPSE_NPC:
        case ITEM_CORPSE_PC:
        case ITEM_FOUNTAIN:
        case ITEM_MAP:
        case ITEM_CLOTHING:
        case ITEM_PORTAL:
            if (!pObjIndex->new_format)
            {
                obj->cost /= 5;
            }
            break;

        case ITEM_TREASURE:
        case ITEM_ROOM_KEY:
        case ITEM_GEM:
        case ITEM_JEWELRY:
        case ITEM_PIECE:
        case ITEM_ENVENOMER:
        case ITEM_BANDAGE:
        case ITEM_TURN:
        case ITEM_HERB:
        case ITEM_SKIN:
        case ITEM_HORN:
        case ITEM_MUSICAL_INSTRUMENT:
        case ITEM_TROPHY:
        case ITEM_PIPE:
        case ITEM_WEED:
        case ITEM_NOTEPAPER:
        case ITEM_TOTEM:
        case ITEM_TOOL:
        case ITEM_SCROLL:
        case ITEM_WEAPON:
        case ITEM_SPELLBOOK:
        case ITEM_SPELLITEM:
        case ITEM_ARMOR:
        case ITEM_POTION:
        case ITEM_PILL:
        case ITEM_SHIELD:
            break;

        case ITEM_JUKEBOX:
            for (i = 0; i < 5; i++)
            {
                obj->value[i] = -1;
            }
            break;

        case ITEM_WAND:
        case ITEM_STAFF:
            if (!pObjIndex->new_format)
            {
                obj->cost *= 2;
            }
            break;

        case ITEM_MONEY:
            if (!pObjIndex->new_format)
            {
                obj->value[0] =  obj->cost;
            }
            break;
    }

    for (paf = pObjIndex->affected; paf != NULL; paf = paf->next)
    {
        if ( paf->location == APPLY_SPELL_AFFECT )
        {
            affect_to_obj(obj,paf);
        }
    }

    if ( !remote )
    {
        obj->next   = object_list;
        object_list = obj;
    }
    pObjIndex->count++;

    obj->rent_cost = RENT_COST( obj->cost );

    return obj;
}

/* duplicate an object exactly -- except contents */
void clone_object(OBJ_DATA *parent, OBJ_DATA *clone)
{
    int i;
    AFFECT_DATA *paf;
    EXTRA_DESCR_DATA *ed,*ed_new;

    if (parent == NULL || clone == NULL)
        return;

    /* start fixing the object */
    clone->name 		=str_dup(parent->name);
    clone->name2		=str_dup(parent->name2);
    clone->name3		=str_dup(parent->name3);
    clone->name4		=str_dup(parent->name4);
    clone->name5		=str_dup(parent->name5);
    clone->name6		=str_dup(parent->name6);


    clone->short_descr 	= str_dup(parent->short_descr);
    clone->description	= str_dup(parent->description);

    if(parent->item_description)
        clone->item_description	= str_dup(parent->item_description);
    if(parent->ident_description)
        clone->ident_description	= str_dup(parent->ident_description);
    if(parent->hidden_description)
        clone->hidden_description	= str_dup(parent->hidden_description);

    clone->item_type	= parent->item_type;
    ext_flags_copy( parent->extra_flags, clone->extra_flags );
    clone->wear_flags	= parent->wear_flags;
    ext_flags_copy( parent->wear_flags2, clone->wear_flags2 );
    clone->weight	= parent->weight;
    clone->trap		= parent->trap;
    clone->cost		= parent->cost;
    /*    clone->level	= parent->level;*/
    clone->condition	= parent->condition;
    /*    clone->material	= str_dup(parent->material);*/
    clone->material	= parent->material;
    clone->timer	= parent->timer;

    //rellik: komponenty, klonowanie w³a¶ciwo¶ci komponentu
    clone->is_spell_item = parent->is_spell_item;
    clone->spell_item_counter = parent->spell_item_counter;
    clone->spell_item_timer = parent->spell_item_timer;

    clone->rent_cost = parent->rent_cost;

    for (i = 0;  i < 7; i ++)
        clone->value[i]	= parent->value[i];

    /* affects */
    clone->enchanted	= parent->enchanted;

    for (paf = parent->affected; paf != NULL; paf = paf->next)
        affect_to_obj(clone,paf);

    /* extended desc */
    for (ed = parent->extra_descr; ed != NULL; ed = ed->next)
    {
        ed_new                  = new_extra_descr();
        ed_new->keyword    	= str_dup( ed->keyword);
        ed_new->description     = str_dup( ed->description );
        ed_new->next           	= clone->extra_descr;
        clone->extra_descr  	= ed_new;
    }

    clone->spec_dam = copy_specdam( parent->spec_dam );

}



/*
 * Clear a new character.
 */
void clear_char( CHAR_DATA *ch )
{
    static CHAR_DATA ch_zero;
    int i;

    *ch				= ch_zero;
    ch->name			= &str_empty[0];
    ch->short_descr		= &str_empty[0];
    ch->long_descr		= &str_empty[0];
    ch->description		= &str_empty[0];
    ch->prompt                  = &str_empty[0];
    ch->logon			= current_time;
    ch->lines			= PAGELEN;
    ch->magic_data		= 0;
    ch->ch_left			= NULL;
    ch->ch_right		= NULL;

    for (i = 0; i < 4; i++)
    {
        ch->armor[i] = 100;
    }

    for(i=0;i < MAX_COUNTER;i++)
    {
        ch->counter[i] = 0;
    }

    ch->position		= POS_STANDING;
    ch->hit				= 20;
    ch->max_hit			= 20;
    ch->max_move		= 100;
    ch->on				= NULL;

    for (i = 0; i < MAX_STATS; i ++)
    {
        ch->perm_stat[i]    = 70;
        ch->new_mod_stat[i] = 0;
    }

    for (i = 0; i < MAX_RESIST; i ++)
    {
        ch->resists[i] = 0;
        ch->healing_from[i] = 0;
    }
    return;
}

/*
 * Get an extra description from a list.
 */
char *get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
    for ( ; ed != NULL; ed = ed->next )
    {
        if ( ed->keyword[0] == '_' )
            continue;

        if ( is_name( (char *) name, ed->keyword ) )
            return ed->description;
    }
    return NULL;
}



/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA *get_mob_index( ush_int vnum )
{
    MOB_INDEX_DATA *pMobIndex;

    for ( pMobIndex  = mob_index_hash[vnum % MAX_KEY_HASH];
            pMobIndex != NULL;
            pMobIndex  = pMobIndex->next )
    {
        if ( pMobIndex->vnum == vnum )
            return pMobIndex;
    }

    if ( fBootDb )
    {
        bug( "Get_mob_index: bad vnum %d.", vnum );
        exit( 1 );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA *get_obj_index( ush_int vnum )
{
    OBJ_INDEX_DATA *pObjIndex;

    for ( pObjIndex  = obj_index_hash[vnum % MAX_KEY_HASH];
            pObjIndex != NULL;
            pObjIndex  = pObjIndex->next )
    {
        if ( pObjIndex->vnum == vnum )
            return pObjIndex;
    }

    if ( fBootDb )
    {
        bug( "Get_obj_index: obj vnum %d nie istnieje.", vnum );
        exit( 1 );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA *get_room_index( ush_int vnum )
{
    ROOM_INDEX_DATA *pRoomIndex;

    for ( pRoomIndex  = room_index_hash[vnum % MAX_KEY_HASH];
            pRoomIndex != NULL;
            pRoomIndex  = pRoomIndex->next )
    {
        if ( pRoomIndex->vnum == vnum )
            return pRoomIndex;
    }

    if ( fBootDb )
#ifdef OFFLINE
        return get_room_index( 1 );
#else
    {
        bug( "Get_room_index: bad vnum room %d nie istnieje.", vnum );
        exit( 1 );
    }
#endif

    return NULL;
}

PROG_CODE *get_mprog_index( char * name )
{
    PROG_CODE *prg;
    for( prg = mprog_list; prg; prg = prg->next )
    {
        if ( FAST_STR_CMP( prg->name, name ) )
            return( prg );
    }
    return NULL;
}


PROG_CODE *get_oprog_index( char * name )
{
    PROG_CODE *prg;
    for( prg = oprog_list; prg; prg = prg->next )
    {
        if ( FAST_STR_CMP( prg->name, name ) )
            return( prg );
    }
    return NULL;
}


PROG_CODE *get_rprog_index( char * name )
{
    PROG_CODE *prg;
    for( prg = rprog_list; prg; prg = prg->next )
    {
        if ( FAST_STR_CMP( prg->name, name ) )
            return( prg );
    }
    return NULL;
}

TRAP_DATA *get_trap_index( ush_int vnum )
{
    TRAP_DATA *trap;

    for( trap = trap_list; trap; trap = trap->next )
    {
        if ( trap->vnum == vnum )
            return( trap );
    }
    return NULL;
}

RAND_DESC_DATA *get_rdesc_index( ush_int vnum )
{
    RAND_DESC_DATA *rdesc;

    for( rdesc = rand_desc_list; rdesc; rdesc = rdesc->next )
    {
        if ( rdesc->vnum == vnum )
            return( rdesc );
    }
    return NULL;
}

DESC_DATA *get_desc_index( AREA_DATA* pArea, char* name )
{
    DESC_DATA *desc;

    for( desc = pArea->desc_data; desc; desc = desc->next )
    {
        if ( NOPOL(desc->name[0]) == NOPOL(name[0]) &&  !str_cmp( desc->name, name ) )
            return ( desc );
    }
    return NULL;
}

BONUS_INDEX_DATA *get_bonus_index( ush_int vnum )
{
    BONUS_INDEX_DATA *bonus;

    for( bonus = bonus_list; bonus; bonus = bonus->next )
    {
        if ( bonus->vnum == vnum )
            return ( bonus );
    }
    return NULL;
}

SONG_DATA *get_song_index( ush_int number )
{
    SONG_DATA *song;
    int count;

    for( count = 1, song = song_list; song; count++, song = song->next )
    {
        if ( count == number )
            return ( song );
    }

    return NULL;
}

HERB_DATA *get_herb_index( char * id )
{
    HERB_DATA *herb;

    for( herb = herb_list; herb; herb = herb->next )
    {
        if ( FAST_STR_CMP( herb->id, id ) )
            return ( herb );
    }
    return NULL;
}

/*
 * Read a letter from a file.
 */
char fread_letter( FILE *fp )
{
    char c;

    do
    {
        c = getc( fp );
    }
    while ( isspace(c) );

    return c;
}

/*
 * Lookahead, if there is anything different from whitespace before next newline (if any).
 * If so, return that char. Else, return '\0'
 */

int next_nonspace_before_newline( FILE *fp )
{
    fpos_t pos;
    if ( fgetpos( fp, &pos ) ) // sprawdzenie czy byl blad
    {
        save_debug_info( "db.c => next_nonspace_before_newline", NULL, "nie udalo sie zapisac pozycji w pliku", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ERROR, TRUE );
        return '\0';
    }
    char c;
    while(TRUE)
    {
        c = getc( fp );
        if ( (c == '\n') || (c == EOF) )
        {
            if ( fsetpos( fp, &pos ) )
            {
                save_debug_info( "db.c => next_nonspace_before_newline", NULL, "KRYTYCZNE: nie udalo sie przywrocic pozycji w pliku", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ERROR, TRUE );
            }
            return '\0';
        }
        if ( ! isspace( c ) )
            break;
    }
    if ( fsetpos( fp, &pos ) )
    {
        save_debug_info( "db.c => next_nonspace_before_newline", NULL, "KRYTYCZNE: nie udalo sie przywrocic pozycji w pliku", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ERROR, TRUE );
    }
    return c;
}

/*
 * Read a number from a file.
 */
int fread_number( FILE *fp )
{
    int number;
    bool sign;
    char c;

    do
    {
        c = getc( fp );
    }
    while ( isspace(c) );

    number = 0;

    sign   = FALSE;
    if ( c == '+' )
    {
        c = getc( fp );
    }
    else if ( c == '-' )
    {
        sign = TRUE;
        c = getc( fp );
    }

    if ( !isdigit(c) )
    {
        number = 30000; //rellik: zmieniam na 30000 mo¿na wykrywaæ ta warto¶æ jako b³±d, lepsze to ni¿ nic
        save_debug_info( "db.c => fread_number", NULL, "przy odczycie spodziwana byla liczba", DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ERROR, TRUE );
        return number;
        /*bug( "Fread_number: bad format.", 0 );
          exit( 1 );*/
    }

    while ( isdigit(c) )
    {
        number = number * 10 + c - '0';
        c      = getc( fp );
    }

    if ( sign )
        number = 0 - number;

    if ( c == '|' )
        number += fread_number( fp );
    else if ( c != ' ' )
        ungetc( c, fp );

    return number;
}

/*
 * Read a number from a file.
 */
long fread_long_number( FILE *fp )
{
    long number;
    bool sign;
    char c;

    do
    {
        c = getc( fp );
    }
    while ( isspace(c) );

    number = 0;

    sign   = FALSE;
    if ( c == '+' )
    {
        c = getc( fp );
    }
    else if ( c == '-' )
    {
        sign = TRUE;
        c = getc( fp );
    }

    if ( !isdigit(c) )
    {
        bug( "Fread_number: bad format.", 0 );
        exit( 1 );
    }

    while ( isdigit(c) )
    {
        number = number * 10 + c - '0';
        c      = getc( fp );
    }

    if ( sign )
        number = 0 - number;

    if ( c == '|' )
        number += fread_long_number( fp );
    else if ( c != ' ' )
        ungetc( c, fp );

    return number;
}

long fread_flag( FILE *fp)
{
    int number;
    char c;
    bool negative = FALSE;

    do
    {
        c = getc(fp);
    }
    while ( isspace(c));

    if (c == '-')
    {
        negative = TRUE;
        c = getc(fp);
    }

    number = 0;

    if (!isdigit(c))
    {
        while (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z'))
        {
            number += flag_convert(c);
            c = getc(fp);
        }
    }

    while (isdigit(c))
    {
        number = number * 10 + c - '0';
        c = getc(fp);
    }

    if (c == '|')
        number += fread_flag(fp);

    else if  ( c != ' ')
        ungetc(c,fp);

    if (negative)
        return -1 * number;

    return number;
}

long flag_convert(char letter )
{
    long bitsum = 0;
    char i;

    if ('A' <= letter && letter <= 'Z')
    {
        bitsum = 1;
        for (i = letter; i > 'A'; i--)
            bitsum *= 2;
    }
    else if ('a' <= letter && letter <= 'z')
    {
        bitsum = 67108864; /* 2^26 */
        for (i = letter; i > 'a'; i --)
            bitsum *= 2;
    }

    return bitsum;
}

void fread_ext_flag( FILE *fp, int *bank, long *vector )
{
    int number;
    long value;
    char c;

    *bank = 0;
    *vector = 0;

    do
    {
        c = getc(fp);
    }
    while ( isspace(c));

    number = 0;

    while ( isdigit( c ) )
    {
        number = number * 10 + c - '0';
        c = getc(fp);
    }

    *bank = number;

    value = 0;

    if ( c == '|' )
        value = fread_flag( fp );
    else
        ungetc( c, fp );

    *vector = value;

    return;
}

void fread_ext_flags( FILE *fp, long *flags )
{
    char c;
    int bank;
    long vector;

    ext_flags_clear( flags );

    do
    {
        fread_ext_flag( fp, &bank, &vector );

        if ( vector > 0 )
        {
            SET_BIT( *(flags+bank), vector );
        }

        c = getc(fp);
    } while ( c == '/' );

    if  ( c != ' ' )
        ungetc( c, fp );

    return;
}

/*
 * Read to end of line (for comments).
 */
void fread_to_eol( FILE *fp )
{
    char c;

    do
    {
        c = getc( fp );
    }
    while ( c != '\n' && c != '\r' );

    do
    {
        c = getc( fp );
    }
    while ( c == '\n' || c == '\r' );

    ungetc( c, fp );
    return;
}



/*
 * Read one word (into static buffer).
 */
char *fread_word( FILE *fp )
{
    static char word[MAX_INPUT_LENGTH];
    char *pword;
    char cEnd;

    do
    {
        cEnd = getc( fp );
    }
    while ( isspace( cEnd ) );

    if ( cEnd == '\'' || cEnd == '"' )
    {
        pword   = word;
    }
    else
    {
        word[0] = cEnd;
        pword   = word+1;
        cEnd    = ' ';
    }

    for ( ; pword < word + MAX_INPUT_LENGTH; pword++ )
    {
        *pword = getc( fp );
        if ( cEnd == ' ' ? isspace(*pword) : *pword == cEnd )
        {
            if ( cEnd == ' ' )
                ungetc( *pword, fp );
            *pword = '\0';
            return word;
        }
    }
    bug( "Fread_word: word too long.", 0 );
    exit( 1 );
    return NULL;
}

/*char *fread_string_eol( FILE * fp )
  {
  char buf[MAX_STRING_LENGTH * 4];
  char *ptr = buf;
  char c;

  do
  c = getc( fp );
  while ( c == ' ' || c == '\t' );

 *ptr = c;

 while ( *ptr != '\n' && *ptr != '\r' && *ptr != EOF )
 *++ptr = getc( fp );

 if ( ptr == buf )
 return &str_empty[0];

 *ptr = '\0';

 return STRALLOC( buf );;
 }*/

char *fread_string_eol( FILE * fp )
{
    char buf[MAX_STRING_LENGTH * 4];
    char *ptr = buf;
    char c;

    /*
     * Skip blanks.
     * Read first char.
     */
    do
        c = getc( fp );
    while ( c == ' ' || c == '\t' );

    *ptr = c;

    while ( *ptr != '\n' && *ptr != '\r' && *ptr != EOF )
        *++ptr = getc( fp );

    if ( ptr == buf )
        return &str_empty[0];

    *ptr = '\0';

    return str_dup( buf );;
}

/*char *fread_string( FILE *fp )
  {
  char buf[MAX_STRING_LENGTH];
  char *plast;
  char c;
  int ln;

  plast = buf;
  buf[0] = '\0';
  ln = 0;

  do
  {
  if ( feof(fp) )
  {
  bug( "fread_string: EOF encountered on read.", 0 );
  if ( fBootDb )
  {
  log_string( "Corrupt file somewhere." );
  exit(1);
  }
  return STRALLOC("");
  }
  c = getc( fp );
  }
  while ( isspace(c) );

  if ( ( *plast++ = c ) == '~' )
  return STRALLOC( "" );

  for ( ;; )
  {
  if ( ln >= (MAX_STRING_LENGTH - 1) )
  {
  bug( "fread_string: string too long", 0 );
 *plast = '\0';
 return STRALLOC( buf );
 }
 switch ( *plast = getc( fp ) )
 {
 default:
 plast++; ln++;
 break;

 case EOF:
 bug( "Fread_string: EOF", 0 );
 if ( fBootDb )
 exit( 1 );
 *plast = '\0';
 return STRALLOC(buf);

 case '\n':
 plast++;  ln++;
 *plast++ = '\r';  ln++;
 break;

 case '\r':
 break;

 case '~':
 *plast = '\0';
 return STRALLOC( buf );
 }
 }
 }*/

/*
 * Read a string from file fp using str_dup (ie: no string hashing)
 */
char *fread_string( FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *plast;
    char c;
    int ln;

    plast = buf;
    buf[0] = '\0';
    ln = 0;

    /*
     * Skip blanks.
     * Read first char.
     */
    do
    {
        if ( feof(fp) )
        {
            bug( "fread_string_no_hash: EOF encountered on read.", 0 );
            if ( fBootDb )
            {
                log_string( "Corrupt file somewhere." );
                exit(1);
            }
            return str_dup("");
        }
        c = getc( fp );
    }
    while ( isspace(c) );

    if ( ( *plast++ = c ) == '~' )
        return str_dup( "" );

    for ( ;; )
    {
        if ( ln >= (MAX_STRING_LENGTH - 1) )
        {
            bug( "fread_string_no_hash: string too long", 0 );
            *plast = '\0';
            return str_dup( buf );
        }
        switch ( *plast = getc( fp ) )
        {
            default:
                plast++; ln++;
                break;

            case EOF:
                bug( "Fread_string_no_hash: EOF", 0 );
                if ( fBootDb )
                    exit( 1 );
                *plast = '\0';
                return str_dup(buf);

            case '\n':
                plast++;  ln++;
                *plast++ = '\r';  ln++;
                break;

            case '\r':
                break;

            case '~':
                *plast = '\0';
                return str_dup( buf );
        }
    }
}

char *str_dup( char const *str )
{
    static char *ret;
    int len;

    if ( !str )
    {
        bug("str_dup: str == NULL", 1 );
        return NULL;
    }

    len = strlen(str)+1;

    CREATE( ret, char, len );
    strcpy( ret, str );
    return ret;
}

void do_memory( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    sprintf( buf, "Affects %6d\n\r", top_affect    ); send_to_char( buf, ch );
    sprintf( buf, "Areas   %6d\n\r", top_area      ); send_to_char( buf, ch );
    sprintf( buf, "ExDes   %6d\n\r", top_ed        ); send_to_char( buf, ch );
    sprintf( buf, "Exits   %6d\n\r", top_exit      ); send_to_char( buf, ch );
    sprintf( buf, "Helps   %6d\n\r", top_help      ); send_to_char( buf, ch );
    sprintf( buf, "Mobs    %6d\n\r", top_mob_index ); send_to_char( buf, ch );
    // Gurthg: 2004-09-09 wy³±czam te dane, poniewa¿ s± niepotrzebne
    //    sprintf( buf, "Mobs    %6d(%d new format)\n\r", top_mob_index,newmobs ); send_to_char( buf, ch );
    //    sprintf( buf, "(in use)%6d\n\r", mobile_count  ); send_to_char( buf, ch );
    //    sprintf( buf, "Objs    %6d(%d new format)\n\r", top_obj_index,newobjs ); send_to_char( buf, ch );
    sprintf( buf, "Objs    %6d\n\r", top_obj_index ); send_to_char( buf, ch );
    sprintf( buf, "Resets  %6d\n\r", top_reset     ); send_to_char( buf, ch );
    sprintf( buf, "Rooms   %6d\n\r", top_room      ); send_to_char( buf, ch );
    sprintf( buf, "Shops   %6d\n\r", top_shop      ); send_to_char( buf, ch );
    sprintf( buf, "Banks   %6d\n\r", top_bank      ); send_to_char( buf, ch );

    return;
}

void do_dump( CHAR_DATA *ch, char *argument )
{
#if 0
    int count,count2,num_pcs,aff_count, memsp_count;
    CHAR_DATA *fch;
    MOB_INDEX_DATA *pMobIndex;
    PC_DATA *pc;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObjIndex;
    ROOM_INDEX_DATA *room;
    EXIT_DATA *exit;
    DESCRIPTOR_DATA *d;
    AFFECT_DATA *af;
    FILE *fp;
    MSPELL_DATA *mspell;
    ush_int vnum,nMatch = 0;

    /* open file */
    fclose(fpReserve);
    fp = fopen("../dmp/mem.dmp","w");

    /* report use of data structures */

    num_pcs = 0;
    aff_count = 0;
    memsp_count = 0;

    /* mobile prototypes */
    fprintf(fp,"MobProt	%4d (%8d bytes)\n",
            top_mob_index, top_mob_index * (sizeof(*pMobIndex)));

    /* mobs */
    count = 0;  count2 = 0;
    for (fch = char_list; fch != NULL; fch = fch->next)
    {
        count++;
        if (fch->pcdata != NULL)
        {
            num_pcs++;
        }
        for (af = fch->affected; af != NULL; af = af->next)
        {
            aff_count++;
        }
        if(fch->pcdata!=NULL)
        {
            for (mspell = fch->memspell;mspell!=NULL;mspell=mspell->next)
            {
                memsp_count++;
            }
        }
    }
    for (fch = char_free; fch != NULL; fch = fch->next)
    {
        count2++;
    }

    fprintf(fp,"Mobs	%4d (%8d bytes), %2d free (%d bytes)\n",
            count, count * (sizeof(*fch)), count2, count2 * (sizeof(*fch)));

    /* pcdata */
    count = 0;
    for (pc = pcdata_free; pc != NULL; pc = pc->next)
        count++;

    fprintf(fp,"Pcdata	%4d (%8d bytes), %2d free (%d bytes)\n",
            num_pcs, num_pcs * (sizeof(*pc)), count, count * (sizeof(*pc)));

    /* descriptors */
    count = 0; count2 = 0;
    for (d = descriptor_list; d != NULL; d = d->next)
        count++;
    for (d= descriptor_free; d != NULL; d = d->next)
        count2++;

    fprintf(fp, "Descs	%4d (%8d bytes), %2d free (%d bytes)\n",
            count, count * (sizeof(*d)), count2, count2 * (sizeof(*d)));

    /* object prototypes */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
        if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
        {
            for (af = pObjIndex->affected; af != NULL; af = af->next)
                aff_count++;
            nMatch++;
        }

    fprintf(fp,"ObjProt	%4d (%8d bytes)\n",
            top_obj_index, top_obj_index * (sizeof(*pObjIndex)));


    /* objects */
    count = 0;  count2 = 0;
    for (obj = object_list; obj != NULL; obj = obj->next)
    {
        count++;
        for (af = obj->affected; af != NULL; af = af->next)
            aff_count++;
    }
    for (obj = obj_free; obj != NULL; obj = obj->next)
        count2++;

    fprintf(fp,"Objs	%4d (%8d bytes), %2d free (%d bytes)\n",
            count, count * (sizeof(*obj)), count2, count2 * (sizeof(*obj)));

    /* affects */
    count = 0;
    for (af = affect_free; af != NULL; af = af->next)
        count++;

    fprintf(fp,"Affects	%4d (%8d bytes), %2d free (%d bytes)\n",
            aff_count, aff_count * (sizeof(*af)), count, count * (sizeof(*af)));

    count = 0;
    for (mspell = mspell_free; mspell != NULL; mspell = mspell->next)
        count++;

    fprintf(fp,"Mspells %4d (%8d bytes), %2d free (%d bytes)\n",
            memsp_count, memsp_count * (sizeof(*mspell)), count, count * (sizeof(*mspell)));


    /* rooms */
    fprintf(fp,"Rooms	%4d (%8d bytes)\n",
            top_room, top_room * (sizeof(*room)));

    /* exits */
    fprintf(fp,"Exits	%4d (%8d bytes)\n",
            top_exit, top_exit * (sizeof(*exit)));

    fclose(fp);

    /* start printing out mobile data */
    fp = fopen("../dmp/mob.dmp","w");

    fprintf(fp,"\nMobile Analysis\n");
    fprintf(fp,  "---------------\n");
    nMatch = 0;
    for (vnum = 0; nMatch < top_mob_index; vnum++)
        if ((pMobIndex = get_mob_index(vnum)) != NULL)
        {
            nMatch++;
            fprintf(fp,"#%-4d %3d active %3d killed     %s\n",
                    pMobIndex->vnum,pMobIndex->count,
                    pMobIndex->killed,pMobIndex->short_descr);
        }
    fclose(fp);

    /* start printing out object data */
    fp = fopen("../dmp/obj.dmp","w");

    fprintf(fp,"\nObject Analysis\n");
    fprintf(fp,  "---------------\n");
    nMatch = 0;
    for (vnum = 0; nMatch < top_obj_index; vnum++)
        if ((pObjIndex = get_obj_index(vnum)) != NULL)
        {
            nMatch++;
            fprintf(fp,"#%-4d %3d active %3d reset      %s\n",
                    pObjIndex->vnum,pObjIndex->count,
                    pObjIndex->reset_num,pObjIndex->short_descr);
        }

    /* close file */
    fclose(fp);
    fpReserve = fopen( NULL_FILE, "r" );
#endif // #if 0
}



/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy( int number )
{
    switch ( number_bits( 2 ) )
    {
        case 0:  number -= 1; break;
        case 3:  number += 1; break;
    }

    return UMAX( 1, number );
}

//losuje liczbe nawet z ujemnych???
int number_range_m( int from, int to )
{
    int power;
    int number;
    int minus;
    int dev;
    //    if (from == 0 && to == 0)
    //	return 0;

    //    if ( ( to = to - from + 1 ) <= 1 )
    //	return from;

    if (to < from) //jesli to mniejsze od from to zamieniamy kolejnosc
    {
        dev = from;
        from = to;
        to = dev;
    }

    minus = 0;
    if (from < 0) // jesli from mniejsze od zera do zapamietujemy o ile i dodajemy ta wartosc do to i do from
    {
        minus = from * (-1);
        from += minus;
        to += minus;
    }

    if (from == 0 && to == 0)
        return 0;

    if ( ( to = to - from + 1 ) <= 1 )
        return from - minus;

    for ( power = 2; power < to; power <<= 1 )
        ;

    while ( ( number = number_mm() & (power -1 ) ) >= to )
        ;

    return from + number - minus; //odejmujemy t± warto¶æ o któr± from by³o mniejsze od zera
}

/*
 * Generate a random number.
 */
int number_range( int from, int to )
{
    int power;
    int number;

    if (from == 0 && to == 0)
        return 0;

    if ( ( to = to - from + 1 ) <= 1 )
        return from;

    for ( power = 2; power < to; power <<= 1 )
        ;

    while ( ( number = number_mm() & (power -1 ) ) >= to )
        ;

    return from + number;
}

/*
 * Generate a percentile roll.
 */
int number_percent( void )
{
    int percent;

    while ( (percent = number_mm() & (128-1) ) > 99 )
        ;

    return 1 + percent;
}



/*
 * Generate a random door.
 */
int number_door( void )
{
    int door;

    while ( ( door = number_mm() & (8-1) ) > 5)
        ;

    return door;
}

int number_bits( int width )
{
    return number_mm( ) & ( ( 1 << width ) - 1 );
}

/* Tener:
 * URANGE - return n if low < n < high. If not, return low if n < low, else high.
 */

inline long int URANGE( long int low, long int n, long int high )
{
    return n < low ? low : ( n > high ? high : n );
}



/* Tener:
 * UMIN - minimum of a and b
 */

inline long int UMIN( long int a, long int b )
{
    return a < b ? a : b;
}

/* Tener:
 * UMAX - maximum of a and b
 */

inline long int UMAX( long int a, long int b )
{
    return b < a ? a : b;
}


/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */

/* I noticed streaking with this random number generator, so I switched
   back to the system srandom call.  If this doesn't work for you,
   define OLD_RAND to use the old system -- Alander */

/* Tener:
   Zmieni³em definicje makr OLD_RAND i NEW_RAND - patrz db.h
 */


#if RANDOM_GEN == 0
static  int     rgiState[2+55];
#endif

void init_mm( )
{

#if RANDOM_GEN == 2
    init_random(time(NULL), getpid());
    return;
#endif

#if RANDOM_GEN == 0
    int *piState;
    int iState;

    piState     = &rgiState[2];

    piState[-2] = 55 - 55;
    piState[-1] = 55 - 24;

    piState[0]  = ((int) current_time) & ((1 << 30) - 1);
    piState[1]  = 1;
    for ( iState = 2; iState < 55; iState++ )
    {
        piState[iState] = (piState[iState-1] + piState[iState-2])
            & ((1 << 30) - 1);
    }
#endif

#if RANDOM_GEN == 1
    srandom(time(NULL)^getpid());
#endif

    return;
}



long number_mm( void )
{
#if RANDOM_GEN == 2
    return genrand_int32();
#endif

#if RANDOM_GEN == 0
    int *piState;
    int iState1;
    int iState2;
    int iRand;

    piState             = &rgiState[2];
    iState1             = piState[-2];
    iState2             = piState[-1];
    iRand               = (piState[iState1] + piState[iState2])
        & ((1 << 30) - 1);
    piState[iState1]    = iRand;
    if ( ++iState1 == 55 )
        iState1 = 0;
    if ( ++iState2 == 55 )
        iState2 = 0;
    piState[-2]         = iState1;
    piState[-1]         = iState2;
    return iRand >> 6;
#endif

#if RANDOM_GEN == 1
    return random() >> 6;
#endif
}


/*
 * Roll some dice.
 */
int dice( int number, int size )
{
    int idice;
    int sum;

    switch ( size )
    {
        case 0: return 0;
        case 1: return number;
    }

    for ( idice = 0, sum = 0; idice < number; idice++ )
        sum += number_range( 1, size );

    return sum;
}



/*
 * Simple linear interpolation.
 */
int interpolate( int level, int value_00, int value_32 )
{
    return value_00 + level * (value_32 - value_00) / 32;
}



/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde( char *str )
{
    for ( ; *str != '\0'; str++ )
    {
        if ( *str == '~' )
            *str = '-';
    }

    return;
}

//Brohacz: sprawdza, czy argument zawiera wylacznie litery
bool check_all_alpha( char *str )
{
    for ( ; *str != '\0'; str++ )
    {
        if ( !isalpha( *str ) && *str != '±' && *str != 'ê' && *str != 'ó' && *str != '¼' && *str != '¿' && *str != 'æ' && *str != '¶' && *str != 'ñ' && *str != '³' )
            return FALSE;
    }

    return TRUE;
}

/*
   Tener [23-12-2008]: wy³±czamy na czas bootowania debugowanie str_cmp - BARDZO przyspiesza reboot
 */

void set_str_cmp_debug( bool enabled )
{
    char buf[MSL];
    str_cmp_debug = enabled;
    sprintf(buf, "str_cmp debugging is now %s.", enabled ? "ENABLED" : "DISABLED" );
    log_string( buf );
}

/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */

bool str_cmp( const char *astr, const char *bstr )
{
    //rellik, do debugowania, najpierw zapisujê astring i bstring, pó¼niej zapisuje do loga 2 ostatnie komend, na koñcu ustawiam save_debug_info [20080429]

    if (str_cmp_debug)
    {
        char tmp[MSL];
        sprintf( tmp, "astr = %s, bstr = %s", astr? astr: "NULL", bstr? bstr: "NULL");

        save_debug_info( "db.c => str_cmp", NULL, tmp, DEBUG_PROJECT_UNKNOWN, DEBUG_LEVEL_ALL, TRUE);
    }


    if ( astr == NULL )
    {
        bug( "Str_cmp: null astr.", 0 );
        return TRUE;
    }

    if ( bstr == NULL )
    {
        bug( "Str_cmp: null bstr.", 0 );
        return TRUE;
    }

    for ( ; *astr || *bstr; astr++, bstr++ )
    {
        //if ( LOWER(*astr) != LOWER(*bstr) )
        if ( NOPOL(*astr) != NOPOL(*bstr) )
            return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix( const char *astr, const char *bstr )
{
    if ( astr == NULL || astr[0]=='\0')
        return TRUE;

    if ( bstr == NULL || bstr[0]=='\0')
        return TRUE;

    for ( ; *astr; astr++, bstr++ )
    {
        //if ( LOWER(*astr) != LOWER(*bstr) )
        if ( NOPOL(*astr) != NOPOL(*bstr) )
            return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;
    int ichar;
    char c0;

    if ( ( c0 = NOPOL(astr[0]) ) == '\0' )
        return FALSE;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);

    for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
    {
        if ( c0 == NOPOL(bstr[ichar]) && !str_prefix( astr, bstr + ichar ) )
            return FALSE;
    }

    return TRUE;
}



/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);
    if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) )
        return FALSE;
    else
        return TRUE;
}

char *uncapitalize( const char *str )
{
    static char strcap[MAX_STRING_LENGTH];
    int i;

    for ( i = 0; str[i] != '\0'; i++ )
        strcap[i] = LOWER(str[i]);
    strcap[i] = '\0';
    strcap[0] = LOWER(strcap[0]);
    return strcap;
}

/*
 * Append a string to a file.
 */
void append_file( CHAR_DATA *ch, char *file, char *str )
{
    FILE *fp;

    if ( str[0] == '\0' )
        return;

    if ( ( fp = fopen( file, "a" ) ) == NULL )
    {
        perror( file );
        bugf( "append_file: nie mo¿na otworzyæ pliku %s (errno %d)", file, errno );
    }
    else
    {
        char *strtime;
        strtime = ctime( &current_time );
        strtime[strlen(strtime)-1] = '\0';
        if ( ch )
            fprintf( fp, "%s : [%5d] %s: %s\n", strtime, ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
        else
            fprintf( fp, "%s : %s\n", strtime, str );
        fclose( fp );
    }

    return;
}

/*
 * Append formated string to a file.
 */
void append_file_format( CHAR_DATA *ch, char *file, char *fmt, ... )
{
    char buf [2*MSL];
    va_list args;
    va_start (args, fmt);
    vsprintf (buf, fmt, args);
    va_end (args);

    append_file ( ch, file, buf );
}


void append_file_format_daily( CHAR_DATA *ch, char *file, char *fmt, ... )
{
    char buf [2*MSL];
    char format_file[MAX_STRING_LENGTH];
    char date[50];
    struct tm *broken_time;

    va_list args;
    va_start (args, fmt);
    vsprintf (buf, fmt, args);
    va_end (args);

    broken_time = localtime( &current_time );
    strftime( date, 50, "%Y-%m-%d", broken_time );
    sprintf( format_file, file, date );
    append_file ( ch, format_file, buf );
}

/*
 * Reports a bug.
 */
void bug( const char *str, int param )
{
    char buf[MAX_STRING_LENGTH];

    if ( fpArea != NULL )
    {
        int iLine;
        int iChar;

        if ( fpArea == stdin )
        {
            iLine = 0;
        }
        else
        {
            iChar = ftell( fpArea );
            fseek( fpArea, 0, 0 );
            for ( iLine = 0; ftell( fpArea ) < iChar; iLine++ )
            {
                while ( getc( fpArea ) != '\n' )
                    ;
            }
            fseek( fpArea, iChar, 0 );
        }

        sprintf( buf, "[*****] FILE: %s LINE: %d", strArea, iLine );
        log_string( buf );
    }

    strcpy( buf, "[*****] BUG: " );
    sprintf( buf + strlen(buf), str, param );
    log_string( buf );

    return;
}

/*
 * Writes a string to the log.
 */
void log_string( const char *str )
{
    char *strtime;

    strtime                    = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';
    fprintf( stderr, "%s :: %s\n", strtime, str );
    return;
}

void load_learn_system( void )
{
    FILE * fp;
    LEARN_DATA *ld, *last = NULL, *tmp1;
    LEARN_LIST *ls, *tmpls;
    char tmp;
    int sn;

    if ( ( fp = fopen( LEARN_FILE, "r" ) ) == NULL )
        return ;

    for ( ; ; )
    {
        if ( feof( fp ) )
        {
            fclose( fp );
            return ;
        }

        tmp = fread_letter( fp );

        if ( tmp == 'm' || tmp == 'M' )
        {
            ld          = new_learn_data();
            ld->vnum    = fread_number( fp );
            ld->next    = NULL;
            last        = ld;

            if ( learn_system == NULL )
                learn_system = ld;
            else
            {
                for ( tmp1 = learn_system; tmp1->next; tmp1 = tmp1->next );
                tmp1->next = ld;
            }

            fread_to_eol( fp );
        }
        else if ( tmp == 's' || tmp == 'S' || tmp == 'p' || tmp == 'P' )
        {
            if ( last == NULL )
            {
                bug( "Load_learn: skill bez moba", 0 );
                fread_to_eol( fp );
            }
            else
            {
                ls          = new_learn_list();
                ls->min     = fread_number( fp );
                ls->max     = fread_number( fp );
                ls->chance  = fread_number( fp );
                if ( tmp == 'p' || tmp == 'P' )
                {
                    ls->payment     = fread_number( fp );
                    ls->pay_from    = fread_number( fp );
                }
                if ( ( sn = skill_lookup( fread_string( fp ) ) ) == -1 )
                {
                    free_learn_list( ls );
                    continue;
                }
                ls->sn   = sn;
                ls->next = NULL;

                if ( last->list == NULL )
                    last->list = ls;
                else
                {
                    for ( tmpls = last->list; tmpls->next; tmpls = tmpls->next );
                    tmpls->next = ls;
                }
                fread_to_eol( fp );
            }
        }
        else
        {
            bug( "Zly wpis w learn.txt", 0 );
            return ;
        }
    }
}

void load_learn_trick_system( void )
{
    FILE * fp;
    LEARN_TRICK_DATA *ld, *last = NULL, *tmp1;
    LEARN_TRICK_LIST *ls, *tmpls;
    char *buf;
    char tmp;
    int sn;

    if ( ( fp = fopen( LEARN_TRICK_FILE, "r" ) ) == NULL )
        return ;

    for ( ; ; )
    {
        if ( feof( fp ) )
        {
            fclose( fp );
            return ;
        }

        tmp = fread_letter( fp );

        if ( tmp == 'm' || tmp == 'M' )
        {
            ld          = new_learn_trick_data();
            ld->vnum    = fread_number( fp );
            ld->next    = NULL;
            last        = ld;

            if ( learn_trick_system == NULL )
                learn_trick_system = ld;
            else
            {
                for ( tmp1 = learn_trick_system; tmp1->next; tmp1 = tmp1->next );
                tmp1->next = ld;
            }

            fread_to_eol( fp );
        }
        else if ( tmp == 't' || tmp == 'T' )
        {
            if ( last == NULL )
            {
                bug( "Load_learn_trick: skill bez moba", 0 );
                fread_to_eol( fp );
            }
            else
            {
                ls          = new_learn_trick_list();
                ls->chance  = fread_number( fp );
                ls->payment  = fread_number( fp );

                buf = fread_string( fp );

                for ( sn = 0; sn < MAX_TRICKS; sn++ )
                {
                    if ( trick_table[ sn ].name == NULL )
                    {
                        free_learn_trick_list( ls );
                        continue;
                    }

                    if( !str_cmp( buf, trick_table[ sn ].name ) )
                        break;
                }

                ls->sn   = sn;
                ls->next = NULL;

                if ( last->list == NULL )
                    last->list = ls;
                else
                {
                    for ( tmpls = last->list; tmpls->next; tmpls = tmpls->next );
                    tmpls->next = ls;
                }
                fread_to_eol( fp );
            }
        }
        else
        {
            bug( "Zly wpis w learn_trick.txt", 0 );
            return ;
        }
    }
}

/*artefact*/
void load_artefact_list( void )
{
    FILE * fp;
    ARTEFACT_DATA *ad, *last = NULL, *tmp1;
    ARTEFACT_OWNER *ow, *olast = NULL;
    ARTEFACT_LOADER *load, *llast = NULL;
    char x;
    if ( ( fp = fopen( ARTEFACT_FILE, "r" ) ) == NULL )
    {
        bug( "error read ARTEFACT_FILE", 0 );
        return ;
    }
    for ( ; ; )
    {
        if ( feof( fp ) )
        {
            fclose( fp );
            return ;
        }
        x = fread_letter( fp );
        switch ( x )
        {
            case 'a':
            case 'A':
                ad              = new_artefact_data();
                ad->avnum       = fread_number( fp );
                ad->count       = fread_number( fp );
                ad->max_count   = fread_number( fp );
                ad->max_day	    = fread_number( fp );
                //ad->max_day = 20;//MAX_ARTEFACT_DAY
                //
                bugf( "read artefact: A %d %d %d %d", ad->avnum, ad->count, ad->max_count, ad->max_day );

                ad->next        = NULL;
                ad->first_owner = NULL;
                last            = ad;
                olast           = NULL;
                llast           = NULL;
                if ( artefact_system == NULL )
                {
                    artefact_system = ad;
                }
                else
                {
                    for ( tmp1 = artefact_system;tmp1->next;tmp1 = tmp1->next );
                    tmp1->next = ad;
                }
                break;
            case 'o':
            case 'O':
                ow          = new_artefact_owner();
                ow->owner   = fread_string( fp );
                ow->status  = fread_number( fp );
                ow->next    = NULL;
                bugf( "read artefact: O %s %d", ow->owner, ow->status );

                if ( olast == NULL ) /*ostatnio czytany artefact*/
                {
                    last->first_owner = ow;
                }
                else /*ostatnio czytany owner*/
                {
                    olast->next = ow;
                }

                olast = ow;
                break;
            case 'l':
            case 'L':
                load            = new_artefact_loader();
                load->type      = fread_number( fp );
                load->vnum      = fread_number( fp );
                load->probdown  = fread_number( fp );
                load->probup    = fread_number( fp );
                load->next      = NULL;

                bugf( "read artefact: L %d %d %d %d", load->type, load->vnum, load->probdown, load->probup  );

                if ( llast == NULL ) /*ostatnio czytany artefact*/
                {
                    last->loader = load;
                }
                else /*ostatnio czytany loader*/
                {
                    llast->next = load;
                }
                llast = load;
                break;
        }
        fread_to_eol( fp );
    }
}

void load_prewait_system()
{
    FILE * fp;
    PRE_WAIT *tmp = NULL, *last = NULL, *tmp1 = NULL;
    PFLAG_DATA *list = NULL, *ls = NULL;
    char tmpchar;
    int sn;
    char buf[MAX_STRING_LENGTH];

    if ( ( fp = fopen( PREWAIT_FILE, "r" ) ) == NULL ) return ;

    for ( ; ; )
    {
        if ( feof( fp ) )
        {
            fclose( fp );
            return ;
        }

        tmpchar = fread_letter( fp );

        if ( tmpchar == 's' || tmpchar == 'S' )
        {
            tmp                 = new_prewait();
            tmp->duration       = fread_number( fp );
            tmp->spell_name     = fread_string( fp );
            if ( ( sn = skill_lookup( tmp->spell_name ) ) == -1 )
            {
                sprintf( buf, "[prewait] Bledna nazwa spella: %s.", tmp->spell_name );
                bug( buf, 0 );
                return ;
            }
            tmp->sn = sn;
            tmp->next = NULL;
            last = tmp;

            if ( pre_waits == NULL )
                pre_waits = tmp;
            else
            {
                for ( tmp1 = pre_waits;tmp1->next;tmp1 = tmp1->next );
                tmp1->next = tmp;
            }

            fread_to_eol( fp );
        }

        else if ( tmpchar == 'k' || tmpchar == 'K' )
        {
            if ( last == NULL )
            {
                bug( "Load_prewait: komentarz bez spella", 0 );
                fread_to_eol( fp );
            }
            else
            {
                list            = new_pflag();
                list->target    = fread_number( fp );
                list->duration  = fread_number( fp );
                list->id        = fread_string( fp );
                list->next      = NULL;

                if ( last->list == NULL )
                    last->list = list;
                else
                {
                    for ( ls = last->list;ls->next;ls = ls->next );
                    ls->next = list;
                }
                fread_to_eol( fp );
            }
        }
        else
        {
            bug( "Zly wpis w prewait.dat", 0 );
            return ;
        }
    }
}


void load_boards( void )
{
    BOARD_DATA * brd = NULL;

    if ( board_system == NULL ) return ;
    for ( brd = board_system;brd;brd = brd->next )
        load_board( brd );
    return ;
}



void load_spell_msg()
{
    FILE * fp;
    char *name;
    char tmpchar;
    sh_int sn, i;
    SPELL_MSG *tmp_el = NULL;
    char buf[MAX_STRING_LENGTH];

    if ( ( fp = fopen( SPELLMSG_FILE, "r" ) ) == NULL )
        return ;

    for ( ; ; )
    {
        if ( feof( fp ) )
        {
            fclose( fp );
            return ;
        }

        tmpchar = fread_letter( fp );

        if ( tmpchar == '#' )
            fread_to_eol( fp );
        else if ( tmpchar == 'M' || tmpchar == 'm' )
        {
            name = fread_string( fp );

            if ( ( sn = skill_lookup( name ) ) < 0 )
            {
                sprintf( buf, "[spell msg] Bledna nazwa spella: %s.", name );
                bug( buf, 0 );
                return ;
            }

            CREATE( tmp_el, SPELL_MSG, 1 );
            tmp_el->name = str_dup( name );
            tmp_el->sn = sn;

            for ( i = 0;i < 5;i++ )
                tmp_el->caster[ i ] = fread_string( fp );

            for ( i = 0;i < 5;i++ )
                tmp_el->victim[ i ] = fread_string( fp );

            for ( i = 0;i < 5;i++ )
                tmp_el->around[ i ] = fread_string( fp );

            tmp_el->next = spellmsg_list;
            spellmsg_list = tmp_el;
            fread_to_eol( fp );
        }
    }
}

/*
 * Cappes only first character.
 */
char *capitalize_first( const char *str )
{
    static char strcap[2][2*MAX_STRING_LENGTH];
    static int strcap_cnt = 0;
    int i;
    int first = 0;

    if ( ++strcap_cnt > 1 )
        strcap_cnt = 0;

    sprintf( strcap[strcap_cnt], "%s", str );

    if ( strcap[strcap_cnt][0] == '\0' )
        return strcap[strcap_cnt];

    if ( strcap[strcap_cnt][0] != '{' && strcap[strcap_cnt][0] != '^' )
        first = 0;
    else
        for ( i = 1; str[i] != '\0'; i++ )
            if ( strcap[strcap_cnt][i] != '{' && strcap[strcap_cnt][i] != '^'
                    && strcap[strcap_cnt][i-1] != '{' && strcap[strcap_cnt][i-1] != '^' )
            {
                first = i;
                break;
            }

    strcap[strcap_cnt][first] = UPPER( strcap[strcap_cnt][first] );

    return strcap[strcap_cnt];
}

void save_misc_data()
{
    PFLAG_DATA *global_flag;
    BOUNTY_DATA *bounty; //Brohacz: bounty
    FILE *fp;
    fclose(fpReserve);

    if ( ( fp = fopen( MISC_FILE, "w" ) ) == NULL )
    {
        bug( "Save_misc_data: fopen", 0 );
    }
    else
    {
        fprintf( fp, "CopyPeriod         %ld\n", misc.copyover_period );
        fprintf( fp, "WorldTime          %ld\n", misc.world_time );
        fprintf( fp, "LogAll             %d\n", fLogAll );
        fprintf( fp, "LogComm            %d\n", fLogComm );
        fprintf( fp, "NewLock            %d\n", newlock );
        fprintf( fp, "WizLock            %d\n", wizlock );

        for ( global_flag = misc.global_flag_list; global_flag; global_flag = global_flag->next )
        {
            if ( global_flag->duration != 0 || !str_prefix( "reg", global_flag->id ) )
                fprintf( fp, "GlobalFlag %s %d\n", global_flag->id, global_flag->duration );
        }

        //Brohacz: bounty: przeniesienie bounty z plikow postaci do globalnej kolejki
        for ( bounty = misc.bounty_list; bounty; bounty = bounty->next )
        {
            fprintf( fp, "Bounty %s %d\n", bounty->name, bounty->value );
        }

        fprintf( fp, "$\n" );
    }
    fclose(fp);
    fpReserve = fopen( NULL_FILE, "r" );

}

void load_misc_data()
{
    FILE *fp;

    misc.copyover_delayed = -1;
    misc.copyover_period = -1;
    misc.copyover_scheduled = -1;
    misc.copyover_save = FALSE;
    misc.world_time = 1;
    misc.global_flag_list = NULL;

    fclose(fpReserve);

    if ( ( fp = fopen( MISC_FILE, "r" ) ) != NULL )
    {
        for ( ; ; )
        {
            char *word;

            word = fread_word( fp );
            if      ( !str_cmp( word, "CopyPeriod" ) ) misc.copyover_period = fread_long_number ( fp );
            else if ( !str_cmp( word, "WorldTime" ) ) misc.world_time = fread_long_number ( fp );
            else if ( !str_cmp( word, "LogAll" ) ) fLogAll = fread_number ( fp );
            else if ( !str_cmp( word, "LogComm" ) ) fLogComm = fread_number ( fp );
            else if ( !str_cmp( word, "NewLock" ) ) newlock = fread_number ( fp );
            else if ( !str_cmp( word, "WizLock" ) ) wizlock = fread_number ( fp );
            else if ( !str_cmp( word, "GlobalFlag" ) )
            {
                int flag_value;
                char *flag_name;
                flag_name = fread_word( fp );
                flag_value = fread_number( fp );
                add_flag( &misc.global_flag_list, flag_name, flag_value );
            }
            else if ( !str_cmp( word, "Bounty" ) )
            {
                int bounty_value;
                char *bounty_name;
                bounty_name = fread_word( fp );
                bounty_value = fread_number( fp );
                add_bounty( &misc.bounty_list, bounty_name, bounty_value );
            }
            else
                break;
        }
        fclose(fp);
    }
    fpReserve = fopen( NULL_FILE, "r" );
}

/* snarf a socials file */
void load_socials( FILE *fp )
{
    SOCIAL_DATA * social;
    char *temp;
    int to = 0;

    for ( ; ; )
    {
        temp = fread_word( fp );
        if ( !strcmp( temp, "#0" ) )
            return ;  /* done */
#if defined(social_debug)
        else
            fprintf( stderr, "%s\n\r", temp );
#endif

        CREATE( social, SOCIAL_DATA, 1 );
        social->char_no_arg = NULL;
        social->others_no_arg = NULL;
        social->char_found = NULL;
        social->others_found = NULL;
        social->vict_found = NULL;
        social->char_not_found = NULL;
        social->char_auto = NULL;
        social->others_auto = NULL;

        social->name = str_dup( temp );
        fread_to_eol( fp );

        to = 0;
        while ( to < 8 )
        {
            temp = fread_string_eol( fp );
            if ( !str_cmp( temp, "$" ) )
            {
                free_string( temp );
                to++;
                continue;
            }
            else if ( !str_cmp( temp, "#" ) )
            {
                free_string( temp );
                break;
            }

            switch ( to )
            {
                case 0:
                    social->char_no_arg = temp;
                    break;
                case 1:
                    social->others_no_arg = temp;
                    break;
                case 2:
                    social->char_found = temp;
                    break;
                case 3:
                    social->others_found = temp;
                    break;
                case 4:
                    social->vict_found = temp;
                    break;
                case 5:
                    social->char_not_found = temp;
                    break;
                case 6:
                    social->char_auto = temp;
                    break;
                case 7:
                    social->others_auto = temp;
                    break;
            }
            to++;
        }

        social->next = social_list;
        social_list = social;
    }
    return ;
}

/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 */
void tail_chain( void )
{
    return;
}

//rellik: do debugowania, DEBUG_INFO przerobione na funkcjê
void DEBUG_INFO( const char string[])
{
    if ( ! previous_command )
    {
        previous_command = malloc( MAX_STRING_LENGTH );
        previous_command[0] = '\0';
    }

    if ( ! last_command )
    {
        last_command = malloc( MAX_STRING_LENGTH );
        last_command[0] = '\0';
    }


    memcpy( previous_command, last_command, MAX_STRING_LENGTH );
    sprintf(last_command, "%s", string ? string : "BRAK INFORMACJI" );

}

/*
   do porównywania na nale¿enie do tej samej klasy abstrakcji dwóch list obiektów.
Argumenty: g³owy obu list, funkcja porównuj±ca i funkcja zwracaj±ca nastêpny obiekt lub NULL je¿eli nie ma takowego

Funkcja nie alokuje pamiêci pomiêdzy jej wywo³aniami, chyba ¿e poprzednio zaalokowana pamiêæ nie wystarczy dla nowego wywo³ania.

Z³o¿ono¶æ pamiêciowa: Theta(n)
Z³o¿ono¶æ czasowa: Theta(n^2)

 */

bool are_lists_equal( void * obj1, void * obj2, bool obj_equal( void*, void* ), void * get_next( void * ) )
{
    int c1, c2; // d³ugo¶ci list
    void * objNow; // aktualnie ogl±dany obiekt
    static char *hasPair = NULL; // tablica par. FALSE na polu j-tym oznacza ¿e dany element nie jest sparowany z jakim¶ innym
    static int num_allocated = 0; // jak d³uga jest tablica hasPair
    bool stillOk = TRUE;


    if ( (obj1 == NULL) && (obj2 == NULL) )
        return TRUE;

    if ( (obj1 == NULL) || (obj2 == NULL) )
        return FALSE;

    /* najpierw liczymy sobie d³ugo¶æ obu list */

    c1 = 0;
    objNow = obj1;
    while( objNow )
    {
        c1++;
        objNow = get_next( objNow );
    }

    c2 = 0;
    objNow = obj2;
    while( objNow )
    {
        c2++;
        objNow = get_next( objNow );
    }

    /* je¿eli siê ró¿ni±, to zwracamy FALSE */

    if ( c1 != c2 )
        return FALSE;

    /* alokujemy tablicê do przechowywania informacji o "sparowanych" elementach */

    if ( hasPair == NULL )
    {
        hasPair = (char*) malloc( sizeof(char) * c1 );
        num_allocated = c1;
    }

    else if ( c1 > num_allocated )
    {
        hasPair = (char*) realloc( hasPair, sizeof(char) * c1 );
        num_allocated = c1;
    }

    memset( hasPair, '\0', sizeof(char) * c1 );

    /* szukamy dla ka¿dego elementu jego pary. Z³o¿ono¶æ: Theta(n^2) */
    objNow = obj1;

    while(objNow && stillOk)
    {
        void * maybePair = obj2;
        int j = 0;

        stillOk = FALSE;

        while(maybePair)
        {
            if ( !hasPair[ j ] && obj_equal( objNow, maybePair ))
            {
                hasPair[ j ] = TRUE;
                maybePair = NULL; // zerujemy maybePair, na znak ¿e dalej nie musimy szukaæ
                stillOk = TRUE;
            }
            else
            {
                maybePair = get_next( maybePair );
                j++;
            }
        }

        objNow = get_next( objNow );
    }

    return stillOk;
}


bool check_specdam_equal( void *a, void * b)
{
    SPEC_DAMAGE * asp = (SPEC_DAMAGE*) a;
    SPEC_DAMAGE * bsp = (SPEC_DAMAGE*) b;

    if ((asp->type == bsp->type)
            && (asp->chance == bsp->chance)
            && (asp->param1 == bsp->param1)
            && (asp->param2 == bsp->param2)
            && (asp->param3 == bsp->param3)
            && (asp->bonus == bsp->bonus)
            && (asp->target_type == bsp->target_type))
    {
        int i;
        for(i=0;i<MAX_VECT_BANK;i++)
        {
            if ( asp->ext_param[i] != bsp->ext_param[i] )
            {
                return FALSE;
            }
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

void * get_next_specdam( void * obj )
{
    SPEC_DAMAGE* sdam = (SPEC_DAMAGE*) obj;

    if ( obj == NULL )
    {
        bug("get_next_specdam: NULL obj", 0);
        return NULL;
    }

    return (void*) sdam->next;
}

SPEC_DAMAGE * copy_specdam( SPEC_DAMAGE* source )
{
    if ( source == NULL )
    {
        return NULL;
    }

    SPEC_DAMAGE * specdam = new_spec_damage();

    specdam->type = source->type;
    specdam->chance = source->chance;
    specdam->param1 = source->param1;
    specdam->param2 = source->param2;
    specdam->param3 = source->param3;
    specdam->bonus = source->bonus;
    specdam->target_type = source->target_type;;

    specdam->next = copy_specdam( source->next );

    return specdam;
}

