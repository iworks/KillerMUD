#ifndef _TRAPS_H_
#define _TRAPS_H_

#define TRAP_UNLOCK	(A)
#define TRAP_OPEN	(B)
#define TRAP_WEAR	(C)
#define TRAP_DROP	(D)
#define TRAP_GET	(E)
#define TRAP_PICKOK	    (F)
#define TRAP_PICKFAILED	(G)


struct	trap_type
{
    char *	name;
    int		flag;
};

#define LIMIT_DEX	 0
#define LIMIT_LEVEL	 1
#define LIMIT_SKILL	 2

extern const struct trap_type trap_type_table[];

/* list func */
void		set_trap_limit	args( ( TRAP_DATA *trap, int limit, int value ) );
int		get_limit	args( ( TRAP_DATA * trap , int limit) );
bool		obj_trap_handler	args( ( CHAR_DATA *ch, OBJ_DATA *obj, int type) );
bool		room_trap_handler	args( ( CHAR_DATA *ch ) );
bool		room_exit_trap_handler	args( (CHAR_DATA *ch, EXIT_DATA *pexit, int type ) );
bool		check_limits	args( (TRAP_DATA *trap, CHAR_DATA *ch) );
#endif
