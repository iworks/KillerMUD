#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "olc.h"




//chance
#define EXIT2		95
#define EXIT3		3
#define EXIT4	    2

#define MAX_VNUM	65535
#define MAP_SIZE	100

struct map_data
{
    int x;
    int y;
    int ex;
};

//mapa
char big_map[ MAP_SIZE ][ MAP_SIZE ];
//punkty
struct map_data list[ MAP_SIZE*MAP_SIZE ];
//flaga
bool RANDOM_AREA_ERROR = FALSE;

int map_x;
int map_y;

int max_wizgroup( CHAR_DATA *ch );



/* na razie bez argumentow */
void generate_map( CHAR_DATA *ch, int size_x, int size_y, int count_rooms, int density )
{
    int tmp;
    int rnd;
    int counter = 0;
    int rooms;

    if ( RANDOM_AREA_ERROR )
        return ;

    map_x = UMIN( MAP_SIZE, size_x );
    map_y = UMIN( MAP_SIZE, size_y );

    if ( count_rooms >= map_x * map_y )
        count_rooms = map_x * map_y / 2;

    map_init();

    /* petla losujaca */
    while ( TRUE )
    {
        if ( counter >= count_rooms )
        {
            show_map( ch );
            map_to_string( ch );
            return ;
        }

        /* pierwszy element */
        if ( counter == 0 )
        {
            big_map[ map_y / 2 ][ map_x / 2 ] = 1;
            counter = 1;
            continue;
        }

        tmp = fill_list( );
        rnd = number_range( 0, tmp - 1 );
        rooms = add_room( rnd, density );
        counter+=rooms;
    }
    return ;
}

void show_map( CHAR_DATA *ch )
{
    char buf[ UMAX( map_y, map_x ) + 2 ];
    int x, y;
    int rooms = 0;

    for ( y = 0; y < map_y; y++ )
    {
        for ( x = 0; x < map_x; x++ )
        {
            if ( big_map[ y ][ x ] )
            {
                buf[ x ] = 'X';
                rooms++;
            }
            else
                buf[ x ] = ' ';
        }


        buf[ map_x ] = '\n';
        buf[ map_x + 1 ] = '\0';

        print_char( ch, "%s", buf );
    }

    print_char( ch, "\n\rKraina sk³ada siê z %d lokacji.\n\r", rooms );
    return ;
}

void map_to_string( CHAR_DATA *ch )
{
    char line[ MAP_SIZE + 2 ];
    char buf[ 2 * MAX_STRING_LENGTH ];
    int x, y;
    int rooms = 0;

    buf[0] = '\0';

    for ( y = 0; y < map_y; y++ )
    {
        for ( x = 0; x < map_x; x++ )
        {
            if ( big_map[ y ][ x ] )
            {
                line[ x ] = 'X';
                rooms++;
            }
            else
                line[ x ] = ' ';
        }


        line[ map_x ] = '\n';
        line[ map_x + 1 ] = '\r';
        line[ map_x + 2 ] = '\0';
        strcat( buf, line );
    }
    free_string( ch->desc->custom_map );
    ch->desc->custom_map = str_dup( buf );
    return ;
}

void string_to_map( CHAR_DATA *ch )
{
    int x, y;
    char* p;
    int max_x = 0, max_y = 0;

    if ( IS_NULLSTR( ch->desc->custom_map ) )
    {
        send_to_char( "U¿yj najpierw komendy 'rmap generate' lub 'rmap edit'.\n\r", ch );
        return;
    }

    map_init();
    p = ch->desc->custom_map;
    x = 0;
    y = 0;

    while( *p )
    {
        while( *p != '\r' )
        {
            if ( *p == 'X' )
            {
                big_map[ y ][ x ] = 1;
                if ( x > max_x )
                    max_x = x;
                if ( y > max_y )
                    max_y = y;
                x++;
            }
            if ( *p == ' ' )
                x++;

            if ( x >= MAP_SIZE )
                x--;
            p++;
        }

        x = 0;
        y++;
        if ( y >= MAP_SIZE )
            y--;
        p++;
    }
    map_x = UMIN( MAP_SIZE, max_x + 1 );
    map_y = UMIN( MAP_SIZE, max_y + 1 );
    return ;
}

void map_init()
{
    int x, y;

    for ( y = 0; y < MAP_SIZE; y++ )
        for ( x = 0; x < MAP_SIZE; x++ )
            big_map[ y ][ x ] = 0;

    for ( y = 0; y < MAP_SIZE*MAP_SIZE; y++ )
    {
        list[ y ].x = 0;
        list[ y ].y = 0;
        list[ y ].ex = 0;
    }

    return ;
}

