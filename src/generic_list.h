#ifndef __GENERIC_LIST_H
#define __GENERIC_LIST_H



typedef enum {
  STOP,
  CONTINUE,
  SKIP
} FLOW_CONTROL;


/*

  Pusta lista to NULL ptr.

 */
/* wewnętrzna struktura genlist może się zmienić - aby dostać się do elementów używaj API - ono zmieniać się będzie znacznie mniej */ 
typedef struct genlist
{
  void * payload;
  struct genlist * next;
} genlist;

/* funkcje */ 
void gl_iterate_list(genlist * gl, FLOW_CONTROL (*iteratee)(void*));
void * gl_find(genlist * gl, int (*pred)(void*) );
void gl_push_front(genlist ** gl, void * payload);
genlist * gl_empty(void);

/* tych funkcji jeszcze nie ma, ale będą */

void * gl_at_nth( genlist * gl, int pos );
void * gl_pop_nth( genlist * gl, int pos );
void gl_insert_at( genlist * gl, int pos, void * payload );
void * gl_reverse(genlist * gl);

/* selftest */
void gl_selftest(void);

#endif
