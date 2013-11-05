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
 * KILLER MUD is copyright 1999-2013 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Grunai                (grunai.mud@gmail.com          ) [Grunai    ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Koper Tadeusz         (jediloop@go2.pl               ) [Garloop   ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Skrzetnicki Krzysztof (gtener@gmail.com              ) [Tener     ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (tzdziech@gmail.com            ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: merc.h 12408 2013-06-12 12:29:02Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/merc.h $
 *
 */
#define NOCRYPT
/*#define SEARCH_SPELL_STANDARD*/

#define NORMAL_DEBUG_INFO

#ifdef NORMAL_DEBUG_INFO
/* zamieniam na zwyk³± funkcjê
 #define DEBUG_INFO( string )    last_command = ( string );
*/
#else
#define DEBUG_INFO( string )
#endif

/**
 * ukrywanie postaci na who
 */
//#define WHO_HIDE

/**
 * prztyczek do wylacza sie z who, uzywac tylko z wlaczonym WHO_HIDE
 */
#ifdef WHO_HIDE
#define ENABLE_SHOW_ON_WHO
#endif

//Rysand - jak siê odkomentuje, to bêdzie pstrzyczek "poka¿ wszystkim którzy mnie widz±, ¿e szukam grupy"
#define ENABLE_SHOW_LFG

//Rysand - mieszanie statów handler.c->get_hashed_stat, tables.c->stat_hash
//#define HASH_STAT

//Rysand - zmiejszanie statów po zabiciu pocz±tkuj±cego
#define NOOB_STAT_REMOVER

//Rysand - dopa³ka z dagger mastery i dagger do backstaba (mniejszy wait i szansa na zdjêcie zbroi)
#define ENHANCED_BACKSTAB

/**
 * maksymalny vnum: 2^17-1
 */
#define MAX_VNUM 131071
/**
 * maksymalna liczba exitów
 */
#define MAX_EXIT_COUNT 6

//Rysand - na razie nie w³±czaæ, wymaga sprawdzenia.
#define ENABLE_NEW_TRACK 1


//Rysand - po wprowadzeniu waita do backstaba z³odziej zosta³ upo¶ledzony.
//inne profki te¿ powinny mieæ ograniczenie na skille "atakuj±ce"
#define ENABLE_WAIT_AFTER_MISS

//rellik, jak zwykle problem gdzie umie¶ciæ deklaracjê funkcji by by³a widziana wszêdzie
//extern void debug_dump();

/* w³±czamy arynchroniczne dns'y na w±tkach */
#define ASYNCH_DNS

/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )         ( )
#define DECLARE_DO_FUN( fun )      void fun( )
#define DECLARE_DO_OFUN( fun )      void fun( )
#define DECLARE_SPEC_FUN( fun )      bool fun( )
#define DECLARE_SPELL_FUN( fun )   void fun( )
#define DECLARE_PROG_FUN( fun )      void fun( )
#define DECLARE_TRICK_FUN( fun )      void fun( )
#else
#define args( list )         list
#define DECLARE_DO_FUN( fun )      DO_FUN    fun
#define DECLARE_DO_OFUN( fun )      DO_OFUN   fun
#define DECLARE_SPEC_FUN( fun )      SPEC_FUN  fun
#define DECLARE_SPELL_FUN( fun )   SPELL_FUN fun
#define DECLARE_PROG_FUN( fun )      PROG_FUN  fun
#define DECLARE_TRICK_FUN( fun )      TRICK_FUN  fun
#endif

/* system calls */
int unlink();
int system();
/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if   !defined(FALSE)
#define FALSE    0
#endif

#if   !defined(TRUE)
#define TRUE    1
#endif

#if   defined(_AIX)
#if   !defined(const)
#define const
#endif
typedef int            sh_int;
typedef int            bool;
#define unix
#else
typedef short   int         sh_int;
typedef unsigned short int      ush_int;
typedef unsigned char         bool;
#endif

//rellik: do debugowania, zmienna u¿ywana do zapamiêtywania nazwy funkcji wywo³uj±cej tam gdzie nie mo¿na przekazaæ tego jako argument funkcji
char gcaller[200];

//rellik: bigflagi, tak wiem ze sa zaimplementowane flagi EXT ale robione na wska¼nikach, kto¶ kto nie zna ich dobrze bêdzie mia³ ciêzko zrozumieæ [20080516]
//rellik, u mnie na windowsie i na linuksie (32 bitowe wersje) int - 4 bajty, long - 4 bajty, short - 2 bajty, mamy wiêc 10*4*8 = 320 flag
//rellik, indeks tabeli mo¿emy swobodnie zwiêkszyæ i bêdzie wszystko dzia³aæ (powinno)
#define BIGFLAG_CAPACITY       10
struct bigflag
{
	unsigned int bank[BIGFLAG_CAPACITY];
};

typedef struct bigflag BIGFLAG;

/* ea */
#define MSL MAX_STRING_LENGTH
#define MIL MAX_INPUT_LENGTH

#ifdef MCCP
/* mccp: support bits */
#include <zlib.h>
#define TELOPT_COMPRESS 85
#define TELOPT_COMPRESS2 86
#define COMPRESS_BUF_SIZE 1024
#endif


/*Modyfikatory obra¿en*/

#define DAMAGE_MODIFIER_PLAYER_VS_MOB 3/2


/*------------*/

/*
 * Structure types.
 */
typedef struct charm_data              CHARM_DATA;
typedef struct bitvect_data            BITVECT_DATA;
typedef struct affect_data             AFFECT_DATA;
typedef struct mspell_data             MSPELL_DATA;
typedef struct memset_data             MEMSET_DATA;
typedef struct spell_failed            SPELL_FAILED;
typedef struct area_data               AREA_DATA;
typedef struct ban_data                BAN_DATA;
typedef struct buf_type                BUFFER;
typedef struct char_data               CHAR_DATA;
typedef struct descriptor_data         DESCRIPTOR_DATA;
typedef struct exit_data               EXIT_DATA;
typedef struct extra_descr_data        EXTRA_DESCR_DATA;
typedef struct help_data               HELP_DATA;
typedef struct help_area_data          HELP_AREA;
typedef struct kill_data               KILL_DATA;
typedef struct mem_data                MEM_DATA;
typedef struct mob_index_data          MOB_INDEX_DATA;
typedef struct note_data               NOTE_DATA;
typedef struct obj_data                OBJ_DATA;
typedef struct obj_index_data          OBJ_INDEX_DATA;
typedef struct pc_data                 PC_DATA;
typedef struct reset_data              RESET_DATA;
typedef struct track_data              TRACK_DATA;
typedef struct room_index_data         ROOM_INDEX_DATA;
typedef struct shop_data               SHOP_DATA;
typedef struct repair_data             REPAIR_DATA;
typedef struct bank_data               BANK_DATA;
typedef struct time_info_data          TIME_INFO_DATA;
typedef struct weather_data            WEATHER_DATA;
typedef struct colour_data             COLOUR_DATA;
typedef struct prog_list               PROG_LIST;
typedef struct prog_code               PROG_CODE;
typedef struct trap_data               TRAP_DATA;
typedef struct misc_data               MISC_DATA;
typedef struct rand_desc_data          RAND_DESC_DATA;
typedef struct desc_data               DESC_DATA;
typedef struct spec_damage             SPEC_DAMAGE;
typedef struct info_descriptor_data    INFO_DESCRIPTOR_DATA;
typedef struct account_data            ACCOUNT_DATA;
typedef struct event2_data             EVENT2_DATA;
typedef struct bonus_index_data        BONUS_INDEX_DATA;
typedef struct bonus_data              BONUS_DATA;
typedef struct song_data               SONG_DATA;
typedef struct perform_data            PERFORM_DATA;
typedef struct social_data             SOCIAL_DATA;
typedef struct flag_type               FLAG_TYPE;
typedef struct ext_flag_type           EXT_FLAG_TYPE;
//typedef struct herb_data               HERB_DATA;
typedef struct room_update_type        ROOM_UPDATE_TYPE;

/*artefact*/
typedef struct artefact_data ARTEFACT_DATA;
typedef struct artefact_owner ARTEFACT_OWNER;
typedef struct artefact_loader ARTEFACT_LOADER;

/*Brohacz: bounty*/
typedef struct bounty_data                 BOUNTY_DATA;

/*shapeshifting*/
typedef struct shapeshift_data SHAPESHIFT_DATA;

/*learn system*/
typedef struct   learn_data      LEARN_DATA;
typedef struct   learn_list      LEARN_LIST;
typedef struct   learn_trick_data      LEARN_TRICK_DATA;
typedef struct   learn_trick_list      LEARN_TRICK_LIST;

/*board system*/
typedef struct message_data MESSAGE_DATA;
typedef struct board_data   BOARD_DATA;
/*pflag system*/
typedef struct pflag_data   PFLAG_DATA;
/*prewait system*/
typedef struct pre_wait     PRE_WAIT;
typedef struct pwait_char   PWAIT_CHAR;
typedef struct event_data   EVENT_DATA;

typedef struct clan_data    CLAN_DATA;
typedef struct clan_stance  CLAN_STANCE;

/*randomy*/
#include "random_items.h"

/*herbs*/
typedef struct herb_data               HERB_DATA;
typedef struct herb_in_room_data       HERB_IN_ROOM_DATA;

typedef struct new_char_data           NEW_CHAR_DATA;

/*mapka*/
typedef struct	mapka_data	MAP_DATA;

/* FRIEND_WHO: typ listy */
typedef struct friend_list FRIEND_LIST;

typedef struct spirit_data                 SPIRIT_DATA;
/*
 * Function types.
 */
typedef void DO_FUN   args( ( CHAR_DATA *ch, char *argument ) );
/*oprogi*/
typedef void DO_OFUN   args( ( OBJ_DATA *obj, char *argument ) );
typedef bool SPEC_FUN  args( ( CHAR_DATA *ch ) );
typedef void SPELL_FUN args( ( int sn, int level, CHAR_DATA *ch, void *vo, int target ) );
typedef void PROG_FUN  args( ( char *argument ) );
typedef void TRICK_FUN args( ( int sn, CHAR_DATA *ch, CHAR_DATA *victim ) );

#define PROGFUN( fun )	void fun( char *argument )

extern char const * last_command;
//rellik: do debugowania, dodaje pamiêtanie te¿ poprzedniego debug_info
extern char const * previous_command;
//rellik: do debugowania, DEBUF_INFO przerabiam z define na funkcjê
extern void DEBUG_INFO( const char* string );

/*pogoda and stuff*/
#define HOURS_PER_DAY   24
#define DAYS_PER_WEEK    7
#define DAYS_PER_MONTH  35
#define MONTHS_PER_YEAR 17
#define DAYS_PER_YEAR   (DAYS_PER_MONTH * MONTHS_PER_YEAR)
#define MAX_YEAR        100000
#define HOURS_PER_YEAR  ( HOURS_PER_DAY * DAYS_PER_MONTH * MONTHS_PER_YEAR )


#define HOUR_SUNRISE            (HOURS_PER_DAY / 4)
#define HOUR_DAY_BEGIN          (HOUR_SUNRISE + 1)
#define HOUR_NOON               (HOURS_PER_DAY / 2)
#define HOUR_SUNSET             ((HOURS_PER_DAY / 4) * 3)
#define HOUR_NIGHT_BEGIN        (HOUR_SUNSET + 1)
#define HOUR_MIDNIGHT           HOURS_PER_DAY


#define SEASON_WINTER           0
#define SEASON_SPRING           1
#define SEASON_SUMMER           2
#define SEASON_FALL             3
#define SEASON_MAX              4

/*
 * String and memory management parameters.
 */
#define MAX_CHUNKS            100
#define MAX_KEY_HASH         (1024*50)
#define MAX_STRING_LENGTH    8192
#define MAX_INPUT_LENGTH      512
#define PAGELEN                22
#define MAX_VECT_BANK           5

/**
 * TROPHY
 *
 * size - dlugosc listy trophy
 * base - bazowa wartosc jednego pelnego killa
 */
#define TROPHY_SIZE 25
#define TROPHY_BASE 12

/* I am lazy :) */
#define MSL MAX_STRING_LENGTH
#define MIL MAX_INPUT_LENGTH

/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */

#define MAX_SCHOOL            8
#define MAX_SKILL           588
#define MAX_LANG             31
#define MAX_GROUP            30
#define MAX_IN_GROUP         15
#define MAX_ALIAS            60
#define MAX_CLASS            11
#define MAX_PC_RACE           7
#define MAX_RESIST           19
#define MAX_DAMAGE_MESSAGE   46
#define MAX_LEVEL            40
#define MAX_COUNTER           5  /* char_data->counter[] */
#define MAX_TRICKS           23
#define MAX_COMPONENTS       60
#define MAX_MEMSETS           3

//sta³e na podstawie race_table
#define RACE_PC_HUMAN         1
#define RACE_PC_ELF           2
#define RACE_PC_DWARF         3
#define RACE_PC_GNOME         4
#define RACE_PC_HALFELF       5
#define RACE_PC_HALFLING      6
#define RACE_PC_HALFORC       7



#define HOARD_RENT_RATE     120

/*Te co zostawilem musza byc na counterach, bo affect nie moze trwac 'do konca tego ticka'- Kainti
   counter[0] - damage z ostatniego ciosu
   counter[1] - damage z przedostatniego ciosu
   counter[2] - damage z przedprzedostatniego ciosu
   counter[3] - berserk i demon aura
   counter[4] - smite evil, smite good i summony mobkow
   counter[5] - toksycznosc mikstur
*/


#define MAX_HOST       10
#define LEVEL_HERO     (MAX_LEVEL - 9) //31
#define LEVEL_IMMORTAL (MAX_LEVEL - 8) //32
#define LEVEL_NEWBIE    8
#define LEVEL_WHO_SHOW 25


#define PULSE_PER_SECOND     4
#define PULSE_EVENT2       ( 1 * PULSE_PER_SECOND)
#define PULSE_VIOLENCE     ( 3 * PULSE_PER_SECOND)
#define PULSE_MOBILE       ( 4 * PULSE_PER_SECOND)
#define PULSE_MUSIC        ( 6 * PULSE_PER_SECOND)
#define PULSE_TICK         (60 * PULSE_PER_SECOND)
#define PULSE_HINT         (90 * PULSE_PER_SECOND)
#define PULSE_AREA        (120 * PULSE_PER_SECOND)
#define PULSE_FIGHT_SI          (PULSE_PER_SECOND)
#define PULSE_TODELETE	 (3600 * PULSE_PER_SECOND) // 1 hour | delayed ch delete - by Fuyara

#define IMPLEMENTOR  MAX_LEVEL
#define CREATOR      (MAX_LEVEL - 1)
#define SUPREME      (MAX_LEVEL - 2)
#define DEITY        (MAX_LEVEL - 3)
#define GOD          (MAX_LEVEL - 4)
#define IMMORTAL     (MAX_LEVEL - 5)
#define DEMI         (MAX_LEVEL - 6)
#define ANGEL        (MAX_LEVEL - 7)
#define AVATAR       (MAX_LEVEL - 8)
#define HERO         LEVEL_HERO

#define MEM_START     0
#define MEM_DONE      1
#define MEM_STOP_ALL  2
#define MEM_STOP      3
#define MEM_NEW       4
#define MEM_DONE_ALL  5
/*
 * ColoUr stuff v2.0, by Lope.
 */
#define CLEAR      "\e[0m"      /* Resets Colour   */
#define C_RED      "\e[0;31m"   /* Normal Colours   */
#define C_GREEN      "\e[0;32m"
#define C_YELLOW   "\e[0;33m"
#define C_BLUE      "\e[0;34m"
#define C_MAGENTA   "\e[0;35m"
#define C_CYAN      "\e[0;36m"
#define C_WHITE      "\e[0;37m"
#define C_D_GREY   "\e[1;30m"     /* Light Colors      */
#define C_B_RED      "\e[1;31m"
#define C_B_GREEN   "\e[1;32m"
#define C_B_YELLOW   "\e[1;33m"
#define C_B_BLUE   "\e[1;34m"
#define C_B_MAGENTA   "\e[1;35m"
#define C_B_CYAN   "\e[1;36m"
#define C_B_WHITE   "\e[1;37m"
#define C_BOLD          "\e[1m"
#define C_UNDERLINE     "\e[4m"
#define C_FLASHING      "\e[5m"
#define C_INVERSE       "\e[7m"

#define C_BACK_BLACK     "\e[0;40m"
#define C_BACK_RED       "\e[0;41m"
#define C_BACK_GREEN     "\e[0;42m"
#define C_BACK_YELLOW    "\e[0;43m"
#define C_BACK_BLUE      "\e[0;44m"
#define C_BACK_MAGENTA   "\e[0;45m"
#define C_BACK_CYAN      "\e[0;46m"
#define C_BACK_WHITE     "\e[0;47m"


#define C_BACK_L_BLACK   "\e[1;40m"
#define C_BACK_L_RED     "\e[1;41m"
#define C_BACK_L_GREEN   "\e[1;42m"
#define C_BACK_L_YELLOW  "\e[1;43m"
#define C_BACK_L_BLUE    "\e[1;44m"
#define C_BACK_L_MAGENTA "\e[1;45m"
#define C_BACK_L_CYAN    "\e[1;46m"
#define C_BACK_L_WHITE   "\e[1;47m"


#define COLOUR_NONE   7      /* White, hmm...   */
#define RED      1      /* Normal Colours   */
#define GREEN      2
#define YELLOW      3
#define BLUE      4
#define MAGENTA      5
#define CYAN      6
#define WHITE      7
#define BLACK      0

#define NORMAL      0      /* Bright/Normal colours */
#define BRIGHT      1

#define ALTER_COLOUR( type )   if( !str_prefix( argument, "red" ) )      \
            {                  \
                ch->pcdata->type[0] = NORMAL;      \
                ch->pcdata->type[1] = RED;         \
            }                  \
            else if( !str_prefix( argument, "hi-red" ) )   \
            {                  \
                ch->pcdata->type[0] = BRIGHT;      \
                ch->pcdata->type[1] = RED;         \
            }                  \
            else if( !str_prefix( argument, "green" ) )   \
            {                  \
                ch->pcdata->type[0] = NORMAL;      \
                ch->pcdata->type[1] = GREEN;      \
            }                  \
            else if( !str_prefix( argument, "hi-green" ) )   \
            {                  \
                ch->pcdata->type[0] = BRIGHT;      \
                ch->pcdata->type[1] = GREEN;      \
            }                  \
            else if( !str_prefix( argument, "yellow" ) )   \
            {                  \
                ch->pcdata->type[0] = NORMAL;      \
                ch->pcdata->type[1] = YELLOW;      \
            }                  \
            else if( !str_prefix( argument, "hi-yellow" ) )   \
            {                  \
                ch->pcdata->type[0] = BRIGHT;      \
                ch->pcdata->type[1] = YELLOW;      \
            }                  \
            else if( !str_prefix( argument, "blue" ) )   \
            {                  \
                ch->pcdata->type[0] = NORMAL;      \
                ch->pcdata->type[1] = BLUE;      \
            }                  \
            else if( !str_prefix( argument, "hi-blue" ) )   \
            {                  \
                ch->pcdata->type[0] = BRIGHT;      \
                ch->pcdata->type[1] = BLUE;      \
            }                  \
            else if( !str_prefix( argument, "magenta" ) )   \
            {                  \
                ch->pcdata->type[0] = NORMAL;      \
                ch->pcdata->type[1] = MAGENTA;      \
            }                  \
            else if( !str_prefix( argument, "hi-magenta" ) ) \
            {                  \
                ch->pcdata->type[0] = BRIGHT;      \
                ch->pcdata->type[1] = MAGENTA;      \
            }                  \
            else if( !str_prefix( argument, "cyan" ) )   \
            {                  \
                ch->pcdata->type[0] = NORMAL;      \
                ch->pcdata->type[1] = CYAN;         \
            }                  \
            else if( !str_prefix( argument, "hi-cyan" ) )   \
            {                  \
                ch->pcdata->type[0] = BRIGHT;      \
                ch->pcdata->type[1] = CYAN;         \
            }                  \
            else if( !str_prefix( argument, "white" ) )   \
            {                  \
                ch->pcdata->type[0] = NORMAL;      \
                ch->pcdata->type[1] = WHITE;      \
            }                  \
            else if( !str_prefix( argument, "hi-white" ) )   \
            {                  \
                ch->pcdata->type[0] = BRIGHT;      \
                ch->pcdata->type[1] = WHITE;      \
            }                  \
            else if( !str_prefix( argument, "grey" ) )   \
            {                  \
                ch->pcdata->type[0] = BRIGHT;      \
                ch->pcdata->type[1] = BLACK;      \
            }                  \
            else if( !str_prefix( argument, "beep" ) )   \
            {                  \
                ch->pcdata->type[2] = 1;         \
            }                  \
            else if( !str_prefix( argument, "nobeep" ) )   \
            {                  \
                ch->pcdata->type[2] = 0;         \
            }                  \
            else                  \
            {                  \
      send_to_char_bw( "Podano nieznany kolor, pozostawiam bez zmian.\n\r", ch );   \
                return;               \
            }

#define LOAD_COLOUR( field )   ch->pcdata->field[1] = fread_number( fp );   \
            if( ch->pcdata->field[1] > 100 )      \
            {                  \
                ch->pcdata->field[1] -= 100;      \
                ch->pcdata->field[2] = 1;         \
            }                  \
            else                  \
            {                  \
                ch->pcdata->field[2] = 0;         \
            }                  \
            if( ch->pcdata->field[1] > 10 )         \
            {                  \
                ch->pcdata->field[1] -= 10;         \
                ch->pcdata->field[0] = 1;         \
            }                  \
            else                  \
            {                  \
                ch->pcdata->field[0] = 0;         \
            }


/*
 * Site ban structure.
 */

#define BAN_SUFFIX  A
#define BAN_PREFIX  B
#define BAN_NEWBIES C
#define BAN_ALL     D
#define BAN_PERMIT  E

struct   ban_data
{
    BAN_DATA *   next;
    bool   valid;
    sh_int   ban_flags;
    sh_int   level;
    char *   site;
    char *  reason;
    char *  person;
    time_t  date_stamp;
    int     period;
};

struct buf_type
{
    BUFFER *    next;
    bool        valid;
    sh_int      state;  /* error state of the buffer */
    int        size;   /* size in k */
    char *      string; /* buffer's string */
};



/*
 * Time and weather stuff.
 */
#define SUN_DARK              0
#define SUN_RISE              1
#define SUN_LIGHT             2
#define SUN_SET               3

#define SKY_CLOUDLESS         0
#define SKY_CLOUDY            1
#define SKY_RAINING           2
#define SKY_LIGHTNING         3

#define MOON_NEW_MOON         0
#define MOON_WAXING_CRESCENT  1
#define MOON_FIRST_QUARTER    2
#define MOON_WAXING_GIBBOUS   3
#define MOON_FULL_MOON        4
#define MOON_WANING_GIBBOUS   5
#define MOON_LAST_QUARTER     6
#define MOON_WANING_CRESCENT  7
#define MOON_PHASES           8
#define MOON_PHASE_DURATION  96

struct   time_info_data
{
    int      hour;
    int      day;
    int      month;
    int      year;
    int      moon_phase;
    int      moon_phase_passed;
};

struct   weather_data
{
    int      mmhg;
    int      change;
    int      sky;
    int      sunlight;
};


/*
 * Connected state for a channel.
 */
#define CON_CLOSED                      -1
#define CON_PLAYING                      0
#define CON_GET_NAME                     1
#define CON_GET_OLD_PASSWORD             2
#define CON_CONFIRM_NEW_NAME             3
#define CON_GET_NEW_PASSWORD             4
#define CON_CONFIRM_NEW_PASSWORD         5
#define CON_GET_NEW_RACE                 6
#define CON_GET_NEW_SEX                  7
#define CON_GET_NEW_CLASS                8
#define CON_GET_ALIGNMENT                9
#define CON_FINISH_CREATE               10
#define CON_BREAK_CONNECT               13
#define CON_COPYOVER_RECOVER            14
#define CON_GET_ODMIANA                 15
#define CON_ACCEPT_ODMIANA              16
#define CON_MENU                        17
#define CON_PREMENU                     18
#define CON_MENU_NEWPASSWD              19
#define CON_MENU_CONFIRM_NEWPASSWD      20
#define CON_MENU_DELETE                 21
#define CON_MENU_NEWPASSWD2             22
#define CON_GET_MAGE_SPECIALIST         23
#define CON_GET_RESERV_PASSWORD         24
#define CON_MENU_CHANGE_TITLE           25
#define CON_NDENY_OLD_PASS              26
#define CON_NDENY_RSV_PASS              27
#define CON_NDENY_GET_NAME              28
#define CON_NDENY_GET_ODMIANA           29
#define CON_NDENY_ACCEPT_ODMIANA        30
#define CON_NDENY_CONFIRM_NAME          31
#define CON_BREAK_CONNECT_LOOP          32


#define DNS_LOOKUP          0  /* Socket is in dns lookup        */
#define DNS_DONE            1  /* The lookup is done.             */

/**
 * player port
 */
#define PORT_PLAYER 4000

/*
 * Descriptor (channel) structure.
 */
struct   descriptor_data
{
	DESCRIPTOR_DATA *	next;
	DESCRIPTOR_DATA *	snoop_by;
	CHAR_DATA *			character;
	CHAR_DATA *			original;
	NEW_CHAR_DATA *		new_char_data;
	bool				valid;
	char *				host;
	char *				ip_addr;
	int					port;
	sh_int				descriptor;
	sh_int				connected;
	bool				fcommand;
	char				inbuf [4 * MAX_INPUT_LENGTH];
	char				incomm [MAX_INPUT_LENGTH];
	char				inlast [MAX_INPUT_LENGTH];
	int					repeat;
	char *				outbuf;
	int					outsize;
	int					outtop;
	char *				showstr_head;
	char *				showstr_point;
	void *				pEdit;			/* OLC */
	char **				pString;		/* OLC */
	int					editor;			/* OLC */
	char *				user;
	sh_int				pass_fail;
#ifdef MCCP /* mccp: support data */
	unsigned char		compressing;
	z_stream *			out_compress;
	unsigned char *		out_compress_buf;
#endif
	CHAR_DATA *			remote_char;	/* nowe nanny */
	ACCOUNT_DATA *		account;		/* nowe nanny */
	char *				spaces;			/* nowe nanny */
	char *				tmp_passwd;		/* nowe nanny */
	char *				other;			/* nowe nanny */
	bool				ignore_input;	/* nowe nanny */
	char *				custom_map;		/* do randomowych map */
	int					lookup_state;	/* do w±tkowanego szukania w DNS */
};

/*
 * Attribute bonus structures.
 */
struct   str_app_type
{
    sh_int   tohit;
    sh_int   todam;
    sh_int   carry;
    sh_int   wield;
};

struct   int_app_type
{
    sh_int   learn;
    sh_int   learn_chance;
    sh_int   max_spells;
};

struct   wis_app_type
{
    sh_int   bonus_spells;
    sh_int   fail;
    sh_int   mod;//do klasy pancerza mnicha
};

struct   dex_app_type
{
    sh_int   defensive;
};

struct   con_app_type
{
    sh_int   hith;
    sh_int   hitl;
    sh_int   shock;
};



/*
 * TO types for act.
 */
#define TO_ROOM          0
#define TO_NOTVICT       1
#define TO_VICT          2
#define TO_CHAR          3
#define TO_ALL          4
#define TO_BUF          5



/*
 * Help table types.
 */
struct   help_data
{
    HELP_DATA *   next;
    HELP_DATA * next_area;
    sh_int   level;
    char *   keyword;
    char *   text;
};

struct help_area_data
{
   HELP_AREA *   next;
   HELP_DATA *   first;
   HELP_DATA *   last;
   AREA_DATA *   area;
   char *      filename;
   bool      changed;
};

/*
 * Shop and repair types.
 */
#define MAX_TRADE    5

#define REP_ITEM_WEAPON		         0
#define REP_ITEM_LSWORD		         1
#define REP_ITEM_AXE		         2
#define REP_ITEM_MACE                3
#define REP_ITEM_DAGGER              4
#define REP_ITEM_SPEAR               5
#define REP_ITEM_FLAIL               6
#define REP_ITEM_WHIP                7
#define REP_ITEM_POLEARM             8
#define REP_ITEM_STAFF               9
#define REP_ITEM_SHSWORD	        10
#define REP_ITEM_SWORDS		        11
#define REP_ITEM_BLUDGEONS	        12
#define REP_ITEM_ARMOR		        13
#define REP_ITEM_ARMOR_LIGHT	    14
#define REP_ITEM_ARMOR_MEDIUM	    15
#define REP_ITEM_ARMOR_HEAVY	    16
#define REP_ITEM_ARMOR_VHEAVY	    17
#define REP_ITEM_CLOTHING	        18
#define REP_ITEM_JEWELRY	        19
#define REP_ITEM_GEM		        20
#define REP_ITEM_MUSICAL_INSTRUMENT 21
#define REP_ITEM_SHIELD		        22
#define REP_ITEM_TOOL			    23
#define REP_ITEM_CLAWS 				24

struct   shop_data
{
    SHOP_DATA *   next;         /* Next shop in list      */
    unsigned int   keeper;         /* Vnum of shop keeper mob   */
    sh_int   buy_type [MAX_TRADE];   /* Item types shop will buy   */
    sh_int   profit_buy;      /* Cost multiplier for buying   */
    sh_int   profit_sell;      /* Cost multiplier for selling   */
    sh_int   open_hour;      /* First opening hour      */
    sh_int   close_hour;      /* First closing hour      */
    ush_int shop_room;
};

struct   bank_data
{
    BANK_DATA *   next;
    ush_int   banker;
    sh_int   provision;
    sh_int   open_hour;
    sh_int   close_hour;
    sh_int  bank_room;
};

struct repair_data
{
    REPAIR_DATA   *next;
    ush_int item_type;
    sh_int   max_cost;
    sh_int   repair_cost_mod;
    sh_int   min_condition;
    sh_int   repair_open_hour;
    sh_int   repair_close_hour;
    ush_int  repair_room;
};

struct repair_item_type
{
    char *	name;
    int		item_id;
    char * 	info;
};

extern struct repair_item_type repair_item_table[];

/*
 * Per-class stuff.
 */

#define MAX_GUILD  2
#define MAX_STATS  7
#define STAT_STR   0
#define STAT_INT   1
#define STAT_WIS   2
#define STAT_DEX   3
#define STAT_CON   4
#define STAT_CHA   5
#define STAT_LUC   6


struct charm_data
{
    CHAR_DATA *      victim;
    int         type;
    CHARM_DATA *   next;
};

struct bitvect_data
{
    int bank;
    int vector;
};

struct   class_type
{
    char *   name;         /* the full name of the class */
    char *   female_name; /* the full female name of the class :P */
    char *   name_31;	//nazwa profesji dla 31 - lewelowca
    char *   female_name_31;//j/w damska nazwa
    char    who_name   [4];   /* Three-letter name for 'who'   */
    sh_int   attr_prime;      /* Prime attribute      */
    sh_int   thac0_00;      /* Thac0 for level  0      */
    sh_int   thac0_32;      /* Thac0 for level 32      */
    sh_int   hp_min;         /* Min hp gained on leveling   */
    sh_int   hp_max;         /* Max hp gained on leveling   */
    sh_int    caster;
    sh_int class_stat_mod[MAX_STATS][2];// NOWE STATY
};

#define POISON_PARALIZING   (A) //5
#define POISON_DEATH        (B) //6
#define POISON_WEAKEN       (C) //7
#define POISON_BLIND        (D) //8
#define POISON_SLEEP        (E) //9
#define POISON_SLOWERED     (F) //0
#define POISON_WEAK         (G) //1
#define POISON_NORMAL       (H) //2
#define POISON_STRONG       (I) //3
#define POISON_STRONGEST    (J) //4
#define MAX_POISON       9

struct poison_type
{
    char *   name;
    sh_int   saved_damage[3];
    sh_int   full_damage[3];
    int      special_effects;
    sh_int   duration[3];

};

#define MAT_METAL   (A)
#define MAT_EASYBREAK   (B)

struct material_type
{
    char *   name;         /* name */
    char *   adjective;
    sh_int   hardness;      /* 1-100 */
    sh_int   fire_res;
    sh_int   acid_res;
    sh_int   shock_res;
    sh_int   cold_res;
    sh_int   other_res;
    int      flag;
};

struct armor_type
{
    char *    name;
    char   min_ac[4];
    char   can_wear[MAX_CLASS];
};

struct item_type
{
    int      type;
    char *   name;
};

struct weapon_type
{
    char *   name;
    sh_int   type;
    sh_int   *gsn;
};

struct instrument_type
{
    char *   name;
    sh_int   type;
    sh_int   *gsn;
};

struct wand_type
{
    char *   name;
    sh_int   type;
};

struct wiznet_type
{
    char *   name;
    long    flag;
    int      level;
};

struct attack_type
{
    char *   name;         /* name */
    char *   noun;         /* message */
    int      damage;         /* damage class */
};

struct race_type
{
	char *		name; 			/* call name of the race */
	char *		name2;
	char *		name3;
	char *		name4;
	char *		name5;
	char *		name6;
	bool		pc_race;		/* can be chosen by pcs */
	char *		act;			/* act bits for the race */
	int			skin_multi;		/*mno¿nik do warto¶ci skóry*/
	char *		aff;
	char *		off;			/* off bits for the race */
	long		form;			/* default form flag for the race */
	long		parts;			/* default parts for the race */
	sh_int		type;
	char *		arrive[3];
	char *		leave[3];
	char		resists[MAX_RESIST];
	int			wear_flag;
  int     min_level;
  int     max_level;
  char *    say_text;
  char *    ask_text;
  char *    say_text_2;
};


struct pc_race_type  /* additional data for pc races */
{
    char *   name;         /* MUST be in race_type */
    char    who_name[12];      /* nieistotne*/
    sh_int   class_flag;      /*bits for available classes*/
    sh_int   bonus_stat[MAX_STATS];   /* bonus +/- do statsow */
    char *   skills[5];      /* bonus skills for the race */
    sh_int   size;         /* rozmiar */
    long   well_known_lang;   /* jezyki/jezyk rasowy */
    long   known_lang;      /* jezyki/jezyk znany troche */
    long   cant_learn_lang;   /* jezyki/jezyk ktorych nigdy sie nie nauczy*/
    sh_int   height[4];   /* wzrost, min-max*/
    sh_int   weight[4];   /* waga min-max */
    sh_int   age[2];   /* wiek min-max */
	sh_int race_stat_mod[MAX_STATS][2]; //modyfikatory do dorolek do stattow zalezne od rasy // NOWE STATY

};


struct spec_type
{
    char *    name;         /* special function name */
    SPEC_FUN *   function;      /* the function */
};


/*
 * Data structure for notes.
 */

#define NOTE_NOTE    0
#define NOTE_PENALTY 2
#define NOTE_NEWS    3
#define NOTE_CHANGES 4

struct   note_data
{
	NOTE_DATA *		next;
	bool			valid;
	sh_int			type;
	char *			sender;
	char *			date;
	char *			to_list;
	char *			subject;
	char *			text;
	time_t			date_stamp;
};

/*
 * do memowania spelli
 */
struct   mspell_data
{
	MSPELL_DATA *	next; 		/* do listy */
	MSPELL_DATA *	sort_next;  /* do sortowania po krêgach */
	bool			valid;		/* do testow */
	sh_int			spell;		/* numer spella */
	sh_int			circle;		/* ktory krag */
	bool			done;		/* czy skonczone */
	sh_int			duration;	/* pozosta³y czas zapamietywania w minidelayach */
};

struct  memset_data
{
	MEMSET_DATA *	next; 		              /* wskaznik do kolejnego memsetu */
	bool			valid;                    /* dla recyclingu */
	char *	        set_name;                 /* nazwa memsetu */
	char            set_spells[MAX_SKILL];    /* spelle w secie, char nie int, bo oszczedzamy pamiec :) */
};

/* lista nauczycieli u jakich gracz jakiegos zaklecia sie nie naumial*/
struct spell_failed
{
	SPELL_FAILED *	next;
	bool			valid;
	unsigned int	vnum;
	sh_int			spell;
};

/*
 * An affect.
 */
struct   affect_data
{
	AFFECT_DATA *	next;
	bool			valid;
	char *			info;
	bool			visible;
	sh_int			where;
	sh_int			type;
	sh_int			level;
	sh_int			duration;
	int 			rt_duration; //rellik: real time duration [20080628]
	sh_int			location;
	int				modifier;
	BITVECT_DATA *	bitvector;
	int				extra_flags; //Rysand - double word do zapisywania flag - np. wielu poisonów
};


/* where definitions */
#define TO_AFFECTS  0
#define TO_OBJECT   1
#define TO_WEAPON   5

/*
 * A kill structure (indexed by level).
 */
struct   kill_data
{
    sh_int      number;
    sh_int      killed;
};

/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (Start of section ... start here)                     *
 *                                                                         *
 ***************************************************************************/

/*
 * Well known mob virtual numbers.
 * Defined in #MOBILES.
 */

// <planty>
#define MOB_VNUM_LIVEOAK     10902//liveoak
#define MOB_VNUM_BARKGUARD     10910//bark guardian
#define MOB_VNUM_WILDTHORN     10911//wildthorn
//</planty>
#define MOB_VNUM_SNAKE        3016 //animate staff maga przemian
#define MOB_VNUM_GOLEM        10904//change staff maga przemian
#define MOB_VNUM_AVATAR 3036//call avatar black knighta

//<raisy>
#define MOB_VNUM_SKELET           11
#define MOB_VNUM_SKELET1           3023
#define MOB_VNUM_SKELET2           3024
#define MOB_VNUM_SKELET3           3025
#define MOB_VNUM_SKELET4           3026
#define MOB_VNUM_SKELET5           3027
#define MOB_VNUM_SKELET6           3028
#define MOB_VNUM_SKELET7           3330
#define MOB_VNUM_SKELET8           3331
#define MOB_VNUM_SKELET9           3332
#define MOB_VNUM_SKELET10          3333
#define MOB_VNUM_ZOMBIE           12
#define MOB_VNUM_ZOMBIE1           3017
#define MOB_VNUM_ZOMBIE2           3018
#define MOB_VNUM_ZOMBIE3           3019
#define MOB_VNUM_ZOMBIE4           3020
#define MOB_VNUM_ZOMBIE5           3021
#define MOB_VNUM_ZOMBIE6           3022
#define MOB_VNUM_GHUL1       13
#define MOB_VNUM_GHUL2       14
#define MOB_VNUM_GHUL3       15
#define MOB_VNUM_GHAST1       16
#define MOB_VNUM_GHAST2       18
#define MOB_VNUM_GHAST3       19
#define MOB_VNUM_SOUL       17
//</raisy>

//<summony>
#define MOB_VNUM_OSA          3001
#define MOB_VNUM_WAZKA        3002
#define MOB_VNUM_ZUK          3003
#define MOB_VNUM_GOBLIN       3004
#define MOB_VNUM_HOBGOBLIN    3005
#define MOB_VNUM_TASLOI       3006
#define MOB_VNUM_GRYF         3007
#define MOB_VNUM_MANTYKORA    3008
#define MOB_VNUM_HARPIA       3009
#define MOB_VNUM_SAHUAGIN     3010
#define MOB_VNUM_GITHYANKI    3011
#define MOB_VNUM_NAGA         3012
#define MOB_VNUM_DZINN        3013
#define MOB_VNUM_JEDNOROZEC   3014
#define MOB_VNUM_HYDRA        3015
//</summony>

//<iluzje>
#define MOB_VNUM_SHADOW_IFRYT 3029
#define MOB_VNUM_SHADOW_FEYR 3030
#define MOB_VNUM_SHADOW_OGR 3031
#define MOB_VNUM_SHADOW_MYKONID 3032
#define MOB_VNUM_SHADOW_HAKOWA_POCZWARA 3033
#define MOB_VNUM_SHADOW_JASZCZUROCZLEK 3034
#define MOB_VNUM_FETCH 3035//sobowtór
#define MOB_VNUM_SHADOW_MSCICIEL 3064
#define MOB_VNUM_SHADOW_NIETOPERZ 3067
#define MOB_VNUM_SHADOW_JEZDZIEC 3068
#define MOB_VNUM_SHADOW_WAZ 3069
#define MOB_VNUM_SHADOW_ZJAWA 3070
#define MOB_VNUM_SHADOW_IMP 3071
#define MOB_VNUM_SHADOW_PAJAK 3072
#define MOB_VNUM_SHADOW_OGAR 3073
#define MOB_VNUM_SHADOW_KRUK 3075
#define MOB_VNUM_SHADOW_SKORPION 3076
#define MOB_VNUM_SHADOW_LEPRECHAUN 3077
#define MOB_VNUM_SHADOW_DZIN 3078
//</iluzje>

//<summon animal>
#define MOB_VNUM_BADGER        10853
#define MOB_VNUM_BEAR          10851
#define MOB_VNUM_BEAR_M        10852
#define MOB_VNUM_BOAR          10855
#define MOB_VNUM_CONSTRICTOR_M 10854
#define MOB_VNUM_EAGLE_M       10857
#define MOB_VNUM_RAWEN         10856
#define MOB_VNUM_SPIDER_M      10858
#define MOB_VNUM_SQUIRREL      10861
#define MOB_VNUM_SQUIRREL_M    10862
#define MOB_VNUM_TIGER         10859
#define MOB_VNUM_TIGER_M       10860
#define MOB_VNUM_WOLF          10863
#define MOB_VNUM_WOLF_M        10864
//</summon animal>

//Drake: Dodanie pomniejszych elementali jako wysokopoziomowe summony dla druidów ( i w przysz³oœci dla magów).
//<elementy>
#define MOB_VNUM_ELEMENTAL_P 10906
#define MOB_VNUM_ELEMENTAL_W 10907
#define MOB_VNUM_ELEMENTAL_Z 10908
#define MOB_VNUM_ELEMENTAL_O 10909
//</elementy>

//<summon malfunction>
#define MOB_VNUM_AURUMVORAX 		3048
#define MOB_VNUM_TYRANOZAUR			3049
#define MOB_VNUM_FENIX 				3050
#define MOB_VNUM_GARGANTUA 			3051
#define MOB_VNUM_DRAKOLICZ 			3052
#define MOB_VNUM_DEMILICZ 			3053
#define MOB_VNUM_UPIORNY_RYCERZ 	3054
#define MOB_VNUM_WAMPIR 			3055
#define MOB_VNUM_GARGULEC 			3056
#define MOB_VNUM_GREATER_GOLEM		3057
#define MOB_VNUM_SKRIAXIT			3058
#define MOB_VNUM_JUGGERNAUT			3059
#define MOB_VNUM_ARGOS				3060
#define MOB_VNUM_ZIELONY_ABISHAI	3061
#define MOB_VNUM_LAMMASU			3062
#define MOB_VNUM_BALOR				3063


//mining mobs - used to scarry pc's, all agressive
#define MOB_VNUM_MINE_DRAKOLICZ 3079
#define MOB_VNUM_MINE_LARGE_WORM 3079
#define MOB_VNUM_MINE_GOBLIN 3080
#define MOB_VNUM_MINE_SMALL_WORM 3081


/* RT ASCII conversions -- used so we can have letters in this file */

#define A         1
#define B         2
#define C         4
#define D         8
#define E         16
#define F         32
#define G         64
#define H         128

#define I         256
#define J         512
#define K          1024
#define L          2048
#define M         4096
#define N          8192
#define O         16384
#define P         32768

#define Q         65536
#define R         131072
#define S         262144
#define T         524288
#define U         1048576
#define V         2097152
#define W         4194304
#define X         8388608

#define Y         16777216
#define Z         33554432
#define aa        67108864    /* doubled due to conflicts */
#define bb        134217728
#define cc        268435456
#define dd        536870912
#define ee        1073741824


/* school definitions */
#define Odrzucanie					(A)
#define Przemiany					(B)
#define Przywolanie					(C)
#define Poznanie					(D)
#define Zauroczenie					(E)
#define Iluzje						(F)
#define Inwokacje					(G)
#define Nekromancja					(H)
#define SpellSpec					(I)
#define SpellSpecAndGeneral			(J)

#define CLASS_MAG					0    //1
#define CLASS_CLERIC				1    //2
#define CLASS_THIEF					2 //4
#define CLASS_WARRIOR				3 //8
#define CLASS_PALADIN				4 //16
#define CLASS_DRUID					5 //32
#define CLASS_BARBARIAN				6 //64
#define CLASS_MONK					7 //128
#define CLASS_BARD					8 //256
#define CLASS_BLACK_KNIGHT			9 //512
#define CLASS_SHAMAN			10      //1024


/* Typy modyfikatoro w walce */
#define FIGHT_PARRY						(A)   /* wiadomo, paruje */
#define FIGHT_DISARM					(B)   /* jesli disarmowal to -4 to trafienia */
#define FIGHT_CASTING					(C)   /* jesli rzuca zaklecie traci atak*/
#define FIGHT_DODGE						(D)   /* dodging */
#define FIGHT_MIGHTY_BLOW				(E)
#define FIGHT_POWER_STRIKE				(F)
#define FIGHT_CRITICAL_STRIKE			(G)
#define FIGHT_OVERWHELMING_STRIKE		(H)
#define FIGHT_CLEAVE					(I)
#define FIGHT_VERTICAL_SLASH			(J)
#define FIGHT_TORMENT_NOFLEE			(K)
#define FIGHT_HTH_CHARGE				(L)
#define FIGHT_TRICK_WIELDSECOND			(M)
#define FIGHT_CRUSHING_MACE_DAM			(N)
#define FIGHT_THOUSANDSLAYER			(O)


/* Typy ras
 * uzyte przy holdach (person/animal/plant/monster)
 */
#define ANIMAL			(A)
#define PLANT			(B)
#define PERSON			(C)
#define MONSTER			(D)
#define UNDEAD			(E)
#define UNKNOWN			(F)
#define INSECT			(G)
#define DEMON			(H)
#define OUTSIDER      (I)

/* damage classes */
#define DAM_NONE                0
#define DAM_BASH                1
#define DAM_PIERCE              2
#define DAM_SLASH               3
#define DAM_FIRE                4
#define DAM_COLD                5
#define DAM_LIGHTNING           6
#define DAM_ACID                7
#define DAM_POISON              8
#define DAM_NEGATIVE            9
#define DAM_HOLY                10
#define DAM_ENERGY              11
#define DAM_MENTAL              12
#define DAM_DISEASE             13
#define DAM_DROWNING            14
#define DAM_LIGHT      			15
#define DAM_OTHER               16
#define DAM_HARM      			17
#define DAM_CHARM      			18
#define DAM_SOUND      			19

/* typy obrazen do tablicy resistow */
#define RESIST_ALL       	0
#define RESIST_PIERCE       1
#define RESIST_SLASH       	2
#define RESIST_BASH       	3
#define RESIST_MAGIC       	4
#define RESIST_FIRE       	5
#define RESIST_MAGIC_FIRE   6
#define RESIST_ELECTRICITY  7
#define RESIST_COLD       	8
#define RESIST_ACID       	9
#define RESIST_POISON      	10
#define RESIST_MAGIC_WEAPON 11
#define RESIST_CHARM      	12
#define RESIST_FEAR      	13
#define RESIST_SUMMON      	14
#define RESIST_NEGATIVE     15
#define RESIST_MENTAL      	16
#define RESIST_HOLY  		17
#define RESIST_SOUND      	18

/* return values for check_imm */
#define IS_NORMAL      		0
#define IS_IMMUNE      		1
#define IS_RESISTANT    	2
#define IS_VULNERABLE      	3

/* IMM bits for mobs */
#define IMM_SUMMON              (A)
#define IMM_CHARM               (B)
#define IMM_MAGIC               (C)
#define IMM_WEAPON              (D)
#define IMM_BASH                (E)
#define IMM_PIERCE              (F)
#define IMM_SLASH               (G)
#define IMM_FIRE                (H)
#define IMM_COLD                (I)
#define IMM_LIGHTNING           (J)
#define IMM_ACID                (K)
#define IMM_POISON              (L)
#define IMM_NEGATIVE            (M)
#define IMM_HOLY                (N)
#define IMM_ENERGY              (O)
#define IMM_MENTAL              (P)
#define IMM_DISEASE             (Q)
#define IMM_DROWNING            (R)
#define IMM_LIGHT      			(S)
#define IMM_SOUND      			(T)
#define IMM_WOOD                (X)
#define IMM_SILVER              (Y)
#define IMM_IRON                (Z)

/* RES bits for mobs */
#define RES_SUMMON      		(A)
#define RES_CHARM      			(B)
#define RES_MAGIC               (C)
#define RES_WEAPON              (D)
#define RES_BASH                (E)
#define RES_PIERCE              (F)
#define RES_SLASH               (G)
#define RES_FIRE                (H)
#define RES_COLD                (I)
#define RES_LIGHTNING           (J)
#define RES_ACID                (K)
#define RES_POISON              (L)
#define RES_NEGATIVE            (M)
#define RES_HOLY                (N)
#define RES_ENERGY              (O)
#define RES_MENTAL              (P)
#define RES_DISEASE             (Q)
#define RES_DROWNING            (R)
#define RES_LIGHT      			(S)
#define RES_SOUND      			(T)
#define RES_WOOD                (X)
#define RES_SILVER              (Y)
#define RES_IRON                (Z)

/* VULN bits for mobs */
#define VULN_SUMMON      (A)
#define VULN_CHARM      (B)
#define VULN_MAGIC              (C)
#define VULN_WEAPON             (D)
#define VULN_BASH               (E)
#define VULN_PIERCE             (F)
#define VULN_SLASH              (G)
#define VULN_FIRE               (H)
#define VULN_COLD               (I)
#define VULN_LIGHTNING          (J)
#define VULN_ACID               (K)
#define VULN_POISON             (L)
#define VULN_NEGATIVE           (M)
#define VULN_HOLY               (N)
#define VULN_ENERGY             (O)
#define VULN_MENTAL             (P)
#define VULN_DISEASE            (Q)
#define VULN_DROWNING           (R)
#define VULN_LIGHT      (S)
#define VULN_SOUND      (T)
#define VULN_WOOD               (X)
#define VULN_SILVER             (Y)
#define VULN_IRON      (Z)

/* body form */
#define FORM_EDIBLE             (A) /* jadalny */
#define FORM_POISON             (B) /* trujacy */
#define FORM_MAGICAL            (C) /* magiczny */
#define FORM_INSTANT_DECAY      (D) /* natychmiastowy rozpad? */
#define FORM_OTHER              (E) /* defined by material bit */

/* actual form */
#define FORM_ANIMAL             (G) /* zwierze */
#define FORM_SENTIENT           (H) /* odczuwajace / czujace? */
#define FORM_UNDEAD             (I) /* martwiak */
#define FORM_CONSTRUCT          (J) /* konstrukt - stworzone cos z czegos, np. golem */
#define FORM_MIST               (K) /* mgla / opar */
#define FORM_INTANGIBLE         (L) /* "niedotykalne" / duchowe */

#define FORM_BIPED              (M) /* dwunozne */
#define FORM_CENTAUR            (N) /* centaur - konio-czlowiek */
#define FORM_INSECT             (O) /* insekt */
#define FORM_SPIDER             (P) /* pajak */
#define FORM_CRUSTACEAN         (Q) /* skorupiak */
#define FORM_WORM               (R) /* robal */
#define FORM_BLOB      (S) /* "kleks"... raczej jakas galareta */

#define FORM_MAMMAL             (V) /* ssak */
#define FORM_BIRD               (W) /* ptak */
#define FORM_REPTILE            (X) /* gad */
#define FORM_SNAKE              (Y) /* wezyk */
#define FORM_DRAGON             (Z) /* smocek */
#define FORM_AMPHIBIAN          (aa) /* plaz */
#define FORM_FISH               (bb)
#define FORM_COLD_BLOOD      	(cc)
#define FORM_WARM      			(F)

/* body parts - czesci ciala */
#define PART_HEAD               (A)
#define PART_ARMS               (B)
#define PART_LEGS               (C)
#define PART_HEART              (D)
#define PART_BRAINS             (E)
#define PART_GUTS               (F)
#define PART_HANDS              (G)
#define PART_FEET               (H)
#define PART_FINGERS            (I)
#define PART_EAR                (J)
#define PART_EYE                (K)
#define PART_LONG_TONGUE        (L)
#define PART_EYESTALKS          (M)
#define PART_TENTACLES          (N)
#define PART_FINS               (O)
#define PART_WINGS              (P)
#define PART_TAIL               (Q)
#define PART_BONES              (R)
#define PART_SKIN               (S)
#define PART_FEATHERS           (T)
/* for combat - do walki */
#define PART_CLAWS              (U)
#define PART_FANGS              (V)
#define PART_HORNS              (W)
#define PART_SCALES             (X)
#define PART_TUSKS              (Y)

extern BITVECT_DATA EXT_NONE;

/*
 * Bits for 'affected_by'.
 * Used in #MOBILES.
 */
extern BITVECT_DATA AFF_NONE;
extern BITVECT_DATA AFF_BLIND;
extern BITVECT_DATA AFF_INVISIBLE;
extern BITVECT_DATA AFF_DETECT_EVIL;
extern BITVECT_DATA AFF_DETECT_INVIS;
extern BITVECT_DATA AFF_DETECT_MAGIC;
extern BITVECT_DATA AFF_DETECT_HIDDEN;
extern BITVECT_DATA AFF_DETECT_GOOD;
extern BITVECT_DATA AFF_SANCTUARY;
extern BITVECT_DATA AFF_FAERIE_FIRE;
extern BITVECT_DATA AFF_INFRARED;
extern BITVECT_DATA AFF_CURSE;
extern BITVECT_DATA AFF_PARALYZE;
extern BITVECT_DATA AFF_POISON;
extern BITVECT_DATA AFF_PROTECT_EVIL;
extern BITVECT_DATA AFF_PROTECT_GOOD;
extern BITVECT_DATA AFF_SNEAK;
extern BITVECT_DATA AFF_HIDE;
extern BITVECT_DATA AFF_SLEEP;
extern BITVECT_DATA AFF_CHARM;
extern BITVECT_DATA AFF_FLYING;
extern BITVECT_DATA AFF_PASS_DOOR;
extern BITVECT_DATA AFF_HASTE;
extern BITVECT_DATA AFF_CALM;
extern BITVECT_DATA AFF_PLAGUE;
extern BITVECT_DATA AFF_WEAKEN;
extern BITVECT_DATA AFF_DARK_VISION;
extern BITVECT_DATA AFF_BERSERK;
extern BITVECT_DATA AFF_COMPREHEND_LANGUAGES;
extern BITVECT_DATA AFF_ENERGIZE;
extern BITVECT_DATA AFF_ARMOR;
extern BITVECT_DATA AFF_TROLL;
extern BITVECT_DATA AFF_ETHEREAL_ARMOR;
extern BITVECT_DATA AFF_SWIM;
extern BITVECT_DATA AFF_REGENERATION;
extern BITVECT_DATA AFF_SLOW;
extern BITVECT_DATA AFF_UNDEAD_INVIS;
extern BITVECT_DATA AFF_ANIMAL_INVIS;
extern BITVECT_DATA AFF_RESIST_FIRE;
extern BITVECT_DATA AFF_RESIST_ACID;
extern BITVECT_DATA AFF_RESIST_COLD;
extern BITVECT_DATA AFF_ENTANGLE;
extern BITVECT_DATA AFF_SILENCE;
extern BITVECT_DATA AFF_DUST_EVIL;
extern BITVECT_DATA AFF_CHANT;
extern BITVECT_DATA AFF_PRAYER;
extern BITVECT_DATA AFF_WATERWALK;
extern BITVECT_DATA AFF_WATERBREATH;
extern BITVECT_DATA AFF_RESIST_LIGHTNING;
extern BITVECT_DATA AFF_BRAVE_CLOAK;
extern BITVECT_DATA AFF_CONFUSION;
extern BITVECT_DATA AFF_ENERGY_SHIELD;
extern BITVECT_DATA AFF_RESIST_MAGIC;
extern BITVECT_DATA AFF_BARK_SKIN;
extern BITVECT_DATA AFF_STONE_SKIN;
extern BITVECT_DATA AFF_MIRROR_IMAGE;
extern BITVECT_DATA AFF_FREE_ACTION;
extern BITVECT_DATA AFF_SHIELD;
extern BITVECT_DATA AFF_WEB;
extern BITVECT_DATA AFF_DEAFNESS;
extern BITVECT_DATA AFF_FEAR;
extern BITVECT_DATA AFF_RESIST_NORMAL_WEAPON;
extern BITVECT_DATA AFF_RESIST_MAGIC_WEAPON;
extern BITVECT_DATA AFF_FIRESHIELD;
extern BITVECT_DATA AFF_ICESHIELD;
extern BITVECT_DATA AFF_REFLECT_SPELL;
extern BITVECT_DATA AFF_MEDITATION;
extern BITVECT_DATA AFF_RECUPERATE;
extern BITVECT_DATA AFF_INCREASE_WOUNDS;
extern BITVECT_DATA AFF_MINOR_GLOBE;
extern BITVECT_DATA AFF_GLOBE;
extern BITVECT_DATA AFF_MAJOR_GLOBE;
extern BITVECT_DATA AFF_BLADE_BARRIER;
extern BITVECT_DATA AFF_IMMOLATE;
extern BITVECT_DATA AFF_RESIST_NEGATIVE;
extern BITVECT_DATA AFF_RESIST_FEAR;
extern BITVECT_DATA AFF_DAZE;
extern BITVECT_DATA AFF_EYES_OF_THE_TORTURER;
extern BITVECT_DATA AFF_RESIST_SUMMON;
extern BITVECT_DATA AFF_ILLUSION;
extern BITVECT_DATA AFF_SOUL;
extern BITVECT_DATA AFF_RAZORBLADED;
extern BITVECT_DATA AFF_BEAST_CLAWS;
extern BITVECT_DATA AFF_ZAKUTY;
extern BITVECT_DATA AFF_HALLUCINATIONS_POSITIVE;
extern BITVECT_DATA AFF_HALLUCINATIONS_NEGATIVE;
extern BITVECT_DATA AFF_DETECT_AGGRESSIVE;
extern BITVECT_DATA AFF_FORCE_FIELD;
extern BITVECT_DATA AFF_CONFUSION_SHELL;
extern BITVECT_DATA AFF_FLOAT;
extern BITVECT_DATA AFF_RESIST_WEAPON;
extern BITVECT_DATA AFF_LOYALTY;
extern BITVECT_DATA AFF_MAZE;
extern BITVECT_DATA AFF_DEFLECT_WOUNDS;
extern BITVECT_DATA AFF_ASTRAL_JOURNEY;
extern BITVECT_DATA AFF_LOOP;
extern BITVECT_DATA AFF_PIERCING_SIGHT;
extern BITVECT_DATA AFF_DETECT_UNDEAD;
extern BITVECT_DATA AFF_SUMMON_DISTORTION;
extern BITVECT_DATA AFF_NONDETECTION;
extern BITVECT_DATA AFF_STABILITY;
extern BITVECT_DATA AFF_HEALING_TOUCH;
extern BITVECT_DATA AFF_PERFECT_SENSES;
extern BITVECT_DATA AFF_CAUTIOUS_SLEEP;
extern BITVECT_DATA AFF_SENSE_LIFE;
extern BITVECT_DATA AFF_SENSE_FATIGUE;
extern BITVECT_DATA AFF_SHIELD_OF_NATURE;
extern BITVECT_DATA AFF_AURA;
extern BITVECT_DATA AFF_SUBDUE_SPIRITS;
extern BITVECT_DATA AFF_SPIRIT_WALK;
extern BITVECT_DATA AFF_HEALING_SALVE;
extern BITVECT_DATA AFF_ANCESTORS_WISDOM;
extern BITVECT_DATA AFF_ANCESTORS_PROTECTION;
extern BITVECT_DATA AFF_ANCESTORS_FAVOR;
extern BITVECT_DATA AFF_ANCESTORS_VISION;
extern BITVECT_DATA AFF_ANCESTORS_FURY;
extern BITVECT_DATA AFF_SPIRIT_OF_LIFE;
extern BITVECT_DATA AFF_BREATH_OF_LIFE;
extern BITVECT_DATA AFF_SPIRITUAL_GUIDANCE;
extern BITVECT_DATA AFF_MAGIC_HASTE;
extern BITVECT_DATA AFF_SCRYING_SHIELD;
extern BITVECT_DATA AFF_SNEAK_INVIS;
extern BITVECT_DATA AFF_FARSIGHT;
extern BITVECT_DATA AFF_SEAL_OF_ATROCITY;
extern BITVECT_DATA AFF_SEAL_OF_DESPAIR;
extern BITVECT_DATA AFF_INVOKE;

/* bits for act */

extern BITVECT_DATA ACT_IS_NPC;
extern BITVECT_DATA ACT_SENTINEL;
extern BITVECT_DATA ACT_SCAVENGER;
extern BITVECT_DATA ACT_MOUNTABLE;
extern BITVECT_DATA ACT_MEMORY;
extern BITVECT_DATA ACT_AGGRESSIVE;
extern BITVECT_DATA ACT_STAY_AREA;
extern BITVECT_DATA ACT_WIMPY;
extern BITVECT_DATA ACT_BARBARIAN;
extern BITVECT_DATA ACT_PALADIN;
extern BITVECT_DATA ACT_PRACTICE;
extern BITVECT_DATA ACT_DRUID;
extern BITVECT_DATA ACT_RAND_ITEMS;
extern BITVECT_DATA ACT_RAISED;
extern BITVECT_DATA ACT_UNDEAD;
extern BITVECT_DATA ACT_NO_EXP;
extern BITVECT_DATA ACT_CLERIC;
extern BITVECT_DATA ACT_MAGE;
extern BITVECT_DATA ACT_THIEF;
extern BITVECT_DATA ACT_WARRIOR;
extern BITVECT_DATA ACT_NOALIGN;
extern BITVECT_DATA ACT_NOPURGE;
extern BITVECT_DATA ACT_OUTDOORS;
extern BITVECT_DATA ACT_STAY_SECTOR;
extern BITVECT_DATA ACT_INDOORS;
extern BITVECT_DATA ACT_MONK;
extern BITVECT_DATA ACT_BARD;
extern BITVECT_DATA ACT_NOFOLLOW;
extern BITVECT_DATA ACT_UPDATE_ALWAYS;
extern BITVECT_DATA ACT_BLACK_KNIGHT;
extern BITVECT_DATA ACT_RUN_PROGS_DESPITE_CHARM;
extern BITVECT_DATA ACT_BOSS;
extern BITVECT_DATA ACT_GOSSIP;
extern BITVECT_DATA ACT_SHAMAN;
extern BITVECT_DATA ACT_REWARD;
extern BITVECT_DATA ACT_SAGE;

extern BITVECT_DATA PLR_UNREGISTERED;
extern BITVECT_DATA PLR_AUTOEXIT;
extern BITVECT_DATA PLR_POL_ISO;
extern BITVECT_DATA PLR_POL_WIN;
extern BITVECT_DATA PLR_POL_NOPOL;
extern BITVECT_DATA PLR_AUTOMEM;
extern BITVECT_DATA PLR_SHOWPOS;
extern BITVECT_DATA PLR_SHOWPOSS;
extern BITVECT_DATA PLR_COMBAT_PROMPT;
extern BITVECT_DATA PLR_AUTOGOLD;
extern BITVECT_DATA PLR_AUTOSPLIT;
extern BITVECT_DATA PLR_REVBOARD;
extern BITVECT_DATA PLR_HOLYLIGHT;
extern BITVECT_DATA PLR_COMMANDSPL;
extern BITVECT_DATA PLR_ACCEPTSURR;
extern BITVECT_DATA PLR_NOFOLLOW;
extern BITVECT_DATA PLR_COLOUR;
extern BITVECT_DATA PLR_PERMIT;
extern BITVECT_DATA PLR_LOG;
extern BITVECT_DATA PLR_DENY;
extern BITVECT_DATA PLR_FREEZE;
extern BITVECT_DATA PLR_THIEF;
extern BITVECT_DATA PLR_KILLER;
extern BITVECT_DATA PLR_STYL1;
extern BITVECT_DATA PLR_STYL2;
extern BITVECT_DATA PLR_STYL3;
extern BITVECT_DATA PLR_STYL4;
extern BITVECT_DATA PLR_SMOKED;
/* OFF bits for mobiles */
extern BITVECT_DATA OFF_AREA_ATTACK;
extern BITVECT_DATA OFF_BACKSTAB;
extern BITVECT_DATA OFF_BASH;
extern BITVECT_DATA OFF_BERSERK;
extern BITVECT_DATA OFF_DISARM;
extern BITVECT_DATA OFF_DODGE;
extern BITVECT_DATA OFF_DEVOUR;
extern BITVECT_DATA OFF_FAST;
extern BITVECT_DATA OFF_KICK;
extern BITVECT_DATA OFF_CHARGE;
extern BITVECT_DATA OFF_PARRY;
extern BITVECT_DATA OFF_RESCUE;
extern BITVECT_DATA OFF_TAIL;
extern BITVECT_DATA OFF_TRIP;
extern BITVECT_DATA OFF_CRUSH;
extern BITVECT_DATA OFF_ASSIST_ALL;
extern BITVECT_DATA OFF_ASSIST_ALIGN;
extern BITVECT_DATA OFF_ASSIST_RACE;
extern BITVECT_DATA OFF_ASSIST_PLAYERS;
extern BITVECT_DATA OFF_ASSIST_VNUM;
extern BITVECT_DATA OFF_STUN;
extern BITVECT_DATA OFF_CIRCLE;
extern BITVECT_DATA OFF_ONE_HALF_ATTACK;
extern BITVECT_DATA OFF_TWO_ATTACK;
extern BITVECT_DATA OFF_THREE_ATTACK;
extern BITVECT_DATA OFF_FOUR_ATTACK;
extern BITVECT_DATA OFF_WARDANCE;
extern BITVECT_DATA OFF_DAMAGE_REDUCTION;
extern BITVECT_DATA OFF_ZERO_ATTACK;
extern BITVECT_DATA OFF_OVERWHELMING_STRIKE;
extern BITVECT_DATA OFF_CLEAVE;
extern BITVECT_DATA OFF_VERTICAL_SLASH;
extern BITVECT_DATA OFF_SMITE_GOOD;
extern BITVECT_DATA OFF_SMITE_EVIL;
extern BITVECT_DATA OFF_SHIELD_BLOCK;
extern BITVECT_DATA OFF_MIGHTY_BLOW;
extern BITVECT_DATA OFF_POWER_STRIKE;
extern BITVECT_DATA OFF_CRITICAL_STRIKE;
extern BITVECT_DATA OFF_DISARMPROOF;
extern BITVECT_DATA OFF_FIRE_BREATH;
extern BITVECT_DATA OFF_FROST_BREATH;
extern BITVECT_DATA OFF_ACID_BREATH;
extern BITVECT_DATA OFF_LIGHTNING_BREATH;
extern BITVECT_DATA OFF_SLAM;
extern BITVECT_DATA OFF_SAP;

/* extra flags for objects */
extern BITVECT_DATA ITEM_GLOW;
extern BITVECT_DATA ITEM_BURN_PROOF;
extern BITVECT_DATA ITEM_DARK;
extern BITVECT_DATA ITEM_UNDESTRUCTABLE;
extern BITVECT_DATA ITEM_EVIL;
extern BITVECT_DATA ITEM_INVIS;
extern BITVECT_DATA ITEM_MAGIC;
extern BITVECT_DATA ITEM_NODROP;
extern BITVECT_DATA ITEM_BLESS;
extern BITVECT_DATA ITEM_ANTI_GOOD;
extern BITVECT_DATA ITEM_ANTI_EVIL;
extern BITVECT_DATA ITEM_ANTI_NEUTRAL;
extern BITVECT_DATA ITEM_NOREMOVE;
extern BITVECT_DATA ITEM_INVENTORY;
extern BITVECT_DATA ITEM_NOPURGE;
extern BITVECT_DATA ITEM_ROT_DEATH;
extern BITVECT_DATA ITEM_TRANSMUTE;
extern BITVECT_DATA ITEM_VIS_DEATH;;
extern BITVECT_DATA ITEM_HIGH_REPAIR_COST;
extern BITVECT_DATA ITEM_NOLOCATE;
extern BITVECT_DATA ITEM_MELT_DROP;
extern BITVECT_DATA ITEM_HAD_TIMER;
extern BITVECT_DATA ITEM_SELL_EXTRACT;
extern BITVECT_DATA ITEM_DRAGGABLE;
extern BITVECT_DATA ITEM_NOUNCURSE;
extern BITVECT_DATA ITEM_UNDEAD_INVIS;
extern BITVECT_DATA ITEM_NOMAGIC;
extern BITVECT_DATA ITEM_ANIMAL_INVIS;
extern BITVECT_DATA ITEM_NO_RENT;
extern BITVECT_DATA ITEM_HIGH_RENT;
extern BITVECT_DATA ITEM_COVER;
extern BITVECT_DATA ITEM_NEWBIE_FREE_RENT;
extern BITVECT_DATA ITEM_FREE_RENT;
extern BITVECT_DATA ITEM_DOUBLE_GRIP;
extern BITVECT_DATA ITEM_SACRED;
extern BITVECT_DATA ITEM_NOREPAIR;
extern BITVECT_DATA ITEM_RAREITEM;
extern BITVECT_DATA ITEM_COMPONENTONLY;

/*EXWEAR (wear_flags2) flags for objects */
extern BITVECT_DATA ITEM_NOMAG;
extern BITVECT_DATA ITEM_NOCLE;
extern BITVECT_DATA ITEM_NOTHI;
extern BITVECT_DATA ITEM_NOWAR;
extern BITVECT_DATA ITEM_NODWARF;
extern BITVECT_DATA ITEM_NOELF;
extern BITVECT_DATA ITEM_NOHUMAN;
extern BITVECT_DATA ITEM_NOHALFELF;
extern BITVECT_DATA ITEM_NOGNOM;
extern BITVECT_DATA ITEM_NOHALFLING;
extern BITVECT_DATA ITEM_NOPAL;
extern BITVECT_DATA ITEM_NODRUID;
extern BITVECT_DATA ITEM_NOBARBARIAN;
extern BITVECT_DATA ITEM_ONLYWAR;
extern BITVECT_DATA ITEM_ONLYMAG;
extern BITVECT_DATA ITEM_ONLYTHI;
extern BITVECT_DATA ITEM_ONLYDRU;
extern BITVECT_DATA ITEM_ONLYCLE;
extern BITVECT_DATA ITEM_ONLYPAL;
extern BITVECT_DATA ITEM_ONLYBAR;
extern BITVECT_DATA ITEM_ONLYHUMAN;
extern BITVECT_DATA ITEM_ONLYELF;
extern BITVECT_DATA ITEM_ONLYGNOM;
extern BITVECT_DATA ITEM_ONLYHALFLING;
extern BITVECT_DATA ITEM_ONLYDWARF;
extern BITVECT_DATA ITEM_ONLYHALFELF;
extern BITVECT_DATA ITEM_ONLYHALFORC;
extern BITVECT_DATA ITEM_NOHALFORC;
extern BITVECT_DATA ITEM_NOBARD;
extern BITVECT_DATA ITEM_ONLYBARD;
extern BITVECT_DATA ITEM_ONLYBLACKKNIGHT;
extern BITVECT_DATA ITEM_NOBLACKKNIGHT;
extern BITVECT_DATA ITEM_NOODRZUCANIE;
extern BITVECT_DATA ITEM_NOPRZEMIANY;
extern BITVECT_DATA ITEM_NOPRZYWOLANIA;
extern BITVECT_DATA ITEM_NOPOZNANIE;
extern BITVECT_DATA ITEM_NOZAUROCZENIA;
extern BITVECT_DATA ITEM_NOILUZJE;
extern BITVECT_DATA ITEM_NOINWOKACJE;
extern BITVECT_DATA ITEM_NONEKROMANCJA;
extern BITVECT_DATA ITEM_ONLYODRZUCANIE;
extern BITVECT_DATA ITEM_ONLYPRZEMIANY;
extern BITVECT_DATA ITEM_ONLYPRZYWOLANIA;
extern BITVECT_DATA ITEM_ONLYPOZNANIE;
extern BITVECT_DATA ITEM_ONLYZAUROCZENIA;
extern BITVECT_DATA ITEM_ONLYILUZJE;
extern BITVECT_DATA ITEM_ONLYINWOKACJE;
extern BITVECT_DATA ITEM_ONLYNEKROMANCJA;
extern BITVECT_DATA ITEM_ONLYGENERAL;
extern BITVECT_DATA ITEM_NOGENERAL;
extern BITVECT_DATA ITEM_NOSHAMAN;
extern BITVECT_DATA ITEM_ONLYSHA;

/* Trigger flags */
extern BITVECT_DATA TRIG_ACT;
extern BITVECT_DATA TRIG_BRIBE;
extern BITVECT_DATA TRIG_DEATH;
extern BITVECT_DATA TRIG_ENTRY;
extern BITVECT_DATA TRIG_FIGHT;
extern BITVECT_DATA TRIG_GIVE;
extern BITVECT_DATA TRIG_GREET;
extern BITVECT_DATA TRIG_GRALL;
extern BITVECT_DATA TRIG_KILL;
extern BITVECT_DATA TRIG_HPCNT;
extern BITVECT_DATA TRIG_RANDOM;
extern BITVECT_DATA TRIG_SPEECH;
extern BITVECT_DATA TRIG_EXIT;
extern BITVECT_DATA TRIG_EXALL;
extern BITVECT_DATA TRIG_DELAY;
extern BITVECT_DATA TRIG_SURR;
extern BITVECT_DATA TRIG_WEAR;
extern BITVECT_DATA TRIG_GET;
extern BITVECT_DATA TRIG_DROP;
extern BITVECT_DATA TRIG_PUT;
extern BITVECT_DATA TRIG_REMOVE;
extern BITVECT_DATA TRIG_REST;
extern BITVECT_DATA TRIG_SIT;
extern BITVECT_DATA TRIG_SLEEP;
extern BITVECT_DATA TRIG_INPUT;
extern BITVECT_DATA TRIG_CONNECT;
extern BITVECT_DATA TRIG_CONSUME;
extern BITVECT_DATA TRIG_TELL;
extern BITVECT_DATA TRIG_FLAGOFF;
extern BITVECT_DATA TRIG_LOCK;
extern BITVECT_DATA TRIG_USE;
extern BITVECT_DATA TRIG_UNLOCK;
extern BITVECT_DATA TRIG_OBJUNLOCK;
extern BITVECT_DATA TRIG_OPEN;
extern BITVECT_DATA TRIG_OBJLOCK;
extern BITVECT_DATA TRIG_CLOSE;
extern BITVECT_DATA TRIG_OBJCLOSE;
extern BITVECT_DATA TRIG_TICK_RANDOM;
extern BITVECT_DATA TRIG_OBJOPEN;
extern BITVECT_DATA TRIG_RESET;
extern BITVECT_DATA TRIG_UNEQUIP;
extern BITVECT_DATA TRIG_PREWEAR;
extern BITVECT_DATA TRIG_PREREMOVE;
extern BITVECT_DATA TRIG_TIME;
extern BITVECT_DATA TRIG_ASK;
extern BITVECT_DATA TRIG_MOUNT;
extern BITVECT_DATA TRIG_ONLOAD;
extern BITVECT_DATA TRIG_DISMOUNT;
extern BITVECT_DATA TRIG_STAND;
extern BITVECT_DATA TRIG_ONEHIT;
extern BITVECT_DATA TRIG_HIT;
extern BITVECT_DATA TRIG_PRECOMMAND;
extern BITVECT_DATA TRIG_HORN;
extern BITVECT_DATA TRIG_PREGET;
extern BITVECT_DATA TRIG_KNOCK;

/* room flagi */
extern BITVECT_DATA ROOM_DARK;
extern BITVECT_DATA ROOM_MEMCLE;
extern BITVECT_DATA ROOM_NO_MOB;
extern BITVECT_DATA ROOM_INDOORS;
extern BITVECT_DATA ROOM_MEMDRU;
extern BITVECT_DATA ROOM_MEMMAG;
extern BITVECT_DATA ROOM_NOMAGIC;
extern BITVECT_DATA ROOM_LIGHT;
extern BITVECT_DATA ROOM_MAGICDARK;
extern BITVECT_DATA ROOM_PRIVATE;
extern BITVECT_DATA ROOM_SAFE;
extern BITVECT_DATA ROOM_SOLITARY;
extern BITVECT_DATA ROOM_PET_SHOP;
extern BITVECT_DATA ROOM_NO_RECALL;
extern BITVECT_DATA ROOM_SLIPPERY_FLOOR;
extern BITVECT_DATA ROOM_BOUNTY_OFFICE;
extern BITVECT_DATA ROOM_HEROES_ONLY;
extern BITVECT_DATA ROOM_NEWBIES_ONLY;
extern BITVECT_DATA ROOM_LAW;
extern BITVECT_DATA ROOM_NOWHERE;
extern BITVECT_DATA ROOM_BANK;
extern BITVECT_DATA ROOM_NOHIDE;
extern BITVECT_DATA ROOM_INN;
extern BITVECT_DATA ROOM_HOARD;
extern BITVECT_DATA ROOM_TRAP;
extern BITVECT_DATA ROOM_NOZECHO;
extern BITVECT_DATA ROOM_RANDDESC;
extern BITVECT_DATA ROOM_NO_HERBS;
extern BITVECT_DATA ROOM_CONSECRATE;
extern BITVECT_DATA ROOM_DESECRATE;
extern BITVECT_DATA ROOM_MEMBARD;
extern BITVECT_DATA ROOM_NO_WEATHER;
extern BITVECT_DATA ROOM_WATCH_TOWER;
extern BITVECT_DATA ROOM_MEMSHA;
extern BITVECT_DATA ROOM_INVOKE;

/*
 * Sex.
 * Used in #MOBILES.
 */
#define SEX_NEUTRAL		0
#define SEX_MALE		1
#define SEX_FEMALE		2

/*
 * Rodzaje (w gramtycznym sensie).
 * U¿ywane w obiektach.
 */
#define GENDER_NONE				0
#define GENDER_NIJAKI			1
#define GENDER_MESKI			2
#define GENDER_ZENSKI			3
#define GENDER_MESKOOSOBOWY		4
#define GENDER_ZENSKOOSOBOWY	5

/* AC types */
#define AC_PIERCE		0
#define AC_BASH			1
#define AC_SLASH		2
#define AC_EXOTIC		3

/* dice */
#define DICE_NUMBER		0
#define DICE_TYPE		1
#define DICE_BONUS		2

/* size */
#define SIZE_TINY		0
#define SIZE_SMALL		1
#define SIZE_MEDIUM		2
#define SIZE_LARGE		3
#define SIZE_HUGE		4
#define SIZE_GIANT		5


/**
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 * vnums 1-5 - reserved for money items look at money.h
 */
#define OBJ_VNUM_RANDOM_ITEM      6//przedmioty randomowe
#define OBJ_VNUM_SPILLED_BLOOD    7//rozlana krew, np po charge
#define OBJ_VNUM_BODY_PART        9//czesc cial wycinane z po ich zgonach (body_parts)
#define OBJ_VNUM_CORPSE_NPC      10//cialo npc
#define OBJ_VNUM_CORPSE_PC       11//cialo pc
#define OBJ_VNUM_SEVERED_HEAD    12//odrabana glowa, np po charge
#define OBJ_VNUM_TORN_HEART      13//nie uzywane, serce
#define OBJ_VNUM_SLICED_ARM      14//odrabana reka, np po charge
#define OBJ_VNUM_SLICED_LEG      15//jw noga
#define OBJ_VNUM_GUTS            16//nie uzywane, flaki
#define OBJ_VNUM_BRAINS          17//mozg, ktory moze wyplynac np po charge
#define OBJ_VNUM_MUSHROOM        20//owocki z create food
#define OBJ_VNUM_LIGHT_BALL      21//kulka z light
#define OBJ_VNUM_SPRING          22//zrodelko z create spring
#define OBJ_VNUM_SPRING_HEAL   3390//zrodelko z create spring, wersja z woda uzdrawiajaca
#define OBJ_VNUM_DISC            23//dysk z floating disc
#define OBJ_VNUM_PORTAL          25//nie uzywane
#define OBJ_VNUM_WHISTLE       2116//gwizdek patrolmana ze spec_patrolman, raczej nie uzywane
#define OBJ_VNUM_ROSE          1001//roza? nie uzywane
#define OBJ_VNUM_GLAZ         10898//glaz, ktory mozna sobie summnac poprzez summon_malfunction
#define OBJ_VNUM_ASH          10899//popió³ do magicznych ksi±g

//bron, z ktorymi sie startuje
#define OBJ_VNUM_SCHOOL_MACE    3700
#define OBJ_VNUM_SCHOOL_DAGGER  3701
#define OBJ_VNUM_SCHOOL_SWORD   3702
#define OBJ_VNUM_SCHOOL_SPEAR   3717
#define OBJ_VNUM_SCHOOL_STAFF   3718
#define OBJ_VNUM_SCHOOL_AXE     3719
#define OBJ_VNUM_SCHOOL_FLAIL   3720
#define OBJ_VNUM_SCHOOL_WHIP    3721
#define OBJ_VNUM_SCHOOL_POLEARM 3722

//wdzianka z ktorymi sie startuje
#define OBJ_VNUM_SCHOOL_VEST    3703
#define OBJ_VNUM_SCHOOL_SHIELD  3704
#define OBJ_VNUM_SCHOOL_BANNER  3716

#define OBJ_VNUM_GOODBARRY         29//jagodka z goobarry
#define OBJ_VNUM_FLAMEBLADE     10872//mieczyk z flame blada
#define OBJ_VNUM_EYE_OF_VISION  10929//oczko z detectami z eye of vision
#define OBJ_VNUM_HOLY_HAMMER    10928//mlotek z holy weapons
#define OBJ_VNUM_HOLY_FLAIL     10914//cepik z holy weapons
#define OBJ_VNUM_HOLY_SWORD     10927//mieczyk z holy weapons
#define OBJ_VNUM_SKIN            3733//skora ze skilla skin
#define OBJ_VNUM_FIREFLIES       3325//swietliki z firefly swarm
#define OBJ_VNUM_FLOATING_SKULL  3326//czaszka z floating skull (swiatlo)
#define OBJ_VNUM_SHILLELAGH      3365//kostur ze spella shillelagh

//drzewka i ich owocki z druidzkiego create tree
#define OBJ_VNUM_TREE1         10919
#define OBJ_VNUM_TREE2         10920
#define OBJ_VNUM_TREE3         10921
#define OBJ_VNUM_TREE1_FRUIT   10922
#define OBJ_VNUM_TREE2_FRUIT   10923
#define OBJ_VNUM_TREE3_FRUIT   10924
#define OBJ_VNUM_TREE_HOT      10925
#define OBJ_VNUM_TREE_COLD     10926


/* sztyleciki z 'create shadow weapon' */
#define OBJ_VNUM_SHADOW_DAGGER                10915
#define OBJ_VNUM_SHADOW_SHORT_SWORD           10916
#define OBJ_VNUM_SHADOW_SHORT_SWORD_VORPAL    10917
#define OBJ_VNUM_SHADOW_SHORT_SWORD_DISPELLER 10918

//mlotek i pancerz ze spiritual armor/hammer/weapon
#define OBJ_VNUM_SPIRITUAL_HAMMER       10903
#define OBJ_VNUM_SPIRITUAL_ARMOR_HEAD   10904
#define OBJ_VNUM_SPIRITUAL_ARMOR_BODY   10905
#define OBJ_VNUM_SPIRITUAL_ARMOR_LEGS   10906
#define OBJ_VNUM_SPIRITUAL_ARMOR_ARMS   10907
#define OBJ_VNUM_SPIRITUAL_ARMOR_HANDS  10908
#define OBJ_VNUM_SPIRITUAL_ARMOR_FEET   10909
#define OBJ_VNUM_SPIRITUAL_STAFF        10911
#define OBJ_VNUM_SPIRITUAL_SHORTSWORD   10912
#define OBJ_VNUM_SPIRITUAL_FLAIL        10913
/* spell 'create symbol' */
#define OBJ_VNUM_CREATE_SYMBOL_EVIL     10859
#define OBJ_VNUM_CREATE_SYMBOL_NEUTRAL  10860
#define OBJ_VNUM_CREATE_SYMBOL_GOOD     10861
/* spell 'razorblade hands' */
#define OBJ_VNUM_NORMAL_RAZORBLADE     10851
#define OBJ_VNUM_SHARP_RAZORBLADE      10852
#define OBJ_VNUM_VORPAL_RAZORBLADE     10853
#define OBJ_VNUM_FALSE_RAZORBLADE      10854
/* spell 'beast claws' */
#define OBJ_VNUM_NORMAL_CLAW            3320
#define OBJ_VNUM_SHARP_CLAW             3321
#define OBJ_VNUM_VORPAL_CLAW            3322
#define OBJ_VNUM_FALSE_CLAW             3384
/* spell 'spiritual light' */
#define OBJ_VNUM_EVIL_BALL              10862
#define OBJ_VNUM_NEUTRAL_BALL           10863
#define OBJ_VNUM_GOOD_BALL              10864
//z singing ruff
#define OBJ_VNUM_RUFF1  10881
#define OBJ_VNUM_RUFF2  10882
#define OBJ_VNUM_RUFF3  10883
#define OBJ_VNUM_RUFF4  10884
#define OBJ_VNUM_RUFF5  10885
#define OBJ_VNUM_RUFF6  10886
#define OBJ_VNUM_RUFF31 10887
#define OBJ_VNUM_RUFF61 10888
//z light nova
#define OBJ_VNUM_LIGHT_NOVA_BALL OBJ_VNUM_LIGHT_BALL // hack

//rellik: mining, bry³ki
#define OBJ_VNUM_PIECE_COAL          3111
#define OBJ_VNUM_PIECE_IRON          3112
#define OBJ_VNUM_PIECE_COPPER        3113
#define OBJ_VNUM_PIECE_SILVER        3114
#define OBJ_VNUM_PIECE_GOLD          3115
#define OBJ_VNUM_PIECE_MITHRIL       3116
#define OBJ_VNUM_PIECE_ADAMANTYT     3117
#define OBJ_VNUM_PIECE_BIG_COAL      3118
#define OBJ_VNUM_PIECE_BIG_IRON      3119
#define OBJ_VNUM_PIECE_BIG_COPPER    3120
#define OBJ_VNUM_PIECE_BIG_SILVER    3121
#define OBJ_VNUM_PIECE_BIG_GOLD      3122
#define OBJ_VNUM_PIECE_BIG_MITHRIL   3123
#define OBJ_VNUM_PIECE_BIG_ADAMANTYT 3124

/* extra przedmioty do mining - szansa 1/1000 */
#define OBJ_VNUM_MINE_IMAGE            6
#define OBJ_VNUM_MINE_SUPERB_PIECE 43677


/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT               1
#define ITEM_SCROLL              2
#define ITEM_WAND                3
#define ITEM_STAFF               4
#define ITEM_WEAPON              5
//                               6
//                               7
#define ITEM_TREASURE            8
#define ITEM_ARMOR               9
#define ITEM_POTION             10
#define ITEM_CLOTHING           11
#define ITEM_FURNITURE          12
#define ITEM_TRASH              13
#define ITEM_CONTAINER          15
#define ITEM_DRINK_CON          17
#define ITEM_KEY                18
#define ITEM_FOOD               19
#define ITEM_MONEY              20
#define ITEM_BOAT               22
#define ITEM_CORPSE_NPC         23
#define ITEM_CORPSE_PC          24
#define ITEM_FOUNTAIN           25
#define ITEM_PILL               26
#define ITEM_PROTECT            27
#define ITEM_MAP                28
#define ITEM_PORTAL             29
//                              30
#define ITEM_ROOM_KEY           31
#define ITEM_GEM                32
#define ITEM_JEWELRY            33
#define ITEM_JUKEBOX            34
#define ITEM_BOARD              35
#define ITEM_PIECE              36
#define ITEM_SPELLBOOK          37
#define ITEM_SPELLITEM          38
#define ITEM_ENVENOMER          39
#define ITEM_BANDAGE            40
#define ITEM_TURN               41
#define ITEM_HERB               42
#define ITEM_SKIN               43
#define ITEM_HORN               44
#define ITEM_MUSICAL_INSTRUMENT 45
#define ITEM_SHIELD             46
#define ITEM_TROPHY             47
#define ITEM_PIPE               48
#define ITEM_WEED               49
#define ITEM_NOTEPAPER          50
#define ITEM_MONSTER_PART       51
#define ITEM_TOOL               52 //rellik: mining
#define ITEM_TOTEM              53 //Drake: Dodatki dla szamana


//rellik: mining rawmaterials
#define RAW_COAL        1
#define RAW_IRON        2
#define RAW_COPPER      3
#define RAW_SILVER      4
#define RAW_GOLD        5
#define RAW_MITHRIL     6
#define RAW_ADAMANTYT   7
#define RAW_GEMS        8

#define MAX_RAWMATERIAL 8


/* start food state - by Fuyara */
#define FOOD_STATE_EDIBLE 0      // gotowe do spo¿ycia
#define FOOD_STATE_RAW    1      // surowe
#define FOOD_STATE_COOKED 2      // gotowane
#define FOOD_STATE_BAKED  3      // pieczone
#define FOOD_STATE_DRIED  4      // suszone
#define FOOD_STATE_SMOKED 5      // wêdzone
#define FOOD_STATE_FRIED  6      // sma¿one
/* end food state */


/* start food state - by Fuyara */
#define FOOD_TYPE_MIXED 0      // mieszane
#define FOOD_TYPE_MEAT  1      // miêso
#define FOOD_TYPE_PLANT 2      // ro¶lina

/* end food state */

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_TAKE      		(A)
#define ITEM_WEAR_FINGER	(B)
#define ITEM_WEAR_NECK      (C)
#define ITEM_WEAR_BODY      (D)
#define ITEM_WEAR_HEAD      (E)
#define ITEM_WEAR_LEGS      (F)
#define ITEM_WEAR_FEET      (G)
#define ITEM_WEAR_HANDS     (H)
#define ITEM_WEAR_ARMS      (I)
#define ITEM_WEAR_SHIELD	(J)
#define ITEM_WEAR_ABOUT     (K)
#define ITEM_WEAR_WAIST     (L)
#define ITEM_WEAR_WRIST     (M)
#define ITEM_WIELD      	(N)
#define ITEM_HOLD			(O)
#define ITEM_WEAR_FLOAT     (Q)
#define ITEM_WEAR_LIGHT     (R)
#define ITEM_INSTRUMENT     (S)
#define ITEM_WEAR_EAR		(T)
#define ITEM_WIELDSECOND	(Z)

/* weapon class */
#define WEAPON_EXOTIC		0
#define WEAPON_SWORD		1
#define WEAPON_DAGGER		2
#define WEAPON_SPEAR		3
#define WEAPON_MACE			4
#define WEAPON_AXE			5
#define WEAPON_FLAIL		6
#define WEAPON_WHIP			7
#define WEAPON_POLEARM		8
#define WEAPON_STAFF		9
//#define WEAPON_TWOHANDER	10
#define WEAPON_SHORTSWORD	11
#define WEAPON_CLAWS 12

//rellik: mining, typy narzêdzi
#define TOOL_EXOTIC  0
#define TOOL_HAMMER  1
#define TOOL_ANVIL   2
#define TOOL_SAW     3
#define TOOL_PICKAXE 4

/* wand type */
#define WAND_NONE			0
#define WAND_DAM_PERCENT	1
#define WAND_DAM_VALUE		2
#define WAND_POSTWAIT		3
#define WAND_MEM_PERCENT	4
#define WAND_MEM_VALUE		5
#define WAND_LEV_PERCENT	6
#define WAND_LEV_VALUE		7
#define WAND_SAVE_PIERCE	8

/* wand special */
#define	WAND_EXTRA_ALIGN_GOOD      (A)
#define	WAND_EXTRA_ALIGN_NEUTRAL   (B)
#define WAND_EXTRA_ALIGN_EVIL      (C)
#define WAND_EXTRA_CLASS_MAG       (D)
#define WAND_EXTRA_CLASS_CLERIC    (E)
#define WAND_EXTRA_CLASS_DRUID     (F)
#define WAND_EXTRA_SCHOOL_0        (G)
#define	WAND_EXTRA_SCHOOL_1        (H)
#define	WAND_EXTRA_SCHOOL_2        (I)
#define	WAND_EXTRA_SCHOOL_3        (J)
#define	WAND_EXTRA_SCHOOL_4        (K)
#define	WAND_EXTRA_SCHOOL_5        (L)
#define	WAND_EXTRA_SCHOOL_6        (M)
#define	WAND_EXTRA_SCHOOL_7        (N)

/* weapon types */
#define WEAPON_FLAMING           (A) // wali od ognia
#define WEAPON_FROST             (B) // wali od zimna
#define WEAPON_VAMPIRIC          (C) // wysysa hapki
#define WEAPON_SHARP             (D) // ostre, niewielkie premie do dama, moze wrzucic bleeding wound
#define WEAPON_VORPAL            (E) // bardzo ostre, przebija stona, znaczace premie do dama, moze wrzucic bleeding wound
#define WEAPON_TWO_HANDS         (F) // trzyma sie to w dwoch lapkach
#define WEAPON_SHOCKING          (G) // wali od elektrycznosci
#define WEAPON_POISON            (H) // jebniety dostaje poisona
#define WEAPON_DISPEL            (I) // teoretycznie castuje przy ciosie dispel magic, ale to chyba nie dziala
#define WEAPON_RANDOM_MAGIC_PLUS (J) // randomowo dodaje broni ummagicznienie na poziomie 0-3
#define WEAPON_PRIMARY           (K) // tylko w wiodaca reka
#define WEAPON_TOXIC             (L) // wali od kwasu
#define WEAPON_SACRED            (M) // taka swieta bron - dodatkowe damy w undeady
#define WEAPON_RESONANT          (N) // wali od dzwieku
#define WEAPON_INJURIOUS         (O) // wrzuca flage bleeding wound na 25% niezaleznie od conow
#define WEAPON_KEEN              (Q) // Wywarzone ostrze(lub ogolnie - bron) ktorym latwiej jest zadac cios krytyczny
#define WEAPON_THUNDERING        (R) // W broni zakleta jest esencja sily, udany cios krytyczny zada jeszcze wieksze obrazenia
#define WEAPON_UNBALANCED        (S) // bron przekleta ze stala szansa na nie trafienie
#define WEAPON_WICKED            (T) // bron przekleta - wysysa punkty zycia w momencie zadania ciosu z trzymajacego bron
#define WEAPON_HEARTSEEKER       (U) // bron samodzielnie dazy do serca przeciwnika, bonus dla BS'a oraz hitroll

/* gate flags */
#define GATE_NORMAL_EXIT   (A)
#define GATE_NOCURSE      (B)
#define GATE_GOWITH      (C)
#define GATE_BUGGY      (D)
#define GATE_RANDOM      (E)

/* furniture flags */
#define STAND_AT      (A)
#define STAND_ON      (B)
#define STAND_IN      (C)
#define SIT_AT         (D)
#define SIT_ON         (E)
#define SIT_IN         (F)
#define REST_AT         (G)
#define REST_ON         (H)
#define REST_IN         (I)
#define SLEEP_AT      (J)
#define SLEEP_ON      (K)
#define SLEEP_IN      (L)
#define PUT_AT         (M)
#define PUT_ON         (N)
#define PUT_IN         (O)
#define PUT_INSIDE      (P)

/*corpse flags*/
#define NO_SKIN         (A)
#define STATIC_WEIGHT   (B)
#define NO_ROT          (C)
#define SLOW_ROT        (D)

/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE            0
#define APPLY_STR            1
#define APPLY_DEX            2
#define APPLY_INT            3
#define APPLY_WIS            4
#define APPLY_CON            5
#define APPLY_SEX            6
#define APPLY_CLASS            7
#define APPLY_LEVEL            8
#define APPLY_AGE            9
#define APPLY_HEIGHT           10
#define APPLY_WEIGHT           11
#define APPLY_RESIST           12
#define APPLY_HIT           13
#define APPLY_MOVE           14
#define APPLY_GOLD           15
#define APPLY_EXP           16
#define APPLY_AC           17
#define APPLY_HITROLL           18
#define APPLY_DAMROLL           19

#define APPLY_SAVING_DEATH        20
#define APPLY_SAVING_ROD        21
#define APPLY_SAVING_PETRI        22
#define APPLY_SAVING_BREATH        23
#define APPLY_SAVING_SPELL        24

#define APPLY_SPELL_AFFECT        25
#define APPLY_SKILL           26
#define APPLY_MEMMING           27
#define APPLY_LANG           28
#define APPLY_CHA           29
#define APPLY_LUC           30

/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE      (A)
#define CONT_PICKPROOF      (B)
#define CONT_CLOSED         (C)
#define CONT_LOCKED         (D)
#define CONT_PUT_ON         (E)
#define CONT_BROKEN         (F)
#define CONT_HARD           (G)
#define CONT_EASY           (H)
#define CONT_EATKEY         (I)
#define CONT_SINGLE_OBJ     (J)
#define CONT_ONLY_VNUM      (K)
#define CONT_COMP    	    (L)

/*
 * Values for lights (value[1]).
 * Used in #OBJECTS.
 * Jak mozna zamocowac swiatlo zeby oswietlalo lokacje
 * bity kolejno 1 - lie, 2 - stick, 3 - hang
 */
#define LIGHT_NONE					0
#define	LIGHT_LIE					(A)
#define LIGHT_STICK					(B)
#define LIGHT_HANG					(C)

/*
 * Values for lights (value[3]).
 * Used in #OBJECTS.
 * Jaki wplyw ma pogoda na swiatlo kiedy jest zamocowane
 */

#define VUL_NONE			0
#define VUL_RAIN			1
#define VUL_WIND			2
/*
* value[4] dla swiatel uzywane jako licznik swiecenia podczas gdy swiatlo jest zamocowane
*/


/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_LIMBO            2
#define ROOM_VNUM_TEMPLE        599

/*
 * numery lokacji do druidzkich czarów przemieszczania siê
 */

#define ROOM_VNUM_DRUID_VANION    1678 // druidzki kr±g na po³udniowy wschód od carrallack

/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH           0
#define DIR_EAST            1
#define DIR_SOUTH           2
#define DIR_WEST            3
#define DIR_UP              4
#define DIR_DOWN            5



/*
 * Exit flags.
 * Used in #ROOMS.
 */
#define EX_ISDOOR            (A)
#define EX_CLOSED            (B)
#define	EX_LOCKED            (C)
#define	EX_HIDDEN            (D)
#define	EX_SECRET            (E)
#define EX_PICKPROOF         (F)
#define EX_NOPASS            (G)
#define EX_EASY              (H)
#define EX_HARD              (I)
#define EX_INFURIATING       (J)
#define EX_NOCLOSE           (K)
#define EX_NOLOCK            (L)
#define EX_RANDOM            (M)
#define EX_NO_MOB            (N)
#define EX_NO_MOUNT          (O)
#define EX_TRAP              (P)
#define EX_NO_FLEE           (R)
#define EX_BROKEN            (Q)
#define EX_EATKEY            (S)
#define EX_WALL_OF_MIST      (T)

#define SECT_NOMEM      (A)
#define SECT_WATER      (B)
#define SECT_NOREST     (C)
#define SECT_AIR        (D)
#define SECT_WATERN     (E)
#define SECT_LIGHT      (F)
#define SECT_NOWEATHER  (G)
#define SECT_THIRST     (H)
#define SECT_CANDRINK   (I)
#define SECT_UNDERWATER (J)
#define SECT_DRUID      (K)
#define SECT_HOT        (L)
#define SECT_COLD       (M)

// maksymalna ilo¶æ sectorów
#define SECT_MAX      50

typedef struct sector_data SECT_DATA;

struct sector_data
{
 char *   name;
 sh_int   mv_loss;
 sh_int   heal_bonus;
 sh_int   mem_bonus;
 sh_int memdru_bonus;
 sh_int min_dam;
 sh_int max_dam;
 sh_int   wait;
 int   flag;
 //tutaj dwie rzeczy do ziolek
 sh_int  herb_reset_time;     //czyli co ile zmieniaja sie herby w roomie
 sh_int  max_herb;            //ile rodzajow max w roomie
};

struct herb_in_room_data                //ZROBIC DO TEGO OBSLUGE PAMIECI
{
       sh_int           herb;  //czyli numer z herb_type_table
       sh_int           herb_status;    //czyli 0 nieruszone, 1 zerwane, 2 ...
       HERB_IN_ROOM_DATA               *next;
       bool valid;
};


/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_TRY           -2
#define WEAR_NONE           -1
#define WEAR_LIGHT            0
#define WEAR_FINGER_L            1
#define WEAR_FINGER_R            2
#define WEAR_NECK_1            3
#define WEAR_NECK_2            4
#define WEAR_BODY            5
#define WEAR_HEAD            6
#define WEAR_LEGS            7
#define WEAR_FEET            8
#define WEAR_HANDS            9
#define WEAR_ARMS           10
#define WEAR_SHIELD           11
#define WEAR_ABOUT           12
#define WEAR_WAIST           13
#define WEAR_WRIST_L           14
#define WEAR_WRIST_R           15
#define WEAR_WIELD           16
#define WEAR_HOLD           17
#define WEAR_FLOAT           18
#define WEAR_SECOND           19
#define WEAR_INSTRUMENT           20
#define WEAR_EAR_L           21
#define WEAR_EAR_R           22
#define MAX_WEAR           25



/***************************************************************************
 *                                                                         *
 *                   VALUES OF INTEREST TO AREA BUILDERS                   *
 *                   (End of this section ... stop here)                   *
 *                                                                         *
 ***************************************************************************/

/*
 * Conditions.
 */
#define COND_DRUNK            0
#define COND_FULL            1
#define COND_THIRST            2
#define COND_HUNGER            3
#define COND_SLEEPY            4

/**
 * Eat limits
 */
#define EAT_AMOUNT	6
#define EAT_FULL	21

/**
 * Drink limits
 */
#define DRINK_AMOUNT	4
#define DRINK_FULL	20
#define DRINK_PART	12
#define DRINK_FULL_FOUNTAIN	16


/**
 * Drunk limits
 */
#define DRUNK_FULL	10
/*
 * Positions.
 */
#define POS_DEAD     0
#define POS_MORTAL   1
#define POS_INCAP    2
#define POS_STUNNED  3
#define POS_SLEEPING 4
#define POS_RESTING  5
#define POS_SITTING  6
#define POS_FIGHTING 7
#define POS_STANDING 8

//jako flagi, do triggerow
#define FP_DEAD     (A)
#define FP_MORTAL   (B)
#define FP_INCAP    (C)
#define FP_STUNNED  (D)
#define FP_SLEEPING (E)
#define FP_RESTING  (F)
#define FP_SITTING  (G)
#define FP_FIGHTING (H)
#define FP_STANDING (I)
#define FP_ALL      (A|B|C|D|E|F|G|H|I)
#define FP_ACTIVE   (F|G|I)

/* RT comm flags -- may be used on both mobs and chars */
#define COMM_QUIET       (A)
#define COMM_DEAF        (B)
#define COMM_NOWIZ       (C)
#define COMM_NONEWBIE    (D) //flaga dla kogo¶ kto nie chce widzieæ tego kana³u
#define COMM_NEWBIE      (E) //flaga dla kogo¶ komu zabroniono u¿ywanie tego kana³u
#define COMM_LFG         (F)
#define COMM_NOMUSIC     (G)
#define COMM_SHOW_ON_WHO (H)
#define COMM_SHOUTSOFF   (J)
#define COMM_HINT        (K)

/* display flags */
#define COMM_COMPACT      (L)
#define COMM_BRIEF        (M)
#define COMM_PROMPT       (N)
#define COMM_COMBINE      (O)
#define COMM_TELNET_GA    (P)
#define COMM_SHOW_AFFECTS (Q)
/*#define COMM_NOGRATS      (R)*/

/* penalties */
#define COMM_NOEMOTE      (T)
#define COMM_NOSHOUT      (U)
#define COMM_NOTELL       (V)
#define COMM_NOCHANNELS   (W)
#define COMM_SNOOP_PROOF  (Y)
#define COMM_AFK          (Z)
/*uciszanie*/

/* WIZnet flags */
#define WIZ_ON            (A)
#define WIZ_TICKS         (B)
#define WIZ_LOGINS         (C)
#define WIZ_SITES         (D)
#define WIZ_LINKS         (E)
#define WIZ_DEATHS         (F)
#define WIZ_RESETS         (G)
#define WIZ_MOBDEATHS      (H)
#define WIZ_FLAGS         (I)
#define WIZ_PENALTIES      (J)
#define WIZ_SACCING         (K)
#define WIZ_LEVELS         (L)
#define WIZ_SECURE         (M)
#define WIZ_SWITCHES      (N)
#define WIZ_SNOOPS         (O)
#define WIZ_RESTORE         (P)
#define WIZ_LOAD         (Q)
#define WIZ_NEWBIE         (R)
#define WIZ_PREFIX         (S)
#define WIZ_SPAM         (T)
#define WIZ_ARTEFACT      (U)
#define WIZ_ARTEFACTLOAD   (V)
#define WIZ_MXP   (bb)
#define WIZ_COMM  (cc)

/* wiz config */
#define WIZCFG_SEXCONV     (A)
#define WIZCFG_LIMITLEVEL  (B)
#define WIZCFG_WIZSPEECH   (C)
#define WIZCFG_SHOWVNUMS   (D)
#define WIZCFG_QUIETMODE   (E)
#define WIZCFG_SHOWYELLS   (F)
#define WIZCFG_COLOURCONV  (G)

/* wiz_groups */
#define W1 (C) /* nowy     */
#define W2 (D) /* tworca   */
#define W3 (E) /* straznik */
#define W4 (F) /* regent   */
#define W5 (G) /* sedzia   */
#define W6 (H) /* lord     */

/* opcje dla close_socket */
#define CS_NORECURSE    (A)

/*
 * Prototype for a mob.
 * This is the in-memory version of #MOBILES.
 */
struct   mob_index_data
{
	MOB_INDEX_DATA *		next;
	SPEC_FUN *				spec_fun;
	SHOP_DATA *				pShop;
	BANK_DATA *				pBank;
	REPAIR_DATA *			pRepair;
	PROG_LIST *				progs;
	AREA_DATA *				area;
	unsigned int			vnum;
	sh_int					group;
	bool					new_format;
	sh_int					count;
	sh_int					killed;
	char*					player_name;
	char*					name2;
	char*					name3;
	char*					name4;
	char*					name5;
	char*					name6;
	char *					short_descr;
	char *					long_descr;
	char *					description;
	long					act[MAX_VECT_BANK];
	long					affected_by[MAX_VECT_BANK];
	sh_int					alignment;
	sh_int					level;
	sh_int					hitroll;
	sh_int					hit[3];
	sh_int					damage[3];
	sh_int					weapon_damage_bonus;
	sh_int					magical_damage;
	int						attack_flags;
	sh_int					ac[4];
	sh_int					stats[MAX_STATS];
	//sh_int					newstats[MAX_STATS];
	sh_int					dam_type;
	long					off_flags[MAX_VECT_BANK];
	char					resists[MAX_RESIST];
        int					healing_from[MAX_RESIST]; // procentowo, np. przy 20% obra¼eä z ognia leci jako leczenie
	sh_int					start_pos;
	sh_int					default_pos;
	sh_int					sex;
	sh_int					race;
	long					wealth;
	long					form;
	long					parts;
	sh_int					size;
	char *					material;
	long					prog_flags[MAX_VECT_BANK];
	int						languages;
	int						speaking;
	sh_int					spells[16];
	int						corpse_vnum;
	int						skin_multiplier;
	char *					comments;
	int						exp_multiplier;
};



/* memory settings */
#define MEM_HATE   0
#define MEM_FEAR   1

/* memory for mobs */
struct mem_data
{
    MEM_DATA    *next;
    bool   valid;
    CHAR_DATA   *who;
    int    reaction;
};

/*learn system*/

struct learn_list
{
 sh_int sn;       /* gsn skilla/spella      */
 sh_int max;      /* do ilu moze wytrenowac (skille tylko)  */
 sh_int min;      /* od ilu mozna cwiczyc   (skille tylko)  */
 sh_int chance;   /* procentowo szansa na nauczenie sie spella (spelle tylko ) */
 sh_int payment;  /* procent op³aty, 0 - nie pobiera op³at */
 sh_int pay_from; /* od jakiego poziomu skilla maj± byæ op³aty */
 LEARN_LIST *next;
 bool valid;
};

struct learn_data
{
 unsigned int vnum;       /* numer moba nauczyciela */
 bool room;
 LEARN_DATA *next;  /* nastepny, do lsty      */
 LEARN_LIST *list;  /*list skilli/spelli      */
 bool valid;
};


/**************/
/*artefact*/
struct artefact_data
{
   unsigned int avnum;
   ush_int count;/*aktualna liczba*/
   ush_int max_count;/*maksymalna liczba*/
   ush_int max_day;/*maksymalnie dni przetrzymywania*/

   ARTEFACT_OWNER *first_owner;/*wsk. ma podliste*/
        ARTEFACT_LOADER *loader;//wsk. na opdliste loaderow
   ARTEFACT_DATA *next;
        bool valid;

};

struct artefact_owner
{
    char *owner;/*wlasciciel*/
    ush_int status;/* 0 - poza gra, 1 - w grze */
    time_t last_logoff;/*ostatnio na mudzie*/
    /*tego nie zapisuje do pliku, bedzie sie aktualizowac przy restarcie*/

    ARTEFACT_OWNER *next;
    bool valid;

};

struct artefact_loader
{
    ush_int type;//0 - room, 1 - obj, 2 - mob
    unsigned int vnum;//vnum powyzszego
    ush_int probdown;//prawdopodobienstwo dolne
    ush_int probup;//gorne, sumarycznie down/up czyli np: 5/8
    ARTEFACT_LOADER *next;
    bool valid;

};

/*boards*/
#define ED_MSG      8
#define ED_MSG1      10
#define ED_PREWAIT   11 /*do prewaitow*/
#define ED_MAP      12 /*wlasne mapki*/

struct board_data
{
    bool               valid;
    unsigned int       vnum;
    MESSAGE_DATA    *  message;
    BOARD_DATA      *  next;
};


struct message_data
{
    bool       valid;
    time_t          datetime;
    char         *  author;
    char         *  title;
    char         *  message;
    int             lang;
    unsigned int    vnum; /*tylko do edycji*/
    MESSAGE_DATA *  next;
};

// Brohacz: bounty
struct bounty_data
{
    bool        valid;
    char	* name;
    int		value;
    BOUNTY_DATA	* next;
};

struct pflag_data
{
    bool       valid;
    char         *  id;
    int        duration;
    int        target;
    PFLAG_DATA   *  next;
};

struct pre_wait
{
     bool        valid;
     sh_int        duration;
     char      * spell_name;
     int         sn; /*dla szybkosci*/
     PFLAG_DATA      * list;
     PRE_WAIT      * next;
};

struct pwait_char
{
     bool        valid;
     sh_int        wait;
     int        target;
     int                  sn;
     void       * vo;
     char      * spell_name;
     PWAIT_CHAR      * next;
};

#define TYPE_TICK_RANDOM   0
#define TYPE_PFLAG      1
#define TYPE_PULSE_RANDOM   2
#define TYPE_TRIG_DELAY      3
#define TYPE_TIME      4

struct room_update_type
{
	ROOM_UPDATE_TYPE *		next;
	ROOM_INDEX_DATA *		room;
	int						type;
	bool					active;
};

typedef struct  room_update_list   ROOM_UPDATE_LIST;

struct room_update_list
{
     ROOM_UPDATE_TYPE * tick_update_list;
     ROOM_UPDATE_TYPE * pulse_update_list;
};

 /*
 0 - rzuca na siebie
 1 - na kogos
 2 - zabija siebie
 3 - zabija kogos
 4 - immune sam
 5 - immune ktos
 */

typedef struct spell_msg SPELL_MSG;
struct spell_msg
{
 char *      name;
 sh_int      sn;
 char *      caster[6];
 char *      victim[6];
 char *      around[6];
 SPELL_MSG *   next;
};



void    print_char   args( (CHAR_DATA *ch, char *fmt, ...) );


#define HEALING      0
#define DEFENCE      1
#define FRAG      2
#define ANTITANK   3
#define ANTICASTER   4

struct spell_data
{
    char *   name;
    sh_int   sn;
    int      rating;
    int      extra;
};

struct spirit_data
{
    bool        valid;
    CHAR_DATA	* ch;
    OBJ_DATA	* corpse;
    SPIRIT_DATA	* next;
};
/*
typedef   struct   lcnv_data      LCNV_DATA;
typedef   struct   lang_data      LANG_DATA;


struct lcnv_data
{
    LCNV_DATA *      next;
    char *      old;
    int         olen;
    char *      new;
    int         nlen;
};

struct lang_data
{
    LANG_DATA *      next;
    char *      name;
    LCNV_DATA *      first_precnv;
    char *      alphabet;
    LCNV_DATA *      first_cnv;
};


struct lang_type
{
 char *name;
 int bit;
};

extern struct lang_type lang_table[MAX_LANG];
char *translate(int percent, const char *in, const char *name);
int knows_language( CHAR_DATA *ch, int language, CHAR_DATA *cch );
int get_langnum( char *flag );
int get_speaking( int speaking );*/


/* EVENTS */

#define EVENT_NONE      0
#define EVENT_SAVE_SYSDATA   1
#define EVENT_SAVE_CORPSES      2
#define EVENT_HIT_GAIN      3
#define EVENT_MANA_GAIN      4
#define EVENT_MOVE_GAIN      5
#define EVENT_RESET_AREA   6
#define EVENT_MEMORIZE_UPDATE   7
#define EVENT_WEATHER_UPDATE   8
#define EVENT_MOBILE_UPDATE   9
#define EVENT_OBJECT_UPDATE   10
#define EVENT_SLOW_CHAR_UPDATE   11
#define EVENT_CHAR_UPDATE   12
#define EVENT_LIST_UPDATE       13
#define EVENT_VIOLENCE_UPDATE   14
#define EVENT_AGGR_UPDATE   15
#define EVENT_HUNGER      16
#define EVENT_PULSE_REGENERATE   17
#define EVENT_CAST_TICK      18
#define EVENT_ROOM_AFF_ADD   19
#define EVENT_ROOM_AFF_REMOVE   20
#define EVENT_ROOM_PROC      21
#define EVENT_MOB_PROC      22
#define EVENT_OBJ_PROC      23
#define EVENT_ZONE_PROC      24
#define EVENT_QUEST_PROC   25
#define EVENT_SHIP_PROC      26
#define EVENT_ROOM_DMG_SPELL    27
#define EVENT_PERSON_DMG_SPELL   28
#define EVENT_SPELL_CAST        29
#define EVENT_CAMP      30
#define EVENT_DELAYED_COMMAND   31
#define EVENT_EXTRACT_CHAR      32
#define EVENT_TROLL_POWER_REGEN 33
#define EVENT_OBJ_DECAY      34
#define EVENT_SKILL_RESET   35
#define EVENT_SHIP_MOVE      36
#define EVENT_FIRE_PLANE   37
#define EVENT_AUTOSAVE      38
#define EVENT_TRACK_DECAY   39
#define EVENT_SPELL_SCRIBING   40
#define EVENT_COMBAT      42
#define EVENT_SONG      43
#define EVENT_BARD_EFFECTDECAY   44
#define EVENT_STUNNED      45
#define EVENT_KO      46
#define EVENT_BRAIN_DRAIN   47
#define EVENT_CONFLAGURATION    48
#define EVENT_BERSERK      49
#define EVENT_MOB_HUNT      50
#define EVENT_UNDERWATER   51
#define EVENT_SWIMMING      52
#define EVENT_ROOM_UPDATE       53
#define EVENT_AREA_UPDATE       54
#define EVENT_OBJECT_SPECIAL    55
#define EVENT_ACID_ARROW        56
#define EVENT_DARKNESS_END      57
#define EVENT_IMMOLATE          58
#define EVENT_CREEPING_DOOM     59
#define EVENT_INCENDIARY_CLOUD  60
#define EVENT_CALL_LIGHTNING   61
#define EVENT_LAVA            62
#define EVENT_CONSECRATE_END   63
#define EVENT_DESECRATE_END      64
#define EVENT_SUMMON_LESSER_METEOR   65
#define EVENT_EXTRACT_SOUL      66
#define EVENT_SUMMON_GREATER_METEOR     67
#define EVENT_TREE_FRUITS     68
#define EVENT_TREE_EXTRACT     69
#define EVENT_SLIPPERY_FLOOR_END     70
#define EVENT_WALL_OF_MIST_END     71
#define EVENT_RUFF     72
#define EVENT_BURN        73
#define EVENT_BLADE        74
#define EVENT_LIGHT_NOVA     75

/*ACTIONS*/

#define ACTION_PRINT         0
#define ACTION_FUNCTION      1
#define ACTION_WAIT          2
#define ACTION_ACT           3
#define ACTION_OBJ_TO_ROOM   4
#define ACTION_OBJ_TO_CHAR	 5
#define ACTION_CHAR_TO_ROOM   6

/* timer */
struct event2_data
{
   int       delay;
   CHAR_DATA   *to;
   int      action;
   DO_FUN   *do_fun;
   char   *args[5];
   void   *argv[5];
   int     argi[5];
   EVENT2_DATA *next;
   bool      valid;
};
extern EVENT2_DATA *events2;


/****   EVENTS     **/
struct event_data
{
    EVENT_DATA * next;
    int      type;
    void *   arg1;
    void *   arg2;
    int      time;
    int      var;
    bool        deleted;
};
extern EVENT_DATA *event_first;

/*
 * Custom character list
 */

typedef struct char_lst CHAR_LST;

struct char_lst
{
	CHAR_DATA * ch;
	CHAR_LST * next;
	CHAR_LST * previous;
};

/*
 * One character (PC or NPC).
 */
struct char_data
{
	CHAR_DATA *				next;
	CHAR_DATA *				next_in_room;
	CHAR_DATA *				next_mind_in_room;
	CHAR_DATA *				master;
	CHAR_DATA *				leader;
	CHAR_DATA *				fighting;
	CHAR_DATA *				reply;
	CHAR_DATA *				prog_target;
	CHAR_DATA *				mount;
	CHAR_DATA *				mounting;
	PWAIT_CHAR *			wait_char;
	MSPELL_DATA *			memspell;
    char				    memspell_cache[MAX_SKILL];
	MSPELL_DATA *			memming;
	MEMSET_DATA *           memset;
	int						count_memspell;
	sh_int					mem_mods[9];
	CHAR_DATA *				hunting;
	MEM_DATA *				memory;
	SPEC_FUN *				spec_fun;
	MOB_INDEX_DATA *		pIndexData;
	DESCRIPTOR_DATA *		desc;
	AFFECT_DATA *			affected;
	NOTE_DATA *				pnote;
	OBJ_DATA *				carrying;
	OBJ_DATA *				on;
	OBJ_DATA *				hoard; //przechowywanie przedmiotow
	ROOM_INDEX_DATA *		in_room;
	ROOM_INDEX_DATA *		was_in_room;
	AREA_DATA *				zone;
	PC_DATA *				pcdata;
	PFLAG_DATA *			pflag_list;
	BONUS_DATA *			bonus;
	bool					valid;
	char *					name;
	char *					name2;
	char *					name3;
	char *					name4;
	char *					name5;
	char *					name6;
	long					id;
	int					magic_data;				/* na wiecej rzeczy */
	int					fight_data;				/* tu to samo :) */
	CHAR_DATA *				ch_left;				/* chains - z kim sie laczy */
	CHAR_DATA *				ch_right;				/* chains - z kim sie laczy */
	ush_int					num_attacks;			/* ilosc atakow monka */
	int						counter[MAX_COUNTER];
	sh_int					version;
	char *					short_descr;
	char *					long_descr;
	char *					description;
	char *					prompt;
	char *					prefix;
	sh_int					group;
	sh_int					sex;
	sh_int					weight;
	sh_int					height;
	sh_int					class;
	sh_int					race;
	sh_int					real_race; 	/* do szkieletów */ /* i shapeshiftingu :) */
	sh_int					level;
	sh_int					trust;
	int					played;
	int					lines;
	time_t					logon;
	sh_int					timer;
	sh_int					wait;
	sh_int					daze;
	sh_int					hit;
	sh_int					max_hit;
	sh_int					move;
	sh_int					max_move;
    long					bank; // always copper value
    long					copper;
	long					silver;
	long					gold;
	long					mithril;

    long					exp;
	long					act[MAX_VECT_BANK];
	long					comm;
	long					wiznet;
	long					wiz_config;
	sh_int					resists[MAX_RESIST];
        int					healing_from[MAX_RESIST]; // patrz komentarz w mobile_data
	sh_int					invis_level;
	sh_int					incog_level;
	long					affected_by[MAX_VECT_BANK];
	sh_int					position;
	sh_int					carry_weight;
	sh_int					carry_number;
	sh_int					saving_throw[5];
	sh_int					alignment;
	sh_int					hitroll;
	sh_int					damroll;
	sh_int					armor[4];
	sh_int					wimpy;
	/* stats */
	sh_int					perm_stat[MAX_STATS];
	sh_int					new_mod_stat[MAX_STATS];
	sh_int					statpointsleft; //aktualnie punkty do wydania
	sh_int					statpointsspent; // wydane punkty
	/* parts stuff */
	long					form;
	long					parts;
	sh_int					size;
	char *					material;
	/* mobile stuff */
	long					off_flags[MAX_VECT_BANK];
	sh_int					damage[3];
	sh_int					dam_type;
	sh_int					start_pos;
	sh_int					default_pos;
	sh_int					prog_delay;
	int						speaking;
	int						prog_response; 			/* do responsedelay */
	bool					precommand_pending; 	/* do trig_precommand */
	DO_FUN *				precommand_fun; 		/* do trig_precommand */
	char *					precommand_arg; 		/* do trig_precommand */
	char *					old_name; 				/* do zmiany imienia po name deny */
	int						walking; 				/* do mob walk */
	char *					host;
	int						reset_vnum;
	int						has_memdat;
	SHAPESHIFT_DATA *		ss_data;
//	CHAR_DATA *			    ss_animal;
	CHAR_DATA *			    ss_backup;
	ROOM_INDEX_DATA *		previous_room;			/* do transfer back i goto back */
	CHAR_LST *				si_followed;
	int					si_followed_cnt;
	bool					no_standard_si;
	sh_int					age;
	int						glory;
	sh_int					condition[5];               /*przeniesione z pcdata - u¿ywane do mêczenia zwierz±t*/
    int                     attack_flags; // typy ataku
    sh_int                  weapon_damage_bonus; // bonus dla broni
    sh_int                  magical_damage; // umagicznienie ataku
};

struct   new_char_data
{
	DESCRIPTOR_DATA *		desc;
	char *					name;
	char *					name2;
	char *					name3;
	char *					name4;
	char *					name5;
	char *					name6;
	char *					description;
	sh_int					level;
	sh_int					sex;
	sh_int					class;
	sh_int					mage_specialist;
	sh_int					race;
	sh_int					alignment;
	char *					password;
	int						creating;				/* flagi tworzenia postaci */
};

typedef struct   murder_list MURDER_LIST;

struct murder_list
{
    char *      name;
    time_t      time;
    unsigned short    room;
    int         char_level;
    int         victim_level;
    MURDER_LIST   *    next;
};


struct   death_stats
{
    int         deaths;
    int         pkdeaths;
    int         suicides;
    int         mob_kills;
    int         player_kills;
    MURDER_LIST   *    pkills_list;
    MURDER_LIST   *    pkdeath_list;

};
typedef struct   death_stats DEATH_STATS;
/*
 * Data which only PC's have.
 */

/*
 * Do questlogow.
 */
struct questlog_data
{
   char * qname; // nazwa wewnêtrzna
   char * title; // tytu³ zadania wy¶wietlany graczowi
   char * currentdesc; // informacja na temat danej czêœci zadania
   char * text; // wiadomo¶æ
   int state; // stan, zakoñczony czy w trakcie wykonywania
   int date; // data dodania
   struct questlog_data *next;
};
typedef struct questlog_data QL_DATA;

/* FRIEND_WHO: struktura z lista znajomych */
#define TIME_TO_FORGET_FRIEND 125 //czas pamietania w dniach
struct friend_list
{
	FRIEND_LIST * next;
	FRIEND_LIST * previous;
	char *name;
	char *opis;
	time_t czas; //czas poznania do sprawdzania czy ju¿ mo¿e zapomnieæ
	bool introduced;
};

struct   pc_data
{
	PC_DATA *			next;
	BUFFER *			buffer;
	COLOUR_DATA *		code;
	ROOM_INDEX_DATA *	mind_in;					//do czaru maga poznania
	bool				valid;
	time_t 				todelete; // delayed ch delete - by Fuyara
	char *				pwd;
	char *				tmp_pwd;
	char *				bamfin;
	char *				bamfout;
	char *				title;
	char *				new_title;
	char *				afk_text;
	sh_int				perm_hit;
	sh_int				perm_move;
	sh_int				true_sex;
	int					last_level;
	unsigned int		last_rent;
	sh_int				mage_specialist;
	sh_int				learned[MAX_SKILL];
	sh_int				learning[MAX_SKILL];
	sh_int				learn_mod[MAX_SKILL];
	char				language[MAX_LANG];
	char				lang_mod[MAX_LANG];
	sh_int				points;
	sh_int				oxygen;
	char *				alias[MAX_ALIAS];
	char *				alias_sub[MAX_ALIAS];
	int			    	trophy[TROPHY_SIZE][2];
	SPELL_FAILED *		spells_failed;
	DEATH_STATS			death_statistics;
	CHARM_DATA *		charm_list;
	OBJ_DATA *			corpse;
	// Kolorki
	int					text[3];					/* {t */
	int					auction[3];					/* {a */
	int					auction_text[3];			/* {A */
	int					gossip[3];					/* {d */
	int					gossip_text[3];				/* {9 */
	int					music[3];					/* {e */
	int					music_text[3];				/* {E */
	int					question[3];				/* {q */
	int					question_text[3];			/* {Q */
	int					answer[3];					/* {f */
	int					answer_text[3];				/* {F */
	int					quote[3];					/* {h */
	int					quote_text[3];				/* {H */
	int					immtalk_text[3];			/* {i */
	int					immtalk_type[3];			/* {I */
	int					info[3];					/* {j */
	int					say[3];						/* {6 */
	int					say_text[3];				/* {7 */
	int					tell[3];					/* {k */
	int					tell_text[3];				/* {K */
	int					reply[3];					/* {l */
	int					reply_text[3];				/* {L */
	int					gtell_text[3];				/* {n */
	int					gtell_type[3];				/* {N */
	int					wiznet[3];					/* {B */
	int					room_title[3];				/* {s */
	int					room_text[3];				/* {S */
	int					room_exits[3];				/* {o */
	int					room_things[3];				/* {O */
	int					prompt[3];					/* {p */
	int					shout[3];					/* {t */
	int					shout_text[3];				/* {T */
	int					yell[3];					/* {u */
	int					yell_text[3];				/* {U */
	int					fight_death[3];				/* {1 */
	int					fight_yhit[3];				/* {2 */
	int					fight_ohit[3];				/* {3 */
	int					fight_thit[3];				/* {4 */
	int					fight_skill[3];				/* {5 */
	int					fight_trick[3];				/* {v */
	int					fight_spell[3];				/* {V */
	int					security;
	long				wiz_conf;
	CLAN_DATA *			clan;
	int					clan_rank;
	char *				ignore;
	char *				last_host;
	int					hosts_count;
	char *				hosts[MAX_HOST];
	char *				name_deny_txt;
	time_t				last_logoff;
	int					last_rent_cost;
	CHAR_DATA *			remote_char;
	char *				account_email;				/* email zwi±zany z kontem */
	int					rolls_count;				/* ile dorolowañ by³o */
	int					new_rolls_count;	//NOWE STATY
//Brohacz: bounty: przeniesienie na globalna liste
//	int					bounty;
	int					tricks[MAX_TRICKS];
	QL_DATA *			ql_data;
	int					perm_hit_per_level[MAX_LEVEL];		/* zapamiêtywanie ile perm_hit go¶æ mia³ na ka¿dym poziomie */
	FRIEND_LIST *friends; /* FRIEND_WHO: pointer to list with names of people we know */
	char *introduced; /* FRIEND_WHO: person you intoduced yourself to */
	BIGFLAG spell_items_knowledge; //rellik: komponenty, pamiêtanie informacji o tym jakie komponenty znamy

/* BEGIN: reward (autoquest) */
	int			 		rewards;
	int					recovery;
	int					hunt_time;
	sh_int				reward_obj;
	sh_int				reward_mob;
    sh_int				rewarder;
/* END: reward (autoquest) */
    sh_int              learning_rasz[MAX_SKILL];//nowe plusy, Raszer
    sh_int              learning_old[MAX_SKILL];//kopia starych plusow..., Raszer
};

/*
 * Liquids.
 */
#define LIQ_WATER        0
#define LIQ_HEALING_WATER 42

struct   liq_type
{
    char *   liq_name;
    char *   liq_name2;
    char *   liq_name3;
    char *   liq_name4;
    char *   liq_name5;
    char *   liq_name6;
    char *   liq_color;
    sh_int   liq_affect[5];
};

//nowy typ tarczek
struct ShieldType
{
    int         size;   //rozmiar 0 - najmniejsza+
    char *      name;   //Nazwa typu tarczy
    int         maxAC;  //maksymalny bonus
    int         maxDam; //maksymalny damage do przyjecia
    int         otherChance;    // szansa zblokowania
                                // innych wiekszej ilosci przeciwnikow
};




/*
 * Extra description data for a room or object.
 */
struct   extra_descr_data
{
    EXTRA_DESCR_DATA *next;   /* Next in list                     */
    bool valid;
    char *keyword;              /* Keyword in look/examine          */
    char *description;          /* What to see                      */
};



/*
 * Prototype for an object.
 */
struct   obj_index_data
{
	OBJ_INDEX_DATA *		next;
	EXTRA_DESCR_DATA *		extra_descr;
	AFFECT_DATA *			affected;
	AREA_DATA *				area;
	PROG_LIST *				progs;
	SPEC_DAMAGE *			spec_dam;
	bool					new_format;
	char *					name;
	char *					name2;
	char *					name3;
	char *					name4;
	char *					name5;
	char *					name6;
	char *					short_descr;
	char *					description;
	char *					item_description;
	char *					ident_description;
	char *					hidden_description;
	unsigned int			vnum;
	sh_int					reset_num;
	sh_int					material;
	sh_int					item_type;
	long					extra_flags[MAX_VECT_BANK];
	long					wear_flags;
	long					wear_flags2[MAX_VECT_BANK];
	sh_int					level;
	sh_int					condition;
	sh_int					count;
	sh_int					length;
	sh_int					weight;
	int						cost;
	int						value[7];
	long					prog_flags[MAX_VECT_BANK];
	int						bonus_set;
	int						rent_cost;
	int						trap;
	char *					comments;
	bool					liczba_mnoga;
	int						gender;			//rodzaj w sensie gramatycznym
	bool 					is_spell_item; //rellik: komponenty, czy jest komponentem
	int						spell_item_counter; //rellik: komponenty, licznik u¿yæ
	int						spell_item_timer; //rellik: komponenty, czas przydatnoœci
    /**
     * repir limits
     *
     * repair_limit     - maksyalna liczba napraw
     * repair_counter   - aktualna liczba napraw (w pliku gracza)
     * repair_penalty   - kara nakladana na repair_condition, czyli ile mozna
     *                    maksymalnie naprawic
     * repair_condition - maksymalny poziom do jakiego wartosc "condition"
     *                    moze zostac podniesiona przez repair/sharp jest
     *                    wartoscia zmienna (w pliku gracza)
     */
    sh_int             repair_limit;
    sh_int             repair_counter;
    sh_int             repair_penalty;
    sh_int             repair_condition;
};



/*
 * One object.
 */
struct obj_data
{
    OBJ_DATA *         next;
    OBJ_DATA *         next_content;
    OBJ_DATA *         contains;
    OBJ_DATA *         in_obj;
    OBJ_DATA *         on;
    CHAR_DATA *        carried_by;
    CHAR_DATA *        hoarded_by;
    EXTRA_DESCR_DATA * extra_descr;
    AFFECT_DATA *      affected;
    OBJ_INDEX_DATA *   pIndexData;
    ROOM_INDEX_DATA *  in_room;
    CHAR_DATA *        prog_target;
    PFLAG_DATA *       pflag_list;
    unsigned int       vnum_hoard;
    bool               valid;
    bool               enchanted;
    char *             owner;
    char *             name;  //mianownik (kto? co?)
    char *             name2; //dope³niacz (kogo? czego?)
    char *             name3; //celownik (komu? czemu?)
    char *             name4; //biernik (kogo? co?)
    char *             name5; //narzêdnik ((z) kim? (z) czym?)
    char *             name6; //miejscownik (o kim? o czym?)
    char *             short_descr;
    char *             description;
    char *             item_description;
    char *             ident_description;
    char *             hidden_description;
    sh_int             item_type;
    long               extra_flags[MAX_VECT_BANK];
    long               wear_flags;
    long               wear_flags2[MAX_VECT_BANK];
    sh_int             wear_loc;
    sh_int             prewear_loc;
    sh_int             length;
    sh_int             weight;
    int                cost;
    sh_int             condition;
    sh_int             material;
    sh_int             timer;
    sh_int             on_ground;
    int                value[7];
    sh_int             prog_delay;
    int                trap;
    int                has_memdat;
    bool               shapeshift;
    bool               liczba_mnoga;
    int                gender;            //rodzaj w sensie gramatycznym
    int                wyciete; //tutaj pamietane co wycieto juz z monster_body_parts na bitach.
    SPEC_DAMAGE *      spec_dam; // spec_dam specyficzny dla tej instancji obiektu
    /* SPECDAM FIX : dorobiæ ³adowanie i zapisywanie spec_dama do danej instancji obiektu */
    bool               is_spell_item; //rellik: komponenty, czy jest komponentem
    int                spell_item_counter; //rellik: komponenty, licznik u¿yæ
    int                spell_item_timer; //rellik: komponenty, czas przydatnoœci, leci tylko podczas gry
    int                rent_cost; //rellik: przeniesienie kosztu renta do instancji
    /**
     * repir limits     - wszystko zapisywalne do pliku gracza
     *
     * repair_limit     - maksyalna liczba napraw
     * repair_counter   - aktualna liczba napraw
     * repair_penalty   - kara nakladana na repair_condition, czyli ile mozna
     *                    maksymalnie naprawic
     * repair_condition - maksymalny poziom do jakiego wartosc "condition"
     *                    moze zostac podniesiona przez repair/sharp jest
     *                    wartoscia zmienna
     */
    sh_int             repair_limit;
    sh_int             repair_counter;
    sh_int             repair_penalty;
    sh_int             repair_condition;
};

/*
 * Exit data.
 */
struct   exit_data
{
	union
	{
		ROOM_INDEX_DATA *	to_room;
		unsigned int		vnum;
	} u1;
	EXIT_DATA *		next;
	CHAR_DATA *		alarm; /* do czaru maga poznania */
	int				exit_info;
	unsigned int	key;
	char *			keyword;
	char *			description;
	char *			nightdescription;
	char *			vName;
	char *			TextOut;
	char *			TextIn;
	int			rs_flags;
	int			orig_door;
	int			trap;
	char *			biernik;
	bool			liczba_mnoga;
};



/*
 * Reset commands:
 *   '*': comment
 *   'M': read a mobile
 *   'O': read an object
 *   'P': put object in object
 *   'G': give object to mobile
 *   'E': equip object to mobile
 *   'D': set state of door
 *   'R': randomize room exits
 *   'S': stop (end of list)
 */

/*
 * Area-reset definition.
 */
struct   reset_data
{
	RESET_DATA *	next;
	char			command;
	unsigned int	arg0;
	unsigned int	arg1;
	int				arg2;
	int				arg3;
	unsigned int	arg4;
};



/*
 * Area definition.
 */
struct   area_data
{
	AREA_DATA *		next;
	HELP_AREA *		helps;
	DESC_DATA *		desc_data;
	char *			file_name;
	char *			name;
	char *			credits;
	sh_int			age;
	sh_int			resetage;
	sh_int			nplayer;
	sh_int			low_range;
	sh_int			high_range;
	unsigned int	min_vnum;
	unsigned int	max_vnum;
	bool			empty;
	char *			builders;
	int				vnum;
	int				area_flags;
	int				security;
	int				region;
	char *			locked;			/* je¶li niepuste nie mozna krainy edytowaæ, w tre¶ci powód tego¿ */
};

/*
mapka
*/
struct mapka_data
{
		MAP_DATA * next;
		ROOM_INDEX_DATA *	room;
		sh_int poz_x;
		sh_int poz_y;
		sh_int poz_z;
};

/*
 * Track - pozycja listy
 */
struct track_data
{
	TRACK_DATA *previous;
	CHAR_DATA* ch;
	time_t czas; //aktualizowane w update, wpisy starsze niz 1h RL s± kasowane
	sh_int kierunek; //tak jak w exitach
	sh_int jak; //0 - pieszo, 1 - konno, 2 - inaczej
	sh_int inout;//0 - wchodz±cy, 1 - wychodz±cy
};
/*
 * Room type.
 */
struct   room_index_data
{
	ROOM_INDEX_DATA *	next;
	CHAR_DATA *			people;
	OBJ_DATA *			contents;
	PROG_LIST *			progs;
	EXTRA_DESCR_DATA *	extra_descr;
	AREA_DATA *			area;
	EXIT_DATA *			exit[MAX_EXIT_COUNT];
	RESET_DATA *		reset_first;
	RESET_DATA *		reset_last;
	CHAR_DATA *			minds; /* do czaru maga poznania */
	char *				name;
	char *				description;
	char *				nightdesc;
	ush_int				day_rand_desc;
	ush_int				night_rand_desc;
	char *				owner;
	unsigned int		vnum;
	long				room_flags[MAX_VECT_BANK];
	sh_int				light;
	sh_int				sector_type;
	sh_int				original_sector_type;
	sh_int				heal_rate;
	long				prog_flags[MAX_VECT_BANK];
	sh_int				prog_delay;
	CHAR_DATA *			prog_target;
	PFLAG_DATA *		pflag_list;
	int					trap;
	sh_int				rent_rate;
	sh_int				herbs_count;
	int					area_part;
	int					has_memdat;
	sh_int				touched; //rellik: mapka, uzywane przy mapce
	HERB_IN_ROOM_DATA      *first_herb;
	sh_int                 herb_update_timer;
	int 				rawmaterial[MAX_RAWMATERIAL]; //rellik: mining, tabela okre¶laj±ca prawdopodobieñstwo wypadniêcia materia³u przy mining 0 = 0 szans czyli nie ma.
	int					rawmaterial_capacity; 				//rellik: mining, ile jest minera³ów w ¿yle aktualnie (ile bry³ek do wyczerpania)
	int					rawmaterial_capacity_now;			//rellik: mining, ile jest aktualnie
	union
	{
		ROOM_INDEX_DATA *	room;
		sh_int				vnum;
	} echo_to;
	TRACK_DATA*         track_data;
};



/*
 * Types of attacks.
 * Must be non-overlapping with spell/skill types,
 * but may be arbitrary beyond that.
 */
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000


/* SAVES TYPE */

#define SAVE_DEATH      0
#define SAVE_ROD      1
#define SAVE_PETRYFICATION   2
#define SAVE_BREATH      3
#define SAVE_SPELL      4


/*
 *  Target types.
 */
#define TAR_IGNORE          0
#define TAR_CHAR_OFFENSIVE       1
#define TAR_CHAR_DEFENSIVE       2
#define TAR_CHAR_SELF          3
#define TAR_OBJ_INV          4
#define TAR_OBJ_CHAR_DEF       5
#define TAR_OBJ_CHAR_OFF       6
#define TAR_OBJ_ROOM          7
#define TAR_OBJ_IGNORE          8

#define TARGET_CHAR 0
#define TARGET_OBJ  1
#define TARGET_ROOM 2
#define TARGET_NONE 3
#define TARGET_SELF 4 //do prewaitów
#define TARGET_SAGE 5 // do ACT_SAGE i do_identify


/*
 * Skills include spells as a particular case.
 */
struct   skill_type
{
    char *   name;         /* Name of skill      */
    sh_int   skill_level[MAX_CLASS];   /* Level needed by class   */
    SPELL_FUN *   spell_fun;      /* Spell pointer (for spells)   */
    sh_int   target;         /* Legal targets      */
    sh_int   minimum_position;   /* Position for caster / user   */
    sh_int *   pgsn;         /* Pointer to associated gsn   */
    sh_int   save_type;      /* czy jest jakis rzut obronny  */
    sh_int   save_mod;      /* jaki modyfikator      */
    sh_int   beats;         /* Waiting time after use   */
    char *   noun_damage;      /* Damage message      */
    char *   msg_off;      /* Wear off message      */
    char *   msg_obj;      /* Wear off message for objects   */
    bool   round_hour;
    sh_int   school;
    sh_int    colour_style1;
    sh_int    colour_style2;
    char *     affect_desc;
    char *     ident_desc;
};

struct school_type
{
    char *    name;
    char *   specialist_name;
    char *   specialist_female_name;
    char *   specialist_name_31;
    char *   specialist_female_name_31;
    sh_int    flag;
    sh_int   reverse_school;
    sh_int   race[3];
};

struct prog_list
{
	PROG_LIST *			next;				/* wska¼nik na nastêpny trigger */
	BITVECT_DATA *		trig_type;			/* na jaki trigger siê odpala */
	char *				trig_phrase;		/* parametr triggera */
	char *				name;				/* nazwa programu (jego identyfikator) */
	char *				code;				/* kod programu */
	int					valid_positions;	/* pozycje moba, które pozwalaj± na odpalenie triggera */
	bool				valid;				/* do zarz±dzania pamiêci± */
};

struct prog_code
{
	PROG_CODE *			next;				/* wska¼nik na nastêpny program */
	AREA_DATA *			area;				/* kraina, do której przypisany jest program */
	char *				name;				/* nazwa programu (jego identyfikator) */
	char *				description;		/* opis programu */
	char *				code;				/* kod programu */
};

struct trap_data
{
	TRAP_DATA *			next;
	unsigned int		vnum;			/* numer identyfikacyjny */
	bool				active;			/* aktywna/rozbrojona */
	int					level;			/* poziom trudnosci pulapki */
	int					type;			/* typ pulapki: room/exit/obj */
	char *				triggered;		/* prog podczepiony pod uruchomienie */
	char *				disarmed;		/* prog podczepiony pod rozbrojenie */
	char *				failed_disarm;	/* prog pod nieudane rozbrojenie */
	int					limit;			/* wymagany: dex/lev/skill */
};

//stuff do randomowych opisów
struct desc_data
{
	DESC_DATA *			next;
	char *				name;
	char *				description;
	int					group;
};

struct rand_desc_data
{
	RAND_DESC_DATA *	next;
	AREA_DATA *			area;
	unsigned int		vnum;			/* numer identyfikacyjny */
	char *				rand_desc;		/* opis z dyrektywami #*/
};

struct misc_data
{
	time_t				copyover_delayed;
	time_t				copyover_period;
	time_t				copyover_scheduled;
	bool				copyover_save;
	long				world_time;
	PFLAG_DATA *		global_flag_list;
	BOUNTY_DATA *		bounty_list;
};

struct info_descriptor_data
{
	INFO_DESCRIPTOR_DATA *	next;
	int						descriptor;
	int						state;
	char					input[MAX_INPUT_LENGTH];
};

struct account_data
{
	char *		email;
	char *		passwd;
	char *		characters;		/* " Imie1 Imie2 Imie3 " */
	char *		activation;
};

struct bonus_data
{
	BONUS_DATA *		next;
	BONUS_INDEX_DATA *	index_data;
};

struct bonus_index_data
{
	BONUS_INDEX_DATA *	next;
	AFFECT_DATA *		affects;
	AREA_DATA *			area;
	EXTRA_DESCR_DATA *	extra_descr;
	unsigned int		vnum;
	long				prog_flags[MAX_VECT_BANK];
	char *				name;
	char *				description;
	char *				comments;
	char *				wear_prog;
	char *				remove_prog;
};

struct perform_data
{
	PERFORM_DATA *		next;
	int					type;				/* flet, harfa, etc - dla jakich instrumentów to jest tekst */
	char *				perform_prog[4];	/* 0 - z³e wykonanie, 1 - s³abe, 2 - ¶rednie, 3 - dobre */
};

struct song_data
{
	SONG_DATA *			next;
	PERFORM_DATA *		perform;
	char *				name;
	char *				description;
	char *				comments;
	int					type;				/* ode, ballad, etc */
	int					slots;				/* ile slotów zajmuje */
	int					target;				/* to_all, to_group etc */
};

//do spec_damage - type
#define SD_TYPE_NONE       0
#define SD_TYPE_MAGIC      1
#define SD_TYPE_RACE       2
#define SD_TYPE_ALIGN      3
#define SD_TYPE_SIZE       4
#define SD_TYPE_SELFSTAT   5
#define SD_TYPE_TARGETSTAT 6
#define SD_TYPE_CLASS      7
#define SD_TYPE_ACT        8

//do spec_damage - parametr domy¶lny
#define SD_PARAM_ANY      -100

//Tener
//do spec_damage - czy specdam zwieksza damage czy umagicznienie broni
#define SD_TARGET_DAM      0
#define SD_TARGET_MAG      1

struct spec_damage
{
    SPEC_DAMAGE *next;
    int type;
    int chance;
    int param1;
    int param2;
    int param3;
    long ext_param[MAX_VECT_BANK];
    int bonus;
    int target_type; // czy specdam zwieksza damage czy umagicznienie broni. mozna dodac jeszcze zwiekszenie hita. Tener
};

struct   weed_type
{
    char *   weed_name;
    char *   weed_name2;
    char *   weed_name3;
    char *   weed_name4;
    char *   weed_name5;
    char *   weed_name6;
    char *   weed_color;
    sh_int   weed_affect[3];
};

/**
 * toksycznosc mikstur
 */
struct toxic_type
{
    char * name;
    char * description;
    char * gender[6];
    sh_int power[2];
};
extern const struct toxic_type toxic_table [];

/*
  A tutaj ziolka (herbs)
*/

//#define MAX_HERB

struct herb_type
{
       char *   lname;  //czesc laicka (czyli opis wygladu)
       char *	lname2;
       char *	lname3;
       char *	lname4;
       char *	lname5;
       char *	lname6;

       char *   name;
       char *	name2;
       char *	name3;
       char *	name4;
       char *	name5;
       char *	name6;

       char *   description;

       int      cost;       //czyli wartosc ziolo +-s
       sh_int   plant_part; //czesc rosliny
       sh_int   difficult;  //trudnosc znalezienia (0-99)
       sh_int   poison; //czy trujaca (-1 to nie) w sensie ENVENOMER

       sh_int   sectors[50]; //prawdopodobienstwo wystapienia (43 to ilosc sectorow+2 na zapas)

       sh_int   eating_effect;
       char *   eating_desc;  //czyli smak
       sh_int   smoke_effect;
       char *   smoke_desc;   //czy smak dymu
};

struct   trick_type
{
    char*	name;//nazwa trika
    int		which_skill;//ktorego skilla jest to trick ( w sn )
    int		which_skill2;//jak -1 to nic nie robi, w przypadku gry trik ma wymagac dwoch skilli, np dragon strike 'spear' i 'twohanded weapon'
    int		alternate_skill1;//jak -1 to nic, skill, ktory jest alternatywa dla which_skill
    int		skill_percent;//na ile musisz skilla(e) wytrenowac by sie mozna bylo tricka nauczyc
    TRICK_FUN* trick_fun;//sama funkcja
    int		frequency;//na ile tickow gwiazdkuje <- to jeszcze nie dziala
    int		chance;//jaka szansa ze zaskoczy przy uzyciu skilla (w dziesiatych procenta!)
    int		learn_freq;//co ile tickow mozna probowac introduce <- to jeszcze nie dziala
};

struct learn_trick_list
{
    sh_int sn;       /* sn tricka      */
    sh_int chance;   /* procentowo szansa na nauczenie sie tricka */
    sh_int payment;  /* ile wezmie kasy za probe uczenia sie */
    LEARN_TRICK_LIST *next;
    bool valid;
};

struct learn_trick_data
{
    unsigned int       vnum;  /* numer moba nauczyciela */
    LEARN_TRICK_DATA * next;  /* nastepny, do lsty      */
    LEARN_TRICK_LIST * list;  /* lista trickow          */
    bool               valid;
};

/* rellik: carve
  tutaj sa czesci cial ktore bedzie mozna wyciac z lepszych potworow, bedzie je mozna sprzedac lub dodawac do mikstur
  wyciecie bedzie zalezao od skill_skin
*/
struct monster_body_parts
{
	char *   name;
	char *	name2;
	char *	name3;
	char *	name4;
	char *	name5;
	char *	name6;
	char * short_desc;
	char * long_desc;
	char *   description;

	char *race; //z race_table
	int owner_vnum; //vnum moba z którego mo¿na wyci±æ, je¶li = 0 to pod uwagê brane race
	int czesc; //ktora czesc z poszczegolnej rasy, unikalne na rase i uzywane w corps do sprawdzania czy juz wyciete
	sh_int difficult;  //jak trudno czê¶æ wyci±æ
	sh_int chance; //szansa na niezepsucie
	int cost; //cena sprzedazy ewentualnej
	int gender;
	bool skn_destroyed; //czy wycinanie tej czesci zniszczy skore
	bool has_funct; //ma funkcjê
	char *color; //kolor cze¶ci
	bool show;
};

bool check_trick args ( ( CHAR_DATA *ch, CHAR_DATA *victim, int sn ) );

/*
 * These are skill_lookup return values for common skills and spells.
 */
extern   sh_int   gsn_backstab;
extern   sh_int   gsn_circle;
extern   sh_int   gsn_dodge;
extern   sh_int   gsn_envenom;
extern   sh_int   gsn_hide;
extern   sh_int   gsn_peek;
extern   sh_int   gsn_pick_lock;
extern   sh_int   gsn_sneak;
extern   sh_int   gsn_steal;

extern   sh_int   gsn_disarm;
extern   sh_int   gsn_enhanced_damage;
extern   sh_int   gsn_kick;
extern   sh_int   gsn_crush;
extern   sh_int   gsn_tail;
extern   sh_int   gsn_skin;
extern   sh_int   gsn_charge;
extern   sh_int   gsn_stun;
extern   sh_int   gsn_shoot;
extern   sh_int   gsn_lay;
extern   sh_int   gsn_garhal;
extern   sh_int   gsn_parry;
extern   sh_int   gsn_shield_block;
extern   sh_int   gsn_rescue;
extern   sh_int   gsn_second_attack;
extern   sh_int   gsn_third_attack;
extern   sh_int   gsn_fire_breath;
extern   sh_int   gsn_gas_breath;
extern   sh_int   gsn_frost_breath;
extern   sh_int   gsn_acid_breath;
extern   sh_int   gsn_lightning_breath;

extern   sh_int   gsn_deafness;
extern   sh_int   gsn_blindness;
extern   sh_int   gsn_power_word_blindness;
extern   sh_int   gsn_pyrotechnics;
extern   sh_int   gsn_sunscorch;
extern   sh_int   gsn_call_lightning;
extern   sh_int   gsn_summon_lesser_meteor;
extern   sh_int   gsn_summon_greater_meteor;
extern   sh_int   gsn_summon_animals;
extern   sh_int   gsn_charm_person;
extern   sh_int   gsn_charm_monster;
extern   sh_int   gsn_daze;
extern   sh_int   gsn_curse;
extern   sh_int   gsn_energy_drain;
extern   sh_int   gsn_detect_invis;
extern   sh_int   gsn_detect_hidden;
extern   sh_int   gsn_wizard_eye;
extern   sh_int   gsn_invis;
extern   sh_int   gsn_mass_invis;
extern  sh_int  gsn_plague;
extern   sh_int   gsn_poison;
extern   sh_int   gsn_feeblemind;
extern   sh_int   gsn_sleep;
extern  sh_int  gsn_fly;
extern  sh_int  gsn_sanctuary;
extern  sh_int  gsn_holdperson;
extern  sh_int   gsn_holdmonster;
extern   sh_int   gsn_holdplant;
extern   sh_int   gsn_holdanimal;
extern   sh_int   gsn_reflect_spell;
extern  sh_int  gsn_holdevil;
extern  sh_int  gsn_ghoul_touch;
extern  sh_int  gsn_domination;
extern  sh_int  gsn_float;
extern  sh_int  gsn_two_hands_fighting;
extern sh_int  gsn_made_undead;
/* new gsns */
extern sh_int  gsn_axe;
extern sh_int  gsn_dagger;
extern sh_int  gsn_flail;
extern sh_int  gsn_mace;
extern sh_int  gsn_polearm;
extern sh_int  gsn_sharpen;
extern sh_int  gsn_recuperate;
extern sh_int  gsn_repair;
extern sh_int  gsn_bladethirst;
extern sh_int  gsn_healing_sleep;
extern sh_int  gsn_energize;
extern sh_int  gsn_shortsword;
extern sh_int  gsn_staff;
extern sh_int  gsn_spear;
extern sh_int  gsn_sword;
extern sh_int  gsn_whip;
extern sh_int  gsn_bash;
extern sh_int  gsn_berserk;
extern sh_int  gsn_divine_favor;
extern sh_int  gsn_divine_power;
extern sh_int  gsn_divine_shield;
extern sh_int  gsn_wardance;
extern sh_int  gsn_damage_reduction;
extern sh_int  gsn_luck;
extern sh_int  gsn_misfortune;
extern sh_int  gsn_slam;
extern sh_int  gsn_sap;
extern sh_int  gsn_claws;

extern sh_int  gsn_playerquityes;
extern sh_int  gsn_playerdeath;
extern sh_int  gsn_playerdeathpenalty;
extern sh_int  gsn_playerdeathlearnpenalty;
extern sh_int  gsn_noob_killer;
extern sh_int  gsn_mirrorfall;
extern sh_int  gsn_dehydration;
extern sh_int  gsn_malnutrition;
extern sh_int  gsn_on_smoke;
extern sh_int  gsn_bleeding_wound;
extern sh_int  gsn_bleed;
extern sh_int  gsn_thigh_jab;
extern sh_int  gsn_weapon_wrench;
extern sh_int  gsn_glorious_impale;
extern sh_int  gsn_rampage;

//style walki
extern sh_int  gsn_two_weapon_fighting;
extern sh_int  gsn_twohander_fighting;

//mastery
extern sh_int	gsn_dagger_mastery;
extern sh_int	gsn_mace_mastery;
extern sh_int	gsn_flail_mastery;
extern sh_int	gsn_sword_mastery;
extern sh_int	gsn_axe_mastery;
extern sh_int	gsn_spear_mastery;
extern sh_int	gsn_staff_mastery;
extern sh_int	gsn_shortsword_mastery;
extern sh_int	gsn_whip_mastery;
extern sh_int	gsn_polearm_mastery;

extern sh_int  gsn_fast_healing;
extern sh_int  gsn_riding;
extern sh_int  gsn_lore;
extern sh_int  gsn_skin;
extern sh_int  gsn_meditation;
extern sh_int  gsn_bandage;
extern sh_int  gsn_smite;
extern sh_int  gsn_turn;
extern sh_int  gsn_detect_traps;
extern sh_int  gsn_disarm_traps;
extern sh_int  gsn_target;
extern sh_int  gsn_trip;

extern sh_int  gsn_recall;
extern sh_int  gsn_track;
extern sh_int  gsn_confusion;
extern sh_int  gsn_regenerate;
extern sh_int  gsn_animal_invis;
extern sh_int  gsn_undead_invis;
extern sh_int  gsn_resist_fire;
extern sh_int  gsn_resist_cold;
extern sh_int  gsn_resist_lightning;
extern sh_int  gsn_resist_acid;
extern sh_int  gsn_resist_magic;
extern sh_int  gsn_fear;
extern sh_int  gsn_brave_cloak;
extern sh_int  gsn_silence;
extern sh_int  gsn_bark_skin;
extern sh_int  gsn_aid;
extern sh_int  gsn_chant;
extern sh_int  gsn_prayer;
extern sh_int  gsn_waterwalk;
extern sh_int  gsn_entangle;
extern sh_int  gsn_web;
extern sh_int  gsn_water_breathing;
extern sh_int  gsn_energy_shield;
extern sh_int  gsn_stone_skin;
extern sh_int  gsn_mirror_image;
extern sh_int  gsn_blur;
extern sh_int  gsn_summon;
extern sh_int  gsn_dimension_door;
extern sh_int  gsn_recharge;
extern sh_int  gsn_fireshield;
extern sh_int  gsn_iceshield;
extern sh_int  gsn_increase_wounds;
extern sh_int  gsn_armor;
extern sh_int  gsn_mighty_blow;
extern sh_int  gsn_power_strike;
extern sh_int  gsn_critical_strike;
extern sh_int  gsn_heat_metal;
extern sh_int  gsn_chill_metal;
extern sh_int  gsn_blade_barrier;
extern sh_int  gsn_mental_barrier;
extern sh_int  gsn_immolate;
extern sh_int  gsn_alicorn_lance;
extern sh_int  gsn_storm_shell;
extern sh_int  gsn_devour;
extern sh_int  gsn_unholy_fury;
extern sh_int  gsn_resist_summon;
extern sh_int  gsn_energy_strike;
extern sh_int  gsn_hallucinations;
extern sh_int  gsn_perfect_self;
extern sh_int  gsn_confusion_shell;
extern sh_int  gsn_maze;
extern sh_int  gsn_antimagic_manacles;
extern sh_int  gsn_astral_journey;
extern sh_int  gsn_steel_scarfskin;
extern sh_int  gsn_acid_arrow;
extern sh_int  gsn_summon_distortion;
extern sh_int  gsn_blink;
extern sh_int  gsn_fetch;
extern sh_int  gsn_draining_hands;
extern sh_int  gsn_magic_hands;
extern sh_int  gsn_yell;
extern sh_int  gsn_inspire;
extern sh_int  gsn_hardiness;
extern sh_int  gsn_defense_curl;
extern sh_int  gsn_mind_fortess;
extern sh_int  gsn_razorbladed;
extern sh_int  gsn_shadow_swarm;
extern sh_int  gsn_dazzling_flash;
extern sh_int  gsn_scrying_shield;
extern sh_int  gsn_free_action;
extern sh_int  gsn_minor_globe_of_invulnerability;
extern sh_int  gsn_globe_of_invulnerability;
extern sh_int  gsn_major_globe_of_invulnerability;
extern sh_int  gsn_resist_normal_weapon;
extern sh_int  gsn_resist_magic_weapon;
extern sh_int  gsn_resist_weapon;
extern sh_int  gsn_nimbleness;
extern sh_int  gsn_draconic_wisdom;
extern sh_int  gsn_insight;
extern sh_int  gsn_behemot_toughness;
extern sh_int  gsn_inspiring_presence;
extern sh_int  gsn_resist_elements;
extern sh_int  gsn_eyes_of_the_torturer;
extern sh_int  gsn_deflect_wounds;
extern sh_int  gsn_breath_of_life;
extern sh_int  gsn_razorblade_hands;
extern sh_int  gsn_stability;
extern sh_int  gsn_perfect_senses;
extern sh_int  gsn_fortitude;
extern sh_int  gsn_cat_grace;
extern sh_int  gsn_owl_wisdom;
extern sh_int  gsn_fox_cunning;
extern sh_int  gsn_bear_endurance;
extern sh_int  gsn_eagle_splendor;
extern sh_int  gsn_ethereal_armor;
extern sh_int  gsn_darkvision;
extern sh_int  gsn_continual_light;
extern sh_int  gsn_finger_of_death;
extern sh_int  gsn_weaken;
//monk
extern sh_int  gsn_flurry_of_blows;
extern sh_int  gsn_concentration;
extern sh_int  gsn_rush;
extern sh_int  gsn_healing_hands;
extern sh_int  gsn_unarmed_strike;
extern sh_int  gsn_monk_dodge;
extern sh_int  gsn_knockdown;
extern sh_int  gsn_stunning_fist;

//druid
extern sh_int gsn_herbs_knowledge;
extern sh_int gsn_animal_rage;
extern sh_int gsn_magic_fang;
extern sh_int gsn_lava_bolt;
extern sh_int gsn_wind_charger;
extern sh_int gsn_nature_curse;
extern sh_int gsn_shapeshift;
extern sh_int gsn_shillelagh;
extern sh_int gsn_beast_claws;
extern sh_int gsn_shield_of_nature;
extern sh_int gsn_fury_of_the_wild;
extern sh_int gsn_create_spring;
extern sh_int gsn_wood_master;
extern sh_int gsn_singing_ruff;

//paladyn
extern sh_int  gsn_holy_weapons;
extern sh_int  gsn_deeds_pool;
extern sh_int  gsn_holy_pool;
extern sh_int  gsn_holy_prayer;
extern sh_int  gsn_prayer_last;
extern sh_int  gsn_sanctification;
extern sh_int  gsn_sacred_group;
extern sh_int  gsn_aura_of_vigor;
extern sh_int  gsn_aura_of_improved_healing;

//kleryk
extern sh_int  gsn_bless;
extern sh_int  gsn_spirit_hammer;
extern sh_int  gsn_word_of_recall;
extern sh_int  gsn_create_symbol;
extern sh_int  gsn_life_transfer;
extern sh_int  gsn_first_aid;
extern sh_int  gsn_healing_touch;
extern sh_int  gsn_protection_evil;
extern sh_int  gsn_protection_good;

//bard
extern sh_int gsn_listen;
extern sh_int gsn_persuasion;
extern sh_int gsn_ocarina;
extern sh_int gsn_flute;
extern sh_int gsn_lute;
extern sh_int gsn_harp;
extern sh_int gsn_mandolin;
extern sh_int gsn_piano;
extern sh_int gsn_music;
extern sh_int gsn_trade;
extern sh_int gsn_lore_intuition;
extern sh_int gsn_tune;
extern sh_int gsn_sing;

//czarny rycerz
extern sh_int gsn_torment;
extern sh_int gsn_overwhelming_strike;
extern sh_int gsn_cleave;
extern sh_int gsn_demon_aura;
extern sh_int gsn_vertical_slash;
extern sh_int gsn_damn_weapon;
extern sh_int gsn_damn_armor;
extern sh_int gsn_smite_good;
extern sh_int gsn_call_avatar;
extern sh_int gsn_hustle;
extern sh_int gsn_control_undead;
extern sh_int gsn_undead_resemblance;

//przemiany
extern sh_int gsn_basic_shapeshift;
extern sh_int gsn_slow;
extern sh_int gsn_major_haste;
extern sh_int gsn_haste;
extern sh_int gsn_giant_strength;
extern sh_int gsn_strength;
extern sh_int gsn_champion_strength;
extern sh_int gsn_bull_strength;

//evoker
extern sh_int gsn_burn;
extern sh_int gsn_rain_of_blades;
extern sh_int gsn_elemental_devastation;
extern sh_int gsn_psionic_blast;
extern sh_int gsn_lesser_psionic_blast;

//art destroy
extern sh_int gsn_art_destroy;

//szaman
extern   sh_int   gsn_invoke_spirit;
extern   sh_int   gsn_subdue_spirits;
extern   sh_int   gsn_spirit_of_life;
extern   sh_int   gsn_spirit_armor;
extern   sh_int   gsn_spiritual_guidance;
extern   sh_int   gsn_ancestors_wisdom;
extern   sh_int   gsn_ancestors_protection;
extern   sh_int   gsn_ancestors_favor;
extern   sh_int   gsn_ancestors_vision;
extern   sh_int   gsn_ancestors_fury;
extern   sh_int   gsn_seal_of_weakness;
extern   sh_int   gsn_seal_of_despair;
extern   sh_int   gsn_seal_of_doom;

//rozne takie
extern sh_int gsn_none;

/*
 * Utility macros.
 */
#define IS_VALID(data)      (((data) != NULL) && ((data)->valid))
#define VALIDATE(data)      ((data)->valid = TRUE)
#define INVALIDATE(data)   ((data)->valid = FALSE)
//#define UMIN(a, b)      ((a) < (b) ? (a) : (b))
//#define UMAX(a, b)      ((a) > (b) ? (a) : (b))
//#define URANGE(a, b, c)      ((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))
//#define LOWER(c)      ((c) >= 'A' && (c) <= 'Z' ? (c)+'a'-'A' : (c))
//#define UPPER(c)      ((c) >= 'a' && (c) <= 'z' ? (c)+'A'-'a' : (c))
#define IS_SET(flag, bit)   (((flag) & (bit)) > 0) // zeby IS_SET dawalo typ bool
#define SET_BIT(var, bit)   ((var) |= (bit))
#define REMOVE_BIT(var, bit)   ((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)    ((var) ^= (bit))

#define EXT_IS_EQUAL(bitvect1, bitvect2)	( ( (bitvect1).bank == (bitvect2).bank ) && ( (bitvect1).vector == (bitvect2).vector ) )
#define EXT_IS_SET(flag, bitvect)			( IS_SET( *( flag + (bitvect).bank ), (bitvect).vector ) )
#define EXT_SET_BIT(flag, bitvect)			( SET_BIT( *( flag + (bitvect).bank ), (bitvect).vector ) )
#define EXT_REMOVE_BIT(flag, bitvect)		( REMOVE_BIT( *( flag + (bitvect).bank ), (bitvect).vector ) )
#define EXT_TOGGLE_BIT(flag, bitvect)		( TOGGLE_BIT( *( flag + (bitvect).bank ), (bitvect).vector ) )

#define IS_NULLSTR(str)      ((str) == NULL || (str)[0] == '\0')
#define ENTRE(min,num,max)   ( ((min) <= (num)) && ((num) <= (max)) )
#define CHECK_POS(a, b, c)   {                     \
               (a) = (b);               \
               if ( (a) < 0 )               \
               bug( "CHECK_POS : " c " == %d < 0", a );   \
            }
                                 \
#define IS_ACTIVE(ch)      (ch->position == POS_RESTING || ch->position == POS_SITTING || ch->position == POS_STANDING)

/*
 * Character macros.
 */
#define IS_NPC(ch)           (EXT_IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMMORTAL(ch)      (get_trust(ch) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)          (get_trust(ch) >= LEVEL_HERO)
#define IS_NEWBIE(ch)        (ch->level <=  LEVEL_NEWBIE)
#define IS_TRUSTED(ch,level) (get_trust((ch)) >= (level))
#define IS_AFFECTED(ch, sn)  (EXT_IS_SET((ch)->affected_by,(sn)))
#define IS_GOOD(ch)          (ch->alignment >= 350)
#define IS_EVIL(ch)          (ch->alignment <= -350)
#define IS_NEUTRAL(ch)       (!IS_GOOD(ch) && !IS_EVIL(ch))
#define IS_SAME_ALIGN(ch, victim)	( ( IS_GOOD( ch ) && IS_GOOD( victim ) ) \
									|| ( IS_EVIL( ch ) && IS_EVIL( victim ) ) \
									|| ( IS_NEUTRAL( ch ) && IS_NEUTRAL( victim ) ) )
#define ALIGN_STR(ch)   ( IS_GOOD(ch) ? "dobry" : IS_EVIL(ch) ? "z³y" : "neutralny" )

#define IS_AWAKE(ch)      (ch->position > POS_SLEEPING)

#define GET_PERCENT_HP(ch)      ((get_max_hp(ch)>0)? (100*ch->hit)/get_max_hp(ch):-1)

#define GET_PERCENT_MV(ch)      ((ch->max_move>0)?(100*ch->move)/ch->max_move:-1)

//#define IS_SPIRIT(ch)       ( ( (ch) && (ch)->pcdata && (ch)->pcdata->corpse ) && (ch)->pcdata->corpse != NULL )

//#define GET_RACE( ch ) ( ch->real_race == 0 || ch->ss_data ? ch->race : ch->real_race )
//przerobione na funkcje
#define SET_RACE( ch, new_race ) \
							if ( ch->real_race == 0 ) \
							{ \
								ch->race = new_race; \
							} \
							else \
							{ \
								ch->real_race = new_race; \
							}

#define IS_OUTSIDE(ch)      (!EXT_IS_SET(                \
                (ch)->in_room->room_flags,          \
                ROOM_INDOORS))

