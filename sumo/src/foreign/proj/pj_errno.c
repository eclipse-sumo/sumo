/* For full ANSI compliance of global variable */
#ifndef lint
static const char SCCSID[]="@(#)pj_errno.c	4.3	95/06/03	GIE	REL";
#endif

#include <projects.h>

C_NAMESPACE int pj_errno = 0;

/************************************************************************/
/*                          pj_get_errno_ref()                          */
/************************************************************************/

int *pj_get_errno_ref()

{
    return &pj_errno;
}

/* end */
