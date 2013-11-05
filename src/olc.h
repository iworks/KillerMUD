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
 *  File: olc.h                                                        *
 *                                                                     *
 *  Much time and thought has gone into this software and you are      *
 *  benefitting.  We hope that you share your changes too.  What goes  *
 *  around, comes around.                                              *
 *                                                                     *
 *  This code was freely distributed with the The Isles 1.1 source     *
 *  code, and has been used here for OLC - OLC would not be what it    *
 *  is without all the previous coders who released their source code. *
 *                                                                     *
 ***********************************************************************
 *                                                                     *
 * This is a header file for all the OLC files.  Feel free to copy it  *
 * into merc.h if you wish.  Many of these routines may be handy       *
 * elsewhere in the code.  -Jason Dinkel                               *
 *                                                                     *
 * The version info.  Please use this info when reporting bugs.        *
 * It is displayed in the game by typing 'version' while editing.      *
 * Do not remove these from the code - by request of Jason Dinkel      *
 *                                                                     *
 ***********************************************************************
 *                                                                     *
 * KILLER MUD is copyright 1999-2011 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: olc.h 11230 2012-04-05 20:53:29Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/olc.h $
 *
 */
#define VERSION	"ILAB Online Creation [Beta 1.0, ROM 2.3 modified]\n\r" \
		"     Port a ROM 2.4 v1.8\n\r"
#define AUTHOR	"     By Jason(jdinkel@mines.colorado.edu)\n\r" \
                "     Modified for use with ROM 2.3\n\r"        \
                "     By Hans Birkeland (hansbi@ifi.uio.no)\n\r" \
                "     Modificado para uso en ROM 2.4b6\n\r"	\
                "     Por Ivan Toledo (itoledo@ctcreuna.cl)\n\r"
#define DATE	"     (Apr. 7, 1995 - ROM mod, Apr 16, 1995)\n\r" \
		"     (Port a ROM 2.4 - Nov 2, 1996)\n\r" \
		"     Version actual : 1.8 - Sep 8, 1998\n\r"
#define CREDITS "     Original by Surreality(cxw197@psu.edu) and Locke(locke@lm.com)"

/*
 * New typedefs.
 */
typedef	bool OLC_FUN		args( ( CHAR_DATA *ch, char *argument ) );
#define DECLARE_OLC_FUN( fun )	OLC_FUN    fun

/* Command procedures needed ROM OLC */
DECLARE_DO_FUN(    do_help    );
DECLARE_SPELL_FUN( spell_null );

/*
 * Connected states for editor.
 */
#define ED_NONE        0
#define ED_AREA        1
#define ED_ROOM        2
#define ED_OBJECT      3
#define ED_MOBILE      4
#define ED_MPCODE      5
#define ED_HELP        6
#define ED_OPCODE      7
#define ED_RPCODE      9
#define ED_TRAPDATA   12
#define ED_RDESCDATA  13
#define ED_BSDATA     14
#define ED_SDATA      15
#define ED_HBDATA     16

/*
 * Interpreter Prototypes
 */
void    aedit     args( ( CHAR_DATA *ch, char *argument ) );
void    redit     args( ( CHAR_DATA *ch, char *argument ) );
void    medit     args( ( CHAR_DATA *ch, char *argument ) );
void    oedit     args( ( CHAR_DATA *ch, char *argument ) );
void    mpedit    args( ( CHAR_DATA *ch, char *argument ) );
void    opedit    args( ( CHAR_DATA *ch, char *argument ) );
void    rpedit    args( ( CHAR_DATA *ch, char *argument ) );
void    tedit     args( ( CHAR_DATA *ch, char *argument ) );
void    rdedit    args( ( CHAR_DATA *ch, char *argument ) );
void    hedit     args( ( CHAR_DATA *, char * ) );
void    bsedit    args( ( CHAR_DATA *ch, char *argument ) );
void    sedit     args( ( CHAR_DATA *ch, char *argument ) );
void    hbedit    args( ( CHAR_DATA *ch, char *argument ) );

/*
 * OLC Constants
 */
#define MAX_MOB	1		/* Default maximum number for resetting mobs */
#define MAX_MOB_WEALTH RATTING_MITHRIL * 1000 /* maksymalna liczba monet do ustawienia mobowi z poziomu OLC */

