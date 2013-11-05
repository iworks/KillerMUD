#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "merc.h"

#include <iconv.h>
#include "charset_utils.h"

static const char MUD_CHARSET[] = "ISO-8859-2";

// polskawy
iconv_t to_pol;
iconv_t to_iso;
iconv_t to_win;
iconv_t to_utf8;

/* 
   funkcja tłumaczy p_buffer według konwersji określonej przez parametr p_iconv.
   działamy według zasady "wszystko albo nic": albo konwersja jest pełna i 
   zawartość p_buffer zostaje podmieniona na nową (funkcja zwraca TRUE), 
   albo nie udało się i zwracamy FALSE.

   buffer_len powinno być na faktyczną długością bufora, funkcja zadba o miejsce dla '\0'.
*/

int translate_buffer(iconv_t p_iconv,
                     char * p_buffer,
                     size_t p_buffer_len)
{
  static char * buf = NULL;
  static size_t buf_len = 0;

  int copy_cnt;

  if (!buf)
    {
      buf_len = 2 * 1024 * 1024; // 2Mb - powinno wystarczyć, heh.
      buf = malloc(sizeof(char) * buf_len);
    }
  
  if ( p_buffer_len > buf_len-1 )
    {
      logformat ( "%s, %s, %d: buffer too long, sorry.", __FILE__, __func__, __LINE__ );  
      return FALSE;
    }

  /* translacja */
  {  
    // kopiujemy wskaźniki, bo iconv() nam je przesuwa.
    char * buf_in = p_buffer; 
    char * buf_out = buf;
  
    // strlen: to może być niebezpieczne, jeżeli w stringu będzie jakiś null byte. w efekcie stracimy kawałek.
    size_t buf_in_left = strlen(p_buffer); 
    size_t buf_out_left = buf_len-1;
  
    if ( -1 == iconv( p_iconv,
                      &buf_in, &buf_in_left,
                      &buf_out, &buf_out_left ) )
      { // jakiś błąd
        logformat ( "%s, %s, %d: iconv returned -1. errno=%s.", __FILE__, __func__, __LINE__, strerror(errno) );  
        return FALSE;
      }

    // wyliczamy ile przeszliśmy
    copy_cnt = buf_out - buf;
    // robimy końcówkę
    buf[copy_cnt] = '\0';
  }

  /* kopiowanie wyniku */
  {  
    memcpy(p_buffer, buf, copy_cnt+1); // copy_cnt+1, bo kopiujemy też '\0'.
  }

  return TRUE;
}

void init_iconv(void)
{
  to_pol = iconv_open("ASCII//TRANSLIT",MUD_CHARSET);
  to_iso = iconv_open("ISO-8859-2//TRANSLIT",MUD_CHARSET);
  to_win = iconv_open("CP-1250//TRANSLIT",MUD_CHARSET);
  to_utf8 = iconv_open("UTF-8//TRANSLIT",MUD_CHARSET);
}