#define WAIT_STATE(ch, npulse)   ((ch)->wait = UMAX((ch)->wait, (npulse)))
#define DAZE_STATE(ch, npulse)  ((ch)->daze = UMAX((ch)->daze, (npulse)))

#define get_carry_weight(ch)   (ch)->carry_weight + (IS_NPC(ch)? 0:money_count_weight_carried_by_character(ch))

#define act(format,ch,arg1,arg2,type)\
   act_new((format),(ch),(arg1),(arg2),(type),POS_RESTING)

#define HAS_TRIGGER(ch,trig)   (EXT_IS_SET((ch)->pIndexData->prog_flags,(trig)))
#define HAS_OTRIGGER(obj,trig)   (EXT_IS_SET((obj)->pIndexData->prog_flags,(trig)))
#define HAS_RTRIGGER(room,trig)   (EXT_IS_SET((room)->prog_flags,(trig)))
#define IS_SWITCHED( ch )       ( ch->desc && ch->desc->original )
#define IS_BUILDER(ch, Area)   ( !IS_NPC(ch) && !IS_SWITCHED( ch ) &&     \
            ( ch->pcdata->security >= Area->security  \
            || strstr( Area->builders, ch->name )     \
            || strstr( Area->builders, "All" ) ) )
#define SPELL_MEMMED( ch, spell ) ( ch->memspell_cache[ spell ] )

