/*
 * Copyright 2017-2022 Morse Micro
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include <linux/string.h>

#include "reg_rules.h"
#include "debug.h"

#define AUTO_BW	NL80211_RRF_AUTO_BW

/**
 * DOC: How To Modify Regulatory (reg.c) and Channel Mapping (s1g_channels.c)
 *
 * - Both are covered here as they are dependent.
 * The available channel maps are stored in `s1g_channels.c`, in the `channel_map` array.
 * This array is built of `morse_dot11ah_ch_map` structs.
 *
 * This struct defines a region/country alpha for the map along with an array of
 * `morse_dot11ah_channels`, which are the explicit map between a 5g channel and:
 * - An s1g channel
 * - the s1g frequency
 * - the s1g bandwidth.
 *
 * In order to make use of these channels, their frequencies need to fall
 * within the allow-listed spectrum defined in a 'regulatory database'
 * entry for the desired region. These entries are found in this file.
 *
 * In order to add a new channel map you must:
 *	1. Define the channel map for your region (alpha), and add it to the
 *	   `mapped_channels` array.
 *	2. Define, in this file, a new morse_regdomain structure for your
 *	   region. Use the naming format `mors_<YOUR ALPHA>_regdom`.
 *	3. Using the `MORSE_REG_RULE` macro, define the blocks of 5G spectrum
 *	   containing your mapped 5G channels.
 *	4. Optional - Add the s1g frequency spectrum for the s1g channels.
 */

/**
 * morse_reg_set_alpha - Set the regulatory domain rules for a given country
 * @alpha: The desired ISO/IEC Alpha2 Country to apply regulatory rules in
 *
 * Finds a set of regulatory rules based on a given alpha code, looking through
 * the internally-defined domains.
 *
 * Return: A pointer to the matching regdomain, defaults to MM.
 *
 */

const struct morse_regdomain *morse_reg_set_alpha(const char *alpha)
{
	const struct morse_regdomain *regdom;

	regdom = morse_reg_alpha_lookup(alpha);
	if (!(regdom) || !alpha)
		return NULL;

	morse_dot11ah_channel_set_map(regdom->alpha2);

	return regdom;
}
EXPORT_SYMBOL(morse_reg_set_alpha);

struct ieee80211_regdomain *morse_regdom_to_ieee80211(const struct morse_regdomain *morse_domain)
{
	struct ieee80211_regdomain *new = kmalloc(sizeof(*new) + (morse_domain->n_reg_rules *
						(sizeof(struct ieee80211_reg_rule))), GFP_KERNEL);
	int i;

	if (!new)
		return NULL;

	new->n_reg_rules = morse_domain->n_reg_rules;
	memcpy(new->alpha2, morse_domain->alpha2, ARRAY_SIZE(morse_domain->alpha2));
	for (i = 0; i < morse_domain->n_reg_rules; i++) {
		memcpy(&new->reg_rules[i], &morse_domain->reg_rules[i].dot11_reg,
		       sizeof(morse_domain->reg_rules[i].dot11_reg));
	}

	return new;
}
EXPORT_SYMBOL(morse_regdom_to_ieee80211);

const struct morse_reg_rule *morse_regdom_get_rule_for_freq(const char *alpha, int frequency)
{
	const struct morse_regdomain *regdom = morse_reg_alpha_lookup(alpha);
	int i;

	for (i = 0; i < regdom->n_reg_rules; i++) {
		if (frequency >= regdom->reg_rules[i].dot11_reg.freq_range.start_freq_khz &&
			frequency <= regdom->reg_rules[i].dot11_reg.freq_range.end_freq_khz)
			return &regdom->reg_rules[i];
	}

	return NULL;
}
EXPORT_SYMBOL(morse_regdom_get_rule_for_freq);

int morse_mac_set_country_info_from_regdom(const struct morse_regdomain *morse_domain,
				struct s1g_operation_parameters *params,
				struct dot11ah_country_ie *country_ie)
{
	const struct ieee80211_freq_range *fr;
	struct country_operating_triplet *oper_triplet;
	int i;
	int start_chan = 0;
	int end_chan = 0;
	int eirp;
	int bw;
	int ret;

	u8 op_bw_mhz = MORSE_OPERATING_CH_WIDTH_DEFAULT;
	u8 pri_bw_mhz = MORSE_PRIM_CH_WIDTH_DEFAULT;
	u8 chan_centre_freq_num = MORSE_OPERATING_CHAN_DEFAULT;
	u8 pri_1mhz_chan_idx = 0;
	u8 pri_ch_op_class = 0;

	if (params) {
		op_bw_mhz = params->op_bw_mhz;
		pri_bw_mhz = params->pri_bw_mhz;
		pri_1mhz_chan_idx = params->pri_1mhz_chan_idx;
		pri_ch_op_class = params->prim_global_op_class;
		chan_centre_freq_num = params->chan_centre_freq_num;
	}

	ret = strscpy(country_ie->country, morse_domain->alpha2,
			ARRAY_SIZE(country_ie->country));

	/* alpha2 has 2 characters */
	if (ret < 2)
		dot11ah_warn("Invalid alpha2 string\n");

	country_ie->country[2] = MORSE_GLOBAL_OPERATING_CLASS_TABLE;

	oper_triplet = &country_ie->ie_triplet;

	oper_triplet->op_triplet_id = MORSE_COUNTRY_OPERATING_TRIPLET_ID;
	oper_triplet->primary_band_op_class = pri_ch_op_class;
	oper_triplet->coverage_class = 0;
	oper_triplet->start_chan = morse_dot11ah_calc_prim_s1g_chan(op_bw_mhz, pri_bw_mhz,
						    chan_centre_freq_num, pri_1mhz_chan_idx);

	oper_triplet->chan_num = 1;

	for (i = 0; i < morse_domain->n_reg_rules; i++) {
		fr = &morse_domain->reg_rules[i].dot11_reg.freq_range;
		eirp = morse_domain->reg_rules[i].dot11_reg.power_rule.max_eirp;
		bw = KHZ_TO_MHZ(morse_domain->reg_rules[i].dot11_reg.freq_range.max_bandwidth_khz);

		if (fr->start_freq_khz > MORSE_S1G_FREQ_MIN_KHZ &&
			fr->end_freq_khz < MORSE_S1G_FREQ_MAX_KHZ) {
			start_chan =  morse_dot11ah_freq_khz_bw_mhz_to_chan(fr->start_freq_khz, bw);
			end_chan = morse_dot11ah_freq_khz_bw_mhz_to_chan(fr->end_freq_khz, bw);
		}
		if (oper_triplet->start_chan >= start_chan &&
			oper_triplet->start_chan < end_chan) {
			/* TODO: SW-7983 - Advertise minimum of EIRP from BCF vs reg rule */
			oper_triplet->max_eirp_dbm = MBM_TO_DBM(eirp);
		}
	}
	return 0;
}
EXPORT_SYMBOL(morse_mac_set_country_info_from_regdom);