int rnd_extype()
{
    int rnd, ex_lim;

    rnd = number_percent();

    if ( rnd >= 0 && rnd <= EXIT2 )
        ex_lim = 2;
    else if ( rnd > EXIT2 && rnd <= EXIT2 + EXIT3 )
        ex_lim = 3;
    else
        ex_lim = 4;

    return ex_lim;
}

int fill_list( void )
{
    int x, y, tmp = 0;
    int ex, rnd;

    for ( y = 0; y < map_y; y++ )
        for ( x = 0; x < map_x; x++ )
        {
            if ( !big_map[ y ][ x ] )
                continue;

            ex = 0;

            if ( x + 1 < map_x && big_map[ y ][ x + 1 ] )
                ex++;
            if ( x - 1 >= 0 && big_map[ y ][ x - 1 ] )
                ex++;
            if ( y + 1 < map_y && big_map[ y + 1 ][ x ] )
                ex++;
            if ( y - 1 >= 0 && big_map[ y - 1 ][ x ] )
                ex++;

            rnd = rnd_extype();

            if ( ex < rnd )
            {
                list[ tmp ].x = x;
                list[ tmp ].y = y;
                list[ tmp++ ].ex = rnd;
            }
        }

    return tmp;
}

bool free_room( int source_room, int density, int room_x, int room_y )
{
    if ( big_map[ room_y ][ room_x ] )
        return FALSE;

    if ( room_x + 1 < map_x && big_map[ room_y ][ room_x + 1 ] && room_y != list[ source_room ].y && room_x + 1 != list[ source_room ].x && number_percent() > density )
        return FALSE;
    if ( room_x - 1 >= 0 && big_map[ room_y ][ room_x - 1 ] && room_y != list[ source_room ].y && room_x - 1 != list[ source_room ].x && number_percent() > density )
        return FALSE;
    if ( room_y + 1 < map_y && big_map[ room_y + 1 ][ room_x ] && room_y + 1 != list[ source_room ].y && room_x != list[ source_room ].x && number_percent() > density )
        return FALSE;
    if ( room_y - 1 >= 0 && big_map[ room_y - 1 ][ room_x ] && room_y - 1!= list[ source_room ].y && room_x != list[ source_room ].x && number_percent() > density )
        return FALSE;

    return TRUE;
}


int add_room( int rnd, int density )
{
    int tmp, ex, exits;
    int ex_dir[ 4 ];
    int rooms = 0;

    exits = list[ rnd ].ex;

    if ( list[ rnd ].x + 1 < map_x && big_map[ list[ rnd ].y ][ list[ rnd ].x + 1 ] )
        exits--;
    if ( list[ rnd ].x - 1 >= 0 && big_map[ list[ rnd ].y ][ list[ rnd ].x - 1 ] )
        exits--;
    if ( list[ rnd ].y + 1 < map_y && big_map[ list[ rnd ].y + 1 ][ list[ rnd ].x ] )
        exits--;
    if ( list[ rnd ].y - 1 >= 0 && big_map[ list[ rnd ].y - 1 ][ list[ rnd ].x ] )
        exits--;

    if ( number_percent() < 20 )
        exits = 1;

    if ( exits <= 0 )
        return 0;

    for ( ; exits > 0 ; exits-- )
    {
        for ( tmp = 0;tmp < 4; tmp++ )
            ex_dir[ tmp ] = 0;

        tmp = 0;

        if ( list[ rnd ].x + 1 < map_x && free_room( rnd, density, list[ rnd ].x + 1, list[ rnd ].y ) )
            ex_dir[ tmp++ ] = 1;
        if ( list[ rnd ].x - 1 >= 0 && free_room( rnd, density, list[ rnd ].x - 1, list[ rnd ].y ) )
            ex_dir[ tmp++ ] = 2;
        if ( list[ rnd ].y + 1 < map_y && free_room( rnd, density, list[ rnd ].x, list[ rnd ].y + 1) )
            ex_dir[ tmp++ ] = 3;
        if ( list[ rnd ].y - 1 >= 0 && free_room( rnd, density, list[ rnd ].x, list[ rnd ].y - 1 ) )
            ex_dir[ tmp++ ] = 4;

        if ( tmp < 1 )
            return rooms;

        ex = number_range( 0, tmp - 1 );

        switch ( ex_dir[ ex ] )
        {
            case 1:
                big_map[ list[ rnd ].y ][ list[ rnd ].x + 1 ] = 1;
                rooms++;
                break;
            case 2:
                big_map[ list[ rnd ].y ][ list[ rnd ].x - 1 ] = 1;
                rooms++;
                break;
            case 3:
                big_map[ list[ rnd ].y + 1 ][ list[ rnd ].x ] = 1;
                rooms++;
                break;
            case 4:
                big_map[ list[ rnd ].y - 1 ][ list[ rnd ].x ] = 1;
                rooms++;
                break;
        }
    }
    return rooms;
}