/*
 * Object macros.
 */
#define CAN_WEAR(obj, part)   (IS_SET((obj)->wear_flags,  (part)))
#define GOT_PART(ch, part)   (IS_SET((ch)->parts,        (part)))
#define IS_OBJ_STAT(obj, stat)   (EXT_IS_SET((obj)->extra_flags, (stat)))
#define IS_WEAPON_STAT(obj,stat)(IS_SET((obj)->value[4],(stat)))
#define WEIGHT_MULT(obj)   ((obj)->item_type == ITEM_CONTAINER ? \
   (obj)->value[4] : 100)

#define SAME_AREA(a, b)   ( a == b || ( a->region != 0 && a->region == b->region ) )
#define SAME_AREA_PART(a, b)   ( a->in_room && b->in_room && a->in_room->area_part == b->in_room->area_part  )

#define FAST_STR_CMP( cmp, str ) ( upper_nopol[(unsigned char)(cmp[0])] == upper_nopol[(unsigned char)(str[0])] && !str_cmp( cmp, str ) )

/*
 * Memory allocation macros.
 */
#define CREATE(result, type, number)                                        \
do                                                                          \
{                                                                           \
    if (!((result) = (type *) calloc ((number), sizeof(type))))             \
    {                                                                       \
   bugf( "Malloc failure @ %s:%d\n", __FILE__, __LINE__ );           \
   abort();                                                                \
    }                                                                       \
} while(0)

