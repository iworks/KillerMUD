/* vals from db.c */
extern bool fBootDb;
extern int		newmobs;
extern int		newobjs;
extern MOB_INDEX_DATA 	* mob_index_hash          [MAX_KEY_HASH];
extern OBJ_INDEX_DATA 	* obj_index_hash          [MAX_KEY_HASH];
extern unsigned int	top_mob_index;
extern unsigned int	top_obj_index;
extern unsigned int	top_trap_index;
extern unsigned int	top_rdesc_index;
extern int  		top_affect;
extern int		top_ed;
extern AREA_DATA 	* area_first;


/* from db2.c */
extern int	social_count;

/* conversion from db.h */
void	convert_mob(MOB_INDEX_DATA *mob);
void	convert_obj(OBJ_INDEX_DATA *obj);

/* macro for flag swapping */
#define GET_UNSET(flag1,flag2)	(~(flag1)&((flag1)|(flag2)))

/* Magic number for memory allocation */
#define MAGIC_NUM 52571214

/* func from db.c */
extern void assign_area_vnum( ush_int vnum );                    /* OLC */

/* from db2.c */

void convert_mobile( MOB_INDEX_DATA *pMobIndex );            /* OLC ROM */
void convert_objects( void );                                /* OLC ROM */
void convert_object( OBJ_INDEX_DATA *pObjIndex );            /* OLC ROM */

/* for mt19937ar.c usage */

//#define OLD_RAND == #define RANDOM_GEN 0 : Mitchell-Moore z Knutha
//#define NEW_RAND == #define RANDOM_GEN 1 : srandom()
//#define RANDOM_GEN 2 : Mersenne Twister

#define RANDOM_GEN 2