int free_vnum( ush_int vnum )
{
    AREA_DATA * pArea;

    for ( pArea = area_first; pArea; pArea = pArea->next )
        if ( vnum >= pArea->min_vnum && pArea->max_vnum >= vnum )
            return pArea->min_vnum;

    return -1;
}


bool generate_area( CHAR_DATA *ch, CHAR_DATA *victim, int starting_vnum )
{
    int rooms = 0;
    int x, y;
    char buf[MAX_STRING_LENGTH];
    ush_int big_map_vnums[ map_y ][ map_x ];
    ROOM_INDEX_DATA *Room_table[ map_y ][ map_x ];
    int max, min, counter, vnum = MAX_VNUM;
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    int iHash;
    int exit;
    int door, rev_door;


    if ( RANDOM_AREA_ERROR )
        return FALSE;

    /* ile roomow */
    for ( y = 0; y < map_y; y++ )
    {
        for ( x = 0; x < map_x; x++ )
        {
            if ( big_map[ y ][ x ] )
                rooms++;
        }
    }

    for ( y = 0; y < map_y; y++ )
    {
        for ( x = 0; x < map_x; x++ )
        {
            big_map_vnums[ y ][ x ] = 0;
            Room_table[ y ][ x ] = NULL;
        }
    }

    counter = 0;

    if ( free_vnum( starting_vnum ) >= 0 )
    {
        send_to_char( "{RPocz±tkowy vnum jest ju¿ czê¶ci± jakiej¶ krainy.{x\n\r", ch );
        if ( ch != victim )
            send_to_char( "{RPocz±tkowy vnum jest ju¿ czê¶ci± jakiej¶ krainy.{x\n\r", victim );
        return FALSE;
    }

    min = vnum = starting_vnum;

    for ( counter = 0; counter < rooms; counter++, vnum++ )
    {
        if ( free_vnum( vnum ) >= 0 )
        {
            print_char( ch, "{RBrak miejsca by zmie¶ciæ %d lokacji zaczynaj±c od vnuma %d.{x\n\r", rooms, min );
            if ( ch != victim )
                print_char( victim, "{RBrak miejsca by zmie¶ciæ %d lokacji zaczynaj±c od vnuma %d.{x\n\r", rooms, min );
            return FALSE;
        }
    }

    max = vnum;
    vnum = min;

    for ( y = 0; y < map_y; y++ )
    {
        for ( x = 0; x < map_x; x++ )
        {
            if ( !big_map[ y ][ x ] )
                continue;

            big_map_vnums[ y ][ x ] = vnum;
            vnum++;
        }
    }

    print_char( ch, "{GPrzydzielono vnumy :%d-%d (%d lokacji){x\n\r", min, max, rooms );
    if ( ch != victim )
        print_char( victim, "{GPrzydzielono vnumy :%d-%d (%d lokacji){x\n\r", min, max, rooms );

    pArea = new_area();
    area_last->next = pArea;
    area_last = pArea;
    SET_BIT( pArea->area_flags, AREA_RANDOM );
    sprintf( buf, "rnd%d.are", min );
    pArea->file_name = str_dup( buf );
    pArea->min_vnum = min;
    pArea->max_vnum = max;
    pArea->security = 9;
    pArea->builders = str_dup( victim->name );
    pArea->credits = str_dup( "NONE" );
    pArea->name = str_dup( "*Random area*" );

    /* tworzymy roomy */
    for ( y = 0;y < map_y;y++ )
        for ( x = 0;x < map_x;x++ )
        {
            if ( !big_map[ y ][ x ] )
                continue;

            pRoom = new_room_index();
            Room_table[ y ][ x ] = pRoom;
            pRoom->area = pArea;
            vnum = big_map_vnums[ y ][ x ];
            pRoom->vnum = vnum;

            if ( vnum > top_vnum_room )
                top_vnum_room = vnum;

            iHash = vnum % MAX_KEY_HASH;
            pRoom->next = room_index_hash[ iHash ];
            room_index_hash[ iHash ] = pRoom;
            pRoom->name = str_dup( "Random room" );
            pRoom->description = str_dup( "Randomowy opis" );
        }
    /* tworzymy exity */
    for ( y = 0;y < map_y;y++ )
        for ( x = 0;x < map_x;x++ )
        {
            if ( !big_map[ y ][ x ] )
                continue;

            for ( exit = 0; exit < 4; exit++ )
            {
                door = exit;
                rev_door = rev_dir[ door ];

                switch ( exit )
                {
                    case 0:
                        if ( y - 1 >= 0 && big_map[ y - 1 ][ x ] && !Room_table[ y ][ x ] ->exit[ door ] )
                        {
                            Room_table[ y ][ x ] ->exit[ door ] = new_exit();
                            Room_table[ y ][ x ] ->exit[ door ] ->u1.to_room = Room_table[ y - 1 ][ x ];
                            Room_table[ y ][ x ] ->exit[ door ] ->orig_door = door;

                            Room_table[ y - 1 ][ x ] ->exit[ rev_door ] = new_exit();
                            Room_table[ y - 1 ][ x ] ->exit[ rev_door ] ->u1.to_room = Room_table[ y ][ x ];
                            Room_table[ y - 1 ][ x ] ->exit[ rev_door ] ->orig_door = rev_door;
                        }
                        break;
                    case 1:
                        if ( x + 1 < map_x && big_map[ y ][ x + 1 ] && !Room_table[ y ][ x ] ->exit[ door ] )
                        {
                            Room_table[ y ][ x ] ->exit[ door ] = new_exit();
                            Room_table[ y ][ x ] ->exit[ door ] ->u1.to_room = Room_table[ y ][ x + 1 ];
                            Room_table[ y ][ x ] ->exit[ door ] ->orig_door = door;

                            Room_table[ y ][ x + 1 ] ->exit[ rev_door ] = new_exit();
                            Room_table[ y ][ x + 1 ] ->exit[ rev_door ] ->u1.to_room = Room_table[ y ][ x ];
                            Room_table[ y ][ x + 1 ] ->exit[ rev_door ] ->orig_door = rev_door;
                        }

                        break;
                    case 2:
                        if ( y + 1 < map_y && big_map[ y + 1 ][ x ] && !Room_table[ y ][ x ] ->exit[ door ] )
                        {
                            Room_table[ y ][ x ] ->exit[ door ] = new_exit();
                            Room_table[ y ][ x ] ->exit[ door ] ->u1.to_room = Room_table[ y + 1 ][ x ];
                            Room_table[ y ][ x ] ->exit[ door ] ->orig_door = door;

                            Room_table[ y + 1 ][ x ] ->exit[ rev_door ] = new_exit();
                            Room_table[ y + 1 ][ x ] ->exit[ rev_door ] ->u1.to_room = Room_table[ y ][ x ];
                            Room_table[ y + 1 ][ x ] ->exit[ rev_door ] ->orig_door = rev_door;
                        }

                        break;
                    case 3:
                        if ( x - 1 >= 0 && big_map[ y ][ x - 1 ] && !Room_table[ y ][ x ] ->exit[ door ] )
                        {
                            Room_table[ y ][ x ] ->exit[ door ] = new_exit();
                            Room_table[ y ][ x ] ->exit[ door ] ->u1.to_room = Room_table[ y ][ x - 1 ];
                            Room_table[ y ][ x ] ->exit[ door ] ->orig_door = door;

                            Room_table[ y ][ x - 1 ] ->exit[ rev_door ] = new_exit();
                            Room_table[ y ][ x - 1 ] ->exit[ rev_door ] ->u1.to_room = Room_table[ y ][ x ];
                            Room_table[ y ][ x - 1 ] ->exit[ rev_door ] ->orig_door = rev_door;
                        }

                        break;
                    default:
                        bug( "Wrong exit in generate_area", 0 );
                        return FALSE;
                }
            }
        }

    return TRUE;
}

