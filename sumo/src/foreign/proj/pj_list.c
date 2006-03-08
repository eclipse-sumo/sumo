#ifndef lint
static const char SCCSID[]="@(#)pj_list.c	4.6	94/02/15	GIE	REL";
#endif
/* Projection System: default list of projections
** Use local definition of PJ_LIST_H for subset.
*/
#define PJ_LIST_H "pj_list.h"
#include "projects.h"

struct PJ_LIST  *pj_get_list_ref()

{
    return pj_list;
}