/*
 * Structure for an OLC editor command.
 */
struct olc_cmd_type
{
    char * const	name;
    OLC_FUN *		olc_fun;
};

/*
 * Structure for an OLC editor startup command.
 */
struct	editor_cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
};

/*
 * Interpreter Table Prototypes
 */
extern const struct olc_cmd_type	aedit_table[];
extern const struct olc_cmd_type	redit_table[];
extern const struct olc_cmd_type	oedit_table[];
extern const struct olc_cmd_type	medit_table[];
extern const struct olc_cmd_type	mpedit_table[];
extern const struct olc_cmd_type	opedit_table[];
extern const struct olc_cmd_type	rpedit_table[];
extern const struct olc_cmd_type	hedit_table[];
extern const struct olc_cmd_type	tedit_table[];
extern const struct olc_cmd_type	rdedit_table[];
extern const struct olc_cmd_type	bsedit_table[];
extern const struct olc_cmd_type	sedit_table[];
extern const struct olc_cmd_type	hbedit_table[];


/*
 * Editor Commands.
 */
DECLARE_DO_FUN( do_aedit        );
DECLARE_DO_FUN( do_redit        );
DECLARE_DO_FUN( do_oedit        );
DECLARE_DO_FUN( do_medit        );
DECLARE_DO_FUN( do_mpedit	);
DECLARE_DO_FUN( do_opedit	);
DECLARE_DO_FUN( do_rpedit	);
DECLARE_DO_FUN( do_hedit	);
DECLARE_DO_FUN( do_tedit	);
DECLARE_DO_FUN( do_rdedit	);
DECLARE_DO_FUN( do_bsedit	);
DECLARE_DO_FUN( do_sedit	);
DECLARE_DO_FUN( do_hbedit	);

/*
 * General Functions
 */
bool show_commands		args ( ( CHAR_DATA *ch, char *argument ) );
bool show_help			args ( ( CHAR_DATA *ch, char *argument ) );
bool edit_done			args ( ( CHAR_DATA *ch ) );
bool show_version		args ( ( CHAR_DATA *ch, char *argument ) );

/*
 * Area Editor Prototypes
 */
DECLARE_OLC_FUN( aedit_show		);
DECLARE_OLC_FUN( aedit_create		);
DECLARE_OLC_FUN( aedit_name		);
DECLARE_OLC_FUN( aedit_file		);
DECLARE_OLC_FUN( aedit_age		);
DECLARE_OLC_FUN( aedit_resetage		);
DECLARE_OLC_FUN( aedit_reset		);
DECLARE_OLC_FUN( aedit_security		);
DECLARE_OLC_FUN( aedit_builder		);
DECLARE_OLC_FUN( aedit_region		);
DECLARE_OLC_FUN( aedit_vnum		);
DECLARE_OLC_FUN( aedit_lvnum		);
DECLARE_OLC_FUN( aedit_uvnum		);
DECLARE_OLC_FUN( aedit_credits		);
DECLARE_OLC_FUN( aedit_lock			);

/*
 * Room Editor Prototypes
 */
DECLARE_OLC_FUN( redit_show		);
DECLARE_OLC_FUN( redit_create		);
DECLARE_OLC_FUN( redit_name		);
DECLARE_OLC_FUN( redit_desc		);
DECLARE_OLC_FUN( redit_nightdesc	);
DECLARE_OLC_FUN( redit_ed		);
DECLARE_OLC_FUN( redit_format		);
DECLARE_OLC_FUN( redit_north		);
DECLARE_OLC_FUN( redit_south		);
DECLARE_OLC_FUN( redit_east		);
DECLARE_OLC_FUN( redit_west		);
DECLARE_OLC_FUN( redit_up		);
DECLARE_OLC_FUN( redit_down		);
DECLARE_OLC_FUN( redit_mreset		);
DECLARE_OLC_FUN( redit_oreset		);
DECLARE_OLC_FUN( redit_links		);
DECLARE_OLC_FUN( redit_mshow		);
DECLARE_OLC_FUN( redit_rent		);
DECLARE_OLC_FUN( redit_oshow		);
DECLARE_OLC_FUN( redit_heal		);
DECLARE_OLC_FUN( redit_clan		);
DECLARE_OLC_FUN( redit_owner		);
DECLARE_OLC_FUN( redit_room		);
DECLARE_OLC_FUN( redit_trap		);
DECLARE_OLC_FUN( redit_sector		);
DECLARE_OLC_FUN( redit_addprog        );  /* ROM */
DECLARE_OLC_FUN( redit_delprog        );  /* ROM */
DECLARE_OLC_FUN( redit_progs		);
DECLARE_OLC_FUN( redit_randday		);
DECLARE_OLC_FUN( redit_randnight	);
DECLARE_OLC_FUN( redit_forroom	);
DECLARE_OLC_FUN( redit_echoto	);
DECLARE_OLC_FUN( redit_areapart	);
DECLARE_OLC_FUN( redit_resource ); //rellik: mining, definiuje minera³y w lokacji
DECLARE_OLC_FUN( redit_capacity ); //rellik: miniing, definiuje zasobno¶æ ¿y³y w bry³ki