#define RECREATE(result,type,number)                                        \
do                                                                          \
{                                                                           \
    if (!((result) = (type *) realloc ((result), sizeof(type) * (number)))) \
    {                                                                       \
   bugf( "Realloc failure @ %s:%d\n", __FILE__, __LINE__ );          \
   abort();                                                                \
    }                                                                       \
} while(0)

#define DISPOSE(point)                      \
do                                          \
{                                           \
   if((point))                              \
  {                                         \
     free((point));                         \
     (point) = NULL;                        \
  }                                         \
} while(0)

#define free_string(point)                  \
if ( (point) != &str_empty[0] )             \
do                                          \
{                                           \
   if((point))                              \
  {                                         \
     free((point));                         \
     (point) = NULL;                        \
  }                                         \
} while(0)

#ifdef HASHSTR
#define STRALLOC(point)      str_alloc((point))
#define QUICKLINK(point)   quick_link((point))
#define QUICKMATCH(p1, p2)   (int) (p1) == (int) (p2)
#define STRFREE(point)                                                              \
do                                                                                  \
{                                                                                   \
   if((point))                                                                      \
  {                                                                                 \
   if( str_free((point)) == -1 )                                                   \
       log_printf( "STRFREEing bad pointer in %s, line %d\n", __FILE__, __LINE__ ); \
    (point) = NULL;                                                                 \
  }                                                                                 \
} while(0)
#else
#define STRALLOC(point)      str_dup((point))
#define QUICKLINK(point)   str_dup((point))
#define QUICKMATCH(p1, p2)   strcmp((p1), (p2)) == 0
#define STRFREE(point)      DISPOSE((point))
#endif

