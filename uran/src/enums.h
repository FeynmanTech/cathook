/*
 * enums.h
 *
 *  Created on: Oct 7, 2016
 *      Author: nullifiedcat
 */

#ifndef ENUMS_H_
#define ENUMS_H_

enum cond {
	zoomed = (1 << 1),
	cloaked = (1 << 4),
	uber = (1 << 5),
	taunting = (1 << 7),
	uber_expiration = (1 << 8),
	kritzkrieg = (1 << 11),
	dead_ringer = (1 << 13),
	bonk = (1 << 14),
	stunned = (1 << 15),
	buff_banner = (1 << 16),
	mini_crit = (1 << 19),
	on_fire = (1 << 22),
	jarate = (1 << 24),
	backup = (1 << 26),
	milk = (1 << 27),
	quickfix_uber = (1 << 28),
	concheror = (1 << 29),
	marked = (1 << 30)
};

enum cond_ex {
	halloween_crit = (1 << 1),
	canteen_crit = (1 << 2),
	hype = (1 << 4),
	first_blood_crit = (1 << 5),
	winning_crit = (1 << 6),
	intelligence_crit = (1 << 7),
	on_kill_crit = (1 << 8),
	phlog_crit = (1 << 12),
	phlog_uber = (1 << 13),
	gru_marked = (1 << 16),
	hidden_uber = (1 << 19),
	canteen_uber = (1 << 20),
	misc_crit = (1 << 24),
	misc_uber = (1 << 25),
	vacc_bullet = (1 << 26),
	vacc_blast = (1 << 27),
	vacc_fire = (1 << 28),
	vacc_pbullet = (1 << 29),
	vacc_pblast = (1 << 30),
	vacc_pfire = (1 << 31)
};

enum cond_ex2 {
	cloak_spell = (1 << 0),
	cloak_spell_fading = (1 << 2),
	immunity_bullet = (1 << 3),
	immunity_blast = (1 << 4),
	immunity_fire = (1 << 5),
	buddha = (1 << 6),
	minify_spell = (1 << 8),
	healing_spell = (1 << 9),
	minify_spell_size = (1 << 11),
	base_jumping = (1 << 16),
	rocket_jumping = (1 << 17),
	powerup_generic = (1 << 25),
	powerup_strength = (1 << 26),
	powerup_haste = (1 << 27),
	powerup_regen = (1 << 28),
	powerup_resistance = (1 << 29),
	powerup_vampire = (1 << 30),
	powerup_reflect = (1 << 31)
};

enum cond_ex3 {
	powerup_precision = (1 << 0),
	powerup_agility = (1 << 1),
	grappling = (1 << 2),
	powerup_knockout = (1 << 7),
	powerup_revenge = (1 << 8),
	powerup_crit = (1 << 9),
	powerup_king = (1 << 13),
	powerup_plague = (1 << 14),
	powerup_supernova = (1 << 15),
	airblasted = (1 << 19)
};

enum powerup_type {
	not_powerup = -1,
	strength,
	resistance,
	vampire,
	reflect,
	haste,
	regeneration,
	precision,
	agility,
	knockout,
	king,
	plague,
	supernova,
	crits,
	POWERUP_COUNT
};

enum powerup_owner {
	neutral = 0,
	red = 1,
	blue = 2
};

enum item_type {
	item_null = -1,
	item_medkit_small,
	item_medkit_medium,
	item_medkit_large,
	item_ammo_small,
	item_ammo_medium,
	item_ammo_large,
	item_mp_strength,
	item_mp_resistance,
	item_mp_vampire,
	item_mp_reflect,
	item_mp_haste,
	item_mp_regeneration,
	item_mp_precision,
	item_mp_agility,
	item_mp_knockout,
	item_mp_king,
	item_mp_plague,
	item_mp_supernova,
	item_mp_crit,
	item_mp_uber, /* this exists for some reason */
	item_mp_warlock, /* never seen that powerup, but the model exists */
	item_mp_thorns /* and this one */
};

enum pack_type {
	not_pack = -1,
	small,
	medium,
	large,
	PACK_COUNT
};

enum entities {
	E_PLAYER = 241
};

enum tf_class {
	tf_scout = 1,
	tf_sniper,
	tf_soldier,
	tf_demoman,
	tf_medic,
	tf_heavy,
	tf_pyro,
	tf_spy,
	tf_engineer
};

enum hitbox {
	hb_head = 0
};

enum weapons {
	WP_SNIPERRIFLE = 295,
	WP_ROCKET_LAUNCHER = 282,
	WP_DIRECT_HIT = 284,
	WP_GRENADE_LAUNCHER = 211,
	WP_HUNTSMAN = 197
};

#endif /* ENUMS_H_ */
