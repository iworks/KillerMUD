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
 * Grunai                (grunai.mud@gmail.com          ) [Grunai    ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Pietrzak Marcin       (marcin@iworks.pl              ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: magic.h 12233 2013-04-09 09:54:32Z illi $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/magic.h $
 *
 * Spell functions.
 * Defined in magic.c.
 *
 */
DECLARE_SPELL_FUN(	spell_null		);
DECLARE_SPELL_FUN(	spell_acid_blast	);
DECLARE_SPELL_FUN(	spell_armor		);
DECLARE_SPELL_FUN(	spell_bless		);
DECLARE_SPELL_FUN(	spell_blindness		);
DECLARE_SPELL_FUN(	spell_burning_hands	);
DECLARE_SPELL_FUN(	spell_calm		);
DECLARE_SPELL_FUN(	spell_cancellation	);
DECLARE_SPELL_FUN(	spell_cause_critical	);
DECLARE_SPELL_FUN(	spell_cause_light	);
DECLARE_SPELL_FUN(	spell_cause_serious	);
DECLARE_SPELL_FUN(	spell_cause_moderate);
DECLARE_SPELL_FUN(	spell_change_sex	);
DECLARE_SPELL_FUN(	spell_chain_lightning   );
DECLARE_SPELL_FUN(	spell_charm_person	);
DECLARE_SPELL_FUN(	spell_chill_touch	);
DECLARE_SPELL_FUN(	spell_colour_spray	);
DECLARE_SPELL_FUN(	spell_continual_light	);
DECLARE_SPELL_FUN(	spell_control_weather	);
DECLARE_SPELL_FUN(	spell_create_food	);
DECLARE_SPELL_FUN(	spell_create_spring	);
DECLARE_SPELL_FUN(	spell_create_water	);
DECLARE_SPELL_FUN(	spell_create_healing_water	);
DECLARE_SPELL_FUN(	spell_cure_blindness	);
DECLARE_SPELL_FUN(	spell_cure_critical	);
DECLARE_SPELL_FUN(	spell_cure_disease	);
DECLARE_SPELL_FUN(	spell_cure_light	);
DECLARE_SPELL_FUN(	spell_cure_poison	);
DECLARE_SPELL_FUN(	spell_cure_serious	);
DECLARE_SPELL_FUN(	spell_cure_moderate	);
DECLARE_SPELL_FUN(	spell_curse			);
DECLARE_SPELL_FUN(	spell_detect_evil	);
DECLARE_SPELL_FUN(	spell_detect_good	);
DECLARE_SPELL_FUN(	spell_detect_hidden	);
DECLARE_SPELL_FUN(	spell_detect_invis	);
DECLARE_SPELL_FUN(	spell_detect_magic	);
DECLARE_SPELL_FUN(	spell_detect_poison	);
DECLARE_SPELL_FUN(	spell_dispel_evil	);
DECLARE_SPELL_FUN(	spell_dispel_good	);
DECLARE_SPELL_FUN(	spell_dispel_magic	);
DECLARE_SPELL_FUN(	spell_earthquake	);
DECLARE_SPELL_FUN(	spell_enchant_armor	);
DECLARE_SPELL_FUN(	spell_enchant_weapon	);
DECLARE_SPELL_FUN(	spell_energy_drain	);
DECLARE_SPELL_FUN(	spell_faerie_fire	);
DECLARE_SPELL_FUN(	spell_faerie_fog	);
DECLARE_SPELL_FUN(	spell_farsight		);
DECLARE_SPELL_FUN(	spell_fireball		);
DECLARE_SPELL_FUN(	spell_fireproof		);
DECLARE_SPELL_FUN(	spell_flamestrike	);
DECLARE_SPELL_FUN(	spell_floating_disc	);
DECLARE_SPELL_FUN(	spell_spiritual_armor			);
DECLARE_SPELL_FUN(	spell_fly						);
DECLARE_SPELL_FUN(	spell_gate						);
DECLARE_SPELL_FUN(	spell_giant_strength			);
DECLARE_SPELL_FUN(	spell_strength					);
DECLARE_SPELL_FUN(	spell_harm						);
DECLARE_SPELL_FUN(	spell_haste						);
DECLARE_SPELL_FUN(	spell_heal						);
DECLARE_SPELL_FUN(	spell_heat_metal				);
DECLARE_SPELL_FUN(	spell_identify					);
DECLARE_SPELL_FUN(	spell_infravision				);
DECLARE_SPELL_FUN(	spell_invis						);
DECLARE_SPELL_FUN(	spell_know_alignment			);
DECLARE_SPELL_FUN(	spell_lightning_bolt			);
DECLARE_SPELL_FUN(	spell_force_bolt				);
DECLARE_SPELL_FUN(	spell_locate_object				);
DECLARE_SPELL_FUN(	spell_magic_missile				);
DECLARE_SPELL_FUN(	spell_mass_healing				);
DECLARE_SPELL_FUN(	spell_mass_invis				);
DECLARE_SPELL_FUN(	spell_nexus						);
DECLARE_SPELL_FUN(	spell_pass_door					);
DECLARE_SPELL_FUN(	spell_plague					);
DECLARE_SPELL_FUN(	spell_poison					);
DECLARE_SPELL_FUN(	spell_portal					);
DECLARE_SPELL_FUN(	spell_protection_evil			);
DECLARE_SPELL_FUN(	spell_protection_good			);
DECLARE_SPELL_FUN(	spell_ray_of_truth				);
DECLARE_SPELL_FUN(	spell_recharge					);
DECLARE_SPELL_FUN(	spell_refresh					);
DECLARE_SPELL_FUN(	spell_mass_refresh					);
DECLARE_SPELL_FUN(	spell_remove_curse				);
DECLARE_SPELL_FUN(	spell_sanctuary					);
DECLARE_SPELL_FUN(	spell_shocking_grasp			);
DECLARE_SPELL_FUN(	spell_spray_of_thorns			);
DECLARE_SPELL_FUN(	spell_shield					);
DECLARE_SPELL_FUN(	spell_sleep						);
DECLARE_SPELL_FUN(	spell_slow						);
DECLARE_SPELL_FUN(	spell_stone_skin				);
DECLARE_SPELL_FUN(	spell_summon					);
DECLARE_SPELL_FUN(	spell_teleport					);
DECLARE_SPELL_FUN(	spell_dimension_door			);
DECLARE_SPELL_FUN(	spell_weaken					);
DECLARE_SPELL_FUN(	spell_word_of_recall			);
DECLARE_SPELL_FUN(	spell_finger_of_death				);
DECLARE_SPELL_FUN(	spell_elemental_devastation		);
DECLARE_SPELL_FUN(	spell_reverse_gravity				);
DECLARE_SPELL_FUN(	spell_gas_breath				);
DECLARE_SPELL_FUN(	spell_lightning_breath			);
DECLARE_SPELL_FUN(	spell_summon_animals			);
DECLARE_SPELL_FUN(	spell_spirit_hammer				);
DECLARE_SPELL_FUN(	spell_hold_person				);
DECLARE_SPELL_FUN(	spell_hold_animal				);
DECLARE_SPELL_FUN(	spell_hold_monster				);
DECLARE_SPELL_FUN(	spell_hold_plant				);
DECLARE_SPELL_FUN(	spell_purify_food				);
DECLARE_SPELL_FUN(	spell_light						);
DECLARE_SPELL_FUN(	spell_transmute_staff			);
DECLARE_SPELL_FUN(	spell_command					);
DECLARE_SPELL_FUN(	spell_confusion					);
DECLARE_SPELL_FUN(	spell_regenerate				);
DECLARE_SPELL_FUN(	spell_undead_invis				);
DECLARE_SPELL_FUN(	spell_animal_invis				);
DECLARE_SPELL_FUN(	spell_resist_fire				);
DECLARE_SPELL_FUN(	spell_resist_cold				);
DECLARE_SPELL_FUN(	spell_resist_lightning			);
DECLARE_SPELL_FUN(	spell_resist_acid				);
DECLARE_SPELL_FUN(	spell_resist_magic				);
DECLARE_SPELL_FUN(	spell_brave_cloak				);
DECLARE_SPELL_FUN(	spell_remove_fear				);
DECLARE_SPELL_FUN(	spell_bark_skin					);
DECLARE_SPELL_FUN(	spell_silence					);
DECLARE_SPELL_FUN(	spell_aid						);
DECLARE_SPELL_FUN(	spell_divine_favor				);
DECLARE_SPELL_FUN(	spell_chant						);
DECLARE_SPELL_FUN(	spell_entangle					);
DECLARE_SPELL_FUN(	spell_web						);
DECLARE_SPELL_FUN(	spell_waterwalk					);
DECLARE_SPELL_FUN(	spell_goodbarry					);
DECLARE_SPELL_FUN(	spell_water_breathing			);
DECLARE_SPELL_FUN(	spell_flame_blade				);
DECLARE_SPELL_FUN(	spell_energy_shield				);
DECLARE_SPELL_FUN(	spell_pyrotechnics				);
DECLARE_SPELL_FUN(	spell_prayer					);
DECLARE_SPELL_FUN(	spell_blur						);
DECLARE_SPELL_FUN(	spell_mirror_image				);
DECLARE_SPELL_FUN(	spell_free_action				);
DECLARE_SPELL_FUN(	spell_remove_paralysis			);
DECLARE_SPELL_FUN(	spell_deafness					);
DECLARE_SPELL_FUN(	spell_mending					);
DECLARE_SPELL_FUN(	spell_hold_undead				);
DECLARE_SPELL_FUN(	spell_fear						);
DECLARE_SPELL_FUN(	spell_darkness					);
DECLARE_SPELL_FUN(	spell_resist_normal_weapon		);
DECLARE_SPELL_FUN(	spell_resist_magic_weapon		);
DECLARE_SPELL_FUN(	spell_resist_elements			);
DECLARE_SPELL_FUN(	spell_fireshield				);
DECLARE_SPELL_FUN(	spell_iceshield					);
DECLARE_SPELL_FUN(	spell_reflect_spell_I			);
DECLARE_SPELL_FUN(	spell_reflect_spell_II			);
DECLARE_SPELL_FUN(	spell_reflect_spell_III			);
DECLARE_SPELL_FUN(	spell_cone_of_cold				);
DECLARE_SPELL_FUN(	spell_lore						);
DECLARE_SPELL_FUN(	spell_lore_undead				);
DECLARE_SPELL_FUN(	spell_bladethirst				);
DECLARE_SPELL_FUN(	spell_energize					);
DECLARE_SPELL_FUN(	spell_healing_sleep				);
DECLARE_SPELL_FUN(	spell_cold_snap					);
DECLARE_SPELL_FUN(	spell_frost_hands				);
DECLARE_SPELL_FUN(	spell_fire_darts				);
DECLARE_SPELL_FUN(	spell_acid_hands				);
DECLARE_SPELL_FUN(	spell_ethereal_armor			);
DECLARE_SPELL_FUN(	spell_domination				);
DECLARE_SPELL_FUN(	spell_increase_wounds			);
DECLARE_SPELL_FUN(	spell_ray_of_enfeeblement		);
DECLARE_SPELL_FUN(	spell_flame_arrow				);
DECLARE_SPELL_FUN(	spell_minor_globe				);
DECLARE_SPELL_FUN(	spell_globe						);
DECLARE_SPELL_FUN(	spell_flare						);
DECLARE_SPELL_FUN(	spell_charm_monster				);
DECLARE_SPELL_FUN(	spell_comprehend_languages		);
DECLARE_SPELL_FUN(	spell_power_word_blindness		);
DECLARE_SPELL_FUN(	spell_power_word_kill			);
DECLARE_SPELL_FUN(	spell_power_word_stun			);
DECLARE_SPELL_FUN(	spell_slow_poison				);
DECLARE_SPELL_FUN(	spell_lesser_restoration		);
DECLARE_SPELL_FUN(	spell_restoration				);
DECLARE_SPELL_FUN(	spell_vampiric_touch			);
DECLARE_SPELL_FUN(	spell_feeblemind				);
DECLARE_SPELL_FUN(	spell_divine_power				);
DECLARE_SPELL_FUN(	spell_animate_dead				);
DECLARE_SPELL_FUN(	spell_produce_fire				);
DECLARE_SPELL_FUN(	spell_bane						);
DECLARE_SPELL_FUN(	spell_darkvision				);
DECLARE_SPELL_FUN(	spell_call_lightning			);
DECLARE_SPELL_FUN(	spell_storm_shell				);
DECLARE_SPELL_FUN(	spell_chill_metal				);
DECLARE_SPELL_FUN(	spell_charm_animal				);
DECLARE_SPELL_FUN(	spell_blade_barrier				);
DECLARE_SPELL_FUN(	spell_create_lesser_undead		);
DECLARE_SPELL_FUN(	spell_dismiss_animal			);
DECLARE_SPELL_FUN(	spell_dismiss_plant			);
DECLARE_SPELL_FUN(	spell_maze 					    );
DECLARE_SPELL_FUN(	spell_float						);
DECLARE_SPELL_FUN(	spell_dismiss_insect			);
DECLARE_SPELL_FUN(	spell_dismiss_person			);
DECLARE_SPELL_FUN(	spell_dismiss_monster			);
DECLARE_SPELL_FUN(	spell_dismiss_undead			);
DECLARE_SPELL_FUN(	spell_psionic_blast				);
DECLARE_SPELL_FUN(	spell_lesser_psionic_blast		);
DECLARE_SPELL_FUN(	spell_chaotic_shock				);
DECLARE_SPELL_FUN(	spell_mind_strike		);
DECLARE_SPELL_FUN(	spell_mind_blast		);
DECLARE_SPELL_FUN(	spell_bull_strength					);
DECLARE_SPELL_FUN(	spell_cat_grace					);
DECLARE_SPELL_FUN(	spell_owl_wisdom					);
DECLARE_SPELL_FUN(	spell_fox_cunning					);
DECLARE_SPELL_FUN(	spell_bear_endurance				);
DECLARE_SPELL_FUN(	spell_eagle_splendor				);

/* mag, specjalista nekromancji */
DECLARE_SPELL_FUN(	spell_raise_ghul				);
DECLARE_SPELL_FUN(	spell_raise_ghast				);
DECLARE_SPELL_FUN(	spell_orb_of_entropy			);
DECLARE_SPELL_FUN(	spell_spirit_armor				);
DECLARE_SPELL_FUN(	spell_ghoul_touch				);
DECLARE_SPELL_FUN(	spell_decay						);
DECLARE_SPELL_FUN(	spell_horrid_wilting			);
DECLARE_SPELL_FUN(	spell_unholy_fury				);
DECLARE_SPELL_FUN(	spell_embalm					);
DECLARE_SPELL_FUN(	spell_soul_trap					);
DECLARE_SPELL_FUN(	spell_floating_skull			);
DECLARE_SPELL_FUN(	spell_banshees_howl				);
DECLARE_SPELL_FUN(	spell_draining_hands			);
DECLARE_SPELL_FUN(	spell_revive					);
DECLARE_SPELL_FUN(	spell_resurrection                              );
DECLARE_SPELL_FUN(	spell_bonelace					);

/* mag, Przemiany */
DECLARE_SPELL_FUN(	spell_might						);
DECLARE_SPELL_FUN(	spell_misfortune				);
DECLARE_SPELL_FUN(	spell_lower_resistance			);
DECLARE_SPELL_FUN(	spell_major_haste				);
DECLARE_SPELL_FUN(	spell_transmute_liquid			);
DECLARE_SPELL_FUN(	spell_mass_fly					);
DECLARE_SPELL_FUN(	spell_razorblade_hands			);
DECLARE_SPELL_FUN(	spell_steel_scarfskin           );
DECLARE_SPELL_FUN(	spell_changestaff				);
DECLARE_SPELL_FUN(	spell_animate_staff				);
DECLARE_SPELL_FUN(	spell_slippery_floor			);
DECLARE_SPELL_FUN(	spell_wall_of_mist				);
DECLARE_SPELL_FUN(	spell_hardiness					);
DECLARE_SPELL_FUN(	spell_share_fitness				);
DECLARE_SPELL_FUN(	spell_group_unity				);
DECLARE_SPELL_FUN(	spell_nimbleness					);
DECLARE_SPELL_FUN(	spell_draconic_wisdom				);
DECLARE_SPELL_FUN(	spell_insight					);
DECLARE_SPELL_FUN(	spell_behemot_toughness			);
DECLARE_SPELL_FUN(	spell_giant_strength			);
DECLARE_SPELL_FUN( spell_mend_golem );

/* mag, Inwokacje */
DECLARE_SPELL_FUN(	spell_force_missiles			);
DECLARE_SPELL_FUN(	spell_thunder_bolt				);
DECLARE_SPELL_FUN(	spell_energy_strike				);
DECLARE_SPELL_FUN(	spell_acid_arrow				);
DECLARE_SPELL_FUN(	spell_flame_lace				);
DECLARE_SPELL_FUN(	spell_rain_of_blades				);
DECLARE_SPELL_FUN(	spell_light_nova				);

/* mag, Odrzucanie */
DECLARE_SPELL_FUN(	spell_protection_from_summon	);
DECLARE_SPELL_FUN(	spell_lesser_magic_resist		);
DECLARE_SPELL_FUN(	spell_force_field				);
DECLARE_SPELL_FUN(	spell_resist_weapon				);
DECLARE_SPELL_FUN(	spell_mantle					);
DECLARE_SPELL_FUN(	spell_major_globe				);
DECLARE_SPELL_FUN(	spell_repayment					);
DECLARE_SPELL_FUN(	spell_antimagic_manacles		);
DECLARE_SPELL_FUN(	spell_great_dispel				);
DECLARE_SPELL_FUN(	spell_deflect_wounds			);
DECLARE_SPELL_FUN(	spell_resist_poison				);
DECLARE_SPELL_FUN(  spell_summon_distortion			);
DECLARE_SPELL_FUN(  spell_stability					);
DECLARE_SPELL_FUN(  spell_exile						);
DECLARE_SPELL_FUN(  spell_defense_curl				);
DECLARE_SPELL_FUN(  spell_perfect_senses			);
DECLARE_SPELL_FUN(  spell_mind_fortess				);
DECLARE_SPELL_FUN(  spell_fortitude					);
DECLARE_SPELL_FUN(  spell_brainwash					);
DECLARE_SPELL_FUN(  spell_scrying_shield					);

/* mag, Poznanie */
DECLARE_SPELL_FUN(	spell_eyes_of_the_torturer		);
DECLARE_SPELL_FUN(	spell_eye_of_vision				);
DECLARE_SPELL_FUN(	spell_detect_aggressive			);
DECLARE_SPELL_FUN(	spell_astral_journey			);
DECLARE_SPELL_FUN(	spell_wizard_eye				);
DECLARE_SPELL_FUN(	spell_piercing_sight			);
DECLARE_SPELL_FUN(	spell_detect_undead				);
DECLARE_SPELL_FUN(	spell_alarm						);
DECLARE_SPELL_FUN(	spell_cautious_sleep			);
DECLARE_SPELL_FUN(	spell_sense_presence			);

/* mag, Przywo³ania */
DECLARE_SPELL_FUN(	spell_summon_lesser_meteor		);
DECLARE_SPELL_FUN(	spell_summon_greater_meteor		);
DECLARE_SPELL_FUN(	spell_summon_insects			);
DECLARE_SPELL_FUN(	spell_summon_greenskins			);
DECLARE_SPELL_FUN(	spell_summon_flying_creatures	);
DECLARE_SPELL_FUN(	spell_summon_strong_creatures	);
DECLARE_SPELL_FUN(	spell_summon_ancient_creatures	);

/* mag, Iluzje */
DECLARE_SPELL_FUN(	spell_create_lesser_illusion	);
DECLARE_SPELL_FUN(	spell_create_greater_illusion	);
DECLARE_SPELL_FUN(	spell_hallucinations			);
DECLARE_SPELL_FUN(	spell_perfect_self				);
DECLARE_SPELL_FUN(	spell_confusion_shell			);
DECLARE_SPELL_FUN(	spell_loop						);
DECLARE_SPELL_FUN(	spell_blink						);
DECLARE_SPELL_FUN(	spell_nondetection				);
DECLARE_SPELL_FUN(	spell_shadow_conjuration		);
DECLARE_SPELL_FUN(	spell_fetch						);
DECLARE_SPELL_FUN(	spell_inspire					);
DECLARE_SPELL_FUN(	spell_shadow_swarm				);
DECLARE_SPELL_FUN(	spell_dazzling_flash			);
DECLARE_SPELL_FUN(	spell_psychic_scream			);

/* mag, zauroczenia.zaczarowania */
DECLARE_SPELL_FUN(	spell_daze						);
DECLARE_SPELL_FUN(	spell_noble_look				);
DECLARE_SPELL_FUN(	spell_puppet_master				);
DECLARE_SPELL_FUN(	spell_loyalty					);
DECLARE_SPELL_FUN(	spell_imbue_with_element		);
DECLARE_SPELL_FUN(	spell_confuse_languages			);
DECLARE_SPELL_FUN(	spell_magic_hands				);
DECLARE_SPELL_FUN(	spell_inspiring_presence			);

/* kleryk, paladyn, druid*/
DECLARE_SPELL_FUN(	spell_luck						);
DECLARE_SPELL_FUN(	spell_slow_rot					);

/* kleryk, druid */
DECLARE_SPELL_FUN(	spell_healing_ring				);
DECLARE_SPELL_FUN(	spell_mass_luck					);
DECLARE_SPELL_FUN(	spell_mental_barrier				);
DECLARE_SPELL_FUN(	spell_shield_of_nature				);

/* DRUID ONLY */
DECLARE_SPELL_FUN(	spell_shillelagh				);
DECLARE_SPELL_FUN(	spell_alicorn_lance				);
DECLARE_SPELL_FUN(	spell_animal_rage				);
DECLARE_SPELL_FUN(	spell_burst_of_fire				);
DECLARE_SPELL_FUN(	spell_burst_of_flame			);
DECLARE_SPELL_FUN(	spell_endure_acid				);
DECLARE_SPELL_FUN(	spell_endure_cold				);
DECLARE_SPELL_FUN(	spell_endure_fire				);
DECLARE_SPELL_FUN(	spell_endure_lightning			);
DECLARE_SPELL_FUN(	spell_freezing_rain				);
DECLARE_SPELL_FUN(	spell_immolate					);
DECLARE_SPELL_FUN(	spell_shadow_weapon				);
DECLARE_SPELL_FUN(	spell_smashing_wave				);
DECLARE_SPELL_FUN(	spell_sunscorch					);
DECLARE_SPELL_FUN(	spell_magic_fang				);
DECLARE_SPELL_FUN(	spell_frost_rift				);
DECLARE_SPELL_FUN(	spell_lava_bolt					);
DECLARE_SPELL_FUN(	spell_hellfire					);
DECLARE_SPELL_FUN(	spell_ice_bolt					);
DECLARE_SPELL_FUN(	spell_wind_shield				);
DECLARE_SPELL_FUN(	spell_beast_claws				);
DECLARE_SPELL_FUN(	spell_wind_charger				);
DECLARE_SPELL_FUN(	spell_firefly_swarm				);
DECLARE_SPELL_FUN(	spell_create_tree				);
DECLARE_SPELL_FUN(	spell_ring_of_vanion			);
DECLARE_SPELL_FUN(	spell_circle_of_vanion			);
DECLARE_SPELL_FUN(	spell_heal_animal				);
DECLARE_SPELL_FUN(	spell_corrode					);
DECLARE_SPELL_FUN(	spell_nature_curse				);
DECLARE_SPELL_FUN(	spell_wood_master				);
DECLARE_SPELL_FUN(	spell_stone_master				);
DECLARE_SPELL_FUN(	spell_singing_ruff				);
DECLARE_SPELL_FUN(	spell_fury_of_the_wild				);
DECLARE_SPELL_FUN(	spell_nature_ally_I				);
DECLARE_SPELL_FUN(	spell_nature_ally_II				);
DECLARE_SPELL_FUN(	spell_nature_ally_III				);
DECLARE_SPELL_FUN(	spell_nature_ally_IV				);
DECLARE_SPELL_FUN(	spell_cure_animal				);
DECLARE_SPELL_FUN(	spell_reinvigore_animal				);
DECLARE_SPELL_FUN(	spell_cure_plant				);
DECLARE_SPELL_FUN(	spell_reinvigore_plant				);
DECLARE_SPELL_FUN(	spell_liveoak					);
DECLARE_SPELL_FUN(	spell_bark_guardian				);
DECLARE_SPELL_FUN(	spell_wildthorn					);

/* PALADYN ONLY */
DECLARE_SPELL_FUN(	spell_hold_evil					);
DECLARE_SPELL_FUN(	spell_divine_shield				);
DECLARE_SPELL_FUN(	spell_holy_weapons				);

/* aury paladyna */
DECLARE_SPELL_FUN(	spell_aura_of_protection		);
DECLARE_SPELL_FUN(	spell_aura_of_precision			);
DECLARE_SPELL_FUN(	spell_aura_of_endurance			);
DECLARE_SPELL_FUN(	spell_aura_of_improved_healing	);
DECLARE_SPELL_FUN(	spell_aura_of_vigor				);
DECLARE_SPELL_FUN(	spell_aura_of_battle_lust		);

/* CLERIC ONLY */
DECLARE_SPELL_FUN(	spell_create_symbol				);
DECLARE_SPELL_FUN(	spell_champions_strength		);
DECLARE_SPELL_FUN(	spell_life_transfer				);
DECLARE_SPELL_FUN(	spell_mass_bless				);
DECLARE_SPELL_FUN(	spell_consecrate				);
DECLARE_SPELL_FUN(	spell_desecrate					);
DECLARE_SPELL_FUN(	spell_change_liquid				);
DECLARE_SPELL_FUN(	spell_ray_of_light				);
DECLARE_SPELL_FUN(	spell_spirit_light				);
DECLARE_SPELL_FUN(	spell_sense_life				);
DECLARE_SPELL_FUN(	spell_spiritual_weapon			);
DECLARE_SPELL_FUN(	spell_holy_bolt			);

/* BARD */
DECLARE_SPELL_FUN(	spell_dancing_lights			);
DECLARE_SPELL_FUN(	spell_alter_instrument			);
DECLARE_SPELL_FUN(	spell_enchant_instrument		);


/* DRIUD ONLY */
DECLARE_SPELL_FUN(	spell_heal_plant				);
DECLARE_SPELL_FUN(	spell_sense_fatigue				);

/* SHAMAN ONLY */
DECLARE_SPELL_FUN(	spell_spirit_of_vision	);
DECLARE_SPELL_FUN(	spell_subdue_spirits		);
DECLARE_SPELL_FUN(	spell_spirit_walk		);
DECLARE_SPELL_FUN(	spell_healing_salve		);
DECLARE_SPELL_FUN(	spell_ancestors_wisdom		);
DECLARE_SPELL_FUN(	spell_seal_of_weakness		);
DECLARE_SPELL_FUN(	spell_ancestors_protection		);
DECLARE_SPELL_FUN(	spell_ancestors_favor		);
DECLARE_SPELL_FUN(	spell_ancestors_vision		);
DECLARE_SPELL_FUN(	spell_ancestors_fury		);
DECLARE_SPELL_FUN(	spell_spirit_shackle		);
DECLARE_SPELL_FUN(	spell_dismiss_outsider		);
DECLARE_SPELL_FUN(	spell_spirit_of_life		);
DECLARE_SPELL_FUN(	spell_breath_of_life		);
DECLARE_SPELL_FUN(	spell_spiritual_guidance		);
DECLARE_SPELL_FUN(	spell_seal_of_doom		);
DECLARE_SPELL_FUN(	spell_lesser_cure_poison	    );
DECLARE_SPELL_FUN(	spell_greater_cure_poison	    );
DECLARE_SPELL_FUN(	spell_seal_of_atrocity	    );
DECLARE_SPELL_FUN(	spell_seal_of_despair	    );

///tutaj tricki wale bo nie chce nowego pliku robic
DECLARE_TRICK_FUN(	trick_null 						);
DECLARE_TRICK_FUN(	trick_vertical_kick 			);
DECLARE_TRICK_FUN(	trick_entwine 					);
DECLARE_TRICK_FUN(	trick_riposte 					);
DECLARE_TRICK_FUN(	trick_cyclone 					);
DECLARE_TRICK_FUN(	trick_flabbergast 				);
DECLARE_TRICK_FUN(	trick_staff_swirl 				);
DECLARE_TRICK_FUN(	trick_dragon_strike				);
DECLARE_TRICK_FUN(	trick_decapitation				);
DECLARE_TRICK_FUN(	trick_thundering_whack			);
DECLARE_TRICK_FUN(	trick_strucking_wallop			);
DECLARE_TRICK_FUN(	trick_shove						);
DECLARE_TRICK_FUN(	trick_ravaging_orb				);
DECLARE_TRICK_FUN(	trick_bleed						);
DECLARE_TRICK_FUN(	trick_thigh_jab					);
DECLARE_TRICK_FUN(	trick_weapon_wrench				);
DECLARE_TRICK_FUN(	trick_crushing_mace				);
DECLARE_TRICK_FUN(	trick_thousandslayer			);
DECLARE_TRICK_FUN(	trick_glorious_impale			);
DECLARE_TRICK_FUN(	trick_divine_impact				);
DECLARE_TRICK_FUN(	trick_dreadful_strike			);
DECLARE_TRICK_FUN(	trick_rampage			        );
DECLARE_TRICK_FUN(	trick_enlightenment		        );


const char *spell_translate( char *source );

//rellik: komponenty, funkcja obs³ugi komponentu przy rzucaniu czaru
bool spell_item_check( CHAR_DATA *ch, int sn, char *name );
int spell_item_get_slot_by_item_name( OBJ_DATA *obj );
int spell_item_get_slot_by_key( int key );
bool spell_item_destroy( bool timer, OBJ_DATA *komp, int slot );
//rellik: komponenty, jeszcze lamerska strukturka do przekazywania danych pomiêdzy funkcjami
struct active_spell_items_type
{
  OBJ_DATA *components[MAX_COMPONENTS]; //grupa komponentów
  int ind[MAX_COMPONENTS]; //grupa indeksów
} active_spell_items;


/*
 * Light nova spell
 */

struct light_nova_data
{
  char specialist;
  int sn;
  int level;
  char spell_level_bonus;
  CHAR_DATA * caster;
  OBJ_DATA * ball;
  ROOM_INDEX_DATA * cast_room;
  int power;
  int countdown;
  char component;
};

typedef struct light_nova_data LIGHT_NOVA_DATA;

/**
 * spell_portal & spell_nexus helper
 */
bool helper_make_portal args ( ( CHAR_DATA *ch, bool is_portal, int sn ) );

/**
 * sector_type modifi functions
 */
int damage_modifier_by_spell_type   args ( ( int dam, int spell_type, CHAR_DATA *ch ) );
int damage_modifier_by_moon_phase   args ( ( int dam ) );
int duration_modifier_by_spell_type args ( ( int duration, int spell_type, CHAR_DATA *ch ) );

/**
 * heal_specific_race_type
 */
void heal_specific_race_type args( ( int sn, int level, CHAR_DATA *ch, void *vo, int target, const char comm_bad_race[], int target_type, int dice_rolls, int dice_sides, int level_mod, const char heal_msg_table[6][MAX_STRING_LENGTH] ) );


