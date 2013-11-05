#ifndef __RANDOM_ITEMS_H
#define __RANDOM_ITEMS_H
/* opis kawałka randoma, stringi opisujące + kawałki bonusa */
struct random_item_part 
{
  char *description[7];
  void (*hook_function)(void);
};
typedef struct random_item_part RANDOM_ITEM_PART;

#endif

