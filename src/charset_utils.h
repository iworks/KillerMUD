#ifndef __CHRASET_UTILS_H
#define __CHRASET_UTILS_H

#include <iconv.h>
void init_iconv(void);

extern iconv_t to_pol; 
extern iconv_t to_iso; 
extern iconv_t to_win; 
extern iconv_t to_utf8;

#endif
