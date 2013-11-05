/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,	   *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *									   *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael	   *
 *  Chastain, Michael Quan, and Mitchell Tse.				   *
 *									   *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc	   *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.						   *
 *									   *
 *  Much time and thought has gone into this software and you are	   *
 *  benefitting.  We hope that you share your changes too.  What goes	   *
 *  around, comes around.						   *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@hypercube.org)				   *
*	    Gabrielle Taylor (gtaylor@hypercube.org)			   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/* instrument class */
#define INSTRUMENT_OCARINA      0
#define INSTRUMENT_FLUTE        1
#define INSTRUMENT_LUTE         2
#define INSTRUMENT_HARP         3
#define INSTRUMENT_MANDOLIN     4
#define INSTRUMENT_PIANO        5
#define INSTRUMENT_SING         31

/* typy piosenek */
#define SONG_MELODY             0
#define SONG_STORY_TELLING      1
#define SONG_BALLAD             2
#define SONG_EPIC               3
#define SONG_COMEDY             4
#define SONG_ODE                5

/* na kogo ma dzialac piosenka */
#define SING_TO_CHAR            0
#define SING_TO_VICTIM          1
#define SING_TO_GROUP           2
#define SING_TO_ENEMY_GROUP     3
#define SING_TO_ALL             4


void            load_songs          args( ( void ) );
