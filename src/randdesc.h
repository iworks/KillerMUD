typedef	DESC_DATA* RANDDESC_FUN		args( ( ROOM_INDEX_DATA *pRoom, char *argument ) );
#define DECLARE_RANDDESC_FUN( fun )	RANDDESC_FUN    fun

typedef	bool RDBOOL_FUN		args( ( ROOM_INDEX_DATA *pRoom, char *argument ) );
#define DECLARE_RDBOOL_FUN( fun )	RDBOOL_FUN    fun

/*
 * Structure for an rand desc commands dispatcher.
 */
struct randdesc_cmd_type
{
    char * const	name;
    RANDDESC_FUN *	function;
};

struct rdbool_cmd_type
{
    char * const	name;
    RDBOOL_FUN *	function;
};

extern const struct randdesc_cmd_type	rdesc_table[];
extern const struct rdbool_cmd_type	    rdbool_table[];

DECLARE_RANDDESC_FUN( rdesc_grouprand );
DECLARE_RANDDESC_FUN( rdesc_show );

DECLARE_RDBOOL_FUN( rdbool_random );
DECLARE_RDBOOL_FUN( rdbool_sector );
DECLARE_RDBOOL_FUN( rdbool_day );
DECLARE_RDBOOL_FUN( rdbool_month );
DECLARE_RDBOOL_FUN( rdbool_dayweek );
DECLARE_RDBOOL_FUN( rdbool_hour );
DECLARE_RDBOOL_FUN( rdbool_room );
