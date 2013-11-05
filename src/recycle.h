/* externs */
extern char str_empty[1];
extern int mobile_count;

/* stuff for providing a crash-proof buffer */

#define MAX_BUF		32768
#define MAX_BUF_LIST 	12
#define BASE_BUF 	1024

/* valid states */
#define BUFFER_SAFE	0
#define BUFFER_OVERFLOW	1
#define BUFFER_FREED 	2

/* note recycling */
#define ND NOTE_DATA
ND	*new_note args( (void) );
void	free_note args( (NOTE_DATA *note) );
#undef ND

#define ED2 EVENT2_DATA
ED2 *new_event2 args( (void) );
void free_event2 args( (ED2 *ev2) );
#undef ED2

/* ban data recycling */
#define BD BAN_DATA
BD	*new_ban args( (void) );
void	free_ban args( (BAN_DATA *ban) );
#undef BD

/* descriptor recycling */
#define DD DESCRIPTOR_DATA
DD	*new_descriptor args( (void) );
void	free_descriptor args( (DESCRIPTOR_DATA *d) );
#undef DD



/* extra descr recycling */
#define ED EXTRA_DESCR_DATA
ED	*new_extra_descr args( (void) );
void	free_extra_descr args( (EXTRA_DESCR_DATA *ed) );
#undef ED

/* affect recycling */
#define AD AFFECT_DATA
AD	*new_affect args( (void) );
void	free_affect args( (AFFECT_DATA *af) );

MSPELL_DATA *new_mspell args( (void) );
void   free_mspell args( (MSPELL_DATA *memset) );

MEMSET_DATA *new_memset args( (void) );
void   free_memset args( (MEMSET_DATA *memset) );

#undef AD

/* object recycling */
#define OD OBJ_DATA
OD	*new_obj args( (void) );
void	free_obj args( (OBJ_DATA *obj) );
#undef OD

/* character recyling */
#define CD CHAR_DATA
#define PD PC_DATA
void    init_new_char_data args( (CHAR_DATA *ch) );
CD      *new_char args( (void) );
void    free_char args( (CHAR_DATA *ch) );
PD      *new_pcdata args( (void) );
void    free_pcdata args( (PC_DATA *pcdata) );
#undef PD
#undef CD

/* mob repair data */
void 		free_repair	args( ( REPAIR_DATA * pRepair ) );
REPAIR_DATA * 	new_repair	args( (void) );

/* mob id and memory procedures */
#define MD MEM_DATA
long 	get_pc_id args( (void) );
long	get_mob_id args( (void) );
MD	*new_mem_data args( (void) );
void	free_mem_data args( ( MEM_DATA *memory) );
MD	*find_memory args( (MEM_DATA *memory, long id) );
#undef MD

/* buffer procedures */
BUFFER	*new_buf args( (void) );
BUFFER  *new_buf_size args( (int size) );
void	free_buf args( (BUFFER *buffer) );
bool	add_buf args( (BUFFER *buffer, char *string) );
bool	add_bufx args( (BUFFER *buffer, char *string) );
void	clear_buf args( (BUFFER *buffer) );
char	*buf_string args( (BUFFER *buffer) );

HELP_AREA *	new_had		args( ( void ) );
HELP_DATA *	new_help	args( ( void ) );
void		free_help	args( ( HELP_DATA * ) );


ARTEFACT_DATA *new_artefact_data args( (void) );
void free_artefact_data       args( (ARTEFACT_DATA *artefact) );

ARTEFACT_OWNER *new_artefact_owner args( (void) );
void free_artefact_owner       args( (ARTEFACT_OWNER *owner) );

ARTEFACT_LOADER *new_artefact_loader args( (void) );
void free_artefact_loader       args( (ARTEFACT_LOADER *loader) );

LEARN_DATA *new_learn_data args( (void) );
void free_learn_data       args( (LEARN_DATA *learn) );

LEARN_TRICK_DATA *new_learn_trick_data args( (void) );
void free_learn_trick_data       args( (LEARN_TRICK_DATA *learn) );

LEARN_LIST *new_learn_list args( (void) );
void free_learn_list       args( (LEARN_LIST *learn) );

LEARN_TRICK_LIST *new_learn_trick_list args( (void) );
void free_learn_trick_list       args( (LEARN_TRICK_LIST *learn) );


MESSAGE_DATA    *new_msg	args( (void) );
void 		free_msg	args( (MESSAGE_DATA *msg) );

/*BOARD_DATA      *new_board  args( (void) );
void		free_board  args( (BOARD_DATA *board));*/

//Brohacz: bounty
BOUNTY_DATA 	*new_bounty	args( (void) );
void 		free_bounty	args( (BOUNTY_DATA *bounty) ); 

PFLAG_DATA 	*new_pflag  	args( (void) );
void 		free_pflag  	args( (PFLAG_DATA *pflag) );

PRE_WAIT 	*new_prewait	args( (void) );
void 		free_prewait	args( (PRE_WAIT *pwait) );

PWAIT_CHAR 	*new_pwait_char args((void));
void 		free_pwait_char args((PWAIT_CHAR *pwait));

SPELL_FAILED 	*new_spell_failed	args( (void) );
void 		free_spell_failed	args( (SPELL_FAILED *sp_failed) );

INFO_DESCRIPTOR_DATA *new_info_descriptor   args( ( void ) );
void                  free_info_descriptor  args( (INFO_DESCRIPTOR_DATA *id ) );

//herbs
HERB_IN_ROOM_DATA *new_herb_room_data       args( ( void ) );
void free_herb_room_data                    args( ( HERB_IN_ROOM_DATA *herb ) );

NEW_CHAR_DATA *
new_new_char_data				args( ( void ) );
void
free_new_char_data				args( ( NEW_CHAR_DATA *new_char ) );

void add_new_affect_rt( OBJ_DATA *obj, int type, int loc, int mod, int level, BITVECT_DATA * bitv, int dur, int rtdur, bool last );
void add_new_affect( OBJ_DATA *obj, int type, int loc, int mod, int level, BITVECT_DATA * bitv, int dur, bool last );