/*
 * Object Editor Prototypes
 */
DECLARE_OLC_FUN( oedit_show		);
DECLARE_OLC_FUN( oedit_create		);

DECLARE_OLC_FUN( oedit_name		);
DECLARE_OLC_FUN( oedit_short		);
DECLARE_OLC_FUN( oedit_long		);
DECLARE_OLC_FUN( oedit_addaffect	);
DECLARE_OLC_FUN( oedit_addapply		);
DECLARE_OLC_FUN( oedit_delaffect	);
DECLARE_OLC_FUN( oedit_value0		);
DECLARE_OLC_FUN( oedit_value1		);
DECLARE_OLC_FUN( oedit_value2		);
DECLARE_OLC_FUN( oedit_value3		);
DECLARE_OLC_FUN( oedit_value4		);  /* ROM */
DECLARE_OLC_FUN( oedit_value5		);  /* XROM */
DECLARE_OLC_FUN( oedit_value6		);  /* XROM */
DECLARE_OLC_FUN( oedit_length		);
DECLARE_OLC_FUN( oedit_weight		);
DECLARE_OLC_FUN( oedit_cost		);
DECLARE_OLC_FUN( oedit_ed		);

DECLARE_OLC_FUN( oedit_extra            );  /* ROM */
DECLARE_OLC_FUN( oedit_exwear           );  /* ROM */
DECLARE_OLC_FUN( oedit_wear             );  /* ROM */
DECLARE_OLC_FUN( oedit_type             );  /* ROM */
DECLARE_OLC_FUN( oedit_material		);  /* ROM */
DECLARE_OLC_FUN( oedit_level            );  /* ROM */
DECLARE_OLC_FUN( oedit_clone            );  /* ROM */
DECLARE_OLC_FUN( oedit_condition        );  /* ROM */
DECLARE_OLC_FUN( oedit_addoprog         );  /* ROM */
DECLARE_OLC_FUN( oedit_deloprog         );  /* ROM */
DECLARE_OLC_FUN( oedit_odmiana          );  /* ROM */
DECLARE_OLC_FUN( oedit_trap             );  /* ROM */
DECLARE_OLC_FUN( oedit_bonus_set        );  /* ROM */
DECLARE_OLC_FUN( oedit_desc				);
DECLARE_OLC_FUN( oedit_ident			);
DECLARE_OLC_FUN( oedit_hidden			);
DECLARE_OLC_FUN( oedit_addcomment		);
DECLARE_OLC_FUN( oedit_delcomment		);
DECLARE_OLC_FUN( oedit_specdam			);
DECLARE_OLC_FUN( oedit_liczba_mnoga		);
DECLARE_OLC_FUN( oedit_gender			);
DECLARE_OLC_FUN( oedit_item_spell	); //rellik: komponenty, definiowanie przedmiotu jako komponentu, obejmuje ustawianie licznika i timera
DECLARE_OLC_FUN( oedit_repair           ); // repair limits

/*
 * Mobile Editor Prototypes
 */
