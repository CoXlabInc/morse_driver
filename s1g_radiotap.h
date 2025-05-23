/*
 * Copyright 2017-2022 Morse Micro
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#ifndef _MORSE_S1G_RADIOTAP_H_
#define _MORSE_S1G_RADIOTAP_H_

#define	IEEE80211_CHAN_700MHZ	0x0002	/* 700 MHz spectrum channel */
#define	IEEE80211_CHAN_800MHZ	0x0004	/* 800 MHz spectrum channel */
#define	IEEE80211_CHAN_900MHZ	0x0008	/* 900 MHz spectrum channel */

#define IEEE80211_RADIOTAP_HALOW_FLAGS_S1G_NDP_CMAC	0x0002
#define IEEE80211_RADIOTAP_HALOW_FLAGS_S1G_NDP_CONTROL	0x0004
#define IEEE80211_RADIOTAP_HALOW_FLAGS_S1G_NDP_MANAGEMENT 0x00f0
#define IEEE80211_RADIOTAP_HALOW_FLAGS_S1G		0x0001
#define IEEE80211_RADIOTAP_HALOW_FLAGS_NDP		0x0010
#define IEEE80211_RADIOTAP_HALOW_TLV			0x001C

/* For S1G radiotap TLV */
#define DOT11_RT_TLV_S1G_TYPE			32
#define DOT11_RT_TLV_S1G_LENGTH			6

/* Known field */
#define DOT11_RT_S1G_KNOWN_PPDU_FMT		0x0001
#define DOT11_RT_S1G_KNOWN_RES_IND		0x0002
#define DOT11_RT_S1G_KNOWN_GI			0x0004
#define DOT11_RT_S1G_KNOWN_NSS			0x0008
#define DOT11_RT_S1G_KNOWN_BW			0x0010
#define DOT11_RT_S1G_KNOWN_MCS			0x0020
#define DOT11_RT_S1G_KNOWN_COLOR		0x0040
#define DOT11_RT_S1G_KNOWN_UPL_IND		0x0080

/* DATA1 field */
#define DOT11_RT_S1G_DAT1_PPDU_FMT_SET(x)	(((x) << 0) & 0x0003)
#define DOT11_RT_S1G_DAT1_RES_IND_SET(x)	(((x) << 2) & 0x000C)
#define DOT11_RT_S1G_DAT1_GI_SET(x)		(((x) << 5) & 0x0020)
#define DOT11_RT_S1G_DAT1_NSS_SET(x)		(((x) << 6) & 0x00C0)
#define DOT11_RT_S1G_DAT1_BW_SET(x)		(((x) << 8) & 0x0F00)
#define DOT11_RT_S1G_DAT1_MCS_SET(x)		(((x) << 12) & 0xF000)

enum dot11_rt_s1g_ppdu_format {
	DOT11_RT_S1G_PPDU_S1G_1M = 0,
	DOT11_RT_S1G_PPDU_S1G_SHORT = 1,
	DOT11_RT_S1G_PPDU_S1G_LONG = 2
};

enum dot11_rt_s1g_response_indication {
	DOT11_RT_S1G_RES_IND_NO_RESP = 0,
	DOT11_RT_S1G_RES_IND_NDP = 1,
	DOT11_RT_S1G_RES_IND_NORMAL = 2,
	DOT11_RT_S1G_RES_IND_LONG = 3
};

enum dot11_rt_s1g_gi {
	DOT11_RT_S1G_GI_LONG = 0,
	DOT11_RT_S1G_GI_SHORT = 1
};

enum dot11_rt_s1g_bandwidth {
	DOT11_RT_S1G_BW_1MHZ = 0,
	DOT11_RT_S1G_BW_2MHZ = 1,
	DOT11_RT_S1G_BW_4MHZ = 2,
	DOT11_RT_S1G_BW_8MHZ = 3,
	DOT11_RT_S1G_BW_16MHZ = 4,
	DOT11_RT_S1G_BW_INVALID = 5
};

/* DATA2 field */
#define DOT11_RT_S1G_DAT2_COLOR_SET(x)		(((x) << 0) & 0x0007)
#define DOT11_RT_S1G_DAT2_UPL_IND_SET(x)	(((x) << 3) & 0x0008)
#define DOT11_RT_S1G_DAT2_RSSI_SET(x)		(((x) << 8) & 0xFF00)

#define IEEE80211_RADIOTAP_HALOW_MASK_NDP_1MHZ	GENMASK_ULL(26, 0)	/* 25 bits */
#define IEEE80211_RADIOTAP_HALOW_MASK_NDP_2MHZ	GENMASK_ULL(36, 0)	/* 37 bits */

#define IEEE80211_RADIOTAP_HALOW_MASK_NDP_BW_2MHZ BIT_ULL(39)	/* bit 39 */

#ifndef IEEE80211_RADIOTAP_ZERO_LEN_PSDU
#define IEEE80211_RADIOTAP_ZERO_LEN_PSDU (26)	/* Older kernels don't have this */
#endif

struct radiotap_s1g_tlv {
	__le16 type;
	__le16 length;
	__le16 known;
	__le16 data1;
	__le16 data2;
} __packed;

#endif /* !_MORSE_S1G_RADIOTAP_H_ */
