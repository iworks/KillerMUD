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
 * Pietrzak Marcin       (marcin.pietrzak@mud.pl        ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Skrzetnicki Krzysztof (tener@tenet.pl                ) [Tener     ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *                                                                     *
 * this is a listing of all the commands and command related data      *
 * wrapper function for safe command execution                         *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: interp.h 10701 2011-12-02 16:03:39Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/tags/12.02/src/interp.h $
 *
 */
void do_function args((CHAR_DATA *ch, DO_FUN *do_fun, char *argument));
//rellik: do debugowania, nie wiem czy tu moze byc deklaracja funkcji (definicja w act_wiz)
void save_debug_info(const char *actual, const char *caller, char *txt,	int nr_proj, int debuglevel, bool include_info);

/* for command types */
#define ML 	MAX_LEVEL	/* implementor */
#define L1	MAX_LEVEL - 1  	/* creator */
#define L2	MAX_LEVEL - 2	/* supreme being */
#define L3	MAX_LEVEL - 3	/* deity */
#define L4 	MAX_LEVEL - 4	/* god */
#define L5	MAX_LEVEL - 5	/* immortal */
#define L6	MAX_LEVEL - 6	/* demigod */
#define L7	MAX_LEVEL - 7	/* angel */
#define L8	MAX_LEVEL - 8	/* avatar */
#define IM	LEVEL_IMMORTAL 	/* avatar */
#define HE	LEVEL_HERO	/* hero */

#define COM_INGORE	1

/* jesli ktos wie to prosze o krotkie komentarze kiedy tego uzywac, zweryfikowac moje hipotezy */
#define OPT_INFORMATION     	(A)				/* nie przerywa healing touch, medit itp */
#define OPT_HIDEHELP        	(B)				/* nie wyswietla helpa o komendzie? */
#define OPT_MINIHOLD        	(C)				/* maly wait po komendzie? */
#define OPT_NEEDHANDS       	(D)				/* potrzebuje rak */
#define OPT_NIEDLAZAKUTYCH  	(E)				/* nie dla tych w kajdanach */
#define OPT_REM_LISTEN      	(F)				/* PROSZE O KOMNETARZ */
#define OPT_MAY_REM_LISTEN  	(G)				/* PROSZE O KOMNETARZ */
#define OPT_NOT_IN_FORCE_FIELD  (H)			/* nie w force field */
#define OPT_COMMUNICATION		(I)					/* PROSZE O KOMNETARZ */
#define OPT_MIND_MOVE			(J)						/* moze byc wykonane podczas gdy char jest mindmove */
#define NIE_DLA_SHAPESHIFTED 	(K)				/* nie dla przemienionych w zwierzaczki */
#define OPT_DZIALANIE_OFENSYWNE (L)			/* wskazuje, ze to jest komenda ofensywna, nie mozna np. przy fear */
#define OPT_RECUPERATE          (M)			/* PROSZE O KOMNETARZ */

/*
 * Structure for a command in the command lookup table.
 */
struct	cmd_type
{
    char * const	name;
    char * const	name_pl;
    DO_FUN *		do_fun;
    sh_int		position;
    sh_int		level;
    sh_int		log;
    int         options;
};

/* the command table itself */
extern	const	struct	cmd_type	cmd_table	[];

/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN(	do_advance	);
DECLARE_DO_FUN( do_affects	);
DECLARE_DO_FUN( do_afk		);
DECLARE_DO_FUN( do_alia		);
DECLARE_DO_FUN( do_alias	);
DECLARE_DO_FUN(	do_allow	);
DECLARE_DO_FUN( do_answer	);
DECLARE_DO_FUN(	do_areas	);
DECLARE_DO_FUN(	do_at		);
DECLARE_DO_FUN(	do_auction	);
/*DECLARE_DO_FUN( do_autoassist	);
DECLARE_DO_FUN( do_autoexit	);
DECLARE_DO_FUN( do_autogold	);
DECLARE_DO_FUN( do_automem	);*/
DECLARE_DO_FUN( do_config	);
/*DECLARE_DO_FUN( do_autoloot	);
DECLARE_DO_FUN( do_autosac	);
DECLARE_DO_FUN( do_autosplit	);*/
DECLARE_DO_FUN(	do_backstab	);
DECLARE_DO_FUN( do_sap		);
DECLARE_DO_FUN(	do_circle	);
DECLARE_DO_FUN(	do_bamfin	);
DECLARE_DO_FUN(	do_bamfout	);
DECLARE_DO_FUN( do_bash		);
DECLARE_DO_FUN( do_berserk	);
DECLARE_DO_FUN( do_wardance	);
DECLARE_DO_FUN( do_damage	);
DECLARE_DO_FUN( do_slam		);
DECLARE_DO_FUN(	do_brandish	);
/*DECLARE_DO_FUN( do_brief	);*/
DECLARE_DO_FUN(	do_bug		);
DECLARE_DO_FUN(	do_buy		);
DECLARE_DO_FUN(	do_cast		);
DECLARE_DO_FUN( do_changes	);
DECLARE_DO_FUN( do_clone	);
DECLARE_DO_FUN(	do_close	);
DECLARE_DO_FUN(	do_colour	);
DECLARE_DO_FUN(	do_commands	);
/*DECLARE_DO_FUN( do_combine	);
DECLARE_DO_FUN( do_compact	);*/
DECLARE_DO_FUN(	do_consider	);
DECLARE_DO_FUN( do_copyover 	);
DECLARE_DO_FUN(	do_credits	);
DECLARE_DO_FUN( do_delete	);
DECLARE_DO_FUN(	do_deny		);
DECLARE_DO_FUN(	do_description	);
DECLARE_DO_FUN(	do_disarm	);
DECLARE_DO_FUN(	do_disconnect	);
DECLARE_DO_FUN(	do_down		);
DECLARE_DO_FUN(	do_drink	);
DECLARE_DO_FUN(	do_drop		);
DECLARE_DO_FUN( do_dump		);
DECLARE_DO_FUN(	do_east		);
DECLARE_DO_FUN(	do_eat		);
DECLARE_DO_FUN(	do_devour	);
DECLARE_DO_FUN(	do_echo		);
DECLARE_DO_FUN(	do_emote	);
DECLARE_DO_FUN(	do_empty	);
DECLARE_DO_FUN( do_enter	);
DECLARE_DO_FUN( do_envenom	);
DECLARE_DO_FUN(	do_equipment	);
DECLARE_DO_FUN(	do_examine	);
DECLARE_DO_FUN(	do_exits	);
DECLARE_DO_FUN(	do_fill		);
DECLARE_DO_FUN( do_flag		);
DECLARE_DO_FUN(	do_flee		);
DECLARE_DO_FUN(	do_follow	);
DECLARE_DO_FUN(	do_force	);
DECLARE_DO_FUN(	do_freeze	);
DECLARE_DO_FUN(	do_hoard	);
DECLARE_DO_FUN(	do_unhoard	);
DECLARE_DO_FUN( do_gain		);
DECLARE_DO_FUN(	do_get		);
DECLARE_DO_FUN(	do_give		);
DECLARE_DO_FUN( do_gossip	);
DECLARE_DO_FUN(	do_goto		);
DECLARE_DO_FUN( do_grats	);
DECLARE_DO_FUN(	do_group	);
DECLARE_DO_FUN( do_groups	);
DECLARE_DO_FUN(	do_gtell	);
DECLARE_DO_FUN(	do_clantell	);
DECLARE_DO_FUN(	do_help		);
DECLARE_DO_FUN(	do_hide		);
DECLARE_DO_FUN(	do_holylight	);
DECLARE_DO_FUN( do_home     );
DECLARE_DO_FUN(	do_idea		);
DECLARE_DO_FUN(	do_ide		);
DECLARE_DO_FUN(	do_immtalk	);
DECLARE_DO_FUN( do_incognito	);
DECLARE_DO_FUN( do_inflect	);
DECLARE_DO_FUN( do_imotd	);
DECLARE_DO_FUN(	do_inventory	);
DECLARE_DO_FUN(	do_invis	);
DECLARE_DO_FUN(	do_kick		);
DECLARE_DO_FUN(	do_crush	);
DECLARE_DO_FUN(	do_tail	);
DECLARE_DO_FUN(	do_skin		);
DECLARE_DO_FUN(	do_carve		); //wycinanie z cia³ czê¶ci do cooking lub alchemy
DECLARE_DO_FUN(	do_ucho	);
DECLARE_DO_FUN( do_trophy	);
DECLARE_DO_FUN(	do_flurry	);
DECLARE_DO_FUN(	do_concentration );
DECLARE_DO_FUN(	do_charge	);
DECLARE_DO_FUN(	do_smite	);
DECLARE_DO_FUN(	do_stun		);
DECLARE_DO_FUN(	do_lay		);
DECLARE_DO_FUN(	do_garhal	);
DECLARE_DO_FUN(	do_sharpen	);
DECLARE_DO_FUN(	do_meditate	);
DECLARE_DO_FUN(	do_recuperate	);
DECLARE_DO_FUN(	do_mend		);
DECLARE_DO_FUN(	do_kill		);
DECLARE_DO_FUN(	do_assist	);
DECLARE_DO_FUN(	do_list		);
DECLARE_DO_FUN( do_load		);
DECLARE_DO_FUN(	do_lock		);
DECLARE_DO_FUN(	do_log		);
DECLARE_DO_FUN(	do_look		);
DECLARE_DO_FUN(	do_peek		);
DECLARE_DO_FUN(	do_memory	);
DECLARE_DO_FUN(	do_mfind	);
DECLARE_DO_FUN(	do_mload	);
DECLARE_DO_FUN(	do_mset		);
DECLARE_DO_FUN(	do_mstat	);
DECLARE_DO_FUN(	do_mwhere	);
DECLARE_DO_FUN( do_mob		);
DECLARE_DO_FUN( do_motd		);
DECLARE_DO_FUN( do_mpstat	);
DECLARE_DO_FUN( do_opstat	);
DECLARE_DO_FUN( do_opdump	);
DECLARE_DO_FUN( do_rpstat	);
DECLARE_DO_FUN( do_rpdump	);
DECLARE_DO_FUN( do_mpdump	);
DECLARE_DO_FUN(	do_murde	);
DECLARE_DO_FUN( do_music	);
DECLARE_DO_FUN( do_newlock	);
DECLARE_DO_FUN( do_news		);
DECLARE_DO_FUN( do_nochannels	);
DECLARE_DO_FUN(	do_noemote	);
DECLARE_DO_FUN( do_nofollow	);
DECLARE_DO_FUN( do_noloot	);
DECLARE_DO_FUN(	do_north	);
DECLARE_DO_FUN(	do_noshout	);
/*DECLARE_DO_FUN( do_nosummon	);*/
DECLARE_DO_FUN(	do_note		);
DECLARE_DO_FUN(	do_blocktell	);
DECLARE_DO_FUN( do_blocknewbie );
DECLARE_DO_FUN(	do_ofind	);
DECLARE_DO_FUN(	do_oload	);
DECLARE_DO_FUN(	do_open		);
DECLARE_DO_FUN(	do_order	);
DECLARE_DO_FUN(	do_oset		);
DECLARE_DO_FUN(	do_ostat	);
DECLARE_DO_FUN( do_outfit	);
DECLARE_DO_FUN( do_owhere	);
DECLARE_DO_FUN(	do_pardon	);
DECLARE_DO_FUN(	do_password	);
DECLARE_DO_FUN(	do_peace	);
DECLARE_DO_FUN( do_pecho	);
DECLARE_DO_FUN( do_penalty	);
DECLARE_DO_FUN( do_permban	);
DECLARE_DO_FUN(	do_pick		);
DECLARE_DO_FUN( do_pmote	);
DECLARE_DO_FUN( do_pour		);
DECLARE_DO_FUN(	do_learn	);
DECLARE_DO_FUN(	do_study	);
DECLARE_DO_FUN(	do_reservation	);
DECLARE_DO_FUN(	do_memorize	);
DECLARE_DO_FUN( do_lstat	);
DECLARE_DO_FUN( do_write	);
DECLARE_DO_FUN( do_board	);
DECLARE_DO_FUN( do_connect	);
DECLARE_DO_FUN( do_drag		);
DECLARE_DO_FUN( do_read		);
DECLARE_DO_FUN( do_lset 	);
DECLARE_DO_FUN( do_fstat 	);
DECLARE_DO_FUN( do_fset 	);
DECLARE_DO_FUN( do_prefix	);
DECLARE_DO_FUN( do_prompt	);
DECLARE_DO_FUN( do_protect	);
DECLARE_DO_FUN(	do_purge	);
DECLARE_DO_FUN(	do_put		);
DECLARE_DO_FUN(	do_quaff	);
DECLARE_DO_FUN( do_question	);
DECLARE_DO_FUN(	do_qui		);
DECLARE_DO_FUN(	do_quit		);
DECLARE_DO_FUN(	do_quit2menu	);
DECLARE_DO_FUN( do_quote	);
DECLARE_DO_FUN( do_read		);
DECLARE_DO_FUN(	do_reboo	);
DECLARE_DO_FUN(	do_reboot	);
DECLARE_DO_FUN(	do_recho	);
DECLARE_DO_FUN(	do_recite	);
DECLARE_DO_FUN(	do_remove	);
DECLARE_DO_FUN( do_offer 	);
DECLARE_DO_FUN(	do_rename	);
DECLARE_DO_FUN(	do_rent		);
DECLARE_DO_FUN( do_replay	);
DECLARE_DO_FUN(	do_reply	);
/*DECLARE_DO_FUN(	do_report	);*/
DECLARE_DO_FUN(	do_rescue	);
DECLARE_DO_FUN(	do_parry	);
DECLARE_DO_FUN(	do_dodge	);
DECLARE_DO_FUN(	do_rest		);
DECLARE_DO_FUN(	do_restore	);
DECLARE_DO_FUN(	do_return	);
DECLARE_DO_FUN(	do_rset		);
DECLARE_DO_FUN(	do_rstat	);
DECLARE_DO_FUN( do_rules	);
DECLARE_DO_FUN(	do_save		);
DECLARE_DO_FUN(	do_say		);
DECLARE_DO_FUN(	do_sayto	);
DECLARE_DO_FUN(	do_ask		);
DECLARE_DO_FUN(	do_speak	);
DECLARE_DO_FUN(	do_languages	);
DECLARE_DO_FUN(	do_scan		);
DECLARE_DO_FUN(	do_score	);
DECLARE_DO_FUN(	do_title	);
DECLARE_DO_FUN(	do_sell		);
DECLARE_DO_FUN(	do_repair	);
DECLARE_DO_FUN(	do_estimate	);
DECLARE_DO_FUN( do_set		);
DECLARE_DO_FUN(	do_shout	);
DECLARE_DO_FUN( do_show		);
DECLARE_DO_FUN(	do_shutdow	);
DECLARE_DO_FUN(	do_shutdown	);
DECLARE_DO_FUN( do_sit		);
DECLARE_DO_FUN( do_addstat	);
DECLARE_DO_FUN( do_skills	);
DECLARE_DO_FUN(	do_sla		);
DECLARE_DO_FUN(	do_slay		);
DECLARE_DO_FUN(	do_otype	);
DECLARE_DO_FUN(	do_owear	);
DECLARE_DO_FUN(	do_sleep	);
DECLARE_DO_FUN(	do_slookup	);
DECLARE_DO_FUN( do_smote	);
DECLARE_DO_FUN(	do_sneak	);
DECLARE_DO_FUN(	do_unsneak	);
DECLARE_DO_FUN(	do_snoop	);
DECLARE_DO_FUN( do_socials	);
DECLARE_DO_FUN(	do_south	);
DECLARE_DO_FUN( do_sockets	);
DECLARE_DO_FUN( do_spells	);
DECLARE_DO_FUN(	do_split	);
DECLARE_DO_FUN(	do_sset		);
DECLARE_DO_FUN(	do_langset	);
DECLARE_DO_FUN(	do_stand	);
DECLARE_DO_FUN( do_stat		);
DECLARE_DO_FUN(	do_steal	);
DECLARE_DO_FUN( do_story	);
DECLARE_DO_FUN( do_string	);
DECLARE_DO_FUN(	do_surrender	);
DECLARE_DO_FUN(	do_switch	);
DECLARE_DO_FUN(	do_tell		);
DECLARE_DO_FUN(	do_time		);
DECLARE_DO_FUN(	do_title	);
/*DECLARE_DO_FUN(	do_train	);*/
DECLARE_DO_FUN(	do_transfer	);
DECLARE_DO_FUN( do_trip		);
DECLARE_DO_FUN(	do_trust	);
DECLARE_DO_FUN(	do_typ		);
DECLARE_DO_FUN(	do_typo		);
DECLARE_DO_FUN( do_unalias	);
DECLARE_DO_FUN(	do_unlock	);
DECLARE_DO_FUN( do_unread	);
DECLARE_DO_FUN(	do_up		);
DECLARE_DO_FUN(	do_value	);
DECLARE_DO_FUN(	do_visible	);
DECLARE_DO_FUN( do_violate	);
DECLARE_DO_FUN( do_vnum		);
DECLARE_DO_FUN(	do_wake		);
DECLARE_DO_FUN(	do_wear		);
DECLARE_DO_FUN(	do_hold		);
DECLARE_DO_FUN(	do_wield	);
DECLARE_DO_FUN(	do_weather	);
DECLARE_DO_FUN(	do_west		);
DECLARE_DO_FUN(	do_where	);
DECLARE_DO_FUN(	do_who		);
DECLARE_DO_FUN(	do_evcheck	);
DECLARE_DO_FUN(	do_wizhelp	);
DECLARE_DO_FUN(	do_wizlock	);
DECLARE_DO_FUN( do_wiznet	);
DECLARE_DO_FUN(	do_yell		);
DECLARE_DO_FUN(	do_zap		);
DECLARE_DO_FUN( do_zecho	);
DECLARE_DO_FUN( do_olc		);
DECLARE_DO_FUN( do_asave	);
DECLARE_DO_FUN( do_alist	);
DECLARE_DO_FUN( do_resets	);
DECLARE_DO_FUN( do_redit	);
DECLARE_DO_FUN( do_aedit	);
DECLARE_DO_FUN( do_medit	);
DECLARE_DO_FUN( do_oedit	);
DECLARE_DO_FUN( do_mpedit	);
DECLARE_DO_FUN( do_opedit	);
DECLARE_DO_FUN( do_rpedit	);
DECLARE_DO_FUN( do_tedit	);
DECLARE_DO_FUN( do_rdedit	);
DECLARE_DO_FUN( do_hedit	);
DECLARE_DO_FUN( do_mount	);
DECLARE_DO_FUN( do_dismount	);
DECLARE_DO_FUN( do_prewait	);
DECLARE_DO_FUN(	do_track	);
DECLARE_DO_FUN(	do_turn		);
DECLARE_DO_FUN(	do_test		);
DECLARE_DO_FUN(	do_rmap		);
DECLARE_DO_FUN(	do_names	);
DECLARE_DO_FUN(	do_secret	);
DECLARE_DO_FUN(	do_use		);
DECLARE_DO_FUN(	do_rlookup			);
DECLARE_DO_FUN( do_wizset			);
DECLARE_DO_FUN( do_wizkomendy		);
DECLARE_DO_FUN( do_wizstat			);
DECLARE_DO_FUN( do_trophy			);
DECLARE_DO_FUN(	do_lore				);
DECLARE_DO_FUN(	do_deposit			);
DECLARE_DO_FUN(	do_withdraw			);
DECLARE_DO_FUN(	do_levels			);
DECLARE_DO_FUN( do_bandage			);
DECLARE_DO_FUN( do_trap				);
DECLARE_DO_FUN( do_target			);
DECLARE_DO_FUN( do_display			);
DECLARE_DO_FUN( do_moblookup		);
DECLARE_DO_FUN( do_objlookup		);
DECLARE_DO_FUN( do_roomlookup 		);
DECLARE_DO_FUN( do_might			);
DECLARE_DO_FUN( do_power			);
DECLARE_DO_FUN( do_critical			);
DECLARE_DO_FUN( do_cstat			);
DECLARE_DO_FUN( do_setclan			);
DECLARE_DO_FUN( do_claninfo			);
DECLARE_DO_FUN( do_leav				);
DECLARE_DO_FUN( do_leave			);
DECLARE_DO_FUN( do_clans			);
DECLARE_DO_FUN( do_initiate			);
DECLARE_DO_FUN( do_demote			);
DECLARE_DO_FUN( do_promote			);
DECLARE_DO_FUN( do_exil				);
DECLARE_DO_FUN( do_exile			);
DECLARE_DO_FUN( do_clanstate		);
DECLARE_DO_FUN(	do_healing			);
DECLARE_DO_FUN(	do_go   			);
DECLARE_DO_FUN( do_aset 			);
DECLARE_DO_FUN( do_astat		 	);
DECLARE_DO_FUN( do_amod 			);
DECLARE_DO_FUN( do_try  			);
DECLARE_DO_FUN( do_mudinfo 			);
DECLARE_DO_FUN( do_deletemesg 		);
DECLARE_DO_FUN( do_ignore 			);
DECLARE_DO_FUN( do_timeset 			);
DECLARE_DO_FUN( do_wizconfig	 	);
DECLARE_DO_FUN( do_condition 		);
DECLARE_DO_FUN( do_hosts 			);
DECLARE_DO_FUN( do_mlist 			);
DECLARE_DO_FUN( do_rlist 			);
DECLARE_DO_FUN( do_olist 			);
DECLARE_DO_FUN( do_tlist 			);
DECLARE_DO_FUN( do_mplist 			);
DECLARE_DO_FUN( do_rplist 			);
DECLARE_DO_FUN( do_oplist 			);
DECLARE_DO_FUN( do_match 			);
DECLARE_DO_FUN( do_bsedit 			);
DECLARE_DO_FUN( do_hbedit 			);
DECLARE_DO_FUN( do_bslist 			);
DECLARE_DO_FUN( do_sedit 			);
DECLARE_DO_FUN(	do_feed 			);
DECLARE_DO_FUN(	do_water 			);
DECLARE_DO_FUN(	do_listen 			);
DECLARE_DO_FUN(	do_remlisten 		);
DECLARE_DO_FUN(	do_grouplist	 	);
DECLARE_DO_FUN(	do_horn 			);
DECLARE_DO_FUN(	do_persuasion 		);
DECLARE_DO_FUN(	do_play 			);
DECLARE_DO_FUN(	do_tune 			);
DECLARE_DO_FUN( do_regions			);
DECLARE_DO_FUN( do_destro			);
DECLARE_DO_FUN( do_destroy			);
DECLARE_DO_FUN( do_feat				);
DECLARE_DO_FUN( do_shapeshift_list	);
DECLARE_DO_FUN( do_reform 			);
DECLARE_DO_FUN( do_torment 			);
DECLARE_DO_FUN( do_overwhelming 	);
DECLARE_DO_FUN( do_cleave			);
DECLARE_DO_FUN( do_demon_aura		);
DECLARE_DO_FUN( do_vertical_slash	);
DECLARE_DO_FUN( do_damn				);
DECLARE_DO_FUN( do_call_avatar		);
DECLARE_DO_FUN( do_hustle			);
DECLARE_DO_FUN( do_control_undead	);
DECLARE_DO_FUN( do_first_aid		);
DECLARE_DO_FUN( do_healing_touch    );
DECLARE_DO_FUN( abort_healing_touch );
DECLARE_DO_FUN( do_bounty           );
DECLARE_DO_FUN( do_reward           );
DECLARE_DO_FUN( do_double_grip      );
DECLARE_DO_FUN( do_pipe             );
DECLARE_DO_FUN( do_moon_phase       );
DECLARE_DO_FUN( do_throw            );
DECLARE_DO_FUN( do_knock            );
DECLARE_DO_FUN( do_blows            );
DECLARE_DO_FUN( do_uncloak          );
DECLARE_DO_FUN( do_pre_holy_prayer  );
DECLARE_DO_FUN( do_holy_prayer      );
DECLARE_DO_FUN( do_sanct			);
DECLARE_DO_FUN( do_tricks			);
DECLARE_DO_FUN( do_introduce		);
DECLARE_DO_FUN( do_ltstat			);
DECLARE_DO_FUN( do_ltset			);
DECLARE_DO_FUN( do_timeshift 		);
DECLARE_DO_FUN( do_randtest 		);
DECLARE_DO_FUN( do_selftest ); // odpalanie sanity-checków na ¿ywym mudzie
DECLARE_DO_FUN( do_nosi 			);
DECLARE_DO_FUN( do_sifollow 		);
DECLARE_DO_FUN( do_illuminate		);  //rellik: stawianie pochodni
DECLARE_DO_FUN( do_map				);    //rellik: mapka
DECLARE_DO_FUN( do_friend				); /* rellik: FRIEND_WHO, declare main command */
DECLARE_DO_FUN( do_reverse		); //rellik: odwraca kolejnosc takich samych rzeczy w inventory
DECLARE_DO_FUN( do_debug		); //rellik: do debugowania, w³±cza i wy³±cza tryb debugowania
DECLARE_DO_FUN( do_questlog ); // Tener: sprawdzanie aktualnych zapisków nt. questów
DECLARE_DO_FUN( do_newbie );    //rellik: newbie channel
DECLARE_DO_FUN( do_sitable ); //rellik: komponenty, pokazuje tabelê powi±zuj±c± komponenty z czarami
DECLARE_DO_FUN( do_silist ); //rellik: komponenty, pokazuje obiekty które s± komponentami
DECLARE_DO_FUN( do_mine ); //rellik: mining, zaczyna wydobycie
DECLARE_DO_FUN( do_rresources ); //rellik: mining, ustawianie surowców dla zakresów vnum
DECLARE_DO_FUN( do_herb ); //agron : zió³ka
DECLARE_DO_FUN( do_fire_breath ); //Drake: Zianie ogniem
DECLARE_DO_FUN( do_frost_breath ); //j/w tyle ¿e lodem
DECLARE_DO_FUN( do_acid_breath ); //j/w tyle ¿e kwasem
DECLARE_DO_FUN( do_lightning_breath ); //j/w tyle ¿e piorunami
DECLARE_DO_FUN( do_fvlist           ); // FREE VNUM LISTING by The Mage (c) 1998
DECLARE_DO_FUN( do_identify ); // identyfikowanie przedmiotow u moba
