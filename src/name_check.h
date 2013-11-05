#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include "merc.h"

#define MAX_HASH	32
#define ALLOWED_NAMES_FILE	"../system/allowed_names.txt"
#define DISALLOWED_NAMES_FILE	"../system/disallowed_names.txt"
#define NEW_NAMES_FILE		"../system/new_names.txt"
#define NAME_DISALLOWED	-1
#define NAME_NOTEXIST	0
#define NAME_ALLOWED	1
#define NAME_NEW	2

typedef struct name_list NAME_LIST;

//nazwy dozwolone i niedozwolone sa trzymane w jednej tablicy
struct name_list
{
    char *	name;
    NAME_LIST *	next;
    int		allowed;
};

void load_files();
void initialize_name_table();
int is_allowed( char *name );
void add_name( char *name, int type );
void save_file( int type );