/*
 * Description macros.
 */
#define PERS(ch, looker)   ( can_see( looker, (ch) ) ?      \
            ( IS_NPC(ch) ? (ch)->short_descr   \
            : (ch)->name ) : "kto¶" )
#define PERS2(ch, looker)   ( can_see( looker, (ch) ) ?      \
            (ch)->name2 : "kogo¶" )
#define PERS3(ch, looker)   ( can_see( looker, (ch) ) ?      \
            (ch)->name3 : "komu¶" )
#define PERS4(ch, looker)   ( can_see( looker, (ch) ) ?      \
            (ch)->name4 : "kogo¶" )
#define PERS5(ch, looker)   ( can_see( looker, (ch) ) ?      \
            (ch)->name5 : "kim¶" )
#define PERS6(ch, looker)   ( can_see( looker, (ch) ) ?      \
            (ch)->name6 : "kim¶" )



#define OBJ_NEXT_CONTENT( ob, ob_next )    ( { \
                                                if ( !ob ) break; \
                                                ob_next = ob->next_content; \
                                             } )

#define OBJ_NEXT( ob, ob_next )            ( { \
                                               if ( !ob ) break; \
                                               ob_next = ob->next; \
                                             } )

#define STANDARD_PROMPT	"<%h/%Hhp %x %v/%Vmv %s> %r %e %t"
/*
 * Structure for a social in the socials table.
 */
