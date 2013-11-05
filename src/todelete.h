typedef struct todelete_data	TODELETE_DATA;

struct todelete_data		// to_delete list strucure
{
    TODELETE_DATA  *next;	// next item on the list
    char	   *name;	// name of the ch
    time_t	    timestamp;	// date when ch was added to the list
    bool 	    valid;	// memory recycling
};

extern void show_todelete	args( ( CHAR_DATA *ch ) );
extern void check_todelete	args( ( CHAR_DATA *ch ) );
extern void process_todelete	args( ( DESCRIPTOR_DATA *d, CHAR_DATA *ch ) );
extern void todelete_menu_mod	args( ( CHAR_DATA *ch ) );
extern void load_todelete_list	args( ( ) );
extern void delete_todelete	args( ( ) );
