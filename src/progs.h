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
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Kamil Kulesza         (kamili@mud.pl                 ) [Kamili    ] *
 * Koper Tadeusz         (garloop@killer-mud.net        ) [Garloop   ] *
 * Pietrzak Marcin       (marcin.pietrzak@mud.pl        ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Skrzetnicki Krzysztof (gtener@gmail.com              ) [Tener     ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: progs.h 11987 2013-01-23 13:56:44Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/progs.h $
 *
 */
//rozne define
#define MAXPFLAGS			4000	//maksymalna ilosc znakow calkowicie we flagach
#define MAX_NESTED_LEVEL	12		//Maximum nested if-else-endif's (stack size)
#define BEGIN_BLOCK			0		//Flag: Begin of if-else-endif block
#define IN_BLOCK			-1		//Flag: Executable statements
#define END_BLOCK			-2		//Flag: End of if-else-endif block
#define MAX_CALL_LEVEL		5		//Maximum nested calls
#define MAX_GOTO_CALL		1000	//Maximum goto calls
#define MAX_NESTED_LEVEL	12		//Maximum nested if-else
#define MAX_VAR				100		//maksymalna liczba zmiennych
#define MAX_GOTO			20		//maksymalna liczba miejsc skoku dla goto
#define UNDEFINED_INT		-234234

//czy maja byc logowane bugi w progach
#define PROG_LOGS

//ifchecki
#define CHK_RAND			(0)
#define CHK_MOBHERE			(1)
#define CHK_OBJHERE			(2)
#define CHK_MOBEXISTS		(3)
#define CHK_OBJEXISTS		(4)
#define CHK_PEOPLE			(5)
#define CHK_PLAYERS			(6)
#define CHK_MOBS			(7)
#define CHK_CLONES			(8)
#define CHK_ORDER			(9)
#define CHK_HOUR			(10)
#define CHK_ISPC			(11)
#define CHK_ISNPC			(12)
#define CHK_ISGOOD			(13)
#define CHK_ISEVIL			(14)
#define CHK_ISNEUTRAL		(15)
#define CHK_ISIMMORT		(16)
#define CHK_ISCHARM			(17)
#define CHK_ISFOLLOW		(18)
#define CHK_ISACTIVE		(19)
#define CHK_ISDELAY			(20)
#define CHK_ISVISIBLE		(21)
#define CHK_HASTARGET		(22)
#define CHK_ISTARGET		(23)
#define CHK_EXISTS			(24)
#define CHK_AFFECTED		(25)
#define CHK_ACT				(26)
#define CHK_OFF				(27)
#define CHK_IMM				(28)
#define CHK_CARRIES			(29)
#define CHK_WEARS			(30)
#define CHK_HAS				(31)
#define CHK_USES			(32)
#define CHK_NAME			(33)
#define CHK_POS				(34)
#define CHK_CLAN			(35)
#define CHK_RACE			(36)
#define CHK_CLASS			(37)
#define CHK_OBJTYPE			(38)
#define CHK_VNUM			(39)
#define CHK_HPCNT			(40)
#define CHK_ROOM			(41)
#define CHK_SEX				(42)
#define CHK_LEVEL			(43)
#define CHK_ALIGN			(44)
#define CHK_MONEY			(45)
#define CHK_OBJVAL0			(46)
#define CHK_OBJVAL1			(47)
#define CHK_OBJVAL2			(48)
#define CHK_OBJVAL3			(49)
#define CHK_OBJVAL4			(50)
#define CHK_GRPSIZE			(51)
#define CHK_HASFLAG			(52)
#define CHK_STR				(53)
#define CHK_INT				(54)
#define CHK_WIS				(55)
#define CHK_DEX				(56)
#define CHK_CON				(57)
#define CHK_EXITHERE		(58)
#define CHK_ROOMHASFLAG		(59)
#define CHK_OBJFLAG			(60)
#define CHK_ROOMFLAG		(61)
#define CHK_CHARFLAG		(62)
#define CHK_HEIGHT			(63)
#define CHK_WEIGHT			(64)
#define CHK_MAGSPEC			(65)
#define CHK_DAYOFWEEK		(66)
#define CHK_DAYOFMONTH		(67)
#define CHK_MONTH			(68)
#define CHK_NAMEAFFECTED	(69)
#define CHK_ISDARKHERE		(70)
#define CHK_SECTOR			(71)
#define CHK_ISMOUNTED		(72)
#define CHK_ISRIDING		(73)
#define CHK_AWEARS			(74)
#define CHK_AHAS			(75)
#define CHK_ACANLOAD		(76)
#define CHK_RESPONSE		(77)
#define CHK_ISHERE			(78)
#define CHK_CHA				(79)
#define CHK_LUC				(80)
#define CHK_LIQUID			(81)
#define CHK_MOVECNT			(82)
#define CHK_MOVE			(83)
#define CHK_OBJVAL5			(84)
#define CHK_OBJVAL6			(85)
#define CHK_OBJSTCOST		(86)
#define CHK_OBJSTCONDITION	(87)
#define CHK_SPEAKING		(88)
#define CHK_LANG			(89)
#define CHK_NUMEVAL			(90)
#define CHK_STREVAL			(91)
#define CHK_VAR_IS_NULL		(92)
#define CHK_INGROUP			(93)
#define CHK_CANMOVE			(94)
#define CHK_ISDOOROPEN		(95)
#define CHK_ISDOORLOCKED	(96)
#define CHK_ISGLOBALFLAG	(97)
#define CHK_GLOBALFLAG		(98)
#define CHK_AGE				(99)
#define CHK_GLORY			(100)
#define CHK_HASQLOG			(101)
#define CHK_ISCARRIED		(102)
#define CHK_ISBURNPROOF		(103)

//operatory porownania integerow
#define EVAL_EQ				0
#define EVAL_GE				1
#define EVAL_LE				2
#define EVAL_GT				3
#define EVAL_LT				4
#define EVAL_NE				5

//operatory porownania integerow
#define STREVAL_EQ				0
#define STREVAL_PRE				1
#define STREVAL_IN				2
#define STREVAL_REG				3

//prog type
#define PROG_TYPE_MOB		1
#define PROG_TYPE_OBJ		2
#define PROG_TYPE_ROOM		3

//variable types
#define PROG_VAR_UNDEFINED	0
#define PROG_VAR_CHAR		1
#define PROG_VAR_OBJ		2
#define PROG_VAR_ROOM		3
#define PROG_VAR_STRING		4
#define PROG_VAR_INT		5

//prog options
#define PROG_OPT_VARCOPY	1

//typt memdat
#define MEMDAT_ANY				0
#define MEMDAT_DELAY			1
#define MEMDAT_MINIDELAY		2
#define MEMDAT_RESPONSEDELAY	3

#define ABORT_PROG()	{ \
							if ( temp_var_list ) \
								free_var_list( &temp_var_list ); \
							if ( running_prog_env->vch ) \
							{ \
								running_prog_env->vch->precommand_pending = FALSE; \
								running_prog_env->vch->precommand_fun = NULL; \
								free_string( running_prog_env->vch->precommand_arg ); \
								running_prog_env->vch->precommand_arg = NULL; \
							} \
							free_prog_env( running_prog_env ); \
							return; \
						}

#define IS_BUILT_IN_VAR(var)	( strlen( var ) > 1 && var[0] == '$' && var[1] != '_' )
#define IS_VAR(var)				( strlen( var ) > 2 && var[0] == '$' && var[1] == '_' )
#define IS_STRING(var)			( var[0] == '"' || var[0] == '\'' )
#define IS_NUMBER(var)			( is_number(var) )

//typedefy
typedef struct prog_env_data	PROG_ENV;	//¶rodowisko uruchomieniowe proga
typedef struct prog_var_data	PROG_VAR;	//zmienne do progów
typedef struct goto_data		GOTO_DATA;	//do komendy goto w progach
typedef struct memdat			MEMDAT;		//do delay i minidelay

//externy
extern CHAR_DATA *					supermob;
extern PROG_ENV *					running_prog_env;
extern PROG_VAR *					temp_var_list;
extern MEMDAT *						memdat_list;
extern const char *					fn_keyword [];
extern const char *					fn_evals [];
extern const char * var_type_to_str [];

#define VAR_CMD( fun )	PROG_VAR * fun( char * vc_name, PROG_VAR * args )

typedef PROG_VAR * VAR_CMD_FUN   args( ( char * vc_name, PROG_VAR * args ) );

struct	mob_cmd_type
{
    char * const	name;
    PROG_FUN *		prog_fun;
};

struct	obj_cmd_type
{
    char * const	name;
    PROG_FUN *		prog_fun;
};

struct	var_cmd_type
{
    char * const	name;
    VAR_CMD_FUN *	cmd_fun;
	int				args_count;
	int				ret_type;
	int				args_type[4];
	char * const	syntax_desc;
};


extern const struct mob_cmd_type	mob_cmd_table [];
extern const struct obj_cmd_type	obj_cmd_table [];
extern const struct var_cmd_type	var_cmd_table [];

//struktury
struct goto_data
{
	char *			name;
	char *			code_start;
	int				state[ MAX_NESTED_LEVEL ];
	int				cond[ MAX_NESTED_LEVEL ];
	int				level;
};

struct memdat
{
	MEMDAT *	next;
	PROG_ENV *	prog_env;
	char *		response;
	sh_int		delay;
	int			type;
};

struct prog_check
{
	char *	name;
	bool	mtrig;
	bool	otrig;
	bool	rtrig;
};

struct prog_env_data
{
	PROG_ENV *			next;						//wskaznik na nastepny prog_env listy aktualnie wykonywanych progow
	char *				name;						//nazwa proga
	char *				source_ptr;					//wskaznik na aktualn± liniê kodu
	CHAR_DATA *			ch;							//mob wykonuj±cy próg (supermob w przypadku obj i room progów)
	OBJ_DATA *			obj;						//obiekt do którego nale¿y obj prog
	ROOM_INDEX_DATA *	room;						//room do którego nale¿y room prog
	CHAR_DATA *			vch;						//cel proga (zazwyczaj gracz/mob który odpali³ triggera)
	CHAR_DATA *			rch;						//randomowy mob w lokacji ($r)
	void *				arg1;						//podstawowy parametr nr 1
	void *				arg2;						//podstawowy parametr nr 2
	int					type;						//typ progu (obj, mob, room);
	BITVECT_DATA *		trigger;					//trigger który odpali³ ten próg
	char *				trig_param;					//fraza (parametr triggera)
	GOTO_DATA *			gotos[ MAX_GOTO ];			//tablica miejsc do których mozna skakac komend± goto
	int					goto_call;					//liczba dotychczasowych wywo³añ goto
	PROG_VAR *      	vars;						//lista zmiennych
	int					level;						//aktualny poziom zag³êbienia if
	int					state[ MAX_NESTED_LEVEL ];	//stan bloków kodu
	int					cond[ MAX_NESTED_LEVEL ];	//warto¶æ logiczna kolejnych poziomów if
	int					options;					//opcje proga - define PROG_OPT_*
	void *				other;						//zmienna wielozadaniowa
};

struct prog_var_data
{
	PROG_VAR *			next;
	char *				name;			//nazwa zmiennej
	union
	{
		CHAR_DATA *			character;	//warto¶æ zmiennej
		OBJ_DATA *			object;		//warto¶æ zmiennej
		ROOM_INDEX_DATA *	room;		//warto¶æ zmiennej
		char *				string;		//warto¶æ zmiennej
		char *				integer;	//warto¶æ zmiennej (przechowujemy jako char* ¿eby by³o mniej konwersji)
	} value;
	int					type;			//typ zmiennej
};


#define DECLARE_VAR_CMD_FUN( fun )      PROG_VAR * fun( char * vc_name, PROG_VAR * args );


struct  trigger_to_pos_type
{
	BITVECT_DATA *	trigger;
	int				position;
};

/* lookup.c */
int
flag_lookup			( const char *name, const struct flag_type *flag_table );
int
flag_value			( const struct flag_type *flag_table, char *argument );

/* progs_engine.c */
int
mprog_trigger_default_pos	( BITVECT_DATA * type );
void
free_prog_env		( PROG_ENV* penv );
void
init_supermob		( void );
void
release_supermob	( void );
int
keyword_lookup		( const char **table, char *keyword );
bool
num_eval			( int lval, int oper, int rval );
bool
str_eval			( char * strlval, int oper, char * strrval );
CHAR_DATA *
get_random_char		( CHAR_DATA *mob, bool PC );
int
count_people_room	( CHAR_DATA *mob, int iFlag );
int
get_order			( CHAR_DATA *ch );
bool
has_item			( CHAR_DATA *ch, unsigned int vnum, sh_int item_type, bool fWear );
bool
carries_item		( CHAR_DATA *ch, unsigned int vnum, sh_int item_type );
bool
get_mob_vnum_room	( CHAR_DATA *ch, unsigned int vnum );
bool
get_obj_vnum_room	( CHAR_DATA *ch, unsigned int vnum );
PROG_ENV *
new_prog_env		( void );
MEMDAT *
new_mdat			( void );
void
free_mdat			( MEMDAT * mdat );
MEMDAT *
get_mob_memdat		( CHAR_DATA * ch, CHAR_DATA * vch, char * prog_name, int type );
MEMDAT *
get_obj_memdat		( OBJ_DATA * obj, CHAR_DATA * vch, char * prog_name, int type );
MEMDAT *
get_room_memdat		( ROOM_INDEX_DATA * room, CHAR_DATA * vch, char * prog_name, int type );
GOTO_DATA *
new_goto_data		( void );
void
free_goto			( GOTO_DATA *del_goto );
GOTO_DATA *
find_goto			( GOTO_DATA * gotos[], char * name );
bool
add_goto			( GOTO_DATA * gotos[], GOTO_DATA * addgoto );
GOTO_DATA *
del_goto			( GOTO_DATA * gotos[], char * name );
int
count_running_progs	( void );
void
add_prog_env		( PROG_ENV * prog_env );
void
create_mprog_env	( char * name, char * source_ptr, CHAR_DATA * ch, CHAR_DATA * vch,
					  void * arg1, void * arg2, BITVECT_DATA * trigger, char * trig_param );
void
create_oprog_env	( char * name, char * source_ptr, OBJ_DATA * obj, CHAR_DATA * vch,
					  void * arg1, void * arg2, BITVECT_DATA * trigger, char * trig_param );
void
create_rprog_env	( char * name, char * source_ptr, ROOM_INDEX_DATA * room, CHAR_DATA * vch,
					  void * arg1, void * arg2, BITVECT_DATA * trigger, char * trig_param );
void
bug_prog_format		( char * fmt, ...);
void
act_prog			( char *format, CHAR_DATA *ch, CHAR_DATA *victim, int type, int min_pos );
void
program_flow		( void );
int
cmd_eval			( char *line, int check );
void
expand_arg			( char *buf, char *format );
OBJ_DATA *
resolve_obj_arg		( char *format );
CHAR_DATA *
resolve_char_arg	( char *format );
ROOM_INDEX_DATA* 
resolve_room_arg    ( char *format );
void
init_supermob		( void );
void
set_supermob		( OBJ_DATA *obj );
void
rset_supermob		( ROOM_INDEX_DATA *room );
void
pset_supermob		( CHAR_DATA *ch );
void
release_supermob	( void );

/* act_wiz.c */
ROOM_INDEX_DATA *
find_location		( CHAR_DATA *, char * );

/* progs_vars.c */
PROG_VAR *
new_prog_var		( char * name, void * value, int type );
PROG_VAR *
copy_prog_var		( char * name, PROG_VAR * cpy_pvar );
void
var_copy_value		( PROG_VAR * pvar, PROG_VAR * cpy_pvar );
void
var_clear			( PROG_VAR * pvar );
void
var_set_value		( PROG_VAR * pvar, void * value, int type );
void
free_prog_var		( PROG_VAR * pvar );
void
push_var			( PROG_VAR *pvar, PROG_VAR ** stack );
PROG_VAR *
pop_var				( PROG_VAR * stack, int number );
void
var_to_list			( PROG_VAR * pvar, PROG_VAR ** list );
void
var_from_list		( PROG_VAR * pvar, PROG_VAR ** list );
void
free_var_list		( PROG_VAR ** list );
void
partial_free_var_list	( PROG_VAR ** list, PROG_VAR * stop );
void
copy_var_list		( PROG_VAR *from, PROG_VAR ** to );
int
count_vars			( PROG_VAR * list );
bool
validate_var_name	( char * name );
PROG_VAR *
get_var				( char * name, PROG_VAR * list );
CHAR_DATA *
get_char_var		( PROG_VAR * pvar );
OBJ_DATA *
get_obj_var			( PROG_VAR * pvar );
ROOM_INDEX_DATA *
get_room_var		( PROG_VAR * pvar );
char *
get_string_var		( PROG_VAR * pvar );
int
get_int_var			( PROG_VAR * pvar );
bool
var_is_null			( PROG_VAR * pvar );
char *
var_to_string		( PROG_VAR * pvar );
char *
grab_var_name		( char *argument, char *arg_first );
bool
is_var_cmd			( char * command );
int
get_var_cmd			( char * command, PROG_VAR * vars );
char *
split_var_name		( char * argument, char * attr );
char *
param_one_argument	( char *argument, char *arg_first );
char *
var_dispatch_cmd	( PROG_VAR * lvar, char * command, char * args );
PROG_VAR *
var_attribute		( PROG_VAR * pvar, char * attr );

/* obj_cmds.c */
bool
setobjflag			( OBJ_DATA *obj, char *argument );
/* mob_cmds.c */
bool
has_qlog		( CHAR_DATA* ch, char* qlname );

/*
 * Mob-command functions.
 */
DECLARE_PROG_FUN( mob_interpret		);
DECLARE_PROG_FUN( obj_interpret		);

DECLARE_PROG_FUN( do_mpasound		);
DECLARE_PROG_FUN( do_mpgecho		);
DECLARE_PROG_FUN( do_mpzecho		);
DECLARE_PROG_FUN( do_mpkill			);
DECLARE_PROG_FUN( do_mpassist		);
DECLARE_PROG_FUN( do_mpjunk			);
DECLARE_PROG_FUN( do_mpechoaround	);
DECLARE_PROG_FUN( do_mpecho			);
DECLARE_PROG_FUN( do_mpechoat		);
DECLARE_PROG_FUN( do_mpmload		);
DECLARE_PROG_FUN( do_mpaddgroup		);
DECLARE_PROG_FUN( do_mpoload		);
DECLARE_PROG_FUN( do_mppurge		);
DECLARE_PROG_FUN( do_mpgoto			);
DECLARE_PROG_FUN( do_mpat			);
DECLARE_PROG_FUN( do_mptransfer		);
DECLARE_PROG_FUN( do_mpgtransfer	);
DECLARE_PROG_FUN( do_mpforce		);
DECLARE_PROG_FUN( do_mpgforce		);
DECLARE_PROG_FUN( do_mpvforce		);
DECLARE_PROG_FUN( do_mpcast			);
DECLARE_PROG_FUN( do_mpdamage		);
DECLARE_PROG_FUN( do_mpdamageex		);
DECLARE_PROG_FUN( do_mpremember		);
DECLARE_PROG_FUN( do_mpforget		);
DECLARE_PROG_FUN( do_mpdelay		);
DECLARE_PROG_FUN( do_mpcancel		);
DECLARE_PROG_FUN( do_mpcall			);
DECLARE_PROG_FUN( do_mpflee			);
DECLARE_PROG_FUN( do_mpotransfer	);
DECLARE_PROG_FUN( do_mpremove		);
DECLARE_PROG_FUN( do_mpsetflag		);
DECLARE_PROG_FUN( do_mpremoveflag	);
DECLARE_PROG_FUN( do_mpequip		);
DECLARE_PROG_FUN( do_mpreward		);
DECLARE_PROG_FUN( do_mpunlock		);
DECLARE_PROG_FUN( do_mplock			);
DECLARE_PROG_FUN( do_mpopen			);
DECLARE_PROG_FUN( do_mpclose		);
DECLARE_PROG_FUN( do_mpsetrent		);
DECLARE_PROG_FUN( do_mpclrmem		);
DECLARE_PROG_FUN( do_mpsethhf		);
DECLARE_PROG_FUN( do_mpexit			);
DECLARE_PROG_FUN( do_mpsetaff		);
DECLARE_PROG_FUN( do_mpaddaff		);
DECLARE_PROG_FUN( do_mpmodflag		);
DECLARE_PROG_FUN( do_mproomdesc		);
DECLARE_PROG_FUN( do_mpmesg			);
DECLARE_PROG_FUN( do_mponehit		);
DECLARE_PROG_FUN( do_mpdelaff		);
DECLARE_PROG_FUN( do_mpapplypoison	);
DECLARE_PROG_FUN( do_mpmobstring	);
DECLARE_PROG_FUN( do_mpobjstring	);
DECLARE_PROG_FUN( do_mpaload		);
DECLARE_PROG_FUN( do_mpgreward		);
DECLARE_PROG_FUN( do_mplog			);
DECLARE_PROG_FUN( do_mpsay			);
DECLARE_PROG_FUN( do_mpsayto		);
DECLARE_PROG_FUN( do_mpcommand		);
DECLARE_PROG_FUN( do_mpwalk			);
DECLARE_PROG_FUN( do_mpsetpos		);
DECLARE_PROG_FUN( do_mpsetposdefaul	);
DECLARE_PROG_FUN( do_mppeace		);
//DECLARE_PROG_FUN( do_mpsmoke		);
DECLARE_PROG_FUN( do_mpquestlog		);
/*
 * Obj-command functions.
 */
DECLARE_PROG_FUN( do_osetflag 		);
DECLARE_PROG_FUN( do_osetobj		);
DECLARE_PROG_FUN( do_omodflag		);
DECLARE_PROG_FUN( do_oremove		);
DECLARE_PROG_FUN( do_ostring		);
DECLARE_PROG_FUN( do_objwear		);
DECLARE_PROG_FUN( do_objunwear		);
DECLARE_PROG_FUN( do_objsubstitute	);

DECLARE_VAR_CMD_FUN( vc_int_add );
DECLARE_VAR_CMD_FUN( vc_int_sub );
DECLARE_VAR_CMD_FUN( vc_int_mult );
DECLARE_VAR_CMD_FUN( vc_int_div );
DECLARE_VAR_CMD_FUN( vc_int_mod );
DECLARE_VAR_CMD_FUN( vc_int_to_str );
DECLARE_VAR_CMD_FUN( vc_int_random );
DECLARE_VAR_CMD_FUN( vc_room_by_vnum );
DECLARE_VAR_CMD_FUN( vc_room_by_char );
DECLARE_VAR_CMD_FUN( vc_room_by_obj );
DECLARE_VAR_CMD_FUN( vc_char_first_in_room );
DECLARE_VAR_CMD_FUN( vc_char_in_room_by_name );
DECLARE_VAR_CMD_FUN( vc_char_in_room_by_vnum );
DECLARE_VAR_CMD_FUN( vc_obj_first_carried );
DECLARE_VAR_CMD_FUN( vc_obj_first_in_room );
DECLARE_VAR_CMD_FUN( vc_obj_in_room_by_name );
DECLARE_VAR_CMD_FUN( vc_obj_in_room_by_vnum );
DECLARE_VAR_CMD_FUN( vc_obj_in_obj_by_vnum );
DECLARE_VAR_CMD_FUN( vc_obj_char_eq_slot );
DECLARE_VAR_CMD_FUN( vc_str_concat );
DECLARE_VAR_CMD_FUN( vc_str_substr );
DECLARE_VAR_CMD_FUN( vc_str_length );
DECLARE_VAR_CMD_FUN( vc_str_to_int );
DECLARE_VAR_CMD_FUN( vc_var_by_name );
DECLARE_VAR_CMD_FUN( vc_common_next_in_list );
DECLARE_VAR_CMD_FUN( vc_common_flag_value );
DECLARE_VAR_CMD_FUN( vc_common_first_flag );
DECLARE_VAR_CMD_FUN( vc_common_next_flag );
DECLARE_VAR_CMD_FUN( vc_datetime );