DECLARE_OLC_FUN( medit_show		);
DECLARE_OLC_FUN( medit_attacks		);
DECLARE_OLC_FUN( medit_create		);
DECLARE_OLC_FUN( medit_name		);
DECLARE_OLC_FUN( medit_short		);
DECLARE_OLC_FUN( medit_long		);
DECLARE_OLC_FUN( medit_shop		);
DECLARE_OLC_FUN( medit_repair		);
DECLARE_OLC_FUN( medit_bank		);
DECLARE_OLC_FUN( medit_desc		);
DECLARE_OLC_FUN( medit_level		);
DECLARE_OLC_FUN( medit_align		);
DECLARE_OLC_FUN( medit_corpse		);
DECLARE_OLC_FUN( medit_skin		);
DECLARE_OLC_FUN( medit_spec		);
DECLARE_OLC_FUN( medit_exp_multiplier		);

DECLARE_OLC_FUN( medit_sex		);  /* ROM */
DECLARE_OLC_FUN( medit_act		);  /* ROM */
DECLARE_OLC_FUN( medit_affect		);  /* ROM */
DECLARE_OLC_FUN( medit_ac		);  /* ROM */
DECLARE_OLC_FUN( medit_stats		);  /* ROM */
DECLARE_OLC_FUN( medit_odmiana		);  /* ROM */
DECLARE_OLC_FUN( medit_form		);  /* ROM */
DECLARE_OLC_FUN( medit_part		);  /* ROM */
DECLARE_OLC_FUN( medit_res		);  /* ROM */
DECLARE_OLC_FUN( medit_heal_from	);
DECLARE_OLC_FUN( medit_material		);  /* ROM */
DECLARE_OLC_FUN( medit_off		);  /* ROM */
DECLARE_OLC_FUN( medit_size		);  /* ROM */
DECLARE_OLC_FUN( medit_hitdice		);  /* ROM */
DECLARE_OLC_FUN( medit_damdice		);  /* ROM */
DECLARE_OLC_FUN( medit_race		);  /* ROM */
DECLARE_OLC_FUN( medit_position		);  /* ROM */
DECLARE_OLC_FUN( medit_gold		);  /* ROM */
DECLARE_OLC_FUN( medit_hitroll		);  /* ROM */
DECLARE_OLC_FUN( medit_damtype		);  /* ROM */
DECLARE_OLC_FUN( medit_group		);  /* ROM */
DECLARE_OLC_FUN( medit_addmprog		);  /* ROM */
DECLARE_OLC_FUN( medit_delmprog		);  /* ROM */
DECLARE_OLC_FUN( medit_speaking		);  /* ROM */
DECLARE_OLC_FUN( medit_languages	);  /* ROM */
DECLARE_OLC_FUN( medit_spell		);  /* ROM */
DECLARE_OLC_FUN( medit_clone		);  /* ROM */
DECLARE_OLC_FUN( medit_progpos		);  /* ROM */

DECLARE_OLC_FUN( medit_dammagic		);  /* ROM */
DECLARE_OLC_FUN( medit_damweapon	);  /* ROM */
DECLARE_OLC_FUN( medit_damflags		);  /* ROM */
DECLARE_OLC_FUN( medit_addcomment   );  /* ROM */
DECLARE_OLC_FUN( medit_delcomment   );  /* ROM */
DECLARE_OLC_FUN( medit_hpcalc       );  /* ROM */

/* Mobprog editor */
DECLARE_OLC_FUN( mpedit_create		);
DECLARE_OLC_FUN( mpedit_code		);
DECLARE_OLC_FUN( mpedit_show		);
DECLARE_OLC_FUN( mpedit_list		);
DECLARE_OLC_FUN( mpedit_description	);
DECLARE_OLC_FUN( mpedit_name		);
DECLARE_OLC_FUN( mpedit_area		);
DECLARE_OLC_FUN( mpedit_varcmd		);

/* Objprog editor */
DECLARE_OLC_FUN( opedit_create		);
DECLARE_OLC_FUN( opedit_code		);
DECLARE_OLC_FUN( opedit_show		);
DECLARE_OLC_FUN( opedit_list		);
DECLARE_OLC_FUN( opedit_description	);
DECLARE_OLC_FUN( opedit_name		);
DECLARE_OLC_FUN( opedit_area		);
DECLARE_OLC_FUN( opedit_varcmd		);

