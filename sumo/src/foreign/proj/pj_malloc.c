/* allocate and deallocate memory */
#ifndef lint
static const char SCCSID[]="@(#)pj_malloc.c	4.3   93/06/12 GIE REL";
#endif
/* These routines are used so that applications can readily replace
** projection system memory allocation/deallocation call with custom
** application procedures.  */
#include <projects.h>
	void *
pj_malloc(size_t size) {
	return(malloc(size));
}
	void
pj_dalloc(void *ptr) {
	free(ptr);
}