struct   social_data
{
	SOCIAL_DATA * next;
    char *    name;
    char *    char_no_arg;
    char *    others_no_arg;
    char *    char_found;
    char *    others_found;
    char *    vict_found;
    char *    char_not_found;
    char *    char_auto;
    char *    others_auto;
};

/*
 * Global constants.
 */
 /*pogoda and stuff*/
extern          WEATHER_DATA            weather_info[SECT_MAX];
extern   		char * 		const      	month_name[ ];
extern   		char * 		const      	day_name[ ];
extern   		char * 		const      	hour_name[ ];

/*********************/
extern  const   struct   str_app_type   str_app      [256];
extern  const   struct   int_app_type   int_app      [34];
extern  const   struct   wis_app_type   wis_app      [34];
extern  const   struct   dex_app_type   dex_app      [34];
extern  const   struct   con_app_type   con_app      [34];

extern  const   struct   class_type   class_table   [MAX_CLASS];
extern  const   struct  material_type   material_table   [];
extern  const   struct  poison_type   poison_table   [MAX_POISON+1];
extern  const   struct  armor_type   armor_table   [];
extern  const   SECT_DATA      sector_table   [];
extern  const   struct   weapon_type   weapon_table   [];
extern  const   struct   instrument_type   instrument_table   [];
extern  const   struct  item_type   item_table   [];
extern  const   struct   wiznet_type   wiznet_table   [];
extern  const   struct   attack_type   attack_table   [];
extern  const   struct  race_type   race_table   [];
extern  const   struct   pc_race_type   pc_race_table   [];
extern  const   struct   spec_type   spec_table   [];
extern  const   struct   liq_type   liq_table   [];
extern  const   struct   weed_type   weed_table   [];
extern  const   struct   skill_type   skill_table   [MAX_SKILL];
extern  const   struct   school_type   school_table   [MAX_SCHOOL];
extern  const   struct  spell_msg   msg_table   [];
extern	ROOM_UPDATE_LIST room_update_list;
extern	const   struct  ShieldType    ShieldList[];
extern  const   struct  wand_type   wand_table[];
extern	const	int 	wear_order [];
extern  const   struct  herb_type herb_table[];
extern  const 	struct  monster_body_parts	body_parts_table[];
extern  const   struct  trick_type   trick_table   [MAX_TRICKS];

/**
 * rellik: mining + tools using
 */
struct tool_type
{
	char *name;
	int bit;
};
struct rawmaterial_type
{
	char *name;
	int bit;
	int skill;
	int vnum;
	int vnum_big;
};
extern const struct tool_type tool_table[];
extern const struct rawmaterial_type rawmaterial_table[];


/*
 * Eventowe pierdo³y, timer.c
 */
void  printf_to_world(char *, ...);
void  printf_to_room    args( (ROOM_INDEX_DATA *, char *, ...) );
char *   nsprintf       args( (char *, char *, ...) );
void  wait_wait         args( (CHAR_DATA *, int, int) );
void  wait_printf       args( (CHAR_DATA *, int, char *, ...) );
void  wait_act          args( (int, char *, void *, void *, void *, int));
void  wait_function     args( (CHAR_DATA *ch,int delay, DO_FUN *do_fun, char *argument) );
void  wait_obj_to_room   args( ( OBJ_DATA *obj, int delay, ROOM_INDEX_DATA *room ) );
void  wait_mob_to_room   args( ( CHAR_DATA *obj, int delay, ROOM_INDEX_DATA *room ) );
CHAR_DATA *wait_mob_to_room_by_vnum args( (int vnum, int delay, ROOM_INDEX_DATA *room));
extern EVENT2_DATA *create_event2 (int, char *);


/*
 * Global variables.
 */
extern      HELP_DATA     *   help_first;
extern      SHOP_DATA     *   shop_first;
extern      BANK_DATA     *   bank_first;

extern      CHAR_DATA     *   char_list;

extern          ARTEFACT_DATA     *     artefact_system;

extern      LEARN_DATA     *   learn_system;
extern      LEARN_TRICK_DATA     *   learn_trick_system;
extern      BOARD_DATA     *   board_system;
extern      SPELL_MSG *      spellmsg_list;
extern      PRE_WAIT     *   pre_waits;

extern      DESCRIPTOR_DATA   *   descriptor_list;
extern      OBJ_DATA     *   object_list;

extern      PROG_CODE     *   mprog_list;
extern      PROG_CODE     *   oprog_list;
extern      PROG_CODE     *   rprog_list;
extern      TRAP_DATA     *   trap_list;
extern      RAND_DESC_DATA     *   rand_desc_list;
extern      BONUS_INDEX_DATA     *   bonus_list;
extern      SONG_DATA     *   song_list;

extern      char         bug_buf      [];
extern      time_t         current_time;
extern      bool         fLogAll;
extern      bool         fLogComm;
extern		bool		wizlock;
extern		bool		newlock;

extern      FILE *         fpReserve;
extern      KILL_DATA      kill_table   [];
extern      char         log_buf      [];
extern      TIME_INFO_DATA      time_info;
extern      bool         MOBtrigger;
extern      bool         OBJtrigger;
extern      MISC_DATA       misc;
extern      INFO_DESCRIPTOR_DATA   * info_descriptor_list;
extern      const char *          real_day_names[7][2];
extern      const char *          real_month_names[12][2];
extern      SOCIAL_DATA *         social_list;
extern      int                   in_thread;
extern      SPIRIT_DATA *   	spirits;

/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if   defined(_AIX)
char *   crypt      args( ( const char *key, const char *salt ) );
#endif

#if   defined(apollo)
int   atoi      args( ( const char *string ) );
void *   calloc      args( ( unsigned nelem, size_t size ) );
char *   crypt      args( ( const char *key, const char *salt ) );
#endif

#if   defined(hpux)
char *   crypt      args( ( const char *key, const char *salt ) );
#endif

#if   defined(linux)
char *   crypt      args( ( const char *key, const char *salt ) );
#endif

#if   defined(macintosh)
#define NOCRYPT
#if   defined(unix)
#undef   unix
#endif
#endif

#if   defined(MIPS_OS)
char *   crypt      args( ( const char *key, const char *salt ) );
#endif

#if   defined(MSDOS)
#define NOCRYPT
#if   defined(unix)
#undef   unix
#endif
#endif

#if   defined(NeXT)
char *   crypt      args( ( const char *key, const char *salt ) );
#endif

#if   defined(sequent)
char *   crypt      args( ( const char *key, const char *salt ) );
int   fclose      args( ( FILE *stream ) );
int   fprintf      args( ( FILE *stream, const char *format, ... ) );
int   fread      args( ( void *ptr, int size, int n, FILE *stream ) );
int   fseek      args( ( FILE *stream, long offset, int ptrname ) );
void   perror      args( ( const char *s ) );
int   ungetc      args( ( int c, FILE *stream ) );
#endif

#if   defined(sun)
char *   crypt      args( ( const char *key, const char *salt ) );
int   fclose      args( ( FILE *stream ) );
int   fprintf      args( ( FILE *stream, const char *format, ... ) );
#if   defined(SYSV)
siz_t   fread      args( ( void *ptr, size_t size, size_t n,
             FILE *stream) );