void do_rmap( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim = ch;
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
    char arg3[MAX_STRING_LENGTH];
    char arg4[MAX_STRING_LENGTH];
    int x, y, rooms, density;

    argument = one_argument( argument, arg1 );

    if ( !str_prefix( arg1, "create" ) )
    {
        if ( max_wizgroup( ch ) < W4 )
        {
            send_to_char( "Aby stworzyæ krainê na podstawie swojej mapy popro¶ Stra¿nika, Sêdziego lub Lorda.\n\r", ch );
            return;
        }

        argument = one_argument( argument, arg1 );

        if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
        {
            send_to_char( "Nie ma takiego gracza ani moba.\n\r", ch );
            return ;
        }

        if ( IS_NULLSTR( victim->desc->custom_map ) )
        {
            if ( ch == victim )
                send_to_char( "Nie masz zdefiniowanej ¿adnej mapy. U¿yj komendy 'rmap generate' lub 'rmap edit'.\n\r", ch );
            else
            {
                send_to_char( "Nie masz zdefiniowanej ¿adnej mapy. U¿yj komendy 'rmap generate' lub 'rmap edit'.\n\r", victim );
                print_char( ch, "%s nie ma zdefiniowanej ¿adnej mapy.\n\r", victim->name );
            }

            return;
        }

        one_argument( argument, arg2 );

        if ( !is_number( arg2 ) )
        {
            print_char( ch, "Podaj pocz±tkowy vnum krainy z zakresu 1-%d.\n\r", MAX_VNUM );
            return;
        }

        string_to_map( victim );
        show_map( ch );
        if ( ch != victim )
            show_map( victim );
        generate_area( ch, victim, atoi( arg2 ) );
    }
    else if ( !str_prefix( arg1, "show" ) )
    {
        argument = one_argument( argument, arg1 );

        if ( !IS_NULLSTR( arg1 ) )
        {
            if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
            {
                send_to_char( "Nie ma takiego gracza ani moba.\n\r", ch );
                return ;
            }
        }

        if ( IS_NULLSTR( victim->desc->custom_map ) )
        {
            if ( ch == victim )
                send_to_char( "Nie masz zdefiniowanej ¿adnej mapy. U¿yj komendy 'rmap generate' lub 'rmap edit'.\n\r", ch );
            else
                print_char( ch, "%s nie ma zdefiniowanej ¿adnej mapy.\n\r", victim->name );
            return;
        }

        string_to_map( victim );
        show_map( victim );
    }
    else if ( !str_prefix( arg1, "generate" ) )
    {
        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );
        argument = one_argument( argument, arg3 );
        argument = one_argument( argument, arg4 );

        if ( !is_number( arg1 ) || atoi( arg1 ) < 1 || atoi( arg1 ) > MAP_SIZE
          || !is_number( arg2 ) || atoi( arg2 ) < 1 || atoi( arg2 ) > MAP_SIZE
          || ( arg3[0] != '\0' && ( !is_number( arg3 ) || atoi( arg3 ) < 1 || atoi( arg3 ) >= atoi( arg1 )*atoi( arg2 ) ) )
          || ( arg4[0] != '\0' && ( !is_number( arg4 ) || atoi( arg4 ) < 1 || atoi( arg4 ) > 100 ) ) )
        {
            print_char( ch, "Sk³adnia:\n\r"
                            "rmap generate <x> <y> [<lokacje>] [<gêsto¶æ>]\n\r"
                            "x - szeroko¶æ krainy (od 1 do %d)\n\r"
                            "y - d³ugo¶æ krainy (od 1 do %d)\n\r"
                            "lokacje - liczba lokacji w krainie (od 1 do (x*y)-1)\n\r"
                            "gêsto¶æ - im wiêksza tym gê¶ciej upakowane lokacje (od 1 do 100)\n\r", MAP_SIZE, MAP_SIZE );
            return;

        }

        x = atoi( arg1 );
        y = atoi( arg2 );

        if ( arg3[0] == '\0' )
            rooms = x * y / 2;
        else
            rooms = atoi( arg3 );

        if ( arg4[0] == '\0' )
            density = 50;
        else
            density = atoi( arg4 );

        generate_map( ch, x, y, rooms, density );
    }
    else if ( !str_prefix( arg1, "edit" ) )
    {
        ch->desc->editor = ED_MAP;
        string_append( ch, &ch->desc->custom_map );
    }
    else
    {
        send_to_char( "Sk³adnia:\n\r"
                      "rmap generate <x> <y> [<lokacje>] [<gêsto¶æ>] - wygenerowanie losowej mapy\n\r"
                      "rmap edit - edycja wygenerowanej mapy\n\r"
                      "rmap show [<imiê>] - wy¶wietlenie aktualnej mapy\n\r"
                      "rmap create [<imiê>] <vnum startowy> - stworzenie krainy na podstawie aktualnej mapy\n\r", ch );
    }


    return;
}