/* Roomprog editor */
DECLARE_OLC_FUN( rpedit_create		);
DECLARE_OLC_FUN( rpedit_code		);
DECLARE_OLC_FUN( rpedit_show		);
DECLARE_OLC_FUN( rpedit_list		);
DECLARE_OLC_FUN( rpedit_description	);
DECLARE_OLC_FUN( rpedit_name		);
DECLARE_OLC_FUN( rpedit_area		);
DECLARE_OLC_FUN( rpedit_varcmd		);

/* Trap stuff */
DECLARE_OLC_FUN( tedit_create		);
DECLARE_OLC_FUN( tedit_show		);
DECLARE_OLC_FUN( tedit_list		);
DECLARE_OLC_FUN( tedit_level		);
DECLARE_OLC_FUN( tedit_active		);
DECLARE_OLC_FUN( tedit_type		);
DECLARE_OLC_FUN( tedit_triggered	);
DECLARE_OLC_FUN( tedit_failed		);
DECLARE_OLC_FUN( tedit_disarmed		);
DECLARE_OLC_FUN( tedit_limit		);

/* Desc stuff */
DECLARE_OLC_FUN( rdedit_create		);
DECLARE_OLC_FUN( rdedit_show	    );
DECLARE_OLC_FUN( rdedit_list	    );
DECLARE_OLC_FUN( rdedit_randdesc    );
DECLARE_OLC_FUN( rdedit_adddesc   	);
DECLARE_OLC_FUN( rdedit_deldesc    	);
DECLARE_OLC_FUN( rdedit_eddesc   	);
DECLARE_OLC_FUN( rdedit_showdesc    );
DECLARE_OLC_FUN( rdedit_listdesc    );
DECLARE_OLC_FUN( rdedit_chgroup     );
DECLARE_OLC_FUN( rdedit_clonedesc   );

/* Bonus set stuff */
DECLARE_OLC_FUN( bsedit_create      );
DECLARE_OLC_FUN( bsedit_addaffect   );
DECLARE_OLC_FUN( bsedit_addapply    );
DECLARE_OLC_FUN( bsedit_delaffect   );
DECLARE_OLC_FUN( bsedit_description );
DECLARE_OLC_FUN( bsedit_wearprog    );
DECLARE_OLC_FUN( bsedit_removeprog  );
DECLARE_OLC_FUN( bsedit_addcomment  );
DECLARE_OLC_FUN( bsedit_delcomment  );
DECLARE_OLC_FUN( bsedit_show        );
DECLARE_OLC_FUN( bsedit_list        );
DECLARE_OLC_FUN( bsedit_name        );
DECLARE_OLC_FUN( bsedit_object      );
DECLARE_OLC_FUN( bsedit_ed          );

/* Song stuff */
DECLARE_OLC_FUN( sedit_create      );
DECLARE_OLC_FUN( sedit_description );
DECLARE_OLC_FUN( sedit_addcomment  );
DECLARE_OLC_FUN( sedit_delcomment  );
DECLARE_OLC_FUN( sedit_show        );
DECLARE_OLC_FUN( sedit_list        );
DECLARE_OLC_FUN( sedit_name        );
DECLARE_OLC_FUN( sedit_type        );
DECLARE_OLC_FUN( sedit_slots       );
DECLARE_OLC_FUN( sedit_target      );
DECLARE_OLC_FUN( sedit_perform     );

DECLARE_OLC_FUN( hbedit_create      );
DECLARE_OLC_FUN( hbedit_show        );
DECLARE_OLC_FUN( hbedit_list        );
DECLARE_OLC_FUN( hbedit_id          );
DECLARE_OLC_FUN( hbedit_name        );
DECLARE_OLC_FUN( hbedit_odmiana     );

/* Editor de helps */
DECLARE_OLC_FUN( hedit_keyword      );
DECLARE_OLC_FUN( hedit_text         );
DECLARE_OLC_FUN( hedit_new          );
DECLARE_OLC_FUN( hedit_level        );
DECLARE_OLC_FUN( hedit_delete       );
DECLARE_OLC_FUN( hedit_show         );
DECLARE_OLC_FUN( hedit_list         );

/*
 * Macros
 */
