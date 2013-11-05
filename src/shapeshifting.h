
typedef struct ss_form                  SS_FORM;

// Poziom od ktorego druid otrzymuje podstawowe przemiany
#define DRUID_BASIC_LEVEL               5

// Do ujednolicenia przemian
#define DRUID_LOW_LEV                   5
#define DRUID_MED_LEV                   13
#define DRUID_HIGH_LEV                  21
#define DRUID_ULT_LEV                   29
#define DRUID_SUPR_LEV                  30

#define DRUID_LOW_MOD                   30
#define DRUID_MED_MOD                   20
#define DRUID_HIGH_MOD                  10
#define DRUID_ULT_MOD                   0
#define DRUID_SUPR_MOD                  -10

// Przedmioty ladowane
#define SS_BEAR_ITEM                    3362
#define SS_EAGLE_ITEM                   3364
#define SS_TIGER_ITEM                   3363
#define SS_DEER_ITEM                    3365

// Wait przy przemianie w jedn± b±d¼ drug± stronê
#define WAIT_SHAPESHIFT                 6

#define MAX_SHAPE                       6
#define MAX_SHAPESHIFT_RACES            10

// Ile maksymalnie komunikwatów ma byæ przy przemianach
#define MAX_SS_FORM_COMM                5

// które pole struktury ss_form->comm_{shift,ref} odpowiada za co
#define COMM_ROOM 0
#define COMM_CHAR 1


struct shapeshift_data
{
        CHAR_DATA *  animal;
        CHAR_DATA *  backup;
        int   shape;
        int   communicate_number;
        int   animal_vnum;
};

struct ss_form
{
        char *  name;
        int  class;
        int  mob_vnum[MAX_SHAPESHIFT_RACES];
        int  mob_vnum_cnt;
        int  level;
        sh_int  item_vnum;
        int  skill_mod;
        sh_int * sn;
        int comm_shift_num;
        char *comm_shift[2][MAX_SS_FORM_COMM];
        int comm_ref_num;
        char *comm_ref[2][MAX_SS_FORM_COMM];
/*         char *  comm_shift_room[ MAX_SS_FORM_COMM ]; */
/*         char *  comm_shift_char[ MAX_SS_FORM_COMM ]; */
/*         char *  comm_ref_room[ MAX_SS_FORM_COMM ]; */
/*         char *  comm_ref_char[ MAX_SS_FORM_COMM ]; */
};

int is_animal_or_plant( CHAR_DATA* ch );

// postac, argument -> postac przemieniona -> postac
void do_reform( CHAR_DATA* ch, char* argument );
void do_shapeshift_list( CHAR_DATA *ch, char* argument );

// Przenies przedmioty z from->carrying do to->carrying,
// sprawdzaj±c wcze¶niej czy all_unequiped( obj ).
//      (dla bezpieczeñstwa podwójne sprawdzenie)
//
// { transfer_inventory( a, b, f ); transfer_inventory( b, a, !f ) } <=> noop

bool transfer_inventory( CHAR_DATA* from, CHAR_DATA* to, bool flag_to_set );

// kopiuje wybrane statystyki postaci src_ch na dest_ch

bool make_char_copy ( CHAR_DATA *src_ch, CHAR_DATA *dest_ch );

// dualne, pierwsza wysmarza nowe dane do przemiany, drugie zapomnia te dane

SHAPESHIFT_DATA* make_shape_data( CHAR_DATA*ch, int shape, CHAR_DATA* animal );
void free_shape_data( SHAPESHIFT_DATA* ssdata );

// czy wszystko sciagniete
inline bool all_unequiped( OBJ_DATA* obj );

// dualne, pierwsze przeprowadza cich± przemianê
// w za³o¿eniu ¿e postaæ *nie ma nic na sobie*
// (bo zak³adany jest nowy ekwipunek i nie ma co zrobic ze starym)
//
// drugie rozwala po cichu ekwipunek i przywraca statystyki postaci
// zapamiêtuje wska¼nik ch->ss_data, ustawia ch->ss_data = NULL oraz
// zwraca zapamiêtany wska¼nik

bool do_shapeshift_silent( CHAR_DATA* ch, SHAPESHIFT_DATA *shape );


SHAPESHIFT_DATA* do_reform_silent( CHAR_DATA* ch );

