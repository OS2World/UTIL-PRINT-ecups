#ifndef CALLCUPS_INCLUDED
#define CALLCUPS_INCLUDED

#include <ecups.h>

typedef struct cups_option_s		/**** Printer Options ****/
{
  char		*name;			/* Name of option */
  char		*value;			/* Value of option */
} cups_option_t;

ULONG CallCups(PECUPSSESSIONDATA peCUPSSession);

APIRET (* _cdecl pfncupsSetServer)(const char *server);
int (* _cdecl pfncupsPrintFile)(const char *printer, const char *filename,const char *title, int num_options,cups_option_t *options);
const char * (* _cdecl pfncupsUser)(void);

#endif
