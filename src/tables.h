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
 * Skrzetnicki Krzysztof (gtener@gmail.com              ) [Tener     ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: tables.h 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/tables.h $
 *
 */
struct flag_type
{
    char *name;
    int bit;
    bool settable;
};

struct flag_type_desc
{
    char *name;
    int bit;
    bool settable;
    char *desc;
};

struct ext_flag_type
{
    char *name;
    BITVECT_DATA *bitvect;
    bool settable;
};

struct position_type
{
    char *name;
    char *short_name;
};

struct sex_type
{
    char *name;
};

struct gender_type
{
    char *name;
};

struct size_type
{
    char *name;
};

struct stat_type
{
    char *name;
};

struct	bit_type
{
	const	struct	flag_type *	table;
	const 	struct  ext_flag_type * sec_table;
	char *	help;
};


extern const int stat_hash [MAX_STATS][MAX_STATS];
/* game tables */
extern	const	struct	position_type	position_table[];
extern	const	struct	sex_type	sex_table[];
extern	const	struct	gender_type	gender_table[];
extern	const	struct	size_type	size_table[];
extern	const	struct	stat_type	stat_table[];

/* flag tables */
extern	const	struct	flag_type	plr_flags[];
extern	const	struct	flag_type	imm_flags[];
extern	const	struct	flag_type	form_flags[];
extern	const	struct	flag_type	part_flags[];
extern	const	struct	flag_type	comm_flags[];
extern	const	struct	flag_type	wear_flags[];
extern	const	struct	flag_type	weapon_flags[];
extern	const	struct	flag_type	container_flags[];
extern	const	struct	flag_type	portal_flags[];
extern	const	struct	flag_type	exit_flags[];
extern	const	struct	flag_type	area_flags[];
extern	const	struct	flag_type	door_resets[];
extern	const	struct	flag_type	wear_loc_strings[];
extern	const	struct	flag_type	wear_loc_flags[];
extern	const	struct	flag_type	corpse_flags[];
extern	const	struct	flag_type	imm_flags[];
extern	const	struct	flag_type	type_flags[];
extern	const	struct	flag_type	apply_flags[];
extern	const	struct	flag_type	sex_flags[];
extern	const	struct	flag_type	furniture_flags[];
extern	const	struct	flag_type	weapon_class[];
extern	const	struct	flag_type	apply_types[];
extern	const	struct	flag_type	weapon_type2[];
extern	const	struct	flag_type	apply_types[];
extern	const	struct	flag_type	size_flags[];
extern	const	struct	flag_type	position_flags[];
extern	const	struct	flag_type	position_trig_flags[];
extern	const	struct	flag_type	ac_type[];
extern	const	struct	bit_type	bitvector_type[];
extern	const	struct	flag_type	prewait_target[];
extern	const	struct	flag_type	dam_types[];
extern	const	struct	flag_type_desc	instrument_class[];
extern	const	struct	flag_type	wand_class[];
extern	const	struct	flag_type	wand_extra[];
extern   const struct   flag_type   totem_table[];

/* extended flags tables */
extern	const	struct	ext_flag_type	affect_flags[];
extern	const	struct	ext_flag_type	act_flags[];
extern	const	struct	ext_flag_type	off_flags[];
extern	const	struct	ext_flag_type	extra_flags[];
extern	const	struct	ext_flag_type	wear_flags2[];
extern	const	struct	ext_flag_type	feat_flags[];
extern	const	struct	ext_flag_type	prog_flags[];
extern	const	struct	ext_flag_type	room_flags[];

