#define LANG_COMMON      (A)  /* Human base language */
#define LANG_ELVEN       (B)  /* Elven base language */
#define LANG_DWARVEN     (C)  /* Dwarven base language */
#define LANG_PIXIE       (D)  /* Pixie/Fairy base language */
#define LANG_OGRE        (E)  /* Ogre base language */
#define LANG_ORCISH      (F)  /* Orc base language */
#define LANG_TROLLISH    (G)  /* Troll base language */
#define LANG_RODENT      (H)  /* Small mammals */
#define LANG_INSECTOID   (I)  /* Insects */
#define LANG_MAMMAL      (J)  /* Larger mammals */
#define LANG_REPTILE     (K)  /* Small reptiles */
#define LANG_DRAGON      (L)  /* Large reptiles, Dragons */
#define LANG_SPIRITUAL   (M)  /* Necromancers or undeads/spectres */
#define LANG_MAGICAL     (N)  /* Spells maybe?  Magical creatures */
#define LANG_GOBLIN      (O)  /* Goblin base language */
#define LANG_GOD         (P)  /* Clerics possibly?  God creatures */
#define LANG_ANCIENT     (R)  /* Prelude to a glyph read skill? */
#define LANG_HALFLING    (S)  /* Halfling base language */
#define LANG_CLAN	 (T)  /* Clan language */
#define LANG_GITH	 (U)  /* Gith Language */
#define LANG_TELEPATIC	 (V)  /* Gith Language */
#define LANG_UNKNOWN      0  /* Anything that doesnt fit a category */
#define VALID_LANGS    ( LANG_COMMON | LANG_ELVEN | LANG_DWARVEN | LANG_PIXIE  \
		       | LANG_OGRE | LANG_ORCISH | LANG_TROLLISH | LANG_GOBLIN \
		       | LANG_HALFLING | LANG_GITH | LANG_TELEPATIC )




typedef	struct	lcnv_data		LCNV_DATA;
typedef	struct	lang_data		LANG_DATA;
extern 		LANG_DATA *		first_lang;

struct lcnv_data
{
    LCNV_DATA *		next;
    char *		old;
    int			olen;
    char *		new;
    int			nlen;
};

struct lang_data
{
    LANG_DATA *		next;
    char *		name;
    LCNV_DATA *		first_precnv;
    char *		alphabet;
    LCNV_DATA *		first_cnv;
};


struct lang_type
{
 char *name;
 int bit;
};

extern struct lang_type lang_table[MAX_LANG];

char 	*translate	args( (int percent, const char *in, const char *name) );
char    *translate_gnomish  args( ( char *string ) );
int 	knows_language	args( ( CHAR_DATA *ch, int language, CHAR_DATA *cch ) );
int 	get_langnum	args( ( char *flag ) );


