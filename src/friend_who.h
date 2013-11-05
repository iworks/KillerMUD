



bool friend_exist( CHAR_DATA *ch, char *name, char *full_name, char *info, time_t *czas, bool *introduced );
void friend_dodaj( CHAR_DATA *ch, char *name, time_t czas, bool introduced );
void friend_note( CHAR_DATA *ch, char *o_kim, char *co, bool quiet );
bool friend_usun( CHAR_DATA *ch, char *name );