#elif !defined(__SVR4)
int   fread      args( ( void *ptr, int size, int n, FILE *stream ) );
#endif
int   fseek      args( ( FILE *stream, long offset, int ptrname ) );
void   perror      args( ( const char *s ) );
int   ungetc      args( ( int c, FILE *stream ) );
#endif

#if   defined(ultrix)
char *   crypt      args( ( const char *key, const char *salt ) );
#endif



/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if   defined(NOCRYPT)
#define crypt(s1, s2)   (s1)
#endif



/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined(macintosh)
#define PLAYER_DIR   ""         /* Player files   */
#define RESERVATION_DIR ""        /* Rezerwacja imion */
#define TEMP_FILE   "romtmp"
#define NULL_FILE   "proto.are"      /* To reserve one stream */
#endif

#if defined(MSDOS)
#define PLAYER_DIR   ""         /* Player files */
#define RESERVATION_DIR ""        /* Rezerwacja imion */
#define TEMP_FILE   "romtmp"
#define NULL_FILE   "nul"         /* To reserve one stream */
#endif

#if defined(unix)
#define PLAYER_DIR      "../player/"           /* Player files */
#define ACCOUNT_DIR      "../player/account/"           /* Player files */
#define GOD_DIR         "../gods/"        /* list of gods */
#define RESERVATION_DIR "../reservation/"           /* Rezerwacja imion */
#define TEMP_FILE   "../player/romtmp"
#define NULL_FILE   "/dev/null"      /* To reserve one stream */
#endif

#define AREA_LIST         "area.lst"  /* List of areas*/
#define BUG_FILE          "../system/bugs.txt" /* For 'bug' and bug()*/
#define TYPO_FILE         "../system/typos.txt" /* For 'typo'*/
#define HELP_FILE         "../system/help.txt" /* For 'help whithout topic'*/
#define NOTE_FILE         "../system/notes.not"/* For 'notes'*/
#define IDEA_FILE         "../system/ideas.txt"
#define PENALTY_FILE      "../system/penal.not"
#define NEWS_FILE         "../system/news.not"
#define CHANGES_FILE      "../system/chang.not"
#define SHUTDOWN_FILE     "shutdown.txt"/* For 'shutdown'*/
#define BAN_FILE          "../system/ban.txt"
#define MUSIC_FILE        "../system/music.txt"
#define LEARN_FILE        "../system/learn.txt"
#define LEARN_TRICK_FILE  "../system/learn_trick.txt"
#define PREWAIT_FILE      "../system/prewait.txt"
#define SPELLMSG_FILE     "../system/spell_msg.txt"
#define SPELLS_FILE       "../system/spells_data.txt"
#define PCCORPSES_FILE    "../system/pccorpses.txt"
#define ARTEFACT_FILE     "../system/artefact.txt"
#define MISC_FILE         "../system/miscdata.txt"
#define RENT_COST_LOG     "../system/rent-cost.txt"
/**
 * logs
 */
#define BUILD_LOG_FILE    "../log/build/build-%s.txt"
#define MONEY_LOG_FILE    "../log/money/money-%s.txt"
#define SPEECH_LOG_FILE   "../log/speech/speech-%s.txt"
#define ARTEFACT_LOG_FILE "../log/artefact/artefact-%s.txt"
#define REGENT_LOG_FILE   "../log/regent/regent-%s.txt"
#define EXP_LOG_FILE      "../log/expirence/expirence-%s.txt"
#define LEARN_LOG_FILE    "../log/learn/learn-%s.txt"
#define DAM_LOG_FILE      "../log/damage/dam-%s.txt"

#define GENERIC_ADDRESS "killer.mud.pl"
#define WWW_ADDRESS     "http://www.killer.mud.pl/"
#define WWW_BLOG        "http://blog.killer.mud.pl/"
#define FORUM_ADDRESS   "http://forum.mud.pl/?c=8"
#define MAIL_ADDRESS    "killer.support@iworks.pl"
#define TELNET_ADDRESS  "telnet://killer.mud.pl:4000/"

/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD   CHAR_DATA
#define MID   MOB_INDEX_DATA
#define OD   OBJ_DATA
#define OID   OBJ_INDEX_DATA
#define RID   ROOM_INDEX_DATA
#define SF   SPEC_FUN
#define AD   AFFECT_DATA
#define PGC   PROG_CODE

/*signals.c*/
void    init_signals   args(());
/************/
/* act_comm.c */
int get_rent_rate    args( ( CHAR_DATA *ch ) );
void     check_sex   args( ( CHAR_DATA *ch ) );
void   add_follower   args( ( CHAR_DATA *ch, CHAR_DATA *master, bool strip_weapon_bonus ) );
void   stop_follower   args( ( CHAR_DATA *ch ) );
void   die_follower   args( ( CHAR_DATA *ch, bool charm ) );
bool   is_same_group   args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void    logformat       args((char * fmt, ...));
#ifdef NEW_NANNY
bool	new_class_ok	args( ( NEW_CHAR_DATA * new_ch, int class ) );
#endif
bool	class_ok		args( ( CHAR_DATA *ch , int class) );

/* act_enter.c */
RID  *get_random_room   args ( (CHAR_DATA *ch) );

/* act_info.c */
void   set_title   args( ( CHAR_DATA *ch, char *title ) );
void   set_new_title   args( ( CHAR_DATA *ch, char *title ) );
char*   is_name_reserved    args( ( char *name ) );
int get_hp_notch args ( ( CHAR_DATA *ch, bool usePerfectSelf ) );
void do_hoard_list args (( CHAR_DATA *ch ));

/* act_move.c */
void   move_char   args( ( CHAR_DATA *ch, int door, bool follow, OBJ_DATA *dragged ) );
bool    check_vname args( ( CHAR_DATA *ch, int door, bool verbose ) );
int     get_door    args( ( CHAR_DATA *ch, char* argument ) );
bool	mind_move_room	args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );

/* act_obj.c */
void   wear_obj   args( (CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace) );
void    wield_weapon    args( (CHAR_DATA *ch, OBJ_DATA *obj, bool primary));
void    get_obj         args( ( CHAR_DATA *ch, OBJ_DATA *obj,
                            OBJ_DATA *container ) );
int     get_rent_cost     args( (CHAR_DATA *ch, OBJ_DATA *object, bool test));
int     get_hoard_cost     args( (CHAR_DATA *ch, OBJ_DATA *object, bool test));
int     get_hand_slots  args( ( CHAR_DATA *ch, int wear ) );


/* act_wiz.c */
void wiznet      args( (char *string, CHAR_DATA *ch, OBJ_DATA *obj,
                long flag, long flag_skip, int min_level ) );
void copyover_recover args((void));
bool sifollow_remove_char( CHAR_DATA* ch, CHAR_DATA* victim );

/* alias.c */
void    substitute_alias args( (DESCRIPTOR_DATA *d, char *input) );

/* ban.c */
bool   check_ban   args( ( char *site, int type) );
void   update_bans   args( ( void ) );


/* comm.c */
void   show_string     args( ( struct descriptor_data *d, char *input) );
void   close_socket    args( ( DESCRIPTOR_DATA *dclose, int options ) );
void   write_to_buffer args( ( DESCRIPTOR_DATA *d, const char *txt, int length ) );
void   write_to_buffer args( ( DESCRIPTOR_DATA *d, const char *txt, int length ) );
void   send_to_char    args( ( const char *txt, CHAR_DATA *ch ) );
void   page_to_char    args( ( const char *txt, CHAR_DATA *ch ) );
char * get_zaimek      args( ( CHAR_DATA *ch, CHAR_DATA *vch, char format ) );
void   act             args( ( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type ) );
void   act_new         args( ( const char *format, CHAR_DATA *ch, void *arg1, void *arg2, int type, int min_pos) );


/*
 * Colour stuff by Lope
 */
int   colour      args( ( char type, CHAR_DATA *ch, char *string ) );
int   simple_colour      args( ( char type, char *string ) );
int   colour_back   args( ( char type, char *string ) );
void   colourconv   args( ( char *buffer, const char *txt, CHAR_DATA *ch ) );
void   send_to_char_bw   args( ( const char *txt, CHAR_DATA *ch ) );
void   page_to_char_bw   args( ( const char *txt, CHAR_DATA *ch ) );
void   printf_to_char   args( ( CHAR_DATA *, char *, ... ) );
void   bugf      args( ( char *, ... ) );
int     sexconv     args( ( CHAR_DATA *ch, const char *str_src, char *str_dest ) );
void    send_to_desc args( ( DESCRIPTOR_DATA *d, const char *txt, bool colour ) );
void 	print_desc	args( ( DESCRIPTOR_DATA *d, bool colour, char *fmt, ...) );

/* db.c */
void   reset_area      args( ( AREA_DATA * pArea ) );      /* OLC */
void   reset_room   args( ( ROOM_INDEX_DATA *pRoom ) );   /* OLC */
char *   print_flags   args( ( int flag ));
char *   print_ext_flags   args( ( long *flags ));
void   boot_db      args( ( void ) );
void   area_update   args( ( void ) );
CD *   create_mobile   args( ( MOB_INDEX_DATA *pMobIndex ) );
void   clone_mobile   args( ( CHAR_DATA *parent, CHAR_DATA *clone) );
OD *   create_object   args( ( OBJ_INDEX_DATA *pObjIndex, bool remote) );
void   clone_object   args( ( OBJ_DATA *parent, OBJ_DATA *clone ) );
void   clear_char   args( ( CHAR_DATA *ch ) );
char *   get_extra_descr   args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *   get_mob_index   args( ( unsigned int vnum ) );
OID *   get_obj_index   args( ( unsigned int vnum ) );
RID *   get_room_index   args( ( unsigned int vnum ) );
PGC *   get_mprog_index args( ( char * name ) );
PGC *   get_oprog_index args( ( char * name ) );
PGC *   get_rprog_index args( ( char * name ) );
BONUS_INDEX_DATA *get_bonus_index args( ( unsigned int vnum ) );
SONG_DATA *get_song_index   args( ( ush_int number ) );
HERB_DATA *	get_herb_index	args( ( char * id ) );
TRAP_DATA * get_trap_index args( ( unsigned int vnum ) );
RAND_DESC_DATA * get_rdesc_index args( ( unsigned int vnum ) );
DESC_DATA * get_desc_index args( ( AREA_DATA* pArea, char* name ) );
int next_nonspace_before_newline( FILE *fp );
char   fread_letter   args( ( FILE *fp ) );
int     fread_number   args( ( FILE *fp ) );
long    fread_long_number   args( ( FILE *fp ) );
long    fread_flag   args( ( FILE *fp ) );
void fread_ext_flag( FILE *fp, int *bank, long *vector );
void fread_ext_flags( FILE *fp, long *flags );
char *   fread_string   args( ( FILE *fp ) );
char *  fread_string_eol args(( FILE *fp ) );
void   fread_to_eol   args( ( FILE *fp ) );
char *   fread_word   args( ( FILE *fp ) );
long   flag_convert   args( ( char letter) );
char *   str_dup      args( ( const char *str ) );
int   number_fuzzy   args( ( int number ) );
int   number_range   args( ( int from, int to ) );
inline long int   URANGE         args( ( long int low, long int n, long int high ) );
inline long int   UMIN         args( ( long int a, long int b ) );
inline long int   UMAX         args( ( long int a, long int b ) );
int   number_percent   args( ( void ) );
int   number_door   args( ( void ) );
int   number_bits   args( ( int width ) );
long     number_mm       args( ( void ) );
int   dice      args( ( int number, int size ) );
int   interpolate   args( ( int level, int value_00, int value_32 ) );
void   smash_tilde   args( ( char *str ) );
bool   str_cmp      args( ( const char *astr, const char *bstr ) );
bool   str_prefix   args( ( const char *astr, const char *bstr ) );
bool   str_infix   args( ( const char *astr, const char *bstr ) );
bool   str_suffix   args( ( const char *astr, const char *bstr ) );
//char *   capitalize   args( ( const char *str ) );
char *   uncapitalize   args( (const char *str ) );
void   append_file   args( ( CHAR_DATA *ch, char *file, char *str ) );
void    append_file_format  args( ( CHAR_DATA *ch, char *file, char *fmt, ... ) );
void    append_file_format_daily  args( ( CHAR_DATA *ch, char *file, char *fmt, ... ) );
void   bug      args( ( const char *str, int param ) );
//rellik: do debugowania
void save_debug_info	args( ( const char *actual, const char *caller, char *txt,	int nr_proj, int debuglevel, bool include_info ) );
void   log_string   args( ( const char *str ) );
void   tail_chain   args( ( void ) );
char *   capitalize_first   args( ( const char *str ) );
#define capitalize( string )   capitalize_first( string )
void    save_misc_data args( ( void ) );

bool are_lists_equal( void * obj1, void * obj2, bool obj_equal( void*, void* ), void * get_next( void * ) );
bool check_specdam_equal( void *a, void * b);
void * get_next_specdam( void * obj );

/* BEGIN: reward (autoquest) */
char *	lowercase			args( ( const char *str ) );
char *	decap				args( ( const char *str ) );
char *	wrapstr				args( ( CHAR_DATA *ch, const char *str ) );
/* END: reward (autoquest) */

/* effect.c */
void   acid_effect   args( (void *vo, int level, int dam, int target) );
void   cold_effect   args( (void *vo, int level, int dam, int target) );
void   fire_effect   args( (void *vo, int level, int dam, int target) );
void   poison_effect args( (void *vo, int level, int dam, int target) );
void   shock_effect  args( (void *vo, int level, int dam, int target) );
void   sound_effect  args( (void *vo, int level, int dam, int target) );

/* fight.c */
int    GET_AC(CHAR_DATA *ch, int type);
bool    is_safe      args( (CHAR_DATA *ch, CHAR_DATA *victim, bool show_message ) );
bool    is_safe_spell   args( (CHAR_DATA *ch, CHAR_DATA *victim, bool area ) );
void   violence_update   args( ( void ) );
void   multi_hit   args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
bool   damage      args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                 int dt, int class, bool show) );
bool   spell_damage      args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                 int dt, int class, bool show) );
bool    damage_old      args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                                int dt, int class, bool show ) );
void   update_pos   args( ( CHAR_DATA *victim ) );
void   stop_fighting   args( ( CHAR_DATA *ch, bool fBoth ) );
void   check_killer   args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
int   compute_tohit   args( (CHAR_DATA *ch, bool primary ) );

/* handler.c */
/*IMPROVEMENT: Brohacz: zwraca slowo 'godzina' w odpowiedniej formie: (za 1) godzinê, (za 2) godziny...*/
const char*  hour_to_str( int value );
AD     *affect_find args( (AFFECT_DATA *paf, int sn));
int		after_death args( ( CHAR_DATA *ch ) );
void   affect_check   args( (CHAR_DATA *ch, int where, BITVECT_DATA * vector ) );
int   count_users   args( (OBJ_DATA *obj) );
void   affect_enchant   args( (OBJ_DATA *obj) );
int    check_immune   args( (CHAR_DATA *ch, int dam_type) );
int    check_resist   args( (CHAR_DATA *ch, int dam_type, int dam) );
char *   resist_name   args( (int type) );
int   resist_number   args( (char *name) );
int    material_lookup args( ( const char *name) );
int   weapon_lookup   args( ( const char *name) );
int   instrument_lookup   args( ( const char *name) );
int   weapon_type   args( ( const char *name) );
int   instrument_type   args( ( const char *name) );
char    *weapon_name   args( ( int weapon_Type) );
char    *instrument_name   args( ( int instrument_Type) );
char   *item_name   args( ( int item_type) );
int   attack_lookup   args( ( const char *name) );
long   wiznet_lookup   args( ( const char *name) );
int   class_lookup   args( ( const char *name) );
bool   is_old_mob   args ( (CHAR_DATA *ch) );
int   get_skill   args( ( CHAR_DATA *ch, int sn ) );
int   get_weapon_sn   args( ( CHAR_DATA *ch, bool primary ) );
int   get_musical_instrument_sn   args( ( CHAR_DATA *ch ) );
int   weapon_sn   args( ( OBJ_DATA *weapon ) );
int   musical_instrument_sn   args( ( OBJ_DATA *musical_instrument ) );
int   get_weapon_skill args(( CHAR_DATA *ch, int sn ) );
int   get_musical_instrument_skill args(( CHAR_DATA *ch, int sn ) );
int     get_age         args( ( CHAR_DATA *ch ) );
void   reset_char   args( ( CHAR_DATA *ch )  );
int   get_trust   args( ( CHAR_DATA *ch ) );
int   get_curr_stat_deprecated   args( ( CHAR_DATA *ch, int stat ) );
int   get_max_hp   args( ( CHAR_DATA *ch ) );
#ifdef NEW_NANNY
char *new_get_class_name	args( ( NEW_CHAR_DATA * new_ch ) );
#endif
char * get_class_name   args( ( CHAR_DATA *ch ) );
int get_class	( CHAR_DATA *ch, int default_class );
int    get_max_train   args( ( CHAR_DATA *ch, int stat ) );
int   can_carry_n   args( ( CHAR_DATA *ch ) );
int   can_carry_w   args( ( CHAR_DATA *ch ) );
bool   is_name      args( ( char *str, char *namelist ) );
bool   is_exact_name   args( ( char *str, char *namelist ) );
void   affect_to_char   args( ( CHAR_DATA *ch, AFFECT_DATA *paf, char *info, bool visible ) );
void   affect_to_obj   args( ( OBJ_DATA *obj, AFFECT_DATA *paf ) );
void   affect_remove   args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void   affect_remove_obj args( (OBJ_DATA *obj, AFFECT_DATA *paf ) );
void   affect_strip   args( ( CHAR_DATA *ch, int sn ) );
void   affect_strip_partial   args( ( CHAR_DATA *ch, int sn, int flags ) );
void   strip_invis   args( ( CHAR_DATA *ch, bool show_message, bool strip_nondetection_spell ) );
bool   is_affected   args( ( CHAR_DATA *ch, int sn ) );
void   affect_join   args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void   char_from_room   args( ( CHAR_DATA *ch ) );
void   char_to_room   args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void   obj_to_char   args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void   obj_from_char   args( ( OBJ_DATA *obj ) );
void   obj_from_hoard args(( OBJ_DATA * obj ));
bool   carring_to_hoard args(( OBJ_DATA *obj, CHAR_DATA *ch, bool check_capacity ));
int    apply_ac   args( ( OBJ_DATA *obj, int iWear, int type ) );
OD *   get_eq_char   args( ( CHAR_DATA *ch, int iWear ) );
bool   can_equip_obj   args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
bool   equip_char   args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear, bool checkwear ) );
void   unequip_char   args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int    count_obj_list   args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void   obj_from_room   args( ( OBJ_DATA *obj ) );
void   obj_to_room   args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void   obj_to_obj   args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void   obj_from_obj   args( ( OBJ_DATA *obj ) );
void   extract_obj   args( ( OBJ_DATA *obj ) );
void   extract_char   args( ( CHAR_DATA *ch, bool fPull ) );
CD *   get_char_room   args( ( CHAR_DATA *ch, char *argument ) );
CD *   get_char_world   args( ( CHAR_DATA *ch, char *argument ) );
OD *   get_obj_type   args( ( ROOM_INDEX_DATA *pRoom, OBJ_INDEX_DATA *pObjIndexData ) );
OD *   get_obj_list   args( ( CHAR_DATA *ch, char *argument, OBJ_DATA *list ) );
OD *   get_obj_carry   args( ( CHAR_DATA *ch, char *argument, CHAR_DATA *viewer ) );
OD *   get_obj_wear   args( ( CHAR_DATA *ch, char *argument, bool dont_check_can_see ) );
OD *   get_obj_wear_r   args( ( CHAR_DATA *ch, char *argument, int number_r, int start ) );
OD *   get_obj_here   args( ( CHAR_DATA *ch, char *argument ) );
OD *   get_obj_here_buggy   args( ( CHAR_DATA *ch, char *argument ) );
OD *   get_obj_here_alt   args( ( CHAR_DATA *ch, char *argument ) );
OD *   get_obj_world   args( ( CHAR_DATA *ch, char *argument ) );
OD *   create_money   args( ( int cooper, int silver, int gold, int mithril ) );
OD *   get_obj_hoard  args((CHAR_DATA *ch, char *argument ));
int   get_obj_number   args( ( OBJ_DATA *obj ) );
int   get_obj_weight   args( ( OBJ_DATA *obj ) );
int   get_true_weight   args( ( OBJ_DATA *obj ) );
bool   room_is_dark   args( ( CHAR_DATA * ch, ROOM_INDEX_DATA *pRoomIndex ) );
bool   is_room_owner   args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room) );
bool   room_is_private   args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool   can_see      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    can_move   args( ( CHAR_DATA *ch )  );
bool   can_see_obj   args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool   can_see_room   args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex) );
bool   can_drop_obj   args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
char *   affect_loc_name   args( ( int location ) );
char *   affect_bit_name   args( ( /*int vector*/CHAR_DATA *ch, BITVECT_DATA * vector) );
char *   extra_bit_name   args( ( int extra_flags ) );
char *   extra2_bit_name   args( ( int extra_flags2 ) );
char *    wear_bit_name   args( ( int wear_flags ) );
char *    wear2_bit_name   args( ( int wear_flags ) );
char *    form_bit_name   args( ( int form_flags ) );
char *   part_bit_name   args( ( int part_flags ) );
char *   weapon_bit_name   args( ( int weapon_flags ) );
char *  comm_bit_name   args( ( int comm_flags ) );
char *   cont_bit_name   args( ( int cont_flags) );
bool   knows_spell   args( ( CHAR_DATA *ch, sh_int sn ) );
bool   knows_spell_book   args( ( CHAR_DATA *ch, sh_int sn ) );
void   poison_to_char   args( (CHAR_DATA *victim, int poison) );
bool    send_mail_message   args( ( const char* to, const char* subject, const char* text ) );
bool    player_exists   args( ( char * name ) );
void    bonus_remove    args( ( CHAR_DATA * ch, BONUS_DATA * bonus ) );
void    change_player_name	args( ( CHAR_DATA * ch, char * new_name ) );
void apply_shamanistic_guide	args( ( CHAR_DATA * ch ) );
OBJ_DATA * find_boat	args( (CHAR_DATA *ch) );
int get_curr_stat args ( ( CHAR_DATA *ch, int stat ) );
SPIRIT_DATA * 	add_spirit	args( ( CHAR_DATA *ch, OBJ_DATA *corpse ) );
void 		del_spirit	args( ( CHAR_DATA *ch ) );
SPIRIT_DATA *	get_spirit	args( ( CHAR_DATA *ch ) );

/*
 * Colour Config
 */
void   default_colour   args( ( CHAR_DATA *ch ) );
void   all_colour   args( ( CHAR_DATA *ch, char *argument ) );
bool    stat_throw      args( ( CHAR_DATA *ch, int stat ) );
bool    ch_vs_victim_stat_throw args( ( CHAR_DATA *ch, CHAR_DATA *victim, int stat ) );
/* interp.c */
void   interpret   args( ( CHAR_DATA *ch, char *argument ) );
bool   is_number   args( ( char *arg ) );
int   number_argument   args( ( char *argument, char *arg ) );
int   mult_argument   args( ( char *argument, char *arg) );
char *   one_argument   args( ( char *argument, char *arg_first ) );
char *   case_one_argument   args( ( char *argument, char *arg_first ) );
/* ten drugi nie obcina duzych liter*/

/* board.c */

void    show_contents   args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
void    save_board   args( ( CHAR_DATA * ch,unsigned int vnum)    );
void    load_board   args( (BOARD_DATA *board));
/* magic.c */
int   find_skill   args( ( CHAR_DATA *ch, const char *name, bool must_know ) );
int   find_spell_new   args( ( CHAR_DATA *ch, char *name, bool must_know ) );
int   skill_lookup   args( ( const char *name ) );
int skill_only_lookup args( ( const char *name ) );
int spell_only_lookup args( ( const char *name ) );
bool   is_skill_lookup   args( ( const char *name ) );
bool   saves_spell   args( ( int level, CHAR_DATA *victim, int dam_type ) );
bool   saves_spell_new args( (CHAR_DATA *ch, int type, sh_int mod, CHAR_DATA *subject, int sn ) );
void   obj_cast_spell   args( ( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj ) );

/* reward.c */
void	reset_hunt		args( ( CHAR_DATA *ch ) );
CD *	find_rewarder	args( ( CHAR_DATA *ch ) );
CD *	find_captain	args( ( CHAR_DATA *ch ) );
void	find_missing	args( ( CHAR_DATA *ch, CHAR_DATA *captain ) );

/* memspell.c */
bool    add_mem         args( (CHAR_DATA *ch, sh_int spell));
void    load_mem   args( (CHAR_DATA *ch, MSPELL_DATA *mem));
bool   remove_mem      args( (CHAR_DATA *ch, MSPELL_DATA * mspell));
void    remove_not_known args( ( CHAR_DATA *ch ) );
void    mem_done        args( (CHAR_DATA *ch));
void	mem_done_all	( CHAR_DATA *ch );
int		count_mspell	( CHAR_DATA * ch, bool all, bool done );
bool    spell_memmed    args( (CHAR_DATA *ch,sh_int spell) );
sh_int    spell_circle   args( (CHAR_DATA *ch, sh_int spell));
void    clear_mem   args( (CHAR_DATA *ch));/*do scinania przy zgonie*/
int    chance_to_learn_spell args( (CHAR_DATA *ch, sh_int spell) );
void    clear_spells_failed   args( (CHAR_DATA *ch) );
void    add_spells_failed   args( (CHAR_DATA *ch, CHAR_DATA *mob, sh_int spell) );
bool    can_learn_here      args( (CHAR_DATA *ch, CHAR_DATA *mob, sh_int spell) );
void    load_spells_failed   args( (CHAR_DATA *ch, unsigned int mob_vnum, sh_int spell) );
bool   can_learn_spell      args( (CHAR_DATA *ch, sh_int spell) );
void   echo_mem      args( (CHAR_DATA *ch, int action) );
void	mspell_update	args( ( void ) );
MSPELL_DATA *	get_mspell_by_sn	( CHAR_DATA * ch, int sn, bool done );
void	get_next_memming	( CHAR_DATA * ch );
bool	can_mem_now			args( ( CHAR_DATA * ch ));

void    save_memset     args( ( CHAR_DATA *ch, char *memset_name, bool overwrite_old ) );
void    load_memset     args( ( CHAR_DATA *ch, char *memset_name ) );
bool    remove_memset   args( ( CHAR_DATA *ch, char *memset_name, bool quiet ) );
void    list_memsets    args( ( CHAR_DATA *ch ) );


/* mob_prog.c */
void   mp_act_trigger   args( ( char *argument, CHAR_DATA *mob, CHAR_DATA *ch,
            void *arg1, void *arg2, BITVECT_DATA *type ) );
bool   mp_percent_trigger args( ( CHAR_DATA *mob, CHAR_DATA *ch,
            void *arg1, void *arg2, BITVECT_DATA *type ) );
void   mp_bribe_trigger  args( ( CHAR_DATA *mob, CHAR_DATA *ch, long int amount ) );
bool   mp_exit_trigger   args( ( CHAR_DATA *ch, int dir ) );
void   mp_give_trigger   args( ( CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj ) );
void   mp_greet_trigger  args( ( CHAR_DATA *ch ) );
void   mp_hprct_trigger  args( ( CHAR_DATA *mob, CHAR_DATA *ch ) );


/* obj_prog.c */
bool    op_put_trigger       args( ( OBJ_DATA *obj,CHAR_DATA *ch, void *arg1, BITVECT_DATA *type ) );
bool    op_common_trigger    args( ( CHAR_DATA *ch, OBJ_DATA *obj, BITVECT_DATA *type ) );
bool    op_consume_trigger   args( (OBJ_DATA *obj, CHAR_DATA *ch) );
bool    op_connect_trigger   args( (OBJ_DATA *obj, CHAR_DATA *ch) );
void    op_time_trigger      args( ( OBJ_DATA *obj, int time ) );
bool    op_input_trigger     args( ( CHAR_DATA *ch, char *string ) );
void	op_act_trigger		 args( ( char *argument, CHAR_DATA *ch, bool room_enabled ) );

/* room_prog.c */
void rp_random_trigger			args( ( ROOM_INDEX_DATA *pRoom, BITVECT_DATA *type ) );
void rp_time_trigger			args( ( ROOM_INDEX_DATA *room, int time ) );
void rp_delay_trigger			args( ( CHAR_DATA *ch ) );
void rp_enter_trigger			args( ( CHAR_DATA *ch ) );
bool rp_exit_trigger			args( ( CHAR_DATA *ch, int door ) );
bool rp_input_trigger			args( ( CHAR_DATA *ch, char * string ) );
bool rp_pos_trigger				args( ( CHAR_DATA *ch,OBJ_DATA *obj, BITVECT_DATA *type ) );
void rp_act_trigger				args( ( char *argument, CHAR_DATA *ch, BITVECT_DATA *type ) );
bool rp_locks_trigger			args( ( CHAR_DATA *ch, int exit, BITVECT_DATA *type ) );
bool rp_knock_trigger			args( ( CHAR_DATA *ch, int door ) );


/* account.c */
bool    load_account            args( ( ACCOUNT_DATA **account, char * email ) );
void    save_account            args( ( ACCOUNT_DATA * account ) );
bool    account_exists          args( ( char * account ) );
bool    validate_email          args( ( const char* email ) );
bool    send_mail_message       args( ( const char* to, const char* subject, const char* text ) );
bool    add_player_to_account   args( ( ACCOUNT_DATA *account, char *name ) );
void    remove_player_from_account   args( ( ACCOUNT_DATA *account, char *name ) );
char*   check_account_playing   args( ( DESCRIPTOR_DATA *dcheck ) );
void    change_account_email    args( ( ACCOUNT_DATA *account, char *new_email ) );
void    generate_password       args( ( char * result, int length ) );

bool    check_password          args( ( DESCRIPTOR_DATA *d, char *indent, char * password ) );

/* hints.c */
void   hint_update   args( ( void ) );

/* progs */
bool    check_trigger   args( (int prog_type, char *name ));
/*********/

