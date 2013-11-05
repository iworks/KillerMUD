#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "generic_list.h"

void gl_selftest(void)
{
  /* testy dla listy intów */
  {
    genlist * g;
    g = gl_empty();
    int arr[] = { 1, 2, 3, 4 };
    gl_push_front(&g, &arr[0]);
    gl_push_front(&g, &arr[1]);
    gl_push_front(&g, &arr[2]);
    gl_push_front(&g, &arr[3]);
    
    FLOW_CONTROL iteratee(void * ptr)
    {
      int * num = ptr;
      printf("%d\n", *num);
      return CONTINUE;
    }
  
    gl_iterate_list(g, iteratee);

    int is_equal_to_two(void * ptr)
    {
      int * num = ptr;
      return (*num == 2);
    }
  
    int is_equal_to_five(void * ptr)
    {
      int * num = ptr;
      return (*num == 5);
    }
  
    {
      int * ptr = gl_find( g, is_equal_to_two );
      if ( ptr )
        {
          printf("Found 2: %d\n", *ptr);
        }
    }

    {
      int * ptr =   gl_find( g, is_equal_to_five );
      if ( ptr )
        {
          printf("Found 5, but shouldn't! %d\n", *ptr);
        }
      else
        {
          printf("NULL returned - not '5' in list - OK\n");
        }
    }
  }

  /* testy dla listy stringów */
  {
    genlist * g;
    g = gl_empty();
    gl_push_front(&g, "foo");
    gl_push_front(&g, "bar");
    gl_push_front(&g, "baz");
    gl_push_front(&g, "xyz");
    gl_push_front(&g, "var");

    FLOW_CONTROL iteratee(void * ptr)
    {
      char * str = ptr; 
      printf("%s\n", str);
      return CONTINUE;
    }
    gl_iterate_list(g, iteratee);
  }
}

genlist * gl_empty(void)
{
  return NULL;
}

/* dodaje element na początku listy */
void gl_push_front(genlist ** gl, void * payload)
{
  genlist * head = malloc(sizeof(genlist));
  head->payload = payload;
  head->next = (*gl);
  (*gl) = head;
}

/* znajduje PIERWSZY element na liście, który spełnia predykat */
void * gl_find(genlist * gl, int (*pred)(void*) )
{
  void * retval = NULL;
  
  FLOW_CONTROL fun(void * payload)
  {
    if ((*pred)(payload))
      {
        retval = payload;
        return STOP;
      }
    return CONTINUE;
  };

  gl_iterate_list(gl, fun);

  return retval;
}

/* iteruje według instrumentacji funkcji */
void gl_iterate_list(genlist * gl, FLOW_CONTROL (*iteratee)(void*))
{
  genlist * gl_ptr;
  for(gl_ptr = gl; gl_ptr; gl_ptr = gl_ptr->next)
    {
      FLOW_CONTROL fc = (*iteratee)(gl_ptr->payload);
      if (fc == STOP)
        return;
    }
}

/* 
   tworzy NOWĄ listę, na podstawie starej 
   
   stara nie jest dealokowana
*/
genlist * gl_filter(genlist * gl, FLOW_CONTROL (*filterfun)(void*))
{
  genlist * gl_ptr;
  genlist * retlist = gl_empty();
  for(gl_ptr = gl; gl_ptr; gl_ptr = gl_ptr->next)
    {
      FLOW_CONTROL fc = (*filterfun)(gl_ptr->payload);
      
      /* skip this item, doesn't include it in result*/
      if (fc == SKIP)
        continue;
      
      /* stop iteration */
      if (fc == STOP)
        break;
      
      /* add item to the list */
      gl_push_front(&retlist,gl_ptr->payload);
    }

  gl_reverse( retlist );
  
  return retlist;
}

/* odwraca listę w miejscu */
void * gl_reverse(genlist * gl)
{
  /*
           GL1 -> GL2 -> foo
    foo <- GL1 <- GL2

    GL2 -> GL1 -> foo
   */
  void swap(genlist * gl1, genlist * gl2)
  {
    genlist * saveptr = gl2->next;
    gl2->next = gl1;
    gl1->next = saveptr;
  }
  
}
