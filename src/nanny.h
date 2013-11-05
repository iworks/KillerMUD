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
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: nanny.h 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/nanny.h $
 *
 */
#ifdef NEW_NANNY
typedef	int NANNY_FUN	args( ( DESCRIPTOR_DATA *d, char *argument ) );

#define DECLARE_NANNY_FUN( fun )    NANNY_FUN fun
#define NANNY( fun )                int fun( DESCRIPTOR_DATA *d, char *argument )

#define NANNY_EXIT						-1
#define NANNY_FIRST_INPUT				10
#define NANNY_CHECK_PLAYER_PASSWD		1
#define NANNY_CHECK_ACCOUNT_PASSWD		2
#define NANNY_NEW_ACCOUNT				3
#define NANNY_PLAYER_MENU				4
#define NANNY_ACCOUNT_MENU				5
#define NANNY_NEWCHAR_MENU				6
#define NANNY_CHOOSE_PLAYER				7
#define NANNY_PLAYER_SALVAGE			8
#define NANNY_CHECK_SALVAGE_PASSWD		9
#define NANNY_CHACC_PASSWD_OLD			11
#define NANNY_CHACC_PASSWD_NEW			12
#define NANNY_CHACC_PASSWD_CONFIRM		13
#define NANNY_DELETE_PLAYER				14
#define NANNY_CONFIRM_DELETE_PLAYER		15
#define NANNY_DELETE_ACCOUNT			16
#define NANNY_CHEMAIL_PASSWD_CONFIRM	17
#define NANNY_CHANGE_EMAIL				18
#define NANNY_ACTIVATE_ACCOUNT			19
#define NANNY_NEWACC_PASSWD				20
#define NANNY_NEWACC_PASSWD_CONFIRM		21
#define NANNY_NEWCHAR_NAME				22
#define NANNY_NEWCHAR_INFLECT			23
#define NANNY_NEWCHAR_CONFIRM_INFLECT	24
#define NANNY_NEWCHAR_SEX				25
#define NANNY_NEWCHAR_RACE				26
#define NANNY_NEWCHAR_CONFIRM_RACE		27
#define NANNY_NEWCHAR_CLASS				28
#define NANNY_NEWCHAR_CONFIRM_CLASS		29
#define NANNY_NEWCHAR_SPEC				30
#define NANNY_NEWCHAR_ALIGN				31
#define NANNY_NEWCHAR_PASSWORD			32
#define NANNY_NEWCHAR_PASSWORD_CONFIRM	33
#define NANNY_NEWCHAR_ACCEPT			34
#define NANNY_NEWCHAR_RESIGN			35
#define NANNY_PLAYER_CHANGE_NAME		36
#define NANNY_NEWCHAR_RESERVED_PWD		37
#define NANNY_PLAYER_RESERVED_PWD		38
#define NANNY_PLAYER_INFLECT			39
#define NANNY_PLAYER_CONFIRM_INFLECT	40

//menukonta
#define ACC_CREATE_PLAYER				1
#define ACC_SHOW_CHARACTERS				2
#define ACC_CHARACTER_MENU				3
#define ACC_SALVAGE_CHARACTER			4
#define ACC_DELETE_CHARACTER			5
#define ACC_LOGOUT_CHARACTERS			6
#define ACC_CHANGE_ACCOUNT_PASSWD		7
#define ACC_SEND_ACCOUNT_PASSWD			8
#define ACC_CHANGE_ACCOUNT_EMAIL		9
#define ACC_DELETE_ACCOUNT				10
#define ACC_REPORT_ERROR				11
#define ACC_ACTIVATE_ACCOUNT			12
#define ACC_SEND_ACTIVATION_CODE		13
#define ACC_LOGOUT						0

//menu gracza
#define CHA_PLAY_OR_CHANGE_NAME			1
#define CHA_SCORE						2
#define CHA_LOGOUT						0

//menu tworzenia postaci
#define CRE_NAME						1
#define CRE_INFLECT						2
#define CRE_SEX							3
#define CRE_RACE						4
#define CRE_CLASS						5
#define CRE_SPEC						6
#define CRE_ALIGN						7
#define CRE_CHARACTERISTIC				8
#define CRE_PASSWORD					9
#define CRE_ACCEPT						10
#define CRE_RESIGN						0

//opcje tworzenia postaci
#define CREATE_NAME						(A)
#define CREATE_INFLECT					(B)
#define CREATE_SEX						(C)
#define CREATE_RACE						(D)
#define CREATE_CLASS					(E)
#define CREATE_CHARACTERISTIC			(F)
#define CREATE_SPEC						(G)
#define CREATE_ALIGN					(H)
#define CREATE_PASSWORD					(I)


