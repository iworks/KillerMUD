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
 * KILLER MUD is copyright 1999-2012 Killer MUD Staff (alphabetical)   *
 *                                                                     *
 * Andrzejczak Dominik   (kainti@go2.pl                 ) [Kainti    ] *
 * Jaron Krzysztof       (chris.jaron@gmail.com         ) [Razor     ] *
 * Koper Tadeusz         (garloop@killer-mud.net        ) [Garloop   ] *
 * Pietrzak Marcin       (marcin.pietrzak@mud.pl        ) [Gurthg    ] *
 * Sawicki Tomasz        (furgas@killer-mud.net         ) [Furgas    ] *
 * Trebicki Marek        (maro@killer.radom.net         ) [Maro      ] *
 * Zdziech Tomasz        (t.zdziech@elka.pw.edu.pl      ) [Agron     ] *
 *                       (laszla.niet@gmail.com         ) [Laszlo    ] *
 *                                                                     *
 ***********************************************************************
 *
 * $Id: const.c 12203 2013-03-29 13:35:28Z grunai $
 * $HeadURL: http://svn.iworks.pl/svn/clients/illi/killer/trunk/src/const.c $
 *
 */
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "interp.h"
#include "lang.h"
#include "music.h"

// ¿ycie
//
const char * condition_info_all [] =
{
    "{G¿adnych ¶ladów{x",
    "{Gzadrapania{x",
    "{glekkie rany{x",
    "{f¶rednie rany{x",
    "{fciê¿kie rany{x",
    "{rogromne rany{x",
    "{rledwo stoi{x",
    "{rumiera{x"
};
const char * condition_info_construct [] =
{
    "{G¿adnych ¶ladów{x",
    "{Gzadrapania{x",
    "{glekkie uszkodzenia{x",
    "{f¶rednie uszkodzenia{x",
    "{fciê¿kie uszkodzenia{x",
    "{rogromne uszkodzenia{x",
    "{rledwo stoi{x",
    "{runieruchomiony{x"
};

//polskie literki i inne gowno
char const pol_iso	[] = "±æê³ñó¶¼¿¡ÆÊ£ÑÓ¦¬¯";

char const pol_win 	[] =
{ 0xB9, 0xE6, 0xEA, 0xB3, 0xF1, 0xF3, 0x9C, 0x9F, 0xBF,
  0xA5, 0xC6, 0xCA, 0xA3, 0xD1, 0xD3, 0x8C, 0x8F, 0xAF, 0x00 };

char const pol_nopl	[] = "acelnoszzACELNOSZZ";

char const lower_pol  [] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* ........ */
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /* ........ */
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, /* ........ */
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /* ........ */
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, /*  !"#$%&' */
  0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /* ()*+,-./ */
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 01234567 */
  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /* 89:;<=>? */
  0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, /* @abcdefg */
  0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, /* hijklmno */
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, /* pqrstuvw */
  0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, /* xyz[\]^_ */
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, /* `abcdefg */
  0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, /* hijklmno */
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, /* pqrstuvw */
  0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, /* xyz{|}~. */
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, /* ........ */
  0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, /* ........ */
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, /* ........ */
  0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x93, 0x9f, /* ........ */
  0xa0, 0xb1, 0xa2, 0xb3, 0xa4, 0xa5, 0xb6, 0xa7, /* .±.³..¶. */
  0xa8, 0xa9, 0xaa, 0xab, 0xbc, 0xad, 0xae, 0xbf, /* ....¼..¿ */
  0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, /* .±.³..¶. */
  0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, /* ....¼..¿ */
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xe6, 0xc7, /* ......æ. */
  0xc8, 0xc9, 0xea, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, /* ..ê..... */
  0xd0, 0xf1, 0xd2, 0xf3, 0xd4, 0xd5, 0xd6, 0xd7, /* .ñ.ó.... */
  0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, /* ........ */
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, /* ......æ. */
  0x68, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, /* ..ê..... */
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, /* .ñ.ó.... */
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff  /* ........ */
};

char const upper_pol  [] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* ........ */
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /* ........ */
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, /* ........ */
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /* ........ */
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, /*  !"#$%&' */
  0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /* ()*+,-./ */
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 01234567 */
  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /* 89:;<=>? */
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* @ABCDEFG */
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* HIJKLMNO */
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* PQRSTUVW */
  0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, /* XYZ[\]^_ */
  0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* `ABCDEFG */
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* HIJKLMNO */
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* PQRSTUVW */
  0x58, 0x59, 0x5a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, /* XYZ{|}~. */
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, /* ........ */
  0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, /* ........ */
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, /* ........ */
  0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x93, 0x9f, /* ........ */
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, /* .¡.£..¦. */
  0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, /* ....¬..¯ */
  0xb0, 0xa1, 0xb2, 0xa3, 0xb4, 0xb5, 0xa6, 0xb7, /* .¡.£..¦. */
  0xb8, 0xb9, 0xba, 0xbb, 0xac, 0xbd, 0xbe, 0xaf, /* ....¬..¯ */
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, /* ......Æ. */
  0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, /* ..Ê..... */
  0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, /* .Ñ.Ó.... */
  0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, /* ........ */
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xc6, 0xe7, /* ......Æ. */
  0x68, 0xe9, 0xca, 0xeb, 0xec, 0xed, 0xee, 0xef, /* ..Ê..... */
  0xf0, 0xd1, 0xf2, 0xd3, 0xf4, 0xf5, 0xf6, 0xf7, /* .Ñ.Ó.... */
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff  /* ........ */
};

char const upper_nopol  [] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* ........ */
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /* ........ */
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, /* ........ */
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /* ........ */
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, /*  !"#$%&' */
  0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /* ()*+,-./ */
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 01234567 */
  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /* 89:;<=>? */
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* @ABCDEFG */
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* HIJKLMNO */
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* PQRSTUVW */
  0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, /* XYZ[\]^_ */
  0x60, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* `ABCDEFG */
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* HIJKLMNO */
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* PQRSTUVW */
  0x58, 0x59, 0x5a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, /* XYZ{|}~. */
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, /* ........ */
  0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, /* ........ */
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, /* ........ */
  0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x93, 0x9f, /* ........ */
  0xa0, 0x41, 0xa2, 0x4c, 0xa4, 0xa5, 0x53, 0xa7, /* .A.L..S. */
  0xa8, 0xa9, 0xaa, 0xab, 0x5a, 0xad, 0xae, 0x5a, /* ....Z..Z */
  0xb0, 0x41, 0xb2, 0x4c, 0xb4, 0xb5, 0x53, 0xb7, /* .A.L..S. */
  0xb8, 0xb9, 0xba, 0xbb, 0x5a, 0xbd, 0xbe, 0x5a, /* ....Z..Z */
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0x43, 0xc7, /* ......C. */
  0xc8, 0xc9, 0x45, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, /* ..E..... */
  0xd0, 0x4e, 0xd2, 0x4f, 0xd4, 0xd5, 0xd6, 0xd7, /* .N.O.... */
  0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, /* ........ */
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0x43, 0xe7, /* ......C. */
  0x68, 0xe9, 0x45, 0xeb, 0xec, 0xed, 0xee, 0xef, /* ..E..... */
  0xf0, 0x4e, 0xf2, 0x4f, 0xf4, 0xf5, 0xf6, 0xf7, /* .N.O.... */
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff  /* ........ */
};

char const isoize_pol  [] =
{
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* ........ */
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /* ........ */
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, /* ........ */
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, /* ........ */
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, /*  !"#$%&' */
  0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, /* ()*+,-./ */
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* 01234567 */
  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, /* 89:;<=>? */
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* @ABCDEFG */
  0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, /* HIJKLMNO */
  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, /* PQRSTUVW */
  0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, /* XYZ[\]^_ */
  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, /* `abcdefg */
  0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, /* hijklmno */
  0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, /* pqrstuvw */
  0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, /* xyz{|}~. */
  0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, /* ........ */
  0x88, 0x89, 0x8a, 0x8b, 0xa6, 0x8d, 0x8e, 0xac, /* ....¦..¬ */
  0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, /* ........ */
  0x98, 0x99, 0x9a, 0x9b, 0xb6, 0x9d, 0x93, 0xbc, /* ....¶..¼ */
  0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa1, 0xa6, 0xa7, /* .¡.£.¡¦. */
  0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, /* ....¬..¯ */
  0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, /* .±.³..¶. */
  0xb8, 0xb1, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, /* .±..¼..¿ */
  0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, /* ......Æ. */
  0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, /* ..Ê..... */
  0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, /* .Ñ.Ó.... */
  0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, /* ........ */
  0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, /* ......æ. */
  0x68, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, /* ..ê..... */
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, /* .ñ.ó.... */
  0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff  /* ........ */
};

/*pogoda and stuff*/
WEATHER_DATA  weather_info[SECT_MAX];
/*****************/
const struct poison_type poison_table[MAX_POISON+1] =
{
	{
	"trucizna spowolniona",
	{ 0, 0, 0 },
	{ 0, 0, 1 },
	POISON_SLOWERED,
	{ 1, 4, 0 },
	},

	{
	"s³aba trucizna",
	{ 1,  5,  0 },
	{ 2,  5,  5 },
	POISON_WEAK,
	{ 2, 4, 1 },
	},

	{
	"¶rednia trucizna",
	{ 2,  5,  10 },
	{ 4,  5,  30 },
	POISON_NORMAL,
	{ 0, 0, -1 },
	},

	{
	"silna trucizna",
	{ 4,  5,  20 },
	{ 5,  5,  60 },
	POISON_STRONG,
	{ 0, 0, -1 },
	},

	{
	"bardzo silna trucizna",
	{  4,  5, 45 },
	{  5,  5, 80 },
	POISON_STRONGEST,
	{ 0, 0, -1 },
	},

	{//5
	"trucizna usypiaj±ca",
	{  0,  0, 0 },
	{  0,  0, 0 },
	POISON_SLEEP,
	{  2, 4, 0  },
	},

	{
	"trucizna o¶lepiaj±ca",
	{  1, 5, 0 },
	{  2, 5, 10 },
	POISON_BLIND,
	{ 0, 0, -1 },
	},

	{
	"trucizna parali¿uj±ca",
	{  3, 5, 0 },
	{  3, 5, 20 },
	POISON_PARALIZING,
	{ 0, 0, -1 },
	},

	{//8
	"trucizna zabijaj±ca",
	{  4, 5, 30 },
	{  0, 0, 0 },
	POISON_DEATH,
	{ 0, 0, -1 },
	},

	{
	"trucizna os³abiaj±ca",
	{  1,  5, 0 },
	{  2,  5, 0 },
	POISON_WEAKEN,
	{ 0, 0, -1 },
	}
};



const struct school_type school_table[MAX_SCHOOL]=
{
	{/* 0 */
	"Odrzucanie",
	"Mistrz Odrzucania",
	"Mistrzyni Odrzucania",
	"Arcymistrz Odrzucania",
	"Arcymistrzyni Odrzucania",
	Odrzucanie,
	Przemiany|Iluzje,
	{1,0,0}
	},

	{/* 1 */
	"Przemiany",
	"Mistrz Przemian",
	"Mistrzyni Przemian",
	"Alchemik",
	"Alchemiczka",
	Przemiany,
	Odrzucanie|Nekromancja,
	{1,5,0},
	},

	{/* 2 */
	"Przywo³ania",
	"Mistrz Przywo³añ",
	"Mistrzyni Przywo³añ",
	"Wielki Przywo³ywacz",
	"Wielka Pani Przywo³añ",
	Przywolanie,
	Poznanie|Inwokacje,
	{1,5,0},
	},

	{/* 3 */
	"Poznanie",
	"Mistrz Poznania",
	"Mistrzyni Poznania",
	"Mêdrzec",
	"Wiedz±ca",
	Poznanie,
	Przywolanie,
	{1,5,2},
	},

	{/* 4 */
	"Zauroczenie",
	"Mistrz Zauroczeñ",
	"Mistrzyni Zauroczeñ",
	"Zaklinacz",
	"Pani Uroków",
	Zauroczenie,
	Inwokacje|Nekromancja,
	{1,5,2},
	},

	{/* 5 */
	"Iluzje",
	"Mistrz Iluzji",
	"Mistrzyni Iluzji",
	"Iluzjonista",
	"Iluzjonistka",
	Iluzje,
	Nekromancja|Inwokacje|Odrzucanie,
	{1,4,0},
	},

	{/* 6 */
	"Inwokacje",
	"Mistrz Inwokacji",
	"Mistrzyni Inwokacji",
	"Mistyk",
	"Kszta³tuj±ca Energiê",
	Inwokacje,
	Zauroczenie|Przywolanie,
	{1,0,0},
	},

	{/* 7 */
	"Nekromancja",
	"Mistrz Nekromancji",
	"Mistrzyni Nekromancji",
	"Nekromanta",
	"W³adczyni Nieumar³ych",
	Nekromancja,
	Iluzje|Zauroczenie,
	{1,0,0},
	}
};