/* save.c */
void   save_char_obj   args( ( CHAR_DATA *ch, bool save_newbies, bool remote ) );
CHAR_DATA * load_char_remote args( ( char *name ) );
CHAR_DATA* init_char	args( ( char *name ) );
bool   load_char_obj   args( ( DESCRIPTOR_DATA *d, char *name, bool remote ) );
void    save_pccorpses  args( ( void ) );
void    load_pccorpses  args( ( void ) );
void    add_host        args( ( CHAR_DATA *ch, char * host, bool insert ) );

/* skills.c */
bool    parse_gen_groups args( ( CHAR_DATA *ch,char *argument ) );
void    list_group_costs args( ( CHAR_DATA *ch ) );
void    list_group_known args( ( CHAR_DATA *ch ) );
/*int    exp_per_level   args( ( CHAR_DATA *ch, int points ) );*/
long exp_per_level   args( ( CHAR_DATA *ch,int level ) );
void    check_improve   args( ( CHAR_DATA *ch, CHAR_DATA *victim, int sn, bool success,
                int multiplier ) );
int    group_lookup   args( (const char *name) );
void   gn_add      args( ( CHAR_DATA *ch, int gn) );
void    gn_remove   args( ( CHAR_DATA *ch, int gn) );
void    group_add   args( ( CHAR_DATA *ch, const char *name, bool deduct) );
void   group_remove   args( ( CHAR_DATA *ch, const char *name) );

/* special.c */
SF *   spec_lookup   args( ( const char *name ) );
char *   spec_name   args( ( SPEC_FUN *function ) );

/* teleport.c */
RID *   room_by_name   args( ( char *target, int level, bool error) );

/*timer.c*/
void *  alloc_perm      args( ( int sMem ) );

/* update.c */
void    new_room_update args( (ROOM_INDEX_DATA *room, int type, bool tick) );
void    disable_room_update args( (ROOM_INDEX_DATA *room, BITVECT_DATA * type) );
void    enable_room_update args( (ROOM_INDEX_DATA *room, BITVECT_DATA * type) );
void   advance_level   args( ( CHAR_DATA *ch, bool hide ) );
void   delevel      args( ( CHAR_DATA *ch, bool hide) );
int    gain_exp   args( ( CHAR_DATA *ch, int gain, bool multiply ) );
void   gain_condition   args( ( CHAR_DATA *ch, int iCond, int value ) );
void   update_handler   args( ( void ) );
void	stats_to_spend_check args( (CHAR_DATA *ch) );

#ifdef MCCP
/* mccp.c */
bool    compressStart       args( ( DESCRIPTOR_DATA *desc, unsigned char telopt ) );
bool    compressEnd         args( ( DESCRIPTOR_DATA *desc ) );
bool    processCompressed   args( ( DESCRIPTOR_DATA *desc ) );
bool    writeCompressed     args( ( DESCRIPTOR_DATA *desc, char *txt, int length ) );
#endif

/* string.c */
void   string_edit   args( ( CHAR_DATA *ch, char **pString ) );
void    string_append   args( ( CHAR_DATA *ch, char **pString ) );
char *   string_replace   args( ( char * orig, char * old, char * new ) );
void    string_add      args( ( CHAR_DATA *ch, char *argument ) );
char *  format_string   args( ( char *oldstring ) );
char *  format_code     args( ( char *oldstring ) );
char *  first_arg       args( ( char *argument, char *arg_first, bool fCase ) );
char *   string_unpad   args( ( char * argument ) );
char *   string_proper   args( ( char * argument ) );
int     count_colors    args( ( char * string, int max ) );
int     center_text     args( ( char *source, char* dest, int width ) );
bool    regexp_match    args( ( char *pattern, char *string ) );
char * strip_colour_codes	args( ( char *string ) );
char * strip_spaces args( ( char *string ) );
char * strip_dupspaces  args( ( char *string ) );
char * rpg_control  args( ( char *string ) );
char * gen_random_str  args( ( int min_lenght, int max_lenght ) );


/* olc.c */
bool   run_olc_editor   args( ( DESCRIPTOR_DATA *d ) );
char   *olc_ed_name   args( ( CHAR_DATA *ch ) );
char   *olc_ed_vnum   args( ( CHAR_DATA *ch ) );

/* bit.c */
char * ext_flag_string	( const struct ext_flag_type *ext_flag_table, long * ext_bits );
long * ext_flag_value	( const struct ext_flag_type * ext_flag_table, char * argument );
char * ext_bit_name		( const struct ext_flag_type *ext_flag_table, BITVECT_DATA * ext_bit );
void   ext_flags_copy	( long * flags_from, long * flags_to );
long * ext_flags_intersection ( long * flags1, long * flags2 );
long * ext_flags_sum	( long * flags1, const long * flags2 );
long * ext_flags_diff   ( long * flags1, const long * flags2 );
bool   ext_flags_same	( long * flags1, long * flags2 );
bool   ext_flags_none	( long * flags );
void   ext_flags_clear	( long * flags );
BITVECT_DATA * vector_to_pointer( const struct ext_flag_type *ext_flag_table, int bank, int vector );

/* lookup.c */
int   race_lookup   args( ( const char *name) );
int   item_lookup   args( ( const char *name) );
int   liq_lookup   args( ( const char *name) );
int   weed_lookup   args( ( const char *name) );
int		position_lookup	args( (const char *name) );
int		sex_lookup	args( (const char *name) );
int		gender_lookup	args( (const char *name) );
int 	size_lookup	args( (const char *name) );
int		flag_lookup	args( (const char *, const FLAG_TYPE  *) );
BITVECT_DATA * ext_flag_lookup args( (const char *name, const EXT_FLAG_TYPE * ext_flag_table) );
bool is_settable ( const char *name, const struct flag_type *flag_table );
bool ext_is_settable ( const char *name, const struct ext_flag_type * ext_flag_table );
HELP_DATA * help_lookup	args( (char *) );
HELP_AREA * had_lookup	args( (char *) );
int language_lookup( char *arg );
//rellik: mining
int tool_lookup( const char *name );
int rawmaterial_lookup( const char *name );

bool check_only_weight_cant_equip( CHAR_DATA *ch, OBJ_DATA *obj );

/*skynet.c*/
bool is_hunting      args(( CHAR_DATA *ch, CHAR_DATA *victim ));
bool is_hating      args(( CHAR_DATA *ch, CHAR_DATA *victim ));
bool is_fearing      args(( CHAR_DATA *ch, CHAR_DATA *victim ));
void stop_hunting   args(( CHAR_DATA *ch ));
void stop_hating   args(( CHAR_DATA *ch, CHAR_DATA *victim, bool All ));
void stop_fearing   args(( CHAR_DATA *ch, CHAR_DATA *victim, bool All ));
void start_hunting   args(( CHAR_DATA *ch, CHAR_DATA *victim ));
void start_hating   args(( CHAR_DATA *ch, CHAR_DATA *victim ));
void start_fearing   args(( CHAR_DATA *ch, CHAR_DATA *victim ));
void hunt_victim   args(( CHAR_DATA *ch ));
void char_walk       args(( CHAR_DATA *ch ));
void found_prey      args(( CHAR_DATA *ch, CHAR_DATA *victim ));

/* event.c */
EVENT_DATA *    create_event   args( ( int type, int time, void * arg1, void * arg2, int var ) );
void            remove_event    args( ( int type, void * arg1, void * arg2, int var ) );

/* hashstr.c */
char *str_alloc( char *str );
char *quick_link( char *str );
int str_free( char *str );
void show_hash( int count );
char *hash_stats( void );
char *check_hash( char *str );
void hash_dump( int hash );
void show_high_hash( int top );

/*artefact.c*/

// gsn_artefact_destroy, rt_duration: parametry systemu

// minimalne i maksymalne trwanie
#define MAX_ARTEFACT_RT_DURATION (60 * 24 * 30 * 1) // maksymalny czas trwania arta. w minutach. aktualnie: 1 miesiac
#define RT_BASE_ART_DURATION (dice(24 * 60 * 10,2)) // podstawowa zywotnosc artow

// przydzielanie punktow
#define RT_POINTS_FOR_LEVEL (dice( 60 * UMAX(level/8,1), UMAX(level - 25, 1) )) // ile punktow za poziom zabitego moba
#define RT_POINTS_FOR_BOSS (dice( 60 * (level / 5), 4 ) * 2) // ile punktow za bossa
#define RT_POINTS_FOR_INVENTORY 25 // ile procent punktow ma leciec do inventory, jak w inv nie ma artow to punkty przechodza na rzeczy ubrane

// komunikaty
#define RT_ART_FIRST_COMMUNICATE ((current_time/60) + dice(30,2)) // kiedy pojawi sie pierwszy komunikat o kondycji arta
#define RT_ART_SHOW_TIME_ONE (dice( 6, 4 )) // jezeli gracz ma schowany art, to po takim czasie probujemy mu wyswietlic nastepny komunikat
#define RT_ART_SHOW_TIME_TWO (dice( 60, 48 )) // po jakim czasie mamy probowac wyswietlic nastepny komunikat po udanym wyswietleniu poprzedniego

int is_artefact         args( (OBJ_DATA *obj) );
void artefact_to_char      args( (OBJ_DATA *obj, CHAR_DATA *ch) );
void artefact_from_char      args( (OBJ_DATA *obj, CHAR_DATA *ch) );
void restore_char_on_artefact_list   args ( (CHAR_DATA *ch) );
void remove_char_from_artefact_list   args ( (CHAR_DATA *ch) );
void quit_artefact_char      args( (CHAR_DATA *ch) );
void create_artefact      args( (int vnum) );
void extract_artefact      args( (OBJ_DATA *obj) );
void extract_artefact_container   args( (OBJ_DATA *obj) );
void load_artefact      args( (ROOM_INDEX_DATA *room,OBJ_DATA *obj,CHAR_DATA *ch) );
int is_artefact_load_room   args( (ROOM_INDEX_DATA *room) );
int is_artefact_load_obj   args( (OBJ_INDEX_DATA *pObj) );
int is_artefact_load_mob   args( (CHAR_DATA *ch) );
void reboot_artefact_list   args( (void) );
void container_artefact_check   args( ( OBJ_DATA *obj,CHAR_DATA *ch, int status) );
void restore_char_on_artefact_list args( (CHAR_DATA *ch) );
void count_artefacts      args( (void) );

bool check_rt_art_destroy args( ( OBJ_DATA * obj, int time_elapsed ) );
void show_rt_art_destroy_info args( ( OBJ_DATA * obj ) );

/* Brohacz: bounty */
bool 		rem_bounty	args( ( BOUNTY_DATA **list, char *argument ) );
BOUNTY_DATA * 	add_bounty	args( ( BOUNTY_DATA **list, char *name, int value ) );
BOUNTY_DATA *	search_bounty	args( ( char *name ) );

/*weather.c*/
void init_weather args((void));
void weather_update args((bool update_time));
/***/
void   remove_charm      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool   add_charm         args( ( CHAR_DATA *ch, CHAR_DATA *victim, bool ignore_resist ) );

/* random_mazes.c */
void    map_init            args( ( void ) );
int     rnd_extype          args( ( void ) );
int     fill_list           args( ( void ) );
void    show_map            args( ( CHAR_DATA *ch ) );
void    map_to_string       args( ( CHAR_DATA *ch ) );
void    string_to_map       args( ( CHAR_DATA *ch ) );
int     add_room            args( ( int rnd, int density ) );

/* memspell.c */
#ifdef NEW_NANNY
bool 	new_can_be_specialist		args( ( NEW_CHAR_DATA * new_ch, int school ) );
bool 	new_can_be_any_specialist	args( ( NEW_CHAR_DATA * new_ch ) );
#endif
bool	can_be_specialist			args( (CHAR_DATA *ch, int school) );
bool	can_be_any_specialist		args( (CHAR_DATA *ch ) );

/* flags.c */
bool
rem_flag			args( ( PFLAG_DATA **list, char *argument ) );
PFLAG_DATA *
add_flag			args( ( PFLAG_DATA **list, char *id, int duration ) );
bool
setcharflag			args( ( CHAR_DATA *ch, char *argument ) );
bool
removecharflag		args( ( CHAR_DATA *ch, char *argument ) );
bool
setobjflag			args( ( OBJ_DATA *obj, char *argument ) );
bool
removeobjflag		args( ( OBJ_DATA *obj, char *argument ) );
bool
setroomflag			args( ( ROOM_INDEX_DATA *room, char *argument ) );
bool
removeroomflag		args( ( ROOM_INDEX_DATA *room, char *argument ) );
bool
setglobalflag		args( ( char *argument ) );
bool
removeglobalflag	args( ( char *argument ) );
bool
flag_exists			args( ( char *buf, PFLAG_DATA *plist ) );
void
update_global_flags	args( ( void ) );

#undef   CD
#undef   MID
#undef   OD
#undef   OID
#undef   RID
#undef   SF
#undef AD

/*****************************************************************************
 *                                    OLC                                    *
 *****************************************************************************/

/*
 * Object defined in limbo.are
 * Used in save.c to load objects that don't exist.
 */
#define OBJ_VNUM_DUMMY   30

/*
 * Area flags.
 */
#define         AREA_NONE       0
#define         AREA_CHANGED    1   /* Area has been modified. */
#define         AREA_ADDED      2   /* Area has been added to. */
#define         AREA_LOADING    4   /* Used for counting in db.c */
#define      AREA_RANDOM   8

#define MAX_DIR   6
#define NO_FLAG -99   /* Must not be used in flags or stats. */


/*
 * Gloves type + jewlery
 */
#define GLOVES_UNDER 0
#define GLOVES_ON    1
#define GLOVES_NONE  2

/*
 * Global Constants
 */
extern   char *   const   dir_name        [];
extern   char *   const   dir_name2       [];
extern   char *   const   dir_name_dir    [];
extern   char *   const   eng_dir_name    [];
extern   const   sh_int   rev_dir         [];          /* sh_int - ROM OLC */
extern   const   struct   spec_type   spec_table   [];

/*
 * Global variables
 */
extern      AREA_DATA *      area_first;
extern      AREA_DATA *      area_last;
extern      SHOP_DATA *      shop_last;
extern      BANK_DATA *      bank_last;

extern      int         top_event;
extern      int         top_affect;
extern      int         top_area;
extern      int         top_ed;
extern      int         top_exit;
extern      int         top_help;
extern      unsigned int      top_mob_index;
extern      unsigned int      top_obj_index;
extern      int         top_reset;
extern      unsigned int      top_room;
extern      int         top_shop;
extern      int         top_bank;

extern      unsigned int      top_vnum_mob;
extern      unsigned int      top_vnum_obj;
extern      unsigned int      top_vnum_room;

extern      char         str_empty       [1];

extern      MOB_INDEX_DATA *   mob_index_hash  [MAX_KEY_HASH];
extern      OBJ_INDEX_DATA *   obj_index_hash  [MAX_KEY_HASH];
extern      ROOM_INDEX_DATA *   room_index_hash [MAX_KEY_HASH];

#define LOWER( c )      (   lower_pol[(unsigned char)(c)])
#define UPPER( c )      (   upper_pol[(unsigned char)(c)])
#define NOPOL( c )      ( upper_nopol[(unsigned char)(c)])
#define ISOIZE( c )     (  isoize_pol[(unsigned char)(c)])

extern   char      const      pol_iso      [ ];
extern   char      const      pol_win    [ ];
extern   char      const      pol_nopl   [ ];
extern   char      const      lower_pol   [ ];
extern   char      const      upper_pol   [ ];
extern   char      const      upper_nopol   [ ];
extern   char      const      isoize_pol   [ ];

/**
 *
 *
 */
extern const char * condition_info_all [];
extern const char * condition_info_construct [];

/*
 * texty, co by zawsze by³o tak samo
 */
#define AFF_SILENCE_TEXT "Nie mo¿esz wydaæ z siebie ¿adnego d¼wiêku.\n\r"

/*
 * modyfikator bazowy szczê¶cia, pozwala na ³atw± manipulacjê tego momentu,
 * w którym czary nie bêd± siê udwaæ.
 * baza liczona dla nieudania siê czaru to:
 * number_range(0, luck+LUCK_BASE_MOD) == 0
 * czyli dla kogo¶ z luc = 24 dla LUCK_BASE_MOD = 25 co 50 czar nie wyjdzie
 * (je¶li chodzi o niewyj¶cie z przypadku)
 */
//rellik: 2008.02.10 zmieniam z 20 na 25 bo gracze narzekaj± ¿e im po kilka czarów za kolej± nie wychodzi, no w sumie po 3 zgonach jak szczescia zostaje ok 10 i przeciwnik ma ze 20 to jest szansa 1/10 ze nie wejdzie
#define LUCK_BASE_MOD 25
/*
 * MIARY D£UGO¦CI
 */
#define WIDTH_SAZEN  = 1728;
#define WIDTH_LOKIEC =  576;
#define WIDTH_STOPA  =  288;
#define WIDTH_CAL    =   24;

void		make_head		args( (CHAR_DATA *ch) );
void		make_arm		args( (CHAR_DATA *ch) );
void		make_leg		args( (CHAR_DATA *ch) );
void		make_blood		args( (CHAR_DATA *ch) );
void		make_brains	args( (CHAR_DATA *ch) );
int			GET_DAMROLL   args( ( CHAR_DATA *ch, OBJ_DATA *weapon ) );
int			GET_SKILL_DAMROLL	args( ( CHAR_DATA *ch, OBJ_DATA *weapon ) );
int			GET_HITROLL	args( ( CHAR_DATA *ch) );
void		heal_undead	args( (CHAR_DATA *ch, CHAR_DATA *victim, sh_int value ) );
void		summon_malfunction	args( (CHAR_DATA *ch, int sn ) );
void	 	destroy_obj args( (CHAR_DATA *ch, OBJ_DATA *obj) );
bool		obj_fall_into_water args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void		one_hit_shield	args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
int			luck_dam_mod args( ( CHAR_DATA *ch, int dam ) );
bool		is_undead args( ( CHAR_DATA *ch ) );
int			GET_RACE args( ( CHAR_DATA *ch ) );
int			GET_REAL_RACE args( ( CHAR_DATA *ch ) );

#define SPELL_COLOUR_HIYELLOW 0
#define SPELL_COLOUR_LOWYELLOW 1
#define SPELL_COLOUR_HIRED 2
#define SPELL_COLOUR_LOWRED 3
#define SPELL_COLOUR_HICYAN 4
#define SPELL_COLOUR_LOWCYAN 5
#define SPELL_COLOUR_HIGREEN 6
#define SPELL_COLOUR_LOWGREEN 7
#define SPELL_COLOUR_HIWHITE 8
#define SPELL_COLOUR_LOWWHITE 9
#define SPELL_COLOUR_HIBLUE 10
#define SPELL_COLOUR_LOWBLUE 11
#define SPELL_COLOUR_HIMAGNETA 12
#define SPELL_COLOUR_LOWMAGNETA 13
#define SPELL_COLOUR_DARK 14
#define RACE_LICZ 84
#define RACE_UPIORNY_RYCERZ 51

/*
TYPY CZESCI ROSLIN
*/
#define PLANT_PART_NONE               0
#define PLANT_PART_STALK              1       //lodyga
#define PLANT_PART_LEAF               2       //lisc
#define PLANT_PART_FLOWER             3       //kwiat
#define PLANT_PART_ROOT               4       //korzen
#define PLANT_PART_SEEDS              5       //nasiona
#define PLANT_PART_BERRYS             6       //jagody
#define PLANT_PART_PERENNIAL          7       //bylina
#define PLANT_PART_FRUITS             8       //owoce
#define PLANT_PART_BULB               9       //cebulka
#define PLANT_PART_BUSH               10       //krzek
#define PLANT_PART_CACTUS             11      //kaktus
#define PLANT_PART_FUNGUS             12      //grzyb

/*
EFEKTY DZIALANIA ZIOLEK
*/
#define PLANT_EFFECT_NONE                   0
#define PLANT_EFFECT_POISON_LEVEL1          1
#define PLANT_EFFECT_POISON_LEVEL10         2
#define PLANT_EFFECT_POISON_LEVEL20         3
#define PLANT_EFFECT_POISON_LEVEL30         4
#define PLANT_EFFECT_POISON_PARALIZING      5
#define PLANT_EFFECT_POISON_DEATH           6
#define PLANT_EFFECT_POISON_WEAKEN          7
#define PLANT_EFFECT_POISON_BLIND           8
#define PLANT_EFFECT_POISON_SLEEP           9
#define PLANT_EFFECT_SMALL_HP	10  //1-5HP
#define PLANT_EFFECT_MEDIUM_HP	11	//5-15
#define PLANT_EFFECT_HIGH_HP	12	//15-30
#define PLANT_EFFECT_SMALL_MOVE	13  //1-5MOVE
#define PLANT_EFFECT_MEDIUM_MOVE	14	//5-15
#define PLANT_EFFECT_HIGH_MOVE	15	//15-30
#define PLANT_EFFECT_SMALL_FOOD	16  //1-5FOOD
#define PLANT_EFFECT_MEDIUM_FOOD	17	//5-15
#define PLANT_EFFECT_HIGH_FOOD	18	//15-30
#define PLANT_EFFECT_SM_SF	19	//AFFECTY 13 + 16
#define PLANT_EFFECT_CURE_POISON	20
#define PLANT_EFFECT_HALLUCINATIONS	21	//31 POZIOM
#define PLANT_EFFECT_MM_SF	22//14+16
#define PLANT_EFFECT_AID 23
#define PLANT_EFFECT_SMALL_THIRST	24 //GASI PRAGNIENIE 1-5
#define PLANT_EFFECT_SF_ST	25 //16 + 24
#define PLANT_EFFECT_HOLD_PERSON	26
#define PLANT_EFFECT_BLIND 27
#define PLANT_EFFECT_P30_H	28	//4+26
#define PLANT_EFFECT_PB_H	29 //8+26
#define PLANT_EFFECT_MINUS_BIG_HP	30 //-100 DO 200 HP
#define PLANT_EFFECT_MINUS_SMALL_HP	31 //-50 DO 100 HP
#define PLANT_EFFECT_ENDURE_POISON	32 //NA 30 POZIOMIE
#define PLANT_EFFECT_SHOCKING_GRASP	33
#define PLANT_EFFECT_SLOW_POISON	34
#define PLANT_EFFECT_MINUS_SMALL_THIRST	35 //- 1-10 THIRST
#define PLANT_EFFECT_MINUS_DEX_P10	36 //AFFECT NA MINUS DEX O 2 + 2
#define PLANT_EFFECT_STOP_BLEEDING 37  //ZATRZYMUJE KRWAWIENIA
#define PLANT_EFFECT_MINUS_MINOR_HP 38 //- 5 DO 15 HP
#define PLANT_EFFECT_SF_SH 39 //16 + 10
#define PLANT_EFFECT_SF_ST_MLH 40 //16+24+41
#define PLANT_EFFECT_MINUS_LITTLE_HP 41 // - 10-20HP
#define PLANT_EFFECT_MF_MT_MLH 42 //17+43+41
#define PLANT_EFFECT_MEDIUM_THIRST 43 // THIRST 5-10
#define PLANT_EFFECT_PP_BLIND 44 //5 +45
/* #define PLANT_EFFECT_
*/

/* Drake - Totemy: typy zwierz±t totemicznych */
#define TOTEMIC_LYNX          (A)
#define TOTEMIC_WOLF          (B)
#define TOTEMIC_BEAR          (C)
#define TOTEMIC_WOLVERINE     (D)
#define TOTEMIC_EAGLE         (E)


#define SN_TRICK_NULL					0
#define SN_TRICK_VERTICAL_KICK			1
#define SN_TRICK_ENTWINE				2
#define SN_TRICK_RIPOSTE				3
#define SN_TRICK_CYCLONE				4
#define SN_TRICK_FLABBERGAST			5
#define SN_TRICK_STAFF_SWIRL			6
#define SN_TRICK_DRAGON_STRIKE			7
#define SN_TRICK_DECAPITATION			8
#define SN_TRICK_THUNDERING_WHACK		9
#define SN_TRICK_STRUCKING_WALLOP		10
#define SN_TRICK_SHOVE					11
#define SN_TRICK_RAVAGING_ORB			12
#define SN_TRICK_BLEED					13
#define SN_TRICK_THIGH_JAB				14
#define SN_TRICK_WEAPON_WRENCH			15
#define SN_TRICK_CRUSHING_MACE			16
#define SN_TRICK_THOUSANDSLAYER			17
#define SN_TRICK_GLORIOUS_IMPALE		18
#define SN_TRICK_DIVINE_IMPACT			19
#define SN_TRICK_DREADFUL_STRIKE		20
#define SN_TRICK_RAMPAGE				21
#define SN_TRICK_ENLIGHTENMENT			22

/*
 * mno¿nik punktów do¶wiadczenia
 *
 * Gurthg: 16-12-2007
 * a pomnó¿my im exp orazy 3, skoro tak marudza na nudê w klepaniu
 * cielawe na co zaczn± marudziæ teraz?
 * Drake:	22-4-2008
 * Nowa Era i dyspozycja Gurta o zmianie exp multi na 2.
 */
#define EXP_MULTIPLIER 1
#define EXP_SINGLE_CAP 15
// nie jest to ju¼ u¼ywane: #define DAM_MULTIPLIER 1

#define MAX_STAT_VALUE 255
#define MAX_NEW_STAT_VALUE 168
#define OLD_28_VALUE 168
#define ROLL_PER_LEVEL 2
#define ROLL_COUNT ( ( MAX_LEVEL - 10 ) / ROLL_PER_LEVEL)


// mnoznik: przez tyle mnozymy stare staty aby otrzymac nowe
// przy 6 - 28*6 = 168, bedzie to maks dla graczy (do 255 dla mobow)
// startowe 13*6 = 78

//#define NOWE_STATY
//wlaczenie nowych statsow

/**
 * struktura odpowiedzialna za definicje zawarta w pliku:
 * skills_base.c ktora to jest odpowiedzialna za szansy
 * losowania umiejetnosci i czarow dla danej profesji.
 */
struct base_skills
{
	int ch_class;
	int learn_chance;
	char *skill_name;
	int learn_min;
	int learn_max;
};

typedef struct base_skills BASE_SKILLS;

void
set_base_skills			args( ( CHAR_DATA *ch ) );

// mo¿liwe stany wpisu o que¶cie, czyli questloga

#define QL_STATE_CURRENT 0
#define QL_STATE_FINISHED 1


/**
 * specdamage ( olc + spells_cle + db )
 */

SPEC_DAMAGE     *new_spec_damage    args( (void ) );
void            free_spec_damage    args( ( SPEC_DAMAGE *specdam ) );
SPEC_DAMAGE * copy_specdam( SPEC_DAMAGE* source );

/**
 * Utils.
 */
AREA_DATA *get_vnum_area args ( ( unsigned int vnum ) );
AREA_DATA *get_area_data args ( ( unsigned int vnum ) );
int flag_value           args ( ( const struct flag_type *flag_table, char *argument) );
char *flag_string        args ( ( const struct flag_type *flag_table, int bits ) );
void add_reset           args ( ( ROOM_INDEX_DATA *room, RESET_DATA *pReset, int index ) );

#define MAX_COMPO_GROUP_SIZE 31
#define MAX_TOTAL_COMPO 100

//rellik: komponenty, struktura wi±¿±ca czar z komponentem.
struct spell_items_type
{
    int key_number; //numer unikatowy tandemu item - spell
	int spell_number; //numer slotu spella
	char *spell_name; //nazwa czaru, w³asna, do identyfikacji na razie
	char *spell_item_name; //nazwa przedmiotu wystêpuj±ca w ->name
	char *official_spell_item_name; //nazwa przedmiotu jak± widzi gracz
	char *action_description; //opis akcji podczas czarowania z tym przedmiotem
	bool item_blows; //czy po zu¿yciu przedmiot robi bum czy tylko traci w³a¶ciwo¶ci
	bool item_blows_timer; //czy po skoñczeniu timera co¶ siê dzieje
	bool special_requirements; //czy jakie¶ specjalne wymagania przy sprawdzaniu posiadania komponentu (np. ubrany)
	int group_nb; //numer grupy przy wielokomponentowo¶ci max 30 grup per sn i 30 komponentow w grupie
	//mo¿na jeszcze dodaæ modyfikator trudnoœci nauczenia siê komponentu
};

extern	const	struct spell_items_type spell_items_table[];

extern bool chk_bigflag( BIGFLAG *flaga, int nrbitu );
extern bool set_bigflag( BIGFLAG *flaga, int nrbitu );
extern bool rmv_bigflag( BIGFLAG *flaga, int nrbitu );
extern bool tgl_bigflag( BIGFLAG *flaga, int nrbitu );
extern bool know_magic_of_item( CHAR_DATA *ch, OBJ_DATA *obj );
extern int spell_items_table_count();

extern void generate_gem( OBJ_DATA *obj, int* points );

/**
 * level info
 */
extern const char * level_info_common       [];
extern const char * level_info_barbarian    [];
extern const char * level_info_bard         [];
extern const char * level_info_black_knight [];
extern const char * level_info_cleric       [];
extern const char * level_info_druid        [];
extern const char * level_info_mag          [];
extern const char * level_info_monk         [];
extern const char * level_info_paladin      [];
extern const char * level_info_thief        [];
extern const char * level_info_warrior      [];
extern const char * level_info_shaman       [];
/**
 * SAGE price (in gold)
 */
#define SAGE_PRICE 1

/**
 * Minimalny rent (w %)
 */
#define RENT_COST_RATE 5

/**
 * ITEM_SPELLBOOK - levels ( v1 )
 */
#define SPELLBOOK_LEVEL_MIN  0
#define SPELLBOOK_LEVEL_MAX 10

/**
 * set flags
 */
#define ALT_FLAGVALUE_SET( _blargh, _table, _arg )\
{\
    int blah = flag_value( _table, _arg );\
    _blargh = (blah == NO_FLAG) ? 0 : blah;\
}

#define ALT_FLAGVALUE_TOGGLE( _blargh, _table, _arg )\
{\
    int blah = flag_value( _table, _arg );\
    _blargh ^= (blah == NO_FLAG) ? 0 : blah;\
}

/**
 * standardowa cena za rent
 */
#define RENT_COST( c ) ( ( c * RENT_COST_RATE ) / 100 )

extern const char heal_plant_msg_table [6][MAX_STRING_LENGTH];
extern const char heal_animal_msg_table [6][MAX_STRING_LENGTH];
extern const char heal_golem_msg_table [6][MAX_STRING_LENGTH];
