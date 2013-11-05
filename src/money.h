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
 * money.h and associated patches copyright 2009 by Gurthg             *
 *                                                                     *
 * In order to use any part of this ROM Merc Diku code you must comply *
 * the original Diku license in 'license.doc' as well the Merc license *
 * in 'license.txt' and also the ROM license in 'rom.license', each to *
 * be found in doc/. Using the reward.c code without conforming to the *
 * requirements of each of these documents is violation of any and all *
 * applicable copyright laws. In particular, you may not remove any of *
 * these copyright notices or claim other's work as your own.          *
 *                                                                     *
 * Much time and thought has gone into this software you are using.    *
 * We hope that you share your improvements, too.                      *
 * "What goes around, comes around."                                   *
 *                                                                     *
 * KILLER MUD is copyright 2009-2011 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * ZMIENIA£E¦ CO¦? DOPISZ SIÊ!                                         *
 *                                                                     *
 * Pietrzak Marcin       (marcin.pietrzak.pl              ) [Gurthg  ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: money.h 7720 2009-07-11 14:25:37Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/branches/09.06_money/src/money.h $
 *
 */
/**
 * nowy sytem monetarny
 *
 * ratting vs cooper
 *
 * 1c
 * 1s = 60c
 * 1g = 15s = 900c
 * 1m = 12g = 180s = 10800c
 *
 */

#define RATTING_SILVER_TO_COPPER 60
#define RATTING_GOLD_TO_SILVER   15
#define RATTING_MITHRIL_TO_GOLD  12

#define RATTING_NONE    0
#define RATTING_COPPER  1
#define RATTING_SILVER  RATTING_SILVER_TO_COPPER * RATTING_COPPER 
#define RATTING_GOLD    RATTING_GOLD_TO_SILVER   * RATTING_SILVER
#define RATTING_MITHRIL RATTING_MITHRIL_TO_GOLD  * RATTING_GOLD

#define OBJ_VNUM_COIN_COPPER  1
#define OBJ_VNUM_COIN_SILVER  2
#define OBJ_VNUM_COIN_GOLD    3
#define OBJ_VNUM_COIN_MITHRIL 4 
#define OBJ_VNUM_COINS        5

#define NOMINATION_NONE       0
#define NOMINATION_COPPER     1
#define NOMINATION_SILVER     2
#define NOMINATION_GOLD       3
#define NOMINATION_MITHRIL    4

bool     money_from_char_to_char                 args ( ( CHAR_DATA *ch, CHAR_DATA *victim, long int amount, int nomination ) );
char *   money_nomination                        args ( ( int nomination, long int amount ) );
char *   money_string                            args ( ( long int amount, bool colorize ) );
char *   money_string_simple                     args ( ( long int amount, int nomination, bool colorize ) );
char *   money_string_short                      args ( ( long int amount ) );
int      money_count_weight_carried_by_character args ( ( CHAR_DATA *ch ) ); 
int      money_nomination_find                   args ( ( char arg [ MAX_INPUT_LENGTH ] ) );
int      money_weight                            args ( ( long int copper, long int silver, long int gold, long int mithril ) );
long int money_copper_from_copper                args ( ( long int copper ) );
long int money_count_copper_all                  args ( ( CHAR_DATA *ch ) );
long int money_count_copper                      args ( ( CHAR_DATA *ch ) );
long int money_count_number_of_coinz             args ( ( OBJ_DATA *obj ) );
long int money_gold_from_copper                  args ( ( long int copper ) );
long int money_mithril_from_copper               args ( ( long int copper ) );
long int money_silver_from_copper                args ( ( long int copper ) );
long int money_to_copper                         args ( ( long int copper, long int silver, long int gold, long int mithril ) );
long int money_to_copper_by_nomination           args ( ( long int amount, int nomination ) );
void     money_from_container                    args ( ( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container ) );
void     money_gain_from_obj                     args ( ( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container ) );
void     money_gain                              args ( ( CHAR_DATA *ch, long int copper ) );
void     money_reduce                            args ( ( CHAR_DATA *ch, long int money ) );
void     money_reset_character_money             args ( ( CHAR_DATA *ch ) );
void     money_split                             args ( ( CHAR_DATA *ch, long int amount, int nomination, bool show_message, bool is_split_all, bool is_autosplit ) );
void     money_steal                             args ( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void     money_transfer_spec_nasty               args ( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

#define MONEY_COLOR( nomination )   ( nomination == NOMINATION_MITHRIL ? "{C" : nomination == NOMINATION_GOLD ? "{Y" : nomination == NOMINATION_SILVER ? "{w" : nomination == NOMINATION_COPPER ? "{y" : "{x" )