//typy wiadomosci mailowych do aktywacji konta
#define MAIL_NEW_ACCOUNT				1
#define MAIL_CHANGE_ACCOUNT				2
#define MAIL_REMIND_CODE				3

//typy wiadomosci mailowych do przypominania hase³
#define MAIL_ACCOUNT_PASSWD_MENU		1
#define MAIL_ACCOUNT_PASSWD_LOGIN		2
#define MAIL_CHAR_PASSWD_MENU			3
#define MAIL_CHAR_PASSWD_LOGIN			4

struct nanny_cmd_type
{
    int         state;
    NANNY_FUN * nanny_fun;
};

/* nanny.c */
void new_nanny( DESCRIPTOR_DATA *d, char *argument );

extern const struct nanny_cmd_type  nanny_table[];

/* functions */
void disconnect_account_players     args( ( DESCRIPTOR_DATA *d, bool verbose ) );
void show_header                    args( ( DESCRIPTOR_DATA *d, char* header, char* plot, \
                                            char* inside_plot, char colorh, char color1,  \
                                            char color2 ) );
void    show_player_menu            args( ( DESCRIPTOR_DATA *d ) );
void    show_account_menu           args( ( DESCRIPTOR_DATA *d ) );
void    show_newchar_menu           args( ( DESCRIPTOR_DATA *d ) );
int     show_players_list           args( ( DESCRIPTOR_DATA *d, bool verbose ) );

/* nanny functions */
DECLARE_NANNY_FUN( nanny_exit );
DECLARE_NANNY_FUN( nanny_first_input );
DECLARE_NANNY_FUN( nanny_check_player_passwd );
DECLARE_NANNY_FUN( nanny_check_account_passwd );
DECLARE_NANNY_FUN( nanny_new_account );
DECLARE_NANNY_FUN( nanny_account_menu );
DECLARE_NANNY_FUN( nanny_newchar_menu );
DECLARE_NANNY_FUN( nanny_choose_player );
DECLARE_NANNY_FUN( nanny_player_salvage );
DECLARE_NANNY_FUN( nanny_check_salvage_passwd );
DECLARE_NANNY_FUN( nanny_chacc_passwd_old );
DECLARE_NANNY_FUN( nanny_chacc_passwd_new );
DECLARE_NANNY_FUN( nanny_chacc_passwd_confirm );
DECLARE_NANNY_FUN( nanny_delete_player );
DECLARE_NANNY_FUN( nanny_confirm_delete_player );
DECLARE_NANNY_FUN( nanny_delete_account );
DECLARE_NANNY_FUN( nanny_chemail_passwd_confirm );
DECLARE_NANNY_FUN( nanny_change_email );
DECLARE_NANNY_FUN( nanny_activate_account );
DECLARE_NANNY_FUN( nanny_newacc_passwd );
DECLARE_NANNY_FUN( nanny_newacc_passwd_confirm );
DECLARE_NANNY_FUN( nanny_newchar_name );
DECLARE_NANNY_FUN( nanny_newchar_reserved_pwd );
DECLARE_NANNY_FUN( nanny_newchar_inflect );
DECLARE_NANNY_FUN( nanny_newchar_confirm_inflect );
DECLARE_NANNY_FUN( nanny_newchar_sex );
DECLARE_NANNY_FUN( nanny_newchar_race );
DECLARE_NANNY_FUN( nanny_newchar_confirm_race );
DECLARE_NANNY_FUN( nanny_newchar_class );
DECLARE_NANNY_FUN( nanny_newchar_confirm_class );
DECLARE_NANNY_FUN( nanny_newchar_spec );
DECLARE_NANNY_FUN( nanny_newchar_align );
DECLARE_NANNY_FUN( nanny_newchar_password );
DECLARE_NANNY_FUN( nanny_newchar_password_confirm );
DECLARE_NANNY_FUN( nanny_newchar_accept );
DECLARE_NANNY_FUN( nanny_newchar_resign );
DECLARE_NANNY_FUN( nanny_player_menu );
DECLARE_NANNY_FUN( nanny_player_change_name );
DECLARE_NANNY_FUN( nanny_player_reserved_pwd );
DECLARE_NANNY_FUN( nanny_player_inflect );
DECLARE_NANNY_FUN( nanny_player_confirm_inflect );
#endif
