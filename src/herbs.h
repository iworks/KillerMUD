struct herb_data
{
	HERB_DATA *			next;			/* wska¿nik na nastêpne zio³o w li¶cie */
	char *				id;				/* jednoznacznie identyfikuje zio³o (podobnie jak nazwy w progach) */
	char *				name;
	char *				name2;
	char *				name3;
	char *				name4;
	char *				name5;
	char *				name6;
};

extern HERB_DATA *	herb_free;
extern HERB_DATA *	herb_list;

HERB_DATA *
new_herb			( void );
void
free_herb			( HERB_DATA * herb );

HERB_IN_ROOM_DATA 	*start_herbs(HERB_IN_ROOM_DATA *first_herb_data, sh_int sector_type);
void 				update_herbs_room(ROOM_INDEX_DATA *room, const char *caller);
void                update_herbs(const char *caller);




