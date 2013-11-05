#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "merc.h"
#include "tables.h"
#include "lang.h"

int flag_lookup ( const char *name, const struct flag_type *flag_table )
{
	int flag;

	for ( flag = 0; flag_table[ flag ].name != NULL; flag++ )
	{
		if ( NOPOL( name[ 0 ] ) == NOPOL( flag_table[ flag ].name[ 0 ] )
		     && !str_prefix( name, flag_table[ flag ].name ) )
			return flag_table[ flag ].bit;
	}

	return NO_FLAG;
}

BITVECT_DATA * ext_flag_lookup ( const char *name, const struct ext_flag_type * ext_flag_table )
{
	int flag;

	for ( flag = 0; ext_flag_table[ flag ].name != NULL; flag++ )
	{
		if ( NOPOL( name[ 0 ] ) == NOPOL( ext_flag_table[ flag ].name[ 0 ] )
		     && !str_prefix( name, ext_flag_table[ flag ].name ) )
			return ext_flag_table[ flag ].bitvect;
	}

	return &EXT_NONE;
}

bool is_settable ( const char *name, const struct flag_type *flag_table )
{
	int flag;

	for ( flag = 0; flag_table[ flag ].name != NULL; flag++ )
	{
		if ( NOPOL( name[ 0 ] ) == NOPOL( flag_table[ flag ].name[ 0 ] )
		     && !str_prefix( name, flag_table[ flag ].name ) )
			return flag_table[ flag ].settable;
	}

	return FALSE;
}

bool ext_is_settable ( const char *name, const struct ext_flag_type * ext_flag_table )
{
	int flag;

	for ( flag = 0; ext_flag_table[ flag ].name != NULL; flag++ )
	{
		if ( NOPOL( name[ 0 ] ) == NOPOL( ext_flag_table[ flag ].name[ 0 ] )
		     && !str_prefix( name, ext_flag_table[ flag ].name ) )
			return ext_flag_table[ flag ].settable;
	}

	return FALSE;
}

int position_lookup ( const char *name )
{
	int pos;

	for ( pos = 0; position_table[ pos ].name != NULL; pos++ )
	{
		if ( NOPOL( name[ 0 ] ) == NOPOL( position_table[ pos ].name[ 0 ] )
		     && !str_prefix( name, position_table[ pos ].name ) )
			return pos;
	}

	return -1;
}

int sex_lookup ( const char *name )
{
	int sex;

	for ( sex = 0; sex_table[ sex ].name != NULL; sex++ )
	{
		if ( NOPOL( name[ 0 ] ) == NOPOL( sex_table[ sex ].name[ 0 ] )
		     && !str_prefix( name, sex_table[ sex ].name ) )
			return sex;
	}

	return -1;
}

int gender_lookup ( const char *name )
{
	int gender;

	for ( gender = 0; gender_table[ gender ].name != NULL; gender++ )
	{
		if ( NOPOL( name[ 0 ] ) == NOPOL( gender_table[ gender ].name[ 0 ] )
		     && !str_prefix( name, gender_table[ gender ].name ) )
			return gender;
	}

	return -1;
}

int size_lookup ( const char *name )
{
	int size;

	for ( size = 0; size_table[ size ].name != NULL; size++ )
	{
		if ( NOPOL( name[ 0 ] ) == NOPOL( size_table[ size ].name[ 0 ] )
		     && !str_prefix( name, size_table[ size ].name ) )
			return size;
	}

	return -1;
}

/* returns race number */
int race_lookup ( const char *name )
{
	int race;

	for ( race = 0; race_table[ race ].name != NULL; race++ )
	{
		if ( NOPOL( name[ 0 ] ) == NOPOL( race_table[ race ].name[ 0 ] )
		     && !str_prefix( name, race_table[ race ].name ) )
			return race;
	}

	return 0;
}

int item_lookup( const char *name )
{
	int type;

	for ( type = 0; item_table[ type ].name != NULL; type++ )
	{
		if ( NOPOL( name[ 0 ] ) == NOPOL( item_table[ type ].name[ 0 ] )
		     && !str_prefix( name, item_table[ type ].name ) )
			return item_table[ type ].type;
	}

	return -1;
}

int liq_lookup ( const char *name )
{
	int liq;

	for ( liq = 0; liq_table[ liq ].liq_name != NULL; liq++ )
	{
		if ( NOPOL( name[ 0 ] ) == NOPOL( liq_table[ liq ].liq_name[ 0 ] )
		     && !str_prefix( name, liq_table[ liq ].liq_name ) )
			return liq;
	}

	return -1;
}

//rellik: mining
int tool_lookup ( const char *name)
{
	int tool;
	for ( tool = 0; tool_table[ tool ].name != NULL; tool++ )
	{
		if ( NOPOL( name[0] ) == NOPOL( tool_table[tool].name[0] )
			 && !str_prefix( name, tool_table[tool].name ) )
		{
			return tool;
		}
	}
	return -1;
}

//rellik: mining
int rawmaterial_lookup( const char *name )
{
	int mat;
	for ( mat = 0; rawmaterial_table[ mat ].name; ++mat )
	{
		if ( !str_prefix( name, rawmaterial_table[ mat ].name ) )
		{
			return mat;
		}
	}
	return -1;
}

int weed_lookup ( const char *name )
{
	int weed;

	for ( weed = 0; weed_table[ weed ].weed_name != NULL; weed++ )
	{
		if ( NOPOL( name[ 0 ] ) == NOPOL( weed_table[ weed ].weed_name[ 0 ] )
		     && !str_prefix( name, weed_table[ weed ].weed_name ) )
			return weed;
	}

	return -1;
}

HELP_DATA * help_lookup( char *keyword )
{
	HELP_DATA * pHelp;
	char temp[ MIL ], argall[ MIL ];

	argall[ 0 ] = '\0';

	while ( keyword[ 0 ] != '\0' )
	{
		keyword = one_argument( keyword, temp );
		if ( argall[ 0 ] != '\0' )
			strcat( argall, " " );
		strcat( argall, temp );
	}

	for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
		if ( is_name( argall, pHelp->keyword ) )
			return pHelp;

	return NULL;
}

HELP_AREA * had_lookup( char *arg )
{
	HELP_AREA * temp;
	extern HELP_AREA * had_list;

	for ( temp = had_list; temp; temp = temp->next )
		if ( !str_cmp( arg, temp->filename ) )
			return temp;

	return NULL;
}

int language_lookup( char *arg )
{
	int langs;

	for ( langs = 0; lang_table[ langs ].bit != LANG_UNKNOWN; langs++ )
	{
		if ( !str_prefix( arg, lang_table[ langs ].name ) )
		{
			return langs;
		}
	}
	return -1;
}