/* Return pointers to what is being edited. */
#define EDIT_MOB(Ch, Mob)	( Mob = (MOB_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_OBJ(Ch, Obj)	( Obj = (OBJ_INDEX_DATA *)Ch->desc->pEdit )
#define EDIT_ROOM(Ch, Room)	( Room = Ch->in_room )
#define EDIT_AREA(Ch, Area)	( Area = (AREA_DATA *)Ch->desc->pEdit )

#define EDIT_MPCODE(Ch, Code)   ( Code = (PROG_CODE*)Ch->desc->pEdit )
#define EDIT_OPCODE(Ch, Code)   ( Code = (PROG_CODE*)Ch->desc->pEdit )
#define EDIT_RPCODE(Ch, Code)   ( Code = (PROG_CODE*)Ch->desc->pEdit )
#define EDIT_TRAPDATA(Ch, Code)   ( Code = (TRAP_DATA*)Ch->desc->pEdit )
#define EDIT_RDESCDATA(Ch, Code)   ( Code = (RAND_DESC_DATA*)Ch->desc->pEdit )
#define EDIT_BSDATA(Ch, Code)   ( Code = (BONUS_INDEX_DATA*)Ch->desc->pEdit )
#define EDIT_SDATA(Ch, Code)   ( Code = (SONG_DATA*)Ch->desc->pEdit )
#define EDIT_HBDATA(Ch, Code)   ( Code = (HERB_DATA*)Ch->desc->pEdit )

/*
 * Prototypes
 */
/* mem.c - memory prototypes. */
#define ED	EXTRA_DESCR_DATA
RESET_DATA	*new_reset_data		args ( ( void ) );
void		free_reset_data		args ( ( RESET_DATA *pReset ) );
AREA_DATA	*new_area		args ( ( void ) );
void		free_area		args ( ( AREA_DATA *pArea ) );
EXIT_DATA	*new_exit		args ( ( void ) );
void		free_exit		args ( ( EXIT_DATA *pExit ) );
ED 		*new_extra_descr	args ( ( void ) );
void		free_extra_descr	args ( ( ED *pExtra ) );
ROOM_INDEX_DATA *new_room_index		args ( ( void ) );
void		free_room_index		args ( ( ROOM_INDEX_DATA *pRoom ) );
AFFECT_DATA	*new_affect		args ( ( void ) );
void		free_affect		args ( ( AFFECT_DATA* pAf ) );
SHOP_DATA	*new_shop		args ( ( void ) );
void		free_shop		args ( ( SHOP_DATA *pBank ) );
BANK_DATA	*new_bank		args ( ( void ) );
void		free_bank		args ( ( BANK_DATA *pBank ) );
OBJ_INDEX_DATA	*new_obj_index		args ( ( void ) );
void		free_obj_index		args ( ( OBJ_INDEX_DATA *pObj ) );
MOB_INDEX_DATA	*new_mob_index		args ( ( void ) );
void		free_mob_index		args ( ( MOB_INDEX_DATA *pMob ) );
#undef	ED

void show_liqlist     args ( ( CHAR_DATA *ch ) );
void show_weedlist    args ( ( CHAR_DATA *ch ) );
void show_damlist     args ( ( CHAR_DATA *ch ) );
void show_armor_list  args ( ( CHAR_DATA *ch ) );
void show_poison_list args ( ( CHAR_DATA *ch ) );
void show_sector_list args ( ( CHAR_DATA *ch ) );

PROG_LIST      *new_prog               args ( ( void ) );
void            free_prog               args ( ( PROG_LIST *mp ) );
PROG_CODE	*new_pcode		args ( (void) );
void		free_pcode		args ( ( PROG_CODE *pMcode));

TRAP_DATA	*new_trapdata		args ( (void) );
void         free_trapdata      args( ( TRAP_DATA *trap ) );

RAND_DESC_DATA	*new_rdescdata		args ( (void) );
void            free_rdescdata      args( ( RAND_DESC_DATA *rdesc ) );

DESC_DATA	    *new_descdata		args ( (void) );
void            free_descdata       args( ( DESC_DATA *desc ) );

BONUS_INDEX_DATA *new_bonus_index   args( ( void ) );
void free_bonus_index               args( ( BONUS_INDEX_DATA *pBonus ) );

SONG_DATA        *new_song_index    args( ( void ) );
void             free_song_index    args( ( SONG_DATA *pSong ) );
