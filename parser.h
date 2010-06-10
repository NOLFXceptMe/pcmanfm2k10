#ifndef _PARSER_H_
#define _PARSER_H_

#include<glib.h>

int parse(gchar *fileName);

#define UNKNOWN_ENTRY	1
#define ACTION_ENTRY	1<<1
#define PROFILE_ENTRY	1<<2
#define MENU_ENTRY		1<<3

#endif