const struct armor_type armor_table[]=
{                                         //Mg,Kl,z³,wo,dr,pl,br,mni,brd,cz,sz
	{"pikowana",             {9, 9, 9, 10}, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
	{"skórzana",             {9, 9, 8, 10}, { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
	{"æwiekowana skórzana",  {7, 8, 7, 10}, { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } },
	{"naszywanica",          {7, 7, 7, 10}, { 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 } },

	{"³uskowa",              {6, 7, 6, 10}, { 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 } },
	{"przeszywanica",        {6, 6, 6, 10}, { 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 } },
	{"skóry zwierzêce",      {6, 8, 6, 10}, { 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1 } },
	{"lekka kolczuga",       {6, 8, 5, 10}, { 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1 } },

	{"zwyk³a kolczuga",      {5, 7, 4, 10}, { 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1 } },
	{"paskowa",              {4, 5, 3, 10}, { 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0 } },
	{"pancerz z br±zu",      {4, 4, 4, 10}, { 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0 } },
	{"wzmacniana skórzana",  {4, 5, 4, 10}, { 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0 } },

	{"pó³p³ytowa",           {3, 3, 3,  8}, { 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0 } },
	{"pó³p³ytowa wzmaciana", {2, 3, 1,  8}, { 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0 } },
	{"p³ytowa",              {1, 2, 0,  8}, { 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0 } },
	{"p³ytowa wzmacniana",   {0, 0, 0,  7}, { 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0 } },

	{NULL, {0}, {0}}
};
const SECT_DATA	sector_table[]=
{
    /* UWAGA: Nie zmieniac kolejnosci!
       mem_druid_bonus - wp³ywa równie¿ na "bonus" utraty move przez druida

       przy dodawniu nowego typu lokacji dodaæ w skillu do_hide (act_move.c) oraz mapuj (act_info.c) ew. do do_illuminate (act_obj.c)
       Name, mv_loss, heal_bonus, mem_bonus, mem_druid_bonus, min_dam, max_dam, wait, flag, herb_reset_time(WAZNE!!! musi byc mniejsze od 100), max_herb (max = 5)
     */
    {"wewn±trz",            1,   0,  0, -1,  0,   0, 1, SECT_NOWEATHER,                                                            60, 0},
    {"miasto",              1,   0,  0, -1,  0,   0, 1, SECT_LIGHT,                                                                60, 0},
    {"pole",                2,   0,  0,  1,  0,   0, 1, SECT_DRUID,                                                                20, 4},
    {"las",                 3,  10,  1,  2,  0,   0, 2, SECT_DRUID,                                                                10, 5},
    {"wzgórza",             4,   0,  0,  1,  0,   0, 2, SECT_DRUID,                                                                20, 4},
    /*5*/
    {"góry",                6,   0,  0,  2,  0,   0, 3, SECT_DRUID,                                                                40, 2},
    {"woda p³yw",           4, -20,  0,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_WATER|SECT_DRUID,                              50, 2},
    {"woda niep",           6, -30,  0,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_WATERN|SECT_WATER|SECT_DRUID,                  60, 1},
    {"nieu¿ywany",          1,   0,  0,  0,  0,   0, 0, 0,                                                                          0, 0},
    {"powietrze",           3, -30, -2,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_AIR|SECT_DRUID,                                 0, 0},
    /*10*/
    {"pustynia",            7, -40, -3,  1,  0,   0, 4, SECT_THIRST|SECT_DRUID|SECT_HOT,                                           80, 1},
    {"puszcza",             6,  10,  1,  2,  0,   0, 3, SECT_DRUID,                                                                10, 5},
    {"bagno",               5, -20, -1,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_DRUID,                                         20, 4},
    {"wys góry",            8, -10, -1,  2,  0,   0, 4, SECT_DRUID,                                                                55, 1},
    {"podziemia",           3,   0,  0,  1,  0,   0, 2, SECT_NOWEATHER,                                                            40, 2},
    /*15*/
    {"jaskinia",            3,  10,  0,  1,  0,   0, 1, SECT_NOWEATHER|SECT_DRUID,                                                 45, 2},
    {"tundra",              5, -20, -1,  2,  0,   0, 3, SECT_DRUID|SECT_COLD,                                                      40, 3},
    {"podziemia naturalne", 3,   0,  0,  1,  0,   0, 2, SECT_NOWEATHER|SECT_DRUID,                                                 55, 1},
    {"droga",               1,   0,  0,  0,  0,   0, 1, 0,                                                                         40, 2},
    {"rzeka",               3,   0,  0,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_WATER|SECT_CANDRINK|SECT_DRUID,                35, 2},
    /*20*/
    {"jezioro",             5,   0,  0,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_WATER|SECT_CANDRINK|SECT_DRUID,                40, 2},
    {"morze",               5,   0,  0,  0,  0,   0, 2, SECT_THIRST|SECT_NOMEM|SECT_NOREST|SECT_WATER|SECT_DRUID,                  50, 1},
    {"ocean",               5,   0,  0,  0,  0,   0, 2, SECT_THIRST|SECT_NOMEM|SECT_NOREST|SECT_WATER|SECT_DRUID,                  60, 1},
    {"lawa",                8, -40,  0,  0, 75, 200, 3, SECT_THIRST|SECT_NOMEM|SECT_NOREST|SECT_DRUID|SECT_HOT,                     0, 0},
    {"ruchome piaski",      9, -40,  0,  0,  0,   0, 5, SECT_THIRST|SECT_NOMEM|SECT_NOREST|SECT_DRUID,                              0, 0},
    /*25*/
    {"gor±ce ¼ród³a",       3,  20,  1,  1,  0,   0, 3, SECT_CANDRINK|SECT_DRUID|SECT_HOT,                                          0, 0},
    {"lodowiec",            7, -20, -2,  2,  0,   0, 3, SECT_DRUID|SECT_COLD,                                                      80, 1},
    {"arktyczny l±d",       5, -10, -1,  1,  0,   0, 3, SECT_DRUID|SECT_COLD,                                                      90, 1},
    {"¶cie¿ka",             2,   0,  0,  0,  0,   0, 1, 0,                                                                         35, 2},
    {"pla¿a",               3,  20,  1,  1,  0,   0, 2, SECT_DRUID,                                                                50, 1},
    /* 30 */
    {"eden",                1,  50,  3,  3,  3,   0, 2, SECT_LIGHT|SECT_DRUID|SECT_HOT,                                            10, 5},
    {"sawanna",             3,   0,  0,  0,  1,   0, 2, SECT_DRUID,                                                                25, 3},
    {"trawa",               3,   0,  0,  0,  1,   0, 1, SECT_DRUID,                                                                15, 5},
    {"step",                2,   0,  0,  0,  0,   0, 1, SECT_DRUID,                                                                30, 2},
    {"pod wod±",            6,   0,  0,  0,  0,   0, 4, SECT_NOMEM|SECT_NOREST|SECT_UNDERWATER|SECT_NOWEATHER|SECT_DRUID,          50, 2},
    /* 35 */
    {"górska ¶cie¿ka",      4,   0,  0,  1,  0,   0, 3, SECT_DRUID,                                                                60, 1},
    {"plac",                2,   0,  0,  0,  0,   0, 1, 0,                                                                         80, 1},
    {"ciemna woda",         6,   0,  0,  0,  0,   0, 4, SECT_NOMEM|SECT_NOREST|SECT_UNDERWATER|SECT_NOWEATHER|SECT_DRUID,           0, 0},
    {"park",                2,  10,  1,  1,  0,   0, 1, SECT_DRUID,                                                                20, 4},
    {"podziemne jezioro",   5,   0,  0,  0,  0,   0, 2, SECT_NOMEM|SECT_NOREST|SECT_WATER|SECT_CANDRINK|SECT_NOWEATHER|SECT_DRUID, 60, 1},
    /* 40 */
    {"wydmy",               3,   0,  0,  1,  0,   0, 2, SECT_THIRST|SECT_DRUID|SECT_HOT,                                           55, 1},
    {"arena",               1,   0,  0,  0,  0,   0, 1, SECT_NOWEATHER,                                                             0, 0},
    {"³±ka",                2,   0,  0,  1,  0,   0, 1, SECT_DRUID,                                                                 5, 5},
    {"stroma ¶cie¿ka",      3,   0,  0,  1,  0,   0, 3, SECT_DRUID,                                                                60, 0},
    {"b³otna ¶cie¿ka",      4,  -1,  0,  1,  0,   0, 3, SECT_DRUID,                                                                60, 0},
    /* 45 */
    {"podziemna droga",     1,   0,  0,  0,  0,   0, 1, SECT_NOWEATHER,                                                            40, 1},
    {"pustynna droga",      3, -30, -2,  1,  0,   0, 4, SECT_THIRST|SECT_DRUID|SECT_HOT,                                           80, 0},
    {"kopalnia",            4,   0,  0,  0,  0,   0, 2, SECT_NOWEATHER,                                                             0, 0},
    {"ruiny",               4,   0,  0,  0,  0,   0, 3, 0,                                                                          0, 0},
    {  NULL,0,0,0,0,0,0,0,0 }
};

const struct material_type material_table[] =
{
 /*
   NAME
   PRZYMIOTNIK
   HARDNESS czyli jak twardy jest material, istotne przy trafieniu czy bron/zbroja ulegla zniszczeniu
   FIRE
   ACID
   ELECTR
   COLD
   OTHER


 UWAGA: nie zmieniaæ kolejno¶ci!

 Dodaj±c metalowy, rdzewiej±cy materia³, proszê dodaæ go czaru spell_corrode
 w pliku spells_dru.c

 */

/*0*/
 { "unused",         "",               1,   1,   1,   1,   1,   1,   0 },
 { "drewno",         "drewniany",     10,   0,  20,  20,  45,  10,   0 },
 { "stal",           "stalowy",       60,  75,  35, 100, 100,  25,   MAT_METAL },
 { "elfia stal",     "stalowy",       75,  85,  55, 100, 100,  25,   MAT_METAL },
 { "doskona³a stal", "stalowy",       65,  80,  45, 100, 100,  25,   MAT_METAL },
/*5*/
 { "mied¼",          "miedziany",     40,  65,   5, 100, 100,  25,   MAT_METAL },
 { "skóra",          "skórzany",      20,  35,   5,  75,  30,  10,   0 },
 { "adamantyt",      "adamantytowy",  92,  95,  70, 100, 100,  30,   MAT_METAL },
 { "mithril",        "mithrilowy",    90,  95,  85, 100, 100,  35,   MAT_METAL },
 { "z³oto",          "z³oty",         25,  45,  25, 100, 100,  25,   MAT_METAL },
/*10*/
 { "srebro",         "srebrny",       35,  55,  15, 100, 100,  25,   MAT_METAL },
 { "kryszta³",       "kryszta³owy",   70,  95, 100, 100, 100,  45,   MAT_EASYBREAK },
 { "nieznany",       "",               1,   0,   0,   0,   0,   0,   0 },
 { "¿elazo",         "¿elazny",       45,  75,  10, 100, 100,  20,   MAT_METAL },
 { "br±z",           "br±zowy",       45,  65,   5, 100, 100,  25,   MAT_METAL },
/*15*/
 { "szk³o",          "szklany",       55, 100, 100, 100, 100,  15,   MAT_EASYBREAK },
 { "kamieñ",         "kamienny",      35,  95, 100, 100, 100,  50,   0 },
 { "jedzenie",       "",               0,   0,   0,   0,   0,   0,   0 },
 { "jedwab",         "jedwabny",       5,   0,   0,   0,  50,   0,   0 },
 { "glina",          "gliniany",       5,   0,   0,   0,  50,   0,   0 },
/*20*/
 { "papier",         "papierowy",      5,   0,   0,   0,  50,   0,   0 },
 { "ko¶æ",           "ko¶ciany",      35,  40,  10,  90,  60,  10,   0 },
 { "marmur",         "marmurowy",     20,  95, 100, 100, 100,  50,   0 },
 { "granit",         "granitowy",     30,  95, 100, 100, 100,  50,   0 },
 { "p³ótno",         "p³ócienny",      5,   0,   0,   0,  50,   0,   0 },
/*25*/
 { "len",            "lniany",         5,   0,   0,   0,  50,   0,   0 },
 { "we³na",          "we³niany",       5,   0,   0,   0,  50,   0,   0 },
 { "krzemieñ",       "krzemienny",    35,  95, 100, 100, 100,  50,   0 },
 { "o³ów",           "o³owiany",      20,  45,  15,  55, 100,  25,   MAT_METAL },
 { "filc",           "filcowy",       10,   0,   0,   0,  60,   0,   0 },
/*30*/
 { "kaszmir",          "kaszmirowy",     5,   0,   0,   0,  40,   0,   0 },
 { "porcelana",        "porcelanowy",   50, 100, 100, 100, 100,  15,   MAT_EASYBREAK },
 { "damasceñska stal", "stalowy",       85,  90,  55, 100, 100,  35,   MAT_METAL },
 { "illit",            "illittowy",     91,  65,  10,  60, 100,  35,   MAT_METAL },
 { "platyna",          "platynowy",     45,  70,  10,  95,  95,  30,   MAT_METAL },
/*35*/
 { "at³as",            "at³asowy",       5,   0,   0,   5,  40,   0,   0 },
 { "diament",          "diamentowy",   100,  95, 100, 100, 100,  45,   0 },
 { "trzcina",          "trzcinowy",      5,   0,  15,  15,  40,   5,   0 },
 { "pergamin",         "pergaminowy",    5,   0,   0,   0,  50,   0,   0 },
 { "ro¶lina",          "ro¶linny",       5,   0,  15,  15,  40,   5,   0 },
/*40*/
 { "chityna",          "chitynowy",     30,  40,  25,  85,  40,  25,   0 },
 { "smocze ³uski",     "smoczy",        65,  80,  40, 100, 100,  70,   0 },
 { "smoczy z±b",       "smoczy",        80, 100,  50, 100, 100,  80,   0 },
 { "mosi±dz",          "mosiê¿ny",      35,  60,   0, 100,  95,  25,   MAT_METAL },
 { "mocna skóra",      "skórzany",      25,  40,  20,  85,  40,  20,   0 },
/*45*/
 { "eteryczny",        "eteryczny",     30,  75,  75,  75,  75,  80,   0 },
 { "konopie",          "konopny",        5,   0,   5,   5,  60,   0,   0 },
 { "w³osie",           "",		         5,   0,   5,   5,  60,   0,   0 },
 { "drewno dêbowe",    "dêbowy",        40,   5,  25,  30,  60,  20,   0 },
 { "stalodrzew",       "stalodrzewny",  50,  30,  35,  45,  80,  40,   0 },
/*50*/
 { "bursztyn",         "bursztynowy",   15,  10,  75,  95,  40,  30,   0 },
 { "pierze",           "",               5,   0,   5,   5,   5,  50,   0 },
 { "luska",            "",              50,   0,  25,   5,   0,   0,   0 },
/**
 * jezeli dodajesz material, dodaj tez obsluge w spells_mag.c dla czaru
 * tworzacego golemy: spell_changestaff
 */
 { NULL,               NULL,             0,   0,   0,   0,   0,   0,   0 }
};


/* item type list */
const struct item_type		item_table	[]	=
{
	{	ITEM_LIGHT,			"light"		},
	{	ITEM_SCROLL,		"scroll"	},
	{	ITEM_WAND,			"wand"		},
	{  	ITEM_STAFF,			"staff"		},
	{ 	ITEM_WEAPON,		"weapon"	},
	{  	ITEM_TREASURE,		"treasure"	},
	{  	ITEM_ARMOR,			"armor"		},
	{	ITEM_POTION,		"potion"	},
	{	ITEM_CLOTHING,		"clothing"	},
	{   ITEM_FURNITURE,		"furniture"	},
	{	ITEM_TRASH,			"trash"		},
	{	ITEM_CONTAINER,		"container"	},
	{	ITEM_DRINK_CON,		"drink"		},
	{	ITEM_KEY,			"key"		},
	{	ITEM_FOOD,			"food"		},
	{	ITEM_MONEY,			"money"		},
	{	ITEM_BOAT,			"boat"		},
	{	ITEM_CORPSE_NPC,	"npc_corpse"},
	{	ITEM_CORPSE_PC,		"pc_corpse"	},
	{ 	ITEM_FOUNTAIN,		"fountain"	},
	{	ITEM_PILL,			"pill"		},
	{	ITEM_PROTECT,		"protect"	},
	{	ITEM_MAP,			"map"		},
	{	ITEM_PORTAL,		"portal"	},
	{	ITEM_ROOM_KEY,		"room_key"	},
	{	ITEM_GEM,			"gem"		},
	{	ITEM_JEWELRY,		"jewelry"	},
	{  	ITEM_JUKEBOX,		"jukebox"	},
	{  	ITEM_BOARD,			"board"		},
	{  	ITEM_PIECE,			"piece"		},
	{ 	ITEM_SPELLBOOK,		"spellbook"	},
	{  	ITEM_SPELLITEM,		"spellitem"	},
	{  	ITEM_ENVENOMER,		"envenomer"	},
	{ 	ITEM_BANDAGE,		"bandage"	},
	{ 	ITEM_TURN,			"turn_item"	},
	{  	ITEM_HERB,			"herb"		},
	{  	ITEM_SKIN,			"skin"		},
	{	ITEM_HORN,			"horn"		},
	{	ITEM_MUSICAL_INSTRUMENT, "musical_instrument"	},
	{	ITEM_SHIELD, 		"shield"	},
	{	ITEM_TROPHY, 		"trophy"	},
	{	ITEM_PIPE,			"pipe"		},
	{	ITEM_WEED,			"weed"		},
	{	ITEM_NOTEPAPER,			"notepaper"		},
	{ ITEM_TOOL, "tool" },
	{ ITEM_TOTEM, "totem" },
	{	0,		NULL		}
};


/* weapon selection table */
const	struct	weapon_type	weapon_table	[]	=
{
   { "sword",	WEAPON_SWORD,		&gsn_sword },
   { "mace",	WEAPON_MACE,		&gsn_mace },
   { "dagger",	WEAPON_DAGGER,		&gsn_dagger },
   { "axe",		WEAPON_AXE,			&gsn_axe },
   { "staff",	WEAPON_SPEAR,		&gsn_spear },
   { "flail",	WEAPON_FLAIL,		&gsn_flail },
   { "whip",	WEAPON_WHIP,		&gsn_whip },
   { "polearm",	WEAPON_POLEARM,		&gsn_polearm },
   { "staff",	WEAPON_STAFF,		&gsn_staff },
   { "short-sword",WEAPON_SHORTSWORD,	&gsn_shortsword },
   { "claws", WEAPON_CLAWS, &gsn_claws },
   { NULL,			0,	NULL	}
};

/* instrument selection table */
const	struct	instrument_type	instrument_table	[]	=
{
   { "ocarina",	INSTRUMENT_OCARINA,		&gsn_ocarina },
   { "flute",	INSTRUMENT_FLUTE,		&gsn_flute },
   { "lute",	INSTRUMENT_LUTE,		&gsn_lute },
   { "harp",	INSTRUMENT_HARP,		&gsn_harp },
   { "mandolin",INSTRUMENT_MANDOLIN,	&gsn_mandolin },
   { "piano",	INSTRUMENT_PIANO,		&gsn_piano },
   { NULL,			0,	NULL	}
};

/* wiznet table and prototype for future flag setting */
const   struct wiznet_type      wiznet_table    []              =
{
   {    "on",           WIZ_ON,         IM },
   {    "prefix",		WIZ_PREFIX,		IM },
   {    "ticks",        WIZ_TICKS,      IM },
   {    "logins",       WIZ_LOGINS,     IM },
   {    "sites",        WIZ_SITES,      L4 },
   {    "links",        WIZ_LINKS,      L7 },
   {	"newbies",		WIZ_NEWBIE,		IM },
   {	"spam",			WIZ_SPAM,		L5 },
   {    "deaths",       WIZ_DEATHS,     IM },
   {    "resets",       WIZ_RESETS,     L4 },
   {    "mobdeaths",    WIZ_MOBDEATHS,  L4 },
   {    "flags",		WIZ_FLAGS,		L5 },
   {	"penalties",	WIZ_PENALTIES,	L5 },
   {	"saccing",		WIZ_SACCING,	L5 },
   {	"levels",		WIZ_LEVELS,		IM },
   {	"load",			WIZ_LOAD,		L2 },
   {	"restore",		WIZ_RESTORE,	L2 },
   {	"snoops",		WIZ_SNOOPS,		L2 },
   {	"switches",		WIZ_SWITCHES,	L2 },
   {	"secure",		WIZ_SECURE,		L2 },
   {	"artefact",		WIZ_ARTEFACT,	L1 },
   {	"artefactload",	WIZ_ARTEFACTLOAD,L1},
   {	"mxp",			WIZ_MXP,		IM },
   {	"comm",			WIZ_COMM,		L3 },
   {	NULL,		0,		0  }
};

const 	struct attack_type	attack_table	[MAX_DAMAGE_MESSAGE]	=
{
/*  0 */
	{	"none",			"uderzenie",		-1		},
	{	"ciecie",		"ciêcie", 			DAM_SLASH	},
	{	"pchniecie",	"pchniêcie",		DAM_PIERCE	},
	{	"ciecie",		"ciêcie",			DAM_SLASH	},
	{	"whip",			"smagniêcie",		DAM_SLASH	},
/*  5 */
	{	"claw",			"szczypniêcie",		DAM_SLASH	},
	{	"podmuch",		"podmuch",			DAM_BASH	},
	{	"walniecie",	"walniêcie",		DAM_BASH	},
	{	"mwalniecie",	"mia¿d¿±ce walniêcie",	DAM_BASH	},
	{	"grep",			"grep",				DAM_SLASH	},
/* 10 */
	{	"gryzienie",	"ugryzienie",		DAM_PIERCE	},
	{	"klucie",		"uk³ucie",			DAM_PIERCE	},
	{	"ssanie",		"ssanie",			DAM_BASH	},
	{	"uderzenie",	"uderzenie",		DAM_BASH	},
	{	"digestion",	"¿r±ce uderzenie",	DAM_ACID	},
/* 15 */
	{	"szarza",		"szar¿a",		DAM_BASH	},
	{	"klapniecie",		"klepniêcie",		DAM_BASH	},
	{	"uderzenie",		"uderzenie",		DAM_BASH	},
	{	"mindsuck",		"wyssanie umys³u",	DAM_MENTAL	},
	{	"magia",		"magia",		DAM_ENERGY	},
/* 20 */
	{	"bmoc",			"boska moc",		DAM_HOLY	},
	{	"ciecie",		"ciêcie",		DAM_SLASH	},
	{	"drapanie",		"drapniêcie",		DAM_SLASH	},
	{	"dziobanie",		"dziobniêcie",		DAM_PIERCE	},
	{	"dziobanieb",		"dziobniêcie",		DAM_BASH	},
/* 25 */
	{	"siekanie",		"siekniêcie",		DAM_SLASH	},
	{	"zadlenie",		"u¿±dlenie",		DAM_PIERCE	},
	{	"walenie",		"walniêcie",		DAM_BASH	},
	{	"sgryz",		"szokuj±ce ugryzienie",	DAM_LIGHTNING	},
	{	"ogryz",		"p³omienne ugryzienie",	DAM_FIRE	},
/* 30 */
	{	"mgryz",		"mro¿±ce ugryzienie",	DAM_COLD	},
	{	"kgryz",		"kwasowe ugryzienie",	DAM_ACID	},
	{	"chomp",		"chomp",		DAM_PIERCE	},
	{	"drain",		"wyssanie ¿ycia",	DAM_NEGATIVE	},
	{	"dzganie",		"d¼gniêcie",		DAM_PIERCE	},
/* 35 */
	{	"slime",		"slime",		DAM_ACID	},
	{	"wstrzas",		"wstrz±¶niêcie",	DAM_LIGHTNING	},
	{	"lupniecie",		"³upniêcie",		DAM_BASH	},
	{	"plomien",		"p³omieñ",		DAM_FIRE	},
	{	"chlod",		"ch³ód",		DAM_COLD	},
/* 40 */
	{	"k±sanie",		"uk±szenie",		DAM_PIERCE	},
	{	"pciecie",		"precyzyjne ciêcie",	DAM_SLASH	},
	{	"zciecie",		"zamaszyste ciêcie",	DAM_SLASH	},
	{	"udtarcz±",		"uderzenie tarcz±",		DAM_BASH	},
	{ "iluzcios",   "uderzenie",   DAM_MENTAL  },

	{	NULL,			NULL,			0	}
};

const	struct	pc_race_type	pc_race_table	[]	=
{
	{ "null race", "", 0, { 0, 0, 0, 0, 0 },
      { "" },  0,0,0, 0,  { 0, 0, 0, 0}, { 0, 0, 0, 0 }
	},

	{
	"cz³owiek",
	"Cz³owiek",
	1|2|4|8|16|32|64|128|256|512|1024,
	{ 0, 0, 0, 0, 0, 2, 1},
	{ "" },
	SIZE_MEDIUM ,
	LANG_COMMON,
	0,
	0,
	{160,215,140,190},
	{650, 1100, 500, 800},
	{16,20},
		{
		{0,1},
		{0,1},
		{0,1},
		{0,1},
		{0,1},
		{0,1},
		{0,1}
		}
	},
	{
	"elf",
	"Elf",
   	1|2|4|8|32|256|1024,
	{ 0, 0,  0, 1, 0, 1, 0},
	{ "" },
	SIZE_SMALL,
	LANG_ELVEN,
	LANG_COMMON,
	LANG_DWARVEN|LANG_OGRE|LANG_ORCISH|LANG_TROLLISH,
	{170,210,165,190},
	{550, 800, 400, 650},
	{65,78},
		{{0,0},{0,1},{0,1},{0,2},{0,1},{0,1},{0,0}}
	},
	{
	"krasnolud",
	"Krasnolud",
	2|4|8|32|64|1024,
	{ 0, 0, 0, 0, 1, -1, 0},
	{ "" },
	SIZE_MEDIUM,
	LANG_DWARVEN,
	LANG_COMMON,
	LANG_ELVEN,
	{110, 150, 100, 145},
	{650,900, 550, 750},
	{37,43},
		{{0,2},{0,1},{0,1},{0,-1},{1,2},{0,0},{0,0}}
	},
	{
	"gnom",
	"Gnom",
	1|2|4|8|32|1024,
	{ 0, 1, -1, 0, 0, 0, 0},
	{ "" },
	SIZE_SMALL,
	LANG_COMMON,
	LANG_DWARVEN|LANG_HALFLING,
	0,
	{110, 150, 100, 145},
	{450, 650, 350, 550},
	{27,33},
		{{0,0},{1,2},{0,1},{0,1},{0,1},{0,0},{0,0}}
	},
	{
	"pó³elf",
	"Pó³elf",
	1|2|4|8|128|256|512|1024,
	{ 0, 0, 0, 0, 0, 0, 0},
	{ "" },
	SIZE_MEDIUM,
	LANG_COMMON,
	LANG_ELVEN,
	0,
	{160,210,145,190},
	{650, 1000, 500, 800},
	{16,25},
		{{0,1},{0,1},{0,1},{0,1},{0,1},{0,1},{0,0}}
	},
	{
	"nizio³ek",
	"Nizio³ek",
	2|4|8|1024,
	{ -1, 0, 0, 1, 0, 0, 2 },
	{ "sneak","hide" },
	SIZE_SMALL,
	LANG_COMMON|LANG_HALFLING,
	LANG_DWARVEN,
	0,
	{110, 150, 100, 145},
	{450, 650, 350, 550},
	{23,27},
		{{0,-1},{0,1},{0,1},{1,2},{0,1},{0,0},{0,1}}
	},
	{
	"pó³ork",
	"Pó³ork",
	8|64|1024,
	{ 1, -1, -1, 0, 1, -2, 0 },
	{ "" },
	SIZE_MEDIUM ,
	LANG_ORCISH,
	LANG_COMMON,
	LANG_ELVEN,
	{170,225,155,200},
	{850, 1350, 600, 1100},
	{14,18},
		{{1,2},{0,-1},{0,0},{0,1},{1,2},{0,0},{0,0}}
	}

};




/*
 * Class table.
 */
const	struct	class_type	class_table	[MAX_CLASS]	=
{
	{
	"Mag", //Nazwa klasy
	"Czarodziejka", //Nazwa klasy: kobita
	"Arcymag", //Nazwa dla 31 poziomu
	"Arcyczarodziejka",//Nazwa dla 31 poziomu: kobita
	"Mag", //Trzy litery odpowiadajace klasie
	STAT_INT, //Glowny atrybut
	20, //Thac0 (szansa trafienia) dla poziomu 0
	12, //Thac0 (szansa trafienia) dla poziomu 32 - im mniej tym lepsza.
	5, //Minimalna ilosc hp, zdobywana przy level upie
	8, //Maxymalna ilosc hp, zdobywana przy level upie
	0, //Numer castera (-1 powoduje ze postac nie czaruje)
		{{0,0},{1,2},{0,1},{0,1},{0,0},{0,1},{0,0}} //NOWE STATY
	},

	{
	"Kleryk",
	"Kap³anka",
	"Hierofant",
	"Arcykap³anka",
	"Cle",
	STAT_WIS,
	20,
	6,
	8,
	13,
	1,
		{{0,1},{0,1},{1,2},{0,0},{0,1},{0,0},{0,0}}
	},

	{
	"Z³odziej",
	"Z³odziejka",
	"Zabójca",
	"Skrytobójczyni",
	"Thi",
	STAT_DEX,
	20,
	7,
	9,
	14,
	-1,
		{{0,1},{0,1},{0,0},{1,2},{0,1},{0,0},{0,0}}

	},

	{
	"Wojownik",
	"Wojowniczka",
	"Szermierz Natchniony",
	"Fechmistrzyni",
	"War",
	STAT_STR,
	20,
	-6,
	15,
	20,
	-1,
		{{1,1},{0,0},{0,0},{1,1},{1,1},{0,0},{0,0}}
	},
	{
	"Paladyn",
	"Paladynka",
	"Krzy¿owiec",
	"Inkwizytorka",
	"Pal",
	STAT_CHA,
	20,
	-2,
	15,
	20,
	 4,
		{{0,1},{0,0},{0,1},{0,1},{0,1},{1,1},{0,0}}
	},
	{
	"Druid",
	"Druidka",
	"Arcydruid",
	"W³adczyni Natury",
	"Dru",
	STAT_WIS,
	20,
	6,
	8,
	13,
	5,
		{{0,1},{0,0},{1,2},{0,1},{0,1},{0,0},{0,0}}
	},
	{
	"Barbarzyñca",
	"Barbarzynka",
	"Berserker",
	"Dzika Kobieta",
	"Bar",
	STAT_STR,
	20,
	 2,
	18,
	25,
	-1,
		{{1,2},{0,0},{0,0},{0,0},{1,2},{0,0},{0,0}}
	},
	{
	"Mnich",
	"Mniszka",
	"Opat",
	"Prze³o¿ona Zakonu",
	"Mni",
	STAT_WIS,
	20,
	-2,
	12,
	18,
	-1,
		{{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}} //UZUPELNIc!!!!
	},
	{
	"Bard",
	"Trubadurka",
	"Artysta",
	"Artystka",
	"Brd",
	STAT_CHA,
	20,
	7,
	9,
	14,
	8,
		{{0,0},{0,1},{0,0},{0,1},{0,0},{1,2},{0,1}}
	},
	{
	"Czarny Rycerz",
	"Czarna Stra¿niczka",
	"Rycerz ¦mierci",
	"W³adczyni Mroku",
	"Blk",
	STAT_STR,
	20,
	-2,
	15,
	20,
	-1,
		{{0,1},{0,0},{0,1},{0,1},{0,1},{1,1},{0,0}}
	},
	{
	"Szaman",
	"Szamanka",
	"Krocz±cy W¶ród Duchów",
	"Krocz±ca W¶ród Duchów",
	"Sha",
	STAT_STR,
	20,
	 4,
	9,
	14,
	10,
		{{0,1},{0,0},{1,1},{0,1},{0,1},{0,0},{0,0}}
	}
};



/*
 * Attribute bonus tables.
 *
 * struct   str_app_type
 * {
 *     sh_int   tohit; - bonus do trafienia
 *     sh_int   todam; - bonus do obrazen
 *     sh_int   carry; - udzwig
 *     sh_int   wield; - bron na wield
 * };
 *
 */

const struct str_app_type str_app [256] =
{
{ -4, -3,   4,  0 },  /* 0  */
{ -4, -3,   6,  0 },
{ -4, -3,   8,  0 },
{ -4, -3,  10,  0 },
{ -4, -3,  12,  0 },
{ -4, -3,  14,  0 },
{ -4, -3,  16,  1 },  /* 6  */
{ -4, -3,  18,  1 },
{ -4, -3,  20,  1 },
{ -4, -3,  22,  1 },
{ -4, -3,  24,  1 },
{ -4, -3,  26,  1 },
{ -4, -3,  28,  2 },//12
{ -4, -3,  30,  2 },
{ -4, -3,  32,  2 },
{ -4, -3,  34,  2 },
{ -4, -3,  36,  2 },
{ -4, -3,  38,  2 },
{ -4, -3,  40,  3 },//18
{ -4, -3,  42,  3 },//19
{ -4, -3,  44,  3 },//20
{ -4, -3,  46,  3 },//21
{ -4, -3,  48,  3 },//22
{ -4, -3,  50,  3 },//23
{ -4, -3,  52,  4 },//24
{ -4, -3,  54,  4 },//25
{ -4, -3,  56,  4 },//26
{ -4, -3,  58,  4 },//27
{ -4, -3,  60,  4 },//28
{ -4, -3,  62,  4 },//29
{ -3, -2,  64,  5 },//30
{ -3, -2,  66,  5 },//31
{ -3, -2,  68,  5 },//32
{ -3, -2,  70,  5 },//33
{ -3, -2,  72,  5 },//34
{ -3, -2,  74,  5 },//35
{ -3, -2,  76,  6 },//36
{ -3, -2,  78,  6 },//37
{ -3, -2,  80,  6 },//38
{ -3, -2,  82,  6 },//39
{ -3, -2,  84,  6 },//40
{ -3, -2,  86,  6 },//41
{ -2, -1,  88,  7 },//42
{ -2, -1,  80,  7 },//43
{ -2, -1,  82,  7 },//44
{ -2, -1,  84,  7 },//45
{ -2, -1,  86,  7 },//46
{ -2, -1,  88,  7 },//47
{ -2, -1,  80,  8 },//48
{ -2, -1,  82,  8 },//49
{ -2, -1,  85,  8 },//50
{ -2, -1,  88,  8 },//51
{ -2, -1,  91,  8 },//52
{ -2, -1,  94,  8 },//53
{ -1, -1,  97,  9 },//54
{ -1, -1, 100,  9 },//55
{ -1, -1, 103,  9 },//56
{ -1, -1, 106,  9 },//57
{ -1, -1, 109,  9 },//58
{ -1, -1, 112,  9 },//59
{ -1,  0, 116, 10 },//60
{ -1,  0, 120, 10 },//61
{ -1,  0, 124, 10 },//62
{ -1,  0, 128, 10 },//63
{ -1,  0, 132, 10 },//64
{ -1,  0, 136, 10 },//65
{  0,  0, 140, 10 }, // 66
{  0,  0, 144, 11 }, // 67
{  0,  0, 148, 11 }, // 68
{  0,  0, 152, 11 }, // 69
{  0,  0, 157, 11 }, // 70
{  0,  0, 162, 11 }, // 71
{  0,  0, 167, 11 }, // 72
{  0,  0, 172, 12 }, // 73
{  0,  0, 177, 12 }, // 74
{  0,  0, 182, 12 }, // 75
{  0,  0, 187, 12 }, // 76
{  0,  0, 192, 12 }, // 77
{  0,  0, 197, 12 }, // 78
{  0,  0, 202, 13 }, // 79
{  0,  0, 208, 13 }, // 80
{  0,  0, 214, 13 }, // 81
{  0,  0, 220, 13 }, // 82
{  0,  0, 226, 13 }, // 83
{  0,  0, 232, 13 }, // 84
{  0,  0, 238, 14 }, // 85
{  0,  0, 244, 14 }, // 86
{  0,  0, 250, 14 }, // 87
{  0,  0, 256, 14 }, // 88
{  0,  0, 262, 14 }, // 89
{  0,  0, 269, 14 }, // 90
{  0,  0, 276, 15 }, // 91
{  0,  0, 283, 15 }, // 92
{  0,  0, 290, 15 }, // 93
{  0,  0, 297, 15 }, // 94
{  0,  0, 304, 15 }, // 95
{  0,  1, 311, 15 }, // 96
{  0,  1, 318, 16 }, // 97
{  0,  1, 325, 16 }, // 98
{  0,  1, 332, 17 }, // 99
{  0,  1, 340, 18 }, // 100
{  0,  1, 348, 19 }, // 101
{  1,  1, 356, 20 }, // 102
{  1,  1, 364, 21 }, // 103
{  1,  1, 372, 22 }, // 104
{  1,  1, 380, 22 }, // 105
{  1,  1, 388, 23 }, // 106
{  1,  1, 396, 24 }, // 107
{  1,  2, 402, 25 }, // 108
{  1,  2, 410, 25 }, // 109
{  1,  2, 419, 25 }, // 110
{  1,  2, 428, 25 }, // 111
{  1,  2, 437, 25 }, // 112
{  1,  2, 446, 25 }, // 113
{  1,  2, 445, 26 }, // 114
{  1,  2, 454, 27 }, // 115
{  1,  2, 463, 27 }, // 116
{  1,  2, 472, 28 }, // 117
{  1,  2, 481, 28 }, // 118
{  1,  2, 490, 29 }, // 119
{  1,  3, 500, 29 }, // 120
{  1,  3, 510, 30 }, // 121
{  1,  3, 520, 30 }, // 122
{  1,  3, 530, 30 }, // 123
{  1,  3, 540, 31 }, // 124
{  1,  3, 550, 31 }, // 125
{  2,  3, 560, 31 }, // 126
{  2,  3, 570, 32 }, // 127
{  2,  3, 580, 32 }, // 128
{  2,  3, 590, 32 }, // 129
{  2,  3, 601, 33 }, // 130
{  2,  3, 612, 33 }, // 131
{  2,  3, 623, 33 }, // 132
{  2,  3, 634, 34 }, // 133
{  2,  3, 645, 34 }, // 134
{  2,  3, 656, 34 }, // 135
{  2,  3, 667, 35 }, // 136
{  2,  3, 678, 35 }, // 137
{  3,  3, 689, 36 }, // 138
{  3,  3, 700, 36 }, // 139
{  3,  4, 712, 36 }, // 140
{  3,  4, 724, 36 }, // 141
{  3,  4, 736, 37 }, // 142
{  3,  4, 748, 37 }, // 143
{  3,  4, 760, 37 }, // 144
{  3,  4, 772, 38 }, // 145
{  3,  4, 784, 38 }, // 146
{  3,  4, 796, 38 }, // 147
{  3,  4, 808, 38 }, // 148
{  3,  4, 820, 39 }, // 149
{  4,  4, 833, 39 }, // 150
{  4,  4, 846, 40 }, // 151
{  4,  4, 859, 40 }, // 152
{  4,  4, 872, 40 }, // 153
{  4,  4, 885, 41 }, // 154
{  4,  4, 898, 42 }, // 155
{  4,  4, 911, 43 }, // 156
{  4,  4, 924, 44 }, // 157
{  4,  4, 937, 45 }, // 158
{  4,  4, 950, 46 }, // 159
{  4,  4, 964, 47 }, // 160
{  4,  4, 978, 48 }, // 161
{  4,  4, 992, 49 }, // 162
{  4,  4,1006, 50 }, // 163
{  4,  4,1020, 52 }, // 164
{  4,  4,1034, 54 }, // 165
{  4,  4,1048, 56 }, // 166
{  4,  4,1062, 58 }, // 167
{  4,  4,1076, 59 }, // 168
{  4,  4,1090, 60 }, // 169
{  4,  4,1205, 62 }, // 170
{  4,  4,1220, 64 }, // 171
{  4,  5,1235, 66 }, // 172
{  4,  5,1250, 68 }, // 173
{  4,  5,1265, 69 }, // 174
{  4,  5,1280, 70 }, // 175
{  4,  5,1295, 71 }, // 176
{  4,  5,1310, 72 }, // 177
{  4,  5,1325, 73 }, // 178
{  4,  5,1340, 74 }, // 179
{  5,  5,1356, 74 }, // 180
{  5,  5,1372, 75 }, // 181
{  5,  5,1388, 76 }, // 182
{  5,  5,1404, 77 }, // 183
{  5,  5,1420, 78 }, // 184
{  5,  5,1436, 79 }, // 185
{  5,  5,1452, 79 }, // 186
{  5,  5,1468, 80 }, // 187
{  5,  6,1484, 81 }, // 188
{  5,  6,1500, 82 }, // 189
{  5,  6,1517, 83 }, // 190
{  5,  6,1534, 84 }, // 191
{  5,  6,1551, 84 }, // 192
{  5,  6,1568, 85 }, // 193
{  5,  6,1585, 86 }, // 194
{  5,  6,1602, 87 }, // 195
{  5,  6,1619, 88 }, // 196
{  5,  6,1636, 89 }, // 197
{  5,  6,1653, 89 }, // 198
{  5,  6,1670, 90 }, // 199
{  5,  6,1688, 91 }, // 200
{  5,  6,1706, 92 }, // 201
{  5,  6,1724, 93 }, // 202
{  5,  7,1742, 94 }, // 203
{  5,  7,1760, 95 }, // 204
{  5,  7,1778, 96 }, // 205
{  5,  7,1796, 97 }, // 206
{  5,  7,1814, 98 }, // 207
{  5,  7,1832, 99 }, // 208
{  5,  7,1850, 100 }, // 209
{  5,  7,1869, 101 }, // 210
{  5,  8,1888, 102 }, // 211
{  5,  8,1917, 103 }, // 212
{  5,  8,1936, 104 }, // 213
{  5,  8,1955, 105 }, // 214
{  5,  8,1974, 106 }, // 215
{  5,  8,1993, 107 }, // 216
{  5,  8,2012, 108 }, // 217
{  5,  8,2031, 109 }, // 218
{  5,  9,2050, 110 }, // 219
{  5,  9,2070, 111 }, // 220
{  5,  9,2090, 112 }, // 221
{  5,  9,2110, 113 }, // 222
{  5,  9,2130, 114 }, // 223
{  5,  9,2150, 115 }, // 224
{  6,  9,2170, 116 }, // 225
{  6,  9,2190, 117 }, // 226
{  6,  9,2210, 118 }, // 227
{  6,  9,2230, 119 }, // 228
{  6,  9,2250, 200 }, // 229
{  6, 10,2270, 201 }, // 230
{  6, 10,2290, 202 }, // 231
{  6, 10,2310, 203 }, // 232
{  6, 10,2330, 204 }, // 233
{  6, 10,2350, 205 }, // 234
{  6, 11,2370, 206 }, // 235
{  6, 11,2390, 207 }, // 236
{  6, 11,2410, 208 }, // 237
{  6, 11,2430, 209 }, // 238
{  6, 11,2450, 210 }, // 239
{  6, 11,2470, 212 }, // 240
{  7, 11,2490, 214 }, // 241
{  7, 11,2510, 216 }, // 242
{  7, 12,2530, 218 }, // 243
{  7, 12,2550, 220 }, // 244
{  7, 12,2570, 223 }, // 245
{  7, 12,2600, 226 }, // 246
{  7, 12,2630, 229 }, // 247
{  7, 12,2660, 232 },  // 248
{  8, 12,2690, 235 },  // 249
{  8, 12,2720, 239 },  // 250
{  8, 13,2750, 243 },  // 251
{  8, 13,2780, 247 },  // 252
{  9, 13,2850, 252 },  // 253
{  9, 13,2900, 259 },  // 254
{ 10, 14,3000, 270 }  /* 255 */
};

const	struct	int_app_type	int_app		[34]		=
{//do studiowania ksiag i uczenia skilli, uczenie sie trikow, nie uzywane
	{  3,-99,  0},	/*  0 */
	{  5,-99,  0},	/*  1 */
	{  7,-99,  0},
	{  8,-50,  0},	/*  3 */
	{  9,-40,  0},
	{ 10,-30,  0},	/*  5 */
	{ 11,-20,  0},
	{ 12,-15,  0},
	{ 13,-10,  0},
	{ 15, -7,  6},
	{ 17, -4,  7},	/* 10 */
	{ 19,  0,  7},
	{ 22,  0,  7},
	{ 25,  0,  9},
	{ 28,  1,  9},
	{ 31,  2, 11},	/* 15 */
	{ 34,  3, 11},
	{ 37,  5, 14},
	{ 42,  8, 18},	/* 18 */
	{ 47, 10, -1},
	{ 55, 12, -1},	/* 20 */
	{ 60, 15, -1},
	{ 65, 20, -1},
	{ 72, 25, -1},
	{ 80, 30, -1},
	{ 85, 35, -1},	/* 25 */
	{ 90, 40, -1},
	{ 95, 50, -1},
	{ 96, 60, -1},	/* 28 */
	{ 97, 65, -1},
	{ 98, 69, -1},
	{ 99, 73, -1},
	{ 100, 77, -1},
	{ 100, 80, -1} /* 33*/
};

const	struct	wis_app_type	wis_app		[34]		=
{//dodatkowe czary kleryka/druida, szansa na zepsucie zaklecia, premia do klasy pancerza mnicha (wartosc * -10), dodatkowe czary dla szamana
	{ -3, 99, -6 },//0
	{ -3, 99, -6 },//1
	{ -3, 99, -6 },//2
	{ -3, 99, -5 },//3
	{ -3, 90, -5 },//4
	{ -2, 80, -4 },//5
	{ -2, 70, -4 },//6
	{ -2, 60, -3 },//7
	{ -1, 50, -3 },//8
	{ -1, 40, -2 },//9
	{ -1, 30, -1 },//10
	{ -1, 20, 0 },//11
	{ 0,  12, 0 },//12
	{ 0,  8, 0 },//13
	{ 0,  5, 0 },//14
	{ 0,  4, 0 },//15
	{ 1,  3, 1 },//16
	{ 1,  2, 1 },//17
	{ 2,  1, 2 },//18
	{ 2,  1, 2 },//19
	{ 2,  0, 3 },//20
	{ 3,  0, 3 },//21
	{ 3,  0, 4 },//22
	{ 4,  0, 4 },//23
	{ 4,  0, 5 },//24
	{ 4,  0, 5 },//25
	{ 5,  0, 6 },//26
	{ 5,  0, 6 },//27
	{ 6,  0, 7 }, //28
	{ 6,  0, 7 },
	{ 7,  0, 7 },
	{ 7,  0, 8 },
	{ 8,  0, 8 },
	{ 8,  0, 8 } /*33*/
};

const	struct	dex_app_type	dex_app		[34]		=
{//modyfikator klasy pancerza
	{   60 },//00
	{   55 },//01
	{   50 },//02
	{   45 },//03
	{   40 },//04
	{   35 },//05
	{   30 },//06
	{   25 },//07
	{   20 },//08
	{   15 },//09
	{   10 },//10
	{    5 },//11
	{    0 },//12
	{    0 },//13
	{    0 },//14
	{    0 },//15
	{    0 },//16
	{ -  5 },//17
	{ - 10 },//18
	{ - 15 },//19
	{ - 20 },//20
	{ - 25 },//21
	{ - 30 },//22
	{ - 35 },//23
	{ - 40 },//24
	{ - 45 },//25
	{ - 50 },//26
	{ - 55 },//27
	{ - 60 }, //28
	{ - 64 },
	{ - 67 },//30
	{ - 69 },
	{ - 71 },//32
	{ - 72 } //33
};

const	struct	con_app_type	con_app		[34]		=
{//mod do hp dla zbrojnych(woj,pal,barb,mnich), mod do hp dla reszty, odwrotnosc szansy % na daze_state przy 'to naprawde bolalo!'
	{ -3,-3, 20 },//0
	{ -3,-3, 25 },//1
	{ -3,-3, 30 },//2
	{ -3,-3, 35 },//3
	{ -3,-3, 40 },//4
	{ -2,-2, 45 },//5
	{ -2,-2, 50 },//6
	{ -2,-2, 55 },//7
	{ -1,-1, 60 },//8
	{ -1,-1, 65 },//9
	{  0,0, 70 },//10
	{  0,0, 75 },//11
	{  0,0, 80 },//12
	{  0,0, 85 },//13
	{  0,0, 88 },//14
	{  1,1, 90 },//15
	{  1,1, 95 },//16
	{  1,1, 97 },//17
	{  2,2, 99 },//18
	{  2,2, 99 },//19
	{  2,2, 99 },//20
	{  3,2, 99 },//21
	{  3,2, 99 },//22
	{  3,2, 99 },//23
	{  4,3, 99 },//24
	{  4,3, 99 },//25
	{  5,3, 99 },//26
	{  5,3, 99 },//27
	{  6,3, 100 }, //28
	{  7,4, 100 },
	{  8,4, 100 },
	{  9,5, 100 },
	{  10,5, 100 },
	{  11,5, 100 } //33


};

/*
 * Liquid properties.
 * Used in world.obj.
 */
const	struct	liq_type	liq_table	[]	=
{
/*
	liq_name,
	liq_name2,
	liq_name3,
	liq_name4,
	liq_name5,
	liq_name6,
	liq_color,
	(proof, full, thirst, food, size)
*/
// 0
	{"woda", "wody", "wodzie", "wodê", "wod±", "wodzie", "przezroczysty", {0, 0,  3, 0,  3}},
	{"piwo", "piwa", "piwu", "piwo", "piwem", "piwie", "piwny", {1, 0,  1, 1,  2}},
	{"czerwone wino", "czerwonego wina", "czerwonemu winu", "czerwone wino", "czerwonym winem", "czerwonym winie", "czerwony", {1, 0,  1, 0,  2}},
	{"jasne piwo", "jasnego piwa", "jasnemu piwu", "jasne piwo", "jasnym piwem", "jasnym piwie", "z³ocisty", {1, 0,  1, 0,  2}},
	{"ciemne piwo", "ciemnego piwa", "ciemnemu piwu", "ciemne piwo", "ciemnym piwem", "ciemnym piwie", "ciemnobr±zowy", {2, 0,  1, 1,  2}},
// 5
	{"whisky", "whisky", "whisky", "whisky", "whisky", "whisky", "br±zowo-z³oty", {3, 0,  1, 0,  2}},
	{"lemoniada", "lemoniady", "lemoniadzie", "lemoniadê", "lemoniad±", "lemoniadzie", "owocowy", {0, 0,  2, 0,  3}},
	{"gard³ogrzmot", "gard³ogrzmota", "gard³ogrzmotowi", "gard³ogrzmot", "gard³ogrzmotem", "gard³ogrzmocie", "bulgocz±cy", {4, 0,  1, 0,  2}},
	{"lokalny specja³", "lokalnego specja³u", "lokalnemu specja³owi", "lokalny specja³", "lokalnym specja³em", "lokalnym specjale", "przezroczysty", {4, 0,  1, 0,  2}},
	{"sok", "soku", "sokowi", "sok", "sokiem", "soku", "zielony", {0, 0,  1, 1,  3}},
// 10
	{"mleko", "mleka", "mleku", "mleko", "mlekiem", "mleku", "bia³y", {0, 0,  2, 1,  3}},
	{"herbata", "herbaty", "herbacie", "herbatê", "herbat±", "herbacie", "br±zowawy", {0, 0,  2, 0,  4}},
	{"kawa", "kawy", "kawie", "kawê", "kaw±", "kawie", "czarny", {0, 0,  1, 0,  4}},
	{"krew", "krwi", "krwi", "krew", "krwi±", "krwi", "szkar³atny", {0, 0,  0, 1,  4}},
	{"s³ona woda", "s³onej wody", "s³onej wodzie", "s³on± wodê", "s³on± wod±", "s³onej wodzie", "przezroczysty", {0, 0, -1, 0,  1}},
// 15
	{"piwo imbirowe", "piwa imbirowego", "piwu imbirowemu", "piwo imbirowe", "piwem imbirowym", "piwie imbirowym", "br±zowy", {2, 0,  1, 1,  3}},
	{"piwo korzenne", "piwa korzennego", "piwu korzennemu", "piwo korzenne", "piwem korzennym", "piwie korzennym", "br±zowy", {2, 0,  1, 1,  3}},
	{"elfie wino", "elfiego wina", "elfiemu winu", "elfie wino", "elfim winem", "elfim winie", "zielony", {2, 0,  2, 0,  4}},
	{"bia³e wino", "bia³ego wina", "bia³emu winu", "bia³e wino", "bia³ym winem", "bia³ym winie", "z³oty", {2, 0,  2, 0,  4}},
	{"wino musuj±ce", "wina musuj±cego", "winu musuj±cemu", "wino musuj±ce", "winem musuj±cym", "winie musuj±cym", "z³otawy", {1, 0,  1, 0,  4}},
// 20
	{"miód pitny", "miodu pitnego", "miodowi pitnemu", "miód pitny", "miodem pitnym", "miodzie pitnym", "miodowy", {3, 0,  2, 1,  2}},
	{"wino ró¿ane", "wina ró¿anego", "winu ró¿anemu", "wino ró¿ane", "winem ró¿anym", "winie ró¿anym", "ró¿owe", {2, 0,  2, 0,  3}},
	{"wino sileañskie", "wina sileañskiego", "winu silañskiemu", "wino sileañskie", "winem sileañskim", "winie sileañskim", "czerwony", {2, 0,  2, 0,  3}},
	{"wódka", "wódki", "wódce", "wódkê", "wódk±", "wódce", "przezroczysty", {4, 0,  1, 0,  2}},
	{"sok porzeczkowy", "soku porzeczkowego", "sokowi porzeczkowemu", "sok porzeczkowy", "sokiem porzeczkowym", "soku porzeczkowym", "czerwony", {0, 0,  3, 1,  2}},
// 25
	{"sok pomarañczowy", "soku pomarañczowego", "sokowi pomarañczowemu", "sok pomarañczowy", "sokiem pomarañczowym", "soku pomarañczowym", "pomarañczowy", {0, 0,  3, 1,  4}},
	{"spirytus", "spirytusu", "spirytusowi", "spirytus", "spirytusem", "spirytusie", "przezroczysty", {5, 0,  1, 0,  2}},
	{"brandy", "brandy", "brandy", "brandy", "brandy", "brandy", "z³oty", {3, 0,  1, 0,  2}},
	{"wódka zio³owa", "wódki zio³owej", "wódce zio³owej", "wódkê zio³ow±", "wódk± zio³ow±", "wódce zio³owej", "przezroczysty", {4, 0,  1, 0,  2}},
	{"woda entów", "wody entów", "wodzie entów", "wodê entów", "wod± entów", "wodzie entów", "przezroczysty", {0, 0,  4, 2,  2}},
// 30
	{"wino lodowe", "wina lodowego", "winu lodowemu", "wino lodowe", "winem lodowym", "winie lodowym", "purpurowe", {2, 0,  2, 0,  2}},
	{"¶liwowica", "¶liwowicy", "¶liwowicy", "¶liwowicê", "¶liwowic±", "¶liwowicy", "przezroczysty", {5, 0,  1, 0,  2}},
	{"wi¶niówka", "wi¶niówki", "wi¶niówce", "wi¶nówkê", "wi¶niówk±", "wi¶niówce", "czerwony", {3, 0,  1, 0,  2}},
	{"wino jab³kowe", "wina jab³kowego", "winu jab³kowemu", "wino jab³kowe", "winem jab³kowym", "winie jab³kowym", "¿ó³tawy", {2, 0,  1, 0,  2}},
	{"rum", "rumu", "rumowi", "rum", "rumem", "rumie", "jasnobr±zowy", {4, 0,  1, 0,  2}},
// 35
	{"mocne wino owocowe", "mocnego wina owocowego", "mocnemu winu owocowemu", "mocne wino owocowe", "mocnym winem owocowym", "mocnym winie owocowym", "mêtny czerwonawy", {3, 0,  2, 1,  3}},
	{"samogon", "samogonu", "samogonowi", "samogon", "samogonem", "samogonie", "przezroczysty", {5, 0,  0, 0,  1}},
	{"absynt", "absyntu", "absyntowi", "absynt", "absyntem", "absyncie", "przezroczysty", {4, 0,  0, 0,  1}},
	{"b³oto", "b³ota", "b³otu", "b³oto", "b³otem", "b³ocie", "b³otnisty", {0, 0,  1, 2,  3}},
	{"sok brzozowy", "soku brzozowego", "sokowi brzozowemu", "sok brzozowy", "sokiem brzozowym", "soku brzozowym", "s³omkowy", {0, 0, 3, 0, 3}},
// 40
	{"kompot", "kompotu", "kompotowi", "kompot", "kompotem", "kompocie", "s³omkowy", {0, 0,  3, 1,  4}},
	{"dziegieæ", "dziegciu", "dziegciowi", "dziegieæ", "dziegciem", "dziegciu", "mêtnobr±zowawy", {0, 0,  3, 0,  3}},
	{"uzdrawiaj±ca woda", "uzdrawiaj±cej wody", "uzdrawiaj±cej wodzie", "uzdrawiaj±c± wodê", "uzdrawiaj±c± wod±", "uzdrawiaj±cej wodzie", "przezroczysty", {0, 0,  3, 0,  3}},
	{"kwas", "kwasu", "kwasowi", "kwas", "kwasem", "kwasie", "zielony", {0, 0,  0, 0, 1}},

// uwaga dodaj±c p³yn proszê zmieniæ maksymaln± warto¶æ losowan± w:
// spells_mag.c : spell_transmute_liquid
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, {   0, 0,  0, 0,  0 }}
};



/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n)	n


const struct  ShieldType ShieldList[] =
{
	{	0,		"puklerz",				2,			18,			0	},
	{	1,		"lekka tarcza",			3,			25,			5	},
	{	2,		"srednia tarcza",		4,			30,			8	},
	{	3,		"ciezka tarcza",		5,			35,			10	},
	{	4,		"pawez",				6,			45,			15	},
	{	0,		NULL,					0,			0,			0	}
};

/*
ziolka (herbs)
*/
const struct herb_type herb_table[]=
{
/* przykladowe
      {"laicka nazwa","laicki dopelniacz","laicki celownik","laicki biernik","laicki narzednik","laicki miejscownik",
      "nazwa","dopelniacz","celownik","biernik","narzednik","miejscownik",
      "opis wygladu",
      cena liczbowo, "jaka czesc rosliny PLANT_PART_", trudnosc liczbowo, trucizna (9 jesli nie),
      {0,0,0,0,0, //to prawdopodobienstwa w sectorach, ida po kolei jak w sector_table
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0},    //jest 45 czyli na zapas...
       efekt jedzenia liczbowo(czyli numer), "opis smaku",
       efekt wypalenia liczbowo(czyli numer), "opis wypalenia"
      }
*/
      {"unknown","unknown","unknown","unknown","unknown","unknown",
      "unknown","unknown","unknown","unknown","unknown","unknown",
      "unknown",
      0, 0, 0, 10,
      {0,0,0,0,0, //to prawdopodobienstwa w sectorach, ida po kolei jak w sector_table
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,
       0,0,0,0,0,0,0,0,0,0},    //jest 45 czyli na zapas...
       0, "unknown",
       0, "unknown"
      },
      {"ogromna zielona bylina", "ogromnej zielonej byliny", "ogromnej zielonej bylinie", "ogromn± zielon± bylinê", "ogromn± zielon± bylin±", "ogromnej zielonej bylinie",
	     "arcydziêgiel litwor", "arcydziêgiel litwor", "arcydziêgiel litwor", "arcydziêgiel litwor", "arcydziêgiel litwor", "arcydziêgiel litwor",
	     "Ogromna zielona bylina s³u¿±ca do przygotowywania naparów lecz±cych.",
	     200, PLANT_PART_PERENNIAL, 2, 9,
	     {0, 0, 5, 10, 10,	      15, 0, 0, 0, 0,	      0, 15, 35, 5, 0,	      0, 0, 0, 0, 0,	      0, 0, 0, 0, 0,	      0, 0, 0, 0, 0,	      100, 0, 10, 0, 0,	      5, 0, 0, 0, 0,	      0, 0, 20, 0, 0,0,0,0,0,0},          //jest 45 czyli na zapas...
	     0, "",
	     0, ""
	    },

	    {"du¿y zó³ty kwiatek", "du¿ego ¿ó³tego kwiatka", "du¿emu ¿ó³temu kwiatkowi", "du¿y ¿ó³ty kwiatek", "du¿ym ¿ó³tym kwiatkiem", "du¿ym ¿ó³tym kwiatku",
	     "arnika górska", "arniki górskiej", "arnice górskiej", "arnikê górsk±", "arnik± górsk±", "arnice górskiej",
	     "Du¿y ¿ó³ty kwiatek o w³a¶ciwo¶ciach leczniczych.",
	     1200, PLANT_PART_FLOWER, 21, 9,
	     {0, 0, 0, 0, 10,
	      25, 0, 0, 0, 0,
	      0, 0, 0, 35, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 0, 0, 0,
	      15, 0, 0, 0, 0,
	      0, 0, 5, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_POISON_LEVEL20, "opis smaku",
	     0, "opis wypalenia"
	    },

	    {"zielone pod³u¿ne li¶cie", "zielonych pod³u¿nych li¶ci", "zielonym pod³u¿nym li¶ciom", "zielone pod³u¿ne li¶cie", "zielonymi pod³u¿nymi li¶æmi", "zielonych pod³u¿nych li¶ciach",
	     "babka lancetowata", "babki lancetowatej", "babce lancetowatej", "babkê lancetowat±", "babk± lancetowat±", "babce lancetowatej",
	     "Zielone pod³u¿ne li¶cie, od wieków wykorzystywane w medycynie.",
	     60, PLANT_PART_LEAF, 1, 9,
	     {0, 5, 50, 5, 5,	      0, 0, 0, 0, 0,	      0, 10, 0, 0, 0,	      0, 0, 0, 25, 0,	      0, 0, 0, 0, 0,	      0, 0, 0, 35, 0,	      100, 0, 60, 10, 0,	      0, 2, 0, 20, 0,	      0, 0, 40, 0, 0,0,0,0,0,0},
		PLANT_EFFECT_SMALL_HP, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"malutkie okr±g³e li¶cie", "malutkich okr±g³ych li¶ci", "malutkim okr±g³ym li¶ciom", "malutkie okr±g³e li¶cie", "malutkimi okr±g³ymi li¶æmi", "malutkich okr±g³ych li¶ciach",
	     "bazylia wonna", "bazylii wonnej", "bazylii wonnej", "bazyliê wonn±", "bazyli± wonn±", "bazylii wonnej",
	     "Malutkie okr±g³e li¶cie cenione jako przyprawa, do np. zup.",
	     85, PLANT_PART_LEAF, 31, 9,
	     {0, 0, 40, 10, 0,	      0, 0, 0, 0, 0,	      0, 20, 0, 0, 0,      0, 0, 0, 0, 0,	      0, 0, 0, 0, 0,	      0, 0, 0, 0, 0,	      100, 0, 10, 0, 0,	      0, 0, 0, 5, 0,0, 0, 10, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"malutkie pod³u¿ne czerwone jagody", "malutkich pod³u¿nych czerwonych jagód", "malutkim pod³u¿nym czerwonym jagodom", "malutkie pod³u¿ne czerwone jagody", "malutkimi pod³u¿nymi czerwonymi jagodami", "malutkich pod³u¿nych czerwonych jagodach",
	     "berberys zwyczajny", "berberysu zwyczajnego", "berberysowi zwyczajnemu", "berberys zwyczajny", "berberysem zwyczajnym", "berberysie zwyczajnym",
	     "Malutkie pod³u¿ne czerwone jagody, czasami jadalne i u¿ywane jako przyprawa, jednak wie¶æ niesie, ¿e s± one magazynowane przez przeró¿nych magów.",
	     85, PLANT_PART_BERRYS, 15, 9,
	     {0, 0, 0, 30, 0,	      0, 0, 0, 0, 0,	      0, 20, 30, 0, 0,	      0, 0, 0, 0, 0,	      0, 0, 0, 0, 0,	      0, 0, 0, 0, 0,	      100, 0, 5, 0, 0,	      0, 0, 0, 10, 0,	      0, 0, 5, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"malutkie okr±g³e czarne owoce", "malutkich okr±g³ych czarnych owoców", "malutkim okr±g³ym czarnym owocom", "malutkie okr±g³e czarne owoce", "malutkimi okr±g³ymi czarnymi owocami", "malutkich okr±g³ych czarnych owocach",
	     "bez czarny", "bzu czarnego", "bzowi czarnemu", "bez czarny", "bezem czarnym", "bzie czarnym",
	     "Malutkie czarne owoce, delikatnie l¶ni±ce i bardzo miekkie, wydzielaj±ce specyficzny zapach.",
	     60, PLANT_PART_FRUITS, 8, 9,
	     {0, 0, 20, 60, 10,
	      5, 0, 0, 0, 0,
	      0, 50, 0, 0, 0,
	      0, 0, 0, 10, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 10, 0,
	      100, 5, 5, 0, 0,
	      0, 0, 0, 40, 0,
	      0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"roz³o¿yste zielone k³±cze", "roz³o¿ystego zielonego k³±cza", "roz³o¿ystemu zielonemu k³±czu", "roz³o¿yste zielone k³±cze", "roz³o¿ystym zielonym k³±czem", "roz³o¿ystym zielonym k³±czu",
	     "boberek trójlistkowy", "boberka trójlistkowego", "boberkowi trójlistkowemu", "boberek trójlistkowy", "boberkiem trójlistkowym", "boberku trójlistkowym",
	     "Malutkie, trójlistkowe ziele, zwane te¿ koniczyn±, czy kozio³kiem, s³ynne z przes±dów o szczê¶ciu, jakie przynosi znalezienie boberka... z czterema listkami.",
	     90, PLANT_PART_STALK, 5, 9,
	     {0, 0, 0, 5, 0,
	      0, 10, 10, 0, 0,
	      0, 10, 30, 0, 0,
	      0, 0, 0, 0, 5,
	      15, 0, 0, 0, 0,
	      0, 0, 0, 0, 15,
	      100, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"gar¶æ ma³ych czerwonych jagód", "gar¶ci ma³ych czerwonych jagód", "gar¶ci ma³ych czerwonych jagód", "gar¶æ ma³ych czerwonych jagód", "gar¶ci± ma³ych czerwonych jagód", "gar¶ci ma³ych czerwonych jagód",
	     "borówka czerwona", "borówki czerwonej", "borówce czerwonej", "borówkê czerwon±", "borówk± czerwon±", "borówce czerwonej",
	     "Owoce czerwone, kuliste jak jagody, cierpkie, gorzkawe i s³odkawo-kwa¶ne. Nadaj± siê do spo¿ycia. Sok z zielonych jagód, zmieszany z wod±, jest doskona³ym napojem od¶wie¿aj±cym.",
	     65, PLANT_PART_BERRYS, 2, 9,
	     {0, 0, 5, 45, 20,
	      0, 0, 0, 0, 0,
	      0, 30, 20, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 5, 5, 0,
	      0, 0, 0, 10, 0,
	      0, 0, 10, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"gar¶æ ma³ych czarnych jagód", "gar¶ci ma³ych czarnych jagód", "gar¶ci ma³ych czarnych jagód", "gar¶æ ma³ych czarnych jagód", "gar¶ci± ma³ych czarnych jagód", "gar¶ci ma³ych czarnych jagód",
	     "borówka czarna", "borówki czarnej", "borówce czarnej", "borówkê czarn±", "borówk± czarn±", "borówce czarnej",
	     "Czarne owoce borówki s± kwaskowate, s³odkawe i trochê cierpkie. W medycynie ludowej jagody czarne s± znanym ¶rodkiem przeciwbiegunkowym.",
	     70, PLANT_PART_BERRYS, 6, 9,
	     {0, 0, 5, 55, 20,
	      0, 0, 0, 0, 0,
	      0, 40, 25, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 5, 5, 0,
	      0, 0, 0, 10, 0,
	      0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"niewielki krzaczek z lancetowatymi li¶ciami", "niewielkiego krzaczka z lancetowatymi li¶ciami", "niewielkiemu krzeczkowi z lancetowatymi li¶ciami", "niewielki krzeczek z lancetowatymi li¶ciami", "niewielkim krzeczkiem z lancetowatymi li¶ciami", "niewielkim krzeczku z lancetowatymi li¶ciami",
	     "bylica estragon", "bylicy estragon", "bylicy estragon", "bylicê estragon", "bylic± estragon", "bylicy estragon",
	     "Krzaczek ten ma ³odygi proste, na wpó³ zdrewnia³e, gór± rozga³êziaj±ce siê i przechodz±ce w cienkie nibyk³osy i maleñkie grona kwiatów. Li¶cie pod³u¿ne, lancetowate, do¶æ liczne. Kwiaty maleñkie, prawie kuliste, zielonkawo¿ó³te.",
	     180, PLANT_PART_BUSH, 35, 9,
	     {0, 0, 50, 10, 10,
	      0, 0, 0, 0, 0,
	      0, 5, 0, 0, 0,
	      0, 0, 0, 10, 5,
	      10, 0, 0, 0, 0,
	      0, 0, 0, 40, 0,
	      100, 0, 20, 0, 0,
	      0, 0, 0, 10, 0,
	      0, 0, 40, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "opis smaku",
	     0, "opis wypalenia"
	    },
	    {"du¿y szarozielony krzaczek", "du¿ego szarozielonego krzaczka", "du¿emu szarozielonemu krzaczkowi", "du¿ego szarozielonego krzaczka", "du¿ym szarozielonym krzaczkiem", "du¿ym szarozielonym krzaczku",
	     "bylica pospolita", "bylicy pospolitej", "bylicy pospolitej", "bylicê pospolit±", "bylic± pospolit±", "bylicy pospolitej",
	     "Jest pó³krzewem o g³ównym pêdzie pó³zdrewnia³ym, rozga³êziaj±cym siê. Li¶cie pierzastodzielne, coraz wy¿ej jakby zanikaj±ce, z wierzchu ciemnozielone, spodem pokryte we³nistym bia³ym meszkiem skierowanym w po³udnie ku s³oñcu.",
	     45, PLANT_PART_BUSH, 20, 9,
	     {0, 0, 70, 25, 15,
	      10, 0, 0, 0, 0,
	      0, 20, 40, 5, 0,
	      0, 0, 0, 20, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 15, 0,
	      100, 10, 60, 40, 0,
	      10, 0, 0, 5, 0,
	      0, 0, 80, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"kilka niewielkich czerwonych kwiatków", "kilku niewielkich czerwonych kwiatków", "kilku niewielkim czerwonym kwiatkom", "kilka niewielkich czerwonych kwiatków", "kilkoma niewielkimi czerwonymi kwiatkami", "kilku niewielkich czerwonych kwiatkach",
	     "centuria pospolita", "centurii pospolitej", "centurii pospolitej", "centuriê pospolit±", "centuri± pospolit±", "centurii pospolitej",
	     "£odygi w dolnej czê¶ci tej ro¶liny s± sze¶ciok±tne, czê¶ciowo puste, a w górnej czê¶ci czterokrotne, w obrêbie kwiatostanów rozga³êzione. Li¶cie, pod³u¿ne, u³o¿one naprzemian³egle, parami. Kwiaty ró¿owoczerwone.",
	     120, PLANT_PART_FLOWER, 45, 9,
	     {0, 0, 30, 20, 0,
	      0, 0, 0, 0, 0,
	      10, 10, 5, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 20,
	      100, 25, 15, 20, 0,
	      0, 0, 0, 10, 0,
	      25, 0, 30, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"kilka niewielkich niebieskich kwiatków", "kilku niewielkich niebieskich kwiatków", "kilku niewielkim niebieskim kwiatkom", "kilka niewielkich niebieskich kwiatków", "kilkoma niewielkimi niebieskimi kwiatkami", "kilku niewielkich niebieskich kwiatkach",
	     "chaber b³awatek", "chabru b³awatka", "chabrowi b³awatkowi", "chaber b³awatek", "chabrem b³awatkiem", "chabrze b³awatku",
	     "Pospolity kwiat-chwast zbo¿owy. Ro¶nie tak¿e na miedzach i nieu¿ytkach, wystêpuje pojedynczo. Znany ze swoich w³a¶ciwo¶ci barwi±cych.",
	     45, PLANT_PART_FLOWER, 2, 9,
	     {0, 0, 90, 5, 25,
	      5, 0, 0, 0, 0,
	      0, 5, 0, 0, 0,
	      0, 0, 0, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 20, 0,
	      100, 5, 60, 30, 0,
	      0, 0, 0, 15, 0,
	      0, 0, 80, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"gruby bia³y korzeñ", "grubego bia³ego korzenia", "grubemu bia³emu korzeniowi", "gruby bia³y korzeñ", "grubym bia³ym korzeniem", "grubym bia³ym korzeniu",
	     "chrzan pospolity", "chrzanu pospolitego", "chrzanowi pospolitemu", "chrzan pospolity", "chrzanem pospolitym", "chrzanie pospolitym",
	     "Jest to poka¼na ro¶lina, o grubej ³odydze, czêsto zdrewnia³ej u spodu. Posiada ogromne li¶cie. S³ynna z zastosowania jako przyprawa, po uprzednim odpowiednim przygotowaniu.",
	     90, PLANT_PART_ROOT, 35, 9,
	     {0, 0, 30, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 5, 20, 0, 0,
	      0, 0, 0, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 5, 0,
	      100, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SM_SF, "unknown",
	     0, "unknown"
	    },
	    {"pomarszczony jasnobrunatny korzeñ", "pomarszczonego jasnobrunatnego korzenia", "pomarszczonemu jasnobrunatnemu korzeniowi", "pomarszczony jasnobrunatny korzeñ", "pomarszczonym jasnobrunatnym korzeniem", "pomarszczonym jasnobrunatnym korzeniu",
	     "cykoria podró¿nik", "cykorii podró¿nik", "cykorii podró¿nik", "cykoriê podró¿nik", "cykori± podró¿nik", "cykorii podró¿nik",
	     "Dolne li¶cie tej nietypowej ro¶liny zebrane s± w swoiste rozety, pierzaste. Górne natomiast - lancetowate. Kwiaty najczê¶niej b³êkitne. Niektórzy mówi±, ¿e stanowi ona sk³adnik piwa... jako bawnik.",
	     25, PLANT_PART_ROOT, 45, 9,
	     {0, 0, 20, 10, 0,
	      0, 0, 0, 0, 0,
	      0, 5, 0, 0, 0,
	      0, 0, 0, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 10, 0,
	      100, 0, 20, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 30, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "unknown",
	     0, "unknown"
	    },
	    {"ma³y silnie rozga³êziony krzak", "ma³ego silnie rozga³êzionego krzaka", "ma³emu silnie rozga³êzionemu krzakowi", "ma³y silnie rozga³êziony krzak", "ma³ym silnie rozga³êzionym krzakiem", "ma³ym silnie rozga³êzionym krzaku",
	     "cz±ber ogrodowy", "cz±bru ogrodowego", "cz±browi ogrodowemu", "cz±ber ogrodowy", "cz±brem ogrodowym", "cz±brze ogrodowym",
	     "Silnie rozga³êziona, du¿a, krzaczasta ro¶lina. Li¶cie ma w±skie, lancetowate. Kwiaty najczê¶ciej pojedyncze, ma³e, bia³e, fio³kowe lub czerwonawe, wzrastaj±ce w k±tach li¶ci.",
	     195, PLANT_PART_BUSH, 24, 9,
	     {0, 0, 30, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 10, 0,
	      100, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_CURE_POISON, "unknown",
	     0, "unknown"
	    },
	    {"ma³a bia³a cuchn±ca cebulka", "ma³ej bia³ej cuchn±cej cebulki", "ma³ej bia³ej cuchn±cej cebulce", "ma³± bia³± cuchn±c± cebulkê", "ma³± bia³± cuchn±c± cebulk±", "ma³ej bia³ej cuchn±cej cebulce",
	     "czosnek pospolity", "czosnku pospolitego", "czosnkowi pospolitemu", "czosnek pospolity", "czosnkiem pospolitym", "czosnku pospolitym",
	     "Niewielka cebulka wydzielaj±ca charakterystyczny zapach. Znana z potê¿nych w³a¶ciow¶ci leczniczych. Niezast±piony ¶rodek przeciw wszelakim w±pierzom.",
	     40, PLANT_PART_BULB, 4, 9,
	     {0, 0, 50, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "unknown",
	     0, "unknown"
	    },
	    {"kolba z³ocisto¿ó³tych kwiatów", "kolby z³ocisto¿ó³tych kwiatów", "kolbie z³ocisto¿ó³tych kwiatów", "kolbê z³ocisto¿ó³tych kwiatów", "kolb± z³ocisto¿ó³tych kwiatów", "kolbie z³ocisto¿ó³tych kwiatów",
	     "dziewanna wielokwiatowa", "dziewanny wielokwiatowej", "dziewannie wielokwiatowej", "dziewannê wielokwiatow±", "dziewann± wielokwiatow±", "dziewannie wielokwiatowej",
	     "Piêkne, z³ocisto¿ó³te kwiatki o przyjemnym zapachu. Zwana tak¿e ludowo gorzygrotem lub szabl±. Podobno mo¿na z niej zrobiæ smaczny napar.",
	     160, PLANT_PART_FLOWER, 55, 9,
	     {0, 0, 20, 5, 35,
	      5, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 5, 0, 0,
	      0, 0, 0, 5, 0,
	      0, 0, 40, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"gar¶æ ¿ó³tych kwiatków", "gar¶ci ¿ó³tych kwiatków", "gar¶ci ¿ó³tych kwiatków", "gar¶æ ¿ó³tych kwiatków", "gar¶ci± ¿ó³tych kwiatków", "gar¶ci ¿ó³tych kwiatków",
	     "dziurawiec lekarski", "dziurawca lekarskiego", "dziurawcowi lekarskiemu", "dziurawiec lekarski", "dziurawcem lekarskim", "dziurawcu lekarskim",
	     "Niedu¿y, charakterystyczy pó³krzew. Kwiaty ¿ó³te, zebrane na szczytach pêdów w niby-baldachy. Li¶æ dziurawca - ogl±dany pod ¶wiat³o - sprawia wra¿enie podziurawionego szpilk±. W rzeczywisto¶ci te niby maleñkie otworki to zbiorniczki olejku - st±d nazwa tego ziela",
	     220, PLANT_PART_FLOWER, 10, 9,
	     {0, 0, 30, 40, 80,
	      60, 0, 0, 0, 0,
	      0, 15, 10, 30, 0,
	      0, 0, 0, 30, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 50, 0,
	      100, 0, 15, 10, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 70, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "unknown",
	     0, "unknown"
	    },
	    {"malutki fioletowo¿ó³ty kwiatek", "malutkiego fioletowo¿ó³tego kwiatka", "malutkiemu fioletowo¿ó³temu kwiatkowi", "malutki fioletowo¿ó³ty kwiatek", "malutkim fioletowo¿ó³tym kwiatkiem", "malutkim fioletowo¿ó³tym kwiatku",
	     "fio³ek trójbarwny", "fio³ka trójbarwnego", "fio³kowi trójbarwnemu", "fio³ek trójbarwny", "fio³kiem trójbarwnym", "fio³ku trójbarwnym",
	     "Nie myliæ tego fio³ka z fio³kiem polnym, którego kwiaty s± mniejsze, a p³atki korony kwiatu nie s± wiêksze od kielicha. Maj± te¿ inn± barwê. Kwiaty trójbarwne, górne p³atki fio³kowe, boczne ja¶niejsze lub bia³e, a dolna warga ¿ó³ta.",
	     150, PLANT_PART_FLOWER, 10, 9,
	     {0, 0, 30, 30, 40,
	      20, 0, 0, 0, 0,
	      0, 10, 0, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 20, 0, 0,
	      0, 0, 0, 60, 0,
	      0, 0, 70, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"malutkie ¿ó³tawe kuleczki", "malutkich ¿ó³tawych kuleczek", "malutkim ¿ó³tawym kuleczkom", "malutkie ¿ó³tawe kuleczki", "malutkimi ¿ó³tawymi kuleczkami", "malutkich ¿ó³tawych kuleczkach",
	     "gorczyca bia³a", "gorczycy bia³ej", "gorczycy bia³ej", "gorczycê bia³±", "gorczyc± bia³±", "gorczycy bia³ej",
	     "Ro¶lina ta jest znana i stosowana od dawna. Ma jasno¿ó³te, lekko pikantne nasiona, które s³u¿± do aromatyzowania ryb, miêsa, wêdlin, sosów itp. ¦wie¿e m³ode li¶cie gorczycy s± spo¿ywane niekiedy jako dodatek do sa³aty i kanapek.",
	     45, PLANT_PART_FRUITS, 20, 9,
	     {0, 0, 70, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 60, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 50, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"gruby wrzecionowaty korzeñ", "grubego wrzecionowatego korzenia", "grubemu wrzecionowatemu korzeniowi", "gruby wrzecionowaty korzeñ", "grubym wrzecionowatym korzeniem", "grubym wrzecionowatym korzeniu",
	     "gorczyca zó³ta", "gorczycy ¿ó³tej", "gorczycy ¿ó³tej", "gorczycê ¿ó³t±", "gorczyc± ¿ó³t±", "gorczycy ¿ó³tej",
	     "Du¿y, dziwny, wrzecionowaty korzeñ o bardzo intensywnym, ale przyjemnym zapachu. Wykorzystywany do preparowania ostrych przypraw przez ludno¶æ niektórych wsi.",
	     150, PLANT_PART_ROOT, 30, 9,
	     {0, 0, 0, 0, 30,
	      50, 0, 0, 0, 0,
	      0, 0, 70, 10, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 60, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "unknown",
	     0, "unknown"
	    },
	    {"niewielka fioletowo ukwiecona bylina", "niewielkiej fioletowo ukwieconej bylinie", "niewielkiej fioletowo ukwieconej bylinie", "niewielk± fioletowo ukwiecon± bylinê", "niewielk± fioletowo ukwiecon± bylin±", "niewielkiej fioletowo ukwieconej bylinie",
	     "hyzop lekarski", "hyzopu lekarskiego", "hyzopowi lekarskiemu", "hyzop lekarski", "hyzopem lekarskim", "hyzopie lekarskim",
	     "Ta ro¶lina jest pó³krzewem ga³êzistym o ³odygach u do³u zdrewnia³ych i gêsto ulistnionych. Kwiaty posiada ciemnob³êkitnofio³kowe, rzadziej bia³e lub ró¿owe. Jej wielkim walorem jest cudny, aromatyczny zapach.",
	     500, PLANT_PART_PERENNIAL, 70, 9,
	     {0, 0, 0, 0, 60,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 50, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_CURE_POISON, "unknown",
	     0, "unknown"
	    },
	    {"ma³a twarda ciemnogranatowa jagoda", "ma³ej twardej ciemnogranatowej jagody", "ma³ej twardej ciemnogranatowej jagodzie", "ma³± tward± ciemnogranatow± jagodê", "ma³± tward± ciemnogranatow± jagod±", "ma³ej twardej ciemnogranatowej jagodzie",
	     "ja³owiec pospolity", "ja³owca pospolitego", "ja³owcowi pospolitemu", "ja³owiec pospolity", "ja³owcem pospolitym", "ja³owcu pospolitym",
	     "Ogl±dasz wnikliwie ten wysmuk³y i wynios³y krzew iglasty. Pamiêtasz, ¿e ¿ywotno¶æ jego przekracza ¿ycie cz³owieka, chocia¿ jego przyrosty s± bardzo ma³e. Ro¶nie na ziemi ja³owej, piaszczystej, st±d jego nazwa, ale spotyka siê go te¿ czêsto na torfowiskach i wrzosowiskach. Do ga³±zki przytoczonych jest kilka du¿ych, twardych, ciemnogranatowych owoców, które wygl±daj± naprawdê ³adnie.",
	     70, PLANT_PART_BERRYS, 1, 9,
	     {0, 0, 0, 60, 40,
	      0, 0, 0, 0, 0,
	      0, 70, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 20, 40, 60, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 30, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"ma³e bia³e kwiatki", "ma³ych bia³ych kwiatków", "ma³ym bia³ym kwiatkom", "ma³e bia³e kwiatki", "ma³ymi bia³ymi kwiatkami", "ma³ych bia³ych kwiatkach",
	     "jasnota bia³a", "jasnoty bia³ej", "jasnocie bia³ej", "jasnotê bia³±", "jasnot± bia³±", "jasnocie bia³ej",
	     "Ta ciekawa ro¶lina jest niew±tpliwie jakim¶ chwastem. Przypomina trochê pokrzywê, szczególnie li¶cie. Ma du¿e, bia³e kwiaty, dziwnie ³adne w porównaniu z reszt± ro¶liny.",
	     10, PLANT_PART_FLOWER, 5, 9,
	     {0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 70, 0, 0,
	      0, 0, 0, 20, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 30, 0,
	      100, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 40, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"niedu¿e fioletowoczarne owoce", "niedu¿ych fioletowoczarnych owoców", "niedu¿ym fioletowoczarnym owocom", "niedu¿e fioletowoczarne owoce", "niedu¿ymi fioletowoczarnymi owocami", "niedu¿ych fioletowoczarnych owocach",
	     "jezyna pospolita", "je¿yny pospolitej", "je¿ynie pospolitej", "je¿ynê pospolit±", "je¿yn± pospolit±", "je¿ynie pospolitej",
	     "Te du¿e, fioletowoczarne owoce wygl±daj± podobnie jak owoce maliny. S± jednak ciemne i o wiele twardsze, w smaku natomiast lekko kwa¶ne, a nawet gorzkie. Jednak dojrza³e owoce bywaj± naprawdê miêkkie i s³odkie. Owoc ten jest czêsto przysmakiem wie¶niaków, s³u¿y tak¿e jako przyprawa.",
	     10, PLANT_PART_FRUITS, 2, 9,
	     {0, 0, 20, 50, 0,
	      0, 0, 0, 0, 0,
	      0, 10, 30, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 60, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SMALL_FOOD, "unknown",
	     0, "unknown"
	    },
	    {"gar¶æ ma³ych suchych nasionek", "gar¶ci ma³ych suchych nasionek", "gar¶ci ma³ych suchych nasionek", "gar¶æ ma³ych suchych nasionek", "gar¶ci± ma³ych suchych nasionek", "gar¶ci ma³ych suchych nasionek",
	     "kolendra siewna", "kolendry siewnej", "kolendrze siewnej", "kolendrê siewn±", "kolendr± siewn±", "kolendrze siewnej",
	     "Te nasionka, niewielkie i suche, wygl±daj± naprawdê niepozornie. Jednak wystarczy je rozgnie¶æ, czy rozetrzeæ, a zaczn± wydzielaæ delikatny, acz bardzo przyjemny zapach, podobny do miêty. Dziêki temu wykorzystywane s± jako sk³adniki aromatyczne i zapachowe.",
	     100, PLANT_PART_SEEDS, 25, 9,
	     {0, 0, 80, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 50, 30, 40, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 60, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"charakterystyczne d³oniaste li¶cie", "charakterystycznych d³oniastych li¶ci", "charakterystycznym d³oniastym li¶ciom", "charakterystyczne d³oniaste li¶cie", "charakterystycznymi d³oniastymi li¶cmi", "charakterystycznych d³oniastych li¶ciach",
	     "konopie", "konopi", "konopiom", "konopie", "konopiami", "konopiach",
	     "Te li¶cie wygl±daj± naprawdê ciekawe. Maj± kszta³t d³oni. Poza tym nic tej ro¶liny nie wyró¿nia spo¶ród wielu innych.",
	     115, PLANT_PART_LEAF, 15, 9,
	     {0, 0, 60, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 30, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 20, 50, 10, 0,
	      0, 0, 0, 10, 0,
	      0, 0, 80, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_HALLUCINATIONS, "unknown",
	     0, "unknown"
	    },
	    {"gar¶æ niewielkich sp³aszonych nasion", "gar¶ci niewielkich sp³aszonych nasion", "gar¶ci niewielkich sp³aszcznych nasion", "gar¶æ niewielkich sp³aszczonych nasion", "gar¶ci± niewielkich sp³aszczonych nasion", "gar¶ci niewielkich sp³aszczonych nasion",
	     "kozieradka pospolita", "kozieradki pospolitej", "kozieradce pospolitej", "kozieradkê pospolit±", "kozieradk± pospolit±", "kozieradce pospolitej",
	     "Niewielkie nasionka, przypominaj±ce kszta³tem fasolkê, czy groszek, musia³y znajdowaæ siê kiedy¶ w jakim¶ str±ku. S± niewielkich rozmiarów, owalne, br±zowe i dosyæ twarde.",
	     75, PLANT_PART_SEEDS, 7, 9,
	     {0, 0, 50, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 40, 10, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 70, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"liczne cienkie bia³e korzonki", "licznych cieñkich bia³ych korzonków", "licznym cienkim bia³ym korzonkom", "liczne cieñkie bia³e korzonki", "licznymi cienkimi bia³ymi korzonkami", "licznych cieñkich bia³ych korzonkach",
	     "koz³ek lekarski", "koz³ka lekarskiego", "koz³kowi lekarskiemu", "koz³ek lekarski", "koz³kiem lekarskim", "koz³ku lekarskim",
	     "Korzonki tej ro¶liny wygl±daj± bardzo ciekawie. Jakby to by³y dziesi±tki grybych, bia³awych nici. S± dosyæ miêkkie, jakby znajdowa³a siê w nich jaka¶ ciecz. S³ynne z w³a¶ciwo¶ci medycznych, po uprzednim ususzeniu.",
	     160, PLANT_PART_ROOT, 33, 9,
	     {0, 0, 50, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 40, 0, 0,
	      0, 0, 0, 0, 20,
	      30, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 70, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"bylina z pierzastosiecznymi li¶æmi", "byliny z pierzastosiecznymi li¶æmi", "bylinie z pierzastosiecznymi li¶æmi", "bylinê z pierzastosiecznymi li¶æmi", "bylin± z pierzastosiecznymi li¶æmi", "bylinie z pierzastosiecznymi li¶æmi",
	     "krwawnik pospolity", "krwawnika pospolitego", "krwawnikowi pospolitemu", "krwawnika pospolitego", "krwawnikiem pospolitym", "krwawniku pospolitym",
	     "Przygl±dasz siê tej ciekawej bylinie dok³adnie. Jest dosyæ wysoka, jednak w±t³a. £ody¿ki s± niezmiernie cienkie. A jednak jakim¶ sposobem utrzymuje du¿e kwiatostany zebrane w nibybaldachimy.",
	     500, PLANT_PART_LEAF, 60, 9,
	     {0, 0, 10, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 25, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 40, 0,
	      100, 0, 30, 5, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_STOP_BLEEDING, "unknown",
	     0, "unknown"
	    },
	    {"ma³y krzaczek wraz z korzeniem", "ma³ego krzaczka wraz z korzeniem", "ma³emu krzaczkowi wraz z korzeniem", "ma³y krzaczek wraz z korzeniem", "ma³ym krzacziem wraz z korzeniem", "ma³ym krzaczki wraz z korzeniem",
	     "lubczyk ogrodowy", "lubczyku ogrodowego", "lubczykowi ogrodowemu", "lubczyk ogrodowy", "lubczykiem ogrodowym", "lubczyku ogrodowym",
	     "Zwyk³a polna ro¶lina, o ciemno¿ó³tych kwiatach. Sta³a siê s³ynna dziêki swoim w³a¶ciwo¶ciom aromatycznym. Jest czêstym sk³adnikiem zup i innych potraw. Szczególnie popularna na wsi. Kr±¿y tak¿e legenda o jej dzia³aniu, podobnym do afrodyzjaku.",
	     250, PLANT_PART_BUSH, 25, 9,
	     {0, 0, 40, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      100, 0, 0, 0, 0,
	      0, 0, 0, 0, 0,
	      0, 0, 30, 0, 0,0,0,0,0,0},
	     0, "unknown",
	     0, "unknown"
	    },
	    {"niewielkie pachn±ce ro¶linki", "niewielkich pachn±cych ro¶linek", "niewielkim pachn±cym ro¶linkom", "niewielkie pachn±ce ro¶linki", "niewielkimi pachn±cymi ro¶linkami", "niewielkich pachn±cych ro¶linkach",
	     "macierzanka piaskowa", "macierzanki piaskowej", "macierzance piaskowej", "macierzankê piaskow±", "macierzank± piaskow±", "macierzance piaskowej",
	     "Ro¶lina ta zwana jest tak¿e cz±berkiem. Jej wygl±d jest naprawdê osobliwy. S± to liczne ma³e ro¶linki, zebrane w du¿e k³±cza. Jednak nie to jest najdziwniejsze. Ro¶lina ta wyzdiela bardzo intensywny zapach, charakterystyczny aromat, dziêki czemu jest czêsto wykorzystywana jako sk³adnik wielu olejków zapachowych i ma¶ci rozgrzewaj±cych.",
	     120, PLANT_PART_BUSH, 11, 9,
     {0, 0, 0, 50, 0,
	     0, 0, 0, 0, 0,
	     0, 60, 0, 0, 0,
	     0, 0, 0, 0, 0,
	     0, 0, 0, 0, 0,
	     0, 0, 0, 0, 0,
	     100, 0, 0, 0, 0,
	     0, 0, 0, 0, 0,
	     0, 0, 20, 0, 0,0,0,0,0,0},
	     PLANT_EFFECT_SM_SF, "unknown",
	     0, "unknown"
     },
     {"ma³y pachn±cy krzaczek", "ma³êgo pachn±cego krzaczka", "ma³emu pachn±cemu krzaczkowi", "ma³y pachn±cy krzaczek", "ma³ym pachn±cym krzaczkiem", "ma³ym pachn±cym krzaczku",
	     "majeranek ogrodowy", "majeranku ogrodowego", "majerankowi ogrodowemu", "majeranek ogrodowy", "majerankiem ogrodowym", "majeranku ogrodowym",
	     "Niewielka ro¶linka, rosn±ca w formie zwartego krzaczka. Jest to jedena z najpopularniejszych przypraw znanych na ¶wiecie. Mo¿na j± dodaæ do ka¿dego dania! Znane s± tak¿e jej w³a¶ciwo¶ci lecznicze, szczególnie pozytywnie wp³ywa na ¿o³±dek.",
		     70, PLANT_PART_BUSH, 5, 9,
		     {0, 0, 50, 5, 0,
		     0, 0, 0, 0, 0,
		     0, 10, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     100, 0, 20, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 60, 0, 0,0,0,0,0,0},
		     PLANT_EFFECT_SM_SF, "unknown",
		     0, "unknown"
	     },
		     {"du¿y czerwony kwiat na ³odydze", "du¿ego czerwonego kwiatu na ³odydze", "du¿emu czerwonemu kwiatu na ³odydze", "du¿y czerwony kwiat na ³odydze", "du¿ym czerwonym kwiatem na ³odydze", "du¿ym czerwonym kwiecie na ³odydze",
		     "mak pospolity", "maku pospolitego", "maku pospolitemu", "mak pospolity", "makiem pospolitym", "maku pospolitym",
		     "Ten charakterystyczny kwiat zna ka¿de dziecko. Ogromne, czerwone p³atki s³u¿± g³ównie do ozdoby w³osów wiejskich dziewczyn. Mo¿na go spotkaæ na ka¿dej ³±ce.",
			     15, PLANT_PART_FLOWER, 1, 9,
		     {0, 0, 90, 0, 40,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 20, 0,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 40, 0,
			     100, 0, 50, 0, 0,
			     10, 0, 0, 0, 0,
			     0, 0, 100, 0, 0,0,0,0,0,0},
			     PLANT_EFFECT_SMALL_FOOD, "unknown",
			     0, "unknown"
		     },
		     {"du¿e purpurowoczarne kwiaty", "du¿ych purpurowoczarnych kwiatów", "du¿ym purpurowoczarnym kwiatom", "du¿e purpurowoczarne kwiaty", "du¿ymi purpurowoczarnymi kwiatami", "du¿ych purpurowoczarnych kwiatach",
			     "malwa czarna", "malwy czarnej", "malwie czarnej", "malwê czarn±", "malw± czarn±", "malwie czarnej",
			     "Ten du¿y, ciemnogranatowy kwiat wygl±da naprawdê majestatycznie. Ciemna barwa jego p³atków kontrastuje z jasnozielon± ³odyg±. Wykorzystywany jest najczê¶ciej jako barwnik, oczywi¶cie ciemny. Ludowi znachowy wykorzystywali wywar z tych kwiatów tak¿e jako ¶rodek pomagaj±cy kobiecie w czasie jej jak¿e kobiecych dolegliwo¶ci.",
			     120, PLANT_PART_FLOWER, 5, 9,
		     {0, 0, 20, 0, 20,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 0, 0,
			     0, 0, 0, 0, 0,
			     100, 0, 20, 0, 0,
			     0, 0, 0, 10, 0,
			     0, 0, 50, 0, 0,0,0,0,0,0},
			     0, "unknown",
			     0, "unknown"
		     },
		     {"cienkie pe³zaj±ce k³±cze", "cieñskiego pe³zaj±cego k³±cza", "cieñskiemu pe³zaj±cemu k³±czu", "cieñkie pe³zaj±ce k³±cze", "cieñkim pe³zaj±cym k³±czem", "cieñkim pe³zaj±cym k³±czu",
			     "marzanka wonna", "marzanki wonnej", "marzance wonnej", "marzankê wonn±", "marzank± wonn±", "marzance wonnej",
			     "Niewielka i niepozorna ro¶lina, sk³adaj±ca siê g³ównie z d³ugich ³odych i wielkich li¶ci. Wykorzystywana od wieków w medycynie ludowej. Po ususzeniu zaczyna wydzielaæ przyjemny zapach, podobny do siana. St±d jej nazwa.",
				     150, PLANT_PART_STALK, 8, 9,
			     {0, 0, 0, 5, 0,
				     0, 0, 0, 0, 0,
				     0, 20, 60, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"ma³e okr±g³e skórzaste li¶cie", "ma³ych okr±g³ych skórzastych li¶ci", "ma³ym okr±g³ym skórzastym li¶ciom", "ma³e okr±g³e skórzaste li¶cie", "ma³ymi okr±g³ymi skórzastymi li¶æmi", "ma³ych okr±g³ych skórzastych li¶ciach",
				     "m±cznica lekarska", "m±cznicy lekarskiej", "m±cznicy lekarskiej", "m±cznicê lekarsk±", "m±cznic± lekarsk±", "m±cznicy lekarskiej",
				     "Najwa¿niejszym elementem tej ro¶linki s± jej ma³e, okr±g³e li¶cie. W dotyku przypominaj± skórê. Jej jedynym zastosowaniem jest medycyna. Li¶cie mo¿na zagotowaæ, stworzyæ napar, który ¶wietnie dzia³a na drogi moczowe.",
				     185, PLANT_PART_LEAF, 26, 9,
			     {0, 0, 0, 50, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 40, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"bylina z owalnosercowatymi li¶æmi", "byliny z owalnosercowatymi li¶æmi", "byliny z owalnosercowatymi li¶æmi", "bylinê z owalnosercowatymi li¶æmi", "bylin± z owalnosercowatymi li¶æmi", "bylinie z owalnosercowatymi li¶æmi",
				     "melisa lekarska", "melisy lekarskiej", "melisie lekarskiej", "melisê lekarsk±", "melis± lekarsk±", "melisie lekarskiej",
				     "Te niewielkie listki, troszkê przypominaj±ce kszta³tem serce, s± jedn± z najstarszych i najczê¶ciej stosowanych ro¶lin lekarskich. Uwarzony z nich napar dzia³a relaksuj±co, odstresowuj±co i oczyszczaj±co.",
				     220, PLANT_PART_LEAF, 22, 9,
			     {0, 0, 40, 25, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 20, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },

			     {"pachn±ce sercowate li¶cie", "pachn±cych sercowatych li¶ci", "pachn±cym sercowatym li¶ciom", "pachn±ce sercowate li¶cie", "pachn±cymi sercowatymi li¶æmi", "pachn±cych sercowatych li¶ciach",
				     "miêta", "miêty", "miêcie", "miêtê", "miêt±", "miêcie",
				     "Przygl±daj±c siê tym sercowatym li¶æiom czujesz naprawdê przyjemny zapach. Gdy je lekko pocierasz i ugniatasz, to aromat siê nasila. Dlatego te¿ podró¿nicy czêsto stosuj± to ziele w czasie podró¿y, bowiem dzia³a koj±co i uspakajaj±co. Jest te¿ znan± przypraw±.",
				     100, PLANT_PART_LEAF, 27, 9,
			     {0, 0, 10, 50, 0,
				     0, 0, 0, 0, 0,
				     0, 30, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 5, 0, 0,0,0,0,0,0},
				     PLANT_EFFECT_HIGH_MOVE, "unknown",
				     0, "unknown"

			     },
			     {"mlecz", "mlecza", "mleczowi", "mlecz", "mleczem", "mleczu",
				     "mniszek pospolity", "mniszka pospolitego", "mniszkowi pospolitemu", "mniszek pospolity", "mniszkiem pospolitym", "mniszku pospolitym",
				     "Jeden z najpopularniejszych chwastów polnych, zwany potocznie mleczem - od swoistego 'mleczka' p³yn±cego w jego ³odydze. Któ¿ z nas w dzieciñstwie nie bawi³ siê nasionami mleczy - spadachronami - które zdmuchiwa³ prosto na twarz kole¿anek i kolegów ? Niewielu natomiast zdaje sobiê sprawê z w³a¶ciwo¶ci moczopêdnych tej ro¶liny, je¿eli zostanie odpowiednio przygotowana i podana.",
				     25, PLANT_PART_FLOWER, 1, 9,
			     {0, 0, 80, 20, 70,
				     40, 0, 0, 0, 0,
				     0, 15, 0, 0, 0,
				     0, 0, 0, 90, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 70, 0,
				     100, 0, 60, 50, 0,
				     0, 0, 0, 90, 0,
				     0, 0, 70, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"walcowate rozga³êzione zielone k³±cze", "walcowatego rozga³êzionego zielonego k³±cza", "walcowatemu rozga³êzionemu zielonemu k³±czu", "walcowate rozga³êzione zielone k³±cze", "walcowatym rozga³êzionym zielonym k³±czem", "walcowatym rozga³êzionym zielonym k³±czu",
				     "mydlnica lekarska", "mydlnicy lekarskiej", "mydlnicy lekarskiej", "mydlnicê lekarsk±", "mydlnic± lekarsk±", "mydlnicy lekarskiej",
				     "Ta ciekawa ro¶linka wygl±da jak zwyk³y polny kwiatek, z niewielkimi bia³ymi kwiatkami. Jednak jej sekret tkwi w korzeniu, który to ma w³a¶ciow¶ci myj±ce, niczym myd³o. St±d wziê³a siê te¿ nazwa tego ziela.",
				     250, PLANT_PART_STALK, 55, 9,
			     {0, 0, 50, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 50, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 20, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 60, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"du¿y pomarañczowy kwiat", "du¿ego pomarañczowego kwiatu", "du¿emu pomarañczowemu kwiatu", "du¿y pomarañczowy kwiat", "du¿ym pomarañczowym kwiatem", "du¿ym pomarañczowym kwiecie",
				     "nagietek lekarski", "nagietka lekarskiego", "nagietkowi lekarskiemu", "nagietka lekarskiego", "nagietkiem lekarskim", "nagietku lekarskim",
				     "Dosyæ pospolita ro¶lina, ¶redniej wielko¶ci. Jej du¿e, ¿ó³te kwiaty mo¿na czêsto zobaczyæ na pospolitej ³±ce. Dlatego ludzie wykorzystuj± j± g³ównie w celach ozdobnych. Czasami wykorzystywana jako sk³adnik ró¿nych mikstur leczniczych.",
				     69, PLANT_PART_FLOWER, 5, 9,
			     {0, 0, 0, 0, 15,
				     5, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 15, 0, 0,
				     0, 0, 0, 10, 0,
				     0, 0, 25, 0, 0,0,0,0,0,0},
				     PLANT_EFFECT_AID, "unknown",
				     0, "unknown"
			     },
			     {"niedu¿a bylina z ¿ó³tym kwiatostanem", "niedu¿ej byliny z ¿ó³tym kwiatostanem", "niedu¿ej bylinie z ¿ó³tym kwiatostanem", "niedu¿± bylinê z ¿ó³tym kwiatostanem", "niedu¿± bylin± z ¿ó³tym kwiatostanem", "niedu¿ej bylinie z ¿ó³tym kwiatostanem",
				     "naw³oæ pospolita", "naw³oci pospolitej", "naw³oci pospolitej", "naw³oæ pospolit±", "naw³oci± pospolit±", "naw³oci pospolitej",
				     "Ogl±dasz t± pospolit± poln± bylinê i stwierdzasz, ¿e nie ma w niej nic ciekawego, poza charakterystyczn± ³odyg±, która mo¿e byæ nawet fioletowa. Od niej odchodzi wiele mniejszych ³ody¿ek, na koñcu których rosn± ma³e, zgrabne ¿ó³te kwiatuszki.",
				     33, PLANT_PART_PERENNIAL, 3, 9,
			     {0, 0, 30, 40, 0,
				     0, 0, 0, 0, 0,
				     0, 50, 70, 0, 0,
				     0, 0, 0, 20, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 30, 0,
				     100, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 40, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"drobne zwisaj±ce ¿ó³te kwiaty", "drobnych zwisaj±cych ¿ó³tych kwiatów", "drobnym zwisaj±cym ¿ó³tym kwiatom", "drobne zwisaj±ce ¿ó³te kwiaty", "drobnymi zwisaj±cymi ¿ó³tymi kwiatami", "drobnych zwisaj±cych ¿ó³tych kwiatach",
				     "nostrzyk lekarski", "nostrzyka lekarskiego", "nostrzykowi lekarskiemu", "nostrzyk lekarski", "nostrzykiem lekarskim", "nostrzyku lekarskim",
				     "Jest to ro¶lina o ³odydze wzniesionej, ¶redniej wysoko¶ci, naga lub w górze ow³osiona. Li¶cie o trzech listkach rombowojajowatych lub lancetowatych. Posiada ³±dne, bia³e kwiaty motylkowe.",
				     135, PLANT_PART_FLOWER, 65, 9,
			     {0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 80, 0, 0,
				     0, 0, 0, 50, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 70, 0,
				     100, 0, 40, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 90, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"niebieskofio³kowe drobne kwiatki", "niebieskofio³kowych drobnych kwiatków", "niebieskofio³kowym drobnym kwiatkom", "niebieskofio³kowe drobne kwiatki", "niebieskofio³kowymi drobnymi kwiatkami", "niebieskofio³kowych drobnych kwiatkach",
				     "ogórecznik lekarski", "ogórecznika lekarskiego", "ogórecznikowi lekarskiemu", "ogórecznika lekarskiego", "ogórecznikiem lekarskim", "ogóreczniku lekarskim",
				     "Ro¶lina ta jest bardzo dziwna, ale intryguj±ca. Ca³a pokryta jest wyra¼nie widocznymi w³oskami i po roztarciu wydziela intensywny zapach przypominaj±cy zapach ¶wie¿ych ogórków. W okresie kwitnienia na ro¶linie pojawiaj± siê stosunkowo du¿e, niebieskie kwiaty zebrane w zwisaj±ce, baldachowate, kwiatostany. Owocem jest pod³u¿na, ciemnobr±zowa nibyszyszka.",
				     675, PLANT_PART_FLOWER, 75, 9,
			     {0, 0, 40, 10, 0,
				     0, 0, 0, 0, 0,
				     0, 5, 20, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 50, 0, 0,
				     0, 0, 0, 60, 0,
				     0, 0, 60, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"spory pomarañczowy kwiat", "sporego pomarañczowego kwiatu", "sporemu pomarañczowemu kwiatowi", "spory pomarañczowy kwiat", "sporym pomarañczowym kwiatem", "sporym pomarañczowym kwiecie",
				     "oman wielki", "omanu wielkiego", "omanowi wielkiemu", "oman wielki", "omanem wielkim", "omanie wielkim",
				     "Przygl±dasz siê dosyæ d³ugo temu du¿emu, pomarañczowemu kwiatu, bowiem jest on dosyæ dziwny. Wygl±da tak, jakby dwa kolory walczy³y w nim o pierwszeñstwo - zielony z pomarañczowym. Jednak poza tym, jest to ca³kiem zwyczajna ro¶lina, wydzielaj±ca lekki, przyjemny zapach.",
				     750, PLANT_PART_FLOWER, 95, 9,
			     {0, 0, 0, 30, 0,
				     0, 0, 0, 0, 0,
				     0, 5, 60, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     100, 0, 0, 0, 0,
				     0, 0, 0, 0, 0,
				     0, 0, 40, 0, 0,0,0,0,0,0},
				     0, "unknown",
				     0, "unknown"
			     },
			     {"d³ugie trawiaste g³adkie li¶cie", "d³ugich trawiastych g³adkich li¶ci", "d³ugim trawiastym g³adkim li¶ciom", "d³ugie trawiaste g³adkie li¶cie", "d³ugimi trawiastymi g³adkimi li¶æmi", "d³ugich trawiastych g³adkich li¶ciach",
				     "perz w³a¶ciwy", "perzu w³a¶ciwego", "perzowi w³a¶ciwemu", "perz w³a¶ciwy", "perzem w³a¶ciwym", "perzu w³a¶ciwym",
				     "Ro¶lina ta, podobna do trawy, potrafi rosn±æ prawie wszêdzie. Wciska siê w najmniejsze za³omy skalne i wykorzystuje ka¿dy skrawek ziemi. S³ynne s± jej ogromne korzenie, kilkukrotnie wiêksze i d³u¿sze ni¿ sama ro¶lina.",
					     1, PLANT_PART_LEAF, 1, 9,
				     {0, 0, 90, 0, 30,
					     0, 0, 0, 0, 0,
					     0, 0, 0, 0, 0,
					     0, 0, 0, 10, 0,
					     0, 0, 0, 0, 0,
					     0, 0, 0, 30, 0,
					     100, 0, 80, 0, 0,
					     0, 0, 0, 70, 0,
					     0, 0, 70, 0, 0,0,0,0,0,0},
					     0, "unknown",
					     0, "unknown"
				     },
				     {"du¿e sercowate z±bkowane li¶cie", "du¿ych sercowatych z±bkowanych li¶ci", "du¿ym sercowatym z±bkowanym li¶ciom", "du¿e sercowate z±bkowane li¶cie", "du¿ymi sercowatymi z±bkowatymi li¶æmi", "du¿ych sercowatych z±bkowanych li¶ciach",
					     "podbia³ pospolity", "podbia³u pospolitego", "podbia³owi pospolitemu", "podbia³ pospolity", "podbia³em pospolitym", "podbiale pospolitym",
					     "Li¶cie te nale¿a³y kiedy¶ do dziwnej i charakterystycznej ro¶liny, o wielkich ¿ó³tych kwiatach. S± one ¶wiatnym sk³adnikiem wielu mikstur i leków. Maj± wszechstronne w³a¶ciwo¶ci.",
						     75, PLANT_PART_LEAF, 19, 9,
					     {0, 0, 0, 0, 0,
						     0, 0, 0, 0, 0,
						     0, 50, 80, 0, 0,
						     0, 0, 0, 30, 0,
						     0, 0, 0, 0, 0,
						     0, 0, 0, 50, 0,
						     100, 0, 40, 0, 0,
						     0, 0, 0, 0, 0,
						     0, 0, 30, 0, 0,0,0,0,0,0},
						     0, "unknown",
						     0, "unknown"
					     },
					     {"zielone parz±ce li¶cie", "zielonych parz±cych li¶ci", "zielonym parz±cym li¶ciom", "zielone parz±ce li¶cie", "zielonymi parz±cymi li¶æmi", "zielonych parz±cych li¶ciach",
						     "pokrzywa zwyczajna", "pokrzywy zwyczajnej", "pokrzywie zwyczajnej", "pokrzywê zwyczajn±", "pokrzyw± zwyczajn±", "pokrzywie zwyczajnej",
						     "Charakterystyczna ro¶lina polna, o wielu w³a¶ciwo¶ciach, tak¿e medycznych. Jednak wykorzystywana jest g³ównie przez dzieci, które chc± poparzyæ ni± swoich rówie¶ników, bowiem jej li¶cie zawieraj± specjalne substancje podra¿niaj±ce na krótki czas skórê.",
						     8, PLANT_PART_LEAF, 1, 9,
					     {0, 0, 80, 50, 20,
						     0, 0, 0, 0, 0,
						     0, 80, 70, 0, 0,
						     0, 0, 0, 50, 0,
						     0, 0, 0, 0, 0,
						     0, 0, 0, 80, 0,
						     100, 0, 80, 0, 0,
						     0, 0, 0, 40, 0,
						     0, 0, 90, 0, 0   ,0,0,0,0,0},
						     PLANT_EFFECT_MINUS_MINOR_HP, "unknown",
						     0, "unknown"
					     },
					     {"drobniutkie zielone listki", "drobniutkich zielonych listków", "drobniutkim zielonym listkom", "drobniutkie zielone listki", "drobniutkimi zielonymi listkami", "drobniutkich zielonych listkach",
						     "po³onicznik nagi", "po³onicznika nagiego", "po³onicznikowi nagiemu", "po³onicznik nagi", "po³onicznikiem nagim", "po³oniczniku nagim",
						     "Niewielka ro¶linka, intensywnie zielona, o równie ma³ych listkach. Bardzo trudna do odnalezienia. Jednak posiada silne w³a¶ciwo¶ci moczopêdne.",
							     315, PLANT_PART_LEAF, 15, 9,
						     {0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 30, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     100, 0, 60, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 30, 0, 0,0,0,0,0,0},
							     0, "unknown",
							     0, "unknown"
						     },
						     {"gar¶æ delikatnych czarnych owoców", "gar¶ci delikatnych czarnych owoców", "gar¶ci delikatnych czarnych owoców", "gar¶æ delikatnych czarnych owoców", "gar¶ci± delikatnych czarnych owoców", "gar¶ci delikatnych czarnych owoców",
							     "porzeczka czarna", "porzeczki czarnej", "porzeczce czarnej", "porzeczkê czarn±", "porzeczk± czarn±", "porzeczce czarnej",
							     "Te niewielkie, miêciutkie owoce pe³ne s± s³odkiego soku. Wykorzystywane s± najczê¶ciej jako barwnik, lub po prostu jako pokarm, szczególnie deserowy.",
							     55, PLANT_PART_FRUITS, 2, 9,
						     {0, 0, 0, 30, 0,
							     0, 0, 0, 0, 0,
							     0, 70, 0, 0, 0,
							     0, 0, 0, 40, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     100, 0, 0, 0, 0,
							     0, 0, 0, 40, 0,
							     0, 0, 0, 0, 0,0,0,0,0,0},
							     PLANT_EFFECT_SMALL_FOOD, "unknown",
							     0, "unknown"
						     },
						     {"gar¶æ ma³ych czarnych owoców", "gar¶ci ma³ych czarnych owoców", "gar¶ci ma³ych czarnych owoców", "gar¶æ ma³ych czarnych owoców", "gar¶ci± ma³ych czarnych owoców", "gar¶ci ma³ych czarnych owoców",
							     "poziomka pospolita", "poziomki pospolitej", "poziomce pospolitej", "poziomkê pospolit±", "poziomk± pospolit±", "poziomce pospolitej",
							     "Owoce te s± bardzo ma³e, przypominaj± malutkie truskawki. S± tak¿e koloru czerwonego. W smaku bardzo s³odkie.",
							     55, PLANT_PART_FRUITS, 2, 9,
						     {0, 0, 0, 80, 0,
							     0, 0, 0, 0, 0,
							     0, 50, 0, 0, 0,
							     0, 0, 0, 40, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     100, 0, 0, 0, 0,
							     0, 0, 0, 40, 0,
							     0, 0, 0, 0, 0,0,0,0,0,0},
							     PLANT_EFFECT_SMALL_FOOD, "unknown",
							     0, "unknown"
						     },
						     {"niewielkie owalne li¶cie", "niewielkich owalnych li¶ci", "niewielkim owalnym li¶ciom", "niewielkie owalne li¶cie", "niewielkimi owalnymi li¶æmi", "niewielkich owalnych li¶ciach",
							     "przetacznik lekarski", "przetacznika lekarskiego", "przetacznikowi lekarskiemu", "przetacznik lekarski", "przetacznikiem lekarskim", "przetaczniku lekarskim",
							     "Ogl±dasz te ciekawe kwiaty, u³o¿one w wê¿owatych, d³ugich gronach i podziasz ich ³agodn±, niebiesk± barwê. Na wsi nazywa siê j± 'lila', od koloru pewnego specjalnego gatunku lilji. Jednak okre¶lenie 'ciemny b³êkit nieba' pasuje tu równie dobrze.",
							     172, PLANT_PART_LEAF, 25, 9,
						     {0, 0, 0, 60, 0,
							     0, 0, 0, 0, 0,
							     0, 20, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     100, 0, 0, 0, 0,
							     0, 0, 0, 0, 0,
							     0, 0, 50, 0, 0,0,0,0,0,0},
							     0, "unknown",
							     0, "unknown"
						     },
	   {"w±skie skórzane listki", "w±skich skórzanych listków", "w±skich skórzanym listkom", "w±skie skórzane listki", "w±¶kimi skórzanymi listkami", "w±skich skórzanych listkach",
	   "rozmaryn lekarski", "rozmarynu lekarskiego", "rozmarynowi lekarskiemu", "rozmaryn lekarski", "rozmarynem lekarskim", "rozmarynie lekarskim",
							     "W±skie listki, które ogl±dasz nale¿± do wyj±tkowej ro¶liny. W ludowej tradycji uchodzi³a ona za symbol mi³o¶ci, ale tak¿e i ¶mierci. Stosowano j± równie¿ jako ozdobê i do sporz±dzania kadzide³. Obecnie ceniona jest jako ro¶lina przyprawowa. ",
									     231, PLANT_PART_LEAF, 15, 9,
								     {0, 0, 0, 10, 0,
									     0, 0, 0, 0, 0,
									     0, 30, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     100, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,0,0,0,0,0},
									     0, "unknown",
									     0, "unknown"
								     },
								     {"niewielkie niebieskozielone li¶cie", "niewielkich niebieskozielonych li¶ci", "niewielkim niebieskozielonym li¶ciom", "niewielkie niebieskozielone li¶cie", "niewielkimi niebieskozielonymi li¶æmi", "niewielkich niebieskozielonych li¶ciach",
									     "ruta zwyczajna", "ruty zwyczajnej", "rucie zwyczajnej", "rutê zwyczajn±", "rut± zwyczajn±", "rucie zwyczajnej",
									     "Ro¶lina ta jest uprawiana jako ro¶lina ozdobna lub dla celów leczniczych. Ma silny aromatyczny zapach i kwitnie drobnymi ¿ó³tozielonymi kwiatkami. Kiedy¶ uchodzi³a za symbol niewinno¶ci, dziewictwa.",
									     67, PLANT_PART_LEAF, 10, 9,
								     {0, 0, 45, 0, 40,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     100, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,0,0,0,0,0},
									     0, "unknown",
									     0, "unknown"
								     },
								     {"zielony jode³kowaty chwast", "zielonego jode³kowatego chwastu", "zielonemu jode³kowatemu chwastowi", "zielony jode³kowaty chwast", "zielonym jode³kowatym chwastem", "zielonym jode³kowatym chwa¶cie",
									     "skrzyp polny", "skrzypu polnego", "skrzypowi polnemu", "skrzyp polny", "skrzypem polnym", "skrzypie polnym",
									     "Skrzyp ³atwo poznaæ - wyra¼nie ró¿ni siê od powszechnie spotykanych ro¶lin. Jego letnie pêdy przypominaj± miniaturowe choinki. Intensywnie zielone, szorstkie lodygi, przy pocieraniu jeden o drug± wydaj± charakterystyczny dzwiêk, skrzypienie.",
									     1, PLANT_PART_STALK, 1, 9,
								     {0, 0, 100, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 30, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 40, 0,
									     100, 0, 30, 0, 0,
									     0, 0, 0, 20, 0,
									     0, 0, 50, 0, 0,0,0,0,0,0},
									     0, "unknown",
									     0, "unknown"
								     },
								     {"ciemnozielone trójk±tnoowalne li¶cie", "ciemnozielonych trójk±tnoowalnych li¶ci", "ciemnozielonym trójk±tnoowalnym li¶ciom", "ciemnozielone trójk±tnoowalne li¶cie", "ciemnozielonymi trójk±tnoowalnymi li¶æmi", "ciemnozielonych trójk±tnoowalnych li¶ciach",
									     "¶wietlik ³±kowy", "¶wietlika ³±kowego", "¶wietlikowi ³±kowemu", "¶wietlik ³±kowy", "¶wietlikiem ³±kowym", "¶wietliku ³±kowym",
									     "Ciekawe listki, wygl±daj± tak, jakby kto¶ lekko ¶ci±³ trójk±ty, po³±czone ze sob± bokami. Sama ro¶lina s³ynie z tego, ¿e ¿yje pó³paso¿ytniczo na korzeniach innych ro¶lin ³±kowych. Kwiaty na tle zieleni, przypominaj± ¶wieczki.",
									     165, PLANT_PART_LEAF, 9, 9,
								     {0, 0, 15, 35, 70,
									     0, 0, 0, 0, 0,
									     0, 5, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     100, 0, 15, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 75, 0, 0,0,0,0,0,0},
									     0, "unknown",
									     0, "unknown"
								     },
								     {"rozeta z±bkowanych pierzastych li¶ci", "rozety z±bkowanych pierzastych li¶ci", "rozecie z±bkowanych pierzastych li¶ci", "rozetê z±bkowanych pierzastych li¶ci", "rozet± z±bkowanych pierzastych li¶ci", "rozecie z±bkowanych pierzastych li¶ci",
									     "tasznik pospolity", "tasznika pospolitego", "tasznikowi pospolitemu", "tasznik pospolity", "tasznikiem pospolitym", "taszniku pospolitym",
									     "Przygl±dasz siê tym ciekawym li¶ciom zebranym w ró¿yczkê. Dooko³a nich widzisz bia³e, drobne kwiaty na szczycie ro¶liny. Posiada tak¿e owoce w postaci sercowatych ³uszczynek, które przypominaj± kszta³tem dawne torby pastusze, st±d nazwa ro¶liny.",
									     65, PLANT_PART_LEAF, 7, 9,
								     {0, 0, 80, 0, 20,
									     5, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 45, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 85, 0,
									     100, 0, 50, 0, 0,
									     0, 0, 0, 70, 0,
									     0, 0, 70, 0, 0,0,0,0,0,0},
									     0, "unknown",
									     0, "unknown"
								     },
								     {"d³ugie mieczowate li¶cie", "d³ugich mieczowatych li¶ci", "d³ugim mieczowatym li¶ciom", "d³ugie mieczowate li¶cie", "d³ugimi mieczowatymi li¶æmi", "d³ugich mieczowatych li¶ciach",
									     "tatarak zwyczajny", "tataraku zwyczajnego", "tatarakowi zwyczajnemu", "tatarak zwyczajny", "tatarakiem zwyczajnym", "tataraku zwyczajnym",
									     "Ro¶lina ta wystêpuje pospolitcie w zbiorowiskacg b³otnych i wodnych. Posiada charakterystyczne, d³ugie, mieczowate li¶cie. Jest silnie aromatyczna, wydziela specyficzny, mi³y zapach po roztarciu. Jest znana ze swoich w³a¶ciwo¶ci lekarskich.",
									     24, PLANT_PART_LEAF, 2, 9,
								     {0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 100, 0, 0,
									     0, 0, 0, 0, 50,
									     80, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     100, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,
									     0, 0, 0, 0, 0,0,0,0,0,0},
									     0, "unknown",
									     0, "unknown"
								     },
								     {"drobne ga³±zki z ró¿owymi kwiatkami", "drobnych ga³±zek z ró¿owymi kwiatkami", "drobnym ga³±zkom z ró¿owymi kwiatkami", "drobne ga³±zki z ró¿owymi kwiatkami", "drobnymi ga³±zkami z ró¿owymi kwiatkami", "drobnych ga³±zkach z ró¿owymi kwiatkami",
									     "tymianek pospolity", "tymianku pospolitego", "tymiankowi pospolitemu", "tymianek pospolity", "tymiankiem pospolitym", "tymianku pospolitym",
									     "Kiedy¶, dawno dawno temu tymianek by³ u¿ywany do balsamowania zw³ok i dodawano go do kadzide³. Ponadto mia³ dodawaæ odwagi na polu walki. St±d jego nazwa, która oznacza odwagê, mêstwo. Do niedawna rycerze dostawali tymianek od swoich dam i przyk³adali go do ran. Poza tym kobiety wierz±, ¿e k±piel w naparze z ro¶liny powiêksza biust.",
												     89, PLANT_PART_FLOWER, 13, 9,
											     {0, 0, 50, 0, 20,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     100, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 40, 0, 0,0,0,0,0,0},
												     PLANT_EFFECT_SM_SF, "unknown",
												     0, "unknown"
											     },
											     {"poskrêcany bia³y korzonek", "poskrêcanego bia³ego korzonka", "poskrêcanemu bia³emu korzonkowi", "poskrêcany bia³y korzonek", "poskrêcanym bia³ym korzonkiem", "poskrêcanym bia³ym korzonku",
												     "wil¿yna ciernista", "wil¿yny ciernistej", "wil¿ynie ciernistej", "wil¿ynê ciernist±", "wil¿yn± ciernist±", "wil¿ycie ciernistej",
												     "Przygl±dasz siê tej niesamowitej ro¶linie, a w³a¶ciwie jej korzeniowi. Jest on d³ugi, z zewn±trz brunatny, na prze³omie ¿ó³tobia³y z wyra¼nymi ciemniejszymi promieniami.",
												     93, PLANT_PART_ROOT, 4, 9,
											     {0, 0, 0, 0, 40,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 20, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 30, 0,
												     100, 0, 30, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 70, 0, 0,0,0,0,0,0},
												     0, "unknown",
												     0, "unknown"
											     },
											     {"ma³y krzaczek z drobnymi li¶æmi", "ma³ego krzaczka z drobnymi li¶æmi", "ma³emu krzaczkowi z drobnymi li¶æmi", "ma³y krzaczek z drobnymi li¶æmi", "ma³ym krzaczkiem z drobnymi li¶æmi", "ma³ym krzaczku z drobnymi li¶æmi",
												     "wrzos zwyczajny", "wrzosu zwyczajnego", "wrzosowi zwyczajnemu", "wrzos zwyczajny", "wrzosem zwyczajnym", "wrzosie zwyczajnym",
												     "Ten charakterystyczny, kolorowy krzaczek kwitnie na ró¿owo, a czasem na bia³o. Jego owocem jest charakterystyczna torebka. Wystêpuje w suchych, piaszczystych borach sosnowych i na terenach torfiastych. Ma w³a¶ciwo¶ci lecznicze miêdzy innymi jest dobry na ¿o³±dek.",
												     109, PLANT_PART_BUSH, 25, 9,
											     {0, 0, 10, 70, 50,
												     0, 0, 0, 0, 0,
												     0, 40, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     100, 10, 90, 90, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 80, 0, 0,0,0,0,0,0},
												     0, "unknown",
												     0, "unknown"
											     },
											     {"gruby br±zowy korzeñ", "grubego br±zowego korzenia", "grubemu br±zowemu korzeniowi", "gruby br±zowy korzeñ", "grubym br±zowym korzeniem", "grubym br±zowym korzeniu",
												     "¿ywokost lekarski", "¿ywokostu lekarskiego", "¿ywokostowi lekarskiemu", "¿ywokost lekarski", "¿ywokostem lekarskim", "¿ywoko¶cie lekarskim",
												     "G³ównym i najwa¿niejszym elementem tej ro¶liny jest jej du¿y, gruby, br±zowy korzeñ. Zas³yn±³ on w zwiazku ze swoimi specyficznymi w³a¶ciwo¶ciami. Nawet nazwa ro¶liny powsta³a st±d, i¿ dawniej u¿ywano ¿ywokostu jako ¶rodka przy¶pieszaj±cego zrastanie siê z³amanych ko¶ci.",
												     335, PLANT_PART_ROOT, 22, 9,
											     {0, 0, 15, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 40, 0, 0,
												     0, 0, 0, 0, 10,
												     20, 0, 0, 0, 0,
												     0, 0, 0, 0, 10,
												     100, 0, 0, 0, 0,
												     0, 0, 0, 0, 0,
												     0, 0, 65, 0, 0,0,0,0,0,0},
												     PLANT_EFFECT_SF_SH, "unknown",
	0, "unknown"
	},
/*65*/                                 {"niedu¿y okr±g³y kaktus","niedu¿ego okr±g³ego kaktusa","niedu¿emu okr±g³emu kaktusowi","niedu¿ego okr±g³ego kaktusa","niedu¿ym okr±g³ym kaktusem","niedu¿ym okr±g³ym kaktusie",
                                          "mammilaria","mammilarii","mammilarii","mammilariê","mammilari±","mammilarii",
                                          "Jest to niedu¿y okr±g³y kaktus pokryty wieloma drobniutkimi kolcami. Wydaj± siê one na do¶æ miêkkie i niegro¼ne, sprawiaj±c na pierwszy rzut oka wra¿enie futerka. Ro¶lina ta jest okr±g³a i niemal idealnie symetryczna.",
                                          670,PLANT_PART_CACTUS  ,90,9,
                                        {0,0,0,0,0,0,0,0,0,0,
                                        40,0,0,0,0,0,0,0,0,0,
                                         0,0,0,0,0,0,0,0,0,0,
                                         0,65,0,40,0,0,0,0,0,0,
                                         0,0,0,0,0,0,0,0,0},
                                         PLANT_EFFECT_SF_ST_MLH,"opis smaku",	0,"opis palenia"},

                                       {"kaktus z piêknym ¿ó³tym kwiatem","kaktusa z piêknym ¿ó³tym kwiatem","kaktusowi z piêknym ¿ó³tym kwiatem","kaktusa z piêknym ¿ó³tym kwiatem","kaktusem z piêknym ¿ó³tym kwiatem","kaktusie z piêknym ¿ó³tym kwiatem",
                                        "notokaktus","notokaktusa","notokaktusowi","notokaktusa","notokaktusem","notokaktusie",
                                         "W oczy rzuca siê piêkny ¿ó³ty kwiat, który wyrós³ z tego niepozornego kaktusa.",
                                         	554,PLANT_PART_CACTUS  ,80,9,
                                         {0,0,0,0,0,0,0,0,0,0,
                                         35,0,0,0,0,0,0,0,0,0,
                                         	0,0,0,0,0,0,0,0,0,0,
                                         	0,55,0,50,0,0,0,0,0,0,
                                         	0,0,0,0,0,0,0,0,0},
                                         	PLANT_EFFECT_SF_ST_MLH,"opis smaku",	0,"opis palenia"},

                                        {"du¿y w±ski kaktus","du¿ego w±skiego kaktusa","du¿emu w±skiemu kaktusowi","du¿ego w±skiego kaktusa","du¿ym w±skim kaktusem","du¿ym w±skim kaktusie",
                                        	"aporokaktus","aporokaktusa","aporokaktusowi","aporokaktusa","aporokaktusem","aporokaktusie",
                                        	"Masz przed sob± niewielkiej wielko¶ci ro¶linê, z której wyrasta niezliczona ilo¶æ kolców.",
                                        	380,PLANT_PART_CACTUS  ,80,9,
                                        	{0,0,0,0,0,0,0,0,0,0,
                                        	50,0,0,0,0,0,0,0,0,0,
                                        	0,0,0,0,0,0,0,0,0,0,
                                        	0,70,0,20,0,0,0,0,0,0,
                                        	0,0,0,0,0,0,0,0,0},
                                        	PLANT_EFFECT_SF_ST_MLH,"opis smaku",	0,"opis palenia"},

                                        {"du¿y okr±g³y kaktus","du¿ego okr±g³ego kaktusa","du¿emu okr±g³emu kaktusowi","du¿ego okr±g³ego kaktusa","du¿ym okr±g³ym kaktusem","du¿ym okr±g³ym kaktusie",
                                        	"echinokaktus","echinokaktusa","echinokaktusowi","echinokaktusa","echinokaktusem","echinokaktusie",
                                        	"Ro¶lina ta jest kszta³tu kulistego i wszêdzie porastaj± j± niewielkie kolce.",
                                        	472,PLANT_PART_CACTUS  ,90,9,
                                        	{0,0,0,0,0,0,0,0,0,0,
                                        	40,0,0,0,0,0,0,0,0,0,
                                        	0,0,0,0,0,0,0,0,0,0,
                                        	0,60,0,25,0,0,0,0,0,0,
                                        	0,0,0,0,0,0,0,0,0},
                                        	PLANT_EFFECT_MF_MT_MLH,"opis smaku",	0,"opis palenia"},

                                        {"niewielki p³aski kaktus","niewielkiego p³askiego kaktusa","niewielkiemu p³askiemu kaktusowi","niewielkiego p³askiego kaktusa","niewielkim p³askim kaktusem","niewielkim p³askim kaktusie",
                                        	"zygokaktus","zygokaktusa","zygokaktusowi","zygokaktusa","zygokaktusem","zygokaktusie",
                                        	"Jest to p³aska ro¶lina, której kolce pn± siê idealnie w górê. Posiada owalny kszta³t i mi³y zapach.",
                                        	649,PLANT_PART_CACTUS  ,70,9,
                                        	{0,0,0,0,0,0,0,0,0,0,
                                        	25,0,0,0,0,0,0,0,0,0,
                                        	0,0,0,0,0,0,0,0,0,0,
                                        	0,50,0,55,0,0,0,0,0,0,
                                        	0,0,0,0,0,0,0,0,0},
                                        	PLANT_EFFECT_SF_ST_MLH,"opis smaku",	0,"opis palenia"},

                                         {"piêkny du¿y fioletowy kwiat","piêknego du¿ego fioletowego kwiata","piêknemu du¿emu fioletowemu kwiatowi","piêkny du¿y fioletowy kwiat","piêknym du¿ym fioletowym kwiatem","piêknym du¿ym fioletowym kwiecie",
                                         	"naparstnica purpurowa","naparstnicy purpurowej","naparstnicy purpurowej","naparstnicê purpurow±","naparstnic± purpurow±","naparstnicy purpurowej",
                                         	"Kwiat o olbrzymim, ró¿owym kielichu, którego li¶cie s± lekko postrzêpione.",
                                         	30,PLANT_PART_FLOWER  ,0,6,
                                         	{0,0,10,10,0,0,0,0,0,0,
                                         	0,15,0,0,0,0,0,0,0,0,
                                         	0,0,0,0,0,0,0,0,0,0,
                                         	0,0,0,0,0,0,0,0,40,0,
                                         	0,0,25,0,0,0,0,0,0},  PLANT_EFFECT_POISON_LEVEL10,"opis smaku",	0,"opis palenia"},

                                         {"dziwne czarne jagody","dziwnych czarnych jagód","dziwnym czarnym jagodom","dziwne czarne jagody","dziwnymi czarnymi jagodami","dziwnych czarnych jagodach",
                                          "pokrzyk","pokrzyku","pokrzykowi","pokrzyk","pokrzykiem","pokrzyku",
                                          "Niewielki krzaczek jakiej¶ ro¶linki, z którego wyrastaj± czarne jagody. Te ma³e, czarne kuleczki wstydliwie kryj± siê pod g³adkimi li¶æmi.",
                                          128,PLANT_PART_BERRYS  ,10,0,
                                          {0,0,0,15,15,0,0,0,0,0,
                                           0,5,15,0,0,0,0,0,0,0,
                                           0,0,0,0,0,0,0,0,0,0,
                                           0,0,0,0,0,0,0,0,0,0,
                                           0,0,5,0,0,0,0,0,0},    PLANT_EFFECT_POISON_LEVEL20,"opis smaku",	0,"opis palenia"},

                                         {"du¿e pierzaste li¶cie","du¿ych pierzastych li¶ci","du¿ym pierzastym li¶ciom","du¿e pierzaste li¶cie","du¿ymi pierzastymi li¶ciami","du¿ych pierzastych li¶ciach",
                                         	"lulek czarny","lulka czarnego","lukowi czarnemu","lulka czarnego","lulkiem czarnym","lulku czarnym",
                                        	"Ro¶lina o du¿ych pierzastych li¶ciach, które wydzielaj± dra¿ni±cy nos zapach. Koñcówki li¶ci s± postrzêpione, zupe³nie jakby jakie¶ ma³e ¿yj±tka je ogryza³y. Ciekawe tylko, czy wysz³o im to na zdrowie.",
                                         	370,PLANT_PART_LEAF  ,50,1,
                                         	{0,0,0,0,0,0,0,0,0,0,
                                         	 0,0,0,0,0,0,10,0,0,0,
                                         	 0,0,0,0,0,0,0,0,0,0,
                                         	 0,10,0,0,0,0,0,0,0,0,
                                         	 10,0,0,0,0,0,0,0,0},	PLANT_EFFECT_POISON_LEVEL30,"opis smaku",	0,"opis palenia"},

                                          {"du¿y bia³y p³aski kwiatek","du¿ego bia³ego p³askiego kwiatka","du¿emu bia³emu p³askiemu kwiatkowi","du¿ego bia³ego p³askiego kwiatka","du¿ym bia³ym p³askim kwiatkiem","du¿ym bia³ym p³askim kwiatku",
                                           "ciemiernik bia³y","ciernika bia³ego","ciernikowi bia³emu","ciernika bia³êgo","ciernikiem bia³ym","cierniku bia³ym",
                                           "Ro¶lina na któr± patrzysz posiada niewielkie listki, prostopadle wyrastaj±ce od ³odygi. Na jej koñcu znajduje siê olbrzymich rozmiarów p³aski kwiat w bia³ym kolorze. Piêknie pachnie.",
                                           189,PLANT_PART_FLOWER  ,20,4,
                                           {0,0,0,0,10,20,0,0,0,0,
                                            0,0,0,50,0,0,15,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0},  PLANT_EFFECT_POISON_LEVEL30,"opis smaku",	0,"opis palenia"},

                                          {"licznie rozga³êzione m³ode pêdy","licznie rozga³êzionego m³odego pêdu","licznie rozga³êzionemu m³odemu pêdowi","licznie rozga³êziony m³odego pêd","licznie rozga³êzionym m³odym pêdem","licznie rozga³êzionym m³odym pêdzie",
                                           "glistnik jaskó³cze ziele","glistnika jaskó³czego ziela","glistnikowi jaskó³czego ziela","glistnika jaskó³czego ziela ","glistnik± jaskó³cze ziele","glistniku jaskó³cze ziele",
                                           "Ro¶lina ta nie posiada nawet najmniejszego listka. Jedyne co widzisz to wij±ce siê wokó³ ³odygi m³ode pêdy.",
                                           15,PLANT_PART_LEAF  ,0,6,
                                           {0,0,25,0,0,0,0,0,0,0,
                                            0,0,80,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0,
                                            0,0,40,0,0,0,0,0,0},
                                            PLANT_EFFECT_POISON_LEVEL20,"opis smaku",	0,"opis palenia"},

                                          {"ma³y szarozielony omszony krzaczek","ma³ego szarozielonego omszonego krzaczka","ma³emu szarozielonemu omszonemu krzaczkowi","ma³ego szarozielonego omszonego krzaczek","ma³ym szarozielonym omszonym krzaczkiem","ma³ym szarozielonym omszonym krzaczku",
                                           "bylica pio³un","bylicy pio³una","bylicy pio³una","bylicê pio³una","bylic± pio³una","bylicy pio³una",
                                           "Ogl±dasz ten szarozielony krzaczek, który jest niewielkich rozmiarów ? mie¶ci siê na d³oni.",
                                           277,PLANT_PART_BUSH  ,50,7,
                                           {0,0,30,5,10,5,0,0,0,0,
                                            0,5,20,0,0,0,0,0,5,0,
                                            0,0,0,0,0,0,0,0,10,0,
                                            0,0,30,10,0,0,0,0,5,0,
                                            0,0,50,0,0,0,0,0,0},
                                            PLANT_EFFECT_POISON_LEVEL10,"opis smaku",	0,"opis palenia"},

                                          {"prze¶liczny bia³y kwiat","prze¶licznego bia³ego kwiata","prze¶licznemu bia³emu kwiatowi","prze¶licznego bia³ego kwiata","prze¶licznym bia³ym kwiatem","prze¶licznym bia³ym kwiecie",
                                           "narcyz bia³y","narcyza bia³ego","narcyzowi bia³emu","narcyza bia³ego","narcyzem bia³ym","narcyzie bia³ym",
                                           "Ro¶lina o p³askich, l¶ni±cych li¶ciach w¶ród których wyrasta przepiêkny bia³y kwiat w formie kielicha.",
                                           10,PLANT_PART_FLOWER  ,0,8,
                                           {0,0,0,0,5,15,0,0,0,0,
                                            0,0,0,5,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0,
                                            0,0,10,0,0,0,0,0,25,0,
                                            0,0,10,0,0,0,0,0,0},
                                            PLANT_EFFECT_POISON_LEVEL10,"opis smaku",	0,"opis palenia"},

                                          {"szary bluszcz z bia³ymi kwiatkami","szarego bluszczu z bia³ymi kwiatkami","szaremu bluszczowi z bia³ymi kwiatkami","szary bluszcz z bia³ymi kwiatkami","szarym bluszczem z bia³ymi kwiatkami","szarym bluszczu z bia³ymi kwiatkami",
                                           "przestêp dwupienny","przestêpu dwupiennego","przestêpowi dwupiennemu","przestêpa dwupiennego","przystêpem dwupiennym","przestêpie dwupiennym",
                                           "Ogl±dasz jak±¶ dziwn± ro¶linê o nieregularnych kszta³tach. Po¶ród m³odych pêdów, wyrastaj± malutkie, bia³e kwiatuszki i kusz± swym wygl±dem wêdrowców.",
                                           295,PLANT_PART_BUSH  ,40,5,
                                           {0,0,0,30,15,0,0,0,0,0,
                                            0,5,30,0,0,0,0,0,5,0,
                                            0,0,0,0,0,0,0,0,10,0,
                                            0,0,10,0,0,5,0,0,20,0,
                                            0,0,5,0,0,0,0,0,0},
                                            PLANT_EFFECT_POISON_LEVEL10,"opis smaku",	0,"opis palenia"},

                                              {"gar¶æ ma³ych bia³ych owoców","gar¶ci ma³ych bia³ych owoców","gar¶ci ma³ych bia³ych owoców","gar¶æ ma³ych bia³ych owoców","gar¶ci± ma³ych bia³ych owoców","gar¶ci ma³ych bia³ych owoców",
                                               "¶nieguliczka bia³a","¶nieguliczki bia³ej","¶nieguliczce bia³ej","¶nieguliczkê bia³±","¶nieguliczk± bia³±","¶nieguliczce bia³ej",
                                               "Ogl±dasz ma³e, bia³e owoce o owalnym kszta³cie. Nie wydzielaj± ¿adnego zapachu, jednak mimo to wygl±daj± na ca³kiem smaczne.",
                                               5,PLANT_PART_FRUITS  ,0,8,
                                               {0,0,0,10,40,0,0,0,0,0,
                                                0,5,60,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,0,0,
                                                0,0,0,0,0,0,0,0,80,0,
                                                0,0,0,0,0,0,0,0,0},
                                                PLANT_EFFECT_POISON_LEVEL1,"opis smaku",	0,"opis palenia"},

                                              {"dzwonkowate fioletowe kwiaty","dzwonkowatych fioletowych kwiatów","dzwonkowatym fioletowym kwiatom","dzwonkowate fioletowe kwiatów","dzwonkowatymi fioletowymi kwiatami","dzwonkowatych fioletowych kwiatach",
                                               "tojad mocny","tojada mocnego","tojadowi mocnemu","tojada mocnego","tojadem mocnym","tojadzie mocnym",
                                               "Przepiêkne kwiaty w fioletowym kolorze. Maj± one kszta³t dzwonu i zachêcaj± swym s³odkim zapachem.",
                                               	12,PLANT_PART_FLOWER  ,0,2,
                                               	{0,0,0,0,40,50,0,0,0,0,
                                               	 0,0,0,25,0,0,0,0,0,0,
                                               	 0,0,0,0,0,0,0,0,0,0,
                                               	 0,0,5,0,0,10,0,0,10,0,
                                               	 0,0,5,0,0,0,0,0,0},
                                               	 PLANT_EFFECT_POISON_PARALIZING,"opis smaku",	0,"opis palenia"},

                                               {"intensywnie czerwone jagody","intensywnie czerwonych jagód","intensywnie czerwonym jagodom","intensywnie czerwone jagody","intensywnie czerwonymi jagodami","intensywnie czerwonych jagodach",
                                               	"wawrzynek wilcze³yko","wawrzynka wilcze³yka","wawrzykowi wilcze³yko","wawrzynka wilcze³ykiem","wawrzykiem wilcze³ykiem","wawrzyku wilcze³yku",
                                               	"Niewielkie, krwistoczerwone jagody. S± okr±g³e niczym ma³e kuleczki i pachn± niezwykle smakowicie.",
                                               	57,PLANT_PART_BERRYS  ,10,0,
                                               	{0,0,0,20,30,15,0,0,0,0,
                                               	 0,10,0,0,0,0,0,0,0,0,
                                               	 0,0,0,0,0,0,0,0,0,0,
                                               	 0,0,0,0,0,0,0,0,0,0,
                                               	 0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_POISON_BLIND,"opis smaku",	0,"opis palenia"},

                                                {"niedu¿e br±zowe grzyby","niedu¿ych br±zowych grzybów","niedu¿ym br±zowym grzybom","niedu¿e br±zowe grzyby","niedu¿ymi br±zowymi grzybami","niedu¿ych br±zowych grzybach",
                                                 "prawdziwki","prawdziwków","prawdziwkom","prawdziwki","prawdziwkami","prawdziwkach",
                                                 "Ogl±dasz niedu¿e, br±zowe grzyby. Wydzielaj± lekko gorzki zapach a ca³y br±zowy kapelusz opiera siê na solidnej, bia³ej nó¿ce.",
                                                 86,PLANT_PART_FUNGUS  ,20,9,
                                                 {0,0,0,30,0,0,0,0,0,0,
                                                  0,40,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0},
                                                  PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                {"ma³e ¿ó³te grzybki","ma³ych ¿ó³tych grzybków","ma³ym ¿ó³tym grzybkom","ma³e ¿ó³te grzybki","ma³ymi ¿ó³tymi grzybkami","ma³ych ¿ó³tych grzybkach",
                                                 "kurki","kurek","kurkom","kurki","kurkami","kurkach",
                                                 "Malutkie grzybki, które maj± ³adny ¿ó³ty kolor. W ca³o¶ci s± postrzêpione i powykrzywiane, jednak mimo to nadaj± siê chyba do jedzenia.",
                                                 126,PLANT_PART_FUNGUS  ,30,9,
                                                 {0,0,0,50,0,0,0,0,0,0,
                                                  0,65,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0},
                                                  PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                {"du¿y grzyb z br±zowymi kropkami","du¿ego grzyba z br±zowymi kropkami","du¿emu grzybowi z br±zowymi kropkami","du¿ego grzyba z br±zowymi kropkami","du¿ym grzybem z br±zowymi kropkami","du¿ym grzybie z br±zowymi kropkami",
                                                 "kania","kani","kaniom","kanie","kaniami","kaniach",
                                                 "Grzyb wysoki na kilkana¶cie centymetrów z kapeluszem pokrytym jakimi¶ br±zowymi kropkami. Opiera siê na cieniutkiej nó¿ce, której bia³y nalot zostawia ¶lady na twoich rêkach.",
                                                 15,PLANT_PART_FUNGUS  ,0,9,
                                                 {0,0,0,40,0,0,0,0,0,0,
                                                  0,30,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0,0,
                                                  0,0,0,0,0,0,0,0,0},
                                                  PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                 {"¶rednie rudoczerwone grzyby","¶rednich rudoczerwonych grzybów","¶rednim rudoczerwonym grzybom","¶rednie rudoczerwone grzybki","¶rednimi rudoczerwonymi grzybami","¶rednich rudoczerwonych grzybach",
                                                  "rydze","rydzy","rydzom","rydze","rydzami","rydzach",
                                                  "Niewielkie grzyby o rudym, postrzêpionym kapeluszu w kszta³cie kielicha. Grzyby te brudz± twoje rêce jak±¶ wydzielin± w kolorze czerwonym.",
                                                  38,PLANT_PART_FUNGUS  ,10,9,
                                                  {0,0,0,10,0,0,0,0,0,0,
                                                   0,40,0,0,0,0,0,0,0,0,
                                                   0,0,0,0,0,0,0,0,0,0,
                                                   0,0,0,0,0,0,0,0,0,0,
                                                   0,0,0,0,0,0,0,0,0},
                                                   PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                 {"du¿y ³adny lekko zielonkawy grzyb","du¿ego ³adnego lekko zielonkawego grzyba","du¿emu ³adnemu lekko zielonkawemu grzybowi","du¿ego ³adnego lekko zielonkawego grzyba","du¿ym ³adnym lekko zielonkawym grzybem","du¿ym ³adnym lekko zielonkawym grzybie",
                                                  "zielonka","zielonki","zielonkom","zielonke","zielonk±","zielonce",	"Du¿ych rozmiarów grzyb o bardzo ³adnym, zielonkawym kolorze. Wygl±da na ca³kiem smaczny.",
                                                  18,PLANT_PART_FUNGUS  ,0,9,
                                                  {0,0,0,20,0,0,0,0,0,0,
                                                   0,70,0,0,0,0,0,0,0,0,
                                                   0,0,0,0,0,0,0,0,0,0,
                                                   0,0,0,0,0,0,0,0,0,0,
                                                   0,0,0,0,0,0,0,0,0},
                                                   PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                 {"ma³e brunatne grzybki","ma³ych brunatnych grzybków","ma³ym brunatnym grzybkom","ma³e brunatne grzybki","ma³ymi brunatnymi grzybkami","ma³ych brunatnych grzybkach",
                                                  "podgrzybki","podgrzybków","podgrzybkom","podgrzybki","podgrzybkami","podgrzybkach",
                                                  "Malutkie grzyby o brunatnym kolorze. Ich owalne kapelusze dumnie opieraj± siê na solidnej, grubej nó¿ce.",
                                                  77,PLANT_PART_FUNGUS  ,30,9,
                                                  {0,0,0,70,0,0,0,0,0,0,
                                                  	0,90,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0},
                                                  	PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                 {"¶rednie pomarañczowe grzyby","¶rednich pomarañczowych grzybów","¶rednim pomarañczowym grzybom","¶rednie pomarañczowe grzyby","¶rednimi pomarañczowymi grzybami","¶rednich pomarañczowych grzybach",
                                                  "ko¼larze","ko¼larzy","ko¼larzom","ko¼larze","ko¼larzami","ko¼larzach",
                                                  "Zwyczajne, ¶rednich rozmiarów grzyby w pomarañczowym kolorze.",
                                                  28,PLANT_PART_FUNGUS  ,10,9,
                                                  {0,0,0,60,0,0,0,0,0,0,
                                                  	0,40,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0},
                                                  	PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                 {"spory grzyb o lepkim kapeluszu","sporego grzyba o lepkim kapeluszu","sporemu grzybowi o lepkim kapeluszu","sporego grzyba o lepkim kapeluszu","sporym grzybem o lepkim kapeluszu","sporym grzybie o lepkim kapeluszu",
                                                  "ma¶lak","ma¶laka","ma¶lakowi","ma¶laka","ma¶lakiem","ma¶laku",
                                                  "Ogl±dasz spory grzyb, którego br±zowy kapelusz pokryty jest jak±¶ dziwn±, lepk± mazi±.",
                                                  8,PLANT_PART_FUNGUS  ,0,9,
                                                  {0,0,0,50,0,0,0,0,0,0,
                                                  	0,20,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0},
                                                  	PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                 {"mnóstwo malutkich br±zowych grzybków","mnóstwa malutkich br±zowych grzybków","mnóstwu malutkich br±zowych grzybków","mnóstwo malutkich br±zowych grzybków","mnóstwem malutkich br±zowych grzybków","mnóstwie malutkich br±zowych grzybków",
                                                  "opieñki","opieñka","opieñkowi","opieñka","opieñkiem","opieñku",
                                                  "Mnóstwo malutkich grzybków w br±zowym kolorze, których kapelusze opieraj± siê na cieniuteñkich nózkach.",
                                                  133,PLANT_PART_FUNGUS  ,30,9,
                                                  {0,0,0,70,0,0,0,0,0,0,
                                                  	0,40,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0,0,
                                                  	0,0,0,0,0,0,0,0,0},
                                                  	PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                  {"niedu¿e bia³e okr±g³e grzyby","niedu¿ych bia³ych okr±g³ych grzybów","niedu¿ym bia³ym okr±g³ym grzybom","niedu¿e bia³e okr±g³e grzyby","niedu¿ymi bia³ymi okr±g³ymi grzybami","niedu¿ych bia³ych okr±g³ych grzybach",
                                                  	"pieczarki jadalne","pieczarek jadalnych","pieczarkom jadalnym","pieczarki jadalne","pieczarkami jadalnymi","pieczarkach jadalnych",
                                                  	"S± to niedu¿e, bia³e grzyby o okr±g³ych kszta³tach.",
                                                  	80,PLANT_PART_FUNGUS  ,20,9,
                                                  	{0,0,50,30,0,0,0,0,0,0,
                                                  	 0,20,0,0,0,0,0,0,0,0,
                                                  	 0,0,0,0,0,0,0,0,0,0,
                                                  	 0,0,0,0,0,0,0,0,0,0,
                                                  	 0,0,40,0,0,0,0,0,0},
                                                  	 PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                   {"du¿y br±zowy grzyb z d³ugim s³upkiem","du¿ego br±zowego grzyba z d³ugim s³upkiem","du¿emu br±zowemu grzybowi z d³ugim s³upkiem","du¿ego br±zowego grzyba z d³ugim s³upkiem","du¿ym br±zowym grzybem z d³ugim s³upkiem","du¿ym br±zowym grzybie z d³ugim s³upkiem",
                                                    "turek","turka","turkowi","turka","turkiem","turku",
                                                    "Sporych rozmiarów grzyb o niewielkim, br±zowym kapeluszu i niespotykanie d³ugim s³upku.",
                                                    15,PLANT_PART_FUNGUS  ,0,9,
                                                    {0,0,0,50,0,0,0,0,0,0,
                                                     0,60,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0},
                                                     PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                   {"ma³y okr±g³y grzybek","ma³ego okr±g³ego grzybka","ma³emu okr±g³emu grzybkowi","ma³ego okr±g³ego grzybka","ma³ym okr±g³ym grzybkiem","ma³ym okr±g³ym grzybku",
                                                    "purchawka","purchawki","purchawce","purchawkê","purchawk±","purchawce",
                                                    "Malutki, okr±g³y grzybek w szarym kolorze. Jest on niezwykle napêcznia³y, zupe³nie jakby w swoim wnêtrzu skrywa³ jak±¶ tajemnice.",
                                                    92,PLANT_PART_FUNGUS  ,20,9,
                                                    {0,0,0,80,0,0,0,0,0,0,
                                                     0,70,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0},
                                                     PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                   {"mnóstwo ma³ych bladoró¿owych grzybków","mnóstwa ma³ych bladoró¿owych grzybków","mnóstwu ma³ych bladoró¿owych grzybków","mnóstwo ma³ych bladoró¿owych grzybków","mnóstwem ma³ych bladoró¿owych grzybków","mnóstwie ma³ych bladoró¿owych grzybków",
                                                    "twardzioszki przydro¿ne","twardzioszków przydro¿nych","twardzoszkom przydro¿nym","twardzoszki przydro¿ne","twardzioszkami przydro¿nymi","twardzioszkach przydro¿nych",
                                                    "Ogromna ilo¶æ ma³ych, bladoró¿owych grzybków, które wydzielaj± s³odki zapach.",
                                                    136,PLANT_PART_FUNGUS  ,30,9,
                                                    {0,0,45,40,0,0,0,0,0,0,
                                                     0,30,0,0,0,0,0,0,60,0,
                                                     0,0,0,0,0,0,0,0,50,0,
                                                     0,0,0,0,0,15,0,0,0,0,
                                                     0,0,55,0,0,0,0,0,0},
                                                     PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                   {"spory charakterystyczny bia³y grzyb","sporego charakterystycznego bia³ego grzyba","sporemu charakterystycznemu bia³emu grzybowi","sporego charakterystycznego bia³ego grzyba","sporym charakterystycznym bia³ym grzybem","sporym charakterystycznym bia³ym grzybie",
                                                    "czerniak ko³pakowaty","czerniaka ko³pakowatego","czerniakowi ko³pakowatemu","czerniaka ko³pakowatego","czerniakiem ko³pakowatym","czerniaku ko³pakowatym",
                                                    "Jest to sporych rozmiarów grzyb. Posiada on charakterystyczne ciemne naloty, które idealnie kontrastuj± z jego bia³± barw±.",
                                                    10,PLANT_PART_FUNGUS  ,0,9,
                                                    {0,0,50,20,0,0,0,0,0,0,
                                                     0,30,70,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,60,0,0,0,0,0,0},
                                                     PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                   {"du¿y br±zowy grzyb wklês³y w ¶rodku","du¿ego br±zowego grzyba wklês³ego w ¶rodku","du¿emu br±zowemu grzybowi wklês³emu w ¶rodku","du¿ego br±zowego grzyba wklês³ego w ¶rodku","du¿ym br±zowym grzybem wklês³ym w ¶rodku","du¿ym br±zowym grzybie wklês³ym w ¶rodku",
                                                    "dwupier¶cieniak cesarski","dwupier¶cieniaka cesarskiego","dwupier¶cieniakowi cesarskiemu","dwupier¶cieniaka cesarskiego","dwupier¶cieniakiem cesarskiem","dwupier¶cieniaku cesarskim",
                                                    "Du¿y grzyb, który wydziela lekko kwa¶ny zapach. Jego kapelusz posiada charakterystyczny kszta³t kielicha, sp³aszczonego ku brzegom.",
                                                    32,PLANT_PART_FUNGUS  ,0,9,
                                                    {0,0,0,35,0,0,0,0,0,0,
                                                     0,35,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0},
                                                     PLANT_EFFECT_MEDIUM_FOOD,"opis smaku",	0,"opis palenia"},

                                                   {"niedu¿y grzyb z czerwonymi blaszkami","niedu¿ego grzyba z czerwonymi blaszkami","niedu¿emu grzybowi z czerwonymi blaszkami","niedu¿ego grzyba z czerwonymi blaszkami","niedu¿ym grzybem z czerwonymi blaszkami","niedu¿ym grzybie z czerwonymi blaszkami",
                                                    "borowik szatañski","borowika szatañskiego","borowikowi szatañskiemu","borowika szatañskiego","borowikiem szatañskim","borowiku szatañskim",
                                                    "Niedu¿y grzyb o kapeluszu w piêknym, jednolicie br±zowym kolorze. Stoi dumnie na solidnej, grubej nodze i kusi swymi czerwonymi blaszkami.",
                                                    116,PLANT_PART_FUNGUS  ,30,3,
                                                    {0,0,0,10,0,0,0,0,0,0,
                                                     0,20,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0},
                                                     PLANT_EFFECT_POISON_DEATH,"opis smaku",	0,"opis palenia"},

                                                   {"du¿y czerwony grzyb w bia³e kropki","du¿ego czerwonego grzyba w bia³e kropki","du¿emu czerwonemu grzybowi w bia³e kropki","du¿ego czerwonego grzyba w bia³e kropki","du¿ym czerwonym grzybem w bia³e kropki","du¿ym czerwonym grzybie w bia³e kropki",
                                                    "muchomor czerwony","muchomora czerwonego","muchomorowi czerwonego","muchomora czerwonego","muchomorem czerwonym","muchomorze czerwonym",
                                                    "Masz przed sob± du¿ego grzyba. Jego kapelusz jest w jednolitym, czerwonym kolorze w ca³o¶ci pokryty ³adnymi, bia³ymi plamkami. Dzielnie stoi na swej jasnej, pod³u¿nej nó¿ce i opiera siê przeciwno¶ci± lasu.",
                                                    145,PLANT_PART_FUNGUS  ,30,2,
                                                    {0,0,0,25,0,0,0,0,0,0,
                                                     0,55,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0,0,
                                                     0,0,0,0,0,0,0,0,0},
                                                     PLANT_EFFECT_POISON_WEAKEN,"opis smaku",	0,"opis palenia"},

                                                    {"du¿y br±zowy bezkszta³tny grzyb","du¿ego br±zowego bezkszta³tnego grzyba","du¿emu br±zowemu bezkszta³tnemu grzybowi","du¿ego br±zowego bezkszta³tnego grzyba","du¿ym br±zowym bezkszta³tnym grzybem","du¿ym br±zowym bezkszta³tnym grzybie",
                                                     "krowiak podwiniêty","krowiaka podwiniêtego","krowiakowi podwiniêtemu","krowiaka podwiniêtego","krowiakiem podwiniêtym","krowiaku podwiniêtym",
                                                     "Sporych rozmiarów br±zowy grzyb o bli¿ej nieokre¶lonych kszta³tach.",
                                                     36,PLANT_PART_FUNGUS  ,10,3,
                                                     {0,0,0,30,0,0,0,0,0,0,
                                                      0,60,0,0,0,0,0,0,0,0,
                                                      0,0,0,0,0,0,0,0,0,0,
                                                      0,0,0,0,0,0,0,0,0,0,
                                                      0,0,0,0,0,0,0,0,0},
                                                      PLANT_EFFECT_POISON_WEAKEN,"opis smaku",	0,"opis palenia"},

                                                     {"ma³y bia³y stercz±cy grzyb","ma³ego bia³ego stercz±cego grzyba","ma³emu bia³emu stercz±cemu grzybowi","ma³ego bia³ego stercz±cego grzyba","ma³ym bia³ym stercz±cym grzybem","ma³ym bia³ym stercz±cym grzybie",
                                                     	"muchomor jadowity","muchomora jadowitego","muchomorowi jadowitemu","muchomora jadowitego","muchomorem jadowitym","muchomorze jadowitym",
                                                     	"Malutki bia³y grzybek, wygl±daj±cy ca³kiem smakowicie.",
                                                     	29,PLANT_PART_FUNGUS  ,10,2,
                                                     	{0,0,0,40,0,0,0,0,0,0,
                                                     	 0,45,0,0,0,0,0,0,0,0,
                                                     	 0,0,0,0,0,0,0,0,0,0,
                                                     	 0,0,0,0,0,0,0,0,0,0,
                                                     	 0,0,0,0,0,0,0,0,0},
                                                     	 PLANT_EFFECT_POISON_SLEEP,"opis smaku",	0,"opis palenia"},

                                                      {"spory br±zowy grzyb","sporego br±zowego grzyba","sporemu br±zowemu grzybowi","sporego br±zowego grzyba","sporym br±zowym grzybem","sporym br±zowym grzybie",
                                                       "muchomor sromotnikowy","muchomora sromotnikowego","muchomorowi sromotnitkowemu","muchomora sromotnikowego","muchomorem sromotkinowym","muchomorze sromotnikowym",
                                                       "£adnych kszta³tów grzybek o szarym kapeluszu. Jego trzon jest niespotykanie gruby.",
                                                       36,PLANT_PART_FUNGUS  ,10,1,
                                                       {0,0,0,55,0,0,0,0,0,0,
                                                        0,45,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0},
                                                        PLANT_EFFECT_POISON_DEATH,"opis smaku",	0,"opis palenia"},

                                                      {"malutkie zielone p³atki","malutkich zielonych p³atków","malutkim zielonym p³atkom","malutkie zielone p³atki","malutkimi zielonymi p³atkami","malutkich zielonych p³atkach",
                                                       "zygotnik le¶ny","zygotnika le¶nego","zygotnikowi le¶nemu","zygotnika le¶nego","zygotnikiem le¶nym","zygotniku le¶nym",
                                                       "Malutkie p³atki jakiej¶ ro¶linki w bardzo ³adnym, jasnozielonym kolorze. S± niezwykle delikatne i pokryte delikatnym meszkiem.",
                                                       288,PLANT_PART_LEAF  ,50,9,
                                                       {0,0,0,30,0,0,0,0,0,0,
                                                        0,60,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0},
                                                        PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                      {"maleñkie zielone p³atki","maleñkich zielonych p³atków","maleñkim zielonym p³atkom","maleñkie zielone p³atki","maleñkimi zielonymi p³atkami","maleñkich zielonych p³atkach",
                                                       "zygotnik górski","zygotnika górskiego","zygotnikowi górskiemu","zygotnkika górskiego","zygotnika górskim","zygotniku górskim",
                                                       "Drobniuteñskie p³atki jakiej¶ ro¶linki w ca³o¶ci pokryte ciemnozielonym meszkiem.",
                                                       602,PLANT_PART_LEAF  ,70,9,
                                                       {0,0,0,0,40,60,0,0,0,0,
                                                        0,0,0,50,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,30,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0},
                                                        PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                      {"du¿y zgni³ozielony li¶æ","du¿ego zgni³ozielonego li¶cia","du¿emu zgni³ozielonemu li¶ciowi","du¿ego zgni³ozielonego li¶cia","du¿ym zgni³ozielonym li¶ciem","du¿ym zgni³ozielonym li¶ci",
                                                       "bagienne ziele","bagiennego ziela","bagiennemu zielowi","bagienne ziele","bagiennym zielem","bagiennym zielu",
                                                       "Du¿y li¶æ w kolorze zgni³ozielonym. Wydziela niespotykanie kwa¶ny zapach.",
                                                       89,PLANT_PART_LEAF  ,0,9,
                                                       {0,0,0,0,0,0,0,0,0,0,
                                                        0,0,80,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0,0,
                                                        0,0,0,0,0,0,0,0,0},
                                                        PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                       {"strza³kowate szare li¶cie","strza³kowatych szarych li¶ci","strza³kowatym szarym li¶ciom","strza³kowate szare li¶cie","strza³kowatymi szarymi li¶ciami","strza³kowatych szarych li¶ciach",
                                                        "szary poraz","szarego poraza","szaremu porazowi","szarego poraza","szarym porazem","szarym porazie",
                                                        "Niewielkie, strza³kowate li¶cie. Maj± szary kolor i s± niespotykanie g³adkie.",
                                                        66,PLANT_PART_LEAF  ,20,9,
                                                        {0,0,0,0,0,0,0,0,0,0,
                                                        	0,0,20,0,0,40,0,0,0,0,
                                                        	0,0,0,0,0,0,0,0,0,0,
                                                        	0,0,0,0,0,0,0,0,0,0,
                                                        	0,0,30,0,0,0,0,0,0},
                                                        	PLANT_EFFECT_PP_BLIND,"opis smaku",	0,"opis palenia"},

                                                        {"czarno-¿ó³te cuchn±ce li¶cie","czarno-¿ó³tych cuchn±cych li¶ci","czarno-¿ó³tym cuchn±cym li¶ciom","czarno-¿ó³te cuchn±ce li¶cie","czarno-¿ó³tymi cuchn±cymi li¶ciami","czarno-¿ó³tych cuchn±cych li¶ciach",
                                                        	"czarny ¿niwiarz","czarnego ¿niwiarza","czarnemu ¿niwiarzowi","czarnego ¿niwiarza","czarnym ¿niwiarzem","czarnym ¿niwiarzu",
                                                        	"Dziwnego kszta³tu li¶cie o czarno-zó³tym zabarwieniu. Wydzielaj± charakterystyczny, cuchn±cy odór i ca³e klej± siê od jakiej¶ lepkiej cieczy.",
                                                        	488,PLANT_PART_LEAF  ,50,0,
                                                        	{0,0,0,0,0,0,0,0,0,0,
                                                        	 0,10,0,0,0,0,0,0,0,0,
                                                        	 0,0,0,0,0,0,0,0,0,0,
                                                        	 0,0,0,0,0,0,0,0,0,0,
                                                        	 0,0,0,0,0,0,0,0,0},
                                                        	 PLANT_EFFECT_MINUS_BIG_HP,"opis smaku",	0,"opis palenia"},

                                                        {"czerwono-ziemisty mech","czerwono-ziemistego mchu","czerwono-ziemistemu mchowi","czerwono-ziemisty mech","czerwono-ziemistym mchem","czerwono-ziemistym mchu",
                                                        	"krwawy mech","krwawego mchu","krwawemu mchowi","krwawy mech","krwawym mchem","krwawym mchu",
                                                        	"Malutka kupka mchu w czerwono-ziemistym kolorze. Pachnie zupe³nie jak mu³!",
                                                        	86,PLANT_PART_BUSH  ,20,9,
                                                        	{0,0,0,20,0,0,0,0,0,0,
                                                        	 0,5,45,0,0,0,0,0,0,0,
                                                        	 0,0,0,0,0,0,0,0,0,0,
                                                        	 0,0,0,0,0,0,0,0,0,0,
                                                        	 0,0,25,0,0,0,0,0,0},
                                                        	 PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                         {"l¶ni±cy z³otem kwiat","l¶ni±cego z³otem kwiata","l¶ni±cemu z³otem kwiatowi","l¶ni±cego z³otem kwiat","l¶ni±cym z³otem kwiatem","l¶ni±cym z³otem kwiecie",
                                                          "szczyglik z³ocisty","szczyglika z³ocistego","szczyglikowi z³ocistemu","szczyglika z³oscistego","szczyglkiem z³ocistym","szczygliku z³ocistym",
                                                          "Przepiêkny dzwoniasty kwiat, który w ca³o¶ci mieni siê kolorem l¶ni±cego z³ota.",
                                                          31,PLANT_PART_FLOWER  ,10,9,
                                                          {0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,0,25,0,0,0,0,0,0,0,
                                                           0,0,35,0,0,0,0,0,0},
                                                           PLANT_EFFECT_ENDURE_POISON,"opis smaku",	0,"opis palenia"},

                                                         {"kolczasty zgnilozielony wodorost","kolczastego zgni³ozielonego wodorostu","kolczastemu zgni³ozielonemu wodorostowi","kolczastego zgni³ozielonego wodorosta","kolczastym zgni³ozielonym wodorostem","kolczastym zgni³ozielonym wodoro¶cie",
                                                          "morski kie³","morskiego k³a","morskiemu k³owi","morskiego k³a","morskim k³em","morskim kle",
                                                          "Bezkszta³tna ro¶linka z której wyrastaj± ma³e kolce o zgni³ozielonym kolorze.",
                                                          500,PLANT_PART_LEAF  ,60,9,
                                                          {0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,20,20,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           10,0,0,0,0,0,0,0,0},
                                                           PLANT_EFFECT_POISON_LEVEL20,"opis smaku",	0,"opis palenia"},

                                                         {"ma³y k³êbek bia³ych korzonków","ma³ego k³êbka bia³ych korzonków","ma³emu k³êbkowi bia³ych korzonków","ma³y k³êbek bia³ych korzonków","ma³ym k³êbkiem bia³ych korzonkó","ma³ym k³êbku bia³ych korzonków",
                                                          "wiatrorost","wiatrorosta","wiatrorostowi","wiatrorosta","wiatrorostem","wiatroro¶cie",
                                                          "Niewielka ilo¶æ poskrêcanych i pozwijanych bia³ych korzonków.",
                                                          39,PLANT_PART_ROOT  ,0,9,
                                                          {0,0,0,0,0,0,0,0,0,50,
                                                          15,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,10,15,0,5,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,10,0,0,0,0,0,0,0},
                                                           PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                          {"ma³a twarda ciernista ga³±zka","ma³ej twardej ciernistej ga³±zki","ma³ej twardej ciernistej ga³±zce","ma³± tward± ciernist± ga³±zkê","ma³± tward± ciernist± ga³±zk±","ma³ej twardej ciernistej ga³±zce",
                                                           "wilczy kie³","wilczego k³a","wilczemu k³owi","wilczego k³a","wilczym k³em","wilczym kle",
                                                           "Obro¶niêta malutkimi cierniami, niezwykle twarda ga³±zka.",
                                                           595,PLANT_PART_NONE ,70,9,
                                                           {0,0,0,0,0,0,0,0,0,0,
                                                            0,0,70,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0},
                                                            PLANT_EFFECT_MINUS_LITTLE_HP,"opis smaku",	0,"opis palenia"},

                                                         {"kolbowaty czarny grzyb","kolbowatego czarnego grzyba","kolbowatemu czarnemu grzybowi","kolbowatego czarnego grzyba","kolbowatym czarnym grzybem","kolbowatym czarnym grzybie",
                                                          "tymora czarna","tymory czarnej","tymorze czarnej","tymorê czarn±","tymor± czarn±","tymorze czarnej",
                                                          "W ca³o¶ci pokryty czarnym kolorem grzyb o kolbowatym kapeluszu i grubej nó¿ce.",
                                                          101,PLANT_PART_FUNGUS  ,20,7,
                                                          {0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,0,45,20,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0},
                                                           PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                          {"ma³e ¿ó³te parz±ce listki","ma³ych ¿ó³tych parz±cych listków","ma³ym ¿ó³tym parz±cym listkom","ma³e ¿ó³te parz±ce listki","ma³ymi parz±cymi listkami","ma³ych ¿ó³tych parz±cych listkach",
                                                           "¿a³obka popielna","¿a³obki popielatej","¿a³obce popielatej","¿a³obkê popielat±","¿a³obk± popielat±","¿a³obce popielatej",
                                                           "Ma³e, postrzêpione przy koñcach listki w ¿ó³tym kolorze.",
                                                           373,PLANT_PART_LEAF  ,60,6,
                                                           {0,0,0,0,0,0,0,0,0,0,
                                                           45,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0,20,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                           25,0,0,0,0,0,0,0,0},
                                                           PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

                                                          {"powykrêcane czarne ga³±zki","powykrêcanych czarnych ga³±zek","powykrêcanym czarnym ga³±zkom","powykrêcane czarne ga³±zki","powykrêcanymi czarnymi ga³±zkami","powykrêcanych czarnych ga³±zkach",
                                                           "czarnokrzew","czarnokrzewu","czarnekrzewowi","czarnokrzew","czarnokrzewem","czarnokrzewie",
                                                           "Pozwijane i poskrêcane w ró¿nych kierunkach czarne ga³±zki.",
                                                           92,PLANT_PART_NONE ,20,9,
                                                           {0,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,65,0,0,0,
                                                            0,0,0,0,0,0,25,25,0,0,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0},
                                                            PLANT_EFFECT_SHOCKING_GRASP,"opis smaku",	0,"opis palenia"},

                                                          {"piêkny wysoki z³oto-srebrny kwiat","piêknego wysokiego z³oto-srebrnego kwiata","piêknemu wysokiemu z³oto-srebrnemu kwiatowi","piêknego wysokiego z³oto-srebrnego kwiata","piêknym wysokim z³oto-srebrnym kwiatem","piêknym wysokim z³oto-srebrnym kwiacie",
                                                           "boski duszczyk","boskiego duszczyka","boskiemu duszczykowi","boskiego duszczyka","boskim duszczykiem","boskim duszczyku",
                                                           "Niezwykle du¿ych rozmiarów kwiat, który wydziela przyjemny, s³odki zapach. ¯ó³te p³atki idealnie kontrastuj± ze srebrnym wnêtrzem kwiatu.",
                                                           86,PLANT_PART_FLOWER  ,20,9,
                                                           {0,0,0,10,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                            0,0,0,0,0,0,0,0,0,0,
                                                            0,0,50,0,0,0,0,0,0},
                                                            PLANT_EFFECT_SLOW_POISON,"opis smaku",	0,"opis palenia"},

                                                           {"ciemnozielone pod³ó¿ne zio³o","ciemnozielonego pod³ó¿nego zio³a","ciemnozielonemu pod³ó¿nemu zio³u","ciemnozielone pod³ó¿ne zio³o","ciemnozielonym pod³ó¿nym zio³em ","ciemnozielone pod³ó¿ne ziele",
                                                           	"eathe arthis","eathe artis","eathe artis","eathe artis","eathe artis","eathe artis",
                                                           	"Zwyczajna prosta ro¶linka w zielonym kolorze. Nie wydziela prawie ¿adnego zapachu i zapewne smakuje równie nijako.",
                                                           	750,PLANT_PART_PERENNIAL  ,40,9,
                                                           	{0,0,0,20,0,0,0,0,0,0,
                                                           	 0,25,0,0,0,0,0,0,0,0,
                                                           	 0,0,0,0,0,0,0,0,0,0,
                                                           	 0,0,0,0,0,0,0,0,0,0,
                                                           	 0,0,10,0,0,0,0,0,0},
                                                           	 PLANT_EFFECT_HIGH_HP,"opis smaku",	0,"opis palenia"},

                                                            {"ma³y z³oty grzybek","ma³ego z³otego grzybka","ma³emu z³otemu grzybkowi","ma³ego z³otego grzybka","ma³ym z³otym grzybkiem","ma³ym z³otym grzybku",
                                                             "korronnik szlachetny","korronnika szlachetnego","korronnikowi szlachetnemu","korronnika szlachetnego","korronnikiem szlachetnym","korronniku szlachetnym",
                                                             "Malutki grzybek, którego kapelusz w z³otym kolorze prawie w ca³o¶ci przys³ania cieniutk± nó¿kê.",
                                                             160,PLANT_PART_FUNGUS  ,30,9,
                                                             {0,0,0,0,0,0,0,0,0,0,
                                                              0,10,0,0,0,0,0,0,0,0,
                                                              0,0,0,0,0,0,0,0,0,0,
                                                              0,0,0,0,0,0,0,0,0,0,
                                                              0,0,0,0,0,0,0,0,0},
                                                              PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},

                                                             {"du¿y bia³y grzyb","du¿ego bia³ego grzyba","du¿emu bia³emu grzybowi","du¿ego bia³ego grzyba","du¿ym bia³ym grzybem","du¿ym bia³ym grzybie",
                                                              "gromnik","gromnika","gromnikowi","gromnika","gromnikiem","gromniku",
                                                              "Sporych rozmiarów grzyb w bia³ym kolorze, gdzieniegdzie pokryty szarymi plamkami.",
                                                              27,PLANT_PART_FUNGUS  ,10,5,
                                                              {0,0,0,0,0,0,0,0,0,0,
                                                               0,0,0,0,20,5,0,40,0,0,
                                                               0,0,0,0,0,0,0,0,0,0,
                                                               0,0,0,0,0,0,0,0,0,0,
                                                               0,0,0,0,0,0,0,0,0},
                                                               PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},

                                                            {"zielony fosforyzuj±cy grzyb","zielonego fosforyzuj±cego grzyba","zielonemu fosforyzuj±cemu grzbowi","zielonego fosforyzuj±cego grzyba","zielonym fosforyzuj±cym grzybem","zielonym fosforyzuj±cym grzybie",
                                                             "szalarek zielony","szalarka zielonego","szalarkowi zielonemu","szalarka zielonego","szalarkiem zielonym","szalarku zielonym",
                                                             "Sporych rozmiarów zielony grzyb, w ca³o¶ci pokryty jak±¶ dziwn±, jasnozielon± wydzielin± o dziwnym zapachu i konstystencji.",
                                                             74,PLANT_PART_FUNGUS  ,20,9,
                                                             {0,0,0,0,0,0,0,0,0,0,
                                                              0,0,0,0,30,10,0,70,0,0,
                                                              0,0,0,0,0,0,0,0,0,0,
                                                              0,0,0,0,0,0,0,0,0,0,
                                                              0,0,0,0,0,0,0,0,0},
                                                              PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},

                                                         {"fioletowy fosforyzuj±cy grzyb","fioletowego fosforyzuj±cego grzyba","fioletowemu fosforyzuj±cemu grzbowi","fioletowego fosforyzuj±cego grzyba","fioletowym fosforyzuj±cym grzybem","fioletowym fosforyzuj±cym grzybie",
                                                          "szalarek fioletowy","szalarka fioletowego","szakarkowi fioletowemu","szalarka fioletowego","szalarkiem fioletowym","szalarku fioletowym",
                                                          "Ma³y grzybek, który w ca³o¶ci pokryty jest jak±¶ dziwn±, lepk± fioletow± ciecz±.",
                                                          125,PLANT_PART_FUNGUS  ,20,9,
                                                          {0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,5,0,0,20,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0,0,
                                                           0,0,0,0,0,0,0,0,0},
                                                           PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},

                                                         {"lekko fosforyzuj±ca ¿ó³ta ple¶ñ","lekko fosforyzuj±æej ¿ó³tej ple¶ni","lekko fosforyzuj±cej ¿ó³tej ple¶ni","lekko fosforyzuj±c± ¿ó³t± ple¶ñ","lekko fosforyzuj±c± ¿ó³t± ple¶ni±","lekko fosforyzuj±cej ¿ó³tej ple¶ni",
                                                         "z³otawka","z³otawki","z³otawce","z³otawkê","z³otawk±","z³otawce",
                                                         "Ple¶ñ w bardzo ³adnym, ¿ó³tym kolorze.",
                                                         323,PLANT_PART_NONE ,60,9,
                                                         {0,0,0,0,0,0,0,0,0,0,
                                                          0,5,0,0,0,0,0,25,0,0,
                                                          0,0,0,0,0,0,0,0,0,0,
                                                          0,0,0,0,0,0,0,0,0,0,
                                                          0,0,0,0,0,0,0,0,0},
                                                          PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},

{"zwyk³a szara ple¶ñ","zwyk³ej szarej ple¶ni","zwyk³ej szarej ple¶ni","zwykl± szar± ple¶ñ","zwyk³± szar± ple¶ni±","zwyk³ej szarej ple¶ni",	"ple¶ñ","ple¶ni","ple¶ni","ple¶ñ","ple¶ni±","ple¶ni",	"Ple¶ñ w brzydkim, szarym kolorze. Potwornie ¶mierdzi i jest dziwnie klej±ca.",	172,PLANT_PART_NONE ,50,8,	{30,30,0,20,0,0,10,10,0,0,	0,40,100,0,20,80,0,100,0,50,	70,20,10,0,0,0,0,0,0,0,	0,0,0,0,50,0,10,100,0,100,	0,10,0,0,0,0,0,0,0},	PLANT_EFFECT_MINUS_SMALL_THIRST,"opis smaku",	0,"opis palenia"},
{"ma³y skamienia³y grzybek","ma³ego skamienia³ego grzybka","ma³emu skamienia³emu grzybkowi","ma³ego smienia³ego grzybka","ma³ym skamienia³ym grzybkiem","ma³ym skamienia³ym grzybku",	"skamienia³y gambis","skamienia³ego gambisa","skamienia³emu gambisowi","skamienia³ego gambisa","skamienia³ym gambisem","skamienia³ym gambisie",	"Ma³y szary grzybek wygl±da jakby by³ z kamienia.",	614,PLANT_PART_FUNGUS  ,70,9,	{0,0,0,0,0,0,0,0,0,0,	0,0,0,30,10,40,0,20,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_MINUS_DEX_P10,"opis smaku",	0,"opis palenia"},
{"du¿y ognistoczerwony grzyb","du¿ego ognistoczerwonego grzyba","du¿emu ognistoczerwonemu grzybowi","du¿ego ognistoczerwonego grzyba","du¿ym ognistoczerwonym grzybem","du¿ym ognistoczerwonym grzybie",	"ognisty grzyb","ognistego grzyba","ognistemu grzybowi","ognistego grzyba","ognistym grzybem","ognistym grzybie",	"Du¿ych rozmiarów grzyb w ognistoczerwonym kolorze. S³upek, na którym opiera siê kapelusz jest niespotykanie gruby.",	62,PLANT_PART_FUNGUS  ,30,9,	{0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,80,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},
{"intensywnie b³êkitny porost","intenstywnie b³êkitnego porostu","intenstywnie b³êkitnemu porostowi","intensywnie b³êkitny porost","intensywnie b³êkitnym porostem","intensywnie b³êkitnym poro¶cie",	"porost lodowcowy","porosta lodowcowego","porostowi lodowcowemu","porost lodowcowy","porostem lodowcowym","poro¶cie lodowcowym",	"Dziwnych kszta³tów b³êkitny porost.",	332,PLANT_PART_BUSH  ,40,9,	{0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,20,0,0,0,	0,0,0,0,0,0,75,45,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},
{"ma³y kolczasty grzybek","ma³ego kolczastego grzybka","ma³emu kolczastemu grzybkowi","ma³ego kolczastego grzybka","ma³ym kolczastym grzybkiem","ma³ym kolczastym grzybku",	"¼dzierkoszczê¿yk","¼dzierkoszczê¿yka","¼dzierkoszczê¿ykowi","¼dzierkoszczê¿yka","¼dzierkoszczê¿ykiem","¼dzierkoszczê¿yku",	"Malutki grzybek w pomarañczowym kolorze. Z jego kapelusza wyrastaj± niewielkie igie³ki gotowe wbiæ siê we wszystko, co tylko siê do nich zbli¿y.",	481,PLANT_PART_FUNGUS  ,60,9,	{0,0,0,0,0,45,0,0,0,0,	0,0,0,40,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,45,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_HALLUCINATIONS,"opis smaku",	0,"opis palenia"},
{"du¿y czarny grzyb z brodawkami","du¿ego czarnego grzyba z brodawkami","du¿emu czarnemu grzybowi z brodawkami","du¿ego czarnego grzyba z brodawkami","du¿ym czarnym grzybem z brodawkami","du¿ym czarnym grzybie z brodawkami",	"karlin zabójczy","karlina zabójczego","karlinowi zabójczemu","karlina zabójczego","karlinem zabójczym","karlinie zabójczym",	"Du¿y grzyb w czarnym kolorze, w ca³o¶ci pokryty jest dziwnymi brodawkami.",	116,PLANT_PART_FUNGUS  ,30,2,	{0,0,0,0,0,0,0,0,0,0,	0,0,25,0,15,0,0,5,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_POISON_PARALIZING,"opis smaku",	0,"opis palenia"},
{"niewielki bezkszta³ty grzyb","niewielkiego bezkszta³tnego grzyba","niewielkiemu bezkszta³tnemu grzybowi","niewielkiego bezkszta³tnego grzyba","niewielkiem bezkszta³tnym grzybem","niewielkim bezkszta³tnym grzybie",	"boliar b³otny","boliara b³otnego","boliarowi b³otnemu","boliara b³otnego","boliarem b³otnym","boliarze b³otnym",	"Niewielki grzyb, którego kapelusz posiada bli¿ej nieokre¶lone kszta³ty.",	84,PLANT_PART_FUNGUS  ,10,9,	{0,0,0,0,0,0,0,0,0,0,	0,0,75,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},
{"maleñkie br±zowe grzybki","maleñkich br±zowych grzybków","maleñkim br±zowym grzybkom","maleñkie br±zowe grzybki","maleñkimi br±zowymi grzybkami","maleñkich br±zowych grzybkach",	"pod¶ciernik","pod¶ciernika","pod¶ciernikowi","pod¶ciernika","pod¶ciernikiem","pod¶cierniku",	"Malutkie grzybki w br±zowym kolorze o niezbyt przyjemnym zapachu.",	168,PLANT_PART_FUNGUS  ,30,9,	{0,0,10,25,40,20,0,0,0,0,	0,55,5,10,0,0,0,0,20,0,	0,0,0,0,0,0,0,0,20,0,	0,5,35,10,0,30,0,0,10,0,	0,0,5,0,0,0,0,0,0},	PLANT_EFFECT_SMALL_FOOD,"opis smaku",	0,"opis palenia"},
{"szara krzaczasta ple¶ñ","szarej krzaczastej ple¶ni","szarej krzaczastej ple¶ni","szar± krzaczast± ple¶ñ","szar± krzaczast± ple¶ni±","szarej krzaczastej ple¶ni",	"eppar nahor","eppar nahor","eppar nahor","eppar nahor","eppar nahor","eppar nahor",	"Szara, krzaczasta ple¶ñ o bardzo dziwnej konsystencji i jeszcze dziwniejszym zapachu.",	227,PLANT_PART_NONE ,50,9,	{0,0,0,0,0,0,0,0,0,0,	0,0,0,0,20,55,0,40,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},
{"malutki czarnoszary grzybek","malutkiego czarnoszarego grzybka","malutkiemu czarnoszaremu grzybkowi","malutki czarnoszary grzybek","malutkim czarnoszarym grzybkiem","malutkim czarnoszarym grzybku",	"eaghar ort'gha","eaghar ort'gha","eaghar ort'gha","eaghar ort'gha","eaghar ort'gha","eaghar ort'gha",	"Niespotykanie malutki grzybek w czarnoszarym kolorze.",	84,PLANT_PART_FUNGUS  ,20,9,	{0,0,0,0,0,0,0,0,0,0,	0,0,0,0,45,20,0,35,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0,0,	0,0,0,0,0,0,0,0,0},	PLANT_EFFECT_NONE,"opis smaku",	0,"opis palenia"},

	{"unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
		    "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
		    "unknown",
		    0, 0, 0, 10,
		    {0, 0, 0, 0, 0,       //to prawdopodobienstwa w sectorach, ida po kolei jak w sector_table
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0,
		     0, 0, 0, 0, 0},          //jest 45 czyli na zapas...
		    0, "unknown",
		    0, "unknown"
	    },



};

//rellik: carve, czesci ciala do wyciecia przez carve
//uwaga: jesli dodajesz cos z vnum i taka sama rasa bez vnum to z vnum musi byc przed rasa inaczej wezmie zawsze wpis z rasa. Tak jak niedzwiedz.
const 	struct monster_body_parts	body_parts_table	[]		=
{
    /* PROSZE NIE ZMIENIAC KOLEJNOSCI */
    /*
       {
       keyword, dope³niacz, celownik, biernik, narzêdnik, miejscownik,
       mianownik, long_desc, item_desc,
       race, vnum, identyfikator czesci dla danej rasy, difficult, chance, cost, gender, czy_niszczy_skórê, czy_ma_funkcje, kolor, czy pokazywaæ ¿e do wyciêcia
       }, komentarz: je¶li vnum=0 szuka po race jesli !=0 to musi sie vnum zgadzac
     */
    { // 0
        "³uska", "{G³uski smoka{x", "{G³usce smoka{x", "{G³uske smoka{x", "{G³uska smoka{x", "{G³usce smoka{x",
        "{G³uska smoka{x", "Ogromna {G³uska{x smoka", "Ogromna {G³uska{x smoka",
        "smok", 0, A, 60, 30, 12000, SEX_FEMALE, TRUE, FALSE, "{G", TRUE
    },
    { // 1
        "kie³ kielsmoka", "{Wk³a smoka{x", "{Wk³owi smoka{x", "{Wkie³ smoka{x", "{Wk³em smoka{x", "{Wkle smoka{x",
        "{Wkie³ smoka{x", "{WKie³{x smoka", "Ogromny polyskuj±cy jadem {Wkie³{x smoka",
        "smok", 0, B, 80, 70, 15000, SEX_MALE, FALSE, TRUE, "{W", FALSE
    },
    { // 2
        "pazur", "{rpazura smoka{x", "{rpazurowi smoka{x", "{rpazur smoka{x", "{rpazurem smoka{x", "{rpazurze smoka{x",
        "{rpazur smoka{x", "{rPazur{x smoka",	"Ogromny ostry {rpazur{x smoka",
        "smok", 0, C, 50, 50, 10000, SEX_MALE, FALSE, TRUE, "{r", TRUE
    },
    { // 3
        "flaki", "{yflaków{x", "{yflakom{x", "{yflaki{x", "{yflakami{x", "{yflakach{x",
        "{yflaki{x", "Rozwalone {yflaki{x, niesmaczny widok",	"Cuchn±ce, paruj±ce {yflaki{x, od biedy pewnie mo¿na to zje¶æ.",
        "pies", 0, A, 10, 100, 1, SEX_NEUTRAL, TRUE, TRUE, "{y", TRUE
    },
    { // 4
        "genitalia", "{wgenitaliów{x", "{wgenitaliom{x", "{wgenitalia{x", "{wgenitaliami{x", "{wgenitaliach{x",
        "{wgenitalia{x", "Jaki¶ pechowiec straci³ swoje {wgenitalia{x",	"Chyba mo¿esz u¿yæ tego jako bardzo sugestywnego naszyjnika",
        "czlowiek", 6030, A, 20, 60, 10, SEX_NEUTRAL, FALSE, TRUE, "{w", FALSE
    },
    { // 5 Ogromny niedzwiedz
        "ucho", "{wucha nied¼wiedzia{x", "{wuchu nied¼wiedzia{x", "{wucho nied¼wiedzia{x", "{wuchem nied¼wiedzia{x", "{wuchu nied¼wiedzia{x",
        "{wucho nied¼wiedzia{x", "Nied¼wied¼, który by³ w³a¶cicielem tego {wucha{x, musia³ byæ ogromny", "Tak, to by³ wyj±tkowy {wnied¼wied¼{x, teraz zosta³o tylko {wucho{x.",
        "nied¼wied¼", 7805, A, 40, 80, 60, SEX_NEUTRAL, FALSE, TRUE, "{w", FALSE
    },
    { // 6 reszta niedzwiedzi
        "ucho", "{yucha nied¼wiedzia{x", "{yuchu nied¼wiedzia{x", "{yucho nied¼wiedzia{x", "{yuchem nied¼wiedzia{x", "{yuchu nied¼wiedzia{x",
        "{yucho nied¼wiedzia{x", "Nied¼wiedzie {yucho{x",	"{yUcho nied¼wiedzia{x, na pami±tkê.",
        "nied¼wied¼", 0, A, 40, 80, 60, SEX_NEUTRAL, FALSE, TRUE, "{y", TRUE
    },
    { // 7
        "ogon kita", "{Rogona lisa{x", "{Rogonowi lisa{x", "{Rogon lisa{x", "{Rogonem lisa{x", "{Rogonie lisa{x",
        "{Rogon lisa{x", "Lisia kita poniewiera siê tutaj na ziemi.", "Piêkna lisia {Rkita{x.",
        "lis", 0, A, 20, 70, 3000, SEX_MALE, FALSE, TRUE, "{R", TRUE
    },
    { // 8
        "³apka zajêcza zajeczalapka", "{W³apki zajêczej{x", "{W³apce zajêczej{x", "{W³apka zajêcza{x", "{W³apk± zajêcz±{x", "{W³apce zajêczej{x",
        "{W³apka zajêcza{x", "Na ziemi le¿y sobie zajêcza ³apka", "{WZajêcza ³apka{x, pewnie przynosi szczê¶cie.",
        "zaj±c", 0, A, 15, 70, 60, SEX_FEMALE, TRUE, TRUE, "{W", TRUE
    },
    { // 9
        "serce kolosa", "{sserca kolosa{x", "{ssercu kolosa{x", "{sserce kolosa{x", "{ssercem kolosa{x", "{ssercu kolosa{x",
        "{sserce kolosa{x", NULL, NULL,
        "umberkolos", 0, A, 25, 65, 10, SEX_NEUTRAL, TRUE, TRUE, "{s", TRUE
    },
    { // 10
        "piszczel", "{Wpiszczeli ogra{x", "{Wpiszczeli ogra{x", "{Wpiszczel ogra{x", "{Wpiszczel± ogra{x", "{Wpiszczeli ogra{x",
        "{Wpiszczel ogra{x", "Ogromna {Wpiszczel{x ogra", "Ogromna {Wpiszczel{x wyrwana z jakiego¶ ogra, wygl±da jak ca³kiem s³uszna maczuga.",
        "ogr", 0, A, 45, 40, 100, SEX_FEMALE, TRUE, TRUE, "{W", TRUE
    },
    { // 11
        "rogi demona", "{Rrogów{x {rdemona{x.", "{Rrogom{x {rdemona{x", "{Rrogi{x {rdemona{x.", "{Rrogami{x {rdemona{x.", "{Rrogach{x {rdemona{x.",
        "{Rrogi{x {rdemona{x", "Zakrwawione {Rrogi{x {rdemona{x.", "Te rogi musia³y nale¿eæ kiedy¶ do bardzo potê¿nego i specyficznego stworzenia. Wydaje ci siê, ¿e nadal s± nasi±kniête jak±¶ moc±. Wa¿± ponad 5 kilogramów. By³yby wspania³ym trofeum dla ka¿dego bohatera. Chyba, ¿e znajdziesz dla nich jakie¶ inne zastosowanie...",
        "demon", 2891, A, 50, 50, 22000, SEX_NEUTRAL, FALSE, TRUE, "{R", FALSE
    },
    { // 12
        "szpony gryfa", "{Wszponow {Dgryfa{x", "{Wszponom {Dgryfa{x", "{Wszpony {Dgryfa{x", "{Wszponami{Dgryfa{x", "{Wszponach {Dgryfa{x",
        "{Wszpony {Dgryfa{x", "{Wszpony {Dgryfa{x ze zwisaj±cymi strzêpkami skóry", "Szpony na tyle ostre, ¿e ciê¿ko je podnie¶æ bez kaleczenia siê. Ich rozmiar i kszta³t dobitnie ¶wiatczy o potêdze stworzenia, z cia³a którego zosta³y wyciête. Sporo wa¿± i wydaj± siê byæ twardsze ni¿ stal. Gdy ich dotykasz czujesz lekkie mrowienie d³oni. Wspania³e trofeum dla prawdziwych ³owców.",
        "gryf", 42820, A, 80, 70, 12000, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },
    { //13
        "skrzydlo nietoperza skrzydlo-nietoperza", "{sskrzyd³a{x nietoperza", "{sskrzyd³u{x nietoperza", "{sskrzyd³o{x nietoperza", "{sskrzyd³em{x nietoperza", "{sskrzyd³u{x nietoperza",
        "{sskrzyd³o{x nietoperza", "Obciête {sskrzyd³o{x nietoperza", "{sSkrzyd³o{x obciête jakiemu¶ nietoperzowi. Ciekawe po co to komu.",
        "nietoperz", 0, A, 1, 95, 10, SEX_NEUTRAL, TRUE, TRUE, "{s", TRUE
    },
    { //14
        "ogon szczura ogon-szczura", "szczurzego {wogona{x", "szczurzemu {wogonowi{x", "szczurzy {wogon{x", "szczurzym {wogonem{x", "szczurzym {wogonie{x",
        "szczurzy {wogon{x", "Obciêty szczurzy {wogon{x", "{sOgon{x, wygl±da na sczurzy.",
        "szczur", 0, A, 1, 95, 10, SEX_NEUTRAL, FALSE, TRUE, "{w", TRUE
    },
    { //15
        "rogjednorozca róg jednoro¿ca", "{Wroga jednoro¿ca{x", "{Wrogu jednoro¿ca{x", "{Wróg jednoro¿ca{x", "{Wrogiem jednoro¿ca{x", "{Wrogu jednoro¿ca{x",
        "{Wróg jednoro¿ca{x", "Obciêty jednoro¿cowi {Wróg{x", "D³ugi {Wróg jednoro¿ca{x ¶wiec±cy bladym blaskiem.",
        "jednoro¿ec", 0, A, 70, 50, 22000, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },
    { //16
        "k³y wilka", "{Wk³ów wilka{x", "{Wk³om wilka{x", "{Wk³y wilka{x", "{Wk³ami wilka{x", "{Wk³ach wilka{x",
        "{Wk³y wilka{x", "Piêkne, ostre wilcze {Wk³y{x", "Ostre jak brzytwa {Wk³y{x wilka.",
        "wilk", 0, A, 40, 70, 4500, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },
    { //17
        "ogon wywerny", "{gogona wywerny{x", "{gogonowi wywerny{x", "{gogon wywerny{x", "{gogonem wywerny{x", "{gogonowi wywerny{x",
        "{gogon wywerny{x", "D³ugi {gogon{x zakoñczony jadowitym grotem.", "D³ugi {gogon{x zakoñczony jadowitym grotem.",
        "wywerna", 0, A, 50, 70, 9000, SEX_MALE, FALSE, TRUE, "{g", TRUE
    },
    { //18
        "pazury niedzwiedzia", "{wpazurów nied¼wiedzia{x", "{wpazurom nied¼wiedzia{x", "{wpazury nied¼wiedzia{x", "{wpazurami nied¼wiedzia{x", "{wpazurach nied¼wiedzia{x",
        "{wpazury nied¼wiedzia{x", "Pazury, s±dz±c po wielko¶ci i kszta³cie - nied¼wiedzie.", "Pazury, s±dz±c po wielko¶ci i kszta³cie - nied¼wiedzie.",
        "nied¼wied¼", 0, B, 30, 70, 4500, SEX_NEUTRAL, FALSE, TRUE, "{w", TRUE
    },
    { //19
        "k³y tyranozaura", "{Wk³ów tyranozaura{x", "{Wk³om tyranozaura{x", "{Wk³y tyranozaura{x", "{Wk³ami tyranozaura{x", "{Wk³ach tyranozaura{x",
        "{Wk³y tyranozaura{x", "Olbrzymie k³y tyranozaura.", "Olbrzymie k³y, wyciête z tyranozaura.",
        "gad", 3049, A, 30, 90, 360000, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },
    { //20
        "k³y likantropa", "{Wk³ów likantropa{x", "{Wk³om likantropa{x", "{Wk³y likantropa{x", "{Wk³ami likantropa{x", "{Wk³ach likantropa{x",
        "{Wk³y likantropa{x", "Ostre jak brzytwy k³y likantropa.", "Niesamowicie ostre k³y, wyciête z likantropa.",
        "likantrop", 0, A, 30, 75, 15000, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },
    { //21
        "serce behemota sercebehemota", "{Rserca{x {rbehemota{x", "{Rsercu{x {rbehemota{x", "{Rserce{x {rbehemota{x", "{Rsercem{x {rbehemota{x", "{Rsercu{x {rbehemota{x",
        "{rserce{x {Rbehemota{x", "Stale ociekaj±ce {rkrwi±{x serce potê¿nego behemota.", "Serce wielko¶ci g³owy, najprawdopodobniej wyciête z jakiego¶ mitycznego potwora",
        "behemot", 17929, A, 60, 55, 36000, SEX_NEUTRAL, TRUE, TRUE, "{R", TRUE
    },
    { //22
        "serce zielonego smoka sercesmoka", "{rserca{x {Gzielonego smoka{x", "{rsercu{x {Gzielonego smoka{x", "{Rserce{x {Gzielonego smoka{x", "{Rsercem{x {Gzielonego smoka{x", "{Rsercu{x {Gzielonego smoka{x",
        "{rserce{X {Gzielonego smoka{x", "Nadal pulsuj±ce niesamowit± moc± serce smoka.", "Pulsuj±ce magi± serce wyciête ze smoka.",
        "smok", 9320, A, 70, 45, 86000, SEX_NEUTRAL, TRUE, TRUE, "{G", FALSE
    },
    { //23
        "poro¿e", "poro¿a", "poro¿u", "poro¿e", "poro¿em", "poro¿u",
        "poro¿e", "Na ziemi le¿y poro¿e jelenia", "Obracasz w d³oni wyciête poro¿e jelenia, trofeum w sam raz nadaj±ce siê by powiesiæ je na ¶cianie.",
        "jeleñ", 0, A, 30, 75, 3000, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },
    { // 24
        "królicza ³apka", "{Wkróliczej ³apki{x", "{Wkróliczej ³apce{x", "{Wkrólicza ³apka{x", "{Wkrólicz± ³apk±{x", "{Wkróliczej ³apce{x",
        "{Wkrólicza ³apka{x", "Na ziemi le¿y w kurzu królicza ³apka." ,"Królicza ³apka wygl±da normalnie.",
        "królik", 0, A, 30, 70, 60, SEX_FEMALE, FALSE, TRUE, "{W", TRUE
    },
    { // 25
        "trupi palec trupi-palec", "{Wtrupiego palca{x", "{Wtrupiemu palcu{x", "{Wtrupi palec{x", "{Wtrupim palcem{x", "{Wtrupim palcu{x",
        "{Wtrupi palec{x", "Le¿y tutaj palec jakiego¶ trupa." ,"Widzisz tutaj palec, oderwany z jakiego¶ trupa.",
        "zombi", 0, A, 10, 80, 1, SEX_MALE, FALSE, TRUE, "{W", TRUE
    },

    { // 26
        "szpony ghula szpony-ghula", "{yszponów {cghula{x", "{yszponom {cghula{x", "{yszpony {cghula{x", "{yszponami {cghula{x", "{yszponach {cghula{x",
        "{yszpony {cghula{x", "Ociekajace smierdzaca mazia szpony leza tutaj." ,"Niewielkie szpony, ca³e pokrytê mazia, która powoduje, ¿e chce ci siê rzygac przy ka¿dym spojrzeniu na nie.",
        "ghul", 0, A, 15, 80, 1, SEX_FEMALE, FALSE, TRUE, "{y", TRUE
    },

    { // 27
        "szpony ghasta szpony-ghasta", "{yszponów {cghasta{x", "{yszponom {cghasta{x", "{yszpony {cghasta{x", "{yszponami {cghasta{x", "{yszponach {cghasta{x",
        "{yszpony {cghasta{x", "Ociekajace smierdzaca mazia szpony leza tutaj." ,"Niewielkie szpony, ca³e pokrytê mazia, która powoduje, ¿e chce ci siê rzygac przy ka¿dym spojrzeniu na nie.",
        "ghast", 0, A, 20, 80, 1, SEX_FEMALE, FALSE, TRUE, "{y", TRUE
    },

    { //28
        "czaszka licza czaszka-licza", "{Wczaszki licza{x", "{Wczaszce licza{x", "{Wczaszka licza{x", "{Wczaszk± licza{x", "{Wczaszce licza{x",
        "{Wczaszka licza{x", "Widzisz tutaj wiekow±, niemal czarn± czaszkê.", "Czaszka, ze staro¶ci i od walania siê w brudzie znacznie poczernia³a. Potyliczna cze¶æ jest nieco peknieta, dok³adnie tam gdzie brakujê kawa³ka kosci.",
        "licz", 0, A, 30, 75, 0, SEX_FEMALE, FALSE, TRUE, "{W", TRUE
    },

    { //29
        "k³y wampira k³y-wampira", "{Dwampirzych k³ów{x", "{Dwampirzym k³om{x", "{Dwampirze k³y{x", "{Dwampirzymi k³ami{x", "{Dwampirzych k³ach{x",
        "{Dwampirze k³y{x", "Ostre jak brzytwy k³y le¿± tutaj.", "Niesamowicie ostre k³y, wyciête jakiemu¶ wampirowi.",
        "wampir", 0, A, 35, 75, 9500, SEX_NEUTRAL, FALSE, TRUE, "{W", TRUE
    },


    {
        NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL,
        NULL, 0, 0, 0, 0, 0, 0, FALSE, FALSE, NULL
    },
};



const	struct	trick_type	trick_table	[MAX_TRICKS]	=
{

/*struct   trick_type
{
    char*	name; nazwa trika
    int		which_skill; ktorego skilla jest to trick ( w sn )
    int		which_skill2; jak -1 to nic nie robi, w przypadku gry trik ma wymagac dwoch skilli, np dragon strike 'spear' i 'twohanded weapon'
    int		alternate_skill1;//jak -1 to nic, skill, ktory jest alternatywa dla which_skill
    int		skill_percent; na ile musisz skilla wytrenowac by sie mozna bylo tricka nauczyc
    TRICK_FUN* trick_fun; sama funkcja
    int		frequency; na ile tickow gwiazdkuje <- to jeszcze nie dziala
    int		chance;  jaka szansa ze zaskoczy przy uzyciu skilla (w dziesiatych procenta!)
    int		learn_freq; co ile tickow mozna probowac introduce <- to jeszcze nie dziala


};*/
//NIE ZMIENIAC KOLEJNOSCI!
	{//0
	"reserved",
	0,
	-1,
	-1,
	0,
	trick_null,
	0,
	0,
	0,
	},

	{//1
	"vertical kick",
	119,//sn kicka
	-1,//nie wymaga innych skilli
	-1,//i nie ma alternatywnego
	85,//trza miec na 85%
	trick_vertical_kick,//wywolywana funkcyjka
	7,//jak juz zaskoczy to przez nastepne 7 tickow nie zaskoczy)
	45,//4,5% ze zaskoczy przy uzyciu skilla kick
	8,//co 8 tickow mozna probowac introduce
	},

	{//2
	"entwine",
	112,//whip
	-1,
	-1,
	80,
	trick_entwine,
	1,
	45,//4,5% na kazde trafienie whipem
	6,
	},

	{//3
	"riposte",
	120, //parry
	-1,
	-1,
	85,
	trick_riposte,
	1,
	53,//5,3% na kazde udane parry
	8,
	},

	{//4
	"cyclone",
	211, //dualwield
	-1,
	-1,
	85,
	trick_cyclone,
	10,
	15,//1,5% na kazda runde walki walczac dualwieldem
	10,
	},

	{//5
	"flabbergast",
	114, //bash
	-1,
	-1,
	75,
	trick_flabbergast,
	3,
	95,//9,5% na kazdego udanego basha
	6,
	},

	{//6
	"staff swirl",
	108, //staff
	187, //twohanded weapon
	-1,
	75,
	trick_staff_swirl,
	3,
	60,//6% na kazda runde walki uzywajac 2h staffa
	8,
	},

	{//7
	"dragon strike",
	110, //spear
	187, //twohanded weapon
	-1,
	91,
	trick_dragon_strike,
	5,
	20,//2% na kazda runde walki uzywajac 2h wlocznie (zamiast rundy walki robi cos milego)
	10,
	},

	{//8
	"decapitation",
	103, //axe
	238, //critical strike
	-1,
	91,
	trick_decapitation,
	8,
	10,//1% przy udanym trafionym critical striku jakimkolwiek toporem
	10,
	},

	{//9
	"thundering whack",
	209, //stun
	-1,
	-1,
	85,
	trick_thundering_whack,
	3,
	40,//4% przy jakimkolwiek stunie
	6,
	},

	{//10
	"strucking wallop",
	173, //charge
	283, //mighty blow
	-1,
	85,
	trick_strucking_wallop,
	3,
	75,//7,5% (przy szarzy wywracajacej only)
	8,
	},

	{//11
	"shove",
	117, //dodge
	122, //trip
	-1,
	75,
	trick_shove,
	2,
	70,//7% przy udanym dodge
	6,
	},

	{//12
	"ravaging orb",
	105, //flail
	187, //twohanded weapon
	-1,
	80,
	trick_ravaging_orb,
	3,
	25,//2,5% na kazda runde walki 2h flailem (zamiast rundy walki robi cos milego)
	8,
	},

	{//13
	"bleed",
	111, //sword
	-1,
	188, //short-sword
	80,
	trick_bleed,
	2,
	18,//1,8% przy kazdym trafieniu swordem badz short swordem
	8,
	},

	{//14
	"thigh jab",
	104, //dagger
	-1,
	-1,
	80,
	trick_thigh_jab,
	3,
	18,//1,8% przy kazdym trafieniu sztyletem
	8,
	},

	{//15
	"weapon wrench",
	385, //whip mastery
	116, //disarm
	-1,
	75,
	trick_weapon_wrench,
	3,
	25,//2,5% na wejscie przy zwyklym ciosie, 25% na wejscie jak ktos proboje parowac kogos z tym trikiem
	8,
	},

	{//16
	"crushing mace",
	106, //mace
	274, //mace mastery
	-1,
	80,
	trick_crushing_mace,
	3,
	20,//2% na wejscie przy zwyklym ciosie macka
	8,
	},

	{//17
	"thousandslayer",
	425, //cleave
	424, //overwhelming
	-1,
	75,
	trick_thousandslayer,
	3,
	150,//15% na wejscie przy cleavie
	8,
	},

	{//18
	"glorious impale",
	110, //spear mastery
	187, //twohanded weapon
	-1,
	82,
	trick_glorious_impale,
	5,
	16,//1,6% na kazdy trafiony cios 2h wlocznia
	10,
	},

	{//19
	"divine impact",
	225, //smite evil
	-1,
	430,//smite good
	75,
	trick_divine_impact,
	5,
	48,//4,8% przy smicie
	10,
	},

	{//20
	"dreadful strike",
	424, //overwh
	423, //torment
	-1,
	75,
	trick_dreadful_strike,
	5,
	40,//4% przy ciosie overwhelmingiem kiedy ma sie naladowanego tormenta
	10,
	},

	{//21
	"rampage",
	239, //damage reduction
	115, //berserk
	-1,
	75,
	trick_rampage,
	5,
	5,//0,5% przy otrzymywaniu obrazen
	10,
	},

	{//22
	"enlightenment",
	126, //meditation
	-1,
	-1,
	80,
	trick_enlightenment,
	3,
	50,//5% po zapamiêtaniu czaru
	10,
	},
};

//rellik: komponenty, struktura wi±¿±ca czar z komponentem.
const struct spell_items_type spell_items_table[] =
{
    //kolejno¶æ hmm ... nieistotna; dobrze pisaæ w grupach sn
    //ilo¶æ wszystkich komponentów ograniczona rozmiarem bigflag (w tej chwili jest 10 intów wiêc daje 320 warto¶ci, w razie potrzeby zwiêkszyæ
    //maksymalnie 99 komponentów per sn, max 30 grup per sn+nazwa_czaru, max 30 elementów w grupie
    /*
       numer komponentowo-spellowy - MUSI BYÆ UNIKATOWY I NIE MO¯E BYÆ 0!!! (u¿ywany do kojarzenia komponentu z wiedz± o nim postaci oraz do okre¶lania funkcji specjalnych po wyczerpaniu)
       					poniewa¿ kolejno¶æ jest dowolna to jest to najpewniejszy identyfikator tandemu komponent-spell
       numer spella,
       wewn. nazwa spella,
       nazwa komponentu do u¿ytkownika,
       nazwa komponentu (istniej±ca w name przedmiotu),
       opis podczas wykorzystywania
       czy wybucha po roz³adowaniu (specjalne rzeczy w magic.c=>spell_item_destroy)
       czy wybucha po skoñczeniu timera (specjalne rzeczy w magic.c=>spell_item_destroy)
       czy posiada specjaln± funkcjê sprawdzaj±c±
       numer grupy ( od 1 do 30 ) - je¶li 0 to nie jest elementem grupy, max 30 grup per sn i 30 komponentow w grupie
     */
    {  1,  94, "summon PC", "kielsmoka", "Kie³ smoka", "Czubek k³a l¶ni b³êkitnym ¶wiat³em gdy kumuluje siê na nim energia, po czym jej w±ska wi±zka wystrzeliwuje w górê.", TRUE, FALSE, FALSE, 0 },
    {  2,  63, "ident jak poznaniak", "lisc-zwyczajny", "{gli¶æ{x", "{gLi¶æ{x zwija siê i kurczy.", TRUE, FALSE, FALSE, 1 },
    {  3,  63, "ident jak poznaniak", "skrzydlo-nietoperza", "{sskrzyd³o{x nietoperza", "{sskrzyd³o{x nietoperza trzepocze po¶miertnym skurczem miê¶ni.", TRUE, FALSE, FALSE, 1 },
    {  4,  63, "ident jak poznaniak", "ogon-szczura", "szczurzy {wogon{x", "szczurzy {wogon{x wyprê¿a siê.", TRUE, FALSE, FALSE, 1 },
    {  5,  63, "ident jak poznaniak", "nefryt", "{Gnefryt{x", "Przez chwilê zieleñ {gnefrytu{x staje siê g³êbsza.", FALSE, FALSE, FALSE, 1 },
    {  6, 278, "raise ghoul", "szpony-ghula", "{bszpony{x {sghula{x", "{bSzpony{x {sghula{x wbijaj± siê g³êboko w cia³o.", TRUE, TRUE, FALSE, 0 },
    {  7, 279, "raise ghast", "szpony-ghasta", "{bszpony{x {sghasta{x", "{bSzpony{x {sghasta{x wbijaj± siê g³êboko w cia³o.", TRUE, TRUE, FALSE, 0 },
    {  8,  92, "mirror", "lustro-mirror", "{Clustro{x", "Czar odbija siê w {Clustrze{x powoduj±c powstanie dodatkowych odbiæ.", TRUE, FALSE, FALSE, 0 },
    {  9, 258, "raise zombie", "trupi-palec", "{strupi palec{x", "{sTrupi palec{x przy³o¿ony do cia³a wrasta i zdaje siê je o¿ywiaæ.", TRUE, TRUE, FALSE, 0 },
    { 10, 281, "orb of entropy", "kamienny-pierscien", "{wkamienny pier¶cieñ{x", "{wkamienny pier¶cieñ{x dr¿y przepe³niony moc±.{x", TRUE, FALSE, TRUE, 0 },
    { 11, 135, "liveoak slaby", "drobne-kawalki-kory", "{ydrobne kawa³ki kory{x", "Z {ydrobnych kawa³ków kory{x tworzy siê poka¼ne drzewo.", TRUE, FALSE, FALSE, 0 },
    { 12, 135, "liveoak mocny", "kawalki-kory", "{ykawa³ki kory{x", "Z {ykawa³ków kory{x tworzy siê ogromne drzewo.", TRUE, FALSE, FALSE, 0 },
    /* druid: Sn: 368 Spell: ring of vanion */
    { 13, 368, "ring of vanion", "liscwielkiegodebu", "{Gwielki zielony lisc{x", "{GWielki zielony li¶æ{x traci swój kolor, robi siê coraz cieñszy, po czym rozpada siê bez ¶ladu.", TRUE, TRUE, FALSE, 0 },
    { 14, 368, "ring of vanion", "zoladzwielkiegodebu", "{Wsrebrzysto po³yskuj±cy, ma³y ¿o³±d¼{x", "{SSrebrzysto po³yskuj±cy ¿o³±d¼{x kurczy siê, po czym znika z cichym trzaskiem.", TRUE, FALSE, FALSE, 0 },
    /* druid: Sn: 369 Spell: circle of vanion */
    { 15, 369, "circle of vanion", "liscwielkiegodebu", "{Gwielki zielony li¶æ{x", "{GWielki zielony li¶æ{x traci swój kolor, robi siê coraz cieñszy, po czym rozpada siê bez ¶ladu.", TRUE, TRUE, FALSE, 1 },
    { 16, 369, "circle of vanion", "zoladzwielkiegodebu", "{Wsrebrzysto po³yskuj±cy, ma³y ¿o³±d¼{x", "{SSrebrzysto po³yskuj±cy ¿o³±d¼{x kurczy siê, po czym znika z cichym trzaskiem.", TRUE, FALSE, FALSE, 1 },
    /* druid: Sn: 241 Spell: alicorn lance */
    { 17, 241, "alicorn lance", "rogjednorozca", "{Wróg jednorozca{x", "Zawarta w rogu magiczna moc jednoro¿ca uwalnia siê.", TRUE, FALSE, FALSE, 0 },
    /* Sn:   1 Spell: acid blast                       Szkola: Inwokacje     Profesje: Mag*/
    {  18,   1, "acid based spell", "buteleczkazkwasem", "{gbuteleczka z kwasem{x", "{gKropla kwasu znika w powietrzu{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 175 Spell: resist acid                      Szkola: Odrzucanie    Profesje: Mag Kleryk Druid*/
    {  19, 175, "acid based spell", "buteleczkazkwasem", "{gbuteleczka z kwasem{x", "{gKropla kwasu znika w powietrzu{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 193 Spell: acid hands                       Szkola: Przemiany     Profesje: Mag*/
    {  20, 193, "acid based spell", "buteleczkazkwasem", "{gbuteleczka z kwasem{x", "{gKropla kwasu znika w powietrzu{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 268 Spell: endure acid                      Szkola: Odrzucanie    Profesje: Mag Kleryk Druid*/
    {  21, 268, "acid based spell", "buteleczkazkwasem", "{gbuteleczka z kwasem{x", "{gKropla kwasu znika w powietrzu{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 405 Spell: acid arrow                       Szkola:               Profesje: Mag*/
    {  22, 405, "acid based spell", "buteleczkazkwasem", "{gbuteleczka z kwasem{x", "{gKropla kwasu znika w powietrzu{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 269 Spell: endure lightning                 Szkola: Odrzucanie    Profesje: Mag Kleryk Druid */
    {  23, 269, "lighting based spell", "bursztyn", "{ykawa³ek bursztynu{x", "{yDrobne {Yiskry{y skacz± po bursztynie{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 146 Spell: resist lightning                 Szkola: Odrzucanie    Profesje: Mag Kleryk Druid */
    {  24, 146, "lighting based spell", "bursztyn", "{ykawa³ek bursztynu{x", "{yDrobne {Yiskry{y skacz± po bursztynie{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn:  69 Spell: lightning bolt */
    {  25, 69, "lighting based spell", "bursztyn", "{ykawa³ek bursztynu{x", "{yDrobne {Yiskry{y skacz± po bursztynie{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn:  88 Spell: shocking grasp    */
    {  26, 88, "lighting based spell", "bursztyn", "{ykawa³ek bursztynu{x", "{yDrobne {Yiskry{y skacz± po bursztynie{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn:  13 Spell: chain lightning */
    {  27, 13, "lighting based spell", "bursztyn", "{ykawa³ek bursztynu{x", "{yDrobne {Yiskry{y skacz± po bursztynie{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 296 Spell: thunder bolt*/
    {  28, 296, "lighting based spell", "bursztyn", "{ykawa³ek bursztynu{x", "{yDrobne {Yiskry{y skacz± po bursztynie{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 266 Spell: endure fire                      Szkola: Odrzucanie    Profesje: Mag Kleryk Druid */
    {  29, 266, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b³yska ciep³ym ¶wiat³em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 143 Spell: resist fire*/
    {  30, 143, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b³yska ciep³ym ¶wiat³em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn:  48 Spell: fireball*/
    {  31, 48, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b³yska ciep³ym ¶wiat³em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 477 Spell: flame lace */
    {  32, 477, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b³yska ciep³ym ¶wiat³em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 264 Spell: burst of flame */
    {  33, 264, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b³yska ciep³ym ¶wiat³em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 265 Spell: burst of fire */
    {  34, 265, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b³yska ciep³ym ¶wiat³em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 240 Spell: sunscorch  */
    {  35, 240, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b³yska ciep³ym ¶wiat³em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn:  50 Spell: flamestrike   */
    {  36, 50, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b³yska ciep³ym ¶wiat³em{x.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 267 Spell: endure cold                      Szkola: Odrzucanie    Profesje: Mag Kleryk Druid */
    {  37, 267, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa siê w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 145 Spell: resist fire*/
    {  38, 145, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa siê w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 196 Spell: cold snap  */
    {  39, 196, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa siê w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 195 Spell: frost hands */
    {  40, 195, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa siê w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 185 Spell: cone of cold */
    {  41, 185, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa siê w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 284 Spell: frost rift */
    {  42, 284, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa siê w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* Sn: 299 Spell: ice bolt */
    {  43, 299, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa siê w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* mag: enchant weapon */
    {  40,  43, "enchant sredni", "sercebehemota", "{Rserce{x {rbehemota{x", "{Rserce{x {rbehemota{x zaczyna pulsowaæ nagle czerwonym ¶wiat³em!", TRUE, FALSE, FALSE, 0 },
    {  45,  43, "enchant mocny", "sercesmoka", "{Rserce{x {Gzielonego smoka{x", "{Rserce{x {Gzielonego smoka{x rozb³yska nagle niezwyk³ym ¶wiat³em!", TRUE, FALSE, FALSE, 0 },
    /* mag animate dead */
    {  46, 232, "Animate z licza", "czaszka-licza", "{Wczaszka licza{x", "{WCzaszka licza{x otacza siê czerwon± aur±!", TRUE, FALSE, FALSE, 0},
    {  47, 232, "Animate z wampira", "k³y-wampira", "{Dwampirze k³y{x", "{Dwampirze k³y{x otaczaj± siê czerwon± aur± i rozpadaj± w py³!", TRUE, FALSE, FALSE, 0},
    {  48, 232, "Animate z mumii", "banda¿e-mumii", "kupka bandazy", "Banda¿e mumii otaczaj± siê czerwon± aur± i rozpadaj± w py³!", TRUE, FALSE, FALSE, 0},
    {  49, 232, "Animate z upiora", "upiorny-py³", "kupka popiolu", "Upiorny py³ po³yskuje czerwon± aur± i rozwiewa siê!", TRUE, FALSE, FALSE, 0},
    /* kleryk: heal */
    {  50,  61, "komponent do heal", "³za-isoryka", "fiolka z ³z± Isoryka", "Fiolka z ³z± Isoryka otacza sie bia³ym ¶wiat³em!", TRUE, FALSE, FALSE, 0},
    /* kleryk: harm */
    {  51,  59, "komponent do harm", "czaszka-licza", "{Wczaszka licza{x", "{WCzaszka licza{x otacza siê czerwon± aur±!", TRUE, FALSE, FALSE, 0},
    /* * spell_mending */
    {  52, 170, "komponent do mend", "ksiezycowyproszek", "{Wksiê¿ycowy proszek{x", "Ksiê¿ycowy proszek znika w dalikatnym b³ysku ¶wiat³a.", FALSE, FALSE, FALSE, 0 },
    /* spell_elemental_devastation */
    {  53,  99, "acid based spell", "buteleczkazkwasem", "{gbuteleczka z kwasem{x", "{gKropla kwasu znika w powietrzu{x.", TRUE, FALSE, FALSE, 0 },
    {  54,  99, "lighting based spell", "bursztyn", "{ykawa³ek bursztynu{x", "{yDrobne {Yiskry{y skacz± po bursztynie{x.", TRUE, FALSE, FALSE, 0 },
    {  55,  99, "fire based spell", "agatognisty", "{ragat {Rognisty{x", "{rAgat {Rognisty{r b³yska ciep³ym ¶wiat³em{x.", TRUE, FALSE, FALSE, 0 },
    {  56,  99, "cold based spell", "srebrnyproszek", "{Dsrebrny proszek{x", "Odrobina {Dsrebrnego proszku{x rozmywa siê w powietrzu.", TRUE, FALSE, FALSE, 0 },
    /* blink */
    {  57, 415, "longer blink", "iluzorycznypyl", "{DIluzoryczny {Wpy³{x", "{DIluzoryczny {Wpy³ {Ymigocze przez chwilê{x.", TRUE, TRUE, FALSE, 0 },
    /* create greater illusion */
    { 58, 315, "cgilvl2", "magiczny-pryzmat", "{Bmagiczny {Ypryzmat{x", "{BMagiczny {Ypryzmat {Rrozb³yskujê szkar³atnym ¶wiat³em{x.", TRUE, TRUE, FALSE, 0 },
    { 59, 315, "cgilvl1", "oko-smoka", "oko smoka", "¬renica oka smoka powiêksza siê na chwilê.", TRUE, TRUE, FALSE, 0 },
    { 60, 402, "wac1", "oko-obserwatora", "oko obserwatora", "¬renica oka obserwatora powiêksza siê na chwilê.", TRUE, TRUE, FALSE,0 },
    /*
     * UWAGA!
     *
     * Dodajac nowy komponent sprawdz, zeby pierwsza liczba byla NOWA
     * ABSOLUTNIE nie wolno zmieniac tych liczb w innym momencie niz
     * player wipe poniewaz sa one wiaza w pliku gracza informacje
     * o znanych komponentach.
     *
     * dodatkowo w pliku merc.h ZMIEN wartosc stalej MAX_COMPONENTS
     *
     * Nowy komponent dodaj POWYZEJ tego kometarza.
     */
    /* null - zakonczenie tablicy */
    { 0, 9999, NULL, NULL, NULL, NULL, FALSE, 0 }
};

int spell_items_table_count()
{
	return ( ( sizeof( spell_items_table ) / sizeof( spell_items_table[0] ) ) - 2 );
}
/**
 * level info common ch->level < 11
 *
 * created by Laszlo (2008-09-04)
 */
const char * level_info_common [] =
{
    "Zjadacz czerstwych bu³ek.",
    "Przeciêtny zjadacz chleba.",
    "Zwyk³y mieszkaniec mie¶ciny.",
    "Nieugiêty morderca s³owików.",
    "Pogromca szczurów.",
    "Podró¿nik gubi±cy siê w gwarze miasta.",
    "Osoba, która nie boi siê wyj¶æ za próg domu.",
    "Osoba, która dobrze zna miasto.",
    "Podró¿nik stawiajacy pierwsze kroki za miastem.",
    "Wêdrowiec rozpoczynaj±cy sw± wêdrówkê po ¶wiecie."
};
/**
 * level info common ch->level > 10 && CLASS_WARRIOR
 *
 * created by Laszlo (2008-09-04)
 */
const char * level_info_warrior [] =
{
    "Pocz±tkuj±cy adept sztuki wojennej.",
    "Osoba pragn±ca poznaæ nowe techniki walki.",
    "Pocz±tkujacy wêdrowiec szukaj±cy przygód w dalekim ¶wiecie.",
    "Osoba, która pozna³a ju¿ pewne techniki walki.",
    "Wojownik nie boj±cy siê walki.",
    "Bohater, splamiony trochê krwi± wrogów.",
    "Wojownik staraj±cy siê doskonaliæ technikê walki.",
    "Kto¶, kto posiad³ ju¿ pewn± wiedzê na temat walki.",
    "Osoba splamiona krwi± wrogów.",
    "Podró¿nik przemierzaj±cy trakty i bezdro¿a ¶wiata.",
    "Podró¿nik, który potrafi przys³u¿yæ siê sprawie.",
    "Osoba, która nie boi siê przygód.",
    "Kto¶, kto zas³yn±³ ju¿ trochê ze swych czynów.",
    "Wojownik, który doskona³e opanowa³ sztukê wojenna.",
    "Bohater, który zas³yn±³ ze swoich czynów w ca³ym ¶wiecie.",
    "Odwa¿ny podró¿nik, który nie lêka siê trudno¶ci.",
    "Nieugiêty pogromca najprzeró¿niejszych stworów.",
    "Wojownik, który w ca³o¶ci jest pokryty zakrzep³± krwi± wrogów.",
    "Nieustraszony wojownik.",
    "Bohater, którego zna prawie ca³y ¶wiat.",
    "Legendarny bohater"
};
/**
 * level info common ch->level > 10 && CLASS_PALADIN
 *
 * created by Laszlo (2008-09-04)
 */
const char * level_info_paladin [] =
{
	"Adept nieznaj±cy jeszcze pe³nej ró¿nicy miedzy dobrem a z³em.",
	"Osoba zaczynaj±ca kroczyæ ¶cie¿k± cnót.",
	"Pocz±tkuj±cy pogromca z³a",
	"Pogromca ghuli.",
	"Nieustraszony pogromca ghastów.",
	"Bohater nieznaj±cy jeszcze swych mo¿liwo¶ci.",
	"Paladyn, maj±cy ju¿ pewne do¶wiadczenie w niszczeniu z³a.",
	"Osoba znaj±ca ju¿ granice dobra i z³a.",
	"Adept sztuki paladyñskiej.",
	"¦mia³ek krocz±cy ¶cie¿k± dobra.",
	"Odwa¿ny ¶mia³ek walcz±cy ze z³em.",
	"Przyk³adny paladyn.",
	"Osoba niszcz±ca z³o tego ¶wiata.",
	"Paladyn nios±cy ¿ar prawdziwej wiary.",
	"Obroñca wdów i niewiast.",
	"Paladyn, którego imiê budzi grozê w z³ych istotach.",
	"Bohater idealnie krocz±cy ¶cie¿k± prawa i sprawiedliwo¶ci",
	"Ulubieniec Portena.",
	"Osoba, którego lêka siê z³o Tamery.",
	"Niedo¶cigniony w wyplenianiu z³a.",
	"Uciele¶nienie wszystkich cnót tego ¶wiata."
};
/**
 * level info common ch->level > 10 && CLASS_CLERIC
 *
 * created by Tanon (2008-09-04)
 */
const char * level_info_cleric [] =
{
	"Nowicjusz ¶wi±tyni.",
	"Poczatkujacy adept ¶wi±tyni.",
	"Adept ¶wi±tyni.",
	"Szukaj±cy ¶cie¿ek wiary.",
	"Pielgrzym.",
	"Niestrudzony pielgrzym.",
	"Krocz±cy ¶cie¿kami wiary.",
	"Osoba, która odnalaz³a wiarê.",
	"Nios±cy wiarê.",
	"S³uga bogów.",
	"Niestrudzony s³uga bogów.",
	"Osoba gotowa, by szerzyæ wiarê.",
	"Diakon ¶wi±tyni.",
	"Kap³an ¶wi±tyni.",
	"Czcigodny kap³an.",
	"Arcykap³an.",
	"Natchniony wol± bogów.",
	"Uzdrowiciel.",
	"Nios±cy nadziejê.",
	"Obroñca wiary.",
	"Pos³aniec bogów."
};
/**
 * level info common ch->level > 10 && CLASS_BARBARIAN
 *
 * created by Laszlo (2008-09-04)
 */
const char * level_info_barbarian [] =
{
	"Pij±cy chrzczone piwo.",
	"Upijaj±cy siê paroma ³ykami jalowcówki.",
	"Pocz±tkuj±cy smakosz piwa.",
	"Racz±cy siê najpodlejszymi alkoholami ¶wiata.",
	"Ja³owe podniebienie.",
	"Osoba, która posmakowa³a sporo trunków.",
	"Niestroni±cy od okazji do flaszeczki.",
	"Kochaj±cy sobie goln±æ.",
	"Ulubieniec karczmarzy.",
	"Postrach gorzelników.",
	"Wytrawny smakosz piwska.",
	"Pogromca bary³ek",
	"Gard³ogrzmot.",
	"Nieznaj±cy umiaru.",
	"Stalowa w±troba.",
	"Pij±cy w zamêcie bitwy.",
	"Pijana furia.",
	"Pijany berserker.",
	"Bestia nieznaj±ca umiaru.",
	"Bohater, który pi³ piwa we wszystkich karczmach Tamery.",
	"Mithrilowa w±troba."
};
/**
 * level info common ch->level > 10 && CLASS_BLACK_KNIGHT
 *
 * created by Tanon(2008-09-04)
 */
const char * level_info_black_knight [] =
{
	"Podró¿nik na rozstajach dróg.",
	"Zagubiony w chaszczach moralno¶ci.",
	"Odkrywaj±cy ¶cie¿kê z³a.",
	"Krocz±cy ¶cie¿k± z³a.",
	"Ciemiê¿ca uci¶nionych.",
	"Morderca kobiet i dzieci.",
	"Splamiony krwi± niewinnych.",
	"Pomiot chaosu.",
	"S³uga chaosu.",
	"Rycerz chaosu.",
	"Czciciel mroku.",
	"S³uga mroku.",
	"Blu¼nierca.",
	"Potêpiony przez bogów.",
	"Rycerz mroku.",
	"Nios±cy rozpacz.",
	"W³adca cierpienia.",
	"Wkraczaj±cy w sferê ciemno¶ci.",
	"Rycerz ciemno¶ci.",
	"Pan ciemno¶ci.",
	"Ksi±¿e ciemno¶ci."
};
/**
 * level info common ch->level > 10 && CLASS_THIEF
 *
 * created by Tanon (2008-09-04)
 */
const char * level_info_thief [] =
{
	"Przedstawiciel marginesu spo³ecznego.",
	"Kradn±cy dzieciom cukierki.",
	"Miejska menda.",
	"Zwyczajna miejska szuja.",
	"Plugawy doliniarz.",
	"Lepkie r±czki.",
	"Cholerny z³odziejaszek.",
	"Paskudny opryszek.",
	"Parszywy szubrawiec.",
	"Oprych natchniony.",
	"Rabu¶ karawan.",
	"Niestrudzony przepatrywacz sakiewek.",
	"Znawca budowy zamków i k³ódek.",
	"Bandyta brutalny lecz subtelny.",
	"Z³odziej nieprzeciêtny.",
	"Znawca anatomii pleców.",
	"Znany zabijaka.",
	"S³awny w³amywacz.",
	"Zabójca zabójców.",
	"Sztyletu cieñ.",
	"Ksi±¿e z³odziei."
};
/**
 * level info common ch->level > 10 && CLASS_MAG
 *
 * created by Laszlo (2008-09-04)
 */
const char * level_info_mag [] =
{
	"Nowicjusz magiczny.",
	"Pocz±tkuj±cy adept magii.",
	"Adept znaj±cy ju¿ pewne podstawy.",
	"Adept doskonal±cy podstawy magii.",
	"Kto¶, kto posiad³ ju¿ podstawow± wiedzê magiczn±.",
	"Adept magicznych s³ów.",
	"Adept magicznych gestów.",
	"Osoba znaj±ca ju¿ magiczne s³owa i gesty.",
	"Osoba mog±ca uznawaæ siebie za maga.",
	"Mag rozpoczynaj±cy swoje pierwsze badania.",
	"Czarodziej poznaj±cy coraz to nowe arkana.",
	"Nieustraszony poszukiwacz wiedzy.",
	"Czarodziej o którym zaczynaj± kr±¿yæ wie¶ci po ¶wiecie.",
	"Mag doskonal±cy swój kunszt.",
	"Mag, którego odkrycia zapewni³y mu spore uznanie.",
	"Nieustraszony badacz zjawisk magicznych.",
	"Uczony, którego wiedza przewy¿sza zwyk³ych ¶miertelników.",
	"Czarownik, którego erudycja przyæmiewa innych.",
	"Mêdrzec uznany na ca³ym ¶wiecie.",
	"Mistrz magii, znaj±cy wszystkie jej arkana.",
	"Prawdziwy arcymistrz, znawca najtajniejszych formu³ i gestów magicznych."
};
/**
 * level info common ch->level > 10 && CLASS_DRUID
 *
 * created by Tanon (2008-09-04)
 */
const char * level_info_druid [] =
{
	"Lubi±cy siedzieæ na trawie.",
	"Mi³o¶nik kwiatów.",
	"Amator grzybków.",
	"Pachn±cy ¶ció³k±.",
	"Mi³o¶nik zieleni.",
	"Kochaj±cy zwierzêta.",
	"Ulubieniec gryzoni.",
	"Cz³owiek lasu.",
	"Pan saren i jeleni.",
	"Znawca lasu.",
	"Spogl±daj±cy prosto w s³oñce.",
	"Zaklinacz wiatru.",
	"Szukaj±cy równowagi.",
	"Stra¿nik natury.",
	"Obroñca natury.",
	"Nieugiêty obroñca natury.",
	"W³adaj±cy mocami natury.",
	"Pan ³±k i lasów.",
	"W³adca zwierz±t.",
	"W³adca bestii.",
	"W³adca natury."
};
/**
 * level info common ch->level > 10 && CLASS_BARD
 */
const char * level_info_bard [] =
{
};
/**
 * level info common ch->level > 10 && CLASS_MONK
 */
const char * level_info_monk [] =
{
};

/**
 * level info common ch->level > 10 && CLASS_SHAMAN
 */
const char * level_info_shaman [] =
{
	"Ten, który szuka pomocy duchów.",
	"Ten, który podró¿uje drog± przodków.",
	"Ten, który poznaje podstawy rytua³ów.",
	"Ten, który poszukuje drogi w¶ród gór.",
	"Ten, który szuka m±dro¶ci przodków.",
	"Ten, który rozmawia z duchami.",
	"Ten, który wêdruje w¶ród gór.",
	"Ten, który siê nie zatrzymuje.",
	"Ten, który poszukuje wizji.",
	"Ten, który rozkazuje duchom.",
	"Ten, który szepta s³owa m±dro¶ci.",
	"Ten, który patrzy z górskiego szczytu.",
	"Ten, który widzi to co niewidoczne.",
	"Ten, który odprawia rytua³y.",
	"Ten, który mówi g³osem gór.",
	"Ten, który znalaz³ cel.",
	"Ten, który mówi g³osem duchów.",
	"Ten, który dost±pi³ wizji.",
	"Ten, który odnalaz³ m±dro¶æ.",
	"Ten, który jest W³adc± Gór.",
	"Ten, który jest W³adc± Gór."
};

struct repair_item_type repair_item_table[] =
{
    {	"bronie",		REP_ITEM_WEAPON,	"wszystkie rodzaje broni"	},
    {	"dlugie miecze",	REP_ITEM_SWORDS,	"konkretny typ broni"		},
    {	"topory",		REP_ITEM_AXE,		"konkretny typ broni"		},
    {	"mace",			REP_ITEM_MACE,		"konkretny typ broni"		},
    {	"sztylety",		REP_ITEM_DAGGER,	"konkretny typ broni"		},
    {	"wlocznie",		REP_ITEM_SPEAR,		"konkretny typ broni"		},
    {	"korbacze",		REP_ITEM_FLAIL,		"konkretny typ broni"		},
    {	"bicze",		REP_ITEM_WHIP,		"konkretny typ broni"		},
    {	"drzewcowe",		REP_ITEM_POLEARM,	"konkretny typ broni"		},
    {	"laski",		REP_ITEM_STAFF,		"konkretny typ broni"		},
    {	"krotkie miecze",	REP_ITEM_SHSWORD,	"konkretny typ broni"		},
    {	"miecze",		REP_ITEM_SWORDS,	"ogolnie miecze, dlugie, krotkie, 2-reczne"	},
    {	"obuchowe",		REP_ITEM_BLUDGEONS,	"wszyskie bronie obuchowe: mace, flail, staff, polearm" },
    {	"zbroje",		REP_ITEM_ARMOR,		"wszystkie zbroje"			},
    {	"zbroje lekkie",	REP_ITEM_ARMOR_LIGHT,	"zbroje lekkie 0-3 w tablicy pancerzy"	},
    {	"zbroje srednie",	REP_ITEM_ARMOR_MEDIUM,	"zbroje 4-7 w tablicy pancerzy"		},
    {	"zbroje ciezkie",	REP_ITEM_ARMOR_HEAVY,	"zbroje 8-11 w tablicy pancerzy"	},
    {	"zbroje b.ciezkie",	REP_ITEM_ARMOR_VHEAVY,	"zbroje 12-15 w tablicy pancerzy"	},
    {	"odziez",		REP_ITEM_CLOTHING,	"obiekty typu clothing"	},
    {	"bizuteria",		REP_ITEM_JEWELRY,	"obiekty typu jewelry"	},
    {	"kamienie",		REP_ITEM_GEM,		"obiekty typu gem"	},
    {	"instrumenty muzyczne",		REP_ITEM_MUSICAL_INSTRUMENT,		"bardowe instrumenty muzyczne"	},
    {	"tarcze",		REP_ITEM_SHIELD,	"wszystkie tarcze"	},
    { "narzêdzia", REP_ITEM_TOOL, "wszystkie narzêdzia" },
    { "szpony",  REP_ITEM_CLAWS, "konkretny typ broni" },
    {	NULL,			0,			NULL			}
};

const char heal_plant_msg_table [6][MAX_STRING_LENGTH] = {
   "Niemal wszystkie uszkodzenia $z goj± siê",
   "Wiekszo¶æ masywnych zniszczeñ $z goi siê.",
   "Kilka du¿ych uszkodzeñ $z goi siê.",
   "Kilka uszkodzeñ $z goi siê.",
   "Kilka zranieñ $z znika.",
   "Niektóre z zadrapañ $z znikaj±."
};

const char heal_animal_msg_table [6][MAX_STRING_LENGTH] = {
   "Prawie wszystkie rany $z goj± siê.",
   "Wiêkszo¶æ ran $z goi siê.",
   "Kilka g³êbokich ran $z goi siê.",
   "Kilka ran $z goi siê.",
   "Kilka zranieñ $z znika.",
   "Niektóre z siniaków $z znikaj±."
};

const char heal_golem_msg_table [6][MAX_STRING_LENGTH] = {
   "Prawie prawie ca³a struktura $z regeneruje siê.",
   "Wiêkszo¶æ uszkodzeñ $z regeneruje siê.",
   "Kilka g³êbokich ubytków $z uzupe³nia siê.",
   "Kilka ubytków $z wype³nia siê.",
   "Kilka rys $z znika.",
   "Niektóre z zarysowañ $z znikaj±."
};
